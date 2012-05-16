#ifndef _ip_factory_H
#define _ip_factory_H

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

typedef struct ip ipheader_t;
typedef struct udphdr udpheader_t;
typedef struct icmphdr icmpheader_t;

/* checksum function for IP header only */
unsigned short csum(unsigned short *buf, int nwords);

/* returns the source ip address */
int getmyip(char *out);

/* fills a icmp header */
/* rest is expected to already be properly htoned */
int fill_icmp_header(icmpheader_t *header, uint8_t type, uint8_t code, uint32_t rest, int numbytes);

/* fills a udp header */
int fill_udp_header(udpheader_t *header, unsigned short src, unsigned short dst, int numbytes);

/* sends an ip packet with the payload in buf */
/* the header is expected to have the following already filled:
   -IP protocol (ip_p)
   -IP source (ip_src)
   -IP destination (ip_dst)
*/
int send_ip_packet(ipheader_t *header, char *buf, int numbytes);

#endif
