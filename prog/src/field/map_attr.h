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
#include "map_attr_def.h"
#include "effect_encount_def.h" //EFFENC_TYPE_ID参照
#include "battle/battle_bg_def.h"  //BtlBgAttr参照

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
extern MAPATTR MAPATTR_GetAttribute(const FLDMAPPER *mapper, const VecFx32 *pos);
extern MAPATTR_VALUE MAPATTR_GetAttrValue( const MAPATTR attr );
extern MAPATTR_FLAG MAPATTR_GetAttrFlag( const MAPATTR attr );
extern BOOL MAPATTR_IsEnable( const MAPATTR attr );
extern BOOL MAPATTR_GetHitchFlag( const MAPATTR attr );

extern BOOL MAPATTR_VALUE_CheckLongGrass( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckJumpUp( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckJumpDown( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckumpLeft( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckJumpRight( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSandStream( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckCounter( const MAPATTR_VALUE val );

//アトリビュート話しかけ関連
extern BOOL MAPATTR_VALUE_CheckMat( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckPC( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckMap( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckTV( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckVendorMachine( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckBookShelf1( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckBookShelf2( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckBookShelf3( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckBookShelf4( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckVase( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckDustBox( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckShopShelf1( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckShopShelf2( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckShopShelf3( const MAPATTR_VALUE val );

extern BOOL MAPATTR_VALUE_CheckEncountShortGrassLow( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountShortGrassHigh( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountLongGrassLow( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountLongGrassHigh( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountGrassLow( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountGrassHigh( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountGrass( const MAPATTR_VALUE val );

extern BOOL MAPATTR_VALUE_CheckKairikiAna( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckShore( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckPool( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckShoal( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckWaterFall( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSnow( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSnowNotCycle( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSnowType( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckIce( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckIce02( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckMarsh( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckDeepSea( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSand( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckDesert( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSandType( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckWaterType( const MAPATTR_VALUE val );

extern BOOL MAPATTR_VALUE_CheckMonoGround( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSeasonGround1( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSeasonGround2( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountGround( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckEncountIndoor( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckLawn( const MAPATTR_VALUE val );

extern BOOL MAPATTR_VALUE_CheckElecFloor( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckElecRock( const MAPATTR_VALUE val );

extern BOOL MAPATTR_VALUE_CheckOze01( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckOzeStairs( const MAPATTR_VALUE val );

extern BOOL MAPATTR_VALUE_CheckDesertDeep( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSnowGrassLow( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckSnowGrassHigh( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckMirrorFloor( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckUpDownFloor( const MAPATTR_VALUE val );

extern BOOL MAPATTR_VALUE_CheckNotMoveUp( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckNotMoveDown( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckNotMoveLeft( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckNotMoveRight( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckCycleNotUse( MAPATTR_VALUE val );

extern BOOL MAPATTR_VALUE_CheckGrassAllYearLow( const MAPATTR_VALUE val );
extern BOOL MAPATTR_VALUE_CheckGrassAllYearHigh( const MAPATTR_VALUE val );

extern EFFENC_TYPE_ID MAPATTR_GetEffectEncountType( MAPATTR attr );
extern BtlBgAttr MAPATTR_GetBattleAttrID( MAPATTR_VALUE value );
extern BOOL MAPATTR_CheckNaminoriUse( const MAPATTR now_attr, const MAPATTR front_attr );
