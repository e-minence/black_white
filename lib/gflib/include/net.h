#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
/**
 * @file	net.h
 * @brief	�ʐM���C�u�����̊O�����J�֐�
 * @author	k.ohno
 * @date    2006.11.4
 */
//=============================================================================
#pragma once

#include "heapsys.h"


#define GFL_NET_WIFI    (1)   ///< WIFI���Q�[���Ŏg�p����ꍇ ON
#define GFL_NET_IRC     (1)   ///< IRC���Q�[���Ŏg�p����ꍇ ON

#if GFL_NET_WIFI
//#include <dwc.h>
#endif  //GFL_NET_WIFI



// �f�o�b�O�p���܂蕶��----------------------

#ifdef PM_DEBUG

extern BOOL GFL_NET_DebugGetPrintOn(void);
#define GFL_NET_DEBUG   (1)   ///< ���[�U�[�C���^�[�t�F�C�X�f�o�b�O�p 0:���� 1:�L��
#define GFL_LOBBY_DEBUG	(1)		///<Wi-Fi�L��f�o�b�O�p 0:���� 1:�L��

#else //PM_DEBUG

#define GFL_NET_DEBUG   (0)   ///< ���[�U�[�C���^�[�t�F�C�X�f�o�b�O�p 0:���� 1:�L��
#define GFL_LOBBY_DEBUG	(0)		///<Wi-Fi�L��f�o�b�O�p 0:���� 1:�L��

#endif  //PM_DEBUG



#ifndef NET_PRINT
#if GFL_NET_DEBUG

#define NET_PRINT(...) \
  if(GFL_NET_DebugGetPrintOn())  (void) ((OS_TPrintf(__VA_ARGS__)))

#else   //GFL_NET_DEBUG

#define NET_PRINT(...)           ((void) 0)

#endif  // GFL_NET_DEBUG
#endif  //GFL_NET_PRINT
// �f�o�b�O�p���܂蕶��----------------------
// �f�[�^�_���v
#ifdef GFL_NET_DEBUG
extern void GFL_NET_SystemDump_Debug(u8* adr, int length, char* pInfoStr);
#define DEBUG_DUMP(a,l,s)  if(GFL_NET_DebugGetPrintOn()) GFL_NET_SystemDump_Debug(a,l,s)
#else
#define DEBUG_DUMP(a,l,s)       ((void) 0)
#endif

// �f�o�b�O�p���܂蕶��----------------------
#define GFL_IRC_DEBUG   (0)   ///< ���[�U�[�C���^�[�t�F�C�X�f�o�b�O�p 0:���� 1:�L��


/// @brief �l�b�g���[�N�P�̂̃n���h��
typedef struct _GFL_NETHANDLE GFL_NETHANDLE;

///< @brief �ʐM�Ǘ��\����
typedef struct _GFL_NETSYS GFL_NETSYS;

// define
#define GFL_NET_NETID_SERVER (0xff)    ///< NetID:�T�[�o�[�̏ꍇ���� ���0����ClientID
#define GFL_NET_NETID_SERVERBIT (0x80)    ///< �T�[�o�[BIT
#define GFL_NET_SENDID_ALLUSER (0xff)  ///< NetID:�S���֑��M����ꍇ
#define GFL_NET_NETID_INVALID  (0xfe)  ///< ���肦�Ȃ�ID
#define GFL_NET_NO_PARENTMACHINE (0)  ///< �e�@�̔ԍ�


#define GFL_NET_CHILD_MAX  ( 7 )    ///<  �q�@�ő吔 15->7��
#define GFL_NET_MACHINE_MAX  (GFL_NET_CHILD_MAX+1)  ///< �@�ő吔
#define GFL_NET_HANDLE_MAX  (GFL_NET_MACHINE_MAX+1) ///<   �ʐM�n���h���ő吔  �q�@�S���{�e�@ ��



#define GFL_NET_TOOL_INVALID_LIST_NO  (-1) ///<�����ȑI��ID

#define GFL_WICON_POSX (240)   ///< ���C�����X�A�C�R���̃f�t�H���g�w�x�ʒu
#define GFL_WICON_POSY (0)

// �����Ŏg�p����DMA�ԍ�   dma.h�Ɉړ����܂��� �ʐM��2�ɂȂ�܂�
///#define _NETWORK_DMA_NO                 (1)


//WM�̃p���[���[�h �K��1���w�肷��
#define _NETWORK_POWERMODE       (1)
//SSL�����̃X���b�h�D�揇��
#define _NETWORK_SSL_PRIORITY     (20)



// �e�@��I���ł��鐔�B
#define  SCAN_PARENT_COUNT_MAX ( 16 )


///�ԊO���ʐM�ł̈�x�ɒx���ő�o�C�g��
#define GFL_NET_IRC_SEND_MAX		(128)	//132byte���̎c��4�o�C�g��IRC���C�u�����̃w�b�_���g�p


/// @brief  �R�}���h�֘A�̒�`

/// �R�[���o�b�N�֐��̏���
typedef void (*GFL_NET_PTRCommRecvFunc)(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
/// �T�C�Y���Œ�̏ꍇ�T�C�Y���֐��ŕԂ�
//typedef int (*PTRCommRecvSizeFunc)(void);
/// ��M�o�b�t�@���O���Ŏ����Ă���ꍇ���̃|�C���^
typedef u8* (*GFL_NET_PTRCommRecvBuffAddr)(int netID, void* pWork, int size);
/// ���M�����ꍇ�ɌĂ΂��R�[���o�b�N
typedef u8* (*GFL_NET_SendCallbackType)(int netID, int command, void* pWork);




/// �R�}���h�p�P�b�g�e�[�u����`
typedef struct {
  GFL_NET_PTRCommRecvFunc callbackFunc;    ///< �R�}���h���������ɌĂ΂��R�[���o�b�N�֐�
  //    PTRCommRecvSizeFunc getSizeFunc; ///< �R�}���h�̑��M�f�[�^�T�C�Y���Œ�Ȃ珑���Ă�������
  GFL_NET_PTRCommRecvBuffAddr getAddrFunc;
} NetRecvFuncTable;

/// �σf�[�^���M�ł��邱�Ƃ������Ă���
//#define   GFL_NET_COMMAND_SIZE_VARIABLE   (0x00ffffff)
/// �R�}���h�̃T�C�Y���֐������������f����ׂ̒�` (�g�p���Ȃ��ł�������)
//#define   _GFL_NET_COMMAND_SIZE_H         (0xff000000)
/// �R�}���h�̃T�C�Y���`����ׂ̃}�N��
//#define   GFL_NET_COMMAND_SIZE( num )     (PTRCommRecvSizeFunc)(_GFL_NET_COMMAND_SIZE_H + num)
/// �R�}���h�̃T�C�Y���`����ׂ̃}�N��
//#define   GFL_NET_COMMAND_VARIABLE     (PTRCommRecvSizeFunc)(_GFL_NET_COMMAND_SIZE_H + GFL_NET_COMMAND_SIZE_VARIABLE)


typedef enum {
  GFL_NET_ERROR_RESET_SAVEPOINT = 1,  ///< ���O�̃Z�[�u�ɖ߂��G���[
  GFL_NET_ERROR_RESET_TITLE,          ///< �^�C�g���ɖ߂��G���[
  GFL_NET_ERROR_RESET_OTHER,          ///< ���̑��̃G���[
} GFL_NET_ERROR_ENUM;


typedef enum {
  GFL_NET_ICON_WIFINCGR,   ///< WIFI�̊G�̃f�[�^
  GFL_NET_ICON_WMNCGR,     ///< Wireless�̊G�̃f�[�^
  GFL_NET_ICON_WMNCLR,     ///< �p���b�g�̃f�[�^
} GFL_NET_ICON_ENUM;       /// �ʐM�A�C�R���t�@�C�����O��������炤�Ƃ��̎��ʔԍ�

enum {
  GFL_NET_TYPE_WIRELESS,		///<���C�����X�ʐM
  GFL_NET_TYPE_WIFI,			///<WIFI�ʐM
  GFL_NET_TYPE_WIFI_LOBBY,	///<WIFI�L��ʐM
  GFL_NET_TYPE_IRC,			///<�ԊO���ʐM
  GFL_NET_TYPE_IRC_WIRELESS,	///<�ԊO���ʐM�Ń}�b�`���O��A���C�����X�ʐM�ֈڍs
  GFL_NET_TYPE_WIRELESS_SCANONLY,	///<���C�����X�ʐM(�X�L������p�E�d�������v��_��)
  GFL_NET_TYPE_WIFI_GTS,		///<GTS�ʐM�iWIFI�f�o�C�X�̃��[�h�̂ݍs���j
};

enum{
  GFL_NET_CHANGEOVER_MODE_NORMAL,       ///<�ʏ���(�e�q�����Ԋu�Ő؂�ւ�)
  GFL_NET_CHANGEOVER_MODE_FIX_PARENT,   ///<�e�Œ�
  GFL_NET_CHANGEOVER_MODE_FIX_CHILD,    ///<�q�Œ�
};



//�r�[�R���X�L�������x�ύX
enum{
  GFL_NET_CROSS_SPEED_FAST,   //����
  GFL_NET_CROSS_SPEED_SLOW,   //�x��
  GFL_NET_CROSS_SPEED_PAUSE,  //�x�~
};





typedef u8 GameServiceID;  ///< �Q�[���T�[�r�XID  �ʐM�̎��
typedef u8 ConnectID;      ///< �ڑ����邽�߂�ID  0-16 �܂�
typedef u8 NetID;          ///< �ʐMID  0-16 �܂�

/// @brief ���M�������󂯎��R�[���o�b�N�^
typedef void (*CBSendEndFunc)(u16 command);

/// �C�N�j���[�����̏I����҂�
typedef void (*PTRIchneumonCallback)(void* pWork, BOOL bSuccess);
/// @brief ��ɑ��鑗�M�f�[�^�𓾂�
typedef u8* (*CBGetEveryTimeData)(void);
/// @brief ��ɑ������M�f�[�^�𓾂� �����Ă��Ȃ���pWork��NULL������
typedef void (*CBRecvEveryTimeData)(int netID, void* pWork, int size);

// ������͐ڑ�������Ɍ����������f�[�^
typedef void* (*NetInfomationGetFunc)(void* pWork);    ///< ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
typedef int (*NetInfomationGetSizeFunc)(void* pWork);  ///< ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐��^
// ������͐e�@����q�@�Ɉ���I�ɑ�����f�[�^
typedef void* (*NetBeaconGetFunc)(void* pWork);        ///< �r�[�R���f�[�^�擾�֐�        ��Ɠ����ł��܂�Ȃ�
typedef int (*NetBeaconGetSizeFunc)(void* pWork);      ///< �r�[�R���f�[�^�T�C�Y�擾�֐�  ��Ɠ����ł��܂�Ȃ�
typedef BOOL (*NetBeaconCompFunc)(GameServiceID GameServiceID1, GameServiceID GameServiceID2,void* pUserWork);  ///< �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����

typedef void (*NetAutoParentConnect)(void* work);  ///< �����ڑ������Ƃ��ɐe�ɂȂ����}�V���ŌĂяo�����֐�

typedef void (*NetErrorFunc)(GFL_NETHANDLE* pNet,int errNo, void* pWork);    ///< �ʐM�G���[���N�������ꍇ�Ă΂�� �ʐM�ؒf���邵���Ȃ�
typedef void (*NetFatalErrorFunc)(GFL_NETHANDLE* pNet,int errNo, void* pWork);    ///< �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �d���ؒf���邵���Ȃ�
typedef void (*NetConnectEndFunc)(void* pWork);  ///< �ʐM�ؒf���ɌĂ΂��֐�

typedef u8* (*NetGetSSID)(void);  ///< �e�q�ڑ����ɔF�؂���ׂ̃o�C�g�� 24byte
typedef void (*NetWifiSaveUserDataFunc)(void);  ///< WiFi���[�U�[�f�[�^���Z�[�u���鎞�ɌĂԊ֐�
typedef void (*NetWifiMargeFrinedDataFunc)(int baseFrinedNo, int delFriendNo);  ///< WiFi�t�����h�f�[�^���}�[�W����󋵎��ɌĂԊ֐�
typedef const char** (*NetIconGraTableFunc)(void);     ///< �ʐM�A�C�R���̃t�@�C��ARC�e�[�u����Ԃ��֐�
typedef void (*NetIconGraNoBuffFunc)(int* pNoBuff);  ///< �ʐM�A�C�R���̃t�@�C��ARC�̔ԍ���Ԃ��֐�
//typedef void (*NetKeyMainFunc)(UI_KEYSYS* pKey, u16 keyData);  ///< �L�[��������

typedef void (*NetModeChangeFunc)(void* pWork, BOOL bResult);   ///< �ʐM�`�ԕύX���ɌĂ΂��R�[���o�b�N
typedef void (*NetEndCallback)(void* pWork);   ///< �ʐM�I�����ɌĂ΂��R�[���o�b�N

typedef void (*NetStepEndCallback)(void* pWork);   ///< �ʐM�̋�؂�ɌĂ΂��ėp�R�[���o�b�N�^
typedef void (*NetPreStepEndCallback)(void* pWork,BOOL bParent);   ///< �ʐM�̋�؂�ɌĂ΂��ėp�R�[���o�b�N�^

typedef void (*NetConnectHardware)(void* pWork,int hardID);  ///< �@�B�I�ɐڑ��������ɌĂ΂��
typedef void (*NetConnectNegotiation)(void* pWork,int netID); ///< �l�S�V�G�[�V���������񂾂�Ă΂��

typedef void (*NetDeleteFriendListCallback)(int deletedIndex, int srcIndex, void* pWork); ///< �t�����h���X�g�폜�R�[���o�b�N

typedef void* (*NetGetDWCUserDataCallback)(void* pWork); ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j�𓾂�DWCUserData
typedef void* (*NetGetDWCFriendDataCallback)(void* pWork); ///< DWC�`���̗F�B���X�gDWCFriendData

/// ��r�R�[���o�b�N
typedef BOOL (IrcGSIDCallback)(u8 mygsid,u8 friendgsid);


/// @brief �ʐM�̏������p�\����
typedef struct{
  const NetRecvFuncTable* recvFuncTable;  ///< ��M�֐��e�[�u���̃|�C���^
  int recvFuncTableNum;              ///< ��M�֐��e�[�u�����ڐ� NELEMS
  NetConnectHardware connectHardFunc;    ///< �n�[�h�Őڑ��������ɌĂ΂��
  NetConnectNegotiation  connectNegotiationFunc;  ///<  �l�S�V�G�[�V�����������ɃR�[��
  NetInfomationGetFunc infoFunc;             ///<���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NetInfomationGetSizeFunc infoSizeFunc;         ///< ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  NetBeaconGetFunc beaconGetFunc;    ///< �r�[�R���f�[�^�擾�֐�
  NetBeaconGetSizeFunc beaconGetSizeFunc;   ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
  NetBeaconCompFunc beaconCompFunc;  ///< �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NetErrorFunc errorFunc;            ///< �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ʐM���I��������K�v������
  NetFatalErrorFunc fatalErrorFunc;      ///< �ʐM�s�\�G���[�������B �d���ؒf����K�v������
  NetConnectEndFunc connectEndFunc;  ///< �ʐM�ؒf���ɌĂ΂��֐�
  NetAutoParentConnect autoParentConnect; ///< �����ڑ������Ƃ��ɐe�ɂȂ����}�V���ŌĂяo�����
#if GFL_NET_WIFI
  NetWifiSaveUserDataFunc wifiSaveFunc;     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NetWifiMargeFrinedDataFunc wifiMargeFunc; ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NetDeleteFriendListCallback friendDeleteFunc;  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  NetGetDWCFriendDataCallback keyList;   ///< DWC�`���̗F�B���X�g
  NetGetDWCUserDataCallback myUserData;  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  u32 heapSize;           ///< DWC�ւ̃q�[�v�T�C�Y
  u32 bDebugServer;        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
  u32 ggid;                 ///< �c�r�ŃQ�[���\�t�g����ʂ���ׂ�ID �C�V��������炤
  HEAPID baseHeapID;        ///< ���ƂȂ�HEAPID
  HEAPID netHeapID;         ///< �ʐM�p��create�����HEAPID
  HEAPID wifiHeapID;        ///< wifi�p��create�����HEAPID
  HEAPID ircHeapID;         ///< IRC�p��create�����HEAPID	��check
  u16 iconX;                ///< �ʐM�A�C�R��X�ʒu
  u16 iconY;                ///< �ʐM�A�C�R��Y�ʒu
  u8 maxConnectNum;         ///< �ő�ڑ��l��
  u8 maxSendSize;           ///< ���M�T�C�Y
  u8 maxBeaconNum;          ///< �ő�r�[�R�����W��  = wifi�t�����h���X�g��
  u8 bCRC;                  ///< CRC�������v�Z���邩�ǂ��� TRUE�̏ꍇ���ׂČv�Z����
  u8 bMPMode;               ///< MP�ʐM���[�h���ǂ���
  u8 bNetType;              ///< �g�p����ʐM���w��(GFL_NET_TYPE_???)	��check
  u8 bTGIDChange;           ///< �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  GameServiceID gsid;                 ///< �Q�[���T�[�r�XID  �ʐM�̎��  �o�g���⃆�j�I���Ƃ��ŕύX����l
#if GFL_NET_IRC
  u32 irc_timeout;			///< �ԊO���Đڑ����̃^�C���A�E�g����
#endif	//GFL_NET_IRC
  u16 maxMPParentSize;    ///< MP�ʐM���ɐe�̑��M�ʂ𑝂₵�����ꍇ�T�C�Y�w�� �������Ȃ��ꍇ�O
  u16 GsidOverwrite;               ///< GSID �����ɐ����������Ă���ꍇ�A gsid���r�Ɏg�킸�� ������Ɣ�r����
} GFLNetInitializeStruct;

//-------------------------------
//  �֐� �`�O�����J�֐��͑S�Ă����ɒ�`
//-------------------------------

//==============================================================================
/**
 * @brief    boot���̒ʐM������
 *           WIFI��IPL�����ݒ肪�N�����ɕK�v�Ȉ�
 * @param    heapID    �g�p����temp������ID
 * @param    errorFunc �G���[���ɌĂяo���֐�
 * @param    fixHeapID �ŏ�����ʐM�̈���Ƃ�ꍇ   �eID
 * @param    childHeapID �ŏ�����ʐM�̈���Ƃ�ꍇ �q��ID  ���̓���ꏏ�̏ꍇ�A�����Ŋm�ۂ����Ȃ�
 * @return   none
 */
//==============================================================================
extern void GFL_NET_Boot(HEAPID heapID, NetErrorFunc errorFunc, HEAPID fixHeapID, HEAPID childHeapID);
//==============================================================================
/**
 * @brief   �ʐM������
 * @param   pNetInit  �ʐM�������\���̂̃|�C���^
 * @param   NetStepEndCallback  �������������ɌĂ΂��R�[���o�b�N
 * @param   pWork  �g�p���Ă��郆�[�U�[�̃��[�N�|�C���^ �R�[���o�b�N�ňꏏ�ɓn���܂�
 * @return  none
 */
//==============================================================================
extern void GFL_NET_Init(const GFLNetInitializeStruct* pNetInit, NetStepEndCallback callback, void* pWork);

//==============================================================================
/**
 * @brief �ʐM�������������������ǂ������m�F����
 * @retval TRUE   �������I��
 * @retval FALSE  ���������܂��I����Ă��Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_IsInit(void);
//==============================================================================
/**
 * @brief  �ʐM���I���������ǂ���
 * @retval TRUE   �I��
 * @retval FALSE  �܂��I�����Ă��Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_IsExit(void);
//==============================================================================
/**
 * @brief   �ʐM�I��
 * @param   netEndCallback    �ʐM���I�������ۂɌĂ΂��R�[���o�b�N�ł�
 * @retval  TRUE:�I�����N�G�X�g���󂯕t����
 * @retval  FALSE:���ɉ���ς� or �V�X�e�����쐬����Ă��Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_Exit(NetEndCallback netEndCallback);
//==============================================================================
/**
 * @brief   �G���[���A�����̈� �I���������������Z�b�g
 */
//==============================================================================
extern void GFL_NET_ResetDisconnect(void);

//==============================================================================
/**
 * @brief   �ʐM�̃��C�����s�֐�
 * @retval  TRUE  �������s���Ă悢�ꍇ
 * @retval  FALSE ���������Ă��Ȃ��̂ŏ������s��Ȃ��ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_Main(void);

//-----�r�[�R���֘A
//==============================================================================
/**
 * @brief �r�[�R�����󂯎��T�[�r�X��ǉ��w��  �i�������̃T�[�r�X�̃r�[�R�����E���������Ɂj
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   GameServiceID gsid  �Q�[���T�[�r�XID  �ʐM�̎��
 * @retval  none
  ���ݖ�����
 */
//==============================================================================
//extern void GFL_NET_AddBeaconServiceID(GFL_NETHANDLE* pNet, GameServiceID gsid);
//==============================================================================
/**
 * @brief   ��M�r�[�R���f�[�^�𓾂�
 * @param   index  �r�[�R���o�b�t�@index
 * @return  �r�[�R���f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
extern void* GFL_NET_GetBeaconData(int index);
//==============================================================================
/**
 * @brief   ��M�r�[�R���ɑΉ�����MAC�A�h���X�𓾂�
 * @param   index  �r�[�R���o�b�t�@index
 * @return  �r�[�R���f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
extern u8* GFL_NET_GetBeaconMacAddress(int index);

//-----�ʐM�n���h���Ǘ�
//==============================================================================
/**
 * @brief   ���̃n���h���̒ʐM�ԍ��𓾂�
 * @param   pNetHandle    �ʐM�n���h���̃|�C���^
 * @retval  NetID         �ʐMID
 */
//==============================================================================
extern NetID GFL_NET_GetNetID(GFL_NETHANDLE* pNetHandle);


//-----�ʐM�ڑ��Ǘ�
//==============================================================================
/**
 * @brief   �q�@���������s���w�肵���e�@�ɐڑ�����
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @return  none
 */
//==============================================================================
extern void GFL_NET_InitClientAndConnectToParent(u8* macAddress);
//==============================================================================
/**
 * @brief   �w�肵���e�@�ɐڑ�����i�q�@�������ς݁j
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @return  none
 */
//==============================================================================
extern void GFL_NET_ConnectToParent(u8* macAddress);
//==============================================================================
/**
 * @brief   �q�@�ɂȂ�r�[�R�����W�߂�
 * @param   none
 * @return  none
 */
//==============================================================================
extern void GFL_NET_StartBeaconScan(void);
//==============================================================================
/**
 * @brief    �e�@�ɂȂ�҂��󂯂�
 * @param    none
 * @return   none
 */
//==============================================================================
extern void GFL_NET_InitServer(void);
//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ��A�N�ł������̂Őڑ�����
 * @param    �ڑ��R�[���o�b�N
 * @return   none
 */
//==============================================================================
extern void GFL_NET_ChangeoverConnect(NetStepEndCallback callback);
//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ��A�K���e�ɂȂ��Đڑ�
 * @param    none
 * @return   none
 */
//==============================================================================
extern void GFL_NET_ChangeoverParent(NetStepEndCallback callback);


//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ�����̂𑬓x�ύX
 * @param    num GFL_NET_CROSS_SPEED_XXX
 * @return   none
 */
//==============================================================================
extern void GFL_NET_ChangeoverChangeSpeed(int num);

//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ��A�N�ł������̂Őڑ����� �ԊO��������p
 * @param    callback     �ڑ��R�[���o�b�N
 * @param    precallback  ���ڂ̐ԊO�����I���������̃R�[���o�b�N
 * @return   none
 */
//==============================================================================
extern void GFL_NET_ChangeoverConnect_IRCWIRELESS(NetStepEndCallback callback,NetPreStepEndCallback precallback,NetStepEndCallback ircendcallback);
//==============================================================================
/**
 * @brief    �ԊO���ڑ���񑩂����l��
 * @return   �ԊO���ڑ���񑩂����l��
 */
//==============================================================================
extern int GFL_NET_GetNowConnectNum_IRCWIRELESS(void);

//==============================================================================
/**
 * @brief    ���[�U�[���[�N�̕ύX
 * @return   none
 */
//==============================================================================
extern void GFL_NET_WorkChange_IRCWIRELESS(void* pWork);

//==============================================================================
/**
 * @brief    �ԊO��+���C�����X�G���[���ǂ���
 * @return   none
 */
//==============================================================================
extern BOOL GFL_NET_GetTypeErrorIRCWIRELESS(void);

//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ�
 * @param    pHandle  �ʐM�n���h���̃|�C���^
 * @return   none
 */
//==============================================================================
extern void GFL_NET_Changeover(NetStepEndCallback callback);
//==================================================================
/**
 * GFL_NET_Changeover or GFL_NET_ChangeoverConnect �̓������[�h���ォ��ύX����
 *
 * @param   mode		      GFL_NET_CHANGEOVER_MODE_???
 * @param   bAuto		      TRUE:�����ڑ�
 * @param   macAddress		�ڑ���(�e)�ւ�MacAddress
 */
//==================================================================
extern void GFL_NET_ChangeoverModeSet(int mode, BOOL bAuto, const u8 *macAddress);
//==============================================================================
/**
 * @brief   WIFI�����_���}�b�`���J�n����
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
extern int GFL_NET_StateStartWifiRandomMatch_Filter(void );

//==============================================================================
/**
 * @brief    �^�C���A�E�g���ԃZ�b�g
 * @return   none
 */
//==============================================================================
extern void GFL_NET_SetTimeOut(int timer);


//==============================================================================
/**
 * @brief    �e�@�𖳐��X�^�[�g��Ԃɂ���
 */
//==============================================================================

extern BOOL GFL_NET_ForceParentStart_IRCWIRELESS(void);

//==============================================================================
/**
 * @brief    �ԊO���ڑ��㉽��ID�ɂȂ邩�\��
 * @param    netID  ����netid   �e�ɂȂ�Ȃ�q�ɂȂ邩�����Ȃ�
 * @return   none
 */
//==============================================================================
extern void GFL_NET_ReserveNetID_IRCWIRELESS(int netID);

//==============================================================================
/**
 * @brief    �ԊO���ڑ��㉽��GSID�ɂȂ邩�\��
 * @param    netID  ����netid   �e�ɂȂ�Ȃ�q�ɂȂ邩�����Ȃ�
 * @return   none
 */
//==============================================================================
extern void GFL_NET_IRCWIRELESS_SetChangeGSID(u8 gsid);

//==============================================================================
/**
 * @brief   �}�b�N�A�h���X�𓾂�
 * @param   mac_address		���ׂ�MAC�A�h���X
 * @param   no		���ׂ�MAC�A�h���XINDEX
 */
//==============================================================================
extern void GFL_NET_IRCWIRELESS_GetMacAddress(u8 *mac_address, int no);

//==============================================================================
/**
 * @brief   GSID�ݒ�R�[���o�b�N
 * @param   callback
 */
//==============================================================================
extern void GFL_NET_IRCWIRELESS_SetGSIDCallback(IrcGSIDCallback* callback);
//==============================================================================
/**
 * @brief   �G���[�̎���IRCWIRLESS�����Z�b�g����֐�
 * @param   void
 */
//==============================================================================
extern void GFL_NET_IRCWIRELESS_ResetSystemError(void);

//==============================================================================
/**
 * @brief    Wi-Fi���r�[�֐ڑ�����
 * @param    GFL_NETHANDLE        �ʐM�n���h���̃|�C���^
 * @param    GFL_WIFI_FRIENDLIST  �t�����h���X�g�\���̂̃|�C���^
 * @return   none
 */
//==============================================================================
extern void GFL_NET_WifiLogin(void);
//==============================================================================
/**
 * @brief    WIFI���r�[�֐ڑ����Ă��邩�ǂ���
 * @param    NetHandle* pNet  �ʐM�n���h��
 * @retval   TRUE   ���r�[�ڑ�
 * @retval   FALSE   �܂��ڑ����Ă��Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_IsWifiLobby(void);




// �F�؂Ɋւ���֐�

//==============================================================================
/**
 * @brief   �����̃}�V���̃n���h�����A��
 * @retval  GFL_NETHANDLE
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_HANDLE_GetCurrentHandle(void);

//==============================================================================
/**
 * @brief ���݂̐ڑ��䐔�𓾂�
 * @retval  int  �ڑ���
 */
//==============================================================================
extern int GFL_NET_GetConnectNum(void);

//==============================================================================
/**
 * @brief ID�̒ʐM�Ɛڑ����Ă��邩�ǂ�����Ԃ�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   NetID id            �l�b�gID
 * @retval  BOOL  �ڑ����Ă�����TRUE
 */
//==============================================================================
extern BOOL GFL_NET_IsConnectMember(NetID id);

//==============================================================================
/**
 * @brief   �ʐM�A�C�R���̈ʒu��ύX���܂�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_ChangeIconPosition(int x,int y);

//==============================================================================
/**
 * @brief   �ʐM�A�C�R�����ēx�����[�h���܂�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_ReloadIcon(void);

//==============================================================================
/**
 * @brief ���M�J�n���q�@�ɓ`����i�e�@��p�֐��j
 *        ���̊֐��ȍ~�ݒ肵�����M�f�[�^���q�@�Ŏg�p�����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   NetID id            �q�@ID
 * @return  none
 */
//==============================================================================
extern void GFL_NET_SendEnableCommand(GFL_NETHANDLE* pNet,const NetID id);

//==============================================================================
/**
 * @brief ���M�������肽���ǂ�������������i�q�@��p�֐��j
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @retval  TRUE   ����
 * @retval  FALSE  �֎~
 */
//==============================================================================
extern BOOL GFL_NET_IsSendEnable(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief ���M�J�n
 *        �S���ɖ������ő��M  ���M�T�C�Y�͏��������̃e�[�u����������o��
 *        �f�[�^�͕K���R�s�[����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   u16 sendCommand                ���M����R�}���h
 * @param[in]   u16 size                       ���M����T�C�Y
 * @param[in]   void* data                     ���M�f�[�^�|�C���^
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_SendData(GFL_NETHANDLE* pNet,const u16 sendCommand,const u16 size,const void* data);

//==============================================================================
/**
 * @brief ���M�J�n
 * @param[in,out]  pNet  �ʐM�n���h��
 * @param[in]   sendID                     ���M���� �S���֑��M����ꍇ GFL_NET_SENDID_ALLUSER
 * @param[in]   sendCommand                ���M����R�}���h
 * @param[in]   size                       ���M�f�[�^�T�C�Y
 * @param[in]   data                       ���M�f�[�^�|�C���^
 * @param[in]   bFast                      �D�揇�ʂ��������đ��M����ꍇTRUE
 * @param[in]   bRepeat                    ���̃R�}���h���L���[�ɂȂ��Ƃ��������M
 * @param[in]   bSendBuffLock              ���M�o�b�t�@���Ăԑ����ێ�����ꍇ�i�ʐM���̃�����������Ȃ��̂ő傫���f�[�^�𑗐M�ł��܂��j
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u16 sendCommand, const u32 size,const void* data, const BOOL bFast, const BOOL bRepeat, const BOOL bSendBuffLock);

//==============================================================================
/**
 * @brief ���M�J�n �����BIT�P�ʂŎw��\
 * @param[in,out]  pNet  �ʐM�n���h��
 * @param[in]   sendBit                    BIT�w�� �q�@ 1=0x01 ...  �e�@ = 0x80
 * @param[in]   sendCommand                ���M����R�}���h
 * @param[in]   size                       ���M�f�[�^�T�C�Y
 * @param[in]   data                       ���M�f�[�^�|�C���^
 * @param[in]   bFast                      �D�揇�ʂ��������đ��M����ꍇTRUE
 * @param[in]   bRepeat                    ���̃R�}���h���L���[�ɂȂ��Ƃ��������M
 * @param[in]   bSendBuffLock              ���M�o�b�t�@���Ăԑ����ێ�����ꍇ�i�ʐM���̃�����������Ȃ��̂ő傫���f�[�^�𑗐M�ł��܂��j
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_SendDataExBit(GFL_NETHANDLE* pNet,const u8 sendBit,const u16 sendCommand, const u32 size,const void* data, const BOOL bFast, const BOOL bRepeat, const BOOL bSendBuffLock);


//==============================================================================
/**
 * @brief ���M�f�[�^���������ǂ���
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @retval  TRUE   �����ꍇ
 * @retval  FALSE  �c���Ă���ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_IsEmptySendData(GFL_NETHANDLE* pNet);


//----------------------------------------------------------------
/**
 * @brief   ����̃R�}���h�𑗐M���I�������ǂ����𒲂ׂ�
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param   command ���ׂ�R�}���h
 * @retval  �R�}���h���݂�����TRUE
 */
//----------------------------------------------------------------
extern BOOL GFL_NET_IsInSendCommandQueue(GFL_NETHANDLE* pNet,int command);

//----------------------------------------------------------------
/**
 * @brief   ����̃R�}���h�����ւ���
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param   command ���ׂ�R�}���h
 * @param   changecommand ��������R�}���h
 * @retval  �u��������
 */
//----------------------------------------------------------------
extern int GFL_NET_ChangeSendCommandQueue(GFL_NETHANDLE* pNet,int command, int changecommand);


//--------���̑��A�c�[����
//==============================================================================
/**
 * @brief    �ڑ��\�ȃ}�V���̑䐔�������܂ސ���Ԃ�
 * @return   �ڑ��l��
 */
//==============================================================================
extern int GFL_NET_GetConnectNumMax(void);
//==============================================================================
/**
 * @brief    ���M�ő�T�C�Y�𓾂�
 * @return   ���M�T�C�Y
 */
//==============================================================================
extern int GFL_NET_GetSendSizeMax(void);

//------------------------------------------------------------------------------
/**
 * @brief    MP�ʐM���̐e�@���M�ő�T�C�Y�𓾂�
 * @return   ���M�T�C�Y
 */
//------------------------------------------------------------------------------
extern int GFL_NET_GetMpParentSendSizeMax(void);


//==============================================================================
/**
 * @brief    �������\���̂𓾂�
 * @return   �������\����
 */
//==============================================================================
extern GFLNetInitializeStruct* GFL_NET_GetNETInitStruct(void);
//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h�𔭍s����
 * @param   NetHandle* pNet  �ʐM�n���h��
 * @param   u8 no   �^�C�~���O��肽���ԍ�
 * @return  none
 */
//==============================================================================
extern void GFL_NET_TimingSyncStart(GFL_NETHANDLE* pNet, const u8 no);

//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param   NetHandle* pNet  �ʐM�n���h��
 * @param   no   �͂��ԍ�
 * @retval  TRUE    �^�C�~���O�����v
 * @retval  FALSE   �^�C�~���O��������Ă��Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_IsTimingSync(GFL_NETHANDLE* pNet, const u8 no);

//==============================================================================
/**
 * @brief   DS���[�h�֒ʐM��؂�ւ���
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   NetModeChangeFunc   �ύX�����ۂɌĂ΂��R�[���o�b�N�֐�
 * @retval  �����������ǂ��� TRUE�Ő���
 */
//==============================================================================
extern BOOL GFL_NET_ChangeDsMode(GFL_NETHANDLE* pNet,NetModeChangeFunc func);
//==============================================================================
/**
 * @brief   MP���[�h�֒ʐM��؂�ւ���
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   NetModeChangeFunc   �ύX�����ۂɌĂ΂��R�[���o�b�N�֐�
 * @retval  �����������ǂ��� TRUE�Ő���
 */
//==============================================================================
extern BOOL GFL_NET_ChangeMpMode(GFL_NETHANDLE* pNet,NetModeChangeFunc func);

//==============================================================================
/**
 * @brief   �T�[�r�X���[�h��؂�ւ���
 * @param   gsid         �T�[�r�X���[�h
 * @param   connectnum   ���[�J���ڑ��l�� �� �}�b�N�X�ȉ�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_ChangeGameService(GameServiceID gsid,int connectnum);


//==============================================================================
/**
 * @brief   �T�[�r�X���[�h���擾����
 * @retval  gsid         �T�[�r�X���[�h
 */
//==============================================================================
extern GameServiceID GFL_NET_GetGameServiceID(void);

//==============================================================================
/**
 * @brief �V�K�ڑ��֎~������ݒ�
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   BOOL bEnable     TRUE=�ڑ����� FALSE=�֎~
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SetClientConnect(GFL_NETHANDLE* pNet,BOOL bEnable);

//==============================================================================
/**
 * @brief   ���Z�b�g�ł����Ԃ��ǂ���
 * @retval  TRUE�Ȃ烊�Z�b�g�\
 */
//==============================================================================

extern BOOL GFL_NET_IsResetEnable(void);

//==============================================================================
/**
 * @brief   �e�@�}�V�����ǂ���
 * @param   void
 * @retval  TRUE   �e�@
 * @retval  FALSE  �q�@
 */
//==============================================================================
extern BOOL GFL_NET_IsParentMachine(void);

//==============================================================================
/**
 * @brief   �e�@�̃n���h�����ǂ���
 * @param   void
 * @retval  TRUE   �e�@
 * @retval  FALSE  �q�@
 */
//==============================================================================
extern BOOL GFL_NET_IsParentHandle(GFL_NETHANDLE* pNetHandle);

//==============================================================================
/**
 * @brief    FatalError���s
 * @param    pNetHandle  �l�b�g�n���h��
 * @param    errorNo     �G���[�ԍ�
 * @return   none
 */
//==============================================================================
extern void GFL_NET_FatalErrorFunc(int errorNo);
//==============================================================================
/**
 * @brief    �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ʐM���I��������K�v������
 * @param    pNetHandle  �l�b�g�n���h��
 * @param    errorNo     �G���[�ԍ�
 * @return   none
 */
//==============================================================================
extern void GFL_NET_ErrorFunc(int errorNo);

//==============================================================================
/**
 * @brief   �G���[�ɂ���ꍇ���̊֐����ĂԂƃG���[�ɂȂ�܂�
 * @param   �G���[���
 * @retval  �󂯕t�����ꍇTRUE
 */
//==============================================================================
extern BOOL GFL_NET_StateSetError(int no);

//==============================================================================
/**
 * @brief   �G���[�t���O���������܂��B  �ċN������킯�ł͂���܂���
 */
//==============================================================================
extern void GFL_NET_StateResetError(void);

//==============================================================================
/**
 * @brief   GFL_NET_StateSetError�Őݒ肳�ꂽ�G���[�ԍ������o���܂�
 * @retval  �G���[���
 */
//==============================================================================
extern int GFL_NET_StateGetError(void);

//==============================================================================
/**
 * @brief   WifiError�̏ꍇ���̔ԍ���Ԃ�  �G���[�ɂȂ��Ă邩�ǂ������m�F���Ă�������o������
 * @param   none
 * @retval  Error�ԍ�
 */
//==============================================================================
extern int GFL_NET_StateGetWifiErrorNo(void);

#if GFL_NET_WIFI
//==============================================================================
/**
 * @brief   ���[�U�[�f�[�^������������
 * @param   DWCUserData  ���[�U�[�f�[�^�\����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WIFI_InitUserData(void *pUserdata);
#endif

//==============================================================================
/**
 * @brief   WIFI�ڑ��v�����������ǂ���
 * @retval  TRUE  ����
 * @retval  FALSE ���Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_WIFI_IsNewPlayer(void);
//==============================================================================
/**
 * @brief   �����_���}�b�`�ڑ����J�n����
 * @param   pNetHandle  �l�b�g�n���h��
 * @retval  TRUE  ����
 * @retval  FALSE ���s
 */
//==============================================================================
extern BOOL GFL_NET_StartRandomMatch(void);

//==============================================================================
/**
 * @brief   �����I��WIFI�R�l�N�V�����ԍ��𓾂�
 * @retval  -1    �R�l�N�V�����������ĂȂ�
 * @retval  0�ȏ� �R�l�N�V������ID
 */
//==============================================================================
extern int GFL_NET_WIFI_GetLocalConnectNo(void);
//==============================================================================
/**
 * @brief   �ʐM�𓯊����񓯊����ɂ��肩����
 * @param   TRUE  ����    FALSE �񓯊�
 * @return  �Ȃ�
 */
//==============================================================================
extern void GFL_NET_SetWifiBothNet(BOOL flag);
//==============================================================================
/**
 * @brief   ���[�U�[�̎g�p���[�N���Đݒ肷��
 * @param   ���[�N�̃|�C���^
 * @return  �Ȃ�
 */
//==============================================================================
extern void GFL_NET_SetUserWork(void* pWork);
//==============================================================================
/**
 * @brief   �����I�ɃG���[���o���s�����ǂ���
 *          (TRUE�̏ꍇ�G���[���݂���ƃu���[�X�N���[���{�ċN���ɂȂ�)
 * @param   bAuto  TRUE�Ō����J�n FALSE�ŃG���[�������s��Ȃ�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SetAutoErrorCheck(BOOL bAuto);
//==============================================================================
/**
 * @brief   �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ肷��
 * @param   bFlg    �ؒf=�G���[�ɂ���
 * @param   bAuto  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SetNoChildErrorCheck(BOOL bFlg);
//==============================================================================
/**
 * @brief    �r�[�R�����x�𓾂�
 * @param    index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   u16
 */
//==============================================================================
extern u16 GFL_NET_WL_GetRssi(int index);
//==============================================================================
/**
 * @brief   �f�[�^�V�F�A�����O�������������ǂ���
 * @retval  TRUE  �f�[�^�V�F�A�����O����
 * @retval  FALSE �f�[�^�V�F�A�����O���s
 */
//==============================================================================
extern BOOL GFL_NET_SystemCheckDataSharing(void);

//==============================================================================
/**
 * @brief   ���̊֐��̓��C�u�����O�ɍ쐬����֐�
 *          �r�[�R���̃w�b�_�[�𕶎���Œ�` size�o�C�g���K�v (�U�o�C�g)
 * @param   pHeader  �����������֐�
 * @param   size     �T�C�Y
 */
//==============================================================================
extern void GFLR_NET_GetBeaconHeader(u8* pHeader, int size);

//==============================================================================
/**
 * @brief   ���̊֐��̓��C�u�����O�ɍ쐬����֐�
 *          GGID  �Q�[���̒ʐMID���擾��֐�
 * @retval  GGID
 */
//==============================================================================
extern u32 GFLR_NET_GetGGID(void);


/*-------------------------------------------------------------------------*
 * Name        : GFL_NET_Align32Alloc
 * Description : �������m�ۊ֐�
 * Arguments   : name  - �m�ۂ��閼�O
 *             : size  - �m�ۂ���T�C�Y
 *             : align - �m�ۂ���A���C�����g
 * Returns     : *void - �m�ۂ����|�C���^
 * HEAP��4byte���E�ŕԂ��Ă��邽�߁A����𖳗����32byte���E��
 * �A���C�����g�������B
 * �Ԃ��A�h���X-4��4byte�Ɍ���alloc�����A�h���X��ۑ����Ă����āA
 * free����ۂɂ͂��̒l���Q�Ƃ��ăt���[����悤�ɂ��Ă���
 *-------------------------------------------------------------------------*/


#ifndef HEAPSYS_DEBUG
//���̊֐��𒼐ڌĂяo���̂͋֎~
extern void* GFI_NET_Align32Alloc( HEAPID id, u32 size );

#define GFL_NET_Align32Alloc( heapID, size )  GFI_NET_Align32Alloc( heapID, size )

#else

//���̊֐��𒼐ڌĂяo���̂͋֎~
extern void* GFI_NET_Align32Alloc( HEAPID heapID, u32 size, const char* filename, u16 linenum );

#define GFL_NET_Align32Alloc( heapID, size )  GFI_NET_Align32Alloc( heapID, size, __FILE__, __LINE__)

#endif  // #ifndef HEAPSYS_DEBUG



/*-------------------------------------------------------------------------*
 * Name        : GFL_NET_Align32Free
 * Description : �������J���֐�
 * Arguments   : *ptr - ������郁�����̃|�C���^
 * Returns     : None.
 *-------------------------------------------------------------------------*/
extern void GFL_NET_Align32Free( void* ptr );


extern void GFL_NET_BG1PosSet(int x, int y);
extern void GFL_NET_BG1PosGet(int *x, int *y);


extern void debugcheck(u32* data,int size );

//==============================================================================
/**
 * @brief       GFL_NETSYS�\���̂̂ɂ���pWork�𓾂�
 * @param       none
 * @return      work
 */
//==============================================================================
extern void* GFL_NET_GetWork(void);
//==============================================================================
/**
 * @brief       GFL_NETSYS�\���̂̂ɂ���pWork��ς���
 * @param       work
 * @return      none
 */
//==============================================================================
extern void GFL_NET_ChangeWork(void* pWork);
//==============================================================================
/**
 * �����̋@��ID��Ԃ�
 * @param
 * @retval  �����̋@��ID  �Ȃ����Ă��Ȃ��ꍇGFL_NET_NO_PARENTMACHINE
 */
//==============================================================================
extern u16 GFL_NET_SystemGetCurrentID(void);
//--------------------------------------------------------------
/**
 * @brief  �������\���̂̕ύX
 * @param[in]   pNetInit  �ʐM�������\���̂̃|�C���^
 */
//--------------------------------------------------------------
extern void GFL_NET_ChangeInitStruct(const GFLNetInitializeStruct* pNetInit);
//--------------------------------------------------------------
/**
 * @brief   �G���[�ɂȂ������Ƀ��[�U�[�R�[���o�b�N����������
 */
//--------------------------------------------------------------
extern void GFL_NET_DeleteInitializeCallback(void);
//----------------------------------------------------------------
/**
 * @brief   wifi�ڑ��������ǂ�����ݒ肷��
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//----------------------------------------------------------------
extern BOOL GFL_NET_SystemGetWifiConnect(void);

//==============================================================================
/**
 * @brief   �G���[��Ԃ��ǂ���
 * @param   none
 * @retval  �G���[�Ȃ�TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsError(void);
//----------------------------------------------------------------
/**
 * @brief   �G���[��ԂȂ�΃G���[�ԍ����擾����
 * @param   none
 * @retval  �G���[No(�G���[�łȂ��ꍇ��0)
 */
//----------------------------------------------------------------
extern int GFL_NET_SystemGetErrorCode(void);

//--------------------------------------------------------------
/**
 * @brief   Wi-Fi�L��̓Ǝ��G���[���������Ă��邩
 * @param   none
 * @retval  �G���[No(�G���[�łȂ��ꍇ��0)
 */
//--------------------------------------------------------------
extern int GFL_NET_SystemIsLobbyError(void);

//----------------------------------------------------------------
/**
 * @brief   �ʐM�\��Ԃ̐l����Ԃ�
 * @param   none
 * @retval  �ڑ��l��
 */
//----------------------------------------------------------------
extern int GFL_NET_SystemGetConnectNum(void);
//----------------------------------------------------------------
/**
 * @brief		����̃R�}���h�𑗐M���I�������ǂ����𒲂ׂ� �N���C�A���g��
 * @param   command ���ׂ�R�}���h
 * @retval  �R�}���h���݂�����TRUE
 */
//----------------------------------------------------------------
extern BOOL GFL_NET_SystemIsSendCommand(int command, int myID);
//----------------------------------------------------------------
/**
 * @brief   �G���[��Ԃ��ǂ���
 * @param   none
 * @retval  �G���[No(�G���[�łȂ��ꍇ��0)
 */
//----------------------------------------------------------------
extern BOOL GFL_NET_IsError(void);
//----------------------------------------------------------------
/**
 * @brief   �G���[��ԂȂ�΃G���[�ԍ����擾����
 * @param   none
 * @retval  �G���[No(�G���[�łȂ��ꍇ��0)
 */
//----------------------------------------------------------------
extern int GFL_NET_GetErrorCode(void);

//--------��D��x���M
//�Z�b�g����ƁA���t���[���ʐM�o�b�t�@�̋󂫂����āA���̕��𑗐M���܂��B
//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F�V�X�e���쐬(���M��
 *
 * @param   heapId �V�X�e���쐬�pHEAP
 * @param   isWifi Wifi���[�h(��l�p�E���̃f�[�^���M�ɋ���҂�
 */
//--------------------------------------------------------------
extern void GFL_NET_LDATA_InitSystem( const HEAPID heapId , const BOOL isWifi );
//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F�V�X�e���폜
 */
//--------------------------------------------------------------
extern void GFL_NET_LDATA_ExitSystem();
//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F���M�f�[�^�ݒ�
 *
 * @param   data   ���M�f�[�^
 * @param   size   ���M�T�C�Y
 * @param   sendBit���M�Ώ�bit
 * @param   isMp   MP�ʐM���[�h���H
 */
//--------------------------------------------------------------
extern void GFL_NET_LDATA_SetSendData( void* data , const u32 size , const u8 sendBit , const BOOL isMp );
//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F��M�o�b�t�@�쐬
 *
 * @param   size   ��M�T�C�Y(�ő�l
 * @param   netID  ���M����netID
 * @param   heapId �V�X�e���쐬�pHEAP
 */
//--------------------------------------------------------------
extern void GFL_NET_LDATA_CreatePostBuffer( u32 size , const u8 netId , const HEAPID heapId );
//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F��M�o�b�t�@�쐬(�O����o�b�t�@��n��
 *
 * @param   size   ��M�T�C�Y
 * @param   netID  ���M����netID
 * @param   heapId �V�X�e���쐬�pHEAP
 * @param   bufferAdr �o�b�t�@
 */
//--------------------------------------------------------------
extern void GFL_NET_LDATA_CreatePostBuffer_SetAddres( u32 size , const u8 netId , const HEAPID heapId , void* bufferAdr );
//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F��M�o�b�t�@�폜
 *
 * @param   netID  ���M����netID
 */
//--------------------------------------------------------------
extern void GFL_NET_LDATA_DeletePostBuffer( const u8 netId );
//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F��M�o�b�t�@�擾
 *
 * @return  void* �f�[�^
 */
//--------------------------------------------------------------
extern void* GFL_NET_LDATA_GetPostData( const u8 netId );
//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F��M�T�C�Y�擾
 *
 * @return  u32 �T�C�Y
 */
//--------------------------------------------------------------
extern const u32 GFL_NET_LDATA_GetPostDataSize( const u8 netId );

//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F���M�����`�F�b�N
 *
 * @return  BOOL TRUE:�I��������
 */
//--------------------------------------------------------------
extern const BOOL GFL_NET_LDATA_IsFinishSend();

//--------------------------------------------------------------
/**
 * @brief   ��D��x���M�F��M�����`�F�b�N
 *
 * @return  BOOL TRUE:�I��������
 */
//--------------------------------------------------------------
extern const BOOL GFL_NET_LDATA_IsFinishPost( const u8 netId );

//==============================================================================
/**
 * @brief   WIFI�ڑ����ǂ���
 * @retval  TRUE WIFI�ڑ�
 */
//==============================================================================
extern BOOL GFL_NET_IsWifiConnect(void);

//==============================================================================
/**
 * @brief   IRC�ڑ����ǂ���
 * @retval  TRUE WIFI�ڑ�
 */
//==============================================================================
extern BOOL GFL_NET_IsIrcConnect(void);


//----------------------------------------------------------------
/**
 * @brief		�ʐMONOFF�ɂ������Ȃ��A�C�R���̈ʒu��ێ��ł��܂�
            ���p���Ă͂����܂���
 * @param  x y �ʐM�A�C�R���̉�ʈʒu
 * @retval  none
 */
//----------------------------------------------------------------
extern void GFL_NET_SetIconForcePosition(int x,int y);

//----------------------------------------------------------------
/**
 * @brief		�ʐMONOFF�ɂ������Ȃ��A�C�R���̈ʒu�����Z�b�g���܂�
 * @retval  none
 */
//----------------------------------------------------------------
extern void GFL_NET_ResetIconForcePosition(void);


#ifdef PM_DEBUG
//----------------------------------------------------------------
/**
 * @brief		�f�o�b�O����������ON
 * @retval  none
 */
//----------------------------------------------------------------
extern void GFL_NET_DebugPrintOn(void);
extern void GFL_NET_DebugPrintOff(void);
#endif


#include "net_devicetbl.h"

#if GFL_NET_WIFI
#include "net_wifi.h"
#endif //GFL_NET_WIFI

//#if GFL_NET_IRC
//#include "net_irc.h"
//#endif	//GFL_NET_IRC

#include "net_command.h"
#include "net_handle.h"
#include "net_icondata.h"
#include "net_icon.h"

#ifdef __cplusplus
} /* extern "C" */
#endif
