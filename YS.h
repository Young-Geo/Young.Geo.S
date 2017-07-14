#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>

#define WORK_THREAD 4

typedef struct _thread_entity_t
{
		pthread_t thread_id;		/* unique ID of this thread */
		struct event_base *base;	/* libevent handle this thread uses */
		struct event notify_event;	/* listen event for notify pipe */
		int notify_receive_fd;		/* receiving end of notify pipe */
		int notify_send_fd; 		/* sending end of notify pipe */
} thread_entity_t;


typedef struct _global_t
{
	int num_threads;
	pthread_t *threads;
	thread_entity_t *thread_entitys;
} global_t;

int sigignore(int sig);
int YS_INIT(global_t *master);
int YS_thread_init(global_t *master);
int setup_thread(thread_entity_t *thread_entity);
int create_worker(void* (*func)(void *), void *arg);

