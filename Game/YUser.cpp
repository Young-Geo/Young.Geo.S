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

	memcpy(this->username, username, strlen((char *)username));
	memcpy(this->password, password, strlen((char *)password));
	this->_user_status = ON_LINE;
	this->_thread_entity = thread_entity;
	return;
}
User::~User(){}



u8 *			User::get_username()
{
	return this->username;
}
int 			User::set_username(u8 *username)
{

	if (!username) {
		xerror("set_username username NULL\n");
		return -1;
	}

	memcpy(this->username, username, strlen((char *)username));
	return 0;
}

u8 *			User::get_password()
{
	return this->password;
}
int 			User::set_password(u8 *password)
{
	if (!password) {
		xerror("set_password password NULL\n");
		return -1;
	}

	memcpy(this->password, password, strlen((char *)password));
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













