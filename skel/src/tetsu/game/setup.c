//============================================================================================
/**
 * @file	setup.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "textprint.h"
#include "gfl_use.h"

#include "main.h"
#include "testmode.h"

#include "double3Ddisp.h"

#include "setup.h"
#include "src/sample_graphic/haruka.naix"

//============================================================================================
//
//
//	���C���R���g���[��
//
//
//============================================================================================
#define BACKGROUND_COL	(GX_RGB(20,20,31))		//�w�i�F
#define DTCM_SIZE		(0x1000)	//DTCM�G���A�̃T�C�Y
#define G3D_FRM_PRI		(1)			//�R�c�ʂ̕`��v���C�I���e�B�[
#define G3D_OBJ_COUNT	(1000)		//g3Dscene�Ŏg�p����sceneObj�̍ő�ݒ�\��
#define G3D_OBJWORK_SZ	(32)		//g3Dscene�Ŏg�p����sceneObj�̃��[�N�T�C�Y
#define G3D_ACC_COUNT	(32)		//g3Dscene�Ŏg�p����sceneObjAccesary�̍ő�ݒ�\��

//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
struct _GAME_SYSTEM {
	GFL_G3D_UTIL*			g3Dutil;		//g3Dutil Lib �n���h��
	GFL_G3D_SCENE*			g3Dscene;		//g3Dscene Lib �n���h��
	GFL_G3D_CAMERA*			g3Dcamera[2];	//g3Dcamera Lib �n���h��
	GFL_G3D_LIGHTSET*		g3Dlightset[2];	//g3Dlight Lib �n���h��
	GFL_PTC_PTR				ptc;			//�p�[�e�B�N��Lib�n���h��
	u8						spa_work[PARTICLE_LIB_HEAP_SIZE];	//�p�[�e�B�N���g�p���[�N
	SCENE_MAP*				sceneMap;		//�R�c�}�b�v�ݒ�n���h��
	SCENE_OBJ*				sceneObj;		//�R�c�I�u�W�F�N�g�ݒ�n���h��
	GFL_TCB*				dbl3DdispVintr;	//vIntrTask�n���h��
	HEAPID					heapID;

	u32						g3DsceneObjID;
};

//------------------------------------------------------------------
/**
 * @brief	�R�c�O���t�B�b�N���f�[�^
 */
//------------------------------------------------------------------
//�J���������ݒ�f�[�^
static const VecFx32 camera0Target	= { 0, 0, 0 };
static const VecFx32 camera0Pos	= { 0, (FX32_ONE * 100), (FX32_ONE * 180) };
static const VecFx32 camera1Pos	= { (FX32_ONE * 100 ), (FX32_ONE * 100), (FX32_ONE * 180) };

//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light0Tbl[] = {
	{ 0, {{ 0, 0, -(FX16_ONE-1) }, 0x7fff } },
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
static const char g3DarcPath2[] = {"src/sample_graphic/haruka.narc"};

//�R�c�O���t�B�b�N���\�[�X�e�[�u��
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ (u32)g3DarcPath2, NARC_haruka_test_floor2_nsbmd,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_human2_nsbmd,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_human2_stay_nsbca,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_human2_walk_nsbca,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_human2_run_nsbca,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_human2_attack_nsbca,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_human2_shoot_nsbca,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_human2_spell_nsbca,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_human2_sit_nsbca,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_sword_nsbmd,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_shield_nsbmd,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_bow_nsbmd,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_staff_nsbmd,  GFL_G3D_UTIL_RESPATH },
};

//---------------------
//�R�c�I�u�W�F�N�g�A�j���[�V������`�e�[�u��
static const GFL_G3D_UTIL_ANM g3Dutil_Human2AnmTbl[] = {
	{ G3DRES_HUMAN2_STAY_BCA, 0 },
	{ G3DRES_HUMAN2_WALK_BCA, 0 },
	{ G3DRES_HUMAN2_RUN_BCA, 0 },
	{ G3DRES_HUMAN2_ATTACK_BCA, 0 },
	{ G3DRES_HUMAN2_SHOOT_BCA, 0 },
	{ G3DRES_HUMAN2_SPELL_BCA, 0 },
	{ G3DRES_HUMAN2_SIT_BCA, 0 },
};

//---------------------
//�R�c�I�u�W�F�N�g��`�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
	{ G3DRES_MAP_FLOOR, 0, G3DRES_MAP_FLOOR, NULL, 0 },
	{ G3DRES_HUMAN2_BMD, 0, G3DRES_HUMAN2_BMD, g3Dutil_Human2AnmTbl, NELEMS(g3Dutil_Human2AnmTbl)},
	{ G3DRES_HUMAN2_BMD, 0, G3DRES_HUMAN2_BMD, g3Dutil_Human2AnmTbl, NELEMS(g3Dutil_Human2AnmTbl)},
	{ G3DRES_ACCE_SWORD, 0, G3DRES_ACCE_SWORD, NULL, 0 },
	{ G3DRES_ACCE_SHIELD, 0, G3DRES_ACCE_SHIELD, NULL, 0 },
	{ G3DRES_ACCE_BOW, 0, G3DRES_ACCE_BOW, NULL, 0 },
	{ G3DRES_ACCE_STAFF, 0, G3DRES_ACCE_STAFF, NULL, 0 },
};

//---------------------
//g3Dscene �����ݒ�e�[�u���f�[�^
static const GFL_G3D_UTIL_SETUP g3Dutil_setup = {
	g3Dutil_resTbl, NELEMS(g3Dutil_resTbl),
	g3Dutil_objTbl, NELEMS(g3Dutil_objTbl),
};

//�p�[�e�B�N���p�A�[�J�C�u�e�[�u���isample�j
static	const	char	*GraphicFileTable[]={
	"src/sample_graphic/spa.narc",
};

//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
//�a�f�ݒ�֐�
static void	bg_init( GAME_SYSTEM* gs );
static void	bg_exit( GAME_SYSTEM* gs );
//�R�c�f�[�^���[�h�֐�
static void g3d_load( GAME_SYSTEM*	gs );
static void	g3d_unload( GAME_SYSTEM* gs );

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
	GFL_ARC_Init( &GraphicFileTable[0], 1 );
	//BG������
	bg_init( gs );
	//�R�c�f�[�^�̃��[�h
	g3d_load( gs );
	//���ʂR�c�pvIntrTask�ݒ�
	gs->dbl3DdispVintr = GFUser_VIntr_CreateTCB( GFL_G3D_DOUBLE3D_VblankIntrTCB, NULL, 0 );

	return gs;
}

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
void	RemoveGameSystem( GAME_SYSTEM* gs )
{
	g3d_unload( gs );	//�R�c�f�[�^�j��

	GFL_TCB_DeleteTask( gs->dbl3DdispVintr );
	bg_exit( gs );

	GFL_ARC_Exit();

	GFL_HEAP_FreeMemory( gs );
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e�����C���֐��i�O�j
 */
//------------------------------------------------------------------
void	MainGameSystemPref( GAME_SYSTEM* gs )
{
	GFL_G3D_SCENE_Main( gs->g3Dscene ); 
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e�����C���֐��i��j
 */
//------------------------------------------------------------------
void	MainGameSystemAfter( GAME_SYSTEM* gs )
{
	//�t���O�ɂ���ĕ`���؂�ւ���
	if( GFL_G3D_DOUBLE3D_GetFlip() ){
		GFL_G3D_CAMERA_Switching( gs->g3Dcamera[0] );
		GFL_G3D_SCENE_SetDrawParticleSW( gs->g3Dscene, TRUE );
	} else {
		GFL_G3D_CAMERA_Switching( gs->g3Dcamera[1] );
		GFL_G3D_SCENE_SetDrawParticleSW( gs->g3Dscene, FALSE );
	}
	GFL_G3D_SCENE_Draw( gs->g3Dscene );  
	GFL_G3D_DOUBLE3D_SetSwapFlag();
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

	//�u�q�`�l�ݒ�
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G); 

	//�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocClearMemoryLo( gs->heapID, 16*2 );
		plt[0] = BACKGROUND_COL;
		GFL_BG_LoadPalette( GFL_BG_FRAME0_M, plt, 16*2, 0 );	//���C����ʂ̔w�i�F�]��
		GFL_BG_LoadPalette( GFL_BG_FRAME0_S, plt, 16*2, 0 );	//�T�u��ʂ̔w�i�F�]��
		GFL_HEAP_FreeMemory( plt );
	}
	//�a�f���[�h�ݒ�
	GFL_BG_InitBG( &bgsysHeader );

	//�R�c�V�X�e���N��
	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, gs->heapID, G3DsysSetup );
	GFL_G3D_DOUBLE3D_Init( gs->heapID );	//���ʂR�c�p�̏�����
	GFL_BG_SetBGControl3D( G3D_FRM_PRI );

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();
}

static void	bg_exit( GAME_SYSTEM* gs )
{
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_G3D_DOUBLE3D_Exit();
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
	G3X_AlphaBlend(TRUE);
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

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
static void g3d_load( GAME_SYSTEM*	gs )
{
	//�z�u���ݒ�

	//g3Dutil���g�p���z��Ǘ�������
	gs->g3Dutil = GFL_G3D_UTIL_Create( &g3Dutil_setup, gs->heapID );
	{
		//�A�j���[�V�����̏����L���ݒ�(0�Ԃ̃A�j���[�V������L���ɂ���)
		GFL_G3D_OBJECT_EnableAnime
		( GFL_G3D_UTIL_GetObjHandle( gs->g3Dutil, G3DOBJ_HUMAN2), HUMAN2_ANM_STAY );
		GFL_G3D_OBJECT_EnableAnime
		( GFL_G3D_UTIL_GetObjHandle( gs->g3Dutil, G3DOBJ_HUMAN2_STOP), HUMAN2_ANM_STAY );
	}

	//g3Dscene���g�p���Ǘ�������
	gs->g3Dscene = GFL_G3D_SCENE_Create( gs->g3Dutil, 
						G3D_OBJ_COUNT, G3D_OBJWORK_SZ, G3D_ACC_COUNT, TRUE, gs->heapID );

	gs->sceneMap = Create3Dmap( gs->g3Dscene, gs->heapID );
	gs->sceneObj = Create3Dobj( gs->g3Dscene, gs->heapID );

	//�J�����쐬
	{
		fx32 far = 4096 << FX32_SHIFT;	//���C���J������default�ݒ���far��L�΂������̂ōĐݒ�

		gs->g3Dcamera[MAINCAMERA_ID] = GFL_G3D_CAMERA_CreateDefault
									( &camera0Pos, &camera0Target, gs->heapID );
		gs->g3Dcamera[SUBCAMERA_ID] = GFL_G3D_CAMERA_CreateDefault
									( &camera1Pos, &camera0Target, gs->heapID );
		GFL_G3D_CAMERA_SetFar( gs->g3Dcamera[MAINCAMERA_ID], &far );
	}

	//���C�g�쐬
	gs->g3Dlightset[MAINLIGHT_ID] = GFL_G3D_LIGHT_Create( &light0Setup, gs->heapID );
	gs->g3Dlightset[SUBLIGHT_ID] = GFL_G3D_LIGHT_Create( &light1Setup, gs->heapID );//���g�p

	//�J�������C�g0���f
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera[MAINCAMERA_ID] );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset[MAINLIGHT_ID] );

	//�p�[�e�B�N�����\�[�X�ǂݍ���
	gs->ptc=GFL_PTC_Create( gs->spa_work, PARTICLE_LIB_HEAP_SIZE,TRUE, gs->heapID );
	GFL_PTC_SetResource( gs->ptc, GFL_PTC_LoadArcResource( 0, 0, gs->heapID ), TRUE, NULL );
}
	
//�j��
static void g3d_unload( GAME_SYSTEM* gs )
{
	GFL_PTC_Delete(gs->ptc);

	GFL_G3D_LIGHT_Delete( gs->g3Dlightset[SUBLIGHT_ID] );
	GFL_G3D_CAMERA_Delete( gs->g3Dcamera[SUBCAMERA_ID] );
	GFL_G3D_LIGHT_Delete( gs->g3Dlightset[MAINLIGHT_ID] );
	GFL_G3D_CAMERA_Delete( gs->g3Dcamera[MAINCAMERA_ID] );

	Delete3Dobj( gs->sceneObj );
	Delete3Dmap( gs->sceneMap );

	GFL_G3D_SCENE_Delete( gs->g3Dscene );  

	GFL_G3D_UTIL_Delete( gs->g3Dutil );
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

GFL_PTC_PTR			Get_GS_Perticle( GAME_SYSTEM* gs )
{
	return gs->ptc;
}

SCENE_MAP*			Get_GS_SceneMap( GAME_SYSTEM* gs )
{
	return gs->sceneMap;
}

SCENE_OBJ*			Get_GS_SceneObj( GAME_SYSTEM* gs )
{
	return gs->sceneObj;
}
