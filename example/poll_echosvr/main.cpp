/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-19
 * Description: echo server
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <limits.h>

#include "global/macro.h"
#include "global/error.h"
#include "sock/sock_base.h"
#include "global/util.h"
#include "sys/sys.h"

#define MAX_LINE 256 /*the max line*/

#define PORT 45678 /*port*/

#define MAX_POLLFD 256

#define LISTENQ 5 /*max connections*/

#define SA reinterpret_cast<sockaddr*>

#define SOCK_BASE TimePass::SockBase

int main(int argc, char **argv) {
    int     listenfd = 0, connfd = 0, max_index = 0, index = 0;
    size_t recv_size = 0;
    struct sockaddr_in servaddr;
    struct pollfd clients[MAX_POLLFD];
    char recv_buff[MAX_LINE];
    listenfd = SOCK_BASE::Socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    TimePass::Util::Zero(&servaddr, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (false == SOCK_BASE::Bind(listenfd,
                                          SA(&servaddr),
                                          sizeof(servaddr))) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    if (false == SOCK_BASE::Listen(listenfd, LISTENQ)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    clients[0].fd = listenfd;
    clients[0].events = POLLRDNORM;
    for (int index = 1; index < MAX_POLLFD; ++index) {
        clients[index].fd = -1;
    }
    max_index = 0;

    while (true) {
        if (false == TimePass::Sys::Poll(clients, max_index + 1,
                                         INFTIM, NULL)) {
            printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
            exit(1);
        }

        if (clients[0].revents & POLLRDNORM) {
            connfd = SOCK_BASE::Accept(listenfd, SA(NULL), NULL);
            if (connfd < 0) {
                printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
            } else {
                for (index = 1; index < MAX_POLLFD; ++index) {
                    if (-1 == clients[index].fd) {
                        clients[index].fd = connfd;
                        clients[index].events = POLLRDNORM;
                        break;
                    }
                }

                if (max_index < MAX_POLLFD && max_index < index) {
                    max_index = index;
                }
            }
        } else {
            for (index = 1; index <= max_index; ++index) {
                /*服务端可能收到RST，会有错误revents POLLER*/
                if (clients[index].revents & (POLLRDNORM | POLLERR)) {
                    if (false == SOCK_BASE::Read(clients[index].fd, recv_buff,
                                                 MAX_LINE, &recv_size)) {
                        printf("%s\n",
                               TimePass::Error::GetLastErrmsg().c_str());

                        SOCK_BASE::Close(clients[index].fd);
                        clients[index].fd = -1;

                        while (max_index > 0 && -1 == clients[max_index].fd) {
                            --max_index;
                        }

                    } else if (recv_size > 0) {
                        if (false == SOCK_BASE::Write(clients[index].fd,
                                                      recv_buff,
                                                      recv_size,
                                                      NULL)) {
                            printf("%s\n",
                                   TimePass::Error::GetLastErrmsg().c_str());
                        }
                    } else {
                        SOCK_BASE::Close(clients[index].fd);
                        clients[index].fd = -1;

                        while (max_index > 0 && -1 == clients[max_index].fd) {
                            --max_index;
                        }
                    }
                    break;
                }
            }
        }
    }
    return 0;
}

