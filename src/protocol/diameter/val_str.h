#ifndef __VAL_STR_H__
#define __VAL_STR_H__

typedef struct _val_str{
	unsigned int val;
	const char *str;
} val_str;

const char *
string_from_value(const unsigned int value, const val_str *valstr, const char *fmt);

int 
value_from_string(const char *str, const val_str *valstr);

#endif
