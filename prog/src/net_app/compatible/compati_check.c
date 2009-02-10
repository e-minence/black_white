//==============================================================================
/**
 * @file	compati_check.c
 * @brief	�����`�F�b�N
 * @author	matsuda
 * @date	2009.02.09(��)
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
	CCBMPWIN_TITLE,		///<�Q�[����
	CCBMPWIN_POINT,		///<���_
	
	CCBMPWIN_MAX,
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
#define CC_ACT_MAX			(64)

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
	PRINT_STREAM	*ps_entry[CCBMPWIN_MAX];
	GFL_MSGDATA		*mm;
	STRBUF			*strbuf_win[CCBMPWIN_MAX];
	GFL_TCBLSYS		*tcbl;
	MSG_DRAW_WIN drawwin[CCBMPWIN_MAX];	//+1 = ���C�����b�Z�[�W�E�B���h�E
	
	//�A�N�^�[
	GFL_CLUNIT *clunit;
	GFL_CLWK *circle_cap[CC_ACT_MAX];

	GFL_TCB *vintr_tcb;
	
	u16 light_pal[16];			///<�~�����������̖��邢�F�̃p���b�g�f�[�^
	
	u32 cgr_id[CHARID_END];
	u32 cell_id[CELLID_END];
	u32 pltt_id[PLTTID_END];
	
	CC_CIRCLE_PACKAGE circle_package;	///<�g�p����T�[�N���f�[�^
}COMPATI_SYS;



//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT CompatiCheck_ProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT CompatiCheck_ProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT CompatiCheck_ProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void CCLocal_VblankFunc(GFL_TCB *tcb, void *work);
static void CCLocal_VramSetting(COMPATI_SYS *cs);
static void CCLocal_BGFrameSetting(COMPATI_SYS *cs);
static void CCLocal_BGGraphicLoad(ARCHANDLE *hdl);
static void CCLocal_MessageSetting(COMPATI_SYS *cs);
static void CCLocal_MessageExit(COMPATI_SYS *cs);
static void CCLocal_ActorSetting(COMPATI_SYS *cs);
static void CCLocal_CircleActor_Create(COMPATI_SYS *cs, ARCHANDLE *hdl);
static void CCLocal_CircleActor_Delete(COMPATI_SYS *cs);
static void CCLocal_CircleActor_PosSet(COMPATI_SYS *cs, const CC_CIRCLE_PACKAGE *circle_package);
static int CCSeq_Init(COMPATI_SYS *cs);
static int CCSeq_Countdown(COMPATI_SYS *cs);
static int CCSeq_Main(COMPATI_SYS *cs);
static int CCSeq_End(COMPATI_SYS *cs);



//==============================================================================
//	�f�[�^
//==============================================================================
///�Ăяo���p��PROC�f�[�^
const GFL_PROC_DATA CompatiCheckProcData = {
	CompatiCheck_ProcInit,
	CompatiCheck_ProcMain,
	CompatiCheck_ProcEnd,
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
static int (* const CCMainSeqTbl[])(COMPATI_SYS *cs) = {
	CCSeq_Init,
	CCSeq_Countdown,
	CCSeq_Main,
	CCSeq_End,
};
///���C���V�[�P���X�ԍ��@��CCMainSeqTbl�ƕ��т𓯂��ɂ��Ă������ƁI
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
static GFL_PROC_RESULT CompatiCheck_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	COMPATI_SYS* cs;
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
	cs = GFL_PROC_AllocWork( proc, sizeof(COMPATI_SYS), HEAPID_COMPATI );
	GFL_STD_MemClear(cs, sizeof(COMPATI_SYS));
	
	//�t�@�C���I�[�v��
	hdl = GFL_ARC_OpenDataHandle(ARCID_COMPATI_CHECK, HEAPID_COMPATI);
	{
		//VRAM�o���N�ݒ�
		CCLocal_VramSetting(cs);
		CCLocal_BGFrameSetting(cs);
		CCLocal_BGGraphicLoad(hdl);
		
		//�e�탉�C�u�����ݒ�
		GFL_BMPWIN_Init(HEAPID_COMPATI);
		GFL_FONTSYS_Init();
		cs->tcbl = GFL_TCBL_Init(HEAPID_COMPATI, HEAPID_COMPATI, 4, 32);

		//���b�Z�[�W�`��ׂ̈̏���
		CCLocal_MessageSetting(cs);

		//�A�N�^�[�ݒ�
		CCLocal_ActorSetting(cs);
		CCLocal_CircleActor_Create(cs, hdl);
		CCLocal_CircleActor_PosSet(cs, &cppw->circle_package);
	}
	//�t�@�C���N���[�Y
	GFL_ARC_CloseDataHandle(hdl);
	
	//V�u�����NTCB�o�^
	cs->vintr_tcb = GFUser_VIntr_CreateTCB(CCLocal_VblankFunc, cs, 5);
	
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
static GFL_PROC_RESULT CompatiCheck_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	COMPATI_SYS* cs = mywk;
	int ret, i;
	BOOL que_ret;
	
	GFL_TCBL_Main( cs->tcbl );
	que_ret = PRINTSYS_QUE_Main( cs->printQue );
	for(i = 0; i < CCBMPWIN_MAX; i++){
		if( PRINT_UTIL_Trans( cs->drawwin[i].printUtil, cs->printQue ) == FALSE){
			//return GFL_PROC_RES_CONTINUE;
		}
		else{
			if(que_ret == TRUE && cs->drawwin[i].message_req == TRUE){
	//			GFL_BMP_Clear( cs->bmp, 0xff );
				GFL_BMPWIN_TransVramCharacter( cs->drawwin[i].win );
				cs->drawwin[i].message_req = FALSE;
			}
		}
	}
	
	ret = CCMainSeqTbl[*seq](cs);
	switch(ret){
	case SEQ_CONTINUE:
		break;
	case SEQ_FINISH:
		return GFL_PROC_RES_FINISH;
	case SEQ_NEXT:
		cs->local_seq = 0;
		cs->local_timer = 0;
		(*seq)++;
		break;
	default:
		cs->local_seq = 0;
		cs->local_timer = 0;
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
static GFL_PROC_RESULT CompatiCheck_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	COMPATI_SYS* cs = mywk;

	GFL_TCB_DeleteTask(cs->vintr_tcb);
	
	CCLocal_CircleActor_Delete(cs);
	CCLocal_MessageExit(cs);

	GFL_CLACT_UNIT_Delete(cs->clunit);
	GFL_CLACT_SYS_Delete();

	GFL_FONT_Delete(cs->fontHandle);
	GFL_TCBL_Exit(cs->tcbl);
	
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
 * @param   cs		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void CCLocal_VramSetting(COMPATI_SYS *cs)
{
	GFL_DISP_SetBank( &CompatiVramBank );
	GFL_BG_Init( HEAPID_COMPATI );
	GFL_BG_SetBGMode( &CompatiBgSysHeader );
}

//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 * @param   cs		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void CCLocal_BGFrameSetting(COMPATI_SYS *cs)
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
	GFL_BG_SetVisible(FRAME_COUNT_M, VISIBLE_ON);
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
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_MessageSetting(COMPATI_SYS *cs)
{
	int i;

	cs->drawwin[CCBMPWIN_TITLE].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 2, 2, 30, 2, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	cs->drawwin[CCBMPWIN_POINT].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 1, 18, 30, 4, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	for(i = 0; i < CCBMPWIN_MAX; i++){
		GF_ASSERT(cs->drawwin[i].win != NULL);
		cs->drawwin[i].bmp = GFL_BMPWIN_GetBmp(cs->drawwin[i].win);
		GFL_BMP_Clear( cs->drawwin[i].bmp, 0xff );
		GFL_BMPWIN_MakeScreen( cs->drawwin[i].win );
		GFL_BMPWIN_TransVramCharacter( cs->drawwin[i].win );
		PRINT_UTIL_Setup( cs->drawwin[i].printUtil, cs->drawwin[i].win );
	}

	GFL_BG_LoadScreenReq(FRAME_BACK_S);

	cs->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_COMPATI );

//		PRINTSYS_Init( HEAPID_COMPATI );
	cs->printQue = PRINTSYS_QUE_Create( HEAPID_COMPATI );

	cs->mm = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_matsu_dat, HEAPID_COMPATI);
	for(i = 0; i < CCBMPWIN_MAX; i++){
		cs->strbuf_win[i] = GFL_STR_CreateBuffer( 64, HEAPID_COMPATI );
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�֘A�j��
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_MessageExit(COMPATI_SYS *cs)
{
	int i;
	
	GFL_MSG_Delete(cs->mm);
	PRINTSYS_QUE_Delete(cs->printQue);
	GFL_FONT_Delete(cs->fontHandle);
	for(i = 0; i < CCBMPWIN_MAX; i++){
		GFL_BMPWIN_Delete(cs->drawwin[i].win);
		GFL_STR_DeleteBuffer(cs->strbuf_win[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �A�N�^�[�ݒ�
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_ActorSetting(COMPATI_SYS *cs)
{
	GFL_CLSYS_INIT clsys_init = GFL_CLSYSINIT_DEF_DIVSCREEN;
	
	clsys_init.oamst_main = 1;	//�ʐM�A�C�R���̕�
	clsys_init.oamnum_main = 128-1;
	clsys_init.tr_cell = CC_ACT_MAX;
	GFL_CLACT_SYS_Create(&clsys_init, &CompatiVramBank, HEAPID_COMPATI);
	
	cs->clunit = GFL_CLACT_UNIT_Create(CC_ACT_MAX, 0, HEAPID_COMPATI);
	GFL_CLACT_UNIT_SetDefaultRend(cs->clunit);

	//OBJ�\��ON
	GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * @brief   �~�A�N�^�[�쐬
 *
 * @param   cs		
 * @param   hdl		�A�[�J�C�u�n���h��
 */
//--------------------------------------------------------------
static void CCLocal_CircleActor_Create(COMPATI_SYS *cs, ARCHANDLE *hdl)
{
	int i;
	
	//���\�[�X�o�^
	cs->cgr_id[CHARID_CIRCLE] = GFL_CLGRP_CGR_Register(
		hdl, NARC_compati_check_cc_circle_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_COMPATI);
	cs->cell_id[CELLID_CIRCLE] = GFL_CLGRP_CELLANIM_Register(
		hdl, NARC_compati_check_cc_circle_NCER, NARC_compati_check_cc_circle_NANR, HEAPID_COMPATI);
	//�p���b�g���\�[�X�o�^(�������~�����F��ς���̂ŉ~�̐����o�^)
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		cs->pltt_id[PLTTID_CIRCLE + i] = GFL_CLGRP_PLTT_RegisterEx(hdl, 
			NARC_compati_check_cc_circle_NCLR,
			CLSYS_DRAW_MAIN, 0, 0, 1, HEAPID_COMPATI);
	}
	
	//�A�N�^�[�o�^
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		cs->circle_cap[i] = GFL_CLACT_WK_Create(cs->clunit, cs->cgr_id[CHARID_CIRCLE], 
			cs->pltt_id[PLTTID_CIRCLE + i], cs->cell_id[CELLID_CIRCLE], 
			&CircleObjParam, CLSYS_DEFREND_MAIN, HEAPID_COMPATI);
		GFL_CLACT_WK_AddAnmFrame(cs->circle_cap[i], FX32_ONE);
		GFL_CLACT_WK_SetDrawEnable(cs->circle_cap[i], FALSE);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �~�A�N�^�[�j��
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_CircleActor_Delete(COMPATI_SYS *cs)
{
	int i;
	
	//�A�N�^�[�폜
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		GFL_CLACT_WK_Remove(cs->circle_cap[i]);
	}
	
	//���\�[�X���
	GFL_CLGRP_CGR_Release(cs->cgr_id[CHARID_CIRCLE]);
	GFL_CLGRP_CELLANIM_Release(cs->cgr_id[CELLID_CIRCLE]);
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		GFL_CLGRP_PLTT_Release(cs->cgr_id[PLTTID_CIRCLE + i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �~�A�N�^�[�̍��W�A�A�j����ݒ�
 *
 * @param   cs					
 * @param   circle_package		�T�[�N���f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void CCLocal_CircleActor_PosSet(COMPATI_SYS *cs, const CC_CIRCLE_PACKAGE *circle_package)
{
	int i;
	GFL_CLACTPOS pos;
	
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		if(circle_package->data[i].type == CC_CIRCLE_TYPE_NULL){
			return;
		}
		pos.x = circle_package->data[i].x;
		pos.y = circle_package->data[i].y;
		GFL_CLACT_WK_SetPos(cs->circle_cap[i], &pos, CLSYS_DEFREND_MAIN);
		GFL_CLACT_WK_SetAnmSeq(cs->circle_cap[i], circle_package->data[i].type);
		GFL_CLACT_WK_SetDrawEnable(cs->circle_cap[i], TRUE);
	}
}


//==============================================================================
//	�V�[�P���X
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ���C���V�[�P���X�F�����ݒ�
 * @param   cs		
 * @retval  �V�[�P���X����
 */
//--------------------------------------------------------------
static int CCSeq_Init(COMPATI_SYS *cs)
{
	switch(cs->local_seq){
	case 0:
		WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 
			WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_COMPATI);
		cs->local_seq++;
		break;
	case 1:
		if(WIPE_SYS_EndCheck() == TRUE){
			return SEQ_NEXT;
		}
		break;
	}
	
	return SEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���C���V�[�P���X�F�J�E���g�_�E��
 * @param   cs		
 * @retval  �V�[�P���X����
 */
//--------------------------------------------------------------
static int CCSeq_Countdown(COMPATI_SYS *cs)
{
	cs->local_timer++;
	if(cs->local_timer < 60){
		return SEQ_CONTINUE;
	}
	cs->local_timer = 0;

	switch(cs->local_seq){
	case 0:
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_X_SET, 256);
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_Y_SET, 0);
		cs->local_seq++;
		break;
	case 1:
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_X_SET, 0);
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_Y_SET, 256);
		cs->local_seq++;
		break;
	case 2:
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_X_SET, 256);
		GFL_BG_SetScroll(FRAME_COUNT_M, GFL_BG_SCROLL_Y_SET, 256);
		cs->local_seq++;
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
 * @param   cs		
 * @retval  �V�[�P���X����
 */
//--------------------------------------------------------------
static int CCSeq_Main(COMPATI_SYS *cs)
{
	return SEQ_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���C���V�[�P���X�F�I������
 * @param   cs		
 * @retval  �V�[�P���X����
 */
//--------------------------------------------------------------
static int CCSeq_End(COMPATI_SYS *cs)
{
	return SEQ_CONTINUE;
}
