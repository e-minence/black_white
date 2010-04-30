//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_util.c
 *	@brief  �f�o�b�O�Ŏg�p���郂�W���[����
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
#include "app/app_printsys_common.h"
#include "sound/pm_sndsys.h"
#include "system/time_icon.h"

//�A�[�J�C�u
#include "arc_def.h"

//�f�o�b�O
#include "debug/debug_str_conv.h"

//�����\��
#include "print/printsys.h"

//�O�����J
#include "debug_util.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					BMPWIN���b�Z�[�W�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
struct _DEBUG_MSGWIN_WORK
{
	PRINT_UTIL				util;				//�����`��
	PRINT_QUE					*p_que;			//�����L���[
	GFL_BMPWIN*				p_bmpwin;		//BMPWIN
	STRBUF*						p_strbuf;		//�����o�b�t�@
	u16								frm;        //BG��
  PRINTSYS_LSB      color;      //�����F
  GFL_POINT         ofs;        //�����`��ʒu�I�t�Z�b�g
  DEBUG_MSGWIN_POS pos_type;  //�`��ʒu�^�C�v
  DEBUG_MSGWIN_TRANS_MODE mode; //�]�����[�h
};
//-------------------------------------
///	���W�v�Z
//=====================================
static void Mystery_Msgwin_CalcPos( const DEBUG_MSGWIN_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos );
static void Mystery_Msgwin_CalcPosCore( DEBUG_MSGWIN_POS type, const GFL_POINT *cp_ofs, GFL_BMP_DATA *p_bmp, STRBUF *p_strbuf, GFL_FONT *p_font, GFL_POINT *p_pos );
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
DEBUG_MSGWIN_WORK * DEBUG_MSGWIN_Init( DEBUG_MSGWIN_TRANS_MODE mode, u16 frm, u8 x, u8 y, u8 w, u8 h, u8 plt, PRINT_QUE *p_que, HEAPID heapID )
{ 
  DEBUG_MSGWIN_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_MSGWIN_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_MSGWIN_WORK) );
  p_wk->p_que			= p_que;
  p_wk->frm       = frm;
  p_wk->color     = DEBUG_MSGWIN_DEFAULT_COLOR;
  p_wk->mode      = mode;

  //�o�b�t�@�쐬
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 512, heapID );

	//BMPWIN�쐬
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, x, y, w, h, plt, GFL_BMP_CHRAREA_GET_B );

	//�v�����g�L���[�ݒ�
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//�]��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), PRINTSYS_LSB_GetB( p_wk->color ) );
  if( p_wk->mode == DEBUG_MSGWIN_TRANS_MODE_AUTO )
  { 
    GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
    GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin );
    GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame( p_wk->p_bmpwin ) );
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �j��
 *
 *	@param	DEBUG_MSGWIN_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWIN_Exit( DEBUG_MSGWIN_WORK* p_wk )
{ 
  //BMPWIN�j��
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );

	//�o�b�t�@�j��
	GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �N���A
 *
 *	@param	DEBUG_MSGWIN_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWIN_Clear( DEBUG_MSGWIN_WORK* p_wk )
{ 
  GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �v�����g�J�n
 *
 *	@param	DEBUG_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWIN_Print( DEBUG_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), PRINTSYS_LSB_GetB( p_wk->color ) );	

	//������쐬
	GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //�ʒu�v�Z
  Mystery_Msgwin_CalcPos( p_wk, p_font, &pos );

	//������`��
	PRINT_UTIL_PrintColor( &p_wk->util, p_wk->p_que, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  STRBUF�v�����g�J�n
 *
 *	@param	DEBUG_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_strbuf         STRBUF  �����j�����Ă����܂��܂���
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWIN_PrintBuf( DEBUG_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), PRINTSYS_LSB_GetB( p_wk->color ) );	

	//������쐬
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //�ʒu�v�Z
  Mystery_Msgwin_CalcPos( p_wk, p_font, &pos );

	//������`��
	PRINT_UTIL_PrintColor( &p_wk->util, p_wk->p_que, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �F�w��
 *
 *	@param	DEBUG_MSGWIN_WORK* p_wk ���[�N
 *	@param  lsb               �F�w��
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWIN_SetColor( DEBUG_MSGWIN_WORK* p_wk, PRINTSYS_LSB color )
{
  p_wk->color = color;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �ʒu�w��
 *
 *	@param	DEBUG_MSGWIN_WORK* p_wk ���[�N
 *	@param	x                       X���W
 *	@param	y                       Y���W
 *	@param	type                    �w��
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWIN_SetPos( DEBUG_MSGWIN_WORK* p_wk, s16 x, s16 y, DEBUG_MSGWIN_POS type )
{ 
  p_wk->ofs.x = x;
  p_wk->ofs.y = y;
  p_wk->pos_type  = type;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �v�����g�������ݏ���
 *
 *	@param	DEBUG_MSGWIN_WORK* p_wk   ���[�N
 *
 *	@return TRUE�Ȃ�Ώ�������  FALSE�Ȃ�΂܂�
 */
//-----------------------------------------------------------------------------
BOOL DEBUG_MSGWIN_PrintMain( DEBUG_MSGWIN_WORK* p_wk )
{ 
  if( p_wk->mode == DEBUG_MSGWIN_TRANS_MODE_AUTO )
  { 
    return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
  }
  else
  {
    if( p_wk->util.transReq )
    {
      if( !PRINTSYS_QUE_IsExistTarget(p_wk->p_que, GFL_BMPWIN_GetBmp(p_wk->util.win)) )
      {
        p_wk->util.transReq = FALSE;
      }
    }
    return !(p_wk->util.transReq);
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W�]���֐�
 *
 *	@param	DEBUG_MSGWIN_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWIN_Trans( DEBUG_MSGWIN_WORK* p_wk )
{ 
  GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
  if( p_wk->mode == DEBUG_MSGWIN_TRANS_MODE_MANUAL )
  { 
    GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin );
    GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( p_wk->p_bmpwin ) );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �`����W�v�Z
 *
 *	@param	const DEBUG_MSGWIN_WORK* cp_wk  ���[�N
 *	@param  �t�H���g
 *	@param	*p_pos                            �`��ʒu�󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void Mystery_Msgwin_CalcPos( const DEBUG_MSGWIN_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos )
{ 
  Mystery_Msgwin_CalcPosCore( cp_wk->pos_type, &cp_wk->ofs, GFL_BMPWIN_GetBmp(cp_wk->p_bmpwin), cp_wk->p_strbuf, p_font, p_pos );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �`��ʒu�v�Z�R�A����
 *
 *	@param	DEBUG_MSGWIN_POS type �ʒu�̎��
 *	@param  cp_ofs                  �ʒu�I�t�Z�b�g
 *	@param	GFL_BMP_DATA *cp_bmp    BMP
 *	@param	*p_strbuf               ������
 *	@param	*p_font                 �t�H���g
 *	@param	*p_pos                  ���W�󂯎��
 */
//-----------------------------------------------------------------------------
static void Mystery_Msgwin_CalcPosCore( DEBUG_MSGWIN_POS type, const GFL_POINT *cp_ofs, GFL_BMP_DATA *p_bmp, STRBUF *p_strbuf, GFL_FONT *p_font, GFL_POINT *p_pos )
{ 
  switch( type )
  { 
  case DEBUG_MSGWIN_POS_ABSOLUTE:     //��Έʒu
    *p_pos  = *cp_ofs;
    break;
  case DEBUG_MSGWIN_POS_WH_CENTER:  //���΍��W  ���A�����Ƃ��ɒ���
    { 
      u32 x, y;
      x = GFL_BMP_GetSizeX( p_bmp ) / 2;
      y = GFL_BMP_GetSizeY( p_bmp ) / 2;
      x -= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 ) / 2;
      y -= PRINTSYS_GetStrHeight( p_strbuf, p_font ) / 2;
      p_pos->x  = x + cp_ofs->x;
      p_pos->y  = y + cp_ofs->y;
    }
    break;
  case DEBUG_MSGWIN_POS_H_CENTER:    //���΍��W  �����̂ݒ���
    { 
      u32 y;
      y = GFL_BMP_GetSizeY( p_bmp ) / 2;
      y -= PRINTSYS_GetStrHeight( p_strbuf, p_font ) / 2;
      p_pos->x  = cp_ofs->x;
      p_pos->y  = y + cp_ofs->y;
    }
    break;
  }
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�e�L�X�g�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
#define DEBUG_TEXT_TYPE_NONE  (DEBUG_TEXT_TYPE_MAX)

//-------------------------------------
/// ���b�Z�[�W�E�B���h�E
//=====================================
struct _DEBUG_TEXT_WORK
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
  APP_PRINTSYS_COMMON_WORK  common;
  TIMEICON_WORK     *p_time;
} ;

//-------------------------------------
///	�v���g�^�C�v
//=====================================
static void DEBUG_TEXT_PrintInner( DEBUG_TEXT_WORK* p_wk, DEBUG_TEXT_TYPE type );

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
DEBUG_TEXT_WORK * DEBUG_TEXT_Init( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  DEBUG_TEXT_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_TEXT_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_TEXT_WORK) );
  p_wk->clear_chr = 0xF;
  p_wk->p_font    = p_font;
  p_wk->p_que     = p_que;
  p_wk->print_update  = DEBUG_TEXT_TYPE_NONE;


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
DEBUG_TEXT_WORK * DEBUG_TEXT_InitOneLine( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  DEBUG_TEXT_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_TEXT_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_TEXT_WORK) );
  p_wk->clear_chr = 0xF;
  p_wk->p_font    = p_font;
  p_wk->p_que     = p_que;
  p_wk->print_update  = DEBUG_TEXT_TYPE_NONE;

  APP_PRINTSYS_COMMON_PrintStreamInit( &p_wk->common, APP_PRINTSYS_COMMON_TYPE_KEY);

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
 *	@param	DEBUG_TEXT_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_TEXT_Exit( DEBUG_TEXT_WORK* p_wk )
{ 
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }
  if( p_wk->p_time )
  { 
    TIMEICON_Exit( p_wk->p_time );
    p_wk->p_time  = NULL;
  }
  if( p_wk->p_keycursor )
  { 
    APP_KEYCURSOR_Delete( p_wk->p_keycursor );
  }

  GFL_TCBL_Exit( p_wk->p_tcbl );

  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );

  GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �`�揈��
 *
 *	@param	DEBUG_TEXT_WORK* p_wk   ���[�N
 *
 */
//-----------------------------------------------------------------------------
void DEBUG_TEXT_Main( DEBUG_TEXT_WORK* p_wk )
{ 
  switch( p_wk->print_update )
  { 
  default:
    /* fallthor */
  case DEBUG_TEXT_TYPE_NONE:
    break;

  case DEBUG_TEXT_TYPE_WAIT:
    PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
    break;

  case DEBUG_TEXT_TYPE_QUE:
    p_wk->is_end_print  = PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
    break;

  case DEBUG_TEXT_TYPE_STREAM:
    if( p_wk->p_stream )
    { 
      BOOL is_end;

      if( p_wk->p_keycursor )
      { 
        APP_KEYCURSOR_Main( p_wk->p_keycursor, p_wk->p_stream, p_wk->p_bmpwin );
      }

      is_end  = APP_PRINTSYS_COMMON_PrintStreamFunc( &p_wk->common, p_wk->p_stream );
      if( is_end )
      { 
        p_wk->is_end_print  = TRUE;
      }
    }
  }

  GFL_TCBL_Main( p_wk->p_tcbl );

}
//----------------------------------------------------------------------------
/*
 *	@brief  �e�L�X�g �v�����g�J�n
 *
 *	@param	DEBUG_TEXT_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param  type              �`��^�C�v
 */
//-----------------------------------------------------------------------------
void DEBUG_TEXT_Print( DEBUG_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, DEBUG_TEXT_TYPE type )
{ 
  //������쐬
  GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //�����`��
  DEBUG_TEXT_PrintInner( p_wk, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �v�����g�J�n  �o�b�t�@��
 *
 *	@param	DEBUG_TEXT_WORK* p_wk ���[�N
 *	@param	STRBUF *cp_strbuf       �����o�b�t�@
 *	@param	type                    �`��^�C�v
 *
 */
//-----------------------------------------------------------------------------
void DEBUG_TEXT_PrintBuf( DEBUG_TEXT_WORK* p_wk, const STRBUF *cp_strbuf, DEBUG_TEXT_TYPE type )
{ 
  //������쐬
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //�����`��
  DEBUG_TEXT_PrintInner( p_wk, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�����`��v���C�x�[�g
 *
 *	@param	DEBUG_TEXT_WORK* p_wk ���[�N
 *	@param	type              �`��^�C�v
 *
 */
//-----------------------------------------------------------------------------
static void DEBUG_TEXT_PrintInner( DEBUG_TEXT_WORK* p_wk, DEBUG_TEXT_TYPE type )
{ 
  //��[����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );

  //�X�g���[���j��
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }

  if( p_wk->p_keycursor )
  { 
    APP_KEYCURSOR_Delete( p_wk->p_keycursor );
    p_wk->p_keycursor = NULL;
  }

  DEBUG_TEXT_EndWait( p_wk );

  //�^�C�v���Ƃ̕����`��
  switch( type )
  { 
  case DEBUG_TEXT_TYPE_WAIT:    //�ҋ@�A�C�R��
    p_wk->p_time  = TIMEICON_Create( GFUser_VIntr_GetTCBSYS(), p_wk->p_bmpwin, p_wk->clear_chr,
        TIMEICON_DEFAULT_WAIT, p_wk->heapID );

    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );
    p_wk->print_update  = DEBUG_TEXT_TYPE_QUE;
    break;

  case DEBUG_TEXT_TYPE_QUE:     //�v�����g�L���[���g��
    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );
    p_wk->print_update  = DEBUG_TEXT_TYPE_QUE;
    break;

  case DEBUG_TEXT_TYPE_STREAM:  //�X�g���[�����g��

    APP_PRINTSYS_COMMON_PrintStreamInit( &p_wk->common, APP_PRINTSYS_COMMON_TYPE_KEY);
    p_wk->p_keycursor  = APP_KEYCURSOR_Create( p_wk->clear_chr, TRUE, FALSE, p_wk->heapID );
    p_wk->p_stream  = PRINTSYS_PrintStream( p_wk->p_bmpwin, 0, 0, p_wk->p_strbuf,
        p_wk->p_font, MSGSPEED_GetWait(), p_wk->p_tcbl, 0, p_wk->heapID, p_wk->clear_chr );
    p_wk->print_update  = DEBUG_TEXT_TYPE_STREAM;
    break;
  }

  p_wk->is_end_print  = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �v�����g�I���҂�  �iQUE�̏ꍇ��QUE�I���ASTREAM�̂Ƃ��͍Ō�܂ŕ����`��I���j
 *
 *	@param	const DEBUG_TEXT_WORK *cp_wk ���[�N
 *
 *	@return TRUE�Ȃ�Ε����`�抮��  FALSE�Ȃ�΍Œ��B
 */
//-----------------------------------------------------------------------------
BOOL DEBUG_TEXT_IsEndPrint( const DEBUG_TEXT_WORK *cp_wk )
{ 
  return cp_wk->is_end_print;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �g��`��
 *
 *	@param	DEBUG_TEXT_WORK *p_wk   ���[�N
 *	@param  �t���[���̃L����
 *	@param  �t���[���̃p���b�g
 */
//-----------------------------------------------------------------------------
void DEBUG_TEXT_WriteWindowFrame( DEBUG_TEXT_WORK *p_wk, u16 frm_chr, u8 frm_plt )
{ 
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_ON, frm_chr, frm_plt );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �ҋ@�A�C�R��������
 *
 *	@param	WBM_TEXT_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_TEXT_EndWait( DEBUG_TEXT_WORK *p_wk )
{ 
  p_wk->is_end_print  = TRUE;
  if( p_wk->p_time )
  { 
    TIMEICON_Exit( p_wk->p_time );
    p_wk->p_time  = NULL;

    //�^�C���A�C�R���̂��Ƃ̓X�N���[�������Ă��邽�߁A�ē]��
    GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin );
    GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(p_wk->p_bmpwin) );
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
#define DEBUG_LIST_W  (12)
//-------------------------------------
///	�I�����X�g
//=====================================
struct _DEBUG_LIST_WORK
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
 *	@param	DEBUG_LIST_SETUP *cp_setup  ���X�g�ݒ胏�[�N
 *	@param	heapID                    heapID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
DEBUG_LIST_WORK * DEBUG_LIST_Init( const DEBUG_LIST_SETUP *cp_setup, HEAPID heapID )
{ 
  DEBUG_LIST_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_LIST_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_LIST_WORK) );
  p_wk->p_que = cp_setup->p_que;

  //BMPWIN�쐬
  { 
    //�E���A�e�L�X�g�{�b�N�X�̏�Ɉʒu���邽��
    //�\�����ڂ���ʒu�A�������v�Z
    const u8 w  = DEBUG_LIST_W;
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
 *	@param	DEBUG_LIST_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_LIST_Exit( DEBUG_LIST_WORK *p_wk )
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
 *	@param	DEBUG_LIST_WORK *p_wk   ���[�N
 *
 *	@return �I�����Ă��Ȃ��Ȃ��DEBUG_LIST_SELECT_NULL ����ȊO�Ȃ�ΑI���������X�g�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 DEBUG_LIST_Main( DEBUG_LIST_WORK *p_wk )
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
struct _DEBUG_MSGWINSET_WORK
{ 
  const DEBUG_MSGWINSET_SETUP_TBL *cp_tbl;
  u32 tbl_len;
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg;
  DEBUG_MSGWIN_WORK *p_msgwin[0];
};
//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET ������
 *  
 *  @param  mode                                      ���f���[�h
 *	@param	const DEBUG_MSGWINSET_SETUP_TBL *cp_tbl �ݒ�e�[�u��
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
DEBUG_MSGWINSET_WORK * DEBUG_MSGWINSET_Init( DEBUG_MSGWIN_TRANS_MODE mode, const DEBUG_MSGWINSET_SETUP_TBL *cp_tbl, u32 tbl_len, u16 frm, u8 plt, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, GFL_FONT *p_font, HEAPID heapID )
{ 
  DEBUG_MSGWINSET_WORK *p_wk;
  u32 size;

  size  = sizeof(DEBUG_MSGWINSET_WORK) + sizeof(DEBUG_MSGWIN_WORK *) * tbl_len;
  p_wk  = GFL_HEAP_AllocMemory( heapID, size );
  GFL_STD_MemClear( p_wk, size );
  p_wk->cp_tbl  = cp_tbl;
  p_wk->tbl_len = tbl_len;
  p_wk->p_font  = p_font;
  p_wk->p_msg   = p_msg;

  //�쐬  ���@��������
  { 
    int i;
    const DEBUG_MSGWINSET_SETUP_TBL *cp_setup;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      cp_setup  = &cp_tbl[i];
      p_wk->p_msgwin[i] = DEBUG_MSGWIN_Init( mode, frm, cp_setup->x, cp_setup->y, 
          cp_setup->w, cp_setup->h, plt, p_que, heapID );

      DEBUG_MSGWIN_SetPos( p_wk->p_msgwin[i], cp_setup->pos_x, cp_setup->pos_y, cp_setup->pos_type );
      DEBUG_MSGWIN_SetColor( p_wk->p_msgwin[i], cp_setup->color );

      if( cp_setup->p_strbuf )
      { 
        DEBUG_MSGWIN_PrintBuf( p_wk->p_msgwin[i], cp_setup->p_strbuf, p_wk->p_font );
      }
      else
      { 
        DEBUG_MSGWIN_Print( p_wk->p_msgwin[i], p_msg, cp_setup->strID, p_wk->p_font );
      }
    }
  }

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ĕ`��  
 *
 *	@param	DEBUG_MSGWINSET_WORK* p_wk            ���[�N
 *	@param	DEBUG_MSGWINSET_PRINT_TBL *cp_print   �������݃f�[�^
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWINSET_Print( DEBUG_MSGWINSET_WORK* p_wk, const DEBUG_MSGWINSET_PRINT_TBL *cp_tbl )
{ 
  //�쐬  ���@��������
  { 
    int i;
    const DEBUG_MSGWINSET_PRINT_TBL *cp_print;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      cp_print  = &cp_tbl[i];

      if( cp_print->is_print )
      { 
        DEBUG_MSGWIN_SetPos( p_wk->p_msgwin[i], cp_print->pos_x, cp_print->pos_y, cp_print->pos_type );
        DEBUG_MSGWIN_SetColor( p_wk->p_msgwin[i], cp_print->color );

        if( cp_print->p_strbuf )
        { 
          DEBUG_MSGWIN_PrintBuf( p_wk->p_msgwin[i], cp_print->p_strbuf, p_wk->p_font );
        }
        else
        { 
          DEBUG_MSGWIN_Print( p_wk->p_msgwin[i], p_wk->p_msg, cp_print->strID, p_wk->p_font );
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET �j��
 *
 *	@param	DEBUG_MSGWINSET_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWINSET_Exit( DEBUG_MSGWINSET_WORK* p_wk )
{ 
  { 
    int i;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      DEBUG_MSGWIN_Exit( p_wk->p_msgwin[i] );
    }
  }

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET�N���A
 *
 *	@param	DEBUG_MSGWINSET_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWINSET_Clear( DEBUG_MSGWINSET_WORK* p_wk )
{ 
  { 
    int i;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      DEBUG_MSGWIN_Clear( p_wk->p_msgwin[i] );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET �v�����g�X
 *
 *	@param	DEBUG_MSGWINSET_WORK* p_wk  ���[�N
 *
 *	@return TRUE�ŕ`�抮��  FALSE�ŕ`�撆
 */
//-----------------------------------------------------------------------------
BOOL DEBUG_MSGWINSET_PrintMain( DEBUG_MSGWINSET_WORK* p_wk )
{ 
  BOOL ret  = TRUE;

  { 
    int i;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      ret &= DEBUG_MSGWIN_PrintMain( p_wk->p_msgwin[i] );
    }
  }

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ʏ���
 *
 *	@param	DEBUG_MSGWINSET_WORK* p_wk  ���[�N
 *	@param	idx                           �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWINSET_ClearOne( DEBUG_MSGWINSET_WORK* p_wk, u16 idx )
{ 
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_msgwin[idx]->p_bmpwin),
      PRINTSYS_LSB_GetB( p_wk->p_msgwin[idx]->color ) );
  GFL_BMPWIN_TransVramCharacter( p_wk->p_msgwin[idx]->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET DEBUG_MSGWIN_TRANS_MODE_AUTO���[�h�̏ꍇ�g�p����]���֐�
 *
 *	@param	DEBUG_MSGWINSET_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWINSET_Trans( DEBUG_MSGWINSET_WORK* p_wk )
{ 
  int i;
  for( i = 0; i < p_wk->tbl_len; i++ )
  { 
    DEBUG_MSGWIN_Trans( p_wk->p_msgwin[i] );
  }
}
//=============================================================================
/**
 *    ��������ɕ`�悵�Ă����A��C�ɏ������ނ��߂̃o�b�t�@
 */
//=============================================================================
struct _DEBUG_MSGWINBUFF_WORK
{ 
  HEAPID heapID;
  DEBUG_MSGWINSET_WORK  *p_winset;
  GFL_BMP_DATA  *p_bmp[0];
};

//----------------------------------------------------------------------------
/**
 *	@brief  �o�b�t�@�̍쐬
 *
 *	@param	DEBUG_MSGWINSET_WORK *p_wk  ���[�N
 *	@param	heapID                        �q�[�vID
 *
 *	@return �o�b�t�@
 */
//-----------------------------------------------------------------------------
DEBUG_MSGWINBUFF_WORK *DEBUG_MSGWINSET_CreateBuff( DEBUG_MSGWINSET_WORK *p_wk, HEAPID heapID )
{ 
  u32 size;
  DEBUG_MSGWINBUFF_WORK *p_buff;

  size    = sizeof(DEBUG_MSGWINBUFF_WORK) + sizeof(GFL_BMP_DATA*) * p_wk->tbl_len;
  p_buff  = GFL_HEAP_AllocMemory( heapID, size );
  GFL_STD_MemClear( p_buff, size );
  p_buff->p_winset  = p_wk;
  p_buff->heapID    = heapID;

  { 
    int i;
    GFL_BMP_DATA  *p_src;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      p_src  = GFL_BMPWIN_GetBmp( p_wk->p_msgwin[i]->p_bmpwin );
      p_buff->p_bmp[i]  = GFL_BMP_Create( GFL_BMP_GetSizeX(p_src)/8, 
          GFL_BMP_GetSizeY(p_src)/8, GFL_BMP_GetColorFormat(p_src), heapID );
    }
  }

  return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�b�t�@�̔j��
 *
 *	@param	DEBUG_MSGWINBUFF_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWINSET_DeleteBuff( DEBUG_MSGWINBUFF_WORK *p_wk )
{ 
  int i;
  for( i = 0; i < p_wk->p_winset->tbl_len; i++ )
  { 
    GFL_BMP_Delete( p_wk->p_bmp[i] );
  }

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�b�t�@�ւ̏�������
 *
 *	@param	DEBUG_MSGWINBUFF_WORK  *p_wk        ���[�N
 *	@param	DEBUG_MSGWINSET_PRINT_TBL *cp_tbl �������݃e�[�u��
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWINSET_PrintBuff( DEBUG_MSGWINBUFF_WORK *p_wk, const DEBUG_MSGWINSET_PRINT_TBL *cp_tbl, PRINT_QUE *p_que )
{ 
  //�쐬  ���@��������
  { 
    int i;
    const DEBUG_MSGWINSET_PRINT_TBL *cp_print;
    GFL_POINT ofs;
    GFL_POINT pos;

    STRBUF  *p_strbuf = GFL_STR_CreateBuffer( 512, p_wk->heapID );

    for( i = 0; i < p_wk->p_winset->tbl_len; i++ )
    { 
      cp_print  = &cp_tbl[i];

      if( cp_print->is_print )
      { 
        GFL_POINT pos;

        //��[����
        GFL_BMP_Clear( p_wk->p_bmp[i], PRINTSYS_LSB_GetB( cp_print->color ) );	

        //�ʒu�v�Z
        ofs.x = cp_print->pos_x;
        ofs.y = cp_print->pos_y;
        Mystery_Msgwin_CalcPosCore( cp_print->pos_type, &ofs, p_wk->p_bmp[i], p_strbuf, p_wk->p_winset->p_font, &pos );

        //������쐬
        if( cp_print->p_strbuf )
        { 
          GFL_STR_CopyBuffer( p_strbuf, cp_print->p_strbuf );
        }
        else
        { 
          GFL_MSG_GetString( p_wk->p_winset->p_msg, cp_print->strID, p_strbuf );
        }

        //������`��
        PRINTSYS_PrintQueColor( p_que, p_wk->p_bmp[i], pos.x, pos.y, p_strbuf, p_wk->p_winset->p_font, cp_print->color );
      }
    }

    GFL_HEAP_FreeMemory( p_strbuf );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�b�t�@�̐ݒ�i�]����Trans�֐��ōs�Ȃ��j
 *  
 *	@param	DEBUG_MSGWINSET_WORK *p_wk    ���[�N
 *	@param	DEBUG_MSGWINBUFF_WORK *cp_buff �o�b�t�@
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGWINSET_SetBuff( DEBUG_MSGWINBUFF_WORK *p_wk )
{ 
  int i;
  GFL_BMP_DATA  *p_src_bmp;
  GFL_BMP_DATA  *p_dst_bmp;
  u8  *p_src;
  u8  *p_dst;


  for( i = 0; i < p_wk->p_winset->tbl_len; i++ )
  { 
    p_src_bmp = p_wk->p_bmp[i];
    p_dst_bmp = GFL_BMPWIN_GetBmp(p_wk->p_winset->p_msgwin[i]->p_bmpwin);
    GFL_BMP_Copy( p_src_bmp, p_dst_bmp );
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
struct _DEBUG_SEQ_WORK
{
	DEBUG_SEQ_FUNCTION	seq_function;		//���s���̃V�[�P���X�֐�
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
 *	@param	DEBUG_SEQ_WORK *p_wk	���[�N
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
DEBUG_SEQ_WORK * DEBUG_SEQ_Init( void *p_wk_adrs, DEBUG_SEQ_FUNCTION seq_function, HEAPID heapID )
{	
  DEBUG_SEQ_WORK *p_wk;

	//�쐬
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_SEQ_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_SEQ_WORK) );

	//������
	p_wk->p_wk_adrs	= p_wk_adrs;

	//�Z�b�g
	DEBUG_SEQ_SetNext( p_wk, seq_function );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	DEBUG_SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_SEQ_Exit( DEBUG_SEQ_WORK *p_wk )
{
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	DEBUG_SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
void DEBUG_SEQ_Main( DEBUG_SEQ_WORK *p_wk )
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
 *	@param	const DEBUG_SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
BOOL DEBUG_SEQ_IsEnd( const DEBUG_SEQ_WORK *cp_wk )
{	
	return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	DEBUG_SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
void DEBUG_SEQ_SetNext( DEBUG_SEQ_WORK *p_wk, DEBUG_SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I��
 *
 *	@param	DEBUG_SEQ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
void DEBUG_SEQ_End( DEBUG_SEQ_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ ���̃V�[�P���X��\��
 *
 *	@param	DEBUG_SEQ_WORK *p_wk  ���[�N
 *	@param	seq             ���̃V�[�P���X
 */
//-----------------------------------------------------------------------------
void DEBUG_SEQ_SetReservSeq( DEBUG_SEQ_WORK *p_wk, int seq )
{ 
  p_wk->reserv_seq  = seq;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ �\�񂳂ꂽ�V�[�P���X�֔��
 *
 *	@param	DEBUG_SEQ_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_SEQ_NextReservSeq( DEBUG_SEQ_WORK *p_wk )
{ 
  p_wk->seq = p_wk->reserv_seq;
}
//=============================================================================
/**
 *    MSGOAM
 */
//=============================================================================
//-------------------------------------
///	�萔
//=====================================
//-------------------------------------
///	���b�Z�[�WOAM
//=====================================
struct _DEBUG_MSGOAM_WORK
{ 
  GFL_BMP_DATA        *p_bmp;     //�����pBMP
  PRINTSYS_LSB        color;      //�����F
  BOOL                trans_req;  //BMP�]�����N�G�X�g�t���O
  GFL_POINT           ofs;        //�����`��ʒu�I�t�Z�b�g
  DEBUG_MSGOAM_POS  pos_type;   //�`��ʒu�^�C�v
  STRBUF              *p_strbuf;  //�����o�b�t�@
  BMPOAM_ACT_PTR		  p_bmpoam_wk;  //BMPOAM
  PRINT_QUE           *p_que;       //�`��L���[
};

//-------------------------------------
///	�����`��ʒu�v�Z
//=====================================
static void Mystery_Msgoam_CalcPos( const DEBUG_MSGOAM_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos );

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  �쐬
 *
 *	@param	const GFL_CLWK_DATA *cp_data  OAM���
 *	@param	w                             ��  �L�����P��
 *	@param	h                             ����  �L�����P��
 *	@param	plt_idx                       GFL_CLGRP�ɓo�^�����p���b�g�C���f�b�N�X
 *	@param  plt_ofs                       �p���b�g�I�t�Z�b�g
 *	@param	p_bmpoam_sys                  BMPOAM�V�X�e��
 *	@param	*p_que  ���b�Z�[�W�`��L���[
 *	@param	heapID  �q�[�vID
 *
 *	@return MSGOAM���[�N
 */
//-----------------------------------------------------------------------------
DEBUG_MSGOAM_WORK * DEBUG_MSGOAM_Init( const GFL_CLWK_DATA *cp_cldata, u8 w, u8 h, u32 plt_idx, u8 plt_ofs, CLSYS_DRAW_TYPE draw_type, BMPOAM_SYS_PTR p_bmpoam_sys, PRINT_QUE *p_que, HEAPID heapID )
{ 
  DEBUG_MSGOAM_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_MSGOAM_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_MSGOAM_WORK) );
  p_wk->p_que = p_que;

  //�o�b�t�@�쐬
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 128, heapID );

  //BMP�쐬
  p_wk->p_bmp = GFL_BMP_Create( w, h, GFL_BMP_16_COLOR, heapID );

	//BMPOAM�쐬
	{	
		BMPOAM_ACT_DATA	actdata;
		
		GFL_STD_MemClear( &actdata, sizeof(BMPOAM_ACT_DATA) );
		actdata.bmp	= p_wk->p_bmp;
		actdata.x		= cp_cldata->pos_x;
		actdata.y		= cp_cldata->pos_y;
		actdata.pltt_index	= plt_idx;
		actdata.soft_pri		= cp_cldata->softpri;
		actdata.setSerface	= draw_type;
		actdata.draw_type		= draw_type;
		actdata.bg_pri			= cp_cldata->bgpri;
    actdata.pal_offset  = plt_ofs;
		p_wk->p_bmpoam_wk	  = BmpOam_ActorAdd( p_bmpoam_sys, &actdata );
	}

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM�@�j��
 *
 *	@param	DEBUG_MSGOAM_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGOAM_Exit( DEBUG_MSGOAM_WORK* p_wk )
{ 
  BmpOam_ActorDel( p_wk->p_bmpoam_wk );
  GFL_BMP_Delete( p_wk->p_bmp );
  GFL_STR_DeleteBuffer( p_wk->p_strbuf );
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  �w�i�F�ŃN���A
 *
 *	@param	DEBUG_MSGOAM_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGOAM_Clear( DEBUG_MSGOAM_WORK* p_wk )
{
  GFL_BMP_Clear( p_wk->p_bmp, PRINTSYS_LSB_GetB(p_wk->color) );
  BmpOam_ActorBmpTrans( p_wk->p_bmpoam_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  �����`��
 *
 *	@param	DEBUG_MSGOAM_WORK* p_wk ���[�N
 *	@param	*p_msg                    ���b�Z�[�W
 *	@param	strID                     �����ԍ�
 *	@param	*p_font                   �t�H���g
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGOAM_Print( DEBUG_MSGOAM_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//��[����
	GFL_BMP_Clear( p_wk->p_bmp, PRINTSYS_LSB_GetB( p_wk->color ) );	

	//������쐬
	GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //�ʒu�v�Z
  Mystery_Msgoam_CalcPos( p_wk, p_font, &pos );

	//������`��
  PRINTSYS_PrintQueColor( p_wk->p_que, p_wk->p_bmp, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
  p_wk->trans_req = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  �����`��STRBUF��
 *
 *	@param	DEBUG_MSGOAM_WORK* p_wk     ���[�N
 *	@param	STRBUF *cp_strbuf             ������
 *	@param	*p_font                       �t�H���g
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGOAM_PrintBuf( DEBUG_MSGOAM_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//��[����
	GFL_BMP_Clear( p_wk->p_bmp, PRINTSYS_LSB_GetB( p_wk->color ) );	

	//������쐬
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //�ʒu�v�Z
  Mystery_Msgoam_CalcPos( p_wk, p_font, &pos );

	//������`��
  PRINTSYS_PrintQueColor( p_wk->p_que, p_wk->p_bmp, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
  p_wk->trans_req = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����F��ݒ�
 *
 *	@param	DEBUG_MSGOAM_WORK* p_wk   ���[�N
 *	@param	color                       �����F
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGOAM_SetStrColor( DEBUG_MSGOAM_WORK* p_wk, PRINTSYS_LSB color )
{ 
  p_wk->color = color;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����`��ʒu�ݒ�
 *
 *	@param	DEBUG_MSGOAM_WORK* p_wk ���[�N
 *	@param	x ���W
 *	@param	y ���W
 *	@param	type  ���W�^�C�v
 */
//-----------------------------------------------------------------------------
void DEBUG_MSGOAM_SetStrPos( DEBUG_MSGOAM_WORK* p_wk, s16 x, s16 y, DEBUG_MSGOAM_POS type )
{ 
  p_wk->ofs.x = x;
  p_wk->ofs.y = y;
  p_wk->pos_type  = type;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �\������
 *
 *	@param	DEBUG_MSGOAM_WORK* p_wk ���[�N
 *
 *	@return TRUE�ŕ`�抮��  FALSE�ŕ`�撆
 */
//-----------------------------------------------------------------------------
BOOL DEBUG_MSGOAM_PrintMain( DEBUG_MSGOAM_WORK* p_wk )
{ 
  if( p_wk->trans_req )
  { 
    if( !PRINTSYS_QUE_IsExistTarget( p_wk->p_que, p_wk->p_bmp ) )
    { 
      BmpOam_ActorBmpTrans( p_wk->p_bmpoam_wk );
      p_wk->trans_req = FALSE;
    }
  }

  return !p_wk->trans_req;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPOAM�擾
 *
 *	@param	DEBUG_MSGOAM_WORK* p_wk   ���[�N
 *
 *	@return BMPOAM
 */
//-----------------------------------------------------------------------------
BMPOAM_ACT_PTR DEBUG_MSGOAM_GetBmpOamAct( DEBUG_MSGOAM_WORK* p_wk )
{ 
  return p_wk->p_bmpoam_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �`����W�v�Z
 *
 *	@param	const DEBUG_MSGOAM_WORK* cp_wk  ���[�N
 *	@param  �t�H���g
 *	@param	*p_pos                            �`��ʒu�󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void Mystery_Msgoam_CalcPos( const DEBUG_MSGOAM_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos )
{ 
  switch( cp_wk->pos_type )
  { 
  case DEBUG_MSGOAM_POS_ABSOLUTE:     //��Έʒu
    *p_pos  = cp_wk->ofs;
    break;
  case DEBUG_MSGOAM_POS_WH_CENTER:  //���΍��W  ���A�����Ƃ��ɒ���
    { 
      u32 x, y;
      x = GFL_BMP_GetSizeX( cp_wk->p_bmp ) / 2;
      y = GFL_BMP_GetSizeY( cp_wk->p_bmp ) / 2;
      x -= PRINTSYS_GetStrWidth( cp_wk->p_strbuf, p_font, 0 ) / 2;
      y -= PRINTSYS_GetStrHeight( cp_wk->p_strbuf, p_font ) / 2;
      p_pos->x  = x + cp_wk->ofs.x;
      p_pos->y  = y + cp_wk->ofs.y;
    }
    break;
  case DEBUG_MSGOAM_POS_H_CENTER:    //���΍��W  �����̂ݒ���
    { 
      u32 y;
      y = GFL_BMP_GetSizeY( cp_wk->p_bmp ) / 2;
      y -= PRINTSYS_GetStrHeight( cp_wk->p_strbuf, p_font ) / 2;
      p_pos->x  = cp_wk->ofs.x;
      p_pos->y  = y + cp_wk->ofs.y;
    }
    break;
  }
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ���l�㉺
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���b�Z�[�WOAM
//=====================================
struct _DEBUG_NUMIN_WORK
{ 
  GFL_BMPWIN  *p_bmpwin;
  GFL_FONT    *p_font;
  s32         min;
  s32         max;
  s32         now;
  u8          keta;
  u8          active;
  u8          frm;
  u8          plt;
  STRBUF      *p_strbuf;
  BOOL        is_end;
};

//-------------------------------------
///	�v���C�x�[�g
//=====================================
static u32 Debug_NumIn_AddKeta( u32 now, u32 keta, s8 dir );
static void Debug_NumIn_Print( DEBUG_NUMIN_WORK *p_wk );
static u8 GetFig( u32 now, u8 keta );
static void PRINT_GetStrWithNumber( STRBUF *p_dst, const char *cp_src, ... );

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���l����  ������
 *
 *	@param	DEBUG_NUMIN_WORK *p_wk  ���[�N
 *	@param	frm                     �t���[��
 *	@param	plt                     �p���b�g
 *	@param	*p_font                 �t�H���g
 *	@param	heapID                  �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
DEBUG_NUMIN_WORK * DEBUG_NUMIN_Init( u8 frm, u8 plt, GFL_FONT *p_font, HEAPID heapID )
{ 
  DEBUG_NUMIN_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_NUMIN_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_NUMIN_WORK) );

  p_wk->p_font  = p_font;
  p_wk->frm     = frm;
  p_wk->plt     = plt;
  p_wk->p_strbuf  = GFL_STR_CreateBuffer( 255, heapID );


  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���l����  �j��
 *
 *	@param	DEBUG_NUMIN_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_NUMIN_Exit( DEBUG_NUMIN_WORK *p_wk )
{ 
  if( p_wk->p_bmpwin )
  { 
    GFL_BMPWIN_Delete( p_wk->p_bmpwin );
  }

  GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���l���͊J�n
 *
 *	@param	DEBUG_NUMIN_WORK *p_wk  ���[�N
 *	@param	keta  ��
 *	@param	min   �ŏ�
 *	@param	max   �ő�
 *	@param	now   ����
 *	@param	x     X���W
 *	@param	y     Y���W
 */
//-----------------------------------------------------------------------------
void DEBUG_NUMIN_Start( DEBUG_NUMIN_WORK *p_wk, u8 keta, s32 min, s32 max, s32 now, u8 x, u8 y )
{ 
  if( p_wk->p_bmpwin == NULL )
  { 

    p_wk->keta  = keta;
    p_wk->min   = min;
    p_wk->max   = max;
    p_wk->now   = now;
    p_wk->is_end  = FALSE;

    p_wk->p_bmpwin  = GFL_BMPWIN_Create( p_wk->frm, x, y, 6, 2, p_wk->plt, GFL_BMP_CHRAREA_GET_B );

    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 15 );
    Debug_NumIn_Print( p_wk );
    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���l���̓��C��
 *
 *	@param	DEBUG_NUMIN_WORK *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void DEBUG_NUMIN_Main( DEBUG_NUMIN_WORK *p_wk )
{ 
  BOOL  is_update = FALSE;

  if( p_wk->p_bmpwin == NULL)
  { 
    return;
  }

  //���E�ŕύX���鐔����؂�ւ���
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  { 
    p_wk->active++;
    p_wk->active  %= p_wk->keta;

    is_update = TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat( ) & PAD_KEY_RIGHT )
  { 
    if( p_wk->active == 0 )
    { 
      p_wk->active  = p_wk->keta  - 1 ;
    }
    else
    { 
      p_wk->active--;
    }
    is_update = TRUE;
  }

  //�㉺�Ő�����؂�ւ���
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )
  { 
    p_wk->now = Debug_NumIn_AddKeta( p_wk->now, p_wk->active, 1 );
    p_wk->now = MATH_CLAMP( p_wk->now, p_wk->min, p_wk->max );
    is_update = TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )
  { 
    p_wk->now = Debug_NumIn_AddKeta( p_wk->now, p_wk->active, -1 );
    p_wk->now = MATH_CLAMP( p_wk->now, p_wk->min, p_wk->max );
    is_update = TRUE;
  }

  //����
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  { 
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );
    GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
    GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
    GFL_BMPWIN_Delete( p_wk->p_bmpwin );
    p_wk->p_bmpwin  = NULL;

    p_wk->is_end  = TRUE;
  }

  //�`��
  if( is_update )
  { 
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 15 );
    Debug_NumIn_Print( p_wk );
    GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  ���l����  ���l�擾
 *
 *	@param	const DEBUG_NUMIN_WORK *cp_wk ���[�N
 *
 *	@return ���͂������l
 */
//-----------------------------------------------------------------------------
u16  DEBUG_NUMIN_GetNum( const DEBUG_NUMIN_WORK *cp_wk )
{ 
  return cp_wk->now;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���l����  ���͂������������ǂ���
 *
 *	@param	const DEBUG_NUMIN_WORK *cp_wk   ���[�N
 *
 *	@return TRUE���͏I��  FALSE���͔j��
 */
//-----------------------------------------------------------------------------
BOOL DEBUG_NUMIN_IsEnd( const DEBUG_NUMIN_WORK *cp_wk )
{ 
  return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���𑫂�
 *
 *	@param	u32 now ����
 *	@param	keta    ��
 *	@param	dir     ����
 *
 *	@return   ���l
 */
//-----------------------------------------------------------------------------
static u32 Debug_NumIn_AddKeta( u32 now, u32 keta, s8 dir )
{ 
  u32 add   = 1;
  while( keta-- )
  { 
    add *= 10;
  }
    
  now +=  dir*add;
  return now;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �\��
 *
 *	@param	DEBUG_NUMIN_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Debug_NumIn_Print( DEBUG_NUMIN_WORK *p_wk )
{ 
  int i;
  PRINTSYS_LSB color;
  u8 fig;

  //�`��
  for( i = 0; i < p_wk->keta; i++ )
  { 
    fig =  GetFig( p_wk->now, p_wk->keta - i - 1 );

    PRINT_GetStrWithNumber( p_wk->p_strbuf, "%d", fig );

    if( p_wk->active  == p_wk->keta - i - 1 )
    { 
      color = PRINTSYS_LSB_Make( 3,4,15 );
    }
    else
    { 
      color = PRINTSYS_LSB_Make( 1,2,15 );
    }
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), i * 12, 0, p_wk->p_strbuf, p_wk->p_font, color );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���Ԗڂ̐��l���擾
 *
 *	@param	u32 now ����
 *	@param	keta    ��
 *
 *	@return keta�Ԗڂ̐��l
 */
//-----------------------------------------------------------------------------
static u8 GetFig( u32 now, u8 keta )
{ 
  static const u32 sc_keta_tbl[]  =
  { 
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
  };

  GF_ASSERT( keta < NELEMS(sc_keta_tbl) );
  return (now % sc_keta_tbl[ keta + 1 ]) / sc_keta_tbl[ keta ] ;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����ɐ��l�������
 *
 *	@param	STRBUF *p_dst   �󂯎��
 *	@param	u16 *cp_src     ������
 */
//-----------------------------------------------------------------------------
static void PRINT_GetStrWithNumber( STRBUF *p_dst, const char *cp_src, ... )
{ 
  char str[128];
  STRCODE str_code[128];
  u16 strlen;
  va_list vlist;

  //���l������
  va_start(vlist, cp_src);
  STD_TVSNPrintf( str, 128, cp_src, vlist );
  va_end(vlist);

  //STRBUF�p�ɕϊ�
  DEB_STR_CONV_SjisToStrcode( str, str_code, 128 );

  strlen  = GFL_STD_StrLen(str)*2;
  str_code[strlen] = GFL_STR_GetEOMCode();

  //STRBUF�ɓ]��
  GFL_STR_SetStringCode( p_dst, str_code);
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ETC
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�t�F�[�h
 *
 *	@param	NNS_GFD_DST_TYPE type �]����
 *	@param	*p_buff               �o�b�t�@
 *	@param	cnt                   �J�E���g
 *	@param	plt_num               �p���b�g�c
 *	@param	plt_col               �p���b�g��
 *	@param	start                 �J�n�F
 *	@param	end                   �I���F
 */
//-----------------------------------------------------------------------------
void DEBUG_UTIL_MainPltAnm( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{ 
  //0�`�P�ɕϊ�
  const fx16 cos = (FX_CosIdx(cnt)+FX16_ONE)/2;
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

  { 
    BOOL ret;
    ret = NNS_GfdRegisterNewVramTransferTask( type,
        plt_num * 32 + plt_col * 2,
        p_buff, 2 );

    GF_ASSERT(ret);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�t�F�[�h�P�s���ׂēh��ւ����
 *
 *	@param	NNS_GFD_DST_TYPE type �]����
 *	@param	*p_buff               �o�b�t�@0x10��
 *	@param	cnt                   �J�E���g
 *	@param	plt_num               �p���b�g�s
 *	@param	GXRgb start[]         �J�n�F0x10��
 *	@param	end[]                 �I���F0x10��
 */
//-----------------------------------------------------------------------------
void DEBUG_UTIL_MainPltAnmLine( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, GXRgb start[], GXRgb end[] )
{ 
  int i;
 //0�`�P�ɕϊ�
  for( i = 0; i < 0x10; i++ )
  { 
    const fx16 cos = (FX_CosIdx(cnt)+FX16_ONE)/2;
    const u8 start_r  = (start[i] & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 start_g  = (start[i] & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 start_b  = (start[i] & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    const u8 end_r  = (end[i] & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 end_g  = (end[i] & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 end_b  = (end[i] & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
    const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
    const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);

    p_buff[i] = GX_RGB(r, g, b);
  }

  { 
    BOOL ret;
    ret = NNS_GfdRegisterNewVramTransferTask( type,
        plt_num * 32,
        p_buff, 32 );
    GF_ASSERT(ret);
  }

}
