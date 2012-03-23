#ifndef _ip_factory_H
#define _ip_factory_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

typedef struct ip ipheader_t;
typedef struct udphdr udpheader_t;

/* checksum function for IP header only */
unsigned short csum(unsigned short *buf, int nwords);

/* returns the source ip address */
int getmyip(char *out);

/* fills a udp header */
int fill_udp_header(udpheader_t *header, unsigned short src, unsigned short dst, int numbytes);

/* sends an ip packet with the payload in buf */
int send_ip_packet(ipheader_t *header, char *buf, int numbytes);

#endif
