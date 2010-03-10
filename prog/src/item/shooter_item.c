//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		shooter_item.c
 *	@brief  �V���[�^�[�A�C�e��
 *	@author	Toru=Nagihashi
 *	@data		2010.03.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�f�[�^
#include "item/shooter_item_data.h" //cdat�̒���MAX���g�p���Ă���̂ł�����O�ɃC���N���[�h
#include "shooter_item_data.cdat"

//�O�����J
#include "item/shooter_item.h"

SDK_COMPILER_ASSERT( SHOOTER_ITEM_MAX/8 <= SHOOTER_ITEM_BIT_TBL_MAX );

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

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================


//----------------------------------------------------------------------------
/**
 *	@brief  �V���[�^�[���g�p�\���`�F�b�N
 *
 *	@param	const SHOOTER_ITEM_BIT_WORK *cp_wk
 *	@param	shooter_item 
 *
 *	@return TRUE�Ŏg�p�\  FALSE�Ŏg�p�s�\
 */
//-----------------------------------------------------------------------------
BOOL SHOOTER_ITEM_IsUse( const SHOOTER_ITEM_BIT_WORK *cp_wk, u32 shooter_item )
{ 
  const u32 bit     = 1<<shooter_item;
  const u8 bit_idx  = bit / 8;
  const u8 bit_pos  = bit % 8;

  return (cp_wk->bit_tbl[ bit_idx ] & bit_pos ) != 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V���[�^�[�A�C�e���̃C���f�b�N�X����A�C�e���֕ϊ�
 *
 *	@param	u32 shooter_item  �V���[�^�[�A�C�e���̃C���f�b�N�X
 *
 *	@return �A�C�e���ԍ�
 */
//-----------------------------------------------------------------------------
u32  SHOOTER_ITEM_ShooterIndexToItemIndex( u32 shooter_item )
{ 
  GF_ASSERT( shooter_item < SHOOTER_ITEM_MAX );
  return sc_shooter_item_to_item[ shooter_item ];
}
