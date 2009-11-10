//==============================================================================
/**
 * @file    battle_rec.c
 * @brief   戦闘録画セーブ
 * @author  matsuda
 * @date    2009.09.01(火)
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_tbl.h"
#include "savedata/battle_rec.h"


//==============================================================================
//  
//==============================================================================
u32 * brs=NULL;


//------------------------------------------------------------------
/**
 * セーブデータサイズを返す
 *
 * @retval  int		
 */
//------------------------------------------------------------------
u32 BattleRec_GetWorkSize( void )
{
	return 0x100;
}

//--------------------------------------------------------------
/**
 * @brief   ワーク初期化
 *
 * @param   rec		
 */
//--------------------------------------------------------------
void BattleRec_WorkInit(void *rec)
{
  return;
}

//--------------------------------------------------------------
/**
 * @brief   認証キーを除いた戦闘録画ワークのアドレスを取得
 *
 * @retval	brsに格納されているワークアドレス(認証キー除く)
 */
//--------------------------------------------------------------
void * BattleRec_RecWorkAdrsGet( void )
{
	u8 *work;
	
	GF_ASSERT(brs);
	
	work = (u8*)brs;
	return &work[sizeof(EX_CERTIFY_SAVE_KEY)];
}

//--------------------------------------------------------------
/**
 * @brief   GDSプロフィールのポインタ取得
 *
 * @param   src		戦闘録画セーブデータへのポインタ
 *
 * @retval  プロフィールのポインタ
 *
 * BattleRec_Load or BattleRec_Initを使用してbrsにデータをロードしている必要があります。
 */
//--------------------------------------------------------------
GDS_PROFILE_PTR BattleRec_GDSProfilePtrGet(void)
{
	GF_ASSERT(brs != NULL);
#if 0 //※check
	return &brs->profile;
#else
  return (void*)brs;
#endif
}
