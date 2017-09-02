#include "YGame_Server.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "YUser.h"
#include "Ypkt.h"
#include "YGame.h"




#define MAX_EVENTS  1024
#define BUFLEN 1024
#define TRUE 1

typedef struct _BUF_T {
	unsigned char *buf;
	int len;
} Buf_t;


/* * status:1表示在监听事件中，0表示不在  * last_active:记录最后一次响应时间,做超时处理 */
struct myevent_s {
    int fd;
    int events;
    void *arg;
    void (*call_back)(int fd, int events, void *arg);
    int status;
//    unsigned char buf[BUFLEN];
//    int len;
	xlist *bufs;
    long last_active;
	User *user;
};


int g_efd;
/* epoll_create返回的句柄 */
struct myevent_s g_events[MAX_EVENTS+1];
/* +1 最后一个用于 listen fd */

void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg)
{
	xassert(ev);
	xassert(call_back);
	xassert(arg);
	//xassert(user);
	
    ev->fd = fd;
	ev->call_back = call_back;
    ev->events = 0;
    ev->arg = arg;
	ev->user = NULL;
    ev->status = 0;
	if (!ev->bufs) {
		ev->bufs = xlist_init();
	}
    //memset(ev->buf, 0, sizeof(ev->buf));
    //ev->len = 0;
    ev->last_active = time(NULL);
    return;
}

void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);

void eventadd(int efd, int events, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}};
    int op;
    epv.data.ptr = ev;
    epv.events = ev->events = events;

    if (ev->status == 1) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }


    if (epoll_ctl(efd, op, ev->fd, &epv) < 0)
        xerror("event add failed [fd=%d], events[%d]\n", ev->fd, events);
    else
        xmessage("event add OK [fd=%d], op=%d, events[%0X]\n", ev->fd, op, events);  
    return;
}


void eventdel(int efd, struct myevent_s *ev)
{
    struct epoll_event epv = {0,  { 0 }};
    if (ev->status != 1)
        return;
    epv.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
}

void acceptconn(int lfd, int events, void *arg)
{
    struct sockaddr_in cin;
    socklen_t len = sizeof(cin);
    int cfd, i;
	int flag = 0;

    if ((cfd = accept(lfd, (struct sockaddr *)&cin, &len)) == -1) {

        if (errno != EAGAIN && errno != EINTR) { 
            /* 暂时不做出错处理 */
			return;
        }

        xerror("%s: accept, %s\n", __func__, strerror(errno));
        return;
    }


    do {

        for (i = 0; i < MAX_EVENTS; i++)
        {
            if (g_events[i].status == 0)
                break;
        }


        if (i == MAX_EVENTS) {

            xmessage("%s: max connect limit[%d]\n", __func__, MAX_EVENTS);
            break;

        }

        flag = 0;
        if ((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0) {
            xmessage("%s: fcntl nonblocking failed, %s\n", __func__, strerror(errno));
            break;
        }

        eventset(&g_events[i], cfd, recvdata, &g_events[i]);
        eventadd(g_efd, EPOLLIN, &g_events[i]);

    } while(0);

    xmessage("new connect [%s:%d][time:%ld], pos[%d]\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i); 

	
}


void recvdata(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;
	unsigned char buf[BUFLEN] = { 0 };
	Buf_t *buft = NULL;

	
    len = recv(fd, buf, sizeof(buf), 0);
    eventdel(g_efd, ev);

    if (len > 0) {
		//处理数据
        buf[len] = '\0';
		buft = (Buf_t *)xmalloc(sizeof(Buf_t));
		xassert(buft);
        xmessage("C[%d]\n", fd);
		pkt_parse_data(buf, len, &buft->buf, &buft->len);
		xlist_add(ev->bufs, NULL, XLIST_PTR, (char *)buft);
		//解析数据
        /* 转换为发送事件 */
        //eventset(ev, fd, senddata, ev);
        //eventadd(g_efd, EPOLLOUT, ev);
    }
    else if (len == 0) {
        close(ev->fd);
        /* ev-g_events 地址相减得到偏移元素位置 */
        xmessage("[fd=%d] pos[%d], closed\n", fd, (int)(ev - g_events));
		//eventdel(g_efd, &g_events[checkpos]);
    }	else {
        close(ev->fd);
        xerror("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }

    return;
}




void senddata(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;
    //处理业务，小写转大写    len = send(fd, ev->buf, ev->len, 0);
    /*
       printf("fd=%d\tev->buf=%s\ttev->len=%d\n", fd, ev->buf, ev->len);
       printf("send len = %d\n", len);
     */
     //遍历加协议
    if (len > 0) {

        eventdel(g_efd, ev);
        eventset(ev, fd, recvdata, ev);
        eventadd(g_efd, EPOLLIN, ev);

    } else {

        close(ev->fd);
        eventdel(g_efd, ev);
        printf("send[fd=%d] error %s\n", fd, strerror(errno));

    }

    return;
}



void initlistensocket(int efd, short port)
{
    int lfd = 0;
    struct sockaddr_in sin;
	
    
	if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		xerror("socket lfd error");
		return;
	}
	
    fcntl(lfd, F_SETFL, O_NONBLOCK);
	
    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);
    eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);
	
	xmemset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;	
    //sin.sin_addr.s_addr = INADDR_ANY;
	inet_pton(AF_INET, GAME_SER_IP, (void *)(&sin.sin_addr.s_addr));
    sin.sin_port = htons(port);	

	bind(lfd, (struct sockaddr *)&sin, sizeof(sin));

	listen(lfd, 128);
	
    return;
}

int		parse_readys(global_t *master)
{
	User *users[GAME_USER_COUNT];
	int i = 0;
	xlist *u_node = NULL;
	Game *game = NULL;
	
	xassert(master);

	while (master->ready_num > GAME_USER_COUNT)
	{
		for (i = 0; i < GAME_USER_COUNT; ++i)
		{
			u_node = xlist_index(master->readys, i);
			if (!u_node || !u_node->value) {
				xerror("xlist_index error");
			}
			users[i] = (User *)u_node->value;
		}
		
		pthread_mutex_lock(&master->mutex_ready);
		for (i = 0; i < GAME_USER_COUNT; ++i)
		{
			xlist_delete(master->readys, (char *)users[i]->get_username());
			--master->ready_num;
		}
		pthread_mutex_unlock(&master->mutex_ready);

		game = new Game(users[0], users[1], users[2]);
		if (!game) {
			xerror("new Game error");
			continue;
		}
		game->display();
		xlist_add(master->games, (char *)game->get_name(), XLIST_PTR, (char *)game);
		
	}
}

int work(struct myevent_s *ev)
{
	int i = 0, size = 0;
	xlist *bufs = NULL;
	Buf_t *buft = NULL;
	
	xassert(ev);
	
	bufs = ev->bufs;
	size = xlist_size(ev->bufs);
	
	while (size-- > 0)
	{
		//解析数据处理相应业务
		buft = (Buf_t *)xlist_popv(bufs);
		if (!buft)
			continue;
		//处理业务
	}

	return 0;
}

static void *game_work(void *arg)
{
	global_t *master = NULL;
	unsigned short port = GAME_SER_PORT;
    struct epoll_event events[MAX_EVENTS+1];
    int checkpos = 0, i, nfd;
	long now = 0, duration = 0;
	struct myevent_s *ev = NULL;

	master = (global_t *)arg;
	xassert(master);
	
    g_efd = master->game_efd = epoll_create(MAX_EVENTS+1);
    if (g_efd <= 0) 
        xerror("create efd in %s err %s\n", __func__, strerror(errno));
	
    initlistensocket(master->game_efd, port);

	
    /* 事件循环 */ 
    xmessage("server running:port[%d]\n", port);

	
    while (TRUE)
    {
        /* 超时验证，每次测试100个链接，不测试listenfd 当客户端60秒内没有和服务器通信，则关闭此客户端链接 */
		
		parse_readys(master);

		now = time(NULL);
        for (i = 0; i < 100; i++, checkpos++)
        {
        
            if (checkpos == MAX_EVENTS)
                checkpos = 0;
            if (g_events[checkpos].status != 1)
                continue;
			
            duration = now - g_events[checkpos].last_active;
			
            if (duration >= 60) {           
                close(g_events[checkpos].fd);                
                xerror("[fd=%d] timeout\n", g_events[checkpos].fd);                
                eventdel(g_efd, &g_events[checkpos]);
				//处理断开连接的逻辑
            }
        }
		
        /* 等待事件发生 */        
        nfd = epoll_wait(g_efd, events, MAX_EVENTS+1, 1000); 
        if (nfd < 0) {            
            xerror("epoll_wait error, exit\n");           
            break;        
        }        

        for (i = 0; i < nfd; i++)
        {
            ev = (struct myevent_s *)events[i].data.ptr;            
            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {                
                ev->call_back(ev->fd, events[i].events, ev->arg);            
            }            
            if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {
                ev->call_back(ev->fd, events[i].events, ev->arg);            
            }

			work(ev);
        }		
    }

	
    /* 退出前释放所有资源 */
	
    return NULL;
}


int YS_game_thread_init(global_t *master)
{
    int ret = 0;
    if ((ret = pthread_create(&master->game_thread_id, NULL, game_work, master)) != 0) {
        xerror("Can't create thread: %s",
                strerror(ret));
        exit(1);
    }
    return 0;
}

