#include"csapp.h"


int max(int n1,int n2)
{
    return n1>n2?n1:n2;
}


void str_cli(FILE *fp,int fd)
{
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
        select(maxfdp1,&rset,NULL,NULL,NULL);

        if(FD_ISSET(fd,&rset)){
            if((n=rio_readlineb(&rio,buf1,MAXLINE))==0){
               if(stdineof==1)
                   return;
               else{
                   printf("server terminate prematurely\n");
                   return;
               }
            }
//            rio_writen(fileno(stdout),buf1,n);
              printf("%s",buf1);
        }
        if(FD_ISSET(fileno(fp),&rset)){
            if((n=rio_readlineb(&rio_in,buf2,MAXLINE))==0){
                stdineof=1;
                shutdown(fd,SHUT_WR);
                FD_CLR(fileno(fp),&rset);
                continue;
            }
            rio_writen(fd,buf2,n);
        }

    }


}






int main(int argc,char **argv)
{
    int clientfd,port;
    char *host,buf[MAXLINE];
    rio_t rio;

//    if(argc!=3){
//        fprintf(stderr,"usage: %s <host><port>\n",argv[0]);
//        exit(0);
//    }
//
//    host=argv[1];
//    port=atoi(argv[2]);

    clientfd=open_clientfd("127.0.0.1",15177);
    rio_readinitb(&rio,clientfd);

//    while(fgets(buf,MAXLINE,stdin)!=NULL){
//        rio_writen(clientfd,buf,strlen(buf));
//        rio_readlineb(&rio,buf,MAXLINE);
//        fputs(buf,stdout);
//    }
//
//    close(clientfd);
//    exit(0);
//


    str_cli(stdin,clientfd);
    exit(0);



}
