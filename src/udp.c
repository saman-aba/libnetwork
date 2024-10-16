#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "udp.h"

#define SERV_PORT 	7000
#define UDP_MSG_MAX	2046

static udp_tunnel_fn tunnel;
static volatile int udpstop = 0;

static struct tnl_engine udp_tnl_engine = {
	.tnl_mode_name 	= "udp_tunnel",
	.tnl_init	= udp_init,
	.tnl_pkt	= udp_pkt_tnl
};

/* Note : Protocol independant udp server loop  */
void lcore_udp_srv(int sockfd, struct sockaddr *clientaddr_ptr, socklen_t clientaddr_len)
{
	int 		nb_rcv;
	socklen_t	len;
	char		buffer[UDP_MSG_MAX];
	
	while(!udpstop)
	{
		len = clientaddr_len;
		nb_rcv = recvfrom(sockfd, 
				buffer, UDP_MSG_MAX, 
				0,
				clientaddr_ptr, &clientaddr_len);
		if(nb_rcv > 0)
			tunnel(buffer, nb_rcv);
	}
}

void udp_server(udp_tunnel_fn fn)
{
	int sockfd;
	struct sockaddr_in serv_addr, clientaddr;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));	
	serv_addr.sin_family =		AF_INET;
	serv_addr.sin_addr.s_addr = 	htonl(INADDR_ANY);
	serv_addr.sin_port = 		htons(SERV_PORT);
	
	bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	tunnel = fn;
	
	lcore_udp_srv(sockfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
}

void udp_server_stop()
{
	udpstop = 1;
}

int udp_conf()
{

}

extern int network_init_stream( struct tnl_stream *ts, stream_id id);
extern int network_init_engine( struct tnl_engine *ts, stream_id id);
void udp_tnl( const char *host, uint16_t port)
{
	static struct tnl_stream udp_ts;

	udp_ts.d_port = port;
	udp_ts.d_addr = malloc( sizeof(host));
	memcpy( udp_ts.d_addr, host, 14);
	
	network_init_stream( &udp_ts, TNL_UDP);
	network_init_engine( &udp_tnl_engine, TNL_UDP);
	udp_init(&udp_ts);
}

void udp_init( struct tnl_stream *ts)
{
	int ret = 0;
	struct sockaddr_in	servaddr;
	memset( &servaddr, 0, sizeof( servaddr));

	servaddr.sin_family 	= AF_INET;
	servaddr.sin_port	= htons( ts->d_port);
	inet_pton( AF_INET, ts->d_addr, &servaddr.sin_addr);
	
	ts->fd = socket( AF_INET, SOCK_DGRAM, 0);
	
	ret = connect( ts->fd, ( struct sockaddr *)&servaddr, sizeof( servaddr));
	if(ret == 0){
		ts->disabled = true;
	} else {
		ts->disabled = false;
		printf("Server unreachable.\n");
	}
}

void udp_client_stop()
{

}

int udp_pkt_tnl(struct tnl_stream *ts)
{
	write(ts->fd, ts->cur_data, ts->cur_data_sz);
	return 0;
}



