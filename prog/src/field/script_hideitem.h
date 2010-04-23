//============================================================================================
/**
 * @file	script_hideitem.h
 * @brief	スクリプト制御：隠しアイテム関連
 * @date	09.09.13
 * @author  tamada
 * 
 * 隠しアイテム関連
 */
//============================================================================================
#pragma once

#include "field/eventwork.h"
//============================================================================================
//============================================================================================
//--------------------------------------------------------------
//	script id --> uniq id
//--------------------------------------------------------------
extern u16 HIDEITEM_GetIDByScriptID( u16 scr_id );

//--------------------------------------------------------------
//	script id --> flag id
//--------------------------------------------------------------
extern u16 HIDEITEM_GetFlagNoByScriptID( u16 scr_id );

//--------------------------------------------------------------
//--------------------------------------------------------------
extern void HIDEITEM_Revival( EVENTWORK * ev );


//--------------------------------------------------------------
// 隠しアイテムテーブルを得る
//--------------------------------------------------------------
extern const void* HIDEITEM_GetTable( u16* num );

