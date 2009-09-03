//============================================================================================
/**
 * @file  field_rail.h
 * @brief ノーグリッドでの自機・カメラ制御構造
 * @author  tamada, tomoya
 * @date  2009.05.18
 *
 */
//============================================================================================

#pragma once

#include "field_camera.h"
#include "field/field_rail_loader.h"
#include "field/rail_location.h"


//------------------------------------------------------------------
// レールシステム、WBでのワーク数
//------------------------------------------------------------------
#define FIELD_RAIL_WORK_MAX (48)


//------------------------------------------------------------------
// 歩きオフセット
// 1歩は、RAIL_WALK_OFS分の距離になります。
//------------------------------------------------------------------
#define RAIL_WALK_OFS	(16)


//------------------------------------------------------------------
// RAILグリッド　と　オフセット　変換
//------------------------------------------------------------------
// RAIL_GRID -> OFS
#define RAIL_GRID_TO_OFS(x)  ( (x) * RAIL_WALK_OFS )
// RAIL_ OFS -> GRID
#define RAIL_OFS_TO_GRID(x)  ( (x) / RAIL_WALK_OFS )


//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _FIELD_RAIL_MAN FIELD_RAIL_MAN;

//------------------------------------------------------------------
// 1つのレール動作ワーク
//------------------------------------------------------------------
typedef struct _FIELD_RAIL_WORK FIELD_RAIL_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _RAIL_POINT RAIL_POINT;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _RAIL_LINE RAIL_LINE;



//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
#define RAIL_CAMERA_SET_WORK_SIZ	(32)
typedef struct _RAIL_CAMERA_SET{
  u32 func_index;
	union
	{
		struct
		{
			u32 param0;
			u32 param1;
			u32 param2;
			u32 param3;
			u32 param4;
			u32 param5;
			u32 param6;
			u32 param7;
		};
		u8 work[RAIL_CAMERA_SET_WORK_SIZ];
	};
}RAIL_CAMERA_SET;

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
#define RAIL_LINEPOS_SET_WORK_SIZ	(32)
typedef struct _RAIL_LINEPOS_SET{
  u32 func_index;
  u32 func_dist_index;
	union
	{
		struct
		{
			u32 param0;
			u32 param1;
			u32 param2;
			u32 param3;
			u32 param4;
			u32 param5;
			u32 param6;
			u32 param7;
		};
		u8 work[RAIL_CAMERA_SET_WORK_SIZ];
	};
}RAIL_LINEPOS_SET;



//------------------------------------------------------------------
//------------------------------------------------------------------
typedef void (RAIL_POS_FUNC)(const FIELD_RAIL_WORK * work, VecFx32 * pos);
typedef void (RAIL_CAMERA_FUNC)(const FIELD_RAIL_MAN * man);
typedef fx32 (RAIL_LINE_DIST_FUNC)(const RAIL_POINT * point_s, const RAIL_POINT * point_e, const RAIL_LINEPOS_SET * line_pos_set );

//------------------------------------------------------------------
/**
 * @brief キーの定義
 *
 * どの入力でどういう方向への移動になるか？の結びつけのための定義
 */
//------------------------------------------------------------------
typedef enum {
  RAIL_KEY_NULL = 0,
  RAIL_KEY_UP,
  RAIL_KEY_RIGHT,
  RAIL_KEY_DOWN,
  RAIL_KEY_LEFT,

  RAIL_KEY_MAX
}RAIL_KEY;

//------------------------------------------------------------------
/**
 * @brief レールタイプ
 *
 * LOCATIONなどの内容が、POINTなのかLINEなのかを判断するための定義
 */
//------------------------------------------------------------------
typedef enum {  
  FIELD_RAIL_TYPE_POINT = 0,
  FIELD_RAIL_TYPE_LINE,

  FIELD_RAIL_TYPE_MAX
}FIELD_RAIL_TYPE;


//------------------------------------------------------------------
/**
 * @brief   １歩速度指定
 *
 * 外部から、移動速度、を指定するためのコマンド
 */
//------------------------------------------------------------------
typedef enum {  
  RAIL_FRAME_16,
  RAIL_FRAME_8,
  RAIL_FRAME_4,
  RAIL_FRAME_2,
  RAIL_FRAME_1,

  RAIL_FRAME_MAX,
}RAIL_FRAME;


//------------------------------------------------------------------
/**
 * @brief テーブルインデックス無効値
 */
//------------------------------------------------------------------
#define RAIL_TBL_NULL	( 0xffffffff )


//-----------------------------------------------------------------------------
/**
 *		名称サイズ
 */
//-----------------------------------------------------------------------------
#define RAIL_NAME_BUF_SIZ	( 48 )

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
enum {  
  RAIL_CONNECT_LINE_MAX = 4,
  RAIL_CONNECT_POINT_MAX = 2,
};


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
struct _RAIL_POINT {  
  //keys[n]にマッチしたらlines[n]に移動する
	u32 lines[RAIL_CONNECT_LINE_MAX];
  u32 keys[RAIL_CONNECT_LINE_MAX];
	s32	width_ofs_max[RAIL_CONNECT_LINE_MAX];

  ///POINTの位置座標
  VecFx32 pos;

	u32 camera_set;
  char name[ RAIL_NAME_BUF_SIZ ];
};


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
struct _RAIL_LINE {
  //point_s --> point_eへの動きをkey方向を正として制御する
	u32 point_s;
	u32 point_e;
  u32 key;
	u32 line_pos_set;

	u32 camera_set;


  char name[ RAIL_NAME_BUF_SIZ ];
  //いずれ必要になるが今考えると混乱する
  //fx32 width_margin;  ///<LINEに対して動ける幅
  //u32 width_divider;
};


//------------------------------------------------------------------
/// レール情報
//------------------------------------------------------------------
struct _RAIL_SETTING{
  u16 point_count;
  u16 line_count;
  u16 camera_count;
  u16 linepos_count;
  u16 camera_func_count;
  u16 linepos_func_count;
  u16 line_dist_func_count;
	u16 dummy00;
  const RAIL_POINT* point_table;
  const RAIL_LINE*	line_table;
	const RAIL_CAMERA_SET*	camera_table;
	const RAIL_LINEPOS_SET* linepos_table;
	RAIL_CAMERA_FUNC*const*	camera_func;
	RAIL_POS_FUNC*const*		line_pos_func;
	RAIL_LINE_DIST_FUNC*const* line_dist_func;

  fx32  ofs_unit;     // 幅の移動単位
} ;



//============================================================================================
//
//
//  レールシステム制御のための関数群
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
extern FIELD_RAIL_MAN * FIELD_RAIL_MAN_Create(HEAPID heapID, u32 work_num, FIELD_CAMERA * camera);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Delete(FIELD_RAIL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Load(FIELD_RAIL_MAN * man, const RAIL_SETTING * setting);
extern void FIELD_RAIL_MAN_Clear(FIELD_RAIL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern BOOL FIELD_RAIL_MAN_IsLoad( const FIELD_RAIL_MAN * man );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Update(FIELD_RAIL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_UpdateCamera(const FIELD_RAIL_MAN * man);


//------------------------------------------------------------------
// 基準となるレールグリッドサイズの取得
// *ないりんさなどがあるため、必ずこの値ではない
//------------------------------------------------------------------
extern fx32 FIELD_RAIL_MAN_GetRailGridSize( const FIELD_RAIL_MAN * man );

// ロケーションでの計算
extern BOOL FIELD_RAIL_MAN_CalcRailKeyLocation(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * now_location, RAIL_KEY key, RAIL_LOCATION * next_location);
extern void FIELD_RAIL_MAN_GetLocationPosition(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * location, VecFx32* pos );

//------------------------------------------------------------------
// 移動管理1物体の生成・破棄
//------------------------------------------------------------------
extern FIELD_RAIL_WORK* FIELD_RAIL_MAN_CreateWork( FIELD_RAIL_MAN * man );
extern void FIELD_RAIL_MAN_DeleteWork( FIELD_RAIL_MAN * man, FIELD_RAIL_WORK* work );

//------------------------------------------------------------------
// バインドするFIELD_RAIL_WORKの設定
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_BindCamera( FIELD_RAIL_MAN * man, const FIELD_RAIL_WORK* work );
extern void FIELD_RAIL_MAN_UnBindCamera( FIELD_RAIL_MAN * man );
extern void FIELD_RAIL_MAN_GetBindWorkPos( const FIELD_RAIL_MAN * man, VecFx32* pos );
#ifdef PM_DEBUG
// 仮処理 色々な部分で、RAILMANからLocationなどを取得しているため、作成
// これを使用している部分は後々は、PLAYERの情報から取得できるようになるはず
extern FIELD_RAIL_WORK* FIELD_RAIL_MAN_DEBUG_GetBindWork( const FIELD_RAIL_MAN * man );
#endif

//------------------------------------------------------------------
//  たぶんデバッグ用途のみ
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_SetActiveFlag(FIELD_RAIL_MAN * man, BOOL flag);
extern BOOL FIELD_RAIL_MAN_GetActiveFlag(const FIELD_RAIL_MAN *man);


//============================================================================================
//
//
//  レール移動制御のための関数群
//
//
//============================================================================================
//------------------------------------------------------------------
// １歩移動リクエスト
//------------------------------------------------------------------
extern BOOL FIELD_RAIL_WORK_ForwardReq( FIELD_RAIL_WORK * work, RAIL_FRAME frame, RAIL_KEY key );

extern void FIELD_RAIL_WORK_SetLocation(FIELD_RAIL_WORK * work, const RAIL_LOCATION * location);
extern void FIELD_RAIL_WORK_GetLocation(const FIELD_RAIL_WORK * work, RAIL_LOCATION * location);
extern void FIELD_RAIL_WORK_GetLastLocation(const FIELD_RAIL_WORK * work, RAIL_LOCATION * location);
extern BOOL FIELD_RAIL_WORK_CheckLocation( const FIELD_RAIL_WORK * work, const RAIL_LOCATION * location );


//------------------------------------------------------------------
//  動作アップデート
//------------------------------------------------------------------
extern void FIELD_RAIL_WORK_Update(FIELD_RAIL_WORK * work);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_WORK_GetPos(const FIELD_RAIL_WORK * work, VecFx32 * pos);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern RAIL_FRAME FIELD_RAIL_WORK_GetActionFrame( const FIELD_RAIL_WORK * work );
extern RAIL_KEY FIELD_RAIL_WORK_GetActionKey( const FIELD_RAIL_WORK * work );
extern BOOL FIELD_RAIL_WORK_IsAction( const FIELD_RAIL_WORK * work );
extern BOOL FIELD_RAIL_WORK_GetLastAction( const FIELD_RAIL_WORK * work );


//------------------------------------------------------------------
// ワーク単位の動作設定
//------------------------------------------------------------------
extern void FIELD_RAIL_WORK_SetActiveFlag( FIELD_RAIL_WORK * work, BOOL flag );
extern BOOL FIELD_RAIL_WORK_IsActive( const FIELD_RAIL_WORK * work );



//------------------------------------------------------------------
// 判定系ツール
//------------------------------------------------------------------
extern BOOL FIELD_RAIL_TOOL_HitCheckSphere( const VecFx32* person, const VecFx32* check, fx32 r );

