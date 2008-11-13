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
#ifndef __NET_H__
#define __NET_H__

#include "ui.h"

#if defined(DEBUG_ONLY_FOR_ohno)
#define GFL_NET_WIFI    (0)   ///< WIFI���Q�[���Ŏg�p����ꍇ ON
#define GFL_NET_IRC     (1)   ///< IRC���Q�[���Ŏg�p����ꍇ ON
#elif defined(DEBUG_ONLY_FOR_matsuda)
#define GFL_NET_WIFI    (0)   ///< WIFI���Q�[���Ŏg�p����ꍇ ON
#define GFL_NET_IRC     (1)   ///< IRC���Q�[���Ŏg�p����ꍇ ON
#else
#define GFL_NET_WIFI    (0)   ///< WIFI���Q�[���Ŏg�p����ꍇ ON
#define GFL_NET_IRC     (1)   ///< IRC���Q�[���Ŏg�p����ꍇ ON
#endif

//#if defined(DEBUG_ONLY_FOR_ohno)
//#define BEACON_TEST (0)
//#else
//#define BEACON_TEST    (0) 
//#endif

// �f�o�b�O�p���܂蕶��----------------------
#if defined(DEBUG_ONLY_FOR_ohno)
#define GFL_NET_DEBUG   (1)   ///< ���[�U�[�C���^�[�t�F�C�X�f�o�b�O�p 0:���� 1:�L��
#else
#define GFL_NET_DEBUG   (0)   ///< ���[�U�[�C���^�[�t�F�C�X�f�o�b�O�p 0:���� 1:�L��
#endif

//#if defined(DEBUG_ONLY_FOR_ohno)
//#undef GFL_NET_DEBUG
//#define GFL_NET_DEBUG   (1)
//#endif  //DEBUG_ONLY_FOR_ohno

#ifndef NET_PRINT
#if GFL_NET_DEBUG
#define NET_PRINT(...) \
  (void) ((OS_TPrintf(__VA_ARGS__)))
#else   //GFL_NET_DEBUG
#define NET_PRINT(...)           ((void) 0)
#endif  // GFL_NET_DEBUG
#endif  //GFL_NET_PRINT
// �f�o�b�O�p���܂蕶��----------------------
// �f�[�^�_���v
#ifdef GFL_NET_DEBUG
extern void GFL_NET_SystemDump_Debug(u8* adr, int length, char* pInfoStr);
#define DEBUG_DUMP(a,l,s)  GFL_NET_SystemDump_Debug(a,l,s)
#else
#define DEBUG_DUMP(a,l,s)       ((void) 0)
#endif

// �f�o�b�O�p���܂蕶��----------------------
#define GFL_IRC_DEBUG   (0)   ///< ���[�U�[�C���^�[�t�F�C�X�f�o�b�O�p 0:���� 1:�L��

#ifndef IRC_PRINT
#if GFL_IRC_DEBUG
#define IRC_PRINT(...) \
  (void) ((OS_TPrintf(__VA_ARGS__)))
#else   //GFL_IRC_DEBUG
#define IRC_PRINT(...)           ((void) 0)
#endif  // GFL_IRC_DEBUG
#endif


/// @brief �l�b�g���[�N�P�̂̃n���h��
typedef struct _GFL_NETHANDLE GFL_NETHANDLE;


// define 
#define GFL_NET_NETID_SERVER (0xff)    ///< NetID:�T�[�o�[�̏ꍇ���� ���0����ClientID
#define GFL_NET_SENDID_ALLUSER (0x10)  ///< NetID:�S���֑��M����ꍇ
#define GFL_NET_NETID_INVALID  (0xfe)  ///< ���肦�Ȃ�ID
//#define GFL_NET_PARENT_NETID    (0)    ///< �e��ID
#define GFL_NET_NO_PARENTMACHINE (0)  ///< �e�@�̔ԍ�


#define GFL_NET_CHILD_MAX  ( 15 )    ///<  �q�@�ő吔
#define GFL_NET_MACHINE_MAX  (GFL_NET_CHILD_MAX+1)  ///< �@�ő吔
#define GFL_NET_HANDLE_MAX  (GFL_NET_MACHINE_MAX+1) ///<   �ʐM�n���h���ő吔  �q�@�S���{�e�@ ��



#define GFL_NET_TOOL_INVALID_LIST_NO  (-1) ///<�����ȑI��ID

#define GFL_WICON_POSX (240)   ///< ���C�����X�A�C�R���̃f�t�H���g�w�x�ʒu
#define GFL_WICON_POSY (0)

// �����Ŏg�p����DMA�ԍ�
#define _NETWORK_DMA_NO                 (1)
//WM�̃p���[���[�h
#define _NETWORK_POWERMODE       (1)
//SSL�����̃X���b�h�D�揇��
#define _NETWORK_SSL_PRIORITY     (20)



// �e�@��I���ł��鐔�B
#define  SCAN_PARENT_COUNT_MAX ( 16 )


///�ԊO���ʐM�ł̈�x�ɒx���ő�o�C�g��
#define GFL_NET_IRC_SEND_MAX		(128)	//132byte���̎c��4�o�C�g��IRC���C�u�����̃w�b�_���g�p


/// @brief  �R�}���h�֘A�̒�`

/// �R�[���o�b�N�֐��̏���
typedef void (*PTRCommRecvFunc)(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
/// �T�C�Y���Œ�̏ꍇ�T�C�Y���֐��ŕԂ�
//typedef int (*PTRCommRecvSizeFunc)(void);
/// ��M�o�b�t�@���O���Ŏ����Ă���ꍇ���̃|�C���^
typedef u8* (*PTRCommRecvBuffAddr)(int netID, void* pWork, int size);
/// ���M�����ꍇ�ɌĂ΂��R�[���o�b�N
typedef u8* (*GFL_NET_SendCallbackType)(int netID, int command, void* pWork);




/// �R�}���h�p�P�b�g�e�[�u����`
typedef struct {
    PTRCommRecvFunc callbackFunc;    ///< �R�}���h���������ɌĂ΂��R�[���o�b�N�֐�
//    PTRCommRecvSizeFunc getSizeFunc; ///< �R�}���h�̑��M�f�[�^�T�C�Y���Œ�Ȃ珑���Ă�������
    PTRCommRecvBuffAddr getAddrFunc;
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
	GFL_NET_TYPE_IRC,			///<�ԊO���ʐM
	GFL_NET_TYPE_IRC_WIRELESS,	///<�ԊO���ʐM�Ń}�b�`���O��A���C�����X�ʐM�ֈڍs
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
typedef void* (*NetInfomationGetFunc)(void);    ///< ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
typedef int (*NetInfomationGetSizeFunc)(void);  ///< ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐��^
// ������͐e�@����q�@�Ɉ���I�ɑ�����f�[�^
typedef void* (*NetBeaconGetFunc)(void);        ///< �r�[�R���f�[�^�擾�֐�        ��Ɠ����ł��܂�Ȃ�
typedef int (*NetBeaconGetSizeFunc)(void);      ///< �r�[�R���f�[�^�T�C�Y�擾�֐�  ��Ɠ����ł��܂�Ȃ�
typedef BOOL (*NetBeaconCompFunc)(GameServiceID GameServiceID1, GameServiceID GameServiceID2);  ///< �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����

typedef void (*NetAutoParentConnect)(void* work);  ///< �����ڑ������Ƃ��ɐe�ɂȂ����}�V���ŌĂяo�����֐� 

typedef void (*NetErrorFunc)(GFL_NETHANDLE* pNet,int errNo, void* pWork);    ///< �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
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

typedef void (*NetConnectHardware)(void* pWork,int hardID);  ///< �@�B�I�ɐڑ��������ɌĂ΂��
typedef void (*NetConnectNegotiation)(void* pWork,int netID); ///< �l�S�V�G�[�V���������񂾂�Ă΂��




#if GFL_NET_WIFI
#include <dwc.h>
#endif  //GFL_NET_WIFI

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
  NetErrorFunc errorFunc;            ///< �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NetConnectEndFunc connectEndFunc;  ///< �ʐM�ؒf���ɌĂ΂��֐�
  NetAutoParentConnect autoParentConnect; ///< �����ڑ������Ƃ��ɐe�ɂȂ����}�V���ŌĂяo�����
#if GFL_NET_WIFI
  NetWifiSaveUserDataFunc wifiSaveFunc;     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NetWifiMargeFrinedDataFunc wifiMargeFunc; ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  DWCFriendData *keyList;   ///< DWC�`���̗F�B���X�g	
  DWCUserData *myUserData;  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
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
#if 0
  u8 bWiFi;                 ///< Wi-Fi�ʐM�����邩�ǂ���
#else
  u8 bNetType;              ///< �g�p����ʐM���w��(GFL_NET_TYPE_???)	��check
#endif
  u8 bTGIDChange;           ///< �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  GameServiceID gsid;                 ///< �Q�[���T�[�r�XID  �ʐM�̎��  �o�g���⃆�j�I���Ƃ��ŕύX����l
} GFLNetInitializeStruct;

//-------------------------------
//  �֐� �`�O�����J�֐��͑S�Ă����ɒ�`
//-------------------------------

//==============================================================================
/**
 * @brief    �ʐM�̃u�[�g��������
 * @param    heapID  �g�p����temp������ID
 * @param    errorFunc  �G���[���ɌĂяo����ʕ\���֐�
 * @return   none
 */
//==============================================================================
extern void GFL_NET_Boot(HEAPID heapID, NetErrorFunc errorFunc);
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
 * @brief   �ʐM�I��
 * @param   netEndCallback    �ʐM���I�������ۂɌĂ΂��R�[���o�b�N�ł�
 */
//==============================================================================
extern void GFL_NET_Exit(NetEndCallback netEndCallback);
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
 * @param   GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @return  none
 */
//==============================================================================
extern void GFL_NET_ConnectToParent(u8* macAddress);
//==============================================================================
/**
 * @brief   �q�@�ɂȂ�r�[�R�����W�߂�
 * @param   pHandle  �ʐM�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================
extern void GFL_NET_StartBeaconScan(void);
//==============================================================================
/**
 * @brief    �e�@�ɂȂ�҂��󂯂�
 * @param    pHandle  �ʐM�n���h���̃|�C���^
 * @return   none
 */
//==============================================================================
extern void GFL_NET_InitServer(void);
//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ��A�N�ł������̂Őڑ�����
 * @param    pHandle  �ʐM�n���h���̃|�C���^
 * @return   none
 */
//==============================================================================
extern void GFL_NET_ChangeoverConnect(NetStepEndCallback callback);
//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ�
 * @param    pHandle  �ʐM�n���h���̃|�C���^
 * @return   none
 */
//==============================================================================
extern void GFL_NET_Changeover(NetStepEndCallback callback);
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
extern BOOL GFL_NET_IsWifiLobby(GFL_NETHANDLE* pNetHandle);




// �F�؂Ɋւ���֐�
//==============================================================================
/**
 * @brief   �ʐM�n���h���𓾂�
 * @param   netID  �ʐMID
 * @return  GFL_NET_HANDLE  �n���h��
 */
//==============================================================================
extern const GFL_NETHANDLE* GFL_NET_HANDLE_Get(int netID);

//==============================================================================
/**
 * @brief   �����̃}�V���̃n���h�����A��
 * @retval  GFL_NETHANDLE
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_HANDLE_GetCurrentHandle(void);

//==============================================================================
/**
 * @brief   �ʐM�\��Ԃ��ǂ���
 * @param   GFL_NET_HANDLE  �n���h��
 * @return  BOOL �ʐM�\�Ȃ�TRUE
 */
//==============================================================================
extern BOOL GFL_NET_HANDLE_IsConnect(const GFL_NETHANDLE* pHandle);

//==============================================================================
/**
 * @brief       �����̃l�S�V�G�[�V����������ł��邩�ǂ���
 * @param       pHandle   �ʐM�n���h��
 * @return      ����ł���ꍇTRUE   �܂��̏ꍇFALSE
 */
//==============================================================================
extern BOOL GFL_NET_IsNegotiation(GFL_NETHANDLE* pHandle);

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
 * @param[in]   sendID                     ���M���� �S���֑��M����ꍇ NET_SENDID_ALLUSER
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
extern BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u8 sendCommand, const u32 size,const void* data, const BOOL bFast, const BOOL bRepeat, const BOOL bSendBuffLock);


//==============================================================================
/**
 * @brief ���M�f�[�^���������ǂ���
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @retval  TRUE   �����ꍇ
 * @retval  FALSE  �c���Ă���ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_IsEmptySendData(GFL_NETHANDLE* pNet);



//--------���̑��A�c�[����
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


#if GFL_NET_WIFI
//==============================================================================
/**
 * @brief   ���[�U�[�f�[�^������������
 * @param   DWCUserData  ���[�U�[�f�[�^�\����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WIFI_InitUserData(DWCUserData *pUserdata);
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
extern BOOL GFL_NET_StartRandomMatch(GFL_NETHANDLE* pNetHandle);
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
 * @brief    �C�N�j���[�������C�u�����݂̂�����������
 * @return   none
 */
//==============================================================================
extern void GFL_NET_InitIchneumon(PTRIchneumonCallback callback,void* pWork);
//==============================================================================
/**
 * @brief    �C�N�j���[�������C�u�����̏I�������点��
 * @retval   none
 */
//==============================================================================
extern void GFL_NET_ExitIchneumon(PTRIchneumonCallback callback,void* pWork);
//==============================================================================
/**
 * @brief    �C�N�j���[�������C�u�����������Ă��邩�ǂ����𓾂�
 * @retval   TRUE �Ȃ瓮�쒆
 */
//==============================================================================
extern BOOL GFL_NET_IsIchneumon(void);
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
extern void* GFL_NET_Align32Alloc( HEAPID id, u32 size );
/*-------------------------------------------------------------------------*
 * Name        : GFL_NET_Align32Free
 * Description : �������J���֐�
 * Arguments   : *ptr - ������郁�����̃|�C���^
 * Returns     : None.
 *-------------------------------------------------------------------------*/
extern void GFL_NET_Align32Free( void* ptr );



extern void debugcheck(u32* data,int size );


#if GFL_NET_WIFI
#include "net_wifi.h"
#endif //GFL_NET_WIFI

#if GFL_NET_IRC
#include "net_irc.h"
#endif	//GFL_NET_IRC

#include "net_command.h"
#include "net_handle.h"
#include "net_icondata.h"

#endif // __NET_H__

#ifdef __cplusplus
} /* extern "C" */
#endif
