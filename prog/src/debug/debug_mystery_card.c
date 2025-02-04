//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_mystery_card.c
 *	@brief  デバッグ用ふしぎなおくりもの作成  
 *	@author	Toru=Nagihashi
 *	@date		2010.02.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//モジュール
#include "poke_tool/monsno_def.h"
#include "item/itemsym.h"
#include "field/gpower_id.h"

//セーブデータ
#include "savedata/mystery_data.h"

//外部公開
#include "debug/debug_mystery_card.h"
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

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  ポケモンいりのデータを作成
 *
 *	@param	GIFT_PACK_DATA *p_data  データ
 */
//-----------------------------------------------------------------------------
void DEBUG_MYSTERY_SetGiftPokeData( GIFT_PACK_DATA *p_data )
{ 
  GFL_STD_MemClear( p_data, sizeof(GIFT_PACK_DATA) );
  //ポケモン作成
  { 
    GIFT_PRESENT_POKEMON  *p_poke = &p_data->data.pokemon;
    p_poke->mons_no = 1;
    p_poke->form_no = 0;
    p_poke->waza1   = 1;
    p_poke->hp_rnd  = 1;               //HP乱数        0xFFでランダム 
    p_poke->pow_rnd = 2;              //攻撃力乱数    0xFFでランダム 
    p_poke->def_rnd = 3;              //防御力乱数    0xFFでランダム 
    p_poke->agi_rnd = 0xFF;              //素早さ乱数    0xFFでランダム 
    p_poke->spepow_rnd  = 0xFF;             //特攻乱数    0xFFでランダム 
    p_poke->spedef_rnd  = 0xFF;             //特防乱数    0xFFでランダム 
    p_poke->rnd   = 0;
    p_poke->egg		= 0;
    p_poke->rare  = 2;
    p_poke->level = 3;
    p_poke->oyaname[0]  = L'デ';
    p_poke->oyaname[1]  = L'バ';
    p_poke->oyaname[2]  = L'ッ';
    p_poke->oyaname[3]  = L'グ';
    p_poke->oyaname[4]  = GFL_STR_GetEOMCode();

    p_poke->nickname[0]  = L'デ';
    p_poke->nickname[1]  = GFL_STR_GetEOMCode();

    p_poke->birth_place = 40001;
    p_poke->get_place   = 1;
    p_poke->version     = 0;
  }
  p_data->gift_type     = MYSTERYGIFT_TYPE_POKEMON;
}

//----------------------------------------------------------------------------
/**
 *	@brief  道具いりのデータを作成
 *
 *	@param	GIFT_PACK_DATA *p_data  データ 
 */
//-----------------------------------------------------------------------------
void DEBUG_MYSTERY_SetGiftItemData( GIFT_PACK_DATA *p_data, u16 itemno )
{ 
  GFL_STD_MemClear( p_data, sizeof(GIFT_PACK_DATA) );
  //道具作成
  { 
    GIFT_PRESENT_ITEM  *p_item = &p_data->data.item;
    p_item->itemNo    = 341;
    p_item->movieflag = FALSE;
  }
  p_data->gift_type     = MYSTERYGIFT_TYPE_ITEM;
}
//----------------------------------------------------------------------------
/**
 *	@brief  GPOWERいりのデータを作成
 *
 *	@param	GIFT_PACK_DATA *p_data  データ 
 */
//-----------------------------------------------------------------------------
void DEBUG_MYSTERY_SetGiftGPowerData( GIFT_PACK_DATA *p_data )
{ 
  GFL_STD_MemClear( p_data, sizeof(GIFT_PACK_DATA) );
  //道具作成
  { 
    GIFT_PRESENT_POWER  *p_gpower = &p_data->data.gpower;
    p_gpower->type  = GPOWER_ID_DISTRIBUTION_END - 1;  //GPOWER_ID_DISTRIBUTION_END - 10 からMAXまでが配布用
  }
  p_data->gift_type     = MYSTERYGIFT_TYPE_POWER;

}
//----------------------------------------------------------------------------
/**
 *	@brief  共通部分のデータ作成
 *
 *	@param	DOWNLOAD_GIFT_DATA *p_data  データ
 *	@param	event_id
 *	@param	only_one_flag
 *	@param	version 
 */
//-----------------------------------------------------------------------------
void DEBUG_MYSTERY_SetGiftCommonData( GIFT_PACK_DATA *p_data, u32 event_id, BOOL only_one_flag )
{ 
  //イベントタイトル
  p_data->event_name[ 0] = L'デ';
  p_data->event_name[ 1] = L'バ';
  p_data->event_name[ 2] = L'ッ';
  p_data->event_name[ 3] = L'グ';
  p_data->event_name[ 4] = L'イ';
  p_data->event_name[ 5] = L'ベ';
  p_data->event_name[ 6] = L'ン';
  p_data->event_name[ 7] = L'ト';
  p_data->event_name[ 8] = L'タ';
  p_data->event_name[ 9] = L'イ';
  p_data->event_name[10] = L'ト';
  p_data->event_name[11] = L'ル';
  p_data->event_name[12] = 0xffff;

  //受信した時間
  {
    RTCDate date;
    GFL_RTC_GetDate( &date );
    p_data->recv_date      = ((2000+date.year)<<MYSTERYGIFT_DATE_SHIFT_YEAR) | (date.month<<MYSTERYGIFT_DATE_SHIFT_MONTH) | (date.day<<MYSTERYGIFT_DATE_SHIFT_DAY);
  }

  //イベントID
  p_data->event_id       = event_id;

  //データ
  p_data->card_message   = 0;
  
  //一度だけ受信フラグ
  p_data->only_one_flag  = only_one_flag;

  //受け取ったかどうか
  p_data->have  = 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ダウンロードふしぎなおくりものでーたを設定
 *
 *	@param	DOWNLOAD_GIFT_DATA *p_data  ワーク
 *	@param	version   バージョン（ビット）
 *	@param	langCode  言語コード
 */
//-----------------------------------------------------------------------------
void DEBUG_MYSTERY_SetDownLoadData( DOWNLOAD_GIFT_DATA *p_data, u32 version, u8 langCode )
{ 
  p_data->LangCode  = langCode;
  p_data->movie_flag  = 0;
  p_data->version = version;

  GFL_STD_MemClear( p_data->event_text, sizeof(STRCODE)*(GIFT_DATA_CARD_TEXT_MAX+EOM_SIZE) );
  p_data->event_text[0] = L'デ';
  p_data->event_text[1] = L'バ';
  p_data->event_text[2] = L'ッ';
  p_data->event_text[3] = L'ク';
  p_data->event_text[4] = L'よ';
  p_data->event_text[5] = L'う';
  p_data->event_text[6] = L'の';
  p_data->event_text[7] = L'　';
  p_data->event_text[8] = L'か';
  p_data->event_text[9] = L'り';
  p_data->event_text[10] = L'フ';
  p_data->event_text[11] = L'ァ';
  p_data->event_text[12] = L'イ';
  p_data->event_text[13] = L'ル';
  p_data->event_text[14] = L'で';
  p_data->event_text[15] = L'す';
  p_data->event_text[16] = GFL_STR_GetEOMCode();
  p_data->crc  = GFL_STD_CrcCalc( p_data, sizeof(DOWNLOAD_GIFT_DATA) - 2 );
}
