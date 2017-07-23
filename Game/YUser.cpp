#include "YUser.h"


User::User()
{
	
}
User::~User()
{

}



u8 *			User::get_username()
{
	return this->username;
}
int 			set_username(u8 *username)
{

	if (!username) {
		xerror("set_username username NULL\n");
		return -1;
	}

	memcpy(this->username, username, strlen(username));
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













