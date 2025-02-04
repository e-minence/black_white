//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_compatible_savedata.h
 *	@brief	赤外線相性チェック用セーブデータ
 *	@author	Toru=Nagihashi
 *	@date		2009.07.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include <strbuf.h>
#include "savedata/save_control.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
#define IRC_COMPATIBLE_SV_NEW_NONE		(127)		//新規ランクがいない
#define IRC_COMPATIBLE_SV_ADD_NONE		(0xFF)	//ランキングに入らなかった
#define IRC_COMPATIBLE_SV_RANKING_MAX	(30)		//ランキング最大数

#define IRC_COMPATIBLE_SV_DATA_SCORE_MAX		(100)		//スコア最大
#define IRC_COMPATIBLE_SV_DATA_PLAYCNT_MAX	(999)		//遊んだ回数最大

#define IRC_COMPATIBLE_SV_DATA_NAME_LEN			(PERSON_NAME_SIZE + EOM_SIZE)	//名前の長さ

#define IRC_COMPATIBLE_SEX_MALE     (1)
#define IRC_COMPATIBLE_SEX_FEMALE   (0)

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	相性チェック用セーブデータ
//=====================================
typedef struct _IRC_COMPATIBLE_SAVEDATA IRC_COMPATIBLE_SAVEDATA;
//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
///	セーブデータ取得取得
//=====================================
extern IRC_COMPATIBLE_SAVEDATA * IRC_COMPATIBLE_SV_GetSavedata( SAVE_CONTROL_WORK* p_sv_ctrl );

//-------------------------------------
///	データ取得関数
//=====================================
extern u32 IRC_COMPATIBLE_SV_GetRankNum( const IRC_COMPATIBLE_SAVEDATA *cp_sv );
extern const STRCODE* IRC_COMPATIBLE_SV_GetPlayerName( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u8	IRC_COMPATIBLE_SV_GetScore( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u16 IRC_COMPATIBLE_SV_GetPlayCount( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u32 IRC_COMPATIBLE_SV_GetSex( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u32 IRC_COMPATIBLE_SV_GetBirthMonth( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u32 IRC_COMPATIBLE_SV_GetBirthDay( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );

extern void IRC_COMPATIBLE_SV_GetPokeData( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank, u16 *p_mons_no, u8 *p_form_no, u8 *p_mons_sex, u8 *p_egg );

extern u8	IRC_COMPATIBLE_SV_GetNewRank( const IRC_COMPATIBLE_SAVEDATA *cp_sv );
extern u8 IRC_COMPATIBLE_SV_GetBioRhythm( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank, const RTCDate * cp_now_date );
extern u32 IRC_COMPATIBLE_SV_GetBestBioRhythm( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u8 player_birth_month, u8 player_birth_day, const RTCDate * cp_now_date );

extern BOOL IRC_COMPATIBLE_SV_IsDayFlag( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 trainerID );

extern BOOL IRC_COMPATIBLE_SV_IsPlayed( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 trainerID );

#ifdef PM_DEBUG
extern void IRC_COMPATIBLE_SV_SetCount( IRC_COMPATIBLE_SAVEDATA *p_sv, u32 rank, u32 play_cnt );
#endif //PM_DEBUG

//-------------------------------------
///	irc_compatible.cでのみ使うprivate関数
//=====================================
extern u8 Irc_Compatible_SV_CalcBioRhythm( u8 birth_month, u8 birth_day, const RTCDate * cp_now_date );

//-------------------------------------
///	データ設定関数
//=====================================
extern u8 IRC_COMPATIBLE_SV_AddRanking( IRC_COMPATIBLE_SAVEDATA *p_sv, const STRCODE *cp_name, u8 score, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID, u16 mons_no, u8 form_no, u8 mons_sex, u8 egg );

//-------------------------------------
///	１日１回フラグ消去
//=====================================
extern void IRC_COMPATIBLE_SV_ClearDayFlag( IRC_COMPATIBLE_SAVEDATA *p_sv );

//-------------------------------------
///	セーブデータシステムで使用する関数
//=====================================
extern int IRC_COMPATIBLE_SV_GetWorkSize( void );
extern void IRC_COMPATIBLE_SV_Init( IRC_COMPATIBLE_SAVEDATA *p_sv );

