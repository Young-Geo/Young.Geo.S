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

#include "Xlog.h"


#define WORK_THREAD 4
#define SER_ADDR INADDR_ANY
#define SER_PORT 8080
#define MAX_LISTEN 128
#define MAX_INT 1024*1024





typedef struct _thread_entity_t
{
		pthread_t thread_id;		/* unique ID of this thread */
		struct event_base *base;	/* libevent handle this thread uses */
		struct event notify_event;	/* listen event for notify pipe */
		int notify_receive_fd;		/* receiving end of notify pipe */
		pthread_mutex_t mutex_rec;
		int notify_send_fd; 		/* sending end of notify pipe */
		pthread_mutex_t mutex_sen;
		int conn_num;
} thread_entity_t;


typedef struct _global_t
{
	pthread_t master_thread_id;
	struct event_base *master_main_base;
	struct event master_main_notify_event;
	int conn_receive_fd;
	int num_threads;
	pthread_t *threads;
	thread_entity_t *thread_entitys;
} global_t;


extern global_t master_thread;
extern struct event_base *master_main_base;


int sigignore(int sig);
int YS_INIT(global_t *master);
int YS_thread_init(global_t *master);
int YS_master_thread_init(global_t *master);
void YS_master_thread_loop(global_t *master);
int setup_thread(thread_entity_t *thread_entity);
int create_worker(void* (*func)(void *), void *arg);
void thread_libevent_process(int fd, short which, void *arg);

