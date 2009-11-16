////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief ISSゾーンシステム
 * @file iss_zone_sys.h
 * @author obata
 * @date 2009.11.06
 */
////////////////////////////////////////////////////////////////////////////////////////////


//==========================================================================================
// ■システムの不完全型宣言
//==========================================================================================
typedef struct _ISS_ZONE_SYS ISS_ZONE_SYS;


//==========================================================================================
// ■作成・破棄
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief ISSゾーンシステムを作成する
 * 
 * @param heap_id 使用するヒープID
 *
 * @return 作成したシステム
 */
//------------------------------------------------------------------------------------------
ISS_ZONE_SYS* ISS_ZONE_SYS_Create( HEAPID heap_id );

//------------------------------------------------------------------------------------------
/**
 * @brief ISSゾーンシステムを破棄する
 *
 * @param sys 破棄するシステム
 */
//------------------------------------------------------------------------------------------
void ISS_ZONE_SYS_Delete( ISS_ZONE_SYS* sys );


//==========================================================================================
// ■動作
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief ゾーン切り替えを通知する
 *
 * @param sys          通知対象のシステム
 * @param next_zone_id 新しいゾーンID
 */
//------------------------------------------------------------------------------------------
void ISS_ZONE_SYS_ZoneChange( ISS_ZONE_SYS* sys, u16 next_zone_id );
