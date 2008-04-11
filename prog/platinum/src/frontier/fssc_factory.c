//============================================================================================
/**
 * @file	fssc_factory.c
 * @bfief	�t�����e�B�A�V�X�e���X�N���v�g�R�}���h�F�t�@�N�g���[
 * @author	Satoshi Nohara
 * @date	07.04.10
 */
//============================================================================================
#include "common.h"
#include <nitro/code16.h> 
#include "system/pm_str.h"
#include "system\msgdata.h"						//MSGMAN_GetString
#include "system/brightness.h"					//ChangeBrightnessRequest
#include "system\wordset.h"						//WordSet_RegistPlayerName
#include "system/bmp_menu.h"
#include "system/bmp_list.h"
#include "system/get_text.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/wipe.h"
#include "system/lib_pack.h"
#include "system/procsys.h"
#include "system/snd_tool.h"

#include "savedata/misc.h"
#include "savedata/b_tower.h"
#include "msgdata\msg.naix"						//NARC_msg_??_dat

#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "gflib/strbuf_family.h"				//����

#include "../field/field_battle.h"				//BattleParam_IsWinResult
#include "../field/field_subproc.h"				//TestBattleProcData
#include "../application/factory.h"

#include "frontier_types.h"
#include "frontier_main.h"
#include "frontier_map.h"
#include "frontier_scr.h"
#include "frontier_tool.h"
#include "frontier_scrcmd.h"
#include "frontier_scrcmd_sub.h"
#include "factory_tool.h"
#include "factory_def.h"
#include "fss_task.h"
#include "fssc_factory_sub.h"
#include "comm_command_frontier.h"

#include "field/eventflag.h"

#include "graphic/frontier_bg_def.h"


//============================================================================================
//
//	��`
//
//============================================================================================
#define FACTORY_SCROLL_MAP_RAIL_Y	(1)			//�}�b�v�̃��[���X�N���[�����x(256�ɂ����悤�ɂ���)
#define FACTORY_RAIL_TCB_PRI		(5)			//�}�b�v�̃��[��TCB�̃v���C�I���e�B


//============================================================================================
//
//	�v���g�^�C�v�錾
//
//============================================================================================
BOOL FSSC_FactoryWorkAlloc( FSS_TASK * core );
BOOL FSSC_FactoryWorkInit( FSS_TASK * core );
BOOL FSSC_FactoryWorkFree( FSS_TASK * core );
BOOL FSSC_FactoryRentalCall( FSS_TASK * core );
BOOL FSSC_FactoryCallGetResult( FSS_TASK * core );
BOOL FSSC_FactoryBattleCall( FSS_TASK * core );
BOOL FSSC_FactoryTradeCall( FSS_TASK * core );
BOOL FSSC_FactoryRentalPokePartySet( FSS_TASK * core );
BOOL FSSC_FactoryBtlAfterPokePartySet( FSS_TASK * core );
BOOL FSSC_FactoryTradePokeChange( FSS_TASK * core );
BOOL FSSC_FactoryTradeAfterPokePartySet( FSS_TASK * core );
BOOL FSSC_FactoryScrWork( FSS_TASK * core );
BOOL FSSC_FactoryLoseCheck( FSS_TASK * core );
BOOL FSSC_FactorySendBuf( FSS_TASK * core );
BOOL FSSC_FactoryRecvBuf( FSS_TASK * core );
static BOOL WaitFactoryRecvBuf( FSS_TASK * core );
void Factory_MapRailMoveMain( TCB_PTR tcb, void * work );
BOOL FSSC_FactoryTalkMsgAppear(FSS_TASK* core);

//�n�������[�N�̉������
static void FactoryFreeMemory( void *parent_work );


//============================================================================================
//
//	�R�}���h
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	�t�@�N�g���[���[�N�m��
 *
 * @param	core	���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryWorkAlloc( FSS_TASK * core ) 
{
	FACTORY_SCRWORK* wk;
	FRONTIER_EX_PARAM* ex_param;
	u16 init = FSSTGetWorkValue( core );
	u16 type = FSSTGetWorkValue( core );
	u16 level = FSSTGetWorkValue( core );

	ex_param = Frontier_ExParamGet( core->fss->fmain );

	wk = FactoryScr_WorkAlloc( ex_param->savedata, init, type, level );
	Frontier_SysWorkSet( core->fss->fmain, wk );

	OS_Printf( "�X�N���v�g�t�@�N�g���[���[�N�m��\n" );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�t�@�N�g���[���[�N������
 *
 * @param	core	���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryWorkInit( FSS_TASK * core ) 
{
	FACTORY_SCRWORK* bf_scr_wk;
	u16 init = FSSTGetWorkValue( core );

	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	FactoryScr_WorkInit( bf_scr_wk, init );

	OS_Printf( "�X�N���v�g�t�@�N�g���[���[�N������\n" );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�t�@�N�g���[���[�N�폜
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryWorkFree( FSS_TASK * core ) 
{
	FACTORY_SCRWORK* bf_scr_wk;
	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	FactoryScr_WorkRelease( bf_scr_wk );
	OS_Printf( "�X�N���v�g�t�@�N�g���[���[�N�J��\n" );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�R�}���h�F�t�@�N�g���[�����^���Ăяo��
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryRentalCall( FSS_TASK * core)
{
	int i;
	FACTORY_SCRWORK* bf_scr_wk;
	FACTORY_CALL_WORK* factory_call;
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );

	//�I�[�o�[���CID�錾
	FS_EXTERN_OVERLAY( factory );
	
	//�t�@�N�g���[�v���Z�X�f�[�^
	static const PROC_DATA FactoryProc = {	
		FactoryProc_Init,
		FactoryProc_Main,
		FactoryProc_End,
		FS_OVERLAY_ID( factory ),
	};

	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	OS_Printf( "�t�@�N�g���[�Ăяo��\n" );

	factory_call = sys_AllocMemory( HEAPID_WORLD, sizeof(FACTORY_CALL_WORK) );
	MI_CpuClear8( factory_call, sizeof(FACTORY_CALL_WORK) );
	factory_call->sv			= ex_param->savedata;
	factory_call->type			= bf_scr_wk->type;
	factory_call->level			= bf_scr_wk->level;
	factory_call->mode			= FACTORY_MODE_RENTAL;
	factory_call->p_m_party		= bf_scr_wk->p_m_party;
	factory_call->p_e_party		= bf_scr_wk->p_e_party;
	factory_call->p_work		= bf_scr_wk;

    Frontier_SubProcSet( core->fss->fmain, &FactoryProc, factory_call, FALSE, FactoryFreeMemory);
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�R�}���h�F�t�@�N�g���[�퓬���ʎ擾���ĊJ��
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryCallGetResult( FSS_TASK * core)
{
	FACTORY_SCRWORK* bf_scr_wk;
	BATTLE_PARAM* param;

	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	param = bf_scr_wk->p_work;

	bf_scr_wk->winlose_flag = BattleParam_IsWinResult( param->win_lose_flag );
	OS_Printf( "bf_scr_wk->winlose_flag = %d\n", bf_scr_wk->winlose_flag );

	//BATTLE_PARAM�̊J��
	BattleParam_Delete( param );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�R�}���h�F�퓬�Ăяo��
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"1"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryBattleCall( FSS_TASK * core)
{
	BATTLE_PARAM* param;
	FACTORY_SCRWORK* bf_scr_wk;
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );

	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	//�o�g���f�[�^������
	param = BtlFactory_CreateBattleParam( bf_scr_wk, ex_param );
	bf_scr_wk->p_work = param;

	//�퓬�؂�ւ�
//	Snd_DataSetByScene( SND_SCENE_BATTLE, SEQ_BA_TRAIN, 1 );		//�o�g���ȍĐ�
    Frontier_SubProcSet(core->fss->fmain, &TestBattleProcData, param, FALSE, NULL );
	OS_Printf( "�X�N���v�g�t�@�N�g���[�퓬�Ăяo��\n" );			//field_encount.c
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�R�}���h�F�t�@�N�g���[�����Ăяo��
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryTradeCall( FSS_TASK * core )
{
	int i;
	FACTORY_SCRWORK* bf_scr_wk;
	FACTORY_CALL_WORK* factory_call;
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );

	//�I�[�o�[���CID�錾
	FS_EXTERN_OVERLAY( factory );
	
	//�t�@�N�g���[�v���Z�X�f�[�^
	static const PROC_DATA FactoryProc = {	
		FactoryProc_Init,
		FactoryProc_Main,
		FactoryProc_End,
		FS_OVERLAY_ID( factory ),
	};

	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	OS_Printf( "�t�@�N�g���[�����Ăяo��\n" );

	//�퓬�Ăяo�����ɁA
	//POKEPARRY[0-2]�ɁA�莝�����Z�b�g����Ă���
	//POKEPARRY[3-5]�ɁA�G�|�P�������Z�b�g����Ă���
		
	//�N���A���Ă���n��
	for( i=0; i < FACTORY_RET_WORK_MAX ;i++ ){
		bf_scr_wk->ret_work[i] = 0;
	}

	factory_call = sys_AllocMemory( HEAPID_WORLD, sizeof(FACTORY_CALL_WORK) );
	MI_CpuClear8( factory_call, sizeof(FACTORY_CALL_WORK) );
	factory_call->sv			= ex_param->savedata;
	factory_call->type			= bf_scr_wk->type;
	factory_call->level			= bf_scr_wk->level;
	factory_call->mode			= FACTORY_MODE_TRADE;
	factory_call->p_m_party		= bf_scr_wk->p_m_party;
	factory_call->p_e_party		= bf_scr_wk->p_e_party;
	factory_call->p_work		= bf_scr_wk;

    Frontier_SubProcSet( core->fss->fmain, &FactoryProc, factory_call, FALSE, FactoryFreeMemory);
	return 1;
}

//�t�@�N�g���[��ʂœn�������[�N�̉������
static void FactoryFreeMemory( void *parent_work )
{
	int i;
	FACTORY_CALL_WORK* factory_call = parent_work;

	//���ʂ��擾
	FactoryScr_GetResult( factory_call->p_work, parent_work );

	//FACTORY_CALL_WORK�̍폜
	sys_FreeMemoryEz( parent_work );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�R�}���h�F�����^��POKEPARTY�̃Z�b�g
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryRentalPokePartySet( FSS_TASK * core )
{
	FACTORY_SCRWORK* bf_scr_wk;
	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	FactoryScr_RentalPartySet( bf_scr_wk );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�R�}���h�F�o�g�����POKEPARTY�̃Z�b�g
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryBtlAfterPokePartySet( FSS_TASK * core )
{
	FACTORY_SCRWORK* bf_scr_wk;
	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	FactoryScr_BtlAfterPartySet( bf_scr_wk );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�R�}���h�F�g���[�h������̃|�P�����ύX
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryTradePokeChange( FSS_TASK * core )
{
	FACTORY_SCRWORK* bf_scr_wk;
	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	FactoryScr_TradePokeChange( bf_scr_wk );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�R�}���h�F�g���[�h������̃|�P������POKEPARTY�̃Z�b�g
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryTradeAfterPokePartySet( FSS_TASK * core )
{
	FACTORY_SCRWORK* bf_scr_wk;
	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	FactoryScr_TradeAfterPartySet( bf_scr_wk );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�R�}���h�FFACTORY_SCR_WORK�̃Z�b�g�A�Q�b�g
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryScrWork( FSS_TASK * core )
{
	FMAP_PTR fmap;
	STRBUF* strbuf;
	POKEMON_PARAM* pp;
	FACTORY_SCRWORK* bf_scr_wk;
	u8 e_max,m_max;
	u32 type_tbl[9][2];
	u32 type,type_num;
	int i,j;
	u8 code		= FSSTGetU8( core );
	u8 param1	= FSSTGetU8( core );
	u8 param2	= FSSTGetU8( core );
	u16* work	= FSSTGetWork( core );

	bf_scr_wk	= Frontier_SysWorkGet( core->fss->fmain );
	fmap		= FSS_GetFMapAdrs( core->fss );

	switch( code ){

	//���[�h(�����^���A����)�̃Z�b�g
	case FA_ID_SET_MODE:
		bf_scr_wk->mode = param1;
		break;

	//���x��(LV50�A�I�[�v��)�̃Z�b�g
	case FA_ID_SET_LEVEL:
		bf_scr_wk->level = param1;
		break;

	//�^�C�v(�V���O���A�_�u���A�}���`�Awifi�}���`)�̃Z�b�g
	case FA_ID_SET_TYPE:
		bf_scr_wk->type = param1;
		break;

	//ret_work�̎擾
	case FA_ID_GET_RET_WORK:
		*work = bf_scr_wk->ret_work[param1];
		break;

	//�A�����̎擾
	case FA_ID_GET_RENSYOU:
		*work = bf_scr_wk->rensyou;
		break;

	//�A�����̃C���N�������g
	case FA_ID_INC_RENSYOU:
		if( bf_scr_wk->rensyou < FACTORY_RENSYOU_MAX ){
			bf_scr_wk->rensyou++;
		}
		break;

	//���Z�b�g���ă^�C�g���ɖ߂�
	case FA_ID_SYSTEM_RESET:
		OS_ResetSystem( 0 );
		break;

	//------------------------------------
	//	FACTORYDATA�ɃA�N�Z�X
	//------------------------------------
	//�Z�[�u�f�[�^���L�����ǂ����Ԃ�
	case FA_ID_IS_SAVE_DATA_ENABLE:
		*work = FACTORYDATA_GetSaveFlag( bf_scr_wk->factory_savedata );
		break;

	//�x�ގ��Ɍ��݂̃v���C�󋵂��Z�[�u�ɏ����o��
	case FA_ID_SAVE_REST_PLAY_DATA:
		FactoryScr_SaveRestPlayData( bf_scr_wk );
		break;

	//------------------------------------
	//	
	//------------------------------------

	//���E���h�����C���N�������g
	case FA_ID_INC_ROUND:
		*work = FactoryScr_IncRound( bf_scr_wk );
		break;

	//�����X�^�[�i���o�[���擾
	case FA_ID_GET_MONSNO:
		*work = bf_scr_wk->enemy_poke[param1].mons_no;
		break;

	//�Z�i���o�[���擾
	case FA_ID_GET_WAZANO:
		*work = bf_scr_wk->enemy_poke[param1].waza[param2];
		break;

	//�|�P�����^�C�v���擾
	case FA_ID_GET_POKE_TYPE:
		pp = PokemonParam_AllocWork( HEAPID_WORLD );
		//�o�g���^���[�p�|�P�����f�[�^����POKEMON_PARAM�𐶐�
		Frontier_PokeParaMake( &bf_scr_wk->enemy_poke[param1], pp, Factory_GetLevel(bf_scr_wk) );
		*work = PokeParaGet( pp, ID_PARA_type1, NULL );
		sys_FreeMemoryEz( pp );
		break;

	//�ł������^�C�v���擾
	case FA_ID_GET_LARGE_TYPE:
		e_max = Factory_GetEnemyPokeNum( bf_scr_wk->type, FACTORY_FLAG_TOTAL );

		//������
		for( i=0; i < e_max ;i++ ){
			type_tbl[i][0] = 0xff;
			type_tbl[i][1] = 0xff;
		}

		pp = PokemonParam_AllocWork( HEAPID_WORLD );
		for( i=0; i < e_max ;i++ ){

			//�o�g���^���[�p�|�P�����f�[�^����POKEMON_PARAM�𐶐�
			Frontier_PokeParaMake( &bf_scr_wk->enemy_poke[i], pp, Factory_GetLevel(bf_scr_wk) );

			//�^�C�v�擾
			type = PokeParaGet( pp, ID_PARA_type1, NULL );

			for( j=i; j > 0 ;j-- ){
				if( type == type_tbl[j-1][0] ){
					type_tbl[j-1][0]++;
					break;
				}
			}

			//�����^�C�v���Ȃ�������
			if( j == 0 ){
				type_tbl[i][0] = type;
				type_tbl[i][1]++;;
			}
		}
		sys_FreeMemoryEz( pp );

		//�ł����������^�C�v��T��
		type = type_tbl[0][0];
		type_num = type_tbl[0][1];
		for( i=0; i < e_max ;i++ ){
			if( type_num < type_tbl[i][1] ){
				type = type_tbl[i][0];
				type_num = type_tbl[i][1];
			}
		}

		//1�������^�C�v�����݂��Ȃ���
		if( type_num == 1 ){
			*work = 0xff;
		}else{
			*work = type;
		}
		break;

	//���񐔂��擾
	case FA_ID_GET_LAP:
		*work = FactoryScr_CommGetLap( bf_scr_wk );
		break;

	//�G�g���[�i�[��OBJ�R�[�h���擾
	case FA_ID_GET_TR_OBJ_CODE:
		*work = FactoryScr_GetEnemyObjCode( bf_scr_wk, param1 );
		break;

	//�s�킵�����̏���
	case FA_ID_SET_LOSE:
		FactoryScr_SetLose( bf_scr_wk );
		break;

	//7�A��(�N���A)�������̏���
	case FA_ID_SET_CLEAR:
		FactoryScr_SetClear( bf_scr_wk );
		break;

	//���E���h�����擾
	case FA_ID_GET_ROUND:
		*work = FactoryScr_GetRound( bf_scr_wk );
		break;

	//���^�C���t���O���擾
	case FA_ID_GET_RETIRE_FLAG:
		*work = bf_scr_wk->pair_retire_flag;
		break;

	//�������������t���O���擾
	case FA_ID_GET_TRADE_YESNO_FLAG:
		*work = bf_scr_wk->pair_trade_yesno_flag;
		break;

	//�ʐM�ݒ�
	case FA_ID_COMM_COMMAND_INITIALIZE:
		CommCommandFrontierInitialize( bf_scr_wk );
		break;

	//�ʐM�^�C�v���`�F�b�N
	case FA_ID_CHECK_COMM_TYPE:
		*work = Factory_CommCheck( bf_scr_wk->type );
		break;

	//�^�C�v�擾
	case FA_ID_GET_TYPE:
		*work = bf_scr_wk->type;
		break;

	//�o�g�������̒n�ʂ̃p���b�g��ύX����(evy��ύX���Ă���)
	case FA_ID_BTL_ROOM_PAL_CHG:
#if 0
		{
			void *buf;
			NNSG2dPaletteData *dat;
			u8 evy;
			FMAP_PTR fmap = FSS_GetFMapAdrs( core->fss );

			//�W��(0�`16) (�ύX�̓x�����A0(���̐F)�`2,3..(���ԐF)�`16(�w�肵���F)
			evy = param1;
			SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 0, 1, evy, 0x0000 );
			SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 1, 1, evy, 0x62cd );
			SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 2, 1, evy, 0x6aee );
			SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 3, 1, evy, 0x774e );
			SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 4, 1, evy, 0x66ed );
		}
#else
		OS_Printf( "�؂�ւ���p���b�g�i���o�[ = %d\n", param1 );
		GF_BGL_ScrPalChange( fmap->bgl, FRMAP_FRAME_MAP, 3, 10, 26, 11, param1 );
		GF_BGL_LoadScreenV_Req( fmap->bgl, FRMAP_FRAME_MAP );			//�X�N���[���f�[�^�]��
#endif
		break;

	//�}�b�v�̃��[���ړ�TCB�ǉ�
	case FA_ID_ADD_MAP_RAIL_TCB:
		bf_scr_wk->rail_move_tcb = TCB_Add( Factory_MapRailMoveMain, FSS_GetFMapAdrs(core->fss), 
											FACTORY_RAIL_TCB_PRI );
		break;

	//�}�b�v�̃��[���ړ�TCB�폜
	case FA_ID_DEL_MAP_RAIL_TCB:
		if( bf_scr_wk->rail_move_tcb != NULL ){
			TCB_Delete( bf_scr_wk->rail_move_tcb );
			bf_scr_wk->rail_move_tcb = NULL;
		}
		break;

	//�퓬�O��b�̂��߂�B_TOWER_PARTNER_DATA�̃Z�b�g
	case FA_ID_SET_B_TOWER_PARTNER_DATA:
		FSRomBattleTowerTrainerDataMake2( &(bf_scr_wk->tr_data[0]), 
						bf_scr_wk->tr_index[bf_scr_wk->round], HEAPID_WORLD, ARC_PL_BTD_TR );
		FSRomBattleTowerTrainerDataMake2( &(bf_scr_wk->tr_data[1]),
						bf_scr_wk->tr_index[bf_scr_wk->round+FACTORY_LAP_ENEMY_MAX], HEAPID_WORLD,
						ARC_PL_BTD_TR );
		break;

	//��l���̎莝���|�P���������Z�b�g
	case FA_ID_BRAIN_PLAYER_POKE_NAME:
		m_max = Factory_GetMinePokeNum( bf_scr_wk->type );
		for( i=0; i < m_max; i++ ){
			pp = PokeParty_GetMemberPointer( bf_scr_wk->p_m_party, i );
			WORDSET_RegisterPokeMonsName( core->fss->wordset, i, PPPPointerGet(pp) );
		}
		break;

	//�Ⴆ��o�g���|�C���g���擾
	case FA_ID_GET_BP_POINT:
		*work = FactoryScr_GetAddBtlPoint( bf_scr_wk );
		break;

	};

	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	�G�t�F�N�g��(���[��)���c�ɃX�N���[�������Â���
 *
 * @param	tcb		TCB_PTR�^
 * @param	work	���[�N
 *
 * @retval	none
 */
//--------------------------------------------------------------
void Factory_MapRailMoveMain( TCB_PTR tcb, void * work )
{
	int y;
	FMAP_PTR fmap = work;
	
	y = GF_BGL_ScrollGetY( fmap->bgl, FRMAP_FRAME_EFF );

	if( y >= 255 ){
		GF_BGL_ScrollReq(fmap->bgl,FRMAP_FRAME_EFF,GF_BGL_SCROLL_Y_SET,0);
	}else{
		GF_BGL_ScrollReq(fmap->bgl,FRMAP_FRAME_EFF,GF_BGL_SCROLL_Y_INC,FACTORY_SCROLL_MAP_RAIL_Y);
	}
	return;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�s�k�`�F�b�N
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryLoseCheck( FSS_TASK * core )
{
	FACTORY_SCRWORK* bf_scr_wk;
	u16* work = FSSTGetWork( core );

	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	*work = bf_scr_wk->winlose_flag;
	OS_Printf( "�s�k�`�F�b�N*work TRUE���� FALSE�s�k = %d\n", *work );
	return 0;
}


//==============================================================================================
//
//	���M�A��M�̌Ăяo��
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * ���M
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactorySendBuf( FSS_TASK * core )
{
	FACTORY_SCRWORK* bf_scr_wk;
	u16 type	= FSSTGetWorkValue( core );
	u16 param	= FSSTGetWorkValue( core );
	u16* ret_wk	= FSSTGetWork( core );

	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	*ret_wk = FactoryScr_CommSetSendBuf( bf_scr_wk, type, param );
	return 1;
}

//--------------------------------------------------------------
/**
 * ��M
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryRecvBuf( FSS_TASK * core )
{
	u16 wk_id = FSSTGetU16( core );

	//���z�}�V���̔ėp���W�X�^�Ƀ��[�N��ID���i�[
	core->reg[0] = wk_id;

	FSST_SetWait( core, WaitFactoryRecvBuf );
	return 1;
}

//return 1 = �I��
static BOOL WaitFactoryRecvBuf( FSS_TASK * core )
{
	FACTORY_SCRWORK* bf_scr_wk;
	u16 type = FSS_GetEventWorkValue( core, core->reg[0] );		//���ӁI

	bf_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	if( bf_scr_wk->recieve_count >= FACTORY_COMM_PLAYER_NUM ){
		bf_scr_wk->recieve_count = 0;
		return 1;
	}

	return 0;
}

//--------------------------------------------------------------
/**
 *	@brief	�o�g���t�@�N�g���[�ΐ�O���b�Z�[�W��p�\��
 */
//--------------------------------------------------------------
BOOL FSSC_FactoryTalkMsgAppear(FSS_TASK* core)
{
	u16	*msg;
	FACTORY_SCRWORK* ba_scr_wk;
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );
	u16	tr_idx = FSSTGetU8(core);	//��l�ڂ���l�ڂ��H

	ba_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	if(ba_scr_wk == NULL){
		return 0;
	}

	msg = ba_scr_wk->tr_data[tr_idx].bt_trd.appear_word;

	FrontierTalkMsgSub( core, msg );
	return 1;
}


