//============================================================================================
/**
 * @file  item.h
 * @brief アイテムデータ処理
 * @author  Hiroyuki Nakamura
 * @date  05.09.06
 */
//============================================================================================

#pragma once

#include <gflib.h>
#include "item/itemsym.h"
#include "item/itemequip.h"


//============================================================================================
//  定数定義
//============================================================================================
typedef struct _ITEM_DATA ITEMDATA;   // アイテムデータ

// 特殊アイテムID（アイコン取得などに使用）
#define ITEM_DUMMY_ID ( 0 )       // ダミーID
#define ITEM_RETURN_ID  ( 0xffff )      // 戻る


// メールアイテムのスタートナンバー
#define MAIL_START_ITEMNUM  (ITEM_GURASUMEERU)
#define MAIL_END_ITEMNUM  (MAIL_START_ITEMNUM+ITEM_MAIL_MAX-1)

//木の実・こやしアイテムのスタートナンバー他
#define NUTS_START_ITEMNUM  ( ITEM_KURABONOMI )   // アイテム番号の最初の木の実
#define NUTS_END_ITEMNUM  ( NUTS_START_ITEMNUM+ITEM_NUTS_MAX-1 )  //アイテム番号の最後の木の実
#define COMPOST_START_ITEMNUM (ITEM_SUKUSUKUKOYASI)
#define ITEM_COMPOST_MAX    (ITEM_NEBANEBAKOYASI-ITEM_SUKUSUKUKOYASI+1)
#define ITEMNO_TO_KINOMINO(no)  ((no)-NUTS_START_ITEMNUM)
#define KINOMINO_TO_ITEMNO(no)  ((no)+NUTS_START_ITEMNUM)



// インデックス取得定義
enum {
  ITEM_GET_DATA = 0,    // アイテムデータ
  ITEM_GET_ICON_CGX,    // アイコンキャラ
  ITEM_GET_ICON_PAL,    // アイコンパレット
  ITEM_GET_AGB_NUM    // AGBのアイテム番号
};

// パラメータ取得定義
enum {
  ITEM_PRM_PRICE,         // 買値
  ITEM_PRM_EQUIP,         // 装備効果
  ITEM_PRM_ATTACK,        // 威力
  ITEM_PRM_EVENT,         // 重要
  ITEM_PRM_CNV,         // 便利ボタン
  ITEM_PRM_POCKET,        // 保存先（ポケット番号）
  ITEM_PRM_FIELD,         // field機能
  ITEM_PRM_BATTLE,        // battle機能
  ITEM_PRM_TUIBAMU_EFF,     // ついばむ効果
  ITEM_PRM_NAGETUKERU_EFF,    // なげつける効果
  ITEM_PRM_NAGETUKERU_ATC,    // なげつける威力
  ITEM_PRM_SIZENNOMEGUMI_ATC,   // しぜんのめぐみ威力
  ITEM_PRM_SIZENNOMEGUMI_TYPE,  // しぜんのめぐみタイプ
  ITEM_PRM_BTL_POCKET,      // 戦闘保存先（ポケット番号）
  ITEM_PRM_W_TYPE,        // ワークタイプ
	ITEM_PRM_ITEM_TYPE,			// アイテム種類
	ITEM_PRM_ITEM_SPEND,		// 消費するか

  ITEM_PRM_SLEEP_RCV,       // 眠り回復
  ITEM_PRM_POISON_RCV,      // 毒回復
  ITEM_PRM_BURN_RCV,        // 火傷回復
  ITEM_PRM_ICE_RCV,       // 氷回復
  ITEM_PRM_PARALYZE_RCV,      // 麻痺回復
  ITEM_PRM_PANIC_RCV,       // 混乱回復
  ITEM_PRM_MEROMERO_RCV,      // メロメロ回復
  ITEM_PRM_ABILITY_GUARD,     // 能力ガード
  ITEM_PRM_DEATH_RCV,       // 瀕死回復
  ITEM_PRM_ALL_DEATH_RCV,     // 全員瀕死回復
  ITEM_PRM_LV_UP,         // レベルアップ
  ITEM_PRM_EVOLUTION,       // 進化
  ITEM_PRM_ATTACK_UP,       // 攻撃力アップ
  ITEM_PRM_DEFENCE_UP,      // 防御力アップ
  ITEM_PRM_SP_ATTACK_UP,      // 特攻アップ
  ITEM_PRM_SP_DEFENCE_UP,     // 特防アップ
  ITEM_PRM_AGILITY_UP,      // 素早さアップ
  ITEM_PRM_HIT_UP,        // 命中率アップ
  ITEM_PRM_CRITICAL_UP,     // クリティカル率アップ
  ITEM_PRM_PP_UP,         // PPアップ
  ITEM_PRM_PP_3UP,        // PPアップ（３段階）
  ITEM_PRM_PP_RCV,        // PP回復
  ITEM_PRM_ALL_PP_RCV,      // PP回復（全ての技）
  ITEM_PRM_HP_RCV,        // HP回復
  ITEM_PRM_HP_EXP,        // HP努力値アップ
  ITEM_PRM_POWER_EXP,       // 攻撃努力値アップ
  ITEM_PRM_DEFENCE_EXP,     // 防御努力値アップ
  ITEM_PRM_AGILITY_EXP,     // 素早さ努力値アップ
  ITEM_PRM_SP_ATTACK_EXP,     // 特攻努力値アップ
  ITEM_PRM_SP_DEFENCE_EXP,    // 特防努力値アップ
  ITEM_PRM_FRIEND1,       // なつき度１
  ITEM_PRM_FRIEND2,       // なつき度２
  ITEM_PRM_FRIEND3,       // なつき度３
  ITEM_PRM_HP_EXP_POINT,      // HP努力値の値
  ITEM_PRM_POWER_EXP_POINT,   // 攻撃努力値の値
  ITEM_PRM_DEFENCE_EXP_POINT,   // 防御努力値の値
  ITEM_PRM_AGILITY_EXP_POINT,   // 素早さ努力値の値
  ITEM_PRM_SP_ATTACK_EXP_POINT, // 特攻努力値の値
  ITEM_PRM_SP_DEFENCE_EXP_POINT,  // 特防努力値の値
  ITEM_PRM_HP_RCV_POINT,      // HP回復値の値
  ITEM_PRM_PP_RCV_POINT,      // pp回復値の値
  ITEM_PRM_FRIEND1_POINT,     // なつき度1の値
  ITEM_PRM_FRIEND2_POINT,     // なつき度2の値
  ITEM_PRM_FRIEND3_POINT,     // なつき度3の値
};

// ワークタイプの種類
enum {
  ITEM_WKTYPE_DUMMY = 0,    // ダミー
  ITEM_WKTYPE_POKEUSE,    // ポケモンに使う
};


///ボールID定義
typedef enum{
 BALLID_NULL,     // NULL
 BALLID_MASUTAABOORU, //01 マスターボール
 BALLID_HAIPAABOORU,  //02 ハイパーボール
 BALLID_SUUPAABOORU,  //03 スーパーボール
 BALLID_MONSUTAABOORU,  //04 モンスターボール
 BALLID_SAFARIBOORU,  //05 サファリボール
 BALLID_NETTOBOORU,   //06 ネットボール
 BALLID_DAIBUBOORU,   //07 ダイブボール
 BALLID_NESUTOBOORU,    //08 ネストボール
 BALLID_RIPIITOBOORU, //09 リピートボール
 BALLID_TAIMAABOORU,    //10 タイマーボール
 BALLID_GOOZYASUBOORU,  //11 ゴージャスボール
 BALLID_PUREMIABOORU, //12 プレミアボール
 BALLID_DAAKUBOORU,   //13 ダークボール
 BALLID_HIIRUBOORU,   //14 ヒールボール
 BALLID_KUIKKUBOORU,    //15 クイックボール
 BALLID_PURESYASUBOORU, //16 プレシャスボール

 BALLID_SUPIIDOBOORU, //17 スピードボール
 BALLID_REBERUBOORU,    //18 レベルボール
 BALLID_RUAABOORU,    //19 ルアーボール
 BALLID_HEBIIBOORU,   //20 ヘビーボール
 BALLID_RABURABUBOORU,  //21 ラブラブボール
 BALLID_HURENDOBOORU, //22 フレンドボール
 BALLID_MUUNBOORU,    //23 ムーンボール
 BALLID_KONPEBOORU,   //24 コンペボール
 BALLID_MAX = BALLID_KONPEBOORU,  //パークボールは実体のないボールなのでIDに含めません
}BALL_ID;


// HP 回復量定義 (下記以外なら、その値分だけ回復する）
enum {
  ITEM_RECOVER_HP_FULL = 255,   ///< 全回復
  ITEM_RECOVER_HP_HALF = 254,   ///< MAXの半分回復
  ITEM_RECOVER_HP_QUOT = 253,   ///< MAXの1/4回復
};

// PP 回復量定義 (下記以外なら、その値分だけ回復する）
enum {
  ITEM_RECOVER_PP_FULL = 127,   ///< 全回復
};

//============================================================================================
//  プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 指定アイテムを指定位置に挿入
 *
 * @param item  アイテムデータ
 * @param pos1  指定アイテム位置
 * @param pos2  挿入位置
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void ItemPutIn( void * item, u16 pos1, u16 pos2 );

//============================================================================================
//============================================================================================
//  データ取得
//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * データインデックス取得
 *
 * @param item    アイテム番号
 * @param type    取得データ
 *
 * @return  指定データ
 *
 * @li  type = ITEM_GET_DATA    : アイテムデータのアーカイブインデックス
 * @li  type = ITEM_GET_ICON_CGX  : アイコンキャラのアーカイブインデックス
 * @li  type = ITEM_GET_ICON_PAL  : アイコンパレットのアーカイブインデックス
 * @li  type = ITEM_GET_AGB_NUM   : AGBのアイテム番号
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetIndex( u16 item, u16 type );

//--------------------------------------------------------------------------------------------
/**
 * AGBのアイテムをDPのアイテムに変換
 *
 * @param agb   AGBのアイテム番号
 *
 * @retval  "ITEM_DUMMY_ID = DPにないアイテム"
 * @retval  "ITEM_DUMMY_ID != DPのアイテム番号"
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_CnvAgbItem( u16 agb );

//--------------------------------------------------------------
/**
 * @brief   アイテムアイコンのアーカイブID取得
 *
 * @retval  アイテムアイコンのアーカイブID
 */
//--------------------------------------------------------------
extern u16 ITEM_GetIconArcID(void);

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコンのセルアーカイブインデックス取得
 *
 * @param none
 *
 * @return  セルアーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetIconCell(void);

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコンのセルアニメアーカイブインデックス取得
 *
 * @param none
 *
 * @return  セルアニメアーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetIconCellAnm(void);

//--------------------------------------------------------------------------------------------
/**
 * アーカイブデータロード
 *
 * @param item    アイテム番号
 * @param type    取得データ
 * @param heap_id   ヒープID
 *
 * @return  指定データ
 *
 * @li  type = ITEM_GET_DATA    : アイテムデータ
 * @li  type = ITEM_GET_ICON_CGX  : アイコンのキャラデータ
 * @li  type = ITEM_GET_ICON_PAL  : アイコンのパレットデータ
 */
//--------------------------------------------------------------------------------------------
extern void * ITEM_GetItemArcData( u16 item, u16 type, HEAPID heap_id );

//--------------------------------------------------------------------------------------------
/**
 * 名前取得
 *
 * @param buf     格納場所
 * @param item    アイテム番号
 * @param heap_id   ヒープID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void ITEM_GetItemName( STRBUF* buf, u16 item, HEAPID heap_id );

//--------------------------------------------------------------------------------------------
/**
 * 説明取得
 *
 * @param   buf     アイテム名格納先バッファ
 * @param item    アイテム番号
 * @param heap_id   ヒープID
 *
 * @return  説明
 */
//--------------------------------------------------------------------------------------------
extern void ITEM_GetInfo( STRBUF * buf, u16 item, HEAPID heap_id );

//--------------------------------------------------------------------------------------------
/**
 * パラメータ取得（アイテム番号指定）
 *
 * @param item  アイテム番号
 * @param param 取得パラメータ定義
 * @param heap_id   ヒープID
 *
 * @return  指定パラメータ
 */
//--------------------------------------------------------------------------------------------
extern s32 ITEM_GetParam( u16 item, u16 param, HEAPID heap_id );

//--------------------------------------------------------------------------------------------
/**
 * パラメータ取得（アイテムデータ指定）
 *
 * @param item  アイテムデータ
 * @param param 取得パラメータ定義
 *
 * @return  指定パラメータ
 */
//--------------------------------------------------------------------------------------------
extern s32 ITEM_GetBufParam( ITEMDATA * item, u16 param );

//--------------------------------------------------------------------------------------------
/**
 * @brief		技マシンかどうか
 *
 * @param		item  アイテム番号
 *
 * @retval	"TRUE = 技マシン"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckWazaMachine( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * 技マシンで覚えられる技を取得
 *
 * @param item  アイテム番号
 *
 * @return  技番号
 */
//--------------------------------------------------------------------------------------------
extern const u16 ITEM_GetWazaNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * @brief 選択された技がポケモン交換に適さない技かどうか（秘伝技かどうか）
 *
 * @param waza  技番号
 *
 * @retval  "TRUE = 交換に適さない技(秘伝技"
 * @retval  "FALSE = 交換に適する技(秘伝技以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckHidenWaza( u16 waza );

//--------------------------------------------------------------------------------------------
/**
 * 技マシン番号取得
 *
 * @param item  アイテム番号
 *
 * @return  技マシン番号
 */
//--------------------------------------------------------------------------------------------
extern u8 ITEM_GetWazaMashineNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * アイテムがメールかどうかをチェック
 *
 * @param item  アイテム番号
 *
 * @retval  "TRUE = メール"
 * @retval  "FALSE = メール以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckMail( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * アイテム番号からメールのデザインを取得
 *
 * @param item  アイテム番号
 *
 * @return  デザインID
 */
//--------------------------------------------------------------------------------------------
extern u8 ITEM_GetMailDesign( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * メールのデザインからアイテム番号を取得
 *
 * @param design    デザインID
 *
 * @return  アイテム番号
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_MailNumGet( u8 design );

//--------------------------------------------------------------------------------------------
/**
 * アイテムが木の実かどうかをチェック
 *
 * @param item  アイテム番号
 *
 * @retval  "TRUE = 木の実"
 * @retval  "FALSE = 木の実以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckNuts( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * 木の実の番号を取得
 *
 * @param item  アイテム番号
 *
 * @return  木の実番号
 */
//--------------------------------------------------------------------------------------------
extern u8 ITEM_GetNutsNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * 指定IDの木の実のアイテム番号を取得
 *
 * @param id    木の実のID
 *
 * @return  アイテム番号
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetNutsID( u8 id );

//--------------------------------------------------------------------------------------------
/**
 * アイテムが漢方薬かどうかをチェック
 *
 * @param item  アイテム番号
 *
 * @retval  "TRUE = 漢方薬"
 * @retval  "FALSE = 漢方薬"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckKanpouyaku( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * アイテムテーブルをすべてメモリに展開
 *
 * @param heapID  展開するメモリヒープID
 *
 * @retval  確保したメモリのアドレス
 */
//--------------------------------------------------------------------------------------------
extern  void  *ITEM_LoadDataTable(HEAPID heapID);

//--------------------------------------------------------------------------------------------
/**
 * アイテムテーブルから任意のアドレスを取得
 *
 * @param item  アイテムテーブル
 * @param index とりだすインデックスナンバー
 *
 * @retval  取得したアドレス
 */
//--------------------------------------------------------------------------------------------
extern  ITEMDATA  *ITEM_GetDataPointer(ITEMDATA *item,u16 index);

//--------------------------------------------------------------------------------------------
/**
 * アイテムをポケモンに持たせられるか
 *
 * @param item  アイテム番号
 *
 * @retval  "TRUE = 可"
 * @retval  "FALSE = 不可"
 *
 *  基本、金銀で追加されたもの
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckPokeAdd( u16 item );


//--------------------------------------------------------------------------------------------
/**
 * 古いアイテムで今は使われていないかどうか検査する
 * @param item  アイテム番号
 * @retval  TRUE  = つかえる
 * @retval  FALSE = つかえない
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckEnable( u16 item );


