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
typedef struct {
	VecFx16	vtx0;
	VecFx16	vtx1;
	VecFx16	vtx2;
}DRAW_TRIANGLE;

typedef struct {
	VecFx16	vtx0;
	VecFx16	vtx1;
	VecFx16	vtx2;
	VecFx16	vtx3;
}DRAW_QUAD;

static BOOL calcTriangle
	(GFL_G3D_CAMERA* g3Dcamera, DRAW_TRIANGLE* triangle, VecFx32* trans, fx32* scale, int x, int y );
static void	drawTriangle( VecFx32* trans, fx32 scale, DRAW_TRIANGLE* triData, GXRgb color );
static void	drawQuad( VecFx32* trans, fx32 scale, DRAW_QUAD* quadData, GXRgb color );
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

	GFL_G3D_CAMERA*			g3Dcamera;	
	GFL_G3D_LIGHTSET*		g3Dlightset;	
	GFL_G3D_UTIL*				g3Dutil;
	u16									g3DutilUnitIdx;

	GFL_CAMADJUST*			gflCamAdjust;
	fx32								cameraLength;
	u16									cameraAngleV;
	u16									cameraAngleH;

	int									timer;

	BOOL								cameraControlMode;

	u32									tpx;
	u32									tpy;
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

	sw = GFL_PROC_AllocWork(proc, sizeof(SAMPLE3_WORK), GFL_HEAPID_APP);

  GFL_STD_MemClear(sw, sizeof(SAMPLE3_WORK));
  sw->heapID = GFL_HEAPID_APP;
	sw->seq = 0;
	sw->timer = 0;
	sw->cameraControlMode = TRUE;
	sw->tpx = 0xffff;
	sw->tpy = 0xffff;

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
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
			if(sw->cameraControlMode == FALSE){
				GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
				sw->cameraControlMode = TRUE;
			} else {
				GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_SUB);
				sw->cameraControlMode = FALSE;
			}
			break;
		}

		if(sw->cameraControlMode == TRUE){
			GFL_CAMADJUST_Main(sw->gflCamAdjust);
		} else {
			GFL_UI_TP_GetPointCont(&sw->tpx, &sw->tpy);
		}

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
			elboard1Idx = objIdx + G3DOBJ_ELBOARD2;
			g3Dobj = GFL_G3D_UTIL_GetObjHandle(sw->g3Dutil, elboard1Idx);

			anmCount = GFL_G3D_OBJECT_GetAnimeCount(g3Dobj);
			for( i=0; i<anmCount; i++ ){ GFL_G3D_OBJECT_EnableAnime(g3Dobj, i); } 

			g3Dtex =	GFL_G3D_RENDER_GetG3DresTex(GFL_G3D_OBJECT_GetG3Drnd(g3Dobj));
		}
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
		{
			DRAW_TRIANGLE	tri0;
			VecFx32				triTrans;
			fx32					triScale;

			if((sw->tpx != 0xffff)&&(sw->tpy != 0xffff)){
				if(calcTriangle(sw->g3Dcamera, &tri0, &triTrans, &triScale, sw->tpx, sw->tpy) == TRUE){
					drawTriangle(&triTrans, triScale, &tri0, GX_RGB(31, 31, 31));
				}
			}
		}
		{
			DRAW_QUAD	plane0 =	{ {-FX32_ONE, 0,  FX32_ONE}, { FX32_ONE, 0,  FX32_ONE},
														{-FX32_ONE, 0, -FX32_ONE}, { FX32_ONE, 0, -FX32_ONE} };
			VecFx32		planeTrans = {64*FX32_ONE, 64*FX32_ONE, 64*FX32_ONE};
			fx32			planeScale = FX32_ONE * 64;

			drawQuad(&planeTrans, planeScale, &plane0, GX_RGB(31, 0, 0));
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
static void	drawTriangle( VecFx32* trans, fx32 scale, DRAW_TRIANGLE* triData, GXRgb color )
{
	G3_PushMtx();
	//���s�ړ��p�����[�^�ݒ�
	G3_Translate(trans->x, trans->y, trans->z);
	//�O���[�o���X�P�[���ݒ�
	G3_Scale( scale, scale, scale );

	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb( GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), TRUE );
	G3_MaterialColorSpecEmi( GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), FALSE );
	G3_PolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
									63, 31, GX_POLYGON_ATTR_MISC_DISP_1DOT );
	
	G3_Begin( GX_BEGIN_TRIANGLES );

	G3_Color(color);

	G3_Vtx(	triData->vtx2.x, triData->vtx2.y, triData->vtx2.z); 
	G3_Vtx(	triData->vtx0.x, triData->vtx0.y, triData->vtx0.z); 
	G3_Vtx(	triData->vtx1.x, triData->vtx1.y, triData->vtx1.z); 

	G3_End();
	G3_PopMtx(1);
}

static void	drawQuad( VecFx32* trans, fx32 scale, DRAW_QUAD* quadData, GXRgb color )
{
	G3_PushMtx();

	//���s�ړ��p�����[�^�ݒ�
	G3_Translate(trans->x, trans->y, trans->z);
	//�O���[�o���X�P�[���ݒ�
	G3_Scale( scale, scale, scale );

	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb( GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), TRUE );
	G3_MaterialColorSpecEmi( GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), FALSE );
	G3_PolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
									63, 31, GX_POLYGON_ATTR_MISC_DISP_1DOT );
	
	G3_Begin( GX_BEGIN_QUADS );

	G3_Color(color);

	G3_Vtx(quadData->vtx2.x, quadData->vtx2.y, quadData->vtx2.z); 
	G3_Vtx(quadData->vtx0.x, quadData->vtx0.y, quadData->vtx0.z); 
	G3_Vtx(quadData->vtx1.x, quadData->vtx1.y, quadData->vtx1.z); 
	G3_Vtx(quadData->vtx3.x, quadData->vtx3.y, quadData->vtx3.z); 

	G3_End();
	G3_PopMtx(1);
}


//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	���W�v�Z
 *
 *
 *
 *
 *
 */
//============================================================================================
static BOOL calcTriangle
	(GFL_G3D_CAMERA* g3Dcamera, DRAW_TRIANGLE* triangle, VecFx32* trans, fx32* scale, int x, int y )
{
	VecFx32		vec0, vec1, vec2;
	fx32			val1, val2;
	fx32			near_backup;

	//�ϊ����x���グ�邽��near�̋������Ƃ�
	GFL_G3D_CAMERA_GetNear(g3Dcamera, &near_backup);
	{
		fx32 near = 64* FX32_ONE;

		GFL_G3D_CAMERA_SetNear(g3Dcamera, &near);
		GFL_G3D_CAMERA_Switching(g3Dcamera);
	}

	if( NNS_G3dScrPosToWorldLine(x, y, &vec0, NULL ) == -1 ){ return FALSE; }

	NNS_G3dScrPosToWorldLine(8, 18*8, &vec1, NULL );
	NNS_G3dScrPosToWorldLine(32, 18*8, &vec2, NULL );

	VEC_Subtract(&vec1, &vec0, &vec1);
	VEC_Subtract(&vec2, &vec0, &vec2);

	val1 = VEC_Mag(&vec1);
	val2 = VEC_Mag(&vec2);

	//���������X�P�[���ɐݒ�
	if( val1 >= val2 ){ *scale = val1; }
	else { *scale = val2; }

	VEC_Set(trans, vec0.x, vec0.y, vec0.z);
	VEC_Fx16Set(&triangle->vtx0, 0, 0, 0);
	VEC_Fx16Set(&triangle->vtx1, FX_Div(vec1.x,*scale), FX_Div(vec1.y,*scale), FX_Div(vec1.z,*scale));
	VEC_Fx16Set(&triangle->vtx2, FX_Div(vec2.x,*scale), FX_Div(vec2.y,*scale), FX_Div(vec2.z,*scale));

	//near���A
	GFL_G3D_CAMERA_SetNear(g3Dcamera, &near_backup);
	GFL_G3D_CAMERA_Switching(g3Dcamera);

	return TRUE;
}

