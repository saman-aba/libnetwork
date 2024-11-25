#include "ether.h"
#include <string.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <stdlib.h>
#include <string.h>
#include "net.h"
extern int network_transmit_frame( struct pkt_buffer *buf); 

int ether_send_frame( struct pkt_buffer *pkt, short proto)
{
	struct ethhdr 	*eth;
	char		*frame = NULL;
	unsigned	frame_len = 0;

	frame_len = pkt->data_len + sizeof(struct ethhdr);
	frame = malloc( frame_len);

	eth = (struct ethhdr *)frame;

	/* TODO : lookup table for addresses has not been implemented yet
	char *dstmac = lookup_ip(addr); */
	/* TODO : Temporary */
	char s_mac[ETH_ALEN] = { 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	 char d_mac[ETH_ALEN] = { 
		0xd8, 0x47, 0x32, 0x9e, 0x44, 0xd1 };

	memcpy(eth->h_source, s_mac /* Must be taken from host*/, ETH_ALEN);
	memcpy(eth->h_dest, d_mac /*Must be taken from lookup table*/, ETH_ALEN);
	eth->h_proto = htons(proto);

	memcpy( frame + sizeof(struct ethhdr), pkt->data, pkt->data_len);
	
	pkt->ethernet_header = 0;
	free( pkt->data);
	pkt->data_len = frame_len;
	pkt->data = frame;

	return network_transmit_frame( pkt);
}
