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
#include <signal.h>


typedef void (*packet_callback_t) (u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

/* prints the contents of a udp packet in a human-readable format */
void print_udp_packet (u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

/* initiates the packet listener which listens for incoming packets
   and prints their contents
*/
int start_listener(char *filter, packet_callback_t callback);

#endif
