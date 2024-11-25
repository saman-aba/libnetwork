
#include "socket.h"

int socket_send(struct ifreq *ifr, struct pkt_buffer *pkt)
{
	int 			sockfd;
	struct sockaddr_ll	sk_addr;
	struct sockaddr_in	ip_addr;
	
	sockfd = socket(AF_PACKET, SOCK_RAW, 0);
	if(sockfd < 0){
		printf("Unable to create socket.\n");
		return 0;
	}
}
