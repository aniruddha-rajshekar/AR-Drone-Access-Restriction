#include "LinkedList.h"

/* Initialize the linked list. Called at the beginning */
struct Node * init_linked_list(struct Node * head){
	FILE *fp;
	char name[123];
	char pw[123];
	char line[300];

	fp = fopen("/data/video/names.txt","r");
	while(fgets(line,sizeof(line),fp) != NULL){
		get_credentials(name,pw,line);
		//printf("%s, %s\n",name,pw);
		head = load_users(head,name,pw);
	}
	fclose(fp);
	return head;
}

/* Helper function for init_linked_list*/
void get_credentials(char * name, char * pw, char * line){
	int i=0,j=0;
	while (line[i] != '\0' && line[i] != ','){
		name[i] = line[i];
		i++;
	}
	name[i] = '\0';
	i++;
	while (line[i] != '\0' && line[i] != '\n'){
		pw[j] = line[i];
		i++;
		j++;
	}
	pw[j] = '\0';
}


/* Add the name and password to the list
*  Called the when the server starts
*/
struct Node * load_users( struct Node * head ,char * name, char * pw){
	if (head == 0){
		head = (struct Node * )malloc(sizeof(struct Node));
		strcpy(head->name,name);
		strcpy(head->pw,pw);
		//printf("Added:%s,%s\n",head->name, head->pw);
		head->next = 0;
	}
	else{
		struct Node * tmp = head;
		//printf("Here\n");
		while (tmp->next != 0){
			tmp = tmp->next;
		}
		tmp->next = (struct Node *)malloc(sizeof(struct Node));
		tmp = tmp->next;
		strcpy(tmp->name,name);
                strcpy(tmp->pw,pw);
                //printf("Added:%s,%s\n",tmp->name, tmp->pw);
                tmp->next = 0;
	}
	
	return head;
}


/* Add the name and password to the list
*  Precondition - the user should not be present in the list
*/
struct Node * add_to_list( struct Node * head ,char * name, char * pw){
	if (head == 0){
		head = (struct Node * )malloc(sizeof(struct Node));
		strcpy(head->name,name);
		strcpy(head->pw,pw);
		//printf("Added:%s,%s\n",head->name, head->pw);
		head->next = 0;
	}
	else{
		struct Node * tmp = head;
		//printf("Here\n");
		while (tmp->next != 0){
			tmp = tmp->next;
		}
		tmp->next = (struct Node *)malloc(sizeof(struct Node));
		tmp = tmp->next;
		strcpy(tmp->name,name);
                strcpy(tmp->pw,pw);
                //printf("Added:%s,%s\n",tmp->name, tmp->pw);
                tmp->next = 0;
	}
	update_file(head);
	return head;
}

/* Update the password for a user present in the list
*  Precondition - the user must be present in the list
*/
struct Node * update_password(struct Node * head, char * name, char * pw){
	struct Node * tmp = head;
	while(tmp != 0){
		if (strcmp(tmp->name, name) == 0){
			strcpy(tmp->pw,pw);
			break;
		}
		else{
			tmp = tmp->next;
		}
	}
	update_file(head);
	return head;
}


/* Returns 1 if the user is present in the list,
*  Returns 0 if user is absent from the list.
*/ 
int is_user_present(struct Node * head, char * name){
	struct Node * tmp = head;
	while (tmp != 0){
		if (strcmp(tmp->name, name) == 0){
			return 1;
		}
		tmp = tmp->next;
	}
	
	return 0;
}

/* Returns 1 if its a valid user,
*  Returns 0 if its not a valid user.
*/ 
int is_valid_user(struct Node * head, char * name, char *pw){
	struct Node * tmp = head;
	while (tmp != 0){
		if (strcmp(tmp->name, name) == 0){
			if (strcmp(tmp->pw, pw) == 0){
				return 1;
			}
		}
		tmp = tmp->next;
	}
	
	return 0;
}

/* Display the contents of the list */
void display_list(struct Node * head, char * result){
	struct Node * tmp = head;
	while (tmp != 0){
		//printf("%s %s\n",tmp->name, tmp->pw);
		strcat(result,tmp->name);
		strcat(result,": ");
		strcat(result,tmp->pw);
		strcat(result,"\n");
		tmp = tmp->next;
	}
}

/* Overwrite the file with the contents of the linked list*/
void update_file(struct Node * head){
	FILE *fp;
	fp = fopen("names.txt","w");
	struct Node * tmp = head;
	while (tmp != 0){
		fprintf(fp,"%s,%s\n",tmp->name, tmp->pw);
		tmp = tmp->next;
	}
	fclose(fp);
}	
