#include "YGame.h"



Game::Game(User * user_1,User * user_2,User * user_3, u8 *name)
{
	this->_user_1 = user_1;
	this->_user_2 = user_2;
	this->_user_3 = user_3;
	if (name) {
		memcpy(this->name, name, USERNAME_LEN * 3);
	}
}

Game::~Game()
{
	//if (_user_1) delete _user_1;
	//if (_user_2) delete _user_2;
	//if (_user_3) delete _user_3;
}

int 	Game::display()
{
	if (!this->_user_1 || !this->_user_2 || !this->_user_3) {
		xerror("error user NULL\n");
		return -1;
	}
	//通知发牌等一切事物
	//改变用户相应状态
	this->_user_1->set_status(IN_GAME);
	this->_user_2->set_status(IN_GAME);
	this->_user_3->set_status(IN_GAME);
	return 0;
}

int Game::stop()
{
	return 0;
}

u8 * 	Game::get_name()
{
	return this->name;
}


