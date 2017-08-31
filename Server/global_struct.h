#ifndef _GLOBAL_STRUCT_H_
#define _GLOBAL_STRUCT_H_

#include "xlist.h"
#include "ocicursor.h"
#include <pthread.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/event_compat.h>




typedef struct _global_t global_t;
typedef struct _thread_entity_t thread_entity_t;


typedef int (*func_work_t)(void *, void *, void *, void *);

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
	pthread_t master_thread_id, game_thread_id;
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




#endif
