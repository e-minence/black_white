//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  スクリプトコマンド：Wi-Fi関連(フィールド常駐)
 * @file   scrcmd_wifi.c
 * @author iwasawa
 * @date   2009.10.20
 *
 */
//////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/vm_cmd.h"
#include "scrcmd.h"
#include "scrcmd_sodateya.h"
#include "app/pokelist.h"
#include "system/main.h"
#include "script_local.h"
#include "app/p_status.h"
#include "event_fieldmap_control.h"
#include "savedata/sodateya_work.h"
#include "fieldmap.h"

#include "scrcmd_wifi.h"

#include "event_wificlub.h"
#include "savedata/wifilist.h"

//====================================================================
// ■プロトタイプ宣言
//====================================================================

//--------------------------------------------------------------------
/**
 * @brief    GSIDが正しいかをチェックする
 *
 * @param	core 仮想マシン制御構造体へのポインタ
 * @param wk   SCRCMD_WORKへのポインタ
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdWifiCheckMyGSID( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  u16*              ret_wk = SCRCMD_GetVMWork( core, work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  WIFI_LIST* wifi_save = SaveData_GetWifiListData( GAMEDATA_GetSaveControlWork( gdata ) );

  *ret_wk = WifiList_CheckMyGSID( wifi_save );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief   友だち手帳に登録されている人数を返す 
 *
 * @param	core 仮想マシン制御構造体へのポインタ
 * @param wk   SCRCMD_WORKへのポインタ
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdWifiGetFriendNum( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  u16*              ret_wk = SCRCMD_GetVMWork( core, work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  WIFI_LIST* wifi_save = SaveData_GetWifiListData( GAMEDATA_GetSaveControlWork( gdata ) );

  *ret_wk = WifiList_GetFriendDataNum( wifi_save );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief   Wi-Fiクラブイベントを呼び出す
 *
 * @param	core 仮想マシン制御構造体へのポインタ
 * @param wk   SCRCMD_WORKへのポインタ
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdWifiClubEventCall( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_WiFiClub( gsys, GAMESYSTEM_GetFieldMapWork(gsys)));
  
	return VMCMD_RESULT_SUSPEND;		///<コマンド実行を中断して制御を返す
}

