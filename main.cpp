#include "YS.h"


int main(int argc, const char **argv)
{
	//int ret = EXIT_SUCCESS;
	
	sigignore(SIGPIPE);
	YS_INIT(&master_thread);

	
	//main_base = event_init();
	//assert(main_base);

	YS_thread_init(&master_thread);
	YS_master_thread_init(&master_thread);

	YS_master_thread_loop(&master_thread);
	//pthread_exit(NULL);// ¼ÌÐø·â×°


	#if 0
	 if (event_base_loop(main_base, 0) != 0) {
	 	ret = EXIT_FAILURE;
	}
	 #endif
	
	return 0;
}


























