//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		dpw_rap.h
 *	@brief  DPWライブラリを共通で使うためのソース
 *	@author	Toru=Nagihashi
 *	@date		2010.02.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <dpw_tr.h>             //Dpw_Common_Profile
#include "savedata/mystatus.h"  //MYSTATUS

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
 *					外部公開
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  Dpw_Common_Profileを作成
 *
 *	@param	*p_dc_profile         作成したPROFILE受け取り
 *	@param	MYSTATUS *p_mystatus  PROFILE作成のために必要なデータ
 */
//-----------------------------------------------------------------------------
extern void DPW_RAP_CreateProfile( Dpw_Common_Profile *p_dc_profile, const MYSTATUS *cp_mystatus );
