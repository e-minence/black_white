//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fld_scenearea.c
 *	@brief  �t�B�[���h�@����V�[���̈�Ǘ�
 *	@author	tomoya takahashi
 *	@data		2009.05.22
 *
 *	���W���[�����FFLD_SCENEAREA
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "fld_scenearea.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
#define FLD_SCENEAREA_ACTIVE_NONE (0xffffffff)

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�[���G���A
//=====================================
struct _FLD_SCENEAREA {
  // �J����
  FIELD_CAMERA* p_camera;
  
  // �f�[�^
  const FLD_SCENEAREA_DATA* cp_data;
  u32 datanum;

  // �N���t���O
  u32 active_area;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  �V�[���G���A�@�쐬
 *
 *	@param	heapID      �q�[�vID
 *	@param	p_camera    �J�������[�N
 *
 *	@return �V�[���G���A�Ǘ��V�X�e��
 */
//-----------------------------------------------------------------------------
FLD_SCENEAREA* FLD_SCENEAREA_Create( u32 heapID, FIELD_CAMERA * p_camera )
{
  FLD_SCENEAREA* p_sys;

  p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_SCENEAREA) );

  p_sys->p_camera = p_camera;

  // �A�N�e�B�u�̈�Ȃ�
  p_sys->active_area = FLD_SCENEAREA_ACTIVE_NONE;

  return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̔j��
 *
 *	@param	p_sys   �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_Delete( FLD_SCENEAREA* p_sys )
{
  FLD_SCENEAREA_Release( p_sys );
  GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �V�[���G���A  ���ݒ�
 *
 *	@param	p_sys     �V�X�e�����[�N
 *	@param	cp_data   �V�[���G���A���
 *	@param	datanum   �G���A��
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_Load( FLD_SCENEAREA* p_sys, const FLD_SCENEAREA_DATA* cp_data, u32 datanum )
{
  // ��񂪂��邩������Ȃ��̂�Release
  FLD_SCENEAREA_Release( p_sys );

  p_sys->cp_data = cp_data;
  p_sys->datanum = datanum;

  // �A�N�e�B�u�̈�Ȃ�
  p_sys->active_area = FLD_SCENEAREA_ACTIVE_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�[���G���A  ���j��
 *
 *	@param	p_sys �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_Release( FLD_SCENEAREA* p_sys )
{
  p_sys->cp_data      = NULL;
  p_sys->datanum      = 0;
  p_sys->active_area  = FLD_SCENEAREA_ACTIVE_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�[���G���A�@�X�V
 *
 *	@param	p_sys     �V�X�e��
 *	@param	cp_pos    ���@�ʒu
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_Updata( FLD_SCENEAREA* p_sys, const VecFx32* cp_pos )
{
  int i;
  u32 now_active;
  
  // �G���A�`�F�b�N
  now_active = FLD_SCENEAREA_ACTIVE_NONE;
  for( i=0; i<p_sys->datanum; i++ ){
    
    // �`�F�b�N
    GF_ASSERT( p_sys->cp_data[i].p_checkArea );
    if( p_sys->cp_data[i].p_checkArea( p_sys, &p_sys->cp_data[i], cp_pos ) ){

      // �G���A�����Ԃ��Ă��܂��B
      GF_ASSERT_MSG( now_active == FLD_SCENEAREA_ACTIVE_NONE, "�G���A�͈͂����Ԃ��Ă��܂��B�C���f�b�N�X%d,%d\n", now_active, i );
      now_active = i;
    }
  }

  // �G���A�C���f�b�N�X�̕ύX���`�F�b�N
  // ���́A�G���A���ς�����Ƃ��͍X�V�������Ă΂Ȃ��悤�ɂ��Ă��܂��B
  // �ĂԂ��Ă΂Ȃ����I�ׂ�悤�ɂ��邱�Ƃ��l���Ă��܂��B
  if( now_active != p_sys->active_area )
  {
    if( now_active != FLD_SCENEAREA_ACTIVE_NONE ){
      if( p_sys->cp_data[now_active].p_inside ){
        p_sys->cp_data[now_active].p_inside( p_sys, &p_sys->cp_data[now_active], cp_pos );
      }
    }

    if( p_sys->active_area != FLD_SCENEAREA_ACTIVE_NONE ){
      if( p_sys->cp_data[p_sys->active_area].p_outside ){
        p_sys->cp_data[p_sys->active_area].p_outside( p_sys, &p_sys->cp_data[p_sys->active_area], cp_pos );
      }
    }

    p_sys->active_area = now_active;
  }
  else if( p_sys->active_area != FLD_SCENEAREA_ACTIVE_NONE )
  {

    // �X�V����
    if( p_sys->cp_data[p_sys->active_area].p_updata ){
      p_sys->cp_data[p_sys->active_area].p_updata( p_sys, &p_sys->cp_data[p_sys->active_area], cp_pos );
    }
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�J�����̎擾
 *
 *	@param	cp_sys  �V�X�e��
 *
 *	@return �t�B�[���h�J����
 */
//-----------------------------------------------------------------------------
FIELD_CAMERA* FLD_SCENEAREA_GetFieldCamera( const FLD_SCENEAREA* cp_sys )
{
  return cp_sys->p_camera;
}
