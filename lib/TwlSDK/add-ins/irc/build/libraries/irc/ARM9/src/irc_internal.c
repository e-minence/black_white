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

static void IRCi_WaitCommand(void);
static u8   IRCi_CommWrite(IRCParam *p);
static void IRCi_CommRead(IRCParam *p);

/*---------------------------------------------------------------------------*/
/* variables */

extern u16         irc_lock_id = (u16)OS_LOCK_ID_ERROR;
static IRCParam    irc_param;

static u8 sSendBuf[COM_TRNS_SIZE] ATTRIBUTE_ALIGN(4);   // ���g���C�p�Ɏ�M�ƕ�����
static u8 sRecvBuf[COM_TRNS_SIZE] ATTRIBUTE_ALIGN(4);
static u8 sBackBuf[COM_TRNS_SIZE+1] ATTRIBUTE_ALIGN(4); // ���g���C�p 1byte ��IRC ����R�}���h��
static u8 last_size;                                    // ���g���C�p
static u8 backup_size;                                  // ���g���C�p


/*---------------------------------------------------------------------------*/
/* function */

static void IRCi_WaitCommand(void)
{
    OSTick time = OS_GetTick();
    while (OS_TicksToMicroSeconds(OS_GetTick() - time) < IRC_COMMAND_WAIT)
    {
        // �R�}���h�͒����҂K�v������
    }
}

void IRCi_Init(void)
{
    irc_param.addr_width = 1;        // 4KEEPROM�Ɠ���
    if (irc_lock_id == (u16)OS_LOCK_ID_ERROR)
    {
        irc_lock_id = (u16)OS_GetLockID();
        //OS_TPrintf("LOCK SUCCESS %d(0x%X)\n", lock_id, lock_id);
    }
    else
    {
        //OS_TPrintf("LOCK ERROR %d(0x%X)\n", lock_id, lock_id);
    }
}

void IRCi_End(void)
{
    if (irc_lock_id != OS_LOCK_ID_ERROR)
    {
        OS_ReleaseLockID(irc_lock_id);
        irc_lock_id = (u16)OS_LOCK_ID_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_EnableSpi

  Description:  CARD-SPI ��L���ɂ���֐�

  Arguments:    cont  : �A���]���t���O. (CSPI_CONTINUOUS_ON or OFF)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void  IRCi_EnableSpi(u32 cont)
{
    const u16 ctrl = (u16)(MCCNT0_MASTER_ON | MCCNT0_INT_OFF | MMCNT0_SEL_SPI | cont);
    reg_MI_MCCNT0 = ctrl;
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_WaitBusy

  Description:  CARD-SPI �̊�����҂֐�

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void IRCi_WaitBusy(void)
{
    while ((reg_MI_MCCNT0 & MCCNT0_SPI_BUSY) != 0)
    {
    }
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

/*---------------------------------------------------------------------------*
  Name:         IRCi_Read

  Description:  IRC ����f�[�^�ǂݍ��݂��s���֐�

  Arguments:    buf  : ��M�o�b�t�@

  Returns:      ��M�T�C�Y
 *---------------------------------------------------------------------------*/
u8 IRCi_Read(u8 *buf)
{
    u8 size;

    irc_param.src = (u32)sRecvBuf;
    irc_param.dst = (u32)buf;
    sRecvBuf[0] = COMMAND_READ;
    buf[0] = 0;

    CARD_LockBackup(irc_lock_id);
    (void)OS_LockCard(irc_lock_id);
    IRCi_WaitBusy();

    {
        //u8 debug;
        int i;

        IRCi_EnableSpi(CSPI_CONTINUOUS_ON | MCCNT0_SPI_CLK_SLOW);
        IRCi_CommWrite(&irc_param); // 0���A���Ă���
        IRCi_WaitCommand();
        IRCi_CommRead(&irc_param);  // �ԊO���f�[�^�T�C�Y���A���Ă���
        IRCi_WaitCommand();
        size = buf[0];

        if (size > COM_TRNS_SIZE)
        {
            //OS_TPrintf("SIZE ERR 0x%X, 0x%X\n", debug, size);
            size = 0;
        }
        irc_param.src = (u32)NULL;
        irc_param.dst = (u32)buf;
        buf[0] = 0; // �N���A
        if (size)
        {
            for (i = 0; i < size; i++)
            {
                if (i == size-1)
                {
                    IRCi_EnableSpi(CSPI_CONTINUOUS_OFF | MCCNT0_SPI_CLK_SLOW);
                }
                IRCi_CommRead(&irc_param);
            }
            //OS_TPrintf("\n");
        }
        else
        { // �f�[�^���Ȃ��ꍇ
            IRCi_EnableSpi(CSPI_CONTINUOUS_OFF | MCCNT0_SPI_CLK_SLOW); // �����Ń��[�h���߂�
            IRCi_CommRead(&irc_param); // �f�[�^�]���I���𑗂邽�߂̃_�~�[���[�h
            //if (debug && buf[0]){ OS_TPrintf("SIZE ERR 0x%X\n", debug); }
        }
        // �Í�
        for (i = 0; i < size; i++)
        {
            buf[i] = (u8)(buf[i] ^ 0xAA);
        }
    }
    IRCi_WaitBusy();
    (void)OS_UnLockCard(irc_lock_id);
    CARD_UnlockBackup(irc_lock_id);
    return (size); //return (buf[0]);
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_Write

  Description:  IRC �ɂ��̂܂܃f�[�^���M���s���֐�(�w�b�_��IRC ���C���ō��)

  Arguments:    data  : �������ރf�[�^�̃A�h���X
                size  : �������ރf�[�^�̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRCi_Write(u8 *data, u8 size)
{
    int i;
    // IRC ����R�}���h�𑗂鏀��
    irc_param.src = (u32)sSendBuf;
    irc_param.dst = (u32)NULL;
    sSendBuf[0] = COMMAND_WRITE;

    CARD_LockBackup(irc_lock_id);
    (void)OS_LockCard(irc_lock_id);
    IRCi_WaitBusy();
    {
        // IRC ����R�}���h(���M)�𔭍s
        IRCi_EnableSpi(CSPI_CONTINUOUS_ON | MCCNT0_SPI_CLK_SLOW);
        IRCi_CommWrite(&irc_param);
        IRCi_WaitCommand();

        // �A���C�����g��IRC ����R�}���h�ł���Ȃ��悤�������瑗��f�[�^�̃Z�b�g�A�b�v
        irc_param.src = (u32)data;
        for (i = 0; i < size; i++)
        {
            if (i == size-1)
            {
                IRCi_EnableSpi(CSPI_CONTINUOUS_OFF | MCCNT0_SPI_CLK_SLOW);
            }
            IRCi_CommWrite(&irc_param);
        }
    }
    IRCi_WaitBusy();
    (void)OS_UnLockCard(irc_lock_id);
    CARD_UnlockBackup(irc_lock_id);
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_Check

  Description:  IRC �J�[�h�`�F�b�N���s���֐�

  Arguments:    None.

  Returns:      �V�^ IRC �Ȃ� 0xAA ��Ԃ�
 *---------------------------------------------------------------------------*/
u8 IRCi_Check(void)
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
