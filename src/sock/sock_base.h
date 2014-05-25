/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-16
 */
#ifndef _SOCK_SOCK_BASE_H_
#define _SOCK_SOCK_BASE_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace TimePass {
namespace SockBase {
/**
 * GetSockOpt's optval type
 */
union OptionVal {
    int i_val;
    long l_val;
    struct linger linger_val;
    struct timeval timeval_val;
};

/**
 * create socket
 * @param family: a domain of communication. such as AF_INET and so on
 * @param type: a socket type. such as SOCK_STREAM、SOCK_DGRAM and so on
 * @param protocol: a protocol of socket. gernerally specialized with zero.
 * @return a socket descriptor
 */
int Socket(int family, int type, int protocol);

/**
 * connect to a remote host
 * @param sockfd: a socket has been created
 * @param p_addr: the remote host's address
 * @param addrlen: the address's byte-size
 * @return true for OK, false for failed and get error message by "Error"
 */
bool Connect(int sockfd, const struct sockaddr* p_addr, socklen_t addrlen);

/**
 * bind a port or address
 * @param sockfd: a socket has been created
 * @param p_myaddr: the local host's address
 * @param addrlen: the address's byte-size
 * @return true for OK, false for failed and get error message by "Error"
 */
bool Bind(int sockfd, struct sockaddr* p_myaddr, socklen_t addrlen);

/**
 * listen the connection of a client
 * @param sockfd: a binded socket
 * @param backlog: the size of the max queue size for saving connections
 * @return true for OK, false for failed and get error message by "Error"
 */
bool Listen(int sockfd, int backlog);

/**
 * accept a connection from a client
 * @param sockfd: a listend socket
 * @param p_cliaddr: a known connection address
 * @param p_addrlen: the address size
 * @return a socket from connection
 */
int Accept(int sockfd, struct sockaddr* p_cliaddr, socklen_t* p_addrlen);


/**
 * get local socket address
 * @param sockfd: the sockfd which has listend or accepted or connected
 * @param p_localaddr: the local address
 * @param p_addrlen: the address's byte-size
 * @return true for OK, false for failed and get error message by "Error"
 */
bool GetSockName(int sockfd,
                 struct sockaddr* p_localaddr,
                 socklen_t* p_addrlen);

/**
 * get remote socket address
 * @param sockfd: the sockfd which has listend or accepted or connected
 * @param p_peeraddr: the remote address
 * @param p_addrlen: the address's byte-size
 * @return true for OK, false for failed and get error message by "Error"
 */
bool GetPeerName(int sockfd, struct sockaddr* p_peeraddr, socklen_t* p_addrlen);

/**
 * read data from socket cache
 * @param sockfd: a socket which connection has established
 * @param p_buff: a buffer to read
 * @param max_size: the buffer's capacity
 * @param p_len: the real reading data bytes, it can be set NULL.
 * @return true for OK, false for failed and get error message by "Error"
 */
bool Read(int sockfd, void* p_buff, size_t max_size, size_t* p_len);

/**
 * write data to socket cache
 * @param sockfd: a socket which connection has established
 * @param p_buff: a buff filled data to write
 * @param max_len: the buffer capacity
 * @param p_len: the real write data bytes
 * @return true for OK, false for failed and get error message by "Error"
 */
bool Write(int sockfd, const void* p_buff, size_t max_len, size_t* p_len);

/**
 * convert host address to network address
 * @param af: the family address
 * @param p_host: host address
 * @param p_dst: the network address for output
 * @return true for OK, false for failed and get error message by "Error"
 */
bool Hton(int af, const char* p_host, void* p_dst);

/**
 * close socket
 * @param sockfd: the socket has been established
 * @return true for OK, false for failed and get error message by "Error"
 */
bool Close(int sockfd);

/**
 * close socket; compared with "Close", close send FIN until socket reference to zero,
 * but shutdown can close in half and have nothing with socket reference.
 * @param sockfd the socket will shutdown
 * @param howto  SHUT_RD，close read；SHUT_WR，close write；SHUT_RDWR, close read and write
 * @return true for OK, false for failed and get error message by "Error"
 */
bool Shutdown(int sockfd, int howto);

/**
 * get socket option value
 * @param sockfd: a socket has been created
 * @param level: option level(SOL_SOCKET,IPPROTO_TCP and so on)
 * @param opt: socket option
 * @param optval: the value buffer
 * @param p_vallen: buffer length
 * @return
 */
bool GetSockOpt(int sockfd, int level, int opt, void* optval, socklen_t* p_vallen);

}; /*namespace SockBase*/
}; /*namespace TimePass*/

#endif /* SOCK_BASE_H_ */
