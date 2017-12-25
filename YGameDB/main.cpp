#include "YS.h"
#include "Yprocess.h"
#include "YGame_Server.h"


int main(int argc, const char **argv)
{
	YS_init(&master_thread);
	
	func_init(&master_thread.func, do_work);
	
	YS_thread_init(&master_thread);

	YS_game_thread_init(&master_thread);
	
	YS_master_thread_init(&master_thread);

	YS_master_thread_loop(&master_thread);
	
	return 0;
}


























