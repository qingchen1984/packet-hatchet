#include "packet_hatchet.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	/* declare CL args */
	arg_file_t *proto = (arg_file_t*) arg_filen("p", "protocol", "<protocol>", 1, 1, "specify the protocol being manipulated");
	arg_file_t *source = (arg_file_t*) arg_filen("s", "source", "<source>", 1, 1, "specify the source IP");
	arg_file_t *dest = (arg_file_t*) arg_filen("d", "dest", "<dest>", 1, 1, "specify the destination IP");
	arg_end_t *end = (arg_end_t*) arg_end(20);
	void *argtable[] = {proto,source,dest,end};
	
	/* parse and act */
	int nerrors = arg_parse(argc,argv,argtable);
	if(nerrors == 0)
	{
		printf("Sending %s packet from %s to %s\n", proto->filename[0], source->filename[0], dest->filename[0]);
	}
	else
	{
		arg_print_errors(stdout, end, argv[0]);
	}
}
