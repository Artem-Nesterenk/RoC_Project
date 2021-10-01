//-----------server includes----------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>

//-----------user includes------------------------

#include "camera.h"
#include "json.h"
#include "json-parser.h"

//-----------user variables -----------------------

const int shmmemSize = 4048;
const char* shmmemName = "photo";

int shm_fd;
void* ptr;
char* method;
char* direction;
int distance;
//---------  server variables  ----------------------
    int lfd, cfd;
    socklen_t clt_addr_len;
    struct sockaddr_in srv_addr, clt_addr;
    char buf[2048] = "";  // Receive buffer
    char command[]=""; //Command buffer
    char* pos; // pointer to detect the ! symbol (End of Command)
    int exitstatus, bytes_read;



//-----------  server functions  ----------------------
void reaper(int sig){
	int status;
	while(wait3(&status,WNOHANG,(struct rusage*)0)>=0);
	
}



void* server_run(void* args)
{
	
	
	
    // Clear the address structure (in bytes), error-prone (the latter two parameters are easy to reverse)
    // memset(&srv_addr, 0, sizeof(srv_addr));
    // bzero can also be used to clear the operation 
    bzero(&srv_addr, 0);
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(8000);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Create a socket
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // port multiplexing
 	 int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));

    // bind the socket
    bind(lfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

    // Listen to the client's connection
    listen(lfd, 128);
    
    clt_addr_len = sizeof(clt_addr);
    
    signal (SIGCHLD, reaper);
    puts ("server created\n");
    while (1)
    { 	
	cfd = accept(lfd, (struct sockaddr *)&clt_addr, &clt_addr_len);
	if(cfd>0){
       		 pid_t pid = fork();

		switch(pid){

		case 0: 
			close(lfd);
			fcntl(cfd, F_SETFL, O_NONBLOCK);

			printf("client no. %d connected \n", cfd);
			printf("New connection, socket fd is %d, ip is: %s, port is %d \n", cfd, inet_ntoa(clt_addr.sin_addr), ntohs(clt_addr.sin_port));

			 while(1)
			    {
					bytes_read = recv(cfd, buf,2048,0);
					if (bytes_read > 0) {
						buf[bytes_read] = '\0';	
						method = json_parser_get_string(buf,"method");
						
						if(strcmp(method,"post")==0){
							send(cfd,"\n POST \n ",8,0);
							direction = json_parser_get_string(buf,"move");
							if(strcmp(direction,"forward")==0){
								distance = json_parser_get_int(buf,"distance");
								/*
								 *  Here we move robot forward on distance
								 */ 							
								}
							if(strcmp(direction,"backward")==0){
								
								/*
								 * Here we move robot backward on distance
								 */							
								}
						}
						if(strcmp(method,"get")==0){
							get_Img_Handler(inet_ntoa(clt_addr.sin_addr),  cfd);
							
						}
						if(strcmp(buf,"EXIT") == 0) break;
					}
						
			    }
			    printf("Exit\n");
			    close(cfd);
			    exit(0);
		default: close(cfd);			
		}
	   }
      }		
    close(lfd); 
}
