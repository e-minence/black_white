
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
#include "btl_field_def.h"

#include "arc_def.h"
#include "battgra/battgra_wb.naix"

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTL_FIELD_WORK
{
	GFL_G3D_RES			*field_resource[ BTL_FIELD_MAX ];
	GFL_G3D_RND			*field_render[ BTL_FIELD_MAX ];
	GFL_G3D_OBJ			*field_obj[ BTL_FIELD_MAX ];
	GFL_G3D_OBJSTATUS	field_status[ BTL_FIELD_MAX ];
	HEAPID				heapID;
};

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

BTL_FIELD_WORK	*BTL_FIELD_Init( int index, HEAPID heapID );
void			BTL_FIELD_Exit( BTL_FIELD_WORK *bsw );
void			BTL_FIELD_Main( BTL_FIELD_WORK *bsw );
void			BTL_FIELD_Draw( BTL_FIELD_WORK *bsw );

//============================================================================================
/**
 *	�w�i�̃��\�[�X�e�[�u��
 */
//============================================================================================
//���f���f�[�^
static	const	int	field_resource_table[][2]={
	{ NARC_battgra_wb_batt_field01_nsbmd, NARC_battgra_wb_batt_bg01_nsbmd },
};

//============================================================================================
/**
 *	�V�X�e��������
 *
 * @param[in]	index	�ǂݍ��ރ��\�[�X��INDEX
 * @param[in]	heapID	�q�[�vID
 */
//============================================================================================
BTL_FIELD_WORK	*BTL_FIELD_Init( int index, HEAPID heapID )
{
	BTL_FIELD_WORK *bfw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTL_FIELD_WORK ) );
	BOOL	ret;
	int		i;

	GF_ASSERT( index < NELEMS( field_resource_table ) );

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

