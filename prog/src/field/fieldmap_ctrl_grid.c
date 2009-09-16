//======================================================================
/**
 * @file	fieldmap_ctrl_grid.c
 * @brief	フィールドマップ　コントロール　グリッド処理
 * @author	kagaya
 * @data	09.04.22
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field/zonedata.h"
#include "arc/fieldmap/zone_id.h"

#include "fldmmdl.h"
#include "fieldmap.h"
#include "field_player_grid.h"

#include "fieldmap_ctrl_grid.h"

#include "arc/arc_def.h"
#include "arc/arc_def.h"
#include "arc/fieldmap/camera_scroll.naix"


//======================================================================
//	範囲情報
//======================================================================
static FIELD_CAMERA_AREA s_FIELD_CAMERA_AREA = 
{
  FIELD_CAMERA_AREA_RECT,
  FIELD_CAMERA_AREA_CONT_TARGET,
};
 



//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	グリッド処理　シーケンス
//--------------------------------------------------------------
typedef enum
{
	GRIDPROC_SEQNO_INIT = 0,
	GRIDPROC_SEQNO_MAIN,
	GRIDPROC_SEQNO_MAX,
}GRIDPROC_SEQNO;

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
//	FIELDMAP_CTRL_GRID*
//--------------------------------------------------------------
struct _TAG_FIELDMAP_CTRL_GRID
{
	HEAPID heapID;
	FIELDMAP_WORK *fieldWork;
	
	u16 seq_proc;	
	u8 padding0;
	u8 padding1;

	BOOL jikiMovePause;

	FIELD_PLAYER_GRID *gridPlayer;
};

//======================================================================
//	proto
//======================================================================
static void mapCtrlGrid_Create(
	FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir );
static void mapCtrlGrid_Delete( FIELDMAP_WORK *fieldWork );
static void mapCtrlGrid_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos );
static const VecFx32 * mapCtrlGrid_GetCameraTarget( FIELDMAP_WORK *fieldWork );

static u16 grid_ChangeFourDir( u16 dir );


static BOOL gym_check( u16 zone_id );

//======================================================================
//	フィールドマップ　グリッド処理
//======================================================================
//--------------------------------------------------------------
///	マップコントロール　グリッド移動
//--------------------------------------------------------------
const DEPEND_FUNCTIONS FieldMapCtrl_GridFunctions =
{
	FLDMAP_CTRLTYPE_GRID,
	mapCtrlGrid_Create,
	mapCtrlGrid_Main,
	mapCtrlGrid_Delete,
  mapCtrlGrid_GetCameraTarget,
};

//--------------------------------------------------------------
/**
 * フィールドマップ　グリッド処理　初期化 
 * @param	fieldWork	FIELDMAP_WORK
 * @param	pos	自機初期位置
 * @param	dir 自機初期方向
 * @retval	nothing
 */
//--------------------------------------------------------------
static void mapCtrlGrid_Create(
	FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir )
{
	HEAPID heapID;
	FIELDMAP_CTRL_GRID *gridWork;
	
	heapID = FIELDMAP_GetHeapID( fieldWork );
	gridWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELDMAP_CTRL_GRID) );
	
	gridWork->heapID = heapID;
	gridWork->fieldWork = fieldWork;
	
	{	//ビルボード設定
#if 0 //標準を使用する
		VecFx32 scale = {
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
		};
		GFL_BBDACT_SYS *bbdActSys = FIELDMAP_GetBbdActSys( fieldWork );
		GFL_BBD_SetScale( GFL_BBDACT_GetBBDSystem(bbdActSys), &scale );
#endif
	}

#if 0
	{	//マップ描画オフセット
		VecFx32 offs = { -FX32_ONE*8, 0, FX32_ONE*8 };
		FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
		FLDMAPPER_SetDrawOffset( mapper, &offs );
	}
#endif

	{ //自機作成
		FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    MMDL *fmmdl = FIELD_PLAYER_GetMMdl( fld_player );
		u16 dir4 = grid_ChangeFourDir( dir );
    MMDL_SetDirDisp( fmmdl, dir4 );
//    KAGAYA_Printf( "わたされた方向 %xH, %d\n", dir, dir4 );
		gridWork->gridPlayer = FIELD_PLAYER_GRID_Init( fld_player, heapID );
    
    { //カメラ座標セット
      const VecFx32 *pos = MMDL_GetVectorPosAddress( fmmdl );
      FIELDMAP_SetNowPosTarget( fieldWork, pos );
    }
	}
  
	FIELDMAP_SetMapCtrlWork( fieldWork, gridWork );


  //  カメラ範囲
  if( (ZONEDATA_GetMatrixID( FIELDMAP_GetZoneID( fieldWork ) ) > 0) && (gym_check( FIELDMAP_GetZoneID( fieldWork ) ) == FALSE) )
  {

    FIELD_CAMERA_RECT* p_rect;
    p_rect = GFL_ARC_UTIL_Load( ARCID_FLD_CAMSCRLL, 
        ZONEDATA_GetMatrixID( FIELDMAP_GetZoneID( fieldWork ) ), 
        FALSE, heapID );
    s_FIELD_CAMERA_AREA.rect = *p_rect;
    TOMOYA_Printf( "xmin[0x%x] xmax[0x%x] zmin[0x%x] zmax[0x%x]\n", 
        p_rect->x_min, p_rect->x_max, p_rect->z_min, p_rect->z_max );
    FIELD_CAMERA_SetCameraArea( FIELDMAP_GetFieldCamera( fieldWork ), &s_FIELD_CAMERA_AREA );

    GFL_HEAP_FreeMemory( p_rect );
  }
}

//--------------------------------------------------------------
/**
 * フィールドマップ　グリッド処理　終了
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void mapCtrlGrid_Delete( FIELDMAP_WORK *fieldWork )
{
	FIELDMAP_CTRL_GRID *gridWork;
	gridWork = FIELDMAP_GetMapCtrlWork( fieldWork );
	FIELD_PLAYER_GRID_Delete( gridWork->gridPlayer );
	GFL_HEAP_FreeMemory( gridWork );
}

//--------------------------------------------------------------
/**
 * フィールドマップ　グリッド処理　メイン
 * @param	fieldWork FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void mapCtrlGrid_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos )
{
	FIELDMAP_CTRL_GRID *gridWork = FIELDMAP_GetMapCtrlWork( fieldWork );
	FIELD_PLAYER_GRID *gridPlayer = gridWork->gridPlayer;
	
  if( gridWork->jikiMovePause == FALSE )
	{	//自機移動
		int key_trg = GFL_UI_KEY_GetTrg( );
		int key_cont = GFL_UI_KEY_GetCont( );
		FIELD_PLAYER_GRID_Move( gridPlayer, key_trg, key_cont );
		
		{
			GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
			GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
			PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
			FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
			MMDL *fmmdl = FIELD_PLAYER_GetMMdl( fld_player );
			u16 tbl[DIR_MAX4] = { 0x0000, 0x8000, 0x4000, 0xc000 };
			int dir = MMDL_GetDirDisp( fmmdl );
#if 0
			MMDL_GetVectorPos( fmmdl, pos );
			PLAYERWORK_setDirection( player, tbl[dir] );
			PLAYERWORK_setPosition( player, pos );
			FIELD_PLAYER_SetDir( fld_player, tbl[dir] );
			FIELD_PLAYER_SetPos( fld_player, pos );
#endif
		}
	}
}

//--------------------------------------------------------------
/**
 * フィールドマップ　グリッド処理　カメラターゲット取得
 * @param fieldWork FIELDMAP_WORK
 * @retval VecFx32* カメラターゲット
 */
//--------------------------------------------------------------
static const VecFx32 * mapCtrlGrid_GetCameraTarget( FIELDMAP_WORK *fieldWork )
{
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  const VecFx32 *pos = MMDL_GetVectorPosAddress( mmdl ); 
  return( pos );
}

//======================================================================
//	グリッド処理　パーツ
//======================================================================
//--------------------------------------------------------------
/**
 * 360度方向->４方向に
 * @param	dir	方向　0x10000単位
 * @retval	u16 DIR_UP等
 */
//--------------------------------------------------------------
static u16 grid_ChangeFourDir( u16 dir )
{
	if( (dir>0x2000) && (dir<0x6000) ){
		dir = DIR_LEFT;
	}else if( (dir >= 0x6000) && (dir <= 0xa000) ){
		dir = DIR_DOWN;
	}else if( (dir > 0xa000)&&(dir < 0xe000) ){
		dir = DIR_RIGHT;
	}else{
		dir = DIR_UP;
	}
	return( dir );
}


//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
FIELD_PLAYER_GRID * FIELDMAP_CTRL_GRID_GetFieldPlayerGrid( FIELDMAP_CTRL_GRID *gridWork )
{
  return( gridWork->gridPlayer );
}

//--------------------------------------------------------------
/**
 * フィールドマップ　グリッド処理　自機動作停止
 * @param fieldMap FIELDMAP_WORK
 * @param flag TRUE=停止 FALSE=動作
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELDMAP_CTRL_GRID_SetPlayerPause( FIELDMAP_WORK *fieldMap, BOOL flag )
{
  FIELDMAP_CTRL_GRID *gridWork = FIELDMAP_GetMapCtrlWork( fieldMap );
#ifdef PM_DEBUG
  {
    FLDMAP_CTRLTYPE type = FIELDMAP_GetMapControlType( fieldMap );
    GF_ASSERT( type == FLDMAP_CTRLTYPE_GRID );
  }
#endif
  gridWork->jikiMovePause = flag;
}




//----------------------------------------------------------------------------
/**
 *	@brief  ぞーんIDからジムチェック
 *
 *	@param	zone_id 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL gym_check( u16 zone_id )
{
  switch( zone_id )
  {
  case ZONE_ID_C01GYM0101:
  case ZONE_ID_C02GYM0101:
  case ZONE_ID_C03GYM0101:
  case ZONE_ID_C04GYM0101:
  case ZONE_ID_C05GYM0101:
  case ZONE_ID_C06GYM0101:
  case ZONE_ID_C07GYM0101:
  case ZONE_ID_C08GYM0101:
    
    return TRUE;
  }

  return FALSE;
}

