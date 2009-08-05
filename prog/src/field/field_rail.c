//============================================================================================
/**
 * @file  field_rail.c
 * @brief ノーグリッドでの自機・カメラ制御構造
 * @author  tamada, tomoya
 * @date  2009.05.18
 *
 */
//============================================================================================

#include <gflib.h>

#include "field/fieldmap_proc.h"

#include "field_rail.h"
#include "field_rail_access.h"

#include "field_camera.h"

//============================================================================================
//============================================================================================
// 歩きオフセット
#define RAIL_WALK_OFS	(8)

// 歩きカウント
static u8 RAIL_COUNTUP[ RAIL_FRAME_MAX ] = 
{
  1,        //RAIL_FRAME_8,
  2,        //RAIL_FRAME_4,
  4,       //RAIL_FRAME_2,
};

//============================================================================================
//
//
//
//    レール表現を実現するためのツール
//
//
//
//============================================================================================

//------------------------------------------------------------------
//------------------------------------------------------------------
static const RAIL_LINEPOS_SET RAIL_LINEPOS_SET_Default =
{
  RAIL_TBL_NULL,
  RAIL_TBL_NULL,
  0,
  0,
  0,
  0,
};


//============================================================================================
//
//
//      カメラ指定のためのツール
//
//
//============================================================================================
//-----------------------------------------------------------------
//------------------------------------------------------------------
static const RAIL_CAMERA_SET RAIL_CAMERA_SET_Default =
{
  RAIL_TBL_NULL,
  0,
  0,
  0,
  0,
};
//------------------------------------------------------------------
//------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
struct _FIELD_RAIL_WORK{  
  FIELD_RAIL_TYPE type;
  union { 
    void * dummy;
    const RAIL_POINT * point;
    const RAIL_LINE * line;
  };
  u16 active;
	u16 dat_index;	// POINT or LINEのインデックス
  /// LINEにいる間の、LINE上でのオフセット位置
  s32 line_ofs;
  s32 line_ofs_max;
  /// LINEにいる間の幅オフセット位置(XZ平面上でLINEに垂直)
  s32 width_ofs;
  s32 width_ofs_max;  // 水平方向分割数
  fx32 ofs_unit;       // 1分割の距離

  RAIL_KEY key;

	/// レールデータ
	const RAIL_SETTING* rail_dat;


	///<一定フレームでの動作用情報
	u8	  req_move;
	s8		ofs_move;
	u16		save_move_key;    // RAIL_KEY
	u16		save_move_frame;  // RAIL_FRAME

  /// 移動が起きた最新のキーバリュー
  u8 last_key;           // RAIL_KEY
  u8 last_frame;         // RAIL_FRAME
  u8 last_move;         // 最新の動作で、移動したのかチェック
  u8 last_pad;          // 予備
  

};

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
struct _FIELD_RAIL_MAN{
  HEAPID heapID;

  BOOL active_flag;

  FIELD_RAIL_WORK * rail_work;
  int work_num;

  // カメラ情報
  FIELD_CAMERA * field_camera;
  const FIELD_RAIL_WORK * camera_bind_work;


  /// ライン、ポイントテーブル
	RAIL_SETTING rail_dat;
};

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static int RailKeyTokeyCont(RAIL_KEY key);
static RAIL_KEY keyContToRailKey(int cont);
static RAIL_KEY getReverseKey(RAIL_KEY key);
static RAIL_KEY getClockwiseKey(RAIL_KEY key);
static RAIL_KEY getAntiClockwiseKey(RAIL_KEY key);




//------------------------------------------------------------------
//------------------------------------------------------------------
static void initRail(FIELD_RAIL_WORK * work, const RAIL_SETTING* rail_dat );
static BOOL isValidRail(const FIELD_RAIL_WORK * work);
static const RAIL_CAMERA_SET * getCameraSet(const FIELD_RAIL_WORK * work);
static const char * getRailName(const FIELD_RAIL_WORK * work);
static void getRailPosition(const FIELD_RAIL_WORK * work, VecFx32 * pos);
static s32 getLineOfsMax( const RAIL_LINE * line, fx32 unit, const RAIL_SETTING* rail_dat );
static s32 getLineWidthOfsMax( const RAIL_LINE * line, u32 line_ofs, u32 line_ofs_max, const RAIL_SETTING* rail_dat );
static RAIL_KEY updateLine( FIELD_RAIL_WORK * work, const RAIL_LINE * line, u32 line_ofs_max, u32 key );
static RAIL_KEY setLine(FIELD_RAIL_WORK * work, const RAIL_LINE * line, RAIL_KEY key, int l_ofs, int w_ofs, int l_ofs_max, int w_ofs_max);
static void setLineData(FIELD_RAIL_WORK * work, const RAIL_LINE * line, RAIL_KEY key, int l_ofs, int w_ofs, int l_ofs_max, int w_ofs_max);
static BOOL isLinePoint_S( const RAIL_SETTING * rail_dat, const RAIL_LINE * line, const RAIL_POINT * point );
static BOOL isLinePoint_E( const RAIL_SETTING * rail_dat, const RAIL_LINE * line, const RAIL_POINT * point );

//------------------------------------------------------------------
//------------------------------------------------------------------
static void initRailDat( RAIL_SETTING * raildat, const RAIL_SETTING * setting );
static const RAIL_POINT* getRailDatPoint( const RAIL_SETTING * raildat, u32 index );
static const RAIL_LINE* getRailDatLine( const RAIL_SETTING * raildat, u32 index );
static const RAIL_CAMERA_SET* getRailDatCamera( const RAIL_SETTING * raildat, u32 index );
static const RAIL_LINEPOS_SET* getRailDatLinePos( const RAIL_SETTING * raildat, u32 index );
static RAIL_CAMERA_FUNC*const getRailDatCameraFunc( const RAIL_SETTING * raildat, u32 index );
static RAIL_POS_FUNC*const getRailDatLinePosFunc( const RAIL_SETTING * raildat, u32 index );
static RAIL_LINE_DIST_FUNC*const getRailDatLineDistFunc( const RAIL_SETTING * raildat, u32 index );

//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL debugCheckLineData(const RAIL_LINE * line, const RAIL_SETTING* rail_dat);
static BOOL debugCheckPointData(const RAIL_POINT * point, const RAIL_SETTING* rail_dat);
static void debugPrintPoint(const char * before, const RAIL_POINT * point, const char * after);
static const char * debugGetRailKeyName(RAIL_KEY key);
static void debugPrintRailInfo(const FIELD_RAIL_WORK * work);


static RAIL_KEY updateLineMove_new(FIELD_RAIL_WORK * work, RAIL_KEY key, u32 count_up);
//static RAIL_KEY updateLineMove(FIELD_RAIL_WORK * work, RAIL_KEY key);
static RAIL_KEY updatePointMove(FIELD_RAIL_WORK * work, RAIL_KEY key, u32 count_up);


static u32 getPointIndex( const RAIL_SETTING * raildat, const RAIL_POINT* point );
static u32 getLineIndex( const RAIL_SETTING * raildat, const RAIL_LINE* line );



// １ワーク単位のアップデート
static void FIELD_RAIL_WORK_Update(FIELD_RAIL_WORK * work);

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void RAIL_LOCATION_Dump(const RAIL_LOCATION * railLoc)
{
  TAMADA_Printf("RAIL_LOC:type = %d, rail_index = %d\n",railLoc->type, railLoc->rail_index);
  TAMADA_Printf("RAIL_LOC:line_ofs = %d, width_ofs = %d\n", railLoc->line_ofs, railLoc->width_ofs);
  TAMADA_Printf("RAIL_LOC:key = %s\n", debugGetRailKeyName(railLoc->key));
}

//------------------------------------------------------------------
/**
 * @brief RAIL_LOCATION構造体の初期化処理
 */
//------------------------------------------------------------------
void RAIL_LOCATION_Init(RAIL_LOCATION * railLoc)
{
  railLoc->type = FIELD_RAIL_TYPE_POINT;
  railLoc->rail_index = 0;
  railLoc->line_ofs = 0;
  railLoc->width_ofs = 0;
  railLoc->key = RAIL_KEY_NULL;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief レール制御システムの生成
 */
//------------------------------------------------------------------
FIELD_RAIL_MAN * FIELD_RAIL_MAN_Create(HEAPID heapID, u32 work_num, FIELD_CAMERA * camera)
{ 
  int i;
  FIELD_RAIL_MAN * man = GFL_HEAP_AllocMemory(heapID, sizeof(FIELD_RAIL_MAN));

  man->heapID = heapID;
  man->active_flag = FALSE;
  man->field_camera = camera;
  man->camera_bind_work = NULL;

  // レールワークのメモリ確保
  man->rail_work = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_RAIL_WORK)*work_num );
  man->work_num = work_num;
  for( i=0; i<work_num; i++ )
  {
    initRail(&man->rail_work[i], &man->rail_dat);
  }
  
  return man;
}

//------------------------------------------------------------------
/**
 * @brief レール制御システムの削除
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Delete(FIELD_RAIL_MAN * man)
{ 
  GFL_HEAP_FreeMemory( man->rail_work );
  GFL_HEAP_FreeMemory(man);
}

//------------------------------------------------------------------
/**
 * @brief レールデータの読み込み
 *
 * とりあえず、最初はPOINTから始まると仮定する
 *
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Load(FIELD_RAIL_MAN * man, const RAIL_SETTING * setting)
{ 
  man->active_flag = TRUE;

  // ポイント、ラインテーブル設定
	initRailDat( &man->rail_dat, setting );
}

//----------------------------------------------------------------------------
/**
 *	@brief  移動管理ワークの生成
 *
 *	@param	man   マネージャ
 *
 *	@return 移動管理ワーク
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_WORK* FIELD_RAIL_MAN_CreateWork( FIELD_RAIL_MAN * man )
{
  FIELD_RAIL_WORK* work;
  int i;

  // 空いているワークを探す
  work = NULL;
  for( i=0; i<man->work_num; i++ )
  {
    if( isValidRail( &man->rail_work[i] ) == FALSE )
    {
      work = &man->rail_work[i];
      break;
    }
  }
  // あきなし！
  GF_ASSERT( work );

  return work;
}

//----------------------------------------------------------------------------
/**
 *	@brief  移動管理ワークの破棄
 *
 *	@param	man   マネージャ
 *	@param	work  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_DeleteWork( FIELD_RAIL_MAN * man, FIELD_RAIL_WORK* work )
{
  initRail( work, &man->rail_dat );
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラにレールワークをバインドする
 *
 *	@param	man   マネージャ
 *	@param	work  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_BindCamera( FIELD_RAIL_MAN * man, const FIELD_RAIL_WORK* work )
{
  man->camera_bind_work = work;
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラのレールワークのバインドをほどく
 *
 *	@param	man   マネージャ
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_UnBindCamera( FIELD_RAIL_MAN * man )
{
  man->camera_bind_work = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  バインドした、レールワークの座標取得
 *
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_GetBindWorkPos( const FIELD_RAIL_MAN * man, VecFx32* pos )
{
  GF_ASSERT( man );
  if(man->camera_bind_work)
  {
    FIELD_RAIL_WORK_GetPos( man->camera_bind_work, pos );
  }
}

#ifdef PM_DEBUG
// デバック専用、バインドしたワークを編集可能な形で取得
FIELD_RAIL_WORK* FIELD_RAIL_MAN_DEBUG_GetBindWork( const FIELD_RAIL_MAN * man )
{
  return (FIELD_RAIL_WORK*)man->camera_bind_work;
}
#endif

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_SetActiveFlag(FIELD_RAIL_MAN * man, BOOL flag)
{
  man->active_flag = flag;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_GetActiveFlag(const FIELD_RAIL_MAN *man)
{
  return man->active_flag;
}

//------------------------------------------------------------------
//  全ワークのアップデート
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Update(FIELD_RAIL_MAN * man)
{
  int i;


  if( !man->active_flag )
  {
    return ;
  }

  for( i=0; i<man->work_num; i++ )
  {
    FIELD_RAIL_WORK_Update( &man->rail_work[i] );
  }
}


//------------------------------------------------------------------
/**
 * @brief カメラ状態のアップデート
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_UpdateCamera(const FIELD_RAIL_MAN * man)
{
  const FIELD_RAIL_WORK * work = man->camera_bind_work;
	const RAIL_CAMERA_SET* camera_set;
  RAIL_CAMERA_FUNC * func = NULL;

  if (!man->active_flag)
  {
    return;
  }
  
  if ( !work )
  {
    return ;
  }

  if (isValidRail(work) == FALSE)
  {
    return;
  }

  switch (work->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    if (work->point->camera_set != RAIL_TBL_NULL)
    {
			camera_set = getRailDatCamera( work->rail_dat, work->point->camera_set );
      func = getRailDatCameraFunc( work->rail_dat, camera_set->func_index );
    }
    break;
  case FIELD_RAIL_TYPE_LINE:
    if (work->line->camera_set != RAIL_TBL_NULL)
    {
			camera_set = getRailDatCamera( work->rail_dat, work->line->camera_set );
      func = getRailDatCameraFunc( work->rail_dat, camera_set->func_index );
    }
    break;
  default:
    GF_ASSERT(0); //NO TYPE;
  }
  if (func)
  {
    func(man);
  }
}


//============================================================================================
//
//
//  レール移動制御のための関数群
//
//
//============================================================================================



//----------------------------------------------------------------------------
/**
 *	@brief  レール位置情報の取得
 *
 *	@param	work      ワーク
 *	@param	location  位置情報格納先
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_WORK_GetLocation(const FIELD_RAIL_WORK * work, RAIL_LOCATION * location)
{
  location->type        = work->type;

  // インデックス取得
  if( work->type==FIELD_RAIL_TYPE_POINT ){
    location->rail_index  = getPointIndex( work->rail_dat, work->point );
  }else{
    location->rail_index  = getLineIndex( work->rail_dat, work->line );
  }

  // 各種オフセット
  location->line_ofs  = work->line_ofs;
  location->width_ofs = work->width_ofs;
  location->key       = work->key;

  RAIL_LOCATION_Dump(location);
}

//----------------------------------------------------------------------------
/**
 *	@brief  レール位置情報の設定
 *
 *	@param	man       マネージャ
 *	@param	location  指定位置情報
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_WORK_SetLocation(FIELD_RAIL_WORK * work, const RAIL_LOCATION * location)
{
  int i;
  const RAIL_LINE* line = NULL;
  int width_ofs_max;
  u32 line_ofs_max;
  int line_ofs;
  int width_ofs;
  int key;

  RAIL_LOCATION_Dump(location);
  // 初期化
  initRail( work, work->rail_dat );

  if( location->type==FIELD_RAIL_TYPE_POINT )
  {
    // 点初期化
    work->type          = FIELD_RAIL_TYPE_POINT;
    work->point         = &work->rail_dat->point_table[ location->rail_index ];

    work->width_ofs_max = work->point->width_ofs_max[0];

    for( i=0; i<RAIL_CONNECT_LINE_MAX; i++ )
    {
      if( work->point->lines[i] != RAIL_TBL_NULL )
      {
        if( line == NULL )
        {
          line = &work->rail_dat->line_table[ work->point->lines[i] ];
          if( line->point_s == location->rail_index )
          {
            line_ofs  = 0;
            width_ofs = 0;
            key       = work->point->keys[i];
          }
          else if( line->point_e == location->rail_index )
          {
            line_ofs  = getLineOfsMax( line, work->ofs_unit, work->rail_dat );
            width_ofs = 0;
            key       = work->point->keys[i];
          }
        }
        else
        {
          // 複数候補がある場合には、最初に見つかったラインを使用します。
          OS_TPrintf( "work::SetLocation Point ラインが複数あります。\n" );
        }
      }
    }
    GF_ASSERT( line );
#ifdef PM_DEBUG
    if( !line )
    {
      // ポイントに降り立つ（エディット用）
      work->type          = FIELD_RAIL_TYPE_POINT;
      work->point         = &work->rail_dat->point_table[ location->rail_index ];

      work->width_ofs_max = work->point->width_ofs_max[0];
      work->active = TRUE;
      return ;
    }
#endif
  }
  else
  {
    
    // ライン初期化
    line        = &work->rail_dat->line_table[ location->rail_index ];
    line_ofs    = location->line_ofs;
    width_ofs   = location->width_ofs;
    key         = location->key;
  }

  line_ofs_max  = getLineOfsMax( line, work->ofs_unit, work->rail_dat );
  width_ofs_max = getLineWidthOfsMax( line, line_ofs, line_ofs_max, work->rail_dat );
  setLineData( work, line, key, line_ofs, width_ofs, line_ofs_max, width_ofs_max );

  TOMOYA_Printf( "key %d\n", key );

  // 方向設定
  work->last_frame = RAIL_FRAME_8;
  work->last_key = key;
  work->active = TRUE;
}

//------------------------------------------------------------------
/**
 * @brief 現在位置の取得
 */
//------------------------------------------------------------------
void FIELD_RAIL_WORK_GetPos(const FIELD_RAIL_WORK * work, VecFx32 * pos)
{
  GF_ASSERT( work );
  getRailPosition(work, pos);
}


//----------------------------------------------------------------------------
/**
 *	@brief	レールシステム　最新の動作を起こした時の動作フレーム
 *
 *	@param	man		
 *
 *	@return	キー情報
 */
//-----------------------------------------------------------------------------
RAIL_FRAME FIELD_RAIL_WORK_GetActionFrame( const FIELD_RAIL_WORK * work )
{
	GF_ASSERT( work );
	return work->last_frame;
}

//----------------------------------------------------------------------------
/**
 *	@brief	レールシステム　最後に移動した方向を取得
 *
 *	@param	man		
 *
 *	@return	方向
 */
//-----------------------------------------------------------------------------
RAIL_KEY FIELD_RAIL_WORK_GetActionKey( const FIELD_RAIL_WORK * work )
{
	GF_ASSERT( work );
	return work->last_key;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レールシステム  最新のUPDATEでいどうしたのか
 *
 *	@param	work 
 *  
 *	@retval TRUE  移動した
 *	@retval FALSE 移動しなかった
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_WORK_GetLastAction( const FIELD_RAIL_WORK * work )
{
	GF_ASSERT( work );
	return work->last_move;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ワーク単位の動作設定
 *
 *	@param	work
 *	@param	flag 
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_WORK_SetActiveFlag( FIELD_RAIL_WORK * work, BOOL flag )
{
  GF_ASSERT(work);
  work->active = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ワーク動作チェック
 *
 *	@param	work  ワーク
 *
 *	@retval TRUE  動作
 *	@retval FALSE 動作してない
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_WORK_IsActive( const FIELD_RAIL_WORK * work )
{
  GF_ASSERT(work);
  return work->active;
}

//----------------------------------------------------------------------------
/**
 *	@brief  1歩移動のリクエスト
 *
 *	@param	work      ワーク
 *	@param	frame     移動フレーム数
 *	@param	key       移動方向キー
 *
 *	@retval TRUE  リクエスト受理
 *	@retval FALSE 現在動作中なので、受理できない
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_WORK_ForwardReq( FIELD_RAIL_WORK * work, RAIL_FRAME frame, RAIL_KEY key )
{
  // 動作していないとき
  if( !work->req_move )
  {
    work->req_move = TRUE;
    work->ofs_move = 0;
    work->save_move_key = key;
    work->save_move_frame = frame;
  }

  return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief 現在位置のアップデート
 */
//------------------------------------------------------------------
static void FIELD_RAIL_WORK_Update(FIELD_RAIL_WORK * work)
{ 
  if (!work->active)
  {
    return;
  }

  if (isValidRail(work) == FALSE)
  {
    return;
  }
  
  work->last_move   = FALSE;
	
	if( work->req_move )
	{
		FIELD_RAIL_TYPE type = work->type;
		RAIL_KEY set_key = RAIL_KEY_NULL;
		RAIL_KEY key = work->save_move_key;
		u32 count_up = RAIL_COUNTUP[ work->save_move_frame ];

		work->ofs_move += count_up;
		

		if(work->type == FIELD_RAIL_TYPE_POINT)
		{ //POINT上のときの移動処理
			TOMOYA_Printf( "count_up point %d\n", count_up );
			set_key = updatePointMove(work, key, count_up);
		}
		else if(work->type == FIELD_RAIL_TYPE_LINE)
		{ //LINE上のときの移動処理
			set_key = updateLineMove_new(work, key, count_up);
		}

		if (set_key != RAIL_KEY_NULL)
		{
			OS_TPrintf("RAIL:%s :line_ofs=%d line_ofs_max=%d width_ofs=%d\n",
					debugGetRailKeyName(set_key), work->line_ofs, work->line_ofs_max, work->width_ofs);

      work->last_key    = set_key;
      work->last_frame  = work->save_move_frame;
      work->last_move   = TRUE;
		}

		if (type != work->type)
		{
			OS_TPrintf("RAIL:change to ");
			debugPrintRailInfo(work);
		}

    // リクエスト移動完了
		if( work->ofs_move >= RAIL_WALK_OFS )
		{
			work->req_move = FALSE;
		}
	}

}



//------------------------------------------------------------------
//  FIELD_RAIL_MANアクセス関数
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	カメラポインタ取得
 */
//-----------------------------------------------------------------------------
FIELD_CAMERA* FIELD_RAIL_MAN_GetCamera( const FIELD_RAIL_MAN * man )
{
	GF_ASSERT( man );
	return  man->field_camera;
}

//----------------------------------------------------------------------------
/**
 *	@brief	今のレールワーク
 */
//-----------------------------------------------------------------------------
const FIELD_RAIL_WORK* FIELD_RAIL_MAN_GetBindWork( const FIELD_RAIL_MAN * man )
{
	GF_ASSERT( man );
	return man->camera_bind_work;
}


//------------------------------------------------------------------
//  FIELD_RAIL_WORKアクセス関数
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	レールタイプの取得
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_TYPE FIELD_RAIL_GetType( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->type;
}

//----------------------------------------------------------------------------
/**
 *	@brief	レールポイントの取得
 */
//-----------------------------------------------------------------------------
const RAIL_POINT* FIELD_RAIL_GetPoint( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_POINT );
	return work->point;
}

//----------------------------------------------------------------------------
/**
 *	@brief	レールラインの取得
 */
//-----------------------------------------------------------------------------
const RAIL_LINE* FIELD_RAIL_GetLine( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_LINE );
	return work->line;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ラインオフセットの取得
 */
//-----------------------------------------------------------------------------
s32 FIELD_RAIL_GetLineOfs( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->line_ofs;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ラインオフセット最大値の取得
 */
//-----------------------------------------------------------------------------
s32 FIELD_RAIL_GetLineOfsMax( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->line_ofs_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	幅オフセットの取得
 */
//-----------------------------------------------------------------------------
s32 FIELD_RAIL_GetWidthOfs( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->width_ofs;
}

//----------------------------------------------------------------------------
/**
 *	@brief	幅オフセット最大値の取得
 */
//-----------------------------------------------------------------------------
s32 FIELD_RAIL_GetWidthOfsMax( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->width_ofs_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	移動単位の取得
 */
//-----------------------------------------------------------------------------
fx32 FIELD_RAIL_GetOfsUnit( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->ofs_unit;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラセットの取得
 */
//-----------------------------------------------------------------------------
const RAIL_CAMERA_SET* FIELD_RAIL_GetCameraSet( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return getCameraSet( work );
}



//------------------------------------------------------------------
//  RAIL_LINEアクセス関数
//------------------------------------------------------------------
const RAIL_POINT* FIELD_RAIL_GetPointStart( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT(work);
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_LINE );
	return getRailDatPoint( work->rail_dat, work->line->point_s );
}
const RAIL_POINT* FIELD_RAIL_GetPointEnd( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT(work);
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_LINE );
	return getRailDatPoint( work->rail_dat, work->line->point_e );
}
const RAIL_LINEPOS_SET* FIELD_RAIL_GetLinePosSet( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT(work);
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_LINE );
	return getRailDatLinePos( work->rail_dat, work->line->line_pos_set );
}

//------------------------------------------------------------------
//  RAIL_POINT
//------------------------------------------------------------------
const RAIL_CAMERA_SET* FIELD_RAIL_POINT_GetCameraSet( const FIELD_RAIL_WORK* work, const RAIL_POINT* point )
{
	GF_ASSERT( work );
	GF_ASSERT( point );
	return getRailDatCamera( work->rail_dat, point->camera_set);
}



//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static const RAIL_LINE * RAILPOINT_getLineByKey(const RAIL_POINT * point, RAIL_KEY key, const RAIL_SETTING* rail_dat)
{
  int i;
  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
    if (point->keys[i] == key)
    {
      return getRailDatLine( rail_dat, point->lines[i] );
    }
  }
  return NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY setLine(FIELD_RAIL_WORK * work, const RAIL_LINE * line, RAIL_KEY key, int l_ofs, int w_ofs, int l_ofs_max, int w_ofs_max)
{
  GF_ASSERT(work->line != line);
  GF_ASSERT(work->line);
  GF_ASSERT(line->name);
  if (work->type == FIELD_RAIL_TYPE_LINE)
  {
    TAMADA_Printf("RAIL: LINE \"%s\" to %s \"%s\"\n",
        work->line->name, debugGetRailKeyName(key), line->name);
  }
  else
  {
    TAMADA_Printf("RAIL: POINT \"%s\" to %s \"%s\"\n",
        work->point->name, debugGetRailKeyName(key), line->name);
  }

  setLineData( work, line, key, l_ofs, w_ofs, l_ofs_max, w_ofs_max );

  debugCheckLineData(line, work->rail_dat);
  return key;
}

// レールシステムにラインの情報を設定する
static void setLineData(FIELD_RAIL_WORK * work, const RAIL_LINE * line, RAIL_KEY key, int l_ofs, int w_ofs, int l_ofs_max, int w_ofs_max)
{
  GF_ASSERT( work );
  GF_ASSERT( line );
  work->type = FIELD_RAIL_TYPE_LINE;
  work->line = line;
  work->key = key;
  work->line_ofs = l_ofs;
  work->width_ofs = w_ofs;
  work->line_ofs_max = l_ofs_max;
  work->width_ofs_max = w_ofs_max;
}

// ラインの開始ポイントと、引数のpointが一致するかチェック
static BOOL isLinePoint_S( const RAIL_SETTING * rail_dat, const RAIL_LINE * line, const RAIL_POINT * point )
{
	const RAIL_POINT * point_s;

	point_s = getRailDatPoint( rail_dat, line->point_s );

	if( point_s == point )
	{
		return TRUE;
	}
	return FALSE;
}

// ラインの終了ポイントと、引数のpointが一致するかチェック
static BOOL isLinePoint_E( const RAIL_SETTING * rail_dat, const RAIL_LINE * line, const RAIL_POINT * point )
{
	const RAIL_POINT * point_e;

	point_e = getRailDatPoint( rail_dat, line->point_e );

	if( point_e == point )
	{
		return TRUE;
	}
	return FALSE;
}



//------------------------------------------------------------------
//------------------------------------------------------------------
// レールデータ
static void initRailDat( RAIL_SETTING * raildat, const RAIL_SETTING * setting )
{
  GFL_STD_MemCopy( setting, raildat, sizeof(RAIL_SETTING) );
}
// ポイント情報取得
static const RAIL_POINT* getRailDatPoint( const RAIL_SETTING * raildat, u32 index )
{
	if(raildat->point_count > index)
	{
		return &raildat->point_table[ index ];	
	}
	GF_ASSERT( RAIL_TBL_NULL==index );
	return NULL;
}
// ライン情報取得
static const RAIL_LINE* getRailDatLine( const RAIL_SETTING * raildat, u32 index )
{
	if(raildat->line_count > index)
	{
		return &raildat->line_table[ index ];
	}
	GF_ASSERT( RAIL_TBL_NULL==index );
	return NULL;
}
// カメラ情報取得
static const RAIL_CAMERA_SET* getRailDatCamera( const RAIL_SETTING * raildat, u32 index )
{
	if(raildat->camera_count > index)
	{
		return &raildat->camera_table[ index ];
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return &RAIL_CAMERA_SET_Default;
}
// ライン位置計算情報取得
static const RAIL_LINEPOS_SET* getRailDatLinePos( const RAIL_SETTING * raildat, u32 index )
{
	if(raildat->linepos_count > index)
	{
		return &raildat->linepos_table[ index ];	
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return &RAIL_LINEPOS_SET_Default;
}
// カメラ位置計算関数取得
static RAIL_CAMERA_FUNC*const getRailDatCameraFunc( const RAIL_SETTING * raildat, u32 index )
{
	if( raildat->camera_func_count > index )
	{
		return raildat->camera_func[ index ];
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return NULL;
}
// 座標計算関数取得
static RAIL_POS_FUNC*const getRailDatLinePosFunc( const RAIL_SETTING * raildat, u32 index )
{
	if( raildat->linepos_func_count > index )
	{	
		return raildat->line_pos_func[ index ];
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return NULL;
}
// ライン距離取得関数
static RAIL_LINE_DIST_FUNC*const getRailDatLineDistFunc( const RAIL_SETTING * raildat, u32 index )
{
	if( raildat->line_dist_func_count > index )
	{	
		return raildat->line_dist_func[ index ];
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return NULL;
}



//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY updateLineMove_new(FIELD_RAIL_WORK * work, RAIL_KEY key, u32 count_up)
{ //LINE上のときの移動処理
  const RAIL_LINE * nLine = work->line;
  s32 nLine_ofs_max = getLineOfsMax( nLine, work->ofs_unit, work->rail_dat ); // 今のLINEのオフセット最大値
  s32 ofs_max;  // 各ラインのオフセット最大値
	s32 next_line_width_max;
	s32 now_line_width_max;

  if (key == nLine->key)
  {//正方向キーの場合
    const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_e );
    const RAIL_LINE * front = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
    const RAIL_LINE * left = RAILPOINT_getLineByKey(nPoint, getAntiClockwiseKey(key), work->rail_dat);
    const RAIL_LINE * right = RAILPOINT_getLineByKey(nPoint, getClockwiseKey(key), work->rail_dat);
    BOOL width_over = FALSE;
    //const RAIL_LINE * back = RAILPOINT_getLineByKey(nPoint, getReverseKey(key), work->rail_dat);
    TAMADA_Printf("↑");
    work->line_ofs += count_up;
    if (work->line_ofs <= nLine_ofs_max) {
      // 今の道幅チェック オーバーしてなければ、通常の更新
      now_line_width_max = getLineWidthOfsMax( nLine, work->line_ofs, nLine_ofs_max, work->rail_dat );
      if( now_line_width_max >= MATH_ABS(work->width_ofs) )
      {
        return key;
      }
      // オーバーしていたら、他の道に遷移する
      width_over = TRUE;
		}
    if ((front!=NULL) && (width_over == FALSE))
    { //正面移行処理
      debugCheckPointData(nPoint, work->rail_dat);
			// 幅チェック
			ofs_max = getLineOfsMax( front, work->ofs_unit, work->rail_dat );
			next_line_width_max = getLineWidthOfsMax( front, work->line_ofs - nLine_ofs_max, ofs_max, work->rail_dat );
			if( next_line_width_max >= MATH_ABS(work->width_ofs) )
			{
				return setLine(work, front, key,
						/*l_ofs*/work->line_ofs - nLine_ofs_max,
						/*w_ofs*/work->width_ofs,
						/*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max); //そのまま
			}
    }
    if ( (left && work->width_ofs < 0) )
    { //左側移行処理
      if ( isLinePoint_S( work->rail_dat, left, nPoint ) )
      { //始端の場合
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( left, -work->width_ofs, ofs_max, work->rail_dat );
        // 左側ラインの範囲内かチェック
        if( (-work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs-nLine_ofs_max)) )
        {
          return setLine(work, left, key,
              /*l_ofs*/-work->width_ofs,
              /*w_ofs*/(work->line_ofs-nLine_ofs_max),
              /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max); 
        }
      }
      else if (isLinePoint_E( work->rail_dat, left, nPoint ))
      { //終端の場合
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( left, ofs_max+work->width_ofs, ofs_max, work->rail_dat );
        // 左側ラインの範囲内かチェック
        if( (-work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs-nLine_ofs_max)) )
        {
          return setLine(work, left, key,
              /*l_ofs*/ofs_max + work->width_ofs, //width_ofs < 0なので実質減算
              /*w_ofs*/-(work->line_ofs-nLine_ofs_max),
              /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max); 
        }
      }
      else GF_ASSERT_MSG(width_over, "%sから%sへの接続異常\n", nLine->name, left->name);
    }
    if (right && work->width_ofs > 0)
    { //右側移行処理
      if ( isLinePoint_S( work->rail_dat, right, nPoint ) )
      { //始端の場合
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( right, work->width_ofs, ofs_max, work->rail_dat );
        // 右側ラインの範囲内かチェック
        if( (work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs-nLine_ofs_max)) )
        {
          return setLine(work, right, key,
              /*l_ofs*/work->width_ofs,
              /*w_ofs*/-(work->line_ofs-nLine_ofs_max),
              /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max); 
        }
      }
			else if ( isLinePoint_E( work->rail_dat, right, nPoint ) )
      { //終端の場合
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( right, ofs_max - work->width_ofs, ofs_max, work->rail_dat );
        // 右側ラインの範囲内かチェック
        if( (work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs-nLine_ofs_max)) )
        {
          return setLine(work, right, key,
              /*l_ofs*/ofs_max - work->width_ofs,
              /*w_ofs*/(work->line_ofs-nLine_ofs_max),
              /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        }
      }
      else GF_ASSERT_MSG(width_over, "%sから%sへの接続異常\n", nLine->name, right->name);
    }

    if( width_over )
    {
			return updateLine( work, nLine, nLine_ofs_max, key );
    }

    //行くあてがない…LINEそのまま、加算をとりけし
    work->line_ofs = nLine_ofs_max;
    return key;
  }
  else if (key == getReverseKey(nLine->key))
  {//逆方向キーの場合
    const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_s );
    //const RAIL_LINE * front = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
    const RAIL_LINE * left = RAILPOINT_getLineByKey(nPoint, getClockwiseKey(key), work->rail_dat);
    const RAIL_LINE * right = RAILPOINT_getLineByKey(nPoint, getAntiClockwiseKey(key), work->rail_dat);
    const RAIL_LINE * back = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
    BOOL width_over = FALSE;
    TAMADA_Printf("↓");
    work->line_ofs -= count_up;
    if (work->line_ofs >= 0) {

      // 今の道幅チェック オーバーしてなければ、通常の更新
      now_line_width_max = getLineWidthOfsMax( nLine, work->line_ofs, nLine_ofs_max, work->rail_dat );
      if( now_line_width_max >= MATH_ABS(work->width_ofs) )
      {
        return key;
      }
      // オーバーしていたら、他の道に遷移する
      width_over = TRUE;
		}
    if ((back != NULL) && (width_over == FALSE))
    {//背面移行処理
      debugCheckPointData(nPoint, work->rail_dat);
      ofs_max = getLineOfsMax( back, work->ofs_unit, work->rail_dat );
			// 幅チェック
			next_line_width_max = getLineWidthOfsMax( back, ofs_max - MATH_ABS(work->line_ofs), ofs_max, work->rail_dat );
			if( next_line_width_max >= MATH_ABS(work->width_ofs) )
			{
	      return setLine(work, back, key,
		        /*l_ofs*/ofs_max - MATH_ABS(work->line_ofs),	// ここには、必ず負の値が来る
			      /*w_ofs*/work->width_ofs,
				    /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
			}
    }
    if (left && work->width_ofs < 0)
    { //左側移行処理
			if ( isLinePoint_S( work->rail_dat, left, nPoint ) )
      { //始端の場合
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( left, -work->width_ofs, ofs_max, work->rail_dat );
        // 左側ラインの範囲内かチェック
        if( (-work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs)) )
        {
          return setLine(work, left, key,
              /*l_ofs*/-work->width_ofs,
              /*w_ofs*/work->line_ofs,
            /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        }
      }
			else if ( isLinePoint_E( work->rail_dat, left, nPoint ) )
      { //終端の場合
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( left, ofs_max+work->width_ofs, ofs_max, work->rail_dat );
        // 左側ラインの範囲内かチェック
        if( (-work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs)) )
        {
          return setLine(work, left, key,
              /*l_ofs*/ofs_max + work->width_ofs, //width_ofs < 0なので実質減算
              /*w_ofs*/-work->line_ofs,
            /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        }
      }
      else GF_ASSERT_MSG(width_over, "%sから%sへの接続異常\n", nLine->name, left->name);
    }
    if (right && work->width_ofs > 0)
    { //右側移行処理
			if ( isLinePoint_S( work->rail_dat, right, nPoint ) )
      { //始端の場合
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( right, work->width_ofs, ofs_max, work->rail_dat );
        // 右側ラインの範囲内かチェック
        if( (work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs)) )
        {
          return setLine(work, right, key,
              /*l_ofs*/work->width_ofs,
              /*w_ofs*/-work->line_ofs,
            /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        } 
      }
			if ( isLinePoint_E( work->rail_dat, right, nPoint ) )
      { //終端の場合
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( right, ofs_max - work->width_ofs, ofs_max, work->rail_dat );
        // 右側ラインの範囲内かチェック
        if( (work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs)) )
        {
          return setLine(work, right, key,
              /*l_ofs*/ofs_max - work->width_ofs,
              /*w_ofs*/work->line_ofs,
            /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        }
      }
      else GF_ASSERT_MSG(width_over, "%sから%sへの接続異常\n", nLine->name, right->name);
    }

    //  幅オーバー処理
    if( width_over )
    {
			return updateLine( work, nLine, nLine_ofs_max, key );
    }
    //行くあてがない…LINEそのまま、減算を取り消し
    work->line_ofs = 0;
    return key;
  }
  else if (key == getClockwiseKey(nLine->key))
  {//時計回り隣方向キーの場合
    TAMADA_Printf("→");
    work->width_ofs += count_up;
    if (work->width_ofs <= work->width_ofs_max)
    {//範囲内の場合、終了
			return updateLine( work, nLine, nLine_ofs_max, key );
    }
		{
			const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_s );
			const RAIL_LINE *right = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
			if(right)
			{
				if ( isLinePoint_S( work->rail_dat, right, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( right, work->width_ofs, ofs_max, work->rail_dat );
					//右側LINEは始端からの場合
					if(work->line_ofs <= next_line_width_max)
					{
						return setLine(work, right, key,
								work->width_ofs,
								- work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
				if ( isLinePoint_E( work->rail_dat, right, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( right, ofs_max - work->width_ofs, ofs_max, work->rail_dat );
					//右側LINEは始端からの場合
					if(work->line_ofs <= next_line_width_max)
					{
						return setLine(work, right, key,
								ofs_max - work->width_ofs,
								work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
			}
		}
		{
			const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_e );
			const RAIL_LINE *right = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
			if(right)
			{
				if ( isLinePoint_S( work->rail_dat, right, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( right, work->width_ofs, ofs_max, work->rail_dat );
					//右側LINEは終端からの場合
					if( work->line_ofs >= (nLine_ofs_max - next_line_width_max) )
					{
						return setLine(work, right, key,
								work->width_ofs,
								nLine_ofs_max - work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
				if ( isLinePoint_E( work->rail_dat, right, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( right, ofs_max - work->width_ofs, ofs_max, work->rail_dat );
					//右側LINEは終端からの場合
					if( work->line_ofs >= (nLine_ofs_max - next_line_width_max) )
					{
						return setLine(work, right, key,
								ofs_max - work->width_ofs,
								- (nLine_ofs_max - work->line_ofs),
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}	
				}
			}
    }
    //中間地点の場合、行くあてがない場合…LINEそのまま、加算を取り消し
    work->width_ofs = work->width_ofs_max;
    return key;
  }
  else if (key == getAntiClockwiseKey(nLine->key))
  {//反時計回り隣方向キーの場合
    TAMADA_Printf("←");
    work->width_ofs -= count_up;
    if (MATH_ABS(work->width_ofs) <= work->width_ofs_max)
    {//範囲内の場合、終了
			return updateLine( work, nLine, nLine_ofs_max, key );
    }
		{
      const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_s );
      const RAIL_LINE* left = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
			if(left)
			{
				if ( isLinePoint_S( work->rail_dat, left, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( left, MATH_ABS(work->width_ofs), ofs_max, work->rail_dat );
					//左側LINEは始端からの場合
					if(work->line_ofs <= next_line_width_max)
					{
						return setLine(work, left, key,
								MATH_ABS(work->width_ofs), 
								work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
				if ( isLinePoint_E( work->rail_dat, left, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( left, ofs_max - MATH_ABS(work->width_ofs), ofs_max, work->rail_dat );
					//左側LINEは始端からの場合
					if(work->line_ofs <= next_line_width_max)
					{
						return setLine(work, left, key,
								ofs_max - MATH_ABS(work->width_ofs),
								- work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
			}
		}
		{
      const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_e );
      const RAIL_LINE* left = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
			if(left)
			{
				if ( isLinePoint_S( work->rail_dat, left, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( left, MATH_ABS(work->width_ofs), ofs_max, work->rail_dat );
					//左側LINEは終端からの場合
					if( work->line_ofs >= (nLine_ofs_max - next_line_width_max) )
					{
						return setLine(work, left, key,
								MATH_ABS(work->width_ofs),
								- (nLine_ofs_max - work->line_ofs),
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
				if ( isLinePoint_E( work->rail_dat, left, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( left, ofs_max - MATH_ABS(work->width_ofs), ofs_max, work->rail_dat );
					//左側LINEは終端からの場合
					if( work->line_ofs >= (nLine_ofs_max - next_line_width_max) )
					{
						return setLine(work, left, key,
								ofs_max - MATH_ABS(work->width_ofs),
								nLine_ofs_max - work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}	
				}
			}
		}
    //中間地点の場合、行くあてがない場合…LINEそのまま、減算を取り消し
    work->width_ofs += count_up;
    return key;
  }
  return RAIL_KEY_NULL;
}

#if 0
//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY updateLineMove(FIELD_RAIL_WORK * work, RAIL_KEY key)
{ //LINE上のときの移動処理
  const RAIL_LINE * nLine = work->line;
  if (key == nLine->key)
  {//正方向キーの場合
    work->line_ofs ++;
    if (work->line_ofs == work->line_ofs_max)
    { // LINE --> point_e への移行処理
      work->type = FIELD_RAIL_TYPE_POINT;
      work->point = nLine->point_e;
      debugCheckPointData(work->point);
    }
    return key;
  }
  else if (key == getReverseKey(nLine->key))
  {//逆方向キーの場合
    work->line_ofs --;
    if (work->line_ofs == 0)
    { // LINE --> POINT_S への移行処理
      work->type = FIELD_RAIL_TYPE_POINT;
      work->point = nLine->point_s;
      debugCheckPointData(work->point);
    }
    return key;
  }
  else if (key == getClockwiseKey(nLine->key))
  {//時計回りとなり方向キーの場合
    if (work->width_ofs < work->width_ofs_max)
    {
      work->width_ofs ++;
    }
    return key;
  }
  else if (key == getAntiClockwiseKey(nLine->key))
  {//反時計回りとなり方向キーの場合
    if (work->width_ofs > - work->width_ofs_max)
    {
      work->width_ofs --;
    }
    return key;
  }
  return RAIL_KEY_NULL;
}
#endif

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY updatePointMove(FIELD_RAIL_WORK * work, RAIL_KEY key, u32 count_up)
{//POINT上のときの移動処理
  int i;
  const RAIL_POINT * nPoint = work->point;
  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
    if (nPoint->keys[i] == key)
    { // POINT --> LINE[i] への移行処理
      const RAIL_LINE * nLine = getRailDatLine( work->rail_dat, nPoint->lines[i] );
      s32 ofs_max;

      // 
      ofs_max = getLineOfsMax( nLine, work->ofs_unit, work->rail_dat );
      
			if ( isLinePoint_S( work->rail_dat, nLine, nPoint ) )
      { 
        setLine(work, nLine, key, count_up, 0, ofs_max, nPoint->width_ofs_max[i]);
        //work->line_ofs = 1;  //最初は１から！
      }
			else if ( isLinePoint_E( work->rail_dat, nLine, nPoint ) )
      {
        setLine(work, nLine, key, ofs_max - count_up, 0, ofs_max, nPoint->width_ofs_max[i]);
        //work->line_ofs = nLine->ofs_max - 1; //最後ー1から！
      }
      //つながったLINEを次のRAILとしてセット
      //work->type = FIELD_RAIL_TYPE_LINE;
      //work->line = nLine;
      //debugCheckLineData(work->line, work->rail_dat);
      return key;
    }
  }
  return RAIL_KEY_NULL;
}



//============================================================================================
/**
 */
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void initRail(FIELD_RAIL_WORK * work, const RAIL_SETTING* rail_dat )
{
  work->type = FIELD_RAIL_TYPE_MAX;
  work->dummy = NULL;
  work->line_ofs = 0;
  work->width_ofs = 0;
  work->key = RAIL_KEY_NULL;
	work->rail_dat = rail_dat;
  work->ofs_unit = rail_dat->ofs_unit;


  work->last_frame = 0;
  work->last_key = RAIL_KEY_NULL;

  work->req_move = FALSE;

  work->active = FALSE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL isValidRail(const FIELD_RAIL_WORK * work)
{
  if (work->type < FIELD_RAIL_TYPE_MAX && work->dummy != NULL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static const RAIL_CAMERA_SET * getCameraSet(const FIELD_RAIL_WORK * work)
{
  switch (work->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    return getRailDatCamera( work->rail_dat, work->point->camera_set );
    break;
  case FIELD_RAIL_TYPE_LINE:
    return getRailDatCamera( work->rail_dat, work->line->camera_set);
    break;
  }
  GF_ASSERT(0);
  return &RAIL_CAMERA_SET_Default;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void getRailPosition(const FIELD_RAIL_WORK * work, VecFx32 * pos)
{
  RAIL_POS_FUNC * func = NULL;
	const RAIL_LINEPOS_SET* line_pos_set;

  if(work->type == FIELD_RAIL_TYPE_POINT)
  {
    //POINTにいるときはPOINTの座標
    *pos = work->point->pos;
  }
  else if(work->type == FIELD_RAIL_TYPE_LINE)
  {
		line_pos_set = getRailDatLinePos( work->rail_dat, work->line->line_pos_set );
    func = getRailDatLinePosFunc( work->rail_dat, line_pos_set->func_index );
    if (func)
    {
			func(work, pos);
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ラインのオフセット最大数を求める
 *
 *	@param	line    ライン情報
 *	@param	unit    １オフセットの距離
 *
 *	@return オフセット最大数
 */
//-----------------------------------------------------------------------------
static s32 getLineOfsMax( const RAIL_LINE * line, fx32 unit, const RAIL_SETTING* rail_dat )
{
  const RAIL_LINEPOS_SET * line_pos_set;
	const RAIL_POINT* point_s;
	const RAIL_POINT* point_e;
	RAIL_LINE_DIST_FUNC* dist_func;
  fx32 dist;
  s32 div;
  s32 amari;

	point_s = getRailDatPoint( rail_dat, line->point_s );
	point_e = getRailDatPoint( rail_dat, line->point_e );

  // ライン動作情報を取得
  line_pos_set = getRailDatLinePos( rail_dat, line->line_pos_set );

	// 距離の取得
	dist_func = getRailDatLineDistFunc( rail_dat, line_pos_set->func_dist_index );
	if( dist_func )
	{
		dist = dist_func( point_s, point_e, line_pos_set );
		div   = FX_Div( dist, unit ) >> FX32_SHIFT; // 小数点は無視
	}
	else
	{
		div = 0;
	}

  // RAIL_WALK_OFSで割り切れる値にする
  amari = div % RAIL_WALK_OFS;
  if( amari >=  (RAIL_WALK_OFS/2) )
  {
    div += RAIL_WALK_OFS - amari;
    TOMOYA_Printf( "ラインオフセット　あまり　があります。 amari=%d\n", amari );
  }
  else if( amari )
  {
    div -= amari;
    TOMOYA_Printf( "ラインオフセット　あまり　があります。 amari=%d\n", amari );
  }

//  OS_TPrintf( "div = %d dist = 0x%x unit = 0x%x\n", div, dist, unit );

  return div;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ライン幅オフセット最大値を求める
 *
 *	@param	line
 *	@param	line_ofs		ライン　オフセット値（進み具合）
 *	@param	rail_dat 
 *
 *	@return	幅オフセット最大値
 */
//-----------------------------------------------------------------------------
static s32 getLineWidthOfsMax( const RAIL_LINE * line, u32 line_ofs, u32 line_ofs_max, const RAIL_SETTING* rail_dat )
{
	s32 width_ofs_max;
	s32 width_ofs_div;
	const RAIL_POINT* cp_point_s;
	const RAIL_POINT* cp_point_e;
	int i, j;
	int width_s, width_e;

	// ラインの始点と終点を取得
	GF_ASSERT( line->point_s != RAIL_TBL_NULL );
	cp_point_s = &rail_dat->point_table[ line->point_s ];
	GF_ASSERT( line->point_e != RAIL_TBL_NULL );
	cp_point_e = &rail_dat->point_table[ line->point_e ];

	// 一致するラインの幅を取得
	width_e = -1;
	width_s = -1;
	for( i=0; i<RAIL_CONNECT_LINE_MAX; i++ )
	{
		if( cp_point_s->lines[i] != RAIL_TBL_NULL )
		{
			if( (u32)(&rail_dat->line_table[cp_point_s->lines[i]]) == (u32)(line) )
			{
				width_s = cp_point_s->width_ofs_max[i];
			}
		}

		if( cp_point_e->lines[i] != RAIL_TBL_NULL )
		{
			if( (u32)(&rail_dat->line_table[cp_point_e->lines[i]]) == (u32)(line) )
			{
				width_e = cp_point_e->width_ofs_max[i];
			}
		}
	}
	if( width_s < 0 )
	{
		width_s = width_e;
	}
	if( width_e < 0 )
	{
		width_e = width_s;
	}
	GF_ASSERT( width_s >= 0 );

	width_ofs_div = width_e - width_s;
	width_ofs_max = (width_ofs_div * (s32)line_ofs) / (s32)line_ofs_max;
	width_ofs_max += width_s;
	

	return width_ofs_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ライン動作　通常更新（幅チェックをする）
 *
 *	@param	work					レールシステム
 *	@param	line					進行中のライン
 *	@param	line_ofs_max	ラインオフセット最大値
 *	@param	key						押されているキー
 *
 *	@return	情報を変更したキー
 */
//-----------------------------------------------------------------------------
static RAIL_KEY updateLine( FIELD_RAIL_WORK * work, const RAIL_LINE * line, u32 line_ofs_max, u32 key )
{
  s32 width_ofs_max = getLineWidthOfsMax( line, work->line_ofs, line_ofs_max, work->rail_dat ); // 今のLINEのオフセット最大値

	work->width_ofs_max = width_ofs_max;
	if( MATH_ABS(work->width_ofs) >= width_ofs_max )
	{
		if(work->width_ofs > 0)
		{
			work->width_ofs = width_ofs_max;
		}
		else
		{
			work->width_ofs = -width_ofs_max;
		}
	}

	return key;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static int RailKeyTokeyCont(RAIL_KEY key)
{
  static const u16 sc_KEYCONT[ RAIL_KEY_MAX ] = 
  {
    0,
    PAD_KEY_UP,
    PAD_KEY_RIGHT,
    PAD_KEY_DOWN,
    PAD_KEY_LEFT,
  };

  return sc_KEYCONT[ key ];
}

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
static RAIL_KEY keyContToRailKey(int cont)
{
  if (cont & PAD_KEY_UP) return RAIL_KEY_UP;
  if (cont & PAD_KEY_LEFT) return RAIL_KEY_LEFT;
  if (cont & PAD_KEY_RIGHT) return RAIL_KEY_RIGHT;
  if (cont & PAD_KEY_DOWN) return RAIL_KEY_DOWN;
  return RAIL_KEY_NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY getReverseKey(RAIL_KEY key)
{
  switch (key) {
  case RAIL_KEY_UP: 
    return RAIL_KEY_DOWN;
  case RAIL_KEY_DOWN:
    return RAIL_KEY_UP;
  case RAIL_KEY_LEFT:
    return RAIL_KEY_RIGHT;
  case RAIL_KEY_RIGHT:
    return RAIL_KEY_LEFT;
  }
  return RAIL_KEY_NULL;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY getClockwiseKey(RAIL_KEY key)
{
  switch (key) {
  case RAIL_KEY_UP: 
    return RAIL_KEY_RIGHT;
  case RAIL_KEY_DOWN:
    return RAIL_KEY_LEFT;
  case RAIL_KEY_LEFT:
    return RAIL_KEY_UP;
  case RAIL_KEY_RIGHT:
    return RAIL_KEY_DOWN;
  }
  return RAIL_KEY_NULL;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY getAntiClockwiseKey(RAIL_KEY key)
{
  switch (key) {
  case RAIL_KEY_UP: 
    return RAIL_KEY_LEFT;
  case RAIL_KEY_DOWN:
    return RAIL_KEY_RIGHT;
  case RAIL_KEY_LEFT:
    return RAIL_KEY_DOWN;
  case RAIL_KEY_RIGHT:
    return RAIL_KEY_UP;
  }
  return RAIL_KEY_NULL;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static const char * getRailName(const FIELD_RAIL_WORK * work)
{
  switch (work->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    return work->point->name;
  case FIELD_RAIL_TYPE_LINE:
    return work->line->name;
  }
  return "NO ENTRY";
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY searchLineInPoint(const RAIL_POINT *point, const RAIL_LINE * line, const RAIL_SETTING* rail_dat)
{
  int i;
	const RAIL_LINE* point_line;
  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
		point_line = getRailDatLine( rail_dat, point->lines[i] );
    if (point_line == line) return point->keys[i];
  }
  return RAIL_KEY_NULL;
}

//============================================================================================
//
//
//    デバッグ用関数
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL debugCheckLineData(const RAIL_LINE * line, const RAIL_SETTING* rail_dat)
{

  RAIL_KEY key;
	const RAIL_POINT* point_s;
	const RAIL_POINT* point_e;

  if(line->point_s == RAIL_TBL_NULL || line->point_e == RAIL_TBL_NULL)
  {
    OS_TPrintf("%sはPOINTが正しくありません\n", line->name);
    return FALSE;
  }
	point_s = getRailDatPoint( rail_dat, line->point_s );
	point_e = getRailDatPoint( rail_dat, line->point_e );
	
  key = searchLineInPoint(point_s, line, rail_dat);
  if (key == RAIL_KEY_NULL)
  {
    OS_TPrintf("%sは%sと正しく接続できていません\n", line->name, point_s->name);
    return FALSE;
  }
  if (key != line->key)
  {
    OS_TPrintf("%sは始端%sとキーがあっていません\n", line->name, point_s->name);
    return FALSE;
  }
  key = searchLineInPoint(point_e, line, rail_dat);
  if (key == RAIL_KEY_NULL)
  {
    OS_TPrintf("%sは%sと正しく接続できていません\n", line->name, point_e->name);
    return FALSE;
  }
  if (key != getReverseKey(line->key))
  {
    OS_TPrintf("%sは終端%sとキーがあっていません\n", line->name, point_e->name);
    return FALSE;
  }
  return TRUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL debugCheckPointData(const RAIL_POINT * point, const RAIL_SETTING* rail_dat)
{
  int i, j;
	const RAIL_LINE* line;
	const RAIL_LINE* line_sarch;
	const RAIL_POINT* point_s;
	const RAIL_POINT* point_e;

  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
    if ((point->keys[i] == RAIL_KEY_NULL) != (point->lines[i] == RAIL_TBL_NULL) )
    {
      OS_TPrintf("%s:LINEとRAIL_KEYの指定に矛盾があります\n", point->name);
      return FALSE;
    }

    if (point->lines[i] == RAIL_TBL_NULL) continue;

		// ライン情報取得
		line = getRailDatLine( rail_dat, point->lines[i] );
		point_s = getRailDatPoint( rail_dat, line->point_s );
		point_e = getRailDatPoint( rail_dat, line->point_e );
		
    if (point_s != point && point_e != point)
    {
      OS_TPrintf("%sは%sと接続していません\n",point->name, line->name);
      return FALSE;
    }

    if (point->keys[i] != RAIL_KEY_NULL)
    {
      for (j = i+1; j < RAIL_CONNECT_LINE_MAX; j++)
      {
        if (point->keys[i] == getReverseKey(point->keys[j]))
        {
					line_sarch = getRailDatLine( rail_dat, point->lines[j] );
          if (line->key != line_sarch->key)
          {
            OS_Printf("%s から %s 経由で %s へ向かうキーに矛盾があります\n",
                line->name, point->name, line_sarch->name);
            return FALSE;
          }
        }
      }
    }
  }
  return TRUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintWholeVector(const char * before, const VecFx32 * vec, const char * after)
{
  if (before)
  {
    OS_TPrintf("%s",before);
  }
  OS_TPrintf("( %d, %d, %d)", FX_Whole(vec->x), FX_Whole(vec->y), FX_Whole(vec->z));
  if (after)
  {
    OS_TPrintf("%s",after);
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintHexVector(const VecFx32 * vec)
{
  OS_TPrintf("(%08x, %08x, %08x)\n", (vec->x), (vec->y), (vec->z));
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintPoint(const char * before, const RAIL_POINT * point, const char * after)
{
  if (point != NULL)
  {
    debugPrintWholeVector(before, &point->pos, after);
  }
  else
  {
    if (before) OS_TPrintf("%s", before);
    OS_TPrintf("( NULL )");
    if (after) OS_TPrintf("%s", after);
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintDegree(const VecFx32 * p0, const VecFx32 * p1)
{
    VecFx32 s,e;
    fx32 angle;
    VEC_Normalize( p0, &s );
    VEC_Normalize( p1, &e );
    angle = FX_AcosIdx( VEC_DotProduct( &s, &e ) );
    OS_TPrintf("Degree: %08x\n",angle);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static const char * debugGetRailKeyName(RAIL_KEY key)
{
    static const char * const names[] = {
      "RAIL_KEY_NULL","RAIL_KEY_UP","RAIL_KEY_RIGHT","RAIL_KEY_DOWN","RAIL_KEY_LEFT"
    };
    GF_ASSERT(key < RAIL_KEY_MAX);
    return names[key];
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintRailInfo(const FIELD_RAIL_WORK * work)
{
  FIELD_RAIL_TYPE type = work->type;
  OS_TPrintf("%d(%s:%08x)\n",
      type,
      getRailName(work),
      work->dummy);
  if (type == FIELD_RAIL_TYPE_POINT)
  {
    debugPrintPoint(NULL, work->point ,"\n");
  } else if (type == FIELD_RAIL_TYPE_LINE)
  {
		const RAIL_POINT* point_s;
		const RAIL_POINT* point_e;

		point_s = getRailDatPoint( work->rail_dat, work->line->point_s );
		point_e = getRailDatPoint( work->rail_dat, work->line->point_e );
    debugPrintPoint("start", point_s, NULL);
    debugPrintPoint("-end", point_e, "\n");
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  ポイントのテーブルインデックスを取得
 *
 *	@param	man       マネージャ
 *	@param	point     ポイント
 *
 *	@return インデックス
 */
//-----------------------------------------------------------------------------
static u32 getPointIndex( const RAIL_SETTING * raildat, const RAIL_POINT* point )
{
  int i;

  for( i=0; i<raildat->point_count; i++ ){
    if( (u32)&raildat->point_table[i] == (u32)point ){
      return i;
    }
  }

  GF_ASSERT_MSG( i<raildat->point_count, "不明なpointです。" );
  return 0; // フリーズ回避
}

//----------------------------------------------------------------------------
/**
 *	@brief  ラインのテーブルインデックスを取得
 *
 *	@param	man       マネージャ
 *	@param	line      ライン
 *
 *	@return インデックス
 */
//-----------------------------------------------------------------------------
static u32 getLineIndex( const RAIL_SETTING * raildat, const RAIL_LINE* line )
{
  int i;

  for( i=0; i<raildat->line_count; i++ ){
    if( (u32)&raildat->line_table[i] == (u32)line ){
      return i;
    }
  }

  GF_ASSERT_MSG( i<raildat->line_count, "不明なlineです。" );
  return 0; // フリーズ回避
}

