#include "listener.h"


#define MAX_MESSAGELEN 1024

int start_udp_listener(unsigned short port, receive_handler_t handler)
{	
	/* Open UDP socket */
	int sd;
	sd = socket(AF_INET, SOCK_DGRAM, 0);
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

	/* bind to listen on specified port */
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	int binder = bind(sd, (struct sockaddr*) &my_addr, sizeof(struct sockaddr_in));
	if(binder < 0)
	{
		perror("bind()");
		printf("bind() call - FAIL\n");
		return -1;
	}
	else
	{
		printf("bind() call - SUCCESS\n");
	}


	char buf[MAX_MESSAGELEN];
	int numread;
	struct sockaddr_in src_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	printf("Listening...\n");

	while(1)
	{
		memset(buf, 0, MAX_MESSAGELEN);
		numread = recvfrom(sd, buf, MAX_MESSAGELEN, 0, (struct sockaddr*) &src_addr, &addrlen);
		if(sd < 0)
		{
			perror("recvfrom()");
			printf("recvfrom() call - FAIL\n");
			return -1;
		}

		if(handler(buf, (size_t) numread, &src_addr, addrlen) < 0)
			fprintf(stderr, "Handler call - FAIL\n");
	}

	return 0;
}

int print_packet (char *message, size_t messagelen, struct sockaddr_in *src_addr, socklen_t addrlen)
{
	if(message == NULL || src_addr == NULL)
		return -1;

	printf("%s:%u->%s\n", inet_ntoa(src_addr->sin_addr), ntohs(src_addr->sin_port), message);
}
