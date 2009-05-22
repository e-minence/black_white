//======================================================================
/**
 * @file	field_player.h
 * @date	2008.9.29
 * @brief	フィールドプレイヤー制御
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "field/fieldmap_proc.h"
#include "fldmmdl.h"
#include "field_g3d_mapper.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
/// PLAYER_MOVE_VALUE
//--------------------------------------------------------------
typedef enum
{
  PLAYER_MOVE_VALUE_STOP, ///<停止状態
  PLAYER_MOVE_VALUE_WALK, ///<移動中
  PLAYER_MOVE_VALUE_TURN, ///<振り向き中
}PLAYER_MOVE_VALUE;

//--------------------------------------------------------------
/// PLAYER_MOVE_STATE
//--------------------------------------------------------------
typedef enum
{
  PLAYER_MOVE_STATE_OFF,    ///<動作なし
  PLAYER_MOVE_STATE_START,  ///<動作開始
  PLAYER_MOVE_STATE_ON,     ///<動作中
  PLAYER_MOVE_STATE_END,    ///<動作終了
}PLAYER_MOVE_STATE;

//======================================================================
//	struct
//======================================================================
///FIELD_PLAYER
typedef struct _FIELD_PLAYER FIELD_PLAYER;

//======================================================================
//	extern
//======================================================================
//作成、削除、更新
extern FIELD_PLAYER * FIELD_PLAYER_Create(
		FIELDMAP_WORK *fieldWork, const VecFx32 *pos, HEAPID heapID );
extern void FIELD_PLAYER_Delete( FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_Update( FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_UpdateMoveStatus( FIELD_PLAYER *fld_player );

//参照、設定
extern void FIELD_PLAYER_GetPos(
		const FIELD_PLAYER *fld_player, VecFx32 *pos );
extern void FIELD_PLAYER_SetPos(
		FIELD_PLAYER *fld_player, const VecFx32 *pos );
extern u16 FIELD_PLAYER_GetDir( const FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_SetDir( FIELD_PLAYER *fld_player, u16 dir );
extern FIELDMAP_WORK * FIELD_PLAYER_GetFieldMapWork(
		FIELD_PLAYER *fld_player );
extern FLDMMDL * FIELD_PLAYER_GetFldMMdl( FIELD_PLAYER *fld_player );
extern FLDMAPPER_GRIDINFODATA * FIELD_PLAYER_GetGridInfoData(
		FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_SetMoveValue(
    FIELD_PLAYER *fld_player, PLAYER_MOVE_VALUE val );
extern PLAYER_MOVE_VALUE FIELD_PLAYER_GetMoveValue(
    const FIELD_PLAYER *fld_player );
extern PLAYER_MOVE_STATE FIELD_PLAYER_GetMoveState(
    const FIELD_PLAYER *fld_player );

//ツール
extern void FIELD_PLAYER_GetFrontGridPos(
		FIELD_PLAYER *fld_player, int *gx, int *gy, int *gz );
extern BOOL FIELD_PLAYER_CheckLiveFldMMdl( FIELD_PLAYER *fld_player );

//======================================================================
//	消します
//======================================================================
extern GFL_BBDACT_RESUNIT_ID GetPlayerBBdActResUnitID(
		FIELD_PLAYER *pcActCont );

#include "field_player_grid.h"
