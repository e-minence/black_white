//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_compatible_savedata.c
 *	@brief	赤外線相性チェック用セーブデータ
 *	@author	Toru=Nagihashi
 *	@data		2009.07.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "system/main.h"
#include "print/str_tool.h"
#include "buflen.h"
#include "system/gfl_use.h"
#include "savedata/save_tbl.h"

#include "savedata/irc_compatible_savedata.h"

#include "system/rtc_tool.h"  //バイオリズムのため

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
#define IRC_COMPATIBLE_SV_EXIST_NONE		(0xFF)		//ランクにいない

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	相性チェックランキング
//=====================================
typedef struct 
{
	STRCODE name[IRC_COMPATIBLE_SV_DATA_NAME_LEN];		// 16
	u32			score			:   7;													//	4	
	u32			play_cnt	:   10;
  u32     sex       :   1;
  u32     birth_month:  4;
  u32     birth_day :   4;
	u32			dummy			:   5;
  u32     is_use    :   1;
	u32			trainerID;														// 4
  u16     mons_no;
  u8      form_no;
  u8      mons_sex  : 6;
  u8      egg_flag  : 1;
  u8      only_day  : 1;
}IRC_COMPATIBLE_RANKING_DATA;

//-------------------------------------
///	相性チェック用セーブデータ
//=====================================
struct _IRC_COMPATIBLE_SAVEDATA
{	
	IRC_COMPATIBLE_RANKING_DATA	rank[IRC_COMPATIBLE_SV_RANKING_MAX];
	u32	new_rank		:8;
  u32 dummy       :24;
};
//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
static u8 Irc_Compatible_SV_InsertRanking( IRC_COMPATIBLE_RANKING_DATA *p_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new );
static BOOL Irc_Compatible_SV_IsExits( const IRC_COMPATIBLE_RANKING_DATA *cp_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new );
static void Irc_Compatible_SV_SetData( IRC_COMPATIBLE_RANKING_DATA *p_data, const STRCODE *cp_name, u8 score, u16 play_cnt, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID, u16 mons_no, u8 form_no, u8 mons_sex, u8 egg );
static const IRC_COMPATIBLE_RANKING_DATA * Irc_Compatible_SV_GetRankDataForTrainerID( const IRC_COMPATIBLE_RANKING_DATA *cp_rank, u16 len, u32 trainerID );
static void Irc_Compatible_SV_AddPlayCount( IRC_COMPATIBLE_RANKING_DATA *p_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new );
//=============================================================================
/**
 *	セーブデータ取得取得
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	セーブデータ取得
 *
 *	@param	SAVE_CONTROL_WORK* p_sv_ctrl	セーブControl
 *
 *	@return	相性チェック用せーぶでーた
 */
//-----------------------------------------------------------------------------
IRC_COMPATIBLE_SAVEDATA * IRC_COMPATIBLE_SV_GetSavedata( SAVE_CONTROL_WORK* p_sv_ctrl )
{	
	return SaveControl_DataPtrGet( p_sv_ctrl, GMDATA_ID_IRCCOMPATIBLE );
}

//=============================================================================
/**
 *	データ取得関数
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ランクインしている人の数を取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	セーブ
 *
 *	@return	ランクイン数
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetRankNum( const IRC_COMPATIBLE_SAVEDATA *cp_sv )
{	
	int i;

	//ランクインの数はセーブしておらず、使用しているかを見て計算する
	for( i = 0; i < IRC_COMPATIBLE_SV_RANKING_MAX; i++ )
	{	
		if( cp_sv->rank[i].is_use == 0 )
		{	
			break;
		}
	}

	return i;
}
//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー名を取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	セーブ
 *	@param	rank																	順位
 *	@retval	STRCODE
 */
//-----------------------------------------------------------------------------
const STRCODE* IRC_COMPATIBLE_SV_GetPlayerName( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{	
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
  return cp_sv->rank[ rank ].name;
}
//----------------------------------------------------------------------------
/**
 *	@brief	スコアを取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	セーブ
 *	@param	rank																	順位
 *
 *	@return	スコア
 */
//-----------------------------------------------------------------------------
u8	IRC_COMPATIBLE_SV_GetScore( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{	
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].score;
}
//----------------------------------------------------------------------------
/**
 *	@brief	プレイ回数を取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	セーブ
 *	@param	rank																	順位
 *
 *	@return	プレイ回数
 */
//-----------------------------------------------------------------------------
u16 IRC_COMPATIBLE_SV_GetPlayCount( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{	
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].play_cnt;
}
//----------------------------------------------------------------------------
/**
 *	@brief  性別取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  セーブ
 *	@param  rank                                  順位
 *
 *	@return 性別
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetSex( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{ 
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].sex;
}
//----------------------------------------------------------------------------
/**
 *	@brief  誕生月取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  セーブ
 *	@param  rank                                  順位
 *
 *	@return 誕生月
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetBirthMonth( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{ 
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].birth_month;
}
//----------------------------------------------------------------------------
/**
 *	@brief  誕生日取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  セーブ
 *	@param  rank                                  順位
 *
 *	@return 誕生日
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetBirthDay( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{ 
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].birth_day;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ポケモン情報を取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  セーブ
 *	@param	rank                                  順位
 *	@param	*p_mons_no          モンスター番号
 *	@param	*p_form_no          フォルム番号
 *	@param	*p_mons_sex         性別
 *	@param	*p_egg              タマゴフラグ
 */
//-----------------------------------------------------------------------------
void IRC_COMPATIBLE_SV_GetPokeData( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank, u16 *p_mons_no, u8 *p_form_no, u8 *p_mons_sex, u8 *p_egg )
{ 
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	*p_mons_no  = cp_sv->rank[ rank ].mons_no;
  *p_form_no  = cp_sv->rank[ rank ].form_no;
  *p_mons_sex = cp_sv->rank[ rank ].mons_sex;
  *p_egg      = cp_sv->rank[ rank ].egg_flag;
}
//----------------------------------------------------------------------------
/**
 *	@brief	新規を取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	セーブ
 *
 *	@return	新規順位
 */
//-----------------------------------------------------------------------------
u8	IRC_COMPATIBLE_SV_GetNewRank( const IRC_COMPATIBLE_SAVEDATA *cp_sv )
{	
	return cp_sv->new_rank;
}
//----------------------------------------------------------------------------
/**
 *	@brief  バイオリズムを取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  セーブ
 *	@param	rank                                  順位
 *	@param  cp_now_date Irc_Compatible_SV_CalcBioRhythmの説明を参照
 *
 *	@return 0〜100の値
 */
//-----------------------------------------------------------------------------
u8 IRC_COMPATIBLE_SV_GetBioRhythm( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank, const RTCDate * cp_now_date )
{ 
  u8 month, day;

  GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );

  month  = IRC_COMPATIBLE_SV_GetBirthMonth( cp_sv, rank );
  day    = IRC_COMPATIBLE_SV_GetBirthDay( cp_sv, rank ); 
  return Irc_Compatible_SV_CalcBioRhythm( month, day, cp_now_date );
}
//----------------------------------------------------------------------------
/**
 *	@brief  一番自分とバイオリズムが合っている人を取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  セーブ
 *	@param	player_birth_month                    プレイヤーの誕生月
 *	@param	player_birth_day                      プレイヤーの誕生日
 *	@param  cp_now_date Irc_Compatible_SV_CalcBioRhythmの説明を参照
 *
 *	@return 一番自分とバイオリズムが合っている人の順位
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetBestBioRhythm( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u8 player_birth_month, u8 player_birth_day, const RTCDate * cp_now_date )
{ 
  const u32 max         = IRC_COMPATIBLE_SV_GetRankNum( cp_sv );
  const u32 player_bio  = Irc_Compatible_SV_CalcBioRhythm( player_birth_month, player_birth_day, cp_now_date );

  int i;
  u32 best_idx  = 0;
  u32 best_dif  = 0xFFFFFFFF;
  u32 dif;

  for( i = 0; i < max; i++ )
  { 
    dif = MATH_IAbs( (s32)IRC_COMPATIBLE_SV_GetScore( cp_sv, i ) - (s32)player_bio );

    //同じだったらランダムで決定
    if( best_dif == dif )
    {
      u32 temp_array[2];
      temp_array[0] = best_idx;
      temp_array[1] = i;
      best_idx  = temp_array[ GFUser_GetPublicRand( NELEMS(temp_array) ) ];

      NAGI_Printf( "%dと%dのランダムで%dになった\n", temp_array[0], temp_array[1], best_idx );
    }
    //違う場合はより近い方
    else if( best_dif > dif )
    { 

      NAGI_Printf( "差が[%d]=%dと[%d]=%dだったので",  i, dif, best_idx, best_dif );

      best_idx  = i;
      best_dif  = dif;

      NAGI_Printf( "%dになった\n",  best_idx );
    }
  }

  return best_idx;
}
//----------------------------------------------------------------------------
/**
 *	@brief  １日１回フラグを取得
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  ワーク
 *	@param	trainerID                             チェックするトレーナーID
 *
 *	@return TRUEならば今日一度プレイしている  FALSEならば本日始めてプレイ
 */
//-----------------------------------------------------------------------------
BOOL IRC_COMPATIBLE_SV_IsDayFlag( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 trainerID )
{ 
  int i;

	for( i = 0; i < IRC_COMPATIBLE_SV_RANKING_MAX; i++ )
	{	
		if( cp_sv->rank[i].trainerID == trainerID &&
        cp_sv->rank[i].is_use == 1 )
		{
		  return cp_sv->rank[i].only_day;
		}
	}
	return FALSE;

}
//----------------------------------------------------------------------------
/**
 *	@brief  プレイしたことがあるかチェック
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  ワーク
 *	@param	trainerID                           トレーナーID
 *
 *	@return TRUE１回でもプレイしたことがある  FALSEない
 */
//-----------------------------------------------------------------------------
BOOL IRC_COMPATIBLE_SV_IsPlayed( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 trainerID )
{ 
  int i;

	for( i = 0; i < IRC_COMPATIBLE_SV_RANKING_MAX; i++ )
	{	
		if( cp_sv->rank[i].trainerID == trainerID &&
        cp_sv->rank[i].is_use == 1 )
		{
		  return TRUE;
		}
	}
	return FALSE;
}


#ifdef PM_DEBUG
//----------------------------------------------------------------------------
/**
 *	@brief  行った回数を増やす
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_sv セーブデータ
 *	@param	rank  ランク数
 */
//-----------------------------------------------------------------------------
void IRC_COMPATIBLE_SV_SetCount( IRC_COMPATIBLE_SAVEDATA *p_sv, u32 rank, u32 play_cnt )
{
	int i;

	for( i = 0; i < IRC_COMPATIBLE_SV_RANKING_MAX; i++ )
	{	
    if( p_sv->rank[i].is_use && p_sv->rank[rank].is_use )
    {
      if( p_sv->rank[i].trainerID == p_sv->rank[rank].trainerID )
      {
        p_sv->rank[i].play_cnt  = MATH_CLAMP( play_cnt, 0, IRC_COMPATIBLE_SV_DATA_PLAYCNT_MAX );
      }
    }
	}
}
#endif //PM_DEBUG

//----------------------------------------------------------------------------
/**
 *	@brief  バイオリズムを計算
 *
 *	@param	month 月
 *	@param	day   日
 *	@param  cp_now_date イベント用時刻とDS内時刻の両方を渡せるように
 *	                    イベント内で使うときはev_time.hのRTCを
 *	                    イベント外で使うときはGFL_RTC_GetDateを外で使用して渡す
 *
 *	@return 0〜100の値
 */
//-----------------------------------------------------------------------------
u8 Irc_Compatible_SV_CalcBioRhythm( u8 birth_month, u8 birth_day, const RTCDate * cp_now_date )
{ 
  enum
  { 
    BIORHYTHM_CYCLE = 30,  //周期
  };
 
  u32 days;
  u32 now_days; //今日の日付を日数に
  u32 days_diff;
  fx32 sin;
  u32 bio;

  //今日までの総日数を計算（年が取れないので、一年だけとする）
  { 
    RTCDate date  = *cp_now_date;
    now_days  = GFL_RTC_GetDaysOffset(&date);
    OS_TFPrintf( 0, "現在の日時　%d年%d月%d日=[%d]\n", date.year, date.month, date.day, now_days );

    date.month  = birth_month;
    date.day    = birth_day;
    days        = GFL_RTC_GetDaysOffset(&date);
    OS_TFPrintf( 0, "自分の誕生日　%d年%d月%d日=[%d]\n", date.year, date.month, date.day, days );
  }

  //誕生日から今日まで何日かかっているか
  if( now_days >= days  )
  { 
    days  += 365;
  }
  days_diff = days - now_days;

  days_diff %= BIORHYTHM_CYCLE;

  sin = FX_SinIdx( 0xFFFF * days_diff / BIORHYTHM_CYCLE );

  bio = ((sin + FX32_ONE) * 50 ) >> FX32_SHIFT;


  OS_TFPrintf( 0, "バイオリズム %d 誕生経過%d 現在%d,差%d\n", bio, days, now_days, days_diff );

  return bio;
}


//=============================================================================
/**
 *	データ設定関数
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ランキングにデータをセット
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_sv	セーブ
 *	@param	STRCODE *cp_name							名前
 *	@param	score													点数（０〜１００）
 *	@param	trainerID											ID
 *
 *	@return	順位	or ランキングに入らなかった場合IRC_COMPATIBLE_SV_ADD_NONE
 */
//-----------------------------------------------------------------------------
u8 IRC_COMPATIBLE_SV_AddRanking( IRC_COMPATIBLE_SAVEDATA *p_sv, const STRCODE *cp_name, u8 score, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID, u16 mons_no, u8 form_no, u8 mons_sex, u8 egg )
{	
	IRC_COMPATIBLE_RANKING_DATA	new_data;
	u16   play_cnt  = 0;  //初期回数は０
  BOOL  only_day  = 1;  //始めての場合は１
  const IRC_COMPATIBLE_RANKING_DATA *cp_pre_data;

	//回数取得
  cp_pre_data = Irc_Compatible_SV_GetRankDataForTrainerID( p_sv->rank, IRC_COMPATIBLE_SV_RANKING_MAX, trainerID );
  if( cp_pre_data )
  {
    play_cnt  = cp_pre_data->play_cnt;
    only_day  = cp_pre_data->only_day;
  }

	//データ作成
	Irc_Compatible_SV_SetData( &new_data, cp_name, score, play_cnt, sex, birth_month, birth_day, trainerID, mons_no, form_no, mons_sex, egg );

	//データ挿入
	p_sv->new_rank	= Irc_Compatible_SV_InsertRanking( p_sv->rank, IRC_COMPATIBLE_SV_RANKING_MAX, &new_data );

	//データ内にnewと同じものがあったら、カウントアップ
	//（上記でいれたデータもカウントアップされます）
	if( Irc_Compatible_SV_IsExits( p_sv->rank, IRC_COMPATIBLE_SV_RANKING_MAX, &new_data ) )
	{	
		Irc_Compatible_SV_AddPlayCount( p_sv->rank, IRC_COMPATIBLE_SV_RANKING_MAX, &new_data );
	}

	return p_sv->new_rank;
}

//----------------------------------------------------------------------------
/**
 *	@brief  一日一回フラグを消去
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_sv ワーク
 */
//-----------------------------------------------------------------------------
void IRC_COMPATIBLE_SV_ClearDayFlag( IRC_COMPATIBLE_SAVEDATA *p_sv )
{ 
	int i;

	for( i = 0; i < IRC_COMPATIBLE_SV_RANKING_MAX; i++ )
	{	
	  p_sv->rank[i].only_day  = 0;
	}
}

//=============================================================================
/**
 *	セーブデータシステムで使用する関数
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief	セーブデータ領域のサイズ
 *
 *	@param	void 
 *
 *	@return	サイズ
 */
//-----------------------------------------------------------------------------
int IRC_COMPATIBLE_SV_GetWorkSize( void )
{	
	return sizeof(IRC_COMPATIBLE_SAVEDATA);
}
//----------------------------------------------------------------------------
/**
 *	@brief	セーブデータ初期化
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_sv		ワーク
 *
 */
//-----------------------------------------------------------------------------
void IRC_COMPATIBLE_SV_Init( IRC_COMPATIBLE_SAVEDATA *p_sv )
{	
	GFL_STD_MemClear( p_sv, sizeof(IRC_COMPATIBLE_SAVEDATA) );
	p_sv->new_rank	= IRC_COMPATIBLE_SV_NEW_NONE;
}
//=============================================================================
/**
 *	PRIVATE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ランキングデータを挿入する
 *
 *	@param	IRC_COMPATIBLE_RANKING_DATA *p_rank		挿入するランキング配列
 *	@param	len																要素数
 *	@param	IRC_COMPATIBLE_RANKING_DATA *cp_new　	挿入するデータ
 *
 *	@return	挿入した場所インデックス
 */
//-----------------------------------------------------------------------------
static u8 Irc_Compatible_SV_InsertRanking( IRC_COMPATIBLE_RANKING_DATA *p_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new )
{
	int i, j;
	int insert	= -1;
	IRC_COMPATIBLE_SAVEDATA temp;

	//挿入箇所をチェック
	for( i = 0; i < len; i++ )
	{	
		if( p_rank[i].is_use == FALSE || p_rank[i].score <= cp_new->score )
		{	
			insert	= i;
			break;
		}
	}

	//挿入できないチェック
	if( insert == -1 )
	{	
		return IRC_COMPATIBLE_SV_ADD_NONE;
	}

	//後ろから1個ずつずらす
	for( j = len - 1; j > insert ; j-- )
	{	
		//処理軽減のために両方の移動先がplay_cntならば（データなしとみなして、行わない）
		if( p_rank[j].play_cnt != 0 ||  p_rank[j-1].play_cnt != 0 )
		{	
			GFL_STD_MemCopy( &p_rank[j-1], &p_rank[j], sizeof(IRC_COMPATIBLE_RANKING_DATA) );
		}
	}

	//挿入
	GFL_STD_MemCopy( cp_new, &p_rank[insert], sizeof(IRC_COMPATIBLE_RANKING_DATA) );
	return insert;
}

//----------------------------------------------------------------------------
/**
 *	@brief	同じ人がもう一度やったかチェックする
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_rank	チェックするランキング配列
 *	@param	len																			要素数
 *	@param	IRC_COMPATIBLE_SAVEDATA *cp_new					チェックするデータ
 *
 *	@return	順位インデックス　または いないならばIRC_COMPATIBLE_SV_EXIST_NONE
 */
//-----------------------------------------------------------------------------
static BOOL Irc_Compatible_SV_IsExits( const IRC_COMPATIBLE_RANKING_DATA *cp_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new )
{	
	int i;

	for( i = 0; i < len; i++ )
	{	
		if( cp_rank[i].trainerID == cp_new->trainerID &&
        cp_rank[i].is_use == 1 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	データを設定
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_data	ワーク
 *	@param	STRCODE *cp_name								名前
 *	@param	score														得点
 *	@param  sex                             性別  1が男 0が女
 *	@param	trainerID												ID
 *
 */
//-----------------------------------------------------------------------------
static void Irc_Compatible_SV_SetData( IRC_COMPATIBLE_RANKING_DATA *p_data, const STRCODE *cp_name, u8 score, u16 play_cnt, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID, u16 mons_no, u8 form_no, u8 mons_sex, u8 egg )
{	
	GF_ASSERT( score <= IRC_COMPATIBLE_SV_DATA_SCORE_MAX );

	//クリア
	GFL_STD_MemClear( p_data, sizeof(IRC_COMPATIBLE_RANKING_DATA) );
	//初期化
  STRTOOL_Copy( cp_name, p_data->name, IRC_COMPATIBLE_SV_DATA_NAME_LEN );
	p_data->score			  = score;
	p_data->trainerID	  = trainerID;
  p_data->sex         = sex;
  p_data->birth_month = birth_month;
  p_data->birth_day   = birth_day;
	p_data->play_cnt	  = play_cnt;
  p_data->mons_no     = mons_no;
  p_data->form_no     = form_no;
  p_data->mons_sex    = mons_sex;
  p_data->egg_flag    = egg & 0x1;
  p_data->is_use      = 1;
  p_data->only_day    = 1;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーIDからランキングデータを取得する
 *
 *	@param	const IRC_COMPATIBLE_RANKING_DATA *cp_rank	ワーク
 *	@param	len																					長さ
 *	@param	trainerID																		ID
 *
 *	@retval	ランキングデータ
 *
 */
//-----------------------------------------------------------------------------
static const IRC_COMPATIBLE_RANKING_DATA * Irc_Compatible_SV_GetRankDataForTrainerID( const IRC_COMPATIBLE_RANKING_DATA *cp_rank, u16 len, u32 trainerID )
{	
	int i;

	for( i = 0; i < len; i++ )
	{	
		if( cp_rank[i].trainerID == trainerID &&
        cp_rank[i].is_use == 1 )
		{
			return &cp_rank[i];
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NEWと同じIDの人のランキングを増やす
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_rank		ランキング配列
 *	@param	len																要素数
 *	@param	IRC_COMPATIBLE_SAVEDATA *cp_new		増やすデータ
 *
 */
//-----------------------------------------------------------------------------
static void Irc_Compatible_SV_AddPlayCount( IRC_COMPATIBLE_RANKING_DATA *p_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new )
{	
	int i;

	for( i = 0; i < len; i++ )
	{	
		if( p_rank[i].trainerID == cp_new->trainerID )
		{
			p_rank[i].play_cnt++;
      p_rank[i].only_day  = 1;
			p_rank[i].play_cnt	= MATH_CLAMP( p_rank[i].play_cnt, 0, IRC_COMPATIBLE_SV_DATA_PLAYCNT_MAX );
		}
	}
}
