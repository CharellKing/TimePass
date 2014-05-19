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
/*创建套接字*/
int Socket(int family, int type, int protocol);

/*连接*/
bool Connect(int sockfd, const struct sockaddr* p_addr, socklen_t addrlen);

/*绑定*/
bool Bind(int sockfd, struct sockaddr* p_myaddr, socklen_t addrlen);

/*监听*/
bool Listen(int sockfd, int backlog);

/*接收*/
int Accept(int sockfd, struct sockaddr* p_cliaddr, socklen_t* p_addrlen);

/*获取本地sock的协议地址*/
bool GetSockName(int sockfd,
                 struct sockaddr* p_localaddr,
                 socklen_t* p_addrlen);

/*获取远程sock的协议地址*/
bool GetPeerName(int sockfd, struct sockaddr* p_peeraddr, socklen_t* p_addrlen);

/*读取数据*/
bool Read(int sockfd, void* p_buff, size_t max_size, size_t* p_len);

/*写数据*/
bool Write(int sockfd, const void* p_buff, size_t max_len, size_t* p_len);

/*将主机地址转化为网络地址*/
int Hton(int af, const char* p_host, void* p_dst);

/*关闭套接字*/
bool Close(int sockfd);

}; /*namespace SockBase*/
}; /*namespace TimePass*/

#endif /* SOCK_BASE_H_ */
