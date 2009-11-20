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

//�����\��
#include "print/printsys.h"

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
///	�{�^�����J���
//=====================================
typedef enum
{
	BR_BTN_PARAM_TYPE,				//�{�^���̃^�C�v	�iBR_BTN_TYPE�j
	BR_BTN_PARAM_DATA1,				//�^�C�v�ɂ�����f�[�^
	BR_BTN_PARAM_DATA2,				//�^�C�v�ɂ�����f�[�^
	BR_BTN_PARAM_VALID,				//�����邩�ǂ���
} BR_BTN_PARAM;

//-------------------------------------
///	
//=====================================
#define BR_BTN_OAMFONT_OFS_X	(4*8)
#define BR_BTN_OAMFONT_OFS_Y	(1*8)

//-------------------------------------
///	�{�^���Ǘ��V�X�e��
//=====================================
//�Ǘ��X�^�b�N��
#define BR_BTN_SYS_STACK_MAX	(3)
//���ʂɃ{�^�����������Ƃ��̍��W
#define TAG_INIT_POS_UP( n )		( 25 + ( ( 36 ) * ( 4-n ) ) )

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�ėp�{�^�����[�N
//=====================================
struct _BR_BTN_WORK
{
	GFL_CLWK					*p_clwk;		//CLWK
	BMPOAM_ACT_PTR		p_oamfont;	//OAM�t�H���g
	GFL_BMP_DATA			*p_bmp;			//�t�H���g
	const BR_BTN_DATA *cp_data;		//�{�^���̃f�[�^
	CLSYS_DRAW_TYPE		display;		//�ǂ���ɕ`�悷�邩
};

//-------------------------------------
///	�{�^���V�X�e���ŊǗ������p�̃{�^�����[�N
//=====================================
typedef struct 
{
	BOOL							is_use;			//�g�p�t���O
  CLSYS_DRAW_TYPE   display;    //�ǂ���̉�ʂ�
	BR_BTN_WORK				*p_btn;			//�{�^�����[�N
	const BR_BTN_DATA *cp_data;		//�{�^���̃f�[�^
} BR_BTNEX_WORK;

//-------------------------------------
///	�o�g�����R�[�_�[�@�{�^���Ǘ�
//=====================================
struct _BR_BTN_SYS_WORK
{
	HEAPID						heapID;
	BR_RES_WORK				*p_res;		//���\�[�X
	GFL_CLUNIT				*p_unit;	//�Z�����j�b�g

	BR_BTN_SYS_STATE	state;	//�{�^���Ǘ��̏��
	BR_BTN_SYS_INPUT	input;	//�{�^�����͏��
	BR_BTN_TYPE				next_type;//���̓���
	BOOL							next_valid;//���̃{�^���������邩�ǂ���
	u32								next_proc;//���̃v���Z�X
	u32								next_mode;//���̃v���Z�X���[�h

	u32								trg_btn;			//�������{�^��

	u8								btn_now_max;	//���݂̃{�^���o�b�t�@�ő�
	u8								btn_now_num;	//���݂̃{�^����
	u8								btn_stack_max;//�{�^���X�^�b�N�o�b�t�@�ő�
	u8								btn_stack_num;//�{�^���X�^�b�N���ݐ�
	BR_BTNEX_WORK			*p_btn_stack;	//�X�^�b�N�ɐςރ{�^���o�b�t�@
	BR_BTNEX_WORK			*p_btn_now;		//���݂̃{�^���o�b�t�@

	BR_BTN_DATA_SYS		*p_btn_data;	//�{�^���f�[�^�o�b�t�@
	BMPOAM_SYS_PTR		p_bmpoam;	//BMPOAM�V�X�e��
};
//=============================================================================
/**
 *					PRIVATE�֐�
*/
//=============================================================================
//-------------------------------------
///	�{�^���V�X�e���̃v���C�x�[�g
//=====================================
static void Br_Btn_Sys_PushStack( BR_BTN_SYS_WORK *p_wk, BR_BTNEX_WORK *p_btn );
static BOOL Br_Btn_Sys_PopStack( BR_BTN_SYS_WORK *p_wk, BR_BTNEX_WORK *p_btn );
static void Br_Btn_Sys_ReLoadBtn( BR_BTN_SYS_WORK *p_wk, BR_MENUID menuID );

//-------------------------------------
///	BTN�̏���
//=====================================
static void BR_BTNEX_Init( BR_BTNEX_WORK *p_wk, const BR_BTN_DATA *cp_data, GFL_CLUNIT *p_unit, BMPOAM_SYS_PTR p_bmpoam, const BR_RES_WORK *cp_res, HEAPID heapID );
static void BR_BTNEX_Exit( BR_BTNEX_WORK *p_wk );
static BOOL BR_BTNEX_GetTrg( const BR_BTNEX_WORK *cp_wk, u32 x, u32 y );
static void BR_BTNEX_StartMove( BR_BTNEX_WORK *p_wk, BR_BTN_MOVE move, const BR_BTNEX_WORK *cp_target );
static BOOL BR_BTNEX_MainMove( BR_BTNEX_WORK *p_wk );
static void BR_BTNEX_Copy( const BR_BTN_SYS_WORK *cp_wk, BR_BTNEX_WORK *p_dst, const BR_BTNEX_WORK *cp_src, CLSYS_DRAW_TYPE	display );
static void BR_BTNEX_SetStackPos( BR_BTNEX_WORK *p_wk, u16 stack_num );
static u32 BR_BTNEX_GetParam( const BR_BTNEX_WORK *cp_wk, BR_BTN_PARAM param );

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
 *  @param	BR_RES_WORK			���\�[�X
 *  @param	BR_BTLREC_SET		�o�g���^����
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
BR_BTN_SYS_WORK *BR_BTN_SYS_Init( BR_MENUID menuID, GFL_CLUNIT *p_unit, BR_RES_WORK *p_res, const BR_BTLREC_SET *cp_rec, HEAPID heapID )
{	
	BR_BTN_SYS_WORK *p_wk;

	GF_ASSERT( menuID < BR_MENUID_MAX );

	//���[�N�쐬
	{	
		p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(BR_BTN_SYS_WORK) );
		GFL_STD_MemClear( p_wk, sizeof(BR_BTN_SYS_WORK) );
		p_wk->p_res				= p_res;
		p_wk->p_unit			= p_unit;
		p_wk->heapID			= heapID;
		p_wk->p_bmpoam		= BmpOam_Init( heapID, p_unit);
		p_wk->p_btn_data	= BR_BTN_DATA_SYS_Init( cp_rec, heapID );
	}

	//���\�[�X�ǂݍ���
	{	
		BR_RES_LoadOBJ( p_res, BR_RES_OBJ_BROWSE_BTN_M, heapID );
		BR_RES_LoadOBJ( p_res, BR_RES_OBJ_BROWSE_BTN_S, heapID );
	}

	//�{�^���o�b�t�@���쐬
	{	
		u32 size;
		
		p_wk->btn_now_max	= BR_BTN_DATA_SYS_GetDataMax(p_wk->p_btn_data);
		size = sizeof(BR_BTNEX_WORK) * p_wk->btn_now_max;
		p_wk->p_btn_now	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk->p_btn_now, size );
	}

	//�X�^�b�N�o�b�t�@���쐬
	{	
		u32 size;

		p_wk->btn_stack_max	= BR_BTN_SYS_STACK_MAX;
		size	= sizeof(BR_BTNEX_WORK) * BR_BTN_SYS_STACK_MAX;
		p_wk->p_btn_stack	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk->p_btn_stack, size );
	}

	//�ŏ��ɓǂݍ��ރ{�^����������
	{	
		//�ǂݍ��ރ{�^���̃����N�������m���A�r���̂��̂ł���΃X�^�b�N�ɐς�
		{	
			u32 num	= BR_BTN_DATA_SYS_GetLinkMax( p_wk->p_btn_data, menuID );

			if( num != 0 )
			{	
				int i;
				const BR_BTN_DATA *	cp_data;
				BR_MENUID	preID;
				u32 btnID;
				BR_BTNEX_WORK	btn	= {0};

				for( i = 0; i < num; i++ )
				{	
					BR_BTN_DATA_SYS_GetLink( p_wk->p_btn_data, menuID, i, &preID, &btnID );

					cp_data	= BR_BTN_DATA_SYS_GetData( p_wk->p_btn_data, preID, btnID );
					BR_BTNEX_Init( &btn, cp_data, p_unit, p_wk->p_bmpoam, p_res, heapID );
					Br_Btn_Sys_PushStack( p_wk, &btn );
				}
			}
		}

		//�ǂݍ��ނ���
		{	
			int i;
			const BR_BTN_DATA *	cp_data;
			p_wk->btn_now_num	= BR_BTN_DATA_SYS_GetDataNum( p_wk->p_btn_data, menuID );

			for( i = 0; i < p_wk->btn_now_num; i++ )
			{	
				cp_data	= BR_BTN_DATA_SYS_GetData( p_wk->p_btn_data, menuID, i );
				BR_BTNEX_Init( &p_wk->p_btn_now[i], cp_data, p_unit, p_wk->p_bmpoam, p_res, heapID );
			}
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
			BR_BTNEX_Exit( &p_wk->p_btn_stack[i] );
		}
	}
	GFL_HEAP_FreeMemory( p_wk->p_btn_stack );

	//�{�^���o�b�t�@�j��
	{	
		int i;
		for( i = 0; i < p_wk->btn_now_num; i++ )
		{	
			BR_BTNEX_Exit( &p_wk->p_btn_now[i] );
		}
	}
	GFL_HEAP_FreeMemory( p_wk->p_btn_now );
	
	//���\�[�X�j��
	{	
		BR_RES_UnLoadOBJ( p_wk->p_res, BR_RES_OBJ_BROWSE_BTN_S );
		BR_RES_UnLoadOBJ( p_wk->p_res, BR_RES_OBJ_BROWSE_BTN_M );
	}

	BR_BTN_DATA_SYS_Exit( p_wk->p_btn_data );
	BmpOam_Exit( p_wk->p_bmpoam );

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
			u32 x, y;
			BOOL is_trg	= FALSE;

			p_wk->input	= BR_BTN_SYS_INPUT_NONE;

			//�{�^���������m
			if( GFL_UI_TP_GetPointTrg( &x, &y ) )
			{	
				for( i = 0; i < p_wk->btn_now_max; i++ )
				{	
					if( BR_BTNEX_GetTrg( &p_wk->p_btn_now[i], x, y ) )
					{	

#ifdef PM_DEBUG
						int j;
						NAGI_Printf("push ! [%d]\n", i );
						for( j = 0; j < BR_BTN_DATA_PARAM_MAX; j++  )
						{	
							NAGI_Printf("param %d\n", BR_BTN_DATA_GetParam( p_wk->p_btn_now[i].cp_data, j) );
						}
#endif	//PM_DEBUG

						p_wk->trg_btn	= i;
						is_trg				= TRUE;
						break;
					}
				}
			}

			//�{�^������J�n
			if( is_trg )
			{	
				for( i = 0; i < p_wk->btn_now_max; i++ )
				{	
					if( p_wk->trg_btn == i )
					{	
						BR_BTNEX_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_PUSH, NULL );
					}
					else
					{	
						BR_BTNEX_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_HIDE, &p_wk->p_btn_now[p_wk->trg_btn] );
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
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_now_max; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_now[i] );
			}

			if( is_end )
			{
				p_wk->state	= BR_BTN_SYS_STATE_BTN_CHANGE;
			}
		}
		break;
	
	case BR_BTN_SYS_STATE_BTN_CHANGE:
		{
			BR_MENUID nextID;
			u32				next_mode;

			p_wk->next_type	= BR_BTNEX_GetParam(&p_wk->p_btn_now[p_wk->trg_btn],BR_BTN_PARAM_TYPE );
			nextID					= BR_BTNEX_GetParam(&p_wk->p_btn_now[p_wk->trg_btn],BR_BTN_PARAM_DATA1 );
			next_mode				= BR_BTNEX_GetParam(&p_wk->p_btn_now[p_wk->trg_btn],BR_BTN_PARAM_DATA2 );
			p_wk->next_valid= BR_BTNEX_GetParam(&p_wk->p_btn_now[p_wk->trg_btn],BR_BTN_PARAM_VALID );

			switch( p_wk->next_type )
			{	
			case BR_BTN_TYPE_SELECT:				//�I��p�{�^��
				//���肵���{�^�����X�^�b�N�ɐς�
				Br_Btn_Sys_PushStack( p_wk, &p_wk->p_btn_now[p_wk->trg_btn] );
				//���̃{�^����ǂݕς���
				Br_Btn_Sys_ReLoadBtn( p_wk, nextID );
				break;

			case BR_BTN_TYPE_RETURN:				//1�O�̃��j���[�֖߂�p�{�^��
				{	
					BR_BTNEX_WORK	btn;
					if( Br_Btn_Sys_PopStack( p_wk, &btn ) )
					{	
						GF_ASSERT( btn.is_use );
						//���̃{�^����ǂݕς���
						Br_Btn_Sys_ReLoadBtn( p_wk, nextID );
						BR_BTNEX_Exit( &btn );
					}
				}
				break;

			case BR_BTN_TYPE_EXIT:					//�o�g�����R�[�_�[�I���p�{�^��
				break;

			case BR_BTN_TYPE_MYRECORD:			//�����̋L�^�{�^��
				p_wk->next_proc	= nextID;
				p_wk->next_mode	= next_mode;
				NAGI_Printf( "�����̋L�^ %d %d \n", nextID, next_mode );
				break;

			case BR_BTN_TYPE_OTHERRECORD:	//�N���̋L�^�{�^��
				p_wk->next_proc	= nextID;
				p_wk->next_mode	= next_mode;
				break;

			case BR_BTN_TYPE_DELETE_MY:		//�����̋L�^�������{�^��
				p_wk->next_proc	= nextID;	
				p_wk->next_mode	= next_mode;
				break;

			case BR_BTN_TYPE_DELETE_OTHER:	//�N���̋L�^�������{�^��
				p_wk->next_proc	= nextID;
				p_wk->next_mode	= next_mode;
				break;

			case BR_BTN_TYPE_CHANGESEQ:		//�V�[�P���X�ύX�{�^��
				p_wk->next_proc	= nextID;
				p_wk->next_mode	= next_mode;
				break;
			}

			{	
				int i;
				//�{�^���o������
				for( i = 0; i < p_wk->btn_now_max; i++ )
				{	
					BR_BTNEX_StartMove( &p_wk->p_btn_now[i], BR_BTN_MOVE_APPEAR, NULL );
				}

				//�X�^�b�N�ɐς񂾁A����{�^���͏㕔�ֈړ�
			//	if( p_wk->btn_stack_num != 0 )
			//	{	
			//		BR_BTNEX_StartMove( &p_wk->p_btn_stack[p_wk->btn_stack_num-1], BR_BTN_MOVE_TAG, NULL );
			//	}
			}
			p_wk->state	= BR_BTN_SYS_STATE_APPEAR_MOVE;
		}
		break;

	case BR_BTN_SYS_STATE_APPEAR_MOVE:
		{	
			int i;
			BOOL is_end	= TRUE;

			for( i = 0; i < p_wk->btn_now_max; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_now[i] );
			}
			for( i = 0; i < p_wk->btn_stack_max; i++ )
			{	
				is_end	&= BR_BTNEX_MainMove( &p_wk->p_btn_stack[i] );
			}
			if( is_end )
			{		
				p_wk->state	= BR_BTN_SYS_STATE_INPUT;
			}
		}
		break;

	case BR_BTN_SYS_STATE_INPUT:
	
		p_wk->state	= BR_BTN_SYS_STATE_WAIT;

		if( p_wk->next_valid )
		{	

			switch( p_wk->next_type )
			{	
			case BR_BTN_TYPE_SELECT:				//�I��p�{�^��
				/* fallthrough */
			case BR_BTN_TYPE_RETURN:				//1�O�̃��j���[�֖߂�p�{�^��
				p_wk->state	= BR_BTN_SYS_STATE_WAIT;
				break;

			case BR_BTN_TYPE_EXIT:					//�o�g�����R�[�_�[�I���p�{�^��
				p_wk->input	= BR_BTN_SYS_INPUT_EXIT;
				break;

			case BR_BTN_TYPE_MYRECORD:			//�����̋L�^�{�^��
				p_wk->input	= BR_BTN_SYS_INPUT_CHANGESEQ;
				break;

			case BR_BTN_TYPE_OTHERRECORD:	//�N���̋L�^�{�^��
				p_wk->input	= BR_BTN_SYS_INPUT_CHANGESEQ;
				break;

			case BR_BTN_TYPE_DELETE_MY:		//�����̋L�^�������{�^��
				break;

			case BR_BTN_TYPE_DELETE_OTHER:	//�N���̋L�^�������{�^��
				break;

			case BR_BTN_TYPE_CHANGESEQ:		//�V�[�P���X�ύX�{�^��			
				p_wk->input	= BR_BTN_SYS_INPUT_CHANGESEQ;
				break;
		}
		break;
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ�	���͏�Ԏ擾
 *
 *	@param	cp_wk	���[�N
 *	@param	p_seq	BR_BTN_SYS_INPUT_CHANGESEQ�̂Ƃ��A���̃V�[�P���X
 *	@param	p_param	BR_BTN_SYS_INPUT_CHANGESEQ�̂Ƃ��A���̃V�[�P���X�̃��[�h
 *
 *	@return	���͂̎��
 */
//-----------------------------------------------------------------------------
BR_BTN_SYS_INPUT BR_BTN_SYS_GetInput( const BR_BTN_SYS_WORK *cp_wk, u32 *p_seq, u32 *p_mode )
{	
	if( p_seq )
	{	
		*p_seq	= cp_wk->next_proc;
	}
	if( p_mode )
	{	
		*p_mode	= cp_wk->next_mode;
	}

	return cp_wk->input;
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
static void Br_Btn_Sys_PushStack( BR_BTN_SYS_WORK *p_wk, BR_BTNEX_WORK *p_btn )
{	
	GF_ASSERT( p_wk->btn_stack_num - 1 < p_wk->btn_stack_max );

	//�ς�
	BR_BTNEX_Copy( p_wk, &p_wk->p_btn_stack[p_wk->btn_stack_num], p_btn, CLSYS_DRAW_MAIN );
	GFL_STD_MemClear( p_btn, sizeof(BR_BTNEX_WORK) );	//�ς񂾂̂ŁA���̏ꏊ�̏�������
	
	p_wk->btn_stack_num++;

	//���W��ݒ� //@todo
	{	
		int i;
		for( i = 0; i < p_wk->btn_stack_num; i++ )
		{	
			BR_BTNEX_SetStackPos( &p_wk->p_btn_stack[p_wk->btn_stack_num - i - 1], i );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���Ǘ��V�X�e������{�^����j������
 *
 *	@param	BR_BTN_SYS_WORK *p_wk	���[�N
 *	@param	*p_btn								�X�^�b�N����󂯎��{�^��
 *
 *	@retval	TRUE�j�������@FALSE�擪�������̂ŉ������Ȃ�����
 */
//-----------------------------------------------------------------------------
static BOOL Br_Btn_Sys_PopStack( BR_BTN_SYS_WORK *p_wk, BR_BTNEX_WORK *p_btn )
{	
	if( p_wk->btn_stack_num != 0 )
	{	
		p_wk->btn_stack_num--;
		*p_btn	= p_wk->p_btn_stack[p_wk->btn_stack_num];
		GFL_STD_MemClear( &p_wk->p_btn_stack[p_wk->btn_stack_num], sizeof(BR_BTNEX_WORK) );


		//���W��ݒ�
		{	
			int i;
			for( i = 0; i < p_wk->btn_stack_num; i++ )
			{	
				BR_BTNEX_SetStackPos( &p_wk->p_btn_stack[p_wk->btn_stack_num - i - 1], i );
			}
		}

		return TRUE;
	}

	return FALSE;
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
	int i;
	for( i = 0; i < p_wk->btn_now_max; i++ )
	{	
		BR_BTNEX_Exit( &p_wk->p_btn_now[i] );
	}

	{	
		int i;
		const BR_BTN_DATA *	cp_data;

		p_wk->btn_now_num	= BR_BTN_DATA_SYS_GetDataNum( p_wk->p_btn_data, menuID );

		for( i = 0; i < p_wk->btn_now_num; i++ )
		{	
			cp_data	= BR_BTN_DATA_SYS_GetData( p_wk->p_btn_data, menuID, i );
			BR_BTNEX_Init( &p_wk->p_btn_now[i], cp_data, p_wk->p_unit, p_wk->p_bmpoam, p_wk->p_res, p_wk->heapID );
		}
	}
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
 *	@param	BR_BTNEX_WORK *p_wk			���[�N
 *	@param	BR_BTN_DATA *cp_data	�{�^�����
 *	@param	*p_unit								OBJ�쐬�p���j�b�g
 *	@param	*p_bmpoam							BMPOAM�V�X�e��
 *	@param	p_res									���\�[�X
 *	@param	heapID								�q�[�vID
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_Init( BR_BTNEX_WORK *p_wk, const BR_BTN_DATA *cp_data, GFL_CLUNIT *p_unit, BMPOAM_SYS_PTR p_bmpoam, const BR_RES_WORK *cp_res, HEAPID heapID )
{	
	u32 plt;

	GF_ASSERT( p_wk->is_use == FALSE );

	GFL_STD_MemClear( p_wk, sizeof(BR_BTNEX_WORK) );
	p_wk->is_use	= TRUE;
	p_wk->cp_data	= cp_data;
  p_wk->display = CLSYS_DRAW_SUB;

	//�{�^���쐬
	{	
		BR_RES_OBJ_DATA	res;
		GFL_FONT *p_font;
		GFL_MSGDATA *p_msg;
		GFL_CLWK_DATA	cldata;
    u32 msgID;

    //�f�[�^�ݒ�
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x    = BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_X );
    cldata.pos_y    = BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_Y );
    cldata.anmseq   = BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_ANMSEQ );
    cldata.softpri  = 1;

		//���\�[�X�ǂݍ���
		BR_RES_GetOBJRes( cp_res, BR_RES_OBJ_BROWSE_BTN_S, &res );
		p_font	= BR_RES_GetFont( cp_res );
		p_msg		= BR_RES_GetMsgData( cp_res );
		msgID		= BR_BTN_DATA_GetParam( cp_data, BR_BTN_DATA_PARAM_MSGID );

		//�쐬
		p_wk->p_btn		= BR_BTN_Init( &cldata, msgID, p_wk->display, p_unit, 
				p_bmpoam, p_font, p_msg, &res, GetHeapLowID( heapID ) );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���j��
 *
 *	@param	BR_BTNEX_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_Exit( BR_BTNEX_WORK *p_wk )
{	
	if( p_wk->is_use )
	{	
    BR_BTN_Exit( p_wk->p_btn );
		GFL_STD_MemClear( p_wk, sizeof(BR_BTNEX_WORK) );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����^�b�`�������ǂ���
 *
 *	@param	const BR_BTNEX_WORK *cp_wk	���[�N
 *	@param	�ʒu���W
 *
 *	@return	TRUE�Ȃ�Ή�����	FALSE�Ȃ�Ή����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BR_BTNEX_GetTrg( const BR_BTNEX_WORK *cp_wk, u32 x, u32 y )
{	
	if( !cp_wk->is_use )
	{	
		return FALSE;
	}

  return BR_BTN_GetTrg( cp_wk->p_btn, x, y );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̓���J�n
 *
 *	@param	BR_BTNEX_WORK *p_wk				���[�N
 *	@param	move										����
 *	@param	BR_BTNEX_WORK *cp_target	����ɕK�v�ȃ^�[�Q���g
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_StartMove( BR_BTNEX_WORK *p_wk, BR_BTN_MOVE move, const BR_BTNEX_WORK *cp_target )
{	
	//@todo
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̃��C������
 *
 *	@param	BR_BTNEX_WORK *p_wk		���[�N
 *
 *	@return	TRUE�Ȃ�΃{�^���̓����I��	FALSE�Ȃ�Γ�����
 */
//-----------------------------------------------------------------------------
static BOOL BR_BTNEX_MainMove( BR_BTNEX_WORK *p_wk )
{	
	//@todo
	return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����R�s�[
 *
 *	@param	p_wk		���[�N					
 *	@param	p_dst		�R�s�[��@�T�u��ʂ̃{�^��
 *	@param	p_src		�R�s�[��	���C����ʂ̃{�^��
 *	@param	display	�ǂ���̉�ʂɃR�s�[���邩
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_Copy( const BR_BTN_SYS_WORK *cp_wk, BR_BTNEX_WORK *p_dst, const BR_BTNEX_WORK *cp_src, CLSYS_DRAW_TYPE display )
{
	u32 plt;	

	GF_ASSERT( p_dst->is_use == FALSE );
	GF_ASSERT( cp_src->is_use == TRUE );
		
	//�����p������
	{
		GFL_STD_MemClear( p_dst, sizeof(BR_BTNEX_WORK) );
		p_dst->cp_data	= cp_src->cp_data;
		p_dst->is_use		= cp_src->is_use;
		p_dst->display	= display;
	}

	//�����Ȃ�΁A�R�s�[�A
	//�Ⴄ�Ȃ�΍�蒼��
	if( cp_src->display	== display )
	{	
		p_dst->p_btn			= cp_src->p_btn;
	}
	else
	{	
		//�V������蒼�����߁A����
		{	
			BR_BTN_Exit( cp_src->p_btn );
		}

    //�{�^���쐬
    {	
      BR_RES_OBJ_DATA	res;
      GFL_FONT *p_font;
      GFL_MSGDATA *p_msg;
      GFL_CLWK_DATA	cldata;
      u32 msgID;

      //�f�[�^�ݒ�
      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
      cldata.pos_x    = BR_BTN_DATA_GetParam( p_dst->cp_data, BR_BTN_DATA_PARAM_X );
      cldata.pos_y    = BR_BTN_DATA_GetParam( p_dst->cp_data, BR_BTN_DATA_PARAM_Y );
      cldata.anmseq   = BR_BTN_DATA_GetParam( p_dst->cp_data, BR_BTN_DATA_PARAM_ANMSEQ );
      cldata.softpri  = 1;

      //���\�[�X�ǂݍ���
			if( display == CLSYS_DRAW_MAIN )
			{	
				BR_RES_GetOBJRes( cp_wk->p_res, BR_RES_OBJ_BROWSE_BTN_M, &res );
			}
			else
			{	
				BR_RES_GetOBJRes( cp_wk->p_res, BR_RES_OBJ_BROWSE_BTN_S, &res );
			}
      p_font	= BR_RES_GetFont( cp_wk->p_res );
      p_msg		= BR_RES_GetMsgData( cp_wk->p_res );
      msgID		= BR_BTN_DATA_GetParam( p_dst->cp_data, BR_BTN_DATA_PARAM_MSGID );

      //�쐬
      p_dst->p_btn		= BR_BTN_Init( &cldata, msgID, display, cp_wk->p_unit, 
          cp_wk->p_bmpoam, p_font, p_msg, &res, cp_wk->heapID );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̍��W��ݒ�
 *
 *	@param	BR_BTNEX_WORK *p_wk	���[�N
 *	@param	x									X���W
 *	@param	y									Y���W
 *
 */
//-----------------------------------------------------------------------------
static void BR_BTNEX_SetStackPos( BR_BTNEX_WORK *p_wk, u16 stack_num )
{	
	GFL_CLACTPOS	pos;
	pos.x	= BR_BTN_DATA_GetParam( p_wk->cp_data, BR_BTN_DATA_PARAM_X );
	pos.y	= TAG_INIT_POS_UP( stack_num );

  BR_BTN_SetPos( p_wk->p_btn, pos.x, pos.y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̏����󂯎��
 *
 *	@param	const BR_BTNEX_WORK *cp_wk	���[�N
 *	@param	param		�{�^���̏��C���f�b�N�X
 *
 *	@return	�{�^�����
 */
//-----------------------------------------------------------------------------
static u32 BR_BTNEX_GetParam( const BR_BTNEX_WORK *cp_wk, BR_BTN_PARAM param )
{	
	u32 ret;
	switch( param )
	{	
	case BR_BTN_PARAM_TYPE:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_TYPE );
		break;

	case BR_BTN_PARAM_DATA1:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_DATA1 );
		break;

	case BR_BTN_PARAM_DATA2:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_DATA2 );
		break;
	
	case BR_BTN_PARAM_VALID:
		ret = BR_BTN_DATA_GetParam( cp_wk->cp_data, BR_BTN_DATA_PARAM_VALID );
		break;


	default:
		ret	= 0;
		GF_ASSERT(0);
	}

	return ret;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�{�^���P�̍쐬�֐�(�e�v���b�N�Ŏg�p����)
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^��������
 *
 *	@param	const GFL_CLWK_DATA *cp_cldata	�ݒ���
 *	@param	msgID														�{�^���ɍڂ��镶����
 *	@param	display													�\����
 *	@param	*p_unit													�A�N�^�[�o�^���j�b�g
 *	@param	p_bmpoam												BMPOAM�o�^�V�X�e��
 *	@param	*p_font													�t�H���g
 *	@param	*p_msg													���b�Z�[�W
 *	@param	BR_RES_OBJ_DATA *cp_res					���\�[�X
 *	@param	heapID													�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
BR_BTN_WORK * BR_BTN_Init( const GFL_CLWK_DATA *cp_cldata, u16 msgID, CLSYS_DRAW_TYPE display, GFL_CLUNIT *p_unit, BMPOAM_SYS_PTR p_bmpoam, GFL_FONT *p_font, GFL_MSGDATA *p_msg, const BR_RES_OBJ_DATA *cp_res, HEAPID heapID )
{	
	BR_BTN_WORK *p_wk;

	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(BR_BTN_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(BR_BTN_WORK) );
  p_wk->display = display;

	//CLWK�쐬
	{	
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_unit, 
				cp_res->ncg, cp_res->ncl, cp_res->nce, 
				cp_cldata, display, heapID );
	}

	//�t�H���g
	{	
		STRBUF			*p_str;

		p_wk->p_bmp	= GFL_BMP_Create( 15, 2, GFL_BMP_16_COLOR, heapID );
		p_str	= GFL_MSG_CreateString( p_msg, msgID );
		PRINTSYS_Print( p_wk->p_bmp, 0, 0, p_str, p_font );

		GFL_STR_DeleteBuffer( p_str );
	}


	//OAM�̕������쐬
	{	
		BMPOAM_ACT_DATA	actdata;
		
		GFL_STD_MemClear( &actdata, sizeof(BMPOAM_ACT_DATA) );
		actdata.bmp	= p_wk->p_bmp;
		actdata.x		= cp_cldata->pos_x + BR_BTN_OAMFONT_OFS_X;
		actdata.y		= cp_cldata->pos_y - BR_BTN_DATA_HEIGHT/2 + BR_BTN_OAMFONT_OFS_Y;
		actdata.pltt_index	= cp_res->ncl;
		actdata.soft_pri		= cp_cldata->softpri - 1;
		actdata.setSerface	= display;
		actdata.draw_type		= display;
		actdata.bg_pri			= cp_cldata->bgpri;
		p_wk->p_oamfont	= BmpOam_ActorAdd( p_bmpoam, &actdata );
		BmpOam_ActorBmpTrans(p_wk->p_oamfont);
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���j��
 *
 *	@param	BR_BTN_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_BTN_Exit( BR_BTN_WORK *p_wk )
{	
	BmpOam_ActorDel( p_wk->p_oamfont );
	GFL_BMP_Delete( p_wk->p_bmp );
	GFL_CLACT_WK_Remove( p_wk->p_clwk );

	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̃^�b�`�����m
 *
 *	@param	const BR_BTN_WORK *cp_wk	���[�N
 *	@param	x													�^�b�`���WX
 *	@param	y													�^�b�`���WY
 *
 *	@return	TRUE�Ń{�^���g���K	FALSE�ŉ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_BTN_GetTrg( const BR_BTN_WORK *cp_wk, u32 x, u32 y )
{	
	GFL_RECT rect;
	GFL_CLACTPOS	pos;

	GFL_CLACT_WK_GetPos( cp_wk->p_clwk, &pos, cp_wk->display );

	rect.left		= pos.x;
	rect.right	= pos.x + BR_BTN_DATA_WIDTH;
	rect.top		= pos.y - BR_BTN_DATA_HEIGHT/2;
	rect.bottom	= pos.y + BR_BTN_DATA_HEIGHT/2;

  return ( ((u32)( x - rect.left) <= (u32)(rect.right - rect.left))
            & ((u32)( y - rect.top) <= (u32)(rect.bottom - rect.top)));
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���̈ʒu��ݒ�
 *
 *	@param	BR_BTN_WORK *p_wk		���[�N
 *	@param	x										���WX
 *	@param	y										���WY
 */
//-----------------------------------------------------------------------------
void BR_BTN_SetPos( BR_BTN_WORK *p_wk, u32 x, u32 y )
{	
	GFL_CLACTPOS	pos;
	pos.x	= x;
	pos.y	= y;

	GFL_CLACT_WK_SetPos( p_wk->p_clwk, &pos, p_wk->display );
	BmpOam_ActorSetPos( p_wk->p_oamfont,	pos.x + BR_BTN_OAMFONT_OFS_X,
																				pos.y - BR_BTN_DATA_HEIGHT/2 + BR_BTN_OAMFONT_OFS_Y );

}
