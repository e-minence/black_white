
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
#include <math.h>

#include "arc_def.h"
#include "system/palanm.h"
#include "battle/battgra_wb.naix"

//#define	USE_RENDER		      //�L���ɂ��邱�Ƃ�NNS�̃����_�����g�p���ĕ`�悷��
#include "system/mcss.h"	    //������USE_RENDER���Q�Ƃ��Ă���̂ł�������Ɉړ��͕s��
#include "system/mcss_tool.h"	//������USE_RENDER���Q�Ƃ��Ă���̂ł�������Ɉړ��͕s��
#include "mcss_def.h"

#define	MCSS_DEFAULT_SHIFT		( FX32_SHIFT - 4 )		//�|���S���P�ӂ̊�̒����ɂ���V�t�g�l
#define	MCSS_DEFAULT_LINE		( 1 << MCSS_DEFAULT_SHIFT )	//�|���S���P�ӂ̊�̒���
#define	MCSS_DEFAULT_Z			( 1 << 8 )
#define	MCSS_DEFAULT_Z_ORTHO	( 1 << 10 )

#define	MCSS_CONST(x)	( x << MCSS_DEFAULT_SHIFT )

#define	MCSS_TEX_ADRS	(0x24000)
#define	MCSS_TEX_SIZE	(0x4000)
#define	MCSS_PAL_ADRS	(0x1000)
#define	MCSS_PAL_SIZE	(0x0020)

#define	MCSS_NORMAL_MTX	( 29 )
#define	MCSS_SHADOW_MTX	( 30 )

#define	MCSS_DEFAULT_SHADOW_ROTATE	( 0xC400 )

#define	MCSS_DEFAULT_SHADOW_OFFSET	( -0x1000 )	//�e�ʒu��Z�����̃I�t�Z�b�g�l

#define MCSS_VCOUNT_BORDER          ( 213 )     //�e�N�X�`���]������Ƃ���VCOUNT�̋��E
                                                //(192~213�������_�����O�G���W���̃u�����N���ԁj

//--------------------------------------------------------------------------
/**
 * �\���̐錾
 */
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * �v���g�^�C�v�錾
 */
//--------------------------------------------------------------------------
static	void	MCSS_DrawAct( MCSS_WORK *mcss, 
							  fx32 pos_x,
							  fx32 pos_y,
							  fx32 scale_x,
							  fx32 scale_y,
							  fx32 tex_s,
							  fx32 tex_t,
							  NNSG2dAnimDataSRT *anim_SRT_c,
							  NNSG2dAnimDataSRT *anim_SRT_mc,
							  NNSG2dImagePaletteProxy *shadow_palette,
							  int node,
							  u32 mcss_ortho_mode,
							  fx32 *pos_z_default );

static	void	MCSS_LoadResource( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_WORK *maw );
static	void	MCSS_GetNewMultiCellAnimation(MCSS_WORK *mcss, NNSG2dMCType	mcType );
static	void	MCSS_MaterialSetup( void );
static	NNSG2dMultiCellAnimation*     GetNewMultiCellAnim_( u16 num );
static	void	MCSS_CalcPaletteFade( MCSS_WORK *mcss );
static  void  MCSS_FreeResource( MCSS_WORK* mcss );

static	void	TCB_LoadResource( GFL_TCB *tcb, void *work );
static	void	TCB_LoadPalette( GFL_TCB *tcb, void *work );
static  void  MCSS_CalcMosaic( MCSS_WORK* mcss, TCB_LOADRESOURCE_WORK* tlw );
static  inline  void  mosaic_make( u8 *buf, int pos_x, int pos_y, int tex_x, int size_x, int size_y, int col );
static  inline  int  mosaic_pos_get( int pos_x, int pos_y, int tex_x );
static  inline  u8  mosaic_col_get( u8 *buf, int pos_x, int pos_y, int tex_x, int size_x, int size_y );

static	void MTX_MultVec44( const VecFx32 *cp_src, const MtxFx44 *cp_m, VecFx32 *p_dst, fx32 *p_w );

#ifdef USE_RENDER
static	void	MCSS_InitRenderer( MCSS_SYS_WORK *mcss_sys );
#endif //USE_RENDER


#ifdef PM_DEBUG
MCSS_WORK*		MCSS_AddDebug( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, const MCSS_ADD_DEBUG_WORK *madw );
static	void	MCSS_LoadResourceDebug( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_DEBUG_WORK *madw );
#endif

//--------------------------------------------------------------------------
/**
 * @brief �V�X�e��������
 *
 * @param[in] max     �V�X�e���Ŏg�p�ł���MCSS��MAX�l
 * @param[in] heapID  �V�X�e���Ŏg�p����q�[�vID
 *
 * @retval  MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
MCSS_SYS_WORK*	MCSS_Init( int max, HEAPID heapID )
{
	MCSS_SYS_WORK	*mcss_sys;

	mcss_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(MCSS_SYS_WORK) );

	mcss_sys->mcss_max		= max;
	mcss_sys->heapID		= heapID;

	mcss_sys->mcss =GFL_HEAP_AllocClearMemory( heapID, sizeof(MCSS_WORK *) * max );

#ifdef USE_RENDER
	MCSS_InitRenderer( mcss_sys );
#endif //USE_RENDER

	//�e�N�X�`���n�̓]����A�h���X���Z�b�g
	mcss_sys->texAdrs = MCSS_TEX_ADRS;
	mcss_sys->palAdrs = MCSS_PAL_ADRS;

	//�e���\�[�X���[�h
	NNS_G2dInitImagePaletteProxy( &mcss_sys->shadow_palette_proxy );

	// load palette data
	{
		TCB_LOADRESOURCE_WORK *tlw = GFL_HEAP_AllocClearMemory( heapID, sizeof( TCB_LOADRESOURCE_WORK ) );

		tlw->palette_p = &mcss_sys->shadow_palette_proxy;
		tlw->pal_ofs = MCSS_PAL_ADRS + 0x100;
		tlw->pBufPltt = GFL_ARC_UTIL_LoadPalette( ARCID_BATTGRA, NARC_battgra_wb_shadow_NCLR, &tlw->pPlttData, heapID );
		GF_ASSERT( tlw->pBufPltt != NULL);

		GFUser_VIntr_CreateTCB( TCB_LoadResource, tlw, 0 );
	}
	
	return mcss_sys;
}

//--------------------------------------------------------------------------
/**
 * @brief �V�X�e���I��
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_Exit( MCSS_SYS_WORK *mcss_sys )
{
	int	i;

	for( i = 0 ; i < mcss_sys->mcss_max ; i++ ){
		if( mcss_sys->mcss[i] != NULL ){
			MCSS_Del( mcss_sys, mcss_sys->mcss[i] );
		}
	}
	GFL_HEAP_FreeMemory( mcss_sys->mcss );
	GFL_HEAP_FreeMemory( mcss_sys );
}

//--------------------------------------------------------------------------
/**
 * @brief �V�X�e�����C��
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_Main( MCSS_SYS_WORK *mcss_sys )
{
	int	index;

	for( index = 0 ; index < mcss_sys->mcss_max ; index++ ){
		if( ( mcss_sys->mcss[ index ] != NULL ) )
    { 
      if( mcss_sys->mcss[ index ]->anm_stop_flag == 0 ){
  			// �A�j���[�V�������X�V���܂�
  			NNS_G2dTickMCAnimation( &mcss_sys->mcss[ index ]->mcss_mcanim, FX32_ONE );
      }
			//�p���b�g�t�F�[�h�`�F�b�N
			if( mcss_sys->mcss[ index ]->pal_fade_flag )
			{	
				MCSS_CalcPaletteFade( mcss_sys->mcss[ index ] );
			}
    }
	}
}

//--------------------------------------------------------------------------
/**
 * @brief LoadResource�����O�ɌĂяo�����R�[���o�b�N�֐��̐ݒ�
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]  func     �Ăяo���R�[���o�b�N�֐�
 * @param[in]  work     �Ăяo���R�[���o�b�N�֐��̈���
 */
//--------------------------------------------------------------------------
void	MCSS_SetCallBackFunc( MCSS_SYS_WORK *mcss_sys, MCSS_CALLBACK_FUNC* func, void* work )
{ 
  mcss_sys->load_resource_callback = func;
  mcss_sys->callback_work = work;
}

//--------------------------------------------------------------------------
/**
 * @brief �R�[���o�b�N���[�N�̐ݒ�
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]  work     �Ăяo���R�[���o�b�N�֐��̈���
 */
//--------------------------------------------------------------------------
void	MCSS_SetCallBackWork( MCSS_SYS_WORK *mcss_sys, void* work )
{ 
  mcss_sys->callback_work = work;
}

//--------------------------------------------------------------------------
/**
 * @brief ���\�[�X�̍ēǂݍ���
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]  mcss     �ēǂݍ��݂���mcss
 * @param[in]  maw      �ēǂݍ��݂��郊�\�[�X
 */
//--------------------------------------------------------------------------
void	MCSS_ReloadResource( MCSS_SYS_WORK *mcss_sys, MCSS_WORK* mcss, MCSS_ADD_WORK* maw )
{ 
  MCSS_FreeResource( mcss );
	mcss->maw = *maw;
  MCSS_LoadResource( mcss_sys, mcss->index, maw );
}

#ifndef USE_RENDER
//�Ǝ��`��
//--------------------------------------------------------------------------
/**
 * @brief �`��V�X�e��
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_Draw( MCSS_SYS_WORK *mcss_sys )
{
	MCSS_WORK									*mcss;
	MCSS_NCEC									*ncec;
	NNSG2dImageProxy					*image_p;
	NNSG2dImagePaletteProxy		*palette_p;
	int												index;
	int												i,node,cell;
	fx32											pos_z_default;
	NNSG2dAnimController			*anim_ctrl_mc;
	NNSG2dAnimDataSRT					anim_SRT_mc;
	NNSG2dAnimController			*anim_ctrl_c;
	NNSG2dAnimDataT						*anim_T_p;
	NNSG2dAnimDataSRT					anim_SRT;
	NNSG2dMCNodeCellAnimArray	*MC_Array;
	NNSG2dAnimSequenceData		*anim;
	VecFx32										pos,anim_pos;
	MtxFx44										inv_camera;
	u16												rotate;

	G3_PushMtx();
	G3_MtxMode( GX_MTXMODE_PROJECTION );
	G3_StoreMtx( 0 );
	G3_MtxMode( GX_MTXMODE_TEXTURE );
	G3_Identity();
	G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );

	if( mcss_sys->mcss_ortho_mode == 0 ){
		G3_LookAt( NNS_G3dGlbGetCameraPos(), NNS_G3dGlbGetCameraUp(), NNS_G3dGlbGetCameraTarget(), NULL );
	}
	else{
		// �J�����s���ݒ肵�܂��B
		// �P�ʍs��Ɠ���
		VecFx32 Eye    = { 0, 0, 0 };          // Eye position
		VecFx32 vUp    = { 0, FX32_ONE, 0 };  // Up
		VecFx32 at     = { 0, 0, -FX32_ONE }; // Viewpoint

		G3_LookAt( &Eye, &vUp, &at, NULL );
	}

	//�r���{�[�h��]�s������߂�
	{
		MtxFx43						camera;
		MtxFx33						rotate;

		MTX_Inverse43( NNS_G3dGlbGetCameraMtx(), &camera );	//�J�����t�s��擾
		MTX_Copy43To33( &camera, &rotate );		//�J�����t�s�񂩂��]�s������o��
		MTX_Copy33To44( &rotate, &inv_camera );
	}

	for( index = 0 ; index < mcss_sys->mcss_max ; index++ ){
		if( ( mcss_sys->mcss[index] != NULL ) && 
		    ( mcss_sys->mcss[index]->vanish_flag == 0 ) &&
		    ( mcss_sys->mcss[index]->is_load_resource == 1 ) ){

			G3_PushMtx();

			mcss		= mcss_sys->mcss[index];
			image_p		= &mcss->mcss_image_proxy;
			pos_z_default = 0;

			anim_ctrl_mc= NNS_G2dGetMCAnimAnimCtrl(&mcss->mcss_mcanim);
			MC_Array = (NNSG2dMCNodeCellAnimArray*)&mcss->mcss_mcanim.multiCellInstance.pCellAnimInstasnces;

			switch( anim_ctrl_mc->pAnimSequence->animType & 0xffff ){
			case NNS_G2D_ANIMELEMENT_INDEX:		// Index �̂�
				anim_SRT_mc.index	= *( (u16 *)anim_ctrl_mc->pCurrent->pContent );
				anim_SRT_mc.rotZ	= 0;
				anim_SRT_mc.sx		= 0x1000;
				anim_SRT_mc.sy		= 0x1000;
				anim_SRT_mc.px		= 0;
				anim_SRT_mc.py		= 0;
				break;
			case NNS_G2D_ANIMELEMENT_INDEX_SRT:	// Index + SRT 
				anim_SRT_mc = *( (NNSG2dAnimDataSRT *)anim_ctrl_mc->pCurrent->pContent );
				break;
			case NNS_G2D_ANIMELEMENT_INDEX_T:	// Index + T 
				anim_T_p			= anim_ctrl_mc->pCurrent->pContent;
				anim_SRT_mc.index	= anim_T_p->index;
				anim_SRT_mc.rotZ	= 0;
				anim_SRT_mc.sx		= 0x1000;
				anim_SRT_mc.sy		= 0x1000;
				anim_SRT_mc.px		= anim_T_p->px;
				anim_SRT_mc.py		= anim_T_p->py;
				break;
			default:
				GF_ASSERT(0);
				break;
			}

			pos.x = mcss->pos.x + mcss->ofs_pos.x;
			pos.y = mcss->pos.y + mcss->ofs_pos.y;
			pos.z = mcss->pos.z + mcss->ofs_pos.z;

			if( mcss_sys->mcss_ortho_mode == 0 ){
				anim_pos.x = MCSS_CONST( anim_SRT_mc.px );
				anim_pos.y = MCSS_CONST( -anim_SRT_mc.py );
			}
			else{
      	MtxFx44	mtx;
	      fx32	w;

	      MTX_Copy43To44( NNS_G3dGlbGetCameraMtx(), &mtx );
	      MTX_Concat44( &mtx, NNS_G3dGlbGetProjectionMtx(), &mtx );

	      MTX_MultVec44( &pos, &mtx, &pos, &w );

	      pos.x = FX_Mul( FX_Div( pos.x, w ), 128 * FX32_ONE );
	      pos.y = FX_Mul( FX_Div( pos.y, w ),  96 * FX32_ONE );
        pos.z = -pos.z;

				anim_pos.x = FX32_CONST( anim_SRT_mc.px );
				anim_pos.y = FX32_CONST( -anim_SRT_mc.py );
				anim_pos.x = FX_Mul( anim_pos.x, mcss->scale.x / 16 );
				anim_pos.y = FX_Mul( anim_pos.y, mcss->scale.y / 16 );
			}

			//�O�����āA�s�ςȃ}���`�Z���f�[�^���J�����g�s��ɂ����Ă���
			G3_Translate( pos.x, pos.y, pos.z );
			//�J�����̋t�s����|�����킹��i�r���{�[�h�����j
			if( mcss_sys->mcss_ortho_mode == 0 ){
				G3_MultMtx44( &inv_camera );
			}

			rotate = mcss->rotate.z >> FX32_SHIFT;

			G3_Translate( anim_pos.x, anim_pos.y, 0 );
			G3_RotZ( -FX_SinIdx( anim_SRT_mc.rotZ + rotate ), FX_CosIdx( anim_SRT_mc.rotZ + rotate ) );
			{	
				VecFx32	scale;

				scale.x = FX_Mul( anim_SRT_mc.sx, mcss->scale.x );
				scale.y = FX_Mul( anim_SRT_mc.sy, mcss->scale.y );

				G3_Scale( FX_Mul( scale.x, mcss->ofs_scale.x ), FX_Mul( scale.y, mcss->ofs_scale.y ), FX32_ONE );
			}

			G3_StoreMtx( MCSS_NORMAL_MTX );

			//�e�`��p�̍s�񐶐�
			G3_LookAt( NNS_G3dGlbGetCameraPos(), NNS_G3dGlbGetCameraUp(), NNS_G3dGlbGetCameraTarget(), NULL );

			//�O�����āA�s�ςȃ}���`�Z���f�[�^���J�����g�s��ɂ����Ă���
			G3_Translate( mcss->pos.x + mcss->ofs_pos.x + mcss->shadow_offset.x, 
			              mcss->pos.y + mcss->shadow_offset.y , 
			              mcss->pos.z + mcss->ofs_pos.z + mcss->shadow_offset.z );

			//�e�p�̉�]
			G3_RotX( FX_SinIdx( mcss->shadow_rotate ), FX_CosIdx( mcss->shadow_rotate ) );
			G3_Translate( MCSS_CONST( anim_SRT_mc.px ), MCSS_CONST( -anim_SRT_mc.py ), 0 );
			G3_RotZ( -FX_SinIdx( anim_SRT_mc.rotZ + rotate ), FX_CosIdx( anim_SRT_mc.rotZ + rotate ) );
			{	
				VecFx32	scale;

				scale.x = FX_Mul( anim_SRT_mc.sx, mcss->shadow_scale.x );
				scale.y = FX_Mul( anim_SRT_mc.sy, mcss->shadow_scale.y );

				G3_Scale( FX_Mul( scale.x, mcss->ofs_scale.x ), FX_Mul( scale.y, mcss->ofs_scale.y ), FX32_ONE );
			}

			G3_StoreMtx( MCSS_SHADOW_MTX );

			MCSS_MaterialSetup();

			G3_TexImageParam(image_p->attr.fmt,
							 GX_TEXGEN_TEXCOORD,
							 image_p->attr.sizeS,
							 image_p->attr.sizeT,
							 GX_TEXREPEAT_ST,
							 GX_TEXFLIP_NONE,
							 image_p->attr.plttUse,
							 image_p->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_3DMAIN]);
			for( i = 0 ; i < mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc.index].numNodes ; i++ ){
        //�������Ȃ�t����`�悷��悤�ɂ���
        if( mcss->alpha != 31 )
        { 
          node = ( mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc.index].numNodes - 1 ) - i;
        }
        else
        { 
          node = i;
        }
				cell = (mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc.index].pHierDataArray[node].nodeAttr & 0xff00 ) >> 8;
				anim_ctrl_c = NNS_G2dGetCellAnimAnimCtrl(&MC_Array->cellAnimArray[cell].cellAnim);
				switch( anim_ctrl_c->pAnimSequence->animType & 0xffff ){
				case NNS_G2D_ANIMELEMENT_INDEX:		// Index �̂�
					anim_SRT.index	= *( (u16 *)anim_ctrl_c->pCurrent->pContent );
					anim_SRT.rotZ	= 0;
					anim_SRT.sx		= 0x1000;
					anim_SRT.sy		= 0x1000;
					anim_SRT.px		= 0;
					anim_SRT.py		= 0;
					break;
				case NNS_G2D_ANIMELEMENT_INDEX_SRT:	// Index + SRT 
					anim_SRT = *( (NNSG2dAnimDataSRT *)anim_ctrl_c->pCurrent->pContent );
					break;
				case NNS_G2D_ANIMELEMENT_INDEX_T:	// Index + T 
					anim_T_p		= anim_ctrl_c->pCurrent->pContent;
					anim_SRT.index	= anim_T_p->index;
					anim_SRT.rotZ	= 0;
					anim_SRT.sx		= 0x1000;
					anim_SRT.sy		= 0x1000;
					anim_SRT.px		= anim_T_p->px;
					anim_SRT.py		= anim_T_p->py;
					break;
				default:
					GF_ASSERT(0);
					break;
				}

				ncec = &mcss->mcss_ncec->ncec[anim_SRT.index];

				//���p�`����
				if( ncec->mepachi_size_x ){
					if( mcss->mepachi_flag ){
						MCSS_DrawAct( mcss,
									  ncec->mepachi_pos_x,
									  ncec->mepachi_pos_y,
									  ncec->mepachi_size_x,
									  ncec->mepachi_size_y,
									  ncec->mepachi_tex_s,
									  ncec->mepachi_tex_t + ncec->mepachi_size_y,
									  &anim_SRT, &anim_SRT_mc, &mcss_sys->shadow_palette_proxy, node,
									  mcss_sys->mcss_ortho_mode,
									  &pos_z_default );
					}
					else{
						MCSS_DrawAct( mcss,
									  ncec->mepachi_pos_x,
									  ncec->mepachi_pos_y,
									  ncec->mepachi_size_x,
									  ncec->mepachi_size_y,
									  ncec->mepachi_tex_s,
									  ncec->mepachi_tex_t,
									  &anim_SRT, &anim_SRT_mc, &mcss_sys->shadow_palette_proxy, node,
									  mcss_sys->mcss_ortho_mode,
									  &pos_z_default );
					}
				}
				MCSS_DrawAct( mcss,
							  ncec->pos_x,
							  ncec->pos_y,
							  ncec->size_x,
							  ncec->size_y,
							  ncec->tex_s,
							  ncec->tex_t,
							  &anim_SRT, &anim_SRT_mc, &mcss_sys->shadow_palette_proxy, node,
							  mcss_sys->mcss_ortho_mode,
							  &pos_z_default );
			}
			G3_PopMtx(1);
		}
	}
	G3_PopMtx(1);
}
#else
//�����_���V�X�e����p�����`��
//--------------------------------------------------------------------------
/**
 * @brief �`��V�X�e��
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_Draw( MCSS_SYS_WORK *mcss_sys )
{
	MCSS_WORK	*mcss;
	int			index;
	int			cell;
	VecFx32		pos,scale;

	NNS_G2dSetupSoftwareSpriteCamera();
	G3_MtxMode( GX_MTXMODE_TEXTURE );
	G3_Identity();
	G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );

	for( index = 0 ; index < mcss_sys->mcss_max ; index++ ){
		if( mcss_sys->mcss[index] != NULL ){
			mcss=mcss_sys->mcss[index];
			//�b���Renderer���g�p���Ă��邪�A���ۂ͓Ǝ��ŕ`�揈�����쐬���Ȃ���΂Ȃ�Ȃ�
			NNS_G2dBeginRendering( &mcss_sys->mcss_render );
			NNS_G2dPushMtx();
			NNS_G2dTranslate( mcss->pos.x,
							  mcss->pos.y,
							  mcss->pos.z);
			NNS_G2dSetRendererImageProxy( &mcss_sys->mcss_render,
										  &mcss->mcss_image_proxy,
										  &mcss->mcss_palette_proxy );
			NNS_G2dDrawMultiCellAnimation( &mcss->mcss_mcanim );
			NNS_G2dPopMtx();
			NNS_G2dEndRendering();
		}
	}
}
#endif

//--------------------------------------------------------------------------
/**
 * @brief �}���`�Z����`��
 *
 * @param[in]  mcss             MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos_x            �`��X���W
 * @param[in]  pos_y            �`��Y���W
 * @param[in]  scale_x          X�����X�P�[��
 * @param[in]  scale_y          Y�����X�P�[��
 * @param[in]  tex_s            �e�N�X�`��S�l
 * @param[in]  tex_t            �e�N�X�`��T�l
 * @param[in]  anm_SRT_c        �Z���A�j���f�[�^�\���̂̃|�C���^
 * @param[in]  anm_SRT_mc       �}���`�Z���A�j���f�[�^�\���̂̃|�C���^
 * @param[in]  shadow_palette   �e�p���b�g�v���L�V
 * @param[in]  node             �}���`�Z���̃m�[�h
 * @param[in]  mcss_ortho_mode  �ˉe���[�h�t���O�i0:�����ˉe�@1:���ˉe�j
 * @param[in]  pos_z_default    �Z����`�悷��x�ɂ��炸Z�����̃I�t�Z�b�g�l
 */
//--------------------------------------------------------------------------
static	void	MCSS_DrawAct( MCSS_WORK *mcss,
							  fx32 pos_x,
							  fx32 pos_y,
							  fx32 scale_x,
							  fx32 scale_y,
							  fx32 tex_s,
							  fx32 tex_t,
							  NNSG2dAnimDataSRT *anim_SRT_c,
							  NNSG2dAnimDataSRT *anim_SRT_mc,
							  NNSG2dImagePaletteProxy *shadow_palette,
							  int node,
							  u32 mcss_ortho_mode,
							  fx32 *pos_z_default )
{
	VecFx32	pos;
  int polyID;
	//�e�̃A���t�@�l�v�Z
  const u8 shadow_alpha = (mcss->shadow_alpha == MCSS_SHADOW_ALPHA_AUTO ?(mcss->alpha/2):mcss->shadow_alpha); 

  if( mcss->alpha == 31 )
  { 
    polyID = 0;
  }
  else
  { 
    polyID = ( node > 63 ) ? 63 : node;
  }

	if( mcss_ortho_mode ){
		G3_OrthoW( FX32_CONST( 96 ),
				   -FX32_CONST( 96 ),
				   -FX32_CONST( 128 ),
				   FX32_CONST( 128 ),
				   FX32_ONE * 1,
				   FX32_ONE * 1024,
				   FX32_ONE,
				   NULL );
		G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
	}

	G3_RestoreMtx( MCSS_NORMAL_MTX );

	G3_TexPlttBase(mcss->mcss_palette_proxy.vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_3DMAIN],
				   mcss->mcss_palette_proxy.fmt);
	G3_PolygonAttr( GX_LIGHTMASK_NONE,				// no lights
                  GX_POLYGONMODE_MODULATE,	// modulation mode
                  GX_CULL_NONE,		    			// cull back
                  polyID,        						// polygon ID(0 - 63)
                  mcss->alpha,	       			// alpha(0 - 31)
                  GX_POLYGON_ATTR_MISC_NONE	// OR of GXPolygonAttrMisc's value
                );

	//�}���`�Z���f�[�^����擾�����ʒu�ŏ����o��
	pos.x = MCSS_CONST( mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc->index].pHierDataArray[node].posX ) + MCSS_CONST( anim_SRT_c->px );
	pos.y = MCSS_CONST( -mcss->mcss_mcanim.pMultiCellDataBank->pMultiCellDataArray[anim_SRT_mc->index].pHierDataArray[node].posY ) + MCSS_CONST( -anim_SRT_c->py );

	G3_Translate( pos.x, pos.y, *pos_z_default );

	G3_RotZ( -FX_SinIdx( anim_SRT_c->rotZ ), FX_CosIdx( anim_SRT_c->rotZ ) );

	G3_Scale( anim_SRT_c->sx, anim_SRT_c->sy, FX32_ONE );

	G3_Translate( pos_x, pos_y, 0 );

	G3_Scale( scale_x, scale_y, FX32_ONE );

	G3_Begin(GX_BEGIN_QUADS);
	G3_TexCoord( tex_s,				tex_t );
	G3_Vtx( 0, 0, 0 );
	G3_TexCoord( tex_s + scale_x,	tex_t );
	G3_Vtx( MCSS_DEFAULT_LINE, 0, 0 );
	G3_TexCoord( tex_s + scale_x,	tex_t + scale_y );
	G3_Vtx( MCSS_DEFAULT_LINE, -MCSS_DEFAULT_LINE, 0 );
	G3_TexCoord( tex_s,				tex_t + scale_y );
	G3_Vtx( 0, -MCSS_DEFAULT_LINE, 0 );
	G3_End();

  //�e�`��
 	G3_MtxMode( GX_MTXMODE_PROJECTION );
 	G3_RestoreMtx( 0 );
 	G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
 	G3_RestoreMtx( MCSS_SHADOW_MTX );

  if( ( mcss->shadow_vanish_flag == 0 ) && ( shadow_alpha ) )
  { 
   	G3_TexPlttBase( shadow_palette->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_3DMAIN], shadow_palette->fmt);
  	G3_PolygonAttr( GX_LIGHTMASK_NONE,				// no lights
        				    GX_POLYGONMODE_MODULATE,	// modulation mode
        				    GX_CULL_NONE,					    // cull back
        				    1,								        // polygon ID(0 - 63)
        				    shadow_alpha,             // alpha(0 - 31)
        				    GX_POLYGON_ATTR_MISC_FOG	// OR of GXPolygonAttrMisc's value
        				   );

  	G3_Translate( pos.x, pos.y, 0 );

  	G3_RotZ( -FX_SinIdx( anim_SRT_c->rotZ ), FX_CosIdx( anim_SRT_c->rotZ ) );

  	G3_Scale( anim_SRT_c->sx, anim_SRT_c->sy, FX32_ONE );

  	G3_Translate( pos_x, pos_y, 0 );

  	G3_Scale( scale_x, scale_y, FX32_ONE );

  	G3_Begin(GX_BEGIN_QUADS);
  	G3_TexCoord( tex_s,				tex_t );
  	G3_Vtx( 0, 0, 0 );
  	G3_TexCoord( tex_s + scale_x,	tex_t );
  	G3_Vtx( MCSS_DEFAULT_LINE, 0, 0 );
  	G3_TexCoord( tex_s + scale_x,	tex_t + scale_y );
  	G3_Vtx( MCSS_DEFAULT_LINE, -MCSS_DEFAULT_LINE, 0 );
  	G3_TexCoord( tex_s,				tex_t + scale_y );
  	G3_Vtx( 0, -MCSS_DEFAULT_LINE, 0 );
  	G3_End();
  }

	if( mcss_ortho_mode == 0 ){
		*pos_z_default -= MCSS_DEFAULT_Z;
	}
	else{
		*pos_z_default -= MCSS_DEFAULT_Z_ORTHO;
	}
}

//--------------------------------------------------------------------------
/**
 * @brief �}���`�Z���o�^
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]  pos_x    �`��X���W
 * @param[in]  pos_y    �`��Y���W
 * @param[in]  pos_Z    �`��Z���W
 * @param[in]  maw      �}���`�Z���o�^�p�p�����[�^�\���̂̃|�C���^
 *
 * @retval  MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
MCSS_WORK*	MCSS_Add( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, const MCSS_ADD_WORK *maw )
{
	int			count;

	for( count = 0 ; count < mcss_sys->mcss_max ; count++ ){
		if( mcss_sys->mcss[ count ] == NULL ){
			mcss_sys->mcss[ count ] = GFL_HEAP_AllocClearMemory( mcss_sys->heapID, sizeof(MCSS_WORK) );
			mcss_sys->mcss[ count ]->index = count;
			mcss_sys->mcss[ count ]->heapID = mcss_sys->heapID;
			mcss_sys->mcss[ count ]->pos.x = pos_x;
			mcss_sys->mcss[ count ]->pos.y = pos_y;
			mcss_sys->mcss[ count ]->pos.z = pos_z;
			mcss_sys->mcss[ count ]->scale.x = FX32_ONE;
			mcss_sys->mcss[ count ]->scale.y = FX32_ONE;
			mcss_sys->mcss[ count ]->scale.z = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_pos.x = 0;
			mcss_sys->mcss[ count ]->ofs_pos.y = 0;
			mcss_sys->mcss[ count ]->ofs_pos.z = 0;
			mcss_sys->mcss[ count ]->ofs_scale.x = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.y = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.z = FX32_ONE;
			mcss_sys->mcss[ count ]->alpha = 31;
			mcss_sys->mcss[ count ]->shadow_alpha = MCSS_SHADOW_ALPHA_AUTO;
			mcss_sys->mcss[ count ]->vanish_flag = MCSS_VANISH_OFF;
			mcss_sys->mcss[ count ]->shadow_rotate = MCSS_DEFAULT_SHADOW_ROTATE;
			mcss_sys->mcss[ count ]->shadow_offset.x = 0;
			mcss_sys->mcss[ count ]->shadow_offset.y = 0;
			mcss_sys->mcss[ count ]->shadow_offset.z = MCSS_DEFAULT_SHADOW_OFFSET;
			mcss_sys->mcss[ count ]->maw = *maw;
			MCSS_LoadResource( mcss_sys, count, maw );
			break;
		}
	}
	//�o�^MAX�l���I�[�o�[
	GF_ASSERT( count < mcss_sys->mcss_max );

	return mcss_sys->mcss[ count ];
}

//--------------------------------------------------------------------------
/**
 * @brief �}���`�Z���폜
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]  mcss     MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_Del( MCSS_SYS_WORK *mcss_sys, MCSS_WORK *mcss )
{
  MCSS_FreeResource( mcss );

	mcss_sys->mcss[ mcss->index ] = NULL;
	GFL_HEAP_FreeMemory( mcss );
}

//--------------------------------------------------------------------------
/**
 * @brief ���ˉe�`�惂�[�h���Z�b�g
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetOrthoMode( MCSS_SYS_WORK *mcss_sys )
{
	mcss_sys->mcss_ortho_mode = 1;
}

//--------------------------------------------------------------------------
/**
 * @brief ���ˉe�`�惂�[�h�����Z�b�g
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_ResetOrthoMode( MCSS_SYS_WORK *mcss_sys )
{
	mcss_sys->mcss_ortho_mode = 0;
}

//--------------------------------------------------------------------------
/**
 * @brief �|�W�V�����Q�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �擾�����|�W�V�������i�[���郏�[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_GetPosition( MCSS_WORK *mcss, VecFx32 *pos )
{
	pos->x = mcss->pos.x;
	pos->y = mcss->pos.y;
	pos->z = mcss->pos.z;
}

//--------------------------------------------------------------------------
/**
 * @brief �|�W�V�����Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �Z�b�g����|�W�V�������i�[���ꂽ���[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetPosition( MCSS_WORK *mcss, VecFx32 *pos )
{
	mcss->pos.x = pos->x;
	mcss->pos.y = pos->y;
	mcss->pos.z = pos->z;
}

//--------------------------------------------------------------------------
/**
 * @brief �I�t�Z�b�g�|�W�V�����Q�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �擾�����|�W�V�������i�[���郏�[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_GetOfsPosition( MCSS_WORK *mcss, VecFx32 *pos )
{
	pos->x = mcss->ofs_pos.x;
	pos->y = mcss->ofs_pos.y;
	pos->z = mcss->ofs_pos.z;
}

//--------------------------------------------------------------------------
/**
 * @brief �I�t�Z�b�g�|�W�V�����Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �Z�b�g����|�W�V�������i�[���ꂽ���[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetOfsPosition( MCSS_WORK *mcss, VecFx32 *pos )
{
	mcss->ofs_pos.x = pos->x;
	mcss->ofs_pos.y = pos->y;
	mcss->ofs_pos.z = pos->z;
}

//--------------------------------------------------------------------------
/**
 * @brief �X�P�[���Q�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �擾�����X�P�[���l���i�[���郏�[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_GetScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	scale->x = mcss->scale.x;
	scale->y = mcss->scale.y;
	scale->z = mcss->scale.z;
}

//--------------------------------------------------------------------------
/**
 * @brief �X�P�[���Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �Z�b�g����X�P�[���l���i�[���ꂽ���[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	mcss->scale.x = scale->x;
	mcss->scale.y = scale->y;
	mcss->scale.z = scale->z;
}

//--------------------------------------------------------------------------
/**
 * @brief �I�t�Z�b�g�X�P�[���Q�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �擾�����X�P�[���l���i�[���郏�[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_GetOfsScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	scale->x = mcss->ofs_scale.x;
	scale->y = mcss->ofs_scale.y;
	scale->z = mcss->ofs_scale.z;
}

//--------------------------------------------------------------------------
/**
 * @brief �I�t�Z�b�g�X�P�[���Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �Z�b�g����X�P�[���l���i�[���ꂽ���[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetOfsScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	mcss->ofs_scale.x = scale->x;
	mcss->ofs_scale.y = scale->y;
	mcss->ofs_scale.z = scale->z;
}

//--------------------------------------------------------------------------
/**
 * @brief ���[�e�[�g�Q�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �擾�������[�e�[�g�l���i�[���郏�[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_GetRotate( MCSS_WORK *mcss, VecFx32 *rotate )
{
	rotate->x = mcss->rotate.x;
	rotate->y = mcss->rotate.y;
	rotate->z = mcss->rotate.z;
}

//--------------------------------------------------------------------------
/**
 * @brief ���[�e�[�g�Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �Z�b�g���郍�[�e�[�g�l���i�[���ꂽ���[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetRotate( MCSS_WORK *mcss, VecFx32 *rotate )
{
	mcss->rotate.x = rotate->x;
	mcss->rotate.y = rotate->y;
	mcss->rotate.z = rotate->z;
}

//--------------------------------------------------------------------------
/**
 * @brief �e�`��p�X�P�[���Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �Z�b�g����X�P�[���l���i�[���ꂽ���[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetShadowScale( MCSS_WORK *mcss, VecFx32 *scale )
{
	mcss->shadow_scale.x = scale->x;
	mcss->shadow_scale.y = scale->y;
	mcss->shadow_scale.z = scale->z;
}
//--------------------------------------------------------------------------
/**
 * @brief �e�`��p��]�p�Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  rot  ��]�p
 */
//--------------------------------------------------------------------------
void	MCSS_SetShadowRotate( MCSS_WORK *mcss, const u16 rot )
{
  mcss->shadow_rotate = rot;
}

//--------------------------------------------------------------------------
/**
 * @brief �e�I�t�Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 * @param[in]  pos  �Z�b�g����X�P�[���l���i�[���ꂽ���[�N�̃|�C���^
 */
//--------------------------------------------------------------------------
void  MCSS_SetShadowOffset( MCSS_WORK *mcss, VecFx32 *ofs )
{
	mcss->shadow_offset.x = ofs->x;
	mcss->shadow_offset.y = ofs->y;
	mcss->shadow_offset.z = ofs->z;
}
void  MCSS_GetShadowOffset( MCSS_WORK *mcss, VecFx32 *ofs )
{
	ofs->x = mcss->shadow_offset.x;
	ofs->y = mcss->shadow_offset.y;
	ofs->z = mcss->shadow_offset.z;
}

//--------------------------------------------------------------------------
/**
 * @brief ���p�`�t���O�Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetMepachiFlag( MCSS_WORK *mcss )
{
	mcss->mepachi_flag = MCSS_MEPACHI_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief ���p�`�t���O���Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_ResetMepachiFlag( MCSS_WORK *mcss )
{
	mcss->mepachi_flag = MCSS_MEPACHI_OFF;
}

//--------------------------------------------------------------------------
/**
 * @brief ���p�`�t���O�t���b�v
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_FlipMepachiFlag( MCSS_WORK *mcss )
{
	mcss->mepachi_flag ^= MCSS_MEPACHI_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief �A�j���X�g�b�v�t���O�Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetAnmStopFlag( MCSS_WORK *mcss )
{
	mcss->anm_stop_flag = MCSS_ANM_STOP_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief �A�j���X�g�b�v�t���O���Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_ResetAnmStopFlag( MCSS_WORK *mcss )
{
	mcss->anm_stop_flag = MCSS_ANM_STOP_OFF;
}

//--------------------------------------------------------------------------
/**
 * @brief �o�j�b�V���t���O�Q�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
int		MCSS_GetVanishFlag( MCSS_WORK *mcss )
{
	return mcss->vanish_flag;
}

//--------------------------------------------------------------------------
/**
 * @brief �o�j�b�V���t���O�Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetVanishFlag( MCSS_WORK *mcss )
{
	mcss->vanish_flag = MCSS_VANISH_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief �o�j�b�V���t���O���Z�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_ResetVanishFlag( MCSS_WORK *mcss )
{
	mcss->vanish_flag = MCSS_VANISH_OFF;
}

//--------------------------------------------------------------------------
/**
 * @brief �o�j�b�V���t���O�t���b�v
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_FlipVanishFlag( MCSS_WORK *mcss )
{
	mcss->vanish_flag ^= MCSS_VANISH_ON;
}

//--------------------------------------------------------------------------
/**
 * @brief �}���`�Z���A�j���R���g���[���Q�b�g
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
NNSG2dMultiCellAnimation* MCSS_GetAnimCtrl( MCSS_WORK *mcss )
{
	return &mcss->mcss_mcanim;
}

//--------------------------------------------------------------------------
/**
 * @brief �}���`�Z���A�j���̃V�[�P���X�����擾
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
u16  MCSS_GetMCellAnmNum( MCSS_WORK *mcss )
{ 
  return mcss->mcss_nmar->numSequences;
}

//--------------------------------------------------------------------------
/**
 * @brief �Î~�A�j�����ł��p�^�[���A�j������Z���̐����擾
 *
 * @param[in]  mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
u8  MCSS_GetStopCellAnms( MCSS_WORK *mcss )
{
	return mcss->mcss_ncen->stop_cellanms;
}

//--------------------------------------------------------------------------
/**
 * @brief �Î~�A�j�����ł��p�^�[���A�j������Z���̃m�[�h���擾
 *
 * @param[in]  mcss   MCSS���[�N�\���̂̃|�C���^
 * @param[in]  index  �m�[�h���擾����C���f�b�N�X
 */
//--------------------------------------------------------------------------
u8  MCSS_GetStopNode( MCSS_WORK *mcss, u8 index )
{
  GF_ASSERT( index < mcss->mcss_ncen->stop_cellanms );
  
	return mcss->mcss_ncen->stop_node[ index ];
}

//--------------------------------------------------------------------------
/**
 * @brief �}���`�Z���̂����悻��X�T�C�Y���擾
 *
 * @param[in]  mcss   MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
u16  MCSS_GetSizeX( MCSS_WORK *mcss )
{ 
  return mcss->mcss_ncec->size_x;
}

//--------------------------------------------------------------------------
/**
 * @brief �}���`�Z���̂����悻��Y�T�C�Y���擾
 *
 * @param[in]  mcss   MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
u16  MCSS_GetSizeY( MCSS_WORK *mcss )
{ 
  return mcss->mcss_ncec->size_y;
}

//--------------------------------------------------------------------------
/**
 * @brief �w�肵���t���[���ɂȂ�����R�[���o�b�N�֐����ĂԂ悤��AnmCtrl�ɓo�^
 *
 * @param[in]  mcss     MCSS���[�N�\���̂̃|�C���^
 * @param[in]  param  ���[�U�����R�Ɏg�p�ł���R�[���o�b�N����
 * @param[in]  pFunc    �Ă΂��R�[���o�b�N�֐��̃|�C���^
 * @param[in]  frameIdx �R�[���o�b�N���Ăԃt���[��
 */
//--------------------------------------------------------------------------
void	MCSS_SetAnimCtrlCallBack( MCSS_WORK *mcss, u32 param, NNSG2dAnmCallBackPtr pFunc, u16 frameIdx )
{	

#if 1
	NNS_G2dSetAnimCtrlCallBackFunctor( NNS_G2dGetMCAnimAnimCtrl(&mcss->mcss_mcanim),
																		 NNS_G2D_ANMCALLBACKTYPE_LAST_FRM, param, pFunc );
#else
  NNS_G2dSetCallBackFunctorAtAnimFrame( NNS_G2dGetMCAnimAnimCtrl(&mcss->mcss_mcanim), param, pFunc, frameIdx );
#endif
}

//--------------------------------------------------------------------------
/**
 * @brief �m�[�h���񂵂ăR�[���o�b�N�֐����ĂԂ悤�ɓo�^
 *
 * @param[in]  mcss   MCSS���[�N�\���̂̃|�C���^
 * @param[in]  param  ���[�U�����R�Ɏg�p�ł���R�[���o�b�N����
 * @param[in]  pFunc  �Ă΂��R�[���o�b�N�֐��̃|�C���^
 */
//--------------------------------------------------------------------------
void	MCSS_SetTraverseMCNodesCallBack( MCSS_WORK *mcss, u32 param, NNSG2dMCTraverseNodeCallBack pFunc )
{	
	NNS_G2dTraverseMCNodes( &mcss->mcss_mcanim.multiCellInstance, pFunc, param );
}

//--------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�Z�b�g
 *
 * @param[in] mcss      MCSS���[�N�\���̂̃|�C���^
 * @param[in]	start_evy	�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 * @param[in]	end_evy		�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���I������16�i�K�j
 * @param[in]	wait			�Z�b�g����p�����[�^�i�E�F�C�g�j
 * @param[in]	rgb				�Z�b�g����p�����[�^�i�t�F�[�h������F�j
 */
//--------------------------------------------------------------------------
void	MCSS_SetPaletteFade( MCSS_WORK *mcss, u8 start_evy, u8 end_evy, u8 wait, u32 rgb )
{	
	GF_ASSERT( mcss );

	mcss->pal_fade_flag				= 1;
	mcss->pal_fade_start_evy	= start_evy;
	mcss->pal_fade_end_evy		= end_evy;
	mcss->pal_fade_wait				= 0;
	mcss->pal_fade_wait_tmp		= wait;
	mcss->pal_fade_rgb				= rgb;
}

//--------------------------------------------------------------------------
/**
 * @brief ���l���Q�b�g
 *
 * @param[in] mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
u8	MCSS_GetAlpha( MCSS_WORK *mcss )
{ 
  return mcss->alpha;
}

//--------------------------------------------------------------------------
/**
 * @brief ���l���Z�b�g
 *
 * @param[in]	mcss		�Z�b�g����}���`�Z�����[�N�\����
 * @param[in]	alpha		�Z�b�g���郿�l(0-31)
 */
//--------------------------------------------------------------------------
void	MCSS_SetAlpha( MCSS_WORK *mcss, u8 alpha )
{ 
  mcss->alpha = alpha;
}

//--------------------------------------------------------------------------
/**
 * @brief �e�̃��l���Q�b�g
 *
 * @param[in] mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
u8    MCSS_GetShadowAlpha( MCSS_WORK *mcss )
{
  return mcss->shadow_alpha;
}

//--------------------------------------------------------------------------
/**
 * @brief �e�̃��l���Z�b�g
 *
 * @param[in]	mcss		�Z�b�g����}���`�Z�����[�N�\����
 * @param[in]	alpha		�Z�b�g���郿�l(0-31)
                      MCSS_SHADOW_ALPHA_AUTO �e�̃A���t�@�l��{�̂̔����ɂ��܂�  
 */
//--------------------------------------------------------------------------
void  MCSS_SetShadowAlpha( MCSS_WORK *mcss, u8 shadow_alpha )
{
  mcss->shadow_alpha = shadow_alpha;
}

//--------------------------------------------------------------------------
/**
 * @brief �e�̃o�j�b�V���t���O���Q�b�g
 *
 * @param[in] mcss MCSS���[�N�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
u8    MCSS_GetShadowVanishFlag( MCSS_WORK *mcss )
{
  return mcss->shadow_vanish_flag;
}

//--------------------------------------------------------------------------
/**
 * @brief �e�̃o�j�b�V���t���O���Z�b�g
 *
 * @param[in]	mcss		�Z�b�g����}���`�Z�����[�N�\����
 * @param[in]	flag		�Z�b�g����l
 */
//--------------------------------------------------------------------------
void  MCSS_SetShadowVanishFlag( MCSS_WORK *mcss, u8 flag )
{
  mcss->shadow_vanish_flag = flag;
}

//--------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�����`�F�b�N
 *
 * @param[in]	mcss		�`�F�b�N����}���`�Z�����[�N�\����
 *
 * @retval  FALSE:�t�F�[�h�I���@TRUE:�t�F�[�h��
 */
//--------------------------------------------------------------------------
BOOL  MCSS_CheckExecutePaletteFade( MCSS_WORK*  mcss )
{ 
  return ( mcss->pal_fade_flag != 0 );
}

//--------------------------------------------------------------------------
/**
 * @brief �e�N�X�`���]���J�n�A�h���X�Z�b�g
 *
 * @param[in]	mcss_sys  MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]	adrs      �Z�b�g����A�h���X
 */
//--------------------------------------------------------------------------
void  MCSS_SetTextureTransAdrs( MCSS_SYS_WORK* mcss_sys, u32 adrs )
{ 
	mcss_sys->texAdrs = adrs;
}

//--------------------------------------------------------------------------
/**
 * @brief �e�N�X�`���p���b�g�]���J�n�A�h���X�Z�b�g
 *
 * @param[in]	mcss_sys  MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]	adrs      �Z�b�g����A�h���X
 */
//--------------------------------------------------------------------------
void  MCSS_SetTexPaletteTransAdrs( MCSS_SYS_WORK* mcss_sys, u32 adrs )
{ 
	mcss_sys->palAdrs = adrs;
}

//--------------------------------------------------------------------------
/**
 * @brief �}���`�Z���A�j���[�V�����Z�b�g
 *
 * @param[in] mcss  MCSS���[�N�\���̂̃|�C���^
 * @param[in] index �Z�b�g����A�j���[�V�����C���f�b�N�X
 */
//--------------------------------------------------------------------------
void	MCSS_SetAnimeIndex( MCSS_WORK* mcss, int index )
{
	const NNSG2dMultiCellAnimSequence* pSequence;

	// �Đ�����V�[�P���X���擾
	pSequence = NNS_G2dGetAnimSequenceByIdx( mcss->mcss_nmar, index );
	GF_ASSERT( pSequence != NULL );

	// �}���`�Z���A�j���[�V�������\�z
	if( mcss->mcss_mcanim_buf )
  { 
	  GFL_HEAP_FreeMemory( mcss->mcss_mcanim_buf );
  }
	MCSS_GetNewMultiCellAnimation( mcss, NNS_G2D_MCTYPE_SHARE_CELLANIM );

	// �}���`�Z���A�j���[�V�����ɍĐ�����V�[�P���X���Z�b�g
	NNS_G2dSetAnimSequenceToMCAnimation( &mcss->mcss_mcanim, pSequence );
}

//--------------------------------------------------------------------------
/**
 * @brief ���U�C�N
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]  mcss     �ēǂݍ��݂���mcss
 * @param[in]  mosaic   ���U�C�N����
 */
//--------------------------------------------------------------------------
u8	MCSS_GetMosaic( MCSS_WORK* mcss )
{ 
  return mcss->mosaic;
}

//--------------------------------------------------------------------------
/**
 * @brief ���U�C�N
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]  mcss     �ēǂݍ��݂���mcss
 * @param[in]  mosaic   ���U�C�N����
 */
//--------------------------------------------------------------------------
void	MCSS_SetMosaic( MCSS_SYS_WORK *mcss_sys, MCSS_WORK* mcss, int mosaic )
{ 
  if( mcss->mosaic == mosaic )
  { 
    return;
  }
  if( mosaic > 15 )
  { 
    mosaic = 15;
  }
  mcss->mosaic = mosaic;
  MCSS_FreeResource( mcss );
  MCSS_LoadResource( mcss_sys, mcss->index, &mcss->maw );
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * @brief ���\�[�X���[�h
 *
 * @param[in]  mcss_sys MCSS�V�X�e���Ǘ��\���̂̃|�C���^
 * @param[in]  count    �o�^�ꏊ�w��
 * @param[in]  maw      �}���`�Z���o�^�p�p�����[�^�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
static	void	MCSS_LoadResource( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_WORK *maw )
{
	MCSS_WORK	*mcss = mcss_sys->mcss[ count ];

	mcss->is_load_resource = 0;

	//�v���L�V������
	NNS_G2dInitImageProxy( &mcss->mcss_image_proxy );
	NNS_G2dInitImagePaletteProxy( &mcss->mcss_palette_proxy );

	// �Z���f�[�^�A�Z���A�j���[�V�����A�}���`�Z���f�[�^�A
	// �}���`�Z���A�j���[�V���������[�h�B
	mcss->mcss_ncer_buf = GFL_ARC_UTIL_LoadCellBank(		maw->arcID, maw->ncer, FALSE, &mcss->mcss_ncer, mcss->heapID );
	GF_ASSERT( mcss->mcss_ncer_buf != NULL );
	mcss->mcss_nanr_buf = GFL_ARC_UTIL_LoadAnimeBank(		maw->arcID, maw->nanr, FALSE, &mcss->mcss_nanr, mcss->heapID );
	GF_ASSERT( mcss->mcss_nanr_buf != NULL );
	mcss->mcss_nmcr_buf = GFL_ARC_UTIL_LoadMultiCellBank(	maw->arcID, maw->nmcr, FALSE, &mcss->mcss_nmcr, mcss->heapID );
	GF_ASSERT( mcss->mcss_nmcr_buf != NULL );
	mcss->mcss_nmar_buf = GFL_ARC_UTIL_LoadMultiAnimeBank(	maw->arcID, maw->nmar, FALSE, &mcss->mcss_nmar, mcss->heapID );
	GF_ASSERT( mcss->mcss_nmar_buf != NULL );

	//
	// �}���`�Z���A�j���[�V�����̎��̂����������܂�
	//
  MCSS_SetAnimeIndex( mcss, 0 );

	//1���̔|���ŕ\�����邽�߂̏��̓ǂݍ��݁i�Ǝ��t�H�[�}�b�g�j
	mcss->mcss_ncec = GFL_ARC_LoadDataAlloc( maw->arcID, maw->ncec, mcss->heapID );
  { 
	  //�Î~�A�j���[�V�������Ƀp�^�[���A�j������m�[�h�f�[�^�i�Ǝ��t�H�[�}�b�g�j
    u32 size = 8 + sizeof( MCSS_NCEC ) * mcss->mcss_ncec->cells;
    mcss->mcss_ncen = ( MCSS_NCEN_WORK *)(mcss->mcss_ncec);
    mcss->mcss_ncen += ( size / sizeof( MCSS_NCEN_WORK ) );
  }

	//
	// VRAM �֘A�̏�����
	//
	{
		TCB_LOADRESOURCE_WORK *tlw = GFL_HEAP_AllocClearMemory( mcss->heapID, sizeof( TCB_LOADRESOURCE_WORK ) );
		tlw->image_p = &mcss->mcss_image_proxy;
		tlw->palette_p = &mcss->mcss_palette_proxy;
		tlw->chr_ofs = mcss_sys->texAdrs + MCSS_TEX_SIZE * count;
		tlw->pal_ofs = mcss_sys->palAdrs + MCSS_PAL_SIZE * count;
		tlw->mcss	 = mcss;
		// load character data for 3D (software sprite)
		{
			tlw->pBufChar = GFL_ARC_UTIL_LoadBGCharacter( maw->arcID, maw->ncbr, FALSE, &tlw->pCharData,
                                                    GFL_HEAP_LOWID( mcss->heapID ) );
			GF_ASSERT( tlw->pBufChar != NULL);
		}

		// load palette data
		{
			tlw->pBufPltt = GFL_ARC_UTIL_LoadPalette( maw->arcID, maw->nclr, &tlw->pPlttData, GFL_HEAP_LOWID( mcss->heapID ) );
			GF_ASSERT( tlw->pBufPltt != NULL);
			mcss->pltt_data = GFL_HEAP_AllocMemory( mcss->heapID, tlw->pPlttData->szByte );
			mcss->pltt_data_size = tlw->pPlttData->szByte;
			MI_CpuCopy16( tlw->pPlttData->pRawData, mcss->pltt_data, tlw->pPlttData->szByte );
		}

    if( mcss_sys->load_resource_callback )
    { 
      if( mcss_sys->load_resource_callback( maw, tlw, mcss_sys->callback_work ) == TRUE )
      { 
        mcss_sys->load_resource_callback = NULL;
        mcss_sys->callback_work = NULL;

      }
    }
    if( mcss->mosaic )
    { 
      MCSS_CalcMosaic( mcss, tlw );
    }
		GFUser_VIntr_CreateTCB( TCB_LoadResource, tlw, 0 );
	}
}

//--------------------------------------------------------------------------
/**
 * @brief ���\�[�X��VRAM�ɓ]��
 */
//--------------------------------------------------------------------------
static	void	TCB_LoadResource( GFL_TCB *tcb, void *work )
{
	TCB_LOADRESOURCE_WORK *tlw = ( TCB_LOADRESOURCE_WORK *)work;
  u16 *v_count = (u16 *)REG_VCOUNT_ADDR;

  //VCount���m�F���Ă������h��
  if( *v_count > MCSS_VCOUNT_BORDER )
  { 
    return;
  }

	if( tlw->mcss ){
    tlw->mcss->is_load_resource = 1;
	}

	if( tlw->pBufChar ){
		// Loading For 3D Graphics Engine.�i�{���́AVRAM�}�l�[�W�����g�p�������j
		NNS_G2dLoadImage2DMapping(
			tlw->pCharData,
			tlw->chr_ofs,
			NNS_G2D_VRAM_TYPE_3DMAIN,
			tlw->image_p );
	
		GFL_HEAP_FreeMemory( tlw->pBufChar );
	}

	if( tlw->pBufPltt ){
		// Loading For 3D Graphics Engine.
		NNS_G2dLoadPalette(
			tlw->pPlttData,
			tlw->pal_ofs,
			NNS_G2D_VRAM_TYPE_3DMAIN,
			tlw->palette_p );

		GFL_HEAP_FreeMemory( tlw->pBufPltt );
	}

	GFL_HEAP_FreeMemory( work );
	GFL_TCB_DeleteTask( tcb );
}

//--------------------------------------------------------------------------
/**
 * @brief �p���b�g��VRAM�ɓ]��
 */
//--------------------------------------------------------------------------
static	void	TCB_LoadPalette( GFL_TCB *tcb, void *work )
{	
	TCB_LOADRESOURCE_WORK *tlw = ( TCB_LOADRESOURCE_WORK *)work;

	NNS_G2dInitImagePaletteProxy( tlw->palette_p );

	// Loading For 3D Graphics Engine.
	NNS_G2dLoadPalette(
		tlw->pPlttData,
		tlw->pal_ofs,
		NNS_G2D_VRAM_TYPE_3DMAIN,
		tlw->palette_p );

	GFL_HEAP_FreeMemory( tlw->pPlttData->pRawData );
	GFL_HEAP_FreeMemory( tlw->pPlttData );
	GFL_HEAP_FreeMemory( work );
	GFL_TCB_DeleteTask( tcb );
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
	}
}

//--------------------------------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�A�j���v�Z
 *
 * @param[in]	mcss	�}���`�Z�����[�N�\����
 */
//--------------------------------------------------------------------------
static	void	MCSS_CalcPaletteFade( MCSS_WORK *mcss )
{	
	if( ( mcss->pal_fade_wait == 0 ) && ( mcss->is_load_resource ) )
	{	
		TCB_LOADRESOURCE_WORK*	tlw = GFL_HEAP_AllocClearMemory( mcss->heapID, sizeof( TCB_LOADRESOURCE_WORK ) );

		tlw->palette_p = &mcss->mcss_palette_proxy;
		tlw->pal_ofs = mcss->mcss_palette_proxy.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_3DMAIN ];

		tlw->pPlttData = GFL_HEAP_AllocMemory( mcss->heapID, sizeof( NNSG2dPaletteData ) );

		tlw->pPlttData->fmt = mcss->mcss_palette_proxy.fmt;
		tlw->pPlttData->bExtendedPlt = FALSE;
		tlw->pPlttData->szByte = mcss->pltt_data_size;
		tlw->pPlttData->pRawData = GFL_HEAP_AllocMemory( mcss->heapID, mcss->pltt_data_size );

		SoftFade( (void *)mcss->pltt_data,
							(void *)tlw->pPlttData->pRawData,
							mcss->pltt_data_size / 2, 
							mcss->pal_fade_start_evy,
							mcss->pal_fade_rgb );

		GFUser_VIntr_CreateTCB( TCB_LoadPalette, tlw, 0 );

		if( mcss->pal_fade_start_evy == mcss->pal_fade_end_evy )
		{	
			mcss->pal_fade_flag = 0;
		}
		else if( mcss->pal_fade_start_evy > mcss->pal_fade_end_evy )
		{	
			mcss->pal_fade_start_evy--;
		}
		else
		{	
			mcss->pal_fade_start_evy++;
		}
		mcss->pal_fade_wait = mcss->pal_fade_wait_tmp;
	}
	else
	{	
		mcss->pal_fade_wait--;
	}
}

//--------------------------------------------------------------------------
/**
 * @brief �m�ۂ��Ă������\�[�X���t���[
 *
 * @param[in]	mcss	�}���`�Z�����[�N�\����
 */
//--------------------------------------------------------------------------
static  void  MCSS_FreeResource( MCSS_WORK* mcss )
{ 
	GF_ASSERT( mcss->mcss_ncer_buf   != NULL );
	GF_ASSERT( mcss->mcss_nanr_buf   != NULL );
	GF_ASSERT( mcss->mcss_nmcr_buf   != NULL );
	GF_ASSERT( mcss->mcss_nmar_buf   != NULL );
	GF_ASSERT( mcss->mcss_mcanim_buf != NULL );
	GF_ASSERT( mcss->mcss_ncec       != NULL );
	GF_ASSERT( mcss->pltt_data       != NULL );

	GFL_HEAP_FreeMemory( mcss->mcss_ncer_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_nanr_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_nmcr_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_nmar_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_mcanim_buf );
	GFL_HEAP_FreeMemory( mcss->mcss_ncec );
	GFL_HEAP_FreeMemory( mcss->pltt_data );

	mcss->mcss_ncer_buf   = NULL;
	mcss->mcss_nanr_buf   = NULL;
	mcss->mcss_nmcr_buf   = NULL;
	mcss->mcss_nmar_buf   = NULL;
	mcss->mcss_mcanim_buf = NULL;
	mcss->mcss_ncec       = NULL;
	mcss->pltt_data       = NULL;
}

//--------------------------------------------------------------------------
/**
 * @brief ���U�C�N����
 *
 * @param[in]	mcss	�}���`�Z�����[�N�\����
 * @param[in]	tlw	  ���\�[�X�f�[�^�\����
 */
//--------------------------------------------------------------------------
static  void  MCSS_CalcMosaic( MCSS_WORK* mcss, TCB_LOADRESOURCE_WORK* tlw )
{ 
  //����16�F�p���b�g�����Ή����܂���
  if( mcss->mcss_image_proxy.attr.fmt != GX_TEXFMT_PLTT16 )
  { 
    return;
  }
  { 
    int tex_x = pow( 2, ( mcss->mcss_image_proxy.attr.sizeS + 2 ) );
    int cells;
    u8* buf = tlw->pCharData->pRawData;

    for( cells = 0 ; cells < mcss->mcss_ncec->cells ; cells++ )
    { 
      int pos_x = mcss->mcss_ncec->ncec[ cells ].tex_s >> FX32_SHIFT;		//�e�N�X�`��s�l
      int pos_y = mcss->mcss_ncec->ncec[ cells ].tex_t >> FX32_SHIFT;		//�e�N�X�`��t�l
      int size_x = mcss->mcss_ncec->ncec[ cells ].size_x >> FX32_SHIFT;		//�Z���T�C�YX
      int size_y = mcss->mcss_ncec->ncec[ cells ].size_y >> FX32_SHIFT;		//�Z���T�C�YY
      int mosaic = mcss->mosaic + 1;
      int col;
      int pos;
      int ofs_x;
      int ofs_y;
      int cnt_y = 0;

      for( ofs_y = 0 ; ofs_y < size_y ; ofs_y += mosaic )
      { 
        int cnt_x = 0;
        for( ofs_x = 0 ; ofs_x < size_x ; ofs_x += mosaic )
        { 
          int put_size_x = ( ( ofs_x + mosaic ) > size_x ) ? ( size_x - mosaic * cnt_x ) : mosaic; 
          int put_size_y = ( ( ofs_y + mosaic ) > size_y ) ? ( size_y - mosaic * cnt_y ) : mosaic; 

          col = mosaic_col_get( buf, pos_x + ofs_x, pos_y + ofs_y, tex_x, put_size_x, put_size_y );
          mosaic_make( buf, pos_x + ofs_x, pos_y + ofs_y, tex_x, put_size_x, put_size_y, col );

          cnt_x++;
        }
        cnt_y++;
      }

      if( mcss->mcss_ncec->ncec[ cells ].mepachi_size_x )
      { 
        int pos_mx = mcss->mcss_ncec->ncec[ cells ].mepachi_tex_s >> FX32_SHIFT;		//�e�N�X�`��s�l
        int pos_my = mcss->mcss_ncec->ncec[ cells ].mepachi_tex_t >> FX32_SHIFT;		//�e�N�X�`��t�l
        int size_mx = mcss->mcss_ncec->ncec[ cells ].mepachi_size_x >> FX32_SHIFT;		//�Z���T�C�YX
        int size_my = ( mcss->mcss_ncec->ncec[ cells ].mepachi_size_y >> FX32_SHIFT ) * 2;		//�Z���T�C�YY
        int cnt_my = 0;

        for( ofs_y = 0 ; ofs_y < size_my ; ofs_y += mosaic )
        { 
          int cnt_mx = 0;
          for( ofs_x = 0 ; ofs_x < size_mx ; ofs_x += mosaic )
          { 
            int put_size_mx = ( ( ofs_x + mosaic ) > size_mx ) ? ( size_mx - mosaic * cnt_mx ) : mosaic; 
            int put_size_my = ( ( ofs_y + mosaic ) > size_my ) ? ( size_my - mosaic * cnt_my ) : mosaic; 

            col = mosaic_col_get( buf, pos_mx + ofs_x, pos_my + ofs_y, tex_x, put_size_mx, put_size_my );
            mosaic_make( buf, pos_mx + ofs_x, pos_my + ofs_y, tex_x, put_size_mx, put_size_my, col );

            cnt_mx++;
          }
          cnt_my++;
        }
      }
    }
  }
}

static  inline  void  mosaic_make( u8 *buf, int pos_x, int pos_y, int tex_x, int size_x, int size_y, int col )
{ 
  int x, y;
  int col_l = col;
  int col_h = col << 4;

  for( y = 0 ; y < size_y ; y++ )
  { 
    for( x = 0 ; x < size_x ; x++ )
    { 
      int put_x = pos_x + x;
      int put_y = pos_y + y;
      int pos = mosaic_pos_get( put_x, put_y, tex_x );

      if( put_x & 1 )
      { 
        buf[ pos ] = ( buf[ pos ] & 0x0f ) | col_h;
      }
      else
      { 
        buf[ pos ] = ( buf[ pos ] & 0xf0 ) | col_l;
      }
    }
  }
}

static  inline  int  mosaic_pos_get( int pos_x, int pos_y, int tex_x )
{ 
  return pos_x / 2 + pos_y * tex_x;
}


static  inline  u8  mosaic_col_get( u8 *buf, int pos_x, int pos_y, int tex_x, int size_x, int size_y )
{ 
#if 1
#if 0
  int i;
  int get_x;
  int get_y;
  int pos;
  int col[ 16 ];
  int ret_col = 0;
  int col_cnt = 0;

  for( i = 0 ; i < 16 ; i++ )
  { 
    col[ i ] = 0;
  }

  for( get_y = 0 ; get_y < size_y ; get_y++ )
  { 
    for( get_x = 0 ; get_x < size_x ; get_x++ )
    { 
      pos = mosaic_pos_get( get_x + pos_x, get_y + pos_y, tex_x );
      i = ( get_x & 1 ) ? ( buf[ pos ] & 0xf0 ) >> 4 : buf[ pos ] & 0x0f;
      col[ i ]++;
    }
  }
  for( i = 1 ; i < 16 ; i++ )
  { 
    if( col_cnt < col[ i ] )
    { 
      col_cnt = col[ i ];
      ret_col = i;
    }
  }
  return ret_col;
#endif
  int i;
  int get_x;
  int get_y;
  int pos;
  int ret_col = 0;

  for( get_y = 0 ; get_y < size_y ; get_y++ )
  { 
    for( get_x = 0 ; get_x < size_x ; get_x++ )
    { 
      pos = mosaic_pos_get( get_x + pos_x, get_y + pos_y, tex_x );
      i = ( get_x & 1 ) ? ( buf[ pos ] & 0xf0 ) >> 4 : buf[ pos ] & 0x0f;
      if( i )
      { 
        ret_col = i;
        break;
      }
    }
    if( ret_col )
    { 
      break;
    }
  }
  return ret_col;
#else
  int pos = mosaic_pos_get( pos_x, pos_y, tex_x );
  return ( pos_x & 1 ) ? ( buf[ pos ] & 0xf0 ) >> 4 : buf[ pos ] & 0x0f;
#endif
}

//�_��峂��炢������
//----------------------------------------------------------------------------
/**
 *	@brief	4x4�s��ɍ��W���|�����킹��
 *			Vec��x,y,z,1�Ƃ��Čv�Z���A�v�Z���Vec��w��Ԃ��܂��B
 *
 *	@param	*cp_src	Vector���W
 *	@param	*cp_m   4*4�s��
 *	@param	*p_dst	Vecror�v�Z����
 *	@param	*p_w    4�ڂ̗v�f�̒l
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static	void MTX_MultVec44( const VecFx32 *cp_src, const MtxFx44 *cp_m, VecFx32 *p_dst, fx32 *p_w )
{
	fx32 x = cp_src->x;
  fx32 y = cp_src->y;
  fx32 z = cp_src->z;
	fx32 w = FX32_ONE;

	GF_ASSERT( cp_src );
	GF_ASSERT( cp_m );
	GF_ASSERT( p_dst );
	GF_ASSERT( p_w );

  p_dst->x = (fx32)(((fx64)x * cp_m->_00 + (fx64)y * cp_m->_10 + (fx64)z * cp_m->_20) >> FX32_SHIFT);
  p_dst->x += cp_m->_30;	//	W=1�Ȃ̂ő�������

  p_dst->y = (fx32)(((fx64)x * cp_m->_01 + (fx64)y * cp_m->_11 + (fx64)z * cp_m->_21) >> FX32_SHIFT);
  p_dst->y += cp_m->_31;//	W=1�Ȃ̂ő�������

  p_dst->z = (fx32)(((fx64)x * cp_m->_02 + (fx64)y * cp_m->_12 + (fx64)z * cp_m->_22) >> FX32_SHIFT);
  p_dst->z += cp_m->_32;//	W=1�Ȃ̂ő�������

	*p_w	= (fx32)(((fx64)x * cp_m->_03 + (fx64)y * cp_m->_13 + (fx64)z * cp_m->_23) >> FX32_SHIFT);
  *p_w	+= cp_m->_33;//	W=1�Ȃ̂ő�������
}

#ifdef USE_RENDER
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
#endif //USE_RENDER

#ifdef PM_DEBUG
//--------------------------------------------------------------------------
/**
 * @brief �}���`�Z���o�^�i�f�o�b�O�p�j
 */
//--------------------------------------------------------------------------
MCSS_WORK*	MCSS_AddDebug( MCSS_SYS_WORK *mcss_sys, fx32	pos_x, fx32	pos_y, fx32	pos_z, const MCSS_ADD_DEBUG_WORK *madw )
{
	int			count;

	for( count = 0 ; count < mcss_sys->mcss_max ; count++ ){
		if( mcss_sys->mcss[ count ] == NULL ){
			mcss_sys->mcss[ count ] = GFL_HEAP_AllocClearMemory( mcss_sys->heapID, sizeof(MCSS_WORK) );
			mcss_sys->mcss[ count ]->index = count;
			mcss_sys->mcss[ count ]->heapID = mcss_sys->heapID;
			mcss_sys->mcss[ count ]->pos.x = pos_x;
			mcss_sys->mcss[ count ]->pos.y = pos_y;
			mcss_sys->mcss[ count ]->pos.z = pos_z;
			mcss_sys->mcss[ count ]->scale.x = FX32_ONE;
			mcss_sys->mcss[ count ]->scale.y = FX32_ONE;
			mcss_sys->mcss[ count ]->scale.z = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.x = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.y = FX32_ONE;
			mcss_sys->mcss[ count ]->ofs_scale.z = FX32_ONE;
			mcss_sys->mcss[ count ]->alpha = 31;
			mcss_sys->mcss[ count ]->shadow_alpha = MCSS_SHADOW_ALPHA_AUTO;
			mcss_sys->mcss[ count ]->vanish_flag = MCSS_VANISH_OFF;
			mcss_sys->mcss[ count ]->shadow_rotate = MCSS_DEFAULT_SHADOW_ROTATE;
			mcss_sys->mcss[ count ]->shadow_offset.x = 0;
			mcss_sys->mcss[ count ]->shadow_offset.y = 0;
			mcss_sys->mcss[ count ]->shadow_offset.z = MCSS_DEFAULT_SHADOW_OFFSET;
			MCSS_LoadResourceDebug( mcss_sys, count, madw );
			break;
		}
	}
	//�o�^MAX�l���I�[�o�[
	GF_ASSERT( count < mcss_sys->mcss_max );

	return mcss_sys->mcss[ count ];
}

//--------------------------------------------------------------------------
/**
 * @brief ���\�[�X���[�h�i�f�o�b�O�p�j
 */
//--------------------------------------------------------------------------
static	void	MCSS_LoadResourceDebug( MCSS_SYS_WORK *mcss_sys, int count, const MCSS_ADD_DEBUG_WORK *madw )
{
	MCSS_WORK	*mcss = mcss_sys->mcss[ count ];

	mcss->is_load_resource = 0;

	//�v���L�V������
	NNS_G2dInitImageProxy( &mcss->mcss_image_proxy );
	NNS_G2dInitImagePaletteProxy( &mcss->mcss_palette_proxy );

	// �Z���f�[�^�A�Z���A�j���[�V�����A�}���`�Z���f�[�^�A
	// �}���`�Z���A�j���[�V���������[�h�B
	NNS_G2dGetUnpackedCellBank( madw->ncer, &mcss->mcss_ncer );
	mcss->mcss_ncer_buf = madw->ncer;
	GF_ASSERT( mcss->mcss_ncer_buf != NULL );

	NNS_G2dGetUnpackedAnimBank( madw->nanr, &mcss->mcss_nanr );
	mcss->mcss_nanr_buf = madw->nanr;
	GF_ASSERT( mcss->mcss_nanr_buf != NULL );

	NNS_G2dGetUnpackedMultiCellBank( madw->nmcr, &mcss->mcss_nmcr );
	mcss->mcss_nmcr_buf =  madw->nmcr;
	GF_ASSERT( mcss->mcss_nmcr_buf != NULL );

	NNS_G2dGetUnpackedMCAnimBank( madw->nmar, &mcss->mcss_nmar );
	mcss->mcss_nmar_buf = madw->nmar;
	GF_ASSERT( mcss->mcss_nmar_buf != NULL );

	//
	// �}���`�Z���A�j���[�V�����̎��̂����������܂�
	//
  MCSS_SetAnimeIndex( mcss, 0 );

	//1���̔|���ŕ\�����邽�߂̏��̓ǂݍ��݁i�Ǝ��t�H�[�}�b�g�j
	mcss->mcss_ncec = madw->ncec;
  { 
	  //�Î~�A�j���[�V�������Ƀp�^�[���A�j������m�[�h�f�[�^�i�Ǝ��t�H�[�}�b�g�j
    u32 size = 8 + sizeof( MCSS_NCEC ) * mcss->mcss_ncec->cells;
    mcss->mcss_ncen = ( MCSS_NCEN_WORK *)(mcss->mcss_ncec);
    mcss->mcss_ncen += ( size / sizeof( MCSS_NCEN_WORK ) );
  }

  //
  // VRAM �֘A�̏�����
  //
  {
		TCB_LOADRESOURCE_WORK *tlw = GFL_HEAP_AllocClearMemory( mcss->heapID, sizeof( TCB_LOADRESOURCE_WORK ) );
		tlw->image_p = &mcss->mcss_image_proxy;
		tlw->palette_p = &mcss->mcss_palette_proxy;
		tlw->chr_ofs = mcss_sys->texAdrs + MCSS_TEX_SIZE * count;
		tlw->pal_ofs = mcss_sys->palAdrs + MCSS_PAL_SIZE * count;
		tlw->mcss	 = mcss;
		// load character data for 3D (software sprite)
		{
			NNS_G2dGetUnpackedBGCharacterData( madw->ncbr, &tlw->pCharData );
			tlw->pBufChar = madw->ncbr;
			GF_ASSERT( tlw->pBufChar != NULL);
    }

		// load palette data
		{
			NNS_G2dGetUnpackedPaletteData( madw->nclr, &tlw->pPlttData );
			tlw->pBufPltt = madw->nclr;
			GF_ASSERT( tlw->pBufPltt != NULL);
			mcss->pltt_data = GFL_HEAP_AllocMemory( mcss->heapID, tlw->pPlttData->szByte );
			mcss->pltt_data_size = tlw->pPlttData->szByte;
			MI_CpuCopy16( tlw->pPlttData->pRawData, mcss->pltt_data, tlw->pPlttData->szByte );
    }

    GFUser_VIntr_CreateTCB( TCB_LoadResource, tlw, 0 );

  }
}

#endif
