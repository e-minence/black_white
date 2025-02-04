//======================================================================
/**
 * @file  scrcmd_gym_init.c
 * @brief  スクリプトコマンド：ジム初期化関連
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "event_fieldmap_control.h"

#include "field_sound.h"

#include "scrcmd_gym.h"

#include "gym_init.h"
#include "gym_elec.h"
#include "gym_normal.h"

FS_EXTERN_OVERLAY(field_gym_init);


//--電気--
//--------------------------------------------------------------
/**
 * 電気ジム初期化
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymElec_Init( VMHANDLE *core, void *wk )
{
  u16 *evt1,*evt2;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  evt1 = SCRCMD_GetVMWork( core, work );
  evt2 = SCRCMD_GetVMWork( core, work );

  GFL_OVERLAY_Load( FS_OVERLAY_ID(field_gym_init) );		//オーバーレイロード
  GYM_INIT_Elec(gsys, *evt1, *evt2);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(field_gym_init));		//オーバーレイアンロード

  return VMCMD_RESULT_CONTINUE;
}

//--ノーマル--
//--------------------------------------------------------------
/**
 * ノーマルジム初期化
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymNormal_Init( VMHANDLE *core, void *wk )
{
  u16 opened;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  opened = SCRCMD_GetVMWorkValue( core, work );

  GFL_OVERLAY_Load( FS_OVERLAY_ID(field_gym_init) );		//オーバーレイロード
  GYM_INIT_Normal(gsys, opened);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(field_gym_init));		//オーバーレイアンロード

  return VMCMD_RESULT_CONTINUE;
}

//--アンチ--
//--------------------------------------------------------------
/**
 * アンチジム初期化
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymAnti_Init( VMHANDLE *core, void *wk )
{
  u16 curtain;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  curtain = SCRCMD_GetVMWorkValue( core, work );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(field_gym_init) );		//オーバーレイロード
  GYM_INIT_Anti(gsys, curtain);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(field_gym_init));		//オーバーレイアンロード

  return VMCMD_RESULT_CONTINUE;
}

//--虫--
//--------------------------------------------------------------
/**
 * 虫ジム初期化
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymInsect_Init( VMHANDLE *core, void *wk )
{
  u16 *sw[8];
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  sw[0] = SCRCMD_GetVMWork( core, work );
  sw[1] = SCRCMD_GetVMWork( core, work );
  sw[2] = SCRCMD_GetVMWork( core, work );
  sw[3] = SCRCMD_GetVMWork( core, work );
  sw[4] = SCRCMD_GetVMWork( core, work );
  sw[5] = SCRCMD_GetVMWork( core, work );
  sw[6] = SCRCMD_GetVMWork( core, work );
  sw[7] = SCRCMD_GetVMWork( core, work );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(field_gym_init) );		//オーバーレイロード
  GYM_INIT_Insect(gsys, sw);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(field_gym_init));		//オーバーレイアンロード

  return VMCMD_RESULT_CONTINUE;
}

//--ドラゴン--
//--------------------------------------------------------------
/**
 * ドラゴンジム初期化
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymDragon_Init( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(field_gym_init) );		//オーバーレイロード
  GYM_INIT_Dragon(gsys);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(field_gym_init));		//オーバーレイアンロード

  return VMCMD_RESULT_CONTINUE;
}
