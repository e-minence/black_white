#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
/**
 * @file	net_devicetbl.h
 * @brief	�ʐM�f�o�C�X�A�N�Z�X�֐��o�^�e�[�u��
 * @author	k.ohno
 * @date    2008.12.19
 */
//=============================================================================
#ifndef GFL_NET_DEVICETBL_H__
#define GFL_NET_DEVICETBL_H__

/// @brief ���M�����R�[���o�b�N
typedef BOOL (*PTRSendDataCallback)(BOOL result);
/// @brief �R�[���o�b�N�֐��̏���
typedef void (*PTRCommRecvLocalFunc)(u16 aid, u16 *data, u16 size);
/// @brief �R�[���o�b�N�֐��̏���
typedef void (*PTRChildConnectCallbackFunc)(u16 aid);
/// �r�[�R�����E�����Ƃ��ɌĂ΂��R�[���o�b�N�֐�
typedef void (*PTRPARENTFIND_CALLBACK)(void);


/// �����I�����ɌĂ΂��R�[���o�b�N
typedef BOOL (*NetDevEndCallback)(BOOL bResult);
// �d���N������ɌĂ΂��
typedef void (*DevBootFunc)(HEAPID heapID, NetErrorFunc errorFunc);
/// �I�[�o���C�Z�b�g �ڑ��N���X�̏����� �������m�ۊ֐�
typedef BOOL (*DevInitFunc)(HEAPID heapID, GFL_NETSYS* pNet,NetDevEndCallback callback, void* pUserWork);
/// �ڑ�  WIFI�ڑ��܂ł��
typedef BOOL (*DevStartFunc)(NetDevEndCallback callback);

typedef int (*DevMainFunc)(u16 bitmap); ///<  ����
typedef BOOL (*DevEndFunc)(BOOL bForce, NetDevEndCallback callback);  ///<   �ؒf
typedef BOOL (*DevExitFunc)(NetDevEndCallback callback); ///<  �������J���I�[�o�[���C�J��

typedef BOOL (*DevChildWorkInitFunc)(int NetID); ///<     �q�@�G���A�N���A
typedef BOOL (*DevParentWorkInitFunc)(void); ///<DevParentWork              �e�@�G���A�N���A

typedef BOOL (*DevMyBeaconSetFunc)(NetDevEndCallback callback); ///<DevMyBeaconSet             �����r�[�R����ݒ�
typedef BOOL (*DevBeaconSearchFunc)(NetDevEndCallback callback); ///<DevBeaconSearch            �����̃Z�b�g �T�[�`�J�n
typedef void* (*DevBeaconGetFunc)(int index); ///<DevBeaconGet               �r�[�R���𓾂�
typedef u8* (*DevBeaconGetMacFunc)(int index); ///<         �r�[�R����MAC�𓾂�
typedef BOOL (*DevIsBeaconChangeFunc)(NetDevEndCallback callback); ///<DevIsBeaconChange          �r�[�R�����X�V���ꂽ���ǂ����𓾂�
typedef BOOL (*DevResetBeaconChangeFlgFunc)(NetDevEndCallback callback); ///<DevResetBeaconChangeFlg    �t���O�����Z�b�g����
typedef BOOL (*DevIsChangeOverTurnFunc)(NetDevEndCallback callback); ///<DevIsChangeOverTurn        �e�q�؂�ւ����Ă悢���ǂ���(�e�@�����ł��r�[�R���𑗐M���I��������ǂ���)

typedef BOOL (*DevSetChildConnectCallbackFunc)(PTRChildConnectCallbackFunc func);  ///< �q�����Ȃ������̃R�[���o�b�N���w�肷��
typedef BOOL (*DevParentModeConnectFunc)(BOOL bChannelChange,NetDevEndCallback callback); ///<DevParentModeConnect       �e�J�n          
typedef BOOL (*DevChildModeConnectFunc)(BOOL bBconInit,NetDevEndCallback callback); ///<DevChildModeConnect        �q���J�n        �r�[�R��index + macAddress  �Ȃ��Ȃ��ꍇ-1
typedef BOOL (*DevChildModeMacConnectFunc)(BOOL bBconInit,void* pMacAddr,int index,NetDevEndCallback callback,PTRPARENTFIND_CALLBACK pCallback); ///<DevChildModeConnect        �q���J�n        �r�[�R��index + macAddress  �Ȃ��Ȃ��ꍇ-1
typedef BOOL (*DevChangeOverModeConnectFunc)(u8* keyStr,int numEntry, BOOL bParent, u32 timelimit,NetDevEndCallback callback); ///<DevChangeOverModeConnect   �����_���ڑ�     �Ȃ��Ȃ��ꍇ-1
typedef BOOL (*DevWifiConnectFunc)(int index, int maxnum, BOOL bVCT); 
typedef BOOL (*DevModeDisconnectFunc)(BOOL bForce, NetDevEndCallback callback); ///<DevModeDisconnect          �ڑ����I���    
typedef BOOL (*DevIsStepDSFunc)(void);  ///< �f�[�^�V�F�A�����O�������M�o�����Ԃ�
typedef BOOL (*DevStepDSFunc)(void* pSendData);  ///< �f�[�^�V�F�A�����O�������M
typedef void* (*DevGetSharedDataAdrFunc)(int index);  ///<�f�[�^�V�F�A�����O�����f�[�^�𓾂�

typedef BOOL (*DevSendDataFunc)(void* data,int size,int no,NetDevEndCallback callback); ///<DevSendData                ���M�֐�                   
typedef BOOL (*DevRecvCallbackFunc)(PTRCommRecvLocalFunc recvCallback); ///<DevRecvCallback            ��M�R�[���o�b�N

typedef BOOL (*DevIsStartFunc)(void); ///<DevIsStart                 �ʐM�ڑ����Ă邩�ǂ���
typedef BOOL (*DevIsConnectFunc)(void); ///<DevIsConnect               ����Ɛڑ����Ă邩�ǂ���
typedef BOOL (*DevIsEndFunc)(void); ///<DevIsEnd                   �ʐM�I�����Ă悢���ǂ���
typedef BOOL (*DevIsIdleFunc)(void); ///<�A�C�h����Ԃ��ǂ���
typedef u32 (*DevGetBitmapFunc)(void); ///<DevGetBitmap               �ڑ���Ԃ�BITMAP�ŕԂ�
typedef u32 (*DevGetCurrentIDFunc)(void); ///<DevGetCurrentID            �����̐ڑ�ID��Ԃ�
typedef int (*DevGetIconLevelFunc)(void); ///<�A�C�R�����x����Ԃ�
typedef int (*DevGetErrorFunc)(void);  ///< �G���[�𓾂�
typedef void (*DevSetNoChildErrorFunc)(BOOL bOn);  ///< �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ�
typedef BOOL (*DevIsConnectableFunc)(int index); ///< ���肪�Ȃ��ł悢��Ԃ��ǂ��� (�ق�friendWifi�p)
typedef BOOL (*DevIsVChatFunc)(void);  ///< VChat���ǂ���
typedef BOOL (*DevIsNewPlayerFunc)(void);  ///< �ڑ����Ă������ǂ�����Ԃ��܂�
typedef void (*DevIrcMoveFunc)(void);   ///< IRC�ړ��֐�
typedef BOOL (*DevIsSendDataFunc)(void);  ///< �f�[�^�𑗂��Ă悢���ǂ���
typedef BOOL (*DevGetSendTurnFunc)(void);  ///< ���M�\�^�[���t���O���擾
typedef BOOL (*DevIsConnectSystemFunc)(void);  ///< �Đڑ����ȂǊ֌W�Ȃ��A�����ɍ��A�q�����Ă��邩
typedef BOOL (*DevGetSendLockFlagFunc)(void); ///< ���M���b�N�t���O���擾
typedef void (*DevConnectWorkInitFunc)(void); ///< ���߂Ă̐ڑ���̃��[�N�ݒ�

typedef BOOL (*DevLobbyLoginFunc)(const void* cp_loginprofile); ///<Wi-Fi�L��Ƀ��O�C��	DWC_LOBBY_Login
typedef void (*DevDebugSetRoomFunc)( u32 locktime, u32 random, u8 roomtype, u8 season ); ///<�f�o�b�O�p �����f�[�^�ݒ�DWC_LOBBY_DEBUG_SetRoomData
typedef BOOL (*DevLobbyUpdateErrorCheckFunc)(void);	///<DWC_LOBBY_UpdateErrorCheck
typedef BOOL (*DevLobbyLoginWaitFunc)(void);	///<DWC_LOBBY_LoginWait
typedef void (*DevLobbyLogoutFunc)(void);	///<DWC_LOBBY_Logout
typedef BOOL (*DevLobbyLogoutWaitFunc)(void);	///<DWC_LOBBY_LogoutWait


/// @brief �f�o�C�X�A�N�Z�X�p�\����
typedef struct{
  DevBootFunc DevBoot;
  DevInitFunc DevInit;                    ///< �I�[�o���C�Z�b�g �ڑ��N���X�̏����� �������m��
  DevStartFunc DevStart;                   ///< �ڑ�  WIFI�ڑ��܂ł��
  DevMainFunc DevMain;                    ///<����
  DevEndFunc DevEnd;                     ///<�ؒf
  DevExitFunc DevExit;                    ///<�������J���I�[�o�[���C�J��

  DevChildWorkInitFunc DevChildWorkInit;           ///<�q�@�G���A�N���A
  DevParentWorkInitFunc DevParentWorkInit;              ///<�e�@�G���A�N���A

  DevMyBeaconSetFunc DevMyBeaconSet;             ///<�����r�[�R����ݒ�
  DevBeaconSearchFunc DevBeaconSearch;            ///<�����̃Z�b�g �T�[�`�J�n
  DevBeaconGetFunc DevBeaconGet;               ///<�r�[�R���𓾂�
  DevBeaconGetMacFunc DevBeaconGetMac;     ///<�r�[�R����MAC�𓾂�
  DevIsBeaconChangeFunc DevIsBeaconChange;          ///<�r�[�R�����X�V���ꂽ���ǂ����𓾂�
  DevResetBeaconChangeFlgFunc DevResetBeaconChangeFlg;    ///<�t���O�����Z�b�g����
  DevIsChangeOverTurnFunc DevIsChangeOverTurn;        ///<�e�q�؂�ւ����Ă悢���ǂ���(�e�@�����ł��r�[�R���𑗐M���I��������ǂ���)

  DevSetChildConnectCallbackFunc DevSetChildConnectCallback; ///< �q�����Ȃ������̃R�[���o�b�N���w�肷��
  DevParentModeConnectFunc DevParentModeConnect;       ///<�e�J�n          
  DevChildModeConnectFunc DevChildModeConnect;        ///<�q���J�n        
  DevChildModeMacConnectFunc   DevChildModeMacConnect; ///<     �q���J�n      macAddress  
  DevChangeOverModeConnectFunc DevChangeOverModeConnect;   ///<�����_���ڑ�     �Ȃ��Ȃ��ꍇ-1
  DevWifiConnectFunc DevWifiConnect;  ///<WIFI�ڑ�
  DevModeDisconnectFunc DevModeDisconnect;          ///<�ڑ����I���    
  DevIsStepDSFunc DevIsStepDS;  ///< �f�[�^�V�F�A�����O�������M�o�����Ԃ�
  DevStepDSFunc DevStepDS;  ///< �f�[�^�V�F�A�����O�������M
  DevGetSharedDataAdrFunc DevGetSharedDataAdr;

  DevSendDataFunc DevSendData;                ///<���M�֐�                   
  DevRecvCallbackFunc DevRecvCallback;            ///<��M�R�[���o�b�N

  DevIsStartFunc DevIsStart;                 ///<�ʐM�ڑ����Ă邩�ǂ���
  DevIsConnectFunc DevIsConnect;               ///<����Ɛڑ����Ă邩�ǂ���
  DevIsEndFunc DevIsEnd;                 ///<�ʐM�I�����Ă悢���ǂ���
  DevIsIdleFunc DevIsIdle;   ///< �A�C�h����Ԃ��ǂ���
  DevGetBitmapFunc DevGetBitmap;               ///<�ڑ���Ԃ�BITMAP�ŕԂ�
  DevGetCurrentIDFunc DevGetCurrentID;            ///<�����̐ڑ�ID��Ԃ�
  DevGetIconLevelFunc DevGetIconLevel;  ///< �A�C�R�����x����Ԃ�
  DevGetErrorFunc DevGetError;  ///< �G���[�𓾂�
  DevSetNoChildErrorFunc DevSetNoChildError;  ///< �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ�
  DevIsConnectableFunc DevIsConnectable;   ///< ���肪�Ȃ��ł悢��Ԃ��ǂ��� (�ق�friendWifi�p)
  DevIsVChatFunc DevIsVChat;   ///< VChat���ǂ���
  DevIsNewPlayerFunc DevIsNewPlayer; ///< �ڑ����Ă������ǂ�����Ԃ��܂�
  DevIrcMoveFunc DevIrcMove;  ///< IRC�ړ��֐�
  DevIsSendDataFunc DevIsSendData; ///< �f�[�^�𑗂��Ă悢���ǂ���
  DevGetSendTurnFunc DevGetSendTurn; ///< ���M�\�^�[���t���O���擾
  DevIsConnectSystemFunc DevIsConnectSystem;  ///< �Đڑ����ȂǊ֌W�Ȃ��A�����ɍ��A�q�����Ă��邩
  DevGetSendLockFlagFunc DevGetSendLockFlag;  ///< ���M���b�N�t���O���擾
  DevConnectWorkInitFunc DevConnectWorkInit;           ///<���߂Ă̐ڑ���̃��[�N�ݒ�

  //-- �ȉ��AWi-Fi�L���p
  DevLobbyLoginFunc DevLobbyLogin;		///<Wi-Fi�L��Ƀ��O�C��	DWC_LOBBY_Login
  DevDebugSetRoomFunc DevDebugSetRoom;	///<�f�o�b�O�p �����f�[�^�ݒ� DWC_LOBBY_DEBUG_SetRoomData
  DevLobbyUpdateErrorCheckFunc DevLobbyUpdateErrorCheck;	///<DWC���r�[�X�V DWC_LOBBY_UpdateErrorCheck
  DevLobbyLoginWaitFunc DevLobbyLoginWait;	///<DWC_LOBBY_LoginWait
  DevLobbyLogoutFunc DevLobbyLogout;	///<DWC_LOBBY_Logout
  DevLobbyLogoutWaitFunc DevLobbyLogoutWait;	///<DWC_LOBBY_LogoutWait

  
} GFLNetDevTable;


/* �ȉ��̊֐���GFLIB�O�ɍ쐬����B�I�[�o�[���C�ƃh���C�o�[�̃��[�h��S������
   net_dev.c �̃T���v�������̂܂܎����Ă���΂悢*/


///   ���C�����X�f�o�C�X�����[�h����
extern GFLNetDevTable* NET_DeviceLoad(int deviceNo);
///   ���C�����X�f�o�C�X���A�����[�h����
extern void NET_DeviceUnload(int deviceNo);



#endif // GFL_NET_DEVICETBL_H__

#ifdef __cplusplus
} /* extern "C" */
#endif
