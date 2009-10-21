//======================================================================
/**
 * @file	scrcmd_pokemon.c
 * @brief	スクリプトコマンド：ポケモン関連
 * @date  2009.09.15
 * @author	tamada GAMEFREAK inc.
 *
 * @todo  ポケルスチェックの確認が行われていない
 * @todo  status_rcv.cの命名規則修正ができたら回復処理を呼び出す
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

#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"

#include "poke_tool/status_rcv.h" //PokeParty_RecoverAll

#include "savedata/box_savedata.h"

#include "event_name_input.h" // EVENT_PartyPokeNameInput

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static int GetPokeCount_NOT_EGG( const POKEPARTY* party );
static int GetPokeCount_BATTLE_ENABLE( const POKEPARTY* party );
static int GetPokeCount_ONLY_EGG( const POKEPARTY* party );
static int GetPokeCount_ONLY_DAME_EGG( const POKEPARTY* party );

//======================================================================
//  ポケモン関連
//======================================================================
//--------------------------------------------------------------
/**
 * 手持ちにポケルス状態のポケモンがいるか？のチェック
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckTemotiPokerus( VMHANDLE * core, void *wk )
{
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = FALSE;

  {
    GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
    POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );
    u32 max = PokeParty_GetPokeCount( party );
    int idx;

    for (idx = 0; idx < max; idx ++)
    {
      u32 result;
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, idx );
      result = PP_Get( pp, ID_PARA_pokerus, NULL );
      if (result != 0)
      {
        *ret_wk = TRUE;
        break;
      }
    }
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

  PokeParty_RecoverAll( party );

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
    GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
    POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );
    u32 max = PokeParty_GetPokeCount( party );
    POKEMON_PARAM * pp;
    u16 nowHP, maxHP;

    if (pos >= max)
    {
      GF_ASSERT_MSG(0, "Temoti Pos %d over max(%d)!!\n", pos, max);
      pos = 0;
    }
    pp = PokeParty_GetMemberPointer( party, pos );
    nowHP = PP_Get( pp, ID_PARA_hp, NULL );
    maxHP = PP_Get( pp, ID_PARA_hpmax, NULL );
    *ret_wk = FALSE;
    if (nowHP == maxHP)
    {
      *ret_wk = TRUE;
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
  GAMEDATA*     gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*    party = GAMEDATA_GetMyPokemon( gdata );
  int             max = PokeParty_GetPokeCountMax( party );
  u32     tamago_flag = 0;
  POKEMON_PARAM* param = NULL;

  // ポケモン指定に対する例外処理
  if( (pos < 0) || (max <= pos) )
  {
    *ret_wk = FALSE;
    return VMCMD_RESULT_CONTINUE;
  }

  // タマゴフラグ取得
  param       = PokeParty_GetMemberPointer( party, pos );
  tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );

  // 結果を格納
  if( tamago_flag == 0 ) *ret_wk = FALSE;
  else                   *ret_wk = TRUE;
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

  POKEPARTY*    party = GAMEDATA_GetMyPokemon( SCRCMD_WORK_GetGameData(work) );
  int             max = PokeParty_GetPokeCountMax( party );
  u8            friend = 0;
  POKEMON_PARAM* param = NULL;

  // ポケモン指定に対する例外処理
  if( (pos < 0) || (PokeParty_GetPokeCountMax(party) <= pos) )
  {
    *ret_wk = 0;
    return VMCMD_RESULT_CONTINUE;
  }

  // なつき度取得
  param       = PokeParty_GetMemberPointer( party, pos );
  if(PP_Get( param, ID_PARA_tamago_flag, NULL ) == TRUE){
    friend = PP_Get( param, ID_PARA_friend, NULL); 
  }

  // 結果を格納
  *ret_wk = friend;
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
  u8              mode = VMGetU8( core );  // 設定モード(0:セット,1:足す,2:引く)

  POKEPARTY*    party = GAMEDATA_GetMyPokemon( SCRCMD_WORK_GetGameData(work) );
  u8            friend = 0;
  POKEMON_PARAM* param = NULL;

  // ポケモン指定に対する例外処理
  if( (pos < 0) || (PokeParty_GetPokeCountMax(party) <= pos) )
  {
    return VMCMD_RESULT_CONTINUE;
  }

  // なつき度取得
  param       = PokeParty_GetMemberPointer( party, pos );
  if(PP_Get( param, ID_PARA_tamago_flag, NULL ) == TRUE)  //タマゴならなにもしない
  {
    return VMCMD_RESULT_CONTINUE;
  }
  friend = PP_Get( param, ID_PARA_friend, NULL); 

  switch(mode)
  {
  case 0: //セット
    if(value > 256){
      friend = 255;
    }else if(value < 0){
      friend = 0;
    }else{
      friend = value;
    }
    break;
  case 1: //加算
    if((friend + value) > 256) {
      friend = 255;
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
  PP_Put( param, ID_PARA_friend, friend); 
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
VMCMD_RESULT EvCmdGetPartyPokeMonsNo( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*    work = (SCRCMD_WORK*)wk;
  u16*          ret_wk = SCRCMD_GetVMWork( core, wk );       // 結果格納先ワーク
  u16              pos = SCRCMD_GetVMWorkValue( core, wk );  // 判定ポケモン指定
  GAMEDATA*      gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*     party = GAMEDATA_GetMyPokemon( gdata );
  POKEMON_PARAM* param = PokeParty_GetMemberPointer( party, pos );

  *ret_wk = (u16)PP_Get( param, ID_PARA_monsno, NULL );
  OBATA_Printf( "EvCmdGetPartyPokeMonsNo : %d\n", *ret_wk );
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
  GAMEDATA*      gdata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*     party = GAMEDATA_GetMyPokemon( gdata );
  POKEMON_PARAM* param = PokeParty_GetMemberPointer( party, pos );

  *ret_wk = (u16)PP_Get( param, ID_PARA_form_no, NULL );
  OBATA_Printf( "EvCmdGetPartyPokeFormNo : %d\n", *ret_wk );
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
    num = GetPokeCount_NOT_EGG( party );
    break;
  case POKECOUNT_MODE_BATTLE_ENABLE:
    num = GetPokeCount_BATTLE_ENABLE( party );
    break;
  case POKECOUNT_MODE_ONLY_EGG:
    num = GetPokeCount_ONLY_EGG( party );
    break;
  case POKECOUNT_MODE_ONLY_DAME_EGG:
    num = GetPokeCount_ONLY_DAME_EGG( party );
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
 * @brief ボックス内のポケモン数を取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdGetBoxPokeCount( VMHANDLE * core, void *wk )
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
  default:
    num = 0;
    break;
  }

  *ret_wk = (u16)num;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
// タマゴを除く手持ちポケモンの数を取得する
static int GetPokeCount_NOT_EGG( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // 全ポケモン数
  POKEMON_PARAM* param;
  u32 tamago_flag;

  // タマゴ以外のポケモン数をカウント
  for( i=0; i<max; i++ )
  {
    param       = PokeParty_GetMemberPointer( party, i );
    tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    if( tamago_flag != TRUE ) num++;
  }
  return num;
}

//--------------------------------------------------------------
// 戦える(タマゴと瀕死を除いた)ポケモン数を取得する
static int GetPokeCount_BATTLE_ENABLE( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // 全ポケモン数
  POKEMON_PARAM* param;
  u32 tamago_flag;
  u32 hp;

  // 戦えるポケモン数をカウント
  for( i=0; i<max; i++ )
  {
    param       = PokeParty_GetMemberPointer( party, i );
    tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    hp          = PP_Get( param, ID_PARA_hp, NULL );
    if( ( tamago_flag != TRUE ) && ( 0 < hp ) ) num++;
  }
  return num;
}

//--------------------------------------------------------------
// タマゴの数(駄目タマゴを除く)を取得する
static int GetPokeCount_ONLY_EGG( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // 全ポケモン数
  POKEMON_PARAM* param;
  u32 tamago_flag;
  u32 fusei_tamago_flag;

  // 駄目じゃないタマゴの数をカウント
  for( i=0; i<max; i++ )
  {
    param             = PokeParty_GetMemberPointer( party, i );
    tamago_flag       = PP_Get( param, ID_PARA_tamago_flag, NULL );
    fusei_tamago_flag = PP_Get( param, ID_PARA_fusei_tamago_flag, NULL );
    if( ( tamago_flag == TRUE ) && ( fusei_tamago_flag == FALSE ) ) num++;
  }
  return num;
}

//--------------------------------------------------------------
// 駄目タマゴの数を取得する
static int GetPokeCount_ONLY_DAME_EGG( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // 全ポケモン数
  POKEMON_PARAM* param;
  u32 fusei_tamago_flag;

  // 駄目タマゴの数をカウント
  for( i=0; i<max; i++ )
  {
    param             = PokeParty_GetMemberPointer( party, i );
    fusei_tamago_flag = PP_Get( param, ID_PARA_fusei_tamago_flag, NULL );
    if( fusei_tamago_flag == TRUE ) num++;
  }
  return num;
}

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
  GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
  POKEPARTY *party = GAMEDATA_GetMyPokemon( gamedata );
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 waza = SCRCMD_GetVMWorkValue( core, wk );
  u16 tno = SCRCMD_GetVMWorkValue( core, wk );
  
  *ret_wk = 0;
  pp = PokeParty_GetMemberPointer( party, tno );
  
  //たまごチェック
  if( PP_Get(pp,ID_PARA_tamago_flag,NULL) != 0 ){
    return VMCMD_RESULT_CONTINUE;
  }
  
  //技リストからチェック
  if( PP_Get(pp,ID_PARA_waza1,NULL) == waza ||
      PP_Get(pp,ID_PARA_waza2,NULL) == waza ||
      PP_Get(pp,ID_PARA_waza3,NULL) == waza ||
      PP_Get(pp,ID_PARA_waza4,NULL) == waza ){
    *ret_wk = 1;
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
    
    //たまごチェック
    if( PP_Get(pp,ID_PARA_tamago_flag,NULL) != 0 ){
      continue;
    }
    
    //技リストからチェック
    if( PP_Get(pp,ID_PARA_waza1,NULL) == waza ||
        PP_Get(pp,ID_PARA_waza2,NULL) == waza ||
        PP_Get(pp,ID_PARA_waza3,NULL) == waza ||
        PP_Get(pp,ID_PARA_waza4,NULL) == waza ){
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
extern VMCMD_RESULT EvCmdAddPokemonToParty( VMHANDLE *core, void *wk )
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
  u16        tokusei = SCRCMD_GetVMWorkValue( core, work );  // コマンド第4引数
  u16          level = SCRCMD_GetVMWorkValue( core, work );  // コマンド第5引数
  u16         itemno = SCRCMD_GetVMWorkValue( core, work );  // コマンド第6引数
  POKEMON_PARAM*  pp = NULL;

  // 手持ちがいっぱいなら追加しない
  if( PokeParty_GetPokeCountMax(party) <= PokeParty_GetPokeCount(party) )
  {
    *ret_wk = FALSE;
    return VMCMD_RESULT_CONTINUE;
  }

  // 追加するポケモンを作成
  pp = PP_Create( monsno, level, PTL_SETUP_ID_AUTO, heap_id );
  PP_Put( pp, ID_PARA_form_no, formno );    // フォーム
  PP_Put( pp, ID_PARA_speabino, tokusei );  // 特性
  PP_Put( pp, ID_PARA_item, itemno );       // 所持アイテム
  { // 親の名前
    STRBUF* buf = MyStatus_CreateNameString( status, heap_id );
    PP_Put( pp, ID_PARA_oyaname, (u32)buf );
    GFL_STR_DeleteBuffer( buf );
  }

  // 手持ちに追加
  PokeParty_Add( party, pp );

  GFL_HEAP_FreeMemory( pp );
  *ret_wk = TRUE;
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * 手持ちポケモンの名前入力画面を呼び出す
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdPartyPokeNameInput( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  u16*        ret_wk = SCRCMD_GetVMWork( core, work );       // コマンド第1引数
  u16          index = SCRCMD_GetVMWorkValue( core, work );  // コマンド第2引数

  // イベントを呼び出す
  SCRIPT_CallEvent( scw, EVENT_NameInput_PartyPoke(gsys, ret_wk, index) );
  return VMCMD_RESULT_SUSPEND;
}
