//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_rebattle_trainer.c
 *	@brief  再戦トレーナー用スクリプト
 *	@author	tomoya takahashi
 *	@date		2010.03.19
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "arc/arc_def.h"
#include "arc/fieldmap/rebattle_trainer.naix"

#include "item/itemsym.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "script_trainer.h"


#include "fieldmap.h"

#include "scrcmd_rebattle_trainer.h"

#include "rebattle_trainer.h"

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
//-------------------------------------
///	再戦トレーナー情報
//=====================================
typedef struct {
  u16 id;
  u16 badge_num;
  u16 code;
  u16 trainer_id;
} REBATTLE_TRAINER_DATA;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static u32 ReBattle_SearchData( const REBATTLE_TRAINER_DATA* cp_data, u32 id, u32 badge_num );

//----------------------------------------------------------------------------
/**
 *	@brief  再戦トレーナー情報を確保
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerStart( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  HEAPID heapID = FIELDMAP_GetHeapID( fparam->fieldMap );
  void* p_data;

  p_data = GFL_ARC_UTIL_Load( ARCID_REBATTLE_TRAINER, 0, FALSE, GFL_HEAP_LOWID(heapID) );

  GF_ASSERT( SCRIPT_GetReBattleTrainerData(sc) == NULL );
  SCRIPT_SetReBattleTrainerData( sc, p_data );

  TOMOYA_Printf( "再戦トレーナー情報を確保\n" );

	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  再戦トレーナー情報を破棄
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerEnd( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  void* p_data;

  p_data = SCRIPT_GetReBattleTrainerData(sc);
  if( p_data ){
    GFL_HEAP_FreeMemory( p_data );
    SCRIPT_SetReBattleTrainerData( sc, NULL );

    TOMOYA_Printf( "再戦トレーナー情報を破棄\n" );
  }

	return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  再戦トレーナー　トレーナーID設定
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerSetUpTrID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  u16 rebattle_id = SCRCMD_GetVMWorkValue( core, work );
  u16 objid = SCRCMD_GetVMWorkValue( core, work );
  u16 badge = SCRCMD_GetVMWorkValue( core, work );
  u16 index;
  MMDL* mmdl;
  const REBATTLE_TRAINER_DATA* cp_data = SCRIPT_GetReBattleTrainerData(sc);

  GF_ASSERT( cp_data );

  index = ReBattle_SearchData( cp_data, rebattle_id, badge );

  mmdl = MMDLSYS_SearchOBJID( mmdlsys, objid );
  GF_ASSERT( mmdl != NULL && "SCRCMD GET MOVECODE NON OBJ" );


  TOMOYA_Printf( "設定情報　TrainerID[%d]\n", cp_data[index].trainer_id );

  // トレーナーIDの視線スクリプト設定
  MMDL_SetEventID( mmdl, SCRIPT_GetTrainerScriptID_ByTrainerID( cp_data[index].trainer_id ) );

	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  再戦トレーナー　ランダムトレーナーの設定
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerSetUpRndTr( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  MMDLSYS *mmdlsys = SCRCMD_WORK_GetMMdlSys( work );
  MMDL *mmdl0;
  MMDL *mmdl1;
  MMDL *mmdl2;
  u16 objid0 = SCRCMD_GetVMWorkValue( core, work );
  u16 objid1 = SCRCMD_GetVMWorkValue( core, work );
  u16 objid2 = SCRCMD_GetVMWorkValue( core, work );
  u16 index0;
  u16 index1;
  u16 index2;
  u16 random_max;
  int i;
  const REBATTLE_TRAINER_DATA* cp_data = SCRIPT_GetReBattleTrainerData(sc);

  GF_ASSERT( cp_data );

  
  random_max = RB_DATA_TBL_MAX - RB_RANDOM_START;


  mmdl0 = MMDLSYS_SearchOBJID( mmdlsys, objid0 );
  GF_ASSERT( mmdl0 != NULL && "SCRCMD GET MOVECODE NON OBJ" );
  mmdl1 = MMDLSYS_SearchOBJID( mmdlsys, objid1 );
  GF_ASSERT( mmdl1 != NULL && "SCRCMD GET MOVECODE NON OBJ" );
  mmdl2 = MMDLSYS_SearchOBJID( mmdlsys, objid2 );
  GF_ASSERT( mmdl2 != NULL && "SCRCMD GET MOVECODE NON OBJ" );
  
  // 3つ抽選
  index0 = GFUser_GetPublicRand( random_max );
  index1 = GFUser_GetPublicRand( random_max );
  index2 = GFUser_GetPublicRand( random_max );

  // かぶらないように
  if( index0 == index1 ){
    index1 = (index0 + 1) % random_max;
  }
  if( index0 == index2 ){
    index2 = (index0 + 2) % random_max;
  }
  if( index1 == index2 ){
    index2 = (index1 + 1) % random_max;
  }

  TOMOYA_Printf( "設定情報 0　OBJCode[%d] TrainerID[%d]\n", cp_data[ RB_RANDOM_START+index0 ].code, cp_data[ RB_RANDOM_START+index0 ].trainer_id );
  TOMOYA_Printf( "設定情報 1　OBJCode[%d] TrainerID[%d]\n", cp_data[ RB_RANDOM_START+index1 ].code, cp_data[ RB_RANDOM_START+index1 ].trainer_id );
  TOMOYA_Printf( "設定情報 2　OBJCode[%d] TrainerID[%d]\n", cp_data[ RB_RANDOM_START+index2 ].code, cp_data[ RB_RANDOM_START+index2 ].trainer_id );

  // 見た目設定
  MMDL_ChangeOBJCode( mmdl0, cp_data[ RB_RANDOM_START+index0 ].code );
  MMDL_ChangeOBJCode( mmdl1, cp_data[ RB_RANDOM_START+index1 ].code );
  MMDL_ChangeOBJCode( mmdl2, cp_data[ RB_RANDOM_START+index2 ].code );
  
  // トレーナーIDを設定
  MMDL_SetEventID( mmdl0, SCRIPT_GetTrainerScriptID_ByTrainerID( cp_data[ RB_RANDOM_START+index0 ].trainer_id ) );
  MMDL_SetEventID( mmdl1, SCRIPT_GetTrainerScriptID_ByTrainerID( cp_data[ RB_RANDOM_START+index1 ].trainer_id ) );
  MMDL_SetEventID( mmdl2, SCRIPT_GetTrainerScriptID_ByTrainerID( cp_data[ RB_RANDOM_START+index2 ].trainer_id ) );

	return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  全トレーナーIDの再戦フラグをクリア
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdReBattleTrainerClearTrEventFlag( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK * gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  int i;
  const REBATTLE_TRAINER_DATA* cp_data = SCRIPT_GetReBattleTrainerData(sc);

  GF_ASSERT( cp_data );


  // すべてクリア
  for( i=0; i<RB_DATA_TBL_MAX; i++ ){
    SCRIPT_ResetEventFlagTrainer( evwork, cp_data[i].trainer_id );
  }
  
	return VMCMD_RESULT_CONTINUE;
}





//-----------------------------------------------------------------------------
/**
 *      private関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  再戦データの検索
 *
 *	@param	cp_data     データ
 *	@param	id          ID
 *	@param	badge_num   バッジ数
 *
 *	@return インデックス
 */
//-----------------------------------------------------------------------------
static u32 ReBattle_SearchData( const REBATTLE_TRAINER_DATA* cp_data, u32 id, u32 badge_num )
{
  int i;

  for( i=0; i<RB_DATA_TBL_MAX; i++ ){

    if( (cp_data[i].id == id) && 
        (cp_data[i].badge_num == badge_num) ){
      return i;
    }
  }

  GF_ASSERT_MSG(0, "再戦データなし");

  return 0;
}



