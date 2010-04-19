//============================================================================================
/**
 * @file		cdemo_main.c
 * @brief		�R�}���h�f����� ���C������
 * @author	Hiroyuki Nakamura
 * @date		09.01.21
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"

#include "cdemo_main.h"
#include "cdemo_comm.cdat"
#include "op_demo.naix"



//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void VBlankTask( GFL_TCB * tcb, void * work );
static void LoadPaletteMain( CDEMO_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// VRAM����U��
static const GFL_DISP_VRAM VramTbl = {
	GX_VRAM_BG_256_BC,						// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_0123_E,			// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_32_H,					// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_128_A,						// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_16_I,					// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_NONE,							// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE					// �e�N�X�`���p���b�g�X���b�g
};




void CDEMOMAIN_VramBankSet(void)
{
	GFL_DISP_SetBank( &VramTbl );
}

void CDEMOMAIN_BgInit( CDEMO_WORK * wk )
{
/*
	GFL_BG_Init( HEAPID_COMMAND_DEMO );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// ���C����ʁF
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, 0x10000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x40, 0, HEAPID_COMMAND_DEMO );
	}
	{	// ���C����ʁF
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x20000, 0x10000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME1_M );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME1_M, 0x40, 0, HEAPID_COMMAND_DEMO );
	}
	{	// ���C����ʁF
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x30000, 0x10000,
			GX_BG_EXTPLTT_23, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_M );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME2_M, 0x40, 0, HEAPID_COMMAND_DEMO );
	}

	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0 );
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0 );

	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_ON );
*/
	GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BG0_AS_2D );

	G2_SetBG2ControlDCBmp( GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000 );
	G2_SetBG3ControlDCBmp( GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x20000 );

	{	// ��]�g�k��������
		MtxFx22 mtx;
	  GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE, FX32_ONE, GFL_CALC2D_AFFINE_MAX_NORMAL );
		G2_SetBG2Affine( &mtx, 0, 0, 0, 0 );
		G2_SetBG3Affine( &mtx, 0, 0, 0, 0 );
	}

	{	// �X�N���[���N���A
		void * buf = GFL_HEAP_AllocClearMemory( HEAPID_COMMAND_DEMO_L, 256*192*2 );
    GX_LoadBG2Bmp( buf, 0, 256*192*2 );
    GX_LoadBG3Bmp( buf, 0, 256*192*2 );
		GFL_HEAP_FreeMemory( buf );
	}

/*
	{	// �T�u��ʃo�b�N�O���E���h�J���[�N���A
		u16	col = 0;
		DC_FlushRange( &col, 2 );
		GXS_LoadBGPltt( &col, 0, 2 );
	}
*/

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_OBJ, VISIBLE_OFF );
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );

/*
//#define SCREEN_SIZE (256 * 256 * 2)
//    GX_LoadBG3Bmp(bitmapBG_directcolor_Texel, 0, SCREEN_SIZE);
	{
		void * buf;
//		buf = GFL_ARC_UTIL_Load( ARCID_CDEMO_DATA, NARC_ld_arceus_ds_2_00000_ntft, FALSE, HEAPID_COMMAND_DEMO );
		buf = GFL_ARC_UTIL_Load( ARCID_CDEMO_DATA, NARC_ld_arceus_ds_2_00000_LZ_bin, TRUE, HEAPID_COMMAND_DEMO );
//		DC_FlushRange( buf, 256*192*2 );
    GX_LoadBG2Bmp( buf, 0, 256*192*2 );
		GFL_HEAP_FreeMemory( buf );
//		buf = GFL_ARC_UTIL_Load( ARCID_CDEMO_DATA, NARC_ld_arceus_ds_2_00206_ntft, FALSE, HEAPID_COMMAND_DEMO );
		buf = GFL_ARC_UTIL_Load( ARCID_CDEMO_DATA, NARC_ld_arceus_ds_2_00206_LZ_bin, TRUE, HEAPID_COMMAND_DEMO );
//		DC_FlushRange( buf, 256*192*2 );
    GX_LoadBG3Bmp( buf, 0, 256*192*2 );
		GFL_HEAP_FreeMemory( buf );
	}
*/
}

void CDEMOMAIN_BgExit(void)
{
/*
	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

	GFL_BG_Exit();
*/
}

void CDEMOMAIN_CommDataLoad( CDEMO_WORK * wk )
{
	wk->commSrc = (int *)CommandData;
	wk->commPos = wk->commSrc;
}

void CDEMOMAIN_CommDataDelete( CDEMO_WORK * wk )
{
}

void CDEMOMAIN_InitVBlank( CDEMO_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

void CDEMOMAIN_ExitVBlank( CDEMO_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

static void VBlankTask( GFL_TCB * tcb, void * work )
{
	CDEMO_WORK * wk = work;

/*
	LoadPaletteMain( wk );

	GFL_BG_VBlankFunc();
*/

//	GFL_CLACT_SYS_VBlankFunc();

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}


//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�C���Z�b�g
 *
 * @param	wk		�`���f�����[�N
 * @param	div		������
 * @param	sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param	next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CDEMOMAIN_FadeInSet( CDEMO_WORK * wk, int div, int sync, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, div, sync, HEAPID_COMMAND_DEMO );

	wk->next_seq = next;
}

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�A�E�g�Z�b�g
 *
 * @param	wk		�`���f�����[�N
 * @param	div		������
 * @param	sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param	next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CDEMOMAIN_FadeOutSet( CDEMO_WORK * wk, int div, int sync, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, div, sync, HEAPID_COMMAND_DEMO );

	wk->next_seq = next;
}

//--------------------------------------------------------------------------------------------
/**
 * �z���C�g�C���Z�b�g
 *
 * @param	wk		�`���f�����[�N
 * @param	div		������
 * @param	sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param	next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CDEMOMAIN_WhiteInSet( CDEMO_WORK * wk, int div, int sync, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_WHITE, div, sync, HEAPID_COMMAND_DEMO );

	wk->next_seq = next;
}

//--------------------------------------------------------------------------------------------
/**
 * �z���C�g�A�E�g�Z�b�g
 *
 * @param	wk		�`���f�����[�N
 * @param	div		������
 * @param	sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param	next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CDEMOMAIN_WhiteOutSet( CDEMO_WORK * wk, int div, int sync, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_WHITE, div, sync, HEAPID_COMMAND_DEMO );

	wk->next_seq = next;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�p���b�g�]�����N�G�X�g
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
void CDEMOMAIN_LoadPaletteRequest( CDEMO_WORK * wk, u32 frm, u32 id )
{
	NNSG2dPaletteData * pal_data;
	void * arc_data;
	CDEMO_PALETTE * pltt;
	
	arc_data = GFL_ARCHDL_UTIL_LoadPalette( wk->gra_ah, id, &pal_data, HEAPID_COMMAND_DEMO_L );

	pltt = &wk->pltt[ frm - GFL_BG_FRAME0_M ];
	pltt->size = pal_data->szByte;

	MI_CpuCopy32( pal_data->pRawData, pltt->buff, pltt->size );

	GFL_HEAP_FreeMemory( arc_data );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�p���b�g�]�����C��
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static void LoadPaletteMain( CDEMO_WORK * wk )
{
	u32	i;

	for( i=0; i<3; i++ ){
		if( wk->pltt[i].size == 0 ){ continue; }

		GX_BeginLoadBGExtPltt();
		GX_LoadBGExtPltt( wk->pltt[i].buff, 0x2000 * i, wk->pltt[i].size );
		GX_EndLoadBGExtPltt();

		wk->pltt[i].size = 0;
	}
}


void CDEMOMAIN_LoadBgGraphic( CDEMO_WORK * wk, u32 chr, u32 pal, u32 scrn, u32 frm )
{
	GFL_ARCHDL_UTIL_TransVramBgCharacter( wk->gra_ah, chr, frm, 0, 0, TRUE, HEAPID_COMMAND_DEMO );
	GFL_ARCHDL_UTIL_TransVramScreen( wk->gra_ah, scrn, frm, 0, 0, FALSE, HEAPID_COMMAND_DEMO );
//	GFL_ARCHDL_UTIL_TransVramPalette(
//		wk->gra_ah, NCLR_START_IDX + comm[2], PALTYPE_MAIN_BG_EX, 0, 0, HEAPID_COMMAND_DEMO );
	CDEMOMAIN_LoadPaletteRequest( wk, frm, pal );
}
















#if 0


//============================================================================================
//	�萔��`
//============================================================================================




//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void VramBankInit(void);
static void BgSysInit(void);
static void Comm3DInit( LEGEND_DEMO_WORK * wk );
static void Comm3DRelease( LEGEND_DEMO_WORK * wk );

#ifdef PM_DEBUG
static void DebugInit(void);
static void DebugAnmFlagSet( BOOL flg );
static BOOL DebugAnmFlagCheck(void);
static void DebugCtrlFlagSet( BOOL flg );
static BOOL DebugCtrlFlagCheck(void);
#endif	// PM_DEBUG




//--------------------------------------------------------------------------------------------
/**
 * ���ʏ�����
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_CommInit( LEGEND_DEMO_WORK * wk )
{
	// ���荞�ݒ�~
	sys_VBlankFuncChange( NULL, NULL );
	sys_HBlankIntrStop();
	// �\��������
	GF_Disp_GX_VisibleControlDirect( 0 );
	GF_Disp_GXS_VisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// ���ʂ����C����
	GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

	VramBankInit();
	BgSysInit();
	Comm3DInit( wk );

#ifdef PM_DEBUG
	DebugInit();
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * ���ʉ������
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_CommRelease( LEGEND_DEMO_WORK * wk )
{
	Comm3DRelease( wk );
}


//--------------------------------------------------------------------------------------------
/**
 * �J�����ݒ�
 *
 * @param	wk		�`���f�����[�N
 * @param	dat		�J�����f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_InitCamera( LEGEND_DEMO_WORK * wk, const LD_CAMERA_DATA * dat )
{
	wk->target = dat->target;

	// �J�����Z�b�g
	GFC_InitCameraTDA(
		&wk->target,
		dat->prm.Distance,	// ����
		&dat->prm.Angle,
		dat->prm.PerspWay,	// ���ˉe�̊p�x
		dat->prm.View,
		TRUE,
		wk->camera );

	GFC_SetCameraClip( dat->near, dat->far, wk->camera );
	GFC_AttachCamera( wk->camera );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g������
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_InitLight(void)
{
	u32	i;

	for( i=0; i<4; i++ ){
		NNS_G3dGlbLightVector( i, 0, 0, 0 );
		NNS_G3dGlbLightColor( i, 0x7fff );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�ݒ�
 *
 * @param	gst		���C�g�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_SetLight( GLST_DATA * gst )
{
	u32	i;

	for( i=0; i<4; i++ ){
		NNS_G3dGlbLightVector(
			i, gst->lightVec[i].x, gst->lightVec[i].y, gst->lightVec[i].z );
		NNS_G3dGlbLightColor( i, gst->Color[i] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �G�b�W�}�[�L���O�ݒ�
 *
 * @param	rgb		�J���[�e�[�u��
 *
 * @return	none
 *
 * @li	rgb = NULL �ŃG�b�W�}�[�L���O����
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_EdgeMarkingSet( const GXRgb * rgb )
{
	if( rgb == NULL ){
		G3X_EdgeMarking( FALSE );
	}else{
		G3X_EdgeMarking( TRUE );
		G3X_SetEdgeColorTable( rgb );	// �G�b�W�}�[�L���O�e�[�u���Z�b�g
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���f�[�^�ǂݍ���
 *
 * @param	wk			�`���f�����[�N
 * @param	arcIndex	�A�[�N�C���f�b�N�X
 * @param	tbl			�V�[�����\�[�X�f�[�^�e�[�u��
 * @param	max			�V�[�����\�[�X�f�[�^��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_Load3DData( LEGEND_DEMO_WORK * wk, u32 arcIndex, const LD_RES_TBL * tbl, u32 max )
{
//	ARCHANDLE * ah;
	LD_SCENE_DATA * scene;
	NNSG3dResTex * tex;
	void * anmRes;
	u16	i, j;

//	ah = ArchiveDataHandleOpen( arcIndex, HEAPID_LEGEND_DEMO );

	sys_InitAllocator( &wk->allocator, HEAPID_LEGEND_DEMO, 4 );

	for( i=0; i<max; i++ ){
		scene = &wk->scene[i];

		// ���f�����O�f�[�^�����[�h
//		scene->rh = ArchiveDataLoadAllocByHandle( ah, tbl[i].imd, HEAPID_LEGEND_DEMO );
		scene->rh = ArcUtil_Load( arcIndex, tbl[i].imd, TRUE, HEAPID_LEGEND_DEMO, ALLOC_BOTTOM );

		simple_3DModelSetResFileAlready( &scene->rendObj, &scene->mdl, &scene->rh );
		tex = NNS_G3dGetTex( scene->rh );	// �e�N�X�`�����\�[�X�擾
//		BindTexture( scene->rh, tex );

		if( tbl[i].glstFlag == TRUE ){
			NNS_G3dMdlUseGlbDiff( scene->mdl );		// �f�B�t���[�Y
			NNS_G3dMdlUseGlbAmb( scene->mdl );		// �A���r�G���g
			NNS_G3dMdlUseGlbSpec( scene->mdl );		// �X�y�L�����[
			NNS_G3dMdlUseGlbEmi( scene->mdl );		// �G�~�b�V����
		}

		// �A�j���[�V���������[�h
		for( j=0; j<LD_SCENE_ANM_MAX; j++ ){
			if( tbl[i].anm[j] == LD_RES_DUMMY ){
				scene->anmMem[j] = NULL;
				scene->anmObj[j] = NULL;
				continue;
			}

//			scene->anmMem[j] = ArchiveDataLoadAllocByHandle( ah, tbl[i].anm[j], HEAPID_LEGEND_DEMO );
			scene->anmMem[j] = ArcUtil_Load( arcIndex, tbl[i].anm[j], TRUE, HEAPID_LEGEND_DEMO, ALLOC_BOTTOM );

			anmRes = NNS_G3dGetAnmByIdx( scene->anmMem[j], 0 );
			// �K�v�ʂ̃��������A���P�[�g
			scene->anmObj[j] = NNS_G3dAllocAnmObj(
								&wk->allocator,		// �g�p����A���P�[�^���w��
								anmRes,				// �A�j���[�V�������\�[�X���w��
								scene->mdl );		// ���f�����\�[�X���w��
			// �C�j�V�����C�Y
			NNS_G3dAnmObjInit(
				scene->anmObj[j],	// �A�j���[�V�����I�u�W�F�N�g�ւ̃|�C���^
				anmRes,				// �A�j�����\�[�X�ւ̃|�C���^
				scene->mdl,			// NNSG3dResMdl�ւ̃|�C���^
				tex );				// NNSG3dResTex�ւ̃|�C���^
			// �A�j���o�C���h
			NNS_G3dRenderObjAddAnmObj( &scene->rendObj, scene->anmObj[j] );
		}
	}

//	ArchiveDataHandleClose( ah );
	
	wk->scene_max = max;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���f�[�^�폜
 *
 * @param	wk			�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_Delete3DData( LEGEND_DEMO_WORK * wk )
{
	LD_SCENE_DATA * scene;
	u16	i, j;

	for( i=0; i<wk->scene_max; i++ ){
		scene = &wk->scene[i];
		for( j=0; j<LD_SCENE_ANM_MAX; j++ ){
			if( scene->anmMem[j] != NULL ){
				NNS_G3dFreeAnmObj( &wk->allocator, scene->anmObj[j] );
				sys_FreeMemoryEz( scene->anmMem[j] );
			}
		}
		sys_FreeMemoryEz( scene->rh );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���\��
 *
 * @param	wk			�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_Draw3DMain( LEGEND_DEMO_WORK * wk )
{
	LD_SCENE_DATA * scene;
	u32	i, j;

	// �`��
	MtxFx33 rotate_tmp = {FX32_ONE,0,0,0,FX32_ONE,0,0,0,FX32_ONE};
	VecFx32 scale = {FX32_ONE,FX32_ONE,FX32_ONE};
	VecFx32 trans = {0,0,0};

#ifdef PM_DEBUG
	if( wk->dat->debug == 1 ){
		if( LDMAIN_AllObjAnmCheck( wk ) == FALSE ){
			LDMAIN_DebugObjAnmClear( wk );
		}
	}
#endif	// PM_DEBUG

	GF_G3X_Reset();
	GFC_CameraLookAt();

	for( j=0; j<wk->scene_max; j++ ){
		scene = &wk->scene[j];

#ifdef PM_DEBUG
		if( DebugAnmFlagCheck() == TRUE ){
			for( i=0; i<LD_SCENE_ANM_MAX; i++ ){
				if( scene->anmObj[i] == NULL ){ continue; }
				if( scene->anmObj[i]->frame+FX32_ONE < NNS_G3dAnmObjGetNumFrame(scene->anmObj[i]) ){
					scene->anmObj[i]->frame += FX32_ONE;
				}
			}
		}
#else
		for( i=0; i<LD_SCENE_ANM_MAX; i++ ){
			if( scene->anmObj[i] == NULL ){ continue; }
			if( scene->anmObj[i]->frame+FX32_ONE < NNS_G3dAnmObjGetNumFrame(scene->anmObj[i]) ){
				scene->anmObj[i]->frame += FX32_ONE;
			}
		}
#endif	// PM_DEBUG

		simple_3DModelDraw(
			&scene->rendObj,
			&trans,
			&rotate_tmp,
			&scale );
	}

//	GF_G3_RequestSwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
	GF_G3_RequestSwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���A�j���`�F�b�N�i�ʁj
 *
 * @param	wk		�`���f�����[�N
 * @param	id		���f���h�c
 *
 * @retval	"TRUE = �A�j����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL LDMAIN_ObjAnmCheck( LEGEND_DEMO_WORK * wk, u32 id )
{
	LD_SCENE_DATA * scene;
	u32	i;
	
	scene = &wk->scene[id];

	for( i=0; i<LD_SCENE_ANM_MAX; i++ ){
		if( scene->anmObj[i] == NULL ){ continue; }
		if( scene->anmObj[i]->frame+FX32_ONE < NNS_G3dAnmObjGetNumFrame(scene->anmObj[i]) ){
			return TRUE;
		}
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c���f���A�j���`�F�b�N�i�S�́j
 *
 * @param	wk		�`���f�����[�N
 *
 * @retval	"TRUE = �A�j����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL LDMAIN_AllObjAnmCheck( LEGEND_DEMO_WORK * wk )
{
	u32	i;

	for( i=0; i<wk->scene_max; i++ ){
		if( LDMAIN_ObjAnmCheck( wk, i ) == TRUE ){
			return TRUE;
		}
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * VRAM������
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VramBankInit(void)
{
	GF_Disp_SetBank( &VramTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * BG������
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgSysInit(void)
{
	GF_BGL_SYS_HEADER sysh = {
		GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
	};
	GF_BGL_InitBG( &sysh );

	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_S, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * ���ʂR�c������
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Comm3DInit( LEGEND_DEMO_WORK * wk )
{
	// �J�����쐬
	wk->camera = GFC_AllocCamera( HEAPID_LEGEND_DEMO );

	simple_3DBGInit( HEAPID_LEGEND_DEMO );

//	G3X_AlphaTest( FALSE, 0 );
//	G3X_AlphaBlend( TRUE );
	G3X_AntiAlias( TRUE );

	G3X_SetFog( FALSE, 0, 0, 0 );
	G3X_SetClearColor( GX_RGB(31,0,0), 0, 0x7fff, 63, FALSE );
/*
	NNS_G3dGlbMaterialColorDiffAmb(
		GX_RGB(31,31,31), GX_RGB(31,31,31), FALSE );
	NNS_G3dGlbMaterialColorSpecEmi(
		GX_RGB(31,31,31), GX_RGB(31,31,31), FALSE );

	NNS_G3dGlbPolygonAttr(
		0x0f,
		GX_POLYGONMODE_MODULATE,
		GX_CULL_NONE,
		0,
		31,
		GX_POLYGON_ATTR_MISC_FOG );
*/
	// �O���[�o���X�e�[�g��������
	NNS_G3dGlbMaterialColorDiffAmb(
		wk->dat->glst.Diff,
		wk->dat->glst.Amb,
		wk->dat->glst.IsSetVtxColor );
	
	NNS_G3dGlbMaterialColorSpecEmi(
		wk->dat->glst.Spec,
		wk->dat->glst.Emi,
		wk->dat->glst.IsShininess );
	
	// �|���S���A�g���r���[�g
	NNS_G3dGlbPolygonAttr(
		wk->dat->glst.LightFlag,
		wk->dat->glst.polyMode,
		wk->dat->glst.cullMode,
		wk->dat->glst.polygonID,
		wk->dat->glst.alpha,
		wk->dat->glst.misc );

	GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 0 );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * ���ʂR�c���
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Comm3DRelease( LEGEND_DEMO_WORK * wk )
{
	simple_3DBGExit();
	GFC_FreeCamera( wk->camera );
}


//============================================================================================
//	���[�V�����u���[
//============================================================================================
static void MotinBlTask( TCB_PTR tcb, void * work );
static void VBlankLCDCChange( TCB_PTR tcb, void* work );
static void MotionBlCapture( LDMAIN_MBL_PARAM * p_data );

//----------------------------------------------------------------------------
/**
 * ���[�V�����u���[������
 *
 * @param	prm		�������f�[�^
 *
 *@return	���[�V�����u���[���[�N
 */
//-----------------------------------------------------------------------------
static LDMAIN_MBL_WORK * MotionBlInit( LDMAIN_MBL_PARAM * prm )
{
	LDMAIN_MBL_WORK * mb;
	TCB_PTR task;
	
	// ���[�V�����u���[�^�X�N���Z�b�g
	task = PMDS_taskAdd( MotinBlTask, sizeof(LDMAIN_MBL_WORK), 5, prm->heap_id );
	mb = TCB_GetWork( task );

	mb->data = *prm;
	mb->tcb = task;
	mb->init_flg = FALSE;

	mb->lcdc = GX_GetBankForLCDC();

	MotionBlCapture( &mb->data );

	// LCDC�`�F���W
	VWaitTCB_Add( VBlankLCDCChange, mb, 0 );

	return mb;
}

//----------------------------------------------------------------------------
/**
 * ���[�V�����u���[�폜
 *
 * @param	data		���[�V�����u���[�I�u�W�F
 * @param	dispMode	������̃f�B�X�v���C���[�h
 * @param	bgMode		�������BG���[�h
 * @param	bg0_2d3d	�������BG�O���RD�Ɏg�p���邩
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
static void MotionBlDelete( LDMAIN_MBL_WORK * mb, GXDispMode dispMode, GXBGMode bgMode, GXBG0As bg0_2d3d )
{
	// �O���t�B�b�N���[�h��߂�
	GX_SetGraphicsMode(dispMode, bgMode,bg0_2d3d);

	GX_SetBankForLCDC( mb->lcdc);
	
	switch( mb->data.dispMode ){
	case GX_DISPMODE_VRAM_A:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_A, HW_VRAM_A_SIZE );
		break;
	case GX_DISPMODE_VRAM_B:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_B, HW_VRAM_B_SIZE );
		break;
	case GX_DISPMODE_VRAM_C:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_C, HW_VRAM_C_SIZE );
		break;
	case GX_DISPMODE_VRAM_D:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE );
		break;
	default:
		GX_SetBankForLCDC(GX_VRAM_LCDC_NONE);
		break;
	}

	// �^�X�N�j��
	PMDS_taskDel( mb->tcb );
}

//----------------------------------------------------------------------------
/**
 * ���[�V�����u���[�@�L���v�`���^�X�N
 *
 * @param	tcb		�^�X�N�|�C���^
 * @param	work	���[�V�����u���[�f�[�^
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
static void MotinBlTask( TCB_PTR tcb, void * work )
{
	LDMAIN_MBL_WORK * mb = work;
	
	//�����������҂�
	if( mb->init_flg ){
		GX_SetCapture(
				mb->data.sz,			// �L���v�`���T�C�Y
				mb->data.mode,			// �L���v�`�����[�h
				mb->data.a,				// �L���v�`���u�����hA
				mb->data.b,				// �L���v�`���u�����hB
				mb->data.dest,			// �]��Vram
				mb->data.eva,			// �u�����h�W��A
				mb->data.evb);			// �u�����h�W��B
	}
}

//----------------------------------------------------------------------------
/**
 * LCDC�̏�Ԃ�ݒ肷��^�X�N
 *
 * @param	tcb		�^�X�N�|�C���^
 * @param	work	�ݒ肷��l�������Ă���
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
static void VBlankLCDCChange( TCB_PTR tcb, void* work )
{
	LDMAIN_MBL_WORK * mb = work;

	// �`��Vram�ݒ�
	switch(mb->data.dispMode){
	case GX_DISPMODE_VRAM_A:
		GX_SetBankForLCDC(GX_VRAM_LCDC_A);
		break;
	case GX_DISPMODE_VRAM_B:
		GX_SetBankForLCDC(GX_VRAM_LCDC_B);
		break;
	case GX_DISPMODE_VRAM_C:
		GX_SetBankForLCDC(GX_VRAM_LCDC_C);
		break;
	case GX_DISPMODE_VRAM_D:
		GX_SetBankForLCDC(GX_VRAM_LCDC_D);
		break;
	default:
		GX_SetBankForLCDC(GX_VRAM_LCDC_NONE);
		break;
	}

	// �O���t�B�b�N���[�h�ݒ�
	GX_SetGraphicsMode(mb->data.dispMode, mb->data.bgMode,mb->data.bg0_2d3d);	

	// ����������
	mb->init_flg = TRUE;

	TCB_Delete( tcb );
}

//----------------------------------------------------------------------------
/**
 * Capture�֐�
 *
 * @param	p_data 
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
static void MotionBlCapture( LDMAIN_MBL_PARAM * p_data )
{
	// �`��Vram������
	switch(p_data->dispMode){
	case GX_DISPMODE_VRAM_A:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_A, HW_VRAM_A_SIZE );
		break;
	case GX_DISPMODE_VRAM_B:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_B, HW_VRAM_B_SIZE );
		break;
	case GX_DISPMODE_VRAM_C:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_C, HW_VRAM_C_SIZE );
		break;
	case GX_DISPMODE_VRAM_D:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE );
		break;
	default:
		break;
	}

	GX_SetCapture(
			p_data->sz,			// �L���v�`���T�C�Y
			p_data->mode,			// �L���v�`�����[�h
			p_data->a,				// �L���v�`���u�����hA
			p_data->b,				// �L���v�`���u�����hB
			p_data->dest,			// �]��Vram
			16,						// �u�����h�W��A
			0);						// �u�����h�W��B	
}

//--------------------------------------------------------------------------------------------
/**
 * ���[�V�����u���[������
 *
 * @param	eva		�u�����h�W��A
 * @param	evb		�u�����h�W��B
 *
 * @return	FLD_MOTION_BL_PTR	���[�V�����u���[�|�C���^
 */
//--------------------------------------------------------------------------------------------
LDMAIN_MBL_WORK * LDMAIN_MotionBlInit( int eva, int evb )
{
	LDMAIN_MBL_WORK * wk;

//	MI_CpuClear32(ret, sizeof(FLD_MOTION_BL_DATA));

	// BG�ʂ̕`���p�~
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG1,VISIBLE_OFF);
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG2,VISIBLE_OFF);
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG3,VISIBLE_OFF);
	GX_ResetBankForBG();
	
	// ���[�V�����u���[�ݒ�
	{
		LDMAIN_MBL_PARAM mb = {
			GX_DISPMODE_VRAM_C,
			GX_BGMODE_0,
			GX_BG0_AS_3D,
			
			GX_CAPTURE_SIZE_256x192,
			GX_CAPTURE_MODE_AB,
			GX_CAPTURE_SRCA_2D3D,
			GX_CAPTURE_SRCB_VRAM_0x00000,
			GX_CAPTURE_DEST_VRAM_C_0x00000,
			0,
			0,
			HEAPID_LEGEND_DEMO
		};
		mb.eva = eva;
		mb.evb = evb;

		wk = MotionBlInit( &mb );
	}	

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * ���[�V�����u���[�폜
 *
 * @param	mb		���[�V�����u���[�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void LDMAIN_MotionBlExit( LDMAIN_MBL_WORK * mb )
{
	// ���ɖ߂�
	MotionBlDelete( mb, GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D );

	GX_SetBankForBG(GX_VRAM_BG_128_C);

	// BG�ʕ`��
	GF_Disp_GX_VisibleControl(
			GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3,
			VISIBLE_ON );
}








//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//	�f�o�b�O
//============================================================================================
#ifdef PM_DEBUG

#define DEBUG_ANGLE_MOVE	( 32 )		// �J�����A���O���ړ���
#define DEBUG_ZOOM_PARAM	( 64 )		// �J�����Y�[���ړ���
#define DEBUG_ZOOM_MAX		( 0x4000 )	// �J�����Y�[���ő�l
#define DEBUG_ZOOM_MIN		( 0 )		// �J�����Y�[���ŏ��l

// �A�j���t���[��������
void LDMAIN_DebugObjAnmClear( LEGEND_DEMO_WORK * wk )
{
	LD_SCENE_DATA * scene;
	NNSG3dResTex * tex;
	void * anmRes;
	u16	i, j;

	for( i=0; i<wk->scene_max; i++ ){
		scene = &wk->scene[i];
		for( j=0; j<LD_SCENE_ANM_MAX; j++ ){
			if( scene->anmObj[j] == NULL ){ continue; }
			scene->anmObj[j]->frame = 0;
		}
	}
}

// ���]
static void DebugCameraRotation( LEGEND_DEMO_WORK * wk )
{
	CAMERA_ANGLE	angle = {0,0,0,0};	// x, y, z, dummy

	if( sys.cont & PAD_KEY_UP ){
		angle.x = DEBUG_ANGLE_MOVE;
		GFC_AddCameraAngleRev( &angle, wk->camera );
	}else if( sys.cont & PAD_KEY_DOWN ){
		angle.x = -DEBUG_ANGLE_MOVE;
		GFC_AddCameraAngleRev( &angle, wk->camera );
	}else if( sys.cont & PAD_KEY_LEFT ){
		angle.y = -DEBUG_ANGLE_MOVE;
		GFC_AddCameraAngleRev( &angle, wk->camera );
	}else if( sys.cont & PAD_KEY_RIGHT ){
		angle.y = DEBUG_ANGLE_MOVE;
		GFC_AddCameraAngleRev( &angle, wk->camera );
	}
}
// ���]
static void DebugCameraRevolution( LEGEND_DEMO_WORK * wk )
{
	CAMERA_ANGLE	angle = {0,0,0,0};	// x, y, z, dummy

	if( sys.cont & PAD_KEY_UP ){
		angle.x = DEBUG_ANGLE_MOVE;
		GFC_AddCameraAngleRot( &angle, wk->camera );
	}else if( sys.cont & PAD_KEY_DOWN ){
		angle.x = -DEBUG_ANGLE_MOVE;
		GFC_AddCameraAngleRot( &angle, wk->camera );
	}else if( sys.cont & PAD_KEY_LEFT ){
		angle.y = DEBUG_ANGLE_MOVE;
		GFC_AddCameraAngleRot( &angle, wk->camera );
	}else if( sys.cont & PAD_KEY_RIGHT ){
		angle.y = -DEBUG_ANGLE_MOVE;
		GFC_AddCameraAngleRot( &angle, wk->camera );
	}
}
// �p�[�X
static void DebugCameraZoom( LEGEND_DEMO_WORK * wk )
{
	u16	persp_way;
//	u8 view;
	fx32 dist;
	
//	CAMERA_ANGLE angle = {0,0,0,0};
//	VecFx32 move = {0,0,0};

	// �Y�[���A�E�g
	if( sys.cont & PAD_KEY_UP ){
		persp_way = GFC_GetCameraPerspWay( wk->camera );
		if( (persp_way+DEBUG_ZOOM_PARAM) < DEBUG_ZOOM_MAX ){
			GFC_AddCameraPerspWay( DEBUG_ZOOM_PARAM, wk->camera );
		}
	// �Y�[���C��
	}else if( sys.cont & PAD_KEY_DOWN ){
		persp_way = GFC_GetCameraPerspWay( wk->camera );
		if( (persp_way-DEBUG_ZOOM_PARAM) > DEBUG_ZOOM_MIN ){
			GFC_AddCameraPerspWay( -DEBUG_ZOOM_PARAM, wk->camera );
		}
	}else if( sys.cont & PAD_KEY_LEFT ){
		GFC_AddCameraPerspWay( DEBUG_ZOOM_PARAM, wk->camera );
		persp_way = GFC_GetCameraPerspWay( wk->camera );
		//�J�����̒��S�ɂ���2D�摜���A���ꂸ�ɕ\���ł��鋗�������߂�
		//�Ԃꂸ�ɕ\�����邽�߂ɂ́A1Dot�@���@1Unit�Ōv�Z����΂悢
		//�J��������ʒ��S�����߂�Ƃ��A��ʔ����̃T�C�Y�́A96Dot
		//�\�����������A�J�����̃p�[�X���l���A96Dot�@���@96Unit�ɂȂ鋗���ɂ���΂悢
		dist = FX_Div(
					FX_Mul( FX_CosIdx( persp_way ), FX_F32_TO_FX32(96) ),	
					FX_SinIdx( persp_way ) );
		GFC_SetCameraDistance( dist, wk->camera );
	}else if( sys.cont & PAD_KEY_RIGHT ){
		GFC_AddCameraPerspWay( -DEBUG_ZOOM_PARAM, wk->camera );
		persp_way = GFC_GetCameraPerspWay( wk->camera );
		//�J�����̒��S�ɂ���2D�摜���A���ꂸ�ɕ\���ł��鋗�������߂�
		//�Ԃꂸ�ɕ\�����邽�߂ɂ́A1Dot�@���@1Unit�Ōv�Z����΂悢
		//�J��������ʒ��S�����߂�Ƃ��A��ʔ����̃T�C�Y�́A96Dot
		//�\�����������A�J�����̃p�[�X���l���A96Dot�@���@96Unit�ɂȂ鋗���ɂ���΂悢
		dist = FX_Div(
					FX_Mul( FX_CosIdx( persp_way ), FX_F32_TO_FX32(96) ),	
					FX_SinIdx( persp_way ));					
		GFC_SetCameraDistance( dist, wk->camera );
	}
}
// ���s�ړ� X-Z
static void DebugCameraShiftXZ( LEGEND_DEMO_WORK * wk )
{
	VecFx32	move = {0,0,0};
	fx32	shift;

	if( sys.cont & PAD_BUTTON_R ){
		shift = ( FX32_ONE * 1 ) / 10;
	}else{
		shift = FX32_ONE;
	}

	if( sys.cont & PAD_KEY_UP ){
		move.z = -shift;
		GFC_ShiftCamera( &move, wk->camera );
	}else if( sys.cont & PAD_KEY_DOWN ){
		move.z = shift;
		GFC_ShiftCamera( &move, wk->camera );
	}else if( sys.cont & PAD_KEY_LEFT ){
		move.x = -shift;
		GFC_ShiftCamera( &move, wk->camera );
	}else if( sys.cont & PAD_KEY_RIGHT ){
		move.x = shift;
		GFC_ShiftCamera( &move, wk->camera );
	}
}
// ���s�ړ� Y
static void DebugCameraShiftY( LEGEND_DEMO_WORK * wk )
{
	VecFx32	move = {0,0,0};
	fx32	shift;

	if( sys.cont & PAD_BUTTON_R ){
		shift = ( FX32_ONE * 1 ) / 10;
	}else{
		shift = FX32_ONE;
	}

	if( sys.cont & PAD_KEY_UP ){
		move.y = shift;
		GFC_ShiftCamera( &move, wk->camera );
	}else if( sys.cont & PAD_KEY_DOWN ){
		move.y = -shift;
		GFC_ShiftCamera( &move, wk->camera );
	}
}
// �N���b�v
static void DebugCameraClip( LEGEND_DEMO_WORK * wk )
{
	fx32	near, far;

	if( sys.cont & PAD_KEY_UP ){
		far  = GFC_GetCameraFar( wk->camera );
		near = GFC_GetCameraNear( wk->camera );
		far += FX32_ONE;
//		OS_Printf("far=%d\n",far/FX32_ONE);
		GFC_SetCameraClip( near, far, wk->camera );
	}else if( sys.cont & PAD_KEY_DOWN ){
		far  = GFC_GetCameraFar( wk->camera );
		near = GFC_GetCameraNear( wk->camera );
		far -= FX32_ONE;
//		OS_Printf("far=%d\n",far/FX32_ONE);
		GFC_SetCameraClip( near, far, wk->camera );
	}else if( sys.cont & PAD_KEY_RIGHT ){
		far  = GFC_GetCameraFar( wk->camera );
		near = GFC_GetCameraNear( wk->camera );
		near += FX32_ONE;
//		OS_Printf("near=%d\n",near/FX32_ONE);
		GFC_SetCameraClip( near, far, wk->camera );
	}else if( sys.cont & PAD_KEY_LEFT ){
		far  = GFC_GetCameraFar( wk->camera );
		near = GFC_GetCameraNear( wk->camera );
		near -= FX32_ONE;
//		OS_Printf("near=%d\n",near/FX32_ONE);
		GFC_SetCameraClip( near, far, wk->camera );
	}
}
// ��������������
static void DebugCameraAddDistance( LEGEND_DEMO_WORK * wk )
{
	GFC_AddCameraDistance( FX32_ONE, wk->camera );
}
// �������߂Â���
static void DebugCameraSubDistance( LEGEND_DEMO_WORK * wk )
{
	GFC_AddCameraDistance( -FX32_ONE, wk->camera );
}
// ���\��
static void DebugCameraInfoPut( LEGEND_DEMO_WORK * wk )
{
/*
	CAMERA_ANGLE	angle;
	VecFx32	target;

	angle  = GFC_GetCameraAngle( wk->camera );
	target = GFC_GetLookTarget( wk->camera );

	OS_Printf( "�����@�J�������@������������������������������������\n" );
	OS_Printf( "�@�@�@�@�@���� : 0x%x\n", GFC_GetCameraDistance(wk->camera) );
	OS_Printf( "�@�@�@�A���O�� : 0x%x, 0x%x, 0x%x\n", angle.x, angle.y, angle.z );
	OS_Printf( "�@�@�@�@�p�[�X : 0x%x\n", GFC_GetCameraPerspWay(wk->camera) );
	OS_Printf(
		"�@�j�A�E�t�@�[ : %d, %d\n",
		GFC_GetCameraNear(wk->camera) / FX32_ONE,
		GFC_GetCameraFar(wk->camera) / FX32_ONE );
	OS_Printf( "�@�@�@�@�����_ : 0x%x, 0x%x, 0x%x\n", target.x, target.y, target.z );
//	OS_Printf( "��
	OS_Printf( "������������������������������������������������������\n" );
*/
}

BOOL LDMAIN_DebugMainControl( LEGEND_DEMO_WORK * wk )
{
	// �V�X�e�����[�h
	if( DebugCtrlFlagCheck() == TRUE ){
		if( sys.trg & PAD_BUTTON_SELECT ){
			return FALSE;					// �����I��
		}else if( sys.trg & PAD_BUTTON_B ){
			DebugAnmFlagSet( TRUE );		// �A�j���ĊJ
			DebugCtrlFlagSet( FALSE );		// �J�������[�h��
		}else if( sys.trg & PAD_BUTTON_A ){
			DebugCtrlFlagSet( FALSE );		// �J�������[�h��
		}
		return TRUE;
	}

	// ���]
	if( sys.cont & PAD_BUTTON_B ){
		DebugCameraRotation( wk );
	// ���]
	}else if( sys.cont & PAD_BUTTON_Y ){
		DebugCameraRevolution( wk );
	// �p�[�X
	}else if( sys.cont & PAD_BUTTON_A ){
		DebugCameraZoom( wk );
	// ���s�ړ� X-Z
	}else if( sys.cont & PAD_BUTTON_X ){
		DebugCameraShiftXZ( wk );
	// ���s�ړ� Y
	}else if( sys.cont & PAD_BUTTON_START ){
		DebugCameraShiftY( wk );
	// �N���b�v
	}else if( sys.cont & PAD_BUTTON_R ){
		DebugCameraClip( wk );
	// ��������������
	}else if( sys.cont & PAD_KEY_UP ){
		DebugCameraAddDistance( wk );
	// �������߂Â���
	}else if( sys.cont & PAD_KEY_DOWN ){
		DebugCameraSubDistance( wk );
	// ���\��
	}else if( sys.trg & PAD_BUTTON_L ){
		DebugCameraInfoPut( wk );
	// �I��
	}else if( sys.trg & PAD_BUTTON_SELECT ){
		if( DebugAnmFlagCheck() == FALSE ){
			DebugAnmFlagSet( TRUE );		// �A�j���ĊJ
		}else{
			DebugAnmFlagSet( FALSE );	// �A�j����~
			DebugCtrlFlagSet( TRUE );	// �V�X�e�����[�h��
		}
	}

	return TRUE;
}

static BOOL DebugAnmFlag  = FALSE;
static BOOL DebugCtrlFlag = FALSE;

// �f�o�b�O������
static void DebugInit(void)
{
	DebugAnmFlagSet( TRUE );
	DebugCtrlFlagSet( FALSE );
}

// �A�j���Đ��t���O�Z�b�g
static void DebugAnmFlagSet( BOOL flg )
{
	DebugAnmFlag = flg;
}

// �A�j���Đ��t���O�`�F�b�N
static BOOL DebugAnmFlagCheck(void)
{
	return DebugAnmFlag;
}

// �f�o�b�O�R���g���[�����[�h�Z�b�g
static void DebugCtrlFlagSet( BOOL flg )
{
	DebugCtrlFlag = flg;
}

// �f�o�b�O�R���g���[���t���O�Z�b�g
static BOOL DebugCtrlFlagCheck(void)
{
	return DebugCtrlFlag;
}

#endif	// PM_DEBUG

#endif
