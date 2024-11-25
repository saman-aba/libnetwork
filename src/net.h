#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include "tunnel.h"


#define NET_ETH_HEADER_LEN	14
#define NET_MTU			1560

/* Flags */
#define NET_CAP_FRM		0x00000001
#define NET_ENBL_TNL		0x00000002
#define NET_ENBL_TX		0x00000004


struct pkt_buffer{
	struct pkt_buffer 	*next;
	struct pkt_buffer 	*prev;

	uint8_t			enable_cap;
	
	uint8_t			enable_tx;
	uint8_t			enable_tnl;
	uint16_t		tnl_idx;
	
	unsigned int		len,
				data_len,
				truesize;

	unsigned short 		mac_len;

	unsigned short 		protocol,
				ethernet_header,
				network_header,
		 		transport_header;

	uint32_t		s_addr;
	uint32_t		d_addr;

	uint16_t		l4_sport;
	uint16_t		l4_dport;

	unsigned char 		*tail,
				*end,
				*head,
				*data;
};

struct pkt_buffer_head{
	struct pkt_buffer	*next;
	struct pkt_buffer 	*prev;
	unsigned int 		qsize;
	pthread_spinlock_t	lock;
};


static inline unsigned int 
pkt_queue_size(const struct pkt_buffer_head *list)
{
	return list->qsize;
}

static inline void 
pkt_queue_head_init(struct pkt_buffer_head *list)
{
	pthread_spin_init(&list->lock, PTHREAD_PROCESS_SHARED);
	/* I can seperate this portion from the lock initiation
	 * in order to have the ability to initialize a list that
	 * its lock is already initialized */
	list->prev = list->next = (struct pkt_buffer *)list;
	list->qsize = 0;
}

static inline bool 
pkt_queue_is_first(const struct pkt_buffer_head *list,
			const struct pkt_buffer *pkt)
{
	return pkt->prev == (const struct pkt_buffer *)list;
}

static inline bool 
pkt_queue_is_last(const struct pkt_buffer_head *list,
					const struct pkt_buffer *pkt)
{
	return pkt->next == (const struct pkt_buffer *)list;
}

static inline void 
_pkt_insert(	struct pkt_buffer *newpkt,
				struct pkt_buffer *prev,
				struct pkt_buffer *next,
				struct pkt_buffer_head *list)
{
	*(volatile struct pkt_buffer **)&(newpkt->next) = next;
	*(volatile struct pkt_buffer **)&(newpkt->prev) = prev;
	*(volatile struct pkt_buffer **)&(next->prev) = newpkt;
	*(volatile struct pkt_buffer **)&(prev->next) = newpkt;

	*(volatile unsigned int *)&(list->qsize) = list->qsize + 1; 
}

static inline void *
pkt_put_data(struct pkt_buffer *pkt, const void *data,
		unsigned int len)
{
	
}	

static inline unsigned char *
pkt_ethernet_header(const struct pkt_buffer *pkt)
{
	return pkt->head + pkt->ethernet_header;
}

static inline unsigned char *
pkt_network_header(const struct pkt_buffer *pkt)
{
	return pkt->head + pkt->network_header;
}

static inline unsigned short 
pkt_transport_header_offset(const struct pkt_buffer *pkt)
{
	return pkt->transport_header;
}

static inline unsigned char *
pkt_transport_header(const struct pkt_buffer *pkt)
{
	return pkt->head + pkt->transport_header;
}

static inline void 
pkt_set_transport_header(struct pkt_buffer *pkt, const int offt)
{
	pkt->transport_header += offt;
}

void network_pkt_buffer_free( struct pkt_buffer *);

#endif
