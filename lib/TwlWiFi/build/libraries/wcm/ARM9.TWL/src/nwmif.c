/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - WCM - libraries
  File:     nwmif.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-24#$
  $Rev: 1187 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/

#include "wcm_private.h"
#include "wcm_message.h"
#include <twl/nwm.h>

/* DWC �ɒʒm���ׂ��ڑ����s���R */
#define WCM_NWMIF_STATECODE_AUTH_ERROR     13
#define WCM_NWMIF_STATECODE_KEY_ERROR      15
#define WCM_NWMIF_STATECODE_CAPACITY_ERROR 17

#include <twl/ltdmain_begin.h>
/*---------------------------------------------------------------------------*
    �����֐��v���g�^�C�v
 *---------------------------------------------------------------------------*/
static void         WcmNwmReset(void);
static void         WcmEditScanExParam(const void* bssid, const void* essid, u32 option);

static void         WcmNwmcbCommon(void* arg);
static void         WcmNwmcbScanEx(void* arg);
static void         WcmNwmcbConnect(void* arg);
static void         WcmNwmcbDisconnect(void* arg);
static void         WcmNwmcbSetRecvFrmBuf(void* arg);
static void         WcmNwmcbUnsetRecvFrmBuf(void* arg);
static void         WcmNwmcbReset(void* arg);
static void         WcmNwmcbSetDCFData(void* arg);
static void         WcmNwmcbSetDCFDataAsync(void* arg);
static void         WcmKACallback(void* arg);
static void         WcmNwmcbSetQos(void* arg);

static OSMessage mesgBuffer[10];
static OSMessageQueue mesgQueue;

/*---------------------------------------------------------------------------*
  Name:         WCM_GetWPAPSK

  Description:  WPA�p�X�t���[�Y�Ɛڑ��Ώۂ� SSID ���� WPA-PSK(���O�F�،�)�𐶐�
                �� PSK �̌v�Z�͐ڑ��ݒ莞�ɂ̂ݍs���A�ڑ��ɂ͌v�Z�ς݂� PSK ���g�p����

  Arguments:    passphrase   - WPA �p�X�t���[�Y�A8 �����ȏ� 63 �����ȉ��� ASCII ������
                ssid         - SSID
                ssidlen      - SSID �̒���(32�����̕K�v����)
                psk          - �������ꂽ PSK �̊i�[��B32Byte

  Returns:      BOOL         - �����̉ہB
                               �p�X�t���[�Y�����قȂ�ꍇ�����s�ƂȂ�
 *---------------------------------------------------------------------------*/

BOOL    WCM_GetWPAPSK(const u8 passphrase[WCM_WPA_PASSPHRASE_LENGTH_MAX], const u8 *ssid, u8 ssidlen, u8 psk[WCM_WPA_PSK_SIZE])
{
    int passLength = STD_GetStringLength ((char*)passphrase);
    u8  zero[WCM_WPA_PSK_SIZE];

    /* �p�X�t���[�Y�̕����񒷊m�F */
    if (passLength < 8 || passLength > 63)
    {
        return FALSE;
    }

    MI_CpuClear8(psk, WCM_WPA_PSK_SIZE);
    MI_CpuClear8(zero, WCM_WPA_PSK_SIZE);

    NWM_Passphrase2PSK(passphrase, ssid, ssidlen, psk);

    if( MI_CpuComp8(psk, zero, WCM_WPA_PSK_SIZE) == 0 ) /* psk �̐��������̂͐ڑ����Ă݂Ȃ��Ɣ��ʕs�\ */
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_NwmifStartupAsync

  Description:  �����@�\�̋N���V�[�P���X���J�n

  Arguments:    None.

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_NwmifStartupAsync(void)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    NWMRetCode  nwmResult;
    
    SDK_POINTER_ASSERT(wcmw);

    // NWM ���C�u����������
    nwmResult = NWM_Init(wcmw->nwm.work, sizeof(wcmw->nwm.work), (u8)wcmw->config.dmano);
    switch (nwmResult)
    {
    case NWM_RETCODE_SUCCESS:
        break;

    case NWM_RETCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // NWM ���C�u���������ɏ���������Ă���

    case NWM_RETCODE_NWM_DISABLE:
        WCMi_Warning(WCMi_WText_UnsuitableArm7);
        return WCM_RESULT_WMDISABLE;            // ARM7 ���� NWM ���C�u���������삵�Ă��Ȃ�

    case NWM_RETCODE_INVALID_PARAM:
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // �z��͈͊O�̃G���[
    }

    wcmw->nwm.pseudoStatusOnConnectFail = 0x0000; // �^�� 802.11 STATE �R�[�h�����������Ă���

    // �����n�[�h�E�F�A�g�p���v���𔭍s
    nwmResult = NWM_LoadDevice(WcmNwmcbCommon);
    switch (nwmResult)
    {
    case NWM_RETCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_WAIT_TO_IDLE);    // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
        wcmw->notifyId = WCM_NOTIFY_STARTUP;
        break;

    case NWM_RETCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "NWM_LoadDevice", nwmResult);
        WCMi_SetPhase(WCM_PHASE_IRREGULAR);
        return WCM_RESULT_FAILURE;              // ARM7 �ւ̗v�����s�Ɏ��s

    case NWM_RETCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // �z��͈͊O�̃G���[
    }
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_NwmifCleanupAsync

  Description:  �����@�\�̒�~�V�[�P���X���J�n

  Arguments:    None.

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_NwmifCleanupAsync(void)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    NWMRetCode  nwmResult;

    SDK_POINTER_ASSERT(wcmw);

    // �����n�[�h�E�F�A�̃V���b�g�_�E���v���𔭍s
    nwmResult = NWM_Close(WcmNwmcbCommon);
    switch (nwmResult)
    {
    case NWM_RETCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_IDLE_TO_WAIT);    // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
        wcmw->notifyId = WCM_NOTIFY_CLEANUP;
        break;

    case NWM_RETCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "NWM_Close", nwmResult);
        return WCM_RESULT_REJECT;               // ARM7 �ւ̗v�����s�Ɏ��s( ���g���C�\ )

    case NWM_RETCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // �z��͈͊O�̃G���[
    }
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_NwmifBeginSearchAsync

  Description:  AP �����T���J�n�V�[�P���X���J�n�A�������͒T�������ύX

  Arguments:    bssid        - 
                essid        - 
                option       - 

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_NwmifBeginSearchAsync(const void* bssid, const void* essid, u32 option)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    NWMRetCode  nwmResult;

    SDK_POINTER_ASSERT(wcmw);

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
    DC_InvalidateRange(wcmw->nwm.scanExParam.scanBuf, wcmw->nwm.scanExParam.scanBufSize);
    wcmw->nwm.scanCount++;
    nwmResult = NWM_StartScanEx(WcmNwmcbScanEx, &(wcmw->nwm.scanExParam));
    switch (nwmResult)
    {
    case NWM_RETCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_IDLE_TO_SEARCH);  // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
        wcmw->notifyId = WCM_NOTIFY_BEGIN_SEARCH;
        break;

    case NWM_RETCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "NWM_StartScanEx", nwmResult);
        return WCM_RESULT_REJECT;               // ARM7 �ւ̗v�����s�Ɏ��s( ���g���C�\ )

    case NWM_RETCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;          // �z��͈͊O�̃G���[
    }
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_NwmifConnectAsync

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
s32 WCMi_NwmifConnectAsync(void)
{
    s32         wcmResult   = WCM_RESULT_SUCCESS;
    WCMWork*    wcmw        = WCMi_GetSystemWork();

    SDK_POINTER_ASSERT(wcmw);

    // begins the AP wireless connection sequence
    {
        NWMRetCode  nwmResult;

        switch(wcmw->privacyDesc.mode)
        {
            case WCM_PRIVACYMODE_NONE:
            case WCM_PRIVACYMODE_WEP40:
            case WCM_PRIVACYMODE_WEP104:
            case WCM_PRIVACYMODE_WEP128:
            /* WEP �ł̐ڑ��̏ꍇ */
            {
                u16 authMode = NWM_AUTHMODE_OPEN;
                
                if ((wcmw->option & WCM_OPTION_MASK_AUTH) == WCM_OPTION_AUTH_SHAREDKEY)
                {
                    authMode = NWM_AUTHMODE_SHARED;
                }

                nwmResult = NWM_SetWEPKey(WcmNwmcbCommon, 
                                                wcmw->privacyDesc.mode,
                                                wcmw->privacyDesc.keyId,
                                                wcmw->privacyDesc.key,
                                                authMode);
            }
            break;

            case WCM_PRIVACYMODE_WPA_TKIP:
            case WCM_PRIVACYMODE_WPA2_TKIP:
            case WCM_PRIVACYMODE_WPA_AES:
            case WCM_PRIVACYMODE_WPA2_AES:
            /* WPA �ł̐ڑ��̏ꍇ */
            {
                NWMWpaParam wpaParam;
                MI_CpuCopy8(wcmw->privacyDesc.key, wpaParam.psk, NWM_WPA_PSK_LENGTH);
                wpaParam.auth = wcmw->privacyDesc.mode;

                nwmResult = NWM_SetWPAPSK(WcmNwmcbCommon, &wpaParam);
            }
                break;

            default:
                /* ���m�̈Í��������͖��� */
                break;
        }

        switch (nwmResult) // miz
        {
        case NWM_RETCODE_OPERATING: // miz
            WCMi_SetPhase(WCM_PHASE_IDLE_TO_DCF); // asynchronous sequence start successful
            wcmw->notifyId = WCM_NOTIFY_CONNECT;
            break;

        case NWM_RETCODE_FIFO_ERROR: // miz
            WCMi_Printf(WCMi_RText_WmSyncError, "NWM_SetWEPKey or NWM_SetWPAPSK", nwmResult); // miz
            return WCM_RESULT_REJECT;           // request to ARM7 failed (retry possible)

        case NWM_RETCODE_ILLEGAL_STATE: // miz
            WCMi_Warning(WCMi_WText_InvalidWmState); // miz
        /* Don't break here*/
        default:
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
            return WCM_RESULT_FATAL_ERROR;      // error outside of anticipated range
        }
    }

    // normal end
    return WCM_RESULT_ACCEPT;
}


/*---------------------------------------------------------------------------*
  Name:         WCMi_NwmifDisconnectAsync

  Description:  

  Arguments:    None.

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_NwmifDisconnectAsync(void)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    NWMRetCode  nwmResult;

    SDK_POINTER_ASSERT(wcmw);

    // DCF �ʐM���[�h�I���v���𔭍s
    nwmResult = NWM_UnsetReceivingFrameBuffer(WcmNwmcbUnsetRecvFrmBuf, NWM_PORT_IPV4_ARP);
    switch (nwmResult)
    {
    case NWM_RETCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
        wcmw->notifyId = WCM_NOTIFY_DISCONNECT;
        break;

    case NWM_RETCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "NWM_UnsetReceivingFrameBuffer", nwmResult);
        return WCM_RESULT_REJECT;           // ARM7 �ւ̗v�����s�Ɏ��s( ���g���C�\ )

    case NWM_RETCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);
    
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;
    }
    
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_NwmifTerminateAsync

  Description:  

  Arguments:    None.

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_NwmifTerminateAsync(void)
{
    u16         nwmState;
    WCMWork*    wcmw = WCMi_GetSystemWork();
    NWMRetCode  nwmResult;

    SDK_POINTER_ASSERT(wcmw);

    // NWM ���C�u�����̓�����Ԃ𒲍�
    nwmState = NWM_GetState();
    
    // WM ������Ԃɂ���~��ԂɑJ�ڂ���悤�A�����J�n
    switch (nwmState)
    {
    case NWM_STATE_INITIALIZED:
        // WM ���C�u�����I��
        nwmResult = NWM_End();
        if (nwmResult == NWM_RETCODE_SUCCESS)
        {
            WCMi_SetPhase(WCM_PHASE_WAIT);
            wcmw->notifyId = WCM_NOTIFY_COMMON;
            return WCM_RESULT_SUCCESS;      // �����I�� WAIT �t�F�[�Y�ւ̈ڍs������
        }
        break;

    case NWM_STATE_LOADED:
        // �����n�[�h�E�F�A�̎g�p�֎~�v���𔭍s
        nwmResult = NWM_UnloadDevice(WcmNwmcbCommon);
        break;

    case NWM_STATE_DISCONNECTED:
        // �����n�[�h�E�F�A�̃V���b�g�_�E���v���𔭍s
        nwmResult = NWM_Close(WcmNwmcbCommon);
        break;

    case NWM_STATE_INFRA_CONNECTED:
        // NWM �ɂ̓��Z�b�g�֐����p�ӂ���Ă��Ȃ����߁A�ʐM���� Terminate �ł� Disconnect ���Ă�ł���
        nwmResult = NWM_Disconnect(WcmNwmcbDisconnect);
        break;

    default:
        // ���Z�b�g�d���Ăяo���Ǘ��t���O���Z�b�g
        wcmw->resetting = WCM_RESETTING_ON;

        // �����ڑ���Ԃ̃��Z�b�g�v���𔭍s NWM_�ɂ̓��Z�b�g�@�\���Ȃ����ߏ�Ԃ𒼐ڕύX
        nwmResult = NWM_RETCODE_NWM_DISABLE;
    }

    // �v�����s�ɑ΂��铯���I�ȏ������ʂ��m�F
    switch (nwmResult)
    {
    case NWM_RETCODE_OPERATING:
        WCMi_SetPhase(WCM_PHASE_TERMINATING); // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
        wcmw->notifyId = WCM_NOTIFY_TERMINATE;
        break;

    case NWM_RETCODE_FIFO_ERROR:
        WCMi_Printf(WCMi_RText_WmSyncError, "NWM_***", nwmResult);
        return WCM_RESULT_REJECT;           // ARM7 �ւ̗v�����s�Ɏ��s( ���g���C�\ )

    case NWM_RETCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        return WCM_RESULT_FATAL_ERROR;      // �z��͈͊O�̃G���[
    }
    return WCM_RESULT_ACCEPT;
}


/*---------------------------------------------------------------------------*
  Name:         WCMi_NwmifSetDCFData

  Description:  ���M�v�����s

  Arguments:    dstAddr      - 
                buf          - 
                len          - 

  Returns:      s32          - 
 *---------------------------------------------------------------------------*/
s32 WCMi_NwmifSetDCFData(const u8* dstAddr, const u16* buf, u16 len, WCMSendInd callback)
{
    NWMRetCode              nwmResult;
    volatile WCMCpsifWork*  wcmiw = WCMi_GetCpsifWork();

    SDK_POINTER_ASSERT(wcmiw);

    /*
     * The WCM library state is unclear upon returning from the block state.
     * As you can't send unless connected to AP, you need to confirm the WCM
     * library state here.
     */

    if (!callback)
    {
        nwmResult = NWM_SendFrame(WcmNwmcbSetDCFData, (const u8*)dstAddr, buf, len);
    }
    else
    {
        wcmiw->sendCallback = callback;
        nwmResult = NWM_SendFrame(WcmNwmcbSetDCFDataAsync, (const u8*)dstAddr, buf, len);
    }
    
    switch (nwmResult)
    {
    case NWM_RETCODE_OPERATING:
        break;

    case NWM_RETCODE_ILLEGAL_STATE:
    case NWM_RETCODE_INVALID_PARAM:
    case NWM_RETCODE_FIFO_ERROR:
    default:
        // ���M�v���Ɏ��s�����ꍇ
        WCMi_Printf(WCMi_RText_WmSyncError, "NWM_SendFrame", nwmResult);
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
        case NWM_RETCODE_SUCCESS:
            break;

        case NWM_RETCODE_FAILED:
        default:
            // ���M�Ɏ��s�����ꍇ
            WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_SendFrame", wcmiw->sendResult);
            return WCM_CPSIF_RESULT_SEND_FAILURE;       // DCF ���M�Ɏ��s
        }
    }
    // ����I��
    return len;
}


/*---------------------------------------------------------------------------*
  Name:         WCMi_NwmifSendNullPacket

  Description:  �L�[�v�A���C�u NULL �p�P�b�g �𑗐M����B
                ���� DCF �p�P�b�g�𑗐M���̏ꍇ�͉����s��Ȃ��B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_NwmifSendNullPacket(void)
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
    if (NWM_RETCODE_OPERATING != NWM_SendFrame(WcmKACallback, (const u8*)w->bssDesc.bssid, (const u16*)(w->sendBuf),
        0))
    {
        WCMi_CpsifUnlockMutexInIRQ((OSMutex*)&(wcmiw->sendMutex));
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_NwmifGetAllowedChannel

  Description:  TWL �����̎g�p���`�����l�����X�g���擾����B

  Arguments:    None.

  Returns:      u16    - 1�`13ch�̂����A�g�p�������ꂽ�`�����l���Q�̃r�b�g�t�B�[���h��Ԃ��܂��B
                         �ŉ��ʃr�b�g���珇�� 1 �`�����l��, 2 �`�����l���c�c�ɑΉ����܂��B
 *---------------------------------------------------------------------------*/
u16 WCMi_NwmifGetAllowedChannel(void)
{
    /* NWM �ł͍ŉ��ʃr�b�g�� 0ch ���Ӗ�����̂ŁA���V�t�g���Ēʒm���� */
    return (u16)(NWM_GetAllowedChannel()>>1);
}

/*---------------------------------------------------------------------------*
  Name:         WcmKACallback

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
    if (((NWMCallback*)arg)->retcode == NWM_RETCODE_SUCCESS)
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
  Name:         WcmNwmReset

  Description:  NWM �ɂ̓��Z�b�g�@�\���Ȃ����߁AWM_Reset �֐������̏������s��
                WM ���C�u�����̃��Z�b�g�������J�n����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNwmReset(void)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();
    NWMRetCode  nwmResult = NWM_RETCODE_MAX; // miz

    if (wcmw->resetting == WCM_RESETTING_OFF)
    {
        // sets reset duplicate call management flags
        wcmw->resetting = WCM_RESETTING_ON;

        // miz : NWM �Ƀ��Z�b�g�@�\���Ȃ��̂ňȉ�����R�[���o�b�N�����J�n--------------------------

        // ���Z�b�g�d���Ăяo���Ǘ��t���O���N���A
        wcmw->resetting = WCM_RESETTING_OFF;

        // Auth-ID ���N���A
        wcmw->nwm.authId = 0;

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
                u16 wlStatus = wcmw->nwm.pseudoStatusOnConnectFail;
                wcmw->nwm.pseudoStatusOnConnectFail = 0x0000;
                
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
            nwmResult = NWM_Close(WcmNwmcbCommon); // miz
            switch (nwmResult) // miz
            {
            case NWM_RETCODE_OPERATING: // miz
                break;

            case NWM_RETCODE_FIFO_ERROR: // miz
                WCMi_Printf(WCMi_RText_WmSyncError, "NWM_Close", nwmResult);
                WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
                WCMi_Notify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
                break;

            case NWM_RETCODE_ILLEGAL_STATE: // miz
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
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmNwmcbCommon

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_Enable , WM_Disable , WM_PowerOn , WM_PowerOff ,
                WM_SetLifeTime , WM_SetBeaconIndication , WM_SetWEPKeyEx
                �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNwmcbCommon(void* arg)
{
    NWMCallback*    cb = (NWMCallback*)arg;
    NWMRetCode      nwmResult = NWM_RETCODE_MAX;
    WCMWork*        wcmw = WCMi_GetSystemWork();

    switch (cb->retcode)
    {
    case NWM_RETCODE_SUCCESS:
        // ���̃X�e�b�v�Ƃ��čs���ׂ�����������
        switch (cb->apiid)
        {
        case NWM_APIID_LOAD_DEVICE:
            // �����n�[�h�E�F�A�ւ̓d�͋����v���𔭍s
            nwmResult = NWM_Open(WcmNwmcbCommon);
            break;

        case NWM_APIID_UNLOAD_DEVICE:
            // NWM ���C�u�����I��
            nwmResult = NWM_End();
            switch (nwmResult)
            {
            case NWM_RETCODE_SUCCESS:
                WCMi_SetPhase(WCM_PHASE_WAIT);        // �񓯊��V�[�P���X����I��
                WCMi_Notify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
                break;

            case NWM_RETCODE_NWM_DISABLE:
                WCMi_Warning(WCMi_WText_InvalidWmState);

            /* Don't break here */
            default:
                WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
                WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
            }

            return; // ���̃X�e�b�v�͂Ȃ��̂ł����ŏI��

        case NWM_APIID_OPEN:
             {
                u8 macAddress[NWM_SIZE_MACADDR];
                OS_GetMacAddress(macAddress);
                WCMi_CpsifSetMacAddress(macAddress);
            }

            WCMi_SetPhase(WCM_PHASE_IDLE);    // �񓯊��V�[�P���X����I��
            WCMi_Notify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
            return; // ���̃X�e�b�v�͂Ȃ��̂ł����ŏI��

        case NWM_APIID_CLOSE:
            // �����n�[�h�E�F�A�̎g�p�֎~�v���𔭍s
            nwmResult = NWM_UnloadDevice(WcmNwmcbCommon);
            break;

        case NWM_APIID_SET_WEPKEY:
        case NWM_APIID_SET_WPA_PSK:
            nwmResult = NWM_SetPowerSaveMode(WcmNwmcbCommon, 
                                             ((wcmw->option & WCM_OPTION_MASK_POWER) == WCM_OPTION_POWER_ACTIVE ? WCM_NWM_POWER_SAVE_OFF : WCM_NWM_POWER_SAVE_ON));
            break;

        case NWM_APIID_SET_PS_MODE:

            // DCF �ʐM���[�h�J�n�v���𔭍s
            nwmResult = NWM_SetReceivingFrameBuffer(WcmNwmcbSetRecvFrmBuf, wcmw->recvBuf, sizeof(wcmw->recvBuf), NWM_PORT_IPV4_ARP);
            break;

        }

        // �����I�ȏ������ʂ��m�F
        switch (nwmResult)
        {
        case NWM_RETCODE_OPERATING:
            break;

        case NWM_RETCODE_FIFO_ERROR:
#if WCM_DEBUG
            switch (cb->apiid)
            {
            case NWM_APIID_LOAD_DEVICE:
                WCMi_Printf(WCMi_RText_WmSyncError, "NWM_Open", nwmResult);
                break;

            case NWM_APIID_CLOSE:
                WCMi_Printf(WCMi_RText_WmSyncError, "NWM_UnloadDevice", nwmResult);
                break;

            case NWM_APIID_SET_WEPKEY:
                WCMi_Printf(WCMi_RText_WmSyncError, "NWM_Connect", nwmResult);
                break;

            case NWM_APIID_SET_RECEIVING_BUF:
                WCMi_Printf(WCMi_RText_WmSyncError, "NWM_SetReceivingFrameBuffer", nwmResult);
                WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
                WCMi_Notify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), 0, __LINE__);
                break;
            }
#endif
            WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
            WCMi_Notify(WCM_RESULT_FAILURE, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
            break;

        case NWM_RETCODE_ILLEGAL_STATE:
            WCMi_Warning(WCMi_WText_InvalidWmState);

        /* Don't break here */
        default:
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
            WCMi_Notify(WCM_RESULT_FATAL_ERROR, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
        }
        break;

    case NWM_RETCODE_FAILED:
#if WCM_DEBUG
        switch (cb->apiid)
        {
        case NWM_APIID_LOAD_DEVICE:
            WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_LoadDevice", cb->retcode); //, cb->wlCmdID, cb->wlResult);
            break;

        case NWM_APIID_UNLOAD_DEVICE:
            WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_UnloadDevice", cb->retcode); //, cb->wlCmdID, cb->wlResult);
            break;

        case NWM_APIID_OPEN:
            WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_Open", cb->retcode); //, cb->wlCmdID, cb->wlResult);
            break;

        case NWM_APIID_CLOSE:
            WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_Close", cb->retcode); //, cb->wlCmdID, cb->wlResult);
            break;

        case NWM_APIID_SET_WEPKEY:
            WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_SetWEPKey", cb->retcode); //, cb->wlCmdID, cb->wlResult);
            break;

        }
#endif
        WCMi_SetPhase(WCM_PHASE_IRREGULAR);       // ARM7 �łȂ�炩�̗��R�ŃG���[
        WCMi_Notify(WCM_RESULT_FAILURE, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
        break;

    case NWM_RETCODE_SDIO_ERROR:    // �������W���[���Ƃ̒ʐM�Ɏ��s (��� LoadDevice �̎��s�ȂǂŔ���)
        WCMi_Printf("SDIO Error: APIID=%02X, ERROR_CODE=%02X\n", cb->apiid, cb->retcode);
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);     // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
        break;
        
    case NWM_RETCODE_ILLEGAL_STATE:
    case NWM_RETCODE_NWM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */

    case NWM_RETCODE_FATAL_ERROR:  //�������W���[�����݂̃\�t�g�E�F�A�ł̑Ή����s�\�ȃG���[
    default:
        WCMi_Printf("%s is received fatal error asyncronously: APIID=%02X, ERROR_CODE=%02X\n", __func__, cb->apiid, cb->retcode);
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);     // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
    }

}

/*---------------------------------------------------------------------------*
  Name:         WcmNwmcbScanEx

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_StartScanEx �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNwmcbScanEx(void* arg)
{
    NWMStartScanCallback*  cb = (NWMStartScanCallback*)arg;
    NWMRetCode   nwmResult = NWM_RETCODE_MAX;
    WCMWork*    wcmw = WCMi_GetSystemWork();

    switch (cb->retcode)
    {
    case NWM_RETCODE_SUCCESS:
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
            if (cb->bssDescCount != 0) // miz : NWM �� state �͂Ȃ����A�������� AP ���͕�����
            {
                // AP ���𔭌������ꍇ�A�����̃��X�g��ҏW���A�v���P�[�V�����ɒʒm
                s32 i;

                DC_InvalidateRange(wcmw->nwm.scanExParam.scanBuf, wcmw->nwm.scanExParam.scanBufSize);
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
                    if ((wcmw->nwm.scanCount % channels) == 0)
                    {
                        // �e�`�����l���̃X�L�������ꏄ�������Ƃ�ʒm
                        WCMi_NotifyEx(WCM_NOTIFY_SEARCH_AROUND, WCM_RESULT_SUCCESS, (void*)(wcmw->nwm.scanCount), 0, __LINE__);
                    }
                }
            }

            // ���̃`�����l���̃X�L�����J�n�v���𔭍s
            wcmw->nwm.scanExParam.channelList = (u16) ((0x0001 << WCMi_GetNextScanChannel((u16) (32 - MATH_CountLeadingZeros(cb->channelList)))) >> 1);
            DC_StoreRange(wcmw->nwm.scanExParam.scanBuf, wcmw->nwm.scanExParam.scanBufSize);
            wcmw->nwm.scanCount++;
            nwmResult = NWM_StartScanEx(WcmNwmcbScanEx, &(wcmw->nwm.scanExParam));
            break;

        case WCM_PHASE_SEARCH_TO_IDLE:
            // �X�L������~�v���𔭍s
            nwmResult = NWM_RETCODE_OPERATING; // miz : ���L switch �Ő���I�������邽�߂̃_�~�[
            WCMi_SetPhase(WCM_PHASE_IDLE); // miz : SCAN �t�F�[�Y���� IDLE �t�F�[�Y�ֈڍs
            WCMi_Notify(WCM_RESULT_SUCCESS, 0, 0, __LINE__); // miz : ����ŁA�X�L������~�̃R�[���o�b�N���Ă΂��B���̃R�[���o�b�N���󂯂Đڑ������ɑJ�ڂ���ׂ��B
            break;

        case WCM_PHASE_TERMINATING:
            // �����I���V�[�P���X���̏ꍇ�͂����Ń��Z�b�g
            WcmNwmReset();
            return;
        }

        // �����I�ȏ������ʂ��m�F
        switch (nwmResult)
        {
        case NWM_RETCODE_OPERATING:
            break;

        case NWM_RETCODE_FIFO_ERROR:
#if WCM_DEBUG
            switch (wcmw->phase)
            {
            case WCM_PHASE_SEARCH:
                WCMi_Printf(WCMi_RText_WmSyncError, "NWM_StartScanEx", nwmResult);
                break;

            case WCM_PHASE_SEARCH_TO_IDLE:// miz : NWM_EndScan ���Ȃ��̂ŃX���[
                break;
            }
#endif
            WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
            WCMi_Notify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
            break;

        case NWM_RETCODE_ILLEGAL_STATE:
            WCMi_Warning(WCMi_WText_InvalidWmState);

        /* Don't break here */
        default:
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
            WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
        }
        break;

    case NWM_RETCODE_FAILED:
        // �X�L�����v���Ɏ��s�����ꍇ�̓��Z�b�g
        WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_StartScanEx", cb->retcode); //, cb->wlCmdID, cb->wlResult);
        WcmNwmReset();
        break;

    case NWM_RETCODE_ILLEGAL_STATE:
    case NWM_RETCODE_NWM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);     // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmNwmcbConnect

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_StartConnectEx �ɑ΂��錋�ʉ����A�y�і����ڑ���̔�ؒf�ʒm
                ���󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNwmcbConnect(void* arg)
{
    NWMConnectCallback*     cb = (NWMConnectCallback*)arg;
    NWMRetCode  nwmResult = NWM_RETCODE_MAX;
    WCMWork*    wcmw = WCMi_GetSystemWork();

    u16         nwmState =NWM_GetState();

    switch (cb->retcode)
    {
    case NWM_RETCODE_SUCCESS:
        switch (nwmState)
        {
        case NWM_STATE_INFRA_CONNECTED:                        // �ڑ������ʒm
            if (wcmw->phase == WCM_PHASE_IRREGULAR)
            {
                // �ؒf���ꂽ��ɐڑ������ʒm�������ꍇ�́A���s�Ƃ݂Ȃ����Z�b�g
                WCMi_Printf(WCMi_RText_WmDisconnected, "NWM_Connect");
                WCMi_SetPhase(WCM_PHASE_IDLE_TO_DCF);         // �t�F�[�Y���㏑��
                WcmNwmReset();
            }
            else
            {
                    // Auth-ID ��ޔ�
                    wcmw->nwm.authId = cb->aid;
                WCMi_SetPhase(WCM_PHASE_DCF);         // �񓯊��V�[�P���X����I��
                WCMi_Notify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
                }
            break;

        default:    // �z��O�̃X�e�[�g�R�[�h
            WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
            WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), /*(void*)(cb->state)*/0, __LINE__);
        }
        break;

    case NWM_RETCODE_FAILED:
        switch (cb->reason)
        {
        case NWM_REASON_AUTH_FAILED: // miz : Auth���s�����ꍇ�����l
        case NWM_REASON_WEP_KEY_ERROR:
        case NWM_REASON_WPA_KEY_ERROR:
        case NWM_REASON_NO_NETWORK_AVAIL: // sato adds
        case NWM_REASON_BSS_DISCONNECTED: // miz : terminated notifications
        case NWM_REASON_LOST_LINK: // miz : performs process similar to terminated one
        case NWM_REASON_ASSOC_FAILED: // miz : Assoc���s�����ꍇ�����l
        case NWM_REASON_NO_RESOURCES_AVAIL: // sato adds
        case NWM_REASON_CSERV_DISCONNECT:   // sato adds
        case NWM_REASON_INVAILD_PROFILE:    // sato adds

        case NWM_REASON_TKIP_MIC_ERROR:
        
        case NWM_REASON_NO_QOS_RESOURCES_AVAIL: // QoS �֘A�̃G���[�A����ł͏o�Ȃ��͂�

            switch (wcmw->phase)
            {
                case WCM_PHASE_DCF_TO_IDLE:
                case WCM_PHASE_DCF:
                    // �ʒm��ʂ�ݒ�
                    wcmw->notifyId = WCM_NOTIFY_DISCONNECT;

                case WCM_PHASE_IDLE_TO_DCF: // NWM ���ڑ��Ɏ��s�����ꍇ
                    // Auth-ID ���N���A
                    wcmw->nwm.authId = 0;

                    // �ڑ����s�̗��R������\�� REASON ���i�[
                    switch (cb->reason)
                    {
                        case NWM_REASON_AUTH_FAILED: // miz : Auth���s�����ꍇ�����l
                            wcmw->nwm.pseudoStatusOnConnectFail = WCM_NWMIF_STATECODE_AUTH_ERROR;
                            break;
                        case NWM_REASON_WEP_KEY_ERROR:
                        case NWM_REASON_WPA_KEY_ERROR:
                            wcmw->nwm.pseudoStatusOnConnectFail = WCM_NWMIF_STATECODE_KEY_ERROR;
                            break;
                        case NWM_REASON_NO_RESOURCES_AVAIL:
                            wcmw->nwm.pseudoStatusOnConnectFail = WCM_NWMIF_STATECODE_CAPACITY_ERROR;
                            break;
                        default:
                            break;
                    }

                /* Don't break here */
                case WCM_PHASE_IRREGULAR:
                    WcmNwmReset();
                    break;
                /* �z��O�̃t�F�[�Y�ł̐ؒf�ʒm�͖������� */
            }
            break;

        case NWM_REASON_DISCONNECT_CMD:
            if(wcmw->phase == WCM_PHASE_IDLE_TO_DCF) //�ڑ������݂Ă���ꍇ�̂ݍĐڑ�����悤�ɂ���
            {
                wcmw->nwm.authId = 0;
                WcmNwmReset();
            }
            break;

        default:
             WCMi_Printf("Receive Unknown Reason Code: %d (phase:%d)\n", cb->reason, wcmw->phase);
             WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
             WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)(cb->reason), __LINE__);
        }
        break;

    case NWM_RETCODE_INVALID_PARAM:
//    case NWM_RETCODE_OVER_MAX_ENTRY:
        // �����ڑ��Ɏ��s�����ꍇ�̓��Z�b�g
        WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_Connect", cb->retcode); //, cb->wlCmdID, cb->wlResult);
        WCMi_SetPhase(WCM_PHASE_IDLE_TO_DCF);     // IRREGULAR �ɂȂ��Ă���\��������̂Ńt�F�[�Y���㏑��
        WcmNwmReset();
        break;

    case NWM_RETCODE_ILLEGAL_STATE:
    case NWM_RETCODE_NWM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_Printf("Receive Unknown Result Code: %d (phase:%d)\n", cb->reason, wcmw->phase);
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);     // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmNwmcbDisconnect

  Description:  WM ���C�u��������̒ʒm���󂯎��n���h���B
                WM_Disconnect �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   WM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNwmcbDisconnect(void* arg)
{
    NWMCallback*    cb = (NWMCallback*)arg;
    WCMWork*        wcmw = WCMi_GetSystemWork();

    switch (cb->retcode)
    {
    case NWM_RETCODE_SUCCESS:
        if (wcmw->phase == WCM_PHASE_IRREGULAR)
        {
            // �ؒf���ꂽ��ɖ����I�Ȑؒf�v���̊����ʒm�������ꍇ�A�O�̂��߃��Z�b�g
            WCMi_Printf(WCMi_RText_WmDisconnected, "NWM_Disconnect");
            WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
            WcmNwmReset();
        }
        else if ( wcmw->phase == WCM_PHASE_TERMINATING )
        {
            // �����I���V�[�P���X���̏ꍇ�͂����Ń��Z�b�g
            WcmNwmReset();
        }
        else
        {
            // Auth-ID ���N���A
            wcmw->nwm.authId = 0;

            WCMi_SetPhase(WCM_PHASE_IDLE);    // �񓯊��V�[�P���X����I��
            WCMi_Notify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
        }
        break;

    case NWM_RETCODE_FAILED:
    case NWM_RETCODE_ILLEGAL_STATE:          // �N���e�B�J���ȃ^�C�~���O�ŒʐM���؂ꂽ�ꍇ�ɕԂ��Ă�����
        // �����ڑ��̐ؒf�v�������s�����ꍇ�̓��Z�b�g
        WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_Disconnect", cb->retcode); //,  cb->wlCmdID, cb->wlResult);
        WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
        WcmNwmReset();
        break;

    case NWM_RETCODE_NWM_DISABLE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmNwmcbSetRecvFrmBuf

  Description:  NWM ���C�u��������̒ʒm���󂯎��n���h���B
                NWM_SetReceivingFrameBuffer �ɑ΂��錋�ʉ����A�y�уf�[�^��M�ʒm
                ���󂯎��B

  Arguments:    arg     -   NWM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNwmcbSetRecvFrmBuf(void* arg) // miz
{
    NWMReceivingFrameCallback*  cb = (NWMReceivingFrameCallback*)arg; // miz
    WCMWork*                    wcmw = WCMi_GetSystemWork();
    static u8 myNwType = NWM_NWTYPE_INFRA;

   switch(cb->retcode)
    {
    case NWM_RETCODE_SUCCESS:  // NWM_SetReceivingFrameBuffer �ɑ΂��錋�ʉ���
        if (wcmw->phase == WCM_PHASE_IRREGULAR)
        {
        // �ؒf���ꂽ��� DCF �J�n�����ʒm�������ꍇ�́A���s�Ƃ݂Ȃ����Z�b�g
            WCMi_Printf(WCMi_RText_WmDisconnected, "NWM_SetReceivingFrameBuffer");
            WCMi_SetPhase(WCM_PHASE_IDLE_TO_DCF); // �t�F�[�Y���㏑��
            WcmNwmReset(); // miz
        }
        else
        {
            NWMRetCode  nwmResult = NWM_RETCODE_MAX;

            // issues an AP wireless connection request
            if( wcmw->bssDesc.capaInfo & NWM_CAPABILITY_ADHOC_MASK )
            {
                myNwType = NWM_NWTYPE_ADHOC;
            }
            else
            {
                myNwType = NWM_NWTYPE_INFRA;
            }
            
            nwmResult = NWM_Connect(WcmNwmcbConnect, myNwType, (NWMBssDesc*)&(wcmw->bssDesc));

            switch (nwmResult)
            {
            case NWM_RETCODE_OPERATING:
                break;

            case NWM_RETCODE_FIFO_ERROR:
                WCMi_Printf(WCMi_RText_WmSyncError, "NWM_Connect", nwmResult);
                WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
                WCMi_Notify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), 0, __LINE__);
                break;

            case NWM_RETCODE_ILLEGAL_STATE:
                WCMi_Warning(WCMi_WText_InvalidWmState);

            /* Don't break here */
            default:
                WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
                WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
            }
        }
        break;

    case NWM_RETCODE_RECV_IND: //�f�[�^��M
        // CPS �Ƃ̃C���^�[�t�F�[�X�� DCF ��M��ʒm
        DC_InvalidateRange(cb->recvBuf, WCM_DCF_RECV_BUF_SIZE);

        /* NWM ���Ԃ� RSSI �l�� s16 �� -4 �` 110 �͈̔͂ŕϓ����邪�A            *
          * WCM �ł� RSSI �l�� u8 �Ƃ��Ĉ������߁A�}�C�i�X�� 0 �ɒu�������L���X�g */
        {
            s16 tmpRssi = cb->rssi;
            if (tmpRssi < 0 )
            {
                tmpRssi = 0;
            }
            if (tmpRssi > 256)
            {
                tmpRssi = 255;
            }
            WCMi_ShelterRssi((u8) tmpRssi);
        }

        WCMi_CpsifRecvCallback(cb->recvBuf->sa, cb->recvBuf->da, cb->recvBuf->frame, (s32)(cb->length - 14)); 
        break;
    default:
        /* [TODO] �G���[���� */
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmNwmcbUnsetRecvFrmBuf

  Description:  NWM ���C�u��������̒ʒm���󂯎��n���h���B
                NWM_UnsetRecvFrmBuf �ɑ΂��錋�ʉ������󂯎��B

  Arguments:    arg     -   NWM ���C�u��������n�����ʒm�p�����[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNwmcbUnsetRecvFrmBuf(void* arg)
{
    NWMCallback*    cb = (NWMCallback*)arg;
    NWMRetCode  	nwmResult;
    WCMWork*        wcmw = WCMi_GetSystemWork();

    switch (cb->retcode)
    {
    case NWM_RETCODE_SUCCESS:
        if (wcmw->phase == WCM_PHASE_IRREGULAR)
        {
            // �ؒf���ꂽ��� DCF �I�������ʒm�������ꍇ�́A���s�Ƃ݂Ȃ����Z�b�g
            WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
            WcmNwmReset();
        }
        else
        {
            nwmResult = NWM_Disconnect(WcmNwmcbDisconnect);
            switch (nwmResult)
            {
            case NWM_RETCODE_OPERATING:
                WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE); // �񓯊��I�ȃV�[�P���X�̊J�n�ɐ���
                wcmw->notifyId = WCM_NOTIFY_DISCONNECT;
                break;

            case NWM_RETCODE_FIFO_ERROR:
                WCMi_Printf(WCMi_RText_WmSyncError, "NWM_Disconnect", nwmResult);
                WCMi_SetPhase(WCM_PHASE_IRREGULAR);   // ARM7 �ւ̗v�����s�Ɏ��s( �V�[�P���X�r��Ń��g���C�s�\ )
                WCMi_Notify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), 0, __LINE__);
                break;

            case NWM_RETCODE_ILLEGAL_STATE:         // �N���e�B�J���ȃ^�C�~���O�ŒʐM���؂ꂽ�ꍇ
            // �ؒf���鐡�O�� AP ������ؒf���ꂽ�ꍇ�́A���s�Ƃ݂Ȃ����Z�b�g
                WCMi_Warning(WCMi_WText_InvalidWmState);
                WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
                WcmNwmReset();
                break;

            /* Don't break here */
             default:
                WCMi_SetPhase(WCM_PHASE_FATAL_ERROR); // �z��͈͊O�̃G���[
                WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
            }
        }
        break;

    case NWM_RETCODE_FAILED:
    case NWM_RETCODE_ILLEGAL_STATE:
        WCMi_Warning(WCMi_WText_InvalidWmState);

    /* Don't break here */
    default:
        WCMi_SetPhase(WCM_PHASE_FATAL_ERROR);
        WCMi_Notify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
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
    wcmw->nwm.scanExParam.scanBuf = (NWMBssDesc*)wcmw->recvBuf;
    wcmw->nwm.scanExParam.scanBufSize = sizeof(wcmw->recvBuf);
    wcmw->nwm.scanExParam.channelList = (u16) ((0x0001 << WCMi_GetNextScanChannel(0)) >> 1);
    wcmw->nwm.scanExParam.scanType = (u16) (((wcmw->option & WCM_OPTION_MASK_SCANTYPE) == WCM_OPTION_SCANTYPE_ACTIVE) ? NWM_SCANTYPE_ACTIVE : NWM_SCANTYPE_PASSIVE);
    wcmw->nwm.scanExParam.channelDwellTime = (u16) ((wcmw->maxScanTime != 0) ? wcmw->maxScanTime : NWM_GetDispersionScanPeriod(wcmw->nwm.scanExParam.scanType));
    
    if (bssid == NULL)
    {
        MI_CpuCopy8(WCM_Bssid_Any, wcmw->nwm.scanExParam.bssid, WCM_BSSID_SIZE);
    }
    else
    {
        MI_CpuCopy8(bssid, wcmw->nwm.scanExParam.bssid, WCM_BSSID_SIZE);
    }

    //MI_CpuCopy8( bssid , wcmw->nwm.scanExParam.bssid , WCM_BSSID_SIZE );
    if ((essid == NULL) || (essid == (void*)WCM_Essid_Any))
    {
        MI_CpuCopy8(WCM_Essid_Any, wcmw->nwm.scanExParam.ssid, WCM_ESSID_SIZE);
        wcmw->nwm.scanExParam.ssidLength = 0;
        wcmw->nwm.scanExParam.ssidMatchLength = 0;
    }
    else
    {
        s32 len;

        MI_CpuCopy8(essid, wcmw->nwm.scanExParam.ssid, WCM_ESSID_SIZE);
        for (len = 0; len < WCM_ESSID_SIZE; len++)
        {
            if (((u8*)essid)[len] == '\0')
            {
                break;
            }
        }

        wcmw->nwm.scanExParam.ssidLength = (u16) len;
        wcmw->nwm.scanExParam.ssidMatchLength = (u16) len;
    }

    wcmw->nwm.scanCount = 0;
}

/*---------------------------------------------------------------------------*
  Name:         WcmNwmcbSetDCFData

  Description:  DCF �t���[�����M�v���ɑ΂��閳���h���C�o����̔񓯊��I�ȉ�����
                �󂯎��R�[���o�b�N�֐��B

  Arguments:    arg -   �����h���C�o����̉����p�����[�^���i�[����Ă���o�b�t�@
                        �̃A�h���X���n�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNwmcbSetDCFData(void* arg)
{
    NWMSendFrameCallback*   cb      = (NWMSendFrameCallback*)arg;
    volatile WCMCpsifWork*  wcmiw   = WCMi_GetCpsifWork();

    switch (cb->apiid)
    {
    case NWM_APIID_SEND_FRAME:
        wcmiw->sendResult = (u16) (cb->retcode);
        if (cb->retcode == NWM_RETCODE_SUCCESS)
        {
            WCMi_ResetKeepAliveAlarm();
        }

        OS_WakeupThread((OSThreadQueue*)&(wcmiw->sendQ));
        break;
    default:
        WCMi_Printf("WcmNwmcbSetDCFData: Invalid callback.\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmNwmcbSetDCFDataAsync

  Description:  DCF �t���[�����M�v���ɑ΂��閳���h���C�o����̔񓯊��I�ȉ�����
                �󂯎��R�[���o�b�N�֐��B

  Arguments:    arg -   �����h���C�o����̉����p�����[�^���i�[����Ă���o�b�t�@
                        �̃A�h���X���n�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNwmcbSetDCFDataAsync(void* arg)
{
    NWMSendFrameCallback*   cb      = (NWMSendFrameCallback*)arg;
    volatile WCMCpsifWork*  wcmiw   = WCMi_GetCpsifWork();
    s32                     wcmResult = WCM_RESULT_SUCCESS;
    WCMSendInd              sendCallback = NULL;

    switch (cb->apiid)
    {
    case NWM_APIID_SEND_FRAME:
        wcmiw->sendResult = (u16) (cb->retcode);
        if (cb->retcode == NWM_RETCODE_SUCCESS)
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
        case NWM_RETCODE_SUCCESS:
            // ����I��
            wcmResult = WCM_RESULT_SUCCESS;
            break;

        case NWM_RETCODE_FAILED:
        default:
            // ���M�Ɏ��s�����ꍇ
            wcmResult = WCM_CPSIF_RESULT_SEND_FAILURE;
            WCMi_Printf(WCMi_RText_WmAsyncError, "NWM_SendFrame", wcmiw->sendResult);
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

// miz : 070612
static void WcmNwmcbSetQos(void* arg)
{
    NWMCallback*    cb = (NWMCallback*)arg;
    NWMRetCode      nwmResult = NWM_RETCODE_MAX;

    OSMessage message = (OSMessage)0x12345;

    switch (cb->retcode)
    {
    case NWM_RETCODE_SUCCESS:
        OS_TPrintf("[WCMN] set_qos success\n");
//		(void)OS_SendMessage(&mesgQueue, message, OS_MESSAGE_BLOCK);
        break;
    default:
        OS_Panic("[WCMN] set_qos error\n");
        break;
    }
}


#include <twl/ltdmain_end.h>

