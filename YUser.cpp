#include "YUser.h"


User::User()
{
	
}
User::~User()
{

}









void * User::get_thread()
{
	return this->_thread_entity;
}
int  User::set_thread(void *thread_entity)
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













