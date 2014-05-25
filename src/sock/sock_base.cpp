/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-16
 */

#include "sock/sock_base.h"

#include <errno.h>
#include <arpa/inet.h>

#include "global/error.h"

/**
 * create socket
 * @param family: a domain of communication. such as AF_INET and so on
 * @param type: a socket type. such as SOCK_STREAM、SOCK_DGRAM and so on
 * @param protocol: a protocol of socket. gernerally specialized with zero.
 * @return a socket descriptor
 */
int TimePass::SockBase::Socket(int family, int type, int protocol) {
    int sock = socket(family, type, protocol);
    if (sock < 0) {
        SET_ERRNO(errno);
        return -1;
    }
    return socket(family, type, protocol);
}

/**
 * connect to a remote host
 * @param sockfd: a socket has been created
 * @param p_addr: the remote host's address
 * @param addrlen: the address's byte-size
 * @return true for OK, false for failed and get error message by "Error"
 */
bool TimePass::SockBase::Connect(int sockfd,
                                 const struct sockaddr* p_addr,
                                 socklen_t addrlen) {
    if (connect(sockfd, p_addr, addrlen) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * bind a port or address
 * @param sockfd: a socket has been created
 * @param p_myaddr: the local host's address
 * @param addrlen: the address's byte-size
 * @return true for OK, false for failed and get error message by "Error"
 */
bool TimePass::SockBase::Bind(int sockfd,
                              struct sockaddr* p_myaddr,
                              socklen_t addrlen) {
    if (bind(sockfd, p_myaddr, addrlen) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * listen the connection of a client
 * @param sockfd: a binded socket
 * @param backlog: the size of the max queue size for saving connections
 * @return true for OK, false for failed and get error message by "Error"
 */
bool TimePass::SockBase::Listen(int sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * accept a connection from a client
 * @param sockfd: a listend socket
 * @param p_cliaddr: a known connection address
 * @param p_addrlen: the address size
 * @return a socket from connection
 */
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

/**
 * get local socket address
 * @param sockfd: the sockfd which has listend or accepted or connected
 * @param p_localaddr: the local address
 * @param p_addrlen: the address's byte-size
 * @return true for OK, false for failed and get error message by "Error"
 */
bool TimePass::SockBase::GetSockName(int sockfd,
                                     struct sockaddr* p_localaddr,
                                     socklen_t* p_addrlen) {
    if (getsockname(sockfd, p_localaddr, p_addrlen) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * get remote socket address
 * @param sockfd: the sockfd which has listend or accepted or connected
 * @param p_peeraddr: the remote address
 * @param p_addrlen: the address's byte-size
 * @return true for OK, false for failed and get error message by "Error"
 */
bool TimePass::SockBase::GetPeerName(int sockfd,
                                     struct sockaddr* p_peeraddr,
                                     socklen_t* p_addrlen) {
    if (getpeername(sockfd, p_peeraddr, p_addrlen) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * read data from socket cache
 * @param sockfd: a socket which connection has established
 * @param p_buff: a buffer to read
 * @param max_size: the buffer's capacity
 * @param p_len: the real reading data bytes, it can be set NULL.
 * @return true for OK, false for failed and get error message by "Error"
 */
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

/**
 * write data to socket cache
 * @param sockfd: a socket which connection has established
 * @param p_buff: a buff filled data to write
 * @param max_len: the buffer capacity
 * @param p_len: the real write data bytes
 * @return true for OK, false for failed and get error message by "Error"
 */
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


/**
 * convert host address to network address
 * @param af: the family address
 * @param p_host: host address
 * @param p_dst: the network address for output
 * @return true for OK, false for failed and get error message by "Error"
 */
bool TimePass::SockBase::Hton(int af, const char* p_host, void* p_dst) {
    if (inet_pton(af, p_host, p_dst) <= 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * close socket
 * @param sockfd: the socket has been established
 * @return true for OK, false for failed and get error message by "Error"
 */
bool TimePass::SockBase::Close(int sockfd) {
    if (close(sockfd) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * close socket; compared with "Close", close send FIN until socket reference to zero,
 * but shutdown can close in half and have nothing with socket reference.
 * @param sockfd the socket will shutdown
 * @param howto  SHUT_RD，close read；SHUT_WR，close write；SHUT_RDWR, close read and write
 * @return true for OK, false for failed and get error message by "Error"
 */
bool TimePass::SockBase::Shutdown(int sockfd, int howto) {
    if (shutdown(sockfd, howto) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}

/**
 * get socket option value
 * @param sockfd: a socket has been created
 * @param level: option level(SOL_SOCKET,IPPROTO_TCP and so on)
 * @param opt: socket option
 * @param optval: the value buffer
 * @param p_vallen: buffer length
 * @return
 */
bool TimePass::SockBase::GetSockOpt(int sockfd, int level, int opt, void* optval, socklen_t* p_vallen) {
    if (getsockopt(sockfd, level, opt, optval, p_vallen) < 0) {
        SET_ERRNO(errno);
        return false;
    }
    return true;
}
