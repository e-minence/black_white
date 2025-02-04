//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fldmmdl_railmove.c
 *	@brief  動作モデル  レール動作系
 *	@author	 tomoya takahashi
 *	@date		2009.08.24
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "fldmmdl.h"

#include "fieldmap.h"
#include "map_attr.h"

#include "fldeff_shadow.h"
#include "fldeff_kemuri.h"
#include "fldeff_grass.h"
#include "fldeff_footmark.h"
#include "fldeff_reflect.h"


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

static int MMdl_CheckMoveStart( const MMDL * mmdl );
static void MMdl_GetAttrMoveBefore( MMDL * mmdl );
static void MMdl_ProcMoveStartFirst( MMDL * mmdl );
static void MMdl_ProcMoveStartSecond( MMDL * mmdl );
static void MMdl_ProcMoveEnd( MMDL * mmdl );

//アトリビュート更新
static BOOL MMdl_UpdateCurrentRailAttr( MMDL * mmdl ); 


// アトリビュート関係
static void MMdl_MapAttrProc_MoveStartFirst( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEnd( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEndJump( MMDL * mmdl );

static void MMdl_MapAttrGrassProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrGrassProc_12(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrFootMarkProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrShadowProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrShadowProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrShadowProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrGroundSmokeProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );




static void MMdl_MapAttrReflect_01(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrReflect_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );





static FLDEFF_CTRL * mmdl_GetFldEffCtrl( MMDL *mmdl );



//----------------------------------------------------------------------------
/**
 *	@brief  フィールド動作モデル　レール動作初期化
 *
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_InitRailMoveProc( MMDL * mmdl )
{
	MMDL_CallMoveInitProc( mmdl );
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVEPROC_INIT );
}

//----------------------------------------------------------------------------
/**
 *	@brief  フィールド動作モデル  レール動作　更新処理
 *
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_UpdateRailMove( MMDL * mmdl )
{
	if( MMDL_CheckMMdlSysStatusBit(
		mmdl,MMDLSYS_STABIT_MOVE_PROC_STOP) ){
		return;
	}
	
	MMdl_GetAttrMoveBefore( mmdl );
	MMdl_ProcMoveStartFirst( mmdl );
	
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) ){
		MMDL_ActionAcmd( mmdl );
	}else if( MMDL_CheckMoveBitMoveProcPause(mmdl) == FALSE ){
		if( MMdl_CheckMoveStart(mmdl) == TRUE ){

			MMDL_CallMoveProc( mmdl );

		}
	}
	
	MMdl_ProcMoveStartSecond( mmdl );
	MMdl_ProcMoveEnd( mmdl );
}


//----------------------------------------------------------------------------
/**
 *	@brief  レールロケーションのアトリビュートを取得する
 *
 *	@param	mmdl        モデル
 *	@param	location    ロケーション
 *
 *	@return マップアトリビュート
 */
//-----------------------------------------------------------------------------
MAPATTR MMDL_GetRailLocationAttr( const MMDL * mmdl, const RAIL_LOCATION* location )
{
  const MMDLSYS* mmdlsys = MMDL_GetMMdlSys( mmdl );
  const FLDNOGRID_MAPPER* pNOGRIDMapper = MMDLSYS_GetNOGRIDMapper( mmdlsys );

  return FLDNOGRID_MAPPER_GetAttr( pNOGRIDMapper, location );
}



//----------------------------------------------------------------------------
/**
 *	@brief  アングルＹａｗから、レールオブジェの表示方向を返す
 *
 *	@param	mmdl        モデル
 *	@param	angleYaw    アングル
 *
 *	@return 方向
 */
//-----------------------------------------------------------------------------
u16 MMDL_RAIL_GetAngleYawToDirFour( MMDL * mmdl, u16 angleYaw )
{
  VecFx16 mmdl_way;
  u16 mmdl_yaw;
  s32 diff_yaw;
  u16 dir;
  static const u8 data_angle16_4[16] =
  {
    DIR_DOWN,
    DIR_RIGHT, DIR_RIGHT, DIR_RIGHT, DIR_RIGHT, DIR_RIGHT, DIR_RIGHT,
    DIR_UP, DIR_UP,
    DIR_LEFT, DIR_LEFT, DIR_LEFT, DIR_LEFT, DIR_LEFT, DIR_LEFT,
    DIR_DOWN,
  };

  dir = MMDL_GetDirDisp( mmdl );

  // カメラ方向と進行方向から、人物の表示方向を求める
  MMDL_Rail_GetDirLineWay( mmdl, dir, &mmdl_way );
  mmdl_way.y = 0;
  VEC_Fx16Normalize( &mmdl_way, &mmdl_way );

  // 平面方向
  mmdl_yaw = FX_Atan2Idx( mmdl_way.x, mmdl_way.z );


  // 角度の差から、角度を求める
  diff_yaw = (s32)mmdl_yaw - (s32)angleYaw;
  if( diff_yaw < 0 )
  {
    diff_yaw += 0x10000;  // プラスにして計算
  }
  diff_yaw >>= 12;

  GF_ASSERT( diff_yaw < 16 );
  return data_angle16_4[ diff_yaw ];
}






//-----------------------------------------------------------------------------
/**
 *      private関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  動作可能　チェック
 *
 *	@param	mmdl 
 *
 *	@retval TRUE  可能
 *	@retval FALSE 不可能
 */
//-----------------------------------------------------------------------------
static int MMdl_CheckMoveStart( const MMDL * mmdl )
{
	if( MMDL_CheckMoveBitMove(mmdl) == TRUE ){
		return( TRUE );
	}
	
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) == 0 ){
		return( TRUE );
	}else if( MMDL_GetMoveCode(mmdl) == MV_TR_PAIR ){ //親の行動に従う
		return( TRUE );
	}
	
	
	return( TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  レールアトリビュート取得
 *
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_GetAttrMoveBefore( MMDL * mmdl )
{
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) )
  {
		if( MMdl_UpdateCurrentRailAttr(mmdl) == TRUE )
    {
			MMDL_OnMoveBitMoveStart( mmdl );
    }
  }
}

//----------------------------------------------------------------------------
/**
 * 移動開始で発生　1st
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_ProcMoveStartFirst( MMDL * mmdl )
{
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_START) ){
		MMdl_MapAttrProc_MoveStartFirst( mmdl );
	}
	
	MMDL_OffMoveBit( mmdl,
		MMDL_MOVEBIT_MOVE_START | MMDL_MOVEBIT_JUMP_START );
}

//----------------------------------------------------------------------------
/**
 * 移動開始で発生　2nd
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_ProcMoveStartSecond( MMDL * mmdl )
{
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_JUMP_START) ){
		MMdl_MapAttrProc_MoveStartJumpSecond( mmdl );
	}else if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_START) ){
		MMdl_MapAttrProc_MoveStartSecond( mmdl );
	}
	
	MMDL_OffMoveBit( mmdl,
		MMDL_MOVEBIT_MOVE_START | MMDL_MOVEBIT_JUMP_START );
}

//----------------------------------------------------------------------------
/**
 * 動作終了で発生
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_ProcMoveEnd( MMDL * mmdl )
{
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_JUMP_END) ){
		MMdl_MapAttrProc_MoveEndJump( mmdl );
	}else if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_END) ){
		MMdl_MapAttrProc_MoveEnd( mmdl );
	}
	
	MMDL_OffMoveBit( mmdl,
		MMDL_MOVEBIT_MOVE_END | MMDL_MOVEBIT_JUMP_END );
}




//----------------------------------------------------------------------------
/**
 *	@brief  現在座標でアトリビュート反映
 *          MMDL_STABIT_ATTR_GET_ERRORのセットも併せて行う
 *
 *	@param	mmdl
 *
 *	@retval TRUE  取得成功
 *	@retval FALSE 取得失敗
 */
//-----------------------------------------------------------------------------
static BOOL MMdl_UpdateCurrentRailAttr( MMDL * mmdl )
{
	MAPATTR old_attr = MAPATTR_ERROR;
	MAPATTR now_attr = old_attr;
	
  if( MMDL_CheckStatusBitAttrGetOFF(mmdl) == FALSE ){
    // レールから、今と前のLOCATIONが取れるようにする
    RAIL_LOCATION old;
    RAIL_LOCATION now;
    const MMDLSYS* mmdlsys = MMDL_GetMMdlSys( mmdl );
    const FLDNOGRID_MAPPER* pNOGRIDMapper = MMDLSYS_GetNOGRIDMapper( mmdlsys );

    MMDL_GetRailLocation( mmdl, &now );
    MMDL_GetOldRailLocation( mmdl, &old );

    old_attr = FLDNOGRID_MAPPER_GetAttr( pNOGRIDMapper, &old );
    now_attr = FLDNOGRID_MAPPER_GetAttr( pNOGRIDMapper, &now );
  }
  
	MMDL_SetMapAttrOld( mmdl, old_attr );
	MMDL_SetMapAttr( mmdl, now_attr );
  
  if( now_attr == MAPATTR_ERROR ){
		MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_ATTR_GET_ERROR );
		return( FALSE );
  }
  
	MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ATTR_GET_ERROR );
	return( TRUE );
}





//======================================================================
//	マップアトリビュート
//======================================================================
//--------------------------------------------------------------
/**
 * 動作開始で発生するアトリビュート処理 1st
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartFirst( MMDL * mmdl )
{
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
      
      MMdl_MapAttrGrassProc_0( mmdl, now, old, prm );
      MMdl_MapAttrShadowProc_0( mmdl, now, old, prm );
      MMdl_MapAttrReflect_01( mmdl, now, old, prm );
    }
  }
}

//--------------------------------------------------------------
/**
 * 動作開始で発生するアトリビュート処理 2nd
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * mmdl )
{
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );

      MMdl_MapAttrGrassProc_12( mmdl, now, old, prm );
      MMdl_MapAttrFootMarkProc_1( mmdl, now, old, prm );
      MMdl_MapAttrShadowProc_1( mmdl, now, old, prm );
      MMdl_MapAttrReflect_01( mmdl, now, old, prm );
      
    }
  }
}

//--------------------------------------------------------------
/**
 * 動作開始で発生するアトリビュート処理 2nd Jump
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * mmdl )
{
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );

      MMdl_MapAttrShadowProc_1( mmdl, now, old, prm );
      MMdl_MapAttrReflect_01( mmdl, now, old, prm );
    }
  }
}

//--------------------------------------------------------------
/**
 * 動作終了で発生するアトリビュート処理　end
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEnd( MMDL * mmdl )
{
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
      
      //終了　アトリビュート処理
      MMdl_MapAttrReflect_2( mmdl, now, old, prm );
      MMdl_MapAttrShadowProc_2( mmdl, now, old, prm );
    }
  }
}

//--------------------------------------------------------------
/**
 * 動作終了で発生するアトリビュート処理　end jump
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEndJump( MMDL * mmdl )
{
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
      
      //終了　アトリビュート処理
      MMdl_MapAttrReflect_2( mmdl, now, old, prm );
      MMdl_MapAttrShadowProc_2( mmdl, now, old, prm );
      MMdl_MapAttrGrassProc_12( mmdl, now, old, prm );
      MMdl_MapAttrGroundSmokeProc_2( mmdl, now, old, prm );
    }
  }
}


//======================================================================
//	アトリビュート　草
//======================================================================
//--------------------------------------------------------------
/**
 * 草　動作開始 0
 * @param	mmdl		MMDL *
 * @param	now			現在のアトリビュート
 * @param	old			過去のアトリビュート
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{

  {
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
    if( (flag&MAPATTR_FLAGBIT_GRASS) ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
      FLDEFF_GRASS_SetMMdl( fectrl, mmdl, FALSE, FLDEFF_GRASS_SHORT );
    }
  }

}

//--------------------------------------------------------------
/**
 * 草　動作 1,2
 * @param	mmdl	MMDL *
 * @param	now			現在のアトリビュート
 * @param	old			過去のアトリビュート
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_12(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
  
  {
    if( now != MAPATTR_ERROR ){
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
      if( (flag&MAPATTR_FLAGBIT_GRASS) ){
        FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
        FLDEFF_GRASS_SetMMdl( fectrl, mmdl, TRUE, FLDEFF_GRASS_SHORT  );
      }
    }
  }

}

//======================================================================
//	アトリビュート　足跡
//======================================================================
//--------------------------------------------------------------
/**
 * 足跡　動作開始 1
 * @param	mmdl	MMDL *
 * @param	now		現在のアトリビュート
 * @param	old		過去のアトリビュート
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrFootMarkProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
  if( old != MAPATTR_ERROR ){
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( old );
  
    if( (flag & MAPATTR_FLAGBIT_FOOTMARK) ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
      FOOTMARK_TYPE type = FOOTMARK_TYPE_HUMAN;
    
      if( prm->footmark_type == MMDL_FOOTMARK_CYCLE ){
        type = FOOTMARK_TYPE_CYCLE;
      }

      FLDEFF_FOOTMARK_SetMMdl( mmdl, fectrl, type );
    }
  }
}

//======================================================================
//	アトリビュート　影
//======================================================================
//--------------------------------------------------------------
/**
 * 影　動作開始 0
 * @param	mmdl	MMDL *
 * @param	now		現在のアトリビュート
 * @param	old		過去のアトリビュート
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{

	MMdl_MapAttrShadowProc_1( mmdl, now, old, prm );
}

//--------------------------------------------------------------
/**
 * 影　動作開始 1
 * @param	mmdl	MMDL *
 * @param	now		現在のアトリビュート
 * @param	old		過去のアトリビュート
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
  {
  	const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
    
		if( MMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
    
		if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_SHADOW_SET) == 0 ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
      FLDEFF_SHADOW_SetMMdl( mmdl, fectrl );
			MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_SET );
    }
  }
}

//--------------------------------------------------------------
/**
 * 影　動作終了 2
 * @param	mmdl	MMDL *
 * @param	now		現在のアトリビュート
 * @param	old		過去のアトリビュート
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
}

//======================================================================
//	アトリビュート　土煙
//======================================================================
//--------------------------------------------------------------
/**
 * 土煙　動作終了 2 
 * @param	mmdl	MMDL *
 * @param	now		現在のアトリビュート
 * @param	old		過去のアトリビュート
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGroundSmokeProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
  {
    FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
    FLDEFF_KEMURI_SetRailMMdl( mmdl, fectrl );
  }
}


//======================================================================
//	アトリビュート　映りこみ
//======================================================================
//--------------------------------------------------------------
/**
 * 映りこみ　動作開始 01
 * @param	mmdl	MMDL *
 * @param	now			現在のアトリビュート
 * @param	old			過去のアトリビュート
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_01(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
  if( prm->reflect_type == MMDL_REFLECT_NON ){
    return;
  }
  
  if( MMDL_CheckMoveBitReflect(mmdl) == FALSE )
  {
    MAPATTR_FLAG flag;
    MAPATTR hit_attr = MAPATTR_ERROR;
    
    #ifdef DEBUG_REFLECT_CHECK
    now = MAPATTR_FLAGBIT_REFLECT << 16;
    #endif
    
    if( now != MAPATTR_ERROR ){
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
      if( (flag&MAPATTR_FLAGBIT_REFLECT) )
      {
        hit_attr = now;
        #ifdef DEBUG_REFLECT_CHECK
        hit_attr = 0;
        #endif
      }
      else
      {
        MAPATTR next = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
        flag = MAPATTR_GetAttrFlag( next );
      
        if( (flag&MAPATTR_FLAGBIT_REFLECT) )
        {
          hit_attr = next;
        }
      }
    
      if( hit_attr != MAPATTR_ERROR )
      {
        REFLECT_TYPE type;
        FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
        MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
      
        MMDL_SetMoveBitReflect( mmdl, TRUE );
      
        type = REFLECT_TYPE_POND; //本来はアトリビュート識別してタイプ決定
        FLDEFF_REFLECT_SetMMdl( mmdlsys, mmdl, fectrl, type );
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * 映りこみ　動作終了 2
 * @param	mmdl	MMDL *
 * @param	now			現在のアトリビュート
 * @param	old			過去のアトリビュート
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
  
	if( prm->reflect_type == MMDL_REFLECT_NON ||
		MMDL_CheckMoveBitReflect(mmdl) == FALSE ){
		return;
  }

  {
    MAPATTR_FLAG flag = MAPATTR_FLAGBIT_NONE;
		MAPATTR attr = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
    
    #ifdef DEBUG_REFLECT_CHECK
    attr = MAPATTR_FLAGBIT_REFLECT << 16;
    #endif
    
    if( attr == MAPATTR_ERROR ){
		  MMDL_SetMoveBitReflect( mmdl, FALSE );
    }else{
      flag = MAPATTR_GetAttrFlag( attr );

      if( (flag&MAPATTR_FLAGBIT_REFLECT) == 0 ){
		    MMDL_SetMoveBitReflect( mmdl, FALSE );
      }
    }
  }
}







//--------------------------------------------------------------
/**
 * FLDEFF_CTRL取得
 * @param mmdl  MMDL*
 * @retval FLDEFF_CTRL*
 */
//--------------------------------------------------------------
static FLDEFF_CTRL * mmdl_GetFldEffCtrl( MMDL *mmdl )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  FIELDMAP_WORK *fieldMapWork = MMDLSYS_GetFieldMapWork( (MMDLSYS*)mmdlsys );
  return( FIELDMAP_GetFldEffCtrl(fieldMapWork) );
}
