#include"csapp.h"
#include"csapp.c"

#define SIGN_IN 1
#define SIGN_UP 2
#define S_CHAT 3
#define CHAT_MSG 4
#define CLOSE_CHAT 5
#define CLIENT_EXIT 6

void printf_online_user(char *);

int max(int n1,int n2)
{
    return n1>n2?n1:n2;
}

void receive_server(char *buf,int *stat)
{
    char flag[MAXLINE];
	char buf1[MAXLINE];
	printf("line1:%s\n",buf);
    if(*stat==1){// chating
        sscanf(buf,"%s %s",buf1,flag);
        if(strcmp(flag,"exit")==0||strcmp(flag,"close")==0)
            *stat=2;
        else
            printf("%s\n",buf);
    }
    else{
        sscanf(buf,"%s",flag);
        if(strcmp(flag,"online:")==0){
            printf_online_user(buf);
        }
        else if(strcmp(flag,"connected!")==0){
            *stat=1;
            printf("chatting now!\n\n\n");
        }
        else{
        printf("%s\n",buf);
        }
    }

}


void receive_stdin(char *buf,int connfd,int *stat,const char *account)
{
    char line[MAXLINE];
	char line1[MAXLINE];
	printf("line2:%s\n",buf);

    if(*stat==1){
		sscanf(buf,"%s",line);
		if(strcmp(line,"exit")==0)
			*stat=3;
		if(strcmp(line,"close")==0){
			*stat=2;
		}
        sprintf(line,"%d %s %s",4,account,buf);
        rio_writen(connfd,line,strlen(line));
    }
    else{
        sscanf(buf,"%s",line);
        if(strcmp(line,"list")==0){
            sprintf(line,"%d \n",7);
            rio_writen(connfd,line,strlen(line));
        }
        else if(strcmp(line,"exit")==0){
            sprintf(line,"%d \n",6);
            rio_writen(connfd,line,strlen(line));
            *stat=3;
        }
        else{
			printf("--%s\n",line);
			printf("%s\n",account);
            sprintf(line1,"%d %s %s\n",3,account,line);
            rio_writen(connfd,line1,strlen(line1));
        }
    }
}


void str_cli(FILE *fp,int fd,const char * account)
{
    int stat=0;
    int maxfdp1,stdineof;
    fd_set rset;
    char buf1[MAXLINE],buf2[MAXLINE];
    int n;
    rio_t rio;
    rio_t rio_in;
    rio_readinitb(&rio_in,fileno(fp));
    rio_readinitb(&rio,fd);

    stdineof=0;
    FD_ZERO(&rset);

    while(1){
        if(stdineof==0)
            FD_SET(fileno(fp),&rset);
        FD_SET(fd,&rset);
        maxfdp1=max(fileno(fp),fd)+1;
		if(stat==2){
			stat=0;
			printf("back to here!\n");
		}
        if(stat==3)
            break;
        select(maxfdp1,&rset,NULL,NULL,NULL);

        if(FD_ISSET(fd,&rset)){// receive data from server
            if((n=rio_readlineb(&rio,buf1,MAXLINE))==0){
               if(stdineof==1)
                   return;
               else{
                   printf("server terminate prematurely\n");
                   exit(0);
               }
            }
//            rio_writen(fileno(stdout),buf1,n);
//              printf("%s\n",buf1);
//
            receive_server(buf1,&stat);
        }
        if(FD_ISSET(fileno(fp),&rset)){//receive data from stdin
            if((n=rio_readlineb(&rio_in,buf2,MAXLINE))==0){
                stdineof=1;
                shutdown(fd,SHUT_WR);
                FD_CLR(fileno(fp),&rset);
                continue;
            }
//            rio_writen(fd,buf2,n);
            receive_stdin(buf2,fd,&stat,account);
        }

    }


}


char read_char(int fd)
{
    char c;
    char buf[MAXLINE];
    rio_t rio;
    rio_readinitb(&rio,fd);
    rio_readlineb(&rio,buf,MAXLINE);
    sscanf(buf,"%c",&c);
    return c;
}

void read_string(char * dst,int fd)
{
    char buf[MAXLINE];
    rio_t rio;
    rio_readinitb(&rio,fd);
    rio_readlineb(&rio,buf,MAXLINE);
    sscanf(buf,"%s",dst);
}

void printf_online_user(char *dst)
{
    char *tmp;
    while(*dst!='\n'){
        if(*dst==' ')
            dst++;
        else{
            tmp=dst;
            while(*dst!=' ')
                dst++;
            if(*dst=='\n'){
                printf("%s",tmp);
                break;
            }
            *dst=0;
            dst++;
            printf("%s\n",tmp);
        }
    }
}




int main()
{
    int clientfd;
    rio_t rio;
    char i_u='0';
    char buf[MAXLINE];
    char buf1[MAXLINE];
    int flag=0;
    char account[11];
    char pwd[21];
    clientfd=open_clientfd("127.0.0.1",15177);
    rio_readinitb(&rio,clientfd);

    while(1){
        flag=0;
        printf("\n\n\n");
        printf("                welcome to tiny QQ!\n");
        printf("\n\n\n");
        printf("        sign in (press 'i') or sign up (press 'u') ");
        printf("\n\n\n");
        i_u=read_char(fileno(stdin));
        while(i_u!='i'&&i_u!='u'){
            printf("input wrong!,input again1!\n");
            i_u=read_char(fileno(stdin));
        }

        while(flag!=1){//  sign in or sign up
            if(i_u=='i'){
                printf("sign_in:please input your account.(then press ENTER to continue)\n");
                read_string(account,fileno(stdin));
                printf("sign_in:please input your password.(then press ENTER to continue)\n");
                read_string(pwd,fileno(stdin));
                sprintf(buf,"%d %s %s\n",SIGN_IN,account,pwd);
                rio_writen(clientfd,buf,strlen(buf));
                rio_readlineb(&rio,buf,MAXLINE);
                if(strcmp(buf,"no_account\n")==0){
                    printf("The account you input is not exist,input your account agin (press 'i') or turn to sign up (press 'u')\n");
                    i_u=read_char(fileno(stdin));
                    while(i_u!='i'&&i_u!='u'){
                        printf("input wrong!,input again!2\n");
                        i_u=read_char(fileno(stdin));
                    }
                }
                else if(strcmp(buf,"pwd_wrong\n")==0){
                    printf("The password is wrong, input your account and password again!\n");
                }
                else if(strcmp(buf,"ok\n")==0)
                    flag=1;
				else if(strcmp(buf,"al_online\n")==0){
					printf("The account is online already!\n");				
				}
            }
            if(i_u=='u'){
                printf("sign_up:please input your account.(then press ENTER to continue)\n");
                read_string(account,fileno(stdin));
                printf("%s\n",account);
                printf("sign_up:please input your password.(then press ENTER to continue)\n");
                read_string(pwd,fileno(stdin));
                printf("%s\n",pwd);
                sprintf(buf,"%d %s %s\n",SIGN_UP,account,pwd);
                printf("%s\n",buf);
                rio_writen(clientfd,buf,strlen(buf));
                rio_readlineb(&rio,buf,MAXLINE);
                printf("reachhere %s\n",buf);
                if(strcmp(buf,"acc_al_exist\n")==0){
                    printf("The account you input is exist, continue to sign up (press 'u') or turn to sign in (press i)\n");
                    i_u=read_char(fileno(stdin));
                    while(i_u!='i'&&i_u=='u'){
                        printf("input wrong!,input again!3\n");
                        i_u=read_char(fileno(stdin));
                    }
                }
                if(strcmp(buf,"ok\n")==0)
                    flag=1;
            }
        }
        // already sign in
        i_u='0';
        flag=0;
        printf("sign in success!\n");

        str_cli(stdin,clientfd,account);
		sprintf(buf1,"6\n");
		rio_writen(clientfd,buf1,strlen(buf1));
//        printf("online_user:\n");
//        rio_readlineb(&rio,buf,MAXLINE);

//        printf_online_user(buf);
//        printf("%s\n",buf);

//        printf("who do you want to talk? input his/her name,or input 'menu' to back to menu--\n");
//        read_string(buf,fileno(stdin));

//        if(strcmp(buf,"menu")==0)
//            continue;

//        sprintf(buf1,"%d %s %s\n",3,account,buf);

		//printf("%s\n",buf1);
//        rio_writen(clientfd,buf1,strlen(buf1));
//        str_cli(stdin,clientfd);










    }






//    str_cli(stdin,clientfd);
//    exit(0);



}
