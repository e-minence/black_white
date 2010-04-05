//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		c_gear_pattern.c
 *	@brief  C Gear �����@�p�^�[���@�Ǘ�
 *	@author	tomoya takahashi
 *	@date		2010.04.05
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "c_gear.naix"

#include "c_gear_pattern.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

#define CGEAR_PATTERN_DATA_SIZE ( sizeof(u8) * (C_GEAR_PANEL_WIDTH * C_GEAR_PANEL_HEIGHT))

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  pattern�̏�ԂɕύX
 *
 *	@param	gamedata    �Q�[���f�[�^
 *	@param	pattern     pattern
 *	@param	heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
void CGEAR_PATTERN_SetUp( CGEAR_SAVEDATA* pCGSV, ARCHANDLE* handle, u32 pattern, HEAPID heapID )
{
  GF_ASSERT( pattern < CGEAR_PATTERN_MAX );

  GFL_ARC_LoadDataByHandle( handle, pattern+NARC_c_gear_pattern01_bin, CGEAR_SV_GetPanelTypeBuff( pCGSV ) );
}

