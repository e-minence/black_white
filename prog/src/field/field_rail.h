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

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _RAIL_POINT RAIL_POINT;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _RAIL_LINE RAIL_LINE;


typedef struct _RAIL_CAMERA_SET RAIL_CAMERA_SET;

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
//------------------------------------------------------------------
enum {  
  RAIL_CONNECT_LINE_MAX = 4,
  RAIL_CONNECT_POINT_MAX = 2,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
struct _RAIL_POINT {  
  //keys[n]にマッチしたらline[n]に移動する
  const RAIL_LINE * line[RAIL_CONNECT_LINE_MAX];
  RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];

  ///POINTの位置座標
  VecFx32 pos;

  const RAIL_CAMERA_SET * camera_set;
};

//------------------------------------------------------------------
//------------------------------------------------------------------
struct _RAIL_LINE { 
  //point[0] --> point[1]への動きをkeyで制御する
  const RAIL_POINT * point[RAIL_CONNECT_POINT_MAX];
  RAIL_KEY key;

  /// 直線状での動作
  u32 line_divider;

  const RAIL_CAMERA_SET * camera_set;

  //いずれ必要になるが今考えると混乱する
  //fx32 width_margin;  ///<LINEに対して動ける幅
  //u32 width_divider;
};


//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum {  
  FIELD_RAIL_TYPE_POINT = 0,
  FIELD_RAIL_TYPE_LINE,

  FIELD_RAIL_TYPE_MAX
}FIELD_RAIL_TYPE;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {  
  FIELD_RAIL_TYPE type;
  union { 
    RAIL_POINT point;
    RAIL_LINE line;
  };
}FIELD_RAIL;


//============================================================================================
//
//
//  レール制御のための関数群
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _FIELD_RAIL_MAN FIELD_RAIL_MAN;

//------------------------------------------------------------------
//------------------------------------------------------------------
extern FIELD_RAIL_MAN * FIELD_RAIL_MAN_Create(HEAPID heapID);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Delete(FIELD_RAIL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Update(FIELD_RAIL_MAN * man, int key_cont);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Load(FIELD_RAIL_MAN * man, const FIELD_RAIL * railData);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Get3DPos(const FIELD_RAIL_MAN * man, VecFx32 * pos);



