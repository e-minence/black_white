//==============================================================================
/**
 * @file	title.c
 * @brief	�^�C�g�����
 * @author	matsuda
 * @date	2008.12.03(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "arc_def.h"
#include "message.naix"
#include "msg\msg_title.h"
#include "test/performance.h"
#include "font/font.naix"
#include "title.naix"
#include "title/title.h"



//==============================================================================
//	�萔��`
//==============================================================================
///�A�N�^�[�ő吔
#define ACT_MAX			(64)

///�A�C�e���A�C�R���̃p���b�g�W�J�ʒu
#define D_MATSU_ICON_PALNO		(0)

///BMP�E�B���h�E
enum{
	WIN_PUSH_JPN,			//�u�X�^�[�g�{�^���������Ă��������v
	WIN_PUSH_ENG,			//�uPUSH START�v
	
	WIN_MAX,
};

///�^�C�g�����SBG��BG�v���C�I���e�B
enum{
	BGPRI_MSG = 0,
	BGPRI_TITLE_LOGO = 2,
};

///�t���[���ԍ�
enum{
	//���C�����
	FRAME_LOGO_M = GFL_BG_FRAME3_M,		///<�^�C�g�����S��BG�t���[��
	
	//�T�u���
	FRAME_MSG_S = GFL_BG_FRAME1_S,		///<���b�Z�[�W�t���[��
	FRAME_LOGO_S = GFL_BG_FRAME3_S,		///<�T�u��ʃ^�C�g�����S��BG�t���[��
};


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
	GFL_BMPWIN		*win;
	GFL_BMP_DATA	*bmp;
	PRINT_UTIL		printUtil[1];
	BOOL			message_req;		///<TRUE:���b�Z�[�W���N�G�X�g��������
}DM_MSG_DRAW_WIN;

typedef struct {

	u16		seq;
	HEAPID	heapID;
	int debug_mode;
	int timer;
	
	GFL_FONT		*fontHandle;
	PRINT_QUE		*printQue;
	PRINT_STREAM	*printStream;
	GFL_MSGDATA		*mm;
	STRBUF			*strbuf_push_jpn;
	STRBUF			*strbuf_push_eng;
	STRBUF			*strbuf_info;
	STRBUF			*strbuf_info_kanji;
	GFL_TCBLSYS		*tcbl;
	DM_MSG_DRAW_WIN drawwin[WIN_MAX];
	
	//�A�N�^�[
	GFL_CLUNIT *clunit;
	GFL_CLWK	*clwk_icon;		//�A�C�e���A�C�R���A�N�^�[
	NNSG2dImagePaletteProxy	icon_pal_proxy;
	NNSG2dCellDataBank	*icon_cell_data;
	NNSG2dAnimBankData	*icon_anm_data;
	void	*icon_cell_res;
	void	*icon_anm_res;
	
	int item_no;
}TITLE_WORK;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void Local_VramSetting(TITLE_WORK *tw);
static void Local_BGFrameSetting(TITLE_WORK *tw);
static void Local_MessageSetting(TITLE_WORK *tw);
static void Local_ActorSetting(TITLE_WORK *tw);
static void Local_MessagePrintMain(TITLE_WORK *tw);
static void Local_BGGraphicLoad(TITLE_WORK *tw);

//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void	TestModeSet(void);


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   proc		
 * @param   seq		
 * @param   pwk		
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
GFL_PROC_RESULT TitleProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TITLE_WORK *tw;
	
	DEBUG_PerformanceSetActive(FALSE);	//�f�o�b�O�F�p�t�H�[�}���X���[�^�[����
	
	// �e����ʃ��W�X�^������
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);
	G2_BlendNone();
	G2S_BlendNone();
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TITLE_DEMO, 0x70000 );
	tw = GFL_PROC_AllocWork( proc, sizeof(TITLE_WORK), HEAPID_TITLE_DEMO );
	GFL_STD_MemClear(tw, sizeof(TITLE_WORK));

	// �㉺��ʐݒ�
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();

	//TCB�쐬
	tw->tcbl = GFL_TCBL_Init( HEAPID_TITLE_DEMO, HEAPID_TITLE_DEMO, 4, 32 );

	//VRAM�ݒ� & BG�t���[���ݒ�
	Local_VramSetting(tw);
	Local_BGFrameSetting(tw);
	Local_BGGraphicLoad(tw);
	
	//���b�Z�[�W�֘A�쐬
	GFL_BMPWIN_Init( HEAPID_TITLE_DEMO );
	GFL_FONTSYS_Init();
	Local_MessageSetting(tw);

	//�A�N�^�[�ݒ�
	Local_ActorSetting(tw);
	
	GFL_BG_SetVisible(FRAME_LOGO_M, VISIBLE_ON);
	GFL_BG_SetVisible(FRAME_LOGO_S, VISIBLE_ON);
//	GFL_BG_SetVisible(FRAME_MSG_S, VISIBLE_ON);
	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
GFL_PROC_RESULT TitleProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TITLE_WORK* tw = mywk;
	BOOL ret = 0;
	
	GFL_TCBL_Main( tw->tcbl );
	Local_MessagePrintMain(tw);
	
	if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_START | PAD_BUTTON_SELECT | PAD_BUTTON_A | PAD_BUTTON_B)){
		return GFL_PROC_RES_FINISH;
	}
	
	switch(tw->seq){
	case 0:
		break;
	}
	
	if(ret == TRUE){
		return GFL_PROC_RES_FINISH;
	}

	GFL_CLACT_UNIT_Draw(tw->clunit);
	GFL_CLACT_Main();
	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
GFL_PROC_RESULT TitleProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TITLE_WORK* tw = mywk;
	int i;
	
	for(i = 0; i < WIN_MAX; i++){
		GFL_BMPWIN_Delete(tw->drawwin[i].win);
	}
	PRINTSYS_QUE_Delete(tw->printQue);
	GFL_MSG_Delete(tw->mm);

	GFL_CLACT_UNIT_Delete(tw->clunit);
	GFL_CLACT_Exit();

	GFL_FONT_Delete(tw->fontHandle);
	GFL_TCBL_Exit(tw->tcbl);
	
	GFL_BG_Exit();
	GFL_BMPWIN_Exit();

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_TITLE_DEMO);
	
	TestModeSet();	//����PROC�Ƃ��ăe�X�g��ʂ�ݒ�
	return GFL_PROC_RES_FINISH;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   VRAM�o���N�����[�h�ݒ�
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_VramSetting(TITLE_WORK *tw)
{
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_B,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_0_A,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_64K,		// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};

	static const GFL_BG_SYS_HEADER sysHeader = {
		GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
	};
	
	GFL_DISP_SetBank( &vramBank );
	GFL_BG_Init( HEAPID_TITLE_DEMO );
	GFL_BG_SetBGMode( &sysHeader );
}

//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_BGFrameSetting(TITLE_WORK *tw)
{
	static const GFL_BG_BGCNT_HEADER bgcnt_frame[] = {	//���C�����BG�t���[��
		{//FRAME_LOGO_M
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x8000,
			GX_BG_EXTPLTT_01, BGPRI_TITLE_LOGO, 0, 0, FALSE
		},
	};

	static const GFL_BG_BGCNT_HEADER sub_bgcnt_frame[] = {	//�T�u���BG�t���[��
		{//FRAME_MSG_S
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, 0x04000,
			GX_BG_EXTPLTT_01, BGPRI_MSG, 0, 0, FALSE
		},
		{//FRAME_LOGO_S
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x8000,
			GX_BG_EXTPLTT_01, BGPRI_TITLE_LOGO, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( FRAME_LOGO_M,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_MSG_S,   &sub_bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_LOGO_S,   &sub_bgcnt_frame[1],   GFL_BG_MODE_TEXT );

	GFL_BG_FillCharacter( FRAME_LOGO_M, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_MSG_S, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_LOGO_S, 0x00, 1, 0 );

	GFL_BG_FillScreen( FRAME_LOGO_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_MSG_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_LOGO_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

	GFL_BG_LoadScreenReq( FRAME_LOGO_M );
	GFL_BG_LoadScreenReq( FRAME_MSG_S );
	GFL_BG_LoadScreenReq( FRAME_LOGO_S );
}

//--------------------------------------------------------------
/**
 * @brief   BG�O���t�B�b�N��VRAM�֓]��
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_BGGraphicLoad(TITLE_WORK *tw)
{
	//���C�����
	GFL_ARC_UTIL_TransVramBgCharacter(
		ARCID_TITLE, NARC_title_wb_logo_bk_NCGR, FRAME_LOGO_M, 0, 0x8000, 0, HEAPID_TITLE_DEMO);
	GFL_ARC_UTIL_TransVramScreen(
		ARCID_TITLE, NARC_title_wb_logo_bk_NSCR, FRAME_LOGO_M, 0, 0, 0, HEAPID_TITLE_DEMO);
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_TITLE, NARC_title_wb_logo_bk_NCLR, PALTYPE_MAIN_BG, 0, 0, HEAPID_TITLE_DEMO);

	//�T�u���
	GFL_ARC_UTIL_TransVramBgCharacter(
		ARCID_TITLE, NARC_title_wb_logo_bk_NCGR, FRAME_LOGO_S, 0, 0x8000, 0, HEAPID_TITLE_DEMO);
	GFL_ARC_UTIL_TransVramScreen(
		ARCID_TITLE, NARC_title_wb_logo_bk_NSCR, FRAME_LOGO_S, 0, 0, 0, HEAPID_TITLE_DEMO);
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_TITLE, NARC_title_wb_logo_bk_NCLR, PALTYPE_SUB_BG, 0, 0, HEAPID_TITLE_DEMO);
}

//--------------------------------------------------------------
/**
 * @brief   �A�N�^�[�ݒ�
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_ActorSetting(TITLE_WORK *tw)
{
	GFL_CLSYS_INIT clsys_init = GFL_CLSYSINIT_DEF_DIVSCREEN;
	
	clsys_init.oamst_main = 1;	//�ʐM�A�C�R���̕�
	clsys_init.oamnum_main = 128-1;
	clsys_init.tr_cell = ACT_MAX;
	GFL_CLACT_Init(&clsys_init, HEAPID_TITLE_DEMO);
	
	tw->clunit = GFL_CLACT_UNIT_Create(ACT_MAX, HEAPID_TITLE_DEMO);
	GFL_CLACT_UNIT_SetDefaultRend(tw->clunit);

	//OBJ�\��ON
	GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�֘A�ݒ�
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_MessageSetting(TITLE_WORK *tw)
{
	int i;

	//BMPWIN�쐬
	tw->drawwin[WIN_PUSH_JPN].win 
		= GFL_BMPWIN_Create(FRAME_MSG_S, 0, 18, 32, 2, 0, GFL_BMP_CHRAREA_GET_F);
	tw->drawwin[WIN_PUSH_ENG].win 
		= GFL_BMPWIN_Create(FRAME_MSG_S, 0, 20, 32, 2, 0, GFL_BMP_CHRAREA_GET_F);
	for(i = 0; i < WIN_MAX; i++){
		tw->drawwin[i].bmp = GFL_BMPWIN_GetBmp(tw->drawwin[i].win);
		GFL_BMP_Clear( tw->drawwin[i].bmp, 0x00 );
		GFL_BMPWIN_MakeScreen( tw->drawwin[i].win );
		GFL_BMPWIN_TransVramCharacter( tw->drawwin[i].win );
		PRINT_UTIL_Setup( tw->drawwin[i].printUtil, tw->drawwin[i].win );
	}
	GFL_BG_LoadScreenReq( FRAME_MSG_S );

	//�t�H���g�쐬
	tw->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_TITLE_DEMO );

	tw->printQue = PRINTSYS_QUE_Create( HEAPID_TITLE_DEMO );

	tw->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_title_dat, HEAPID_TITLE_DEMO );

	GFL_MSGSYS_SetLangID( 1 );	//JPN_KANJI
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�`�惁�C��
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_MessagePrintMain(TITLE_WORK *tw)
{
	int i;
	
	PRINTSYS_QUE_Main( tw->printQue );

	for(i = 0; i < WIN_MAX; i++){
		if( PRINT_UTIL_Trans( tw->drawwin[i].printUtil, tw->printQue ) == TRUE){
			//return GFL_PROC_RES_CONTINUE;
		}
		else{
			if(tw->drawwin[i].message_req == TRUE){
	//			GFL_BMP_Clear( tw->bmp, 0xff );
				GFL_BMPWIN_TransVramCharacter( tw->drawwin[i].win );
				tw->drawwin[i].message_req = FALSE;
			}
		}
	}
}

