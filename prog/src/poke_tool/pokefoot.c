//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		pokefoot.c
 *	@brief		�|�P�������ՃO���t�B�b�N�f�[�^�擾�֐�
 *	@author		tomoya takahashi
 *	@data		2006.01.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "poke_tool/monsno_def.h"

#define	__POKEFOOT_H_GLOBAL
#include "poke_tool/pokefoot.h"

#include "arc_def.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------

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
 *	@brief	�����X�^�[�i���o�[���瑫�ՃL�����N�^�f�[�^�擾
 *
 *	@param	mons_no		�����X�^�[�i���o�[
 *	@param	heap		�q�[�v
 *
 *	@return	�t�@�C���f�[�^
 */
//-----------------------------------------------------------------------------
void* PokeFootCharPtrGet( int mons_no, HEAPID heap )
{	
	void* p_buff;
	
	GF_ASSERT( mons_no <= MONSNO_END );

	p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CHAR_DMMY + mons_no, TRUE, heap );

	// �擾���s
	GF_ASSERT( p_buff );

	return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���Ճp���b�g�f�[�^�擾
 *
 *	@param	heap	�q�[�v
 *
 *	@return	�t�@�C���f�[�^
 */
//-----------------------------------------------------------------------------
void* PokeFootPlttPtrGet( HEAPID heap )
{
	void* p_buff;
	
	p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_PLTT, FALSE, heap );

	// �擾���s
	GF_ASSERT( p_buff );

	return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ՃZ���f�[�^�擾
 *
 *	@param	heap	�q�[�v
 *
 *	@return	�t�@�C���f�[�^
 */
//-----------------------------------------------------------------------------
//void* PokeFootCellPtrGet( HEAPID heap )
void* PokeFootCellPtrGet( int mons_no, HEAPID heap )
{
	void* p_buff;

	GF_ASSERT( mons_no <= MONSNO_END );

	//p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELL, TRUE, heap );
  if( mons_no <= POKEFOOT_MONS_NO_OLD_MAX )
  {
    p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELL, TRUE, heap );
  }
  else
  {
    p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELL_NEW, TRUE, heap );
  }

	// �擾���s
	GF_ASSERT( p_buff );

	return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ՃZ���A�j���f�[�^�擾
 *
 *	@param	heap	�q�[�v
 *
 *	@return	�t�@�C���f�[�^
 */
//-----------------------------------------------------------------------------
//void* PokeFootCellAnmPtrGet( HEAPID heap )
void* PokeFootCellAnmPtrGet( int mons_no, HEAPID heap )
{
	void* p_buff;
	
	//p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELLANM, TRUE, heap );
  if( mons_no <= POKEFOOT_MONS_NO_OLD_MAX )
  {
	  p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELLANM, TRUE, heap );
  }
  else
  {
	  p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELLANM_NEW, TRUE, heap );
  }

	// �擾���s
	GF_ASSERT( p_buff );

	return p_buff;
}



// �A�[�J�C�u�t�@�C��IDX�@�f�[�^IDX
//----------------------------------------------------------------------------
/**
 *	@brief	�A�[�J�C�u�t�@�C��IDX�擾
 *
 *	@param	none	
 *
 *	@return	�A�[�J�C�u�t�@�C��IDX
 */
//-----------------------------------------------------------------------------
int PokeFootArcFileGet( void )
{
	return ARCID_POKEFOOT_GRA;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^�f�[�^IDX�擾
 *
 *	@param	mons_no		�����X�^�[�i���o�[
 *
 *	@return	�L�����N�^�f�[�^IDX
 */
//-----------------------------------------------------------------------------
int PokeFootCharDataIdxGet( int mons_no )
{
	return POKEFOOT_ARC_CHAR_DMMY + mons_no;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�f�[�^IDX�擾
 *
 *	@param	none
 *
 *	@return	�p���b�g�f�[�^IDX
 */
//-----------------------------------------------------------------------------
int PokeFootPlttDataIdxGet( void )
{
	return POKEFOOT_ARC_PLTT;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���f�[�^IDX�擾
 *
 *	@param	none
 *
 *	@return	�Z���f�[�^IDX
 */
//-----------------------------------------------------------------------------
//int PokeFootCellDataIdxGet( void )
int PokeFootCellDataIdxGet( int mons_no )
{
  if( mons_no <= POKEFOOT_MONS_NO_OLD_MAX )
  {
	  return POKEFOOT_ARC_CELL;
  }
  else
  {
	  return POKEFOOT_ARC_CELL_NEW;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�j���f�[�^IDX�擾
 *
 *	@param	none
 *
 *	@return	�Z���A�j���f�[�^IDX
 */
//-----------------------------------------------------------------------------
//int PokeFootCellAnmDataIdxGet( void )
int PokeFootCellAnmDataIdxGet( int mons_no )
{
  if( mons_no <= POKEFOOT_MONS_NO_OLD_MAX )
  {
	  return POKEFOOT_ARC_CELLANM;
  }
  else
  {
	  return POKEFOOT_ARC_CELLANM_NEW;
  }
}

