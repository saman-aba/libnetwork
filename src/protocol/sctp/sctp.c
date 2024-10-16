#include <stdint.h>
#include "sctp.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

static sctp_sock_t sk;
void sctp_handle_pkt_receive(char *pkt, unsigned int pkt_size,
		unsigned int src_ip,
		unsigned int dst_ip){
	if(!sctp_parse_pkt(pkt, pkt_size, NULL))
		return; /* Invalid packet */
}
int sctp_parse_pkt(const char *in, unsigned in_len, sctp_pkt_t *out){
	
	int ret = 0;
	unsigned nb_chnk = 0;
	unsigned offt = 0;
	unsigned short length = 0;
	unsigned rem = 0;
	int pad = 0;

	out->cmn_hdr = (sctp_cmn_hdr_t*)in;
	offt += SCTP_COMMON_HEADER_SIZE;
	rem = in_len - offt;
	
	while(rem){
		 
		out->chunks[nb_chnk] = (sctp_chunk_t *)(in + offt);
		length = ntohs(out->chunks[nb_chnk]->len);
		if(length <= 0)
			return -1;
		
		pad =((length % 4) ? (4 - (length%4)): 0);

		offt += (length + pad);
		rem -= (length + pad);
		
		if(out->chunks[nb_chnk]->type == SCTP_DATA_CHUNK){
			nb_chnk++;
		}
	}
	return nb_chnk;
}

int sctp_parse_common_hdr(char *in, sctp_cmn_hdr_t *out){
	
	out->src_port = (in[0] << 8) | in[1];
	out->dst_port = (in[2] << 8) | in[3];
	out->verification_tag = (in[4] << 24) | 
				(in[5] << 16) | 
				(in[6] << 8)  | 
				in[7];
	out->checksum = (in[8] << 24) | 
			(in[9] << 16) | 
			(in[10] << 8) | 
			in[11]; 
	
	return 0;
}

int *sctp_parse_chunk(char *in, sctp_chunk_t *out){
	
	out->type = (unsigned char)in[0];
	out->len = (unsigned short)((in[2] << 8) | in[3]);
	
	switch(out->type){
	case SCTP_DATA_CHUNK:{
		break;
	}
	case SCTP_INIT_CHUNK:{
		break;
	}
	case SCTP_INIT_ACK_CHUNK:{
		break;
	}
	case SCTP_SACK_CHUNK:{
		break;
	}
	case SCTP_HEARTBEAT_CHUNK:{
		break;
	}
	case SCTP_HEARTBEAT_ACK_CHUNK:{
		break;
	}
	case SCTP_ABORT_CHUNK:{
		break;
	}
	case SCTP_SHUTDOWN_CHUNK:{
		break;
	}
	case SCTP_SHUTDOWN_ACK_CHUNK:{
		break;
	}
	case SCTP_ERROR_CHUNK:{
		break;
	}
	case SCTP_COOKIE_ECHO_CHUNK:{
		break;
	}
	case SCTP_COOKIE_ACK_CHUNK:{
		break;
	}
	case SCTP_ECNE_CHUNK:{
		break;
	}
	default: 
		break;
	}
	return 0;
	
}

sctp_data_chunk_t *sctp_parse_data_chunk(char *in, unsigned len){
	
}


extern int ip_send_frame(char *buf, unsigned buf_len, const char *addr, char proto);

int sctp_send(const char *buf, unsigned buf_len, 
		const char *addr, uint16_t port, char type, char flags)
{
	struct sctp_cmn_hdr 		*sctp_cmn;
	struct sctp_chunk		*chunk;
	struct sctp_data_chunk_hdr	*data_chunk_hdr;

	unsigned 			hdr_sz;

	char 				*frame = NULL;
	unsigned 			frame_len = 0;

	hdr_sz = sizeof(struct sctp_cmn_hdr);

	switch(type)
	{
		case SCTP_DATA_CHUNK:{
			hdr_sz += sizeof(struct sctp_data_chunk_hdr) +
				sizeof(struct sctp_chunk);
			frame_len = buf_len + hdr_sz;
			frame = malloc( frame_len);
			memset( frame, 0, frame_len);

			sctp_cmn = (struct sctp_cmn_hdr *)frame;
			chunk = ( struct sctp_chunk *)(frame + sizeof(struct sctp_cmn_hdr));
			break;
		}
		default:
			return -1;
	}

	memcpy(frame + hdr_sz, buf, buf_len);
	sctp_cmn->src_port 		= htons(4001);
	sctp_cmn->dst_port 		= port;
	sctp_cmn->verification_tag	= buf_len + hdr_sz;
	sctp_cmn->checksum		= 0;

	chunk->type = type;
	chunk->flags = flags;
	chunk->len = htons(buf_len + 
			sizeof(struct sctp_chunk) +
			sizeof(struct sctp_data_chunk_hdr));
	

	return ip_send_frame( frame, frame_len, addr, IPPROTO_SCTP);
}


