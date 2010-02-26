//============================================================================================
/**
	* @file	pmsiv_category.c
	* @bfief	�ȈՉ�b���͉�ʁi�`�扺�����F�J�e�S���E�C�j�V�����j
	* @author	taya
	* @date	06.02.10
	*/
//============================================================================================
#include <gflib.h>

#include "system\main.h"
#include "system\pms_word.h"
#include "print\printsys.h"

#include "arc_def.h"
#include "message.naix"
#include "pmsi.naix"
#include "msg\msg_pms_category.h"

#include "pms_input_prv.h"
#include "pms_input_view.h"
#include "pmsiv_tool.h"
#include "pmsi_initial_data.h"



//======================================================================
enum {
	INPUTAREA_PALTYPE_MAX = 10,

	STR_TMPBUF_LEN = 128,	// �\���O�̃e���|�����Ƃ��Ďg�p�B�ő啶�����B

	HARD_WINDOW_TOP = 0,
	HARD_WINDOW_BOTTOM = 160,
	HARD_WINDOW_LEFT = 0,
	HARD_WINDOW_RIGHT = 255,
	HARD_WINDOW_BLDY = -10,

	BRIGHT_EFFECT_WAIT = PMSI_FRAMES(8),
};

enum {
	CATEGORY_WIN_WIDTH = 9,
	CATEGORY_WIN_HEIGHT = 2,
	CATEGORY_WIN_CHARSIZE = CATEGORY_WIN_WIDTH*CATEGORY_WIN_HEIGHT,

	CATEGORY_WIN_XORG = 1,
	CATEGORY_WIN_YORG = 9,
	CATEGORY_WIN_X_MARGIN = 1,
	CATEGORY_WIN_Y_MARGIN = 1,

	CATEGORY_WIN_ROWS = 3,

	INITIAL_WIN_XORG = 1,     ///< �L�����P��
	INITIAL_WIN_YORG = 8,     ///< �L�����P��
	INITIAL_WIN_WIDTH = 30,
	INITIAL_WIN_HEIGHT = 14,
	INITIAL_WIN_CHARSIZE = INITIAL_WIN_WIDTH*INITIAL_WIN_HEIGHT,
	
  SUBLIST_WIN_XORG = 1,     ///< �L�����P��
	SUBLIST_WIN_YORG = 1,     ///< �L�����P��
	SUBLIST_WIN_WIDTH = 30,
	SUBLIST_WIN_HEIGHT = 22,
	SUBLIST_WIN_CHARSIZE = SUBLIST_WIN_WIDTH*SUBLIST_WIN_HEIGHT,
  SUBLIST_WIN_STR_NUM = SUBLIST_WIN_HEIGHT / 2 * 2, ///< �ő區�ڐ�
  SUBLIST_WIN_SCROLL_SYNC = PMSI_FRAMES(4), ///< �X�N���[���ɂ�����SYNC

  INPUT_WIN_XORG    = 10,
  INPUT_WIN_YORG    = 3,
  INPUT_WIN_WIDTH   = 11,
  INPUT_WIN_HEIGHT  = 2,
	INPUT_WIN_CHARSIZE = INPUT_WIN_WIDTH * INPUT_WIN_HEIGHT,

#if 0
	BACK_WIN_WIDTH = 8,
	BACK_WIN_HEIGHT = 2,
	BACK_WIN_CHARSIZE = BACK_WIN_WIDTH*BACK_WIN_HEIGHT,
#endif

	CATEGORY_BACK_WIN_XORG = 12,
	CATEGORY_BACK_WIN_YORG = 21,
	INITIAL_BACK_WIN_XORG = 44,
	INITIAL_BACK_WIN_YORG = 21,

	// BG offset
#if 0	//pms_input_prv.h�Ɉړ�
	GROUPMODE_BG_XOFS = -4,
	INITIALMODE_BG_XOFS = 252,

//	CATEGORY_BG_DISABLE_YOFS = 2,
//	CATEGORY_BG_ENABLE_YOFS = 26,
	CATEGORY_BG_DISABLE_YOFS = 16,
	CATEGORY_BG_ENABLE_YOFS = 16,
#endif
	CATEGORY_BG_DISABLE_EFF_VECTOR = (CATEGORY_BG_DISABLE_YOFS - CATEGORY_BG_ENABLE_YOFS),
	CATEGORY_BG_ENABLE_EFF_VECTOR = (CATEGORY_BG_ENABLE_YOFS - CATEGORY_BG_DISABLE_YOFS),
	ENABLE_MOVE_FRAMES = PMSI_FRAMES(3),
	MODECHANGE_MOVE_FRAMES = PMSI_FRAMES(8),

	// OBJ pos
	CATEGORY_CURSOR_OX = ( (CATEGORY_WIN_XORG*8) + ((CATEGORY_WIN_WIDTH*8)/2) ) - GROUPMODE_BG_XOFS,
	CATEGORY_CURSOR_OY = ( (CATEGORY_WIN_YORG*8) + ((CATEGORY_WIN_HEIGHT*8)/2) ) - CATEGORY_BG_ENABLE_YOFS,
	CATEGORY_CURSOR_X_MARGIN = ((CATEGORY_WIN_WIDTH+CATEGORY_WIN_X_MARGIN)*8),
	CATEGORY_CURSOR_Y_MARGIN = ((CATEGORY_WIN_HEIGHT+CATEGORY_WIN_Y_MARGIN)*8),

	CATEGORY_CURSOR_BACK_XPOS = 28*8,//((CATEGORY_BACK_WIN_XORG*8)+ (CATEGORY_WIN_WIDTH*8)/2),
	CATEGORY_CURSOR_BACK_YPOS = 22*8,//((CATEGORY_BACK_WIN_YORG*8)+ (CATEGORY_WIN_HEIGHT*8)/2) - CATEGORY_BG_ENABLE_YOFS,

	INITIAL_CURSOR_OX = ((INITIAL_WIN_XORG)*8-GROUPMODE_BG_XOFS) + 6,
	INITIAL_CURSOR_OY = ((INITIAL_WIN_YORG*8)-CATEGORY_BG_ENABLE_YOFS) + 8,
};

enum {
	CATEGORY_WIN_COL_LETTER = 0x01,
	CATEGORY_WIN_COL_SHADOW = 0x02,
	CATEGORY_WIN_COL_GROUND = 0x0f,

  CATEGORY_INPUT_WIN_COL_L = 0xF,
  CATEGORY_INPUT_WIN_COL_S = 0xE,
  CATEGORY_INPUT_WIN_COL_B = 0x1,

  CATEGORY_SUBLIST_WIN_COL_L = 0x1,
  CATEGORY_SUBLIST_WIN_COL_S = 0x2,
  CATEGORY_SUBLIST_WIN_COL_B = 0xF,

	CATEGORY_WIN_UNKNOWN_COL_LETTER = 0x03,
	CATEGORY_WIN_UNKNOWN_COL_SHADOW = 0x04,
};

static const u8 msg_pms_category_str_id[CATEGORY_GROUP_MAX] =
{
  pms_category_01,
  pms_category_02,
  pms_category_03,
  pms_category_04,
  pms_category_05,
  pms_category_06,
  pms_category_13,
  pms_category_07,
  pms_category_08,
  pms_category_09,
  pms_category_11,
  pms_category_10,
};


//======================================================================

//======================================================================
typedef struct {
	void*         parent_work;

	fx32   pos;
	fx32   end_pos;
	fx32   add_value;
	u32    timer;
}SCROLL_WORK;

//--------------------------------------------------------------
/**
	*	
	*/
//--------------------------------------------------------------
struct _PMSIV_CATEGORY {
	PMS_INPUT_VIEW*        vwk;
	const PMS_INPUT_WORK*  mwk;
	const PMS_INPUT_DATA*  dwk;

	u32            mode;
	u32            eff_seq;

	GFL_TCB        *effect_task;
	PMSIV_TOOL_BLEND_WORK   blend_work;
	PMSIV_TOOL_SCROLL_WORK  scroll_work;

	GFL_CLWK		*cursor_actor;
  BOOL        input_blink_cursor_visible;  // InputBlick���s���Ƃ���cursor_actor�̕\�����o���Ă���

	int*			p_key_mode;
	
	GFL_BMPWIN		*winGroup[CATEGORY_GROUP_MAX];
	GFL_BMPWIN		*winInitial;
  GFL_BMPWIN    *winInput;
  GFL_BMPWIN    *winSubList;
  PRINT_UTIL    printSubList;
};


//==============================================================
// Prototype
//==============================================================
static u32 setup_group_window( PMSIV_CATEGORY* wk, u32 charpos );
static u32 setup_initial_window( PMSIV_CATEGORY* wk, u32 charpos );
static u32 setup_input_window( PMSIV_CATEGORY* wk, u32 charpos );
static void setup_sublist_window( PMSIV_CATEGORY* wk );
static void setup_actor( PMSIV_CATEGORY* wk );


//------------------------------------------------------------------
/**
	* 
	*
	* @param   vwk		
	* @param   mwk		
	* @param   dwk		
	*
	* @retval  PMSIV_CATEGORY*		
	*/
//------------------------------------------------------------------
PMSIV_CATEGORY*  PMSIV_CATEGORY_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk )
{
	PMSIV_CATEGORY*  wk = GFL_HEAP_AllocMemory( HEAPID_PMS_INPUT_VIEW, sizeof(PMSIV_CATEGORY) );

	wk->vwk = vwk;
	wk->mwk = mwk;
	wk->dwk = dwk;

	wk->mode = CATEGORY_MODE_GROUP;

	wk->effect_task = NULL;

	wk->cursor_actor = NULL;
	wk->input_blink_cursor_visible = FALSE;

	wk->p_key_mode = PMSI_GetKTModePointer(wk->mwk);
	return wk;
}
//------------------------------------------------------------------
/**
	* 
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_Delete( PMSIV_CATEGORY* wk )
{
	u8 i;
	if( wk->cursor_actor )
	{
		GFL_CLACT_WK_Remove( wk->cursor_actor );
	}
	for( i=0;i<CATEGORY_GROUP_MAX;i++ )
	{
		if( wk->winGroup[i] != NULL )
		{
			GFL_BMPWIN_Delete( wk->winGroup[i] );
		}
	}
	if( wk->winInitial != NULL )
	{
		GFL_BMPWIN_Delete( wk->winInitial );
	}
  if( wk->winInput != NULL )
  {
		GFL_BMPWIN_Delete( wk->winInput );
  }
  if( wk->winSubList != NULL )
  {
		GFL_BMPWIN_Delete( wk->winSubList );
  }
	GFL_HEAP_FreeMemory( wk );
}


//-----------------------------------------------------------------------------
/**
 *	@brief  �J�e�S�� �O���t�B�b�N������
 *
 *	@param	PMSIV_CATEGORY* wk
 *	@param	p_handle 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_CATEGORY_SetupGraphicDatas( PMSIV_CATEGORY* wk, ARCHANDLE* p_handle )
{
	u32 charpos;
	const u16 GX_WND_PLANEMASK_ALL = (	GX_WND_PLANEMASK_BG0|
										GX_WND_PLANEMASK_BG1|
										GX_WND_PLANEMASK_BG2|
										GX_WND_PLANEMASK_BG3|
										GX_WND_PLANEMASK_OBJ);

	GFL_ARCHDL_UTIL_TransVramScreen(p_handle, NARC_pmsi_pms_bg_main1_NSCR,
		FRM_MAIN_CATEGORY, 0, 0, FALSE, HEAPID_PMS_INPUT_VIEW );

	charpos = GFL_ARCHDL_UTIL_TransVramBgCharacter(p_handle, NARC_pmsi_pms_bg_main1_NCGR,
		FRM_MAIN_CATEGORY, 0, 0, FALSE, HEAPID_PMS_INPUT_VIEW );
	charpos /= 0x20;

  HOSAKA_Printf("BG FRM_MAIN_CATEGORY base charpos=%d \n", charpos);

	//TODO �����̓L�����̃A�h���X���Œ肵�Ă��Ȃ��ƁA�����ɏ㏑������邩��
	//BMPWIN��ێ�����悤�ɕύX
	charpos = setup_group_window( wk, charpos );
  
  HOSAKA_Printf("BG FRM_MAIN_CATEGORY group charpos=%d \n", charpos);
	
  charpos = setup_initial_window( wk, charpos );
  
  HOSAKA_Printf("BG FRM_MAIN_CATEGORY initial charpos=%d \n", charpos);

  charpos = setup_input_window( wk, charpos );
  
  HOSAKA_Printf("BG FRM_MAIN_CATEGORY inputword charpos=%d \n", charpos);

  // SUB��ʓ��͌�⃊�X�g������
  setup_sublist_window( wk );

	GFL_BG_SetScroll( FRM_MAIN_CATEGORY, GFL_BG_SCROLL_X_SET, GROUPMODE_BG_XOFS );
	GFL_BG_SetScroll( FRM_MAIN_CATEGORY, GFL_BG_SCROLL_Y_SET, CATEGORY_BG_DISABLE_YOFS );

	setup_actor( wk );

	// �I���ł��Ȃ���ԂƂ������Ƃ��������߁A�P�x�𗎂Ƃ��Ă���
	G2_SetBlendBrightness( FRM_MAIN_CATEGORY_BLENDMASK, HARD_WINDOW_BLDY );
	G2_SetWnd0InsidePlane(GX_WND_PLANEMASK_ALL, TRUE);
	G2_SetWndOutsidePlane(GX_WND_PLANEMASK_ALL^FRM_MAIN_CATEGORY_WNDMASK, TRUE);
	G2_SetWnd0Position(0,0,255,168);
	GX_SetVisibleWnd( GX_WNDMASK_W0 );

	GFL_BG_LoadScreenReq( FRM_MAIN_CATEGORY );
}


//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	PMSIV_CATEGORY* wk
 *	@param	charpos 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u32 setup_group_window( PMSIV_CATEGORY* wk, u32 charpos )
{
	int i, x, y;
	GFL_BMPWIN *win;
	GFL_MSGDATA* msgman;
	STRBUF* str;
	u32   print_color, print_xpos;
	GFL_FONT *fontHandle;

	fontHandle = PMSIView_GetFontHandle(wk->vwk);
	msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_pms_category_dat, HEAPID_PMS_INPUT_VIEW );

	x = CATEGORY_WIN_XORG;
	y = CATEGORY_WIN_YORG;

	for(i=0; i<CATEGORY_GROUP_MAX; i++)
	{
		if( (i!=0) && (i%CATEGORY_WIN_ROWS==0) )
		{
			x = CATEGORY_WIN_XORG;
			y += (CATEGORY_WIN_HEIGHT + CATEGORY_WIN_Y_MARGIN);
		}

		if( PMSI_DATA_GetGroupEnableWordCount(wk->dwk, i) )
		{
			str = GFL_MSG_CreateString( msgman, msg_pms_category_str_id[i] );
			GFL_FONTSYS_SetColor( CATEGORY_WIN_COL_LETTER, CATEGORY_WIN_COL_SHADOW, CATEGORY_WIN_COL_GROUND );
		}
		else
		{
			str = GFL_MSG_CreateString( msgman, pms_category_unknown );
			GFL_FONTSYS_SetColor( CATEGORY_WIN_UNKNOWN_COL_LETTER, CATEGORY_WIN_UNKNOWN_COL_SHADOW, CATEGORY_WIN_COL_GROUND );
		}

		GF_ASSERT(charpos < (1024-CATEGORY_WIN_CHARSIZE));
		win = GFL_BMPWIN_Create( FRM_MAIN_CATEGORY, x, y, 
						CATEGORY_WIN_WIDTH, CATEGORY_WIN_HEIGHT, 
						PALNUM_MAIN_CATEGORY,GFL_BMP_CHRAREA_GET_B );

		GFL_BMP_Clear(GFL_BMPWIN_GetBmp(win),CATEGORY_WIN_COL_GROUND);

		print_xpos = ((CATEGORY_WIN_WIDTH*8)-PRINTSYS_GetStrWidth( str,fontHandle, 0))/2;
		PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), print_xpos, 0, str,fontHandle );

    // �f�t�H���g�̓O���[�v���[�h�̂��߁A�����Ɠ����ɓ]��
		GFL_BMPWIN_MakeScreen( win );
		GFL_BMPWIN_TransVramCharacter( win );

		GFL_STR_DeleteBuffer(str);
		wk->winGroup[i] = win;

		charpos += CATEGORY_WIN_CHARSIZE;
		x += (CATEGORY_WIN_WIDTH+CATEGORY_WIN_X_MARGIN);
	}

	GFL_MSG_Delete(msgman);

	return charpos;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	PMSIV_CATEGORY* wk
 *	@param	charpos 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u32 setup_initial_window( PMSIV_CATEGORY* wk, u32 charpos )
{
	GFL_BMPWIN  *win;
	STRBUF* buf;
	u32 print_color, initial_max, i, x, y;
	GFL_FONT *fontHandle;
	fontHandle = PMSIView_GetFontHandle(wk->vwk);

	GF_ASSERT(charpos < (1024-INITIAL_WIN_CHARSIZE));

	win = GFL_BMPWIN_Create( FRM_MAIN_CATEGORY, INITIAL_WIN_XORG, INITIAL_WIN_YORG, 
					INITIAL_WIN_WIDTH, INITIAL_WIN_HEIGHT, 
					PALNUM_MAIN_CATEGORY,GFL_BMP_CHRAREA_GET_B );
	charpos += INITIAL_WIN_CHARSIZE;

	buf = GFL_STR_CreateBuffer(4, HEAPID_PMS_INPUT_VIEW);

	GFL_BMP_Clear(GFL_BMPWIN_GetBmp(win),CATEGORY_WIN_COL_GROUND);

	initial_max = PMSI_INITIAL_DAT_GetInitialMax();
	for(i=0; i<initial_max; i++)
	{
		PMSI_INITIAL_DAT_GetStr(i, buf);
		PMSI_INITIAL_DAT_GetPrintPos(i, &x, &y);

#if 0
		if( PMSI_DATA_GetInitialEnableWordCount(wk->dwk, i) )
#endif
		{
			GFL_FONTSYS_SetColor( CATEGORY_WIN_COL_LETTER, CATEGORY_WIN_COL_SHADOW, CATEGORY_WIN_COL_GROUND );
		}
#if 0
		else
		{
			GFL_FONTSYS_SetColor( CATEGORY_WIN_UNKNOWN_COL_LETTER, CATEGORY_WIN_UNKNOWN_COL_SHADOW, CATEGORY_WIN_COL_GROUND );
		}
#endif
		
		PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), x, y, buf,fontHandle );
	}

	  GFL_BMPWIN_TransVramCharacter(win);
//	GFL_BMPWIN_MakeScreen(win);

	GFL_STR_DeleteBuffer(buf);

	wk->winInitial = win;

	return charpos;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���̓E�B���h�E����
 *
 *	@param	PMSIV_CATEGORY* wk
 *	@param	charpos 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u32 setup_input_window( PMSIV_CATEGORY* wk, u32 charpos )
{
	GFL_BMPWIN  *win;
	
  GF_ASSERT(charpos < (1024-INPUT_WIN_CHARSIZE));

  win = GFL_BMPWIN_Create( FRM_MAIN_CATEGORY, INPUT_WIN_XORG, INPUT_WIN_YORG, 
					INPUT_WIN_WIDTH, INPUT_WIN_HEIGHT, 
					PALNUM_MAIN_CATEGORY,GFL_BMP_CHRAREA_GET_B );
  charpos += INPUT_WIN_CHARSIZE;

  // �܂��X�N���[���͍��Ȃ�
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win), CATEGORY_INPUT_WIN_COL_B);
  GFL_BMPWIN_TransVramCharacter(win);

	wk->winInput = win;

  return charpos;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �P���⃊�X�g
 *
 *	@param	PMSIV_CATEGORY* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void setup_sublist_window( PMSIV_CATEGORY* wk )
{
	GFL_BMPWIN  *win;
	STRBUF* buf;
	
  win = GFL_BMPWIN_Create( FRM_SUB_SEARCH_LIST, SUBLIST_WIN_XORG, SUBLIST_WIN_YORG, 
					SUBLIST_WIN_WIDTH, SUBLIST_WIN_HEIGHT, 
					0, GFL_BMP_CHRAREA_GET_B );

  GFL_BMPWIN_MakeScreen( win );
  GFL_BG_LoadScreenReq( FRM_SUB_SEARCH_LIST );
  
  // �������W�ݒ�
  GFL_BG_SetScroll( FRM_SUB_SEARCH_LIST, GFL_BG_SCROLL_Y_SET, -GX_LCD_SIZE_Y ); 

  wk->winSubList = win;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	PMSIV_CATEGORY* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void setup_actor( PMSIV_CATEGORY* wk )
{
	PMSIV_CELL_RES  header;

	PMSIView_SetupDefaultActHeader( wk->vwk, &header, PMSIV_LCD_MAIN, /*BGPRI_MAIN_CATEGORY*/BGPRI_MAIN_EDITAREA);
	wk->cursor_actor = PMSIView_AddActor( wk->vwk, &header, CATEGORY_CURSOR_OX, CATEGORY_CURSOR_OY,
			ACTPRI_CATEGORY_CURSOR, NNS_G2D_VRAM_TYPE_2DMAIN );

	GFL_CLACT_WK_SetAnmSeq( wk->cursor_actor, ANM_CATEGORY_CURSOR_ACTIVE );
	GFL_CLACT_WK_SetDrawEnable( wk->cursor_actor, FALSE );
}


//======================================================================================
//======================================================================================

//------------------------------------------------------------------
/**
	* �J�[�\���\���I���I�t
	*
	* @param   wk		
	* @param   flag		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_VisibleCursor( PMSIV_CATEGORY* wk, BOOL flag )
{

	if( flag )
	{
		if(*wk->p_key_mode == GFL_APP_KTST_KEY){
			GFL_CLACT_WK_SetDrawEnable( wk->cursor_actor, TRUE );
			if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_GROUP ){
				GFL_CLACT_WK_SetAnmSeq( wk->cursor_actor, ANM_CATEGORY_CURSOR_ACTIVE );
			}else{
				GFL_CLACT_WK_SetAnmSeq( wk->cursor_actor, ANM_INITIAL_CURSOR_ACTIVE );
			}
		}else{
			GFL_CLACT_WK_SetDrawEnable( wk->cursor_actor, FALSE );
		}
	}else{
		GFL_CLACT_WK_SetDrawEnable( wk->cursor_actor, FALSE );
	}
}

//------------------------------------------------------------------
/**
	* �J�[�\���ړ�
	*
	* @param   wk		
	* @param   pos		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_MoveCursor( PMSIV_CATEGORY* wk, u32 pos )
{
	GFL_CLACTPOS  clPos;
	u32  mode;
	u32  anm;

	mode = PMSI_GetCategoryMode(wk->mwk);
		
  if( mode == CATEGORY_MODE_GROUP && pos != CATEGORY_POS_BACK )
	{
    GF_ASSERT( pos != CATEGORY_POS_SELECT );
    GF_ASSERT( pos != CATEGORY_POS_ERASE );

    clPos.x = CATEGORY_CURSOR_OX + CATEGORY_CURSOR_X_MARGIN * (pos % CATEGORY_WIN_ROWS);
    clPos.y = CATEGORY_CURSOR_OY + CATEGORY_CURSOR_Y_MARGIN * (pos / CATEGORY_WIN_ROWS);
    anm = ANM_CATEGORY_CURSOR_ACTIVE;
  }
  else
  {
    PMSIV_MENU* menu_wk = PMSIView_GetMenuWork( wk->vwk );
			
		anm = ANM_CATEGORY_BACK_CURSOR_ACTIVE; // �����\�����Ȃ�OAM

    if( pos == CATEGORY_POS_SELECT )
    {
      PMSIV_MENU_TaskMenuSetActive( menu_wk, 0 ,TRUE );
    }
    else if( pos == CATEGORY_POS_ERASE )
    {
      PMSIV_MENU_TaskMenuSetActive( menu_wk, 1 ,TRUE );
    }
    else if( pos == CATEGORY_POS_BACK )
    {
      PMSIV_MENU_TaskMenuSetActive( menu_wk, 2 ,TRUE );
    }
    else
    {
			u32 x, y;
      
      PMSIV_MENU_TaskMenuSetActive( menu_wk, NULL , FALSE );
			
      PMSI_INITIAL_DAT_GetPrintPos(pos, &x, &y);
			clPos.x = INITIAL_CURSOR_OX + x;
			clPos.y = INITIAL_CURSOR_OY + y;
			anm = ANM_INITIAL_CURSOR_ACTIVE;
    }
  }
    
  GFL_CLACT_WK_SetPos( wk->cursor_actor, &clPos , CLSYS_DEFREND_MAIN );
  GFL_CLACT_WK_SetAnmSeq( wk->cursor_actor, anm );
}

//------------------------------------------------------------------
/**
	* �J�e�S���I���J�n�G�t�F�N�g�J�n
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_StartEnableBG( PMSIV_CATEGORY* wk )
{
	wk->eff_seq = 0;

	PMSIV_TOOL_SetupBrightWork( &(wk->blend_work),
			FRM_MAIN_CATEGORY_BLENDMASK, HARD_WINDOW_BLDY, 0, BRIGHT_EFFECT_WAIT );
  
  HOSAKA_Printf("PMSIV_CATEGORY_StartEnableBG \n");
}

//------------------------------------------------------------------
/**
	* �J�e�S���I���J�n�G�t�F�N�g�I���҂�
	*
	* @param   wk		
	*
	* @retval  BOOL	TRUE�ŏI��
	*/
//------------------------------------------------------------------
BOOL PMSIV_CATEGORY_WaitEnableBG( PMSIV_CATEGORY* wk )
{
	switch( wk->eff_seq ){
	case 0:
		if( PMSIV_TOOL_WaitBright( (&wk->blend_work) ) )
		{
      return TRUE;
#if 0
			PMSIV_TOOL_SetupScrollWork( &(wk->scroll_work), 
					FRM_MAIN_CATEGORY, PMSIV_TOOL_SCROLL_DIRECTION_Y,
					CATEGORY_BG_ENABLE_EFF_VECTOR, ENABLE_MOVE_FRAMES );
			wk->eff_seq++;
#endif
    }
		break;

#if 0
	case 1:
		return PMSIV_TOOL_WaitScroll( &(wk->scroll_work) );
#endif 

  default : GF_ASSERT(0);
	}

	return FALSE;
}

//------------------------------------------------------------------
/**
	* �J�e�S���I���I���G�t�F�N�g�J�n
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_StartDisableBG( PMSIV_CATEGORY* wk )
{
	wk->eff_seq = 0;

	PMSIV_TOOL_SetupScrollWork( &(wk->scroll_work), FRM_MAIN_CATEGORY, PMSIV_TOOL_SCROLL_DIRECTION_Y,
				 CATEGORY_BG_DISABLE_EFF_VECTOR, ENABLE_MOVE_FRAMES );

  HOSAKA_Printf("PMSIV_CATEGORY_StartDisableBG \n");
}

//------------------------------------------------------------------
/**
	* �J�e�S���I���I���G�t�F�N�g�I���҂�
	*
	* @param   wk		
	*
	* @retval  BOOL	TRUE�ŏI��
	*/
//------------------------------------------------------------------
BOOL PMSIV_CATEGORY_WaitDisableBG( PMSIV_CATEGORY* wk )
{
	switch( wk->eff_seq ){
	case 0:
	if( PMSIV_TOOL_WaitScroll(&(wk->scroll_work)) )
		{
			PMSIV_TOOL_SetupBrightWork( &(wk->blend_work), FRM_MAIN_CATEGORY_BLENDMASK, 0, HARD_WINDOW_BLDY, BRIGHT_EFFECT_WAIT );
			wk->eff_seq++;
		}
		break;

	case 1:
		return PMSIV_TOOL_WaitBright( (&wk->blend_work) );
	}

	return FALSE;
}

//------------------------------------------------------------------
/**
	* �J�e�S���I���I����̈ʒu��BG�I�t�Z�b�g�������I�ɖ߂�
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_SetDisableBG( PMSIV_CATEGORY* wk )
{
	GFL_BG_SetScroll( FRM_MAIN_CATEGORY, GFL_BG_SCROLL_Y_SET, CATEGORY_BG_DISABLE_YOFS );
}


//------------------------------------------------------------------
/**
	* �J�e�S���I���E�B���h�E�P�x�_�E���G�t�F�N�g�J�n
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_StartBrightDown( PMSIV_CATEGORY* wk )
{
	PMSIV_TOOL_SetupBrightWork( &(wk->blend_work), FRM_MAIN_CATEGORY_BLENDMASK, 0, HARD_WINDOW_BLDY, BRIGHT_EFFECT_WAIT );
}
BOOL PMSIV_CATEGORY_WaitBrightDown( PMSIV_CATEGORY* wk )
{
	return PMSIV_TOOL_WaitBright(&(wk->blend_work));
}


//------------------------------------------------------------------
/**
	* �J�e�S���I���E�B���h�E�t�F�[�h�A�E�g�J�n
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_StartFadeOut( PMSIV_CATEGORY* wk )
{
	const int GX_BLEND_ALL = (
			GX_BLEND_PLANEMASK_BG0|
			GX_BLEND_PLANEMASK_BG1|
			GX_BLEND_PLANEMASK_BG2|
			GX_BLEND_PLANEMASK_BG3|
			GX_BLEND_PLANEMASK_OBJ|
			GX_BLEND_PLANEMASK_BD);

	PMSIV_TOOL_SetupBlendWork( &wk->blend_work, FRM_MAIN_CATEGORY_BLENDMASK, GX_BLEND_ALL, PMSIV_BLEND_MAX, 0, PMSI_FRAMES(6) );
}

//------------------------------------------------------------------
/**
	* �J�e�S���I���E�B���h�E�t�F�[�h�A�E�g�J�n
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
BOOL PMSIV_CATEGORY_WaitFadeOut( PMSIV_CATEGORY* wk )
{
	if( PMSIV_TOOL_WaitBlend(&wk->blend_work) )
	{
		GFL_BG_SetVisible( FRM_MAIN_CATEGORY, FALSE );
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
	* �J�e�S���I���E�B���h�E�t�F�[�h�C���J�n
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_StartFadeIn( PMSIV_CATEGORY* wk )
{
	const int GX_BLEND_ALL = (
			GX_BLEND_PLANEMASK_BG0|
			GX_BLEND_PLANEMASK_BG1|
			GX_BLEND_PLANEMASK_BG2|
			GX_BLEND_PLANEMASK_BG3|
			GX_BLEND_PLANEMASK_OBJ|
			GX_BLEND_PLANEMASK_BD );

	G2_SetBlendAlpha( FRM_MAIN_CATEGORY_BLENDMASK, GX_BLEND_ALL, 0, 16 );
	GFL_BG_SetVisible( FRM_MAIN_CATEGORY, TRUE );

	PMSIV_TOOL_SetupBlendWork( &wk->blend_work, FRM_MAIN_CATEGORY_BLENDMASK, GX_BLEND_ALL, 0, PMSIV_BLEND_MAX, PMSI_FRAMES(6) );
}

//------------------------------------------------------------------
/**
	* �J�e�S���I���E�B���h�E�t�F�[�h�C���I���҂�
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
BOOL PMSIV_CATEGORY_WaitFadeIn( PMSIV_CATEGORY* wk )
{
	return PMSIV_TOOL_WaitBlend(&wk->blend_work);
}


//------------------------------------------------------------------
/**
	* �J�e�S�����[�h�ɉ����Ăa�f�I�t�Z�b�g��ύX�i�����j
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_ChangeModeBG( PMSIV_CATEGORY* wk )
{
	int xofs;

	if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_GROUP )
	{
		xofs = GROUPMODE_BG_XOFS;
	}
	else
	{
		xofs = INITIALMODE_BG_XOFS;
	}

	GFL_BG_SetScrollReq( FRM_MAIN_CATEGORY, GFL_BG_SCROLL_X_SET, xofs );
}


//------------------------------------------------------------------
/**
	* �J�e�S�����[�h�ɉ����Ăa�f�ύX(V�u�����N�]��)
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_StartModeChange( PMSIV_CATEGORY* wk )
{
  ARCHANDLE* p_handle;

	p_handle = GFL_ARC_OpenDataHandle( ARCID_PMSI_GRAPHIC, HEAPID_PMS_INPUT_VIEW );
	
  if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_GROUP )
	{
    int i;

	  GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank(p_handle, NARC_pmsi_pms_bg_main1_NSCR,
		  FRM_MAIN_CATEGORY, 0, 0, 0, FALSE, HEAPID_PMS_INPUT_VIEW );

    for(i=0; i<CATEGORY_GROUP_MAX; i++)
	  {
      GFL_BMPWIN_MakeScreen( wk->winGroup[i] );
    }
  }
  else
  {
    //@TODO ��1�L����������
	  GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank(p_handle, NARC_pmsi_pms_bg_main01_NSCR,
		  FRM_MAIN_CATEGORY, 0, 0, 0, FALSE, HEAPID_PMS_INPUT_VIEW );
	  
    GFL_BMPWIN_MakeScreen( wk->winInitial );
    GFL_BMPWIN_MakeScreen( wk->winInput );
  }
	
  GFL_ARC_CloseDataHandle( p_handle );

	GFL_BG_LoadScreenV_Req( FRM_MAIN_CATEGORY );
  
#if 0
  // �X�N���[��
	int vector;

	if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_GROUP )
	{
		vector = (GROUPMODE_BG_XOFS - INITIALMODE_BG_XOFS);
	}
	else
	{
		vector = (INITIALMODE_BG_XOFS - GROUPMODE_BG_XOFS);
	}

	PMSIV_TOOL_SetupScrollWork( &(wk->scroll_work), FRM_MAIN_CATEGORY, PMSIV_TOOL_SCROLL_DIRECTION_X,
				 vector, MODECHANGE_MOVE_FRAMES );
#endif

}

BOOL PMSIV_CATEGORY_WaitModeChange( PMSIV_CATEGORY* wk )
{
  return TRUE;
//	return PMSIV_TOOL_WaitScroll( &(wk->scroll_work) );
}

#include "pmsi_search.h"

//-----------------------------------------------------------------------------
/**
 *	@brief  ���͂��ꂽ�����`����X�V
 *
 *	@param	PMSIV_CATEGORY* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSIV_CATEGORY_InputWordUpdate( PMSIV_CATEGORY* wk )
{
	GFL_FONT *fontHandle;
  GFL_BMPWIN* win;
  STRBUF* buf;

  GF_ASSERT( wk );

	fontHandle = PMSIView_GetFontHandle(wk->vwk);
  win = wk->winInput;

  buf = GFL_STR_CreateBuffer( PMS_INPUTWORD_MAX+1, HEAPID_PMS_INPUT_VIEW );

  PMSI_GetInputWord( wk->mwk, buf );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win), CATEGORY_INPUT_WIN_COL_B);
  GFL_FONTSYS_SetColor( CATEGORY_INPUT_WIN_COL_L, CATEGORY_INPUT_WIN_COL_S, CATEGORY_INPUT_WIN_COL_B );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, 0, buf, fontHandle );

  GFL_STR_DeleteBuffer( buf );
		
	GFL_BMPWIN_MakeScreen( win );
  GFL_BMPWIN_TransVramCharacter( win );
	
  GFL_BG_LoadScreenV_Req( FRM_MAIN_CATEGORY );
}

static void start_move_sublist_down( PMSIV_CATEGORY* wk );
static void start_move_sublist_up( PMSIV_CATEGORY* wk, u32 count );
//-----------------------------------------------------------------------------
/**
 *	@brief  �P���⃊�X�g�ړ��J�n
 *
 *	@param	PMSIV_CATEGORY* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_CATEGORY_StartMoveSubWinList( PMSIV_CATEGORY* wk, BOOL is_enable )
{ 
  int i; 
  u32 count;

  count = PMSI_GetSearchResultCount(wk->mwk);
  
  // �ő�l����
  count = MATH_CLAMP( count, 0, SUBLIST_WIN_STR_NUM );
  
  if( is_enable )
  {
    // �o���ݒ�
    start_move_sublist_up( wk, count );
  }
  else
  {
    start_move_sublist_down( wk );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �Ђ����� �J�n
 *
 *	@param	PMSIV_CATEGORY* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void start_move_sublist_down( PMSIV_CATEGORY* wk )
{
  u32 vector;

  // �o�Ă��镪����������
  vector = -GX_LCD_SIZE_Y - GFL_BG_GetScrollY( FRM_SUB_SEARCH_LIST );
    
  HOSAKA_Printf("set down vector=%d \n",vector );

  PMSIV_TOOL_SetupScrollWork( &wk->scroll_work,
      FRM_SUB_SEARCH_LIST, PMSIV_TOOL_SCROLL_DIRECTION_Y,
      vector, SUBLIST_WIN_SCROLL_SYNC );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �łĂ��� �J�n
 *
 *	@param	PMSIV_CATEGORY* wk
 *	@param	count 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void start_move_sublist_up( PMSIV_CATEGORY* wk, u32 count )
{
  int i; 
  PRINTSYS_LSB color;
  PRINT_QUE* print_que;
	GFL_FONT *fontHandle;
  STRBUF* buf;
  
  color = PRINTSYS_LSB_Make( CATEGORY_SUBLIST_WIN_COL_L, CATEGORY_SUBLIST_WIN_COL_S, CATEGORY_SUBLIST_WIN_COL_B );
  print_que = PMSIView_GetPrintQue(wk->vwk);
	fontHandle = PMSIView_GetFontHandle(wk->vwk);
  buf = GFL_STR_CreateBuffer( 7*2+1, HEAPID_PMS_INPUT_VIEW );

  // ��[�N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->winSubList), CATEGORY_SUBLIST_WIN_COL_B );

  PRINT_UTIL_Setup( &wk->printSubList, wk->winSubList );

  // �������ʕ`��
  for( i=0; i<count; i++ )
  {
    int px, py;
    
    if( i%2 == 0 )
    {
      px = SUBLIST_WIN_XORG * 8;
    }
    else
    {
      px = ( SUBLIST_WIN_XORG + SUBLIST_WIN_WIDTH / 2 ) * 8;
    }

    py = (i/2) * 8 * 2;

    PMSI_GetSearchResultString( wk->mwk, i, buf );

    PRINT_UTIL_PrintColor( &wk->printSubList, print_que, px, py, buf, fontHandle, color );
  }

  GFL_STR_DeleteBuffer( buf );

  // �ړ��ݒ�
//  GFL_BG_SetScroll( FRM_SUB_SEARCH_LIST, GFL_BG_SCROLL_Y_SET, -GX_LCD_SIZE_Y ); 
    
  // ���݂̍��W����ړ��ʂ��v�Z
  {
    u32 row;
    int newofs;
    int oldofs; 
    int tarofs; 

    // �s��
    row = (count+1)/2; ///< �J��オ��

    // �����l����̈ړ���
    oldofs = GFL_BG_GetScrollY( FRM_SUB_SEARCH_LIST ) + GX_LCD_SIZE_Y;

    newofs = SUBLIST_WIN_YORG * 8 + row * 8 * 2; //< -192����̈ړ���
    
    // ���W����( �Œ���G�f�B�b�g�E�B���h�E�͉B��� )
    newofs = MATH_CLAMP( newofs, 6*8, GX_LCD_SIZE_Y );

    // �ړI�n�v�Z
    tarofs = newofs - oldofs;
  
    PMSIV_TOOL_SetupScrollWork( &wk->scroll_work,
        FRM_SUB_SEARCH_LIST, PMSIV_TOOL_SCROLL_DIRECTION_Y,
        tarofs, SUBLIST_WIN_SCROLL_SYNC );

    HOSAKA_Printf("set newofs=%d oldofs=%d tarofs=%d \n", newofs, oldofs, tarofs );
  }

  // �X�N���[������
  GFL_BMPWIN_MakeScreen( wk->winSubList );
  GFL_BG_LoadScreenV_Req( FRM_SUB_SEARCH_LIST );

  // �\��
  GFL_BG_SetVisible( FRM_SUB_SEARCH_LIST, TRUE );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �P���⃊�X�g�o������
 *
 *	@param	PMSIV_CATEGORY* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL PMSIV_CATEGORY_WaitMoveSubWinList( PMSIV_CATEGORY* wk, BOOL is_enable )
{
  PRINT_QUE* print_que = PMSIView_GetPrintQue( wk->vwk );
  BOOL flag1 = TRUE;
  BOOL flag2;

  // �v�����g���s�Ȃ��̂�UP�̂�
  if( is_enable )
  {
    flag1 = PRINT_UTIL_Trans( &wk->printSubList, print_que );
  }

  flag2 = PMSIV_TOOL_WaitScroll( &(wk->scroll_work) );

//  HOSAKA_Printf("f1=%d f2=%d \n",flag1, flag2);
  
  if( flag1 && flag2 )
  {
    HOSAKA_Printf("list move end pos=%d \n", GFL_BG_GetScrollY( FRM_SUB_SEARCH_LIST ) );
    
    // DOWN�I������
    if( is_enable == FALSE )
    {
      // ��\����
      GFL_BG_SetVisible( FRM_SUB_SEARCH_LIST, FALSE );
    }
    return TRUE;
  }

  return FALSE;
}

//------------------------------------------------------------------
/**
	* �J�e�S���������������͂ɂ����āA1�������͂����Ƃ��A���̕����̂Ƃ���𖾖ł�����
	* �J�e�S���킴�s�N�`���[�Ȃǂ���I�������āA�I�������Ƃ��A���̑I�������Ƃ���𖾖ł�����
	*
	* @param   wk		
	* @param   pos		
	*
	*/
//------------------------------------------------------------------
void PMSIV_CATEGORY_InputBlink( PMSIV_CATEGORY* wk, u32 pos )
{
  // InputBlick���s���Ƃ���cursor_actor�̕\�����o���Ă���
  wk->input_blink_cursor_visible = GFL_CLACT_WK_GetDrawEnable( wk->cursor_actor );

  // cursor_actor�𖾖ŃA�j���ɂ���
  GFL_CLACT_WK_SetDrawEnable( wk->cursor_actor, TRUE );
  {

    // PMSIV_CATEGORY_MoveCursor ���Q�l�Ɉʒu�����߂�
	GFL_CLACTPOS  clPos;
	u32  mode;
	u32  anm;

	mode = PMSI_GetCategoryMode(wk->mwk);
		
  if( mode == CATEGORY_MODE_GROUP && pos != CATEGORY_POS_BACK )
	{
    GF_ASSERT( pos != CATEGORY_POS_SELECT );
    GF_ASSERT( pos != CATEGORY_POS_ERASE );

    clPos.x = CATEGORY_CURSOR_OX + CATEGORY_CURSOR_X_MARGIN * (pos % CATEGORY_WIN_ROWS);
    clPos.y = CATEGORY_CURSOR_OY + CATEGORY_CURSOR_Y_MARGIN * (pos / CATEGORY_WIN_ROWS);
    anm = ANM_CATEGORY_CURSOR_ON;
  }
  else
  {
    //PMSIV_MENU* menu_wk = PMSIView_GetMenuWork( wk->vwk );
			
		anm = ANM_CATEGORY_BACK_CURSOR_ACTIVE; // �����\�����Ȃ�OAM

    if( pos == CATEGORY_POS_SELECT )
    {
    }
    else if( pos == CATEGORY_POS_ERASE )
    {
    }
    else if( pos == CATEGORY_POS_BACK )
    {
    }
    else
    {
			u32 x, y;
      
      //PMSIV_MENU_TaskMenuSetActive( menu_wk, NULL , FALSE );
			
      PMSI_INITIAL_DAT_GetPrintPos(pos, &x, &y);
			clPos.x = INITIAL_CURSOR_OX + x;
			clPos.y = INITIAL_CURSOR_OY + y;
			anm = ANM_INITIAL_CURSOR_ON;
    }
  }
    
  GFL_CLACT_WK_SetPos( wk->cursor_actor, &clPos , CLSYS_DEFREND_MAIN );
  GFL_CLACT_WK_SetAnmSeq( wk->cursor_actor, anm );

  }
}
//------------------------------------------------------------------
/**
	* �J�e�S���������������͂ɂ����āA1�������͂����Ƃ��A���̕����̂Ƃ���𖾖ł����I���܂ő҂�
	* �J�e�S���킴�s�N�`���[�Ȃǂ���I�������āA�I�������Ƃ��A���̑I�������Ƃ���𖾖ł����I���܂ő҂�
	*
	* @param   wk	
  *
	* @retval  TRUE�̂Ƃ��A���ł��I������	
	*
	* @note    ���ł̍X�V�����̊֐��ōs���Ă���̂ŁAPMSIV_CATEGORY_InputBlink���Ă񂾌�͖��t���[���Ăяo���ĉ�����(1�t���[�����ɉ���Ăяo���Ă��A1�t���[�����̏��������i�܂Ȃ�)�B
	*
	*/
//------------------------------------------------------------------
BOOL PMSIV_CATEGORY_WaitInputBlink( PMSIV_CATEGORY* wk )
{
  u16 anm_seq = GFL_CLACT_WK_GetAnmSeq( wk->cursor_actor );
  if(    anm_seq != ANM_CATEGORY_CURSOR_ON
      && anm_seq != ANM_INITIAL_CURSOR_ON )
    return TRUE;
  
  // �I�����Ă��邩
  if( !GFL_CLACT_WK_CheckAnmActive( wk->cursor_actor ) )
  {
    // InputBlink���s���O��cursor_actor��\�����Ă�����
    GFL_CLACT_WK_SetDrawEnable( wk->cursor_actor, wk->input_blink_cursor_visible );

    // cursor_actor�̃A�j�������ɖ߂��Ă���
    {
      // PMSIV_CATEGORY_MoveCursor �Q�l
      // �ʒu�͕ς��Ȃ��Ă���
	u32  mode;
	u32  anm;

	mode = PMSI_GetCategoryMode(wk->mwk);
		
  if( mode == CATEGORY_MODE_GROUP )
	{
    anm = ANM_CATEGORY_CURSOR_ACTIVE;
  }
  else
  {
    {
			anm = ANM_INITIAL_CURSOR_ACTIVE;
    }
  }
    
  GFL_CLACT_WK_SetAnmSeq( wk->cursor_actor, anm );
    }

    wk->input_blink_cursor_visible = FALSE;

    return TRUE;
  }
  return FALSE;
}

