//======================================================================
/**
 * @file	startmenu.c
 * @brief	�ŏ�����E����������s���g�b�v���j���[
 * @author	ariizumi
 * @data	09/01/07
 */
//======================================================================
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
#include "startmenu.naix"
#include "msg/msg_startmenu.h"

#include "net/dwc_raputil.h"
#include "savedata/save_control.h"
#include "savedata/mystatus.h"
#include "app/name_input.h"
#include "title/startmenu.h"
#include "title/title.h"
#include "title/game_start.h"
#include "test/ariizumi/ari_debug.h"
//======================================================================
//	define
//======================================================================
#define BG_PLANE_MENU GFL_BG_FRAME1_M
#define BG_PLANE_BACK_GROUND GFL_BG_FRAME3_M

//BG�L�����A�h���X
#define START_MENU_FRAMECHR1	1
#define START_MENU_FRAMECHR2	(START_MENU_FRAMECHR1 + TALK_WIN_CGX_SIZ)

//�p���b�g
#define START_MENU_PLT_FONT		0
#define START_MENU_PLT_SEL		3
#define START_MENU_PLT_NOSEL	4

#define START_MENU_FONT_TOP  2
#define START_MENU_FONT_LEFT 2

//======================================================================
//	enum
//======================================================================
typedef enum 
{
	SMS_FADE_IN,
	SMS_SELECT,
	SMS_FADE_OUT,

	SMS_MAX,
}START_MENU_STATE;

//�X�^�[�g���j���[�̍���
enum START_MENU_ITEM
{
	SMI_CONTINUE,		//��������
	SMI_NEWGAME,		//�ŏ�����
	SMI_MYSTERY_GIFT,	//�s�v�c�ȑ��蕨
	SMI_WIFI_SETTING,	//Wifi�ݒ�
	SMI_EMAIL_SETTING,	//E-Mail�ݒ�
	
	SMI_MAX,
	
	SMI_RETURN_TITLE = 0xFF,	//B�Ŗ߂�
};

//======================================================================
//	typedef struct
//======================================================================

//���ڏ��
typedef struct
{
	BOOL enable_;
	u8	top_;
	GFL_BMPWIN *win_;
}START_MENU_ITEM_WORK;

//���[�N
typedef struct
{
	HEAPID heapId_;
	START_MENU_STATE state_;
		
	u8		selectItem_;	//�I�𒆍���
	u8		selectButtom_;	//�I�𒆈ʒu(���[
	int		dispPos_;		//�\�����ʒu(dot�P��
	int		targetPos_;		//�\���ڕW�ʒu(dot�P��
	u8		length_;		//���j���[�̒���(�L�����P��
	
	MYSTATUS	*mystatus_;
	
	GFL_FONT *fontHandle_;
	GFL_MSGDATA *msgMng_;	//���j���[�쐬�̂Ƃ���ł����L��
	GFL_TCB		*vblankFuncTcb_;

	GFL_CLUNIT	*cellUnit_;
	GFL_CLWK	*cellCursor_[2];
	NNSG2dImagePaletteProxy	cursorPltProxy_;
	NNSG2dCellDataBank	*cursorCellData_;
	NNSG2dAnimBankData	*cursorAnmData_;
	void	*cursorCellRes_;
	void	*cursorAnmRes_;

	START_MENU_ITEM_WORK itemWork_[SMI_MAX];
}START_MENU_WORK;

//���ڐݒ���
//���ڂ��J�����H
typedef BOOL (*START_MENU_ITEM_CheckFunc)(START_MENU_WORK *work);
//���莞����
typedef BOOL (*START_MENU_ITEM_SelectFunc)(START_MENU_WORK *work);
//���ړ��`��
typedef void (*START_MENU_ITEM_DrawFunc)(START_MENU_WORK *work,GFL_BMPWIN *win, const u8 idx );
typedef struct
{
	u8	height_;	//����(�L�����P��
	//�e�퓮��
	START_MENU_ITEM_CheckFunc	checkFunc_;
	START_MENU_ITEM_SelectFunc	selectFunc_;
	START_MENU_ITEM_DrawFunc	drawFunc_;
}START_MENU_ITEM_SETTING;


//======================================================================
//	proto
//======================================================================

static void START_MENU_VBlankFunc(GFL_TCB *tcb,void *work);

static void	START_MENU_InitGraphic( START_MENU_WORK *work );
static void	START_MENU_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane );

static void	START_MENU_CreateMenuItem( START_MENU_WORK *work );

static BOOL START_MENU_MoveSelectItem( START_MENU_WORK *work , BOOL isDown );
static void START_MENU_ChangeActiveItem( START_MENU_WORK *work , const u8 newItem , const u8 oldItem );

//���j���[���ڗp�@�\�֐�
static BOOL START_MENU_ITEM_TempCheck( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_TempCheckFalse( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_CheckContinue( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_CommonSelect( START_MENU_WORK *work );
static void START_MENU_ITEM_CommonDraw( START_MENU_WORK *work , GFL_BMPWIN *win, const u8 idx );
static void START_MENU_ITEM_ContinueDraw( START_MENU_WORK *work , GFL_BMPWIN *win, const u8 idx );

static void START_MENU_ITEM_MsgDrawFunc( START_MENU_WORK *work , GFL_BMPWIN *win , u8 posX , u8 posY , u16 msgId );
static void START_MENU_ITEM_WordMsgDrawFunc( START_MENU_WORK *work , GFL_BMPWIN *win , u8 posX , u8 posY , u16 msgId , WORDSET *word);

//Proc�f�[�^
static GFL_PROC_RESULT START_MENU_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT START_MENU_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT START_MENU_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

const GFL_PROC_DATA StartMenuProcData = {
	START_MENU_ProcInit,
	START_MENU_ProcMain,
	START_MENU_ProcEnd,
};

//���j���[���ڐݒ�f�[�^�z��
static const START_MENU_ITEM_SETTING ItemSettingData[SMI_MAX] =
{
	{ 10 , START_MENU_ITEM_CheckContinue , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_ContinueDraw },
	{  2 , START_MENU_ITEM_TempCheck , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_CommonDraw },
	{  2 , START_MENU_ITEM_TempCheck , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_CommonDraw },
	{  2 , START_MENU_ITEM_TempCheck , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_CommonDraw },
	{  2 , START_MENU_ITEM_TempCheck , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_CommonDraw },
};



//--------------------------------------------------------------
static GFL_PROC_RESULT START_MENU_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	u8 i;
	
	START_MENU_WORK *work;
		//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_STARTMENU, 0x80000 );
	work = GFL_PROC_AllocWork( proc, sizeof(START_MENU_WORK), HEAPID_STARTMENU );
	GFL_STD_MemClear(work, sizeof(START_MENU_WORK));

	work->heapId_ = HEAPID_STARTMENU;
	for( i=0;i<SMI_MAX;i++ )
	{
		work->itemWork_[i].enable_ = FALSE;
	}
	work->mystatus_ = SaveData_GetMyStatus( SaveControl_GetPointer() );
	work->state_ = SMS_FADE_IN;
	START_MENU_InitGraphic( work );

	//�t�H���g�p�p���b�g
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , START_MENU_PLT_FONT * 32, 16*2, work->heapId_ );
	work->fontHandle_ = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId_ );

	//WinFrame�p�O���t�B�b�N�ݒ�
	BmpWinFrame_GraphicSet( BG_PLANE_MENU , START_MENU_FRAMECHR1 , START_MENU_PLT_SEL 	, 0, work->heapId_);
	BmpWinFrame_GraphicSet( BG_PLANE_MENU , START_MENU_FRAMECHR2 , START_MENU_PLT_NOSEL , 1, work->heapId_);
	//�w�i�F
	*((u16 *)HW_BG_PLTT) = 0x7d8c;//RGB(12, 12, 31);
	
	START_MENU_CreateMenuItem( work );
	
	work->vblankFuncTcb_ = GFUser_VIntr_CreateTCB( START_MENU_VBlankFunc , (void*)work , 0 );

	GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN , 16 , 0 , ARI_FADE_SPD );
	
	return GFL_PROC_RES_FINISH;
}

FS_EXTERN_OVERLAY(mystery);
extern const GFL_PROC_DATA MysteryGiftProcData;
//--------------------------------------------------------------------------
static GFL_PROC_RESULT START_MENU_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	START_MENU_WORK *work = mywk;

	//�I���ɂ�镪�򏈗�
	{
		switch( work->selectItem_ )
		{
		case SMI_CONTINUE:		//��������
			//Proc�̕ύX�𒆂ł���Ă�
			GameStart_Continue();
			break;
		case SMI_NEWGAME:		//�ŏ�����
			//Proc�̕ύX�𒆂ł���Ă�
			GameStart_Beginning();
			//���O���͂�
			//GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &NameInputProcData,(void*)NAMEIN_MYNAME);
			break;
			
		case SMI_MYSTERY_GIFT:	//�s�v�c�ȑ��蕨
			GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(mystery), &MysteryGiftProcData, NULL);
			break;

        case SMI_WIFI_SETTING: //WIFI�ݒ�
			GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(wifi_util), &WifiUtilProcData, NULL);
            break;
            
		case SMI_RETURN_TITLE:
			GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
			break;
		default:
			//Proc�̕ύX�𒆂ł���Ă�
			GameStart_Beginning();
			break;
		}
	}

	//�J������
	{
		u8 i;
		GFL_TCB_DeleteTask( work->vblankFuncTcb_ );

		GFL_CLACT_WK_Remove( work->cellCursor_[0] );
		GFL_CLACT_WK_Remove( work->cellCursor_[1] );
		GFL_HEAP_FreeMemory( work->cursorCellRes_ );
		GFL_HEAP_FreeMemory( work->cursorAnmRes_ );
		GFL_CLACT_UNIT_Delete( work->cellUnit_ );
		GFL_CLACT_SYS_Delete();

		for( i=0;i<SMI_MAX;i++ )
		{
			if( work->itemWork_[i].enable_ == TRUE )
			{
				GFL_BMPWIN_Delete( work->itemWork_[i].win_ );
			}
		}

		GFL_FONT_Delete( work->fontHandle_ );
		GFL_BMPWIN_Exit();

		GFL_BG_FreeBGControl(BG_PLANE_MENU);
		GFL_BG_FreeBGControl(BG_PLANE_BACK_GROUND);
		GFL_BG_Exit();
		
		GFL_PROC_FreeWork( proc );

		GFL_HEAP_DeleteHeap( HEAPID_STARTMENU );
	}

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
static GFL_PROC_RESULT START_MENU_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	START_MENU_WORK *work = mywk;
	const int keyTrg = GFL_UI_KEY_GetTrg();
	
	switch( work->state_ )
	{
	case SMS_FADE_IN:
		if( GFL_FADE_CheckFade() == FALSE )
		{
			work->state_ = SMS_SELECT;
		}
		break;
		
	case SMS_SELECT:
		if( keyTrg & PAD_KEY_UP )
		{
			START_MENU_MoveSelectItem( work , FALSE );
		}
		else
		if( keyTrg & PAD_KEY_DOWN )
		{
			START_MENU_MoveSelectItem( work , TRUE );
		}
		else
		if( keyTrg & PAD_BUTTON_A )
		{
			const BOOL ret = ItemSettingData[work->selectItem_].selectFunc_( work );
			if( ret == TRUE )
			{
				GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN , 0 , 16 , ARI_FADE_SPD );
				work->state_ = SMS_FADE_OUT;
			}
		}
		else
		if( keyTrg & PAD_BUTTON_B )
		{
			GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN , 0 , 16 , ARI_FADE_SPD );
			work->selectItem_ = SMI_RETURN_TITLE;
			work->state_ = SMS_FADE_OUT;
		}
		break;
		
	case SMS_FADE_OUT:
		if( GFL_FADE_CheckFade() == FALSE )
		{
			return GFL_PROC_RES_FINISH;
		}
		break;
	}

	
	//�X�N���[������(������ɃI�[�o�[�����ȂǓ���ĂȂ��̂�8�̖񐔂�
	if( work->targetPos_ > work->dispPos_ )
	{
		work->dispPos_ += 2;
		GFL_BG_SetScroll( BG_PLANE_MENU , GFL_BG_SCROLL_Y_SET , work->dispPos_ );
	}
	else	//���j���[�̓��̈ʒu�Ȃ̂Ōv�Z�������E�E�E
	if( work->targetPos_ < work->dispPos_ )
	{
		work->dispPos_ -= 2;
		GFL_BG_SetScroll( BG_PLANE_MENU , GFL_BG_SCROLL_Y_SET , work->dispPos_ );
	}

	//OBJ�̍X�V
	GFL_CLACT_SYS_Main();

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	VBLank Function
 * @param	NONE
 * @return	NONE
 */
//------------------------------------------------------------------
static void START_MENU_VBlankFunc(GFL_TCB *tcb,void *work)
{
	GFL_CLACT_SYS_VBlankFunc();
}

//--------------------------------------------------------------------------
//	�O���t�B�b�N���菉����
//--------------------------------------------------------------------------
static void	START_MENU_InitGraphic( START_MENU_WORK *work )
{
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_32K,		// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};

	static const GFL_BG_SYS_HEADER sysHeader = {
		GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
	};
	
	static const GFL_BG_BGCNT_HEADER bgCont_Menu = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	};

	static const GFL_BG_BGCNT_HEADER bgCont_BackGround = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	};
	
	GX_SetMasterBrightness(-16);	
	GXS_SetMasterBrightness(-16);
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);

	GFL_DISP_SetBank( &vramBank );
	GFL_BG_Init( work->heapId_ );
	GFL_BG_SetBGMode( &sysHeader );	

	START_MENU_InitBgFunc( &bgCont_Menu , BG_PLANE_MENU );
	START_MENU_InitBgFunc( &bgCont_BackGround , BG_PLANE_BACK_GROUND );
	
	GFL_BMPWIN_Init( work->heapId_ );
	
	//OBJ�n
	{
		NNSG2dImageProxy	imgProxy;
		GFL_CLWK_RES	cellRes;
		GFL_CLWK_DATA	cellInitData;

		GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
		cellSysInitData.oamst_main = 0x10;	//�f�o�b�O���[�^�̕�
		cellSysInitData.oamnum_main = 128-0x10;
		GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId_ );
		work->cellUnit_  = GFL_CLACT_UNIT_Create( 2 , 0, work->heapId_ );
		GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit_ );

		NNS_G2dInitImagePaletteProxy( &work->cursorPltProxy_ );
		NNS_G2dInitImageProxy( &imgProxy );
		//�e��f�ނ̓ǂݍ���
		GFL_ARC_UTIL_TransVramPaletteMakeProxy( ARCID_STARTMENU , NARC_startmenu_title_cursor_NCLR , 
				NNS_G2D_VRAM_TYPE_2DMAIN , 0 , work->heapId_ , &work->cursorPltProxy_ );
		
		work->cursorCellRes_ = GFL_ARC_UTIL_LoadCellBank( ARCID_STARTMENU , NARC_startmenu_title_cursor_NCER , 
					FALSE , &work->cursorCellData_ , work->heapId_ );
	
		work->cursorAnmRes_ = GFL_ARC_UTIL_LoadAnimeBank( ARCID_STARTMENU , NARC_startmenu_title_cursor_NANR ,
					FALSE , &work->cursorAnmData_ , work->heapId_ );
		
		GFL_ARC_UTIL_TransVramCharacterMakeProxy( ARCID_STARTMENU , NARC_startmenu_title_cursor_NCGR , 
					FALSE , 0 , 0 , NNS_G2D_VRAM_TYPE_2DMAIN , 0 , work->heapId_ , &imgProxy );

		//�Z���̐���
		GFL_CLACT_WK_SetCellResData( &cellRes , &imgProxy , &work->cursorPltProxy_ ,
					work->cursorCellData_ , work->cursorAnmData_ );

		cellInitData.pos_x = 128;
		cellInitData.pos_y =  8;
		cellInitData.anmseq = 0;
		cellInitData.softpri = 0;
		cellInitData.bgpri = 0;
		//�����
		work->cellCursor_[0] = GFL_CLACT_WK_Add( work->cellUnit_ , &cellInitData ,
					&cellRes , CLSYS_DEFREND_MAIN , work->heapId_ );
		GFL_CLACT_WK_SetAutoAnmSpeed( work->cellCursor_[0], FX32_ONE );
		GFL_CLACT_WK_SetAutoAnmFlag( work->cellCursor_[0], TRUE );
		GFL_CLACT_WK_SetDrawEnable( work->cellCursor_[0], FALSE );
		
		//�����
		cellInitData.pos_y =  192-8;
		cellInitData.anmseq = 1;
		work->cellCursor_[1] = GFL_CLACT_WK_Add( work->cellUnit_ , &cellInitData ,
					&cellRes , CLSYS_DEFREND_MAIN , work->heapId_ );
		GFL_CLACT_WK_SetAutoAnmSpeed( work->cellCursor_[1], FX32_ONE );
		GFL_CLACT_WK_SetAutoAnmFlag( work->cellCursor_[1], TRUE );
		GFL_CLACT_WK_SetDrawEnable( work->cellCursor_[1], FALSE );
				
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
	}

}

//--------------------------------------------------------------------------
//	Bg������ �@�\��
//--------------------------------------------------------------------------
static void	START_MENU_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane )
{
	GFL_BG_SetBGControl( bgPlane, bgCont, GFL_BG_MODE_TEXT );
	GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
	GFL_BG_ClearFrame( bgPlane );
	GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------------------
//	���j���[�̍쐬
//--------------------------------------------------------------------------
static void	START_MENU_CreateMenuItem( START_MENU_WORK *work )
{
	u8	i;
	u8	bgTopPos = 1;	//�A�C�e���̕\���ʒu(�L�����P��

	work->msgMng_ = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_startmenu_dat, work->heapId_);
	work->selectItem_ = 0xFF;	//�ŏ��ɃA�N�e�B�u�Ȃ͈̂�ԏ�ɂ��邽�߂̔���p
	for( i=0;i<SMI_MAX;i++ )
	{
		if( ItemSettingData[i].checkFunc_(work) == TRUE )
		{
			work->itemWork_[i].enable_ = TRUE;
			work->itemWork_[i].win_ = GFL_BMPWIN_Create( 
						BG_PLANE_MENU , 
						3 , bgTopPos ,
						26 , ItemSettingData[i].height_ ,
						START_MENU_PLT_FONT , 
						GFL_BMP_CHRAREA_GET_B );
			GFL_BMPWIN_MakeScreen( work->itemWork_[i].win_ );
			GFL_BMPWIN_TransVramCharacter( work->itemWork_[i].win_ );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->itemWork_[i].win_), 0xf );

			ItemSettingData[i].drawFunc_(work,work->itemWork_[i].win_,i);
			
			GFL_BMPWIN_TransVramCharacter( work->itemWork_[i].win_ );
			BmpWinFrame_Write(work->itemWork_[i].win_, WINDOW_TRANS_ON, 
							START_MENU_FRAMECHR1, START_MENU_PLT_NOSEL);
			
			bgTopPos += ItemSettingData[i].height_ + 2;
			if( work->selectItem_ == 0xFF )
			{
				//��ԏ�̍���
				work->selectItem_ = i;
			}
		}
	}
	GFL_MSG_Delete( work->msgMng_ );
	work->msgMng_ = NULL;
	
	GF_ASSERT( work->selectItem_ != 0xFF );
	work->selectButtom_ = ItemSettingData[work->selectItem_].height_ + 2;
	work->length_ = bgTopPos-1;	//���̃��j���[�̘g�̕��������Ă���̂�1����
	START_MENU_ChangeActiveItem( work , work->selectItem_ , 0xFF );
	
	//�ŏ��̉��J�[�\���̕\���`�F�b�N
	if( work->length_ > 24 )
		GFL_CLACT_WK_SetDrawEnable( work->cellCursor_[1], TRUE );

}

//--------------------------------------------------------------------------
//	���ڈړ�
//	@return �ړ��ł������H
//	@value  BOOL isDown�@FALSE:���ړ� TRUE:���ړ�
//--------------------------------------------------------------------------
static BOOL START_MENU_MoveSelectItem( START_MENU_WORK *work , BOOL isDown )
{
	const int moveValue = (isDown==TRUE ? 1 : -1 );
	const int endValue = (isDown==TRUE ? SMI_MAX : -1 );
	int	i;
	BOOL isShow;
	//�J�n�ʒu�͂P���������Ă���
	for( i = work->selectItem_ + moveValue; i != endValue ; i += moveValue )
	{
		if( work->itemWork_[i].enable_ == TRUE )
		{
			break;
		}
	}
	if( i == endValue )
	{
		//�����Ȃ�����
		return FALSE;
	}
	
	START_MENU_ChangeActiveItem( work , i , work->selectItem_ );
	if( isDown == TRUE )
	{
		work->selectButtom_ += ItemSettingData[i].height_ + 2;	//�g��+2
	}
	else
	{
		work->selectButtom_ -= ItemSettingData[work->selectItem_].height_ + 2;	//�g��+2
	}
	
	work->selectItem_ = i;
	
	//�\���ʒu�v�Z
	if( work->selectButtom_*8 > work->targetPos_ + 192 )
	{
		//���̃`�F�b�N
		work->targetPos_ = work->selectButtom_*8 -192;
	}
	else	//���j���[�̓��̈ʒu�Ȃ̂Ōv�Z�������E�E�E
	if( ( work->selectButtom_ - ItemSettingData[work->selectItem_].height_ - 2 )*8 < work->targetPos_ )
	{
		//��̃`�F�b�N
		work->targetPos_ = ( work->selectButtom_ - ItemSettingData[work->selectItem_].height_ - 2 )*8;
	}

	//�J�[�\���\���̃`�F�b�N
	//��
	if( work->targetPos_ > 0 )
		GFL_CLACT_WK_SetDrawEnable( work->cellCursor_[0], TRUE );
	else
		GFL_CLACT_WK_SetDrawEnable( work->cellCursor_[0], FALSE );

	//��
	if( work->targetPos_ + 192 < work->length_*8 )
		GFL_CLACT_WK_SetDrawEnable( work->cellCursor_[1], TRUE );
	else
		GFL_CLACT_WK_SetDrawEnable( work->cellCursor_[1], FALSE );
	
	return TRUE;
}

//--------------------------------------------------------------------------
//	�I�����j���[�̐؂�ւ�
//--------------------------------------------------------------------------
static void START_MENU_ChangeActiveItem( START_MENU_WORK *work , const u8 newItem , const u8 oldItem )
{
	if( newItem != 0xFF )
	{
		BmpWinFrame_Write(work->itemWork_[newItem].win_, WINDOW_TRANS_ON, 
						START_MENU_FRAMECHR2, START_MENU_PLT_SEL);
	}
	if( oldItem != 0xFF )
	{
		BmpWinFrame_Write(work->itemWork_[oldItem].win_, WINDOW_TRANS_ON, 
						START_MENU_FRAMECHR1, START_MENU_PLT_NOSEL);
	}
}

#pragma mark MenuSelectFunc

//--------------------------------------------------------------------------
//	���j���[��L���ɂ��邩�H
//	@return TRUE=�L��
//--------------------------------------------------------------------------
static BOOL START_MENU_ITEM_TempCheck( START_MENU_WORK *work )
{
	return TRUE;
}
static BOOL START_MENU_ITEM_TempCheckFalse( START_MENU_WORK *work )
{
	return FALSE;
}
static BOOL START_MENU_ITEM_CheckContinue( START_MENU_WORK *work )
{
	return SaveData_GetExistFlag( SaveControl_GetPointer() );
}

//--------------------------------------------------------------------------
//	���莞�̓���
//	@return TRUE=�X�^�[�g���j���̏I��������
//--------------------------------------------------------------------------
static BOOL START_MENU_ITEM_CommonSelect( START_MENU_WORK *work )
{
	//���̂Ƃ���I�����ꂽ
	return TRUE;
}
//���j���[���`�拤��(idx��msgId��ǂݕ�����
static void START_MENU_ITEM_CommonDraw( START_MENU_WORK *work , GFL_BMPWIN *win , const u8 idx )
{
	static const msgIdArr[SMI_MAX] =
	{
		START_MENU_STR_ITEM_01_01,	//���ۂ͂��Ȃ�(�_�~�[
		START_MENU_STR_ITEM_02,
		START_MENU_STR_ITEM_03,
		START_MENU_STR_ITEM_04,
		START_MENU_STR_ITEM_05,
	};
	
	START_MENU_ITEM_MsgDrawFunc( work , win , 0,0, msgIdArr[idx] );
}

//�������烁�j���[���`��
static void START_MENU_ITEM_ContinueDraw( START_MENU_WORK *work , GFL_BMPWIN *win , const u8 idx )
{
	u8 lCol,sCol,bCol;
	WORDSET *word;
	STRBUF *nameStr;
	const STRCODE *myname = MyStatus_GetMyName( work->mystatus_ );

	word = WORDSET_Create( work->heapId_ );
	//Font�F��߂����ߎ���Ă���
	GFL_FONTSYS_GetColor( &lCol,&sCol,&bCol );

	START_MENU_ITEM_MsgDrawFunc( work , win ,  0, 0, START_MENU_STR_ITEM_01_01 );

	GFL_FONTSYS_SetColor( 5,sCol,bCol );
	START_MENU_ITEM_MsgDrawFunc( work , win , 32,16, START_MENU_STR_ITEM_01_02 );
	START_MENU_ITEM_MsgDrawFunc( work , win , 32,32, START_MENU_STR_ITEM_01_03 );
	START_MENU_ITEM_MsgDrawFunc( work , win , 32,48, START_MENU_STR_ITEM_01_04 );
	START_MENU_ITEM_MsgDrawFunc( work , win , 32,64, START_MENU_STR_ITEM_01_05 );

	//���O	
	nameStr = 	GFL_STR_CreateBuffer( 16, work->heapId_ );
	if( MyStatus_CheckNameClear( work->mystatus_ ) == FALSE )
	{
		GFL_STR_SetStringCode( nameStr , myname );
	}
	else
	{
		//TODO �O�̂��ߖ��O�������ĂȂ��Ƃ��ɗ����Ȃ��悤�ɂ��Ă���
		u16 tempName[7] = { L'N',L'o',L'N',L'a',L'm',L'e',0xFFFF };
		GFL_STR_SetStringCode( nameStr , tempName );
	}
	WORDSET_RegisterWord( word, 0, nameStr, 0, TRUE , 0 );
	START_MENU_ITEM_WordMsgDrawFunc( work , win ,192,16, START_MENU_STR_ITEM_01_06 , word);

	//�v���C����
	WORDSET_RegisterNumber( word, 0, 9, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
	WORDSET_RegisterNumber( word, 1, 2, 2, STR_NUM_DISP_ZERO , STR_NUM_CODE_HANKAKU );
	START_MENU_ITEM_WordMsgDrawFunc( work , win ,192,32, START_MENU_STR_ITEM_01_07 , word);
	//�|�P�����}��
	WORDSET_RegisterNumber( word, 0, 55, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
	START_MENU_ITEM_WordMsgDrawFunc( work , win ,192,48, START_MENU_STR_ITEM_01_08 , word);
	//�o�b�W
	WORDSET_RegisterNumber( word, 0, MyStatus_GetBadgeCount(work->mystatus_), 1, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
	START_MENU_ITEM_WordMsgDrawFunc( work , win ,192,64, START_MENU_STR_ITEM_01_09 , word);

	//Font�F��߂�
	GFL_FONTSYS_SetColor( lCol,sCol,bCol );
	
	GFL_STR_DeleteBuffer( nameStr );
	WORDSET_Delete( word );
}

static void START_MENU_ITEM_MsgDrawFunc( START_MENU_WORK *work , GFL_BMPWIN *win , u8 posX , u8 posY , u16 msgId )
{
	STRBUF  *str = GFL_STR_CreateBuffer( 96 , work->heapId_ );
	GFL_MSG_GetString( work->msgMng_ , msgId , str );
	PRINTSYS_Print( GFL_BMPWIN_GetBmp(win),
					posX+START_MENU_FONT_LEFT,
					posY+START_MENU_FONT_TOP,
					str,
					work->fontHandle_);
	
	GFL_STR_DeleteBuffer( str );
}

static void START_MENU_ITEM_WordMsgDrawFunc( START_MENU_WORK *work , GFL_BMPWIN *win , u8 posX , u8 posY , u16 msgId , WORDSET *word)
{
	u8 strLen;
	STRBUF  *str = GFL_STR_CreateBuffer( 96 , work->heapId_ );
	STRBUF  *baseStr = GFL_STR_CreateBuffer( 96 , work->heapId_ );
	GFL_MSG_GetString( work->msgMng_ , msgId , baseStr );
	WORDSET_ExpandStr( word , str , baseStr );

	strLen = PRINTSYS_GetStrWidth( str , work->fontHandle_ , 0 );
	
	PRINTSYS_Print( GFL_BMPWIN_GetBmp(win),
					posX+START_MENU_FONT_LEFT - strLen,
					posY+START_MENU_FONT_TOP,
					str,
					work->fontHandle_);
	
	GFL_STR_DeleteBuffer( baseStr );
	GFL_STR_DeleteBuffer( str );
	
}
