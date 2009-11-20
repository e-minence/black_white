////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  フィールドタスク(カメラの回転処理)
 * @file   field_task_camera_rot.h
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------------------
/**
 * @brief カメラのヨー回転タスクを作成する
 *
 * @param fieldmap タスク動作対象のフィールドマップ
 * @param frame    タスク動作フレーム数
 * @param angle    最終的な角度( 2πラジアンを65536分割した値を単位とする数 )
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraRot_Yaw( FIELDMAP_WORK* fieldmap, int frame, u16 angle );

//------------------------------------------------------------------------------------------
/**
 * @brief カメラのピッチ回転タスクを作成する
 *
 * @param fieldmap タスク動作対象のフィールドマップ
 * @param frame    タスク動作フレーム数
 * @param angle    最終的な角度( 2πラジアンを65536分割した値を単位とする数 )
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraRot_Pitch( FIELDMAP_WORK* fieldmap, int frame, u16 angle );
