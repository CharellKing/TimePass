/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-23
 * Description: echo server
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <list>

#include "global/macro.h"
#include "global/error.h"
#include "sock/sock_base.h"
#include "global/util.h"
#include "sys/sys.h"

#define MAX_LINE 256 /*the max line*/

#define PORT 45678 /*port*/

#define LISTENQ 5 /*max connections*/

#define SA reinterpret_cast<sockaddr*>

#define SOCKBASE TimePass::SockBase

int main(int argc, char **argv) {
    int listenfd = 0, connfd = 0, maxfd = 0, ready_nfd = 0;
    size_t recv_size = 0;
    fd_set rset;
    struct sockaddr_in servaddr;
    char recv_buff[MAX_LINE] = {0};
    std::list<int> fds;

    listenfd = SOCKBASE::Socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    TimePass::Util::Zero(&servaddr, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (false == SOCKBASE::Bind(listenfd,
                                          SA(&servaddr),
                                          sizeof(servaddr))) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    if (false == SOCKBASE::Listen(listenfd, LISTENQ)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }


    maxfd = listenfd;
    FD_ZERO(&rset);
    FD_SET(listenfd, &rset);
    fds.push_back(listenfd);

    while (true) {
        if (false == TimePass::Sys::Select(maxfd + 1, &rset, NULL,
                                           NULL, NULL, &ready_nfd)) {
            printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
            exit(1);
        }

        if (FD_ISSET(listenfd, &rset)) {
            connfd = SOCKBASE::Accept(listenfd, SA(NULL), NULL);
            if (connfd < 0) {
                printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
            } else {
                FD_SET(connfd, &rset);
                fds.push_back(connfd);
                if (connfd > maxfd) {
                    maxfd = connfd;
                }
            }
        } else {
            std::list<int>::iterator iter = fds.begin();
            while (fds.end() != iter) {
                if (FD_ISSET(*iter, &rset)) {
                    if (false == SOCKBASE::Read(connfd, recv_buff,
                                                MAX_LINE, &recv_size)) {
                        printf("%s\n",
                               TimePass::Error::GetLastErrmsg().c_str());
                    } else if (recv_size > 0) {
                        if (false == SOCKBASE::Write(connfd, recv_buff,
                                                     recv_size, NULL)) {
                            printf("%s\n",
                                   TimePass::Error::GetLastErrmsg().c_str());
                        }
                    } else {
                        fds.erase(iter);
                        FD_CLR(*iter, &rset);
                        SOCKBASE::Close(*iter);
                    }
                    break;
                }
                ++iter;
            }
        }
    }
    return 0;
}

