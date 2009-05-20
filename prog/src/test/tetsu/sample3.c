//============================================================================================
/**
 * @file	sample3.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include <wchar.h>
#include "gflib.h"
#include "system/gfl_use.h"
#include "arc_def.h"

#include "message.naix"
#include "font/font.naix"

#include "system/main.h"

#include "print/printsys.h"
#include "print/str_tool.h"

#include "test/camera_adjust_view.h"

#include "system/talkmsgwin.h"

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�萔
 */
//------------------------------------------------------------------
#define STRBUF_SIZE		(64*2)

#define TEXT_FRAME		(GFL_BG_FRAME3_M)
#define STATUS_FRAME	(GFL_BG_FRAME0_S)
#define WIN_PLTTID		(14)
#define TEXT_PLTTID		(15)
#define COL_SIZ				(2)
#define PLTT_SIZ			(16*COL_SIZ)

#define BACKGROUND_COLOR (0)
//------------------------------------------------------------------
/**
 * @brief		�a�f�`��f�[�^
 */
//------------------------------------------------------------------
#define BCOL1 (1)
#define BCOL2 (2)
#define LCOL	(3)
//------------------------------------------------------------------
/**
 * @brief	�^�b�`�p�l������e�[�u��
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
 * @brief	�����l�e�[�u��
 */
//------------------------------------------------------------------
static const GFL_CAMADJUST_SETUP camAdjustData= {
	0,
	GFL_DISPUT_BGID_S0, GFL_DISPUT_PALID_15,
};

//------------------------------------------------------------------
/**
 * @brief	
 */
//------------------------------------------------------------------
static const u16 testMsg[] = { L"�Z���t�����悤�I�I" };
static const u16 testMsg2[] = { L"�A��" };

//============================================================================================
/**
 *
 * @brief	�\���̒�`
 *
 */
//============================================================================================
typedef struct {
	const u16*		pCommand;
	u16						command;
	u16						commParam[8];
}TEST_MSG_COMM;

typedef struct {
	HEAPID							heapID;
	int									seq;

	GFL_FONT*						fontHandle;

	GFL_G3D_CAMERA*			g3Dcamera;	
	GFL_G3D_LIGHTSET*		g3Dlightset;	
	GFL_G3D_UTIL*				g3Dutil;
	u16									g3DutilUnitIdx;

	GFL_CAMADJUST*			gflCamAdjust;
	fx32								cameraLength;
	u16									cameraAngleV;
	u16									cameraAngleH;

	int									timer;

	TALKMSGWIN_SYS*			tmsgwinSys;
	int									tmsgwinIdx;
	VecFx32							tmsgTarget;
	BOOL								tmsgwinConnect;

	STRBUF*							strBuf[TALKMSGWIN_NUM];
	VecFx32							twinTarget[TALKMSGWIN_NUM];

	TEST_MSG_COMM				msgComm;
}SAMPLE3_WORK;

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�Ăяo���pPROC��`
 *
 *
 *
 *
 *
 */
//============================================================================================
static BOOL	sample3(SAMPLE3_WORK* sw);

//------------------------------------------------------------------
/**
 * @brief	proc�֐�
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT Sample3Proc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	SAMPLE3_WORK* sw = NULL;
	int i;

	sw = GFL_PROC_AllocWork(proc, sizeof(SAMPLE3_WORK), GFL_HEAPID_APP);

  GFL_STD_MemClear(sw, sizeof(SAMPLE3_WORK));
  sw->heapID = GFL_HEAPID_APP;
	sw->seq = 0;
	sw->timer = 0;
	sw->tmsgwinIdx = 0;
	sw->tmsgwinConnect = FALSE;

	for(i=0; i<TALKMSGWIN_NUM; i++){ sw->strBuf[i] = GFL_STR_CreateBuffer(STRBUF_SIZE, sw->heapID); }

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
static GFL_PROC_RESULT Sample3Proc_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	SAMPLE3_WORK*	sw;
	sw = mywk;

	sw->timer++;
	if(sample3(sw) == TRUE){
		return GFL_PROC_RES_CONTINUE;
	}
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
static GFL_PROC_RESULT Sample3Proc_End(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	SAMPLE3_WORK*	sw;
	int i;

	sw = mywk;

	for( i=0; i<TALKMSGWIN_NUM; i++ ){ GFL_STR_DeleteBuffer(sw->strBuf[i]); }

	GFL_PROC_FreeWork(proc);

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	proc�e�[�u��
 */
//------------------------------------------------------------------
const GFL_PROC_DATA DebugWatanabeSample3ProcData = {
	Sample3Proc_Init,
	Sample3Proc_Main,
	Sample3Proc_End,
};





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	���C��
 *
 *
 *
 *
 *
 */
//============================================================================================
static void systemSetup(SAMPLE3_WORK* sw);
static void systemFramework(SAMPLE3_WORK* sw);
static void systemDelete(SAMPLE3_WORK* sw);

static BOOL calcTarget(GFL_G3D_CAMERA* g3Dcamera, int x, int y, VecFx32* target);
static void makeStr(const u16* str, STRBUF* strBuf);
static void commFunc(SAMPLE3_WORK* sw);

static void setSampleMsg1(SAMPLE3_WORK* sw, int idx);
//------------------------------------------------------------------
/**
 *
 * @brief	�t���[�����[�N
 *
 */
//------------------------------------------------------------------
static BOOL	sample3(SAMPLE3_WORK* sw)
{
	switch(sw->seq){
	case 0:
		systemSetup(sw);

		sw->gflCamAdjust = GFL_CAMADJUST_Create(&camAdjustData, sw->heapID);
		GFL_CAMADJUST_SetCameraParam
			(sw->gflCamAdjust, &sw->cameraAngleV, &sw->cameraAngleH, &sw->cameraLength); 

		sw->seq++;
		break;

	case 1:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
			sw->seq = 3;
			break;
		}
		GFL_CAMADJUST_Main(sw->gflCamAdjust);

		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
			setSampleMsg1(sw, 0);
			sw->seq++;
		} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
		} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
		} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
		} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R ){
		}
		systemFramework(sw);
		break;

	case 2:
		GFL_CAMADJUST_Main(sw->gflCamAdjust);

		commFunc(sw);
		systemFramework(sw);

		if(sw->msgComm.pCommand == NULL){ sw->seq = 1; }
		break;

	case 3:
		GFL_CAMADJUST_Delete(sw->gflCamAdjust);
		systemDelete(sw);
		return FALSE;
	}
	return TRUE;
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�V�X�e���Z�b�g�A�b�v
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�f�B�X�v���C���f�[�^
 */
//------------------------------------------------------------------
///�u�q�`�l�o���N�ݒ�\����
static const GFL_DISP_VRAM dispVram = {
	GX_VRAM_BG_128_C,				//���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE,			//���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_32_H,			//�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE,		//�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E,				//���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE,		//���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_NONE,			//�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	//�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_01_AB,				//�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_0_G,			//�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};

static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	

static const GFL_BG_BGCNT_HEADER Textcont = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};
static const GFL_BG_BGCNT_HEADER Statuscont = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_128x128,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

//�J���������ݒ�f�[�^
static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 cameraPos	= { 0, (FX32_ONE * 32), (FX32_ONE * 64) };

//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light0Tbl[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light0Setup = { light0Tbl, NELEMS(light0Tbl) };

#include "arc/elboard_test.naix"

enum {
	G3DRES_ELBOARD2_BMD = 0,
	G3DRES_ELBOARD2_BTX,
	G3DRES_ELBOARD2_BTA,
};

static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard2_test_nsbmd, GFL_G3D_UTIL_RESARC },
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard2_test_nsbtx, GFL_G3D_UTIL_RESARC },
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard2_test_nsbta, GFL_G3D_UTIL_RESARC },
};

static const GFL_G3D_UTIL_ANM g3Dutil_anm2Tbl[] = {
	{ G3DRES_ELBOARD2_BTA, 0 },
};

enum {
	G3DOBJ_ELBOARD2 = 0,
};

static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
	{ G3DRES_ELBOARD2_BMD, 0, G3DRES_ELBOARD2_BTX, g3Dutil_anm2Tbl, NELEMS(g3Dutil_anm2Tbl) },
};

static const GFL_G3D_UTIL_SETUP g3Dutil_setup = {
	g3Dutil_resTbl, NELEMS(g3Dutil_resTbl),
	g3Dutil_objTbl, NELEMS(g3Dutil_objTbl),
};

static const GFL_G3D_OBJSTATUS g3DobjStatus1 = {
	{ 0, 0, 0 },																				//���W
	{ FX32_ONE, FX32_ONE, FX32_ONE },										//�X�P�[��
	{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },	//��]
};

//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ聕�f�[�^�]��
 */
//------------------------------------------------------------------
static void	bg_init(HEAPID heapID)
{
	//VRAM�ݒ�
	GFL_DISP_SetBank(&dispVram);

	//�a�f�V�X�e���N��
	GFL_BG_Init(heapID);

	//�a�f���[�h�ݒ�
	GFL_BG_SetBGMode(&bgsysHeader);

	//�a�f�R���g���[���ݒ�
	GFL_BG_SetBGControl(TEXT_FRAME, &Textcont, GFL_BG_MODE_TEXT);
	GFL_BG_SetPriority(TEXT_FRAME, 0);
	GFL_BG_SetVisible(TEXT_FRAME, VISIBLE_ON);
	GFL_BG_SetBGControl(STATUS_FRAME, &Statuscont, GFL_BG_MODE_TEXT);
	GFL_BG_SetPriority(STATUS_FRAME, 0);
	GFL_BG_SetVisible(STATUS_FRAME, VISIBLE_ON);

//	GFL_BG_FillCharacter(TEXT_FRAME, 0, 1, 0);	// �擪�ɃN���A�L�����z�u
//	GFL_BG_ClearScreen(TEXT_FRAME);

	//�R�c�V�X�e���N��
	GFL_G3D_Init
		(GFL_G3D_VMANLNK, GFL_G3D_TEX384K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K, 0, heapID, NULL );
	GFL_BG_SetBGControl3D(1);

	//�r�b�g�}�b�v�E�C���h�E�N��
	GFL_BMPWIN_Init(heapID);

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
	GFL_DISP_SetDispOn();
}

static void	bg_exit(void)
{
	GFL_G3D_Exit();
	GFL_BMPWIN_Exit();
	GFL_BG_FreeBGControl(STATUS_FRAME);
	GFL_BG_FreeBGControl(TEXT_FRAME);
	GFL_BG_Exit();
}

//------------------------------------------------------------------
/**
 * @brief		�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static void systemSetup(SAMPLE3_WORK* sw)
{
	//��{�Z�b�g�A�b�v
	bg_init(sw->heapID);
	//�t�H���g�p�p���b�g�]��
	GFL_BG_SetBackGroundColor( TEXT_FRAME, BACKGROUND_COLOR );
	//�t�H���g�n���h���쐬
	sw->fontHandle = GFL_FONT_Create
		(	ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, sw->heapID);

	//�J�����쐬
	sw->g3Dcamera = GFL_G3D_CAMERA_CreateDefault(&cameraPos, &cameraTarget, sw->heapID);
	GFL_G3D_CAMERA_Switching(sw->g3Dcamera);

	//�R�c�I�u�W�F�N�g�쐬
	{
		u16						objIdx, elboard1Idx;
		GFL_G3D_OBJ*	g3Dobj;
		GFL_G3D_RES*	g3Dtex;
		int i, anmCount;

		//���\�[�X�쐬
		sw->g3Dutil = GFL_G3D_UTIL_Create(NELEMS(g3Dutil_resTbl), NELEMS(g3Dutil_objTbl), sw->heapID );
		sw->g3DutilUnitIdx = GFL_G3D_UTIL_AddUnit( sw->g3Dutil, &g3Dutil_setup );

		//�A�j���[�V������L���ɂ���
		objIdx = GFL_G3D_UTIL_GetUnitObjIdx(sw->g3Dutil, sw->g3DutilUnitIdx);
		{
			elboard1Idx = objIdx + G3DOBJ_ELBOARD2;
			g3Dobj = GFL_G3D_UTIL_GetObjHandle(sw->g3Dutil, elboard1Idx);

			anmCount = GFL_G3D_OBJECT_GetAnimeCount(g3Dobj);
			for( i=0; i<anmCount; i++ ){ GFL_G3D_OBJECT_EnableAnime(g3Dobj, i); } 

			g3Dtex =	GFL_G3D_RENDER_GetG3DresTex(GFL_G3D_OBJECT_GetG3Drnd(g3Dobj));
		}
	}
	{
		TALKMSGWIN_SYS_INI ini = {TEXT_FRAME, WIN_PLTTID, TEXT_PLTTID};
		TALKMSGWIN_SYS_SETUP setup;		
		setup.heapID = sw->heapID;
		setup.g3Dcamera = sw->g3Dcamera;
		setup.fontHandle = sw->fontHandle;
		setup.ini = ini;

		sw->tmsgwinSys = TALKWINMSG_SystemCreate(&setup);
	}
}

//------------------------------------------------------------------
/**
 * @brief		�t���[�����[�N
 */
//------------------------------------------------------------------
#define PITCH_LIMIT (0x200)
static void systemFramework(SAMPLE3_WORK* sw)
{
	//�����ƃA���O���ɂ��J�����ʒu�v�Z
	{
		VecFx32 target = {0,0,0};
		VecFx32 cameraPos;
		VecFx32 vecCamera;
		fx16 sinYaw = FX_SinIdx(sw->cameraAngleV);
		fx16 cosYaw = FX_CosIdx(sw->cameraAngleV);
		fx16 sinPitch = FX_SinIdx(sw->cameraAngleH);
		fx16 cosPitch = FX_CosIdx(sw->cameraAngleH);
	
		if(cosPitch < 0){ cosPitch = -cosPitch; }	// �����肵�Ȃ��悤�Ƀ}�C�i�X�l�̓v���X�l�ɕ␳
		if(cosPitch < PITCH_LIMIT){ cosPitch = PITCH_LIMIT; }	// 0�l�ߕӂ͕s���\���ɂȂ邽�ߕ␳

		// �J�����̍��W�v�Z
		VEC_Set( &vecCamera, sinYaw * cosPitch, sinPitch * FX16_ONE, cosYaw * cosPitch);
		VEC_Normalize(&vecCamera, &vecCamera);
		VEC_MultAdd(sw->cameraLength, &vecCamera, &target, &cameraPos);

		GFL_G3D_CAMERA_SetPos(sw->g3Dcamera, &cameraPos);
	}
	TALKMSGWIN_SystemMain(sw->tmsgwinSys);
	TALKMSGWIN_SystemDraw2D(sw->tmsgwinSys);

	//�R�c�`��
	GFL_G3D_DRAW_Start();			//�`��J�n
	{
		GFL_G3D_CAMERA_Switching(sw->g3Dcamera);
		{
			u16						objIdx, elboard1Idx;
			GFL_G3D_OBJ*	g3Dobj;
			int i, anmCount;

			objIdx = GFL_G3D_UTIL_GetUnitObjIdx(sw->g3Dutil, sw->g3DutilUnitIdx );
			elboard1Idx = objIdx + G3DOBJ_ELBOARD2;
			g3Dobj = GFL_G3D_UTIL_GetObjHandle(sw->g3Dutil, elboard1Idx);

			GFL_G3D_DRAW_DrawObject(g3Dobj, &g3DobjStatus1);

			anmCount = GFL_G3D_OBJECT_GetAnimeCount(g3Dobj);
			for( i=0; i<anmCount; i++ ){ GFL_G3D_OBJECT_LoopAnimeFrame(g3Dobj, i, FX32_ONE ); } 
		}
		TALKMSGWIN_SystemDraw3D(sw->tmsgwinSys);

	}
	GFL_G3D_DRAW_End();				//�`��I���i�o�b�t�@�X���b�v�j					
}

//------------------------------------------------------------------
/**
 * @brief		�j��
 */
//------------------------------------------------------------------
static void systemDelete(SAMPLE3_WORK* sw)
{
	TALKMSGWIN_SystemDelete(sw->tmsgwinSys);

	GFL_G3D_UTIL_DelUnit(sw->g3Dutil, sw->g3DutilUnitIdx);
	GFL_G3D_UTIL_Delete(sw->g3Dutil);

	GFL_G3D_CAMERA_Delete(sw->g3Dcamera);

	GFL_FONT_Delete(sw->fontHandle);
	bg_exit();
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�R���g���[��
 *
 *
 *
 *
 *
 */
//============================================================================================
static BOOL calcTarget(GFL_G3D_CAMERA* g3Dcamera, int x, int y, VecFx32* target)
{
	BOOL result;
	fx32 near_backup;

	//�ϊ����x���グ�邽��near�̋������Ƃ�
	GFL_G3D_CAMERA_GetNear(g3Dcamera, &near_backup);
	{
		fx32 near = 64* FX32_ONE;

		GFL_G3D_CAMERA_SetNear(g3Dcamera, &near);
		GFL_G3D_CAMERA_Switching(g3Dcamera);
	}

	if( NNS_G3dScrPosToWorldLine(x, y, target, NULL) == -1 ){
		result = FALSE;
	} else {
		result = TRUE;
	}
	//near���A
	GFL_G3D_CAMERA_SetNear(g3Dcamera, &near_backup);
	GFL_G3D_CAMERA_Switching(g3Dcamera);

	return result;
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�R���g���[��
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	������쐬
 */
//------------------------------------------------------------------
static void makeStr(const u16* str, STRBUF* strBuf)
{
	STRCODE strTmp[STRBUF_SIZE];
	u32 strLen;

	//�����񒷂��擾
	const u16 checkLen = wcslen(str);
	if(checkLen >= STRBUF_SIZE){ strLen = STRBUF_SIZE - 1; }
	else { strLen = checkLen; }

	//�I�[�R�[�h��ǉ����Ă���STRBUF�ɕϊ�
	GFL_STD_MemCopy(str, strTmp, strLen*2);
	strTmp[strLen] = GFL_STR_GetEOMCode();

	GFL_STR_SetStringCode(strBuf, strTmp);
}

//------------------------------------------------------------------
/**
 * @brief	�e�X�g�R�}���h�e�[�u��
 */
//------------------------------------------------------------------
typedef BOOL (COMM_FUNC)(SAMPLE3_WORK* sw);
typedef struct {
	COMM_FUNC*	commFunc;
	int					commParamNum;
}COMM_TBL;

enum {
	COMM_DMY = 0,
	COMMEND,
	COMMWAIT,
	COMMSETMSG,
	COMMCONNECTMSG,
	COMMDELMSG,

	COMM_MAX,
};
static const COMM_TBL commTbl[COMM_MAX];

enum {
	MSG_TEST1 = 0,
	MSG_TEST2,
	MSG_TEST3,
	MSG_TEST4,
	MSG_TEST5,
	MSG_TEST6,
	MSG_TEST7,
	MSG_TEST8,
	MSG_TEST9,

	MSG_TEST_MAX,
};
static const u16* testMsgTbl[MSG_TEST_MAX];
//------------------------------------------------------------------
/**
 * @brief	�e�X�g�R�}���h
 */
//------------------------------------------------------------------
static void commFunc(SAMPLE3_WORK* sw)
{
	int i;

	if(sw->msgComm.pCommand == NULL){ return; }

	if(sw->msgComm.command != 0){
		if( commTbl[sw->msgComm.command].commFunc(sw) == TRUE ){ return; }
	}

	sw->msgComm.command = *(sw->msgComm.pCommand);
	(sw->msgComm.pCommand)++;

	for( i=0; i<commTbl[sw->msgComm.command].commParamNum; i++ ){
		sw->msgComm.commParam[i] = *(sw->msgComm.pCommand);
		(sw->msgComm.pCommand)++;
	}
}

//------------------------------------------------------------------
static BOOL commEnd(SAMPLE3_WORK* sw)
{
	sw->msgComm.pCommand = NULL;
	sw->msgComm.command = 0;
	return TRUE;
}

//------------------------------------------------------------------
static BOOL commWait(SAMPLE3_WORK* sw)
{
	if(sw->msgComm.commParam[0] == 0){ return FALSE; }
	sw->msgComm.commParam[0]--;
	return TRUE;
}

//------------------------------------------------------------------
static BOOL commSetMsg(SAMPLE3_WORK* sw)
{
	TALKMSGWIN_SETUP tmsgwinSetup;
	u16	winIdx = sw->msgComm.commParam[0];

	tmsgwinSetup.color = GX_RGB(31,31,31);
	tmsgwinSetup.winpx = sw->msgComm.commParam[1];
	tmsgwinSetup.winpy = sw->msgComm.commParam[2];
	tmsgwinSetup.winsx = sw->msgComm.commParam[3];
	tmsgwinSetup.winsy = sw->msgComm.commParam[4];
	makeStr(testMsgTbl[sw->msgComm.commParam[5]], sw->strBuf[winIdx]);
	
	calcTarget(	sw->g3Dcamera, 
							sw->msgComm.commParam[6], 
							sw->msgComm.commParam[7], 
							&sw->twinTarget[winIdx]);

	TALKMSGWIN_CreateWindowIdx(	sw->tmsgwinSys,
															winIdx,
															&sw->twinTarget[winIdx],
															sw->strBuf[winIdx],
															&tmsgwinSetup);

	TALKMSGWIN_OpenWindow(sw->tmsgwinSys, winIdx);

	return FALSE;
}

//------------------------------------------------------------------
static BOOL commConnectMsg(SAMPLE3_WORK* sw)
{
	TALKMSGWIN_SETUP tmsgwinSetup;
	u16	winIdx = sw->msgComm.commParam[0];

	tmsgwinSetup.color = GX_RGB(31,31,31);
	tmsgwinSetup.winpx = sw->msgComm.commParam[1];
	tmsgwinSetup.winpy = sw->msgComm.commParam[2];
	tmsgwinSetup.winsx = sw->msgComm.commParam[3];
	tmsgwinSetup.winsy = sw->msgComm.commParam[4];
	makeStr(testMsgTbl[sw->msgComm.commParam[5]], sw->strBuf[winIdx]);
	
	TALKMSGWIN_CreateWindowIdxConnect(sw->tmsgwinSys,
																		winIdx,
																		sw->msgComm.commParam[6],
																		sw->strBuf[winIdx],
																		&tmsgwinSetup);

	TALKMSGWIN_OpenWindow(sw->tmsgwinSys, winIdx);

	return FALSE;
}

//------------------------------------------------------------------
static BOOL commDelMsg(SAMPLE3_WORK* sw)
{
	TALKMSGWIN_DeleteWindow(sw->tmsgwinSys, sw->msgComm.commParam[0]);

	return FALSE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static const COMM_TBL commTbl[COMM_MAX] = {
	{ NULL,							0 },
	{ commEnd,					0 },
	{ commWait,					1 },
	{ commSetMsg,				8 },
	{ commConnectMsg,		7 },
	{ commDelMsg,				1 },
};


//------------------------------------------------------------------
//------------------------------------------------------------------
static const u16* testMsgTbl[MSG_TEST_MAX] = {
	{ L"����ł��̂P�������łȂ��E�E�E" },
	{ L"�S���E�E�E�I" },
	{ L"�܂胉�X�g�E�E�E" },
	{ L"�P�V���܂ŃZ�[�t�E�E�E�I" },
	{ L"�����؂�E�E�E�I" },
	{ L"�_���I�т��́E�E�E" },
	{ L"��͂�E�E�E" },
	{ L"�킵�E�E�E�I" },
	{ L"����" },
};

//------------------------------------------------------------------
//------------------------------------------------------------------
static const u16 sampleMsg1[] = {
	COMMSETMSG, 0, 10, 1, 20, 2, MSG_TEST1, 248, 184,
	COMMWAIT, 120,
	COMMCONNECTMSG, 1, 20, 6, 6, 2, MSG_TEST2, 0,
	COMMWAIT, 120,
	COMMCONNECTMSG, 2, 3, 6, 12, 2, MSG_TEST3, 1,
	COMMWAIT, 120,
	COMMCONNECTMSG, 3, 1, 11, 14, 2, MSG_TEST4, 2,
	COMMWAIT, 120,
	COMMCONNECTMSG, 4, 20, 11, 10, 2, MSG_TEST5, 3,
	COMMWAIT, 120,
	COMMCONNECTMSG, 5, 18, 16, 12, 2, MSG_TEST6, 4,
	COMMWAIT, 120,
	COMMCONNECTMSG, 6, 6, 16, 8, 2, MSG_TEST7, 5,
	COMMWAIT, 120,
	COMMCONNECTMSG, 7, 16, 20, 6, 2, MSG_TEST8, 6,
	COMMWAIT, 60,

	COMMSETMSG, 8, 6, 20, 6, 2, MSG_TEST9, 8, 184,
	COMMWAIT, 120,

	COMMDELMSG, 0,
	COMMDELMSG, 1,
	COMMDELMSG, 2,
	COMMDELMSG, 3,
	COMMDELMSG, 4,
	COMMDELMSG, 5,
	COMMDELMSG, 6,
	COMMDELMSG, 7,
	COMMDELMSG, 8,
	COMMEND,
};

static const u16* sampleMsgTbl[] = {
	sampleMsg1, sampleMsg1, sampleMsg1, sampleMsg1,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
static void setSampleMsg1(SAMPLE3_WORK* sw, int idx)
{
	sw->msgComm.pCommand = sampleMsgTbl[idx];
}

