/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - WCM - libraries
  File:     cpsif.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-12-19#$
  $Rev: 178 $
  $Author: adachi_hiroaki $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"
#include "wcm_message.h"

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
#define WCMCPSIF_DUMMY_IRQ_THREAD   ((OSThread*)OS_IrqHandler)

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/

/* 
 * ThreadQueue �� Mutex �ȂǁAWCM���C�u�������I�������ۂɉ������Ă��܂���
 * ����ϐ��Q�� static �ȕϐ��Ƃ���B�����̕ϐ��͈�x�����������ƂQ�x��
 * ������ꂽ��ď��������ꂽ�肵�Ȃ��B
 */
static WCMCpsifWork wcmCpsifw;

/*---------------------------------------------------------------------------*
    �����֐��v���g�^�C�v
 *---------------------------------------------------------------------------*/
static void WcmCpsifWmCallbackForSendAsync(void* arg);
static u32 WCMi_MergeToBuffer(u8 deviceId, u16* sendBuf, 
                              const void* header, u32 headerLen,
                              const void* body, u32 bodyLen);


/*---------------------------------------------------------------------------*
  Name:         WCMi_InitCpsif

  Description:  CPS ���C�u�����Ƃ̃C���^�[�t�F�[�X�ƂȂ镔���ɂ��āA���[�N
                �ϐ��Q������������B��x������������ď������͂��Ȃ��B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_InitCpsif(void)
{
    if (wcmCpsifw.initialized == 0)
    {
        wcmCpsifw.initialized = 1;
        wcmCpsifw.sendResult = WM_ERRCODE_SUCCESS;
        MI_CpuClear8(wcmCpsifw.macAddress, sizeof(wcmCpsifw.macAddress));

        /* wcmCpsifw.recvCallback �͕ύX����Ă���\��������̂ŏ��������Ȃ� */
        OS_InitThreadQueue(&(wcmCpsifw.sendQ));
        OS_InitMutex(&(wcmCpsifw.sendMutex));
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_CpsifRecvCallback

  Description:  �����h���C�o�� DCF �t���[������M�����ۂɌĂяo�����֐��B
                ���荞�݃T�[�r�X���ŌĂяo����ACPS �̎�M�R�[���o�b�N�֐���
                �Ăяo���B

  Arguments:    srcAdrs    -   ��M���� DCF �t���[���\���̂Ɋ܂܂��A���M���A�h���X
                destAdrs   -   ��M���� DCF �t���[���\���̂Ɋ܂܂��A���M��A�h���X
                data       -   ��M�����f�[�^
                length     -   ��M���� DCF �t���[���\���̂̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_CpsifRecvCallback(u8 *srcAdrs, u8 *destAdrs, u8 *data, s32 length)
{
#ifndef SDK_FINALROM
    if (WCM_DebugFlags & WCM_DEBUGFLAG_DUMP_RECV)
    {
        OS_TPrintf("Received frame: src=%02x:%02x:%02x:%02x:%02x:%02x\n",
                   srcAdrs[0], srcAdrs[1], srcAdrs[2], srcAdrs[3], srcAdrs[4], srcAdrs[5]);
        WCMi_DumpMemory(data, (u32)length);
    }
#endif
    if (wcmCpsifw.recvCallback != NULL)
    {
        wcmCpsifw.recvCallback(srcAdrs, destAdrs, data, length);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_GetCpsifWork
  
  Description:  
  
  Argument:     None.
  
  Returns:      WCMCpsifWork - 
 *---------------------------------------------------------------------------*/
WCMCpsifWork* WCMi_GetCpsifWork(void)
{
    SDK_ASSERT(wcmCpsifw.initialized != 0);
    return &wcmCpsifw;
}
/*---------------------------------------------------------------------------*
  Name:         WCMi_CpsifSendNullPacket

  Description:  �L�[�v�A���C�u NULL �p�P�b�g �𑗐M����B
                ���� DCF �p�P�b�g�𑗐M���̏ꍇ�͉����s��Ȃ��B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_CpsifSendNullPacket(void)
{
    WCMWork*    w = WCMi_GetSystemWork();
    
    if (!w)
    {
        return;
    }
    
    switch(w->deviceId)
    {
    case WCM_DEVICEID_WM:
        WCMi_WmifSendNullPacket();
        break;
#ifdef SDK_TWL
    case WCM_DEVICEID_NWM:
        if (OS_IsRunOnTwl())
        {
            WCMi_NwmifSendNullPacket();
            break;
        }
        // no break;
#endif
    default:
        break;;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetApMacAddress

  Description:  �����h���C�o���ڑ����m�����Ă��� AP �� MAC �A�h���X���擾����B
    NOTICE:     ���̊֐��ɂ���Ď擾�����|�C���^�������o�b�t�@�̓��e�́A���荞��
                ���ɂ���ԕω��ɔ����ĕύX���ꂽ��A�o�b�t�@���g��������ꂽ��
                ����\��������_�ɒ��ӁB���荞�݋֎~������ԂŌĂяo���A���e��
                �ʃo�b�t�@�ɑޔ����邱�Ƃ𐄏�����B

  Arguments:    None.

  Returns:      u8*     -   �ڑ����m�����Ă��� AP �� MAC �A�h���X�ւ̃|�C���^��
                            �Ԃ��B�ڑ����m������Ă��Ȃ��ꍇ�ɂ� NULL ��Ԃ��B
 *---------------------------------------------------------------------------*/
u8* WCM_GetApMacAddress(void)
{
    u8*         mac = NULL;
    WCMWork*    w = WCMi_GetSystemWork();
    OSIntrMode  e = OS_DisableInterrupts();

    // �������m�F
    if (w != NULL)
    {
        // WCM ���C�u�����̏�Ԋm�F
        if ((w->phase == WCM_PHASE_DCF) && (w->resetting == WCM_RESETTING_OFF))
        {
            /* �����h���C�o�ɂ����ăR�l�N�V�������m������Ă����� */
            mac = w->bssDesc.bssid;
        }
    }

    (void)OS_RestoreInterrupts(e);
    return mac;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetApEssid

  Description:  �����h���C�o���ڑ����m�����Ă��� AP �� ESS-ID ���擾����B
    NOTICE:     ���̊֐��ɂ���Ď擾�����|�C���^�������o�b�t�@�̓��e�́A���荞��
                ���ɂ���ԕω��ɔ����ĕύX���ꂽ��A�o�b�t�@���g��������ꂽ��
                ����\��������_�ɒ��ӁB���荞�݋֎~������ԂŌĂяo���A���e��
                �ʃo�b�t�@�ɑޔ����邱�Ƃ𐄏�����B

  Arguments:    length  -   ESS-ID �̒������擾���� u16 �^�ϐ��ւ̃|�C���^���w��
                            ����B�ڑ����m������Ă��Ȃ��ꍇ�ɂ� 0 ���i�[�����B
                            �������擾����K�v���Ȃ��ꍇ�ɂ́A���̈����� NULL ��
                            �w�肵�Ă��悢�B

  Returns:      u8*     -   �ڑ����m�����Ă��� AP �� ESS-ID �f�[�^��ւ̃|�C���^
                            ��Ԃ��B�ڑ����m������Ă��Ȃ��ꍇ�ɂ� NULL ��Ԃ��B
 *---------------------------------------------------------------------------*/
u8* WCM_GetApEssid(u16* length)
{
    u8*         essid = NULL;
    u16         len = 0;
    WCMWork*    w = WCMi_GetSystemWork();
    OSIntrMode  e = OS_DisableInterrupts();

    // �������m�F
    if (w != NULL)
    {
        // WCM ���C�u�����̏�Ԋm�F
        if ((w->phase == WCM_PHASE_DCF) && (w->resetting == WCM_RESETTING_OFF))
        {
            /* �����h���C�o�ɂ����ăR�l�N�V�������m������Ă����� */
            essid = &(w->bssDesc.ssid[0]);
            len = w->bssDesc.ssidLength;
        }
    }

    (void)OS_RestoreInterrupts(e);
    if (length != NULL)
    {
        *length = len;
    }

    return essid;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SetRecvDCFCallback

  Description:  CPS �̎�M�R�[���o�b�N�֐���o�^����B
                �����h���C�o�� DCF �t���[������M����x�ɂ����œo�^������M
                �R�[���o�b�N�֐����Ăяo�����B
    NOTICE:     �����œo�^�����R�[���o�b�N�֐����I�[�o�[���C���ɂ���ꍇ�ɂ́A
                ��ѐ悪�������ɑ��݂��Ȃ��Ȃ�O�ɓo�^���Ă���R�[���o�b�N�֐���
                NULL �ŏ㏑���N���A����K�v������_�ɒ��ӁB

  Arguments:    callback    -   DCF ��M�R�[���o�b�N�֐����w�肷��B
                                NULL ���w�肷��Ǝ�M�ʒm����Ȃ��Ȃ�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCM_SetRecvDCFCallback(WCMRecvInd callback)
{
    OSIntrMode  e = OS_DisableInterrupts();

    wcmCpsifw.recvCallback = callback;

    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SendDCFData

  Description:  �����h���C�o�� DCF �t���[�����M���w������BDCF �t���[���̑��M
                �����A�������͎��s����܂œ����Ńu���b�N���đ҂B�܂��A������
                �ڑ���Ԃ� DCF �t���[���𑗐M�ł��Ȃ���Ԃ̏ꍇ�ɂ͎��s����B
    NOTICE:     �����̃X���b�h����񓯊��ɌĂяo����邱�Ƃ�z�肵�Ă��邪�A
                ���荞�݃T�[�r�X���ŌĂяo����邱�Ƃ͑z�肵�Ă��Ȃ��_�ɒ��ӁB

  Arguments:    dstAddr -   DCF �t���[���̂��Đ�ƂȂ� MAC �A�h���X���w�肷��B
                buf     -   ���M����f�[�^�ւ̃|�C���^���w�肷��B
                            ���֐�����߂�܂ł͎w�肵���o�b�t�@�̓��e�͕ێ�
                            ���Ă����K�v������_�ɒ��ӁB
                len     -   ���M����f�[�^�̒������o�C�g�P�ʂŎw�肷��B

  Returns:      s32     -   ���M�ɐ��������ꍇ�ɑ��M���ꂽ�f�[�^����Ԃ��B
                            ���s�����ꍇ�ɂ͕��l��Ԃ��B
 *---------------------------------------------------------------------------*/
s32 WCM_SendDCFData(const u8* dstAddr, const u8* buf, s32 len)
{
    return WCM_SendDCFDataEx(dstAddr, buf, len, NULL, 0);
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SendDCFDataEx

  Description:  �����h���C�o�� DCF �t���[�����M���w������BDCF �t���[���̑��M
                �����A�������͎��s����܂œ����Ńu���b�N���đ҂B�܂��A������
                �ڑ���Ԃ� DCF �t���[���𑗐M�ł��Ȃ���Ԃ̏ꍇ�ɂ͎��s����B
    NOTICE:     �����̃X���b�h����񓯊��ɌĂяo����邱�Ƃ�z�肵�Ă��邪�A
                ���荞�݃T�[�r�X���ŌĂяo����邱�Ƃ͑z�肵�Ă��Ȃ��_�ɒ��ӁB

  Arguments:    dstAddr 	-   DCF �t���[���̂��Đ�ƂȂ� MAC �A�h���X���w�肷��B
  				header      -   ���M����f�[�^�̃w�b�_�����ւ̃|�C���^���w�肷��B
  				headerLen   -   ���M����f�[�^�̃w�b�_�̒������o�C�g�P�ʂŎw�肷��B
  				body        -   ���M����f�[�^�̃{�f�B�����ւ̃|�C���^���w�肷��B
  				bodyLen     -   ���M����f�[�^�̃{�f�B�����̒������o�C�g�P�ʂŎw�肷��B
                
                * header�Abody �Ɋւ��ẮA���֐�����߂�܂ł͎w�肵���o�b�t�@�̓��e��
                  �ێ����Ă����K�v������_�ɒ��ӁB

  Returns:      s32     -   ���M�ɐ��������ꍇ�ɑ��M���ꂽ�f�[�^����Ԃ��B
                            ���s�����ꍇ�ɂ͕��l��Ԃ��B
 *---------------------------------------------------------------------------*/
s32 WCM_SendDCFDataEx(const u8* dstAddr, const u8* header, s32 headerLen,
                      const u8* body, s32 bodyLen)
{
    OSIntrMode          e = OS_DisableInterrupts();
    volatile WCMWork*   w = WCMi_GetSystemWork();
    s32                 result;
    u32                 totalLen = (u32)(headerLen + bodyLen);

    // �������ς݂��m�F
    if (w == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_BEFORE_INIT;        // WCM ���C�u����������������Ă��Ȃ�
    }

#if WCM_DEBUG
    // �p�����[�^�m�F
    if ((dstAddr == NULL) || (header == NULL))
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "dstAddr or header");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // header �܂��� body �̃|�C���^�� NULL
    }

    if (headerLen <= 0)
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "headerLen");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // headerLen �܂��� bodyLen �� 0 �ȉ�
    }

    if (headerLen + bodyLen > WCM_ROUNDUP32(WM_DCF_MAX_SIZE))
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "headerLen + bodyLen");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // (headerLen + bodyLen) �� �T�C�Y����
    }

    if ((u32) header & 0x01 || (u32) body & 0x01)
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "header or body");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // header �܂��� body �̃o�b�t�@����A�h���X
    }

    // ���荞�݃T�[�r�X�����ǂ������m�F
    if (OS_GetProcMode() == HW_PSR_IRQ_MODE)
    {
        WCMi_Warning(WCMi_WText_InIrqMode);
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_IN_IRQ_MODE;        // IRQ ���[�h�ł̌Ăяo���͋֎~
    }
#endif

    // �ʃX���b�h���r�����M�u���b�N���̏ꍇ�͉��������̂�҂�
    OS_LockMutex(&(wcmCpsifw.sendMutex));

    /*
     * �u���b�N��Ԃ��畜�A�������_�ŁAWCM���C�u�����̏�Ԃ͕s���B
     * AP �Ɛڑ�����Ă����ԂłȂ��Ƒ��M�͂ł��Ȃ����߁A������ WCM ���C�u������
     * ��Ԃ��m�F����K�v������B
     */
    w = WCMi_GetSystemWork();
    if (w == NULL)
    {
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_BEFORE_INIT;        // �Q�Ă���Ԃ� WCM ���C�u�������I�����Ă��܂����ꍇ
    }

    if ((w->phase != WCM_PHASE_DCF) || (w->resetting == WCM_RESETTING_ON))
    {
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_NO_CONNECTION;      // ���M���悤�Ƃ������� AP �Ƃ̐ڑ����m�����Ă��Ȃ��ꍇ
    }

    /*
     * ���M�f�[�^���L���b�V���Z�[�t�ȃo�b�t�@�Ɉ�U�R�s�[����B
     * �����̃X���b�h����̑��M�v���͔r���I�ɏ��������̂ŁA�R�s�[��̃L���b�V���Z�[�t��
     * �o�b�t�@�͈��̑��M����������Ηǂ��͂��B
     * 
     * �w�b�_�����ƃ{�f�B�����ɕ�����Ă����f�[�^���AWCMWork �� SendBuf �ɂ܂Ƃ߂ăR�s�[����
     */
    totalLen = WCMi_MergeToBuffer(w->deviceId, (u16*)w->sendBuf, header, (u32)headerLen, body, (u32)bodyLen);

#ifndef SDK_FINALROM
    if (WCM_DebugFlags & WCM_DEBUGFLAG_DUMP_SEND)
    {
        OS_TPrintf("Send frame: %02x:%02x:%02x:%02x:%02x:%02x\n",
                   dstAddr[0], dstAddr[1], dstAddr[2], dstAddr[3], dstAddr[4], dstAddr[5]);
        WCMi_DumpMemory((u8*)(w->sendBuf), totalLen);
    }
#endif

    switch(w->deviceId)
    {
    case WCM_DEVICEID_WM:
        result = WCMi_WmifSetDCFData(dstAddr, (u16*)w->sendBuf, (u16) totalLen, NULL);
        break;
#ifdef SDK_TWL
    case WCM_DEVICEID_NWM:
        if (OS_IsRunOnTwl())
        {
            result = WCMi_NwmifSetDCFData(dstAddr, (u16*)w->sendBuf, (u16) totalLen, NULL);
            break;
        }
        // no break;
#endif
    default:
        result = WCM_CPSIF_RESULT_ILLEGAL_PARAM;
    }
    
    // ����I��
    OS_UnlockMutex(&(wcmCpsifw.sendMutex));
    (void)OS_RestoreInterrupts(e);

#ifndef SDK_FINALROM
    if (WCM_DebugFlags & WCM_DEBUGFLAG_DUMP_SEND)
    {
        OS_TPrintf("Send frame complete\n");
    }
#endif
    return headerLen + bodyLen;
}
/*---------------------------------------------------------------------------*
  Name:         WCM_SendDCFDataAsync

  Description:  �����h���C�o�� DCF �t���[�����M���w������BDCF �t���[���̑��M
                ������҂����ɒ����ɐ����Ԃ��B�܂��A���� DCF �t���[���𑗐M
                ���ł������̏ꍇ��A�����̐ڑ���Ԃ� DCF �t���[���𑗐M�ł���
                ����Ԃ̏ꍇ�ɂ͒����Ɏ��s����B
    NOTICE:     ���荞�݃T�[�r�X���ł��Ăяo���͉\�B

  Arguments:    dstAddr -   DCF �t���[���̂��Đ�ƂȂ� MAC �A�h���X���w�肷��B
                buf     -   ���M����f�[�^�ւ̃|�C���^���w�肷��B
                            ���֐�����߂�܂ł͎w�肵���o�b�t�@�̓��e�͕ێ�
                            ���Ă����K�v������_�ɒ��ӁB
                len     -   ���M����f�[�^�̒������o�C�g�P�ʂŎw�肷��B
                callback -  ���̃f�[�^�����M�\�ɂȂ����ۂɌĂяo�����֐�
                            �ւ̃|�C���^���w�肷��B

  Returns:      s32     -   ���M�̗\��ɐ��������ꍇ�ɂ�0��Ԃ��B
                            ���s�����ꍇ�ɂ͕��l��Ԃ��B
 *---------------------------------------------------------------------------*/
s32 WCM_SendDCFDataAsync(const u8* dstAddr, const u8* buf, s32 len, WCMSendInd callback)
{
    OSIntrMode          e = OS_DisableInterrupts();
    s32                 result;
    u32                 totalLen;
    volatile WCMWork*   w = WCMi_GetSystemWork();

    // �������ς݂��m�F
    if (w == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_BEFORE_INIT;        // WCM ���C�u����������������Ă��Ȃ�
    }

    if ((w->phase != WCM_PHASE_DCF) || (w->resetting == WCM_RESETTING_ON))
    {
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_NO_CONNECTION;      // AP �Ƃ̐ڑ����m�����Ă��Ȃ��ꍇ
    }

#if WCM_DEBUG
    // �p�����[�^�m�F
    if ((dstAddr == NULL) || (buf == NULL))
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "dstAddr or buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // �|�C���^�� NULL
    }

    if (len <= 0)
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "len");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // len �� 0 �ȉ�
    }

    if (len > WCM_ROUNDUP32(WM_DCF_MAX_SIZE))
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "len");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // len �� �T�C�Y����
    }

    if ((u32) buf & 0x01)
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // buf ����A�h���X
    }
#endif

    // �r�����b�N (IRQ ���[�h�ɂ������Ŕr���������s��)
    if (FALSE == WCMi_CpsifTryLockMutexInIRQ(&(wcmCpsifw.sendMutex)))
    {
        (void)OS_RestoreInterrupts(e);
        WCMi_Warning("WCMi_CpsifTryLockMutexInIRQ failed.");
        return WCM_CPSIF_RESULT_SEND_FAILURE;       // ���̃p�P�b�g�𑗐M��
    }
    /*
     * ���M�f�[�^���L���b�V���Z�[�t�ȃo�b�t�@�Ɉ�U�R�s�[����B
     * �����̃X���b�h����̑��M�v���͔r���I�ɏ��������̂ŁA�R�s�[��̃L���b�V���Z�[�t��
     * �o�b�t�@�͈��̑��M����������Ηǂ��͂��B
     */
     
    totalLen = WCMi_MergeToBuffer(w->deviceId, (u16*)w->sendBuf, buf, (u32)len, NULL, 0);
    
    // ���M�v�����s
    switch(w->deviceId)
    {
    case WCM_DEVICEID_WM:
        result = WCMi_WmifSetDCFData(dstAddr, (u16*)w->sendBuf, (u16) totalLen, callback);
        break;
#ifdef SDK_TWL
    case WCM_DEVICEID_NWM:
        if (OS_IsRunOnTwl())
        {
            result = WCMi_NwmifSetDCFData(dstAddr, (u16*)w->sendBuf, (u16) totalLen, callback);
            break;
        }
        // no break;
#endif
    default:
        result = WCM_CPSIF_RESULT_ILLEGAL_PARAM;
    }
    
    if (result < 0)
    {
        WCMi_CpsifUnlockMutexInIRQ(&(wcmCpsifw.sendMutex));
    }
    // ���������ꍇ�̓R�[���o�b�N�� WCMi_CpsifUnlockMutexInIRQ ���Ă΂��

    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}


/*---------------------------------------------------------------------------*
  Name:         WCMi_CpsifTryLockMutexInIRQ

  Description:  IRQ ���荞�݃T�[�r�X���ɂ����Ĕr������p Mutex �̃��b�N�����݂�B
                ���̊֐��Ń��b�N���� Mutex �� WcmCpsifUnlockMutexInIRQ �֐���
                �̂݃A�����b�N�\�B

  Arguments:    mutex   -   �r������p OSMutex �ϐ��ւ̃|�C���^���w��B

  Returns:      BOOL    -   ���b�N�ɐ��������ꍇ�� TRUE ��Ԃ��B���ɂȂ�炩��
                            �X���b�h���烍�b�N����Ă����ꍇ�ɂ� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL WCMi_CpsifTryLockMutexInIRQ(OSMutex* mutex)
{
#if WCM_DEBUG
    if (mutex == NULL)
    {
        OS_TWarning("WCMi_CpsifTryLockMutexInIRQ: Invalid argument.");
        return FALSE;
    }
#endif
    if (mutex->thread == NULL)
    {
        mutex->thread = WCMCPSIF_DUMMY_IRQ_THREAD;
        mutex->count++;
        return TRUE;
    }
    else if (mutex->thread == WCMCPSIF_DUMMY_IRQ_THREAD)
    {
        mutex->count++;
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_CpsifUnlockMutexInIRQ

  Description:  IRQ ���荞�݃T�[�r�X���ɂ����Ĕr������p Mutex �̃A�����b�N��
                ���݂�BWcmCpsifTryLockMutexInIRQ �֐��Ń��b�N���� Mutex �ɑ�
                ���Ă̂ݗL���B
                �A�����b�N�ł��Ȃ������ꍇ�ɂ́A�����m���i�͂Ȃ��B

  Arguments:    mutex   -   �r������p OSMutex �ϐ��ւ̃|�C���^���w��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_CpsifUnlockMutexInIRQ(OSMutex* mutex)
{
#if WCM_DEBUG
    if (mutex == NULL)
    {
        OS_TWarning("WCMi_CpsifUnlockMutexInIRQ: Invalid argument.");
        return;
    }
#endif
    if (mutex->thread == WCMCPSIF_DUMMY_IRQ_THREAD)
    {
        if (--(mutex->count) == 0)
        {
            mutex->thread = NULL;
            OS_WakeupThread(&(mutex->queue));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_CpsifSetMacAddress

  Description:  CPS �ɓn�� MAC �A�h���X��ݒ肷��.

  Arguments:    macAddress  - MAC �A�h���X�̊i�[��|�C���^���w��.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_CpsifSetMacAddress(u8* macAddress)
{
    MI_CpuCopy(macAddress, wcmCpsifw.macAddress, 6);
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetMacAddress

  Description:  �I������Ă��閳���f�o�C�X�� MAC �A�h���X���擾����B

  Arguments:    macAddress  - MAC �A�h���X�̊i�[��|�C���^���w��.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCM_GetMacAddress(u8 *macAddress)
{
    MI_CpuCopy(wcmCpsifw.macAddress, macAddress, 6);
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_MergeToBuffer

  Description:  

  Arguments:    deviceId     - 
                sendBuf      - 
                header       - 
                headerLen    - 
                body         - 
                bodyLen      - 

  Returns:      u32          - 
 *---------------------------------------------------------------------------*/
static u32 WCMi_MergeToBuffer(u8 deviceId, u16* sendBuf, 
                              const void* header, u32 headerLen,
                              const void* body, u32 bodyLen)
{
    void*   copyDest = NULL;
    u32     totalLen = (u32)(headerLen + bodyLen);
    
    /*
     * WM �� NWM �ł͑��M���ɓn���ׂ��t���[���̌`�����Ⴄ���߁A
     * �ǂ��炪�g���Ă��邩�ɉ����Č`����ϊ�����B
     *
     *        | da | sa | len | llc/snap | type | data |
     * WM                      <-----data from cps---->
     *                        ^ ptr
     *
     * NWM    <---reserved---> <-----data from cps---->
     *        ^ ptr
     */
    
    switch(deviceId)
    {
    default:
    case WCM_DEVICEID_WM:
        copyDest = sendBuf;
        break;
#ifdef SDK_TWL
    case WCM_DEVICEID_NWM:
        copyDest = (void*)((u32)sendBuf + WCM_SIZE_ETHHDR);
        ((u16*)sendBuf)[ WCM_SIZE_MACADDR * 2 / 2 ] = MI_HToBE16((u16)totalLen);
        totalLen += WCM_SIZE_ETHHDR;
        break;
#endif
    }
    
    SDK_POINTER_ASSERT(copyDest);
    
    /*
     * ���M�f�[�^���L���b�V���Z�[�t�ȃo�b�t�@�Ɉ�U�R�s�[����B
     * �����̃X���b�h����̑��M�v���͔r���I�ɏ��������̂ŁA�R�s�[��̃L���b�V���Z�[�t��
     * �o�b�t�@�͈��̑��M����������Ηǂ��͂��B
     * 
     * �w�b�_�����ƃ{�f�B�����ɕ�����Ă����f�[�^���AWCMWork �� SendBuf �ɂ܂Ƃ߂ăR�s�[����
     */
    MI_CpuCopy(header, copyDest, (u32)headerLen );
    if (body && bodyLen > 0)
    {
        MI_CpuCopy(body, (u8*)copyDest + headerLen, (u32) bodyLen);
    }
    
    return totalLen;
}


/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
