/*---------------------------------------------------------------------------*
  Project:  TwlSDK - IRC - libraries
  File:     irc.c

  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-04-20 #$
  $Rev: 10429 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitro/irc.h>

/*---------------------------------------------------------------------------*/
/* constants */

#define CSPI_CONTINUOUS_ON  0x0040
#define CSPI_CONTINUOUS_OFF 0x0000

//#define COMMAND_IRC       (0x00033)   // IRC ���ʎq
#define COMMAND_READ        (0x0001)    // IRC ���ʎq�����˂�
#define COMMAND_WRITE       (0x0002)    // IRC ���ʎq�����˂�
#define COMMAND_REGW8       (0x0003)    // IRC ���ʎq�����˂�
#define COMMAND_REGR8       (0x0004)    // IRC ���ʎq�����˂�
#define COMMAND_REGW16      (0x0005)    // IRC ���ʎq�����˂�
#define COMMAND_REGR16      (0x0006)    // IRC ���ʎq�����˂�
#define COMMAND_STATUS      (0x0007)    // 

#define MCCNT0_SPI_CLK_MASK 0x0003  /* �{�[���[�g�ݒ�}�X�N */
#define MCCNT0_SPI_CLK_4M   0x0000  /* �{�[���[�g 4.19MHz */
#define MCCNT0_SPI_CLK_2M   0x0001  /* �{�[���[�g 2.10MHz */
#define MCCNT0_SPI_CLK_1M   0x0002  /* �{�[���[�g 1.05MHz */
#define MCCNT0_SPI_CLK_524K 0x0003  /* �{�[���[�g 524kHz */
#define MCCNT0_SPI_BUSY     0x0080  /* SPI �r�W�[�t���O */
#define MMCNT0_SEL_MASK     0x2000  /* CARD ROM / SPI �I���}�X�N */
#define MMCNT0_SEL_CARD     0x0000  /* CARD ROM �I�� */
#define MMCNT0_SEL_SPI      0x2000  /* CARD SPI �I�� */
#define MCCNT0_INT_MASK     0x4000  /* �]���������荞�݃}�X�N */
#define MCCNT0_INT_ON       0x4000  /* �]���������荞�ݗL�� */
#define MCCNT0_INT_OFF      0x0000  /* �]���������荞�ݖ��� */
#define MCCNT0_MASTER_MASK  0x8000  /* CARD �}�X�^�[�}�X�N */
#define MCCNT0_MASTER_ON    0x8000  /* CARD �}�X�^�[ ON */
#define MCCNT0_MASTER_OFF   0x0000  /* CARD �}�X�^�[ OFF */

#define MCCNT0_SPI_CLK_SLOW MCCNT0_SPI_CLK_1M
//#define MCCNT0_SPI_CLK_SLOW       MCCNT0_SPI_CLK_524K

#define COM_DATA_SIZE   (180-4)                              // ���[�U����M�o�b�t�@�T�C�Y
#define COM_TRNS_SIZE   (1+sizeof(IRCHeader)+COM_DATA_SIZE)  // �V�X�e������M�o�b�t�@�T�C�Y

#define IRC_READ_WAIT       50
#define IRC_WRITE_WAIT      50
#define IRC_COMMAND_WAIT    60

/*---------------------------------------------------------------------------*/
/* declarations */

// SPI �����Ǘ��p�����[�^
typedef struct
{
    u32  addr_width; /* �A�h���X�\���o�C�g�� */
    u32  src;        /* �]���� */
    u32  dst;        /* �]���� */
    BOOL cmp;        /* ��r���� */

} IRCParam;


static void IRCi_WaitBusy(void);
static   u8 IRCi_CommWrite(IRCParam *p);
static void IRCi_CommRead(IRCParam *p);
static void IRCi_EnableSpi(u32 cont);


/*---------------------------------------------------------------------------*/
/* variables */

extern u16         irc_lock_id = (u16)OS_LOCK_ID_ERROR;
static IRCParam    irc_param;

static u8 sRecvBuf[COM_TRNS_SIZE] ATTRIBUTE_ALIGN(4);
static u8 last_size;                                    // ���g���C�p
static u8 backup_size;                                  // ���g���C�p


/*---------------------------------------------------------------------------*/
/* function */


/*---------------------------------------------------------------------------*
  Name:         IRCi_Check

  Description:  IRC �J�[�h�`�F�b�N���s���֐�

  Arguments:    None.

  Returns:      �V�^ IRC �Ȃ� 0xAA ��Ԃ�
 *---------------------------------------------------------------------------*/
u8 IRCi_Check_Another(void)
{
    u8 buf[2];
    irc_param.src = (u32)sRecvBuf;
    irc_param.dst = (u32)(buf);
    sRecvBuf[0] = 0x08; 

    CARD_LockBackup(irc_lock_id);
    (void)OS_LockCard(irc_lock_id);
    {
        OSTick time = OS_GetTick();
        while (OS_TicksToMicroSeconds(OS_GetTick() - time) < IRC_COMMAND_WAIT)
        {
            // �R�}���h�͒����҂K�v������
        }
    }
    IRCi_WaitBusy();
    {
        IRCi_EnableSpi(CSPI_CONTINUOUS_ON | MCCNT0_SPI_CLK_SLOW);
        buf[0] = IRCi_CommWrite(&irc_param);
        {
            OSTick time = OS_GetTick();
            while (OS_TicksToMicroSeconds(OS_GetTick() - time) < IRC_COMMAND_WAIT)
            {
                // �R�}���h�͒����҂K�v������
            }
        }
        IRCi_EnableSpi(CSPI_CONTINUOUS_OFF | MCCNT0_SPI_CLK_SLOW);
        IRCi_CommRead(&irc_param); // 0xAA ���Ԃ��Ă���
        //OS_TPrintf("second 0x%X\n", buf[0]);
        //OS_TPrintf("third 0x%X\n", buf[1]);
    }
    IRCi_WaitBusy();
    (void)OS_UnLockCard(irc_lock_id);
    CARD_UnlockBackup(irc_lock_id);

    return buf[0];
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_WaitBusy

  Description:  CARD-SPI �̊�����҂֐�

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void IRCi_WaitBusy(void)
{
    while ((reg_MI_MCCNT0 & MCCNT0_SPI_BUSY) != 0)
    {
    }
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_EnableSpi

  Description:  CARD-SPI ��L���ɂ���֐�

  Arguments:    cont  : �A���]���t���O. (CSPI_CONTINUOUS_ON or OFF)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void  IRCi_EnableSpi(u32 cont)
{
    const u16 ctrl = (u16)(MCCNT0_MASTER_ON | MCCNT0_INT_OFF | MMCNT0_SEL_SPI | cont);
    reg_MI_MCCNT0 = ctrl;
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_CommWrite

  Description:  IRC �� 1 �o�C�g�������݂��s���֐�

  Arguments:    p  : �������݂��s�����߂̏��

  Returns:      �������݂��s������̃��W�X�^�̒l
 *---------------------------------------------------------------------------*/
static u8 IRCi_CommWrite(IRCParam *p)
{

    vu16   tmp;
    OSTick time = OS_GetTick();

    while (OS_TicksToMicroSeconds(OS_GetTick() - time) < IRC_WRITE_WAIT)
    {
    }

    reg_MI_MCD0 = (u16)MI_ReadByte((void*)p->src);

    ++p->src;
    IRCi_WaitBusy();
    tmp = reg_MI_MCD0;
    
    return ((u8)tmp);
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_CommRead

  Description:  IRC ���� 1 �o�C�g�ǂݍ��݂��s���֐�

  Arguments:    p  : �ǂݍ��݂��s�����߂̏��

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void IRCi_CommRead(IRCParam *p)
{
    OSTick time = OS_GetTick();
    while (OS_TicksToMicroSeconds(OS_GetTick() - time) < IRC_READ_WAIT)
    {
    }

    reg_MI_MCD0 = 0;
    IRCi_WaitBusy();
    // �����ňÍ������s����IRC�Ƃ̒ʐM�����
    MI_WriteByte((void*)p->dst, (u8)reg_MI_MCD0);

    ++p->dst;
}


//=============================================================================================
/**
 * @brief ���V�^IRC�J�[�h���𔻕ʂ��邽�߂̊֐�
 *
 * @param   none 
 *
 * @retval  BOOL �V�^ IRC �J�[�h�Ȃ� TRUE ��
 *               �����łȂ���� FALSE ��Ԃ��܂�
 */
//=============================================================================================
BOOL IRC_Check_Another(void)
{
    u8 ret = IRCi_Check_Another();
    if (IRCi_Check_Another() == 0xAA)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
