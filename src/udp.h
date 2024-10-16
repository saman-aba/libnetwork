#ifndef __UDP_H__
#define __UDP_H__

#include <sys/socket.h>
#include <arpa/inet.h>
#include "tunnel.h"

typedef void ( *udp_tunnel_fn)(const char *, int);

void 
lcore_udp_srv(int sockfd, struct sockaddr *clientaddr_ptr, socklen_t clientaddr_len);

void
udp_server(udp_tunnel_fn fn);

void
udp_server_stop();

void udp_tnl( const char *host, uint16_t port);

int udp_pkt_tnl(struct tnl_stream *ts);

void udp_init( struct tnl_stream *ts);

void
udp_client_stop();

int 
udp_client(const char *h_ip, unsigned short h_port);

int 
udp_send(int sockfd, const char *data, unsigned short data_len);
#endif
