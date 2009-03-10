//======================================================================
/**
 * @file	dressup_system.h
 * @brief	�X�e�[�W ���Z���C��
 * @author	ariizumi
 * @data	09/03/02
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "font/font.naix"

#include "arc_def.h"
#include "musical_item.naix"
#include "stage_gra.naix"
#include "print/printsys.h"

#include "sound/snd_strm.h"
#include "snd_strm.naix"

#include "infowin/infowin.h"
#include "test/ariizumi/ari_debug.h"
#include "musical/mus_poke_draw.h"
#include "musical/mus_item_draw.h"
#include "musical/musical_camera_def.h"

#include "musical/musical_stage_sys.h"
#include "sta_local_def.h"
#include "sta_acting.h"
#include "sta_act_effect.h"
#include "sta_act_poke.h"
#include "sta_act_obj.h"
#include "script/sta_act_script.h"

#include "eff_def/mus_eff.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define ACT_FRAME_MAIN_3D		GFL_BG_FRAME0_M
#define ACT_FRAME_MAIN_FONT		GFL_BG_FRAME1_M
#define ACT_FRAME_MAIN_CURTAIN	GFL_BG_FRAME2_M
#define ACT_FRAME_MAIN_BG		GFL_BG_FRAME3_M

#define ACT_FRAME_SUB_BG		GFL_BG_FRAME1_S
#define ACT_FRAME_SUB_INFO		GFL_BG_FRAME3_S

#define ACT_PAL_INFO		(0xE)
#define ACT_PAL_FONT		(0xF)

#define ACT_SPOT_NUM	(2)

#define ACT_OBJECT_IVALID (-1)

#define ACT_BG_SCROLL_MAX (256)

//Msg�n
#define ACT_MSG_POS_X ( 4 )
#define ACT_MSG_POS_Y ( 20 )
#define ACT_MSG_POS_WIDTH  ( 24 )
#define ACT_MSG_POS_HEIGHT ( 4 )

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
	AST_NONE,	//����
	AST_CIRCLE,	//�~�`
	AST_SHINES,	//��`
}ACT_SPOT_TYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
//�X�|�b�g���C�g
typedef struct
{
	ACT_SPOT_TYPE	type;	//�L���t���O��
	GXRgb	color;
	u8		alpha;
	fx32	posX;
	fx32	posY;
	fx32	val1;	//���a or ��̕�(����
	fx32	val2;	//���� or ���̕�(����
}ACT_SPOT_WORK;

struct _ACTING_WORK
{
	HEAPID heapId;
	
	u16		scrollOffset;
	u16		makuOffset;
	
	ACTING_INIT_WORK *initWork;

	MUS_POKE_DRAW_SYSTEM	*drawSys;
	MUS_ITEM_DRAW_SYSTEM	*itemDrawSys;
	
	STA_POKE_SYS		*pokeSys;
	STA_POKE_WORK		*pokeWork[MUSICAL_POKE_MAX];
	
	STA_OBJ_SYS			*objSys;
	STA_OBJ_WORK		*objWork[ACT_OBJECT_MAX];

	STA_EFF_SYS			*effSys;
	STA_EFF_WORK		*effWork[ACT_EFFECT_MAX];
	
	ACT_SPOT_WORK		spotWork[ACT_SPOT_NUM];

	GFL_G3D_CAMERA		*camera;
	GFL_BBD_SYS			*bbdSys;
	
	STA_SCRIPT_SYS		*scriptSys;
	
	GFL_TCBLSYS			*tcblSys;
	GFL_BMPWIN			*msgWin;
	GFL_FONT			*fontHandle;
	PRINT_STREAM		*printHandle;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
ACTING_WORK*	STA_ACT_InitActing( ACTING_INIT_WORK *initWork );
void	STA_ACT_TermActing( ACTING_WORK *work );
ACTING_RETURN	STA_ACT_LoopActing( ACTING_WORK *work );

static void STA_ACT_SetupGraphic( ACTING_WORK *work );
static void STA_ACT_SetupBg( ACTING_WORK *work );
static void	STA_ACT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane  , u8 mode );
static void STA_ACT_SetupPokemon( ACTING_WORK *work );
static void STA_ACT_SetupItem( ACTING_WORK *work );
static void STA_ACT_SetupEffect( ACTING_WORK *work );
static void STA_ACT_SetupMessage( ACTING_WORK *work );

static void STA_ACT_UpdateItem( ACTING_WORK *work );
static void STA_ACT_UpdateScroll( ACTING_WORK *work );
static void STA_ACT_UpdateMessage( ACTING_WORK *work );
static void STA_ACT_DrawSpotLight( ACTING_WORK *work );
static void STA_ACT_DrawSpotLight_Circle( ACTING_WORK *work , ACT_SPOT_WORK *spotWork );
static void STA_ACT_DrawSpotLight_Shines( ACTING_WORK *work , ACT_SPOT_WORK *spotWork );



static const GFL_DISP_VRAM vramBank = {
	GX_VRAM_BG_128_D,				// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_23_G,			// ���C��2D�G���W����BG�g���p���b�g
//	GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_16_F,				// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_NONE,			// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_01_AB,				// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_0123_E,			// �e�N�X�`���p���b�g�X���b�g
	GX_OBJVRAMMODE_CHAR_1D_32K,
	GX_OBJVRAMMODE_CHAR_1D_32K
};
//	A �e�N�X�`��
//	B �e�N�X�`��
//	C SubBg
//	D MainBg
//	E �e�N�X�`���p���b�g
//	F MainObj
//	G MainBgExPlt
//	H None
//	I None

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
#include "script/script_table.h"	//���̃X�N���v�g��`�p
#include "script/script_test.dat"	//�X�N���v�g�e�X�g�f�[�^
ACTING_WORK*	STA_ACT_InitActing( ACTING_INIT_WORK *initWork )
{
	u8 i;
	ACTING_WORK *work = GFL_HEAP_AllocMemory( initWork->heapId , sizeof( ACTING_WORK ));
	
	work->heapId = initWork->heapId;
	work->initWork = initWork;
	work->scrollOffset = 0;
	work->makuOffset = 0;
	STA_ACT_SetupGraphic( work );
	STA_ACT_SetupBg( work );
	STA_ACT_SetupMessage( work);
	STA_ACT_SetupPokemon( work );
	STA_ACT_SetupItem( work );
	STA_ACT_SetupEffect( work );
	
	for( i=0;i<ACT_SPOT_NUM;i++ )
	{
		work->spotWork[i].type = AST_NONE;
	}
	
	work->scriptSys = STA_SCRIPT_InitSystem( work->heapId , work );
	
	INFOWIN_Init( ACT_FRAME_SUB_INFO,ACT_PAL_INFO,work->heapId);

	SND_STRM_SetUp( ARCID_SNDSTRM, NARC_snd_strm_poketari_swav, SND_STRM_PCM8, SND_STRM_8KHZ, work->heapId );
	SND_STRM_Play();

	//�t�F�[�h�Ȃ��̂ŉ�����
	GX_SetMasterBrightness(0);	
	GXS_SetMasterBrightness(0);
	
	return work;
}

void	STA_ACT_TermActing( ACTING_WORK *work )
{
	//�t�F�[�h�Ȃ��̂ŉ�����
	GX_SetMasterBrightness(-16);	
	GXS_SetMasterBrightness(-16);

	SND_STRM_Stop();
	SND_STRM_Release();
	
	INFOWIN_Exit();
	
	STA_SCRIPT_ExitSystem( work->scriptSys );

	GFL_BMPWIN_Delete( work->msgWin );
	GFL_FONT_Delete( work->fontHandle );
	GFL_TCBL_Exit( work->tcblSys );
	
	STA_EFF_ExitSystem( work->effSys );
	STA_OBJ_ExitSystem( work->objSys );
	STA_POKE_ExitSystem( work->pokeSys );

	MUS_POKE_DRAW_TermSystem( work->drawSys );
	MUS_ITEM_DRAW_TermSystem( work->itemDrawSys );

	GFL_BBD_DeleteSys( work->bbdSys );
	GFL_G3D_CAMERA_Delete( work->camera );
	GFL_G3D_Exit();
	GFL_BG_FreeBGControl( ACT_FRAME_MAIN_3D );
	GFL_BG_FreeBGControl( ACT_FRAME_MAIN_FONT );
	GFL_BG_FreeBGControl( ACT_FRAME_MAIN_CURTAIN );
	GFL_BG_FreeBGControl( ACT_FRAME_MAIN_BG );
	GFL_BG_FreeBGControl( ACT_FRAME_SUB_BG );
	GFL_BG_FreeBGControl( ACT_FRAME_SUB_INFO );
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();

	GFL_HEAP_FreeMemory( work );
	
}

ACTING_RETURN	STA_ACT_LoopActing( ACTING_WORK *work )
{
#if DEB_ARI
	if(	GFL_UI_KEY_GetTrg() & PAD_BUTTON_R &&
		(GFL_UI_KEY_GetCont() & PAD_BUTTON_L) == 0)
	{
		VecFx32 pos = {ACT_POS_X(128.0f),ACT_POS_X(96.0f),FX32_CONST(190.0f)};
		STA_EFF_CreateEmmitter( work->effWork[0] , MUS_EFF_00_OP_TITLE01 , &pos );
		STA_EFF_CreateEmmitter( work->effWork[1] , MUS_EFF_01_OP_DEMOBG2_BUBBLE1 , &pos );
	}
	if(	GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
	{
		STA_SCRIPT_SetScript( work->scriptSys , (void*)musicalScriptTestData );
	}
	if(	GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
	{
		u8 i;
		VecFx32 scale;
		for( i=0;i<MUSICAL_POKE_MAX;i++ )
		{
			STA_POKE_GetScale( work->pokeSys , work->pokeWork[i] , &scale );
			scale.x *= -1;
			STA_POKE_SetScale( work->pokeSys , work->pokeWork[i] , &scale );
		}
	}

#endif

	INFOWIN_Update();
	STA_ACT_UpdateScroll(work);

#if DEB_ARI
	if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_L) == 0 ||
		GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
#endif
	{
		STA_SCRIPT_UpdateSystem( work->scriptSys );

		STA_POKE_UpdateSystem( work->pokeSys );
		STA_OBJ_UpdateSystem( work->objSys );
		STA_EFF_UpdateSystem( work->effSys );
		STA_ACT_UpdateMessage( work );
		
		MUS_POKE_DRAW_UpdateSystem( work->drawSys ); 
	}

	//3D�`��	
	GFL_G3D_DRAW_Start();
	GFL_G3D_DRAW_SetLookAt();
	{
		STA_ACT_DrawSpotLight( work );
//		STA_POKE_DrawSystem( work->pokeSys );
		MUS_POKE_DRAW_DrawSystem( work->drawSys ); 
//		STA_OBJ_DrawSystem( work->objSys );
		STA_POKE_UpdateSystem_Item( work->pokeSys );	//�|�P�̕`���ɓ���邱��
		GFL_BBD_Draw( work->bbdSys , work->camera , NULL );
		STA_EFF_DrawSystem( work->effSys );
	}
	GFL_G3D_DRAW_End();

	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT &&
		GFL_UI_KEY_GetCont() & PAD_BUTTON_START )
	{
		return ACT_RET_GO_END;
	}

	return ACT_RET_CONTINUE;
}


//--------------------------------------------------------------
//	�`��n������
//--------------------------------------------------------------
static void STA_ACT_SetupGraphic( ACTING_WORK *work )
{
	GFL_DISP_GX_InitVisibleControl();
	GFL_DISP_GXS_InitVisibleControl();
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );
	WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
	WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
	WIPE_ResetWndMask(WIPE_DISP_MAIN);
	WIPE_ResetWndMask(WIPE_DISP_SUB);
	
	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
	GFL_DISP_SetBank( &vramBank );
	
	GFL_BG_Init( work->heapId );
	GFL_BMPWIN_Init( work->heapId );
	
	//Vram���蓖�Ă̐ݒ�
	{
		static const GFL_BG_SYS_HEADER sys_data = {
				GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		
		// BG1 MAIN (����
		static const GFL_BG_BGCNT_HEADER header_main1 = {
			0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x08000,0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};
		// BG2 MAIN (��
		static const GFL_BG_BGCNT_HEADER header_main2 = {
			0, 0, 0x1000, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x00000,0x4000,
			GX_BG_EXTPLTT_01, 1, 1, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};
		// BG3 MAIN (�w�i 256*16�F
		static const GFL_BG_BGCNT_HEADER header_main3 = {
			0, 0, 0x2000, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x10000,0x10000,
			GX_BG_EXTPLTT_23, 3, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};

		// BG1 SUB (�w�i
		static const GFL_BG_BGCNT_HEADER header_sub1 = {
			0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000,0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};
		// BG3 SUB (Info
		static const GFL_BG_BGCNT_HEADER header_sub3 = {
			0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x6000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};

		GFL_BG_SetBGMode( &sys_data );
		STA_ACT_SetupBgFunc( &header_main1, ACT_FRAME_MAIN_FONT , GFL_BG_MODE_TEXT);
		STA_ACT_SetupBgFunc( &header_main2, ACT_FRAME_MAIN_CURTAIN , GFL_BG_MODE_TEXT);
		STA_ACT_SetupBgFunc( &header_main3, ACT_FRAME_MAIN_BG , GFL_BG_MODE_256X16);
		STA_ACT_SetupBgFunc( &header_sub1 , ACT_FRAME_SUB_BG , GFL_BG_MODE_TEXT);
		STA_ACT_SetupBgFunc( &header_sub3 , ACT_FRAME_SUB_INFO , GFL_BG_MODE_TEXT);
		
		GFL_BG_SetVisible( ACT_FRAME_MAIN_3D , TRUE );
	}
	
	{	//3D�n�̐ݒ�
		static const VecFx32 cam_pos = MUSICAL_CAMERA_POS;
		static const VecFx32 cam_target = MUSICAL_CAMERA_TRG;
		static const VecFx32 cam_up = MUSICAL_CAMERA_UP;
		//�G�b�W�}�[�L���O�J���[
		static	const	GXRgb stage_edge_color_table[8]=
			{ GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };
		GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT80K,
						0, work->heapId, NULL );
		GFL_BG_SetBGControl3D( 2 );	//NNS_g3dInit�̒��ŕ\���D�揇�ʕς��E�E�E
		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
#if 0	//�����ˉe�J����
		work->camera = 	GFL_G3D_CAMERA_Create( GFL_G3D_PRJPERS, 
											 FX32_SIN13,
											 FX32_COS13,
											 FX_F32_TO_FX32( 1.33f ),
											 NULL,
											 FX32_ONE,
											 FX32_ONE * 300,
											 NULL,
											 &cam_pos,
											 &cam_up,
											 &cam_target,
											 work->heapId );
#else
		//���ˉe�J����
		work->camera = 	GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH, 
											 FX32_ONE*12.0f,
											 0,
											 0,
											 FX32_ONE*16.0f,
											 MUSICAL_CAMERA_NEAR,
											 MUSICAL_CAMERA_FAR,
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
		G3X_AntiAlias( FALSE );
		G3X_AlphaBlend( TRUE );
		
		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );
	}
	{
		GFL_BBD_SETUP bbdSetup = {
			128,128,
			{FX32_ONE,FX32_ONE,FX32_ONE},
			GX_RGB(0,0,0),
			GX_RGB(0,0,0),
			GX_RGB(0,0,0),
			GX_RGB(0,0,0),
			0
		};
		//�r���{�[�h�V�X�e���\�z
		work->bbdSys = GFL_BBD_CreateSys( &bbdSetup , work->heapId );
	}
	
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0 , GX_BLEND_PLANEMASK_BG3 , 31 , 31 );
}

//--------------------------------------------------------------
//	BG�̏�����
//--------------------------------------------------------------
static void STA_ACT_SetupBg( ACTING_WORK *work )
{
	ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_STAGE_GRA , work->heapId );

	GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_stage_bg00_NCLR , 
										PALTYPE_MAIN_BG_EX , 0x6000 , 0 , work->heapId );
	GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_stage_bg00_NCGR ,
										ACT_FRAME_MAIN_BG , 0 , 0, FALSE , work->heapId );
	GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_stage_bg_NSCR , 
										ACT_FRAME_MAIN_BG ,  0 , 0, FALSE , work->heapId );

	GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_maku_NCLR , 
										PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
	GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_maku_NCGR ,
										ACT_FRAME_MAIN_CURTAIN , 0 , 0, FALSE , work->heapId );
	GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_maku_NSCR , 
										ACT_FRAME_MAIN_CURTAIN ,  0 , 0, FALSE , work->heapId );

	GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_audience_NCLR , 
										PALTYPE_SUB_BG , 0 , 0 , work->heapId );
	GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_audience_NCGR ,
										ACT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
	GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_audience_NSCR , 
										ACT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );

	GFL_ARC_CloseDataHandle(arcHandle);

	GFL_BG_LoadScreenReq(ACT_FRAME_MAIN_BG);
	GFL_BG_LoadScreenReq(ACT_FRAME_SUB_BG);

}

//--------------------------------------------------------------------------
//	Bg������ �@�\��
//--------------------------------------------------------------------------
static void	STA_ACT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
	GFL_BG_SetBGControl( bgPlane, bgCont, mode );
	GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
	GFL_BG_ClearFrame( bgPlane );
	GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------
//	�\���|�P�����̏�����
//--------------------------------------------------------------
static void STA_ACT_SetupPokemon( ACTING_WORK *work )
{
	u8 i;
	VecFx32 pos = {0,FX32_CONST(160.0f),FX32_CONST(170.0f)};
	const fx32 XBase = FX32_CONST(512.0f/(MUSICAL_POKE_MAX+1));
	
	work->drawSys = MUS_POKE_DRAW_InitSystem( work->heapId );
	work->itemDrawSys = MUS_ITEM_DRAW_InitSystem( work->bbdSys , MUSICAL_POKE_MAX*MUS_POKE_EQUIP_MAX, work->heapId );
	
	work->pokeSys = STA_POKE_InitSystem( work->heapId , work->drawSys , work->itemDrawSys );

	for( i=0;i<MUSICAL_POKE_MAX;i++ )
	{
		work->pokeWork[i] = NULL;
	}

	
	for( i=0;i<MUSICAL_POKE_MAX;i++ )
	{
		pos.x = XBase*(i+1);
		work->pokeWork[i] = STA_POKE_CreatePoke( work->pokeSys , &work->initWork->musPoke[i] );
		STA_POKE_SetPosition( work->pokeSys , work->pokeWork[i] , &pos );

		pos.z -= FX32_CONST(30.0f);	//�ЂƃL�����̌��݂�30�ƌ���
	}
}

//--------------------------------------------------------------
//�A�C�e���̏�����
//--------------------------------------------------------------
static void STA_ACT_SetupItem( ACTING_WORK *work )
{
	int i;
	VecFx32 pos = {0,0,0};
	
	for( i=0;i<ACT_OBJECT_MAX;i++ )
	{
		work->objWork[i] = NULL;
	}

	//�w�iObj
	{
		work->objSys = STA_OBJ_InitSystem( work->heapId , work->bbdSys );

		pos.x = FX32_CONST(448.0f );
		pos.y = FX32_CONST( 96.0f );
		pos.z = FX32_CONST( 50.0f);
		work->objWork[0] = STA_OBJ_CreateObject( work->objSys , 0 );
		STA_OBJ_SetPosition( work->objSys , work->objWork[0] , &pos );

		pos.x = FX32_CONST( 192.0f );
		pos.y = FX32_CONST( 100.0f );
		pos.z = FX32_CONST( 50.0f );
		work->objWork[1] = STA_OBJ_CreateObject( work->objSys , 1 );
		STA_OBJ_SetPosition( work->objSys , work->objWork[1] , &pos );

		pos.x = FX32_CONST(  80.0f );
		pos.y = FX32_CONST( 160.0f );
		pos.z = FX32_CONST( 180.0f );
		work->objWork[2] = STA_OBJ_CreateObject( work->objSys , 1 );
		STA_OBJ_SetPosition( work->objSys , work->objWork[2] , &pos );
	}


}

static void STA_ACT_SetupEffect( ACTING_WORK *work )
{
	int i;
	work->effSys = STA_EFF_InitSystem( work->heapId);

	for( i=0;i<ACT_EFFECT_MAX;i++ )
	{
		work->effWork[i] = NULL;
	}

	work->effWork[0] = STA_EFF_CreateEffect( work->effSys , NARC_stage_gra_mus_eff_00_spa );
	work->effWork[1] = STA_EFF_CreateEffect( work->effSys , NARC_stage_gra_mus_eff_01_spa );
}

static void STA_ACT_SetupMessage( ACTING_WORK *work )
{
	//���b�Z�[�W�p����
	work->msgWin = GFL_BMPWIN_Create( ACT_FRAME_MAIN_FONT , ACT_MSG_POS_X , ACT_MSG_POS_Y ,
									ACT_MSG_POS_WIDTH , ACT_MSG_POS_HEIGHT , ACT_PAL_FONT ,
									GFL_BMP_CHRAREA_GET_B );
	GFL_BMPWIN_TransVramCharacter( work->msgWin );
	GFL_BMPWIN_MakeScreen( work->msgWin );
	GFL_BG_LoadScreenReq(ACT_FRAME_MAIN_FONT);
	
	
	//�t�H���g�ǂݍ���
	work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );

	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , ACT_PAL_FONT*0x20, 16*1, work->heapId );
	
	work->tcblSys = GFL_TCBL_Init( work->heapId , work->heapId , 3 , 0x100 );
	work->printHandle = NULL;
}



static void STA_ACT_UpdateItem( ACTING_WORK *work )
{
	int i;
	int ePos;
	VecFx32 pos;
/*	
	for( i=0;i<MUSICAL_POKE_MAX;i++ )
	{
		VecFx32 pokePos;
		MUS_POKE_DRAW_GetPosition( work->drawWork[i] , &pokePos);
		for( ePos=0;ePos<MUS_POKE_EQUIP_MAX;ePos++ )
		{
			if( work->itemWork[i][ePos] != NULL )
			{
				MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( work->drawWork[i] , ePos );
				if( equipData->isEnable == TRUE )
				{
					pos.x = ACT_POS_X_FX(equipData->pos.x+FX32_CONST(128.0f));
					pos.y = ACT_POS_Y_FX(equipData->pos.y+FX32_CONST(96.0f));
					pos.z = pokePos.z+FX32_HALF;	//�Ƃ肠�����|�P�̑O�ɏo��
					//OS_Printf("[%.2f][%.2f]\n",F32_CONST(equipData->pos.z),F32_CONST(pokePos.z));
					MUS_ITEM_DRAW_SetPosition(	work->itemDrawSys , 
												work->itemWork[i][ePos] ,
												&pos );
					MUS_ITEM_DRAW_SetRotation(	work->itemDrawSys , 
												work->itemWork[i][ePos] ,
												equipData->rot );
					
				}
			}
		}
	}
*/
}

static void STA_ACT_UpdateScroll( ACTING_WORK *work )
{
	BOOL isUpdate = FALSE;
	const u8 spd = 2;
	if(	GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
	{
		if( work->scrollOffset + spd < ACT_BG_SCROLL_MAX )
		{
			work->scrollOffset += spd;
		}
		else
		{
			work->scrollOffset = ACT_BG_SCROLL_MAX;
		}
		isUpdate = TRUE;
	}
	if(	GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
	{
		if( work->scrollOffset - spd > 0 )
		{
			work->scrollOffset -= spd;
		}
		else
		{
			work->scrollOffset = 0;
		}
		
		isUpdate = TRUE;
	}
	
	if( isUpdate == TRUE )
	{
		GFL_BG_SetScroll( ACT_FRAME_MAIN_BG , GFL_BG_SCROLL_X_SET , work->scrollOffset );
		GFL_BG_SetScroll( ACT_FRAME_MAIN_CURTAIN , GFL_BG_SCROLL_X_SET , work->scrollOffset );
		STA_POKE_System_SetScrollOffset( work->pokeSys , work->scrollOffset ); 
		STA_OBJ_System_SetScrollOffset( work->objSys , work->scrollOffset ); 
	}
	
	if(	GFL_UI_KEY_GetCont() & PAD_KEY_UP )
	{
		if( work->makuOffset + spd < ACT_CURTAIN_SCROLL_MAX )
		{
			work->makuOffset += spd;
		}
		else
		{
			work->makuOffset = ACT_CURTAIN_SCROLL_MAX;
		}
	}
	if(	GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
	{
		if( work->makuOffset - spd > 0 )
		{
			work->makuOffset -= spd;
		}
		else
		{
			work->makuOffset = 0;
		}
		
	}
	GFL_BG_SetScroll( ACT_FRAME_MAIN_CURTAIN , GFL_BG_SCROLL_Y_SET , work->makuOffset );
}

#include <wchar.h>	//wcslen
static void STA_ACT_UpdateMessage( ACTING_WORK *work )
{
	GFL_TCBL_Main( work->tcblSys );

	if( work->printHandle != NULL  )
	{
		if( PRINTSYS_PrintStreamGetState( work->printHandle ) == PRINTSTREAM_STATE_DONE )
		{
			PRINTSYS_PrintStreamDelete( work->printHandle );
			work->printHandle = NULL;
		}
	}

#if DEB_ARI
	if( work->printHandle == NULL )
	{
		if( GFL_UI_KEY_GetCont() & PAD_BUTTON_A )
		{
			STRCODE str[48] = L"������\���e�X�g";
			STRBUF	*testStr = GFL_STR_CreateBuffer( 48 , work->heapId );
			const u16 strLen = wcslen(str);
			
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0 );
			
			str[strLen] = GFL_STR_GetEOMCode();
			GFL_STR_SetStringCode( testStr , str );

			work->printHandle = PRINTSYS_PrintStream( work->msgWin , 0,0, testStr ,work->fontHandle ,
													1 , work->tcblSys , 2 , work->heapId , 0 );
			GFL_STR_DeleteBuffer( testStr );
		}
	}
#endif
}

//�`��̂Ƃ�fx16�����g���Ȃ��̂ō��W�v�Z�̔{����ς���
#define ACT_SPOT_POS_X(val)	((val)/16/4)
#define ACT_SPOT_POS_Y(val)	(FX32_CONST(192.0f)-(val))/16/4
static void STA_ACT_DrawSpotLight( ACTING_WORK *work )
{
	u8 i;
	for( i=0;i<ACT_SPOT_NUM;i++ )
	{
		switch( work->spotWork[i].type )
		{
		case AST_CIRCLE:
			STA_ACT_DrawSpotLight_Circle( work , & work->spotWork[i] );
			break;
		case AST_SHINES:
			STA_ACT_DrawSpotLight_Shines( work , & work->spotWork[i] );
			break;
		}
	}
	
#if DEB_ARI
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
	{
		static u8 idx=0;
		if( work->spotWork[0].type == AST_NONE )
		{
			VecFx32 pos;
			STA_POKE_GetPosition( work->pokeSys , work->pokeWork[idx] , &pos );
			work->spotWork[0].type = AST_CIRCLE;
			work->spotWork[0].color = GX_RGB(31,31,0);
			work->spotWork[0].alpha = 15;
			work->spotWork[0].posX = pos.x;
			work->spotWork[0].posY = pos.y;
			work->spotWork[0].val1 = FX32_CONST(48.0f);
			
		}
		else
		if( work->spotWork[0].type == AST_CIRCLE )
		{
			VecFx32 pos;
			STA_POKE_GetPosition( work->pokeSys , work->pokeWork[idx] , &pos );
			work->spotWork[0].type = AST_SHINES;
			work->spotWork[0].color = GX_RGB(31,31,0);
			work->spotWork[0].alpha = 15;
			work->spotWork[0].posX = pos.x;
			work->spotWork[0].posY = pos.y+FX32_CONST(16.0f);
			work->spotWork[0].val1 = FX32_CONST(12.0f);
			work->spotWork[0].val2 = FX32_CONST(40.0f);
			
		}
		else
		if( work->spotWork[0].type == AST_SHINES )
		{
			work->spotWork[0].type = AST_NONE;
			idx = (idx+1)%MUSICAL_POKE_MAX;
		}
	}
#endif
}

static void STA_ACT_DrawSpotLight_Circle( ACTING_WORK *work , ACT_SPOT_WORK *spotWork )
{
	u32 i;
	u16 add = 0x800;
	const fx32 posX = spotWork->posX - FX32_CONST(work->scrollOffset);
	fx16 rad = ACT_SPOT_POS_X(spotWork->val1);
	G3_PushMtx();
	G3_PolygonAttr(GX_LIGHTMASK_NONE,  // no lights
				   GX_POLYGONMODE_MODULATE, 	// modulation mode
				   GX_CULL_NONE,	   // cull none
				   0,				   // polygon ID(0 - 63)
				   spotWork->alpha,	   // alpha(0 - 31)
				   GX_POLYGON_ATTR_MISC_XLU_DEPTH_UPDATE				   // OR of GXPolygonAttrMisc's value
		);

	G3_Translate( 0,0,FX32_CONST(199.0f));
	G3_Scale( FX32_ONE*4,FX32_ONE*4,FX32_ONE);
	G3_Begin(GX_BEGIN_TRIANGLES);
	{
		G3_Color(spotWork->color);
		for( i=0;i<0x10000;i+=0x800)
		{
			G3_Vtx(ACT_SPOT_POS_X(posX),ACT_SPOT_POS_Y(spotWork->posY),0);
			G3_Vtx(	ACT_SPOT_POS_X(posX) + FX_Mul(FX_SinIdx((u16)i),rad) ,
					ACT_SPOT_POS_Y(spotWork->posY) + FX_Mul(FX_CosIdx((u16)i),rad) ,
					0);
			G3_Vtx(	ACT_SPOT_POS_X(posX) + FX_Mul(FX_SinIdx((u16)(i+add)),rad) ,
					ACT_SPOT_POS_Y(spotWork->posY) + FX_Mul(FX_CosIdx((u16)(i+add)),rad) ,
					0);
			
		}
	}
	G3_End();
	G3_PopMtx(1);
}

static void STA_ACT_DrawSpotLight_Shines( ACTING_WORK *work , ACT_SPOT_WORK *spotWork )
{
	const fx32 posX = spotWork->posX - FX32_CONST(work->scrollOffset);
	G3_PushMtx();
	G3_PolygonAttr(GX_LIGHTMASK_NONE,  // no lights
				   GX_POLYGONMODE_MODULATE, 	// modulation mode
				   GX_CULL_NONE,	   // cull none
				   0,				   // polygon ID(0 - 63)
				   spotWork->alpha,	   // alpha(0 - 31)
				   GX_POLYGON_ATTR_MISC_XLU_DEPTH_UPDATE				   // OR of GXPolygonAttrMisc's value
		);

	G3_Translate( 0,0,FX32_CONST(199.0f));
	G3_Scale( FX32_ONE*4,FX32_ONE*4,FX32_ONE);
	G3_Begin(GX_BEGIN_QUADS);
	{
		G3_Color(spotWork->color);
		G3_Vtx(ACT_SPOT_POS_X(posX - spotWork->val1),ACT_SPOT_POS_Y(0),0);
		G3_Vtx(ACT_SPOT_POS_X(posX + spotWork->val1),ACT_SPOT_POS_Y(0),0);
		G3_Vtx(ACT_SPOT_POS_X(posX + spotWork->val2),ACT_SPOT_POS_Y(spotWork->posY),0);
		G3_Vtx(ACT_SPOT_POS_X(posX - spotWork->val2),ACT_SPOT_POS_Y(spotWork->posY),0);
	}
	G3_End();
	G3_End();
	G3_PopMtx(1);
}

#pragma mark [> offer func
//--------------------------------------------------------------
//	�X�N���v�g�p�ɊO���񋟊֐�
//--------------------------------------------------------------
STA_POKE_SYS* STA_ACT_GetPokeSys( ACTING_WORK *work )
{
	return work->pokeSys;
}
STA_POKE_WORK* STA_ACT_GetPokeWork( ACTING_WORK *work , const u8 idx )
{
	GF_ASSERT( idx < MUSICAL_POKE_MAX );
	return work->pokeWork[idx];
}
void STA_ACT_SetPokeWork( ACTING_WORK *work , STA_POKE_WORK *pokeWork , const u8 idx )
{
	GF_ASSERT( idx < MUSICAL_POKE_MAX );
	work->pokeWork[idx] = pokeWork;
}

STA_OBJ_SYS* STA_ACT_GetObjectSys( ACTING_WORK *work )
{
	return work->objSys;
}
STA_OBJ_WORK* STA_ACT_GetObjectWork( ACTING_WORK *work , const u8 idx )
{
	GF_ASSERT( idx < ACT_OBJECT_MAX );
	return work->objWork[idx];
}
void STA_ACT_SetObjectWork( ACTING_WORK *work , STA_OBJ_WORK *objWork , const u8 idx )
{
	GF_ASSERT( idx < ACT_OBJECT_MAX );
	work->objWork[idx] = objWork;
}

STA_EFF_SYS* STA_ACT_GetEffectSys( ACTING_WORK *work )
{
	return work->effSys;
}
STA_EFF_WORK* STA_ACT_GetEffectWork( ACTING_WORK *work , const u8 idx )
{
	GF_ASSERT( idx < ACT_EFFECT_MAX );
	return work->effWork[idx];
}
void STA_ACT_SetEffectWork( ACTING_WORK *work , STA_EFF_WORK *effWork , const u8 idx )
{
	GF_ASSERT( idx < ACT_EFFECT_MAX );
	work->effWork[idx] = effWork;
}

u16		STA_ACT_GetCurtainHeight( ACTING_WORK *work )
{
	return work->makuOffset;
}
void	STA_ACT_SetCurtainHeight( ACTING_WORK *work , const u16 height )
{
	work->makuOffset = height;
}
u16		STA_ACT_GetStageScroll( ACTING_WORK *work )
{
	return work->scrollOffset;
}
void	STA_ACT_SetStageScroll( ACTING_WORK *work , const u16 scroll )
{
	work->scrollOffset = scroll;
}



