/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_rom.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-07-14#$
  $Rev: 10904 $
  $Author: yosizaki $

 *---------------------------------------------------------------------------*/


#include <nitro/card/rom.h>
#include <nitro/card/pullOut.h>
#include <nitro/card/rom.h>


#include "../include/card_common.h"
#include "../include/card_rom.h"


#if defined(SDK_ARM9) && defined(SDK_TWL)
#define CARD_USING_HASHCHECK
#endif // defined(SDK_ARM9) && defined(SDK_TWL)

#if defined(SDK_ARM9) || (defined(SDK_ARM7) && defined(SDK_ARM7_READROM_SUPPORT))
#define CARD_USING_ROMREADER
#endif // defined(SDK_ARM9) || (defined(SDK_ARM7) && defined(SDK_ARM7_READROM_SUPPORT))


/*---------------------------------------------------------------------------*/
/* constants */

#define CARD_COMMAND_PAGE           0x01000000
#define CARD_COMMAND_ID             0x07000000
#define CARD_COMMAND_REFRESH        0x00000000
#define CARD_COMMAND_STAT           CARD_COMMAND_ID
#define CARD_COMMAND_MASK           0x07000000
#define CARD_RESET_HI               0x20000000
#define CARD_COMMAND_OP_G_READID    0xB8
#define CARD_COMMAND_OP_G_READPAGE  0xB7
#define CARD_COMMAND_OP_G_READSTAT  0xD6
#define CARD_COMMAND_OP_G_REFRESH   0xB5
#ifdef SDK_TWL
#define CARD_COMMAND_OP_N_READID    0x90
#define CARD_COMMAND_OP_N_READPAGE  0x00
#define CARD_COMMAND_OP_N_READSTAT  CARD_COMMAND_OP_G_READSTAT
#define CARD_COMMAND_OP_N_REFRESH   CARD_COMMAND_OP_G_REFRESH
#endif

// ROM ID

#define CARD_ROMID_1TROM_MASK       0x80000000UL  // 1T-ROM type
#define CARD_ROMID_TWLROM_MASK      0x40000000UL  // TWL-ROM
#define CARD_ROMID_REFRESH_MASK     0x20000000UL  // ���t���b�V���T�|�[�g

// ROM STATUS

#define CARD_ROMST_RFS_WARN_L1_MASK 0x00000004UL
#define CARD_ROMST_RFS_WARN_L2_MASK 0x00000008UL
#define CARD_ROMST_RFS_READY_MASK   0x00000020UL


/*---------------------------------------------------------------------------*/
/* variables */

// ���[�h���̃x�[�X�I�t�Z�b�g�B(�ʏ�� 0)
u32         cardi_rom_base;

// ���̑��̓������B
static int                (*CARDiReadRomFunction) (void *userdata, void *buffer, u32 offset, u32 length);

static CARDTransferInfo     CARDiDmaReadInfo[1];
static CARDTransferInfo    *CARDiDmaReadRegisteredInfo;

static u32                  cache_page = 1;
static u8                   CARDi_cache_buf[CARD_ROM_PAGE_SIZE] ATTRIBUTE_ALIGN(32);
static BOOL                 CARDiEnableCacheInvalidationIC = FALSE;
static BOOL                 CARDiEnableCacheInvalidationDC = TRUE;

extern BOOL OSi_IsThreadInitialized;

static u8                   CARDiOwnSignature[CARD_ROM_DOWNLOAD_SIGNATURE_SIZE] ATTRIBUTE_ALIGN(4);


/*---------------------------------------------------------------------------*/
/* functions */

void CARD_RefreshRom(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetRomOp

  Description:  �J�[�h�R�}���h�ݒ�

  Arguments:    command    �R�}���h
                offset     �]���y�[�W��

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_SetRomOp(u32 command, u32 offset)
{
    u32     cmd1 = (u32)((offset >> 8) | (command << 24));
    u32     cmd2 = (u32)((offset << 24));
    // �O�̂��ߑO���ROM�R�}���h�̊����҂��B
    while ((reg_MI_MCCNT1 & REG_MI_MCCNT1_START_MASK) != 0)
    {
    }
    // �}�X�^�[�C�l�[�u���B
    reg_MI_MCCNT0 = (u16)(REG_MI_MCCNT0_E_MASK | REG_MI_MCCNT0_I_MASK |
                          (reg_MI_MCCNT0 & ~REG_MI_MCCNT0_SEL_MASK));
    // �R�}���h�ݒ�B
    reg_MI_MCCMD0 = MI_HToBE32(cmd1);
    reg_MI_MCCMD1 = MI_HToBE32(cmd2);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetRomFlag

  Description:  �J�[�h�R�}���h�R���g���[���p�����[�^���擾

  Arguments:    flag       �J�[�h�f�o�C�X�֔��s����R�}���h�̃^�C�v
                           (CARD_COMMAND_PAGE / CARD_COMMAND_ID /
                            CARD_COMMAND_STAT / CARD_COMMAND_REFRESH)

  Returns:      �J�[�h�R�}���h�R���g���[���p�����[�^
 *---------------------------------------------------------------------------*/
SDK_INLINE u32 CARDi_GetRomFlag(u32 flag)
{
    u32     rom_ctrl = *(vu32 *)(HW_CARD_ROM_HEADER + 0x60);
    return (u32)(flag | REG_MI_MCCNT1_START_MASK | CARD_RESET_HI | (rom_ctrl & ~CARD_COMMAND_MASK));
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_IsTwlRom

  Description:  �Q�[���J�[�h��TWL�Ή�ROM�𓋍ڂ��Ă��邩�ǂ�������B
                Nitro�A�v���P�[�V�����ł�TWL�Ή�ROM���ڎ���TRUE��Ԃ��܂��B

  Arguments:    None.

  Returns:      TWL�Ή�ROM�Ȃ�TRUE�B
 *---------------------------------------------------------------------------*/
BOOL CARDi_IsTwlRom(void)
{
    u32 iplCardID = *(u32 *)(HW_BOOT_CHECK_INFO_BUF);

    // �N��������J�[�h��������Ώ��FALSE
    if ( ! iplCardID )
    {
        return FALSE;
    }

    return (CARDi_ReadRomID() & CARD_ROMID_TWLROM_MASK) ? TRUE : FALSE;
}

#ifdef SDK_TWL

/*---------------------------------------------------------------------------*
  Name:         CARDi_IsNormalMode

  Description:  �Q�[���J�[�h��NORMAL���[�h���ǂ�������B

  Arguments:    None.

  Returns:      �Q�[���J�[�h��NORMAL���[�h�Ȃ�TRUE�B
 *---------------------------------------------------------------------------*/
static BOOL CARDi_IsNormalMode(void)
{
    const CARDRomHeaderTWL *oh = CARD_GetOwnRomHeaderTWL();

    return OS_IsRunOnTwl() &&
           (OS_GetBootType() != OS_BOOTTYPE_ROM) &&
           oh->access_control.game_card_on &&
           ! oh->access_control.game_card_nitro_mode;
}

#endif

/*---------------------------------------------------------------------------*
  Name:         CARDi_StartRomPageTransfer

  Description:  ROM�y�[�W�]�����J�n�B

  Arguments:    offset     �]������ROM�I�t�Z�b�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_StartRomPageTransfer(u32 offset)
{
    u8 op = CARD_COMMAND_OP_G_READPAGE;
#ifdef SDK_TWL
    if ( CARDi_IsNormalMode() )
    {
        op = CARD_COMMAND_OP_N_READPAGE;
    }
#endif

    CARDi_SetRomOp(op, offset);
    reg_MI_MCCNT1 = CARDi_GetRomFlag(CARD_COMMAND_PAGE);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomIDCore

  Description:  �J�[�h ID �̓ǂݏo���B

  Arguments:    None.

  Returns:      �J�[�h ID
 *---------------------------------------------------------------------------*/
u32 CARDi_ReadRomIDCore(void)
{
    u8 op = CARD_COMMAND_OP_G_READID;
#ifdef SDK_TWL
    if ( CARDi_IsNormalMode() )
    {
        op = CARD_COMMAND_OP_N_READID;
    }
#endif

    CARDi_SetRomOp(op, 0);
    reg_MI_MCCNT1 = (u32)(CARDi_GetRomFlag(CARD_COMMAND_ID) & ~REG_MI_MCCNT1_L1_MASK);
    while ((reg_MI_MCCNT1 & REG_MI_MCCNT1_RDY_MASK) == 0)
    {
    }
    return reg_MI_MCD1;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomStatusCore

  Description:  �J�[�h�X�e�[�^�X�̓ǂݏo���B
                ���t���b�V���Ή�ROM���o���̂ݔ��s�B
                �Ή�ROM�𓋍ڂ���NITRO�A�v���P�[�V�����ł��K�v�ɂȂ�B

  Arguments:    None.

  Returns:      �J�[�h�X�e�[�^�X
 *---------------------------------------------------------------------------*/
u32 CARDi_ReadRomStatusCore(void)
{
    u32 iplCardID = *(u32 *)(HW_BOOT_CHECK_INFO_BUF);

    if ( ! (iplCardID & CARD_ROMID_REFRESH_MASK) )
    {
        return CARD_ROMST_RFS_READY_MASK;
    }

    CARDi_SetRomOp(CARD_COMMAND_OP_G_READSTAT, 0);
    reg_MI_MCCNT1 = (u32)(CARDi_GetRomFlag(CARD_COMMAND_STAT) & ~REG_MI_MCCNT1_L1_MASK);
    while ((reg_MI_MCCNT1 & REG_MI_MCCNT1_RDY_MASK) == 0)
    {
    }
    return reg_MI_MCD1;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_RefreshRom

  Description:  �J�[�hROM�o�b�h�u���b�N�̃��t���b�V���B
                ���t���b�V���Ή�ROM���o���̂ݔ��s�B
                �Y��ROM�𓋍ڂ���NITRO�A�v���P�[�V�����ł��K�v�ɂȂ�B
                CARD_ReadRom���ł��ɂ߂ċH��ECC�␳�s�\�ԋ߂̏�ԂɂȂ�ƁA
                �����I�Ƀ��t���b�V������܂����A���t���b�V��������50msec���x
                ������\��������܂��̂ŁA�A�v���P�[�V�����ɂƂ���
                �x���������Ă����S�ȃ^�C�~���O�Œ���I�ɌĂяo���ĉ������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_RefreshRom(void)
{
    SDK_ASSERT(CARD_IsAvailable());
    SDK_TASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_ROM, "must be locked by CARD_LockRom()");

#if defined(SDK_ARM9)
    (void)CARDi_WaitForTask(&cardi_common, TRUE, NULL, NULL);
    // �����ŃJ�[�h�������o���s��
    CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
#endif // defined(SDK_ARM9)

    CARDi_RefreshRom(CARD_ROMST_RFS_WARN_L1_MASK | CARD_ROMST_RFS_WARN_L2_MASK);

#if defined(SDK_ARM9)
    cardi_common.cmd->result = CARD_RESULT_SUCCESS;
    CARDi_EndTask(&cardi_common);
#endif // defined(SDK_ARM9)
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_RefreshRom

  Description:  �J�[�hROM�o�b�h�u���b�N�̃��t���b�V���B
                ���t���b�V���Ή�ROM���o���̂ݔ��s�B
                �Y��ROM�𓋍ڂ���NITRO�A�v���P�[�V�����ł��K�v�ɂȂ�B

  Arguments:    warn_mask         ���[�j���O���x���w��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_RefreshRom(u32 warn_mask)
{
    if (CARDi_ReadRomStatusCore() & warn_mask)
    {
        CARDi_RefreshRomCore();
        // ���t���b�V�������܂�100msec�ȏォ����\��������
        while ( !(CARDi_ReadRomStatusCore() & CARD_ROMST_RFS_READY_MASK) )
        {
            // �\�ł���΃X���[�v
            if ( OSi_IsThreadInitialized && 
                 OS_IsAlarmAvailable() )
            {
                OS_Sleep(1);
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_RefreshRomCore

  Description:  �J�[�hROM�o�b�h�u���b�N�̃��t���b�V���B
                �Y��ROM�𓋍ڂ���NITRO�A�v���P�[�V�����ł��K�v�ɂȂ�B
                �J�[�h�փR�}���h������I�ɔ��s���邾���Ȃ̂Ń��C�e���V�ݒ�͖����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_RefreshRomCore(void)
{
    CARDi_SetRomOp(CARD_COMMAND_OP_G_REFRESH, 0);
    reg_MI_MCCNT1 = (u32)(CARDi_GetRomFlag(CARD_COMMAND_REFRESH) & ~REG_MI_MCCNT1_L1_MASK);
    while (reg_MI_MCCNT1 & REG_MI_MCCNT1_START_MASK)
    {
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomWithCPU

  Description:  CPU���g�p����ROM�]���B
                �L���b�V����y�[�W�P�ʂ̐������l������K�v�͖�����
                �]�������܂Ŋ֐����u���b�L���O����_�ɒ��ӁB

  Arguments:    userdata          (���̃R�[���o�b�N�Ƃ��Ďg�p���邽�߂̃_�~�[)
                buffer            �]����o�b�t�@
                offset            �]����ROM�I�t�Z�b�g
                length            �]���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
int CARDi_ReadRomWithCPU(void *userdata, void *buffer, u32 offset, u32 length)
{
    int     retval = (int)length;
    // �p�ɂɎg�p����O���[�o���ϐ������[�J���ϐ��փL���b�V���B
    u32         cachedPage = cache_page;
    u8  * const cacheBuffer = CARDi_cache_buf;
    while (length > 0)
    {
        // ROM�]���͏�Ƀy�[�W�P�ʁB
        u8     *ptr = (u8 *)buffer;
        u32     n = CARD_ROM_PAGE_SIZE;
        u32     pos = MATH_ROUNDDOWN(offset, CARD_ROM_PAGE_SIZE);
        // �ȑO�̃y�[�W�Ɠ����Ȃ�΃L���b�V�����g�p�B
        if (pos == cachedPage)
        {
            ptr = cacheBuffer;
        }
        else
        {
            // �o�b�t�@�֒��ړ]���ł��Ȃ��Ȃ�L���b�V���֓]���B
            if(((pos != offset) || (((u32)buffer & 3) != 0) || (length < n)))
            {
                cachedPage = pos;
                ptr = cacheBuffer;
            }
            // 4�o�C�g�����̕ۏ؂��ꂽ�o�b�t�@��CPU�Œ��ڃ��[�h�B
            CARDi_StartRomPageTransfer(pos);
            {
                u32     word = 0;
                for (;;)
                {
                    // 1���[�h�]��������҂B
                    u32     ctrl = reg_MI_MCCNT1;
                    if ((ctrl & REG_MI_MCCNT1_RDY_MASK) != 0)
                    {
                        // �f�[�^��ǂݏo���A�K�v�Ȃ�o�b�t�@�֊i�[�B
                        u32     data = reg_MI_MCD1;
                        if (word < (CARD_ROM_PAGE_SIZE / sizeof(u32)))
                        {
                            ((u32 *)ptr)[word++] = data;
                        }
                    }
                    // 1�y�[�W�]�������Ȃ�I���B
                    if ((ctrl & REG_MI_MCCNT1_START_MASK) == 0)
                    {
                        break;
                    }
                }
            }
        }
        // �L���b�V���o�R�Ȃ�L���b�V������]���B
        if (ptr == cacheBuffer)
        {
            u32     mod = offset - pos;
            n = MATH_MIN(length, CARD_ROM_PAGE_SIZE - mod);
            MI_CpuCopy8(cacheBuffer + mod, buffer, n);
        }
        buffer = (u8 *)buffer + n;
        offset += n;
        length -= n;
    }
    // �A�N�Z�X���ɔ��������J�[�h�����𔻒�B
    CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
    // ���[�J���ϐ�����O���[�o���ϐ��֔��f�B
    cache_page = cachedPage;
    (void)userdata;
    return retval;
}

#if defined(CARD_USING_ROMREADER)
/*---------------------------------------------------------------------------*
  Name:         CARDi_DmaReadPageCallback

  Description:  ROM�y�[�WDMA�]�������R�[���o�b�N�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_DmaReadPageCallback(void)
{
    CARDTransferInfo   *info = CARDiDmaReadRegisteredInfo;
    if (info)
    {
        info->src += CARD_ROM_PAGE_SIZE;
        info->dst += CARD_ROM_PAGE_SIZE;
        info->len -= CARD_ROM_PAGE_SIZE;
        // �K�v�Ȃ玟�̃y�[�W��]���B
        if (info->len > 0)
        {
            CARDi_StartRomPageTransfer(info->src);
        }
        // �]�������B
        else
        {
            cardi_common.DmaCall->Stop(cardi_common.dma);
            (void)OS_DisableIrqMask(OS_IE_CARD_DATA);
            (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
            CARDiDmaReadRegisteredInfo = NULL;
            // �A�N�Z�X���ɔ��������J�[�h�����𔻒�B
            CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
            if (info->callback)
            {
                (*info->callback)(info->userdata);
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomWithDMA

  Description:  DMA���g�p����ROM�]���B
                �ŏ��̃y�[�W�̓]�����J�n������֐��͂������ɐ����Ԃ��B

  Arguments:    info              �]�����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ReadRomWithDMA(CARDTransferInfo *info)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    CARDiDmaReadRegisteredInfo = info;
    // �J�[�h�]���������荞�݂�ݒ�B
    (void)OS_SetIrqFunction(OS_IE_CARD_DATA, CARDi_DmaReadPageCallback);
    (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
    (void)OS_EnableIrqMask(OS_IE_CARD_DATA);
    (void)OS_RestoreInterrupts(bak_psr);
    // CARD-DMA�̐ݒ�B (�]���ҋ@)
    cardi_common.DmaCall->Recv(cardi_common.dma, (void *)&reg_MI_MCD1, (void *)info->dst, CARD_ROM_PAGE_SIZE);
    // �ŏ���ROM�]���J�n�B
    CARDi_StartRomPageTransfer(info->src);
}

static void CARDi_DmaReadDone(void *userdata)
{
    (void)userdata;
#ifdef SDK_ARM9
    // �A�N�Z�X���ɔ��������J�[�h�����𔻒�B
    CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
#endif
    // ROM-ECC�␳�̍ŏI�x���i�K�Ń��t���b�V��
    CARDi_RefreshRom(CARD_ROMST_RFS_WARN_L2_MASK);

    cardi_common.cmd->result = CARD_RESULT_SUCCESS;
    CARDi_EndTask(&cardi_common);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_IsRomDmaAvailable

  Description:  ROM�]���ɔ񓯊�DMA���g�p�\������B

  Arguments:    dma        DMA�`�����l��
                dst        �]����o�b�t�@
                src        �]����ROM�I�t�Z�b�g
                len        �]���T�C�Y

  Returns:      ROM�]����DMA���g�p�\�Ȃ�TRUE�B
 *---------------------------------------------------------------------------*/
static BOOL CARDi_IsRomDmaAvailable(u32 dma, void *dst, u32 src, u32 len)
{
    return (dma <= MI_DMA_MAX_NUM) && (len > 0) && (((u32)dst & 31) == 0) &&
#ifdef SDK_ARM9
        (((u32)dst + len <= OS_GetITCMAddress()) || ((u32)dst >= OS_GetITCMAddress() + HW_ITCM_SIZE)) &&
        (((u32)dst + len <= OS_GetDTCMAddress()) || ((u32)dst >= OS_GetDTCMAddress() + HW_DTCM_SIZE)) &&
#endif
        (((src | len) & (CARD_ROM_PAGE_SIZE - 1)) == 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomSyncCore

  Description:  �����J�[�h�ǂݍ��݂̋��ʏ���

  Arguments:    c          CARDiCommon �\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_ReadRomSyncCore(CARDiCommon *c)
{
    // ���s���ɉ�����ROM�A�N�Z�X���[�`�������s�B
    (void)(*CARDiReadRomFunction)(NULL, (void*)c->dst, c->src, c->len);
#ifdef SDK_ARM9
    // �A�N�Z�X���ɔ��������J�[�h�����𔻒�B
    CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
#endif
    // ROM-ECC�␳�̍ŏI�x���i�K�Ń��t���b�V��
    CARDi_RefreshRom(CARD_ROMST_RFS_WARN_L2_MASK);

    cardi_common.cmd->result = CARD_RESULT_SUCCESS;
}
#endif // defined(CARD_USING_ROMREADER)

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRom

  Description:  ROM ���[�h�̊�{�֐�

  Arguments:    dma        �g�p���� DMA �`�����l��
                src        �]�����I�t�Z�b�g
                dst        �]���惁�����A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊����[�h���w�肷��Ȃ� TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ReadRom(u32 dma,
                   const void *src, void *dst, u32 len,
                   MIDmaCallback callback, void *arg, BOOL is_async)
{
#if defined(CARD_USING_ROMREADER)
    CARDiCommon *const c = &cardi_common;

    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERT(CARDi_GetTargetMode() == CARD_TARGET_ROM);
    SDK_TASSERTMSG((dma != 0), "cannot specify DMA channel 0");

    // CARD�A�N�Z�X�ɑ΂��鐳�����̔���B
    CARD_CheckEnabled();
    if ((CARDi_GetAccessLevel() & CARD_ACCESS_LEVEL_ROM) == 0)
    {
        OS_TPanic("this program cannot access CARD-ROM!");
    }

    // ARM9 ���̔r���҂��B
    (void)CARDi_WaitForTask(c, TRUE, callback, arg);

    // DMA�C���^�t�F�[�X�̎擾�B
    c->DmaCall = CARDi_GetDmaInterface(dma);
    c->dma = (u32)((c->DmaCall != NULL) ? (dma & MI_DMA_CHANNEL_MASK) : MI_DMA_NOT_USE);
    if (c->dma <= MI_DMA_MAX_NUM)
    {
        c->DmaCall->Stop(c->dma);
    }

    // ����̓]���p�����[�^��ݒ�B
    c->src = (u32)((u32)src + cardi_rom_base);
    c->dst = (u32)dst;
    c->len = (u32)len;

    // �]���p�����[�^��ݒ�B
    {
        CARDTransferInfo   *info = CARDiDmaReadInfo;
        info->callback = CARDi_DmaReadDone;
        info->userdata = NULL;
        info->src = c->src;
        info->dst = c->dst;
        info->len = c->len;
        info->work = 0;
    }

    // �n�b�V���`�F�b�N�������Ȋ��Ȃ�ꍇ�ɂ����DMA�]�������p�\�B
    if ((CARDiReadRomFunction == CARDi_ReadRomWithCPU) &&
        CARDi_IsRomDmaAvailable(c->dma, (void *)c->dst, c->src, c->len))
    {
#if defined(SDK_ARM9)
        // �]���͈͂̃T�C�Y�ɉ����ăL���b�V���������̕��@��؂�ւ��B
        OSIntrMode bak_psr = OS_DisableInterrupts();
        if (CARDiEnableCacheInvalidationIC)
        {
            CARDi_ICInvalidateSmart((void *)c->dst, c->len, c->flush_threshold_ic);
        }
        if (CARDiEnableCacheInvalidationDC)
        {
            CARDi_DCInvalidateSmart((void *)c->dst, c->len, c->flush_threshold_dc);
        }
        (void)OS_RestoreInterrupts(bak_psr);
#endif
        // DMA�ɂ��񓯊��]���J�n�B
        CARDi_ReadRomWithDMA(CARDiDmaReadInfo);
        // �����]����v������Ă���Ȃ炱���Ŋ����҂��B
        if (!is_async)
        {
            CARD_WaitRomAsync();
        }
    }
    else
    {
        // CPU�]���̏ꍇ�ł����߃L���b�V���͕s�����ɂȂ�̂Ŗ��������K�v�B
        if (CARDiEnableCacheInvalidationIC)
        {
            CARDi_ICInvalidateSmart((void *)c->dst, c->len, c->flush_threshold_ic);
        }
        // CPU�ɂ�鉽�炩�̒��ڏ������K�v�Ȃ�X���b�h�փ^�X�N�o�^�B
        (void)CARDi_ExecuteOldTypeTask(CARDi_ReadRomSyncCore, is_async);
    }
#else
    (void)dma;
    (void)is_async;
    (void)CARDi_ReadRomWithCPU(NULL, dst, (u32)src, len);
    if (callback)
    {
        (*callback)(arg);
    }
#endif // defined(CARD_USING_ROMREADER)
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomID

  Description:  �J�[�h ID �̓ǂݏo��

  Arguments:    None.

  Returns:      �J�[�h ID.
 *---------------------------------------------------------------------------*/
u32 CARDi_ReadRomID(void)
{
    u32     ret = 0;

    SDK_ASSERT(CARD_IsAvailable());
    SDK_TASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_ROM, "must be locked by CARD_LockRom()");

#if defined(SDK_ARM9)
    (void)CARDi_WaitForTask(&cardi_common, TRUE, NULL, NULL);
#endif // defined(SDK_ARM9)

    /* ���ڃA�N�Z�X�\�Ȃ̂ł����Ŏ��s */
    ret = CARDi_ReadRomIDCore();
#ifdef SDK_ARM9
    // �����ŃJ�[�h�������o���s��
    CARDi_CheckPulledOutCore(ret);
#endif

#if defined(SDK_ARM9)
    cardi_common.cmd->result = CARD_RESULT_SUCCESS;
    CARDi_EndTask(&cardi_common);
#endif // defined(SDK_ARM9)

    return ret;
}

#if defined(CARD_USING_HASHCHECK)
#include <twl/ltdmain_begin.h>

// ROM�n�b�V���v�Z�p�o�b�t�@�B
u8     *CARDiHashBufferAddress = NULL;
u32     CARDiHashBufferLength = 0;
static CARDRomHashContext   context[1];

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadCardWithHash

  Description:  �n�b�V�����m�F����ROM�]���B

  Arguments:    buffer            �]����o�b�t�@
                offset            �]����ROM�I�t�Z�b�g
                length            �]���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
static int CARDi_ReadCardWithHash(void *userdata, void *buffer, u32 offset, u32 length)
{
    (void)userdata;
    CARD_ReadRomHashImage(context, buffer, offset, length);
    return (int)length;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadCardWithHashInternalAsync

  Description:  �n�b�V�����ؔ�ROM�]���̓����ŕ����I�Ɏg�p����񓯊�DMA�]���B

  Arguments:    userdata          ���[�U��`�̈���
                buffer            �]����o�b�t�@
                offset            �]����ROM�I�t�Z�b�g
                length            �]���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
static int CARDi_ReadCardWithHashInternalAsync(void *userdata, void *buffer, u32 offset, u32 length)
{
    if (cardi_common.dma == MI_DMA_NOT_USE)
    {
        length = 0;
    }
    else
    {
        CARDRomHashContext *context = (CARDRomHashContext *)userdata;
        static CARDTransferInfo card_hash_info[1];
        DC_FlushRange(buffer, length);
        card_hash_info->callback = (void(*)(void*))CARD_NotifyRomHashReadAsync;
        card_hash_info->userdata = context;
        card_hash_info->src = offset;
        card_hash_info->dst = (u32)buffer;
        card_hash_info->len = length;
        card_hash_info->command = 0;
        card_hash_info->work = 0;
        CARDi_ReadRomWithDMA(card_hash_info);
    }
    return (int)length;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitRomHash

  Description:  �\�Ȃ�n�b�V�����ؔ�ROM�]���ɐ؂�ւ���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_InitRomHash(void)
{
    // FS_Init�֐��Ȃǂ̌Ăяo���ŕK�v�Ƃ������O��
    // �A�v���P�[�V�������A���[�i��S�ď���Ă��܂����Ƃ��������߁A
    // �n�b�V���R���e�L�X�g�p�o�b�t�@�͏�ɂ����Ŋm�ۂ��Ă����B
    u8     *lo = (u8 *)MATH_ROUNDUP((u32)OS_GetMainArenaLo(), 32);
    u8     *hi = (u8 *)MATH_ROUNDDOWN((u32)OS_GetMainArenaHi(), 32);
    u32     len = CARD_CalcRomHashBufferLength(CARD_GetOwnRomHeaderTWL());
    if (&lo[len] > hi)
    {
        OS_TPanic("cannot allocate memory for ROM-hash from ARENA");
    }
    else
    {
        OS_SetMainArenaLo(&lo[len]);
        CARDiHashBufferAddress = lo;
        CARDiHashBufferLength = len;
        // ROM�u�[�g���n�b�V���e�[�u�������݂�����Ȃ���ۂɃ��[�h�B
        if ((OS_GetBootType() == OS_BOOTTYPE_ROM) &&
            ((((const u8 *)HW_TWL_ROM_HEADER_BUF)[0x1C] & 0x01) != 0))
        {
            cardi_common.dma = MI_DMA_NOT_USE;
            CARDiReadRomFunction = CARDi_ReadCardWithHash;
            {
                u16     id = (u16)OS_GetLockID();
                CARD_LockRom(id);
                CARD_InitRomHashContext(context,
                                        CARD_GetOwnRomHeaderTWL(),
                                        CARDiHashBufferAddress,
                                        CARDiHashBufferLength,
                                        CARDi_ReadRomWithCPU,
                                        CARDi_ReadCardWithHashInternalAsync,
                                        context);
                // �����o�b�t�@���������Ďg�p����Ȃ��悤�|�C���^��j���B
                CARDiHashBufferAddress = NULL;
                CARDiHashBufferLength = 0;
                CARD_UnlockRom(id);
                OS_ReleaseLockID(id);
            }
        }
    }
}
#include <twl/ltdmain_end.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitRom

  Description:  ROM�A�N�Z�X�Ǘ������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_InitRom(void)
{
#if defined(CARD_USING_ROMREADER)
    CARDiReadRomFunction = CARDi_ReadRomWithCPU;
    // TWL-SDK�ł�ROM�f�[�^�A�N�Z�X�Ƀn�b�V�����؂����{���邪
    // DS�_�E�����[�h�v���C�̏����f�[�^�̓n�b�V���v�Z��ɖ��ߍ��܂�邽�߁A
    // �����CARD_ReadRom()�ŕs�p�ӂɓǂݏo�����Ƃ��ւ����
    // �����I��CARDi_GetOwnSignature()�ŎQ�Ƃ���K�v������B
    if ((OS_GetBootType() == OS_BOOTTYPE_ROM) && CARD_GetOwnRomHeader()->rom_size)
    {
        u16     id = (u16)OS_GetLockID();
        CARD_LockRom(id);
        (void)CARDi_ReadRomWithCPU(NULL, CARDiOwnSignature,
                                   CARD_GetOwnRomHeader()->rom_size,
                                   CARD_ROM_DOWNLOAD_SIGNATURE_SIZE);
        CARD_UnlockRom(id);
        OS_ReleaseLockID(id);
    }
#if defined(CARD_USING_HASHCHECK)
    // ���p�\�Ȃ�n�b�V�����ؕt���̃��[�`�����̗p�B
    if (OS_IsRunOnTwl())
    {
		CARDi_InitRomHash();
    }
#endif
#else
    CARDiReadRomFunction = NULL;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         CARD_WaitRomAsync

  Description:  ROM�A�N�Z�X�֐�����������܂őҋ@

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_WaitRomAsync(void)
{
    (void)CARDi_WaitAsync();
}

/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitRomAsync

  Description:  ROM�A�N�Z�X�֐�����������������

  Arguments:    None.

  Returns:      ROM�A�N�Z�X�֐����������Ă����TRUE
 *---------------------------------------------------------------------------*/
BOOL CARD_TryWaitRomAsync(void)
{
    return CARDi_TryWaitAsync();
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetOwnSignature

  Description:  �������g��DS�_�E�����[�h�v���C�����f�[�^���擾�B

  Arguments:    None.

  Returns:      �������g��DS�_�E�����[�h�v���C�����f�[�^�B
 *---------------------------------------------------------------------------*/
const u8* CARDi_GetOwnSignature(void)
{
    return CARDiOwnSignature;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetOwnSignature

  Description:  �������g��DS�_�E�����[�h�v���C�����f�[�^��ݒ�B
                ��J�[�h�u�[�g���ɏ�ʃ��C�u��������Ăяo���B

  Arguments:    DS�_�E�����[�h�v���C�����f�[�^

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_SetOwnSignature(const void *signature)
{
    MI_CpuCopy8(signature, CARDiOwnSignature, CARD_ROM_DOWNLOAD_SIGNATURE_SIZE);
}

#if defined(SDK_ARM9)
/*---------------------------------------------------------------------------*
  Name:         CARD_GetCacheFlushFlag

  Description:  �L���b�V���������������I�ɍs�����ǂ����̐ݒ�t���O���擾�B

  Arguments:    icache            ���߃L���b�V���̎����������t���O���i�[����|�C���^
                                  NULL�ł���Ζ��������
                dcache            �f�[�^�L���b�V���̎����������t���O���i�[����|�C���^
                                  NULL�ł���Ζ��������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_GetCacheFlushFlag(BOOL *icache, BOOL *dcache)
{
    SDK_ASSERT(CARD_IsAvailable());
    if (icache)
    {
        *icache = CARDiEnableCacheInvalidationIC;
    }
    if (dcache)
    {
        *dcache = CARDiEnableCacheInvalidationDC;
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_SetCacheFlushFlag

  Description:  �L���b�V���������������I�ɍs�����ǂ�����ݒ�B
                �f�t�H���g�ł͖��߃L���b�V����FALSE�Ńf�[�^�L���b�V����TRUE�B

  Arguments:    icache            ���߃L���b�V���̎�����������L���ɂ���Ȃ�TRUE
                dcache            �f�[�^�L���b�V���̎������������L���ɂ���Ȃ�TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_SetCacheFlushFlag(BOOL icache, BOOL dcache)
{
    SDK_ASSERT(CARD_IsAvailable());
    CARDiEnableCacheInvalidationIC = icache;
    CARDiEnableCacheInvalidationDC = dcache;
}
#endif
