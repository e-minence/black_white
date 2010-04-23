///////////////////////////////////////////////////////////////////////////////
// GameSpy ATLAS Competition System Header File
//
// NOTE: This is an auto-generated file, do not edit this file directly.
//  
// Atlasサーバから吐き出されるhとcです。
// 変更項目は以下
// ・DWC_GdbGetMyRecordsAsyncで使うため、使用キー文字列をテーブル化
// ・strcmpを使用していたのをGFL_STD_StrCmpへ変更
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ATLAS_SYACHI2DS_V1_H_
#define _ATLAS_SYACHI2DS_V1_H_

#ifdef __cplusplus
extern "C" {
#endif

//ランダムマッチ用
extern const char** ATLAS_RND_GetFieldName( void );
extern u32 ATLAS_RND_GetFieldNameNum( void );

//WIFI大会用
extern const char** ATLAS_WIFI_GetFieldName( void );
extern u32 ATLAS_WIFI_GetFieldNameNum( void );

extern int ATLAS_GET_KEY(char* keyName);
extern char* ATLAS_GET_KEY_NAME(int keyId);
extern int ATLAS_GET_STAT(char* statName);
extern char* ATLAS_GET_STAT_NAME(int statId);
extern int ATLAS_GET_STAT_PAGE_BY_ID(int statId);
extern int ATLAS_GET_STAT_PAGE_BY_NAME(char* statName);

#define ATLAS_RULE_SET_VERSION 1

// KEYS
// Use these key ID's to report match data for your game.

#define KEY_DISCONNECTS_COUNTER 26  // [TYPE: int]
#define KEY_ADD_DISCONNECTS_COUNTER 27  // [TYPE: int]
#define KEY_ADD_DISCONNECTS_WIFICUP_COUNTER 28  // [TYPE: int]
#define KEY_NUM_SINGLE_WIN_COUNTER 5  // [TYPE: int]
#define KEY_NUM_SINGLE_LOSE_COUNTER 6  // [TYPE: int]
#define KEY_NUM_DOUBLE_LOSE_COUNTER 7  // [TYPE: int]
#define KEY_NUM_DOUBLE_WIN_COUNTER 8  // [TYPE: int]
#define KEY_NUM_ROTATE_WIN_COUNTER 9  // [TYPE: int]
#define KEY_NUM_ROTATE_LOSE_COUNTER 10  // [TYPE: int]
#define KEY_NUM_TRIPLE_WIN_COUNTER 11  // [TYPE: int]
#define KEY_NUM_TRIPLE_LOSE_COUNTER 12  // [TYPE: int]
#define KEY_NUM_SHOOTER_WIN_COUNTER 13  // [TYPE: int]
#define KEY_NUM_SHOOTER_LOSE_COUNTER 14  // [TYPE: int]
#define KEY_ARENA_GAMETYPE_1V1_SINGLE 15  // [TYPE: byte]
#define KEY_ARENA_GAMETYPE_1V1_DOUBLE 16  // [TYPE: byte]
#define KEY_ARENA_GAMETYPE_1V1_ROTATE 17  // [TYPE: byte]
#define KEY_ARENA_GAMETYPE_1V1_TRIPLE 18  // [TYPE: byte]
#define KEY_ARENA_GAMETYPE_1V1_SHOOTER 19  // [TYPE: byte]
#define KEY_CHEATS_COUNTER 20  // [TYPE: int]
#define KEY_ARENA_GAMETYPE_1V1_WIFICUP 21  // [TYPE: byte]
#define KEY_NUM_WIFICUP_WIN_COUNTER 22  // [TYPE: int]
#define KEY_NUM_WIFICUP_LOSE_COUNTER 23  // [TYPE: int]
#define KEY_CHEATS_WIFICUP_COUNTER 24  // [TYPE: int]
#define KEY_DISCONNECTS_WIFICUP_COUNTER 25  // [TYPE: int]

///////////////////////////////////////////////////////////////////////////////

// STATS
// Use these stat ID's to query aggregate statistics for your game.

#define ARENA_ADVANCED_MODIFIER 30  // [TYPE: float]
#define ARENA_ELO_RATING_1V1_DOUBLE 20  // [TYPE: int]
#define ARENA_ELO_RATING_1V1_ROTATE 21  // [TYPE: int]
#define ARENA_ELO_RATING_1V1_SHOOTER 23  // [TYPE: int]
#define ARENA_ELO_RATING_1V1_SINGLE 19  // [TYPE: int]
#define ARENA_ELO_RATING_1V1_TRIPLE 22  // [TYPE: int]
#define ARENA_ELO_RATING_1V1_WIFICUP 36  // [TYPE: int]
#define ARENA_ELO_RATING_MAXIMUM_STAT 25  // [TYPE: int]
#define ARENA_ELO_RATING_MINIMUM_STAT 26  // [TYPE: int]
#define ARENA_FLOOR_MODIFIER 28  // [TYPE: float]
#define ARENA_INITIAL_RATING 35  // [TYPE: int]
#define ARENA_INTERMED_MODIFIER 29  // [TYPE: float]
#define ARENA_MASTER_MODIFIER 31  // [TYPE: float]
#define ARENA_RATING_DIFF_HIGH 32  // [TYPE: float]
#define ARENA_RATING_DIFF_LOW 33  // [TYPE: float]
#define ARENA_SCORE_WEIGHT_NORMAL 27  // [TYPE: int]
#define CHEATS_COUNTER 24  // [TYPE: int]
#define CHEATS_WIFICUP_COUNTER 39  // [TYPE: int]
#define COMPLETE_MATCHES_COUNTER 4  // [TYPE: int]
#define DISCONNECTS_COUNTER 3  // [TYPE: int]
#define DISCONNECTS_WIFICUP_COUNTER 40  // [TYPE: int]
#define NUM_DOUBLE_LOSE_COUNTER 11  // [TYPE: int]
#define NUM_DOUBLE_WIN_COUNTER 12  // [TYPE: int]
#define NUM_ROTATE_LOSE_COUNTER 14  // [TYPE: int]
#define NUM_ROTATE_WIN_COUNTER 13  // [TYPE: int]
#define NUM_SHOOTER_LOSE_COUNTER 18  // [TYPE: int]
#define NUM_SHOOTER_WIN_COUNTER 17  // [TYPE: int]
#define NUM_SINGLE_LOSE_COUNTER 10  // [TYPE: int]
#define NUM_SINGLE_WIN_COUNTER 9  // [TYPE: int]
#define NUM_TRIPLE_LOSE_COUNTER 16  // [TYPE: int]
#define NUM_TRIPLE_WIN_COUNTER 15  // [TYPE: int]
#define NUM_WIFICUP_LOSE_COUNTER 38  // [TYPE: int]
#define NUM_WIFICUP_WIN_COUNTER 37  // [TYPE: int]
#define NUM_WINLOSE_COUNTER_MINIMUM_STAT 34  // [TYPE: int]


#ifdef __cplusplus
}
#endif

#endif // _ATLAS_SYACHI2DS_V1_H_
