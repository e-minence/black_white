
//============================================================================================
/**
 * @file	  dreamworld_netdata.h
 * @brief	  ポケモンドリームワールドに必要なデータ
 * @author	k.ohno
 * @date	  2009.11.08
 */
//============================================================================================
#pragma once

#include "savedata/dreamworld_data.h"

#define DREAM_WORLD_SERVER_POKMEONLIST_MAX (1024)  //寝るポケモンのリスト最大


typedef enum{
  DREAM_WORLD_SERVER_ERROR_NONE,        // 正常です
  DREAM_WORLD_SERVER_ERROR_SERVER_FULL,	// サーバーの容量オーバーです
  DREAM_WORLD_SERVER_ALREADY_EXISTS,	  // サーバーに既にデータがあります
  DREAM_WORLD_SERVER_ILLEGAL_DATA,		// アップロードされたデータが壊れています
  DREAM_WORLD_SERVER_CHEAT_DATA,		  // アップロードされたデータが不正です
  DREAM_WORLD_SERVER_NO_DATA,		      // サーバにデータがありません
  DREAM_WORLD_SERVER_END,             // サービスが終了している
  DREAM_WORLD_SERVER_ILLEGAL_TOKEN,   // 認証トークンに問題がある
  DREAM_WORLD_SERVER_NO_ACCOUNT,      // アカウントがありません
  DREAM_WORLD_SERVER_ERROR_ETC,       // その他サーバがエラーの場合 これ以上の値が来ても同様にエラーにする
  DREAM_WORLD_SERVER_ERROR_MAX
} DREAM_WORLD_SERVER_ERROR_TYPE;   /// サーバから返ってくるサーバの状態

typedef enum{
  DREAM_WORLD_SERVER_PLAY_EMPTY,   ///< ポケモンを預けていない  == 大好きクラブ未登録
  DREAM_WORLD_SERVER_PLAY_NONE,    ///< まだ遊んでいません  == 大好きクラブ登録完了+セーブデータアップロード
  DREAM_WORLD_SERVER_PLAY_ACTIVE,  ///< 遊んでいる最中です  == PDWで夢みる遊びを開始
  DREAM_WORLD_SERVER_PLAY_END,     ///< ドリームワールドで遊びました == 夢を見終わった
  DREAM_WORLD_SERVER_PLAY_FINISH,  ///< サービスが終了しています == PDW終了時 サーバは稼動
  DREAM_WORLD_SERVER_PLAY_MAX
} DREAM_WORLD_SERVER_PLAY_TYPE;   /// サーバから返ってくる遊びに関しての情報



/// 状態確認構造体
typedef struct {
  u16 PlayStatus; // DREAM_WORLD_SERVER_PLAY_TYPE
  ///<  ここからはLV1ご褒美の為の実装
  u16 findPokemon;         ///< であったポケモン番号
  u16 findPokemonTecnique; ///< であったポケモンに覚えさせる技
  u8 findPokemonForm;      ///< であったポケモンのフォルム番号
  u8 findPokemonSex;       ///< であったポケモン性別    0=MALE 1=FEMALE 2=NEUTER 3=RANDOM
  u16 itemID[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< 持ち帰ったアイテム
  u8 itemNum[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< 持ち帰ったアイテムの個数
}DREAM_WORLD_SERVER_STATUS_DATA;



/// アカウント作成構造体
struct DREAM_WORLD_SERVER_CREATE_DATA{
  u8 GameSyncID[12];   /// GameSyncID
};

#define DOWNLOADPOKE_MORE_VER (1)   //ダウンロードポケモンを増やす場合１

#if DOWNLOADPOKE_MORE_VER  //新しくする構造体

#define DREAM_WORLD_SERVER_DOWNLOADPOKE_MAX (10)   //ダウンロードできるポケモン最大数

/// ダウンロードポケモン構造体
typedef struct {
  u16 findPokemon;         ///< であったポケモン番号
  u16 findPokemonTecnique; ///< であったポケモンに覚えさせる技
  u8 findPokemonForm;      ///< であったポケモンのフォルム番号
  u8 findPokemonSex;       ///< であったポケモン性別    0=MALE 1=FEMALE 2=NEUTER 3=RANDOM
  u8 findPokemonAct;  ///< であったポケモンの動き
  u8 dummy;
}DREAM_WORLD_SERVER_DOWNLOADPOKE_DATA;


/// ダウンロードデータ構造体
typedef struct {
  u32 uploadCount;     ///< アップロード回数
  DREAM_WORLD_SERVER_DOWNLOADPOKE_DATA poke[DREAM_WORLD_SERVER_DOWNLOADPOKE_MAX];  ///< であったポケモン
  u16 sleepPokemonLv;   ///< 寝ていたポケモンのレベルアップ値
  u8 sleepPokemonState;  ///< 寝ているポケモンの状態 // DREAM_WORLD_SERVER_PLAY_TYPEと同じ
  u8 musicalNo;      ///< ミュージカル演目 webで選択した番号  無い場合 0xff
  u8 cgearNo;        ///< CGEAR画像   webで選択した番号  無い場合 0xff
  u8 zukanNo;        ///< 図鑑画像    webで選択した番号  無い場合 0xff
  u8 signin;        ///<	大好きクラブで登録が終わったかどうか  登録済みなら 1
  u8 bGet;          ///<	このデータをすでに受け取ったかどうか 
  u16 itemID[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< 持ち帰ったアイテム
  u8 itemNum[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< 持ち帰ったアイテムの個数
  DREAM_WORLD_FURNITUREDATA Furniture[DREAM_WORLD_DATA_MAX_FURNITURE];  ///<	26*5=130  配信家具情報
  u16 CRC;  /// このデータのCRC-16/CCITT
} DREAM_WORLD_SERVER_DOWNLOAD_DATA;


#else

#define DREAM_WORLD_SERVER_DOWNLOADPOKE_MAX (1)   //ダウンロードできるポケモン最大数

/// ダウンロードデータ構造体
typedef struct {
  u32 uploadCount;     ///< アップロード回数
  u16 sleepPokemonLv;   ///< 寝ていたポケモンのレベルアップ値
  u16 findPokemon;         ///< であったポケモン番号
  u16 findPokemonTecnique; ///< であったポケモンに覚えさせる技
  u8 findPokemonForm;      ///< であったポケモンのフォルム番号
  u8 findPokemonSex;       ///< であったポケモン性別    0=MALE 1=FEMALE 2=NEUTER 3=RANDOM
  u8 sleepPokemonState;  ///< 寝ているポケモンの状態 // DREAM_WORLD_SERVER_PLAY_TYPEと同じ
  u8 musicalNo;      ///< ミュージカル演目 webで選択した番号  無い場合 0xff
  u8 cgearNo;        ///< CGEAR画像   webで選択した番号  無い場合 0xff
  u8 zukanNo;        ///< 図鑑画像    webで選択した番号  無い場合 0xff
  u8 signin;        ///<	大好きクラブで登録が終わったかどうか  登録済みなら 1
  u8 bGet;          ///<	このデータをすでに受け取ったかどうか 
  u16 itemID[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< 持ち帰ったアイテム
  u8 itemNum[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< 持ち帰ったアイテムの個数
  DREAM_WORLD_FURNITUREDATA Furniture[DREAM_WORLD_DATA_MAX_FURNITURE];  ///<	26*5=130  配信家具情報
  u8 findPokemonAct;  ///< であったポケモンの動き
  u8 dummy;
  u16 CRC;  /// このデータのCRC-16/CCITT
} DREAM_WORLD_SERVER_DOWNLOAD_DATA;



#endif



/// ダウンロード完了をPDWサーバに知らせる構造体
typedef struct {
  u8 bGet;          ///<	このデータをすでに受け取ったかどうか
  u8 dummy;
  u8 dummy2;
  u8 dummy3;
} DREAM_WORLD_SERVER_DOWNLOAD_FINISH_DATA;



/// ポケモンリストデータ構造体
typedef struct {
  u8 pokemonList[DREAM_WORLD_SERVER_POKMEONLIST_MAX/8];  ///< 寝かせてよいポケモンBITLIST
} DREAM_WORLD_SERVER_POKEMONLIST_DATA;



typedef enum{
  DREAM_WORLD_MATCHUP_NONE,     // 登録していない状態
  DREAM_WORLD_MATCHUP_SIGNUP,	  // web登録した。DS->SAKEの接続はまだ
  DREAM_WORLD_MATCHUP_ENTRY,	  // 参加中
  DREAM_WORLD_MATCHUP_END,	  // 大会終了
  DREAM_WORLD_MATCHUP_RETIRE,		 // 途中で解除

  DREAM_WORLD_MATCHUP_CHANGE_DS,		// DS本体を差し替えた
} DREAM_WORLD_MATCHUP_TYPE;   /// 世界対戦の進行状況

//下記構造体に書き込まれる、書き込みフラグ
typedef enum
{ 
  DREAM_WORLD_ENTRYFLAG_DS_WRITE,
  DREAM_WORLD_ENTRYFLAG_GPF_WRITE,
} DREAM_WORLD_ENTRYFLAG;

/// 世界対戦確認用データ構造体
typedef struct {
  u32 WifiMatchUpID;   ///< 世界対戦の開催ID
  u8 GPFEntryFlg;      ///< GPFから書き込んだらDREAM_WORLD_ENTRYFLAG_GPF_WRITE DSから書き込んだらDREAM_WORLD_ENTRYFLAG_DS_WRITE
  u8 WifiMatchUpState; ///< 世界対戦の進行状況  //DREAM_WORLD_MATCHUP_TYPE
  u8 signin;        ///<	大好きクラブで登録が終わったかどうか  登録済みなら 1
  u8 dummy;
} DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA;


/// 世界対戦書き込みデータ構造体
typedef struct {
  u8 GPFEntryFlg;      ///< GPFから書き込んだらDREAM_WORLD_ENTRYFLAG_GPF_WRITE DSから書き込んだらDREAM_WORLD_ENTRYFLAG_DS_WRITE
  u8 WifiMatchUpState; ///< 世界対戦の進行状況
  u8 dummy1;
  u8 dummy2;
#ifdef BUGFIX_GFBTS1976_20100714
  u32 profileID;      ///< プロファイルID
#endif //BUGFIX_GFBTS1976_20100714
} DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA;


