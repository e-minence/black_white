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
#include "musical/mus_item_draw.h"
#include "dup_fitting.h"
#include "dup_fitting_item.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define FIT_FRAME_MAIN_3D	GFL_BG_FRAME0_M
#define FIT_FRAME_MAIN_CASE	GFL_BG_FRAME2_M
#define FIT_FRAME_MAIN_BG	GFL_BG_FRAME3_M
#define FIT_FRAME_SUB_BG	GFL_BG_FRAME3_S

#define DEG_TO_U16(val) ((val)*0x10000/360)
#define U16_TO_DEG(val) ((val)*360/0x10000)

//BBD�p���W�ϊ�(�J�����̕�����v�Z
#define FIT_POS_X(val)	FX32_CONST((val)/16.0f)
#define FIT_POS_Y(val)	FX32_CONST((192.0f-(val))/16.0f)
#define FIT_POS_X_FX(val)	((val)/16)
#define FIT_POS_Y_FX(val)	(FX32_CONST(192.0f)-(val))/16

//�|�P�����\���ʒu
static const int FIT_POKE_POS_X = 128;
static const int FIT_POKE_POS_Y = 112;
static const fx32 FIT_POKE_POS_X_FX = FIT_POS_X( FIT_POKE_POS_X );
static const fx32 FIT_POKE_POS_Y_FX = FIT_POS_Y( FIT_POKE_POS_Y );
static const fx32 FIT_POKE_POS_Z_FX = FX32_CONST(-40000.0f);

//�A�C�e���\����
static const u16 ITEM_LIST_NUM = 14;
//static const u16 ITEM_LIST_NUM = 48;
static const u16 ITEM_FIELD_NUM = 64;
static const u16 ITEM_EQUIP_NUM = MUS_POKE_EQUIP_MAX;//9
static const u16 ITEM_DISP_NUM = ITEM_LIST_NUM+ITEM_FIELD_NUM+ITEM_EQUIP_NUM+1;	//�y���Ŏ���

//�A�C�e�����X�g�n��`
//���S�_
static const int LIST_CENTER_X = 128;
static const int LIST_CENTER_Y = 80;
static const int LIST_CENTER_X_FX = FX32_CONST(LIST_CENTER_X);
static const int LIST_CENTER_Y_FX = FX32_CONST(LIST_CENTER_Y);
//�T�C�Y
static const int LIST_SIZE_X = 72;
static const int LIST_SIZE_Y = 48;
//FX32_CONST(LIST_SIZE_Y/LIST_SIZE_X)���蓮�v�Z
static const float LIST_SIZE_RATIO = 0.66f;
//�T�C�Y
static const int LIST_TPHIT_MAX_X = LIST_SIZE_X+20;
static const int LIST_TPHIT_MIN_X = LIST_SIZE_X-20;
static const int LIST_TPHIT_MAX_Y = LIST_SIZE_Y+20;
static const int LIST_TPHIT_MIN_Y = LIST_SIZE_Y-20;
static const float LIST_TPHIT_RATIO_MAX = (float)LIST_TPHIT_MAX_Y/(float)LIST_TPHIT_MAX_X;
static const float LIST_TPHIT_RATIO_MIN = (float)LIST_TPHIT_MIN_Y/(float)LIST_TPHIT_MIN_X;
//�k�����n�܂�ʒu(90�x)/�����n�߂�ʒu(155�x)/������ʒu(170�x)
static const u16 LIST_SIZE_DEPTH[3] = {0x4000,0x6E00,0x78E0};
//��]���n�߂�p�x(u16)
static const u16 LIST_ROTATE_ANGLE = DEG_TO_U16(2);

//�A�C�e��1�̊Ԋu
static const u16 LIST_ONE_ANGLE = 0x10000/ITEM_LIST_NUM;
static const u32 LIST_FULL_ANGLE = LIST_ONE_ANGLE*MUSICAL_ITEM_MAX;


//�t�B�[���h(�u���Ă���)�A�C�e���n
static const fx32 FIELD_ITEM_DEPTH = FX32_CONST(-10);	//���X�g��0�`8

//�����Ă�A�C�e���n
static const fx32 HOLD_ITEM_DEPTH = FX32_CONST(10.0f);
static const fx32 RETURN_LIST_ITEM_DEPTH = FX32_CONST(9.0f);

//�����A�C�e���n
static const u16 HOLD_ITEM_SNAP_LENGTH = 12;
static const fx32 EQUIP_ITEM_DEPTH = FX32_CONST(8.5f);


//���X�g�ɖ߂�A�j���[�V����
static const u16 ITEM_RETURN_ANIME_CNT = 30;


//�\�����ʃ���
//��
//�ێ��A�C�e��			10
//List�߂蒆			 9
//�����i				 8.5
//�A�C�e�����X�g		8�`0
//�t�B�[���h�A�C�e��	-10�`-10-0.1*�\����
//�|�P����				-40000	//���Ԃ�mcss�ō��W�ϊ��������Ă�E�E�E����ň�ԉ��ɏo��
//��

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
	IG_NONE,
	IG_LIST,
	IG_FIELD,
	IG_EQUIP,
	IG_ANIME,
}ITEM_GROUPE;


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

struct _FITTING_WORK
{
	HEAPID heapId;
	FITTING_INIT_WORK *initWork;
	MUS_POKE_DATA_WORK	*pokeData;
	GFL_TCB *vBlankTcb;
	
	//TP�n
	BOOL tpIsTrg;
	BOOL tpIsTouch;
	u32 tpx,tpy;
	u32 befTpx,befTpy;
	u16 listBefAngle;
	BOOL listAngleEnable;	//���̕ϐ��̗L����
	BOOL isOpenList;		//���X�g�̕\�����
	MUS_POKE_EQUIP_POS	snapPos;	//�����ɋz�����Ă���
	
	FIT_ITEM_WORK	*holdItem;	//�ێ����Ă���A�C�e��
	ITEM_GROUPE		holdItemType;

	//�A�C�e���n
	GFL_G3D_RES	*itemRes[MUSICAL_ITEM_MAX];
	FIT_ITEM_GROUP	*itemGroupList;
	FIT_ITEM_GROUP	*itemGroupField;
	FIT_ITEM_GROUP	*itemGroupEquip;
	//���X�g�ɖ߂�Ƃ��ȂǑ���s�ŃA�j������ł��镨
	FIT_ITEM_GROUP	*itemGroupAnime;

	u16	listAngle;
	s16 listSpeed;
	u16	listHoldMove;	//�����Ă���Ƃ��̈ړ���
	s32	listTotalMove;	//���X�g�S�̂̈ړ���
	
	//3D�`��Ɋւ�镨
	MUS_POKE_DRAW_SYSTEM	*drawSys;
	MUS_POKE_DRAW_WORK		*drawWork;
	MUS_ITEM_DRAW_SYSTEM	*itemDrawSys;
	GFL_G3D_CAMERA			*camera;
	GFL_BBD_SYS				*bbdSys;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void DUP_FIT_VBlankFunc(GFL_TCB *, void * );

static void DUP_FIT_SetupGraphic( FITTING_WORK *work );
static void DUP_FIT_SetupPokemon( FITTING_WORK *work );
static void DUP_FIT_SetupItem( FITTING_WORK *work );
static void DUP_FIT_CalcItemListAngle( FITTING_WORK *work , u16 angle , s16 moveAngle );
static void DUP_FIT_OpenItemList( FITTING_WORK *work );
static void DUP_FIT_CloseItemList( FITTING_WORK *work );

static void DUP_FIT_UpdateTpMain( FITTING_WORK *work );
static void DUP_FIT_UpdateTpList( FITTING_WORK *work , s16 subX , s16 subY );
static void DUP_FIT_UpdateTpHoldItem( FITTING_WORK *work );
static void DUP_FIT_UpdateTpHoldingItem( FITTING_WORK *work );
static void DUP_FIT_UpdateTpDropItemToField( FITTING_WORK *work );
static void DUP_FIT_UpdateTpDropItemToList( FITTING_WORK *work );
static void DUP_FIT_UpdateTpDropItemToEquip(  FITTING_WORK *work );

static const BOOL DUP_FIT_CheckIsEquipItem( FITTING_WORK *work , const MUS_POKE_EQUIP_POS pos);
static void DUP_FIT_UpdateItemAnime( FITTING_WORK *work );
static void DUP_FIT_CreateItemListToField( FITTING_WORK *work );


static BOOL DUP_FIT_CheckPointInOval( s16 subX , s16 subY , s16 size , float ratioYX );


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
	work->tpIsTrg = FALSE;
	work->tpIsTouch = FALSE;
	work->listAngleEnable = FALSE;
	work->holdItem = NULL;
	work->holdItemType = IG_NONE;
	work->listTotalMove = LIST_FULL_ANGLE-0x8000;	//����]�̈ʒu����X�^�[�g
	work->snapPos = MUS_POKE_EQU_INVALID;
	DUP_FIT_SetupGraphic( work );
	DUP_FIT_SetupPokemon( work );
	DUP_FIT_SetupItem( work );
	
	GFUser_VIntr_CreateTCB( DUP_FIT_VBlankFunc , work , 8 );
	
	GX_SetMasterBrightness(0);	
	GXS_SetMasterBrightness(0);
	return work;
}

//--------------------------------------------------------------
//	�������C�� �J��
//--------------------------------------------------------------
void	DUP_FIT_TermFitting( FITTING_WORK *work )
{
	GFL_TCB_DeleteTask( work->vBlankTcb );
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

#if DEB_ARI
	VecFx32 scale,pos;
	GFL_BBD_GetScale( work->bbdSys , &scale );
	GFL_BBD_SetScale( work->bbdSys , &scale );
	MUS_POKE_DRAW_GetPosition( work->drawWork , &pos);
	MUS_POKE_DRAW_SetPosition( work->drawWork , &pos);
	if(GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
	{
		work->listAngle += 0x100;
		DUP_FIT_CalcItemListAngle( work , work->listAngle , -0x100 );
	}
	if(GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
	{
		work->listAngle -= 0x100;
		DUP_FIT_CalcItemListAngle( work , work->listAngle , +0x100 );
	}
	if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
	{
		if( work->isOpenList == TRUE )
		{
			DUP_FIT_CloseItemList( work);
		}
		else
		{
			DUP_FIT_OpenItemList( work);
		}
	}
#endif

	DUP_FIT_UpdateTpMain( work );
	DUP_FIT_UpdateItemAnime( work );

	MUS_POKE_DRAW_UpdateSystem( work->drawSys ); 

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

static void DUP_FIT_VBlankFunc(GFL_TCB *tcb, void *wk )
{
	FITTING_WORK *work = wk;
	FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
	while( item != NULL )
	{
		MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
		const u16 itemId = DUP_FIT_ITEM_GetItemIdx( item );
		const u16 newId = DUP_FIT_ITEM_GetNewItemIdx( item );
		
		if( itemId != newId )
		{
			MUS_ITEM_DRAW_ChengeGraphic(  work->itemDrawSys , itemDrawWork , newId , work->itemRes[MUS_ITEM_DRAW_GetArcIdx(newId)] );
			DUP_FIT_ITEM_SetItemIdx( item , newId );
		}
		
		
		item = DUP_FIT_ITEM_GetNextItem(item);
	}
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
//	�\���|�P�����̏�����
//--------------------------------------------------------------
static void DUP_FIT_SetupPokemon( FITTING_WORK *work )
{
	VecFx32 pos = {FIT_POKE_POS_X_FX,FIT_POKE_POS_Y_FX,FIT_POKE_POS_Z_FX};
//	VecFx32 pos = {FX32_ONE*10,FX32_ONE*10,FX32_ONE*-7};	//�ʒu�͓K��
	work->drawSys = MUS_POKE_DRAW_InitSystem( work->heapId );
	work->drawWork = MUS_POKE_DRAW_Add( work->drawSys , work->initWork->musPoke );
	work->pokeData = MUS_POKE_DRAW_GetPokeData( work->drawWork);
	MUS_POKE_DRAW_SetPosition( work->drawWork , &pos);
}

//--------------------------------------------------------------
//�A�C�e���̏�����
//--------------------------------------------------------------
static void DUP_FIT_SetupItem( FITTING_WORK *work )
{
	int i;
	work->itemDrawSys = MUS_ITEM_DRAW_InitSystem( work->bbdSys , ITEM_DISP_NUM , work->heapId );
	for( i=0;i<MUSICAL_ITEM_MAX;i++ )
	{
		work->itemRes[i] = MUS_ITEM_DRAW_LoadResource( i );
	}
	
	work->itemGroupList = DUP_FIT_ITEMGROUP_CreateGroup( work->heapId , ITEM_LIST_NUM );
	work->itemGroupField = DUP_FIT_ITEMGROUP_CreateGroup( work->heapId , ITEM_FIELD_NUM );
	work->itemGroupEquip = DUP_FIT_ITEMGROUP_CreateGroup( work->heapId , MUS_POKE_EQUIP_MAX );
	work->itemGroupAnime = DUP_FIT_ITEMGROUP_CreateGroup( work->heapId , 4 );
	for( i=0;i<ITEM_LIST_NUM;i++ )
	{
		FIT_ITEM_WORK* item;
		VecFx32 pos = {0,0,0};
		
		item = DUP_FIT_ITEM_CreateItem( work->heapId , work->itemDrawSys , i , work->itemRes[i] , &pos );
		DUP_FIT_ITEMGROUP_AddItem( work->itemGroupList,item );
	}
	DUP_FIT_CalcItemListAngle( work , 0 , 0 );
	work->isOpenList = TRUE;
}

//--------------------------------------------------------------
//�A�C�e�����X�g�̈ʒu�E�T�C�Y�v�Z
//--------------------------------------------------------------
static void DUP_FIT_CalcItemListAngle( FITTING_WORK *work , u16 angle , s16 moveAngle )
{
	//�~�͉�����O��0�x�Ƃ��āAdepth�͉���0x8000 �オ0x0000
	int i = 0;
	FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
	
	work->listTotalMove += moveAngle;
	if( work->listTotalMove < 0 )
	{
		work->listTotalMove += LIST_FULL_ANGLE;
	}
	if( work->listTotalMove >= LIST_FULL_ANGLE )
	{
		work->listTotalMove -= LIST_FULL_ANGLE;
	}
	

	while( item != NULL )
	{
		GFL_POINT dispPos;
		VecFx32 pos;
		u16 oneAngle = (i*0x10000/ITEM_LIST_NUM+angle)%0x10000;
		u16 depth;
		MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
		const fx32 sin = (fx32)FX_SinIdx( oneAngle );
		const fx32 cos = (fx32)FX_CosIdx( oneAngle );
		const fx32 posX = LIST_CENTER_X_FX+sin*LIST_SIZE_X;
		const fx32 posY = LIST_CENTER_Y_FX+cos*LIST_SIZE_Y;
		
		//�G�̓ǂݑւ������Ƃ�
		{
			const u16 nowId = DUP_FIT_ITEM_GetItemIdx( item );
			u16 newId;
			s32 subAngle;
			s32 itemTotalAngle;

			subAngle = (oneAngle + 0x8000 )%0x10000;
			itemTotalAngle = (work->listTotalMove+subAngle);
			if( itemTotalAngle < 0 )
			{
				itemTotalAngle += LIST_FULL_ANGLE;
			}
			if( itemTotalAngle >= LIST_FULL_ANGLE )
			{
				itemTotalAngle -= LIST_FULL_ANGLE;
			}
			
			newId = (itemTotalAngle+(LIST_ONE_ANGLE/2))/LIST_ONE_ANGLE;
			DUP_FIT_ITEM_SetNewItemIdx( item , newId );
		}
		
		//�ʒu�̌v�Z
		if( oneAngle < 0x8000 )
		{
			depth = oneAngle;
		}
		else
		{
			depth = 0x8000-(oneAngle-0x8000);
		}
		dispPos.x = F32_CONST(posX);
		dispPos.y = F32_CONST(posY);
		pos.x = FIT_POS_X_FX(posX);
		pos.y = FIT_POS_Y_FX(posY);
		pos.z = 0x8000-depth;
	
		MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , itemDrawWork , &pos );
		DUP_FIT_ITEM_SetPosition( item , &dispPos );
		//�T�C�Y�̌v�Z
		{
			if( depth < LIST_SIZE_DEPTH[0] )
			{
				MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork , FX16_ONE , FX16_ONE );
				MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , itemDrawWork , TRUE );
				DUP_FIT_ITEM_SetScale( item , FX32_ONE );
			}
			else if( depth < LIST_SIZE_DEPTH[1] )
			{
				//100%��50%
				const u16 subAngle = LIST_SIZE_DEPTH[1]-depth;
				const fx16 size = (FX16_CONST(0.5f)*subAngle/(LIST_SIZE_DEPTH[1]-LIST_SIZE_DEPTH[0]))+FX16_CONST(0.5f);
				MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork , size,size );
				MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , itemDrawWork , TRUE );
				DUP_FIT_ITEM_SetScale( item , size );
			}
			else if( depth < LIST_SIZE_DEPTH[2] )
			{
				//50%��0%
				const u16 subAngle = LIST_SIZE_DEPTH[2]-depth;
				const fx16 size = FX16_CONST(0.5f)*subAngle/(LIST_SIZE_DEPTH[2]-LIST_SIZE_DEPTH[1]);
				MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork , size,size );
				MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , itemDrawWork , TRUE );
				DUP_FIT_ITEM_SetScale( item , size );
			}
			else
			{
				MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , itemDrawWork , FALSE );
				DUP_FIT_ITEM_SetScale( item , 0 );
			}
		}
		
		item = DUP_FIT_ITEM_GetNextItem(item);
		i++;
	}
	item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
	
}

//--------------------------------------------------------------
//�A�C�e�����X�g�̕\��(�J�n
//--------------------------------------------------------------
static void DUP_FIT_OpenItemList( FITTING_WORK *work )
{
	FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
	GF_ASSERT( work->isOpenList == FALSE );
	//���X�g�̊p�x�v�Z���ŕ\����\��������Ă���̂ŁA�����őS�A�C�e����Ώۂɂ��������͂Ȃ�
/*
	while( item != NULL )
	{
		MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
		
		MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , drawWork , FALSE );

		item = DUP_FIT_ITEM_GetNextItem(item);
	}
*/
	DUP_FIT_CalcItemListAngle( work , work->listAngle , 0 );
	
	work->isOpenList = TRUE;

}
//--------------------------------------------------------------
//�A�C�e�����X�g�̔�\��(�J�n
//--------------------------------------------------------------
static void DUP_FIT_CloseItemList( FITTING_WORK *work )
{
	FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
	GF_ASSERT( work->isOpenList == TRUE );
	while( item != NULL )
	{
		MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
		
		MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , drawWork , FALSE );

		item = DUP_FIT_ITEM_GetNextItem(item);
	}

	work->isOpenList = FALSE;
	
}

//--------------------------------------------------------------
//�^�b�`�y������X�V���C��
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpMain( FITTING_WORK *work )
{
	BOOL isTouchList = FALSE;
	work->befTpx = work->tpx;
	work->befTpy = work->tpy;
	work->tpIsTrg = GFL_UI_TP_GetTrg();
	work->tpIsTouch = GFL_UI_TP_GetPointCont( &work->tpx,&work->tpy );
	if( work->tpIsTrg || work->tpIsTouch )
	{
		
		if( work->holdItemType == IG_FIELD && work->holdItem != NULL )
		{
			//�A�C�e���ێ����̏���
			DUP_FIT_UpdateTpHoldingItem( work );
		}
		else if( work->isOpenList == TRUE )
		{
			//�A�C�e�����X�g�̒����H
			//X���W�ɂ͔��������Y�̃T�C�Y�Ōv�Z����
			const s16 subX = work->tpx - LIST_CENTER_X;
			const s16 subY = work->tpy - LIST_CENTER_Y;
			const u32 centerLen = F32_CONST(FX_Sqrt(FX32_CONST(subX*subX+subY*subY)));
			//�����̉~�ɋ��邩�H
			BOOL hitMinOval,hitMaxOval;
			hitMinOval = DUP_FIT_CheckPointInOval( subX,subY,LIST_TPHIT_MIN_Y,LIST_TPHIT_RATIO_MIN );
			hitMaxOval = DUP_FIT_CheckPointInOval( subX,subY,LIST_TPHIT_MAX_Y,LIST_TPHIT_RATIO_MAX );
			if( hitMinOval == FALSE && hitMaxOval == TRUE )
			{
				isTouchList = TRUE;
				DUP_FIT_UpdateTpList( work , subX , subY );
			}
			else if( (hitMinOval == FALSE && hitMaxOval == FALSE )||
					 (hitMinOval == TRUE && hitMaxOval == TRUE ))
			{
				if( DUP_FIT_ITEMGROUP_IsItemMax(work->itemGroupField) == FALSE )
				{
					//�~�̓���������
					if( work->holdItemType == IG_LIST &&
						work->holdItem != NULL )
					{
						DUP_FIT_CreateItemListToField( work );
					}
				}
			}
		}
		if( isTouchList == FALSE && work->tpIsTrg == TRUE )
		{
			DUP_FIT_UpdateTpHoldItem(work);
		}
	}
	else
	{
		//�A�C�e�����X�g�̒����H
		//X���W�ɂ͔��������Y�̃T�C�Y�Ōv�Z����
		const s16 subX = work->befTpx - LIST_CENTER_X;
		const s16 subY = work->befTpy - LIST_CENTER_Y;
		const u32 centerLen = F32_CONST(FX_Sqrt(FX32_CONST(subX*subX+subY*subY)));
		//�����̉~�ɋ��邩�H
		BOOL hitMinOval,hitMaxOval;
		hitMinOval = DUP_FIT_CheckPointInOval( subX,subY,LIST_TPHIT_MIN_Y,LIST_TPHIT_RATIO_MIN );
		hitMaxOval = DUP_FIT_CheckPointInOval( subX,subY,LIST_TPHIT_MAX_Y,LIST_TPHIT_RATIO_MAX );
		if( work->holdItemType == IG_FIELD && work->holdItem != NULL )
		{
			if( work->snapPos != MUS_POKE_EQU_INVALID )
			{
				DUP_FIT_UpdateTpDropItemToEquip( work );
			}
			else
			if( hitMinOval == FALSE && hitMaxOval == TRUE &&
				work->isOpenList == TRUE )
			{
				DUP_FIT_UpdateTpDropItemToList( work );
			}
			else
			{
				DUP_FIT_UpdateTpDropItemToField( work );
			}
		}
		
		work->listAngleEnable = FALSE;
		work->holdItem = NULL;
		work->holdItemType = IG_NONE;
		work->snapPos = MUS_POKE_EQU_INVALID;
	}
	//�����ŉ��`
	if( work->listSpeed != 0 && isTouchList == FALSE )
	{
		s32 tempAngle = (s32)work->listAngle+0x10000-work->listSpeed;
		if( tempAngle < 0 )
			tempAngle += 0x10000;
		if( tempAngle >= 0x10000 )
			tempAngle -= 0x10000;
		work->listAngle = tempAngle;
		DUP_FIT_CalcItemListAngle( work , work->listAngle , work->listSpeed );
		if( work->listSpeed > 0x80 || work->listSpeed < -0x80 )
		{
			work->listSpeed *= 0.75f;
		}
		else
		{ 
			work->listSpeed = 0;
		}
	}	
}
//--------------------------------------------------------------
//�^�b�`�y������X�V���X�g
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpList( FITTING_WORK *work , s16 subX , s16 subY )
{
	//��]�͑ȉ~�Ȃ̂Ŋp�x�Ō��Ȃ��ق����ǂ�����
	//�ł��p�x����Ȃ��Ɖ񂹂Ȃ��E�E�E
	const u16 angle = FX_Atan2Idx(subX*FX32_ONE,subY*FX32_ONE);
	
	//���݃`�F�b�N
	if( work->tpIsTrg == TRUE )
	{
		FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
		//���łɏ�����
		work->listSpeed = 0;
		work->listHoldMove = 0;
		work->holdItem = NULL;

		while( item != NULL )
		{
			//�T�C�Y���C�ɂ��邩�H
//			if( DUP_FIT_ITEM_GetScale( item ) == FX32_ONE &&
//				DUP_FIT_ITEM_CheckHit( item , work->tpx,work->tpy ) == TRUE )
			if( DUP_FIT_ITEM_CheckHit( item , work->tpx,work->tpy ) == TRUE )
			{
				work->holdItem = item;
				work->holdItemType = IG_LIST;
				//DUP_FIT_ITEM_CheckLengthSqrt( item , work->tpx,work->tpy );
				ARI_TPrintf("Item Hold\n");
				item = NULL;
			}
			else
			{
				item = DUP_FIT_ITEM_GetNextItem(item);
			}
		}
	}
	//��]����
	if( work->listAngleEnable == TRUE )
	{
		u8 i;
		//�O��Ƃ̃��X�g�ɑ΂���p�x��
		const s32 subAngle = work->listBefAngle - angle;
		//�O��̍��W����ǂ����ɓ��������H�̊p�x
		//const u16 moveAngle = FX_Atan2Idx((work->tpx-work->befTpx)*FX32_ONE,(work->tpy-work->befTpy)*FX32_ONE);
		//��]�`�F�b�N
		if( subAngle > LIST_ROTATE_ANGLE ||
			subAngle < -LIST_ROTATE_ANGLE )
		{
			s32 tempAngle = (s32)work->listAngle+0x10000-subAngle;
			if( tempAngle < 0 )
				tempAngle += 0x10000;
			if( tempAngle >= 0x10000 )
				tempAngle -= 0x10000;
			work->listAngle = tempAngle;
			DUP_FIT_CalcItemListAngle( work , work->listAngle , subAngle );
			
			if( ( work->listSpeed>0 && work->listSpeed<0) ||
				( work->listSpeed<0 && work->listSpeed>0) )
			{
				work->listSpeed = 0;
			}
			else
			{
				work->listSpeed = subAngle;
			}

			work->listHoldMove += MATH_ABS(subAngle);
			if( work->listHoldMove >= DEG_TO_U16(10) )
			{
				//�ړ����������̂ŃA�C�e���ێ�����
				work->holdItem = NULL;
				work->holdItemType = IG_NONE;
			}
		}
		else
		{
			work->listSpeed = 0;
		}
	}
	work->listBefAngle = angle;
	work->listAngleEnable = TRUE;
}

//--------------------------------------------------------------
//�t�B�[���h�E�����̃A�C�e�����E��
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpHoldItem( FITTING_WORK *work )
{
	BOOL isEquipList = TRUE;
	FIT_ITEM_WORK* item;
	item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
	if( item == NULL || DUP_FIT_ITEMGROUP_IsItemMax(work->itemGroupField) == TRUE )
	{
		//�t�B�[���h�̃A�C�e���������ς��Ȃ瑕���A�C�e���͏E���Ȃ�
		item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupField );
		isEquipList = FALSE;
	}
	while( item != NULL )
	{
		if( DUP_FIT_ITEM_CheckHit( item , work->tpx,work->tpy ) == TRUE )
		{
			work->holdItem = item;
			work->holdItemType = IG_FIELD;
			//DUP_FIT_ITEM_CheckLengthSqrt( item , work->tpx,work->tpy );
			ARI_TPrintf("Item Hold[%d]\n",DUP_FIT_ITEM_GetItemIdx(item));
			item = NULL;
		}
		else
		{
			item = DUP_FIT_ITEM_GetNextItem(item);
			if( item == NULL && isEquipList == TRUE )
			{
				item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupField );
				isEquipList = FALSE;
			}
		}
		
	}
	
	if( isEquipList == TRUE && item != work->holdItem )
	{
		DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupEquip , work->holdItem );
		DUP_FIT_ITEMGROUP_AddItem( work->itemGroupField , work->holdItem );
		//���W���킹�Ɛ[�x�ݒ�
		DUP_FIT_UpdateTpHoldingItem( work );
	}

}

//--------------------------------------------------------------
//�t�B�[���h�̃A�C�e���������Ă���
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpHoldingItem( FITTING_WORK *work )
{
	u16 snapLen = HOLD_ITEM_SNAP_LENGTH;
	GFL_POINT dispPos;
	GFL_POINT pokePosSub;
	VecFx32 pos = {0,0,HOLD_ITEM_DEPTH};
	fx16 scaleX,scaleY;
	MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
	
	pokePosSub.x = work->tpx - FIT_POKE_POS_X;
	pokePosSub.y = work->tpy - FIT_POKE_POS_Y;
	work->snapPos = MUS_POKE_DATA_SearchEquipPosition( work->pokeData , &pokePosSub , &snapLen );
	if( work->snapPos != MUS_POKE_EQU_INVALID &&
		DUP_FIT_CheckIsEquipItem( work , work->snapPos ) == FALSE )
	{
		
		dispPos = *MUS_POKE_DATA_GetEquipPosition( work->pokeData , work->snapPos );
		dispPos.x += FIT_POKE_POS_X;
		dispPos.y += FIT_POKE_POS_Y;
	}
	else
	{
		work->snapPos = MUS_POKE_EQU_INVALID;
		dispPos.x = work->tpx;
		dispPos.y = work->tpy;
	}

	pos.x = FIT_POS_X(dispPos.x);
	pos.y = FIT_POS_Y(dispPos.y);
	MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , itemDrawWork , &pos );
	DUP_FIT_ITEM_SetPosition( work->holdItem , &dispPos );
	
	MUS_ITEM_DRAW_GetSize( work->itemDrawSys , itemDrawWork ,&scaleX,&scaleY );
	if( scaleX < FX16_ONE || scaleY < FX16_ONE )
	{
		scaleX += FX16_CONST( 0.1f );
		scaleY += FX16_CONST( 0.1f );
		if( scaleX > FX16_ONE )
		{
			scaleX = FX16_ONE;
		}
		if( scaleY > FX16_ONE )
		{
			scaleY = FX16_ONE;
		}
		MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork ,scaleX,scaleY );
	}
}


//--------------------------------------------------------------
//���X�g����t�B�[���h�փA�C�e����u��(���̂܂ܕێ���Ԃ�
//--------------------------------------------------------------
static void DUP_FIT_CreateItemListToField( FITTING_WORK *work )
{
	FIT_ITEM_WORK* item;
	VecFx32 pos = {0,0,HOLD_ITEM_DEPTH};
	u16 itemIdx = DUP_FIT_ITEM_GetItemIdx( work->holdItem );
	MUS_ITEM_DRAW_WORK *itemDrawWork,*holdDrawWork;
	GFL_POINT dispPos;
	fx16 scaleX,scaleY;

	dispPos.x = work->tpx;
	dispPos.y = work->tpy;
	pos.x = FIT_POS_X(work->tpx);
	pos.y = FIT_POS_Y(work->tpy);

	item = DUP_FIT_ITEM_CreateItem( work->heapId , work->itemDrawSys , itemIdx , work->itemRes[itemIdx] , &pos );
	DUP_FIT_ITEMGROUP_AddItem( work->itemGroupField,item );

	DUP_FIT_ITEM_SetPosition( item , &dispPos );
	//�T�C�Y�̈��p��
	holdDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
	itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
	MUS_ITEM_DRAW_GetSize( work->itemDrawSys , holdDrawWork ,&scaleX,&scaleY );
	MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork ,scaleX,scaleY );

	//�ێ��A�C�e�����������������̂ɕς���
	work->holdItem = item;
	work->holdItemType = IG_FIELD;
}

//--------------------------------------------------------------
//�A�C�e���𗣂� �t�B�[���h��
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpDropItemToField( FITTING_WORK *work )
{
	fx32 depth = FIELD_ITEM_DEPTH;
	FIT_ITEM_WORK *item;
	MUS_ITEM_DRAW_WORK *holdDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
	//��x�A�C�e�������X�g������A���ɂȂ�����
	DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupField , work->holdItem );
	DUP_FIT_ITEMGROUP_AddItemTop( work->itemGroupField , work->holdItem );
	
	MUS_ITEM_DRAW_SetSize( work->itemDrawSys , holdDrawWork ,FX16_ONE,FX16_ONE );
	//�[�x�̍Đݒ�
	item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupField );
	while( item != NULL )
	{
		VecFx32 pos;
		MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
		MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );
		pos.z = depth;
		MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );
		
		depth -= FX32_CONST(0.1f);
		item = DUP_FIT_ITEM_GetNextItem(item);
	}
	
#if DEB_ARI
	DUP_FIT_ITEM_DumpList( work->itemGroupField ,2 );
#endif
}
//--------------------------------------------------------------
//�A�C�e���𗣂� ���X�g��
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpDropItemToList( FITTING_WORK *work )
{
	VecFx32 pos;
	MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
	MUS_ITEM_DRAW_WORK *holdDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );

	//���X�g��t���ւ��č��W���Đݒ�
	DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupField , work->holdItem );
	DUP_FIT_ITEMGROUP_AddItemTop( work->itemGroupAnime , work->holdItem );
	
	MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );
	pos.z = RETURN_LIST_ITEM_DEPTH;
	MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );

	MUS_ITEM_DRAW_SetSize( work->itemDrawSys , holdDrawWork ,FX16_ONE,FX16_ONE );

	DUP_FIT_ITEM_SetCount( work->holdItem , 0 );
	
#if DEB_ARI
	DUP_FIT_ITEM_DumpList( work->itemGroupField ,2 );
#endif
}

//--------------------------------------------------------------
//�A�C�e���𗣂� ������
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpDropItemToEquip(  FITTING_WORK *work )
{
	VecFx32 pos;
	fx32 depth = EQUIP_ITEM_DEPTH;
	FIT_ITEM_WORK *item;
	MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
	MUS_ITEM_DRAW_WORK *holdDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
	//���X�g��t���ւ��č��W���Đݒ�
	DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupField , work->holdItem );
	DUP_FIT_ITEMGROUP_AddItemTop( work->itemGroupEquip , work->holdItem );

	MUS_ITEM_DRAW_SetSize( work->itemDrawSys , holdDrawWork ,FX16_ONE,FX16_ONE );

	DUP_FIT_ITEM_SetCount( work->holdItem , work->snapPos );
	
	//�[�x�̍Đݒ�
	item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
	while( item != NULL )
	{
		VecFx32 pos;
		MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
		MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );
		pos.z = depth;
		MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );
		
		depth -= FX32_CONST(0.1f);
		item = DUP_FIT_ITEM_GetNextItem(item);
	}
	
#if DEB_ARI
	DUP_FIT_ITEM_DumpList( work->itemGroupField ,2 );
#endif
	
}

//--------------------------------------------------------------
//�w��ӏ��ɑ��������邩�H
//--------------------------------------------------------------
static const BOOL DUP_FIT_CheckIsEquipItem( FITTING_WORK *work , const MUS_POKE_EQUIP_POS pos)
{
	FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
	while( item != NULL )
	{
		if( pos == DUP_FIT_ITEM_GetCount( item ) )
		{
			return TRUE;
		}
		item = DUP_FIT_ITEM_GetNextItem(item);
	}
	return FALSE;
}

//--------------------------------------------------------------
//�A�C�e���̃A�j���[�V����(itemGroupAnime�ɓo�^����Ă��镨�̏���
//--------------------------------------------------------------
static void DUP_FIT_UpdateItemAnime( FITTING_WORK *work )
{
	FIT_ITEM_WORK *nextItem = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupAnime );
	while( nextItem != NULL )
	{
		FIT_ITEM_WORK *item = nextItem;
		MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
		u16 cnt = DUP_FIT_ITEM_GetCount( item );

		//remove����\��������̂ŁA��Ɏ����Ƃ��Ă���
		nextItem = DUP_FIT_ITEM_GetNextItem(item);

		cnt++;
		if( ITEM_RETURN_ANIME_CNT > cnt )
		{
			fx16 size = FX16_ONE * (ITEM_RETURN_ANIME_CNT-cnt) / ITEM_RETURN_ANIME_CNT;
			MUS_ITEM_DRAW_SetSize( work->itemDrawSys , drawWork , size , size );
			DUP_FIT_ITEM_SetCount( item , cnt );
		}
		else
		{
			DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupAnime , item );
			DUP_FIT_ITEM_DeleteItem( item , work->itemDrawSys );
		}
	}
}



//--------------------------------------------------------------
//�ȉ~�̓����蔻��
//--------------------------------------------------------------
static BOOL DUP_FIT_CheckPointInOval( s16 subX , s16 subY , s16 size , float ratioYX )
{
	const s16 newSubX = (s16)(subX*ratioYX);
	if( newSubX * newSubX + subY * subY < size*size )
		return TRUE;
	else
		return FALSE;
}
