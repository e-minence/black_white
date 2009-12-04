/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - WCM - libraries
  File:     system.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-19#$
  $Rev: 1094 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include "wcm_private.h"
#include "wcm_message.h"

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/

// �S�� 0xff �� ESSID
const u8    WCM_Essid_Any[WCM_ESSID_SIZE] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// �S�� 0xff �� BSSID
const u8    WCM_Bssid_Any[WCM_BSSID_SIZE] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

u32                 WCM_DebugFlags = 0;

/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static WCMWork*     wcmw = NULL;
static u8           WCMi_OverridedDeviceId = WCM_DEVICEID_DEFAULT;

/*---------------------------------------------------------------------------*
    �����֐��v���g�^�C�v
 *---------------------------------------------------------------------------*/
static void         WcmConfigure(WCMConfig* config, WCMNotify notify);
static void         WcmInitOption(void);
static void         WcmKeepAliveAlarm(void* arg);

u8                  WCMi_GetDefaultDeviceId(void);
void                WCMi_OverrideDefaultDeviceId(u8 deviceId);

/* �R���p�C�����A�T�[�V���� */
#ifndef SDK_ASSERT_ON_COMPILE
#define SDK_ASSERT_ON_COMPILE(expr) extern assert_on_compile ## #__LINE__ (char a[(expr) ? 1 : -1])
#endif

/*---------------------------------------------------------------------------*
  Name:         WCM_Init
  Name:         WCM_InitEx

  Description:  WCM ���C�u����������������B�����֐��B

  Arguments:    buf     -   WCM ���C�u�����������I�Ɏg�p���郏�[�N�o�b�t�@�ւ�
                            �|�C���^���w�肷��B������������́AWCM_Finish �֐�
                            �ɂ���� WCM ���C�u�������I������܂ŕʂ̗p�r�ɗp��
                            �Ă͂����Ȃ��B
                len     -   ���[�N�o�b�t�@�̃T�C�Y���w�肷��BWCM_WORK_SIZE ���
                            �������T�C�Y���w�肷��Ƃ��̊֐��͎��s����B
                deviceId-   �g�p���閳���f�o�C�X��ID���w�肷��B

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̏������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_NOT_ENOUGH_MEM
 *---------------------------------------------------------------------------*/
s32 WCM_Init(void* buf, s32 len)
{
    return WCM_InitEx(buf, len, WCM_DEVICEID_WM);
}

s32 WCM_InitEx(void* buf, s32 len, u8 deviceId)
{
    OSIntrMode  e = OS_DisableInterrupts();
    s32         expectLen = 0;
    
    // WCM_WORK_SIZE �萔�̑Ó������R���p�C�����Ɋm�F
    SDK_ASSERT_ON_COMPILE(sizeof(WCMWork) <= WCM_WORK_SIZE);
    SDK_ASSERT_ON_COMPILE(sizeof(WCMBssDesc) <= WCM_APLIST_SIZE);
    SDK_ASSERT_ON_COMPILE(sizeof(WCMBssDesc) == sizeof(WMBssDesc));
#ifdef SDK_TWL
    SDK_ASSERT_ON_COMPILE(sizeof(WCMWork) <= WCM_WORK_SIZE_NWM);
    SDK_ASSERT_ON_COMPILE(sizeof(WCMBssDesc) >= sizeof(NWMBssDesc));
#else
    SDK_ASSERT_ON_COMPILE(sizeof(WCMWork) <= WCM_WORK_SIZE_WM);
#endif

    // �����������m�F
    if (wcmw != NULL)
    {
        WCMi_Warning(WCMi_WText_AlreadyInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // ���ɏ������ς�
    }

    // �p�����[�^���m�F
    if (buf == NULL)
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // �w��o�b�t�@�� NULL
    }

    if ((u32) buf & 0x01f)
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // �w��o�b�t�@�� 32 �o�C�g�A���C���łȂ�
    }

    // �o�b�t�@�̓��e���N���A�B�����f�o�C�X��p�̈�̏����������˂Ă���
    MI_CpuClear8(buf, (u32)len);

    // �����f�o�C�X�� ID ���w�肳��Ă��Ȃ��ꍇ�͓��샂�[�h����
    // �g�p���ׂ������f�o�C�X������
    if (deviceId == WCM_DEVICEID_DEFAULT)
    {
        if (WCMi_OverridedDeviceId != WCM_DEVICEID_DEFAULT)
        {
            deviceId = WCMi_OverridedDeviceId;
        }
        else
        {
            deviceId = WCMi_GetDefaultDeviceId();
        }
    }
    
    // �g�p���閳���f�o�C�X�ɉ����ĕK�v�ȃ��[�N�̈�̃T�C�Y�����߂�
    if (!WCM_GetWorkSize(&expectLen, deviceId))
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "deviceId");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // �f�o�C�XID������
    }

    // ���[�N�̈�̑傫�����\�������m�F
    if (len < expectLen)
    {
        WCMi_Warning(WCMi_WText_IllegalParam, "len");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_NOT_ENOUGH_MEM;   // �w��o�b�t�@�̃T�C�Y������Ȃ�
    }

    // �e���[�N�ϐ���������
    wcmw = (WCMWork*)buf;
    wcmw->phase = WCM_PHASE_WAIT;           // �����t�F�[�Y��������
    wcmw->notifyId = WCM_NOTIFY_COMMON;     // �񓯊� API �ʒm ID ��������
    wcmw->maxScanTime = 0;                  // �Œ�X�L�������Ԃ�������
    wcmw->apListLock = WCM_APLIST_UNLOCK;   // AP ���ێ����X�g�̃��b�N��Ԃ�������
    wcmw->resetting = WCM_RESETTING_OFF;    // ���Z�b�g�d���Ăяo���Ǘ��t���O��������
    wcmw->deviceId = deviceId;              // �g�p���閳���f�o�C�X���L��
    
    WcmInitOption();                        // �I�v�V�����ݒ��������
    WCMi_InitCpsif();                       // CPS �C���^�[�t�F�[�X��������

    // �\���̃����o�̃A���C�������g���m�F
    SDK_ASSERT( ((u32)&wcmw->sendBuf & 0x1f) == 0 );
    SDK_ASSERT( ((u32)&wcmw->recvBuf & 0x1f) == 0 );
    SDK_ASSERT( ((u32)&wcmw->bssDesc & 0x1f) == 0 );
    
    SDK_ASSERT( ((u32)&wcmw->wm.work & 0x1f) == 0 );
#ifdef SDK_TWL
    SDK_ASSERT( ((u32)&wcmw->nwm.work & 0x1f) == 0 );
#endif

    // �A���[����������
    if (!OS_IsTickAvailable())
    {
        OS_InitTick();
    }

    if (!OS_IsAlarmAvailable())
    {
        OS_InitAlarm();
    }

    OS_CreateAlarm(&(wcmw->keepAliveAlarm));

    // ����I��
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_Finish

  Description:  WCM���C�u�������I������B�����֐��BWCM_Init�֐��ɂ���Ďw�肵��
                ���[�N�p�o�b�t�@�͉�������B

  Arguments:    None.

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̏������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
s32 WCM_Finish(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // �������ς݂��m�F
    if (wcmw == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // ����������Ă��Ȃ�
    }

    // �����t�F�[�Y�m�F
    if (wcmw->phase != WCM_PHASE_WAIT)
    {
        WCMi_Warning(WCMi_WText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // WAIT �t�F�[�Y�łȂ�
    }

    // ���[�N�o�b�t�@�����
    wcmw = NULL;

    // ����I��
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetWorkSize

  Description:  �w�肵�������f�o�C�X���g�p���邽�߂ɕK�v�ȃ��[�N�T�C�Y���擾
                ����B

  Arguments:    workSize     - �擾�������[�N�T�C�Y���i�[����|�C���^���w��
                deviceId     - �g�p���閳���f�o�C�X�� ID

  Returns:      BOOL         - �����̉ہB
                               NITRO ���쎞�ɐV�����̃��[�N�T�C�Y���擾���悤
                               �Ƃ����ꍇ�����s�ƂȂ�B
 *---------------------------------------------------------------------------*/
BOOL WCM_GetWorkSize(s32* workSize, u8 deviceId)
{
    if (deviceId == WCM_DEVICEID_DEFAULT)
    {
        deviceId = WCMi_GetDefaultDeviceId();
    }
    switch(deviceId)
    {
    case WCM_DEVICEID_WM:
        *workSize = WCM_WORK_SIZE_WM;
        break;
#ifdef SDK_TWL
    case WCM_DEVICEID_NWM:
        if (!OS_IsRunOnTwl())
        {
            return FALSE;
        }
        *workSize = WCM_WORK_SIZE_NWM;
        break;
#endif
    default:
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_StartupAsync

  Description:  �����@�\�̋N���V�[�P���X���J�n����B
                �����I�ɂ� WAIT ���� IDLE �փt�F�[�Y�̈ڍs�������s����B
                �񓯊��֐��ł���A�����I�ȕԂ�l�� WCM_RESULT_ACCEPT �ł�����
                �ꍇ�ɂ� notify �Ŏw�肵���R�[���o�b�N�֐��ɂ��񓯊��I��
                �������ʂ��ʒm�����B
                �񓯊��������ʂ̒ʒm��ʂ� WCM_NOTIFY_STARTUP �ƂȂ�B

  Arguments:    config  -   WCM �̓���ݒ�ƂȂ�\���̂ւ̃|�C���^���w��B
                notify  -   �񓯊��I�ȏ������ʂ�ʒm����R�[���o�b�N�֐����w��B
                            ���̃R�[���o�b�N�֐��͈Ȍ�̔񓯊��I�Ȋ֐��̌���
                            �ʒm�ɋ��ʂ��Ďg�p�����B

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̓����I�ȏ������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_WMDISABLE ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_StartupAsync(WCMConfig* config, WCMNotify notify)
{
    OSIntrMode  e           = OS_DisableInterrupts();
    s32         wcmResult   = WCM_RESULT_SUCCESS;
    
    // �������ς݂��m�F
    if (wcmw == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // ����������Ă��Ȃ�
    }

    // �����t�F�[�Y�m�F
    switch (wcmw->phase)
    {
    case WCM_PHASE_WAIT:
        WcmConfigure(config, notify);
        break;

    case WCM_PHASE_WAIT_TO_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;                 // ���ɓ������N�G�X�g�̔񓯊�������

    case WCM_PHASE_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;                  // ��ԑJ�ږڕW�t�F�[�Y�Ɉڍs�ς�

    default:
        WCMi_Warning(WCMi_WText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // �v�����󂯕t�����Ȃ��t�F�[�Y
    }

    // WM_Init ���Ăяo���K�v���������O�̂��ߊm�F���Ă��� 
#ifdef WM_PRECALC_ALLOWEDCHANNEL /* (!SDK_TEG && SDK_VERSION_WL >= 15600) */
    // �����ʐM���֎~����Ă���{�̂łȂ����Ƃ��m�F
    if (0 == WM_GetAllowedChannel())
    {
        WCMi_Warning(WCMi_WText_WirelessForbidden);
        return WCM_RESULT_WMDISABLE;            // �����ʐM��������Ă��Ȃ�
    }
#else
#error
#endif

    switch(wcmw->deviceId)
    {
    case WCM_DEVICEID_WM:
        wcmResult = WCMi_WmifStartupAsync();
        break;
#ifdef SDK_TWL
    case WCM_DEVICEID_NWM:
        if (OS_IsRunOnTwl())
        {
            wcmResult = WCMi_NwmifStartupAsync();
            break;
        }
        // no break;
#endif
    default:
        wcmResult = WCM_RESULT_FATAL_ERROR;
    }

    (void)OS_RestoreInterrupts(e);
    return wcmResult;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_CleanupAsync

  Description:  �����@�\�̒�~�V�[�P���X���J�n����B
                �����I�ɂ� IDLE ���� WAIT �փt�F�[�Y�̈ڍs�������s����B
                �񓯊��֐��ł���A�����I�ȕԂ�l�� WCM_RESULT_ACCEPT �ł�����
                �ꍇ�ɂ� WCM_StartupAsync �֐��Ŏw�肵���R�[���o�b�N�֐��ɂ��
                �񓯊��I�ȏ������ʂ��ʒm�����B
                �񓯊��������ʂ̒ʒm��ʂ� WCM_NOTIFY_CLEANUP �ƂȂ�B

  Arguments:    None.

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̓����I�ȏ������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_CleanupAsync(void)
{
    OSIntrMode  e           = OS_DisableInterrupts();
    s32         wcmResult   = WCM_RESULT_SUCCESS;

    // �������ς݂��m�F
    if (wcmw == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // ����������Ă��Ȃ�
    }

    // �����t�F�[�Y�m�F
    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
        break;

    case WCM_PHASE_IDLE_TO_WAIT:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;                 // ���ɓ������N�G�X�g�̔񓯊�������

    case WCM_PHASE_WAIT:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;                  // ��ԑJ�ږڕW�t�F�[�Y�Ɉڍs�ς�

    default:
        WCMi_Warning(WCMi_WText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // �v�����󂯕t�����Ȃ��t�F�[�Y
    }

    switch(wcmw->deviceId)
    {
    case WCM_DEVICEID_WM:
        wcmResult = WCMi_WmifCleanupAsync();
        break;
#ifdef SDK_TWL
    case WCM_DEVICEID_NWM:
        if (OS_IsRunOnTwl())
        {
            wcmResult = WCMi_NwmifCleanupAsync();
            break;
        }
        // no break;
#endif
    default:
        wcmResult = WCM_RESULT_FATAL_ERROR;
    }

    // ����I��
    (void)OS_RestoreInterrupts(e);
    return wcmResult;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SearchAsync

  Description:  AP �����T���J�n�V�[�P���X�A�������͒�~�V�[�P���X���J�n����B
                �p�����[�^�ɂ���� WCM_BeginSearchAsync �֐����Ăяo�����A
                WCM_EndSearchAsync �֐����Ăяo�����U�蕪���邾���̊֐��B

  Arguments:    bssid   -   �T������ AP �� BSSID ���w�肷��BNULL ���w�肵��
                            �ꍇ�ɂ́A�T���̒�~���w���������̂Ƃ݂Ȃ����B
                essid   -   �T������ AP �� ESSID ���w�肷��BNULL ���w�肵��
                            �ꍇ�ɂ́A�T���̒�~���w���������̂Ƃ݂Ȃ����B
                option  -   �I�v�V�����ݒ�t�H�[�}�b�g�ɏ]���� 32 �r�b�g��
                            �I�v�V�����ύX�p�ϐ����w�肷��B0 ���w�肵���ꍇ��
                            �ύX�͍s���Ȃ��B

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̓����I�ȏ������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_SearchAsync(const void* bssid, const void* essid, u32 option)
{
    // �p�����[�^���m�F
    if ((bssid == NULL) || (essid == NULL))
    {
        return WCM_EndSearchAsync();
    }

    return WCM_BeginSearchAsync(bssid, essid, option);
}

/*---------------------------------------------------------------------------*
  Name:         WCM_BeginSearchAsync

  Description:  AP �����T���J�n�V�[�P���X���J�n����B���Ɏ����T����Ԃł�����
                �ꍇ�ɁA�T�������̕ύX�̂ݍs�����Ƃ��\�B
                �����I�ɂ� IDLE ���� SEARCH �փt�F�[�Y�̈ڍs�������s����B
                �񓯊��֐��ł���A�����I�ȕԂ�l�� WCM_RESULT_ACCEPT �ł�����
                �ꍇ�ɂ� WCM_StartupAsync �֐��Ŏw�肵���R�[���o�b�N�֐��ɂ��
                �񓯊��I�ȏ������ʂ��ʒm�����B
                �񓯊��������ʂ̒ʒm��ʂ� WCM_NOTIFY_BEGIN_SEARCH �ƂȂ�B

  Arguments:    bssid   -   �T������ AP �� BSSID ���w�肷��BNULL ��������
                            WCM_BSSID_ANY ���w�肵���ꍇ�͔C�ӂ� BSSID ������
                            AP ��T������B
                essid   -   �T������ AP �� ESSID ���w�肷��BNULL ��������
                            WCM_ESSID_ANY ���w�肵���ꍇ�͔C�ӂ� ESSID ������
                            AP ��T������B
                option  -   �I�v�V�����ݒ�t�H�[�}�b�g�ɏ]���� 32 �r�b�g��
                            �I�v�V�����ύX�p�ϐ����w�肷��B0 ���w�肵���ꍇ��
                            �ύX�͍s��Ȃ��B

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̓����I�ȏ������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_BeginSearchAsync(const void* bssid, const void* essid, u32 option)
{
    OSIntrMode  e           = OS_DisableInterrupts();
    s32         wcmResult   = WCM_RESULT_SUCCESS;

    // �������ς݂��m�F
    if (wcmw == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // ����������Ă��Ȃ�
    }

    // AP �����T���J�n�V�[�P���X���J�n�A�������͒T�������ύX
    switch(wcmw->deviceId)
    {
    case WCM_DEVICEID_WM:
        wcmResult = WCMi_WmifBeginSearchAsync(bssid, essid, option);
        break;
#ifdef SDK_TWL
    case WCM_DEVICEID_NWM:
        if (OS_IsRunOnTwl())
        {
            wcmResult = WCMi_NwmifBeginSearchAsync(bssid, essid, option);
            break;
        }
        // no break;
#endif
    default:
        wcmResult = WCM_RESULT_FATAL_ERROR;
    }

    (void)OS_RestoreInterrupts(e);
    return wcmResult;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_EndSearchAsync

  Description:  AP �����T����~�V�[�P���X���J�n����B
                �����I�ɂ� SEARCH ���� IDLE �փt�F�[�Y�̈ڍs�������s����B
                �񓯊��֐��ł���A���@�I�ȕԂ�l�� WCM_RESULT_ACCEPT �ł�����
                �ꍇ�ɂ� WCM_StartupAsync �֐��Ŏw�肵���R�[���o�b�N�֐��ɂ��
                �񓯊��I�ȏ������ʂ��ʒm�����B
                �񓯊��������ʂ̒ʒm��ʂ� WCM_NOTIFY_END_SEARCH �ƂȂ�B

  Arguments:    None.

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̓����I�ȏ������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_EndSearchAsync(void)
{
    OSIntrMode  e           = OS_DisableInterrupts();

    // �������ς݂��m�F
    if (wcmw == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // ����������Ă��Ȃ�
    }

    // �����t�F�[�Y�m�F
    switch (wcmw->phase)
    {
    case WCM_PHASE_SEARCH:
        WCMi_SetPhase(WCM_PHASE_SEARCH_TO_IDLE);
        wcmw->notifyId = WCM_NOTIFY_END_SEARCH;
        break;

    case WCM_PHASE_SEARCH_TO_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS; // ���ɓ������N�G�X�g�̔񓯊�������

    case WCM_PHASE_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;  // ��ԑJ�ږڕW�t�F�[�Y�Ɉڍs�ς�

    default:
        WCMi_Warning(WCMi_WText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // �v�����󂯕t�����Ȃ��t�F�[�Y
    }

    /* �X�L������~�v���̔��s�� StartScanEx �̃R�[���o�b�N���ōs���̂ŁA�����ł͍s��Ȃ� */

    // ����I��
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_ConnectAsync

  Description:  AP �Ƃ̖����ڑ��V�[�P���X���J�n����B
                �����I�ɂ� IDLE ���� DCF �փt�F�[�Y�̈ڍs�������s����B
                �񓯊��֐��ł���A�����I�ȕԂ�l�� WCM_RESULT_ACCEPT �ł�����
                �ꍇ�ɂ� WCM_StartupAsync �֐��Ŏw�肵���R�[���o�b�N�֐��ɂ��
                �񓯊��I�ȏ������ʂ��ʒm�����B
                �񓯊��������ʂ̒ʒm��ʂ� WCM_NOTIFY_CONNECT �ƂȂ�B

  Arguments:    bssDesc -   �ڑ����� AP �̖����l�b�g���[�N�����w�肷��B
                            WCM_SearchAsync �ɂ���ē���ꂽ��񂪂��̂܂܎w��
                            ����邱�Ƃ�z��B
                wepDesc -   WCMWepDesc �^�� WEP �L�[�ɂ��Í����ݒ�����w��
                            ����BNULL �̏ꍇ�́AWEP �Í����Ȃ��Ƃ����ݒ�ɂȂ�B
                option  -   �I�v�V�����ݒ�t�H�[�}�b�g�ɏ]���� 32 �r�b�g��
                            �I�v�V�����ύX�p�ϐ����w�肷��B0 ���w�肵���ꍇ��
                            �ύX�͍s���Ȃ��B

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̓����I�ȏ������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_ConnectAsync(void* bssDesc, void* wepDesc, u32 option)
{
    OSIntrMode      e           = OS_DisableInterrupts();
    WCMWork*        wcmw        = WCMi_GetSystemWork();
    s32             wcmResult   = WCM_RESULT_SUCCESS;
    /*
        WCMWepDesc �͊�{�I�� WCMPrivacyDesc �ɃL���X�g���Ă����S�B
        �������Amode �����o�̒l�ɂ���ċ��p�̂̃����o���؂�ւ�鎖�ɒ��ӁB
     */
    WCMPrivacyDesc* privacyDesc = (WCMPrivacyDesc*)wepDesc;
    
    // �������ς݂��m�F
    if (wcmw == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // ����������Ă��Ȃ�
    }

    // �����t�F�[�Y�m�F
    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
        // �p�����[�^�m�F
        if (bssDesc == NULL)
        {
            WCMi_Warning(WCMi_WText_IllegalParam, "bssDesc");
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;          // bssDesc ������ NULL
        }
        if (((WMBssDesc*)bssDesc)->gameInfoLength > 0)
        {
            WCMi_Warning(WCMi_WText_IllegalParam, "bssDesc");
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;          // bssDesc ������ AP ���ł͂Ȃ� DS �e�@
        }
        
        if (privacyDesc != NULL)
        {
            // wepDesc �̒��g�̈�ѐ����`�F�b�N
            if ((privacyDesc->mode <= WCM_PRIVACYMODE_WEP128 && privacyDesc->keyId >= 4)
                || wcmw->privacyDesc.keyLen > 64)
            {
                WCMi_Warning(WCMi_WText_IllegalParam, "wepDesc");
                (void)OS_RestoreInterrupts(e);
                return WCM_RESULT_FAILURE;      // wepDesc �������z��O�̓��e
            }

            // wepDesc �̒��g�� WCMWork ��Ɉ��S�ɃR�s�[
            wcmw->privacyDesc.mode = privacyDesc->mode;
            if (privacyDesc->mode == WCM_PRIVACYMODE_NONE)
            {
                MI_CpuClear8(wcmw->privacyDesc.key, WM_SIZE_WEPKEY);
            }
            else if (privacyDesc->mode <= WCM_PRIVACYMODE_WEP128)
            {
                wcmw->privacyDesc.keyId = privacyDesc->keyId;
                MI_CpuCopy8(privacyDesc->key, wcmw->privacyDesc.key, WM_SIZE_WEPKEY);
            }
            else /* WPA */
            {
                wcmw->privacyDesc.keyLen = privacyDesc->keyLen;
                MI_CpuCopy8(privacyDesc->key, wcmw->privacyDesc.key, wcmw->privacyDesc.keyLen);
            }
        }
        else
        {
            MI_CpuClear8(&(wcmw->privacyDesc), sizeof(WCMPrivacyDesc));
        }
        MI_CpuCopy8(bssDesc, &(wcmw->bssDesc), sizeof(WMBssDesc));
        
        (void)WCM_UpdateOption(option);
        break;

    case WCM_PHASE_IDLE_TO_DCF:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;             // ���ɓ������N�G�X�g�̔񓯊�������

    case WCM_PHASE_DCF:
        (void)OS_RestoreInterrupts(e);          // ��ԑJ�ږڕW�t�F�[�Y�Ɉڍs�ς�
        return WCM_RESULT_SUCCESS;

    default:
        WCMi_Warning(WCMi_WText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // �v�����󂯕t�����Ȃ��t�F�[�Y
    }

    // AP �Ƃ̖����ڑ��V�[�P���X
    switch(wcmw->deviceId)
    {
    case WCM_DEVICEID_WM:
        wcmResult = WCMi_WmifConnectAsync();
        break;
#ifdef SDK_TWL
    case WCM_DEVICEID_NWM:
        if (OS_IsRunOnTwl())
        {
            wcmResult = WCMi_NwmifConnectAsync();
            break;
        }
        // no break;
#endif
    default:
        wcmResult = WCM_RESULT_FATAL_ERROR;
    }

    // ����I��
    (void)OS_RestoreInterrupts(e);
    return wcmResult;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_DisconnectAsync

  Description:  AP �Ƃ̖����ڑ���ؒf����V�[�P���X���J�n����B
                �����I�ɂ� DCF ���� IDLE �փt�F�[�Y�̈ڍs�������s����B
                �񓯊��֐��ł���A�����I�ȕԂ�l�� WCM_RESULT_ACCEPT �ł�����
                �ꍇ�ɂ� WCM_StartupAsync �֐��Ŏw�肵���R�[���o�b�N�֐��ɂ��
                �񓯊��I�ȏ������ʂ��ʒm�����B
                �񓯊��������ʂ̒ʒm��ʂ� WCM_NOTIFY_DISCONNECT �ƂȂ�B

  Arguments:    None.

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̓����I�ȏ������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_DisconnectAsync(void)
{
    OSIntrMode  e           = OS_DisableInterrupts();
    s32         wcmResult   = WCM_RESULT_SUCCESS;

    // �������ς݂��m�F
    if (wcmw == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // ����������Ă��Ȃ�
    }

    // �����t�F�[�Y�m�F
    switch (wcmw->phase)
    {
    case WCM_PHASE_DCF:
        break;

    case WCM_PHASE_DCF_TO_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;             // ���ɓ������N�G�X�g�̔񓯊�������

    case WCM_PHASE_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;              // ��ԑJ�ږڕW�t�F�[�Y�Ɉڍs�ς�

    default:
        WCMi_Warning(WCMi_WText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // �v�����󂯕t�����Ȃ��t�F�[�Y
    }

    // AP �Ƃ̖����ڑ��̐ؒf�V�[�P���X���J�n
    if (wcmw->resetting == WCM_RESETTING_ON)
    {
        /* �ؒf�ʒm���󂯂ă��Z�b�g���Ȃ̂ŁA���̃��Z�b�g�������Đؒf�v�������s���ꂽ���Ƃɂ��� */
        WCMi_SetPhase(WCM_PHASE_DCF_TO_IDLE);
        wcmw->notifyId = WCM_NOTIFY_DISCONNECT;
    }
    else
    {
        switch(wcmw->deviceId)
        {
        case WCM_DEVICEID_WM:
            wcmResult = WCMi_WmifDisconnectAsync();
            break;
#ifdef SDK_TWL
        case WCM_DEVICEID_NWM:
            if (OS_IsRunOnTwl())
            {
                wcmResult = WCMi_NwmifDisconnectAsync();
                break;
            }
            // no break;
#endif
        default:
            wcmResult = WCM_RESULT_FATAL_ERROR;
        }
    }

    // ����I��
    (void)OS_RestoreInterrupts(e);
    return wcmResult;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_TerminateAsync

  Description:  �����@�\�̋����I���V�[�P���X���J�n����B
                �ʂ̔񓯊����������s����Ă��Ȃ�����͂ǂ̃t�F�[�Y����ł����s
                �\�ł���A�����I�ɂ� WAIT �ւƃt�F�[�Y�̈ڍs�������s����B
                �񓯊��֐��ł���A�����I�ȕԂ�l�� WCM_RESULT_ACCEPT �ł�����
                �ꍇ�ɂ� WCM_SartupAsync �֐��Ŏw�肵���R�[���o�b�N�֐��ɂ��
                �񓯊��I�ȏ������ʂ��ʒm�����B
                �񓯊��������ʂ̒ʒm��ʂ� WCM_NOTIFY_TERMINATE �ƂȂ�B

  Arguments:    None.

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̓����I�ȏ������ʂ��Ԃ����B
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_TerminateAsync(void)
{
    OSIntrMode  e           = OS_DisableInterrupts();
    s32         wcmResult   = WCM_RESULT_SUCCESS;

    // �������ς݂��m�F
    if (wcmw == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // ����������Ă��Ȃ�
    }

    // �����t�F�[�Y�m�F
    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
    case WCM_PHASE_DCF:
    case WCM_PHASE_IRREGULAR:
        break;

    case WCM_PHASE_TERMINATING:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;             // ���ɓ������N�G�X�g�̔񓯊�������

    case WCM_PHASE_WAIT:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;              // ��ԑJ�ږڕW�t�F�[�Y�Ɉڍs�ς�

    case WCM_PHASE_SEARCH:
        WCMi_SetPhase(WCM_PHASE_TERMINATING);
        wcmw->notifyId = WCM_NOTIFY_TERMINATE;
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_ACCEPT;               // �񓯊������v�������

    /* ���Z�b�g�v���̔��s�� StartScanEx �̃R�[���o�b�N���ōs���̂ŁA�����ł͍s��Ȃ� */
    case WCM_PHASE_WAIT_TO_IDLE:
    case WCM_PHASE_IDLE_TO_WAIT:
    case WCM_PHASE_IDLE_TO_SEARCH:
    case WCM_PHASE_SEARCH_TO_IDLE:
    case WCM_PHASE_IDLE_TO_DCF:
    case WCM_PHASE_DCF_TO_IDLE:
    case WCM_PHASE_FATAL_ERROR:
    default:
        WCMi_Warning(WCMi_WText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // �v�����󂯕t�����Ȃ��t�F�[�Y
    }

    // �����@�\�̋����I���V�[�P���X���J�n����
    if (wcmw->resetting == WCM_RESETTING_ON)
    {
        /* DCF �ʐM���ɐؒf����ă��Z�b�g���Ȃ̂ŁA���̃��Z�b�g�������ċ����I���v�������s���ꂽ���Ƃɂ��� */
        WCMi_SetPhase(WCM_PHASE_TERMINATING);
        wcmw->notifyId = WCM_NOTIFY_TERMINATE;
    }
    else
    {
        switch(wcmw->deviceId)
        {
        case WCM_DEVICEID_WM:
            wcmResult = WCMi_WmifTerminateAsync();
            break;
#ifdef SDK_TWL
        case WCM_DEVICEID_NWM:
            if (OS_IsRunOnTwl())
            {
                wcmResult = WCMi_NwmifTerminateAsync();
                break;
            }
            // no break;
#endif
        default:
            wcmResult = WCM_RESULT_FATAL_ERROR;
        }
    }

    // ����I��
    (void)OS_RestoreInterrupts(e);
    return wcmResult;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetPhase

  Description:  WCM ���C�u�����̓������ ( �t�F�[�Y ) ���擾����B�����֐��B

  Arguments:    None.

  Returns:      s32     -   ���݂� WCM ���C�u�����̃t�F�[�Y��Ԃ��B
 *---------------------------------------------------------------------------*/
s32 WCM_GetPhase(void)
{
    OSIntrMode  e = OS_DisableInterrupts();
    s32         phase = WCM_PHASE_NULL;

    // �������ς݂��m�F
    if (wcmw != NULL)
    {
        phase = (s32) (wcmw->phase);
    }

    (void)OS_RestoreInterrupts(e);
    return phase;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_UpdateOption

  Description:  WCM ���C�u�����̃I�v�V�����ݒ���X�V����B

  Arguments:    option  -   �I�v�V�����ݒ�t�H�[�}�b�g�ɏ]���� 32 �r�b�g��
                            �I�v�V�����ύX�p�ϐ����w�肷��B
                            0 ���w�肵���ꍇ�͉����X�V���s��Ȃ�����ɂȂ�B

  Returns:      u32     -   �ύX���s���O�̃I�v�V�����ϐ���Ԃ��B
 *---------------------------------------------------------------------------*/
u32 WCM_UpdateOption(u32 option)
{
    OSIntrMode  e = OS_DisableInterrupts();
    u32         filter = 0;
    u32         old_option = wcmw->option;

    // �������m�F
    if (wcmw == NULL)
    {
        (void)OS_RestoreInterrupts(e);
        return 0;
    }

    // �X�V���ׂ��I�v�V�����J�e�S���𒊏o���A��U�N���A���ׂ��r�b�g��ҏW����
    if (option & WCM_OPTION_TEST_CHANNEL)
    {
        filter |= WCM_OPTION_FILTER_CHANNEL;
        if (!(option & WCM_OPTION_FILTER_CHANNEL))
        {
            option |= WCM_OPTION_CHANNEL_RDC;
        }
    }

    if (option & WCM_OPTION_TEST_POWER)
    {
        filter |= WCM_OPTION_FILTER_POWER;
    }

    if (option & WCM_OPTION_TEST_AUTH)
    {
        filter |= WCM_OPTION_FILTER_AUTH;
    }

    if (option & WCM_OPTION_TEST_SCANTYPE)
    {
        filter |= WCM_OPTION_FILTER_SCANTYPE;
    }

    if (option & WCM_OPTION_TEST_ROUNDSCAN)
    {
        filter |= WCM_OPTION_FILTER_ROUNDSCAN;
    }

    // �I�v�V�����ϐ��̊e�r�b�g���X�V
    wcmw->option = (u32) ((old_option &~filter) | option);

    (void)OS_RestoreInterrupts(e);
    return old_option;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetAllowedChannel

  Description:  ���ݎg�p���Ă��閳���f�o�C�X�̎g�p���`�����l�����X�g���擾����B

  Arguments:    None.

  Returns:      u16    - 1�`13ch�̂����A�g�p�������ꂽ�`�����l���Q�̃r�b�g�t�B�[���h��Ԃ��܂��B
                         �ŉ��ʃr�b�g���珇�� 1 �`�����l��, 2 �`�����l���c�c�ɑΉ����܂��B
 *---------------------------------------------------------------------------*/
u16 WCM_GetAllowedChannel(void)
{
    WCMWork*    wcmw = WCMi_GetSystemWork();

    /* �����g�p�\��ԂłȂ��Ǝg�p���`�����l�����擾�ł��Ȃ� NWM �ɍ��킹�� *
     * �t�F�C�Y�� WCM_PHASE_IDLE�`WCM_PHASE_DCF_TO_IDLE �̎��̂ݐ������l��Ԃ� */
    if( wcmw->phase >= WCM_PHASE_IDLE && wcmw->phase <= WCM_PHASE_DCF_TO_IDLE)
    {
        switch(wcmw->deviceId)
        {
            case WCM_DEVICEID_WM:
                return WCMi_WmifGetAllowedChannel();
                break;
#ifdef SDK_TWL
            case WCM_DEVICEID_NWM:
                if (OS_IsRunOnTwl())
                {
                    return WCMi_NwmifGetAllowedChannel();
                }
                break;
#endif /* SDK_TWL */
        }
    }
    /* ���m�̃f�o�C�X���I������Ă���ꍇ�A�����f�o�C�X���g�p�\�ȏ�ԂłȂ��ꍇ�� 0 ��Ԃ��B*/
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SetChannelScanTime

  Description:  AP �̎����T�����ɂP�̃`�����l�����X�L�������鎞�Ԃ�ݒ肷��B

  Arguments:    msec    -   �P�`�����l�����X�L�������鎞�Ԃ� ms �P�ʂŎw��B
                            10 �` 1000 �܂ł̊ԂŐݒ�\�����A���͈̔͊O�̒l��
                            �w�肷��Ǝ����T���̓f�t�H���g�̐ݒ莞�ԂŒT�����s��
                            �悤�ɂȂ�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCM_SetChannelScanTime(u16 msec)
{
    // �������m�F
    if (wcmw != NULL)
    {
        if ((msec >= 10) && (msec <= 1000))
        {
            wcmw->maxScanTime = msec;
        }
        else
        {
            wcmw->maxScanTime = 0;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_GetSystemWork

  Description:  WCM ���C�u�����������Ŏg�p���Ă��郏�[�N�o�b�t�@�ւ̃|�C���^��
                �擾���邽�߂̓����֐��B

  Arguments:    None.

  Returns:      WCMWork*    -   ���[�N�o�b�t�@�ւ̃|�C���^��Ԃ��B�������O�Ȃ�
                                �o�b�t�@�����݂��Ȃ��ꍇ�� NULL �ɂȂ�B
 *---------------------------------------------------------------------------*/
WCMWork* WCMi_GetSystemWork(void)
{
    return wcmw;
}


/*---------------------------------------------------------------------------*
  Name:         WcmConfigure

  Description:  WCM �̓�������ݒ��ҏW����B

  Arguments:    config  -   WCM �̓���ݒ�\���̂ւ̃|�C���^���w��B
                notify  -   �񓯊��I�ȏ������ʂ�ʒm����R�[���o�b�N�֐����w��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmConfigure(WCMConfig* config, WCMNotify notify)
{
    if (config == NULL)
    {
        // �ݒ肪������Ȃ��ꍇ�A�f�t�H���g�l���g�p����
        wcmw->config.dmano = 3;
        wcmw->config.pbdbuffer = NULL;
        wcmw->config.nbdbuffer = 0;
        wcmw->config.nbdmode = 0;
    }
    else
    {
        // DMA �ԍ���ޔ�
        if (config->dmano &~(0x03))
        {
            // DMA �ԍ��� 0 �` 3 �łȂ��Ƃ����Ȃ�
            WCMi_Warning(WCMi_WText_IllegalParam, "config->dmano");
        }

        wcmw->config.dmano = (config->dmano & 0x03);

        // AP ���ێ����X�g�̈�̐ݒ��ޔ�
        if ((((4 - ((u32) (config->pbdbuffer) & 0x03)) % 4) + sizeof(WCMApListHeader)) > config->nbdbuffer)
        {
            // ���X�g�Ǘ��p�w�b�_�̈悷��m�ۂł��Ȃ��T�C�Y�Ȃ̂ŁA�o�b�t�@�Ȃ��̏ꍇ�Ɠ���
            wcmw->config.pbdbuffer = NULL;
            wcmw->config.nbdbuffer = 0;
        }
        else
        {
            /*
             * �^����ꂽ�o�b�t�@�� 4 �o�C�g�A���C������Ă��Ȃ��\�������邽�߁A
             * 4 �o�C�g�A���C�����ꂽ�ʒu�ɂ��炵�A�T�C�Y�����̕��ڌ��肳����B
             */
            wcmw->config.pbdbuffer = (void*)WCM_ROUNDUP4((u32) (config->pbdbuffer));
            wcmw->config.nbdbuffer = config->nbdbuffer - (s32) ((4 - ((u32) (config->pbdbuffer) & 0x03)) % 4);
            MI_CpuClear8(wcmw->config.pbdbuffer, (u32) (wcmw->config.nbdbuffer));
        }

        wcmw->config.nbdmode = config->nbdmode;
    }

    // �ʒm�x�N�g���ޔ�
    wcmw->notify = notify;
}

/*---------------------------------------------------------------------------*
  Name:         WcmInitOption

  Description:  WCM ���C�u�����̃I�v�V�����ݒ�l������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmInitOption(void)
{
    /*
     * IW �� I/O �����킹��ׂɉ��L�̂悤�ȃf�t�H���g�ݒ�ɂȂ��Ă��邪�A
     * CHANNEL �� ALL �ASCANTYPE �� ACTIVE �ł������ق����Ó��Ǝv����B
     */
    wcmw->option = WCM_OPTION_CHANNEL_RDC |
        WCM_OPTION_POWER_SAVE |
        WCM_OPTION_AUTH_OPENSYSTEM |
        WCM_OPTION_SCANTYPE_PASSIVE |
        WCM_OPTION_ROUNDSCAN_IGNORE;
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_GetNextScanChannel

  Description:  �I�v�V�����ɐݒ肳��Ă���T�����̃`�����l�����X�g����A����
                �X�L�������ׂ��`�����l�������肷��B

  Arguments:    channel -   ����X�L���������`�����l����n���B

  Returns:      u16     -   ����X�L�������ׂ��`�����l����Ԃ��B
 *---------------------------------------------------------------------------*/
u16 WCMi_GetNextScanChannel(u16 channel)
{
    s32 i;

    for (i = 0; i < 13; i++)
    {
        if (wcmw->option & (0x0001 << (((channel + i) % 13) + 1)))
        {
            /* TWL �����͎g�p����鍑���Ɏg�p�\�ȃ`�����l�����قȂ�̂ŁA���p�ł���`�����l���݂̂�Ԃ��B */
            if (WCM_GetAllowedChannel() & (0x0001 << (((channel + i) % 13))))
            {
                break;
            }
        }
    }

    return(u16) (((channel + i) % 13) + 1);
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_Notify

  Description:  �񓯊��I�ȏ����̏������ʂ��R�[���o�b�N����B
                �s����ɔ�������ʒm�����̊֐���ʂ��ăR�[���o�b�N�����B
                �ʒm��ʂɂ��Ă� WCM �������ʕϐ����玩���I�ɐݒ肳��A����
                �d�������ʒm������邽�߂ɁA�ʒm����x�ɃN���A�����B

  Arguments:    result  -   �������ʂ��w�肷��B
                para0   -   �ʒm�֐��ɓn���p�����[�^[ 0 ]
                para1   -   �ʒm�֐��ɓn���p�����[�^[ 1 ]
                para2   -   �ʒm�֐��ɓn���p�����[�^[ 2 ]

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_Notify(s16 result, void* para0, void* para1, s32 para2)
{
    s16 notifyId = wcmw->notifyId;

    wcmw->notifyId = WCM_NOTIFY_COMMON;
    WCMi_NotifyEx(notifyId, result, para0, para1, para2);
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_NotifyEx

  Description:  �A�v���P�[�V�����ւ̃R�[���o�b�N�ɂ��ʒm���s���B

  Arguments:    result  -   �������ʂ��w�肷��B
                para0   -   �ʒm�֐��ɓn���p�����[�^[ 0 ]
                para1   -   �ʒm�֐��ɓn���p�����[�^[ 1 ]
                para2   -   �ʒm�֐��ɓn���p�����[�^[ 2 ]

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_NotifyEx(s16 notify, s16 result, void* para0, void* para1, s32 para2)
{
    if (wcmw->notify)
    {
        WCMNotice   notice;

        notice.notify = notify;
        notice.result = result;
        notice.parameter[0].p = para0;
        notice.parameter[1].p = para1;
        notice.parameter[2].n = para2;
        wcmw->notify(&notice);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_SetPhase

  Description:  WCM ���C�u�����̓������ ( �t�F�[�Y ) ��ύX����B
                FATAL_ERROR ��Ԃ���ʂ̃t�F�[�Y�ɕύX�͂ł��Ȃ��B
                �܂��ADCF �ʐM��Ԃւ̃t�F�[�Y�ύX�y�� DCF �ʐM��Ԃ����
                �t�F�[�Y�ύX���Ď����A�L�[�v�A���C�u�p�P�b�g���M�p�A���[����
                ����𐧌䂷��B

  Arguments:    phase   -   �ύX����t�F�[�Y���w��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_SetPhase(u8 phase)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // DCF �ʐM��Ԃ���ʂ̏�ԂɈڍs����ۂɃA���[�����~
    if ((wcmw->phase == WCM_PHASE_DCF) && (phase != WCM_PHASE_DCF))
    {
        // Keep Alive �p�A���[����~
        OS_CancelAlarm(&(wcmw->keepAliveAlarm));
    }

    // FATAL ERROR ��ԂłȂ��ꍇ�͎w���ԂɕύX
    if (wcmw->phase != WCM_PHASE_FATAL_ERROR)
    {
        wcmw->phase = phase;
    }

    // DCF �ʐM��ԂɈڍs����ۂɃA���[�����J�n
    if (phase == WCM_PHASE_DCF)
    {
        // Keep Alive Alarm �̓�d�Ăт�h�~
        OS_CancelAlarm(&(wcmw->keepAliveAlarm));
        // Keep Alive �p�A���[���J�n
        OS_SetAlarm(&(wcmw->keepAliveAlarm), OS_SecondsToTicks(WCM_KEEP_ALIVE_SPAN), WcmKeepAliveAlarm, NULL);
    }
    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_ResetKeepAliveAlarm

  Description:  �L�[�v�A���C�u NULL �p�P�b�g���M�p�A���[�������Z�b�g���A�K��
                ���Ԃ̃A���[����ݒ肵�����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_ResetKeepAliveAlarm(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // ���߂ăA���[�����Z�b�g
    OS_CancelAlarm(&(wcmw->keepAliveAlarm));
    if (wcmw->phase == WCM_PHASE_DCF)
    {
        OS_SetAlarm(&(wcmw->keepAliveAlarm), OS_SecondsToTicks(WCM_KEEP_ALIVE_SPAN), WcmKeepAliveAlarm, NULL);
    }
    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
  Name:         WcmKeepAliveAlarm

  Description:  �L�[�v�A���C�u NULL �p�P�b�g���M�p�A���[���̃A���[���n���h���B

  Arguments:    arg     -   ���g�p�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmKeepAliveAlarm(void* arg)
{
#pragma unused(arg)

    //WCMi_CpsifSendNullPacket();
    WCMi_ResetKeepAliveAlarm();
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_GetDefaultDeviceId

  Description:  

  Arguments:    None.

  Returns:      u8      - 
 *---------------------------------------------------------------------------*/
u8 WCMi_GetDefaultDeviceId(void)
{
#ifdef SDK_NITRO
    return WCM_DEVICEID_WM;
#else  /* !SDK_NITRO */
#ifdef SDK_WIFI_NWM
    if (OS_IsRunOnTwl())
    {
        return WCM_DEVICEID_NWM;
    }
    else
#endif /* SDK_WIFI_NWM */
    {
        return WCM_DEVICEID_WM;
    }
#endif /* !SDK_NITRO */
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_OverrideDefaultDeviceId

  Description:  

  Arguments:    deviceId     - 

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_OverrideDefaultDeviceId(u8 deviceId)
{
    if (WCM_DEVICEID_DEFAULT <= deviceId && deviceId <= WCM_DEVICEID_NWM)
    {
        WCMi_OverridedDeviceId = deviceId;
    }
    else
    {
        OS_Panic("Invalide device Id.");
    }
}


#ifndef SDK_FINALROM
/*---------------------------------------------------------------------------*
  Name:         WCMi_DumpMemory

  Description:  

  Arguments:    pDump        - 
                len          - 

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_DumpMemory(const u8* pDump, u32 len)
{
    int     i;
    for (i = 0; i < len; i++)
    {
        if (i % 16)
        {
            OS_TPrintf(" ");
        }

        OS_TPrintf("%02X", pDump[i]);
        if ((i % 16) == 15)
        {
            OS_TPrintf("\n");
        }
    }
    OS_TPrintf("\n");
}
#endif
/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
