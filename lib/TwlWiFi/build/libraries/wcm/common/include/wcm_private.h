/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - WCM - libraries
  File:     wcm_private.h

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

#ifndef NITROWIFI_WCM_PRIVATE_H_
#define NITROWIFI_WCM_PRIVATE_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/
#include <nitroWiFi/wcm.h>
#include <nitro/os.h>

#include "wcm_wmif.h"
#ifdef SDK_TWL
#include "wcm_nwmif.h"
#endif

#include "wcm_cpsif.h"

/*---------------------------------------------------------------------------*
    �萔 ��`
 *---------------------------------------------------------------------------*/

// ���Z�b�g�d���Ăяo���Ǘ��t���O�̏��
#define WCM_RESETTING_OFF   0
#define WCM_RESETTING_ON    1

// AP ���ێ����X�g���̊e�u���b�N�̎g�p���
#define WCM_APLIST_BLOCK_EMPTY  0
#define WCM_APLIST_BLOCK_OCCUPY 1

// Keep Alive �p NULL �p�P�b�g���M�Ԋu(�b)
#define WCM_KEEP_ALIVE_SPAN ((OSTick) 70)

// �f�o�b�O�\���L���ݒ�
#ifdef SDK_FINALROM
#define WCM_DEBUG   0
#else
#define WCM_DEBUG   1
#endif

// �ڑ����̎��s�R�[�h��`
#define WCM_CONNECT_STATUSCODE_ILLEGAL_RATESET  18

#if WCM_DEBUG
#define WCMi_Printf     OS_TPrintf("[WCM] %s(%d) Report: ", __FILE__, __LINE__); \
    OS_TPrintf
#define WCMi_Warning    OS_TPrintf("[WCM] %s(%d) Warning: ", __FILE__, __LINE__); \
    OS_TPrintf
#else
#define WCMi_Printf(...)    ((void)0)
#define WCMi_Warning(...)   ((void)0)
#endif

/*---------------------------------------------------------------------------*
    �\���� ��`
 *---------------------------------------------------------------------------*/

// WCM �������[�N�ϐ��Q���܂Ƃ߂��\����
// TWL �r���h�̎��̓T�C�Y��������BNITRO �r���h�̏ꍇ�A�\���̃����o�̏��Ԃ͈Ⴄ
// ���K�v�ȃ��[�N�T�C�Y���̂��̂� NITRO-WiFi �Ɠ����B
typedef struct WCMWork
{
    u8              sendBuf[WCM_DCF_SEND_BUF_SIZE];
    u8              recvBuf[WCM_DCF_RECV_BUF_SIZE * 2];
    
    // key �� 32 �o�C�g�����ɂ��邽�߁AWCMPrivacyDesc �Ɠ�����
    // �ϐ��̏��Ԃ�ς��������̍\���̂��g��
    struct
    {
        u8  key[WM_SIZE_WEPKEY];
        u8  mode;                   // �Í������[�h ( WCM_PRIVACYMODE_* )
        union
        {
            u8  keyId;              // mode <= WCM_PRIVACYMODE_128 �̎�
            u8  keyLen;             // mode >  WCM_PRIVACYMODE_128 �̎� (WPA)
        };
        u8  _reserved[2];
    } privacyDesc;
    
    OSAlarm         keepAliveAlarm;
    
    WCMConfig       config;
    WCMNotify       notify;
    
    u32             option;
    u16             maxScanTime;
    s16             notifyId;
    
    u8              apListLock;
    u8              resetting;
    
    u8              phase;
    u8              deviceId;
    
    // �������� 32 �o�C�g����
    WCMBssDesc      bssDesc;
    union
    {
        // �������p
        struct
        {
            u8              work[WCM_ROUNDUP32( WM_SYSTEM_BUF_SIZE )];
            WMScanExParam   scanExParam;
            // ���ʃp�����[�^�ɂ��������A�\���̃T�C�Y�̐����ɂ�肱�̏ꏊ�ɁB
            u32             scanCount;
            u16             authId;
            // WL �̌��ʃR�[�h
            u16             wlStatusOnConnectFail;
        } wm;
        
#ifdef SDK_TWL
        // �V�����p
        // deviceId == WCM_DEVICEID_NWM �̎��̂݃A�N�Z�X�ł���
        // ����ȊO�̓��������m�ۂ���Ă��Ȃ���������Ȃ�
        struct
        {
            u8              work[WCM_ROUNDUP32( NWM_SYSTEM_BUF_SIZE )];
            NWMScanExParam  scanExParam;
            u32             scanCount;
            u16             authId;
            // ���ԈႢ���n���h�����O���邽�߂̋^�� WL �̌��ʃR�[�h
            u16             pseudoStatusOnConnectFail;
        } nwm;
#endif
    };
} WCMWork;

// WCM CPS-IF ������ԊǗ��p�\����
typedef struct WCMCpsifWork
{
    u8  initialized;                // ���[�N�ϐ��������m�F�p
    u8  reserved[3];                // �p�f�B���O  (OSThreadQueue ��4byte�ȏ��)
    OSThreadQueue   sendQ;          // DCF ���M�����҂��u���b�N�p Thread Queue
    OSMutex         sendMutex;      // �����X���b�h�ɂ�鑗�M�̔r������p Mutex
    u16             sendResult;     // DCF ���M�̔񓯊��I�ȏ������ʑޔ��G���A
    u16             padding1;
    WCMRecvInd      recvCallback;   // DCF ��M�R�[���o�b�N��ޔ��G���A
    WCMSendInd      sendCallback;   // DCF ���M�R�[���o�b�N��ޔ��G���A
    u8              macAddress[6];
    u16             padding2;
} WCMCpsifWork;

// AP ���ێ����X�g���̃u���b�N�\����
typedef struct WCMApList
{
    u8      state;
    u8      reserved;
    u16     linkLevel;
    u32     index;
    void*   previous;
    void*   next;
    u32     data[WCM_ROUNDUP4( WCM_APLIST_SIZE ) / sizeof(u32) ];
} WCMApList;

// AP ���ێ����X�g�̊Ǘ��p�w�b�_�\����
typedef struct WCMApListHeader
{
    u32         count;
    WCMApList*  head;
    WCMApList*  tail;

} WCMApListHeader;

/*---------------------------------------------------------------------------*
    �֐� ��`
 *---------------------------------------------------------------------------*/

// WCM ���C�u�����̓����Ǘ��\���̂ւ̃|�C���^���擾
WCMWork*    WCMi_GetSystemWork(void);

// AP ��񔭌����ɌĂяo����� �ێ����X�g�ւ̃G���g���[�ǉ�(�X�V)�v��
//void        WCMi_EntryApList(WMBssDesc* bssDesc, u16 linkLevel);
void        WCMi_EntryApList(WCMBssDesc* bssDesc, u16 linkLevel);

// Keep Alive �p�A���[�������Z�b�g����֐�
void        WCMi_ResetKeepAliveAlarm(void);

// DCF �f�[�^��M���Ɏ�M�d�g���x����U�ޔ�����֐�
void        WCMi_ShelterRssi(u8 rssi);
u8          WCMi_GetRssiAverage(void);

// WCM ���C�u�����̓������ ( �t�F�[�Y ) ��ύX����֐�
void        WCMi_SetPhase(u8 phase);

// �A�v���P�[�V�����ւ̃R�[���o�b�N�ɂ��ʒm���s���֐�
void        WCMi_Notify(s16 result, void* para0, void* para1, s32 para2);
void        WCMi_NotifyEx(s16 notify, s16 result, void* para0, void* para1, s32 para2);

// ���ɃX�L�������ׂ��`�����l�������肷��֐�
u16         WCMi_GetNextScanChannel(u16 channel);

// �������̃_���v�֐�
void        WCMi_DumpMemory(const u8* pDump, u32 len);
// �f�o�b�O�p�t���O
extern u32  WCM_DebugFlags;


/*
   cpsif.c
 */
// WCM_Init �֐�����Ăяo����� CPS �C���^�[�t�F�[�X�������֐�
void        WCMi_InitCpsif(void);

// DCF �f�[�^��M���ɌĂяo����� CPS �C���^�[�t�F�[�X�ւ̃R�[���o�b�N�֐�
void        WCMi_CpsifRecvCallback(u8 *srcAdrs, u8 *destAdrs, u8 *data, s32 length);

// Keep Alive �p NULL �p�P�b�g�𑗐M����֐�
void        WCMi_CpsifSendNullPacket(void);

WCMCpsifWork* WCMi_GetCpsifWork(void);
BOOL        WCMi_CpsifTryLockMutexInIRQ(OSMutex* mutex);
void        WCMi_CpsifUnlockMutexInIRQ(OSMutex* mutex);
void        WCMi_CpsifSetMacAddress(u8* macAddress);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* NITROWIFI_WCM_PRIVATE_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
