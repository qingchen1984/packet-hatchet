/* The below code is inspired by the pcap tutorial:
*  http://www.tcpdump.org/pcap.html
*/

#include "listener.h"

#define SIZE_ETHERNET 14
#define SNAPLEN 1024

static void callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	printf("Captured a packet of length %i.\n", header->len);
}

int start_listener(char *filter)
{
	/* look up devs to listen on as sniffing device */
	char *dev, errbuf[PCAP_ERRBUF_SIZE];
	dev = pcap_lookupdev(errbuf);
	if(dev == NULL)
	{
		fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
		return -1;
	}
	printf("Connected to device %s - SUCCESS\n", dev);

	/* get IP of sniffing device */
	bpf_u_int32 net, mask;
	if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1)
	{
		fprintf(stderr, "Couldn't get IP of device: %s\n", errbuf);
		return -1;
	}
	struct in_addr temp_addr;
	temp_addr.s_addr = net;
	printf("Found effective IP of %s - SUCCESS\n", inet_ntoa(temp_addr));

	/* open the stream */
	pcap_t *handle;
	handle = pcap_open_live(dev, SNAPLEN, 1, 1000, errbuf);
	if(handle == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return -1;
	}
	printf("Live stream opened - SUCCESS\n");


	/* filter the stream */
	struct bpf_program fp;
	if(pcap_compile(handle, &fp, filter, 0, net) == -1)
	{
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter, pcap_geterr(handle));
		return -1;
	}
	if(pcap_setfilter(handle, &fp) == -1)
	{
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter, pcap_geterr(handle));
		return -1;
	}

	pcap_loop(handle, -1, callback, NULL);

	pcap_freecode(&fp);
	pcap_close(handle);

	return 0;
}
