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

#include "arc_def.h"
#include "musical_item.naix"
#include "stage_gra.naix"

#include "infowin/infowin.h"
#include "test/ariizumi/ari_debug.h"
#include "musical/mus_poke_draw.h"
#include "musical/mus_item_draw.h"

#include "musical/musical_stage_sys.h"
#include "musical/stage/sta_acting.h"

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

//BBD�p���W�ϊ�(�J�����̕�����v�Z
#define ACT_POS_X(val)	FX32_CONST((val)/16.0f)
#define ACT_POS_Y(val)	FX32_CONST((192.0f-(val))/16.0f)
#define ACT_POS_X_FX(val)	((val)/16)
#define ACT_POS_Y_FX(val)	(FX32_CONST(192.0f)-(val))/16

#define ACT_OBJECT_MAX (5)
#define ACT_OBJECT_IVALID (-1)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct
{
	int resIdx;
	int bbdIdx;
}ACT_OBJ_WORK;

struct _ACTING_WORK
{
	HEAPID heapId;
	
	ACTING_INIT_WORK *initWork;

	GFL_G3D_RES	*itemRes[MUSICAL_POKE_MAX][MUS_POKE_EQUIP_MAX];
	MUS_ITEM_DRAW_WORK	*itemWork[MUSICAL_POKE_MAX][MUS_POKE_EQUIP_MAX];

	ACT_OBJ_WORK	object[ACT_OBJECT_MAX];

	MUS_POKE_DRAW_SYSTEM	*drawSys;
	MUS_POKE_DRAW_WORK		*drawWork[MUSICAL_POKE_MAX];
	MUS_POKE_DATA_WORK		*pokeData[MUSICAL_POKE_MAX];
	MUS_ITEM_DRAW_SYSTEM	*itemDrawSys;

	GFL_G3D_CAMERA		*camera;
	GFL_BBD_SYS			*bbdSys;
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

static void STA_ACT_UpdateItem( ACTING_WORK *work );



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
ACTING_WORK*	STA_ACT_InitActing( ACTING_INIT_WORK *initWork )
{
	ACTING_WORK *work = GFL_HEAP_AllocMemory( initWork->heapId , sizeof( ACTING_WORK ));
	
	work->heapId = initWork->heapId;
	work->initWork = initWork;
	STA_ACT_SetupGraphic( work );
	STA_ACT_SetupBg( work );
	STA_ACT_SetupPokemon( work );
	STA_ACT_SetupItem( work );
	
	INFOWIN_Init( ACT_FRAME_SUB_INFO,ACT_PAL_INFO,work->heapId);

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
	
	if(GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
	{
		VecFx32 scale = {FX32_CONST(0.75f)*16,FX32_CONST(0.75f)*16,FX32_ONE};
		MUS_POKE_DRAW_SetScale( work->drawWork[0] , &scale );
		MUS_POKE_DRAW_SetScale( work->drawWork[1] , &scale );
		MUS_POKE_DRAW_SetScale( work->drawWork[2] , &scale );
		MUS_POKE_DRAW_SetScale( work->drawWork[3] , &scale );
	}
	else
	if(GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
	{
		VecFx32 scale = {FX32_CONST(1.00f)*16,FX32_CONST(1.00f)*16,FX32_ONE};
		MUS_POKE_DRAW_SetScale( work->drawWork[0] , &scale );
		MUS_POKE_DRAW_SetScale( work->drawWork[1] , &scale );
		MUS_POKE_DRAW_SetScale( work->drawWork[2] , &scale );
		MUS_POKE_DRAW_SetScale( work->drawWork[3] , &scale );
	}

	INFOWIN_Update();

	MUS_POKE_DRAW_UpdateSystem( work->drawSys ); 
	STA_ACT_UpdateItem( work );

	//3D�`��	
	GFL_G3D_DRAW_Start();
	GFL_G3D_DRAW_SetLookAt();
	{
		MUS_POKE_DRAW_DrawSystem( work->drawSys ); 
		GFL_BBD_Draw( work->bbdSys , work->camera , NULL );
	}
	GFL_G3D_DRAW_End();

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
			GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000,0x6000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};
		// BG2 MAIN (��
		static const GFL_BG_BGCNT_HEADER header_main2 = {
			0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x8000,
			GX_BG_EXTPLTT_01, 1, 1, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};
		// BG3 MAIN (�w�i 256*16�F
		static const GFL_BG_BGCNT_HEADER header_main3 = {
			0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
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
		GFL_BG_SetBGControl3D( 2 );
		STA_ACT_SetupBgFunc( &header_main1, ACT_FRAME_MAIN_FONT , GFL_BG_MODE_TEXT);
		STA_ACT_SetupBgFunc( &header_main2, ACT_FRAME_MAIN_CURTAIN , GFL_BG_MODE_TEXT);
		STA_ACT_SetupBgFunc( &header_main3, ACT_FRAME_MAIN_BG , GFL_BG_MODE_256X16);

		STA_ACT_SetupBgFunc( &header_sub1 , ACT_FRAME_SUB_BG , GFL_BG_MODE_TEXT);
		STA_ACT_SetupBgFunc( &header_sub3 , ACT_FRAME_SUB_INFO , GFL_BG_MODE_TEXT);
		
		GFL_BG_SetVisible( ACT_FRAME_MAIN_3D , TRUE );
	}
	
	{	//3D�n�̐ݒ�
//		static const VecFx32 cam_pos = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 7.8f ), FX_F32_TO_FX32( 21.0f ) };
//		static const VecFx32 cam_target = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 2.6f ), FX_F32_TO_FX32( 0.0f ) };
		static const VecFx32 cam_pos = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 101.0f ) };
		static const VecFx32 cam_target = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 0.0f ) };
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
											 FX32_ONE*12.0f,
											 0,
											 0,
											 FX32_ONE*16.0f,
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
		
		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );
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
	VecFx32 pos = {0,ACT_POS_Y(160.0f),FX32_CONST(-40000.0f)};
//	VecFx32 pos = {FX32_ONE*10,FX32_ONE*10,FX32_ONE*-7};	//�ʒu�͓K��
	u8 i;
	const float XBase = 256.0f/(MUSICAL_POKE_MAX+1);
	work->drawSys = MUS_POKE_DRAW_InitSystem( work->heapId );
	for( i=0;i<MUSICAL_POKE_MAX;i++ )
	{
		pos.x = ACT_POS_X(XBase*(i+1));
		pos.z -= FX32_CONST(500.0f);
		work->drawWork[i] = MUS_POKE_DRAW_Add( work->drawSys , &work->initWork->musPoke[i] );
		work->pokeData[i] = MUS_POKE_DRAW_GetPokeData( work->drawWork[i]);
		MUS_POKE_DRAW_SetPosition( work->drawWork[i] , &pos);
		MUS_POKE_DRAW_StartAnime( work->drawWork[i] );
	}
}

//--------------------------------------------------------------
//�A�C�e���̏�����
//--------------------------------------------------------------
static void STA_ACT_SetupItem( ACTING_WORK *work )
{
	int i;
	int ePos;
	VecFx32 pos = {0,0,0};
	work->itemDrawSys = MUS_ITEM_DRAW_InitSystem( work->bbdSys , MUSICAL_POKE_MAX*MUS_POKE_EQUIP_MAX, work->heapId );
	//�����i�̏�����
	for( i=0;i<MUSICAL_POKE_MAX;i++ )
	{
		for( ePos=0;ePos<MUS_POKE_EQUIP_MAX;ePos++ )
		{
			const u16 itemNo = work->initWork->musPoke[i].equip[ePos].type;
			if( itemNo != MUSICAL_ITEM_INVALID )
			{
				work->itemRes[i][ePos] = MUS_ITEM_DRAW_LoadResource( itemNo );
				work->itemWork[i][ePos] = MUS_ITEM_DRAW_AddResource( work->itemDrawSys , itemNo , work->itemRes[i][ePos] , &pos );
				MUS_ITEM_DRAW_SetSize( work->itemDrawSys , work->itemWork[i][ePos] , FX16_ONE , FX16_ONE );
			}
			else
			{
				work->itemRes[i][ePos] = NULL;
				work->itemWork[i][ePos] = NULL;
				
			}
		}
	}

	//�w�iObj
	{
		const BOOL flg = TRUE;
		for( i=0;i<ACT_OBJECT_MAX;i++ )
		{
			work->object[i].resIdx = ACT_OBJECT_IVALID;
			work->object[i].bbdIdx = ACT_OBJECT_IVALID;
		}
		
		pos.x = ACT_POS_X( 64.0f );
		pos.y = ACT_POS_Y( 96.0f );
		pos.z = FX32_CONST(-50.0f);
		
		work->object[0].resIdx = GFL_BBD_AddResourceArc( work->bbdSys , ARCID_STAGE_GRA , 
									NARC_stage_gra_stage_ojb01_nsbtx , GFL_BBD_TEXFMT_PAL256 ,
									GFL_BBD_TEXSIZ_128x128 , 128 , 128 );
		work->object[0].bbdIdx = GFL_BBD_AddObject( work->bbdSys , work->object[0].resIdx , FX16_CONST(4.0f) , FX16_CONST(4.0f) , 
									&pos , 31 , GFL_BBD_LIGHT_NONE );
		GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->object[0].bbdIdx , &flg );

		pos.x = ACT_POS_X( 192.0f );
		pos.y = ACT_POS_Y( 100.0f );
		pos.z = FX32_CONST(-50.0f);
		
		work->object[1].resIdx = GFL_BBD_AddResourceArc( work->bbdSys , ARCID_STAGE_GRA , 
									NARC_stage_gra_stage_ojb02_nsbtx , GFL_BBD_TEXFMT_PAL256 ,
									GFL_BBD_TEXSIZ_128x128 , 128 , 128 );
		work->object[1].bbdIdx = GFL_BBD_AddObject( work->bbdSys , work->object[1].resIdx , FX16_CONST(4.0f) , FX16_CONST(4.0f) , 
									&pos , 31 , GFL_BBD_LIGHT_NONE );
		GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->object[1].bbdIdx , &flg );

		pos.x = ACT_POS_X(  96.0f );
		pos.y = ACT_POS_Y( 152.0f );
		pos.z = FX32_CONST( 10.0f);
		
		work->object[2].resIdx = GFL_BBD_AddResourceArc( work->bbdSys , ARCID_STAGE_GRA , 
									NARC_stage_gra_stage_ojb02_nsbtx , GFL_BBD_TEXFMT_PAL256 ,
									GFL_BBD_TEXSIZ_128x128 , 128 , 128 );
		work->object[2].bbdIdx = GFL_BBD_AddObject( work->bbdSys , work->object[2].resIdx , FX16_CONST(4.0f) , FX16_CONST(4.0f) , 
									&pos , 31 , GFL_BBD_LIGHT_NONE );
		GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->object[2].bbdIdx , &flg );
	}
}

static void STA_ACT_UpdateItem( ACTING_WORK *work )
{
	int i;
	int ePos;
	VecFx32 pos;
	for( i=0;i<MUSICAL_POKE_MAX;i++ )
	{
		for( ePos=0;ePos<MUS_POKE_EQUIP_MAX;ePos++ )
		{
			if( work->itemWork[i][ePos] != NULL )
			{
				MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( work->drawWork[i] , ePos );
				if( equipData->isEnable == TRUE )
				{
					pos.x = ACT_POS_X_FX(equipData->pos.x+FX32_CONST(128.0f));
					pos.y = ACT_POS_Y_FX(equipData->pos.y+FX32_CONST(96.0f));
					pos.z = 0;
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
	GFL_BBD_GetObjectTrans( work->bbdSys , work->object[0].bbdIdx , &pos );
	GFL_BBD_SetObjectTrans( work->bbdSys , work->object[0].bbdIdx , &pos );
}
