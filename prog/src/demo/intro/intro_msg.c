//=============================================================================
/**
 *
 *	@file		intro_msg.c
 *	@brief  �C���g�� ���b�Z�[�W����
 *	@author	hosaka genya
 *	@data		2009.12.15
 *
 */
//=============================================================================
#include <gflib.h>
#include "system/bmp_winframe.h" // for BmpWinFrame_
#include "system/bmp_menulist.h" // for BMPMENU_LIST_WORK

#include "app/app_keycursor.h"  // for APP_KEYCURSOR

// ������֘A
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" // for PRINT_QUE

#include "gamesystem/msgspeed.h"  // for MSGSPEED_GetWait

#include "font/font.naix"

#include "sound/pm_sndsys.h" // for 

//�A�[�J�C�u
#include "arc_def.h" // for ARCID_XXX

#include "print/printsys.h" // for PRINTSYS_

#include "intro_sys.h"
#include "intro_msg.h" // for extern

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  CGX_BMPWIN_FRAME_POS = 1,
  STRBUF_SIZE = 1600,
};

#define MSG_SKIP_BTN (PAD_BUTTON_A|PAD_BUTTON_B)

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//-------------------------------------
///	���X�g
//=====================================
typedef struct 
{
	BMPMENULIST_WORK	*p_list;
	BMP_MENULIST_DATA *p_list_data;
	u32		select;
} INTRO_LIST_WORK;

//--------------------------------------------------------------
///	���b�Z�[�W���� ���C�����[�N
//==============================================================
struct _INTRO_MSG_WORK {
  // [PRIVATE]
  HEAPID        heap_id;
  PRINT_QUE*    print_que;
  WORDSET*      wordset;
  GFL_MSGDATA*  msghandle;
  GFL_FONT*     font;

  // �X�g���[���Đ�
  PRINT_STREAM* print_stream;
  GFL_TCBLSYS*  msg_tcblsys;
  GFL_BMPWIN*   win_dispwin;
  
  // �L�[�J�[�\��
  APP_KEYCURSOR_WORK* cursor_work;
  
  // ���X�g
  INTRO_LIST_WORK   list;
  PRINT_UTIL        print_util;
  GFL_BMPWIN*       win_list;

  // ������
  STRBUF*   strbuf;
  STRBUF*   exp_strbuf;
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
//static STRBUF * MSGDAT_UTIL_AllocExpandString( const WORDSET *wordset, GFL_MSGDATA *MsgManager, u32 strID, HEAPID heapID );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ����W���[�� ����
 *
 *	@param	HEAPID heap_id �q�[�vID
 *
 *	@retval INTRO_MSG_WORK* wk ���C�����[�N
 */
//-----------------------------------------------------------------------------
INTRO_MSG_WORK* INTRO_MSG_Create( HEAPID heap_id )
{
  INTRO_MSG_WORK* wk;

  // ���C�����[�N �A���P�[�g
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(INTRO_MSG_WORK) );
  
  // �����o������
  wk->heap_id = heap_id;
  wk->wordset = WORDSET_Create( heap_id );
  // @TODO intro.c�Ɋ��ɂ���B��������B
  wk->print_que = PRINTSYS_QUE_Create( heap_id );

  wk->msg_tcblsys = GFL_TCBL_Init( wk->heap_id, wk->heap_id, 1, 0 );

  // ���b�Z�[�W�p�t�H���g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0x20*PLTID_BG_TEXT_M, 0x20, heap_id );

  // �t���[���E�B���h�E�p�̃L������p��
  BmpWinFrame_GraphicSet( BG_FRAME_TEXT_M, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_WIN_M, MENU_TYPE_FIELD, heap_id );

  // �t�H���g��W�J
  wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_MEMORY, FALSE, wk->heap_id );

  wk->strbuf      = GFL_STR_CreateBuffer( STRBUF_SIZE, wk->heap_id );
  wk->exp_strbuf  = GFL_STR_CreateBuffer( STRBUF_SIZE, wk->heap_id );

  // �E�B���h�E����
  // @TODO ���X�g���o���ꏊ�w��
  wk->win_list = GFL_BMPWIN_Create( BG_FRAME_TEXT_M, 22, 13, 9, 4, PLTID_BG_TEXT_M, GFL_BMP_CHRAREA_GET_B );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->win_list ), 0 ); // �N���A���Ă���
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win_list );

  // �E�B���h�E����
  wk->win_dispwin = GFL_BMPWIN_Create( BG_FRAME_TEXT_M, 1, 19, 30, 4, PLTID_BG_TEXT_M, GFL_BMP_CHRAREA_GET_B );

  // �E�B���h�E�J�[�\��������
  wk->cursor_work = APP_KEYCURSOR_Create( 15, wk->heap_id );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ����W���[�� �j��
 *
 *	@param	INTRO_MSG_WORK* wk  ���C�����[�N
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_Exit( INTRO_MSG_WORK* wk )
{
  APP_KEYCURSOR_Delete( wk->cursor_work );

  GFL_STR_DeleteBuffer( wk->strbuf );
  GFL_STR_DeleteBuffer( wk->exp_strbuf );
  
  GFL_FONT_Delete( wk->font );

  // �܂��I�����Ă��Ȃ�������\����������ꍇ�͔j��
  if( wk->print_stream )
  {
    PRINTSYS_PrintStreamDelete( wk->print_stream );
  }
  
  GFL_BMPWIN_Delete( wk->win_dispwin );
  
  if( wk->win_list )
  {
    GFL_BMPWIN_Delete( wk->win_list );
  }

  GFL_TCBL_Exit( wk->msg_tcblsys );

  PRINTSYS_QUE_Delete( wk->print_que );
  
  if( wk->msghandle )
  {
    GFL_MSG_Delete( wk->msghandle );
  }

  WORDSET_Delete( wk->wordset );

  // ���C�����[�N �j��
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W���\�[�X�������[�h
 *
 *	@param	INTRO_MSG_WORK* wk
 *	@param	type
 *	@param	msg_dat_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_LoadGmm( INTRO_MSG_WORK* wk, GflMsgLoadType type, u16 msg_dat_id )
{
  if( wk->msghandle )
  {
    GFL_MSG_Delete( wk->msghandle );
  }
  wk->msghandle = GFL_MSG_Create( type, ARCID_MESSAGE, msg_dat_id, wk->heap_id );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ����W���[�� �又��
 *
 *	@param	INTRO_MSG_WORK* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_Main( INTRO_MSG_WORK* wk )
{
  GFL_TCBL_Main( wk->msg_tcblsys );
	PRINTSYS_QUE_Main( wk->print_que );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�v�����g �J�n
 *
 *	@param	INTRO_MSG_WORK* wk�@���C�����[�N
 *	@param	str_id ���b�Z�[�WID
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
//void INTRO_MSG_SetPrint( INTRO_MSG_WORK* wk, int str_id, WORDSET_CALLBACK callback_func, void* callback_arg )
void INTRO_MSG_SetPrint( INTRO_MSG_WORK* wk, int str_id )
{
  const u8 clear_color = 15;
  GFL_BMPWIN* win;
  int msgspeed;
  
  msgspeed  = MSGSPEED_GetWait();
  win       = wk->win_dispwin;

//  HOSAKA_Printf("msgspeed = %d str_id=%d \n", msgspeed, str_id);

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(win), clear_color);
  GFL_FONTSYS_SetColor(1, 2, clear_color);

  GFL_MSG_GetString( wk->msghandle, str_id, wk->strbuf );

#ifdef PM_DEBUG
  GFL_STR_CheckBufferValid( wk->strbuf ); ///< �j���`�F�b�N
#endif

#if 0
  // �R�[���o�b�N��WORDSET
  if( callback_func )
  {
    callback_func( wk->wordset, callback_arg );
  }
#endif

  WORDSET_ExpandStr( wk->wordset, wk->exp_strbuf, wk->strbuf );

#ifdef PM_DEBUG
  GFL_STR_CheckBufferValid( wk->exp_strbuf ); ///< �j���`�F�b�N
#endif

  wk->print_stream = PRINTSYS_PrintStream( win, 4, 0, wk->exp_strbuf, wk->font, msgspeed,
                                           wk->msg_tcblsys, 0xffff, wk->heap_id, clear_color );

  BmpWinFrame_Write( win, WINDOW_TRANS_ON_V, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_M );

  // �]��
  GFL_BMPWIN_TransVramCharacter( win );
  GFL_BMPWIN_MakeScreen( win );

  GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(win) );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �C���g���f�� �v�����g�X�g���[����Ԏ擾
 *
 *	@param	INTRO_MSG_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
PRINTSTREAM_STATE INTRO_MSG_GetPrintState( INTRO_MSG_WORK* wk )
{
  GF_ASSERT( wk );
 
  if( wk->print_stream == NULL )
  {
    // NULL��Ԃł��l��Ԃ��Ă��܂��̂̓Z�L�����e�B��D�܂����Ȃ����A
    // intro_cmd.c��CMD_MCSS_SET_PEPACHI_WITH_PRINT�ŁA
    // �v�����g�R�}���h�I����ɂ��̊֐�����߂�l���󂯎�肽���̂ŁA��ނ𓾂��B
    return PRINTSTREAM_STATE_DONE;
  }

  return PRINTSYS_PrintStreamGetState( wk->print_stream );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�v�����g �I���҂�
 *
 *	@param	INTRO_MSG_WORK* wk 
 *
 *	@retval TRUE:�I��
 */
//-----------------------------------------------------------------------------
BOOL INTRO_MSG_PrintProc( INTRO_MSG_WORK* wk )
{
  PRINTSTREAM_STATE state;

//  HOSAKA_Printf("print state= %d \n", state );

  if( wk->print_stream )
  {
    APP_KEYCURSOR_Proc( wk->cursor_work, wk->print_stream, wk->win_dispwin );

    state = PRINTSYS_PrintStreamGetState( wk->print_stream );

    switch( state )
    {
    case PRINTSTREAM_STATE_DONE : // �I��
      PRINTSYS_PrintStreamDelete( wk->print_stream );
      wk->print_stream = NULL;
      return TRUE;

    case PRINTSTREAM_STATE_PAUSE : // �ꎞ��~��
      // �L�[���͑҂�
      if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE || (  GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL ) || GFL_UI_TP_GetTrg() )
      {
        PRINTSYS_PrintStreamReleasePause( wk->print_stream );
        GFL_SOUND_PlaySE( SEQ_SE_DECIDE1 );
      }
      break;

    case PRINTSTREAM_STATE_RUNNING :  // ���s��
      // ���b�Z�[�W�X�L�b�v
      if( (GFL_UI_KEY_GetCont() & MSG_SKIP_BTN) || GFL_UI_TP_GetTrg() )
      {
        PRINTSYS_PrintStreamShortWait( wk->print_stream, 0 );
      }
      break;

    default : GF_ASSERT(0);
    }

    return FALSE;
  }

  return FALSE;
}





//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

#if 0
//----------------------------------------------------------------------------
/**
 *  @brief  ������W�J����MSG���擾
 *
 *  @param  WORDSET *wordset  ���[�h�Z�b�g
 *  @param  *MsgManager       ���b�Z�[�W
 *  @param  strID             ����ID
 *  @param  heapID            �e���|�����q�[�vID
 *
 *  @return ������W�J���STRBUF
 */
//-----------------------------------------------------------------------------
static STRBUF * MSGDAT_UTIL_AllocExpandString( const WORDSET *wordset, GFL_MSGDATA *MsgManager, u32 strID, HEAPID heapID )
{
  STRBUF  *src;
  STRBUF  *dst;
  src = GFL_MSG_CreateString( MsgManager, strID );
  dst = GFL_STR_CreateBuffer( GFL_STR_GetBufferLength( src ), heapID );

  WORDSET_ExpandStr( wordset, src, dst );

  GFL_STR_DeleteBuffer( src );

  return dst;
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	PRINT_UTIL��ݒ肵�擾
 *
 *	@param	MSG_WORK *wk	���[�N
 *	@param	BMPWIN
 *
 *	@return	PRINT_UTIL
 */
//-----------------------------------------------------------------------------
static PRINT_UTIL * MSG_GetPrintUtil( INTRO_MSG_WORK *wk, GFL_BMPWIN*	p_bmpwin )
{	
	PRINT_UTIL_Setup( &wk->print_util, p_bmpwin );
	return &wk->print_util;
}

//=============================================================================
/**
 *					LIST
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���X�g�V�X�e���@������
 *
 *	@param	INTRO_LIST_WORK *wk	���[�N
 *	@param	*cp_tbl					�ݒ�e�[�u��
 *	@param	tbl_max					�ݒ�e�[�u����
 *	@param  is_cansel       �L�����Z��
 *
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_LIST_Start( INTRO_MSG_WORK* wk, const INTRO_LIST_DATA *cp_tbl, u32 tbl_max, BOOL is_cansel )
{	
  INTRO_LIST_WORK* list = &wk->list;
  HEAPID heapID         = wk->heap_id;
  GFL_BMPWIN* p_bmpwin  = wk->win_list;

	//�N���A
	GFL_STD_MemClear( list, sizeof(INTRO_LIST_WORK));

	//LISTDATA create
	{	
		int i;

		list->p_list_data	= BmpMenuWork_ListCreate( tbl_max, heapID );

		for( i = 0; i < tbl_max; i++ )
		{	
      GFL_MSG_GetString( wk->msghandle, cp_tbl[i].str_id, wk->strbuf );

      BmpMenuWork_ListAddString( &list->p_list_data[i], wk->strbuf, cp_tbl[i].param, heapID );
		}
	}

	//LIST create
	{	
		BMPMENULIST_HEADER	header;

		GFL_STD_MemClear( &header, sizeof(BMPMENULIST_HEADER));
		header.list				= list->p_list_data;
		header.win				= p_bmpwin;
		header.count			= tbl_max;
		header.line				= 5;
		header.rabel_x		= 0;
		header.data_x			= 16;
		header.cursor_x		= 0;
		header.line_y			= 2;
		header.f_col			= 1;
		header.b_col			= 15;
		header.s_col			= 2;
		header.msg_spc		= 0;
		header.line_spc		= 0;
		header.page_skip	= BMPMENULIST_LRKEY_SKIP;
		header.font				= 0;
		header.c_disp_f		= 0;
		header.work				= NULL;
		header.font_size_x= 16;
		header.font_size_y= 16;
		header.msgdata		= NULL;
		header.print_util	= MSG_GetPrintUtil( wk, p_bmpwin );
		header.print_que	= wk->print_que;
		header.font_handle= wk->font;

		list->p_list	= BmpMenuList_Set( &header, 0, 0, heapID );
		BmpMenuList_SetCursorBmp( list->p_list, heapID );
	}

  GFL_BMPWIN_MakeTransWindow( p_bmpwin ); // �X�N���[�����L�����]��
  BmpWinFrame_Write( p_bmpwin, WINDOW_TRANS_ON_V, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_M ); // ����Ƀt���[��������

  if( is_cansel )
  {
    BmpMenuList_SetCancelMode( list->p_list, BMPMENULIST_CANCELMODE_USE );
  }
  else
  {
    BmpMenuList_SetCancelMode( list->p_list, BMPMENULIST_CANCELMODE_NOT );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���X�g�V�X�e��	�j��
 *
 *	@param	INTRO_LIST_WORK *wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_LIST_Finish( INTRO_MSG_WORK *wk )
{
  INTRO_LIST_WORK* list = &wk->list;

  BmpWinFrame_Clear( wk->win_list, WINDOW_TRANS_ON_V );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->win_list ), 0 ); // �N���A
  GFL_BMPWIN_TransVramCharacter( wk->win_list ); // �]��

	BmpMenuList_Exit( list->p_list, NULL, NULL );
	BmpMenuWork_ListDelete( list->p_list_data );
	GFL_STD_MemClear( list, sizeof(INTRO_LIST_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���X�g�V�X�e��	���C������
 *
 *	@param	INTRO_LIST_WORK *wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_LIST_Main( INTRO_MSG_WORK *wk )
{	
  INTRO_LIST_WORK* list = &wk->list;

	list->select	= BmpMenuList_Main(	list->p_list );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���X�g����҂�
 *
 *	@param	const INTRO_LIST_WORK *cwk	���[�N
 *	@param	*p_select								���X�g�̃C���f�b�N�X
 *
 *	@retval	TRUE�Ȃ�Ό���
 *	@retval	FALSE�Ȃ�ΑI��
 */
//-----------------------------------------------------------------------------
INTRO_LIST_SELECT INTRO_MSG_LIST_IsDecide( INTRO_MSG_WORK *wk, u32 *p_select )
{	
  INTRO_LIST_WORK* list = &wk->list;

	switch( list->select)
  { 
  case BMPMENULIST_NULL:
    return INTRO_LIST_SELECT_NONE;

	case BMPMENULIST_CANCEL:
    return INTRO_LIST_SELECT_CANCEL;

  default:
		if( p_select )
		{	
			*p_select	= list->select;
		}
		return INTRO_LIST_SELECT_DECIDE;
  }

	return INTRO_LIST_SELECT_NONE;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  WORDSET�ւ̃|�C���^���擾
 *
 *	@param	INTRO_MSG_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
WORDSET* INTRO_MSG_GetWordSet( INTRO_MSG_WORK* wk )
{
  return wk->wordset;
}
