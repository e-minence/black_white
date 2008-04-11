//============================================================================================
/**
 * @file	scr_stage.c
 * @bfief	�X�N���v�g�R�}���h�F�o�g���X�e�[�W(��t�܂��)
 * @author	Satoshi Nohara
 * @date	06.06.13
 */
//============================================================================================
#include "common.h"

#include <nitro/code16.h> 
#include "system/pm_str.h"
#include "system\msgdata.h"			//MSGMAN_GetString
#include "system\wordset.h"			//WordSet_RegistPlayerName
#include "system/bmp_menu.h"
#include "system/bmp_list.h"
#include "system/get_text.h"
#include "system/lib_pack.h"
#include "poketool/poke_number.h"	//PMNumber_GetMode
#include "savedata/sp_ribbon.h"		//SaveData_GetSpRibbon
#include "savedata/frontier_savedata.h"
#include "gflib/strbuf_family.h"	//����
//�t�B�[���h
#include "fieldsys.h"
#include "field_subproc.h"
//�X�N���v�g
#include "script.h"
#include "scrcmd.h"
#include "scrcmd_def.h"
#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "sysflag.h"
#include "syswork.h"
#include "scr_tool.h"
//�o�g���X�e�[�W
#include "scr_stage.h"
#include "scr_stage_sub.h"
#include "savedata/stage_savedata.h"
#include "../frontier/stage_def.h"
//�ʐM
#include "communication/comm_def.h"
#include "communication/comm_tool.h"
#include "communication/comm_system.h"

//#include "../frontier/comm_command_frontier.h"
#include "comm_command_field.h"
#include "frontier/frontier_tool.h"


//============================================================================================
//
//	�\���̐錾
//
//============================================================================================
///�|�P�����I���C�x���g���[�N
typedef struct _STAGE_POKESEL_EVENT{
	int	seq;
	u8	plist_type;							//�|�P�������X�g�^�C�v
	u8	pos;								//�|�P�������X�g�̌��݃J�[�\���ʒu
	u8	sel[STAGE_ENTRY_POKE_MAX];
	void** sp_wk;
}STAGE_POKESEL_EVENT;

///�|�P�����I���C�x���g�V�[�P���XID
typedef enum{
	STAGE_POKESEL_PLIST_CALL,				//�|�P�������X�g�Ăяo��
	STAGE_POKESEL_PLIST_WAIT,				//�|�P�������X�g�I���҂�
	STAGE_POKESEL_PST_CALL,					//�|�P�����X�e�[�^�X�Ăяo��
	STAGE_POKESEL_PST_WAIT,					//�|�P�����X�e�[�^�X�I���҂�
	STAGE_POKESEL_EXIT,						//�I��
};


//============================================================================================
//
//	�v���g�^�C�v�錾
//
//============================================================================================
BOOL EvCmdBattleStageTools(VM_MACHINE* core);
static BOOL StageScrTools_CheckEntryPokeNum(u16 num,SAVEDATA *savedata);
BOOL EvCmdBattleStageSetContinueNG( VM_MACHINE * core );

BOOL EvCmdBattleStageCommMonsNo(VM_MACHINE* core);
void EventCall_StageComm( GMEVENT_CONTROL* event, u16 monsno, u16* ret_wk );
static BOOL GMEVENT_StageComm( GMEVENT_CONTROL* event );

//�|�P�������X�g���|�P�����X�e�[�^�X
void EventCmd_StagePokeSelectCall( GMEVENT_CONTROL *event, void** buf, u8 plist_type );
static BOOL BtlStageEv_PokeSelect( GMEVENT_CONTROL *ev );
static int BtlStage_PokeListCall( STAGE_POKESEL_EVENT* wk,FIELDSYS_WORK* fsys,int heapID );
static int BtlStage_PokeListWait( STAGE_POKESEL_EVENT* wk, FIELDSYS_WORK* fsys );
static int BtlStage_PokeStatusCall(STAGE_POKESEL_EVENT* wk,FIELDSYS_WORK* fsys,int heapID );
static int BtlStage_PokeStatusWait( STAGE_POKESEL_EVENT* wk, FIELDSYS_WORK* fsys );


//============================================================================================
//
//	�R�}���h
//
//============================================================================================

//--------------------------------------------------------------
/**
 *	@brief	�o�g���X�e�[�W�p�R�}���h�Q�Ăяo���C���^�[�t�F�[�X
 *
 *	@param	com_id		u16:�R�}���hID
 *	@param	retwk_id	u16:�Ԃ�l���i�[���郏�[�N��ID
 */
//--------------------------------------------------------------
BOOL EvCmdBattleStageTools(VM_MACHINE* core)
{
	u8 plist_type;
	u8 buf8[4];
	u16 buf16[4];
	u16	com_id,param,retwk_id;
	u16* ret_wk,*prm_wk;
	void** buf;
	BTOWER_SCRWORK* wk;
	STAGEDATA* stage_sv;
	STAGESCORE* score_sv;

	com_id	= VMGetU16(core);
	param	= VMGetWorkValue(core);
	retwk_id= VMGetU16(core);
	ret_wk	= GetEventWorkAdrs( core->fsys, retwk_id );

	stage_sv= SaveData_GetStageData( core->fsys->savedata );
	score_sv= SaveData_GetStageScore( core->fsys->savedata );
	buf		= GetEvScriptWorkMemberAdrs( core->fsys, ID_EVSCR_SUBPROC_WORK );

	switch( com_id ){

	//0:�Q���\�ȃ|�P�������̃`�F�b�N(�V���O����p�ȏ����ɂȂ��Ă���)
	case STAGE_TOOL_CHK_ENTRY_POKE_NUM:
		*ret_wk = StageScrTools_CheckEntryPokeNum( param, core->fsys->savedata );
		break;

	//1:�A�������擾
	case STAGE_TOOL_GET_CLEAR_FLAG:
		//WIFI�̂ݓ���
		if( param == STAGE_TYPE_WIFI_MULTI ){
			*ret_wk = FrontierRecord_Get(SaveData_GetFrontier(core->fsys->savedata), 
										FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
										Frontier_GetFriendIndex(FRID_STAGE_MULTI_WIFI_CLEAR_BIT) );
		}else{
			*ret_wk = (u16)STAGESCORE_GetScoreData( score_sv, STAGESCORE_ID_CLEAR_FLAG, 
													param, 0, NULL);
		}
		break;

	//2:�A�����̃|�P�����i���o�[���擾
	case STAGE_TOOL_GET_CLEAR_MONSNO:
		*ret_wk = FrontierRecord_Get(SaveData_GetFrontier(core->fsys->savedata), 
									StageScr_GetMonsNoRecordID(param),
									Frontier_GetFriendIndex(StageScr_GetMonsNoRecordID(param)) );
		break;

	//3:�A�����t���O�N���A�A�A�����R�[�h�N���A�A�^�C�v�J�E���g�N���A
	case STAGE_TOOL_SET_NEW_CHALLENGE:
		BattleStageSetNewChallenge( core->fsys->savedata, score_sv, param );
		break;

	//4:�|�P�������X�g��ʌĂяo��
	case STAGE_TOOL_SELECT_POKE:

		//�o�g���^�C�v�ɂ���ĕ���
		if( param == STAGE_TYPE_SINGLE ){
			plist_type = PL_TYPE_SINGLE;
		}else if( param == STAGE_TYPE_DOUBLE ){
			plist_type = PL_TYPE_DOUBLE;
		}else if( param == STAGE_TYPE_MULTI ){
			plist_type = PL_TYPE_MULTI;
		}else{
			plist_type = PL_TYPE_MULTI;
		}

		OS_Printf( "stage_type = %d\n", param );
		EventCmd_StagePokeSelectCall( core->event_work, buf, plist_type );
		return 1;

	default:
		OS_Printf( "�n���ꂽcom_id = %d\n", com_id );
		GF_ASSERT( (0) && "com_id�����Ή��ł��I" );
		*ret_wk = 0;
		break;
	}

	return 0;
}

//--------------------------------------------------------------
/**
 *	@brief	�o�g���X�e�[�W�@�Q���\�ȃ|�P�������̃`�F�b�N
 *
 *	@param	num		�Q���ɕK�v�ȃ|�P������
 *
 *	@retval	true	�Q���\
 *	@retval	false	�Q���s��
 *
 *	�^�}�S�łȂ��āA�`���n�ȊO�����C���邩
 */
//--------------------------------------------------------------
static BOOL StageScrTools_CheckEntryPokeNum(u16 num,SAVEDATA *savedata)
{
	u8 i,j,ct,pokenum;
	u16	monsno;
	POKEPARTY* party;
	POKEMON_PARAM* pp;
	
	party	= SaveData_GetTemotiPokemon( savedata );
	pokenum = PokeParty_GetPokeCount( party );

	//�莝���̐����K�v�ȃ|�P��������������
	if( pokenum < num ){
		return FALSE;
	}

	for( i=0, ct=0; i < pokenum ;i++ ){
		pp		= PokeParty_GetMemberPointer( party, i );
		monsno	= PokeParaGet( pp, ID_PARA_monsno, NULL );		//�|�P�����i���o�[
		
		//�^�}�S�łȂ����`�F�b�N
		if( PokeParaGet(pp,ID_PARA_tamago_flag,NULL) != 0 ){
			continue;
		}

		//�o�g���^���[�ɎQ���ł��Ȃ��|�P���������`�F�b�N
		if( BattleTowerExPokeCheck_MonsNo(monsno) == TRUE ){
			continue;
		}

		ct++;
	}

	//������ʉ߂����|�P���������`�F�b�N
	if( ct < num ){
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���v�g�R�}���h�F�o�g���X�e�[�W�p��NG�Z�b�g
 * @param		core
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdBattleStageSetContinueNG( VM_MACHINE * core )
{
	STAGESCORE* score_wk;
	u16 type = VMGetWorkValue(core);

	score_wk = SaveData_GetStageScore( core->fsys->savedata );

	BattleStageSetNewChallenge( core->fsys->savedata, score_wk, type );
	return 0;
}


//============================================================================================
//
//	�ʐM
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 *	@brief	�ʐM�}���`�f�[�^���M
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdBattleStageCommMonsNo(VM_MACHINE* core)
{
	u16 monsno	= VMGetWorkValue( core );
	u16* ret_wk	= VMGetWork( core );

	OS_Printf( "�o�g���X�e�[�W�ʐM�}���`�f�[�^���M\n" );
	OS_Printf( "�|�P�����i���o�[ monsno = %d\n", monsno );
	
	EventCall_StageComm( core->event_work, monsno, ret_wk );
	return 1;
}
	
//--------------------------------------------------------------------------------------------
/**
 * @brief	�C�x���g�[���R�}���h�F����M
 *
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @param	monsno		�|�P�����i���o�[
 */
//--------------------------------------------------------------------------------------------
void EventCall_StageComm( GMEVENT_CONTROL* event, u16 monsno, u16* ret_wk )
{
	FLDSCR_STAGE_COMM* comm_wk;

	comm_wk = sys_AllocMemory( HEAPID_WORLD, sizeof(FLDSCR_STAGE_COMM) );
	memset( comm_wk, 0, sizeof(FLDSCR_STAGE_COMM) );

	comm_wk->mine_monsno = monsno;
	comm_wk->ret_wk = ret_wk;
	//CommCommandFrontierInitialize( comm_wk );
	CommCommandFieldInitialize( comm_wk );

	FieldEvent_Call( event, GMEVENT_StageComm, comm_wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	����M
 *
 * @param	event		GMEVENT_CONTROL�^
 *
 * @retval	"FALSE = ���s��"
 * @retval	"TRUE = ���s�I��"
 */
//--------------------------------------------------------------------------------------------
static BOOL GMEVENT_StageComm( GMEVENT_CONTROL* event )
{
	FLDSCR_STAGE_COMM* comm_wk = FieldEvent_GetSpecialWork( event );

	switch( comm_wk->seq ){

	case 0:
		if( CommSendData(CF_STAGE_MONSNO,comm_wk,sizeof(FLDSCR_STAGE_COMM)) == TRUE ){
			comm_wk->seq++;
		}
		break;
		
	case 1:
		if( comm_wk->recieve_count >= STAGE_COMM_PLAYER_NUM ){

			//�����|�P������I��ł��邩�`�F�b�N
			if( comm_wk->mine_monsno == comm_wk->pair_monsno ){
				*comm_wk->ret_wk = 0;
			}else{
				*comm_wk->ret_wk = 1;
			}

			OS_Printf( "comm_wk->mine_monsno = %d\n", comm_wk->mine_monsno );
			OS_Printf( "comm_wk->pair_monsno = %d\n", comm_wk->pair_monsno );
			OS_Printf( "*comm_wk->ret_wk = %d\n", *comm_wk->ret_wk );
			comm_wk->seq++;
		}
		break;

	case 2:
		sys_FreeMemoryEz( comm_wk );
		return TRUE;
	};

	return FALSE;
}


//============================================================================================
//
//	�|�P�������X�g���|�P�����X�e�[�^�X
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	�|�P�����I���@�T�u�C�x���g�Ăяo��
 *
 * @param	event	GMEVENT_CONTROL*
 *
 * @retval	nothing
 */
//--------------------------------------------------------------
void EventCmd_StagePokeSelectCall( GMEVENT_CONTROL *event, void** buf, u8 plist_type )
{
	FIELDSYS_WORK* fsys		= FieldEvent_GetFieldSysWork( event );
	STAGE_POKESEL_EVENT* wk = sys_AllocMemory( HEAPID_WORLD, sizeof(STAGE_POKESEL_EVENT) );
	MI_CpuClear8( wk, sizeof(STAGE_POKESEL_EVENT) );

	wk->plist_type	= plist_type;
	wk->sp_wk		= buf;

	FieldEvent_Call( fsys->event, BtlStageEv_PokeSelect, wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�Q�[���C�x���g�R���g���[���@�|�P�������X�g���X�e�[�^�X
 *
 * @param	ev	GMEVENT_CONTROL *
 *
 * @retval	BOOL	TRUE=�C�x���g�I��
 */
//--------------------------------------------------------------------------------------------
static BOOL BtlStageEv_PokeSelect( GMEVENT_CONTROL *ev )
{
	FIELDSYS_WORK* fsys		= FieldEvent_GetFieldSysWork( ev );
	STAGE_POKESEL_EVENT* wk	= FieldEvent_GetSpecialWork( ev );

	switch( wk->seq ){

	//�|�P�������X�g�Ăяo��
	case STAGE_POKESEL_PLIST_CALL:
		wk->seq = BtlStage_PokeListCall( wk, fsys, HEAPID_WORLD );
		break;

	//�|�P�������X�g�I���҂�
	case STAGE_POKESEL_PLIST_WAIT:
		wk->seq = BtlStage_PokeListWait( wk, fsys );
		break;

	//�|�P�����X�e�[�^�X�Ăяo��
	case STAGE_POKESEL_PST_CALL:
		wk->seq = BtlStage_PokeStatusCall( wk, fsys, HEAPID_WORLD );
		break;

	//�|�P�����X�e�[�^�X�I���҂�
	case STAGE_POKESEL_PST_WAIT:
		wk->seq = BtlStage_PokeStatusWait( wk, fsys );
		break;

	//�I��
	case STAGE_POKESEL_EXIT:
		sys_FreeMemoryEz( wk );		//STAGE_POKESEL_EVENT���J��
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	�o�g���X�e�[�W�@�|�P�������X�g�Ăяo��
 */
//--------------------------------------------------------------------------------------------
static int BtlStage_PokeListCall( STAGE_POKESEL_EVENT* wk,FIELDSYS_WORK* fsys,int heapID )
{
	u8 i;

	//PLIST_DATA* pld = PLISTDATA_Create(HEAPID_WORLD, fsys, wk->plist_type, PL_MODE_BATTLE_STAGE);
	PLIST_DATA* pld = sys_AllocMemory( HEAPID_WORLD, sizeof(PLIST_DATA) );
	MI_CpuClearFast( pld, sizeof(PLIST_DATA) );

	//PILSTDATA_Create�̒��g�Ɠ���
	pld->pp			= SaveData_GetTemotiPokemon( fsys->savedata );
	pld->myitem		= SaveData_GetMyItem( fsys->savedata );
	pld->mailblock	= SaveData_GetMailBlock( fsys->savedata );
	pld->cfg		= SaveData_GetConfig( fsys->savedata );

	//�^�C�v�̓V���O���Œ�ł悳����(�m�F����)
	//pld->type		= wk->plist_type;
	pld->type		= PL_TYPE_SINGLE;

	pld->mode		= PL_MODE_BATTLE_STAGE;
	pld->fsys		= fsys;

	//�J�[�\���ʒu
	pld->ret_sel	= wk->pos;

	//�I�����Ă���ʒu(���X�g���X�e�[�^�X�����X�g�ŏ�Ԃ𕜋A������)
	for( i=0; i < STAGE_ENTRY_POKE_MAX ;i++ ){
		pld->in_num[i] = wk->sel[i];
	}

	pld->in_min		= 1;			//�Q���ŏ���
	pld->in_max		= 1;			//�Q���ő吔

	if( wk->plist_type == PL_TYPE_DOUBLE ){
		pld->in_min = 2;			//�Q���ŏ���
		pld->in_max = 2;			//�Q���ő吔
	}

	GameSystem_StartSubProc( fsys, &PokeListProcData, pld );

	*(wk->sp_wk) = pld;

	return STAGE_POKESEL_PLIST_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	�|�P�������X�g�I���҂�
 */
//--------------------------------------------------------------------------------------------
static int BtlStage_PokeListWait( STAGE_POKESEL_EVENT* wk, FIELDSYS_WORK* fsys )
{
	int	ret;
	PLIST_DATA* pld;

	// �T�u�v���Z�X�I���҂�
	if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ) {
		return STAGE_POKESEL_PLIST_WAIT;
	}

	pld = *(wk->sp_wk);

	//�f�[�^�擾
	switch( pld->ret_sel ){

	case PL_SEL_POS_EXIT:					//��߂�(pld�͊J�����Ă��Ȃ�)
		return STAGE_POKESEL_EXIT;

	case PL_SEL_POS_ENTER:					//����(pld�͊J�����Ă��Ȃ�)
		return STAGE_POKESEL_EXIT;

	default:								//�悳���݂�
		break;
	}

	//�I�����Ă����Ԃ��A�|�P�����X�e�[�^�X���Ă񂾂��Ƃɕ��A�����邽�߂ɕK�v
	MI_CpuCopy8( pld->in_num, wk->sel, STAGE_ENTRY_POKE_MAX );	//���ݑI�΂�Ă���|�P������ۑ�

	//�|�W�V������ۑ�
	wk->pos = pld->ret_sel;

	sys_FreeMemoryEz(pld);
	*(wk->sp_wk) = NULL;
	return	STAGE_POKESEL_PST_CALL;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	�o�g���X�e�[�W�@�|�P�����X�e�[�^�X�Ăяo��
 */
//--------------------------------------------------------------------------------------------
static int BtlStage_PokeStatusCall(STAGE_POKESEL_EVENT* wk,FIELDSYS_WORK* fsys,int heapID )
{
	PSTATUS_DATA* psd;
	SAVEDATA* sv;

	static const u8 PST_PageTbl[] = {
		PST_PAGE_INFO,			//�u�|�P�������傤�ق��v
		PST_PAGE_MEMO,			//�u�g���[�i�[�����v
		PST_PAGE_PARAM,			//�u�|�P�����̂���傭�v
		PST_PAGE_CONDITION,		//�u�R���f�B�V�����v
		PST_PAGE_B_SKILL,		//�u���������킴�v
		PST_PAGE_C_SKILL,		//�u�R���e�X�g�킴�v
		PST_PAGE_RIBBON,		//�u���˂񃊃{���v
		PST_PAGE_RET,			//�u���ǂ�v
		PST_PAGE_MAX
	};
	
	sv = fsys->savedata;

	//�|�P�����X�e�[�^�X���Ăяo��
	psd = sys_AllocMemoryLo( heapID, sizeof(PSTATUS_DATA) );
	MI_CpuClear8( psd,sizeof(PSTATUS_DATA) );

	psd->cfg		= SaveData_GetConfig( sv );
	psd->ppd		= SaveData_GetTemotiPokemon( sv );
	psd->zukan_mode	= PMNumber_GetMode( sv );
	psd->ev_contest	= PokeStatus_ContestFlagGet( sv );
	psd->ppt		= PST_PP_TYPE_POKEPARTY;
	psd->pos		= wk->pos;
	psd->max		= (u8)PokeParty_GetPokeCount( psd->ppd );
	psd->waza		= 0;
	psd->mode		= PST_MODE_NORMAL;
	psd->ribbon		= SaveData_GetSpRibbon( sv );

	PokeStatus_PageSet( psd, PST_PageTbl );
	PokeStatus_PlayerSet( psd, SaveData_GetMyStatus(sv) );
	
	GameSystem_StartSubProc( fsys, &PokeStatusProcData, psd );
	
	*(wk->sp_wk) = psd;
	return STAGE_POKESEL_PST_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	�o�g���X�e�[�W�@�|�P�����X�e�[�^�X�I���҂�
 */
//--------------------------------------------------------------------------------------------
static int BtlStage_PokeStatusWait( STAGE_POKESEL_EVENT* wk, FIELDSYS_WORK* fsys )
{
	PSTATUS_DATA* psd;

	//�T�u�v���Z�X�I���҂�
	if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ) {
		return STAGE_POKESEL_PST_WAIT;
	}

	psd = *(wk->sp_wk);
	
	//�؂�ւ���ꂽ�J�����g��ۑ�����
	wk->pos = psd->pos;

	sys_FreeMemoryEz( psd );
	*(wk->sp_wk) = NULL;

	return STAGE_POKESEL_PLIST_CALL;
}


