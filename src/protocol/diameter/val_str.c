#include "val_str.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>
static const char *_try_string_from_value_index(const unsigned int value,
		const val_str *valstr,
		int *index)
{
	int i = 0;
	assert(index);
	if(valstr){
		while(valstr[i].str){
			if(valstr[i].val == value){
				*index = i;	
				return valstr[i].str;
			}
			i++;
		}
	}
	*index = -1;
	return NULL;
}

const char *_try_string_from_value(const unsigned int value,
		const val_str *valstr)
{
	int dummy;
	return _try_string_from_value_index(value, valstr, &dummy);
}

const char *string_from_value(const unsigned int value,
		const val_str *valstr,
		const char *fmt)
{
	const char *ret;
	assert(fmt);
	ret = _try_string_from_value(value, valstr);
	return ret;	
}


int value_from_string(const char *str, const val_str *valstr)
{
	int i = 0;
	if(valstr){
		while(valstr[i].str){
			if(!strcmp(valstr[i].str, str))
				return valstr[i].val;
			i++;
		}
	}
	return -1;
}
