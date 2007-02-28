//============================================================================================
/**
 * @file	debug_watanabe.c
 * @brief	�f�o�b�O���e�X�g�p�A�v���i�n�ӂ���p�j
 * @author	tamada
 * @date	2007.02.01
 */
//============================================================================================
#include "gflib.h"
#include "textprint.h"
#include "g3d_scene.h"
#include "g3d_camera.h"

#include "main.h"
#include "testmode.h"

static void	TestModeWorkSet( HEAPID heapID );
static void	TestModeWorkRelease( void );
static BOOL	TestModeControl( void );
//============================================================================================
//
//
//		�v���Z�X�̒�`
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̏�����
 *
 * �����Ńq�[�v�̐�����e�평�����������s���B
 * �����i�K�ł�mywk��NULL�����AGFL_PROC_AllocWork���g�p�����
 * �ȍ~�͊m�ۂ������[�N�̃A�h���X�ƂȂ�B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugWatanabeMainProcInit
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WATANABE_DEBUG, 0x20000 );
	TestModeWorkSet( HEAPID_WATANABE_DEBUG );
//	TestModeWorkSet( GFL_HEAPID_APP );

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̃��C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugWatanabeMainProcMain
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	if( TestModeControl() == TRUE ){
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̏I������
 *
 * �P�ɏI�������ꍇ�A�e�v���Z�X�ɏ������Ԃ�B
 * GFL_PROC_SysSetNextProc���Ă�ł����ƁA�I���セ�̃v���Z�X��
 * �������J�ڂ���B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugWatanabeMainProcEnd
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TestModeWorkRelease();
	GFL_HEAP_DeleteHeap( HEAPID_WATANABE_DEBUG );

//	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &TestMainProcData, NULL);
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA DebugWatanabeMainProcData = {
	DebugWatanabeMainProcInit,
	DebugWatanabeMainProcMain,
	DebugWatanabeMainProcEnd,
};




//============================================================================================
//
//
//	���C���R���g���[��
//
//
//============================================================================================
typedef struct {
	HEAPID					heapID;
	int						seq;

	GFL_G3D_UTIL*			g3Dutil;
	GFL_G3D_OBJSTATUS		g3DobjStatus[16];
	GFL_G3D_SCENE*			g3Dscene;
	u32						g3DsceneObjID;
	GFL_G3D_CAMERA*			g3Dcamera;

	u16						work[16];
}TETSU_WORK;

TETSU_WORK* tetsuWork;

typedef struct {
	int		seq;

	VecFx32	rotateTmp;
}BALL_WORK;

#include "sample_graphic/test9ball.naix"

//�a�f�ݒ�֐�
static void	bg_init( HEAPID heapID );
static void	bg_exit( void );
//�R�c�a�f�쐬�֐�
static void G3DsysSetup( void );
static void g3d_load( HEAPID heapID );
static void g3d_move( void );
static void g3d_draw( void );
static void	g3d_unload( void );

static inline void rotateCalc( VecFx32* rotSrc, MtxFx33* rotDst );
static void ball_rotateX( GFL_G3D_SCENEOBJ* sceneObj, void* work );
static void ball_rotateY( GFL_G3D_SCENEOBJ* sceneObj, void* work );
static void ball_rotateZ( GFL_G3D_SCENEOBJ* sceneObj, void* work );

static void SceneObjTransAddAll( GFL_G3D_SCENE* g3Dscene, VecFx32* trans );
//------------------------------------------------------------------
/**
 * @brief	�f�[�^
 */
//------------------------------------------------------------------
#include "debug_watanabe.dat"

//------------------------------------------------------------------
/**
 * @brief	���[�N�̊m�ۂƔj��
 */
//------------------------------------------------------------------
static void	TestModeWorkSet( HEAPID heapID )
{
	tetsuWork = GFL_HEAP_AllocMemoryClear( heapID, sizeof(TETSU_WORK) );
	tetsuWork->heapID = heapID;
}

static void	TestModeWorkRelease( void )
{
	GFL_HEAP_FreeMemory( tetsuWork );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
//------------------------------------------------------------------
static BOOL	TestModeControl( void )
{
	BOOL return_flag = FALSE;
	int i;

	switch( tetsuWork->seq ){

	case 0:
		//������
		GFL_STD_MTRandInit(0);
		bg_init( tetsuWork->heapID );
		tetsuWork->seq++;
		break;

	case 1:
		//��ʍ쐬
		g3d_load( tetsuWork->heapID );	//�R�c�f�[�^�쐬
		tetsuWork->work[0] = 0;
		tetsuWork->work[1] = 0;
		tetsuWork->seq++;
		break;

	case 2:
		if( GFL_UI_KeyGetTrg() & PAD_BUTTON_R ){
			tetsuWork->seq++;
		} else if( GFL_UI_KeyGetCont() & PAD_KEY_LEFT ){
			VecFx32 trans = {  FX32_ONE, 0, 0 };
			SceneObjTransAddAll( tetsuWork->g3Dscene, &trans );
		} else if( GFL_UI_KeyGetCont() & PAD_KEY_RIGHT ){
			VecFx32 trans = { -FX32_ONE, 0, 0 };
			SceneObjTransAddAll( tetsuWork->g3Dscene, &trans );
		} else if( GFL_UI_KeyGetCont() & PAD_KEY_UP ){
			VecFx32 trans = { 0, 0,  FX32_ONE };
			SceneObjTransAddAll( tetsuWork->g3Dscene, &trans );
		} else if( GFL_UI_KeyGetCont() & PAD_KEY_DOWN ){
			VecFx32 trans = { 0, 0, -FX32_ONE };
			SceneObjTransAddAll( tetsuWork->g3Dscene, &trans );
		}
		if( GFL_UI_KeyGetTrg() & PAD_BUTTON_A ){
			GFL_G3D_CameraOn( tetsuWork->g3Dcamera );
		}
		g3d_move();
		tetsuWork->work[0]++;
		g3d_draw();		//�R�c�f�[�^�`��
		break;

	case 3:
		//�I��
		g3d_unload();	//�R�c�f�[�^�j��
		bg_exit();
		return_flag = TRUE;
		break;
	}
	return return_flag;
}

//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ聕�f�[�^�]��
 */
//------------------------------------------------------------------
static void	bg_init( HEAPID heapID )
{
	//�a�f�V�X�e���N��
	GFL_BG_sysInit( heapID );

	//�u�q�`�l�ݒ�
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G); 

	//�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocMemoryLowClear( heapID, 16*2 );
		plt[0] = GX_RGB( 8, 15, 8);
		GFL_BG_PaletteSet( GFL_BG_FRAME0_M, plt, 16*2, 0 );
		GFL_BG_PaletteSet( GFL_BG_FRAME1_M, plt, 16*2, 0 );

		GFL_HEAP_FreeMemory( plt );
	}

	//�a�f���[�h�ݒ�
	GFL_BG_InitBG( &bgsysHeader );

	//�R�c�V�X�e���N��
	GFL_G3D_sysInit( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, heapID, G3DsysSetup );
	GFL_BG_BGControlSet3D( G3D_FRM_PRI );

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_DispSelect( GFL_DISP_3D_TO_MAIN );
}

static void	bg_exit( void )
{
	GFL_G3D_sysExit();
	GFL_BG_sysExit();
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
//�쐬
static void g3d_load( HEAPID heapID )
{
	int	i;

	tetsuWork->g3Dutil = GFL_G3D_UtilsysCreate( &g3Dutil_setup, heapID );
	tetsuWork->g3Dscene = GFL_G3D_SceneCreate( tetsuWork->g3Dutil, NELEMS(g3DsceneObjData)+1,
												sizeof(BALL_WORK), heapID );
	tetsuWork->g3DsceneObjID = GFL_G3D_SceneObjAdd
								( tetsuWork->g3Dscene, g3DsceneObjData, NELEMS(g3DsceneObjData) );
	tetsuWork->g3Dcamera = GFL_G3D_CameraDefaultCreate( &cameraPos, &cameraTarget, heapID );
	GFL_G3D_sysLightSet( 0, &light0Vec, 0x7fff );
}
	
//����
static void g3d_move( void )
{
	GFL_G3D_SceneMain( tetsuWork->g3Dscene );  
}

//�`��
static void g3d_draw( void )
{
	GFL_G3D_SceneDraw( tetsuWork->g3Dscene );  
}
	
//�j��
static void g3d_unload( void )
{
	GFL_G3D_CameraDelete( tetsuWork->g3Dcamera );
	GFL_G3D_SceneObjDel( tetsuWork->g3Dscene, tetsuWork->g3DsceneObjID, NELEMS(g3DsceneObjData) );
	GFL_G3D_SceneDelete( tetsuWork->g3Dscene );  

	GFL_G3D_UtilsysDelete( tetsuWork->g3Dutil );
}
	
//------------------------------------------------------------------
/**
 * @brief	�R�c����
 */
//------------------------------------------------------------------
static inline void rotateCalc( VecFx32* rotSrc, MtxFx33* rotDst )
{
	MtxFx33 tmp;

	MTX_RotX33(	rotDst, FX_SinIdx((u16)rotSrc->x), FX_CosIdx((u16)rotSrc->x) );

	MTX_RotY33(	&tmp, FX_SinIdx((u16)rotSrc->y), FX_CosIdx((u16)rotSrc->y) );
	MTX_Concat33( rotDst, &tmp, rotDst );

	MTX_RotZ33(	&tmp, FX_SinIdx((u16)rotSrc->z), FX_CosIdx((u16)rotSrc->z) );
	MTX_Concat33( rotDst, &tmp, rotDst );
}

static inline void drawSWset( GFL_G3D_SCENEOBJ* sceneObj, BOOL sw )
{
	BOOL swBuf = sw;
	GFL_G3D_SceneObjDrawSWSet( sceneObj, &swBuf );
}

static void ball_rotateX( GFL_G3D_SCENEOBJ* sceneObj, void* work )
{
	MtxFx33 rotate;
	BALL_WORK* ballWk = (BALL_WORK*)work;

	switch( ballWk->seq ){
		case 0:
			{
				u16 val;
				GFL_G3D_SceneObjIDGet( sceneObj, &val );
				ballWk->rotateTmp.x = g3DanmRotateSpeed * val;
				ballWk->rotateTmp.y = g3DanmRotateSpeed * val;
				ballWk->rotateTmp.z = g3DanmRotateSpeed * val;
			}
			drawSWset( sceneObj, TRUE );
			ballWk->seq++;
			break;
		case 1:
			ballWk->rotateTmp.x += g3DanmRotateSpeed;
			rotateCalc( &ballWk->rotateTmp, &rotate );
			GFL_G3D_SceneObjStatusRotateSet( sceneObj, &rotate );
			break;
	}
}

static void ball_rotateY( GFL_G3D_SCENEOBJ* sceneObj, void* work )
{
	MtxFx33 rotate;
	BALL_WORK* ballWk = (BALL_WORK*)work;

	switch( ballWk->seq ){
		case 0:
			{
				u16 val;
				GFL_G3D_SceneObjIDGet( sceneObj, &val );
				ballWk->rotateTmp.x = g3DanmRotateSpeed * val;
				ballWk->rotateTmp.y = g3DanmRotateSpeed * val;
				ballWk->rotateTmp.z = g3DanmRotateSpeed * val;
			}
			drawSWset( sceneObj, TRUE );
			ballWk->seq++;
			break;
		case 1:
			ballWk->rotateTmp.y += g3DanmRotateSpeed;
			rotateCalc( &ballWk->rotateTmp, &rotate );
			GFL_G3D_SceneObjStatusRotateSet( sceneObj, &rotate );
			break;
	}
}

static void ball_rotateZ( GFL_G3D_SCENEOBJ* sceneObj, void* work )
{
	MtxFx33 rotate;
	BALL_WORK* ballWk = (BALL_WORK*)work;

	switch( ballWk->seq ){
		case 0:
			{
				u16 val;
				GFL_G3D_SceneObjIDGet( sceneObj, &val );
				ballWk->rotateTmp.x = g3DanmRotateSpeed * val;
				ballWk->rotateTmp.y = g3DanmRotateSpeed * val;
				ballWk->rotateTmp.z = g3DanmRotateSpeed * val;
			}
			drawSWset( sceneObj, TRUE );
			ballWk->seq++;
			break;
		case 1:
			ballWk->rotateTmp.z += g3DanmRotateSpeed;
			rotateCalc( &ballWk->rotateTmp, &rotate );
			GFL_G3D_SceneObjStatusRotateSet( sceneObj, &rotate );
			break;
	}
}

static void SceneObjTransAddAll( GFL_G3D_SCENE* g3Dscene, VecFx32* trans )
{
	GFL_G3D_SCENEOBJ*	g3DsceneObj;
	VecFx32				tmp;
	int					i;
	u16					idx = tetsuWork->g3DsceneObjID;

	for( i=0; i<NELEMS(g3DsceneObjData); i++ ){
		g3DsceneObj = GFL_G3D_SceneObjGet( g3Dscene, idx );
		GFL_G3D_SceneObjStatusTransGet( g3DsceneObj, &tmp );
		tmp.x += trans->x;
		tmp.y += trans->y;
		tmp.z += trans->z;
		GFL_G3D_SceneObjStatusTransSet( g3DsceneObj, &tmp );
		idx++;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static void G3DsysSetup( void )
{
	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_TOON );
	G3X_AntiAlias( FALSE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( FALSE );		// �A���t�@�u�����h�@�I��
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);
}




