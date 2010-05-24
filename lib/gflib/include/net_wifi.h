#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
/**
 * @file	net_wifi.h
 * @brief	WIFI�ʐM���C�u�����̊O�����J�֐�
 * @author	k.ohno
 * @date    2007.2.28
 */
//=============================================================================
#ifndef __NET_WIFI_H__
#define __NET_WIFI_H__


//==============================================================================
/**
 * @brief   Wi-Fi�ڑ������� ���������ɌĂяo���Ă����K�v������
 * @param   heapID     �������̈�
 * @param   errorFunc  �G���[�\���֐�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WifiStart( int heapID, NetErrorFunc errorFunc );

//==============================================================================
/**
 * @brief   Wi-Fi�ݒ��ʂ��Ăяo��
 * @param   heapID  �������̈�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WifiUtility( int heapID );

//==============================================================================
/**
 * �����_���}�b�`���s���֐�
 * @target   ���c�����ŃQ�[�����J�ÁB�O�ȏ�c�ڑ����ɍs������́A�t�����h���X�g��̈ʒu
 * @retval  ���c�����B�O�c���s�B
 */
//==============================================================================
extern int GFL_NET_DWC_StartMatch( u8* keyStr,int numEntry, BOOL bParent, u32 timelimit );

//==============================================================================
/**
 * aid��Ԃ��܂��B�ڑ�����܂ł�MYDWC_NONE_AID��Ԃ��܂��B
 * @retval  aid�B�������ڑ��O��MYDWC_NONE_AID
 */
//==============================================================================
extern u32 GFL_NET_DWC_GetAid(void);
#define MYDWC_NONE_AID (0xffffffff)

//==============================================================================
/**
 * �Q�[����W�E�Q�����s���֐��B
 * @target   ���c�����ŃQ�[�����J�ÁB�O�ȏ�c�ڑ����ɍs������́A�t�����h���X�g��̈ʒu
 * @retval  ���c�����B�O�c���s�B
 */
//==============================================================================
extern int GFL_NET_DWC_StartGame( int target,int maxnum, BOOL bVCT );
//==============================================================================
/**
 * �ڑ������m�������ɌĂ΂��֐����Z�b�g����
 * @param 	pFunc  �ڑ����ɌĂ΂��֐�
 * @param 	pWork  ���[�N�G���A
 * @retval  none
 */
//==============================================================================
// �ڑ��R�[���o�b�N�^
//  �F�B���w��̏ꍇaid�͎�����aid
//  �F�B�w��̏ꍇaid�͑���̂�aid
typedef void (*MYDWCConnectFunc) (u16 aid,void* pWork);
extern void GFL_NET_DWC_SetConnectCallback( MYDWCConnectFunc pFunc, void* pWork );
//==============================================================================
/**
 * @brief   �]����Ԓ��̐ڑ�����Ԃ�
 * @param   none
 * @retval  ��
 */
//==============================================================================
extern int GFL_NET_DWC_AnybodyEvalNum(void);
//==============================================================================
/**
 * �ؒf�����m�������ɌĂ΂��֐����Z�b�g����
 * @param 	pFunc  �ؒf���ɌĂ΂��֐�
 * @param 	pWork  ���[�N�G���A
 * @retval  none
 */
//==============================================================================
// �ؒf�R�[���o�b�N�^
typedef void (*MYDWCDisconnectFunc) (u16 aid,void* pWork);
extern void GFL_NET_DWC_SetDisconnectCallback( MYDWCDisconnectFunc pFunc, void* pWork );

//==============================================================================
/**
 * DWC�ʐM�����X�V���s��
 * @param none
 * @retval 0�c����, ���c�G���[����
 */
//==============================================================================
extern void GFL_NET_DWC_StartVChat(void);
//==============================================================================
/**
 * @brief   �{�C�X�`���b�g���~���܂�
 * @param   void
 * @retval  void
 */
//==============================================================================
extern void GFL_NET_DWC_StopVChat(void);

// ���łɁA�����f�[�^�����X�g��ɂ���B���̏ꍇ�͉�������K�v�Ȃ��B
#define DWCFRIEND_INLIST 0
// ���łɁA�����f�[�^�����X�g��ɂ��邪�A�㏑�����邱�Ƃ��]�܂����ꍇ�B
#define DWCFRIEND_OVERWRITE 1
// �����f�[�^�̓��X�g��ɂȂ��B
#define DWCFRIEND_NODATA 2
// �f�[�^���������Ȃ��B
#define DWCFRIEND_INVALID 3

//==============================================================================
/**
 * �G���[�R�[�h��ێ�����
 * @param   code  DWC_GetLastErrorEx�Ŏ擾�����G���[�ԍ�
 * @param   type  DWC_GetLastErrorEx�Ŏ擾���������^�C�vDWCErrorType
 * @param   ret   DWC_GetLastErrorEx�Ŏ擾�����G���[ DWCError
 * @param   no    ��ɃV�X�e�����G���[�@ERRORCODE_HEAP��ERRORCODE_TIMEOUT
 */
//==============================================================================
extern void GFL_NET_StateSetWifiError(int code, int type, int ret, int no );

//==============================================================================
/**
 * WifiError�̃G���[���N���A����B�y�x�̃G���[���������邽�߂̂��́B
 * �d�x�ȃG���[�͐ؒf��V���b�g�_�E�����������Ȃ��������邱�ƁI
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateClearWifiError( void );

typedef struct{
  int errorCode;
  int errorType;
  int errorRet;
  int errorUser;
} GFL_NETSTATE_DWCERROR;

//==============================================================================
/**
 * �G���[�R�[�h�������o��
 * @param   none
 * @retval  none
 */
//==============================================================================
extern GFL_NETSTATE_DWCERROR* GFL_NET_StateGetWifiError(void);


#define GFL_NET_STATE_MATCHING (0)
#define GFL_NET_STATE_MATCHED (1)
#define GFL_NET_STATE_NOTMATCH (2)
#define GFL_NET_STATE_TIMEOUT (3)
#define GFL_NET_STATE_DISCONNECTING (4)
#define GFL_NET_STATE_FAIL (5)

//==============================================================================
/**
 * WIFI�̃X�e�[�g�J�ڂ𓾂�
 * @brief   �}�b�`���O�������������ǂ����𔻒�
 * @param   pNetHandle   GFL_NETHANDLE
 * @param   target: ���Ȃ�e�A�O�ȏ�Ȃ�Ȃ��ɂ����e�@�̔ԍ�
 * @retval  1. �����@�@0. �ڑ���   2. �G���[��L�����Z���Œ��f
 */
//==============================================================================
extern int GFL_NET_StateGetWifiStatus(void);

//==============================================================================
/**
 * WifiError���ǂ���
 * @param   none
 * @retval  Error
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiError(void);
//==============================================================================
/**
 * @brief   Wifi�ؒf�R�}���h���󂯎�����ꍇTRUE
 * @param   none
 * @retval  WifiLogout�Ɉڍs�����ꍇTRUE
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiDisconnect(void);
//==============================================================================
/**
 * @brief   WIFI�w��ڑ����J�n����
 * @param   target    �Ȃ��ɂ����e�@�̔ԍ�
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
extern int GFL_NET_StateStartWifiPeerMatch( int target );
//==============================================================================
/**
 * @brief   WIFI�����_���}�b�`���J�n����
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
extern int GFL_NET_StateStartWifiRandomMatch( void);
//==============================================================================
/**
 * @brief   WIFI�����_���}�b�`���J�n����
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
extern int GFL_NET_StateStartWifiRandomMatch_RateMode(void );
//==============================================================================
/**
 * @brief   WIFI�Ń}�b�`���O��؂�ꍇ�̏���
 * @param   bAuto  �������狭���ؒf����Ƃ���FALSE �ڑ��󋵂ɂ��킹��ꍇTRUE
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateWifiMatchEnd(BOOL bAuto);
//==============================================================================
/**
 * @brief   wifi�}�b�`���O��Ԃփ��O�C������
 * @param   netHeapID   netLib�Ŋm�ێg�p����ID
 * @param   wifiHeapID  NitroWifi&NitroDWC&VCT�Ŋm�ۂ��郁����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateWifiEnterLogin(void);
//==============================================================================
/**
 * @brief   WIFI�Ń��O�A�E�g���s���ꍇ�̏�
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateWifiLogout(NetEndCallback netEndCallback);
//----------------------------------------------------------------------------
/**
 *	@brief	�Z�[�u���ėǂ����ǂ����m�F����t���O
 *	@param	TRUE �Z�[�u���Ă悢
 */
//-----------------------------------------------------------------------------
extern u8 GFL_NET_DWC_GetSaving(void);
//==============================================================================
/**
 * @brief   WifiLogin�Ɉڍs�������ǂ����̊m�F
 * @param   none
 * @retval  WifiLogin�Ɉڍs�����ꍇTRUE
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiLoginState(void);
//==============================================================================
/**
 * @brief   WifiError���ǂ���
 * @param   none
 * @retval  Error
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiError(void);
//==============================================================================
/**
 * @brief   Wifi�ؒf�R�}���h���󂯎�����ꍇTRUE
 * @param   none
 * @retval  WifiLogout�Ɉڍs�����ꍇTRUE
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiDisconnect(void);
//==============================================================================
/**
 * @brief   WifiLoginMatch�Ɉڍs�������ǂ����̊m�F
 * @param   none
 * @retval  WifiLoginMatch�Ɉڍs�����ꍇTRUE
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiLoginMatchState(void);
//==============================================================================
/**
 * @brief   WIFI�Ń}�b�`���O��؂�ꍇ�̏��� �ؒf�R�}���h�𑗐M
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateSendWifiMatchEnd(GFL_NETHANDLE* pNet);

//----------------------------------------------------------------------------
/**
 *	@brief	WiFi���r�[��p	���O�C���֐�
 *
 *	@param	p_save				�Z�[�u�f�[�^
 *	@param	cp_initprofile		�������v���t�B�[���f�[�^(�|�C���^��ێ�����ׁA���̂͊O���ŉ�����Ȃ��悤��)
 */
//-----------------------------------------------------------------------------
extern void GFL_NET_StateWifiLobbyLogin( const void* cp_initprofile );
#ifdef PM_DEBUG
extern void GFL_NET_StateWifiLobbyLogin_Debug( const void* cp_initprofile, u32 season, u32 room );
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	WiFi���r�[�̃��O�A�E�g
 */
//-----------------------------------------------------------------------------
extern void GFL_NET_StateWifiLobbyLogout( void );

//----------------------------------------------------------------------------
/**
 *	@brief	DWC_LoginAsync�̃��O�C���܂ł������������`�F�b�N
 *	@retval	TRUE	����
 *	@retval	FALSE	���ĂȂ�
 */
//-----------------------------------------------------------------------------
extern BOOL GFL_NET_StateWifiLobbyDwcLoginCheck( void );

//----------------------------------------------------------------------------
/**
 *	@brief	WiFi���r�[�~�j�Q�[���J�n
 *
 *	@param	type		�Q�[���^�C�v(DWC_LOBBY_MG_TYPE)
 */
//-----------------------------------------------------------------------------
extern void GFL_NET_StateWifiP2PStart( int type );

//----------------------------------------------------------------------------
/**
 *	@brief	WiFi���r�[�~�j�Q�[���I��
 *			P2P�ؒf�`�F�b�N�́AGFL_NET_StateWifiP2PGetConnectState�ōs���Ă�������
 */
//-----------------------------------------------------------------------------
extern void GFL_NET_StateWifiP2PEnd( void );





// ��M�R�[���o�b�N�^�BWHReceiverFunc�ƌ`��ς���BOOL��ǉ�
typedef BOOL (*GFL_NET_MYDWCReceiverFunc) (u16 aid, u16 *data, u16 size);

//�ڑ����� �t�����h�R�[�h���X�g�C���f�b�N�X
typedef BOOL (*GFL_NET_MYDWCConnectModeCheckFunc) (int index,void* pWork);

#define DWCRAP_STARTGAME_OK (0)
#define DWCRAP_STARTGAME_NOTSTATE  (-1)
#define DWCRAP_STARTGAME_RETRY  (-2)
#define DWCRAP_STARTGAME_FAILED  (-3)
#define DWCRAP_STARTGAME_FIRSTSAVE (-4)

#define STEPMATCH_CONTINUE 0

#define __ERROR_FRIENDS_SHORTAGE_DWC  (10)  ///< @todo �����Ȍ��т� dev_wifi�Ŋm�F���Ă�����

#define STEPMATCH_SUCCESS  (1000)    //�̂�DWC_ERROR_NUM
#define STEPMATCH_CANCEL (STEPMATCH_SUCCESS+1)
#define STEPMATCH_FAIL (STEPMATCH_SUCCESS+2)
#define STEPMATCH_TIMEOUT (STEPMATCH_SUCCESS+3)
#define STEPMATCH_DISCONNECT (STEPMATCH_SUCCESS+4)
#define ERRORCODE_0 (STEPMATCH_SUCCESS+5)
#define ERRORCODE_HEAP (STEPMATCH_SUCCESS+6)
#define STEPMATCH_CONNECT (STEPMATCH_SUCCESS+7)
#define ERRORCODE_TIMEOUT (STEPMATCH_SUCCESS+8)
#define ERRORCODE_DISCONNECT (STEPMATCH_SUCCESS+9)


#if GFL_NET_WIFI //wifi
// �������牺�̓f�o�C�X���t�A�N�Z�X����ׂɕK�v�Ȃ��̂Ȃ̂Ŏg�p���Ȃ��ł�������
extern void GFI_NET_NetWifiSaveUserDataFunc(void);
extern void GFI_NET_NetWifiMargeFrinedDataFunc(int deletedIndex,int srcIndex);
extern void* GFI_NET_GetMyDWCUserData(void);  //DWCUserData
extern void* GFI_NET_GetMyDWCFriendData(void);//DWCFriendData
extern int GFI_NET_GetFriendNumMax(void);

#endif //GFL_NET_WIFI


#endif //__NET_WIFI_H__
#ifdef __cplusplus
} /* extern "C" */
#endif
