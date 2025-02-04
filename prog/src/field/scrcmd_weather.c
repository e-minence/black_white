//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_weather.c
 *	@brief  天気スクリプトコマンド
 *	@author	tomoya takahashi
 *	@date		2010.02.10
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/pm_weather.h"

#include "script.h"
#include "script_def.h"
#include "script_local.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "fieldmap.h"

#include "scrcmd_weather.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  天気変更
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWeatherChange( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELDMAP_WORK* fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_WEATHER* weather = FIELDMAP_GetFieldWeather( fieldWork );
  u16 weather_no = SCRCMD_GetVMWorkValue( core, work );  // コマンド第1引数
  u16 is_notenvse = SCRCMD_GetVMWorkValue( core, work );  // コマンド第1引数

  // ゾーンの基本天気チェック
  if( weather_no == SCR_WEATHER_NO_ZONE_DEF ){
    weather_no = PM_WEATHER_GetZoneWeatherNo( gsys, FIELDMAP_GetZoneID( fieldWork ) );
  }

  if(is_notenvse){
    // 通常SE再生で天気を起動
    FIELD_WEATHER_ChangeNotEnvSe( weather, weather_no );
  }else{
    // 環境SE再生で天気を起動
    FIELD_WEATHER_Change( weather, weather_no );
  }
  GAMEDATA_SetWeatherNo( gdata, weather_no );

  return VMCMD_RESULT_CONTINUE;
}



