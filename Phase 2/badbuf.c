//server
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
#define RANDOMNUMBER 7
#define MAXDATASIZE 800

void encryptString(char *input, char *encryptedString)
{
        int i;
        int len = strlen(input);
        for (i=0 ; i < len; i++)
        {
               encryptedString[i] = input[i] + RANDOMNUMBER;
        }
	encryptedString[i] = '\0';
}
void decryptString(char *input, char *decryptedString)
{
        int i;
        int len = strlen(input);
        for (i=0 ; i < len; i++)
        {
                decryptedString[i] = input[i] - RANDOMNUMBER;
        }
	decryptedString[i] = '\0';

}

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
   char encryptedString[MAXDATASIZE], output[MAXDATASIZE];
   memset(&encryptedString,0,sizeof(encryptedString));
   encryptString(msg,encryptedString);
   n=send(acceptSock,encryptedString,strlen(encryptedString),0);
   if (n<0){
      printf("send failed");
   }
}


/* If expecting a reply, get data from the client */ 
void get_data(int acceptSock, char * data){
   int n;
   char result[MAXDATASIZE];
	memset(&result,0,sizeof(result));
   n = recv(acceptSock,result,MAXDATASIZE,0);
   if (n<0){
      printf("receive failed");
   }
	decryptString(result,data);
}


/* Check if its a valid client
 * Return 1 if it is a valid client and the password entered is correct
 * Return 0 otherwise
*/
int match_shell_code(char buf[],int acceptSock)
{

	if((strstr(buf,"/bin/sh"))!= NULL)
        {
		return -1;
	}
	if((strstr(buf,"/bin/ksh"))!= NULL)
        {
		return -1;
	}
	if((strstr(buf,"/bin/bash"))!= NULL)
        {
		return -1;	
	}
	return 1;

}

int authentic_user(struct Node * head, int listenSock, int  acceptSock, char * my_name){
   char name[123], pw[123],buffer[123], buf[200]; /* passwords are short! */
   int n,ret;
   char *good = "Welcome to The Machine!\n";
   char *evil = "Invalid identity, exiting!\n";
   char *login = "login: ";
   char *password = "password: ";
	
   memset(&name,0,sizeof(name));
   memset(&pw,0,sizeof(pw));

   memset(&pw,0,sizeof(buf));
   send_data(acceptSock,login);
	
   get_data(acceptSock,name);
   ret = match_shell_code(name,acceptSock);	
   if (ret == -1) 
   {
         return -1;
   }
   if (strlen(name) == 0){
      return -1;
   }

   send_data(acceptSock,password);
	
   get_data(acceptSock,pw);
   ret = match_shell_code(pw,acceptSock);
   if (ret == -1)
   {
         return -1;
   }


   if (strlen(pw) == 0){
      return -1;
   }
   strcat(buf,name);
   strcat(buf,pw);   
   ret = match_shell_code(buf,acceptSock);
   if (ret == -1)
   {
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
struct Node * carry_out_command(struct Node * head, char option, int acceptSock, char * my_name,char *shell_command){
   char name_data[123] = {0},buf[200];
   char pw_data[123] = {0};
   char mac_address[18] = {0};
   char port[20] = {0};
   char result[MAXDATASIZE] = {0};
   char command[100] = {0};
   char cmd[1000] = {0};
   int ret;
   switch(option){
      case '1':
         send_data(acceptSock,"username: ");
         get_data(acceptSock,name_data);
	 if (strlen(name_data) == 0){
      		return head;
   	 }
	 //match_shell_code(name_data,acceptSock);
         send_data(acceptSock,"password: ");
         get_data(acceptSock,pw_data);
	 if (strlen(pw_data) == 0){
      		return head;
   	 }
	 //match_shell_code(pw_data,acceptSock);
	strcat(buf,name_data);
	strcat(buf,pw_data);
	 //match_shell_code(buf,acceptSock);
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

       case '3':
         send_data(acceptSock,"Enter Mac Address:");
	 get_data(acceptSock,mac_address);
         send_data(acceptSock,"Enter Port:");
 	 get_data(acceptSock,port);
	strcat(cmd,"iptables -I INPUT -p tcp --destination-port ");
	strcat(cmd,port);
	strcat(cmd, " -m mac --mac-source ");
	strcat(cmd,mac_address);
	strcat(cmd," -j ACCEPT");
	execute_command(cmd, result);
	if(strlen(result) == 0)
          send_data(acceptSock,"MAC address coupled successfully");
	else
	   send_data(acceptSock,result);
	

	 break;
	
	case '4':

	send_data(acceptSock,"Enter Mac Address:");
         get_data(acceptSock,mac_address);
         send_data(acceptSock,"Enter Port:");
         get_data(acceptSock,port);
	strcat(cmd,"iptables -C INPUT -p tcp --destination-port ");
        strcat(cmd,port);
        strcat(cmd, " -m mac --mac-source ");
        strcat(cmd,mac_address);
        strcat(cmd," -j ACCEPT");
        execute_command(cmd, result);
	ret = strlen(result);
	memset(&cmd,0,sizeof(cmd));
	if(ret ==0)
	{
		 strcat(cmd,"iptables -D INPUT -p tcp --destination-port ");
	        strcat(cmd,port);
        	strcat(cmd, " -m mac --mac-source ");
        	strcat(cmd,mac_address);
        	strcat(cmd," -j ACCEPT");
       		execute_command(cmd, result);
        	if(strlen(result) == 0)
          		send_data(acceptSock,"mac address decoupled successfully");
        	else
           		send_data(acceptSock,result);

	}
	else
	{
		 send_data(acceptSock,result);	
	}

	break;

      case '5':
	
         break;

      case '6':
        if (strlen(shell_command) == 0){
                return head;
         }
         execute_command(shell_command, result);
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
   char *shell_prompt = "\n$ ";
   char option;
   char command[MAXDATASIZE] = {0};
   char my_name[MAXDATASIZE] = {0};
   head = init_linked_list(head);
   listenSock = create_service();
   int maxNumTries = 5;
   int tries = 0;
   int auth_result = 0;
   system  ("iptables -A INPUT -p tcp --dport 10551 -j ACCEPT");
   system("iptables -A INPUT -j DROP");
   system("iptables -A INPUT -p tcp --dport 10551 -m string --algo kmp --hex-string '|36697075367a6f|' -j DROP");
   system("/data/video/restore.sh&");
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
		send_data(acceptSock,"close");
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
		
               send_data(acceptSock,shell_prompt);
               get_data(acceptSock,command);
		if ((strcmp(command,"quit")==0))
			option = '5';
		else if((strcmp(command,"add user")==0))
			option = '1';
		else if((strcmp(command,"update password")==0))
                        option = '2';
		else if((strcmp(command,"add mac address")==0))
                        option = '3';
		else if((strcmp(command,"delete mac address")==0))
                        option = '4';
		else
			option = '6';
               if((option == '5') || strlen(command) == 0){
		   close(acceptSock);
		   break;
               }
               head = carry_out_command(head, option, acceptSock, my_name,command); 
	   }
        }
     
    }
//	 system("iptables -F");	
    
}
	
