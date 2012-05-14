#ifndef _bouncer_H
#define _bouncer_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

/* initiates the packet bouncer which merely listens for incoming packets
   and reflects them back at the sender
*/
int start_bouncer();

#endif
