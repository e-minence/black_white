//==============================================================================
/**
 * @file    intrude_subdisp.h
 * @brief   侵入下画面のヘッダ
 * @author  matsuda
 * @date    2009.10.17(土)
 */
//==============================================================================
#pragma once


//==============================================================================
//  型定義
//==============================================================================
typedef struct _INTRUDE_SUBDISP * INTRUDE_SUBDISP_PTR;


//==============================================================================
//  外部関数宣言
//==============================================================================
extern INTRUDE_SUBDISP_PTR INTRUDE_SUBDISP_Init(GAMESYS_WORK *gsys);
extern void INTRUDE_SUBDISP_Exit(INTRUDE_SUBDISP_PTR unisub);
extern void INTRUDE_SUBDISP_Update(INTRUDE_SUBDISP_PTR unisub);
extern void INTRUDE_SUBDISP_Draw(INTRUDE_SUBDISP_PTR unisub);

