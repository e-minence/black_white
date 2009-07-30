//======================================================================
/**
 * @file	evwkdef.c
 * @brief	フラグ、ワーク領域定義
 * @author	Satoshi Nohara
 * @date	2005.11.07
 *
 * 2007.06.04	プラチナ用にオフセットを変更
 */
//======================================================================
#ifndef __EVWKDEF_H__
#define __EVWKDEF_H__

//======================================================================
//  フラグ定義関連
//======================================================================
//--------------------------------------------------------------
//  script/saveflag.h
//--------------------------------------------------------------
#define USRFLAG_START		0
#define USRFLAG_MAX			2400 //スクリプト定義のフラグ最大数
#define USRFLAG_AREA_MAX	(USRFLAG_MAX/8)

//--------------------------------------------------------------
//  field/sysflag.h
//--------------------------------------------------------------
#define SYSFLAG_START		(USRFLAG_MAX)
#define	SYSFLAG_MAX			320 //システム定義のフラグ最大数
#define	SYSFLAG_AREA_MAX	(SYSFLAG_MAX/8)

#define TIMEFLAG_START		(USRFLAG_MAX+SYSFLAG_MAX)
#define TIMEFLAG_MAX		192 //時間経過イベント用フラグ最大数
#define	TIMEFLAG_AREA_MAX	(TIMEFLAG_MAX/8)

//--------------------------------------------------------------
//  フラグ最大数
//--------------------------------------------------------------
#define	EVENT_FLAG_MAX		(USRFLAG_MAX+SYSFLAG_MAX+TIMEFLAG_MAX)
#define	EVENT_FLAG_AREA_MAX	(EVENT_FLAG_MAX/8)

//--------------------------------------------------------------
//  フラグオフセット
//--------------------------------------------------------------
#define	SVFLG_START			0x0000 //セーブフラグスタートナンバー
#define	SCFLG_START			0x4000 //制御フラグスタートナンバー

//--------------------------------------------------------------
//  トレーナーフラグ
//--------------------------------------------------------------
#define TR_FLAG_START 1360 //トレーナーID対応フラグ開始位置
#define TR_FLAG_MAX (USRFLAG_MAX-TR_FLAG_START)	//トレーナーID対応フラグ最大数

//wb 0907ROM用トレーナーフラグ
#define TR_FLAG_R01_0907ROM_00 (TR_FLAG_START+0)
#define TR_FLAG_R01_0907ROM_01 (TR_FLAG_START+1)
#define TR_FLAG_R01_0907ROM_02 (TR_FLAG_START+2)
#define TR_FLAG_R01_0907ROM_03 (TR_FLAG_START+3)
#define TR_FLAG_R01_0907ROM_04 (TR_FLAG_START+4)
#define TR_FLAG_R01_0907ROM_05 (TR_FLAG_START+5)

//--------------------------------------------------------------
//  セーブしないフラグ関連定義 制御フラグ
//--------------------------------------------------------------
#define	CTRLFLAG_MAX		64 //制御フラグ最大数
#define	CTRLFLAG_AREA_MAX	(CTRLFLAG_MAX/8)

//--------------------------------------------------------------
//  フラグスタートナンバー
//--------------------------------------------------------------
#define FE_FLAG_START		(100)		//イベント進行制御フラグ(100-369)
#define FV_FLAG_START		(370)		//バニッシュフラグ(369-729)
#define FH_FLAG_START		(730)		//隠しアイテムフラグ(256+28個)

//wb 0907ROM用スクリプトバニッシュフラグ
#define FV_T01R0102_RIVAL   (FV_FLAG_START+0)
#define FV_T01R0102_SUPPORT (FV_FLAG_START+1)
#define FV_T01R0102_BALL00  (FV_FLAG_START+2)
#define FV_T01R0102_BALL01  (FV_FLAG_START+3)
#define FV_T01R0102_BALL02  (FV_FLAG_START+4)
#define FV_T01R0401_HAKASE  (FV_FLAG_START+5)
#define FV_T01R0401_RIVAL   (FV_FLAG_START+6)
#define FV_T01R0401_SUPPORT (FV_FLAG_START+7)
#define FV_R01_HAKASE       (FV_FLAG_START+8)
#define FV_R01_RIVAL        (FV_FLAG_START+9)
#define FV_R01_SUPPORT      (FV_FLAG_START+10)
#define FV_T02PC0101_HAKASE (FV_FLAG_START+11)

//フィールド上のアイテムフラグ(256+72個)
//730+256+28=1014
//1014+256+72=1342
//1360〜トレーナー
#define FB_FLAG_START (FH_FLAG_START+256+28)

#define LOCAL_FLAG_MAX		(64)						//ローカルフラグ数(8bit*8=64)
#define LOCAL_FLAG_AREA_MAX	(LOCAL_FLAG_MAX/8)

//======================================================================
//  ワーク関連定義
//======================================================================
//--------------------------------------------------------------
//  script/savework.h
//--------------------------------------------------------------
#define EVENT_WORK_AREA_MAX	288

#define	SVWK_START			0x4000 //セーブワーク定義スタートナンバー

//ワークオフセット
#define LOCAL_WORK_START	(0+SVWK_START) //ローカルワーク開始
#define LOCAL_WORK_MAX		(32) //ローカルワーク数
#define OBJCHR_WORK_START	(LOCAL_WORK_START+LOCAL_WORK_MAX)	//変更可能なOBJキャラ指定ワーク開始
#define OBJCHR_WORK_MAX		(16)								//変更可能なOBJキャラ指定ワーク数
#define OTHERS_WORK_START	(OBJCHR_WORK_START+OBJCHR_WORK_MAX)	//その他のワーク開始
#define OTHERS_WORK_MAX		(64)								//その他のワーク数
#define SYSWORK_MAX			(LOCAL_WORK_MAX+OBJCHR_WORK_MAX+OTHERS_WORK_MAX)	//

#define	SVSCRWK_START		(SVWK_START+SYSWORK_MAX)			//script/savework.h(スクリプト用)

//wb 0907ROM用スクリプトセーブワーク
#define WK_0907ROM00 (SVSCRWK_START)
#define WK_0907ROM01 (SVSCRWK_START+1)
#define WK_0907ROM02 (SVSCRWK_START+2)
#define WK_0907ROM03 (SVSCRWK_START+3)
#define WK_0907ROM04 (SVSCRWK_START+4)
#define WK_0907ROM05 (SVSCRWK_START+5)
#define WK_0907ROM06 (SVSCRWK_START+6)
#define WK_0907ROM07 (SVSCRWK_START+7)

#define	SCWK_START			0x8000						//データ交換・汎用ワーク定義スタートナンバー

#if 0
//----------------------------------------------------------------
//
//	エメラルド
//
//----------------------------------------------------------------
//フラグ数 = 2400
//ぎりぎりまで使用していた

//トレーナーフラグ数 = 864
//854使用していた

//ワーク数 = 256
//多少余裕ある感じ？だった
#endif

#if 0
//----------------------------------------------------------------
//
//	ダイヤ・パール
//
//----------------------------------------------------------------
トレーナーフラグ
開始	1360
最大数	1040
2400 - 1360 = 1040

ローカルフラグ
0-63

未使用
63-99

イベントフラグ
100-399

バニッシュフラグ
400-729

隠しアイテムフラグ
730-985(255)

フィールド上のアイテム
986-1241

未使用
1242-1359
#endif

#endif //__EVWKDEF_H__
