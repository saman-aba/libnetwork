#ifndef __IP_H__
#define __IP_H__

#define L3_ICMP			1
#define L3_IGMP			2
#define L3_GGP			3
#define L3_IP_IN_IP		4
#define L3_TCP			6
#define L3_CHAOS		16
#define L3_UDP			17
#define L3_DCCP			21
#define L3_SCTP			84

#define GET_IP_HDR_LEN(ip_hdr_ptr)		(ip_hdr_ptr->ihl * 4)
#define GET_IP_HDR_TOTAL_LENGTH(ip_hdr_ptr)	(ip_hdr_ptr->total_len)	

#define GET_IP_HDR_PAYLOAD(ip_hdr_ptr) 		((char *)ip_hdr_ptr +		\
						(ip_hdr_ptr->ihl * 4))
#define GET_IP_HDR_PAYLOAD_SIZE(ip_hdr_ptr) 	(GET_IP_HDR_TOTAL_LENGTH(ip_hdr_ptr) - 	\
						GET_IP_HDR_LEN(ip_hdr_ptr))

#pragma pack (push,1)
typedef struct ip_hdr{
	char			version;
	char 			ihl;
	
	struct{
		unsigned char  	dscp:6;
		unsigned char  	ecn:2;
	};
	short			total_len;
	short			identification;
	struct{
		unsigned short 	flags:2;
		unsigned short 	fragment_offt:14;
	};
	char			ttl;
	char 			protocol;
	short			checksome;	
	unsigned int 		src_ip;
	unsigned int 		dst_ip;
	
} ip_hdr_t;
#pragma pack(pop)

void 
l3_handle_pkt_receive_from_l2(char *pkt, unsigned int pkt_size, 
	int protocol_type);

void 
l3_handle_pkt(char *pkt, unsigned int pkt_size, 
	int protocol_type);

#endif
