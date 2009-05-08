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

#include "system/el_scoreboard.h"

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
static const u16 testMsg[] = { L"�d���������΂������Ă݂悤�I�I�E�E�E�E�E�E" };

static const GFL_CAMADJUST_SETUP camAdjustData= {
	0,
	GFL_DISPUT_BGID_S0, GFL_DISPUT_PALID_15,
};

//============================================================================================
/**
 *
 * @brief	�\���̒�`
 *
 */
//============================================================================================
typedef struct {
	HEAPID							heapID;
	int									seq;

	STRBUF*							strBuf;

	GFL_G3D_CAMERA*			g3Dcamera;	
	GFL_G3D_LIGHTSET*		g3Dlightset;	
	GFL_G3D_UTIL*				g3Dutil;
	u16									g3DutilUnitIdx;

	GFL_CAMADJUST*			gflCamAdjust;
	fx32								cameraLength;
	u16									cameraAngleV;
	u16									cameraAngleH;

	u16									targetObjID;

	int									timer;
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

static void makeStr2(const u16* str, STRBUF* strBuf)
{
	STRCODE strTmp[STRBUF_SIZE];
	u32 strLen;
	u16*	pStr = strTmp;

	//�����񒷂��擾
	const u16 checkLen = wcslen(str);
	if(checkLen >= STRBUF_SIZE/2){ strLen = STRBUF_SIZE/2; }
	else { strLen = checkLen; }

	//�I�[�R�[�h��ǉ����Ă���STRBUF�ɕϊ�
	{
		int i;
		for( i=0; i<strLen; i++ ){
			pStr[i*2+0] = str[i];
			if( i== strLen-1){
				pStr[i*2+1] = GFL_STR_GetEOMCode();
			} else {
				pStr[i*2+1] = 0xfffe;
			}
		}
	}
	GFL_STR_SetStringCode(strBuf, strTmp);
}

//------------------------------------------------------------------
/**
 * @brief	���[�N�̏��������j��
 */
//------------------------------------------------------------------
static void	workInitialize(SAMPLE3_WORK* sw)
{
	sw->seq = 0;
	sw->timer = 0;

	sw->strBuf = GFL_STR_CreateBuffer(STRBUF_SIZE, sw->heapID);
	makeStr(testMsg, sw->strBuf);
}

static void	workFinalize(SAMPLE3_WORK* sw)
{
	GFL_STR_DeleteBuffer(sw->strBuf);
}

//------------------------------------------------------------------
/**
 * @brief	proc�֐�
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT Sample3Proc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	SAMPLE3_WORK* sw = NULL;

	sw = GFL_PROC_AllocWork(proc, sizeof(SAMPLE3_WORK), GFL_HEAPID_APP);

  GFL_STD_MemClear(sw, sizeof(SAMPLE3_WORK));
  sw->heapID = GFL_HEAPID_APP;

	workInitialize(sw);

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
	sw = mywk;

	workFinalize(sw);

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
			sw->seq++;
			break;
		}
		GFL_CAMADJUST_Main(sw->gflCamAdjust);

		systemFramework(sw);
		break;

	case 2:
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
	G3DRES_ELBOARD_BMD = 0,
	G3DRES_ELBOARD_BTX,
	G3DRES_ELBOARD_BCA,
	G3DRES_ELBOARD_BTA,
	G3DRES_ELBOARD2_BMD,
	G3DRES_ELBOARD2_BTX,
	G3DRES_ELBOARD2_BTA,
};

static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard_test_nsbmd, GFL_G3D_UTIL_RESARC },
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard_test_nsbtx, GFL_G3D_UTIL_RESARC },
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard_test_nsbca, GFL_G3D_UTIL_RESARC },
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard_test_nsbta, GFL_G3D_UTIL_RESARC },
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard2_test_nsbmd, GFL_G3D_UTIL_RESARC },
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard2_test_nsbtx, GFL_G3D_UTIL_RESARC },
	{	ARCID_ELBOARD_TEST, NARC_elboard_test_elboard2_test_nsbta, GFL_G3D_UTIL_RESARC },
};

static const GFL_G3D_UTIL_ANM g3Dutil_anm1Tbl[] = {
	{ G3DRES_ELBOARD_BCA, 0 },
	{ G3DRES_ELBOARD_BTA, 0 },
};

static const GFL_G3D_UTIL_ANM g3Dutil_anm2Tbl[] = {
	{ G3DRES_ELBOARD2_BTA, 0 },
};

enum {
	G3DOBJ_ELBOARD1 = 0,
	G3DOBJ_ELBOARD2,
};

static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
	{ G3DRES_ELBOARD_BMD, 0, G3DRES_ELBOARD_BTX, g3Dutil_anm1Tbl, NELEMS(g3Dutil_anm1Tbl) },
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

	GFL_BG_FillCharacter(TEXT_FRAME, 0, 1, 0);	// �擪�ɃN���A�L�����z�u
	GFL_BG_ClearScreen(TEXT_FRAME);

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
			elboard1Idx = objIdx + G3DOBJ_ELBOARD1;
			g3Dobj = GFL_G3D_UTIL_GetObjHandle(sw->g3Dutil, elboard1Idx);

			anmCount = GFL_G3D_OBJECT_GetAnimeCount(g3Dobj);
			for( i=0; i<anmCount; i++ ){ GFL_G3D_OBJECT_EnableAnime(g3Dobj, i); } 

			g3Dtex =	GFL_G3D_RENDER_GetG3DresTex(GFL_G3D_OBJECT_GetG3Drnd(g3Dobj));
		} 
		{
			elboard1Idx = objIdx + G3DOBJ_ELBOARD2;
			g3Dobj = GFL_G3D_UTIL_GetObjHandle(sw->g3Dutil, elboard1Idx);

			anmCount = GFL_G3D_OBJECT_GetAnimeCount(g3Dobj);
			for( i=0; i<anmCount; i++ ){ GFL_G3D_OBJECT_EnableAnime(g3Dobj, i); } 

			g3Dtex =	GFL_G3D_RENDER_GetG3DresTex(GFL_G3D_OBJECT_GetG3Drnd(g3Dobj));
		}
		sw->targetObjID = G3DOBJ_ELBOARD1;
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
	//�R�c�`��
	{
		VecFx32 target = {0,0,0};
		VecFx32 cameraPos;

		//�����ƃA���O���ɂ��J�����ʒu�v�Z
		{
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
			GFL_G3D_CAMERA_Switching(sw->g3Dcamera);
		}

		GFL_G3D_DRAW_Start();			//�`��J�n
		GFL_G3D_DRAW_SetLookAt();	//�J�����O���[�o���X�e�[�g�ݒ�		
		{
			u16						objIdx, elboard1Idx;
			GFL_G3D_OBJ*	g3Dobj;
			int i, anmCount;

			objIdx = GFL_G3D_UTIL_GetUnitObjIdx(sw->g3Dutil, sw->g3DutilUnitIdx );
			elboard1Idx = objIdx + sw->targetObjID;
			g3Dobj = GFL_G3D_UTIL_GetObjHandle(sw->g3Dutil, elboard1Idx);

			GFL_G3D_DRAW_DrawObject(g3Dobj, &g3DobjStatus1);

			anmCount = GFL_G3D_OBJECT_GetAnimeCount(g3Dobj);
			for( i=0; i<anmCount; i++ ){ GFL_G3D_OBJECT_LoopAnimeFrame(g3Dobj, i, FX32_ONE ); } 
		}
		GFL_G3D_DRAW_End();				//�`��I���i�o�b�t�@�X���b�v�j					
	}
}

//------------------------------------------------------------------
/**
 * @brief		�j��
 */
//------------------------------------------------------------------
static void systemDelete(SAMPLE3_WORK* sw)
{
	GFL_G3D_UTIL_DelUnit(sw->g3Dutil, sw->g3DutilUnitIdx);
	GFL_G3D_UTIL_Delete(sw->g3Dutil);

	GFL_G3D_CAMERA_Delete(sw->g3Dcamera);

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




//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�`��
 *
 *
 *
 *
 *
 */
//============================================================================================
static void	drawObject( VecFx32* trans, fx32 scale, fx16* vtxData )
{
	//�O���[�o���X�P�[���ݒ�
	//�����_���W�ݒ�l�ɐ���������̂ŁA�e�N�X�`���T�C�Y�ɍ��킹�邽�߃X�P�[���̕��ŋz������
	scale *= FX16_ONE;
	G3_Scale( scale, scale, scale );

	G3_PushMtx();

	//���s�ړ��p�����[�^�ݒ�
	G3_Translate(FX_Div(trans->x,scale), FX_Div(trans->y,scale), FX_Div(trans->z,scale));

	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb( GX_RGB(31, 31, 31), GX_RGB(0, 0, 0), TRUE );
	G3_MaterialColorSpecEmi( GX_RGB(31, 31, 31), GX_RGB(0, 0, 0), FALSE );
	G3_PolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
									63, 31, GX_POLYGON_ATTR_MISC_FOG );
	
	G3_Begin( GX_BEGIN_QUADS );

	G3_Color(GX_RGB(31, 31, 31));

//	G3_Vtx(vtxData[0][0], vtxData[0][1], 0);
//	G3_Vtx(vtxData[1][1], vtxData[1][1], 0);
//	G3_Vtx(vtxData[2][1], vtxData[2][1], 0);
//	G3_Vtx(vtxData[3][1], vtxData[3][1], 0);

	G3_End();
	G3_PopMtx(1);
}


