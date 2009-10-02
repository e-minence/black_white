/*---------------------------------------------------------------------------*
  Project:  TwlSDK - IRC - include
  File:     irc_interrnal.h

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
#ifndef NITRO_IRC_IRC_INTERNAL_H_
#define NITRO_IRC_IRC_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/* constants */

#define IRC_SUM_VALUE       (0x80)  // �`�F�b�N�T�������l

/*---------------------------------------------------------------------------*/
/* declarations */

typedef enum
{
    IRC_ST_DISCONNECT,
    IRC_ST_WAIT_RESPONSE,   // �R�l�N�g�R�}���h�𔭍s���ă��X�|���X��҂��Ă�����(���Z���_�[)
    IRC_ST_WAIT_ACK,        // ���X�|���X�𔭍s����ACK��҂��Ă�����(�����V�[�o�[)
    IRC_ST_SENDER,
    IRC_ST_RECEIVER,
    IRC_ST_NUM

} IRC_STATUS;

typedef struct
{
    u8    command;    // �R�}���h
    u8    unitId;     // UNIT_ID
    u8    sumL;       // �`�F�b�N�T�����
    u8    sumH;       // �`�F�b�N�T������
    //u32 uniqueId;

} IRCHeader;

typedef struct
{
    IRC_STATUS status;
    BOOL flg_connect;

} IRCStatus;


// �ʐM�R�}���h���X�g(�V�X�e���R�}���h�̂�)
typedef enum
{
    // �V�X�e���R�}���h
    IR_RESET    = (0xF2),
    IR_SHUTDOWN = (0xF4),
    IR_RETRY    = (0xF6),

    IR_ACK      = (0xF8),   // ACK
    IR_RESPONSE = (0xFA),   // ���X�|���X
    IR_CONNECT  = (0xFC),   // �R�l�N�g
    IR_NUM

} IRCommand;



void IRCi_Init(void);
void IRCi_End(void);

/*---------------------------------------------------------------------------*
  Name:         IRCi_Read

  Description:  IRC ����f�[�^�ǂݍ��݂��s���֐�

  Arguments:    buf  : ��M�o�b�t�@

  Returns:      ��M�T�C�Y
 *---------------------------------------------------------------------------*/
u8 IRCi_Read(u8 *buf);

/*---------------------------------------------------------------------------*
  Name:         IRCi_Write

  Description:  IRC �ɂ��̂܂܃f�[�^���M���s���֐�(�w�b�_��IRC ���C���ō��)

  Arguments:    data  : �������ރf�[�^�̃A�h���X
                size  : �������ރf�[�^�̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRCi_Write(u8 *data, u8 size);

/*---------------------------------------------------------------------------*
  Name:         IRCi_Check

  Description:  IRC �J�[�h�`�F�b�N���s���֐�

  Arguments:    None.

  Returns:      �V�^ IRC �Ȃ� 0xAA ��Ԃ�
 *---------------------------------------------------------------------------*/
u8 IRCi_Check(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NITRO_IRC_IRC_INTERNAL_H_
