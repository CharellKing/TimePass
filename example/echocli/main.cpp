/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-19
 * Description: echo client
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <algorithm>

#include "global/macro.h"
#include "global/error.h"
#include "sock/sock_base.h"
#include "global/util.h"
#include "sys/sys.h"

#define MAX_LINE 256 /*最大的行*/

#define PORT 45678   /*端口号*/

#define SA reinterpret_cast<sockaddr*>

void EchoSvr(int sockfd, FILE* fp) {
    char sendbuff[MAX_LINE] = {0}, recvbuff[MAX_LINE] = {0};
    size_t n = 0;
    int maxfd = 0;
    fd_set rset;

    FD_ZERO(&rset);
    while (true) {
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfd = std::max(fileno(fp), sockfd) + 1;


        if (false == TimePass::Sys::Select(maxfd, &rset, NULL, NULL, NULL, NULL)) {
            printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
            break;
        } else {
            if (FD_ISSET(sockfd, &rset)) {
                if (false == TimePass::SockBase::Read(sockfd, recvbuff,
                                                      MAX_LINE, &n)) {
                    printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
                } else if (n > 0){
                    recvbuff[n] = '\0';
                    printf("%s", recvbuff);
                } else {
                    /*服务端子进程断开，会给客户端发送FIN, Select会捕获到读的socket，但是
                     * 读出的数据为空
                     */
                    printf("catch server child pid terminate\n");
                    break;
                }
            } else if (FD_ISSET(fileno(fp), &rset)) {
                if (fgets(sendbuff, MAX_LINE - 1, fp)) {
                    if (false == TimePass::SockBase::Write(sockfd, sendbuff,
                                                           strlen(sendbuff), NULL)) {
                        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
                    }
                } else {
                    /*客户端数据发送完毕，出现两种情况，不能立即关闭：
                     * （1）客户端缓冲区里面的数据还没有发送完毕
                     * （2）服务端的数据还没有完全返回
                     * */

                    TimePass::SockBase::Shutdown(sockfd, SHUT_WR);
                    FD_CLR(fileno(fp), &rset);
                    continue;
                }
            }
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
