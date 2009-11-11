//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_btn.c
 *	@brief	�o�g�����R�[�_�[�{�^���Ǘ�
 *	@author	Toru=Nagihashi
 *	@data		2009.11.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�����̃��W���[��
#include "br_btn_data.h"
#include "br_btn.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�{�^���ړ��^�C�v
//=====================================
typedef enum
{
	BR_BTN_MOVE_PUSH,		//�����ꂽ�{�^���̏���					target=NULL
	BR_BTN_MOVE_HIDE,		//�����ꂽ�{�^���̊i�[���鏈��	target=PUSH
	BR_BTN_MOVE_APPEAR,	//�{�^���o������								target=NULL
	BR_BTN_MOVE_TAG,		//���̃^�O��f���o�������֓o��target=TAG(�����)
} BR_BTN_MOVE;
//-------------------------------------
///	�{�^���ړ��^�C�v
//=====================================
typedef enum
{
	BR_BTN_PARAM_TYPE,				//�{�^���̃^�C�v	�iBR_BTN_TYPE�j
	BR_BTN_PARAM_DATA,				//�^�C�v�ɂ�����f�[�^
} BR_BTN_PARAM;

//-------------------------------------
///	�{�^���Ǘ��V�X�e��
//=====================================
//�Ǘ��X�^�b�N��
#define BR_BTN_SYS_STACK_MAX	(3)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�{�^���P��
//=====================================
typedef struct 
{
	BOOL							is_use;		//�g�p�t���O
	GFL_CLWK					*p_clwk;	//CLWK
	const BR_BTN_DATA *cp_data;	//�{�^���̃f�[�^
} BR_BTN_WORK;

//-------------------------------------
///	�o�g�����R�[�_�[�@�{�^���Ǘ�
//=====================================
struct _BR_BTN_SYS_WORK
{

	BR_BTN_SYS_STATE	state;	//�{�^���Ǘ��̏��
	BR_BTN_SYS_INPUT	input;	//�{�^�����͏��

	u32					trg_btn;			//�������{�^��

	u8					btn_now_max;	//���݂̃{�^���o�b�t�@�ő�
	u8					btn_now_num;	//���݂̃{�^����
	u8					btn_stack_max;//�{�^���X�^�b�N�o�b�t�@�ő�
	u8					btn_stack_num;//�{�^���X�^�b�N���ݐ�
	BR_BTN_WORK *p_btn_stack;	//�X�^�b�N�ɐςރ{�^���o�b�t�@
	BR_BTN_WORK	*p_btn_now;		//���݂̃{�^���o�b�t�@
};
//=============================================================================
/**
 *					PRIVATE�֐�
*/
//=============================================================================
//-------------------------------------
///	�{�^���V�X�e���̃v���C�x�[�g
//=====================================
static void Br_Btn_Sys_PushStack( BR_BTN_SYS_WORK *p_wk, BR_BTN_WORK *p_btn );
static void Br_Btn_Sys_ReLoadBtn( BR_BTN_SYS_WORK *p_wk, BR_MENUID menuID );

//-------------------------------------
///	BTN�̏���
//=====================================
static void BR_BTN_Init( BR_BTN_WORK *p_wk, const BR_BTN_DATA *cp_data, GFL_CLUNIT *p_unit, HEAPID heapID );
static void BR_BTN_Exit( BR_BTN_WORK *p_wk );
static BOOL BR_BTN_GetTrg( const BR_BTN_WORK *cp_wk );
static void BR_BTN_StartMove( BR_BTN_WORK *p_wk, BR_BTN_MOVE move, const BR_BTN_WORK *cp_target );
static BOOL BR_BTN_MainMove( BR_BTN_WORK *p_wk );
static void BR_BTN_Copy( const BR_BTN_WORK *cp_wk, BR_BTN_WORK *p_wk );
static u32 BR_BTN_GetParam( const BR_BTN_WORK *cp_wk, BR_BTN_PARAM param );

//=============================================================================
/**
 *					PUBILIC�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ�������
 *
 *  @param	mode						�N�����[�h
 *  @param	GFL_CLUNIT			���j�b�g
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
BR_BTN_SYS_WORK *BR_BTN_SYS_Init( BR_MODE mode, GFL_CLUNIT *p_unit, HEAPID heapID )
{	
	BR_BTN_SYS_WORK *p_wk;

	//���[�N�쐬
	{	
		p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(BR_BTN_SYS_WORK) );
		GFL_STD_MemClear( p_wk, sizeof(BR_BTN_SYS_WORK) );
	}

	//���\�[�X�ǂݍ���
	{	
		
	}

	//�{�^���o�b�t�@���쐬
	{	
		u32 size;
		
		p_wk->btn_now_max	= BR_BTN_DATA_SYS_GetDataMax();
		size = sizeof(BR_BTN_WORK) * p_wk->btn_now_max;
		p_wk->p_btn_now	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk->p_btn_now, size );
	}

	//�X�^�b�N�o�b�t�@���쐬
	{	
		u32 size;

		p_wk->btn_stack_max	= BR_BTN_SYS_STACK_MAX;
		size	= sizeof(BR_BTN_WORK) * BR_BTN_SYS_STACK_MAX;
		p_wk->p_btn_stack	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk->p_btn_stack, size );
	}

	//�ŏ��ɓǂݍ��ރ{�^����������
	{	
		int i;
		int max;
		const BR_BTN_DATA *	cp_data;

		BR_MENUID	startID;

		//�������j���[�擾
		startID	= BR_BTN_DATA_SYS_GetStartMenuID( mode );

		p_wk->btn_now_num	= BR_BTN_DATA_SYS_GetDataNum( mode );
		for( i = 0; i < p_wk->btn_now_num; i++ )
		{	
			cp_data	= BR_BTN_DATA_SYS_GetData( mode, i );
			BR_BTN_Init( &p_wk->p_btn_now[i], cp_data, p_unit, heapID );
		}
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ��j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
void BR_BTN_SYS_Exit( BR_BTN_SYS_WORK *p_wk )
{	
	//�X�^�b�N�o�b�t�@�j��
	{	
		int i;
		for( i = 0; i < p_wk->btn_stack_num; i++ )
		{	
			BR_BTN_Exit( &p_wk->p_btn_stack[i] );
		}
	}
	GFL_HEAP_FreeMemory( p_wk->p_btn_stack );

	//�{�^���o�b�t�@�j��
	{	
		int i;
		for( i = 0; i < p_wk->btn_now_num; i++ )
		{	
			BR_BTN_Exit( &p_wk->p_btn_now[i] );
		}
	}
	GFL_HEAP_FreeMemory( p_wk->p_btn_now );
	
	//���\�[�X�j��
	{	

	}

	//���[�N�j��
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ����C������
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
void BR_BTN_SYS_Main( BR_BTN_SYS_WORK *p_wk )
{
	switch( p_wk->state )
	{	
	case BR_BTN_SYS_STATE_WAIT:	
		{	
			int i;
			BOOL is_trg	= FALSE;


			p_wk->input	= BR_BTN_SYS_INPUT_NONE;

			//�{�^���������m
			for( i = 0; i < p_wk->btn_now_max; i++ )
			{	
				if( BR_BTN_GetTrg( &p_wk->p_btn_now[i] ) )
				{	
					p_wk->trg_btn	= i;
					is_trg				= TRUE;
					break;
				}
			}

			//�{�^������J�n
			if( is_trg )
			{	
				for( i = 0; i < p_wk->btn_now_max; i++ )
				{	
					if( p_wk->trg_btn == i )
					{	
						BR_BTN_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_PUSH, NULL );
					}
					else
					{	
						BR_BTN_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_HIDE, &p_wk->p_btn_now[p_wk->trg_btn] );
					}
				}

				p_wk->state	= BR_BTN_SYS_STATE_HANGER_MOVE;
			}
		}
		break;

	case BR_BTN_SYS_STATE_HANGER_MOVE:
		//�{�^�����B���܂ł̓���
		{	
			int i;
			BOOL is_end	= FALSE;

			for( i = 0; i < p_wk->btn_now_max; i++ )
			{	
				is_end	&= BR_BTN_MainMove( &p_wk->p_btn_now[i] );
			}

			if( is_end )
			{
				p_wk->state	= BR_BTN_SYS_STATE_BTN_CHANGE;
			}
		}
		break;
	
	case BR_BTN_SYS_STATE_BTN_CHANGE:
		//���肵���{�^�����X�^�b�N�ɐς݁A�N���A
		Br_Btn_Sys_PushStack( p_wk, &p_wk->p_btn_now[p_wk->trg_btn] );
		BR_BTN_Exit( &p_wk->p_btn_now[p_wk->trg_btn] );
		//���̃{�^����ǂݕς���
		Br_Btn_Sys_ReLoadBtn( p_wk, 0 );	//@todo
		{	
			int i;

			//�{�^���o������
			for( i = 0; i < p_wk->btn_now_max; i++ )
			{	
				BR_BTN_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_APPEAR, NULL );
			}

			//�X�^�b�N�ɐς񂾁A����{�^���͏㕔�ֈړ�
			BR_BTN_StartMove( &p_wk->p_btn_stack[p_wk->btn_stack_num-1], BR_BTN_MOVE_TAG, NULL );
		}
		p_wk->state	= BR_BTN_SYS_STATE_APPEAR_MOVE;
		break;

	case BR_BTN_SYS_STATE_APPEAR_MOVE:
		{	
			int i;
			BOOL is_end	= FALSE;

			for( i = 0; i < p_wk->btn_now_max; i++ )
			{	
				is_end	&= BR_BTN_MainMove( &p_wk->p_btn_now[i] );
			}
			for( i = 0; i < p_wk->btn_stack_max; i++ )
			{	
				is_end	&= BR_BTN_MainMove( &p_wk->p_btn_stack[i] );
			}
			if( is_end )
			{
				p_wk->state	= BR_BTN_SYS_STATE_BTN_CHANGE;
			}
		}
		break;

	case BR_BTN_SYS_STATE_INPUT:
		if( 0 )
		{	
			p_wk->input	= BR_BTN_SYS_INPUT_CHANGESEQ;
		}
		p_wk->state	= BR_BTN_SYS_STATE_WAIT;
		break;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ�	���͏�Ԏ擾
 *
 *	@param	cp_wk	���[�N
 *	@param	p_seq	BR_BTN_SYS_INPUT_CHANGESEQ�̂Ƃ��A���̂��[����
 *
 *	@return	���͂̎��
 */
//-----------------------------------------------------------------------------
BR_BTN_SYS_INPUT BR_BTN_SYS_GetInput( const BR_BTN_SYS_WORK *cp_wk, u32 *p_seq )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ�	��Ԏ擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	���݂̏��
 */
//-----------------------------------------------------------------------------
BR_BTN_SYS_STATE BR_BTN_SYS_GetState( const BR_BTN_SYS_WORK *cp_wk )
{	
	return cp_wk->state;
}
//=============================================================================
/**
 *		�{�^���Ǘ��V�X�e���̃v���C�x�[�g
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���Ǘ��V�X�e���Ƀ{�^����ς�
 *
 *	@param	BR_BTN_SYS_WORK *p_wk		���[�N
 *	@param	*p_btn									�X�^�b�N�ɐςރ{�^��
 */
//-----------------------------------------------------------------------------
static void Br_Btn_Sys_PushStack( BR_BTN_SYS_WORK *p_wk, BR_BTN_WORK *p_btn )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^����j�����A�ۓǂݍ��݂���
 *
 *	@param	BR_BTN_SYS_WORK *p_wk ���[�N
 *	@param	BR_MENUID							�ǂݍ��ރ{�^���̃��j���[ID
 */
//-----------------------------------------------------------------------------
static void Br_Btn_Sys_ReLoadBtn( BR_BTN_SYS_WORK *p_wk, BR_MENUID menuID )
{	

}

//=============================================================================
/**
 *		�{�^���P�P�̏���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^��������
 *
 *	@param	BR_BTN_WORK *p_wk			���[�N
 *	@param	BR_BTN_DATA *cp_data	�{�^�����
 *	@param	*p_unit								OBJ�쐬�p���j�b�g
 *	@param	heapID								�q�[�vID
 */
//-----------------------------------------------------------------------------
static void BR_BTN_Init( BR_BTN_WORK *p_wk, const BR_BTN_DATA *cp_data, GFL_CLUNIT *p_unit, HEAPID heapID )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���j��
 *
 *	@param	BR_BTN_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_BTN_Exit( BR_BTN_WORK *p_wk )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����^�b�`�������ǂ���
 *
 *	@param	const BR_BTN_WORK *cp_wk	���[�N
 *
 *	@return	TRUE�Ȃ�Ή�����	FALSE�Ȃ�Ή����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BR_BTN_GetTrg( const BR_BTN_WORK *cp_wk )
{	
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̓���J�n
 *
 *	@param	BR_BTN_WORK *p_wk				���[�N
 *	@param	move										����
 *	@param	BR_BTN_WORK *cp_target	����ɕK�v�ȃ^�[�Q���g
 */
//-----------------------------------------------------------------------------
static void BR_BTN_StartMove( BR_BTN_WORK *p_wk, BR_BTN_MOVE move, const BR_BTN_WORK *cp_target )
{	
	//@todo
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̃��C������
 *
 *	@param	BR_BTN_WORK *p_wk		���[�N
 *
 *	@return	TRUE�Ȃ�΃{�^���̓����I��	FALSE�Ȃ�Γ�����
 */
//-----------------------------------------------------------------------------
static BOOL BR_BTN_MainMove( BR_BTN_WORK *p_wk )
{	
	//@todo
	return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����R�s�[
 *
 *	@param	const BR_BTN_WORK *cp_wk	���[�N
 *	@param	*p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void BR_BTN_Copy( const BR_BTN_WORK *cp_wk, BR_BTN_WORK *p_wk )
{	
	//@todo
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̏����󂯎��
 *
 *	@param	const BR_BTN_WORK *cp_wk	���[�N
 *	@param	param		�{�^���̏��C���f�b�N�X
 *
 *	@return	�{�^�����
 */
//-----------------------------------------------------------------------------
static u32 BR_BTN_GetParam( const BR_BTN_WORK *cp_wk, BR_BTN_PARAM param )
{	
	u32 ret;
	switch( param )
	{	
	case BR_BTN_PARAM_TYPE:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_DATA );
		break;

	case BR_BTN_PARAM_DATA:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_TYPE );
		break;

	default:
		ret	= 0;
		GF_ASSERT(0);
	}

	return ret;
}
