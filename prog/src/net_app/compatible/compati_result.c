//==============================================================================
/**
 * @file	compati_control.c
 * @brief	�����`�F�b�N�Q�[���̓�����ƌ��ʉ��
 * @author	matsuda
 * @date	2009.02.10(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include "net\network_define.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "arc_def.h"
#include "message.naix"
#include "test_graphic\d_taya.naix"
#include "font/font.naix"
#include "system\gfl_use.h"
#include "compati_check.naix"
#include "system/wipe.h"

#include "compati_check.h"
#include "compati_types.h"
#include "compati_tool.h"


//==============================================================================
//	�萔��`
//==============================================================================
///�t���[���ԍ�
enum{
	//���C�����
	FRAME_BACK_M = GFL_BG_FRAME3_M,		///<�w�i
	FRAME_COUNT_M = GFL_BG_FRAME2_M,		///<�J�E���g
	
	//�T�u���
	FRAME_BACK_S = GFL_BG_FRAME3_S,		///<�w�i
	FRAME_MSG_S = GFL_BG_FRAME1_S,		///<���b�Z�[�W�t���[��
};

///BG�v���C�I���e�B
enum{
	//���C�����
	BGPRI_BACK_M = 3,
	BGPRI_COUNT_M = 0,
	
	//�T�u���
	BGPRI_BACK_S = 3,
	BGPRI_MSG_S = 1,
};

///�t�H���g�̃p���b�g�ԍ�
#define CC_FONT_PALNO		(15)

///BMP�E�B���h�Eindex
enum{
	CRBMPWIN_TITLE,		///<�Q�[����
	CRBMPWIN_POINT,		///<���_
	
	CRBMPWIN_MAX,
};

///�ԊO�����M�R�}���h
enum{
	//�q���瑗�M
	CMD_CHILD_PROFILE = 0,		///<�q�̃v���t�B�[��
	
	//�e���瑗�M
	CMD_PARENT_PROFILE = 2,		///<�e�̃v���t�B�[��
	CMD_PARENT_SUCCESS = 4,		///<�S�Ă̎�M����
};

enum{
	MY_CHILD,		///<�q�ł���
	MY_PARENT,		///<�e�ł���
};

//--------------------------------------------------------------
//	�A�N�^�[
//--------------------------------------------------------------
///�A�N�^�[�ő吔
#define CR_ACT_MAX			(64)

//--------------------------------------------------------------
//	���\�[�XID
//--------------------------------------------------------------
enum{
	//�L����ID
	CHARID_CIRCLE,
	CHARID_END,		//�L����ID�I�[
	
	//�Z��ID
	CELLID_CIRCLE,
	CELLID_END,		//�Z��ID�I�[

	//�p���b�gID
	PLTTID_CIRCLE,
	PLTTID_CIRCLE_END = PLTTID_CIRCLE + CC_CIRCLE_MAX - 1,
	PLTTID_END,		//�p���b�gID�I�[
};


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
	GFL_BMPWIN		*win;
	GFL_BMP_DATA	*bmp;
	PRINT_UTIL		printUtil[1];
	BOOL			message_req;		///<TRUE:���b�Z�[�W���N�G�X�g��������
}MSG_DRAW_WIN;

///�����`�F�b�N�V�X�e�����[�N�\����
typedef struct {
	s16 local_seq;
	s16 local_timer;
	
	GFL_FONT		*fontHandle;
	PRINT_QUE		*printQue;
	GFL_MSGDATA		*mm;
	STRBUF			*strbuf_win[CRBMPWIN_MAX];
	GFL_TCBLSYS		*tcbl;
	MSG_DRAW_WIN drawwin[CRBMPWIN_MAX];	//+1 = ���C�����b�Z�[�W�E�B���h�E
	
	//�A�N�^�[
	GFL_CLUNIT *clunit;
	GFL_CLWK *circle_cap[CR_ACT_MAX];

	GFL_TCB *vintr_tcb;
	
	u16 light_pal[16];			///<�~�����������̖��邢�F�̃p���b�g�f�[�^
	
	u32 cgr_id[CHARID_END];
	u32 cell_id[CELLID_END];
	u32 pltt_id[PLTTID_END];
	
	CC_CIRCLE_PACKAGE circle_package;	///<�g�p����T�[�N���f�[�^

	union{
		CCNET_FIRST_PARAM first_param[2];	///< [0]:�����̃f�[�^ [1]:��M��������̃f�[�^
		CCNET_RESULT_PARAM result_param[2];	///< [0]:�����̃f�[�^ [1]:��M��������̃f�[�^
	};
}COMPATI_RESULT_SYS;



//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT CompatiResult_ProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT CompatiResult_ProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT CompatiResult_ProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void CCLocal_VblankFunc(GFL_TCB *tcb, void *work);
static void CCLocal_VramSetting(COMPATI_RESULT_SYS *crs);
static void CCLocal_BGFrameSetting(COMPATI_RESULT_SYS *crs);
static void CCLocal_BGGraphicLoad(ARCHANDLE *hdl);
static void CCLocal_MessageSetting(COMPATI_RESULT_SYS *crs);
static void CCLocal_MessageExit(COMPATI_RESULT_SYS *crs);
static void CCLocal_ActorSetting(COMPATI_RESULT_SYS *crs);
static int CRSeq_Init(COMPATI_RESULT_SYS *crs);
static int CRSeq_Countdown(COMPATI_RESULT_SYS *crs);
static int CRSeq_Main(COMPATI_RESULT_SYS *crs);
static int CRSeq_End(COMPATI_RESULT_SYS *crs);



//==============================================================================
//	�f�[�^
//==============================================================================
///�Ăяo���p��PROC�f�[�^
const GFL_PROC_DATA CompatiResultProcData = {
	CompatiResult_ProcInit,
	CompatiResult_ProcMain,
	CompatiResult_ProcEnd,
};

//--------------------------------------------------------------
//	BG
//--------------------------------------------------------------
///VRAM�o���N�ݒ�
static const GFL_DISP_VRAM CompatiVramBank = {
	GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_0123_E,			// �e�N�X�`���p���b�g�X���b�g
	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};

///BG���[�h�ݒ�
static const GFL_BG_SYS_HEADER CompatiBgSysHeader = {
	GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
};

//--------------------------------------------------------------
//	���C���V�[�P���X
//--------------------------------------------------------------
///���C���V�[�P���X�̊֐��e�[�u��
static int (* const CCResultSeqTbl[])(COMPATI_RESULT_SYS *crs) = {
	CRSeq_Init,
	CRSeq_Countdown,
	CRSeq_Main,
	CRSeq_End,
};
///���C���V�[�P���X�ԍ��@��CCResultSeqTbl�ƕ��т𓯂��ɂ��Ă������ƁI
enum{
	SEQ_INIT,
	SEQ_COUNTDOWN,
	SEQ_MAIN,
	SEQ_END,
	
	SEQ_CONTINUE,		///<�V�[�P���X���̂܂�
	SEQ_NEXT,			///<���̃V�[�P���X��
	SEQ_FINISH,			///<�I��
};

//--------------------------------------------------------------
//	�A�N�^�[�w�b�_
//--------------------------------------------------------------
///�~�A�N�^�[�w�b�_(���C����ʗp)
static const GFL_CLWK_DATA CircleObjParam = {
	0, 0,		//pos_x, pos_y
	0, 			//anmseq
	10, 1,	//softpri, bgpri
};


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   PROC ������
 *
 * @param   proc		
 * @param   seq		
 * @param   pwk		
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT CompatiResult_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	COMPATI_RESULT_SYS* crs;
	ARCHANDLE *hdl;
	COMPATI_PARENTWORK *cppw = pwk;
	
	//��ʂ�^���Â�
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);

	//�e����ʃ��W�X�^������
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);
	GX_SetVisiblePlane(0);
	GXS_SetVisiblePlane(0);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2_BlendNone();
	G2S_BlendNone();
	
	//�㉺��ʐݒ�
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_SUB);

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_COMPATI, 0x70000 );
	crs = GFL_PROC_AllocWork( proc, sizeof(COMPATI_RESULT_SYS), HEAPID_COMPATI );
	GFL_STD_MemClear(crs, sizeof(COMPATI_RESULT_SYS));
	
	//�t�@�C���I�[�v��
	hdl = GFL_ARC_OpenDataHandle(ARCID_COMPATI_CHECK, HEAPID_COMPATI);
	{
		//VRAM�o���N�ݒ�
		CCLocal_VramSetting(crs);
		CCLocal_BGFrameSetting(crs);
		CCLocal_BGGraphicLoad(hdl);
		
		//�e�탉�C�u�����ݒ�
		GFL_BMPWIN_Init(HEAPID_COMPATI);
		GFL_FONTSYS_Init();
		crs->tcbl = GFL_TCBL_Init(HEAPID_COMPATI, HEAPID_COMPATI, 4, 32);

		//���b�Z�[�W�`��ׂ̈̏���
		CCLocal_MessageSetting(crs);

		//�A�N�^�[�ݒ�
		CCLocal_ActorSetting(crs);
	}
	//�t�@�C���N���[�Y
	GFL_ARC_CloseDataHandle(hdl);
	
	//V�u�����NTCB�o�^
	crs->vintr_tcb = GFUser_VIntr_CreateTCB(CCLocal_VblankFunc, crs, 5);
	
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   PROC ���C��
 *
 * @param   proc		
 * @param   seq		
 * @param   pwk		
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT CompatiResult_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	COMPATI_RESULT_SYS* crs = mywk;
	int ret, i;
	BOOL que_ret;
	
	GFL_TCBL_Main( crs->tcbl );
	que_ret = PRINTSYS_QUE_Main( crs->printQue );
	for(i = 0; i < CRBMPWIN_MAX; i++){
		if( PRINT_UTIL_Trans( crs->drawwin[i].printUtil, crs->printQue ) == FALSE){
			//return GFL_PROC_RES_CONTINUE;
		}
		else{
			if(que_ret == TRUE && crs->drawwin[i].message_req == TRUE){
	//			GFL_BMP_Clear( crs->bmp, 0xff );
				GFL_BMPWIN_TransVramCharacter( crs->drawwin[i].win );
				crs->drawwin[i].message_req = FALSE;
			}
		}
	}
	
	ret = CCResultSeqTbl[*seq](crs);
	switch(ret){
	case SEQ_CONTINUE:
		break;
	case SEQ_FINISH:
		return GFL_PROC_RES_FINISH;
	case SEQ_NEXT:
		crs->local_seq = 0;
		crs->local_timer = 0;
		(*seq)++;
		break;
	default:
		crs->local_seq = 0;
		crs->local_timer = 0;
		*seq = ret;
		break;
	}

	GFL_CLACT_SYS_Main();
	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   PROC �I��
 *
 * @param   proc		
 * @param   seq		
 * @param   pwk		
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT CompatiResult_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	COMPATI_RESULT_SYS* crs = mywk;

	GFL_TCB_DeleteTask(crs->vintr_tcb);
	
	CCLocal_MessageExit(crs);

	GFL_CLACT_UNIT_Delete(crs->clunit);
	GFL_CLACT_SYS_Delete();

	GFL_TCBL_Exit(crs->tcbl);
	
	GFL_BG_Exit();
	GFL_BMPWIN_Exit();

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_COMPATI);
	
	return GFL_PROC_RES_FINISH;
}

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   V�u�����NTCB
 *
 * @param   tcb			
 * @param   work		
 */
//--------------------------------------------------------------
static void CCLocal_VblankFunc(GFL_TCB *tcb, void *work)
{
	GFL_CLACT_SYS_VBlankFunc();
	GFL_BG_VBlankFunc();
}

//--------------------------------------------------------------
/**
 * @brief   VRAM�o���N�����[�h�ݒ�
 * @param   crs		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void CCLocal_VramSetting(COMPATI_RESULT_SYS *crs)
{
	GFL_DISP_SetBank( &CompatiVramBank );
	GFL_BG_Init( HEAPID_COMPATI );
	GFL_BG_SetBGMode( &CompatiBgSysHeader );
}

//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 * @param   crs		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void CCLocal_BGFrameSetting(COMPATI_RESULT_SYS *crs)
{
	static const GFL_BG_BGCNT_HEADER bgcnt_frame[] = {	//���C�����BG�t���[��
		{//FRAME_BACK_M
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x4000,
			GX_BG_EXTPLTT_01, BGPRI_BACK_M, 0, 0, FALSE
		},
		{//FRAME_COUNT_M
			0, 0, 0x2000, 0,
			GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, 0x8000,
			GX_BG_EXTPLTT_01, BGPRI_COUNT_M, 0, 0, FALSE
		},
	};

	static const GFL_BG_BGCNT_HEADER sub_bgcnt_frame[] = {	//�T�u���BG�t���[��
		{//FRAME_BACK_S
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x4000,
			GX_BG_EXTPLTT_01, BGPRI_BACK_S, 0, 0, FALSE
		},
		{//FRAME_MSG_S
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, 0x08000,
			GX_BG_EXTPLTT_01, BGPRI_MSG_S, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( FRAME_BACK_M,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_COUNT_M,   &bgcnt_frame[1],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_MSG_S,   &sub_bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_BACK_S,   &sub_bgcnt_frame[1],   GFL_BG_MODE_TEXT );

	GFL_BG_FillCharacter( FRAME_BACK_M, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_COUNT_M, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_MSG_S, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_BACK_S, 0x00, 1, 0 );

	GFL_BG_FillScreen( FRAME_BACK_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_COUNT_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_MSG_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_BACK_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

	GFL_BG_SetVisible(FRAME_BACK_M, VISIBLE_ON);
	GFL_BG_SetVisible(FRAME_COUNT_M, VISIBLE_OFF);
	GFL_BG_SetVisible(FRAME_BACK_S, VISIBLE_ON);
	GFL_BG_SetVisible(FRAME_MSG_S, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * @brief   BG�O���t�B�b�N��VRAM�֓]��
 *
 * @param   hdl		�A�[�J�C�u�n���h��
 */
//--------------------------------------------------------------
static void CCLocal_BGGraphicLoad(ARCHANDLE *hdl)
{
	//�p���b�g
	GFL_ARCHDL_UTIL_TransVramPalette(hdl, NARC_compati_check_cc_bg_m_NCLR, 
		PALTYPE_MAIN_BG, 0, 0, HEAPID_COMPATI);
	GFL_ARCHDL_UTIL_TransVramPalette(hdl, NARC_compati_check_cc_bg_s_NCLR, 
		PALTYPE_SUB_BG, 0, 0, HEAPID_COMPATI);

	//���C�����
	GFL_ARCHDL_UTIL_TransVramBgCharacter(hdl, NARC_compati_check_cc_main_NCGR, 
		FRAME_BACK_M, 0, 0x4000, 0, HEAPID_COMPATI);
	GFL_ARCHDL_UTIL_TransVramScreen(hdl, NARC_compati_check_cc_back_m_NSCR, 
		FRAME_BACK_M, 0, 0, 0, HEAPID_COMPATI);

	GFL_ARCHDL_UTIL_TransVramBgCharacter(hdl, NARC_compati_check_cc_main_NCGR, 
		FRAME_COUNT_M, 0, 0x4000, 0, HEAPID_COMPATI);
	GFL_ARCHDL_UTIL_TransVramScreen(hdl, NARC_compati_check_cc_count_m_NSCR, 
		FRAME_COUNT_M, 0, 0, 0, HEAPID_COMPATI);

	//�T�u���
	GFL_ARCHDL_UTIL_TransVramBgCharacter(hdl, NARC_compati_check_cc_sub_NCGR, 
		FRAME_BACK_S, 0, 0x4000, 0, HEAPID_COMPATI);
	GFL_ARCHDL_UTIL_TransVramScreen(hdl, NARC_compati_check_cc_back_s_NSCR, 
		FRAME_BACK_S, 0, 0, 0, HEAPID_COMPATI);

	//�t�H���g�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, 
		PALTYPE_MAIN_BG, CC_FONT_PALNO * 0x20, 0x20, HEAPID_COMPATI);
	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, 
		PALTYPE_SUB_BG, CC_FONT_PALNO * 0x20, 0x20, HEAPID_COMPATI);

	GFL_BG_LoadScreenReq( FRAME_BACK_M );
	GFL_BG_LoadScreenReq( FRAME_COUNT_M );
	GFL_BG_LoadScreenReq( FRAME_MSG_S );
	GFL_BG_LoadScreenReq( FRAME_BACK_S );
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�`��֘A�̐ݒ�
 *
 * @param   crs		
 */
//--------------------------------------------------------------
static void CCLocal_MessageSetting(COMPATI_RESULT_SYS *crs)
{
	int i;

	crs->drawwin[CRBMPWIN_TITLE].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 2, 2, 30, 2, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	crs->drawwin[CRBMPWIN_POINT].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 1, 18, 30, 4, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	for(i = 0; i < CRBMPWIN_MAX; i++){
		GF_ASSERT(crs->drawwin[i].win != NULL);
		crs->drawwin[i].bmp = GFL_BMPWIN_GetBmp(crs->drawwin[i].win);
		GFL_BMP_Clear( crs->drawwin[i].bmp, 0xff );
		GFL_BMPWIN_MakeScreen( crs->drawwin[i].win );
		GFL_BMPWIN_TransVramCharacter( crs->drawwin[i].win );
		PRINT_UTIL_Setup( crs->drawwin[i].printUtil, crs->drawwin[i].win );
	}

	GFL_BG_LoadScreenReq(FRAME_BACK_S);

	crs->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_COMPATI );

//		PRINTSYS_Init( HEAPID_COMPATI );
	crs->printQue = PRINTSYS_QUE_Create( HEAPID_COMPATI );

	crs->mm = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_matsu_dat, HEAPID_COMPATI);
	for(i = 0; i < CRBMPWIN_MAX; i++){
		crs->strbuf_win[i] = GFL_STR_CreateBuffer( 64, HEAPID_COMPATI );
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�֘A�j��
 *
 * @param   crs		
 */
//--------------------------------------------------------------
static void CCLocal_MessageExit(COMPATI_RESULT_SYS *crs)
{
	int i;
	
	GFL_MSG_Delete(crs->mm);
	PRINTSYS_QUE_Delete(crs->printQue);
	GFL_FONT_Delete(crs->fontHandle);
	for(i = 0; i < CRBMPWIN_MAX; i++){
		GFL_BMPWIN_Delete(crs->drawwin[i].win);
		GFL_STR_DeleteBuffer(crs->strbuf_win[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �A�N�^�[�ݒ�
 *
 * @param   crs		
 */
//--------------------------------------------------------------
static void CCLocal_ActorSetting(COMPATI_RESULT_SYS *crs)
{
	GFL_CLSYS_INIT clsys_init = GFL_CLSYSINIT_DEF_DIVSCREEN;
	
	clsys_init.oamst_main = 1;	//�ʐM�A�C�R���̕�
	clsys_init.oamnum_main = 128-1;
	clsys_init.tr_cell = CR_ACT_MAX;
	GFL_CLACT_SYS_Create(&clsys_init, &CompatiVramBank, HEAPID_COMPATI);
	
	crs->clunit = GFL_CLACT_UNIT_Create(CR_ACT_MAX, 0, HEAPID_COMPATI);
	GFL_CLACT_UNIT_SetDefaultRend(crs->clunit);

	//OBJ�\��ON
	GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
}


//==============================================================================
//	�V�[�P���X
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ���C���V�[�P���X�F�����ݒ�
 * @param   crs		
 * @retval  �V�[�P���X����
 */
//--------------------------------------------------------------
static int CRSeq_Init(COMPATI_RESULT_SYS *crs)
{
	switch(crs->local_seq){
	case 0:
		WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 
			WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_COMPATI);
		crs->local_seq++;
		break;
	case 1:
		if(WIPE_SYS_EndCheck() == TRUE){
		//	return SEQ_NEXT;
			return SEQ_END;
		}
		break;
	}
	
	return SEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���C���V�[�P���X�F�J�E���g�_�E��
 * @param   crs		
 * @retval  �V�[�P���X����
 */
//--------------------------------------------------------------
static int CRSeq_Countdown(COMPATI_RESULT_SYS *crs)
{
	crs->local_timer++;
	if(crs->local_timer < 60){
		return SEQ_CONTINUE;
	}
	crs->local_timer = 0;

	switch(crs->local_seq){
	case 0:
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_X_SET, 256);
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_Y_SET, 0);
		crs->local_seq++;
		break;
	case 1:
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_X_SET, 0);
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_Y_SET, 256);
		crs->local_seq++;
		break;
	case 2:
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_X_SET, 256);
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_Y_SET, 256);
		crs->local_seq++;
		break;
	case 3:
		GFL_BG_SetVisible(FRAME_COUNT_M, VISIBLE_OFF);
		return SEQ_NEXT;
	}
	return SEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���C���V�[�P���X�F���C��
 * @param   crs		
 * @retval  �V�[�P���X����
 */
//--------------------------------------------------------------
static int CRSeq_Main(COMPATI_RESULT_SYS *crs)
{
	return SEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���C���V�[�P���X�F�I������
 * @param   crs		
 * @retval  �V�[�P���X����
 */
//--------------------------------------------------------------
static int CRSeq_End(COMPATI_RESULT_SYS *crs)
{
	switch(crs->local_seq){
	case 0:
		WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, 
			WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_COMPATI);
		crs->local_seq++;
		break;
	case 1:
		if(WIPE_SYS_EndCheck() == TRUE){
			return SEQ_FINISH;
		}
		break;
	}
	return SEQ_CONTINUE;
}
