//============================================================================================
/**
 * @file	sample2.c
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

typedef struct _EL_SCOREBOARD	EL_SCOREBOARD;

typedef enum {
	ELB_TEXSIZ_16x16 = 0,
	ELB_TEXSIZ_32x16,
	ELB_TEXSIZ_64x16,
	ELB_TEXSIZ_128x16,
	ELB_TEXSIZ_256x16,
	ELB_TEXSIZ_512x16,
	ELB_TEXSIZ_1024x16,
}ELB_TEXSIZ;

EL_SCOREBOARD*	ELBOARD_Add( ELB_TEXSIZ sizMode, u8 strLen, HEAPID heapID );
void						ELBOARD_Delete( EL_SCOREBOARD* elb );
void						ELBOARD_Main( EL_SCOREBOARD* elb );
void						ELBOARD_Draw( EL_SCOREBOARD* elb, 
															VecFx32* trans, fx32 scale, fx16 sizX, fx16 sizY, 
															GFL_G3D_CAMERA* g3Dcamera, GFL_G3D_LIGHTSET* g3Dlightset );
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

//------------------------------------------------------------------
/**
 * @brief		�a�f�`��f�[�^
 */
//------------------------------------------------------------------
static const u16 plttData[PLTT_SIZ/2] = { 
	0x0000, 0x18C6, 0x2108, 0x021F, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

static const u16 plttData2[PLTT_SIZ/2] = { 
	0x0000, 0x18C6, 0x2108, 0x031F, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

#define BCOL1 (1)
#define BCOL2 (2)
#define LCOL	(3)
#define COLDATA_LETTER1 (0x21f)
#define COLDATA_LETTER2 (0x31f)
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

	PRINT_QUE*					printQue;
	GFL_FONT*						fontHandle;

	GFL_BMPWIN*					bmpwin;	

	STRBUF*							strBuf;
	GFL_BMP_DATA*				bmp;	

	GFL_G3D_CAMERA*			g3Dcamera;	
	GFL_G3D_LIGHTSET*		g3Dlightset;	

	EL_SCOREBOARD*			elb;

	GFL_CAMADJUST*			gflCamAdjust;
	fx32								cameraLength;
	u16									cameraAngleV;
	u16									cameraAngleH;

	int									timer;
}SAMPLE2_WORK;

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
static BOOL	sample2(SAMPLE2_WORK* sw);

//------------------------------------------------------------------
/**
 * @brief	���[�N�̏��������j��
 */
//------------------------------------------------------------------
static void	workInitialize(SAMPLE2_WORK* sw)
{
	sw->seq = 0;
	sw->timer = 0;

	sw->strBuf = GFL_STR_CreateBuffer(STRBUF_SIZE, sw->heapID);
}

static void	workFinalize(SAMPLE2_WORK* sw)
{
	GFL_STR_DeleteBuffer(sw->strBuf);
}

//------------------------------------------------------------------
/**
 * @brief	proc�֐�
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT Sample2Proc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	SAMPLE2_WORK* sw = NULL;

	sw = GFL_PROC_AllocWork(proc, sizeof(SAMPLE2_WORK), GFL_HEAPID_APP);

    GFL_STD_MemClear(sw, sizeof(SAMPLE2_WORK));
    sw->heapID = GFL_HEAPID_APP;

	workInitialize(sw);

    return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
static GFL_PROC_RESULT Sample2Proc_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	SAMPLE2_WORK*	sw;
	sw = mywk;

	sw->timer++;
	if(sample2(sw) == TRUE){
		return GFL_PROC_RES_CONTINUE;
	}
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
static GFL_PROC_RESULT Sample2Proc_End(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	SAMPLE2_WORK*	sw;
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
const GFL_PROC_DATA DebugWatanabeSample2ProcData = {
	Sample2Proc_Init,
	Sample2Proc_Main,
	Sample2Proc_End,
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
static void systemSetup(SAMPLE2_WORK* sw);
static void systemFramework(SAMPLE2_WORK* sw);
static void systemDelete(SAMPLE2_WORK* sw);

static void makeStr(const u16* str, STRBUF* strBuf);
static void printStr
			(const STRBUF* str, GFL_BMP_DATA* bmp, PRINT_QUE* printQue, GFL_FONT* fontHandle);
	
//------------------------------------------------------------------
/**
 *
 * @brief	�t���[�����[�N
 *
 */
//------------------------------------------------------------------
static BOOL	sample2(SAMPLE2_WORK* sw)
{
	switch(sw->seq){
	case 0:
		systemSetup(sw);

		makeStr(testMsg, sw->strBuf);
		printStr(sw->strBuf, sw->bmp, sw->printQue, sw->fontHandle);

		sw->gflCamAdjust = GFL_CAMADJUST_Create(&camAdjustData, sw->heapID);
		GFL_CAMADJUST_SetCameraParam
			(sw->gflCamAdjust, &sw->cameraAngleV, &sw->cameraAngleH, &sw->cameraLength); 

		sw->seq++;
		break;

	case 1:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
			sw->seq++;
		}
		GFL_CAMADJUST_Main(sw->gflCamAdjust);
		{
			u16 col;
			if( sw->timer & 0x0004 ){ col = COLDATA_LETTER1; } else { col = COLDATA_LETTER2; }
			GFL_BG_LoadPalette(TEXT_FRAME, (void*)&col, COL_SIZ, TEXT_PLTTID*PLTT_SIZ + LCOL*COL_SIZ);
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
static void systemSetup(SAMPLE2_WORK* sw)
{
	//��{�Z�b�g�A�b�v
	bg_init(sw->heapID);
	//�t�H���g�p�p���b�g�]��
	GFL_BG_SetBackGroundColor( TEXT_FRAME, 0x7fff );
	GFL_BG_LoadPalette( TEXT_FRAME, (void*)plttData, PLTT_SIZ, TEXT_PLTTID * PLTT_SIZ );
	//�t�H���g�n���h���쐬
	sw->fontHandle = GFL_FONT_Create(	ARCID_FONT,
										NARC_font_large_nftr,
										GFL_FONT_LOADTYPE_FILE,
										FALSE,
										sw->heapID);
	//�v�����g�L���[�n���h���쐬
	sw->printQue = PRINTSYS_QUE_Create(sw->heapID);

	//�`��p�r�b�g�}�b�v�쐬
	sw->bmpwin = GFL_BMPWIN_Create(	TEXT_FRAME, 4, 2, 24, 2, TEXT_PLTTID, GFL_BG_CHRAREA_GET_F );
	GFL_BMPWIN_MakeScreen(sw->bmpwin);
	GFL_BG_LoadScreenReq(TEXT_FRAME);

	//�J�����쐬
	sw->g3Dcamera = GFL_G3D_CAMERA_CreateDefault(&cameraPos, &cameraTarget, sw->heapID);
	GFL_G3D_CAMERA_Switching(sw->g3Dcamera);
	//���C�g�쐬
	sw->g3Dlightset = GFL_G3D_LIGHT_Create( &light0Setup, sw->heapID );
	GFL_G3D_LIGHT_Switching(sw->g3Dlightset);

	sw->bmp = GFL_BMPWIN_GetBmp(sw->bmpwin);

	sw->elb = ELBOARD_Add( ELB_TEXSIZ_256x16, 64, sw->heapID );
}

//------------------------------------------------------------------
/**
 * @brief		�t���[�����[�N
 */
//------------------------------------------------------------------
#define PITCH_LIMIT (0x200)
static void systemFramework(SAMPLE2_WORK* sw)
{
	PRINTSYS_QUE_Main(sw->printQue);
	GFL_BMPWIN_TransVramCharacter(sw->bmpwin);

	ELBOARD_Main(sw->elb);

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
		}

		GFL_G3D_DRAW_Start();			//�`��J�n
		GFL_G3D_DRAW_SetLookAt();	//�J�����O���[�o���X�e�[�g�ݒ�		
		{
			fx32		scale = FX32_ONE;
			fx16		sizX = 7 * FX16_ONE;
			fx16		sizY = 1 * FX16_ONE;

			ELBOARD_Draw(sw->elb, &target, scale, sizX, sizY, sw->g3Dcamera, sw->g3Dlightset);
		}
		GFL_G3D_DRAW_End();				//�`��I���i�o�b�t�@�X���b�v�j					
	}
}

//------------------------------------------------------------------
/**
 * @brief		�j��
 */
//------------------------------------------------------------------
static void systemDelete(SAMPLE2_WORK* sw)
{
	ELBOARD_Delete(sw->elb);

	GFL_G3D_LIGHT_Delete(sw->g3Dlightset);
	GFL_G3D_CAMERA_Delete(sw->g3Dcamera);

	PRINTSYS_QUE_Clear(sw->printQue);
	GFL_BMPWIN_Delete(sw->bmpwin);

	PRINTSYS_QUE_Delete(sw->printQue);
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





//============================================================================================
/**
 * @file	ele_scoreboard.c
 * @brief	�d���f���V�X�e��
 */
//============================================================================================
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
 *
 * @brief	�^�錾
 *
 */
//------------------------------------------------------------------
struct _EL_SCOREBOARD {
	HEAPID				heapID;

	GXTexSizeS		s;						//SDK�p�f�[�^�T�C�YX��`
	GXTexSizeT		t;						//SDK�p�f�[�^�T�C�YY��`
	u16						texSizX;			//�e�N�X�`���f�[�^�T�C�YX
	u16						texSizY;			//�e�N�X�`���f�[�^�T�C�YY
	NNSGfdTexKey	texVramKey;		//�e�N�X�`���u�q�`�l�L�[
	NNSGfdPlttKey	plttVramKey;	//�p���b�g�u�q�`�l�L�[

	PRINT_QUE*		printQue;
	GFL_FONT*			fontHandle;

	STRBUF*				strBuf1;
	STRBUF*				strBuf2;

	GFL_BMP_DATA*	bmp;	
	GFL_BMP_DATA*	bmpTmp;	

	int						timer;
};

//------------------------------------------------------------------
//SDK�p�f�[�^�T�C�Y��`�@�ϊ��e�[�u���iGFL_BBD_TEXSIZ �ɑΉ��j
typedef struct {
	GXTexSizeS	s;
	GXTexSizeT	t;
	u16					sizX;
	u16					sizY;
}GX_SIZ_TBL;

static const GX_SIZ_TBL GX_sizTbl[] = {
	{ GX_TEXSIZE_S16,		GX_TEXSIZE_T16,		16,		16		}, 
	{ GX_TEXSIZE_S32,		GX_TEXSIZE_T16,		32,		16		}, 
	{ GX_TEXSIZE_S64,		GX_TEXSIZE_T16,		64,		16		}, 
	{ GX_TEXSIZE_S128,	GX_TEXSIZE_T16,		128,	16		}, 
	{ GX_TEXSIZE_S256,	GX_TEXSIZE_T16,		256,	16		}, 
	{ GX_TEXSIZE_S512,	GX_TEXSIZE_T16,		512,	16		}, 
	{ GX_TEXSIZE_S1024,	GX_TEXSIZE_T16,		1024,	16		}, 
};

//------------------------------------------------------------------
/**
 * @brief	�E�C���h�E�w�i�N���A
 */
//------------------------------------------------------------------
static void clearBitmap(GFL_BMP_DATA* bmp)
{
	u8*	dataAdrs = GFL_BMP_GetCharacterAdrs(bmp);
	u32	writeSize = GFL_BMP_GetBmpDataSize(bmp);
	int	i;

	for( i= 0; i<writeSize; i++ ){ dataAdrs[i] = ((BCOL2 << 4) | BCOL1); }

}

//------------------------------------------------------------------
/**
 * @brief	������`��
 */
//------------------------------------------------------------------
static void printStr
			(const STRBUF* str, GFL_BMP_DATA* bmp, PRINT_QUE* printQue, GFL_FONT* fontHandle)
{
	PRINTSYS_LSB	lsb = PRINTSYS_LSB_Make(LCOL,0,0);
	clearBitmap(bmp);

	PRINTSYS_PrintQueColor(printQue, bmp, 0, 2, str, fontHandle, lsb);
}

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
 * @brief	�r�b�g�}�b�v�ϊ�
 */
//------------------------------------------------------------------
static void convBitmap(GFL_BMP_DATA* src, GFL_BMP_DATA* dst)
{
	u16	size_x = GFL_BMP_GetSizeX(src)/8;						//�摜�f�[�^��Xdot�T�C�Y
	u16	size_y = GFL_BMP_GetSizeY(src)/8;						//�摜�f�[�^��Ydot�T�C�Y
	u16	col = (u16)GFL_BMP_GetColorFormat(src);			//�J���[���[�h���f�[�^�T�C�Y
	u16	size_cx = col/8;														//�L�����N�^�f�[�^X�T�C�Y
	u8*	srcAdrs = GFL_BMP_GetCharacterAdrs(src);
	u8*	dstAdrs = GFL_BMP_GetCharacterAdrs(dst);
	int	i, cx, cy, x, y;

	for( i=0; i<size_x * size_y * col; i++ ){
		y = i/(size_x * size_cx);
		x = i%(size_x * size_cx);
		dstAdrs[i] = srcAdrs[ ((y/8)*size_x + (x/size_cx))*col + ((y%8)*size_cx + (x%size_cx)) ];
	}
}


//------------------------------------------------------------------
/**
 * @brief	�I�u�W�F�N�g�ǉ�
 */
//------------------------------------------------------------------
EL_SCOREBOARD* ELBOARD_Add( ELB_TEXSIZ sizMode, u8 strLen, HEAPID heapID )
{
	EL_SCOREBOARD* elb = GFL_HEAP_AllocClearMemory(heapID, sizeof(EL_SCOREBOARD));

	elb->heapID = heapID;

	//�`��p�ݒ�
	if( sizMode >= NELEMS(GX_sizTbl) ){ sizMode = NELEMS(GX_sizTbl)-1; }
	elb->s = GX_sizTbl[sizMode].s;
	elb->t = GX_sizTbl[sizMode].t;
	elb->texSizX = GX_sizTbl[sizMode].sizX;
	elb->texSizY = GX_sizTbl[sizMode].sizY;

	//�u�q�`�l�m��
	elb->texVramKey = NNS_GfdAllocTexVram(elb->texSizX * elb->texSizY, FALSE, 0);
	elb->plttVramKey = NNS_GfdAllocPlttVram(16, FALSE, 0);

	//�t�H���g�n���h���쐬
	elb->fontHandle = GFL_FONT_Create
						(ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID);
	//�v�����g�L���[�n���h���쐬
	elb->printQue = PRINTSYS_QUE_Create(heapID);

	//������o�b�t�@�쐬
	elb->strBuf1 = GFL_STR_CreateBuffer(strLen, heapID);
	elb->strBuf2 = GFL_STR_CreateBuffer(strLen, heapID);

	//�r�b�g�}�b�v�쐬
	elb->bmp = GFL_BMP_Create(elb->texSizX/8, elb->texSizY/8, GFL_BMP_16_COLOR, heapID);
	elb->bmpTmp = GFL_BMP_Create(elb->texSizX/8, elb->texSizY/8, GFL_BMP_16_COLOR, heapID);

	elb->timer = 0;

	return elb;
}	

//------------------------------------------------------------------
/**
 * @brief	�I�u�W�F�N�g�j��
 */
//------------------------------------------------------------------
void	ELBOARD_Delete( EL_SCOREBOARD* elb )
{
	GFL_BMP_Delete(elb->bmpTmp);
	GFL_BMP_Delete(elb->bmp);

	GFL_STR_DeleteBuffer(elb->strBuf2);
	GFL_STR_DeleteBuffer(elb->strBuf1);

	PRINTSYS_QUE_Clear(elb->printQue);
	PRINTSYS_QUE_Delete(elb->printQue);
	GFL_FONT_Delete(elb->fontHandle);

	NNS_GfdFreePlttVram(elb->plttVramKey);
	NNS_GfdFreeTexVram(elb->texVramKey);
}	

//------------------------------------------------------------------
/**
 *
 * @brief	����
 *
 */
//------------------------------------------------------------------
void	ELBOARD_Main( EL_SCOREBOARD* elb )
{
	elb->timer++;

	makeStr(testMsg, elb->strBuf1);
	printStr(elb->strBuf1, elb->bmp, elb->printQue, elb->fontHandle);
	convBitmap(elb->bmp, elb->bmpTmp);

	{
		void* src;
		u32 dst, siz;

		if(elb->timer & 0x0004){ src = (void*)plttData; } else { src = (void*)plttData2; }
		dst = NNS_GfdGetPlttKeyAddr(elb->plttVramKey);
		siz = PLTT_SIZ;
		NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_PLTT, dst, src, siz);

		src = GFL_BMP_GetCharacterAdrs(elb->bmpTmp);
		dst = NNS_GfdGetTexKeyAddr(elb->texVramKey);
		siz = NNS_GfdGetTexKeySize(elb->texVramKey);
		NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, dst, src, siz);
	}
}

//------------------------------------------------------------------
/**
 *
 * @brief	�`��
 *
 */
//------------------------------------------------------------------
#define ELB_DIF			(GX_RGB(31, 31, 31))
#define ELB_AMB			(GX_RGB(16, 16, 16))
#define ELB_SPE			(GX_RGB(16, 16, 16))
#define ELB_EMI			(GX_RGB(0, 0, 0))
#define ELB_POLID 	(63)

void	ELBOARD_Draw( EL_SCOREBOARD* elb, 
										VecFx32* trans, fx32 scale, fx16 sizX, fx16 sizY, 
										GFL_G3D_CAMERA* g3Dcamera, GFL_G3D_LIGHTSET* g3Dlightset )
{
	MtxFx33			mtx;
	VecFx16			vecN;
	VecFx32			transTmp;
	fx32				s0, t0, s1, t1;

	G3X_Reset();

	//�J�����ݒ�擾
	{
		VecFx32		camPos, camUp, target, vecNtmp;
		MtxFx43		mtxCamera;

		GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
		GFL_G3D_CAMERA_GetCamUp( g3Dcamera, &camUp );
		GFL_G3D_CAMERA_GetTarget( g3Dcamera, &target );

		G3_LookAt( &camPos, &camUp, &target, &mtxCamera );	//mtxCamera�ɂ͍s��v�Z���ʂ��Ԃ�
		MTX_Copy43To33( &mtxCamera, &mtx );		//�J�����t�s�񂩂��]�s������o��

		VEC_Subtract( &camPos, &target, &vecNtmp );
		VEC_Normalize( &vecNtmp, &vecNtmp );
		VEC_Fx16Set( &vecN, vecNtmp.x, vecNtmp.y, vecNtmp.z );
	}
	if( g3Dlightset ){ GFL_G3D_LIGHT_Switching( g3Dlightset ); }

	//�O���[�o���X�P�[���ݒ�
#if 0
	G3_Scale( scale, scale, scale );
#else
	G3_Scale( scale * FX16_ONE, scale * FX16_ONE, scale * FX16_ONE );
#endif
	s0 = 0;
	s1 = s0 + elb->texSizX * FX32_ONE;
	t0 = 0;
	t1 = t0 + elb->texSizY * FX32_ONE;

	G3_PushMtx();

	VEC_Set(&transTmp, FX_Div(trans->x,scale), FX_Div(trans->y,scale), FX_Div(trans->z,scale));
	//��]�A���s�ړ��p�����[�^�ݒ�
	G3_MultTransMtx33(&mtx, &transTmp);

	G3_TexImageParam(	GX_TEXFMT_PLTT16, GX_TEXGEN_TEXCOORD, elb->s, elb->t,
										GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
										GX_TEXPLTTCOLOR0_TRNS, NNS_GfdGetTexKeyAddr(elb->texVramKey) );
	G3_TexPlttBase( NNS_GfdGetPlttKeyAddr(elb->plttVramKey), GX_TEXFMT_PLTT16 );

	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb( ELB_DIF, ELB_AMB, TRUE );
	G3_MaterialColorSpecEmi( ELB_SPE, ELB_EMI, FALSE );
	G3_PolygonAttr(	GFL_BBD_LIGHTMASK_0123, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
									ELB_POLID, 31, GX_POLYGON_ATTR_MISC_FOG );
	
	G3_Begin( GX_BEGIN_QUADS );

	if( g3Dlightset )	{ G3_Normal(vecN.x, vecN.y, vecN.z); } 	//���ʃ|���S���Ȃ̂Ŗ@���x�N�g���͋��p
	else							{ G3_Color(GX_RGB(31, 31, 31)); }
#if 0
	G3_TexCoord(s0, t0);
	G3_Vtx(-sizX, sizY, 0);

	G3_TexCoord(s0, t1);
	G3_Vtx(-sizX, -sizY, 0);

	G3_TexCoord(s1, t1);
	G3_Vtx(sizX, -sizY, 0);

	G3_TexCoord(s1, t0);
	G3_Vtx(sizX, sizY, 0);
#else
	{
		fx16	setSizX = elb->texSizX / 2;
		fx16	setSizY = elb->texSizY / 2;

		G3_TexCoord(s0, t0);
		G3_Vtx(-setSizX, setSizY, 0);

		G3_TexCoord(s0, t1);
		G3_Vtx(-setSizX, -setSizY, 0);

		G3_TexCoord(s1, t1);
		G3_Vtx(setSizX, -setSizY, 0);

		G3_TexCoord(s1, t0);
		G3_Vtx(setSizX, setSizY, 0);
	}
#endif
	G3_End();
	G3_PopMtx(1);

	//�����G3D_System�ōs���̂ŁA�����ł͂��Ȃ�
	//G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}


