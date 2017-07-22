#ifndef _YUSER_H_
#define _YUSER_H_

#include "afx/xlog.h"


class User
{
public:
	void  *get_thread();
	int  set_thread(void *thread_entity);
protected:
	
private:
	//username,password
	void *_thread_entity;
};


















#endif
