/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - include
  File:     nwm.h

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef LIBRARIES_NWM_ARM9_NWM_H__
#define LIBRARIES_NWM_ARM9_NWM_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include <twl.h>
#include <nitro/wm.h>

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/

#define NWM_NUM_MAX_AP_AID              2007   // �������[�^����^����ꂤ�� AssociationID �̍ő�l

#define NWM_SIZE_SSID                     32
#define NWM_SIZE_MACADDR                   6
#define NWM_SIZE_BSSID                    NWM_SIZE_MACADDR

#define NWM_SIZE_WEP_40BIT                 5
#define NWM_SIZE_WEP_104BIT               13
#define NWM_SIZE_WEP_128BIT               16

#define NWM_SIZE_WEP                      20   // WM(DS�ڑ��ݒ�) �Ƃ̌݊����m�ۂ̂���

#define NWM_WEPMODE_OPEN                   WM_WEPMODE_NO
#define NWM_WEPMODE_40BIT                  WM_WEPMODE_40BIT
#define NWM_WEPMODE_104BIT                 WM_WEPMODE_104BIT
#define NWM_WEPMODE_128BIT                 WM_WEPMODE_128BIT

#define NWM_WPAMODE_WPA_TKIP               (WM_WEPMODE_128BIT + 1)
#define NWM_WPAMODE_WPA2_TKIP              (WM_WEPMODE_128BIT + 2)
#define NWM_WPAMODE_WPA_AES                (WM_WEPMODE_128BIT + 3)
#define NWM_WPAMODE_WPA2_AES               (WM_WEPMODE_128BIT + 4)


#define NWM_BSS_DESC_SIZE                 64   // NWM_StartScan()�ň����n���A�e�@���i�[�p�o�b�t�@�̃T�C�Y
#define NWM_BSS_DESC_SIZE_MAX            512
#define NWM_FRAME_SIZE_MAX              1522   // �ő�f�[�^�t���[���T�C�Y(1500(MTU) + 22(802.3 header))
#define NWM_SCAN_NODE_MAX                 32   // 1���Scan�Ŕ����\�ȍő�e�@�䐔
#define NWM_SIZE_SCANBUF_MAX           65535   // �ő�̃o�b�t�@�T�C�Y

#define NWM_SYSTEM_BUF_SIZE           (0x00003000)

// bitmask for capability information
#define NWM_CAPABILITY_ESS_MASK         0x0001
#define NWM_CAPABILITY_ADHOC_MASK       0x0002
#define NWM_CAPABILITY_CP_POLLABLE_MASK 0x0004
#define NWM_CAPABILITY_CP_POLL_REQ_MASK 0x0008
#define NWM_CAPABILITY_PRIVACY_MASK     0x0010
#define NWM_CAPABILITY_SPREAMBLE_MASK   0x0020
#define NWM_CAPABILITY_PBCC_MASK        0x0040
#define NWM_CAPABILITY_CH_AGILITY_MASK  0x0800
#define NWM_CAPABILITY_SPECTRUM_MASK    0x0100
#define NWM_CAPABILITY_QOS_MASK         0x0200
#define NWM_CAPABILITY_SSLOTTIME_MASK   0x0400
#define NWM_CAPABILITY_APSD_MASK        0x0800


#define NWM_SCANTYPE_PASSIVE    0
#define NWM_SCANTYPE_ACTIVE     1

#define NWM_DEFAULT_PASSIVE_SCAN_PERIOD 105 /* NWM �ł� WM �����f�t�H���g�l���傫�����Ƃɒ��� */
#define NWM_DEFAULT_ACTIVE_SCAN_PERIOD   30 /* NWM �ł� �A�N�e�B�u�X�L�����ƃp�b�V�u�X�L�����̃f�t�H���g�l���Ⴄ���Ƃɒ��� */

#define NWM_RATESET_1_0M                0x0001
#define NWM_RATESET_2_0M                0x0002
#define NWM_RATESET_5_5M                0x0004
#define NWM_RATESET_6_0M                0x0008
#define NWM_RATESET_9_0M                0x0010
#define NWM_RATESET_11_0M               0x0020
#define NWM_RATESET_12_0M               0x0040
#define NWM_RATESET_18_0M               0x0080
#define NWM_RATESET_24_0M               0x0100
#define NWM_RATESET_36_0M               0x0200
#define NWM_RATESET_48_0M               0x0400
#define NWM_RATESET_54_0M               0x0800
#define NWM_RATESET_11B_MASK            ( NWM_RATESET_1_0M | NWM_RATESET_2_0M | NWM_RATESET_5_5M | NWM_RATESET_11_0M )
#define NWM_RATESET_11G_MASK            ( NWM_RATESET_1_0M | NWM_RATESET_2_0M | NWM_RATESET_5_5M | NWM_RATESET_11_0M     \
                                        | NWM_RATESET_6_0M | NWM_RATESET_9_0M | NWM_RATESET_12_0M | NWM_RATESET_18_0M    \
                                        | NWM_RATESET_24_0M | NWM_RATESET_36_0M | NWM_RATESET_48_0M | NWM_RATESET_54_0M )

#define NWM_WPA_PSK_LENGTH               32
#define NWM_WPA_PASSPHRASE_LENGTH_MAX    64


/* �����N���x�� */
#define NWM_RSSI_INFRA_LINK_LEVEL_1  12
#define NWM_RSSI_INFRA_LINK_LEVEL_2  17
#define NWM_RSSI_INFRA_LINK_LEVEL_3  22

#define NWM_RSSI_ADHOC_LINK_LEVEL_1  12 /* [TODO] TBD */
#define NWM_RSSI_ADHOC_LINK_LEVEL_2  17 /* [TODO] TBD */
#define NWM_RSSI_ADHOC_LINK_LEVEL_3  22 /* [TODO] TBD */

/*---------------------------------------------------------------------------*
    �}�N����`
 *---------------------------------------------------------------------------*/


// �eAPI��ID
typedef enum NWMApiid
{
    NWM_APIID_INIT = 0,                 // NWM_Init()
    NWM_APIID_RESET,                    // NWM_Reset()
    NWM_APIID_END,                      // NWM_End()

    NWM_APIID_LOAD_DEVICE,              // NWM_LoadDevice()
    NWM_APIID_UNLOAD_DEVICE,            // NWM_UnloadDevice()
    NWM_APIID_OPEN,                     // NWM_Open()
    NWM_APIID_CLOSE,                    // NWM_Close()

    NWM_APIID_START_SCAN,               // NWM_StartScan()
    NWM_APIID_CONNECT,                  // NWM_Connect()
    NWM_APIID_DISCONNECT,               // NWM_Disconnect()
    NWM_APIID_SET_RECEIVING_BUF,        // NWM_SetReceivingFrameBuffer()
    NWM_APIID_SEND_FRAME,               // NWM_SendFrame()
    NWM_APIID_UNSET_RECEIVING_BUF,      // NWM_UnsetReceivingFrameBuffer()
    NWM_APIID_SET_WEPKEY,               // NWM_SetWEPKey()
    NWM_APIID_SET_PS_MODE,              // NWM_SetPowerSaveMode()

    NWM_APIID_SET_WPA_KEY,              //
    NWM_APIID_SET_WPA_PARAMS,           //

    NWM_APIID_CREATE_QOS,               //
    NWM_APIID_SET_WPA_PSK,              // NWM_SetWPAPSK()
    NWM_APIID_INSTALL_FIRMWARE,         // NWMi_InstallFirmware()
    NWM_APIID_ASYNC_KIND_MAX,           //  : �񓯊������̎��
  
    NWM_APIID_INDICATION = 128,         //  : Indication�R�[���o�b�N�p

    NWM_APIID_UNKNOWN = 255             //  : �s���̃R�}���h�ԍ�����ARM7����Ԃ��l
}
NWMApiid;


// API�̃��U���g�R�[�h
typedef enum NWMRetCode
{
    NWM_RETCODE_SUCCESS         =  0,
    NWM_RETCODE_FAILED          =  1,
    NWM_RETCODE_OPERATING       =  2,
    NWM_RETCODE_ILLEGAL_STATE   =  3,
    NWM_RETCODE_NWM_DISABLE     =  4,
    NWM_RETCODE_INVALID_PARAM   =  5,
    NWM_RETCODE_FIFO_ERROR      =  6,
    NWM_RETCODE_FATAL_ERROR     =  7,   // �\�t�g�E�F�A�ł̑Ή����s�\�ȃG���[
    NWM_RETCODE_NETBUF_ERROR    =  8,
    NWM_RETCODE_WMI_ERROR       =  9,
    NWM_RETCODE_SDIO_ERROR      = 10,
    NWM_RETCODE_RECV_IND        = 11,
    NWM_RETCODE_INDICATION      = 12,   // internal use only
  
    NWM_RETCODE_MAX
} NWMRetCode;

// NWM �̃X�e�[�g�R�[�h
typedef enum NWMState
{
    NWM_STATE_NONE            = 0x0000,
    NWM_STATE_INITIALIZED     = 0x0001,    // INITIALIZED state
    NWM_STATE_LOADED          = 0x0002,    // LOADED state
    NWM_STATE_DISCONNECTED    = 0x0003,    // DISCONNECTED state
    NWM_STATE_INFRA_CONNECTED = 0x0004,    // CONNECTED STA (infrastructure) state
    NWM_STATE_ADHOC_CONNECTED = 0x0005     // CONNECTED STA (Ad Hoc) state
}
NWMState;

typedef enum NWMReasonCode
{
    NWM_REASON_API_SUCCESS          = 0,
  
    /* for infra mode */
    NWM_REASON_NO_NETWORK_AVAIL     = 1,
    NWM_REASON_LOST_LINK            = 2,
    NWM_REASON_DISCONNECT_CMD       = 3,
    NWM_REASON_BSS_DISCONNECTED     = 4,
    NWM_REASON_AUTH_FAILED          = 5,
    NWM_REASON_ASSOC_FAILED         = 6,
    NWM_REASON_NO_RESOURCES_AVAIL   = 7,
    NWM_REASON_CSERV_DISCONNECT     = 8,
    NWM_REASON_INVAILD_PROFILE      = 9,

    NWM_REASON_WEP_KEY_ERROR        =10,

    /* for WPA supplicant */
    NWM_REASON_WPA_KEY_ERROR        =11,
    NWM_REASON_TKIP_MIC_ERROR       =12,
    
    /* for Wireless QoS (802.11e) */
    NWM_REASON_NO_QOS_RESOURCES_AVAIL   = 13,
    
    NWM_REASON_UNKNOWN

} NWMReasonCode;

typedef enum NWMAuthMode
{
  NWM_AUTHMODE_OPEN,    /* dot11 authentication */
  NWM_AUTHMODE_SHARED,  /* dot11 authentication */
  NWM_AUTHMODE_WPA_PSK_TKIP         = NWM_WPAMODE_WPA_TKIP,   /* WCM_WEPMODE_* �Ɣ��Ȃ��悤�ɒ��� */
  NWM_AUTHMODE_WPA2_PSK_TKIP        = NWM_WPAMODE_WPA2_TKIP,
  NWM_AUTHMODE_WPA_PSK_AES          = NWM_WPAMODE_WPA_AES,
  NWM_AUTHMODE_WPA2_PSK_AES         = NWM_WPAMODE_WPA2_AES
}
NWMAuthMode, NWMauthMode;


// Element ID of Infomation Elements
typedef enum NWMElementID {
  NWM_ELEMENTID_SSID               = 0,
  NWM_ELEMENTID_SUPPORTED_RATES    = 1,
  NWM_ELEMENTID_FH_PARAMETER_SET   = 2,
  NWM_ELEMENTID_DS_PARAMETER_SET   = 3,
  NWM_ELEMENTID_CF_PARAMETER_SET   = 4,
  NWM_ELEMENTID_TIM                = 5,
  NWM_ELEMENTID_IBSS_PARAMETER_SET = 6,
  NWM_ELEMENTID_COUNTRY            = 7,
  NWM_ELEMENTID_HP_PARAMETERS      = 8,
  NWM_ELEMENTID_HP_TABLE           = 9,
  NWM_ELEMENTID_REQUEST            = 10,
  NWM_ELEMENTID_QBSS_LOAD          = 11,
  NWM_ELEMENTID_EDCA_PARAMETER_SET = 12,
  NWM_ELEMENTID_TSPEC              = 13,
  NWM_ELEMENTID_TRAFFIC_CLASS      = 14,
  NWM_ELEMENTID_SCHEDULE           = 15,
  NWM_ELEMENTID_CHALLENGE_TEXT     = 16,

  NWM_ELEMENTID_POWER_CONSTRAINT   = 32,
  NWM_ELEMENTID_POWER_CAPABILITY   = 33,
  NWM_ELEMENTID_TPC_REQUEST        = 34,
  NWM_ELEMENTID_TPC_REPORT         = 35,
  NWM_ELEMENTID_SUPPORTED_CHANNELS = 36,
  NWM_ELEMENTID_CH_SWITCH_ANNOUNCE = 37,
  NWM_ELEMENTID_MEASURE_REQUEST    = 38,
  NWM_ELEMENTID_MEASURE_REPORT     = 39,
  NWM_ELEMENTID_QUIET              = 40,
  NWM_ELEMENTID_IBSS_DFS           = 41,
  NWM_ELEMENTID_ERP_INFORMATION    = 42,
  NWM_ELEMENTID_TS_DELAY           = 43,
  NWM_ELEMENTID_TCLASS_PROCESSING  = 44,
  NWM_ELEMENTID_HT_CAPABILITY      = 45,
  NWM_ELEMENTID_QOS_CAPABILITY     = 46,
  NWM_ELEMENTID_RSN                = 48,
  NWM_ELEMENTID_EX_SUPPORTED_RATES = 50,
  NWM_ELEMENTID_HT_INFORMATION     = 61,
  
  NWM_ELEMENTID_VENDOR             = 221,
  NWM_ELEMENTID_NINTENDO           = 221
}
NWMElementID;

typedef enum NWMPowerMode {
  NWM_POWERMODE_ACTIVE,
  NWM_POWERMODE_STANDARD,
  NWM_POWERMODE_UAPSD
} NWMPowerMode;

typedef enum NWMAccessCategory {
  NWM_AC_BE,          /* best effort */
  NWM_AC_BK,          /* background */
  NWM_AC_VI,          /* video */
  NWM_AC_VO,          /* voice */
  NWM_AC_NUM
} NWMAccessCategory;

typedef enum NWMNwType
{
  NWM_NWTYPE_INFRA,
  NWM_NWTYPE_ADHOC,
  NWM_NWTYPE_WPS,
  NWM_NWTYPE_NUM
} NWMNwType;

typedef enum NWMFramePort
{
    NWM_PORT_IPV4_ARP, /* for TCP/IP */
    NWM_PORT_EAPOL,    /* for WPA supplicant */
    NWM_PORT_OTHERS,
    NWM_PORT_NUM
} NWMFramePort, NWMframePort;

typedef void (*NWMCallbackFunc) (void *arg);     // NWM API�̃R�[���o�b�N�^

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/

typedef struct NWMBssDesc
{
    u16     length;                    // 2
    s16     rssi;                      // 4
    u8      bssid[NWM_SIZE_BSSID];     // 10
    u16     ssidLength;                // 12
    u8      ssid[NWM_SIZE_SSID];       // 44
    u16     capaInfo;                  // 46
    struct
    {
        u16     basic;                 // 48
        u16     support;               // 50
    }
    rateSet;
    u16     beaconPeriod;              // 52
    u16     dtimPeriod;                // 54
    u16     channel;                   // 56
    u16     cfpPeriod;                 // 58
    u16     cfpMaxDuration;            // 60
    u16     reserved;                  // 62 just for compatibility with WM (must be 0)
    u16     elementCount;              // 64
    u16     elements[1];
} NWMBssDesc, NWMbssDesc;


typedef struct NWMInfoElements
{
  u8  id;
  u8  length;
  u16 element[1];
}
NWMInfoElements;


//---------------------------------------
// NWM  Scan �p�����[�^�\����
typedef struct NWMScanParam
{
    NWMBssDesc *scanBuf;                   // �e�@�����i�[���邽�߂̃o�b�t�@
    u16         scanBufSize;               // scanBuf�̃T�C�Y
    u16         channelList;               // Scan���s��channel���X�g(�����w���)
    u16         channelDwellTime;          // 1�`�����l���������Scan����(ms)
    u16         scanType;                  // passive or active
    u8          bssid[NWM_SIZE_BSSID];     // Scan�Ώ�MacAddress(0xff�Ȃ�S�e�@�Ώ�)
    u16         ssidLength;                // Scan�Ώۂ�SSID��(0�̏ꍇ�͑S�m�[�h���ΏۂɂȂ�)
    u8          ssid[NWM_SIZE_SSID];       // Scan�Ώۂ�SSID
    u16         rsv[6];
} NWMScanParam, NWMscanParam;

// NWM  Scan �p�����[�^�\����
typedef struct NWMScanExParam
{
    NWMBssDesc *scanBuf;                   // �e�@�����i�[���邽�߂̃o�b�t�@
    u16         scanBufSize;               // scanBuf�̃T�C�Y
    u16         channelList;               // Scan���s��channel���X�g(�����w���)
    u16         channelDwellTime;          // 1�`�����l���������Scan����(ms)
    u16         scanType;                  // passive or active
    u8          bssid[NWM_SIZE_BSSID];     // Scan�Ώ�MacAddress(0xff�Ȃ�S�e�@�Ώ�)
    u16         ssidLength;                // Scan�Ώۂ�SSID��(0�̏ꍇ�͑S�m�[�h���ΏۂɂȂ�)
    u8          ssid[NWM_SIZE_SSID];       // Scan�Ώۂ�SSID
    u16         ssidMatchLength;           // �X�L�����Ώۂ� SSID �}�b�`���O��
    u16         rsv[5];
} NWMScanExParam, NWMscanExParam;


//---------------------------------------
// NWM  WPA parameter structure

typedef struct NWMWpaParam {
    u16   auth;     // NWMAuthMode (can use PSK only)
    u8    psk[NWM_WPA_PSK_LENGTH];
} NWMWpaParam;

// Frame��M�o�b�t�@�\����
typedef struct NWMRecvFrameHdr
{
  u8   da[NWM_SIZE_MACADDR];
  u8   sa[NWM_SIZE_MACADDR];
  u8   pid[2];
  u8   frame[2];
} NWMRecvFrameHdr;

//==========================================================================================

// �ʏ�̃R�[���o�b�N�̈���
typedef struct NWMCallback
{
    u16     apiid;
    u16     retcode;

} NWMCallback;

// NWM_StartScan()�̃R�[���o�b�N�̈���
typedef struct NWMStartScanCallback
{
    u16     apiid;
    u16     retcode;
    u32     channelList;               // �����E�������Ɋւ�炸�A�X�L���������`�����l�����X�g
    u8      reserved[2];               // padding
    u16     bssDescCount;              // �������ꂽ�e�@�̐�
    u32     allBssDescSize;
    NWMBssDesc *bssDesc[NWM_SCAN_NODE_MAX];  // �e�@���̐擪�A�h���X
    u16     linkLevel[NWM_SCAN_NODE_MAX];   // ��M�d�g���x

} NWMStartScanCallback, NWMstartScanCallback;

// NWM_Connect()�̃R�[���o�b�N�̈���
typedef struct NWMConnectCallback
{
    u16     apiid;
    u16     retcode;
    u16     channel;
    u8      bssid[NWM_SIZE_BSSID];
    s8      rssi;
    u8      padding;
    u16     aid;                       // CONNECTED�̂Ƃ��̂݁A���g�Ɋ��蓖�Ă�ꂽAID
    u16     reason;                    // �ؒf����reason. NWMReasonCode �Œ�`�����B
    u16     listenInterval;
    u8      networkType;
    u8      beaconIeLen;
    u8      assocReqLen;
    u8      assocRespLen;
    u8      assocInfo[2]; /* this field consists of beaconIe, assocReq, assocResp */
} NWMConnectCallback, NWMconnectCallback;

// NWM_Disconnect()�̃R�[���o�b�N�̈���
typedef struct NWMDisconnectCallback
{
    u16   apiid;
    u16   retcode;
    u16   reason;
    u16   rsv;
} NWMDisconnectCallback, NWMdisconnectCallback;

// NWM_SendFrame()�̃R�[���o�b�N�̈���
typedef struct NWMSendFrameCallback
{
    u16     apiid;
    u16     retcode;
    NWMCallbackFunc callback;
} NWMSendFrameCallback;

// NWM_SetReceivingFrameBuffer()�̃R�[���o�b�N�̈���
typedef struct NWMReceivingFrameCallback
{
  u16   apiid;
  u16   retcode;
  u16   port;
  u16   rssi;
  u32   length;
  NWMRecvFrameHdr *recvBuf;

} NWMReceivingFrameCallback;


/*===========================================================================
  NWM APIs
  ===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         NWM_Init

  Description:  NWM���C�u�����̏������������s���B
                ARM9���̏������݂̂��s�������֐��B

  Arguments:    sysBuf     -   �Ăяo�����ɂ���Ċm�ۂ��ꂽ�o�b�t�@�ւ̃|�C���^�B

                bufSize    -   �Ăяo�����ɂ���Ċm�ۂ��ꂽ�o�b�t�@�̃T�C�Y�B

                dmaNo      -   NWM���g�p����DMA�ԍ��B

  Returns:      NWMRetCode -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_Init(void* sysBuf, u32 bufSize, u8 dmaNo);

/*---------------------------------------------------------------------------*
  Name:         NWM_Reset

  Description:  TWL�����h���C�o���N���������ATWL�������W���[�������Z�b�g����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_Reset(NWMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         NWM_LoadDevice

  Description:  TWL�������W���[�����N������B
                ������Ԃ� NWM_STATE_INITIALIZED ��Ԃ��� NWM_STATE_LOADED ���
                �ɑJ�ڂ����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_LoadDevice(NWMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         NWM_UnloadDevice

  Description:  TWL�������W���[�����V���b�g�_�E������B
                ������Ԃ� NWM_STATE_LOADED ��Ԃ��� NWM_STATE_INITIALIZED ���
                �ɑJ�ڂ����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_UnloadDevice(NWMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         NWM_Open

  Description:  TWL�������g�p�\��Ԃ�ON�ɂ���B
                ������Ԃ� NWM_STATE_LOADED ��Ԃ��� NWM_STATE_DISCONNECTED ���
                �ɑJ�ڂ����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_Open(NWMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         NWM_Close

  Description:  TWL�������g�p�\��Ԃ�OFF�ɂ���B
                ������Ԃ� NWM_STATE_DISCONNECTED  ��Ԃ���
                NWM_STATE_LOADED ��ԂɑJ�ڂ����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_Close(NWMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         NWM_End

  Description:  NWM���C�u�����̏I���������s���B
                ARM9���̏I���݂̂��s�������֐��B

  Arguments:    None

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_End(void);

/*---------------------------------------------------------------------------*
  Name:         NWM_StartScan

  Description:  AP�̃X�L�������J�n����B
                ��x�̌Ăяo���ŕ�����̐e�@�����擾�ł���B
                �X�L�������ƃX�L������ł̏�ԑJ�ڂ͂Ȃ��B

  Arguments:    callback     -  �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                                ������ NWMStartScanCallback �\���̂̌`�ŕԂ����B
                param        -  �X�L�����ݒ���������\���̂ւ̃|�C���^�B
                                �X�L�������ʂ̏��� param->scanBuf �� ARM7 �����ڏ����o��
                                �̂ŁA�L���b�V�����C���ɍ��킹�Ă����K�v������B
                                �X�̃p�����[�^�� NWMScanParam �\���̂��Q�ƁB

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_StartScan(NWMCallbackFunc callback, const NWMScanParam *param);

/*---------------------------------------------------------------------------*
  Name:         NWM_StartScanEx

  Description:  AP�̃X�L�������J�n����B
                ��x�̌Ăяo���ŕ�����̐e�@�����擾�ł���B
                �X�L�������ƃX�L������ł̏�ԑJ�ڂ͂Ȃ��B
                �X�L�������ʂ�SSID�̕����}�b�`���O�����邱�Ƃ��ł���B

  Arguments:    callback     -  �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                                ������ NWMStartScanCallback �\���̂̌`�ŕԂ����B
                param        -  �X�L�����ݒ���������\���̂ւ̃|�C���^�B
                                ssidMatchLength �Ŏw�肵�����������A�X�L�����Ώۂ�SSID��
                                �������W���[������̃X�L�������ʂ��ƍ����A
                                �Y��������̂̂݃X�L�������ʂƂ��ď����o���B
                                �X�L�������ʂ̏��� param->scanBuf �� ARM7 �����ڏ����o��
                                �̂ŁA�L���b�V�����C���ɍ��킹�Ă����K�v������B
                                �X�̃p�����[�^�� NWMScanExParam �\���̂��Q�ƁB

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_StartScanEx(NWMCallbackFunc callback, const NWMScanExParam *param);

/*---------------------------------------------------------------------------*
  Name:         NWM_Connect

  Description:  AP�ւ̐ڑ����J�n����BDS�e�@�ւ̐ڑ��͕s�B

  Arguments:    callback     -  �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                                ������ NWMConnectCallback �\���̂̌`�ŕԂ����B
                nwType       -  NWMNwType �Œ�`�����l���Ƃ�B
                                �ʏ�� NWM_NWTYPE_INFRA ���g�p���Ă��������B
                pBdesc       -  �ڑ�����AP�̏��B
                                NWM_StartScan�ɂĎ擾�����\���̂��w�肷��B
                                ���̍\���̂̎��̂͋����I�ɃL���b�V���X�g�A�����B
                                �X�̃p�����[�^�� NWMBssDesc �\���̂��Q�ƁB

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_Connect(NWMCallbackFunc callback, u8 nwType, const NWMBssDesc *pBdesc);

/*---------------------------------------------------------------------------*
  Name:         NWM_Disconnect

  Description:  AP�ւ̐ڑ���ؒf����B

  Arguments:    callback     -  �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                                ������ NWMDisconnectCallback �\���̂̌`�ŕԂ����B

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_Disconnect(NWMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         NWM_SetWEPKey

  Description:  WEP�Í����[�h�AWEP�Í��L�[��ݒ肷��B
                NWM_Connect���ĂԑO�ɐݒ肵�Ă����K�v������B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                wepmode     -   NWM_WEPMODE_OPEN  : �Í��@�\�Ȃ�
                                NWM_WEPMODE_40BIT : RC4(40bit)�Í����[�h
                                NWM_WEPMODE_104BIT: RC4(104bit)�Í����[�h
                                NWM_WEPMODE_128BIT: RC4(128bit)�Í����[�h
                wepkeyid    -   4�w�肵��wepkey�̂ǂ���f�[�^���M�Ɏg�p���邩��I�����܂��B
                                0�`3�Ŏw�肵�܂��B
                wepkey      -   �Í��L�[�f�[�^( 80�o�C�g )�ւ̃|�C���^�B
                                �L�[�f�[�^��4�̃f�[�^�ō\������A���ꂼ��20�o�C�g�B
                                �e20�o�C�g�̂����A
                                 40�r�b�g���[�h�ł�  5 �o�C�g
                                104�r�b�g���[�h�ł� 13 �o�C�g
                                128�r�b�g���[�h�ł� 16 �o�C�g
                                �̃f�[�^���g�p�����B
                                �܂��A���̃f�[�^�̎��̂͋����I�ɃL���b�V���X�g�A�����B
                authMode    -   �ڑ����̔F�ؕ����B
                                NWM_AUTHMODE_OPEN  : �I�[�v���F��
                                NWM_AUTHMODE_SHARED: ���L���F��

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_SetWEPKey(NWMCallbackFunc callback, u16 wepmode, u16 wepkeyid, const u8 *wepkey, u16 authMode);

/*---------------------------------------------------------------------------*
  Name:         NWM_Passphrase2PSK

  Description:  WPA�p�X�t���[�Y����APSK���v�Z����B�����֐��B

  Arguments:    passphrase   -   WPA�p�X�t���[�Y�������o�b�t�@�ւ̃|�C���^�B
                                 �ő�63�o�C�g�B
                ssid         -   �ڑ���AP��SSID�������o�b�t�@�ւ̃|�C���^
                ssidlen      -   �ڑ���AP��SSID�̃T�C�Y
                psk          -   �v�Z���ꂽPSK���i�[����o�b�t�@�ւ̃|�C���^�B
                                 32�o�C�g�Œ�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NWM_Passphrase2PSK(const u8 passphrase[NWM_WPA_PASSPHRASE_LENGTH_MAX], const u8 *ssid, u8 ssidlen, u8 psk[NWM_WPA_PSK_LENGTH]);

/*---------------------------------------------------------------------------*
  Name:         NWM_SetWPAPSK

  Description:  WPA�Í����[�h�AWPA�Í��L�[��ݒ肷��B
                NWM_Connect���ĂԑO�ɐݒ肵�Ă����K�v������B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                param       -   WPA�ݒ���������\���̂ւ̃|�C���^�B
                                �X�̃p�����[�^�� NWMWpaParam �\���̂��Q�ƁB

  Returns:      NWMRetCode  -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_SetWPAPSK(NWMCallbackFunc callback, NWMWpaParam *param);

/*---------------------------------------------------------------------------*
  Name:         NWM_SetReceivingFrameBuffer

  Description:  ��M�o�b�t�@�̐ݒ���s���B

  Arguments:    callback     -  �񓯊������������������A����уf�[�^����M��������
                                �Ăяo�����R�[���o�b�N�֐��B
                                ������ NWMReceivingFrameCallback �\���̂̌`�ŕԂ����B
                                �f�[�^��M���́ANWM_RETCODE_RECV_IND �𔺂��B
                                
                recvBuf      -  �f�[�^��M�o�b�t�@�ւ̃|�C���^�B
                                ARM7�����ڃf�[�^�������o���̂ŁA�L���b�V���ɒ��ӂ��Ă��������B
                recvBufSize  -  �f�[�^��M�o�b�t�@�̃T�C�Y�B��M�o�b�t�@���́A���̃T�C�Y����
                                1536(0x600)�o�C�g�������������ɂȂ�܂��B(�����_�ȉ��؎̂�)
                                ��M��肱�ڂ�������邽�߁A3072(0xC00)�o�C�g�ȏ�m�ۂ��Ă��������B
                protocol     -  �ǂ̃v���g�R���ɑ΂��Ă̎�M�o�b�t�@�����w�肵�܂��B
                                NWM_PORT_IPV4_ARP: IPv4��TCP/IP�f�[�^�����ARP�f�[�^
                                                   (�ʏ�͂���������g�����������B)
                                NWM_PORT_EAPOL   : EAPOL�f�[�^(WPS�p)
                                NWM_PORT_OTHERS  : ���̑��̃v���g�R��

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_SetReceivingFrameBuffer(NWMCallbackFunc callback, u8* recvBuf, u16 recvBufSize, u16 protocol);

/*---------------------------------------------------------------------------*
  Name:         NWM_SendFrame

  Description:  �������W���[���ւ̑��M�f�[�^�̃Z�b�g���s���B
                ���M�������������Ƃ�ۏ؂�����̂ł͂Ȃ����Ƃɒ��ӁB

  Arguments:    callback     -  �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                                ������ NWMSendFrameCallback �\���̂̌`�ŕԂ����B
                destAddr     -  ���M�Ώۂ�BSSID�������o�b�t�@�ւ̃|�C���^�B
                sendFrame    -  ���M�f�[�^�ւ̃|�C���^�B
                                ���M�f�[�^�̎��̂͋����I�ɃL���b�V���X�g�A
                                �����_�ɒ��ӁB
                sendFrameSize-  ���M�f�[�^�̃T�C�Y�B

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_SendFrame(NWMCallbackFunc callback, const u8 *destAddr,
                         const u16 *sendFrame, u16 sendFrameSize);

/*---------------------------------------------------------------------------*
  Name:         NWM_UnsetReceivingFrameBuffer

  Description:  ��M�o�b�t�@�̐ݒ����������B

  Arguments:    callback     -  �񓯊������������������A����уf�[�^����M��������
                                �Ăяo�����R�[���o�b�N�֐��B
                                
                protocol     -  �ǂ̃v���g�R���ɑ΂��Ă̎�M�o�b�t�@�����w�肵�܂��B
                                NWM_PORT_IPV4_ARP: IPv4��TCP/IP�f�[�^�����ARP�f�[�^
                                                   (�ʏ�͂���������g�����������B)
                                NWM_PORT_EAPOL   : EAPOL�f�[�^(WPS�p)
                                NWM_PORT_OTHERS  : ���̑��̃v���g�R��

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                NWM_RETCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_UnsetReceivingFrameBuffer(NWMCallbackFunc callback, u16 protocol);

/*---------------------------------------------------------------------------*
  Name:         NWM_SetPowerSaveMode

  Description:  PowerSaveMode��ύX����

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                powerSave   -   �ȓd�̓��[�h���w�肷��B
                                NWM_POWERMODE_ACTIVE  : �ȓd�̓��[�hOFF
                                NWM_POWERMODE_STANDARD: 802.11�W���̏ȓd�̓��[�h
                                NWM_POWERMODE_UAPSD   : 802.11e�Œ�߂���g���ȓd�̓��[�h
                                                        (Unscheduled Automatic Power Save Delivery)

  Returns:      NWMRetCode  -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_SetPowerSaveMode(NWMCallbackFunc callback, NWMPowerMode powerSave);

/*---------------------------------------------------------------------------*
  Name:         NWM_GetMacAddress

  Description:  TWL�������W���[������擾�������@��MAC�A�h���X���擾����B
                �����֐��B

  Arguments:    macAddr      -   ���@��MAC�A�h���X���i�[����o�b�t�@�ւ̃|�C���^�B

  Returns:      NWMRetCode   -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_GetMacAddress(u8* macAddr);

/*---------------------------------------------------------------------------*
  Name:         NWM_GetBssDesc

  Description:  NWM_StartScan �ɂ��X�L�������ʂ̃o�b�t�@����ABSS�����擾����B
                �����֐��B

  Arguments:    bssbuf       -   �X�L�������ʂ������o�b�t�@�ւ̃|�C���^
                bsssize      -   �X�L�������ʂ������o�b�t�@�̃T�C�Y
                index        -   �擾����BSS���̃C���f�b�N�X

  Returns:      NWMBssDesc   -   index�Ԗڂɓ����Ă���BSS����Ԃ��B
                                 �擾�ł��Ȃ������ꍇ��NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
NWMBssDesc* NWM_GetBssDesc(void* bssbuf, u32 bsssize, int index);

/*---------------------------------------------------------------------------*
  Name:         NWM_GetAllowedChannel

  Description:  �ʐM���Ɏg�p�������ꂽ�`�����l�����擾����B�����֐��B
                NWM_STATE_DISCONNECTED �ȍ~(NWM_Open ������)�̏�ԂŌĂяo���Ă��������B

  Arguments:    None.

  Returns:      u16 -   �g�p���`�����l���̃r�b�g�t�B�[���h��Ԃ��B�ŉ��ʃr�b�g��
                        1�`�����l���A�ŏ�ʃr�b�g��16�`�����l���������B�r�b�g��1��
                        �`�����l�����g�p���A�r�b�g��0�̃`�����l���͎g�p�֎~�B
                        �ʏ�� 1�`13 �`�����l���̓��������̃r�b�g��1�ɂȂ����l��
                        �Ԃ����B0x0000���Ԃ��ꂽ�ꍇ�͎g�p�������ꂽ�`�����l����
                        ���݂��Ȃ����߁A�����@�\���̂��̂��g�p�֎~�ł���B
                        �܂��A�����������ȂǊ֐��Ɏ��s�����ꍇ��0x8000���Ԃ����B
 *---------------------------------------------------------------------------*/
u16 NWM_GetAllowedChannel(void);

/*---------------------------------------------------------------------------*
  Name:         NWM_CalcLinkLevel

  Description:  nwm_common_private.h �ɂĒ�`����Ă���臒l���烊���N���x�����Z�o

  Arguments:    s16          -   Atheros �h���C�o����ʒm�����RSSI�l

  Returns:      u16          -   WM �Ɠ��l�̃����N���x��
 *---------------------------------------------------------------------------*/
u16 NWM_CalcLinkLevel(s16 rssi);

/*---------------------------------------------------------------------------*
  Name:         NWM_GetDispersionScanPeriod

  Description:  STA �Ƃ��� AP �������� DS �e�@��T������ۂɐݒ肷�ׂ��T�����x���Ԃ��擾����B

  Arguments:    u16 scanType - �X�L�����^�C�v�ANWM_SCANTYPE_PASSIVE or NWM_SCANTYPE_ACTIVE
  
  Returns:      u16 -   �ݒ肷�ׂ��T�����x����(ms)�B
 *---------------------------------------------------------------------------*/
u16 NWM_GetDispersionScanPeriod(u16 scanType);

/*---------------------------------------------------------------------------*
  Name:         NWM_GetState

  Description:  NWM �̏�Ԃ��擾����B

  Arguments:    None.
  
  Returns:      u16 -   NWM �̏�ԁBNWMState �񋓌^�ŕ\�����B
 *---------------------------------------------------------------------------*/
u16 NWM_GetState(void);

/*---------------------------------------------------------------------------*
  Name:         NWM_GetInfoElements

  Description:  BSS��񂩂�A�w�肵�����v�f(IE)���擾����B

  Arguments:    bssDesc           -   �擾�ΏۂƂȂ�BSS���B
                elementID         -   802.11�Œ�߂�����v�f��ElementID�B

  Returns:      NWMInfoElements   -   �w�肵�����v�f�����݂���΁A
                                      �Y��������v�f�ւ̃|�C���^��Ԃ��B
                                      �擾�ł��Ȃ������ꍇ��NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
NWMInfoElements* NWM_GetInfoElements(NWMBssDesc *bssDesc, u8 elementID);

/*---------------------------------------------------------------------------*
  Name:         NWM_GetVenderInfoElements

  Description:  BSS��񂩂�A�w�肵���x���_���v�f(IE)���擾����B
                WPA���̏��v�f�擾�ɗp����B

  Arguments:    bssDesc           -   �擾�ΏۂƂȂ�BSS���B
                elementID         -   802.11�Œ�߂�����v�f��ElementID�B
                                      �K��NWM_ELEMENTID_VENDOR �Ƃ��Ďw�肷��B
                ouiType           -   OUI(3�o�C�g)��Type(1�o�C�g)�̔z��B
                                      WPA�̏ꍇ�A0x00 0x50 0xf2 0x01

  Returns:      NWMInfoElements   -   �w�肵���x���_���v�f�����݂���΁A
                                      �Y������x���_���v�f�ւ̃|�C���^��Ԃ��B
                                      �擾�ł��Ȃ������ꍇ��NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
NWMInfoElements* NWM_GetVenderInfoElements(NWMBssDesc *bssDesc, u8 elementID, const u8 ouiType[4]);


#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* LIBRARIES_NWM_ARM9_NWM_H__ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
