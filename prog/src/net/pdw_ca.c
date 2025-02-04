#include <gflib.h>
#include <nitro.h>
#include <nitroWiFi/soc.h>
#include "net/network_define.h"

//本番用は不正検査と同じ場所に問い合わせ
#ifdef DEBUG_SERVER

static char ca_name[] = "JP, Tokyo, Minato-ku, bfs, pokemon-ds.basementfactorysystems.com";

static unsigned char ca_modulo[] = {
	0xe2,0xa6,0x1e,0x13,0xf6,0x47,0xb2,0x71,
	0xd2,0xfb,0x67,0x48,0x1c,0xdd,0x62,0x6b,
	0x90,0xad,0xed,0x49,0x71,0x98,0xe8,0x9d,
	0x1b,0x92,0x34,0x43,0x09,0xc1,0x6c,0xff,
	0x26,0xbd,0x61,0xf9,0x34,0x1c,0x94,0x34,
	0x22,0x6b,0xb1,0x22,0x9d,0x4a,0xa7,0xb5,
	0x41,0x24,0xd6,0xbd,0x46,0xbc,0x3e,0x54,
	0xd8,0xff,0xd7,0xd5,0x7c,0xee,0xb8,0xc2,
	0x64,0x03,0x16,0xc1,0x5d,0xd6,0xae,0x3c,
	0x6d,0x22,0x93,0x6b,0xb8,0xcc,0xa4,0xa2,
	0x43,0x86,0x52,0xfa,0x24,0xd5,0xd3,0x6c,
	0xdd,0x33,0x54,0xa3,0xa4,0x9d,0x6c,0xb4,
	0x73,0xba,0xf3,0x0a,0xd8,0x97,0x4a,0xda,
	0xb2,0x5a,0x15,0x13,0xa9,0x15,0xfc,0xa7,
	0x62,0xe3,0x7a,0x25,0xd2,0x85,0xa5,0x15,
	0xa3,0x0d,0xdd,0x7f,0x0a,0xc2,0x8b,0xb9
};

static unsigned char ca_exponent[] = {
	0x01,0x00,0x01
};

CPSCaInfo PDW_CA = {
	ca_name,
	sizeof(ca_modulo),
	ca_modulo,
	sizeof(ca_exponent),
	ca_exponent
};

#endif

