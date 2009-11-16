//============================================================================
/**
 *
 *	@file		pmsi_search.h
 *	@brief
 *	@author		hosaka genya
 *	@data		2009.11.14
 *
 */
//============================================================================
#pragma once

typedef struct _PMS_INPUT_SEARCH PMS_INPUT_SEARCH;

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================
enum
{ 
  PMS_INPUTWORD_MAX = 7,  ///< 入力できる最大文字数
};

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
extern PMS_INPUT_SEARCH* PMSI_SEARCH_Create( const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk, HEAPID heap_id );
extern void PMSI_SEARCH_Delete( PMS_INPUT_SEARCH* wk );
extern void PMSI_SEARCH_AddWord( PMS_INPUT_SEARCH* wk, u16 word_code );
extern BOOL PMSI_SEARCH_DelWord( PMS_INPUT_SEARCH* wk );
extern void PMSI_SEARCH_ClearWord( PMS_INPUT_SEARCH* wk );
extern void PMSI_SEARCH_SetInputWord( PMS_INPUT_SEARCH* wk, STRBUF* out_buf );
extern BOOL PMSI_SEARCH_Start( PMS_INPUT_SEARCH*wk );
extern u16 PMSI_SEARCH_GetResultCount( PMS_INPUT_SEARCH* wk );
extern STRBUF* PMSI_SEARCH_CreateResultString( PMS_INPUT_SEARCH* wk, u8 result_idx );


