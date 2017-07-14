#include "YS.h"

global_t master_thread;
struct event_base *main_base;


int sigignore(int sig)
{
    struct sigaction sa = { .sa_handler = SIG_IGN, .sa_flags = 0 };

    if (sigemptyset(&sa.sa_mask) == -1 || sigaction(sig, &sa, 0) == -1) {
        return -1;
    }
    return 0;
}

int YS_INIT(global_t *master)
{
	assert(master);
	master->num_threads = WORK_THREAD;
	return 0;
}

int setup_thread(thread_entity_t *thread_entity)
{
	int ret = 0;
	thread_entity->base = base_
	
	return 0;
}

int YS_thread_init(global_t *master)
{
	int ret = 0, nthreads = 0, i, pfd[2];	

	assert(master);
	
	nthreads = master->num_threads;
	

	for (i = 0; i < nthreads; i++)
	{
		master->threads = (pthread_t *)cmalloc(nthreads, sizeof(pthread_t));
		assert(master->threads);

		master->thread_entitys = (thread_entity_t *)cmalloc(nthreads, sizeof(thread_entity_t));
		assert(master->thread_entitys);
		
        if (pipe(pfd)) {
            perror("Can't create notify pipe");
            exit(1);
        }
		
        master->thread_entitys[i].notify_receive_fd = pfd[0];
        master->thread_entitys[i].notify_send_fd = pfd[1];

        setup_thread(&master->thread_entitys[i]);
    }

    /* Create threads after we've done all the libevent setup. */
	for(i = 0; i < nthreads; i++)
	{
    	create_worker(worker_libevent, &threads[i]);
    }
	
	return 0;
}

int main(int argc, const char **argv)
{
	int ret = EXIT_SUCCESS;
	
	sigignore(SIGPIPE);
	YS_INIT(&master_thread);

	
	main_base = event_init();
	assert(main_base);





	 if (event_base_loop(main_base, 0) != 0) {
	 	ret = EXIT_FAILURE;
		}
	
	return 0;
}
