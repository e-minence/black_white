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

//------------------------------------------------------------------
/**
 * @brief	���f�[�^
 */
//------------------------------------------------------------------
typedef struct _SAMPLE_SETUP SAMPLE_SETUP;

static SAMPLE_SETUP*	SetupGameSystem( HEAPID heapID );
static void			RemoveGameSystem( SAMPLE_SETUP* gs );
static void			MainGameSystem( SAMPLE_SETUP* gs );

//�A�[�J�C�u�h�m�c�d�w
enum {
	ARCID_FLDMAP = 0,
	ARCID_FLDBLD,
};

typedef struct _SAMPLE_MAP		SAMPLE_MAP;

#define MAP3D_SCALE			(FX32_ONE*1)
#define MAP3D_GRID_LEN		(MAP3D_SCALE*16)
#define MAP3D_GRID_SIZEX	(32)
#define MAP3D_GRID_SIZEZ	(32)

static SAMPLE_MAP*	Create3Dmap( GFL_G3D_SCENE* g3Dscene, HEAPID heapID );
static void			Delete3Dmap( SAMPLE_MAP* sampleMap );

static void moveMapDebug( SAMPLE_SETUP* gs );
//============================================================================================
//
//
//	���C���R���g���[��
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
typedef struct {
	HEAPID			heapID;
	SAMPLE_SETUP*	gs;
//	GAME_CONTROL*	gc; 

	int				myNetID;
	int				seq;
	int				mainContSeq;
	int				timer;
//-------------
	int				totalPlayerCount;
	int				playNetID;
//-------------
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
		sampleWork->myNetID = 0;
		sampleWork->playNetID = sampleWork->myNetID;
		sampleWork->seq++;
		break;

	case 1:
		//�V�X�e���Z�b�g�A�b�v
		sampleWork->gs = SetupGameSystem( sampleWork->heapID );
		sampleWork->seq++;
		sampleWork->mainContSeq = 0;
		break;

	case 2:
		moveMapDebug( sampleWork->gs );

		if( GameEndCheck( GFL_UI_KEY_GetCont() ) == TRUE ){
			sampleWork->seq++;
		}
		MainGameSystem( sampleWork->gs );
		break;

	case 3:
		RemoveGameSystem( sampleWork->gs );
		sampleWork->seq++;
		break;

	case 4:
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
//
//
//	�Z�b�g�A�b�v
//
//
//============================================================================================
#define BACKGROUND_COL	(GX_RGB(20,20,31))		//�w�i�F
#define DTCM_SIZE		(0x1000)				//DTCM�G���A�̃T�C�Y
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

	SAMPLE_MAP*				sampleMap;		//�R�c�}�b�v�ݒ�n���h��
	GFL_BBDACT_SYS*			bbdActSys;		//�r���{�[�h�A�N�g�V�X�e���ݒ�n���h��

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
	"test_graphic/fld_map.narc",
	"test_graphic/build_model.narc",
};

//------------------------------------------------------------------
/**
 * @brief	�R�c�O���t�B�b�N���f�[�^
 */
//------------------------------------------------------------------
//�J���������ݒ�f�[�^
static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 camera0Pos	= { 0, (FX32_ONE * 100), (FX32_ONE * 180) };

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
#define G3D_BBDACT_RESMAX	(64)		//billboardAct�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
#define G3D_BBDACT_ACTMAX	(256)		//billboardAct�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��

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
static void	g3d_trans_BBD( GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz );

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
	{
		fx32 far = 768 << FX32_SHIFT;	//���C���J������default�ݒ���far��L�΂������̂ōĐݒ�

		gs->g3Dcamera = GFL_G3D_CAMERA_CreateDefault
									( &camera0Pos, &cameraTarget, gs->heapID );
		GFL_G3D_CAMERA_SetFar( gs->g3Dcamera, &far );
	}

	//���C�g�쐬
	gs->g3Dlightset = GFL_G3D_LIGHT_Create( &light0Setup, gs->heapID );

	gs->sampleMap = Create3Dmap( gs->g3Dscene, gs->heapID );
	gs->bbdActSys = GFL_BBDACT_CreateSys
					( G3D_BBDACT_RESMAX, G3D_BBDACT_ACTMAX, g3d_trans_BBD, gs->heapID );
	//�J�������C�g0���f
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset );
}
	
//����
static void g3d_control( SAMPLE_SETUP* gs )
{
	GFL_BBDACT_Main( gs->bbdActSys );
	GFL_G3D_SCENE_Main( gs->g3Dscene ); 
}

//�`��
static void g3d_draw( SAMPLE_SETUP* gs )
{
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset );
	GFL_BBDACT_Draw( gs->bbdActSys, gs->g3Dcamera, gs->g3Dlightset );
	GFL_G3D_SCENE_Draw( gs->g3Dscene );  
}

//�j��
static void g3d_unload( SAMPLE_SETUP* gs )
{
	GFL_BBDACT_DeleteSys( gs->bbdActSys );
	Delete3Dmap( gs->sampleMap );

	GFL_G3D_LIGHT_Delete( gs->g3Dlightset );
	GFL_G3D_CAMERA_Delete( gs->g3Dcamera );

	GFL_G3D_SCENE_Delete( gs->g3Dscene );  
	GFL_G3D_UTIL_Delete( gs->g3Dutil );
}
	
static void	g3d_vblank( GFL_TCB* tcb, void* work )
{
	SAMPLE_SETUP* gs =  (SAMPLE_SETUP*)work;
}

//BBD�pVRAM�]���֐�
static void	g3d_trans_BBD( GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz )
{
	NNS_GFD_DST_TYPE transType;

	if( type == GFL_BBDACT_TRANSTYPE_DATA ){
		transType = NNS_GFD_DST_3D_TEX_VRAM;
	} else {
		transType = NNS_GFD_DST_3D_TEX_PLTT;
	}
	NNS_GfdRegisterNewVramTransferTask( transType, dst, (void*)src, siz );
}






//============================================================================================
/**
 *
 * @brief	�}�b�v����
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
#define MAP_BLOCK_COUNT	(4)

typedef struct {
	u16	id;
	u16	count;
}MAPOBJ_HEADER;

typedef struct {
	int					idx;
	MAPOBJ_HEADER		floor;
	VecFx32				trans;
}MAP_BLOCK_DATA;

struct _SAMPLE_MAP {
	GFL_G3D_SCENE*		g3Dscene;
	u16					unitIdx;
	u16					resIdx;
	u16					objIdx;
	MAP_BLOCK_DATA		mapBlock[MAP_BLOCK_COUNT];
	u16					anmTimer;
};

typedef struct {
	const GFL_G3D_SCENEOBJ_DATA*	data;
	u16								dataCount;
}MAPDATA;

static void	CreateMapGraphicData( SAMPLE_MAP* sampleMap, MAP_BLOCK_DATA* mapBlock, int mapID );
static void	DeleteMapGraphicData( SAMPLE_MAP* sampleMap, MAP_BLOCK_DATA* mapBlock );

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
#include "graphic_data/test_graphic/fld_map.naix"
#include "graphic_data/test_graphic/build_model_id.h"

enum {
	G3DRES_MAP_FLOOR = 0,
	G3DRES_MAP_FLOOR0,
	G3DRES_MAP_FLOOR1,
	G3DRES_MAP_FLOOR2,
	G3DRES_MAP_FLOOR3,
	G3DRES_BLD_BUILD0,
	G3DRES_BLD_BUILD1,
	G3DRES_BLD_BUILD2,
	G3DRES_BLD_BUILD3,
	G3DRES_BLD_DOOR0,
	G3DRES_BLD_DOOR1,
	G3DRES_BLD_DOOR2,
	G3DRES_BLD_DOOR3,
};
//�R�c�O���t�B�b�N���\�[�X�e�[�u��
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_FLDMAP, NARC_fld_map_map14_16c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDMAP, NARC_fld_map_map21_18c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDMAP, NARC_fld_map_map21_19c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDMAP, NARC_fld_map_map22_18c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDMAP, NARC_fld_map_map22_19c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_PC, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_T5_S01, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_C4_H01A, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_T5_S02, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_T5_O01, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_T5_O01B, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_C4_DOOR1, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_P_DOOR, GFL_G3D_UTIL_RESARC },
};

//---------------------
enum {
	G3DOBJ_MAP_FLOOR = 0,
	G3DOBJ_MAP_FLOOR0,
	G3DOBJ_MAP_FLOOR1,
	G3DOBJ_MAP_FLOOR2,
	G3DOBJ_MAP_FLOOR3,
	G3DOBJ_BLD_BUILD0,
	G3DOBJ_BLD_BUILD1,
	G3DOBJ_BLD_BUILD2,
	G3DOBJ_BLD_BUILD3,
	G3DOBJ_BLD_DOOR0,
	G3DOBJ_BLD_DOOR1,
	G3DOBJ_BLD_DOOR2,
	G3DOBJ_BLD_DOOR3,
};
//�R�c�I�u�W�F�N�g��`�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
	{ G3DRES_MAP_FLOOR, 0, G3DRES_MAP_FLOOR, NULL, 0 },
	{ G3DRES_MAP_FLOOR0, 0, G3DRES_MAP_FLOOR0, NULL, 0 },
	{ G3DRES_MAP_FLOOR1, 0, G3DRES_MAP_FLOOR1, NULL, 0 },
	{ G3DRES_MAP_FLOOR2, 0, G3DRES_MAP_FLOOR2, NULL, 0 },
	{ G3DRES_MAP_FLOOR3, 0, G3DRES_MAP_FLOOR3, NULL, 0 },
	{ G3DRES_BLD_BUILD0, 0, G3DRES_BLD_BUILD0, NULL, 0 },
	{ G3DRES_BLD_BUILD1, 0, G3DRES_BLD_BUILD1, NULL, 0 },
	{ G3DRES_BLD_BUILD2, 0, G3DRES_BLD_BUILD2, NULL, 0 },
	{ G3DRES_BLD_BUILD3, 0, G3DRES_BLD_BUILD3, NULL, 0 },
	{ G3DRES_BLD_DOOR0, 0, G3DRES_BLD_DOOR0, NULL, 0 },
	{ G3DRES_BLD_DOOR1, 0, G3DRES_BLD_DOOR1, NULL, 0 },
	{ G3DRES_BLD_DOOR2, 0, G3DRES_BLD_DOOR2, NULL, 0 },
	{ G3DRES_BLD_DOOR3, 0, G3DRES_BLD_DOOR3, NULL, 0 },
};

//---------------------
//g3Dscene �����ݒ�e�[�u���f�[�^
static const GFL_G3D_UTIL_SETUP g3Dutil_setup = {
	g3Dutil_resTbl, NELEMS(g3Dutil_resTbl),
	g3Dutil_objTbl, NELEMS(g3Dutil_objTbl),
};

static const GFL_G3D_SCENEOBJ_DATA mapGraphicData0[] = {
	{	G3DOBJ_MAP_FLOOR, 0, 1, 31, FALSE, TRUE,
		{	{ 0, -FX32_ONE*80, 0 },
			{ MAP3D_SCALE, MAP3D_SCALE, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD0, 0, 1, 31, FALSE, TRUE,
		{	{ 0x89000, 0x21800, 0x93000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD1, 0, 1, 31, FALSE, TRUE,
		{	{ 0xffff7000, 0x21800, 0xfff29000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfff34000, 0x21800, 0xfff2d000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xa0000, 0x21800, 0xfff2d000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfff6d000, 0x21800, 0x99000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD3, 0, 1, 31, FALSE, TRUE,
		{	{ 0, 0x1000, 0 },
			{ MAP3D_SCALE/2, MAP3D_SCALE/2, MAP3D_SCALE/2 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR0, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfffc8000, 0, 0xfff90000 },
			{ MAP3D_SCALE, MAP3D_SCALE, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR1, 0, 1, 31, FALSE, TRUE,
		{	{ 0x28000, 0, 0xfff90000 },
			{ MAP3D_SCALE, MAP3D_SCALE, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR2, 0, 1, 31, FALSE, TRUE,
		{	{ 0x1000, 0x22000, 0xfff36000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfff36000, 0x22000, 0xfff3a000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xa2000, 0x22000, 0xfff3a000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfff6f000, 0x22000, 0xa6000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR3, 0, 1, 31, FALSE, TRUE,
		{	{ 0x88000, 0x22000, 0xa6000 },
			{ MAP3D_SCALE, MAP3D_SCALE/2, MAP3D_SCALE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
};

static const MAPDATA mapData[] = {
	{ mapGraphicData0, NELEMS(mapGraphicData0) },
};

static const VecFx32 mapTransOffs[] = {
	{ -FX32_ONE*256, 0, -FX32_ONE*256 },
	{  FX32_ONE*256, 0, -FX32_ONE*256 + FX32_ONE*32 },
	{ -FX32_ONE*256, 0,  FX32_ONE*256 },
	{  FX32_ONE*256, 0,  FX32_ONE*256 + FX32_ONE*32 },
};

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v����
 */
//------------------------------------------------------------------
static SAMPLE_MAP*	Create3Dmap( GFL_G3D_SCENE* g3Dscene, HEAPID heapID )
{
	SAMPLE_MAP* sampleMap = GFL_HEAP_AllocClearMemory( heapID, sizeof(SAMPLE_MAP) );
	int	i;

	//�R�c�f�[�^�Z�b�g�A�b�v
	sampleMap->g3Dscene = g3Dscene;
	sampleMap->unitIdx = GFL_G3D_SCENE_AddG3DutilUnit( g3Dscene, &g3Dutil_setup );
	sampleMap->resIdx = GFL_G3D_SCENE_GetG3DutilUnitResIdx( g3Dscene, sampleMap->unitIdx );
	sampleMap->objIdx = GFL_G3D_SCENE_GetG3DutilUnitObjIdx( g3Dscene, sampleMap->unitIdx );

	//�}�b�v�쐬
	{
		int mapID = 0;

		for( i=0; i<MAP_BLOCK_COUNT; i++ ){
			sampleMap->mapBlock[i].idx = i;
			CreateMapGraphicData( sampleMap, &sampleMap->mapBlock[i], mapID );
			sampleMap->mapBlock[i].trans.x = mapTransOffs[i].x;
			sampleMap->mapBlock[i].trans.y = mapTransOffs[i].y;
			sampleMap->mapBlock[i].trans.z = mapTransOffs[i].z;
			{
				GFL_G3D_SCENEOBJ* g3DsceneObj;
				VecFx32 trans;
				int j;

				for( j=0; j<sampleMap->mapBlock[i].floor.count; j++ ){
					g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sampleMap->g3Dscene, 
													sampleMap->mapBlock[i].floor.id + j );
					GFL_G3D_SCENEOBJ_GetPos( g3DsceneObj, &trans );
					VEC_Add( &trans, &sampleMap->mapBlock[i].trans, &trans );
					GFL_G3D_SCENEOBJ_SetPos( g3DsceneObj, &trans );
				}
			}
		}
	}
	sampleMap->anmTimer = 0;

	return sampleMap;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�j��
 */
//------------------------------------------------------------------
static void	Delete3Dmap( SAMPLE_MAP* sampleMap )
{
	int	i;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		DeleteMapGraphicData( sampleMap, &sampleMap->mapBlock[i] );
	}
	GFL_G3D_SCENE_DelG3DutilUnit( sampleMap->g3Dscene, sampleMap->unitIdx );
	GFL_HEAP_FreeMemory( sampleMap );
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�}�b�v�f�[�^�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static void	CreateMapGraphicData( SAMPLE_MAP* sampleMap, MAP_BLOCK_DATA* mapBlock, int mapID )
{
	GFL_G3D_SCENEOBJ_DATA*	pdata;

	GF_ASSERT( mapID < NELEMS(mapData) );

	mapBlock->floor.count = mapData[mapID].dataCount;
	mapBlock->floor.id = GFL_G3D_SCENEOBJ_Add(	sampleMap->g3Dscene, 
												mapData[mapID].data,
												mapData[mapID].dataCount,
												sampleMap->objIdx );
}

//------------------------------------------------------------------
static void	DeleteMapGraphicData( SAMPLE_MAP* sampleMap, MAP_BLOCK_DATA* mapBlock )
{
	GFL_G3D_SCENEOBJ_Remove(	sampleMap->g3Dscene, 
								mapBlock->floor.id, 
								mapBlock->floor.count ); 
}









//------------------------------------------------------------------
#define MV_SPEED	(FX32_ONE*4)
static void moveMapDebug( SAMPLE_SETUP* gs )
{
	VecFx32	trans;
	VecFx32	vecMove = { 0, 0, 0 };
	BOOL mvf = FALSE;

	if( GFL_UI_KEY_CheckCont( PAD_BUTTON_A ) == TRUE ){
		vecMove.y = -MV_SPEED;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_BUTTON_B ) == TRUE ){
		vecMove.y = MV_SPEED;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_KEY_UP ) == TRUE ){
		vecMove.z = -MV_SPEED;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_KEY_DOWN ) == TRUE ){
		vecMove.z = MV_SPEED;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_KEY_LEFT ) == TRUE ){
		vecMove.x = -MV_SPEED;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_KEY_RIGHT ) == TRUE ){
		vecMove.x = MV_SPEED;
		mvf = TRUE;
	}

	if( mvf == TRUE ){
		VecFx32 pos, target;

		GFL_G3D_CAMERA_GetPos( gs->g3Dcamera, &pos );
		GFL_G3D_CAMERA_GetTarget( gs->g3Dcamera, &target );

		VEC_Add( &pos, &vecMove, &pos );
		VEC_Add( &target, &vecMove, &target );

		GFL_G3D_CAMERA_SetPos( gs->g3Dcamera, &pos );
		GFL_G3D_CAMERA_SetTarget( gs->g3Dcamera, &target );
	}
}

