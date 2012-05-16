/* Code inspired by
	http://www.tenouk.com/Module43a.html
	http://stackoverflow.com/questions/212528/linux-c-get-the-ip-address-of-local-computer
*/

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

	struct ifaddrs *myaddrs;
	struct ifaddrs *curaddr;
	void *temp;

	getifaddrs(&myaddrs);
	
	for(curaddr = myaddrs; curaddr != NULL; curaddr = curaddr->ifa_next)
	{
		/* IPv4 */
		if(curaddr->ifa_addr->sa_family == AF_INET)
		{
			temp = &((struct sockaddr_in*)curaddr->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, temp, out, INET_ADDRSTRLEN);

			/* TODO: How do I know they're using interface eth0? */
			if(strcmp(curaddr->ifa_name, "eth0") == 0)
			       break;	
		}
		/* IPv6 */
		else if(curaddr->ifa_addr->sa_family == AF_INET6)
		{
			temp = &((struct sockaddr_in6*)curaddr->ifa_addr)->sin6_addr;
			inet_ntop(AF_INET6, temp, out, INET6_ADDRSTRLEN);
		}
	}

	if(myaddrs != NULL) freeifaddrs(myaddrs);

	return 0;
}

int fill_icmp_header(icmpheader_t *header, uint8_t type, uint8_t code, uint32_t rest, int numbytes)
{
	if(header == NULL)
		return -1;

	header->type = type;
	header->code = code;
	header->checksum = 0;
	memcpy(((uint32_t*)header) + 1, &rest, sizeof(uint32_t));

	header->checksum = csum((unsigned short*)header, (sizeof(icmpheader_t) + numbytes)/sizeof(short));

	return 0;
}

int fill_udp_header(udpheader_t *header, unsigned short src, unsigned short dst, int numbytes)
{
	if(header == NULL)
		return -1;

	header->source = htons(src);
	header->dest = htons(dst);
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
	header->ip_hl = 5; /* header length is 5 32-bit octets */
	header->ip_v = 4; /* IPv4 */
	header->ip_tos = 16; /* low delay */
	header->ip_len = sizeofpacket;
	header->ip_id = htons(54321); /* identifier used for fragmentation */
	header->ip_off = 0; /* fragmentation options */
	header->ip_ttl = 64; /* max num hops */
	header->ip_sum = 0;
	
	/* fill packet */
	memcpy(packet, (char*) header, sizeof(ipheader_t));
	memcpy(packet + sizeof(ipheader_t), (char*) buf, numbytes);
	
	header->ip_sum = csum((unsigned short*)packet, sizeofpacket / sizeof(short));

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

	if(sendto(sd, packet, header->ip_len, 0, (struct sockaddr *) &din, sizeof(din)) < 0)
	{
		perror("sendto()");
		printf("sendto() call - FAIL\n");
		return -1;
	}
	else
	{
		printf("Message sent! - SUCCESS\n");
	}
	
	return 0;
}
