#ifndef _YGAME_H_
#define _YGAME_H_

#include "YUser.h"
#include "virtual-Game.h"

#define GAME_LEN (USERNAME_LEN * 3)



class Game : public virtual_Game
{
public:
	Game(User *_user_1, User *_user_2, User *_user_3, u8 *name);
	~Game();
public:
	virtual int display();
	virtual int stop();
	virtual u8 *get_name();
protected:
	
private:
	//Èý¸öuser
	User *_user_1, *_user_2, *_user_3;
	u8 name[GAME_LEN];
};














#endif
