
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
#include "battle/battgra_wb.naix"

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTLV_FIELD_WORK
{
	GFL_G3D_RES*          field_resource[ BTLV_FIELD_MAX ];
	GFL_G3D_RND*          field_render[ BTLV_FIELD_MAX ];
	GFL_G3D_OBJ*          field_obj[ BTLV_FIELD_MAX ];
	GFL_G3D_OBJSTATUS     field_status[ BTLV_FIELD_MAX ];
  EFFTOOL_PAL_FADE_WORK epfw;
  u32                   vanish_flag :1;
  u32                               :31;
	HEAPID                heapID;
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

//============================================================================================
/**
 *	�w�i�̃��\�[�X�e�[�u��
 */
//============================================================================================
//���f���f�[�^
static	const	int	field_resource_table[][BTLV_FIELD_MAX]={
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
BTLV_FIELD_WORK	*BTLV_FIELD_Init( int index, HEAPID heapID )
{
	BTLV_FIELD_WORK *bfw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_FIELD_WORK ) );
	BOOL	ret;
	int		i;

	GF_ASSERT( index < NELEMS( field_resource_table ) );

	bfw->heapID = heapID;

  bfw->epfw.g3DRES          = GFL_HEAP_AllocMemory( bfw->heapID, 4 * BTLV_FIELD_MAX );
  bfw->epfw.pData_dst       = GFL_HEAP_AllocMemory( bfw->heapID, 4 * BTLV_FIELD_MAX );
  bfw->epfw.pal_fade_flag   = 0;
  bfw->epfw.pal_fade_count  = BTLV_FIELD_MAX;

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
		bfw->field_status[ i ].scale.x = FX32_ONE * 2;
		bfw->field_status[ i ].scale.y = FX32_ONE;
		bfw->field_status[ i ].scale.z = FX32_ONE;
		MTX_Identity33( &bfw->field_status[ i ].rotate );

    //�p���b�g�t�F�[�h�p���[�N����
    { 
      NNSG3dResFileHeader*	header = GFL_G3D_GetResourceFileHeader( bfw->field_resource[ i ] );
      NNSG3dResTex*		    	pTex = NNS_G3dGetTex( header ); 
      u32                   size = (u32)pTex->plttInfo.sizePltt << 3;
    
      bfw->epfw.g3DRES[ i ]     = bfw->field_resource[ i ];
      bfw->epfw.pData_dst[ i ]  = GFL_HEAP_AllocMemory( bfw->heapID, size );
    }
	}

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
    GFL_HEAP_FreeMemory( bfw->epfw.pData_dst[ i ] );
	}

  GFL_HEAP_FreeMemory( bfw->epfw.g3DRES );
  GFL_HEAP_FreeMemory( bfw->epfw.pData_dst );

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
  //�p���b�g�t�F�[�h
  BTLV_EFFTOOL_CalcPaletteFade( &bfw->epfw );
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

  if( bfw->vanish_flag )
  { 
    return;
  }

	for( i = 0 ; i < BTLV_FIELD_MAX ; i++ ){
		GFL_G3D_DRAW_DrawObject( bfw->field_obj[ i ], &bfw->field_status[ i ] );
	}
}

//============================================================================================
/**
 *	�p���b�g�t�F�[�h�Z�b�g
 *
 * @param[in]	bfw	      BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	start_evy	�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 * @param[in]	end_evy		�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���I������16�i�K�j
 * @param[in]	wait			�Z�b�g����p�����[�^�i�E�F�C�g�j
 * @param[in]	rgb				�Z�b�g����p�����[�^�i�t�F�[�h������F�j
 */
//============================================================================================
void	BTLV_FIELD_SetPaletteFade( BTLV_FIELD_WORK *bfw, u8 start_evy, u8 end_evy, u8 wait, u16 rgb )
{ 
	GF_ASSERT( bfw );

	bfw->epfw.pal_fade_flag      = 1;
	bfw->epfw.pal_fade_start_evy = start_evy;
	bfw->epfw.pal_fade_end_evy   = end_evy;
	bfw->epfw.pal_fade_wait      = 0;
	bfw->epfw.pal_fade_wait_tmp  = wait;
	bfw->epfw.pal_fade_rgb       = rgb;
}

//============================================================================================
/**
 *	�p���b�g�t�F�[�h���s�����`�F�b�N����
 *
 * @param[in]	bfw  BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 *
 * @retval  TRUE:���s���@FALSE:�I��
 */
//============================================================================================
BOOL	BTLV_FIELD_CheckExecutePaletteFade( BTLV_FIELD_WORK* bfw )
{ 
  return ( bfw->epfw.pal_fade_flag != 0 );
}

//============================================================================================
/**
 *	�o�j�b�V���t���O�Z�b�g
 *
 * @param[in]	bsw   BTLV_STAGE�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	flag  �Z�b�g����t���O( BTLV_FIELD_VANISH_ON BTLV_FIELD_VANISH_OFF )
 */
//============================================================================================
void	BTLV_FIELD_SetVanishFlag( BTLV_FIELD_WORK* bfw, BTLV_FIELD_VANISH flag )
{ 
  bfw->vanish_flag = flag;
}

