#include "server.h"
#include "camera.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
	
int res;	

int  main (void){
	printf ("ALL is good\n");
		pthread_t thread_1;
		res = pthread_create(&thread_1, NULL, server_run, NULL);
		printf ("Server_thread created\n");
		//when detached thread terminates, all resoursec will be automatically relase back to the system 
		if(!res)
		{
			pthread_detach(thread_1);
			}
		
		pthread_t thread_2;
		res = pthread_create(&thread_2, NULL, img_Queue, NULL);
		if(!res)
		{
			pthread_detach(thread_2);
			}
		
		
		while(1){};
	return 0;
	}
