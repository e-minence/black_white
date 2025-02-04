////////////////////////////////////////////////////////////////
/**
 * @brief  調査レーダー 調査内容変更画面のインデックス定義
 * @file   research_list_index.h
 * @author obata
 * @date   2010.02.15
 */
////////////////////////////////////////////////////////////////
#pragma once


// 状態
typedef enum {
  RRL_STATE_SETUP,               // 準備
  RRL_STATE_STANDBY,             // 最初のキー入力待ち
  RRL_STATE_KEY_WAIT,            // キー入力待ち
  RRL_STATE_AUTO_SCROLL,         // 自動スクロール
  RRL_STATE_SLIDE_CONTROL,       // スクロール操作
  RRL_STATE_RETURN_FROM_STANDBY, // 戻る ( From キー最初のキー入力待ち )
  RRL_STATE_RETURN_FROM_KEYWAIT, // 戻る ( From キー入力待ち )
  RRL_STATE_TO_CONFIRM_STANDBY,  // 調査項目確定の確認 ( スタンバイ ) へ
  RRL_STATE_CONFIRM_STANDBY,     // 調査項目確定の確認 ( スタンバイ )
  RRL_STATE_TO_CONFIRM_KEY_WAIT, // 調査項目確定の確認 ( キー入力待ち ) へ
  RRL_STATE_CONFIRM_KEY_WAIT,    // 調査項目確定の確認 ( キー入力待ち )
  RRL_STATE_CONFIRM_CANCEL,      // 調査項目確定の確認をキャンセル
  RRL_STATE_CONFIRM_DETERMINE,   // 調査項目確定
  RRL_STATE_CLEAN_UP,            // 後片付け
  RRL_STATE_FINISH,              // 終了
} RRL_STATE;

// 選択項目
typedef enum {
  MENU_ITEM_DETERMINATION_OK,     //「けってい」
  MENU_ITEM_DETERMINATION_CANCEL, //「やめる」
  MENU_ITEM_NUM,                  // 総数
} MENU_ITEM;

// タッチ範囲 ( メニュー項目 )
typedef enum {
  MENU_TOUCH_AREA_OK_BUTTON,     //「決定」ボタン
  MENU_TOUCH_AREA_CANCEL_BUTTON, //「やめる」ボタン
  MENU_TOUCH_AREA_DUMMY,         // 終端コード
  MENU_TOUCH_AREA_NUM,           // 総数
} MENU_TOUCH_AREA_INDEX;

// タッチ範囲 ( 調査項目リスト )
typedef enum {
  TOPIC_TOUCH_AREA_TOPIC_0,       // 調査項目0
  TOPIC_TOUCH_AREA_TOPIC_1,       // 調査項目1
  TOPIC_TOUCH_AREA_TOPIC_2,       // 調査項目2
  TOPIC_TOUCH_AREA_TOPIC_3,       // 調査項目3
  TOPIC_TOUCH_AREA_TOPIC_4,       // 調査項目4
  TOPIC_TOUCH_AREA_TOPIC_5,       // 調査項目5
  TOPIC_TOUCH_AREA_TOPIC_6,       // 調査項目6
  TOPIC_TOUCH_AREA_TOPIC_7,       // 調査項目7
  TOPIC_TOUCH_AREA_TOPIC_8,       // 調査項目8
  TOPIC_TOUCH_AREA_TOPIC_9,       // 調査項目9
  TOPIC_TOUCH_AREA_DUMMY,         // 終端コード
  TOPIC_TOUCH_AREA_NUM,           // 総数
} TOPIC_TOUCH_AREA_INDEX;

// タッチ範囲 ( スクロールバー )
typedef enum {
  SCROLL_TOUCH_AREA_BAR,   // スクロールバー
  SCROLL_TOUCH_AREA_DUMMY, // 終端コード
  SCROLL_TOUCH_AREA_NUM,   // 総数
} SCROLL_TOUCH_AREA_INDEX;

// メッセージデータ
typedef enum {
  MESSAGE_STATIC,         // 定型文
  MESSAGE_QUESTIONNAIRE,  // アンケート関連
  MESSAGE_NUM,            // 総数
} MESSAGE_INDEX;

// BGFont オブジェクト
typedef enum {
  BG_FONT_TITLE,         // 上画面 タイトル
  BG_FONT_DIRECTION,     // 上画面 説明文
  BG_FONT_TOPIC_TITLE,   // 上画面 調査項目名
  BG_FONT_TOPIC_CAPTION, // 上画面 調査項目の補足文
  BG_FONT_QUESTION_1,    // 上画面 質問1
  BG_FONT_QUESTION_2,    // 上画面 質問2
  BG_FONT_QUESTION_3,    // 上画面 質問3
  BG_FONT_NUM,
} BG_FONT_INDEX;

// OBJ リソースID
typedef enum {
  OBJ_RESOURCE_MAIN_CHARACTER,  // MAIN-OBJ キャラクタ
  OBJ_RESOURCE_MAIN_PALETTE,    // MAIN-OBJ パレット
  OBJ_RESOURCE_MAIN_CELL_ANIME, // MAIN-OBJ セル アニメ
  OBJ_RESOURCE_MAIN_PALETTE_COMMON_BUTTON, // MAIN-OBJ パレット
  OBJ_RESOURCE_SUB_CHARACTER,   // SUB-OBJ  キャラクタ
  OBJ_RESOURCE_SUB_PALETTE,     // SUB-OBJ  パレット
  OBJ_RESOURCE_SUB_CELL_ANIME,  // SUB-OBJ  セル アニメ
  OBJ_RESOURCE_NUM,             // 総数
} OBJ_RESOURCE_ID;

// パレットアニメーション
typedef enum {
  PALETTE_ANIME_TOPIC_CURSOR_ON,   // 調査項目カーソルON
  PALETTE_ANIME_TOPIC_CURSOR_SET,  // 調査項目カーソルSET
  PALETTE_ANIME_TOPIC_SELECT,      // 調査項目選択リアクション
  PALETTE_ANIME_MENU_CURSOR_ON,    // メニュー項目カーソルON
  PALETTE_ANIME_MENU_CURSOR_SET,   // メニュー項目カーソルSET
  PALETTE_ANIME_MENU_SELECT,       // メニュー項目選択リアクション
  PALETTE_ANIME_NUM,               // 総数
} PALETTE_ANIME_INDEX;

// セルアクターユニット
typedef enum {
  CLUNIT_SUB_OBJ,  // SUB-OBJ
  CLUNIT_MAIN_OBJ, // MAIN-OBJ
  CLUNIT_BMPOAM,   // BMP-OAM アクターで使用
  CLUNIT_NUM,      // 総数
} CLUNIT_INDEX;

// セルアクターワーク
typedef enum {
  CLWK_SCROLL_CONTROL,// スクロールバーのつまみ
  CLWK_SCROLL_BAR,    // スクロールバー
  CLWK_SELECT_ICON_0, // 調査項目選択アイコン0
  CLWK_NUM,           // 総数
} CLWK_INDEX; 

// BMP-OAM
typedef enum {
  BMPOAM_ACTOR_CONFIRM,    // 確認メッセージ
  BMPOAM_ACTOR_OK,         //「けってい」
  BMPOAM_ACTOR_CANCEL,     //「やめる」
  BMPOAM_ACTOR_DETERMINE,  //「ちょうさを　かいしします！」
  BMPOAM_ACTOR_NUM,        // 総数
} BMPOAM_ACTOR_INDEX;
