#ifndef __TUNNEL_H__
#define __TUNNEL_H__

#include <stdint.h>
#include <stdbool.h>


typedef uint16_t 	port_t;
typedef uint8_t		lcoreid_t;

struct tnl_lcore {
	lcoreid_t	lcoreid;
	volatile char 	stopped;
	uint64_t	tot_cycles;
};

struct tnl_stream {
	int		fd;

	bool		disabled;
	
	uint16_t	d_port;
	uint16_t	s_port;

	uint8_t		*d_addr;

	uint64_t	rcv_pkts;
	uint64_t	tx_pkts;

	char 		*cur_data;
	uint32_t	cur_data_sz;
	
	struct tnl_lcore lcore;
};

typedef enum{
	TNL_UDP,
	TNL_TCP,
	TNL_SCTP,
	TNL_RAW,
	TNL_FORWARD,
	TNL_MAX
}stream_id;

typedef int 	(*tnl_begin_fn)(port_t port);
typedef void 	(*tnl_end_fn)(port_t port);
typedef void	(*tnl_init_fn)(struct tnl_stream *ts);
typedef int 	(*tnl_pkt_fn)(struct tnl_stream *ts);

struct tnl_engine{
	const char 	*tnl_mode_name;
	tnl_begin_fn	tnl_begin;
	tnl_end_fn	tnl_end;
	tnl_init_fn	tnl_init;
	tnl_pkt_fn	tnl_pkt;
	const char	*status;	
};

#endif 
