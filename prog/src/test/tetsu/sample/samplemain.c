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
#define BACKGROUND_COL	(GX_RGB(20,20,31))		//�w�i�F
#define DTCM_SIZE		(0x1000)				//DTCM�G���A�̃T�C�Y

typedef struct _SAMPLE_MAP		SAMPLE_MAP;
static SAMPLE_MAP*	Create3Dmap( HEAPID heapID );
static void			Delete3Dmap( SAMPLE_MAP* sampleMap );
static void			Draw3Dmap( SAMPLE_MAP* sampleMap, GFL_G3D_CAMERA* g3Dcamera );
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
};

static const char font_path[] = {"gfl_font.dat"};
//------------------------------------------------------------------
/**
 * @brief	�R�c�O���t�B�b�N���f�[�^
 */
//------------------------------------------------------------------
//�J���������ݒ�f�[�^
static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 cameraPos	= { 0, (FX32_ONE * 100), (FX32_ONE * 180) };

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

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
static SAMPLE_SETUP*	SetupGameSystem( HEAPID heapID )
{
	SAMPLE_SETUP*	gs = GFL_HEAP_AllocMemory( heapID, sizeof(SAMPLE_SETUP) );
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

	return gs;
}

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
static void	RemoveGameSystem( SAMPLE_SETUP* gs )
{
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
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

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
		fx32 far = 4096 << FX32_SHIFT;

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
}

//�`��
static void g3d_draw( SAMPLE_SETUP* gs )
{
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset );
	Draw3Dmap( gs->sampleMap, gs->g3Dcamera );
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
#define MAP_BLOCK_COUNT	(4)
#define MAPID_NULL	(0xffffffff)

typedef struct {
	u32					idx;
	VecFx32				trans;
	u32					texDataAdrs;
	u32					texPlttAdrs;
#if 0
	u8					g3DresTex[0x1000];
#else
	GFL_G3D_RES*		g3Dres;
#endif
}MAP_BLOCK_DATA;

struct _SAMPLE_MAP {
	GFL_G3D_SCENE*		g3Dscene;
	HEAPID				heapID;
	MAP_BLOCK_DATA		mapBlock[MAP_BLOCK_COUNT];
};

static void	CreateMapGraphicData
		( SAMPLE_MAP* sampleMap, const int blockID, const VecFx32* trans, const u32 mapID );
static void	DeleteMapGraphicData( SAMPLE_MAP* sampleMap, int blockID );

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
#include "graphic_data/test_graphic/fld_act.naix"

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v����
 */
//------------------------------------------------------------------
static SAMPLE_MAP*	Create3Dmap( HEAPID heapID )
{
	SAMPLE_MAP* sampleMap = GFL_HEAP_AllocClearMemory( heapID, sizeof(SAMPLE_MAP) );
	int i;

	sampleMap->heapID = heapID;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		sampleMap->mapBlock[i].idx = MAPID_NULL;
	}
	return sampleMap;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�j��
 */
//------------------------------------------------------------------
static void	Delete3Dmap( SAMPLE_MAP* sampleMap )
{
	GFL_HEAP_FreeMemory( sampleMap );
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�}�b�v�f�[�^�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static void	CreateMapGraphicData
		( SAMPLE_MAP* sampleMap, const int blockID, const VecFx32* trans, const u32 mapID )
{
	GFL_G3D_SCENEOBJ_DATA*	pdata;
	GFL_G3D_RES*			g3DresTex;
	NNSG3dTexKey			texDataKey;
	NNSG3dPlttKey			texPlttKey;
	MAP_BLOCK_DATA*			mapBlock;

	GF_ASSERT( blockID < MAP_BLOCK_COUNT );
	if( mapID == MAPID_NULL ){
		return;
	}
	mapBlock = &sampleMap->mapBlock[blockID];

	GF_ASSERT( mapBlock->idx == MAPID_NULL );

	mapBlock->idx = mapID;
	VEC_Set( &mapBlock->trans, trans->x, trans->y, trans->z );
#if 0
	g3DresTex = (GFL_G3D_RES*)&mapBlock->g3DresTex[0];
	GFL_G3D_LoadResourceArc( ARCID_FLDACT, NARC_fld_act_tex32x32_nsbtx, g3DresTex ); 
#else
	g3DresTex = GFL_G3D_CreateResourceArc( ARCID_FLDACT, NARC_fld_act_tex32x32_nsbtx ); 
	mapBlock->g3Dres = g3DresTex;
#endif
	if( GFL_G3D_TransVramTexture( g3DresTex ) == FALSE ){
		GF_ASSERT(0);
	}
	texDataKey = GFL_G3D_GetTextureDataVramKey( g3DresTex );
	texPlttKey = GFL_G3D_GetTexturePlttVramKey( g3DresTex );

	mapBlock->texDataAdrs = NNS_GfdGetTexKeyAddr( texDataKey );
	mapBlock->texPlttAdrs = NNS_GfdGetPlttKeyAddr( texPlttKey );
	{
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
	OS_Printf("mapGraphic block%x is Created id=%x, tKey=%x, pKey=%x, tAdrs=%x, pAdrs=%x\n", 
				blockID, mapBlock->idx, texDataKey, texPlttKey,
				mapBlock->texDataAdrs, mapBlock->texPlttAdrs );
}

//------------------------------------------------------------------
static void	DeleteMapGraphicData( SAMPLE_MAP* sampleMap, int blockID )
{
	GFL_G3D_RES*			g3DresTex;
	MAP_BLOCK_DATA*			mapBlock;

	GF_ASSERT( blockID < MAP_BLOCK_COUNT );

	mapBlock = &sampleMap->mapBlock[blockID];

	if( mapBlock->idx == MAPID_NULL ){
		return;
	}
#if 0
	g3DresTex = (GFL_G3D_RES*)&mapBlock->g3DresTex[0];
	if( GFL_G3D_FreeVramTexture( g3DresTex ) == FALSE ){
		GF_ASSERT(0);
	}
#else
	
	g3DresTex = mapBlock->g3Dres;
	if( GFL_G3D_FreeVramTexture( g3DresTex ) == FALSE ){
		GF_ASSERT(0);
	}
	GFL_G3D_DeleteResource( g3DresTex );
#endif
	mapBlock->idx = MAPID_NULL;
	OS_Printf("mapGraphic block%x is Deleted\n", blockID);
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
	G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 63, 31, 0);

	G3_Scale( FX32_ONE*256, FX32_ONE*256, FX32_ONE*256 );

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &sampleMap->mapBlock[i];
		if( mapBlock->idx != MAPID_NULL ){
			G3_PushMtx();
	
			G3_Translate( mapBlock->trans.x/256, mapBlock->trans.y/256, mapBlock->trans.z/256 );

			G3_TexImageParam(	GX_TEXFMT_PLTT16, GX_TEXGEN_TEXCOORD, 
								GX_TEXSIZE_S32, GX_TEXSIZE_T32,
								GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
								GX_TEXPLTTCOLOR0_TRNS, mapBlock->texDataAdrs );
			G3_TexPlttBase( mapBlock->texPlttAdrs, GX_TEXFMT_PLTT16 );

			G3_Begin( GX_BEGIN_QUADS );

			G3_TexCoord( 0, FX32_ONE*32 );
			G3_Vtx( -FX16_ONE, 0, FX16_ONE );

			G3_TexCoord( FX32_ONE*32, FX32_ONE*32 );
			G3_Vtx( FX16_ONE, 0, FX16_ONE );

			G3_TexCoord( FX32_ONE*32, 0 );
			G3_Vtx( FX16_ONE, 0, -FX16_ONE );

			G3_TexCoord( 0, 0 );
			G3_Vtx( -FX16_ONE, 0, -FX16_ONE );
	
			G3_End();
			G3_PopMtx(1);
		}
	}
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
#define MV_SPEED	(FX32_ONE*4)
static void	MainCursor( CURSOR_CONT* cursor )
{
	VecFx32	pos, target;
	VecFx32	vecMove = { 0, 0, 0 };
	int		key;

	key = GFL_UI_KEY_GetCont();

	if( key & PAD_BUTTON_A ){
		vecMove.y = -MV_SPEED;
	}
	if( key & PAD_BUTTON_B ){
		vecMove.y = MV_SPEED;
	}
	if( key & PAD_KEY_UP ){
		vecMove.z = -MV_SPEED;
	}
	if( key & PAD_KEY_DOWN ){
		vecMove.z = MV_SPEED;
	}
	if( key & PAD_KEY_LEFT ){
		vecMove.x = -MV_SPEED;
	}
	if( key & PAD_KEY_RIGHT ){
		vecMove.x = MV_SPEED;
	}
	VEC_Add( &cursor->trans, &vecMove, &cursor->trans );
	VEC_Add( &cursor->trans, &cameraPos, &pos );
	target.x = cursor->trans.x;
	target.y = 0;
	target.z = cursor->trans.z;

	GFL_G3D_SCENEOBJ_SetPos(	GFL_G3D_SCENEOBJ_Get(cursor->gs->g3Dscene, cursor->cursorIdx), 
								&target );
	GFL_G3D_CAMERA_SetTarget( cursor->gs->g3Dcamera, &target );
	GFL_G3D_CAMERA_SetPos( cursor->gs->g3Dcamera, &pos );
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
typedef struct {
	u32			mapID;
	VecFx32		trans;
}MAP_BLOCK_IDX;

struct _MAP_CONT {
	HEAPID			heapID;
	SAMPLE_SETUP*	gs;
	MAP_BLOCK_IDX	mapBlockIdx[MAP_BLOCK_COUNT];
};

#define MAP3D_SCALE			(FX32_ONE*1)
#define MAP3D_GRID_LEN		(MAP3D_SCALE*16)
#define MAP3D_GRID_SIZEX	(32)
#define MAP3D_GRID_SIZEZ	(32)
#define MAP_BLOCK_LENX		(MAP3D_GRID_LEN * MAP3D_GRID_SIZEX)
#define MAP_BLOCK_LENZ		(MAP3D_GRID_LEN * MAP3D_GRID_SIZEZ)
#define MAP_BLOCK_SIZX		(32)
#define MAP_BLOCK_SIZZ		(32)
#define MAP_BLOCK_IDXMAX	(MAP_BLOCK_SIZX * MAP_BLOCK_SIZZ)

static void  GetMapperBlockIndex( const VecFx32* pos, MAP_BLOCK_IDX* mapBlockIdx );
//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static const u32 mapIdxData[] = {
	  9, 10, 11, 12, 13, 14, 15,
	 31, 32, 33, 34, 35, 36, 37,
	 66, 67, 68, 69, 70, 71, 72,
	 22, 23, 24, 25, 26, 27, 28,
	 45, 46, 47, 48, 49, 50, 51,
	 87, 88, 89, 90, 91, 92, 93,
	 53, 54, 55, 56, 57, 58, 59,
};

static const VecFx32 mapTransOffs[] = {
//	{ -FX32_ONE*256, 0, -FX32_ONE*256 },
//	{  FX32_ONE*256, 0, -FX32_ONE*256 },
//	{ -FX32_ONE*256, 0,  FX32_ONE*256 },
//	{  FX32_ONE*256, 0,  FX32_ONE*256 },
	{  FX32_ONE*256, 0,  FX32_ONE*256 },
	{  FX32_ONE*(256+512), 0,  FX32_ONE*256 },
	{  FX32_ONE*256, 0,  FX32_ONE*(256+512) },
	{  FX32_ONE*(256+512), 0,  FX32_ONE*(256+512) },
};

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
	int i;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		DeleteMapGraphicData( mapper->gs->sampleMap, i );
	}
	GFL_HEAP_FreeMemory( mapper );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
//------------------------------------------------------------------
static void	MainMapper( MAP_CONT* mapper, VecFx32* pos )
{
	MAP_BLOCK_IDX nowBlockIdx[MAP_BLOCK_COUNT];
	int	i;

 	GetMapperBlockIndex( pos, &nowBlockIdx[0] );

	if(   ( mapper->mapBlockIdx[0].mapID != nowBlockIdx[0].mapID )
		||( mapper->mapBlockIdx[1].mapID != nowBlockIdx[1].mapID )
		||( mapper->mapBlockIdx[2].mapID != nowBlockIdx[2].mapID )
		||( mapper->mapBlockIdx[3].mapID != nowBlockIdx[3].mapID )){
		//reload
		
		//���X�V
		for( i=0; i<MAP_BLOCK_COUNT; i++ ){
			DeleteMapGraphicData( mapper->gs->sampleMap, i );
		}
		for( i=0; i<MAP_BLOCK_COUNT; i++ ){
			CreateMapGraphicData( mapper->gs->sampleMap, i, 
								&nowBlockIdx[i].trans, nowBlockIdx[i].mapID );
			mapper->mapBlockIdx[i] = nowBlockIdx[i];
		}
		OS_Printf("\n");
	}
}


//------------------------------------------------------------------
/**
 * @brief	�}�b�v�u���b�N�擾
 */
//------------------------------------------------------------------
static const int blockIdxOffs[][MAP_BLOCK_COUNT][2] = {
	{ {-1,-1},{-1, 0},{ 0,-1},{ 0, 0} },	//�����̂���u���b�N���獶������ɂS�u���b�N�Ƃ�
	{ {-1, 0},{-1, 1},{ 0, 0},{ 0, 1} },	//�����̂���u���b�N����E������ɂS�u���b�N�Ƃ�
	{ { 0,-1},{ 0, 0},{ 1,-1},{ 1, 0} },	//�����̂���u���b�N���獶�������ɂS�u���b�N�Ƃ�
	{ { 0, 0},{ 0, 1},{ 1, 0},{ 1, 1} },	//�����̂���u���b�N����E�������ɂS�u���b�N�Ƃ�
};

static void GetMapperBlockIndex( const VecFx32* pos, MAP_BLOCK_IDX* mapBlockIdx )
{
	u32		blockIdx, blockx, blockz;
	fx32	posx, posz;
	int		i, offsx, offsz, blockPat = 0;
	
	blockx = FX_Whole( FX_Div( pos->x, MAP_BLOCK_LENX ) );
	blockz = FX_Whole( FX_Div( pos->z, MAP_BLOCK_LENZ ) );

	posx = FX_Mod( pos->x, MAP_BLOCK_LENX );
	posz = FX_Mod( pos->z, MAP_BLOCK_LENZ );

	if( posx > (MAP_BLOCK_LENX/2) ){
		blockPat += 1;
	}
	if( posz > (MAP_BLOCK_LENZ/2) ){
		blockPat += 2;
	}
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		offsx = blockIdxOffs[blockPat][i][1];
		offsz = blockIdxOffs[blockPat][i][0];

		blockIdx = (blockz + offsz) * MAP_BLOCK_SIZX + (blockx + offsx);

		if( blockIdx >= MAP_BLOCK_IDXMAX ){
			blockIdx = MAPID_NULL;
		}
		mapBlockIdx[i].mapID = blockIdx;
		mapBlockIdx[i].trans.x = FX32_ONE*(256 + (blockx + offsx)*512);
		mapBlockIdx[i].trans.y = 0;
		mapBlockIdx[i].trans.z = FX32_ONE*(256 + (blockz + offsz)*512);
	}
}


