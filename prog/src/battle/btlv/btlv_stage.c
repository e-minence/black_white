
//============================================================================================
/**
 * @file	btlv_stage.c
 * @brief	�퓬��ʕ`��p�֐��i���~�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================
#include <gflib.h>

#include "btlv_stage.h"
#include "btlv_efftool.h"

#include "arc_def.h"
#include "battle/battgra_wb.naix"

#include "data/btlv_stage.cdat"

//============================================================================================
/**
 *	�萔�錾
 */
//============================================================================================

#define	BTLV_STAGE_DEFAULT_SCALE  ( FX32_ONE )	//���~�̃f�t�H���g�X�P�[���l
#define	BTLV_STAGE_ANM_MAX        ( 0 )	    		//���~�̃A�j���[�V������
                                                //�i�������֐�����P�̎��͂O��v������Ă���̂Ŏ��ۂ͂P�j

#define	BTLV_STAGE_ANM_NO         ( 0 )         //���~�̃A�j���[�V�����i���o�[
#define	BTLV_STAGE_ANM_WAIT       ( FX32_ONE )  //���~�̃A�j���[�V�����E�F�C�g

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTLV_STAGE_WORK
{
	GFL_G3D_RES*            stage_resource;
  int                     anm_count;
	GFL_G3D_RES**           stage_anm_resource;
	GFL_G3D_ANM**           stage_anm;
	GFL_G3D_RND*            stage_render;
	GFL_G3D_OBJ*            stage_obj;
	GFL_G3D_OBJSTATUS       stage_status[ BTLV_STAGE_MAX ];
  EFFTOOL_PAL_FADE_WORK   epfw;
  u32                     vanish_flag :1;
  u32                                 :31;
	HEAPID		  	          heapID;
};

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================
static  void  BTLV_STAGE_CalcPaletteFade( BTLV_STAGE_WORK* bsw );

//============================================================================================
/**
 *	�V�X�e��������
 *
 * @param[in]	index	�ǂݍ��ރ��\�[�X��INDEX
 * @param[in]	heapID	�q�[�vID
 */
//============================================================================================
BTLV_STAGE_WORK	*BTLV_STAGE_Init( int index, HEAPID heapID )
{
	BTLV_STAGE_WORK *bsw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_STAGE_WORK ) );
	BOOL	ret;

	GF_ASSERT( index < NELEMS( stage_resource_table ) );

	bsw->heapID = heapID;

	//���\�[�X�ǂݍ���
	bsw->stage_resource = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, stage_resource_table[ index ]->nsbmd );
	ret = GFL_G3D_TransVramTexture( bsw->stage_resource );
	GF_ASSERT( ret == TRUE );

  bsw->anm_count = stage_resource_table[ index ]->anm_count;

	//RENDER����
	bsw->stage_render = GFL_G3D_RENDER_Create( bsw->stage_resource, 0, bsw->stage_resource );

  if( bsw->anm_count )
  { 
    int i;

    bsw->stage_anm_resource = GFL_HEAP_AllocMemory( bsw->heapID, 4 * bsw->anm_count );
    bsw->stage_anm = GFL_HEAP_AllocMemory( bsw->heapID, 4 * bsw->anm_count );

    for( i = 0 ; i < bsw->anm_count ; i++ )
    { 
		  //ANIME����
	    bsw->stage_anm_resource[ i ] = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, stage_resource_table[ index ]->anm_table[ i ] );
		  bsw->stage_anm[ i ] = GFL_G3D_ANIME_Create( bsw->stage_render, bsw->stage_anm_resource[ i ], BTLV_STAGE_ANM_MAX ); 
    }
		//OBJ����
		bsw->stage_obj = GFL_G3D_OBJECT_Create( bsw->stage_render, bsw->stage_anm, bsw->anm_count );

    //ANIME�N��
    for( i = 0 ; i < bsw->anm_count ; i++ )
    { 
		  GFL_G3D_OBJECT_EnableAnime( bsw->stage_obj, i );
    }
  }
  //�p���b�g�t�F�[�h�p���[�N����
  { 
  	NNSG3dResFileHeader*	header = GFL_G3D_GetResourceFileHeader( bsw->stage_resource );
  	NNSG3dResTex*		    	pTex = NNS_G3dGetTex( header ); 
    u32                   size = (u32)pTex->plttInfo.sizePltt << 3;

    bsw->epfw.g3DRES     = GFL_HEAP_AllocMemory( bsw->heapID, 4 );
    bsw->epfw.pData_dst  = GFL_HEAP_AllocMemory( bsw->heapID, 4 );
    
    bsw->epfw.g3DRES[ 0 ]     = bsw->stage_resource;
    bsw->epfw.pData_dst[ 0 ]  = GFL_HEAP_AllocMemory( bsw->heapID, size );
	  bsw->epfw.pal_fade_flag   = 0;
	  bsw->epfw.pal_fade_count  = 1;
  }

	//���������~
	bsw->stage_status[ BTLV_STAGE_MINE ].trans.x = stage_pos_table[ BTLV_STAGE_MINE ].x;
	bsw->stage_status[ BTLV_STAGE_MINE ].trans.y = stage_pos_table[ BTLV_STAGE_MINE ].y;
	bsw->stage_status[ BTLV_STAGE_MINE ].trans.z = stage_pos_table[ BTLV_STAGE_MINE ].z;
	bsw->stage_status[ BTLV_STAGE_MINE ].scale.x = BTLV_STAGE_DEFAULT_SCALE;
	bsw->stage_status[ BTLV_STAGE_MINE ].scale.y = BTLV_STAGE_DEFAULT_SCALE;
	bsw->stage_status[ BTLV_STAGE_MINE ].scale.z = BTLV_STAGE_DEFAULT_SCALE;
	MTX_Identity33( &bsw->stage_status[ BTLV_STAGE_MINE ].rotate );

	//���葤���~
	bsw->stage_status[ BTLV_STAGE_ENEMY ].trans.x = stage_pos_table[ BTLV_STAGE_ENEMY ].x;
	bsw->stage_status[ BTLV_STAGE_ENEMY ].trans.y = stage_pos_table[ BTLV_STAGE_ENEMY ].y;
	bsw->stage_status[ BTLV_STAGE_ENEMY ].trans.z = stage_pos_table[ BTLV_STAGE_ENEMY ].z;
	bsw->stage_status[ BTLV_STAGE_ENEMY ].scale.x = BTLV_STAGE_DEFAULT_SCALE;
	bsw->stage_status[ BTLV_STAGE_ENEMY ].scale.y = BTLV_STAGE_DEFAULT_SCALE;
	bsw->stage_status[ BTLV_STAGE_ENEMY ].scale.z = BTLV_STAGE_DEFAULT_SCALE;
	MTX_Identity33( &bsw->stage_status[ BTLV_STAGE_ENEMY ].rotate );

	//�G�b�W�}�[�L���O�J���[�Z�b�g
	G3X_SetEdgeColorTable( &stage_resource_table[ index ]->edge_color[ 0 ] );

	return bsw;
}

//============================================================================================
/**
 *	�V�X�e���I��
 *
 * @param[in]	bsw	BTLV_STAGE�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_STAGE_Exit( BTLV_STAGE_WORK* bsw )
{
	GFL_G3D_OBJECT_Delete( bsw->stage_obj );

  if( bsw->anm_count )
  { 
    int i;

    for( i = 0 ; i < bsw->anm_count ; i++ )
    { 
		  GFL_G3D_ANIME_Delete( bsw->stage_anm[ i ] );
		  GFL_G3D_DeleteResource( bsw->stage_anm_resource[ i ] );
    }
		GFL_HEAP_FreeMemory( bsw->stage_anm );
		GFL_HEAP_FreeMemory( bsw->stage_anm_resource );
  }

	GFL_G3D_RENDER_Delete( bsw->stage_render );
	GFL_G3D_DeleteResource( bsw->stage_resource );

  GFL_HEAP_FreeMemory( bsw->epfw.pData_dst[ 0 ] );
  GFL_HEAP_FreeMemory( bsw->epfw.pData_dst );
  GFL_HEAP_FreeMemory( bsw->epfw.g3DRES );

	GFL_HEAP_FreeMemory( bsw );
}

//============================================================================================
/**
 *	�V�X�e�����C��
 *
 * @param[in]	bsw	BTLV_STAGE�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_STAGE_Main( BTLV_STAGE_WORK* bsw )
{
  //�A�j���[�V����
	if(	bsw->anm_count ){
    int i;
    for( i = 0 ; i < bsw->anm_count ; i++ )
    { 
		  GFL_G3D_OBJECT_LoopAnimeFrame( bsw->stage_obj, i, BTLV_STAGE_ANM_WAIT ); 
    }
	}
  //�p���b�g�t�F�[�h
  BTLV_EFFTOOL_CalcPaletteFade( &bsw->epfw );
}

//============================================================================================
/**
 *	�`��
 *
 * @param[in]	bsw	BTLV_STAGE�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_STAGE_Draw( BTLV_STAGE_WORK* bsw )
{
	int	i;

  if( bsw->vanish_flag )
  { 
    return;
  }

	for( i = 0 ; i < BTLV_STAGE_MAX ; i++ ){
		GFL_G3D_DRAW_DrawObject( bsw->stage_obj, &bsw->stage_status[ i ] );
	}
}

//============================================================================================
/**
 *	�p���b�g�t�F�[�h�Z�b�g
 *
 * @param[in]	bsw       BTLV_STAGE�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	start_evy	�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 * @param[in]	end_evy		�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���I������16�i�K�j
 * @param[in]	wait			�Z�b�g����p�����[�^�i�E�F�C�g�j
 * @param[in]	rgb				�Z�b�g����p�����[�^�i�t�F�[�h������F�j
 */
//============================================================================================
void	BTLV_STAGE_SetPaletteFade( BTLV_STAGE_WORK* bsw, u8 start_evy, u8 end_evy, u8 wait, u16 rgb )
{ 
	GF_ASSERT( bsw );

	bsw->epfw.pal_fade_flag      = 1;
	bsw->epfw.pal_fade_start_evy = start_evy;
	bsw->epfw.pal_fade_end_evy   = end_evy;
	bsw->epfw.pal_fade_wait      = 0;
	bsw->epfw.pal_fade_wait_tmp  = wait;
	bsw->epfw.pal_fade_rgb       = rgb;
}

//============================================================================================
/**
 *	�p���b�g�t�F�[�h���s�����`�F�b�N����
 *
 * @param[in]	bsw  BTLV_STAGE�Ǘ����[�N�ւ̃|�C���^
 *
 * @retval  TRUE:���s���@FALSE:�I��
 */
//============================================================================================
BOOL	BTLV_STAGE_CheckExecutePaletteFade( BTLV_STAGE_WORK* bsw )
{ 
  return ( bsw->epfw.pal_fade_flag != 0 );
}

//============================================================================================
/**
 *	�o�j�b�V���t���O�Z�b�g
 *
 * @param[in]	bsw   BTLV_STAGE�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	flag  �Z�b�g����t���O( BTLV_STAGE_VANISH_ON BTLV_STAGE_VANISH_OFF )
 */
//============================================================================================
void	BTLV_STAGE_SetVanishFlag( BTLV_STAGE_WORK* bsw, BTLV_STAGE_VANISH flag )
{ 
  bsw->vanish_flag = flag;
}
