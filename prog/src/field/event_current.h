///////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  水流による強制移動イベント
 * @file   event_current.h
 * @author obata
 * @date   2009.11.27
 */
///////////////////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------------------------
/**
 * @brief 水流移動イベントを作成する
 *
 * @param parent   親となるイベント
 * @param gsys     ゲームシステム
 * @param fieldmap フィールドマップ
 *
 * @return 移動床イベント
 */
//---------------------------------------------------------------------------------------------
extern GMEVENT* EVENT_PlayerMoveOnCurrent( GMEVENT* parent, 
                                           GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );

//---------------------------------------------------------------------------------------------
/**
 * @brief 水流アトリビュート判定
 *
 * @param attrval アトリビュート
 *
 * @return 指定したアトリビュートが水流なら TRUE, そうでないなら FALSE
 */
//---------------------------------------------------------------------------------------------
extern BOOL CheckAttributeIsCurrent( MAPATTR_VALUE attrval );
