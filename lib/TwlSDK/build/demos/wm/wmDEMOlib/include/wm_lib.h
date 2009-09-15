/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - demos - wmDEMOlib
  File:     wm_lib.h

  Copyright 2006-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef WMDEMOLIB_WM_LIB_H_
#define WMDEMOLIB_WM_LIB_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/* wm_lib.h */
#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#include <nitro/wm/common/wm.h>

#define WM_MODE_MP_PARENT       1
#define WM_MODE_MP_CHILD        2
#define WM_MODE_INFRASTRUCTURE  3

#define WM_DMA_NO 2

#define WM_PORT_KEYSHARING      15

#define WM_TYPE_CHILD_CONNECTED             0
#define WM_TYPE_CHILD_DISCONNECTED          1
#define WM_TYPE_MP_PARENT_SENT              2
#define WM_TYPE_MP_PARENT_RECV              3
#define WM_TYPE_PARENT_FOUND                4
#define WM_TYPE_PARENT_NOT_FOUND            5
#define WM_TYPE_CONNECTED_TO_PARENT         6
#define WM_TYPE_DISCONNECTED                7
#define WM_TYPE_MP_CHILD_SENT               8
#define WM_TYPE_MP_CHILD_RECV               9
#define WM_TYPE_DISCONNECTED_FROM_PARENT    10
#define WM_TYPE_CONNECT_FAILED              11
#define WM_TYPE_DCF_CHILD_SENT              12
#define WM_TYPE_DCF_CHILD_SENT_ERR          13
#define WM_TYPE_DCF_CHILD_RECV              14
#define WM_TYPE_DISCONNECT_COMPLETE         15
#define WM_TYPE_DISCONNECT_FAILED           16
#define WM_TYPE_END_COMPLETE                17
#define WM_TYPE_MP_CHILD_SENT_ERR           18
#define WM_TYPE_MP_PARENT_SENT_ERR          19
#define WM_TYPE_MP_STARTED                  20
#define WM_TYPE_INIT_COMPLETE               21
#define WM_TYPE_END_MP_COMPLETE             22
#define WM_TYPE_SET_GAMEINFO_COMPLETE       23
#define WM_TYPE_SET_GAMEINFO_FAILED         24
#define WM_TYPE_MP_SEND_ENABLE              25
#define WM_TYPE_PARENT_STARTED              26
#define WM_TYPE_BEACON_LOST                 27
#define WM_TYPE_BEACON_SENT                 28
#define WM_TYPE_BEACON_RECV                 29
#define WM_TYPE_MP_SEND_DISABLE             30
#define WM_TYPE_DISASSOCIATE                31
#define WM_TYPE_REASSOCIATE                 32
#define WM_TYPE_AUTHENTICATE                33
#define WM_TYPE_SET_LIFETIME                34
#define WM_TYPE_DCF_STARTED                 35
#define WM_TYPE_DCF_SENT                    36
#define WM_TYPE_DCF_SENT_ERR                37
#define WM_TYPE_DCF_RECV                    38
#define WM_TYPE_DCF_END                     39
#define WM_TYPE_MPACK_IND                   40
#define WM_TYPE_MP_CHILD_SENT_TIMEOUT       41
#define WM_TYPE_SEND_QUEUE_FULL_ERR         42

#define WM_TYPE_API_ERROR                   255 // API�R�[���̖߂�l�ŃG���[
#define WM_TYPE_ERROR                       256 // �R�[���o�b�N�ł̃G���[


#define WL_HEADER_LENGTH_P  48         // �e�@��M����WL�w�b�_��
#define WM_HEADER_LENGTH    (2+4)      // WMHeader��
#define WL_HEADER_LENGTH_T  8          // �S�̂�WL�w�b�_��
#define WL_HEADER_LENGTH_C  8          // ���ꂼ��̎q�@�f�[�^��WL�w�b�_��

// �}�N�� �q�@��M�o�b�t�@�T�C�Y�v�Z
#define WM_CalcChildSendBufSize(_pInfo_)        (u16)(WM_SIZE_MP_CHILD_SEND_BUFFER(((WMgameInfo *)(&(((WMbssDesc*)(_pInfo_))->gameInfo)))->childMaxSize, TRUE))
#define WM_CalcChildRecvBufSize(_pInfo_)        (u16)(WM_SIZE_MP_CHILD_RECEIVE_BUFFER(((WMgameInfo *)(&(((WMbssDesc*)(_pInfo_))->gameInfo)))->parentMaxSize, TRUE))

// �}�N�� �e�@����M�o�b�t�@�T�C�Y�v�Z
#define WM_CalcParentSendBufSize(_libParam_)    (u16)(WM_SIZE_MP_PARENT_SEND_BUFFER(((WM_lib_param*)(&(_libParam_)))->parentParam->parentMaxSize, TRUE))
#define WM_CalcParentRecvBufSize(_libParam_)    (u16)(WM_SIZE_MP_PARENT_RECEIVE_BUFFER(((WM_lib_param*)(&(_libParam_)))->parentParam->childMaxSize, WM_NUM_MAX_CHILD, TRUE))


typedef void (*WMcallbackFunc2) (u16 type, void *arg);  // WM�̃R�[���o�b�N�^(���̂Q)

// wm_lib �p�����[�^�\����
typedef struct
{
    void   *wmBuf;                     // WM�V�X�e���o�b�t�@
    WMpparam *parentParam;             // �e�@���ݒ�(�e�@���g�p)
    WMbssDesc *parentInfoBuf;          // �e�@���擾�o�b�t�@(�q�@���g�p)
    u16    *sendBuf;                   // ���M�o�b�t�@
    u16    *recvBuf;                   // MP��M�o�b�t�@
    WMdcfRecvBuf *dcfBuf;              // DCF��M�o�b�t�@
    WMstatus *statusBuf;               // �X�e�[�^�X�擾�p�o�b�t�@(�ł���΍폜������)
    WMcallbackFunc2 callback;          // WM_lib�p�R�[���o�b�N
    WMbssDesc *pInfo;                  // �ڑ���e�@���|�C���^(�q�@�Ŏg�p)
    u16     sendBufSize;               // ���M�o�b�t�@�̃T�C�Y
    u16     recvBufSize;               // MP��M�o�b�t�@�̃T�C�Y
    u16     dcfBufSize;                // DCF��M�o�b�t�@�̃T�C�Y
    u16     mode;                      // WM_MODE_***
    u16     endReq;
    u16     mpStarted;                 // MP�J�n�ς݃t���O
    u16     mpBusy;                    // MP���M�v�����t���O
    u16     child_bitmap;              // �q�@�ڑ���
    u16     parentInfoBufSize;         // �e�@���擾�o�b�t�@�̃T�C�Y(StartScanEx�Ŏg�p)

    // KeySharing�֘A
    u16     keySharing;                // KeySharing���s�t���O(1:KS�����s, 0:KS�����s���Ȃ�)
    WMkeySetBuf *ksBuf;                // KeySharing�p�o�b�t�@

    // �A�����M�֘A
    u16     contSend;                  // �A�����M���s�t���O(1:�A�����M, 0:1�t���[�����̒ʐM)

    // wep�֘A
    u16     wepMode;                   // �ڑ��Ɏg�p����WEPmode
    u16     wepKeyId;                  // �ڑ��Ɏg�p����WEPKey��ID
    u8      wepKey[80];                // �ڑ��Ɏg�p����WEPkey

    // �ڑ��F�؊֘A
    u16     authMode;                  // WM_AUTHMODE_OPEN_SYSTEM or WM_AUTHMODE_SHARED_KEY

    // gameinfo�֘A
    u8     *uname;                     // UserName
    u8     *gname;                     // GameName
    u16     currentTgid;               // �ڑ���e�@��TGID(BeaconRecv.Ind�Ń`�F�b�N����)

    // user gameinfo�֘A
    u16     userGameInfoLength;        // ���[�U�[�̈�̒���
    u16    *userGameInfo;              // ���[�U�[�̈�o�b�t�@�̃|�C���^

    // MP�đ��֘A
    BOOL    no_retry;                  // �đ��������s�t���O(FALSE:�đ������s, TRUE:�đ�����)

    // ScanEx�֘A
    BOOL    scanExFlag;                // ScanEx���g�p����ꍇ��TRUE�ɂ���

    u32     ggidFilter;                // GGID�t�B���^�����O(0xffffffff�Ȃ�S���ʉ�)

//    u16             rsv1;               // for 4byte padding
}
WM_lib_param;

//-----------------------------------
// WM_lib_Init()
int     WM_lib_Init(WM_lib_param * para);       // WM_lib�̏�����

//-----------------------------------
// WM_lib_SetMPData()
int     WM_lib_SetMPData(u16 *data, u16 size);  // MP�f�[�^�̑��M

//-----------------------------------
// WM_lib_SetMPDataEX()
int     WM_lib_SetMPDataEX(u16 *data, u16 size, u16 pollbitmap);
                                                // MP�f�[�^�̑��M(pollbitmap�Ή��j

//-----------------------------------
// WM_lib_End()
int     WM_lib_End(void);              // �e�@���[�h �I���v��

//-----------------------------------
// WM_lib_ConnectToParent()
int     WM_lib_ConnectToParent(WMbssDesc *pInfo);       // �e�@�ւ̐ڑ��v��

//-----------------------------------
// WM_lib_SetDCFData()
int     WM_lib_SetDCFData(const u8 *destAdr, u16 *data, u16 size);      // DCF�f�[�^�̑��M

//-----------------------------------
// WM_lib_SetGameInfo()
int     WM_lib_SetGameInfo(u16 *userGameInfo, u16 size, u32 ggid, u16 tgid);

//-----------------------------------
// WM_lib_Disconnect()
int     WM_lib_Disconnect(u16 aid);    // �e�@����q�@(aid)��ؒf����


//-----------------------------------
// WM_lib_CheckMPSend()
BOOL    WM_lib_CheckMPSend(void);      // MP���M���`�F�b�N


//-----------------------------------
// WM_lib_GetKeySet()
int     WM_lib_GetKeySet(WMkeySet *keySet);

//-----------------------------------
// WM_lib_set_auto()
void    WM_lib_set_auto(BOOL flag);    // �e�q�@�Ƃ������o�^

//-----------------------------------
// WM_lib_set_bssid()
void    WM_lib_set_bssid(u16 bssid0, u16 bssid1, u16 bssid2);   // BssId�ݒ�

//-----------------------------------
// WM_lib_CurrentScanChannel()

void    WM_lib_set_mp_dcf_parallel(BOOL flag);
void    WM_lib_set_no_retry_flag(BOOL flag);

//-----------------------------------
// Scan�p�p�����[�^�ݒ�E�擾�֐�
void    WM_lib_set_max_scan_time(u16 time);     // Scan�p�����Ԑݒ�
u16     WM_lib_get_max_scan_time(void); // Scan�p�����Ԏ擾
void    WM_lib_set_scan_channel_seek(BOOL flag);        // �`�����l���V�[�N�ݒ�
void    WM_lib_set_channel(u16 channel);        // �`�����l���ݒ�
u16     WM_lib_CurrentScanChannel(void);        // ���݃X�L�������̃`�����l����Ԃ�
void    WM_lib_set_scanBssid(u8 *bssid);        // BSSID�t�B���^�����O

//-----------------------------------
// ScanEx�p�p�����[�^�ݒ�E�擾�֐�
void    WM_lib_set_scanEx_maxChannelTime(u16 time);     // maxChannelTime�ݒ�
u16     WM_lib_get_scanEx_maxChannelTime(void); // maxChannelTime�擾
void    WM_lib_set_scanEx_channelList(u16 channel);     // channelList(bitmap)�ݒ�
u16     WM_lib_get_scanEx_channelList(void);    // channelList(bitmap)�擾
void    WM_lib_set_scanEx_scanType(u16 scanType);       // ScanType�ݒ� WM_SCANTYPE_ACTIVE(0), WM_SCANTYPE_PASSIVE(1)
u16     WM_lib_get_scanEx_scanType(void);       // ScanType�擾 WM_SCANTYPE_ACTIVE(0), WM_SCANTYPE_PASSIVE(1)
void    WM_lib_set_scanEx_ssidLength(u16 ssidLength);   // SSID�t�B���^�����O�pSSID���ݒ�
void    WM_lib_set_scanEx_ssid(u8 *ssid);       // SSID�t�B���^�����O�pSSID�ݒ�
void    WM_lib_set_scanEx_bssid(u8 *bssid);     // BSSID�t�B���^�����O�pBSSID�ݒ�

//-----------------------------------
// WM_lib_CalcRSSI()
u16     WM_lib_CalcRSSI(u16 rate_rssi, u16 aid);        // rate_rssi��RSSI�l�ɕϊ�


/************ wm_tool.h �������Ƃ� �J�n **********************/


#define WM_NUM_PARENT_INFORMATIONS 16
#define WM_NUM_CHILD_INFORMATIONS 15


typedef struct
{
    u16     found_parent_count;        // �����ςݐe�@��
    u16     my_aid;                    // ���g�Ɋ��蓖�Ă�ꂽAID;
    WMmpRecvBuf *recvBuf;              // WM_StartMP()�ň����n���AMP�o�b�t�@
    u16    *sendBuf;                   // WM_SetMPData()�ň����n���AMP���M�f�[�^�o�b�t�@
    u8      MacAdrList[WM_NUM_PARENT_INFORMATIONS][6];  // �e�@MacAddress�ۑ��p(4�䕪)
    WMbssDesc *parentInfo[WM_NUM_PARENT_INFORMATIONS];  // �e�@���i�[�p(4�䕪) �ڑ����Ɏg�p����BssDesc
    u16     channelList[WM_NUM_PARENT_INFORMATIONS];    // �e�@channel�ۑ��p(4�䕪)
    u16     ssidLength[WM_NUM_PARENT_INFORMATIONS];     // �e�@SSID�̒���
    u8      ssidList[WM_NUM_PARENT_INFORMATIONS][32];   // �e�@SSID�ۑ��p(4�䕪)
    u16     gameInfoLength[WM_NUM_PARENT_INFORMATIONS]; // �e�@GameInfo�̒���

    u16     find_counter[WM_NUM_PARENT_INFORMATIONS];   // �e�@������

    u16     rsv[10];
    WMgameInfo gameInfoList[WM_NUM_PARENT_INFORMATIONS] ATTRIBUTE_ALIGN(32);    // �e�@GameInfo�ۑ��p(4�䕪)
}
childInfo;


typedef struct
{
    u8      addr[6];
    u8      reserved[2];               // for 4 byte padding
}
MACADDRESS;

typedef struct
{
    int     valid;
    MACADDRESS macaddr;
}
CHILD_INFO;

typedef void (*WM_LIB_CALLBACK) (u16 type, void *arg);


int     wm_lib_get_last_found_parent_no(void);
void    save_parentInfo(WMstartScanCallback *buf, WM_lib_param * param, childInfo * info);
void    save_parentInfoEx(WMstartScanExCallback *buf, WM_lib_param * param, childInfo * info);


BOOL    wm_lib_get_own_macaddress(MACADDRESS * ma);





/* �������p�֐� */



/* �����\��̊֐� */
void    wm_lib_add_child_list(WMstartParentCallback *arg);
void    wm_lib_delete_child_list(WMstartParentCallback *arg);
void    wm_lib_add_parent_list(WMstartScanCallback *arg);
void    wm_lib_add_parent_listEx(WMStartScanExCallback *arg);
void    wm_lib_delete_parent_list(WMstartScanCallback *arg);
void    wm_lib_parent_found_count_reset(void);
void    wm_lib_set_my_aid(int aid);

/* �e�@�q�@���p�֐� */
void    wm_lib_comm_init(void);
int     wm_lib_start(void);
const char *wm_lib_get_wlib_version(void);
void    wm_lib_set_mode(u16 mode);
void    wm_lib_set_recvbuffer(u8 *buf);
void    wm_lib_set_sendbuffer(u8 *buf);
void    wm_lib_set_callback(WM_LIB_CALLBACK callback);
int     wm_lib_get_recvbuffer_size(void);
int     wm_lib_get_sendbuffer_size(void);
void    wm_lib_set_keySharing_mode(int flag);
void    wm_lib_set_contSend_mode(int flag);
void    wm_lib_set_multiboot_mode(int flag);
void    wm_lib_set_ggid(u32 ggid);
u32     wm_lib_get_ggid(void);
void    wm_lib_set_tgid(u16 tgid);
u16     wm_lib_get_tgid(void);
void    wm_lib_set_scan_bssid(u16 bssid0, u16 bssid1, u16 bssid2);


/* �e�@�p�֐� */
void    wm_lib_set_gameinfo_gamename(char *str);
void    wm_lib_set_gameinfo_username(char *user_name);
u32     wm_lib_get_parent_gameinfo_ggid(int id);
u16     wm_lib_get_parent_gameinfo_tgid(int id);
u16    *wm_lib_get_parent_gameinfo_usergameinfo(int id);
u16     wm_lib_get_parent_gameinfo_usergameinfosize(int id);

void    wm_lib_set_parent_send_size(u16 size);
void    wm_lib_set_parent_channel(u16 ch);
void    wm_lib_set_beacon_period(u16 period_ms);


void    wm_lib_set_parent_recv_size_per_child(u16 size);
BOOL    wm_lib_get_child_macaddress(int aid, MACADDRESS * ma);
BOOL    wm_lib_is_child_valid(int aid);
int     wm_lib_set_gameinfo(void);

/* �q�@�p�֐� */

BOOL    wm_lib_is_parent_gameinfo_valid(int id);
int     wm_lib_get_parent_gameinfo_channel(int id);
int     wm_lib_get_parent_gameinfo_parent_sendmaxsize(int id);
int     wm_lib_get_parent_gameinfo_child_sendbufsize(int id);
int     wm_lib_get_parent_gameinfo_child_recvbufsize(int id);
const char *wm_lib_get_parent_gameinfo_username(int id);
const char *wm_lib_get_parent_gameinfo_gamename(int id);


void   *WM_lib_get_mp_parent_callback_ptr(void);
void   *WM_lib_get_mp_child_callback_ptr(void);


int     wm_lib_get_parent_found_count(void);
int     wm_lib_connect_parent(int parent_no);
BOOL    wm_lib_get_parent_macaddress(int id, MACADDRESS * ma);
int     wm_lib_get_my_aid(void);

/*  get Keysharing Keyset   */
int     wm_lib_get_keyset(WMkeySet *keyset);
int     wm_lib_connect_parent_via_bssdesc(WMbssDesc *bssDesc);

void    wm_lib_set_ggidFilter(u32 ggidFilter);

/*
    get pointer to  WMbssDesc structure
*/
WMbssDesc *wm_get_parent_bssdesc(int id, WMbssDesc *bssDescp);

int     wm_lib_connect_parent_via_bssdesc(WMbssDesc *bssDescp);

/************ wm_tool.h �������Ƃ� �I�� **********************/

/****************************************************/


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* WMDEMOLIB_WM_LIB_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
