
//============================================================================================
/**
 * @file	btlv_field.c
 * @brief	�퓬��ʕ`��p�֐��i�t�B�[���h�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================

#include <gflib.h>

#include "btlv_effect.h"

#include "arc_def.h"
#include "battgra/battgra_wb.naix"

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTLV_FIELD_WORK
{
	GFL_TCBSYS			*tcb_sys;
	GFL_G3D_RES			*field_resource[ BTLV_FIELD_MAX ];
	GFL_G3D_RND			*field_render[ BTLV_FIELD_MAX ];
	GFL_G3D_OBJ			*field_obj[ BTLV_FIELD_MAX ];
	GFL_G3D_OBJSTATUS	field_status[ BTLV_FIELD_MAX ];
	HEAPID				heapID;
};

typedef	struct
{
	BTLV_FIELD_WORK		*bfw;
	EFFTOOL_MOVE_WORK	emw;
}BTLV_FIELD_TCB_WORK;

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

BTLV_FIELD_WORK	*BTLV_FIELD_Init( GFL_TCBSYS *tcb_sys, int index, HEAPID heapID );
void			BTLV_FIELD_Exit( BTLV_FIELD_WORK *bfw );
void			BTLV_FIELD_Main( BTLV_FIELD_WORK *bfw );
void			BTLV_FIELD_Draw( BTLV_FIELD_WORK *bfw );

//TCB�֐��Q
static	void	BTLV_FIELD_TCBAdd( BTLV_FIELD_WORK *bfw );
static	void	BTLV_FIELD_TCB_MoveShadowLine( GFL_TCB *tcb, void *work );

//============================================================================================
/**
 *	�w�i�̃��\�[�X�e�[�u��
 */
//============================================================================================
//���f���f�[�^
static	const	int	field_resource_table[][BTLV_FIELD_MAX]={
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
BTLV_FIELD_WORK	*BTLV_FIELD_Init( GFL_TCBSYS *tcb_sys, int index, HEAPID heapID )
{
	BTLV_FIELD_WORK *bfw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_FIELD_WORK ) );
	BOOL	ret;
	int		i;

	GF_ASSERT( index < NELEMS( field_resource_table ) );

	bfw->tcb_sys = tcb_sys;
	bfw->heapID = heapID;

	for( i = 0 ; i < BTLV_FIELD_MAX ; i++ ){
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

	BTLV_FIELD_TCBAdd( bfw );

	return bfw;
}

//============================================================================================
/**
 *	�V�X�e���I��
 *
 * @param[in]	bfw	BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_FIELD_Exit( BTLV_FIELD_WORK *bfw )
{
	int i;

	for( i = 0 ; i < BTLV_FIELD_MAX ; i++ ){
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
 * @param[in]	bfw	BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_FIELD_Main( BTLV_FIELD_WORK *bfw )
{
}

//============================================================================================
/**
 *	�`��
 *
 * @param[in]	bfw	BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_FIELD_Draw( BTLV_FIELD_WORK *bfw )
{
	int	i;

	for( i = 0 ; i < BTLV_FIELD_MAX ; i++ ){
		GFL_G3D_DRAW_DrawObject( bfw->field_obj[ i ], &bfw->field_status[ i ] );
	}
}

//TCB�֐��Q
static	void	BTLV_FIELD_TCBAdd( BTLV_FIELD_WORK *bfw )
{
	BTLV_FIELD_TCB_WORK *bftw = GFL_HEAP_AllocClearMemory( bfw->heapID, sizeof( BTLV_FIELD_TCB_WORK ) );

	//�e���ړ��G�t�F�N�g�Z�b�g
	bftw->bfw = bfw;
	bftw->emw.move_type		= EFFTOOL_CALCTYPE_INTERPOLATION;
	bftw->emw.vec_time		= BTLV_FIELD_SHADOW_LINE_MOVE_FRAME;
	bftw->emw.vec_time_tmp	= BTLV_FIELD_SHADOW_LINE_MOVE_FRAME;
	bftw->emw.wait			= 0;
	bftw->emw.wait_tmp		= BTLV_FIELD_SHADOW_LINE_MOVE_WAIT;
	bftw->emw.count			= 0;
	bftw->emw.start_value.x	= 0;
	bftw->emw.start_value.y	= 0;
	bftw->emw.start_value.z	= BTLV_FIELD_SHADOW_LINE_START_POS;
	bftw->emw.end_value.x	= 0;
	bftw->emw.end_value.y	= 0;
	bftw->emw.end_value.z	= 0;
	bfw->field_status[ BTLV_FIELD_SHADOW_LINE ].trans.z =  BTLV_FIELD_SHADOW_LINE_START_POS;
	BTLV_EFFTOOL_CalcMoveVector( &bftw->emw.start_value, 
								&bftw->emw.end_value, 
								&bftw->emw.vector, 
								FX32_CONST( BTLV_FIELD_SHADOW_LINE_MOVE_FRAME ) );
	GFL_TCB_AddTask( bfw->tcb_sys, BTLV_FIELD_TCB_MoveShadowLine, bftw, 0 );
}

//============================================================================================
/**
 *	�e���̈ړ�
 */
//============================================================================================
static	void	BTLV_FIELD_TCB_MoveShadowLine( GFL_TCB *tcb, void *work )
{
	BTLV_FIELD_TCB_WORK *bftw = ( BTLV_FIELD_TCB_WORK * )work;
	BOOL	ret;

	ret = BTLV_EFFTOOL_CalcParam( &bftw->emw, &bftw->bfw->field_status[ BTLV_FIELD_SHADOW_LINE ].trans );
	if( ret == TRUE ){
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

