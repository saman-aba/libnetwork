#ifndef __ETHERNET_H__
#define __ETHERNET_H__


#define ETHER_PROTO_LOOP	0x0060
#define ETHER_PROTO_IP		0x0800

#define ETHER_ADDR_LEN		6

struct ethernet_hdr{
	unsigned char 		dest[ETHER_ADDR_LEN];
	unsigned char		source[ETHER_ADDR_LEN];
	unsigned short		proto;
} __attribute__((packed));

void
ethernet_frame_transmit(int sockfd, struct pkt_buffer *);


#endif
