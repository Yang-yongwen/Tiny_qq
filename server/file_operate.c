#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include "csapp.h"

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

struct user_info_node *create_node(const char *account,const char *pwd)
{
    struct user_info_node *result=(struct user_info_node *)malloc(sizeof(struct user_info_node));
    result->next=NULL;
    memcpy(result->info.account,account,strlen(account)+1);
    memcpy(result->info.pwd,pwd,strlen(pwd)+1);
    return result;
}

void user_info_list_init(struct user_info_list *list,int fd)
{
    list->head=create_node(" "," ");// make a head_node
    list->tail=list->head;
    int n;
    char buf1[MAXLINE],buf2[MAXLINE],buf[MAXLINE];
    rio_t rio;
    lseek(fd,0,SEEK_SET);

    rio_readinitb(&rio,fd);
    while((n=rio_readlineb(&rio,buf,MAXLINE)!=0)){//read all info
        sscanf(buf,"%s %s",buf1,buf2);
        list->tail=list->tail->next=create_node(buf1,buf2);
    }
}


void user_info_list_append(struct user_info_list *list,struct user_info *user_info)
{

    struct user_info_node *node=create_node(user_info->account,user_info->pwd);
    list->tail=list->tail->next=node;


}

void user_info_file_update(struct user_info *info,int fd)
{
    char buf[MAXLINE];
    sprintf (buf,"%s %s",info->account,info->pwd);
//    printf("%d",(int)strlen(buf));
    strcat(buf,"\n");
//    printf("%d",(int)strlen(buf));
    write(fd,buf,strlen(buf));
}

int check_info(struct user_info_list *list,struct user_info *info)
{
    struct user_info_node *tmp=list->head;
    while((tmp=tmp->next)!=NULL){
        if(strcmp(tmp->info.account,info->account)==0){
            if(strcmp(tmp->info.pwd,info->pwd)==0)
                return OK;
            else
                return PWD_WRONG;
        }
//        if((strcmp(tmp->info.account,info->account)==0)&&(strcmp(tmp->info.pwd,info->pwd)==0))
//            return 1;
    }
    return NO_ACCOUNT;
}

void printf_user_info(struct user_info_list *list)
{
    char buf[MAXLINE];
    struct user_info_node *tmp=list->head;
    while((tmp=tmp->next)!=NULL){
        sprintf(buf,"%s %s",tmp->info.account,tmp->info.pwd);
        printf("%s\n",buf);
    }
}




//int main(int argc, char *argv[])
//{
//
//    int fd,n;
//    char buf1[MAXLINE],buf2[MAXLINE];
//    rio_t rio;
//    struct user_info_list list;
//
////    struct user_info user[2];
//
////    sprintf(buf1,"%s %s\n%s %s\n","1234","5678","4321","8765");
//
//
//
//    fd=open("foo.txt",O_CREAT|O_RDWR|O_APPEND,0);
//    rio_readinitb(&rio,fd);
//
//   struct user_info tmp;//
//   scanf("%s %s",tmp.account,tmp.pwd);////////
///   n=write(fd,buf1,strlen(buf1));
///    lseek(fd,0,SEEK_SET);
///    int i=0;
///    for(i=0;(n=rio_readlineb(&rio,buf2,MAXLINE))!=0;i++){
///        sscanf(buf2,"%s %s",user[i].account,user[i].pwd);
///        printf("%s %s\n",user[i].account,user[i].pwd);
///    }
/////
///   struct user_info_node *tmp=create_node("nihao","moyu");
//  user_info_list_init(&list,fd);
///   user_info_file_update(&tmp->info,fd);
///   user_info_list_init(&list,fd);
///   user_info_list_append(&list,&tmp->info);
///
//  if(check_info(&list,&tmp)){
//   printf("right!\n");
//  }//
//  printf_user_info(&list);//////
//}









