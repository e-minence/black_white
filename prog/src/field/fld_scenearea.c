//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fld_scenearea.c
 *	@brief  �t�B�[���h�@����V�[���̈�Ǘ�
 *	@author	tomoya takahashi
 *	@date		2009.05.22
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
  const FLD_SCENEAREA_FUNC* cp_func;
  u32 datanum;

  // �N���t���O
  u32 active_area;

	// ����t���O
	u32 active_flag;


	// �R�[���֐�ID
	u32 call_funcID;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static BOOL FUNC_IsAreaCheckFunc( const FLD_SCENEAREA_FUNC* cp_func, u32 id );
static BOOL FUNC_IsUpdateFunc( const FLD_SCENEAREA_FUNC* cp_func, u32 id );

static BOOL FUNC_CallAreaCheckFunc( const FLD_SCENEAREA_FUNC* cp_func, u32 id, const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void FUNC_CallUpdateFunc( const FLD_SCENEAREA_FUNC* cp_func, u32 id, const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );

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

  p_sys->active_flag = TRUE;

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
void FLD_SCENEAREA_Load( FLD_SCENEAREA* p_sys, const FLD_SCENEAREA_DATA* cp_data, u32 datanum, const FLD_SCENEAREA_FUNC* cp_func )
{
  // ��񂪂��邩������Ȃ��̂�Release
  FLD_SCENEAREA_Release( p_sys );

  p_sys->cp_data = cp_data;
  p_sys->cp_func = cp_func;
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
	p_sys->cp_func			= NULL;
  p_sys->datanum      = 0;
  p_sys->active_area  = FLD_SCENEAREA_ACTIVE_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�[���G���A�@�X�V
 *
 *	@param	p_sys     �V�X�e��
 *	@param	cp_pos    ���@�ʒu
 *
 *	@retval FLD_SCENEAREA_UPDATE_NONE �X�V�Ȃ�
 *	@retval ���̑�                    �X�V�����f�[�^�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 FLD_SCENEAREA_Update( FLD_SCENEAREA* p_sys, const VecFx32* cp_pos )
{
  int i;
  u32 now_active;
	BOOL result;

  if( !p_sys->active_flag )
  {
    return FLD_SCENEAREA_UPDATE_NONE;
  }

	p_sys->call_funcID = FLD_SCENEAREA_FUNC_NULL;
  
  // �G���A�`�F�b�N
  now_active = FLD_SCENEAREA_ACTIVE_NONE;
  for( i=0; i<p_sys->datanum; i++ ){
    
    // �`�F�b�N
		result = FUNC_CallAreaCheckFunc( p_sys->cp_func, p_sys->cp_data[i].checkArea_func,
				p_sys, &p_sys->cp_data[i], cp_pos );
    if( result ){

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

			FUNC_CallUpdateFunc( p_sys->cp_func, 
					p_sys->cp_data[now_active].inside_func, 
					p_sys, &p_sys->cp_data[now_active], cp_pos );
			p_sys->call_funcID = p_sys->cp_data[now_active].inside_func;
    }

    if( p_sys->active_area != FLD_SCENEAREA_ACTIVE_NONE ){

			FUNC_CallUpdateFunc( p_sys->cp_func, 
					p_sys->cp_data[p_sys->active_area].outside_func, 
					p_sys, &p_sys->cp_data[p_sys->active_area], cp_pos );
			p_sys->call_funcID = p_sys->cp_data[p_sys->active_area].outside_func;
    }

    p_sys->active_area = now_active;
  }
  else if( p_sys->active_area != FLD_SCENEAREA_ACTIVE_NONE )
  {

    // �X�V����
		FUNC_CallUpdateFunc( p_sys->cp_func, 
				p_sys->cp_data[p_sys->active_area].update_func, 
				p_sys, &p_sys->cp_data[p_sys->active_area], cp_pos );
		p_sys->call_funcID = p_sys->cp_data[p_sys->active_area].update_func;
  }

  return p_sys->active_area;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���A�@�ǂݍ��ݍς݂��H
 *
 *	@param	cp_sys  �V�X�e��
 *
 *	@retval TRUE  �ǂݍ��ݍς�* 
 *	@retval FLASE �ǂݍ���łȂ�
 */
//-----------------------------------------------------------------------------
BOOL FLD_SCENEAREA_IsLoad( const FLD_SCENEAREA* cp_sys )
{
  GF_ASSERT( cp_sys );
  if( cp_sys->datanum == 0 )
  {
    return FALSE;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�N�e�B�u�G���A�̎擾
 *
 *	@param	cp_sys  �V�X�e��
 *
 *	@retval �A�N�e�B�u�G���A
 *  @retval FLD_SCENEAREA_ACTIVE_NONE �Ȃ�
 */
//-----------------------------------------------------------------------------
u32 FLD_SCENEAREA_GetActiveArea( const FLD_SCENEAREA* cp_sys )
{
  GF_ASSERT( cp_sys );
  return cp_sys->active_area;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�V�����Ŏg�p�����֐���ID�̎擾
 *
 *	@param	cp_sys	�V�X�e��
 *
 *	@retval	�֐�ID
 *	@retval	FLD_SCENEAREA_FUNC_NULL	�����R�[�����ĂȂ�
 */
//-----------------------------------------------------------------------------
u32 FLD_SCENEAREA_GetUpdateFuncID( const FLD_SCENEAREA* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->call_funcID;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�N�e�B�u�t���O�ݒ�
 *
 *	@param	p_sys		�V�X�e��
 *	@param	flag		�t���O
 */	
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_SetActiveFlag( FLD_SCENEAREA* p_sys, BOOL flag )
{
	GF_ASSERT( p_sys );
	p_sys->active_flag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�N�e�B�u�t���O�̎擾
 *
 *	@param	cp_sys	�V�X�e��
 *
 *	@retval	TRUE	
 *	@retval	FALSE	
 */
//-----------------------------------------------------------------------------
BOOL FLD_SCENEAREA_GetActiveFlag( const FLD_SCENEAREA* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->active_flag;
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




//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	�G���A�`�F�b�N�֐��@�����邩�`�F�b�N
 *
 *	@param	cp_func	�֐����
 *	@param	id			ID
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FUNC_IsAreaCheckFunc( const FLD_SCENEAREA_FUNC* cp_func, u32 id )
{
	GF_ASSERT( cp_func );
	if( cp_func->checkarea_count > id )
	{
		if( cp_func->cpp_checkArea[ id ] )
		{
			return TRUE;
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ʏ�A�b�v�f�[�g�֐��@�����邩�`�F�b�N
 *
 *	@param	cp_func	�֐����
 *	@param	id			ID
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FUNC_IsUpdateFunc( const FLD_SCENEAREA_FUNC* cp_func, u32 id )
{
	GF_ASSERT( cp_func );
	if( cp_func->update_count > id )
	{
		if( cp_func->cpp_update[ id ] )
		{
			return TRUE;
		}
	}
	return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	�G���A�`�F�b�N�֐��R�[��
 */
//-----------------------------------------------------------------------------
static BOOL FUNC_CallAreaCheckFunc( const FLD_SCENEAREA_FUNC* cp_func, u32 id, const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
	if( FUNC_IsAreaCheckFunc( cp_func, id ) )
	{
		return cp_func->cpp_checkArea[ id ]( cp_sys, cp_data, cp_pos );
	}

	// ����
	// �G���A�`�F�b�N�֐��͂���ׂ�
	GF_ASSERT_MSG( 0, "�G���A�`�F�b�N�֐��͂���ׂ��B" );
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�b�v�f�[�g
 */	
//-----------------------------------------------------------------------------
static void FUNC_CallUpdateFunc( const FLD_SCENEAREA_FUNC* cp_func, u32 id, const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
	if( FUNC_IsUpdateFunc( cp_func, id ) )
	{
		cp_func->cpp_update[ id ]( cp_sys, cp_data, cp_pos );
	}
}




