//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		tradepoke_after_save.c
 *	@brief  ポケモン交換　その後　セーブデータ
 *	@author	tomoya takahashi
 *	@date		2010.03.26
 *
 *	モジュール名：TRPOKE_AFTER_SAVE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "savedata/tradepoke_after_save.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	1データ
//=====================================
typedef struct {
  u8 level;
  u8 sex;
  u8 seikaku;
  u8 speabino;
  u32 personal_rnd;
	STRCODE	nickname[MONS_NAME_SIZE+EOM_SIZE];	//16h	ニックネーム(MONS_NAME_SIZE=10)+(EOM_SIZE=1)=11
  u8  indata;
  u8  pad[1];
} TRPOKE_AFTER_DATA;


//-------------------------------------
///	交換ポケモン　その後　セーブワーク
//=====================================
struct _TRPOKE_AFTER_SAVE 
{
  TRPOKE_AFTER_DATA data[TRPOKE_AFTER_SAVE_TYPE_MAX];
};

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------
//	セーブデータシステムが依存する関数
//----------------------------------------------------------
int TRPOKE_AFTER_SV_GetWorkSize(void)
{
  return sizeof(TRPOKE_AFTER_SAVE);
}

//----------------------------------------------------------------------------
/**
 *	@brief  メモリ確保
 */
//-----------------------------------------------------------------------------
TRPOKE_AFTER_SAVE* TRPOKE_AFTER_SV_AllocWork(HEAPID heapID)
{
  TRPOKE_AFTER_SAVE* sv;
  sv = GFL_HEAP_AllocClearMemory( heapID, sizeof(TRPOKE_AFTER_SAVE) );
  return sv;
}

//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 */
//-----------------------------------------------------------------------------
void TRPOKE_AFTER_SV_Init(TRPOKE_AFTER_SAVE* sv)
{
  GFL_STD_MemClear( sv, sizeof(TRPOKE_AFTER_SAVE) );
}


//----------------------------------------------------------
//	データアクセス
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  データ有無
 *
 *	@param	sv      セーブ
 *	@param	type    タイプ
 *
 *	@retval TRUE    セーブあり
 *	@retval FALSE   セーブなし
 */
//-----------------------------------------------------------------------------
BOOL TRPOKE_AFTER_SV_IsData( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[ type ].indata;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レベルを取得
 *
 *	@param	sv    ワーク
 *	@param	type  タイプ
 *
 *	@return レベル
 */
//-----------------------------------------------------------------------------
u8 TRPOKE_AFTER_SV_GetLevel( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  )
{
  u8 ret;
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  ret = sv->data[ type ].level + TRPOKE_AFTER_LEVEL_ADD;
  if( ret > TRPOKE_AFTER_LEVEL_MAX ){
    ret = TRPOKE_AFTER_LEVEL_MAX;
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  性別を取得
 *
 *	@param	sv      ワーク
 *	@param	type    タイプ
 *
 *	@return 性別
 */
//-----------------------------------------------------------------------------
u8 TRPOKE_AFTER_SV_GetSex( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[ type ].sex;
}

//----------------------------------------------------------------------------
/**
 *	@brief  とくせいを取得
 *
 *	@param	sv      ワーク
 *	@param	type    タイプ
 *
 *	@return 特性
 */
//-----------------------------------------------------------------------------
u8 TRPOKE_AFTER_SV_GetSpeabino( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[ type ].speabino;
}

//----------------------------------------------------------------------------
/**
 *	@brief  固体乱数を取得
 *
 *	@param	sv      ワーク
 *	@param	type    タイプ
 *
 *	@return 固体乱数
 */
//-----------------------------------------------------------------------------
u32 TRPOKE_AFTER_SV_GetRnd( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[ type ].personal_rnd;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ニックネームを取得
 *
 *	@param	sv      ワーク
 *	@param	type    タイプ
 *	@param	str     文字列格納先
 */
//-----------------------------------------------------------------------------
const STRCODE* TRPOKE_AFTER_SV_GetNickName( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[type].nickname;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ポケモン情報を設定
 *
 *	@param	sv      ワーク
 *	@param	type    タイプ
 *	@param	pp      ポケモン情報
 */
//-----------------------------------------------------------------------------
void TRPOKE_AFTER_SV_SetData( TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type, const POKEMON_PARAM * pp )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );

  sv->data[type].level = PP_Get( pp, ID_PARA_level, NULL );
  sv->data[type].sex = PP_Get( pp, ID_PARA_sex, NULL );
  sv->data[type].seikaku = PP_Get( pp, ID_PARA_seikaku, NULL );
  sv->data[type].speabino = PP_Get( pp, ID_PARA_speabino, NULL );
  sv->data[type].personal_rnd = PP_Get( pp, ID_PARA_personal_rnd, NULL );
  PP_Get( pp, ID_PARA_nickname_raw, sv->data[type].nickname );
  sv->data[type].indata = TRUE;
}



