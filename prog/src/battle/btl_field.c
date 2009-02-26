
//============================================================================================
/**
 * @file	btl_field.c
 * @brief	�퓬��ʕ`��p�֐��i�t�B�[���h�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================

#include <gflib.h>

#include "btl_field.h"
#include "btl_efftool.h"

#include "arc_def.h"
#include "battgra/battgra_wb.naix"

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTL_FIELD_WORK
{
	GFL_TCBSYS			*tcb_sys;
	GFL_G3D_RES			*field_resource[ BTL_FIELD_MAX ];
	GFL_G3D_RND			*field_render[ BTL_FIELD_MAX ];
	GFL_G3D_OBJ			*field_obj[ BTL_FIELD_MAX ];
	GFL_G3D_OBJSTATUS	field_status[ BTL_FIELD_MAX ];
	HEAPID				heapID;
};

typedef	struct
{
	BTL_FIELD_WORK		*bfw;
	EFFTOOL_MOVE_WORK	emw;
}BTL_FIELD_TCB_WORK;

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

BTL_FIELD_WORK	*BTL_FIELD_Init( GFL_TCBSYS *tcb_sys, int index, HEAPID heapID );
void			BTL_FIELD_Exit( BTL_FIELD_WORK *bfw );
void			BTL_FIELD_Main( BTL_FIELD_WORK *bfw );
void			BTL_FIELD_Draw( BTL_FIELD_WORK *bfw );

//TCB�֐��Q
static	void	BTL_FIELD_TCBAdd( BTL_FIELD_WORK *bfw );
static	void	BTL_FIELD_TCB_MoveShadowLine( GFL_TCB *tcb, void *work );

//============================================================================================
/**
 *	�w�i�̃��\�[�X�e�[�u��
 */
//============================================================================================
//���f���f�[�^
static	const	int	field_resource_table[][BTL_FIELD_MAX]={
	{ NARC_battgra_wb_batt_field01_nsbmd, NARC_battgra_wb_batt_bg01_nsbmd, NARC_battgra_wb_batt_field02_nsbmd },
};

//============================================================================================
/**
 *	�V�X�e��������
 *
 * @param[in]	tcb_sys	�V�X�e�����Ŏg�p����TCBSYS�\���̂ւ̃|�C���^
 * @param[in]	index	�ǂݍ��ރ��\�[�X��INDEX
 * @param[in]	heapID	�q�[�vID
 */
//============================================================================================
BTL_FIELD_WORK	*BTL_FIELD_Init( GFL_TCBSYS *tcb_sys, int index, HEAPID heapID )
{
	BTL_FIELD_WORK *bfw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTL_FIELD_WORK ) );
	BOOL	ret;
	int		i;

	GF_ASSERT( index < NELEMS( field_resource_table ) );

	bfw->tcb_sys = tcb_sys;
	bfw->heapID = heapID;

	for( i = 0 ; i < BTL_FIELD_MAX ; i++ ){
		//���\�[�X�ǂݍ���
		bfw->field_resource[ i ] = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, field_resource_table[ index ][ i ] );
		ret = GFL_G3D_TransVramTexture( bfw->field_resource[ i ] );
		GF_ASSERT( ret == TRUE );

		//RENDER����
		bfw->field_render[ i ] = GFL_G3D_RENDER_Create( bfw->field_resource[ i ], 0, bfw->field_resource[ i ] );

		//OBJ����
		bfw->field_obj[ i ] = GFL_G3D_OBJECT_Create( bfw->field_render[ i ], NULL, 0 );

		bfw->field_status[ i ].trans.x = 0;
		bfw->field_status[ i ].trans.y = 0;
		bfw->field_status[ i ].trans.z = 0;
		bfw->field_status[ i ].scale.x = FX32_ONE;
		bfw->field_status[ i ].scale.y = FX32_ONE;
		bfw->field_status[ i ].scale.z = FX32_ONE;
		MTX_Identity33( &bfw->field_status[ i ].rotate );
	}

	BTL_FIELD_TCBAdd( bfw );

	return bfw;
}

//============================================================================================
/**
 *	�V�X�e���I��
 *
 * @param[in]	bfw	BTL_FIELD�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_FIELD_Exit( BTL_FIELD_WORK *bfw )
{
	int i;

	for( i = 0 ; i < BTL_FIELD_MAX ; i++ ){
		GFL_G3D_DeleteResource( bfw->field_resource[ i ] );
		GFL_G3D_RENDER_Delete( bfw->field_render[ i ] );
		GFL_G3D_OBJECT_Delete( bfw->field_obj[ i ] );
	}

	GFL_HEAP_FreeMemory( bfw );
}

//============================================================================================
/**
 *	�V�X�e�����C��
 *
 * @param[in]	bfw	BTL_FIELD�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_FIELD_Main( BTL_FIELD_WORK *bfw )
{
	int trg = GFL_UI_KEY_GetTrg();

	if( trg & PAD_BUTTON_DEBUG ){
		BTL_FIELD_TCBAdd( bfw );
	}
}

//============================================================================================
/**
 *	�`��
 *
 * @param[in]	bfw	BTL_FIELD�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_FIELD_Draw( BTL_FIELD_WORK *bfw )
{
	int	i;

	for( i = 0 ; i < BTL_FIELD_MAX ; i++ ){
		GFL_G3D_DRAW_DrawObject( bfw->field_obj[ i ], &bfw->field_status[ i ] );
	}
}

//TCB�֐��Q
static	void	BTL_FIELD_TCBAdd( BTL_FIELD_WORK *bfw )
{
	BTL_FIELD_TCB_WORK *bftw = GFL_HEAP_AllocClearMemory( bfw->heapID, sizeof( BTL_FIELD_TCB_WORK ) );

	//�e���ړ��G�t�F�N�g�Z�b�g
	bftw->bfw = bfw;
	bftw->emw.move_type		= EFFTOOL_CALCTYPE_INTERPOLATION;
	bftw->emw.vec_time		= BTL_FIELD_SHADOW_LINE_MOVE_FRAME;
	bftw->emw.vec_time_tmp	= BTL_FIELD_SHADOW_LINE_MOVE_FRAME;
	bftw->emw.wait			= 0;
	bftw->emw.wait_tmp		= BTL_FIELD_SHADOW_LINE_MOVE_WAIT;
	bftw->emw.count			= 0;
	bftw->emw.start_value.x	= 0;
	bftw->emw.start_value.y	= 0;
	bftw->emw.start_value.z	= BTL_FIELD_SHADOW_LINE_START_POS;
	bftw->emw.end_value.x	= 0;
	bftw->emw.end_value.y	= 0;
	bftw->emw.end_value.z	= 0;
	bfw->field_status[ BTL_FIELD_SHADOW_LINE ].trans.z =  BTL_FIELD_SHADOW_LINE_START_POS;
	BTL_EFFTOOL_CalcMoveVector( &bftw->emw.start_value, 
								&bftw->emw.end_value, 
								&bftw->emw.vector, 
								FX32_CONST( BTL_FIELD_SHADOW_LINE_MOVE_FRAME ) );
	GFL_TCB_AddTask( bfw->tcb_sys, BTL_FIELD_TCB_MoveShadowLine, bftw, 0 );
}

//============================================================================================
/**
 *	�e���̈ړ�
 */
//============================================================================================
static	void	BTL_FIELD_TCB_MoveShadowLine( GFL_TCB *tcb, void *work )
{
	BTL_FIELD_TCB_WORK *bftw = ( BTL_FIELD_TCB_WORK * )work;
	BOOL	ret;

	ret = BTL_EFFTOOL_CalcParam( &bftw->emw, &bftw->bfw->field_status[ BTL_FIELD_SHADOW_LINE ].trans );
	if( ret == TRUE ){
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

