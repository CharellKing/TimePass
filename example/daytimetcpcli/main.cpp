/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-19
 * Description: daytime client
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

int main(int argc, char **argv)
{
    int     sockfd;
    char    recvline[MAX_LINE + 1];
    struct sockaddr_in servaddr;
    size_t len = 0;

    if (argc != 2) {
        printf("usage: daytimetcpcli <IPaddress>\n");
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

    if (TimePass::SockBase::Connect(sockfd, (sockaddr*)& servaddr, sizeof(sockaddr)) == false) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }


    while (TimePass::SockBase::Read(sockfd, recvline, MAX_LINE, &len)) {
        recvline[len] = 0;        /* null terminate */
        printf("%s\n", recvline);
    }
    exit(0);
}
