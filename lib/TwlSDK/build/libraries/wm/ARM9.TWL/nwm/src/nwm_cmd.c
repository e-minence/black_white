/*---------------------------------------------------------------------------*
  Project:  TwlSDK - NWM - libraries
  File:     nwm_cmd.c

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

#include <twl.h>

#include "nwm_common_private.h"
#include "nwm_arm9_private.h"

static u32 *NwmGetCommandBuffer4Arm7(void);

/*---------------------------------------------------------------------------*
  Name:         NWMi_SetCallbackTable

  Description:  �e�񓯊��֐��ɑΉ������R�[���o�b�N�֐���o�^����B

  Arguments:    id          -   �񓯊��֐���API ID�B
                callback    -   �o�^����R�[���o�b�N�֐��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NWMi_SetCallbackTable(NWMApiid id, NWMCallbackFunc callback)
{
    NWMArm9Buf *sys = NWMi_GetSystemWork();

    SDK_NULL_ASSERT(sys);

    sys->callbackTable[id] = callback;
}


/*---------------------------------------------------------------------------*
  Name:         NWMi_SetReceiveCallbackTable

  Description:  �f�[�^�t���[����M�p�R�[���o�b�N�֐���o�^����B

  Arguments:    id          -   �񓯊��֐���API ID�B
                callback    -   �o�^����R�[���o�b�N�֐��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NWMi_SetReceiveCallbackTable(NWMFramePort port, NWMCallbackFunc callback)
{
    NWMArm9Buf *sys = NWMi_GetSystemWork();

    SDK_NULL_ASSERT(sys);

    sys->recvCallbackTable[port] = callback;
}

/*---------------------------------------------------------------------------*
  Name:         NWMi_SendCommand

  Description:  FIFO�o�R�Ń��N�G�X�g��ARM7�ɑ��M����B
                u32�^�̃p�����[�^�������������R�}���h�̏ꍇ�́A
                �p�����[�^��񋓂��Ďw�肷��B

  Arguments:    id          -   ���N�G�X�g�ɑΉ�����API ID�B
                paramNum    -   ���z�����̐��B
                ...         -   ���z�����B

  Returns:      int -   NWM_RETCODE_*�^�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NWMRetCode NWMi_SendCommand(NWMApiid id, u16 paramNum, ...)
{
    va_list vlist;
    s32     i;
    int     result;
    u32    *tmpAddr;
    NWMArm9Buf *sys = NWMi_GetSystemWork();

    // �R�}���h���M�p�̃o�b�t�@���m��
    tmpAddr = NwmGetCommandBuffer4Arm7();
    if (tmpAddr == NULL)
    {
        NWM_WARNING("Failed to get command buffer.\n");
        return NWM_RETCODE_FIFO_ERROR;
    }

    // API ID
    *(u16 *)tmpAddr = (u16)id;

    // �w�萔�̈�����ǉ�
    va_start(vlist, paramNum);
    for (i = 0; i < paramNum; i++)
    {
        tmpAddr[i + 1] = va_arg(vlist, u32);
    }
    va_end(vlist);

    DC_StoreRange(tmpAddr, NWM_APIFIFO_BUF_SIZE);

    // FIFO�Œʒm
    result = PXI_SendWordByFifo(PXI_FIFO_TAG_WMW, (u32)tmpAddr, FALSE);

    (void)OS_SendMessage(&sys->apibufQ.q, tmpAddr, OS_MESSAGE_BLOCK);

    if (result < 0)
    {
        NWM_WARNING("Failed to send command through FIFO.\n");
        return NWM_RETCODE_FIFO_ERROR;
    }

    return NWM_RETCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         NWMi_SendCommandDirect

  Description:  FIFO�o�R�Ń��N�G�X�g��ARM7�ɑ��M����B
                ARM7 �ɑ���R�}���h�𒼐ڎw�肷��B

  Arguments:    data        -   ARM7 �ɑ���R�}���h�B
                length      -   ARM7 �ɑ���R�}���h�̃T�C�Y�B

  Returns:      int -   NWM_RETCODE_*�^�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NWMRetCode NWMi_SendCommandDirect(void *data, u32 length)
{
    int     result;
    u32    *tmpAddr;
    NWMArm9Buf *sys = NWMi_GetSystemWork();

    SDK_ASSERT(length <= NWM_APIFIFO_BUF_SIZE);

    // �R�}���h���M�p�̃o�b�t�@���m��
    tmpAddr = NwmGetCommandBuffer4Arm7();
    if (tmpAddr == NULL)
    {
        NWM_WARNING("Failed to get command buffer.\n");
        return NWM_RETCODE_FIFO_ERROR;
    }

    // ARM7 �ɑ���R�}���h���p�o�b�t�@�ɃR�s�[����
    MI_CpuCopy8(data, tmpAddr, length);

    DC_StoreRange(tmpAddr, length);

    // FIFO�Œʒm
    result = PXI_SendWordByFifo(PXI_FIFO_TAG_WMW, (u32)tmpAddr, FALSE);

    (void)OS_SendMessage(&sys->apibufQ.q, tmpAddr, OS_MESSAGE_BLOCK);

    if (result < 0)
    {
        NWM_WARNING("Failed to send command through FIFO.\n");
        return NWM_RETCODE_FIFO_ERROR;
    }

    return NWM_RETCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         NwmGetCommandBuffer4Arm7

  Description:  ARM7 �����̃R�}���h�p�o�b�t�@���v�[������m�ۂ���

  Arguments:    None.

  Returns:      �m�ۂł���΂��̒l, �m�ۂł��Ȃ���� NULL
 *---------------------------------------------------------------------------*/
u32 *NwmGetCommandBuffer4Arm7(void)
{
    u32    *tmpAddr = NULL;
    NWMArm9Buf *sys = NWMi_GetSystemWork();

    do {
        if (FALSE == OS_ReceiveMessage(&sys->apibufQ.q, (OSMessage *)&tmpAddr, OS_MESSAGE_NOBLOCK))
        {
            return NULL;
        }

        // invalidate entire apibuf
        DC_InvalidateRange(sys->apibuf, NWM_APIBUF_NUM * NWM_APIFIFO_BUF_SIZE);

        // �����O�o�b�t�@���g�p�\�ȏ��(�L���[����t�łȂ�)�����m�F
        DC_InvalidateRange(tmpAddr, sizeof(u16));

        if ((*((u16 *)tmpAddr) & NWM_API_REQUEST_ACCEPTED) == 0)
        {
            // return pending buffer to tail of queue
            (void)OS_SendMessage(&sys->apibufQ.q, tmpAddr, OS_MESSAGE_BLOCK);
            tmpAddr = NULL;
            continue;
        }
    } while(tmpAddr == NULL);

    return tmpAddr;
}

