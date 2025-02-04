//======================================================================
/**
 * @file	ds_system.h
 * @brief	DSの設定等の取得ラッパー
 * @author	ariizumi
 * @data	20/01/23
 *
 * モジュール名：DS_SYSTEM
 */
//======================================================================

#pragma once

//--------------------------------------------------------------
//	@berif DSiで起動しているか？
//
//  @return BOOL TRUE DSi
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsRunOnTwl( void );

//--------------------------------------------------------------
//	@berif ペアレンタルコントロールで写真の送受信を拒否しているか？
//
//  @return BOOL TRUE 送受信拒否
//  @return BOOL FALSE 送受信可 or DS
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsRestrictPhotoExchange( void );

//--------------------------------------------------------------
//	@berif ペアレンタルコントロールでユーザー作成コンテンツの
//         送受信を拒否しているか？
//
//  @return BOOL TRUE 送受信拒否
//  @return BOOL FALSE 送受信可 or DS
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsRestrictUGC( void );


//--------------------------------------------------------------
//	@berif DSの無線通信設定の取得
//
//  @return BOOL TRUE 通信許可 or DS
//  @return BOOL FALSE 通信不可
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsAvailableWireless( void );

//--------------------------------------------------------------
//	@berif Wifiの使用規約同意チェック
//         ここでTRUEでもWifi接続時のチェック・バージョン更新などでつなげない時もある。
//
//  @return BOOL TRUE 同意済み
//  @return BOOL FALSE 同意してない
//--------------------------------------------------------------
const BOOL DS_SYSTEM_IsAgreeEULA( void );

//--------------------------------------------------------------
//	@berif DSの誕生日設定の取得
//
//	@param  month 月格納先
//	@param  day   日格納先
//--------------------------------------------------------------
void DS_SYSTEM_GetBirthDay( u8* month, u8* day );
