#include "YUser.h"


User::User()
{
	this->_user_status = ON_LINE;
}

User::User(u8 *username, u8 *password, p_g thread_entity)
{
	if (!username || !password) {
		xerror("User username password NULL\n");
		return;
	}

	memcpy(this->_username, username, strlen((char *)username));
	memcpy(this->_password, password, strlen((char *)password));
	this->_user_status = ON_LINE;
	this->_thread_entity = thread_entity;
	return;
}

User::User(u32 id, u8 *username, u8 *password, u32 money_z, u32 money_d, u32 solo_w, u32 solo_f, u16 solo_s, u16 lv, p_g thread_entity)
{
	if (!username || !password) {
		xerror("new User username password NULL\n");
		return;
	}
	
	memcpy(this->_username, username, strlen((char *)username));
	memcpy(this->_password, password, strlen((char *)password));
	
	this->_id = id;
	this->_money_z = money_z;
	this->_money_d = money_d;
	this->_solo_w = solo_w;
	this->_solo_f = solo_f;
	this->_solo_s = solo_s;
	this->_lv = lv;
	this->_user_status = ON_LINE;
	this->_thread_entity = thread_entity;
}

User::~User(){}



u8 *			User::get_username()
{
	return this->_username;
}

int 			User::set_username(u8 *username)
{

	if (!username) {
		xerror("set_username username NULL\n");
		return -1;
	}

	memcpy(this->_username, username, strlen((char *)username));
	return 0;
}

u8 *			User::get_password()
{
	return this->_password;
}
int 			User::set_password(u8 *password)
{
	if (!password) {
		xerror("set_password password NULL\n");
		return -1;
	}

	memcpy(this->_password, password, strlen((char *)password));
	return 0;
}


p_g User::get_thread()
{
	return this->_thread_entity;
}
int  User::set_thread(p_g thread_entity)
{
	if (!thread_entity) {
		xerror("User set_thread error \n");
		return -1;
	}
	this->_thread_entity = thread_entity;
	return 0;
}

USER_STATUS 	User::get_status()
{
	return this->_user_status;
}
void			User::set_status(USER_STATUS user_status)
{
	this->_user_status = user_status;
}













