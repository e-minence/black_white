/*---------------------------------------------------------------------------*
  Project:  TwlSDK - IRC - include
  File:     irc.h

  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-17#$
  $Rev: 10767 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_IRC_IRC_H_
#define NITRO_IRC_IRC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/irc/irc_internal.h>


#define IRC_DEBUG   (1) // ��f�o�b�O���͒�`�������Ă����Ă�������


#ifdef IRC_DEBUG
#define PRINTF OS_Printf
#else
#define PRINTF TRUE ? (void)0 : OS_Printf
#endif

/*---------------------------------------------------------------------------*/
/* constants */

#define IRC_TRNS_MAX (180)   // ��x�ɑ����]���o�C�g��(���J�[�h�̏ꍇ�� 128 �ɕύX����K�v������)

/*---------------------------------------------------------------------------*/
/* declarations */

// �G���[�R�[�h
typedef enum
{
    IRC_ERROR_NONE,             // �G���[�Ȃ�
    //IRC_ERROR_NOTCONNECT,     // ��ڑ���
    IRC_ERROR_TIMEOUT,          // �^�C���A�E�g�G���[
    //IRC_ERROR_CHECKSUM,       // �`�F�b�N�T���G���[->���g���C�����ă^�C���A�E�g�����ꍇ�̂݃G���[
    IRC_ERROR_NUM

} IRCError;


// �R�[���o�b�N�֐��̌^��`
typedef void (*IRCConnectCallback)(u8 id, BOOL isSender);
typedef void (*IRCRecvCallback)(u8 *data, u8 size, u8 command, u8 unitId);
typedef void (*IRCShutdownCallback)(BOOL isError);

/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         IRC_Init

  Description:  ���C�u�����̏������ƃ��j�b�g�ԍ���ݒ肷�邽�߂̊֐�

  Arguments:    unitId : �ʐM�̎��ʂɎg����@�� ID
                         �C�V�����甭�s���ꂽ�@�� ID ���w�肵�Ă�������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_Init(u8 unitId);

/*---------------------------------------------------------------------------*
  Name:         IRC_Move

  Description:  ���t���[���Ăԃv���Z�X�֐�

  Arguments:    None.

  Returns:      IRCError �^�̃G���[�ԍ���Ԃ��܂�
 *---------------------------------------------------------------------------*/
IRCError IRC_Move(void);

/*---------------------------------------------------------------------------*
  Name:         IRC_Send

  Description:  �f�[�^�𑗐M���邽�߂̊֐�

  Arguments:    buf     : ���M�������f�[�^�̐擪�A�h���X
                          �R�}���h�̂ݑ��肽���ꍇ�� NULL ���w�肵�Ă�������
                size    : ���M����f�[�^�̃o�C�g��
                command : �ʐM���e�����ʂ��邽�߂̃R�}���h�ԍ�
                          �����̂ݎg�p�\�ł����A���e�̓A�v���P�[�V�����Ŏ��R�ɐݒ肵�Ă�������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_Send(const u8 *buf, u8 size, u8 command);

/*---------------------------------------------------------------------------*
  Name:         IRC_SetConnectCallback

  Description:  �ڑ��R�[���o�b�N��ݒ肷�邽�߂̊֐�

  Arguments:    callback : �ڑ����ɌĂяo�����R�[���o�b�N

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_SetConnectCallback(IRCConnectCallback callback);

/*---------------------------------------------------------------------------*
  Name:         IRC_SetRecvCallback

  Description:  �f�[�^��M�R�[���o�b�N��ݒ肷�邽�߂̊֐�

  Arguments:    callback : �f�[�^��M���ɌĂяo�����R�[���o�b�N

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_SetRecvCallback(IRCRecvCallback callback);

/*---------------------------------------------------------------------------*
  Name:         IRC_SetShutdownCallback

  Description:  �ؒf�R�[���o�b�N��ݒ肷�邽�߂̊֐�

  Arguments:    callback : �ؒf���ɌĂяo�����R�[���o�b�N

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_SetShutdownCallback(IRCShutdownCallback callback);

/*---------------------------------------------------------------------------*
  Name:         IRC_Connect

  Description:  �ڑ��v�����邽�߂̊֐�
                �{�֐����Ă񂾕�(���̗v�����󂯂����Ƌt)���Z���_�ƂȂ�܂�

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_Connect(void);

/*---------------------------------------------------------------------------*
  Name:         IRC_IsConnect

  Description:  �ʐM�����N������Ă��邩�ǂ����m�F���邽�߂̊֐�

  Arguments:    None.

  Returns:      �R�l�N�V����������A�ʐM�\�ł���� TRUE ��
                �����łȂ���� FALSE ��Ԃ��܂�
 *---------------------------------------------------------------------------*/
BOOL IRC_IsConnect(void);

/*---------------------------------------------------------------------------*
  Name:         IRC_IsSender

  Description:  �ʐM�J�n���s�����[��(�Z���_��)���ǂ������ׂ邽�߂̊֐�
                (�ڑ����̂ݗL��)

  Arguments:    None.

  Returns:      ��ɐڑ��v�����o�������ł���� TRUE ��
                �����łȂ���� FALSE ��Ԃ��܂�
 *---------------------------------------------------------------------------*/
BOOL IRC_IsSender(void);

/*---------------------------------------------------------------------------*
  Name:         IRC_Shutdown

  Description:  �ʐM�������I�����邽�߂̊֐�

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_Shutdown(void);

/*---------------------------------------------------------------------------*
  Name:         IRC_Check

  Description:  �V�^ IRC �J�[�h���𔻕ʂ��邽�߂̊֐�

  Arguments:    None.

  Returns:      �V�^ IRC �J�[�h�Ȃ� TRUE ��
                �����łȂ���� FALSE ��Ԃ��܂�
 *---------------------------------------------------------------------------*/
BOOL IRC_Check(void);


// ����ɂ͕K�{�łȂ��֐�
/*---------------------------------------------------------------------------*
  Name:         IRC_SetUnitNumber

  Description:  ���j�b�g�ԍ���ݒ肷�邽�߂̊֐�

  Arguments:    unitId : �ʐM�̎��ʂɎg����@�� ID
                         �C�V�����甭�s���ꂽ�@�� ID ���w�肵�Ă�������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_SetUnitNumber(u8 unitId);

/*---------------------------------------------------------------------------*
  Name:         IRC_GetUnitNumber

  Description:  �ʐM����̃��j�b�g�ԍ����m�F���邽�߂̊֐�

  Arguments:    None.

  Returns:      �ʐM����̃��j�b�g�ԍ���Ԃ��܂�(�ؒf���� 0xFF)
 *---------------------------------------------------------------------------*/
u8 IRC_GetUnitNumber(void);

/*---------------------------------------------------------------------------*
  Name:         IRC_GetRetryCount

  Description:  �g�[�^���̃��g���C�񐔂��擾���邽�߂̊֐�(�f�o�b�O�p)

  Arguments:    None.

  Returns:      ���g���C�񐔂�Ԃ��܂�
 *---------------------------------------------------------------------------*/
u32 IRC_GetRetryCount(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NITRO_IRC_IRC_H_
