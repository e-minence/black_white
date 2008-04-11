//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		field_hblank.h
 *	@brief		フィールドHBlankシステム
 *	@author		tomoya takahashi
 *	@data		2006.04.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELD_HBLANK_H__
#define __FIELD_HBLANK_H__

#undef GLOBAL
#ifdef	__FIELD_HBLANK_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	状態定数
//=====================================
enum{
	FIELD_HBLANK_STATE_STANDBY,
	FIELD_HBLANK_STATE_DO,
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	Hブランク期間【管理システム】
//=====================================
typedef struct _FIELD_HBLANK_SYS FIELD_HBLANK_SYS;

//-------------------------------------
//	Hブランク期間【オブジェクト】
//=====================================
typedef struct _FIELD_HBLANK_OBJ FIELD_HBLANK_OBJ;

//-------------------------------------
//	Hブランク期間コールバック関数　型
//=====================================
typedef void (*FLDHBLANK_FUNC)( FIELD_HBLANK_OBJ* p_hbw, void* p_work );
//-----------------------------------------------------------------------------
/**
 *					【管理システム】プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	【管理システム】初期化関数
 *	@param	heap 
 *	@return	作成した【管理システム】
 */
//-----------------------------------------------------------------------------
GLOBAL FIELD_HBLANK_SYS* FLDHBLANK_SYS_Init( u32 heap );
//----------------------------------------------------------------------------
/**
 *	@brief	【管理システム】破棄処理
 *	@param	p_sys	【管理システム】
 */
//-----------------------------------------------------------------------------
GLOBAL void FLDHBLANK_SYS_Delete( FIELD_HBLANK_SYS* p_sys );
//----------------------------------------------------------------------------
/**
 *	@brief	【管理システム】HBlank処理	スタート　HBLANK割り込み設定
 *	@param	p_sys	【管理システム】
 */
//-----------------------------------------------------------------------------
GLOBAL void FLDHBLANK_SYS_Start( FIELD_HBLANK_SYS* p_sys );
//----------------------------------------------------------------------------
/**
 *	@brief	【管理システム】HBlank処理終了	HBLANK割り込みの解除
 *	@param	p_sys	【管理システム】
 */
//-----------------------------------------------------------------------------
GLOBAL void FLDHBLANK_SYS_Stop( FIELD_HBLANK_SYS* p_sys );
//----------------------------------------------------------------------------
/**
 *	@brief	【管理システム】の状態を取得
 *	@param	cp_sys	【管理システム】
 *
 *	@return	FIELD_HBLANK_STATE_STANDBY	開始待ち
 *	@return	FIELD_HBLANK_STATE_DO		実行中
 */
//-----------------------------------------------------------------------------
GLOBAL u32 FLDHBLANK_SYS_GetState( const FIELD_HBLANK_SYS* cp_sys );


//-----------------------------------------------------------------------------
/**
 *					【オブジェクト】プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	【オブジェクト】登録
 *	@param	p_sys		【管理システム】
 *	@param	func		コールバック関数
 *	@param	p_work		ワーク
 *
 *	@return	作成された【オブジェクト】	NULLの時失敗（最大数登録済み）
 */
//-----------------------------------------------------------------------------
GLOBAL FIELD_HBLANK_OBJ* FLDHBLANK_OBJ_Add( FIELD_HBLANK_SYS* p_sys, FLDHBLANK_FUNC func, void* p_work );
//----------------------------------------------------------------------------
/**
 *	@brief	【オブジェクト】破棄
 *	@param	p_obj	【オブジェクト】
 */
//-----------------------------------------------------------------------------
GLOBAL void FLDHBLANK_OBJ_Delete( FIELD_HBLANK_OBJ* p_obj );


#undef	GLOBAL
#endif		// __FIELD_HBLANK_H__

