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

#define SOCK_BASE TimePass::SockBase

int main(int argc, char **argv) {
    int tcp_sockfd = SOCK_FAILED, udp_sockfd = SOCK_FAILED;
    SOCK_BASE::OptionVal optval;
    socklen_t len = 0;
    tcp_sockfd = TimePass::SockBase::Socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sockfd < 0) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    udp_sockfd = TimePass::SockBase::Socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd < 0) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    }

    if (false ==SOCK_BASE::GetSockOpt(tcp_sockfd, SOL_SOCKET,
                                      SO_RCVBUF, &optval, &len)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    } else {
        printf("tcp recv_buff size = %d", optval.i_val);
    }

    if (false ==SOCK_BASE::GetSockOpt(tcp_sockfd, SOL_SOCKET,
                                      SO_SNDBUF, &optval, &len)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    } else {
        printf("tcp send_buff size = %d", optval.i_val);
    }

    if (false ==SOCK_BASE::GetSockOpt(udp_sockfd, SOL_SOCKET,
                                      SO_RCVBUF, &optval, &len)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    } else {
        printf("udp recv_buff size = %d", optval.i_val);
    }

    if (false ==SOCK_BASE::GetSockOpt(udp_sockfd, SOL_SOCKET,
                                      SO_SNDBUF, &optval, &len)) {
        printf("%s\n", TimePass::Error::GetLastErrmsg().c_str());
        exit(1);
    } else {
        printf("udp send_buff size = %d", optval.i_val);
    }

    return 0;
}
