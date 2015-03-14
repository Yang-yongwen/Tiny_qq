#include "csapp.h"
#include "file_operate.c"
#include "chat_op.c"
#include "csapp.c"
#define SIGN_IN 1
#define SIGN_UP 2
#define S_CHAT 3
#define CHAT_MSG 4
#define CLOSE_CHAT 5
#define CLIENT_EXIT 6
#define SENT_ONLINE_USER 7

#define PORT 15177


void sign_up(const char *account,const char *pwd, int fd,int info_fd,struct user_info_list *u_list,struct online_user_list *o_list)
{
    char buf[MAXLINE];
    struct user_info info;
    memcpy(info.account,account,strlen(account)+1);
    memcpy(info.pwd,pwd,strlen(pwd)+1);
    if((check_info(u_list,&info))!=NO_ACCOUNT){
        sprintf(buf,"acc_al_exist\n");
        rio_writen(fd,buf,strlen(buf));
    }
	else{
		sprintf(buf,"ok\n");
		rio_writen(fd,buf,strlen(buf));
    	user_info_list_append(u_list,&info);
    	user_info_file_update(&info,info_fd);
		online_user_add(o_list,account,fd);
	}
}
void sign_in(const char *account,const char *pwd,int fd,struct user_info_list *u_list,struct online_user_list *o_list)
{
    char buf[MAXLINE];
    struct user_info info;
	printf("%d--%d\n",(int)strlen(account),(int)strlen(pwd));
    memcpy(info.account,account,strlen(account)+1);
    memcpy(info.pwd,pwd,strlen(pwd)+1);
	printf("%s--%s\n",info.account,info.pwd);

    int ret=check_info(u_list,&info);
    if(ret==NO_ACCOUNT){
        sprintf(buf,"no_account\n");
        rio_writen(fd,buf,strlen(buf));
        return;
    }
    if(ret==PWD_WRONG){
        sprintf(buf,"pwd_wrong\n");
        rio_writen(fd,buf,strlen(buf));
        return;
    }
    if(ret==OK){
		if(is_online(o_list,account)){
			sprintf(buf,"al_online\n");
        	rio_writen(fd,buf,strlen(buf));
			return;		
		}
        printf_online_user(o_list);
        sprintf(buf,"ok\n");
        rio_writen(fd,buf,strlen(buf));
        online_user_add(o_list,account,fd);
        printf_online_user(o_list);
//		sent_online_user(o_list,fd);
        return;
    }
}

void start_chat(struct online_user_list *o_list,struct chat_list *c_list,const char *a1,const char *a2,int fd1)
{
    char buf[MAXLINE];
    int fd2;
    if(!(is_online(o_list,a2))){
        sprintf(buf,"%s is not online\n",a2);
        rio_writen(fd1,buf,strlen(buf));
        return;
    }

    fd2= find_fd(o_list,a2);
    printf_chat_list(c_list);
    chat_list_add(c_list,a1,fd1,a2,fd2);
    printf_chat_list(c_list);
	sprintf(buf,"connected! now chat with %s\n",a2);
	rio_writen(fd1,buf,strlen(buf));
	sprintf(buf,"connected! now chat with %s\n",a1);
	rio_writen(fd2,buf,strlen(buf));
    return;
}

void chat_msg(struct chat_list *c_list,const char *account,const char *msg)
{
    sent_message(c_list,account,msg);
}

void client_close(struct chat_list *c_list ,int fd)
{
    chat_list_rm(c_list,fd);
    printf_chat_list(c_list);
}


void client_exit(struct chat_list *c_list,struct online_user_list *o_list,int fd)
{
	chat_list_rm(c_list,fd);
	online_user_rm(o_list,fd);
}


void process(char *buf,int connfd,int info_fd,struct chat_list *c_list,struct online_user_list *o_list,struct user_info_list *u_list)
{
    int sym;
    char buf1[MAXLINE],buf2[MAXLINE];

    sscanf(buf,"%d%s%s",&sym,buf1,buf2);
	printf("%d--%s--%s\n",sym,buf1,buf2);

    switch(sym){
        case SIGN_UP:
            sign_up(buf1,buf2,connfd,info_fd,u_list,o_list);
            break;
        case SIGN_IN:
            sign_in(buf1,buf2,connfd,u_list,o_list);
            break;
        case S_CHAT:
            start_chat(o_list,c_list,buf1,buf2,connfd);
            break;
        case CHAT_MSG:
            printf("reach here\n");
            //chat_msg(c_list,buf1,buf2);
			chat_msg(c_list,buf1,buf+strlen(buf1)+3);
            break;
        case CLOSE_CHAT:
            client_close(c_list,connfd);
            break;
		case SENT_ONLINE_USER:
			sent_online_user(o_list,connfd);
			break;
		case CLIENT_EXIT:
			client_exit(c_list,o_list,connfd);
        default:
			printf("test\nuser_info_list:\n");
			printf_user_info(u_list);
			printf("online_user:\n");
			printf_online_user(o_list);
            printf("chat_list:\n");
            printf_chat_list(c_list);
            break;
    }
}












typedef struct {
    int maxfd;
    fd_set read_set;
    fd_set ready_set;
    int nready;
    int maxi;
    int clientfd[FD_SETSIZE];
    rio_t clientrio[FD_SETSIZE];
}pool;



void init_pool(int listenfd,pool *p)
{
    int i;
    p->maxi=-1;
    for(i=0;i<FD_SETSIZE;i++)
        p->clientfd[i]=-1;
    p->maxfd=listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd,&p->read_set);
}

void add_client(int connfd,pool *p)
{
    int i;
    p->nready--;
    for(i=0;i<FD_SETSIZE;i++){
        if(p->clientfd[i]<0){
            p->clientfd[i]=connfd;
//            rio_readinitb(&p->clientrio[i],connfd);
            FD_SET(connfd,&p->read_set);
            if(connfd>p->maxfd)
                p->maxfd=connfd;
            if(i>p->maxi)
                p->maxi=i;
            break;
        }
    }
    if(i==FD_SETSIZE)
        printf("too many client!\n");
}

void check_clients(pool *p,int info_fd,struct user_info_list *u_list,struct online_user_list *o_list,struct chat_list *c_list)
{
    int i,connfd,n;
    char buf[MAXLINE];
    rio_t rio;

    for(i=0;(i<=p->maxi)&&(p->nready>0);i++){
        connfd=p->clientfd[i];
//        rio=p->clientrio[i];
		rio_readinitb(&rio,connfd);

        if((connfd>0)&&(FD_ISSET(connfd,&p->ready_set))){
            p->nready--;
            if((n=rio_readlineb(&rio,buf,MAXLINE))!=0){
                process(buf,connfd,info_fd,c_list,o_list,u_list);
            }
			else{
            	client_exit(c_list,o_list,connfd);
        	}
        }

    }
}










int main()
{
    int listenfd,connfd,info_fd;
    socklen_t clientlen=sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    static pool pool;

    listenfd=open_listenfd(PORT);
    init_pool(listenfd,&pool);

    struct user_info_list u_list;
    struct online_user_list o_list;
    struct chat_list c_list;


    info_fd=open("info.txt",O_CREAT|O_RDWR|O_APPEND,0);

    user_info_list_init(&u_list,info_fd);
    chat_list_init(&c_list);
    online_user_init(&o_list);

    printf_user_info(&u_list);




    while(1){
        pool.ready_set=pool.read_set;
        pool.nready=select(pool.maxfd+1,&pool.ready_set,NULL,NULL,NULL);
        if(FD_ISSET(listenfd,&pool.ready_set)){
            connfd=accept(listenfd,(SA *)&clientaddr,&clientlen);
            add_client(connfd,&pool);
        }

        check_clients(&pool,info_fd,&u_list,&o_list,&c_list);
    }







}
