//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_const.h
 *	@brief	�t�B�[���h�O�����J�萔
 *	@author	...
 *	@date		2009.07.23
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					�O���b�h���P��(UNIT)
*/
//-----------------------------------------------------------------------------
#define FIELD_CONST_GRID_SIZE					(16)
#define FIELD_CONST_GRID_FX32_SIZE		(16<<FX32_SHIFT)
#define FIELD_CONST_GRID_HALF_SIZE	  (8)
#define FIELD_CONST_GRID_HALF_FX32_SIZE	  (8<<FX32_SHIFT)

#define GRID_TO_FX32( grid )  ( (grid * FIELD_CONST_GRID_SIZE) << FX32_SHIFT )
#define FX32_TO_GRID( fx_val )  ( FX_Whole(fx_val) / FIELD_CONST_GRID_SIZE )

#ifdef _cplusplus
}	// extern "C"{
#endif



