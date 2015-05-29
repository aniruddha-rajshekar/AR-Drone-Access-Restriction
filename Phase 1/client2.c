#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT "10551"
#define MAXDATASIZE 800

void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void * listenThread(void * sockfd){
   int sock_fd = (int )sockfd;
   char buf[MAXDATASIZE];
   int numbytes;
   char * close_sock = "close";
   while(1){
      memset(&buf, '\0', MAXDATASIZE);
      if ((numbytes = recv(sock_fd, buf, MAXDATASIZE-1, 0)) == -1){
         printf("recv error\n");
         exit(1);
      }
      printf("%s",buf);
      fflush(stdout);
      
      if (strcmp(buf,close_sock) == 0){
         close(sock_fd);
	 printf("Exiting\n");
         exit(1);
      }	
   }
}

/* Send the content of "msg" to the client */
void send_data(int acceptSock, char * msg){
   int n;   
   n=send(acceptSock,msg,strlen(msg),0);
   if (n<0){
      perror("send failed");
   }
   //printf("send**%d**\n",n); // FOR DEBUGGING
}


/* If expecting a reply, get data from the client */ 
void get_data(int acceptSock, char * data){
   int n;   
   n = recv(acceptSock,data,MAXDATASIZE,0);
   if (n < 0){
      perror("receive failed");
   }
   
   //printf("get**%d**\n",n); // FOR DEBUGGING
}

int main (int argc, char *argv[]){
	
	char command[MAXDATASIZE];
	char buf[MAXDATASIZE];
	char s[INET6_ADDRSTRLEN];
	char name[MAXDATASIZE];
	size_t capacity = MAXDATASIZE;
	int sockfd, numbytes, rv;
	pthread_t tid;	
	struct addrinfo hints, *servinfo, *p;

	if (argc != 2){
		printf("Usage: client hostname\n");
		return 1;
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0){
		printf("getaddrinfo error\n");
		return 1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next){
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			printf("client: socket\n");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			printf("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL){
		printf("Client failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
	printf("client: connecting to %s\n",s);
	freeaddrinfo(servinfo);
	pthread_create(&tid,NULL,listenThread,(void *)sockfd);	
	while(1){
	        memset(&buf, '\0', MAXDATASIZE);
	        memset(&command, '\0', MAXDATASIZE);		
                fgets(command,MAXDATASIZE,stdin);
	        if (strlen(command) > 0 && command[strlen(command)-1] == '\n'){
			//if (!(strcmp(command,"\n") == 0)){
	                	command[strlen(command)-1] = '\0';
			//}
	        }
		
		if (send(sockfd, command, strlen(command),0) == 0){
			printf("Send error\n");
			break;
		}
		
		if (strcmp(command,"4") == 0){
			break;
		}			
		
	}
	close(sockfd);
	return 0;
}


