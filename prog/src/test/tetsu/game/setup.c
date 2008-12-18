//============================================================================================
/**
 * @file	setup.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include <gflib.h>
#include <textprint.h>

#include "system\gfl_use.h"
#include "system/main.h"
//#include "testmode.h"

#include "double3Ddisp.h"

#include "setup.h"

#include "arc_def.h"

//============================================================================================
//
//
//	���C���R���g���[��
//
//
//============================================================================================
#define BACKGROUND_COL	(GX_RGB(31,31,31))		//�w�i�F
#define DTCM_SIZE		(0x1000)				//DTCM�G���A�̃T�C�Y
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
#define BITMAPWIN_COUNT		(32)

struct _GAME_SYSTEM {
	GFL_G3D_UTIL*			g3Dutil;		//g3Dutil Lib �n���h��
	u16						g3DutilUnitIdx;	//g3Dutil Unit�C���f�b�N�X
	GFL_G3D_SCENE*			g3Dscene;		//g3Dscene Lib �n���h��
	GFL_G3D_CAMERA*			g3Dcamera[2];	//g3Dcamera Lib �n���h��
	GFL_G3D_LIGHTSET*		g3Dlightset[2];	//g3Dlight Lib �n���h��
	GFL_TCB*				dbl3DdispVintr;	//vIntrTask�n���h��

	GFL_PTC_PTR				ptc;			//�p�[�e�B�N��Lib�n���h��
	u8						spa_work[PARTICLE_LIB_HEAP_SIZE];	//�p�[�e�B�N���g�p���[�N

	GFL_BMPWIN*				bitmapWin[BITMAPWIN_COUNT];	//bitmapWin Lib �n���h��

	u32						clactRes[64][2];//clact ���\�[�XINDEX
	u32						clactResCount;	//clact ���\�[�X��
	GFL_CLUNIT*				clactUnit[8];	//clact ���j�b�g
	GFL_TCB*				g2dVintr;		//2D�pvIntrTask�n���h��
	GFL_TCB*				g3dVintr;		//3D�pvIntrTask�n���h��

	SCENE_ACTSYS*			sceneActSys;	//�R�c�A�N�^�[�V�X�e���ݒ�n���h��
	SCENE_MAP*				sceneMap;		//�R�c�}�b�v�ݒ�n���h��
	GFL_BBDACT_SYS*			bbdActSys;		//�r���{�[�h�A�N�g�V�X�e���ݒ�n���h��

	HEAPID					heapID;
};

//------------------------------------------------------------------
/**
 * @brief	�f�B�X�v���C���f�[�^
 */
//------------------------------------------------------------------
///�u�q�`�l�o���N�ݒ�\����
static const GFL_DISP_VRAM dispVram = {
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
	GX_OBJVRAMMODE_CHAR_1D_64K,		// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};

//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�e�[�u��
 */
//------------------------------------------------------------------
#include "test_graphic/test3d.naix"
#include "test_graphic/mapobj.naix"

#if 0
static	const	char	*GraphicFileTable[]={
	"test_graphic/test3d.narc",
	"test_graphic/mapobj.narc",
	"test_graphic/spaEffect.narc",
	"test_graphic/fld_act.narc",
	"test_graphic/fld_map.narc",
	"test_graphic/build_model.narc",
};
#endif

//------------------------------------------------------------------
/**
 * @brief	�R�c�O���t�B�b�N���f�[�^
 */
//------------------------------------------------------------------
//�J���������ݒ�f�[�^
static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 camera0Pos	= { 0, (FX32_ONE * 100), (FX32_ONE * 180) };
static const VecFx32 camera1Pos	= { (FX32_ONE * 100 ), (FX32_ONE * 100), (FX32_ONE * 180) };

//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light0Tbl[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
//	{ 1, {{ (FX16_ONE-1)/2, (FX16_ONE-1)/2, (FX16_ONE-1)/2 }, GX_RGB(31,0,0) } },
//	{ 2, {{ (FX16_ONE-1)/2, (FX16_ONE-1)/2, (FX16_ONE-1)/2 }, GX_RGB(0,31,0) } },
//	{ 3, {{ (FX16_ONE-1)/2, (FX16_ONE-1)/2, (FX16_ONE-1)/2 }, GX_RGB(0,0,31) } },
};
static const GFL_G3D_LIGHT_DATA light1Tbl[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, 0x001f } },
};
static const GFL_G3D_LIGHTSET_SETUP light0Setup = { light0Tbl, NELEMS(light0Tbl) };
static const GFL_G3D_LIGHTSET_SETUP light1Setup = { light1Tbl, NELEMS(light1Tbl) };

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

//------------------------------------------------------------------
/**
 * @brief		�Q�c���\�[�X�f�[�^
 */
//------------------------------------------------------------------
static const GFL_BG_BGCNT_HEADER playiconBGcont = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};
static const GFL_BG_BGCNT_HEADER textBGcont = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};
static const GFL_BG_BGCNT_HEADER maskBGcont = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

static const u16 defaultFontPalette[] = {
	GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0),
	GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0),
	GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0),
	GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0), GX_RGB( 0, 0, 0), GX_RGB(31,31,31),
};

#define GAME_STATUS_WIN_PX		(18)
#define GAME_STATUS_WIN_PY		(0)
#define GAME_STATUS_WIN_SX		(14)
#define GAME_STATUS_WIN_SY		(18)
#define GAME_MSG_WIN_PX			(0)
#define GAME_MSG_WIN_PY			(18)
#define GAME_MSG_WIN_SX			(32)
#define GAME_MSG_WIN_SY			(6)
#define GAME_MAP_WIN_PX			(0)
#define GAME_MAP_WIN_PY			(0)
#define GAME_MAP_WIN_SX			(18)
#define GAME_MAP_WIN_SY			(18)

//bmp, writex, writey, spacex, spacey, colorF, colorB, mode
static const GFL_TEXT_PRINTPARAM playerStatusWinPrintParam = {
	NULL, 1, 1, 1, 1, 15, 1, GFL_TEXT_WRITE_16
};
static const GFL_TEXT_PRINTPARAM msgWinPrintParam = {
	NULL, 1, 1, 1, 1, 1, 15, GFL_TEXT_WRITE_16
};

static const u16 textBitmapWinList[][5] = {
	//�X�e�[�^�X�E�C���h�E
	{ GAME_STATUS_WIN_PX, GAME_STATUS_WIN_PY, GAME_STATUS_WIN_SX, GAME_STATUS_WIN_SY, TEXT_PLTT },
	//���b�Z�[�W�E�C���h�E
	{ GAME_MSG_WIN_PX, GAME_MSG_WIN_PY, GAME_MSG_WIN_SX, GAME_MSG_WIN_SY, TEXT_PLTT },
	//�}�b�v�E�C���h�E
	{ GAME_MAP_WIN_PX, GAME_MAP_WIN_PY, GAME_MAP_WIN_SX, GAME_MAP_WIN_SY, MAP_PLTT },
};

static const u16 maskBitmapWinList[][5] = {
	//�}�b�v�E�C���h�E
	{ GAME_MAP_WIN_PX+1, GAME_MAP_WIN_PY+1, GAME_MAP_WIN_SX-2, GAME_MAP_WIN_SY-2, MASK_PLTT },
};

static const GFL_CLSYS_INIT clactIni = {
	0,0,	// ���C���@�T�[�t�F�[�X�̍�����W
	0,1000,	// �T�u�@�T�[�t�F�[�X�̍�����W
	0,128,	// ���C�����OAM�Ǘ��J�n�ʒu�A�Ǘ���
	0,128,	// �T�u���OAM�Ǘ��J�n�ʒu�A�Ǘ���
	64,		// �Z��Vram�]���Ǘ���
};

static const GFL_OBJGRP_INIT_PARAM objgrpIni = {
	64,			///< �o�^�ł���L�����f�[�^��
	64,			///< �o�^�ł���p���b�g�f�[�^��
	64,			///< �o�^�ł���Z���A�j���p�^�[����
	16,			///< �o�^�ł���}���`�Z���A�j���p�^�[�����i�����󖢑Ή��j
};

enum {
	CLACT_RESLOAD_END = 0xffffffff,
	CLACT_RESLOAD_CGX = 0xfffffffe,
	CLACT_RESLOAD_PLT = 0xfffffffd,
	CLACT_RESLOAD_CEL = 0xfffffffc,
	CLACT_RESLOAD_CGX_TRANS = 0xfffffffb,
	CLACT_RESLOAD_CGX_TRANS_LOOP = 0xfffffffa,
};

static const u32 clactResList[] = {
	//�}�b�v�A�C�R��
	CLACT_RESLOAD_CGX, NARC_mapobj_icon_NCGR, (u32)GFL_VRAM_2D_SUB,
	CLACT_RESLOAD_PLT, NARC_mapobj_icon_NCLR, (u32)GFL_VRAM_2D_SUB, PLTT_DATSIZ * 0,
	CLACT_RESLOAD_PLT, NARC_mapobj_icon2_NCLR, (u32)GFL_VRAM_2D_SUB, PLTT_DATSIZ * 1,
	CLACT_RESLOAD_CEL, NARC_mapobj_icon_NCER, NARC_mapobj_icon_NANR,
	//�X�e�[�^�X�A�C�R��
	CLACT_RESLOAD_CEL, NARC_mapobj_status_NCER, NARC_mapobj_status_NANR,
	CLACT_RESLOAD_CGX_TRANS_LOOP, NARC_mapobj_status_NCGR, (u32)GFL_VRAM_2D_MAIN, STATUS_SETUP_NUM,
	CLACT_RESLOAD_PLT, NARC_mapobj_status_NCLR, (u32)GFL_VRAM_2D_MAIN, PLTT_DATSIZ * 0,
	//�I���R�}���h
	CLACT_RESLOAD_END,
};

static const u32 clactUnitList[] = {
	CLACT_WKSIZ_MAPOBJ,	//�}�b�v�A�C�R��
	CLACT_WKSIZ_STATUS,	//�X�e�[�^�X�A�C�R��
};

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
static void	bg_init( GAME_SYSTEM* gs );
static void	bg_exit( GAME_SYSTEM* gs );
//�R�c�֐�
static void g3d_load( GAME_SYSTEM* gs );
static void g3d_control( GAME_SYSTEM* gs );
static void g3d_draw( GAME_SYSTEM* gs );
static void	g3d_unload( GAME_SYSTEM* gs );
//�Q�c�֐�
static void g2d_load( GAME_SYSTEM* gs );
static void g2d_control( GAME_SYSTEM* gs );
static void g2d_draw( GAME_SYSTEM* gs );
static void	g2d_unload( GAME_SYSTEM* gs );
static void	g2d_vblank( GFL_TCB* tcb, void* work );
static void	g3d_vblank( GFL_TCB* tcb, void* work );
static void	g3d_trans_BBD( GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz );

static void* GetScrnData_for_NitroScrnData( void* scrnFile );
static void* GetCharData_for_NitroCharData( void* charFile );
static void* GetPlttData_for_NitroPlttData( void* plttFile );
//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
GAME_SYSTEM*	SetupGameSystem( HEAPID heapID )
{
	GAME_SYSTEM*	gs = GFL_HEAP_AllocMemory( heapID, sizeof(GAME_SYSTEM) );
	gs->heapID = heapID;

	//����������
	GFL_STD_MtRandInit(0);

	//ARC�V�X�e��������
//	GFL_ARC_Init( &GraphicFileTable[0], NELEMS(GraphicFileTable) );	gfl_use.c��1�񂾂��������ɕύX

	//VRAM�N���A
	GFL_DISP_ClearVRAM( GX_VRAM_D );
	//VRAM�ݒ�
	GFL_DISP_SetBank( &dispVram );

	//BG������
	bg_init( gs );
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

	//�R�c�f�[�^�̃��[�h
	g3d_load( gs );
	gs->g3dVintr = GFUser_VIntr_CreateTCB( g3d_vblank, (void*)gs, 0 );
	//�Q�c�f�[�^�̃��[�h
	g2d_load( gs );
	gs->g2dVintr = GFUser_VIntr_CreateTCB( g2d_vblank, (void*)gs, 0 );

	return gs;
}

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
void	RemoveGameSystem( GAME_SYSTEM* gs )
{
	GFL_TCB_DeleteTask( gs->g2dVintr );
	g2d_unload( gs );	//�Q�c�f�[�^�j��
	GFL_TCB_DeleteTask( gs->g3dVintr );
	g3d_unload( gs );	//�R�c�f�[�^�j��

	bg_exit( gs );
//	GFL_ARC_Exit();

	GFL_HEAP_FreeMemory( gs );
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e�����C���֐�
 */
//------------------------------------------------------------------
void	MainGameSystem( GAME_SYSTEM* gs )
{
	g2d_control( gs );
	g2d_draw( gs );
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

static void	bg_init( GAME_SYSTEM* gs )
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
	GFL_BG_SetBGControl( PLAYICON_FRM, &playiconBGcont, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( PLAYICON_FRM, PLAYICON_FRM_PRI );
	GFL_BG_SetVisible( PLAYICON_FRM, VISIBLE_ON );
	GFL_BG_SetBGControl( TEXT_FRM, &textBGcont, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( TEXT_FRM, TEXT_FRM_PRI );
	GFL_BG_SetVisible( TEXT_FRM, VISIBLE_ON );
	GFL_BG_SetBGControl( MASK_FRM, &maskBGcont, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( MASK_FRM, MASK_FRM_PRI );
	GFL_BG_SetVisible( MASK_FRM, VISIBLE_ON );

	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 8 );

	//�r�b�g�}�b�v�E�C���h�E�V�X�e���̋N��
	GFL_BMPWIN_Init( gs->heapID );

	//�R�c�V�X�e���N��
	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, gs->heapID, G3DsysSetup );
	GFL_BG_SetBGControl3D( G3D_FRM_PRI );

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
	GFL_DISP_SetDispOn();
}

static void	bg_exit( GAME_SYSTEM* gs )
{
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );

	GFL_G3D_Exit();
	GFL_BMPWIN_Exit();
	GFL_BG_FreeBGControl( MASK_FRM );
	GFL_BG_FreeBGControl( TEXT_FRM );
	GFL_BG_FreeBGControl( PLAYICON_FRM );
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

	// �t�H�O�Z�b�g�A�b�v
    {
        u32     fog_table[8];
        int     i;

        G3X_SetFog(TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0800, 0x0e00 );

		G3X_SetFogColor(BACKGROUND_COL, 0);

        for ( i=0; i<8; i++ ){
            fog_table[i] = (u32)(((i*16)<<0) | ((i*16+4)<<8) | ((i*16+8)<<16) | ((i*16+12)<<24));
        }
        G3X_SetFogTable(&fog_table[0]);
	}

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	G3X_SetEdgeColorTable( edgeColorTable ); 
	G3X_EdgeMarking( TRUE );

	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_W );
}


static SCENE_ACT* testSceneAct[8];
#define	NPC_SETNUM	(4)
//------------------------------------------------------------------
/**
 * @brief		�R�c�f�[�^���[�h
 */
//------------------------------------------------------------------
//�쐬
static void g3d_load( GAME_SYSTEM* gs )
{
	//�z�u���ݒ�

	//g3Dutil���g�p���z��Ǘ�������
	gs->g3Dutil = GFL_G3D_UTIL_Create( G3D_UTIL_RESCOUNT, G3D_UTIL_OBJCOUNT, gs->heapID );
	//g3Dscene���g�p���Ǘ�������
	gs->g3Dscene = GFL_G3D_SCENE_Create( gs->g3Dutil, 
						G3D_SCENE_OBJCOUNT, G3D_OBJWORK_SZ, G3D_ACC_COUNT, TRUE, gs->heapID );

	//�J�����쐬
	{
		//fx32 far = 4096 << FX32_SHIFT;	//���C���J������default�ݒ���far��L�΂������̂ōĐݒ�
		fx32 far = 768 << FX32_SHIFT;	//���C���J������default�ݒ���far��L�΂������̂ōĐݒ�

		gs->g3Dcamera[MAINCAMERA_ID] = GFL_G3D_CAMERA_CreateDefault
									( &camera0Pos, &cameraTarget, gs->heapID );
		gs->g3Dcamera[SUBCAMERA_ID] = GFL_G3D_CAMERA_CreateDefault
									( &camera1Pos, &cameraTarget, gs->heapID );
		GFL_G3D_CAMERA_SetFar( gs->g3Dcamera[MAINCAMERA_ID], &far );
	}

	//���C�g�쐬
	gs->g3Dlightset[MAINLIGHT_ID] = GFL_G3D_LIGHT_Create( &light0Setup, gs->heapID );
	gs->g3Dlightset[SUBLIGHT_ID] = GFL_G3D_LIGHT_Create( &light1Setup, gs->heapID );//���g�p

	gs->sceneActSys = Create3DactSys( gs->g3Dscene, gs->heapID );
	gs->sceneMap = Create3Dmap( gs->g3Dscene, gs->heapID );
	gs->bbdActSys = GFL_BBDACT_CreateSys
					( G3D_BBDACT_RESMAX, G3D_BBDACT_ACTMAX, g3d_trans_BBD, gs->heapID );
	//-------------------
	//�m�o�b�e�X�g
	{
		int i;
		VecFx32 trans;

		for( i=0; i<NPC_SETNUM; i++ ){
			testSceneAct[i] = Create3DactNPC( gs->g3Dscene, gs->heapID );

			do{
				trans.x = (GFUser_GetPublicRand( 32 ) - ( 32/2 )) * FX32_ONE*16;
				trans.y = 0;
				trans.z = (GFUser_GetPublicRand( 32 ) - ( 32/2 )) * FX32_ONE*16;
			}while( CheckGroundOutRange( gs->sceneMap, &trans ) == FALSE );
			Set3DactTrans( testSceneAct[i], &trans );
		}
	}
	//-------------------
	//�J�������C�g0���f
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera[MAINCAMERA_ID] );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset[MAINLIGHT_ID] );

	//�p�[�e�B�N�����\�[�X�ǂݍ���
	gs->ptc=GFL_PTC_Create( gs->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, gs->heapID );
	GFL_PTC_SetResource(	gs->ptc, GFL_PTC_LoadArcResource( ARCID_EFFECT, 0, gs->heapID ), 
							TRUE, NULL );
}
	
//����
static void g3d_control( GAME_SYSTEM* gs )
{
	GFL_G3D_SCENE_Main( gs->g3Dscene ); 
	GFL_BBDACT_Main( gs->bbdActSys );
}

//�`��
static void g3d_draw( GAME_SYSTEM* gs )
{
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera[MAINCAMERA_ID] );
	GFL_G3D_SCENE_SetDrawParticleSW( gs->g3Dscene, TRUE );

	GFL_BBDACT_Draw( gs->bbdActSys, gs->g3Dcamera[MAINCAMERA_ID], gs->g3Dlightset[MAINLIGHT_ID] );
	GFL_G3D_SCENE_Draw( gs->g3Dscene );  
}

//�j��
static void g3d_unload( GAME_SYSTEM* gs )
{
	GFL_PTC_Delete(gs->ptc);

	//-------------------
	//�r���{�[�h�e�X�g
	//�m�o�b�e�X�g
	{
		int i;
		for( i=0; i<NPC_SETNUM; i++ ){
			Delete3DactNPC( testSceneAct[i] );
		}
	}
	//-------------------
	GFL_BBDACT_DeleteSys( gs->bbdActSys );
	Delete3Dmap( gs->sceneMap );
	Delete3DactSys( gs->sceneActSys );

	GFL_G3D_LIGHT_Delete( gs->g3Dlightset[SUBLIGHT_ID] );
	GFL_G3D_CAMERA_Delete( gs->g3Dcamera[SUBCAMERA_ID] );
	GFL_G3D_LIGHT_Delete( gs->g3Dlightset[MAINLIGHT_ID] );
	GFL_G3D_CAMERA_Delete( gs->g3Dcamera[MAINCAMERA_ID] );

	GFL_G3D_SCENE_Delete( gs->g3Dscene );  
	GFL_G3D_UTIL_Delete( gs->g3Dutil );
}
	
static void	g3d_vblank( GFL_TCB* tcb, void* work )
{
	GAME_SYSTEM* gs =  (GAME_SYSTEM*)work;
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

//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
static void	g2d_load( GAME_SYSTEM* gs )
{
	int i,p;
	//�a�f�֘A
	{
		//�p���b�g�쐬���]��
		GFL_BG_LoadPalette( TEXT_FRM, (void*)defaultFontPalette, 
							PLTT_DATSIZ, TEXT_PLTT * PLTT_DATSIZ );

		for( i=0; i<BITMAPWIN_COUNT; i++ ){
			gs->bitmapWin[i] = NULL;
		}

		p = 0;
		//�쐬
		for( i=0; i<NELEMS(textBitmapWinList); i++ ){
			//�r�b�g�}�b�v�E�C���h�E�쐬

			gs->bitmapWin[p] = GFL_BMPWIN_Create(	TEXT_FRM, 
												textBitmapWinList[i][0], textBitmapWinList[i][1], 
												textBitmapWinList[i][2], textBitmapWinList[i][3], 
												textBitmapWinList[i][4], GFL_BG_CHRAREA_GET_B );
			//�e�L�X�g�w�i�h��Ԃ�
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp( gs->bitmapWin[p] ), 0 );
	
			//�r�b�g�}�b�v�L�����N�^�[���A�b�v�f�[�g
			GFL_BMPWIN_TransVramCharacter( gs->bitmapWin[p] );
			GFL_BMPWIN_MakeScreen( gs->bitmapWin[p] );
			p++;
		}
		for( i=0; i<NELEMS(maskBitmapWinList); i++ ){
			//�r�b�g�}�b�v�E�C���h�E�쐬

			gs->bitmapWin[p] = GFL_BMPWIN_Create(	MASK_FRM, 
												maskBitmapWinList[i][0], maskBitmapWinList[i][1], 
												maskBitmapWinList[i][2], maskBitmapWinList[i][3], 
												maskBitmapWinList[i][4], GFL_BG_CHRAREA_GET_B );
			//�}�X�N�ʔw�i�h��Ԃ�
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp( gs->bitmapWin[p] ), 0 ); 
	
			//�r�b�g�}�b�v�L�����N�^�[���A�b�v�f�[�g
			GFL_BMPWIN_TransVramCharacter( gs->bitmapWin[p] );
			GFL_BMPWIN_MakeScreen( gs->bitmapWin[p] );
			p++;
		}
		{
			//�}�b�v�p
			void* map_pltt = GFL_ARC_LoadDataAlloc
						( ARCID_SAMPLE, NARC_haruka_gamemap_NCLR, gs->heapID );
			void* map_char = GFL_ARC_LoadDataAlloc
						( ARCID_SAMPLE, NARC_haruka_gamemap_NCGR, gs->heapID );
			void* bmp_adrs = (void*)(GFL_BMP_GetCharacterAdrs
										( GFL_BMPWIN_GetBmp( gs->bitmapWin[G2DBMPWIN_MAP] )));
	
			GFL_BG_LoadPalette( TEXT_FRM, GetPlttData_for_NitroPlttData( map_pltt ), 
								PLTT_DATSIZ, MAP_PLTT * PLTT_DATSIZ );
			GFL_STD_MemCopy32(	GetCharData_for_NitroCharData( map_char ), bmp_adrs, 
								GAME_MAP_WIN_SX * GAME_MAP_WIN_SY * 0x20 );
	
			GFL_HEAP_FreeMemory( map_char );
			GFL_HEAP_FreeMemory( map_pltt );
		}
		{
			//�A�C�R���p
			void* playicon_pltt = GFL_ARC_LoadDataAlloc
						( ARCID_SAMPLE, NARC_haruka_playicon_NCLR, gs->heapID );
			void* playicon_char = GFL_ARC_LoadDataAlloc
						( ARCID_SAMPLE, NARC_haruka_playicon_NCGR, gs->heapID );
			void* playicon_scrn = GFL_ARC_LoadDataAlloc
						( ARCID_SAMPLE, NARC_haruka_playicon_NSCR, gs->heapID );
	
			GFL_BG_LoadPalette( PLAYICON_FRM, GetPlttData_for_NitroPlttData( playicon_pltt ), 
								PLTT_DATSIZ*2, PLAYICON_PLTT * PLTT_DATSIZ );
			GFL_BG_LoadCharacter( PLAYICON_FRM, GetCharData_for_NitroCharData( playicon_char ),
								8*16*0x20, 0 );
#if 0
			GFL_BG_LoadScreenBuffer( PLAYICON_FRM, GetScrnData_for_NitroScrnData( playicon_scrn ), 
								0x800 );
#endif
			GFL_HEAP_FreeMemory( playicon_scrn );
			GFL_HEAP_FreeMemory( playicon_char );
			GFL_HEAP_FreeMemory( playicon_pltt );
		}
	}
	GFL_BG_LoadScreenReq( PLAYICON_FRM );
	GFL_BG_LoadScreenReq( TEXT_FRM );
	GFL_BG_LoadScreenReq( MASK_FRM );

	//�n�a�i�֘A
	{
		ARCHANDLE* clactArc = GFL_ARC_OpenDataHandle( ARCID_MAPOBJ, gs->heapID );
		u32	comm;
		u32	celIDtmp;

		//�V�X�e��������
		GFL_CLACT_Init( &clactIni, gs->heapID );
		GFL_OBJGRP_sysStart( gs->heapID, &dispVram, &objgrpIni );

		i = 0;
		gs->clactResCount = 0;	//clact ���\�[�X��

		while( clactResList[i] != CLACT_RESLOAD_END ){
			comm = clactResList[i];
			switch( comm ){ 
			case CLACT_RESLOAD_CGX:
				{
					u32 cgxID = clactResList[++i];
					GFL_VRAM_TYPE vtype = clactResList[++i];
					gs->clactRes[gs->clactResCount][0] = comm;
					gs->clactRes[gs->clactResCount][1] = GFL_OBJGRP_RegisterCGR
										( clactArc, cgxID, FALSE, vtype, gs->heapID );
					gs->clactResCount++;
				}
				break;
			case CLACT_RESLOAD_CEL:
				{
					u32 celID = clactResList[++i];
					u32 anmID = clactResList[++i];
					gs->clactRes[gs->clactResCount][0] = comm;
					gs->clactRes[gs->clactResCount][1] = GFL_OBJGRP_RegisterCellAnim
										( clactArc, celID, anmID, gs->heapID );
					celIDtmp = gs->clactRes[gs->clactResCount][1];
					gs->clactResCount++;
				}
				break;
			case CLACT_RESLOAD_PLT:
				{
					u32	palID = clactResList[++i];
					GFL_VRAM_TYPE vtype = clactResList[++i];
					u16	poffs  = clactResList[++i];
					gs->clactRes[gs->clactResCount][0] = comm;
					gs->clactRes[gs->clactResCount][1] = GFL_OBJGRP_RegisterPltt
										( clactArc, palID, vtype, poffs, gs->heapID );
					gs->clactResCount++;
				}
				break;
			case CLACT_RESLOAD_CGX_TRANS:
				{	//�]���b�f�w�p�B���O�ɑΉ��Z����o�^���邱��
					u32 cgxID = clactResList[++i];
					GFL_VRAM_TYPE vtype = clactResList[++i];
					gs->clactRes[gs->clactResCount][0] = comm;
					gs->clactRes[gs->clactResCount][1] = GFL_OBJGRP_RegisterCGR_VramTransfer
										( clactArc, cgxID, FALSE, vtype, celIDtmp, gs->heapID );
					gs->clactResCount++;
				}
				break;
			case CLACT_RESLOAD_CGX_TRANS_LOOP:
				{	//�]���b�f�w�p�B���O�ɑΉ��Z����o�^���邱��
					u32 cgxID = clactResList[++i];
					GFL_VRAM_TYPE vtype = clactResList[++i];
					int loopCount = clactResList[++i];
					int j;

					for( j=0; j<loopCount; j++ ){
						gs->clactRes[gs->clactResCount][0] = comm;
						gs->clactRes[gs->clactResCount][1] = GFL_OBJGRP_RegisterCGR_VramTransfer
											( clactArc, cgxID, FALSE, vtype, celIDtmp, gs->heapID );
						gs->clactResCount++;
					}
				}
				break;
			}
			i++;
		}
		//�b�k�`�b�s���j�b�g�쐬
		for( i=0; i<NELEMS(clactUnitList); i++ ){
			gs->clactUnit[i] = GFL_CLACT_UNIT_Create( clactUnitList[i], gs->heapID );
		}

		GFL_ARC_CloseDataHandle( clactArc );
	}
}

static void g2d_control( GAME_SYSTEM* gs )
{
	int i;

	GFL_CLACT_ClearOamBuff();
	for( i=0; i<NELEMS(clactUnitList); i++ ){
		GFL_CLACT_UNIT_Draw( gs->clactUnit[i] );
	}
}

static void g2d_draw( GAME_SYSTEM* gs )
{
	GFL_CLACT_Main();
}

static void	g2d_unload( GAME_SYSTEM* gs )
{
	int i;
	{
		for( i=0; i<NELEMS(clactUnitList); i++ ){
 			GFL_CLACT_UNIT_Delete( gs->clactUnit[i] );
		}

		for( i=0; i<gs->clactResCount; i++ ){
			switch( gs->clactRes[i][0] ){
			case CLACT_RESLOAD_CGX:
				GFL_OBJGRP_ReleaseCGR( gs->clactRes[i][1] );
				break;
			case CLACT_RESLOAD_CEL:
				GFL_OBJGRP_ReleaseCellAnim( gs->clactRes[i][1] );
				break;
			case CLACT_RESLOAD_PLT:
				GFL_OBJGRP_ReleasePltt( gs->clactRes[i][1] );
				break;
			}
		}
		GFL_OBJGRP_sysEnd();
		GFL_OBJGRP_Exit();
		GFL_CLACT_Exit();
	}
	{
		//�j��
		for( i=0; i<BITMAPWIN_COUNT; i++ ){
			if( gs->bitmapWin[i] ){
				GFL_BMPWIN_Delete( gs->bitmapWin[i] );
			}
		}
	}
}

static void	g2d_vblank( GFL_TCB* tcb, void* work )
{
	GAME_SYSTEM* gs =  (GAME_SYSTEM*)work;

	GFL_CLACT_VBlankFuncTransOnly();
}

//------------------------------------------------------------------
/**
 * @brief		NNS�|�C���^�擾�֘A
 */
//------------------------------------------------------------------
static void* GetScrnData_for_NitroScrnData( void* scrnFile )
{
	NNSG2dScreenData* data;

	NNS_G2dGetUnpackedScreenData( scrnFile, &data );
	return &data->rawData[0];
}

static void* GetCharData_for_NitroCharData( void* charFile )
{
	NNSG2dCharacterData* data;

	NNS_G2dGetUnpackedBGCharacterData( charFile, &data );
	return data->pRawData;
}

static void* GetPlttData_for_NitroPlttData( void* plttFile )
{
	NNSG2dPaletteData* data;

	NNS_G2dGetUnpackedPaletteData( plttFile, &data );
	return data->pRawData;
}

//------------------------------------------------------------------
/**
 * @brief		�n���h���擾
 */
//------------------------------------------------------------------
GFL_G3D_UTIL*		Get_GS_G3Dutil( GAME_SYSTEM* gs )
{
	return gs->g3Dutil;
}

GFL_G3D_SCENE*		Get_GS_G3Dscene( GAME_SYSTEM* gs )
{
	return gs->g3Dscene;
}

GFL_G3D_CAMERA*		Get_GS_G3Dcamera( GAME_SYSTEM* gs, int cameraID )
{
	return gs->g3Dcamera[ cameraID ];
}

GFL_G3D_LIGHTSET*	Get_GS_G3Dlight( GAME_SYSTEM* gs, int lightID )
{
	return gs->g3Dlightset[ lightID ];
}

GFL_BMPWIN*			Get_GS_BmpWin( GAME_SYSTEM* gs, int bmpwinID )
{
	GF_ASSERT( bmpwinID < BITMAPWIN_COUNT );
	GF_ASSERT( gs->bitmapWin[ bmpwinID ] );
		
	return gs->bitmapWin[ bmpwinID ];
}

GFL_PTC_PTR			Get_GS_Perticle( GAME_SYSTEM* gs )
{
	return gs->ptc;
}

SCENE_ACTSYS*		Get_GS_SceneActSys( GAME_SYSTEM* gs )
{
	return gs->sceneActSys;
}

SCENE_MAP*			Get_GS_SceneMap( GAME_SYSTEM* gs )
{
	return gs->sceneMap;
}

GFL_BBDACT_SYS*		Get_GS_BillboardActSys( GAME_SYSTEM* gs )	
{
	return gs->bbdActSys;
}

GFL_CLUNIT*			Get_GS_ClactUnit( GAME_SYSTEM* gs, u32 unitID )
{
	return gs->clactUnit[unitID];
}

u32					Get_GS_ClactResIdx( GAME_SYSTEM* gs, u32 resID )
{
	GF_ASSERT( resID < gs->clactResCount );
	return gs->clactRes[resID][1];
}

