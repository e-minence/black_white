//============================================================================================
/**
 * @file	samplemain.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "include/system/gfl_use.h"
#include "textprint.h"

void	SampleBoot( HEAPID heapID );
void	SampleEnd( void );
BOOL	SampleMain( void );

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	���C������
 *
 *
 *
 *
 *
 */
//============================================================================================
typedef struct _SAMPLE_SETUP SAMPLE_SETUP;

static SAMPLE_SETUP*	SetupGameSystem( HEAPID heapID );
static void				RemoveGameSystem( SAMPLE_SETUP* gs );
static void				MainGameSystem( SAMPLE_SETUP* gs );

//�A�[�J�C�u�h�m�c�d�w
enum {
	ARCID_TEST3D = 0,
	ARCID_FLDACT,
	ARCID_FLDMAP,
};

typedef struct _CURSOR_CONT	CURSOR_CONT;
static CURSOR_CONT*		CreateCursor( SAMPLE_SETUP*	gs, HEAPID heapID );
static void				DeleteCursor( CURSOR_CONT* cursor );
static void				MainCursor( CURSOR_CONT* cursor);
static void				GetCursorTrans( CURSOR_CONT* cursor, VecFx32* trans );

typedef struct _MAP_CONT	MAP_CONT;
static MAP_CONT*		CreateMapper( SAMPLE_SETUP*	gs, HEAPID heapID );
static void				DeleteMapper( MAP_CONT* mapper );
static void				MainMapper( MAP_CONT* mapper, VecFx32* pos );

//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
typedef struct {
	HEAPID			heapID;
	int				seq;
	int				timer;

	SAMPLE_SETUP*	gs;
	CURSOR_CONT*	cursor;
	MAP_CONT*		mapper;

}SAMPLE_WORK;

//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
static BOOL GameEndCheck( int cont );

SAMPLE_WORK* sampleWork;

//------------------------------------------------------------------
/**
 * @brief	���[�N�̊m�ۂƔj��
 */
//------------------------------------------------------------------
void	SampleBoot( HEAPID heapID )
{
	sampleWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(SAMPLE_WORK) );
	sampleWork->heapID = heapID;

	GFL_UI_TP_Init( sampleWork->heapID );
}

void	SampleEnd( void )
{
	GFL_UI_TP_Exit();

	GFL_HEAP_FreeMemory( sampleWork );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
static void Debug_Regenerate( void );
#define MAPOFFS 0//((64-3) * 8 * FX32_ONE)
//------------------------------------------------------------------
BOOL	SampleMain( void )
{
	BOOL return_flag = FALSE;
	int i;

	sampleWork->timer++;
	GFL_UI_TP_Main();

	switch( sampleWork->seq ){

	case 0:
		//�V�X�e���Z�b�g�A�b�v
		sampleWork->gs = SetupGameSystem( sampleWork->heapID );

		sampleWork->seq++;
		break;

	case 1:
		sampleWork->cursor = CreateCursor( sampleWork->gs, sampleWork->heapID );
		sampleWork->mapper = CreateMapper( sampleWork->gs, sampleWork->heapID );

		sampleWork->seq++;
		break;

	case 2:
		if( GameEndCheck( GFL_UI_KEY_GetCont() ) == TRUE ){
			sampleWork->seq++;
		}
		{
			VecFx32 pos;

			MainCursor( sampleWork->cursor );
			GetCursorTrans( sampleWork->cursor, &pos );
			MainMapper( sampleWork->mapper, &pos );
		}
		MainGameSystem( sampleWork->gs );
		break;

	case 3:
		DeleteMapper( sampleWork->mapper );
		DeleteCursor( sampleWork->cursor );

		sampleWork->seq++;
		break;

	case 4:
		RemoveGameSystem( sampleWork->gs );
		return_flag = TRUE;
		break;
	}
	return return_flag;
}

//------------------------------------------------------------------
/**
 * @brief	�I���`�F�b�N
 */
//------------------------------------------------------------------
static BOOL GameEndCheck( int cont )
{
	int resetCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START;

	if( (cont & resetCont ) == resetCont ){
		return TRUE;
	} else {
		return FALSE;
	}
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�Z�b�g�A�b�v
 *
 *
 *
 *
 *
 */
//============================================================================================
#define BACKGROUND_COL	(GX_RGB(31,31,31))		//�w�i�F
#define DTCM_SIZE		(0x1000)				//DTCM�G���A�̃T�C�Y

typedef struct _SAMPLE_MAP		SAMPLE_MAP;
static SAMPLE_MAP*	Create3Dmap( HEAPID heapID );
static void			Delete3Dmap( SAMPLE_MAP* sampleMap );
static void			Draw3Dmap( SAMPLE_MAP* sampleMap, GFL_G3D_CAMERA* g3Dcamera );
static void			Draw3Dmap2( SAMPLE_MAP* sampleMap, GFL_G3D_CAMERA* g3Dcamera );
static void			Load3Dmap( SAMPLE_MAP* sampleMap );
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
struct _SAMPLE_SETUP {
	GFL_G3D_UTIL*			g3Dutil;		//g3Dutil Lib �n���h��
	u16						g3DutilUnitIdx;	//g3Dutil Unit�C���f�b�N�X
	GFL_G3D_SCENE*			g3Dscene;		//g3Dscene Lib �n���h��
	GFL_G3D_CAMERA*			g3Dcamera;		//g3Dcamera Lib �n���h��
	GFL_G3D_LIGHTSET*		g3Dlightset;	//g3Dlight Lib �n���h��
	GFL_TCB*				g3dVintr;		//3D�pvIntrTask�n���h��

	SAMPLE_MAP*				sampleMap;

	HEAPID					heapID;
};

//------------------------------------------------------------------
/**
 * @brief	�f�B�X�v���C���f�[�^
 */
//------------------------------------------------------------------
///�u�q�`�l�o���N�ݒ�\����
static const GFL_BG_DISPVRAM dispVram = {
	GX_VRAM_BG_16_F,				//���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE,			//���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_128_C,			//�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE,		//�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E,				//���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE,		//���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_16_I,			//�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	//�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_01_AB,				//�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_0_G,			//�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
};

//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�e�[�u��
 */
//------------------------------------------------------------------
static	const	char	*GraphicFileTable[]={
	"test_graphic/test3d.narc",
	"test_graphic/fld_act.narc",
	"test_graphic/fld_map.narc",
};

static const char font_path[] = {"gfl_font.dat"};
//------------------------------------------------------------------
/**
 * @brief	�R�c�O���t�B�b�N���f�[�^
 */
//------------------------------------------------------------------
//�J���������ݒ�f�[�^
static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 cameraPos	= { 0, (FX32_ONE * 64), (FX32_ONE * 128) };

//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light0Tbl[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light0Setup = { light0Tbl, NELEMS(light0Tbl) };
//------------------------------------------------------------------
/**
 * @brief	�R�c���\�[�X�f�[�^
 */
//------------------------------------------------------------------
#define G3D_FRM_PRI			(1)			//�R�c�ʂ̕`��v���C�I���e�B�[
#define G3D_UTIL_RESCOUNT	(512)		//g3Dutil�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
#define G3D_UTIL_OBJCOUNT	(128)		//g3Dutil�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��
#define G3D_SCENE_OBJCOUNT	(256)		//g3Dscene�Ŏg�p����sceneObj�̍ő�ݒ�\��
#define G3D_OBJWORK_SZ		(64)		//g3Dscene�Ŏg�p����sceneObj�̃��[�N�T�C�Y
#define G3D_ACC_COUNT		(32)		//g3Dscene�Ŏg�p����sceneObjAccesary�̍ő�ݒ�\��

static const GXRgb edgeColorTable[8] = {
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
};
//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
//�a�f�ݒ�֐�
static void	bg_init( SAMPLE_SETUP* gs );
static void	bg_exit( SAMPLE_SETUP* gs );
//�R�c�֐�
static void g3d_load( SAMPLE_SETUP* gs );
static void g3d_control( SAMPLE_SETUP* gs );
static void g3d_draw( SAMPLE_SETUP* gs );
static void	g3d_unload( SAMPLE_SETUP* gs );
static void	g3d_vblank( GFL_TCB* tcb, void* work );

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
static SAMPLE_SETUP*	SetupGameSystem( HEAPID heapID )
{
	SAMPLE_SETUP*	gs = GFL_HEAP_AllocClearMemory( heapID, sizeof(SAMPLE_SETUP) );
	gs->heapID = heapID;

	//����������
	GFL_STD_MtRandInit(0);

	//ARC�V�X�e��������
	GFL_ARC_Init( &GraphicFileTable[0], NELEMS(GraphicFileTable) );

	//VRAM�N���A
	GFL_DISP_ClearVRAM( GX_VRAM_D );
	//VRAM�ݒ�
	GFL_DISP_SetBank( &dispVram );

	//BG������
	bg_init( gs );

	//�R�c�f�[�^�̃��[�h
	g3d_load( gs );
	gs->g3dVintr = GFUser_VIntr_CreateTCB( g3d_vblank, (void*)gs, 0 );

	return gs;
}

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
static void	RemoveGameSystem( SAMPLE_SETUP* gs )
{
	GFL_TCB_DeleteTask( gs->g3dVintr );
	g3d_unload( gs );	//�R�c�f�[�^�j��

	bg_exit( gs );
	GFL_ARC_Exit();

	GFL_HEAP_FreeMemory( gs );
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e�����C���֐�
 */
//------------------------------------------------------------------
static void	MainGameSystem( SAMPLE_SETUP* gs )
{
	g3d_control( gs );
	g3d_draw( gs );
}

//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ聕�f�[�^�]��
 */
//------------------------------------------------------------------
static void G3DsysSetup( void );
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	

static void	bg_init( SAMPLE_SETUP* gs )
{
	//�t�H���g�ǂݍ���
	GFL_TEXT_CreateSystem( font_path );

	//�a�f�V�X�e���N��
	GFL_BG_Init( gs->heapID );

	//�w�i�F�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocClearMemoryLo( gs->heapID, 16*2 );
		plt[0] = BACKGROUND_COL;
		GFL_BG_LoadPalette( GFL_BG_FRAME0_M, plt, 16*2, 0 );	//���C����ʂ̔w�i�F�]��
		GFL_BG_LoadPalette( GFL_BG_FRAME0_S, plt, 16*2, 0 );	//�T�u��ʂ̔w�i�F�]��
		GFL_HEAP_FreeMemory( plt );
	}
	//�a�f���[�h�ݒ�
	GFL_BG_SetBGMode( &bgsysHeader );

	//�a�f�R���g���[���ݒ�
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 8 );

	//�R�c�V�X�e���N��
	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, gs->heapID, G3DsysSetup );
	GFL_BG_SetBGControl3D( G3D_FRM_PRI );

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
	GFL_DISP_SetDispOn();
}

static void	bg_exit( SAMPLE_SETUP* gs )
{
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );

	GFL_G3D_Exit();
	GFL_BG_Exit();
	GFL_TEXT_DeleteSystem();
}

// �R�c�Z�b�g�A�b�v�R�[���o�b�N
static void G3DsysSetup( void )
{
	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_TOON );
	G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );
#if 1
	// �t�H�O�Z�b�g�A�b�v
    {
        u32     fog_table[8];
        int     i;

        G3X_SetFog(TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x1000, 0x01000 );

        G3X_SetFogColor(BACKGROUND_COL, 0);

        for ( i=0; i<8; i++ ){
            fog_table[i] = (u32)(((i*16)<<0) | ((i*16+4)<<8) | ((i*16+8)<<16) | ((i*16+12)<<24));
        }
        G3X_SetFogTable(&fog_table[0]);
	}

#else
	G3X_SetFog( FALSE, 0, 0, 0 );
#endif
	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	G3X_SetEdgeColorTable( edgeColorTable ); 
	G3X_EdgeMarking( TRUE );

	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_W );
}


//------------------------------------------------------------------
/**
 * @brief		�R�c�f�[�^���[�h
 */
//------------------------------------------------------------------
//�쐬
static void g3d_load( SAMPLE_SETUP* gs )
{
	//�z�u���ݒ�

	//g3Dutil���g�p���z��Ǘ�������
	gs->g3Dutil = GFL_G3D_UTIL_Create( G3D_UTIL_RESCOUNT, G3D_UTIL_OBJCOUNT, gs->heapID );
	//g3Dscene���g�p���Ǘ�������
	gs->g3Dscene = GFL_G3D_SCENE_Create( gs->g3Dutil, 
						G3D_SCENE_OBJCOUNT, G3D_OBJWORK_SZ, G3D_ACC_COUNT, TRUE, gs->heapID );

	//�J�����쐬
	gs->g3Dcamera = GFL_G3D_CAMERA_CreateDefault( &cameraPos, &cameraTarget, gs->heapID );
	{
		fx32 far = 1500 << FX32_SHIFT;
		//fx32 far = 1024 << FX32_SHIFT;

		GFL_G3D_CAMERA_SetFar( gs->g3Dcamera, &far );
	}
	//���C�g�쐬
	gs->g3Dlightset = GFL_G3D_LIGHT_Create( &light0Setup, gs->heapID );

	gs->sampleMap = Create3Dmap( gs->heapID );

	//�J�������C�g0���f
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset );
}
	
//����
static void g3d_control( SAMPLE_SETUP* gs )
{
	GFL_G3D_SCENE_Main( gs->g3Dscene ); 
	Load3Dmap( gs->sampleMap );
}

//�`��
static void g3d_draw( SAMPLE_SETUP* gs )
{
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset );
	Draw3Dmap2( gs->sampleMap, gs->g3Dcamera );
	GFL_G3D_SCENE_Draw( gs->g3Dscene );  
}

//�j��
static void g3d_unload( SAMPLE_SETUP* gs )
{
	GFL_G3D_LIGHT_Delete( gs->g3Dlightset );
	GFL_G3D_CAMERA_Delete( gs->g3Dcamera );

	Delete3Dmap( gs->sampleMap );

	GFL_G3D_SCENE_Delete( gs->g3Dscene );  
	GFL_G3D_UTIL_Delete( gs->g3Dutil );
}
	
static void	g3d_vblank( GFL_TCB* tcb, void* work )
{
	SAMPLE_SETUP* gs =  (SAMPLE_SETUP*)work;

	//Load3Dmap( gs->sampleMap );
}

//============================================================================================
/**
 * @brief	�}�b�v�`��V�X�e��
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
#define MAP_BLOCK_COUNT	(9)
#define MAPID_NULL	(0xffffffff)
#define	MAPDATA_SIZE	(0x18000) 
#define	MAPTEX_SIZE		(0x4000) 
#define	MAPTRANS_SIZE	(0x18000/16) 

#define	MAPSCALE_WIDTH		(512) 
#define MAP_WIDTH			(MAPSCALE_WIDTH*2)

typedef struct {
	u32				idx;
	VecFx32			trans;
	ARCHANDLE*		arc;
	BOOL			dataLoadReq;
	u32				dataLoadCount;
	u32				dataID;
	GFL_G3D_RES*	g3DresMdl;	//�n�`���f�����\�[�X
	u8				data[MAPDATA_SIZE];
	GFL_G3D_RES*	g3DresTex;	//�n�`�e�N�X�`�����\�[�X
	u8				tex[MAPTEX_SIZE];
	GFL_G3D_RND*	g3Drnd;		//�n�`�����_�[

	//���e�X�g�p
	u32				texDataAdrs;
	u32				texPlttAdrs;
}MAP_BLOCK_DATA;

struct _SAMPLE_MAP {
	GFL_G3D_SCENE*		g3Dscene;
	HEAPID				heapID;
	MAP_BLOCK_DATA		mapBlock[MAP_BLOCK_COUNT];
};

static void	CreateMapGraphicData( SAMPLE_MAP* sampleMap );
static void	DeleteMapGraphicData( SAMPLE_MAP* sampleMap );

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
#include "graphic_data/test_graphic/fld_act.naix"
#include "graphic_data/test_graphic/fld_map.naix"

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v����
 */
//------------------------------------------------------------------
static SAMPLE_MAP*	Create3Dmap( HEAPID heapID )
{
	SAMPLE_MAP* sampleMap = GFL_HEAP_AllocClearMemory( heapID, sizeof(SAMPLE_MAP) );

	sampleMap->heapID = heapID;

	CreateMapGraphicData( sampleMap );
	return sampleMap;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�j��
 */
//------------------------------------------------------------------
static void	Delete3Dmap( SAMPLE_MAP* sampleMap )
{
	DeleteMapGraphicData( sampleMap );

	GFL_HEAP_FreeMemory( sampleMap );
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�f�[�^�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static void	CreateMapGraphicData( SAMPLE_MAP* sampleMap )
{
	MAP_BLOCK_DATA*	mapBlock;
	int				i;
	u32				g3DresHeaderSize = GFL_G3D_GetResourceHeaderSize();

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &sampleMap->mapBlock[i];

		//������
		mapBlock->idx = MAPID_NULL;
		VEC_Set( &mapBlock->trans, 0, 0, 0 );
		mapBlock->dataLoadReq = FALSE;
		mapBlock->dataLoadCount = 0;
		mapBlock->dataID = 0;
		mapBlock->g3Drnd = NULL;
		//�A�[�J�C�u�n���h���쐬
		mapBlock->arc = GFL_ARC_OpenDataHandle( ARCID_FLDMAP, sampleMap->heapID );
		//���f�����\�[�X�w�b�_�쐬
		mapBlock->g3DresMdl = GFL_HEAP_AllocClearMemory( sampleMap->heapID, g3DresHeaderSize );
		//�e�N�X�`�����\�[�X�w�b�_�쐬
		mapBlock->g3DresTex = GFL_HEAP_AllocClearMemory( sampleMap->heapID, g3DresHeaderSize );

		//�T���v���p
		{
			GFL_G3D_RES*	g3DresTex;
			NNSG3dTexKey	texDataKey;
			NNSG3dPlttKey	texPlttKey;

			g3DresTex = (GFL_G3D_RES*)&mapBlock->tex[0];
			//GFL_G3D_LoadResourceArc( ARCID_FLDACT, NARC_fld_act_tex32x32_nsbtx, g3DresTex ); 
			GFL_G3D_LoadResourceArc( ARCID_FLDMAP, NARC_fld_map_map23_09c_nsbtx, g3DresTex ); 
			if( GFL_G3D_TransVramTexture( g3DresTex ) == FALSE ){ GF_ASSERT(0); }

			texDataKey = GFL_G3D_GetTextureDataVramKey( g3DresTex );
			texPlttKey = GFL_G3D_GetTexturePlttVramKey( g3DresTex );

			mapBlock->texDataAdrs = NNS_GfdGetTexKeyAddr( texDataKey );
			mapBlock->texPlttAdrs = NNS_GfdGetPlttKeyAddr( texPlttKey );
		}
#if 0
		OS_Printf("mapGraphic block%x is Created id=%x, tAdrs=%x, pAdrs=%x\n", 
					blockID, mapBlock->idx, mapBlock->texDataAdrs, mapBlock->texPlttAdrs );
#endif
	}
}

//------------------------------------------------------------------
static void	DeleteMapGraphicData( SAMPLE_MAP* sampleMap )
{
	MAP_BLOCK_DATA*	mapBlock;
	int				i;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &sampleMap->mapBlock[i];

		if( mapBlock->g3Drnd != NULL ){
			GFL_G3D_RENDER_Delete( mapBlock->g3Drnd );
		}
		GFL_HEAP_FreeMemory( mapBlock->g3DresTex );
		GFL_HEAP_FreeMemory( mapBlock->g3DresMdl );
		GFL_ARC_CloseDataHandle( mapBlock->arc );

		//�T���v���p
		{
			GFL_G3D_RES*	g3DresTex;

			g3DresTex = (GFL_G3D_RES*)&mapBlock->tex[0];
			if( GFL_G3D_FreeVramTexture( g3DresTex ) == FALSE ){ GF_ASSERT(0); }
		}
#if 0
	OS_Printf("mapGraphic block%x is Deleted id=%x, tAdrs=%x, pAdrs=%x\n", 
				blockID, mapBlock->idx, mapBlock->texDataAdrs, mapBlock->texPlttAdrs );
#endif
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���[�h
 */
static void	Make3Dmap( SAMPLE_MAP* sampleMap, const int blockID, const u32 mapID );
//------------------------------------------------------------------
static void	Load3Dmap( SAMPLE_MAP* sampleMap )
{
	MAP_BLOCK_DATA*	mapBlock;
	int				i;

	GF_ASSERT( sampleMap );

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &sampleMap->mapBlock[i];

		if( mapBlock->dataLoadReq == TRUE ){
			if( mapBlock->dataLoadCount == 0 ){
				//���[�h�J�n
				//OS_Printf(" map loaded start arcID = %x\n", mapBlock->arc );
#if 0
				GFL_ARC_LoadDataOfsByHandle( mapBlock->arc, mapBlock->dataID, 
												0, MAPTRANS_SIZE, (void*)mapBlock->data );
#else
				GFL_ARC_LoadDataOfsByHandle( mapBlock->arc, NARC_fld_map_map23_09c_nsbmd,
												0, MAPTRANS_SIZE, (void*)&mapBlock->data[0] );
#endif
				//OS_Printf(" map loaded block = %x, div = %x \n", i, mapBlock->dataLoadCount );
				mapBlock->dataLoadCount++;
			} else {
				u32	dataAdrs = mapBlock->dataLoadCount * MAPTRANS_SIZE;

				if( (dataAdrs + MAPTRANS_SIZE) < MAPDATA_SIZE){
					//���[�h�p��
					GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPTRANS_SIZE, 
														(void*)&mapBlock->data[dataAdrs] );
					//OS_Printf(" map loaded block = %x, div = %x \n", i, mapBlock->dataLoadCount );
					mapBlock->dataLoadCount++;
				} else {
					//�ŏI���[�h
					GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPDATA_SIZE - dataAdrs,
														(void*)&mapBlock->data[dataAdrs] );
					//OS_Printf(" map loaded block = %x, div = %x \n", i, mapBlock->dataLoadCount );

					//���f�����\�[�X�n���h�����e�쐬
					GFL_G3D_CreateResource( mapBlock->g3DresMdl, 
											(NNSG3dResFileHeader*)&mapBlock->data[0] );
					//�e�N�X�`�����\�[�X�n���h�����e�쐬
					GFL_G3D_CreateResource( mapBlock->g3DresTex, 
											(NNSG3dResFileHeader*)&mapBlock->tex[0] );
					OS_Printf(" map resTex block = %x, tex = %x \n", i, mapBlock->g3DresTex );
					//�����_�[�쐬
					if( mapBlock->g3Drnd != NULL ){
						GFL_G3D_RENDER_Delete( mapBlock->g3Drnd );
					}
					mapBlock->g3Drnd = GFL_G3D_RENDER_Create(	mapBlock->g3DresMdl, 0, 
																//mapBlock->g3DresTex );
																(GFL_G3D_RES*)&mapBlock->tex[0] );
					OS_Printf(" map render block = %x, rnd = %x \n", i, mapBlock->g3Drnd );

					//Make3Dmap( sampleMap, i, mapBlock->dataID );
					mapBlock->dataLoadReq = FALSE;
					mapBlock->dataLoadCount = 0;
				}
			}
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���[�h���N�G�X�g�ݒ�
 */
//------------------------------------------------------------------
static void	Set3DmapLoadReq
	( SAMPLE_MAP* sampleMap, const int blockID, const VecFx32* trans, const u32 dataID )
{
	MAP_BLOCK_DATA* mapBlock;

	GF_ASSERT( blockID < MAP_BLOCK_COUNT );

	mapBlock = &sampleMap->mapBlock[blockID];

	mapBlock->dataLoadReq = FALSE;	//���荞�ݏ����h�~�̂��߈�U��~

	mapBlock->dataLoadCount = 0;
	mapBlock->dataID = dataID;
	mapBlock->dataLoadReq = TRUE;

	mapBlock->idx = dataID;
	VEC_Set( &mapBlock->trans, trans->x, trans->y, trans->z );
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�f�B�X�v���C
 */
//------------------------------------------------------------------
static void	Draw3Dmap( SAMPLE_MAP* sampleMap, GFL_G3D_CAMERA* g3Dcamera )
{
	MAP_BLOCK_DATA* mapBlock;
	int				i;

	G3X_Reset();

	//�J�����ݒ�
	{
		VecFx32		camPos, camUp, target;

		GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
		GFL_G3D_CAMERA_GetCamUp( g3Dcamera, &camUp );
		GFL_G3D_CAMERA_GetTarget( g3Dcamera, &target );
		G3_LookAt( &camPos, &camUp, &target, NULL );
	}
	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(16, 16, 16), TRUE );
	G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE );
	G3_PolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, 
					GX_CULL_NONE, 63, 31, GX_POLYGON_ATTR_MISC_FOG);

	G3_Scale( FX32_ONE*MAPSCALE_WIDTH, FX32_ONE*MAPSCALE_WIDTH, FX32_ONE*MAPSCALE_WIDTH );


	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &sampleMap->mapBlock[i];
		//if(( mapBlock->idx != MAPID_NULL )&&( mapBlock->dataLoadReq == FALSE )){
		if( mapBlock->idx != MAPID_NULL ){
#if 0
#else
			int		x, z, d;
			fx16	vtx0x, vtx0z, len;

			G3_PushMtx();

			G3_Translate(	mapBlock->trans.x/MAPSCALE_WIDTH,
							mapBlock->trans.y/MAPSCALE_WIDTH,
							mapBlock->trans.z/MAPSCALE_WIDTH );

			G3_TexImageParam(	GX_TEXFMT_PLTT16, GX_TEXGEN_TEXCOORD, 
								GX_TEXSIZE_S32, GX_TEXSIZE_T32,
								GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
								GX_TEXPLTTCOLOR0_TRNS, mapBlock->texDataAdrs );
			G3_TexPlttBase( mapBlock->texPlttAdrs, GX_TEXFMT_PLTT16 );

			G3_Begin( GX_BEGIN_QUADS );

			d = 32;
			len = FX16_ONE*2/d;

			for( z=0; z<d; z++ ){
				for( x=0; x<d; x++ ){
					vtx0x = -FX16_ONE + x*len;
					vtx0z = -FX16_ONE + z*len;

					G3_TexCoord( 0, FX32_ONE*32 );
					G3_Vtx( vtx0x, 0, vtx0z + len );

					G3_TexCoord( FX32_ONE*32, FX32_ONE*32 );
					G3_Vtx( vtx0x + len, 0, vtx0z + len );

					G3_TexCoord( FX32_ONE*32, 0 );
					G3_Vtx( vtx0x + len, 0, vtx0z );

					G3_TexCoord( 0, 0 );
					G3_Vtx( vtx0x, 0, vtx0z );
				}
			}
			G3_End();
			G3_PopMtx(1);
#endif
		}
	}
}

static void	Draw3Dmap2( SAMPLE_MAP* sampleMap, GFL_G3D_CAMERA* g3Dcamera )
{
	MAP_BLOCK_DATA* mapBlock;
	BOOL			cullResult; 
	VecFx32			scale = { FX32_ONE*2, FX32_ONE, FX32_ONE*2 };
	MtxFx33			rotate = { FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE };
	int				i;

	//�`��J�n
	GFL_G3D_DRAW_Start();
	//�J�����O���[�o���X�e�[�g�ݒ�		
 	GFL_G3D_DRAW_SetLookAt();

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &sampleMap->mapBlock[i];
		if(		( mapBlock->idx != MAPID_NULL )
				&&( mapBlock->dataLoadReq == FALSE )
				&&( mapBlock->g3Drnd != NULL )){
			// �ʒu�ݒ�
			NNS_G3dGlbSetBaseTrans( &mapBlock->trans );
			// �p�x�ݒ�
			NNS_G3dGlbSetBaseRot( &rotate );
			// �X�P�[���ݒ�
			NNS_G3dGlbSetBaseScale( &scale );
			//�O���[�o���X�e�[�g���f
			NNS_G3dGlbFlush();

			//cullResult = GFL_G3D_DRAW_CheckCulling( g3Dobj );
			NNS_G3dDraw( GFL_G3D_RENDER_GetRenderObj(mapBlock->g3Drnd) );
		}
	}
	//�`��I���i�o�b�t�@�X���b�v�j
	//GFL_G3D_DRAW_End();							
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�f�[�^�쐬�i�T���v���p�j
 */
//------------------------------------------------------------------
static void	Make3Dmap( SAMPLE_MAP* sampleMap, const int blockID, const u32 mapID )
{
	GFL_G3D_RES*		g3DresTex;
	MAP_BLOCK_DATA*		mapBlock;

	GF_ASSERT( blockID < MAP_BLOCK_COUNT );

	mapBlock = &sampleMap->mapBlock[blockID];
	g3DresTex = (GFL_G3D_RES*)&mapBlock->tex[0];

	if( mapID != MAPID_NULL ){
		//�e�N�X�`���f�[�^�쐬
		NNSG3dResFileHeader*	file = GFL_G3D_GetResourceFileHeader( g3DresTex ); 
		NNSG3dResTex*			texfile = GFL_G3D_GetResTex( g3DresTex );
		u32						texData = (u32)((u8*)texfile + texfile->texInfo.ofsTex);
		GFL_BMP_DATA*			bmpTex = GFL_BMP_Create( 4,4,GFL_BMP_16_COLOR,sampleMap->heapID );
		GFL_TEXT_PRINTPARAM		printParam = { NULL, 0, 0, 1, 0, 15, 1, GFL_TEXT_WRITE_16 };
		char					mapName[] = {"mapID"};
		char					texName[] = {"0000"};
		u32						texSize = GFL_BMP_GetBmpDataSize( bmpTex );
		u32						colorB = mapID%7+1;

		//�}�b�v�h�c�𕶎��\������
		GFL_BMP_Clear( bmpTex, colorB );
		printParam.bmp = bmpTex;
		printParam.colorB = colorB;
		texName[0] = '0'+ mapID/1000;
		texName[1] = '0'+ mapID%1000/100;
		texName[2] = '0'+ mapID%100/10;
		texName[3] = '0'+ mapID%10;
	
		printParam.writex = 4;
		printParam.writey = 8;
		printParam.colorF = 15;
		GFL_TEXT_PrintSjisCode( mapName, &printParam );
		printParam.writex = 4;
		printParam.writey = 16;
		printParam.colorF = 14;
		GFL_TEXT_PrintSjisCode( texName, &printParam );
	
		GFL_BMP_DataConv_to_BMPformat( bmpTex, FALSE, sampleMap->heapID );
		GFL_STD_MemCopy32( GFL_BMP_GetCharacterAdrs( bmpTex ), (void*)texData, texSize ); 

		NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, 
										mapBlock->texDataAdrs, (void*)texData, texSize );
		GFL_BMP_Delete( bmpTex );
	}
	//OS_Printf("mapGraphic block%x is maked id=%x, tAdrs=%x, pAdrs=%x\n", 
	//			blockID, mapBlock->idx, mapBlock->texDataAdrs, mapBlock->texPlttAdrs );
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�R���g���[��
 *
 *
 *
 *
 *
 */
//============================================================================================
//============================================================================================
/**
 * @brief	�J�[�\��
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
struct _CURSOR_CONT {
	HEAPID				heapID;
	SAMPLE_SETUP*		gs;
	u16					unitIdx;
	u16					resIdx;
	u16					objIdx;

	u16					cursorIdx;
	VecFx32				trans;
	fx32				cameraHeight;
	u16					direction;
};

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
#include "graphic_data/test_graphic/test3d.naix"

enum {
	G3DRES_CURSOR = 0,
};
//�R�c�O���t�B�b�N���\�[�X�e�[�u��
static const GFL_G3D_UTIL_RES cursor_g3Dutil_resTbl[] = {
	{ ARCID_TEST3D, NARC_haruka_test_wall_nsbmd, GFL_G3D_UTIL_RESARC },
};

enum {
	G3DOBJ_CURSOR = 0,
};
//�R�c�I�u�W�F�N�g��`�e�[�u��
static const GFL_G3D_UTIL_OBJ cursor_g3Dutil_objTbl[] = {
	{ G3DRES_CURSOR, 0, G3DRES_CURSOR, NULL, 0 },
};

//---------------------
//g3Dscene �����ݒ�e�[�u���f�[�^
static const GFL_G3D_UTIL_SETUP cursor_g3Dsetup = {
	cursor_g3Dutil_resTbl, NELEMS(cursor_g3Dutil_resTbl),
	cursor_g3Dutil_objTbl, NELEMS(cursor_g3Dutil_objTbl),
};

static const GFL_G3D_SCENEOBJ_DATA cursorObject[] = {
	{	G3DOBJ_CURSOR, 0, 1, 16, TRUE, TRUE,
		{	{ 0, -FX32_ONE*64, 0 },
			{ FX32_ONE/4, FX32_ONE/4, FX32_ONE/4 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
};

//------------------------------------------------------------------
/**
 * @brief	������
 */
//------------------------------------------------------------------
static CURSOR_CONT*	CreateCursor( SAMPLE_SETUP*	gs, HEAPID heapID )
{
	CURSOR_CONT* cursor = GFL_HEAP_AllocClearMemory( heapID, sizeof(CURSOR_CONT) );

	cursor->heapID = heapID;
	cursor->gs = gs;
	cursor->trans.x = FX32_ONE*1024;//0;
	cursor->trans.y = FX32_ONE*1024;//0;
	cursor->trans.z = FX32_ONE*1024;//0;
	cursor->cameraHeight = 0;
	cursor->direction = 0;

	//�R�c�f�[�^�Z�b�g�A�b�v
	cursor->unitIdx = GFL_G3D_SCENE_AddG3DutilUnit( cursor->gs->g3Dscene, &cursor_g3Dsetup );
	cursor->resIdx = GFL_G3D_SCENE_GetG3DutilUnitResIdx( cursor->gs->g3Dscene, cursor->unitIdx );
	cursor->objIdx = GFL_G3D_SCENE_GetG3DutilUnitObjIdx( cursor->gs->g3Dscene, cursor->unitIdx );
	cursor->cursorIdx = GFL_G3D_SCENEOBJ_Add(	cursor->gs->g3Dscene, 
												cursorObject, NELEMS(cursorObject),
												cursor->objIdx );
	
	GFL_G3D_SCENEOBJ_SetPos(	GFL_G3D_SCENEOBJ_Get(cursor->gs->g3Dscene, cursor->cursorIdx),
								&cursor->trans );
	return cursor;
}

//------------------------------------------------------------------
/**
 * @brief	�I��
 */
//------------------------------------------------------------------
static void	DeleteCursor( CURSOR_CONT* cursor )
{
	GFL_G3D_SCENEOBJ_Remove( cursor->gs->g3Dscene, cursor->cursorIdx, NELEMS(cursorObject) ); 
	GFL_G3D_SCENE_DelG3DutilUnit( cursor->gs->g3Dscene, cursor->unitIdx );

	GFL_HEAP_FreeMemory( cursor );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
//------------------------------------------------------------------
#define MV_SPEED		(8)
#define RT_SPEED		(FX32_ONE/4)
#define	CAMERA_LENGTH	(128)
#define	CAMERA_TARGET_HEIGHT	(128)
static void	MainCursor( CURSOR_CONT* cursor )
{
	VecFx32	pos, target;
	VecFx32	vecMove = { 0, 0, 0 };
	int		key;

	key = GFL_UI_KEY_GetCont();

	if( key & PAD_BUTTON_B ){
		vecMove.y = -MV_SPEED * FX32_ONE;
	}
	if( key & PAD_BUTTON_X ){
		vecMove.y = MV_SPEED * FX32_ONE;
	}
	if( key & PAD_KEY_UP ){
		vecMove.x = MV_SPEED * FX_SinIdx( (u16)(cursor->direction + 0x8000) );
		vecMove.z = MV_SPEED * FX_CosIdx( (u16)(cursor->direction + 0x8000) );
	}
	if( key & PAD_KEY_DOWN ){
		vecMove.x = MV_SPEED * FX_SinIdx( (u16)(cursor->direction + 0x0000) );
		vecMove.z = MV_SPEED * FX_CosIdx( (u16)(cursor->direction + 0x0000) );
	}
	if( key & PAD_KEY_LEFT ){
		vecMove.x = MV_SPEED * FX_SinIdx( (u16)(cursor->direction + 0xc000) );
		vecMove.z = MV_SPEED * FX_CosIdx( (u16)(cursor->direction + 0xc000) );
	}
	if( key & PAD_KEY_RIGHT ){
		vecMove.x = MV_SPEED * FX_SinIdx( (u16)(cursor->direction + 0x4000) );
		vecMove.z = MV_SPEED * FX_CosIdx( (u16)(cursor->direction + 0x4000) );
	}
	if( key & PAD_BUTTON_R ){
		cursor->direction -= RT_SPEED;
	}
	if( key & PAD_BUTTON_L ){
		cursor->direction += RT_SPEED;
	}
	VEC_Add( &cursor->trans, &vecMove, &cursor->trans );
	cursor->trans.y = 0;
	cursor->cameraHeight += vecMove.y;

	VEC_Set( &target, cursor->trans.x, CAMERA_TARGET_HEIGHT*FX32_ONE, cursor->trans.z );
	pos.x = cursor->trans.x + CAMERA_LENGTH * FX_SinIdx(cursor->direction);
	pos.y = cursor->cameraHeight;
	pos.z = cursor->trans.z + CAMERA_LENGTH * FX_CosIdx(cursor->direction);

	GFL_G3D_SCENEOBJ_SetPos(	GFL_G3D_SCENEOBJ_Get(cursor->gs->g3Dscene, cursor->cursorIdx), 
								&cursor->trans );
	GFL_G3D_CAMERA_SetTarget( cursor->gs->g3Dcamera, &target );
	GFL_G3D_CAMERA_SetPos( cursor->gs->g3Dcamera, &pos );

	//OS_Printf(" pos {%x,%x,%x}\n", cursor->trans.x, cursor->trans.y, cursor->trans.z );
}

//------------------------------------------------------------------
/**
 * @brief	
 */
//------------------------------------------------------------------
static void	GetCursorTrans( CURSOR_CONT* cursor, VecFx32* trans )
{
	*trans = cursor->trans;
}


//============================================================================================
/**
 * @brief	�}�b�v
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
#define MAP_BLOCK_SET_COUNT	(9)//(4)

typedef struct {
	u32			mapID;
	VecFx32		trans;
}MAP_BLOCK_IDX;

struct _MAP_CONT {
	HEAPID			heapID;
	SAMPLE_SETUP*	gs;
	MAP_BLOCK_IDX	mapBlockIdx[MAP_BLOCK_COUNT];
};

#define MAP_BLOCK_LENX		(FX32_ONE * MAP_WIDTH)
#define MAP_BLOCK_LENZ		(FX32_ONE * MAP_WIDTH)
#define MAP_BLOCK_SIZX		(32)
#define MAP_BLOCK_SIZZ		(32)
#define MAP_BLOCK_IDXMAX	(MAP_BLOCK_SIZX * MAP_BLOCK_SIZZ)
#define MAP3D_GRID_SIZEX	(32)
#define MAP3D_GRID_SIZEZ	(32)
#define MAP3D_GRID_LENX		(MAP_BLOCK_LENX / MAP3D_GRID_SIZEX)
#define MAP3D_GRID_LENZ		(MAP_BLOCK_LENZ / MAP3D_GRID_SIZEZ)

static void GetMapperBlockIndex
		( const VecFx32* pos, MAP_BLOCK_IDX* mapBlockIdx, GFL_G3D_CAMERA* g3Dcamera );
static BOOL	ReloadMapperBlock( MAP_CONT* mapper, MAP_BLOCK_IDX* new );
static BOOL	CheckMapperBlockReload( MAP_BLOCK_IDX* resist, MAP_BLOCK_IDX* now, MAP_BLOCK_IDX* new );
//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
 * @brief	������
 */
//------------------------------------------------------------------
static MAP_CONT*	CreateMapper( SAMPLE_SETUP*	gs, HEAPID heapID )
{
	MAP_CONT* mapper = GFL_HEAP_AllocClearMemory( heapID, sizeof(MAP_CONT) );
	int i;

	mapper->heapID = heapID;
	mapper->gs = gs;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapper->mapBlockIdx[i].mapID = MAPID_NULL;
	}
	return mapper;
}

//------------------------------------------------------------------
/**
 * @brief	�I��
 */
//------------------------------------------------------------------
static void	DeleteMapper( MAP_CONT* mapper )
{
	GFL_HEAP_FreeMemory( mapper );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
//------------------------------------------------------------------
static void	MainMapper( MAP_CONT* mapper, VecFx32* pos )
{
	MAP_BLOCK_IDX nowBlockIdx[MAP_BLOCK_SET_COUNT];

 	GetMapperBlockIndex( pos, &nowBlockIdx[0], mapper->gs->g3Dcamera );
	if( ReloadMapperBlock( mapper, &nowBlockIdx[0] ) == TRUE ){
		//OS_Printf("map reloaded\n");
	}
}


//------------------------------------------------------------------
/**
 * @brief	�}�b�v�u���b�N�擾
 */
//------------------------------------------------------------------
typedef struct {
	s16	z;
	s16	x;
}BLOCK_OFFS;

static const BLOCK_OFFS blockPat_U[] = {
	{-1,-1},{-1, 0},{-1, 1},{ 0, 0},	//�����̂���u���b�N����O�����ɂS�u���b�N�Ƃ�
};
static const BLOCK_OFFS blockPat_UL[] = {
	{-1,-1},{-1, 0},{ 0,-1},{ 0, 0},	//�����̂���u���b�N���獶�O�����ɂS�u���b�N�Ƃ�
};
static const BLOCK_OFFS blockPat_L[] = {
	{-1,-1},{ 0,-1},{ 0, 0},{ 1,-1},	//�����̂���u���b�N���獶�����ɂS�u���b�N�Ƃ�
};
static const BLOCK_OFFS blockPat_DL[] = {
	{ 0,-1},{ 0, 0},{ 1,-1},{ 1, 0},	//�����̂���u���b�N���獶������ɂS�u���b�N�Ƃ�
};
static const BLOCK_OFFS blockPat_D[] = {
	{ 0, 0},{ 1,-1},{ 1, 0},{ 1, 1},	//�����̂���u���b�N���������ɂS�u���b�N�Ƃ�
};
static const BLOCK_OFFS blockPat_DR[] = {
	{ 0, 0},{ 0, 1},{ 1, 0},{ 1, 1},	//�����̂���u���b�N����E������ɂS�u���b�N�Ƃ�
};
static const BLOCK_OFFS blockPat_R[] = {
	{-1, 1},{ 0, 0},{ 0, 1},{ 1, 1},	//�����̂���u���b�N����E�����ɂS�u���b�N�Ƃ�
};
static const BLOCK_OFFS blockPat_UR[] = {
	{-1, 0},{-1, 1},{ 0, 0},{ 0, 1},	//�����̂���u���b�N����E�O�����ɂS�u���b�N�Ƃ�
};

static const BLOCK_OFFS blockPat_Around[] = {
	{-1,-1},{-1, 0},{-1, 1},{ 0,-1},{ 0, 0},{ 0, 1},{ 1,-1},{ 1, 0},{ 1, 1},
};

static const BLOCK_OFFS* blockIdxOffs_FL[] = {
	blockPat_UL, blockPat_UL, blockPat_UL, blockPat_UL, 
	blockPat_UL, blockPat_UL, blockPat_UL, blockPat_UL, 
	blockPat_UL, blockPat_UL, blockPat_UL, blockPat_UL, 
	blockPat_UL, blockPat_UL, blockPat_UL, blockPat_UL, 
};
static const BLOCK_OFFS* blockIdxOffs_FR[] = {
	blockPat_UR, blockPat_UR, blockPat_UR, blockPat_UR, 
	blockPat_UR, blockPat_UR, blockPat_UR, blockPat_UR, 
	blockPat_UR, blockPat_UR, blockPat_UR, blockPat_UR, 
	blockPat_UR, blockPat_UR, blockPat_UR, blockPat_UR, 
};
static const BLOCK_OFFS* blockIdxOffs_BL[] = {
	blockPat_DL, blockPat_DL, blockPat_DL, blockPat_DL, 
	blockPat_DL, blockPat_DL, blockPat_DL, blockPat_DL, 
	blockPat_DL, blockPat_DL, blockPat_DL, blockPat_DL, 
	blockPat_DL, blockPat_DL, blockPat_DL, blockPat_DL, 
};
static const BLOCK_OFFS* blockIdxOffs_BR[] = {
	blockPat_DR, blockPat_DR, blockPat_DR, blockPat_DR, 
	blockPat_DR, blockPat_DR, blockPat_DR, blockPat_DR, 
	blockPat_DR, blockPat_DR, blockPat_DR, blockPat_DR, 
	blockPat_DR, blockPat_DR, blockPat_DR, blockPat_DR, 
};

static const BLOCK_OFFS* blockIdxOffs_All[] = {
	blockPat_Around, blockPat_Around, blockPat_Around, blockPat_Around, 
	blockPat_Around, blockPat_Around, blockPat_Around, blockPat_Around, 
	blockPat_Around, blockPat_Around, blockPat_Around, blockPat_Around, 
	blockPat_Around, blockPat_Around, blockPat_Around, blockPat_Around, 
};

static const BLOCK_OFFS** blockIdxOffs[] = {
//	blockIdxOffs_FL, blockIdxOffs_FR, blockIdxOffs_BL, blockIdxOffs_BR,
	blockIdxOffs_All, blockIdxOffs_All, blockIdxOffs_All, blockIdxOffs_All,
};

static void GetMapperBlockIndex
		( const VecFx32* pos, MAP_BLOCK_IDX* mapBlockIdx, GFL_G3D_CAMERA* g3Dcamera )
{
	u32		blockIdx, blockx, blockz;
	fx32	posx, posz, scalar;
	int		i, offsx, offsz;
	u16		direction;
	u8		blockPat, dirPat;
	VecFx32	center;
	const BLOCK_OFFS** idxTbl;
	
	//���݃u���b�N��XZindex�擾
	blockx = FX_Whole( FX_Div( pos->x, MAP_BLOCK_LENX ) );
	blockz = FX_Whole( FX_Div( pos->z, MAP_BLOCK_LENZ ) );
	posx = FX_Mod( pos->x, MAP_BLOCK_LENX );
	posz = FX_Mod( pos->z, MAP_BLOCK_LENZ );

	//���݃J���������ɂ��p�^�[���擾
	direction = GFL_G3D_CAMERA_GetRadianXZ( g3Dcamera );
	dirPat = (direction >> 12)&0x000f;

	//���݃u���b�N�̒��S���W�Z�o
	VEC_Set( &center,	(blockx * MAP_WIDTH + MAP_WIDTH/2) * FX32_ONE, 
						0, 
						(blockz * MAP_WIDTH + MAP_WIDTH/2) * FX32_ONE );
	//scalar = GFL_G3D_CAMERA_GetDotProduct( g3Dcamera, &center );

	//���݃u���b�N���ʒu�ɂ��p�^�[���e�[�u���擾
	blockPat = 0;
	if( posx > (MAP_BLOCK_LENX/2) ){
		blockPat += 1;
	}
	if( posz > (MAP_BLOCK_LENZ/2) ){
		blockPat += 2;
	}
	idxTbl = blockIdxOffs[blockPat];

	for( i=0; i<MAP_BLOCK_SET_COUNT; i++ ){
		offsx = blockx + idxTbl[dirPat][i].x;
		offsz = blockz + idxTbl[dirPat][i].z;

		if((offsx < 0)||(offsx >= MAP_BLOCK_SIZX)||(offsz < 0)||(offsz >= MAP_BLOCK_SIZZ)){
			blockIdx = MAPID_NULL;
		} else {
			blockIdx = offsz * MAP_BLOCK_SIZX + offsx;

			if( blockIdx >= MAP_BLOCK_IDXMAX ){
				blockIdx = MAPID_NULL;
			}
		}
		mapBlockIdx[i].mapID = blockIdx;
		mapBlockIdx[i].trans.x = (offsx * MAP_WIDTH + MAP_WIDTH/2) * FX32_ONE;
		mapBlockIdx[i].trans.y = 0;
		mapBlockIdx[i].trans.z = (offsz * MAP_WIDTH + MAP_WIDTH/2) * FX32_ONE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�u���b�N�X�V�`�F�b�N
 */
//------------------------------------------------------------------
static BOOL	ReloadMapperBlock( MAP_CONT* mapper, MAP_BLOCK_IDX* new )
{
	BOOL addFlag, delFlag, delProcFlag, addProcFlag, reloadFlag;
	int i, j, c;

	reloadFlag = FALSE;

	//�X�V�u���b�N�`�F�b�N
	delProcFlag = FALSE;
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		if( mapper->mapBlockIdx[i].mapID != MAPID_NULL ){
			delFlag = FALSE;
			for( j=0; j<MAP_BLOCK_SET_COUNT; j++ ){
				if(( mapper->mapBlockIdx[i].mapID == new[j].mapID )&&(delFlag == FALSE )){
					new[j].mapID = MAPID_NULL;
					delFlag = TRUE;
				}
			}
			if( delFlag == FALSE ){
				mapper->mapBlockIdx[i].mapID = MAPID_NULL;
				delProcFlag = TRUE;
			}
		}
	}
	//�X�V
	addProcFlag = FALSE;
	c = 0;
	for( i=0; i<MAP_BLOCK_SET_COUNT; i++ ){
		if( new[i].mapID != MAPID_NULL ){
			addFlag = FALSE;
			for( j=0; j<MAP_BLOCK_COUNT; j++ ){
				if(( mapper->mapBlockIdx[j].mapID == MAPID_NULL )&&(addFlag == FALSE )){
					Set3DmapLoadReq( mapper->gs->sampleMap, j, &new[i].trans, new[i].mapID );
					mapper->mapBlockIdx[j] = new[i];
					addFlag = TRUE;
					addProcFlag = TRUE;
				}
			}
			if( addFlag == FALSE ){
				GF_ASSERT(0);
			}
		}
	}
	if( addProcFlag == TRUE ){
		reloadFlag = TRUE;
	}
	return reloadFlag;
}

