#include "packet_hatchet.h"
#include "ip_factory.h"
#include <stdio.h>

/*
TODO:
	- make '<' flag work
*/

int main(int argc, char** argv)
{
	int exitstatus = 0;


	/* declare CL args */
	arg_lit_t *help = (arg_lit_t*) arg_lit0("h", "help", "prints the command glossary");
	arg_lit_t *myip = (arg_lit_t*) arg_lit0("m", NULL, "prints the external ip of the interface currently being used");
	
	arg_file_t *proto = (arg_file_t*) arg_filen("p", "protocol", "acronym", 0, 1, "specify the protocol being manipulated");
	arg_file_t *source = (arg_file_t*) arg_filen("s", "source", "x.x.x.x", 0, 1, "specify the source IP");
	arg_file_t *dest = (arg_file_t*) arg_filen("d", "dest", "x.x.x.x", 0, 1, "specify the destination IP");
	arg_int_t *sport = (arg_int_t*) arg_intn(NULL, "srcport", "short", 0, 1, "specify the source port if applicable");
	arg_int_t *dport = (arg_int_t*) arg_intn(NULL, "dstport", "short", 0, 1, "specify the destination port if applicable");

	arg_str_t *mcontent = (arg_str_t*) arg_strn(NULL, NULL, "string", 0, 1, "message content as a string");
	
	arg_end_t *end = (arg_end_t*) arg_end(20);
	void *argtable[] = {help,myip,proto,source,dest,sport,dport,mcontent,end};
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

		if(!mcontent->count)
			mcontent->sval[0] = "";

		/* get glossary */
		if(help->count)
		{
			arg_print_glossary(stdout, argtable, "%-25s %s\n");
		}
		/* get current IP address */
		else if(myip->count)
		{
			printf("Current outfacing IP address is 127.0.0.1\n");
		}
		/* send packet */
		else
		{
			if(!proto->count || !source->count || !dest->count)
			{
				fprintf(stderr, "error: expected <protocol>, <source>, and <dest> specified.\n");
				exitstatus = -1;
				goto exit_prog;
			}


			enum Protocol protocol = parse_protocol(proto->filename[0]);
			if(protocol  == proto_ICMP)
			{
				printf("ICMP currently not supported.\n");
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

				int numbytes = strlen(mcontent->sval[0]) + 1;

				printf("Sending UDP packet...\nSource -> %s:%i\n"
							       "Destination -> %s:%i\n"
							       "Message Length -> %i bytes\n",
							       source->filename[0], srcport,
							       dest->filename[0], dstport,
							       numbytes);


				/* construct UDP header */
				int err;
				int payloadsize = sizeof(udpheader_t) + numbytes;
				char ip_payload[payloadsize];

				if((err = fill_udp_header((udpheader_t*) ip_payload, srcport, dstport, numbytes)) != 0)
				{
					fprintf(stderr, "error: could not fill udp header, returned %i.\n", err);
					exitstatus = -1;
					goto exit_prog;
				}

		
				/* set up IP payload */
				memcpy(ip_payload + sizeof(udpheader_t), mcontent->sval[0], numbytes);

				/* send the ip packet */
				ipheader_t iph;
				iph.ip_p = 17; /* UDP */
				inet_aton(source->filename[0], (struct in_addr*) &iph.ip_src);
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
