//==============================================================================
/**
 * @file    union_types.h
 * @brief   ユニオンルームで共通で使用する定義類
 * @author  matsuda
 * @date    2009.07.02(木)
 */
//==============================================================================
#pragma once

#include "buflen.h"
#include "app/trainer_card.h"


//==============================================================================
//  定数定義
//==============================================================================
///ビーコンデータが有効なものである事を示す数値
#define UNION_BEACON_VALID        (0x7a)

///ユニオン：ステータス
enum{
  UNION_STATUS_NORMAL,      ///<通常状態(何もしていない)
  UNION_STATUS_ENTER,       ///<ユニオンルームへ進入
  UNION_STATUS_LEAVE,       ///<ユニオンルームから退出
  
  UNION_STATUS_CONNECT_REQ,     ///<接続リクエスト実行中
  UNION_STATUS_CONNECT_ANSWER,  ///<接続リクエストを受けた側が返信として接続しにいっている状態
  UNION_STATUS_TALK_PARENT,     ///<接続確立後の会話(親)
  UNION_STATUS_TALK_CHILD,      ///<接続確立後の会話(子)
  
  UNION_STATUS_TRAINERCARD, ///<トレーナーカード
  UNION_STATUS_PICTURE,     ///<お絵かき
  UNION_STATUS_BATTLE,      ///<戦闘
  UNION_STATUS_TRADE,       ///<交換
  UNION_STATUS_GURUGURU,    ///<ぐるぐる交換
  UNION_STATUS_RECORD,      ///<レコードコーナー
  UNION_STATUS_SHUTDOWN,    ///<切断
  
  UNION_STATUS_CHAT,        ///<チャット編集中
  
  UNION_STATUS_MAX,
};

///ユニオン：アピール番号
enum{
  UNION_APPEAL_NULL,        ///<アピール無し
  
  UNION_APPEAL_BATTLE,      ///<戦闘
  UNION_APPEAL_TRADE,       ///<交換
  UNION_APPEAL_RECORD,      ///<レコードコーナー
  UNION_APPEAL_PICTURE,     ///<お絵かき
  UNION_APPEAL_GURUGURU,    ///<ぐるぐる交換
};

///ビーコン：バトル：戦闘モード
enum{
  UNION_BATTLE_MODE_SINGLE,     ///<1vs1 シングルバトル
  UNION_BATTLE_MODE_DOUBLE,     ///<1vs1 ダブルバトル
  UNION_BATTLE_MODE_TRIPLE,     ///<1vs1 トリプルバトル
  UNION_BATTLE_MODE_ROTATION,   ///<1vs1 ローテーションバトル
  
  UNION_BATTLE_MODE_MULTI,      ///<2vs2 マルチバトル
};

///ユニオンビーコン：受信状況
enum{
  UNION_BEACON_RECEIVE_NULL,    ///<ビーコン変化なし
  UNION_BEACON_RECEIVE_NEW,     ///<新規ビーコン
  UNION_BEACON_RECEIVE_UPDATE,  ///<ビーコン更新
};

///ユニオンルームのキャラクタ寿命(フレーム単位)
#define UNION_CHAR_LIFE         (30 * 15)  //フィールドの為、1/30

///同期用のタイミングコマンド番号
enum{
  UNION_TIMING_TRAINERCARD_PARAM = 1,   ///<トレーナーカードの情報交換前
  UNION_TIMING_TRAINERCARD_PROC_BEFORE, ///<トレーナーカード画面呼び出し前
  UNION_TIMING_TRAINERCARD_PROC_AFTER,  ///<トレーナーカード画面終了後
};

///サブPROC呼び出しID
typedef enum{
  UNION_SUBPROC_ID_NULL,              ///<サブPROC無し
  UNION_SUBPROC_ID_TRAINERCARD,       ///<トレーナーカード
}UNION_SUBPROC_ID;


//==============================================================================
//  構造体定義
//==============================================================================
///UNION_SYSTEM構造体の不定形ポインタ型
typedef struct _UNION_SYSTEM * UNION_SYSTEM_PTR;


//--------------------------------------------------------------
//  ビーコン
//--------------------------------------------------------------
///ビーコン：バトル
typedef struct{
  u8 battle_mode;           ///<戦闘モード(UNION_BATTLE_MODE_???)
  u8 regulation;            ///<レギュレーション番号　※check 構造体丸ごとの方がいいかも
  u8 padding[2];
}UNION_BEACON_BATTLE;

///ビーコン：チャット
typedef struct{
  u8 chat_no;
  u8 padding[3];
  u16 word[2];
}UNION_BEACON_CHAT;


///ユニオンで送受信するビーコンデータ
typedef struct{
  u8 connect_mac_address[6];  ///<接続したい人へのMacAddress

  u8 pm_version;              ///<PM_VERSION
  u8 language;                ///<PM_LANG
  u8 union_status;            ///<プレイヤーの状況(UNION_STATUS_???)
  u8 appeal_no;               ///<アピール番号(UNION_APPEAL_???)
  
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];  ///<名前
  
  u8 data_valid;              ///<UNION_BEACON_VALID:このビーコンデータは有効なものである
  u8 trainer_view;            ///<トレーナータイプ(ユニオンルーム内での見た目)
  u8 sex;                     ///<性別
  u8 padding;
  
  union{
    UNION_BEACON_CHAT chat;
    UNION_BEACON_BATTLE battle;
  };

  u8 reserve[16];             ///<将来の為の予約
}UNION_BEACON;

///受信したビーコンデータから作成されたPCパラメータ
typedef struct{
  UNION_BEACON beacon;
  u8 mac_address[6];          ///<送信相手のMacAddress
  u8 update_flag;             ///<ビーコンデータ受信状況(UNION_BEACON_???)
  u8 padding;
  
  u16 life;                    ///<寿命(フレーム単位)　新しくビーコンを受信しなければ消える
  u8 padding2[2];
  
  u8 event_status;            ///<イベントステータス(BPC_EVENT_STATUS_???)
  u8 next_event_status;       ///<次に実行するイベントステータス(BPC_EVENT_STATUS_???)
  u8 func_proc;               ///<動作プロセスNo(BPC_SUBPROC_???)
  u8 func_seq;                ///<動作プロセスシーケンス
}UNION_BEACON_PC;

//--------------------------------------------------------------
//  自分データ
//--------------------------------------------------------------
///トレーナーカード情報
typedef struct{
  TRCARD_CALL_PARAM *card_param;  ///<カード画面呼び出しようのParentWork
  TR_CARD_DATA *my_card;      ///<自分のカード情報(送信バッファ)
  TR_CARD_DATA *target_card;  ///<相手のカード情報(受信バッファ)
  u8 target_card_receive;     ///<TRUE:相手のカードを受信した
  u8 padding[3];
}UNION_TRCARD;

///送受信で変更するパラメータ類(自機がフリー動作の状態になるたびに初期化される)
typedef struct{
  u8 force_exit;              ///<TRUE:通信相手から強制切断を受信
  u8 mainmenu_select;         ///<メインメニューでの選択結果
  u8 mainmenu_yesno_result;   ///<「はい(TRUE)」「いいえ(FALSE)」選択結果
  u8 submenu_select;          ///<メインメニュー後のサブメニューの選択結果
  UNION_TRCARD trcard;        ///<トレーナーカード情報
}UNION_MY_COMM;

///ユニオンルーム内での自分の状況
typedef struct{
  //↓送信ビーコンに含めるデータ
  u8 union_status;            ///<プレイヤーの状況(UNION_STATUS_???)
  u8 appeal_no;               ///<アピール番号(UNION_APPEAL_???)
  u8 padding2[2];
  
  //↓構造体内の一部のデータのみを送信データに含める
  UNION_BEACON_PC *calling_pc; ///<接続して欲しい人のreceive_beaconへのポインタ
  UNION_MY_COMM mycomm;         ///<送受信で変更するパラメータ類
  
  //↓ここから下は通信では送らないデータ
  UNION_BEACON_PC *answer_pc;  ///<接続したい人のreceive_beaconへのポインタ
  UNION_BEACON_PC *connect_pc; ///<接続中の人のreceive_beaconへのポインタ
  s16 wait;
  u8 next_union_status;       ///<次に実行するプレイヤーの状況(UNION_STATUS_???)
  u8 func_proc;
  u8 func_seq;
  u8 padding[3];
}UNION_MY_SITUATION;

//--------------------------------------------------------------
//  システム
//--------------------------------------------------------------
typedef struct{
  UNION_SUBPROC_ID id;       ///<サブPROC呼び出しID
  void *parent_work;         ///<サブPROCに渡すParentWorkへのポインタ
}UNION_SUB_PROC;

