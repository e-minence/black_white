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

#include "savedata/savedata_def.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WiFi���r�[�Q�[�����
//	�����DP�Ɠ��l
//=====================================
#define DWC_LOBBY_GAMENAME			("pokemondpds")					// �Q�[����
#define DWC_LOBBY_SECRET_KEY		("1vTlwb")					// �V�[�N���b�g�L�[


//-------------------------------------
///	�`�����l���X�e�[�^�X
//=====================================
typedef enum {
    DWC_LOBBY_CHANNEL_STATE_NONE,           // �`�����l���ɓ����Ă��Ȃ��B
    DWC_LOBBY_CHANNEL_STATE_LOGINWAIT,		// �`�����l���ɓ������B
    DWC_LOBBY_CHANNEL_STATE_CONNECT,		// �`�����l���ɓ����ς݁B
    DWC_LOBBY_CHANNEL_STATE_LOGOUTWAIT,     // �`�����l���ɑގ����B
    DWC_LOBBY_CHANNEL_STATE_ERROR           // �`�����l����Ԃ��擾�ł��܂���ł����B
} DWC_LOBBY_CHANNEL_STATE;



//-------------------------------------
///	�������擾�萔
//=====================================
typedef enum {
	DWC_LOBBY_ROOMDATA_LOCKTIME,		// �`�����l�������b�N���鎞��
	DWC_LOBBY_ROOMDATA_RANDOM,			// �T�[�o���琶������郉���_���l
	DWC_LOBBY_ROOMDATA_ROOMTYPE,		// �����̃^�C�v
	DWC_LOBBY_ROOMDATA_SEASON,			// �G�ߔԍ�
	DWC_LOBBY_ROOMDATA_ARCEUS,			// �A���Z�E�X�̕\���������邩 
	DWC_LOBBY_ROOMDATA_CLOSETIME,		// �����̃��b�N����N���[�Y�܂ł̎���
	DWC_LOBBY_ROOMDATA_FIRESTART,		// �����̃��b�N����ԉΊJ�n�܂ł̎���
	DWC_LOBBY_ROOMDATA_FIREEND,			// �����̃��b�N����ԉΏI���܂ł̎���
	DWC_LOBBY_ROOMDATA_MAX,				// �V�X�e�����Ŏg�p���܂�
} DWC_LOBBY_ROOMDATA_TYPE;	

//-------------------------------------
///	�T�u�`�����l���^�C�v
//=====================================
typedef enum {
	DWC_LOBBY_SUBCHAN_FOOT1,			// ���Ճ{�[�h�P
	DWC_LOBBY_SUBCHAN_FOOT2,			// ���Ճ{�[�h�Q
	DWC_LOBBY_SUBCHAN_CLOCK,			// ���E���v
	DWC_LOBBY_SUBCHAN_NEWS,				// ���r�[�j���[�X
	DWC_LOBBY_SUBCHAN_NONE,				// �V�X�e�����Ŏg�p���܂�
} DWC_LOBBY_SUBCHAN_TYPE;

//-------------------------------------
///	�T�u�`�����l�����O�C���҂�
//=====================================
typedef enum {
	DWC_LOBBY_SUBCHAN_LOGIN_WAIT,			// �T�u�`�����l���Ƀ��O�C���ҋ@��
	DWC_LOBBY_SUBCHAN_LOGIN_OK,				// �T�u�`�����l���Ƀ��O�C������
	DWC_LOBBY_SUBCHAN_LOGIN_NG,				// �T�u�`�����l���Ƀ��O�C�����s
} DWC_LOBBY_SUBCHAN_LOGIN_RESULT;

//-------------------------------------
///	�~�j�Q�[���^�C�v
//=====================================
typedef enum {
	DWC_LOBBY_MG_BALLSLOW,			// �ʂ���
	DWC_LOBBY_MG_BALANCEBALL,		// �ʏ��
	DWC_LOBBY_MG_BALLOON,			// ���D���
	DWC_LOBBY_MG_NUM,				// �V�X�e�����Ŏg�p���܂�
} DWC_LOBBY_MG_TYPE;


//-------------------------------------
///	�~�j�Q�[���G���g���[�߂�l
//=====================================
typedef enum {
	DWC_LOBBY_MG_ENTRYNONE,			// �������Ă��Ȃ�
	DWC_LOBBY_MG_ENTRYWAIT,			// �G���g���[��
	DWC_LOBBY_MG_ENTRYOK,			// �G���g���[����
	DWC_LOBBY_MG_ENTRYNG,			// �G���g���[���s
} DWC_LOBBY_MG_ENTRYRET;




//-------------------------------------
///	�ގ��Ґ��E�n�}�f�[�^
//=====================================
#define DWC_WLDDATA_MAX		(20)	// �ő�i�[�n�搔


//-------------------------------------
///	���[�U��
//=====================================
#define DWC_LOBBY_USERIDTBL_IDX_NONE	( 0xffffffff )		// ���[�UID����e�[�u���C���f�b�N�X�擾���s�߂�l


//-------------------------------------
///	�~�j�Q�[���^�C�����~�b�g
//=====================================
#define DWC_LOBBY_MG_TIMELIMIT	(60)



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
///	Wi-Fi���r�[�R�[���o�b�N�Z�b�g�\����
//=====================================
typedef struct {
	pDWC_LOBBY_UserInCallBack				p_user_in;			// �v���C���[����
	pDWC_LOBBY_UserOutCallBack				p_user_out;			// �v���C���[�ގ�
	pDWC_LOBBY_UserProfileUpDateCallBack	p_profile_update;	// �v���C���[�v���t�B�[���X�V
	pDWC_LOBBY_EventCallBack				p_event;			// �C�x���g����
} DWC_LOBBY_CALLBACK;





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
} DWC_LOBBY_MSGCOMMAND;






//-------------------------------------
///	�`�����l�������[�UID��
//=====================================
typedef struct {
	u32			num;
	const s32*	cp_tbl;
} DWC_LOBBY_CHANNEL_USERID;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// ���C���`�����l��
extern void DWC_LOBBY_Init( u32 heapID, SAVEDATA* p_save, u32 profilesize, const DWC_LOBBY_CALLBACK* cp_callback,  void* p_callbackwork );	// ���[�N�̏�����
extern void DWC_LOBBY_Exit( void );												// ���[�N�̔j��
extern DWC_LOBBY_CHANNEL_STATE DWC_LOBBY_Update( void );						// ���r�[�̍X�V�֐�
extern PPW_LOBBY_ERROR DWC_LOBBY_GetErr( void );								// ���r�[�G���[��Ԏ擾
extern BOOL DWC_LOBBY_Login( const void* cp_loginprofile );						// ���C���`�����l���փ��O�C��
extern BOOL DWC_LOBBY_LoginEx( const void* cp_loginprofile, u32 prefix );			// ���C���`�����l���փ��O�C��	�����`�����l����I��
extern BOOL DWC_LOBBY_LoginWait( void );										// ���C���`�����l�����O�C�������҂�
extern void DWC_LOBBY_Logout( void );										// ���C���`�����l�����O�A�E�g
extern BOOL DWC_LOBBY_LogoutWait( void );										// ���C���`�����l�����O�A�E�g�����҂�
extern DWC_LOBBY_CHANNEL_STATE DWC_LOBBY_GetState( void );						// ���C���`�����l���̏��

// �v���t�B�[���f�[�^
extern s32 DWC_LOBBY_GetMyUserID( void );							// �����̃��[�UID
extern void DWC_LOBBY_SetMyProfile( const void* cp_data );			// �����̃v���t�B�[�����X�V
extern const void* DWC_LOBBY_GetMyProfile( void );					// �����̃v���t�B�[�����擾
extern const void* DWC_LOBBY_GetUserProfile( s32 userid );			// �l�̃v���t�B�[�����擾
extern void DWC_LOBBY_GetLoginTime( s32 userid, s64* p_time );		// ���O�C���������Ԃ��擾
extern s32	DWC_LOBBY_GetOldUser( void );							// ��Ԑ̂��炢��v���C���[���擾
extern s32	DWC_LOBBY_GetOldUser_UserDesc( s32 desc_user );			// �ؒf�����l�ȊO�ň�Ԑ̂��炢��v���C���[���擾

// �T�u�`�����l��ROOM
extern BOOL DWC_LOBBY_SUBCHAN_Login( DWC_LOBBY_SUBCHAN_TYPE channel );			// �T�u�`�����l���փ��O�C��
extern DWC_LOBBY_SUBCHAN_LOGIN_RESULT DWC_LOBBY_SUBCHAN_LoginWait( void );								// �T�u�`�����l���փ��O�C���҂�
extern BOOL DWC_LOBBY_SUBCHAN_Logout( void );									// �T�u�`�����l�����O�A�E�g
extern BOOL DWC_LOBBY_SUBCHAN_LogoutWait( void );								// �T�u�`�����l�����O�A�E�g�҂�
extern DWC_LOBBY_CHANNEL_STATE DWC_LOBBY_SUBCHAN_GetState( void );				// �T�u�`�����l���̏��

// ���[�UID�񋓎擾
// �`�����l�����ɒN������̂��킩��
extern void DWC_LOBBY_GetUserIDTbl( DWC_LOBBY_CHANNEL_USERID* p_data );			// ���C���`�����l�������[�UID�񋓎擾
extern void DWC_LOBBY_SUBCHAN_GetUserIDTbl( DWC_LOBBY_CHANNEL_USERID* p_data );	// �T�u�`�����l�������[�UID�񋓎擾
extern u32	DWC_LOBBY_GetUserIDIdx( s32 userid );								// ���C���`�����l�����̃��[�UID����񋓓���INDEX�����߂�
extern u32	DWC_LOBBY_SUBCHAN_GetUserIDIdx( s32 userid );						// �T�u�`�����l�����̃��[�UID����񋓓���INDEX�����߂�
extern s32	DWC_LOBBY_GetUserIdxID( u32 idx );								// ���C���`�����l������INDEX���烆�[�UID�����߂�
extern s32	DWC_LOBBY_SUBCHAN_GetUserIdxID( u32 idx );						// �T�u�`�����l������INDEX���烆�[�UID�����߂�

// ���r�[�f�[�^
extern void DWC_LOBBY_GetTime( s64* p_time );		// ���݂�GMT
extern void DWC_LOBBY_GetOpenTime( s64* p_time );	// �������쐬��������
extern void DWC_LOBBY_GetLockTime( s64* p_time );	// ���������b�N��������
extern u32 DWC_LOBBY_GetRoomData( DWC_LOBBY_ROOMDATA_TYPE type );	// �e�����擾


// �ގ��҂̍��f�[�^
extern BOOL DWC_LOBBY_WLDDATA_Send( void );
extern BOOL DWC_LOBBY_WLDDATA_SendWait( void );
extern u16 DWC_LOBBY_WLDDATA_GetNation( u8 index );		// �ގ��҂̍�ID
extern u8 DWC_LOBBY_WLDDATA_GetArea( u8 index );		// �ގ��҂̒n��ID
extern BOOL DWC_LOBBY_WLDDATA_GetDataFlag( u8 index );	// �ގ��ҍ��f�[�^�����邩


// ���b�Z�[�W����M
// LOBBY_�����Ȃ�		���C���`�����l���p
// LOBBY_SUBCHAN		�T�u�`�����l���p
//						�i���Ճ{�[�h�A���E���v�A�j���[�X�̂Ȃ��ō�����������`�����l���j
extern void DWC_LOBBY_SetMsgCmd( const DWC_LOBBY_MSGCOMMAND* cp_tbl, u32 tblnum, void* p_work );			// �R�}���h�̐ݒ�
extern void DWC_LOBBY_SUBCHAN_SetMsgCmd( const DWC_LOBBY_MSGCOMMAND* cp_tbl, u32 tblnum, void* p_work );
extern void DWC_LOBBY_CleanMsgCmd( void );															// �R�}���h�e�[�u����j��
extern void DWC_LOBBY_SUBCHAN_CleanMsgCmd( void );
extern void DWC_LOBBY_SendChannelMsg( u32 type, const void* cp_data, u32 size );					// �`�����l���S�̂ɑ��M
extern void DWC_LOBBY_SendPlayerMsg( u32 type, s32 userid, const void* cp_data, u32 size );						// �l�ɑ΂��đ��M
extern void DWC_LOBBY_SUBCHAN_SendMsg( u32 type, const void* cp_data, u32 size );					// �T�u�`�����l���̓`�����l���S�̂̑��M�����Ȃ�

// �~�j�Q�[����W
// ���̊֐��̃A�N�Z�X��COMM_STATE���s���܂��B
// P2P�֌W�̏��̎擾��COMM_�`�֐��ōs���Ă��������B
extern BOOL DWC_LOBBY_MG_StartRecruit( DWC_LOBBY_MG_TYPE type, u32 maxnum );	// ��W�J�n
extern void DWC_LOBBY_MG_EndRecruit( void );					// ��W�I��
extern void DWC_LOBBY_MG_StartGame( void );						// ���̏�ԂŃQ�[�����J�n������
extern BOOL DWC_LOBBY_MG_CheckStartGame( void );				// �Q�����Ă���Q�[�����J�n���邩�`�F�b�N
extern void DWC_LOBBY_MG_EndConnect( void );					// P2P�ʐM�̏I��
extern BOOL DWC_LOBBY_MG_ForceEnd( void );						// ��W���I������K�v�����邩
extern BOOL DWC_LOBBY_MG_CheckRecruit( DWC_LOBBY_MG_TYPE type );// ��W���̎擾
extern BOOL DWC_LOBBY_MG_CheckEntryOk( DWC_LOBBY_MG_TYPE type );// �G���g���[�ł��邩�`�F�b�N
extern u32 DWC_LOBBY_MG_GetRest( DWC_LOBBY_MG_TYPE type );		// ���Ɖ��l�͂���邩
extern u32 DWC_LOBBY_MG_GetEntryNum( DWC_LOBBY_MG_TYPE type );	// ���l��W�ɎQ�����Ă��邩
extern BOOL DWC_LOBBY_MG_Entry( DWC_LOBBY_MG_TYPE type );		// ��W�ɎQ��
extern s32 DWC_LOBBY_MG_GetTimeLimit( DWC_LOBBY_MG_TYPE type );	// ��W�̐������Ԃ��擾����
extern DWC_LOBBY_MG_ENTRYRET DWC_LOBBY_MG_EntryWait( void );	// ��W�Q�������҂�
extern DWC_LOBBY_MG_TYPE DWC_LOBBY_MG_GetConnectType( void );	// �ڑ����̃~�j�Q�[�����擾
extern s32 DWC_LOBBY_MG_GetParentUserID( void );				// �e��userid���擾
extern BOOL DWC_LOBBY_MG_MyParent( void );						// �������e���`�F�b�N
extern BOOL DWC_LOBBY_MG_CheckConnect( void );					// �N����P2P��Ԃɂ��邩�`�F�b�N


#ifdef PM_DEBUG
// �f�o�b�N�p�X�P�W���[���ݒ�
extern void DWC_LOBBY_DEBUG_SetRoomData( u32 locktime, u32 random, u8 roomtype, u8 season );
extern void DWC_LOBBY_DEBUG_PlayerIN( const void* cp_data, s32 userid );			// ���̐l�����邱�Ƃɂ���
extern void DWC_LOBBY_DEBUG_SetProfile( const void* cp_data, u32 userid );			// ���̐l�����邱�Ƃɂ���

#endif

#endif		// __DWC_LOBBYLIB_H__

