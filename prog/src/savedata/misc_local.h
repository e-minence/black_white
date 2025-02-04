
#include "pm_define.h"
#include "field/research_team_def.h"

///すれ違い：自己紹介メッセージ長
#define SAVE_SURETIGAI_SELFINTRODUCTION_LEN   (8 + 1)  //EOM込み
///すれ違い：お礼メッセージ長
#define SAVE_SURETIGAI_THANKYOU_LEN           (8 + 1)  //EOM込み

#define GYM_MAX     (8)
#define EVT_ROCK_MAX     (8)

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	未分類セーブデータ
//	・どこに分類するべきか判断に迷うもの。
//	・規模が小さくそのためにブロック確保をするのがもったいないもの
//=====================================
struct _MISC
{	
  u32 gold;         //お金
  u32 badge;     //バッチBIT

  //お気に入りポケモン
  u16 favorite_monsno;		//お気に入りポケモン
	u8  favorite_form_no:6;		//お気に入りポケモンのフォルム番号
	u8  favorite_egg_flag:1;	//お気に入りポケモンのタマゴフラグ
  u8  favorite_sex:1;       //お気に入りポケモンの性別  ポケアイコン用の性別なので　♀のときのみON
  u8  padding;
	u8	namein_mode[NAMEIN_SAVEMODE_MAX];	//8つ

	//パルパーク
	u32 palpark_highscore:28;
	u32 palpark_finish_state:4;

	//すれ違い
  u32 thanks_recv_count;         ///<お礼を受けた回数
  u32 suretigai_count;           ///<すれ違い人数
  STRCODE self_introduction[SAVE_SURETIGAI_SELFINTRODUCTION_LEN]; ///<自己紹介
  STRCODE thankyou_message[SAVE_SURETIGAI_THANKYOU_LEN];  ///<お礼メッセージ
  u8 research_team_rank;         ///<調査隊隊員ランク(RESEARCH_TEAM_RANK_xxx)

	//タイトルメニュー
	u8	start_menu_open;		///<メニュー表示

  // すれ違い調査隊
  u8  research_request_id; // 受けている調査依頼ID ( RESEARCH_REQ_ID_xxxx )
  u8  research_question_id[MAX_QNUM_PER_RESEARCH_REQ]; // 調べている質問ID (3つ)
  u16 research_count[MAX_QNUM_PER_RESEARCH_REQ]; // 調査開始からの調査人数(3つ)
  s64 research_start_time;  // 調査依頼を受けた時の時間[秒]

  //ジムリーダー戦勝利時の手持ち記録
  u16 gym_win_monsno[GYM_MAX][TEMOTI_POKEMAX];

  //配布イベント用予約領域３２バイト
  u32 event_lock[EVT_ROCK_MAX];

  //購入特典不思議配布用フラグ
  u32 fushigi_purchase_privilege;

  //バトルレコーダー色変えフラグ
  u32 battle_recorder_flag;

  // 電光掲示板
  u16 champ_news_minutes; // チャンピオン情報を流す残り時間[min]
  
  u8 player_trainer_data[10];  ///< プレイヤー自身のトレーナーデータ(使用方未定)
};

