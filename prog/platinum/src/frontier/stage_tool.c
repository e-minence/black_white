//==============================================================================
/**
 * @file	stage_tool.c
 * @brief	バトルステージ関連ツール類
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
//	extern宣言
//
//==============================================================================
//トレーナーデータ生成（自分側）
extern void BattleParam_TrainerDataMake( BATTLE_PARAM* bp );
//トレーナーデータ生成（敵側）
extern void BattleParam_EnemyTrainerDataMake( BATTLE_PARAM* bp );


//==============================================================================
//	データ
//==============================================================================
#include "battle/trtype_def.h"

//サーチ用にデータテーブルをたくさん作ったのでファイルを分けた
#include "stage.dat"


//==============================================================================
//
//
//
//==============================================================================
//ポケモンの基本設定データ
typedef struct{
	u8 level;			//ポケモンレベル
	u8 pow_rnd;			//パワー乱数
	u16 ai;				//AI
}STAGE_POKE_DATA;

//ランクに対応
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

//ボスは固定
//90戦目(5週目最後)		LV50	31 エキスパート
//180戦目(10週目最後)	LV100	31 エキスパート


//==============================================================================
//	プロトタイプ宣言
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
 * @brief   トレーナー"タイプ"を決定
 *
 * @param   type			シングル、ダブルなど
 * @param   rank			ランク(0-9,10)
 * @param   round			ラウンド数
 * @param   tr_index		トレーナータイプを格納するポインタ
 *
 * @retval  none
 *
 * 「たんぱんこぞう、たんぱんこぞう」という形で選出されても問題ない
 * トレーナーのインデックスを取得する時に同じ「たんぱんこぞう」にならなければ良い
 * 使用していない「たんぱんこぞう」が存在しない時は、
 * トレーナータイプをもう一度選出しなければいけない
 */
//--------------------------------------------------------------
void Stage_CreateTrType( u8 csr_pos, u8 set_num, u8 rank, u8 round, u16 tr_index[] )
{
	int i;
	u16 no;
	u8 set_pos,rankno;
	int set_count = 0;

	set_pos = (round * STAGE_ENTRY_POKE_MAX);		//セットする位置

	rankno = Stage_GetRank( rank );

	OS_Printf( "カーソル位置 = %d\n", csr_pos );
	OS_Printf( "ポケモンタイプ = %d\n", Stage_GetPokeType(csr_pos) );
	OS_Printf( "ランク = %d\n", rankno );
	OS_Printf( "ラウンド = %d\n", round );

	do{
		no = ( gf_rand() % STAGE_RANK_TR_MAX );

		//テーブルからトレーナータイプを取得
		if( no < STAGE_RANK_TR1_NUM ){
			tr_index[set_pos+set_count] = rank_tr1_list[rankno][no];
		}else{
			tr_index[set_pos+set_count] = rank_tr2_list[csr_pos][no-STAGE_RANK_TR1_NUM];
		}

		OS_Printf( "トレーナータイプ tr_index = %d\n", tr_index[set_pos+set_count] );
		set_count++;

	}while( set_count < set_num );

	return;
}

//--------------------------------------------------------------
/**
 * @brief   トレーナー"インデックス"を決定
 *
 * @param   type			シングル、ダブルなど
 * @param   rank			ランク(0-9,10)
 * @param   round			ラウンド数
 * @param   tr_index		トレーナーインデックスを格納するポインタ(トレーナータイプを格納しておく)
 *
 * @retval  none
 *
 * ループ長いかも、、、確認する！
 *
 * サーチが遅かったらデータテーブルをトレーナータイプ別にソートしておいて、
 * トレーナーインデックスもデータに含めておくなどが必要
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
	set_pos		= (round * STAGE_ENTRY_POKE_MAX);		//セットする位置
	
	rankno = Stage_GetRank( rank );

	OS_Printf( "ランク = %d\n", rankno );
	OS_Printf( "ラウンド = %d\n", round );

	//ブレーン(シングルのみ)
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
	//0)すでにトレーナータイプがtr_indexに格納されている
	//
	//1)300の中からランダムを取得
	//
	//2)同じトレーナータイプを探す
	//
	//3)indexが被っていないかチェック
	//
	/////////////////////////////////////////////////////////////////
#endif

	do{
		//タワーデータのトレーナータイプテーブル(300)の中からランダム取得
		no = ( gf_rand() % TOWER_TRTYPE_LIST_MAX );
		start_no = no;

		while( 1 ){

			//トレーナータイプを取得(★見つからなかった場合はtr_index[..]が変更されることあり
			tr_type = tr_index[set_pos+set_count];
			OS_Printf( "探すtr_type = %d\n", tr_type );

			//同じトレーナータイプを見つけた
			if( tr_type == tower_trtype_list[no] ){

				//前のラウンドまでのトレーナーインデックスと比較する
				//for( i=0; i < set_pos ;i++ ){
				
				//前のラウンドまでと、二人選出の時があるので、一人目もトレーナーインデックス比較
				for( i=0; i < (set_pos+set_count) ;i++ ){
					//if( tr_index[i] == tr_index[set_pos+set_count] ){
					//ランダムで取得したインデックス(no)が被っているかチェック
					if( tr_index[i] == no ){
						break;		//被っている
					}
				}

				//被っていなかった！
				//if( i == set_pos ){

				//ループが最後まで回っていたら、被っていない！
				if( i == (set_pos+set_count) ){
					//トレーナーインデックスを格納
					//tr_index[set_count] = tower_trtype_list[no];
					tr_index[set_pos+set_count] = no;
					OS_Printf("トレーナインデックス tr_index = %d\n", tr_index[set_pos+set_count]);
					set_count++;
					break;
				}
			}

			//次のデータへ
			no++;

			//終端まできたので頭に戻す
			if( no >= TOWER_TRTYPE_LIST_MAX ){
				no = 0;
			}

			//////////////////////////
			//空きが見つからなかった
			//////////////////////////
			if( no == start_no ){

				//違うトレーナータイプに変更する
				while( 1 ){
					chg_no = ( gf_rand() % STAGE_RANK_TR1_NUM );

					//if( tr_index[set_pos+set_count] != rank_tr1_list[rankno][chg_no] ){
					if( tr_type != rank_tr1_list[rankno][chg_no] ){
						OS_Printf( "未使用なしタイプ = %d\n", tr_index[set_pos+set_count] );
						tr_index[set_pos+set_count] = rank_tr1_list[rankno][chg_no];
						OS_Printf( "変更したタイプ = %d\n", tr_index[set_pos+set_count] );
						break;
					}
				}
			}
		}
	}while( set_count < set_num );

	return;
}

#if 0
//アーカイブファイルから、１つずつメモリ展開してサーチは大変！

//トレーナーデータ
B_TOWER_TRAINER_ROM_DATA * Frontier_TrainerRomDataGet(u16 trainer_index,int heap_id)
{
	return	ArchiveDataLoadMalloc(ARC_PL_BTD_TR,trainer_index,heap_id);
}

//ポケモンデータ
void Frontier_PokemonRomDataGet(B_TOWER_POKEMON_ROM_DATA *prd,int index)
{
	ArchiveDataLoad(prd,ARC_PL_STAGE_PM,index);
}
#endif

//
typedef struct{
	u16	index_start;		//インデックス開始
	u16	index_end;			//インデックス終了
}STAGE_POKE_RANGE;

#if 1
//07.12.27更新
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
 * @brief   敵ポケモンを決定する
 *
 * @param   set_num			選出する数(必ず1)
 * @param   poketype		選択したタイプ
 * @param   rank			選択したタイプのランク
 * @param   monsno			自分が出すポケモンナンバー
 * @param   poke_index[]	決定したポケモンデータのインデックスを格納するポインタ(2匹分用意)
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

	set_pos = (round * STAGE_ENTRY_POKE_MAX);		//セットする位置

	rankno = Stage_GetRank( rank );

	p_range = &stage_poke_range[rankno];
	rand_range = (p_range->index_end - p_range->index_start);		//レンジ範囲
	OS_Printf( "p_range->index_start = %d\n", p_range->index_start );
	OS_Printf( "p_range->index_end = %d\n", p_range->index_end );

	no = p_range->index_start + ( gf_rand() % rand_range );
	start_no = no;
	no -= STAGE_POKE_INDEX_START;

	while( set_count < set_num ){

		//今まで生成してきたポケモンと被っていないかチェック
		for( j=0; j < set_pos ;j++ ){
			if( poke_index[j] == (no + 1) ){		//★tower poke 0はdummyなのでindexを+1
				break;	//被った
			}
		}

		//被っていなかった時
		if( j == set_pos ){

			//？？？タイプの時
			if( poketype == HATE_TYPE ){

				//挑戦中のポケモンは除外する
				if( monsno != stage_poke_no[no] ){
					//poke_index[set_pos+set_count] = no;
					poke_index[set_pos+set_count] = (no + 1);//★tower poke 0はdummyなのでindexを+1
					OS_Printf( "poke_no = %d\n", stage_poke_no[no] );
					OS_Printf( "poke_index = %d\n", no );
					set_count++;
				}
			}else{

				//片方のタイプが一致すればOK
				if( (poketype == stage_poke_type[no][0]) || (poketype == stage_poke_type[no][1]) ){

					//挑戦中のポケモンは除外する
					if( monsno != stage_poke_no[no] ){
						//poke_index[set_pos+set_count] = no;
						poke_index[set_pos+set_count] = (no + 1);//★towerpoke 0=dmyなのでindexを+1
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

		//終端まで探したが見つからなかったので頭に戻る
		if( (no + STAGE_POKE_INDEX_START) >= p_range->index_end ){
			no = (p_range->index_start - STAGE_POKE_INDEX_START);
		}
	};

	return;
}

//--------------------------------------------------------------
/**
 * @brief	バトルステージ用　バトルパラメータ生成
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

	//ダブル用に2人ずつデータを格納しているのでラウンド数を調整
	//(tr_index[],enemy_poke[],enemy_poke_index[])
	round = (wk->round * STAGE_ENTRY_POKE_MAX);
	OS_Printf( "調整したround = %d\n", round );

	//参加ポケモン数を取得
	m_max = Stage_GetMinePokeNum( wk->type );
	e_max = Stage_GetEnemyPokeNum( wk->type );

	//回復処理
	party = SaveData_GetTemotiPokemon( wk->sv );
	PokeParty_RecoverAll( party );

	//戦闘パラメータの生成処理(field_battle.c)(fssc_stage.c FSSC_StageCallGetResultでDelete)
	bp = BattleParam_Create( HEAPID_WORLD, Stage_GetFightType(wk->type) );
	
	//戦闘パラメータの収集コア(フィールド)
	BattleParam_SetParamByGameDataCore( bp, NULL, 
										ex_param->savedata,
										ex_param->zone_id,
										ex_param->fnote_data,
										ex_param->bag_cursor,
										ex_param->battle_cursor);

	//現状は固定！
	bp->bg_id		= BG_ID_STAGE;			//基本背景指定
	bp->ground_id	= GROUND_ID_STAGE;		//基本地面指定

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//MINE：選んだ手持ちポケモンをセット
	PokeParty_Init( bp->poke_party[POKEPARTY_MINE], m_max );

	//MINE：自分のポケモンデータセット
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < m_max ;i++ ){

		//POKEMON_PARAM構造体からPOKEMON_PARAM構造体へコピー
		PokeCopyPPtoPP( PokeParty_GetMemberPointer(party,wk->mine_poke_pos[i]), pp );

		//戦闘パラメータセット:ポケモンを加える
		BattleParam_AddPokemon( bp, pp, POKEPARTY_MINE );
	}
	sys_FreeMemoryEz( pp );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//MINE：トレーナーデータ生成
	BattleParam_TrainerDataMake( bp );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//ENEMY1：ROMからトレーナーデータを確保
	p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[round], HEAPID_WORLD, ARC_PL_BTD_TR );
	//OS_Printf( "取得したtr_index[%d] = %d\n", round, wk->tr_index[round] );
	sys_FreeMemoryEz( p_rom_tr );

	//ENEMY1：トレーナーデータをセット
	BattleParamTrainerDataSet( bp, &bt_trd, e_max, CLIENT_NO_ENEMY, HEAPID_WORLD );

	//ENEMY1：選んだ手持ちポケモンをセット
	PokeParty_Init( bp->poke_party[POKEPARTY_ENEMY], e_max );

	//通信マルチのみランク10固定！(WIFIは違う)
	rank = Stage_GetTypeLevel( wk->csr_pos, &wk->type_level[wk->type][0] );
	//if( Stage_CommCheck(wk->type) == TRUE ){
	if( wk->type == STAGE_TYPE_MULTI ){
		rank = (STAGE_TYPE_LEVEL_MAX - 1);
	}

	//AIセット
	ai = Stage_GetStagePokeDataAI( rank );

	//ブレーン時のAIセット
	if( wk->type == STAGE_TYPE_SINGLE ){

		set_pos	= (round * STAGE_ENTRY_POKE_MAX);		//セットする位置

		if( (wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_1ST) ||
			(wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_2ND) ){
			ai = FR_AI_EXPERT;
			OS_Printf( "BRAIN AI SET\n" );
		}
	}

	for( i=0; i < CLIENT_MAX ;i++ ){
		bp->trainer_data[i].aibit = ai;
	}
	OS_Printf( "トレーナーのAI = %d\n", ai );

	//敵ポケモンを一度に生成
	Stage_PokemonParamCreateAll(	&wk->enemy_poke[round], rank, wk->tr_index[round],
									&wk->enemy_poke_index[round], 
									e_max, HEAPID_WORLD, ARC_PL_STAGE_PM );

	//ENEMY1：戦闘パラメータセット:ポケモンを加える
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < e_max ;i++ ){

		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成(同じポケモンなので固定)
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
		//パートナーのデータを格納しておかないとダメ！
		//B_TOWER_PARTNER_DATA	tr_data[2];

		//MINE2：トレーナーデータ生成
		BattleParam_TrainerDataMake( bp );

		//MINE2：パートナーのMyStatusを取得してコピー
		my = CommInfoGetMyStatus( 1 - CommGetCurrentID() );
		MyStatus_Copy( my, bp->my_status[CLIENT_NO_MINE2] );

		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		//パートナーのデータを格納しておかないとダメ！
		//B_TOWER_PARTNER_DATA	tr_data[2];

		//ENEMY2：ROMからトレーナーデータを確保
		p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[round+1], HEAPID_WORLD, 
											ARC_PL_BTD_TR );
		//OS_Printf( "取得したtr_index[%d+1] = %d\n", round, wk->tr_index[round+1] );
		sys_FreeMemoryEz( p_rom_tr );

		//ENEMY2：トレーナーデータをセット
		BattleParamTrainerDataSet( bp, &bt_trd, e_max, CLIENT_NO_ENEMY2, HEAPID_WORLD );

#if 1
		//ENEMY2：選んだ手持ちポケモンをセット
		PokeParty_Init( bp->poke_party[POKEPARTY_ENEMY_PAIR], e_max );

		//ENEMY2：戦闘パラメータセット:ポケモンを加える
		pp = PokemonParam_AllocWork( HEAPID_WORLD );

		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		//Frontier_PokeParaMake( &wk->enemy_poke[round+1], pp, Stage_GetPokeLevel(wk,rank));
		//同じポケモンをセットしている(これで良い？)
		Frontier_PokeParaMake( &wk->enemy_poke[round], pp, Stage_GetPokeLevel(wk,rank) );

		//POKEMON_PARAM構造体からPOKEMON_PARAM構造体へコピー
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
 * @brief	プレイモードからFIGHT_TYPEを返す
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
 * @brief   タイプによって1人のトレーナーが持つ自分のポケモンの数を取得
 *
 * @param   type
 *
 * @retval  "ポケモンの数"
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
 * @brief   タイプによって1人のトレーナーが持つ敵ポケモンの数を取得
 *
 * @param   type
 *
 * @retval  "ポケモンの数"
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
 * @brief   バトルステージで使用するポケモンを一度に複数匹生成
 *
 * @param   pwd					ポケモンパラメータ展開場所(set_num分の要素数がある配列である事)
 * @param   type_level			タイプごとのレベル
 * @param   poke_index		
 * @param   set_num				何体のポケモンを生成するか
 * @param   heap_id				ヒープID
 */
//--------------------------------------------------------------
static void Stage_PokemonParamCreateAll( B_TOWER_POKEMON *pwd, u8 type_level, u16 tr_index, u16 poke_index[], int set_num, int heap_id, int arcPM )
{
	int i;
	u32 per_rnd;
	u8 power_rnd;
	u32 num;

	//ブレーンの時は特殊
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

//今何人目かをLVにプラスする
static u32 Stage_GetPokeLevel( STAGE_SCRWORK* wk, u8 rank )
{
	u8 set_pos;
	u32 lv;
	int mine_lv;

	set_pos	= (wk->round * STAGE_ENTRY_POKE_MAX);		//セットする位置

	//ブレーンの時は、ポケモンのレベルを自分の手持ちポケモンランクに合わせる
	if( (wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_1ST) ||
		(wk->tr_index[set_pos] == STAGE_LEADER_TR_INDEX_2ND) ){

		//手持ちポケモンのレベルからランクを取得
		mine_lv = Stage_GetMineLevelRank( wk );
		lv = (mine_lv * 10) + 1 + wk->round;
		if( lv > 100 ){
			lv = 100;
		}
	}else{
		//ラウンド数は0スタートなので、+1して計算
		lv = ( Stage_GetStagePokeDataLevel(rank) + 1 + wk->round );
	}

	OS_Printf( "敵ポケモンのLV = %d\n", lv );
	OS_Printf( "wk->round = %d\n", wk->round );	
	return lv;
}

//タイプ並びテーブル
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
	DUMMY_TYPE,			//注意！
	DUMMY_TYPE,			//注意！
	HATE_TYPE,			//？？？
};

//--------------------------------------------------------------
/**
 * @brief   カーソル位置のポケモンタイプを取得
 *
 * @param   csr_pos		カーソル位置
 */
//--------------------------------------------------------------
u8 Stage_GetPokeType( u8 csr_pos )
{
	return type_tbl[csr_pos];
}

//--------------------------------------------------------------
/**
 * @brief	通信タイプかチェック
 *
 * @param	type		タイプ
 *
 * @return	"FALSE = 通信ではない"
 * @return	"TRUE  = 通信である"
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
 * @brief	手持ちポケモンのレベルからランクを取得
 *
 * @param	wk		
 *
 * @return	"ランク"
 */
//--------------------------------------------------------------
u8 Stage_GetMineLevelRank( STAGE_SCRWORK* wk )
{
	u32 lv;
	int mine_lv;
	POKEPARTY* party;
	POKEMON_PARAM* poke;

	//手持ちポケモンのレベルを取得
	party = SaveData_GetTemotiPokemon( wk->sv );
	OS_Printf( "手持ち場所 = %d\n", wk->mine_poke_pos[0] );
	poke = PokeParty_GetMemberPointer( party, wk->mine_poke_pos[0] );
	mine_lv = PokeParaGet( poke, ID_PARA_level, NULL );

	OS_Printf( "level(ランク) = %d\n", (mine_lv / 10) );
	return ( mine_lv / 10 );
}

//--------------------------------------------------------------
/**
 * @brief	stage_poke_data:レベルを取得
 *
 * @param	rank		ランク
 *
 * @return	"レベル"
 */
//--------------------------------------------------------------
static u8 Stage_GetStagePokeDataLevel( u8 rank )
{
	return stage_poke_data[ Stage_GetRank(rank) ].level;
}


//--------------------------------------------------------------
/**
 * @brief	stage_poke_data:パワー乱数を取得
 *
 * @param	rank		ランク
 *
 * @return	"レベル"
 */
//--------------------------------------------------------------
static u8 Stage_GetStagePokeDataPowRnd( u8 rank )
{
	return stage_poke_data[ Stage_GetRank(rank) ].pow_rnd;
}


//--------------------------------------------------------------
/**
 * @brief	stage_poke_data:AIを取得
 *
 * @param	rank		ランク
 *
 * @return	"レベル"
 */
//--------------------------------------------------------------
static u16 Stage_GetStagePokeDataAI( u8 rank )
{
	return stage_poke_data[ Stage_GetRank(rank) ].ai;
}

//--------------------------------------------------------------
/**
 * @brief	制限をかけたランクを取得
 *
 * @param	rank		ランク
 *
 * @return	"制限をかけたランク"
 */
//--------------------------------------------------------------
static u16 Stage_GetRank( u8 rank )
{
	u8 no;
	no = rank;

	//ランクの値に制限をかける
	if( no >= STAGE_TYPE_LEVEL_MAX ){
		no = (STAGE_TYPE_LEVEL_MAX - 1);
	}

	return no;
}


