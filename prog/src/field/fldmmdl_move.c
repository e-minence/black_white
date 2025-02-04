//======================================================================
/**
 *
 * @file  fieldobj_move.c
 * @brief  動作モデル 動作系
 * @author  kagaya
 * @date  05.07.25
 *
 */
//======================================================================
#include "fldmmdl.h"

#include "include/gamesystem/pm_season.h"

#include "fieldmap.h"

#include "fldeff_shadow.h"
#include "fldeff_kemuri.h"
#include "fldeff_grass.h"
#include "fldeff_footmark.h"
#include "fldeff_reflect.h"
#include "fldeff_ripple.h"
#include "fldeff_splash.h"
#include "fldeff_d06denki.h"

#include "sound/pm_sndsys.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
//  debug
//--------------------------------------------------------------

//--------------------------------------------------------------
///  ステータスビット
//--------------------------------------------------------------
///移動動作を禁止するステータスビット
#define MOVEBIT_MOVE_ERROR (MMDL_MOVEBIT_HEIGHT_GET_ERROR|MMDL_MOVEBIT_ATTR_GET_ERROR)

//--------------------------------------------------------------
///  アトリビュートオフセット
//--------------------------------------------------------------
#define ATTROFFS_Y_NUMA (NUM_FX32(-12))///<沼Yオフセット
#define ATTROFFS_Y_NUMA_DEEP (NUM_FX32(-14))///<深い沼Yオフセット
#define ATTROFFS_Y_YUKI (NUM_FX32(-12))///<雪Yオフセット
#define ATTROFFS_Y_YUKI_DEEP (NUM_FX32(-14))///<深い雪Yオフセット
#define ATTROFFS_Y_YUKI_DEEP_MOST (NUM_FX32(-16))///<更に深い雪Yオフセット
#define ATTROFFS_Y_DESERT_DEEP (NUM_FX32(-7)) ///<深い砂漠Yオフセット

///高さ差分による移動制限
#define HEIGHT_DIFF_COLLISION (FX32_ONE*(20))


//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// ATTRDATA
//--------------------------------------------------------------
typedef struct
{
  MAPATTR attr_old;
  MAPATTR attr_now;
  MAPATTR_VALUE attr_val_old;
  MAPATTR_VALUE attr_val_now;
  MAPATTR_FLAG attr_flag_old;
  MAPATTR_FLAG attr_flag_now;
  
  const OBJCODE_PARAM *objcode_prm;
  FLDEFF_CTRL *fectrl;
  
  u8 season;
  u8 padding[3];
}ATTRDATA;

//======================================================================
//  proto
//======================================================================
static int MMdl_CheckMoveStart( const MMDL * mmdl );
static void MMdl_GetHeightMoveBefore( MMDL * mmdl );
static void MMdl_GetAttrMoveBefore( MMDL * mmdl );
static void MMdl_ProcMoveStartFirst( MMDL * mmdl );
static void MMdl_ProcMoveStartSecond( MMDL * mmdl );
static void MMdl_ProcMoveEnd( MMDL * mmdl );

static void mmdl_InitAttrData( const MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrProc_MoveStartFirst( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEnd( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEndJump( MMDL * mmdl );

static void MMdl_MapAttrHeight_02( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrGrassProc_0( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrGrassProc_1( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrGrassProc_2( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrGrassProc_Jump2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrFootMarkProc_1( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrSplashProc_0( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrSplashProc_12( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrSplashProc_Jump1( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrShadowProc_01( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrShadowProc_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrGroundSmokeProc_2( MMDL *mmdl, ATTRDATA *data );

//static void MMdl_MapAttrLGrassProc_0( MMDL *mmdl, ATTRDATA *data );
//static void MMdl_MapAttrLGrassProc_1( MMDL *mmdl, ATTRDATA *data );

//static void MMdl_MapAttrNGrassProc_0( MMDL *mmdl, ATTRDATA *data );
//static void MMdl_MapAttrNGrassProc_1( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrPoolProc_1( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrPoolProc_2( MMDL *mmdl, ATTRDATA *data );

//static void MMdl_MapAttrSwampProc_1( MMDL *mmdl, ATTRDATA *data );
//static void MMdl_MapAttrSwampProc_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrReflect_01( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrReflect_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrBiriBiri_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrSEProc_1( MMDL *mmdl, ATTRDATA *data );

static BOOL MMdl_HitCheckMoveAttr(
  const MMDL * mmdl, u16 dir, const VecFx32 pos );

static BOOL (* const DATA_HitCheckAttr_Now[DIR_MAX4])( MAPATTR_VALUE val );
static BOOL (* const DATA_HitCheckAttr_Next[DIR_MAX4])( MAPATTR_VALUE val );

#if 0
static BOOL MMdl_GetMapGridInfo(
  const MMDL *mmdl, const VecFx32 *pos,
  FLDMAPPER_GRIDINFO *pGridInfo );
#endif

//======================================================================
//  フィールド動作モデル 動作
//======================================================================
//--------------------------------------------------------------
/**
 * フィールド動作モデル 動作初期化
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_InitMoveProc( MMDL * mmdl )
{
  MMDL_CallMoveInitProc( mmdl );
  MMDL_MoveSubProcInit( mmdl );
  MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVEPROC_INIT );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデル動作
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateMove( MMDL * mmdl )
{
  if( MMDL_CheckMMdlSysStatusBit(
    mmdl,MMDLSYS_STABIT_MOVE_PROC_STOP) ){
    return;
  }
  
  MMdl_GetHeightMoveBefore( mmdl );
  MMdl_GetAttrMoveBefore( mmdl );
  MMdl_ProcMoveStartFirst( mmdl );
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) ){
    MMDL_ActionAcmd( mmdl );
  }else if( MMDL_CheckMoveBitMoveProcPause(mmdl) == FALSE ){
    if( MMdl_CheckMoveStart(mmdl) == TRUE ){
      if( MMDL_MoveSub(mmdl) == FALSE ){
        MMDL_CallMoveProc( mmdl );
      }
    }
    else
    {
      D_MMDL_DPrintf("ERROR MMDL Move Start ID=%xH GX=%xH,GZ=%xH\n",
        MMDL_GetOBJID(mmdl),MMDL_GetGridPosX(mmdl),MMDL_GetGridPosZ(mmdl));
    }
  }
  
  MMdl_ProcMoveStartSecond( mmdl );
  MMdl_ProcMoveEnd( mmdl );
}

//--------------------------------------------------------------
/**
 * 動作可能かどうかチェック
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=可能,FALSE=不可
 */
//--------------------------------------------------------------
static int MMdl_CheckMoveStart( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBitMove(mmdl) == FALSE ){
    if( MMDL_CheckMoveBit(mmdl,MOVEBIT_MOVE_ERROR) == 0 ){
      return( TRUE );
    }
    
    if( MMDL_GetMoveCode(mmdl) == MV_TR_PAIR ){ //親の行動に従う
      return( TRUE );
    }
    
    {  //移動禁止フラグ相殺チェック
      u32 flag = MMDL_GetMoveBit( mmdl );
    
      //高さ取得しない場合
      if( (flag & MMDL_MOVEBIT_HEIGHT_GET_ERROR) &&
          MMDL_CheckStatusBitHeightGetOFF(mmdl) == FALSE ){
        return( FALSE );
      }
    
      //アトリビュート取得しない場合
      if( (flag & MMDL_MOVEBIT_ATTR_GET_ERROR) &&  
          MMDL_CheckStatusBitAttrGetOFF(mmdl) == FALSE ){
        return( FALSE );
      }
    }
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * 移動開始時に行う高さ取得
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_GetHeightMoveBefore( MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_HEIGHT_GET_ERROR) ){
    MMDL_UpdateCurrentHeight( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * 移動開始時に行うアトリビュート取得
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_GetAttrMoveBefore( MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) ){
    if( MMDL_UpdateCurrentMapAttr(mmdl) == TRUE ){
      MMDL_OnMoveBitMoveStart( mmdl );
    }
  }
}

//--------------------------------------------------------------
/**
 * 移動開始で発生　1st
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_ProcMoveStartFirst( MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_START) ){
    MMDL_CallMoveProcAfterDrawInit( mmdl );
    MMdl_MapAttrProc_MoveStartFirst( mmdl );
  }
  
  MMDL_OffMoveBit( mmdl,
    MMDL_MOVEBIT_MOVE_START | MMDL_MOVEBIT_JUMP_START );
}

//--------------------------------------------------------------
/**
 * 移動開始で発生　2nd
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
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

//--------------------------------------------------------------
/**
 * 動作終了で発生
 * @param  mmdl  MMDL *GX_WNDMASK_NONE
 * @retval  nothing
 */
//--------------------------------------------------------------
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

//======================================================================
//  マップアトリビュート
//======================================================================
//--------------------------------------------------------------
/**
 * アトリビュートデータ初期化
 * @param mmdl MMDL
 * @param data ATTRDATA
 * @retval nothing
 */
//--------------------------------------------------------------
static void mmdl_InitAttrData( const MMDL *mmdl, ATTRDATA *data )
{
  MI_CpuClear( data, sizeof(ATTRDATA) );
  
  data->attr_old = MMDL_GetMapAttrOld( mmdl );
  data->attr_now = MMDL_GetMapAttr( mmdl );
  
  if( data->attr_now != MAPATTR_ERROR ){
    data->attr_val_now = MAPATTR_GetAttrValue( data->attr_now );
    data->attr_flag_now = MAPATTR_GetAttrFlag( data->attr_now );
  }
  
  if( data->attr_old != MAPATTR_ERROR ){
    data->attr_val_old = MAPATTR_GetAttrValue( data->attr_old );
    data->attr_flag_old = MAPATTR_GetAttrFlag( data->attr_old );
  }
  
  data->objcode_prm = MMDL_GetOBJCodeParam( mmdl );
  
  {
    MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
    FIELDMAP_WORK *fieldmap = MMDLSYS_GetFieldMapWork( mmdlsys );
    data->fectrl = FIELDMAP_GetFldEffCtrl( fieldmap );
    data->season = FLDEFF_CTRL_GetSeasonID( data->fectrl );
  }
}

//--------------------------------------------------------------
/**
 * 動作開始で発生するアトリビュート処理 1st
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartFirst( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) == 0 ){
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      ATTRDATA data;
      mmdl_InitAttrData( mmdl, &data );
    
      MMdl_MapAttrGrassProc_0( mmdl, &data );
      MMdl_MapAttrSplashProc_0( mmdl, &data );
      MMdl_MapAttrShadowProc_01( mmdl, &data );
      MMdl_MapAttrHeight_02( mmdl, &data );
      MMdl_MapAttrReflect_01( mmdl, &data );
    }
  }
}

//--------------------------------------------------------------
/**
 * 動作開始で発生するアトリビュート処理 2nd
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) == 0 ){
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      ATTRDATA data;
      mmdl_InitAttrData( mmdl, &data );
    
      MMdl_MapAttrGrassProc_1( mmdl, &data );
      MMdl_MapAttrFootMarkProc_1( mmdl, &data );
      MMdl_MapAttrSplashProc_12( mmdl, &data );
      MMdl_MapAttrShadowProc_01( mmdl, &data );
      MMdl_MapAttrPoolProc_1( mmdl, &data );
      MMdl_MapAttrReflect_01( mmdl, &data );
    
      MMdl_MapAttrSEProc_1( mmdl, &data ); //描画関係ない?
    }
  }
}

//--------------------------------------------------------------
/**
 * 動作開始で発生するアトリビュート処理 2nd Jump
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) == 0 ){
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      ATTRDATA data;
      mmdl_InitAttrData( mmdl, &data );
    
      MMdl_MapAttrShadowProc_01( mmdl, &data );
      MMdl_MapAttrReflect_01( mmdl, &data );
      MMdl_MapAttrSplashProc_Jump1( mmdl, &data );

      MMdl_MapAttrSEProc_1( mmdl, &data );//描画関係ない?
    }
  }
}

//--------------------------------------------------------------
/**
 * 動作終了で発生するアトリビュート処理　end
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEnd( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) == 0 ){
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      ATTRDATA data;
      mmdl_InitAttrData( mmdl, &data );
    
      //終了　アトリビュート処理
      MMdl_MapAttrHeight_02( mmdl, &data );
      MMdl_MapAttrGrassProc_2( mmdl, &data );
      MMdl_MapAttrPoolProc_2( mmdl, &data );
      MMdl_MapAttrSplashProc_12( mmdl, &data );
      MMdl_MapAttrReflect_2( mmdl, &data );
      MMdl_MapAttrShadowProc_2( mmdl, &data );
      MMdl_MapAttrBiriBiri_2( mmdl, &data );
    }
  }
}

//--------------------------------------------------------------
/**
 * 動作終了で発生するアトリビュート処理　end jump
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEndJump( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) == 0 ){
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      ATTRDATA data;
      mmdl_InitAttrData( mmdl, &data );
    
      //終了　アトリビュート処理
      MMdl_MapAttrHeight_02( mmdl, &data );
      MMdl_MapAttrPoolProc_2( mmdl, &data );
      MMdl_MapAttrSplashProc_12( mmdl, &data );
      MMdl_MapAttrReflect_2( mmdl, &data );
      MMdl_MapAttrShadowProc_2( mmdl, &data );
      MMdl_MapAttrGrassProc_Jump2( mmdl, &data );
      MMdl_MapAttrGroundSmokeProc_2( mmdl, &data );
      MMdl_MapAttrBiriBiri_2( mmdl, &data );
    }
  }
}

//======================================================================
//  アトリビュート可変高さ
//======================================================================
//--------------------------------------------------------------
/**
 * アトリビュート可変高さ　動作開始、動作終了
 * @param  mmdl    MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrHeight_02( MMDL *mmdl, ATTRDATA *data )
{
  if( MMDL_CheckStatusBitAttrOffsetOFF(mmdl) == FALSE ){
    if( MAPATTR_VALUE_CheckDesertDeep(data->attr_val_now) ){
      VecFx32 offs = { 0, ATTROFFS_Y_DESERT_DEEP, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &offs );
      return;
    }
  }
  
  {
    VecFx32 vec = { 0, 0, 0 };
    MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
  }
}

#if 0 //old
static void MMdl_MapAttrHeight_02(
    MMDL * mmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  if( MMDL_CheckStatusBitAttrOffsetOFF(mmdl) == FALSE ){
    if( MAPATTR_IsSwampDeep(now) == TRUE ||
      MAPATTR_IsSwampGrassDeep(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_NUMA_DEEP, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
    
    if( MAPATTR_IsSwamp(now) == TRUE || MAPATTR_IsSwampGrass(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_NUMA, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
    
    if( MAPATTR_IsSnowDeepMost(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP_MOST, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
    
    if( MAPATTR_IsSnowDeep(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
    
    if( MAPATTR_IsShallowSnow(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_YUKI, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
  }
  
  {
    VecFx32 vec = { 0, 0, 0 };
    MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
  }
}
#endif

//======================================================================
//  アトリビュート　草
//======================================================================
//--------------------------------------------------------------
/**
 * 草　タイプ返し
 * @param val ATTR_VALUE
 * @retval FLDEFF_GRASSTYPE
 */
//--------------------------------------------------------------
static FLDEFF_GRASSTYPE getGrassType( MAPATTR_VALUE val, int season )
{
  FLDEFF_GRASSTYPE type = FLDEFF_GRASS_SHORT;
  
  if( MAPATTR_VALUE_CheckGrassAllYearLow(val) ){ //季節変化無し
    type = FLDEFF_GRASS_ALLYEAR_SHORT;
  }else if( MAPATTR_VALUE_CheckGrassAllYearHigh(val) ){ //季節変化無し
    type = FLDEFF_GRASS_ALLYEAR_SHORT2;
  }else if( MAPATTR_VALUE_CheckSnowGrassHigh(val) ){ //冬のみ特殊
    type = FLDEFF_GRASS_SHORT2;
    
    if( season == PMSEASON_WINTER ){
      type = FLDEFF_GRASS_SNOW2;
    }
  }else if( MAPATTR_VALUE_CheckSnowGrassLow(val) ){ //冬のみ特殊
    type = FLDEFF_GRASS_SHORT;
    
    if( season == PMSEASON_WINTER ){
      type = FLDEFF_GRASS_SNOW;
    }
  }else if( MAPATTR_VALUE_CheckEncountShortGrassHigh(val) ){
    type = FLDEFF_GRASS_SHORT2;
  }else if( MAPATTR_VALUE_CheckEncountLongGrassLow(val) ){
    type = FLDEFF_GRASS_LONG;
  }else if( MAPATTR_VALUE_CheckEncountLongGrassHigh(val) ){
    type = FLDEFF_GRASS_LONG2;
  }
  
  return( type );
}

//--------------------------------------------------------------
/**
 * 草　動作開始 0
 * @param  mmdl    MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_0( MMDL *mmdl, ATTRDATA *data )
{
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_GRASS) ){
    FLDEFF_GRASSTYPE type = getGrassType( data->attr_val_now, data->season );
    FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, FALSE, type );
  }
}

//--------------------------------------------------------------
/**
 * 草　動作 1
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_1( MMDL *mmdl, ATTRDATA *data )
{
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_GRASS) )
  {
    BOOL anm = TRUE;
    u16 dir = MMDL_GetDirMove( mmdl );
    FLDEFF_GRASSTYPE type = getGrassType( data->attr_val_now, data->season );
    
    #if 1 //BTS4949　草むらを歩いたとき不自然
    if( dir == DIR_UP ){
      return; //上方向への移動の際は終了時にセットする
    }
    #endif
    
    FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, anm, type );
  }
}

#if 0
static void MMdl_MapAttrGrassProc_1( MMDL *mmdl, ATTRDATA *data )
{
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_GRASS) ){
    BOOL anm = TRUE;
    FLDEFF_GRASSTYPE type = getGrassType( data->attr_val_now, data->season );
    FLDEFF_GRASSLEN len = FLDEFF_GRASS_GetLenType( type );
    
    if( len == FLDEFF_GRASSLEN_SHORT ){
      if( MMDL_GetDirMove(mmdl) == DIR_UP ){
        anm = FALSE;
      }
    }else{ //long
      if( MMDL_GetDirMove(mmdl) == DIR_UP ){
        return; //上方向は終了時にセットする
      }
    }
    
    FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, anm, type );
  }
}
#endif

//--------------------------------------------------------------
/**
 * 草　動作 2
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_2( MMDL *mmdl, ATTRDATA *data )
{
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_GRASS) )
  {
    BOOL anm = TRUE;
    u16 dir = MMDL_GetDirMove( mmdl );
    FLDEFF_GRASSTYPE type = getGrassType( data->attr_val_now, data->season );
    
    #if 1 //BTS4949　草むらを歩いたとき不自然
    //上方向への移動の際は終了時にセットする
    if( dir == DIR_UP ){
      FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, anm, type );
    }
    #endif
  }
}

#if 0
static void MMdl_MapAttrGrassProc_2( MMDL *mmdl, ATTRDATA *data )
{
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_GRASS) ){
    FLDEFF_GRASSTYPE type = getGrassType(
        data->attr_val_now, data->season );
    FLDEFF_GRASSLEN len = FLDEFF_GRASS_GetLenType( type );
    
    if( len == FLDEFF_GRASSLEN_LONG ){
      if( MMDL_GetDirMove(mmdl) == DIR_UP ){
        FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, TRUE, type );
      }
    }
  }
}
#endif

//--------------------------------------------------------------
/**
 * 草　動作 ジャンプ 2
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_Jump2( MMDL *mmdl, ATTRDATA *data )
{
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_GRASS) ){
    FLDEFF_GRASSTYPE type = getGrassType( data->attr_val_now, data->season );
    FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, TRUE, type );
  }
}



//======================================================================
//  アトリビュート　足跡
//======================================================================
//--------------------------------------------------------------
/**
 * 足跡　動作開始 1
 * @param  mmdl  MMDL *
 * @param  now    現在のアトリビュート
 * @param  old    過去のアトリビュート
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrFootMarkProc_1( MMDL *mmdl, ATTRDATA *data )
{
  if( (data->attr_flag_old & MAPATTR_FLAGBIT_FOOTMARK) )
  {
    FOOTMARK_TYPE type = FOOTMARK_TYPE_MAX;
    
    if( MAPATTR_VALUE_CheckSeasonGround1(data->attr_val_old) )
    {
      if( data->season == PMSEASON_WINTER )
      {
        type = FOOTMARK_TYPE_HUMAN_SNOW;
      
        if( data->objcode_prm->footmark_type == MMDL_FOOTMARK_CYCLE )
        {
          type = FOOTMARK_TYPE_CYCLE_SNOW;
        }
      }
    }
    else if( MAPATTR_VALUE_CheckSeasonGround2(data->attr_val_old) )
    {
      if( data->season == PMSEASON_SPRING ||
          data->season == PMSEASON_WINTER )
      {
        type = FOOTMARK_TYPE_HUMAN_SNOW;
      
        if( data->objcode_prm->footmark_type == MMDL_FOOTMARK_CYCLE )
        {
          type = FOOTMARK_TYPE_CYCLE_SNOW;
        }
      }
    }
    else if( MAPATTR_VALUE_CheckSnowType(data->attr_val_old) == TRUE ||
        MAPATTR_VALUE_CheckSnowNotCycle(data->attr_val_old) == TRUE )
    {
      type = FOOTMARK_TYPE_HUMAN_SNOW;
      
      if( data->objcode_prm->footmark_type == MMDL_FOOTMARK_CYCLE )
      {
        type = FOOTMARK_TYPE_CYCLE_SNOW;
      }
    }
    else if( MAPATTR_VALUE_CheckDesertDeep(data->attr_val_old) == TRUE )
    {
      type = FOOTMARK_TYPE_DEEPSNOW;       
      
      #if 0 //自機の足跡SE処理でのみ鳴らす
      PMSND_PlaySE( SEQ_SE_FLD_91 );
      #endif
    }
    else
    {
      type = FOOTMARK_TYPE_HUMAN;
      
      if( data->objcode_prm->footmark_type == MMDL_FOOTMARK_CYCLE )
      {
        type = FOOTMARK_TYPE_CYCLE;
      }
    }
     
    if( type != FOOTMARK_TYPE_MAX ){
      FLDEFF_FOOTMARK_SetMMdl( mmdl, data->fectrl, type );
    }
  }
}

//======================================================================
//  アトリビュート　水飛沫
//======================================================================
//--------------------------------------------------------------
/**
 * 水飛沫　動作開始 0
 * @param  mmdl  MMDL 
 * @param  now    現在のアトリビュート
 * @param  old    過去のアトリビュート
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_0( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckShoal(data->attr_val_now) == TRUE ){
    if( MMDL_CheckMoveBitShoalEffect(mmdl) == FALSE ){
      FLDEFF_SHOAL_SetMMdl( data->fectrl, mmdl );
      MMDL_SetMoveBitShoalEffect( mmdl, TRUE );
    }
  }else{
    MMDL_SetMoveBitShoalEffect( mmdl, FALSE );
  }
}

//--------------------------------------------------------------
/**
 * 水飛沫　動作開始終了 12
 * @param  mmdl  MMDL *
 * @param  now    現在のアトリビュート
 * @param  old    過去のアトリビュート
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_12( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckShoal(data->attr_val_now) == TRUE ){
    if( MMDL_CheckMoveBitShoalEffect(mmdl) == FALSE ){
      FLDEFF_SHOAL_SetMMdl( data->fectrl, mmdl );
      MMDL_SetMoveBitShoalEffect( mmdl, TRUE );
    }
  }else if( MAPATTR_VALUE_CheckMarsh(data->attr_val_now) == TRUE ){
    FLDEFF_SPLASH_SetMMdl( data->fectrl, mmdl );
  }else{
    MMDL_SetMoveBitShoalEffect( mmdl, FALSE );
  }
}

//--------------------------------------------------------------
/**
 * 水飛沫　ジャンプ動作開始 1
 * @param  mmdl  MMDL *
 * @param  now    現在のアトリビュート
 * @param  old    過去のアトリビュート
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_Jump1( MMDL *mmdl, ATTRDATA *data )
{
  MMDL_SetMoveBitShoalEffect( mmdl, FALSE );
}

//======================================================================
//  アトリビュート　影
//======================================================================
//--------------------------------------------------------------
/**
 * 影　表示許可判定
 * @param mmdl  MMDL *
 * @param data  ATTRDATA
 * @retval BOOL TRUE=影を表示
 */
//--------------------------------------------------------------
static BOOL mmdl_CheckShadowUse( const MMDL *mmdl, const ATTRDATA *data )
{
  const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
  
  if( MMDLSYS_CheckJoinShadow(fos) == FALSE ||
      data->objcode_prm->shadow_type == MMDL_SHADOW_NON )
  {
    return FALSE;
  }
  return TRUE;
}
//--------------------------------------------------------------
/**
 * 影　表示許可Attr判定
 * @param mmdl  MMDL *
 * @param data  ATTRDATA
 * @retval BOOL TRUE=影を表示
 */
//--------------------------------------------------------------
static BOOL mmdl_CheckShadowAttr( const ATTRDATA *data )
{
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_SHADOW) == 0 )
  {
    return FALSE;
  }
  //季節変化地面＆冬
  if( data->season == PMSEASON_WINTER &&
        MAPATTR_VALUE_CheckSeasonGround1(data->attr_val_now) )
  {
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 影　動作開始 0,1
 * @param  mmdl  MMDL *
 * @param  now    現在のアトリビュート
 * @param  old    過去のアトリビュート
 * @retval  nothing
 * @note 影をセット。影セット済みの場合は何もしない。
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_01( MMDL *mmdl, ATTRDATA *data )
{
  if( mmdl_CheckShadowUse( mmdl, data ) == TRUE &&
      mmdl_CheckShadowAttr( data ) == TRUE &&
      MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_SHADOW_SET) == 0 )
  {
    FLDEFF_SHADOW_SetMMdl( mmdl, data->fectrl );
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_SET );
  }
}

//--------------------------------------------------------------
/**
 * 影　動作終了 2
 * @param  mmdl  MMDL *
 * @param  now    現在のアトリビュート
 * @param  old    過去のアトリビュート
 * @retval  nothing
 * @note 影セットは判定せず影非表示フラグのON,OFFのみ。
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_2( MMDL *mmdl, ATTRDATA *data )
{
  if( mmdl_CheckShadowUse( mmdl, data ) == TRUE )
  {
    if( mmdl_CheckShadowAttr( data ) == FALSE )
    {
      MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH );
    }
    else
    {
      MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH );
    }
  }
}

//======================================================================
//  アトリビュート　土煙
//======================================================================
//--------------------------------------------------------------
/**
 * 土煙　動作終了 2 
 * @param  mmdl  MMDL *
 * @param  now    現在のアトリビュート
 * @param  old    過去のアトリビュート
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGroundSmokeProc_2( MMDL *mmdl, ATTRDATA *data )
{
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_WATER) == 0 ){
    if( MMDL_CheckMoveBitNonCreateMoveEffect(mmdl) == FALSE ){
      FLDEFF_KEMURI_SetMMdl( mmdl, data->fectrl );
    }
  }
}

//======================================================================
//  アトリビュート　長い草
//======================================================================
#if 0 //old
//--------------------------------------------------------------
/**
 * 長い草　動作開始 0
 * @param  mmdl    MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrLGrassProc_0( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_IsLongGrass(now) == TRUE ){
    FE_mmdlLGrass_Add( mmdl, FALSE );
  }
}

//--------------------------------------------------------------
/**
 * 長い草　動作開始 1
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrLGrassProc_1( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_IsLongGrass(now) == TRUE ){
    FE_mmdlLGrass_Add( mmdl, TRUE );
  }
}
#endif

//======================================================================
//  アトリビュート　沼草
//======================================================================
#if 0 //old
//--------------------------------------------------------------
/**
 * 沼草　動作開始 0
 * @param  mmdl    MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrNGrassProc_0( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_IsSwampGrass(now) == TRUE ){
    FE_mmdlNGrass_Add( mmdl, FALSE );
  }
}

//--------------------------------------------------------------
/**
 * 沼草　動作開始 1
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrNGrassProc_1( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_IsSwampGrass(now) == TRUE ){
    FE_mmdlNGrass_Add( mmdl, TRUE );
  }
}
#endif

//======================================================================
//  アトリビュート　水たまり
//======================================================================
//--------------------------------------------------------------
/**
 * 水たまり　動作開始 1
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrPoolProc_1( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckPool(data->attr_val_old) == TRUE ){
    FLDEFF_RIPPLE_Set( data->fectrl,
      MMDL_GetOldGridPosX(mmdl),
      MMDL_GetOldGridPosZ(mmdl),
      MMDL_GetVectorPosY(mmdl) );
  }
}

//--------------------------------------------------------------
/**
 * 水たまり　動作終了 2
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrPoolProc_2( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckPool(data->attr_val_now) == TRUE ){
    FLDEFF_RIPPLE_Set( data->fectrl,
      MMDL_GetGridPosX(mmdl),
      MMDL_GetGridPosZ(mmdl),
      MMDL_GetVectorPosY(mmdl) );
  }
}

//======================================================================
//  アトリビュート　沼
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * 沼　動作開始 1
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSwampProc_1( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_IsSwamp(old) == TRUE ){
    FE_mmdlNRippleSet( mmdl,
      MMDL_GetOldGridPosX(mmdl),
      MMDL_GetOldGridPosY(mmdl), 
      MMDL_GetOldGridPosZ(mmdl) );
  }
}

//--------------------------------------------------------------
/**
 * 沼　動作終了 2
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSwampProc_2( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_IsSwamp(now) == TRUE ){
    FE_mmdlNRippleSet( mmdl,
      MMDL_GetGridPosX(mmdl),
      MMDL_GetGridPosY(mmdl), 
      MMDL_GetGridPosZ(mmdl) );
  }
}
#endif

//======================================================================
//  アトリビュート　映りこみ
//======================================================================
//--------------------------------------------------------------
/**
 * 映りこみ　動作開始 01
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_01( MMDL *mmdl, ATTRDATA *data )
{
  if( data->objcode_prm->reflect_type == MMDL_REFLECT_NON ){
    return;
  }
  
  if( MMDL_CheckMoveBitReflect(mmdl) == FALSE )
  {
    MAPATTR attr = MAPATTR_ERROR;
    
    if( (data->attr_flag_now & MAPATTR_FLAGBIT_REFLECT) )
    {
      attr = data->attr_now;
    }
    else
    {
      MAPATTR next = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( next );
      
      if( (flag & MAPATTR_FLAGBIT_REFLECT) )
      {
        attr = next;
      }
    }
    
    if( attr != MAPATTR_ERROR )
    {
      REFLECT_TYPE type = REFLECT_TYPE_POND;
      MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
      MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
       
      if( MAPATTR_VALUE_CheckMirrorFloor(val) == TRUE ||
          MAPATTR_VALUE_CheckIce02(val) == TRUE ){
        type = REFLECT_TYPE_MIRROR;
      }
      
      FLDEFF_REFLECT_SetMMdl( mmdlsys, mmdl, data->fectrl, type );
      MMDL_SetMoveBitReflect( mmdl, TRUE );
    }
  }
}

//--------------------------------------------------------------
/**
 * 映りこみ　動作終了 2
 * @param  mmdl  MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_2( MMDL *mmdl, ATTRDATA *data )
{
  if( data->objcode_prm->reflect_type == MMDL_REFLECT_NON ||
    MMDL_CheckMoveBitReflect(mmdl) == FALSE ){
    return;
  }
  
  {
    MAPATTR_FLAG flag = MAPATTR_FLAGBIT_NONE;
    MAPATTR attr = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
    
    if( attr == MAPATTR_ERROR ){
      MMDL_SetMoveBitReflect( mmdl, FALSE );
    }else{
      flag = MAPATTR_GetAttrFlag( attr );
      
      if( (flag & MAPATTR_FLAGBIT_REFLECT) == 0 ){
        MMDL_SetMoveBitReflect( mmdl, FALSE );
      }
    }
  }
}

//======================================================================
//  アトリビュート　ビリビリ床
//======================================================================
//--------------------------------------------------------------
/**
 * ビリビリ床　動作終了 2
 * @param  mmdl    MMDL *
 * @param  now      現在のアトリビュート
 * @param  old      過去のアトリビュート
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrBiriBiri_2( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckElecFloor(data->attr_val_now) == TRUE ){
    FLDEFF_D06DENKI_BIRIBIRI_SetMMdl( mmdl, data->fectrl );
  }
}

//======================================================================
//  アトリビュート関連SE
//======================================================================
//--------------------------------------------------------------
/**
 * SE　動作開始 1
 * @param  mmdl  MMDL *
 * @param  now    現在のアトリビュート
 * @param  old    過去のアトリビュート
 * @param  param    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSEProc_1( MMDL *mmdl, ATTRDATA *data )
{
}

//======================================================================
//  移動チェック
//======================================================================
//--------------------------------------------------------------
/**
 * 指定座標障害物チェック
 * @param  fos    MMDLSYS *
 * @param  x    チェックするX座標  グリッド
 * @param  z    チェックするZ座標  グリッド
 * @retval  u32    ヒットビット。MMDL_MOVEHITBIT_LIM等
 */
//--------------------------------------------------------------
#if 0
u32 MMDL_PosHitCheck( const MMDLSYS *fos, int x, int z )
{
  u32 ret;
  
  ret = MMDL_MOVEHITBIT_NON;
  
  if(  GetHitAttr(MMDLSYS_FieldSysWorkGet(fos),x,z) == TRUE ){
    ret |= MMDL_MOVEHITBIT_ATTR;
  }
  
  if( MMDLSYS_SearchGridPos(fos,x,z,FALSE) != NULL ){
    ret |= MMDL_MOVEHITBIT_OBJ;
  }
  
  return( ret );
}
#endif

//--------------------------------------------------------------
/**
 * フィールド動作モデル移動チェック
 * @param  mmdl  MMDL *
 * @param  vec    現在実座標
 * @param  x    移動先X座標  グリッド
 * @param  y    移動先Y座標　グリッド
 * @param  z    移動先Z座標  グリッド
 * @param  dir    移動方向 DIR_UP等
 * @retval  u32    ヒットビット。MMDL_MOVEHITBIT_LIM等
 */
//--------------------------------------------------------------
u32 MMDL_HitCheckMove(
  const MMDL *mmdl, const VecFx32 *vec, s16 x, s16 y, s16 z, u16 dir )
{
  u32 ret;
  u32 attr;
  fx32 height;
  VecFx32 pos;
  
  ret = MMDL_MOVEHITBIT_NON;
  
  pos.x = GRID_SIZE_FX32( x ) + GRID_HALF_FX32;
  pos.y = vec->y; //GRID_SIZE_FX32( y );
  pos.z = GRID_SIZE_FX32( z ) + GRID_HALF_FX32;
    
  if( MMDL_HitCheckMoveLimit(mmdl,x,z) == TRUE ){
    ret |= MMDL_MOVEHITBIT_LIM;
  }
  
  if( MMdl_HitCheckMoveAttr(mmdl,dir,pos) == TRUE ){
    ret |= MMDL_MOVEHITBIT_ATTR;
  }
  
  if( MMDL_GetMapPosHeight(mmdl,&pos,&height) == TRUE ){
    fx32 diff = vec->y - height;
    if( diff < 0 ){ diff = -diff; }
    if( diff >= HEIGHT_DIFF_COLLISION ){
      ret |= MMDL_MOVEHITBIT_HEIGHT;
    }
    
    y = SIZE_GRID_FX32( height ); //高さが取れた際は移動先の高さで更新
  }else{
    ret |= MMDL_MOVEHITBIT_HEIGHT;
  }
  
  if( MMDL_HitCheckMoveFellow(mmdl,x,y,z) == TRUE ){
    ret |= MMDL_MOVEHITBIT_OBJ;
  }
  
  {
    const FLDMAPPER *pG3DMapper;
    
    pG3DMapper = MMDLSYS_GetG3DMapper( MMDL_GetMMdlSys(mmdl) );
    
    //pos->yはCheckOutRange()内では未評価<090916時点>
    if( FLDMAPPER_CheckOutRange(pG3DMapper,&pos) == TRUE ){
      ret |= MMDL_MOVEHITBIT_OUTRANGE;
    }
  }

  return( ret );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデル移動チェック　現在位置から判定
 * @param  mmdl  MMDL * 
 * @param  x    移動先X座標  グリッド
 * @param  y    移動先Y座標
 * @param  z    移動先Z座標  グリッド
 * @param  dir    移動方向 DIR_UP等
 * @retval  u32    ヒットビット。MMDL_MOVEHITBIT_LIM等
 */
//--------------------------------------------------------------
u32 MMDL_HitCheckMoveCurrent(
  const MMDL * mmdl, s16 x, s16 y, s16 z, u16 dir )
{
  VecFx32 vec;
  MMDL_GetVectorPos( mmdl, &vec );
  return( MMDL_HitCheckMove(mmdl,&vec,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデル移動チェック　現在位置+方向版
 * @param  mmdl  MMDL * 
 * @param  dir    移動する方向。DIR_UP等
 * @retval  u32    ヒットビット。MMDL_MOVEHITBIT_LIM等
 */
//--------------------------------------------------------------
u32 MMDL_HitCheckMoveDir( const MMDL * mmdl, u16 dir )
{
  s16 x,y,z;
  x = MMDL_GetGridPosX( mmdl ) + MMDL_TOOL_GetDirAddValueGridX( dir );
  y = MMDL_GetGridPosY( mmdl );
  z = MMDL_GetGridPosZ( mmdl ) + MMDL_TOOL_GetDirAddValueGridZ( dir );
  return( MMDL_HitCheckMoveCurrent(mmdl,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * 動作モデル同士のヒットチェック用矩形作成
 * @param mmdl 対象となるMMDL
 * @param x 始点となるX座標 グリッド
 * @param z 始点となるZ座標 グリッド
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_CreateHitCheckRect(
    const MMDL *mmdl, s16 x, s16 z, MMDL_RECT *rect )
{
  s16 size;
  
  size = (s16)MMDL_GetGridSizeX( mmdl ) - 1; //1origin
  if( size < 0 ){
    size = 0;
  }
  
  rect->left = x;
  rect->right = x + size;
  
  size = (s16)MMDL_GetGridSizeZ( mmdl ) - 1; //1origin
  if( size < 0 ){
    size = 0;
  }
  
  rect->top = z - size; //始点は左下
  rect->bottom = z;
}

//--------------------------------------------------------------
/**
 * MMDL_RECT同士のヒットチェック
 * @param rect0 チェックするMMDL_RECT
 * @param rect1 チェックするMMDL_RECT
 * @retval BOOL TRUE=ヒット
 */
//--------------------------------------------------------------
BOOL MMDL_HitCheckRect( const MMDL_RECT *rect0, const MMDL_RECT *rect1 )
{
  s16 size0,size1;
  MMDL_RECT h0,h1;
  
  size0 = rect0->right - rect0->left;
  if( size0 < 0 ){ size0 = -size0; }
  size1 = rect1->right - rect1->left;
  if( size1 < 0 ){ size1 = -size1; }
  
  if( size0 > size1 ){
    h0.left = rect0->left;
    h0.right = rect0->right;
    h1.left = rect1->left;
    h1.right = rect1->right;
  }else{
    h0.left = rect1->left;
    h0.right = rect1->right;
    h1.left = rect0->left;
    h1.right = rect0->right;
  }
  
  if( (h0.left <= h1.left && h0.right >= h1.left) ||
      (h0.left <= h1.right && h0.right >= h1.right) ){
    size0 = rect0->bottom - rect0->top;
    if( size0 < 0 ){ size0 = -size0; }
    size1 = rect1->bottom - rect1->top;
    if( size1 < 0 ){ size1 = -size1; }
    
    if( size0 > size1 ){
      h0.top = rect0->top;
      h0.bottom = rect0->bottom;
      h1.top = rect1->top;
      h1.bottom = rect1->bottom;
    }else{
      h0.top = rect1->top;
      h0.bottom = rect1->bottom;
      h1.top = rect0->top;
      h1.bottom = rect0->bottom;
    }
    
    if( (h0.top <= h1.top && h0.bottom >= h1.top) ||
        (h0.top <= h1.bottom && h0.bottom >= h1.bottom) ){
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデル同士の衝突チェック
 * @param mmdl0 MMDL* 判定元MMDL
 * @param mmdl1 MMDL* 判定対象MMDL
 * @param x0  判定する始点X座標  グリッド
 * @param y0  判定する始点Y座標  グリッド
 * @param z0  判定する始点Z座標  グリッド
 * @param old_hit TRUE=mmdl1の過去座標も判定する
 * @retval  BOOL  TRUE=衝突アリ
 */
//--------------------------------------------------------------
BOOL MMDL_HitCheckFellow( const MMDL *mmdl0, const MMDL *mmdl1,
    s16 x0, s16 y0, s16 z0, BOOL old_hit )
{
  s16 x1,y1,z1,sy;
  MMDL_RECT rect0,rect1;
  
  MMDL_CreateHitCheckRect( mmdl0, x0, z0, &rect0 );
  
  if( MMDL_CheckStatusBit(mmdl1,MMDL_STABIT_FELLOW_HIT_NON) == 0 )
  {
    x1 = MMDL_GetGridPosX( mmdl1 );
    z1 = MMDL_GetGridPosZ( mmdl1 );
    MMDL_CreateHitCheckRect( mmdl1, x1, z1, &rect1 );
        
    if( MMDL_HitCheckRect(&rect0,&rect1) )
    {
      y1 = MMDL_GetGridPosY( mmdl1 );
      sy = y0 - y1;
        
      if( sy < 0 )
      {
        sy = -sy;
      }
    
      if( sy < H_GRID_FELLOW_SIZE )
      {
        return( TRUE );
      }
    }
    
    if( old_hit == TRUE )
    {
      x1 = MMDL_GetOldGridPosX( mmdl1 );
      z1 = MMDL_GetOldGridPosZ( mmdl1 );
      MMDL_CreateHitCheckRect( mmdl1, x1, z1, &rect1 );
      
      if( MMDL_HitCheckRect(&rect0,&rect1) )
      {
        y1 = MMDL_GetOldGridPosY( mmdl1 );
        sy = y0 - y1;
        
        if( sy < 0 )
        {
          sy = -sy;
        }
            
        if( sy < H_GRID_FELLOW_SIZE )
        {
          return( TRUE );
        }
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデル同士の衝突チェック
 * @param  mmdl  MMDL * 
 * @param  x    移動先X座標  グリッド
 * @param  y    移動先X座標  グリッド
 * @param  z    移動先X座標  グリッド
 * @retval  BOOL  TRUE=衝突アリ
 */
//--------------------------------------------------------------
BOOL MMDL_HitCheckMoveFellow( const MMDL *mmdl0, s16 x0, s16 y0, s16 z0 )
{
  u32 no=0;
  MMDL *mmdl1;
  const MMDLSYS *sys = MMDL_GetMMdlSys( mmdl0 );
  
  while( MMDLSYS_SearchUseMMdl(sys,&mmdl1,&no) == TRUE ){
    if( mmdl0 != mmdl1 ){
      if( MMDL_HitCheckFellow(mmdl0,mmdl1,x0,y0,z0,TRUE) == TRUE ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

#if 0 //old
BOOL MMDL_HitCheckMoveFellow( const MMDL * mmdl, s16 x, s16 y, s16 z )
{
  u32 no=0;
  int hx,hz;
  MMDL *cmmdl;
  const MMDLSYS *sys;
  
  sys = MMDL_GetMMdlSys( mmdl );
  
  while( MMDLSYS_SearchUseMMdl(sys,&cmmdl,&no) == TRUE ){
    if( cmmdl != mmdl ){
      if( MMDL_CheckStatusBit(
        cmmdl,MMDL_STABIT_FELLOW_HIT_NON) == 0 ){
        hx = MMDL_GetGridPosX( cmmdl );
        hz = MMDL_GetGridPosZ( cmmdl );
        {
          BOOL debug = FALSE;
          if( MMDL_GetOBJID(mmdl) == MMDL_ID_PLAYER ){
            debug = TRUE;
          }
          if( hx == x && hz == z ){
            int hy = MMDL_GetGridPosY( cmmdl );
            int sy = hy - y;
            if( sy < 0 ){ sy = -sy; }
            if( sy < H_GRID_FELLOW_SIZE ){
              return( TRUE );
            }
          }
        
          hx = MMDL_GetOldGridPosX( cmmdl );
          hz = MMDL_GetOldGridPosZ( cmmdl );
        
          if( hx == x && hz == z ){
            int hy = MMDL_GetGridPosY( cmmdl );
            int sy = hy - y;
            if( sy < 0 ){ sy = -sy; }
            if( sy < H_GRID_FELLOW_SIZE ){
              return( TRUE );
            }
          }
        }
      }
    }
  }
  
  return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * 動作モデルとX,Z座標のヒットチェック
 * @param
 * @retval
 */
//--------------------------------------------------------------
BOOL MMDL_HitCheckXZ( const MMDL *mmdl, s16 x0, s16 z0, BOOL old_hit )
{
  MMDL_RECT rect;
  s16 x1 = MMDL_GetGridPosX( mmdl );
  s16 z1 = MMDL_GetGridPosZ( mmdl );
  MMDL_CreateHitCheckRect( mmdl, x1, z1, &rect );
  
  if( rect.left <= x0 && rect.right >= x0 ){
    if( rect.top <= z0 && rect.bottom >= z0 ){
      return( TRUE );
    }
  }
  
  if( old_hit == TRUE ){
    x1 = MMDL_GetOldGridPosX( mmdl );
    z1 = MMDL_GetOldGridPosZ( mmdl );
    MMDL_CreateHitCheckRect( mmdl, x1, z1, &rect );
    
    if( rect.left <= x0 && rect.right >= x0 ){
      if( rect.top <= z0 && rect.bottom >= z0 ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデル移動制限チェック
 * @param  mmdl  MMDL * 
 * @param  x    移動先X座標  グリッド
 * @param  y    移動先Y座標  グリッド
 * @param  z    移動先Z座標  グリッド
 * @retval  BOOL  TRUE=制限越え
 */
//--------------------------------------------------------------
BOOL MMDL_HitCheckMoveLimit( const MMDL * mmdl, s16 x, s16 z )
{
  s16 init,limit,min,max;
  
  init = MMDL_GetInitGridPosX( mmdl );
  limit = MMDL_GetMoveLimitX( mmdl );
  
  if( limit != MOVE_LIMIT_NOT ){
    min = init - limit;
    max = init + limit;
  
    if( min > x || max < x ){
      return( TRUE );
    }
  }
  
  init = MMDL_GetInitGridPosZ( mmdl );
  limit = MMDL_GetMoveLimitZ( mmdl );
  
  if( limit != MOVE_LIMIT_NOT ){
    min = init - limit;
    max = init + limit;
  
    if( min > z || max < z ){
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * フィールド動作モデルアトリビュートヒットチェック
 * @param  mmdl  MMDL * 
 * @param  dir    移動方向 DIR_UP等
 * @param  pos    移動先X座標,現在位置のY,移動先Z座標
 * @retval  int    TRUE=移動不可アトリビュート
 */
//--------------------------------------------------------------
static BOOL MMdl_HitCheckMoveAttr(
  const MMDL * mmdl, u16 dir, const VecFx32 pos )
{
  if( MMDL_CheckStatusBitAttrGetOFF(mmdl) == FALSE ){
    BOOL first;
    MAPATTR attr,n_attr;
    VecFx32 pos0,pos1;
    u8 x0,x1,z0,z1;
    
    first = FALSE;
    x1 = MMDL_GetGridSizeX( mmdl );
    z1 = MMDL_GetGridSizeZ( mmdl );
    pos0 = pos;
    
    for( z0 = 0; z0 < z1; z0++, pos0.z -= GRID_FX32 )
    {
      for( x0 = 0, pos1 = pos0; x0 < x1; x0++, pos1.x += GRID_FX32 )
      {
        if( MMDL_GetMapPosAttr(mmdl,&pos1,&attr) == FALSE )
        {
          return( TRUE );
        }
        
        if( MAPATTR_GetHitchFlag(attr) == TRUE )
        {
          return( TRUE );
        }
        
        if( first == FALSE ){
          first = TRUE;
          n_attr = attr;
        }
      }
    }
    
    {
      MAPATTR_VALUE val = MAPATTR_GetAttrValue( MMDL_GetMapAttr(mmdl) );
      
      if( DATA_HitCheckAttr_Now[dir](val) == TRUE ){
        return( TRUE );
      }

      val = MAPATTR_GetAttrValue( n_attr );
      
      if( DATA_HitCheckAttr_Next[dir](val) == TRUE ){
        return( TRUE );
      }
    }

    return( FALSE );
  }
  
  return( TRUE ); //移動不可アトリビュート
}

#if 0
static BOOL MMdl_HitCheckMoveAttr(
  const MMDL * mmdl, const VecFx32 pos )
{
  if( MMDL_CheckStatusBitAttrGetOFF(mmdl) == FALSE ){
    MAPATTR attr;
    VecFx32 pos0,pos1;
    u8 x0,x1,z0,z1;
    
    x1 = MMDL_GetGridSizeX( mmdl );
    z1 = MMDL_GetGridSizeZ( mmdl );
    pos0 = pos;
    
    for( z0 = 0; z0 < z1; z0++, pos0.z -= GRID_FX32 )
    {
      for( x0 = 0, pos1 = pos0; x0 < x1; x0++, pos1.x += GRID_FX32 )
      {
        if( MMDL_GetMapPosAttr(mmdl,&pos1,&attr) == FALSE )
        {
          return( TRUE );
        }
        
        if( MAPATTR_GetHitchFlag(attr) == TRUE )
        {
          return( TRUE );
        }
      }
    }
    
    return( FALSE );
  }
  
  return( TRUE ); //移動不可アトリビュート
}
#endif

//--------------------------------------------------------------
///  現在位置アトリビュートから判定する移動制御アトリビュートチェック関数
//--------------------------------------------------------------
static BOOL (* const DATA_HitCheckAttr_Now[DIR_MAX4])( MAPATTR_VALUE val ) =
{
  MAPATTR_VALUE_CheckNotMoveUp,
  MAPATTR_VALUE_CheckNotMoveDown,
  MAPATTR_VALUE_CheckNotMoveLeft,
  MAPATTR_VALUE_CheckNotMoveRight,
};

//--------------------------------------------------------------
///  未来位置アトリビュートから判定する移動制御アトリビュートチェック関数
//--------------------------------------------------------------
static BOOL (* const DATA_HitCheckAttr_Next[DIR_MAX4])( MAPATTR_VALUE val ) =
{
  MAPATTR_VALUE_CheckNotMoveDown,
  MAPATTR_VALUE_CheckNotMoveUp,
  MAPATTR_VALUE_CheckNotMoveRight,
  MAPATTR_VALUE_CheckNotMoveLeft,
};

//--------------------------------------------------------------
/**
 * マップグリッド情報を取得
 * @param  mmdl  MMDL
 * @param  pos  取得する座標
 * @param  pGridInfo 情報格納先
 * @retval  BOOL FALSE=マップ情報が無い
 */
//--------------------------------------------------------------
#if 0
static BOOL MMdl_GetMapGridInfo(
  const MMDL *mmdl, const VecFx32 *pos, FLDMAPPER_GRIDINFO *pGridInfo )
{
  const FLDMAPPER *pG3DMapper =
    MMDLSYS_GetG3DMapper( MMDL_GetMMdlSys(mmdl) );
  
  if( FLDMAPPER_GetGridInfo(pG3DMapper,pos,pGridInfo) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * マップグリッド情報取得
 * @param  mmdl  MMDL
 * @param  pos 取得する座標(x,zに取得したい位置座標、yには現在のheightが格納されていること)
 * @param  pGridData  グリッドアトリビュートデータ格納先
 * @retval  0未満 情報が取得できない
 *
 *  @li GridInfoに取得された全階層をサーチし、現在のYに最も近いheight,attrを返します
 *  @li GridInfoの配列0の方がプライオリティが高いです
 */
//--------------------------------------------------------------
static int MMdl_GetMapPosGridData(
  const MMDL *mmdl, const VecFx32 *pos, FLDMAPPER_GRIDINFODATA* pGridData)
{
  const FLDMAPPER *pG3DMapper =
    MMDLSYS_GetG3DMapper( MMDL_GetMMdlSys(mmdl) );
  
#if 0  //どうしたものか
  if( FLDMAPPER_CheckTrans(pG3DMapper) == TRUE ){
    return( FLDMAPPER_GetGridData(pG3DMapper,pos,pGridData) );
  }
#else
  return( FLDMAPPER_GetGridData(pG3DMapper,pos,pGridData) );
#endif
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * マップアトリビュート取得
 * @param  mmdl  MMDL
 * @param  pos  取得する座標(x,zに取得したい位置座標、yには現在のheightが格納されていること)
 * @param  attr  アトリビュート格納先
 * @retval  BOOL  FALSE=アトリビュートが存在しない。またはロード中である。
 */
//--------------------------------------------------------------
BOOL MMDL_GetMapPosAttr(
  const MMDL *mmdl, const VecFx32 *pos, u32 *attr )
{
  FLDMAPPER_GRIDINFODATA gridData;
  *attr = 0;
  
  if( MMdl_GetMapPosGridData(mmdl,pos,&gridData) == TRUE ){
    *attr = gridData.attr;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * マップ高さ取得
 * @param  mmdl  MMDL
 * @param  pos  取得する座標
 * @param  height  高さ格納先
 * @retval  BOOL  FALSE=高さが存在しない。またはロード中である。
 */
//--------------------------------------------------------------
BOOL MMDL_GetMapPosHeight(
  const MMDL *mmdl, const VecFx32 *pos, fx32 *height )
{
#if 1
  FLDMAPPER_GRIDINFODATA gridData;
  
  *height = 0;
  
  if( MMdl_GetMapPosGridData(mmdl,pos,&gridData) == TRUE ){
    *height = gridData.height;
    return( TRUE );
  }
  
  return( FALSE );
#else
  FLDMAPPER_GRIDINFO GridInfo;
  *height = 0;

  if( MMdl_GetMapGridInfo(mmdl,pos,&GridInfo) == TRUE ){
    if( GridInfo.count ){
      int    i = 0;
      fx32  h_tmp,diff1,diff2;
      
      *height = GridInfo.gridData[i].height;
      i++;
      
      while( i < GridInfo.count ){
        h_tmp = GridInfo.gridData[i].height;
        diff1 = *height - pos->y;
        diff2 = h_tmp - pos->y;
        
        if( FX_Mul(diff2,diff2) < FX_Mul(diff1,diff1) ){
          *height = h_tmp;
        }
        i++;
      }
      
      return( TRUE );
    }
  }

  return( FALSE );
#endif
}

//======================================================================
//  グリッド座標移動系
//======================================================================
//--------------------------------------------------------------
/**
 * 方向で現在座標を更新。×高さは更新されない。<-するようにした
 * @param  mmdl    MMDL * 
 * @param  dir      移動方向
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateGridPosDir( MMDL * mmdl, u16 dir )
{
  MMDL_SetOldGridPosX( mmdl, MMDL_GetGridPosX(mmdl) );
  MMDL_SetOldGridPosY( mmdl, MMDL_GetGridPosY(mmdl) );
  MMDL_SetOldGridPosZ( mmdl, MMDL_GetGridPosZ(mmdl) );
  
  MMDL_AddGridPosX( mmdl, MMDL_TOOL_GetDirAddValueGridX(dir) );
  MMDL_AddGridPosZ( mmdl, MMDL_TOOL_GetDirAddValueGridZ(dir) );
  
  {
    VecFx32 pos;
    fx32 height = 0;
    MMDL_TOOL_GetCenterGridPos(
        MMDL_GetGridPosX(mmdl), MMDL_GetGridPosZ(mmdl), &pos );
    pos.y = MMDL_GetVectorPosY( mmdl );
    MMDL_GetMapPosHeight( mmdl, &pos, &height );
    MMDL_SetGridPosY( mmdl, SIZE_GRID_FX32(height) );
  }
}

//--------------------------------------------------------------
/**
 * 現在座標で過去座標を更新
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateGridPosCurrent( MMDL * mmdl )
{
  MMDL_SetOldGridPosX( mmdl, MMDL_GetGridPosX(mmdl) );
  MMDL_SetOldGridPosY( mmdl, MMDL_GetGridPosY(mmdl) );
  MMDL_SetOldGridPosZ( mmdl, MMDL_GetGridPosZ(mmdl) );
}

//--------------------------------------------------------------
/**
 * 現在のグリッド座標から指定方向先のアトリビュートを取得
 * @param  mmdl  MMDL *
 * @param  dir    DIR_UP等
 * @retval  u32    MAPATTR
 */
//--------------------------------------------------------------
MAPATTR MMDL_GetMapDirAttr( MMDL * mmdl, u16 dir )
{
  VecFx32 pos;
  MAPATTR attr = MAPATTR_ERROR;
  s16 gx = MMDL_GetGridPosX( mmdl );
  s16 gz = MMDL_GetGridPosZ( mmdl );
  
  MMDL_TOOL_GetCenterGridPos( gx, gz, &pos );
  pos.y = MMDL_GetVectorPosY( mmdl );
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  
  MMDL_GetMapPosAttr( mmdl, &pos, &attr );
  return( attr );
}

//======================================================================
//  3D座標系
//======================================================================
//--------------------------------------------------------------
/**
 * 指定値を加算する
 * @param  mmdl    MMDL * 
 * @param  val      移動量
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddVectorPos( MMDL * mmdl, const VecFx32 *val )
{
  VecFx32 vec;
  MMDL_GetVectorPos( mmdl, &vec );
  vec.x += val->x;
  vec.y += val->y;
  vec.z += val->z;
  MMDL_SetVectorPos( mmdl, &vec );
}

//--------------------------------------------------------------
/**
 * 指定値を指定方向に加算する
 * @param  mmdl    MMDL * 
 * @param  dir      移動4方向。DIR_UP等
 * @param  val      移動量(正
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddVectorPosDir( MMDL *mmdl, u16 dir, fx32 val )
{
  VecFx32 vec;
  MMDL_GetVectorPos( mmdl, &vec );
  switch( dir ){
  case DIR_UP:    vec.z -= val;  break;
  case DIR_DOWN:    vec.z += val;  break;
  case DIR_LEFT:    vec.x -= val;  break;
  case DIR_RIGHT:    vec.x += val;  break;
  }
  MMDL_SetVectorPos( mmdl, &vec );
}

//--------------------------------------------------------------
/**
 * 現在座標から高さ取得し実座標に反映
 * MMDL_MOVEBIT_HEIGHT_GET_ERRORのセットも併せて行う
 * @param  mmdl    MMDL * 
 * @retval  BOOL  TRUE=高さが取れた。FALSE=取れない。
 * MMDL_MOVEBIT_HEIGHT_GET_ERRORで取得可能
 */
//--------------------------------------------------------------
BOOL MMDL_UpdateCurrentHeight( MMDL * mmdl )
{
  VecFx32 vec_pos,vec_pos_h;
  
  MMDL_GetVectorPos( mmdl, &vec_pos );
  vec_pos_h = vec_pos;

  // レール動作中は行わない。
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_RAIL_MOVE) ){
    return FALSE;
  }
  
  if( MMDL_CheckStatusBitHeightGetOFF(mmdl) == TRUE ){
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_HEIGHT_GET_ERROR );
    return( FALSE );
  }
  
  {
    fx32 height;
    int ret = MMDL_GetMapPosHeight( mmdl, &vec_pos_h, &height );
    
    if( ret == TRUE ){
      vec_pos.y = height;
      MMDL_SetVectorPos( mmdl, &vec_pos );
      MMDL_SetOldGridPosY( mmdl, MMDL_GetGridPosY(mmdl) );
      MMDL_SetGridPosY( mmdl, SIZE_GRID_FX32(height) );
      MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_HEIGHT_GET_ERROR );
    }else{
      MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_HEIGHT_GET_ERROR );
    }
    return( ret );
  }
}

//--------------------------------------------------------------
/**
 * 現在座標でアトリビュート反映
 * MMDL_MOVEBIT_ATTR_GET_ERRORのセットも併せて行う
 * @retval  BOOL TRUE=取得成功。FALSE=失敗。
 */
//--------------------------------------------------------------
BOOL MMDL_UpdateCurrentMapAttr( MMDL * mmdl )
{
  BOOL ret_o = FALSE;
  BOOL ret_n = FALSE;
  MAPATTR old_attr = MAPATTR_ERROR;
  MAPATTR now_attr = old_attr;
  
  if( MMDL_CheckStatusBitAttrGetOFF(mmdl) == FALSE ){
    VecFx32 pos;
    int gx = MMDL_GetOldGridPosX( mmdl );
    int gy = MMDL_GetOldGridPosY( mmdl );
    int gz = MMDL_GetOldGridPosZ( mmdl );
    
    pos.x = GRID_SIZE_FX32( gx );
    pos.y = GRID_SIZE_FX32( gy );
    pos.z = GRID_SIZE_FX32( gz );
    ret_o = MMDL_GetMapPosAttr( mmdl, &pos, &old_attr );
    
    gx = MMDL_GetGridPosX( mmdl );
    gz = MMDL_GetGridPosZ( mmdl );
    MMDL_TOOL_GetCenterGridPos( gx, gz, &pos );
    pos.y = MMDL_GetVectorPosY( mmdl );
    ret_n = MMDL_GetMapPosAttr( mmdl, &pos, &now_attr );
  }
  
  if( ret_o == TRUE ){
    MMDL_SetMapAttrOld( mmdl, old_attr );
  }
  
  if( ret_n == TRUE ){
    MMDL_SetMapAttr( mmdl, now_attr );
  }
  
  if( ret_n == TRUE ){ //正常取得
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ATTR_GET_ERROR );
    return( TRUE );
  }
  
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_ATTR_GET_ERROR ); //異常
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * 指定動作モデルの画面サイズの範囲内に自機が居るかどうか
 * @param 
 * @retval
 */
//--------------------------------------------------------------
BOOL MMDL_CheckPlayerDispSizeRect( const MMDL *mmdl )
{
  MMDL *jiki;
  
  jiki = MMDLSYS_SearchMMdlPlayer( MMDL_GetMMdlSys(mmdl) );
  
  if( jiki != NULL ){
    MMDL_RECT rect;
    s16 jx = MMDL_GetGridPosX( jiki );
    s16 jz = MMDL_GetGridPosZ( jiki );
    s16 gx = MMDL_GetGridPosX( mmdl );
    s16 gz = MMDL_GetGridPosZ( mmdl );
    
    rect.left = gx - 18; //1Grid 16dotで。さらに緩めな判定として2Grid足す
    rect.right = gx + 17;
    rect.top = gz - 18;
    rect.bottom = gz + 17;
    
    if( rect.top <= jz && rect.bottom >= jz ){
      if( rect.left <= jx && rect.right >= jx ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//======================================================================
//  描画初期化、復帰直後に呼ばれる動作初期化関数
//======================================================================
//--------------------------------------------------------------
/**
 * 描画初期化、復帰直後に呼ばれる動作初期化関数。
 * 隠れ蓑系動作は動作関数内でエフェクト系のセットを行っている。
 * イベント等のポーズが初期化直後から発生している場合、
 * 動作関数が呼ばれない為、描画初期化時にこの関数を呼ぶ事で対策。
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveProcAfterDrawInit( MMDL * mmdl )
{
  u16 code = MMDL_GetMoveCode( mmdl );
  if( code == MV_HIDE_SNOW || code == MV_HIDE_SAND ||
    code == MV_HIDE_GRND || code == MV_HIDE_KUSA ){
    MMDL_CallMoveProc( mmdl );
  }
}

//======================================================================
//  動作ツール
//======================================================================
//--------------------------------------------------------------
///  4方向別座標増減　X
//--------------------------------------------------------------
static const int DATA_DirGridValueX[] =
{ 0, 0, -1, 1 };

//--------------------------------------------------------------
///  4方向別座標増減　Y
//--------------------------------------------------------------
static const int DATA_DirGridValueY[] =
{ 0, 0, 0, 0 };

//--------------------------------------------------------------
///  4方向別座標増減　Z
//--------------------------------------------------------------
static const int DATA_DirGridValueZ[] =
{ -1, 1, 0, 0 };

//--------------------------------------------------------------
/**
 * 方向からX方向増減値を取得
 * @param  dir    方向。DIR_UP等
 * @retval  s16 グリッド単位での増減値
 */
//--------------------------------------------------------------
s16 MMDL_TOOL_GetDirAddValueGridX( u16 dir )
{
  return( (s16)DATA_DirGridValueX[dir] );
}

//--------------------------------------------------------------
/**
 * 方向からZ方向増減値を取得
 * @param  dir    方向。DIR_UP等
 * @retval  int    グリッド単位での増減値
 */
//--------------------------------------------------------------
s16 MMDL_TOOL_GetDirAddValueGridZ( u16 dir )
{
  return( (s16)DATA_DirGridValueZ[dir] );
}

//--------------------------------------------------------------
/**
 * 指定値を指定方向に加算する
 * @param  dir      移動4方向。DIR_UP等
 * @param  vec      加える対象
 * @param  val      移動量(正
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_AddDirVector( u16 dir, VecFx32 *vec, fx32 val )
{
  switch( dir ){
  case DIR_UP:    vec->z -= val;  break;
  case DIR_DOWN:    vec->z += val;  break;
  case DIR_LEFT:    vec->x -= val;  break;
  case DIR_RIGHT:    vec->x += val;  break;
  }
}

//--------------------------------------------------------------
/**
 * 指定値を指定方向に加算する　グリッド
 * @param  dir      移動4方向。DIR_UP等
 * @param  gx      加える対象　グリッドx
 * @param gz      加える対象　グリッドz
 * @param  val      移動量(正
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_AddDirGrid( u16 dir, s16 *gx, s16 *gz, s16 val )
{
  switch( dir ){
  case DIR_UP: *gz -= val; break;
  case DIR_DOWN: *gz += val; break;
  case DIR_LEFT: *gx -= val; break;
  case DIR_RIGHT: *gx += val; break;
  }
}

//--------------------------------------------------------------
/**
 * 指定されたグリッド座標の中心位置を実座標で取得
 * @param  gx  グリッドX座標  
 * @param  gz  グリッドZ座標
 * @param  vec  座標格納先
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_GetCenterGridPos( s16 gx, s16 gz, VecFx32 *vec )
{
  vec->x = GRID_SIZE_FX32( gx ) + GRID_HALF_FX32;
  vec->z = GRID_SIZE_FX32( gz ) + GRID_HALF_FX32;
}

//--------------------------------------------------------------
/**
 * 指定された実座標をグリッド単位に変更
 * @param  gx  グリッドX座標格納先
 * @param  gy  グリッドY座標格納先
 * @param  gz  グリッドZ座標格納先
 * @param  vec  変換する座標
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_VectorPosToGridPos(
  s16 *gx, s16 *gy, s16 *gz, const VecFx32 *vec )
{
  *gx = SIZE_GRID_FX32( vec->x );
  *gy = SIZE_GRID_FX32( vec->y );
  *gz = SIZE_GRID_FX32( vec->z );
}

//--------------------------------------------------------------
/**
 * 指定されたグリッド座標を実座標に変更
 * @param  gx  グリッドX座標
 * @param  gy  グリッドY座標
 * @param  gz  グリッドZ座標
 * @param  vec  座標変換先
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_GridPosToVectorPos(
    const s16 gx, const s16 gy, const s16 gz, VecFx32 *pos )
{
  pos->x = GRID_SIZE_FX32( gx );
  pos->y = GRID_SIZE_FX32( gy );
  pos->z = GRID_SIZE_FX32( gz );
}

//--------------------------------------------------------------
//  方向反転テーブル
//--------------------------------------------------------------
static const u32 DATA_DirFlipTbl[] =
{ DIR_DOWN, DIR_UP, DIR_RIGHT, DIR_LEFT };

//--------------------------------------------------------------
/**
 * 与えられた方向を反転する
 * @param  dir    DIR_UP等
 * @retval  int    dirの反転方向
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_FlipDir( u16 dir )
{
  return( DATA_DirFlipTbl[dir] );
}

//--------------------------------------------------------------
/**
 * 距離間方向を取得
 * @param  ax  対象A X座標
 * @param  az  対象A Z座標
 * @param  bx  対象B X座標
 * @param  bz  対象B Z座標
 * @retval  int  対象Aから対象Bへの方向 DIR_UP等
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_GetRangeDir( int ax, int az, int bx, int bz )
{
  if( ax > bx ){ return( DIR_LEFT ); }
  if( ax < bx ){ return( DIR_RIGHT ); }
  if( az > bz ){ return( DIR_UP ); }
  return( DIR_DOWN );
}

//--------------------------------------------------------------
/// カメラ横角度変換
//--------------------------------------------------------------
static const u8 data_angle8_4[16] =
{
  0,
  1, 1, 1, 1, 1, 1,
  2, 2,
  3, 3, 3, 3, 3, 3,
  0,
};

static const u16 data_dir_angle[DIR_MAX4] =
{
  0x8000, 0, 0x4000, 0xC000,
};

static const u8 data_angleChange360_4[4][4] =
{
  {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT}, //0 0x0000
  {DIR_RIGHT,DIR_LEFT,DIR_UP,DIR_DOWN}, //1 0x4000
  {DIR_DOWN,DIR_UP,DIR_RIGHT,DIR_LEFT}, //2 0x8000
  {DIR_LEFT,DIR_RIGHT,DIR_DOWN,DIR_UP}, //3 0xc000
};

//--------------------------------------------------------------
/**
 * カメラ横角度から表示する４方向を取得
 * @param dir 角度0時の方向。
 * @param angleYaw カメラ角度
 * @retval u16 DIR_UP等
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_GetAngleYawToDirFour( u16 dir, u16 angleYaw )
{
  angleYaw += data_dir_angle[dir];  //　dirの方向が下向きになる角度分アングルを変更
  angleYaw >>= 12; // angle/0x1000(16)
  angleYaw = data_angle8_4[angleYaw];
  angleYaw = data_angleChange360_4[angleYaw][DIR_DOWN]; // 下向き基準で考えているので。
  GF_ASSERT( angleYaw < DIR_MAX4 );
  return( angleYaw );
}
