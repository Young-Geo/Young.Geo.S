#ifndef __VIRTUAL_GAME_H_
#define __VIRTUAL_GAME_H_

#include "xlog.h"
#include "afx.h"

class virtual_Game {
public:
	virtual_Game(){}
	~virtual_Game(){}
public:
	virtual int display() = 0;
	virtual int stop() = 0;
	virtual u8 *get_name() = 0;
};

#endif
