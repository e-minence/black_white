/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wmPadRead-child
  File:     data.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: data.c,v $
  Revision 1.1  2007/04/18 09:27:37  kitase_hirotake
  Initial Upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

/*---------------------- パレットデータ  ---------------------------*/
#define RGB555(r,g,b) (b<<10|g<<5|r)

const u16 samplePlttData[16][16] = {
    {RGB555(31, 31, 31), RGB555(0, 0, 0), RGB555(0, 0, 0), RGB555(0, 0, 0),},   // Black
    {RGB555(31, 31, 31), RGB555(31, 0, 0), RGB555(31, 0, 0), RGB555(31, 0, 0),},        // Red
    {RGB555(31, 31, 31), RGB555(0, 31, 0), RGB555(0, 31, 0), RGB555(0, 31, 0),},        // Green
    {RGB555(31, 31, 31), RGB555(0, 0, 31), RGB555(0, 0, 31), RGB555(0, 0, 31),},        // Blue
    {RGB555(31, 31, 31), RGB555(31, 31, 0), RGB555(31, 31, 0), RGB555(31, 31, 0),},     // Yellow
    {RGB555(31, 31, 31), RGB555(0, 31, 31), RGB555(0, 31, 31), RGB555(0, 31, 31),},     // Cyan
    {RGB555(31, 31, 31), RGB555(31, 0, 31), RGB555(31, 0, 31), RGB555(31, 0, 31),},     // Purple
};


/*---------------------- キャラクタデータ  -------------------------*/

const u32 sampleCharData[8 * 0x100] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 0000h
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01010010, 0x01010010, 0x00000110,     // 0001h
    0x00011010, 0x01100010, 0x00000010, 0x00000010,
    0x00000000, 0x01011010, 0x01010010, 0x00010010,     // 0002h
    0x00100010, 0x00100010, 0x00100001, 0x00100001,
    0x00000000, 0x01010001, 0x01010001, 0x01111111,     // 0003h
    0x00000001, 0x00000001, 0x00000001, 0x01111110,
    0x00000000, 0x01010000, 0x01111111, 0x00100000,     // 0004h
    0x00100000, 0x00010000, 0x00001000, 0x00000110,
    0x00000000, 0x01010000, 0x01010100, 0x00001010,     // 0005h
    0x00010001, 0x00100001, 0x01000000, 0x00000000,
    0x00000000, 0x01011000, 0x01011000, 0x01111111,     // 0006h
    0x00001000, 0x00101010, 0x01001010, 0x01001001,
    0x00000000, 0x01010010, 0x01101111, 0x01010010,     // 0007h
    0x00010010, 0x00010010, 0x00010010, 0x00001001,
    0x00000000, 0x01010010, 0x01011111, 0x00000100,     // 0008h
    0x00011111, 0x00001000, 0x00000001, 0x00011110,
    0x00000000, 0x01010000, 0x01011000, 0x00000110,     // 0009h
    0x00000001, 0x00000110, 0x00011000, 0x00100000,
    0x00000000, 0x01010000, 0x01111101, 0x00010001,     // 000ah
    0x00010001, 0x00010001, 0x00010001, 0x00001010,
    0x00000000, 0x01010000, 0x01011110, 0x00100000,     // 000bh
    0x00000000, 0x00000001, 0x00000001, 0x00111110,
    0x00000000, 0x01010100, 0x01011111, 0x00001000,     // 000ch
    0x00010000, 0x00000001, 0x00000001, 0x00011110,
    0x00000000, 0x01010001, 0x01010001, 0x00000001,     // 000dh
    0x01000001, 0x01000001, 0x00100010, 0x00011100,
    0x00000000, 0x01010000, 0x01111111, 0x00011000,     // 000eh
    0x00010100, 0x00010100, 0x00011000, 0x00001100,
    0x00000000, 0x01010010, 0x01111111, 0x00010010,     // 000fh
    0x00010010, 0x00000010, 0x00000010, 0x00111100,
    0x00000000, 0x00001110, 0x01010100, 0x01010010,     // 0010h
    0x00111111, 0x00000100, 0x00000100, 0x00011000,
    0x00000000, 0x01010100, 0x01011111, 0x00000100,     // 0011h
    0x01110100, 0x00000010, 0x00001010, 0x01110010,
    0x00000000, 0x01010100, 0x01011111, 0x00000010,     // 0012h
    0x00011110, 0x00100001, 0x00100000, 0x00011110,
    0x00000000, 0x01010000, 0x01011100, 0x00100011,     // 0013h
    0x01000000, 0x01000000, 0x00100000, 0x00011100,
    0x00000000, 0x01010000, 0x01111111, 0x00010000,     // 0014h
    0x00001000, 0x00001000, 0x00001000, 0x00110000,
    0x00000000, 0x01010010, 0x01010010, 0x00001100,     // 0015h
    0x00000010, 0x00000001, 0x00000001, 0x00111110,
    0x00000000, 0x01010001, 0x01111101, 0x00010001,     // 0016h
    0x00010001, 0x00111001, 0x01010101, 0x00011001,
    0x00000000, 0x01010100, 0x01010011, 0x01110010,     // 0017h
    0x00010001, 0x00010001, 0x00001010, 0x00000100,
    0x00000000, 0x01011110, 0x01011000, 0x00000100,     // 0018h
    0x00101001, 0x01010001, 0x01010001, 0x00001100,
    0x00000000, 0x01010000, 0x01011100, 0x00010010,     // 0019h
    0x00010010, 0x00100001, 0x01000000, 0x00000000,
    0x00000000, 0x01011101, 0x01010001, 0x00111101,     // 001ah
    0x00010001, 0x00011001, 0x00110101, 0x00001001,
    0x00000000, 0x01110001, 0x01011101, 0x00110001,     // 001bh
    0x00010001, 0x00111001, 0x01010101, 0x00011001,
    0x00000000, 0x01110100, 0x01010011, 0x00110010,     // 001ch
    0x00010001, 0x00010001, 0x00001010, 0x00000100,
    0x00000000, 0x01101110, 0x01011000, 0x00100100,     // 001dh
    0x00101001, 0x01010001, 0x01010001, 0x00001100,
    0x00000000, 0x01110000, 0x01011100, 0x00110010,     // 001eh
    0x00010010, 0x00100001, 0x01000000, 0x00000000,
    0x00000000, 0x01111101, 0x01010001, 0x00111101,     // 001fh
    0x00010001, 0x00011001, 0x00110101, 0x00001001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 0020h
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00001000, 0x00001000, 0x00001000,     // 0021h
    0x00001000, 0x00001000, 0x00000000, 0x00001000,
    0x00000000, 0x01101100, 0x01001000, 0x00100100,     // 0022h
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00100100, 0x01111111, 0x00100100,     // 0023h
    0x00100100, 0x01111111, 0x00010010, 0x00010010,
    0x00000000, 0x00001000, 0x01111110, 0x00001001,     // 0024h
    0x00111110, 0x01001000, 0x00111111, 0x00001000,
    0x00000000, 0x01000010, 0x00100101, 0x00010010,     // 0025h
    0x00001000, 0x00100100, 0x01010010, 0x00100001,
    0x00000000, 0x00001110, 0x00010001, 0x00001001,     // 0026h
    0x01000110, 0x00101001, 0x00110001, 0x01001110,
    0x00000000, 0x00011000, 0x00010000, 0x00001000,     // 0027h
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01110000, 0x00001000, 0x00000100,     // 0028h
    0x00000100, 0x00000100, 0x00001000, 0x01110000,
    0x00000000, 0x00000111, 0x00001000, 0x00010000,     // 0029h
    0x00010000, 0x00010000, 0x00001000, 0x00000111,
    0x00000000, 0x00001000, 0x01001001, 0x00101010,     // 002ah
    0x00011100, 0x00101010, 0x01001001, 0x00001000,
    0x00000000, 0x00001000, 0x00001000, 0x00001000,     // 002bh
    0x01111111, 0x00001000, 0x00001000, 0x00001000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 002ch
    0x00000000, 0x00001100, 0x00001000, 0x00000100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 002dh
    0x01111111, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 002eh
    0x00000000, 0x00000000, 0x00000000, 0x00001100,
    0x00000000, 0x01000000, 0x00100000, 0x00010000,     // 002fh
    0x00001000, 0x00000100, 0x00000010, 0x00000001,
    0x00000000, 0x00111110, 0x01000001, 0x01000001,     // 0030h
    0x01000001, 0x01000001, 0x01000001, 0x00111110,
    0x00000000, 0x00011100, 0x00010000, 0x00010000,     // 0031h
    0x00010000, 0x00010000, 0x00010000, 0x00010000,
    0x00000000, 0x00111110, 0x01000001, 0x01000000,     // 0032h
    0x00111110, 0x00000001, 0x00000001, 0x01111111,
    0x00000000, 0x00111110, 0x01000001, 0x01000000,     // 0033h
    0x00111110, 0x01000000, 0x01000001, 0x00111110,
    0x00000000, 0x00100000, 0x00110000, 0x00101000,     // 0034h
    0x00100100, 0x00100010, 0x01111111, 0x00100000,
    0x00000000, 0x01111111, 0x00000001, 0x00111111,     // 0035h
    0x01000000, 0x01000000, 0x01000001, 0x00111110,
    0x00000000, 0x00111110, 0x00000001, 0x00111111,     // 0036h
    0x01000001, 0x01000001, 0x01000001, 0x00111110,
    0x00000000, 0x01111111, 0x00100000, 0x00100000,     // 0037h
    0x00010000, 0x00010000, 0x00001000, 0x00001000,
    0x00000000, 0x00111110, 0x01000001, 0x01000001,     // 0038h
    0x00111110, 0x01000001, 0x01000001, 0x00111110,
    0x00000000, 0x00111110, 0x01000001, 0x01000001,     // 0039h
    0x01000001, 0x01111110, 0x01000000, 0x00111110,
    0x00000000, 0x00000000, 0x00001100, 0x00000000,     // 003ah
    0x00000000, 0x00000000, 0x00001100, 0x00000000,
    0x00000000, 0x00000000, 0x00001100, 0x00000000,     // 003bh
    0x00000000, 0x00001100, 0x00001000, 0x00000100,
    0x00000000, 0x01100000, 0x00011000, 0x00000110,     // 003ch
    0x00000001, 0x00000110, 0x00011000, 0x01100000,
    0x00000000, 0x00000000, 0x01111111, 0x00000000,     // 003dh
    0x00000000, 0x00000000, 0x01111111, 0x00000000,
    0x00000000, 0x00000011, 0x00001100, 0x00110000,     // 003eh
    0x01000000, 0x00110000, 0x00001100, 0x00000011,
    0x00000000, 0x00111110, 0x01000001, 0x01000001,     // 003fh
    0x00110000, 0x00001000, 0x00000000, 0x00001000,
    0x00000000, 0x00011100, 0x00100010, 0x01001001,     // 0040h
    0x01010101, 0x01010101, 0x01010101, 0x00111010,
    0x00000000, 0x00001000, 0x00010100, 0x00010100,     // 0041h
    0x00100010, 0x00111110, 0x01000001, 0x01000001,
    0x00000000, 0x00111111, 0x01000001, 0x01000001,     // 0042h
    0x00111111, 0x01000001, 0x01000001, 0x00111111,
    0x00000000, 0x00111100, 0x01000010, 0x00000001,     // 0043h
    0x00000001, 0x00000001, 0x01000010, 0x00111100,
    0x00000000, 0x00011111, 0x00100001, 0x01000001,     // 0044h
    0x01000001, 0x01000001, 0x00100001, 0x00011111,
    0x00000000, 0x01111111, 0x00000001, 0x00000001,     // 0045h
    0x01111111, 0x00000001, 0x00000001, 0x01111111,
    0x00000000, 0x01111111, 0x00000001, 0x00000001,     // 0046h
    0x00111111, 0x00000001, 0x00000001, 0x00000001,
    0x00000000, 0x00111100, 0x01000010, 0x00000001,     // 0047h
    0x01111001, 0x01000001, 0x01000010, 0x00111100,
    0x00000000, 0x01000001, 0x01000001, 0x01000001,     // 0048h
    0x01111111, 0x01000001, 0x01000001, 0x01000001,
    0x00000000, 0x00111110, 0x00001000, 0x00001000,     // 0049h
    0x00001000, 0x00001000, 0x00001000, 0x00111110,
    0x00000000, 0x01000000, 0x01000000, 0x01000000,     // 004ah
    0x01000001, 0x01000001, 0x00100010, 0x00011100,
    0x00000000, 0x01100001, 0x00011001, 0x00000101,     // 004bh
    0x00000011, 0x00000101, 0x00011001, 0x01100001,
    0x00000000, 0x00000001, 0x00000001, 0x00000001,     // 004ch
    0x00000001, 0x00000001, 0x00000001, 0x01111111,
    0x00000000, 0x01000001, 0x01100011, 0x01010101,     // 004dh
    0x01001001, 0x01000001, 0x01000001, 0x01000001,
    0x00000000, 0x01000001, 0x01000011, 0x01000101,     // 004eh
    0x01001001, 0x01010001, 0x01100001, 0x01000001,
    0x00000000, 0x00011100, 0x00100010, 0x01000001,     // 004fh
    0x01000001, 0x01000001, 0x00100010, 0x00011100,
    0x00000000, 0x00111111, 0x01000001, 0x01000001,     // 0050h
    0x00111111, 0x00000001, 0x00000001, 0x00000001,
    0x00000000, 0x00011100, 0x00100010, 0x01000001,     // 0051h
    0x01000001, 0x01011001, 0x00100010, 0x01011100,
    0x00000000, 0x00111111, 0x01000001, 0x01000001,     // 0052h
    0x00111111, 0x01000001, 0x01000001, 0x01000001,
    0x00000000, 0x00111110, 0x01000001, 0x00000001,     // 0053h
    0x00111110, 0x01000000, 0x01000001, 0x00111110,
    0x00000000, 0x01111111, 0x00001000, 0x00001000,     // 0054h
    0x00001000, 0x00001000, 0x00001000, 0x00001000,
    0x00000000, 0x01000001, 0x01000001, 0x01000001,     // 0055h
    0x01000001, 0x01000001, 0x00100010, 0x00011100,
    0x00000000, 0x01000001, 0x01000001, 0x00100010,     // 0056h
    0x00100010, 0x00010100, 0x00010100, 0x00001000,
    0x00000000, 0x01000001, 0x01000001, 0x01000001,     // 0057h
    0x01001001, 0x01010101, 0x01100011, 0x01000001,
    0x00000000, 0x01000001, 0x00100010, 0x00010100,     // 0058h
    0x00001000, 0x00010100, 0x00100010, 0x01000001,
    0x00000000, 0x01000001, 0x00100010, 0x00010100,     // 0059h
    0x00001000, 0x00001000, 0x00001000, 0x00001000,
    0x00000000, 0x01111111, 0x00100000, 0x00010000,     // 005ah
    0x00001000, 0x00000100, 0x00000010, 0x01111111,
    0x00000000, 0x01111100, 0x00000100, 0x00000100,     // 005bh
    0x00000100, 0x00000100, 0x00000100, 0x01111100,
    0x00000000, 0x00100010, 0x00010100, 0x00111110,     // 005ch
    0x00001000, 0x00111110, 0x00001000, 0x00001000,
    0x00000000, 0x00011111, 0x00010000, 0x00010000,     // 005dh
    0x00010000, 0x00010000, 0x00010000, 0x00011111,
    0x00000000, 0x00001000, 0x00010100, 0x00100010,     // 005eh
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 005fh
    0x00000000, 0x00000000, 0x00000000, 0x01111111,
    0x00000000, 0x00010000, 0x00001000, 0x00011000,     // 0060h
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00011110, 0x00100001,     // 0061h
    0x00111110, 0x00100001, 0x00100001, 0x01011110,
    0x00000000, 0x00000001, 0x00000001, 0x00111111,     // 0062h
    0x01000001, 0x01000001, 0x01000001, 0x00111111,
    0x00000000, 0x00000000, 0x00111100, 0x01000010,     // 0063h
    0x00000001, 0x00000001, 0x01000010, 0x00111100,
    0x00000000, 0x01000000, 0x01000000, 0x01111110,     // 0064h
    0x01000001, 0x01000001, 0x01000001, 0x01111110,
    0x00000000, 0x00000000, 0x00111110, 0x01000001,     // 0065h
    0x01111111, 0x00000001, 0x01000001, 0x00111110,
    0x00000000, 0x00110000, 0x00001000, 0x00001000,     // 0066h
    0x01111111, 0x00001000, 0x00001000, 0x00001000,
    0x00000000, 0x00000000, 0x01111110, 0x01000001,     // 0067h
    0x01000001, 0x01111110, 0x01000000, 0x00111110,
    0x00000000, 0x00000001, 0x00000001, 0x00000001,     // 0068h
    0x00111111, 0x01000001, 0x01000001, 0x01000001,
    0x00000000, 0x00001000, 0x00000000, 0x00001000,     // 0069h
    0x00001000, 0x00001000, 0x00001000, 0x00001000,
    0x00000000, 0x00100000, 0x00000000, 0x00100000,     // 006ah
    0x00100000, 0x00100001, 0x00100001, 0x00011110,
    0x00000000, 0x00000001, 0x00000001, 0x01100001,     // 006bh
    0x00011001, 0x00000111, 0x00011001, 0x01100001,
    0x00000000, 0x00001000, 0x00001000, 0x00001000,     // 006ch
    0x00001000, 0x00001000, 0x00001000, 0x00001000,
    0x00000000, 0x00000000, 0x00110111, 0x01001001,     // 006dh
    0x01001001, 0x01001001, 0x01001001, 0x01001001,
    0x00000000, 0x00000000, 0x00111111, 0x01000001,     // 006eh
    0x01000001, 0x01000001, 0x01000001, 0x01000001,
    0x00000000, 0x00000000, 0x00011100, 0x00100010,     // 006fh
    0x01000001, 0x01000001, 0x00100010, 0x00011100,
    0x00000000, 0x00000000, 0x00111101, 0x01000011,     // 0070h
    0x01000001, 0x01000011, 0x00111101, 0x00000001,
    0x00000000, 0x00000000, 0x01011110, 0x01100001,     // 0071h
    0x01000001, 0x01100001, 0x01011110, 0x01000000,
    0x00000000, 0x00000000, 0x00110001, 0x00001101,     // 0072h
    0x00000011, 0x00000001, 0x00000001, 0x00000001,
    0x00000000, 0x00000000, 0x00111110, 0x01000001,     // 0073h
    0x00001110, 0x00110000, 0x01000001, 0x00111110,
    0x00000000, 0x00000100, 0x00000100, 0x01111111,     // 0074h
    0x00000100, 0x00000100, 0x00000100, 0x01111000,
    0x00000000, 0x00000000, 0x01000001, 0x01000001,     // 0075h
    0x01000001, 0x01000001, 0x01000001, 0x01111110,
    0x00000000, 0x00000000, 0x01000001, 0x01000001,     // 0076h
    0x00100010, 0x00100010, 0x00010100, 0x00001000,
    0x00000000, 0x00000000, 0x01000001, 0x01000001,     // 0077h
    0x01001001, 0x00101010, 0x00101010, 0x00010100,
    0x00000000, 0x00000000, 0x00100001, 0x00010010,     // 0078h
    0x00001100, 0x00001100, 0x00010010, 0x00100001,
    0x00000000, 0x00000000, 0x01000001, 0x01000001,     // 0079h
    0x00100010, 0x00011100, 0x00001000, 0x00000110,
    0x00000000, 0x00000000, 0x00111111, 0x00010000,     // 007ah
    0x00001000, 0x00000100, 0x00000010, 0x00111111,
    0x00000000, 0x00001000, 0x00011110, 0x01100100,     // 007bh
    0x00011000, 0x00100100, 0x00000100, 0x01111000,
    0x00000000, 0x00000000, 0x00011110, 0x00000100,     // 007ch
    0x00011110, 0x00110101, 0x00101101, 0x00010010,
    0x00000000, 0x00000000, 0x00000000, 0x00010001,     // 007dh
    0x00100001, 0x00100001, 0x00000001, 0x00000010,
    0x00000000, 0x00000000, 0x00011100, 0x00000000,     // 007eh
    0x00011110, 0x00100000, 0x00100000, 0x00011100,
    0x00000000, 0x00000000, 0x00011100, 0x00000000,     // 007fh
    0x00111110, 0x00010000, 0x00001100, 0x00110010,
    0x00000000, 0x00000000, 0x00000100, 0x00101111,     // 0080h
    0x01000100, 0x00011110, 0x00100101, 0x00010110,
    0x00000000, 0x00000000, 0x00001010, 0x00011110,     // 0081h
    0x00101011, 0x00100010, 0x00010100, 0x00000100,
    0x00000000, 0x00000000, 0x00001000, 0x00011101,     // 0082h
    0x00101011, 0x00101001, 0x00011001, 0x00000100,
    0x00000000, 0x00000000, 0x00001000, 0x00111000,     // 0083h
    0x00001000, 0x00011110, 0x00101001, 0x00000110,
    0x00000000, 0x00000000, 0x00000000, 0x00011100,     // 0084h
    0x00100011, 0x00100000, 0x00100000, 0x00011100,
    0x00000000, 0x00000110, 0x01001001, 0x00110000,     // 0085h
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000100, 0x00111111, 0x00000100,     // 0086h
    0x00111110, 0x01010101, 0x01001101, 0x00100110,
    0x00000000, 0x00000000, 0x00100001, 0x01000001,     // 0087h
    0x01000001, 0x01000001, 0x00000001, 0x00000010,
    0x00000000, 0x00111100, 0x00000000, 0x00111110,     // 0088h
    0x01000000, 0x01000000, 0x00100000, 0x00011100,
    0x00000000, 0x00011100, 0x00000000, 0x00111110,     // 0089h
    0x00010000, 0x00001000, 0x00010100, 0x01100010,
    0x00000000, 0x00100100, 0x01011111, 0x00000100,     // 008ah
    0x00111110, 0x01000101, 0x01000101, 0x00100010,
    0x00000000, 0x00100010, 0x01001111, 0x01010010,     // 008bh
    0x01010010, 0x00010010, 0x00010010, 0x00001001,
    0x00000000, 0x00000100, 0x00111110, 0x00001000,     // 008ch
    0x00111110, 0x00010000, 0x00000010, 0x00111100,
    0x00000000, 0x00100000, 0x00011000, 0x00000110,     // 008dh
    0x00000001, 0x00000110, 0x00011000, 0x00100000,
    0x00000000, 0x00100000, 0x01111101, 0x00100001,     // 008eh
    0x00100001, 0x00100001, 0x00100001, 0x00010010,
    0x00000000, 0x00011110, 0x00100000, 0x00000000,     // 008fh
    0x00000000, 0x00000001, 0x00000001, 0x00111110,
    0x00000000, 0x00001000, 0x01111111, 0x00010000,     // 0090h
    0x00100000, 0x00000010, 0x00000010, 0x00111100,
    0x00000000, 0x00000001, 0x00000001, 0x00000001,     // 0091h
    0x01000001, 0x01000001, 0x00100010, 0x00011100,
    0x00000000, 0x00010000, 0x01111111, 0x00011000,     // 0092h
    0x00010100, 0x00010100, 0x00011000, 0x00001100,
    0x00000000, 0x00100010, 0x01111111, 0x00100010,     // 0093h
    0x00100010, 0x00000010, 0x00000010, 0x01111100,
    0x00000000, 0x00111100, 0x00010000, 0x00001100,     // 0094h
    0x01111111, 0x00001000, 0x00001000, 0x00110000,
    0x00000000, 0x00000100, 0x00011111, 0x00000100,     // 0095h
    0x01110100, 0x00000010, 0x00001010, 0x01110010,
    0x00000000, 0x00001000, 0x01111111, 0x00000100,     // 0096h
    0x00111100, 0x01000010, 0x01000000, 0x00111100,
    0x00000000, 0x00000000, 0x00011100, 0x00100011,     // 0097h
    0x01000000, 0x01000000, 0x00100000, 0x00011100,
    0x00000000, 0x01111111, 0x00010000, 0x00001000,     // 0098h
    0x00001000, 0x00001000, 0x00001000, 0x00110000,
    0x00000000, 0x00000010, 0x00110010, 0x00001100,     // 0099h
    0x00000010, 0x00000001, 0x00000001, 0x00111110,
    0x00000000, 0x00100100, 0x01001111, 0x01000010,     // 009ah
    0x00010001, 0x00111100, 0x00010010, 0x00001100,
    0x00000000, 0x00000010, 0x01111010, 0x01000010,     // 009bh
    0x00000010, 0x00000010, 0x00001010, 0x01110010,
    0x00000000, 0x00100010, 0x00111110, 0x01010010,     // 009ch
    0x01001011, 0x01101101, 0x01010101, 0x00110010,
    0x00000000, 0x00110010, 0x01001011, 0x01000110,     // 009dh
    0x01000110, 0x01110010, 0x01001011, 0x00110010,
    0x00000000, 0x00011100, 0x00101010, 0x01001001,     // 009eh
    0x01001001, 0x01000101, 0x01000101, 0x00110010,
    0x00000000, 0x00100001, 0x01111101, 0x00100001,     // 009fh
    0x00100001, 0x00111001, 0x01100101, 0x00011001,
    0x00000000, 0x00000100, 0x00100011, 0x01100010,     // 00a0h
    0x00100001, 0x00100001, 0x00010010, 0x00001100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 00a1h
    0x00000000, 0x00000100, 0x00001010, 0x00000100,
    0x00000000, 0x01110000, 0x00010000, 0x00010000,     // 00a2h
    0x00010000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 00a3h
    0x00001000, 0x00001000, 0x00001000, 0x00001110,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 00a4h
    0x00000000, 0x00000010, 0x00000100, 0x00000100,
    0x00000000, 0x00000000, 0x00000000, 0x00011000,     // 00a5h
    0x00011000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01111111, 0x01000000, 0x01111111,     // 00a6h
    0x01000000, 0x01000000, 0x00100000, 0x00011100,
    0x00000000, 0x00000000, 0x00111111, 0x00100000,     // 00a7h
    0x00010100, 0x00001100, 0x00000100, 0x00000010,
    0x00000000, 0x00000000, 0x00100000, 0x00100000,     // 00a8h
    0x00010000, 0x00001111, 0x00001000, 0x00001000,
    0x00000000, 0x00000000, 0x00000100, 0x00111111,     // 00a9h
    0x00100001, 0x00100000, 0x00010000, 0x00001100,
    0x00000000, 0x00000000, 0x00000000, 0x00111110,     // 00aah
    0x00001000, 0x00001000, 0x00001000, 0x01111111,
    0x00000000, 0x00000000, 0x00010000, 0x00111111,     // 00abh
    0x00011000, 0x00010100, 0x00010010, 0x00011001,
    0x00000000, 0x00000000, 0x00000010, 0x00111111,     // 00ach
    0x00100010, 0x00010010, 0x00000100, 0x00000100,
    0x00000000, 0x00000000, 0x00000000, 0x00111110,     // 00adh
    0x00100000, 0x00100000, 0x00100000, 0x01111111,
    0x00000000, 0x00000000, 0x00111110, 0x00100000,     // 00aeh
    0x00111110, 0x00100000, 0x00100000, 0x00111110,
    0x00000000, 0x00000000, 0x00100101, 0x00101010,     // 00afh
    0x00101010, 0x00100000, 0x00010000, 0x00001110,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 00b0h
    0x01111111, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01111111, 0x01000000, 0x00101000,     // 00b1h
    0x00011000, 0x00001000, 0x00001000, 0x00000100,
    0x00000000, 0x01000000, 0x00100000, 0x00011000,     // 00b2h
    0x00010111, 0x00010000, 0x00010000, 0x00010000,
    0x00000000, 0x00001000, 0x01111111, 0x01000001,     // 00b3h
    0x01000001, 0x01000000, 0x00100000, 0x00011000,
    0x00000000, 0x00000000, 0x00111110, 0x00001000,     // 00b4h
    0x00001000, 0x00001000, 0x00001000, 0x01111111,
    0x00000000, 0x00100000, 0x01111111, 0x00110000,     // 00b5h
    0x00101000, 0x00100100, 0x00100010, 0x00110001,
    0x00000000, 0x00000100, 0x01111111, 0x01000100,     // 00b6h
    0x01000100, 0x01000100, 0x01000010, 0x00100001,
    0x00000000, 0x00000100, 0x00111111, 0x00001000,     // 00b7h
    0x01111111, 0x00010000, 0x00010000, 0x00010000,
    0x00000000, 0x01111100, 0x01000100, 0x01000100,     // 00b8h
    0x01000010, 0x01000000, 0x00100000, 0x00011000,
    0x00000000, 0x00000010, 0x01111110, 0x00100010,     // 00b9h
    0x00100001, 0x00100000, 0x00010000, 0x00001100,
    0x00000000, 0x01111110, 0x01000000, 0x01000000,     // 00bah
    0x01000000, 0x01000000, 0x01000000, 0x01111110,
    0x00000000, 0x00100010, 0x01111111, 0x00100010,     // 00bbh
    0x00100010, 0x00100000, 0x00010000, 0x00001100,
    0x00000000, 0x00000011, 0x00000100, 0x01000011,     // 00bch
    0x01000100, 0x00100000, 0x00011000, 0x00000111,
    0x00000000, 0x01111111, 0x01000000, 0x00100000,     // 00bdh
    0x00010000, 0x00011000, 0x00100100, 0x01000011,
    0x00000000, 0x00000010, 0x01111111, 0x01000010,     // 00beh
    0x00100010, 0x00000010, 0x00000010, 0x01111100,
    0x00000000, 0x01000001, 0x01000010, 0x01000000,     // 00bfh
    0x00100000, 0x00100000, 0x00011000, 0x00000110,
    0x00000000, 0x01111110, 0x01000010, 0x01001110,     // 00c0h
    0x01110001, 0x01000000, 0x00100000, 0x00011000,
    0x00000000, 0x01100000, 0x00011110, 0x00010000,     // 00c1h
    0x01111111, 0x00010000, 0x00010000, 0x00001100,
    0x00000000, 0x01000101, 0x01001010, 0x01001010,     // 00c2h
    0x01000000, 0x00100000, 0x00010000, 0x00001110,
    0x00000000, 0x00111110, 0x00000000, 0x01111111,     // 00c3h
    0x00010000, 0x00010000, 0x00001000, 0x00000110,
    0x00000000, 0x00000010, 0x00000010, 0x00000110,     // 00c4h
    0x00011010, 0x01100010, 0x00000010, 0x00000010,
    0x00000000, 0x00010000, 0x00010000, 0x01111111,     // 00c5h
    0x00010000, 0x00010000, 0x00001000, 0x00000110,
    0x00000000, 0x00000000, 0x00111110, 0x00000000,     // 00c6h
    0x00000000, 0x00000000, 0x00000000, 0x01111111,
    0x00000000, 0x01111110, 0x01000000, 0x01000100,     // 00c7h
    0x00101000, 0x00010000, 0x00101000, 0x01000110,
    0x00000000, 0x00001000, 0x01111111, 0x00100000,     // 00c8h
    0x00010000, 0x00011100, 0x01101011, 0x00001000,
    0x00000000, 0x01000000, 0x01000000, 0x01000000,     // 00c9h
    0x00100000, 0x00100000, 0x00011000, 0x00000111,
    0x00000000, 0x00010010, 0x00100010, 0x00100010,     // 00cah
    0x01000010, 0x01000010, 0x01000001, 0x01000001,
    0x00000000, 0x00000001, 0x00000001, 0x01111111,     // 00cbh
    0x00000001, 0x00000001, 0x00000001, 0x01111110,
    0x00000000, 0x01111111, 0x01000000, 0x01000000,     // 00cch
    0x01000000, 0x00100000, 0x00010000, 0x00001110,
    0x00000000, 0x00000000, 0x00000100, 0x00001010,     // 00cdh
    0x00010001, 0x00100001, 0x01000000, 0x00000000,
    0x00000000, 0x00001000, 0x00001000, 0x01111111,     // 00ceh
    0x00001000, 0x00101010, 0x01001010, 0x01001001,
    0x00000000, 0x01111111, 0x01000000, 0x01000000,     // 00cfh
    0x00100010, 0x00010100, 0x00001000, 0x00010000,
    0x00000000, 0x00001110, 0x01110000, 0x00001110,     // 00d0h
    0x01110000, 0x00000110, 0x00011000, 0x01100000,
    0x00000000, 0x00001000, 0x00001000, 0x00000100,     // 00d1h
    0x00000100, 0x00100010, 0x01000010, 0x01111111,
    0x00000000, 0x01000000, 0x01000000, 0x00100100,     // 00d2h
    0x00101000, 0x00010000, 0x00101100, 0x01000011,
    0x00000000, 0x01111111, 0x00000100, 0x01111111,     // 00d3h
    0x00000100, 0x00000100, 0x00000100, 0x01111000,
    0x00000000, 0x00000010, 0x01111111, 0x01000010,     // 00d4h
    0x00100010, 0x00010100, 0x00000100, 0x00000100,
    0x00000000, 0x00000000, 0x00111110, 0x00100000,     // 00d5h
    0x00100000, 0x00100000, 0x00100000, 0x01111111,
    0x00000000, 0x01111110, 0x01000000, 0x01000000,     // 00d6h
    0x01111110, 0x01000000, 0x01000000, 0x01111110,
    0x00000000, 0x00111110, 0x00000000, 0x01111111,     // 00d7h
    0x01000000, 0x01000000, 0x00100000, 0x00011100,
    0x00000000, 0x01000010, 0x01000010, 0x01000010,     // 00d8h
    0x01000010, 0x01000000, 0x00100000, 0x00011000,
    0x00000000, 0x00001010, 0x00001010, 0x00001010,     // 00d9h
    0x01001010, 0x01001010, 0x00101010, 0x00011001,
    0x00000000, 0x00000010, 0x00000010, 0x01000010,     // 00dah
    0x01000010, 0x00100010, 0x00010010, 0x00001110,
    0x00000000, 0x01111111, 0x01000001, 0x01000001,     // 00dbh
    0x01000001, 0x01000001, 0x01000001, 0x01111111,
    0x00000000, 0x01111111, 0x01000001, 0x01000001,     // 00dch
    0x01000000, 0x01000000, 0x00100000, 0x00011100,
    0x00000000, 0x01000011, 0x01000100, 0x01000000,     // 00ddh
    0x01000000, 0x00100000, 0x00010000, 0x00001111,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 00deh
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,     // 00dfh
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00011110, 0x00001000, 0x00000100,     // 00e0h
    0x00101001, 0x01010001, 0x01010001, 0x00001100,
    0x00000000, 0x00000000, 0x00001100, 0x00010010,     // 00e1h
    0x00010010, 0x00100001, 0x01000000, 0x00000000,
    0x00000000, 0x01111101, 0x00100001, 0x01111101,     // 00e2h
    0x00100001, 0x00111001, 0x01100101, 0x00011001,
    0x00000000, 0x00111100, 0x00010000, 0x00111100,     // 00e3h
    0x00010000, 0x00011100, 0x00110010, 0x00001100,
    0x00000000, 0x00001110, 0x00101000, 0x00101000,     // 00e4h
    0x00111110, 0x01100101, 0x00100101, 0x00010010,
    0x00000000, 0x00000100, 0x00101111, 0x01000100,     // 00e5h
    0x00000110, 0x01000101, 0x01000101, 0x00111110,
    0x00000000, 0x00100010, 0x00100010, 0x00111110,     // 00e6h
    0x01010010, 0x01010101, 0x01001101, 0x00100110,
    0x00000000, 0x00000100, 0x00011111, 0x00000010,     // 00e7h
    0x00011111, 0x01000010, 0x01000010, 0x00111100,
    0x00000000, 0x00010010, 0x00111110, 0x01010011,     // 00e8h
    0x01000010, 0x00100100, 0x00000100, 0x00000100,
    0x00000000, 0x00001000, 0x00111101, 0x01001011,     // 00e9h
    0x01001001, 0x01001001, 0x00111000, 0x00000100,
    0x00000000, 0x00001000, 0x00111000, 0x00001000,     // 00eah
    0x00001000, 0x00011110, 0x00101001, 0x00000110,
    0x00000000, 0x00011000, 0x00100000, 0x00000100,     // 00ebh
    0x00111010, 0x01000110, 0x01000000, 0x00111000,
    0x00000000, 0x01000010, 0x01000010, 0x01000010,     // 00ech
    0x01000110, 0x01000000, 0x00100000, 0x00011000,
    0x00000000, 0x00111110, 0x00010000, 0x00111100,     // 00edh
    0x01000011, 0x01001100, 0x01010010, 0x00111100,
    0x00000000, 0x00100010, 0x00110011, 0x00101010,     // 00eeh
    0x00100110, 0x00100010, 0x00100011, 0x01000010,
    0x00000000, 0x00111110, 0x00010000, 0x00111100,     // 00efh
    0x01000011, 0x01000000, 0x01000010, 0x00111100,
    0x00000000, 0x00000010, 0x00111011, 0x01000110,     // 00f0h
    0x01000010, 0x01000011, 0x01000010, 0x00110010,
    0x00000000, 0x00000100, 0x00000100, 0x00000010,     // 00f1h
    0x01000110, 0x01000101, 0x01000101, 0x00111001,
    0x00000000, 0x01010100, 0x01111111, 0x00100100,     // 00f2h
    0x00100100, 0x00100100, 0x00100010, 0x00010001,
    0x00000000, 0x01010100, 0x01011111, 0x00000100,     // 00f3h
    0x00111111, 0x00001000, 0x00001000, 0x00001000,
    0x00000000, 0x01011110, 0x01100010, 0x00100010,     // 00f4h
    0x00100001, 0x00100000, 0x00010000, 0x00001100,
    0x00000000, 0x01010010, 0x01111110, 0x00100010,     // 00f5h
    0x00100001, 0x00100000, 0x00010000, 0x00001100,
    0x00000000, 0x01010000, 0x01111111, 0x00100000,     // 00f6h
    0x00100000, 0x00100000, 0x00100000, 0x00111111,
    0x00000000, 0x01010010, 0x01010010, 0x00111111,     // 00f7h
    0x00010010, 0x00010000, 0x00010000, 0x00001100,
    0x00000000, 0x01010011, 0x01010100, 0x00100011,     // 00f8h
    0x00100100, 0x00010000, 0x00001000, 0x00000111,
    0x00000000, 0x01010000, 0x01011111, 0x00010000,     // 00f9h
    0x00001000, 0x00001100, 0x00010010, 0x00100001,
    0x00000000, 0x01010010, 0x01111111, 0x00100010,     // 00fah
    0x00010010, 0x00000010, 0x00000010, 0x00111100,
    0x00000000, 0x01010001, 0x01010010, 0x00100000,     // 00fbh
    0x00100000, 0x00010000, 0x00001000, 0x00000110,
    0x00000000, 0x01011110, 0x01010010, 0x00100110,     // 00fch
    0x00111001, 0x00100000, 0x00010000, 0x00001100,
    0x00000000, 0x01010000, 0x01011110, 0x00010000,     // 00fdh
    0x01111111, 0x00010000, 0x00010000, 0x00001100,
    0x00000000, 0x00100101, 0x01001010, 0x00101010,     // 00feh
    0x00100000, 0x00010000, 0x00001000, 0x00000111,
    0x00000000, 0x01011110, 0x01010000, 0x00111111,     // 00ffh
    0x00001000, 0x00001000, 0x00001000, 0x00000110
};
