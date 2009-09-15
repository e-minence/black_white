/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - demos - UnitTours/2D_CharBg_256_16
  File:     data.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-20#$
  $Rev: 9005 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

/*
 ******************************************************************************
 *
 *  Project    :
 *               nitroSDK�T���v���v���O����
 *  Title      :
 *               �A�t�B���g���E256�F�~16�g���p���b�g�L�����N�^�a�f��\��
 *  Explanation:
 *�@�@�@�@�@�@�@ 256�F�~16�̊g���p���b�g�̂a�f���A�A�t�B���ϊ��i��]�g��k���j
 *             ���s���ĕ\������B
 *
 ******************************************************************************
 */
#ifndef BG_DATA_H_
#define BG_DATA_H_


/*-------------------------- Data  -------------------------------*/
extern const unsigned char kakucho_8bit_isc[128 * 16];
extern const unsigned char kakucho_8bit_icl[512 * 16];
extern const unsigned char kakucho_8bit_icg[2086 * 16 + 4];

// #define SCREEN_SIZE  ( 128 * 16)
#define SCREEN_SIZE	( 128 * 16)
#define PALETTE_SIZE	( 512 * 16)
#define CHAR_SIZE	(2086 * 16)


#endif
