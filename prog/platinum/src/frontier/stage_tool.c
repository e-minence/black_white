//==============================================================================
/**
 * @file	stage_tool.c
 * @brief	�o�g���X�e�[�W�֘A�c�[����
 * @author	nohara
 * @date	2007.06.06
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/pm_str.h"
#include "poketool/poke_tool.h"
#include "poketool/status_rcv.h"

#include "frontier_tool.h"
#include "frontier_def.h"
#include "stage_def.h"
#include "stage_tool.h"

#include "msgdata/msg.naix"					//

#include "gflib/strbuf_family.h"
#include "savedata/savedata_def.h"
#include "savedata/savedata.h"
#include "savedata/zukanwork.h"
#include "savedata/record.h"
#include "savedata/b_tower_local.h"
#include "field/encount_set.h"
#include "field/ev_time.h"
#include "field/fieldobj_code.h"
#include "field/location.h"
#include "communication/comm_tool.h"
#include "communication/comm_system.h"
#include "communication/comm_info.h"
#include "communication/comm_def.h"
#include "battle/attr_def.h"
#include "battle/battle_common.h"
#include "battle/b_tower_data.h"


//==============================================================================
//
//	extern�錾
//
//==============================================================================
//�g���[�i�[�f�[�^�����i�������j
extern void BattleParam_TrainerDataMake( BATTLE_PARAM* bp );
//�g���[�i�[�f�[�^�����i�G���j
extern void BattleParam_EnemyTrainerDataMake( BATTLE_PARAM* bp );


//==============================================================================
//	�f�[�^
//==============================================================================
#include "battle/trtype_def.h"

//�T�[�`�p�Ƀf�[�^�e�[�u�����������������̂Ńt�@�C���𕪂���
#include "stage.dat"


//==============================================================================
//
//
//
//==============================================================================
//�|�P�����̊�{�ݒ�f�[�^
typedef struct{
	u8 level;			//�|�P�������x��
	u8 pow_rnd;			//�p���[����
	u16 ai;				//AI
}STAGE_POKE_DATA;

//�����N�ɑΉ�
static const STAGE_POKE_DATA stage_poke_data[STAGE_TYPE_LEVEL_MAX] = {
	{ 0,	11,		FR_AI_YASEI },	//1
	{ 10,	11,		FR_AI_YASEI },	//2
	{ 20,	15,		FR_AI_YASEI },	//3
	{ 30,	15,		FR_AI_BASIC },	//4
	{ 40,	15,		FR_AI_BASIC },	//5
	{ 50,	19,		FR_AI_BASIC },	//6
	{ 60,	19,		FR_AI_EXPERT },	//7
	{ 70,	19,		FR_AI_EXPERT },	//8
	{ 80,	23,		FR_AI_EXPERT },	//9
	{ 90,	23,		FR_AI_EXPERT },	//10
};

//�{�X�͌Œ�
//90���(5�T�ڍŌ�)		LV50	31 �G�L�X�p�[�g
//180���(10�T�ڍŌ�)	LV100	31 �G�L�X�p�[�g


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
void Stage_CreateTrType( u8 csr_pos, u8 set_num, u8 rank, u8 round, u16 tr_index[] );
void Stage_CreateTrIndex( u8 type, u8 set_num, int lap, u8 rank, u8 round, u16 tr_index[] );
void Stage_CreatePokeIndex( u8 set_num, u8 poketype, u8 rank, u8 round, u16 monsno, u16 poke_index[] );
u8 Stage_GetMinePokeNum( u8 type );
u8 Stage_GetEnemyPokeNum( u8 type );
BATTLE_PARAM* BtlStage_CreateBattleParam( STAGE_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param );
static u32 Stage_GetFightType( u8 type );
static void Stage_PokemonParamCreateAll( B_TOWER_POKEMON *pwd, u8 type_level, u16 tr_index, u16 poke_index[], int set_num, int heap_id, int arcPM );
static u32 Stage_GetPokeLevel( STAGE_SCRWORK* wk, u8 rank );
u8 Stage_GetPokeType( u8 csr_pos );
u8 Stage_GetMineLevelRank( STAGE_SCRWORK* wk );

static u8 Stage_GetStagePokeDataLevel( u8 rank );
static u8 Stage_GetStagePokeDataPowRnd( u8 rank );
static u16 Stage_GetStagePokeDataAI( u8 rank );
static u16 Stage_GetRank( u8 rank );


//==============================================================================
//	
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   �g���[�i�["�^�C�v"������
 *
 * @param   type			�V���O���A�_�u���Ȃ�
 * @param   rank			�����N(0-9,10)
 * @param   round			���E���h��
 * @param   tr_index		�g���[�i�[�^�C�v���i�[����|�C���^
 *
 * @retval  none
 *
 * �u����ς񂱂����A����ς񂱂����v�Ƃ����`�őI�o����Ă����Ȃ�
 * �g���[�i�[�̃C���f�b�N�X���擾���鎞�ɓ����u����ς񂱂����v�ɂȂ�Ȃ���Ηǂ�
 * �g�p���Ă��Ȃ��u����ς񂱂����v�����݂��Ȃ����́A
 * �g���[�i�[�^�C�v��������x�I�o���Ȃ���΂����Ȃ�
 */
//--------------------------------------------------------------
void Stage_CreateTrType( u8 csr_pos, u8 set_num, u8 rank, u8 round, u16 tr_index[] )
{
	int i;
	u16 no;
	u8 set_pos,rankno;
	int set_count = 0;

	set_pos = (round * STAGE_ENTRY_POKE_MAX);		//�Z�b�g����ʒu

	rankno = Stage_GetRank( rank );

	OS_Printf( "�J�[�\���ʒu = %d\n", csr_pos );
	OS_Printf( "�|�P�����^�C�v = %d\n", Stage_GetPokeType(csr_pos) );
	OS_Printf( "�����N = %d\n", rankno );
	OS_Printf( "���E���h = %d\n", round );

	do{
		no = ( gf_rand() % STAGE_RANK_TR_MAX );

		//�e�[�u������g���[�i�[�^�C�v���擾
		if( no < STAGE_RANK_TR1_NUM ){
			tr_index[set_pos+set_count] = rank_tr1_list[rankno][no];
		}else{
			tr_index[set_pos+set_count] = rank_tr2_list[csr_pos][no-STAGE_RANK_TR1_NUM];
		}

		OS_Printf( "�g���[�i�[�^�C�v tr_index = %d\n", tr_index[set_pos+set_count] );
		set_count++;

	}while( set_count < set_num );

	return;
}

//--------------------------------------------------------------
/**
 * @brief   �g���[�i�["�C���f�b�N�X"������
 *
 * @param   type			�V���O���A�_�u���Ȃ�
 * @param   rank			�����N(0-9,10)
 * @param   round			���E���h��
 * @param   tr_index		�g���[�i�[�C���f�b�N�X���i�[����|�C���^(�g���[�i�[�^�C�v���i�[���Ă���)
 *
 * @retval  none
 *
 * ���[�v���������A�A�A�m�F����I
 *
 * �T�[�`���x��������f�[�^�e�[�u�����g���[�i�[�^�C�v�ʂɃ\�[�g���Ă����āA
 * �g���[�i�[�C���f�b�N�X���f�[�^�Ɋ܂߂Ă����Ȃǂ��K�v
 *
 * index, trtype, 
 * 100,		TRTYPE_TANPAN,
 * 399,		TRTYPE_TANPAN,
 * 10,		TRTYPE_MINI,
 * 29,		TRTYPE_MINI,
 */
//--------------------------------------------------------------
void Stage_CreateTrIndex( u8 type, u8 set_num, int lap, u8 rank, u8 round, u16 tr_index[] )
{
	int i,start_no,check_num,set_count,tr_type;
	u16 no,chg_no;
	u8 set_pos,rankno;
	
	set_count	= 0;
	set_pos		= (round * STAGE_ENTRY_POKE_MAX);		//�Z�b�g����ʒu
	
	rankno = Stage_GetRank( rank );

	OS_Printf( "�����N = %d\n", rankno );
	OS_Printf( "���E���h = %d\n", round );

	//�u���[��(�V���O���̂�)
	if( type == STAGE_TYPE_SINGLE ){
		check_num = (lap * STAGE_LAP_ENEMY_MAX) + (round + 1);
		if( check_num == STAGE_LEADER_SET_1ST ){
			tr_index[set_pos] = STAGE_LEADER_TR_INDEX_1ST;
			return;
		}else if( check_num == STAGE_LEADER_SET_2ND ){
			tr_index[set_pos] = STAGE_LEADER_TR_INDEX_2ND;
			return;
		}
	}

#if 0
	/////////////////////////////////////////////////////////////////
	//
	//0)���łɃg���[�i�[�^�C�v��tr_index�Ɋi�[����Ă���
	//
	//1)300�̒����烉���_�����擾
	//
	//2)�����g���[�i�[�^�C�v��T��
	//
	//3)index������Ă��Ȃ����`�F�b�N
	//
	/////////////////////////////////////////////////////////////////
#endif

	do{
		//�^���[�f�[�^�̃g���[�i�[�^�C�v�e�[�u��(300)�̒����烉���_���擾
		no = ( gf_rand() % TOWER_TRTYPE_LIST_MAX );
		start_no = no;

		while( 1 ){

			//�g���[�i�[�^�C�v���擾(��������Ȃ������ꍇ��tr_index[..]���ύX����邱�Ƃ���
			tr_type = tr_index[set_pos+set_count];
			OS_Printf( "�T��tr_type = %d\n", tr_type );

			//�����g���[�i�[�^�C�v��������
			if( tr_type == tower_trtype_list[no] ){

				//�O�̃��E���h�܂ł̃g���[�i�[�C���f�b�N�X�Ɣ�r����
				//for( i=0; i < set_pos ;i++ ){
				
				//�O�̃��E���h�܂łƁA��l�I�o�̎�������̂ŁA��l�ڂ��g���[�i�[�C���f�b�N�X��r
				for( i=0; i < (set_pos+set_count) ;i++ ){
					//if( tr_index[i] == tr_index[set_pos+set_count] ){
					//�����_���Ŏ擾�����C���f�b�N�X(no)������Ă��邩�`�F�b�N
					if( tr_index[i] == no ){
						break;		//����Ă���
					}
				}

				//����Ă��Ȃ������I
				//if( i == set_pos ){

				//���[�v���Ō�܂ŉ���Ă�����A����Ă��Ȃ��I
				if( i == (set_pos+set_count) ){
					//�g���[�i�[�C���f�b�N�X���i�[
					//tr_index[set_count] = tower_trtype_list[no];
					tr_index[set_pos+set_count] = no;
					OS_Printf("�g���[�i�C���f�b�N�X tr_index = %d\n", tr_index[set_pos+set_count]);
					set_count++;
					break;
				}
			}

			//���̃f�[�^��
			no++;

			//�I�[�܂ł����̂œ��ɖ߂�
			if( no >= TOWER_TRTYPE_LIST_MAX ){
				no = 0;
			}

			//////////////////////////
			//�󂫂�������Ȃ�����
			//////////////////////////
			if( no == start_no ){

				//�Ⴄ�g���[�i�[�^�C�v�ɕύX����
				while( 1 ){
					chg_no = ( gf_rand() % STAGE_RANK_TR1_NUM );

					//if( tr_index[set_pos+set_count] != rank_tr1_list[rankno][chg_no] ){
					if( tr_type != rank_tr1_list[rankno][chg_no] ){
						OS_Printf( "���g�p�Ȃ��^�C�v = %d\n", tr_index[set_pos+set_count] );
						tr_index[set_pos+set_count] = rank_tr1_list[rankno][chg_no];
						OS_Printf( "�ύX�����^�C�v = %d\n", tr_index[set_pos+set_count] );
						break;
					}
				}
			}
		}
	}while( set_count < set_num );

	return;
}

#if 0
//�A�[�J�C�u�t�@�C������A�P���������W�J���ăT�[�`�͑�ρI

//�g���[�i�[�f�[�^
B_TOWER_TRAINER_ROM_DATA * Frontier_TrainerRomDataGet(u16 trainer_index,int heap_id)
{
	return	ArchiveDataLoadMalloc(ARC_PL_BTD_TR,trainer_index,heap_id);
}

//�|�P�����f�[�^
void Frontier_PokemonRomDataGet(B_TOWER_POKEMON_ROM_DATA *prd,int index)
{
	ArchiveDataLoad(prd,ARC_PL_STAGE_PM,index);
}
#endif

//
typedef struct{
	u16	index_start;		//�C���f�b�N�X�J�n
	u16	index_end;			//�C���f�b�N�X�I��
}STAGE_POKE_RANGE;

#if 1
//07.12.27�X�V
#define STAGE_POKE_INDEX_START	( 1 )
#else
#define STAGE_POKE_INDEX_START	( 895 )
#endif

static const STAGE_POKE_RANGE stage_poke_range[] = {
	{ STAGE_POKE_INDEX_START,	154 },		//1
	{ STAGE_POKE_INDEX_START,	154 },		//2
	{ STAGE_POKE_INDEX_START,	270 },		//3

	{ STAGE_POKE_INDEX_START,	270 },		//4
	{ STAGE_POKE_INDEX_START,	270 },		//5

	{ 155,						375 },		//6
	{ 155,						375 },		//7
	{ 155,						375 },		//8

	{ 271,						477 },		//9
	{ 271,						477 },		//10
};
#define STAGE_POKE_RANGE_MAX	( NELEMS(stage_poke_range) )

//--------------------------------------------------------------
/**
 * @brief   �G�|�P���������肷��
 *
 * @param   set_num			�I�o���鐔(�K��1)
 * @param   poketype		�I�������^�C�v
 * @param   rank			�I�������^�C�v�̃����N
 * @param   monsno			�������o���|�P�����i���o�[
 * @param   poke_index[]	���肵���|�P�����f�[�^�̃C���f�b�N�X���i�[����|�C���^(2�C���p��)
 *
 * @retval  none
 */
//--------------------------------------------------------------
void Stage_CreatePokeIndex(u8 set_num, u8 poketype, u8 rank, u8 round, u16 monsno, u16 poke_index[])
{
	const STAGE_POKE_RANGE* p_range;
	u8 set_pos,rankno;
	u16 no,rand_range,start_no,i,j;
	int set_count = 0;
	u16 range_no = 0;

	set_pos = (round * STAGE_ENTRY_POKE_MAX);		//�Z�b�g����ʒu

	rankno = Stage_GetRank( rank );

	p_range = &stage_poke_range[rankno];
	rand_range = (p_range->index_end - p_range->index_start);		//�����W�͈�
	OS_Printf( "p_range->index_start = %d\n", p_range->index_start );
	OS_Printf( "p_range->index_end = %d\n", p_range->index_end );

	no = p_range->index_start + ( gf_rand() % rand_range );
	start_no = no;
	no -= STAGE_POKE_INDEX_START;

	while( set_count < set_num ){

		//���܂Ő������Ă����|�P�����Ɣ���Ă��Ȃ����`�F�b�N
		for( j=0; j < set_pos ;j++ ){
			if( poke_index[j] == (no + 1) ){		//��tower poke 0��dummy�Ȃ̂�index��+1
				break;	//�����
			}
		}

		//����Ă��Ȃ�������
		if( j == set_pos ){

			//�H�H�H�^�C�v�̎�
			if( poketype == HATE_TYPE ){

				//���풆�̃|�P�����͏��O����
				if( monsno != stage_poke_no[no] ){
					//poke_index[set_pos+set_count] = no;
					poke_index[set_pos+set_count] = (no + 1);//��tower poke 0��dummy�Ȃ̂�index��+1
					OS_Printf( "poke_no = %d\n", stage_poke_no[no] );
					OS_Printf( "poke_index = %d\n", no );
					set_count++;
				}
			}else{

				//�Е��̃^�C�v����v�����OK
				if( (poketype == stage_poke_type[no][0]) || (poketype == stage_poke_type[no][1]) ){

					//���풆�̃|�P�����͏��O����
					if( monsno != stage_poke_no[no] ){
						//poke_index[set_pos+set_count] = no;
						poke_index[set_pos+set_count] = (no + 1);//��towerpoke 0=dmy�Ȃ̂�index��+1
						OS_Printf( "poke_type0 = %d\n", stage_poke_type[no][0] );
						OS_Printf( "poke_type1 = %d\n", stage_poke_type[no][1] );
						OS_Printf( "poke_no = %d\n", stage_poke_no[no] );
						OS_Printf( "poke_index = %d\n", no );
						set_count++;
					}
				}
			}
		}

		no++;
		//OS_Printf( "no = %d\n", no );

		//�I�[�܂ŒT������������Ȃ������̂œ��ɖ߂�
		if( (no + STAGE_POKE_INDEX_START) >= p_range->index_end ){
			no = (p_range->index_start - STAGE_POKE_INDEX_START);
		}
	};

	return;
}

//--------------------------------------------------------------
/**
 * @brief	�o�g���X�e�[�W�p�@�o�g���p�����[�^����
 */
//--------------------------------------------------------------
BATTLE_PARAM* BtlStage_CreateBattleParam( STAGE_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param )
{
	int i;
	u16 ai;
	u32	val32,rank;
	u8 m_max,e_max,round,set_pos;
	BATTLE_PARAM* bp;
	POKEMON_PARAM* pp;
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* p_rom_tr;
	MYSTATUS* my;
	POKEPARTY* party;

	//�_�u���p��2�l���f�[�^���i�[���Ă���̂Ń��E���h���𒲐�
	//(tr_index[],enemy_poke[],enemy_poke_index[])
	round = (wk->round * STAGE_ENTRY_POKE_MAX);
	OS_Printf( "��������round = %d\n", round );

	//�Q���|�P���������擾
	m_max = Stage_GetMinePokeNum( wk->type );
	e_max = Stage_GetEnemyPokeNum( wk->type );

	//�񕜏���
	party = SaveData_GetTemotiPokemon( wk->sv );
	PokeParty_RecoverAll( party );

	//�퓬�p�����[�^�̐�������(field_battle.c)(fssc_stage.c FSSC_StageCallGetResult��Delete)
	bp = BattleParam_Create( HEAPID_WORLD, Stage_GetFightType(wk->type) );
	
	//�퓬�p�����[�^�̎��W�R�A(�t�B�[���h)
	BattleParam_SetParamByGameDataCore( bp, NULL, 
										ex_param->savedata,
										ex_param->zone_id,
										ex_param->fnote_data,
										ex_param->bag_cursor,
										ex_param->battle_cursor);

	//����͌Œ�I
	bp->bg_id		= BG_ID_STAGE;			//��{�w�i�w��
	bp->ground_id	= GROUND_ID_STAGE;		//��{�n�ʎw��

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//MINE�F�I�񂾎莝���|�P�������Z�b�g
	PokeParty_Init( bp->poke_party[POKEPARTY_MINE], m_max );

	//MINE�F�����̃|�P�����f�[�^�Z�b�g
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < m_max ;i++ ){

		//POKEMON_PARAM�\���̂���POKEMON_PARAM�\���̂փR�s�[
		PokeCopyPPtoPP( PokeParty_GetMemberPointer(party,wk->mine_poke_pos[i]), pp );

		//�퓬�p�����[�^�Z�b�g:�|�P������������
		BattleParam_AddPokemon( bp, pp, POKEPARTY_MINE );
	}
	sys_FreeMemoryEz( pp );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//MINE�F�g���[�i�[�f�[�^����
	BattleParam_TrainerDataMake( bp );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//ENEMY1�FROM����g���[�i�[�f�[�^���m��
	p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[round], HEAPID_WORLD, ARC_PL_BTD_TR );
	//OS_Printf( "�擾����tr_index[%d] = %d\n", round, wk->tr_index[round] );
	sys_FreeMemoryEz( p_rom_tr );

	//ENEMY1�F�g���[�i�[�f�[�^���Z�b�g
	BattleParamTrainerDataSet( bp, &bt_trd, e_max, CLIENT_NO_ENEMY, HEAPID_WORLD );

	//ENEMY1�F�I�񂾎莝���|�P�������Z�b�g
	PokeParty_Init( bp->poke_party[POKEPARTY_ENEMY], e_max );

	//�ʐM�}���`�̂݃����N10�Œ�I(WIFI�͈Ⴄ)
	rank = Stage_GetTypeLevel( wk->csr_pos, &wk->type_level[wk->type][0] );
	//if( Stage_CommCheck(wk->type) == TRUE ){
	if( wk->type == STAGE_TYPE_MULTI ){
		rank = (STAGE_TYPE_LEVEL_MAX - 1);
	}

	//AI�Z�b�g
	ai = Stage_GetStagePokeDataAI( rank );

	//�u���[������AI�Z�b�g
	if( wk->type == STAGE_TYPE_SINGLE ){

		set_pos	= (round * STAGE_ENTRY_POKE_MAX);		//�Z�b�g����ʒu

		if( (wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_1ST) ||
			(wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_2ND) ){
			ai = FR_AI_EXPERT;
			OS_Printf( "BRAIN AI SET\n" );
		}
	}

	for( i=0; i < CLIENT_MAX ;i++ ){
		bp->trainer_data[i].aibit = ai;
	}
	OS_Printf( "�g���[�i�[��AI = %d\n", ai );

	//�G�|�P��������x�ɐ���
	Stage_PokemonParamCreateAll(	&wk->enemy_poke[round], rank, wk->tr_index[round],
									&wk->enemy_poke_index[round], 
									e_max, HEAPID_WORLD, ARC_PL_STAGE_PM );

	//ENEMY1�F�퓬�p�����[�^�Z�b�g:�|�P������������
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < e_max ;i++ ){

		//�o�g���^���[�p�|�P�����f�[�^����POKEMON_PARAM�𐶐�(�����|�P�����Ȃ̂ŌŒ�)
		//Frontier_PokeParaMake( &wk->enemy_poke[round+i], pp, Stage_GetPokeLevel(wk,rank) );
		Frontier_PokeParaMake( &wk->enemy_poke[round], pp, Stage_GetPokeLevel(wk,rank) );
		
		BattleParam_AddPokemon( bp, pp, POKEPARTY_ENEMY );
	}
	sys_FreeMemoryEz( pp );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	switch( wk->type ){

	case STAGE_TYPE_MULTI:
	case STAGE_TYPE_WIFI_MULTI:

		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		//�p�[�g�i�[�̃f�[�^���i�[���Ă����Ȃ��ƃ_���I
		//B_TOWER_PARTNER_DATA	tr_data[2];

		//MINE2�F�g���[�i�[�f�[�^����
		BattleParam_TrainerDataMake( bp );

		//MINE2�F�p�[�g�i�[��MyStatus���擾���ăR�s�[
		my = CommInfoGetMyStatus( 1 - CommGetCurrentID() );
		MyStatus_Copy( my, bp->my_status[CLIENT_NO_MINE2] );

		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		//�p�[�g�i�[�̃f�[�^���i�[���Ă����Ȃ��ƃ_���I
		//B_TOWER_PARTNER_DATA	tr_data[2];

		//ENEMY2�FROM����g���[�i�[�f�[�^���m��
		p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[round+1], HEAPID_WORLD, 
											ARC_PL_BTD_TR );
		//OS_Printf( "�擾����tr_index[%d+1] = %d\n", round, wk->tr_index[round+1] );
		sys_FreeMemoryEz( p_rom_tr );

		//ENEMY2�F�g���[�i�[�f�[�^���Z�b�g
		BattleParamTrainerDataSet( bp, &bt_trd, e_max, CLIENT_NO_ENEMY2, HEAPID_WORLD );

#if 1
		//ENEMY2�F�I�񂾎莝���|�P�������Z�b�g
		PokeParty_Init( bp->poke_party[POKEPARTY_ENEMY_PAIR], e_max );

		//ENEMY2�F�퓬�p�����[�^�Z�b�g:�|�P������������
		pp = PokemonParam_AllocWork( HEAPID_WORLD );

		//�o�g���^���[�p�|�P�����f�[�^����POKEMON_PARAM�𐶐�
		//Frontier_PokeParaMake( &wk->enemy_poke[round+1], pp, Stage_GetPokeLevel(wk,rank));
		//�����|�P�������Z�b�g���Ă���(����ŗǂ��H)
		Frontier_PokeParaMake( &wk->enemy_poke[round], pp, Stage_GetPokeLevel(wk,rank) );

		//POKEMON_PARAM�\���̂���POKEMON_PARAM�\���̂փR�s�[
		//PokeCopyPPtoPP( PokeParty_GetMemberPointer(party,wk->mine_poke_pos[1]), pp );
		
		BattleParam_AddPokemon( bp, pp, POKEPARTY_ENEMY_PAIR );

		sys_FreeMemoryEz( pp );
#endif

		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

		break;
	};

	return bp;
}

//--------------------------------------------------------------
/**
 * @brief	�v���C���[�h����FIGHT_TYPE��Ԃ�
 */
//--------------------------------------------------------------
static u32 Stage_GetFightType( u8 type )
{
	switch( type ){

	case STAGE_TYPE_SINGLE:
		return FIGHT_TYPE_BATTLE_TOWER_1vs1;

	case STAGE_TYPE_DOUBLE:
		return FIGHT_TYPE_BATTLE_TOWER_2vs2;

	case STAGE_TYPE_MULTI:
		return FIGHT_TYPE_BATTLE_TOWER_SIO_MULTI;	
		//return FIGHT_TYPE_MULTI_SIO;	

	case STAGE_TYPE_WIFI_MULTI:
		return FIGHT_TYPE_BATTLE_TOWER_SIO_MULTI;
		//return FIGHT_TYPE_MULTI_SIO;	
	}

	return FIGHT_TYPE_BATTLE_TOWER_1vs1;
}

//--------------------------------------------------------------
/**
 * @brief   �^�C�v�ɂ����1�l�̃g���[�i�[���������̃|�P�����̐����擾
 *
 * @param   type
 *
 * @retval  "�|�P�����̐�"
 */
//--------------------------------------------------------------
u8 Stage_GetMinePokeNum( u8 type )
{
	switch( type ){

	case STAGE_TYPE_DOUBLE:
		return 2;
	};

	return 1;
}

//--------------------------------------------------------------
/**
 * @brief   �^�C�v�ɂ����1�l�̃g���[�i�[�����G�|�P�����̐����擾
 *
 * @param   type
 *
 * @retval  "�|�P�����̐�"
 */
//--------------------------------------------------------------
u8 Stage_GetEnemyPokeNum( u8 type )
{
	switch( type ){

	case STAGE_TYPE_DOUBLE:
		return 2;
	};

	return 1;
}

//--------------------------------------------------------------
/**
 * @brief   �o�g���X�e�[�W�Ŏg�p����|�P��������x�ɕ����C����
 *
 * @param   pwd					�|�P�����p�����[�^�W�J�ꏊ(set_num���̗v�f��������z��ł��鎖)
 * @param   type_level			�^�C�v���Ƃ̃��x��
 * @param   poke_index		
 * @param   set_num				���̂̃|�P�����𐶐����邩
 * @param   heap_id				�q�[�vID
 */
//--------------------------------------------------------------
static void Stage_PokemonParamCreateAll( B_TOWER_POKEMON *pwd, u8 type_level, u16 tr_index, u16 poke_index[], int set_num, int heap_id, int arcPM )
{
	int i;
	u32 per_rnd;
	u8 power_rnd;
	u32 num;

	//�u���[���̎��͓���
	if( tr_index == STAGE_LEADER_TR_INDEX_1ST ){
		num = 27;
	}else if( tr_index == STAGE_LEADER_TR_INDEX_2ND ){
		num = 31;
	}else{
		num = Stage_GetStagePokeDataPowRnd( type_level );
	}
	
	for( i=0; i < set_num; i++ ){
		OS_Printf( "*********************\npoke_index[%d] = %d\n", i, poke_index[i] );
		Frontier_PokemonParamCreate(	&pwd[i], poke_index[i], i, num, num, heap_id, arcPM );
	}

	return;
}

//�����l�ڂ���LV�Ƀv���X����
static u32 Stage_GetPokeLevel( STAGE_SCRWORK* wk, u8 rank )
{
	u8 set_pos;
	u32 lv;
	int mine_lv;

	set_pos	= (wk->round * STAGE_ENTRY_POKE_MAX);		//�Z�b�g����ʒu

	//�u���[���̎��́A�|�P�����̃��x���������̎莝���|�P���������N�ɍ��킹��
	if( (wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_1ST) ||
		(wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_2ND) ){

		//�莝���|�P�����̃��x�����烉���N���擾
		mine_lv = Stage_GetMineLevelRank( wk );
		lv = (mine_lv * 10) + 1 + wk->round;
		if( lv > 100 ){
			lv = 100;
		}
	}else{
		//���E���h����0�X�^�[�g�Ȃ̂ŁA+1���Čv�Z
		lv = ( Stage_GetStagePokeDataLevel(rank) + 1 + wk->round );
	}

	OS_Printf( "�G�|�P������LV = %d\n", lv );
	OS_Printf( "wk->round = %d\n", wk->round );	
	return lv;
}

//�^�C�v���уe�[�u��
static const u8 type_tbl[TYPE_X_MAX*TYPE_Y_MAX] = {
	NORMAL_TYPE,
	FIRE_TYPE,
	WATER_TYPE,
	ELECTRIC_TYPE,

	KUSA_TYPE,
	KOORI_TYPE,
	BATTLE_TYPE,
	POISON_TYPE,

	JIMEN_TYPE,
	HIKOU_TYPE,
	SP_TYPE,
	MUSHI_TYPE,

	IWA_TYPE,
	GHOST_TYPE,
	DRAGON_TYPE,
	AKU_TYPE,

	METAL_TYPE,
	DUMMY_TYPE,			//���ӁI
	DUMMY_TYPE,			//���ӁI
	HATE_TYPE,			//�H�H�H
};

//--------------------------------------------------------------
/**
 * @brief   �J�[�\���ʒu�̃|�P�����^�C�v���擾
 *
 * @param   csr_pos		�J�[�\���ʒu
 */
//--------------------------------------------------------------
u8 Stage_GetPokeType( u8 csr_pos )
{
	return type_tbl[csr_pos];
}

//--------------------------------------------------------------
/**
 * @brief	�ʐM�^�C�v���`�F�b�N
 *
 * @param	type		�^�C�v
 *
 * @return	"FALSE = �ʐM�ł͂Ȃ�"
 * @return	"TRUE  = �ʐM�ł���"
 */
//--------------------------------------------------------------
BOOL Stage_CommCheck( u8 type );
BOOL Stage_CommCheck( u8 type )
{
	switch( type ){
	case STAGE_TYPE_MULTI:
	case STAGE_TYPE_WIFI_MULTI:
		return TRUE;
	};

	return FALSE;

}

//--------------------------------------------------------------
/**
 * @brief	�莝���|�P�����̃��x�����烉���N���擾
 *
 * @param	wk		
 *
 * @return	"�����N"
 */
//--------------------------------------------------------------
u8 Stage_GetMineLevelRank( STAGE_SCRWORK* wk )
{
	u32 lv;
	int mine_lv;
	POKEPARTY* party;
	POKEMON_PARAM* poke;

	//�莝���|�P�����̃��x�����擾
	party = SaveData_GetTemotiPokemon( wk->sv );
	OS_Printf( "�莝���ꏊ = %d\n", wk->mine_poke_pos[0] );
	poke = PokeParty_GetMemberPointer( party, wk->mine_poke_pos[0] );
	mine_lv = PokeParaGet( poke, ID_PARA_level, NULL );

	OS_Printf( "level(�����N) = %d\n", (mine_lv / 10) );
	return ( mine_lv / 10 );
}

//--------------------------------------------------------------
/**
 * @brief	stage_poke_data:���x�����擾
 *
 * @param	rank		�����N
 *
 * @return	"���x��"
 */
//--------------------------------------------------------------
static u8 Stage_GetStagePokeDataLevel( u8 rank )
{
	return stage_poke_data[ Stage_GetRank(rank) ].level;
}


//--------------------------------------------------------------
/**
 * @brief	stage_poke_data:�p���[�������擾
 *
 * @param	rank		�����N
 *
 * @return	"���x��"
 */
//--------------------------------------------------------------
static u8 Stage_GetStagePokeDataPowRnd( u8 rank )
{
	return stage_poke_data[ Stage_GetRank(rank) ].pow_rnd;
}


//--------------------------------------------------------------
/**
 * @brief	stage_poke_data:AI���擾
 *
 * @param	rank		�����N
 *
 * @return	"���x��"
 */
//--------------------------------------------------------------
static u16 Stage_GetStagePokeDataAI( u8 rank )
{
	return stage_poke_data[ Stage_GetRank(rank) ].ai;
}

//--------------------------------------------------------------
/**
 * @brief	�����������������N���擾
 *
 * @param	rank		�����N
 *
 * @return	"�����������������N"
 */
//--------------------------------------------------------------
static u16 Stage_GetRank( u8 rank )
{
	u8 no;
	no = rank;

	//�����N�̒l�ɐ�����������
	if( no >= STAGE_TYPE_LEVEL_MAX ){
		no = (STAGE_TYPE_LEVEL_MAX - 1);
	}

	return no;
}


