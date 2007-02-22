//===================================================================
/**
 * @file	testmode.c
 */
//===================================================================
#include "gflib.h"
#include "main.h"
#include "textprint.h"

#include "testmode.h"

void	TestModeSet(void);


//============================================================================================
//
//
//	���C���R���g���[��
//
//
//============================================================================================
#define G3DUTIL_USE
typedef struct {
	int						seq;
	u16						listPosition;
	void*					chrbuf;
	GFL_BMP_DATA*			bmpHeader;
	GFL_TEXT_PRINTPARAM*	textParam;

	GFL_BMPWIN*				bmpwin[32];
#ifdef G3DUTIL_USE
	GFL_G3D_UTIL_SCENE*		g3Dscene;
#else
	GFL_G3D_RES*			g3Dres[4];
	GFL_G3D_RND*			g3Drnd[2];
	GFL_G3D_ANM*			g3Danm[2];
	GFL_G3D_OBJ*			g3Dobj[2];
#endif
	GFL_G3D_OBJSTATUS		status[2];

	u16						work[16];
}TESTMODE_WORK;

typedef struct {
	const char*	msg;
	u8 posx;
	u8 posy;
	u8 sizx;
	u8 sizy;
}TESTMODE_PRINTLIST;

enum {
	NUM_TITLE = 0,
	NUM_URL,
	NUM_SELECT1,
	NUM_SELECT2,
	NUM_SELECT3,
	NUM_SELECT4,
	NUM_SELECT5,
};

enum {
	MSG_WHITE = 0,
	MSG_RED,
};

#include "sample_graphic/titledemo.naix"
#include "testmode.dat"



//�a�f�ݒ�֐�
static void	bg_init( void );
static void	bg_exit( void );
//�r�b�g�}�b�v�ݒ�֐�
static void msg_bmpwin_make(TESTMODE_WORK * testmode, u8 bmpwinNum, const char* msg, u8 px, u8 py, u8 sx, u8 sy );
static void msg_bmpwin_trush( TESTMODE_WORK * testmode, u8 bmpwinNum );
static void msg_bmpwin_palset( TESTMODE_WORK * testmode, u8 bmpwinNum, u8 pal );
//�Q�c�a�f�쐬�֐�
static void	g2d_load( TESTMODE_WORK * testmode );
static void	g2d_unload( TESTMODE_WORK * testmode );
//�R�c�a�f�쐬�֐�
static void	g3d_load( TESTMODE_WORK * testmode );
static void g3d_draw( TESTMODE_WORK * testmode );
static void	g3d_unload( TESTMODE_WORK * testmode );
//�G�t�F�N�g
static void g3d_control_effect( TESTMODE_WORK * testmode );

//------------------------------------------------------------------
/**
 * @brief	�I�����X�g�|�W�V�����擾
 */
//------------------------------------------------------------------
static u16	TestModeSelectPosGet( TESTMODE_WORK * testmode )
{
	return testmode->listPosition;
}

//------------------------------------------------------------------
/**
 * @brief	���X�g�I��
 */
//------------------------------------------------------------------
static BOOL	TestModeControl( TESTMODE_WORK * testmode )
{
	BOOL return_flag = FALSE;
	int i;

	switch( testmode->seq ){

	case 0:
		//������
		bg_init();
		testmode->listPosition = 0;
		testmode->seq++;
		break;

	case 1:
		//��ʍ쐬
		g2d_load(testmode);	//�Q�c�f�[�^�쐬
		g3d_load(testmode);	//�R�c�f�[�^�쐬
		testmode->seq++;
		break;

	case 2:
		//�p���b�g�`�F���W�ɂ��J�[�\���`��
		for(i=0;i<NELEMS(selectList);i++){
			if( i == testmode->listPosition ){
				//���݂̃J�[�\���ʒu��ԕ����ŕ\��
				msg_bmpwin_palset( testmode, NUM_SELECT1+i, MSG_RED );
			}else{
				msg_bmpwin_palset( testmode, NUM_SELECT1+i, MSG_WHITE );
			}
			//�r�b�g�}�b�v�X�N���[���ĕ`��
			GFL_BMPWIN_MakeScrn( testmode->bmpwin[NUM_SELECT1+i] );
		}
		//�a�f�X�N���[���]�����N�G�X�g���s
		GFL_BG_LoadScreenReq( TEXT_FRM );
		testmode->seq++;

		g3d_control_effect(testmode);
		g3d_draw(testmode);		//�R�c�f�[�^�`��
		break;

	case 3:
		//�L�[����
		if( GFL_UI_KeyGetTrg() == PAD_BUTTON_A ) {
			if(		( testmode->listPosition == 0)		//�킽�Ȃׂ̂Ƃ��J��
				||	( testmode->listPosition == 1)		//���܂��̂Ƃ��J��
				||	( testmode->listPosition == 2)		//�����ׂ̂Ƃ��J��
				||	( testmode->listPosition == 3)		//�����̂̂Ƃ��J��
			  )
			{
				testmode->seq++;
			}
		} else if( GFL_UI_KeyGetTrg() == PAD_KEY_UP ){
			if( testmode->listPosition > 0 ){
				testmode->listPosition--;
				testmode->seq--;
			}
		} else if( GFL_UI_KeyGetTrg() == PAD_KEY_DOWN ){
			if( testmode->listPosition < NELEMS(selectList)-1 ){
				testmode->listPosition++;
				testmode->seq--;
			}
		}
		g3d_control_effect(testmode);
		g3d_draw(testmode);		//�R�c�f�[�^�`��
		break;

	case 4:
		//�I��
		g3d_unload(testmode);	//�R�c�f�[�^�j��
		g2d_unload(testmode);	//�Q�c�f�[�^�j��
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
	GX_SetBankForBG(GX_VRAM_BG_64_E);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G); 

	//�a�f���[�h�ݒ�
	GFL_BG_InitBG( &bgsysHeader );

	//�a�f�R���g���[���ݒ�
	GFL_BG_BGControlSet( TEXT_FRM, &bgCont3, GFL_BG_MODE_TEXT );
	GFL_BG_PrioritySet( TEXT_FRM, TEXT_FRM_PRI );
	GFL_BG_VisibleSet( TEXT_FRM, VISIBLE_ON );

	//�r�b�g�}�b�v�E�C���h�E�V�X�e���̋N��
	GFL_BMPWIN_sysInit( heapID );

	//�R�c�V�X�e���N��
	GFL_G3D_sysInit( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, heapID, NULL );
	GFL_BG_BGControlSet3D( G3D_FRM_PRI );
	//GFL_G3D_UtilsysInit( G3D_UTIL_RESSIZ, G3D_UTIL_OBJSIZ, G3D_UTIL_ANMSIZ, heapID );  
}

static void	bg_exit( void )
{
	//GFL_G3D_UtilsysExit();  
	GFL_G3D_sysExit();
	GFL_BMPWIN_sysExit();
	GFL_BG_BGControlExit( TEXT_FRM );
	GFL_BG_sysExit();
}

//------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�r�b�g�}�b�v�E�C���h�E�R���g���[��
 */
//------------------------------------------------------------------
static void msg_bmpwin_make(TESTMODE_WORK * testmode, u8 bmpwinNum, const char* msg, u8 px, u8 py, u8 sx, u8 sy )
{
	//�r�b�g�}�b�v�쐬
	testmode->bmpwin[bmpwinNum] = GFL_BMPWIN_Create( TEXT_FRM, px, py, sx, sy, 0, 
														GFL_BG_CHRAREA_GET_B );

	//�e�L�X�g���r�b�g�}�b�v�ɕ\��
	testmode->textParam->bmp = GFL_BMPWIN_GetBmp( testmode->bmpwin[ bmpwinNum ] );
	GFL_TEXT_PrintSjisCode( msg, testmode->textParam );

	//�r�b�g�}�b�v�L�����N�^�[���A�b�v�f�[�g
	GFL_BMPWIN_UploadChar( testmode->bmpwin[bmpwinNum] );
	//�r�b�g�}�b�v�X�N���[���쐬
	GFL_BMPWIN_MakeScrn( testmode->bmpwin[bmpwinNum] );
}
	
static void msg_bmpwin_trush( TESTMODE_WORK * testmode, u8 bmpwinNum )
{
	GFL_BMPWIN_Delete( testmode->bmpwin[bmpwinNum] );
}
	
static void msg_bmpwin_palset( TESTMODE_WORK * testmode, u8 bmpwinNum, u8 pal )
{
	GFL_BMPWIN_SetPal( testmode->bmpwin[bmpwinNum], pal );
}
	
//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
static void	g2d_load( TESTMODE_WORK * testmode )
{
	u16 heapID = GFL_HEAPID_APP;

	//�t�H���g�ǂݍ���
	GFL_TEXT_sysInit( font_path );
	//�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocMemoryLowClear( heapID, 16*2 );
		plt[0] = G2D_BACKGROUND_COL;
		plt[1] = G2D_FONT_COL;
		GFL_BG_PaletteSet( TEXT_FRM, plt, 16*2, 0 );
		plt[1] = G2D_FONTSELECT_COL;
		GFL_BG_PaletteSet( TEXT_FRM, plt, 16*2, 16*2 );

		GFL_HEAP_FreeMemory( plt );
	}
	//�����\���p�����[�^���[�N�쐬
	{
		GFL_TEXT_PRINTPARAM* param = GFL_HEAP_AllocMemoryLowClear
										( heapID,sizeof(GFL_TEXT_PRINTPARAM));
		*param = default_param;
		testmode->textParam = param;
	}
	//�����\���r�b�g�}�b�v�̍쐬
	{
		int i;

		//�\��r�b�g�}�b�v�̍쐬
		for(i=0;i<NELEMS(indexList);i++){
			msg_bmpwin_make( testmode, NUM_TITLE+i, indexList[i].msg,
							indexList[i].posx, indexList[i].posy, 
							indexList[i].sizx, indexList[i].sizy );
		}
		//�I�����ڃr�b�g�}�b�v�̍쐬
		for(i=0;i<NELEMS(selectList);i++){
			msg_bmpwin_make( testmode, NUM_SELECT1+i, selectList[i].msg,
							selectList[i].posx, selectList[i].posy,
							selectList[i].sizx, selectList[i].sizy );
		}
	}
	//�a�f�X�N���[���]�����N�G�X�g���s
	GFL_BG_LoadScreenReq( TEXT_FRM );
}

static void	g2d_unload( TESTMODE_WORK * testmode )
{
	int i;

	//�I�����ڃr�b�g�}�b�v�̔j��
	for(i=0;i<NELEMS(selectList);i++){
		msg_bmpwin_trush( testmode, NUM_SELECT1+i );
	}
	//�\��r�b�g�}�b�v�̔j��
	for(i=0;i<NELEMS(indexList);i++){
		msg_bmpwin_trush( testmode, NUM_TITLE+i );
	}
	GFL_HEAP_FreeMemory( testmode->textParam );
	GFL_TEXT_sysExit();
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�f�[�^
 */
//------------------------------------------------------------------
static const char g3DarcPath[] = {"src/sample_graphic/titledemo.narc"};

enum {
	G3DRES_AIR_BMD = 0,
	G3DRES_AIR_BTA,
	G3DRES_IAR_BMD,
	G3DRES_IAR_BTA,
};

enum {
	G3D_AIR = 0,
	G3D_IAR,
};

#ifdef G3DUTIL_USE
static const GFL_G3D_UTIL_SCENE_RES g3Dscene_resTbl[] = {
	{ (u32)g3DarcPath, NARC_titledemo_title_air_nsbmd, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_titledemo_title_air_nsbta, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_titledemo_title_iar_nsbmd, GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath, NARC_titledemo_title_iar_nsbta, GFL_G3D_UTIL_RESPATH },
};

static const GFL_G3D_UTIL_SCENE_ANM g3Dscene_anm1Tbl[] = {
	{ G3DRES_AIR_BTA, 0 },
};

static const GFL_G3D_UTIL_SCENE_ANM g3Dscene_anm2Tbl[] = {
	{ G3DRES_IAR_BTA, 0 },
};

static const GFL_G3D_UTIL_SCENE_OBJ g3Dscene_objTbl[] = {
	{ G3DRES_AIR_BMD, 0, G3DRES_AIR_BMD, g3Dscene_anm1Tbl, NELEMS(g3Dscene_anm1Tbl) },
	{ G3DRES_IAR_BMD, 0, G3DRES_IAR_BMD, g3Dscene_anm2Tbl, NELEMS(g3Dscene_anm2Tbl) },
};

static const GFL_G3D_UTIL_SCENE_SETUP g3Dscene_setup = {
	g3Dscene_resTbl, NELEMS(g3Dscene_resTbl),
	g3Dscene_objTbl, NELEMS(g3Dscene_objTbl),
};
#endif

//------------------------------------------------------------------
/**
 * @brief		�R�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
static void g3d_load( TESTMODE_WORK * testmode )
{
#ifdef G3DUTIL_USE
	u16 heapID = GFL_HEAPID_APP;

	testmode->g3Dscene = GFL_G3D_UtilsysCreate( &g3Dscene_setup, heapID );
#else
	//		���\�[�X�Z�b�g�A�b�v
	testmode->g3Dres[ G3DRES_AIR_BMD ] = GFL_G3D_ResCreatePath
										( g3DarcPath, NARC_titledemo_title_air_nsbmd );
	testmode->g3Dres[ G3DRES_AIR_BTA ] = GFL_G3D_ResCreatePath
										( g3DarcPath, NARC_titledemo_title_air_nsbta );
	testmode->g3Dres[ G3DRES_IAR_BMD ] = GFL_G3D_ResCreatePath
										( g3DarcPath, NARC_titledemo_title_iar_nsbmd );
	testmode->g3Dres[ G3DRES_IAR_BTA ] = GFL_G3D_ResCreatePath
										( g3DarcPath, NARC_titledemo_title_iar_nsbta );
	
	//		���\�[�X�]��
	GFL_G3D_VramLoadTex( testmode->g3Dres[ G3DRES_AIR_BMD ] );
	GFL_G3D_VramLoadTex( testmode->g3Dres[ G3DRES_IAR_BMD ] );
	//		�����_�[�쐬
	testmode->g3Drnd[ G3D_AIR ] = GFL_G3D_RndCreate(	testmode->g3Dres[ G3DRES_AIR_BMD ], 0, 
														testmode->g3Dres[ G3DRES_AIR_BMD ] );
	testmode->g3Drnd[ G3D_IAR ] = GFL_G3D_RndCreate(	testmode->g3Dres[ G3DRES_IAR_BMD ], 0, 
														testmode->g3Dres[ G3DRES_IAR_BMD] );
	//		�A�j���쐬
	testmode->g3Danm[ G3D_AIR ] = GFL_G3D_AnmCreate(	testmode->g3Drnd[ G3D_AIR ], 
														testmode->g3Dres[ G3DRES_AIR_BTA ], 0 );
	testmode->g3Danm[ G3D_IAR ] = GFL_G3D_AnmCreate(	testmode->g3Drnd[ G3DIAR ], 
														testmode->g3Dres[ G3DRES_IAR_BTA ], 0 );
	//		�I�u�W�F�N�g�쐬
	testmode->g3Dobj[ G3D_AIR ] = GFL_G3D_ObjCreate(	testmode->g3Drnd[ G3D_AIR ], 
														&testmode->g3Danm[ G3D_AIR ], 1 );
	testmode->g3Dobj[ G3D_IAR ] = GFL_G3D_ObjCreate(	testmode->g3Drnd[ G3D_IAR ], 
														&testmode->g3Danm[ G3D_IAR ], 1 );
#endif
	//�`��X�e�[�^�X���[�N�ݒ�
	testmode->status[ G3D_AIR ] = status0;
	testmode->status[ G3D_IAR ] = status1;

	//�J�����Z�b�g
	GFL_G3D_sysProjectionSet(	GFL_G3D_PRJPERS, 
								FX_SinIdx( cameraPerspway ), FX_CosIdx( cameraPerspway ), 
								cameraAspect, 0, cameraNear, cameraFar, 0 );
	GFL_G3D_sysLookAtSet( (VecFx32*)&cameraPos, (VecFx32*)&cameraUp, (VecFx32*)&cameraTarget );

	testmode->work[0] = 0;
}
	
static void g3d_draw( TESTMODE_WORK * testmode )
{
	GFL_G3D_OBJ* g3Dobj[2];
#ifdef G3DUTIL_USE
	g3Dobj[ G3D_AIR ] = GFL_G3D_UtilsysObjHandleGet( testmode->g3Dscene, G3D_AIR  );
	g3Dobj[ G3D_IAR ] = GFL_G3D_UtilsysObjHandleGet( testmode->g3Dscene, G3D_IAR  );
#else
	g3Dobj[ G3D_AIR ] = testmode->g3Dobj[ G3D_AIR ];
	g3Dobj[ G3D_IAR ] = testmode->g3Dobj[ G3D_IAR ];
#endif
	GFL_G3D_DrawStart();
	GFL_G3D_DrawLookAt();
	{
		GFL_G3D_ObjDraw( g3Dobj[ G3D_AIR ], &testmode->status[ G3D_AIR ] );
		GFL_G3D_ObjDraw( g3Dobj[ G3D_IAR ], &testmode->status[ G3D_IAR ] );
	}
	GFL_G3D_DrawEnd();

	GFL_G3D_ObjContAnmFrameAutoLoop( g3Dobj[ G3D_AIR ], 0, FX32_ONE ); 
	GFL_G3D_ObjContAnmFrameAutoLoop( g3Dobj[ G3D_IAR ], 0, FX32_ONE ); 
}
	
static void g3d_unload( TESTMODE_WORK * testmode )
{
#ifdef G3DUTIL_USE
	GFL_G3D_UtilsysDelete( testmode->g3Dscene );
#else
	GFL_G3D_ObjDelete( testmode->g3Dobj[ G3D_IAR ] );
	GFL_G3D_ObjDelete( testmode->g3Dobj[ G3D_AIR ] );

	GFL_G3D_AnmDelete( testmode->g3Danm[ G3D_IAR ] );
	GFL_G3D_AnmDelete( testmode->g3Danm[ G3D_AIR ] );

	GFL_G3D_RndDelete( testmode->g3Drnd[ G3D_IAR ] );
	GFL_G3D_RndDelete( testmode->g3Drnd[ G3D_AIR ] );

	GFL_G3D_VramUnloadTex( testmode->g3Dres[ G3DRES_IAR_BMD ] );
	GFL_G3D_VramUnloadTex( testmode->g3Dres[ G3DRES_AIR_BMD ] );

	GFL_G3D_ResDelete( testmode->g3Dres[ G3DRES_IAR_BTA ] );
	GFL_G3D_ResDelete( testmode->g3Dres[ G3DRES_IAR_BMD ] );
	GFL_G3D_ResDelete( testmode->g3Dres[ G3DRES_AIR_BTA ] );
	GFL_G3D_ResDelete( testmode->g3Dres[ G3DRES_AIR_BMD ] );
#endif
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

static void g3d_control_effect( TESTMODE_WORK * testmode )
{
	MtxFx33 rotate;
	VecFx32 rotate_tmp = { 0, 0, 0 };
	GFL_G3D_OBJ* g3Dobj;

	//��]�v�Z
	rotate_tmp.y = g3DanmRotateSpeed * testmode->work[0];	//�x����]
	rotateCalc( &rotate_tmp, &testmode->status[0].rotate );

	rotate_tmp.y = -g3DanmRotateSpeed * testmode->work[0];	//�x����]
	rotateCalc( &rotate_tmp, &testmode->status[1].rotate );

	testmode->work[0]++;
}
	



//============================================================================================
//
//
//	�v���Z�X�R���g���[��
//
//
//============================================================================================
static const	GFL_PROC_DATA TestMainProcData;

//------------------------------------------------------------------
/**
 * @brief		�v���Z�X�ݒ�
 */
//------------------------------------------------------------------
void	TestModeSet(void)
{
	GFL_PROC_SysCallProc(NO_OVERLAY_ID, &TestMainProcData, NULL);
}

//------------------------------------------------------------------
/**
 * @brief		������
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	TESTMODE_WORK * testmode;
	testmode = GFL_PROC_AllocWork( proc, sizeof(TESTMODE_WORK), GFL_HEAPID_APP );
	GFL_STD_MemClear(testmode, sizeof(TESTMODE_WORK));
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief		���C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	TESTMODE_WORK * testmode = mywk;
	if( TestModeControl(testmode) == TRUE ){
		return GFL_PROC_RES_FINISH;
	}
	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief		�I��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	TESTMODE_WORK * testmode = mywk;
	switch( TestModeSelectPosGet(testmode) ) {
	case 0:
		//�킽�Ȃ�
		GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &DebugWatanabeMainProcData, NULL);
		break;
	case 1:
		//���܂�
		GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &DebugTamadaMainProcData, NULL);
		break;
	case 2:
		//������
		GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &DebugSogabeMainProcData, NULL);
		break;
	case 3:
		//������
		GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &DebugOhnoMainProcData, NULL);
		break;
	default:
		//����
		//�Ȃ��ނ�
		//�����͂�
		break;
	}
	GFL_PROC_FreeWork(mywk);

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��e�[�u��
 */
//------------------------------------------------------------------
const GFL_PROC_DATA TestMainProcData = {
	TestModeProcInit,
	TestModeProcMain,
	TestModeProcEnd,
};






