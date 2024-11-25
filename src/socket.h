#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "if_info"
#include "net.h"

/*
	EBADF  	fd is not a valid file descriptor.
	EFAULT 	argp references an inaccessible memory area.
	EINVAL 	op or argp is not valid.
	ENOTTY 	fd is not associated with a character special device.
	ENOTTY 	The specified operation does not apply to the kind of
    		object that the file descriptor fd references.
*/
int socket_send(struct ifreq *,struct pkt_buffer *);

#endif
