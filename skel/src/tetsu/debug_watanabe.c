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

//static void	TestModeWorkSet( HEAPID heapID );
//static void	TestModeWorkRelease( void );
//static BOOL	TestModeControl( void );
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
//	if( TestModeControl() == TRUE ){
		return GFL_PROC_RES_FINISH;
//	}

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
//	TestModeWorkRelease();

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




#if 0

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

#include "sample_graphic/test9ball.naix"

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
	G3RES_1BALLMDL = 0,
	G3RES_2BALLMDL,
	G3RES_3BALLMDL,
	G3RES_4BALLMDL,
	G3RES_5BALLMDL,
	G3RES_6BALLMDL,
	G3RES_7BALLMDL,
	G3RES_8BALLMDL,
	G3RES_9BALLMDL,
	G3RES_1BALL,
	G3RES_2BALL,
	G3RES_3BALL,
	G3RES_4BALL,
	G3RES_5BALL,
	G3RES_6BALL,
	G3RES_7BALL,
	G3RES_8BALL,
	G3RES_9BALL,
};

static const char g3Ddata_path[] = {"src/sample_graphic/test9ball.narc"};

static const GFL_G3D_UTIL_RES g3DresouceTable[] = 
{
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH, FALSE },
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH, FALSE },
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH, FALSE },
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH, FALSE },
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH, FALSE },
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH, FALSE },
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH, FALSE },
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH, FALSE },
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH, FALSE },
{ (u32)g3Ddata_path, NARC_test9ball_test1ball_nsbtx, GFL_G3D_UTIL_RESPATH, TRUE },
{ (u32)g3Ddata_path, NARC_test9ball_test2ball_nsbtx, GFL_G3D_UTIL_RESPATH, TRUE },
{ (u32)g3Ddata_path, NARC_test9ball_test3ball_nsbtx, GFL_G3D_UTIL_RESPATH, TRUE },
{ (u32)g3Ddata_path, NARC_test9ball_test4ball_nsbtx, GFL_G3D_UTIL_RESPATH, TRUE },
{ (u32)g3Ddata_path, NARC_test9ball_test5ball_nsbtx, GFL_G3D_UTIL_RESPATH, TRUE },
{ (u32)g3Ddata_path, NARC_test9ball_test6ball_nsbtx, GFL_G3D_UTIL_RESPATH, TRUE },
{ (u32)g3Ddata_path, NARC_test9ball_test7ball_nsbtx, GFL_G3D_UTIL_RESPATH, TRUE },
{ (u32)g3Ddata_path, NARC_test9ball_test8ball_nsbtx, GFL_G3D_UTIL_RESPATH, TRUE },
{ (u32)g3Ddata_path, NARC_test9ball_test9ball_nsbtx, GFL_G3D_UTIL_RESPATH, TRUE },
};

//�I�u�W�F�N�g�ݒ�e�[�u��
enum {
	G3OBJ_1BALL = 0,
	G3OBJ_2BALL,
	G3OBJ_3BALL,
	G3OBJ_4BALL,
	G3OBJ_5BALL,
	G3OBJ_6BALL,
	G3OBJ_7BALL,
	G3OBJ_8BALL,
	G3OBJ_9BALL,
};

#define UL_OFFSX (-FX32_ONE*3*32)
#define UL_OFFSZ (-FX32_ONE*3*32)
#define UP_OFFSX ( FX32_ONE*0*32)
#define UP_OFFSZ (-FX32_ONE*3*32)
#define UR_OFFSX ( FX32_ONE*3*32)
#define UR_OFFSZ (-FX32_ONE*3*32)

#define LF_OFFSX (-FX32_ONE*3*32)
#define LF_OFFSZ ( FX32_ONE*0*32)
#define CN_OFFSX ( FX32_ONE*0*32)
#define CN_OFFSZ ( FX32_ONE*0*32)
#define RG_OFFSX ( FX32_ONE*3*32)
#define RG_OFFSZ ( FX32_ONE*0*32)

#define DL_OFFSX (-FX32_ONE*3*32)
#define DL_OFFSZ ( FX32_ONE*3*32)
#define DN_OFFSX ( FX32_ONE*0*32)
#define DN_OFFSZ ( FX32_ONE*3*32)
#define DR_OFFSX ( FX32_ONE*3*32)
#define DR_OFFSZ ( FX32_ONE*3*32)

static const GFL_G3D_UTIL_OBJ g3DobjectTable[] = 
{
#if 0
	{ G3RES_9BALLMDL,0,G3RES_9BALL,
		{ -FX32_ONE*32, 0, -FX32_ONE*32 }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_8BALLMDL,0,G3RES_8BALL,
		{ -FX32_ONE*32, 0,  FX32_ONE* 0 }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_7BALLMDL,0,G3RES_7BALL,
		{ -FX32_ONE*32, 0,  FX32_ONE*32 }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_6BALLMDL,0,G3RES_6BALL,
		{ -FX32_ONE* 0, 0, -FX32_ONE*32 }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_5BALLMDL,0,G3RES_5BALL,
		{  FX32_ONE* 0, 0,  FX32_ONE* 0 }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_4BALLMDL,0,G3RES_4BALL,
		{  FX32_ONE* 0, 0,  FX32_ONE*32 }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_3BALLMDL,0,G3RES_3BALL,
		{  FX32_ONE*32, 0, -FX32_ONE*32 }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_2BALLMDL,0,G3RES_2BALL,
		{  FX32_ONE*32, 0,  FX32_ONE* 0 }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_1BALLMDL,0,G3RES_1BALL,
		{  FX32_ONE*32, 0,  FX32_ONE*32 }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
#else
	{ G3RES_9BALLMDL,0,G3RES_9BALL,
		{ -FX32_ONE*32 + UP_OFFSX, 0, -FX32_ONE*32 + UP_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_8BALLMDL,0,G3RES_8BALL,
		{ -FX32_ONE*32 + UP_OFFSX, 0,  FX32_ONE* 0 + UP_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_7BALLMDL,0,G3RES_7BALL,
		{ -FX32_ONE*32 + UP_OFFSX, 0,  FX32_ONE*32 + UP_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_6BALLMDL,0,G3RES_6BALL,
		{ -FX32_ONE* 0 + UP_OFFSX, 0, -FX32_ONE*32 + UP_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_5BALLMDL,0,G3RES_5BALL,
		{  FX32_ONE* 0 + UP_OFFSX, 0,  FX32_ONE* 0 + UP_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_4BALLMDL,0,G3RES_4BALL,
		{  FX32_ONE* 0 + UP_OFFSX, 0,  FX32_ONE*32 + UP_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_3BALLMDL,0,G3RES_3BALL,
		{  FX32_ONE*32 + UP_OFFSX, 0, -FX32_ONE*32 + UP_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_2BALLMDL,0,G3RES_2BALL,
		{  FX32_ONE*32 + UP_OFFSX, 0,  FX32_ONE* 0 + UP_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_1BALLMDL,0,G3RES_1BALL,
		{  FX32_ONE*32 + UP_OFFSX, 0,  FX32_ONE*32 + UP_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },

	{ G3RES_9BALLMDL,0,G3RES_9BALL,
		{ -FX32_ONE*32 + LF_OFFSX, 0, -FX32_ONE*32 + LF_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_8BALLMDL,0,G3RES_8BALL,
		{ -FX32_ONE*32 + LF_OFFSX, 0,  FX32_ONE* 0 + LF_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_7BALLMDL,0,G3RES_7BALL,
		{ -FX32_ONE*32 + LF_OFFSX, 0,  FX32_ONE*32 + LF_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_6BALLMDL,0,G3RES_6BALL,
		{ -FX32_ONE* 0 + LF_OFFSX, 0, -FX32_ONE*32 + LF_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_5BALLMDL,0,G3RES_5BALL,
		{  FX32_ONE* 0 + LF_OFFSX, 0,  FX32_ONE* 0 + LF_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_4BALLMDL,0,G3RES_4BALL,
		{  FX32_ONE* 0 + LF_OFFSX, 0,  FX32_ONE*32 + LF_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_3BALLMDL,0,G3RES_3BALL,
		{  FX32_ONE*32 + LF_OFFSX, 0, -FX32_ONE*32 + LF_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_2BALLMDL,0,G3RES_2BALL,
		{  FX32_ONE*32 + LF_OFFSX, 0,  FX32_ONE* 0 + LF_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_1BALLMDL,0,G3RES_1BALL,
		{  FX32_ONE*32 + LF_OFFSX, 0,  FX32_ONE*32 + LF_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },

	{ G3RES_9BALLMDL,0,G3RES_9BALL,
		{ -FX32_ONE*32 + RG_OFFSX, 0, -FX32_ONE*32 + RG_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_8BALLMDL,0,G3RES_8BALL,
		{ -FX32_ONE*32 + RG_OFFSX, 0,  FX32_ONE* 0 + RG_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_7BALLMDL,0,G3RES_7BALL,
		{ -FX32_ONE*32 + RG_OFFSX, 0,  FX32_ONE*32 + RG_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_6BALLMDL,0,G3RES_6BALL,
		{ -FX32_ONE* 0 + RG_OFFSX, 0, -FX32_ONE*32 + RG_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_5BALLMDL,0,G3RES_5BALL,
		{  FX32_ONE* 0 + RG_OFFSX, 0,  FX32_ONE* 0 + RG_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_4BALLMDL,0,G3RES_4BALL,
		{  FX32_ONE* 0 + RG_OFFSX, 0,  FX32_ONE*32 + RG_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_3BALLMDL,0,G3RES_3BALL,
		{  FX32_ONE*32 + RG_OFFSX, 0, -FX32_ONE*32 + RG_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_2BALLMDL,0,G3RES_2BALL,
		{  FX32_ONE*32 + RG_OFFSX, 0,  FX32_ONE* 0 + RG_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_1BALLMDL,0,G3RES_1BALL,
		{  FX32_ONE*32 + RG_OFFSX, 0,  FX32_ONE*32 + RG_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },

	{ G3RES_9BALLMDL,0,G3RES_9BALL,
		{ -FX32_ONE*32 + DN_OFFSX, 0, -FX32_ONE*32 + DN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_8BALLMDL,0,G3RES_8BALL,
		{ -FX32_ONE*32 + DN_OFFSX, 0,  FX32_ONE* 0 + DN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_7BALLMDL,0,G3RES_7BALL,
		{ -FX32_ONE*32 + DN_OFFSX, 0,  FX32_ONE*32 + DN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_6BALLMDL,0,G3RES_6BALL,
		{ -FX32_ONE* 0 + DN_OFFSX, 0, -FX32_ONE*32 + DN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_5BALLMDL,0,G3RES_5BALL,
		{  FX32_ONE* 0 + DN_OFFSX, 0,  FX32_ONE* 0 + DN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_4BALLMDL,0,G3RES_4BALL,
		{  FX32_ONE* 0 + DN_OFFSX, 0,  FX32_ONE*32 + DN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_3BALLMDL,0,G3RES_3BALL,
		{  FX32_ONE*32 + DN_OFFSX, 0, -FX32_ONE*32 + DN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_2BALLMDL,0,G3RES_2BALL,
		{  FX32_ONE*32 + DN_OFFSX, 0,  FX32_ONE* 0 + DN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
	{ G3RES_1BALLMDL,0,G3RES_1BALL,
		{  FX32_ONE*32 + DN_OFFSX, 0,  FX32_ONE*32 + DN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },

	{ G3RES_9BALLMDL,0,G3RES_9BALL,
		{  FX32_ONE* 0 + CN_OFFSX, 0,  FX32_ONE* 0 + CN_OFFSZ }, { FX32_ONE, FX32_ONE, FX32_ONE },
		{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE }, 0, TRUE },
#endif
};

#if 0
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
#endif

static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 cameraPos		= { 0, (FX32_ONE * 512), (FX32_ONE * 300) };
static const VecFx32 cameraUp		= { 0, FX32_ONE, 0 };

#define cameraPerspway	( 0x0b60 )
#define cameraAspect	( FX32_ONE * 4 / 3 )
#define cameraNear		( 1 << FX32_SHIFT )
#define cameraFar		( 1024 << FX32_SHIFT )

#define g3DanmRotateSpeed	( 0x100 )
#define g3DanmFrameSpeed	( FX32_ONE )
	
static const VecFx32 light0Vec		= { -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) };
static const VecFx32 light1Vec		= {  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) };


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
GFL_G3D_RES* g3Dres[8];
GFL_G3D_OBJ* g3Dobj[8];

//�쐬
static void g3d_load( void )
{
	//���\�[�X���I�u�W�F�N�g���A�j���[�V�������ꊇ�ݒ�
	tetsuWork->g3DresTblIdx = GFL_G3D_UtilResLoad( g3DresouceTable, NELEMS(g3DresouceTable) );
	tetsuWork->g3DobjTblIdx = GFL_G3D_UtilObjLoad( g3DobjectTable, NELEMS(g3DobjectTable) );
	//�J�����Z�b�g
	GFL_G3D_sysProjectionSet(	GFL_G3D_PRJPERS, 
								FX_SinIdx( cameraPerspway ), FX_CosIdx( cameraPerspway ), 
								cameraAspect, 0, cameraNear, cameraFar, 0 );
	GFL_G3D_sysLookAtSet( (VecFx32*)&cameraPos, (VecFx32*)&cameraUp, (VecFx32*)&cameraTarget );

	GFL_G3D_sysLightSet( 0, (VecFx16*)&light0Vec, 0x7fff );
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
	GFL_G3D_UtilObjUnload( tetsuWork->g3DresTblIdx, NELEMS(g3DobjectTable) );
	GFL_G3D_UtilResUnload( tetsuWork->g3DobjTblIdx, NELEMS(g3DresouceTable) );
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

	//��]�v�Z
	{
		int i;

		for( i=0; i<NELEMS(g3DobjectTable); i++ ){
			//�I�u�W�F�N�g�n���h�����擾
			g3Dobj = GFL_G3D_UtilObjGet( tetsuWork->g3DobjTblIdx + i );

			rotate_tmp.y = g3DanmRotateSpeed * tetsuWork->work[0] * (i+1);	//�x����]
			GFL_G3D_UtilObjDrawRotateCalcYX( &rotate_tmp, &rotate );
			GFL_G3D_ObjContSetRotate( g3Dobj, &rotate );
		}
	}
	tetsuWork->work[0]++;
}





#endif
