//======================================================================
/**
 * @file	field_player_nogrid.h
 * @brief	ノングリッド移動　フィールドプレイヤー制御
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "field_player.h"
#include "field_player_core.h"
#include "field_camera.h"
#include "fldmmdl.h"
#include "field_effect.h"


#include "field_rail.h"

#include "field_sound.h"

//======================================================================
//	define
//======================================================================
//-------------------------------------
///	氷上で回転するSE
//=====================================
#define FIELD_PLAYER_SE_NOGRID_ICE_SPIN ( SEQ_SE_FLD_66 )

//======================================================================
//	struct
//======================================================================
//-------------------------------------
///	レール動作用補助ワーク
//=====================================
typedef struct _FIELD_PLAYER_NOGRID FIELD_PLAYER_NOGRID;


//======================================================================
//	FIELD_PLAYER内部で使用される関数
//======================================================================

// 生成・破棄
extern FIELD_PLAYER_NOGRID* FIELD_PLAYER_NOGRID_Create( FIELD_PLAYER_CORE* p_player_core, HEAPID heapID );
extern void FIELD_PLAYER_NOGRID_Delete( FIELD_PLAYER_NOGRID* p_player );

// 動作リセット　再構築
extern void FIELD_PLAYER_NOGRID_Restart( FIELD_PLAYER_NOGRID* p_player, const RAIL_LOCATION* cp_pos );
extern void FIELD_PLAYER_NOGRID_Stop( FIELD_PLAYER_NOGRID* p_player );

// 動作
extern void FIELD_PLAYER_NOGRID_Move( FIELD_PLAYER_NOGRID* p_player, int key_trg, int key_cont );

// 移動不可能ヒットチェック
extern BOOL FIELD_PLAYER_NOGRID_IsHitch( const FIELD_PLAYER_NOGRID* cp_player );

// オート動作　チェック
extern BOOL FIELD_PLAYER_NOGRID_IsAutoMove( const FIELD_PLAYER_NOGRID* cp_player );

// 各種パラメータの設定・取得
extern void FIELD_PLAYER_NOGRID_SetLocation( FIELD_PLAYER_NOGRID* p_player, const RAIL_LOCATION* cp_location );
extern void FIELD_PLAYER_NOGRID_GetLocation( const FIELD_PLAYER_NOGRID* cp_player, RAIL_LOCATION* p_location );
extern void FIELD_PLAYER_NOGRID_GetDirLocation( const FIELD_PLAYER_NOGRID* cp_player, u16 dir, RAIL_LOCATION* p_location );
extern void FIELD_PLAYER_NOGRID_GetPos( const FIELD_PLAYER_NOGRID* cp_player, VecFx32* p_pos );
extern FIELD_RAIL_WORK* FIELD_PLAYER_NOGRID_GetRailWork( const FIELD_PLAYER_NOGRID* cp_player );

// 自機動作の停止
extern void FIELD_PLAYER_NOGRID_ForceStop( FIELD_PLAYER_NOGRID* p_player );

#ifdef PM_DEBUG
//======================================================================
//	旧動作
//======================================================================
extern void FIELD_PLAYER_NOGRID_Free_Move( FIELD_PLAYER *fld_player, int key, fx32 onedist );
extern void FIELD_PLAYER_C3_Move(
		FIELD_PLAYER *fld_player, int key, u16 angle );
#endif


