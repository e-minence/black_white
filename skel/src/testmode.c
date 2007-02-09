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

static void		TestModeWorkSet( HEAPID heapID );
static void		TestModeWorkRelease( void );
static BOOL		TestModeControl( void );
static u16		TestModeSelectPosGet( void );

static const	GFL_PROC_DATA TestMainProcData;
//============================================================================================
//
//
//	�v���Z�X�R���g���[��
//
//
//============================================================================================
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
	TestModeWorkSet( GFL_HEAPID_APP );
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief		���C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	if( TestModeControl() == TRUE ){
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
	switch( TestModeSelectPosGet() ) {
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
	TestModeWorkRelease();

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





//============================================================================================
//
//
//	���C���R���g���[��
//
//
//============================================================================================
typedef struct {
	int						seq;
	u16						listPosition;
	void*					chrbuf;
	GFL_BMP_DATA*			bmpHeader;
	GFL_TEXT_PRINTPARAM*	textParam;

	GFL_BMPWIN*				bmpwin[32];

	u16						g3DresTblIdx;
	u16						g3DobjTblIdx;
	u16						g3DanmTblIdx;

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

TESTMODE_WORK* testmode;

//�a�f�ݒ�֐�
static void	bg_init( void );
static void	bg_exit( void );
//�r�b�g�}�b�v�ݒ�֐�
static void msg_bmpwin_make( u8 bmpwinNum, const char* msg, u8 px, u8 py, u8 sx, u8 sy );
static void msg_bmpwin_trush( u8 bmpwinNum );
static void msg_bmpwin_palset( u8 bmpwinNum, u8 pal );
//�Q�c�a�f�쐬�֐�
static void	g2d_load( void );
static void	g2d_unload( void );
//�R�c�a�f�쐬�֐�
static void	g3d_load( void );
static void g3d_draw( void );
static void	g3d_unload( void );
//�G�t�F�N�g
static void g3d_control_effect( void );
//------------------------------------------------------------------
/**
 * @brief	���[�N�̊m�ۂƔj��
 */
//------------------------------------------------------------------
static void	TestModeWorkSet( HEAPID heapID )
{
	testmode = GFL_HEAP_AllocMemoryClear( heapID, sizeof(TESTMODE_WORK) );
}

static void	TestModeWorkRelease( void )
{
	GFL_HEAP_FreeMemory( testmode );
}

//------------------------------------------------------------------
/**
 * @brief	�I�����X�g�|�W�V�����擾
 */
//------------------------------------------------------------------
static u16	TestModeSelectPosGet( void )
{
	return testmode->listPosition;
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
				//���݂̃J�[�\���ʒu��ԕ����ŕ\��
				msg_bmpwin_palset( NUM_SELECT1+i, MSG_RED );
			}else{
				msg_bmpwin_palset( NUM_SELECT1+i, MSG_WHITE );
			}
			//�r�b�g�}�b�v�X�N���[���ĕ`��
			GFL_BMPWIN_MakeScrn( testmode->bmpwin[NUM_SELECT1+i] );
		}
		//�a�f�X�N���[���]�����N�G�X�g���s
		GFL_BG_LoadScreenReq( TEXT_FRM );
		testmode->seq++;

		g3d_control_effect();
		g3d_draw();		//�R�c�f�[�^�`��
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
		g3d_control_effect();
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
	GFL_G3D_UtilsysInit( G3D_UTIL_RESSIZ, G3D_UTIL_OBJSIZ, G3D_UTIL_ANMSIZ, heapID );  
}

static void	bg_exit( void )
{
	GFL_G3D_UtilsysExit();  
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
			msg_bmpwin_make( NUM_TITLE+i, indexList[i].msg,
							indexList[i].posx, indexList[i].posy, 
							indexList[i].sizx, indexList[i].sizy );
		}
		//�I�����ڃr�b�g�}�b�v�̍쐬
		for(i=0;i<NELEMS(selectList);i++){
			msg_bmpwin_make( NUM_SELECT1+i, selectList[i].msg,
							selectList[i].posx, selectList[i].posy,
							selectList[i].sizx, selectList[i].sizy );
		}
	}
	//�a�f�X�N���[���]�����N�G�X�g���s
	GFL_BG_LoadScreenReq( TEXT_FRM );
}

static void	g2d_unload( void )
{
	int i;

	//�I�����ڃr�b�g�}�b�v�̔j��
	for(i=0;i<NELEMS(selectList);i++){
		msg_bmpwin_trush( NUM_SELECT1+i );
	}
	//�\��r�b�g�}�b�v�̔j��
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
//�쐬
static void g3d_load( void )
{
	//���\�[�X���I�u�W�F�N�g���A�j���[�V�������ꊇ�ݒ�
	GFL_G3D_UtilAllLoad( g3DresouceTable, NELEMS(g3DresouceTable), &testmode->g3DresTblIdx,
						 g3DobjectTable, NELEMS(g3DobjectTable), &testmode->g3DobjTblIdx,
						 g3DanimetionTable, NELEMS(g3DanimetionTable), &testmode->g3DanmTblIdx );

	//�J�����Z�b�g
	GFL_G3D_sysProjectionSet(	GFL_G3D_PRJPERS, 
								FX_SinIdx( cameraPerspway ), FX_CosIdx( cameraPerspway ), 
								cameraAspect, 0, cameraNear, cameraFar, 0 );
	GFL_G3D_sysLookAtSet( (VecFx32*)&cameraPos, (VecFx32*)&cameraUp, (VecFx32*)&cameraTarget );

	testmode->work[0] = 0;
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
	GFL_G3D_UtilAllUnload(	NELEMS(g3DresouceTable), &testmode->g3DresTblIdx,
							NELEMS(g3DobjectTable), &testmode->g3DobjTblIdx,
							NELEMS(g3DanimetionTable), &testmode->g3DanmTblIdx );
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
		g3Dobj = GFL_G3D_UtilObjGet( testmode->g3DobjTblIdx + G3OBJ_AIR );
		//AIR�̃A�j���[�V�����n���h�����擾
		g3Danm = GFL_G3D_UtilAnmGet( testmode->g3DanmTblIdx + G3ANM_AIR );

		rotate_tmp.y = g3DanmRotateSpeed * testmode->work[0];	//�x����]
		GFL_G3D_UtilObjDrawRotateCalcYX( &rotate_tmp, &rotate );

		//AIR�̉�]�X�e�[�^�X���Z�b�g
		GFL_G3D_ObjContSetRotate( g3Dobj, &rotate );
		//�A�j���[�V�����R���g���[��
		GFL_G3D_ObjContAnmFrameAutoLoop( g3Danm, g3DanmFrameSpeed );
	}
	{
		//IAR�̃I�u�W�F�N�g�n���h�����擾
		g3Dobj = GFL_G3D_UtilObjGet( testmode->g3DobjTblIdx + G3OBJ_IAR );
		//IAR�̃A�j���[�V�����n���h�����擾
		g3Danm = GFL_G3D_UtilAnmGet( testmode->g3DanmTblIdx + G3ANM_IAR );

		rotate_tmp.y = -g3DanmRotateSpeed * testmode->work[0];	//�x����]
		GFL_G3D_UtilObjDrawRotateCalcYX( &rotate_tmp, &rotate );

		//IAR�̉�]�X�e�[�^�X���Z�b�g
		GFL_G3D_ObjContSetRotate( g3Dobj, &rotate );
		//�A�j���[�V�����R���g���[��
		GFL_G3D_ObjContAnmFrameAutoLoop( g3Danm, g3DanmFrameSpeed );
	}
	testmode->work[0]++;
}
	




