/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_spi.c

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


#include <nitro.h>

#include "../include/card_common.h"
#include "../include/card_spi.h"

/* CARD-SPI �̐��� */


/*---------------------------------------------------------------------------*/
/* constants */

/* reg_MI_MCCNT0 �e�r�b�g*/

#define MCCNT0_SPI_CLK_MASK	0x0003 /* �{�[���[�g�ݒ�}�X�N */
#define MCCNT0_SPI_CLK_4M	0x0000 /* �{�[���[�g 4.19MHz */
#define MCCNT0_SPI_CLK_2M	0x0001 /* �{�[���[�g 2.10MHz */
#define MCCNT0_SPI_CLK_1M	0x0002 /* �{�[���[�g 1.05MHz */
#define MCCNT0_SPI_CLK_524K	0x0003 /* �{�[���[�g 524kHz */
#define MCCNT0_SPI_BUSY		0x0080 /* SPI �r�W�[�t���O */
#define	MMCNT0_SEL_MASK		0x2000 /* CARD ROM / SPI �I���}�X�N */
#define	MMCNT0_SEL_CARD		0x0000 /* CARD ROM �I�� */
#define	MMCNT0_SEL_SPI		0x2000 /* CARD SPI �I�� */
#define MCCNT0_INT_MASK		0x4000 /* �]���������荞�݃}�X�N */
#define MCCNT0_INT_ON		0x4000 /* �]���������荞�ݗL�� */
#define MCCNT0_INT_OFF		0x0000 /* �]���������荞�ݖ��� */
#define MCCNT0_MASTER_MASK	0x8000 /* CARD �}�X�^�[�}�X�N */
#define MCCNT0_MASTER_ON	0x8000 /* CARD �}�X�^�[ ON */
#define MCCNT0_MASTER_OFF	0x0000 /* CARD �}�X�^�[ OFF */


/*---------------------------------------------------------------------------*/
/* variables */

typedef struct
{                                      /* SPI �����Ǘ��p�����[�^. */
    u32     rest_comm;                 /* �S���M�o�C�g���̎c��. */
    u32     src;                       /* �]���� */
    u32     dst;                       /* �]���� */
    BOOL    cmp;                       /* ��r���� */
}
CARDiParam;

static CARDiParam cardi_param;


/*---------------------------------------------------------------------------*/
/* functions */

static BOOL CARDi_CommandCheckBusy(void);
static void CARDi_CommArray(const void *src, void *dst, u32 n, void (*func) (CARDiParam *));
static void CARDi_CommReadCore(CARDiParam * p);
static void CARDi_CommWriteCore(CARDiParam * p);
static void CARDi_CommVerifyCore(CARDiParam * p);

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArrayRead

  Description:  ���[�h�R�}���h�̌㑱�ǂݏo������.

  Arguments:    dst               �ǂݏo���惁����.
                len               �ǂݏo���T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_CommArrayRead(void *dst, u32 len)
{
    CARDi_CommArray(NULL, dst, len, CARDi_CommReadCore);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArrayWrite

  Description:  ���C�g�R�}���h�̌㑱�������ݏ���.

  Arguments:    dst               �������݌�������.
                len               �������݃T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_CommArrayWrite(const void *src, u32 len)
{
    CARDi_CommArray(src, NULL, len, CARDi_CommWriteCore);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArrayVerify

  Description:  (��r�̂��߂�)���[�h�R�}���h�̌㑱��r����.

  Arguments:    src               ��r��������.
                len               ��r�T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_CommArrayVerify(const void *src, u32 len)
{
    CARDi_CommArray(src, NULL, len, CARDi_CommVerifyCore);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EnableSpi

  Description:  CARD-SPI ��L���ɂ���.

  Arguments:    cont              �A���]���t���O. (CSPI_CONTINUOUS_ON or OFF)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_EnableSpi(u32 cont)
{
    /*
     * �����N���b�N�X�s�[�h���x���f�o�C�X���o�ꂵ����
     * MCCNT0_SPI_CLK_4M ���v���p�e�B�z��ɒǉ����ē��I�ύX.
     */
    const u16 ctrl = (u16)(MCCNT0_MASTER_ON |
                           MCCNT0_INT_OFF | MMCNT0_SEL_SPI | MCCNT0_SPI_CLK_4M | cont);
    reg_MI_MCCNT0 = ctrl;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitBusy

  Description:  CARD-SPI �̊�����҂�.

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_WaitBusy(void)
{
    while ((reg_MI_MCCNT0 & MCCNT0_SPI_BUSY) != 0)
    {
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandBegin

  Description:  �R�}���h���s�J�n�̐錾.

  Arguments:    len               ���s����R�}���h��.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_CommandBegin(int len)
{
    cardi_param.rest_comm = (u32)len;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandEnd

  Description:  �R�}���h���M��������.

  Arguments:    force_wait     �����E�F�C�g���� [ms]
                timeout        �^�C���A�E�g���� [ms]

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_CommandEnd(u32 force_wait, u32 timeout)
{
    if (force_wait + timeout > 0)
    {
        /*
         * �����E�F�C�g���Ԃ�����΂��̕������ҋ@.
         * �ŏ��̘b�ƈ���� FLASH ���E�F�C�g���������ꂽ.
         */
        if (force_wait > 0)
        {
            OS_Sleep(force_wait);
        }
        if (timeout > 0)
        {
            /*
             * PageWrite �R�}���h�̂݁u�O�����������E�F�C�g�v
             * �Ȃ̂ł��̂悤�ȓ���ȃ��[�v�ɂȂ�.
             */
            int     rest = (int)(timeout - force_wait);
            while (!CARDi_CommandCheckBusy() && (rest > 0))
            {
                int     interval = (rest < 5) ? rest : 5;
                OS_Sleep((u32)interval);
                rest -= interval;
            }
        }
        /*
         * ���̑��̃R�}���h�͎w�莞�ԃE�F�C�g�ς݂Ȃ̂�
         * ReadStatus �� 1 �񂾂��ŗǂ�.
         */
        /* �����܂ł��ăr�W�[�Ȃ�^�C���A�E�g */
        if (!CARDi_CommandCheckBusy())
        {
            cardi_common.cmd->result = CARD_RESULT_TIMEOUT;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandReadStatus

  Description:  ���[�h�X�e�[�^�X

  Arguments:    None

  Returns:      �X�e�[�^�X�l
 *---------------------------------------------------------------------------*/
u8 CARDi_CommandReadStatus(void)
{
    const u8    buf = COMM_READ_STATUS;
    u8          dst;
    CARDi_CommandBegin(1 + 1);
    CARDi_CommArrayWrite(&buf, 1);
    CARDi_CommArrayRead(&dst, 1);
    CARDi_CommandEnd(0, 0);
    return dst;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandCheckBusy

  Description:  �f�o�C�X�̃r�W�[���������ꂽ�����[�h�X�e�[�^�X�R�}���h�Ŕ���.

  Arguments:    None

  Returns:      �r�W�[�łȂ���� TRUE.
 *---------------------------------------------------------------------------*/
static BOOL CARDi_CommandCheckBusy(void)
{
    return ((CARDi_CommandReadStatus() & COMM_STATUS_WIP_BIT) == 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitPrevCommand

  Description:  �R�}���h���s�O�̃r�W�[�`�F�b�N.
                �O��̃R�}���h�ɂ���ăr�W�[��Ԃł���� 50[ms] �܂ő҂�.
                (���C�u�����̎����ƃJ�[�h�̐ڐG������Ȍ���ʏ킠�肦�Ȃ�)

  Arguments:    None.

  Returns:      �R�}���h�����������s�\�ł���� TRUE.
 *---------------------------------------------------------------------------*/
static BOOL CARDi_WaitPrevCommand(void)
{
    CARDi_CommandEnd(0, 50);
    /* �����Ń^�C���A�E�g�̏ꍇ�͖����� */
    if (cardi_common.cmd->result == CARD_RESULT_TIMEOUT)
    {
        cardi_common.cmd->result = CARD_RESULT_NO_RESPONSE;
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArray

  Description:  �R�}���h���s�̋��ʏ���.

  Arguments:    src               �������̃�����. (�s�g�p�Ȃ�NULL)
                dst               ������̃�����. (�s�g�p�Ȃ�NULL)
                len               �����T�C�Y.
                func              �������e.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommArray(const void *src, void *dst, u32 len, void (*func) (CARDiParam *))
{
    CARDiParam *const p = &cardi_param;
    p->src = (u32)src;
    p->dst = (u32)dst;
    CARDi_EnableSpi(CSPI_CONTINUOUS_ON);
    for (; len > 0; --len)
    {
        if (!--p->rest_comm)
        {
            CARDi_EnableSpi(CSPI_CONTINUOUS_OFF);
        }
        CARDi_WaitBusy();
        (*func) (p);
    }
    if (!p->rest_comm)
    {
        reg_MI_MCCNT0 = (u16)(MCCNT0_MASTER_OFF | MCCNT0_INT_OFF);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommReadCore

  Description:  1�o�C�g���[�h����.

  Arguments:    p                 �R�}���h�p�����[�^.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommReadCore(CARDiParam * p)
{
    reg_MI_MCD0 = 0;
    CARDi_WaitBusy();
    MI_WriteByte((void *)p->dst, (u8)reg_MI_MCD0);
    ++p->dst;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommWriteCore

  Description:  1�o�C�g���C�g����.

  Arguments:    p                 �R�}���h�p�����[�^.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommWriteCore(CARDiParam * p)
{
    vu16    tmp;
    reg_MI_MCD0 = (u16)MI_ReadByte((void *)p->src);
    ++p->src;
    CARDi_WaitBusy();
    tmp = reg_MI_MCD0;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommVerifyCore

  Description:  1�o�C�g��r����.

  Arguments:    p                 �R�}���h�p�����[�^.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommVerifyCore(CARDiParam * p)
{
    reg_MI_MCD0 = 0;
    CARDi_WaitBusy();
    /*
     * ���[�h���Ĉ�v���Ȃ���Β��f.
     * �������A���]���𔲂��Ȃ���΂����Ȃ��̂�
     * ���Ȃ��Ƃ� 1 �񂾂��]�v�ɓǂޕK�v������.
     */
    if ((u8)reg_MI_MCD0 != MI_ReadByte((void *)p->src))
    {
        p->cmp = FALSE;
        if (p->rest_comm > 1)
        {
            p->rest_comm = 1;
        }
    }
    ++p->src;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteEnable

  Description:  �f�o�C�X�̏������ݑ���L����. (Write �n�R�}���h�̑O�ɖ���K�v)

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_WriteEnable(void)
{
    static const u8 arg[1] = { COMM_WRITE_ENABLE, };
    CARDi_CommandBegin(sizeof(arg));
    CARDi_CommArrayWrite(arg, sizeof(arg));
    CARDi_CommandEnd(0, 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SendSpiAddressingCommand

  Description:  �A�h���X�w��R�}���h�̐ݒ�.

  Arguments:    addr              �{���ΏۂƂȂ�f�o�C�X��̃A�h���X.
                mode              ���s����R�}���h.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_SendSpiAddressingCommand(u32 addr, u32 mode)
{
    const u32 width = cardi_common.cmd->spec.addr_width;
    u32     addr_cmd;
    switch (width)
    {
    case 1:
        /* 4kbit �f�o�C�X�� [A:8] ���R�}���h�̈ꕔ�ɂȂ� */
        addr_cmd = (u32)(mode | ((addr >> 5) & 0x8) | ((addr & 0xFF) << 8));
        break;
    case 2:
        addr_cmd = (u32)(mode | (addr & 0xFF00) | ((addr & 0xFF) << 16));
        break;
    case 3:
        addr_cmd = (u32)(mode |
                         ((addr >> 8) & 0xFF00) | ((addr & 0xFF00) << 8) | ((addr & 0xFF) << 24));
        break;
    default:
        SDK_ASSERT(FALSE);
        break;
    }
    CARDi_CommArrayWrite(&addr_cmd, (u32)(1 + width));
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitStatusRegister

  Description:  FRAM/EEPROM �̏ꍇ, �N�����ɃX�e�[�^�X���W�X�^�̕␳�������s��.
                �EFRAM �͓d���������Ƀ��C�g�v���e�N�g�̕ω����N���肤�邽��.
                �EEEPROM �͔[�����ɏ����l�s�������肤�邽��.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_InitStatusRegister(void)
{
    /*
     * �X�e�[�^�X���W�X�^���ُ�ȏ����l����肤��f�o�C�X�ɑ΂��Ă�
     * ����g�p���ɓK�X�␳����.
     */
    const u8 stat = cardi_common.cmd->spec.initial_status;
    if (stat != 0xFF)
    {
        static BOOL status_checked = FALSE;
        if (!status_checked)
        {
            if (CARDi_CommandReadStatus() != stat)
            {
                CARDi_SetWriteProtectCore(stat);
            }
            status_checked = TRUE;
        }
    }
}


/********************************************************************/
/*
 * �������ڏ���.
 * ���̒i�K�ł͂��łɔr���⃊�N�G�X�g�����S�Ċ������Ă���.
 * �T�C�Y�̐����ɂ��Ă����łɍl���ς݂̂��̂Ƃ���.
 */

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadBackupCore

  Description:  �f�o�C�X�ւ̃��[�h�R�}���h�S��.

  Arguments:    src               �ǂݏo�����̃f�o�C�X�I�t�Z�b�g.
                dst               �ǂݏo����̃������A�h���X.
                len               �ǂݏo���T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ReadBackupCore(u32 src, void *dst, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        /* ���[�h�Ɋւ��Ă̓y�[�W���E�̐����������̂ňꊇ���� */
        CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + len));
        CARDi_SendSpiAddressingCommand(src, COMM_READ_ARRAY);
        CARDi_CommArrayRead(dst, len);
        CARDi_CommandEnd(0, 0);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramBackupCore

  Description:  �f�o�C�X�ւ̃v���O����(����������������)�R�}���h�S��.

  Arguments:    dst               �������ݐ�̃f�o�C�X�I�t�Z�b�g.
                src               �������݌��̃������A�h���X.
                len               �������݃T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ProgramBackupCore(u32 dst, const void *src, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        /* ���C�g�̓y�[�W���E���܂����Ȃ��悤�ɕ������� */
        const u32 page = cmd->spec.page_size;
        while (len > 0)
        {
            const u32 mod = (u32)(dst & (page - 1));
            u32     size = page - mod;
            if (size > len)
            {
                size = len;
            }
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + size));
            CARDi_SendSpiAddressingCommand(dst, COMM_PROGRAM_PAGE);
            CARDi_CommArrayWrite(src, size);
            CARDi_CommandEnd(cmd->spec.program_page, 0);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            src = (const void *)((u32)src + size);
            dst += size;
            len -= size;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteBackupCore

  Description:  �f�o�C�X�ւ̃��C�g(���� + �v���O����)�R�}���h�S��.

  Arguments:    dst               �������ݐ�̃f�o�C�X�I�t�Z�b�g.
                src               �������݌��̃������A�h���X.
                len               �������݃T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_WriteBackupCore(u32 dst, const void *src, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        /* ���C�g�̓y�[�W���E���܂����Ȃ��悤�ɕ������� */
        const u32 page = cmd->spec.page_size;
        while (len > 0)
        {
            const u32 mod = (u32)(dst & (page - 1));
            u32     size = page - mod;
            if (size > len)
            {
                size = len;
            }
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + size));
            CARDi_SendSpiAddressingCommand(dst, COMM_PAGE_WRITE);
            CARDi_CommArrayWrite(src, size);
            CARDi_CommandEnd(cmd->spec.write_page, cmd->spec.write_page_total);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            src = (const void *)((u32)src + size);
            dst += size;
            len -= size;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_VerifyBackupCore

  Description:  �f�o�C�X�ւ̃x���t�@�C(���ۂ̓��[�h + ��r����)�R�}���h�S��.

  Arguments:    dst               ��r��̃f�o�C�X�I�t�Z�b�g.
                src               ��r���̃������A�h���X.
                len               ��r�T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_VerifyBackupCore(u32 dst, const void *src, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        /* ���[�h�Ɋւ��Ă̓y�[�W���E�̐����������̂ňꊇ���� */
        cardi_param.cmp = TRUE;
        CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + len));
        CARDi_SendSpiAddressingCommand(dst, COMM_READ_ARRAY);
        CARDi_CommArrayVerify(src, len);
        CARDi_CommandEnd(0, 0);
        if ((cmd->result == CARD_RESULT_SUCCESS) && !cardi_param.cmp)
        {
            cmd->result = CARD_RESULT_FAILURE;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSectorCore

  Description:  �f�o�C�X�ւ̃Z�N�^�����R�}���h�S��.

  Arguments:    dst               ������̃f�o�C�X�I�t�Z�b�g.
                len               �����T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EraseBackupSectorCore(u32 dst, u32 len)
{
    CARDiCommandArg *const cmd = cardi_common.cmd;
    const u32 sector = cmd->spec.sect_size;
    /* �����͈͂��Z�N�^�̐����{�ɐ������Ă��Ȃ��ꍇ�͏��������Ȃ� */
    if (((dst | len) & (sector - 1)) != 0)
    {
        cmd->result = CARD_RESULT_INVALID_PARAM;
    }
    else if (CARDi_WaitPrevCommand())
    {
        /* �Z�N�^���E�P�ʂŏ��� */
        while (len > 0)
        {
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + 0));
            CARDi_SendSpiAddressingCommand(dst, COMM_SECTOR_ERASE);
            CARDi_CommandEnd(cmd->spec.erase_sector, cmd->spec.erase_sector_total);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            dst += sector;
            len -= sector;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSubSectorCore

  Description:  �f�o�C�X�ւ̃T�u�Z�N�^�����R�}���h�S��.

  Arguments:    dst               ������̃f�o�C�X�I�t�Z�b�g.
                len               �����T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EraseBackupSubSectorCore(u32 dst, u32 len)
{
    CARDiCommandArg *const cmd = cardi_common.cmd;
    const u32 sector = cmd->spec.subsect_size;
    /* �����͈͂��T�u�Z�N�^�̐����{�ɐ������Ă��Ȃ��ꍇ�͏��������Ȃ� */
    if (((dst | len) & (sector - 1)) != 0)
    {
        cmd->result = CARD_RESULT_INVALID_PARAM;
    }
    else if (CARDi_WaitPrevCommand())
    {
        /* �Z�N�^���E�P�ʂŏ��� */
        while (len > 0)
        {
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + 0));
            CARDi_SendSpiAddressingCommand(dst, COMM_SUBSECTOR_ERASE);
            CARDi_CommandEnd(cmd->spec.erase_subsector, cmd->spec.erase_subsector_total);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            dst += sector;
            len -= sector;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseChipCore

  Description:  �f�o�C�X�ւ̃`�b�v�����R�}���h�S��.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EraseChipCore(void)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        static const u8 arg[1] = { COMM_CHIP_ERASE, };
        CARDi_WriteEnable();
        CARDi_CommandBegin(sizeof(arg));
        CARDi_CommArrayWrite(arg, sizeof(arg));
        CARDi_CommandEnd(cmd->spec.erase_chip, cmd->spec.erase_chip_total);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetWriteProtectCore

  Description:  �f�o�C�X�ւ̃��C�g�v���e�N�g�R�}���h�S��.

  Arguments:    stat              �ݒ肷��v���e�N�g�t���O.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_SetWriteProtectCore(u16 stat)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg *const cmd = cardi_common.cmd;
        int     retry_count = 10;
        u8      arg[2];
        arg[0] = COMM_WRITE_STATUS;
        arg[1] = (u8)stat;
        do
        {
            CARDi_WriteEnable();
            CARDi_CommandBegin(1 + 1);
            CARDi_CommArrayWrite(&arg, sizeof(arg));
            CARDi_CommandEnd(5, 0);
        }
        while ((cmd->result == CARD_RESULT_TIMEOUT) && (--retry_count > 0));
    }
}


#if	0

/********************************************************************/
/********************************************************************/
/* ���ؒ��f�o�C�X�ŗL�R�}���h ***************************************/
/********************************************************************/
/********************************************************************/


/* ID ���[�h */
static void CARDi_ReadIdCore(void)
{
    /*
     * EEPROM, FRAM �ɂ͂��̃R�}���h������.
     * ST �� 2Mbit FLASH �ɂ����̃R�}���h������?
     * �Ή��O�̃R�}���h�o�C�g���o���̓����`����������Ȃ�.
     * �Ƃ����, ���̃R�}���h�� 2Mbit ���͏�Ɏg�p�s�Ƃ��邩
     * CARD_BACKUP_TYPE_FLASH_2MBITS_SANYO �Ȃǂƍו������邩.
     * ������ɂ��悻���܂Ŏ��O�ɋ敪���Ă���̂�
     * ID ���K�v�ɂȂ�V�[��������̂��낤��...?
     * �]�͂�����Γ����Ő���������Ɏg�p�\��.
     */
    cardi_common.cmd->result = CARD_RESULT_UNSUPPORTED;
}

/* 2M FLASH �̂ݎg�p�\�ȃR�}���h **********************************/

/* �y�[�W���� (FLASH) */
static void CARDi_ErasePageCore(u32 dst)
{
    CARDi_WriteEnable();
    CARDi_CommandBegin(1 + cardi_common.cmd->spec.addr_width + 0);
    CARDi_SendSpiAddressingCommand(dst, COMM_PAGE_ERASE);
    CARDi_CommandEnd(cardi_common.cmd->spec.erase_page, 0);
}


#endif
