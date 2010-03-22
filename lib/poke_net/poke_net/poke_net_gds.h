/*===========================================================================*
  Project:  Syachi PokemonGDS Network Library
  File:     poke_net_gds.h

  $NoKeywords: $
 *===========================================================================*/
/*! @file
	@brief	Syachi Pokemon GDS WiFi ���C�u����
	@date	2010/03/17
*/
//===========================================================================
/**
		@mainpage	Syachi Pokemon GDS WiFi ���C�u����
			<b>�E�T�v</b><br>
			�@�{���C�u�����́ANintendoDS�EDSi��Ń|�P�����f�c�r�֘A�̒ʐM���s���ׂ̂��̂ł��B<br>
			<br>
			<b>�E����</b><br>
			�@�{�w���v�t�@�C���̓��t�@�����X�}�j���A���ł��B<br>
			�@�Z�b�g�A�b�v���@�͕ʓr syachi_poke_net_inst.pdf ���Q�Ƃ��Ă��������B<br>
			<br>
			<b>�E�o�[�W��������</b><br>
			�@0.21�@[10/03/17]�@�EGDS�\���̂̈ꕔ�ύX�ւ̑Ή�<br>
			�@0.20�@[10/03/05]�@�ESSL�ʐM������<br>
			�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�ESSL�ʐM�̎����ɔ����A�O�o�[�W�����܂ŒʐM���ɍs���Ă����Í����^��������p�~<br>
			�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�EPOKE_NET_GDS_Initialize�֐��̈�����ύX<br>
			�@0.10�@[10/02/15]�@�E���񃊃��[�X<br>
			<br>
			<b>�E�ӎ�</b><br>
			�@���̐��i�ɂ́AOpenSSL Toolkit �Ŏg�p���邽�߂�OpenSSL Project �ɂ���ĊJ�����ꂽ�\�t�g�E�F�A���g�ݍ��܂�Ă��܂��B (http://www.openssl.org/)<br>
			�@���̐��i�ɂ́AEric Young �� (eay@cryptsoft.com) �ɂ���č쐬���ꂽ�Í��\�t�g�E�F�A���܂܂�Ă��܂��B<br>
			�@���̐��i�ɂ́ATim Hudson �� (tjh@cryptsoft.com) �ɂ���č쐬���ꂽ�\�t�g�E�F�A���܂܂�Ă��܂��B<br>
			<br>
			�@<b>�� ��L�̋L�ڂ�Windows�ŒʐM���C�u�����ł�OpenSSL�̗��p�ɔ����L�ڂł��B</b><br>
			�@�@�@<b>Nitro/TWL�ŒʐM���C�u�����A�y�т��̃T���v���v���O�����ɂ͈�؊֌W�͂���܂���B</b><br>
			<br>
			<br>
		@version	0.21<br>

*/

#ifndef ___POKE_NET_GDS___
#define ___POKE_NET_GDS___

#include "poke_net.h"


#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------*
					�^�E�萔�錾
 *-----------------------------------------------------------------------*/

//! �|�P���� WiFi GDS ���C�u�����̌��݂̏��
typedef enum
{
	POKE_NET_GDS_STATUS_INACTIVE,			//!< ��ғ���
	POKE_NET_GDS_STATUS_INITIALIZED,		//!< ��������
	POKE_NET_GDS_STATUS_REQUEST,			//!< ���N�G�X�g���s��
	POKE_NET_GDS_STATUS_NETSETTING,			//!< �l�b�g���[�N�ݒ蒆
	POKE_NET_GDS_STATUS_CONNECTING,			//!< �ڑ���
	POKE_NET_GDS_STATUS_SENDING,			//!< ���M��
	POKE_NET_GDS_STATUS_RECEIVING,			//!< ��M��
	POKE_NET_GDS_STATUS_ABORTED,			//!< ���f�I��
	POKE_NET_GDS_STATUS_FINISHED,			//!< ����I��
	POKE_NET_GDS_STATUS_ERROR,				//!< �G���[�I��
	POKE_NET_GDS_STATUS_COUNT
}
POKE_NET_GDS_STATUS;


//! �|�P���� WiFi GDS ���C�u�������G���[�I�������ۂ̏ڍ�
typedef enum
{
	POKE_NET_GDS_LASTERROR_NONE,			//!< ���ɂȂ�
	POKE_NET_GDS_LASTERROR_NOTINITIALIZED,	//!< ����������Ă��Ȃ�
	POKE_NET_GDS_LASTERROR_ILLEGALREQUEST,	//!< ���N�G�X�g���ُ�
	POKE_NET_GDS_LASTERROR_CREATESOCKET,	//!< �\�P�b�g�����Ɏ��s
	POKE_NET_GDS_LASTERROR_IOCTRLSOCKET,	//!< �\�P�b�g�̃m���u���b�L���O���Ɏ��s
	POKE_NET_GDS_LASTERROR_NETWORKSETTING,	//!< �l�b�g���[�N�ݒ莸�s
	POKE_NET_GDS_LASTERROR_CREATETHREAD,	//!< �X���b�h�������s
	POKE_NET_GDS_LASTERROR_CONNECT,			//!< �ڑ����s
	POKE_NET_GDS_LASTERROR_SENDREQUEST,		//!< ���N�G�X�g�G���[
	POKE_NET_GDS_LASTERROR_RECVRESPONSE,	//!< ���X�|���X�G���[
	POKE_NET_GDS_LASTERROR_CONNECTTIMEOUT,	//!< �ڑ��^�C���A�E�g
	POKE_NET_GDS_LASTERROR_SENDTIMEOUT,		//!< ���M�^�C���A�E�g
	POKE_NET_GDS_LASTERROR_RECVTIMEOUT,		//!< ��M�^�C���A�E�g
	POKE_NET_GDS_LASTERROR_ABORTED,			//!< ���f�I��
	POKE_NET_GDS_LASTERROR_GETSVL,			//!< SVL�擾�G���[
	POKE_NET_GDS_LASTERROR_COUNT
}
POKE_NET_GDS_LASTERROR;


/*-----------------------------------------------------------------------*
					�O���[�o���ϐ���`
 *-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*
					�֐��O���錾
 *-----------------------------------------------------------------------*/
extern BOOL POKE_NET_GDS_Initialize(POKE_NET_REQUESTCOMMON_AUTH *_auth, const char* _Url, unsigned short _PortNo);
extern void POKE_NET_GDS_Release();

extern POKE_NET_GDS_STATUS POKE_NET_GDS_GetStatus();
extern POKE_NET_GDS_LASTERROR POKE_NET_GDS_GetLastErrorCode();
extern BOOL POKE_NET_GDS_Abort();

extern void* POKE_NET_GDS_GetResponse();
extern long  POKE_NET_GDS_GetResponseSize();
extern long  POKE_NET_GDS_GetResponseMaxSize(long _reqno);

extern void POKE_NET_GDS_SetThreadLevel(unsigned long _level);


// ==============================================
// �ʐM�C���^�t�F�[�X
// ==============================================

// �~���[�W�J���V���b�g
// POKE_NET_GDS_MusicalShotRegist	: �A�b�v���[�h
// POKE_NET_GDS_MusicalShotGet		: �|�P�����I���_�E�����[�h
// (���� : poke_net_gds.c)
extern BOOL POKE_NET_GDS_MusicalShotRegist(MUSICAL_SHOT_SEND* _data, void* _response);
extern BOOL POKE_NET_GDS_MusicalShotGet(long _pokemonno, void* _response);

// �o�g���r�f�I
// POKE_NET_GDS_BattleDataRegist				: �A�b�v���[�h
// POKE_NET_GDS_BattleDataSearchCondition		: ���킵������
// POKE_NET_GDS_BattleDataSearchRankingLatest	: �����L���O���� >> �ŐV30��
// POKE_NET_GDS_BattleDataSearchRankingComm		: �����L���O���� >> �ʐM�ΐ탉���L���O
// POKE_NET_GDS_BattleDataSearchRankingSubway	: �����L���O���� >> �o�g���T�u�E�F�C�����L���O
// POKE_NET_GDS_BattleDataGet					: �_�E�����[�h
// POKE_NET_GDS_BattleDataFavorite				: ���C�ɓ���o�^
// (���� : poke_net_gds.c)
extern BOOL POKE_NET_GDS_BattleDataRegist(BATTLE_REC_SEND* _data, void* _response);
extern BOOL POKE_NET_GDS_BattleDataSearchCondition(BATTLE_REC_SEARCH_SEND* _condition, void* _response);
extern BOOL POKE_NET_GDS_BattleDataSearchRankingLatest(u32 _svrversion, void* _response);
extern BOOL POKE_NET_GDS_BattleDataSearchRankingComm(u32 _svrversion, void* _response);
extern BOOL POKE_NET_GDS_BattleDataSearchRankingSubway(u32 _svrversion, void* _response);
extern BOOL POKE_NET_GDS_BattleDataGet(u64 _code, u32 _serverVersion, void* _response);
extern BOOL POKE_NET_GDS_BattleDataFavorite(u64 _code, void* _response);

// �f�o�b�O���b�Z�[�W
// (���� : poke_net_gds.c)
extern BOOL POKE_NET_GDS_DebugMessageRequest(char* _message, void* _response);

// �_���v
// POKE_NET_GDS_DEBUG_PrintProfile		: �v���t�B�[�����e���o��
// POKE_NET_GDS_DEBUG_PrintProfile_Light: �v���t�B�[�����e���o��(�y��)
// POKE_NET_GDS_DEBUG_PrintMusicalshot	: �~���[�W�J���V���b�g���e���o��
// POKE_NET_GDS_DEBUG_PrintBattleHeader	: �o�g���w�b�_���e���o��
// POKE_NET_GDS_DEBUG_PrintBattleData	: �o�g���f�[�^���e���o��
// POKE_NET_GDS_DEBUG_PrintResponse		: ���X�|���X���e���o��
// (���� : poke_net_gds_debug.c)
extern void POKE_NET_GDS_DEBUG_PrintProfile(GDS_PROFILE* _profile);
extern void POKE_NET_GDS_DEBUG_PrintProfile_Light(GDS_PROFILE* _profile);
extern void POKE_NET_GDS_DEBUG_PrintMusicalshot(MUSICAL_SHOT_DATA* _pMusical);
extern void POKE_NET_GDS_DEBUG_PrintBattleHeader(BATTLE_REC_HEADER* _pBattleHeader);
extern void POKE_NET_GDS_DEBUG_PrintBattleData(BATTLE_REC_WORK* _pBattleData);
extern void POKE_NET_GDS_DEBUG_PrintResponse(POKE_NET_RESPONSE* _response, long _size);


#ifndef SDK_FINALROM

#else

#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // ___POKE_NET_GDS___
