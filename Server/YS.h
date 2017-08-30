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


#include "xlog.h"
#include "xlist.h"
#include "afx.h"
#include "xchain.h"


#include "Ypkt.h"
#include "YUser.h"
#include "global_struct.h"


#define WORK_THREAD 4
#define SER_ADDR INADDR_ANY
#define SER_PORT 8080
#define MAX_LISTEN 128
#define MAX_INT 1024*1024
#define DATABASE_SID "//127.0.0.1/landlord_game"
#define DATABASE_USR "root"
#define DATABASE_PWD "anxan4444anxan"



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
