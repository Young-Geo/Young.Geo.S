#ifndef _YGAME_H_
#define _YGAME_H_

using namespace std;
#include <string>
#include <deque>
#include <iterator>
#include <algorithm>
#include "global_struct.h"
#include "virtual-Game.h"
#include "YUser.h"
#include "YCard.h"

#define GAME_LEN (USERNAME_LEN * 3)



class Game : public virtual_Game
{
public:
	Game(User *_user_1, User *_user_2, User *_user_3, p_g arg);
	virtual ~Game();
public:
	virtual int 	display();//开始游戏
	virtual void 	stop();//停止游戏
	virtual void 	destory();//销毁游戏
public:
	u8 *	get_name();//获取名字
	int 	create_card();//创建棋牌
	int 	get_car_count();
	int		Shuffle();//洗牌
	int		deal();//发牌
	int		data2client();//把牌的数据发给客户端
protected:
	
private:
	//三个user
	User *_user_1, *_user_2, *_user_3;
	u8 name[GAME_LEN];
	deque<Card *> *cards;
	p_g master;
};














#endif
