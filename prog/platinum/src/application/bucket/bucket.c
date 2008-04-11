//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bucket.c
 *	@brief		�o�P�b�g�Q�[��
 *	@author		tomoya takahashi
 *	@data		2007.06.19
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "system/wipe.h"
#include "system/snd_tool.h"
#include "system/arc_util.h"

#include "ballslow_data.naix"

#include "application/wifi_lobby/minigame_tool.h"

#include "bct_common.h"
#include "application/bucket/comm_bct_command.h"
#include "application/bucket/comm_bct_command_func.h"
#include "application/bucket/bct_surver.h"
#include "application/bucket/bct_client.h"
#include "application/bucket/bct_local.h"
#include "application/bucket.h"


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
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
#define BCT_VRAMTR_MAN_NUM	( 32 )

// ���C���V�[�P���X
enum{
	BCT_MAINSEQ_ENTRY_INIT,
	BCT_MAINSEQ_ENTRY_WAIT,
	BCT_MAINSEQ_INIT,
	BCT_MAINSEQ_WIPE_IN,
	BCT_MAINSEQ_WIPE_INWAIT,
	BCT_MAINSEQ_STARTWAIT,
	BCT_MAINSEQ_COUNTDOWN,
	BCT_MAINSEQ_MAIN,
	BCT_MAINSEQ_SCORECOMM,
	BCT_MAINSEQ_SCORECOMMWAIT,
	BCT_MAINSEQ_RESULT,
	BCT_MAINSEQ_WIPE_OUT,
	BCT_MAINSEQ_WIPE_OUTWAIT,
	BCT_MAINSEQ_DELETE,
	BCT_MAINSEQ_RESULT_INIT,
	BCT_MAINSEQ_RESULT_WAIT,
};

// PLNETID�̃e�[�u���ɏ����l�Ƃ��ē����Ă���l
#define BCT_PLNETID_DEF		( 0xffff )

// �Ƃ肠����
#ifdef BCT_DEBUG_TIMESELECT
static u32 BCT_TIME_LIMIT = 1500;	// 50
#else
#define BCT_TIME_LIMIT	(30*40)
#endif


#ifdef PM_DEBUG
//#define BCT_DEBUG_ENTRY_CHG	// �G���g���[��ʂ����낢��ς���
#endif
#ifdef BCT_DEBUG_ENTRY_CHG
static int DEBUG_ENTRY_COUNT = 0;
#endif

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�o�P�b�g�Q�[�����[�N
//=====================================
typedef struct _BUCKET_WK{
	MNGM_ENTRYWK*		p_entry;
	MNGM_RESULTWK*		p_result;
	MNGM_ENRES_PARAM	enres_param;
	MNGM_RESULT_PARAM	result_param;
	BCT_SURVER*			p_surver;
	BCT_CLIENT*			p_client;
	BOOL start;
	BOOL end;
	BOOL score_get;	// �݂�Ȃ̓��_����M������
	u16 netid;
	u16 plno;
	BCT_GAMEDATA		gamedata;	// �Q�[���f�[�^
	u32 raregame;		// rare�Q�[��
} ;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static void BCT_VBlank( void* p_work );
static void BCT_ClientNutsSend( BUCKET_WK* p_wk );
static void BCT_ClientMiddleScoreSend( BUCKET_WK* p_wk );
static void BCT_GAMEDATA_Load( BUCKET_WK* p_wk, u32 heapID );
static void BCT_GAMEDATA_Release( BUCKET_WK* p_wk );


//----------------------------------------------------------------------------
/**
 *	@brief	�o�P�b�g�Q�[���v���Z�X�@�쐬
 *
 *	@param	p_proc		���[�N
 *	@param	p_seq		�V�[�P���X
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval	PROC_RES_FINISH,			///<����I��
 */
//-----------------------------------------------------------------------------
PROC_RESULT BucketProc_Init( PROC * p_proc, int * p_seq )
{
	BUCKET_WK* p_wk;
	BUCKET_PROC_WORK* pp = PROC_GetParentWork(p_proc);
	BOOL result;

#ifdef BCT_DEBUG_TIMESELECT
	OS_Printf( "���[�h�������ł������� x[40] a[45] b[50] y[55]\n" );
	result = FALSE;

	if( sys.cont & PAD_BUTTON_X ){
		BCT_TIME_LIMIT = 30*40;
		result = TRUE;
		OS_Printf( "40�b���[�h\n" );
	}else if( sys.cont & PAD_BUTTON_A ){
		BCT_TIME_LIMIT = 30*45;
		result = TRUE;
		OS_Printf( "45�b���[�h\n" );
	}else if( sys.cont & PAD_BUTTON_B ){
		BCT_TIME_LIMIT = 30*50;
		result = TRUE;
		OS_Printf( "50�b���[�h\n" );
	}else if( sys.cont & PAD_BUTTON_Y ){
		BCT_TIME_LIMIT = 30*55;
		result = TRUE;
		OS_Printf( "55�b���[�h\n" );
	}
	if( result == FALSE ){
		return PROC_RES_CONTINUE;
	}
#endif

	// �q�[�v�쐬
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_BUCKET, 0x60000 );

	// ���[�N�쐬
	p_wk = PROC_AllocWork( p_proc, sizeof(BUCKET_WK), HEAPID_BUCKET );
	memset( p_wk, 0, sizeof(BUCKET_WK) );


	// �G���g���[�E���ʉ�ʃp�����[�^�쐬
	MNGM_ENRES_PARAM_Init( &p_wk->enres_param, pp->wifi_lobby, pp->p_save, pp->vchat, &pp->lobby_wk );

	// �Q�[���\���f�[�^�ǂݍ���
	BCT_GAMEDATA_Load( p_wk, HEAPID_BUCKET );

	return PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�o�P�b�g�Q�[���v���Z�X�@���s
 *
 *	@param	p_proc		���[�N
 *	@param	p_seq		�V�[�P���X
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval	PROC_RES_FINISH,			///<����I��
 */
//-----------------------------------------------------------------------------
PROC_RESULT BucketProc_Main( PROC* p_proc, int* p_seq )
{
	BUCKET_WK* p_wk = PROC_GetWork( p_proc );
	BUCKET_PROC_WORK* pp = PROC_GetParentWork(p_proc);
	BOOL result;
	u32 check;
	u32 comm_num;

	switch( (*p_seq) ){
	case BCT_MAINSEQ_ENTRY_INIT:
#ifdef BCT_DEBUG_ENTRY_CHG
		switch(DEBUG_ENTRY_COUNT){
		case 0:
			TOMOYA_PRINT( "slow\n" );
			p_wk->p_entry = MNGM_ENTRY_InitBallSlow( &p_wk->enres_param, HEAPID_BUCKET );
			break;

		case 1:
			TOMOYA_PRINT( "balance\n" );
			p_wk->p_entry = MNGM_ENTRY_InitBalanceBall( &p_wk->enres_param, HEAPID_BUCKET );
			break;

		case 2:
			TOMOYA_PRINT( "balloon\n" );
			p_wk->p_entry = MNGM_ENTRY_InitBalloon( &p_wk->enres_param, HEAPID_BUCKET );
			break;
		}
#else
		p_wk->p_entry = MNGM_ENTRY_InitBallSlow( &p_wk->enres_param, HEAPID_BUCKET );
#endif
		(*p_seq) ++;
		break;

	case BCT_MAINSEQ_ENTRY_WAIT:
		if( MNGM_ENTRY_Wait( p_wk->p_entry ) == TRUE ){
			p_wk->raregame	= MNGM_ENTRY_GetRareGame( p_wk->p_entry );
			MNGM_ENTRY_Exit( p_wk->p_entry );
			(*p_seq) ++;
		}
		break;
		
	case BCT_MAINSEQ_INIT:
		// V�u�����NH�u�����N�֐��ݒ�
		sys_VBlankFuncChange( BCT_VBlank, p_wk );	// VBlank�Z�b�g
		sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

		// �e��t���O�̏�����
		p_wk->start		= FALSE;
		p_wk->end		= FALSE;
		p_wk->score_get = FALSE;

		// VramTransferManager������
		initVramTransferManagerHeap( BCT_VRAMTR_MAN_NUM, HEAPID_BUCKET );

		// �ʐM�J�n
		CommCommandBCTInitialize( p_wk );

		// �ʐM�l�����擾
		comm_num = CommInfoGetEntryNum();
		GF_ASSERT( comm_num > 1 );

		// �ʐMID�擾
		p_wk->netid = CommGetCurrentID();
		p_wk->plno = MNGM_ENRES_PARAM_GetNetIDtoPlNO( &p_wk->enres_param, p_wk->netid );

		// �T�[�o�[������
		if( p_wk->netid == COMM_PARENT_ID ){
			p_wk->p_surver = BCT_SURVER_Init( HEAPID_BUCKET, BCT_TIME_LIMIT, comm_num, &p_wk->gamedata );
		}
		p_wk->p_client = BCT_CLIENT_Init( HEAPID_BUCKET, BCT_TIME_LIMIT, comm_num, p_wk->plno, &p_wk->gamedata );

		WirelessIconEasy();  // �ڑ����Ȃ̂ŃA�C�R���\��

		// VChatOn
		if( pp->vchat ){
			// �{�C�X�`���b�g�J�n
			mydwc_startvchat( HEAPID_BUCKET );
			TOMOYA_PRINT( "vct on\n" );
		}

		// ���A�Q�[���`�F�b�N���s��
		{
			BCT_GAME_TYPE_WK gametype;

			TOMOYA_PRINT( "p_wk->raregame = %d\n", p_wk->raregame );

			switch( p_wk->raregame ){
			case MNGM_RAREGAME_BUCKET_NORMAL:
				// ���̂܂�
				gametype.scale_rev	= FALSE;
				gametype.rota_rev	= FALSE;
				break;
			case MNGM_RAREGAME_BUCKET_REVERSE:
				// �t��]
				gametype.scale_rev	= FALSE;
				gametype.rota_rev	= TRUE;
				break;
			case MNGM_RAREGAME_BUCKET_BIG:
				// �X�P�[�����]
				gametype.scale_rev	= TRUE;
				gametype.rota_rev	= FALSE;
				break;

			default:
				GF_ASSERT(0);
				break;
			}

			BCT_CLIENT_GameTypeSet( p_wk->p_client, &gametype );
		}
		

//		Snd_DataSetByScene( SND_SCENE_BCT, SEQ_KINOMI1, 1 );

		TOMOYA_PRINT( "init\n" );
		(*p_seq) ++;
		break;

	case BCT_MAINSEQ_WIPE_IN:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_DOORIN, 
				WIPE_TYPE_DOORIN, WIPE_FADE_OUTCOLOR, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BUCKET );
		(*p_seq) ++;
		break;

	case BCT_MAINSEQ_WIPE_INWAIT:
		BCT_CLIENT_StartMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_NONE );	// �}���m�[�����܂킵�Ă���
		if( WIPE_SYS_EndCheck() ){
			// �e�Ȃ�Q�[���X�^�[�g�𑗐M
			if( p_wk->netid == COMM_PARENT_ID ){
				CommSendData( CNM_BCT_START, NULL,  0 );
			}
			(*p_seq) ++;
		}
		break;

	case BCT_MAINSEQ_STARTWAIT:
		BCT_CLIENT_StartMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_NONE );	// �}���m�[�����܂킵�Ă���
		if( p_wk->start ){
			(*p_seq) ++;
		}
		break;

	case BCT_MAINSEQ_COUNTDOWN:	// �J�E���g�_�E������
		result = BCT_CLIENT_StartMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_COUNT_START );	// �J�E���g�_�E��
		if( result == FALSE ){
			(*p_seq) ++;
		}
		break;

	case BCT_MAINSEQ_MAIN:
		if( (p_wk->end) ){
			u32 score;
			score = BCT_CLIENT_ScoreGet( p_wk->p_client );
			// �����̃X�R�A�𑗐M
			CommSendData( CNM_BCT_SCORE, &score, sizeof(u32) );

			(*p_seq) ++;
			break;
		}

		
		if( p_wk->netid == COMM_PARENT_ID ){
			result = BCT_SURVER_Main( p_wk->p_surver );

			// �Q�[�����x���̕ύX�𑗐M
			if( BCT_SURVER_CheckGameLevelChange( p_wk->p_surver ) ){
				u32 level;
				level = BCT_SURVER_GetGameLevel( p_wk->p_surver );
				CommSendData( CNM_BCT_GAME_LEVEL, &level, sizeof(u32) );

				// ���M�����̂Ńt���OOFF
				BCT_SURVER_ClearGameLevelChange( p_wk->p_surver );
			}
			
			// �I���̑��M
			if( result == FALSE ){
				CommSendData( CNM_BCT_END, NULL,  0 );
			}
#ifdef BCT_DEBUG_NOT_TIMECOUNT
			if( sys.trg & PAD_BUTTON_CANCEL ){	
				CommSendData( CNM_BCT_END, NULL,  0 );
			}
#endif	// BCT_DEBUG_NOT_TIMECOUNT
		}
		BCT_CLIENT_Main( p_wk->p_client );

		// �؂̎��f�[�^���M
		BCT_ClientNutsSend( p_wk );

		// �r���o�߂̑��M�`�F�b�N
		BCT_ClientMiddleScoreSend( p_wk );
		break;

	case BCT_MAINSEQ_SCORECOMM:
		BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_NONE );	
		if( p_wk->netid == COMM_PARENT_ID ){
			// �e�݂͂�Ȃ��瓾�_�����W���Ă݂�Ȃɑ���
			result = BCT_SURVER_ScoreAllUserGetCheck( p_wk->p_surver );
			if( result == TRUE ){
				BCT_SCORE_COMM score;
				BCT_SURVER_ScoreGet( p_wk->p_surver, &score );
				CommSendData( CNM_BCT_ALLSCORE, &score, sizeof(BCT_SCORE_COMM) );
				(*p_seq) ++;
			}
		}else{
			// �N���C�A���g�͂����Ɍ��ʎ�M����
			(*p_seq) ++;
		}
		break;

	case BCT_MAINSEQ_SCORECOMMWAIT:
		BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_STARTEVENT_NONE );	
		if( p_wk->score_get == TRUE ){
			(*p_seq) ++;
		}
		break;
		
	case BCT_MAINSEQ_RESULT:
		// ���ʔ��\
		result = BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_ENDEVENT_RESULTON );	
		if( result == FALSE ){
			(*p_seq)++;
		}
		break;

	case BCT_MAINSEQ_WIPE_OUT:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_DOOROUT, 
				WIPE_TYPE_DOOROUT, WIPE_FADE_OUTCOLOR, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BUCKET );
		BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_ENDEVENT_RESULTON );	
		(*p_seq) ++;
		break;

	case BCT_MAINSEQ_WIPE_OUTWAIT:
		BCT_CLIENT_EndMain( p_wk->p_client, BCT_CLIENT_ENDEVENT_RESULTON );	
		if( WIPE_SYS_EndCheck() ){

			// �I�������J�n
			CommTimingSyncStart(BCT_SYNCID_END);
			(*p_seq) ++;
		}
		break;

	case BCT_MAINSEQ_DELETE:
		// ��������������܂ő҂�
		if(!CommIsTimingSync(BCT_SYNCID_END)){
			TOMOYA_PRINT( "sync_wait\n" );
			return PROC_RES_CONTINUE;
		}

		// VChatOff
		if( pp->vchat ){
			// �{�C�X�`���b�g�I��
			mydwc_stopvchat();
			TOMOYA_PRINT( "vct off\n" );
		}

		if( p_wk->netid == COMM_PARENT_ID ){
			BCT_SURVER_Delete( p_wk->p_surver );
		}
		BCT_CLIENT_Delete( p_wk->p_client );

		sys_VBlankFuncChange( NULL, NULL );	// VBlank�Z�b�g
		sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

		//
		DellVramTransferManager();

		(*p_seq)++;
		break;


	case BCT_MAINSEQ_RESULT_INIT:
		{
			u32 rank;

			// ���ʌv�Z
			MNGM_RESULT_CalcRank( &p_wk->result_param, p_wk->enres_param.num );

			// ���ʂ���K�W�F�b�g�A�b�v�`�F�b�N
			rank = p_wk->result_param.result[ p_wk->plno ];
			
		}
#ifdef BCT_DEBUG_ENTRY_CHG
		switch(DEBUG_ENTRY_COUNT){
		case 0:
			TOMOYA_PRINT( "slow\n" );
			p_wk->p_result = MNGM_RESULT_InitBallSlow( &p_wk->enres_param, &p_wk->result_param, HEAPID_BUCKET );
			break;

		case 1:
			TOMOYA_PRINT( "balance\n" );
			p_wk->p_result = MNGM_RESULT_InitBalanceBall( &p_wk->enres_param, &p_wk->result_param, HEAPID_BUCKET );
			break;

		case 2:
			TOMOYA_PRINT( "balloon\n" );
			p_wk->result_param.balloon = 14;
			p_wk->p_result = MNGM_RESULT_InitBalloon( &p_wk->enres_param, &p_wk->result_param, HEAPID_BUCKET );
			break;
		}

		DEBUG_ENTRY_COUNT  = (DEBUG_ENTRY_COUNT + 1) % 3;
#else
	p_wk->p_result = MNGM_RESULT_InitBallSlow( &p_wk->enres_param, &p_wk->result_param, HEAPID_BUCKET );
#endif
		(*p_seq)++;
		break;
		
	case BCT_MAINSEQ_RESULT_WAIT:
		{
			BOOL replay;
			
			if( MNGM_RESULT_Wait( p_wk->p_result ) == TRUE ){
				
				// replay�`�F�b�N
				replay = MNGM_RESULT_GetReplay( p_wk->p_result );
				
				MNGM_RESULT_Exit( p_wk->p_result );
				if( replay == FALSE ){
					return PROC_RES_FINISH;
				}else{
					(*p_seq) = BCT_MAINSEQ_ENTRY_INIT;
				}
			}
		}
		break;
	}

	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�o�P�b�g�Q�[���v���Z�X�@�j��
 *
 *	@param	p_proc		���[�N
 *	@param	p_seq		�V�[�P���X
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval	PROC_RES_FINISH,			///<����I��
 */
//-----------------------------------------------------------------------------
PROC_RESULT BucketProc_End( PROC* p_proc, int* p_seq )
{
	BUCKET_WK* p_wk = PROC_GetWork( p_proc );
	BUCKET_PROC_WORK* pp = PROC_GetParentWork(p_proc);

	switch( *p_seq ){
	case 0:
		// �Q�[���\���f�[�^�j��
		BCT_GAMEDATA_Release( p_wk );

		// ���[�N�j��
		PROC_FreeWork( p_proc );

		// �q�[�v�j��
		sys_DeleteHeap( HEAPID_BUCKET );

		CommStateSetErrorCheck(FALSE,FALSE);

		// �ʐM����
		CommTimingSyncStart(BCT_SYNCID_ERR_END);
		(*p_seq)++;
		break;

	case 1:
		if(!CommIsTimingSync(BCT_SYNCID_ERR_END)){
			return PROC_RES_FINISH;
		}
		break;

	default:
		break;
	}
	

	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�o�P�b�g�J�n
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void Bucket_StartSet( BUCKET_WK* p_wk )
{
	p_wk->start = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�o�P�b�g�I��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void Bucket_EndSet( BUCKET_WK* p_wk )
{
	p_wk->end = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�o�[�ɖ؂̎��f�[�^��n��
 *
 *	@param	p_wk		���[�N
 *	@param	cp_data		�؂̎��f�[�^
 *	@param	netid		�l�b�gID
 */
//-----------------------------------------------------------------------------
void Bucket_SurverNutsSet( BUCKET_WK* p_wk, const BCT_NUT_COMM* cp_data, u32 netid )
{
	u32 pl_no;
	pl_no = MNGM_ENRES_PARAM_GetNetIDtoPlNO( &p_wk->enres_param, netid );	// �v���C���[NO�ɂ��ēn��
	BCT_SURVER_SetNutData( p_wk->p_surver, cp_data, pl_no );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�N���C�A���g�ɖ؂̎��f�[�^��n��
 *
 *	@param	p_wk		���[�N
 *	@param	cp_data		�؂̎��f�[�^
 *	@param	netid		�l�b�gID
 */
//-----------------------------------------------------------------------------
void Bucket_ClientNutsSet( BUCKET_WK* p_wk, const BCT_NUT_COMM* cp_data, u32 netid )
{
	BCT_CLIENT_NutsDataSet( p_wk->p_client, cp_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�[�o�[���[�N�ɃX�R�A��ݒ肷��
 *
 *	@param	p_wk		���[�N
 *	@param	score		�X�R�A
 *	@param	netid		�l�b�gID
 */
//-----------------------------------------------------------------------------
void Bucket_SurverScoreSet( BUCKET_WK* p_wk, u32 score, u32 netid )
{
	u32 pl_no;
	pl_no = MNGM_ENRES_PARAM_GetNetIDtoPlNO( &p_wk->enres_param, netid );	// �v���C���[NO�ɂ��ēn��
	if( p_wk->p_surver != NULL ){
		BCT_SURVER_ScoreSet( p_wk->p_surver, score, pl_no );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�S�����̃X�R�A����M
 *
 *	@param	p_wk		���[�N
 *	@param	cp_data		�f�[�^
 *	@param	netid		�l�b�gID
 */
//-----------------------------------------------------------------------------
void Bucket_ClientAllScoreSet( BUCKET_WK* p_wk, const BCT_SCORE_COMM* cp_data, u32 netid )
{
	BCT_CLIENT_AllScoreSet( p_wk->p_client, cp_data );
	p_wk->score_get = TRUE;


	// �X�R�A���猋�ʔ��\�p�����[�^���쐬����
	{
		int i;
		for( i=0; i<BCT_PLAYER_NUM; i++ ){
			p_wk->result_param.score[i]		= cp_data->score[i];
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�N���C�A���g���[�N�ɃQ�[���f�[�^�̒i�K��ݒ�
 *
 *	@param	p_wk	���[�N
 *	@param	idx		�Q�[���f�[�^�̒i�K
 */
//-----------------------------------------------------------------------------
void Bucket_ClientGameDataIdxSet( BUCKET_WK* p_wk, u32 idx )
{
	BCT_CLIENT_GameDataIdxSet( p_wk->p_client, idx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�N���C�A���g�ɓr���̓��_��ݒ肷��
 *
 *	@param	p_wk		���[�N
 *	@param	score		���_
 *	@param	netid		�l�b�gID
 */
//-----------------------------------------------------------------------------
void Bucket_ClientMiddleScoreSet( BUCKET_WK* p_wk, u32 score, u32 netid )
{
	u32 pl_no;
	pl_no = MNGM_ENRES_PARAM_GetNetIDtoPlNO( &p_wk->enres_param, netid );	// �v���C���[NO�ɂ��ēn��
	BCT_CLIENT_MiddleScoreSet( p_wk->p_client, score, pl_no );
}


//-----------------------------------------------------------------------------
/**
 *				�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	VBlank�֐�
 */
//-----------------------------------------------------------------------------
static void BCT_VBlank( void* p_work )
{
	BUCKET_WK* p_wk = p_work;

	BCT_CLIENT_VBlank( p_wk->p_client );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�N���C�A���g�̖؂̎��f�[�^�𑗐M����
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void BCT_ClientNutsSend( BUCKET_WK* p_wk )
{
	static u8 timing = 10;
	static u8 count = 0;
	BCT_NUT_COMM comm;
	
#if 1
	while( BCT_CLIENT_NutsDataGet( p_wk->p_client, &comm ) == TRUE ){
		CommSendData( CNM_BCT_NUTS, &comm, sizeof(BCT_NUT_COMM) );
	}
#else
	// ����^�C�~���O�ő��M����
	count ++;
	if( count >= timing ){
		comm.pl_no = p_wk->plno;
		comm.in_flag = TRUE;
		comm.way.x = 2251;
		comm.way.y = 3422;
		comm.way.z = 3;
		comm.power = 17879;
		comm.mat.x = -266;
		comm.mat.y = 0;
		comm.mat.z = 2;
			
		count = 0;
		CommSendData( CNM_BCT_NUTS, &comm, sizeof(BCT_NUT_COMM) );
	}

	if( sys.trg & PAD_KEY_UP ){
		timing ++;
		OS_Printf( "send timing %d\n", timing );
	}else if( sys.trg & PAD_KEY_DOWN ){
		timing --;
		OS_Printf( "send timing %d\n", timing );
	}
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�r���o�ߓ��_�𑗐M����
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void BCT_ClientMiddleScoreSend( BUCKET_WK* p_wk )
{
	s32 time;
	u32 score;

	time = BCT_CLIENT_GetTime( p_wk->p_client );

	// �ŏ��͑���Ȃ�
	if( time == 0 ){
		return;
	}

	if( (time % BCT_MIDDLE_SCORE_SEND_TIMING) == 0 ){

		// ���M
		score = BCT_CLIENT_ScoreGet( p_wk->p_client );
		CommSendData( CNM_BCT_MIDDLESCORE, &score, sizeof(u32) );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���\���f�[�^�ǂݍ���
 *
 *	@param	p_wk		���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void BCT_GAMEDATA_Load( BUCKET_WK* p_wk, u32 heapID )
{
	u32 size;
	int i;
	
	p_wk->gamedata.p_tbl	= ArcUtil_LoadEx( ARC_BUCKET_DATA, NARC_ballslow_data_bucket_data_bin, FALSE, heapID, ALLOC_TOP, &size );
	p_wk->gamedata.tblnum	= size / sizeof(BCT_GAMEDATA_ONE);

	// MYSTATUSTBL
	for( i=0; i<p_wk->enres_param.num; i++ ){
		p_wk->gamedata.cp_status[i]	= CommInfoGetMyStatus( MNGM_ENRES_PARAM_GetPlNOtoNetID( &p_wk->enres_param, i ) );
		p_wk->gamedata.vip[i]		= MNGM_ENRES_PARAM_GetVipFlag( &p_wk->enres_param, i );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���\���f�[�^�j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void BCT_GAMEDATA_Release( BUCKET_WK* p_wk )
{
	sys_FreeMemoryEz( p_wk->gamedata.p_tbl );
	p_wk->gamedata.p_tbl	= NULL;
	p_wk->gamedata.tblnum	= 0;
}


