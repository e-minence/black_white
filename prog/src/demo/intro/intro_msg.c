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

// ������֘A
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" // for PRINT_QUE

#include "gamesystem/msgspeed.h"  // for MSGSPEED_GetWait

#include "font/font.naix"
#include "message.naix"

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
};

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	���b�Z�[�W���� ���C�����[�N
//==============================================================
struct _INTRO_MSG_WORK {
  // [PRIVATE]
  HEAPID        heap_id;
  PRINT_QUE*    print_que;
  WORDSET*      wordset;
  PRINT_STREAM* print_stream;
  GFL_TCBLSYS*  msg_tcblsys;
  GFL_MSGDATA*  msghandle;
  GFL_FONT*     font;
  GFL_BMPWIN*   win_dispwin;
  GFL_ARCUTIL_TRANSINFO  bgchar_frame;
};


//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static STRBUF * MSGDAT_UTIL_AllocExpandString( const WORDSET *wordset, GFL_MSGDATA *MsgManager, u32 strID, HEAPID heapID );

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
  wk->print_que = PRINTSYS_QUE_Create( heap_id );
  
  // ���b�Z�[�W�n���h����ROM�ɓW�J���Ă���
  wk->msghandle = GFL_MSG_Create( GFL_MSG_LOAD_FAST, ARCID_MESSAGE, NARC_message_intro_dat, heap_id );

  wk->msg_tcblsys = GFL_TCBL_Init( wk->heap_id, wk->heap_id, 1, 0 );

  // ���b�Z�[�W�p�t�H���g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*PLTID_BG_TEXT_M, 0x20, heap_id );

  // �t���[���E�B���h�E�p�̃L������p��
  BmpWinFrame_GraphicSet( BG_FRAME_TEXT_M, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_WIN_M, MENU_TYPE_FIELD, heap_id );

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
  // �܂��I�����Ă��Ȃ�������\����������ꍇ�͔j��
  if( wk->print_stream )
  {
    PRINTSYS_PrintStreamDelete( wk->print_stream );
  }
  
  if( wk->win_dispwin )
  {
    GFL_BMPWIN_Delete( wk->win_dispwin );
  }

  GFL_TCBL_Exit( wk->msg_tcblsys );

  PRINTSYS_QUE_Delete( wk->print_que );

  GFL_MSG_Delete( wk->msghandle );

  WORDSET_Delete( wk->wordset );

  // ���C�����[�N �j��
  GFL_HEAP_FreeMemory( wk );
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
void INTRO_MSG_SetPrint( INTRO_MSG_WORK* wk, int str_id )
{
  GFL_BMPWIN* win;
  STRBUF* str;

  // �E�B���h�E����
  if( wk->win_dispwin == NULL )
  {
    wk->win_dispwin = GFL_BMPWIN_Create( BG_FRAME_TEXT_M, 1, 19, 30, 4, PLTID_BG_TEXT_M, GFL_BMP_CHRAREA_GET_B );
  }
  
  win = wk->win_dispwin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(win), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);

  str = MSGDAT_UTIL_AllocExpandString( wk->wordset, wk->msghandle, str_id, wk->heap_id );

  wk->print_stream = PRINTSYS_PrintStream( win, 0, 0, str, wk->font, MSGSPEED_GetWait(),
                                           wk->msg_tcblsys, 1, wk->heap_id, 15 );

  GFL_STR_DeleteBuffer( str );
  
  BmpWinFrame_Write( win, WINDOW_TRANS_ON_V, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_M );

  // �]��
  GFL_BMPWIN_TransVramCharacter( win );
  GFL_BMPWIN_MakeScreen( win );

  GFL_BG_LoadScreenV_Req( BG_FRAME_TEXT_M );
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
BOOL INTRO_MSG_IsPrintEnd( INTRO_MSG_WORK* wk )
{
  if( wk->print_stream )
  {
    PRINTSTREAM_STATE state = PRINTSYS_PrintStreamGetState( wk->print_stream );

    switch( state )
    {
    case PRINTSTREAM_STATE_DONE :
      PRINTSYS_PrintStreamDelete( wk->print_stream );
      wk->print_stream = NULL;
      break;

    case PRINTSTREAM_STATE_PAUSE :
      if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE || GFL_UI_TP_GetTrg() )
      {
        PRINTSYS_PrintStreamReleasePause( wk->print_stream );
      }
      break;

    default : GF_ASSERT(0);
    }
  }

  return TRUE; // �I��
}





//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//@TODO WB�ɂ͂Ȃ��̂łƂ肠���������Ɏ��̂�����
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

