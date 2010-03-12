//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_util.c
 *	@brief  ���E�ΐ�p���ʃ��W���[��
 *	@author	Toru=Nagihashi
 *	@data		2009.11.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
//�V�X�e��
#include "system/gfl_use.h"
#include "gamesystem/msgspeed.h"
#include "system/bmp_winframe.h"
#include "app/app_keycursor.h"
#include "app/app_printsys_common.h"

//�����\��
#include "print/printsys.h"

#include "system/time_icon.h"

//�����̃��W���[��
#include "wifibattlematch_snd.h"

//�O�����J
#include "wifibattlematch_util.h"
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�e�L�X�g�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
#define WBM_TEXT_TYPE_NONE  (WBM_TEXT_TYPE_MAX)

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
struct _WBM_TEXT_WORK
{
  GFL_MSGDATA       *p_msg;
  GFL_FONT          *p_font;
  PRINT_STREAM      *p_stream;
  GFL_TCBLSYS       *p_tcbl;
  GFL_TCBSYS        *p_tcb;
  void              *p_tcb_area;
  TIMEICON_WORK     *p_time;
  GFL_BMPWIN*       p_bmpwin;
  STRBUF*           p_strbuf;
  u16               clear_chr;
  HEAPID            heapID;
  PRINT_UTIL        util;
  PRINT_QUE         *p_que;
  u32               print_update;
  BOOL              is_end_print;
  APP_KEYCURSOR_WORK* p_keycursor;
  APP_PRINTSYS_COMMON_WORK  common;
};
//-------------------------------------
///	�v���g�^�C�v
//=====================================
static void WBM_TEXT_PrintInner( WBM_TEXT_WORK* p_wk, WBM_TEXT_TYPE type );

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  ������
 *
 *	@param	u16 frm       BG��
 *	@param	font_plt      �t�H���g�p���b�g
 *	@param  frm_plt       �t���[���p���b�g
 *	@param  frm_chr       �t���[���L����
 *	@param	heapID  �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WBM_TEXT_WORK * WBM_TEXT_Init( u16 frm, u16 font_plt, u16 frm_plt, u16 frm_chr, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  WBM_TEXT_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WBM_TEXT_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(WBM_TEXT_WORK) );
  p_wk->clear_chr = 0xF;
  p_wk->p_font    = p_font;
  p_wk->p_que     = p_que;
  p_wk->print_update  = WBM_TEXT_TYPE_NONE;
  p_wk->heapID    = heapID;

  APP_PRINTSYS_COMMON_PrintStreamInit( &p_wk->common, APP_PRINTSYS_COMMON_TYPE_KEY);

  //��������J�[�\���쐬
  p_wk->p_keycursor  = APP_KEYCURSOR_Create( p_wk->clear_chr, TRUE, FALSE, heapID );

  //�o�b�t�@�쐬
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 512, heapID );

	//BMPWIN�쐬
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, 1, 19, 30, 4, font_plt, GFL_BMP_CHRAREA_GET_B );

	//�v�����g�L���[�ݒ�
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//�]��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  p_wk->p_tcbl      = GFL_TCBL_Init( heapID, heapID, 1, 32 );
  p_wk->p_tcb_area  = GFL_HEAP_AllocMemory( heapID, GFL_TCB_CalcSystemWorkSize( 1 ) );
  p_wk->p_tcb       = GFL_TCB_Init( 1, p_wk->p_tcb_area );

  //�t���[��
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_ON, frm_chr, frm_plt );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �j��
 *
 *	@param	WBM_TEXT_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WBM_TEXT_Exit( WBM_TEXT_WORK* p_wk )
{ 
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }

  if( p_wk->p_time )
  { 
    TILEICON_Exit( p_wk->p_time );
    p_wk->p_time  = NULL;
  }

  GFL_TCB_Exit( p_wk->p_tcb );
  GFL_HEAP_FreeMemory( p_wk->p_tcb_area );
  GFL_TCBL_Exit( p_wk->p_tcbl );

  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );

  GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  APP_KEYCURSOR_Delete( p_wk->p_keycursor );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W  ���C������
 *
 *	@param	WBM_TEXT_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WBM_TEXT_Main( WBM_TEXT_WORK* p_wk )
{ 
  switch( p_wk->print_update )
  { 
  default:
    /* fallthor */
  case WBM_TEXT_TYPE_NONE:
    break;
    
  case WBM_TEXT_TYPE_WAIT:
    PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
    break;

  case WBM_TEXT_TYPE_QUE:
    p_wk->is_end_print  = PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
    break;

  case WBM_TEXT_TYPE_STREAM:
    if( p_wk->p_stream )
    { 
      BOOL is_end;

      APP_KEYCURSOR_Main( p_wk->p_keycursor, p_wk->p_stream, p_wk->p_bmpwin );
      is_end  = APP_PRINTSYS_COMMON_PrintStreamFunc( &p_wk->common, p_wk->p_stream );

      if( is_end )
      { 
        p_wk->is_end_print  = TRUE;
      }
    }
    break;
  }

  GFL_TCBL_Main( p_wk->p_tcbl );
  GFL_TCB_Main( p_wk->p_tcb );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �v�����g�J�n
 *
 *	@param	WBM_TEXT_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void WBM_TEXT_Print( WBM_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, WBM_TEXT_TYPE type )
{ 
  //������쐬
  GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  WBM_TEXT_PrintInner( p_wk, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �v�����g�J�n  �i�f�o�b�O�Łj
 *
 *	@param	WBM_TEXT_WORK* p_wk ���[�N
 *	@param	str              ������   0xFFFF�I�[
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void WBM_TEXT_PrintDebug( WBM_TEXT_WORK* p_wk, const u16 *cp_str, u16 len, GFL_FONT *p_font )
{ 
	//������쐬
	GFL_STR_SetStringCodeOrderLength( p_wk->p_strbuf, cp_str, len );

  WBM_TEXT_PrintInner( p_wk, WBM_TEXT_TYPE_QUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�����`��v���C�x�[�g
 *
 *	@param	WBM_TEXT_WORK* p_wk ���[�N
 *	@param	type              �`��^�C�v
 *
 */
//-----------------------------------------------------------------------------
static void WBM_TEXT_PrintInner( WBM_TEXT_WORK* p_wk, WBM_TEXT_TYPE type )
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
  case WBM_TEXT_TYPE_WAIT:
    GF_ASSERT( p_wk->p_time == NULL );
    p_wk->p_time  = TIMEICON_Create( p_wk->p_tcb, p_wk->p_bmpwin, p_wk->clear_chr,
        TIMEICON_DEFAULT_WAIT, p_wk->heapID );
    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );
    p_wk->print_update  = WBM_TEXT_TYPE_QUE;
    break;

  case WBM_TEXT_TYPE_QUE:     //�v�����g�L���[���g��
    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );
    p_wk->print_update  = WBM_TEXT_TYPE_QUE;
    break;

  case WBM_TEXT_TYPE_STREAM:  //�X�g���[�����g��
    p_wk->p_stream  = PRINTSYS_PrintStream( p_wk->p_bmpwin, 0, 0, p_wk->p_strbuf,
        p_wk->p_font, MSGSPEED_GetWait(), p_wk->p_tcbl, 0, p_wk->heapID, p_wk->clear_chr );
    p_wk->print_update  = WBM_TEXT_TYPE_STREAM;
    break;
  }

  p_wk->is_end_print  = FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W  �I�����m
 *
 *	@param	const WBM_TEXT_WORK* cp_wk  ���[�N
 *
 *	@return TRUE�Ńv�����g�I����  FALSE�Ńv�����g��
 */
//-----------------------------------------------------------------------------
BOOL WBM_TEXT_IsEnd( const WBM_TEXT_WORK* cp_wk )
{ 
  return cp_wk->is_end_print;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W  �ҋ@�I��
 *
 *	@param	WBM_TEXT_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WBM_TEXT_EndWait( WBM_TEXT_WORK* p_wk )
{ 
  p_wk->is_end_print  = TRUE;
  if( p_wk->p_time )
  { 
    TILEICON_Exit( p_wk->p_time );
    p_wk->p_time  = NULL;
  }
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
#define WBM_LIST_W  (12)
//-------------------------------------
///	�I�����X�g
//=====================================
struct _WBM_LIST_WORK
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
 *	@param	WBM_LIST_SETUP *cp_setup  ���X�g�ݒ胏�[�N
 *	@param	heapID                    heapID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WBM_LIST_WORK * WBM_LIST_Init( const WBM_LIST_SETUP *cp_setup, HEAPID heapID )
{ 
  //�E���A�e�L�X�g�{�b�N�X�̏�Ɉʒu���邽��
  //�\�����ڂ���ʒu�A�������v�Z
  const u8 w  = WBM_LIST_W;
  const u8 h  = cp_setup->list_max * 2;
  const u8 x  = 32 - w - 1; //1�̓t���[����
  const u8 y  = 24 - h - 1 - 6; //�P�͎����̃t���[������6�̓e�L�X�g��
  return WBM_LIST_InitEx( cp_setup, x, y, w, h, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������
 *
 *	@param	WBM_LIST_SETUP *cp_setup  ���X�g�ݒ胏�[�N
 *	@param  x ���WX
 *	@param  y ���WY
 *	@param  w ��
 *	@param  h ����
 *	@param	heapID                    heapID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WBM_LIST_WORK * WBM_LIST_InitEx( const WBM_LIST_SETUP *cp_setup, u8 x, u8 y, u8 w, u8 h, HEAPID heapID )
{ 
  WBM_LIST_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WBM_LIST_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(WBM_LIST_WORK) );
  p_wk->p_que = cp_setup->p_que;

  //BMPWIN�쐬
  { 
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
 *	@param	WBM_LIST_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WBM_LIST_Exit( WBM_LIST_WORK *p_wk )
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
 *	@param	WBM_LIST_WORK *p_wk   ���[�N
 *
 *	@return �I�����Ă��Ȃ��Ȃ��WBM_LIST_SELECT_NULL ����ȊO�Ȃ�ΑI���������X�g�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 WBM_LIST_Main( WBM_LIST_WORK *p_wk )
{ 
  PRINT_UTIL_Trans( &p_wk->print_util, p_wk->p_que );
  return BmpMenuList_Main( p_wk->p_list );
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �V�[�P���X�Ǘ�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
struct _WBM_SEQ_WORK
{
	WBM_SEQ_FUNCTION	seq_function;		//���s���̃V�[�P���X�֐�
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
 *	@param	WBM_SEQ_WORK *p_wk	���[�N
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
WBM_SEQ_WORK * WBM_SEQ_Init( void *p_wk_adrs, WBM_SEQ_FUNCTION seq_function, HEAPID heapID )
{	
  WBM_SEQ_WORK *p_wk;

	//�쐬
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WBM_SEQ_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(WBM_SEQ_WORK) );

	//������
	p_wk->p_wk_adrs	= p_wk_adrs;

	//�Z�b�g
	WBM_SEQ_SetNext( p_wk, seq_function );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	WBM_SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WBM_SEQ_Exit( WBM_SEQ_WORK *p_wk )
{
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	WBM_SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
void WBM_SEQ_Main( WBM_SEQ_WORK *p_wk )
{	
	if( p_wk->seq_function )
	{	
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_wk_adrs );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I���擾
 *
 *	@param	const WBM_SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
BOOL WBM_SEQ_IsEnd( const WBM_SEQ_WORK *cp_wk )
{	
	return cp_wk->seq_function == NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	WBM_SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
void WBM_SEQ_SetNext( WBM_SEQ_WORK *p_wk, WBM_SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I��
 *
 *	@param	WBM_SEQ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
void WBM_SEQ_End( WBM_SEQ_WORK *p_wk )
{	
  WBM_SEQ_SetNext( p_wk, NULL );
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ ���̃V�[�P���X��\��
 *
 *	@param	WBM_SEQ_WORK *p_wk  ���[�N
 *	@param	seq             ���̃V�[�P���X
 */
//-----------------------------------------------------------------------------
void WBM_SEQ_SetReservSeq( WBM_SEQ_WORK *p_wk, int seq )
{ 
  p_wk->reserv_seq  = seq;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ �\�񂳂ꂽ�V�[�P���X�֔��
 *
 *	@param	WBM_SEQ_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WBM_SEQ_NextReservSeq( WBM_SEQ_WORK *p_wk )
{ 
  p_wk->seq = p_wk->reserv_seq;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �T�u�v���Z�X
 *				    �E�v���Z�X���s��������V�X�e��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���[�N
//=====================================
struct _WBM_SUBPROC_WORK
{
	GFL_PROCSYS			  *p_procsys;
	void						  *p_proc_param;

	HEAPID					  heapID;
	void						  *p_wk_adrs;
	const WBM_SUBPROC_DATA			*cp_procdata_tbl;
  u32               tbl_len;

	u8							  next_procID;
	u8							  now_procID;
	u16							  seq;

  GFL_PROC_MAIN_STATUS  status;
} ;
//-------------------------------------
///	�O�����J
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	WBM_SUBPROC�V�X�e��	������
 *
 *	@param	WBM_SUBPROC_WORK *p_wk	���[�N
 *	@param	cp_procdata_tbl			�v���Z�X�ڑ��e�[�u��
 *	@param	void *p_wk_adrs			�A���b�N�֐��Ɖ���֐��ɓn�����[�N
 *	@param	heapID							�V�X�e���\�z�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
WBM_SUBPROC_WORK * WBM_SUBPROC_Init( const WBM_SUBPROC_DATA *cp_procdata_tbl, u32 tbl_len, void *p_wk_adrs, HEAPID heapID )
{	
  WBM_SUBPROC_WORK *p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WBM_SUBPROC_WORK ) );
	GFL_STD_MemClear( p_wk, sizeof(WBM_SUBPROC_WORK) );
	p_wk->p_procsys				= GFL_PROC_LOCAL_boot( heapID );
	p_wk->p_wk_adrs				= p_wk_adrs;
	p_wk->cp_procdata_tbl	= cp_procdata_tbl;
  p_wk->tbl_len         = tbl_len;
	p_wk->heapID					= heapID;

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WBM_SUBPROC�V�X�e��	�j��
 *
 *	@param	WBM_SUBPROC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
void WBM_SUBPROC_Exit( WBM_SUBPROC_WORK *p_wk )
{	
	GF_ASSERT( p_wk->p_proc_param == NULL );

	GFL_PROC_LOCAL_Exit( p_wk->p_procsys );

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	WBM_SUBPROC�V�X�e��	���C������
 *
 *	@param	WBM_SUBPROC_WORK *p_wk	���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��	FALSE�Ȃ��PROC�����݂���
 */
//-----------------------------------------------------------------------------
BOOL WBM_SUBPROC_Main( WBM_SUBPROC_WORK *p_wk )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_ALLOC_PARAM,
		SEQ_MAIN,
		SEQ_FREE_PARAM,
    SEQ_NEXT,
		SEQ_END,
	};

	switch( p_wk->seq )
	{	
	case SEQ_INIT:
		p_wk->now_procID	= p_wk->next_procID;
		p_wk->seq	= SEQ_ALLOC_PARAM;
		break;

	case SEQ_ALLOC_PARAM:
		//�v���Z�X�����쐬�֐�������ΌĂяo��
		if( p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func )
		{	
			p_wk->p_proc_param	= p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func(
					p_wk->heapID, p_wk->p_wk_adrs );
		}
		else
		{	
			p_wk->p_proc_param	= NULL;
		}

		//�v���b�N�Ăяo��
		GFL_PROC_LOCAL_CallProc( p_wk->p_procsys, p_wk->cp_procdata_tbl[	p_wk->now_procID ].ov_id,
					p_wk->cp_procdata_tbl[	p_wk->now_procID ].cp_procdata, p_wk->p_proc_param );

		p_wk->seq	= SEQ_MAIN;
		break;

	case SEQ_MAIN:
		{	
			p_wk->status	= GFL_PROC_LOCAL_Main( p_wk->p_procsys );
			if( GFL_PROC_MAIN_NULL == p_wk->status )
			{	
				p_wk->seq	= SEQ_FREE_PARAM;
			}
		}
		break;

	case SEQ_FREE_PARAM:
		//�v���Z�X�����j���֐��Ăяo��
		if( p_wk->cp_procdata_tbl[	p_wk->now_procID ].free_func )
		{	
			if( p_wk->cp_procdata_tbl[	p_wk->now_procID ].free_func( p_wk->p_proc_param, p_wk->p_wk_adrs ) )
      { 
        p_wk->p_proc_param	= NULL;
				p_wk->seq	= SEQ_NEXT;
      }
		}
    else
    { 
				p_wk->seq	= SEQ_NEXT;
    }
    break;

  case SEQ_NEXT:
		//�������̃v���Z�X������ΌĂяo��
		//�Ȃ���ΏI��
		if( p_wk->now_procID != p_wk->next_procID )
		{	
			p_wk->seq	= SEQ_INIT;
		}
		else
		{	
			p_wk->seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WBM_SUBPROC�V�X�e�� PROC��Ԃ��擾
 *
 *	@param	const WBM_SUBPROC_WORK *cp_wk   ���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
GFL_PROC_MAIN_STATUS WBM_SUBPROC_GetStatus( const WBM_SUBPROC_WORK *cp_wk )
{ 
  return cp_wk->status;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WBM_SUBPROC�V�X�e��	�v���Z�X���N�G�X�g
 *
 *	@param	WBM_SUBPROC_WORK *p_wk	���[�N
 *	@param	proc_id							�Ăԃv���Z�XID
 *
 */
//-----------------------------------------------------------------------------
void WBM_SUBPROC_CallProc( WBM_SUBPROC_WORK *p_wk, u32 procID )
{	
	p_wk->next_procID	= procID;
}


