/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - WCM - libraries
  File:     wmif.c

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

#include "wcm_private.h"
#include "wcm_message.h"

/*---------------------------------------------------------------------------*
    �����֐��v���g�^�C�v
 *---------------------------------------------------------------------------*/
static void         WcmWmReset(void);
static void         WcmEditScanExParam(const void* bssid, const void* essid, u32 option);

static void         WcmWmcbIndication(void* arg);
static void         WcmWmcbCommon(void* arg);
static void         WcmWmcbScanEx(void* arg);
static void         WcmWmcbEndScan(void* arg);
static void         WcmWmcbConnect(void* arg);
static void         WcmWmcbDisconnect(void* arg);
static void         WcmWmcbStartDCF(void* arg);
static void         WcmWmcbEndDCF(void* arg);
static void         WcmWmcbReset(void* arg);
static void         WcmWmcbSetDCFData(void* arg);
static void         WcmWmcbSetDCFDataAsync(void* arg);
static void         WcmKACallback(void* arg);

/*---------------------------------------------------------------------------*
  Name:         WCMi_WmifStartupAsync

  Description:  �����@�\�̋N���V�[�P���X���J�n

  Arguments:    None.

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_WmifStartupAsync(void)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    WMErrCode   wmResult;

    // WM ���C�u����������
    wmResult = WM_Init(wcmw->wm.work, (u16) wcmw->config.dmano);
    switch (wmResult)
    {
    case WM_ERRCODE_SUCCESS:
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // WM ���C�u���������ɏ���������Ă���

    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(WCMi_WText_UnsuitableArm7);
        return WCM_RESULT_WMDISABLE;            // ARM7 ���� WM ���C�u���������삵�Ă��Ȃ�

    case WM_ERRCODE_INVALID_PARAM:
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // �z��͈͊O�̃G���[
    }

    // �s����� ARM7 ����̒ʒm���󂯎��x�N�g����ݒ�
    wmResult = WM_SetIndCallback(WcmWmcbIndication);
    if (wmResult != WM_ERRCODE_SUCCESS)
    {
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // �z��͈͊O�̃G���[
    }

    // CPS �ɓn�� MAC �A�h���X��ݒ�
    {
        u8 macAddress[WM_SIZE_MACADDR];
        OS_GetMacAddress(macAddress);
        WCMi_CpsifSetMacAddress(macAddress);
    }

    // �����n�[�h�E�F�A�g�p���v���𔭍s
    wmResult = WM_Enable(WcmWmcbCommon);
    switch (wmResult)
    {
    case WM_ERRCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_WAIT_TO_IDLE);    // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
        wcmw->notifyId = WCM_NOTIFY_STARTUP;
        break;

    case WM_ERRCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "WM_Enable", wmResult);
        WCMi_SetPhase(WCM_PHASE_IRREGULAR);
        return WCM_RESULT_FAILURE;              // ARM7 �ւ̗v�����s�Ɏ��s

    case WM_ERRCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // �z��͈͊O�̃G���[
    }
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_WmifCleanupAsync

  Description:  �����@�\�̒�~�V�[�P���X���J�n

  Arguments:    None.

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_WmifCleanupAsync(void)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    WMErrCode   wmResult;

    // �����n�[�h�E�F�A�̃V���b�g�_�E���v���𔭍s
    wmResult = WM_PowerOff(WcmWmcbCommon);
    switch (wmResult)
    {
    case WM_ERRCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_IDLE_TO_WAIT);    // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
        wcmw->notifyId = WCM_NOTIFY_CLEANUP;
        break;

    case WM_ERRCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "WM_PowerOff", wmResult);
        return WCM_RESULT_REJECT;               // ARM7 �ւ̗v�����s�Ɏ��s( ���g���C�\ )

    case WM_ERRCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // �z��͈͊O�̃G���[
    }
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_WmifBeginSearchAsync

  Description:  AP �����T���J�n�V�[�P���X���J�n�A�������͒T�������ύX

  Arguments:    bssid        - 
                essid        - 
                option       - 

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_WmifBeginSearchAsync(const void* bssid, const void* essid, u32 option)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    WMErrCode   wmResult;

    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
        break;

    case WCM_PHASE_IDLE_TO_SEARCH:
        WcmEditScanExParam(bssid, essid, option);
        return WCM_RESULT_PROGRESS;             // ���ɓ������N�G�X�g�̔񓯊�������

    case WCM_PHASE_SEARCH:
        WcmEditScanExParam(bssid, essid, option);
        return WCM_RESULT_SUCCESS;              // ��ԑJ�ږڕW�t�F�[�Y�Ɉڍs�ς�

    default:
        WCMi_Warning(WCMi_WText_IllegalPhase, wcmw->phase);
        return WCM_RESULT_FAILURE;              // �v�����󂯕t�����Ȃ��t�F�[�Y
    }

    // �X�L�����J�n�v���𔭍s
    WcmEditScanExParam(bssid, essid, option);
    DC_InvalidateRange(wcmw->wm.scanExParam.scanBuf, wcmw->wm.scanExParam.scanBufSize);
    wcmw->wm.scanCount++;
    wmResult = WM_StartScanEx(WcmWmcbScanEx, &(wcmw->wm.scanExParam));
    switch (wmResult)
    {
    case WM_ERRCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_IDLE_TO_SEARCH);  // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
        wcmw->notifyId = WCM_NOTIFY_BEGIN_SEARCH;
        break;

    case WM_ERRCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "WM_StartScanEx", wmResult);
        return WCM_RESULT_REJECT;               // ARM7 �ւ̗v�����s�Ɏ��s( ���g���C�\ )

    case WM_ERRCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // �z��͈͊O�̃G���[
    }
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_WmifConnectAsync

  Description:  AP �Ƃ̖����ڑ��V�[�P���X���J�n����B
                �����I�ɂ� IDLE ���� DCF �փt�F�[�Y�̈ڍs�������s����B
                �񓯊��֐��ł���A�����I�ȕԂ�l�� WCM_RESULT_ACCEPT �ł�����
                �ꍇ�ɂ� WCM_StartupAsync �֐��Ŏw�肵���R�[���o�b�N�֐��ɂ��
                �񓯊��I�ȏ������ʂ��ʒm�����B
                �񓯊��������ʂ̒ʒm��ʂ� WCM_NOTIFY_CONNECT �ƂȂ�B

  Arguments:    None.

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̓����I�ȏ������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCMi_WmifConnectAsync(void)
{
    s32         wcmResult   = WCM_RESULT_SUCCESS;
    WCMWork*    wcmw        = WCMi_GetSystemWork();

    SDK_POINTER_ASSERT(wcmw);

    if (wcmw->phase == WCM_PHASE_IDLE)
    {
        // �T�|�[�g����f�[�^�]�����[�g�� NITRO �p�ɉ���
        wcmw->bssDesc.rateSet.support = (u16) (WCM_ADDITIONAL_RATESET | wcmw->bssDesc.rateSet.basic);
    }

    // AP �Ƃ̖����ڑ��V�[�P���X���J�n
    {
        WMErrCode wmResult;
        // �����t�@�[���E�F�A�̃^�C���A�E�g�ݒ�v���𔭍s
        wmResult = WM_SetLifeTime(WcmWmcbCommon, 0xffff, WCM_CAM_LIFETIME, 0xffff, 0xffff);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WCMi_SetPhase(WCM_PHASE_IDLE_TO_DCF); // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
            wcmw->notifyId = WCM_NOTIFY_CONNECT;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(WCMi_RText_WmSyncError, "WM_SetLifeTime", wmResult);
            return WCM_RESULT_REJECT;           // ARM7 �ւ̗v�����s�Ɏ��s( ���g���C�\ )

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(WCMi_WText_InvalidWmState);

        /* Don't break here */
        default:
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
            return WCM_RESULT_FATAL_ERROR;      // �z��͈͊O�̃G���[
        }
    }
    // ����I��
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_WmifDisconnectAsync

  Description:  

  Arguments:    None.

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_WmifDisconnectAsync(void)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    WMErrCode   wmResult;

    // DCF �ʐM���[�h�I���v���𔭍s
    wmResult = WM_EndDCF(WcmWmcbEndDCF);
    switch (wmResult)
    {
    case WM_ERRCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE); // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
        wcmw->notifyId = WCM_NOTIFY_DISCONNECT;
        break;

    case WM_ERRCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "WM_EndDCF", wmResult);
        return WCM_RESULT_REJECT;           // ARM7 �ւ̗v�����s�Ɏ��s( ���g���C�\ )

    case WM_ERRCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;      // �z��͈͊O�̃G���[
    }
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_WmifTerminateAsync

  Description:  

  Arguments:    None.

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_WmifTerminateAsync(void)
{
    WMStatus*   ws;
    u16         wmState;
    WCMWork*    wcmw = WCMi_GetSystemWork();
    WMErrCode   wmResult;

    SDK_POINTER_ASSERT(wcmw);

    // WM ���C�u�����̓�����Ԃ𒲍�
    ws = (WMStatus*)WMi_GetStatusAddress();
    DC_InvalidateRange(ws, 2);
    wmState = ws->state;

    // WM ������Ԃɂ���~��ԂɑJ�ڂ���悤�A�����J�n
    switch (wmState)
    {
    case WM_STATE_READY:
        // WM ���C�u�����I��
        wmResult = WM_Finish();
        if (wmResult == WM_ERRCODE_SUCCESS)
        {
            WCMi_SetPhase(WCM_PHASE_WAIT);
            wcmw->notifyId = WCM_NOTIFY_COMMON;
            return WCM_RESULT_SUCCESS;      // �����I�� WAIT �t�F�[�Y�ւ̈ڍs������
        }
        break;

    case WM_STATE_STOP:
        // �����n�[�h�E�F�A�̎g�p�֎~�v���𔭍s
        wmResult = WM_Disable(WcmWmcbCommon);
        break;

    case WM_STATE_IDLE:
        // �����n�[�h�E�F�A�̃V���b�g�_�E���v���𔭍s
        wmResult = WM_PowerOff(WcmWmcbCommon);
        break;

    default:
        // ���Z�b�g�d���Ăяo���Ǘ��t���O���Z�b�g
        wcmw->resetting = WCM_RESETTING_ON;

        // �����ڑ���Ԃ̃��Z�b�g�v���𔭍s
        wmResult = WM_Reset(WcmWmcbReset);
    }

    // �v�����s�ɑ΂��铯���I�ȏ������ʂ��m�F
    switch (wmResult)
    {
    case WM_ERRCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_TERMINATING); // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
        wcmw->notifyId = WCM_NOTIFY_TERMINATE;
        break;

    case WM_ERRCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "WM_***", wmResult);
        return WCM_RESULT_REJECT;           // ARM7 �ւ̗v�����s�Ɏ��s( ���g���C�\ )

    case WM_ERRCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;      // �z��͈͊O�̃G���[
    }
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_WmifSetDCFData

  Description:  ���M�v�����s

  Arguments:    dstAddr      - 
                buf          - 
                len          - 

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_WmifSetDCFData(const u8* dstAddr, const u16* buf, u16 len, WCMSendInd callback)
{
    WMErrCode               wmResult;
    volatile WCMCpsifWork*  wcmiw = WCMi_GetCpsifWork();

    SDK_POINTER_ASSERT(wcmiw);

    if (!callback)
    {
        wmResult = WM_SetDCFData(WcmWmcbSetDCFData, (const u8*)dstAddr, buf, len);
    }
    else
    {
        wcmiw->sendCallback = callback;
        wmResult = WM_SetDCFData(WcmWmcbSetDCFDataAsync, (const u8*)dstAddr, buf, len);
    }
    
    switch (wmResult)
    {
    case WM_ERRCODE_OPERATING:
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_INVALID_PARAM:
    case WM_ERRCODE_FIFO_ERROR:
    default:
        // ���M�v���Ɏ��s�����ꍇ
        WCMi_Printf(WCMi_RText_WmSyncError, "WM_SetDCFData", wmResult);
        return WCM_CPSIF_RESULT_SEND_FAILURE;   // DCF ���M�Ɏ��s
    }

    if (!callback)
    {
        // ���M�����҂��u���b�N
        OS_SleepThread((OSThreadQueue*)&(wcmiw->sendQ));
        /*
         * �u���b�N��Ԃ��畜�A�������_�ŁAWCM���C�u�����̏�Ԃ͕s���B
         * WCM ���C�u�������I������Ă��Ă��A���M���ʂ͕ێ�����Ă���̂�
         * WCM ���C�u�����̏�Ԃ��m�F����K�v�͂Ȃ��B
         */
        switch (wcmiw->sendResult)
        {
        case WM_ERRCODE_SUCCESS:
            break;

        case WM_ERRCODE_FAILED:
        default:
            // ���M�Ɏ��s�����ꍇ
            WCMi_Printf(WCMi_RText_WmAsyncError, "WM_SetDCFData", wcmiw->sendResult);
            return WCM_CPSIF_RESULT_SEND_FAILURE;       // DCF ���M�Ɏ��s
        }
    }
    
    return len;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_WmifSendNullPacket

  Description:  �L�[�v�A���C�u NULL �p�P�b�g �𑗐M����B
                ���� DCF �p�P�b�g�𑗐M���̏ꍇ�͉����s��Ȃ��B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_WmifSendNullPacket(void)
{
    volatile WCMWork*   w = WCMi_GetSystemWork();
    volatile WCMCpsifWork* wcmiw = WCMi_GetCpsifWork();

    // WCM ���C�u�����̓�����Ԃ��m�F
    if (w == NULL)
    {
        return;
    }

    if ((w->phase != WCM_PHASE_DCF) || (w->resetting == WCM_RESETTING_ON))
    {
        return;
    }

    // �r�����b�N
    if (FALSE == WCMi_CpsifTryLockMutexInIRQ((OSMutex*)&(wcmiw->sendMutex)))
    {
        return;
    }

    // ���M�v�����s
    if (WM_ERRCODE_OPERATING != WM_SetDCFData(WcmKACallback, (const u8*)w->bssDesc.bssid, (const u16*)(w->sendBuf),
        0))
    {
        WCMi_CpsifUnlockMutexInIRQ((OSMutex*)&(wcmiw->sendMutex));
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_WmifGetAllowedChannel

  Description:  NITREO �����̎g�p���`�����l�����X�g���擾����B

  Arguments:    None.

  Returns:      u16    - 1�`13ch�̂����A�g�p�������ꂽ�`�����l���Q�̃r�b�g�t�B�[���h��Ԃ��܂��B
                         �ŉ��ʃr�b�g���珇�� 1 �`�����l��, 2 �`�����l���c�c�ɑΉ����܂��B
 *---------------------------------------------------------------------------*/
u16 WCMi_WmifGetAllowedChannel(void)
{
    u16 tmp =WM_GetAllowedChannel();

    u16 low  = 0x0001;
    u16 high = 0x8000;

    if(tmp == 0)
    {
        return 0;
    }

    while(!(high & tmp))
    {
        high = (u16)((high>>1) | 0x8000);
    }
    high <<= 1;

    while(!(low & tmp))
    {
        low = (u16)((low<<1) | 0x0001);
    }
    low >>= 1;

    return (u16)~(high | low);

}

/*---------------------------------------------------------------------------*
  Name:         WcmCpsifKACallback

  Description:  DCF �t���[�����M�v���ɑ΂��閳���h���C�o����̔񓯊��I�ȉ�����
                �󂯎��R�[���o�b�N�֐��B�L�[�v�A���C�u NULL �p�P�b�g���M��
                ��p�B�ʏ�̃p�P�b�g���M�Ƃ͔r���I�ɑ��M������s�����ƂŁA
                ������o�^�ł��Ȃ��R�[���o�b�N�֐���؂�ւ��Ďg�����Ƃ��\�B

  Arguments:    arg -   �����h���C�o����̉����p�����[�^���i�[����Ă���o�b�t�@
                        �̃A�h���X���n�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmKACallback(void* arg)
{
    volatile WCMCpsifWork* wcmiw = WCMi_GetCpsifWork();
#if WCM_DEBUG
    if (((WMCallback*)arg)->errcode == WM_ERRCODE_SUCCESS)
    {
        WCMi_Printf("Send NULL packet for KeepAlive.\n");
    }

#else
#pragma unused(arg)
#endif

    // �����̐��ۂɊւ�炸�r�����b�N����������
    WCMi_CpsifUnlockMutexInIRQ((OSMutex*)&(wcmiw->sendMutex));
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmReset

  Description:  WM_Reset �֐��ɂ�� WM ���C�u�����̃��Z�b�g�������J�n����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmReset(void)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    WMErrCode   wmResult;

    if (wcmw->resetting == WCM_RESETTING_OFF)
    {
        // ���Z�b�g�d���Ăяo���Ǘ��t���O���Z�b�g
        wcmw->resetting = WCM_RESETTING_ON;

        // �����ڑ���Ԃ̃��Z�b�g�v���𔭍s
        wmResult = WM_Reset(WcmWmcbReset);
        if (wmResult != WM_ERRCODE_OPERATING)
        {
            /* ���Z�b�g�Ɏ��s�����ꍇ�͕����s�\ */
            WCMi_Printf(WCMi_RText_WmSyncError, "WM_Reset", wmResult);
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
            WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbIndication

  Description:  WM ���C�u��������̕s����ʒm���󂯎��n���h���B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbIndication(void* arg)
{
    WMIndCallback*  cb = (WMIndCallback*)arg;
    WCMWork*        wcmw = WCMi_GetSystemWork();

    /* WCM ���������Ɋ֌W���Ȃ��ʒm�͖������� */
    if (cb->errcode == WM_ERRCODE_FIFO_ERROR)
    {
        if ((cb->state == WM_STATECODE_FIFO_ERROR) && (cb->reason == WM_APIID_AUTO_DISCONNECT))
        {
            /*
             * �ʐM�̐��������ۂĂȂ��Ȃ�v���I�ȃn�[�h�E�F�A�G���[�� ARM7 ���Ō��m����A
             * �����I�ɐؒf���悤�Ƃ��������N�G�X�g�L���[���l�܂��Ă��Đؒf�v����\��ł��Ȃ������ꍇ
             */
            switch (wcmw->phase)
            {
            case WCM_PHASE_IDLE_TO_DCF:
                WCMi_SetPhase(WCM_PHASE_IRREGULAR);
                break;

            case WCM_PHASE_DCF:
            case WCM_PHASE_IRREGULAR:
                WcmWmReset();
                break;

            case WCM_PHASE_DCF_TO_IDLE:
                WCMi_SetPhase(WCM_PHASE_IRREGULAR);
                break;

                /* �z��O�̃t�F�[�Y�ł̒ʒm�͖������� */
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbCommon

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_Enable , WM_Disable , WM_PowerOn , WM_PowerOff ,
                WM_SetLifeTime , WM_SetBeaconIndication , WM_SetWEPKeyEx
                �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbCommon(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WMErrCode       wmResult = WM_ERRCODE_MAX;
    WCMWork*        wcmw = WCMi_GetSystemWork();

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        // ���̃X�e�b�v�Ƃ��čs���ׂ�����������
        switch (cb->apiid)
        {
        case WM_APIID_ENABLE:
            // �����n�[�h�E�F�A�ւ̓d�͋����v���𔭍s
            wmResult = WM_PowerOn(WcmWmcbCommon);
            break;

        case WM_APIID_DISABLE:
            // WM ���C�u�����I��
            wmResult = WM_Finish();
            switch (wmResult)
            {
            case WM_ERRCODE_SUCCESS:
                WCMi_SetPhase(WCM_PHASE_WAIT);        // �񓯊��V�[�P���X����I��
                WCMi_Notify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
                break;

            case WM_ERRCODE_WM_DISABLE:
                WCMi_Warning(WCMi_WText_InvalidWmState);

            /* Don't break here */
            default:
                WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
                WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
            }

            return; // ���̃X�e�b�v�͂Ȃ��̂ł����ŏI��

        case WM_APIID_POWER_ON:
            WCMi_SetPhase(WCM_PHASE_IDLE);    // �񓯊��V�[�P���X����I��
            WCMi_Notify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
            return; // ���̃X�e�b�v�͂Ȃ��̂ł����ŏI��

        case WM_APIID_POWER_OFF:
            // �����n�[�h�E�F�A�̎g�p�֎~�v���𔭍s
            wmResult = WM_Disable(WcmWmcbCommon);
            break;

        case WM_APIID_SET_LIFETIME:
            // �r�[�R������M�ʒm OFF �v���𔭍s
            wmResult = WM_SetBeaconIndication(WcmWmcbCommon, 0);
            break;

        case WM_APIID_SET_BEACON_IND:
            // WEP �Í����ݒ�v���𔭍s
            wmResult = WM_SetWEPKeyEx(WcmWmcbCommon, (u16) (wcmw->privacyDesc.mode), (u16) (wcmw->privacyDesc.keyId),
                                      (u8*)(wcmw->privacyDesc.key));
            break;

        case WM_APIID_SET_WEPKEY_EX:
            // AP �ւ̖����ڑ��v���𔭍s
            wmResult = WM_StartConnectEx(WcmWmcbConnect, (WMBssDesc*)&(wcmw->bssDesc), NULL,
                                         (BOOL)((wcmw->option & WCM_OPTION_MASK_POWER) == WCM_OPTION_POWER_ACTIVE ? FALSE : TRUE),
                                         (u16) ((wcmw->option & WCM_OPTION_MASK_AUTH) == WCM_OPTION_AUTH_SHAREDKEY ? WM_AUTHMODE_SHARED_KEY :
                                                     WM_AUTHMODE_OPEN_SYSTEM));
            break;
        }

        // �����I�ȏ������ʂ��m�F
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            break;

        case WM_ERRCODE_FIFO_ERROR:
#if WCM_DEBUG
            switch (cb->apiid)
            {
            case WM_APIID_ENABLE:
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_PowerOn", wmResult);
                break;

            case WM_APIID_POWER_OFF:
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_Disable", wmResult);
                break;

            case WM_APIID_SET_LIFETIME:
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_SetBeaconIndication", wmResult);
                break;

            case WM_APIID_SET_BEACON_IND:
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_SetWEPKeyEx", wmResult);
                break;

            case WM_APIID_SET_WEPKEY_EX:
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_StartConnectEx", wmResult);
                break;
            }
#endif
            WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
            WCMi_Notify(WCM_RESULT_FAILURE, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(WCMi_WText_InvalidWmState);

        /* Don't break here */
        default:
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
            WCMi_Notify(WCM_RESULT_FATAL_ERROR, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
#if WCM_DEBUG
        switch (cb->apiid)
        {
        case WM_APIID_ENABLE:
            WCMi_Printf(WCMi_RText_WmAsyncError, "WM_Enable", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_DISABLE:
            WCMi_Printf(WCMi_RText_WmAsyncError, "WM_Disable", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_POWER_ON:
            WCMi_Printf(WCMi_RText_WmAsyncError, "WM_PowerOn", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_POWER_OFF:
            WCMi_Printf(WCMi_RText_WmAsyncError, "WM_PowerOff", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_SET_LIFETIME:
            WCMi_Printf(WCMi_RText_WmAsyncError, "WM_SetLifeTime", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_SET_BEACON_IND:
            WCMi_Printf(WCMi_RText_WmAsyncError, "WM_SetBeaconIndication", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_SET_WEPKEY_EX:
            WCMi_Printf(WCMi_RText_WmAsyncError, "WM_SetWEPKeyEx", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;
        }
#endif
        WCMi_SetPhase(WCM_PHASE_IRREGULAR);       // ARM7 �łȂ�炩�̗��R�ŃG���[
        WCMi_Notify(WCM_RESULT_FAILURE, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);     // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbScanEx

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_StartScanEx �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbScanEx(void* arg)
{
    WMStartScanExCallback*  cb = (WMStartScanExCallback*)arg;
    WMErrCode   wmResult = WM_ERRCODE_MAX;
    WCMWork*    wcmw = WCMi_GetSystemWork();

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        // �����T���J�n���̏ꍇ�́A����Ɏ����T�����[�h�ɓ��������Ƃ�ʒm
        if (wcmw->phase == WCM_PHASE_IDLE_TO_SEARCH)
        {
            WCMi_SetPhase(WCM_PHASE_SEARCH);

            /* ���̒ʒm���ɂĎ����T����~��v�������ꍇ���z�肳��� */
            WCMi_Notify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
        }

        // ���̃X�e�b�v�Ƃ��čs���ׂ�����������
        switch (wcmw->phase)
        {
        case WCM_PHASE_SEARCH:
            wcmw->notifyId = WCM_NOTIFY_FOUND_AP;
            if (cb->state == WM_STATECODE_PARENT_FOUND)
            {
                // AP ���𔭌������ꍇ�A�����̃��X�g��ҏW���A�v���P�[�V�����ɒʒm
                s32 i;

                DC_InvalidateRange(wcmw->wm.scanExParam.scanBuf, wcmw->wm.scanExParam.scanBufSize);
                for (i = 0; i < cb->bssDescCount; i++)
                {
                    WCMi_EntryApList((WCMBssDesc*)cb->bssDesc[i], cb->linkLevel[i]);

                    /*
                     * IW ��I/O �����킹��ׂɉ��L�̂悤�ȃp�����[�^�\���ɂ��Ă��邪�A
                     * cb �� i ���p�����[�^�Ɏ������ق����Ó��Ǝv����B
                     */
                    WCMi_NotifyEx(WCM_NOTIFY_FOUND_AP, WCM_RESULT_SUCCESS, cb->bssDesc[i], (void*)cb, __LINE__);
                }
            }

            // �w�肳�ꂽ�`�����l�����ꏄ�������𔻒�
            if ((wcmw->option & WCM_OPTION_MASK_ROUNDSCAN) == WCM_OPTION_ROUNDSCAN_NOTIFY)
            {
                u32 channels = MATH_CountPopulation(wcmw->option & WCM_OPTION_FILTER_CHANNEL);

                if (channels > 0)
                {
                    if ((wcmw->wm.scanCount % channels) == 0)
                    {
                        // �e�`�����l���̃X�L�������ꏄ�������Ƃ�ʒm
                        WCMi_NotifyEx(WCM_NOTIFY_SEARCH_AROUND, WCM_RESULT_SUCCESS, (void*)(wcmw->wm.scanCount), 0, __LINE__);
                    }
                }
            }

            // ���̃`�����l���̃X�L�����J�n�v���𔭍s
            wcmw->wm.scanExParam.channelList = (u16) ((0x0001 << WCMi_GetNextScanChannel((u16) (32 - MATH_CountLeadingZeros(cb->channelList)))) >> 1);
            DC_StoreRange(wcmw->wm.scanExParam.scanBuf, wcmw->wm.scanExParam.scanBufSize);
            wcmw->wm.scanCount++;
            wmResult = WM_StartScanEx(WcmWmcbScanEx, &(wcmw->wm.scanExParam));
            break;

        case WCM_PHASE_SEARCH_TO_IDLE:
            // �X�L������~�v���𔭍s
            wmResult = WM_EndScan(WcmWmcbEndScan);
            break;

        case WCM_PHASE_TERMINATING:
            // �����I���V�[�P���X���̏ꍇ�͂����Ń��Z�b�g
            WcmWmReset();
            return;
        }

        // �����I�ȏ������ʂ��m�F
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            break;

        case WM_ERRCODE_FIFO_ERROR:
#if WCM_DEBUG
            switch (wcmw->phase)
            {
            case WCM_PHASE_SEARCH:
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_StartScanEx", wmResult);
                break;

            case WCM_PHASE_SEARCH_TO_IDLE:
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_EndScan", wmResult);
                break;
            }
#endif
            WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
            WCMi_Notify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(WCMi_WText_InvalidWmState);

        /* Don't break here */
        default:
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
            WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
        // �X�L�����v���Ɏ��s�����ꍇ�̓��Z�b�g
        WCMi_Printf(WCMi_RText_WmAsyncError, "WM_StartScanEx", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmWmReset();
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);     // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbEndScan

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_EndScan �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbEndScan(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        WCMi_SetPhase(WCM_PHASE_IDLE);        // �񓯊��V�[�P���X����I��
        WCMi_Notify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
        break;

    case WM_ERRCODE_FAILED:
        // �X�L������~�v���Ɏ��s�����ꍇ�̓��Z�b�g
        WCMi_Printf(WCMi_RText_WmAsyncError, "WM_EndScan", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmWmReset();
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbConnect

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_StartConnectEx �ɑ΂��錋�ʉ����A�y�і����ڑ���̔�ؒf�ʒm
                ���󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbConnect(void* arg)
{
    WMStartConnectCallback*     cb = (WMStartConnectCallback*)arg;
    WMErrCode   wmResult = WM_ERRCODE_MAX;
    WCMWork*    wcmw = WCMi_GetSystemWork();

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        switch (cb->state)
        {
        case WM_STATECODE_DISCONNECTED:                     // ��ؒf�ʒm
        case WM_STATECODE_BEACON_LOST:                      // �ؒf���ꂽ�ꍇ�Ɠ����̏������s��
            switch (wcmw->phase)
            {
            case WCM_PHASE_DCF_TO_IDLE:
                // Auth-ID ���N���A
                wcmw->wm.authId = 0;

            /* Don't break here */
            case WCM_PHASE_IDLE_TO_DCF:
                WCMi_SetPhase(WCM_PHASE_IRREGULAR);
                break;

            case WCM_PHASE_DCF:
                // Auth-ID ���N���A
                wcmw->wm.authId = 0;

                // �ʒm��ʂ�ݒ�
                wcmw->notifyId = WCM_NOTIFY_DISCONNECT;

            /* Don't break here */
            case WCM_PHASE_IRREGULAR:
                WcmWmReset();
                break;

                /* �z��O�̃t�F�[�Y�ł̐ؒf�ʒm�͖������� */
            }
            break;

#if SDK_VERSION_MAJOR > 3 || (SDK_VERSION_MAJOR == 3 && SDK_VERSION_MINOR > 0) || \
        (SDK_VERSION_MAJOR == 3 && SDK_VERSION_MINOR == 0 && SDK_VERSION_RELSTEP >= 20100)

        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:         // ��������̐ؒf�ł͐ؒf�֐����ŏ������Ă���̂ŁA�����Ȃ�
            break;
#endif

        case WM_STATECODE_CONNECT_START:                    // �ڑ��̓r���o�߂Ȃ̂ŁA�����Ȃ�
            break;

        case WM_STATECODE_CONNECTED:                        // �ڑ������ʒm
            if (wcmw->phase == WCM_PHASE_IRREGULAR)
            {
                // �ؒf���ꂽ��ɐڑ������ʒm�������ꍇ�́A���s�Ƃ݂Ȃ����Z�b�g
                WCMi_Printf(WCMi_RText_WmDisconnected, "WM_StartConnectEx");
                WCMi_SetPhase(WCM_PHASE_IDLE_TO_DCF);         // �t�F�[�Y���㏑��
                WcmWmReset();
            }
            else
            {
                // AID �� 1 �` 2007 �܂ł͈̔͂ŗ^������͂�
                if ((WCM_AID_MIN <= cb->aid) && (cb->aid <= WCM_AID_MAX))
                {
                    // Auth-ID ��ޔ�
                    wcmw->wm.authId = cb->aid;

                    // DCM �ʐM���[�h�J�n�v���𔭍s
                    wmResult = WM_StartDCF(WcmWmcbStartDCF, (WMDcfRecvBuf *) (wcmw->recvBuf), WCM_DCF_RECV_BUF_SIZE);
                    switch (wmResult)
                    {
                    case WM_ERRCODE_OPERATING:
                        break;

                    case WM_ERRCODE_FIFO_ERROR:
                        WCMi_Printf(WCMi_RText_WmSyncError, "WM_StartDCF", wmResult);
                        WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
                        WCMi_Notify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), 0, __LINE__);
                        break;

                    case WM_ERRCODE_ILLEGAL_STATE:
                        WCMi_Warning(WCMi_WText_InvalidWmState);

                    /* Don't break here */
                    default:
                        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
                        WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
                    }
                }
                else
                {
                    // �z��O�� AID ���^����ꂽ�ꍇ�́A���s�Ƃ݂Ȃ����Z�b�g
                    WCMi_Printf(WCMi_RText_InvalidAid, "WM_StartConnectEx");
                    WcmWmReset();
                }
            }
            break;

        default:    // �z��O�̃X�e�[�g�R�[�h
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
            WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)(cb->state), __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
        wcmw->wm.wlStatusOnConnectFail = cb->wlStatus;

    case WM_ERRCODE_NO_ENTRY:
    case WM_ERRCODE_INVALID_PARAM:
    case WM_ERRCODE_OVER_MAX_ENTRY:
        // �����ڑ��Ɏ��s�����ꍇ�̓��Z�b�g
        WCMi_Printf(WCMi_RText_WmAsyncError, "WM_StartConnectEx", cb->errcode, cb->wlCmdID, cb->wlResult);
        WCMi_SetPhase(WCM_PHASE_IDLE_TO_DCF);     // IRREGULAR �ɂȂ��Ă���\��������̂Ńt�F�[�Y���㏑��
        WcmWmReset();
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);     // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbDisconnect

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_Disconnect �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbDisconnect(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WCMWork*        wcmw = WCMi_GetSystemWork();

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        if (wcmw->phase == WCM_PHASE_IRREGULAR)
        {
            // �ؒf���ꂽ��ɖ����I�Ȑؒf�v���̊����ʒm�������ꍇ�A�O�̂��߃��Z�b�g
            WCMi_Printf(WCMi_RText_WmDisconnected, "WM_Disconnect");
            WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
            WcmWmReset();
        }
        else
        {
            // Auth-ID ���N���A
            wcmw->wm.authId = 0;

            WCMi_SetPhase(WCM_PHASE_IDLE);    // �񓯊��V�[�P���X����I��
            WCMi_Notify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
    case WM_ERRCODE_ILLEGAL_STATE:          // �N���e�B�J���ȃ^�C�~���O�ŒʐM���؂ꂽ�ꍇ�ɕԂ��Ă�����
        // �����ڑ��̐ؒf�v�������s�����ꍇ�̓��Z�b�g
        WCMi_Printf(WCMi_RText_WmAsyncError, "WM_Disconnect", cb->errcode, cb->wlCmdID, cb->wlResult);
        WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
        WcmWmReset();
        break;

    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbStartDCF

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_StartDCF �ɑ΂��錋�ʉ����A�y�� DCF ���[�h�ł̃f�[�^��M�ʒm
                ���󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbStartDCF(void* arg)
{
    WMStartDCFCallback*     cb = (WMStartDCFCallback*)arg;
    WCMWork*                wcmw = WCMi_GetSystemWork();

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        switch (cb->state)
        {
        case WM_STATECODE_DCF_START:
            if (wcmw->phase == WCM_PHASE_IRREGULAR)
            {
                // �ؒf���ꂽ��� DCF �J�n�����ʒm�������ꍇ�́A���s�Ƃ݂Ȃ����Z�b�g
                WCMi_Printf(WCMi_RText_WmDisconnected, "WM_StartDCF");
                WCMi_SetPhase(WCM_PHASE_IDLE_TO_DCF); // �t�F�[�Y���㏑��
                WcmWmReset();
            }
            else
            {
                WCMi_SetPhase(WCM_PHASE_DCF);         // �񓯊��V�[�P���X����I��
                WCMi_Notify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
            }
            break;

        case WM_STATECODE_DCF_IND:
            // �d�g���x��ޔ�
            WCMi_ShelterRssi((u8) (cb->recvBuf->rate_rssi >> 8));

            // CPS �Ƃ̃C���^�[�t�F�[�X�� DCF ��M��ʒm
            DC_InvalidateRange(cb->recvBuf, WCM_DCF_RECV_BUF_SIZE);
            WCMi_CpsifRecvCallback(cb->recvBuf->srcAdrs, cb->recvBuf->destAdrs, (u8 *)cb->recvBuf->data, (s32)cb->recvBuf->length);
            break;

        default:    // �z��O�̃X�e�[�g�R�[�h
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
            WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)(cb->state), __LINE__);
        }
        break;

    /* StartDCF �ɂ� WM_ERRCODE_FAILED �͕Ԃ��Ă��Ȃ��̂ŏȗ� */
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);     // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbEndDCF

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_EndDCF �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbEndDCF(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WMErrCode       wmResult = WM_ERRCODE_MAX;
    WCMWork*        wcmw = WCMi_GetSystemWork();

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        if (wcmw->phase == WCM_PHASE_IRREGULAR)
        {
            // �ؒf���ꂽ��� DCF �I�������ʒm�������ꍇ�́A���s�Ƃ݂Ȃ����Z�b�g
            WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
            WcmWmReset();
        }
        else
        {
            // AP �Ƃ̖����ڑ��ؒf�v���𔭍s
            wmResult = WM_Disconnect(WcmWmcbDisconnect, 0);
            switch (wmResult)
            {
            case WM_ERRCODE_OPERATING:
                break;

            case WM_ERRCODE_FIFO_ERROR:
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_Disconnect", wmResult);
                WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
                WCMi_Notify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), 0, __LINE__);
                break;

            case WM_ERRCODE_ILLEGAL_STATE:          // �N���e�B�J���ȃ^�C�~���O�ŒʐM���؂ꂽ�ꍇ
                // �ؒf���鐡�O�� AP ������ؒf���ꂽ�ꍇ�́A���s�Ƃ݂Ȃ����Z�b�g
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_Disconnect", wmResult);
                WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
                WcmWmReset();
                break;

            default:
                WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
                WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
            }
        }
        break;

    case WM_ERRCODE_FAILED:
    case WM_ERRCODE_ILLEGAL_STATE:                  // �N���e�B�J���ȃ^�C�~���O�ŒʐM���؂ꂽ�ꍇ
        // DCF �ʐM���[�h�I���Ɏ��s�����ꍇ�̓��Z�b�g
        WCMi_Printf(WCMi_RText_WmAsyncError, "WM_EndDCF", cb->errcode, cb->wlCmdID, cb->wlResult);
        WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);         // IRREGULAR �ɂȂ��Ă���\��������̂Ńt�F�[�Y���㏑��
        WcmWmReset();
        break;

    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);         // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbReset

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_Reset �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbReset(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WMErrCode       wmResult = WM_ERRCODE_MAX;
    WCMWork*        wcmw = WCMi_GetSystemWork();

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        // ���Z�b�g�d���Ăяo���Ǘ��t���O���N���A
        wcmw->resetting = WCM_RESETTING_OFF;

        // Auth-ID ���N���A
        wcmw->wm.authId = 0;

        switch (wcmw->phase)
        {
        case WCM_PHASE_IDLE_TO_SEARCH:  // AP �����T���J�n��
        case WCM_PHASE_SEARCH:          // AP �����T����
            // �X�L���������Ɏ��s���ă��Z�b�g���邱�ƂɂȂ����|��ʒm
            WCMi_SetPhase(WCM_PHASE_IDLE);
            WCMi_Notify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
            break;

        case WCM_PHASE_SEARCH_TO_IDLE:  // AP �����T����~��
            // �v������ AP �����T����~�����������������Ƃ�ʒm
            WCMi_SetPhase(WCM_PHASE_IDLE);
            WCMi_Notify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
            break;

        case WCM_PHASE_IDLE_TO_DCF:     // �ڑ���
            // �ڑ������Ɏ��s���ă��Z�b�g���邱�ƂɂȂ����|��ʒm
            {
                u16 wlStatus = wcmw->wm.wlStatusOnConnectFail;

                wcmw->wm.wlStatusOnConnectFail = 0x0000;
#ifdef WCM_CAMOUFLAGE_RATESET
                /* ���[�g�Z�b�g�̕s�����ɂ��ڑ��Ɏ��s�������̑΍� */
                if (wlStatus == WCM_CONNECT_STATUSCODE_ILLEGAL_RATESET)
                {
                    if ((wcmw->bssDesc.rateSet.support & WCM_CAMOUFLAGE_RATESET) != WCM_CAMOUFLAGE_RATESET)
                    {
                        WCMi_Printf(WCMi_RText_SupportRateset, wcmw->bssDesc.ssid);
                        wcmw->bssDesc.rateSet.support |= WCM_CAMOUFLAGE_RATESET;

                        // �����I�ɐڑ������g���C
                        wmResult = WM_StartConnectEx(WcmWmcbConnect, (WMBssDesc*)&(wcmw->bssDesc), NULL,
                                                     (BOOL)((wcmw->option & WCM_OPTION_MASK_POWER) == WCM_OPTION_POWER_ACTIVE ? FALSE : TRUE),
                                                     (u16) ((wcmw->option & WCM_OPTION_MASK_AUTH) == WCM_OPTION_AUTH_SHAREDKEY ?
                                                                     WM_AUTHMODE_SHARED_KEY : WM_AUTHMODE_OPEN_SYSTEM
                                                                     ));

                        // �����I�ȏ������ʂ��m�F
                        switch (wmResult)
                        {
                        case WM_ERRCODE_OPERATING:
                            break;

                        case WM_ERRCODE_FIFO_ERROR:
                            WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
                            WCMi_Notify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), (void*)wlStatus, __LINE__);
                            break;

                        case WM_ERRCODE_ILLEGAL_STATE:
                            WCMi_Warning(WCMi_WText_InvalidWmState);

                        /* Don't break here */
                        default:
                            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
                            WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)wlStatus, __LINE__);
                        }

                        return;
                    }
                }
#endif
                WCMi_SetPhase(WCM_PHASE_IDLE);
                WCMi_Notify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), (void*)wlStatus, __LINE__);
            }
            break;

        case WCM_PHASE_DCF:         // DCF �ʐM��
        case WCM_PHASE_IRREGULAR:
            // AP ����ؒf���ꂽ���Ƃ�ʒm
            WCMi_SetPhase(WCM_PHASE_IDLE);
            WCMi_Notify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), (void*)1, __LINE__);

            break;

        case WCM_PHASE_DCF_TO_IDLE: // �ؒf��
            // �v�������ؒf�����������������Ƃ�ʒm
            WCMi_SetPhase(WCM_PHASE_IDLE);
            WCMi_Notify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
            break;

        case WCM_PHASE_TERMINATING: // �����I����
            // ���Z�b�g�̊����ɑ����āAPHASE_WAIT �ɖ߂��悤�ɘA���I�ɏ������p��
            wmResult = WM_PowerOff(WcmWmcbCommon);
            switch (wmResult)
            {
            case WM_ERRCODE_OPERATING:
                break;

            case WM_ERRCODE_FIFO_ERROR:
                WCMi_Printf(WCMi_RText_WmSyncError, "WM_Reset", wmResult);
                WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
                WCMi_Notify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
                break;

            case WM_ERRCODE_ILLEGAL_STATE:
                WCMi_Warning(WCMi_WText_InvalidWmState);

            /* Don't break here */
            default:
                WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
                WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
            }
            break;

        default:
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);     // �z��͈͊O�̃G���[
            WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, (void*)(wcmw->phase), __LINE__);
        }
        break;

    default:
        /* ���Z�b�g�Ɏ��s�����ꍇ�͕����s�\ */
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);         // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmEditScanExParam

  Description:  WM ���C�u�����ɃX�L�������w������ۂ̃X�L�����ݒ�\���̂�ҏW����B

  Arguments:    bssid   -   �T������ BSSID �t�B���^�[�BWCM_BSSID_ANY �͑S�� 0xff
                            �ł��� BSSID �Ȃ̂ŁA�t�B���^�[���Ȃ��ݒ�ƂȂ�B
                essid   -   �T������ ESSID �t�B���^�[�BWCM_ESSID_ANY ���w�肷���
                            ���� 0 �� ESSID �Ƃ݂Ȃ���A�t�B���^�[���Ȃ��ݒ�ƂȂ�B
                option  -   �I�v�V�����ύX�p�ϐ��B�ύX���s���� WCM ���C�u��������
                            ���ʎg�p����� option ���ς���āA���ɂ͖߂�Ȃ��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmEditScanExParam(const void* bssid, const void* essid, u32 option)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    (void)WCM_UpdateOption(option);
    wcmw->wm.scanExParam.scanBuf = (WMBssDesc*)wcmw->recvBuf;
    wcmw->wm.scanExParam.scanBufSize = WM_SIZE_SCAN_EX_BUF;
    wcmw->wm.scanExParam.channelList = (u16) ((0x0001 << WCMi_GetNextScanChannel(0)) >> 1);
    wcmw->wm.scanExParam.maxChannelTime = (u16) ((wcmw->maxScanTime != 0) ? wcmw->maxScanTime : WM_GetDispersionScanPeriod());
    wcmw->wm.scanExParam.scanType = (u16) (((wcmw->option & WCM_OPTION_MASK_SCANTYPE) == WCM_OPTION_SCANTYPE_ACTIVE) ? WM_SCANTYPE_ACTIVE : WM_SCANTYPE_PASSIVE);
    if (bssid == NULL)
    {
        MI_CpuCopy8(WCM_Bssid_Any, wcmw->wm.scanExParam.bssid, WCM_BSSID_SIZE);
    }
    else
    {
        MI_CpuCopy8(bssid, wcmw->wm.scanExParam.bssid, WCM_BSSID_SIZE);
    }

    //MI_CpuCopy8( bssid , wcmw->wm.scanExParam.bssid , WCM_BSSID_SIZE );
    if ((essid == NULL) || (essid == (void*)WCM_Essid_Any))
    {
        MI_CpuCopy8(WCM_Essid_Any, wcmw->wm.scanExParam.ssid, WCM_ESSID_SIZE);
        wcmw->wm.scanExParam.ssidLength = 0;
    }
    else
    {
        s32 len;

        MI_CpuCopy8(essid, wcmw->wm.scanExParam.ssid, WCM_ESSID_SIZE);
        for (len = 0; len < WCM_ESSID_SIZE; len++)
        {
            if (((u8*)essid)[len] == '\0')
            {
                break;
            }
        }

        wcmw->wm.scanExParam.ssidLength = (u16) len;
    }

    wcmw->wm.scanCount = 0;
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbSetDCFData

  Description:  DCF �t���[�����M�v���ɑ΂��閳���h���C�o����̔񓯊��I�ȉ�����
                �󂯎��R�[���o�b�N�֐��B

  Arguments:    arg -   �����h���C�o����̉����p�����[�^���i�[����Ă���o�b�t�@
                        �̃A�h���X���n�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbSetDCFData(void* arg)
{
    WMCallback*             cb = (WMCallback*)arg;
    volatile WCMCpsifWork*  wcmiw = WCMi_GetCpsifWork();

    switch (cb->apiid)
    {
    case WM_APIID_SET_DCF_DATA:
        wcmiw->sendResult = (u16) (cb->errcode);
        if (cb->errcode == WM_ERRCODE_SUCCESS)
        {
            WCMi_ResetKeepAliveAlarm();
        }

        OS_WakeupThread((OSThreadQueue*)&(wcmiw->sendQ));
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbSetDCFDataAsync

  Description:  DCF �t���[�����M�v���ɑ΂��閳���h���C�o����̔񓯊��I�ȉ�����
                �󂯎��R�[���o�b�N�֐��B

  Arguments:    arg -   �����h���C�o����̉����p�����[�^���i�[����Ă���o�b�t�@
                        �̃A�h���X���n�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbSetDCFDataAsync(void* arg)
{
    WMCallback*             cb = (WMCallback*)arg;
    volatile WCMCpsifWork*  wcmiw = WCMi_GetCpsifWork();
    s32                     wcmResult = WCM_RESULT_SUCCESS;
    WCMSendInd              sendCallback = NULL;

    switch (cb->apiid)
    {
    case WM_APIID_SET_DCF_DATA:
        wcmiw->sendResult = (WMErrCode) (cb->errcode);
        if (cb->errcode == WM_ERRCODE_SUCCESS)
        {
            WCMi_ResetKeepAliveAlarm();
        }

        /*
         * �R�[���o�b�N���Ăяo���ꂽ���_�ŁAWCM���C�u�����̏�Ԃ͕s���B
         * WCM ���C�u�������I������Ă��Ă��A���M���ʂ͕ێ�����Ă���̂�
         * WCM ���C�u�����̏�Ԃ��m�F����K�v�͂Ȃ��B
         */
        switch (wcmiw->sendResult)
        {
        case WM_ERRCODE_SUCCESS:
            // ����I��
            wcmResult = WCM_RESULT_SUCCESS;
            break;

        case WM_ERRCODE_FAILED:
        default:
            // ���M�Ɏ��s�����ꍇ
            wcmResult = WCM_CPSIF_RESULT_SEND_FAILURE;
            WCMi_Printf(WCMi_RText_WmAsyncError, "WCM_SendDCFData", wcmiw->sendResult);
        }

        sendCallback = wcmiw->sendCallback;
        wcmiw->sendCallback = NULL;
        WCMi_CpsifUnlockMutexInIRQ((OSMutex*)&(wcmiw->sendMutex));

        SDK_POINTER_ASSERT(sendCallback);
        
        // ���M�����R�[���o�b�N��ʒm
        sendCallback(wcmResult);
        break;
    }
}
