#include<stdio.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include <stddef.h>
#include "LinkedList.h"

#define MAXDATASIZE 800

int match(struct Node * head, char *name, char *pw) {
  
  return is_valid_user(head,name,pw);
}

void execute_command( char * com, char * result){
	FILE * fp;
        char each_line[200] = {0};

	strcat(com, " >out.txt 2>out.txt");
	system(com);
	fp = fopen("out.txt","r"); 
	
	while(fgets(each_line,sizeof(each_line),fp) != NULL){
		strcat(result,each_line);
		memset(&each_line,0,sizeof(each_line));
	}
        
	fclose(fp);
	  
}

/* Open the socket*/
int create_service()
{
 	int                     n,portno,acceptSock,listenSock;
        char                    login[100],pwd[100];
        struct sockaddr_in      serv_addr;

        serv_addr.sin_family=AF_INET;
        serv_addr.sin_port=htons(10551);
        serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
        listenSock = socket(AF_INET,SOCK_STREAM,0);
        if (listenSock < 0){
                perror("Socket Creation failed!");
                exit(1);
        }

        n = bind(listenSock,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
        if (listenSock < 0){
                perror("Bind failed!");
                exit(1);
        }
        n = listen(listenSock,20);
	if (n<0){
                perror("Listen failed!");
                exit(1);
        }
	printf("socket waiting..\n");
	return listenSock;
}

/* Send the content of "msg" to the client */
void send_data(int acceptSock, char * msg){
   int n;   
   n=send(acceptSock,msg,strlen(msg),0);
   if (n<0){
      printf("send failed");
   }
}


/* If expecting a reply, get data from the client */ 
void get_data(int acceptSock, char * data){
   int n;   
   n = recv(acceptSock,data,MAXDATASIZE,0);
   if (n<0){
      printf("receive failed");
   }
}

/* Check if its a valid client
 * Return 1 if it is a valid client and the password entered is correct
 * Return 0 otherwise
*/
int authentic_user(struct Node * head, int listenSock, int  acceptSock, char * my_name){
   char name[123], pw[123],buffer[123]; /* passwords are short! */
   int n;
   char *good = "Welcome to The Machine!\n";
   char *evil = "Invalid identity, exiting!\n";
   char *login = "login: ";
   char *password = "password: ";
   

   memset(&name,0,sizeof(name));
   memset(&pw,0,sizeof(pw));

   
   send_data(acceptSock,login);
   get_data(acceptSock,name);

   if (strlen(name) == 0){
      return -1;
   }

   send_data(acceptSock,password);
   get_data(acceptSock,pw);
   if (strlen(pw) == 0){
      return -1;
   }
   
   strcpy(my_name,name);
   memset(&buffer, 0,sizeof(buffer));
   if( match(head,name,pw) == 1 ){
      
      strcpy(buffer,"Welcome to the machine\n");
      send_data(acceptSock,buffer);
      return 1;
   }
   else{
      
      strcpy(buffer,"Invalid identity\n");
      send_data(acceptSock,buffer);
      return 0;
   }
}


/*
 * Helper function to execute the action specified by the client.
 * Hopefully works
 * 1 - add new user, 2 - update password, 3 - execute shell command, 4 - quit
*/
struct Node * carry_out_command(struct Node * head, char option, int acceptSock, char * my_name){
   char name_data[123] = {0};
   char pw_data[123] = {0};
   char result[MAXDATASIZE] = {0};
   char command[100] = {0};
   switch(option){
      case '1':
         send_data(acceptSock,"username: ");
         get_data(acceptSock,name_data);
	 if (strlen(name_data) == 0){
      		return head;
   	 }
         send_data(acceptSock,"password: ");
         get_data(acceptSock,pw_data);
	 if (strlen(pw_data) == 0){
      		return head;
   	 }
         if (is_user_present(head,name_data) == 0){
         	head = add_to_list(head, name_data, pw_data);
         	strcpy(result,"Successfully added the user");
	 }
	 else{
		strcpy(result, "User already present");
	 }
         send_data(acceptSock,result);
         break;

      case '2':
         send_data(acceptSock,"new password: ");
         get_data(acceptSock,pw_data);
	 if (strlen(pw_data) == 0){
      		return head;
   	 }
         head = update_password(head, my_name, pw_data);
         strcpy(result,"Successfully updated the password");
         send_data(acceptSock,result);
         break;

   /*   case '3':
         display_list(head,result);
         send_data(acceptSock,result);
         break;*/

      case '3':
         send_data(acceptSock,">");
         get_data(acceptSock,command);
	 if (strlen(command) == 0){
      		return head;
   	 }
         execute_command(command, result);
         send_data(acceptSock,result);
         break;

      default:
         break;
   }
   return head;
}


main(){
   struct Node * head = 0;
   int listenSock,acceptSock,n;
   char * options ="\n1 - add user\n2 - update password\n3 - execute shell\n4 - quit\n";
   char option;
   char command[MAXDATASIZE] = {0};
   char my_name[MAXDATASIZE] = {0};
   head = init_linked_list(head);
   listenSock = create_service();
   int maxNumTries = 5;
   int tries = 0;
   int auth_result = 0;

      while(1){
         tries = 0;
         auth_result = 0;
	 acceptSock = accept(listenSock,(struct sockaddr *)NULL,NULL);
         memset(&my_name,0,sizeof(my_name));
         while (tries < maxNumTries){
		
		tries++;
                auth_result = authentic_user(head, listenSock, acceptSock,my_name);
         	if (auth_result == 1 || auth_result == -1){
            		break;
         	}
	}
        if (auth_result == -1){
		close(acceptSock);
		continue;
	}
	if (tries >= maxNumTries){
                send_data(acceptSock,"close");
		close(acceptSock);
		continue;
	}
        else{
	    while(1){
               memset(&command,0,sizeof(command));
		
               send_data(acceptSock,options);
               get_data(acceptSock,command);
               option = command[0];
               if (option == '4' || strlen(command) == 0){
		   close(acceptSock);
		   break;
               }
               head = carry_out_command(head, option, acceptSock, my_name); 
	   }
        }
     
    }	
    
}
	
