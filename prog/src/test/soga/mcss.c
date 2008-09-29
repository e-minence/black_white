//============================================================================================
/**
 * @file	mcss.c
 * @brief	�}���`�Z���\�t�g�E�G�A�X�v���C�g�`��p�֐�
 * @author	soga
 * @date	2008.09.16
 */
//============================================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>

#include "mcss.h"

//--------------------------------------------------------------------------
/**
 * �v���g�^�C�v�錾
 */
//--------------------------------------------------------------------------
MCSS_SYS_WORK*	MCSS_Init( int max, VecFx32 *camPos,VecFx32 *target, VecFx32 *camUp,HEAPID heapID );
void	MCSS_Exit( MCSS_SYS_WORK *mcss_sys );
void	MCSS_Main( MCSS_SYS_WORK *mcss_sys );
void	MCSS_Draw( MCSS_SYS_WORK *mcss_sys );
MCSS_WORK*	MCSS_Add( MCSS_SYS_WORK *mcss_sys,
					  fx32			pos_x,
					  fx32			pos_y,
					  fx32			pos_z,
					  ARCID			arcID,
					  ARCDATID		ncbr,
					  ARCDATID		nclr,
					  ARCDATID		ncer,
					  ARCDATID		nanr,
					  ARCDATID		nmcr,
					  ARCDATID		nmar );
void	MCSS_Del( MCSS_SYS_WORK *mcss_sys, MCSS_WORK *mcss );

static	void	MCSS_LoadResource( MCSS_SYS_WORK	*mcss_sys,
								   MCSS_WORK		*mcss,
								   int				chr_ofs,
								   int				pal_ofs,
								   ARCID			arcID,
								   ARCDATID			ncbr,
								   ARCDATID			nclr,
								   ARCDATID			ncer,
								   ARCDATID			nanr,
								   ARCDATID			nmcr,
								   ARCDATID			nmar );
//static	NNSG2dMultiCellAnimation*	MCSS_GetNewMultiCellAnimation(MCSS_WORK *mcss, NNSG2dMCType	mcType );
static	void	MCSS_GetNewMultiCellAnimation(MCSS_WORK *mcss, NNSG2dMCType	mcType );
static	void	MCSS_MaterialSetup( void );
static	void	MCSS_InitRenderer( MCSS_SYS_WORK *mcss_sys );
static NNSG2dMultiCellAnimation*     GetNewMultiCellAnim_( u16 num );

//--------------------------------------------------------------------------
/**
 * �V�X�e��������
 */
//--------------------------------------------------------------------------
MCSS_SYS_WORK*	MCSS_Init( int max, VecFx32 *camPos,VecFx32 *target, VecFx32 *camUp,HEAPID heapID )
{
	MCSS_SYS_WORK	*mcss_sys;

	mcss_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(MCSS_SYS_WORK) );

	mcss_sys->max = max;
	mcss_sys->camPos = camPos;
	mcss_sys->target = target;
	mcss_sys->camUp = camUp;
	mcss_sys->heapID = heapID;

	mcss_sys->mcss =GFL_HEAP_AllocClearMemory( heapID, 4 * max );

	MCSS_InitRenderer( mcss_sys );
	
	return mcss_sys;
}

//--------------------------------------------------------------------------
/**
 * �V�X�e���I��
 */
//--------------------------------------------------------------------------
void	MCSS_Exit( MCSS_SYS_WORK *mcss_sys )
{
	int	i;

	for( i = 0 ; i < mcss_sys->max ; i++ ){
		if( mcss_sys->mcss[i] != NULL ){
			MCSS_Del( mcss_sys, mcss_sys->mcss[i] );
		}
	}
	GFL_HEAP_FreeMemory( mcss_sys );
}

//--------------------------------------------------------------------------
/**
 * �V�X�e�����C��
 */
//--------------------------------------------------------------------------
void	MCSS_Main( MCSS_SYS_WORK *mcss_sys )
{
	int	index;

	for( index = 0 ; index < mcss_sys->max ; index++ ){
		if( mcss_sys->mcss[index] != NULL ){
			// �A�j���[�V�������X�V���܂�
			NNS_G2dTickMCAnimation( &mcss_sys->mcss[index]->mcss_mcanim, FX32_ONE );
		}
	}
}

MCSS_WORK	*mcss_p;

//--------------------------------------------------------------------------
/**
 * �`��V�X�e��
 */
//--------------------------------------------------------------------------
void	MCSS_Draw( MCSS_SYS_WORK *mcss_sys )
{
	MCSS_WORK	*mcss;
	int	index;

	for( index = 0 ; index < mcss_sys->max ; index++ ){
		if( mcss_sys->mcss[index] != NULL ){
			mcss_p=mcss_sys->mcss[index];
			mcss=mcss_sys->mcss[index];
			//�b���Renderer���g�p���Ă��邪�A���ۂ͓Ǝ��ŕ`�揈�����쐬���Ȃ���΂Ȃ�Ȃ�
			NNS_G2dBeginRendering( &mcss_sys->mcss_render );
			NNS_G2dPushMtx();
//			G3_LookAt(mcss_sys->camPos, mcss_sys->camUp, mcss_sys->target, &mcss_sys->mcss_render.rendererCore.mtxFor3DGE);
			NNS_G2dTranslate( mcss->pos.x,
							  mcss->pos.y,
							  mcss->pos.z);
			NNS_G2dScale( FX32_ONE, -FX32_ONE , FX32_ONE );
//			NNS_G2dRotZ( FX_SinIdx( cell->rotate ), FX_CosIdx( cell->rotate ) );
			NNS_G2dSetRendererImageProxy( &mcss_sys->mcss_render,
										  &mcss->mcss_image_proxy,
										  &mcss->mcss_palette_proxy );
			NNS_G2dDrawMultiCellAnimation( &mcss->mcss_mcanim );
			NNS_G2dPopMtx();
			NNS_G2dEndRendering();
		}
	}
}

//--------------------------------------------------------------------------
/**
 * �}���`�Z���o�^
 */
//--------------------------------------------------------------------------
MCSS_WORK*	MCSS_Add( MCSS_SYS_WORK *mcss_sys,
					  fx32			pos_x,
					  fx32			pos_y,
					  fx32			pos_z,
					  ARCID			arcID,
					  ARCDATID		ncbr,
					  ARCDATID		nclr,
					  ARCDATID		ncer,
					  ARCDATID		nanr,
					  ARCDATID		nmcr,
					  ARCDATID		nmar )
{
	int			count;

	for( count = 0 ; count < mcss_sys->max ; count++ ){
		if( mcss_sys->mcss[ count ] == NULL ){
			mcss_sys->mcss[ count ] = GFL_HEAP_AllocClearMemory( mcss_sys->heapID, sizeof(MCSS_WORK) );
			mcss_sys->mcss[ count ]->index = count;
			mcss_sys->mcss[ count ]->heapID = mcss_sys->heapID;
			mcss_sys->mcss[ count ]->pos.x = pos_x;
			mcss_sys->mcss[ count ]->pos.y = pos_y;
			mcss_sys->mcss[ count ]->pos.z = pos_z;
			MCSS_LoadResource( mcss_sys,
							   mcss_sys->mcss[ count ],
							   0x8000+0x4000*count,0x100+0x20*count,
							   arcID,
							   ncbr,
							   nclr,
							   ncer,
							   nanr,
							   nmcr,
							   nmar );
			break;
		}
	}
	//�o�^MAX�l���I�[�o�[
	GF_ASSERT( count < mcss_sys->max );

	return mcss_sys->mcss[ count ];
}

//--------------------------------------------------------------------------
/**
 * �}���`�Z���폜
 */
//--------------------------------------------------------------------------
void	MCSS_Del( MCSS_SYS_WORK *mcss_sys, MCSS_WORK *mcss )
{
	GFL_HEAP_FreeMemory( mcss->mcss_ncer_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_nanr_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_nmcr_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_nmar_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_mcanim_buf );

	mcss_sys->mcss[ mcss->index ] = NULL;
	GFL_HEAP_FreeMemory( mcss );
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * ���\�[�X���[�h
 */
//--------------------------------------------------------------------------
static	void	MCSS_LoadResource( MCSS_SYS_WORK	*mcss_sys,
								   MCSS_WORK		*mcss,
								   int				chr_ofs,
								   int				pal_ofs,
								   ARCID			arcID,
								   ARCDATID			ncbr,
								   ARCDATID			nclr,
								   ARCDATID			ncer,
								   ARCDATID			nanr,
								   ARCDATID			nmcr,
								   ARCDATID			nmar )
{
	//�v���L�V������
	NNS_G2dInitImageProxy( &mcss->mcss_image_proxy );
	NNS_G2dInitImagePaletteProxy( &mcss->mcss_palette_proxy );

	// �Z���f�[�^�A�Z���A�j���[�V�����A�}���`�Z���f�[�^�A
	// �}���`�Z���A�j���[�V���������[�h�B
	mcss->mcss_ncer_buf = GFL_ARC_UTIL_LoadCellBank(		arcID, ncer, FALSE, &mcss->mcss_ncer, mcss->heapID );
	GF_ASSERT( mcss->mcss_ncer_buf != NULL );
	mcss->mcss_nanr_buf = GFL_ARC_UTIL_LoadAnimeBank(		arcID, nanr, FALSE, &mcss->mcss_nanr, mcss->heapID );
	GF_ASSERT( mcss->mcss_nanr_buf != NULL );
	mcss->mcss_nmcr_buf = GFL_ARC_UTIL_LoadMultiCellBank(	arcID, nmcr, FALSE, &mcss->mcss_nmcr, mcss->heapID );
	GF_ASSERT( mcss->mcss_nmcr_buf != NULL );
	mcss->mcss_nmar_buf = GFL_ARC_UTIL_LoadMultiAnimeBank(	arcID, nmar, FALSE, &mcss->mcss_nmar, mcss->heapID );
	GF_ASSERT( mcss->mcss_nmar_buf != NULL );

	//
	// �}���`�Z���A�j���[�V�����̎��̂����������܂�
	//
	{
		const NNSG2dMultiCellAnimSequence* pSequence;

		// �Đ�����V�[�P���X���擾
		pSequence = NNS_G2dGetAnimSequenceByIdx( mcss->mcss_nmar, 0 );
		GF_ASSERT( pSequence != NULL );

		// �}���`�Z���A�j���[�V�������\�z
		MCSS_GetNewMultiCellAnimation( mcss, NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM);

		// �}���`�Z���A�j���[�V�����ɍĐ�����V�[�P���X���Z�b�g
		NNS_G2dSetAnimSequenceToMCAnimation( &mcss->mcss_mcanim, pSequence);
	}

    //
    // VRAM �֘A�̏�����
    //
    {
		void* pBuf;

//���ʁAOAM�ł̕\���͂��Ȃ��̂ŃR�����g�A�E�g
#if 0
		// load character data for 2D
		{
			NNSG2dCharacterData*	pCharData = NULL;

			pBuf = GFL_ARC_UTIL_LoadBGCharacter( arcID, ncgr, FALSE, &pCharData, mcss->heapID );
			GF_ASSERT( pBuf != NULL);

			// Loading For 2D Graphics Engine.�i�{���́AVRAM�}�l�[�W�����g�p�������j
			NNS_G2dLoadImage2DMapping(
				pCharData,
				CHARA_BASE,
				NNS_G2D_VRAM_TYPE_2DMAIN,
				&mcss_sys->mcss_image_proxy );

			// �L�����N�^�f�[�^�� VRAM �ɃR�s�[�����̂�
			// ���� pBuf �͊J�����܂��B�ȉ������B
			GFL_HEAP_FreeMemory( pBuf );
		}
#endif

		// load character data for 3D (software sprite)
		{
			NNSG2dCharacterData* pCharData;

			pBuf = GFL_ARC_UTIL_LoadBGCharacter( arcID, ncbr, FALSE, &pCharData, mcss->heapID );
			GF_ASSERT( pBuf != NULL);

			// Loading For 3D Graphics Engine.�i�{���́AVRAM�}�l�[�W�����g�p�������j
			NNS_G2dLoadImage2DMapping(
				pCharData,
				chr_ofs,
				NNS_G2D_VRAM_TYPE_3DMAIN,
				&mcss->mcss_image_proxy );

			GFL_HEAP_FreeMemory( pBuf );
        }

		// load palette data
		{
			NNSG2dPaletteData* pPlttData;

			pBuf = GFL_ARC_UTIL_LoadPalette( arcID, nclr, &pPlttData, mcss->heapID );
			GF_ASSERT( pBuf != NULL);

//���ʁAOAM�ł̕\���͂��Ȃ��̂ŃR�����g�A�E�g
#if 0
			// Loading For 2D Graphics Engine.
			NNS_G2dLoadPalette(
				pPlttData,
				PLTT_BASE,
				NNS_G2D_VRAM_TYPE_2DMAIN,
				&mcss_sys->mcss_palette_proxy );
#endif

            // Loading For 3D Graphics Engine.
			NNS_G2dLoadPalette(
				pPlttData,
				pal_ofs,
				NNS_G2D_VRAM_TYPE_3DMAIN,
				&mcss->mcss_palette_proxy );

			GFL_HEAP_FreeMemory( pBuf );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MCSS_GetNewMultiCellAnimation

  Description:  �V���� NNSG2dMultiCellAnimation ���擾���ď��������܂��B

  Arguments:    pAnimBank:          �A�j���[�V�����f�[�^�o���N
                pCellDataBank:      �Z���f�[�^�o���N
                pMultiCellDataBank: �}���`�Z���f�[�^�o���N
                mcType:             �}���`�Z�����̂̎��
                
  Returns:      �v�[������擾�����������ς݂� NNSG2dMultiCellAnimation ��
                �̃|�C���^
 *---------------------------------------------------------------------------*/
static	void	MCSS_GetNewMultiCellAnimation(MCSS_WORK *mcss, NNSG2dMCType	mcType )
{
	const u32 szWork = NNS_G2dGetMCWorkAreaSize( mcss->mcss_nmcr, mcType );
	mcss->mcss_mcanim_buf = GFL_HEAP_AllocMemory( mcss->heapID, szWork );

	// ������
	NNS_G2dInitMCAnimationInstance( &mcss->mcss_mcanim, 
                                    mcss->mcss_mcanim_buf,
                                    mcss->mcss_nanr, 
                                    mcss->mcss_ncer, 
                                    mcss->mcss_nmcr, 
                                    mcType );
}

/*---------------------------------------------------------------------------*
  Name:         G2DDemo_MaterialSetup

  Description:  �}�e���A���̋��ʐݒ���s���܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static	void	MCSS_MaterialSetup(void)
{
	// for software sprite-setting
	{
		G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31),		// diffuse
								GX_RGB(16, 16, 16),		// ambient
								TRUE					// use diffuse as vtx color if TRUE
								);

		G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),		// specular
								GX_RGB( 0,  0,  0),		// emission
                                FALSE					// use shininess table if TRUE
                                );

		G3_PolygonAttr(GX_LIGHTMASK_NONE,				// no lights
					   GX_POLYGONMODE_MODULATE,			// modulation mode
					   GX_CULL_NONE,					// cull back
					   0,								// polygon ID(0 - 63)
					   31,								// alpha(0 - 31)
					   0								// OR of GXPolygonAttrMisc's value
					   );
	}
}

/*---------------------------------------------------------------------------*
  Name:         MCSS_InitRenderer

  Description:  Renderer �� Surface �����������܂��B

  Arguments:    pRenderer:  ���������� Renderer �ւ̃|�C���^�B
                pSurface:   ���������� Surface �ւ̃|�C���^�B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static	void	MCSS_InitRenderer( MCSS_SYS_WORK *mcss_sys )
{
	NNSG2dViewRect* pRect = &(mcss_sys->mcss_surface.viewRect);
 
	NNS_G2dInitRenderer( &mcss_sys->mcss_render );
	NNS_G2dInitRenderSurface( &mcss_sys->mcss_surface );

	pRect->posTopLeft.x = FX32_ONE * 0;
	pRect->posTopLeft.y = FX32_ONE * 0;
	pRect->sizeView.x = FX32_ONE * SCREEN_WIDTH;
	pRect->sizeView.y = FX32_ONE * SCREEN_HEIGHT;

	//�Ƃ肠����OAM�`��͂��Ȃ��̂ŁA�R�����g�A�E�g
//	pSurface->pFuncOamRegister       = CallBackAddOam;
//	pSurface->pFuncOamAffineRegister = CallBackAddAffine;

	NNS_G2dAddRendererTargetSurface( &mcss_sys->mcss_render, &mcss_sys->mcss_surface );
	NNS_G2dSetRendererSpriteZoffset( &mcss_sys->mcss_render, DEFAULT_Z_OFFSET );

	mcss_sys->mcss_surface.type = NNS_G2D_SURFACETYPE_MAIN3D;
}

