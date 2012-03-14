#include "packet_hatchet.h"
#include <stdio.h>

/*
TODO:
   -Add print glossary
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
	arg_end_t *end = (arg_end_t*) arg_end(20);
	void *argtable[] = {help,myip,proto,source,dest,end};
	
	/* parse and act */
	int nerrors = arg_parse(argc,argv,argtable);
	if(nerrors == 0)
	{


		/* Get glossary */
		if(help->count)
		{
			arg_print_glossary(stdout, argtable, "%-25s %s\n");
		}
		/* Get current IP address */
		else if(myip->count)
		{
			printf("Current outfacing IP address is 127.0.0.1\n");
		}
		/* Send packet */
		else
		{
			if(!proto->count || !source->count || !dest->count)
			{
				fprintf(stderr, "error: expected <protocol>, <source>, and <dest> specified.\n");
				exitstatus = -1;
				goto exit_prog;
			}

			printf("Sending %s packet from %s to %s\n", proto->filename[0], source->filename[0], dest->filename[0]);
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
