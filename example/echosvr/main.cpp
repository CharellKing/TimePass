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
#include <netinet/in.h>

#include "global/macro.h"
#include "global/error.h"
#include "sock/sock_base.h"
#include "global/util.h"
#include "sys/sys.h"

#define MAX_LINE 256 /*the max line*/

#define PORT 45678 /*port*/

#define LISTENQ 5 /*max connections*/

#define SA reinterpret_cast<sockaddr*>

void EchoClient(int connfd) {
    size_t n = 0;
    char buff[MAX_LINE];

    while (TimePass::SockBase::Read(connfd, buff, MAX_LINE, &n)) {
        if (false == TimePass::SockBase::Write(connfd, buff, n, NULL)) {
            printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        }
    }
}

int main(int argc, char **argv) {
    int     listenfd, connfd;
    struct sockaddr_in servaddr;

    listenfd = TimePass::SockBase::Socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    TimePass::Util::Zero(&servaddr, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (false == TimePass::SockBase::Bind(listenfd,
                                          SA(&servaddr),
                                          sizeof(servaddr))) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    if (false == TimePass::SockBase::Listen(listenfd, LISTENQ)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    while (true) {
        connfd = TimePass::SockBase::Accept(listenfd, SA(NULL), NULL);
        if (connfd < 0) {
            printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        } else {
            pid_t childpid = TimePass::Sys::Fork();
            if (0 == childpid) {
                TimePass::SockBase::Close(listenfd);
                EchoClient(connfd);
            }
        }
    }
    return 0;
}

