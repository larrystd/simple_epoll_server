/*
 * @Author: your name
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /epoll_server-master/webserver/HttpServer.cpp
 */
#include "HttpServer.h"

void HttpServer::start(int port) {
    if((listen_fd=socket(AF_INET, SOCK_STREAM, 0)) < 0) // return fd of socket
    {
        cout << "socket error\n" << endl;
        exit(-1);
    }
    bzero(&serverSocketaddr, sizeof(struct sockaddr_in));
    serverSocketaddr.sin_family = AF_INET;
    serverSocketaddr.sin_port = htons(port);
    serverSocketaddr.sin_addr.s_addr = htonl(0);

    if (::bind(listen_fd, (sockaddr*)&serverSocketaddr, sizeof(serverSocketaddr)) != 0)
    {
        cerr << "Error: failed to bind port (" << port << ")!" << endl;
        exit(-1);
    }

    if (listen(listen_fd, 5) < 0)
    {
        cerr << "Error: failed to listen!!!" << endl;
        exit(-1);
    }

    epoll_fd = epoll_create(MAXEVENT);   // create epoll
    struct epoll_event event;
    event.events = EPOLLIN|EPOLLET;
    event.data.fd = listen_fd;

    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_fd,&event)<0)    // 注册listen_fd
    {
        printf("epoll 加入失败 fd:%d\n",listen_fd);
        exit(-1);
    }
    //ret会返回在规定的时间内获取到IO数据的个数，并把获取到的event保存在eventList中，注意在每次执行该函数时eventList都会清空，由epoll_wait函数填写。
    for (;;) {
        ret = epoll_wait(epoll_fd, eventList, MAXEVENT, -1); // Returns the number of triggered events returned in "events" buffer
        for (int n = 0; n < ret; ++n) { //
            if (eventList[n].data.fd == listen_fd) {    // 如果文件描述符是socket对应的，需要处理socket
                bzero(&clientSocketaddr, sizeof(struct sockaddr_in));   // 置零
                socklen_t len = sizeof(clientSocketaddr);
                int connd = accept(listen_fd,(struct sockaddr *) &clientSocketaddr, &len);  //接受连接
                if (connd== -1) {
                    perror("accept");
                    exit(-1);
                }
                if ((fcntl(connd, F_SETFL, fcntl(connd, F_GETFD, 0) | O_NONBLOCK))>0)   // 给文件设置标志，不阻塞，也就是没可读数据返回-1
                {
                    perror("set blocking fail");
                    exit(-1);
                }
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = connd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connd,&event) == -1) {   // 注册connd到epoll_ctl
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            } else {
                handle(eventList[n].data.fd);   // 处理读写文件fd，也就是connd
            }
        }
    }
}

void HttpServer::handle(int clientfd)
{
     int recvLen=0;
     char recvBuf[MAX_DATA_SIZE];
     memset(recvBuf,0,sizeof(recvBuf));
     recvLen=recv(clientfd,(char *)recvBuf,MAX_DATA_SIZE,0);    // 向connd内写消息
     if(recvLen==0)
         return;
     else if(recvLen<0)
     {
         perror("recv Error");
       exit(-1);
     }
    //各种处理
    printf("接收到的数据:%s \n",recvBuf);
    return;
 }