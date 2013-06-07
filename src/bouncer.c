#include "bouncer.h"

int bounce_udp_packet (char *message, size_t messagelen, struct sockaddr_in *src_addr, socklen_t addrlen)
{
	if(message == NULL || src_addr == NULL)
		return -1;

	
	printf("%s:%u->%s\n", inet_ntoa(src_addr->sin_addr), ntohs(src_addr->sin_port), message);	
	return 0;
}
