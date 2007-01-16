//=============================================================================
/**
 * @file	net_state.h
 * @brief	�ʐM��Ԃ��Ǘ�����T�[�r�X  �ʐM�̏�ʂɂ���
 *          �X���b�h�̂ЂƂƂ��ē����A�����̒ʐM��Ԃ⑼�̋@���
 *          �J�n��I�����Ǘ�����
 *          communication�����݂̂��Ĉړ�
 * @author	k.ohno
 * @date    2006.01.12
 */
//=============================================================================

#ifndef __NET_STATE_H__
#define __NET_STATE_H__


//==============================================================================
// extern�錾
//==============================================================================
#ifdef PM_DEBUG
extern void DebugOhnoCommDebugUnderNo(int no);
extern void DebugOhnoCommDebugUnderParentOnly(int no);
extern void DebugOhnoCommDebugUnderChildOnly(int no);
#endif


extern void GFL_NET_StateDeviceInitialize(GFL_NETHANDLE* pNetHandle);

extern void GFL_NET_StateConnectMacAddress(GFL_NETHANDLE* pNetHandle);
extern void GFL_NET_StateConnectParent(GFL_NETHANDLE* pNetHandle,int heapID);
extern void GFL_NET_StateRecvExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void GFL_NET_StateRecvNegotiation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void GFL_NET_StateRecvNegotiationReturn(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void GFL_NET_StateExit(GFL_NETHANDLE* pNetHandle);


extern BOOL CommStateIsInitialize(void);

// �o�g�����ɐe�ɂȂ�ꍇ�̏����J�n
extern void CommStateEnterBattleParent(int serviceNo, int regulationNo, BOOL bWifi);
// �o�g�����Ɏq�ɂȂ�ꍇ�̏����J�n
extern void CommStateEnterBattleChild(int serviceNo, int regulationNo, BOOL bWifi);
// �o�g�����ɐe�����߂��ۂ̎q�@�̏���
extern void CommStateConnectBattleChild(int connectIndex);
// �o�g�����Ɏq�@���ċN�����鎞�̏���
extern void CommStateRebootBattleChild(void);
// �I�������葱��
extern void CommStateExitBattle(void);
// �o�g���ڑ���STATE�Ȃ̂��ǂ����Ԃ�
extern BOOL CommIsBattleConnectingState(void);






extern void CommStateSetPokemon(u8* sel);
// �R�s�[
extern void CommStateGetPokemon(u8* sel);




/// �ڑ��m�F����
extern void CommRecvNegotiation(const int netID, const int size, const void* pData, void* pWork,GFL_NETHANDLE* pNetHandle);
/// �ڑ��m�F���� �e�@����̕Ԏ�
extern void CommRecvNegotiationReturn(const int netID, const int size, const void* pData, void* pWork,GFL_NETHANDLE* pNetHandle);
/// �ڑ��m�F�R�}���h�̃T�C�Y
extern int CommRecvGetNegotiationSize(void);

/// �����Ă���l�̐��𐧌�
extern void CommStateSetLimitNum(int num);


/// ���Z�b�g�p�̒ʐM�ؒf
extern BOOL CommStateExitReset(void);

extern void CommStateCheckFunc(void);

extern void CommStateSetEntryChildEnable(BOOL bEntry);

extern void CommStateSetErrorCheck(BOOL bFlg,BOOL bAuto);
extern BOOL CommStateGetErrorCheck(void);

extern BOOL CommStateGetWifiDPWError(void);


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
extern int CommStateGetWifiErrorNo(void);
extern BOOL CommStateIsWifiError(void);

//�Q�[�����J�n����B// 2006.4.13 �g���ǉ�
extern int CommWifiBattleStart( int target );

//�}�b�`���O�������������ǂ����𔻒� // 2006.4.13 �g���ǉ�
extern int CommWifiIsMatched();

// �ʐM��WIFI�ڑ����Ă��邩�ǂ�����Ԃ�
extern BOOL CommStateIsWifiConnect(void);



#define COMM_ERROR_RESET_SAVEPOINT  (1)
#define COMM_ERROR_RESET_TITLE  (2)
#define COMM_ERROR_RESET_GTS  (3)
#define COMM_ERROR_RESET_OTHER (4)


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
extern void CommRecvExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);

extern BOOL CommStateIsResetEnd(void);

extern void CommStateSetParentOnlyFlg(BOOL bFlg);


#define COMM_ERRORTYPE_ARESET (0)     // A�{�^���Ń��Z�b�g
#define COMM_ERRORTYPE_POWEROFF (1)   // �d����؂�Ȃ��Ƃ����Ȃ�
#define COMM_ERRORTYPE_TITLE (2)     // A�{�^���Ń��Z�b�g �^�C�g���߂�
#define COMM_ERRORTYPE_GTS (3)     // A�{�^���Ń��Z�b�g �Z�[�u�|�C���g�֖߂�

// �G���[�\���֐�
extern void ComErrorWarningResetCall( int heapID, int type, int code );
extern void DWClibWarningCall( int heapID, int error_msg_id );


// �G���[�ɂ���֐�   �ȉ��̔ԍ����g�p���Ă�������
//#define COMM_ERROR_RESET_SAVEPOINT  (1)
//#define COMM_ERROR_RESET_TITLE  (2)
//#define COMM_ERROR_RESET_GTS  (3)
//#define COMM_ERROR_RESET_OTHER (4)
extern BOOL CommStateSetError(int no);


extern void GFL_NET_StateMainProc(GFL_NETHANDLE* pHandle);



#endif //__NET_STATE_H__

