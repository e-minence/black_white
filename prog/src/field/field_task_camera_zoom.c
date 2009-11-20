////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  フィールドタスク(カメラのズーム処理)
 * @file   field_task_camera_zoom.c
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <gflib.h>
#include "field_task.h"
#include "fieldmap.h"
#include "field/field_camera.h"
#include "field_task_camera_zoom.h"


//==========================================================================================
// ■タスクワーク
//==========================================================================================
typedef struct
{
  u8             seq;       // シーケンス
  FIELDMAP_WORK* fieldmap;  // 処理対象のフィールドマップ
  u16            frame;     // 経過フレーム数
  u16            endFrame;  // 終了フレーム
  fx32           startDist; // カメラ距離初期値
  fx32           moveDist;  // 移動距離
  fx32           endDist;   // カメラ距離最終値

} ZOOM_WORK;


//==========================================================================================
// ■非公開関数のプロトタイプ宣言
//==========================================================================================
static RETVAL_MAIN CameraLinearZoom( void* wk );
static RETVAL_MAIN CameraSharpZoom( void* wk );


//------------------------------------------------------------------------------------------
/**
 * @brief カメラを等速ズームさせるタスクを作成する
 *
 * @param fieldmap 処理対象のフィールドマップ
 * @param frame    ズーム完了までのフレーム数
 * @param dist     ズーム距離
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraLinearZoom( FIELDMAP_WORK* fieldmap, u16 frame, fx32 dist )
{
  FIELD_TASK* task;
  ZOOM_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // 生成
  task = FIELD_TASK_Create( heap_id, sizeof(ZOOM_WORK), CameraLinearZoom );
  // 初期化
  work = FIELD_TASK_GetWork( task );
  work->seq       = 0;
  work->fieldmap  = fieldmap;
  work->frame     = 0;
  work->endFrame  = frame;
  work->moveDist  = dist;

  return task;
}

//------------------------------------------------------------------------------------------
/**
 * @brief カメラを急加速ズームさせるタスクを作成する
 *
 * @param fieldmap 処理対象のフィールドマップ
 * @param frame    ズーム完了までのフレーム数
 * @param dist     ズーム距離
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraSharpZoom( FIELDMAP_WORK* fieldmap, u16 frame, fx32 dist )
{
  FIELD_TASK* task;
  ZOOM_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // 生成
  task = FIELD_TASK_Create( heap_id, sizeof(ZOOM_WORK), CameraSharpZoom );
  // 初期化
  work = FIELD_TASK_GetWork( task );
  work->seq       = 0;
  work->fieldmap  = fieldmap;
  work->frame     = 0;
  work->endFrame  = frame;
  work->moveDist  = dist;

  return task;
}


//==========================================================================================
// ■タスク処理関数
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief カメラの等速ズーム
 */
//------------------------------------------------------------------------------------------
static RETVAL_MAIN CameraLinearZoom( void* wk )
{
  ZOOM_WORK*      work = (ZOOM_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );
  FIELD_CAMERA_MODE cam_mode;

  switch( work->seq )
  {
  case 0:
    work->startDist = FIELD_CAMERA_GetAngleLen( camera );
    work->endDist   = FIELD_CAMERA_GetAngleLen( camera ) + work->moveDist;
    work->seq++;
    break;
  case 1:
    work->frame++;
    cam_mode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    { // 線形補完
      float    t = work->frame / (float)work->endFrame;
      float   d1 = FX_FX32_TO_F32( work->startDist );
      float   d2 = FX_FX32_TO_F32( work->endDist );
      float dist = (1-t) * d1 + ( t * d2);
      FIELD_CAMERA_SetAngleLen( camera, FX_F32_TO_FX32( dist ) ); 
    }
    FIELD_CAMERA_ChangeMode( camera, cam_mode );
    if( work->endFrame <= work->frame )
    {
      return FINISH;
    }
    break;
  }
  return CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief カメラの急加速ズーム
 */
//------------------------------------------------------------------------------------------
static RETVAL_MAIN CameraSharpZoom( void* wk )
{
  ZOOM_WORK*      work = (ZOOM_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );
  FIELD_CAMERA_MODE cam_mode;

  switch( work->seq )
  {
  case 0:
    work->startDist = FIELD_CAMERA_GetAngleLen( camera );
    work->endDist   = FIELD_CAMERA_GetAngleLen( camera ) + work->moveDist;
    work->seq++;
    break;
  case 1:
    work->frame++;
    cam_mode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    if( work->frame <= work->endFrame)
    { // 2次関数 f(x) = √ax
      float a, t, d1, d2, dist; 
      a    = 1.0f;
      t    = work->frame / (float)work->endFrame;
      t    = sqrt( a * t );
      d1   = FX_FX32_TO_F32( work->startDist );
      d2   = FX_FX32_TO_F32( work->endDist );
      dist = (1-t) * d1 + ( t * d2);
      FIELD_CAMERA_SetAngleLen( camera, FX_F32_TO_FX32( dist ) ); 
    }
    FIELD_CAMERA_ChangeMode( camera, cam_mode );
    if( work->endFrame <= work->frame )
    {
      return FINISH;
    }
    break;
  }
  return CONTINUE;
}
