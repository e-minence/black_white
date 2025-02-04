//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		misc.c
 *	@brief	未分類セーブデータアクセス
 *	@author	Toru=Nagihashi
 *	@data		2009.10.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "savedata/misc.h"
#include "savedata/save_tbl.h"

#include "app/name_input.h" //NAMEIN_POKEMONのため
#include "field/research_team_def.h"
#include "../../../resource/research_radar/data/question_id.h" // for QUESTION_ID_xxxx

#include "system/main.h"             // for HEAPID_xxxx
#include "arc/arc_def.h"             // for ARCID_xxxx
#include "arc/script_message.naix"   // for NARC_xxxx
#include "msg/script/msg_c03r0101.h" // for msg_xxxx

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
#include "misc_local.h"

//=============================================================================
/**
 *	プロトタイプ
 */
//=============================================================================
//-------------------------------------
///	名前入力
//=====================================
static void MISC_InitNameIn( MISC *p_misc );



#ifdef PM_DEBUG
//デバッグ用クイズ番号
int DebugQuizNo = 0;
//デバッグテレビ用番組番号
u32 DebugTvNo = 0;
#endif

//=============================================================================
/**
 *	セーブデータシステムに依存する関数
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MISC構造体のサイズ取得
 *
 *	@return	MISC構造体のサイズ
 */
//-----------------------------------------------------------------------------
int MISC_GetWorkSize( void )
{	
	return sizeof(MISC);
}
//----------------------------------------------------------------------------
/**
 *	@brief	MISC構造体初期化
 *
 *	@param	MISC *p_msc ワーク
 */
//-----------------------------------------------------------------------------
void MISC_Init( MISC *p_msc )
{	
	GFL_STD_MemClear( p_msc, sizeof(MISC) );

	MISC_InitNameIn( p_msc );
	
	p_msc->palpark_highscore = 0;
	
  // すれ違いデフォルトメッセージ
  {
    GFL_MSGDATA* msg;
    STRBUF* strbuf;

    msg = GFL_MSG_Create( 
        GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE,
        NARC_script_message_c03r0101_dat, GetHeapLowID(HEAPID_SAVE) );

    // 挨拶
    strbuf = GFL_MSG_CreateString( msg, msg_c03r0101_input_a );
    GFL_STR_GetStringCode( 
        strbuf, p_msc->self_introduction, SAVE_SURETIGAI_SELFINTRODUCTION_LEN );
    GFL_STR_DeleteBuffer( strbuf );

    // お礼
    strbuf = GFL_MSG_CreateString( msg, msg_c03r0101_input_b );
    GFL_STR_GetStringCode( 
        strbuf, p_msc->thankyou_message, SAVE_SURETIGAI_THANKYOU_LEN );
    GFL_STR_DeleteBuffer( strbuf );

    GFL_MSG_Delete( msg );
  }

  p_msc->research_team_rank = RESEARCH_TEAM_RANK_0;
  p_msc->research_request_id = RESEARCH_REQ_ID_NONE;
  p_msc->research_question_id[0] = QUESTION_ID_DUMMY;
  p_msc->research_question_id[1] = QUESTION_ID_DUMMY;
  p_msc->research_question_id[2] = QUESTION_ID_DUMMY;
}

//=============================================================================
/**
 *	セーブデータ取得のための関数
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	未分類データのCONST版ポインタ取得
 *
 *	@param	SAVE_CONTROL_WORK * cp_sv	セーブデータ保持ワークへのポインタ
 *
 *	@return	MISC
 */
//-----------------------------------------------------------------------------
const MISC * SaveData_GetMiscConst( const SAVE_CONTROL_WORK * cp_sv)
{	
	return (const MISC *)SaveData_GetMisc( (SAVE_CONTROL_WORK *)cp_sv);
}
//----------------------------------------------------------------------------
/**
 *	@brief	未分類データのポインタ取得
 *
 *	@param	SAVE_CONTROL_WORK * p_sv	セーブデータ保持ワークへのポインタ
 *
 *	@return	MISC
 */
//-----------------------------------------------------------------------------
MISC * SaveData_GetMisc( SAVE_CONTROL_WORK * p_sv)
{	
	MISC	*p_msc;
	p_msc = SaveControl_DataPtrGet(p_sv, GMDATA_ID_MISC);
	return p_msc;
}

//=============================================================================
/**
 *	名前入力の関数
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	名前入力のモード設定
 *
 *	@param	MISC *p_misc	MISC
 *	@param	mode					モード
 *	@param	input_type		入力タイプ
 */
//-----------------------------------------------------------------------------
void MISC_SetNameInMode( MISC *p_misc, u32 mode, u8 input_type )
{	
  GF_ASSERT( mode < NAMEIN_SAVEMODE_MAX );
	p_misc->namein_mode[ mode ]	= input_type;
}

//----------------------------------------------------------------------------
/**
 *	@brief	名前入力のモード取得
 *
 *	@param	const MISC *cp_misc		MISC
 *	@param	mode									モード
 *
 *	@return	入力タイプ
 */
//-----------------------------------------------------------------------------
u8 MISC_GetNameInMode( const MISC *cp_misc, u32 mode )
{	
  GF_ASSERT( mode < NAMEIN_SAVEMODE_MAX );
	return cp_misc->namein_mode[ mode ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	名前入力の初期化
 *
 *	@param	MISC *p_misc ワーク
 */
//-----------------------------------------------------------------------------
static void MISC_InitNameIn( MISC *p_misc )
{
  GFL_STD_MemClear( p_misc->namein_mode, sizeof(u8)*NAMEIN_SAVEMODE_MAX );
	p_misc->namein_mode[ NAMEIN_POKEMON ] = 1;  //ポケモンだけカタカナ開始
}

//==============================================================================
//  GDS
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   お気に入りポケモンのセット
 *
 * @param   misc		
 * @param   monsno		ポケモン番号
 * @param   form_no		フォルム番号
 * @param   egg_flag	タマゴフラグ
 * @param   sex       性別
 */
//--------------------------------------------------------------
void MISC_SetFavoriteMonsno(MISC * misc, int monsno, int form_no, int egg_flag, int sex )
{
  //OS_TPrintf( "お気に入りポケモンは%dでフォルムは%dで、性別は %d、タマゴフラグ%dです\n", monsno, form_no, egg_flag );
	misc->favorite_monsno = monsno;
	misc->favorite_form_no = form_no;
	misc->favorite_egg_flag = egg_flag;
  misc->favorite_sex  = (sex == PTL_SEX_FEMALE);  //ポケモンアイコン用の性別なので♀のときのみON
}

//--------------------------------------------------------------
/**
 * @brief   お気に入りポケモン取得
 *
 * @param   misc		
 * @param   monsno		ポケモン番号(お気に入りを設定していない場合は0)
 * @param   form_no		フォルム番号
 * @param   egg_flag	タマゴフラグ
 * @param   p_sex     性別
 */
//--------------------------------------------------------------
void MISC_GetFavoriteMonsno(const MISC * misc, int *monsno, int *form_no, int *egg_flag, int *p_sex )
{
	*monsno = misc->favorite_monsno;
	*form_no = misc->favorite_form_no;
	*egg_flag = misc->favorite_egg_flag;
  *p_sex    = misc->favorite_sex;
}

//--------------------------------------------------------------
/**
 * @brief   パルパーク：ハイスコア取得/設定
 *
 * @param   misc		
 */
//--------------------------------------------------------------
const u32  MISC_GetPalparkHighscore(const MISC *misc)
{
	return misc->palpark_highscore;
}
void  MISC_SetPalparkHighscore(MISC *misc , u32 score)
{
	misc->palpark_highscore = score;
}

//--------------------------------------------------------------
/**
 * @brief   パルパーク：終了ステート取得/設定
 *          script/palpark_scr_local.h に定義
 * @param   misc		
 */
//--------------------------------------------------------------
const u8  MISC_GetPalparkFinishState(const MISC *misc)
{
	return misc->palpark_finish_state;
}
void  MISC_SetPalparkFinishState(MISC *misc , u8 state)
{
	misc->palpark_finish_state = state;
}


//==============================================================================
//  すれ違い通信
//==============================================================================
//==================================================================
/**
 * すれ違い通信：お礼を言われた回数を取得
 *
 * @param   misc		
 *
 * @retval  u32		お礼を言われた回数
 */
//==================================================================
u32 MISC_CrossComm_GetThanksRecvCount(const MISC *misc)
{
  return misc->thanks_recv_count;
}

//==================================================================
/**
 * すれ違い通信：お礼を言われた回数をセット
 *
 * @param   misc		
 */
//==================================================================
void MISC_CrossComm_SetThanksRecvCount(MISC *misc, u32 count )
{
  if(misc->thanks_recv_count <= CROSS_COMM_THANKS_RECV_COUNT_MAX){
    misc->thanks_recv_count = count;
  }
}

//==================================================================
/**
 * すれ違い通信：お礼を言われた回数をインクリメント
 *
 * @param   misc		
 *
 * @retval  u32		インクリメント後のお礼を言われた回数
 */
//==================================================================
u32 MISC_CrossComm_IncThanksRecvCount(MISC *misc)
{
  if(misc->thanks_recv_count < CROSS_COMM_THANKS_RECV_COUNT_MAX){
    misc->thanks_recv_count++;
  }
  return misc->thanks_recv_count;
}

//==================================================================
/**
 * すれ違いした人数を取得
 *
 * @param   misc		
 *
 * @retval  u32		すれ違いをした人数
 */
//==================================================================
u32 MISC_CrossComm_GetSuretigaiCount(const MISC *misc)
{
  return misc->suretigai_count;
}

//==================================================================
/**
 * すれ違いした人数をセット
 *
 * @param   misc		
 */
//==================================================================
void MISC_CrossComm_SetSuretigaiCount( MISC *misc, u32 count )
{
  if(misc->suretigai_count <= CROSS_COMM_SURETIGAI_COUNT_MAX){
    misc->suretigai_count = count;
  }
}

//==================================================================
/**
 * すれ違いをした人数をインクリメント
 *
 * @param   misc		
 *
 * @retval  u32		インクリメント後のすれ違いをした人数
 */
//==================================================================
u32 MISC_CrossComm_IncSuretigaiCount(MISC *misc)
{
  if(misc->suretigai_count < CROSS_COMM_SURETIGAI_COUNT_MAX){
    misc->suretigai_count++;
  }
  return misc->suretigai_count;
}

//==================================================================
/**
 * すれ違い通信の自己紹介文を取得
 *
 * @param   misc		
 *
 * @retval  const STRCODE *		
 */
//==================================================================
const STRCODE * MISC_CrossComm_GetSelfIntroduction(const MISC *misc)
{
  return misc->self_introduction;
}

//==================================================================
/**
 * すれ違い通信の自己紹介文をセット
 *
 * @param   misc		
 * @param   srcbuf		
 */
//==================================================================
void MISC_CrossComm_SetSelfIntroduction(MISC *misc, const STRBUF *srcbuf)
{
  GFL_STR_GetStringCode( srcbuf, misc->self_introduction, SAVE_SURETIGAI_SELFINTRODUCTION_LEN );
}

//==================================================================
/**
 * すれ違い通信のお礼メッセージを取得
 *
 * @param   misc		
 *
 * @retval  const STRCODE *		
 */
//==================================================================
const STRCODE * MISC_CrossComm_GetThankyouMessage(const MISC *misc)
{
  return misc->thankyou_message;
}

//==================================================================
/**
 * すれ違い通信のお礼メッセージをセット
 *
 * @param   misc		
 * @param   srcbuf		
 */
//==================================================================
void MISC_CrossComm_SetThankyouMessage(MISC *misc, const STRBUF *srcbuf)
{
  GFL_STR_GetStringCode( srcbuf, misc->thankyou_message, SAVE_SURETIGAI_THANKYOU_LEN );
}

//==================================================================
/**
 * すれ違い通信の調査隊隊員ランクをゲット
 *
 * @param   misc		
 * 
 * @retval  RESEARCH_TEAM_RANK_xxx
 */
//==================================================================
int MISC_CrossComm_GetResearchTeamRank(const MISC *misc)
{
  return misc->research_team_rank;
}

//==================================================================
/**
 * すれ違い通信の調査隊隊員ランクをセット
 *
 * @param   misc		
 * @param   rank		RESEARCH_TEAM_RANK_xxx
 */
//==================================================================
void MISC_CrossComm_SetResearchTeamRank(MISC *misc, int rank)
{
  misc->research_team_rank = rank;
}

//----------------------------------------------------------
/**
 * @brief バッジ保持状態の取得
 * @param my      自分状態保持ワークへのポインタ
 * @param badge_id  バッジの指定
 * @return  BOOL  バッジを持っているかどうか
 */
//----------------------------------------------------------
BOOL MISC_GetBadgeFlag(const MISC * misc, int badge_id)
{
  if (misc->badge & (1 << badge_id)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

//----------------------------------------------------------
/**
 * @brief バッジ取得のセット
 * @param misc      自分状態保持ワークへのポインタ
 * @param badge_id  バッジの指定
 */
//----------------------------------------------------------
void MISC_SetBadgeFlag(MISC * misc, int badge_id)
{
  misc->badge |= (1 << badge_id);
}

//----------------------------------------------------------
/**
 * @brief 持っているバッジの数を取得する
 * @param misc      自分状態保持ワークへのポインタ
 * @return  int   持っているバッジの数（０〜３２）
 */
//----------------------------------------------------------
int MISC_GetBadgeCount(const MISC * misc)
{
  int count = 0;
  u32 badge;

  for (badge = misc->badge; badge != 0; badge >>= 1) {
    if (badge & 1) {
      count ++;
    }
  }
  return count;
}
//----------------------------------------------------------
/**
 * @brief 持っている金額を取得する
 * @param my    自分状態保持ワークへのポインタ
 * @return  u32   持っている金額
 */
//----------------------------------------------------------
u32 MISC_GetGold(const MISC * misc)
{
  return misc->gold;
}

//----------------------------------------------------------
/**
 * @brief   お金を設定する
 * @param misc    自分状態保持ワークへのポインタ
 * @param gold  セットする金額
 * @return  u32   現在の手持ち金額
 */
//----------------------------------------------------------
u32 MISC_SetGold(MISC * misc, u32 gold)
{
  if (gold > MY_GOLD_MAX) {
    gold = MY_GOLD_MAX;
  }
  misc->gold = gold;
  return misc->gold;
}

//----------------------------------------------------------
/**
 * @brief  お金を増やす
 * @param misc    自分状態保持ワークへのポインタ
 * @param add   加える金額
 * @return  u32   現在の手持ち金額
 */
//----------------------------------------------------------
u32 MISC_AddGold(MISC * misc, u32 add)
{
  if (add > MY_GOLD_MAX) {
    misc->gold = MY_GOLD_MAX;
  } else {
    misc->gold += add;
  }
  if (misc->gold > MY_GOLD_MAX) {
    misc->gold = MY_GOLD_MAX;
  }
  return misc->gold;
}

//----------------------------------------------------------
/**
 * @brief    お金を減らす
 * @param misc    自分状態保持ワークへのポインタ
 * @param sub   引き出す金額
 * @return  u32   現在の手持ち金額
 */
//----------------------------------------------------------
u32 MISC_SubGold(MISC * misc, u32 sub)
{
  if (misc->gold < sub) {
    misc->gold = 0;
  } else {
    misc->gold -= sub;
  }
  return misc->gold;
}

//==================================================================
//
//
//      サブイベント関連
//
//
//==================================================================
//----------------------------------------------------------
//----------------------------------------------------------
void MISC_SetGymVictoryInfo( MISC * misc, int gym_id, const u16 * monsnos )
{
  int i;
  GF_ASSERT( gym_id < GYM_MAX );
  for (i = 0; i < TEMOTI_POKEMAX; i++)
  {
    misc->gym_win_monsno[gym_id][i] = monsnos[i];
  }
}

//----------------------------------------------------------
//----------------------------------------------------------
void MISC_GetGymVictoryInfo( const MISC * misc, int gym_id, u16 * monsnos )
{
  int i;
  GF_ASSERT( gym_id < GYM_MAX );
  for (i = 0; i < TEMOTI_POKEMAX; i++)
  {
    monsnos[i] = misc->gym_win_monsno[gym_id][i];
  }
}


//==============================================================================
//  タイトルメニュー
//==============================================================================

//----------------------------------------------------------
/**
 * @brief		フラグセット
 * @param		misc
 * @param		type		メニュータイプ
 * @param		flg			フラグ
 * @return  
 */
//----------------------------------------------------------
void MISC_SetStartMenuFlag( MISC * misc, MISC_STARTMENU_TYPE type, MISC_STARTMENU_FLAG flg )
{
	misc->start_menu_open |= ( flg << (type<<1) );
}

//----------------------------------------------------------
/**
 * @brief		フラグ取得
 * @param		misc
 * @param		type		メニュータイプ
 * @retval	フラグ
 */
//----------------------------------------------------------
u8 MISC_GetStartMenuFlag( const MISC * misc, MISC_STARTMENU_TYPE type )
{
	return ( ( misc->start_menu_open >> (type<<1) ) & (MISC_STARTMENU_FLAG_OPEN|MISC_STARTMENU_FLAG_VIEW) );
}

//----------------------------------------------------------
/**
 * @brief		フラグ取得（全体）
 * @param		misc
 * @retval	フラグ
 */
//----------------------------------------------------------
u8 MISC_GetStartMenuFlagAll( const MISC * misc )
{
	return misc->start_menu_open;
}

//----------------------------------------------------------
/**
 * @brief		フラグセット（全体）
 * @param		misc
 * @param		flg			フラグ
 * @return  
 */
//----------------------------------------------------------
void MISC_SetStartMenuFlagAll( MISC * misc, u8 flg )
{
	misc->start_menu_open = flg;
}


//==============================================================================
// すれ違い調査隊
//==============================================================================
//----------------------------------------------------------
/**
 * @brief 受けている調査依頼IDを取得する
 *
 * @param misc
 *
 * @return 調査依頼ID ( RESEARCH_REQ_ID_xxxx )
 */
//----------------------------------------------------------
u8 MISC_GetResearchRequestID( const MISC* misc )
{
  return misc->research_request_id;
}

//----------------------------------------------------------
/**
 * @brief 調査隊として調べている調査依頼IDをセットする
 *
 * @param misc
 * @param id  セットする調査依頼ID ( RESEARCH_REQ_ID_xxxx )
 */
//----------------------------------------------------------
void MISC_SetResearchRequestID( MISC* misc, u8 id )
{
  GF_ASSERT( id <= RESEARCH_REQ_ID_MAX );
  misc->research_request_id = id;
}

//----------------------------------------------------------
/**
 * @brief 調査隊として調べている質問IDを取得する
 *
 * @param misc
 * @param idx  取得するインデックス
 *
 * @return 質問ID ( QUESTION_ID_xxxx )
 */
//----------------------------------------------------------
u8 MISC_GetResearchQuestionID( const MISC* misc, u8 idx )
{
  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );
  return misc->research_question_id[ idx ];
}

//----------------------------------------------------------
/**
 * @brief 調査隊として調べている質問IDをセットする
 *
 * @param misc
 * @param idx セットするインデックス
 * @param id  セットする調査依頼ID ( QUESTION_ID_xxxx )
 */
//----------------------------------------------------------
void MISC_SetResearchQuestionID( MISC* misc, u8 idx, u8 id )
{
  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );
  GF_ASSERT( (id == QUESTION_ID_DUMMY) || (id <= QUESTION_ID_MAX) );
  misc->research_question_id[ idx ] = id;
}
//----------------------------------------------------------
/**
 * @brief 調査隊として調査中の質問について,
 *        調査を開始してからの調査人数を取得する
 *
 * @param misc
 * @param idx  調査依頼内の質問インデックス ( 0 〜 MAX_QNUM_PER_RESEARCH_REQ-1 )
 *
 * @return 開始からの調査人数
 */
//----------------------------------------------------------
u16 MISC_GetResearchCount( const MISC* misc, u8 idx )
{
  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );
  return misc->research_count[ idx ];
}

//----------------------------------------------------------
/**
 * @brief 調査隊として調査中の質問について,
 *        調査を開始してからの調査人数をセットする
 *
 * @param misc
 * @param idx   調査依頼内の質問インデックス ( 0 〜 MAX_QNUM_PER_RESEARCH_REQ-1 ) 
 * @param count セットする人数
 */
//----------------------------------------------------------
void MISC_SetResearchCount( MISC* misc, u8 idx, u16 count )
{
  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );
  misc->research_count[idx] = count;
}

//----------------------------------------------------------
/**
 * @brief 調査隊として調査中の質問について,
 *        調査を開始してからの調査人数を加算する
 *
 * @param misc
 * @param idx   調査依頼内の質問インデックス ( 0 〜 MAX_QNUM_PER_RESEARCH_REQ-1 ) 
 * @param count 加算人数
 */
//----------------------------------------------------------
void MISC_AddResearchCount( MISC* misc, u8 idx, u16 add )
{
  u32 now, sum;

  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );

  now = misc->research_count[idx];
  sum = now + add; 
  if( 0xffff < sum ) { sum = 0xffff; }

  misc->research_count[idx] = sum;
}

//----------------------------------------------------------
/**
 * @brief 調査依頼を受けた時の時間[秒]を取得する
 *
 * @param misc
 *
 * @return 調査開始時点での時間[秒]
 */
//----------------------------------------------------------
s64 MISC_GetResearchStartTimeBySecond( const MISC* misc )
{
  return misc->research_start_time;
}

//----------------------------------------------------------
/**
 * @brief 調査依頼を受けた時の時間[秒]をセットする
 *
 * @param misc
 * @param time 調査開始時点の時間[秒]
 */
//----------------------------------------------------------
void MISC_SetResearchStartTimeBySecond( MISC* misc, s64 time )
{
  misc->research_start_time = time;
}

//----------------------------------------------------------
/**
 * @brief イベントロック値をセット
 *
 * @param misc
 * @param inNo    イベントナンバー
 * @param inValue 値
 */
//----------------------------------------------------------
void MISC_SetEvtRockValue( MISC* misc, const int inNo, const u32 inValue )
{
  if ( inNo < EVT_ROCK_MAX ) misc->event_lock[inNo] = inValue;
}

//----------------------------------------------------------
/**
 * @brief イベントロック値を取得
 *
 * @param misc
 * @param inNo    イベントナンバー
 * @return u32    値
 */
//----------------------------------------------------------
u32 MISC_GetEvtRockValue( MISC* misc, const int inNo )
{
  if (inNo >= EVT_ROCK_MAX)
  {
    GF_ASSERT_MSG(0,"No Error");
    return 0;
  }

  return misc->event_lock[inNo];
}

//----------------------------------------------------------
/**
 * @brief チャンピオンニュースの残り時間を取得する
 *
 * @param  misc
 *
 * @return 電光掲示板でチャンプニュースを流す残り時間[min]
 */
//----------------------------------------------------------
u16 MISC_GetChampNewsMinutes( const MISC* misc )
{
  return misc->champ_news_minutes;
}

//----------------------------------------------------------
/**
 * @brief チャンピオンニュースの残り時間を設定する
 *
 * @param misc
 * @param minutes 残り時間[min]
 */
//----------------------------------------------------------
void MISC_SetChampNewsMinutes( MISC* misc, u16 minutes )
{
  misc->champ_news_minutes = minutes;
}

//----------------------------------------------------------
/**
 * @brief チャンピオンニュースの残り時間をデクリメントする
 *
 * @param misc
 * @param minutes 経過時間[min]
 */
//----------------------------------------------------------
void MISC_DecChampNewsMinutes( MISC* misc, u16 minutes )
{
  if( minutes <= misc->champ_news_minutes ) {
    misc->champ_news_minutes -= minutes;
  }
  else {
    misc->champ_news_minutes = 0;
  }
}
//==============================================================================
//  バトルレコーダー
//==============================================================================
//----------------------------------------------------------------------------
/** 
 *	@brief  バトルレコーダーの色を取得
 *
 *	@param	const MISC * misc   ミスク
 *
 *	@return バトルレコーダーの色
 */
//-----------------------------------------------------------------------------
u8 MISC_GetBattleRecorderColor( const MISC * misc )
{ 
  return misc->battle_recorder_flag;
}
//----------------------------------------------------------------------------
/**
 *	@brief  バトルレコーダーの色を設定
 *
 *	@param	MISC * misc ミスク
 *	@param	color       色（net_app/battle_recorder/br_res.hにあるBR_RES_COLOR_TYPE列挙）
 */
//-----------------------------------------------------------------------------
void MISC_SetBattleRecorderColor( MISC * misc, u8 color )
{ 
  misc->battle_recorder_flag  = color;
}
