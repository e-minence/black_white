//============================================================================================
/**
 * @file	fss_stage_sub.c
 * @bfief	�t�����e�B�A�V�X�e���X�N���v�g�R�}���h�T�u�F�X�e�[�W
 * @author	Satoshi Nohara
 * @date	07.06.08
 */
//============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/record.h"
#include "system/buflen.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "system/gamedata.h"
#include "gflib/strbuf_family.h"
#include "../field/fieldsys.h"
#include "savedata/frontier_savedata.h"
#include "savedata/frontier_ex.h"
#include "savedata/b_tower.h"
#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "../field/sysflag.h"
#include "../field/syswork.h"

#include "application/stage.h"
#include "frontier_tool.h"
#include "frontier_def.h"
#include "stage_tool.h"
#include "comm_command_frontier.h"
#include "fssc_stage_sub.h"
#include "../field/scr_tool.h"		//StageScr_GetWinRecordID
#include "itemtool/itemsym.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"
#include "communication/comm_info.h"


//============================================================================================
//
//	�v���g�^�C�v�錾
//
//============================================================================================
STAGE_SCRWORK* StageScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u8 pos, u8 pos2 );
void StageScr_WorkEnemySet( STAGE_SCRWORK* wk, u16 init );
static void StageScr_InitEnemySet( STAGE_SCRWORK* wk );
void StageScr_WorkRelease( STAGE_SCRWORK* wk );
void StageScr_GetResult( STAGE_SCRWORK* wk, void* stage_call );
u16 StageCall_GetRetWork( void* param, u8 pos );
void StageScr_StageRenshouCopyExtra( STAGE_SCRWORK* wk, u16* ret_wk );

//�ʐM
BOOL StageScr_CommSetSendBuf( STAGE_SCRWORK* wk, u16 type, u16 param );

static u16 StageScr_TypeLevelRecordGet( SAVEDATA* sv, u8 type, u8 csr_pos, u16* l_num, u16* h_num );
static void StageScr_TypeLevelRecordSet( SAVEDATA* sv, u8 type, u8 csr_pos, u8 num );
void StageScr_TypeLevelMaxOff( STAGE_SCRWORK* wk );


//============================================================================================
//
//	�֐�
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	���[�N�G���A�m��
 *
 * @param	savedata		�Z�[�u�f�[�^�̃|�C���^
 * @param	init			�ŏ����炩�������炩
 * @param	type			�^�C�v(�V���O���A�_�u���Ȃ�)
 * @param	pos				�I�������|�P�����ʒu
 * @param	pos2			�I�������|�P�����ʒu
 * @param	none
 *	
 * �����Ȃ炸 StageScr_WorkRelease()�ŗ̈���J�����邱��
 */
//--------------------------------------------------------------
STAGE_SCRWORK* StageScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u8 pos, u8 pos2 )
{
	STAGEDATA* stage_sv;
	STAGESCORE* score_sv;
	u8 clear_flag,level,m_max;
	u16	i,l_num,h_num,set_num;
	POKEMON_PARAM* temp_poke;
	static STAGE_SCRWORK* wk;					//���ƂŊm�F]]]]]]]]]]]]]]]]]]]]]]]]]

	wk = sys_AllocMemory( HEAPID_WORLD, sizeof(STAGE_SCRWORK) );
	MI_CpuClear8( wk, sizeof(STAGE_SCRWORK) );

	//�Z�[�u�f�[�^�擾
	wk->stage_savedata	= SaveData_GetStageData( savedata );
	wk->sv				= savedata;
	wk->heapID			= HEAPID_WORLD;
	wk->p_party			= PokeParty_AllocPartyWork( HEAPID_WORLD );		//�p�[�e�B�������m��

	stage_sv = wk->stage_savedata;
	score_sv = SaveData_GetStageScore( savedata );

	//�V�K���A�p����
	if( init == 0 ){
		wk->type	= type;
		m_max		= Stage_GetMinePokeNum( wk->type );
		wk->round	= 0;

		STAGEDATA_Init( stage_sv );	//���f�f�[�^������

		//WIFI�̂ݓ���
		if( wk->type == STAGE_TYPE_WIFI_MULTI ){
			clear_flag = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), 
										FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
										Frontier_GetFriendIndex(FRID_STAGE_MULTI_WIFI_CLEAR_BIT) );
		}else{
			//�N���A�������t���O���擾
			clear_flag = (u8)STAGESCORE_GetScoreData(	score_sv, STAGESCORE_ID_CLEAR_FLAG, 
														wk->type, 0, NULL );
		}

		if( clear_flag == 1 ){
			wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
									StageScr_GetWinRecordID(wk->type),
									Frontier_GetFriendIndex(StageScr_GetWinRecordID(wk->type)) );
		}else{
			wk->rensyou = 0;

			//���݂̃o�g���^�C�v�̃^�C�v���x�����N���A����
			for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
				StageScr_TypeLevelRecordSet( wk->sv, wk->type, i, 0 );
			}
		}

		//�I�������莝���|�P�����̈ʒu
		wk->mine_poke_pos[0] = pos;
		wk->mine_poke_pos[1] = pos2;

	}else{
		//���݂̃v���C�i�s�f�[�^�擾
		wk->type	= (u8)STAGEDATA_GetPlayData( stage_sv, STAGEDATA_ID_TYPE, 0, 0, NULL );
		m_max = Stage_GetMinePokeNum( wk->type );
		wk->round	= (u8)STAGEDATA_GetPlayData( stage_sv, STAGEDATA_ID_ROUND,0, 0, NULL );
		wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
									StageScr_GetWinRecordID(wk->type),
									Frontier_GetFriendIndex(StageScr_GetWinRecordID(wk->type)) );

		//---------------------------------------------------------------------------------------
		//�ۑ����Ă������莝���|�P�����ʒu���擾
		for( i=0; i < m_max ;i++ ){
			wk->mine_poke_pos[i] = 
				(u8)STAGEDATA_GetPlayData(stage_sv, STAGEDATA_ID_MINE_POKE_POS, i, 0, NULL );
			OS_Printf( "wk->mine_poke_pos[%d] = %d\n", i, wk->mine_poke_pos[i] );
		}

		//---------------------------------------------------------------------------------------
		//�ۑ����Ă������G�g���[�i�[��index�擾
		for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
			wk->tr_index[i] = (u16)STAGEDATA_GetPlayData(	stage_sv, STAGEDATA_ID_TR_INDEX, 
															i, 0, NULL);
			OS_Printf( "tr_index[%d] = %d\n", i, wk->tr_index[i] );
		}

		//---------------------------------------------------------------------------------------
		//�ۑ����Ă������G�|�P������index���擾
		for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
			wk->enemy_poke_index[i] = (u8)STAGEDATA_GetPlayData( stage_sv, 
														STAGEDATA_ID_ENEMY_POKE_INDEX, 
														i, 0, NULL );
			OS_Printf( "wk->enemy_poke_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
		}

	}

	//�����A�C�e����ۑ����Ă���
	for( i=0; i < m_max; i++ ){
		temp_poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->sv), 
												wk->mine_poke_pos[i] );
		wk->itemno[i] = PokeParaGet( temp_poke, ID_PARA_item, NULL );
	}

	wk->win_cnt	= 0;
	wk->lap		= (u16)(wk->rensyou / STAGE_LAP_ENEMY_MAX);

	//�ʐM�}���`�̂݃����N���x����10�Œ�ɂ��ăX�^�[�g������
	if( wk->type == STAGE_TYPE_MULTI ){

		//�����͑S�Ẵ^�C�v���擾���ăZ�b�g�Ȃ̂�(/2)���Ȃ��Ă悢
		for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
			//�������̓��R�[�h�ł͂Ȃ��I
			Stage_SetTypeLevel( i, &wk->type_level[STAGE_TYPE_MULTI][0], (STAGE_TYPE_LEVEL_MAX-1) );
		}
	}else{

		//�����͑S�Ẵ^�C�v���擾���ăZ�b�g�Ȃ̂�(/2)���Ȃ��Ă悢
		for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
			set_num = StageScr_TypeLevelRecordGet(	savedata, wk->type, i, &l_num, &h_num );

			//�������̓��R�[�h�ł͂Ȃ��I
			Stage_SetTypeLevel( i, &wk->type_level[wk->type][0], set_num );
		}
	}

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	�G�g���[�i�[�ƓG�|�P������I�o����
 *
 * @param	savedata	�Z�[�u�f�[�^�ւ̃|�C���^
 * @param	init		���������[�h BTWR_PLAY_NEW:���߂���ABTWR_PLAY_CONTINE:��������
 */
//--------------------------------------------------------------
void StageScr_WorkEnemySet( STAGE_SCRWORK* wk, u16 init )
{
	//OS_Printf( "�O �p�� ����wk = %d\n", wk );
	
	//�V�K���A�p����
	if( init == 0 ){
		StageScr_InitEnemySet( wk );
	}else{
		StageScr_InitEnemySet( wk );
	}

	//OS_Printf( "�� �V�K ����wk = %d\n", wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	�V�K�F�G�g���[�i�[�A�G�|�P�����f�[�^�𐶐�
 *
 * @param	wk		STAGE_SCRWORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void StageScr_InitEnemySet( STAGE_SCRWORK* wk )
{
	u8 level,set_num,set_pos;
	int i,mine_lv;
	POKEMON_PARAM* temp_poke;
	B_TOWER_POKEMON poke;

	OS_Printf( "�V�K�F�f�[�^�𐶐�\n" );

	//�|�P�����^�C�v��I��ł���łȂ��ƃg���[�i�[�̃C���f�b�N�X�����肷�邱�Ƃ��ł��Ȃ�
	
	//�g���[�i�[index�쐬
	
	//���肷��|�P���������V���O���ȊO��2�C�ɂȂ�
	set_num = 1;
	if( wk->type != STAGE_TYPE_SINGLE ){
		set_num = STAGE_ENTRY_POKE_MAX;
	}

	//�������̓��R�[�h�ł͂Ȃ��I
	level = Stage_GetTypeLevel( wk->csr_pos, &wk->type_level[wk->type][0] );

	Stage_CreateTrType( wk->csr_pos, set_num, level, wk->round, wk->tr_index );
	Stage_CreateTrIndex( wk->type, set_num, wk->lap, level, wk->round, wk->tr_index );

	//���Ȃ��悤�ɂ��邽�߂�10�C�ŏ��ɐ����������������A
	//�ǂ̃|�P�����^�C�v��I�Ԃ��͂��̎��łȂ��Ƃ킩��Ȃ��̂ŁA
	//���񐶐�����

	//�u���[���̎��́A�I�o���郉���N�������̃��x���ɍ��킹��
	set_pos = (wk->round * STAGE_ENTRY_POKE_MAX);		//�Z�b�g����ʒu
	if( (wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_1ST) ||
		(wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_2ND) ){

		//�莝���|�P�����̃��x�����烉���N���擾
		mine_lv = Stage_GetMineLevelRank( wk );
		level = mine_lv;
		OS_Printf( "�u���[���o�ꎞ�@�莝������Z�o���������N = %d\n", level );
	}

	//�|�P����index�쐬
	temp_poke = PokeParty_GetMemberPointer( wk->p_party, 0 );

	//��������|�P������1�ł悢(�����|�P�������o���d�g�݂Ȃ̂�)
	//Stage_CreatePokeIndex(	set_num, wk->poke_type, level, wk->round,
	Stage_CreatePokeIndex(	1, wk->poke_type, level, wk->round,
							PokeParaGet(temp_poke,ID_PARA_monsno,NULL),
							wk->enemy_poke_index );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	���[�N�G���A���J������
 */
//--------------------------------------------------------------
void StageScr_WorkRelease( STAGE_SCRWORK* wk )
{
	if( wk == NULL ){
		return;
	}

	if( wk->p_party != NULL ){
		sys_FreeMemoryEz( wk->p_party );
	}

	MI_CpuClear8( wk, sizeof(STAGE_SCRWORK) );
	sys_FreeMemoryEz( wk );
	wk = NULL;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	�^�C�v�I����ʌĂяo����̌��ʎ擾
 *
 * @param	wk		STAGE_SCRWORK�̃|�C���^
 */
//--------------------------------------------------------------
void StageScr_GetResult( STAGE_SCRWORK* wk, void* stage_call )
{
	int i;

	//���ʂ��擾����
	wk->ret_work	= StageCall_GetRetWork( stage_call, 0 );
	wk->csr_pos		= wk->ret_work;								//�J�[�\���ʒu
	wk->poke_type	= Stage_GetPokeType( wk->ret_work );		//�J�[�\���ʒu�̃|�P�����^�C�v�擾

	//�J�[�\���ʒu�𒲐�
	if( wk->csr_pos >= 17 ){
		wk->csr_pos = STAGE_TR_TYPE_MAX - 1;					//17
	}

	OS_Printf( "wk->ret_work = %d\n", wk->ret_work );
	return;
}

//----------------------------------------------------------------------------
/**
 * @brief	�o�g���X�e�[�W�̃|�P�����I����ʂł̌��ʂ��擾
 *
 * @param	param	STAGE_CALL_WORK�̃|�C���^
 * @param	pos		ret_work[pos](0-5)
 *
 * @return	"����"
 */
//----------------------------------------------------------------------------
u16 StageCall_GetRetWork( void* param, u8 pos )
{
	STAGE_CALL_WORK* stage_call = param;
	return stage_call->ret_work;
}

//--------------------------------------------------------------
/**
 * @brief   �o�g���X�e�[�W�̊O���A���L�^���������݁A�Z�[�u���s��
 *
 * @param	wk		STAGE_SCRWORK�̃|�C���^
 */
//--------------------------------------------------------------
void StageScr_StageRenshouCopyExtra( STAGE_SCRWORK* wk, u16* ret_wk )
{
	LOAD_RESULT load_result;
	SAVE_RESULT save_result;

	//�O���Z�[�u�t�@�C�������[�h
	//�풓�ɔz�u����Ă���t�����e�B�A���R�[�h����Z�b�g����l���擾
	//�O���Z�[�u�̒l�Ɣ�r���A�傫����΃Z�b�g�@���@�O���Z�[�u���s

	//WIFI�ȊO�͏�������
	if( wk->type != STAGE_TYPE_WIFI_MULTI ){
		*ret_wk = FrontierEx_StageRenshou_RenshouCopyExtra( wk->sv, 
									StageScr_GetMaxWinRecordID(wk->type),
									StageScr_GetMonsNoRecordID(wk->type),
									Frontier_GetFriendIndex(StageScr_GetMaxWinRecordID(wk->type)),
									StageScr_GetExMaxWinRecordID(wk->type),
									HEAPID_WORLD, &load_result, &save_result );
		OS_Printf( "���[�h���� = %d\n", load_result );
		OS_Printf( "�Z�[�u���� = %d\n", save_result );
	}else{
		*ret_wk = FALSE;
	}

	return;
}


/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
void StageScr_SaveRestPlayData( STAGE_SCRWORK* wk );
//void StageScr_SetNG( STAGE_SCRWORK* wk );
u16	StageScr_IncRound( STAGE_SCRWORK* wk );
u16	StageScr_GetRound( STAGE_SCRWORK* wk );
u16 StageScr_GetEnemyObjCode( STAGE_SCRWORK* wk, u8 param );
void StageScr_SetLose( STAGE_SCRWORK* wk );
void StageScr_SetClear( STAGE_SCRWORK* wk );

//--------------------------------------------------------------
/**
 * @brief	�x�ނƂ��Ɍ��݂̃v���C�󋵂��Z�[�u�ɏ����o��
 *
 * @param	wk		STAGE_SCRWORK�̃|�C���^
 */
//--------------------------------------------------------------
void StageScr_SaveRestPlayData( STAGE_SCRWORK* wk )
{
	u16	i;
	u8	buf8[4];
	u16	buf16[4];
	u32	buf32[4];
	u16 l_num,h_num,set_num;
	POKEMON_PARAM* temp_poke;
	STAGEDATA* f_sv = wk->stage_savedata;
	STAGESCORE* score_sv = SaveData_GetStageScore( wk->sv );

	//"�V���O���A�_�u���A�}���`�Awifi�}���`"�����o��
	buf8[0] = wk->type;
	STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_TYPE, 0, 0, buf8 );
	
	//�Z�[�u�t���O��L����ԂɃ��Z�b�g
	STAGEDATA_SetSaveFlag( wk->stage_savedata, TRUE );

	//"���E���h��"�����o��(0-17�l�ڂ̉��l�ڂ�������炷)
	buf8[0] = wk->round;
	//OS_Printf( "wk->round = %d\n", wk->round );
	//OS_Printf( "buf8[0] = %d\n", buf8[0] );
	STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_ROUND, 0, 0, buf8 );

#if 0
	//�A�� / 10 = ����
	//�A�� % 10 = ���l�ڂ�
#endif
	//"�A����"�����o��(�u����27�l�ڂł��v�Ƃ����悤�Ɏg��)
	//OS_Printf( "rensyou = %d\n", wk->rensyou );
	FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), StageScr_GetWinRecordID(wk->type), 
						Frontier_GetFriendIndex(StageScr_GetWinRecordID(wk->type)), wk->rensyou );
	
#if 0
	//"�ő�A����"�����o��
	after = FrontierRecord_SetIfLarge(	SaveData_GetFrontier(wk->sv),
									StageScr_GetMaxWinRecordID(wk->type),
									Frontier_GetFriendIndex(StageScr_GetMaxWinRecordID(wk->type)),
									wk->rensyou );
#else
	//������1�C�������̈�Ȃ��̂ŕK���㏑������
	//
	//StageScr_StageRenshouCopyExtra(...)�̒��ŁA��(��)�ƊO���f�[�^�ɕۑ�����Ă���f�[�^���A
	//��r���āA�O���f�[�^�̕��ɂ������㏑�����邩���ʂ��Ă���
	//
	//���̗̈�̃f�[�^�͐��у{�[�h�ł����Ȃ��̂ő��v�Ȃ͂��B

	FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), StageScr_GetMaxWinRecordID(wk->type),
						Frontier_GetFriendIndex(StageScr_GetMaxWinRecordID(wk->type)),
						wk->rensyou );
#endif

	//"10�A��(�N���A)�������t���O"�����o��
	buf8[0] = wk->clear_flag;
	STAGESCORE_PutScoreData( score_sv, STAGESCORE_ID_CLEAR_FLAG, wk->type, 0, buf8 );

	//WIFI�̂ݓ���
	if( wk->type == STAGE_TYPE_WIFI_MULTI ){
		FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
					FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
					Frontier_GetFriendIndex(FRID_STAGE_MULTI_WIFI_CLEAR_BIT), wk->clear_flag );
	}

	//"�G�g���[�i�[�C���f�b�N�X"�����o��
	for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
		buf16[0] = wk->tr_index[i];
		STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_TR_INDEX, i, 0, buf16 );
	}

	//"�Q�����Ă���莝���|�P�����̈ʒu"�����o��
	for( i=0; i < STAGE_ENTRY_POKE_MAX ;i++ ){
		buf8[0] = wk->mine_poke_pos[i];
		STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_MINE_POKE_POS, i, 0, buf8 );
	}

	//�����͑S�Ẵ^�C�v���Z�b�g�Ȃ̂�(/2)���Ȃ��Ă悢
	//"�^�C�v���Ƃ̃��x��"�����o��
	for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
		//�������̓��R�[�h�ł͂Ȃ��I
		buf8[0] = Stage_GetTypeLevel( i, &wk->type_level[wk->type][0] );
		StageScr_TypeLevelRecordSet( wk->sv, wk->type, i, buf8[0] );
	}
	
	//"�G�|�P�����f�[�^�����Ȃ��悤�ɂ���"�����o��
	for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
		buf16[0] = wk->enemy_poke_index[i];
		STAGEDATA_PutPlayData( wk->stage_savedata, STAGEDATA_ID_ENEMY_POKE_INDEX, i, 0, buf16 );
	}

	//"�A�����̃|�P�����i���o�["�����o��
	temp_poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->sv), 
											wk->mine_poke_pos[0] );
	FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
						StageScr_GetMonsNoRecordID(wk->type),
						Frontier_GetFriendIndex(StageScr_GetMonsNoRecordID(wk->type)), 
						PokeParaGet(temp_poke,ID_PARA_monsno,NULL) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	���E���h�����C���N�������g
 *
 * @param	wk		STAGE_SCRWORK�̃|�C���^
 */
//--------------------------------------------------------------
u16	StageScr_IncRound( STAGE_SCRWORK* wk )
{
	wk->round++;
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	���E���h�����擾
 *
 * @param	wk		STAGE_SCRWORK�̃|�C���^
 */
//--------------------------------------------------------------
u16	StageScr_GetRound( STAGE_SCRWORK* wk )
{
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	�ΐ�g���[�i�[OBJ�R�[�h�擾
 *
 * @param	wk		STAGE_SCRWORK�̃|�C���^
 * @param	param	0=1�l�ځA1=2�l��(�G�̃p�[�g�i�|)
 */
//--------------------------------------------------------------
u16 StageScr_GetEnemyObjCode( STAGE_SCRWORK* wk, u8 param )
{
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* p_rom_tr;
	u8 index;

	//�擾����g���[�i�[�f�[�^�̃C���f�b�N�X���擾
	index = (wk->round * STAGE_ENTRY_POKE_MAX) + param;

	//ROM����g���[�i�[�f�[�^���m��
	p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[index], HEAPID_WORLD, ARC_PL_BTD_TR );
	sys_FreeMemoryEz( p_rom_tr );

	//�g���[�i�[�^�C�v����OBJ�R�[�h���擾���Ă���
	return Frontier_TrType2ObjCode( bt_trd.tr_type );
}

//--------------------------------------------------------------
/**
 * @brief	�s�킵�����̏���
 *
 * @param	wk		STAGE_SCRWORK�̃|�C���^
 */
//--------------------------------------------------------------
void StageScr_SetLose( STAGE_SCRWORK* wk )
{
	int i;
	OS_Printf( "\n�o�g���X�e�[�W�f�[�^�@�s��Z�b�g\n" );

	//�������̓��R�[�h�ł͂Ȃ��I
	//���x�����N���A
	Stage_ClearTypeLevel( &wk->type_level[wk->type][0] );	//STAGE_SCRWORK

	//���݂�5�A���Ȃǂ�ۑ�����K�v������I
	StageScr_SaveRestPlayData( wk );		//�Z�[�u�f�[�^�ɑ��

	//�V�K���A�p�����́AWK_SCENE_STAGE_LOBBY�Ɋi�[����Ă���̂ŁA
	//������round�Ȃǂ��N���A���Ȃ��Ă��A
	//WK_SCENE_STAGE_LOBBY���p���ł͂Ȃ���ԂŃZ�[�u�����̂ŁA
	//��t�ɘb�������Ă��A�V�K����ɂȂ胏�[�N�̓N���A�����B
	return;
}

//--------------------------------------------------------------
/**
 * @brief	10�A��(�N���A)�������̏���
 *
 * @param	wk		STAGE_SCRWORK�̃|�C���^
 */
//--------------------------------------------------------------
void StageScr_SetClear( STAGE_SCRWORK* wk )
{
	POKEMON_PARAM* temp_poke;
	OS_Printf( "\n�o�g���X�e�[�W�f�[�^�@10�A��(�N���A)�Z�b�g\n" );
	
	wk->clear_flag = 1;						//10�A��(�N���A)�������t���OON

	//�A�����̃|�P�����i���o�[
	//temp_poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->sv), 
	//										wk->mine_poke_pos[0] );
	//wk->clear_monsno[wk->type] = PokeParaGet( temp_poke, ID_PARA_monsno, NULL );

	//wk->rensyou		= 0;				//���݂̘A����
	if( wk->lap < STAGE_LAP_MAX ){
		wk->lap++;							//���񐔂̃C���N�������g
	}
	wk->round			= 0;				//�����l�ځH
	StageScr_SaveRestPlayData( wk );		//�Z�[�u�f�[�^�ɑ��
	return;
}


//==============================================================================================
//
//	�ʐM(CommStart)
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	���M�E�F�C�g�@
 *
 * @param	wk			STAGE_SCRWORK�^�̃|�C���^
 * @param	type		���M�^�C�v
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL StageScr_CommSetSendBuf( STAGE_SCRWORK* wk, u16 type, u16 param )
{
	int ret;

	switch( type ){

	//
	case STAGE_COMM_BASIC:
		ret = CommStageSendBasicData( wk );
		break;

	case STAGE_COMM_TR:
		ret = CommStageSendTrData( wk );
		break;

	case STAGE_COMM_ENEMY:
		ret = CommStageSendEnemyPokeData( wk );
		break;

	case STAGE_COMM_RETIRE:
		ret = CommStageSendRetireFlag( wk, param );
		break;
	};

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�W�N���A���ɖႦ��o�g���|�C���g���擾
 *
 * @param	wk
 *
 * @return	"�Ⴆ��o�g���|�C���g"
 *
 * �ʐM�}���`�͌Œ�B�V���O���A�_�u���͎����̂݁B
 * WIFI�͑���Ǝ��񐔓����B
 * �Ȃ̂Ŏ��񐔃`�F�b�N�͂���Ȃ��B
 */
//--------------------------------------------------------------------------------------------
u16	StageScr_GetAddBtlPoint( STAGE_SCRWORK* wk );
u16	StageScr_GetAddBtlPoint( STAGE_SCRWORK* wk )
{
	static const u8 bppoint_normal[STAGE_LAP_MAX+1] = {0,
										1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 
										4, 6, 6, 8, 8, 10, 10, 12 };	//0,1���`18���ȍ~

	OS_Printf( "���� = %d\n", wk->lap );
	OS_Printf( "�A���� = %d\n", wk->rensyou );

	//�u���[���`�F�b�N
	if( wk->type == STAGE_TYPE_SINGLE ){
		if( (wk->rensyou == STAGE_LEADER_SET_1ST) || (wk->rensyou == STAGE_LEADER_SET_2ND) ){
			return 20;
		}

	//�ʐM�}���`��BP�Œ�I
	}else if( wk->type == STAGE_TYPE_MULTI ){
		return 6;
	}

	//�ő���񐔃`�F�b�N
	if( wk->lap >= STAGE_LAP_MAX ){
		return bppoint_normal[ STAGE_LAP_MAX ];		//�e�[�u����+1���Ă���̂�
	}

	return bppoint_normal[ wk->lap ];
}


//==============================================================================================
//
//	�^�C�v���x���֘A
//
//==============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v���x���̃��R�[�h���擾
 *
 * @param	wk
 *
 * @return	"���R�[�h"
 */
//--------------------------------------------------------------------------------------------
static u16 StageScr_TypeLevelRecordGet( SAVEDATA* sv, u8 type, u8 csr_pos, u16* l_num, u16* h_num )
{
	u16 now_type_level;

	//���݂̃^�C�v���x�����擾
	now_type_level = FrontierRecord_Get(SaveData_GetFrontier(sv), 
							StageScr_GetTypeLevelRecordID(type,csr_pos),
							Frontier_GetFriendIndex(StageScr_GetTypeLevelRecordID(type,csr_pos)) );

	now_type_level &= 0xff;				//1byte�����g��Ȃ�
	*l_num = (now_type_level & 0xf);	//�^�C�v[0]
	*h_num = (now_type_level >> 4);		//�^�C�v[1]�Ƃ����悤�Ɋi�[����Ă���

	OS_Printf( "\n**************************\n" );
	OS_Printf( "csr_pos = %d\n", csr_pos );
	OS_Printf( "now_type_level = %d\n", now_type_level );
	OS_Printf( "*l_num = %d\n", *l_num );
	OS_Printf( "*h_num = %d\n", *h_num );

	if( (csr_pos % 2) == 0 ){
		return *l_num;
	}else{
		return *h_num;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v���x���̃��R�[�h���Z�b�g
 *
 * @param	wk
 *
 * @return	"���R�[�h"
 */
//--------------------------------------------------------------------------------------------
static void StageScr_TypeLevelRecordSet( SAVEDATA* sv, u8 type, u8 csr_pos, u8 num )
{
	u8 offset;
	u8 param;
	u8 set_num,total;
	u16 l_num,h_num;

#ifdef PM_DEBUG
	OS_Printf( "���R�[�h�ύX�O�̒l\n" );
	StageScr_TypeLevelRecordGet( sv, type, csr_pos, &l_num, &h_num );
	total = ( (h_num << 4) | l_num );
	OS_Printf( "total = %d\n", total );
#endif

	offset	= ( csr_pos / 2 );			//�t���O�̃I�t�Z�b�g[0,1][2,3],,,
	param	= ( csr_pos % 2 );			//�t���O�̂ǂ����4bit���擾

	//�Z�b�g����ق��̂ݏ���
	if( param == 0 ){
		total = total & 0xf0;
	}else{
		total = total & 0x0f;
	}
	OS_Printf( "total = %d\n", total );

	//�Z�b�g����l���r�b�g����
	set_num = (num << (4 * param));
	OS_Printf( "set_num = %d\n", set_num );

	//�i�[
	total |= set_num;
	OS_Printf( "total = %d\n", total );

	FrontierRecord_Set(	SaveData_GetFrontier(sv), 
					StageScr_GetTypeLevelRecordID(type,csr_pos),
					Frontier_GetFriendIndex(StageScr_GetTypeLevelRecordID(type,csr_pos)), total );

#ifdef PM_DEBUG
	OS_Printf( "���R�[�h�ύX��̒l\n" );
	StageScr_TypeLevelRecordGet( sv, type, csr_pos, &l_num, &h_num );
#endif

	return;
}

//--------------------------------------------------------------
/**
 * @brief	�S�Ẵ^�C�v�̃����N������ς݂̍ő�ɂȂ��Ă����璧�킵�Ă��Ȃ���Ԃɂ���
 *
 * @param	wk		STAGE_SCRWORK�̃|�C���^
 *
 * �����R�[�h�ł͂Ȃ�
 */
//--------------------------------------------------------------
void StageScr_TypeLevelMaxOff( STAGE_SCRWORK* wk )
{
	u8 rank;
	int i;

	//�}���`�ȊO�̎�
	if( wk->type != STAGE_TYPE_MULTI ){

		//�u???�v�͔������̂�17
		//for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
		for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){

			//�������̓��R�[�h�ł͂Ȃ��I
			rank = Stage_GetTypeLevel(	i, &wk->type_level[wk->type][0] );
			if( rank < STAGE_TYPE_LEVEL_MAX ){
				break;
			}
		}

		//�S�Ẵ^�C�v�̃����N������ς݂̍ő�ɂȂ��Ă���
		//if( i == STAGE_TR_TYPE_MAX ){
		if( i == (STAGE_TR_TYPE_MAX-1) ){

			//���킵�Ă��Ȃ��ő�ɂ���
			//for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
			for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){
				//�������̓��R�[�h�ł͂Ȃ��I
				Stage_SetTypeLevel( i, &wk->type_level[wk->type][0], 
									(STAGE_TYPE_LEVEL_MAX-1) );
			}
		}
	}

#if 0	//�f�o�b�N�Z�b�g

	//���킵�Ă��Ȃ��ő�ɂ���
	//for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
	for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){
		//�������̓��R�[�h�ł͂Ȃ��I
		Stage_SetTypeLevel( i, &wk->type_level[wk->type][0], 
							(STAGE_TYPE_LEVEL_MAX-1) );
	}
#endif

	return;
}


