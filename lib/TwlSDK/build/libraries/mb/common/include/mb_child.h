/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - include
  File:     mb_child.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
/* ==========================================================================

    MB���C�u�����̎q�@�p�w�b�_�ł��B
    �}���`�u�[�g�q�@�����IPL�ɂē����C�u�������g�p����ꍇ�́A
    nitro/mb.h�ɉ����A���̃w�b�_���C���N���[�h���Ă��������B

   ==========================================================================*/


#ifndef _MB_CHILD_H_
#define _MB_CHILD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/mb.h>
#include "mb_fileinfo.h"
#include "mb_gameinfo.h"

/* ---------------------------------------------------------------------

        define�萔

   ---------------------------------------------------------------------*/

#define MB_CHILD_SYSTEM_BUF_SIZE        (0x6000)

#define MB_MAX_SEND_BUFFER_SIZE         (0x400)
#define MB_MAX_RECV_BUFFER_SIZE         (0x80)

/* ----------------------------------------------------------------------------

    �}���`�u�[�gRAM�A�h���X��`(�����g�p)

   ----------------------------------------------------------------------------*/

/* 
    �}���`�u�[�g�v���O�����z�u�\�͈�

    �����}���`�u�[�g�ɂ�����ARM9�R�[�h�̏������[�h�ő�T�C�Y��2.5MB�ł��B
    
    ARM9�R�[�h�̃��[�h�\�̈��
    MB_LOAD_AREA_LO �` MB_LOAD_AREA_HI
    �͈̔͂ł��B
 */
#define MB_LOAD_AREA_LO                         ( HW_MAIN_MEM )
#define MB_LOAD_AREA_HI                         ( HW_MAIN_MEM + 0x002c0000 )
#define MB_LOAD_MAX_SIZE                        ( MB_LOAD_AREA_HI - MB_LOAD_AREA_LO )


/*
    �}���`�u�[�g����ARM7 static�̎q�@���ł̈ꎞ��M�o�b�t�@�A�h���X��`
    
    ARM7�R�[�h�̃��[�h�\�̈��

    a) 0x02000000 �` 0x02300000
       (MB_LOAD_AREA_LO �` MB_ARM7_STATIC_RECV_BUFFER_END)
    b) 0x02300000 �` 0x023fe000
       (MB_ARM7_STATIC_RECV_BUFFER_END �` 0x023fe000)
    c) 0x037f8000 �` 0x0380f000
       (����WRAM)

    �̂����ꂩ�ɂȂ�܂��B
    
    ������
    ARM7�R�[�h�����WRAM�������� 0x02300000 �ȍ~�Ƀ��[�h����ꍇ�A

    0x022c0000 �` 0x02300000
    (MB_LOAD_AREA_HI �` MB_ARM7_STATIC_RECV_BUFFER_END)
    �Ƀo�b�t�@��݂��A���̃o�b�t�@�Ɏ�M�������̂���
    �u�[�g���Ɏw��A�h���X�ɍĔz�u����悤�ɂ��Ă��܂��B
    
    �E0x02300000 �̃A�h���X���܂����z�u��ARM7�R�[�h
    �E0x02300000 �ȍ~�̃A�h���X�ŁA�T�C�Y�� 0x40000 ( MB_ARM7_STATIC_RECV_BUFFER_SIZE ) 
    �@�𒴂���ARM7�R�[�h�͐��퓮���ۏ؂ł��܂���̂ŁA
    �@���̂悤�ȃR�[�h�̍쐬���֎~�������܂��B
 */

#define MB_ARM7_STATIC_RECV_BUFFER              MB_LOAD_AREA_HI
#define MB_ARM7_STATIC_RECV_BUFFER_END          ( HW_MAIN_MEM + 0x00300000 )
#define MB_ARM7_STATIC_RECV_BUFFER_SIZE         ( MB_ARM7_STATIC_RECV_BUFFER_END - MB_LOAD_AREA_HI )
#define MB_ARM7_STATIC_LOAD_AREA_HI             ( MB_LOAD_AREA_HI + MB_ARM7_STATIC_RECV_BUFFER_SIZE )
#define MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE       ( 0x18000 - 0x1000 )    // 0x18000 = HW_WRAM_SIZE + HW_PRV_WRAM_SIZE

/* BssDesc�̈ꎞ�i�[�ꏊ */
#define MB_BSSDESC_ADDRESS                      ( HW_MAIN_MEM + 0x003fe800 )
/* MBDownloadFileInfo�̈ꎞ�i�[�ꏊ */
#define MB_DOWNLOAD_FILEINFO_ADDRESS            ( MB_BSSDESC_ADDRESS + ( ( ( MB_BSSDESC_SIZE ) + (32) - 1 ) & ~((32) - 1) ) )
/* �}���`�u�[�g���N���������_�ō������Ă���ROM�J�[�h��ROM�w�b�_�ޔ��ꏊ */
#define MB_CARD_ROM_HEADER_ADDRESS              ( MB_DOWNLOAD_FILEINFO_ADDRESS + ( ( ( MB_DOWNLOAD_FILEINFO_SIZE ) + (32) - 1 ) & ~((32) - 1) ) )
/* ROM Header�̊i�[�ꏊ */
#define MB_ROM_HEADER_ADDRESS                   (HW_MAIN_MEM + 0x007ffe00)


/* ---------------------------------------------------------------------

        �}���`�u�[�g���C�u����(MB)�q�@�pAPI. IPL �Ŏg�p����܂�

   ---------------------------------------------------------------------*/

/* �q�@�p�����[�^�ݒ� & �X�^�[�g */
int     MB_StartChild(void);

/* MB�q�@�Ŏg�p���郏�[�N�������̃T�C�Y���擾 */
int     MB_GetChildSystemBufSize(void);

/* �q�@��Ԓʒm�R�[���o�b�N�̐ݒ� */
void    MB_CommSetChildStateCallback(MBCommCStateCallbackFunc callback);

/* �q�@��Ԃ̎擾 */
int     MB_CommGetChildState(void);

/* �_�E�����[�h�J�n */
BOOL    MB_CommStartDownload(void);

/* �q�@�̃_�E�����[�h�i���󋵂� 0 �` 100 �Ŏ擾 */
u16     MB_GetChildProgressPercentage(void);

/* �e�@�ւ̐ڑ����݁��_�E�����[�h�t�@�C���v�� */
int     MB_CommDownloadRequest(int index);      // MbBeaconRecvStatus�̃��X�gNo.�Őڑ��v������e�@���w��B

#ifdef __cplusplus
}
#endif

#endif /*  _MB_CHILD_H_    */
