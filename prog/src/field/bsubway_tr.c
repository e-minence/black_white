//======================================================================
/**
 * @file bsubway_tr.c
 * @brief バトルサブウェイ　トレーナー関連
 * @author  Miyuki Iwasawa
 * @date  06.04.17
 * @note GSより移植 kaga
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "savedata/save_tbl.h"
#include "savedata/record.h"
#include "savedata/battle_rec.h"

#include "gamesystem/btl_setup.h"
#include "poke_tool/poke_tool.h"
#include "tr_tool/tr_tool.h"
#include "battle/battle.h"
#include "pm_define.h"
#include "buflen.h"
#include "sound/pm_sndsys.h"

#include "fldmmdl.h"
#include "field_encount.h"
#include "message.naix"

#include "bsubway_scr_def.h"
#include "bsubway_scr.h"
#include "bsubway_scrwork.h"
#include "savedata/bsubway_savedata.h"

//#include "../mystery/fntequ.h"

#define __BSUBWAY_FLD_H_GLOBAL
#include "bsubway_tr.h"
#include "event_bsubway.h"

//正式データが出来るまでインクルード
#include "item/itemsym.h"
#include "poke_tool/monsno_def.h"
#include "waza_tool/wazano_def.h"
//#include "battle/battle_common.h"
#include "tr_tool/trtype_def.h"
#include "battle/bsubway_battle_data.h"
#include "tr_tool/trno_def.h"
//#include "b_tower_fld.dat"

#include "fld_btl_inst_tool.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
#if 0 //100425 del saito
static BOOL set_BSWayPokemonParam(
    BSUBWAY_SCRWORK* wk,BSUBWAY_TRAINER_ROM_DATA *trd,
    u16 tr_no,BSUBWAY_POKEMON *pwd,u8 cnt,
    u16 *set_poke_no,u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM *poke, HEAPID heapID );

static BSUBWAY_TRAINER_ROM_DATA * get_TrainerRomData(
    u16 tr_no, HEAPID heapID );

static void get_PokemonRomData(
    BSUBWAY_POKEMON_ROM_DATA *prd,int index);

static void  make_TrainerData(
    BATTLE_SETUP_PARAM *bp, BSUBWAY_PARTNER_DATA *tr_data,
    int cnt, BTL_CLIENT_ID client_no, HEAPID heapID );

static STRCODE * PM_strcpy( STRCODE* to_str, const STRCODE* from_str );
#endif  //100425 del saito
static u16 get_Rand( BSUBWAY_SCRWORK *wk );

#if 0 //100424 fld_btl_inst_tool.c に移動　saito
//======================================================================
//  トレーナータイプ
//======================================================================
//--------------------------------------------------------------
/// タワーに出現するトレーナータイプ←→OBJコード
//--------------------------------------------------------------
static const u16 btower_trtype2objcode[][2] =
{
 {TRTYPE_TANPAN,  BOY2},  ///<たんパンこぞう
 {TRTYPE_MINI,  GIRL1},  ///<ミニスカート
 {TRTYPE_SCHOOLB,  BOY1},  ///<じゅくがえり
 {TRTYPE_SCHOOLG, GIRL3 },  ///<じゅくがえり
 {TRTYPE_PRINCE,  BOY4},  ///<おぼっちゃま
 {TRTYPE_PRINCESS,  GIRL4},  ///<おじょうさま
 {TRTYPE_KINDERGARTENM, BABYBOY2},    //えんじ♂
 {TRTYPE_KINDERGARTENW, BABYGIRL2},    //えんじ♀
 {TRTYPE_BACKPACKERM, BACKPACKERM},//バックパッカー♂
 {TRTYPE_BACKPACKERW, BACKPACKERW},//バックパッカー♂
 {TRTYPE_WAITER,WAITER}, //ウエーター
 {TRTYPE_WAITRESS,WAITRESS}, //ウエートレス
 {TRTYPE_DAISUKIM,  MIDDLEMAN1},  ///<だいすきクラブ
 {TRTYPE_DAISUKIW,  MIDDLEWOMAN1},  ///<だいすきクラブ
 {TRTYPE_DOCTOR, DOCTOR },   //ドクター
 {TRTYPE_NURSE,  NURSE},    //ナース
 {TRTYPE_CYCLINGM,  CYCLEM},  ///<サイクリング♂
 {TRTYPE_CYCLINGW,  CYCLEW},  ///<サイクリング♀
 {TRTYPE_GENTLE,  GENTLEMAN},  ///<ジェントルマン
 {TRTYPE_MADAM,  LADY},  ///<マダム
 {TRTYPE_BREEDERM,  MAN1},  ///<ポケモンブリーダー
 {TRTYPE_BREEDERW,  WOMAN1},  ///<ポケモンブリーダー
 {TRTYPE_SCIENTISTM,  ASSISTANTM},  //けんきゅういん♂
 {TRTYPE_SCIENTISTW,  ASSISTANTW},  //けんきゅういん♀
 {TRTYPE_ESPM,  ESPM},  ///<サイキッカー
 {TRTYPE_ESPW,  ESPW},  ///<サイキッカー
 {TRTYPE_KARATE,  FIGHTERM},  //からておう
 {TRTYPE_BATTLEG,  GIRL2},  ///<バトルガール
 {TRTYPE_RANGERM,  MAN3},  ///<ポケモンレンジャー
 {TRTYPE_RANGERW,  WOMAN3},  ///<ポケモンレンジャー
 {TRTYPE_ELITEM,  MAN3},  ///<エリートトレーナー
 {TRTYPE_ELITEW,  WOMAN3},  ///<エリートトレーナー
 {TRTYPE_VETERANM,  OLDMAN1},  ///<ベテラントレーナー
 {TRTYPE_VETERANW,  OLDWOMAN1},  ///<ベテラントレーナー
 {TRTYPE_FISHING,  FISHING},  ///<つりびと
 {TRTYPE_MOUNT,  MAN1},  ///<やまおとこ
 {TRTYPE_WORKER1,  WORKMAN},  ///<さぎょういん
 {TRTYPE_WORKER2,  WORKMAN},  ///<さぎょういん
 {TRTYPE_JUGGLING, CLOWN },   //クラウン
 {TRTYPE_ARTIST,  OLDMAN1},  ///<げいじゅつか
 {TRTYPE_POLICE,  POLICEMAN},  ///<おまわりさん
 {TRTYPE_HEADS,  BADMAN},  ///<スキンヘッズ
 {TRTYPE_BADRIDER, BADRIDER},   //ぼうそうぞく
 {TRTYPE_CLEANING, CLEANINGM},   //せいそういん
 {TRTYPE_RAIL,RAILMAN }, //てつどういん
 {TRTYPE_PILOT, PILOT},   //パイロット
 {TRTYPE_BUSINESS1, BUSINESSMAN},    //ビジネスマン1
 {TRTYPE_BUSINESS2, BUSINESSMAN},    //ビジネスマン2
 {TRTYPE_PARASOL,  AMBRELLA},  ///<パラソルおねえさん
 {TRTYPE_BAKER,     BAKER },   //ベーカリー
 {TRTYPE_CHILDCARE, WOMAN3}, //ほいくし
 {TRTYPE_MAID, MAID},    //メイド
 {TRTYPE_OL, OL},    //ＯＬ
};

#define TRTYPE2OBJCODE_MAX  (NELEMS(btower_trtype2objcode))

//--------------------------------------------------------------
/**
 * @brief  トレーナータイプから人物OBJコードを返す
 * @param tr_type トレーナータイプ
 * @retval u16 OBJコード
 */
//--------------------------------------------------------------
u16 BSUBWAY_GetTrainerOBJCode( u8 tr_type )
{
  int i;

  for( i = 0; i < TRTYPE2OBJCODE_MAX; i++ ){
    if( btower_trtype2objcode[i][0] == tr_type ){
      return btower_trtype2objcode[i][1];
    }
  }
  return BOY1;
}
#endif
#if 0   //100424 未使用につき　削除　saito
//--------------------------------------------------------------
/**
 * @brief  トレーナータイプが対応しているか
 * @param tr_type トレーナータイプ
 * @retval BOOL TRUE=対応
 */
//--------------------------------------------------------------
static BOOL check_TrainerType(u8 tr_type)
{
  int i;

  for( i = 0;i < TRTYPE2OBJCODE_MAX; i++ ){
    if( btower_trtype2objcode[i][0] == tr_type ){
      return( TRUE );
    }
  }
  return( FALSE );
}
#endif

//======================================================================
//  バトルサブウェイ関連
//======================================================================
#if 0   //現時点で未使用　100426
typedef struct
{
  u16  size;      ///< 配列サイズ
  u16  strlen;    ///< 文字列長（EOMを含まない）
  u32  magicNumber; ///< 正常に初期化済みであることをチェックするためのナンバー
  STRCODE  buffer[1]; ///< 配列本体
}DEBUG_STRBUF;
#endif

#if 0   //100426  del  saito
//--------------------------------------------------------------
/**
 *  @brief バトルサブウェイ用ポケモンデータからPOKEMON_PARAMを生成
 *  @param src BSUBWAY_POKEMON
 *  @param dest POKEMON_PARAM
 *  @retval nothing
 */
//--------------------------------------------------------------
static void make_PokePara(
    const BSUBWAY_POKEMON *src, POKEMON_PARAM *dest )
{
  int i;
  u8 buf8,waza_pp;
  u16 buf16;
  u32 buf32;

  PP_Clear(dest);

  //monsno,level,pow_rnd,idno
  PP_SetupEx( dest, src->mons_no, 50, PTL_SETUP_ID_AUTO,
      (src->power_rnd & 0x3FFFFFFF), src->personal_rnd);

  //form_no
  buf8 = src->form_no;
  PP_Put( dest, ID_PARA_form_no, buf8 );

  //装備アイテム設定
  PP_Put( dest,ID_PARA_item, src->item_no );

  //技設定
  for(i = 0;i < 4;i++){
    buf16 = src->waza[i];
    PP_Put(dest,ID_PARA_waza1+i,buf16);
    buf8 = (src->pp_count >> (i*2))&0x03;
    PP_Put(dest,ID_PARA_pp_count1+i, buf8);

    //pp再設定
    waza_pp = (u8)PP_Get(dest,ID_PARA_pp_max1+i,NULL);
    PP_Put(dest,ID_PARA_pp1+i,waza_pp);
  }

  //ID設定
  buf32 = src->id_no;
  PP_Put(dest,ID_PARA_id_no,buf32);

  //経験値設定
  buf8 = src->hp_exp;
  PP_Put(dest,ID_PARA_hp_exp,buf8);
  buf8 = src->pow_exp;
  PP_Put(dest,ID_PARA_pow_exp,buf8);
  buf8 = src->def_exp;
  PP_Put(dest,ID_PARA_def_exp,buf8);
  buf8 = src->agi_exp;
  PP_Put(dest,ID_PARA_agi_exp,buf8);
  buf8 = src->spepow_exp;
  PP_Put(dest,ID_PARA_spepow_exp,buf8);
  buf8 = src->spedef_exp;
  PP_Put(dest,ID_PARA_spedef_exp,buf8);

  //特性設定
  PP_Put(dest,ID_PARA_speabino, src->tokusei);

  //なつきど設定
  PP_Put(dest,ID_PARA_friend,src->natuki);

  { //名前設定
    STRBUF *nick_name;
    nick_name = GFL_STR_CreateBuffer(
        MONS_NAME_SIZE+EOM_SIZE, HEAPID_PROC );
    
#if 0
      GFL_STR_SetStringCode( nick_name, src->nickname );
#else
    {
      int i;
      u16 eomCode = GFL_STR_GetEOMCode();
      STRCODE code[MONS_NAME_SIZE+EOM_SIZE];
      const STRCODE *sz = src->nickname;
      
      for( i = 0; i < (MONS_NAME_SIZE+EOM_SIZE); i++ ){
        code[i] = sz[i];
      }
      
      code[i-1] = eomCode;
      GFL_STR_SetStringCode( nick_name, code );
    }
    
    /*
    {
      u16 eomCode = GFL_STR_GetEOMCode();
      DEBUG_STRBUF *d_strbuf = (DEBUG_STRBUF*)nick_name;
      const STRCODE *sz = src->nickname;
      
      OS_TPrintf( " SetStringCode ... " );
      d_strbuf->strlen = 0;
      
      while( *sz != eomCode )
      {
        if( d_strbuf->strlen >= (d_strbuf->size-1) )
        {
          GF_ASSERT_MSG( 0, "STRBUF overflow: busize=%d, EOMCode=%04x",
              d_strbuf->size, eomCode );
          break;
        }
        
        OS_TPrintf("%04x,", *sz);
        d_strbuf->buffer[ d_strbuf->strlen++ ] = *sz++;
      }
      
      OS_TPrintf( "\n" );
      
      d_strbuf->buffer[d_strbuf->strlen] = eomCode;
    }
    */
#endif

    PP_Put( dest, ID_PARA_nickname, (u32)nick_name );
    GFL_STR_DeleteBuffer( nick_name );
  }

  PP_Put(dest,ID_PARA_country_code,src->country_code); //カントリーコード

  //パラメータ再計算
  {
    u32 lv = PP_CalcLevel( dest );
    PP_Put( dest, ID_PARA_level,lv );
  }
}

//--------------------------------------------------------------
/**
 * @brief  バトルサブウェイ　プレイモードからFIGHT_TYPEを返す
 * @param play_mode プレイモード
 * @retval u32 FIGHT_TYPE
 */
//--------------------------------------------------------------
#if 0
static u32 get_FightType(u8 play_mode)
{
  switch(play_mode){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_WIFI:
//    return FIGHT_TYPE_BATTLE_TOWER_1vs1;
    return BTL_RULE_SINGLE;
  case BSWAY_MODE_DOUBLE:
//  return FIGHT_TYPE_BATTLE_TOWER_2vs2;
    return BTL_RULE_DOUBLE;
#if 0 //wb null
  case BSWAY_MODE_MULTI:
    return FIGHT_TYPE_BATTLE_TOWER_AI_MULTI;
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_WIFI_MULTI:
    return FIGHT_TYPE_BATTLE_TOWER_SIO_MULTI;
#endif
  }
  return BTL_RULE_SINGLE;
}
#endif

//--------------------------------------------------------------
/**
 * @brief  バトルサブウェイ　トレーナーデータ生成
 * @param heapID HEAPID
 * @retval BSP_TRAINER_DATA*
 */
//--------------------------------------------------------------
static BSP_TRAINER_DATA * create_BSP_TRAINER_DATA( HEAPID heapID )
{
  BSP_TRAINER_DATA* tr_data = GFL_HEAP_AllocClearMemory( heapID, sizeof( BSP_TRAINER_DATA ) );
  tr_data->name =   GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, heapID );
  return tr_data;
}

//--------------------------------------------------------------
/**
 * BATTLE_SETUP_PARAM作成
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param gsys GAMESYS_WORK
 * @retval BATTLE_SETUP_PARAM
 */
//--------------------------------------------------------------
BATTLE_SETUP_PARAM * BSUBWAY_SCRWORK_CreateBattleParam(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
  int i;
  BATTLE_SETUP_PARAM *dst;
  BTL_FIELD_SITUATION sit;
  BTL_CLIENT_ID client;
  POKEPARTY *party;
  POKEMON_PARAM *pp;
  POKEMON_PARAM *entry_pp;
  BSP_TRAINER_DATA *tr_data;
  BSUBWAY_PARTNER_DATA *bsw_partner;
  BSUBWAY_TRAINER *bsw_trainer;
  
  HEAPID heapID = HEAPID_PROC;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
  u16 play_mode = wk->play_mode;
  
  BTL_FIELD_SITUATION_SetFromFieldStatus(
      &sit, gdata, GAMESYSTEM_GetFieldMapWork(gsys) );
  
  dst = BATTLE_PARAM_Create( heapID );
  
  //初期化
  switch( play_mode ){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_S_SINGLE:
  case BSWAY_MODE_WIFI:
    BTL_SETUP_Single_Trainer( dst, gdata, &sit, TRID_NULL, heapID );
    break;
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_S_DOUBLE:
    BTL_SETUP_Double_Trainer( dst, gdata, &sit, TRID_NULL, heapID );
    break;
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
    BTL_SETUP_AIMulti_Trainer( dst, gdata, &sit,
        TRID_NULL, TRID_NULL, TRID_NULL, heapID );
    break;
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    {
      GFL_NETHANDLE *netHandle = GFL_NET_HANDLE_GetCurrentHandle();
      int commPos = 0;
      
      if( GFL_NET_SystemGetCurrentID() != GFL_NET_NO_PARENTMACHINE ){
        commPos = 2; //通信子である
      }
      
      BTL_SETUP_AIMulti_Comm( dst, gdata,
          netHandle, BTL_COMM_DS, commPos,
          TRID_NULL, TRID_NULL, heapID );
    }
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  BTL_SETUP_AllocRecBuffer( dst, heapID );
  
  { //プレイヤー設定
    const POKEPARTY *myparty = BSUBWAY_SCRWORK_GetPokePartyUse( wk );
    client = BTL_CLIENT_PLAYER;
    
    //MyStatus
    dst->playerStatus[client] = mystatus;
    
    //トレーナーデータ
    tr_data = dst->tr_data[client];
    
    MyStatus_CopyNameString( mystatus, tr_data->name );
    tr_data->tr_type = TRTYPE_HERO + MyStatus_GetMySex( mystatus );
    
    //ポケモンパーティ
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      pp = PokeParty_GetMemberPointer( myparty, wk->member[i] );
      POKETOOL_CopyPPtoPP( pp, entry_pp );
      
      if( PP_Get(pp,ID_PARA_level,NULL) != 50 ){
        u32 exp = POKETOOL_GetMinExp(
              PP_Get(pp,ID_PARA_monsno,NULL),
              PP_Get(pp,ID_PARA_form_no,NULL),
              50 );
        
        PP_Put( entry_pp, ID_PARA_exp, exp );
        PP_Renew( entry_pp );
      }
      
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }

  { //敵トレーナー設定
    client = BTL_CLIENT_ENEMY1;
    
    //敵トレーナー0 トレーナーデータ
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->tr_data[0];
    bsw_trainer = &bsw_partner->bt_trd;
    
    if( play_mode == BSWAY_MODE_WIFI ){
      tr_data->tr_id = 0;
    }else{
      tr_data->tr_id = bsw_trainer->player_id;
    }
    
    tr_data->tr_type = bsw_trainer->tr_type;
    tr_data->ai_bit = 0x00000087;  //最強
    
    //トレーナーデータ　name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //トレーナーデータ　word
    if( play_mode == BSWAY_MODE_WIFI ){
      tr_data->win_word = *(PMS_DATA*)bsw_trainer->win_word;
      tr_data->lose_word = *(PMS_DATA*)bsw_trainer->lose_word;
    }
    
    //ポケモンパーティ
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    entry_pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
  
  if( dst->multiMode != BTL_MULTIMODE_NONE ) //マルチ
  { //敵トレーナー２設定
    client = BTL_CLIENT_ENEMY2;
    
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->tr_data[1];
    bsw_trainer = &bsw_partner->bt_trd;
    
    tr_data->tr_id = bsw_trainer->player_id;
    tr_data->tr_type = bsw_trainer->tr_type;
    tr_data->ai_bit = 0x00000087;  //最強
    
    //トレーナーデータ　name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //トレーナーデータ　word
    //特に設定無し
    
    //ポケモンパーティ
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
  
  if( dst->multiMode == BTL_MULTIMODE_PA_AA ) //AIマルチ
  { //AIパートナー設定
    client = BTL_CLIENT_PARTNER;
    KAGAYA_Printf( "パートナーNo.%d\n", wk->partner );
    
    //トレーナーデータ
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->five_data[wk->partner];
    bsw_trainer = &bsw_partner->bt_trd;
    
    tr_data->tr_id = bsw_trainer->player_id;
    tr_data->tr_type = bsw_trainer->tr_type;
    tr_data->ai_bit = 0x00000087;  //最強
    
    //トレーナーデータ　name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //トレーナーデータ　word
    //特に無し
    
    //ポケモンパーティ
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
#if 0
  else if( dst->multiMode == BTL_MULTIMODE_PP_AA ) //通信マルチ
  { //通信マルチパートナー設定
  }
#endif
  
  BTL_SETUP_SetSubwayMode( dst ); //一通りセットした後に呼ぶ事
  return dst;
}
#endif    //100426 del saito

//--------------------------------------------------------------
/**
 * BATTLE_SETUP_PARAM作成
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param gsys GAMESYS_WORK
 * @retval BATTLE_SETUP_PARAM
 */
//--------------------------------------------------------------
BATTLE_SETUP_PARAM * BSUBWAY_SCRWORK_CreateBattleParam(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
  HEAPID heapID = HEAPID_PROC;
  BATTLE_SETUP_PARAM *dst;
  POKEPARTY *party;

  party = PokeParty_AllocPartyWork( GFL_HEAP_LOWID(heapID) );
  {
    int i;
	  POKEMON_PARAM * pp;
    const POKEPARTY *myparty = BSUBWAY_SCRWORK_GetPokePartyUse( wk );
	  PokeParty_Init( party, wk->member_num );
    for( i=0; i<wk->member_num; i++ ){
      pp = PokeParty_GetMemberPointer( myparty, wk->member[i] );
      PokeParty_Add( party, pp );
    }
	}

  dst = FBI_TOOL_CreateBattleParam(
          gsys, party, wk->play_mode,
          wk->tr_data,
          &wk->five_data[wk->partner],
          wk->member_num );

  GFL_HEAP_FreeMemory(party);

  return dst;
}

#if 0 //old
BATTLE_SETUP_PARAM * BSUBWAY_SCRWORK_CreateBattleParam(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
  int i;
  BATTLE_SETUP_PARAM *dst;
  BTL_FIELD_SITUATION sit;
  BTL_CLIENT_ID client;
  POKEPARTY *party;
  POKEMON_PARAM *pp;
  POKEMON_PARAM *entry_pp;
  BSP_TRAINER_DATA *tr_data;
  BSUBWAY_PARTNER_DATA *bsw_partner;
  BSUBWAY_TRAINER *bsw_trainer;
  
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
  u16 play_mode = wk->play_mode;
  
  dst = BATTLE_PARAM_Create( HEAPID_PROC );
  
  BTL_FIELD_SITUATION_SetFromFieldStatus( &dst->fieldSituation, gdata, GAMESYSTEM_GetFieldMapWork(gsys) );
  
  dst->netHandle = NULL;
  dst->commMode = BTL_COMM_NONE;
  dst->commPos = 0;
  dst->multiMode = BTL_MULTIMODE_NONE;
  dst->recBuffer = NULL;
  dst->fRecordPlay = FALSE;
  dst->competitor = BTL_COMPETITOR_SUBWAY;
  
  dst->commNetIDBit = 0xffff;
  
  dst->party[BTL_CLIENT_PLAYER] = NULL;
  dst->party[BTL_CLIENT_ENEMY1] = NULL;
  dst->party[BTL_CLIENT_PARTNER] = NULL;
  dst->party[BTL_CLIENT_ENEMY2] = NULL;
  dst->playerStatus[BTL_CLIENT_PLAYER] = NULL;
  dst->playerStatus[BTL_CLIENT_ENEMY1] = NULL;
  dst->playerStatus[BTL_CLIENT_PARTNER] = NULL;
  dst->playerStatus[BTL_CLIENT_ENEMY2] = NULL;
  
  dst->gameData     = gdata;
  dst->itemData     = GAMEDATA_GetMyItem( gdata );
  dst->bagCursor    = GAMEDATA_GetBagCursor( gdata );
  dst->zukanData    = GAMEDATA_GetZukanSave( gdata );
  dst->recordData   = GAMEDATA_GetRecordPtr( gdata );
  
  {
    SAVE_CONTROL_WORK *saveCtrl = GAMEDATA_GetSaveControlWork( gdata );
    dst->configData = SaveData_GetConfig( saveCtrl );
  }
  
//dst->commSupport  = GAMEDATA_GetCommPlayerSupportPtr( gdata );
  dst->commSupport  = NULL;
  
  dst->musicDefault = SEQ_BGM_VS_SUBWAY_TRAINER;
  dst->musicPinch = SEQ_BGM_BATTLEPINCH;
  dst->result = BTL_RESULT_WIN;
  
  BTL_SETUP_AllocRecBuffer( dst, HEAPID_PROC );
  
  dst->rule = BTL_RULE_SINGLE;
  
  switch( play_mode ){
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_S_DOUBLE:
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    dst->rule = BTL_RULE_DOUBLE;
    break;
  }
  
  switch( play_mode ){
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
    dst->multiMode = BTL_MULTIMODE_PA_AA;
    break;
  case BSWAY_PLAYMODE_COMM_MULTI:
  case BSWAY_PLAYMODE_S_COMM_MULTI:
    dst->multiMode = BTL_MULTIMODE_PP_AA;
    dst->netHandle = GFL_NET_HANDLE_GetCurrentHandle();
    dst->commMode = BTL_COMM_DS;
    dst->competitor = BTL_COMPETITOR_COMM;
//  dst->playerStatus[BTL_CLIENT_PARTNER] = &wk->mystatus_fr;
     
    if( GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE ){
      dst->commPos = 0;
    }else{
      dst->commPos = 2;
    }
    break;
  }
  
  //録画データ生成のため、
  //対戦相手のMYSTATUS, POKEPARTYを受け取るバッファとして確保します taya
  {
    u32 i;
    u32 heapID = HEAPID_PROC;

    for(i=0; i<BTL_CLIENT_NUM; ++i)
    {
      if( i != BTL_CLIENT_PLAYER )
      {
        if( dst->playerStatus[i] == NULL ){
          dst->playerStatus[i] = MyStatus_AllocWork( heapID );
        }
        if( dst->party[i] == NULL ){
          dst->party[i] = PokeParty_AllocPartyWork( heapID );
        }
      }
    }
  }
  
  { //プレイヤー設定
    const POKEPARTY *myparty = BSUBWAY_SCRWORK_GetPokePartyUse( wk );
    client = BTL_CLIENT_PLAYER;
    
    //MyStatus
    dst->playerStatus[client] = mystatus;
    
    //トレーナーデータ
    dst->tr_data[client] = create_BSP_TRAINER_DATA( HEAPID_PROC );
    tr_data = dst->tr_data[client];
    
    MyStatus_CopyNameString( mystatus, tr_data->name );
    tr_data->tr_type = TRTYPE_HERO + MyStatus_GetMySex( mystatus );
    
    //ポケモンパーティ
    dst->party[client] = PokeParty_AllocPartyWork( HEAPID_PROC );
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      pp = PokeParty_GetMemberPointer( myparty, wk->member[i] );
      POKETOOL_CopyPPtoPP( pp, entry_pp );
      
      if( PP_Get(pp,ID_PARA_level,NULL) != 50 ){
        u32 exp = POKETOOL_GetMinExp(
              PP_Get(pp,ID_PARA_monsno,NULL),
              PP_Get(pp,ID_PARA_form_no,NULL),
              50 );
        
        PP_Put( entry_pp, ID_PARA_exp, exp );
        PP_Renew( entry_pp );
      }
      
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }

  { //敵トレーナー設定
    client = BTL_CLIENT_ENEMY1;
    
    //トレーナーデータ
    dst->tr_data[client] = create_BSP_TRAINER_DATA( HEAPID_PROC );
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->tr_data[0];
    bsw_trainer = &bsw_partner->bt_trd;
    
    if( play_mode == BSWAY_MODE_WIFI ){
      tr_data->tr_id = 0;
    }else{
      tr_data->tr_id = bsw_trainer->player_id;
    }
    
    tr_data->tr_type = bsw_trainer->tr_type;
//    tr_data->ai_bit = 0xFFFFFFFF;  //最強
    tr_data->ai_bit = 0x00000087;  //最強
    
    //トレーナーデータ　name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //トレーナーデータ　word
    PMSDAT_Clear( &tr_data->win_word );
    PMSDAT_Clear( &tr_data->lose_word );
    
    if( play_mode == BSWAY_MODE_WIFI ){
      tr_data->win_word = *(PMS_DATA*)bsw_trainer->win_word;
      tr_data->lose_word = *(PMS_DATA*)bsw_trainer->lose_word;
    }
    
    //ポケモンパーティ
//    dst->party[client] = PokeParty_AllocPartyWork( HEAPID_PROC );
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
  
  if( dst->multiMode != BTL_MULTIMODE_NONE ) //マルチ
  { //敵トレーナー２設定
    client = BTL_CLIENT_ENEMY2;
    
    dst->tr_data[client] = create_BSP_TRAINER_DATA( HEAPID_PROC );
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->tr_data[1];
    bsw_trainer = &bsw_partner->bt_trd;
    
    tr_data->tr_id = bsw_trainer->player_id;
    tr_data->tr_type = bsw_trainer->tr_type;
//    tr_data->ai_bit = 0xFFFFFFFF;  //最強
    tr_data->ai_bit = 0x00000087;  //最強
    
    //トレーナーデータ　name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //トレーナーデータ　word
    PMSDAT_Clear( &tr_data->win_word );
    PMSDAT_Clear( &tr_data->lose_word );
    
    //ポケモンパーティ
//  dst->party[client] = PokeParty_AllocPartyWork( HEAPID_PROC );
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
  
  if( dst->multiMode == BTL_MULTIMODE_PA_AA ) //AIマルチ
  { //AIパートナー設定
    client = BTL_CLIENT_PARTNER;
    
    KAGAYA_Printf( "パートナーNo.%d\n", wk->partner );

    //トレーナーデータ
    dst->tr_data[client] = create_BSP_TRAINER_DATA( HEAPID_PROC );
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->five_data[wk->partner];
    bsw_trainer = &bsw_partner->bt_trd;
    
    tr_data->tr_id = bsw_trainer->player_id;
    tr_data->tr_type = bsw_trainer->tr_type;
//    tr_data->ai_bit = 0xFFFFFFFF;  //最強
    tr_data->ai_bit = 0x00000087;  //最強
    
    //トレーナーデータ　name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //トレーナーデータ　word
    PMSDAT_Clear( &tr_data->win_word );
    PMSDAT_Clear( &tr_data->lose_word );
    
    //ポケモンパーティ
//    dst->party[client] = PokeParty_AllocPartyWork( HEAPID_PROC );
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
#if 0
  else if( dst->multiMode == BTL_MULTIMODE_PP_AA ) //通信マルチ
  { //通信マルチパートナー設定
  }
#endif
  
  return dst;
}
#endif

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 */
//--------------------------------------------------------------
COMM_BTL_DEMO_PARAM * BSUBWAY_SCRWORK_CreateBattleDemoParam(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  COMM_BTL_DEMO_PARAM *demo;
  demo = &bsw_scr->comm_btl_demo_param;
  return( demo );
}

//======================================================================
//  通信関連コマンド
//======================================================================
#if 0
/**
 *  @brief  バトルサブウェイ　送られてきたプレイヤーデータを受け取る
 */
u16 BTowerComm_RecvPlayerData(FIELDSYS_WORK* fsys,const u16* recv_buf)
{
  u16  ret = 0;
  BSUBWAY_SCRWORK* wk = fsys->btower_wk;

  wk->pare_sex = (u8)recv_buf[0];
  wk->pare_poke[0] = recv_buf[1];
  wk->pare_poke[1] = recv_buf[2];
  wk->pare_stage = recv_buf[3];

  wk->partner = 5+wk->pare_sex;

  OS_Printf("sio multi mem = %d,%d:%d,%d\n",wk->mem_poke[0],wk->mem_poke[1],wk->pare_poke[0],wk->pare_poke[1]);
  if(  wk->mem_poke[0] == wk->pare_poke[0] ||
    wk->mem_poke[0] == wk->pare_poke[1]){
    ret += 1;
  }
  if(  wk->mem_poke[1] == wk->pare_poke[0] ||
    wk->mem_poke[1] == wk->pare_poke[1]){
    ret += 2;
  }
  return ret;
}

/**
 *  @brief  バトルサブウェイ　送られてきたトレーナーデータを受け取る
 */
u16  BTowerComm_RecvTrainerData(FIELDSYS_WORK* fsys,const u16* recv_buf)
{
  int i;
  BSUBWAY_SCRWORK* wk = fsys->btower_wk;

  if(CommGetCurrentID() == COMM_PARENT_ID){
    return 0;  //親は送信するだけなので受け取らない
  }

  MI_CpuCopy8(recv_buf,wk->trainer,BSUBWAY_STOCK_TRAINER_MAX*2);
  OS_Printf("sio multi trainer01 = %d,%d:%d,%d\n",
      wk->trainer[0],wk->trainer[1],wk->trainer[2],wk->trainer[3]);
  OS_Printf("sio multi trainer02 = %d,%d:%d,%d\n",
      wk->trainer[4],wk->trainer[5],wk->trainer[6],wk->trainer[7]);
  OS_Printf("sio multi trainer03 = %d,%d:%d,%d\n",
      wk->trainer[8],wk->trainer[9],wk->trainer[10],wk->trainer[11]);
  OS_Printf("sio multi trainer04 = %d,%d\n",
      wk->trainer[12],wk->trainer[13]);
  return 1;
}

/**
 *  @brief  バトルサブウェイ　送られてきたリタイアするかどうかの結果を受け取る
 *
 *  @retval  0  リタイアしない
 *  @retval  1  リタイアする
 */
u16  BTowerComm_RecvRetireSelect(FIELDSYS_WORK* fsys,const u16* recv_buf)
{
  int i;
  BSUBWAY_SCRWORK* wk = fsys->btower_wk;

  OS_Printf("sio multi retire = %d,%d\n",wk->retire_f,recv_buf[0]);
  if(wk->retire_f || recv_buf[0]){
    return 1;
  }
  return 0;
}

/**
 *  @brief  バトルサブウェイ　自機性別とモンスターNoを送信
 */
void BTowerComm_SendPlayerData(BSUBWAY_SCRWORK* wk,SAVEDATA *sv)
{
  int i;
  POKEPARTY* party;
  POKEMON_PARAM *pp;
  MYSTATUS  *my = SaveData_GetMyStatus(sv);

  wk->send_buf[0] = MyStatus_GetMySex(my);
  party = SaveData_GetTemotiPokemon(sv);
  for(i = 0;i < 2;i++){
    wk->send_buf[1+i] =
      PP_Get(PokeParty_GetMemberPointer(party,wk->member[i]),
            ID_PARA_monsno,NULL);
  }

  //ここはBSWAY_MODE_COMM_MULTI専用
  OS_Printf( "wk->play_mode = %d\n", wk->play_mode );
  wk->send_buf[3] =
    TowerScoreData_SetStage(wk->scoreSave,BSWAY_MODE_COMM_MULTI,BSWAY_DATA_get);
}

/**
 *  @brief  バトルサブウェイ　通信マルチ　抽選したトレーナーNoを子機に送信
 */
void BTowerComm_SendTrainerData(BSUBWAY_SCRWORK* wk)
{
  MI_CpuCopy8(wk->trainer,wk->send_buf,BSUBWAY_STOCK_TRAINER_MAX*2);
}

/**
 *  @brief  バトルサブウェイ　通信マルチ　リタイアするかどうかを互いに送信
 *
 *  @param  retire  TRUEならリタイア
 */
void BTowerComm_SendRetireSelect(BSUBWAY_SCRWORK* wk,u16 retire)
{
  //自分の選択結果をワークに保存
  wk->retire_f = retire;
  wk->send_buf[0] = retire;
}
#endif

//======================================================================
//  トレーナーデータ
//======================================================================
//--------------------------------------------------------------
//  トレーナー番号テーブル　ノーマル １〜３週目のラウンド１〜６
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Normal1_6[3][2] =
{
  {  1-1,  50-1}, //stage 1 round 1-6
  { 31-1,  70-1}, //stage 2 round 1-6
  { 51-1, 110-1}, //stage 3 round 1-6
};

//--------------------------------------------------------------
//  トレーナー番号テーブル　ノーマル １〜２週目のラウンド７
//  ３週目は専用の番号になる。
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Normal_Round7[2][2] =
{
  {  1-1,  50-1}, //stage 1 round 7
  { 31-1,  70-1}, //stage 2 round 7
};

//--------------------------------------------------------------
//  トレーナー番号テーブル　スーパー　１〜４週目のラウンド１〜６
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Super1_6[4][2]=
{
  {111-1,160-1},
  {141-1,180-1},
  {161-1,200-1},
  {181-1,220-1},
};

//--------------------------------------------------------------
//  トレーナー番号テーブル　スーパー　１〜４週目のラウンド７
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Super7[4][2]=
{
  {161-1,180-1},
  {181-1,200-1},
  {201-1,220-1},
  {221-1,240-1},
};

//--------------------------------------------------------------
//  トレーナー番号テーブル　スーパー　５週目以降の１〜７
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Super_Stage5[1][2]=
{
  {201-1,300-1}, //全部同じ
};

//--------------------------------------------------------------
/**
 * @brief  トレーナーナンバー抽選
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param stage ステージ数
 * @param round 何人目か
 * @param play_mode プレイモード
 * @retval u16 トレーナーナンバー
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_GetTrainerNo(
    BSUBWAY_SCRWORK* wk, u16 stage, u8 round, int play_mode, u8 pair_no )
{
  u16 no = 0;
  u16 super = 0;
  s16 boss_no = -1;
  const u16 *tbl;
  
  OS_Printf( "BSW GET TRAINER NO stage = %d round = %d\n", stage, round );
  
  switch( play_mode ) //スーパーモードか？
  {
  case BSWAY_MODE_S_SINGLE:
  case BSWAY_MODE_S_DOUBLE:
  case BSWAY_MODE_S_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    super = 1;
  }
  
  if( round == 6 ) //専用ボスか？
  {
    if( super == 0 )  //ノーマル
    {
      if( stage == 2 )
      {
        boss_no = 0;  //ノーマルボス
      }
    }
    else if( stage == 6 ) //スーパーでステージ6
    {
      boss_no = 1;  //スーパーボス
    }
  }
  
  if( boss_no != -1 ) //専用ボス
  {
    switch( play_mode )
    {
    case BSWAY_MODE_SINGLE:
    case BSWAY_MODE_S_SINGLE:
      if( boss_no == 0 )
      {
        no = BSW_TR_DATANO_SINGLE;
      }
      else if( boss_no == 1 )
      {
        no = BSW_TR_DATANO_S_SINGLE;
      }
      break;
    case BSWAY_MODE_DOUBLE:
    case BSWAY_MODE_S_DOUBLE:
      if( boss_no == 0 )
      {
        no = BSW_TR_DATANO_DOUBLE;
      }
      else if( boss_no == 1 )
      {
        no = BSW_TR_DATANO_S_DOUBLE;
      }
      break;
    case BSWAY_MODE_MULTI:
    case BSWAY_MODE_COMM_MULTI:
    case BSWAY_MODE_S_MULTI:
    case BSWAY_MODE_S_COMM_MULTI:
      if( boss_no == 0 )
      {
        if( pair_no == 0 )
        {
          no = BSW_TR_DATANO_MULTI0;
        }
        else
        {
          no = BSW_TR_DATANO_MULTI1;
        }
      }
      else if( boss_no == 1 )
      {
        if( pair_no == 0 )
        {
          no = BSW_TR_DATANO_S_MULTI0;
        }
        else
        {
          no = BSW_TR_DATANO_S_MULTI1;
        }
      }
      break;
    default:
      GF_ASSERT( 0 );
    }
  }
  else if( super == 0 ) //ノーマルモード
  {
    if( stage < 3 ) //ステージ２まで
    {
      if( round < 6 ) //ラウンド１〜６
      {
        tbl = TrainerNoRangeTable_Normal1_6[stage];
        no = tbl[1] - tbl[0] + 1;
        no = tbl[0] + (get_Rand(wk) % no);
      }
      else  //ラウンド７
      {
        if( stage < 2 )
        {
          tbl = TrainerNoRangeTable_Normal_Round7[stage];
          no = tbl[1] - tbl[0] + 1;
          no = tbl[0] + (get_Rand(wk) % no);
        }
        else //ステージ２以上はありえない
        {
          GF_ASSERT( 0 );
        }
      }
    }
    else //ステージ３以上はありえない
    {
      GF_ASSERT( 0 );
    }
  }
  else //スーパーモード
  {
    if( stage < 4 ) //ステージ４まで
    {
      if( round < 6 ) //ラウンド１〜６
      {
        tbl = TrainerNoRangeTable_Super1_6[stage];
        no = tbl[1] - tbl[0] + 1;
        no = tbl[0] + (get_Rand(wk) % no);
      }
      else //ラウンド７
      {
        tbl = TrainerNoRangeTable_Super7[stage];
        no = tbl[1] - tbl[0] + 1;
        no = tbl[0] + (get_Rand(wk) % no);
      }
    }
    else //スーパーステージ５以降
    {
      tbl = TrainerNoRangeTable_Super_Stage5[0];
      no = tbl[1] - tbl[0] + 1;
      no = tbl[0] + (get_Rand(wk) % no);
    }
  }
  
  OS_Printf( "BSW GET TRAINER NO GET NUM = %d\n", no );
  return no;
}

#if 0 //100425 del saito
//--------------------------------------------------------------
/**
 *  バトルサブウェイ　romトレーナーデータ展開
 *  BSUBWAY_TRAINER_ROM_DATA型をメモリ確保して返すので、
 *  呼び出し側が明示的に解放すること
 *  @param tr_data BSUBWAY_PARTER_DATA
 *  @param tr_no トレーナーナンバー
 *  @param heapID HEAPID
 *  @retval BSUBWAY_TRAINER_ROM_DATA*
 *  TOWER_AI_MULTI_ONLY フィールド上で呼ばれる処理
 *  TOWER_AI_MULTI_ONLY 似た処理frontier_tool.c Frontier_TrainerDataGet
 */
//--------------------------------------------------------------
static BSUBWAY_TRAINER_ROM_DATA * alloc_TrainerRomData(
    BSUBWAY_PARTNER_DATA *tr_data, u16 tr_no, HEAPID heapID )
{
  BSUBWAY_TRAINER_ROM_DATA  *trd;
  GFL_MSGDATA *msgdata;
  STRBUF *name;
  
  msgdata =  GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_btdtrname_dat, heapID );
  
  MI_CpuClear8(tr_data, sizeof(BSUBWAY_PARTNER_DATA));
  trd = get_TrainerRomData(tr_no,heapID);
  
  //トレーナーIDをセット
  tr_data->bt_trd.player_id = tr_no + 1; //1 origin
  
  //トレーナー出現メッセージ
  tr_data->bt_trd.appear_word[0] = 0xFFFF;
  tr_data->bt_trd.appear_word[1] = tr_no*3;
  
  //トレーナーデータをセット
  tr_data->bt_trd.tr_type=trd->tr_type;
  
  //GSデータからの移植による処理
  //wbでは存在していないタイプを書き換え
  #if 0
  if( check_TrainerType(trd->tr_type) == FALSE ){
    OS_Printf( "BSUBWAY ERROR TRAINER TYPE" );
    tr_data->bt_trd.tr_type = TRTYPE_TANPAN;
  }
  #endif
  
  OS_Printf( "BSUBWAY allocTrainerRomData NameNo = %d\n", tr_no );

  name = GFL_MSG_CreateString( msgdata, tr_no );
  GFL_STR_GetStringCode( name,
      &tr_data->bt_trd.name[0], BUFLEN_PERSON_NAME );
  GFL_STR_DeleteBuffer(name);
  GFL_MSG_Delete( msgdata );
  return trd;
}

//--------------------------------------------------------------
/// アイテムテーブル
//  持ちポケモン決定はランダムでしているが無限ループ防止のため、
//  ある程度まわしたら、ポケモンの不一致のみをチェックして、
//  アイテムを固定で持たせるためのアイテムテーブル
//--------------------------------------------------------------
static const u16 BattleTowerPokemonItem[]={
  ITEM_HIKARINOKONA,
  ITEM_RAMUNOMI,
  ITEM_TABENOKOSI,
  ITEM_SENSEINOTUME,
};

//--------------------------------------------------------------
/**
 * ポケモンの性格を取得
 * 性格は、個性乱数を25で割った余りから算出される
 * @param[in]  rnd  取得したい個性乱数
 * @return  取得した性格
 */
//--------------------------------------------------------------
static u8  get_PokeSeikaku(u32 rnd)
{
  return (u8)(rnd % 25);
}

//--------------------------------------------------------------
/**
 *  @brief  バトルサブウェイのポケモンパラメータ生成
 *  @param pwd  [in/out]ポケモンパラメータの展開場所
 *  @param poke_no  [in]タワーromデータポケモンナンバー
 *  @param poke_id  [in]ポケモンにセットするid
 *  @param poke_rnd [in]セットする個性乱数(0が引き渡されたら関数内で生成)
 *  @param pow_rnd  [in]ポケモンにセットするpow_rnd値
 *  @param mem_idx  [in]メンバーindex。一体目or二体目
 *  @param itemfix  [in]TRUEなら固定アイテム。FALSEならromデータのアイテム
 *  @param heapID [in]テンポラリメモリを確保するヒープID
 *  @retval u32 personal_rnd:生成されたポケモンの個性乱数値
 * TOWER_AI_MULTI_ONLY フィールド上で呼ばれる処理
 * TOWER_AI_MULTI_ONLY 似た処理frontier_tool.c Frontier_PokemonParamMake
 */
//--------------------------------------------------------------
static u32 make_PokemonParam(
    BSUBWAY_SCRWORK *bsw_scr, BSUBWAY_POKEMON *pwd,
    u16 poke_no, u32 poke_id, u32 poke_rnd, u8 pow_rnd,
    u8 mem_idx, BOOL itemfix, HEAPID heapID )
{
  int i;
  int  exp;
  u32  personal_rnd;
  u8  friend;
  BSUBWAY_POKEMON_ROM_DATA  prd_s;

  MI_CpuClear8(pwd,sizeof(BSUBWAY_POKEMON));

  //ロムデータロード
  get_PokemonRomData(&prd_s,poke_no);

  //モンスターナンバー
  pwd->mons_no=prd_s.mons_no;

  //フォルムナンバー
  pwd->form_no=prd_s.form_no;

  //装備道具
  if(itemfix){
    //50回以上まわしていたフラグがTUREなら、
    //装備アイテムは固定のものを持たせる
    pwd->item_no=BattleTowerPokemonItem[mem_idx];
  }else{
    //romデータに登録されたアイテムを持たせる
    pwd->item_no=prd_s.item_no;
  }

  //なつき度は255がデフォルト
  friend=255;
  for(i=0;i<WAZA_TEMOTI_MAX;i++){
    pwd->waza[i]=prd_s.waza[i];
    //やつあたりを持っているときは、なつき度を0にする
    if(prd_s.waza[i]==WAZANO_YATUATARI){
      friend=0;
    }
  }

  //IDNo
  pwd->id_no=poke_id;

  if(poke_rnd == 0){
    //個性乱数
    do{
//    personal_rnd=(gf_rand()|gf_rand()<<16);
      personal_rnd=(get_Rand(bsw_scr)|get_Rand(bsw_scr)<<16);
#if 0
    }while((prd_s.chr!=get_PokeSeikaku(personal_rnd))&&(PokeRareGetPara(poke_id,personal_rnd)==TRUE));
#else
    //プラチナはタワーも修正する(08.03.17)(似た処理がfrontier_tool.cにもあるので注意！)
    //データの性格と一致していない"もしくは"レアの時は、ループを回す
    }while((prd_s.chr!=get_PokeSeikaku(personal_rnd))||(
          POKETOOL_CheckRare(poke_id,personal_rnd)==TRUE));
#endif
    //OS_Printf( "決定したpersonal_rnd = %d\n", personal_rnd );
    //OS_Printf( "get_PokeSeikaku = %d\n",get_PokeSeikaku(personal_rnd) );
    //OS_Printf( "レアじゃないか = %d\n", PokeRareGetPara(poke_id,personal_rnd) );
    pwd->personal_rnd=personal_rnd;
  }else{
    pwd->personal_rnd = poke_rnd;  //0でなければ引数の値を使用
    personal_rnd = poke_rnd;
  }


  //パワー乱数
  pwd->hp_rnd=pow_rnd;
  pwd->pow_rnd=pow_rnd;
  pwd->def_rnd=pow_rnd;
  pwd->agi_rnd=pow_rnd;
  pwd->spepow_rnd=pow_rnd;
  pwd->spedef_rnd=pow_rnd;

  //努力値
#if 0 //wb null
  exp=0;
  for(i=0;i<6;i++){
    if(prd_s.exp_bit&No2Bit(i)){
      exp++;
    }
  }
  if((PARA_EXP_TOTAL_MAX/exp)>255){
    exp=255;
  }else{
    exp=PARA_EXP_TOTAL_MAX/exp;
  }
  for(i = 0;i < 6;i++){
    if(prd_s.exp_bit&No2Bit(i)){
      pwd->exp[i]=exp;
    }
  }
#endif

  //技ポイント
  pwd->pp_count=0;

  //国コード
  pwd->country_code = 0;

  //特性
  i= POKETOOL_GetPersonalParam( pwd->mons_no,0,POKEPER_ID_speabi2);
  if(i){
    if(pwd->personal_rnd&1){
      pwd->tokusei=i;
    }else{
      pwd->tokusei=POKETOOL_GetPersonalParam(pwd->mons_no,0,POKEPER_ID_speabi1);
    }
  }else{
    pwd->tokusei=POKETOOL_GetPersonalParam( pwd->mons_no,0,POKEPER_ID_speabi1);
  }

  //なつき度
  pwd->natuki=friend;

  //ニックネーム
  {
   GFL_MSGDATA *msgdata = GFL_MSG_Create(
       GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_monsname_dat,
       heapID );
    GFL_MSG_GetStringRaw( msgdata,
        pwd->mons_no, pwd->nickname, MONS_NAME_SIZE+EOM_SIZE );
    GFL_MSG_Delete( msgdata );
  }
  return personal_rnd;
}
#endif
//--------------------------------------------------------------
/**
 * バトルサブウェイトレーナーデータ生成
 * （ロムデータをBSUBWAY_PARTNER_DATA構造体に展開）
 * @param tr_data  [in/out]生成するBSUBWAY_PARTNAER_DATA構造体
 * @param tr_no [in]生成元になるトレーナーID
 * @param cnt [in]トレーナーに持たせるポケモンの数
 * @param set_poke_no [in]ペアトレーナーのポケモン（NULL チェックなし）
 * @param set_item_no [in]ペアのポケモンの装備アイテム（NULL チェックなし）
 * @param poke [in/out] 抽選ポケモン二体のパラメータ格納先(NULLだとなし）
 * @param heapID  [in]ヒープID
 * @retval TRUE 抽選ループが50回超えた FALSE 抽選ループが50回以内で終わった
 * TOWER_AI_MULTI_ONLY フィールド上で呼ばれる処理
 * TOWER_AI_MULTI_ONLY 似た処理tower_tool.c FSRomBattleTowerTrainerDataMake
 * TOWER_AI_MULTI_ONLY サロンで、5人衆の手持ちポケモンを生成している
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_MakeRomTrainerData(
    BSUBWAY_SCRWORK *bsw_scr, BSUBWAY_PARTNER_DATA *tr_data,
    u16 tr_no, int cnt,
    u16 *set_poke_no,u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM* poke,HEAPID heapID)
{
  BOOL      ret = 0;
#if 0     //100424 del saito  
  BSUBWAY_TRAINER_ROM_DATA  *trd;
  //トレーナーデータセット
  trd = alloc_TrainerRomData(tr_data,tr_no,heapID);

  //ポケモンデータをセット
  ret = set_BSWayPokemonParam(
      bsw_scr,trd,tr_no,&tr_data->btpwd[0],cnt,
      set_poke_no,set_item_no,poke,heapID);
  GFL_HEAP_FreeMemory( trd );
#else
  ret = FBI_TOOL_MakeRomTrainerData(
      tr_data, tr_no, cnt, set_poke_no, set_item_no, poke, heapID);
#endif  
  return ret;
}

//--------------------------------------------------------------
/**
 * バトルサブウェイトレーナーの持ちポケモンのパワー乱数を決定する
 * @param  tr_no  トレーナーナンバー
 * @return  パワー乱数
 * b_tower_fld.c→b_tower_evに移動
 */
//--------------------------------------------------------------
static u8 get_PowerRnd(u16 tr_no)
{
  u8  pow_rnd;

  if(tr_no<100){
    pow_rnd=(0x1f/8)*1;
  }
  else if(tr_no<120){
    pow_rnd=(0x1f/8)*2;
  }
  else if(tr_no<140){
    pow_rnd=(0x1f/8)*3;
  }
  else if(tr_no<160){
    pow_rnd=(0x1f/8)*4;
  }
  else if(tr_no<180){
    pow_rnd=(0x1f/8)*5;
  }
  else if(tr_no<200){
    pow_rnd=(0x1f/8)*6;
  }
  else if(tr_no<220){
    pow_rnd=(0x1f/8)*7;
  }
  else{
    pow_rnd=0x1f;
  }
  return pow_rnd;
}

//--------------------------------------------------------------
/**
 * バトルサブウェイ ペアトレーナーデータ再生成
 * （セーブされたAIマルチパートナーのデータを
 * BSUBWAY_PARTNER_DATA構造体に展開）
 * @param tr_data [in/out] 生成するBSUBWAY_PARTNAER_DATA構造体
 * @param tr_no [in]  生成元になるトレーナーID
 * @param fixitem [in] TUREなら固定アイテム、FALSEならromアイテムをセット
 * @param poke  [in] ポケモンデータ再生成に必要な構造体型データへのポインタ
 * @param heapID [in] ヒープID
 * @retval nothing
 * TOWER_AI_MULTI_ONLY フィールド上で呼ばれる処理
 * TOWER_AI_MULTI_ONLY AIマルチで休むの後、再開した時に呼ばれる
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_MakePartnerRomData(
    BSUBWAY_SCRWORK *wk, BSUBWAY_PARTNER_DATA *tr_data,
    u16 tr_no,BOOL itemfix,
    const BSUBWAY_PAREPOKE_PARAM* poke,HEAPID heapID)
{
  int  i;
  u8  pow_rnd = 0;
  BSUBWAY_TRAINER_ROM_DATA  *trd;

  //トレーナーデータセット
//  trd = alloc_TrainerRomData(tr_data,tr_no,heapID); 100425 del saito
  trd = FBI_TOOL_AllocTrainerRomData( tr_data,tr_no,heapID );

  //ポケモンデータをセット
  pow_rnd=get_PowerRnd(tr_no);

  for(i = 0;i < BSUBWAY_STOCK_PAREPOKE_MAX;i++){
#if 0   //100425 del saito    
    make_PokemonParam(
      wk,&(tr_data->btpwd[i]),
      poke->poke_no[i],poke->poke_tr_id,
      poke->poke_rnd[i],pow_rnd,i,itemfix,heapID);
#endif
    FBI_TOOL_MakePokemonParam(
        &(tr_data->btpwd[i]),
        poke->poke_no[i],poke->poke_tr_id,
        poke->poke_rnd[i],pow_rnd,i,itemfix,heapID );
  }
  GFL_HEAP_FreeMemory( trd );
}
#if 0 //100425 del saito
//--------------------------------------------------------------
/**
 * バトルサブウェイのポケモンを決める
 * @param trd [in]トレーナーデータ
 * @param tr_no [in]トレーナーナンバー
 * @param pwd [in/out]BSUBWAY_POKEMON構造体
 * @param cnt [in]トレーナーに持たせるポケモンの数
 * @param set_poke_no [in]ペアトレーナーポケモン (NULLだとチェックなし
 * @param set_item_no [in]ペアトレーナーポケモンアイテム (NULL チェックなし
 * @param poke [in/out]抽選ポケモン二体のパラメータ格納先(NULL チェックなし
 * @param heapID [in]ヒープID
 * @retval  FALSE  抽選ループが50回以内で終わった
 * @retval  TRUE  抽選ループが50回以内で終わらなかった
 * TOWER_AI_MULTI_ONLY フィールド上で呼ばれる処理
 */
//--------------------------------------------------------------
static BOOL set_BSWayPokemonParam(
    BSUBWAY_SCRWORK *wk, BSUBWAY_TRAINER_ROM_DATA *trd,
    u16 tr_no, BSUBWAY_POKEMON *pwd, u8 cnt,
    u16 *set_poke_no, u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM *poke, HEAPID heapID )
{
  int  i,j;
  u8  pow_rnd;
  u8  poke_index;
  u32  id;
  int  set_index;
  int  set_index_no[4];
  u32  set_rnd_no[4];
  int  set_count;
  int  loop_count;
  BOOL  ret = 0;
  BSUBWAY_POKEMON_ROM_DATA  prd_s;
  BSUBWAY_POKEMON_ROM_DATA  prd_d;

  //手持ちポケモンのMAXは4体まで
  GF_ASSERT(cnt<=4);

  set_count=0;
  loop_count=0;
  while(set_count!=cnt){
//    poke_index = gf_rand()%trd->use_poke_cnt;
    poke_index = get_Rand(wk)%trd->use_poke_cnt;
    set_index=trd->use_poke_table[poke_index];
    get_PokemonRomData(&prd_d,set_index);

    //モンスターナンバーのチェック（同一のポケモンは持たない）
    for(i=0;i<set_count;i++){
      get_PokemonRomData(&prd_s,set_index_no[i]);
      if(prd_s.mons_no==prd_d.mons_no){
        break;
      }
    }
    
    //ペアを組んでいるトレーナーの持ちポケモンとのチェック
    if(set_poke_no!=NULL){
      //モンスターナンバーのチェック（同一のポケモンは持たない）
      for(i=0;i<cnt;i++){
        if(set_poke_no[i]==prd_d.mons_no){
          break;
        }
      }
      if(i!=cnt){
        continue;
      }
    }

    //50回まわして、決まらないようなら、同一アイテムチェックはしない
    if(loop_count<50){
      //装備アイテムのチェック（同一のアイテムは持たない）
      for(i=0;i<set_count;i++){
        get_PokemonRomData(&prd_s,set_index_no[i]);
        if((prd_s.item_no)&&(prd_s.item_no==prd_d.item_no)){
          break;
        }
      }
      if(i!=set_count){
        loop_count++;
        continue;
      }
      //ペアを組んでいるトレーナーの持ちポケモンの装備アイテムとのチェック
      if(set_item_no!=NULL){
        //装備アイテムのチェック（同一のアイテムは持たない）
        for(i=0;i<cnt;i++){
          if((set_item_no[i]==prd_d.item_no) && (set_item_no[i]!=0)){
            break;
          }
        }
        if(i!=cnt){
          loop_count++;
          continue;
        }
      }
    }

    set_index_no[set_count]=set_index;
    set_count++;
  }

  pow_rnd=get_PowerRnd(tr_no);
//id=(gf_rand()|(gf_rand()<<16));
  id=(get_Rand(wk)|(get_Rand(wk)<<16));

  if(loop_count >= 50){
    ret = TRUE;
  }
  for(i=0;i<set_count;i++){
    set_rnd_no[i] = make_PokemonParam(wk,&(pwd[i]),
      set_index_no[i],id,0,pow_rnd,i,ret,heapID);
  }
  if(poke == NULL){
    return ret;
  }
  //ポインタがNULLでなければ、抽選されたポケモンの必要なパラメータを返す
  poke->poke_tr_id = id;
  for(i = 0;i< BSUBWAY_STOCK_PAREPOKE_MAX;i++){
    poke->poke_no[i] = set_index_no[i];
    poke->poke_rnd[i] = set_rnd_no[i];
  }
  return ret;
}

//--------------------------------------------------------------
/**
 *  バトルサブウェイトレーナーロムデータの読み出し
 * @param tr_no  [in]トレーナーナンバー
 * @param heapID  [in]メモリ確保するためのヒープID
 * TOWER_AI_MULTI_ONLY フィールド上で呼ばれる処理
 * TOWER_AI_MULTI_ONLY 似た処理 frontier_tool.c Frontier_TrainerRomDataGet
 */
//--------------------------------------------------------------
static BSUBWAY_TRAINER_ROM_DATA * get_TrainerRomData(
    u16 tr_no, HEAPID heapID )
{
  tr_no += BSW_TR_ARCDATANO_ORG;
  OS_Printf( "BSUBWAY load TrainerRomData Num = %d\n", tr_no );
  return GFL_ARC_UTIL_Load( ARCID_BSW_TR, tr_no, 0, heapID );
}
#endif
#if 0 //old gs
static void * get_TrainerRomData( u16 tr_no, HEAPID heapID )
{
#ifdef DEBUG_ONLY_FOR_kagaya
  OS_Printf( "BSUBWAY load TrainerRomData Num = %d\n", tr_no );
#endif
  //AIマルチ限定なのでプラチナ！
  return GFL_ARC_UTIL_Load( ARCID_PL_BTD_TR, tr_no, 0, heapID );
}

//--------------------------------------------------------------
/**
 *  バトルサブウェイポケモンロムデータの読み出し
 * @param[in]  prd    読み出したポケモンデータの格納先
 * @param[in]  index  読み出すポケモンデータのインデックス
 * TOWER_AI_MULTI_ONLY フィールド上で呼ばれる処理
 * TOWER_AI_MULTI_ONLY 似た処理 frontier_tool.c Frontier_PokemonRomDataGet
 */
//--------------------------------------------------------------
static void get_PokemonRomData(
    BSUBWAY_POKEMON_ROM_DATA *prd, int index)
{
//  index += BSW_PM_ARCDATANO_ORG;
  OS_Printf( "BSUBWAY load PokemonRomData Num = %d\n", index );
  GFL_ARC_LoadData( (void*)prd, ARCID_BSW_PD, index );
}

#if 0 //old gs
static void get_PokemonRomData(
    BSUBWAY_POKEMON_ROM_DATA *prd,int index)
{
  //ここは通信はありえないのでプラチナ限定！(AIマルチ)
  GFL_ARC_LoadData( (void*)prd, ARCID_PL_BTD_PM, index );
}
#endif

//--------------------------------------------------------------
/**
 *  バトルタワートレーナーデータ生成
 * @param[in/out]  bp      生成するBATTLE_PARAM構造体
 * @param[in]    tr_data    生成元になるトレーナーデータ
 * @param[in]    cnt      トレーナーの持ちポケモン数
 * @param[in]    client_no  生成するclient_no
 * @param[in]    heapID    ヒープID（POKEMON_PARAMの生成に必要）
 */
//--------------------------------------------------------------
static void  make_TrainerData(
    BATTLE_SETUP_PARAM *bp, BSUBWAY_PARTNER_DATA *tr_data,
    int cnt, BTL_CLIENT_ID client_no, HEAPID heapID )
{
  int i,j;
  PMS_DATA *pd;
  POKEMON_PARAM  *pp;
  
  //トレーナーIDをセット
  bp->tr_data[client_no]->tr_id = tr_data->bt_trd.player_id;
  
  //トレーナーデータをセット
  bp->tr_data[client_no]->tr_type = tr_data->bt_trd.tr_type;
  
#if 0
  //GSデータからの移植による処理
  //wbでは存在していないタイプを書き換え
  if( check_TrainerType(bp->tr_data[client_no]->tr_type) == FALSE ){
    bp->tr_data[client_no]->tr_type = TRTYPE_TANPAN;
  }
#endif

//PM_strcpy( bp->tr_data[client_no].name, &tr_data->bt_trd.name[0] );
  GFL_STR_SetStringCode( bp->tr_data[client_no]->name, tr_data->bt_trd.name );
  
  pd = (PMS_DATA*)&tr_data->bt_trd.win_word[0];
  bp->tr_data[client_no]->win_word = *pd;
  pd = (PMS_DATA*)&tr_data->bt_trd.lose_word[0];
  bp->tr_data[client_no]->lose_word = *pd;
  
  //ポケモンデータをセット
  {
    POKEPARTY **party = &bp->party[client_no];

    pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    *party = PokeParty_AllocPartyWork( HEAPID_PROC );
    PokeParty_Init( *party, TEMOTI_POKEMAX );
    
    for(i=0;i<cnt;i++){
      make_PokePara( &tr_data->btpwd[i], pp );
      PokeParty_Add( *party, pp );
    }
  
    GFL_HEAP_FreeMemory( pp );
  }
}

//======================================================================
//  パーツ
//======================================================================
//--------------------------------------------------------------
/**
 * 文字列コピー
 * @param  to_str    コピー先バッファ
 * @param  from_str  文字列データ
 * @return  バッファにコピーしたデータ最後のEOM_へのポインタ
 *  データがない(文字列データ先頭がEOM_だった)場合、
 *  EOM_をバッファ先頭に書きこみ、そこへのポインタを返す
 */
//--------------------------------------------------------------
static STRCODE * PM_strcpy( STRCODE* to_str, const STRCODE* from_str )
{
  while( *from_str != GFL_STR_GetEOMCode() ){
    *to_str = *from_str;
    to_str++;
    from_str++;
  }
  *to_str = GFL_STR_GetEOMCode();
  return to_str;
}
#endif    //100425 del saito
/**
 *  @brief  参加できないポケモン名をタグ展開
 */
#if 0
WORDSET * BtlTower_SetNgPokeName(SAVEDATA* sv,u16 pokenum,u16 sex,u8 flag,u8* num)
{
  u8  i;
  u16  mons;
  STRBUF *buf,*nbuf;
  ZUKAN_WORK* zukan;
  WORDSET*  wset;
  MSGDATA_MANAGER* man;

  buf = STRBUF_Create( BUFLEN_POKEMON_NAME+BUFLEN_EOM_SIZE, HEAPID_FIELD );
  nbuf = STRBUF_Create( 2, HEAPID_FIELD );
  zukan = SaveData_GetZukanWork(sv);

  man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_monsname_dat, HEAPID_FIELD );
  wset = WORDSET_CreateEx(BSUBWAY_EXPOKE_NUM+1,BUFLEN_POKEMON_NAME+BUFLEN_EOM_SIZE,HEAPID_FIELD);

  WORDSET_RegisterNumber( wset,0,pokenum,1,NUMBER_DISPTYPE_LEFT,NUMBER_CODETYPE_DEFAULT);

  for(i = 0;i < BSUBWAY_EXPOKE_NUM;i++){
    mons = BattleTowerExPoke_MonsNoGet(i);
    if(ZukanWork_GetPokeSeeFlag(zukan,mons)){
      MSGMAN_GetString(man,mons,buf);
      //単語セット
      //バッファID
      //文字列
      //性別コード
      //単／複（TRUEで単数）
      //言語コード
      WORDSET_RegisterWord( wset,(*num)+1,buf,sex,flag,PM_LANG );
      (*num)++;
    }
  }

  MSGMAN_Delete(man);
  STRBUF_Delete( nbuf );
  STRBUF_Delete( buf );

  return wset;
}
#endif

//--------------------------------------------------------------
/**
 * 乱数取得
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval u16 乱数
 * @note 乱数固定の名残
 */
//--------------------------------------------------------------
static u16 get_Rand( BSUBWAY_SCRWORK *wk )
{
  return( GFUser_GetPublicRand(0xffffffff)/65535 );
}

//======================================================================
//  wb null
//======================================================================
#if 0
static void btwoer_TrainerDataMake(
    u8 play_mode,BSUBWAY_TRAINER* src,BSP_TRAINER_DATA * dest, HEAPID heapID )
{
  MI_CpuClear8(dest,sizeof(BSP_TRAINER_DATA));

  dest->tr_type = src->tr_type;  //トレーナータイプ
//dest->tr_gra = 0;//src->tr_type;  //トレーナータイプ
  dest->ai_bit = 0xFFFFFFFF;  //最強

  //トレーナー名
  dest->name = GFL_STR_CreateBuffer( PERSON_NAME_SIZE+EOM_SIZE,

//  GFL_STR_CopyBuffer(
  PM_strcpy(dest->name,src->name);

  //勝ち負けメッセージ
  MI_CpuCopy8(src->win_word,&dest->win_word,sizeof(PMS_DATA));
  MI_CpuCopy8(src->lose_word,&dest->lose_word,sizeof(PMS_DATA));

  switch(play_mode){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_WIFI:
    dest->fight_type = BTL_RULE_SINGLE;
    break;
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_WIFI_MULTI:
    dest->fight_type = BTL_RULE_DOUBLE;
    break;
  }
}
#endif

/**
 *  @brief  トレーナー対戦データ生成
 */
#if 0
static void btltower_SetTrainerData(BSUBWAY_SCRWORK* wk,BATTLE_PARAM* bp,
    BSUBWAY_PARTNER_DATA* tr,u8 tr_id,u8 tr_client,u8 poke_client )
{
  int i;
  POKEMON_PARAM *pp;

  //トレーナーデータをセット
  btower_TrainerDataMake(
      wk->play_mode,&(tr->bt_trd),&(bp->trainer_data[tr_client]));

  //トレーナーID入力
  bp->trainer_id[tr_client] = tr_id;//wk->tr_data[0].bt_trd.tr_type;

  //対戦相手のポケモンデータをセット
  pp = GFL_HEAP_AllocClearMemory( wk->heapID, POKETOOL_GetWorkSize() );

  PokeParty_Init(bp->poke_party[poke_client],wk->member_num);

  for(i = 0;i < wk->member_num;i++){
    make_PokePara(&(tr->btpwd[i]),pp);
    #if 0
    BattleParam_AddPokemon(bp,pp,poke_client);
    #else
    {
      int result;
      result = PokeParty_Add(bp->poke_party[client_no], pp);
    }
    #endif
  }
  GFL_HEAP_FreeMemory( pp );
}
#endif

