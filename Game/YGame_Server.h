#ifndef _YGAME_SERVER_H_
#define _YGAME_SERVER_H_

#include "afx.h"
#include "YS.h"
#include "YUser.h"

#define EVOUT(g_efd, ev, call_back)	\ 
	do {	\
		eventdel((g_efd), (ev));	\
		eventset((ev), ((ev)->fd), (call_back), (ev));	\
		eventadd((g_efd), (EPOLLOUT), (ev));	\	
	} while (0)

#define EVIN(g_efd, ev, call_back)	\ 
		do {	\
			eventdel((g_efd), (ev));	\
			eventset((ev), ((ev)->fd), (call_back), (ev));	\
			eventadd((g_efd), (EPOLLIN), (ev));	\	
		} while (0)

#define EVMOD(g_efd, ev, call_back, event)	\ 
		do {	\
			eventdel((g_efd), (ev));	\
			eventset((ev), ((ev)->fd), (call_back), (ev));	\
			eventadd((g_efd), (event), (ev)); \	
		} while (0)


typedef enum _Game_type {
	FIRST = 4,
	DEAL,
	DISCARD
} Game_type;

int YS_game_thread_init(global_t *master);


#endif
