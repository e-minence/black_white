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


#undef GLOBAL
#ifdef __NET_SYS_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

// �f�o�b�O�p���܂蕶��----------------------
#define GFL_NET_DEBUG   (1)   ///< ���[�U�[�C���^�[�t�F�C�X�f�o�b�O�p 0:���� 1:�L��

#if defined(DEBUG_ONLY_FOR_ohno)
#undef GFL_NET_DEBUG
#define GFL_NET_DEBUG   (1)
#endif  //DEBUG_ONLY_FOR_ohno

#ifndef NET_PRINT
#if GFL_NET_DEBUG
#define NET_PRINT(...) \
  (void) ((OS_Printf(__VA_ARGS__)))
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


/// @brief �ʐM�Ǘ��\����
typedef struct _GFL_NETSYS GFL_NETSYS;
/// @brief �l�b�g���[�N�P�̂̃n���h��
typedef struct _GFL_NETHANDLE GFL_NETHANDLE;

// define 
//#define NET_NETID_ALLUSER (0xff)  ///< NetID:�S���֑��M����ꍇ
#define NET_NETID_SERVER (0xfe)   ///< NetID:�T�[�o�[�̏ꍇ���� ���0����ClientID
#define NET_SENDID_ALLUSER (0xff)  ///< NetID:�S���֑��M����ꍇ

#define GFL_NET_TOOL_INVALID_LIST_NO  (-1) ///<�����ȑI��ID


/// @brief  �R�}���h�֘A�̒�`

/// �R�[���o�b�N�֐��̏���
typedef void (*PTRCommRecvFunc)(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
/// �T�C�Y���Œ�̏ꍇ�T�C�Y���֐��ŕԂ�
typedef int (*PTRCommRecvSizeFunc)(void);
/// ��M�o�b�t�@���O���Ŏ����Ă���ꍇ���̃|�C���^
typedef u8* (*PTRCommRecvBuffAddr)(int netID, void* pWork, int size);

/// �R�}���h�p�P�b�g�e�[�u����`
typedef struct {
    PTRCommRecvFunc callbackFunc;    ///< �R�}���h���������ɌĂ΂��R�[���o�b�N�֐�
    PTRCommRecvSizeFunc getSizeFunc; ///< �R�}���h�̑��M�f�[�^�T�C�Y���Œ�Ȃ珑���Ă�������
    PTRCommRecvBuffAddr getAddrFunc;
} NetRecvFuncTable;

/// �σf�[�^���M�ł��邱�Ƃ������Ă���
#define   GFL_NET_COMMAND_SIZE_VARIABLE   (0x00ffffff)
/// �R�}���h�̃T�C�Y���֐������������f����ׂ̒�` (�g�p���Ȃ��ł�������)
#define   _GFL_NET_COMMAND_SIZE_H         (0xff000000)
/// �R�}���h�̃T�C�Y���`����ׂ̃}�N��
#define   GFL_NET_COMMAND_SIZE( num )     (PTRCommRecvSizeFunc)(_GFL_NET_COMMAND_SIZE_H + num)
/// �R�}���h�̃T�C�Y���`����ׂ̃}�N��
#define   GFL_NET_COMMAND_VARIABLE     (PTRCommRecvSizeFunc)(_GFL_NET_COMMAND_SIZE_H + num)


typedef enum {
  GFL_NET_ERROR_RESET_SAVEPOINT = 1,  ///< ���O�̃Z�[�u�ɖ߂��G���[
  GFL_NET_ERROR_RESET_TITLE,          ///< �^�C�g���ɖ߂��G���[
  GFL_NET_ERROR_RESET_OTHER,          ///< ���̑��̃G���[
} GFL_NET_ERROR_ENUM;

typedef u8 GameServiceID;  ///< �Q�[���T�[�r�XID  �ʐM�̎��
typedef u8 ConnectID;      ///< �ڑ����邽�߂�ID  0-16 �܂�
typedef u8 NetID;          ///< �ʐMID  0-200 �܂�

/// @brief ���M�������󂯎��R�[���o�b�N�^
typedef void (*CBSendEndFunc)(u16 command);

/// @brief ��ɑ��鑗�M�f�[�^�𓾂�
typedef u8* (*CBGetEveryTimeData)(void);
/// @brief ��ɑ������M�f�[�^�𓾂� �����Ă��Ȃ���pWork��NULL������
typedef void (*CBRecvEveryTimeData)(int netID, void* pWork, int size);


typedef void* (*NetBeaconGetFunc)(void);    ///< �r�[�R���f�[�^�擾�֐�
typedef int (*NetBeaconGetSizeFunc)(void);    ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
typedef BOOL (*NetBeaconCompFunc)(int GameServiceID1, int GameServiceID2);  ///< �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
typedef void (*NetErrorFunc)(GFL_NETHANDLE* pNet,int errNo);    ///< �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
typedef void (*NetConnectEndFunc)(GFL_NETHANDLE* pNet);  ///< �ʐM�ؒf���ɌĂ΂��֐�
typedef u8* (*NetGetSSID)(void);  ///< �e�q�ڑ����ɔF�؂���ׂ̃o�C�g�� 24byte



/// @brief �ʐM�̏������p�\����
typedef struct{
  NetRecvFuncTable* recvFuncTable;  ///< ��M�֐��e�[�u���̃|�C���^
  NetBeaconGetFunc beaconGetFunc;  ///< �r�[�R���f�[�^�擾�֐�
  NetBeaconGetSizeFunc beaconGetSizeFunc;  ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
  NetBeaconCompFunc beaconCompFunc; ///< �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NetErrorFunc errorFunc;           ///< �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NetConnectEndFunc connectEndFunc;  ///< �ʐM�ؒf���ɌĂ΂��֐�
  NetGetSSID getSSID;        ///< �e�q�ڑ����ɔF�؂���ׂ̃o�C�g��  
  int gsid;       ///< �Q�[���T�[�r�XID  �ʐM�̎��
  int ggid;                 ///< �c�r�ŃQ�[���\�t�g����ʂ���ׂ�ID
  u32  allocNo;             ///< alloc���邽�߂̔ԍ�
  u8 maxConnectNum;   ///< �ő�ڑ��l��
  u8 maxBeaconNum;    ///< �ő�r�[�R�����W��
  u8 bNetwork;    ///< �ʐM���J�n���邩�ǂ���
} GFLNetInitializeStruct;

//-------------------------------
//  �֐� �`�O�����J�֐��͑S�Ă����ɒ�`
//-------------------------------

// �֐��؂�o���������� funccut.rb  k.ohno 2006.12.5 
//==============================================================================
/**
 * @brief �ʐM������
 * @param[in]   pNetInit  �ʐM�������\���̂̃|�C���^
 * @param[in]   netID     �l�b�gID
 * @return none
 */
//==============================================================================
extern void GFL_NET_Initialize(const GFLNetInitializeStruct* pNetInit,int heapID);
extern GFL_NETHANDLE* GFL_NET_GetNetHandle(int netID);
extern BOOL GFL_NET_IsNegotiation(GFL_NETHANDLE* pHandle);

//==============================================================================
/**
 * @brief  �ʐM�I��
 * @retval  TRUE  �I�����܂���
 * @retval  FALSE �I�����܂��� ���Ԃ��󂯂Ă������Ă�ł�������
 */
//==============================================================================
extern BOOL GFL_NET_Finalize(void);
//-----�r�[�R���֘A
//==============================================================================
/**
 * @brief �r�[�R�����󂯎��T�[�r�X��ǉ��w��  �i�������̃T�[�r�X�̃r�[�R�����E���������Ɂj
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   GameServiceID gsid  �Q�[���T�[�r�XID  �ʐM�̎��
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_AddBeaconServiceID(GFL_NETHANDLE* pNet, GameServiceID gsid);
//==============================================================================
/**
 * @brief ��M�r�[�R���f�[�^�𓾂�
 * @param   index  �r�[�R���o�b�t�@index
 * @return  �r�[�R���f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
extern void* GFL_NET_GetBeaconData(int index);
//==============================================================================
/**
 * @brief ��M�r�[�R���ɑΉ�����MAC�A�h���X�𓾂�
 * @param   index  �r�[�R���o�b�t�@index
 * @return  �r�[�R���f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
extern u8* GFL_NET_GetBeaconMacAddress(int index);
//--------�ڑ��E�ؒf
//==============================================================================
/**
 * @brief   �ʐM�n���h����MainStatus���܂킷
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern void GFL_NET_MainProc(void);
//==============================================================================
/**
 * @brief   �ʐM�n���h�������   �ʐM��P�ʂ̃��[�N�G���A�̎�
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_CreateHandle(void);
//==============================================================================
/**
 * @brief   �ʐM�n���h��������   �ʐM��P�ʂ̃��[�N�G���A�̎�
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern void GFL_NET_DeleteNetHandle(GFL_NETHANDLE* pHandle);
//==============================================================================
/**
 * @brief �q�@�ɂȂ�ڑ�����
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern void GFL_NET_ClientConnect(GFL_NETHANDLE* pHandle);
//==============================================================================
/**
 * @brief �q�@�ɂȂ�w�肵���e�@�ɐڑ�����
 * @param   GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @return  none
 */
//==============================================================================
extern void GFL_NET_ClientConnectTo(GFL_NETHANDLE* pHandle,u8* macAddress, BOOL bAlloc);
//==============================================================================
/**
 * @brief    �e�@�ɂȂ�҂��󂯂�
 * @param    GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @return   none
 */
//==============================================================================
extern void GFL_NET_ServerConnect(GFL_NETHANDLE* pHandle);
//==============================================================================
/**
 * @brief �e�@�ɂȂ�w�肳�ꂽ�q�@�̐ڑ���҂��󂯂�
 * @param    GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @param   num            �q�@�l��
 * @return  none
 */
//==============================================================================
extern void GFL_NET_ServerConnectTo(GFL_NETHANDLE* pHandle,const u8* macAddress, const int num);
//==============================================================================
/**
 * @brief �e�q�؂�ւ��ڑ����s��
 * @param    GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================
extern void GFL_NET_SwitchConnect(GFL_NETHANDLE* pHandle);
//==============================================================================
/**
 * @brief ���̃n���h���̒ʐM�ԍ��𓾂�
 * @param[in,out]  NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  NetID     �ʐMID
 */
//==============================================================================
extern NetID GFL_NET_GetNetID(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief ���݂̐ڑ��䐔�𓾂�
 * @retval  int  �ڑ���
 */
//==============================================================================
extern int GFL_NET_GetConnectNum(void);
//extern int GFL_NET_GetConnectNum(GFL_NETHANDLE* pNet);
//==============================================================================
/**
 * @brief ID�̒ʐM�Ɛڑ����Ă��邩�ǂ�����Ԃ�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   NetID id            �l�b�gID
 * @retval  BOOL  �ڑ����Ă�����TRUE
 */
//==============================================================================
extern BOOL GFL_NET_IsConnectMember(GFL_NETHANDLE* pNet,NetID id);
//==============================================================================
/**
 * @brief   �ʐM�ؒf����
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_Disconnect(void);
//==============================================================================
/**
 * @brief �ڑ������ǂ���
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  none
 */
//==============================================================================
extern BOOL GFL_NET_IsConnect(GFL_NETHANDLE* pNet);
//==============================================================================
/**
 * @brief �ڑ����ύX
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   u8 num     �ύX��
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SetClientConnectNum(GFL_NETHANDLE* pNet,u8 num);
//==============================================================================
/**
 * @brief �V�K�ڑ��֎~������ݒ�
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   BOOL bEnable     TRUE=�ڑ����� FALSE=�֎~
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SetClientConnect(GFL_NETHANDLE* pNet,BOOL bEnable);
//--------���M
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
 * @param[in]   void* data                     ���M�f�[�^�|�C���^
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_SendData(GFL_NETHANDLE* pNet,const u16 sendCommand,const void* data);
//==============================================================================
/**
 * @brief ���M�J�n
 * @param[in,out]  pNet  �ʐM�n���h��
 * @param[in]   sendID                     ���M���� �S���֑��M����ꍇ NET_SENDID_ALLUSER
 * @param[in]   sendCommand                ���M����R�}���h
 * @param[in]   pCBSendEndFunc  ���M������������R�[���o�b�N�֐��̓o�^
 * @param[in]   size                       ���M�f�[�^�T�C�Y
 * @param[in]   data                     ���M�f�[�^�|�C���^
 * @param[in]   bDataCopy                 �f�[�^���R�s�[����ꍇTRUE
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u16 sendCommand, const u32 size,const void* data, const BOOL bFast, const BOOL bRepeat);
//==============================================================================
/**
 * @brief ���M�f�[�^���������ǂ���
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @retval  TRUE   �����ꍇ
 * @retval  FALSE  �c���Ă���ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_IsEmptySendData(GFL_NETHANDLE* pNet);
//==============================================================================
/**
 * @brief   ���t���[�����肽���f�[�^��o�^����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param   pGet      �f�[�^�擾�֐�
 * @param   pRecv     ��M�֐�
 * @param   size      �T�C�Y
 * @return  none
 */
//==============================================================================
extern void GFL_NET_SetEveryTimeSendData(GFL_NETHANDLE* pNet, CBGetEveryTimeData* pGet, CBRecvEveryTimeData* pRecv,const int size);
//--------���̑��A�c�[����
//==============================================================================
/**
 * @brief �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   u8 no   �^�C�~���O��肽���ԍ�
 * @return      none
 */
//==============================================================================
extern void GFL_NET_TimingSyncStart(GFL_NETHANDLE* pNet, const u8 no);
//==============================================================================
/**
 * @brief �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   no   �͂��ԍ�
 * @retval  TRUE    �^�C�~���O�����v
 * @retval  FALSE   �^�C�~���O��������Ă��Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_IsTimingSync(GFL_NETHANDLE* pNet, const u8 no);

#include "../src/network/net_command.h"


#endif // __NET_H__

