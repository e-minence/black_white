//============================================================================================
/**
 * @file	mysign.c
 * @bfief	�}�C�T�C���쐬����
 * @author	Akito Mori
 * @date	05.10.05
 */
//============================================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "arc_def.h"
#include "font/font.naix"

#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "message.naix"
#include "mysign.naix"
#include "msg/msg_oekaki.h"

#include "savedata/config.h"
#include "savedata/trainercard_data.h"
#include "savedata/record.h"
#include "system/wipe.h"
#include "gamesystem\msgspeed.h"

#include "app/mysign.h"
#include "mysign_local.h"
#include "test/ariizumi/ari_debug.h"

// SE�p��`
#if SIGN_USE_SND
#define OEKAKI_MOVE_SE		(SEQ_SE_DP_SELECT)
#define OEKAKI_DECIDE_SE	(SEQ_SE_DP_SELECT)
#define OEKAKI_BS_SE		(SEQ_SE_DP_SELECT)
#endif

#define BUTTON_NUM			( 1 )

#include "mysign.naix"			// �O���t�B�b�N�A�[�J�C�u��`

#include "startmenu.naix"

//============================================================================================
//	�萔��`
//============================================================================================
enum {
	SEQ_IN = 0,
	SEQ_MAIN,
	SEQ_OUT,
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

/*** �֐��v���g�^�C�v ***/
static void VBlankFunc( GFL_TCB *tcb , void * work );
static void VramBankSet(void);
static void BgInit(void);
static void InitWork( MYSIGN_WORK *wk );
static void FreeWork( MYSIGN_WORK *wk );
static void BgExit(void);
static void BgGraphicSet( MYSIGN_WORK * wk );
static void InitCellActor(MYSIGN_WORK *wk);
static void SetCellActor(MYSIGN_WORK *wk);
static void BmpWinInit(MYSIGN_WORK *wk );
static void BmpWinDelete( MYSIGN_WORK *wk );
static void ControlCursor(MYSIGN_WORK *wk);
static void CursorAppearUpDate(MYSIGN_WORK *wk, int arrow);
static void NormalTouchFunc(MYSIGN_WORK *wk);
static int Oekaki_MainNormal( MYSIGN_WORK *wk, int seq );
static void EndSequenceCommonFunc( MYSIGN_WORK *wk );
static int Oekaki_EndSelectPutString( MYSIGN_WORK *wk, int seq );
static int Oekaki_EndSelectWait( MYSIGN_WORK *wk, int seq );
static int 	Oekaki_EndChild( MYSIGN_WORK *wk, int seq );
static int 	Oekaki_EndChildWait( MYSIGN_WORK *wk, int seq );
static int 	Oekaki_EndChildWait2( MYSIGN_WORK *wk, int seq );
static int Oekaki_ReWrite( MYSIGN_WORK *wk, int seq );
static int Oekaki_ReWriteWait( MYSIGN_WORK *wk, int seq );
static int Oekaki_EndParentOnly( MYSIGN_WORK *wk, int seq );
static int Oekaki_EndParentOnlyWait( MYSIGN_WORK *wk, int seq );
static void DrawPoint_to_Line( 	GFL_BMPWIN *win, 	const u8 *brush, 	int px, int py, int *sx, int *sy, 	int count, int flag );
static void Stock_OldTouch( TOUCH_INFO *all, OLD_TOUCH_INFO *stock );
static void DrawBrushLine( GFL_BMPWIN *win, TOUCH_INFO *all, OLD_TOUCH_INFO *old, int draw );
static void CursorColTrans(u16 *CursorCol);
static void EndMessagePrint( MYSIGN_WORK *wk, int msgno );
static int EndMessageWait( MYSIGN_WORK *wk );
static int Oekaki_LogoutChildMes( MYSIGN_WORK *wk, int seq );
static int Oekaki_LogoutChildClose( MYSIGN_WORK *wk, int seq );
static int Oekaki_LogoutChildMesWait( MYSIGN_WORK *wk, int seq );
static void EndButtonAppearChange( GFL_CLWK *act[], BOOL flag );
static int Oekaki_ReWriteSelect( MYSIGN_WORK *wk, int seq );
static void TouchYesNoStart( TOUCH_SW_SYS* touch_sub_window_sys );
static void Output_SignData( u8 *des, u8 *src );
static void *PrintCGXOnly( MYSIGN_WORK *wk,GFL_BMPWIN * win, STRBUF *msg, int y);
static void BrushCanvas( MYSIGN_WORK *wk);
static void PlayScruchSe( SCRUCH_INFO *scruchInfo );
static void _BmpWinPrint_Rap(
			GFL_BMPWIN * win, void * src,
			int src_x, int src_y, int src_dx, int src_dy,
			int win_x, int win_y, int win_dx, int win_dy );

void MYSIGN_BmpCutOamSize( GFL_BMPWIN* cp_bmp, int oam_csx, int oam_csy, int bmp_cmx, int bmp_cmy, char* char_buff );


static int (*FuncTable[])(MYSIGN_WORK *wk, int seq)={
	NULL,							// MYSIGN_MODE_INIT  = 0, 
	Oekaki_MainNormal,              // MYSIGN_MODE,
	Oekaki_EndSelectPutString,      // MYSIGN_MODE_END_SELECT,
	Oekaki_EndSelectWait,			// MYSIGN_MODE_END_SELECT_WAIT,
	Oekaki_ReWrite,					// MYSIGN_MODE_REWRITE_
	Oekaki_ReWriteWait,				// MYSIGN_MODE_REWRITE_WAIT
	Oekaki_ReWriteSelect,			// MYSIGN_MODE_REWRITE_SELECT
};

static const GFL_DISP_VRAM vramBank = {
	GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g

	GX_OBJVRAMMODE_CHAR_1D_32K,		// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};


//============================================================================================
//	�v���Z�X�֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param	proc	�v���Z�X�f�[�^
 * @param	seq		�V�[�P���X
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT MySignProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
	MYSIGN_WORK * wk;

	switch(*seq){
	case 0:
		WIPE_SetBrightness( WIPE_DISP_MAIN,WIPE_FADE_BLACK );
		WIPE_SetBrightness( WIPE_DISP_SUB,WIPE_FADE_BLACK );
		
		GFL_DISP_GX_InitVisibleControl();
		GFL_DISP_GXS_InitVisibleControl();

		GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OEKAKI, 0x60000 );

		wk = GFL_PROC_AllocWork( proc, sizeof(MYSIGN_WORK), HEAPID_OEKAKI );
		memset( wk, 0, sizeof(MYSIGN_WORK) );
		GFL_BG_Init( HEAPID_OEKAKI );

		// ������}�l�[�W���[����
		wk->WordSet    = WORDSET_Create( HEAPID_OEKAKI );
		wk->MsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_oekaki_dat, HEAPID_OEKAKI );

//		GFL_UI_KEY_SetRepeatSpeed( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );

		// VRAM �o���N�ݒ�
		GFL_DISP_SetBank( &vramBank );
		
		// BGL���W�X�^�ݒ�
		BgInit();					

///		WIPE_ResetBrightness( WIPE_DISP_MAIN );	<<20060715 del
///		WIPE_ResetBrightness( WIPE_DISP_SUB );	<<20060715 del
		
		// �P�x�ύX�Z�b�g
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 16, 1, HEAPID_OEKAKI );
		

		// �p�����[�^�擾
		{
			SAVE_CONTROL_WORK *sv = SaveControl_GetPointer();
			// �T�C�������o���o�b�t�@�|�C���^�擾
			wk->SignBuf = (u8*)TRCSave_GetSignDataPtr(TRCSave_GetSaveDataPtr(sv));

			// ���R�[�h�f�[�^�|�C���^�擾
			wk->record  = (RECORD *)SaveData_GetRecord(sv);
			wk->config  = SaveData_GetConfig(sv);
		}

		//BG�O���t�B�b�N�Z�b�g
		BgGraphicSet( wk );

		// VBlank�֐��Z�b�g
		wk->vblankFunc = GFUser_VIntr_CreateTCB( VBlankFunc , wk , 1 );
		wk->printTcblSys = GFL_TCBL_Init( HEAPID_OEKAKI , HEAPID_OEKAKI , 3 , 0 );

		// �{�^���p�t�H���g��ǂݍ���
		wk->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_OEKAKI );

		// ���[�N������
		InitWork( wk );

		// CellActor�V�X�e��������
		InitCellActor(wk);
		
		// CellActro�\���o�^
		SetCellActor(wk);

		// BMPWIN�o�^�E�`��
		BmpWinInit(wk);

		// �T�E���h�f�[�^���[�h(�T�C������)(BGM���p��)
#if SIGN_USE_SND
		Snd_DataSetByScene( SND_SCENE_SUB_TRCARD, 0, 0 );
#endif
	
		// ��ʏo�͂��㉺����ւ���
		GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
		
		//�T���v�����O�J�n
		GFL_UI_TP_AutoStartNoBuff();

		(*seq)++;
		break;
	case 1:
		wk = mywk;
		(*seq) = SEQ_IN;
		return GFL_PROC_RES_FINISH;
		break;
	}
	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F���C��
 *
 * @param	proc	�v���Z�X�f�[�^
 * @param	seq		�V�[�P���X
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------

GFL_PROC_RESULT MySignProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
	MYSIGN_WORK * wk  = mywk;

	switch( *seq ){
	case SEQ_IN:
		if( WIPE_SYS_EndCheck() ){
			// ���C�v�����҂�
			*seq = SEQ_MAIN;
		}
		break;

	case SEQ_MAIN:
		// �J�[�\���ړ�

		// �V�[�P���X���̓���
		if(FuncTable[wk->seq]!=NULL){
			*seq = (*FuncTable[wk->seq])( wk, *seq );
		}

		CursorColTrans(&wk->CursorPal);

		break;

	case SEQ_OUT:
		if( WIPE_SYS_EndCheck() ){
			return GFL_PROC_RES_FINISH;
		}
		break;
	}
	GFL_CLACT_SYS_Main();			// �Z���A�N�^�[�풓�֐�

	if( wk->printTcblSys != NULL )
		GFL_TCBL_Main( wk->printTcblSys );

	return GFL_PROC_RES_CONTINUE;
}

#define DEFAULT_NAME_MAX		18

// �_�C���E�p�[���ŕς��񂾂낤
#define MALE_NAME_START			0
#define FEMALE_NAME_START		18


//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I��
 *
 * @param	proc	�v���Z�X�f�[�^
 * @param	seq		�V�[�P���X
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT MySignProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
	MYSIGN_WORK  *wk    = mywk;
	int i;

	// �T�C�����Z�[�u�f�[�^�ɏ����o��
	Output_SignData( wk->SignBuf, GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(wk->OekakiBoard) ) );

	//�T���v�����O�I��
	GFL_UI_TP_AutoStop();

	// Vblank���Ԓ���BG�]���I��
	GFL_TCB_DeleteTask( wk->vblankFunc );
	GFL_TCBL_Exit( wk->printTcblSys );

	// �Z���A�N�^�[���\�[�X���
/*
	// �L�����]���}�l�[�W���[�j��
	CLACT_U_CharManagerDelete(wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES]);
	CLACT_U_CharManagerDelete(wk->resObjTbl[SUB_LCD][CLACT_U_CHAR_RES]);

	// �p���b�g�]���}�l�[�W���[�j��
	CLACT_U_PlttManagerDelete(wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES]);
	CLACT_U_PlttManagerDelete(wk->resObjTbl[SUB_LCD][CLACT_U_PLTT_RES]);
		
	// �L�����E�p���b�g�E�Z���E�Z���A�j���̃��\�[�X�}�l�[�W���[�j��
	for(i=0;i<CLACT_RESOURCE_NUM;i++){
		CLACT_U_ResManagerDelete(wk->resMan[i]);
	}
	// �Z���A�N�^�[�Z�b�g�j��
	CLACT_DestSet(wk->clactSet);

	//OAM�����_���[�j��
	REND_OAM_Delete();
*/
	
	// �Z���A�N�^�[�Z�b�g�j��
	GFL_CLACT_UNIT_Delete(wk->cellUnit);

	//OAM�����_���[�j��
	GFL_CLACT_SYS_Delete();


	// ���\�[�X���
//	DeleteCharManager();
//	DeletePlttManager();

	// BMP�E�B���h�E�J��
	BmpWinDelete( wk );

	// �{�^���p�t�H���g���
	GFL_FONT_Delete( wk->fontHandle );

	// BGL�폜
	BgExit();

	// ���b�Z�[�W�}�l�[�W���[�E���[�h�Z�b�g�}�l�[�W���[���
	GFL_MSG_Delete( wk->MsgManager );
	WORDSET_Delete( wk->WordSet );

	// ���[�N���
	FreeWork( wk );

	GFL_PROC_FreeWork( proc );				// PROC���[�N�J��

	// ����ւ���Ă����㉺��ʏo�͂����ɖ߂�
	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

	GFL_HEAP_DeleteHeap( HEAPID_OEKAKI );

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * VBlank�֐�
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB *tcb , void * work )
{
	// �Z���A�N�^�[
	GFL_CLACT_SYS_VBlankFunc();

	GFL_BG_VBlankFunc();
	
//	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}


//--------------------------------------------------------------------------------------------
/**
 * BG�ݒ�
 *
 * @param	ini		BGL�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgInit( void )
{
	// BG SYSTEM
	{	
		GFL_BG_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &BGsys_data );
	}

	// ���C����ʕ�����0
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, 0x8000,GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearFrame( GFL_BG_FRAME0_M );
		GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );


	}

	// ���C����ʃ��N�K�L��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x6000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearFrame( GFL_BG_FRAME1_M );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );

	}

	// ���C����ʔw�i
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
//		GFL_BG_ClearFrame( GFL_BG_FRAME2_M );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
	}

	// �T�u��ʃe�L�X�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000,0x8000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearFrame( GFL_BG_FRAME0_S );
		GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
	}

	// �T�u��ʔw�i��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x7000,GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
	}

	GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0, 1, 0 );
	GFL_BG_FillCharacter( GFL_BG_FRAME0_S, 0, 1, 0 );

}


#define TALK_MESSAGE_BUF_NUM	( 40*2 )
#define END_MESSAGE_BUF_NUM		( 10*2 )
#define  TITLE_MESSAGE_BUF_NUM	( 20*2 )

//------------------------------------------------------------------
/**
 * ���G�������[�N������
 *
 * @param   wk		MYSIGN_WORK*
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void InitWork( MYSIGN_WORK *wk )
{
	int i;
/*
	for(i=0;i<OEKAKI_MEMBER_MAX;i++){
		wk->TrainerName[i] = GFL_STR_CreateBuffer( PERSON_NAME_SIZE+EOM_SIZE, HEAPID_OEKAKI );
		wk->AllTouchResult[i].size = 0;
		wk->OldTouch[i].size = 0;
	}
*/
	wk->OldTouch.size = 0;

	// �u��߂�v������o�b�t�@�쐬
	wk->EndString   = GFL_STR_CreateBuffer( END_MESSAGE_BUF_NUM,   HEAPID_OEKAKI );
	wk->TitleString = GFL_STR_CreateBuffer( TITLE_MESSAGE_BUF_NUM, HEAPID_OEKAKI );
	wk->TalkString  = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM,  HEAPID_OEKAKI );

	// �u���V������
	wk->brush_color  = 0;
	

	wk->seq = MYSIGN_MODE;
	
	// �u��߂�v�擾
	GFL_MSG_GetString( wk->MsgManager, msg_oekaki_13, wk->EndString );

	// �g���[�i�[�T�C����������
	GFL_MSG_GetString( wk->MsgManager, msg_oekaki_10, wk->TitleString );

	// ����ʃE�C���h�E�V�X�e��������
	wk->TouchSubWindowSys = TOUCH_SW_AllocWork( HEAPID_OEKAKI );

	MI_CpuClearFast( &wk->scruchInfo, sizeof(SCRUCH_INFO));
}

//------------------------------------------------------------------
/**
 * $brief   ���[�N���
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void FreeWork( MYSIGN_WORK *wk )
{
	int i;

	TOUCH_SW_FreeWork( wk->TouchSubWindowSys );

/*
	for(i=0;i<OEKAKI_MEMBER_MAX;i++){
		STRBUF_Delete( wk->TrainerName[i] );
	}
*/
	GFL_STR_DeleteBuffer( wk->TalkString ); 
	GFL_STR_DeleteBuffer( wk->TitleString  ); 
	GFL_STR_DeleteBuffer( wk->EndString  ); 

}

//--------------------------------------------------------------------------------------------
/**
 * BG���
 *
 * @param	ini		BGL�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( void )
{

	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

	GFL_BG_Exit();
}


//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param	wk		�|�P�������X�g��ʂ̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( MYSIGN_WORK * wk )
{

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette( ARCID_MYSIGN , NARC_mysign_mysign_m_NCLR, PALTYPE_MAIN_BG, 0, 16*2*3,  HEAPID_OEKAKI);
	GFL_ARC_UTIL_TransVramPalette( ARCID_MYSIGN , NARC_mysign_mysign_s_NCLR, PALTYPE_SUB_BG,  0, 16*2*2,  HEAPID_OEKAKI);
	
	// ��b�t�H���g�p���b�g�]��
//	TalkFontPaletteLoad( PALTYPE_MAIN_BG, 13*0x20, HEAPID_OEKAKI );
//	TalkFontPaletteLoad( PALTYPE_SUB_BG,  13*0x20, HEAPID_OEKAKI );
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , 13*0x20, 16*2, HEAPID_OEKAKI );


	// �O�L�����ڂ��N���A����
	GFL_BG_FillCharacter( GFL_BG_FRAME1_M, 0, 1, 0);

	// ���C�����BG2�L�����]��
	GFL_ARC_UTIL_TransVramBgCharacter( ARCID_MYSIGN, NARC_mysign_mysign_m_NCGR, GFL_BG_FRAME2_M, 0, 32*8*0x20, 0, HEAPID_OEKAKI);

	// ���C�����BG2�X�N���[���]��
	GFL_ARC_UTIL_TransVramScreen( ARCID_MYSIGN, NARC_mysign_mysign_m_NSCR, GFL_BG_FRAME2_M, 0, 32*24*2, 0, HEAPID_OEKAKI);



	// �T�u���BG1�L�����]��
	GFL_ARC_UTIL_TransVramBgCharacter( ARCID_MYSIGN, NARC_mysign_mysign_s_NCGR, GFL_BG_FRAME1_S, 0, 32*8*0x20, 0, HEAPID_OEKAKI);

	// �T�u���BG1�X�N���[���]��
	GFL_ARC_UTIL_TransVramScreen(   ARCID_MYSIGN, NARC_mysign_mysign_s_NSCR, GFL_BG_FRAME1_S, 0, 32*24*2, 0, HEAPID_OEKAKI);

	// ���C����ʉ�b�E�C���h�E�O���t�B�b�N�]��
	BmpWinFrame_GraphicSet(
				GFL_BG_FRAME0_M, 1, 12,  CONFIG_GetWindowType(wk->config), HEAPID_OEKAKI );

	BmpWinFrame_GraphicSet(
				GFL_BG_FRAME0_M, 1+TALK_WIN_CGX_SIZ, 11, 0, HEAPID_OEKAKI );

}


//------------------------------------------------------------------
/**
 * ���[�_�[��ʗp�Z���A�N�^�[������
 *
 * @param   wk		���[�_�[�\���̂̃|�C���^
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void InitCellActor(MYSIGN_WORK *wk)
{
	int i;
	ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_MYSIGN , HEAPID_OEKAKI );

	GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN , &vramBank, HEAPID_OEKAKI );
	
	// �Z���A�N�^�[������
	wk->cellUnit = GFL_CLACT_UNIT_Create( 50, 0, HEAPID_OEKAKI );
	
//	CLACT_U_SetSubSurfaceMatrix( &wk->renddata, 0, NAMEIN_SUB_ACTOR_DISTANCE );

	
	//���\�[�X�}�l�[�W���[������
//	for(i=0;i<CLACT_RESOURCE_NUM;i++){		//���\�[�X�}�l�[�W���[�쐬
//		wk->resMan[i] = CLACT_U_ResManagerInit(2, i, HEAPID_OEKAKI);
//	}


	//---------���ʗp-------------------

	//chara�ǂݍ���
//	wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar_ArcHandle(wk->resMan[CLACT_U_CHAR_RES], 
//							p_handle, NARC_mysign_obj_lz_ncgr, 1, 0, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_OEKAKI);
	wk->resObjTbl[MYSIGN_RES_MAIN_CHAR] = 
			GFL_CLGRP_CGR_Register( arcHandle , NARC_mysign_mysign_m_obj_NCGR , 0 , CLSYS_DRAW_MAIN , HEAPID_OEKAKI );

	//pal�ǂݍ���
	//wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt_ArcHandle(wk->resMan[CLACT_U_PLTT_RES],
	//						p_handle, NARC_mysign_mysign_m_obj_nclr, 0, 0, NNS_G2D_VRAM_TYPE_2DMAIN, 3, HEAPID_OEKAKI);
	wk->resObjTbl[MYSIGN_RES_MAIN_PLTT] = 
			GFL_CLGRP_PLTT_Register( arcHandle , NARC_mysign_mysign_m_obj_NCLR, CLSYS_DRAW_MAIN , 0 , HEAPID_OEKAKI );
	
	//cell�ǂݍ���
//	wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(wk->resMan[CLACT_U_CELL_RES],
//							p_handle, NARC_mysign_obj_lz_ncer, 1, 0, CLACT_U_CELL_RES,HEAPID_OEKAKI);
	//�����֐���anim�ǂݍ���
//	wk->resObjTbl[MAIN_LCD][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(wk->resMan[CLACT_U_CELLANM_RES],
//							p_handle, NARC_mysign_obj_lz_nanr, 1, 0, CLACT_U_CELLANM_RES,HEAPID_OEKAKI);
	wk->resObjTbl[MYSIGN_RES_MAIN_ANIM] = 
			GFL_CLGRP_CELLANIM_Register( arcHandle , NARC_mysign_mysign_m_obj_NCER , NARC_mysign_mysign_m_obj_NANR , HEAPID_OEKAKI );


	//---------����ʗp-------------------



	//chara�ǂݍ���
//	wk->resObjTbl[SUB_LCD][CLACT_U_CHAR_RES] = CLACT_U_ResManagerResAddArcChar_ArcHandle(wk->resMan[CLACT_U_CHAR_RES], 
//							p_handle, NARC_mysign_obj_lz_ncgr, 1, 1, NNS_G2D_VRAM_TYPE_2DSUB, HEAPID_OEKAKI);
	wk->resObjTbl[MYSIGN_RES_SUB_CHAR] = 
			GFL_CLGRP_CGR_Register( arcHandle , NARC_mysign_mysign_m_obj_NCGR , 0 , CLSYS_DRAW_SUB , HEAPID_OEKAKI );

	//pal�ǂݍ���
//	wk->resObjTbl[SUB_LCD][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt_ArcHandle(wk->resMan[CLACT_U_PLTT_RES],
//							p_handle, NARC_mysign_mysign_m_obj_nclr, 0, 1, NNS_G2D_VRAM_TYPE_2DSUB, 3, HEAPID_OEKAKI);
	wk->resObjTbl[MYSIGN_RES_SUB_PLTT] = 
			GFL_CLGRP_PLTT_Register( arcHandle , NARC_mysign_mysign_m_obj_NCLR, CLSYS_DRAW_SUB , 0 , HEAPID_OEKAKI );

	//cell�ǂݍ���
//	wk->resObjTbl[SUB_LCD][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(wk->resMan[CLACT_U_CELL_RES],
//							p_handle, NARC_mysign_obj_lz_ncer, 1, 1, CLACT_U_CELL_RES,HEAPID_OEKAKI);
	//�����֐���anim�ǂݍ���
//	wk->resObjTbl[SUB_LCD][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(wk->resMan[CLACT_U_CELLANM_RES],
//							p_handle, NARC_mysign_obj_lz_nanr, 1, 1, CLACT_U_CELLANM_RES,HEAPID_OEKAKI);
	wk->resObjTbl[MYSIGN_RES_SUB_ANIM] = 
			GFL_CLGRP_CELLANIM_Register( arcHandle , NARC_mysign_mysign_m_obj_NCER , NARC_mysign_mysign_m_obj_NANR , HEAPID_OEKAKI );

	// ���\�[�X�}�l�[�W���[����]��

	// Chara�]��
//	CLACT_U_CharManagerSet( wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] );
//	CLACT_U_CharManagerSet( wk->resObjTbl[SUB_LCD][CLACT_U_CHAR_RES] );

	// �p���b�g�]��
//	CLACT_U_PlttManagerSet( wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] );
//	CLACT_U_PlttManagerSet( wk->resObjTbl[SUB_LCD][CLACT_U_PLTT_RES] );

	GFL_ARC_CloseDataHandle( arcHandle );
}

#define TRAINER_NAME_POS_X		( 24 )
#define TRAINER_NAME_POS_Y		( 32 )
#define TRAINER_NAME_POS_SPAN	( 32 )

static const u16 pal_button_oam_table[BUTTON_NUM][3]={
	{48+12*8*2-2*8, 171+4, 0},
//	{48       , 171+4, 5},
//	{48+12*8*1, 171+4, 7},
};

//------------------------------------------------------------------
/**
 * �Z���A�N�^�[�o�^
 *
 * @param   wk			MYSIGN_WORK*
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SetCellActor(MYSIGN_WORK *wk)
{
	int i;
	// �Z���A�N�^�[�w�b�_�쐬
/*
	CLACT_U_MakeHeader(&wk->clActHeader_m, 0, 0, 0, 0, CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
	0, 0,
	wk->resMan[CLACT_U_CHAR_RES],
	wk->resMan[CLACT_U_PLTT_RES],
	wk->resMan[CLACT_U_CELL_RES],
	wk->resMan[CLACT_U_CELLANM_RES],
	NULL,NULL);

	CLACT_U_MakeHeader(&wk->clActHeader_s, 1, 1, 1, 1, CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
	0, 0,
	wk->resMan[CLACT_U_CHAR_RES],
	wk->resMan[CLACT_U_PLTT_RES],
	wk->resMan[CLACT_U_CELL_RES],
	wk->resMan[CLACT_U_CELLANM_RES],
	NULL,NULL);
*/
	{
		//�o�^���i�[
		GFL_CLWK_DATA	addData;
		//�Z���A�N�^�[�\���J�n

		// ���C����ʃp���b�g�E�u��߂�v�{�^���̓o�^
		for(i=0;i<BUTTON_NUM;i++){
			addData.pos_x = pal_button_oam_table[i][0];
			addData.pos_y = pal_button_oam_table[i][1];
			addData.anmseq = pal_button_oam_table[i][2];
			wk->ButtonActWork[i] = GFL_CLACT_WK_Create( wk->cellUnit ,
						wk->resObjTbl[MYSIGN_RES_MAIN_CHAR] ,
						wk->resObjTbl[MYSIGN_RES_MAIN_PLTT] ,
						wk->resObjTbl[MYSIGN_RES_MAIN_ANIM] ,
						&addData , CLSYS_DEFREND_MAIN , HEAPID_OEKAKI );
//			CLACT_SetAnmFlag(wk->ButtonActWork[i],1);
			GFL_CLACT_WK_SetAutoAnmSpeed( wk->ButtonActWork[i], FX32_ONE );
			GFL_CLACT_WK_SetAutoAnmFlag( wk->ButtonActWork[i], TRUE );
			GFL_CLACT_WK_SetDrawEnable( wk->ButtonActWork[i], TRUE );
//			CLACT_PaletteNoChg( wk->ButtonActWork[i], 0 );
			if(i==0){
				GFL_CLACT_WK_SetBgPri( wk->ButtonActWork[i], 2 );
			}

//			CLACT_SetDrawFlag( wk->ButtonActWork[i], 0 );
		}
	}	
	GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );	//���C�����OBJ�ʂn�m
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );	//�T�u���OBJ�ʂn�m
	
}


//------------------------------------------------------------------
/**
 * @brief   �n�`�l�p��BmpWin�̃������ɂ̂ݕ������`�悷��
 *
 * @param   win			BMPWIN
 * @param   msg			STRBUF
 * @param   y			BMP���̕`��J�n�x���W
 * @param   fnt_index	�t�H���I�w��
 * @param   col			�J���[�w��
 *
 * @retval  void *		�`�悵�ă������̃g�b�v
 */
//------------------------------------------------------------------
static void* PrintCGXOnly( MYSIGN_WORK *wk,GFL_BMPWIN * win, STRBUF *msg, int y)
{
	int x,length;

	length = PRINTSYS_GetStrWidth(msg,wk->fontHandle,0);
	x      = (GFL_BMPWIN_GetScreenSizeX(win)*8-length)/2;

	//GF_STR_PrintColor(	win, fnt_index, msg, x, y, MSG_NO_PUT, col,NULL);
	PRINTSYS_Print( GFL_BMPWIN_GetBmp(win) 
				, x, 0 , msg , wk->fontHandle );

	return GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp( win ));
}


// ���������{�[�hBMP�i����ʁj
#define OEKAKI_BOARD_POSX	 ( 4 )
#define OEKAKI_BOARD_POSY	 ( 9 )


// ���O�\��BMP�i���ʁj
#define OEKAKI_NAME_BMP_W	 ( 10 )
#define OEKAKI_NAME_BMP_H	 (  2 )
#define OEKAKI_NAME_BMP_SIZE (OEKAKI_NAME_BMP_W * OEKAKI_NAME_BMP_H)

#define END_BMP_OFFSET		 (1+OEKAKI_BOARD_W*OEKAKI_BOARD_H)
#define TITLE_BMP_OFFSET     (END_BMP_OFFSET + OEKAKI_END_BMP_W*OEKAKI_END_BMP_H)

// �u��߂�v������BMP�i����ʁj
#define OEKAKI_END_BMP_X	( 26 )
#define OEKAKI_END_BMP_Y	( 21 )
#define OEKAKI_END_BMP_W	( 8  )
#define OEKAKI_END_BMP_H	( 2  )

// �u�g���[�i�[�T�C�����������I�v
#define TITLE_BMP_X			(  2 )
#define TITLE_BMP_Y			(  2 )
#define TITLE_BMP_W			( 28 )
#define TITLE_BMP_H			(  2 )

// ��b�E�C���h�E�\���ʒu��`
#define OEKAKI_TALK_X		(  2 )
#define OEKAKI_TALK_Y		(  1 )

#define OBJ_VRAM_WORD_OFFSET		( 0 )
#define OBJ_VRAM_WORD_TRANS_SIZE	( 0x20*4*2 )
//------------------------------------------------------------------
/**
 * BMPWIN�����i�����p�l���Ƀt�H���g�`��j
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinInit( MYSIGN_WORK *wk )
{
	GFL_BMPWIN_Init( HEAPID_OEKAKI );
	// ---------- ���C����� ------------------

	// BG0��BMP�i��b�E�C���h�E�j�E�C���h�E�m��
	wk->MsgWin = GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
		OEKAKI_TALK_X, OEKAKI_TALK_Y, 27, 4, 13, GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0 );
	GFL_BMPWIN_MakeScreen(wk->MsgWin);
	GFL_BMPWIN_TransVramCharacter(wk->MsgWin);

	// BG1�ʗpBMP�i���G�����摜�j�E�C���h�E�m��
	wk->OekakiBoard = GFL_BMPWIN_Create( GFL_BG_FRAME1_M,
		OEKAKI_BOARD_POSX, OEKAKI_BOARD_POSY, OEKAKI_BOARD_W, OEKAKI_BOARD_H, 1, GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->OekakiBoard), 0 );
	GFL_BMPWIN_MakeScreen(wk->OekakiBoard);
	GFL_BMPWIN_TransVramCharacter(wk->OekakiBoard);

	// BG1��BMP�i��߂�j�E�C���h�E�m�ہE�`��

	wk->EndWin = GFL_BMPWIN_Create( GFL_BG_FRAME1_M,
		OEKAKI_END_BMP_X, OEKAKI_END_BMP_Y, OEKAKI_END_BMP_W, OEKAKI_END_BMP_H, 2, GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->EndWin), 0 );

//	GF_STR_PrintColor( &wk->EndWin, FONT_BUTTON, wk->EndString, 0, 0, MSG_ALLPUT, GF_PRINTCOLOR_MAKE(0x1,0x2,0x3),NULL);
	{
		void *objcharaadr;
		int  i;
		objcharaadr = PrintCGXOnly(wk,wk->EndWin, wk->EndString, 1);

		// BMP�����ŕ`�悵���o�b�t�@��OBJ�ɓ]�����Ĕ��f������
		DC_FlushRange(objcharaadr,0x20*8*2);	
		for(i=0;i<2;i++){						
			MYSIGN_BmpCutOamSize( wk->EndWin, 4, 2, 4*i, 0,  (char*)wk->TransWork);
			DC_FlushRange(wk->TransWork,OBJ_VRAM_WORD_TRANS_SIZE);	
			GX_LoadOBJ(wk->TransWork, OBJ_VRAM_WORD_OFFSET+i*OBJ_VRAM_WORD_TRANS_SIZE, OBJ_VRAM_WORD_TRANS_SIZE);
		}
	}


	// BG1��BMP�i�g���[�i�[�T�C�����������I�j�E�C���h�E�m�ہE�`��
	wk->TitleWin= GFL_BMPWIN_Create(GFL_BG_FRAME1_M,
	TITLE_BMP_X, TITLE_BMP_Y, TITLE_BMP_W, TITLE_BMP_H, 13,  GFL_BMP_CHRAREA_GET_B );

	{
		const int length = PRINTSYS_GetStrWidth(wk->TitleString,wk->fontHandle,0);
		const int x      = (GFL_BMPWIN_GetScreenSizeX(wk->TitleWin)*8-length)/2;

		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->TitleWin), 0 );
		//GF_STR_PrintColor( wk->TitleWin, FONT_TALK, wk->TitleString, x, 0, MSG_ALLPUT, GF_PRINTCOLOR_MAKE(0x1,0x2,0x0),NULL);
		PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->TitleWin) , x, 0 , wk->TitleString ,  wk->fontHandle );
		GFL_BMPWIN_MakeScreen(wk->TitleWin);
		GFL_BMPWIN_TransVramCharacter(wk->TitleWin);
	}
/*
	// ----------- �T�u��ʖ��O�\��BMP�m�� ------------------
	{
		int i;
		for(i=0;i<OEKAKI_MEMBER_MAX;i++){
			&wk->TrainerNameWin[i] GFL_BMPWIN_Create( GFL_BG_FRAME0_S,	
				TRAINER_NAME_POS_X/8+2, TRAINER_NAME_POS_Y/8+i*4-1, 10, 2, 13,  1+i*OEKAKI_NAME_BMP_SIZE);
			GFL_BG_BmpWinDataFill( &wk->TrainerNameWin[i], 0 );
		}
	}
*/
	GFL_BG_LoadScreenV_Req(GFL_BG_FRAME0_M);
	GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_M);

}	

// �͂��E������BMP�i����ʁj
#define YESNO_WIN_FRAME_CHAR	( 1 + TALK_WIN_CGX_SIZ )
#define YESNO_CHARA_OFFSET	(1 + TALK_WIN_CGX_SIZ + MENU_WIN_CGX_SIZ )
#define YESNO_WINDOW_X		( 22   )
#define YESNO_WINDOW_Y1		(  7   )
#define YESNO_WINDOW_Y2		(  7+6 )
#define YESNO_CHARA_W		( 8 )
#define YESNO_CHARA_H		( 4 )


//------------------------------------------------------------------
/**
 * $brief   �m�ۂ���BMPWIN�����
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinDelete( MYSIGN_WORK *wk )
{
	int i;
	
//	for(i=0;i<OEKAKI_MEMBER_MAX;i++){
//		GFL_BMPWIN_Delete( wk->TrainerNameWin[i] );
//	}
	GFL_BMPWIN_Delete( wk->TitleWin );
	GFL_BMPWIN_Delete( wk->EndWin );
	GFL_BMPWIN_Delete( wk->OekakiBoard );
	GFL_BMPWIN_Delete( wk->MsgWin );

	GFL_BMPWIN_Exit();
}


//------------------------------------------------------------------
/**
 * �J�[�\���ړ�����
 *
 * @param   wk		MYSIGN_WORK*
 *
 * @retval  none	
 */
//------------------------------------------------------------------
static void ControlCursor(MYSIGN_WORK *wk)
{
	int move  = 0;
	int arrow = 0;


	// �ړ�������������ύX
	if(move){
		CursorAppearUpDate(wk,arrow);		// �ړ��̌��ʃJ�[�\���̌`�󂪂ǂ̂悤�ɕς�邩�����肷��
	}

}


//------------------------------------------------------------------
/**
 * �ړ��̌��ʃJ�[�\���̌`�󂪂ǂ̂悤�ɕς�邩�����肷��
 * �����̏�ł���΁A�����`�̃J�[�\���A�{�^���̏�ł���Β����`�̃J�[�\��
 * �����̍X�V���s���邱�ƂŁA�T�u��ʂ̒��̃t�H���g�̍X�V���s����
 *
 * @param   wk			MYSIGN_WORK*
 * @param   arrow		�ړ�����
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void CursorAppearUpDate(MYSIGN_WORK *wk, int arrow)
{
	VecFx32 tmpVex;

//	tmpVex.x = FX32_ONE*(MAIN_CURSOR_POSX+wk->Cursor.x*12);
//	tmpVex.y = FX32_ONE*(MAIN_CURSOR_POSY+(wk->Cursor.y-1)*16);
//	CLACT_SetMatrix(wk->clActWork[CLACT_CURSOR_NUM], &tmpVex);


}

#define PAL_BUTTON_X	(  0  )
#define PAL_BUTTON_Y	( 150 )
#define PAL_BUTTON_W	( 12*8 )
#define PAL_BUTTON_H	( 5*8 )
#define PAL_BUTTON_RECT ( 12*8 -1 )
#define END_BUTTON_RECT ( 8*8 -1 )

#define PAL_BUTTON0_X	( PAL_BUTTON_X+PAL_BUTTON_W*0 )
#define PAL_BUTTON1_X	( PAL_BUTTON_X+PAL_BUTTON_W*1 )
#define END_BUTTON_X	( PAL_BUTTON_X+PAL_BUTTON_W*2 )


static const GFL_UI_TP_HITTBL sub_button_hittbl[] =
{
	{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,END_BUTTON_X,END_BUTTON_X+END_BUTTON_RECT},
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
	{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON0_X,PAL_BUTTON0_X+PAL_BUTTON_RECT},
	{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON1_X,PAL_BUTTON1_X+PAL_BUTTON_RECT},
};

#define DRAW_AREA_X	(  8*OEKAKI_BOARD_POSX  )
#define DRAW_AREA_Y	(  8*OEKAKI_BOARD_POSY  )
#define DRAW_AREA_W	( OEKAKI_BOARD_W*8 )
#define DRAW_AREA_H	( OEKAKI_BOARD_H*8)

static const GFL_UI_TP_HITTBL sub_canvas_touchtbl[] =
{
	{DRAW_AREA_Y,DRAW_AREA_Y+DRAW_AREA_H,DRAW_AREA_X,DRAW_AREA_X+DRAW_AREA_W},
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
};


// ����ʗp�͂��E�����������̃^�b�`����e�[�u��
static const GFL_UI_TP_HITTBL end_button_touchtbl[] =
{
	{ YESNO_WINDOW_Y1*8, (YESNO_WINDOW_Y1+YESNO_CHARA_H)*8, YESNO_WINDOW_X*8, (YESNO_WINDOW_X+YESNO_CHARA_W)*8,},
	{ YESNO_WINDOW_Y2*8, YESNO_WINDOW_Y2*8+YESNO_CHARA_H*8, YESNO_WINDOW_X*8, (YESNO_WINDOW_X+YESNO_CHARA_W)*8,},
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
};



//------------------------------------------------------------------
/**
 * �^�b�`�p�l���ŉ������{�^�����ւ���
 *
 * @param   wk		MYSIGN_WORK�̃|�C���^
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void NormalTouchFunc(MYSIGN_WORK *wk)
{
	int button=-1,touch;

	// ���͏���

	// �u���V�؂�ւ�
	button=GFL_UI_TP_HitTrg( sub_button_hittbl );
	if( button != GFL_UI_TP_HIT_NONE ){
		if(button==0){
			// �u��߂�v����������E�C���h�E�`��J�n
			if(wk->seq==MYSIGN_MODE){
				EndMessagePrint( wk, msg_oekaki_11 );
				wk->seq = MYSIGN_MODE_END_SELECT;
				EndButtonAppearChange( wk->ButtonActWork, TRUE );
#if SIGN_USE_SND
				Snd_SePlay( SEQ_SE_DP_DECIDE );
#endif
			}
		}else{
			// �I���{�^������Ȃ��ꍇ�́A�F�ύX
		}
	}

	// �J�[�\���ʒu�ύX
	touch = GFL_UI_TP_HitTrg( sub_canvas_touchtbl );

//	CLACT_SetDrawFlag( wk->MainActWork[0], 0 );
	if( touch != GFL_UI_TP_HIT_NONE )
	{
		// �����̃J�[�\���̓^�b�`�p�l���̍��W�𔽉f������
		BrushCanvas(wk);
	}

	// �T���v�����O�����擾���Ċi�[
	{
		TP_ONE_DATA	tpData;
		int i;
		if(GFL_UI_TP_AutoSamplingMain( &tpData, TP_BUFFERING_JUST, 1 )==TP_OK){
			for(i=0;i<tpData.Size;i++){
				wk->MyTouchResult.x[i] = tpData.TPDataTbl[i].x;
				wk->MyTouchResult.y[i] = tpData.TPDataTbl[i].y;
			}
			wk->MyTouchResult.size  = tpData.Size;
			wk->MyTouchResult.brush = wk->brush_color;
		}
	}


	// �����ڂ̏���
//	button=GF_TP_RectHitCont( sub_button_hittbl );


}

//------------------------------------------------------------------
/**
 * $brief   ��߂�{�^���̃I���E�I�t
 *
 * @param   act		�A�N�^�[�̃|�C���^
 * @param   flag		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void EndButtonAppearChange( GFL_CLWK *act[], BOOL flag )
{
	if(flag==TRUE){
		GFL_CLACT_WK_SetAnmSeq( act[0], pal_button_oam_table[0][2]+1 );
	}else{
		GFL_CLACT_WK_SetAnmSeq( act[0], pal_button_oam_table[0][2] );
	}
}

//------------------------------------------------------------------
/**
 * $brief   ���G�����{�[�h�ʏ폈��
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Oekaki_MainNormal( MYSIGN_WORK *wk, int seq )
{
	NormalTouchFunc(wk);			//  �^�b�`�p�l������

	DrawBrushLine( wk->OekakiBoard, &wk->MyTouchResult, &wk->OldTouch, 1 );

	return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �I���I�������̋��ʏ����֐�(���ŕ`�擙�j
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void EndSequenceCommonFunc( MYSIGN_WORK *wk )
{
	ControlCursor(wk);		//	DebugOBJPOSGet(wk);
	DrawBrushLine( wk->OekakiBoard, &wk->MyTouchResult, &wk->OldTouch, 0 );
	
}

//------------------------------------------------------------------
/**
 * $brief   YESNO�E�C���h�E�V�X�e���X�^�[�g
 *
 * @param   bgl		
 * @param   touch_sub_window_sys		
 *
 * @retval  static		
 */
//------------------------------------------------------------------
static void TouchYesNoStart( TOUCH_SW_SYS* touch_sub_window_sys )
{
	TOUCH_SW_PARAM param;

	// YES NO �E�B���h�E�{�^���̕\��
	param.bg_frame	= GFL_BG_FRAME0_M;
	param.char_offs	= YESNO_CHARA_OFFSET+YESNO_CHARA_W*YESNO_CHARA_H;
	param.pltt_offs = 8;
	param.x			= 25;
	param.y			= 6;

//	param.kt_st = TOUCH_SW_GetKTStatus(wk->TouchSubWindowSys);
	param.key_pos = 0;
	param.type = TOUCH_SW_TYPE_S;

	TOUCH_SW_Init( touch_sub_window_sys, &param );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );

}


//------------------------------------------------------------------
/**
 * $brief   ���G�����{�[�h�u��߂�v��I��������
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectPutString( MYSIGN_WORK *wk, int seq )
{
	if( EndMessageWait( wk ) ){

		// YES NO �E�B���h�E�{�^���̕\��
		TouchYesNoStart( wk->TouchSubWindowSys );
	
		wk->seq = MYSIGN_MODE_END_SELECT_WAIT;
	}
	EndSequenceCommonFunc( wk );		//�I���I�����̋��ʏ���
	return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �u��߂܂����H�v�͂��E�������I��҂�
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectWait( MYSIGN_WORK *wk, int seq )
{
	int result;
	result = TOUCH_SW_Main( wk->TouchSubWindowSys );
	switch(result){				//��߂܂����H
	case TOUCH_SW_RET_YES:						//�͂�
		// �X�R�A���Z
		RECORD_Score_Add( wk->record, SCORE_ID_WRITE_SIGN );
		RECORD_Inc( wk->record, RECID_MYSIGN_WRITE );
		
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );
		PRINTSYS_PrintStreamDelete( wk->printHandle );
		TOUCH_SW_Reset( wk->TouchSubWindowSys );
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, 16, 1, HEAPID_OEKAKI );
		return SEQ_OUT;
		break;
	case TOUCH_SW_RET_NO:						//������
		wk->seq = MYSIGN_MODE_REWRITE;
		EndButtonAppearChange( wk->ButtonActWork, FALSE );
//		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );
		PRINTSYS_PrintStreamDelete( wk->printHandle );
		TOUCH_SW_Reset( wk->TouchSubWindowSys );
		break;
	}

	// �둗�M��h��
	wk->MyTouchResult.size = 0;

	EndSequenceCommonFunc( wk );		//�I���I�����̋��ʏ���

	return seq;
}



//------------------------------------------------------------------
/**
 * $brief   
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Oekaki_ReWriteSelect( MYSIGN_WORK *wk, int seq )
{
	int result = TOUCH_SW_Main( wk->TouchSubWindowSys );
	switch(result){				//�����Ȃ����܂����H
	case TOUCH_SW_RET_YES:						//�͂�
		
		wk->seq = MYSIGN_MODE;
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );
		PRINTSYS_PrintStreamDelete( wk->printHandle );
		TOUCH_SW_Reset( wk->TouchSubWindowSys );

		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->OekakiBoard) , 0 );
		GFL_BMPWIN_TransVramCharacter( wk->OekakiBoard );

		break;
	case TOUCH_SW_RET_NO:						//������
		wk->seq = MYSIGN_MODE;
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );
		PRINTSYS_PrintStreamDelete( wk->printHandle );
		TOUCH_SW_Reset( wk->TouchSubWindowSys );
		break;
	}

	return seq;
}


//------------------------------------------------------------------
/**
 * $brief   �u��߂Ȃ��v�ɂ������A�{�[�h�̃N���A�����邩�H
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Oekaki_ReWrite( MYSIGN_WORK *wk, int seq )
{
	EndMessagePrint( wk, msg_oekaki_12 );		// �����Ȃ����܂����H
	wk->seq = MYSIGN_MODE_REWRITE_WAIT;

	EndSequenceCommonFunc( wk );				//�I���I�����̋��ʏ���
	return seq;

}

//------------------------------------------------------------------
/**
 * $brief   �e�@���I���ƌ������̂ŏI���
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Oekaki_ReWriteWait( MYSIGN_WORK *wk, int seq )
{
	if( EndMessageWait( wk ) ){
		wk->seq = MYSIGN_MODE_REWRITE_SELECT;
		TouchYesNoStart( wk->TouchSubWindowSys );
//		return SEQ_OUT;
	}

	EndSequenceCommonFunc( wk );		//�I���I�����̋��ʏ���
	return seq;

}









//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------
// �u���V�p�^�[��
//---------------------------------------------------------

// �ǂ����Ă��p���b�g�f�[�^��3bit�ɏk�߂Ă��܂������̂ŁA�����F��8�ɂ���
// �F�w�莩�̂�0-7�ɓ��ĂĂ���B0�͓����F����BD�ʂ����ɂ��č���������悤�ɂ��Ă���

// BMP�f�[�^�͍Œቡ8dot���K�v�Ȃ̂ŁA4x4�̃h�b�g�f�[�^����肽������
// �Q�o�C�g���ƂɎQ�Ƃ���Ȃ��f�[�^�������Q�o�C�g�K�v
static const u8 oekaki_brush[2][8][16]={
	{/*  0  */              /*  1  */               /*  2  */              /*  3  */
//	{0x00,0x00, 0x00,0x00,  0x11,0x01,  0x00,0x00,  0x11,0x01, 0x00,0x00,  0x11,0x01,0x00,0x00,},
//	{0x00,0x00, 0x00,0x00,  0x00,0x00,  0x00,0x00,  0x00,0x00, 0x00,0x00,  0x00,0x00,0x00,0x00,},

	{0x00,0x00, 0x00,0x00,  0x10,0x01,  0x00,0x00,  0x10,0x01, 0x00,0x00,  0x00,0x00,0x00,0x00,},
	{0x00,0x00, 0x00,0x00,  0x00,0x00,  0x00,0x00,  0x00,0x00, 0x00,0x00,  0x00,0x00,0x00,0x00,},


	},                                                                     
	{                                                                      
	{0x00,0x00, 0x11,0x11,  0x10,0x01,  0x10,0x11,  0x10,0x01, 0x11,0x11,  0x00,0x00,},
	{0x00,0x00, 0x22,0x22,  0x20,0x02,  0x22,0x22,  0x20,0x02, 0x22,0x22,  0x00,0x00,},
	},
};

/*


















*/



//==============================================================================
/**
 * @brief   �`��J�n�ʒu���}�C�i�X�����ɂ����Ă��`��ł���BmpWinPrint���b�p�[
 * @retval  none		
 */
//==============================================================================
static void _BmpWinPrint_Rap(
			GFL_BMPWIN * win, void * src,
			int src_x, int src_y, int src_dx, int src_dy,
			int win_x, int win_y, int win_dx, int win_dy )
{
	GFL_BMP_DATA *srcBmp = GFL_BMP_CreateWithData( (u8*)src , 1,1,GFL_BMP_16_COLOR,HEAPID_OEKAKI );
	if(win_x < 0){
		int diff;
		diff = win_x*-1;
		if(diff>win_dx){
			diff = win_dx;
		}
		win_x   = 0;
		src_x  += diff;
		src_dx -= diff;
		win_dx -= diff;
	}

	if(win_y < 0){
		int diff;
		diff = win_y*-1;
		if(diff>win_dy){
			diff = win_dy;
		}
		win_y   = 0;
		src_y  += diff;
		src_dy -= diff;
		win_dy -= diff;
	}

	GFL_BMP_Print( srcBmp, GFL_BMPWIN_GetBmp(win), src_x, src_y, win_x, win_y, win_dx, win_dy, 0);

	GFL_BMP_Delete( srcBmp );
}


//------------------------------------------------------------------
/**
 * $brief   ���C���`��
 *
 * @param   win		
 * @param   brush		
 * @param   px		
 * @param   py		
 * @param   sx		
 * @param   sy		
 * @param   count		
 * @param   flag		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DrawPoint_to_Line( 
	GFL_BMPWIN *win, 
	const u8 *brush, 
	int px, int py, int *sx, int *sy, 
	int count, int flag )
{
	int dx, dy, s, step;
	int x1 = *sx;
	int y1 = *sy;
	int x2 = px;
	int y2 = py;

	// ����͌��_�ۑ��̂�
	if(count==0 && flag == 0){
		*sx = px;		*sy = py;
		return;
	}
	

	dx = MATH_IAbs(x2 - x1);  dy = MATH_IAbs(y2 - y1);
	if (dx > dy) 
	{
		if (x1 > x2)
		{
			step = (y1 > y2) ? 1 : -1;
			s = x1;  x1 = x2;  x2 = s;  y1 = y2;
		} 
		else
		{	
			step = (y1 < y2) ? 1: -1;
		}

//		GFL_BG_BmpWinPrint( win, (void*)brush,	0, 0, 4, 4, x1, y1, 4, 4 );
		_BmpWinPrint_Rap( win, (void*)brush,	0, 0, 4, 4, x1, y1, 4, 4 );
		s = dx >> 1;
		while (++x1 <= x2)
		{
			if ((s -= dy) < 0)
			{
				s += dx;  y1 += step;
			};
//			GFL_BG_BmpWinPrint( win, (void*)brush,	0, 0, 4, 4, x1, y1, 4, 4 );
			_BmpWinPrint_Rap( win, (void*)brush,	0, 0, 4, 4, x1, y1, 4, 4 );
		}
	}
	else
	{
		if (y1 > y2)
		{
			step = (x1 > x2) ? 1 : -1;
			s = y1;  y1 = y2;  y2 = s;  x1 = x2;
		}
		else
		{
			step = (x1 < x2) ? 1 : -1;
		}
//		GFL_BG_BmpWinPrint( win, (void*)brush,	0, 0, 4, 4, x1, y1, 4, 4 );
		_BmpWinPrint_Rap( win, (void*)brush,	0, 0, 4, 4, x1, y1, 4, 4 );
		s = dy >> 1;
		while (++y1 <= y2)
		{
			if ((s -= dx) < 0)
			{
				s += dy;  x1 += step;
			}
//			GFL_BG_BmpWinPrint( win, (void*)brush,	0, 0, 4, 4, x1, y1, 4, 4 );
			_BmpWinPrint_Rap( win, (void*)brush,	0, 0, 4, 4, x1, y1, 4, 4 );
		}
	}
	
	
	*sx = px;			*sy = py;

}

static void Stock_OldTouch( TOUCH_INFO *all, OLD_TOUCH_INFO *stock )
{
	stock->size = all->size;
	if(all->size!=0){
		stock->x = all->x[all->size-1];
		stock->y = all->y[all->size-1];
	}
}



static int debug_count;
//------------------------------------------------------------------
/**
 * $brief  �ʐM�Ŏ�M�����^�b�`�p�l���̌��ʃf�[�^�����ɕ`�悷��
 *
 * @param   win		
 * @param   all		
 * @param   draw	��������ōs����CGX�ύX��]�����邩�H(0:���Ȃ�	1:����j
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DrawBrushLine( GFL_BMPWIN *win, TOUCH_INFO *all, OLD_TOUCH_INFO *old, int draw )
{
	int px,py,i,r,flag=0, sx, sy;

//	OS_Printf("id0=%d,id1=%d,id2=%d,id3=%d,id4=%d\n",all[0].size,all[1].size,all[2].size,all[3].size,all[4].size);
	
	if(all->size!=0){
		if(old->size){
			sx = old->x-OEKAKI_BOARD_POSX*8;
			sy = old->y-OEKAKI_BOARD_POSY*8;
		}
		for(r=0;r<all->size;r++){
			px = all->x[r] - OEKAKI_BOARD_POSX*8;
			py = all->y[r] - OEKAKI_BOARD_POSY*8;
			

			// BG1�ʗpBMP�i���G�����摜�j�E�C���h�E�m��
			DrawPoint_to_Line(win, oekaki_brush[0][all->brush], px, py, &sx, &sy, r, old->size);
			flag = 1;
		}
		
	}
	if(flag && draw)
	{
//		OS_Printf("write board %d times\n",debug_count++);
		GFL_BMPWIN_TransVramCharacter( win );
		GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_M);
	}
	
	// ����̍ŏI���W�̃o�b�N�A�b�v�����   
    Stock_OldTouch(all, old);
	all->size = 0;		// ��x�`�悵������W���͎̂Ă�
	
}

//------------------------------------------------------------------
/**
 * �J�[�\���̃p���b�g�ύX�i�_�Łj
 *
 * @param   CursorCol	sin�ɓn���p�����[�^�i360�܂Łj
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void CursorColTrans(u16 *CursorCol)
{
	/*
	fx32  sin;
	GXRgb tmp;
	int   r,g,b;

	*CursorCol+=20;
	if(*CursorCol>360){
		*CursorCol = 0;
	}

	sin = GFL_CALC_Sin360R(*CursorCol);
	g   = 15 +( sin * 10 ) / FX32_ONE;
	tmp = GX_RGB(29,g,0);


	GX_LoadOBJPltt((u16*)&tmp, ( 12 )*2, 2);
	*/
}





#define PLATE_CHARA_OFFSET1	( 12 )
#define PLATE_CHARA_OFFSET2	( 16 )

static const u8 plate_num[4]={2,2,3,2};
static const u8 plate_table[4][3]={
	{8*8,17*8,0,},
	{8*8,17*8,0,},
	{5*8,12*8,18*8,},
	{8*8,17*8,0,},

};
static const u8 plate_chara_no[][5]={
	{PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
	{PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
	{PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
	{PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
};




//------------------------------------------------------------------
/**
 * $brief   ��b�E�C���h�E�\��
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void EndMessagePrint( MYSIGN_WORK *wk, int msgno )
{
	// ������擾
	STRBUF *tempbuf;
	
	tempbuf = GFL_STR_CreateBuffer(TALK_MESSAGE_BUF_NUM,HEAPID_OEKAKI);
	GFL_MSG_GetString(  wk->MsgManager, msgno, tempbuf );
	WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );
	GFL_STR_DeleteBuffer(tempbuf);

	// ��b�E�C���h�E�g�`��
	GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->MsgWin),15/*FBMP_COL_WHITE*/);
	BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, 1, 12 );
	GFL_BMPWIN_MakeScreen(wk->MsgWin);
	GFL_BMPWIN_TransVramCharacter(wk->MsgWin);
	GFL_BG_LoadScreenV_Req(GFL_BG_FRAME0_M);

	// ������`��J�n
	wk->printHandle = PRINTSYS_PrintStream( wk->MsgWin, 0,0
			,wk->TalkString ,wk->fontHandle ,MSGSPEED_GetWait() 
			,wk->printTcblSys , 0 , HEAPID_OEKAKI , 0);


}

//------------------------------------------------------------------
/**
 * $brief   ��b�\���E�C���h�E�I���҂�
 *
 * @param   msg_index		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int EndMessageWait( MYSIGN_WORK *wk )
{
	if(PRINTSYS_PrintStreamGetState( wk->printHandle )==PRINTSTREAM_STATE_DONE){
		
		return 1;
	}
	return 0;
}


//------------------------------------------------------------------
/**
 * $brief   �T�C���f�[�^���������ށi16�F�f�[�^��1bit�ɏk�߂ď�������)
 *
 * @param   des		
 * @param   src		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void Output_SignData( u8 *des, u8 *src )
{
	int i,s,count=0;
	
	
	s=0;
	*des = 0;
	for(i=0;i<OEKAKI_GRAPHI_SIZE;i++){
		int w;

		// ���ʃf�[�^��1bit��
		w = src[i]&0x0f;
		if(w==1){
			*des |= (1<<s);
		}
		s++;

		// ��ʃf�[�^��1bit��
		w = src[i]>>4;
		if(w==1){
			*des |= (1<<s);
		}
		s++;

		// 8bit�ɂȂ����珑���o��
		if(s==8){
			count++;
			des++;
			*des=0;
			s=0;
		}
		
		if( count>= OEKAKI_GRAPHI_SIZE ){
			GF_ASSERT(0 && "���k�f�[�^���̈�𒴂���");
		}

	}
}


#define MIN_SCRUCH	(3)
#define MAX_SCRUCH	(40)

//--------------------------------------------------------------------------------------------
/**
 * �o�b�W����
 *
 * @param	wk		��ʂ̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BrushCanvas( MYSIGN_WORK *wk)
{
	BOOL scruch;
	int sub;
	u32 tpx,tpy;
	const BOOL isTouch = GFL_UI_TP_GetPointCont( &tpx,&tpy );
	scruch = FALSE;
	//�ێ����W�ƃ^�b�`���W�̍��������
	if( isTouch == TRUE && wk->BeforeTouch == TRUE ){	//�l�L�����H

		//�������K��l�ȏ�̏ꍇ�͖��������Ƃɂ���
		if ( wk->BeforeX > tpx ){		//�O��̂ق����l���傫�����H
			sub = wk->BeforeX - tpx;
			wk->scruchInfo.DirX = -1;
		}else{
			sub = tpx - wk->BeforeX;
			wk->scruchInfo.DirX = 1;
		}
		if ( (sub>=MIN_SCRUCH)&&(sub<=MAX_SCRUCH) ){
			if ( wk->BeforeY > tpy ){	//�O��̂ق����l���傫�����H
				sub = wk->BeforeY - tpy;
				wk->scruchInfo.DirY = -1;
			}else{
				sub = tpy - wk->BeforeY;
				wk->scruchInfo.DirY = 1;
			}
			if (sub<=MAX_SCRUCH){
				scruch = TRUE;			//��������
				PlayScruchSe( &wk->scruchInfo );
//				Snd_SePlay( SEQ_SE_DP_KYU01 );
			}else{
//				ClearScruchSndNow(&wk->ScruchSnd);
			}
		}else if (sub<=MAX_SCRUCH){
			if ( wk->BeforeY > tpy ){		//�O��̂ق����l���傫�����H
				sub = wk->BeforeY - tpy;
				wk->scruchInfo.DirY = -1;
			}else{
				sub = tpy - wk->BeforeY;
				wk->scruchInfo.DirY = 1;
			}
			if ((sub>=MIN_SCRUCH)&&(sub<=MAX_SCRUCH)){
				scruch = TRUE;		//��������
				PlayScruchSe( &wk->scruchInfo );
//				Snd_SePlay( SEQ_SE_DP_KYU01 );
			}else{
//				ClearScruchSndNow(&wk->ScruchSnd);
			}
		}

	}
		//�ێ����W�X�V
	wk->BeforeX = tpx;
	wk->BeforeY = tpy;
	wk->BeforeTouch = isTouch;
}


//------------------------------------------------------------------
/**
 * @brief   ������ς����̂ł���΁A���ʉ����u�L���b�v�Ɩ炷
 *
 * @param   scruchInfo		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void PlayScruchSe( SCRUCH_INFO *scruchInfo )
{
	//���񔻒�
	if ((scruchInfo->OldDirX == 0)&&(scruchInfo->OldDirY == 0)){
		OS_Printf("����\n");//����
#if SIGN_USE_SND
		if(!Snd_SePlayCheck( SEQ_SE_DP_KYU01 )){
			Snd_SePlay( SEQ_SE_DP_KYU01 );
		}
#endif
	}
	//�O��̕����ƍ���̕������r
	if ((scruchInfo->OldDirX*scruchInfo->DirX<0) ||
			(scruchInfo->OldDirY*scruchInfo->DirY<0)){
//		OS_Printf("�؂�ւ�\n");//�������ς�����̂ŉ���؂�ւ���
#if SIGN_USE_SND
		if(!Snd_SePlayCheck( SEQ_SE_DP_KYU01 )){
			Snd_SePlay( SEQ_SE_DP_KYU01 );
		}
#endif
	}

	scruchInfo->OldDirX = scruchInfo->DirX;
	scruchInfo->OldDirY = scruchInfo->DirY;
	scruchInfo->DirX = 0;
	scruchInfo->DirY = 0;
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	BMP�̃L�����N�^�f�[�^��OAM�̃T�C�Y�Ő؂���
 *
 *	@param	bmp				�r�b�g�}�b�v�f�[�^
 *	@param	oam_csx			OAM�̉��T�C�Y	�i�L�����N�^�P�ʁj
 *	@param	oam_csy			OAM�̏c�T�C�Y	�i�L�����N�^�P�ʁj
 *	@param	bmp_cmx			�r�b�g�}�b�v�؂��荶�゘���W	�i�L�����N�^�P�ʁj
 *	@param	bmp_cmy			�r�b�g�}�b�v�؂��荶�゙���W	�i�L�����N�^�P�ʁj
 *	@param	char_buff		�o�͐�L�����N�^�o�b�t�@ (oam_csx * oam_csy)*32byte�@�T�C�Y�ȏ�̗̈�
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
void MYSIGN_BmpCutOamSize( GFL_BMPWIN* cp_bmp, int oam_csx, int oam_csy, int bmp_cmx, int bmp_cmy, char* char_buff )
{
	const int MYSIGN_CHAR_BYTE	= 32;	// 1char�T�C�Y
	int i;				// ���[�v�p
	int buff_out;		// �o�b�t�@�������ݐ�
	int buff_in;		// �o�b�t�@�ǂݍ��ݐ�
	
	// bmp�f�[�^�̃T�C�Y������邩�`�F�b�N
	GF_ASSERT( GFL_BMPWIN_GetScreenSizeX(cp_bmp) >= (oam_csx + bmp_cmx) );
	GF_ASSERT( GFL_BMPWIN_GetScreenSizeX(cp_bmp) >= (oam_csy + bmp_cmy) );
	
	// ���[�J���o�b�t�@�Ƀf�[�^���
	for( i=0; i<oam_csy; i++ ){
		
		buff_out = i * oam_csx;
		buff_out *= MYSIGN_CHAR_BYTE;
		buff_in =  ((i + bmp_cmy) * GFL_BMPWIN_GetScreenSizeX(cp_bmp));
		buff_in += bmp_cmx;
		buff_in *= MYSIGN_CHAR_BYTE;
		memcpy( char_buff + buff_out, (char*)(GFL_BMP_GetCharacterAdrs(GFL_BMPWIN_GetBmp(cp_bmp)) ) + buff_in , MYSIGN_CHAR_BYTE * oam_csx );
	}
}
