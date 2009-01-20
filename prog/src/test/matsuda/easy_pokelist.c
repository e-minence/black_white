//==============================================================================
/**
 * @file	easy_pokelist.c
 * @brief	�ȈՃ|�P�������X�g���
 * @author	matsuda
 * @date	2008.12.05(��)
 *
 * ���K�@�\�ɂ������͑S�������ȈՓI�ȃ|�P�������X�g��ʂł�
 * �g�p����ꍇ�͂��̂���Ŏg���Ă�������
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
#include "msg\msg_d_matsu.h"
#include "test/performance.h"
#include "font/font.naix"
#include "test_graphic/easy_pokelist.naix"
#include "pm_define.h"
#include "test/easy_pokelist.h"
#include "system\gfl_use.h"



//==============================================================================
//	�萔��`
//==============================================================================
///�A�N�^�[�ő吔
#define ACT_MAX			(64)

///�t�H���g�̃p���b�g�ԍ�
#define D_FONT_PALNO	(14)

///�^�C�g�����SBG��BG�v���C�I���e�B
enum{
	BGPRI_MSG = 0,
	BGPRI_MIST = 0,
	BGPRI_TITLE_LOGO = 2,
	BGPRI_3D = 1,
};

///�t���[���ԍ�
enum{
	//���C�����
	FRAME_WALL_M = GFL_BG_FRAME3_M,		///<�ǎ�
	FRAME_MSG_M = GFL_BG_FRAME1_M,		///<���b�Z�[�W�t���[��
	
	//�T�u���
	FRAME_WALL_S = GFL_BG_FRAME3_S,		///<�ǎ�
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
	int wait;
	int master_bright;
	int mode;			///<���̃��j���[��ʂֈ����n�����[�h
	
	GFL_FONT		*fontHandle;
	PRINT_QUE		*printQue;
	PRINT_STREAM	*printStream;
	GFL_MSGDATA		*mm;
	STRBUF			*strbuf_name[TEMOTI_POKEMAX];
	GFL_TCBLSYS		*tcbl;
	DM_MSG_DRAW_WIN drawwin_poke[TEMOTI_POKEMAX];
	
	int push_timer;				//[PUSH START BUTTON]�̓_�ŊԊu���J�E���g
	int push_visible;
	
	//�A�N�^�[
	GFL_CLUNIT *clunit;
	GFL_CLWK	*clwk_icon;		//�A�C�e���A�C�R���A�N�^�[
	NNSG2dImagePaletteProxy	icon_pal_proxy;
	NNSG2dCellDataBank	*icon_cell_data;
	NNSG2dAnimBankData	*icon_anm_data;
	void	*icon_cell_res;
	void	*icon_anm_res;

	EASY_POKELIST_PARENT *epp;
	
	int cursor_y;
	int poke_num;
	GFL_TCB *vintr_tcb;
}EASY_POKELIST_WORK;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
GFL_PROC_RESULT EasyPokeListInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT EasyPokeListMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT EasyPokeListEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void Local_VramSetting(EASY_POKELIST_WORK *ew);
static void Local_BGFrameSetting(EASY_POKELIST_WORK *ew);
static void Local_MessageSetting(EASY_POKELIST_WORK *ew);
static void Local_ActorSetting(EASY_POKELIST_WORK *ew);
static void Local_MessagePrintMain(EASY_POKELIST_WORK *ew);
static void Local_BGGraphicLoad(EASY_POKELIST_WORK *ew);
static void Local_MessagePut(EASY_POKELIST_WORK *ew, int win_index, STRBUF *strbuf, int x, int y);
static void Local_MsgLoadPokeNameAll(EASY_POKELIST_WORK *ew, POKEPARTY *party);
static void VintrTCB_VblankFunc(GFL_TCB *tcb, void *work);


//==============================================================================
//	�f�[�^
//==============================================================================
///�^�C�g����ʌĂяo���悤��PROC�f�[�^
const GFL_PROC_DATA EasyPokeListData = {
	EasyPokeListInit,
	EasyPokeListMain,
	EasyPokeListEnd,
};


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
GFL_PROC_RESULT EasyPokeListInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	EASY_POKELIST_WORK *ew;
	EASY_POKELIST_PARENT *epp = pwk;
	
	// �e����ʃ��W�X�^������
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);
	G2_BlendNone();
	G2S_BlendNone();
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_POKELIST, 0x40000 );
	ew = GFL_PROC_AllocWork( proc, sizeof(EASY_POKELIST_WORK), HEAPID_POKELIST );
	GFL_STD_MemClear(ew, sizeof(EASY_POKELIST_WORK));

	if(epp == NULL){	//NULL�̎��̓f�o�b�O�ł̎g�p�Ȃ̂ł����ō쐬���Ă��܂�
		POKEMON_PARAM *pp;
		int i, monsno;
		u8 mac_address[6];
		
		epp = GFL_HEAP_AllocClearMemory(HEAPID_POKELIST, sizeof(EASY_POKELIST_PARENT));
		epp->party = PokeParty_AllocPartyWork(HEAPID_POKELIST);
		
		//�K���Ƀ|�P�������쐬
		OS_GetMacAddress(mac_address);
		pp = PP_Create(1, 50, 123456, HEAPID_POKELIST);
		for(i = 0; i < TEMOTI_POKEMAX; i++){
			OS_TPrintf("monsno = %d\n", mac_address[i]);
			monsno = (mac_address[i] == 0) ? 1 : mac_address[i];
			PP_Setup(pp, monsno, 50, 123456);
			PokeParty_Add(epp->party, pp);
		}
		GFL_HEAP_FreeMemory(pp);
	}
	ew->epp = epp;
	ew->poke_num = PokeParty_GetPokeCount(ew->epp->party);
	
	// �㉺��ʐݒ�
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();

	//TCB�쐬
	ew->tcbl = GFL_TCBL_Init( HEAPID_POKELIST, HEAPID_POKELIST, 4, 32 );

	//VRAM�ݒ� & BG�t���[���ݒ�
	Local_VramSetting(ew);
	Local_BGFrameSetting(ew);
	Local_BGGraphicLoad(ew);
	
	//���b�Z�[�W�֘A�쐬
	GFL_BMPWIN_Init( HEAPID_POKELIST );
	GFL_FONTSYS_Init();
	Local_MessageSetting(ew);
	Local_MsgLoadPokeNameAll(ew, epp->party);

	//�A�N�^�[�ݒ�
	Local_ActorSetting(ew);

	GFL_BG_SetVisible(FRAME_WALL_M, VISIBLE_ON);
	GFL_BG_SetVisible(FRAME_MSG_M, VISIBLE_ON);
//	GFL_BG_SetVisible(FRAME_WALL_S, VISIBLE_ON);

	ew->vintr_tcb = GFUser_VIntr_CreateTCB(VintrTCB_VblankFunc, ew, 5);
	
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
GFL_PROC_RESULT EasyPokeListMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	EASY_POKELIST_WORK* ew = mywk;
	enum{
		SEQ_WAIT,
		SEQ_FADEIN,
		SEQ_MAIN,
		SEQ_FADEOUT,
	};
	
	GFL_TCBL_Main( ew->tcbl );
	Local_MessagePrintMain(ew);
	
	if(ew->seq == SEQ_MAIN){
		int trg = GFL_UI_KEY_GetTrg();
		if(trg & (PAD_BUTTON_START | PAD_BUTTON_SELECT | PAD_BUTTON_A)){
			ew->mode = trg;
			ew->seq = SEQ_FADEOUT;
		}
		else if(trg & PAD_BUTTON_B){
			ew->cursor_y = -1;
			ew->mode = trg;
			ew->seq = SEQ_FADEOUT;
		}
	}
	
	switch(ew->seq){
	case SEQ_WAIT:
		ew->master_bright = -16;
		ew->seq++;
//		break;
	case SEQ_FADEIN:
		ew->master_bright++;
		GX_SetMasterBrightness(ew->master_bright);
		GXS_SetMasterBrightness(ew->master_bright);
		if(ew->master_bright >= 0){
			ew->seq++;
		}
		break;
	case SEQ_MAIN:
		switch(GFL_UI_KEY_GetRepeat()){
		case PAD_KEY_UP:
			ew->cursor_y--;
			if(ew->cursor_y < 0){
				ew->cursor_y = ew->poke_num - 1;
			}
			Local_MsgLoadPokeNameAll(ew, ew->epp->party);
			break;
		case PAD_KEY_DOWN:
			ew->cursor_y++;
			if(ew->cursor_y >= ew->poke_num){
				ew->cursor_y = 0;
			}
			Local_MsgLoadPokeNameAll(ew, ew->epp->party);
			break;
		}
		break;
	case SEQ_FADEOUT:
		ew->master_bright--;
		GX_SetMasterBrightness(ew->master_bright);
		GXS_SetMasterBrightness(ew->master_bright);
		if(ew->master_bright <= -16){
			return GFL_PROC_RES_FINISH;
		}
		break;
	}
	
	GFL_CLACT_SYS_Main();
	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
GFL_PROC_RESULT EasyPokeListEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	EASY_POKELIST_WORK* ew = mywk;
	EASY_POKELIST_PARENT *epp = pwk;
	int i, mode;
	
	mode = ew->mode;

	GFL_TCB_DeleteTask(ew->vintr_tcb);
	
	if(pwk == NULL){
		GFL_HEAP_FreeMemory(ew->epp->party);
		GFL_HEAP_FreeMemory(ew->epp);
	}
	else{
		epp->select_pos = ew->cursor_y;
		OS_TPrintf("�I���|�P������%d\n", epp->select_pos);
	}
	
	for(i = 0; i < TEMOTI_POKEMAX; i++){
		GFL_STR_DeleteBuffer(ew->strbuf_name[i]);
		GFL_BMPWIN_Delete(ew->drawwin_poke[i].win);
	}

	PRINTSYS_QUE_Delete(ew->printQue);
	GFL_MSG_Delete(ew->mm);

	GFL_CLACT_UNIT_Delete(ew->clunit);
	GFL_CLACT_SYS_Delete();

	GFL_FONT_Delete(ew->fontHandle);
	GFL_TCBL_Exit(ew->tcbl);
	
	GFL_BG_Exit();
	GFL_BMPWIN_Exit();

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_POKELIST);
	
	return GFL_PROC_RES_FINISH;
}

static void VintrTCB_VblankFunc(GFL_TCB *tcb, void *work)
{
	GFL_CLACT_SYS_VBlankFunc();
}

//==============================================================================
//	
//==============================================================================
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
	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};
//--------------------------------------------------------------
/**
 * @brief   VRAM�o���N�����[�h�ݒ�
 * @param   ew		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_VramSetting(EASY_POKELIST_WORK *ew)
{

	static const GFL_BG_SYS_HEADER sysHeader = {
		GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
	};
	
	GFL_DISP_SetBank( &vramBank );
	GFL_BG_Init( HEAPID_POKELIST );
	GFL_BG_SetBGMode( &sysHeader );
}

//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 * @param   ew		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_BGFrameSetting(EASY_POKELIST_WORK *ew)
{
	static const GFL_BG_BGCNT_HEADER bgcnt_frame[] = {	//���C�����BG�t���[��
		{//FRAME_WALL_M
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x8000,
			GX_BG_EXTPLTT_01, BGPRI_TITLE_LOGO, 0, 0, FALSE
		},
		{//FRAME_MSG_M
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, 0x4000,
			GX_BG_EXTPLTT_01, BGPRI_MIST, 0, 0, FALSE
		},
	};

	static const GFL_BG_BGCNT_HEADER sub_bgcnt_frame[] = {	//�T�u���BG�t���[��
		{//FRAME_WALL_S
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x8000,
			GX_BG_EXTPLTT_01, BGPRI_TITLE_LOGO, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( FRAME_WALL_M,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_MSG_M,   &bgcnt_frame[1],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_WALL_S,   &sub_bgcnt_frame[0],   GFL_BG_MODE_TEXT );

	GFL_BG_FillCharacter( FRAME_WALL_M, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_MSG_M, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_WALL_S, 0x00, 1, 0 );

	GFL_BG_FillScreen( FRAME_WALL_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_MSG_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_WALL_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

	GFL_BG_LoadScreenReq( FRAME_WALL_M );
	GFL_BG_LoadScreenReq( FRAME_MSG_M );
	GFL_BG_LoadScreenReq( FRAME_WALL_S );
}

//--------------------------------------------------------------
/**
 * @brief   BG�O���t�B�b�N��VRAM�֓]��
 * @param   ew		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_BGGraphicLoad(EASY_POKELIST_WORK *ew)
{
	//���C�����
	GFL_ARC_UTIL_TransVramBgCharacter(
		ARCID_EASYPOKELIST, NARC_easy_pokelist_list_sub_NCGR, 
		FRAME_WALL_M, 0, 0x8000, 0, HEAPID_POKELIST);
	GFL_ARC_UTIL_TransVramScreen(
		ARCID_EASYPOKELIST, NARC_easy_pokelist_list_sub_NSCR, 
		FRAME_WALL_M, 0, 0, 0, HEAPID_POKELIST);
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_EASYPOKELIST, NARC_easy_pokelist_list_sub_NCLR, 
		PALTYPE_MAIN_BG, 0, 0, HEAPID_POKELIST);

	//�T�u���
	GFL_ARC_UTIL_TransVramBgCharacter(
		ARCID_EASYPOKELIST, NARC_easy_pokelist_list_sub_NCGR, 
		FRAME_WALL_S, 0, 0x8000, 0, HEAPID_POKELIST);
	GFL_ARC_UTIL_TransVramScreen(
		ARCID_EASYPOKELIST, NARC_easy_pokelist_list_sub_NSCR, 
		FRAME_WALL_S, 0, 0, 0, HEAPID_POKELIST);
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_EASYPOKELIST, NARC_easy_pokelist_list_sub_NCLR, 
		PALTYPE_SUB_BG, 0, 0, HEAPID_POKELIST);
}

//--------------------------------------------------------------
/**
 * @brief   �A�N�^�[�ݒ�
 * @param   ew		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_ActorSetting(EASY_POKELIST_WORK *ew)
{
	GFL_CLSYS_INIT clsys_init = GFL_CLSYSINIT_DEF_DIVSCREEN;
	
	clsys_init.oamst_main = 1;	//�ʐM�A�C�R���̕�
	clsys_init.oamnum_main = 128-1;
	clsys_init.tr_cell = ACT_MAX;
	GFL_CLACT_SYS_Create(&clsys_init, &vramBank, HEAPID_POKELIST);
	
	ew->clunit = GFL_CLACT_UNIT_Create(ACT_MAX, 0,HEAPID_POKELIST);
	GFL_CLACT_UNIT_SetDefaultRend(ew->clunit);

	//OBJ�\��ON
	GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�֘A�ݒ�
 * @param   ew		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_MessageSetting(EASY_POKELIST_WORK *ew)
{
	int i;

	//BMPWIN�쐬
	for(i = 0; i < TEMOTI_POKEMAX; i++){
		ew->drawwin_poke[i].win 
			= GFL_BMPWIN_Create(FRAME_MSG_M, 2, 2+i*2, 30, 2, D_FONT_PALNO, GFL_BMP_CHRAREA_GET_F);
		ew->drawwin_poke[i].bmp = GFL_BMPWIN_GetBmp(ew->drawwin_poke[i].win);
		GFL_BMP_Clear( ew->drawwin_poke[i].bmp, 0x00 );
		GFL_BMPWIN_MakeScreen( ew->drawwin_poke[i].win );
		GFL_BMPWIN_TransVramCharacter( ew->drawwin_poke[i].win );
		PRINT_UTIL_Setup( ew->drawwin_poke[i].printUtil, ew->drawwin_poke[i].win );
	}
	GFL_BG_LoadScreenReq( FRAME_MSG_M );

	//�t�H���g�쐬
	ew->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_POKELIST );

	ew->printQue = PRINTSYS_QUE_Create( HEAPID_POKELIST );

	ew->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_matsu_dat, HEAPID_POKELIST );

	//STRBUF�쐬
	for(i = 0; i < TEMOTI_POKEMAX; i++){
		ew->strbuf_name[i] = GFL_STR_CreateBuffer( 64, ew->heapID );
	}

	//�t�H���g�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
		0x20*D_FONT_PALNO, 0x20, HEAPID_POKELIST);
	
	GFL_MSGSYS_SetLangID( 1 );	//JPN_KANJI
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�`�惁�C��
 * @param   ew		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_MessagePrintMain(EASY_POKELIST_WORK *ew)
{
	int i;
	BOOL que_ret;
	
	que_ret = PRINTSYS_QUE_Main( ew->printQue );

	for(i = 0; i < TEMOTI_POKEMAX; i++){
		if( PRINT_UTIL_Trans( ew->drawwin_poke[i].printUtil, ew->printQue ) == FALSE){
			//return GFL_PROC_RES_CONTINUE;
		}
		else{
			if(que_ret == TRUE && ew->drawwin_poke[i].message_req == TRUE){
	//			GFL_BMP_Clear( ew->bmp, 0xff );
				GFL_BMPWIN_TransVramCharacter( ew->drawwin_poke[i].win );
				ew->drawwin_poke[i].message_req = FALSE;
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W��\������
 *
 * @param   ew			
 * @param   strbuf		�\�����郁�b�Z�[�W�f�[�^
 * @param   x			X���W(dot)
 * @param   y			Y���W(dot)
 */
//--------------------------------------------------------------
static void Local_MessagePut(EASY_POKELIST_WORK *ew, int win_index, STRBUF *strbuf, int x, int y)
{
	GFL_BMP_Clear( ew->drawwin_poke[win_index].bmp, 0x00 );
	PRINTSYS_PrintQue(ew->printQue, ew->drawwin_poke[win_index].bmp, x, y, strbuf, ew->fontHandle);
	ew->drawwin_poke[win_index].message_req = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   [PUSH START BUTTON]�̕�����`��
 *
 * @param   ew		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_MsgLoadPokeNameAll(EASY_POKELIST_WORK *ew, POKEPARTY *party)
{
	int i, poke_max;
	POKEMON_PARAM *pp;
	int x_ofs;
	
	poke_max = PokeParty_GetPokeCount(party);
	for(i = 0; i < poke_max; i++){
		pp = PokeParty_GetMemberPointer(party, i);
		PP_Get(pp, ID_PARA_nickname, ew->strbuf_name[i]);
		x_ofs = (i == ew->cursor_y) ? 2 : 0;
		Local_MessagePut(ew, i, ew->strbuf_name[i], x_ofs*8, 0);
	}
}

