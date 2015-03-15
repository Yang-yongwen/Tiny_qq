#ifndef SERVER_H
#define SERVER_H
#include "csapp.h"

#define SIGN_IN 1
#define SIGN_UP 2
#define S_CHAT 3
#define CHAT_MSG 4
#define CLOSE_CHAT 5
#define CLIENT_EXIT 6
#define SENT_ONLINE_USER 7

#define PORT 15177

typedef struct {
    int maxfd;
    fd_set read_set;
    fd_set ready_set;
    int nready;
    int maxi;
    int clientfd[FD_SETSIZE];
    rio_t clientrio[FD_SETSIZE];
}pool;



void init_pool(int listenfd,pool *p);
void add_client(int connfd,pool *p);
void check_clients(pool *p,int info_fd,struct user_info_list *u_list,struct online_user_list *o_list,struct chat_list *c_list);
void sign_up(const char *account,const char *pwd, int fd,int info_fd,struct user_info_list *u_list,struct online_user_list *o_list);
void sign_in(const char *account,const char *pwd,int fd,struct user_info_list *u_list,struct online_user_list *o_list);
void start_chat(struct online_user_list *o_list,struct chat_list *c_list,const char *a1,const char *a2,int fd1);
void chat_msg(struct chat_list *c_list,const char *account,const char *msg);
void client_close(struct chat_list *c_list ,int fd);
void client_exit(struct chat_list *c_list,struct online_user_list *o_list,int fd);
void process(char *buf,int connfd,int info_fd,struct chat_list *c_list,struct online_user_list *o_list,struct user_info_list *u_list);

#endif
