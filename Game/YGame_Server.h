#ifndef _YGAME_SERVER_H_
#define _YGAME_SERVER_H_

#include "afx.h"
#include "YS.h"
#include "YUser.h"

typedef enum _Game_type {
	first = 4
} Game_type;

int YS_game_thread_init(global_t *master);


#endif
