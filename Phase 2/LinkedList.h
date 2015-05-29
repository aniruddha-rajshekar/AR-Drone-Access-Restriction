#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct Node{
        char name[123];
        char pw[123];
        struct Node * next;
};

struct Node * init_linked_list(struct Node * head);
void get_credentials(char * name, char * pw, char * line);
struct Node * update_password(struct Node * head, char * name, char * pw);
struct Node * add_to_list(struct Node * head, char * name, char * pw);
struct Node * load_users(struct Node * head, char * name, char * pw);
int is_user_present(struct Node * head, char * name);
int is_valid_user(struct Node * head, char * name, char *pw);
void display_list(struct Node * head, char * result);
void update_file(struct Node *head);

