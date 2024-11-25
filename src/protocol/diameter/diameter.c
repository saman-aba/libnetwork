#include "diameter.h"
#include "protocol/sctp.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "val_str.h"
#include <arpa/inet.h>
#include <jansson.h>
#include "net.h"

static const val_str avp_type_table[] = {
	{OctetString, 			"OctetString"},
	{Integer32,			"Integer32"},
	{Integer64,			"Integer64"},
	{Unsigned32,			"Unsigned32"},
	{Unsigned64,			"Unsigned64"},
	{Float32,			"Float32"},
	{Float64,			"Float64"},
	{Grouped,			"Grouped"}
};
static const val_str avp_display[] = {
	{DIAMETER_USER_NAME,		"User-Name"},
	{DIAMETER_HOST_IP_ADDRESS,	"Host-IP-Address"},
	{DIAMETER_AUTH_APPLICATION_ID,	"Auth-Application-Id"},
	{DIAMETER_ACCT_APPLICATION_ID,	"Acct-Application-Id"},
	{DIAMETER_VENDOR_SPECIFIC_APPLICATION_ID,	
					"Vendor-Specific-Application-Id"},
	{DIAMETER_SESSION_ID, 		"Session-Id"},
	{DIAMETER_ORIGIN_HOST, 		"Origin-Host"},
	{DIAMETER_SUPPORTED_VENDOR_ID,	"Supported-Vendor-Id"},
	{DIAMETER_VENDOR_ID,		"Vendor-Id"},
	{DIAMETER_FIRMWARE_REVISION,	"Firmware-Revision"},
	{DIAMETER_RESULT_CODE,		"Result-Code"},
	{DIAMETER_PRODUCT_NAME,		"Product-Name"},
	{DIAMETER_RESULT_CODE, 		"Result-Code"},
	{DIAMETER_ORIGIN_STATE_ID,	"Origin-State-Id"},
	{DIAMETER_AUTH_SESSION_STATE,	"Auth-Session-State"},
	{DIAMETER_PROXY_HOST,		"Proxy-Host"},
	{DIAMETER_ROUTE_RECORD,		"Route-Record"},
	{DIAMETER_DESTINATION_REALM,	"Destination-Realm"},
	{DIAMETER_PROXY_INFO,		"Proxy-Info"},
	{DIAMETER_DESTINATION_HOST,	"Destination-Host"},
	{DIAMETER_ORIGIN_REALM,		"Origin-Realm"},				//296
	{DIAMETER_EXPERIMENTAL_RESULT,	"Experimental-Result"},
	{DIAMETER_INBAND_SECURITY_ID,	"Inband-Security-Id"},				//299

	{DIAMETER_MIP_HOME_AGENT_HOST,	"MIP-Home-Agent-Host"},				//348
	{DIAMETER_MIP6_AGENT_INFO, 	"MIP6-Agent-Info"},				//486
	{DIAMETER_SERVICE_SELECTION,	"Service-Selection"},				//493
	
	{DIAMETER_VISITED_NETWORK_IDENTIFIER,
					"Visited-Network-Identifier"},			//600
	{DIAMETER_OC_SUPPORTED_FEATURES,"OC-Supported-Features"},			//620
	{DIAMETER_OC_FEATURE_VECTOR, 	"OC-Feature-Vector"},
	{DIAMETER_OC_PEER_ALGO,		"OC-Peer-Algo"},				//648
	{DIAMETER_SOURCEID,		"SourceID"},					//649

	{DIAMETER_RAT_TYPE,		"RAT-Type"},					//1007
	{DIAMETER_TERMINAL_INFORMATION,	"Terminal-Information"},			//1032

	{DIAMETER_ULR_FLAGS,		"ULR-Flags"},					//1405
	{DIAMETER_ULA_FLAGS,		"ULA-Flags"},					//1406
	{DIAMETER_VISITED_PLMN_ID,	"Visited-PLMN-Id"},				//1407
	{DIAMETER_SUBSCRIPTION_DATA,	"Subscription-Data"},
	{DIAMETER_REQUESTED_EUTRAN_AUTHENTICATION_INFO, 
					"Requested-EUTRAN-Authentication-Info"},
	{DIAMETER_REQUESTED_UTRAN_GERAN_AUTHENTICATION_INFO,
					"Requested-UTRAN-GRAN-Authentication-Info"},
	{DIAMETER_NUMBER_OF_REQUESTED_VECTORS, 
					"Number-Of-Requested-Vectors"},
	{DIAMETER_RE_SYNCHRONIZATION_INFO, 
					"Re-Synchronization-Info"},			//1411
	{DIAMETER_IMMEDIATE_RESPONSE_PREFERRED, 
					"Immediate-Response-Preferred"},		//1412
	{DIAMETER_CONTEXT_IDENTIFIER,	"Context-Identifier"},				//1423
	{DIAMETER_SGSN_NUMBER,		"SGSN-Numner"},					//1489
	{DIAMETER_HOMOGENEOUS_SUPPORT_OF_IMS_VOICE_OVER_PS_SESSIONS,
					"Homogeneous-Support-of-IMS-Voice-Over-PS-Sessions"},

	{DIAMETER_ACTIVE_APN,		"Active-APN"},					//1612
	{DIAMETER_UE_SRVCC_CAPABILITY,	"UE-SRVCC-Capability"},				//1615
	{DIAMETER_EQUIVALENT_PLMN_LIST,	"Equivalent-PLMN-List"},			//1637
	{DIAMETER_MME_NUMBER_FOR_MT_SMS,"MME-Number-for-MT-SMS"},			//1645
	{DIAMETER_SMS_REGISTER_REQUEST,	"SMS-Register-Request"},			//1648
	{DIAMETER_SGS_MME_IDENTITY,	"SGs-MME-Identity"},				//1664
	{DIAMETER_COUPLED_NODE_DIAMETER_ID,
					"Coupled-Node-Diameter-ID"},			//1666

	{DIAMETER_GMLC_ADDRESS, 	"GMLC-Address"}					//2405

};


int _serialize_diameter_avp( struct diameter_avp *avp, char *buf);

static int _create_os_avp( struct diameter_avp *avp, char *val, int val_sz)
{
	avp->data.octetstring = malloc( val_sz + 1);
	memcpy( avp->data.octetstring, val, val_sz);
	avp->data.octetstring[ val_sz] = '\0';


	AVP_HEADER(avp).length += val_sz;
	avp->pad = AVP_DATA_PAD( avp->data.octetstring);

	free(val);
	return 0;
}
static int _create_i32_avp( struct diameter_avp *avp, int32_t val, int val_sz)
{
	avp->data.int32 = val;
	AVP_HEADER(avp).length += sizeof( int32_t);
	return 0;
}
static int _create_i64_avp( struct diameter_avp *avp, int64_t val, int val_sz)
{
	avp->data.int64 = val;
	AVP_HEADER(avp).length += sizeof( int64_t);
	return 0;
}
static int _create_u32_avp( struct diameter_avp *avp, uint32_t val, int val_sz)
{
	avp->data.unsigned32 = val;
	AVP_HEADER(avp).length += sizeof( uint32_t);
	return 0;
}
static int _create_u64_avp( struct diameter_avp *avp, uint64_t val, int val_sz)
{
	avp->data.unsigned64 = val;
	AVP_HEADER(avp).length += sizeof( uint64_t);
	return 0;
}
static int _create_grouped_avp( struct diameter_avp *avp, struct diameter_avp **arr, int val_sz)
{
	avp->data.group = arr;
	AVP_HEADER(avp).length += val_sz;
	return 0;
}

static int _create_avp_data( struct diameter_avp *avp, avp_type type, uint64_t data, int data_sz)
{
	switch( type)
	{
		case Unknown:
			break;
		case OctetString:
			_create_os_avp( avp, ( char *)data, data_sz);
			break;
		case Integer32:
			_create_i32_avp( avp, (int32_t)data, sizeof( int32_t));
			break;
		case Integer64:
			_create_i64_avp( avp, ( int64_t )data, sizeof( int64_t));
			break;
		case Unsigned32:
			_create_u32_avp( avp, ( uint32_t)data, sizeof( uint32_t));
			break;
		case Unsigned64:
			_create_u64_avp( avp, ( uint64_t)data, sizeof( uint64_t));
			break;
		case Float32:

			break;
		case Float64:

			break;
		case Grouped:
			_create_grouped_avp( avp, ( struct diameter_avp **)data, data_sz);

			break;
	}
}
struct diameter_avp *diameter_new_avp( avp_type type,  unsigned int code, 
		unsigned char flags, uint64_t data, int data_sz, unsigned int vendor_id)
{
	struct diameter_avp *avp;
	avp = malloc( sizeof( struct diameter_avp));
	memset( avp, 0, sizeof( struct diameter_avp));
	
	avp->type = type;
	AVP_HEADER( avp).length = sizeof( struct diameter_avp_hdr);

	AVP_HEADER( avp).code = code;
	AVP_HEADER( avp).flags = flags;

	if( flags & 0x80)
	{
		AVP_HEADER( avp).length += sizeof( unsigned int);
		avp->vendor_id = vendor_id;
	}

	_create_avp_data( avp, type, data, data_sz);

	return avp;
}

struct diameter_pkt *diameter_new_packet()
{	
	struct diameter_pkt *pkt = 
		calloc(1, sizeof(struct diameter_pkt));
	pkt->header.version = 1;
	pkt->header.length = 20;
	pkt->header.flags = 0x80;
	pkt->header.application_id = 0;
	return pkt;
}


void diameter_insert_avp(struct diameter_pkt *pkt, struct diameter_avp *obj)
{
	int index = 0;
	while(pkt->list[index])
		index++;
	if(index > AVP_LIST_SIZE - 1)
		return;
	pkt->list[index] = obj;	
	pkt->lsize++;
	
	pkt->header.length += AVP_HEADER(obj).length;
	pkt->header.length += obj->pad;
}

void diameter_insert_avp_after(struct diameter_pkt *pkt, 
		struct diameter_avp *node, struct diameter_avp *newavp)
{
	struct diameter_avp *tmp;
	unsigned index = pkt->lsize;
	while(pkt->list[index] != node)
	{
		pkt->list[index + 1] = pkt->list[index];
		index--;
	
		if(!index) 
			return;
	}
	pkt->list[index + 1] = newavp;
}

void diameter_insert_avp_before(struct diameter_pkt *pkt, struct diameter_avp *avp)
{

}

void
diameter_swap_avp(struct diameter_pkt *pkt, 
		const unsigned int first, const unsigned int second)
{
	struct diameter_avp *tmp = 0;
	if(pkt->list[first] && pkt->list[second]);
	{
		tmp = pkt->list[first];
		pkt->list[first] = pkt->list[second];
		pkt->list[second] = tmp;
	}
}

void
diameter_remove_avp(struct diameter_pkt *pkt, unsigned short index)
{
	int i = 0;
	if(pkt->list[index])
	{
		pkt->header.length -= (pkt->list[index]->header.length +
					pkt->list[index]->pad);
		if(pkt->list[index]->type == OctetString)
			free(pkt->list[index]->data.octetstring);
		// TODO : handle grouped avp remove and free
		free(pkt->list[index]);
		i = index +1;

		while(pkt->list[i]){
			pkt->list[i - 1] = pkt->list[i];
			i++;
		}
		pkt->lsize--;
		pkt->list[pkt->lsize] = NULL;
	}
}

typedef int( *os_format_fn)( char **, char *);
//typedef char *( os_formatter_fn)( char *);
int vplmn_os_formatter( char **vplmn, char *i_str)
{
	
	int len = 3;
	*vplmn  = malloc(4);
	
	(*vplmn)[0] = ( i_str[0] - '0') << 4 | ( i_str[1] - '0');
	(*vplmn)[1] = ( i_str[2] - '0' < 9 ? (i_str[2] - '0') : 0x0f) << 4 | ( i_str[3] - '0');
	(*vplmn)[2] = ( i_str[4] - '0') << 4 | ( i_str[5] - '0');
	(*vplmn)[3] = '\0';

	return len;
}

int default_os_formatter( char **o_str, char *i_str)
{
	int len = strlen( i_str);

	*o_str = malloc( len + 1);
	memcpy( (*o_str), i_str, len);
	(*o_str)[len] = '\0';
	
	return len;
}


os_format_fn override_os_formatter( int type)
{
	switch(type)
	{
		case 1407:
			return vplmn_os_formatter;
		default:
			return default_os_formatter;
	}
}

int read_os_json_value( char **o_str, char *i_str, os_format_fn formatter)
{
	return formatter( o_str, i_str);
}

int _parse_json_avp_array( struct diameter_avp **arr, json_t *avp_arr)
{
	int			ret = 0;
	char 			*key;
	uint64_t		value;
	int			value_sz = 0;;
	unsigned 		avp_count = 0;
	int 			index = 0;
	avp_type		type;
	unsigned int		vendorId = 0;

	json_t 			*obj,
				*avp_obj, 
				*avp_type_obj, 
				*avp_hdr_obj,
				*avp_value_obj,
				*dim_avp_hdr_obj;

	unsigned int 		avpCode	= 0;
	unsigned char 		avpFlags = 0;

	avp_count = json_array_size( avp_arr);

	for( ; index < avp_count; index++)
	{
		avp_obj 	= json_array_get( avp_arr, index);
		avp_type_obj 	= json_object_get( avp_obj, "type");

		type = value_from_string(json_string_value(avp_type_obj), 
				avp_type_table);

		avpCode = json_integer_value(
			json_object_get( avp_obj, "code"));
		avpFlags = json_integer_value(
			json_object_get( avp_obj, "flags"));

		json_t *vendorObj = json_object_get( avp_obj, "vendor-id");
		if( vendorObj) 	{
			vendorId = json_integer_value( vendorObj);
			avpFlags = 0x80;
			ret += sizeof( vendorId);
		}

		avp_value_obj = json_object_get( avp_obj, "value");

		
		int integer = 0;
		switch( json_typeof( avp_value_obj))
		{
			case JSON_STRING:
			{
				value_sz  = read_os_json_value( ( char **)&value, 
					(char *)json_string_value( avp_value_obj),
					override_os_formatter(avpCode));
				break;
			}
			case JSON_INTEGER:
			{
				value = json_integer_value( avp_value_obj);
				if( type == Integer64 ||
					type == Unsigned64 ||
					type == Float64)
					value_sz = 8;
				else
					value_sz = 4;
				break;
			}
			case JSON_ARRAY:
			{
				int inner_arr_sz = json_array_size( avp_value_obj);
				struct diameter_avp **inner_arr = 
					malloc( ( inner_arr_sz +1)* 
						sizeof(void *));
				memset( inner_arr, 0, ( inner_arr_sz + 1)* sizeof( void *));

				value_sz = _parse_json_avp_array( inner_arr, avp_value_obj);
				value = ( uint64_t)inner_arr;
				break;
			}
		}

		ret += value_sz + AVP_HEADER_SIZE + ( 4 - value_sz %4)%4 ; // TODO : fix it

		arr[index] = diameter_new_avp( type, avpCode, avpFlags, 
			value, value_sz, vendorId);
		value = 0;

	}
	return ret;
}

int diameter_parse_json_avps( struct diameter_pkt *pkt, json_t *dim_obj)
{
	pkt->header.length += _parse_json_avp_array( pkt->list, 
		json_object_get( dim_obj, "avps"));
	return 0;
}

void diameter_parse_json_header( struct diameter_pkt *pkt, json_t *dim_obj)
{
	char 	*key;
	json_t 	*obj,
		*hdr_obj;
	
	hdr_obj = json_object_get( dim_obj, "common-header");

	json_object_foreach( hdr_obj, key, obj)
	{
		if( !strcmp( key, "version"))
			pkt->header.version = json_integer_value( obj);
		if( !strcmp( key, "flags"))
			pkt->header.flags = json_integer_value( obj);
		if( !strcmp( key, "command-code"))
			pkt->header.command_code = json_integer_value( obj);
		if( !strcmp( key, "application-id"))
			pkt->header.application_id = json_integer_value( obj);
		if( !strcmp( key, "hop-by-hop-id"))
			pkt->header.hop_by_hop_id = json_integer_value( obj);
		if( !strcmp( key, "end-to-end-id"))
			pkt->header.end_to_end_id = json_integer_value( obj);
	}
}
struct diameter_pkt *
diameter_read_json_packet( const char *buf)
{
	json_t 		*json,
			*dimObj;
	json_error_t 	error;
	struct diameter_pkt *pkt;

	json = json_loads( buf, 0, &error);
	if(!json){
		printf("Unable to parse buffer. line : %d, error :%s\n", 
			error.line, 
			error.text);
		return NULL;
	}
	
	pkt = diameter_new_packet();
	dimObj = json_object_get( json, "diameter");
	diameter_parse_json_header( pkt, dimObj);
	diameter_parse_json_avps( pkt, dimObj);

	json_decref( dimObj);
	json_decref( json);
	
	return pkt;
}


static int _serialize_octetstring( char *buf, char *ostr, int len, int pad)
{
	int ret = len;
	memcpy( buf, ostr, len);
	if(pad){
		memset( buf + ret, 0, pad);	
		ret += pad;
	}

	return ret;
}

static int _serialize_integer32( char *buf, int32_t val)
{
	// TODO : do we need network byte order or not?
	int ret = sizeof( int32_t);
	memcpy( buf, &val, ret);

	return ret;
}

static int _serialize_integer64( char *buf, int64_t val)
{
	int ret = sizeof(int64_t);
	memcpy( buf, &val, ret);

	return ret;
}

static int _serialize_unsigned32( char *buf, uint32_t val)
{
	int ret = sizeof( int32_t);
	memcpy( buf, &val, ret);

	return ret;
}

static int _serialize_unsigned64( char *buf, uint64_t val)
{
	int ret = sizeof( uint64_t);
	memcpy( buf, &val, ret);
	return ret;
}

static int _serialize_float32( char *buf, float val)
{
	int ret = sizeof( float);
	memcpy( buf, &val, ret);
	return ret;
}

static int _serialize_float64( char *buf, double val)
{
	int ret = sizeof( double);
	memcpy( buf, &val, ret);
	return ret;
}

static int _serialize_group( char *buf, struct diameter_avp **arr)
{
	int ret = 0, index = 0;

	while(arr[index])
	{
		ret += _serialize_diameter_avp( arr[index], buf + ret);
		index++;
	}
	return ret;
}

static int _serialize_avp_header( struct diameter_avp *avp, char *buf)
{
	int ret = 0;
	struct diameter_avp_hdr	hdr = {0};
	hdr.code = htonl(avp->header.code);
	hdr.flags = avp->header.flags;

	ret = sizeof(struct diameter_avp_hdr);

	if(avp->header.flags & 0x80){
		uint32_t vendor_hton = htonl(avp->vendor_id);
		memcpy( buf + ret, &vendor_hton, sizeof( uint32_t));
		ret += sizeof(uint32_t);
	}
	
	hdr.length = (( avp->header.length & 0x0000ff) << 16) 	|
		( avp->header.length & 0x00ff00)		|
		(( avp->header.length & 0xff0000) >> 16);


	memcpy( buf, &hdr, sizeof(struct diameter_avp_hdr));

	return ret;
}

static int _serialize_avp_data( struct diameter_avp *avp, char *buf)
{
	int ret = 0;
	switch( avp->type)
	{
		case OctetString:
		{
			int size = avp->header.length - AVP_HEADER_SIZE -
				((avp->header.flags & 0x80) ? 4: 0);
			ret = _serialize_octetstring( buf, avp->data.octetstring, 
				size , avp->pad);
			break;
		}
		case Integer32:
		{
			ret = _serialize_integer32( buf, avp->data.int32);
			break;
		}
		case Integer64:
		{
			ret = _serialize_integer64( buf, avp->data.int64);
			break;
		}
		case Unsigned32:
		{
			ret = _serialize_unsigned32( buf, avp->data.unsigned32);
			break;
		}
		case Unsigned64:
		{
			ret = _serialize_unsigned64( buf, avp->data.unsigned64);
			break;
		}
		case Float32:
		{
			ret = _serialize_float32( buf, avp->data.float32);
			break;
		}
		case Float64:
		{
			ret = _serialize_float64( buf, avp->data.float64);
			break;
		}
		case Grouped:
		{
			ret = _serialize_group( buf, avp->data.group);
			break;
		}
	}
	return ret;
}

int _serialize_diameter_avp( struct diameter_avp *avp, char *buf)
{
	int ret = 0;
	
	ret += _serialize_avp_header( avp, buf);
	ret += _serialize_avp_data( avp, buf + ret);
	
	return ret;
}
	
int diameter_serialize_packet( const struct diameter_pkt *pkt, char *buf)
{
	int 	i = 0,
		offt = 0; 

	struct diameter_hdr tmphdr = {0};

	tmphdr.version 		= pkt->header.version;
	tmphdr.length 		= ((pkt->header.length & 0x0000ff) << 16) 	|
				((pkt->header.length & 0x00ff00))		|
				((pkt->header.length & 0xff0000) >> 16);

	tmphdr.flags 		= pkt->header.flags;
	tmphdr.command_code 	= ((pkt->header.command_code & 0x0000ff) << 16) |
				((pkt->header.command_code & 0x00ff00))		|
				((pkt->header.command_code & 0xff0000) >> 16);

	tmphdr.application_id 	= htonl(pkt->header.application_id);
	tmphdr.hop_by_hop_id 	= htonl(pkt->header.hop_by_hop_id);
	tmphdr.end_to_end_id 	= htonl(pkt->header.end_to_end_id);
	
	memcpy(buf, &tmphdr, sizeof(struct diameter_hdr));
	offt += sizeof(struct diameter_hdr);
	
	for(; i < AVP_LIST_SIZE ; i++)
	{
		if(!(pkt->list[i]))
			break;
		offt += _serialize_diameter_avp(pkt->list[i], 
				buf + offt);
	}
	return offt;
}

void
diameter_deserialize_packet(const char *buf, int buf_size, struct diameter_pkt *pkt)
{
/*	int offt = 0;
	
	struct diameter_hdr tmp_dimhdr;

	memcpy(&tmp_dimhdr,buf ,sizeof(struct diameter_hdr));

	pkt->header.version = tmp_dimhdr.version;
	pkt->header.length = ntohl(tmp_dimhdr.length);
	pkt->header.length = 	((tmp_dimhdr.length & 0x0000ff) << 16)	|
				((tmp_dimhdr.length & 0x00ff00) << 8)	|
				((tmp_dimhdr.length & 0xff0000));

	pkt->header.flags = tmp_dimhdr.flags;
	pkt->header.command_code = 	((tmp_dimhdr.command_code & 0x0000ff) << 16) 	|
					((tmp_dimhdr.command_code & 0x00ff00) << 8)	|
					((tmp_dimhdr.command_code & 0xff0000));

	pkt->header.application_id = ntohl(tmp_dimhdr.application_id);
	pkt->header.hop_by_hop_id = ntohl(tmp_dimhdr.hop_by_hop_id);
	pkt->header.end_to_end_id = ntohl(tmp_dimhdr.end_to_end_id);

	offt += sizeof(struct diameter_hdr);


	struct diameter_avp_hdr tmp_avphdr;
	int data_sz;

	while(offt < buf_size)
	{
		memset(&tmp_avphdr, 0, sizeof(struct diameter_avp_hdr));
		struct diameter_avp *avp = malloc(sizeof(struct diameter_avp));
		
		memcpy(&tmp_avphdr, buf + offt, sizeof(struct diameter_avp_hdr));
		offt += sizeof(struct diameter_avp_hdr);
		
		avp->header.code = ntohl(tmp_avphdr.code);
		avp->header.flags = tmp_avphdr.flags;
		avp->header.length = 	((tmp_avphdr.length & 0x0000ff) << 16)	|
					((tmp_avphdr.length & 0x00ff00) << 8)	|
					((tmp_avphdr.length & 0xff0000));
		
		data_sz = avp->header.length - sizeof(struct diameter_avp_hdr);
		avp->pad = (4 - data_sz%4)%4;
	
		avp->data = malloc(data_sz + avp->pad);
		memcpy(avp->data, buf + offt, data_sz);
		offt += data_sz;
		if(avp->pad)
		{
			memset(avp->data, 0, avp->pad);
			offt += avp->pad;
		};

		diameter_insert_avp(pkt,avp);
	}
	*/
}


int diameter_send( struct diameter_pkt *diam_pkt, 
	const char *addr, uint16_t port, char proto, 
	uint64_t flags)
{
	char 		*buf = NULL;
	uint16_t 	buf_len = 0;
	int 		result;
	struct pkt_buffer *pkt;

	pkt = malloc( sizeof( struct pkt_buffer));
	
	pkt->tnl_idx 	= 0;
	pkt->data_len 	= diam_pkt->header.length;
	pkt->data 	= malloc( pkt->data_len);

	pkt->d_addr	= inet_addr(addr);
	pkt->l4_sport	= htons(4001);
	pkt->l4_dport	= htons(port);

	if( flags & NET_CAP_FRM)
		pkt->enable_cap = 1;
	if( flags & NET_ENBL_TNL)
		pkt->enable_tnl = 1;

	result = diameter_serialize_packet( diam_pkt, pkt->data);

	if( result != pkt->data_len)
	{
		printf("Something went wrong.");
		return -1;
	}

	sctp_send( pkt, SCTP_DATA_CHUNK, 3);
	
	return result;
}

void _print_avp( const struct diameter_avp **avp_l, int indent)
{	
	int index = 0;
	const struct diameter_avp *avp;

	while(avp_l[index])
	{
		printf("\n");
		avp = avp_l[index];
		char *avp_name = string_from_value(avp->header.code, 
					avp_display, 
					"Unknown");
		printf("\t\tAVP %d \t(%s)\n", index, avp_name);	
		printf("\t\tCode \t\t: %u\n", avp->header.code);
		printf("\t\tLength \t\t: %u\n", avp->header.length);
		printf("\t\t%s \t:",avp_name);

	
		switch(avp->type)
		{
			case OctetString:
				printf("\n\t\t\t%s\n", avp->data.octetstring);
				break;
			case Integer32:
				printf(" %d\n", avp->data.int32 );
				break;
			case Integer64:
				printf( " %ld\n", avp->data.int64);
			case Grouped:
				_print_avp( (const struct diameter_avp **)avp->data.group, 1);
				break;
		}
		index++;
	}
}

void
diameter_print_packet(const struct diameter_pkt *pkt)
{
	unsigned i;
	assert(pkt);
	printf("\t------------------Diameter-------------------\n");
	printf("\tVersion:%10d\n", pkt->header.version);
	printf("\tLength:\t%10u\n", pkt->header.length);
	printf("\tFlags:\n");
	printf("\t	------AVPS-------	\n");

	_print_avp( (const struct diameter_avp **)pkt->list, 0);
	printf("\n");
}

