#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include "if_info.h"

struct if_info *get_if_info(int family)
{
	struct if_info *ifi, *ifihead, **ifipnext;
	char *buf, *offt, *charptr;
	char lastname[IFNAMSIZ];
	int 	sockfd,
		len,
		lastlen = 0;
	
	struct sockaddr_in 	*sinptr;
	struct ifreq 		*ifr,
				ifrcpy;
	struct ifconf		ifc;

	sockfd = socket(AF_PACKET, SOCK_RAW, 0);
	len = 100 * sizeof(struct ifreq);

	for(;;)
	{
		buf = malloc(len);
		ifc.ifc_len = len;
		ifc.ifc_buf = buf;
		if(ioctl(sockfd, SIOCGIFCONF, &ifc) < 0){
			perror("ioctl error");
			return NULL;
		}else{
			if(ifc.ifc_len == lastlen)
				break;
			lastlen += ifc.ifc_len;	
		}
		len += 10 * sizeof(struct ifreq);
		free(buf);
	}
	
	ifihead = NULL;
	ifipnext = &ifihead;
	lastname[0] = 0;
	
	for(offt = buf; offt < buf + ifc.ifc_len;)
	{
		ifr = (struct ifreq *)offt;
		
		if(ifr->ifr_addr.sa_family == AF_INET6)
			continue;
		len = sizeof(struct sockaddr_in);
		
		offt += sizeof(struct ifreq);
	
	
		if((charptr = strchr(ifr->ifr_name, ':')) != NULL)
			*charptr = 0; /* replace : with NULL */
		if(!strncmp(lastname, ifr->ifr_name, IFNAMSIZ))
			continue;
		memcpy(lastname, ifr->ifr_name, IFNAMSIZ);
		
		ifrcpy = *ifr;
		ioctl(sockfd, SIOCGIFFLAGS, &ifrcpy);
		if(!(ifrcpy.ifr_flags & IFF_UP))
			continue;
		ifi = calloc(1, sizeof(struct if_info));
		*ifipnext = ifi;
		ifipnext = &ifi->next;
	
		ifi->flags = ifrcpy.ifr_flags;
		ifi->mtu = ifrcpy.ifr_mtu;
		memcpy(ifi->name, ifrcpy.ifr_name, IF_NAME_SZ);
		ifi->name[IF_NAME_SZ - 1] = '\0';
	
		if(ifr->ifr_addr.sa_family == AF_INET)
		{
			sinptr = (struct sockaddr_in*) &ifr->ifr_addr;
			ifi->addr = calloc(1, sizeof(struct sockaddr_in));
			memcpy(ifi->addr, sinptr, sizeof(struct sockaddr_in));
#ifdef			SIOCGIFBRDADDR
#endif
#ifdef			SIOCGIFDSTADDR
#endif
		}
	}
	close(sockfd);
	free(buf);
	return ifihead;
	

}

void free_if_info(struct if_info *ifihead)
{
	struct if_info *ifi, *ifinext;
	for(ifi = ifihead; ifi ; ifi = ifinext)
	{
		if(ifi->addr)
			free(ifi->addr);
		if(ifi->brd_addr)
			free(ifi->brd_addr);
		if(ifi->dst_addr)
			free(ifi->dst_addr);
		ifinext = ifi->next;
		free(ifi);
	}
}

