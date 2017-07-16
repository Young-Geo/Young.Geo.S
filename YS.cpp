#include "YS.h"

global_t master_thread;
struct event_base *master_main_base;


int sigignore(int sig)
{
    struct sigaction sa = { .sa_handler = SIG_IGN, .sa_flags = 0 };

    if (sigemptyset(&sa.sa_mask) == -1 || sigaction(sig, &sa, 0) == -1) {
        return -1;
    }
    return 0;
}

int YS_INIT(global_t *master)
{
	assert(master);
	memset(master, 0, sizeof(global_t))
	master->num_threads = WORK_THREAD;
	return 0;
}
void thread_libevent_process(int fd, short which, void *arg)
{
	char type = '0', cfd = 0;
	// rec conn
	assert(arg);
	thread_entity_t *thread_entity = (thread_entity_t *)arg;
	if (1 != read(fd, &type, 1)) {
		fprintf(stderr, "Can't read conn type \n");
		return;
	}

	switch (type)
	{
		case 'C':
			{
				struct bufferevent *bev = NULL;
				if ((4 != read(fd, &cfd, sizeof(cfd))) || cfd < 0) {
					fprintf(stderr, "Can't read conn fd \n");
					return;
				}
				//加入到 base中
				bev = bufferevent_socket_new(thread_entity->base, cfd, BEV_OPT_CLOSE_ON_FREE);

			    //当前刚创建好的bufferevent事件 注册一些回调函数
			    bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);

			    //启动监听bufferevnet的 读事件 和 写事件
			    bufferevent_enable(bev, EV_READ|EV_WRITE);

				++thread_entity->conn_num;//添加链接数
			}
		break;


		default:
		break;
	}
}
int setup_thread(thread_entity_t *thread_entity)
{
	int ret = 0;
	thread_entity->base = event_init();


	 if (!thread_entity->base) {
        fprintf(stderr, "Can't allocate event base\n");
        exit(1);
    }

    /* Listen for notifications from other threads */
    event_set(&thread_entity->notify_event, thread_entity->notify_receive_fd,
              EV_READ | EV_PERSIST, thread_libevent_process, thread_entity);
    event_base_set(thread_entity->base, &thread_entity->notify_event);

    if (event_add(&thread_entity->notify_event, 0) == -1) {
        fprintf(stderr, "Can't monitor libevent notify pipe\n");
        exit(1);
    }
	
	return 0;
}

int create_worker(void* (*func)(void *), void *arg)
{
    int             ret;	
	thread_entity_t *thread_entitys;

	assert(arg);
	thread_entity_t *thread_entitys = (thread_entity_t *)arg;

	
    if ((ret = pthread_create(&thread_entitys->thread_id, NULL, func, arg)) != 0) {
        fprintf(stderr, "Can't create thread: %s\n",
                strerror(ret));
        exit(1);
    }
	return 0;
}


void *worker_libevent(void *arg)
{
	assert(arg);
	thread_entity_t *thread_entitys = (thread_entity_t *)arg;
    event_base_loop(thread_entitys->base, 0);
	return NULL;
}



int YS_thread_init(global_t *master)
{
	int ret = 0, nthreads = 0, i, pfd[2];	

	assert(master);
	
	nthreads = master->num_threads;
	

	for (i = 0; i < nthreads; i++)
	{
		master->threads = (pthread_t *)cmalloc(nthreads, sizeof(pthread_t));
		assert(master->threads);

		master->thread_entitys = (thread_entity_t *)cmalloc(nthreads, sizeof(thread_entity_t));
		assert(master->thread_entitys);
		
        if (pipe(pfd)) {
            perror("Can't create notify pipe");
            exit(1);
        }
		
        master->thread_entitys[i].notify_receive_fd = pfd[0];
        master->thread_entitys[i].notify_send_fd = pfd[1];

        setup_thread(&master->thread_entitys[i]);
    }

    /* Create threads after we've done all the libevent setup. */
	for(i = 0; i < nthreads; i++)
	{
    	create_worker(worker_libevent, &master->thread_entitys[i]);
    }

	
	return 0;
}

void master_libevent_work(int fd, short which, void *arg)
{
	int cfd = 0, i = 0, num = MAX_INT, inx = 0;
	struct sockaddr_in caddr;
	socklen_t slen;
	// accept
	assert(arg);
	slen = sizeof(caddr);
	global_t *master = (global_t *)arg;

	cfd = accept(master->conn_receive_fd, (struct sockaddr *)&caddr, &slen);
	if (cfd < 0) {
		fprintf(stderr, "master_work socket error %d\n", lfd);
		return;
	}
	
	// 计算出接受任务的线程
	for (i = 0; i < master->num_threads; ++i)
	{
		if (num > master->thread_entitys[i].conn_num) {
			num = master->thread_entitys[i].conn_num;
			inx = i;
		}
	}

	//write 'c'
	if (1 != write(master->thread_entitys[inx].notify_send_fd, 'C', 1)) {
		close(cfd);
		return;
	}
	// 把文件描述符发送给相应的线程
	if (sizeof(cfd) != write(master->thread_entitys[inx].notify_send_fd, &cfd, sizeof(cfd))) {
		close(cfd);
		return;
	}
}


static void *master_work(void *arg)
{
	int lfd;
	struct sockaddr_in addr;
	socklen_t slen;

	//socket
	assert(arg);
	global_t *master = (global_t *)arg;


	if((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "master_work socket error %d\n", lfd);
		exit(1);
	}

	master->conn_receive_fd = lfd;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(SER_ADDR);
	addr.sin_port = htons(SER_PORT);
	slen = sizeof(addr);
	if (0 != bind(lfd, (struct sockaddr *)&addr, slen)) {
		fprintf(stderr, "master_work bind error %d\n", lfd);
		exit(1);
	}
	if (0 != listen(lfd, MAX_LISTEN)) {
		fprintf(stderr, "master_work listen error %d\n", lfd);
		exit(1);
	}


	master->master_main_base = master_main_base = event_init();


	 if (!master_main_base) {
        fprintf(stderr, "Can't allocate event base\n");
        exit(1);
    }

    /* Listen for notifications from other threads */
    event_set(&master->master_main_notify_event, master->conn_receive_fd,
              EV_READ | EV_PERSIST, master_libevent_work, arg);
    event_base_set(master->master_main_base, &master->master_main_notify_event);

    if (event_add(&master->master_main_notify_event, 0) == -1) {
        fprintf(stderr, "Can'tevent_add\n");
        exit(1);
    }
	


	
    event_base_loop(master->master_main_base, 0);
	return NULL;
}


int YS_master_thread_init(global_t *master)
{
	int ret = 0;
	if ((ret = pthread_create(&master->master_thread_id, NULL, master_work, master)) != 0) {
        fprintf(stderr, "Can't create thread: %s\n",
                strerror(ret));
        exit(1);
    }
	return 0;
}

void YS_master_thread_loop(global_t *master)
{	
	pthread_exit(NULL);// 继续封装
}


int main(int argc, const char **argv)
{
	int ret = EXIT_SUCCESS;
	
	sigignore(SIGPIPE);
	YS_INIT(&master_thread);

	
	main_base = event_init();
	assert(main_base);

	YS_thread_init(&master_thread);
	YS_master_thread_init(&master_thread);

	YS_master_thread_loop(&master_thread);
	//pthread_exit(NULL);// 继续封装


	#if 0
	 if (event_base_loop(main_base, 0) != 0) {
	 	ret = EXIT_FAILURE;
	}
	 #endif
	
	return 0;
}
