//======================================================================
/**
 * @file	usescript.h
 * @brief	スクリプトで使用するファイルのインクルードなど
 * @author	Satoshi Nohara
 * @date	2005.10.04
 */
//======================================================================
#ifndef __USESCRIPT_H__
#define __USESCRIPT_H__

#define	__ASM_NO_DEF_

#define ASM_CPP							//pm_version.h

//======================================================================
//	define
//======================================================================
//#ifdef PM_DEBUG
#define SCRIPT_PM_DEBUG					//*.ev内でPM_DEBUGが無効なので
#define SCRIPT_PL_NULL					//機能無効化
//#endif

//--------------------------------------------------------------
//(エメラルドではvmstat.hに定義してあった)
//--------------------------------------------------------------
#define	LT				0				/* <	*/
#define	EQ				1				/* ==	*/
#define	GT				2				/* >	*/
#define	LE				3				/* <=	*/
#define	GE				4				/* >=	*/
#define	NE				5				/* !=	*/
#define	EQUAL			EQ
#define	LITTLER			LT
#define	GREATER			GT
#define	LT_EQ			LE
#define	GT_EQ			GE

#define	FLGON			1
#define	FLGOFF			0

#define CMPID_LT    0
#define CMPID_EQ    1
#define CMPID_GT    2
#define CMPID_LT_EQ 3
#define CMPID_GT_EQ 4
#define CMPID_NE    5
#define CMPID_OR    6
#define CMPID_AND   7
#define CMPID_TRUE  8
#define CMPID_FALSE 9
#define CMPID_GET   0xff

//nitro/type.hで定義されているがインクルードできないので自力で定義
#define	TRUE			1
#define	FALSE			0

//スクリプト通信受付の判別定義
#define SCR_COMM_ID_NOTHING	(0)	//何もなし
#define SCR_COMM_ID_BATTLE	(1)	//バトル
#define SCR_COMM_ID_UNION	(2)	//ユニオン
#define SCR_COMM_ID_WIFI	(3)	//WiFi
#define SCR_COMM_ID_RECORD	(4)	//レコード(sp_***.evに埋め込んでしまったので残している)
#define SCR_COMM_ID_CONTEST	(5)			//コンテスト
#define SCR_COMM_ID_GTC		(6)			//GTC

//--------------------------------------------------------------
//*.evの中で使用する定義
//--------------------------------------------------------------

/*
	// 話かけた方向を保持するワーク
	u16 TalkSiteWork = 0;

	// 手持ちがいっぱいでＰＣに転送された時
	u16 BoxNoWork = 0;			//何番のBOXか
	u16 PasoNoWork = 0;			//そのBOXの何番目か

	//ユニオンルームでのイベント状態制御用ワーク
	u16 UnionRoomWork = 0;
*/

/*アンノーンいせき*/
#define ANOON_SEE_NUM_1	(10)
#define ANOON_SEE_NUM_2	(26)

//メニュー用戻り値
#define EV_WIN_B_CANCEL (0xfffe)

//フェード共通速度
#define DISP_FADE_SPEED (8)


#define BAG_POCKET_NORMAL   0
#define BAG_POCKET_DRUG     1
#define BAG_POCKET_WAZA     2
#define BAG_POCKET_NUTS     3
#define BAG_POCKET_EVENT    4

//======================================================================
//	
//======================================================================
//共通スクリプトデータID
#include "common_scr_def.h"

//動作モデル表示コード
#include "../../../prog/src/field/fldmmdl_code.h"
#include "../../../prog/arc/fieldmap/fldmmdl_objcode.h"

//方向
#include "../../../prog/include/field/field_dir.h"

//src/field
#include "../../../prog/src/field/script_def.h"

//イベントワーク、フラグ
#include "../flagwork/flag_define.h"  //イベントフラグ
#include "../flagwork/work_define.h"  //イベントワーク

//サウンドラベル
#include "../../sound/wb_sound_data.sadl"

#include "../../personal/monsno_def.h"		//ポケモンナンバー
#include "../../itemconv/itempocket_def.h"  //どうぐポケットナンバー指定
#include "../../../prog/include/item/itemsym.h"

#include "../zonetable/zone_id.h"     //ゾーンID参照

//トレーナーID
#include "../../../prog/include/tr_tool/trno_def.h"

#if 0 //wb null

#include "../../../include/pm_version.h"

//include/field
#include "../../../include/field/evwkdef.h"
#include "../../../include/field/poketch_app_no.h"	//ポケッチアプリナンバー
#include "../../../include/field/weather_no.h"		//天候ナンバー

#include "../../../include/system/brightness.h"		//輝度
#include "../../../include/system/window.h"			//ウィンドウ
#include "../../../include/system/wipe.h"			//ワイプ
#include "../../../include/system/timezone.h"		//時間帯

#include "../../../include/battle/battle_common.h"	//戦闘関連
#include "../../../include/battle/trno_def.h"		//トレーナーナンバー定義

#include "../../../include/application/br_sys_def.h"//GDS定義
#include "../../../include/application/wifi_p2pmatch_def.h"	//P2P対戦マッチングボード
#include "../../../include/application/imageClip/imc_itemid_define.h"	//イメージクリップ定義
#include "../../../include/application/box_mode.h"	//ボックスモード定義
#include "../../../include/application/bag_def.h"	//ポケット定義
#include "../../../include/application/seal_id.h"	//シール定義
#include "../../../include/contest/contest_def.h"	//コンテスト
#include "../../../include/itemtool/itemsym.h"		//アイテムナンバー定義
#include "../../../include/communication/comm_def.h"//通信

#include "../../../include/battle/wazano_def.h"		//ワザNo
#include "../../../include/battle/battle_server.h"	//フォルムNo

//include/savedata
#include "../../../include/savedata/fnote_mem.h"	//冒険ノート
#include "../../../include/savedata/score_def.h"	//スコア
#include "../../../include/savedata/record.h"	//スコア

//src/field
#include "../../field/script_def.h"					//特殊スクリプト定義
#include "../../field/ev_win.h"						//イベントウィンドウ定義
#include "../../field/comm_union_def.h"				//ユニオン定義
#include "../../field/comm_union_beacon.h"			//ユニオンルーム用ビーコン定義
#include "../../field/comm_direct_counter_def.h"	//通信ダイレクトコーナー
#include "../../field/fieldobj_code.h"				//フィールドOBJで使用するシンボル、コード宣言
#include "../../field/player_code.h"				//REQBIT
#include "../../field/board.h"						//看板
#include "../../field/honey_tree_def.h"				//蜜木ステート定義
#include "../../field/sysflag_def.h"				//システムフラグ定義
#include "../../field/syswork_def.h"				//システムワーク定義
#include "../../field/gym_def.h"					//ジム関連定義
#include "../../field/safari_train_def.h"
#include "../../field/field_trade.h"				//ゲーム内交換
#include "../../field/scr_cycling.h"				//サイクリングロード座標

#include "../../data/sound/pl_sound_data.sadl"		//サウンドナンバー定義
#include "../../../include/system/snd_def.h"		//サウンド定義の置き換え定義
#include "../maptable/zone_id.h"					//ゾーンID定義
#include "../shopdata/fs_item_def.h"				//固定アイテムショップ定義
#include "../shopdata/fs_goods_def.h"				//固定グッズショップ定義
#include "../shopdata/fs_seal_def.h"				//固定シールショップ定義

#include "../../field/move_pokemon_def.h"			//移動ポケモン定義
#include "../../field/elevator_anm_def.h"			//エレベータ関連定義
#include "../../field/ship_demo_def.h"				//船デモ関連定義

#include "../../frontier/factory_def.h"				//ファクトリー定義
#include "../../frontier/stage_def.h"				//ステージ定義
#include "../../frontier/castle_def.h"				//キャッスル定義
#include "../../frontier/roulette_def.h"			//ルーレット定義
#include "../../frontier/fss_scene.h"				//フロンティアシーン定義
#include "../../frontier/frontier_def.h"			//フロンティア定義
#endif //wb null

#include "../../../prog/include/waza_tool/wazano_def.h" //ワザNo

//ワイプフェードの基本の値
#define SCR_WIPE_DIV				(WIPE_DEF_DIV)
#define SCR_WIPE_SYNC				(WIPE_DEF_SYNC)

//======================================================================
//
//	共通メッセージ関連
//
//======================================================================
#include "../../message/dst/script/msg_common_scr.h"
//#include "../../../prog/include/msg/script/msg_common_scr.h"
#if 0 //wb null
#include "../../../include/msgdata/msg_common_scr.h"
#include "../../../include/msgdata/msg_ev_win.h"
#include "../../../include/msgdata/msg_gameover.h"
//#include "msglist.h"
#endif //wb null

#endif //__USESCRIPT_H__
