/* Code inspired by http://www.tenouk.com/Module43a.html */

#include "ip_factory.h"

#define MAX_PCKT_LEN 8192

unsigned short csum(unsigned short *buf, int nwords)
{
	unsigned long sum;
	for(sum = 0; nwords > 0; nwords--)
		sum += *buf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

	return (unsigned short) (~sum);
}

int getmyip(char *out)
{
	if(out == NULL)
		return -1;

	memcpy(out, "172.12.131.1", sizeof(char) * 10);
	return 0;
}

int fill_udp_header(udpheader_t *header, unsigned short src, unsigned short dst, int numbytes)
{
	if(header == NULL)
		return -1;

	header->source = src;
	header->dest = dst;
	header->len = htons(sizeof(udpheader_t) + numbytes); 
	header->check = 0; /* This is optional and I am lazy, ignore it */

	return 0;
}

int send_ip_packet(ipheader_t *header, char *buf, int numbytes)
{
	int sizeofpacket = sizeof(ipheader_t) + numbytes;
	if(sizeofpacket > MAX_PCKT_LEN)
	{
		printf("Cannot send ip packet of len %i. Too large. - FAIL\n", sizeofpacket);
		return -1;
	}

	/* open a raw socket */
	int sd;
	sd = socket(PF_INET, SOCK_RAW, header->ip_p);
	if(sd < 0)
	{
		perror("socket()");
		printf("socket() call - FAIL\n");
		return -1;
	}
	else
	{
		printf("socket() call - SUCCESS\n");
	}
	
	char packet[sizeofpacket];
	memset(packet, 0, sizeofpacket);

	/* set remaining ip header */
	header->ip_len = sizeofpacket;
	header->ip_sum = csum((unsigned short*)packet, sizeofpacket);
	
	/* fill packet */
	memcpy(packet, (char*) header, sizeof(ipheader_t));
	memcpy(packet + sizeof(ipheader_t), (char*) buf, numbytes);

	/* setup socket addresses */
	struct sockaddr_in sin, din;
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;
	memcpy(&sin.sin_addr.s_addr, &header->ip_src, sizeof(in_addr_t));
	memcpy(&din.sin_addr.s_addr, &header->ip_dst, sizeof(in_addr_t));

	/* send out the packet */
	int one = 1;
	int *val = &one;
	if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)))
	{
		perror("setsockopt()");
		printf("setsockopt() call - FAIL\n");
		return -1;
	}
	else
	{
		printf("setsockopt() call - SUCCESS\n");
	}

	if(sendto(sd, packet, header->ip_len, 0, (struct sockaddr *) &sin, sizeof(sin)) < 0)
	{
		printf("Message sent! - SUCCESS\n");
	}
	else
	{
		perror("sendto()");
		printf("sendto() call - FAIL\n");
		return -1;
	}
	
	return 0;
}
