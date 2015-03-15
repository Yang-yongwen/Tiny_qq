#ifndef FILE_OPERATE_H
#define FILE_OPERATE_H

#include "csapp.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

#define OK 1
#define NO_ACCOUNT 2
#define PWD_WRONG 3

struct user_info{
    char account[10];
    char pwd[20];
};
struct user_info_node{
    struct user_info info;
    struct user_info_node *next;
};

struct user_info_list{
    struct user_info_node *head,*tail;
};



struct user_info_node *create_node(const char *account,const char *pwd);
void user_info_list_init(struct user_info_list *list,int fd);
void user_info_list_append(struct user_info_list *list,struct user_info *user_info);
void user_info_file_update(struct user_info *info,int fd);
int check_info(struct user_info_list *list,struct user_info *info);
void printf_user_info(struct user_info_list *list);

#endif
