#include "Yprocess.h"



int work(void *arg, xchain *rchain, xchain *wchain)
{
	return 0;
}

int do_work(void *arg, void *r, void *w)
{

	thread_entity_t *thread_entity = NULL;
	xchain *rchain = NULL, *wchain = NULL;
	unsigned char type = '\0';


	assert(arg);
	assert(r);
	assert(w);


	thread_entity = (thread_entity_t *)arg;
	rchain = (xchain *)r, wchain = (xchain *)w;

	xchain_get(rchain, (void *)&type, 1);
	

	switch (type)//各种业务处理
	{
		case 'C':
			{
				//登录处理
				//登录成功
				User *user = new User();
				user->set_status(ON_LINE);
				user->set_thread(thread_entity);
				xlist_add(thread_entity->users, (const char *)user->get_username(), XLIST_STRING, (char *)user);
			}
		break;
		
		default:
			
		break;
	}
	
	return 0;
}























