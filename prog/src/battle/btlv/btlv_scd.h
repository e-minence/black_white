//=============================================================================================
/**
 * @file	btlv_scd.h
 * @brief	ポケモンWB バトル 描画 下画面制御モジュール
 * @author	taya
 *
 * @date	2008.11.18	作成
 */
//=============================================================================================
#ifndef __BTLV_SCD_H__
#define __BTLV_SCD_H__

//--------------------------------------------------------------
/**
 *	モジュールハンドラ型
 */
//--------------------------------------------------------------
typedef struct _BTLV_SCD	BTLV_SCD;

#include <tcbl.h>
#include "print/gf_font.h"

#include "battle/btl_main.h"
#include "battle/btl_pokeparam.h"
#include "battle/btl_action.h"
#include "battle/btl_pokeselect.h"
#include "btlv_core.h"

//--------------------------------------------------------------
/**
 *	アクション選択中の経過
 */
//--------------------------------------------------------------
typedef enum {
	BTLV_SCD_SelAction_Still = 0,	///< まだ選択中
	BTLV_SCD_SelAction_Done,			///< 選択おわった

	BTLV_SCD_SelAction_Warn_Kodawari,	///< こだわり中、出せないワザが選ばれた

}BtlvScd_SelAction_Result;



extern BTLV_SCD*  BTLV_SCD_Create( const BTLV_CORE* vcore,
		const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon, GFL_TCBLSYS* tcbl, GFL_FONT* font, u8 playerClientID, HEAPID heapID );
extern void BTLV_SCD_Delete( BTLV_SCD* wk );

extern void BTLV_SCD_Setup( BTLV_SCD* wk );

extern void BTLV_SCD_StartActionSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest );
extern BtlvScd_SelAction_Result BTLV_SCD_WaitActionSelect( BTLV_SCD* wk );
extern void BTLV_SCD_RestartActionSelect( BTLV_SCD* wk );

extern void BTLV_SCD_StartPokemonSelect( BTLV_SCD* wk );
extern BOOL BTLV_SCD_WaitPokemonSelect( BTLV_SCD* wk );

extern void BTLV_SCD_PokeSelect_Start( BTLV_SCD* wk, const BTL_POKESELECT_PARAM* param, BTL_POKESELECT_RESULT* result );
extern BOOL BTLV_SCD_PokeSelect_Wait( BTLV_SCD* wk );

extern void BTLV_SCD_StartCommWaitInfo( BTLV_SCD* wk );
extern BOOL BTLV_SCD_WaitCommWaitInfo( BTLV_SCD* wk );
extern void BTLV_SCD_ClearCommWaitInfo( BTLV_SCD* wk );


#endif
