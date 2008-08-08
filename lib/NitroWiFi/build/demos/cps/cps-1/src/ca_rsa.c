#include <nitro.h>
#include <nitroWiFi.h>

static char ca_name[] = "US, RSA Data Security, Inc., Secure Server Certification Authority";

static unsigned char    ca_modulo[] = {
    0x92,0xce,0x7a,0xc1,0xae,0x83,0x3e,0x5a,
    0xaa,0x89,0x83,0x57,0xac,0x25,0x01,0x76,
    0x0c,0xad,0xae,0x8e,0x2c,0x37,0xce,0xeb,
    0x35,0x78,0x64,0x54,0x03,0xe5,0x84,0x40,
    0x51,0xc9,0xbf,0x8f,0x08,0xe2,0x8a,0x82,
    0x08,0xd2,0x16,0x86,0x37,0x55,0xe9,0xb1,
    0x21,0x02,0xad,0x76,0x68,0x81,0x9a,0x05,
    0xa2,0x4b,0xc9,0x4b,0x25,0x66,0x22,0x56,
    0x6c,0x88,0x07,0x8f,0xf7,0x81,0x59,0x6d,
    0x84,0x07,0x65,0x70,0x13,0x71,0x76,0x3e,
    0x9b,0x77,0x4c,0xe3,0x50,0x89,0x56,0x98,
    0x48,0xb9,0x1d,0xa7,0x29,0x1a,0x13,0x2e,
    0x4a,0x11,0x59,0x9c,0x1e,0x15,0xd5,0x49,
    0x54,0x2c,0x73,0x3a,0x69,0x82,0xb1,0x97,
    0x39,0x9c,0x6d,0x70,0x67,0x48,0xe5,0xdd,
    0x2d,0xd6,0xc8,0x1e,0x7b
};

static unsigned char    ca_exponent[] = {
    0x01,0x00,0x01
};

CPSCaInfo   ca_rsa = {
    ca_name,
    sizeof(ca_modulo),
    ca_modulo,
    sizeof(ca_exponent),
    ca_exponent
};
