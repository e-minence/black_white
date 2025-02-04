//============================================================================
/**
 *
 *	@file		intro_mcss.h
 *	@brief  イントロデモ MCSSラッパー
 *	@author hosaka genya
 *	@data		2009.12.18
 *
 */
//============================================================================
#pragma once

#include <gflib.h>
#include "system/mcss.h"
#include "demo/intro.h"

typedef struct _INTRO_MCSS_WORK INTRO_MCSS_WORK;

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================

//=============================================================================
/**
 *								EXTERN宣言
 */
//=============================================================================



extern INTRO_MCSS_WORK* INTRO_MCSS_Create( HEAPID heap_id, INTRO_SCENE_ID scene );

extern void INTRO_MCSS_Exit( INTRO_MCSS_WORK* wk );

extern void INTRO_MCSS_Main( INTRO_MCSS_WORK* wk );

extern void INTRO_MCSS_Add( INTRO_MCSS_WORK* wk, fx32 px, fx32 py, fx32 pz, const MCSS_ADD_WORK* add, u8 id );

extern void INTRO_MCSS_AddPoke( INTRO_MCSS_WORK* wk, fx32 px, fx32 py, fx32 pz, int monsno, u8 id );

extern void INTRO_MCSS_SetVisible( INTRO_MCSS_WORK* wk, BOOL is_visible, u8 id );

extern void INTRO_MCSS_SetAnimeIndex( INTRO_MCSS_WORK* wk, u8 id, int anm_idx, BOOL anm_reset );

extern BOOL INTRO_MCSS_CheckAnime( INTRO_MCSS_WORK * wk, u8 id );

extern void INTRO_MCSS_ResetAnimeFlag( INTRO_MCSS_WORK * wk, u8 id );

extern void INTRO_MCSS_SetScale( INTRO_MCSS_WORK* wk, u8 id, VecFx32* scale );

extern void INTRO_MCSS_SetAlpha( INTRO_MCSS_WORK* wk, u8 id, u8 alpha );

extern void INTRO_MCSS_SetMepachi( INTRO_MCSS_WORK* wk, u8 id, BOOL is_mepachi_flag );

extern void INTRO_MCSS_SetPaletteFade( INTRO_MCSS_WORK* wk, u8 id, u8 start_evy, u8 end_evy, u8 wait, u32 rgb );

extern BOOL INTRO_MCSS_MoveX( INTRO_MCSS_WORK * wk, u8 id, fx32 mx, fx32 px );

extern BOOL INTRO_MCSS_PokeFall( INTRO_MCSS_WORK * wk, fx32 my, fx32 end );

extern void INTRO_MCSS_PokeAnime( INTRO_MCSS_WORK * wk );

