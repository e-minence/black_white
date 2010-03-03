
#include "pm_define.h"

///すれ違い：自己紹介メッセージ長
#define SAVE_SURETIGAI_SELFINTRODUCTION_LEN   (10 + 1)  //EOM込み
///すれ違い：お礼メッセージ長
#define SAVE_SURETIGAI_THANKYOU_LEN           (10 + 1)  //EOM込み

#define GYM_MAX     (8)


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

  u16 favorite_monsno;		//お気に入りポケモン
	u8  favorite_form_no:7;		//お気に入りポケモンのフォルム番号
	u8  favorite_egg_flag:1;	//お気に入りポケモンのタマゴフラグ
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

  u8 padding[2];

  //ジムリーダー戦勝利時の手持ち記録
  u16 gym_win_monsno[GYM_MAX][TEMOTI_POKEMAX];
};

