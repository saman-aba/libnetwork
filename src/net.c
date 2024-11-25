#include "net.h"
#include <string.h>
#include <stdlib.h>

#include <pcap.h>

static unsigned char 		capture_pcap_flag = 0;
static struct tnl_stream 	*tss[TNL_MAX];
static struct tnl_engine 	*tes[TNL_MAX];

int net_pkt_receive(char *pkt, unsigned int pkt_size){
	return 0;
}

void network_free_pkt_buffer( struct pkt_buffer *pkt)
{
	free(pkt->data);
	free(pkt);
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
int capture_frame_pcap( struct pkt_buffer *pkt)
{
	pcap_t			*pcap_handle;
	pcap_dumper_t		*pcap_dumper;
	struct pcap_pkthdr	packet_header;

	pcap_handle = pcap_open_dead( DLT_EN10MB, 65535);
	if( !pcap_handle)
		return -1;
	pcap_dumper = pcap_dump_open( pcap_handle, "tmp.pcap");
	if( !pcap_dumper)
		return -1;
	packet_header.ts.tv_sec = time( NULL);
	packet_header.ts.tv_usec = 0;
	packet_header.caplen = pkt->data_len;
	packet_header.len = pkt->data_len;

	pcap_dump(( u_char *)pcap_dumper, &packet_header, pkt->data);

	pcap_dump_close( pcap_dumper);
	pcap_close( pcap_handle);
	return 0;
}
int network_transmit_frame( struct pkt_buffer *pkt)
{
	int bytes_sent = 0;
	struct tnl_stream *ts;
	struct tnl_engine *te;

	if( pkt->enable_cap)
	{
		capture_frame_pcap( pkt);
	}
	if( pkt->enable_tnl)
	{
		ts = tss[pkt->tnl_idx];
		te = tes[pkt->tnl_idx];
		ts->cur_data = pkt->data;
		ts->cur_data_sz = pkt->data_len;
		bytes_sent = te->tnl_pkt(ts);
	}
	if( pkt->enable_tx)
	{
	}
 
	network_free_pkt_buffer(pkt);

	return bytes_sent;
}

void _pkt_buffer_free( struct pkt_buffer *pkt)
{
	
}


