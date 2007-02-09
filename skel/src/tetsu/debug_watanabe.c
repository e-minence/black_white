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
	TestModeWorkSet( GFL_HEAPID_APP );

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

	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &TestMainProcData, NULL);
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
	int						seq;

	u16						g3DresTblIdx;
	u16						g3DobjTblIdx;
	u16						g3DanmTblIdx;

	u16						work[16];
}TESTMODE_WORK;

TESTMODE_WORK* tetsuWork;

#include "sample_graphic/titledemo.naix"

//�a�f�ݒ�֐�
static void	bg_init( void );
static void	bg_exit( void );
//�R�c�a�f�쐬�֐�
static void	g3d_load( void );
static void g3d_draw( void );
static void	g3d_unload( void );
//�G�t�F�N�g
static void g3d_control_effect( void );

//------------------------------------------------------------------
/**
 * @brief	�f�[�^
 */
//------------------------------------------------------------------
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	

#define DTCM_SIZE		(0x1000)
#define G3D_FRM_PRI		(1)

#define G3D_UTIL_RESSIZ	(64)
#define G3D_UTIL_OBJSIZ	(64)
#define G3D_UTIL_ANMSIZ	(64)

//���\�[�X�ݒ�e�[�u��
enum {
	G3RES_AIR,
	G3RES_AIRANM,
	G3RES_IAR,
	G3RES_IARANM,
};

static const GFL_G3D_UTIL_RES g3DresouceTable[] = 
{
{(u32)"src/sample_graphic/titledemo.narc",NARC_titledemo_title_air_nsbmd,GFL_G3D_UTIL_RESPATH,TRUE},
{(u32)"src/sample_graphic/titledemo.narc",NARC_titledemo_title_air_nsbta,GFL_G3D_UTIL_RESPATH,0},
{(u32)"src/sample_graphic/titledemo.narc",NARC_titledemo_title_iar_nsbmd,GFL_G3D_UTIL_RESPATH,TRUE},
{(u32)"src/sample_graphic/titledemo.narc",NARC_titledemo_title_iar_nsbta,GFL_G3D_UTIL_RESPATH,0},
};

//�I�u�W�F�N�g�ݒ�e�[�u��
enum {
	G3OBJ_AIR,
	G3OBJ_IAR,
};

static const GFL_G3D_UTIL_OBJ g3DobjectTable[] = 
{
	{
		G3RES_AIR,0,G3RES_AIR,
		{ -FX32_ONE*64, 0, 0 },								//���W
		{ FX32_ONE*4/5, FX32_ONE*4/5, FX32_ONE*4/5 },		//�X�P�[��
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },	//��]
		0,TRUE,
	},
	{
		G3RES_IAR,0,G3RES_IAR,
		{ FX32_ONE*64, -FX32_ONE*48, 0 },					//���W
		{ FX32_ONE*3/5, FX32_ONE*3/5, FX32_ONE*3/5 },		//�X�P�[��
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },	//��]
		0,TRUE,
	},
};

//�A�j���[�V�����ݒ�e�[�u��
enum {
	G3ANM_AIR,
	G3ANM_IAR,
};

static const GFL_G3D_UTIL_ANM g3DanimetionTable[] = 
{
	{ G3RES_AIRANM, 0, G3OBJ_AIR, TRUE },
	{ G3RES_IARANM, 0, G3OBJ_IAR, TRUE },
};

static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 cameraPos		= { 0, (FX32_ONE * 58), (FX32_ONE * 256) };
static const VecFx32 cameraUp		= { 0, FX32_ONE, 0 };

#define cameraPerspway	( 0x0b60 )
#define cameraAspect	( FX32_ONE * 4 / 3 )
#define cameraNear		( 1 << FX32_SHIFT )
#define cameraFar		( 1024 << FX32_SHIFT )

#define g3DanmRotateSpeed	( 0x100 )
#define g3DanmFrameSpeed	( FX32_ONE )
	

//------------------------------------------------------------------
/**
 * @brief	���[�N�̊m�ۂƔj��
 */
//------------------------------------------------------------------
static void	TestModeWorkSet( HEAPID heapID )
{
	tetsuWork = GFL_HEAP_AllocMemoryClear( heapID, sizeof(TESTMODE_WORK) );
}

static void	TestModeWorkRelease( void )
{
	GFL_HEAP_FreeMemory( tetsuWork );
}

//------------------------------------------------------------------
/**
 * @brief	���X�g�I��
 */
//------------------------------------------------------------------
static BOOL	TestModeControl( void )
{
	BOOL return_flag = FALSE;
	int i;

	switch( tetsuWork->seq ){

	case 0:
		//������
		bg_init();
		tetsuWork->seq++;
		break;

	case 1:
		//��ʍ쐬
		g3d_load();	//�R�c�f�[�^�쐬
		tetsuWork->seq++;
		break;

	case 2:
		if( GFL_UI_KeyGetTrg() & PAD_BUTTON_R ){
			tetsuWork->seq++;
		}
		g3d_control_effect();
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
static void	bg_init( void )
{
	u16 heapID = GFL_HEAPID_APP;

	//�a�f�V�X�e���N��
	GFL_BG_sysInit( heapID );

	//�u�q�`�l�ݒ�
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G); 

	//�a�f���[�h�ݒ�
	GFL_BG_InitBG( &bgsysHeader );

	//�R�c�V�X�e���N��
	GFL_G3D_sysInit( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, heapID, NULL );
	GFL_BG_BGControlSet3D( G3D_FRM_PRI );
	GFL_G3D_UtilsysInit( G3D_UTIL_RESSIZ, G3D_UTIL_OBJSIZ, G3D_UTIL_ANMSIZ, heapID );  
}

static void	bg_exit( void )
{
	GFL_G3D_UtilsysExit();  
	GFL_G3D_sysExit();
	GFL_BG_sysExit();
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
//�쐬
static void g3d_load( void )
{
	//���\�[�X���I�u�W�F�N�g���A�j���[�V�������ꊇ�ݒ�
	GFL_G3D_UtilAllLoad( g3DresouceTable, NELEMS(g3DresouceTable), &tetsuWork->g3DresTblIdx,
						 g3DobjectTable, NELEMS(g3DobjectTable), &tetsuWork->g3DobjTblIdx,
						 g3DanimetionTable, NELEMS(g3DanimetionTable), &tetsuWork->g3DanmTblIdx );

	//�J�����Z�b�g
	GFL_G3D_sysProjectionSet(	GFL_G3D_PRJPERS, 
								FX_SinIdx( cameraPerspway ), FX_CosIdx( cameraPerspway ), 
								cameraAspect, 0, cameraNear, cameraFar, 0 );
	GFL_G3D_sysLookAtSet( (VecFx32*)&cameraPos, (VecFx32*)&cameraUp, (VecFx32*)&cameraTarget );

	tetsuWork->work[0] = 0;
}
	
//�`��
static void g3d_draw( void )
{
	GFL_G3D_UtilDraw();
}
	
//�j��
static void g3d_unload( void )
{
	//���\�[�X���I�u�W�F�N�g���A�j���[�V�������ꊇ�j��
	GFL_G3D_UtilAllUnload(	NELEMS(g3DresouceTable), &tetsuWork->g3DresTblIdx,
							NELEMS(g3DobjectTable), &tetsuWork->g3DobjTblIdx,
							NELEMS(g3DanimetionTable), &tetsuWork->g3DanmTblIdx );
}
	
//------------------------------------------------------------------
/**
 * @brief	�R�c����
 */
//------------------------------------------------------------------
static void g3d_control_effect( void )
{
	MtxFx33 rotate;
	VecFx32 rotate_tmp = { 0, 0, 0 };
	GFL_G3D_OBJ* g3Dobj;
	GFL_G3D_ANM* g3Danm;

	//��]�v�Z
	{
		//AIR�̃I�u�W�F�N�g�n���h�����擾
		g3Dobj = GFL_G3D_UtilObjGet( tetsuWork->g3DobjTblIdx + G3OBJ_AIR );
		//AIR�̃A�j���[�V�����n���h�����擾
		g3Danm = GFL_G3D_UtilAnmGet( tetsuWork->g3DanmTblIdx + G3ANM_AIR );

		rotate_tmp.y = g3DanmRotateSpeed * tetsuWork->work[0];	//�x����]
		GFL_G3D_UtilObjDrawRotateCalcYX( &rotate_tmp, &rotate );

		//AIR�̉�]�X�e�[�^�X���Z�b�g
		GFL_G3D_ObjContSetRotate( g3Dobj, &rotate );
		//�A�j���[�V�����R���g���[��
		GFL_G3D_ObjContAnmFrameAutoLoop( g3Danm, g3DanmFrameSpeed );
	}
	{
		//IAR�̃I�u�W�F�N�g�n���h�����擾
		g3Dobj = GFL_G3D_UtilObjGet( tetsuWork->g3DobjTblIdx + G3OBJ_IAR );
		//IAR�̃A�j���[�V�����n���h�����擾
		g3Danm = GFL_G3D_UtilAnmGet( tetsuWork->g3DanmTblIdx + G3ANM_IAR );

		rotate_tmp.y = -g3DanmRotateSpeed * tetsuWork->work[0];	//�x����]
		GFL_G3D_UtilObjDrawRotateCalcYX( &rotate_tmp, &rotate );

		//IAR�̉�]�X�e�[�^�X���Z�b�g
		GFL_G3D_ObjContSetRotate( g3Dobj, &rotate );
		//�A�j���[�V�����R���g���[��
		GFL_G3D_ObjContAnmFrameAutoLoop( g3Danm, g3DanmFrameSpeed );
	}
	tetsuWork->work[0]++;
}





