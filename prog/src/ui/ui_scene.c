//=============================================================================
/**
 *
 *	@file		ui_scene.c
 *	@brief	�V�[���Ǘ����W���[��
 *	@author	hosaka genya
 *	@data		2009.07.01
 *
 */
//=============================================================================
//	2009.07.16 : ���C���I������Cleanup���Ă΂�Ă��Ȃ��ƃT�u�V�[�P���X������������Ȃ������s����C��

#include <gflib.h>
#include "ui_scene.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{	
	SCENE_SEQ_INIT = 0,
	SCENE_SEQ_SET_UP,
	SCENE_SEQ_MAIN,
	SCENE_SEQ_END,
	SCENE_SEQ_SET_NEXT,
};

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	�V�[���Ǘ����[�N
//==============================================================
typedef struct UI_SCENE_CNT_tag{
	const UI_SCENE_FUNC_SET* func_set_tbl;
	void* work;
	u16 scene_id;
	u16 scene_id_next;
	u8 scene_max;
	u8 seq;
	u8 sub_seq;
	u8 padding[1];
} UI_SCENE_CNT_WORK;

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief	�V�[���Ǘ����W���[�� ����
 *
 *	@param	HEAPID heapID	�q�[�vID
 *	@param	UI_SCENE_FUNC_SET* func_set_tbl	�V�[���e�[�u��
 *	@param	scene_max	�V�[����
 *	@param	work		�V�[���S�̂ŎQ�Ƃ��郏�[�N
 *
 *	@retval	�����������[�N
 */
//-----------------------------------------------------------------------------
UI_SCENE_CNT_PTR UI_SCENE_CNT_Create( HEAPID heapID, const UI_SCENE_FUNC_SET* func_set_tbl, int scene_max, void* work )
{	
	UI_SCENE_CNT_PTR ptr;

	ptr = GFL_HEAP_AllocMemory( heapID, sizeof( UI_SCENE_CNT_WORK ) );
	GFL_STD_MemClear( ptr, sizeof( UI_SCENE_CNT_WORK ) );

	// �����o������
	ptr->func_set_tbl	  = func_set_tbl;
	ptr->scene_max	  	= scene_max;
	ptr->work		      	= work;
	ptr->scene_id_next  = UI_SCENE_ID_NULL;

	return ptr;
}

//-----------------------------------------------------------------------------
/**
 *	@brief	�V�[���Ǘ����W���[�� �폜
 *
 *	@param	UI_SCENE_CNT_PTR ptr 
 *
 *	@retval	none
 */
//-----------------------------------------------------------------------------
void UI_SCENE_CNT_Delete( UI_SCENE_CNT_PTR ptr )
{	
	GFL_HEAP_FreeMemory( ptr );
}

//-----------------------------------------------------------------------------
/**
 *	@brief	�V�[���Ǘ����W���[�� �又��
 *
 *	@param	UI_SCENE_CNT_PTR ptr 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL UI_SCENE_CNT_Main( UI_SCENE_CNT_PTR ptr )
{
	const UI_SCENE_FUNC_SET* func_set_tbl;
	
	func_set_tbl = &ptr->func_set_tbl[ ptr->scene_id ];

	switch( ptr->seq )
	{	
		// ����������
		case SCENE_SEQ_INIT :
			if(	func_set_tbl->Init == NULL )
			{	
				OS_TPrintf("scene Id:%d Init Skip\n", ptr->scene_id );
				ptr->seq = SCENE_SEQ_SET_UP;
			}
			else
			{	
				if( func_set_tbl->Init( ptr, ptr->work ) )
				{	
					ptr->sub_seq = 0;
					OS_TPrintf("scene Id:%d Init End\n", ptr->scene_id );
					ptr->seq = SCENE_SEQ_SET_UP;
				}
				break;
			}
			/* FALLTHROW */

		// �O����
		case SCENE_SEQ_SET_UP :
			if( func_set_tbl->SetUp != NULL )
			{	
				OS_TPrintf("scene Id:%d Call Setup\n", ptr->scene_id );
				func_set_tbl->SetUp( ptr, ptr->work );
			}
			ptr->seq = SCENE_SEQ_MAIN;
			/* FALLTHROW */

		// �又��
		case SCENE_SEQ_MAIN :
			// �又���Ȃ��͋����Ȃ� 
			GF_ASSERT( func_set_tbl->Main != NULL );

			if( func_set_tbl->Main( ptr, ptr->work ) )
			{	
				OS_TPrintf("scene Id:%d Main End\n", ptr->scene_id );

				ptr->sub_seq = 0;
				
				// �㏈��
				if( func_set_tbl->CleanUp != NULL )
				{	
					OS_TPrintf("scene Id:%d Call CleanUp\n", ptr->scene_id );
					func_set_tbl->CleanUp( ptr, ptr->work );
				}

				ptr->seq = SCENE_SEQ_END;
			}
			break;

		// �I������
		case SCENE_SEQ_END :
			if( func_set_tbl->Exit == NULL )
			{	
				OS_TPrintf("scene Id:%d Exit Skip\n", ptr->scene_id );
				ptr->seq = SCENE_SEQ_SET_NEXT;
			}
			else
			{	
				if( func_set_tbl->Exit( ptr, ptr->work ) )
				{	
					ptr->sub_seq = 0;
					OS_TPrintf("scene Id:%d Exit End\n", ptr->scene_id );
					ptr->seq = SCENE_SEQ_SET_NEXT;
				}
				break;
			}
			/* FALLTHROW */

		// ���̃V�[�����Z�b�g
		case SCENE_SEQ_SET_NEXT : 
			GF_ASSERT( ptr->scene_id_next != UI_SCENE_ID_NULL );

			ptr->scene_id		= ptr->scene_id_next;
			ptr->scene_id_next	= UI_SCENE_ID_NULL; 

			if( ptr->scene_id == UI_SCENE_ID_END )
			{	
				// �V�[���Ǘ��I��
				return TRUE;
			}
			else
			{	
				// ���̃V�[����
				ptr->seq = 0;
				ptr->sub_seq = 0;
			}
			break;

		default : GF_ASSERT(0);
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief	���̃V�[�����Z�b�g
 *
 *	@param	UI_SCENE_CNT_PTR ptr
 *	@param	scene_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void UI_SCENE_CNT_SetNextScene( UI_SCENE_CNT_PTR ptr, int scene_id )
{	
	GF_ASSERT( ptr );
	GF_ASSERT( scene_id == UI_SCENE_ID_END || scene_id < ptr->scene_max );

	ptr->scene_id_next = scene_id;
}

//-----------------------------------------------------------------------------
/**
 *	@brief	���݂̃V�[��ID���擾
 *
 *	@param	UI_SCENE_CNT_PTR ptr 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
int UI_SCENE_CNT_GetSceneID( UI_SCENE_CNT_PTR ptr )
{	
	return ptr->scene_id;
}

//-----------------------------------------------------------------------------
/**
 *	@brief	�T�u�V�[�P���T���擾
 *
 *	@param	UI_SCENE_CNT_PTR ptr 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
u8 UI_SCENE_CNT_GetSubSeq( UI_SCENE_CNT_PTR ptr )
{	
	GF_ASSERT( ptr->seq != SCENE_SEQ_SET_UP );

	return ptr->sub_seq;
}

//-----------------------------------------------------------------------------
/**
 *	@brief	�T�u�V�[�P���T���C���N�������g
 *
 *	@param	UI_SCENE_CNT_PTR ptr 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void UI_SCENE_CNT_IncSubSeq( UI_SCENE_CNT_PTR ptr )
{	
	GF_ASSERT( ptr->seq != SCENE_SEQ_SET_UP );

	ptr->sub_seq++;
}


