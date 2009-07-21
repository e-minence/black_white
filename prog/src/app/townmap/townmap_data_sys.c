//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		townmap_data_sys.c
 *	@brief	�^�E���}�b�v�f�[�^�ǂݍ���
 *	@author	Toru=Nagihashi
 *	@data		2009.07.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>

//archive
#include "arc_def.h"
#include "townmap_data.naix"

//mine
#include "townmap_data_sys.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�f�[�^�{��
//=====================================
typedef struct 
{
	u16	param[TOWNMAP_PARAM_MAX];
} TOWNMAP_DATA_STRUCT;



//-------------------------------------
///	�^�E���}�b�v�f�[�^
//=====================================
/*
typedef struct _TOWNMAP_DATA TOWNMAP_DATA;
*/

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^�A���P�[�g
 *
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	�}�b�v�f�[�^�n���h��
 */
//-----------------------------------------------------------------------------
TOWNMAP_DATA *TOWNMAP_DATA_Alloc( HEAPID heapID )
{	
	void *p_handle;
	p_handle	= GFL_ARC_UTIL_Load( ARCID_TOWNMAP_DATA, 
			NARC_townmap_data_townmap_data_dat, FALSE, heapID );

	return p_handle;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^���
 *
 *	@param	TOWNMAP_DATA *p_wk	�}�b�v�f�[�^�n���h��
 */
//-----------------------------------------------------------------------------
void TOWNMAP_DATA_Free( TOWNMAP_DATA *p_wk )
{	
	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^���擾
 *
 *	@param	const		TOWNMAP_DATA *cp_wk	�}�b�v�f�[�^�n���h��
 *	@param	idx			�ꏊ�C���f�b�N�X�iMAX�@TOWNMAP_DATA_MAX�j
 *	@param	param		TOWNMAP_PARAM��
 *
 *	@return	�l
 */
//-----------------------------------------------------------------------------
u16 TOWNMAP_DATA_GetParam( const TOWNMAP_DATA *cp_wk, u16 idx, TOWNMAP_PARAM param )
{	
	GF_ASSERT( cp_wk );
	GF_ASSERT( idx < TOWNMAP_DATA_MAX );
	GF_ASSERT( param < TOWNMAP_PARAM_MAX );

	{	
		const TOWNMAP_DATA_STRUCT *cp_data;
		cp_data	= (const TOWNMAP_DATA_STRUCT*)((const u8*)(cp_wk) + sizeof(TOWNMAP_DATA_STRUCT) * idx);

		return cp_data->param[ param ];
	}
}
