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

#define TEXT_FRAME		(GFL_BG_FRAME3_S)
#define TEXT_DISPSIDE	(PALTYPE_SUB_BG)
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

//============================================================================================
/**
 *
 * @brief	�\���̒�`
 *
 */
//============================================================================================
typedef struct {
	HEAPID				heapID;
	int						seq;

	PRINT_QUE*		printQue;
	GFL_FONT*			fontHandle;

	GFL_BMPWIN*		bmpwin;	

	STRBUF*				strBuf;
	GFL_BMP_DATA*	bmp;	

	int						timer;
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

		sw->seq++;
		break;

	case 1:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
			sw->seq++;
		}
		{
			u16 col;
			if( sw->timer & 0x0004 ){ col = COLDATA_LETTER1; } else { col = COLDATA_LETTER2; }
			GFL_BG_LoadPalette(TEXT_FRAME, (void*)&col, COL_SIZ, TEXT_PLTTID*PLTT_SIZ + LCOL*COL_SIZ);
		}
		
		systemFramework(sw);
		break;

	case 2:
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
	GX_VRAM_BG_128_A,				//���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE,			//���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_32_H,			//�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE,		//�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E,				//���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE,		//���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_NONE,			//�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	//�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_NONE,				//�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_NONE,			//�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
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

	GFL_BG_FillCharacter(TEXT_FRAME, 0, 1, 0);	// �擪�ɃN���A�L�����z�u
	GFL_BG_ClearScreen(TEXT_FRAME);

	//�r�b�g�}�b�v�E�C���h�E�N��
	GFL_BMPWIN_Init(heapID);

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
	GFL_DISP_SetDispOn();
}

static void	bg_exit(void)
{
	GFL_BMPWIN_Exit();
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
	GFL_BG_LoadPalette( TEXT_FRAME, (void*)plttData, PLTT_SIZ, TEXT_PLTTID * PLTT_SIZ );
	//�t�H���g�n���h���쐬
	sw->fontHandle = GFL_FONT_Create(	ARCID_FONT,
										NARC_font_large_nftr,
										GFL_FONT_LOADTYPE_FILE,
										FALSE,
										sw->heapID);
	//�v�����g�L���[�n���h���쐬
	sw->printQue = PRINTSYS_QUE_Create(sw->heapID);

	//�`��p�r�b�g�}�b�v�쐬�i��ʑS�́j
	sw->bmpwin = GFL_BMPWIN_Create(	TEXT_FRAME,
									0, 0, 32, 24,
									TEXT_PLTTID, GFL_BG_CHRAREA_GET_F );
	GFL_BMPWIN_MakeScreen(sw->bmpwin);
	GFL_BG_LoadScreenReq(TEXT_FRAME);

	sw->bmp = GFL_BMPWIN_GetBmp(sw->bmpwin);
}

//------------------------------------------------------------------
/**
 * @brief		�t���[�����[�N
 */
//------------------------------------------------------------------
static void systemFramework(SAMPLE2_WORK* sw)
{
	PRINTSYS_QUE_Main(sw->printQue);
	GFL_BMPWIN_TransVramCharacter(sw->bmpwin);
}

//------------------------------------------------------------------
/**
 * @brief		�j��
 */
//------------------------------------------------------------------
static void systemDelete(SAMPLE2_WORK* sw)
{
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
 * @brief	�E�C���h�E�w�i�N���A
 */
//------------------------------------------------------------------
static void clearBitmap(GFL_BMP_DATA* bmp)
{
	u8*	dataAdrs = GFL_BMP_GetCharacterAdrs(bmp);
	u32	writeSize = (GFL_BMP_GetBmpDataSize(bmp) / 2 );
	int	i;

	for( i= 0; i<writeSize; i++ ){ dataAdrs[i] = ((BCOL2 << 4) | BCOL1); }

}

//------------------------------------------------------------------
/**
 * @brief	���O�`��
 */
//------------------------------------------------------------------
static void printStr
			(const STRBUF* str, GFL_BMP_DATA* bmp, PRINT_QUE* printQue, GFL_FONT* fontHandle)
{
	PRINTSYS_LSB	lsb = PRINTSYS_LSB_Make(LCOL,0,0);
	clearBitmap(bmp);

	PRINTSYS_PrintQueColor(printQue, bmp, 0, 0, str, fontHandle, lsb);
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



