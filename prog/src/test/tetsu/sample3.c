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
#include "msg/msg_d_tetsu.h"

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
#define BACKGROUND_COLOR2 (0x7c00)
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

	GFL_CAMADJUST*			gflCamAdjust;
	fx32								cameraLength;
	u16									cameraAngleV;
	u16									cameraAngleH;
  // camera_adjust�𓮂������߂ɒǉ����܂����B tomoya takahashi
	u16									cameraFovy;
  u16                 padding;
	fx32								cameraFar;

	int									timer;

	TALKMSGWIN_SYS*			tmsgwinSys;
	int									tmsgwinIdx;
	VecFx32							cameraTarget;
	BOOL								tmsgwinConnect;

	GFL_MSGDATA*				msgManager;
	STRBUF*							strBuf[TALKMSGWIN_NUM];
	VecFx32							twinTarget[TALKMSGWIN_NUM];

	TEST_MSG_COMM				msgComm;

	int									mode;

	int									testPat;
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
	sw->mode = 0;

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
static void commFunc(SAMPLE3_WORK* sw);

static void setSampleMsg1(SAMPLE3_WORK* sw, int idx);
//------------------------------------------------------------------
/**
 *
 * @brief	�t���[�����[�N
 *
 */
//------------------------------------------------------------------
#define FIX_WIN_IDX	(0)
static BOOL	sample3(SAMPLE3_WORK* sw)
{
	switch(sw->seq){
	case 0:
		systemSetup(sw);

		sw->gflCamAdjust = GFL_CAMADJUST_Create(&camAdjustData, sw->heapID);

		sw->cameraAngleV = 0;
		sw->cameraAngleH = 0;
		sw->cameraLength = 8*FX32_ONE; 
    sw->cameraFovy   = defaultCameraFovy/2 *PERSPWAY_COEFFICIENT;
    sw->cameraFar    = defaultCameraFar;
		GFL_CAMADJUST_SetCameraParam
			(sw->gflCamAdjust, &sw->cameraAngleV, &sw->cameraAngleH, &sw->cameraLength, &sw->cameraFovy, &sw->cameraFar); 

		sw->testPat = 0;
		sw->seq++;
		break;

	case 1:
		if( sw->testPat == 0 ){
			VEC_Set(&sw->cameraTarget,0,0,0);
			GFL_BG_SetBackGroundColor( TEXT_FRAME, BACKGROUND_COLOR );
		} else {
			VEC_Set(&sw->cameraTarget,0x10000*FX32_ONE,0,0*FX32_ONE);
			GFL_BG_SetBackGroundColor( TEXT_FRAME, BACKGROUND_COLOR2 );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
			sw->seq = 4;
			break;
		}
		if( sw->mode == 0 ){
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
				GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_SUB);
				sw->mode = 1;
				break;
			}
			GFL_CAMADJUST_Main(sw->gflCamAdjust);

			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
				setSampleMsg1(sw, 0);
				sw->seq = 2;
			} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
			} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
				sw->cameraAngleV = 0;
				sw->cameraAngleH = 0;
				sw->cameraLength = 8*FX32_ONE; 
			} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
			} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
			} else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R ){
				if(sw->testPat)	{ sw->testPat = 0; }
				else						{ sw->testPat = 1; }
			}
		} else {
			u32 tpx, tpy;

			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
				GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
				sw->mode = 0;
				break;
			}
			if( GFL_UI_TP_GetPointTrg( &tpx, &tpy ) == TRUE ){
				GFL_MSG_GetString(sw->msgManager, DEBUG_TETSU_STR0_4, sw->strBuf[FIX_WIN_IDX]);
				//calcTarget(	sw->g3Dcamera, tpx, tpy, &sw->twinTarget[FIX_WIN_IDX]);
				VEC_Set(&sw->twinTarget[FIX_WIN_IDX], 
								sw->cameraTarget.x + 16*FX32_ONE,
								sw->cameraTarget.y - 16*FX32_ONE,
								sw->cameraTarget.z + 16*FX32_ONE);

				TALKMSGWIN_CreateFixWindowAuto
					(sw->tmsgwinSys, FIX_WIN_IDX, &sw->twinTarget[FIX_WIN_IDX], sw->strBuf[FIX_WIN_IDX], 15);
				TALKMSGWIN_OpenWindow(sw->tmsgwinSys, FIX_WIN_IDX);
				sw->seq = 3;
			}
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
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
			TALKMSGWIN_DeleteWindow(sw->tmsgwinSys, FIX_WIN_IDX);
			sw->seq = 1;
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
			if( sw->mode == 0 ){
				GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_SUB);
				sw->mode = 1;
			} else {
				GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
				sw->mode = 0;
			}
			break;
		} 
		if( sw->mode == 0 ){ GFL_CAMADJUST_Main(sw->gflCamAdjust); }

		systemFramework(sw);
		break;

	case 4:
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
	//�t�H���g�n���h���쐬
	sw->fontHandle = GFL_FONT_Create
		(	ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, sw->heapID);
	//���b�Z�[�W�}�l�[�W���쐬
	sw->msgManager = GFL_MSG_Create
		(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_tetsu_dat, sw->heapID);

	//�J�����쐬
	sw->g3Dcamera = GFL_G3D_CAMERA_CreateDefault(&cameraPos, &cameraTarget, sw->heapID);
	GFL_G3D_CAMERA_Switching(sw->g3Dcamera);

	{
		TALKMSGWIN_SYS_INI ini = {TEXT_FRAME, WIN_PLTTID, TEXT_PLTTID};
		TALKMSGWIN_SYS_SETUP setup;		
		setup.heapID = sw->heapID;
		setup.g3Dcamera = sw->g3Dcamera;
		setup.fontHandle = sw->fontHandle;
		setup.chrNumOffs = 0x10;
		setup.ini = ini;

		sw->tmsgwinSys = TALKMSGWIN_SystemCreate(&setup);
		TALKMSGWIN_SystemDebugOn(sw->tmsgwinSys);
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
		//VecFx32 target = {12136*FX32_ONE,0,12968*FX32_ONE};
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
		VEC_MultAdd(sw->cameraLength, &vecCamera, &sw->cameraTarget, &cameraPos);

		GFL_G3D_CAMERA_SetTarget(sw->g3Dcamera, &sw->cameraTarget);
		GFL_G3D_CAMERA_SetPos(sw->g3Dcamera, &cameraPos);
		GFL_G3D_CAMERA_Switching(sw->g3Dcamera);
	}
	TALKMSGWIN_SystemMain(sw->tmsgwinSys);
	TALKMSGWIN_SystemDraw2D(sw->tmsgwinSys);

	//�R�c�`��
	GFL_G3D_DRAW_Start();			//�`��J�n
	GFL_G3D_DRAW_SetLookAt();
	TALKMSGWIN_SystemDraw3D(sw->tmsgwinSys);

	{
		VecFx32 pos, up, target;

		G3X_Reset();

		GFL_G3D_CAMERA_GetPos(sw->g3Dcamera, &pos);
		GFL_G3D_CAMERA_GetCamUp(sw->g3Dcamera, &up);
		GFL_G3D_CAMERA_GetTarget(sw->g3Dcamera, &target);
		G3_LookAt(&pos, &up, &target, NULL);

		G3_PushMtx();
		//���s�ړ��p�����[�^�ݒ�
		G3_Translate(sw->cameraTarget.x, sw->cameraTarget.y, sw->cameraTarget.z);

		//�O���[�o���X�P�[���ݒ�
		G3_Scale(32 * FX32_ONE, 32 * FX32_ONE, 32 * FX32_ONE);

		G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, 0, 0, 0, 0, GX_TEXPLTTCOLOR0_USE, 0);
	
		//�}�e���A���ݒ�
		G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(16, 16, 16), TRUE);
		G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);
		G3_PolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 63, 31, 0);
		
		G3_Begin( GX_BEGIN_QUADS );
	
		G3_Color(GX_RGB(31,0,0));
	
		G3_Vtx(-(FX16_ONE-1), 0, (FX16_ONE-1));
		G3_Vtx(-(FX16_ONE-1), 0, -(FX16_ONE-1));
		G3_Vtx((FX16_ONE-1), 0, -(FX16_ONE-1));
		G3_Vtx((FX16_ONE-1), 0, (FX16_ONE-1));
	
		G3_End();
		G3_PopMtx(1);
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

	GFL_G3D_CAMERA_Delete(sw->g3Dcamera);

	GFL_MSG_Delete(sw->msgManager);
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
	u16	winIdx = sw->msgComm.commParam[0];
	u16	msgID = sw->msgComm.commParam[3];
	u16	msgWidth;
	u16	msgHeight = 2;

	GFL_MSG_GetString(sw->msgManager, msgID, sw->strBuf[winIdx]);
	msgWidth = GFL_MSG_GetDispAreaWidth(sw->msgManager, msgID) / 8;

//	calcTarget(	sw->g3Dcamera, 
//							sw->msgComm.commParam[4], 
//							sw->msgComm.commParam[5], 
//							&sw->twinTarget[winIdx]);
	VEC_Set(&sw->twinTarget[winIdx], 
					sw->cameraTarget.x + 16*FX32_ONE,
					sw->cameraTarget.y - 16*FX32_ONE,
					sw->cameraTarget.z + 16*FX32_ONE);

	TALKMSGWIN_CreateFloatWindowIdx(	sw->tmsgwinSys,
																		winIdx,
																		&sw->twinTarget[winIdx],
																		sw->strBuf[winIdx],
																		sw->msgComm.commParam[1] - msgWidth/2,
																		sw->msgComm.commParam[2],
																		msgWidth, msgHeight,
																		15);

	TALKMSGWIN_OpenWindow(sw->tmsgwinSys, winIdx);

	return FALSE;
}

//------------------------------------------------------------------
static BOOL commConnectMsg(SAMPLE3_WORK* sw)
{
	u16	winIdx = sw->msgComm.commParam[0];
	u16	msgID = sw->msgComm.commParam[3];
	u16	msgWidth;
	u16	msgHeight = 2;

	GFL_MSG_GetString(sw->msgManager, msgID, sw->strBuf[winIdx]);
	msgWidth = GFL_MSG_GetDispAreaWidth(sw->msgManager, msgID) / 8;
	
	TALKMSGWIN_CreateFloatWindowIdxConnect( sw->tmsgwinSys,
																					winIdx,
																					sw->msgComm.commParam[4],
																					sw->strBuf[winIdx],
																					sw->msgComm.commParam[1] - msgWidth/2,
																					sw->msgComm.commParam[2],
																					msgWidth, msgHeight,
																					15);

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
	{ commSetMsg,				6 },
	{ commConnectMsg,		5 },
	{ commDelMsg,				1 },
};


//------------------------------------------------------------------
//------------------------------------------------------------------
static const u16 sampleMsg1[] = {
	COMMSETMSG,				0, 20, 2, DEBUG_TETSU_STR0_1, 128, 160,
	COMMWAIT,					120,
	COMMCONNECTMSG,		1, 19, 8, DEBUG_TETSU_STR0_2, 0,
	COMMWAIT,					120,
	COMMCONNECTMSG,		2, 18, 14, DEBUG_TETSU_STR0_3, 1,
	COMMWAIT,					120,

	COMMDELMSG,				0,
	COMMDELMSG,				1,
	COMMDELMSG,				2,
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

