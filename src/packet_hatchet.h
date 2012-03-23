#ifndef _packet_hatchet_H
#define _packet_hatchet_H

#include <stdlib.h>
#include <string.h>
#include <argtable2.h>

typedef struct arg_file arg_file_t;
typedef struct arg_lit arg_lit_t;
typedef struct arg_end arg_end_t;

enum Protocol { proto_ICMP, proto_UDP, proto_TCP, proto_invalid };

enum Protocol parse_protocol(const char *str)
{
	if(strcasecmp(str, "ICMP") == 0)
		return proto_ICMP;
	else if(strcasecmp(str, "UDP") == 0)
		return proto_UDP;
	else if(strcasecmp(str, "TCP") == 0)
		return proto_TCP;

	return proto_invalid;
}

#endif
