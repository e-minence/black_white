
//============================================================================================
/**
 * @file	btl_stage.c
 * @brief	�퓬��ʕ`��p�֐��i���~�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================
#include <gflib.h>

#include "btl_stage.h"
#include "btl_stage_def.h"

#include "arc_def.h"
#include "battgra/battgra_wb.naix"

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTL_STAGE_WORK
{
	GFL_G3D_RES			*stage_resource;
	GFL_G3D_RES			*stage_anm_resource;
	GFL_G3D_ANM			*stage_anm;
	GFL_G3D_RND			*stage_render;
	GFL_G3D_OBJ			*stage_obj;
	GFL_G3D_OBJSTATUS	stage_status[ BTL_STAGE_MAX ];
	HEAPID				heapID;
};

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

BTL_STAGE_WORK	*BTL_STAGE_Init( int index, HEAPID heapID );
void			BTL_STAGE_Exit( BTL_STAGE_WORK *bsw );
void			BTL_STAGE_Main( BTL_STAGE_WORK *bsw );
void			BTL_STAGE_Draw( BTL_STAGE_WORK *bsw );

//============================================================================================
/**
 *	���~�̈ʒu�e�[�u��
 */
//============================================================================================
static	const	VecFx32	stage_pos_table[]={
	{ FX_F32_TO_FX32( -3.845f ), 0, FX_F32_TO_FX32(   4.702f ) },
	{ FX_F32_TO_FX32(  4.964f ), 0, FX_F32_TO_FX32( -12.540f ) },
};

//============================================================================================
/**
 *	���~�̃��\�[�X�e�[�u��
 */
//============================================================================================
//���f���f�[�^
static	const	int	stage_resource_table[]={
	NARC_battgra_wb_batt_stage01_nsbmd,
};

//�A�j���f�[�^
static	const	int	stage_anm_resource_table[]={
	NARC_battgra_wb_batt_stage01_nsbca,
};

//�G�b�W�}�[�L���O�J���[
static	const	GXRgb stage_edge_color_table[][8]={
	{ GX_RGB( 0, 0, 0 ), GX_RGB( 6, 6, 6 ), 0, 0, 0, 0, 0, 0 },
};

//============================================================================================
/**
 *	�V�X�e��������
 *
 * @param[in]	index	�ǂݍ��ރ��\�[�X��INDEX
 * @param[in]	heapID	�q�[�vID
 */
//============================================================================================
BTL_STAGE_WORK	*BTL_STAGE_Init( int index, HEAPID heapID )
{
	BTL_STAGE_WORK *bsw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTL_STAGE_WORK ) );
	BOOL	ret;

	GF_ASSERT( index < NELEMS( stage_resource_table ) );
	GF_ASSERT( index < NELEMS( stage_anm_resource_table ) );

	bsw->heapID = heapID;

	//���\�[�X�ǂݍ���
	bsw->stage_resource = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, stage_resource_table[ index ] );
	bsw->stage_anm_resource = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, stage_anm_resource_table[ index ] );
	ret = GFL_G3D_TransVramTexture( bsw->stage_resource );
	GF_ASSERT( ret == TRUE );

	//RENDER����
	bsw->stage_render = GFL_G3D_RENDER_Create( bsw->stage_resource, 0, bsw->stage_resource );

	//ANIME����
	bsw->stage_anm = GFL_G3D_ANIME_Create( bsw->stage_render, bsw->stage_anm_resource, BTL_STAGE_ANM_MAX ); 

	//OBJ����
	bsw->stage_obj = GFL_G3D_OBJECT_Create( bsw->stage_render, &bsw->stage_anm, BTL_STAGE_ANMTBL_MAX );

	//ANIME�N��
	GFL_G3D_OBJECT_EnableAnime( bsw->stage_obj, BTL_STAGE_ANM_NO );

	//���������~
	bsw->stage_status[ BTL_STAGE_MINE ].trans.x = stage_pos_table[ BTL_STAGE_MINE ].x;
	bsw->stage_status[ BTL_STAGE_MINE ].trans.y = stage_pos_table[ BTL_STAGE_MINE ].y;
	bsw->stage_status[ BTL_STAGE_MINE ].trans.z = stage_pos_table[ BTL_STAGE_MINE ].z;
	bsw->stage_status[ BTL_STAGE_MINE ].scale.x = BTL_STAGE_DEFAULT_SCALE;
	bsw->stage_status[ BTL_STAGE_MINE ].scale.y = BTL_STAGE_DEFAULT_SCALE;
	bsw->stage_status[ BTL_STAGE_MINE ].scale.z = BTL_STAGE_DEFAULT_SCALE;
	MTX_Identity33( &bsw->stage_status[ BTL_STAGE_MINE ].rotate );

	//���葤���~
	bsw->stage_status[ BTL_STAGE_ENEMY ].trans.x = stage_pos_table[ BTL_STAGE_ENEMY ].x;
	bsw->stage_status[ BTL_STAGE_ENEMY ].trans.y = stage_pos_table[ BTL_STAGE_ENEMY ].y;
	bsw->stage_status[ BTL_STAGE_ENEMY ].trans.z = stage_pos_table[ BTL_STAGE_ENEMY ].z;
	bsw->stage_status[ BTL_STAGE_ENEMY ].scale.x = BTL_STAGE_DEFAULT_SCALE;
	bsw->stage_status[ BTL_STAGE_ENEMY ].scale.y = BTL_STAGE_DEFAULT_SCALE;
	bsw->stage_status[ BTL_STAGE_ENEMY ].scale.z = BTL_STAGE_DEFAULT_SCALE;
	MTX_Identity33( &bsw->stage_status[ BTL_STAGE_ENEMY ].rotate );

	//�G�b�W�}�[�L���O�J���[�Z�b�g
	G3X_SetEdgeColorTable( &stage_edge_color_table[ index ][ 0 ] );

	return bsw;
}

//============================================================================================
/**
 *	�V�X�e���I��
 *
 * @param[in]	bsw	BTL_STAGE�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_STAGE_Exit( BTL_STAGE_WORK *bsw )
{
	GFL_G3D_DeleteResource( bsw->stage_resource );
	GFL_G3D_DeleteResource( bsw->stage_anm_resource );
	GFL_G3D_RENDER_Delete( bsw->stage_render );
	GFL_G3D_ANIME_Delete( bsw->stage_anm );
	GFL_G3D_OBJECT_Delete( bsw->stage_obj );

	GFL_HEAP_FreeMemory( bsw );
}

//============================================================================================
/**
 *	�V�X�e�����C��
 *
 * @param[in]	bsw	BTL_STAGE�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_STAGE_Main( BTL_STAGE_WORK *bsw )
{
	GFL_G3D_OBJECT_LoopAnimeFrame( bsw->stage_obj, BTL_STAGE_ANM_NO, BTL_STAGE_ANM_WAIT ); 
}

//============================================================================================
/**
 *	�`��
 *
 * @param[in]	bsw	BTL_STAGE�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_STAGE_Draw( BTL_STAGE_WORK *bsw )
{
	int	i;

	for( i = 0 ; i < BTL_STAGE_MAX ; i++ ){
		GFL_G3D_DRAW_DrawObject( bsw->stage_obj, &bsw->stage_status[ i ] );
	}
}

