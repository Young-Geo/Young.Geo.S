#ifndef _YS_H_
#define _YS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>


#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/event_compat.h>

#include "xlog.h"
#include "xlist.h"
#include "afx.h"
#include "xchain.h"

#include "Ypkt.h"
#include "YUser.h"
#include "global_struct.h"
#include "ocicursor.h"


#define WORK_THREAD 4
#define SER_ADDR INADDR_ANY
#define SER_PORT 8080
#define MAX_LISTEN 128
#define MAX_INT 1024*1024
#define DATABASE_SID "//127.0.0.1/landlord_game"
#define DATABASE_USR "root"
#define DATABASE_PWD "anxan4444anxan"


typedef int (*func_work_t)(void *, void *, void *);

typedef struct _func_t {
	func_work_t do_work;
} func_t;

typedef struct _conn_queue_t {
	xlist *conn_queue;		
	pthread_mutex_t mutex_connqueue;
} conn_queue_t;

typedef struct _thread_entity_t
{
	int inx;
	pthread_t thread_id;		/* unique ID of this thread */
	struct event_base *base;	/* libevent handle this thread uses */
	struct event notify_event;	/* listen event for notify pipe */
	int notify_receive_fd;		/* receiving end of notify pipe */
	int notify_send_fd; 		/* sending end of notify pipe */
	int conn_num;
	conn_queue_t conn_queue;
	xlist *users;		
	pthread_mutex_t mutex_users;
	global_t *master;
	COCICursor coc;
	COCIDatabase database;
} thread_entity_t;


typedef struct _global_t
{
	pthread_t master_thread_id;
	struct event_base *master_main_base;
	struct event master_main_notify_event;
	func_t func;
	int conn_receive_fd;
	int num_threads;
	int last_thread;
	pthread_t *threads;//鍙互鍒犻櫎
	thread_entity_t *thread_entitys;
	xlist **arr_users;//已经连接的用户的链表数组
	//游戏中链表
	xlist *games;
	pthread_mutex_t mutex_game;
	//各种状态链表
	xlist *readys;//已经准备就绪用户的链表	
	pthread_mutex_t mutex_ready;
} global_t;


extern global_t master_thread;
extern struct event_base *master_main_base;

////////////////////
int func_init(func_t *func, func_work_t do_work);
////////////////////
int YS_init(global_t *master);
int YS_thread_init(global_t *master);
int YS_master_thread_init(global_t *master);
void YS_master_thread_loop(global_t *master);



#endif
