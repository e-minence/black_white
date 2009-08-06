#pragma once

//////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  カレンダー天気データ
 * @author obata
 * @date   2009.08.05
 *
 */
//////////////////////////////////////////////////////////////////////////////////////////

#include <gflib.h>


// 不完全型の宣言
typedef struct _CALENDER_WEATHER_INFO CALENDER_WEATHER_INFO;


//----------------------------------------------------------------------------------------
/**
 * @brief カレンダー天気データを作成する
 *
 * @param heap_id 使用するヒープID
 *
 * @return 作成したカレンダー天気データ
 */
//----------------------------------------------------------------------------------------
CALENDER_WEATHER_INFO* CALENDER_WEATHER_INFO_Create( HEAPID heap_id );

//----------------------------------------------------------------------------------------
/**
 * @brief 指定日・指定ゾーンの天気を取得する
 *
 * @param p_info  カレンダー天気データ
 * @param p_date  日付
 * @param zone_id ゾーンID
 *
 * @return 指定日の指定ゾーンの天気
 */
//----------------------------------------------------------------------------------------
u16 CALENDER_WEATHER_INFO_GetWeather( CALENDER_WEATHER_INFO* p_info, const RTCDate* p_date, u16 zone_id );

//----------------------------------------------------------------------------------------
/**
 * @brief カレンダー天気データを破棄する
 *
 * @param p_info 破棄するデータ
 */ 
//----------------------------------------------------------------------------------------
void CALENDER_WEATHER_INFO_Delete( CALENDER_WEATHER_INFO* p_info );
