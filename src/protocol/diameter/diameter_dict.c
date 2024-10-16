#include "diameter_dict.h"
#include "diameter.h"
#include <stdlib.h>


char sample_session_id[] = {
	"bgdmmehw02.epc.mnc005.mcc418.3gppnetwork.org;0;1662824859;126634055"};
char sample_origin_host[] = {
	"bgdmmehw02.epc.mnc005.mcc418.3gppnetwork.org"};
char sample_origin_realm[] = {
	"epc.mnc005.mcc418.3gppnetwork.org"};
char sample_destination_host[] = {
// 0x85, 0x86, 0x87, 0x0a, 0x0d, 0x0c, 0x88, 0x89, 0x8a, 0x47, 0x03, 0x34, 0xff};
	"hfmda.epc.mnc011.mcc433.3gppnetwork.org"};
char sample_destination_realm[] = {
	"epc.mnc011.mcc433.3gppnetwork.org"};
char sample_user_name[] = {
	"432113933730951"};
char sample_visited_plmn_id[] = {0x14, 0xf8, 0x02};

char sample_host_ip_address[] = {"0.0.0.0"};

char sample_product_name[] = {"Sample-Product-Name"};

static const struct command_code_format ccf_dict[] = {
	{ "CER", 257, "Capability-Exchange-Request", 0,
		{
			DIAMETER_ORIGIN_HOST,
			DIAMETER_ORIGIN_REALM,
			DIAMETER_HOST_IP_ADDRESS,
			DIAMETER_VENDOR_ID,
			DIAMETER_PRODUCT_NAME,
			DIAMETER_ORIGIN_STATE_ID,
			DIAMETER_SUPPORTED_VENDOR_ID,
			DIAMETER_AUTH_APPLICATION_ID,
			DIAMETER_INBAND_SECURITY_ID,
			DIAMETER_ACCT_APPLICATION_ID,
			DIAMETER_VENDOR_SPECIFIC_APPLICATION_ID,
			DIAMETER_FIRMWARE_REVISION}},
	{ "RAR", 258, "Re-Auth", 0,
		{ 
			DIAMETER_SESSION_ID,
			DIAMETER_ORIGIN_HOST,
			DIAMETER_ORIGIN_REALM,
			DIAMETER_DESTINATION_REALM,
			DIAMETER_DESTINATION_HOST,
			DIAMETER_AUTH_APPLICATION_ID,
			DIAMETER_RE_AUTH_REQUEST_TYPE,
			DIAMETER_USER_NAME}},
	{ "ACR", 271, "Accounting-Request", 0},
	{ "", 274, "Abort-Session", 0},
	{ "", 275, "Session-Termination", 0},
	{ "", 280, "Device-Watchdo", 0},
	{ "", 282, "Disconnect-Peer", 0}, 
	/* SIP */
	{ "UAR", 283, "User-Authorization-Request", 0,
		{
			DIAMETER_SESSION_ID,
			DIAMETER_AUTH_APPLICATION_ID,
			DIAMETER_AUTH_SESSION_STATE,
			DIAMETER_ORIGIN_HOST,
			DIAMETER_ORIGIN_REALM,
			DIAMETER_DESTINATION_REALM,
			DIAMETER_SIP_AOR,
			DIAMETER_DESTINATION_HOST,
			DIAMETER_USER_NAME,
			DIAMETER_SIP_USER_AUTHORIZATION_TYPE,
			DIAMETER_PROXY_INFO,
			DIAMETER_ROUTE_RECORD}},
	{ "ULR", 316, "Update-Location-Request", 16777251, 
		{
		}},
	{ "ULA", 316, "Update-Location-Answer", 16777251,
		{
			DIAMETER_SESSION_ID,
			DIAMETER_RESULT_CODE,
			DIAMETER_EXPERIMENTAL_RESULT,
			DIAMETER_AUTH_SESSION_STATE,
			DIAMETER_ORIGIN_HOST,
			DIAMETER_ORIGIN_REALM,
//			DIAMETER_SUPPORTED_FEATURES,
			DIAMETER_ULA_FLAGS,
			DIAMETER_SUBSCRIPTION_DATA,
//			DIAMETER_AVP,
			DIAMETER_FAILED_AVP,
			DIAMETER_PROXY_INFO,
			DIAMETER_ROUTE_RECORD}},
	{ "AIR", 318, "Authentication-Information-Request", 16777251,
		{
			DIAMETER_SESSION_ID,
//			DIAMETER_AUTH_SESSION_STATE,
			DIAMETER_ORIGIN_HOST,
			DIAMETER_ORIGIN_REALM,
			DIAMETER_DESTINATION_HOST,
			DIAMETER_DESTINATION_REALM,
			DIAMETER_USER_NAME,
//			DIAMETER_REQUESTED_EUTRAN_AUTHENTICATION_INFO,
//			DIAMETER_REQUESTED_UTRAN_GERAN_AUTHENTICATION_INFO,
//			DIAMETER_HOST_IP_ADDRESS,

//			DIAMETER_ULA_FLAGS,	
			DIAMETER_VISITED_PLMN_ID}},
	{ "AIA", 318, "Authentication-Information-Answer", 16777251,
		{
			DIAMETER_SESSION_ID,
			DIAMETER_RESULT_CODE,
//			DIAMETER_EXPERIMENTAL_RESULT,
			DIAMETER_AUTH_SESSION_STATE,
			DIAMETER_ORIGIN_HOST,
			DIAMETER_ORIGIN_REALM,
//			DIAMETER_SUPPORTED_FEATURES,
//			DIAMETER_AUTHENTICATION_INFO,
//			DIAMETER_FAILED_AVP,
//			DIAMETER_PROXY_INFO,
//			DIAMETER_ROUT_RECORD
			}},
	{ "IDR", 319, "Insert-Subscriber-Data-Request", 16777251,
		{
			DIAMETER_SESSION_ID,
			DIAMETER_AUTH_SESSION_STATE,
			DIAMETER_ORIGIN_HOST,
			DIAMETER_ORIGIN_REALM,
			DIAMETER_DESTINATION_HOST,
			DIAMETER_DESTINATION_REALM,
			DIAMETER_USER_NAME,
//			DIAMETER_SUPPORTED_FEATURES,
			DIAMETER_SUBSCRIPTION_DATA,
			DIAMETER_IDR_FLAGS,
//			DIAMETER_AVP,
			DIAMETER_PROXY_INFO,
			DIAMETER_ROUTE_RECORD
		}}
};

static const struct diameter_avp avp_dict[] = {
	{	.header = {.code = DIAMETER_USER_NAME},
		.type = OctetString,
		.data = sample_user_name},
	{	.header = {.code = DIAMETER_SIP_AOR},
		.type = OctetString},
	{	.header = {.code = DIAMETER_HOST_IP_ADDRESS},
		.type = OctetString,
		.data = sample_host_ip_address },
	{	.header = {.code = DIAMETER_AUTH_APPLICATION_ID},
	 	.type = Unsigned32},
	{ 	.header = {.code = DIAMETER_ACCT_APPLICATION_ID},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_VENDOR_SPECIFIC_APPLICATION_ID},
	 	.type = Grouped},
	{	.header = {.code = DIAMETER_SESSION_ID}, 
	 	.type = OctetString, 	
		.data = sample_session_id},
	{ 	.header = {.code = DIAMETER_ORIGIN_HOST},
		.type = OctetString,
		.data = sample_origin_host},
	{	.header = {.code = DIAMETER_SUPPORTED_VENDOR_ID},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_VENDOR_ID},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_FIRMWARE_REVISION},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_RESULT_CODE},
		.type = Integer32},
	{	.header = {.code = DIAMETER_PRODUCT_NAME},
		.type = OctetString,
		.data = sample_product_name },
	{ 	.header = {.code = DIAMETER_RESULT_CODE},
		.type = Integer32},
	{	.header = {.code = DIAMETER_AUTH_SESSION_STATE},
		.type = Integer32},
	{	.header = {.code = DIAMETER_ORIGIN_STATE_ID},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_PROXY_HOST},
		.type = OctetString},
	{	.header = {.code = DIAMETER_ROUTE_RECORD},
		.type = OctetString},
	{	.header = {.code = DIAMETER_DESTINATION_REALM},
	 	.type = OctetString,
		.data = sample_destination_realm},
	{	.header = {.code = DIAMETER_EXPERIMENTAL_RESULT},
		.type = Grouped},
	{	.header = {.code = DIAMETER_PROXY_INFO},
	 	.type = Grouped},
	{	.header = {.code = DIAMETER_DESTINATION_HOST},
	 	.type = OctetString,
		.data = sample_destination_host},
	{	.header = {.code = DIAMETER_ORIGIN_REALM},
	 	.type = OctetString,
		.data = sample_origin_realm},
	{	.header = {.code = DIAMETER_INBAND_SECURITY_ID},
		.type = Integer32},
	{	.header = {.code = DIAMETER_SIP_ACCOUNTING_INFORMATION},
		.type = Grouped},
	{	.header = {.code = DIAMETER_SIP_ACCOUNTING_SERVER_URI},
		.type = OctetString},
	{	.header = {.code = DIAMETER_SIP_CREDIT_CONTROL_SERVER_URI},
		.type = OctetString},
	{	.header = {.code = DIAMETER_SIP_SERVER_URI},
		.type = OctetString},
	{	.header = {.code = DIAMETER_SIP_SERVER_CAPABILITIES},
		.type = Grouped},
	{	.header = {.code = DIAMETER_SIP_MANDATORY_CAPABILITY},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_SIP_OPTIONAL_CAPABILITY},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_SIP_SERVER_ASSIGNMENT_TYPE},
		.type = Integer32},
	{	.header = {.code = DIAMETER_SIP_AUTH_DATA_ITEM},
		.type = Grouped},
	{	.header = {.code = DIAMETER_SIP_AUTHENTICATION_SCHEME},
		.type = Integer32},
	{	.header = {.code = DIAMETER_SIP_ITEM_NUMBER},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_SIP_AUTHENTICATE},
		.type = Grouped},
	{	.header = {.code = DIAMETER_SIP_AUTHORIZATION},
		.type = Grouped},
	{	.header = {.code = DIAMETER_SIP_AUTHENTICATION_INFO},
		.type = Grouped},
	{	.header = {.code = DIAMETER_SIP_NUMBER_AUTH_ITEMS},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_SIP_DEREGISTRATION_REASON},
		.type = Grouped},
	{	.header = {.code = DIAMETER_SIP_REASON_CODE},
		.type = OctetString},
	{	.header = {.code = DIAMETER_SIP_REASON_INFO},
		.type = OctetString},
	{	.header = {.code = DIAMETER_SIP_VISITED_NETWORK_ID},
		.type = OctetString},
	{	.header = {.code = DIAMETER_SIP_USER_AUTHORIZATION_TYPE},
		.type = Integer32},
	{	.header = {.code = DIAMETER_SIP_SUPPORTED_USER_DATA_TYPE},
		.type = OctetString},
	{	.header = {.code = DIAMETER_SIP_USER_DATA}, 
		.type = Grouped},
	{	.header = {.code = DIAMETER_SIP_USER_DATA_TYPE},
		.type = OctetString},
	{	.header = {.code = DIAMETER_SIP_USER_DATA_CONTENTS},
		.type = OctetString},
	{	.header = {.code = DIAMETER_SIP_USER_DATA_ALREADY_AVAILABLE},
		.type = Integer32},
	{	.header = {.code = DIAMETER_SIP_METHOD},
		.type = OctetString},
	{	.header = {.code = DIAMETER_VISITED_PLMN_ID, .flags = 0x80},
		.type = OctetString,
		.data = sample_visited_plmn_id},
	{	.header = {.code = DIAMETER_ULA_FLAGS},
		.type = Unsigned32},
	{	.header = {.code = DIAMETER_REQUESTED_EUTRAN_AUTHENTICATION_INFO},
	 	.type = Grouped},
	{	.header = {.code = DIAMETER_APN_CONFIGURATION},
	 	.type = Grouped},
	{	.header = {.code = DIAMETER_ICS_INDICATOR},
		.type = Integer32},
	{	.header = {.code = DIAMETER_VPLMN_DYNAMIC_ADDRESS_ALLOWED},
	 	.type = Integer32},
	{	.header = {.code = DIAMETER_USER_ID},
	 	.type = OctetString},
	{	.header = {.code = DIAMETER_EVENT_THRESHOLD_RSRQ},
	 	.type = Unsigned32},
	{	.header = {.code = DIAMETER_PUR_FLAGS},
	 	.type = Unsigned32},
	{	.header = {.code = DIAMETER_A_MSISDN},
	 	.type = OctetString},
	{	.header = {.code = DIAMETER_GROUP_PLMN_ID},
	 	.type = OctetString},
	{	.header = {.code = DIAMETER_UE_REACHABILITY_CONFIGURATION},
	 	.type = Grouped},
	{	.header ={.code = DIAMETER_ACCURACY},
	 	.type = Unsigned32}
};

struct diameter_avp *diameter_avp_dict_code_lookup(unsigned int code)
{
	int i =0;
	struct diameter_avp *ret = NULL;
	for(;i < (sizeof(avp_dict)/sizeof(struct diameter_avp)); i++)
	{
		if(avp_dict[i].header.code != code)
			continue;
		
		ret = malloc(sizeof(struct diameter_avp));
		memset(ret, 0, sizeof(struct diameter_avp));

		AVP_HEADER(ret).code = code;
		AVP_HEADER(ret).length = sizeof(struct diameter_avp_hdr);
		AVP_HEADER(ret).flags = avp_dict[i].header.flags;
		
		if(AVP_HEADER(ret).flags & 0x80 ){
			AVP_HEADER(ret).length += sizeof(unsigned int);
			ret->vendor_id = 10415;
		}
		ret->type = avp_dict[i].type;
		switch(ret->type)
		{
			case OctetString:
				if(!avp_dict[i].data.octetstring)
					break;
				char *str = 
				ret->data.octetstring = (char *)malloc(strlen(str));
				memcpy(ret->data.octetstring, 
					avp_dict[i].data.octetstring,
					strlen(avp_dict[i].data.octetstring));
				ret->pad = (4 - strlen(avp_dict[i].data.octetstring)%4)%4;
				AVP_HEADER(ret).length += strlen(avp_dict[i].data.octetstring);
				break;
			case Float32:
				AVP_HEADER(ret).length += 4;
				ret->data.float32 = avp_dict[i].data.float32;
				break;
			case Integer32:
				AVP_HEADER(ret).length += 4;
				ret->data.int32 = avp_dict[i].data.int32;
				break;
			case Unsigned32:
				AVP_HEADER(ret).length += 4;
				ret->data.unsigned32 = avp_dict[i].data.unsigned32;
				break;
			case Float64:
				AVP_HEADER(ret).length += 8;
				ret->data.unsigned32 = avp_dict[i].data.unsigned32;
				break;
			case Unsigned64:
				AVP_HEADER(ret).length += 8;
				ret->data.unsigned64 = avp_dict[i].data.unsigned64;
				break;
			case Integer64:
				AVP_HEADER(ret).length += 8;
				ret->data.int64 = avp_dict[i].data.int64;
				break;
			case Grouped:
//				for(
//				diameter_avp_dict_code_lookup()
			default:
				break;
		}
	}
	return ret;
}


ccf_t *diameter_ccf_dict_code_lookup(char *id)
{
	int index = 0;
	for(; index < (sizeof(ccf_dict)/sizeof(ccf_t)); index++)
	{
		if(strncmp(ccf_dict[index].id, id, 3))
			continue;
		return &(ccf_dict[index]);
	}
	return NULL;
}

