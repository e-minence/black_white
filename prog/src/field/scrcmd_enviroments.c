//======================================================================
/**
 * @file  scrcmd_enviroments.c
 * @brief スクリプトコマンド：システム管理データ関連
 * @date  2009.09.22
 * @author  tamada GAMEFREAK inc.
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
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_enviroments.h"

#include "system/main.h"
#include "system/rtc_tool.h"  //GFL_RTC_GetTimeZone
#include "savedata/save_tbl.h"
#include "savedata/dendou_save.h"
#include "savedata/mystatus.h"  //MyStatus_〜
#include "savedata/config.h" // CONFIG
#include "savedata/box_savedata.h"  //BOX_MANAGER
#include "savedata/trainercard_data.h" // TR_CARD_SV_PTR 
#include "savedata/save_control.h"
#include "savedata/zukan_savedata.h" 
#include "field/zonedata.h"   //ZONEDATA_GetMessageArcID
#include "field/map_matrix.h"   //MAP_MATRIX
#include "field/field_const.h"  //GRID_TO_FX32
#include "net_app/union/union_beacon_tool.h" // UnionView_xxxx
#include "map_replace.h"    //MAPREPLACE_ChangeFlag
#include "warpdata.h"   //WARPDATA_IsValidID

#include "../../../resource/fldmapdata/flagwork/flag_define.h"  //for SYS_FLAG_
#include "report.h" //REPORT_SAVE_TYPE_VAL
#include "savedata/misc.h"
#include "ev_time.h"


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ロムバージョンの取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetRomVersion( VMHANDLE *core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = GET_VERSION(); //include/pm_version.h
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief メッセージ表示モード（漢字・かな）の切り替え
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChangeLangID( VMHANDLE *core, void *wk )
{
  // 設定画面と食い違う現象が起きていたので、セーブデータを見るように
  // 修正しました。名木橋
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  SAVE_CONTROL_WORK * sv = GAMEDATA_GetSaveControlWork( gdata );
  CONFIG *config  = SaveData_GetConfig( sv );

  MOJIMODE id = CONFIG_GetMojiMode( config );
  if( id == MOJIMODE_HIRAGANA ){ //ひらがな
    id = MOJIMODE_KANJI;
  }else if( id == MOJIMODE_KANJI ){ //漢字
    id = MOJIMODE_HIRAGANA;
  }
  CONFIG_SetMojiMode( config, id );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief メッセージ表示モード（漢字・かな）の取得
 * @retval  VMCMD_RESULT_CONTINUE
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetLangID( VMHANDLE *core, void *wk )
{
  // 設定画面と食い違う現象が起きていたので、セーブデータを見るように
  // 修正しました。名木橋
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  SAVE_CONTROL_WORK * sv = GAMEDATA_GetSaveControlWork( gdata );
  CONFIG *config  = SaveData_GetConfig( sv );

  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = CONFIG_GetMojiMode( config );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 乱数の取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetRand( VMHANDLE *core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 limit = SCRCMD_GetVMWorkValue( core, wk );
  *ret_wk = GFUser_GetPublicRand( limit );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 現在のスクリプトでのメッセージアーカイブIDを取得
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetNowMsgArcID( VMHANDLE * core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = SCRCMD_WORK_GetMsgDataID( wk );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief トレーナーカードランクの取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetTrainerCardRank( VMHANDLE *core, void *wk )
{
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );

  *ret_wk = TRAINERCARD_GetCardRank( gdata );
  return VMCMD_RESULT_CONTINUE;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief 時間帯の取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetTimeZone( VMHANDLE *core, void *wk )
{
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = EVTIME_GetSeasonTimeZone( gamedata );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 曜日の取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetWeek( VMHANDLE * core, void *wk )
{
  RTCDate now_date;
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );

  *ret_wk = EVTIME_GetWeek( gamedata );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 日付の取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param  wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetDate( VMHANDLE * core, void *wk )
{
  RTCDate now_date;
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  u16 *ret_month = SCRCMD_GetVMWork( core, wk ); // 第一引数
  u16 *ret_day   = SCRCMD_GetVMWork( core, wk ); // 第二引数

  *ret_month = EVTIME_GetMonth( gamedata );
  *ret_day   = EVTIME_GetDay( gamedata );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 日付の取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param  wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetTime( VMHANDLE * core, void *wk )
{
  RTCDate now_date;
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  u16 *      ret_hour = SCRCMD_GetVMWork( core, wk ); // 第一引数
  u16 *    ret_minute = SCRCMD_GetVMWork( core, wk ); // 第二引数

  *ret_hour   = EVTIME_GetHour( gamedata );
  *ret_minute = EVTIME_GetMinute( gamedata );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 現在の季節を取得する
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetSeasonID( VMHANDLE *core, void * wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  *ret_wk = GAMEDATA_GetSeasonID( gdata );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 自分の誕生日の取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBirthDay( VMHANDLE * core, void*wk )
{
  u16 *month_wk = SCRCMD_GetVMWork( core, wk );
  u16 *day_wk = SCRCMD_GetVMWork( core, wk );
#if (defined(SDK_TWL))
  OSOwnerInfoEx ownerInfo;
  OS_GetOwnerInfoEx( &ownerInfo );
#else
  OSOwnerInfo ownerInfo;
  OS_GetOwnerInfo( &ownerInfo );
#endif
  *month_wk = ownerInfo.birthday.month;
  *day_wk   = ownerInfo.birthday.day;

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 自分の性別を取得する
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetMySex( VMHANDLE *core, void * wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
  *ret_wk = MyStatus_GetMySex( &player->mystatus );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * バッジフラグの取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBadgeFlag( VMHANDLE * core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 badge_id = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );

  *ret_wk = MISC_GetBadgeFlag( GAMEDATA_GetMiscWork(gdata), badge_id );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * バッジフラグのセット
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetBadgeFlag( VMHANDLE * core, void *wk )
{
  u16 badge_id = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );

  MISC_SetBadgeFlag( GAMEDATA_GetMiscWork(gdata), badge_id );

  // バッジ取得日付の保存
  {
    RTCDate date;
    GFL_RTC_GetDate(&date);
    TRCSave_SetBadgeDate( GAMEDATA_GetTrainerCardPtr(gdata), badge_id, 
                          date.year, date.month, date.day );
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * バッジフラグ取得数のカウント
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBadgeCount( VMHANDLE * core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );

  *ret_wk = MISC_GetBadgeCount( GAMEDATA_GetMiscWork(gdata) );
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * セーブデータの状態を取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetSaveDataStatus( VMHANDLE * core, void *wk )
{
  GAMEDATA*        gdata = SCRCMD_WORK_GetGameData( wk );
  SAVE_CONTROL_WORK* scw = GAMEDATA_GetSaveControlWork( gdata );
  u16*         ret_exist = SCRCMD_GetVMWork( core, wk );  // コマンド第1引数
  u16*           ret_new = SCRCMD_GetVMWork( core, wk );  // コマンド第2引数
  u16*        ret_volume = SCRCMD_GetVMWork( core, wk );  // コマンド第3引数
  u32         total_size = 0; // セーブ全体のサイズ
  u32        actual_size = 0; // セーブされる実サイズ

  // セーブデータが存在するかどうか
  *ret_exist = SaveData_GetExistFlag( scw );

  // プレイ中のデータが新規データかどうか
  *ret_new   = SaveControl_NewDataFlagGet( scw );

  SaveControl_GetActualSize( scw, &actual_size, &total_size );
  if ( actual_size * REPORT_SAVE_TYPE_VAL < total_size )
  {
    *ret_volume = 0;  //ふつう
  } else {
    *ret_volume = 1;  //たくさん
  }

  return VMCMD_RESULT_CONTINUE;
} 

//======================================================================
//
//
//  マップ接続関連など
//  
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ゾーンIDを取得する
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZoneID( VMHANDLE *core, void * wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = SCRCMD_WORK_GetZoneID( wk );
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief ワープIDのセット
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetWarpID( VMHANDLE * core, void *wk )
{
  GAMEDATA*        gamedata = SCRCMD_WORK_GetGameData( wk );
  u16 warp_id = SCRCMD_GetVMWorkValue( core, wk );

  if ( WARPDATA_IsValidID( warp_id ) == TRUE ) {
    GAMEDATA_SetWarpID( gamedata, warp_id );
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 特殊接続先の設定：現在位置を記憶
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetSpLocationHere( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA*        gamedata = SCRCMD_WORK_GetGameData( wk );
  LOCATION spLoc;
  PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gamedata );
  const VecFx32 * vec = PLAYERWORK_getPosition( player );
  LOCATION_SetDirect( &spLoc, PLAYERWORK_getZoneID( player ), 
      PLAYERWORK_getDirection_Type( player ), vec->x, vec->y, vec->z );
  GAMEDATA_SetSpecialLocation( gamedata, &spLoc );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 特殊接続先の設定：直接指定
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetSpLocationDirect( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA*        gamedata = SCRCMD_WORK_GetGameData( wk );
  LOCATION spLoc;

  u16 zone_id = SCRCMD_GetVMWorkValue( core, work );
  u16 dir = SCRCMD_GetVMWorkValue( core, work );
  u16 gx = SCRCMD_GetVMWorkValue( core, work );
  u16 gy = SCRCMD_GetVMWorkValue( core, work );
  u16 gz = SCRCMD_GetVMWorkValue( core, work );

  LOCATION_SetDirect( &spLoc, zone_id, dir, 
      GRID_TO_FX32( gx ), GRID_TO_FX32( gy ), GRID_TO_FX32( gz ) );
  GAMEDATA_SetSpecialLocation( gamedata, &spLoc );
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief マップ置き換えリクエスト
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChangeMapReplaceFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = wk;
  GAMESYS_WORK * gamesys = SCRCMD_WORK_GetGameSysWork( wk );
  GAMEDATA*        gamedata = SCRCMD_WORK_GetGameData( wk );

  u16 id = SCRCMD_GetVMWorkValue( core, work );
  u16 on_off = SCRCMD_GetVMWorkValue( core, work );
  u16 direct = SCRCMD_GetVMWorkValue( core, work );

  MAPREPLACE_ChangeFlag( gamedata, id, on_off );
  if ( direct )
  {
    MAP_MATRIX * pMat = GAMEDATA_GetMapMatrix( gamedata );
    PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gamedata );
    u16 zone_id = PLAYERWORK_getZoneID( player );
    u16 matID = ZONEDATA_GetMatrixID( zone_id );
    HEAPID heapID = SCRCMD_WORK_GetHeapID( work );

    MAP_MATRIX_Init( pMat, matID, zone_id, heapID );
    MAP_MATRIX_CheckReplace( pMat, gamesys, heapID );
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief マップ置き換え状態の取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetMapReplaceFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA*        gamedata = SCRCMD_WORK_GetGameData( wk );

  u16       id = SCRCMD_GetVMWorkValue( core, work );
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );

  *ret_wk = MAPREPLACE_GetFlag( gamedata, id );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ずかんフラグのセット 
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 * @note
 * ずかんフラグを正確にセットするためにはPOKEMON_PARAMが必要だが、
 * 存在しないのでコマンド内部で生成している。
 * POKEMON_PARAM依存のものをスクリプトが指定しないような仕組みが必要。
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetZukanFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA* gamedata = SCRCMD_WORK_GetGameData( work );
  ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  HEAPID     heap_id = SCRCMD_WORK_GetHeapID( work );
  u16 set_mode = SCRCMD_GetVMWorkValue( core, work ); // 引数1
  u16 monsno = SCRCMD_GetVMWorkValue( core, work ); // 引数2

  POKEMON_PARAM * pp = PP_Create( monsno, 1, PTL_SETUP_ID_NOT_RARE, heap_id );
  switch( set_mode )
  {
  case ZUKANCTRL_MODE_SEE:  // 見た
    ZUKANSAVE_SetPokeSee( zukan, pp );
    break;
  case ZUKANCTRL_MODE_GET:  // 捕まえた
    GF_ASSERT(0); //ポケモンゲットには対応しない。システムで行うべき。
    break;
  default:
    OS_Printf( "==============================================================\n" );
    OS_Printf( "スクリプト: 図鑑セットコマンドに指定する引数に誤りがあります。\n" );
    OS_Printf( "==============================================================\n" );
    break;
  } 
  GFL_HEAP_FreeMemory( pp );
  
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief   図鑑フラグの取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZukanFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA* gamedata = SCRCMD_WORK_GetGameData( work );
  const ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  u16 get_mode = SCRCMD_GetVMWorkValue( core, work ); // 引数1
  u16 monsno = SCRCMD_GetVMWorkValue( core, work ); // 引数2
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );

  switch( get_mode )
  {
  case ZUKANCTRL_MODE_SEE:  // 見た
    *ret_wk = ZUKANSAVE_GetPokeSeeFlag( zukan, monsno );
    break;
  case ZUKANCTRL_MODE_GET:  // 捕まえた
    *ret_wk = ZUKANSAVE_GetPokeGetFlag( zukan, monsno );
    break;
  default:
    GF_ASSERT(0);
    *ret_wk = FALSE;
    break;
  } 
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief 図鑑フラグのカウント
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZukanCount( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA* gamedata = SCRCMD_WORK_GetGameData( work );
  const ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  u16 get_mode = SCRCMD_GetVMWorkValue( core, work );
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );

  switch( get_mode )
  {
  case ZUKANCTRL_MODE_SEE:  // 見た数
    *ret_wk = ZUKANSAVE_GetPokeSeeCount( zukan );
    break;
  case ZUKANCTRL_MODE_GET:  // 捕まえた数
    *ret_wk = ZUKANSAVE_GetPokeGetCount( zukan );
    break;
  default:
    *ret_wk = 0;
    OS_Printf( "================================================================\n" );
    OS_Printf( "スクリプト: 図鑑カウントコマンドに指定する引数に誤りがあります。\n" );
    OS_Printf( "================================================================\n" );
    break;
  } 
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ずかんの機能操作：全国図鑑フラグのセット
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetZukanZenkokuFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*      work = wk;
  GAMEDATA*     gamedata = SCRCMD_WORK_GetGameData( work );
  ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  ZUKANSAVE_SetZenkokuZukanFlag( zukan );
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief ずかんの機能操作：全国図鑑フラグの取得
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZukanZenkokuFlag( VMHANDLE * core, void *wk )
{
  u16 *           ret_wk = SCRCMD_GetVMWork( core, wk );
  SCRCMD_WORK*      work = wk;
  GAMEDATA*     gamedata = SCRCMD_WORK_GetGameData( work );
  ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  *ret_wk = ZUKANSAVE_GetZenkokuZukanFlag( zukan );
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief ずかんの機能操作：フォルム詳細画面追加
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetZukanGraphicFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*      work = wk;
  GAMEDATA*     gamedata = SCRCMD_WORK_GetGameData( work );
  ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  ZUKANSAVE_SetGraphicVersionUpFlag( zukan );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ワイヤレスセーブモードを取得する
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param  wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetWirelessSaveMode( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork( gdata );
  CONFIG*          config = SaveData_GetConfig( save );
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );  // コマンド第一引数
  WIRELESSSAVE_MODE mode;

  // ワイヤレスセーブモード取得
  mode = CONFIG_GetWirelessSaveMode( config );
  OBATA_Printf( "=====================\n" );
  OBATA_Printf( "wirelesssavemode = %d\n", mode );
  OBATA_Printf( "=====================\n" );

  // 対応する定数を返す
  switch( mode )
  {
  case WIRELESSSAVE_OFF:  *ret_wk = WIRELESS_SAVEMODE_OFF;  break;
  case WIRELESSSAVE_ON:   *ret_wk = WIRELESS_SAVEMODE_ON;   break;
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief シューズゲット
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param  wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetShoes( VMHANDLE *core, void *wk )
{
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  EVENTWORK_SetEventFlag( evwork, SYS_FLAG_RUNNINGSHOES );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ボックス追加壁紙をセットする
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBoxWallPaper( VMHANDLE * core, void * wk )
{
  u16 flag = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  BOX_MANAGER * box = GAMEDATA_GetBoxManager( gamedata );
  if ( flag == SCR_BOX_EX_WALLPAPER1 || flag == SCR_BOX_EX_WALLPAPER2 )
  {
    BOXDAT_SetExWallPaperFlag( box, flag );
  }
  else
  {
    GF_ASSERT( 0 );
  }
  return VMCMD_RESULT_CONTINUE;
}
SDK_COMPILER_ASSERT( SCR_BOX_EX_WALLPAPER1 == BOX_EX_WALLPAPER_SET_FLAG_1 );
SDK_COMPILER_ASSERT( SCR_BOX_EX_WALLPAPER2 == BOX_EX_WALLPAPER_SET_FLAG_2 );



//--------------------------------------------------------------
/**
 * @brief トレーナータイプが変更されているかどうかをチェックする.
 *        変更されていない場合, 
 *        → 変更済みに設定し, 
 *        → デフォルトで設定されたトレーナータイプを取得する.
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param  wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPlayerViewChange( VMHANDLE * core, void * wk ) {
  SCRCMD_WORK* work       = (SCRCMD_WORK*)wk;
  u16*         retWk_disp = SCRCMD_GetVMWork( core, work ); // 第一引数: 表示されているかどうかの格納先
  u16*         retWk_type = SCRCMD_GetVMWork( core, work ); // 第二引数: 自身のトレーナータイプの格納先
  GAMEDATA*    gameData   = SCRCMD_WORK_GetGameData( work );
  PLAYER_WORK* player     = GAMEDATA_GetMyPlayerWork( gameData );
  MYSTATUS*    mystatus   = &( player->mystatus );
  SAVE_CONTROL_WORK* saveWork   = GAMEDATA_GetSaveControlWork( gameData );
  TR_CARD_SV_PTR     trCardSave = TRCSave_GetSaveDataPtr( saveWork );
  u8 unionViewIdx;

  // 自身のトレーナータイプを取得
  unionViewIdx = MyStatus_GetTrainerView( mystatus );
  *retWk_type  = UnionView_GetTrType( unionViewIdx );

  // 表示されているかどうかを取得
  *retWk_disp = TRCSave_GetTrainerViewChange( trCardSave );

  // 表示されている状態に変更
  TRCSave_SetTrainerViewChange( trCardSave );
  
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//======================================================================
SDK_COMPILER_ASSERT( SCR_STARTMENU_FLAG_HUSHIGI == MISC_STARTMENU_TYPE_HUSHIGI );
SDK_COMPILER_ASSERT( SCR_STARTMENU_FLAG_BATTLE == MISC_STARTMENU_TYPE_BATTLE );
SDK_COMPILER_ASSERT( SCR_STARTMENU_FLAG_GAMESYNC == MISC_STARTMENU_TYPE_GAMESYNC );
SDK_COMPILER_ASSERT( SCR_STARTMENU_FLAG_MACHINE == MISC_STARTMENU_TYPE_MACHINE );

enum { MISC_STARTMENU_TYPE_MAX = MISC_STARTMENU_TYPE_MACHINE + 1 };

//--------------------------------------------------------------
/**
 * @brief タイトルメニュー項目の許可フラグ状態取得
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetStartMenuFlag( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK* work       = (SCRCMD_WORK*)wk;
  u16 flag = SCRCMD_GetVMWorkValue( core, wk );
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( work );
  MISC * misc = GAMEDATA_GetMiscWork( gamedata );

  GF_ASSERT( flag < MISC_STARTMENU_TYPE_MAX );
  *ret_wk = ( (MISC_GetStartMenuFlag( misc, flag ) & MISC_STARTMENU_FLAG_OPEN )  != 0 );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief タイトルメニュー項目の許可フラグON
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetStartMenuFlag( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK* work       = (SCRCMD_WORK*)wk;
  u16 flag = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( work );
  MISC * misc = GAMEDATA_GetMiscWork( gamedata );

  GF_ASSERT( flag < MISC_STARTMENU_TYPE_MAX );
  MISC_SetStartMenuFlag( misc, flag, MISC_STARTMENU_FLAG_OPEN );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief タイトルメニュー項目のＮＥＷラベルが表示済みかどうか
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChkStartMenuViewFlag( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK* work       = (SCRCMD_WORK*)wk;
  u16 flag = SCRCMD_GetVMWorkValue( core, wk );
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( work );
  MISC * misc = GAMEDATA_GetMiscWork( gamedata );

  GF_ASSERT( flag < MISC_STARTMENU_TYPE_MAX );
  if ( (MISC_GetStartMenuFlag( misc, flag ) == (MISC_STARTMENU_FLAG_OPEN|MISC_STARTMENU_FLAG_VIEW) ) )
  {
    *ret_wk = TRUE;
  }
  else *ret_wk = FALSE;

  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief「殿堂入り」データが存在するかどうかをチェックする
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChkDendouData( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK* work     = (SCRCMD_WORK*)wk;
  GAMEDATA*    gameData = SCRCMD_WORK_GetGameData( work );
  EVENTWORK*   evwork   = GAMEDATA_GetEventWork( gameData );
  SAVE_CONTROL_WORK* save;
  LOAD_RESULT result;
  DENDOU_SAVEDATA* dendouData;
  u16* ret_wk; 

  // 引数を取得
  ret_wk = SCRCMD_GetVMWork( core, wk ); // 第一引数: 戻り値の格納先

  // チャンピオンに勝っている
  if( EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_CHAMPION_WIN ) ) { 

    // 外部データをロード
    save = GAMEDATA_GetSaveControlWork( gameData );
    result = SaveControl_Extra_Load( save, SAVE_EXTRA_ID_DENDOU, HEAPID_PROC );

    // 読み込み結果を返す
    switch( result ) {
    case LOAD_RESULT_NULL:
      *ret_wk = SCR_DENDOU_DATA_NULL;
      break;
    case LOAD_RESULT_OK:
    case LOAD_RESULT_NG:
      dendouData = 
        SaveControl_Extra_DataPtrGet( save, SAVE_EXTRA_ID_DENDOU, EXGMDATA_ID_DENDOU );
      // レコードが０件
      if( DendouData_GetRecordCount( dendouData ) == 0 ) {
        *ret_wk = SCR_DENDOU_DATA_NULL;
      }
      else {
        *ret_wk = SCR_DENDOU_DATA_OK;
      }
      break;
    default:
      *ret_wk = SCR_DENDOU_DATA_NG;
      break;
    }

    // 外部データを解放
    SaveControl_Extra_Unload( save, SAVE_EXTRA_ID_DENDOU );
  }
  // チャンピオンに勝っていない
  else {
    // データは存在しない
    *ret_wk = SCR_DENDOU_DATA_NULL;
  }

  return VMCMD_RESULT_CONTINUE;
}
