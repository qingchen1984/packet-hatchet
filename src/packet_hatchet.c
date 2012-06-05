#include "packet_hatchet.h"


#define MAX_MESSAGELEN 256
#define FILTER_BUFLEN 20

/*
TODO:
	-unify max message lengths in a config.h?
	-dns resolver support
*/

int main(int argc, char** argv)
{
	int exitstatus = 0;


	/* declare CL args */
	arg_lit_t *help = (arg_lit_t*) arg_lit0("h", "help", "prints the command glossary");
	arg_lit_t *myip = (arg_lit_t*) arg_lit0("m", NULL, "prints the external ip of the interface currently being used");
	arg_lit_t *bounce_opt = (arg_lit_t*) arg_lit0("b", NULL, "creates a bouncer to send the message received back to the sender");
	arg_lit_t *listen_opt = (arg_lit_t*) arg_lit0("l", NULL, "creates a listener to print the messages received");
	
	arg_file_t *proto = (arg_file_t*) arg_filen("p", "protocol", "acronym", 0, 1, "specify the protocol being manipulated");
	arg_file_t *source = (arg_file_t*) arg_filen("s", "source", "x.x.x.x", 0, 1, "specify the source IP");
	arg_file_t *dest = (arg_file_t*) arg_filen("d", "dest", "x.x.x.x", 0, 1, "specify the destination IP");
	arg_int_t *sport = (arg_int_t*) arg_intn(NULL, "srcport", "short", 0, 1, "specify the source port if applicable");
	arg_int_t *dport = (arg_int_t*) arg_intn(NULL, "dstport", "short", 0, 1, "specify the destination port if applicable");

	arg_str_t *mcontent = (arg_str_t*) arg_strn(NULL, NULL, "string", 0, 1, "message content as a string");
	
	arg_end_t *end = (arg_end_t*) arg_end(20);
	void *argtable[] = {help,myip,bounce_opt,listen_opt,proto,source,
			    dest,sport,dport,mcontent,end};

	if(arg_nullcheck(argtable) != 0)
	{
		fprintf(stderr, "error: insufficient memory");
		exitstatus = -1;
		goto exit_prog;
	}

	/* parse and act */
	int nerrors = arg_parse(argc,argv,argtable);
	if(nerrors == 0)
	{
		
		char sourceipbuf[INET6_ADDRSTRLEN];
		size_t contentlen = 0;
		char message_content[MAX_MESSAGELEN + 1];

		/* get glossary */
		if(help->count)
		{
			arg_print_glossary(stdout, argtable, "%-25s %s\n");
		}

		/* get current IP address */
		else if(myip->count)
		{
			if(getmyip(sourceipbuf) == 0)
			{
				printf("Your packets will have the source IP address %s\n", sourceipbuf);
			}
			else
			{
				fprintf(stderr, "error: could not get your IP address.\n");
				exitstatus = -1;
				goto exit_prog;
			}
		}
		
		/* start bouncer */
		else if(bounce_opt->count)
		{
			if(!proto->count)
			{
				fprintf(stderr, "error: expected <protocol> specified.\n");
				exitstatus = -1;
				goto exit_prog;
			}
			
			enum Protocol protocol = parse_protocol(proto->filename[0]);
			if(protocol  == proto_UDP)
			{
				if(!sport->count)
				{
					fprintf(stderr, "error: expected <srcport> specified.\n");
					exitstatus = -1;
					goto exit_prog;
				}

				printf("Starting bouncer for UDP packets on port %u...\n", sport->ival[0]);
				
				/* start_udp_listener(sport->ival[0], bounce_udp_packet);*/
			}
			else
			{
				fprintf(stderr, "Bouncing for %s packets is not supported.\n", proto->filename[0]);
				exitstatus = -1;
				goto exit_prog;
			}
		}
		
		/* start listener */
		else if(listen_opt->count)
		{
			if(!proto->count)
			{
				fprintf(stderr, "error: expected <protocol> specified.\n");
				exitstatus = -1;
				goto exit_prog;
			}
			
			enum Protocol protocol = parse_protocol(proto->filename[0]);
			if(protocol  == proto_UDP)
			{
				if(!sport->count)
				{
					fprintf(stderr, "error: expected <srcport> specified.\n");
					exitstatus = -1;
					goto exit_prog;
				}

				printf("Starting listener for UDP packets on port %u...\n", sport->ival[0]);
				
				char filter[FILTER_BUFLEN];
				memset(filter, 0, FILTER_BUFLEN);
				sprintf(filter, "udp dst port %i", sport->ival[0]);

				start_listener(filter);
				/* start_udp_listener(sport->ival[0], print_packet);*/
			}
			else
			{
				fprintf(stderr, "Listening for %s packets is not supported.\n", proto->filename[0]);
				exitstatus = -1;
				goto exit_prog;
			}
		}


		/* send packet */
		else
		{
			/* take into account stdin reading if necessary */
			if(!mcontent->count)
			{
				contentlen = read(STDIN_FILENO, message_content, MAX_MESSAGELEN);
				if(contentlen < 0)
				{
					fprintf(stderr, "error: could not read message from stdin.\n");
					perror("read");
					exitstatus = -1;
					goto exit_prog;

				}
				message_content[contentlen] = '\0';
			}
			else
			{
				int tempstrlen = strlen(mcontent->sval[0]);
				contentlen = tempstrlen > MAX_MESSAGELEN ? MAX_MESSAGELEN : tempstrlen;
				memcpy(message_content, mcontent->sval[0], contentlen);
				message_content[contentlen] = '\0';
			}

			if(!proto->count || !dest->count)
			{
				fprintf(stderr, "error: expected <protocol> and <dest> specified.\n");
				exitstatus = -1;
				goto exit_prog;
			}
			
			if(!source->count)
			{
				if(getmyip(sourceipbuf) != 0)
				{
					fprintf(stderr, "error: could not get your IP address.\n");
					exitstatus = -1;
					goto exit_prog;
				}
			}
			else
			{
				strncpy(sourceipbuf, source->filename[0], INET6_ADDRSTRLEN);
			}


			enum Protocol protocol = parse_protocol(proto->filename[0]);
			if(protocol  == proto_ICMP)
			{
				time_t t;
				if(time(&t) == -1)
				{
					fprintf(stderr, "error: could not get timestamp.\n");
					exitstatus = -1;
					goto exit_prog;
				}

				printf("Sending ICMP ping packet...\nSource -> %s\n"
							       "Destination -> %s\n"
							       "Message -> %i\n",
							       sourceipbuf,
							       dest->filename[0],
							       t);

				/* construct ICMP header */
				int err;
				int payloadsize = sizeof(icmpheader_t) + sizeof(time_t);
				char ip_payload[payloadsize];
				
				/* copy in timestamp */
				/* we must do this first for the checksum calculation */
				t = htonl(t);
				memcpy(ip_payload + sizeof(icmpheader_t), &t, sizeof(time_t));
				
				/* identifier is lower 16 bits,
				   sequence number is upper 16 bits */ 
				uint32_t rest = htons(0x00);
				rest <<= 16;
				rest |= htons(0x7b);

				if((err = fill_icmp_header((icmpheader_t*) ip_payload, 8, 0, rest, sizeof(time_t))) != 0)
				{
					fprintf(stderr, "error: could not fill icmp header, returned %i.\n", err);
					exitstatus = -1;
					goto exit_prog;
				}


				/* send the ip packet */
				ipheader_t iph;
				iph.ip_p = 1; /* ICMP */
				inet_aton(sourceipbuf, (struct in_addr*) &iph.ip_src);
				inet_aton(dest->filename[0], (struct in_addr*) &iph.ip_dst);
				if((err = send_ip_packet(&iph, ip_payload, payloadsize)) != 0)
				{
					fprintf(stderr, "error: could not send ip packet, returned %i.\n", err);
					exitstatus = -1;
					goto exit_prog;
				}
			}
			else if(protocol  == proto_UDP)
			{
				/* get port info */
				unsigned short srcport = sport->count ? (unsigned short) sport->ival[0] : 0;
				if(!dport->count)
				{
					fprintf(stderr, "error: <dstport> specified.\n");
					exitstatus = -1;
					goto exit_prog;
				}
				unsigned short dstport = (unsigned short) dport->ival[0];

				printf("Sending UDP packet...\nSource -> %s:%i\n"
							       "Destination -> %s:%i\n"
							       "Message Length -> %i bytes\n",
							       sourceipbuf, srcport,
							       dest->filename[0], dstport,
							       contentlen);


				/* construct UDP header */
				int err;
				int payloadsize = sizeof(udpheader_t) + contentlen;
				char ip_payload[payloadsize];

				if((err = fill_udp_header((udpheader_t*) ip_payload, srcport, dstport, contentlen)) != 0)
				{
					fprintf(stderr, "error: could not fill udp header, returned %i.\n", err);
					exitstatus = -1;
					goto exit_prog;
				}

		
				/* set up IP payload */
				memcpy(ip_payload + sizeof(udpheader_t), message_content, contentlen);

				/* send the ip packet */
				ipheader_t iph;
				iph.ip_p = 17; /* UDP */
				inet_aton(sourceipbuf, (struct in_addr*) &iph.ip_src);
				inet_aton(dest->filename[0], (struct in_addr*) &iph.ip_dst);
				if((err = send_ip_packet(&iph, ip_payload, payloadsize)) != 0)
				{
					fprintf(stderr, "error: could not send ip packet, returned %i.\n", err);
					exitstatus = -1;
					goto exit_prog;
				}
			}
			else if(protocol  == proto_TCP)
			{
				printf("TCP currently not supported.\n");
			}
			else
			{
				fprintf(stderr, "error: protocol %s is not supported.\n", proto->filename[0]);
			}
	
			
		}

	}
	else
	{
		arg_print_errors(stdout, end, argv[0]);
		exitstatus = -1;
		goto exit_prog;
	}

exit_prog:
	arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));
	exit(exitstatus);
}
