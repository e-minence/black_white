//=============================================================================
/**
 * @file	comm_state.h
 * @brief	�ʐM��Ԃ��Ǘ�����T�[�r�X  �ʐM�̏�ʂɂ���
 *          �X���b�h�̂ЂƂƂ��ē����A�����̒ʐM��Ԃ⑼�̋@���
 *          �J�n��I�����Ǘ�����
 *          communication�����݂̂��Ĉړ�
 * @author	k.ohno
 * @date    2006.01.12
 */
//=============================================================================

#pragma once

#include "savedata/save_control.h"
//#include "savedata/regulation.h"
typedef void REGULATION;

//#include "net_old/comm_dwc_lobbylib.h"
#include "src/net/dwc_lobbylib.h"   //WIFIlobby

#ifdef PM_DEBUG
#define COMMST_DEBUG_WFLBY_START	// �f�o�b�N�J�n��`
#endif

//==============================================================================
// extern�錾
//==============================================================================
#ifdef PM_DEBUG
extern void DebugOhnoCommDebugUnderNo(int no);
extern void DebugOhnoCommDebugUnderParentOnly(int no);
extern void DebugOhnoCommDebugUnderChildOnly(int no);
#endif


extern BOOL CommStateIsInitialize(void);
// �n���ɓ���ꍇ�̏��� �X�e�[�g���n���p�ɕύX�����
extern void CommStateEnterUnderGround(SAVE_CONTROL_WORK* pSaveData);
// �n�����ł�ꍇ�̏��� �X�e�[�g�������Ȃ��ɕύX�����
extern void CommStateExitUnderGround(void);
// �n���ɂ����Đڑ���STATE�Ȃ̂��ǂ����Ԃ�
extern BOOL CommIsUnderGroundConnectingState(void);
// �n���ɂ�����RESET�X�e�[�g�Ȃ̂��ǂ���
extern BOOL CommIsUnderResetState(void);
// �n���ɂ����Ĕ閧��n�ɓ���ׂɐؒf���s��
extern void CommStateUnderGroundOfflineSecretBase(void);
// �閧��n����o�Ă�����ڑ��������s��
extern void CommStateUnderGroundOnlineSecretBase(void);
// �n���̃r�[�R�����W���ĊJ
extern void CommStateUnderRestart(void);
// �n���̃r�[�R�����W���ĊJ
extern void CommStateUnderParentRestart(void);
// �n��������~��Ԃɂ���
extern void CommStaetUnderPause(BOOL bFlg);
// �������@��X�^�[�g
extern void CommStateUnderGroundDigFossilStart(void);
extern void CommStateUnderGroundDigFossilEnd(void);


// �o�g�����ɐe�ɂȂ�ꍇ�̏����J�n
#ifdef PM_DEBUG
extern void CommStateEnterBattleParent(SAVE_CONTROL_WORK* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg, BOOL bWifi, int soloDebugNo);
#else
extern void CommStateEnterBattleParent(SAVE_CONTROL_WORK* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg, BOOL bWifi);
#endif
// �o�g�����Ɏq�ɂȂ�ꍇ�̏����J�n
#ifdef PM_DEBUG
extern void CommStateEnterBattleChild(SAVE_CONTROL_WORK* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg, BOOL bWifi, int soloDebugNo);
#else
extern void CommStateEnterBattleChild(SAVE_CONTROL_WORK* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg, BOOL bWifi);
#endif
// �o�g�����ɐe�����߂��ۂ̎q�@�̏���
extern void CommStateConnectBattleChild(int connectIndex);
// �o�g�����Ɏq�@���ċN�����鎞�̏���
extern void CommStateRebootBattleChild(void);
// �I�������葱��
extern void CommStateExitBattle(void);
// �o�g���ڑ���STATE�Ȃ̂��ǂ����Ԃ�
extern BOOL CommIsBattleConnectingState(void);

/// ���j�I���q�@�ɂȂ�\��Ȃ̂ŒN���Ȃ���Ȃ�
extern void CommStateChildReserve(void);




extern void CommStateSetPokemon(u8* sel);
// �R�s�[
extern void CommStateGetPokemon(u8* sel);



#ifdef PM_DEBUG
// �^�C�g������ʐM�f�o�b�O���s�����̊֐�
extern void CommStateDBattleConnect(BOOL bParent, int gameMode ,SAVE_CONTROL_WORK* pSaveData);
// WIFI�X�e�[�g����
extern void CommStateWifiCheckFunc(void);

// �f�o�b�O�X�^�[�g�����낦��֐�
extern void CommStateRecvDebugStart(int netID, int size, void* pData, void* pWork);
// �퓬�V�[�P���X�ɐi��ł������ǂ���
extern BOOL CommStateDBattleIsReady(void);


#endif

// ���G�����f���悤�ڑ��ɕύX
extern void CommStateUnionPictureBoardChange(void);
//���G����state�Ɉڍs�����ꍇTRUE
extern BOOL CommStateIsUnionPictureBoardState(void);
// ���G�����q�@�Ƃ��Čq��
extern void CommStateUnionPictureBoardChild(int index);

// ���R�[�h�R�[�i�[�̏�Ԃɐ؂�ւ���
extern void CommStateUnionRecordCornerChange(void);
// ���R�[�h�R�[�i�[�q�@�Ƃ��Čq��
extern void CommStateUnionRecordCornerChild(int index);

// ���邮������̏�Ԃɐ؂�ւ���
extern void CommStateUnionGuru2Change(void);
// ���邮������q�@�Ƃ��Čq��
extern void CommStateUnionGuru2Child(int index);


/// �ڑ��m�F����
extern void CommRecvNegotiation(int netID, int size, void* pData, void* pWork);
/// �ڑ��m�F���� �e�@����̕Ԏ�
extern void CommRecvNegotiationReturn(int netID, int size, void* pData, void* pWork);
/// �ڑ��m�F�R�}���h�̃T�C�Y
extern int CommRecvGetNegotiationSize(void);

/// �����Ă���l�̐��𐧌�
extern void CommStateSetLimitNum(int num);
/// �ۂ������ڑ��J�n
extern void CommStateEnterPockchChild(SAVE_CONTROL_WORK* pSaveData);
/// �ۂ����������葱��
extern void CommStateExitPoketch(void);
/// �ۂ������ɂ����ď����WSTATE�Ȃ̂��ǂ����Ԃ�
extern BOOL CommIsPoketchSearchingState(void);

// �p�[�e�B�[�Q�[�����
#define PARTYGAME_MYSTERY_BCON (0x01)
#define PARTYGAME_RANGER_BCON  (0x02)
#define PARTYGAME_WII_BCON     (0x04)
#define PARTYGAME_DOWNLOAD_BCON (0x08)

// * �p�[�e�B�[�Q�[�������̒ʐM�����J�n�i�q�@��Ԃ̂݁j
extern void CommStateEnterPartyGameScanChild(SAVE_CONTROL_WORK* pSaveData);
// �p�[�e�B�[�Q�[���T�[�`�̏I������
extern void CommStateExitPartyGameScan(void);
// �E�����r�[�R����BIT��Ԃ�
extern u8 CommStateGetPartyGameBit(void);

/// ���Z�b�g�p�̒ʐM�ؒf
extern BOOL CommStateExitReset(void);

extern void CommStateCheckFunc(void);

// WIFI�o�g���ڑ��p�֐�
#ifdef PM_DEBUG
void CommStateWifiEnterBattleChild(SAVE_CONTROL_WORK* pSaveData, int serviceNo, int regulationNo, int soloDebugNo);
#else
void CommStateWifiEnterBattleChild(SAVE_CONTROL_WORK* pSaveData, int serviceNo, int regulationNo);
#endif
#ifdef PM_DEBUG
void CommStateWifiEnterBattleParent(SAVE_CONTROL_WORK* pSaveData, int serviceNo, int regulationNo, int soloDebugNo);
#else
void CommStateWifiEnterBattleParent(SAVE_CONTROL_WORK* pSaveData, int serviceNo, int regulationNo);
#endif
//

extern void CommStateSetEntryChildEnable(BOOL bEntry);

//extern void CommStateSetDisconnectFlg(BOOL bFlg);
extern void CommStateSetErrorCheck(BOOL bFlg,BOOL bAuto);
//extern void CommStateSetErrorCheck(BOOL bFlg);
extern BOOL CommStateGetErrorCheck(void);

extern BOOL CommStateGetWifiDPWError(void);


extern void* CommStateWifiEnterLogin(SAVE_CONTROL_WORK* pSaveData, int wifiFriendStatusSize);
extern void* CommStateGetMatchWork(void);
extern BOOL CommStateIsWifiLoginState(void);
extern BOOL CommStateIsWifiLoginMatchState(void);
extern void CommStateWifiMatchEnd(void);
extern void CommStateWifiTradeMatchEnd(void);
extern void CommStateWifiBattleMatchEnd(void);
extern BOOL CommStateIsWifiDisconnect(void);

extern int CommWifiWaitingCancel( void );

extern int CommStateGetServiceNo(void);
extern int CommStateGetRegulationNo(void);
#ifdef PM_DEBUG
extern int CommStateGetSoloDebugNo(void);
#endif

extern void CommStateWifiLogout(void);
//extern int CommStateGetWifiErrorNo(void);
extern BOOL CommStateIsWifiError(void);

//�Q�[�����J�n����
extern int CommWifiBattleStart( int target );
// �S�l�p�Q�[�����J�n����
extern int CommWifiPofinStart( int target );

//�}�b�`���O�������������ǂ����𔻒�
extern int CommWifiIsMatched();

// �ʐM��WIFI�ڑ����Ă��邩�ǂ�����Ԃ�
extern BOOL CommStateIsWifiConnect(void);


extern void CommStateWifiDPWStart(SAVE_CONTROL_WORK* pSaveData);
extern void CommStateWifiDPWEnd(void);

extern void CommStateWifiFusigiStart(SAVE_CONTROL_WORK* pSaveData);
extern void CommStateWifiFusigiEnd(void);

extern void CommStateWifiEMailStart(SAVE_CONTROL_WORK* pSaveData);
extern void CommStateWifiEMailEnd(void);



// ���r�[�ł��g�p�ł���֐�
//	BOOL CommStateIsInitialize();
//	BOOL CommStateIsWifiLoginState(void);
//	BOOL CommStateIsWifiLoginMatchState(void);
//	int CommStateGetWifiErrorNo(void);
//	BOOL CommStateIsWifiError(void);
//	BOOL CommStateIsWifiConnect(void);
//
// ���r�[��p�̊֐�
// ���̊֐����ĂԑO��DWC_LOBBY_Init���s���Ă����Ă��������B
extern void CommStateWifiLobbyLogin( SAVE_CONTROL_WORK* p_save, const void* cp_initprofile );
extern void CommStateWifiLobbyLogout( void );
extern BOOL CommStateWifiLobbyDwcLoginCheck( void );
extern BOOL CommStateWifiLobbyError( void );

extern void CommStateWifiP2PStart( DWC_LOBBY_MG_TYPE type, const HEAPID heapID, SAVE_CONTROL_WORK* pSaveData, int wifiFriendStatusSize );
//extern void CommStateWifiP2PStart( DWC_LOBBY_MG_TYPE type );

extern void CommStateWifiP2PEnd( void );	// CommStateWifiP2PGetConnectState�֐��Őؒf�`�F�b�N���s������
enum{
	COMMSTATE_WIFIP2P_CONNECT_NONE,			// �ڑ����ĂȂ�
	COMMSTATE_WIFIP2P_CONNECT_MATCHWAIT,	// �ڑ��҂�
	COMMSTATE_WIFIP2P_CONNECT_MATCH,		// �ڑ�����
	COMMSTATE_WIFIP2P_CONNECT_DISCONNECT,	// �ؒf��
};
extern u32 CommStateWifiP2PGetConnectState( void );
#ifdef COMMST_DEBUG_WFLBY_START
extern void CommStateWifiLobbyLogin_Debug( SAVE_CONTROL_WORK* p_save, const void* cp_initprofile, u32 season, u32 room );
#endif	// COMMST_DEBUG_WFLBY_START


#define COMM_ERROR_RESET_SAVEPOINT  (1)
#define COMM_ERROR_RESET_TITLE  (2)
#define COMM_ERROR_RESET_GTS  (3)
#define COMM_ERROR_RESET_OTHER (4)
#if PL_G252_081217_FIX
#define COMM_ERROR_RESET_POWEROFF (5)
#endif //PL_G252_081217_FIX


// ���Z�b�g����G���[�����������ꍇ�ɌĂ�
extern void CommSetErrorReset(u8 type);
// ���Z�b�g����G���[�������������ǂ���
extern u8 CommIsResetError(void);

// fatal�G���[�֐�
extern void CommFatalErrorFunc(int no);
extern void CommFatalErrorFunc_NoNumber( void );


// �n���͂��߂̃C�x���g���s���ꍇ�ݒ�
extern void CommStateSetFirstEvent(void);

// WIFI����ؒf�v��
extern void CommRecvExit(int netID, int size, void* pData, void* pWork);

extern BOOL CommStateIsResetEnd(void);

extern void CommStateSetParentOnlyFlg(BOOL bFlg);


#define COMM_ERRORTYPE_ARESET (0)     // A�{�^���Ń��Z�b�g
#define COMM_ERRORTYPE_POWEROFF (1)   // �d����؂�Ȃ��Ƃ����Ȃ�
#define COMM_ERRORTYPE_TITLE (2)     // A�{�^���Ń��Z�b�g �^�C�g���߂�
#define COMM_ERRORTYPE_GTS (3)     // A�{�^���Ń��Z�b�g �Z�[�u�|�C���g�֖߂�
#define COMM_ERRORTYPE_ARESET_WIFI (5)  // A�{�^���Ń��Z�b�g(Wi-Fi�ڑ���)
#define COMM_ERRORTYPE_TITLE_WIFI (6)  // A�{�^���Ń��Z�b�g �^�C�g���߂�(Wi-Fi�ڑ���)

// �G���[�\���֐�
extern void ComErrorWarningResetCall( int heapID, int type, int code );
extern void DWClibWarningCall( int heapID, int error_msg_id );


// �G���[�ɂ���֐�   �ȉ��̔ԍ����g�p���Ă�������
//#define COMM_ERROR_RESET_SAVEPOINT  (1)
//#define COMM_ERROR_RESET_TITLE  (2)
//#define COMM_ERROR_RESET_GTS  (3)
//#define COMM_ERROR_RESET_OTHER (4)
extern BOOL CommStateSetError(int no);

// WIFI�|�t�B��
extern void CommStateChangeWiFiPofin( void );
extern void CommStateChangeWiFiLobbyMinigame( void );
extern void CommStateChangeWiFiLogin( void );
extern void CommStateChangeWiFiBattle( void );
extern void CommStateChangeWiFiClub( void );

extern void CommStateChangeWiFiFactory( void );

// �|�P�b�`�̎��ȊO�ŒʐM���Ă��邩�ǂ�����Ԃ�
// @retval  �ʐM��Ԓ���TRUE
extern BOOL CommStateIsInitializeOtherPoketch(void);
//
extern void CommStateSetWifiError(int code, int type, int ret);

typedef struct{
  int errorCode;
  int errorType;
  int errorRet;
} COMMSTATE_DWCERROR;

extern COMMSTATE_DWCERROR* CommStateGetWifiError(void);



