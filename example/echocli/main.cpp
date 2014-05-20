/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-19
 * Description: echo client
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

#define PORT 45678   /*端口号*/

#define SA reinterpret_cast<sockaddr*>

void EchoSvr(int sockfd, FILE* fp) {
    char sendbuff[MAX_LINE] = {0}, recvbuff[MAX_LINE] = {0};
    size_t n = 0;
    while (fgets(sendbuff, MAX_LINE - 1, fp)) {
        if (false == TimePass::SockBase::Write(sockfd, sendbuff,
                                               strlen(sendbuff), NULL)) {
            printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        }

        if (false == TimePass::SockBase::Read(sockfd, recvbuff,
                                              MAX_LINE, &n)) {
            printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        } else {
            recvbuff[n] = '\0';
            printf("%s", recvbuff);
        }
    }
}

int main(int argc, char **argv) {
    int     sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) {
        printf("usage: echocli <IPaddress>\n");
        exit(1);
    }

    sockfd = TimePass::SockBase::Socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    TimePass::Util::Zero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);  /* daytime server */

    if (TimePass::SockBase::Hton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    if (false == TimePass::SockBase::Connect(sockfd,
                                             SA(&servaddr),
                                             sizeof(sockaddr))) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    EchoSvr(sockfd, stdin);
    TimePass::SockBase::Close(sockfd);

    return 0;
}
