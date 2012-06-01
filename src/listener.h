#ifndef _listener_H
#define _listener_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <ifaddrs.h>
#include <arpa/inet.h>


typedef int (*receive_handler_t) (char *message, size_t messagelen, struct sockaddr_in *src_addr, socklen_t addrlen);

int print_packet (char *message, size_t messagelen, struct sockaddr_in *src_addr, socklen_t addrlen);

/* initiates the packet listener which listens for incoming packets
   and prints their contents
*/
int start_udp_listener(unsigned short port, receive_handler_t handler);
int start_icmp_listener(receive_handler_t handler);

#endif
