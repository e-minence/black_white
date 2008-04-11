//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_ev_mg.c
 *	@brief		�~�j�Q�[���C�x���g
 *	@author		tomoya takahashi
 *	@data		2008.01.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/msgdata.h"
#include "system/fontproc.h"

#include "communication/communication.h"

#include "msgdata/msg_wifi_h_info.h"

#include "wflby_ev_mg.h"
#include "wflby_snd.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG
//#define	WFLBY_EV_MG_DEBUG_MATCHING_STATE	// �}�b�`���O�X�e�[�^�X�\��
#endif

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�~�j�Q�[���Q���V�[�P���X
//=====================================
enum {
	WFLBY_EV_MG_INIT,
	WFLBY_EV_MG_ENTRY_CHK,		// �Q���ł��邩�`�F�b�N
	WFLBY_EV_MG_ENTRY_SEL,		// �Q�����邩�I��
	WFLBY_EV_MG_ENTRY_SELWAIT,	// �Q�����邩�I��҂�
	WFLBY_EV_MG_MATCHSTART,		// �}�b�`���O�J�n
	WFLBY_EV_MG_MATCHWAIT,		// �}�b�`���O�E�G�C�g
	WFLBY_EV_MG_MATCHOKWAIT,	// �^�C�����~�b�g���z������Ƀ}�b�`���O�����҂�
	WFLBY_EV_MG_INFO_INIT,		// CommInfo�̏�����	
	WFLBY_EV_MG_INFO_POKE,		// CommInfo��PokeData���M
	WFLBY_EV_MG_STARTWAIT,		// �Q�[���J�n�҂�
	WFLBY_EV_MG_PLIDX_RESET,	// PLAYER_IDX������
	WFLBY_EV_MG_PLIDX_SEND,		// PLAYER_IDX���M
	WFLBY_EV_MG_PLIDX_CHECK,	// PLAYER_IDX�`�F�b�N
	WFLBY_EV_MG_COMM_START,		// �ʐM�J�n
	WFLBY_EV_MG_CANCEL_MSG,		// CANCEL�����`�F�b�N
	WFLBY_EV_MG_CANCEL_CHECK,	// CANCEL�����`�F�b�N
	WFLBY_EV_MG_CANCEL_WAIT,	// CANCEL�����`�F�b�N
	WFLBY_EV_MG_CANCEL_MW,		// CANCEL�����`�F�b�N	MATCHWAIT�ɖ߂�
	WFLBY_EV_MG_CANCEL_ININ,	// CANCEL�����`�F�b�N	INFO_INIT�ɍs��
	WFLBY_EV_MG_ERREND,			// �ʐM�ؒf�I��
	WFLBY_EV_MG_ERRENDWAIT,		// �ʐM�ؒf�I���҂�
	WFLBY_EV_MG_MSG_WAIT,		// ���b�Z�[�W�I���҂�
	WFLBY_EV_MG_MSG_SYNC,		// ���b�Z�[�W�I���҂���������
	WFLBY_EV_MG_MSG_SYNCWAIT,	// ���b�Z�[�W�I���҂������҂�
	WFLBY_EV_MG_END,
} ;

//-------------------------------------
///	�~�j�Q�[���ʐM�ؒf�V�[�P���X
//=====================================
enum {
	WFLBY_EV_MG_END_REQ,
	WFLBY_EV_MG_END_WAIT,
} ;


#define _TIMING_GAME_CHECK			(13)// �Ȃ���������
#define _TIMING_GAME_CHECK2			(14)// �Ȃ���������
#define _TIMING_GAME_START			(15)// �^�C�~���O�����낦��
#define _TIMING_GAME_START2			(18)// �^�C�~���O�����낦��
#define _TIMING_POKEDATA_SEND		(17)// �^�C�~���O�����낦��

#define WFLBY_EV_MG_SYNC_RETRANS_TIMING	(5*30)	// �ʐl�����R�}���h���ē]������^�C�~���O


//-------------------------------------
///	�~�j�Q�[���J���o���`�惂�[�h
//=====================================
enum {
	WFLBY_EV_MG_KANBAN_MODE_NORMAL,		// �ʏ�
	WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN,	// �J�E���g�_�E���@�\��
	WFLBY_EV_MG_KANBAN_MODE_NUM,
} ;
enum{
	WFLBY_EV_MG_KANBAN_DRAW_MSK_TITLE		= 1 << 0,
	WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAYERNUM	= 1 << 1,
	WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAY		= 1 << 2,
	WFLBY_EV_MG_KANBAN_DRAW_MSK_COUNT		= 1 << 3,	
};
#define WFLBY_EV_MG_KANBAN_DRAW_MSK_ALL	( 0xff )




//-------------------------------------
///	�v���C���[�~�j�Q�[���f����
//=====================================
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_X				( 18 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_Y				( 3 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX			( 13 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZY			( 8 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_SIZY	( 10 )

#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_X	( 0 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_Y	( 0 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_X	( 36 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_Y	( 16 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_X	( 0 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_Y	( 32 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_X	( 0 )
#define WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_Y	( 64 )


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	�J���o�����[�N
//=====================================
typedef struct {
	u8	recruit;
	u8	num;
	u8	play;
	u8	minigame_type;
	s16 number;
	u8	mode;
	u8	write;
} WFLBY_EV_MG_KANBANWK;



//-------------------------------------
///	�~�j�Q�[���Q���C�x���g���[�N
//=====================================
typedef struct {
	s16 timeout;
	u16 start_game_set;
	u8  msg_ret_seq;
	s8	msg_wait;
	u8	msg_sync;
	u8	last_entry_num;
	u16 match_entry_num;
	u16 sync_count;	// ���������ɂǂ̂��炢�������Ă��邩

	s32 tmp_userid;

	// �r�b�g�}�b�v���X�g��p
	BMPLIST_HEADER		bmplist;
	u32					bmplist_num;	// �r�b�g�}�b�v���X�g��

	// �J���o�����[�N
	WFLBY_EV_MG_KANBANWK	kanban;
} WFLBY_EV_MG_WK;



//-------------------------------------
///	�v���C���[�ŔC�x���g���[�N
//=====================================
typedef struct {
	WFLBY_EV_MG_KANBANWK data;
} WFLBY_EV_DEF_PLAYER_KANBANWK;


//-----------------------------------------------------------------------------
/**
 *		���b�Z�[�W���X�g�f�[�^
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	��{�w�b�_�[
//=====================================
static const BMPLIST_HEADER sc_WFLBY_EV_MG_HEADER = {
	NULL,
	NULL,
	NULL,
	NULL,

	0,	//���X�g���ڐ�
	2,	//�\���ő區�ڐ�
	0,								//���x���\���w���W
	8,								//���ڕ\���w���W
	0,								//�J�[�\���\���w���W
	0,								//�\���x���W
	FBMP_COL_BLACK,					//�\�������F
	FBMP_COL_WHITE,					//�\���w�i�F
	FBMP_COL_BLK_SDW,				//�\�������e�F
	0,								//�����Ԋu�w
	16,								//�����Ԋu�x
	BMPLIST_NO_SKIP,				//�y�[�W�X�L�b�v�^�C�v
	FONT_SYSTEM,					//�����w��(�{���� u8 �����ǁA����Ȃɍ��Ȃ��Ǝv���̂�)
	0,								//�a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)

	NULL
};
//-------------------------------------
///	�Q���I�����X�g
//=====================================
enum {
	WFLBY_EV_MG_SELLIST_ENTRY,	// �Q������
	WFLBY_EV_MG_SELLIST_NO,		// �Q�����Ȃ�
	WFLBY_EV_MG_SELLIST_NUM,	// �Q���l��
} ;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MsgPrint( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, WFLBY_ROOMWK* p_rmwk, u32 msgidx, u32 retseq );
static void WFLBY_EV_MG_MsgPrintSync( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, WFLBY_ROOMWK* p_rmwk, u32 msgidx, u32 retseq, u32 syncno, u32 msgwait );
static void WFLBY_EV_MG_Sync( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, u32 retseq, u32 syncno );
static void WFLBY_EV_MG_WK_InitBmpList( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk, u32 num, u32 msg_start );
static void WFLBY_EV_MG_WK_ExitBmpList( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk );


// �J���o��
static void WFLBY_EV_MG_MINIGAME_PrintInit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, u32 minigame_type, u32 kanban_mode,  s32 count );
static void WFLBY_EV_MG_MINIGAME_PrintExit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_MG_MINIGAME_PrintSetCount( WFLBY_EV_MG_KANBANWK* p_evwk, s32 count );
static void WFLBY_EV_MG_MINIGAME_PrintMinigame( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, BOOL force );

//----------------------------------------------------------------------------
/**
 *	@brief		�~�j�Q�[���̊J�n�C�x���g
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_MG_Start( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_MG_WK* p_evwk;
	WFLBY_EV_MG_PARAM* p_param;
	WFLBY_SYSTEM* p_system;
	
	p_param	= WFLBY_EVENTWK_GetParam( p_wk );
	p_evwk	= WFLBY_EVENTWK_GetWk( p_wk );

	p_system = WFLBY_ROOM_GetSystemData( p_rmwk );

//	OS_TPrintf( "mg start seq=%d\n", WFLBY_EVENTWK_GetSeq( p_wk ) );


	// �Q�d��switch���̃`�F�b�N�������邪�G���[�`�F�b�N��
	// �����ȃV�[�P���X�ɓ����̂�����Ȃ̂ł����ň�C�Ƀ`�F�b�N����
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){

	// �}�b�`������
	case WFLBY_EV_MG_INFO_INIT:		// CommInfo�̏�����	
	case WFLBY_EV_MG_INFO_POKE:		// CommInfo��PokeData���M
	case WFLBY_EV_MG_STARTWAIT:		// �Q�[���J�n�҂�
	case WFLBY_EV_MG_PLIDX_RESET:	// PLAYER_IDX������
	case WFLBY_EV_MG_PLIDX_SEND:		// PLAYER_IDX���M
	case WFLBY_EV_MG_PLIDX_CHECK:	// PLAYER_IDX�`�F�b�N
		// �ڑ��l������������G���[��
		if( p_evwk->match_entry_num > DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) ){
			OS_TPrintf( "p_evwk->match_entry_num > DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) = %d > %d\n", p_evwk->match_entry_num,  DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) );
			p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
			break;
		}

		// �ʐM�l�����G���g���[�l���������Ƃ��͂�������
		if( p_evwk->match_entry_num > CommGetConnectNum() ){
			OS_TPrintf( "p_evwk->match_entry_num > CommGetConnectNum() = %d > %d\n", p_evwk->match_entry_num, CommGetConnectNum() );
			p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
			break;
		}

		// �ڑ����ؒf���ꂽ��I��
		if( CommStateWifiP2PGetConnectState() == COMMSTATE_WIFIP2P_CONNECT_NONE ){
			OS_TPrintf( "CommStateWifiP2PGetConnectState() == COMMSTATE_WIFIP2P_CONNECT_NONE\n" );
			p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
			break;
		}

	// �^�C�����~�b�g���z������Ƀ}�b�`���O�����҂�
	case WFLBY_EV_MG_MATCHOKWAIT:	
		// �e�ŁE�ڑ��l�����������ɂȂ�����I����
		if( DWC_LOBBY_MG_MyParent() == TRUE ){
			if( DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) <= 1 ){
				OS_TPrintf( "DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) <= 1\n" );
				p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;
			}
		}



	// �}�b�`������{�}�b�`�O
	case WFLBY_EV_MG_MATCHWAIT:		// �}�b�`���O�E�G�C�g
	case WFLBY_EV_MG_CANCEL_CHECK:	// CANCEL�����`�F�b�N
	case WFLBY_EV_MG_CANCEL_WAIT:	// CANCEL�����`�F�b�N
	case WFLBY_EV_MG_CANCEL_MW:		// CANCEL�����`�F�b�N	MATCHWAIT�ɖ߂�
	case WFLBY_EV_MG_CANCEL_ININ:	// CANCEL�����`�F�b�N	INFO_INIT�ɍs��
		// �G���[�`�F�b�N
		switch( CommWifiIsMatched() ){
		case 2:		// �G���[��CANCEL
		case 3:		// �^�C���A�E�g
		case 4:		// �ؒf
		case 5:		// �y�x�Ȃ���[
			OS_TPrintf( "err CommWifiIsMatched()==%d\n", CommWifiIsMatched() );
			p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
			break;
		// ���̑��̃G���[��ڑ����Ȃǂ͂��̂܂܂ɂ��Ă���
		// ���̑��̃G���[��ROOM�Ń`�F�b�N���Ă�
		default:
			break;
		}
		break;	


	// ���̑��̓G���[�`�F�b�N����Ȃ�
	default:
		break;
	}






	
	
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// �}�b�`���O�J�n
	case WFLBY_EV_MG_INIT:
		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_MG_WK) );

		// �V�[�P���X��i�߂Ă���
		WFLBY_EVENTWK_AddSeq( p_wk );
		break;

	// �Q���ł��邩�`�F�b�N
	case WFLBY_EV_MG_ENTRY_CHK:		

		// �Q�����b�N���������ĂȂ����H
		if( WFLBY_SYSTEM_MGFLAG_GetLock( p_system ) ){
			p_param->in_ok = WFLBY_EV_MG_RET_NG_LOCK;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
			break;
		}
		
		// �������ĂȂ����H
		if( WFLBY_SYSTEM_Event_GetMiniGameStop( p_system ) ){
			// �����~�j�Q�[���J�Â��Ă�
			p_param->in_ok = WFLBY_EV_MG_RET_NG_TIME;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
			break;
		}

		
		// ��W����
		if( DWC_LOBBY_MG_CheckRecruit( p_param->mg_type ) == TRUE ){

			// ��������Ȃ����`�F�b�N
			if( (DWC_LOBBY_MG_GetRest( p_param->mg_type ) == 0) ||
				(DWC_LOBBY_MG_CheckEntryOk( p_param->mg_type ) == FALSE)){
				// �����~�j�Q�[���J�Â��Ă�
				p_param->in_ok = WFLBY_EV_MG_RET_NG_PLAYING;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
				break;
			}
		}

		// ���̂Ƃ���Q���ł���
		// �Q�����邩����
		WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_param->wflby_mg_type, 0 );
		WFLBY_EV_MG_MsgPrint( p_evwk, p_wk, p_rmwk, msg_wifi_h_info_01_01, WFLBY_EV_MG_ENTRY_SEL );

		Snd_SePlay( WFLBY_SND_MINIGAME );
		break;

	// �Q�����邩�I��
	case WFLBY_EV_MG_ENTRY_SEL:		
		// �r�b�g�}�b�v���X�g�̕\��
		WFLBY_EV_MG_WK_InitBmpList( p_evwk, p_rmwk, WFLBY_EV_MG_SELLIST_NUM, msg_wifi_h_info_01_02 );
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist, 0, 0 );

		// �I��҂���
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ENTRY_SELWAIT );
		break;
		
	// �Q�����邩�I��҂�
	case WFLBY_EV_MG_ENTRY_SELWAIT:	
		{
			u32 sel;
			BOOL end = FALSE;
			sel = WFLBY_ROOM_LISTWIN_Main( p_rmwk );
			switch( sel ){
			// �ؒf��
			case BMPLIST_CANCEL:
			case WFLBY_EV_MG_SELLIST_NO:
				p_param->in_ok = WFLBY_EV_MG_RET_NG_MY;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
				end = TRUE;
				break;

			// �ڑ���
			case WFLBY_EV_MG_SELLIST_ENTRY:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MATCHSTART );
				end = TRUE;
				break;

			default:
				break;
			}
			
			if( end == TRUE ){
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_MG_WK_ExitBmpList( p_evwk, p_rmwk );
			}
		}
		break;

	// �}�b�`���O�J�n
	case WFLBY_EV_MG_MATCHSTART:

		// �O�̂��߂�����x�������Ă��Ȃ����`�F�b�N����	
		// �������ĂȂ����H
		if( WFLBY_SYSTEM_Event_GetMiniGameStop( p_system ) ){
			// �����~�j�Q�[���J�Â��Ă�
			p_param->in_ok = WFLBY_EV_MG_RET_NG_TIME;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
			break;
		}

		
		// �e�̂Ƃ��́A�l�������������Q�[�����J�n������
		p_evwk->timeout = DWC_LOBBY_MG_GetTimeLimit( p_param->mg_type );
		
		// ���̃����o�[��҂��Ă��܂��ƃ��b�Z�[�W
		{
			STRBUF* p_str;
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_02_02 );
			WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
			WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
		}

		// P2P�ڑ��J�n
		CommStateWifiP2PStart( p_param->mg_type );

		// ��W�J�n��������
		if( DWC_LOBBY_MG_MyParent() == TRUE ){
			// �e�Ȃ��W�J�n��������
			WFLBY_SYSTEM_TOPIC_SendMiniGame( p_system, p_param->wflby_mg_type, 1, 
					plno, 0, 0, 0, FALSE );
			p_evwk->last_entry_num = 1;
		}

		//	�~�j�Q�[���J���o���\��
		WFLBY_EV_MG_MINIGAME_PrintInit( &p_evwk->kanban, p_rmwk, 
				p_param->wflby_mg_type, WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN,
				p_evwk->timeout );
		WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->kanban, p_rmwk, TRUE );

		// �V�[�P���X��i�߂Ă���
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MATCHWAIT );
		break;	

	// �}�b�`���O�҂�
	case WFLBY_EV_MG_MATCHWAIT:
		{
			u32 state;
			u32 con_num;

			state = CommStateWifiP2PGetConnectState();
			con_num = DWC_LOBBY_MG_GetEntryNum( p_param->mg_type );

#ifdef WFLBY_EV_MG_DEBUG_MATCHING_STATE
			{
				DWCMatchState match_state;
				match_state = DWC_GetMatchingState();
				OS_TPrintf( "matching state = %d\n", match_state );
			}
#endif

			// �^�C���A�E�g�J�E���g
			p_evwk->timeout  = DWC_LOBBY_MG_GetTimeLimit( p_param->mg_type );
			WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, p_evwk->timeout );

			// �J���o���`��X�V
			WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->kanban, p_rmwk, FALSE );


			// �}�b�`���O���s
			if( state == COMMSTATE_WIFIP2P_CONNECT_NONE ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;

			}

			// �}�b�`���O����
			if( state == COMMSTATE_WIFIP2P_CONNECT_MATCH ){
				
				p_param->in_ok = WFLBY_EV_MG_RET_OK;

				// �l���ɂ���ă��b�Z�[�W��ύX����
				{
					u32 msgidx;
					STRBUF* p_str;

					if( con_num == WFLBY_MINIGAME_MAX ){
						msgidx = msg_wifi_h_info_02_04;
					}else{
						msgidx = msg_wifi_h_info_03_01;
					}
					
					WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
					p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msgidx );
					WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
					WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_INFO_INIT );

					// ���̐ڑ��l����ۑ�
					p_evwk->match_entry_num	= con_num;
				}

				// �J���o���`��X�V
				// ����ȍ~�͕ς�邱�Ƃ͂Ȃ��̂ōX�V���Ȃ�
				WFLBY_EV_MG_MINIGAME_PrintSetCount( &p_evwk->kanban, 0 );
				WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->kanban, p_rmwk, FALSE );
				break;
			}


			// B�{�^���ŎQ��Cancel
			if( sys.trg & PAD_BUTTON_CANCEL ){
				
				if( DWC_LOBBY_MG_MyParent() == FALSE ){
					// �q�@�p����
					p_param->in_ok = WFLBY_EV_MG_RET_NG_BCAN;
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				}else{
					// �e�@�̓`�F�b�N������
					p_param->in_ok = WFLBY_EV_MG_RET_NG_BCAN;
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_MSG );
				}
				OS_TPrintf( "B cancel\n" );
				break;
			}
			

			// �e�Ȃ�^�C���A�E�g�`�F�b�N
			// ��W�l���ύX�g�s�b�N����
			if( DWC_LOBBY_MG_MyParent() == TRUE ){
				// �l���̕ύX���`�F�b�N
				if( p_evwk->last_entry_num != con_num ){
					p_evwk->last_entry_num = con_num;
					WFLBY_SYSTEM_TOPIC_SendMiniGame( p_system, 
							p_param->wflby_mg_type, con_num, plno,
							0,0,0, FALSE );
				}
				
				if( p_evwk->timeout == 0 ){
					// �l�����P�l�Ȃ�^�C���A�E�g
					if( DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) <= 1 ){
						
						// �^�C���A�E�g
						p_param->in_ok = WFLBY_EV_MG_RET_NG_MIN;
						WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
					
						OS_TPrintf( "lobby mg timeout \n" );
						break;
					}
				}
			}

			// ��W���������Ă�����ł���A�l���ɓ���Ȃ�����
			if( (DWC_LOBBY_MG_CheckRecruit( p_param->mg_type ) == TRUE) &&
				(DWC_LOBBY_MG_CheckEntryOk( p_param->mg_type ) == FALSE) ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_NUM;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;
			}

			// �^�C���A�E�g�ɂȂ�����ڑ������҂���
			if( p_evwk->timeout == 0 ){
				// ���Ԃ������̂Ői��  �}�b�`���O�̌��ʑ҂���
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MATCHOKWAIT );
				{
					STRBUF* p_str;

					WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
					p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_03_01 );
					WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
					WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
				}
				OS_TPrintf( "match ok wait start\n" );
				break;
			}
		}
		break;

	// �^�C�����~�b�g���z������Ƀ}�b�`���O�����҂�
	// �����P�Ƀ}�b�`���O����������̂�҂�
	case WFLBY_EV_MG_MATCHOKWAIT:
		{
			u32 state;
			u32 con_num;

			state = CommStateWifiP2PGetConnectState();
			con_num = DWC_LOBBY_MG_GetEntryNum( p_param->mg_type );

#ifdef WFLBY_EV_MG_DEBUG_MATCHING_STATE
			{
				DWCMatchState match_state;
				match_state = DWC_GetMatchingState();
				OS_TPrintf( "matching state = %d\n", match_state );
			}
#endif

			// �G���[�`�F�b�N
			switch( CommWifiIsMatched() ){
			case 3:
			case 4:
				p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;
			// ���̑��̃G���[��ڑ����Ȃǂ͂��̂܂܂ɂ��Ă���
			// ���̑��̃G���[��ROOM�Ń`�F�b�N���Ă�
			default:
				break;
			}

			// �}�b�`���O���s
			if( state == COMMSTATE_WIFIP2P_CONNECT_NONE ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;

			}

			// �}�b�`���O����
			if( state == COMMSTATE_WIFIP2P_CONNECT_MATCH ){
				
				p_param->in_ok = WFLBY_EV_MG_RET_OK;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_INFO_INIT );

				// ���̐ڑ��l����ۑ�
				p_evwk->match_entry_num	= con_num;
				break;
			}

			// ��W���������Ă�����ł���A�l���ɓ���Ȃ�����
			if( (DWC_LOBBY_MG_CheckRecruit( p_param->mg_type ) == TRUE) &&
				(DWC_LOBBY_MG_CheckEntryOk( p_param->mg_type ) == FALSE) ){
				p_param->in_ok = WFLBY_EV_MG_RET_NG_NUM;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
				break;
			}
		}
		break;

	// CommInfo�̏�����
	case WFLBY_EV_MG_INFO_INIT:	

		// �X�e�[�^�X��ύX����
		WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), p_param->wflby_mg_status );
		

		OS_TPrintf( "matchok netid%d\n", CommGetCurrentID() );

		// 4�l�ڑ����[�h�ɂ���
		CommStateChangeWiFiLobbyMinigame();

		// Info
		CommInfoInitialize( WFLBY_SYSTEM_GetSaveData( WFLBY_ROOM_GetSystemData( p_rmwk ) ), NULL );

		// �����Ȃ�
		CommSetWifiBothNet(FALSE);

		
		// �����J�n
		WFLBY_EV_MG_Sync( p_evwk, p_wk, WFLBY_EV_MG_INFO_POKE, _TIMING_POKEDATA_SEND );
		break;
	
	// CommInfo��PokeData���M
	case WFLBY_EV_MG_INFO_POKE:
		// ������������
		CommInfoSendPokeData();

		// �����̓G���g���[
		CommInfoSetEntry( CommGetCurrentID() );

		// �ʐM�J�n���ߑ҂���
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_STARTWAIT );
		break;

	// �Q�[���J�n�҂�
	case WFLBY_EV_MG_STARTWAIT:
		// �V���������l��ENTRY��Ԃɂ���
		{
			int netid;
			while( (netid = CommInfoGetNewNameID()) != INVALID_NETID ){
				CommInfoSetEntry(netid);
				OS_TPrintf( "Entry=%d  EntryNum=%d\n", netid, CommInfoGetEntryNum() );
			}
		}

		/*
        if( CommGetCurrentID() == 0 ){
            // �V�KPOKEDATA����M������݂�Ȃɑ��M
			CommInfoSendArray_ServerSide();	// �݂�Ȃ���������POKEDATA�𑗐M
        }
		//*/
		// �l�������������Q�[�����J�n������
		if( CommInfoGetEntryNum() >= DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) ){
			if( DWC_LOBBY_MG_MyParent() == TRUE ){
				if( p_evwk->start_game_set == FALSE ){
					DWC_LOBBY_MG_StartGame();
					p_evwk->start_game_set = TRUE;
				}
			}

			// �e����Q�[���J�n���߂�������Q�[�����J�n����
			if( DWC_LOBBY_MG_CheckStartGame() == TRUE ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_PLIDX_RESET );
			}
		}
		break;

	// PLAYER_IDX������
	case WFLBY_EV_MG_PLIDX_RESET:	

		CommToolTempDataReset();
		WFLBY_SYSTEM_PLIDX_Clear( p_system );	// P2P�ʐM���胍�r�[��PLIDX�o�b�t�@�N���A
		WFLBY_EV_MG_Sync( p_evwk, p_wk, WFLBY_EV_MG_PLIDX_SEND, _TIMING_GAME_CHECK2 );
		break;

	// PLAYER_IDX���M
	case WFLBY_EV_MG_PLIDX_SEND:
		p_evwk->tmp_userid = WFLBY_SYSTEM_GetMyUserID( p_system );
		CommToolSetTempData( CommGetCurrentID(), &p_evwk->tmp_userid );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_PLIDX_CHECK );
		break;	

	// PLAYER_IDX�`�F�b�N
	case WFLBY_EV_MG_PLIDX_CHECK:

		{
			int i;
			int con_num;
			int current_id;
			const s32* cp_data;
			int ok_num;


			// �G���g���[�����擾
			con_num = CommInfoGetEntryNum();

			current_id = CommGetCurrentID();

			ok_num = 0;

			for( i=0; i<con_num; i++ ){

				if( current_id != i ){

					// PLAYER_IDX���ꏏ���`�F�b�N
					cp_data = CommToolGetTempData(i);	

					// �X�e�[�^�X�̎�M���������Ă��邩�`�F�b�N
					if( cp_data != NULL ){

						// P2P�ʐM���Ă��鑊���PLIDX��ۑ�����
						WFLBY_SYSTEM_PLIDX_Set( p_system, cp_data[0], i );	// P2P�ʐM���胍�r�[��PLIDX�o�b�t�@�N���A
						ok_num ++;
					}
				}else{
					// ����
					WFLBY_SYSTEM_PLIDX_SetMyData( p_system, i );	// P2P�ʐM���胍�r�[��PLIDX�o�b�t�@�N���A
					ok_num ++;
				}
			}

			// �S���̃X�e�[�^�X����M���ă`�F�b�N���ʂ�OK�Ȃ玟�̏�����
			if( ok_num == con_num ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_COMM_START );
			}
		}
		break;
		
	// �ʐM�J�n
	case WFLBY_EV_MG_COMM_START:

		CommStateSetErrorCheck(TRUE,TRUE);

		// �e�Ȃ�Q�[�������Ă��邱�Ƃ𑗐M
		if( DWC_LOBBY_MG_MyParent() == TRUE ){
			u8 pl_tbl[ WFLBY_MINIGAME_MAX ] = {0};
			int i;
			int count;
			u32 status;
			const WFLBY_USER_PROFILE* cp_profile;

			// �~�j�Q�[���ɎQ�����Ă���l���擾
			count = 1;
			for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
				if( i != plno ){
					cp_profile = WFLBY_SYSTEM_GetUserProfile( p_system, i );
					if( cp_profile ){
						status = WFLBY_SYSTEM_GetProfileStatus( cp_profile );
						if( status == p_param->wflby_mg_status ){
							pl_tbl[ count ] = i;
							count ++;
						}
					}
				}
			}
//			GF_ASSERT( count == DWC_LOBBY_MG_GetEntryNum( p_param->mg_type ) );
			
			// �e�Ȃ�Q�[���J�n�g�s�b�N�쐬
			WFLBY_SYSTEM_TOPIC_SendMiniGame( p_system, p_param->wflby_mg_type,
					count, plno, pl_tbl[1], pl_tbl[2], pl_tbl[3], TRUE );

			OS_TPrintf( "mgame start topic send count==%d\n", count );
		}

		// �{�E�P���m�[�g
		WFLBY_SYSTEM_FNOTE_SetPlayMinigame( p_system, p_param->wflby_mg_type );
				
		// �������������I��
		WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
		WFLBY_EV_MG_Sync( p_evwk, p_wk,
				WFLBY_EV_MG_END, _TIMING_GAME_START2 ); 
		break;

	// CANCEL�����`�F�b�N		���b�Z�[�W�\��
	case WFLBY_EV_MG_CANCEL_MSG:
		// �^�C���E�G�C�g�A�C�R���j��
		WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
		// ���b�Z�[�W�\��
		WFLBY_EV_MG_MsgPrint( p_evwk, p_wk, p_rmwk, 
				msg_wifi_h_info_05_07, WFLBY_EV_MG_CANCEL_CHECK );
		// �T�u�E�B���h�E��j��
		WFLBY_EV_MG_MINIGAME_PrintExit( &p_evwk->kanban, p_rmwk );
		break;

	// CANCEL�����`�F�b�N		YESNO�E�B���h�E�\��
	case WFLBY_EV_MG_CANCEL_CHECK:
		// YESNO�E�B���h�EON
		WFLBY_ROOM_YESNOWIN_StartNo( p_rmwk );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_WAIT );
		break;

	// CANCEL�����`�F�b�N		YESNO�I��҂�
	case WFLBY_EV_MG_CANCEL_WAIT:
		{
			WFLBY_ROOM_YESNO_RET result;
			result = WFLBY_ROOM_YESNOWIN_Main( p_rmwk );
			switch( result ){
			// �͂�
			case WFLBY_ROOM_YESNO_OK:	
				// YESNO�E�B���h�E��������
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );

				// �Q�����b�N�ݒ�
				WFLBY_SYSTEM_MGFLAG_SetLock( p_system );
				break;
				
			// ������
			case WFLBY_ROOM_YESNO_NO:
				// YESNO�E�B���h�E������,MATCHWAIT�ɖ߂�
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_MW );
				break;

			// �I�ђ�
			case WFLBY_ROOM_YESNO_WAIT:	
				// �}�b�`���O������҂�
				{
					u32 state;
					u32 con_num;

					state	= CommStateWifiP2PGetConnectState();
					con_num = DWC_LOBBY_MG_GetEntryNum( p_param->mg_type );


					// �}�b�`���O���s
					if( state == COMMSTATE_WIFIP2P_CONNECT_NONE ){
						p_param->in_ok = WFLBY_EV_MG_RET_NG_DISCON;
						WFLBY_ROOM_YESNOWIN_End( p_rmwk );
						WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERREND );
						break;

					}

					// �}�b�`���O����
					if( state == COMMSTATE_WIFIP2P_CONNECT_MATCH ){

						// ���̐ڑ��l����ۑ�
						p_evwk->match_entry_num	= con_num;

						// YESNO�E�B���h�E������,INFO_INIT�ɍs��
						WFLBY_ROOM_YESNOWIN_End( p_rmwk );
						WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_CANCEL_ININ );
					}
				}
				break;
			}
		}
		break;

	// CANCEL�����`�F�b�N	MATCHWAIT�ɖ߂�
	case WFLBY_EV_MG_CANCEL_MW:		
		WFLBY_EV_MG_MINIGAME_PrintInit( &p_evwk->kanban, p_rmwk, 
				p_param->wflby_mg_type, WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN,
				DWC_LOBBY_MG_GetTimeLimit( p_param->mg_type ) );
		WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->kanban, p_rmwk, TRUE );

		// ���̃����o�[��҂��Ă��܂��ƃ��b�Z�[�W
		{
			STRBUF* p_str;
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_02_02 );
			WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
			WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
		}
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MATCHWAIT );
		break;
		
	// CANCEL�����`�F�b�N	INFO_INIT�ɍs��
	case WFLBY_EV_MG_CANCEL_ININ:	
		{
			u32 con_num;

			con_num = DWC_LOBBY_MG_GetEntryNum( p_param->mg_type );

			p_param->in_ok = WFLBY_EV_MG_RET_OK;

			// �l���ɂ���ă��b�Z�[�W��ύX����
			{
				u32 msgidx;
				STRBUF* p_str;

				if( con_num == WFLBY_MINIGAME_MAX ){
					msgidx = msg_wifi_h_info_02_04;
				}else{
					msgidx = msg_wifi_h_info_03_01;
				}
				
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msgidx );
				WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
				WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_INFO_INIT );

			}

			// �J���o�����ĕ\��
			WFLBY_EV_MG_MINIGAME_PrintInit( &p_evwk->kanban, p_rmwk, 
					p_param->wflby_mg_type, WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN,
					0 );
			WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->kanban, p_rmwk, TRUE );
		}
		break;
		
	// �ʐM�ؒf�I��
	case WFLBY_EV_MG_ERREND:
		// �ʐM�ؒf
        CommInfoFinalize();
		CommStateWifiP2PEnd();
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_ERRENDWAIT );
		break;

	// �ʐM�ؒf�I���҂�
	case WFLBY_EV_MG_ERRENDWAIT:
		{
			u32 status;
			status = CommStateWifiP2PGetConnectState();
			if( status == COMMSTATE_WIFIP2P_CONNECT_NONE ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END );
			}
		}
		break;

	// ���b�Z�[�W�I���҂�
	case WFLBY_EV_MG_MSG_WAIT:		
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->msg_ret_seq );
		}
		break;

	// ���b�Z�[�W�I���҂���������
	case WFLBY_EV_MG_MSG_SYNC:
		if( p_evwk->msg_wait > 0 ){
			p_evwk->msg_wait--;
		}
		if( p_evwk->msg_wait == 0 ){
			// �����J�n
			CommTimingSyncStart(p_evwk->msg_sync);
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_MSG_SYNCWAIT );
		}
		break;

	// ���b�Z�[�W�I���҂���������
	case WFLBY_EV_MG_MSG_SYNCWAIT:
		// �R�}���h�ē]������
		p_evwk->sync_count ++;
		if( p_evwk->sync_count >= WFLBY_EV_MG_SYNC_RETRANS_TIMING ){
			OS_TPrintf( "re sync start \n" );
			CommTimingSyncStart(p_evwk->msg_sync);
			p_evwk->sync_count = 0;
		}
		
		// ���������҂�
		if(CommIsTimingSync(p_evwk->msg_sync)){
			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->msg_ret_seq );
		}
		break;

	// �I��
	case WFLBY_EV_MG_END:

		//���Ƃ��܂�
		{
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );
			WFLBY_EV_MG_WK_ExitBmpList( p_evwk, p_rmwk );
			WFLBY_EV_MG_MINIGAME_PrintExit( &p_evwk->kanban, p_rmwk );
		}
		
		WFLBY_EVENTWK_DeleteWk( p_wk );
		return TRUE;
	}

	return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief		�~�j�Q�[������̑ގ�����
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_MG_End( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// �ؒf���N�G�X�g
	case WFLBY_EV_MG_END_REQ:
		// P2P�ʐM��ؒf����
		CommStateSetErrorCheck(FALSE,FALSE);
		
		// Info�I��
		CommInfoFinalize();

		// �ʐM�ؒf
		CommStateWifiP2PEnd();

		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_MG_END_WAIT );
		break;
		
	// �ؒf�҂�
	case WFLBY_EV_MG_END_WAIT:
		if( CommStateWifiP2PGetConnectState() == COMMSTATE_WIFIP2P_CONNECT_NONE ){
			return TRUE;
		}
		break;
	}
	
	
	return FALSE;
}










//----------------------------------------------------------------------------
/**
 *	@brief		�~�j�Q�[���J���o���C�x���g
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_DEF_PlayerA_MINIGAME_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_PLAYER_KANBANWK* p_evwk;

	p_evwk = WFLBY_EVENTWK_GetWk( p_wk );
	


	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	case 0:
		{
			STRBUF* p_str;
			u32 evno;
			u32 msgidx;
			u32 minigame_type;

			p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_PLAYER_KANBANWK) );

			evno = WFLBY_ROOM_GetEvNo( p_rmwk );

			switch( evno ){
			case WFLBY_MAPEVID_EV_KANBAN00:
				msgidx = msg_hiroba_bord_02;
				minigame_type = WFLBY_GAME_BALLSLOW;
				break;
			case WFLBY_MAPEVID_EV_KANBAN01:
				msgidx = msg_hiroba_bord_01;
				minigame_type = WFLBY_GAME_BALANCEBALL;
				break;
			case WFLBY_MAPEVID_EV_KANBAN02:
				msgidx = msg_hiroba_bord_03;
				minigame_type = WFLBY_GAME_BALLOON;
				break;
			default:
				GF_ASSERT(0);
				return TRUE;
			}

			WFLBY_ROOM_MSG_SetMinigame( p_rmwk, minigame_type, 0 );
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msgidx );
			WFLBY_ROOM_BOARDWIN_PrintStr( p_rmwk, p_str );

			// �ڑ���Ԃ�\��
			WFLBY_EV_MG_MINIGAME_PrintInit( &p_evwk->data, p_rmwk, minigame_type, WFLBY_EV_MG_KANBAN_MODE_NORMAL, 0 );
			// 
			WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->data, p_rmwk, TRUE );
			
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	case 1:
		if( (WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk )) || (sys.trg & PAD_KEY_RIGHT) || (sys.trg & PAD_KEY_LEFT) || (sys.trg & PAD_KEY_DOWN) ){
			WFLBY_EV_MG_MINIGAME_PrintExit( &p_evwk->data, p_rmwk );
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );
			{
				WFLBY_3DOBJCONT* p_objcont;
				WFLBY_3DPERSON* p_person;

				p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
				p_person	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
				WFLBY_3DOBJCONT_SetWkMove( p_person, WFLBY_3DOBJCONT_MOVEPLAYER );
			}
			WFLBY_EVENTWK_DeleteWk( p_wk );
			return TRUE;
		}else{
			WFLBY_EV_MG_MINIGAME_PrintMinigame( &p_evwk->data, p_rmwk, FALSE );
		}
		break;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\������
 *
 *	@param	p_wk		���[�N
 *	@param	p_eventwk	�C�x���g���[�N
 *	@param	p_rmwk		�������[�N
 *	@param	msgidx		���b�Z�[�WIDX
 *	@param	retseq		�߂�V�[�P���X
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MsgPrint( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, WFLBY_ROOMWK* p_rmwk, u32 msgidx, u32 retseq )
{
	STRBUF* p_str;

	p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msgidx );
	WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

	// �߂�V�[�P���X�ݒ�
	p_wk->msg_ret_seq = retseq;

	
	WFLBY_EVENTWK_SetSeq( p_eventwk, WFLBY_EV_MG_MSG_WAIT );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\������	�ʐM����
 *
 *	@param	p_wk		���[�N
 *	@param	p_eventwk	�C�x���g���[�N
 *	@param	p_rmwk		�������[�N
 *	@param	msgidx		���b�Z�[�WIDX
 *	@param	retseq		�߂�V�[�P���X
 *	@param	syncno		�����i���o�[
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MsgPrintSync( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, WFLBY_ROOMWK* p_rmwk, u32 msgidx, u32 retseq, u32 syncno, u32 msgwait )
{
	STRBUF* p_str;

	p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msgidx );
	WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

	// �߂�V�[�P���X�ݒ�
	p_wk->msg_ret_seq = retseq;

	// �����i���o�[�ݒ�
	p_wk->msg_sync = syncno;

	// �E�G�C�g�ݒ�
	p_wk->msg_wait = msgwait;

	// �V���N�������҂��E�G�C�g
	p_wk->sync_count = 0;

	
	WFLBY_EVENTWK_SetSeq( p_eventwk, WFLBY_EV_MG_MSG_SYNC );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM����
 *
 *	@param	p_wk			���[�N
 *	@param	p_eventwk		�C�x���g���[�N
 *	@param	retseq			���̃V�[�P���X
 *	@param	syncno			�����萔
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_Sync( WFLBY_EV_MG_WK* p_wk, WFLBY_EVENTWK* p_eventwk, u32 retseq, u32 syncno )
{
	// �߂�V�[�P���X�ݒ�
	p_wk->msg_ret_seq = retseq;

	// �����i���o�[�ݒ�
	p_wk->msg_sync = syncno;

	// �E�G�C�g�ݒ�
	p_wk->msg_wait = 0;

	// �V���N�������҂��E�G�C�g
	p_wk->sync_count = 0;

	
	WFLBY_EVENTWK_SetSeq( p_eventwk, WFLBY_EV_MG_MSG_SYNC );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�r�b�g�}�b�v���X�g�̍쐬
 *
 *	@param	p_evwk			�C�x���g���[�N
 *	@param	p_rmwk			�������[�N
 *	@param	num				���X�g��
 *	@param	msg_start		�ŏ��̕�����
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_WK_InitBmpList( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk, u32 num, u32 msg_start )
{
	int i;
	STRBUF* p_str;

	// �o�b�t�@�쐬
	WFLBY_ROOM_LISTWIN_CreateBmpList( p_rmwk, num );
	p_evwk->bmplist_num = num;

	// �f�[�^�쐬
	for( i=0; i<num; i++ ){
		p_str = WFLBY_ROOM_MSG_Get(p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_start+i );
		WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, p_str, i );
	}

	// �r�b�g�}�b�v���X�g�w�b�_�[�ɐݒ�
	p_evwk->bmplist			= sc_WFLBY_EV_MG_HEADER;
	p_evwk->bmplist.count	= p_evwk->bmplist_num;
	if( p_evwk->bmplist.line > p_evwk->bmplist_num ){
		p_evwk->bmplist.line = p_evwk->bmplist_num;
	}
	p_evwk->bmplist.list	= WFLBY_ROOM_LISTWIN_GetBmpList( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�r�b�g�}�b�v���X�g�j������
 *
 *	@param	p_evwk	�C�x���g���[�N
 *	@param	p_rmwk	�������[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_WK_ExitBmpList( WFLBY_EV_MG_WK* p_evwk, WFLBY_ROOMWK* p_rmwk )
{
	WFLBY_ROOM_LISTWIN_DeleteBmpList( p_rmwk );
}







//----------------------------------------------------------------------------
/**
 *	@brief	�J���o�����[�N�̏�����
 *
 *	@param	p_evwk			���[�N
 *	@param	minigame_type	�~�j�Q�[���^�C�v
 *	@param	kanban_mode		�J���o�����[�h
 *	@param	count			�J�E���g�̒l
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintInit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, u32 minigame_type, u32 kanban_mode,  s32 count )
{
	static const u8 sc_WINSIZY[ WFLBY_EV_MG_KANBAN_MODE_NUM ] = {
		WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZY,
		WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_SIZY,
	};
	
	GF_ASSERT( kanban_mode < WFLBY_EV_MG_KANBAN_MODE_NUM );

	memset( p_evwk, 0, sizeof(WFLBY_EV_MG_KANBANWK) );

	p_evwk->mode			= kanban_mode;
	p_evwk->minigame_type	= minigame_type;
	WFLBY_EV_MG_MINIGAME_PrintSetCount( p_evwk, count );

	// �ڑ���Ԃ�\��
	WFLBY_ROOM_SUBWIN_Start( p_rmwk, 
			WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_X,
			WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_Y,
			WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX,
			sc_WINSIZY[kanban_mode] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J���o�����[�N�̔j��
 *
 *	@param	p_evwk			���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintExit( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk )
{
	memset( p_evwk, 0, sizeof(WFLBY_EV_MG_KANBANWK) );
	WFLBY_ROOM_SUBWIN_End( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J���o���ŕ\������J�E���^�̒l��ݒ�
 *
 *	@param	p_evwk		���[�N
 *	@param	count		�J�E���^�̒l	�i�V���N���P�ʁj
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintSetCount( WFLBY_EV_MG_KANBANWK* p_evwk, s32 count )
{
	s16 set_num;

	set_num = count / 30;
	// �b�ɕύX
	if( set_num != p_evwk->number ){
		p_evwk->number = set_num;
		p_evwk->write |= WFLBY_EV_MG_KANBAN_DRAW_MSK_COUNT;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�~�j�Q�[���`��
 *
 *	@param	p_evwk		�J���o���f�[�^���[�N
 *	@param	p_rmwk		�������[�N
 *	@param	force		��΂ɏ���
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_MG_MINIGAME_PrintMinigame( WFLBY_EV_MG_KANBANWK* p_evwk, WFLBY_ROOMWK* p_rmwk, BOOL force )
{
	STRBUF* p_str;
	u32 dwc_minigame;
	BOOL recruit;
	BOOL play;
	u32 num;


	// �t���O���ď���
	if( force ){
		p_evwk->write = WFLBY_EV_MG_KANBAN_DRAW_MSK_ALL;
	}else{

		// DWC���C�u�����̃~�j�Q�[���^�C�v�ɕύX
		switch( p_evwk->minigame_type ){
		case WFLBY_GAME_BALLSLOW:	// �ʓ���
			dwc_minigame = DWC_LOBBY_MG_BALLSLOW;
			break;
		case WFLBY_GAME_BALANCEBALL:	// �ʏ��
			dwc_minigame = DWC_LOBBY_MG_BALANCEBALL;
			break;
		case WFLBY_GAME_BALLOON:		// �ӂ�������
			dwc_minigame = DWC_LOBBY_MG_BALLOON;
			break;
		
		default:
			GF_ASSERT(0);
			dwc_minigame = DWC_LOBBY_MG_BALLOON;
			break;
		}



		// �V��ł邩�`�F�b�N
		{
			recruit = DWC_LOBBY_MG_CheckRecruit( dwc_minigame );
			if( p_evwk->recruit != recruit ){
				p_evwk->write |= WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAY;
			}
			p_evwk->recruit = recruit;

			play = DWC_LOBBY_MG_CheckEntryOk( dwc_minigame );
			if( p_evwk->play != play ){
				p_evwk->write |= WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAY;
			}
			p_evwk->play = play;
		}

		// �l���`�F�b�N
		{
			if( recruit == TRUE ){
				num = DWC_LOBBY_MG_GetRest( dwc_minigame );
			}else{
				num = WFLBY_MINIGAME_MAX;
			}
			if( p_evwk->num != num ){
				p_evwk->write |= WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAYERNUM;
			}
			p_evwk->num = num;
		}

	}
	


	// �`�揈��
	// �^�C�g��
	if( p_evwk->write & WFLBY_EV_MG_KANBAN_DRAW_MSK_TITLE ){
		// �\���N���A
		WFLBY_ROOM_SUBWIN_ClearRect( p_rmwk, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_X, WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_Y,
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX*8, 16 );

		// �~�j�Q�[�����`��
		WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_evwk->minigame_type, 0 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_profile_05_06 );
		WFLBY_ROOM_SUBWIN_Print( p_rmwk, p_str, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_X,
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_TITLE_Y );
	}

	
	// �V��ł邩
	if( p_evwk->write & WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAY ){
		// �\���N���A
		WFLBY_ROOM_SUBWIN_ClearRect( p_rmwk, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_X, WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_Y,
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX*8, 16 );

		// �V��ł�Ƃ������o��
		if( (p_evwk->play == FALSE) && (p_evwk->recruit == TRUE) ){
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_bord2_02 );
			WFLBY_ROOM_SUBWIN_Print( p_rmwk, p_str, 
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_X,
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYING_Y );
		}
	}

	// �Q���l���Ǝc��l��
	if( p_evwk->write & WFLBY_EV_MG_KANBAN_DRAW_MSK_PLAYERNUM ){
			// �\���N���A->���̂ق�����
			WFLBY_ROOM_SUBWIN_ClearRect( p_rmwk, 
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_X, 
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_Y,
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX*8, 16 );

		// �`��
		WFLBY_ROOM_MSG_SetNumber( p_rmwk, p_evwk->num, 1, 1, NUMBER_DISPTYPE_ZERO );
		WFLBY_ROOM_MSG_SetNumber( p_rmwk, WFLBY_MINIGAME_MAX - p_evwk->num, 1, 0, NUMBER_DISPTYPE_ZERO );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_bord2_01 );
		WFLBY_ROOM_SUBWIN_Print( p_rmwk, p_str, 
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_X,
				WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_PLAYNUM_Y );
	}

	// �J�E���g�l
	if( p_evwk->mode == WFLBY_EV_MG_KANBAN_MODE_COUNTDOWN ){
		if( p_evwk->write & WFLBY_EV_MG_KANBAN_DRAW_MSK_COUNT ){

			// �\���N���A
			WFLBY_ROOM_SUBWIN_ClearRect( p_rmwk, 
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_X, 
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_Y,
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_SIZX*8, 16 );

			// �`��
			WFLBY_ROOM_MSG_SetNumber( p_rmwk, p_evwk->number, 2, 0, NUMBER_DISPTYPE_ZERO );
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_02_03 );
			WFLBY_ROOM_SUBWIN_Print( p_rmwk, p_str, 
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_X,
					WFLBY_EV_DEF_PLAYER_KANBAN_SUBWIN_COUNT_Y );
			
		}
	}

	// �`��t���O�N���A
	p_evwk->write = 0;
}


