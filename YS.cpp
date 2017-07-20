#include "YS.h"

global_t master_thread;
struct event_base *master_main_base;


int sigignore(int sig)
{
    struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN, sa.sa_flags = 0;
    if (sigemptyset(&sa.sa_mask) == -1 || sigaction(sig, &sa, 0) == -1) {
        return -1;
    }
    return 0;
}

int YS_INIT(global_t *master)
{
	assert(master);
	memset(master, 0, sizeof(global_t));
	master->num_threads = WORK_THREAD;
	return 0;
}

//读取玩数据之后执行
void read_cb(struct bufferevent *bev, void *arg)
{
	assert(arg);
	thread_entity_t *thread_entity = (thread_entity_t *)arg;

	char *data = NULL;
	int len = 0, i = 0;

	    /* This callback is invoked when there is data to read on bev. */
    struct evbuffer *input = bufferevent_get_input(bev);
    //input 就是当前bufferevent的输入缓冲区地址，如果想得到用户端数据
    //就从input中去获取

    struct evbuffer *output = bufferevent_get_output(bev);
    //output 就是当前bufferevent的输出缓冲区地址，如果想向客户端写数据
    //就将数据写到output中就可以了

	len =  evbuffer_get_length(input);
    //input 拿出来， 
    data = (char *)malloc(len + 1);

	evbuffer_copyout(input, (void *)data, len);

    //小->大
    for (i = 0; i < len; ++i)
		data[i] = toupper(data[i]);



    /* Copy all the data from the input buffer to the output buffer. */
    evbuffer_add_buffer(output, input);
	evbuffer_add(output, (void *)data, len);
	free(data);
}


//写完之后执行
void write_cb(struct bufferevent *bev, void *arg)
{
	assert(arg);
	thread_entity_t *thread_entity = (thread_entity_t *)arg;
}


//异常处理
void event_cb(struct bufferevent *bev, short what, void *arg)
{
	assert(arg);
	thread_entity_t *thread_entity = (thread_entity_t *)arg;

	if (what & BEV_EVENT_ERROR)
        perror("Error from bufferevent");
    if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        bufferevent_free(bev);
    }
}

void thread_libevent_process(int fd, short which, void *arg)
{
	char buf[512], type = '0', cfd = 0, i = 0;
	// rec conn
	assert(arg);
	thread_entity_t *thread_entity = (thread_entity_t *)arg;

	pthread_mutex_lock(&thread_entity->mutex_rec);
	if (read(fd, buf, sizeof(buf)) < 0) {
		pthread_mutex_unlock(&thread_entity->mutex_sen);		
		pthread_mutex_unlock(&thread_entity->mutex_rec);
		fprintf(stderr, "Can't read conn type \n");
		return;
	}
	pthread_mutex_unlock(&thread_entity->mutex_sen);

	type = buf[i];
	++i;

	switch (type)
	{
		case 'C':
			{
				struct bufferevent *bev = NULL;
				/*
				if ((read(fd, &cfd, sizeof(cfd)) < 0 ) || cfd < 0) {
					fprintf(stderr, "Can't read conn fd \n");
					return;
				}
				*/
				sscanf(&buf[i], ":%d", &cfd);
				if (cfd < 0) {
					fprintf(stderr, "sscanf Can't read conn fd \n");
					return;
				}
				//加入到 base中
				bev = bufferevent_socket_new(thread_entity->base, cfd, BEV_OPT_CLOSE_ON_FREE);

			    //当前刚创建好的bufferevent事件 注册一些回调函数
			    bufferevent_setcb(bev, read_cb, write_cb, event_cb, arg);

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

typedef void * (*func_t)(void *arg);
int create_worker(func_t func, void *arg)
{
    int             ret;	
	thread_entity_t *thread_entitys;

	assert(arg);
	thread_entitys = (thread_entity_t *)arg;

	
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

	master->threads = (pthread_t *)calloc(nthreads, sizeof(pthread_t));
	assert(master->threads);

	master->thread_entitys = (thread_entity_t *)calloc(nthreads, sizeof(thread_entity_t));
	assert(master->thread_entitys);
		

	for (i = 0; i < nthreads; i++)
	{
        if (pipe(pfd)) {
            perror("Can't create notify pipe");
            exit(1);
        }
		
        master->thread_entitys[i].notify_receive_fd = pfd[0];
        master->thread_entitys[i].notify_send_fd = pfd[1];

        setup_thread(&master->thread_entitys[i]);
		
  		pthread_mutex_init(&master->thread_entitys[i].mutex_rec, NULL);
  		pthread_mutex_init(&master->thread_entitys[i].mutex_sen, NULL);
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
	char buf[512];
	struct sockaddr_in caddr;
	socklen_t slen;
	// accept
	assert(arg);
	slen = sizeof(caddr);
	global_t *master = (global_t *)arg;

	cfd = accept(master->conn_receive_fd, (struct sockaddr *)&caddr, &slen);
	if (cfd < 0) {
		fprintf(stderr, "master_libevent_work socket error %d\n", cfd );
		return;
	}

	xmessage("conn num %d, %d, %d, %d,\n", master->thread_entitys[0].conn_num, 
										master->thread_entitys[1].conn_num,
										master->thread_entitys[2].conn_num,
										master->thread_entitys[3].conn_num
										);
	// 计算出接受任务的线程
	for (i = 0; i < master->num_threads; ++i)
	{
		if (num > master->thread_entitys[i].conn_num) {
			num = master->thread_entitys[i].conn_num;
			inx = i;
		}
	}

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "C:%d", cfd);

	pthread_mutex_lock(&master->thread_entitys[inx].mutex_sen);
	if (strlen(buf) != write(master->thread_entitys[inx].notify_send_fd, buf, strlen(buf))) {
		pthread_mutex_unlock(&master->thread_entitys[inx].mutex_sen);		
		pthread_mutex_unlock(&master->thread_entitys[inx].mutex_rec);
		xerror("send cfd error\n");
		close(cfd);
		return;
	}
	pthread_mutex_unlock(&master->thread_entitys[inx].mutex_rec);
	/*
	//write 'c'
	if (1 != write(master->thread_entitys[inx].notify_send_fd, "C", strlen("C"))) {
		close(cfd);
		return;
	}
	// 把文件描述符发送给相应的线程
	if (sizeof(cfd) != write(master->thread_entitys[inx].notify_send_fd, &cfd, sizeof(cfd))) {
		close(cfd);
		return;
	}
	*/
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

