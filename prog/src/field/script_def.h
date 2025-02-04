//======================================================================
/**
 * @file  script_def.h
 * @brief スクリプト定義ファイル
 *
 * スクリプト環境とプログラムとで共通して参照する必要がある定義を集約するヘッダファイル。
 * スクリプト側はenumなどC特有の文法解釈ができないため、defineを使用すること。
 */
//======================================================================
#pragma once

//======================================================================
//
//  スクリプトヘッダ関連
//
//  スクリプト側ではマクロに埋め込まれているため、直接これらの値を
//  意識的に使用することはない。
//======================================================================
//--------------------------------------------------------------
//特殊スクリプト定義
//--------------------------------------------------------------
#define SP_SCRID_NONE       (0)
#define SP_SCRID_SCENE_CHANGE   (1)     ///<特殊スクリプトテーブルでのシーン発動イベントデータの指定ID
#define SP_SCRID_ZONE_CHANGE    (2)     ///<特殊スクリプトテーブルでのゾーン切り替えスクリプトの指定ID
#define SP_SCRID_FIELD_RECOVER  (3)     ///<特殊スクリプトテーブルでのフィールド復帰スクリプト指定ID
#define SP_SCRID_FIELD_INIT     (4)     ///<特殊スクリプトテーブルでのフィールド初期化スクリプト指定ID

///イベントデータテーブル末尾を判定するための値
#define EV_DATA_END_CODE  (0xfd13)  //適当な値

//======================================================================
//
//    OBJ関連
//
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
//エイリアス識別用スクリプトID
#define SP_SCRID_ALIES        (0xffff)

//連れ歩きOBJを取得するOBJID定義
#define SCR_OBJID_MV_PAIR     (0xf2)

//透明ダミーOBJを取得するOBJID定義
#define SCR_OBJID_DUMMY       (0xf1)

//--------------------------------------------------------------
//--------------------------------------------------------------
#define SCR_OBJPARAM0         (0)
#define SCR_OBJPARAM1         (1)
#define SCR_OBJPARAM2         (2)

//======================================================================
//    トレーナー関連の定義
//======================================================================
//トレーナー視線動作タイプ定義
#define SCR_EYE_TR_TYPE_SINGLE    (0) ///<単独動作
#define SCR_EYE_TR_TYPE_DOUBLE    (1) ///<ダブル動作
#define SCR_EYE_TR_TYPE_TAG       (2) ///<タッグ動作（ひとりずつ動く）

//トレーナーの番号定義
#define SCR_EYE_TR_0        (0)
#define SCR_EYE_TR_1        (1)

//トレーナーの戦闘ルールタイプ定義
#define SCR_TR_BTL_RULE_SINGLE    0   ///<シングル
#define SCR_TR_BTL_RULE_DOUBLE    1   ///<ダブル
#define SCR_TR_BTL_RULE_TRIPLE    2   ///<トリプル
#define SCR_TR_BTL_RULE_ROTATION  3   ///<ローテーション

#define SCR_TR_SPTYPE_NONE        0   ///<トレーナー特殊タイプ：なし
#define SCR_TR_SPTYPE_RECOVER     1   ///<トレーナー特殊タイプ：回復
#define SCR_TR_SPTYPE_ITEM        2   ///<トレーナー特殊タイプ：アイテム

//======================================================================
//    各コマンド依存の定義
//======================================================================
//--------------------------------------------------------------
//fld_menu.c
//--------------------------------------------------------------
#define SCR_REPORT_OK       (0)     //セーブした
#define SCR_REPORT_CANCEL     (1)     //セーブしていない

///YES_NO_WINの戻り値
#define SCR_YES   (0) //YES_NO_WINの戻り値 「はい」を返した
#define SCR_NO    (1) //YES_NO_WINの戻り値 「いいえ」を返した

//ショップのID定義
#define SCR_SHOPID_NULL        ( 0xFF )  //変動ショップ呼び出しID
#define SCR_SHOPID_BP_ITEM     ( 0xFE )  //BPアイテムショップ呼び出しID
#define SCR_SHOPID_BP_WAZA     ( 0xFD )  //BP技マシンショップ呼び出しID
#define SCR_SHOPID_BLACK_CITY  ( 0xFC )  //ブラックシティショップ <---未使用になります
#define SCR_SHOPID_BLACK_CITY4 ( 0xFB )  //ブラックシティショップ呼び出しID:0
#define SCR_SHOPID_BLACK_CITY3 ( 0xFA )  //ブラックシティショップ呼び出しID:1
#define SCR_SHOPID_BLACK_CITY2 ( 0xF9 )  //ブラックシティショップ呼び出しID:2
#define SCR_SHOPID_BLACK_CITY1 ( 0xF8 )  //ブラックシティショップ呼び出しID:3
#define SCR_SHOPID_BLACK_CITY0 ( 0xF7 )  //ブラックシティショップ呼び出しID:4

//サブプロセス(アプリ)の終了コード定義
#define SCR_PROC_RETMODE_EXIT    (0) //一発終了
#define SCR_PROC_RETMODE_NORMAL  (1) //通常終了

//バッグプロセス呼び出しコード定義
#define SCR_BAG_MODE_SELL   (0)
#define SCR_BAG_MODE_SELECT (1)
#define SCR_BAG_MODE_FIELD  (2)

//フィールド通信切断処理イベントの結果
#define SCR_FIELD_COMM_EXIT_OK      (0) //正常に通信切断処理が終わった
#define SCR_FIELD_COMM_EXIT_CANCEL  (1) //切断処理をキャンセル
#define SCR_FIELD_COMM_EXIT_ERROR   (2) //切断処理中になんらかのエラー

//--------------------------------------------------------------
/// BmpMenuの説明メッセージ無効ID
//--------------------------------------------------------------
#define SCR_BMPMENU_EXMSG_NULL   (0xFFFF)

//--------------------------------------------------------------
//  バトル関連
//--------------------------------------------------------------
///TRAINER_LOSE_CHECK,NORMAL_LOSE_CHECK
#define SCR_BATTLE_RESULT_LOSE (0) //負け
#define SCR_BATTLE_RESULT_WIN  (1) //勝ち

#define SCR_BATTLE_MODE_NONE    (0) //特にモード指定なし
#define SCR_BATTLE_MODE_NOLOSE  (1) //負けないモード

///野生ポケモン戦闘呼び出し時に指定できるフラグ
#define SCR_WILD_BTL_FLAG_NONE      (0x0000)  //フラグ無し
#define SCR_WILD_BTL_FLAG_LEGEND    (0x0001)  //伝説戦闘(エンカウントメッセージが変化する)
#define SCR_WILD_BTL_FLAG_RARE      (0x0002)  //強制レアポケ戦闘  ＷＢでは未実装
#define SCR_WILD_BTL_FLAG_ITEMNONE  (0x0004)  //所持アイテム強制無し
#define SCR_WILD_BTL_FLAG_SPEABI3   (0x0008)  //第3特性適用
#define SCR_WILD_BTL_FLAG_NO_RARE   (0x0010)  //レア禁止
#define SCR_WILD_BTL_FLAG_MALE      (0x0020)  //オスと戦闘
#define SCR_WILD_BTL_FLAG_FEMALE    (0x0040)  //メスと戦闘
#define SCR_WILD_BTL_FLAG_WILD_TALK (0x0080)  //話しかけ野生戦闘(エンカウントメッセージが変化する)
#define SCR_WILD_BTL_FLAG_IMMORTAL  (0x0100)  //シン・ム戦闘専用(捕まえるまで何度でも復活するので終了演出が特殊)


///野生ポケモン戦　再戦コードチェック
#define SCR_WILD_BTL_RET_CAPTURE  (0) //捕まえた
#define SCR_WILD_BTL_RET_ESCAPE   (1) //逃げた
#define SCR_WILD_BTL_RET_WIN      (2) //倒した

//--------------------------------------------------------------
//曜日取得関数の戻り値
//  _GET_WEEK
//--------------------------------------------------------------
#define RTC_WEEK_SUNDAY   0
#define RTC_WEEK_MONDAY   1
#define RTC_WEEK_TUESDAY  2
#define RTC_WEEK_WEDNESDAY  3
#define RTC_WEEK_THURSDAY 4
#define RTC_WEEK_FRIDAY   5
#define RTC_WEEK_SATURDAY 6

//--------------------------------------------------------------
//バッジフラグ指定IDの定義
//  _GET_BADGE_FLAG/_SET_BADGE_FLAG
//--------------------------------------------------------------
#define BADGE_ID_01   0
#define BADGE_ID_02   1
#define BADGE_ID_03   2
#define BADGE_ID_04   3
#define BADGE_ID_05   4
#define BADGE_ID_06   5
#define BADGE_ID_07   6
#define BADGE_ID_08   7

//--------------------------------------------------------------
// 手持ちポケモン数カウントモード
//--------------------------------------------------------------
#define POKECOUNT_MODE_TOTAL         (0)  // 手持ちの数
#define POKECOUNT_MODE_NOT_EGG       (1)  // タマゴを除く手持ちの数
#define POKECOUNT_MODE_BATTLE_ENABLE (2)  // 戦える(タマゴと瀕死を除いた)ポケモン数
#define POKECOUNT_MODE_ONLY_EGG      (3)  // タマゴの数(駄目タマゴを除く)
#define POKECOUNT_MODE_ONLY_DAME_EGG (4)  // 駄目タマゴの数
#define POKECOUNT_MODE_EMPTY         (5)  // 空いている数

//--------------------------------------------------------------
//
//--------------------------------------------------------------
#define SCR_POKEPARA_MONSNO           (5)   ///<RO ポケモンナンバー
#define SCR_POKEPARA_ITEMNO           (6)   ///<RO 保持アイテムナンバー
#define SCR_POKEPARA_COUNTRY_CODE     (12)  ///<RO 国コード
#define SCR_POKEPARA_HP_RND           (70)  ///<WO HP乱数
#define SCR_POKEPARA_POW_RND          (71)  ///<WO 攻撃力乱数
#define SCR_POKEPARA_DEF_RND          (72)  ///<WO 防御力乱数
#define SCR_POKEPARA_AGI_RND          (73)  ///<WO 素早さ乱数
#define SCR_POKEPARA_SPEPOW_RND       (74)  ///<WO 特攻乱数
#define SCR_POKEPARA_SPEDEF_RND       (75)  ///<WO 特防乱数
#define SCR_POKEPARA_HAIHU_FLAG       (109) ///<RO イベント配布フラグ
#define SCR_POKEPARA_SEX              (110) ///<RO 性別
#define SCR_POKEPARA_FORMNO           (111) ///<RO フォルムナンバー
#define SCR_POKEPARA_SEIKAKU          (112) ///<RO 性格
#define SCR_POKEPARA_NICKNAME_FLAG    (117) ///<RO ニックネームフラグ
#define SCR_POKEPARA_GET_ROM          (119) ///<RO 捕まえたロム
//#define SCR_POKEPARA_GET_YEAR         (143) ///<RO タマゴを入手した時の年
//#define SCR_POKEPARA_GET_MONTH        (144) ///<RO タマゴを入手した時の月
//#define SCR_POKEPARA_GET_DAY          (145) ///<RO タマゴを入手した時の日
//#define SCR_POKEPARA_BIRTH_YEAR       (146) ///<RO 生まれた or 捕まえた年
//#define SCR_POKEPARA_BIRTH_MONTH      (147) ///<RO 生まれた or 捕まえた月
//#define SCR_POKEPARA_BIRTH_DAY        (148) ///<RO 生まれた or 捕まえた日
//#define SCR_POKEPARA_GET_PLACE        (149) ///<RO タマゴを入手した場所
//#define SCR_POKEPARA_BIRTH_PLACE      (150) ///<RO 生まれた or 捕獲した場所
#define SCR_POKEPARA_GET_LEVEL        (153) ///<RO 捕獲した時のレベル
#define SCR_POKEPARA_OYA_SEX          (154) ///<RO 親の性別
#define SCR_POKEPARA_LEVEL            (158) ///<RO 現在のレベル

//--------------------------------------------------------------
// ポケモン捕獲場所　チェック
//--------------------------------------------------------------
#define POKE_GET_PLACE_CHECK_WF         (0)  // ホワイトフォレストで捕獲したのかチェック
#define POKE_GET_PLACE_CHECK_MAX        (1)  // ホワイトフォレストで捕獲したのかチェック

//--------------------------------------------------------------
//  ずかん操作モード指定
//--------------------------------------------------------------
#define ZUKANCTRL_MODE_SEE     (0)   ///<ずかん操作モード：見た数
#define ZUKANCTRL_MODE_GET     (1)   ///<ずかん操作モード：捕まえた数

//--------------------------------------------------------------
//  配置モデル用アニメ指定
//--------------------------------------------------------------
#define SCR_BMID_DOOR           (1)       ///<配置モデル分類：ドア
#define SCR_BMID_SANDSTREAM     (2)       ///<配置モデル分類：流砂
#define SCR_BMID_PCMACHINE      (3)       ///<配置モデル分類：回復マシン
#define SCR_BMID_PC             (4)       ///<配置モデル分類：パソコン
#define SCR_BMID_PCEV_DOOR      (5)       ///<配置モデル分類：ポケセンエレベーター：ドア
#define SCR_BMID_PCEV_FLOOR     (6)       ///<配置モデル分類：ポケセンエレベーター：フロア
#define SCR_BMID_WARP           (7)       ///<配置モデル分類：ワープ
#define SCR_BMID_EVENT          (8)       ///<配置モデル分類：ワープ
#define SCR_BMID_PPMACHINE      (9)       ///<配置モデル分類：パルパークマシン

#define  SCR_BMANM_DOOR_OPEN    (0)       ///<ドアアニメ：開く
#define  SCR_BMANM_DOOR_CLOSE   (1)       ///<ドアアニメ：閉じる

#define  SCR_BMANM_PCEV_OPEN    (0)       ///<ポケセンエレベータアニメ：開く
#define  SCR_BMANM_PCEV_CLOSE   (1)       ///<ポケセンエレベータアニメ：閉じる

#define  SCR_BMANM_PCEV_UP      (0)       ///<ポケセンエレベータアニメ：上がる
#define  SCR_BMANM_PCEV_DOWN    (1)       ///<ポケセンエレベータアニメ：下がる

#define  SCR_BMANM_PPM_MOV    (0)       ///<パルパークマシンアニメ：起動
#define  SCR_BMANM_PPM_LOOP    (1)       ///<パルパークマシンアニメ：ループ

//--------------------------------------------------------------
/// メッセージウィンドウ上下指定
//--------------------------------------------------------------
#define SCRCMD_MSGWIN_DEFAULT   (0)
#define SCRCMD_MSGWIN_UP        (1)
#define SCRCMD_MSGWIN_DOWN      (2)
#define SCRCMD_MSGWIN_UPLEFT    (3)
#define SCRCMD_MSGWIN_DOWNLEFT  (4)
#define SCRCMD_MSGWIN_UPRIGHT   (5)
#define SCRCMD_MSGWIN_DOWNRIGHT (6)
#define SCRCMD_MSGWIN_MAX       (7)

//--------------------------------------------------------------
/// システムメッセージウィンドウ　タイムアイコン　スイッチ
//--------------------------------------------------------------
#define SCR_SYSWIN_TIMEICON_ON  (0)
#define SCR_SYSWIN_TIMEICON_OFF (1)

//--------------------------------------------------------------
/// BGウィンドウタイプ
//--------------------------------------------------------------
#define SCRCMD_BGWIN_TYPE_INFO (0)
#define SCRCMD_BGWIN_TYPE_TOWN (1)
#define SCRCMD_BGWIN_TYPE_POST (2)
#define SCRCMD_BGWIN_TYPE_ROAD (3)

//--------------------------------------------------------------
/// 特殊ウィンドウタイプ
//--------------------------------------------------------------
#define SCRCMD_SPWIN_TYPE_LETTER (0)
#define SCRCMD_SPWIN_TYPE_BOOK (1)

//--------------------------------------------------------------
/// バトルレコーダー呼び出しモード指定
//--------------------------------------------------------------
#define SCRCMD_BTL_RECORDER_MODE_VIDEO   (0)
#define SCRCMD_BTL_RECORDER_MODE_MUSICAL (1)

//--------------------------------------------------------------
/// 対戦時パーティタイプ指定
//--------------------------------------------------------------
#define SCR_BTL_PARTY_SELECT_TEMOTI   (0) //手持ち
#define SCR_BTL_PARTY_SELECT_BTLBOX   (1) //バトルボックス
#define SCR_BTL_PARTY_SELECT_CANCEL   (2) //選択キャンセル
#define SCR_BTL_PARTY_SELECT_NG       (3) //レギュレーションNG

//--------------------------------------------------------------
/// ゲーム内交換可能チェック戻り値
//--------------------------------------------------------------
#define FLD_TRADE_ENABLE         (0)  // 交換可
#define FLD_TRADE_DISABLE_EGG    (1)  // 交換不可(タマゴのため)
#define FLD_TRADE_DISABLE_MONSNO (2)  // 交換不可(違うモンスターのため)
#define FLD_TRADE_DISABLE_SEX    (3)  // 交換不可(違う性別のため)

//--------------------------------------------------------------
/// ワイヤレスセーブモード
//--------------------------------------------------------------
#define WIRELESS_SAVEMODE_OFF (0)
#define WIRELESS_SAVEMODE_ON  (1)

//--------------------------------------------------------------
/// 技思い出し結果
//--------------------------------------------------------------
#define SCR_WAZAOSHIE_RET_SET    (0)
#define SCR_WAZAOSHIE_RET_CANCEL (1)

//--------------------------------------------------------------
/// 技おぼえモード指定
//--------------------------------------------------------------
#define SCR_SKILLTEACH_MODE_DRAGON    (0)
#define SCR_SKILLTEACH_MODE_STRONGEST (1)
#define SCR_SKILLTEACH_MODE_COALESCENCE (2)

#define SCR_SKILLTEACH_CHECK_RESULT_OK         (0)
#define SCR_SKILLTEACH_CHECK_RESULT_POKEMON_NG (1)
#define SCR_SKILLTEACH_CHECK_RESULT_NATSUKI_NG (2)
#define SCR_SKILLTEACH_CHECK_RESULT_ALREADY_NG (3)

//--------------------------------------------------------------
/// ボックス終了モード
//--------------------------------------------------------------
#define SCR_BOX_END_MODE_MENU   (0)  // 選択メニューへ戻る
#define SCR_BOX_END_MODE_C_GEAR (1)  // C-gearまで戻る

//--------------------------------------------------------------
/// メールボックス終了モード
//--------------------------------------------------------------
#define SCR_MAILBOX_END_MODE_MENU   (0)  // 選択メニューへ戻る
#define SCR_MAILBOX_END_MODE_C_GEAR (1)  // C-gearまで戻る

//--------------------------------------------------------------
// パソコンの殿堂入り画面呼び出し
//--------------------------------------------------------------
#define SCRCMD_DENDOU_PC_FIRST  (0) //「はじめてクリア」
#define SCRCMD_DENDOU_PC_DENDOU (1) //「殿堂入り」

//--------------------------------------------------------------
/// ゲームマニュアル終了モード
//--------------------------------------------------------------
#define SCR_MANUAL_RESULT_CLOSE  (0) // ×ボタン
#define SCR_MANUAL_RESULT_RETURN (1) // 戻りボタン

//--------------------------------------------------------------
//  ジャッジの判定で取得する項目
//--------------------------------------------------------------
#define SCR_JUDGE_MODE_TOTAL      (0)
#define SCR_JUDGE_MODE_HIGH_VALUE (1)
#define SCR_JUDGE_MODE_HP         (2)
#define SCR_JUDGE_MODE_POW        (3)
#define SCR_JUDGE_MODE_DEF        (4)
#define SCR_JUDGE_MODE_AGI        (5)
#define SCR_JUDGE_MODE_SPPOW      (6)
#define SCR_JUDGE_MODE_SPDEF      (7)

//--------------------------------------------------------------
//  ずかん評価のモード指定
//--------------------------------------------------------------
//#define SCR_ZUKAN_HYOUKA_MODE_AUTO        (0)   ///<自動判別（パソコン）
#define SCR_ZUKAN_HYOUKA_MODE_GLOBAL      (1)   ///<常に全国ずかん（博士・父）
#define SCR_ZUKAN_HYOUKA_MODE_LOCAL_SEE   (2)   ///<地方ずかん・見た（博士・娘）
#define SCR_ZUKAN_HYOUKA_MODE_LOCAL_GET   (3)   ///<地方ずかん・見た（博士・娘）

//--------------------------------------------------------------
/// ボックス追加壁紙指定  _SET_BOX_WALLPAPER()に使用
//--------------------------------------------------------------
#define SCR_BOX_EX_WALLPAPER1   (1)
#define SCR_BOX_EX_WALLPAPER2   (2)

//--------------------------------------------------------------
//  不思議な贈り物　配達員
//--------------------------------------------------------------
#define SCR_POSTMAN_REQ_EXISTS  (0) ///<存在チェック
#define SCR_POSTMAN_REQ_ENABLE  (1) ///<受け取れるか？チェック
#define SCR_POSTMAN_REQ_OK_MSG  (2) ///<受け取ったメッセージID
#define SCR_POSTMAN_REQ_NG_MSG  (3) ///<受け取れないメッセージID
#define SCR_POSTMAN_REQ_RECEIVE (4) ///<受け取り処理
#define SCR_POSTMAN_REQ_OBJID   (5) ///<配達員のOBJIDを取得
#define SCR_POSTMAN_REQ_OBJSTAT (6) ///<配達員OBJの状態取得
#define SCR_POSTMAN_REQ_TYPE    (7) ///<受け取る贈り物のタイプ


#define SCR_POSTMAN_OBJ_NONE    (0) ///<配達員OBJ:存在しない
#define SCR_POSTMAN_OBJ_EXISTS  (1) ///<配達員OBJ:存在する

#define SCR_POSTMAN_TYPE_POKEMON        (1) ///<贈り物の種類：ポケモン
#define SCR_POSTMAN_TYPE_TAMAGO         (2) ///<贈り物の種類：タマゴ
#define SCR_POSTMAN_TYPE_NORMAL_ITEM    (3) ///<贈り物の種類：アイテム
#define SCR_POSTMAN_TYPE_LIBERTY_TICKET (4) ///<贈り物の種類：大事なアイテム（リバティチケット）
#define SCR_POSTMAN_TYPE_GPOWER         (5) ///<贈り物の種類：Gパワー

//--------------------------------------------------------------
//  PDW配達員
//--------------------------------------------------------------
#define SCR_PDW_POSTMAN_REQ_EXISTS      (0) ///<存在チェック
#define SCR_PDW_POSTMAN_REQ_ENABLE      (1) ///<受け取れるか？チェック
#define SCR_PDW_POSTMAN_REQ_OK_INFO     (2) ///<受け取り情報表示
#define SCR_PDW_POSTMAN_REQ_RECEIVE     (3) ///<受け取り処理
#define SCR_PDW_POSTMAN_REQ_NG_COUNT    (4) ///<受け取れないカウント取得
#define SCR_PDW_POSTMAN_REQ_NG_ITEM     (5) ///<受け取れないどうぐ名セット

//--------------------------------------------------------------
/// ビーコン送信リクエストID
//--------------------------------------------------------------
#define SCR_BEACON_SET_REQ_ITEM_GET           (0) ///<フィールドでアイテムを拾った
#define SCR_BEACON_SET_REQ_FSKILL_USE         (1) ///<フィールド技使用系
#define SCR_BEACON_SET_REQ_UNION_COUNTER      (2) ///<ユニオンカウンター処理開始開始
#define SCR_BEACON_SET_REQ_TRIAL_HOUSE_START  (3) ///<トライアルハウス挑戦開始
#define SCR_BEACON_SET_REQ_TRIAL_HOUSE_RANK   (4) ///<トライアルハウスランクセット
#define SCR_BEACON_SET_REQ_POKE_SHIFTER_START (5) ///<ポケシフター挑戦開始
#define SCR_BEACON_SET_REQ_SUBWAY_START       (6) ///<サブウェイ挑戦開始
#define SCR_BEACON_SET_REQ_SUBWAY_WIN         (7) ///<サブウェイ勝利
#define SCR_BEACON_SET_REQ_SUBWAY_TROPHY_GET  (8) ///<サブウェイトロフィーゲット

//--------------------------------------------------------------
//    タイトルからのスタートメニュー項目
//--------------------------------------------------------------
#define SCR_STARTMENU_FLAG_HUSHIGI   (0)    ///<「不思議な贈り物」
#define SCR_STARTMENU_FLAG_BATTLE    (1)    ///<「バトル大会」
#define SCR_STARTMENU_FLAG_GAMESYNC  (2)    ///<「ゲームシンク設定」
#define SCR_STARTMENU_FLAG_MACHINE   (3)    ///<「転送マシンをつかう」

//--------------------------------------------------------------
//  移動ポケモン
//  src/field/event_movepoke.hの定義と一致している必要がある。
//--------------------------------------------------------------
#define SCR_EV_MOVEPOKE_ANIME_RAI_KAZA_INSIDE (0)
#define SCR_EV_MOVEPOKE_ANIME_RAI_KAZA_OUTSIDE (1)
#define SCR_EV_MOVEPOKE_ANIME_TUCHI_INSIDE  (2)
#define SCR_EV_MOVEPOKE_ANIME_TUCHI_OUTSIDE (3)


//--------------------------------------------------------------
//  サブスクリーン操作
//--------------------------------------------------------------
#define SCR_EV_SUBSCREEN_MODE_SAVE  (0) // セーブ画面にする
#define SCR_EV_SUBSCREEN_MODE_MAX  (1)


//--------------------------------------------------------------
//  海底神殿操作
//--------------------------------------------------------------
#define SCR_EV_DIVING_MAPCHANGE_DOWN    (0)
#define SCR_EV_DIVING_MAPCHANGE_UP      (1)
#define SCR_EV_DIVING_MAPCHANGE_KURUKURU_UP      (2)
#define SCR_EV_DIVING_MAPCHANGE_MAX     (3)

//--------------------------------------------------------------
///　図鑑完成賞状&路線図表示アプリ起動コード (同一アプリを呼び出す)
//--------------------------------------------------------------
#define SCR_ZUKAN_AWARD_CHIHOU    (0)  // 地方図鑑完成
#define SCR_ZUKAN_AWARD_ZENKOKU   (1)  // 全国図鑑完成
#define SCR_ZUKAN_AWARD_SUBWAY_ROUTE_MAP  (2)  // 路線図

//--------------------------------------------------------------
/// 自機動作形態
//--------------------------------------------------------------
#define SCR_PLAYER_FORM_NORMAL (0)
#define SCR_PLAYER_FORM_CYCLE (1)
#define SCR_PLAYER_FORM_SWIM (2)
#define SCR_PLAYER_FORM_DIVING (3)

//--------------------------------------------------------------
/// ボールアニメタイプ指定用
//--------------------------------------------------------------
#define SCR_BALL_ANM_TYPE_OUT   (0)
#define SCR_BALL_ANM_TYPE_IN   (1)


//--------------------------------------------------------------
/// 鑑定士用
//--------------------------------------------------------------
#define SCR_ITEM_JUDGE_OBJTYPE_GOURMET      (0)   // グルメ
#define SCR_ITEM_JUDGE_OBJTYPE_STONE_MANIA  (1)   // 石マニア
#define SCR_ITEM_JUDGE_OBJTYPE_RICH         (2)   // 大富豪
#define SCR_ITEM_JUDGE_OBJTYPE_KOUKO        (3)   // 考古学者

//--------------------------------------------------------------
//    シンボルエンカウント：情報取得用
//--------------------------------------------------------------
#define SCR_SYMMAP_INFO_IS_MINE         (0)   ///<自分のパレスマップか？
#define SCR_SYMMAP_INFO_IS_KEEPZONE     (1)   ///<キープゾーンか？
#define SCR_SYMMAP_INFO_IS_ENTRANCE     (2)   ///<入ったマップか？
#define SCR_SYMMAP_INFO_FRONT_MONSNO    (3)   ///<話しかけ対象のポケモンナンバー
#define SCR_SYMMAP_INFO_FRONT_FORMNO    (4)   ///<話しかけ対象のフォルムナンバー


//--------------------------------------------------------------
// 「殿堂入り」データチェックの戻り値
//--------------------------------------------------------------
#define SCR_DENDOU_DATA_NULL (0) // データなし
#define SCR_DENDOU_DATA_OK   (1) // 正常データあり
#define SCR_DENDOU_DATA_NG   (2) // 異常データあり

//--------------------------------------------------------------
//    ゲームクリア指定    _GOTO_GAMECLEAR_DEMO()コマンド用
//    prog/src/field/event_gameclear.hのGAMECLEAR_MODEと一致している必要がある
//--------------------------------------------------------------
#define SCR_GAMECLEAR_MODE_FIRST      (0)     ///<初回ゲームクリア
#define SCR_GAMECLEAR_MODE_DENDOU     (1)     ///<2回目以降、殿堂入り



//--------------------------------------------------------------
// 交換ポケモン　と　再戦イベント
// savedata/tradepoke_after_save.h TRPOKE_AFTER_SAVE_TYPEと一致
//--------------------------------------------------------------
#define SCR_TRPOKE_AFTER_SAVE_C02 (0)    // シッポウ
#define SCR_TRPOKE_AFTER_SAVE_C05 (1)    // ホドモエ




//--------------------------------------------------------------
//    c01ジムカーテン状況
//--------------------------------------------------------------
#define SCR_GYM01_CURTAIN_NOTOPEN     (0)     ///<カーテン空いてない
#define SCR_GYM01_CURTAIN_OPEN_ONE    (1)     ///<カーテン1つ開いている 
#define SCR_GYM01_CURTAIN_OPEN_TWO    (3)     ///<カーテン2つ開いている 
#define SCR_GYM01_CURTAIN_OPEN_THREE  (7)     ///<カーテン3つ開いている


//--------------------------------------------------------------
//    ポケモン作成 性別指定
//    @note poke_tool.hの PtlSexSpecと一致
//    @note オスメスのあるポケモンでSCR_PTL_SEX_UNKNOWNを指定すると不問になる
//--------------------------------------------------------------
#define SCR_SEX_SPEC_MALE     (0)   //♂
#define SCR_SEX_SPEC_FEMALE   (1)   //♀
#define SCR_SEX_SPEC_UNKNOWN  (2)   //どちらでも可

//--------------------------------------------------------------
//    ポケモン作成 特性指定
//    @note poke_tool.hの PtlTokuseiSpecと一致
//--------------------------------------------------------------
#define SCR_TOKUSEI_SPEC_1    (0)     //特性1をセット
#define SCR_TOKUSEI_SPEC_2    (1)     //特性2をセット
#define SCR_TOKUSEI_SPEC_BOTH (2)     //どちらでも可

//--------------------------------------------------------------
//    ポケモン作成 レア指定
//    @note poke_tool.hの PtlRareSpecと一致
//--------------------------------------------------------------
#define SCR_RARE_SPEC_FALSE   (0)  //レアにしない
#define SCR_RARE_SPEC_TRUE    (1)  //レアにする
#define SCR_RARE_SPEC_BOTH    (2)  //どちらでも可

//--------------------------------------------------------------
//  侵入：通信相手のパレスマップに動的に配置する爺さんのOBJ_ID
//--------------------------------------------------------------
#define OBJID_PALACE_OLDMAN1   (100)  //自分のパレス島の右隣にいる爺さん
#define OBJID_PALACE_OLDMAN2   (101)  //自分のパレス島の右方向の2つ先にいる爺さん

//--------------------------------------------------------------
//  Ｎ城謁見の間のクライマックスデモ専用コマンド定義
//--------------------------------------------------------------
#define SCR_CRIMAX_CMD_TEMOTI_CHECK (0)   ///<手持ちに入ったかのチェック
#define SCR_CRIMAX_CMD_SORT         (1)   ///<先頭に来るように並び替える
#define SCR_CRIMAX_CMD_GET_FROM_BOX (2)   ///<ボックスから手持ちに入れる（引数＝交換する位置）

//--------------------------------------------------------------
//  天気　ゾーンの基本天気設定
//--------------------------------------------------------------
#define SCR_WEATHER_NO_ZONE_DEF ( 0xffff )

//--------------------------------------------------------------
// 育て屋: 特殊チェック項目
//--------------------------------------------------------------
#define SCR_SODATEYA_CHECK_ID_TAKEBACK_MENU (0) // 引き取りメニューの分類
//--------------------------------------------------------------
// 育て屋: 特殊チェックの結果
//--------------------------------------------------------------
// 引き取りメニューの分類 ( SCR_SODATEYA_CHECK_ID_TAKEBACK_MENU )
#define SCR_SODATEYA_CHECK_RESULT_TAKEBACK_MENU_UNKNOWN (0) // 性別なし
#define SCR_SODATEYA_CHECK_RESULT_TAKEBACK_MENU_MALE    (1) // ♂
#define SCR_SODATEYA_CHECK_RESULT_TAKEBACK_MENU_FEMALE  (2) // ♀

//--------------------------------------------------------------
// パソコン: 停止方法
//--------------------------------------------------------------
#define SCR_PASOKON_OFF_NORMAL (0) // 通常
#define SCR_PASOKON_OFF_QUICK  (1) // XBREAK
