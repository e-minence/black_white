/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos - spi - nvram - DEMOlib
  File:     nvram.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-12-17#$
  $Rev: 175 $
  $Author: seiki_masashi $
 *---------------------------------------------------------------------------*/
#ifndef TWL_DEMOS_SPI_NVRAM_DEMOLIB_NVRAM_H_
#define TWL_DEMOS_SPI_NVRAM_DEMOLIB_NVRAM_H_
#ifdef  __cplusplus
extern  "C" {
#endif
/*---------------------------------------------------------------------------*/

#include    <twl/types.h>

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
typedef enum NVRAMResult
{
    NVRAM_RESULT_SUCCESS    =   0,  /* ���� */
    NVRAM_RESULT_BUSY,              /* �r�����䒆 (���g���C�L��) */
    NVRAM_RESULT_ILLEGAL_PARAMETER, /* �s���ȃp�����[�^ */
    NVRAM_RESULT_SEND_ERROR,        /* PXI�ɂ��R�}���h���M�Ɏ��s (���g���C�L��) */
    NVRAM_RESULT_ILLEGAL_STATE,     /* ��ԕs�� */
        /* ���C�u������������/��O�n���h������̓����֐��Ăяo��/PXI���ڑ���ɂ��Write���� */
    NVRAM_RESULT_WRITE_ERROR,       /* �������ݒ��Ƀf�o�C�X�G���[ (���g���C�L��) */
    NVRAM_RESULT_VERIFY_ERROR,      /* �x���t�@�C�`�F�b�N�ŕs��v */
    NVRAM_RESULT_FATAL_ERROR,       /* ��L�ȊO�̃G���[ */
        /* PXI���ڑ���ɂ���ԊǗ��̔j�]/�Ǘ��ϐ��j�󓙁A�����s�\�ȃG���[ */
    NVRAM_RESULT_MAX

} NVRAMResult;

#define     NVRAM_STATUS_REGISTER_WIP       0x01
#define     NVRAM_STATUS_REGISTER_WEL       0x02
#define     NVRAM_STATUS_REGISTER_ERSER     0x20

#define     NVRAM_RETRY_SPAN                (OS_MilliSecondsToTicks(1))

/*---------------------------------------------------------------------------*
    �^��`
 *---------------------------------------------------------------------------*/
typedef void (*NVRAMCallback) (NVRAMResult result, void* arg);

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/
static void        NVRAM_Init(void);
static NVRAMResult NVRAM_ReadAsync(u32 address, u32 size, void* pData, NVRAMCallback callback, void* arg);
static NVRAMResult NVRAM_Read(u32 address, u32 size, void* pData);
static NVRAMResult NVRAM_WriteAsync(u32 address, u32 size, const void* pData, NVRAMCallback callback, void* arg);
static NVRAMResult NVRAM_Write(u32 address, u32 size, const void* pData);
static NVRAMResult NVRAM_VerifyAsync(u32 address, u32 size, const void* pData, NVRAMCallback callback, void* arg);
static NVRAMResult NVRAM_Verify(u32 address, u32 size, const void* pData);


/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}   /* extern "C" */
#endif
#endif  /* TWL_DEMOS_SPI_NVRAM_DEMOLIB_NVRAM_H_ */
