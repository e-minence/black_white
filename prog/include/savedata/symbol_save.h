//==============================================================================
/**
 * @file    symbol_save.h
 * @brief   シンボルエンカウント用セーブデータ
 * @author  matsuda
 * @date    2010.01.06(水)
 */
//==============================================================================
#pragma once


//==============================================================================
//  型定義
//==============================================================================
typedef struct _SYMBOL_SAVE_WORK SYMBOL_SAVE_WORK;


//==============================================================================
//  外部関数宣言
//==============================================================================
extern u32 SymbolSave_GetWorkSize( void );
extern void SymbolSave_WorkInit(void *work);
extern void SymbolSave_Set(SYMBOL_SAVE_WORK *symbol_save, u16 monsno);

//----------------------------------------------------------
//	セーブデータ取得のための関数
//----------------------------------------------------------
extern SYMBOL_SAVE_WORK* SymbolSave_GetSymbolData(SAVE_CONTROL_WORK* pSave);
