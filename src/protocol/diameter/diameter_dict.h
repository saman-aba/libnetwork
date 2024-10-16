#ifndef __DIAMETER_DICT__
#define __DIAMETER_DICT__

#include "diameter.h"

typedef struct command_code_format{
	char id[3];
	unsigned int code;
	char full_name[64];
	unsigned int application_id;
	unsigned int avp_list[64];
}ccf_t;

struct diameter_avp *
diameter_avp_dict_code_lookup(unsigned int code);

ccf_t *
diameter_ccf_dict_code_lookup(char *id);

#endif
