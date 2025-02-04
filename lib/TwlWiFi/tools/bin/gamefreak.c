#include <nitro.h>
#include <nitroWiFi.h>

static char ca_name[] = "JP, Tokyo, GAME FREAK inc., gfnetwork, wbext.gamefreak.co.jp, gfnetwork@gamefreak.co.jp";

static unsigned char ca_modulo[] = {
	0xac,0x56,0x9b,0x18,0xfd,0xe0,0x91,0x87,
	0xe2,0x6d,0xf1,0xe1,0x85,0xa1,0xa7,0x33,
	0x56,0x6f,0xce,0x0f,0x9c,0x44,0x43,0xf2,
	0x40,0xd6,0x08,0x93,0xe6,0x65,0xba,0x6d,
	0xca,0x27,0x0f,0xa5,0x87,0x0f,0xd1,0xd7,
	0x08,0x0c,0x97,0x6b,0x62,0xed,0x34,0x42,
	0x9a,0x54,0x3c,0x2c,0x37,0xd3,0x8c,0xe9,
	0x94,0x50,0x26,0x96,0x70,0xcc,0x1b,0x2c,
	0x1d,0x8a,0x6a,0x0b,0x1d,0xc9,0x11,0x22,
	0x91,0xae,0xaf,0xdd,0xe8,0xab,0x22,0xa7,
	0xfc,0x3a,0x91,0x4f,0x42,0x96,0xbb,0xde,
	0x78,0x61,0xec,0x69,0xae,0xc6,0x4c,0xbe,
	0x48,0x3a,0x8a,0xf3,0xd5,0xa8,0xf6,0xe5,
	0x45,0xdf,0xb7,0xbf,0x03,0x1f,0xde,0x3d,
	0x56,0x16,0xd5,0x18,0xf5,0xeb,0xe2,0x61,
	0x13,0x4a,0x95,0x66,0x94,0xb3,0xf7,0x31,
	0xb6,0xa9,0x98,0x54,0xbf,0x41,0x92,0x36,
	0xe0,0x83,0x07,0xd2,0xf0,0xa1,0xc2,0x89,
	0xd1,0x2d,0x9e,0x00,0x95,0x99,0xa2,0x66,
	0x40,0x92,0x1b,0x98,0x32,0x60,0xdb,0x02,
	0xa4,0x34,0x96,0x85,0x43,0x39,0x8e,0x2a,
	0x5e,0xbb,0xff,0xbb,0xd6,0x25,0x25,0x99,
	0x29,0x45,0xe1,0x79,0x79,0xa0,0x42,0xd1,
	0x19,0xe9,0xb8,0x63,0x05,0xf8,0x15,0xa9,
	0x4d,0x70,0xcd,0x2e,0x4b,0xde,0x34,0x31,
	0x9d,0xea,0xf7,0xda,0x90,0xea,0x06,0xa4,
	0xec,0x10,0x82,0x7d,0xa3,0x41,0xdb,0x5e,
	0xb0,0x84,0x2e,0x94,0xca,0x97,0x96,0xc0,
	0x9c,0x44,0x79,0x75,0x1c,0xb7,0x1d,0x45,
	0x42,0x83,0xf6,0x67,0x67,0x84,0x53,0x78,
	0x10,0x00,0x41,0x30,0x6f,0x7f,0xe7,0x6e,
	0xee,0xd7,0xd5,0xf9,0xef,0x77,0xbc,0x09
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
