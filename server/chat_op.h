#ifndef CHAT_OP_H
#define CHAT_OP_H

#include"csapp.h"



struct online_user{
    char account[10];
    int fd;
    struct online_user *next;
};


struct chat_node{
    char account1[10];
    int fd1;
    char account2[10];
    int fd2;
    struct chat_node *next;
};

struct online_user_list{
    struct online_user *head,*tail;
};

struct chat_list{
    struct chat_node *head,*tail;
};

void online_user_init(struct online_user_list *list);
void online_user_add(struct online_user_list *list,const char *account,int fd);
void printf_online_user(struct online_user_list *list);
void sent_online_user(struct online_user_list *list,int fd);
void online_user_rm(struct online_user_list *list,int fd);
void chat_list_init(struct chat_list *list);
void chat_list_add(struct chat_list *list,const char *account1,int fd1,const char *account2,int fd2);
void printf_chat_list(struct chat_list *list);
void chat_list_rm(struct chat_list *list,int fd);
int is_online(struct online_user_list *list,const char * account);
int find_fd(struct online_user_list *list,const char * account);
int find_other_fd(struct chat_list *list,const char *account);
void sent_message(struct chat_list *list,const char *account,const char *msg);
#endif
