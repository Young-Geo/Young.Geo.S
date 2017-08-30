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
	virtual int 	display();//��ʼ��Ϸ
	virtual void 	stop();//ֹͣ��Ϸ
	virtual void 	destory();//������Ϸ
public:
	u8 *	get_name();//��ȡ����
	int 	create_card();//��������
	int 	get_car_count();
	int		Shuffle();//ϴ��
	int		deal();//����
	int		data2client();//���Ƶ����ݷ����ͻ���
protected:
	
private:
	//����user
	User *_user_1, *_user_2, *_user_3;
	u8 name[GAME_LEN];
	deque<Card *> *cards;
	p_g master;
};














#endif
