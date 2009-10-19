//==============================================================================
/**
 * @file	save_tbl.h
 * @brief	WBセーブ
 * @author	matsuda
 * @date	2008.08.27(水)
 */
//==============================================================================
#ifndef __SAVE_H__
#define __SAVE_H__

#include <backup_system.h>


//---------------------------------------------------------------------------
///	セーブに使用しているセクタ数
//---------------------------------------------------------------------------
#define	SAVE_PAGE_MAX		(36)

#define	SAVE_SECTOR_SIZE	(0x1000)

//以下はデバッグツール用のセーブ領域です by soga
#define EDIT_ANM_SAVE_2		( 45 * SAVE_SECTOR_SIZE )	//ポケモンanimeデータ
#define CB_EDIT_SAVE		( 55 * SAVE_SECTOR_SIZE )	//カプセルボールセーブデータ
#define EDIT_ANM_SAVE		( 58 * SAVE_SECTOR_SIZE )	//ポケモンエディットデータ
#define	POKE_ANM_SAVE		(60*SAVE_SECTOR_SIZE)		//ポケモンアニメツール用セーブ領域（未使用のはず）
#define	DEBUG_FIGHT_SAVE	(62*SAVE_SECTOR_SIZE)		//デバッグファイト用セーブ領域


///セーブデータ識別ID
enum{
	GMDATA_ID_BOXDATA,	//ボックスデータグループ
	GMDATA_ID_BOXTRAY_01,	//ボックストレーデータ
	GMDATA_ID_BOXTRAY_02,
	GMDATA_ID_BOXTRAY_03,
	GMDATA_ID_BOXTRAY_04,
	GMDATA_ID_BOXTRAY_05,
	GMDATA_ID_BOXTRAY_06,
	GMDATA_ID_BOXTRAY_07,
	GMDATA_ID_BOXTRAY_08,
	GMDATA_ID_BOXTRAY_09,
	GMDATA_ID_BOXTRAY_10,
	GMDATA_ID_BOXTRAY_11,
	GMDATA_ID_BOXTRAY_12,
	GMDATA_ID_BOXTRAY_13,
	GMDATA_ID_BOXTRAY_14,
	GMDATA_ID_BOXTRAY_15,
	GMDATA_ID_BOXTRAY_16,
	GMDATA_ID_BOXTRAY_17,
	GMDATA_ID_BOXTRAY_18,
	GMDATA_ID_BOXTRAY_19,
	GMDATA_ID_BOXTRAY_20,
	GMDATA_ID_BOXTRAY_21,
	GMDATA_ID_BOXTRAY_22,
	GMDATA_ID_BOXTRAY_23,
	GMDATA_ID_BOXTRAY_24,
	GMDATA_ID_BOXTRAY_PALPARK,  //パルパーク用ボックス
	GMDATA_ID_MYITEM,
	GMDATA_ID_MYPOKE,
	GMDATA_ID_PLAYER_DATA,
	GMDATA_ID_SITUATION,
	GMDATA_ID_WIFIHISTORY,      //Wi-Fi遊んだ履歴
	GMDATA_ID_WIFILIST,         //ともだちコード
	GMDATA_ID_WIFI_NEGOTIATION,  //Wi-Fiネゴシエーション用交換した人のPID
  GMDATA_ID_CGEAR,    //C-GEAR
	GMDATA_ID_TRCARD,		//トレーナーカード(サイン
	GMDATA_ID_MYSTERYDATA,	//不思議な贈り物
	GMDATA_ID_PERAPVOICE,	//ぺラップヴォイス
	GMDATA_ID_SYSTEM_DATA,	//システムデータ
	GMDATA_ID_RECORD,  		//レコード(スコア
	GMDATA_ID_PMS,  		//簡易文
	GMDATA_ID_MMDL,		//動作モデル
	GMDATA_ID_MUSICAL,		//ミュージカル
  GMDATA_ID_RANDOMMAP,  //ランダム生成マップ
	GMDATA_ID_IRCCOMPATIBLE,	//相性チェック
  GMDATA_ID_EVENT_WORK, //イベントワーク
	GMDATA_ID_WORLDTRADEDATA,	//GTS
  GMDATA_ID_REGULATION_DATA, //レギュレーション
  GMDATA_ID_GIMMICK_WORK,    //ギミック
  GMDATA_ID_BATTLE_BOX,    //バトルボックス
  GMDATA_ID_SODATEYA_WORK,  // 育て屋ワーク
  GMDATA_ID_ROCKPOS, //かいりき岩
	GMDATA_ID_MISC,			//未分類
	GMDATA_ID_INTRUDE,  //侵入
  
	GMDATA_ID_MAX,
};

///外部セーブデータ識別ID：戦闘録画：自分
enum{
  EXGMDATA_ID_BATTLE_REC_MINE,
};

///外部セーブデータ識別ID：戦闘録画：ダウンロード0番
enum{
  EXGMDATA_ID_BATTLE_REC_DL_0,
};

///外部セーブデータ識別ID：戦闘録画：ダウンロード1番
enum{
  EXGMDATA_ID_BATTLE_REC_DL_1,
};

///外部セーブデータ識別ID：戦闘録画：ダウンロード2番
enum{
  EXGMDATA_ID_BATTLE_REC_DL_2,
};

///外部セーブデータ識別ID：ストリーミング
enum{
  EXGMDATA_ID_STREAMING,
};


//==============================================================================
//	外部データ宣言
//==============================================================================
extern const GFL_SVLD_PARAM SaveParam_Normal;
extern const GFL_SVLD_PARAM SaveParam_ExtraTbl[];


#endif	//__SAVE_H__

