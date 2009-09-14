//======================================================================
/**
 * @file  map_attr.h
 * @brief	マップアトリビュート
 * @author kagaya
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_g3d_mapper.h"

//======================================================================
//  define
//======================================================================
#define MAPATTR_ERROR (0xffffffff) ///<アトリビュートエラー
#define MAPATTR_VAL_ERR (0x00FF)  ///<アトリビュートの無効Value

//--------------------------------------------------------------
/// MAPATTR_FLAGBIT
//--------------------------------------------------------------
typedef enum
{
  MAPATTR_FLAGBIT_NONE = 0, ///<何も無し
  MAPATTR_FLAGBIT_HITCH = (1<<0), ///<進入不可フラグ ON=不可
  MAPATTR_FLAGBIT_WATER = (1<<1), ///<水アトリビュート ON=水
  MAPATTR_FLAGBIT_ENCOUNT = (1<<2), ///<敵出現フラグ ON=出現
  MAPATTR_FLAGBIT_FOOTMARK = (1<<3), ///<足跡表示フラグ ON=足跡有り
  MAPATTR_FLAGBIT_SPLASH = (1<<4), ///<水飛沫表示フラグ ON=水飛沫有り
  MAPATTR_FLAGBIT_GRASS = (1<<5), ///<草揺れフラグ ON=草
  MAPATTR_FLAGBIT_REFLECT = (1<<6), ///<映り込み表示フラグ ON=映り込み有り
  MAPATTR_FLAGBIT_SHADOW = (1<<7), ///<影表示フラグ ON=影表示有り
}MAPATTR_FLAGBIT;

//======================================================================
//  struct
//======================================================================
///MAPATTR
typedef u32 MAPATTR;
///MAPATTR_VALUE
typedef u16 MAPATTR_VALUE;
///MAPATTR_FLAG
typedef u16 MAPATTR_FLAG;

//======================================================================
//  proto
//======================================================================
extern MAPATTR MAPATTR_GetAttribute(FLDMAPPER *mapper, const VecFx32 *pos);
extern MAPATTR_VALUE MAPATTR_GetAttrValue( const MAPATTR attr );
extern MAPATTR_FLAG MAPATTR_GetAttrFlag( const MAPATTR attr );
extern BOOL MAPATTR_GetHitchFlag( const MAPATTR attr );

extern BOOL MAPATTR_VALUE_CheckLongGrass( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckJumpUp( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckJumpDown( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckumpLeft( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckJumpRight( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckCounter( const MAPATTR_VALUE val );

//アトリビュート話しかけ関連
extern BOOL MAPATTR_VALUE_CheckMat( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckPC( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckMap( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckTV( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckBookShelf1( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckBookShelf2( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckBookShelf3( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckBookShelf4( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckVase( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckDustBox( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckShopShelf1( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckShopShelf2( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckShopShelf3( const MAPATTR_VALUE val );

extern BOOL MAPATTR_VALUE_CheckEncountGrassA1( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountGrassA2( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountGrassB1( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountGrassB2( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountGrassA( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountGrassB( const MAPATTR_VALUE val );
