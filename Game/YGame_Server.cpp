#include "YGame_Server.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "YUser.h"
#include "Ypkt.h"
#include "YGame.h"



#define CARD_COUNT 17
#define LARD_CARD 3
#define MAX_EVENTS  1024
#define BUFLEN 1024
#define TRUE 1

typedef struct _BUF_T {
	unsigned char *buf;
	int len;
} Buf_t;

typedef enum _ev_status {
	NONE,
	INIT,
	ADD,
	DEL,
	OFF
} ev_status;


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
	xlist *outbufs;
    long last_active;
	User *user;
};


static int g_efd = 0;
/* epoll_create返回的句柄 */
static struct myevent_s g_events[MAX_EVENTS+1] = { 0 };
/* +1 最后一个用于 listen fd */
static xlist *wait_senddata_user = NULL;

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
	//ev->user = NULL;
    ev->status = INIT;
	if (!ev->bufs) {
		ev->bufs = xlist_init();
		xmessage("init in bufs");
	}
	if (!ev->outbufs) {
		ev->outbufs = xlist_init();		
		xmessage("init out bufs");
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

    if (ev->status == ADD) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        ev->status = ADD;
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
    if (ev->status != ADD)
        return;
    epv.data.ptr = ev;
    ev->status = DEL;
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

/*
	int bNoDelay = 1;
	if (-1 == setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY, (void *)&bNoDelay, sizeof(int))) {
		xerror("setsockopt ");
		return;
	}
*/


    do {

        for (i = 0; i < MAX_EVENTS; i++)
        {
            if (g_events[i].status == NONE)
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
    //eventdel(g_efd, ev);

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
    int len, size = 0, ret = 0, buf_len = 0;
	xlist *bufs = NULL;
	Buf_t *buft = NULL;
	unsigned char *buf = NULL;
    //处理业务，小写转大写    len = send(fd, ev->buf, ev->len, 0);
    /*
       printf("fd=%d\tev->buf=%s\ttev->len=%d\n", fd, ev->buf, ev->len);
       printf("send len = %d\n", len);
     */
     //遍历加协议
     
	xassert(ev);
	
	bufs = ev->outbufs;
	size = xlist_size(ev->outbufs);
	
	while (size-- > 0)
	{
		//解析数据处理相应业务
		buft = (Buf_t *)xlist_popv(bufs);
		xassert(buft);
		xassert(buft->buf);
		ret = pkt_make_client(buft->buf, buft->len, &buf, &buf_len);
		if (ret) {
			xerror("pkt_make_client ");
			goto end;
		}

		len += send(fd, buf, buf_len, 0);		
		xmessage("send fd = %d ++len = %d ok", fd, len);

		end:
		if (buf) 		xfree(buf);
		if (buft->buf) 	xfree(buft->buf);
		if (buft) 		xfree(buft);
	}
	
    if (len > 0) {
        
		xmessage("send fd = %d count len = %d ok", fd, len);
		EVMOD(g_efd, ev, recvdata, EPOLLIN);

    } else {

        close(ev->fd);
        eventdel(g_efd, ev);
        xerror("send[fd=%d] error %s\n", fd, strerror(errno));

    }

	
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
    sin.sin_addr.s_addr = INADDR_ANY;
	//inet_pton(AF_INET, GAME_SER_IP, (void *)(&sin.sin_addr.s_addr));
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

	while (master->ready_num >= GAME_USER_COUNT)
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
			xlist_kill_remove(master->readys, (char *)users[i]->get_username());
			--master->ready_num;
		}
		pthread_mutex_unlock(&master->mutex_ready);

		game = new Game(users[0], users[1], users[2]);
		if (!game) {
			xerror("new Game error");
			continue;
		}		

		if (game->display()) {
			xerror("game display error");
			return -1;
		}

		for (i = 0; i < GAME_USER_COUNT; ++i)
		{		
			xlist_add(wait_senddata_user, NULL, XLIST_PTR, (char *)users[i]);
		}
		
		xlist_add(master->games, (char *)game->get_name(), XLIST_PTR, (char *)game);
		
	}

	return 0;
}


User *	 get_user(xlist *games, xlist *readys, unsigned char *username)
{
	xlist *game_temp = NULL, *readys_temp = NULL;
	Game *game = NULL;
	User *user = NULL;
	
	if (!games || !readys) {
		xerror("games readys NULL");
		return NULL;
	}

	game_temp = games;
	readys_temp = readys;
	
	while (game_temp && game_temp->next)
	{
		game = (Game *)game_temp->value;
		xassert(game);
		user = game->get_user_by_name(username);
		if (user)
			return user;

		game_temp = game_temp->next;
	}

	while (readys_temp && readys_temp->next)
	{
		user = (User *)readys_temp->value;
		xassert(user);
		if (!xstrncmp((char *)username, (char *)user->get_username(), (USERNAME_LEN - 1)))
			return user;

		readys_temp = readys_temp->next;
	}

	return NULL;
}

int work(struct myevent_s *ev, void *arg)
{
	int i = 0, size = 0;
	xlist *bufs = NULL;
	Buf_t *buft = NULL, *obuft = NULL;
	unsigned short type = 0;
	unsigned char *buf = NULL, data[BUFFER_SIZE], flag = 0;
	User *user = NULL;
	global_t *master = NULL;
	
	xassert(ev);
	xassert(arg);
	
	bufs = ev->bufs;
	user = ev->user;
	size = xlist_size(ev->bufs);
	master = (global_t *)arg;
	buf = data;
	
	while (size-- > 0)
	{
		//解析数据处理相应业务
		buft = (Buf_t *)xlist_popv(bufs);
		if (!buft || !buft->buf)
			continue;
		//处理业务
		xassert(buft->buf);
		buf = buft->buf;
		IN16_LE(buf, type);

		xmessage("in type = %d", type);

		switch (type)//游戏业务处理
		{
			case FIRST:
				{
					//取名字
					unsigned char username[USERNAME_LEN] = { 0 };					
					User *newuser = NULL;
					
					xmemcpy(username, buf, USERNAME_LEN);
					if ((newuser = get_user(master->games, master->readys, username))) {
						if (!ev->user) {
							ev->user = newuser;
							newuser->set_ev((p_g)ev);
						}
						flag = 1;						
					}

					
					xmessage("GAME rec %s", username);
					obuft = (Buf_t *)xmalloc(sizeof(Buf_t));
					xassert(obuft);
					buf = obuft->buf = (unsigned char *)xmalloc(3);
					obuft->len = 3;
					OUT16_LE(buf, FIRST);
					OUT8(buf, flag);
					//可以选择回发错误信息
					xlist_add(ev->outbufs, NULL, XLIST_PTR, (char *)obuft);
					//EVOUT(g_efd, ev, senddata);
					EVMOD(g_efd, ev, senddata, EPOLLOUT);
				}
			break;
			
			case DEAL://请求发牌
				{
					/*
					int buf_len = 0;
					if (!user) {
						xerror("DEAL user error");//暂时处理
						break;
					}
					//ok
					if (user->Card_Count() != CARD_COUNT) {
						xerror("card count %d\n", user->Card_Count());//暂时处理
						buf_len = 3;
						flag = 0;
					} else {
						buf_len = CARD_COUNT + 3;
						flag = 1;
					}
					
					obuft = (Buf_t *)xmalloc(sizeof(Buf_t));
					xassert(obuft);
					buf = obuft->buf = (unsigned char *)xmalloc(buf_len);
					xassert(obuft->buf);
					obuft->len = buf_len;
					OUT16_LE(buf, DEAL);
					OUT8(buf, flag);

					if (flag) {
						xmemcpy(buf, user->get_carddata(), CARD_COUNT);
					}

					xlist_add(ev->outbufs, NULL, XLIST_PTR, (char *)obuft);
					EVMOD(g_efd, ev, senddata, EPOLLOUT);
					*/
				}
			break;
			
			case DISCARD://抢地主
				{
					obuft = (Buf_t *)xmalloc(sizeof(Buf_t));
					xassert(obuft);
					buf = obuft->buf = (unsigned char *)xmalloc(3);
					xassert(obuft->buf);
					obuft->len = 3;
					OUT16_LE(buf, DISCARD);
					OUT8(buf, 1);
					xlist_add(ev->outbufs, NULL, XLIST_PTR, (char *)obuft);
					EVMOD(g_efd, ev, senddata, EPOLLOUT);
				}
			break;

			default:
			break;
		}


		//free buf and Buf_t
		if (buft && buft->buf) xfree(buft->buf);
		if (buft) xfree(buft);
	}

	return 0;
}

int			send_waitdata(xlist  *wait_list)
{
	xlist *list = NULL, *element = NULL, *prev = NULL;
	User *user = NULL;	
	Buf_t *obuft = NULL;
	unsigned char *buf = NULL, *u_buf = NULL;
	int len = 0;	
	struct myevent_s *ev = NULL;
	
	xassert(wait_list);

	list = wait_list;

	while (list && list->next)
	{
		user = (User *)list->value;
		xassert(user);

		if (!(ev = (struct myevent_s *)user->get_ev())) {
			xmessage("user not conn");
			list = list->next;
			continue;
		}

		u_buf = user->get_wait_senddata(&len);
		if (!u_buf || !len) {
			xerror("error user buf NULL");			
			list = list->next;
			continue;
		}
		
		obuft = (Buf_t *)xmalloc(sizeof(Buf_t));
		xassert(obuft);
		buf = obuft->buf = (unsigned char *)xmalloc(len + 3);
		xassert(obuft->buf);
		obuft->len = len + 3;
		
		OUT16_LE(buf, DEAL);
		OUT8(buf, 1);
		xmemcpy((char *)buf, (char *)u_buf, len);

		xlist_add(ev->outbufs, NULL, XLIST_PTR, (char *)obuft);
		EVMOD(g_efd, ev, senddata, EPOLLOUT);
		
		prev = list;
		list = list->next;
		element = xlist_break(wait_list, prev);
		
		xfree(element->key);
  		xfree(element);
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
    if (g_efd <= 0) {
        xerror("create efd in %s err %s\n", __func__, strerror(errno));
		return NULL;
    }

	if (!(wait_senddata_user = xlist_init())) {
		xerror("wait_senddata_user init ");
		return NULL;
	}
	
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
				ev->status = OFF;
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
			work(ev, arg);
        }	


		
		send_waitdata(wait_senddata_user);
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

