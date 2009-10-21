//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fieldmap_ctrl_nogrid_work.h
 *	@brief  �t�B�[���h�}�b�v�R���g���[���@�m�[�O���b�h�@���ʃ��[�N
 *	@author	tomoya takahashi
 *	@date		2009.08.28
 *
 *	���W���[�����FFIELDMAP_CTRL_NOGRID_WORK
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>


#include "fieldmap_ctrl_nogrid_work.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�B�[���h�}�b�v�R���g���[���@�m�[�O���b�h�}�b�v�@���ʃ��[�N
//=====================================
struct _FIELDMAP_CTRL_NOGRID_WORK 
{
  FIELD_PLAYER_NOGRID* p_player_work;

  void* p_localwork;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N����
 *
 *  @param  p_fieldwork �t�B�[���h���[�N
 *	@param	heapID      �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
FIELDMAP_CTRL_NOGRID_WORK* FIELDMAP_CTRL_NOGRID_WORK_Create( FIELDMAP_WORK* p_fieldwork, HEAPID heapID )
{
  FIELDMAP_CTRL_NOGRID_WORK* p_wk;
  FIELD_PLAYER* p_player;
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_fieldwork );

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELDMAP_CTRL_NOGRID_WORK) );

  // �t�B�[���h�v���C���[�̎擾
  p_player = FIELDMAP_GetFieldPlayer( p_fieldwork );

  // �m�[�O���b�h�v���C���[�̐���
  p_wk->p_player_work = FIELD_PLAYER_NOGRID_Create( p_player, heapID );


  //���[���̃o�C���h
  FLDNOGRID_MAPPER_BindCameraWork( p_mapper, FIELD_PLAYER_NOGRID_GetRailWork( p_wk->p_player_work ) );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELDMAP_CTRL_NOGRID_WORK_Delete( FIELDMAP_CTRL_NOGRID_WORK* p_wk )
{
  // ���[�J�����[�N�̃t���[���s���Ă��邩�`�F�b�N
  GF_ASSERT( p_wk->p_localwork == NULL );

  // �m�[�O���b�h�v���C���[�̔j��
  FIELD_PLAYER_NOGRID_Delete( p_wk->p_player_work );

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h�}�b�v���[�N�@���C������
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELDMAP_CTRL_NOGRID_WORK_Main( FIELDMAP_CTRL_NOGRID_WORK* p_wk )
{
  // ���@����
  FIELD_PLAYER_NOGRID_Move( p_wk->p_player_work, GFL_UI_KEY_GetTrg(), GFL_UI_KEY_GetCont() );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h���[�N�@���[�J�����[�N�̊m��
 *
 *	@param	p_wk        ���[�N
 *	@param	heapID      �q�[�vID
 *	@param	size        �T�C�Y
 *
 *	@return ���[�J�����[�N�m��
 */
//-----------------------------------------------------------------------------
void* FIELDMAP_CTRL_NOGRID_WORK_AllocLocalWork( FIELDMAP_CTRL_NOGRID_WORK* p_wk, HEAPID heapID, u32 size )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( p_wk->p_localwork == NULL );

  p_wk->p_localwork = GFL_HEAP_AllocClearMemory( heapID, size );
  return p_wk->p_localwork;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h���[�N  ���[�J�����[�N�̔j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELDMAP_CTRL_NOGRID_WORK_FreeLocalWork( FIELDMAP_CTRL_NOGRID_WORK* p_wk )
{
  GF_ASSERT( p_wk );

  if( p_wk->p_localwork )
  {
    GFL_HEAP_FreeMemory( p_wk->p_localwork );
    p_wk->p_localwork = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h���[�N  ���[�J�����[�N�̎擾
 *
 *	@param	cp_wk   ���[�N
 *  
 *	@return ���[�J�����[�N
 */
//-----------------------------------------------------------------------------
void* FIELDMAP_CTRL_NOGRID_WORK_GetLocalWork( const FIELDMAP_CTRL_NOGRID_WORK* cp_wk )
{
  GF_ASSERT( cp_wk );

  return cp_wk->p_localwork;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h���[�N�@�v���C���[���[�N�̎擾
 *
 *	@param	cp_wk   ���[�N
 *  
 *	@return �m�[�O���b�h�v���C���[���[�N
 */
//-----------------------------------------------------------------------------
FIELD_PLAYER_NOGRID* FIELDMAP_CTRL_NOGRID_WORK_GetNogridPlayerWork( const FIELDMAP_CTRL_NOGRID_WORK* cp_wk )
{
  GF_ASSERT( cp_wk );

  return cp_wk->p_player_work;
}

