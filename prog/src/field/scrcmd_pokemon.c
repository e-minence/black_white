//======================================================================
/**
 * @file	scrcmd_pokemon.c
 * @brief	スクリプトコマンド：ポケモン関連
 * @date  2009.09.15
 * @author	tamada GAMEFREAK inc.
 *
 */
//======================================================================

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_def.h"
#include "script_local.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_pokemon.h"
#include "poke_tool/tokusyu_def.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"

#include "poke_tool/status_rcv.h" //PokeParty_RecoverAll

#include "savedata/box_savedata.h"

#include "event_poke_status.h" // for EvCmdPartyPokeSelect

#include "poke_tool/poke_memo.h"  //POKE_MEMO_SetTrainerMemoPP
#include "field/zonedata.h" //ZONEDATA_GetPlaceNameID


#include "app/waza_oshie.h"  // for WAZAOSHIE_xxxx

#include "msg/msg_place_name.h"  // for MAPNAME_xxxx

#include "waza_tool/wazano_def.h" // for WAZANO_xxxx

#include "item/item.h"    //for ITEM_
#include "item/itemtype_def.h"    //for ITEMTYPE_BALL

#include "print/global_msg.h"  //for GlobalMsg_PokeName


//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================

typedef struct ADD_POKE_PRM_tag
{
  HEAPID HeapID;
  int MonsNo;
  int FormNo;
  int Level;
  int ItemNo;
  PtlTokuseiSpec Tokusei;
  PtlSexSpec SexSel;
  PtlRareSpec RareSel;
  int Ball;
}ADD_POKE_PRM;

static GMEVENT_RESULT EVENT_FUNC_PokeSelect(GMEVENT * event, int * seq, void * work);

static POKEMON_PARAM* MakePokeParam( GAMEDATA *gdata, ADD_POKE_PRM *prm );
static BOOL AddPokeToBox(GAMEDATA* gdata, ADD_POKE_PRM *prm);
static BOOL AddPokeToParty(GAMEDATA* gdata, ADD_POKE_PRM *prm);

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ツール関数：指定位置の手持ちポケモンを取得する
 * @param work
 * @param pos
 * @param poke_para
 * @return  BOOL  取得に成功したとき=TRUE、失敗した時=FALSE（位置が大きすぎるなど）
 */
//--------------------------------------------------------------
BOOL SCRCMD_GetTemotiPP( SCRCMD_WORK * work, u16 pos, POKEMON_PARAM ** poke_para )
{
  GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );
  u32 max = PokeParty_GetPokeCount( party );
  BOOL result;

  if (pos < max)
  {
    result = TRUE;
  }
  else
  {
    GF_ASSERT_MSG(0, "Temoti Pos %d over max(%d)!!\n", pos, max);
    result = FALSE;
    pos = 0;
  }
  *poke_para = PokeParty_GetMemberPointer( party, pos );
  return result;
}

//--------------------------------------------------------------
/**
 * @brief ツール関数：指定位置の手持ちポケモンのパラメータを取得する
 * @param work
 * @param pos
 * @param param_id
 * @return  u32   取得したパラメータ
 *
 * 名前など、u32で返すことができない物を取得しようとするとアクセス例外になる
 */
//--------------------------------------------------------------
u32 SCRCMD_GetTemotiPPValue( SCRCMD_WORK * work, u16 pos, int param_id )
{
  POKEMON_PARAM * pp;
  if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == TRUE )
  {
    return PP_Get( pp, param_id, NULL );
  }
  return 0;
}
//--------------------------------------------------------------
/**
 * @brief ツール関数：指定位置の手持ちポケモンのパラメータをセットする
 * @param work
 * @param pos
 * @param param_id
 * @param value
 * @return  BOOL  セットできた場合TRUEを返す
 *
 */
//--------------------------------------------------------------
BOOL SCRCMD_SetTemotiPPValue( SCRCMD_WORK *work, u16 pos, int param_id, u32 value )
{
  POKEMON_PARAM * pp;
  if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == TRUE )
  {
    PP_Put( pp, param_id, value );
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------
/**
 * @brief ツール関数：指定したわざを持っているか？のチェック
 * @param pp
 * @param wazano
 * @return  BOOL
 */
//--------------------------------------------------------------
BOOL SCRCMD_CheckTemotiWaza( const POKEMON_PARAM * pp, u16 wazano )
{
  //たまごチェック
  if( PP_Get(pp,ID_PARA_tamago_flag,NULL) != 0 ){
    return FALSE;
  }
  //技リストからチェック
  if( PP_Get(pp,ID_PARA_waza1,NULL) == wazano ||
      PP_Get(pp,ID_PARA_waza2,NULL) == wazano ||
      PP_Get(pp,ID_PARA_waza3,NULL) == wazano ||
      PP_Get(pp,ID_PARA_waza4,NULL) == wazano ){
    return TRUE;
  }
  return FALSE;
}

//======================================================================
//  ポケモン関連
//======================================================================
//--------------------------------------------------------------
/**
 * @brief てもちポケモンのタイプ取得
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeType( VMHANDLE * core, void *wk )
{
  u16 * ret_wk1 = SCRCMD_GetVMWork( core, wk );  //結果格納ワーク
  u16 * ret_wk2 = SCRCMD_GetVMWork( core, wk );  //結果格納ワーク
  u16 pos = SCRCMD_GetVMWorkValue( core, wk );  //ポケモンの位置
  POKEMON_PARAM * pp;
  if ( SCRCMD_GetTemotiPP( wk, pos, &pp ) == TRUE )
  {
    *ret_wk1 = PP_Get( pp, ID_PARA_type1, NULL );
    *ret_wk2 = PP_Get( pp, ID_PARA_type2, NULL );
  }
  else
  {
    *ret_wk1 =  POKETYPE_NORMAL;
    *ret_wk2 =  POKETYPE_NORMAL;
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * 手持ちポケモン回復
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPokemonRecover( VMHANDLE * core, void *wk )
{
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );

  STATUS_RCV_PokeParty_RecoverAll( party );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンの努力値チェック
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetParamEXP( VMHANDLE *core, void *wk )
{
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );  //結果格納ワーク
  u16 pos = SCRCMD_GetVMWorkValue( core, wk );  //ポケモンの位置
  POKEMON_PARAM * pp;
  u16 exp = 0;

  if ( SCRCMD_GetTemotiPP( wk, pos, &pp ) == TRUE )
  {
    exp += PP_Get( pp, ID_PARA_hp_exp, NULL );
    exp += PP_Get( pp, ID_PARA_pow_exp, NULL );
    exp += PP_Get( pp, ID_PARA_def_exp, NULL );
    exp += PP_Get( pp, ID_PARA_agi_exp, NULL );
    exp += PP_Get( pp, ID_PARA_spepow_exp, NULL );
    exp += PP_Get( pp, ID_PARA_spedef_exp, NULL );
  }
  *ret_wk = exp;

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンのHPチェック
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPokemonHP( VMHANDLE * core, void *wk )
{

  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );  //結果格納ワーク
  u16 pos = SCRCMD_GetVMWorkValue( core, wk );  //ポケモンの位置

  {
    POKEMON_PARAM * pp;
    *ret_wk = FALSE;
    if ( SCRCMD_GetTemotiPP( wk, pos, &pp ) == TRUE )
    {
      u16 nowHP, maxHP;
      nowHP = PP_Get( pp, ID_PARA_hp, NULL );
      maxHP = PP_Get( pp, ID_PARA_hpmax, NULL );
      if (nowHP == maxHP || PP_Get( pp, ID_PARA_tamago_flag, NULL ) == TRUE)
      {
        *ret_wk = TRUE;
      }
    }
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンのタマゴかどうかの判定
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPokemonEgg( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*   work = (SCRCMD_WORK*)wk;
  u16*         ret_wk = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16             pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定
  POKEMON_PARAM * pp;

  *ret_wk = FALSE;
  if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == TRUE )
  {
    if ( PP_Get( pp, ID_PARA_tamago_flag, NULL ) == 0 )
    {
      *ret_wk = FALSE;
    }
    else
    {
      *ret_wk = TRUE;
    }
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンのなつき度を取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPokemonFriendValue( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*            ret_wk = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16             pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定

  POKEMON_PARAM * pp;
  *ret_wk = 0;
  if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == TRUE )
  {
    if ( PP_Get( pp, ID_PARA_tamago_flag, NULL ) == FALSE )
    {
      *ret_wk = PP_Get( pp, ID_PARA_friend, NULL );
    }
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンのなつき度をセット
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetPokemonFriendValue( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16             pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定
  u16             value = SCRCMD_GetVMWorkValue( core, wk );  // 設定値
  u16             mode = VMGetU16( core );  // 設定モード(0:セット,1:足す,2:引く)

  u8            friend = 0;
  POKEMON_PARAM* pp = NULL;

  if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == FALSE )
  { // ポケモン指定に対する例外処理
    return VMCMD_RESULT_CONTINUE;
  }

  // なつき度取得
  if(PP_Get( pp, ID_PARA_tamago_flag, NULL ) == TRUE)  //タマゴならなにもしない
  {
    return VMCMD_RESULT_CONTINUE;
  }
  friend = PP_Get( pp, ID_PARA_friend, NULL); 

  switch(mode)
  {
  case 0: //セット
    if(value > PTL_FRIEND_MAX){
      friend = PTL_FRIEND_MAX;
    }else if(value < 0){
      friend = 0;
    }else{
      friend = value;
    }
    break;
  case 1: //加算
    if((friend + value) > PTL_FRIEND_MAX) {
      friend = PTL_FRIEND_MAX;
    }else{
      friend += value;
    }
    break;
  case 2: //減算
    if(friend < value){
      friend = 0;
    }else{
      friend -= value;
    }
  }
  PP_Put( pp, ID_PARA_friend, friend); 
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンのモンスターナンバーを取得
 * @note   タマゴのときは、MONSNO_TAMAGOがセットされます
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeMonsNo( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*          ret_wk = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16              pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定
  u32 tamago_flg;

  tamago_flg = SCRCMD_GetTemotiPPValue( work, pos, ID_PARA_tamago_flag );
  if (tamago_flg) *ret_wk = MONSNO_TAMAGO;
  else *ret_wk = SCRCMD_GetTemotiPPValue( work, pos, ID_PARA_monsno );
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンの形状ナンバーを取得する
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeFormNo( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*          ret_wk = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16              pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定

  *ret_wk = SCRCMD_GetTemotiPPValue( work, pos, ID_PARA_form_no );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyFrontPoke( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK* work = (SCRCMD_WORK*)wk;
  u16*       ret_wk = SCRCMD_GetVMWork( core, work );     // コマンド第1引数
  u16          mode = SCRCMD_GetVMWorkValue( core, wk );  // コマンド第2引数
  GAMEDATA*   gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gdata );
  u16 num;

  switch( mode )
  {
  case POKECOUNT_MODE_BATTLE_ENABLE:  // 戦える = タマゴ以外
    num = PokeParty_GetMemberTopIdxBattleEnable( party );
    break;
  case POKECOUNT_MODE_NOT_EGG:  // タマゴを除く
    num = PokeParty_GetMemberTopIdxNotEgg( party );
    break;
  default:
    num = 0;
    GF_ASSERT_MSG(0, "Get Poke Index MODE:%d is not enable!!\n", mode );
    break;
  }

  *ret_wk = (u16)num;
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンの数を取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeCount( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = (SCRCMD_WORK*)wk;
  u16*       ret_wk = SCRCMD_GetVMWork( core, work );
  u16          mode = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA*   gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gdata );
  int           num = 0;

  // 指定されたモードに応じたポケモン数をカウント
  switch( mode )
  {
  case POKECOUNT_MODE_TOTAL:
    num = PokeParty_GetPokeCount( party );
    break;
  case POKECOUNT_MODE_NOT_EGG:
    num = PokeParty_GetPokeCountNotEgg( party );
    break;
  case POKECOUNT_MODE_BATTLE_ENABLE:
    num = PokeParty_GetPokeCountBattleEnable( party );
    break;
  case POKECOUNT_MODE_ONLY_EGG:
    num = PokeParty_GetPokeCountOnlyEgg( party );
    break;
  case POKECOUNT_MODE_ONLY_DAME_EGG:
    num = PokeParty_GetPokeCountOnlyDameEgg( party );
    break;
  case POKECOUNT_MODE_EMPTY:
    {
      int max = PokeParty_GetPokeCountMax( party );
      int now = PokeParty_GetPokeCount( party );
      if ( max<now )
      {
        GF_ASSERT_MSG(0,"max over max=%d now=%d",max, now);
        num = 0;
      }
      else num = max - now;
    }
    break;
  default:
    num = 0;
    break;
  }

  *ret_wk = (u16)num;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンの数を取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeCountByMonsNo( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = (SCRCMD_WORK*)wk;
  u16      target_monsno = SCRCMD_GetVMWorkValue( core, work );
  u16*      ret_wk = SCRCMD_GetVMWork( core, work );
  GAMEDATA*   gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gdata );
  
  u16 num,total;

  total = PokeParty_GetPokeCount( party );
  num = 0;

  if ( (0<target_monsno)&&(target_monsno<=MONSNO_END) )
  {
    int i;
    for (i=0;i<total;i++)
    {
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, i );
      if (pp != NULL)
      {
        u32 tamago_flag = PP_Get( pp, ID_PARA_tamago_flag, NULL );
        if (!tamago_flag)    //タマゴチェック
        {
          u16 monsno;
          //モンスターナンバーを取得
          monsno = PP_Get(pp,ID_PARA_monsno,NULL);
          if (monsno == target_monsno) num++;
        }
      }
    }

    *ret_wk = num;
  }
  else
  {
    GF_ASSERT(0);
    *ret_wk = 0;
  }
  return VMCMD_RESULT_CONTINUE;
} 

//--------------------------------------------------------------
/**
 * @brief ボックス内のポケモン数を取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBoxPokeCount( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = (SCRCMD_WORK*)wk;
  u16*       ret_wk = SCRCMD_GetVMWork( core, work );     // コマンド第1引数
  u16          mode = SCRCMD_GetVMWorkValue( core, wk );  // コマンド第2引数
  GAMEDATA*   gdata = SCRCMD_WORK_GetGameData( work );
  BOX_MANAGER*  box = GAMEDATA_GetBoxManager( gdata );
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gdata );
  int           num = 0;

  // 指定されたモードに応じたポケモン数をカウント
  switch( mode )
  {
  case POKECOUNT_MODE_TOTAL:  // タマゴ込み
    num = BOXDAT_GetPokeExistCountTotal( box );
    break;
  case POKECOUNT_MODE_NOT_EGG:  // タマゴを除く
  case POKECOUNT_MODE_BATTLE_ENABLE:  // 戦える = タマゴ以外
    num = BOXDAT_GetPokeExistCount2Total( box );
    break;
  case POKECOUNT_MODE_ONLY_EGG: // タマゴのみ
    num  = BOXDAT_GetPokeExistCountTotal( box );
    num -= BOXDAT_GetPokeExistCount2Total( box );
    break;
  case POKECOUNT_MODE_EMPTY:   //空き数
    {
      int max = BOX_POKESET_MAX;
      int now = BOXDAT_GetPokeExistCountTotal( box );
      if ( max<now )
      {
        GF_ASSERT_MSG(0,"max over max=%d now=%d",max, now);
        num = 0;
      }
      else num = max - now;
    }
    break;
  default:
    num = 0;
    break;
  }

  *ret_wk = (u16)num;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 指定モンスターナンバーのポケモンが手持ちのどこにいるかを取得（先頭から数えて始めに見つかった位置を返す）
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPosByMonsNo( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = (SCRCMD_WORK*)wk;
  u16      target_monsno = SCRCMD_GetVMWorkValue( core, work );
  u16*      ret_wk = SCRCMD_GetVMWork( core, work );
  u16*      pos = SCRCMD_GetVMWork( core, work );
  GAMEDATA*   gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gdata );
  
  u16 num,total;

  total = PokeParty_GetPokeCount( party );
  num = 0;

  if ( (0<target_monsno)&&(target_monsno<=MONSNO_END) )
  {
    int i;
    for (i=0;i<total;i++)
    {
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, i );
      if (pp != NULL)
      {
        u32 tamago_flag = PP_Get( pp, ID_PARA_tamago_flag, NULL );
        if (!tamago_flag)    //タマゴチェック
        {
          u16 monsno;
          //モンスターナンバーを取得
          monsno = PP_Get(pp,ID_PARA_monsno,NULL);
          if (monsno == target_monsno) break;
        }
      }
    }

    if (i < total)
    {
      *ret_wk = TRUE;
      *pos = i;
    }else *ret_wk = FALSE;
  }
  else
  {
    GF_ASSERT(0);
    *ret_wk = FALSE;
  }
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * 手持ちポケモンが指定された技を覚えているかチェック
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChkPokeWaza( VMHANDLE *core, void *wk )
{
  POKEMON_PARAM *pp;
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 waza = SCRCMD_GetVMWorkValue( core, wk );
  u16 tno = SCRCMD_GetVMWorkValue( core, wk );
  
  *ret_wk = 0;
  if ( SCRCMD_GetTemotiPP( wk, tno, &pp ) == TRUE)
  {
    if ( SCRCMD_CheckTemotiWaza( pp, waza ) == TRUE )
    {
      *ret_wk = 1;
    }
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * 手持ちポケモンが指定された技を覚えているかチェック　手持ちポケモン全体
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChkPokeWazaGroup( VMHANDLE *core, void *wk )
{
  int i,max;
  POKEMON_PARAM *pp;
  GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
  POKEPARTY *party = GAMEDATA_GetMyPokemon( gamedata );
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 waza = SCRCMD_GetVMWorkValue( core, wk );
  
  max = PokeParty_GetPokeCount( party );

#ifdef DEBUG_ONLY_FOR_kagaya  //test
  *ret_wk = 0;
  return VMCMD_RESULT_CONTINUE;
#endif
  
  for( i = 0, *ret_wk = 6; i < max; i++ ){
    pp = PokeParty_GetMemberPointer( party, i );
    
    if ( SCRCMD_CheckTemotiWaza( pp, waza ) == TRUE )
    {
      *ret_wk = i;
      break;
    }
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ポケモンを手持ちに追加
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdAddPokemonToParty( VMHANDLE *core, void *wk )
{
  ADD_POKE_PRM prm;
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*    gdata = SCRCMD_WORK_GetGameData( work );
  HEAPID     heap_id = SCRCMD_WORK_GetHeapID( work );
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gdata );
  MYSTATUS*   status = GAMEDATA_GetMyStatus( gdata );
  u16*        ret_wk = SCRCMD_GetVMWork( core, work );       // コマンド第1引数
  u16         monsno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第2引数
  u16         formno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第3引数
  u16          level = SCRCMD_GetVMWorkValue( core, work );  // コマンド第4引数

  //パラメータセット
  {
    prm.HeapID = heap_id;
    prm.MonsNo = monsno;
    prm.FormNo = formno;
    prm.Level = level;
    prm.ItemNo = 0; //アイテムなし
    prm.Tokusei = PTL_TOKUSEI_SPEC_BOTH;  //不問
    prm.SexSel = PTL_SEX_SPEC_UNKNOWN;    //不問
    prm.RareSel = PTL_RARE_SPEC_BOTH;     //不問
    prm.Ball = ITEM_MONSUTAABOORU;        //モンスターボール
  }

  *ret_wk = AddPokeToParty(gdata, &prm);
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ポケモンを手持ちに追加　詳細指定型
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdAddPokemonToPartyEx( VMHANDLE *core, void *wk )
{
  ADD_POKE_PRM prm;
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*    gdata = SCRCMD_WORK_GetGameData( work );
  HEAPID     heap_id = SCRCMD_WORK_GetHeapID( work );
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gdata );
  MYSTATUS*   status = GAMEDATA_GetMyStatus( gdata );
  u16*        ret_wk = SCRCMD_GetVMWork( core, work );       // コマンド第1引数
  u16         monsno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第2引数
  u16         formno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第3引数
  u16          level = SCRCMD_GetVMWorkValue( core, work );  // コマンド第4引数
  u16        tokusei = SCRCMD_GetVMWorkValue( core, work );  // コマンド第5引数
  u16         sex   = SCRCMD_GetVMWorkValue( core, work );   // コマンド第6引数
  u16         rare   = SCRCMD_GetVMWorkValue( core, work );  // コマンド第7引数
  u16         itemno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第8引数
  u16         ball   = SCRCMD_GetVMWorkValue( core, work );  // コマンド第9引数

  //パラメータセット
  {
    prm.HeapID = heap_id;
    prm.MonsNo = monsno;
    prm.FormNo = formno;
    prm.Level = level;
    prm.ItemNo = itemno;
    prm.Tokusei = tokusei;
    if ( prm.Tokusei >= PTL_TOKUSEI_SPEC_MAX)
    {
      GF_ASSERT_MSG(0,"%d:tokusei error",prm.Tokusei);
      prm.Tokusei = PTL_TOKUSEI_SPEC_BOTH;
    }
    prm.SexSel = sex;
    if ( prm.SexSel >= PTL_SEX_SPEC_MAX)
    {
      GF_ASSERT_MSG(0,"%d:sex error",prm.SexSel);
      prm.SexSel = PTL_SEX_SPEC_UNKNOWN;
    }
    prm.RareSel = rare;
    if ( prm.RareSel >= PTL_RARE_SPEC_MAX)
    {
      GF_ASSERT_MSG(0,"%d:rare error",prm.RareSel);
      prm.RareSel = PTL_RARE_SPEC_BOTH;
    }
    prm.Ball = ball;
  }

  *ret_wk = AddPokeToParty(gdata, &prm);
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * 手持ちポケモンの技個数取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPokemonWazaNum( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // コマンド第1引数
  u16                 pos = SCRCMD_GetVMWorkValue( core, work );       // コマンド第2引数

  int          waza_count = 0;
  POKEMON_PARAM *pp;
  
  if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == TRUE )
  {
    for( i=0; i<PTL_WAZA_MAX; i++ )
    {
      const u32 wazaNo = PP_Get( pp , ID_PARA_waza1+i , NULL );
      if( wazaNo != 0 )
      {
        waza_count++;
      }
    }
  }
  *ret_wk = waza_count;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * 技IDの取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPokemonWazaID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // コマンド第1引数
  u16            poke_pos = SCRCMD_GetVMWorkValue( core, work );       // コマンド第2引数
  u16            waza_pos = SCRCMD_GetVMWorkValue( core, work );       // コマンド第3引数
  
  POKEMON_PARAM *pp;
  if ( SCRCMD_GetTemotiPP( work, poke_pos, &pp ) == TRUE )
  {
    const u32 wazaNo = PP_Get( pp , ID_PARA_waza1+waza_pos , NULL );
    *ret_wk = wazaNo;
  }
  else
  {
    *ret_wk = 0;  //エラー対処
  }
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * 技の追加
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetPokemonWaza( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16            poke_pos = SCRCMD_GetVMWorkValue( core, work );       // コマンド第2引数
  u16            waza_pos = SCRCMD_GetVMWorkValue( core, work );       // コマンド第3引数
  u16             waza_id = SCRCMD_GetVMWorkValue( core, work );       // コマンド第3引数

  POKEMON_PARAM      *pp;
  BOOL          fastMode;

  if ( SCRCMD_GetTemotiPP( work, poke_pos, &pp ) == FALSE )
  { //エラー対処用
    return VMCMD_RESULT_CONTINUE;
  }

  fastMode = PP_FastModeOn( pp );

  if( waza_pos > PTL_WAZA_MAX )
  {
    //押し出しモード
    PP_SetWazaPush( pp , waza_id );
  }
  else
  if( waza_id == 0 )
  {
    //忘れモード
    u8 i;
    u8 forgetPos = waza_pos;
    if( waza_pos < PTL_WAZA_MAX-1 )
    {
      //詰める
      for( i=waza_pos ; i<PTL_WAZA_MAX-1 ; i++ )
      {
        const u32 temp_waza_no = PP_Get( pp , ID_PARA_waza1+i+1 , NULL );
        if( temp_waza_no != 0 )
        {
          const u32 temp_waza_cnt = PP_Get( pp , ID_PARA_pp_count1+i+1 , NULL );
          const u32 temp_waza_pp = PP_Get( pp , ID_PARA_pp1+i+1 , NULL );
          PP_Put( pp , ID_PARA_waza1+i , temp_waza_no );
          PP_Put( pp , ID_PARA_pp_count1+i , temp_waza_cnt );
          PP_Put( pp , ID_PARA_pp1+i , temp_waza_pp );

          forgetPos = i+1;
        }
        else
        {
          break;
        }
      }
    }
    PP_Put( pp , ID_PARA_waza1+forgetPos , 0 );
    PP_Put( pp , ID_PARA_pp_count1+forgetPos , 0 );
    PP_Put( pp , ID_PARA_pp1+forgetPos , 0 );
  }
  else
  {
    //置き換えモード
    PP_SetWazaPos( pp , waza_id , waza_pos );
    
  }
  PP_FastModeOff( pp , fastMode );
  return VMCMD_RESULT_CONTINUE;
  
}


//--------------------------------------------------------------
/**
 * @brief 指定の手持ちポケモンの親が自分かどうかをチェックする
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPokeOwner( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*   work = (SCRCMD_WORK*)wk;
  u16*         ret_wk = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16             pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定
  GAMEDATA*     gdata = SCRCMD_WORK_GetGameData( work );
  MYSTATUS*   status = GAMEDATA_GetMyStatus( gdata );
  POKEMON_PARAM* pp = NULL;

  if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == FALSE )
  {
    GF_ASSERT(0);
    *ret_wk = FALSE;    //親は他人
    return VMCMD_RESULT_CONTINUE;
  }
  
  {
    //ＩＤ同じか？
    u32 id;
    id = PP_Get( pp, ID_PARA_id_no, NULL );
    NOZOMU_Printf("poke_id = %d\n",id);
    NOZOMU_Printf("my_id = %d\n",MyStatus_GetID(status) );
    if ( id != MyStatus_GetID(status) )
    {
      *ret_wk = FALSE;    //親は他人
      return VMCMD_RESULT_CONTINUE;
    }
  }

  {
    //親名一致してるか？
    STRBUF*         strbuf1;
    STRBUF*         strbuf2;
    HEAPID          heap_id = SCRCMD_WORK_GetHeapID( work );
    strbuf1 = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, heap_id );
    strbuf2 = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, heap_id );
    if ( strbuf1 && strbuf2 )
    {
      PP_Get( pp, ID_PARA_oyaname, strbuf1 );
      MyStatus_CopyNameString( status,  strbuf2 );
      if (GFL_STR_CompareBuffer( strbuf1, strbuf2 ) == FALSE)
      {
        //バッファ解放
        GFL_STR_DeleteBuffer(strbuf1);
        GFL_STR_DeleteBuffer(strbuf2);
        NOZOMU_Printf("親名不一致\n");
        *ret_wk = FALSE;    //親は他人
        return VMCMD_RESULT_CONTINUE;
      }
    }
    else
    {
      GF_ASSERT(0);
      //バッファ確保出来なかったら他人とみなす
      *ret_wk = FALSE;    //親は他人
      return VMCMD_RESULT_CONTINUE;
    }
    //バッファ解放
    GFL_STR_DeleteBuffer(strbuf1);
    GFL_STR_DeleteBuffer(strbuf2);
  }

  {
    //親の性別同じか？
    u32 parent_sex;
    u32 my_sex;
    parent_sex = PP_Get( pp, ID_PARA_oyasex, NULL );
    my_sex = MyStatus_GetMySex(status);
    NOZOMU_Printf("parent_sex = %d\n",parent_sex);
    NOZOMU_Printf("my_sex = %d\n",my_sex);
    if ( parent_sex != my_sex )
    {
      *ret_wk = FALSE;    //親は他人
      return VMCMD_RESULT_CONTINUE;
    }
  }

  {
    //カセットバージョン同じか？
    u32 ver;
    ver = PP_Get( pp, ID_PARA_get_cassette, NULL );
    NOZOMU_Printf("poke_ver = %d\n",ver);
    NOZOMU_Printf("rom_ver = %d\n",CasetteVersion);
    if (ver != CasetteVersion)
    {
      *ret_wk = FALSE;    //親は他人
      return VMCMD_RESULT_CONTINUE;
    }
  }

  *ret_wk = TRUE;     //自分が親
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * フォルムチェンジ
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChgFormNo( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16            poke_pos = SCRCMD_GetVMWorkValue( core, work );       //手持ち位置
  u16            formno = SCRCMD_GetVMWorkValue( core, work );       //フォルムナンバー

  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  
  POKEMON_PARAM *pp = PokeParty_GetMemberPointer( party , poke_pos );
  BOOL rc;

  rc = PP_ChangeFormNo( pp, formno );
  GF_ASSERT(rc);
  
  //図鑑登録「見た」
  {
    ZUKAN_SAVEDATA *zw = GAMEDATA_GetZukanSave( gdata );
    ZUKANSAVE_SetPokeSee(zw, pp);
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ロトムフォルムチェンジ
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChgRotomFormNo( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16            poke_pos = SCRCMD_GetVMWorkValue( core, work );       //手持ち位置
  u16            waza_pos = SCRCMD_GetVMWorkValue( core, work );       //技位置
  u16            formno = SCRCMD_GetVMWorkValue( core, work );       //フォルムナンバー

  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  
  POKEMON_PARAM *pp = PokeParty_GetMemberPointer( party , poke_pos );
  BOOL rc;

  rc = PP_ChangeRotomFormNo( pp, formno, waza_pos );
  GF_ASSERT(rc);
  
  //図鑑登録「見た」
  {
    ZUKAN_SAVEDATA *zw = GAMEDATA_GetZukanSave( gdata );
    ZUKANSAVE_SetPokeSee(zw, pp);
  }

  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief 思い出し技の有無を取得する
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckRemaindWaza( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );     // コマンド第一引数(結果を受け取るワーク)
  u16            poke_pos = SCRCMD_GetVMWorkValue( core, work );// コマンド第二引数(手持ち位置)

  HEAPID          heap_id = SCRCMD_WORK_GetHeapID( work );
  u16*               waza = NULL;
  POKEMON_PARAM*     poke;

  if ( SCRCMD_GetTemotiPP( work, poke_pos, &poke ) == TRUE )
  {
  // 思い出し技の有無を返す
    waza = WAZAOSHIE_GetRemaindWaza( poke, heap_id );
    *ret_wk = WAZAOSHIE_WazaTableChack( waza );
    GFL_HEAP_FreeMemory( waza );
  }
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief 捕獲場所IDの取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPartyPokeGetPlace( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*          ret_wk = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16              pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定
  u16              type = SCRCMD_GetVMWorkValue( core, wk );  // POKE_GET_PLACE_CHECK_〜
  u16 place;
  static const u16 POKE_GET_PLACE_CHECK_PLACE[POKE_GET_PLACE_CHECK_MAX] = 
  {
    MAPNAME_WC10,
  };

  GF_ASSERT( type < POKE_GET_PLACE_CHECK_MAX );
  
  place = SCRCMD_GetTemotiPPValue( work, pos, ID_PARA_get_place );
  
  if( place == POKE_GET_PLACE_CHECK_PLACE[ type ] ){
    *ret_wk = TRUE;
  }else{
    *ret_wk = FALSE;
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 捕獲日時の取得
 * @note  捕獲日時は、ID_PARA_birth〜に入っています。ID_PARA_get〜ではありません
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeGetDate( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*          ret_year = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16*          ret_month = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16*          ret_day = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16              pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定
  POKEMON_PARAM* param;
  if ( SCRCMD_GetTemotiPP( work, pos, &param ) == TRUE )
  {
    *ret_year = (u16)PP_Get( param, ID_PARA_birth_year, NULL );
    *ret_month = (u16)PP_Get( param, ID_PARA_birth_month, NULL );
    *ret_day = (u16)PP_Get( param, ID_PARA_birth_day, NULL );
  }
  else
  {
    *ret_year = 0;
    *ret_month = 0;
    *ret_day = 0;
  }
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  u16 para_id;
  u16 max;
}SET_PARAM_DEF;
//--------------------------------------------------------------
//  定義とのズレがあればコンパイル時エラーにする
//--------------------------------------------------------------
SDK_COMPILER_ASSERT( SCR_POKEPARA_MONSNO == ID_PARA_monsno );
SDK_COMPILER_ASSERT( SCR_POKEPARA_ITEMNO == ID_PARA_item );
SDK_COMPILER_ASSERT( SCR_POKEPARA_COUNTRY_CODE == ID_PARA_country_code );
SDK_COMPILER_ASSERT( SCR_POKEPARA_HP_RND == ID_PARA_hp_rnd );
SDK_COMPILER_ASSERT( SCR_POKEPARA_POW_RND == ID_PARA_pow_rnd );
SDK_COMPILER_ASSERT( SCR_POKEPARA_DEF_RND == ID_PARA_def_rnd );
SDK_COMPILER_ASSERT( SCR_POKEPARA_AGI_RND == ID_PARA_agi_rnd );
SDK_COMPILER_ASSERT( SCR_POKEPARA_SPEPOW_RND == ID_PARA_spepow_rnd );
SDK_COMPILER_ASSERT( SCR_POKEPARA_SPEDEF_RND == ID_PARA_spedef_rnd );
SDK_COMPILER_ASSERT( SCR_POKEPARA_HAIHU_FLAG == ID_PARA_event_get_flag );
SDK_COMPILER_ASSERT( SCR_POKEPARA_SEX == ID_PARA_sex );
SDK_COMPILER_ASSERT( SCR_POKEPARA_FORMNO == ID_PARA_form_no );
SDK_COMPILER_ASSERT( SCR_POKEPARA_SEIKAKU == ID_PARA_seikaku );
//SDK_COMPILER_ASSERT( SCR_POKEPARA_GET_YEAR == ID_PARA_get_year );
//SDK_COMPILER_ASSERT( SCR_POKEPARA_GET_MONTH == ID_PARA_get_month );
//SDK_COMPILER_ASSERT( SCR_POKEPARA_GET_DAY == ID_PARA_get_day );
//SDK_COMPILER_ASSERT( SCR_POKEPARA_BIRTH_YEAR == ID_PARA_birth_year );
//SDK_COMPILER_ASSERT( SCR_POKEPARA_BIRTH_MONTH == ID_PARA_birth_month );
//SDK_COMPILER_ASSERT( SCR_POKEPARA_BIRTH_DAY == ID_PARA_birth_day );
SDK_COMPILER_ASSERT( SCR_POKEPARA_NICKNAME_FLAG == ID_PARA_nickname_flag );
SDK_COMPILER_ASSERT( SCR_POKEPARA_GET_ROM == ID_PARA_get_cassette );
//SDK_COMPILER_ASSERT( SCR_POKEPARA_GET_PLACE == ID_PARA_get_place );
//SDK_COMPILER_ASSERT( SCR_POKEPARA_BIRTH_PLACE == ID_PARA_birth_place );
SDK_COMPILER_ASSERT( SCR_POKEPARA_GET_LEVEL == ID_PARA_get_level );
SDK_COMPILER_ASSERT( SCR_POKEPARA_OYA_SEX == ID_PARA_oyasex );
SDK_COMPILER_ASSERT( SCR_POKEPARA_LEVEL == ID_PARA_level );

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンのパラメータセット
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetPartyPokeParameter( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16              pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定
  u16               id = SCRCMD_GetVMWorkValue( core, wk );  // 
  u16            value = SCRCMD_GetVMWorkValue( core, wk );  // セットする値
  static const SET_PARAM_DEF tbl[] = {
    { SCR_POKEPARA_HP_RND,     31 },
    { SCR_POKEPARA_POW_RND,    31 },
    { SCR_POKEPARA_DEF_RND,    31 },
    { SCR_POKEPARA_AGI_RND,    31 },
    { SCR_POKEPARA_SPEPOW_RND, 31 },
    { SCR_POKEPARA_SPEDEF_RND, 31 },
  };
  int i;
  for ( i = 0; i < NELEMS(tbl); i++ )
  {
    if ( id == tbl[i].para_id )
    {
      if ( value <= tbl[i].max )
      {
        POKEMON_PARAM * pp;
        if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == TRUE )
        {
          PP_Put( pp, id, value );
          PP_Renew( pp ); //PPを再計算
        }
      }
      else
      {
        GF_ASSERT( 0 ); //設定値が大きすぎる
      }
      return VMCMD_RESULT_CONTINUE;
    }
  }
  GF_ASSERT_MSG(0, "SCRCMD:POKEMON PARAMETER id=%dは未対応\n", id );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちポケモンのパラメータ取得
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPartyPokeParameter( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*          ret_wk = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16              pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定
  u16               id = SCRCMD_GetVMWorkValue( core, wk );  // 
  static const u8 id_tbl[] = {
    SCR_POKEPARA_MONSNO,
    SCR_POKEPARA_ITEMNO,
    SCR_POKEPARA_COUNTRY_CODE,
    SCR_POKEPARA_HAIHU_FLAG,
    SCR_POKEPARA_SEX,
    SCR_POKEPARA_FORMNO,
    SCR_POKEPARA_SEIKAKU,
    SCR_POKEPARA_NICKNAME_FLAG,
    SCR_POKEPARA_GET_ROM,
    //SCR_POKEPARA_GET_YEAR,
    //SCR_POKEPARA_GET_MONTH,
    //SCR_POKEPARA_GET_DAY,
    //SCR_POKEPARA_BIRTH_YEAR,
    //SCR_POKEPARA_BIRTH_MONTH,
    //SCR_POKEPARA_BIRTH_DAY,
    //SCR_POKEPARA_GET_PLACE,
    //SCR_POKEPARA_BIRTH_PLACE,
    SCR_POKEPARA_GET_LEVEL,
    SCR_POKEPARA_OYA_SEX,
    SCR_POKEPARA_LEVEL
  };
  int i;
  for ( i = 0; i < NELEMS(id_tbl); i++ )
  {
    if ( id == id_tbl[i] )
    {
      *ret_wk = SCRCMD_GetTemotiPPValue( work, pos, id );
      return VMCMD_RESULT_CONTINUE;
    }
  }
  GF_ASSERT_MSG(0, "SCRCMD:POKEMON PARAMETER id=%dは未対応\n", id );
  *ret_wk = 0;  //エラーよけ
  return VMCMD_RESULT_CONTINUE;
}



//======================================================================
//
//  ポケモン入手処理関連
//
//======================================================================
//--------------------------------------------------------------
/**
 * ポケモンをボックスに追加
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdAddPokemonToBox( VMHANDLE *core, void *wk )
{
  ADD_POKE_PRM prm;
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*    gdata = SCRCMD_WORK_GetGameData( work );
  HEAPID     heap_id = SCRCMD_WORK_GetHeapID( work );
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gdata );
  MYSTATUS*   status = GAMEDATA_GetMyStatus( gdata );
  u16*        ret_wk = SCRCMD_GetVMWork( core, work );       // コマンド第1引数
  u16         monsno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第2引数
  u16         formno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第3引数
  u16          level = SCRCMD_GetVMWorkValue( core, work );  // コマンド第4引数

  //パラメータセット
  {
    prm.HeapID = heap_id;
    prm.MonsNo = monsno;
    prm.FormNo = formno;
    prm.Level = level;
    prm.ItemNo = 0; //アイテムなし
    prm.Tokusei = PTL_TOKUSEI_SPEC_BOTH;  //不問
    prm.SexSel = PTL_SEX_SPEC_UNKNOWN;    //不問
    prm.RareSel = PTL_RARE_SPEC_BOTH;     //不問
    prm.Ball = ITEM_MONSUTAABOORU;        //モンスターボール
  }

  *ret_wk = AddPokeToBox(gdata, &prm);
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ポケモンをボックスに追加　詳細設定型
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdAddPokemonToBoxEx( VMHANDLE *core, void *wk )
{
  ADD_POKE_PRM prm;
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*    gdata = SCRCMD_WORK_GetGameData( work );
  HEAPID     heap_id = SCRCMD_WORK_GetHeapID( work );
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gdata );
  MYSTATUS*   status = GAMEDATA_GetMyStatus( gdata );
  u16*        ret_wk = SCRCMD_GetVMWork( core, work );       // コマンド第1引数
  u16         monsno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第2引数
  u16         formno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第3引数
  u16          level = SCRCMD_GetVMWorkValue( core, work );  // コマンド第4引数
  u16        tokusei = SCRCMD_GetVMWorkValue( core, work );  // コマンド第5引数
  u16        sex = SCRCMD_GetVMWorkValue( core, work );      // コマンド第6引数
  u16        rare = SCRCMD_GetVMWorkValue( core, work );     // コマンド第7引数
  u16         itemno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第8引数
  u16         ball   = SCRCMD_GetVMWorkValue( core, work );  // コマンド第9引数

  //パラメータセット
  {
    prm.HeapID = heap_id;
    prm.MonsNo = monsno;
    prm.FormNo = formno;
    prm.Level = level;
    prm.ItemNo = itemno;
    prm.Tokusei = tokusei;
    if ( prm.Tokusei >= PTL_TOKUSEI_SPEC_MAX)
    {
      GF_ASSERT_MSG(0,"%d:tokusei error",prm.Tokusei);
      prm.Tokusei = PTL_TOKUSEI_SPEC_BOTH;
    }
    prm.SexSel = sex;
    if ( prm.SexSel >= PTL_SEX_SPEC_MAX)
    {
      GF_ASSERT_MSG(0,"%d:sex error",prm.SexSel);
      prm.SexSel = PTL_SEX_SPEC_UNKNOWN;
    }
    prm.RareSel = rare;
    if ( prm.RareSel >= PTL_RARE_SPEC_MAX)
    {
      GF_ASSERT_MSG(0,"%d:rare error",prm.RareSel);
      prm.RareSel = PTL_RARE_SPEC_BOTH;
    }
    prm.Ball = ball;
  }

  *ret_wk = AddPokeToBox(gdata, &prm);
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * タマゴを手持ちに追加　１８番道路でもらうイベント専用
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdAddTamagoToParty( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*    gdata = SCRCMD_WORK_GetGameData( work );
  HEAPID     heap_id = SCRCMD_WORK_GetHeapID( work );
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gdata );
  MYSTATUS*   status = GAMEDATA_GetMyStatus( gdata );
  u16*        ret_wk = SCRCMD_GetVMWork( core, work );       // コマンド第1引数
  u16         monsno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第2引数
  u16         formno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第3引数
  POKEMON_PARAM*  pp = NULL;

  // 手持ちがいっぱいなら追加しない
  if( PokeParty_GetPokeCountMax(party) <= PokeParty_GetPokeCount(party) )
  {
    *ret_wk = FALSE;
    return VMCMD_RESULT_CONTINUE;
  }

  // 追加するポケモンを作成
  pp = PP_Create( monsno, 1, PTL_SETUP_ID_AUTO, heap_id );
  PP_Put( pp, ID_PARA_form_no, formno );    // フォーム

  // 親の名前
  {
    MYSTATUS* myStatus;
    STRBUF* name;
    myStatus = GAMEDATA_GetMyStatus( gdata );
    name     = MyStatus_CreateNameString( myStatus, heap_id );
    PP_Put( pp, ID_PARA_oyaname, (u32)name );
    GFL_STR_DeleteBuffer( name );
  }

  // 孵化歩数
  {
    u32 monsno, formno, birth;
    POKEMON_PERSONAL_DATA* personal;
    monsno   = PP_Get( pp, ID_PARA_monsno, NULL );
    formno   = PP_Get( pp, ID_PARA_form_no, NULL );
    personal = POKE_PERSONAL_OpenHandle( monsno, formno, heap_id );
    birth    = POKE_PERSONAL_GetParam( personal, POKEPER_ID_egg_birth );
    POKE_PERSONAL_CloseHandle( personal );

    // タマゴの間は, なつき度を孵化カウンタとして利用する
    PP_Put( pp, ID_PARA_friend, birth );
  }

  // タマゴフラグ
  PP_Put( pp, ID_PARA_tamago_flag, TRUE );

  // ニックネーム ( タマゴ )
  {
    STRBUF* name;

    name = GFL_MSG_CreateString( GlobalMsg_PokeName, MONSNO_TAMAGO );
    PP_Put( pp, ID_PARA_nickname, (u32)name );
    GFL_STR_DeleteBuffer( name );
  } 

  PP_Renew( pp );

  // トレーナーメモ
  {
    POKE_MEMO_SetTrainerMemoPP( pp, POKE_MEMO_EGG_FIRST, status, POKE_MEMO_PERSON_TOREZYAHANTA, heap_id );
  }

  // 手持ちに追加
  PokeParty_Add( party, pp );

  GFL_HEAP_FreeMemory( pp );
  *ret_wk = TRUE;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * 追加ポケモンを作成
 * @param	gdata   ゲームデータポインタ
 * @param prm     追加ポケモンパラメータ
 * @retval POKEMON_PARAM
 *
 * @todo  ForUpper フォルム変更はPP_ChangeFormNoを使用したほうがいいのですが、時期を考えて対処を見送ります
 * @tod   変更はアッパーでお願いします　20100628 Saito
 */
//--------------------------------------------------------------
static POKEMON_PARAM* MakePokeParam( GAMEDATA *gdata, ADD_POKE_PRM *prm )
{
  u32 rnd;
  u32 id;
  POKEMON_PARAM*  pp = NULL;
  MYSTATUS*   status = GAMEDATA_GetMyStatus( gdata );
  //性別、レア、特性から個性乱数を選出して、セット
  {
    MYSTATUS *my = GAMEDATA_GetMyStatus( gdata );
    id = MyStatus_GetID(my);
    rnd = POKETOOL_CalcPersonalRandSpec( id, prm->MonsNo, prm->FormNo, prm->SexSel, prm->Tokusei, prm->RareSel );
  }
  // 追加するポケモンを作成
  pp = PP_CreateEx( prm->MonsNo, prm->Level, id, PTL_SETUP_ID_AUTO, rnd, prm->HeapID );
  PP_Put( pp, ID_PARA_form_no, prm->FormNo );    // フォーム  <<@todo  PP_ChangeFormNoを使用したほうが良いです。
  PP_Put( pp, ID_PARA_item, prm->ItemNo );       // 所持アイテム
  {
    //捕獲ボールセット
    int item_type = ITEM_GetParam( prm->Ball, ITEM_PRM_ITEM_TYPE, prm->HeapID );
    if (item_type == ITEMTYPE_BALL){
      //get_ballパラメータはBALLIDなので、アイテムナンバーから変換
      PP_Put( pp, ID_PARA_get_ball, ITEM_GetBallID(prm->Ball) );     // 捕獲ボールセット
    }
  }

  {
    PLAYER_WORK* player_wk = GAMEDATA_GetMyPlayerWork( gdata );
    POKE_MEMO_SetTrainerMemoPP( pp, POKE_MEMO_SET_CAPTURE, status,
        ZONEDATA_GetPlaceNameID( PLAYERWORK_getZoneID( player_wk ) ), prm->HeapID );
  }
  PP_Renew( pp );

  return pp;
}

//--------------------------------------------------------------
/**
 * ポケモンをボックスに追加
 * @param	gdata ゲームデータポインタ
 * @param prm   追加ポケモンパラメータ
 * @retval BOOL TRUE：追加成功
 */
//--------------------------------------------------------------
static BOOL AddPokeToBox(GAMEDATA* gdata, ADD_POKE_PRM *prm)
{
  POKEMON_PARAM*  pp = NULL;
  BOX_MANAGER *box = GAMEDATA_GetBoxManager(gdata);

  // ボックスがいっぱいなら追加しない
  {
    int max = BOX_POKESET_MAX;
    int now = BOXDAT_GetPokeExistCountTotal( box );
    if ( max<=now )
    {
      return FALSE;
    }
  }

  pp = MakePokeParam( gdata, prm );

  //ボックスに追加
  {
    BOOL rc;
    rc = BOXDAT_PutPokemon( box, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pp) );
    GF_ASSERT(rc);
  }

  //一応図鑑登録
  ZUKANSAVE_SetPokeGet(GAMEDATA_GetZukanSave( gdata ), pp);

  GFL_HEAP_FreeMemory( pp );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * ポケモンを手持ちに追加
 * @param	gdata ゲームデータポインタ
 * @param prm   追加ポケモンパラメータ
 * @retval BOOL TRUE：追加成功
 */
//--------------------------------------------------------------
static BOOL AddPokeToParty(GAMEDATA* gdata, ADD_POKE_PRM *prm)
{
  POKEMON_PARAM*  pp = NULL;
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gdata );

  // 手持ちがいっぱいなら追加しない
  if( PokeParty_GetPokeCountMax(party) <= PokeParty_GetPokeCount(party) )
  {
    return FALSE;
  }

  pp = MakePokeParam( gdata, prm );

  // 手持ちに追加
  PokeParty_Add( party, pp );

  //一応図鑑登録
  ZUKANSAVE_SetPokeGet(GAMEDATA_GetZukanSave( gdata ), pp);

  GFL_HEAP_FreeMemory( pp );
  return TRUE;

}
