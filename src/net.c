#include "net.h"
#include <string.h>
#include <stdlib.h>
struct tnl_stream *tss[TNL_MAX];
struct tnl_engine *tes[TNL_MAX];

int net_pkt_receive(char *pkt, unsigned int pkt_size){
	return 0;
}

int network_init_engine( struct tnl_engine *te, stream_id id)
{
	int i = 0;
	for( ; i < TNL_MAX ; i++)
	{
		if( i == id){
			tes[TNL_UDP] = te;
			break;
		}
	}
	return 0;
}
int network_init_stream( struct tnl_stream *ts, stream_id id)
{
	int i = 0;
	for( ; i < TNL_MAX ; i++)
	{
		if( i == id){
			tss[i] = ts;
			break;
		}
	}
	return 0;
}

int network_transmit_frame( char *buf, unsigned buf_len)
{
	struct tnl_stream *ts;
	struct tnl_engine *te;

	ts = tss[TNL_UDP];
	te = tes[TNL_UDP];

	ts->cur_data = buf;
	ts->cur_data_sz = buf_len;

	te->tnl_pkt(ts);



	free(buf);
}
