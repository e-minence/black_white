/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos - spi - nvram - DEMOlib
  File:     nvram.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include    "nvram.h"
#include    <nitro/hw/common/armArch.h>
#include    <nitro/pxi.h>
#include    <nitro/mi.h>
#include    <nitro/os.h>
#include    <nitro/spi/common/type.h>

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
enum
{
    NVRAM_STATE_BEFORE_INITIALIZE   =   0,
    NVRAM_STATE_INITIALIZING,
    NVRAM_STATE_INITIALIZED

};

typedef enum NVRAMLock
{
    NVRAM_LOCK_OFF  =   0,
    NVRAM_LOCK_READ,
    NVRAM_LOCK_WRITE,
    NVRAM_LOCK_VERIFY,
    NVRAM_LOCK_MAX

} NVRAMLock;

#ifdef  SDK_DEBUG
#define     NVRAMi_Warning      OS_TWarning
//#define NVRAM_DEBUG
#else
#define     NVRAMi_Warning(...) ((void)0)
#endif

#ifdef  NVRAM_DEBUG
#define     NVRAMi_Printf       OS_TPrintf
#else
#define     NVRAMi_Printf(...)  ((void)0)
#endif

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
typedef struct NVRAMWork
{
    NVRAMLock       lock;
    NVRAMCallback   callback;
    void*           callbackArg;
    OSAlarm         alarm;
    OSTick          tick;
    u32             seq;
    u32             arg[3];

} NVRAMWork;

/*---------------------------------------------------------------------------*
    �ϐ���`
 *---------------------------------------------------------------------------*/
static u32          nvramInitialized    =   NVRAM_STATE_BEFORE_INITIALIZE;
static NVRAMWork    nvramw;
static u32          nvramTemp[HW_CACHE_LINE_SIZE / sizeof(u32)] ATTRIBUTE_ALIGN(HW_CACHE_LINE_SIZE);

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/
static BOOL     NVRAMi_IsInitialized(void);
static void     NVRAMi_ReadCallback(u32 data, BOOL err);
static void     NVRAMi_WriteCallback(u32 data, BOOL err);
static void     NVRAMi_VerifyCallback(u32 data, BOOL err);
static void     NVRAMi_Callback(PXIFifoTag tag, u32 data, BOOL err);
static void     NVRAMi_SyncCallback(NVRAMResult result, void* arg);
static void     NVRAMi_AlarmHandler(void* arg);
static BOOL     NVRAMi_VerifyDataArray(const u8* data0, const u8* data1, u32 size);

static BOOL     NVRAMi_WriteEnable(void);
static BOOL     NVRAMi_WriteDisable(void);
static BOOL     NVRAMi_ReadStatusRegister(u8* pData);
static BOOL     NVRAMi_ReadDataBytes(u32 address, u32 size, u8* pData);
static BOOL     NVRAMi_PageWrite(u32 address, u16 size, const u8* pData);
static BOOL     NVRAMi_SoftwareReset(void);

/*---------------------------------------------------------------------------*
  Name:         NVRAM_Init
  Description:  NVRAM ���C�u����������������B
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NVRAM_Init(void)
{
    OSIntrMode  e   =   OS_DisableInterrupts();

    /* ��������Ԋm�F */
    if (nvramInitialized != NVRAM_STATE_BEFORE_INITIALIZE)
    {
        (void)OS_RestoreInterrupts(e);
        return;
    }
    nvramInitialized    =   NVRAM_STATE_INITIALIZING;
    (void)OS_RestoreInterrupts(e);

    /* PXI �����ݒ� */
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_NVRAM, PXI_PROC_ARM7))
    {
    }
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_NVRAM, NVRAMi_Callback);

    /* ���[�N�p�ϐ������� */
    MI_CpuFillFast(&nvramw, 0, sizeof(nvramw));

    /* �A���[�������� */
    if (OS_IsTickAvailable() == FALSE)
    {
        OS_InitTick();
    }
    if (OS_IsAlarmAvailable() == FALSE)
    {
        OS_InitAlarm();
    }
    OS_CreateAlarm(&(nvramw.alarm));

    /* �������������� */
    nvramInitialized    =   NVRAM_STATE_INITIALIZED;
}

/*---------------------------------------------------------------------------*
  Name:         NVRAM_ReadAsync
  Description:  NVRAM ����f�[�^��ǂݏo���B
                �񓯊��֐��Ȃ̂ŁA���ۂ̏������ʂ̓R�[���o�b�N���ɓn�����B
  Arguments:    address -   NVRAM ��̃I�t�Z�b�g���w��B
                size    -   �ǂݏo���T�C�Y���o�C�g�P�ʂŎw��B
                pData   -   �ǂݏo�����f�[�^���i�[����o�b�t�@���w��B
                            �l�� ARM7 �����ڏ����o���̂ŃL���b�V���ɒ��ӁB
                callback -  �񓯊������������ɌĂяo���R�[���o�b�N�֐����w��B
                arg     -   �R�[���o�b�N�֐��ɓn���p�����[�^���w��B
  Returns:      NVRAMResult -   �񓯊������J�n�����̌��ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NVRAMResult
NVRAM_ReadAsync(u32 address, u32 size, void* pData, NVRAMCallback callback, void* arg)
{
    OSIntrMode  e;

    SDK_NULL_ASSERT(pData);
    SDK_NULL_ASSERT(callback);

    /* ���C�u�����������m�F */
    if (NVRAMi_IsInitialized() == FALSE)
    {
#ifdef  SDK_DEBUG
        NVRAMi_Warning("%s: NVRAM library is not initialized.\n", __FUNCTION__);
#endif
        return NVRAM_RESULT_ILLEGAL_STATE;
    }

#ifdef  SDK_DEBUG
    /* �p�����[�^�m�F */
    if ((address >= SPI_NVRAM_ALL_SIZE) || ((address + size) > SPI_NVRAM_ALL_SIZE))
    {
        NVRAMi_Warning("%s: Source range (%u - %u) is over max size of NVRAM (%u).\n", __FUNCTION__, address, address + size, SPI_NVRAM_ALL_SIZE);
    }
    if ((((u32)pData % HW_CACHE_LINE_SIZE) != 0) || ((size % HW_CACHE_LINE_SIZE) != 0))
    {
        NVRAMi_Warning("%s: Destination buffer (%p - %p) is not aligned on %d bytes boundary.\n", __FUNCTION__, pData, (void*)((u32)pData + size), HW_CACHE_LINE_SIZE);
    }
#endif

    /* �r����Ԋm�F */
    e   =   OS_DisableInterrupts();
    if (nvramw.lock != NVRAM_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(e);
        return NVRAM_RESULT_BUSY;
    }
    nvramw.lock =   NVRAM_LOCK_READ;
    (void)OS_RestoreInterrupts(e);

    /* �f�[�^�ǂݏo���R�}���h���M */
    nvramw.callback     =   callback;
    nvramw.callbackArg  =   arg;
    nvramw.seq  =   0;
    e   =   OS_DisableInterrupts();
    if (TRUE == NVRAMi_ReadDataBytes(address, size, pData))
    {
        NVRAMi_Printf("[NVRAM] ReadDataBytes(%u, %u, %p) = TRUE\n", address, size, pData);
        (void)OS_RestoreInterrupts(e);
        return NVRAM_RESULT_SUCCESS;
    }
    else
    {
        NVRAMi_Printf("[NVRAM] ReadDataBytes(%u, %u, %p) = FALSE\n", address, size, pData);
        nvramw.lock =   NVRAM_LOCK_OFF;
        (void)OS_RestoreInterrupts(e);
        return NVRAM_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_ReadCallback
  Description:  �f�[�^�ǂݏo���V�[�P���X���� PXI ���荞�݃R�[���o�b�N�֐��B
  Arguments:    data    -   ���b�Z�[�W�f�[�^�B
                err     -   PXI �]���G���[�t���O
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
NVRAMi_ReadCallback(u32 data, BOOL err)
{
    NVRAMResult result      =   NVRAM_RESULT_FATAL_ERROR;
    u16         pxiResult   =   (u16)((data & SPI_PXI_RESULT_DATA_MASK) >> SPI_PXI_RESULT_DATA_SHIFT);

    NVRAMi_Printf("[NVRAM] Callback(%u, %s)\n", pxiResult, ((err == TRUE) ? "TRUE" : "FALSE"));
    /* PXI �ʐM�G���[���m�F */
    if ((err == FALSE) && (pxiResult == SPI_PXI_RESULT_SUCCESS))
    {
        result  =   NVRAM_RESULT_SUCCESS;
    }
    else
    {
        if (err == TRUE)
        {
            result  =   NVRAM_RESULT_SEND_ERROR;
        }
        else
        {
            switch (pxiResult)
            {
            case SPI_PXI_RESULT_INVALID_PARAMETER:
                result  =   NVRAM_RESULT_ILLEGAL_PARAMETER;
                break;
            case SPI_PXI_RESULT_ILLEGAL_STATUS:
                result  =   NVRAM_RESULT_ILLEGAL_STATE;
                break;
            case SPI_PXI_RESULT_EXCLUSIVE:
                result  =   NVRAM_RESULT_BUSY;
                break;
            }
        }
    }

    /* �r����Ԃ����Z�b�g */
    nvramw.lock =   NVRAM_LOCK_OFF;
    nvramw.seq  =   0;

    /* �R�[���o�b�N�֐��Ăяo�� */
    if (nvramw.callback != NULL)
    {
        NVRAMCallback   cb  =   nvramw.callback;
    
        nvramw.callback =   NULL;
        cb(result, nvramw.callbackArg);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NVRAM_Read
  Description:  NVRAM ����f�[�^��ǂݏo���B
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
  Arguments:    address -   NVRAM ��̃I�t�Z�b�g���w��B
                size    -   �ǂݏo���T�C�Y���o�C�g�P�ʂŎw��B
                pData   -   �ǂݏo�����f�[�^���i�[����o�b�t�@���w��B
                            �l�� ARM7 �����ڏ����o���̂ŃL���b�V���ɒ��ӁB
  Returns:      NVRAMResult -   �����I�ȏ������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NVRAMResult
NVRAM_Read(u32 address, u32 size, void* pData)
{
    NVRAMResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ���荞�݃n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
#ifdef  SDK_DEBUG
        NVRAMi_Warning("%s: Could not process in exception handler.\n", __FUNCTION__);
#endif
        return NVRAM_RESULT_ILLEGAL_STATE;
    }

    /* �����擾�p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   NVRAM_ReadAsync(address, size, pData, NVRAMi_SyncCallback, (void*)(&msgQ));
    if (result == NVRAM_RESULT_SUCCESS)
    {
        if (FALSE == OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK))
        {
            OS_Panic("%s: Failed to receive message.\n", __FUNCTION__);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         NVRAM_WriteAsync
  Description:  NVRAM �Ƀf�[�^���������ށB
                �񓯊��֐��Ȃ̂ŁA���ۂ̏������ʂ̓R�[���o�b�N���ɓn�����B
  Arguments:    address -   NVRAM ��̃I�t�Z�b�g���w��B
                size    -   �������ރT�C�Y���o�C�g�P�ʂŎw��B
                pData   -   �������ރf�[�^���i�[����Ă���o�b�t�@���w��B
                            �l�� ARM7 �����ړǂݏo���̂ŃL���b�V���ɒ��ӁB
                callback -  �񓯊������������ɌĂяo���R�[���o�b�N�֐����w��B
                arg     -   �R�[���o�b�N�֐��ɓn���p�����[�^���w��B
  Returns:      NVRAMResult -   �񓯊������J�n�����̌��ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NVRAMResult
NVRAM_WriteAsync(u32 address, u32 size, const void* pData, NVRAMCallback callback, void* arg)
{
    OSIntrMode  e;

    SDK_NULL_ASSERT(pData);
    SDK_NULL_ASSERT(callback);

    /* ���C�u�����������m�F */
    if (NVRAMi_IsInitialized() == FALSE)
    {
#ifdef  SDK_DEBUG
        NVRAMi_Warning("%s: NVRAM library is not initialized.\n", __FUNCTION__);
#endif
        return NVRAM_RESULT_ILLEGAL_STATE;
    }

#ifdef  SDK_DEBUG
    /* �p�����[�^�m�F */
    if ((address >= SPI_NVRAM_ALL_SIZE) || ((address + size) > SPI_NVRAM_ALL_SIZE))
    {
        NVRAMi_Warning("%s: Destination range (%u - %u) is over max size of NVRAM (%u).\n", __FUNCTION__, address, address + size, SPI_NVRAM_ALL_SIZE);
    }
#if 0
    // DC_StoreRange ������������s���Ă���΁A�L���b�V�����C���ɉ����Ă���K�v�͂Ȃ�
    if ((((u32)pData % HW_CACHE_LINE_SIZE) != 0) || ((size % HW_CACHE_LINE_SIZE) != 0))
    {
        NVRAMi_Warning("%s: Source buffer (%p - %p) is not aligned on %d bytes boundary.\n", __FUNCTION__, pData, (void*)((u32)pData + size), HW_CACHE_LINE_SIZE)
    }
#endif
#endif

    /* �r����Ԋm�F */
    e   =   OS_DisableInterrupts();
    if (nvramw.lock != NVRAM_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(e);
        return NVRAM_RESULT_BUSY;
    }
    nvramw.lock =   NVRAM_LOCK_WRITE;
    (void)OS_RestoreInterrupts(e);

    /* �X�e�[�^�X���W�X�^�ǂݏo���R�}���h���s */
    nvramw.callback     =   callback;
    nvramw.callbackArg  =   arg;
    nvramw.arg[0]   =   address;
    nvramw.arg[1]   =   size;
    nvramw.arg[2]   =   (u32)pData;
    nvramw.seq  =   0;
    e   =   OS_DisableInterrupts();
    if (TRUE == NVRAMi_WriteEnable())
    {
        NVRAMi_Printf("[NVRAM] WriteEnable() = TRUE\n");
        (void)OS_RestoreInterrupts(e);
        return NVRAM_RESULT_SUCCESS;
    }
    else
    {
        NVRAMi_Printf("[NVRAM] WriteEnable() = FALSE\n");
        nvramw.lock =   NVRAM_LOCK_OFF;
        (void)OS_RestoreInterrupts(e);
        return NVRAM_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_WriteCallback
  Description:  �f�[�^�������݃V�[�P���X���� PXI ���荞�݃R�[���o�b�N�֐��B
  Arguments:    data    -   ���b�Z�[�W�f�[�^�B
                err     -   PXI �]���G���[�t���O
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
NVRAMi_WriteCallback(u32 data, BOOL err)
{
    NVRAMResult     result      =   NVRAM_RESULT_FATAL_ERROR;
    u16             pxiResult   =   (u16)((data & SPI_PXI_RESULT_DATA_MASK) >> SPI_PXI_RESULT_DATA_SHIFT);
    u16             tempSize    =   (u16)(SPI_NVRAM_PAGE_SIZE - (nvramw.arg[0] % SPI_NVRAM_PAGE_SIZE));

    tempSize    =   (u16)((tempSize > nvramw.arg[1]) ? nvramw.arg[1] : tempSize);

    NVRAMi_Printf("[NVRAM] Callback(%u, %s)\n", pxiResult, ((err == TRUE) ? "TRUE" : "FALSE"));
    switch (nvramw.seq)
    {
    case 0: /* ���C�g�G�l�[�u������ */
        if ((err == FALSE) && (pxiResult == SPI_PXI_RESULT_SUCCESS))
        {
            /* �y�[�W�������݃R�}���h�𔭍s */
            if (TRUE == NVRAMi_PageWrite(nvramw.arg[0], tempSize, (const u8*)nvramw.arg[2]))
            {
                NVRAMi_Printf("[NVRAM] PageWrite(%u, %u, %p) = TRUE\n", nvramw.arg[0], tempSize, nvramw.arg[2]);
                nvramw.seq  =   1;
                return;
            }
            NVRAMi_Printf("[NVRAM] PageWrite(%u, %u, %p) = FALSE\n", nvramw.arg[0], tempSize, nvramw.arg[2]);
            result  =   NVRAM_RESULT_SEND_ERROR;
        }
        else
        {
            if (err == TRUE)
            {
                result  =   NVRAM_RESULT_SEND_ERROR;
            }
            else if (pxiResult == SPI_PXI_RESULT_EXCLUSIVE)
            {
                result  =   NVRAM_RESULT_BUSY;
            }
        }
        break;
    
    case 1: /* ���C�g���� */
        if ((err == FALSE) && (pxiResult == SPI_PXI_RESULT_SUCCESS))
        {
            /* �������݂ɐ����������|�C���^��i�߂� */
            nvramw.arg[0]   +=  tempSize;
            nvramw.arg[1]   -=  tempSize;
            nvramw.arg[2]   +=  tempSize;
            /* ���C�g�����҂��^�C���A�E�g�p�Ɏ��Ԍv���J�n */
            nvramw.tick     =   OS_GetTick();
            /* �X�e�[�^�X���W�X�^�ǂݏo�� */
            DC_InvalidateRange(nvramTemp, HW_CACHE_LINE_SIZE);
            if (TRUE == NVRAMi_ReadStatusRegister((u8*)nvramTemp))
            {
                NVRAMi_Printf("[NVRAM] ReadStatusRegister(%p) = TRUE\n", nvramTemp);
                nvramw.seq  =   2;
                return;
            }
            NVRAMi_Printf("[NVRAM] ReadStatusRegister(%p) = FALSE\n", nvramTemp);
            /* �X�e�[�^�X���W�X�^�ǂݏo�����ʃR�[���o�b�N���U������A���[�����N�� */
            nvramw.seq  =   2;
            *((u8*)nvramTemp)   =   NVRAM_STATUS_REGISTER_WIP;
            DC_StoreRange(nvramTemp, HW_CACHE_LINE_SIZE);
            OS_SetAlarm(&(nvramw.alarm), NVRAM_RETRY_SPAN, NVRAMi_AlarmHandler, (void*)(SPI_PXI_RESULT_SUCCESS << SPI_PXI_RESULT_DATA_SHIFT));
            return;
        }
        else
        {
            if (err == TRUE)
            {
                result  =   NVRAM_RESULT_SEND_ERROR;
            }
            else
            {
                switch(pxiResult)
                {
                case SPI_PXI_RESULT_INVALID_PARAMETER:
                    result  =   NVRAM_RESULT_ILLEGAL_PARAMETER;
                    break;
                case SPI_PXI_RESULT_ILLEGAL_STATUS:
                    result  =   NVRAM_RESULT_ILLEGAL_STATE;
                    break;
                case SPI_PXI_RESULT_EXCLUSIVE:
                    /* �y�[�W�������݃R�}���h���Ĕ��s */
                    if (TRUE == NVRAMi_PageWrite(nvramw.arg[0], tempSize, (const u8*)nvramw.arg[2]))
                    {
                        NVRAMi_Printf("[NVRAM] PageWrite(%u, %u, %p) = TRUE\n", nvramw.arg[0], tempSize, nvramw.arg[2]);
                        return;
                    }
                    NVRAMi_Printf("[NVRAM] PageWrite(%u, %u, %p) = FALSE\n", nvramw.arg[0], tempSize, nvramw.arg[2]);
                    result  =   NVRAM_RESULT_SEND_ERROR;
                    break;
                }
            }
        }
        break;
    
    case 2: /* �X�e�[�^�X���W�X�^�ǂݏo������ */
        if ((err == FALSE) && (pxiResult == SPI_PXI_RESULT_SUCCESS))
        {
            if ((*((u8*)nvramTemp) & NVRAM_STATUS_REGISTER_ERSER) != 0)
            {
                /* �\�t�g���Z�b�g�R�}���h���s */
                if (TRUE == NVRAMi_SoftwareReset())
                {
                    NVRAMi_Printf("[NVRAM] SoftwareReset() = TRUE\n");
                    nvramw.seq  =   4;
                    return;
                }
                NVRAMi_Printf("[NVRAM] SoftwareReset() = FALSE\n");
                /* �\�t�g���Z�b�g���ʃR�[���o�b�N���U������A���[���N�� */
                nvramw.seq  =   4;
                OS_SetAlarm(&(nvramw.alarm), NVRAM_RETRY_SPAN, NVRAMi_AlarmHandler, (void*)(SPI_PXI_RESULT_EXCLUSIVE << SPI_PXI_RESULT_DATA_SHIFT));
                return;
            }
            else if ((*((u8*)nvramTemp) & NVRAM_STATUS_REGISTER_WIP) == 0)
            {
                if (tempSize > 0)
                {
                    /* ���̃y�[�W�������݃V�[�P���X�Ɉڍs */
                    if (TRUE == NVRAMi_WriteEnable())
                    {
                        NVRAMi_Printf("[NVRAM] WriteEnable() = TRUE\n");
                        nvramw.seq  =   0;
                        return;
                    }
                    NVRAMi_Printf("[NVRAM] WriteEnable() = FALSE\n");
                    result  =   NVRAM_RESULT_SEND_ERROR;
                    break;
                }
                /* ���C�g�f�B�Z�[�u���R�}���h���s */
                if (TRUE == NVRAMi_WriteDisable())
                {
                    NVRAMi_Printf("[NVRAM] WriteDisable() = TRUE\n");
                    nvramw.seq  =   3;
                    return;
                }
                NVRAMi_Printf("[NVRAM] WriteDisable() = FALSE\n");
                /* ���C�g�f�B�Z�[�u�����ʃR�[���o�b�N���U������A���[�����N�� */
                nvramw.seq  =   3;
                OS_SetAlarm(&(nvramw.alarm), NVRAM_RETRY_SPAN, NVRAMi_AlarmHandler, (void*)(SPI_PXI_RESULT_EXCLUSIVE << SPI_PXI_RESULT_DATA_SHIFT));
                return;
            }
        }
        else
        {
            if ((err == FALSE) && (pxiResult != SPI_PXI_RESULT_EXCLUSIVE))
            {
                break;
            }
        }
        /* �X�e�[�^�X���W�X�^�ǂݏo���R�}���h���Ĕ��s */
        DC_InvalidateRange(nvramTemp, HW_CACHE_LINE_SIZE);
        if (TRUE == NVRAMi_ReadStatusRegister((u8*)nvramTemp))
        {
            NVRAMi_Printf("[NVRAM] ReadStatusRegister(%p) = TRUE\n", nvramTemp);
            return;
        }
        NVRAMi_Printf("[NVRAM] ReadStatusRegister(%p) = FALSE\n", nvramTemp);
        /* �X�e�[�^�X���W�X�^�ǂݏo�����ʃR�[���o�b�N���U������A���[�����N�� */
        nvramw.seq  =   2;
        *((u8*)nvramTemp)   =   NVRAM_STATUS_REGISTER_WIP;
        DC_StoreRange(nvramTemp, HW_CACHE_LINE_SIZE);
        OS_SetAlarm(&(nvramw.alarm), NVRAM_RETRY_SPAN, NVRAMi_AlarmHandler, (void*)(SPI_PXI_RESULT_SUCCESS << SPI_PXI_RESULT_DATA_SHIFT));
        return;
    
    case 3: /* ���C�g�f�B�Z�[�u������ */
        if ((err == FALSE) && (pxiResult == SPI_PXI_RESULT_SUCCESS))
        {
            result  =   NVRAM_RESULT_SUCCESS;
        }
        else
        {
            if ((err == FALSE) && (pxiResult != SPI_PXI_RESULT_EXCLUSIVE))
            {
                break;
            }
            /* ���C�g�f�B�Z�[�u���R�}���h�Ĕ��s */
            if (TRUE == NVRAMi_WriteDisable())
            {
                NVRAMi_Printf("[NVRAM] WriteDisable() = TRUE\n");
                return;
            }
            NVRAMi_Printf("[NVRAM] WriteDisable() = FALSE\n");
            /* ���C�g�f�B�Z�[�u�����ʃR�[���o�b�N���U������A���[���N�� */
            nvramw.seq  =   3;
            OS_SetAlarm(&(nvramw.alarm), NVRAM_RETRY_SPAN, NVRAMi_AlarmHandler, (void*)(SPI_PXI_RESULT_EXCLUSIVE << SPI_PXI_RESULT_DATA_SHIFT));
            return;
        }
        break;
    
    case 4: /* �\�t�g���Z�b�g���� */
        if ((err == FALSE) && (pxiResult == SPI_PXI_RESULT_SUCCESS))
        {
            result  =   NVRAM_RESULT_WRITE_ERROR;
        }
        else
        {
            if ((err == FALSE) && (pxiResult != SPI_PXI_RESULT_EXCLUSIVE))
            {
                break;
            }
            /* �\�t�g���Z�b�g�R�}���h���Ĕ��s */
            if (TRUE == NVRAMi_SoftwareReset())
            {
                NVRAMi_Printf("[NVRAM] SoftwareReset() = TRUE\n");
                return;
            }
            NVRAMi_Printf("[NVRAM] SoftwareReset() = FALSE\n");
            /* �\�t�g���Z�b�g���ʃR�[���o�b�N���U������A���[���N�� */
            nvramw.seq  =   4;
            OS_SetAlarm(&(nvramw.alarm), NVRAM_RETRY_SPAN, NVRAMi_AlarmHandler, (void*)(SPI_PXI_RESULT_EXCLUSIVE << SPI_PXI_RESULT_DATA_SHIFT));
            return;
        }
        break;
    }

    /* �������ʂ��R�[���o�b�N */
    nvramw.lock =   NVRAM_LOCK_OFF;
    nvramw.seq  =   0;
    if (nvramw.callback != NULL)
    {
        NVRAMCallback   cb  =   nvramw.callback;
    
        nvramw.callback =   NULL;
        cb(result, nvramw.callbackArg);
    }
}


/*---------------------------------------------------------------------------*
  Name:         NVRAM_Write
  Description:  NVRAM �Ƀf�[�^���������ށB
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
  Arguments:    address -   NVRAM ��̃I�t�Z�b�g���w��B
                size    -   �������ރT�C�Y���o�C�g�P�ʂŎw��B
                pData   -   �������ރf�[�^���i�[����Ă���o�b�t�@���w��B
                            �l�� ARM7 �����ړǂݏo���̂ŃL���b�V���ɒ��ӁB
  Returns:      NVRAMResult -   �����I�ȏ������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NVRAMResult
NVRAM_Write(u32 address, u32 size, const void* pData)
{
    NVRAMResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ���荞�݃n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
#ifdef  SDK_DEBUG
        NVRAMi_Warning("%s: Could not process in exception handler.\n", __FUNCTION__);
#endif
        return NVRAM_RESULT_ILLEGAL_STATE;
    }

    /* �����擾�p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   NVRAM_WriteAsync(address, size, pData, NVRAMi_SyncCallback, (void*)(&msgQ));
    if (result == NVRAM_RESULT_SUCCESS)
    {
        if (FALSE == OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK))
        {
            OS_Panic("%s: Failed to receive message.\n", __FUNCTION__);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         NVRAM_VerifyAsync
  Description:  NVRAM �̃f�[�^����v�m�F����B
                �񓯊��֐��Ȃ̂ŁA���ۂ̏������ʂ̓R�[���o�b�N���ɓn�����B
  Arguments:    address -   NVRAM ��̃I�t�Z�b�g���w��B
                size    -   ��r����f�[�^�T�C�Y���o�C�g�P�ʂŎw��B
                pData   -   ��r����f�[�^���i�[����Ă���o�b�t�@���w��B
                callback -  �񓯊������������ɌĂяo���R�[���o�b�N�֐����w��B
                arg     -   �R�[���o�b�N�֐��ɓn���p�����[�^���w��B
  Returns:      NVRAMResult -   �񓯊������J�n�����̌��ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NVRAMResult
NVRAM_VerifyAsync(u32 address, u32 size, const void* pData, NVRAMCallback callback, void* arg)
{
    OSIntrMode  e;

    SDK_NULL_ASSERT(pData);
    SDK_NULL_ASSERT(callback);

    /* ���C�u�����������m�F */
    if (NVRAMi_IsInitialized() == FALSE)
    {
#ifdef  SDK_DEBUG
        NVRAMi_Warning("%s: NVRAM library is not initialized.\n", __FUNCTION__);
#endif
        return NVRAM_RESULT_ILLEGAL_STATE;
    }

#ifdef  SDK_DEBUG
    /* �p�����[�^�m�F */
    if ((address >= SPI_NVRAM_ALL_SIZE) || ((address + size) > SPI_NVRAM_ALL_SIZE))
    {
        NVRAMi_Warning("%s: Target range (%u - %u) is over max size of NVRAM (%u).\n", __FUNCTION__, address, address + size, SPI_NVRAM_ALL_SIZE);
    }
#endif

    /* �r����Ԋm�F */
    e   =   OS_DisableInterrupts();
    if (nvramw.lock != NVRAM_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(e);
        return NVRAM_RESULT_BUSY;
    }
    nvramw.lock =   NVRAM_LOCK_VERIFY;
    (void)OS_RestoreInterrupts(e);

    /* �f�[�^�ǂݏo���R�}���h���s */
    nvramw.callback     =   callback;
    nvramw.callbackArg  =   arg;
    nvramw.arg[0]   =   address;
    nvramw.arg[1]   =   size;
    nvramw.arg[2]   =   (u32)pData;
    nvramw.seq  =   0;
    DC_InvalidateRange(nvramTemp, HW_CACHE_LINE_SIZE);
    e   =   OS_DisableInterrupts();
    if (TRUE == NVRAMi_ReadDataBytes(address, ((size > HW_CACHE_LINE_SIZE) ? HW_CACHE_LINE_SIZE : size), (void*)nvramTemp))
    {
        NVRAMi_Printf("[NVRAM] ReadDataBytes(%u, %u, %p) = TRUE\n", address, ((size > HW_CACHE_LINE_SIZE) ? HW_CACHE_LINE_SIZE : size), nvramTemp);
        (void)OS_RestoreInterrupts(e);
        return NVRAM_RESULT_SUCCESS;
    }
    else
    {
        NVRAMi_Printf("[NVRAM] ReadDataBytes(%u, %u, %p) = FALSE\n", address, ((size > HW_CACHE_LINE_SIZE) ? HW_CACHE_LINE_SIZE : size), nvramTemp);
        nvramw.lock =   NVRAM_LOCK_OFF;
        (void)OS_RestoreInterrupts(e);
        return NVRAM_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_VerifyCallback
  Description:  �f�[�^��v�m�F�V�[�P���X���� PXI ���荞�݃R�[���o�b�N�֐��B
  Arguments:    data    -   ���b�Z�[�W�f�[�^�B
                err     -   PXI �]���G���[�t���O
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
NVRAMi_VerifyCallback(u32 data, BOOL err)
{
    NVRAMResult     result      =   NVRAM_RESULT_FATAL_ERROR;
    u16             pxiResult   =   (u16)((data & SPI_PXI_RESULT_DATA_MASK) >> SPI_PXI_RESULT_DATA_SHIFT);
    u16             tempSize    =   (u16)((nvramw.arg[1] > HW_CACHE_LINE_SIZE) ? HW_CACHE_LINE_SIZE : nvramw.arg[1]);

    NVRAMi_Printf("[NVRAM] Callback(%u, %s)\n", pxiResult, ((err == TRUE) ? "TRUE" : "FALSE"));
    if ((err == FALSE) && (pxiResult == SPI_PXI_RESULT_SUCCESS))
    {
        if (FALSE == NVRAMi_VerifyDataArray((const u8*)nvramTemp, (const u8*)(nvramw.arg[2]), (u32)tempSize))
        {
            result  =   NVRAM_RESULT_VERIFY_ERROR;
        }
        else
        {
            /* �ǂݏo���ɐ����������|�C���^��i�߂� */
            nvramw.arg[0]   +=  tempSize;
            nvramw.arg[1]   -=  tempSize;
            nvramw.arg[2]   +=  tempSize;
            /* ���̓ǂݏo���V�[�P���X�Ɉڍs */
            if (nvramw.arg[1] > 0)
            {
                DC_InvalidateRange(nvramTemp, HW_CACHE_LINE_SIZE);
                if (TRUE == NVRAMi_ReadDataBytes(nvramw.arg[0], ((nvramw.arg[1] > HW_CACHE_LINE_SIZE) ? HW_CACHE_LINE_SIZE : nvramw.arg[1]), (void*)nvramTemp))
                {
                    NVRAMi_Printf("[NVRAM] ReadDataBytes(%u, %u, %p) = TRUE\n", nvramw.arg[0], ((nvramw.arg[1] > HW_CACHE_LINE_SIZE) ? HW_CACHE_LINE_SIZE : nvramw.arg[1]), nvramTemp);
                    return;
                }
                NVRAMi_Printf("[NVRAM] ReadDataBytes(%u, %u, %p) = FALSE\n", nvramw.arg[0], ((nvramw.arg[1] > HW_CACHE_LINE_SIZE) ? HW_CACHE_LINE_SIZE : nvramw.arg[1]), nvramTemp);
                result  =   NVRAM_RESULT_SEND_ERROR;
            }
            else
            {
                /* �S��v�m�F���� */
                result  =   NVRAM_RESULT_SUCCESS;
            }
        }
    }
    else
    {
        if (err == TRUE)
        {
            result  =   NVRAM_RESULT_SEND_ERROR;
        }
        else
        {
            switch(pxiResult)
            {
            case SPI_PXI_RESULT_INVALID_PARAMETER:
                result  =   NVRAM_RESULT_ILLEGAL_PARAMETER;
                break;
            case SPI_PXI_RESULT_ILLEGAL_STATUS:
                result  =   NVRAM_RESULT_ILLEGAL_STATE;
                break;
            case SPI_PXI_RESULT_EXCLUSIVE:
                /* �f�[�^�ǂݏo���R�}���h���Ĕ��s */
                DC_InvalidateRange(nvramTemp, HW_CACHE_LINE_SIZE);
                if (TRUE == NVRAMi_ReadDataBytes(nvramw.arg[0], tempSize, (void*)nvramTemp))
                {
                    NVRAMi_Printf("[NVRAM] ReadDataBytes(%u, %u, %p) = TRUE\n", nvramw.arg[0], tempSize, nvramTemp);
                    return;
                }
                NVRAMi_Printf("[NVRAM] ReadDataBytes(%u, %u, %p) = FALSE\n", nvramw.arg[0], tempSize, nvramTemp);
                result  =   NVRAM_RESULT_SEND_ERROR;
                break;
            }
        }
    }

    /* �������ʂ��R�[���o�b�N */
    nvramw.lock =   NVRAM_LOCK_OFF;
    nvramw.seq  =   0;
    if (nvramw.callback != NULL)
    {
        NVRAMCallback   cb  =   nvramw.callback;
    
        nvramw.callback =   NULL;
        cb(result, nvramw.callbackArg);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NVRAM_Verify
  Description:  NVRAM �̃f�[�^����v�m�F����B
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
  Arguments:    address -   NVRAM ��̃I�t�Z�b�g���w��B
                size    -   ��r����f�[�^�T�C�Y���o�C�g�P�ʂŎw��B
                pData   -   ��r����f�[�^���i�[����Ă���o�b�t�@���w��B
  Returns:      NVRAMResult -   �����I�ȏ������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NVRAMResult
NVRAM_Verify(u32 address, u32 size, const void* pData)
{
    NVRAMResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ���荞�݃n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
#ifdef  SDK_DEBUG
        NVRAMi_Warning("%s: Could not process in exception handler.\n", __FUNCTION__);
#endif
        return NVRAM_RESULT_ILLEGAL_STATE;
    }

    /* �����擾�p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   NVRAM_VerifyAsync(address, size, pData, NVRAMi_SyncCallback, (void*)(&msgQ));
    if (result == NVRAM_RESULT_SUCCESS)
    {
        if (FALSE == OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK))
        {
            OS_Panic("%s: Failed to receive message.\n", __FUNCTION__);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_IsInitialized
  Description:  NVRAM ���C�u�������������ς݂��ǂ�����������B
  Arguments:    None.
  Returns:      BOOL    -   �������ς݂̏ꍇ�� TRUE ��Ԃ��B
                            �������O�A���������̏ꍇ�ɂ� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
NVRAMi_IsInitialized(void)
{
    return (BOOL)(nvramInitialized == NVRAM_STATE_INITIALIZED);
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_Callback
  Description:  PXI ���荞�݃R�[���o�b�N�֐��B
  Arguments:    tag     -   PXI ���b�Z�[�W��ʁB
                data    -   ���b�Z�[�W�f�[�^�B
                err     -   PXI �]���G���[�t���O
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
NVRAMi_Callback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(tag)

    switch (nvramw.lock)
    {
    case NVRAM_LOCK_READ:
        NVRAMi_ReadCallback(data, err);
        break;
    case NVRAM_LOCK_WRITE:
        NVRAMi_WriteCallback(data, err);
        break;
    case NVRAM_LOCK_VERIFY:
        NVRAMi_VerifyCallback(data, err);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_SyncCallback
  Description:  �����֐����p�̃R�[���o�b�N�֐��B�x�~���Ă���X���b�h�ɔ񓯊�
                �������ʂ𑗂邱�Ƃœ���\��Ԃɕ��A������B
  Arguments:    result  -   NVRAM ����̔񓯊��������ʂ��n�����B
                arg     -   �����֐��Ăяo���X���b�h����M�ҋ@���Ă��郁�b�Z�[�W
                            �L���[�ւ̃|�C���^���n�����B
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
NVRAMi_SyncCallback(NVRAMResult result, void* arg)
{
    SDK_NULL_ASSERT(arg);

    if (FALSE == OS_SendMessage((OSMessageQueue*)arg, (OSMessage)result, OS_MESSAGE_NOBLOCK))
    {
        OS_Panic("%s: Failed to send message.\n", __FUNCTION__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_AlarmHandler
  Description:  PXI �R�[���o�b�N���U������A���[���n���h���B
  Arguments:    arg     -   PXI �R�[���o�b�N�� data �Ƃ��Ďg�p�����B
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
NVRAMi_AlarmHandler(void* arg)
{
    NVRAMi_Callback(PXI_FIFO_TAG_NVRAM, (u32)arg, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_VerifyDataArray
  Description:  �Q�̃f�[�^��̈�v�m�F���s���B
  Arguments:    data0   -   ��r����f�[�^��ւ̃|�C���^�B
                data1   -   ��r����f�[�^��ւ̃|�C���^�B
                size    -   ��r����T�C�Y���o�C�g�P�ʂŎw��B
  Returns:      BOOL    -   ��v���m�F���ꂽ�ꍇ�� TRUE ��Ԃ��B
                            ��v���Ă��Ȃ��ꍇ�ɂ� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
NVRAMi_VerifyDataArray(const u8* data0, const u8* data1, u32 size)
{
    u32     i;

    for (i = 0; i < size; i ++)
    {
        if (data0[i] != data1[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
    INSTRUCTIONS
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         NVRAMi_WriteEnable
  Description:  NVRAM�Ɂu�������݋��v���߂𔭍s����B
  Arguments:    None.
  Returns:      BOOL    -   ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                            ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
NVRAMi_WriteEnable(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_WREN << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_WriteDisable
  Description:  NVRAM�Ɂu�������݋֎~�v���߂𔭍s����B
  Arguments:    None.
  Returns:      BOOL    -   ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                            ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
NVRAMi_WriteDisable(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_WRDI << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_ReadStatusRegister
  Description:  NVRAM�Ɂu�X�e�[�^�X���W�X�^�ǂݏo���v���߂𔭍s����B
  Arguments:    pData   -   �ǂݏo�����l���i�[����ϐ��ւ̃|�C���^�B
                            �l��ARM7�����ڏ����o���̂ŃL���b�V���ɒ��ӁB
  Returns:      BOOL    -   ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                            ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
NVRAMi_ReadStatusRegister(u8* pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_RDSR << 8) | ((u32)pData >> 24), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (((u32)pData >> 8) & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (2 << SPI_PXI_INDEX_SHIFT) | (((u32)pData << 8) & 0x0000ff00), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_ReadDataBytes
  Description:  NVRAM�Ɂu�ǂݏo���v���߂𔭍s����B
  Arguments:    address -   NVRAM��̓ǂݏo���J�n�A�h���X�B24bit�̂ݗL���B
                size    -   �A�����ēǂݏo���o�C�g���B
                pData   -   �ǂݏo�����l���i�[����z��B
                            �l��ARM7�����ڏ����o���̂ŃL���b�V���ɒ��ӁB
  Returns:      BOOL    -   ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                            ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
NVRAMi_ReadDataBytes(u32 address, u32 size, u8* pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_READ << 8) |
                               ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (2 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [5]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (5 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_PageWrite
  Description:  NVRAM�Ɂu�y�[�W�������݁v���߂𔭍s����B
                NVRAM�̓����ł́A�u�y�[�W�����v�Ɓu�y�[�W��������(�����t)�v��
                �A�����čs����B
  Arguments:    address -   NVRAM��̏������݊J�n�A�h���X�B24bit�̂ݗL���B
                size    -   �A�����ď������ރo�C�g���B
                            address + size ���y�[�W���E(256�o�C�g)���z�����
                            �z�������̃f�[�^�͖��������B
                pData   -   �������ޒl���i�[����Ă���z��B
                            ARM7�����ړǂݏo���̂ŁA�L���b�V�����烁�������̂�
                            �m���ɏ����o���Ă����K�v������B
  Returns:      BOOL    -   ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                            ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
NVRAMi_PageWrite(u32 address, u16 size, const u8* pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_PW << 8) | ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM, (2 << SPI_PXI_INDEX_SHIFT) | (u32)size, 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NVRAMi_SoftwareReset
  Description:  NVRAM�Ɂu�\�t�g�E�F�A���Z�b�g�v���߂𔭍s����B
                �f�o�C�X��LE25FW203T�̏ꍇ�ɗL���B
                �f�o�C�X��M45PE40�̏ꍇ�͖��������B
  Arguments:    None.
  Returns:      BOOL    -   ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                            ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
NVRAMi_SoftwareReset(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_SR << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}
