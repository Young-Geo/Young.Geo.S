#include "YUser.h"

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














