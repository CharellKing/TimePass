/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-19
 * Description: daytime server
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "global/macro.h"
#include "global/error.h"
#include "sock/sock_base.h"
#include "global/util.h"

#define MAX_LINE 256 /*最大的行*/

#define PORT 45678 /*端口号*/

#define LISTENQ 5 /*监听队列的长度*/

int main(int argc, char **argv)
{
    int     listenfd, connfd;
    struct sockaddr_in servaddr;
    char    buff[MAX_LINE];
    time_t  ticks;

    if (argc != 2) {
        printf("usage: daytimetcpcli <IPaddress>");
    }

    listenfd = TimePass::SockBase::Socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    TimePass::Util::Zero(&servaddr, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (false == TimePass::SockBase::Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr))) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    if (false == TimePass::SockBase::Listen(listenfd, LISTENQ)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    while(true) {
        connfd = TimePass::SockBase::Accept(listenfd, (sockaddr*)NULL, NULL);
        if (connfd < 0) {
            printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        } else {
            ticks = time(NULL);
            snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
            if (false == TimePass::SockBase::Write(connfd, buff, MAX_LINE, NULL)) {
                printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
            }
        }
    }
    return 0;
}
