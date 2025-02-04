////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  フィールドタスク(自機の回転処理)
 * @file   field_task_player_rot.h
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------------------
/**
 * @brief 自機の等速回転タスクを作成する
 *
 * @param fieldmap タスクを追加するフィールドマップ
 * @param frame    動作フレーム数
 * @param rot_num  回転回数
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_PlayerRotate( FIELDMAP_WORK* fieldmap, int frame, int rot_num );
extern FIELD_TASK* FIELD_TASK_PlayerRotateEX( FIELDMAP_WORK* fieldmap, int frame, int rot_num, MMDL* mmdl );

//------------------------------------------------------------------------------------------
/**
 * @brief 自機の加速回転タスクを作成する
 *
 * @param fieldmap タスクを追加するフィールドマップ
 * @param frame    動作フレーム数
 * @param rot_num  回転回数
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_PlayerRotate_SpeedUp( FIELDMAP_WORK* fieldmap, int frame, int rot_num );

//------------------------------------------------------------------------------------------
/**
 * @brief 自機の減速回転タスクを作成する
 *
 * @param fieldmap タスクを追加するフィールドマップ
 * @param frame    動作フレーム数
 * @param rot_num  回転回数
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_PlayerRotate_SpeedDown( FIELDMAP_WORK* fieldmap, int frame, int rot_num );
extern FIELD_TASK* FIELD_TASK_PlayerRotate_SpeedDownDir( FIELDMAP_WORK* fieldmap, int frame, int rot_num , int endDir );
