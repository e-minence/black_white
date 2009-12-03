//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_data.c
 *	@brief  �f�[�^�o�b�t�@
 *	@author	Toru=Nagihashi
 *	@data		2009.12.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�O�����J
#include "wifibattlematch_data.h"


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
///	���[�N�@�s���S�^�Ƃ��Ă������݂��Ȃ�
//=====================================
/*
struct _WIFIBATTLEMATCH_DATA_WORK
{ 

};
*/
//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�o�b�t�@�쐬
 *
 *	@param	HEAPID heapID   �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_DATA_WORK *WIFIBATTLEMATCH_DATA_Init( HEAPID heapID )
{ 
  WIFIBATTLEMATCH_DATA_WORK *p_wk;
  u32 size;

  size  = WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE*2;
  p_wk  = GFL_HEAP_AllocMemory( heapID, size );
  GFL_STD_MemClear( p_wk, size );

  return p_wk;
} 
//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�o�b�t�@�j��
 *
 *	@param	WIFIBATTLEMATCH_DATA_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_DATA_Exit( WIFIBATTLEMATCH_DATA_WORK *p_wk )
{ 
  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̃f�[�^�̃|�C���^�擾
 *
 *	@param	WIFIBATTLEMATCH_DATA_WORK *p_wk ���[�N
 *
 *	@return �f�[�^�|�C���^
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_ENEMYDATA * WIFIBATTLEMATCH_DATA_GetPlayerDataPtr( WIFIBATTLEMATCH_DATA_WORK *p_wk )
{ 
  return (WIFIBATTLEMATCH_ENEMYDATA *)p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �G�f�[�^�̃|�C���^�擾
 *
 *	@param	WIFIBATTLEMATCH_DATA_WORK *p_wk ���[�N
 *
 *	@return �f�[�^�|�C���^
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_ENEMYDATA * WIFIBATTLEMATCH_DATA_GetEnemyDataPtr( WIFIBATTLEMATCH_DATA_WORK *p_wk )
{ 
  return (WIFIBATTLEMATCH_ENEMYDATA *)((u8*)p_wk + WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE);
}
