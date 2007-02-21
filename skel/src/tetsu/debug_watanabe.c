//============================================================================================
/**
 * @file	debug_watanabe.c
 * @brief	デバッグ＆テスト用アプリ（渡辺さん用）
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
//		プロセスの定義
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	プロセスの初期化
 *
 * ここでヒープの生成や各種初期化処理を行う。
 * 初期段階ではmywkはNULLだが、GFL_PROC_AllocWorkを使用すると
 * 以降は確保したワークのアドレスとなる。
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
 * @brief	プロセスのメイン
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
 * @brief	プロセスの終了処理
 *
 * 単に終了した場合、親プロセスに処理が返る。
 * GFL_PROC_SysSetNextProcを呼んでおくと、終了後そのプロセスに
 * 処理が遷移する。
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
//	メインコントロール
//
//
//============================================================================================
typedef struct {
	int						seq;

	GFL_G3D_UTIL_SCENE*		g3Dscene;
	GFL_G3D_OBJSTATUS		g3DobjStatus[16];

	u16						work[16];
}TETSU_WORK;

TETSU_WORK* tetsuWork;

#include "sample_graphic/test9ball.naix"

//ＢＧ設定関数
static void	bg_init( void );
static void	bg_exit( void );
//３ＤＢＧ作成関数
static void	g3d_load( void );
static void g3d_draw( void );
static void	g3d_unload( void );
//エフェクト
static void g3d_control_effect( void );

//------------------------------------------------------------------
/**
 * @brief	データ
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

static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 cameraPos		= { 0, (FX32_ONE * 256), (FX32_ONE * 256) };
static const VecFx32 cameraUp		= { 0, FX32_ONE, 0 };

#define cameraPerspway	( 0x0b60 )
#define cameraAspect	( FX32_ONE * 4 / 3 )
#define cameraNear		( 1 << FX32_SHIFT )
#define cameraFar		( 1024 << FX32_SHIFT )

#define g3DanmRotateSpeed	( 0x100 )
#define g3DanmFrameSpeed	( FX32_ONE )
	
static const VecFx32 light0Vec		= { -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) };
static const VecFx32 light1Vec		= {  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) };

static const char g3DarcPath[] = {"src/sample_graphic/test9ball.narc"};

enum {
	G3DRES_BALL_BMD = 0,
	G3DRES_BALL1_TEX,
	G3DRES_BALL2_TEX,
	G3DRES_BALL3_TEX,
	G3DRES_BALL4_TEX,
	G3DRES_BALL5_TEX,
	G3DRES_BALL6_TEX,
	G3DRES_BALL7_TEX,
	G3DRES_BALL8_TEX,
	G3DRES_BALL9_TEX,
};

static const GFL_G3D_UTIL_SCENE_RES g3Dscene_resTbl[] = {
	{ (u32)g3DarcPath, NARC_test9ball_test1ball_nsbmd, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_test9ball_test1ball_nsbtx, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_test9ball_test2ball_nsbtx, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_test9ball_test3ball_nsbtx, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_test9ball_test4ball_nsbtx, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_test9ball_test5ball_nsbtx, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_test9ball_test6ball_nsbtx, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_test9ball_test7ball_nsbtx, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_test9ball_test8ball_nsbtx, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_test9ball_test9ball_nsbtx, GFL_G3D_UTIL_RESPATH },
};

static const GFL_G3D_UTIL_SCENE_OBJ g3Dscene_objTbl[] = {
	{ G3DRES_BALL_BMD, 0, G3DRES_BALL1_TEX, NULL, 0 },
	{ G3DRES_BALL_BMD, 0, G3DRES_BALL2_TEX, NULL, 0 },
	{ G3DRES_BALL_BMD, 0, G3DRES_BALL3_TEX, NULL, 0 },
	{ G3DRES_BALL_BMD, 0, G3DRES_BALL4_TEX, NULL, 0 },
	{ G3DRES_BALL_BMD, 0, G3DRES_BALL5_TEX, NULL, 0 },
	{ G3DRES_BALL_BMD, 0, G3DRES_BALL6_TEX, NULL, 0 },
	{ G3DRES_BALL_BMD, 0, G3DRES_BALL7_TEX, NULL, 0 },
	{ G3DRES_BALL_BMD, 0, G3DRES_BALL8_TEX, NULL, 0 },
	{ G3DRES_BALL_BMD, 0, G3DRES_BALL9_TEX, NULL, 0 },
};

static const GFL_G3D_UTIL_SCENE_SETUP g3Dscene_setup = {
	g3Dscene_resTbl, NELEMS(g3Dscene_resTbl),
	g3Dscene_objTbl, NELEMS(g3Dscene_objTbl),
};

static const GFL_G3D_OBJSTATUS statusDefault = {
	{0,0,0},{FX32_ONE,FX32_ONE,FX32_ONE},{FX32_ONE,0,0,0,FX32_ONE,0,0,0,FX32_ONE},
};

static const VecFx32 g3DobjectTransTable[] = 
{
	{ -FX32_ONE*48, 0, -FX32_ONE*48 }, 
	{ -FX32_ONE*48, 0,  FX32_ONE* 0 },
	{ -FX32_ONE*48, 0,  FX32_ONE*48 },
	{ -FX32_ONE* 0, 0, -FX32_ONE*48 },
	{  FX32_ONE* 0, 0,  FX32_ONE* 0 },
	{  FX32_ONE* 0, 0,  FX32_ONE*48 },
	{  FX32_ONE*48, 0, -FX32_ONE*48 },
	{  FX32_ONE*48, 0,  FX32_ONE* 0 },
	{  FX32_ONE*48, 0,  FX32_ONE*48 },
};

//------------------------------------------------------------------
/**
 * @brief	ワークの確保と破棄
 */
//------------------------------------------------------------------
static void	TestModeWorkSet( HEAPID heapID )
{
	tetsuWork = GFL_HEAP_AllocMemoryClear( heapID, sizeof(TETSU_WORK) );
}

static void	TestModeWorkRelease( void )
{
	GFL_HEAP_FreeMemory( tetsuWork );
}

//------------------------------------------------------------------
/**
 * @brief	メイン
 */
//------------------------------------------------------------------
static BOOL	TestModeControl( void )
{
	BOOL return_flag = FALSE;
	int i;

	switch( tetsuWork->seq ){

	case 0:
		//初期化
		bg_init();
		tetsuWork->seq++;
		break;

	case 1:
		//画面作成
		g3d_load();	//３Ｄデータ作成
		tetsuWork->seq++;
		break;

	case 2:
		if( GFL_UI_KeyGetTrg() & PAD_BUTTON_R ){
			tetsuWork->seq++;
		}
		g3d_control_effect();
		g3d_draw();		//３Ｄデータ描画
		break;

	case 3:
		//終了
		g3d_unload();	//３Ｄデータ破棄
		bg_exit();
		return_flag = TRUE;
		break;
	}
	return return_flag;
}

//------------------------------------------------------------------
/**
 * @brief		ＢＧ設定＆データ転送
 */
//------------------------------------------------------------------
static void	bg_init( void )
{
	u16 heapID = GFL_HEAPID_APP;

	//ＢＧシステム起動
	GFL_BG_sysInit( heapID );

	//ＶＲＡＭ設定
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G); 

	//パレット作成＆転送
	{
		u16* plt = GFL_HEAP_AllocMemoryLowClear( heapID, 16*2 );
		plt[0] = GX_RGB( 8, 15, 8);
		GFL_BG_PaletteSet( GFL_BG_FRAME0_M, plt, 16*2, 0 );
		GFL_BG_PaletteSet( GFL_BG_FRAME1_M, plt, 16*2, 0 );

		GFL_HEAP_FreeMemory( plt );
	}

	//ＢＧモード設定
	GFL_BG_InitBG( &bgsysHeader );

	//３Ｄシステム起動
	GFL_G3D_sysInit( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, heapID, NULL );
	GFL_BG_BGControlSet3D( G3D_FRM_PRI );
}

static void	bg_exit( void )
{
	GFL_G3D_sysExit();
	GFL_BG_sysExit();
}

//------------------------------------------------------------------
/**
 * @brief		３Ｄデータコントロール
 */
//------------------------------------------------------------------
//作成
static void g3d_load( void )
{
	u16 heapID = GFL_HEAPID_APP;
	int	i;

	tetsuWork->g3Dscene = GFL_G3D_UtilsysCreate( &g3Dscene_setup, heapID );
	//描画ステータスワーク設定
	for( i=0; i<NELEMS(g3Dscene_objTbl); i++ ){
		tetsuWork->g3DobjStatus[i] = statusDefault;
		tetsuWork->g3DobjStatus[i].trans = g3DobjectTransTable[i];
	}
	//カメラセット
	GFL_G3D_sysProjectionSet(	GFL_G3D_PRJPERS, 
								FX_SinIdx( cameraPerspway ), FX_CosIdx( cameraPerspway ), 
								cameraAspect, 0, cameraNear, cameraFar, 0 );
	GFL_G3D_sysLookAtSet( (VecFx32*)&cameraPos, (VecFx32*)&cameraUp, (VecFx32*)&cameraTarget );
	GFL_G3D_sysLightSet( 0, (VecFx16*)&light0Vec, 0x7fff );

	tetsuWork->work[0] = 0;
}
	
//描画
static void g3d_draw( void )
{
	GFL_G3D_OBJ* g3Dobj;
	int	i;

	GFL_G3D_DrawStart();
	GFL_G3D_DrawLookAt();

	for( i=0; i<NELEMS(g3Dscene_objTbl); i++ ){
		g3Dobj = GFL_G3D_UtilsysObjHandleGet( tetsuWork->g3Dscene, i );
		GFL_G3D_ObjDraw( g3Dobj, &tetsuWork->g3DobjStatus[i] );
	}
	GFL_G3D_DrawEnd();
}
	
//破棄
static void g3d_unload( void )
{
	GFL_G3D_UtilsysDelete( tetsuWork->g3Dscene );
}
	
//------------------------------------------------------------------
/**
 * @brief	３Ｄ動作
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

static void g3d_control_effect( void )
{
	MtxFx33 rotate;
	VecFx32 rotate_tmp = { 0, 0, 0 };
	GFL_G3D_OBJ* g3Dobj;

	//回転計算
	{
		int i;

		for( i=0; i<NELEMS(g3Dscene_objTbl); i++ ){
			//オブジェクトハンドルを取得
			g3Dobj = GFL_G3D_UtilsysObjHandleGet( tetsuWork->g3Dscene, i );

			rotate_tmp.y = g3DanmRotateSpeed * tetsuWork->work[0] * (i+1);	//Ｙ軸回転
			rotateCalc( &rotate_tmp, &tetsuWork->g3DobjStatus[i].rotate );
		}
	}
	tetsuWork->work[0]++;
}



