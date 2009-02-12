//======================================================================
/**
 * @file	dressup_system.h
 * @brief	�h���X�A�b�v �������C��
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"

#include "arc_def.h"
#include "musical_item.naix"

#include "test/ariizumi/ari_debug.h"
#include "musical/mus_poke_draw.h"
#include "dup_fitting.h"

//======================================================================
//	define
//======================================================================

#define FIT_FRAME_MAIN_3D	GFL_BG_FRAME0_M
#define FIT_FRAME_MAIN_CASE	GFL_BG_FRAME2_M
#define FIT_FRAME_MAIN_BG	GFL_BG_FRAME3_M
#define FIT_FRAME_SUB_BG	GFL_BG_FRAME3_S

//BBD�p���W�ϊ�(�J�����̕�����v�Z
#define BBD_POS_X(val)	FX32_CONST((val)/32.0f)
#define BBD_POS_Y(val)	FX32_CONST(480.0f-(val)/32.0f)

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
struct _FITTING_WORK
{
	HEAPID heapId;
	FITTING_INIT_WORK *initWork;
	
	//3D�`��Ɋւ�镨
	MUS_POKE_DRAW_SYSTEM	*drawSys;
	MUS_POKE_DRAW_WORK		*drawWork;
	GFL_G3D_CAMERA			*camera;
	GFL_BBD_SYS				*bbdSys;
};

//======================================================================
//	proto
//======================================================================
static void DUP_FIT_SetupGraphic( FITTING_WORK *work );
static void DUP_FIT_SetupPokemon( FITTING_WORK *work );


static const GFL_DISP_VRAM vramBank = {
	GX_VRAM_BG_128_D,				// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_NONE,				// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_NONE,			// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_01_AB,				// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g
	GX_OBJVRAMMODE_CHAR_1D_32K,
	GX_OBJVRAMMODE_CHAR_1D_32K
};
//	A �e�N�X�`��
//	B �e�N�X�`��
//	C SubBg
//	D MainBg
//	E �e�N�X�`���p���b�g
//	F �e�N�X�`���p���b�g
//	G None(OBJ?)
//	H None
//	I None


//--------------------------------------------------------------
//	�������C�� ������
//--------------------------------------------------------------
FITTING_WORK*	DUP_FIT_InitFitting( FITTING_INIT_WORK *initWork )
{
	FITTING_WORK *work = GFL_HEAP_AllocMemory( initWork->heapId , sizeof( FITTING_WORK ));

	work->heapId = initWork->heapId;
	work->initWork = initWork;
	DUP_FIT_SetupGraphic( work );
	DUP_FIT_SetupPokemon( work );
	
	
	GX_SetMasterBrightness(0);	
	GXS_SetMasterBrightness(0);
	return work;
}

//--------------------------------------------------------------
//	�������C�� �J��
//--------------------------------------------------------------
void	DUP_FIT_TermFitting( FITTING_WORK *work )
{
	GFL_G3D_CAMERA_Delete( work->camera );
	GFL_G3D_Exit();
	GFL_BG_Exit();
	GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
//	�������C�� ���[�v
//--------------------------------------------------------------
FITTING_RETURN	DUP_FIT_LoopFitting( FITTING_WORK *work )
{
	VecFx32 scale;
	MUS_POKE_DRAW_UpdateSystem( work->drawSys ); 

	GFL_BBD_GetScale( work->bbdSys , &scale );
	GFL_BBD_SetScale( work->bbdSys , &scale );

	//3D�`��	
	GFL_G3D_DRAW_Start();
	GFL_G3D_DRAW_SetLookAt();
	{
		MUS_POKE_DRAW_DrawSystem( work->drawSys ); 
		GFL_BBD_Draw( work->bbdSys , work->camera , NULL );
	}
	GFL_G3D_DRAW_End();

	return FIT_RET_CONTINUE;
}

//--------------------------------------------------------------
//	�`��n������
//--------------------------------------------------------------
static void DUP_FIT_SetupGraphic( FITTING_WORK *work )
{
	GFL_DISP_GX_InitVisibleControl();
	GFL_DISP_GXS_InitVisibleControl();
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );
	WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
	WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
	WIPE_ResetWndMask(WIPE_DISP_MAIN);
	WIPE_ResetWndMask(WIPE_DISP_SUB);
	
	GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
	GFL_DISP_SetBank( &vramBank );
	
	GFL_BG_Init( work->heapId );
	
	//Vram���蓖�Ă̐ݒ�
	{
		static const GFL_BG_SYS_HEADER sys_data = {
				GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		
		// BG2 MAIN (�W
		static const GFL_BG_BGCNT_HEADER header_main2 = {
			0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x6000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};
		// BG3 MAIN (�w�i
		static const GFL_BG_BGCNT_HEADER header_main3 = {
			0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x6000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};

		// BG3 SUB (�w�i
		static const GFL_BG_BGCNT_HEADER header_sub3 = {
			0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x6000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};

		GFL_BG_SetBGMode( &sys_data );
		GFL_BG_SetBGControl3D( 1 );
		GFL_BG_SetBGControl( FIT_FRAME_MAIN_CASE,  &header_main2, GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( FIT_FRAME_MAIN_BG,  &header_main3, GFL_BG_MODE_TEXT );

		GFL_BG_SetBGControl( FIT_FRAME_SUB_BG, &header_sub3, GFL_BG_MODE_TEXT );
		
		GFL_BG_SetVisible( FIT_FRAME_MAIN_3D , TRUE );
	}
	
	{	//3D�n�̐ݒ�
		static const VecFx32 cam_pos = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 7.8f ), FX_F32_TO_FX32( 21.0f ) };
		static const VecFx32 cam_target = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 2.6f ), FX_F32_TO_FX32( 0.0f ) };
		static const VecFx32 cam_up = { 0, FX32_ONE, 0 };
		//�G�b�W�}�[�L���O�J���[
		static	const	GXRgb stage_edge_color_table[8]=
			{ GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };
		GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT80K,
						0, work->heapId, NULL );
		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
#if 0	//�����ˉe�J����
		work->camera = 	GFL_G3D_CAMERA_Create( GFL_G3D_PRJPERS, 
											 FX32_SIN13,
											 FX32_COS13,
											 FX_F32_TO_FX32( 1.33f ),
											 NULL,
											 FX32_ONE,
											 FX32_ONE * 180,
											 NULL,
											 &cam_pos,
											 &cam_up,
											 &cam_target,
											 work->heapId );
#else
		//���ˉe�J����
		work->camera = 	GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH, 
											 FX32_ONE*15.0f,
											 0,
											 0,
											 FX32_ONE*20.0f,
											 FX32_ONE,
											 FX32_ONE * 180,
											 NULL,
											 &cam_pos,
											 &cam_up,
											 &cam_target,
											 work->heapId );
#endif
		
		GFL_G3D_CAMERA_Switching( work->camera );
		//�G�b�W�}�[�L���O�J���[�Z�b�g
		G3X_SetEdgeColorTable( &stage_edge_color_table[0] );
		G3X_EdgeMarking( TRUE );
	}
	{
		GFL_BBD_SETUP bbdSetup = {
			128,128,
			{FX32_ONE,FX32_ONE,0},
			GX_RGB(0,0,0),
			GX_RGB(0,0,0),
			GX_RGB(0,0,0),
			GX_RGB(0,0,0),
			0
		};
		//�r���{�[�h�V�X�e���\�z
		work->bbdSys = GFL_BBD_CreateSys( &bbdSetup , work->heapId );
	}
}

//--------------------------------------------------------------
//	�\���|�P�����̏�����
//--------------------------------------------------------------
static void DUP_FIT_SetupPokemon( FITTING_WORK *work )
{
	VecFx32 pos = {FX32_ONE*10,FX32_ONE*10,FX32_ONE*-7};	//�ʒu�͓K��
//	VecFx32 pos = {0,0,FX32_ONE*-7};	//�ʒu�͓K��
	work->drawSys = MUS_POKE_DRAW_InitSystem( work->heapId );
	work->drawWork = MUS_POKE_DRAW_Add( work->drawSys , work->initWork->musPoke );

	MUS_POKE_DRAW_SetPosition( work->drawWork , &pos);

	{
		/*
		VecFx32 bbdPos[2] = 	//�ʒu�͓K��
		{
			{FX32_ONE*10,FX32_ONE*10,FX32_ONE*-7},
			{FX32_ONE*12,FX32_ONE*5,FX32_ONE},
//			{0,0,FX32_ONE*-7},
//			{0,FX32_ONE,0}
		};
		u16 resIdx,bbdIdx;
		BOOL flg = TRUE;
		
		resIdx = GFL_BBD_AddResourceArc( work->bbdSys , ARCID_MUSICAL_ITEM , NARC_musical_item_item01_nsbtx ,
								GFL_BBD_TEXFMT_PAL4 , GFL_BBD_TEXSIZ_32x32 ,
								32 , 32 );
		GFL_BBD_AddResourceArc( work->bbdSys , ARCID_MUSICAL_ITEM , NARC_musical_item_item02_nsbtx ,
								GFL_BBD_TEXFMT_PAL4 , GFL_BBD_TEXSIZ_32x32 ,
								32 , 32 );
		bbdIdx = GFL_BBD_AddObject( work->bbdSys , resIdx , FX32_ONE,FX32_ONE , &bbdPos[0] , 31 ,GFL_BBD_LIGHT_NONE);
		GFL_BBD_SetObjectDrawEnable( work->bbdSys , bbdIdx , &flg );
		bbdIdx = GFL_BBD_AddObject( work->bbdSys , resIdx+1 , FX32_ONE,FX32_ONE , &bbdPos[1] , 31 ,GFL_BBD_LIGHT_NONE);
		GFL_BBD_SetObjectDrawEnable( work->bbdSys , bbdIdx , &flg );
		*/
	}
}
