//////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file   field_place_name.h
 * @brief  地名表示ウィンドウ
 * @author obata_toshihiro
 * @date   2009.07   
 *
 */
//////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "field/field_msgbg.h"

//====================================================================================
// ■関数一覧
//====================================================================================
/*
FIELD_PLACE_NAME* FIELD_PLACE_NAME_Create( HEAPID heap_id, FLDMSGBG* msgbg );
void FIELD_PLACE_NAME_Delete( FIELD_PLACE_NAME* p_sys );
void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* p_sys );
void FIELD_PLACE_NAME_Draw( FIELD_PLACE_NAME* p_sys );
void FIELD_PLACE_NAME_Display( FIELD_PLACE_NAME* p_sys, u32 next_zone_id );
void FIELD_PLACE_NAME_DisplayForce( FIELD_PLACE_NAME* p_sys, u32 zone_id );
void FIELD_PLACE_NAME_Hide( FIELD_PLACE_NAME* p_sys );
*/


//====================================================================================
// ■システムの不完全型
//====================================================================================
typedef struct _FIELD_PLACE_NAME FIELD_PLACE_NAME;


//====================================================================================
// ■作成・破棄
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief 地名表示システムを作成する
 *
 * @param heap_id 使用するヒープID
 * @param msgbg   使用するメッセージ表示システム
 *
 * @return 地名表示システム・ワークへのポインタ
 */
//------------------------------------------------------------------------------------
extern FIELD_PLACE_NAME* FIELD_PLACE_NAME_Create( HEAPID heap_id, FLDMSGBG* msgbg );

//------------------------------------------------------------------------------------
/**
 * @brief 地名表示システムを破棄する
 *
 * @param p_sys 破棄するシステム
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_Delete( FIELD_PLACE_NAME* p_sys );


//====================================================================================
// ■動作
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief 地名表示システムの動作処理
 *
 * @param p_sys 動かすシステム
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* p_sys );

//------------------------------------------------------------------------------------
/**
 * @brief 地名表示ウィンドウの描画処理
 *
 * @param p_sys 描画対象システム
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_Draw( FIELD_PLACE_NAME* p_sys );


//====================================================================================
// ■制御
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief ゾーンの切り替えを通知し, 新しい地名を表示する
 *
 * @param p_sys        ゾーン切り替えを通知するシステム
 * @param next_zone_id 新しいゾーンID
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_Display( FIELD_PLACE_NAME* p_sys, u32 next_zone_id );

//------------------------------------------------------------------------------------
/**
 * @brief 地名ウィンドウを強制的に表示する
 *
 * @param p_sys   表示するシステム
 * @param zone_id 表示する場所のゾーンID
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_DisplayForce( FIELD_PLACE_NAME* p_sys, u32 zone_id );

//------------------------------------------------------------------------------------
/**
 * @brief 地名ウィンドウの表示を強制的に終了する
 *
 * @param p_sys 表示を終了するシステム
 */
//------------------------------------------------------------------------------------
extern void FIELD_PLACE_NAME_Hide( FIELD_PLACE_NAME* p_sys );

extern void FIELD_PLACE_NAME_RecoverBG(FIELD_PLACE_NAME* sys);
