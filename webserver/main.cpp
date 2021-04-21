/*
 * @Author: your name
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: /epoll_server-master/webserver/main.cpp
 */
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "HttpServer.h"

using namespace std;

int main(int argc, char *argv[])
{
    int port = 12345;

    HttpServer *server = new HttpServer();

    server->start(port);  // 单线程

    delete server;
    return 0;
}