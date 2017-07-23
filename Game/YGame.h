#ifndef _YGAME_H_
#define _YGAME_H_

#include "YUser.h"



class Game
{
public:
	Game(User *_user_1, User *_user_2, User *_user_3);
	~Game();
public:
	int display();
protected:
	
private:
	//Èý¸öuser
	User *_user_1;
	User *_user_2;
	User *_user_3;
};














#endif
