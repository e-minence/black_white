/*---------------------------------------------------------------------------*
  Project:     TwlSDK - include - mi
  File:        secureUncompression.h

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-06-20#$
  $Rev: 6675 $
  $Author: nishimoto_takashi $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_SECURE_UNCOMPRESSION_H__
#define NITRO_MI_SECURE_UNCOMPRESSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>

#define MI_ERR_SUCCESS              0
#define MI_ERR_UNSUPPORTED          -1
#define MI_ERR_SRC_SHORTAGE         -2
#define MI_ERR_SRC_REMAINDER        -3
#define MI_ERR_DEST_OVERRUN         -4
#define MI_ERR_ILLEGAL_TABLE        -5

//======================================================================
//          ���k�f�[�^�W�J
//======================================================================

/*---------------------------------------------------------------------------*
  Name:         MI_SecureUncompressAny

  Description:  �f�[�^�w�b�_���爳�k�^�C�v�𔻕ʂ��āA
                �K���ȓW�J���������s���܂��B
                ���ׂĂ̈��k��ނ̓W�J�����������N����܂��̂ŁA
                ����̈��k�t�H�[�}�b�g�ȊO�g�p���Ă��Ȃ��ꍇ�ɂ�
                ���k��ʖ��̊֐������s���������悢��������܂���B

  Arguments:    srcp    �\�[�X�A�h���X
                srcSize �\�[�X�f�[�^�T�C�Y
                destp   �f�X�e�B�l�[�V�����A�h���X
                dstSize �f�X�e�B�l�[�V�����T�C�Y

  Returns:      �ϊ��ɐ��������ꍇ�� 0
                ���s�����ꍇ�ɂ͕��̃G���[�R�[�h
 *---------------------------------------------------------------------------*/
s32 MI_SecureUncompressAny( const void* srcp, u32 srcSize, void* destp, u32 dstSize );


/*---------------------------------------------------------------------------*
  Name:         MI_SecureUncompressRL

  Description:  ���������O�X���k�f�[�^�W�������W�J

  �E���������O�X���k�f�[�^��W�J���A8bit�P�ʂŏ������݂܂��B
  �E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B

  �E�f�[�^�w�b�_
      u32 :4                  �\��
          compType:4          ���k�^�C�v�i = 3�j
          destSize:24         �W�J��̃f�[�^�T�C�Y
  
  �E�t���O�f�[�^�t�H�[�}�b�g
      u8  length:7            �W�J�f�[�^�� - 1�i�����k���j
                              �W�J�f�[�^�� - 3�i�A����3Byte�ȏ㎞�݈̂��k�j
          flag:1              �i0, 1�j = �i�����k�f�[�^, ���k�f�[�^�j
  
  Arguments:    *srcp   �\�[�X�A�h���X
                srcSize �\�[�X�f�[�^�T�C�Y
                *destp  �f�X�e�B�l�[�V�����A�h���X
                dstSize �f�X�e�B�l�[�V�����T�C�Y

  Returns:      �ϊ��ɐ��������ꍇ�� 0
                ���s�����ꍇ�ɂ͕��̃G���[�R�[�h
 *---------------------------------------------------------------------------*/
s32 MI_SecureUncompressRL( const void *srcp, u32 srcSize, void *destp, u32 dstSize );


/*---------------------------------------------------------------------------*
  Name:         MI_SecureUncompressLZ
  
  Description:  �k�y�V�V���k�f�[�^�W�������W�J
  
  �ELZ77���k�f�[�^��W�J���A8bit�P�ʂŏ������݂܂��B
  �E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
  
  �E�f�[�^�w�b�_
      u32 :4                  �\��
          compType:4          ���k�^�C�v�i = 1�j
          destSize:24         �W�J��̃f�[�^�T�C�Y
  
  �E�t���O�f�[�^�t�H�[�}�b�g
      u8  flags               ���k�^�����k�t���O
                              �i0, 1�j = �i�����k�f�[�^, ���k�f�[�^�j
  �E�R�[�h�f�[�^�t�H�[�}�b�g�iBig Endian�j
      u16 length:4            �W�J�f�[�^�� - 3�i��v��3Byte�ȏ㎞�݈̂��k�j
          offset:12           ��v�f�[�^�I�t�Z�b�g - 1
  
  Arguments:    *srcp   �\�[�X�A�h���X
                srcSize �\�[�X�f�[�^�T�C�Y
                *destp  �f�X�e�B�l�[�V�����A�h���X
                dstSize �f�X�e�B�l�[�V�����T�C�Y
  
  Returns:      �ϊ��ɐ��������ꍇ�� 0
                ���s�����ꍇ�ɂ͕��̃G���[�R�[�h
 *---------------------------------------------------------------------------*/
s32 MI_SecureUncompressLZ( const void *srcp, u32 srcSize, void *destp, u32 dstSize );


/*---------------------------------------------------------------------------*
  Name:         MI_SecureUncompressHuffman
  
  Description:  �n�t�}�����k�f�[�^�W�J
  
  �E�n�t�}�����k�f�[�^��W�J���A32bit�P�ʂŏ������݂܂��B
  �E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B
  �E�f�X�e�B�l�[�V�����A�h���X��4Byte���E�ɍ��킹�Ă��������B
  �E�W�J��̃o�b�t�@�T�C�Y��4Byte�̔{���������p�ӂ��Ă�������
  
  �E�f�[�^�w�b�_
      u32 bitSize:4           �P�f�[�^�E�r�b�g�T�C�Y�i�ʏ� 4|8�j
          compType:4          ���k�^�C�v�i = 2�j
          destSize:24         �W�J��̃f�[�^�T�C�Y
  
  �E�c���[�e�[�u��
      u8           treeSize        �c���[�e�[�u���T�C�Y/2 - 1
      TreeNodeData nodeRoot        ���[�g�m�[�h
  
      TreeNodeData nodeLeft        ���[�g���m�[�h
      TreeNodeData nodeRight       ���[�g�E�m�[�h
  
      TreeNodeData nodeLeftLeft    �����m�[�h
      TreeNodeData nodeLeftRight   ���E�m�[�h
  
      TreeNodeData nodeRightLeft   �E���m�[�h
      TreeNodeData nodeRightRight  �E�E�m�[�h
  
              �E
              �E
  
    ���̌�Ɉ��k�f�[�^�{��
  
  �ETreeNodeData�\����
      u8  nodeNextOffset:6    ���m�[�h�f�[�^�ւ̃I�t�Z�b�g - 1�i2Byte�P�ʁj
          rightEndFlag:1      �E�m�[�h�I���t���O
          leftEndzflag:1      ���m�[�h�I���t���O
                              �I���t���O���Z�b�g����Ă���ꍇ
                              ���m�[�h�Ƀf�[�^������
  
  Arguments:    *srcp   �\�[�X�A�h���X
                srcSize �\�[�X�f�[�^�T�C�Y
                *destp  �f�X�e�B�l�[�V�����A�h���X
                dstSize �f�X�e�B�l�[�V�����T�C�Y

  Returns:      �ϊ��ɐ��������ꍇ�� 0
                ���s�����ꍇ�ɂ͕��̃G���[�R�[�h
 *---------------------------------------------------------------------------*/
s32 MI_SecureUncompressHuffman( const void *srcp, u32 srcSize, void *destp, u32 dstSize );

/*---------------------------------------------------------------------------*
  Name:         MI_SecureUnfilterDiff

  Description:  �����t�B���^�ϊ��̕��� �W�������W�J
  
    �E�����t�B���^�𕜌����A8bit�P�ʂŏ������݂܂��B
    �EVRAM�ɒ��ړW�J���邱�Ƃ͂ł��܂���B
    �E���k�f�[�^�̃T�C�Y��4�̔{���ɂȂ�Ȃ������ꍇ��
      �o���邾��0�ŋl�߂Ē������ĉ������B
    �E�\�[�X�A�h���X��4Byte���E�ɍ��킹�ĉ������B

  Arguments:    *srcp   �\�[�X�A�h���X
                srcSize �\�[�X�T�C�Y
                *destp  �f�X�e�B�l�[�V�����A�h���X
                dstSize �f�X�e�B�l�[�V�����T�C�Y

  Returns:      �ϊ��ɐ��������ꍇ�� 0
                ���s�����ꍇ�ɂ͕��̃G���[�R�[�h
 *---------------------------------------------------------------------------*/
s32 MI_SecureUnfilterDiff( register const void *srcp, u32 srcSize, register void *destp, u32 dstSize );

/*---------------------------------------------------------------------------*
  Name:         MI_SecureUncompressBLZ

  Description:  Uncompress BLZ ( backword LZ )

  Arguments:
      void *srcp              source address ( data buffer compressed by compBLZ.exe )
      u32   srcSize           source data size ( data size of compressed data )
      u32   dstSize           data size after uncompressing
      
  Returns:      None.
 *---------------------------------------------------------------------------*/
s32 MI_SecureUncompressBLZ(const void *srcp, u32 srcSize, u32 dstSize);

//================================================================================

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_MI_SECURE_UNCOMPRESSION_H__ */
#endif
