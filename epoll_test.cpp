#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define __DEBUG__ 1
#define DEBUG(argv, format...)do{\
    if (__DEBUG__){\
        fprintf(stderr, argv, ##format);\
    }\
}while(0)

#define MAXLINE 10000
#define OPEN_MAX 100
#define LISTENQ 20
#define INFTIM 1000

int setnoblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0)
    {
        perror("fcntl(sock, GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock, SETFL, opts)");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    int i, maxi, listenfd, connfd, sockfd, epfd, nfds;
    ssize_t n;
    char line[MAXLINE];
    socklen_t client;
    struct epoll_event ev, events[20];
    epfd = epoll_create(256);
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setnoblocking(listenfd);

    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    char local_addr[] = "127.0.0.1";
    inet_aton(local_addr, &(serveraddr.sin_addr));
    serveraddr.sin_port = htons(port);

    bind(listenfd, (sockaddr *)&serveraddr, sizeof(serveraddr));
    
    listen(listenfd, LISTENQ);

    maxi = 0;
    for(;;)
    {
        nfds = epoll_wait(epfd, events, 20, 500);
        for(i = 0; i < nfds; ++i)
        {
            DEBUG("for[%d]\n", i);
            DEBUG("data.fd[%d], listenfd[%d]\n", events[i].data.fd, listenfd);
            if (events[i].data.fd == listenfd)
            {
                DEBUG("accept. nfds[%d]\n", nfds);
                connfd = accept(listenfd, (sockaddr *)&clientaddr, &client);
                if (connfd < 0)
                {
                    perror("connfd<0");
                    exit(1);
                }
                setnoblocking(connfd);
                char *str = inet_ntoa(clientaddr.sin_addr);
                DEBUG("connfd[%d],connect from:%s\n", connfd, str);

                ev.data.fd = connfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            else if(events[i].events & EPOLLIN)
            {
                DEBUG("read. nfds[%d]\n", nfds);
                if ((sockfd = events[i].data.fd) < 0)
                {
                    continue;
                }
                if ((n = read(sockfd, line, MAXLINE)) < 0)
                {
                    if (errno = ECONNRESET)
                    {
                        close(sockfd);
                        events[i].data.fd = -1;
                    }
                    else
                    {
                        printf("read line error!\n");
                    }
                }
                else if(0 == n)
                {
                    close(sockfd);
                    events[i].data.fd = -1;
                    DEBUG("fd[%d] exits!\n", sockfd);
                }
                DEBUG("read[%s]\n", line);
                ev.data.fd = sockfd;
                ev.events = EPOLLOUT | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            }
            else if(events[i].events & EPOLLOUT)
            {
                DEBUG("write. nfds[%d]\n", nfds);
                sockfd = events[i].data.fd;
                char ret_buf[MAXLINE];
                line[n] = 0;
                snprintf(ret_buf, MAXLINE, "fd[%d]:%s%c%c", sockfd, line, 30, 0);
                DEBUG("line: %s, ret_buf:%s\n", line, ret_buf);
                n = strlen(ret_buf);
                write(sockfd, ret_buf, n);

                ev.data.fd = sockfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            }
        }
    }
}
