//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		dwc_lobbylib.h
 *	@brief		���r�[�T�[�o�Ǘ��֐�
 *	@author		tomoya takahashi
 *	@data		2007.12.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __DWC_LOBBYLIB_H__
#define __DWC_LOBBYLIB_H__

#include <ppwlobby/ppw_lobby.h>

#include "savedata/save_control.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�`�����l���X�e�[�^�X
//=====================================
typedef enum {
    OLDDWC_LOBBY_CHANNEL_STATE_NONE,           // �`�����l���ɓ����Ă��Ȃ��B
    OLDDWC_LOBBY_CHANNEL_STATE_LOGINWAIT,		// �`�����l���ɓ������B
    OLDDWC_LOBBY_CHANNEL_STATE_CONNECT,		// �`�����l���ɓ����ς݁B
    OLDDWC_LOBBY_CHANNEL_STATE_LOGOUTWAIT,     // �`�����l���ɑގ����B
    OLDDWC_LOBBY_CHANNEL_STATE_ERROR           // �`�����l����Ԃ��擾�ł��܂���ł����B
} OLDDWC_LOBBY_CHANNEL_STATE;



//-------------------------------------
///	�������擾�萔
//=====================================
typedef enum {
	OLDDWC_LOBBY_ROOMDATA_LOCKTIME,		// �`�����l�������b�N���鎞��
	OLDDWC_LOBBY_ROOMDATA_RANDOM,			// �T�[�o���琶������郉���_���l
	OLDDWC_LOBBY_ROOMDATA_ROOMTYPE,		// �����̃^�C�v
	OLDDWC_LOBBY_ROOMDATA_SEASON,			// �G�ߔԍ�
	OLDDWC_LOBBY_ROOMDATA_ARCEUS,			// �A���Z�E�X�̕\���������邩 
	OLDDWC_LOBBY_ROOMDATA_CLOSETIME,		// �����̃��b�N����N���[�Y�܂ł̎���
	OLDDWC_LOBBY_ROOMDATA_FIRESTART,		// �����̃��b�N����ԉΊJ�n�܂ł̎���
	OLDDWC_LOBBY_ROOMDATA_FIREEND,			// �����̃��b�N����ԉΏI���܂ł̎���
	OLDDWC_LOBBY_ROOMDATA_MAX,				// �V�X�e�����Ŏg�p���܂�
} OLDDWC_LOBBY_ROOMDATA_TYPE;	

//-------------------------------------
///	�T�u�`�����l���^�C�v
//=====================================
typedef enum {
	OLDDWC_LOBBY_SUBCHAN_FOOT1,			// ���Ճ{�[�h�P
	OLDDWC_LOBBY_SUBCHAN_FOOT2,			// ���Ճ{�[�h�Q
	OLDDWC_LOBBY_SUBCHAN_CLOCK,			// ���E���v
	OLDDWC_LOBBY_SUBCHAN_NEWS,				// ���r�[�j���[�X
	OLDDWC_LOBBY_SUBCHAN_NONE,				// �V�X�e�����Ŏg�p���܂�
} OLDDWC_LOBBY_SUBCHAN_TYPE;

//-------------------------------------
///	�T�u�`�����l�����O�C���҂�
//=====================================
typedef enum {
	OLDDWC_LOBBY_SUBCHAN_LOGIN_WAIT,			// �T�u�`�����l���Ƀ��O�C���ҋ@��
	OLDDWC_LOBBY_SUBCHAN_LOGIN_OK,				// �T�u�`�����l���Ƀ��O�C������
	OLDDWC_LOBBY_SUBCHAN_LOGIN_NG,				// �T�u�`�����l���Ƀ��O�C�����s
} OLDDWC_LOBBY_SUBCHAN_LOGIN_RESULT;

//-------------------------------------
///	�~�j�Q�[���^�C�v
//=====================================
typedef enum {
	OLDDWC_LOBBY_MG_BALLSLOW,			// �ʂ���
	OLDDWC_LOBBY_MG_BALANCEBALL,		// �ʏ��
	OLDDWC_LOBBY_MG_BALLOON,			// ���D���
	OLDDWC_LOBBY_MG_NUM,				// �V�X�e�����Ŏg�p���܂�
} OLDDWC_LOBBY_MG_TYPE;


//-------------------------------------
///	�~�j�Q�[���G���g���[�߂�l
//=====================================
typedef enum {
	OLDDWC_LOBBY_MG_ENTRYNONE,			// �������Ă��Ȃ�
	OLDDWC_LOBBY_MG_ENTRYWAIT,			// �G���g���[��
	OLDDWC_LOBBY_MG_ENTRYOK,			// �G���g���[����
	OLDDWC_LOBBY_MG_ENTRYNG,			// �G���g���[���s
} OLDDWC_LOBBY_MG_ENTRYRET;

//-------------------------------------
///	���[���
//=====================================
typedef enum {
	OLDDWC_LOBBY_ANKETO_STATE_NONE,	// ���M�O	
	OLDDWC_LOBBY_ANKETO_STATE_SENDING,	// ���M��
	OLDDWC_LOBBY_ANKETO_STATE_SENDOK,	// ���M����
	OLDDWC_LOBBY_ANKETO_STATE_SENDNG,	// ���M���s
} OLDDWC_LOBBY_ANKETO_STATE;


//-------------------------------------
///	�A���P�[�g�f�[�^
//=====================================
typedef enum {
	OLDDWC_LOBBY_ANKETO_DATA_NOW_QUESTION_SERIAL,	// ���̎���ʂ��ԍ�
	OLDDWC_LOBBY_ANKETO_DATA_NOW_QUESTION_NO,		// ���̎���ԍ�
	OLDDWC_LOBBY_ANKETO_DATA_NOW_QUESTION_SPECIAL,	// ���̎��₪�X�y�V������
	OLDDWC_LOBBY_ANKETO_DATA_NOW_QUESTION_NINI,	// ���̎��₪�C�ӎ��₩
	
	OLDDWC_LOBBY_ANKETO_DATA_LAST_QUESTION_SERIAL,	// �O��̎���ʂ��ԍ�
	OLDDWC_LOBBY_ANKETO_DATA_LAST_QUESTION_NO,		// �O��̎���ԍ�
	OLDDWC_LOBBY_ANKETO_DATA_LAST_QUESTION_SPECIAL,// �O��̎��₪�X�y�V������
	OLDDWC_LOBBY_ANKETO_DATA_LAST_QUESTION_NINI,	// �O��̎��₪�C�ӎ��₩

	OLDDWC_LOBBY_ANKETO_DATA_LAST_RESULT_A,	// �O��̎��₪����A
	OLDDWC_LOBBY_ANKETO_DATA_LAST_RESULT_B,	// �O��̎��₪����B
	OLDDWC_LOBBY_ANKETO_DATA_LAST_RESULT_C,	// �O��̎��₪����C

	OLDDWC_LOBBY_ANKETO_DATA_MAX,	// DATA�ő吔
} OLDDWC_LOBBY_ANKETO_DATA;

//-------------------------------------
///	�A���P�[�g���b�Z�[�W���
//	�C�ӎ���
//=====================================
typedef enum {
	OLDDWC_LOBBY_ANKETO_MESSAGE_NOW_QUESTION,	// ���̖��
	OLDDWC_LOBBY_ANKETO_MESSAGE_NOW_ANSWER_00,	// ���̉�A
	OLDDWC_LOBBY_ANKETO_MESSAGE_NOW_ANSWER_01,	// ���̉�B
	OLDDWC_LOBBY_ANKETO_MESSAGE_NOW_ANSWER_02,	// ���̉�C

	OLDDWC_LOBBY_ANKETO_MESSAGE_LAST_QUESTION,		// �O��̖��
	OLDDWC_LOBBY_ANKETO_MESSAGE_LAST_ANSWER_00,	// �O��̉�A
	OLDDWC_LOBBY_ANKETO_MESSAGE_LAST_ANSWER_01,	// �O��̉�B
	OLDDWC_LOBBY_ANKETO_MESSAGE_LAST_ANSWER_02,	// �O��̉�C

	OLDDWC_LOBBY_ANKETO_MESSAGE_MAX,	// MESSAGE�ő吔
} OLDDWC_LOBBY_ANKETO_MESSAGE;

//-------------------------------------
///	�W�v����擾�p
//=====================================
typedef enum {
	OLDDWC_LOBBY_ANKETO_LANGUAGE_NOW,
	OLDDWC_LOBBY_ANKETO_LANGUAGE_LAST,
	OLDDWC_LOBBY_ANKETO_LANGUAGE_MAX,
} OLDDWC_LOBBY_ANKETO_LANGUAGE_DATA;



//-------------------------------------
///	�ގ��Ґ��E�n�}�f�[�^
//=====================================
#define DWC_WLDDATA_MAX		(20)	// �ő�i�[�n�搔


//-------------------------------------
///	���[�U��
//=====================================
#define OLDDWC_LOBBY_USERIDTBL_IDX_NONE	( 0xffffffff )		// ���[�UID����e�[�u���C���f�b�N�X�擾���s�߂�l


//-------------------------------------
///	�~�j�Q�[���^�C�����~�b�g
//=====================================
#define OLDDWC_LOBBY_MG_TIMELIMIT	15  //(60)


//-------------------------------------
///	VIP���
//=====================================
#define OLDDWC_LOBBY_VIP_BUFFNUM	( 128 )		// �o�b�t�@��
#define OLDDWC_LOBBY_VIP_KEYNONE	( 0 )		// �����t���Ȃ��Ƃ��̃L�[�l


//-------------------------------------
/// �A���P�[�g�@�萔
//=====================================
#define OLDDWC_LOBBY_ANKETO_NOT_QUESTION	( PPW_LOBBY_INVALID_QUESTION_NO )	// �����Ȏ���



//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/**
 *	@brief		�v���C���[�����R�[���o�b�N�֐��^
 *
 *	@param		userid		�`�����l���̃��[�UID
 *	@param		cp_profile	���̃��[�U�̃v���t�B�[��
 *	@param		p_work		��ƃ��[�N
 *	@param		mydata		�����̃f�[�^
 */
//=====================================
typedef void (*pDWC_LOBBY_UserInCallBack)( s32 userid, const void* cp_profile, void* p_work, BOOL mydata );

//-------------------------------------
/**
 *	@brief		�v���C���[�ގ��R�[���o�b�N�֐��^
 *
 *	@param		userid		�`�����l���̃��[�UID
 *	@param		p_work		��ƃ��[�N
 */
//=====================================
typedef void (*pDWC_LOBBY_UserOutCallBack)( s32 userid, void* p_work );

//-------------------------------------
/**
 *	@brief		�v���t�B�[���X�V�R�[���o�b�N�֐��^
 *
 *	@param		userid		�`�����l���̃��[�UID
 *	@param		cp_profile	���̃��[�U�̃v���t�B�[��
 *	@param		p_work		��ƃ��[�N
 */
//=====================================
typedef void (*pDWC_LOBBY_UserProfileUpDateCallBack)( s32 userid, const void* cp_profile, void* p_work );

//-------------------------------------
/**
 *	@brief		���ԃC�x���g�����R�[���o�b�N�֐��^
 *
 *	@param		event		�C�x���g���
 *	@param		p_work		��ƃ��[�N
 */
//=====================================
typedef void (*pDWC_LOBBY_EventCallBack)( PPW_LOBBY_TIME_EVENT event, void* p_work );

//-------------------------------------
/**
 *	@brief		�������s���`�F�b�N��v���t�B�[����M�R�[���o�b�N�֐��^
 *
 *	@param		cp_profile	�`�F�b�N��̃v���t�B�[��
 *	@param		p_work		��ƃ��[�N
 */
//=====================================
typedef void (*pDWC_LOBBY_CheckProfileCallBack)( const void* cp_profile, u32 profile_size, void* p_work );

//-------------------------------------
///	Wi-Fi���r�[�R�[���o�b�N�Z�b�g�\����
//=====================================
typedef struct {
	pDWC_LOBBY_UserInCallBack				p_user_in;			// �v���C���[����
	pDWC_LOBBY_UserOutCallBack				p_user_out;			// �v���C���[�ގ�
	pDWC_LOBBY_UserProfileUpDateCallBack	p_profile_update;	// �v���C���[�v���t�B�[���X�V
	pDWC_LOBBY_EventCallBack				p_event;			// �C�x���g����
	pDWC_LOBBY_CheckProfileCallBack			p_check_profile;	// �������s���`�F�b�N��v���t�B�[����M
} OLDDWC_LOBBY_CALLBACK;





//-------------------------------------
/**
 *	@brief		���b�Z�[�W��M�R�[���o�b�N�֐��^
 *
 *	@param		userid		�`�����l���̃��[�UID
 *	@param		cp_data		��M�f�[�^
 *	@param		size		�T�C�Y
 *	@param		p_work		���[�U��Ɨp���[�N
 */
//=====================================
typedef void (*pDWC_LOBBY_MsgDataRecvCallBack)( s32 userid, const void* cp_data, u32 size, void* p_work );
//-------------------------------------
///	���b�Z�[�W�f�[�^�R�}���h�f�[�^
//=====================================
typedef struct {
	pDWC_LOBBY_MsgDataRecvCallBack		p_func;			// ��M�R�[���o�b�N�֐�
	u32									size;			// �f�[�^�T�C�Y
} OLDDWC_LOBBY_MSGCOMMAND;






//-------------------------------------
///	�`�����l�������[�UID��
//=====================================
typedef struct {
	u32			num;
	const s32*	cp_tbl;
} OLDDWC_LOBBY_CHANNEL_USERID;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// ���C���`�����l��
extern void OLDDWC_LOBBY_Init( u32 heapID, SAVE_CONTROL_WORK* p_save, u32 profilesize, const OLDDWC_LOBBY_CALLBACK* cp_callback,  void* p_callbackwork );	// ���[�N�̏�����
extern void OLDDWC_LOBBY_Exit( void );												// ���[�N�̔j��
extern OLDDWC_LOBBY_CHANNEL_STATE OLDDWC_LOBBY_Update( void );						// ���r�[�̍X�V�֐�
extern PPW_LOBBY_ERROR OLDDWC_LOBBY_GetErr( void );								// ���r�[�G���[��Ԏ擾
extern s32 OLDDWC_LOBBY_GetErrNo( PPW_LOBBY_ERROR error );								// ���r�[�G���[��Ԏ擾
extern BOOL OLDDWC_LOBBY_Login( const void* cp_loginprofile );						// ���C���`�����l���փ��O�C��
extern BOOL OLDDWC_LOBBY_LoginEx( const void* cp_loginprofile, u32 prefix );			// ���C���`�����l���փ��O�C��	�����`�����l����I��
extern BOOL OLDDWC_LOBBY_LoginWait( void );										// ���C���`�����l�����O�C�������҂�
extern void OLDDWC_LOBBY_Logout( void );										// ���C���`�����l�����O�A�E�g
extern BOOL OLDDWC_LOBBY_LogoutWait( void );										// ���C���`�����l�����O�A�E�g�����҂�
extern OLDDWC_LOBBY_CHANNEL_STATE OLDDWC_LOBBY_GetState( void );						// ���C���`�����l���̏��
extern BOOL OLDDWC_LOBBY_CheckExcessFlood( void );	// �ʐM�ߏ��Ԏ擾	�擾��t���O��OFF�ɂ��܂�
extern BOOL OLDDWC_LOBBY_CheckProfileError( void );							// �v���t�B�[�����s���`�F�b�N

// �v���t�B�[���f�[�^
extern s32 OLDDWC_LOBBY_GetMyUserID( void );							// �����̃��[�UID
extern void OLDDWC_LOBBY_SetMyProfile( const void* cp_data );			// �����̃v���t�B�[�����X�V
extern const void* OLDDWC_LOBBY_GetMyProfile( void );					// �����̃v���t�B�[�����擾
extern const void* OLDDWC_LOBBY_GetUserProfile( s32 userid );			// �l�̃v���t�B�[�����擾
extern void OLDDWC_LOBBY_GetLoginTime( s32 userid, s64* p_time );		// ���O�C���������Ԃ��擾
extern s32	OLDDWC_LOBBY_GetOldUser( void );							// ��Ԑ̂��炢��v���C���[���擾
extern s32	OLDDWC_LOBBY_GetOldUser_UserDesc( s32 desc_user );			// �ؒf�����l�ȊO�ň�Ԑ̂��炢��v���C���[���擾

// �T�u�`�����l��ROOM
extern BOOL OLDDWC_LOBBY_SUBCHAN_Login( OLDDWC_LOBBY_SUBCHAN_TYPE channel );			// �T�u�`�����l���փ��O�C��
extern OLDDWC_LOBBY_SUBCHAN_LOGIN_RESULT OLDDWC_LOBBY_SUBCHAN_LoginWait( void );								// �T�u�`�����l���փ��O�C���҂�
extern BOOL OLDDWC_LOBBY_SUBCHAN_Logout( void );									// �T�u�`�����l�����O�A�E�g
extern BOOL OLDDWC_LOBBY_SUBCHAN_LogoutWait( void );								// �T�u�`�����l�����O�A�E�g�҂�
extern OLDDWC_LOBBY_CHANNEL_STATE OLDDWC_LOBBY_SUBCHAN_GetState( void );				// �T�u�`�����l���̏��
extern BOOL OLDDWC_LOBBY_SUBCHAN_LoginCheck( OLDDWC_LOBBY_SUBCHAN_TYPE channel );						// �T�u�`�����l���̐l�����`�F�b�N���ē�����Ԃ��`�F�b�N

// ���[�UID�񋓎擾
// �`�����l�����ɒN������̂��킩��
extern void OLDDWC_LOBBY_GetUserIDTbl( OLDDWC_LOBBY_CHANNEL_USERID* p_data );			// ���C���`�����l�������[�UID�񋓎擾
extern void OLDDWC_LOBBY_SUBCHAN_GetUserIDTbl( OLDDWC_LOBBY_CHANNEL_USERID* p_data );	// �T�u�`�����l�������[�UID�񋓎擾
extern u32	OLDDWC_LOBBY_GetUserIDIdx( s32 userid );								// ���C���`�����l�����̃��[�UID����񋓓���INDEX�����߂�
extern u32	OLDDWC_LOBBY_SUBCHAN_GetUserIDIdx( s32 userid );						// �T�u�`�����l�����̃��[�UID����񋓓���INDEX�����߂�
extern s32	OLDDWC_LOBBY_GetUserIdxID( u32 idx );								// ���C���`�����l������INDEX���烆�[�UID�����߂�
extern s32	OLDDWC_LOBBY_SUBCHAN_GetUserIdxID( u32 idx );						// �T�u�`�����l������INDEX���烆�[�UID�����߂�

// ���r�[�f�[�^
extern void OLDDWC_LOBBY_GetTime( s64* p_time );		// ���݂�GMT
extern void OLDDWC_LOBBY_GetOpenTime( s64* p_time );	// �������쐬��������
extern void OLDDWC_LOBBY_GetLockTime( s64* p_time );	// ���������b�N��������
extern u32 OLDDWC_LOBBY_GetRoomData( OLDDWC_LOBBY_ROOMDATA_TYPE type );	// �e�����擾


// �ގ��҂̍��f�[�^
extern BOOL OLDDWC_LOBBY_WLDDATA_Send( void );
extern BOOL OLDDWC_LOBBY_WLDDATA_SendWait( void );
extern u16 OLDDWC_LOBBY_WLDDATA_GetNation( u8 index );		// �ގ��҂̍�ID
extern u8 OLDDWC_LOBBY_WLDDATA_GetArea( u8 index );		// �ގ��҂̒n��ID
extern BOOL OLDDWC_LOBBY_WLDDATA_GetDataFlag( u8 index );	// �ގ��ҍ��f�[�^�����邩


// ���b�Z�[�W����M
// LOBBY_�����Ȃ�		���C���`�����l���p
// LOBBY_SUBCHAN		�T�u�`�����l���p
//						�i���Ճ{�[�h�A���E���v�A�j���[�X�̂Ȃ��ō�����������`�����l���j
extern void OLDDWC_LOBBY_SetMsgCmd( const OLDDWC_LOBBY_MSGCOMMAND* cp_tbl, u32 tblnum, void* p_work );			// �R�}���h�̐ݒ�
extern void OLDDWC_LOBBY_SUBCHAN_SetMsgCmd( const OLDDWC_LOBBY_MSGCOMMAND* cp_tbl, u32 tblnum, void* p_work );
extern void OLDDWC_LOBBY_CleanMsgCmd( void );															// �R�}���h�e�[�u����j��
extern void OLDDWC_LOBBY_SUBCHAN_CleanMsgCmd( void );
extern void OLDDWC_LOBBY_SendChannelMsg( u32 type, const void* cp_data, u32 size );					// �`�����l���S�̂ɑ��M
extern void OLDDWC_LOBBY_SendPlayerMsg( u32 type, s32 userid, const void* cp_data, u32 size );						// �l�ɑ΂��đ��M
extern void OLDDWC_LOBBY_SUBCHAN_SendMsg( u32 type, const void* cp_data, u32 size );					// �T�u�`�����l���̓`�����l���S�̂̑��M�����Ȃ�

// �~�j�Q�[����W
// ���̊֐��̃A�N�Z�X��COMM_STATE���s���܂��B
// P2P�֌W�̏��̎擾��COMM_�`�֐��ōs���Ă��������B
extern BOOL OLDDWC_LOBBY_MG_StartRecruit( OLDDWC_LOBBY_MG_TYPE type, u32 maxnum );	// ��W�J�n
extern void OLDDWC_LOBBY_MG_EndRecruit( void );					// ��W�I��
extern void OLDDWC_LOBBY_MG_StartGame( void );						// ���̏�ԂŃQ�[�����J�n������
extern BOOL OLDDWC_LOBBY_MG_CheckStartGame( void );				// �Q�����Ă���Q�[�����J�n���邩�`�F�b�N
extern void OLDDWC_LOBBY_MG_EndConnect( void );					// P2P�ʐM�̏I��
extern BOOL OLDDWC_LOBBY_MG_ForceEnd( void );						// ��W���I������K�v�����邩
extern BOOL OLDDWC_LOBBY_MG_CheckRecruit( OLDDWC_LOBBY_MG_TYPE type );// ��W���̎擾
extern BOOL OLDDWC_LOBBY_MG_CheckEntryOk( OLDDWC_LOBBY_MG_TYPE type );// �G���g���[�ł��邩�`�F�b�N
extern u32 OLDDWC_LOBBY_MG_GetRest( OLDDWC_LOBBY_MG_TYPE type );		// ���Ɖ��l�͂���邩
extern u32 OLDDWC_LOBBY_MG_GetEntryNum( OLDDWC_LOBBY_MG_TYPE type );	// ���l��W�ɎQ�����Ă��邩
extern BOOL OLDDWC_LOBBY_MG_Entry( OLDDWC_LOBBY_MG_TYPE type );		// ��W�ɎQ��
extern s32 OLDDWC_LOBBY_MG_GetTimeLimit( OLDDWC_LOBBY_MG_TYPE type );	// ��W�̐������Ԃ��擾����
extern OLDDWC_LOBBY_MG_ENTRYRET OLDDWC_LOBBY_MG_EntryWait( void );	// ��W�Q�������҂�
extern OLDDWC_LOBBY_MG_TYPE OLDDWC_LOBBY_MG_GetConnectType( void );	// �ڑ����̃~�j�Q�[�����擾
extern s32 OLDDWC_LOBBY_MG_GetParentUserID( void );				// �e��userid���擾
extern BOOL OLDDWC_LOBBY_MG_MyParent( void );						// �������e���`�F�b�N
extern BOOL OLDDWC_LOBBY_MG_CheckConnect( void );					// �N����P2P��Ԃɂ��邩�`�F�b�N

// VIP�f�[�^�擾
extern BOOL OLDDWC_LOBBY_VIP_CheckVip( s32 userid );
extern s32 OLDDWC_LOBBY_VIP_GetAikotobaKey( s32 userid );


// �A���P�[�g�f�[�^
extern void OLDDWC_LOBBY_ANKETO_SubMit( s32 select );					// ���[���M
extern OLDDWC_LOBBY_ANKETO_STATE OLDDWC_LOBBY_ANKETO_WaitSubMit( void );	// ���[�����҂�
extern s32 OLDDWC_LOBBY_ANKETO_GetData( OLDDWC_LOBBY_ANKETO_DATA type );	// �A���P�[�g�f�[�^�擾
extern u16* OLDDWC_LOBBY_ANKETO_GetMessage( OLDDWC_LOBBY_ANKETO_MESSAGE type );	// �A���P�[�g���b�Z�[�W�擾
extern BOOL OLDDWC_LOBBY_ANKETO_GetLanguageSummarize( OLDDWC_LOBBY_ANKETO_LANGUAGE_DATA type, u32 lang );


#ifdef PM_DEBUG
// �f�o�b�N�p�X�P�W���[���ݒ�
extern void OLDDWC_LOBBY_DEBUG_SetRoomData( u32 locktime, u32 random, u8 roomtype, u8 season );
extern void OLDDWC_LOBBY_DEBUG_PlayerIN( const void* cp_data, s32 userid );			// ���̐l�����邱�Ƃɂ���
extern void OLDDWC_LOBBY_DEBUG_SetProfile( const void* cp_data, u32 userid );			// ���̐l�����邱�Ƃɂ���

#endif

#endif		// __DWC_LOBBYLIB_H__

