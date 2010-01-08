//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_util.c
 *	@brief  �ӂ����Ȃ�������̂Ŏg�p���郂�W���[����
 *	@author	Toru=Nagihashi
 *	@data		2009.12.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "gamesystem/msgspeed.h"
#include "system/bmp_winframe.h"
#include "system/main.h"    //HEAPID
#include "print/gf_font.h"  //GFL_FONT
#include "app/app_keycursor.h"

//�����\��
#include "print/printsys.h"

//�O�����J
#include "mystery_util.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					BMPWIN���b�Z�[�W�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
struct _MYSTERY_MSGWIN_WORK
{
	PRINT_UTIL				util;				//�����`��
	PRINT_QUE					*p_que;			//�����L���[
	GFL_BMPWIN*				p_bmpwin;		//BMPWIN
	STRBUF*						p_strbuf;		//�����o�b�t�@
	u16								clear_chr;	//�N���A�L����
	u16								frm;        //BG��
};
//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  ������
 *
 *	@param	u16 frm BG��
 *	@param	x       X���W
 *	@param	y       Y���W
 *	@param	w       ��
 *	@param	h       ����
 *	@param	plt     �t�H���g�p���b�g
 *	@param	*p_que  �L���[
 *	@param	heapID  �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
MYSTERY_MSGWIN_WORK * MYSTERY_MSGWIN_Init( u16 frm, u8 x, u8 y, u8 w, u8 h, u8 plt, PRINT_QUE *p_que, HEAPID heapID )
{ 
  MYSTERY_MSGWIN_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_MSGWIN_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_MSGWIN_WORK) );
  p_wk->clear_chr = 0;
  p_wk->p_que			= p_que;
  p_wk->frm       = frm;

  //�o�b�t�@�쐬
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 512, heapID );

	//BMPWIN�쐬
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, x, y, w, h, plt, GFL_BMP_CHRAREA_GET_B );

	//�v�����g�L���[�ݒ�
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//�]��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �j��
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_Exit( MYSTERY_MSGWIN_WORK* p_wk )
{ 
  //BMPWIN�j��
  GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );

	//�o�b�t�@�j��
	GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �v�����g�J�n
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_Print( MYSTERY_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font )
{ 
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//������쐬
	GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

	//������`��
	PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_font );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  STRBUF�v�����g�J�n
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_strbuf         STRBUF  �����j�����Ă����܂��܂���
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_PrintBuf( MYSTERY_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font )
{ 
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//������쐬
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

	//������`��
	PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_font );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �v�����g�J�n  �F�w���
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param	*p_font           �t�H���g
 *	@param  lsb               �F�w��
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_PrintColor( MYSTERY_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font, PRINTSYS_LSB lsb )
{ 
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//������쐬
	GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

	//������`��
	PRINT_UTIL_PrintColor( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_font, lsb );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �v�����g�������ݏ���
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk   ���[�N
 *
 *	@return TRUE�Ȃ�Ώ�������  FALSE�Ȃ�΂܂�
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_MSGWIN_PrintMain( MYSTERY_MSGWIN_WORK* p_wk )
{ 
	return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�e�L�X�g�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
#define MYSTERY_TEXT_TYPE_NONE  (MYSTERY_TEXT_TYPE_MAX)

//-------------------------------------
/// ���b�Z�[�W�E�B���h�E
//=====================================
struct _MYSTERY_TEXT_WORK
{
  GFL_MSGDATA       *p_msg;
  GFL_FONT          *p_font;
  PRINT_STREAM      *p_stream;
  GFL_TCBLSYS       *p_tcbl;
  GFL_BMPWIN*       p_bmpwin;
  STRBUF*           p_strbuf;
  u16               clear_chr;
  u16               heapID;
  PRINT_UTIL        util;
  PRINT_QUE         *p_que;
  u32               print_update;
  BOOL              is_end_print;
  APP_KEYCURSOR_WORK* p_keycursor;
} ;

//-------------------------------------
///	�v���g�^�C�v
//=====================================
static void MYSTERY_TEXT_PrintInner( MYSTERY_TEXT_WORK* p_wk, MYSTERY_TEXT_TYPE type );

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  ������
 *
 *	@param	u16 frm       BG��
 *	@param	font_plt      �t�H���g�p���b�g
 *	@param  PRINT_QUE     �v�����gQ
 *	@param  GFL_FONT      �t�H���g
 *	@param	heapID        �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
MYSTERY_TEXT_WORK * MYSTERY_TEXT_Init( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  MYSTERY_TEXT_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_TEXT_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_TEXT_WORK) );
  p_wk->clear_chr = 0xF;
  p_wk->p_font    = p_font;
  p_wk->p_que     = p_que;
  p_wk->print_update  = MYSTERY_TEXT_TYPE_NONE;

  p_wk->p_keycursor  = APP_KEYCURSOR_Create( 0, TRUE, TRUE, heapID );

  //�o�b�t�@�쐬
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 512, heapID );

	//BMPWIN�쐬
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, 1, 19, 30, 4, font_plt, GFL_BMP_CHRAREA_GET_B );

	//�v�����g�L���[�ݒ�
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//�]��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  p_wk->p_tcbl    = GFL_TCBL_Init( heapID, heapID, 1, 32 );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  ������  �P�s���b�Z�[�W
 *
 *	@param	u16 frm       BG��
 *	@param	font_plt      �t�H���g�p���b�g
 *	@param  PRINT_QUE     �v�����gQ
 *	@param  GFL_FONT      �t�H���g
 *	@param	heapID        �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
MYSTERY_TEXT_WORK * MYSTERY_TEXT_InitOneLine( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  MYSTERY_TEXT_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_TEXT_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_TEXT_WORK) );
  p_wk->clear_chr = 0xF;
  p_wk->p_font    = p_font;
  p_wk->p_que     = p_que;
  p_wk->print_update  = MYSTERY_TEXT_TYPE_NONE;

  p_wk->p_keycursor  = APP_KEYCURSOR_Create( p_wk->clear_chr, TRUE, TRUE, heapID );

  //�o�b�t�@�쐬
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 512, heapID );

	//BMPWIN�쐬
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, 1, 21, 30, 2, font_plt, GFL_BMP_CHRAREA_GET_B );

	//�v�����g�L���[�ݒ�
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//�]��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  p_wk->p_tcbl    = GFL_TCBL_Init( heapID, heapID, 1, 32 );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �j��
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_Exit( MYSTERY_TEXT_WORK* p_wk )
{ 
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }

  GFL_TCBL_Exit( p_wk->p_tcbl );

  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );

  GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  APP_KEYCURSOR_Delete( p_wk->p_keycursor );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �`�揈��
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk   ���[�N
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_Main( MYSTERY_TEXT_WORK* p_wk )
{ 
  switch( p_wk->print_update )
  { 
  default:
    /* fallthor */
  case MYSTERY_TEXT_TYPE_NONE:
    break;

  case MYSTERY_TEXT_TYPE_QUE:
    p_wk->is_end_print  = PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
    break;

  case MYSTERY_TEXT_TYPE_STREAM:
    if( p_wk->p_stream )
    { 
      PRINTSTREAM_STATE state;
      state  = PRINTSYS_PrintStreamGetState( p_wk->p_stream );

      APP_KEYCURSOR_Main( p_wk->p_keycursor, p_wk->p_stream, p_wk->p_bmpwin );

      switch( state )
      { 
      case PRINTSTREAM_STATE_RUNNING:  ///< �������s���i�����񂪗���Ă���j

        // ���b�Z�[�W�X�L�b�v
        if( GFL_UI_KEY_GetCont() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) )
        {
          PRINTSYS_PrintStreamShortWait( p_wk->p_stream, 0 );
        }
        break;

      case PRINTSTREAM_STATE_PAUSE:    ///< �ꎞ��~���i�y�[�W�؂�ւ��҂����j

        //���s
        if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) )
        { 
          PRINTSYS_PrintStreamReleasePause( p_wk->p_stream );
        }
        break;

      case PRINTSTREAM_STATE_DONE:     ///< ������I�[�܂ŕ\������
        p_wk->is_end_print  = TRUE;
        break;
      }
    }
    break;
  }

  GFL_TCBL_Main( p_wk->p_tcbl );

}
//----------------------------------------------------------------------------
/*
 *	@brief  �e�L�X�g �v�����g�J�n
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param  type              �`��^�C�v
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_Print( MYSTERY_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, MYSTERY_TEXT_TYPE type )
{ 
  //������쐬
  GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //�����`��
  MYSTERY_TEXT_PrintInner( p_wk, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �v�����g�J�n  �o�b�t�@��
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk ���[�N
 *	@param	STRBUF *cp_strbuf       �����o�b�t�@
 *	@param	type                    �`��^�C�v
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_PrintBuf( MYSTERY_TEXT_WORK* p_wk, const STRBUF *cp_strbuf, MYSTERY_TEXT_TYPE type )
{ 
  //������쐬
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //�����`��
  MYSTERY_TEXT_PrintInner( p_wk, type );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�����`��v���C�x�[�g
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk ���[�N
 *	@param	type              �`��^�C�v
 *
 */
//-----------------------------------------------------------------------------
static void MYSTERY_TEXT_PrintInner( MYSTERY_TEXT_WORK* p_wk, MYSTERY_TEXT_TYPE type )
{ 
  //��[����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );

  //�X�g���[���j��
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }

  //�^�C�v���Ƃ̕����`��
  switch( type )
  { 
  case MYSTERY_TEXT_TYPE_QUE:     //�v�����g�L���[���g��
    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );
    p_wk->print_update  = MYSTERY_TEXT_TYPE_QUE;
    break;

  case MYSTERY_TEXT_TYPE_STREAM:  //�X�g���[�����g��
    p_wk->p_stream  = PRINTSYS_PrintStream( p_wk->p_bmpwin, 0, 0, p_wk->p_strbuf,
        p_wk->p_font, MSGSPEED_GetWait(), p_wk->p_tcbl, 0, p_wk->heapID, p_wk->clear_chr );
    p_wk->print_update  = MYSTERY_TEXT_TYPE_STREAM;
    break;
  }

  p_wk->is_end_print  = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �v�����g�I���҂�  �iQUE�̏ꍇ��QUE�I���ASTREAM�̂Ƃ��͍Ō�܂ŕ����`��I���j
 *
 *	@param	const MYSTERY_TEXT_WORK *cp_wk ���[�N
 *
 *	@return TRUE�Ȃ�Ε����`�抮��  FALSE�Ȃ�΍Œ��B
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_TEXT_IsEndPrint( const MYSTERY_TEXT_WORK *cp_wk )
{ 
  return cp_wk->is_end_print;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �g��`��
 *
 *	@param	MYSTERY_TEXT_WORK *p_wk   ���[�N
 *	@param  �t���[���̃L����
 *	@param  �t���[���̃p���b�g
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_WriteWindowFrame( MYSTERY_TEXT_WORK *p_wk, u16 frm_chr, u8 frm_plt )
{ 
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_ON, frm_chr, frm_plt );
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ���X�g
 *				    �E�ȒP�Ƀ��X�g���o�����߂ɍő�l���߂���
 *            �E�\���������X�g�ő吔
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================/
#define MYSTERY_LIST_W  (12)
//-------------------------------------
///	�I�����X�g
//=====================================
struct _MYSTERY_LIST_WORK
{ 
  GFL_BMPWIN        *p_bmpwin;
  PRINT_QUE         *p_que;
  PRINT_UTIL        print_util;
  BMPMENULIST_WORK  *p_list;
  BMP_MENULIST_DATA *p_list_data;
};
//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������
 *
 *	@param	MYSTERY_LIST_SETUP *cp_setup  ���X�g�ݒ胏�[�N
 *	@param	heapID                    heapID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
MYSTERY_LIST_WORK * MYSTERY_LIST_Init( const MYSTERY_LIST_SETUP *cp_setup, HEAPID heapID )
{ 
  MYSTERY_LIST_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_LIST_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_LIST_WORK) );
  p_wk->p_que = cp_setup->p_que;

  //BMPWIN�쐬
  { 
    //�E���A�e�L�X�g�{�b�N�X�̏�Ɉʒu���邽��
    //�\�����ڂ���ʒu�A�������v�Z
    const u8 w  = MYSTERY_LIST_W;
    const u8 h  = cp_setup->list_max * 2;
    const u8 x  = 32 - w - 1; //1�̓t���[����
    const u8 y  = 24 - h - 1 - 6; //�P�͎����̃t���[������6�̓e�L�X�g��
    p_wk->p_bmpwin  = GFL_BMPWIN_Create( cp_setup->frm, x, y, w, h, cp_setup->font_plt, GFL_BMP_CHRAREA_GET_B );
    BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_OFF, cp_setup->frm_chr, cp_setup->frm_plt );
    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

    PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );
  }
  //���X�g�f�[�^�쐬
  { 
    int i;
    p_wk->p_list_data = BmpMenuWork_ListCreate( cp_setup->list_max, heapID );
    for( i = 0; i < cp_setup->list_max; i++ )
    { 
      BmpMenuWork_ListAddArchiveString( p_wk->p_list_data,
          cp_setup->p_msg, cp_setup->strID[ i ], i, heapID );
    }
  }
  //���X�g�쐬
  { 
    static const BMPMENULIST_HEADER sc_def_header =
    { 
      NULL,
      NULL,
      NULL,
      NULL,

      0,  //count
      0,  //line
      0,  //rabel_x
      13, //data_x  
      0,  //cursor_x
      3,  //line_y
      1,  //f
      15, //b
      2,  //s
      0,  //msg_spc
      1,  //line_spc
      BMPMENULIST_NO_SKIP,  //page_skip
      0,  //font
      0,  //c_disp_f
      NULL,
      12,
      12,

      NULL,
      NULL,
      NULL,
      NULL,
      
    };
    BMPMENULIST_HEADER  header;
    header  = sc_def_header;

    header.list         = p_wk->p_list_data;
    header.count        = cp_setup->list_max;
    header.line         = cp_setup->list_max;
    header.win          = p_wk->p_bmpwin;
    header.msgdata      = cp_setup->p_msg;
    header.print_util   = &p_wk->print_util;
    header.print_que    = cp_setup->p_que;
    header.font_handle  = cp_setup->p_font;
    p_wk->p_list  = BmpMenuList_Set( &header, 0, 0, heapID );

    BmpMenuList_SetCursorBmp( p_wk->p_list, heapID );
    BmpMenuList_SetCancelMode( p_wk->p_list, BMPMENULIST_CANCELMODE_USE );
    
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�j������
 *
 *	@param	MYSTERY_LIST_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_LIST_Exit( MYSTERY_LIST_WORK *p_wk )
{ 
  BmpMenuList_Exit( p_wk->p_list, NULL, NULL );
  BmpMenuWork_ListDelete( p_wk->p_list_data );

  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g���C������
 *
 *	@param	MYSTERY_LIST_WORK *p_wk   ���[�N
 *
 *	@return �I�����Ă��Ȃ��Ȃ��MYSTERY_LIST_SELECT_NULL ����ȊO�Ȃ�ΑI���������X�g�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 MYSTERY_LIST_Main( MYSTERY_LIST_WORK *p_wk )
{ 
  u32 ret;
  PRINT_UTIL_Trans( &p_wk->print_util, p_wk->p_que );
  ret = BmpMenuList_Main( p_wk->p_list );

  if( ret == BMPMENULIST_CANCEL )
  { 
    //�L�����Z���̂Ƃ��́A
    //��Ԍ����I�񂾂��Ƃɂ���
    ret = BmpListParamGet( p_wk->p_list, BMPMENULIST_ID_COUNT ) - 1;
  }

  return ret;
}
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  BMPWIN�̏W�܂�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
struct _MYSTERY_MSGWINSET_WORK
{ 
  const MYSTERY_MSGWINSET_SETUP_TBL *cp_tbl;
  u32 tbl_len;
  MYSTERY_MSGWIN_WORK *p_msgwin[0];
};
//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET ������
 *
 *	@param	const MYSTERY_MSGWINSET_SETUP_TBL *cp_tbl �ݒ�e�[�u��
 *	@param	tbl_len                                   �e�[�u����
 *	@param	frm                                       �t���[��
 *	@param	plt                                       �p���b�g
 *	@param	*p_que                                    �L���[
 *	@param	*p_msg                                    ���b�Z�[�W
 *	@param	*p_font                                   �t�H���g
 *	@param	heapID                                    �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
MYSTERY_MSGWINSET_WORK * MYSTERY_MSGWINSET_Init( const MYSTERY_MSGWINSET_SETUP_TBL *cp_tbl, u32 tbl_len, u16 frm, u8 plt, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, GFL_FONT *p_font, HEAPID heapID )
{ 
  MYSTERY_MSGWINSET_WORK *p_wk;
  u32 size;

  size  = sizeof(MYSTERY_MSGWINSET_WORK) + sizeof(MYSTERY_MSGWIN_WORK *) * tbl_len;
  p_wk  = GFL_HEAP_AllocMemory( heapID, size );
  GFL_STD_MemClear( p_wk, size );
  p_wk->cp_tbl  = cp_tbl;
  p_wk->tbl_len = tbl_len;

  //�쐬  ���@��������
  { 
    int i;
    const MYSTERY_MSGWINSET_SETUP_TBL *cp_setup;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      cp_setup  = &cp_tbl[i];
      p_wk->p_msgwin[i] = MYSTERY_MSGWIN_Init( frm, cp_setup->x, cp_setup->y, 
          cp_setup->w, cp_setup->h, plt, p_que, heapID );

      if( cp_setup->p_strbuf )
      { 
        MYSTERY_MSGWIN_PrintBuf( p_wk->p_msgwin[i], cp_setup->p_strbuf, p_font );
      }
      else
      { 
        MYSTERY_MSGWIN_Print( p_wk->p_msgwin[i], p_msg, cp_setup->strID, p_font );
      }
    }
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET �j��
 *
 *	@param	MYSTERY_MSGWINSET_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWINSET_Exit( MYSTERY_MSGWINSET_WORK* p_wk )
{ 
  { 
    int i;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      MYSTERY_MSGWIN_Exit( p_wk->p_msgwin[i] );
    }
  }

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET �v�����g�X
 *
 *	@param	MYSTERY_MSGWINSET_WORK* p_wk  ���[�N
 *
 *	@return TRUE�ŕ`�抮��  FALSE�ŕ`�撆
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_MSGWINSET_PrintMain( MYSTERY_MSGWINSET_WORK* p_wk )
{ 
  BOOL ret  = TRUE;

  { 
    int i;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      ret &= MYSTERY_MSGWIN_PrintMain( p_wk->p_msgwin[i] );
    }
  }

  return ret;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ���j���[
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
#define MYSTERY_MENU_BMPWIN_X             (2)
#define MYSTERY_MENU_BMPWIN_Y             (3)
#define MYSTERY_MENU_BMPWIN_W             (28)
#define MYSTERY_MENU_BMPWIN_TOP_MARGIN    (1)
#define MYSTERY_MENU_BMPWIN_BOTTOM_MARGIN (1)
#define MYSTERY_MENU_BMPWIN_LEFT_MARGIN   (1)

//-------------------------------------
///	�I�����X�g
//=====================================
struct _MYSTERY_MENU_WORK
{ 
  GFL_BMPWIN        *p_bmpwin;
  PRINT_QUE         *p_que;
  PRINT_UTIL        print_util;
  STRBUF            *p_strbuf[ MYSTERY_MENU_WINDOW_MAX ];
  GFL_CLWK          *p_cursor;
  u32               select;
  u32               list_max;
  u16								clear_chr;	//�N���A�L����
  PRINT_UTIL				util;				//�����`��
  u16               color[ MYSTERY_MENU_WINDOW_MAX ];
  u16               cnt;
  u16               font_plt;
};
//-------------------------------------
///	�v���g�^�C�v
//=====================================
static void PalletFadeMain( u16 *p_buff, u16 *p_cnt, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end );
//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���j���[  ������
 *
 *	@param	const MYSTERY_MENU_SETUP *cp_setup  �ݒ�\����
 *	@param	heapID                              �q�[�vID 
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
MYSTERY_MENU_WORK * MYSTERY_MENU_Init( const MYSTERY_MENU_SETUP *cp_setup, HEAPID heapID )
{
  MYSTERY_MENU_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_MENU_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_MENU_WORK) );
  p_wk->p_que			= cp_setup->p_que;
  p_wk->list_max  = cp_setup->list_max;
  p_wk->p_cursor  = cp_setup->p_cursor;
  p_wk->font_plt  = cp_setup->font_plt;

  //�P�T���R�s�[
  { 
    int i;
    static const u16 sc_back_plt  = GX_RGB(31,31,31);
 //   const u32 src = HW_BG_PLTT+cp_setup->font_plt*0x20+0xf*2;

    for( i = 0; i < p_wk->list_max; i++ )
    {
      const u32 dst = HW_BG_PLTT+cp_setup->font_plt*0x20+(0xF-(i+1))*2;
      GFL_STD_MemCopy(&sc_back_plt,(void*)(dst),2);
    }
  }


  //BMPWIN�쐬
  p_wk->p_bmpwin  = GFL_BMPWIN_Create( cp_setup->frm, MYSTERY_MENU_BMPWIN_X, MYSTERY_MENU_BMPWIN_Y, MYSTERY_MENU_BMPWIN_W, cp_setup->list_max*2+MYSTERY_MENU_BMPWIN_BOTTOM_MARGIN+MYSTERY_MENU_BMPWIN_TOP_MARGIN, cp_setup->font_plt, GFL_BMP_CHRAREA_GET_B );
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_ON, cp_setup->frm_chr, cp_setup->frm_plt );

  //�v�����g�L���[�ݒ�
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

  //BMPWIN�]��
  { 
    int i;
    s16 pos_y;
    
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_wk->p_bmpwin ), 0xF );
    for( i = 0; i < p_wk->list_max; i++ )
    {
      pos_y   = MYSTERY_MENU_BMPWIN_TOP_MARGIN*8 + i*16;
      GFL_BMP_Fill( GFL_BMPWIN_GetBmp( p_wk->p_bmpwin ),
          0, pos_y, MYSTERY_MENU_BMPWIN_W*8, 16, 0xF-(i+1) );
    }

    for( i = 0; i < p_wk->list_max; i++ )
    {
      if( cp_setup->p_msg )
      { 
        p_wk->p_strbuf[i]	= GFL_MSG_CreateString( cp_setup->p_msg, cp_setup->strID[ i ] );
      }
      else
      { 
        p_wk->p_strbuf[i]	= GFL_STR_CreateCopyBuffer( cp_setup->p_strbuf[i], heapID );
      }

      //������`��
      PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, MYSTERY_MENU_BMPWIN_LEFT_MARGIN*8,
          MYSTERY_MENU_BMPWIN_TOP_MARGIN*8+16*i, p_wk->p_strbuf[i], cp_setup->p_font );
    }
  }
  GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  //�J�[�\���ݒ�
  {
    GFL_CLACTPOS  pos;
    pos.x = 128;
    pos.y = 32+0;
    GFL_CLACT_WK_SetPos( p_wk->p_cursor, &pos, CLSYS_DEFREND_MAIN );
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor, TRUE );
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���j���[  �j��
 *
 *	@param	MYSTERY_MENU_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_MENU_Exit( MYSTERY_MENU_WORK *p_wk )
{ 
  GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor, FALSE );
  { 
    int i;
    for( i = 0; i < p_wk->list_max; i++ )
    { 
      GFL_STR_DeleteBuffer( p_wk->p_strbuf[i] );
    }
  }
  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���j���[  ���C������
 *
 *	@param	MYSTERY_MENU_WORK *p_wk   ���[�N
 *
 *	@return �I����������
 */
//-----------------------------------------------------------------------------
u32 MYSTERY_MENU_Main( MYSTERY_MENU_WORK *p_wk )
{ 
  int trg = GFL_UI_KEY_GetTrg(); 
  int repeat  = GFL_UI_KEY_GetRepeat();
  BOOL is_update  = FALSE;

  //����
  if( repeat & PAD_KEY_UP )
  { 
    if( p_wk->select == 0 )
    { 
      p_wk->select  = p_wk->list_max - 1;
    }
    else
    { 
      p_wk->select--;
    }
    is_update  = TRUE;
  }
  else if( repeat & PAD_KEY_DOWN )
  { 
    p_wk->select++;
    p_wk->select  %= p_wk->list_max;
    is_update  = TRUE;
  }
  else if( trg & PAD_BUTTON_DECIDE )
  { 
    return p_wk->select;
  }
  else if( trg & PAD_BUTTON_CANCEL )
  { 
    p_wk->select  = p_wk->list_max-1;
    return p_wk->select;
  }

  //�A�b�v�f�[�g
  if( is_update )
  {
    //�J�[�\���ݒ�
    {
      GFL_CLACTPOS  pos;
      pos.x = 128;
      pos.y = (MYSTERY_MENU_BMPWIN_Y+MYSTERY_MENU_BMPWIN_TOP_MARGIN)*8+16*p_wk->select;
      GFL_CLACT_WK_SetPos( p_wk->p_cursor, &pos, CLSYS_DEFREND_MAIN );
    }
  }

  //�����`��
  PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );


  //�p���b�g�A�j��
  if( p_wk->cnt + 0x400 >= 0x10000 )
  {
    p_wk->cnt = p_wk->cnt+0x400-0x10000;
  }
  else
  {
    p_wk->cnt += 0x400;
  }

  return MYSTERY_MENU_SELECT_NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�̔w�i�𖾖�
 *
 *	@param	MYSTERY_MENU_WORK *p_wk   ���[�N
 *	@param	list_num                  ���ł����郊�X�g
 */
//-----------------------------------------------------------------------------
void MYSTERY_MENU_BlinkMain( MYSTERY_MENU_WORK *p_wk, u32 list_num )
{ 
  PalletFadeMain( &p_wk->color[ list_num ], &p_wk->cnt, p_wk->font_plt,
      0xF-(list_num+1), GX_RGB( 31,31,31 ), GX_RGB( 16, 31, 31 ) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  BG�@�p���b�g�t�F�[�h
 *
 *  @param  u16 *p_buff �F�ۑ��o�b�t�@�iVBlank�]���̂��߁j
 *  @param  *p_cnt      �J�E���^�o�b�t�@
 *  @param  add         �J�E���^���Z�l
 *  @param  plt_num     �p���b�g�c�ԍ�
 *  @param  plt_col     �p���b�g���ԍ�
 *  @param  start       �J�n�F
 *  @param  end         �I���F
 */
//-----------------------------------------------------------------------------
static void PalletFadeMain( u16 *p_buff, u16 *p_cnt, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{
  {
    //1�`0�ɕϊ�
    const fx16 cos = (FX_CosIdx(*p_cnt)+FX16_ONE)/2;
    const u8 start_r  = (start & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 start_g  = (start & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 start_b  = (start & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    const u8 end_r  = (end & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 end_g  = (end & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 end_b  = (end & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
    const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
    const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);

    *p_buff = GX_RGB(r, g, b);

    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        plt_num * 32 + plt_col *2 ,
                                        p_buff, 2 );
  }

}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �V�[�P���X�Ǘ�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
struct _MYSTERY_SEQ_WORK
{
	MYSTERY_SEQ_FUNCTION	seq_function;		//���s���̃V�[�P���X�֐�
	BOOL is_end;									//�V�[�P���X�V�X�e���I���t���O
	int seq;											//���s���̃V�[�P���X�֐��̒��̃V�[�P���X
	void *p_wk_adrs;							//���s���̃V�[�P���X�֐��ɓn�����[�N
  int reserv_seq;               //�\��V�[�P���X
};

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	������
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk	���[�N
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
MYSTERY_SEQ_WORK * MYSTERY_SEQ_Init( void *p_wk_adrs, MYSTERY_SEQ_FUNCTION seq_function, HEAPID heapID )
{	
  MYSTERY_SEQ_WORK *p_wk;

	//�쐬
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_SEQ_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(MYSTERY_SEQ_WORK) );

	//������
	p_wk->p_wk_adrs	= p_wk_adrs;

	//�Z�b�g
	MYSTERY_SEQ_SetNext( p_wk, seq_function );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_Exit( MYSTERY_SEQ_WORK *p_wk )
{
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_Main( MYSTERY_SEQ_WORK *p_wk )
{	
	if( !p_wk->is_end )
	{	
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_wk_adrs );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I���擾
 *
 *	@param	const MYSTERY_SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
BOOL MYSTERY_SEQ_IsEnd( const MYSTERY_SEQ_WORK *cp_wk )
{	
	return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_SetNext( MYSTERY_SEQ_WORK *p_wk, MYSTERY_SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I��
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_End( MYSTERY_SEQ_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ ���̃V�[�P���X��\��
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk  ���[�N
 *	@param	seq             ���̃V�[�P���X
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_SetReservSeq( MYSTERY_SEQ_WORK *p_wk, int seq )
{ 
  p_wk->reserv_seq  = seq;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ �\�񂳂ꂽ�V�[�P���X�֔��
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_NextReservSeq( MYSTERY_SEQ_WORK *p_wk )
{ 
  p_wk->seq = p_wk->reserv_seq;
}
