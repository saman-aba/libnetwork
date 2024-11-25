
#define TCP		6
#define UDP		17
#define SCTP		84
void 
l4_handle_pkt_receive(char *pkt, unsigned int pkt_size, 
		int protocol, 
		unsigned int src_ip, 
		unsigned int dst_ip);
