//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_rail_access.h
 *	@brief	フィールドレール　アクセス処理
 *	@author	tomoya takahashi
 *	@date		2009.07.07
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "field_rail.h"

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

//------------------------------------------------------------------
//  FIELD_RAIL_MANアクセス関数
//------------------------------------------------------------------
extern FIELD_CAMERA* FIELD_RAIL_MAN_GetCamera( const FIELD_RAIL_MAN * man );
extern const FIELD_RAIL* FIELD_RAIL_MAN_GetNowRail( const FIELD_RAIL_MAN * man );

//------------------------------------------------------------------
//  FIELD_RAILアクセス関数
//------------------------------------------------------------------
extern FIELD_RAIL_TYPE FIELD_RAIL_GetType( const FIELD_RAIL* rail );
extern const RAIL_POINT* FIELD_RAIL_GetPoint( const FIELD_RAIL* rail );
extern const RAIL_LINE* FIELD_RAIL_GetLine( const FIELD_RAIL* rail );
extern s32 FIELD_RAIL_GetLineOfs( const FIELD_RAIL* rail );
extern s32 FIELD_RAIL_GetLineOfsMax( const FIELD_RAIL* rail );
extern s32 FIELD_RAIL_GetWidthOfs( const FIELD_RAIL* rail );
extern s32 FIELD_RAIL_GetWidthOfsMax( const FIELD_RAIL* rail );
extern fx32 FIELD_RAIL_GetOfsUnit( const FIELD_RAIL* rail );
extern const RAIL_CAMERA_SET* FIELD_RAIL_GetCameraSet( const FIELD_RAIL* rail );


//------------------------------------------------------------------
//  RAIL_LINEアクセス関数
//------------------------------------------------------------------
extern const RAIL_POINT* FIELD_RAIL_GetPointStart( const FIELD_RAIL* rail );
extern const RAIL_POINT* FIELD_RAIL_GetPointEnd( const FIELD_RAIL* rail );
extern const RAIL_LINEPOS_SET* FIELD_RAIL_GetLinePosSet( const FIELD_RAIL* rail );


//------------------------------------------------------------------
//  RAIL_POINT
//------------------------------------------------------------------
extern const RAIL_CAMERA_SET* FIELD_RAIL_POINT_GetCameraSet( const FIELD_RAIL* rail, const RAIL_POINT* point );


