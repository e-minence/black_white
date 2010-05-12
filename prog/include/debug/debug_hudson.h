//============================================================================
/**
 *
 *	@file		debug_hudson.h
 *	@brief  hudson用ユーティリティ
 *	@author		hosaka genya
 *	@data		2009.11.25
 *
 */
//============================================================================
#pragma once

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

#ifdef DEBUG_ONLY_FOR_hudson

//---------------------------------
// HUDSONのみ実体を定義
//---------------------------------

// HUDSON用アサート
#define HUDSON_ASSERT(exp)  GF_ASSERT(exp)
// PRINT関数
#define HUDSON_Printf(...)  OS_Printf(__VA_ARGS__)
// PRINT関数(軽量版)
#define HUDSON_TPrintf(...) OS_TPrintf(__VA_ARGS__)

#else // DEBUG_ONLY_FOR_hudson

//---------------------------------
// 無効化
//---------------------------------
#define HUDSON_ASSERT(exp) ((void)0)
#define HUDSON_TPrintf(...) ((void)0)
#define HUDSON_Printf(...) ((void)0)

#endif // DEBUG_ONLY_FOR_hudson


