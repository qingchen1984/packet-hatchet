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
#include <pcap.h>

/* initiates the packet listener which listens for incoming packets
   and prints their contents
*/
int start_listener(char *filter);

#endif
