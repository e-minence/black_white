//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		  fld_trade.c
 *	@brief		ゲーム内交換
 *	@author		tomoya takahashi (obata)
 *	@data		  2006.05.15 (2009.12.09 HGSSより移植)
 *
 *	このソースは field_tradeオーバーレイ領域に属しています
 *	このソースに定義された全関数は field_tradeオーバーレイ領域を明示的にロードしてからでないと
 *	利用できないので注意してください！
 *
 *	オーバーレイロード＆アンロード構文
 *
 *	#include "system/pm_overlay.h"
 *	
 *	FS_EXTERN_OVERLAY(field_trade);
 *	Overlay_Load( FS_OVERLAY_ID( field_trade ), OVERLAY_LOAD_NOT_SYNCHRONIZE );
 *	Overlay_UnloadID( FS_OVERLAY_ID( field_trade ) );
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "fld_trade_local.h"
#include "fld_trade.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "savedata/mystatus.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "arc/arc_def.h"  // for ARCID_MESSAGE, ARCID_FLD_TRADE_POKE
#include "arc/message.naix"  // for NARC_message_fld_trade_dat
#include "net_app/pokemontrade.h"  // for PokemonTradeDemoProcData
#include "event_fieldmap_control.h"  // for EVENT_FieldSubProc
#include "system/main.h"  // for HEAPID_PROC
#include "savedata/zukan_savedata.h"  // for ZUKANSAVE_xxxx
#include "../../../resource/fld_trade/fld_trade_list.h"  // for FLD_TRADE_POKE_xxxx
#include "poke_tool/shinka_check.h"
#include "demo/shinka_demo.h"
#include "poke_tool/poke_memo.h"
#include "include/msg/msg_fld_trade.h"

#define DEBUG_MODE

FS_EXTERN_OVERLAY(pokemon_trade);


//=============================================================================
// ■ゲーム内交換ワーク
//=============================================================================
struct _FLD_TRADE_WORK {

	HEAPID              heapID;
	u32                 tradeNo;	  // 交換ナンバー
	FLD_TRADE_POKEDATA* pokeData;		// 交換ポケモンデータ
	POKEMON_PARAM*		  pokeParam;	// 交換ポケモン
	MYSTATUS*			      pokeOya;		// 交換相手パラメータ

};


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static STRBUF* GetTradeMsgData( u32 heapID, u32 idx ); 
static void SetPokemonParam( GAMEDATA* gameData, HEAPID heapID, 
                             POKEMON_PARAM* pp, FLD_TRADE_POKEDATA* data, 
	                           u32 tradeNo ); 
#ifdef DEBUG_MODE
static void PP_Dump( const POKEMON_PARAM* pp );
static void FTP_Dump( const FLD_TRADE_POKEDATA* ftp );
#endif


//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム内交換ワークを生成する
 *
 *	@param	heap_id	ヒープID
 *	@param	tradeNo	交換番号
 */
//-----------------------------------------------------------------------------
FLD_TRADE_WORK* FLD_TRADE_WORK_Create( u32 heap_id, u32 tradeNo )
{
	FLD_TRADE_WORK* work;
	STRBUF* strbuf;
	STRCODE str_arry[ 128 ];

	GF_ASSERT( tradeNo < FLD_TRADE_POKE_NUM );
	
	work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(FLD_TRADE_WORK) );
	work->heapID   = heap_id;
	work->tradeNo = tradeNo;

	// 交換データ読み込み
  work->pokeData = GFL_ARC_LoadDataAllocOfs( ARCID_FLD_TRADE_POKE, tradeNo, 
                                               heap_id, 0, sizeof(FLD_TRADE_POKEDATA) );
  // 交換ポケモンデータ作成
	work->pokeParam = GFL_HEAP_AllocMemory( heap_id, sizeof(POKEMON_PARAM) );

	// 親データ作成
	work->pokeOya = MyStatus_AllocWork( heap_id );
	MyStatus_Init( work->pokeOya );
	strbuf = GetTradeMsgData( heap_id, work->pokeData->str_id_oya_name );
	GFL_STR_GetStringCode( strbuf, str_arry, 128 );
	GFL_STR_DeleteBuffer( strbuf );
	MyStatus_SetMyName( work->pokeOya, str_arry );
	MyStatus_SetMySex( work->pokeOya, work->pokeData->oya_sex );
	
	return work;
}

//----------------------------------------------------------------------------
/**
 *	@brief ゲーム内交換ワークを破棄する
 *
 *	@param	work		
 */
//-----------------------------------------------------------------------------
void FLD_TRADE_WORK_Delete( FLD_TRADE_WORK* work )
{
	GFL_HEAP_FreeMemory( work->pokeData );
	GFL_HEAP_FreeMemory( work->pokeParam );
	GFL_HEAP_FreeMemory( work->pokeOya );
	GFL_HEAP_FreeMemory( work );
}

//----------------------------------------------------------------------------
/**
 *	@brief	交換するポケモンナンバー取得
 *
 *	@param	cwork		ワーク
 *
 *	@return	ポケモンナンバー
 */
//-----------------------------------------------------------------------------
u32 FLD_TRADE_WORK_GetTradeMonsno( const FLD_TRADE_WORK* cwork )
{
	return cwork->pokeData->monsno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	交換に必要なポケモンナンバー取得
 *
 *	@param	cwork		ワーク
 *
 *	@return	ポケモンナンバー
 */
//-----------------------------------------------------------------------------
u32 FLD_TRADE_WORK_GetChangeMonsno( const FLD_TRADE_WORK* cwork )
{
	return cwork->pokeData->change_monsno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	交換に必要なポケモンの性別を取得
 *
 *	@param	cwork		ワーク
 *
 *	@return	必要なポケモンの性別（0:オス　1:メス　2:問わず（もしくは性別なし）
 *          MONS_MALE、MONS_FEMALE、MONS_UNKに準拠してます。
 */
//-----------------------------------------------------------------------------
u32 FLD_TRADE_WORK_GetChangeMonsSex( const FLD_TRADE_WORK* cwork )
{
	return cwork->pokeData->change_monssex;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親性別　取得
 *
 *	@param	cwork		ワーク
 *
 *	@return	性別
 */
//-----------------------------------------------------------------------------
u32 FLD_TRADE_WORK_GetTradeMonsOyaSex( const FLD_TRADE_WORK* cwork )
{
	return cwork->pokeData->oya_sex;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ取得
 *	破棄が必要
 */
//-----------------------------------------------------------------------------
static STRBUF* GetTradeMsgData( u32 heapID, u32 str_id )
{
	STRBUF* strbuf;
	
	GFL_MSGDATA* msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
                                     ARCID_MESSAGE, NARC_message_fld_trade_dat, 
                                     heapID );
	strbuf = GFL_MSG_CreateString( msg, str_id );
  GFL_MSG_Delete( msg );
	return strbuf;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ポケモンパラムにデータの設定
 *
 *  @param  gameData
 *	@param	heapID		heapID
 *	@param	pp		    ポケモンパラム
 *	@param	data		  データ
 *	@param	tradeNo	tradeナンバー
 */
//-----------------------------------------------------------------------------
static void SetPokemonParam( GAMEDATA* gameData, HEAPID heapID, 
                             POKEMON_PARAM* pp, FLD_TRADE_POKEDATA* data, 
	                           u32 tradeNo )
{
	STRBUF* strbuf;
	u32 placeid; 
  u32 personal_rnd;

  // 指定の性別になるような個性乱数を生成
  personal_rnd = POKETOOL_CalcPersonalRandEx( data->mons_id, data->monsno, data->formno, data->sex, 0, FALSE );
	
	// モンスターナンバー　レベル　固体乱数　ID設定
	PP_SetupEx( pp, 
              data->monsno, 
              data->level, 
              data->mons_id, 
              PTL_SETUP_POW_AUTO, 
              personal_rnd );
  // フォーム
  PP_Put( pp, ID_PARA_form_no, data->formno );

#ifdef DEBUG_MODE
  PP_Dump( pp );
#endif

	// ニックネーム
	strbuf = GetTradeMsgData( heapID, data->str_id_nickname );
	PP_Put( pp, ID_PARA_nickname, (u32)strbuf );
	GFL_STR_DeleteBuffer( strbuf );

	// 各種乱数設定
	PP_Put( pp, ID_PARA_hp_rnd,     data->hp_rnd );
	PP_Put( pp, ID_PARA_pow_rnd,    data->at_rnd );
	PP_Put( pp, ID_PARA_def_rnd,    data->df_rnd );
	PP_Put( pp, ID_PARA_agi_rnd,    data->sp_rnd );
	PP_Put( pp, ID_PARA_spepow_rnd, data->sa_rnd );
	PP_Put( pp, ID_PARA_spedef_rnd, data->sd_rnd );

	// 特性
  {
    POKEMON_PERSONAL_DATA* ppd;
    u32 sp1, sp2, sp3;
    u32 tokusei;

    ppd = POKE_PERSONAL_OpenHandle( data->monsno, data->formno, heapID );
    sp1 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi1 );
    sp2 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi2 );
    sp3 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi3 );
    POKE_PERSONAL_CloseHandle( ppd );

    switch( data->tokusei_idx ) {
    case 0: tokusei = sp1; break;
    case 1: tokusei = sp2; break;
    case 2: tokusei = sp3; break;
    default: 
      GF_ASSERT(0);
      tokusei = sp1;
      break;
    } 
    PP_Put( pp, ID_PARA_speabino, tokusei );
  }

  // 性格
  PP_Put( pp, ID_PARA_seikaku, data->seikaku );

	// かっこよさなど
	PP_Put( pp, ID_PARA_style,     data->style );
	PP_Put( pp, ID_PARA_beautiful, data->beautiful );
	PP_Put( pp, ID_PARA_cute,      data->cute );
	PP_Put( pp, ID_PARA_clever,    data->clever );
	PP_Put( pp, ID_PARA_strong,    data->strong );
	
	// アイテム
	PP_Put( pp, ID_PARA_item, data->item );

	// 親の名前
	strbuf = GetTradeMsgData( heapID, data->str_id_oya_name );
	PP_Put( pp, ID_PARA_oyaname, (u32)strbuf );
	GFL_STR_DeleteBuffer( strbuf );

	// 親の性別	
	PP_Put( pp, ID_PARA_oyasex, data->oya_sex );

	// 国コード
	PP_Put( pp, ID_PARA_country_code, data->world_code );

  // トレーナーメモ
  {
    MYSTATUS* mystatus;
    mystatus = GAMEDATA_GetMyStatus( gameData );
    POKE_MEMO_SetTrainerMemoPP( 
        pp, POKE_MEMO_GAME_TRADE, mystatus, POKE_MEMO_PLACE_GAME_TRADE, heapID );
  }

#ifdef DEBUG_MODE
  PP_Dump( pp );
#endif

	// 計算しなおし
	PP_Renew( pp );

	// レアにならないようにデータ設定されていなかった場合NG
  GF_ASSERT( PP_CheckRare( pp ) == FALSE && "レアポケです。");
} 


#ifdef DEBUG_MODE
static void PP_Dump( const POKEMON_PARAM* pp )
{
  u32 val; 
  OBATA_Printf( "---------------------------------------------- PP\n" );
  val = PP_Get( pp, ID_PARA_personal_rnd, NULL ); OBATA_Printf( "personal_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_monsno, NULL ); OBATA_Printf( "monsno = %d\n", val );
  val = PP_Get( pp, ID_PARA_form_no, NULL ); OBATA_Printf( "form_no = %d\n", val );
  val = PP_Get( pp, ID_PARA_item, NULL ); OBATA_Printf( "item = %d\n", val );
  val = PP_Get( pp, ID_PARA_id_no, NULL ); OBATA_Printf( "id_no = %d\n", val );
  val = PP_Get( pp, ID_PARA_exp, NULL ); OBATA_Printf( "exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_friend, NULL ); OBATA_Printf( "friend = %d\n", val );
  val = PP_Get( pp, ID_PARA_speabino, NULL ); OBATA_Printf( "tokusei_idx = %d\n", val );
  val = PP_Get( pp, ID_PARA_sex, NULL ); OBATA_Printf( "sex = %d\n", val );
  val = PP_Get( pp, ID_PARA_seikaku, NULL ); OBATA_Printf( "seikaku = %d\n", val );
  val = PP_Get( pp, ID_PARA_mark, NULL ); OBATA_Printf( "mark = %d\n", val );
  val = PP_Get( pp, ID_PARA_country_code, NULL ); OBATA_Printf( "country_code = %d\n", val );
  val = PP_Get( pp, ID_PARA_hp_exp, NULL ); OBATA_Printf( "hp_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_pow_exp, NULL ); OBATA_Printf( "pow_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_def_exp, NULL ); OBATA_Printf( "def_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_agi_exp, NULL ); OBATA_Printf( "agi_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_spepow_exp, NULL ); OBATA_Printf( "spepow_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_spedef_exp, NULL ); OBATA_Printf( "spedef_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_style, NULL ); OBATA_Printf( "style = %d\n", val );
  val = PP_Get( pp, ID_PARA_beautiful, NULL ); OBATA_Printf( "beautiful = %d\n", val );
  val = PP_Get( pp, ID_PARA_cute, NULL ); OBATA_Printf( "cute = %d\n", val );
  val = PP_Get( pp, ID_PARA_clever, NULL ); OBATA_Printf( "clever = %d\n", val );
  val = PP_Get( pp, ID_PARA_strong, NULL ); OBATA_Printf( "strong = %d\n", val );
  val = PP_Get( pp, ID_PARA_fur, NULL ); OBATA_Printf( "fur = %d\n", val );
  val = PP_Get( pp, ID_PARA_hp_rnd, NULL ); OBATA_Printf( "hp_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_pow_rnd, NULL ); OBATA_Printf( "pow_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_def_rnd, NULL ); OBATA_Printf( "def_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_agi_rnd, NULL ); OBATA_Printf( "agi_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_spepow_rnd, NULL ); OBATA_Printf( "spepow_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_spedef_rnd, NULL ); OBATA_Printf( "spedef_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_oyasex, NULL ); OBATA_Printf( "oyasex = %d\n", val );
  val = PP_Get( pp, ID_PARA_level, NULL ); OBATA_Printf( "level = %d\n", val );
  val = PP_Get( pp, ID_PARA_get_place, NULL ); OBATA_Printf( "get_place = %d\n", val );
  OBATA_Printf( "-------------------------------------------------\n" );
}
#endif

#ifdef DEBUG_MODE
static void FTP_Dump( const FLD_TRADE_POKEDATA* ftp )
{
  OBATA_Printf( "--------------------------------------------- FTP\n" );
  OBATA_Printf( "monsno = %d\n", ftp->monsno );
  OBATA_Printf( "formno = %d\n", ftp->formno );
  OBATA_Printf( "level = %d\n", ftp->level );
  OBATA_Printf( "hp_rnd = %d\n", ftp->hp_rnd );
  OBATA_Printf( "at_rnd = %d\n", ftp->at_rnd );
  OBATA_Printf( "df_rnd = %d\n", ftp->df_rnd );
  OBATA_Printf( "sp_rnd = %d\n", ftp->sp_rnd );
  OBATA_Printf( "sa_rnd = %d\n", ftp->sa_rnd );
  OBATA_Printf( "sd_rnd = %d\n", ftp->sd_rnd );
  OBATA_Printf( "tokusei_idx = %d\n", ftp->tokusei_idx );
  OBATA_Printf( "seikaku = %d\n", ftp->seikaku );
  OBATA_Printf( "sex = %d\n", ftp->sex );
  OBATA_Printf( "mons_id = %d\n", ftp->mons_id );
  OBATA_Printf( "style = %d\n", ftp->style );
  OBATA_Printf( "beautiful = %d\n", ftp->beautiful );
  OBATA_Printf( "cute = %d\n", ftp->cute );
  OBATA_Printf( "clever = %d\n", ftp->clever );
  OBATA_Printf( "strong = %d\n", ftp->strong );
  OBATA_Printf( "item = %d\n", ftp->item );
  OBATA_Printf( "oya_sex = %d\n", ftp->oya_sex );
  OBATA_Printf( "fur = %d\n", ftp->fur );
  OBATA_Printf( "world_code = %d\n", ftp->world_code );
  OBATA_Printf( "change_monsno = %d\n", ftp->change_monsno );
  OBATA_Printf( "change_monssex = %d\n", ftp->change_monssex );
  OBATA_Printf( "-------------------------------------------------\n" );
}
#endif


//========================================================================================
// ■イベント
//========================================================================================
typedef struct 
{
  GAMESYS_WORK*           gameSystem;
  GAMEDATA*               gameData;
  POKEPARTY*              pokeParty;  // 手持ちポケパーティ
  u8                      tradeNo;    // 交換データNo.
  u8                      partyPos;   // 交換に出すポケモンの手持ちインデックス
  FLD_TRADE_WORK*         tradeWork;  // 交換ワーク
  POKEMONTRADE_DEMO_PARAM tradeDemoParam;   // 交換デモのパラメータ
  SHINKA_DEMO_PARAM*      shinkaDemoParam;  // 進化デモのパラメータ

} FLD_TRADE_EVWORK;

// シーケンス番号
enum {
  SEQ_INIT,         // イベント初期化
  SEQ_TRADE,        // 交換デモ
  SEQ_DATA_UPDATE,  // データ更新
  SEQ_EVOLUTION,    // 進化デモ
  SEQ_EXIT,         // イベント終了
};

//----------------------------------------------------------------------------------------
/**
 * @brief ゲーム内交換イベント処理関数
 */
//----------------------------------------------------------------------------------------
static GMEVENT_RESULT FieldPokeTradeEvent( GMEVENT* event, int* seq, void* wk )
{
  FLD_TRADE_EVWORK* work       = (FLD_TRADE_EVWORK*)wk;
  GAMESYS_WORK*     gameSystem = work->gameSystem;
  GAMEDATA*         gameData   = work->gameData;
  POKEPARTY*        pokeParty  = work->pokeParty;
  FIELDMAP_WORK*    fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );
  POKEMON_PARAM*    pokeParam  = PokeParty_GetMemberPointer( pokeParty, work->partyPos );

  switch( *seq ) {
  // イベント初期化
  case SEQ_INIT:
    // 交換ワーク生成
    work->tradeWork = FLD_TRADE_WORK_Create( HEAPID_PROC, work->tradeNo );
    SetPokemonParam( gameData, HEAPID_PROC, 
        work->tradeWork->pokeParam, work->tradeWork->pokeData, work->tradeNo );
#ifdef DEBUG_MODE
    PP_Dump( work->tradeWork->pokeParam );
    FTP_Dump( work->tradeWork->pokeData );
#endif
    *seq = SEQ_TRADE;
    break;

  // 交換デモ呼び出し
  case SEQ_TRADE:
    {
      GMEVENT* demo;
      FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gameSystem );
      work->tradeDemoParam.gamedata = gameData; 
      work->tradeDemoParam.pMy      = GAMEDATA_GetMyStatus( gameData );
      work->tradeDemoParam.pMyPoke  = pokeParam;
      work->tradeDemoParam.pNPC     = work->tradeWork->pokeOya;
      work->tradeDemoParam.pNPCPoke = work->tradeWork->pokeParam;
      demo = EVENT_FieldSubProc( gameSystem, fieldmap, 
          FS_OVERLAY_ID(pokemon_trade), &PokemonTradeDemoProcData, &work->tradeDemoParam );
      GMEVENT_CallEvent( event, demo );
    }
    *seq = SEQ_DATA_UPDATE;
    break;

  // データ更新
  case SEQ_DATA_UPDATE:
    // 手持ちポケ上書き
    PokeParty_SetMemberData( pokeParty, work->partyPos, work->tradeWork->pokeParam );
    // 図鑑登録
    {
      ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gameData );
      ZUKANSAVE_SetPokeSee( zukan, work->tradeWork->pokeParam );  // 見た
      ZUKANSAVE_SetPokeGet( zukan, work->tradeWork->pokeParam );  // 捕まえた
    }
    *seq = SEQ_EXIT;
    break;

  // 進化デモ呼び出し
  case SEQ_EVOLUTION:
    {
      SHINKA_COND cond;
      u16 afterMonsNo = SHINKA_Check( pokeParty, pokeParam, SHINKA_TYPE_TUUSHIN, 0, GAMEDATA_GetSeasonID( gameData ),
                                      &cond, HEAPID_PROC );

      if( afterMonsNo ) {
        GMEVENT* demo;

        // パラメータを生成
        //GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
        //work->shinkaDemoParam = SHINKADEMO_AllocParam( 
        //    HEAPID_PROC, gameData, pokeParty, afterMonsNo, work->partyPos, cond, TRUE );
        //GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
        {
          SHINKA_DEMO_PARAM* sdp = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof( SHINKA_DEMO_PARAM ) );
          sdp->gamedata          = gameData;
          sdp->ppt               = pokeParty;
          sdp->after_mons_no     = afterMonsNo;
          sdp->shinka_pos        = work->partyPos;
          sdp->shinka_cond       = cond;
          sdp->b_field           = TRUE;
          sdp->b_enable_cancel   = FALSE;
          work->shinkaDemoParam  = sdp;
        }
        // デモ呼び出し
        demo = EVENT_FieldSubProc( gameSystem, fieldmap, 
            FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, work->shinkaDemoParam );
        GMEVENT_CallEvent( event, demo );
      }
    }
    *seq = SEQ_EXIT;
    break;
  
  // イベント終了処理
  case SEQ_EXIT:
    // 進化デモの後始末
    if( work->shinkaDemoParam ) {
        //GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
      //SHINKADEMO_FreeParam( work->shinkaDemoParam );
      //GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
      {
        SHINKA_DEMO_PARAM* sdp = work->shinkaDemoParam;
        GFL_HEAP_FreeMemory( sdp );
      }
    }
    // 交換ワーク破棄
    FLD_TRADE_WORK_Delete( work->tradeWork );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------------------
/**
 * @brief ゲーム内交換イベントを生成する
 *
 * @param gsys ゲームシステム
 * @param tradeNo  交換データNo.
 * @param party_pos 交換に出すポケモンの手持ちインデックス
 *
 * @return 生成したイベント
 */
//----------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldPokeTrade( GAMESYS_WORK* gsys, u8 tradeNo, u8 party_pos )
{
  GMEVENT* event;
  FLD_TRADE_EVWORK* work;
  GAMEDATA* gameData;
  POKEPARTY* pokeParty;

  gameData = GAMESYSTEM_GetGameData( gsys );
  pokeParty = GAMEDATA_GetMyPokemon( gameData );

  // 生成
  event = GMEVENT_Create( gsys, NULL, FieldPokeTradeEvent, sizeof(FLD_TRADE_EVWORK) );

  // 初期化
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gsys;
  work->gameData = gameData;
  work->pokeParty = pokeParty;
  work->tradeNo = tradeNo;
  work->partyPos = party_pos; 
  work->shinkaDemoParam = NULL;
  return event;
}
