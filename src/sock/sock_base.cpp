/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-16
 */

#include "sock/sock_base.h"

#include <errno.h>
#include <arpa/inet.h>

#include "global/error.h"


/*创建套接字*/
int TimePass::SockBase::Socket(int family, int type, int protocol) {
    int sock = socket(family, type, protocol);
    if (sock < 0) {
        SET_ERRNO(errno);
        return -1;
    }
    return socket(family, type, protocol);
}

/*连接*/
bool TimePass::SockBase::Connect(int sockfd,
                                 const struct sockaddr* p_addr,
                                 socklen_t addrlen) {
    if (connect(sockfd, p_addr, addrlen) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*绑定*/
bool TimePass::SockBase::Bind(int sockfd,
                              struct sockaddr* p_myaddr,
                              socklen_t addrlen) {
    if (bind(sockfd, p_myaddr, addrlen) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*监听*/
bool TimePass::SockBase::Listen(int sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*接收*/
int TimePass::SockBase::Accept(int sockfd,
                               struct sockaddr* p_cliaddr,
                               socklen_t* p_addrlen) {
    int acc_sock = accept(sockfd, p_cliaddr, p_addrlen);
    if (acc_sock < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return acc_sock;
}

/*获取本地sock的协议地址*/
bool TimePass::SockBase::GetSockName(int sockfd,
                                     struct sockaddr* p_localaddr,
                                     socklen_t* p_addrlen) {
    if (getsockname(sockfd, p_localaddr, p_addrlen) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*获取远程sock的协议地址*/
bool TimePass::SockBase::GetPeerName(int sockfd,
                                     struct sockaddr* p_peeraddr,
                                     socklen_t* p_addrlen) {
    if (getpeername(sockfd, p_peeraddr, p_addrlen) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*读取数据*/
bool TimePass::SockBase::Read(int sockfd,
                              void* p_buff,
                              size_t max_size,
                              size_t* p_len) {
    int len = read(sockfd, p_buff, max_size);
    if (len < 0) {
        SET_ERRNO(errno);
        return false;
    }

    if (NULL != p_len) {
        *p_len = len;
    }
    return true;
}

/*写数据*/
bool TimePass::SockBase::Write(int sockfd,
                               const void* p_buff,
                               size_t max_size,
                               size_t* p_len) {
    int len = write(sockfd, p_buff, max_size);
    if (len < 0) {
        SET_ERRNO(errno);
        return false;
    }

    if (NULL != p_len) {
        *p_len = len;
    }
    return true;
}


/*将主机地址转化为网络地址*/
int TimePass::SockBase::Hton(int af, const char* p_host, void* p_dst) {
    if (inet_pton(af, p_host, p_dst) <= 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/*关闭套接字*/
bool TimePass::SockBase::Close(int sockfd) {
    if (close(sockfd) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * 关闭socket,与close不同的是，close必须引用为0才能发送FIN.而且它还可以采用半开关闭
 * @param sockfd 要关闭的套节字
 * @param howto  SHUT_RD，关闭读这一半；SHUT_WR，关闭写这一半；SHUT_RDWR，全部关闭
 * @return true - 成功；false - 失败；通过Error可以获取错误信息
 */
bool TimePass::SockBase::Shutdown(int sockfd, int howto) {
    return 0 == shutdown(sockfd, howto);
}
