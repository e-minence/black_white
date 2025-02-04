//==============================================================================
/**
 * @file  save_tbl.h
 * @brief WBセーブ
 * @author  matsuda
 * @date  2008.08.27(水)
 */
//==============================================================================
#ifndef __SAVE_H__
#define __SAVE_H__

#include <backup_system.h>


//---------------------------------------------------------------------------
/// セーブに使用しているセクタ数
//---------------------------------------------------------------------------
#define SAVE_PAGE_MAX   (36)

#define SAVE_SECTOR_SIZE  (0x1000)

//以下はデバッグツール用のセーブ領域です
#define DEBUG_FIGHT_SAVE  (126*SAVE_SECTOR_SIZE)    //デバッグファイト用セーブ領域
#define DEBUG_SURETIGAI_SAVE  (125*SAVE_SECTOR_SIZE)  //すれ違い通信用セーブ領域

///EXTRA領域で戦闘録画が使用するセーブサイズ
#define SAVESIZE_EXTRA_BATTLE_REC   (SAVE_SECTOR_SIZE * 2)
///EXTRA領域でCGEAR画像が使用するセーブサイズ
#define SAVESIZE_EXTRA_CGEAR_PICTURE   (SAVE_SECTOR_SIZE * 3)
///EXTRA領域でバトル検定が使用するセーブサイズ
#define SAVESIZE_EXTRA_BATTLE_EXAMINATION   (SAVE_SECTOR_SIZE * 1)
///EXTRA領域でストリーミングが使用するセーブサイズ
#define SAVESIZE_EXTRA_STREAMING    (SAVE_SECTOR_SIZE * 32)  //128Kbyte
///EXTRA領域で図鑑壁紙が使用するセーブサイズ
#define SAVESIZE_EXTRA_ZUKAN_WALLPAPER    (0x6800)  //(0x7000)  //CGX6000 + PLTT1000
///EXTRA領域で殿堂入りが使用するセーブサイズ
#define SAVESIZE_EXTRA_DENDOU             (0x1800)

///管理外領域で不思議な贈り物が使用するセーブサイズ
#define SAVESIZE_OUTSIDE_MYSTERY          (0x400)


//--------------------------------------------------------------
//  
//--------------------------------------------------------------
#define SAVE_MAGIC_NUMBER  (0x31053527)
#define SECTOR_MAX    (SAVE_PAGE_MAX)

///ノーマル領域で使用するセーブサイズ
#define SAVESIZE_NORMAL   (SAVE_SECTOR_SIZE * SECTOR_MAX)

///ミラーリング領域で使用するセーブ開始アドレス
#define MIRROR_SAVE_ADDRESS   (0x24000)
///外部セーブが使用するセーブ開始アドレス
#define EXTRA_SAVE_START_ADDRESS    (0x48000)

///外部セーブ：メモリマッピング
enum{
  EXTRA_MM_REC_MINE = EXTRA_SAVE_START_ADDRESS,
  EXTRA_MM_REC_MINE_MIRROR = EXTRA_MM_REC_MINE + SAVESIZE_EXTRA_BATTLE_REC,
  
  EXTRA_MM_REC_DL_0 = EXTRA_MM_REC_MINE_MIRROR + SAVESIZE_EXTRA_BATTLE_REC,
  
  EXTRA_MM_REC_DL_1 = EXTRA_MM_REC_DL_0 + SAVESIZE_EXTRA_BATTLE_REC,
  
  EXTRA_MM_REC_DL_2 = EXTRA_MM_REC_DL_1 + SAVESIZE_EXTRA_BATTLE_REC,
  
  EXTRA_MM_CGEAR = EXTRA_MM_REC_DL_2 + SAVESIZE_EXTRA_BATTLE_REC,

  EXTRA_MM_BATTLE_EXAMINATION = EXTRA_MM_CGEAR + SAVESIZE_EXTRA_CGEAR_PICTURE,

  EXTRA_MM_STREAMING = EXTRA_MM_BATTLE_EXAMINATION + SAVESIZE_EXTRA_BATTLE_EXAMINATION,

  EXTRA_MM_ZUKAN_WALLPAPER = EXTRA_MM_STREAMING + SAVESIZE_EXTRA_STREAMING,
  
  EXTRA_MM_DENDOU = EXTRA_MM_ZUKAN_WALLPAPER + SAVESIZE_EXTRA_ZUKAN_WALLPAPER,
  EXTRA_MM_DENDOU_MIRROR = EXTRA_MM_DENDOU + SAVESIZE_EXTRA_DENDOU,
};

///管理外セーブ：メモリマッピング
enum{
  OUTSIDE_MM_MYSTERY = EXTRA_MM_DENDOU_MIRROR + SAVESIZE_EXTRA_DENDOU,
  OUTSIDE_MM_MYSTERY_MIRROR = OUTSIDE_MM_MYSTERY + SAVESIZE_OUTSIDE_MYSTERY,
};


//--------------------------------------------------------------
//  
//--------------------------------------------------------------
///セーブデータ識別ID
enum{
  GMDATA_ID_BOXDATA,        //ボックスデータグループ
  GMDATA_ID_BOXTRAY_01,     //ボックストレーデータ
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
  GMDATA_ID_MYITEM,           // どうぐ
  GMDATA_ID_MYPOKE,           // てもちポケモン
  GMDATA_ID_PLAYER_DATA,      // プレイヤー関連セーブデータ
  GMDATA_ID_SITUATION,        // フィールド状況データ
  GMDATA_ID_WIFIHISTORY,      // Wi-Fi遊んだ履歴
  GMDATA_ID_WIFILIST,         // ともだちコード
  GMDATA_ID_WIFI_NEGOTIATION, // Wi-Fiネゴシエーション用交換した人のPID
  GMDATA_ID_CGEAR,            // C-GEAR
  GMDATA_ID_TRCARD,           // トレーナーカード(サイン
  GMDATA_ID_MYSTERYDATA,      // 不思議な贈り物
  GMDATA_ID_DREAMWORLD,       // PDW
  GMDATA_ID_PERAPVOICE,       // ぺラップヴォイス
  GMDATA_ID_SYSTEM_DATA,      // システムデータ
  GMDATA_ID_RECORD,           // レコード(スコア
  GMDATA_ID_PMS,              // 簡易文
  GMDATA_ID_MAILDATA,         // メールデータ
  GMDATA_ID_MMDL,             // 動作モデル
  GMDATA_ID_MUSICAL,          // ミュージカル
  GMDATA_ID_RANDOMMAP,        // ランダム生成マップ
  GMDATA_ID_IRCCOMPATIBLE,    // 相性チェック
  GMDATA_ID_EVENT_WORK,       // イベントワーク
  GMDATA_ID_WORLDTRADEDATA,   // GTS
  GMDATA_ID_REGULATION_DATA,  // レギュレーション
  GMDATA_ID_GIMMICK_WORK,     // ギミック
  GMDATA_ID_BATTLE_BOX,       // バトルボックス
  GMDATA_ID_SODATEYA_WORK,    // 育て屋ワーク
  GMDATA_ID_ROCKPOS,          // かいりき岩
  GMDATA_ID_MISC,             // 未分類(小さいデータの集まり）
  GMDATA_ID_INTRUDE,          // 侵入
  GMDATA_ID_SHORTCUT,         // Yボタン登録
  GMDATA_ID_ZUKAN,            // ポケモン図鑑
  GMDATA_ID_ENCOUNT,          // エンカウント関連セーブデータ
  GMDATA_ID_BSUBWAY_PLAYDATA, // バトルサブウェイ　プレイデータ
  GMDATA_ID_BSUBWAY_SCOREDATA,// バトルサブウェイ　スコアデータ
  GMDATA_ID_BSUBWAY_WIFIDATA, // バトルサブウェイ WiFiデータ
  GMDATA_ID_BATTLEMATCH,      // バトル大会
  GMDATA_ID_SYMBOL,           // シンボルエンカウント
  GMDATA_ID_MYPMS,            // 主人公の挨拶文データ
  GMDATA_ID_QUESTIONNAIRE,    // すれ違いアンケート
  GMDATA_ID_UNSV,             // 国連
  GMDATA_ID_THSV,             // トライアルハウスランキング表示用
  GMDATA_ID_GAMECLEAR_POKE,   // ゲームクリアてもちポケモンデータ
  GMDATA_ID_TRPOKE_AFTER,     // 交換ポケモンのその後
  GMDATA_ID_ETC,              // 未分類ワークその２
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


///外部セーブデータ識別ID：CGEARピクチャー
enum{
  EXGMDATA_ID_CGEAR_PICTURE,
};

///外部セーブデータ識別ID：バトル検定
enum{
  EXGMDATA_ID_BATTLE_EXAMINATION,
};

///外部セーブデータ識別ID：ミュージカル配信
enum{
  EXGMDATA_ID_MUSICAL_DIST,
};

///外部セーブデータ識別ID：図鑑壁紙
enum{
  EXGMDATA_ID_ZUKAN_WALLPAPER,
};

///外部セーブデータ識別ID：殿堂入り
enum{
  EXGMDATA_ID_DENDOU,
};



//==============================================================================
//  外部データ宣言
//==============================================================================
extern const GFL_SVLD_PARAM SaveParam_Normal;
extern const GFL_SVLD_PARAM SaveParam_ExtraTbl[];


#endif  //__SAVE_H__

