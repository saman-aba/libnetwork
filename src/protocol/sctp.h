#ifndef __SCTP_H__
#define __SCTP_H__

#include <stdlib.h>
#include "net.h"
typedef struct sctp_pkt		sctp_pkt_t;
typedef struct sctp_cmn_hdr 	sctp_cmn_hdr_t;
typedef struct sctp_chunk	sctp_chunk_t;
typedef struct sctp_chunk_param	sctp_chunk_param_t;
typedef struct sctp_data_chunk	sctp_data_chunk_t;
typedef struct sctp_init_chunk	sctp_init_chunk_t;

#define SCTP_MAX_CHUNK			16

#define SCTP_DATA_CHUNK			0
#define SCTP_INIT_CHUNK			1
#define SCTP_INIT_ACK_CHUNK		2
#define SCTP_SACK_CHUNK			3
#define SCTP_HEARTBEAT_CHUNK		4
#define SCTP_HEARTBEAT_ACK_CHUNK	5
#define SCTP_ABORT_CHUNK		6
#define SCTP_SHUTDOWN_CHUNK		7
#define SCTP_SHUTDOWN_ACK_CHUNK		8
#define SCTP_ERROR_CHUNK		9
#define SCTP_COOKIE_ECHO_CHUNK		10
#define SCTP_COOKIE_ACK_CHUNK		11
#define SCTP_ECNE_CHUNK			12

/* SCTP packet format
0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Common Header                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          Chunk #1                             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           ...                                 |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          Chunk #n                             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#define SCTP_COMMON_HEADER_SIZE		12
#define SCTP_DATA_CHUNK_HEAD_SIZE	12	
#define SCTP_CHUNK_HEAD_SIZE		4

#define SCTP_DATA_CHAR(chunk_ptr)		\
		(char *)chunk_ptr + 		\
		SCTP_CHUNK_HEAD_SIZE + 		\
		SCTP_DATA_CHUNK_HEAD_SIZE
#define SCTP_CHUNK_SIZE(chunk_ptr)		\
		(ntohs(chunk_ptr->len) - 	\
		SCTP_CHUNK_HEAD_SIZE)

#define SCTP_DATA_SIZE(chunk_ptr)		\
		(SCTP_CHUNK_SIZE(chunk_ptr) -	\
		SCTP_DATA_CHUNK_HEAD_SIZE)

#pragma pack (push,1)

/* SCTP Common Header Format
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|     Source Port Number        |     Destination Port Number   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      Verification Tag                         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           Checksum                            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+			
*/
struct sctp_cmn_hdr{
	unsigned short src_port;
	unsigned short dst_port;
	unsigned int verification_tag;
	unsigned int checksum;
};

/* Chunk Field Descriptions
0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Chunk Type  | Chunk  Flags  |        Chunk Length           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
\                                                               \
/                          Chunk Value                          /
\                                                               \
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
struct sctp_chunk{
	unsigned char	type;
	union{
		unsigned char flags;
		struct {
			unsigned char reserved:5;
			unsigned char u_bit:1; /* Unordered */
			unsigned char b_bit:1; /* Beginning fragment */
			unsigned char e_bit:1; /* Ending fragment */
		};
	};
	unsigned short 	len;
};
/* Chunk Types :
	0 - Payload Data (DATA)
	1 - Initiation (INIT)
	2 - Initiation Acknowledgement (INIT ACK)
	3 - Selective Acknowledgement (SACK)
	4 - Heartbeat Request (HEARTBEAT)
	5 - Heartbeat Acknowledgement (HEARTBEAT ACK)
	6 - Abort (ABORT)
	7 - Shutdown (SHUTDOWN)
	8 - Shutdown Acknowledgement (SHUTDOWN ACK)
	9 - Operation Error (ERROR)
	10- State Cookie (COOKIE ECHO)
	11- Cookie Acknowledgement (COOKIE ACK)
	12- Reserved for Explicit Congestion Notification Echo(ECNE)
	13- Reserved for Congestion Window Reduced (CWR)
	14- Shutdown Complete (SHUTDOWN COMPLETE)
	15 to 62- available
	63- reserved for IETF-defined Chunk Extensions
	64 to 126- available
	127- reserved for IETF-defined Chunk Extensions
	128 to 190- available
	191- reserved for IETF-defined Chunk Extensions
	192 to 254- available
	255- reserved for IETF-defined Chunk Extensions
*/

struct sctp_chunk_param {
	unsigned short type;
	unsigned short len;
	/* value MUST be a multiple of 4 */
	char *value;
};

struct sctp_data_chunk_hdr{
	unsigned int 	tsn;
	unsigned short 	sid;	/* Stream Identifier */
	unsigned short 	ssn;	/* Stream Sequence Number */
	unsigned int 	ppid;	/* Payload Protocol */
};

struct sctp_init_chunk{
	unsigned 	initTag;
	unsigned 	a_rwnd;
	unsigned short 	n_outbound;
	unsigned short 	n_inbound;
	unsigned	initTsn;
	sctp_chunk_param_t *params;

};

typedef struct sctp_pkt {
	sctp_cmn_hdr_t *cmn_hdr;
	sctp_chunk_t *chunks[SCTP_MAX_CHUNK]; // Im not sure about this
}sctp_pkt_t;

#pragma pack (pop)

typedef struct sctp_socket {
	sctp_pkt_t *pkt; 
	unsigned int pkt_size;
	unsigned int src_ip;
	unsigned int dst_ip;
}sctp_sock_t;

int
sctp_parse_common_hdr(char *in, sctp_cmn_hdr_t *out);

int
sctp_parse_chunks(char *in, sctp_chunk_t *out);

/* Chunk value parser for packets with chunk type = 0 (Payload Data). */
sctp_data_chunk_t *
sctp_parse_data_chunk(char *in, unsigned len);

sctp_init_chunk_t *
sctp_parse_init_chunk(char *in);

int
sctp_parse_pkt(const char *in, unsigned in_len, sctp_pkt_t *out);

int sctp_send( struct pkt_buffer *pkt, char type, char flags);
void 
sctp_handle_pkt_receive(char *pkt, unsigned int pkt_size,
			unsigned int src_ip,
			unsigned int dst_ip);
sctp_sock_t *
sctp_socket_create();

int
sctp_socket_destroy(sctp_sock_t *sock);

#endif //__SCTP_H__
