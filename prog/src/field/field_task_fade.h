/////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  フィールドタスク( 画面フェード )
 * @file   field_task_fade.h
 * @author obata
 * @date   2009.11.20
 */
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
/**
 * @brief フィールドタスクを作成する( 画面フェードリクエスト処理 )
 *
 * @param fieldmap  動作対象フィールドマップ
 * @param mode	    フェードモード（GFL_FADE_MASTER_BRIGHT_BLACKOUT/_WHITEOUT)
 * @param	start_evy	スタート輝度（0〜16）
 * @param	end_evy		エンド輝度（0〜16）
 * @param	wait		  フェードスピード
 *
 * @return 作成したタスク
 */
//-----------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_Fade( FIELDMAP_WORK* fieldmap, 
                                    int mode, int start_evy, int end_evy, int wait );
