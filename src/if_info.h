#ifndef __IF_INFO__
#define __IF_INFO__

#include <net/if.h>
#define IF_NAME_SZ		16
#define INTERFACE_HW_ADDR	8
struct if_info{
	char 		name[IF_NAME_SZ];
	short 		index;
	short 		mtu;
	unsigned char 	hw_addr[INTERFACE_HW_ADDR];
	short		hw_addr_len;

	short		flags;
	
	
	struct sockaddr *addr;
	struct sockaddr *brd_addr;
	struct sockaddr *dst_addr;
	
	struct if_info *next; 	
};

struct if_info *get_if_info(int);
void free_if_info(struct if_info *);


#endif
