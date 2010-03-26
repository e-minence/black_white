//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_pokesearch.h
 *	@brief  �|�P��������
 *	@author	Toru=Nagihashi
 *	@date		2009.11.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "br_res.h"
#include "savedata/zukan_savedata.h"
#include "system/bmp_oam.h"
#include "br_fade.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�|�P���������I������
//=====================================
typedef enum
{
  BR_POKESEARCH_SELECT_NONE,
  BR_POKESEARCH_SELECT_CANCEL,
  BR_POKESEARCH_SELECT_DECIDE,
} BR_POKESEARCH_SELECT;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�|�P�����������[�N
//=====================================
typedef struct _BR_POKESEARCH_WORK BR_POKESEARCH_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
extern BR_POKESEARCH_WORK *BR_POKESEARCH_Init( const ZUKAN_SAVEDATA *cp_zkn, BR_RES_WORK *p_res, GFL_CLUNIT *p_unit,  BMPOAM_SYS_PTR p_bmpoam, BR_FADE_WORK *p_fade, HEAPID heapID );
extern void BR_POKESEARCH_Exit( BR_POKESEARCH_WORK *p_wk );
extern void BR_POKESEARCH_Main( BR_POKESEARCH_WORK *p_wk );
extern void BR_POKESEARCH_StartUp( BR_POKESEARCH_WORK *p_wk );
extern void BR_POKESEARCH_CleanUp( BR_POKESEARCH_WORK *p_wk );
extern BR_POKESEARCH_SELECT BR_POKESEARCH_GetSelect( const BR_POKESEARCH_WORK *cp_wk, u16 *p_mons_no );
