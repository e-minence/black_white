////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  フィールドタスク(自機の描画オフセット移動)
 * @file   field_task_player_drawoffset.h
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------------------
/**
 * @brief プレイヤーの描画オフセット移動タスクを作成する
 *
 * @param fieldmap タスク動作対象のフィールドマップ
 * @param frame    タスク動作フレーム数
 * @param move     移動ベクトル
 *
 * @return 作成したフィールドタスク
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_TransDrawOffset( FIELDMAP_WORK* fieldmap, int frame, const VecFx32* vec ); 
extern FIELD_TASK* FIELD_TASK_TransDrawOffsetEX( FIELDMAP_WORK* fieldmap, int frame, const VecFx32* vec, MMDL* mmdl ); 
