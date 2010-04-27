//============================================================================================
/**
	* @file	pms_input_view.h
	* @bfief	�ȈՉ�b���͉�ʁi�`�惁�C���|�������Q�ƃw�b�_�j
	* @author	taya
	* @date	06.02.07
	*/
//============================================================================================
#ifndef __PMS_INPUT_VIEW_H__
#define __PMS_INPUT_VIEW_H__

#include "pmsi.naix"

#define  PMSI_FPS_60		// ���ꂪ�L�����Ɗe��G�t�F�N�g��60fps�œ�����

#ifdef PMSI_FPS_60
#define  PMSI_FRAME_RATIO	(2)
#else
#define  PMSI_FRAME_RATIO	(1)
#endif

#define  PMSI_FRAMES(n)		((n)*PMSI_FRAME_RATIO)
#define  PMSI_ANM_SPEED		((FX32_ONE*2)/PMSI_FRAME_RATIO)

//============================================================================================
//============================================================================================


//------------------------------------------------
/**
	* �t���[������U��
	*/ 
//------------------------------------------------
enum {
	FRM_MAIN_EDITAREA =  GFL_BG_FRAME0_M,
	FRM_MAIN_CATEGORY =  GFL_BG_FRAME1_M,
	FRM_MAIN_WORDWIN  =  GFL_BG_FRAME2_M,
	FRM_MAIN_BACK	    =  GFL_BG_FRAME3_M,

	FRM_MAIN_BUTTON   =  GFL_BG_FRAME0_M,
  FRM_MAIN_TASKMENU =  GFL_BG_FRAME0_M,
  FRM_MAIN_BAR      =  GFL_BG_FRAME3_M,

	FRM_SUB_EDITAREA = GFL_BG_FRAME0_S,
	FRM_SUB_BG = GFL_BG_FRAME1_S,
  FRM_SUB_SEARCH_LIST = GFL_BG_FRAME2_S,
  FRM_SUB_EXPLAIN  = GFL_BG_FRAME3_S,

	FRM_MAIN_EDITAREA_BLENDMASK = GX_BLEND_PLANEMASK_BG0,
	FRM_MAIN_CATEGORY_BLENDMASK = GX_BLEND_PLANEMASK_BG1,
	FRM_MAIN_WORDWIN_BLENDMASK  = GX_BLEND_PLANEMASK_BG2,
	FRM_MAIN_BUTTON_BLENDMASK   = GX_BLEND_PLANEMASK_BG3,

	FRM_MAIN_EDITAREA_WNDMASK  = GX_WND_PLANEMASK_BG0,
	FRM_MAIN_CATEGORY_WNDMASK  = GX_WND_PLANEMASK_BG1,
	FRM_MAIN_WORDWIN_WNDMASK   = GX_WND_PLANEMASK_BG2,
	FRM_MAIN_BUTTON_WNDMASK    = GX_WND_PLANEMASK_BG3,

};
//------------------------------------------------
/**
	* BG�p���b�g����U��
	*/ 
//------------------------------------------------
enum {
	PALNUM_MAIN_EDITAREA = 0x00,
	PALNUM_MAIN_YESNO	 = 0x03, //@TODO ����
	PALNUM_MAIN_TASKMENU = 0x08,
	PALNUM_MAIN_TOUCHBAR = 0x0a,
  PALNUM_MAIN_CATEGORY = 0x0c,
	PALNUM_MAIN_WORDWIN  = 0x0d,
	PALNUM_MAIN_SYSWIN   = 0x0e,
	PALNUM_MAIN_TALKWIN  = 0x0f,
	
  PALNUM_MAIN_BUTTON = 0x0a, // @TODO �g���ĂȂ��̂ŏ���

  PALNUM_MAIN_TASKMENU_PASSIVE  = 0x0e,

  // �T�u��ʂ�BG�̃p���b�g��RAM�̈ʒu
  PALNUM_SUB_ALL         = 0,  // �S��(5�{)
  PALNUM_SUB_EXPLAIN     = 5,  // ���ʐ�����(1�{)
  // �T�u��ʂ�BG�̃p���b�g�̖{��
  PALAMOUNT_SUB_ALL      = 5,  // �S��(5�{)
  PALAMOUNT_SUB_EXPLAIN  = 1,  // ���ʐ�����(1�{)
  // �T�u��ʂ�BG�̃p���b�g�̓ǂݍ��݊J�n�ʒu
  PALSRCPOS_SUB_ALL      = 0,  // �S��(5�{)
  PALSRCPOS_SUB_EXPLAIN  = 4,  // ���ʐ�����(���C��4�Ԃ̃p���b�g1�{�����g��)
};
//------------------------------------------------
/**
	* OBJ�p���b�g����U��
	*/ 
//------------------------------------------------
enum
{ 
//PALNUM_OBJ_M_MENU   = 0x2, 
//PALLINE_NUM_OBJ_M_MENU = 3,   ///< 3�{�g�p

  PALNUM_OBJ_M_COMMON   = 0x0, ///< 6�{�g�p(��OBJ)

  PALNUM_OBJ_M_PMSDRAW  = 0x7, ///< 5�{�g�p
  PALNUM_OBJ_M_TOUCHBAR = 0xc, ///< 3�{�g�p
  
  PALNUM_OBJ_S_PMSDRAW  = 0x7, ///< 5�{�g�p
};

//------------------------------------------------
/**
	* �g�p�t�H���g
	*/ 
//------------------------------------------------

enum {
	PMSI_FONT_EDITAREA_BASE =	0,	//FONT_TALK,
	PMSI_FONT_EDITAREA_WORD =	0,	//FONT_TALK,
	PMSI_FONT_BUTTON =			1,	//FONT_SYSTEM,
	PMSI_FONT_CATEGORY =		1,	//FONT_SYSTEM,
	PMSI_FONT_WORDWIN =			1,	//FONT_SYSTEM,
	PMSI_FONT_MESSAGE =			0,	//FONT_TALK,
	PMSI_FONT_YESNO =			1,	//FONT_SYSTEM,
};


//------------------------------------------------
/**
	* �A�v���X�e�[�^�X
	*/ 
//------------------------------------------------
enum{
	PMSI_ST_EDIT,
	PMSI_ST_BUTTON,
};

//------------------------------------------------
/**
	* LCD
	*/ 
//------------------------------------------------
enum {
	PMSIV_LCD_MAIN = 0,
	PMSIV_LCD_SUB,
  PMSIV_LCD_MAX,
};

enum {
	BGPRI_MAIN_EDITAREA = 0,
	BGPRI_MAIN_CATEGORY,
	BGPRI_MAIN_WORDWIN,
	BGPRI_MAIN_BACK,
	BGPRI_MAIN_BUTTON = 0,

	BGPRI_SUB_BASE = 0,
};

enum {
	ACTPRI_EDITAREA_CURSOR,
	ACTPRI_EDITAREA_ARROW,
	ACTPRI_EDITAREA_DECO,
	ACTPRI_CATEGORY_CURSOR,
	ACTPRI_WORDWIN_CURSOR,
	ACTPRI_WORDWIN_ARROW,
	ACTPRI_BUTTON_CURSOR,
};

enum {
	ANM_EDITAREA_CURSOR_ACTIVE,
	ANM_EDITAREA_CURSOR_STOP,
	ANM_BUTTON_CURSOR_ACTIVE,
	ANM_BUTTON_CURSOR_STOP,
	ANM_CATEGORY_CURSOR_ACTIVE,
	ANM_CATEGORY_CURSOR_STOP,
	ANM_INITIAL_CURSOR_ACTIVE,
	ANM_INITIAL_CURSOR_STOP,
	ANM_WORDWIN_CURSOR_ACTIVE,
	ANM_WORDWIN_CURSOR_STOP,

	ANM_WORD_ARROW_DOWN,
	ANM_WORD_ARROW_UP,
	ANM_EDITAREA_ARROW_RIGHT,
	ANM_EDITAREA_ARROW_LEFT,

	ANM_EDITAREA_ALLCOVER_CURSOR_ACTIVE,
	ANM_EDITAREA_ALLCOVER_CURSOR_STOP,

	ANM_CATEGORY_BACK_CURSOR_ACTIVE,
	ANM_CATEGORY_BACK_CURSOR_STOP,
	
	ANM_EDITAREA_SCR_BTN,
	ANM_EDITAREA_SCR_L01,
	ANM_EDITAREA_SCR_L02,
	ANM_EDITAREA_SCR_R01,
	ANM_EDITAREA_SCR_R02,
	ANM_EDITAREA_SCR_BAR,

	ANM_WORD_SCR_U01,
	ANM_WORD_SCR_U02,
	ANM_WORD_SCR_D01,
	ANM_WORD_SCR_D02,

  ANM_WORDWIN_CURSOR_ON,
  ANM_EDITAREA_CURSOR_ON,
  ANM_INITIAL_CURSOR_ON,
  ANM_CATEGORY_CURSOR_ON,
  ANM_BUTTON_CURSOR_ON,
};

typedef struct
{
	u32 pltIdx;
	u32 ncgIdx;
	u32 anmIdx;
}PMSIV_CELL_RES;

//==============================================================================================
// ���������W���[���ւ̏���
//==============================================================================================
extern GFL_TCB* PMSIView_AddVTask( GFL_TCB_FUNC func, void* wk, int pri );
extern GFL_CLUNIT* PMSIView_GetCellUnit( PMS_INPUT_VIEW* vwk );
extern GFL_FONT*  PMSIView_GetFontHandle( PMS_INPUT_VIEW* vwk );
extern PRINT_QUE* PMSIView_GetPrintQue( PMS_INPUT_VIEW* vwk );
//extern NNSG2dImageProxy*  PMSIView_GetObjImageProxy( PMS_INPUT_VIEW* vwk, int lcd );
//extern NNSG2dImagePaletteProxy*  PMSIView_GetObjPaletteProxy( PMS_INPUT_VIEW* vwk, int lcd );
extern void PMSIView_SetupDefaultActHeader( PMS_INPUT_VIEW* vwk, PMSIV_CELL_RES* header, u32 lcd, u32 bgpri );
extern GFL_CLWK* PMSIView_AddActor( PMS_INPUT_VIEW* vwk, PMSIV_CELL_RES* header, u32 x, u32 y, u32 actpri, int drawArea );
extern void PMSIView_GetDecoResource( PMS_INPUT_VIEW* vwk, PMSIV_CELL_RES* out_res, u32 lcd );

extern u32 PMSIView_GetScrollBarPos( PMS_INPUT_VIEW* vwk, u32 px, u32 py );
extern void PMSIView_SetScrollBarPos( PMS_INPUT_VIEW* vwk, u32 py );
extern u32 PMSIView_GetScrollBarPosCount( PMS_INPUT_VIEW* vwk, u32 max );

extern void PMSIView_SetBackScreen( PMS_INPUT_VIEW* vwk, BOOL is_wordwin );

extern void PMSIView_ChangeKTEditArea( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk );
extern void PMSIView_ChangeKTCategory( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk );
extern void PMSIView_ChangeKTWordWin( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk );


//================================================================================
//  ���͗�����
//================================================================================
typedef struct _PMSIV_EDIT		PMSIV_EDIT;

extern PMSIV_EDIT*  PMSIV_EDIT_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk );
extern void PMSIV_EDIT_Delete( PMSIV_EDIT* wk );
extern void PMSIV_EDIT_SetupGraphicDatas( PMSIV_EDIT* wk, ARCHANDLE* p_handle );
extern void PMSIV_EDIT_UpdateEditArea( PMSIV_EDIT* wk );
extern void PMSIV_EDIT_ScrollSet( PMSIV_EDIT* wk,u8 scr_dir);
extern int PMSIV_EDIT_ScrollWait( PMSIV_EDIT* wk);
extern void PMSIV_EDIT_GetSentenceWordPos( PMSIV_EDIT* wk ,GFL_UI_TP_HITTBL* tbl,u8 idx);

extern void PMSIV_EDIT_StopCursor( PMSIV_EDIT* wk );
extern void PMSIV_EDIT_ActiveCursor( PMSIV_EDIT* wk );
extern void PMSIV_EDIT_MoveCursor( PMSIV_EDIT* wk, int pos );
extern void PMSIV_EDIT_VisibleCursor( PMSIV_EDIT* wk, BOOL flag );

extern void PMSIV_EDIT_ChangeSMsgWin(PMSIV_EDIT* wk,u8 mode);
extern void PMSIV_EDIT_StopArrow( PMSIV_EDIT* wk );
extern void PMSIV_EDIT_ActiveArrow( PMSIV_EDIT* wk );


extern u32 PMSIV_EDIT_GetWordPosMax( const PMSIV_EDIT* wk );

//================================================================================
//  �R�}���h�{�^������
//================================================================================
typedef struct _PMSIV_BUTTON	PMSIV_BUTTON;

extern PMSIV_BUTTON*  PMSIV_BUTTON_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk );
extern void PMSIV_BUTTON_Delete( PMSIV_BUTTON* wk );
extern void PMSIV_BUTTON_SetupGraphicDatas( PMSIV_BUTTON* wk, ARCHANDLE* p_handle );
/**
	*	@brief	�����Ă�/��߂�@�{�^���̕`���ԃR���g���[��
	*	@param	draw_f	�{�^���{�̂̕`��On/Off
	*	@param	cur_f	�I���J�[�\���̕`��On/Off
	*/
extern void PMSIV_BUTTON_UpdateButton( PMSIV_BUTTON* wk,BOOL draw_f,BOOL cur_f);
extern void PMSIV_BUTTON_UpdateRetButton( PMSIV_BUTTON* wk,u8 draw_mode);

extern void PMSIV_BUTTON_StopCursor( PMSIV_BUTTON* wk );
extern void PMSIV_BUTTON_ActiveCursor( PMSIV_BUTTON* wk );
extern void PMSIV_BUTTON_MoveCursor( PMSIV_BUTTON* wk, int pos );
extern void PMSIV_BUTTON_VisibleCursor( PMSIV_BUTTON* wk, BOOL flag );
extern BOOL PMSIV_BUTTON_GetCursorVisibleFlag( PMSIV_BUTTON* wk );

extern void PMSIV_BUTTON_ChangeCategoryButton(PMSIV_BUTTON* wk);
extern BOOL PMSIV_BUTTON_WaitChangeCategoryButton( PMSIV_BUTTON* wk );

extern void PMSIV_BUTTON_Hide( PMSIV_BUTTON* wk );
extern void PMSIV_BUTTON_Appear( PMSIV_BUTTON* wk );

//================================================================================
//  �J�e�S���E�B���h�E����
//================================================================================
typedef struct _PMSIV_CATEGORY	PMSIV_CATEGORY;

extern PMSIV_CATEGORY*  PMSIV_CATEGORY_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk );
extern void PMSIV_CATEGORY_Delete( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_SetupGraphicDatas( PMSIV_CATEGORY* wk, ARCHANDLE* p_handle );
extern void PMSIV_CATEGORY_StartEnableBG( PMSIV_CATEGORY* wk );
extern BOOL PMSIV_CATEGORY_WaitEnableBG( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_StartDisableBG( PMSIV_CATEGORY* wk );
extern BOOL PMSIV_CATEGORY_WaitDisableBG( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_VisibleCursor( PMSIV_CATEGORY* wk, BOOL flag );
extern void PMSIV_CATEGORY_MoveCursor( PMSIV_CATEGORY* wk, u32 pos );
extern void PMSIV_CATEGORY_StartFadeOut( PMSIV_CATEGORY* wk );
extern BOOL PMSIV_CATEGORY_WaitFadeOut( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_StartFadeIn( PMSIV_CATEGORY* wk );
extern BOOL PMSIV_CATEGORY_WaitFadeIn( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_SetDisableBG( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_ChangeModeBG( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_StartModeChange( PMSIV_CATEGORY* wk );
extern BOOL PMSIV_CATEGORY_WaitModeChange( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_StartBrightDown( PMSIV_CATEGORY* wk );
extern BOOL PMSIV_CATEGORY_WaitBrightDown( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_InputWordUpdate( PMSIV_CATEGORY* wk );
extern void PMSIV_CATEGORY_StartMoveSubWinList( PMSIV_CATEGORY* wk, BOOL is_enable );
extern BOOL PMSIV_CATEGORY_WaitMoveSubWinList( PMSIV_CATEGORY* wk, BOOL is_enable );

extern void PMSIV_CATEGORY_InputBlink( PMSIV_CATEGORY* wk, u32 pos );
extern BOOL PMSIV_CATEGORY_WaitInputBlink( PMSIV_CATEGORY* wk );


//================================================================================
//  �P��E�B���h�E����
//================================================================================
typedef struct _PMSIV_WORDWIN	PMSIV_WORDWIN;

// �X�N���[���o�[�̃T�C�Y
enum {
	PMSIV_TPWD_RAIL_PX = 236,
	PMSIV_TPWD_RAIL_PY = 0,
	PMSIV_TPWD_RAIL_SX = 16,
	PMSIV_TPWD_RAIL_SY = 152+8 -4,//168,

	PMSIV_TPWD_BAR_SX = 16,
	PMSIV_TPWD_BAR_SY = 14,//24,
};


extern PMSIV_WORDWIN*  PMSIV_WORDWIN_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk );
extern void PMSIV_WORDWIN_Delete( PMSIV_WORDWIN* wk );
extern void PMSIV_WORDWIN_SetupGraphicDatas( PMSIV_WORDWIN* wk );
extern void PMSIV_WORDWIN_SetupWord( PMSIV_WORDWIN* wk );
extern void PMSIV_WORDWIN_SetupWordBar( PMSIV_WORDWIN* wk, int idx );
extern void PMSIV_WORDWIN_StartFadeIn( PMSIV_WORDWIN* wk );
extern BOOL PMSIV_WORDWIN_WaitFadeIn( PMSIV_WORDWIN* wk );
extern void PMSIV_WORDWIN_VisibleCursor( PMSIV_WORDWIN* wk, BOOL flag );
extern void PMSIV_WORDWIN_MoveCursor( PMSIV_WORDWIN* wk, u32 pos );
extern void PMSIV_WORDWIN_StartScroll( PMSIV_WORDWIN* wk, int vector );
extern BOOL PMSIV_WORDWIN_WaitScroll( PMSIV_WORDWIN* wk );
extern void PMSIV_WORDWIN_StartFadeOut( PMSIV_WORDWIN* wk );
extern BOOL PMSIV_WORDWIN_WaitFadeOut( PMSIV_WORDWIN* wk );

extern void PMSIV_WORDWIN_GetScrollBarPos( PMSIV_WORDWIN * wk, GFL_CLACTPOS * pos );
extern void PMSIV_WORDWIN_MoveScrollBar( PMSIV_WORDWIN * wk, u32 py );
extern u32 PMSIV_WORDWIN_GetScrollBarPosCount( PMSIV_WORDWIN * wk, u32 max );
extern void PMSIV_WORDWIN_SetScrollBarPos( PMSIV_WORDWIN * wk, u32 now, u32 max );

extern void PMSIV_WORDWIN_InputBlink( PMSIV_WORDWIN* wk, u32 pos );
extern BOOL PMSIV_WORDWIN_WaitInputBlink( PMSIV_WORDWIN* wk );


//================================================================================
//  �T�u��ʏ���
//================================================================================
typedef struct _PMSIV_SUB	PMSIV_SUB;

enum {
	SUB_BUTTON_UP,
	SUB_BUTTON_DOWN,
};
enum {
	SUB_BUTTON_STATE_RELEASE,
	SUB_BUTTON_STATE_HOLD,
};

extern PMSIV_SUB*  PMSIV_SUB_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk );
extern void PMSIV_SUB_Delete( PMSIV_SUB* wk );
extern void PMSIV_SUB_SetupGraphicDatas( PMSIV_SUB* wk, ARCHANDLE* p_handle );
extern void PMSIV_SUB_ChangeCategoryButton( PMSIV_SUB* wk );
extern BOOL PMSIV_SUB_WaitChangeCategoryButton( PMSIV_SUB* wk );
extern void PMSIV_SUB_VisibleArrowButton( PMSIV_SUB* wk, BOOL flag );
extern void PMSIV_SUB_ChangeArrowButton( PMSIV_SUB* wk, int pos, int state );

//==============================================================
// �^�X�N���j���[�Ǘ����W���[��
//==============================================================
typedef struct _PMSIV_MENU PMSIV_MENU;


//-----------------------------
///	����ID
//=============================
typedef enum
{ 
  CATEGORY_DECIDE_ID_CANCEL = 0,  ///< �L�����Z��
  CATEGORY_DECIDE_ID_CHANGE,  ///< �ؑփ{�^��
  CATEGORY_DECIDE_ID_ERASE,  ///< �����{�^��
  CATEGORY_DECIDE_ID_SEARCH,  ///< �����{�^��
  CATEGORY_DECIDE_ID_MAX,
} CATEGORY_DECIDE_ID;

typedef enum
{ 
  EDIT_BUTTON_ID_LEFT = 0, ///< ���{�^��
  EDIT_BUTTON_ID_RIGHT,    ///< �E�{�^��
  EDIT_BUTTON_ID_MAX,
} EDIT_BUTTON_ID;

extern PMSIV_MENU* PMSIV_MENU_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk );
extern void PMSIV_MENU_Delete( PMSIV_MENU* wk );
extern void PMSIV_MENU_Main( PMSIV_MENU* wk );
extern void PMSIV_MENU_Clear( PMSIV_MENU* wk );
extern void PMSIV_MENU_SetupEdit( PMSIV_MENU* wk );
extern void PMSIV_MENU_SetupCategory( PMSIV_MENU* wk );
extern void PMSIV_MENU_SetupWordWin( PMSIV_MENU* wk );
extern void PMSIV_MENU_UpdateEditIcon( PMSIV_MENU* wk );
extern void PMSIV_MENU_UpdateEditTaskMenu( PMSIV_MENU* wk );
extern void PMSIV_MENU_UpdateCategoryTaskMenu( PMSIV_MENU* wk );
extern void PMSIV_MENU_TaskMenuSetActive( PMSIV_MENU* wk, u8 pos, BOOL is_on );
extern BOOL PMSIV_MENU_TaskMenuIsFinish( PMSIV_MENU* wk, u8 pos );
extern void PMSIV_MENU_TaskMenuSetDecide( PMSIV_MENU* wk, u8 pos, BOOL is_on );
extern void PMSIV_MENU_SetDecideCategory( PMSIV_MENU* wk, CATEGORY_DECIDE_ID id );
extern BOOL PMSIV_MENU_IsFinishCategory( PMSIV_MENU* wk, CATEGORY_DECIDE_ID id );
extern void PMSIV_MENU_ResetDecideCategory( PMSIV_MENU* wk, CATEGORY_DECIDE_ID id );
extern void PMSIV_MENU_TouchEditButton( PMSIV_MENU* wk, EDIT_BUTTON_ID id );

extern PMSIV_MENU* PMSIView_GetMenuWork( PMS_INPUT_VIEW* vwk );


#endif
