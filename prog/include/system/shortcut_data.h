//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		shortcut_data.h
 *	@brief	Yボタン登録
 *	@author	Toru=Nagihashi
 *	@date		2009.10.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	Yボタン登録箇所
//=====================================
typedef enum
{
	SHORTCUT_ID_ZITENSYA,			//自転車
	SHORTCUT_ID_TOWNMAP,			//タウンマップ
	SHORTCUT_ID_PALACEGO,			//パレスへゴー
	SHORTCUT_ID_BTLRECORDER,	//バトルレコーダー
	SHORTCUT_ID_FRIENDNOTE,		//友達手賞
	SHORTCUT_ID_TURIZAO,			//つりざお

	SHORTCUT_ID_PSTATUS_STATUS,		//ポケモン情報＞ステータス
	SHORTCUT_ID_PSTATUS_ABILITY,	//ポケモン情報＞のうりょく
	SHORTCUT_ID_PSTATUS_RIBBON,		//ポケモン情報＞きねんリボン
	SHORTCUT_ID_BAG_ITEM,					//バッグ＞どうぐ
	SHORTCUT_ID_BAG_RECOVERY,			//バッグ＞かいふく
	SHORTCUT_ID_BAG_WAZAMACHINE,	//バッグ＞技マシン
	SHORTCUT_ID_BAG_NUTS,					//バッグ＞きのみ
	SHORTCUT_ID_BAG_IMPORTANT,		//バッグ＞たいせつなもの
	SHORTCUT_ID_ZUKAN_MENU,				//ずかん＞メニュー
	SHORTCUT_ID_ZUKAN_BUNPU,			//ずかん＞分布
	SHORTCUT_ID_ZUKAN_CRY,				//ずかん＞なぎごえ
	SHORTCUT_ID_TRCARD_FRONT,			//トレーナーカード＞ぜんめん
	SHORTCUT_ID_TRCARD_BACK,			//トレーナーカード＞うらめん
	SHORTCUT_ID_CONFIG,						//せってい


	SHORTCUT_ID_MAX,
	SHORTCUT_ID_NULL	= SHORTCUT_ID_MAX,	//データなし
} SHORTCUT_ID;

//=============================================================================
/**
 *					外部公開
*/
//=============================================================================
extern SHORTCUT_ID SHORTCUT_DATA_GetItemToShortcutID( u16 item );
