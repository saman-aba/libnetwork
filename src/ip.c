#include "ip.h"
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <net/ethernet.h>
#include "net.h"
static void
l3_handle_ip_pkt_receive_from_l2(ip_hdr_t *pkt, unsigned int pkt_size){

	char *l4_hdr;
	
	char dst_ip_addr[16];
	char src_ip_addr[16];

	ip_hdr_t *ip_hdr = pkt;
}

void l3_handle_pkt_receive_from_l2(char *pkt, unsigned int pkt_size, int protocol_type){
	switch(protocol_type)	{
		default:
			l3_handle_ip_pkt_receive_from_l2((ip_hdr_t *)pkt, pkt_size);
	}
}

void l3_handle_pkt(char *pkt, unsigned int pkt_size, int protocol_type){
	l3_handle_pkt_receive_from_l2(pkt, pkt_size, protocol_type);
}

extern int ether_send_frame( struct pkt_buffer *, short);

int ip_send_frame( struct pkt_buffer *pkt, char proto)
{
	struct iphdr	*ip;
	char 		*frame = NULL;
	unsigned 	frame_len = 0;

	frame_len = pkt->data_len + sizeof(struct iphdr);
	frame = malloc( frame_len);
	memcpy(frame + sizeof(struct iphdr), pkt->data, pkt->data_len);
	free( pkt->data);
	pkt->data = frame;
	pkt->data_len = frame_len;

	ip = (struct iphdr *)frame;
	
	ip->ihl 	= 5;
	ip->version 	= 4;
	ip->tos		= 0;
	ip->tot_len	= htons(frame_len);
	ip->id		= htonl(54321);
	ip->frag_off 	= 64;
	ip->ttl		= 64;
	ip->protocol 	= proto;
	ip->check	= 0;
	ip->saddr	= inet_addr("192.168.93.57"); // TODO : Must get this from host info
	ip->daddr	= pkt->d_addr;

	return ether_send_frame( pkt, ETH_P_IP);
}

