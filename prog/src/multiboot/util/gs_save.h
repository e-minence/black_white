
#define CRC_LOADCHECK (1)
//---------------------------------------------------------------------------
/// セーブに使用しているセクタ数
//---------------------------------------------------------------------------
#define GS_SAVE_PAGE_MAX   (35)

#define GS_SAVE_SECTOR_SIZE  (0x1000)
//---------------------------------------------------------------------------
/// セーブ項目の定義
//---------------------------------------------------------------------------
typedef enum {
  //ノーマルデータグループ
  GS_GMDATA_ID_SYSTEM_DATA,
  GS_GMDATA_ID_PLAYER_DATA,
  GS_GMDATA_ID_TEMOTI_POKE,
  GS_GMDATA_ID_TEMOTI_ITEM,
  GS_GMDATA_ID_EVENT_WORK,
  GS_GMDATA_ID_SITUATION,
  GS_GMDATA_ID_ZUKANWORK,
  GS_GMDATA_ID_SODATEYA,
  GS_GMDATA_ID_FRIEND,
  GS_GMDATA_ID_MISC,
  GS_GMDATA_ID_FIELDOBJSV,
  GS_GMDATA_ID_REGULATION,
  GS_GMDATA_ID_IMAGECLIPDATA,
  GS_GMDATA_ID_MAILDATA,
  GS_GMDATA_ID_RANDOMGROUP,
  GS_GMDATA_ID_TRCARD,
  GS_GMDATA_ID_RECORD,
  GS_GMDATA_ID_CUSTOM_BALL,
  GS_GMDATA_ID_PERAPVOICE,
  GS_GMDATA_ID_FRONTIER,
  GS_GMDATA_ID_SP_RIBBON,
  GS_GMDATA_ID_ENCOUNT,
  GS_GMDATA_ID_WORLDTRADEDATA,
  GS_GMDATA_ID_TVWORK,
  GS_GMDATA_ID_GUINNESS,
  GS_GMDATA_ID_WIFILIST,
  GS_GMDATA_ID_WIFIHISTORY,
  GS_GMDATA_ID_FUSHIGIDATA,
  GS_GMDATA_ID_POKEPARKDATA,
  GS_GMDATA_ID_PKTH_WPOKE_SAVE,
  GS_GMDATA_ID_PMS,
  GS_GMDATA_ID_EMAIL,
  GS_GMDATA_ID_WFHIROBA,
  GS_GMDATA_ID_PAIR_POKE,
  GS_GMDATA_ID_PGEAR_SAVE,
  GS_GMDATA_ID_SAFARI_SAVE,
  GS_GMDATA_ID_PICTURE_SAVE,
  GS_GMDATA_ID_POKETHLON_SAVE,
  GS_GMDATA_ID_BONGURI_SAVE,
  GS_GMDATA_ID_PHC_SAVE,
  GS_GMDATA_ID_TR_HOUSE_SAVE,
  GS_GMDATA_NORMAL_FOOTER,  //WBで追加。セーブのCRCとかカウンタとか入ってる
  GS_GMDATA_PADDING,        //WBで追加。ボックスの前に256アライメントやっているので
  
  //ボックスデータグループ
  GS_GMDATA_ID_BOXDATA,
  GS_GMDATA_BOX_FOOTER,     //WBで追加。ボックスのCRCとかカウンタとか入ってる

  GS_GMDATA_ID_MAX,
}GS_GMDATA_ID;
//=============================================================================
//=============================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define MAGIC_NUMBER_GS (0x20060623)
#define GS_SECTOR_SIZE   (GS_SAVE_SECTOR_SIZE)
#define GS_SECTOR_MAX    (GS_SAVE_PAGE_MAX)

#define FIRST_MIRROR_START  (0)
#define SECOND_MIRROR_START (64)

#define MIRROR1ST (0)
#define MIRROR2ND (1)
#define MIRRORERROR (2)

#define HEAPID_SAVE_TEMP  (HEAPID_BASE_APP)

#define GS_SEC_DATA_SIZE   GS_SECTOR_SIZE

#define GS_SAVE_SIZE (GS_SECTOR_SIZE * GS_SECTOR_MAX)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct {
  u32 g_count;    ///<グローバルカウンタ（MYデータ、BOXデータ共有）
  u32 size;     ///<データサイズ（フッタサイズ含む）
  u32 magic_number; ///<マジックナンバー
  u8 blk_id;      ///<対象のブロック指定ID
  u16 crc;      ///<データ全体のCRC値
}GS_SAVE_FOOTER;

//---------------------------------------------------------------------------
/**
 * @brief セーブワーク構造体
 *
 * 実際のセーブされる部分の構造
 */
//---------------------------------------------------------------------------
/*
typedef struct {
  u8 data[GS_SECTOR_SIZE * GS_SECTOR_MAX];  ///<実際のデータ保持領域
}SAVEWORK;
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define GS_BOX_MAX_TRAY      (18)
#define GS_BOX_MAX_POS       (5*6)
#define GS_BOX_TRAYNAME_BUFSIZE  (20)  // 日本語８文字＋EOM。海外版用の余裕も見てこの程度。
#define GS_BOX_TRAY_ALL_USE_BIT	(0b111111111111111111)

typedef struct
{
  u32 dummy[0x88/4];
}GS_POKEMON_PARAM;


//フラッシュのページをまたがないように1トレー分のデータを256バイトアライメントする
typedef struct
{
  GS_POKEMON_PARAM  ppp[GS_BOX_MAX_POS];
  u8 dummy[16];
}GS_BOX_TRAY_DATA;

typedef struct {
/// POKEMON_PASO_PARAM  ppp[BOX_MAX_TRAY][BOX_MAX_POS];
  GS_BOX_TRAY_DATA   btd[GS_BOX_MAX_TRAY];
  //ここより下のデータが２５６バイトアライメントされたところにマッピングされる
  u32         currentTrayNumber;                //4
  u32         UseBoxBits;                   //4
  STRCODE     trayName[GS_BOX_MAX_TRAY][GS_BOX_TRAYNAME_BUFSIZE]; //2*20*18 = 720
  u8          wallPaper[GS_BOX_MAX_TRAY];            //18
  u8          daisukiBitFlag;                 //1
  u8          dummy[17];    //アライメントダミー      //17
  
  //ポケモンデータが0x1000*18 バイト = 0x12000
  //ポケモンデータ以外で764バイト = 0x2fc
  //計0x122fcバイト　
  //これにＣＲＣ４バイトを追加すると、0x12300となり、256バイトアライメントされ、
  //フッター情報のみの書き込みの際に、フラッシュページをまたがなくなる
}GS_BOX_DATA;


//---------------------------------------------------------------------------
//  アイテム関係
//---------------------------------------------------------------------------
typedef struct
{
  u16 id;   // アイテム番号
  u16 no;   // 個数
}GS_MINEITEM;

#define GS_BAG_NORMAL_ITEM_MAX    ( 165 )   // 道具ポケット最大数
#define GS_BAG_EVENT_ITEM_MAX     ( 50 )    // 大切な物ポケット最大数
#define GS_BAG_WAZA_ITEM_MAX      ( 101 )   // 技マシンポケット最大数
#define GS_BAG_SEAL_ITEM_MAX      ( 12 )    // シールポケット最大数
#define GS_BAG_DRUG_ITEM_MAX      ( 40 )    // 薬ポケット最大数
#define GS_BAG_NUTS_ITEM_MAX      ( 64 )    // 木の実ポケット最大数
#define GS_BAG_BALL_ITEM_MAX      ( 24 )    // モンスターボールポケット最大数
#define GS_BAG_BATTLE_ITEM_MAX    ( 30 )    // 戦闘用アイテムポケット最大数

typedef struct {
  GS_MINEITEM MyNormalItem[ GS_BAG_NORMAL_ITEM_MAX ]; // 手持ちの普通の道具
  GS_MINEITEM MyEventItem[ GS_BAG_EVENT_ITEM_MAX ];   // 手持ちの大切な道具
  GS_MINEITEM MySkillItem[ GS_BAG_WAZA_ITEM_MAX ];    // 手持ちの技マシン
  GS_MINEITEM MySealItem[ GS_BAG_SEAL_ITEM_MAX ];   // 手持ちのシール(実際はメール
  GS_MINEITEM MyDrugItem[ GS_BAG_DRUG_ITEM_MAX ];   // 手持ちの薬
  GS_MINEITEM MyNutsItem[ GS_BAG_NUTS_ITEM_MAX ];   // 手持ちの木の実
  GS_MINEITEM MyBallItem[ GS_BAG_BALL_ITEM_MAX ];   // 手持ちのモンスターボール
  GS_MINEITEM MyBattleItem[ GS_BAG_BATTLE_ITEM_MAX ]; // 手持ちの戦闘用アイテム
  u16 cnv_button1;                // 便利ボタン1(Yボタン）
  u16 cnv_button2;                // 便利ボタン2
}GS_MYITEM;

//------------------------------------------------------------------
//  MYSTATUSの定義  国コード参照
//------------------------------------------------------------------
typedef struct
{
  //CONFIG本当は別構造体だけど・・・
	u16 msg_speed:4;			///<MSGSPEED		ＭＳＧ送りの速度
	u16 sound_mode:2;			///<SOUNDMODE		サウンド出力
	u16 battle_rule:1;			///<BATTLERULE		戦闘ルール
	u16 wazaeff_mode:1;			///<WAZAEFF_MODE	わざエフェクト
	u16 input_mode:2;			///<INPUTMODE		入力モード
	u16 window_type:5;			///<WINTYPE			ウィンドウタイプ
	u16 dummy:1;
  //CONFIG本当は別構造体だけど・・・(ここまで

  STRCODE name[8];    // 16
  u32 id;                     // 20
  u32 gold;                   // 24

  u8 sex; 
  u8 region_code;                 //26

  u8 badge_johto;                   
  u8 trainer_view;  // ユニオンルーム内での見た目フラグ //28
  u8 rom_code;    // 0:ダイヤ  1:パール   // 29

  u8 dp_clear:1;    // DPクリアしているかフラグ
  u8 dp_zenkoku:1;  // DP全国図鑑手に入れているかフラグ
  u8 dummy1:6;                  // 30
  u8 dummy2;                    // 31
  u8 badge_kanto;   //金銀用にキープ　2007.08.09(木) matsuda  // 32
}GS_MYSTATUS;

//------------------------------------------------------------------
//  ふしぎデータの定義
//------------------------------------------------------------------
#define GS_MYSTERYGIFT_TYPE_NONE		0	// 何も無い
#define GS_MYSTERYGIFT_TYPE_POKEMON	1	// ポケモン
#define GS_MYSTERYGIFT_TYPE_POKEEGG	2	// タマゴ
#define GS_MYSTERYGIFT_TYPE_ITEM		3	// どうぐ
#define GS_MYSTERYGIFT_TYPE_RULE		4	// ルール
#define GS_MYSTERYGIFT_TYPE_GOODS		5	// グッズ
#define GS_MYSTERYGIFT_TYPE_ACCESSORY	6	// アクセサリ
#define GS_MYSTERYGIFT_TYPE_RANGEREGG	7	// マナフィーのタマゴ
#define GS_MYSTERYGIFT_TYPE_MEMBERSCARD	8	// メンバーズカード
#define GS_MYSTERYGIFT_TYPE_LETTER		9	// オーキドのてがみ
#define GS_MYSTERYGIFT_TYPE_WHISTLE	10	// てんかいのふえ
#define GS_MYSTERYGIFT_TYPE_POKETCH	11	// ポケッチ
#define GS_MYSTERYGIFT_TYPE_SECRET_KEY	12	// 秘密の鍵
#define GS_MYSTERYGIFT_TYPE_MOVIE		13	// 映画配布
#define GS_MYSTERYGIFT_TYPE_PHC		14	// PHCコース
#define GS_MYSTERYGIFT_TYPE_PHOTO		15	// 写真
#define GS_MYSTERYGIFT_TYPE_MAX	16	// 
#define GS_MYSTERYGIFT_TYPE_CLEAR		255	// ふしぎ領域の強制クリア

#define GS_GIFT_DELIVERY_MAX	8	// 配達員８つ
#define GS_GIFT_CARD_MAX		3	// カードデータ３つ
#define GS_GIFT_DATA_CARD_TITLE_MAX	36
#define GS_GIFT_DATA_CARD_TEXT_MAX		250
#define GS_MYSTERYGIFT_POKEICON	3
// サイズ固定用構造体
typedef struct
{
  u8 data[256];
}GS_GIFT_PRESENT_ALL;

// どうぐ
typedef struct
{
  int itemNo;
  int movieflag;
}GS_GIFT_PRESENT_ITEM;

typedef union 
{
  GS_GIFT_PRESENT_ALL    all;
//  GIFT_PRESENT_POKEMON    pokemon;
//  GIFT_PRESENT_POKEEGG    egg;
  GS_GIFT_PRESENT_ITEM   item;
//  GIFT_PRESENT_GOODS    goods;
//  GIFT_PRESENT_RULE   rule;
//  GIFT_PRESENT_ACCESSORY  accessory;
//  GIFT_PRESENT_RANGEREGG  rangeregg;
//  GIFT_PRESENT_MEMBERSCARD  memberscard;
//  GIFT_PRESENT_LETTER   letter;
//  GIFT_PRESENT_WHISTLE    whistle;
//  GIFT_PRESENT_POKETCH    poketch;
//  GIFT_PRESENT_PHCMAP   phcmap;
//  GIFT_PRESENT_PICTURE    picture;
//  GIFT_PRESENT_REMOVE   remove;
}GS_GIFT_PRESENT;

// ふしぎなおくりもの　ビーコンデータ
typedef struct
{
  STRCODE event_name[GS_GIFT_DATA_CARD_TITLE_MAX]; // イベントタイトル
  u32 version;            // 対象バージョン(０の場合は制限無しで配布)
  u16 event_id;           // イベントＩＤ(最大2048件まで)
  u8 only_one_flag: 1;    // １度だけ受信フラグ(0..何度でも受信可能 1..１回のみ)
  u8 access_point: 1;     // アクセスポイント(もしかして必要なくなった？)
  u8 have_card: 1;        // カード情報を含んでいるか(0..含んでいない  1..含んでる)
  u8 delivery_flag: 1;    // 配達員から受け取るものを含んでいるか
  u8 re_deal_flag: 1;     // 孫配布する事が可能か？(0..出来ない 1..出来る)
  u8 groundchild_flag: 1; // 孫配布フラグ(0..違う 1..孫配布)
  u8 dummy: 2;
}GS_GIFT_BEACON;

// 配達員(最大８つ)
typedef struct
{
  u16 gift_type;
  u16 link : 2;         // カードへのリンク(0: 2:リンク 3リンクなし)
  u16 dummy : 14;
  GS_GIFT_PRESENT data;
}GS_GIFT_DELIVERY;


// カード情報(最大３つ)
typedef struct 
{
  u16 gift_type;
  u16 dummy;            // 配達員へのリンク
  GS_GIFT_PRESENT data;

  GS_GIFT_BEACON beacon;   // ビーコン情報と同等の情報を持つ

  STRCODE event_text[GS_GIFT_DATA_CARD_TEXT_MAX];  // 説明テキスト
  u8 re_deal_count;     // 再配布の回数(0〜254、255は無制限)
  u16 pokemon_icon[GS_MYSTERYGIFT_POKEICON]; // ポケモンアイコン３つ分

  // ↑配布するのはここまで
  // ↓この下はフラッシュにセーブする時のみ必要なデータ
  
  u8 re_dealed_count;       // 配布した回数
  s32 recv_date;        // 受信した時間
  
} GS_GIFT_CARD;

#define GS_FUSHIGI_DATA_MAX_EVENT		2048
typedef struct 
{
  u8 recv_flag[GS_FUSHIGI_DATA_MAX_EVENT / 8];   //256 * 8 = 2048 bit
  GS_GIFT_DELIVERY delivery[GS_GIFT_DELIVERY_MAX];    // 配達員８つ
  GS_GIFT_CARD card[GS_GIFT_CARD_MAX];      // カード情報３つ
  GS_GIFT_CARD rockcapcard;      // ロックカプセル用１つ
}GS_FUSHIGI_DATA;
