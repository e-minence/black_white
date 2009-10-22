//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  スクリプトコマンド：ショップ関連
 * @file   scrcmd_shop.c
 * @author iwasawa
 * @date   2009.10.20
 *
 */
//////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/vm_cmd.h"
#include "scrcmd.h"
#include "app/pokelist.h"
#include "system/main.h"
#include "script_local.h"
#include "app/p_status.h"
#include "event_fieldmap_control.h"
#include "scrcmd_shop.h"
#include "fieldmap.h"
#include "script_def.h"

FS_EXTERN_OVERLAY(pokelist);

typedef struct _SHOP_EVENT_WORK{
  void* bag_param;
  u16*  ret_work;
}SHOP_EVENT_WORK;

//====================================================================
// ■プロトタイプ宣言
//====================================================================

//--------------------------------------------------------------------
/**
 * @brief ショップ購入イベント呼び出し 
 *
 * @param	core 仮想マシン制御構造体へのポインタ
 * @param wk   SCRCMD_WORKへのポインタ
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdCallShopProcBuy( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  u16               shop_id = SCRCMD_GetVMWorkValue( core, work );
  u16*              ret_work = SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK*     gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  if(shop_id == SCR_SHOPID_NULL){
    //変動ショップ呼び出し 
  }else{
    //固定ショップ呼び出し
  }

  /*
    イベント終了時に ret_work に終了モードを返してください
    SCR_PROC_RETMODE_EXIT 一発終了
    SCR_PROC_RETMODE_NORMAL 継続
  */
  return VMCMD_RESULT_SUSPEND;
}

