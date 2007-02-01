//===================================================================
/**
 * @file	testmode.c
 */
//===================================================================
#include "gflib.h"
#include "main.h"
#include "textprint.h"

#include "testmode.h"

#include "sample_graphic/titledemo.naix"

//------------------------------------------------------------------
typedef struct {
	int						seq;
	u16						listPosition;
	void*					chrbuf;
	GFL_BMP_DATA*			bmpHeader;
	GFL_TEXT_PRINTPARAM*	textParam;

	GFL_BMPWIN*				bmpwin[32];

	GFL_G3D_RES*			g3Dres[8];
	GFL_G3D_OBJ*			g3Dobj[8];

	u16						work[16];
}TESTMODE_WORK;

TESTMODE_WORK* testmode;

void	TestModeInit(void);
void	TestModeMain(void);

static BOOL	TestModeControl( void );
static const GFL_PROC_DATA TestMainProcData;

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

static const char	test_index1[] = 
{"�Q�[���t���[�N�@�v���O�������C�u�����@�e�X�g���[�h\nGame Freak Libraries TestMode"};
static const char	test_index2[] = {"http://www.gamefreak.co.jp"};

static const char	test_select1[]	= {"�킽�Ȃׁ@�Ă�@�@Tetsuya Watanabe"};
static const char	test_select2[]	= {"���܂��@���������@�@Sousuke Tamada"};
static const char	test_select3[]	= {"�����ׁ@�Ђ����@�@Hisashi Sogabe"};
static const char	test_select4[]	= {"�����́@���݁@�@Katsumi Ohno"};
static const char	test_select5[]	= {"����@�܂����@�@Masao Taya"};
static const char	test_select6[]	= {"�Ȃ��ނ�@�Ђ�䂫�@�@Hiroyuki Nakamura"};
static const char	test_select7[]	= {"�܂��@�悵�̂�@�@Yoshinori Matsuda"};
static const char	test_select8[]	= {"������@�������@�@Keita Kagaya"};
static const char	test_select9[]	= {"���Ƃ��@���������@�@Daisuke Gotou"};
static const char	test_select10[]	= {"�̂͂�@���Ƃ��@�@Satoshi Nohara"};
static const char	test_select11[]	= {"�����͂��@�Ƃ���@�@Tomoya Takahashi"};
static const char	test_select12[]	= {"����@�����Ɓ@�@Akito Mori"};
static const char	test_select13[]	= {"�������@�Ƃ��݂��@�@Tomomichi Ohta"};
static const char	test_select14[]	= {"���킳��@�݂䂫�@�@Miyuki Iwasawa"};
static const char	test_select15[]	= {"�����Ƃ��@�̂��ށ@�@Nozomu Saitou"};

typedef struct {
	const char*	msg;
	u8 posx;
	u8 posy;
	u8 sizx;
	u8 sizy;
}TESTMODE_PRINTLIST;

static const TESTMODE_PRINTLIST indexList[] = {
	{ test_index1, 2,  1, 30, 2 },
	{ test_index2, 2, 22, 30, 1 },
};

static const TESTMODE_PRINTLIST selectList[] = {
#if 0
	{ test_select1,  4,  5, 24, 1 },
	{ test_select2,  4,  6, 24, 1 },
	{ test_select3,  4,  7, 24, 1 },
	{ test_select4,  4,  8, 24, 1 },
	{ test_select5,  4,  9, 24, 1 },
	{ test_select6,  4, 10, 24, 1 },
	{ test_select7,  4, 11, 24, 1 },
	{ test_select8,  4, 12, 24, 1 },
	{ test_select9,  4, 13, 24, 1 },
	{ test_select10, 4, 14, 24, 1 },
	{ test_select11, 4, 15, 24, 1 },
	{ test_select12, 4, 16, 24, 1 },
	{ test_select13, 4, 17, 24, 1 },
	{ test_select14, 4, 18, 24, 1 },
	{ test_select15, 4, 19, 24, 1 },
#else
	{ test_select1,  4,  5, 24, 1 },
	{ test_select2,  4,  7, 24, 1 },
	{ test_select3,  4,  9, 24, 1 },
	{ test_select4,  4, 11, 24, 1 },
	{ test_select5,  4, 13, 24, 1 },
	{ test_select6,  4, 15, 24, 1 },
	{ test_select11, 4, 17, 24, 1 },
#endif
};

#define TEXT_FRM	(GFL_BG_FRAME3_M)
//------------------------------------------------------------------
/**
 * @brief		������
 *
 * ����A���g�̓��[�g�v���Z�X�̐����݂̂ƂȂ��Ă���
 */
//------------------------------------------------------------------
void	TestModeInit(void)
{
	GFL_PROC_SysCallProc(NO_OVERLAY_ID, &TestMainProcData, NULL);
//	testmode = GFL_HEAP_AllocMemoryClear(GFL_HEAPID_APP,sizeof(TESTMODE_WORK));
}

//------------------------------------------------------------------
/**
 * @brief		���C��
 *
 * TestModeInit�ŃZ�b�g���ꂽ�v���Z�X�����삵�Ă��邽�߁A
 * ����͒��g�͉����Ȃ���ԂƂȂ��Ă���B
 */
//------------------------------------------------------------------
void	TestModeMain(void)
{
	/* 
	if( TestModeControl() == TRUE ){
		GFL_HEAP_FreeMemory( testmode );
	}
	*/
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
	{
		GX_SetBankForTex(GX_VRAM_TEX_01_AB);
		GX_SetBankForBG(GX_VRAM_BG_64_E);
		GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G); 
	}
	{
		//�a�f���[�h�ݒ�
		GFL_BG_SYS_HEADER bgsysHeader = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
		};	
		GFL_BG_InitBG( &bgsysHeader );
	}
	{
		//�a�f�R���g���[���ݒ�

		//BG3( message frame )
		GFL_BG_BGCNT_HEADER bgCont3 = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x04000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_BGCNT_HEADER bgCont0 = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x04000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_BGControlSet( TEXT_FRM, &bgCont3, GFL_BG_MODE_TEXT );
		GFL_BG_PrioritySet( TEXT_FRM, 0 );
		GFL_BG_VisibleSet( TEXT_FRM, VISIBLE_ON );
	}
	//�r�b�g�}�b�v�E�C���h�E�V�X�e���̋N��
	{
		GFL_BMPWIN_sysInit( heapID );
	}
	//�R�c�V�X�e���N��
	{
		GFL_G3D_sysInit( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						0x1000, heapID, NULL );
		//BG0( 3D frame )
		GFL_BG_BGControlSet3D( 1 );
	}
}

static void	bg_exit( void )
{
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
//�쐬
static void msg_bmpwin_make( u8 bmpwinNum, const char* msg, u8 px, u8 py, u8 sx, u8 sy )
{
	testmode->bmpwin[bmpwinNum] = GFL_BMPWIN_Create( TEXT_FRM, px, py, sx, sy, 0, 0 );

	testmode->textParam->bmp = GFL_BMPWIN_GetBmp( testmode->bmpwin[ bmpwinNum ] );
	testmode->textParam->writex = 0;
	testmode->textParam->writey = 0;
	GFL_TEXT_PrintSjisCode( msg, testmode->textParam );

	GFL_BMPWIN_UploadChar( testmode->bmpwin[bmpwinNum] );
	GFL_BMPWIN_MakeScrn( testmode->bmpwin[bmpwinNum] );
}
	
//�j��
static void msg_bmpwin_trush( u8 bmpwinNum )
{
	GFL_BMPWIN_Delete( testmode->bmpwin[bmpwinNum] );
}
	
//�p���b�g�Đݒ�
static void msg_bmpwin_palset( u8 bmpwinNum, u8 pal )
{
	GFL_BMPWIN_SetPal( testmode->bmpwin[bmpwinNum], pal );
}
	
//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
static void	g2d_load( void )
{
	u16 heapID = GFL_HEAPID_APP;

	//�t�H���g�ǂݍ���
	GFL_TEXT_sysInit( "gfl_graphic/gfl_font.dat" );
	//�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocMemoryLowClear( heapID, 16*2 );
		plt[0] = 0x5041;	//��(�w�i)

		plt[1] = 0x7fff;	//��
		GFL_BG_PaletteSet( TEXT_FRM, plt, 16*2, 0 );
		plt[1] = 0x001f;	//��
		GFL_BG_PaletteSet( TEXT_FRM, plt, 16*2, 16*2 );

		GFL_HEAP_FreeMemory( plt );
	}
	//�����\���p�����[�^���[�N�쐬
	{
		GFL_TEXT_PRINTPARAM* param = GFL_HEAP_AllocMemoryLowClear
										( heapID,sizeof(GFL_TEXT_PRINTPARAM));
		param->spacex = 1;
		param->spacey = 1;
		param->colorF = 1;
		param->colorB = 0;
		param->mode = GFL_TEXT_WRITE_16;
		testmode->textParam = param;
	}
	//�����\���r�b�g�}�b�v�̍쐬
	{
		int i;

		for(i=0;i<NELEMS(indexList);i++){
			msg_bmpwin_make( NUM_TITLE+i, indexList[i].msg,
							indexList[i].posx, indexList[i].posy, 
							indexList[i].sizx, indexList[i].sizy );
		}
		for(i=0;i<NELEMS(selectList);i++){
			msg_bmpwin_make( NUM_SELECT1+i, selectList[i].msg,
							selectList[i].posx, selectList[i].posy,
							selectList[i].sizx, selectList[i].sizy );
		}
	}
	GFL_BG_LoadScreenReq( TEXT_FRM );
}

static void	g2d_unload( void )
{
	int i;

	for(i=0;i<NELEMS(selectList);i++){
		msg_bmpwin_trush( NUM_SELECT1+i );
	}
	for(i=0;i<NELEMS(indexList);i++){
		msg_bmpwin_trush( NUM_TITLE+i );
	}
	GFL_HEAP_FreeMemory( testmode->textParam );
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
#define TD_TITLEP_CMR_CPX_ED			(FX32_ONE * -148)
#define TD_TITLEP_CMR_CPY_ED			(FX32_ONE * 58)
#define TD_TITLEP_CMR_CPZ_ED			(FX32_ONE * 190)
#define TD_TITLEP_CMR_TPX_ED			(FX32_ONE * 8)
#define TD_TITLEP_CMR_TPY_ED			(FX32_ONE * -5)
#define TD_TITLEP_CMR_TPZ_ED			(FX32_ONE * 1)

#define TD_TITLE_CAMERA_PERSPWAY		(0x0b60)
#define TD_TITLE_CAMERA_CLIP_NEAR		(0)
#define TD_TITLE_CAMERA_CLIP_FAR		(FX32_ONE*100)

//�쐬
static void g3d_load( void )
{
	const char* path = "sample_graphic/titledemo.narc";

	//���f�����e�N�X�`�����\�[�X�ǂݍ���
	testmode->g3Dres[0] = GFL_G3D_ResourceCreatePath( path, NARC_titledemo_title_air_nsbmd ); 
	//�A�j���[�V�������\�[�X�ǂݍ���
	testmode->g3Dres[1] = GFL_G3D_ResourceCreatePath( path, NARC_titledemo_title_air_nsbta ); 
	//�e�N�X�`�����\�[�X�]��
	GFL_G3D_TransVramTex( testmode->g3Dres[0] );

	//�R�c�I�u�W�F�N�g�쐬
	testmode->g3Dobj[0] = GFL_G3D_ObjCreate(	testmode->g3Dres[0], 0,		//model
												testmode->g3Dres[0],		//texture
												testmode->g3Dres[1], 0 );	//animation
	{
		//�J�����Z�b�g
		VecFx32	targetPos = {TD_TITLEP_CMR_TPX_ED, TD_TITLEP_CMR_TPY_ED, TD_TITLEP_CMR_TPZ_ED};
		VecFx32	cameraPos = {TD_TITLEP_CMR_CPX_ED, TD_TITLEP_CMR_CPY_ED, TD_TITLEP_CMR_CPZ_ED};
		VecFx32 cameraUp = { 0, FX32_ONE, 0 };

		GFL_G3D_sysProjectionSet( GFL_G3D_PRJPERS,
						FX_SinIdx( TD_TITLE_CAMERA_PERSPWAY ), 
						FX_CosIdx( TD_TITLE_CAMERA_PERSPWAY ), 
						( FX32_ONE * 4 / 3 ), 0, 
						( 1 << FX32_SHIFT ), ( 1024 << FX32_SHIFT ), 0 );
		GFL_G3D_sysLookAtSet( &cameraPos, &cameraUp, &targetPos );
	}
	testmode->work[0] = 0;
}
	
//�j��
static void g3d_unload( void )
{
	GFL_G3D_ObjDelete( testmode->g3Dobj[0] ); 

	GFL_G3D_ResourceDelete( testmode->g3Dres[1] ); 
	GFL_G3D_ResourceDelete( testmode->g3Dres[0] ); 
}
	
static void g3d_draw( void )
{
	VecFx32 trans	= { 0, 0, 0 };											//���W
	MtxFx33 rotate	= { FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE };	//��]
	VecFx32 scale	= { FX32_ONE, FX32_ONE, FX32_ONE };						//�X�P�[��

	//�`��J�n
	GFL_G3D_DrawStart();
	//�J�����O���[�o���X�e�[�g�ݒ�		
 	GFL_G3D_DrawLookAt();

	//�e�I�u�W�F�N�g�`��
	{
		//�I�u�W�F�N�g�`��J�n
		GFL_G3D_ObjDrawStart();
		//�I�u�W�F�N�g���v�Z
		{
			//��]�v�Z
			VecFx32 rotate_tmp = { 0, 0, 0 };
			rotate_tmp.y = 0x100 * testmode->work[0];	//�x����]
			GFL_G3D_ObjDrawRotateCalcYX( &rotate_tmp, &rotate );
		}
		//�I�u�W�F�N�g���]��
		GFL_G3D_ObjDrawStatusSet( &trans, &rotate, &scale );
		//�I�u�W�F�N�g�`��
		GFL_G3D_ObjDraw( testmode->g3Dobj[0] );

		//�A�j���[�V�����R���g���[��
		if( GFL_G3D_ObjContAnmFrameInc( testmode->g3Dobj[0], FX32_ONE ) == FALSE ){
			GFL_G3D_ObjContAnmFrameReset( testmode->g3Dobj[0] );
		}
	}
	//�`��I���i�o�b�t�@�X���b�v�j
	GFL_G3D_DrawEnd();							

	testmode->work[0]++;
}
	
//------------------------------------------------------------------
/**
 * @brief	�v�����g����
 */
//------------------------------------------------------------------
static BOOL	TestModeControl( void )
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
		g2d_load();	//�Q�c�f�[�^�쐬
		g3d_load();	//�R�c�f�[�^�쐬
		testmode->seq++;
		break;

	case 2:
		//�p���b�g�`�F���W�ɂ��J�[�\���`��
		for(i=0;i<NELEMS(selectList);i++){
			if( i == testmode->listPosition ){
				msg_bmpwin_palset( NUM_SELECT1+i, MSG_RED );
			}else{
				msg_bmpwin_palset( NUM_SELECT1+i, MSG_WHITE );
			}
			GFL_BMPWIN_MakeScrn( testmode->bmpwin[NUM_SELECT1+i] );
		}
		GFL_BG_LoadScreenReq( TEXT_FRM );
		testmode->seq++;
		g3d_draw();		//�R�c�f�[�^�`��
		break;

	case 3:
		//�L�[����
		if( GFL_UI_KeyGetTrg() == PAD_BUTTON_A ) {
			if( testmode->listPosition == 1) {
				//�Ƃ肠�������܂��̂Ƃ������J�ڂ���
				testmode->seq++;
			}
			if( testmode->listPosition == 3) {
				//�Ƃ肠���������̂̂Ƃ������J�ڂ���
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
		g3d_draw();		//�R�c�f�[�^�`��
		break;

	case 4:
		//�I��
		g3d_unload();	//�R�c�f�[�^�j��
		g2d_unload();	//�Q�c�f�[�^�j��
		bg_exit();
		return_flag = TRUE;
		break;
	}
	return return_flag;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	testmode = GFL_HEAP_AllocMemoryClear(GFL_HEAPID_APP,sizeof(TESTMODE_WORK));
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	if( TestModeControl() == TRUE ){
		return GFL_PROC_RES_FINISH;
	}
	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	switch (testmode->listPosition) {
	case 0:
		//�킽�Ȃ�
		break;
	case 1:
		//���܂�
		GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &DebugTamadaMainProcData, NULL);
		break;
	case 2:
		//������
		break;
	case 3:
		//������
		GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &DebugOhnoMainProcData, NULL);
		break;
	case 4:
		//�݂͂�
		break;
	default:
		break;
	}
	GFL_HEAP_FreeMemory( testmode );
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static const GFL_PROC_DATA TestMainProcData = {
	TestModeProcInit,
	TestModeProcMain,
	TestModeProcEnd,
};

//------------------------------------------------------------------
//------------------------------------------------------------------




