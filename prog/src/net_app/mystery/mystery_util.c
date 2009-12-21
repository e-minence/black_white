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

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
//MYSTERY_MSGWIN_WORK�Ɠ���

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  ������
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
MYSTERY_TEXT_WORK * MYSTERY_TEXT_Init( u16 frm, u8 font_plt, u8 frm_plt, u16 frm_chr, PRINT_QUE *p_que, HEAPID heapID )
{ 
  MYSTERY_MSGWIN_WORK *p_wk;

  p_wk  = MYSTERY_MSGWIN_Init( frm, 1, 19, 30, 4, font_plt, p_que, heapID );
  p_wk->clear_chr = 0xF;
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_ON, frm_chr, frm_plt );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g1�s  ������
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
MYSTERY_TEXT_WORK * MYSTERY_TEXT_InitOneLine( u16 frm, u8 font_plt, u8 frm_plt, u16 frm_chr, PRINT_QUE *p_que, HEAPID heapID )
{ 
  MYSTERY_MSGWIN_WORK *p_wk;

  p_wk  = MYSTERY_MSGWIN_Init( frm, 1, 21, 30, 2, font_plt, p_que, heapID );
  p_wk->clear_chr = 0xF;
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_ON, frm_chr, frm_plt );

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
  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  MYSTERY_MSGWIN_Exit( p_wk );
}
//----------------------------------------------------------------------------
/*
 *	@brief  �e�L�X�g �v�����g�J�n
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_Print( MYSTERY_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font )
{ 
	MYSTERY_MSGWIN_Print( p_wk, p_msg, strID, p_font );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �v�����g�҂�
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk   ���[�N
 *
 *	@return TRUE�Ńv�����g����  FALSE�Ńv�����g��
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_TEXT_PrintMain( MYSTERY_TEXT_WORK* p_wk )
{ 
  return MYSTERY_MSGWIN_PrintMain( p_wk );
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
  BOOL is_update  = FALSE;

  //����
  if( trg & PAD_KEY_UP )
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
  else if( trg & PAD_KEY_DOWN )
  { 
    p_wk->select++;
    p_wk->select  %= p_wk->list_max;
    is_update  = TRUE;
  }
  else if( trg & PAD_BUTTON_A )
  { 
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
