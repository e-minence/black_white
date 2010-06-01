//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_util.c
 *	@brief  �ePROC�Ŏg�����ʃ��W���[���n
 *	@author	Toru=Nagihashi
 *	@data		2009.11.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//�V�X�e��
#include "system/pms_draw.h"
#include "app/app_menu_common.h"
#include "pokeicon/pokeicon.h"
#include "system/nsbtx_to_clwk.h"

//�����̃��W���[��
#include "br_inner.h"
#include "br_snd.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "msg/msg_battle_rec.h"
#include "wifi_unionobj_plt.cdat" //���j�I��OBJ�̃p���b�g�ʒu
#include "wifi_unionobj.naix"

//�O�����J
#include "br_util.h"
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					BMPWIN���b�Z�[�W�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
struct _BR_MSGWIN_WORK
{
	PRINT_UTIL				util;				//�����`��
	PRINT_QUE					*p_que;			//�����L���[
	GFL_BMPWIN*				p_bmpwin;		//BMPWIN
	STRBUF*						p_strbuf;		//�����o�b�t�@
	u16								clear_chr;	//�N���A�L����
	u16								frm;        //BG��
  BR_MSGWIN_POS     pos_type;   //�ʒu�̎��
  GFL_POINT         pos;        //�ʒu
};
//-------------------------------------
///	�v���C�x�[�g
//=====================================
static void Br_MsgWin_CalcPos( BR_MSGWIN_POS  pos_type, const GFL_POINT *cp_ofs, GFL_BMP_DATA *p_bmp, STRBUF *p_strbuf, GFL_FONT *p_font, GFL_POINT *p_pos );

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
BR_MSGWIN_WORK * BR_MSGWIN_Init( u16 frm, u8 x, u8 y, u8 w, u8 h, u8 plt, PRINT_QUE *p_que, HEAPID heapID )
{ 
  BR_MSGWIN_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BR_MSGWIN_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BR_MSGWIN_WORK) );
  p_wk->clear_chr = 0;
  p_wk->p_que			= p_que;
  p_wk->frm       = frm;

  //�o�b�t�@�쐬
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 255, heapID );

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
 *	@param	BR_MSGWIN_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_MSGWIN_Exit( BR_MSGWIN_WORK* p_wk )
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
 *	@param	BR_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void BR_MSGWIN_Print( BR_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//������쐬
	GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //�`��ʒu�v�Z
  Br_MsgWin_CalcPos( p_wk->pos_type, &p_wk->pos, GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->p_strbuf, p_font, &pos );

	//������`��
	PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, pos.x, pos.y, p_wk->p_strbuf, p_font );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  STRBUF�v�����g�J�n
 *
 *	@param	BR_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_strbuf         STRBUF  �����j�����Ă����܂��܂���
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void BR_MSGWIN_PrintBuf( BR_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//������쐬
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //�`��ʒu�v�Z
  Br_MsgWin_CalcPos( p_wk->pos_type, &p_wk->pos, GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->p_strbuf, p_font, &pos );

	//������`��
	PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, pos.x, pos.y, p_wk->p_strbuf, p_font );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �v�����g�J�n  �F�w���
 *
 *	@param	BR_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param	*p_font           �t�H���g
 *	@param  lsb               �F�w��
 */
//-----------------------------------------------------------------------------
void BR_MSGWIN_PrintColor( BR_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font, PRINTSYS_LSB lsb )
{ 
  GFL_POINT pos;
  
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//������쐬
	GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //�`��ʒu�v�Z
  Br_MsgWin_CalcPos( p_wk->pos_type, &p_wk->pos, GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->p_strbuf, p_font, &pos );

	//������`��
	PRINT_UTIL_PrintColor( &p_wk->util, p_wk->p_que, pos.x, pos.y, p_wk->p_strbuf, p_font, lsb );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  STRBUF�v�����g�J�n
 *
 *	@param	BR_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_strbuf         STRBUF  �����j�����Ă����܂��܂���
 *	@param	*p_font           �t�H���g
 *	@param  lsb               �F�w��
 */
//-----------------------------------------------------------------------------
void BR_MSGWIN_PrintBufColor( BR_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font, PRINTSYS_LSB lsb )
{ 
  GFL_POINT pos;

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//������쐬
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //�`��ʒu�v�Z
  Br_MsgWin_CalcPos( p_wk->pos_type, &p_wk->pos, GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->p_strbuf, p_font, &pos );

	//������`��
	PRINT_UTIL_PrintColor( &p_wk->util, p_wk->p_que, pos.x, pos.y, p_wk->p_strbuf, p_font, lsb );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �v�����g�������ݏ���
 *
 *	@param	BR_MSGWIN_WORK* p_wk   ���[�N
 *
 *	@return TRUE�Ȃ�Ώ�������  FALSE�Ȃ�΂܂�
 */
//-----------------------------------------------------------------------------
BOOL BR_MSGWIN_PrintMain( BR_MSGWIN_WORK* p_wk )
{ 
	return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN���b�Z�[�W  �ʒu�ݒ�
 *
 *	@param	BR_MSGWIN_WORK* p_wk  ���[�N
 *	@param	x                  �ʒu
 *	@param	y                  �ʒu
 *	@param	type                �ʒu�̎��
 */
//-----------------------------------------------------------------------------
void BR_MSGWIN_SetPos( BR_MSGWIN_WORK* p_wk, s16 x, s16 y, BR_MSGWIN_POS type )
{ 
  p_wk->pos_type  = type;
  p_wk->pos.x       = x;
  p_wk->pos.y       = y;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`��ʒu�v�Z
 *
 *	@param	BR_MSGWIN_POS  pos_type �ʒu�̎��
 *	@param	GFL_POINT *cp_ofs       �ʒu
 *	@param	*p_bmp                  ��������BMP
 *	@param	*p_strbuf               ��������STR
 *	@param	*p_font                 �g�p����FONT
 *	@param	*p_pos                  �ʒu�󂯎��
 */
//-----------------------------------------------------------------------------
static void Br_MsgWin_CalcPos( BR_MSGWIN_POS  pos_type, const GFL_POINT *cp_ofs, GFL_BMP_DATA *p_bmp, STRBUF *p_strbuf, GFL_FONT *p_font, GFL_POINT *p_pos )
{ 
  switch( pos_type )
  { 
  case BR_MSGWIN_POS_ABSOLUTE:
    *p_pos  = *cp_ofs;
    break;
  case BR_MSGWIN_POS_WH_CENTER:
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
  }
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  ���X�g�\���\����
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	CLWK�̐�
//=====================================
enum
{
  BR_LIST_CLWK_ALLOW_U,
  BR_LIST_CLWK_ALLOW_D,
  BR_LIST_CLWK_ALLOW_MAX,
  BR_LIST_CLWK_BAR_L  = BR_LIST_CLWK_ALLOW_MAX,
  BR_LIST_CLWK_BAR_R,
  BR_LIST_CLWK_BAR_MAX,
  BR_LIST_CLWK_MAX  = BR_LIST_CLWK_BAR_MAX,
};

//-------------------------------------
///	�X���C�_�[�̓��͊J�n����
//=====================================
#define BR_LIST_SLIDE_DISTANCE  (8)

#define BR_LIST_HITTBL_MAX  (10)

typedef BOOL (*BR_LIST_ENABLE_FUNC)( const BR_LIST_WORK *cp_wk );
typedef BOOL (*BR_LIST_MOVE_FUNC)( BR_LIST_WORK *p_wk, s8 value );
typedef void (*BR_LIST_SELECT_FUNC)( BR_LIST_WORK *p_wk );
//-------------------------------------
///	���X�g
//=====================================
struct _BR_LIST_WORK
{ 
  BR_LIST_PARAM       param;
  GFL_CLWK            *p_clwk[BR_LIST_CLWK_MAX];
  GFL_BMPWIN          *p_bmpwin;

  u16   min;
  u16   max;
  u16   cursor;   //�y�[�W���̂ǂ��ɂ��邩
  u16   list;     //�����ږڂɂ��邩 
  u16   line_max; //1�y�[�W�����C����
  u32   select_param; //�I����������
  s8    value;
  u8    is_allow_visible;
  s8    dumy[2];
  
  BR_LIST_MOVE_FUNC   move_callback;
  BR_LIST_ENABLE_FUNC enable_callback;
  BR_LIST_SELECT_FUNC select_callback;

  GFL_UI_TP_HITTBL    hittbl[BR_LIST_HITTBL_MAX];

  BOOL      is_hit;   //�X���C�_�[
  u16       trg_y;    //�X���C�_�[�g���K���W
  GFL_BMP_DATA        *p_bmp[0];

};
//-------------------------------------
///	�v���g�^�C�v
//=====================================
static void Br_List_Write( BR_LIST_WORK *p_wk, u16 now );
static s8 Br_List_Slide( BR_LIST_WORK* p_wk );
static void BMP_Copy( const GFL_BMP_DATA *cp_src, GFL_BMP_DATA *p_dst, u16 src_x, u16 src_y, u16 dst_x, u16 dst_y, u16 w, u16 h );

static BOOL Br_TouchList_MoveCallback( BR_LIST_WORK *p_wk, s8 value );
static BOOL Br_CursorList_MoveCallback( BR_LIST_WORK *p_wk, s8 value );

static BOOL Br_TouchList_EnableCallback( const BR_LIST_WORK *cp_wk );
static BOOL Br_CursorList_EnableCallback( const BR_LIST_WORK *cp_wk );

static void Br_TouchList_SelectCallback( BR_LIST_WORK *p_wk );
static void Br_CursorList_SelectCallback( BR_LIST_WORK *p_wk );
//-------------------------------------
///	�f�[�^
//=====================================
static const GFL_POINT sc_cursor_pos[]  =
{ 
  { 
    24,76
  },
  { 
    24,100
  },
  { 
    24,124
  },
  { 
    24,148
  },
  { 
    24,172
  },
};

//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������
 *
 *	@param	const BR_LIST_PARAM *cp_param ���X�g����
 *	@param	heapID                        �q�[�v
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
BR_LIST_WORK * BR_LIST_Init( const BR_LIST_PARAM *cp_param, HEAPID heapID )
{ 
  BR_LIST_WORK  *p_wk;

  //���[�N�m�ہA������
  { 
    u32 size;

    size  = sizeof(BR_LIST_WORK) + sizeof(GFL_BMP_DATA*) * cp_param->list_max;
    p_wk  = GFL_HEAP_AllocMemory( heapID, size );
    GFL_STD_MemClear( p_wk, size );
    p_wk->param     = *cp_param;
    p_wk->min       = 0;
    p_wk->max       = cp_param->list_max;
    p_wk->line_max  = cp_param->h / cp_param->str_line;

    p_wk->line_max  = MATH_IMin( p_wk->line_max, cp_param->list_max );

    if( cp_param->p_pos )
    { 
      p_wk->list      = cp_param->p_pos->list;
      p_wk->cursor    = cp_param->p_pos->cursor;
    }
  }

  //�^�C�v�ʐݒ�
  if( p_wk->param.type == BR_LIST_TYPE_TOUCH )
  { 
    int i;
    for( i = 0; i < p_wk->line_max; i++ )
    {
      GF_ASSERT( i < BR_LIST_HITTBL_MAX )
      p_wk->hittbl[ i ].rect.left    = (p_wk->param.x)*8;
      p_wk->hittbl[ i ].rect.top     = (p_wk->param.y + i * p_wk->param.str_line)*8;
      p_wk->hittbl[ i ].rect.right   = (p_wk->param.x + p_wk->param.w)*8;
      p_wk->hittbl[ i ].rect.bottom  = (p_wk->param.y + (i+1) * (p_wk->param.str_line))*8;
    }
    GF_ASSERT( i < BR_LIST_HITTBL_MAX )
    p_wk->hittbl[ i ].rect.left    = GFL_UI_TP_HIT_NONE;
    p_wk->hittbl[ i ].rect.top     = GFL_UI_TP_HIT_NONE;
    p_wk->hittbl[ i ].rect.right   = GFL_UI_TP_HIT_NONE;
    p_wk->hittbl[ i ].rect.bottom  = GFL_UI_TP_HIT_NONE;

    p_wk->move_callback   = Br_TouchList_MoveCallback;
    p_wk->enable_callback = Br_TouchList_EnableCallback;
    p_wk->select_callback = Br_TouchList_SelectCallback;
  }
  else if( p_wk->param.type == BR_LIST_TYPE_CURSOR )
  { 
    p_wk->move_callback   = Br_CursorList_MoveCallback;
    p_wk->enable_callback = Br_CursorList_EnableCallback;
    p_wk->select_callback = Br_CursorList_SelectCallback;

    if( p_wk->param.p_balleff_main )
    { 
      BR_BALLEFF_StartMove( p_wk->param.p_balleff_main, BR_BALLEFF_MOVE_CIRCLE, &sc_cursor_pos[ p_wk->cursor ] );
    }
  }

  //BMPWIN�쐬
  {
    p_wk->p_bmpwin  = GFL_BMPWIN_Create( cp_param->frm, cp_param->x, cp_param->y, cp_param->w, cp_param->h, cp_param->plt, GFL_BMP_CHRAREA_GET_B );
    GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin );
    GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(p_wk->p_bmpwin) );
  }

  //���쐬
  { 
    int i;
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA res;
    CLSYS_DRAW_TYPE type;
    BOOL ret;

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    if( cp_param->frm < GFL_BG_FRAME0_S )
    { 
      BR_RES_LoadOBJ( p_wk->param.p_res, BR_RES_OBJ_ALLOW_M, heapID );
      ret = BR_RES_GetOBJRes( p_wk->param.p_res, BR_RES_OBJ_ALLOW_M, &res );
      GF_ASSERT( ret );
      type  = CLSYS_DRAW_MAIN;
    }
    else
    { 
      BR_RES_LoadOBJ( p_wk->param.p_res, BR_RES_OBJ_ALLOW_S, heapID );
      ret = BR_RES_GetOBJRes( p_wk->param.p_res, BR_RES_OBJ_ALLOW_S, &res );
      GF_ASSERT( ret );
      type  = CLSYS_DRAW_SUB;
    }

    cldata.pos_x    = 128;
    cldata.softpri  = 0;
    for( i = 0; i < BR_LIST_CLWK_ALLOW_MAX; i++ )
    { 
      if( cp_param->type == BR_LIST_TYPE_TOUCH )
      { 
        cldata.pos_y    = i == 0 ? 16: 128;
      }
      else
      { 
        cldata.pos_y    = i == 0 ? 64: 184;
      }
      cldata.anmseq   = i;

      p_wk->p_clwk[i] = GFL_CLACT_WK_Create( cp_param->p_unit, 
				res.ncg, res.ncl, res.nce, 
				&cldata, type, heapID );
      GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[i], TRUE );
      GFL_CLACT_WK_SetObjMode( p_wk->p_clwk[i], GX_OAM_MODE_XLU );
    }
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_U], FALSE );
  }

  //�o�[�쐬
  { 
    int i;
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA res;
    BOOL ret;

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    cldata.pos_y    = 72;
    cldata.anmseq   = 5;
    cldata.softpri  = 0;

    ret = BR_RES_GetOBJRes( p_wk->param.p_res, BR_RES_OBJ_SHORT_BTN_S, &res );
    GF_ASSERT( ret );

    for( i = BR_LIST_CLWK_BAR_L; i < BR_LIST_CLWK_BAR_MAX; i++ )
    { 
      cldata.pos_x    = i == BR_LIST_CLWK_BAR_L? 24: 232;
      p_wk->p_clwk[i]  = GFL_CLACT_WK_Create( p_wk->param.p_unit, 
				res.ncg, res.ncl, res.nce, 
				&cldata, CLSYS_DRAW_SUB, heapID );
      GFL_CLACT_WK_SetObjMode( p_wk->p_clwk[i], GX_OAM_MODE_XLU );
    }
  }

  //������BMP�����Ă���
  { 
    int i;
    GFL_FONT  *p_font;
    const STRBUF *cp_strbuf;

    p_font  = BR_RES_GetFont( p_wk->param.p_res );

    for( i = 0; i < p_wk->param.list_max; i++ )
    { 
      cp_strbuf = p_wk->param.cp_list[i].str;

      if( cp_strbuf != NULL )
      { 
        p_wk->p_bmp[i]  = GFL_BMP_Create( p_wk->param.w, p_wk->param.str_line, GFL_BMP_16_COLOR, heapID );
        GFL_BMP_Clear( p_wk->p_bmp[i], 0 );
        PRINTSYS_PrintColor( p_wk->p_bmp[i], 0, 0, cp_strbuf, p_font, BR_PRINT_COL_NORMAL );
      }
    }
  }

  //�����\�����v�����g
  { 
    Br_List_Write( p_wk, p_wk->list );
  }

  //���X�g���\����菭�Ȃ��Ȃ�΁A��������
  { 
    if(  cp_param->list_max < p_wk->line_max )
    { 
      p_wk->is_allow_visible  =   FALSE;
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_U], FALSE );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_D], FALSE );
    }
    else
    { 
      p_wk->is_allow_visible  =   TRUE;

      //��ԏ�܂ł�����A���̖���
      //��ԉ��܂ł�����A���̖�������
      if( p_wk->is_allow_visible )
      { 
        if( 0 == p_wk->list )
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_U], FALSE );
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_U], TRUE );
        }

        if( p_wk->list + p_wk->line_max == p_wk->param.list_max )
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_D], FALSE );
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_D], TRUE );
        }
      }
    }
  }

  //���X�g�����삷��Ȃ�΁A�o�[���A�j��ON
  { 
    if( BR_LIST_IsMoveEnable( p_wk ) )
    { 
      int i;
      for( i = BR_LIST_CLWK_BAR_L; i < BR_LIST_CLWK_BAR_MAX; i++ )
      { 
        GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[i], TRUE );
      }
    }
  }

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�j��
 *
 *	@param	BR_LIST_WORK* p_wk �j�����郏�[�N
 */
//-----------------------------------------------------------------------------
void BR_LIST_Exit( BR_LIST_WORK* p_wk )
{ 
  int i;

  if( p_wk->param.p_balleff_main )
  { 
    BR_BALLEFF_StartMove( p_wk->param.p_balleff_main, BR_BALLEFF_MOVE_NOP, NULL );
  }

  for( i = 0; i < BR_LIST_CLWK_MAX; i++ )
  { 
    GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
  }

  for( i = 0; i < p_wk->param.list_max; i++ )
  { 
    if( p_wk->p_bmp[i] && p_wk->param.cp_list[i].str )
    { 
      GFL_BMP_Delete( p_wk->p_bmp[i] );
    }
  }
  GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );

  if( p_wk->param.frm < GFL_BG_FRAME0_S )
  { 
    BR_RES_UnLoadOBJ( p_wk->param.p_res, BR_RES_OBJ_ALLOW_M );
  }
  else
  { 
    BR_RES_UnLoadOBJ( p_wk->param.p_res, BR_RES_OBJ_ALLOW_S );
  }


  if( p_wk->param.p_pos )
  { 
    p_wk->param.p_pos->list   = p_wk->list;
    p_wk->param.p_pos->cursor = p_wk->cursor;
  }

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C������
 *
 *	@param	BR_LIST_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_LIST_Main( BR_LIST_WORK* p_wk )
{ 
  //�X�N���[��
  if( BR_LIST_IsMoveEnable( p_wk ) )
  { 
    //�X���C�_�[����
    p_wk->value = Br_List_Slide( p_wk );
    if( p_wk->value != 0 )
    {   
      //��󃂁[�h�Ȃ�΁A���̏ꏊ�ɂ��ĕϓ�
      //�^�b�`���[�h�Ȃ�΃��X�g�����ړ���
      if( p_wk->move_callback( p_wk, p_wk->value ) )
      { 
        if( !PMSND_CheckPlayingSEIdx(BR_SND_SE_SLIDE) )
        {
          PMSND_PlaySE( BR_SND_SE_SLIDE );
        }
        Br_List_Write( p_wk, p_wk->list );
      }

      //��ԏ�܂ł�����A���̖���
      //��ԉ��܂ł�����A���̖�������
      if( p_wk->is_allow_visible )
      { 
        if( 0 == p_wk->list )
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_U], FALSE );
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_U], TRUE );
        }

        if( p_wk->list + p_wk->line_max == p_wk->param.list_max )
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_D], FALSE );
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_D], TRUE );
        }
      }
    }
  }

  //���X�g�I��
  {
    p_wk->select_callback( p_wk );
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�������邩�ǂ���
 *
 *	@param	const BR_LIST_WORK* cp_wk   ���[�N
 *
 *	@return TRUE�Ȃ�΃��X�g����\  FALSE�Ȃ�Εs�\
 */
//-----------------------------------------------------------------------------
BOOL BR_LIST_IsMoveEnable( const BR_LIST_WORK* cp_wk )
{ 
  return cp_wk->enable_callback( cp_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �I�����擾
 *
 *	@param	const BR_LIST_WORK* cp_wk ���X�g
 *
 *	@return BMP_MENULIST_DATA�Őݒ肵���p�����[�^ �����Ă��Ȃ��ꍇ��BR_LIST_SELECT_NONE
 */
//-----------------------------------------------------------------------------
u32 BR_LIST_GetSelect( const BR_LIST_WORK* cp_wk )
{ 
  return cp_wk->select_param;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMP��ݒ�
 *
 *	@param	BR_LIST_WORK* p_wk  ���[�N
 *	@param	idx                 �C���f�b�N�X
 *	@param	*p_src              p_src
 */
//-----------------------------------------------------------------------------
void BR_LIST_SetBmp( BR_LIST_WORK* p_wk, u16 idx, GFL_BMP_DATA *p_src )
{ 
  GF_ASSERT( idx < p_wk->param.list_max );
  GF_ASSERT( p_wk->param.cp_list[idx].str == NULL );
  p_wk->p_bmp[ idx ]  = p_src; 
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMP�擾
 *
 *	@param	BR_LIST_WORK* p_wk  ���[�N
 *	@param	idx                 �C���f�b�N�X
 *
 *	@retval   BMP
 */
//-----------------------------------------------------------------------------
GFL_BMP_DATA *BR_LIST_GetBmp( const BR_LIST_WORK* cp_wk, u16 idx )
{ 
  GF_ASSERT( idx < cp_wk->param.list_max );
  return cp_wk->p_bmp[ idx ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�擾
 *
 *	@param	BR_LIST_WORK* p_wk    ���[�N
 *	@param	param                 �擾�C���f�b�N�X
 *
 *	@return �擾��������
 */
//-----------------------------------------------------------------------------
u32 BR_LIST_GetParam( const BR_LIST_WORK* cp_wk, BR_LIST_PARAM_IDX param )
{ 
  switch( param )
  { 
  case BR_LIST_PARAM_IDX_CURSOR_POS:
    return cp_wk->cursor;

  case BR_LIST_PARAM_IDX_LIST_POS:
    return cp_wk->list;

  case BR_LIST_PARAM_IDX_MOVE_TIMING:
    return cp_wk->value;

  case BR_LIST_PARAM_IDX_LIST_MAX:
    return cp_wk->param.list_max;

  default:
    GF_ASSERT(0);
    return 0;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �\��
 *
 *	@param	BR_LIST_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
extern void BR_LIST_Write( BR_LIST_WORK *p_wk )
{ 
  Br_List_Write( p_wk, p_wk->list );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �\��
 *
 *	@param	BR_LIST_WORK *p_wk ���[�N
 *	@param	now                 ���݂̃��X�g�̈ʒu
 */
//-----------------------------------------------------------------------------
static void Br_List_Write( BR_LIST_WORK *p_wk, u16 now )
{ 
  int i;


  NAGI_Printf( "list %d cursor %d line %d max %d \n", p_wk->list, p_wk->cursor, p_wk->line_max, p_wk->param.list_max );

  //�����\�����v�����g
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );
  for( i = now; i < now + p_wk->line_max; i++ )
  { 
    //GFL_BMP_Print���g���ƕ����������݂ƃX�s�[�h���قڕς��Ȃ��̂ŁA
    //���O�̃L�����P�ʃR�s�[�֐���������������}��
    if( i < p_wk->param.list_max )
    { 
      if( p_wk->p_bmp[i] )
      { 
        BMP_Copy( p_wk->p_bmp[i], GFL_BMPWIN_GetBmp(p_wk->p_bmpwin),
          0, 0, 0, (i - now) * p_wk->param.str_line, p_wk->param.w, p_wk->param.str_line );
      }
    }
  }
  GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );


}

//----------------------------------------------------------------------------
/**
 *	@brief  �X���C�h����
 *
 *	@param	BR_LIST_WORK* p_wk  ���[�N
 *
 *	@return 1�Ȃ�΁��ɂP���� -�Ȃ�΁��ɂP���� 0�Ȃ�Γ���Ȃ�
 */
//-----------------------------------------------------------------------------
static s8 Br_List_Slide( BR_LIST_WORK* p_wk )
{ 
  ///< �X���C�_�[�̓����蔻��
  static const GFL_UI_TP_HITTBL sc_slider_hit_tbl[] = {
    { 8, 128,   0 +  8,   0 + 32 },
    { 8, 128, 256 - 32, 256 -  8 },
    { GFL_UI_TP_HIT_END, GFL_UI_TP_HIT_END, GFL_UI_TP_HIT_END, GFL_UI_TP_HIT_END }
  };

  s8 value  = 0;

  if( GFL_UI_TP_HitCont( sc_slider_hit_tbl ) != GFL_UI_TP_HIT_NONE )
  { 
    u32   x, y;

    GFL_UI_TP_GetPointCont( &x, &y );

    if( !p_wk->is_hit )
    { 
      //�X���C�_�[�g���K�[
      p_wk->is_hit  = TRUE;
      p_wk->trg_y   = y;
    }
    else
    { 
      //�X���C�_�[��
      s32 dif_y = y - p_wk->trg_y;
      if( MATH_IAbs( dif_y ) > BR_LIST_SLIDE_DISTANCE )
      { 
        if( dif_y < 0 )
        { 
          value = -1;
        }
        else
        { 
          value = 1;
        }
        p_wk->trg_y = y;
      }
    }
  }
  else
  { 
    //�X���C�_�[����
    if( p_wk->is_hit )
    {
      p_wk->is_hit  = FALSE;
    }
  }

  return value;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BMP�̃L�����P�ʃR�s�[�����iPRINT�̓h�b�g�P�ʂŃR�s�[����̂ł�葁���j
 *
 *	@param	const GFL_BMP_DATA *cp_src	�]����
 *	@param	*p_dst	�]����
 *	@param	src_x		�]����X���W�i�ȉ����ׂăL�����P�ʁj
 *	@param	src_y		�]����Y���W
 *	@param	dst_x		�]����X���W	
 *	@param	dst_y		�]����Y���W
 *	@param	w				�]����
 *	@param	h				�]������
 *
 */
//-----------------------------------------------------------------------------
static void BMP_Copy( const GFL_BMP_DATA *cp_src, GFL_BMP_DATA *p_dst, u16 src_x, u16 src_y, u16 dst_x, u16 dst_y, u16 w, u16 h )
{	
	const u8	*cp_src_chr		= (const u8 *)GFL_BMP_GetCharacterAdrs( (GFL_BMP_DATA *)(cp_src) );
	u8	*p_dst_chr					= (u8 *)GFL_BMP_GetCharacterAdrs( p_dst );
	const u8 *cp_src_chr_ofs;
	u8 *p_dst_chr_ofs;

	cp_src_chr_ofs	= cp_src_chr + ((w * src_y) + src_x)*GFL_BG_1CHRDATASIZ;
	p_dst_chr_ofs		= p_dst_chr	+ ((w * dst_y) + dst_x)*GFL_BG_1CHRDATASIZ;

	GFL_STD_MemCopy32( cp_src_chr_ofs, p_dst_chr_ofs, w * h * GFL_BG_1CHRDATASIZ );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`�����X�g�̂Ƃ��̈ړ�
 *
 *	@param	BR_LIST_WORK *p_wk   ���[�N
 *	@param	value                 1�Ȃ�΁���-1�Ȃ�΁���
 *
 *	@return TRUE�ŕύX  FALSE�ŕύX���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_TouchList_MoveCallback( BR_LIST_WORK *p_wk, s8 value )
{ 
  if( 0 < p_wk->list && value == -1 )
  { 
    p_wk->list  += value;
    return TRUE;
  }

  if( p_wk->list + p_wk->line_max < p_wk->param.list_max && value == 1 )
  { 
    p_wk->list  += value;
    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�[�\�������X�g�̂Ƃ��̈ړ�
 *
 *	@param	BR_LIST_WORK *p_wk   ���[�N
 *	@param	value                 1�Ȃ�΁���-1�Ȃ�΁���
 *
 *	@return TRUE�ŕύX  FALSE�ŕύX���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_CursorList_MoveCallback( BR_LIST_WORK *p_wk, s8 value )
{ 
  BOOL ret  = FALSE;

  //���X�g�̔����̈ʒu
  const int check_p = (p_wk->line_max-1)/2 + (p_wk->line_max-1)%2;

  if( value > 0 )
  {
    if( check_p == p_wk->cursor && p_wk->list + p_wk->line_max < p_wk->param.list_max )
    {
      //���X�g�������̂̓`�F�b�N�|�C���g�܂ł��Ă��āA
      //������������܂�
      p_wk->list += value;
      ret = TRUE;
    }
    else if( p_wk->list + p_wk->cursor < p_wk->param.list_max-1 )
    {
      //�J�[�\����������̂̓��X�g�͈͓̔��̂�
      p_wk->cursor += value;
      ret = TRUE;
    }
  }
  else if( value < 0 )
  {
    if( check_p == p_wk->cursor && 0 < p_wk->list  )
    {
      //���X�g�������̂̓`�F�b�N�|�C���g�܂ł��Ă��āA
      //�͈͓��̂Ƃ�
      p_wk->list += value;
      ret = TRUE;
    }
    else if( 0 < p_wk->cursor )
    {
      //�J�[�\����������̂̓��X�g�͈͓̔��̂�
      p_wk->cursor += value;
      ret = TRUE;
    }
  }

  //�����Ƃ��J�[�\�����ړ�����
  if( ret )
  { 
    OS_TFPrintf( 3, "cur=%d lis=%d max=%d line=%d v=%d\n", p_wk->cursor, p_wk->list, p_wk->param.list_max, p_wk->line_max,value );

    if( p_wk->param.p_balleff_main )
    { 
      BR_BALLEFF_StartMove( p_wk->param.p_balleff_main, BR_BALLEFF_MOVE_CIRCLE, &sc_cursor_pos[ p_wk->cursor ] );
    }
  }

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`���X�g������\��
 *	        �i�^�b�`���X�g�̓���\���́A���ڐ����E�B���h�E�ȏォ�ǂ����j
 *
 *	@param	const BR_LIST_WORK *cp_wk   ���[�N
 *
 *	@return TRUE�œ���  FALSE�œ��삵�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_TouchList_EnableCallback( const BR_LIST_WORK *cp_wk )
{ 
  return cp_wk->param.list_max >= cp_wk->line_max;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �J�[�\�����X�g������\��
 *	        �i�J�[�\�����X�g�̓���\���́A�P���傫�����j
 *
 *	@param	const BR_LIST_WORK *cp_wk   ���[�N
 *
 *	@return TRUE�œ���  FALSE�œ��삵�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_CursorList_EnableCallback( const BR_LIST_WORK *cp_wk )
{ 
  return cp_wk->param.list_max > 1;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`���X�g�̑I���֐�
 *
 *	@param	BR_LIST_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_TouchList_SelectCallback( BR_LIST_WORK *p_wk )
{ 
  int ret;
  p_wk->select_param  = BR_LIST_SELECT_NONE;

  ret = GFL_UI_TP_HitTrg( p_wk->hittbl );
  if( ret != GFL_UI_TP_HIT_NONE )
  {
    if( p_wk->param.p_balleff_sub )
    { 
      u32 x, y;
      GFL_POINT pos;

      GFL_UI_TP_GetPointTrg( &x, &y );
      pos.x = x;
      pos.y = y;
      BR_BALLEFF_StartMove( p_wk->param.p_balleff_sub, BR_BALLEFF_MOVE_EMIT, &pos );
    }

    PMSND_PlaySE( BR_SND_SE_OK );
    p_wk->select_param  = p_wk->param.cp_list[ ret + p_wk->list ].param;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �J�[�\�����X�g�̑I���֐�
 *
 *	@param	BR_LIST_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_CursorList_SelectCallback( BR_LIST_WORK *p_wk )
{ 
  GF_ASSERT_MSG( p_wk->cursor + p_wk->list < p_wk->param.list_max, "cur=%d lis=%d max=%d\n", p_wk->cursor, p_wk->list, p_wk->param.list_max );
  p_wk->select_param  = p_wk->param.cp_list[ p_wk->cursor + p_wk->list ].param;
}
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  ���ʃe�L�X�g
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�e�L�X�g���[�N
//    ����MSGWIN�Ɠ���
//=====================================
//typedef BR_MSGWIN_WORK  ;

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʃe�L�X�g�\��
 *
 *	@param	PRINT_QUE *p_que  �L���[
 *	@param	heapID            �q�[�vID 
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
BR_TEXT_WORK * BR_TEXT_Init( BR_RES_WORK *p_res, PRINT_QUE *p_que, HEAPID heapID )
{ 
  BR_TEXT_WORK *p_wk;

  //BG�ǂݍ���
  BR_RES_LoadBG( p_res, BR_RES_BG_TEXT_M, heapID );

  //���[�N�쐬
  p_wk  = BR_MSGWIN_Init( BG_FRAME_M_TEXT, 1, 19, 30, 4, PLT_BG_M_COMMON+2, p_que, heapID );
  p_wk->clear_chr = 0xC;
  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 *
 *	@param	BR_TEXT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_TEXT_Exit( BR_TEXT_WORK *p_wk, BR_RES_WORK *p_res )
{ 
  //���[�N�j��
  BR_MSGWIN_Exit( p_wk );

  //BG�j��
  BR_RES_UnLoadBG( p_res, BR_RES_BG_TEXT_M );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����\��
 *
 *	@param	BR_TEXT_WORK* p_wk  ���[�N
 *	@param	*p_res              ���\�[�X
 *	@param	strID               ����ID
 */
//-----------------------------------------------------------------------------
void BR_TEXT_Print( BR_TEXT_WORK* p_wk, const BR_RES_WORK *cp_res, u32 strID )
{ 
  GFL_FONT    *p_font;
  GFL_MSGDATA *p_msg;

  p_font  = BR_RES_GetFont( cp_res );
  p_msg   = BR_RES_GetMsgData( cp_res );
  BR_MSGWIN_PrintColor( p_wk, p_msg, strID, p_font, BR_PRINT_COL_INFO );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����\��  �o�b�t�@
 *
 *	@param	BR_TEXT_WORK* p_wk  ���[�N
 *	@param	*p_res              ���\�[�X
 *	@param	strbuf              �����o�b�t�@
 */
//-----------------------------------------------------------------------------
void BR_TEXT_PrintBuff( BR_TEXT_WORK* p_wk, const BR_RES_WORK *cp_res, const STRBUF *cp_strbuf )
{ 
  GFL_FONT    *p_font;

  p_font  = BR_RES_GetFont( cp_res );
  BR_MSGWIN_PrintBufColor( p_wk, cp_strbuf, p_font, BR_PRINT_COL_INFO );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �\������
 *
 *	@param	BR_TEXT_WORK* p_wk  ���[�N
 *
 *	@return TRUE�ŕ\��  FALSE�ŕ\�����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_TEXT_PrintMain( BR_TEXT_WORK* p_wk )
{ 
  return BR_MSGWIN_PrintMain( p_wk );
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  �v���t�B�[����ʍ쐬
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�v���t�B�[��MSGWIN
//=====================================
typedef enum
{
  BR_PROFILE_MSGWINID_M_NAME = 0,   //�������̃v���t�B�[��
  BR_PROFILE_MSGWINID_M_BIRTH,      //�������܂�
  BR_PROFILE_MSGWINID_M_PLACE,      //����ł���΂���
  BR_PROFILE_MSGWINID_M_COUNTRY,    //�Z��ł���ꏊ����
  BR_PROFILE_MSGWINID_M_AREA,       //�Z��ł���n��
  BR_PROFILE_MSGWINID_M_INTRO,      //�������傤����
  BR_PROFILE_MSGWINID_M_WORD,       //���ȏЉ

  BR_PROFILE_MSGWINID_M_MAX,

} BR_PROFILE_MSGWINID_M;
//-------------------------------------
///	�v���t�B�[�����[�N
//=====================================
struct _BR_PROFILE_WORK
{ 
  BR_MSGWIN_WORK    *p_msgwin[ BR_PROFILE_MSGWINID_M_MAX ];
  BR_RES_WORK       *p_res;
  PMS_DRAW_WORK     *p_pms;


  GFL_CLWK          *p_pokeicon;
  GFL_CLWK          *p_selficon;
  u32               res_icon_plt;
  u32               res_icon_chr;
  u32               res_icon_cel;
  u32               res_self_plt;
  u32               res_self_chr;
  u32               res_self_cel;
};

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �v���t�B�[�����ʂ��쐬
 *
 *	@param	GDS_PROFILE_PTR cp_profile  �\���p�f�[�^
 *	@param	*p_res                      ���\�[�X
 *	@param	*p_unit                     ���j�b�g
 *	@param	*p_que                      �L���[
 *	@param	heapID                      �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
BR_PROFILE_WORK * BR_PROFILE_CreateMainDisplay( const GDS_PROFILE_PTR cp_profile, BR_RES_WORK *p_res, GFL_CLUNIT *p_unit, PRINT_QUE *p_que, HEAPID heapID )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_PROFILE_MSGWINID_M_MAX]  =
  { 
    {
      4,
      4,
      24,
      2,
      msg_13,
    },
    {
      17,
      8,
      11,
      4,
      msg_15,
    },
    {
      4,
      11,
      12,
      2,
      msg_16,
    },
    {
      4,
      13,
      24,
      2,
      msg_22,
    },
    {
      4,
      15,
      24,
      2,
      msg_23,
    },
    {
      4,
      17,
      11,
      2,
      msg_17,
    },
    { 
      4,
      19,
      24,
      4,
      0,
    }
  };


  GFL_FONT *p_font;
  BR_PROFILE_WORK * p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BR_PROFILE_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BR_PROFILE_WORK) );
  p_wk->p_res = p_res;


  p_font  = BR_RES_GetFont( p_res );

  { 
    p_wk->p_pms = PMS_DRAW_Init( p_unit, CLSYS_DRAW_MAIN, p_que, p_font, PLT_OBJ_M_PMS, 1, heapID );
    PMS_DRAW_SetPmsObjMode( p_wk->p_pms, 0, GX_OAM_MODE_XLU );
  }

	BR_RES_LoadBG( p_wk->p_res, BR_RES_BG_RECORD_M_PROFILE, heapID );

 //���b�Z�[�WWIN�쐬
  {
    int i;
    GFL_MSGDATA *p_msg;
    WORDSET *p_word;
    STRBUF  *p_strbuf;
    STRBUF  *p_src;

    BOOL is_msg   = TRUE;

    p_msg   = BR_RES_GetMsgData( p_res );
    p_word  = BR_RES_GetWordSet( p_res );

    for( i = 0; i < BR_PROFILE_MSGWINID_M_MAX; i++ )
    { 
      p_wk->p_msgwin[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_que, heapID );

      BR_MSGWIN_SetPos( p_wk->p_msgwin[i], 0, 0, BR_MSGWIN_POS_ABSOLUTE );

      switch(i)
      { 
      case BR_PROFILE_MSGWINID_M_NAME:   //�������̃v���t�B�[��
        { 
          STRBUF  *p_name;
          p_strbuf  = GFL_STR_CreateBuffer( 128, heapID );
          p_name    = GDS_Profile_CreateNameString( cp_profile, heapID );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterWord( p_word, 0, p_name, GDS_Profile_GetSex(cp_profile), TRUE, PM_LANG );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_name );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;
      case BR_PROFILE_MSGWINID_M_BIRTH:      //�������܂�
        { 
          const u32 month = GDS_Profile_GetMonthBirthday( cp_profile );
          p_strbuf  = GFL_STR_CreateBuffer( 128, heapID );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, month, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;
      case BR_PROFILE_MSGWINID_M_COUNTRY:    //�Z��ł���ꏊ����
        { 
          const u32 nation = GDS_Profile_GetNation( cp_profile );
          p_strbuf  = GFL_STR_CreateBuffer( 128, heapID );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterCountryName( p_word, 0, nation );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;
      case BR_PROFILE_MSGWINID_M_AREA:       //�Z��ł���n��
        { 
          const u32 nation  = GDS_Profile_GetNation( cp_profile );
          const u32 area    = GDS_Profile_GetArea( cp_profile );
          p_strbuf  = GFL_STR_CreateBuffer( 128, heapID );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterLocalPlaceName( p_word, 0, nation, area );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );

          GFL_STR_DeleteBuffer( p_src );
        }
        break;

      case BR_PROFILE_MSGWINID_M_WORD:    //���ȏЉ
        { 
          STRBUF  *p_self;
          PMS_DATA pms;
          p_self    = GDS_Profile_GetSelfIntroduction( cp_profile, &pms, heapID );
          if( p_self )
          { 
            p_strbuf  = GFL_STR_CreateBuffer( 128, heapID );
            p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
            WORDSET_RegisterWord( p_word, 0, p_self, GDS_Profile_GetSex(cp_profile), TRUE, PM_LANG );
            WORDSET_ExpandStr( p_word, p_strbuf, p_src );
            GFL_STR_DeleteBuffer( p_self );
            GFL_STR_DeleteBuffer( p_src );
          }
          else
          { 
            PMS_DRAW_SetNullColorPallet( p_wk->p_pms, 0 );
            PMS_DRAW_SetPrintColor( p_wk->p_pms, BR_PRINT_COL_NORMAL );
            PMS_DRAW_Print( p_wk->p_pms, p_wk->p_msgwin[i]->p_bmpwin, &pms, 0 ); 
            is_msg  = FALSE;
          }
        }
        break;

      default:
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
      }

      if( is_msg )
      { 
        BR_MSGWIN_PrintBufColor( p_wk->p_msgwin[i], p_strbuf, p_font, BR_PRINT_COL_NORMAL );

        GFL_STR_DeleteBuffer( p_strbuf );
      }
    }
  }


  //�|�P����
  { 
    const int egg   = GDS_Profile_GetEggFlag(cp_profile);
    const int mons   = GDS_Profile_GetMonsNo(cp_profile);
    const int form   = GDS_Profile_GetFormNo(cp_profile);
    const int sex   = GDS_Profile_GetPokeSex(cp_profile);

    if( mons != 0 )
    { 
      { 
        ARCHANDLE *p_handle;
        p_handle  = GFL_ARC_OpenDataHandle( ARCID_POKEICON, GFL_HEAP_LOWID(heapID) );
        p_wk->res_icon_plt  = GFL_CLGRP_PLTT_RegisterComp( p_handle, 
            POKEICON_GetPalArcIndex(),
            CLSYS_DRAW_MAIN, PLT_OBJ_M_POKEICON*0x20, GFL_HEAP_LOWID(heapID) );
        p_wk->res_icon_cel = GFL_CLGRP_CELLANIM_Register( p_handle,
            POKEICON_GetCellArcIndex(), POKEICON_GetAnmArcIndex(), GFL_HEAP_LOWID(heapID) );
        p_wk->res_icon_chr  = GFL_CLGRP_CGR_Register( p_handle,
            POKEICON_GetCgxArcIndexByMonsNumber( mons, form, sex, egg ),
            FALSE, CLSYS_DRAW_MAIN, GFL_HEAP_LOWID(heapID) );
        GFL_ARC_CloseDataHandle( p_handle );
      }
      { 
        GFL_CLWK_DATA data;
        GFL_STD_MemClear( &data, sizeof(GFL_CLWK_DATA) );
        data.pos_x  = 75;
        data.pos_y  = 64;
        data.anmseq = POKEICON_ANM_HPMAX;
        p_wk->p_pokeicon  = GFL_CLACT_WK_Create( p_unit,
            p_wk->res_icon_chr,p_wk->res_icon_plt,p_wk->res_icon_cel,
            &data, CLSYS_DEFREND_MAIN, heapID );
        GFL_CLACT_WK_SetPlttOffs( p_wk->p_pokeicon, POKEICON_GetPalNum( mons, form, sex, egg ), CLWK_PLTTOFFS_MODE_OAM_COLOR );
        GFL_CLACT_WK_SetObjMode( p_wk->p_pokeicon, GX_OAM_MODE_XLU );
      }
    }
  }

  //�����݂̂���
  { 
    const int self  = GDS_Profile_GetTrainerView(cp_profile);
    { 
      ARCHANDLE *p_handle;
      p_handle  = GFL_ARC_OpenDataHandle( ARCID_WIFIUNIONCHAR, GFL_HEAP_LOWID(heapID) );
      p_wk->res_self_plt  = GFL_CLGRP_PLTT_RegisterEx( p_handle, 
                          NARC_wifi_unionobj_wifi_union_obj_NCLR,
                          CLSYS_DRAW_MAIN, PLT_OBJ_M_TRAINER*0x20, 
                          sc_wifi_unionobj_plt[self], 1, heapID );
      p_wk->res_self_cel = GFL_CLGRP_CELLANIM_Register( p_handle,
          NARC_wifi_unionobj_front00_NCER, 
          NARC_wifi_unionobj_front00_NANR, heapID );
      p_wk->res_self_chr  = GFL_CLGRP_CGR_Register( p_handle,
          NARC_wifi_unionobj_front00_NCGR + self,
          FALSE, CLSYS_DRAW_MAIN, heapID );
      GFL_ARC_CloseDataHandle( p_handle );
    }
    { 
      GFL_CLWK_DATA data;
      GFL_STD_MemClear( &data, sizeof(GFL_CLWK_DATA) );
      data.pos_x  = 48;
      data.pos_y  = 64;
      p_wk->p_selficon  = GFL_CLACT_WK_Create( p_unit,
          p_wk->res_self_chr,p_wk->res_self_plt,p_wk->res_self_cel,
            &data, CLSYS_DEFREND_MAIN, heapID );
      GFL_CLACT_WK_SetPlttOffs( p_wk->p_selficon, 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );
      GFL_CLACT_WK_SetObjMode( p_wk->p_selficon, GX_OAM_MODE_XLU );
    }
  }


  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �v���t�B�[�����ʂ�j��
 *
 *	@param	BR_PROFILE_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_PROFILE_DeleteMainDisplay( BR_PROFILE_WORK *p_wk )
{ 
  //OBJ�J��
  { 
    if( p_wk->p_selficon )
    { 
      GFL_CLACT_WK_Remove( p_wk->p_selficon );
      GFL_CLGRP_PLTT_Release(p_wk->res_self_plt);
      GFL_CLGRP_CGR_Release(p_wk->res_self_chr);
      GFL_CLGRP_CELLANIM_Release(p_wk->res_self_cel);
    }

    if( p_wk->p_pokeicon )
    { 
      GFL_CLACT_WK_Remove( p_wk->p_pokeicon );
      GFL_CLGRP_PLTT_Release(p_wk->res_icon_plt);
      GFL_CLGRP_CGR_Release(p_wk->res_icon_chr);
      GFL_CLGRP_CELLANIM_Release(p_wk->res_icon_cel);
    }
  }

  //BG�J��
  { 
    int i;
    for( i = 0; i < BR_PROFILE_MSGWINID_M_MAX; i++ )
    { 
      if( p_wk->p_msgwin[i] )
      { 
        BR_MSGWIN_Exit( p_wk->p_msgwin[i] );
        p_wk->p_msgwin[i] = NULL;
      }
    }
  }

  PMS_DRAW_Exit( p_wk->p_pms );

  BR_RES_UnLoadBG( p_wk->p_res, BR_RES_BG_RECORD_M_PROFILE );

  GFL_HEAP_FreeMemory( p_wk );

  GFL_BG_LoadScreenReq( BG_FRAME_M_FONT );
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	BR_PROFILE_WORK *p_wk 
 *
 *	@return TRUE�ŕ\��  FALSE�ł܂�
 */
//-----------------------------------------------------------------------------
BOOL BR_PROFILE_PrintMain( BR_PROFILE_WORK *p_wk )
{ 
  BOOL  ret = TRUE;
  int i;

  for( i = 0; i < BR_PROFILE_MSGWINID_M_MAX; i++ )
  { 
    if( p_wk->p_msgwin[i] )
    { 
      ret &= BR_MSGWIN_PrintMain( p_wk->p_msgwin[i] );
    }
  }

  PMS_DRAW_Main( p_wk->p_pms );

  return ret;
}

//=============================================================================
/**
 *  �V�[�P���X�V�X�e��
 */
//=============================================================================
//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
struct _BR_SEQ_WORK
{
	BR_SEQ_FUNCTION	seq_function;		//���s���̃V�[�P���X�֐�
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
 *	@param	BR_SEQ_WORK *p_wk	���[�N
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
BR_SEQ_WORK * BR_SEQ_Init( void *p_wk_adrs, BR_SEQ_FUNCTION seq_function, HEAPID heapID )
{	
  BR_SEQ_WORK *p_wk;

	//�쐬
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BR_SEQ_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(BR_SEQ_WORK) );

	//������
	p_wk->p_wk_adrs	= p_wk_adrs;

	//�Z�b�g
	BR_SEQ_SetNext( p_wk, seq_function );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	BR_SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void BR_SEQ_Exit( BR_SEQ_WORK *p_wk )
{
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	BR_SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
void BR_SEQ_Main( BR_SEQ_WORK *p_wk )
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
 *	@param	const BR_SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
BOOL BR_SEQ_IsEnd( const BR_SEQ_WORK *cp_wk )
{	
	return cp_wk->seq_function == NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	BR_SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
void BR_SEQ_SetNext( BR_SEQ_WORK *p_wk, BR_SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I��
 *
 *	@param	BR_SEQ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
void BR_SEQ_End( BR_SEQ_WORK *p_wk )
{	
  BR_SEQ_SetNext( p_wk, NULL );
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ ���̃V�[�P���X��\��
 *
 *	@param	BR_SEQ_WORK *p_wk  ���[�N
 *	@param	seq             ���̃V�[�P���X
 */
//-----------------------------------------------------------------------------
void BR_SEQ_SetReservSeq( BR_SEQ_WORK *p_wk, int seq )
{ 
  p_wk->reserv_seq  = seq;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ �\�񂳂ꂽ�V�[�P���X�֔��
 *
 *	@param	BR_SEQ_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_SEQ_NextReservSeq( BR_SEQ_WORK *p_wk )
{ 
  p_wk->seq = p_wk->reserv_seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���݂̃V�[�P���X�Ɠ������ǂ���
 *
 *	@param	const BR_SEQ_WORK *cp_wk  ���[�N
 *	@param	seq_function              �V�[�P���X
 *
 *	@return TRUE���� FALSE�قȂ�
 */
//-----------------------------------------------------------------------------
BOOL BR_SEQ_IsComp( const BR_SEQ_WORK *cp_wk, BR_SEQ_FUNCTION seq_function )
{ 
  return cp_wk->seq_function  == seq_function;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  �������{�[�����o
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	��`
//=====================================
#define BR_BALLEFF_CLWK_MAX (12)

//-------------------------------------
///	����p���[�N
//=====================================
typedef struct 
{
  GFL_POINT center_pos;
  GFL_POINT now_pos;
  u16 r;
  u16 max_r;
  u16 init_angle;
  u16 end_angle;
  int cnt_max;
} MOVE_EMIT;

typedef struct 
{
  GFL_POINT center_pos;
  GFL_POINT now_pos;
  u16 r;
  u16 init_angle;
  int cnt_max;
} MOVE_CIRCLE;

typedef struct 
{
  GFL_POINT start_pos;
  GFL_POINT end_pos;
  GFL_POINT now_pos;

  int wait_cnt;
  int cnt_max;
} MOVE_LINE;

typedef struct 
{
  const GFL_POINT *cp_homing_pos;
  VecFx32 now_pos;
  fx32 speed;
} MOVE_HOMING;

typedef struct
{ 
  GFL_POINT center_pos;
  GFL_POINT now_pos;
  u16 r_x;
  u16 r_y;
  u16 init_angle;
  u16 ellipse_angle;
  int cnt_max;
} MOVE_ELLIPSE;

//-------------------------------------
///	�J�[�\�����[�N
//=====================================
struct _BR_BALLEFF_WORK
{ 
  GFL_POINT   init_pos;
  const GFL_POINT   *cp_folow_pos;
  CLSYS_DRAW_TYPE draw;
  BR_SEQ_WORK *p_seq;
  BR_RES_WORK *p_res;

  union
  { 
    MOVE_LINE     line;
    MOVE_CIRCLE   circle;
    MOVE_HOMING   homing;
    MOVE_EMIT     emit;
    MOVE_ELLIPSE  ellipse;
  }move[BR_BALLEFF_CLWK_MAX];
  GFL_POINT   now_pos[ BR_BALLEFF_CLWK_MAX ];
  GFL_CLWK    *p_clwk[ BR_BALLEFF_CLWK_MAX ];
  int         cnt;
  BOOL        is_end;
  BOOL        is_homing;

  BOOL        is_end_one[ BR_BALLEFF_CLWK_MAX ];
};

//-------------------------------------
///	�v���g�^�C�v
//=====================================
//�V�[�P���X
static void Br_BallEff_Seq_Nop( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BallEff_Seq_Emit( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BallEff_Seq_Line( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BallEff_Seq_Opening( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BallEff_Seq_BigCircle( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BallEff_Seq_Circle( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BallEff_Seq_CircleCont( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BallEff_Seq_EmitFollow( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BallEff_Seq_OpeningTouch( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//����
static void MOVE_EMIT_Init( MOVE_EMIT *p_wk, const GFL_POINT *cp_center_pos, u16 r, u16 max_r, u16 init_angle, u16 end_angle, int cnt_max );
static BOOL MOVE_EMIT_Main( MOVE_EMIT *p_wk, GFL_POINT *p_now_pos, int cnt );
static void MOVE_CIRCLE_Init( MOVE_CIRCLE *p_wk, const GFL_POINT *cp_center_pos, u16 r, u16 init_angle, int cnt_max );
static BOOL MOVE_CIRCLE_Main( MOVE_CIRCLE *p_wk, GFL_POINT *p_now_pos, int cnt );
static void MOVE_LINE_Init( MOVE_LINE *p_wk, const GFL_POINT *cp_start_pos, const GFL_POINT *cp_end_pos, int wait_cnt, int cnt_max );
static BOOL MOVE_LINE_Main( MOVE_LINE *p_wk, GFL_POINT *p_now_pos, int cnt );
//�z�[�~���O
static void MOVE_HOMING_Init( MOVE_HOMING *p_wk, const GFL_POINT *cp_homing_pos,const GFL_POINT *cp_init_pos, fx32 speed );
static BOOL MOVE_HOMING_Main( MOVE_HOMING *p_wk, GFL_POINT *p_now_pos );
//�ȉ~
static void MOVE_ELLIPSE_Init( MOVE_ELLIPSE *p_wk, const GFL_POINT *cp_center_pos, u16 r_x, u16 r_y, u16 init_angle, u16 ellipse_angle, int cnt_max );
static BOOL MOVE_ELLIPSE_Main( MOVE_ELLIPSE *p_wk, GFL_POINT *p_now_pos, int cnt );

//-------------------------------------
///	�f�[�^
//=====================================
//��������̃f�[�^
static const GFL_POINT sc_line_start_pos  =
{ 
  256/2,
  16
};
static const GFL_POINT sc_line_end_pos  =
{ 
  256/2,
  192+24
};
#define BR_BALLEFF_LINE_SYNC  (25*2)

//�I�[�v�j���O�̃f�[�^
static const GFL_POINT sc_opening_start_pos  =
{ 
  256/2,
  -16
};
static const GFL_POINT sc_opening_end_pos  =
{ 
  256/2,
  192/2
};
#define BR_BALLEFF_OPENING_SYNC  (18*2)

//�ǔ��f�[�^
#define BR_BALLEFF_HOMING_SPEED_FIX  (FX32_CONST( 8.0f ) / 2)
#define BR_BALLEFF_HOMING_SPEED(x)  (BR_BALLEFF_HOMING_SPEED_FIX  - (x * FX32_CONST( 0.4f ) / 2))
#define BR_BALLEFF_HOMING_SPEED_OP(x)  (FX32_CONST( 7.0f ) / 2  - (x * FX32_CONST( 0.7f ) / 2))
#define BR_BALLEFF_HOMING_SPEED_MENU(x)  (FX32_CONST( 8.0f ) / 2  - (x * FX32_CONST( 0.6f ) / 2))

//�~������
#define BR_BALLEFF_CIRCLE_R (15)
#define BR_BALLEFF_CIRCLE_SYNC (90)
#define BR_BALLEFF_CIRCLE_USE_MAX (6)

//���ˏ�ɓ���
#define BR_BALLEFF_EMIT_SYNC (15)
#define BR_BALLEFF_EMIT_R_MIN (8)
#define BR_BALLEFF_EMIT_R_MAX (20)
#define BR_BALLEFF_EMIT_ANGLE_MIN(x,max) (0xFFFF*x/max)
#define BR_BALLEFF_EMIT_ANGLE_MAX(x,max) (BR_BALLEFF_EMIT_ANGLE_MIN(x,max)+0xFFFF/(max-1))
#define BR_BALLEFF_EMIT_FOLLOW_SYNC (16)

//-------------------------------------
///	�O�����J
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �{�[���G�t�F�N�g�쐬
 *
 *  @param  GFL_CLUNIT          OAM�o�^���j�b�g
 *	@param	BR_RES_WORK *p_res  ���\�[�X
 *	@param	draw    �`���
 *	@param	heapID  �q�[�vID 
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
BR_BALLEFF_WORK *BR_BALLEFF_Init( GFL_CLUNIT *p_unit, BR_RES_WORK *p_res, CLSYS_DRAW_TYPE draw, HEAPID heapID )
{ 
  BR_BALLEFF_WORK *p_wk = GFL_HEAP_AllocMemory( heapID, sizeof(BR_BALLEFF_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BR_BALLEFF_WORK) );
  p_wk->p_res = p_res;
  p_wk->draw  = draw;

  //�V�[�P���X�V�X�e���쐬
  p_wk->p_seq = BR_SEQ_Init( p_wk, Br_BallEff_Seq_Nop, heapID );

  //���\�[�X�ǂݍ���
  BR_RES_LoadOBJ( p_wk->p_res, BR_RES_OBJ_BALL_M + draw, heapID );

  //CLWK�쐬
  { 
    int i;
    BOOL ret;
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA resdata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    ret = BR_RES_GetOBJRes( p_wk->p_res, BR_RES_OBJ_BALL_M + draw, &resdata );
    GF_ASSERT( ret );
    for( i = 0; i < BR_BALLEFF_CLWK_MAX; i++ )
    { 
      p_wk->p_clwk[i] = GFL_CLACT_WK_Create( 
          p_unit, resdata.ncg, resdata.ncl, resdata.nce, &cldata, draw, heapID
          );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
      GFL_CLACT_WK_SetObjMode( p_wk->p_clwk[i], GX_OAM_MODE_XLU );
    }
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �{�[���G�t�F�N�g�j��
 *
 *	@param	BR_BALLEFF_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_BALLEFF_Exit( BR_BALLEFF_WORK *p_wk )
{ 
  //CLWK�j��
  { 
    int i;
    for( i = 0; i < BR_BALLEFF_CLWK_MAX; i++ )
    { 
      GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
    }
  }

  //���\�[�X�j��
  BR_RES_UnLoadOBJ( p_wk->p_res, BR_RES_OBJ_BALL_M + p_wk->draw );

  //�V�[�P���X�j��
  BR_SEQ_Exit( p_wk->p_seq );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �{�[���G�t�F�N�g���C������
 *
 *	@param	BR_BALLEFF_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_BALLEFF_Main( BR_BALLEFF_WORK *p_wk )
{ 
  BR_SEQ_Main( p_wk->p_seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �{�[���G�t�F�N�g  �G�t�F�N�g����J�n
 *
 *	@param	BR_BALLEFF_WORK *p_wk ���[�N
 *	@param	move                  ����̎�ށi�񋓎Q�Ɓj
 *	@param	GFL_POINT *cp_pos     ����J�n�ʒu or ���čs�����W�̃|�C���^
 */
//-----------------------------------------------------------------------------
void BR_BALLEFF_StartMove( BR_BALLEFF_WORK *p_wk, BR_BALLEFF_MOVE move, const GFL_POINT *cp_pos )
{ 
  if( cp_pos )
  { 
    p_wk->init_pos  = *cp_pos;
    p_wk->cp_folow_pos  = cp_pos;
  }
  p_wk->is_end      = FALSE;

  if( move == BR_BALLEFF_MOVE_NOP && BR_SEQ_IsComp( p_wk->p_seq, Br_BallEff_Seq_BigCircle ) )
  { 
    PMSND_PlaySE( BR_SND_SE_SEARCH_OK );
  }

  OS_TPrintf( "X%d Y%d\n", p_wk->init_pos.x, p_wk->init_pos.y );

  switch( move )
  { 
  case BR_BALLEFF_MOVE_NOP:           //�����Ȃ�(�\������Ȃ�)
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_Nop );
    break;
  case BR_BALLEFF_MOVE_EMIT:          //���˂ɓ���          STOP
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_Emit );
    break;
  case BR_BALLEFF_MOVE_LINE:          //���̓���            STOP
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_Line );
    break;
  case BR_BALLEFF_MOVE_OPENING:       //�J�n�̓���          STOP
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_Opening );
    break;
  case BR_BALLEFF_MOVE_BIG_CIRCLE:    //�傫���~��`��      LOOP
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_BigCircle );
    break;
  case BR_BALLEFF_MOVE_CIRCLE:        //�~��`��            LOOP
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_Circle );
    break;
  case BR_BALLEFF_MOVE_CIRCLE_CONT:   //�ꏊ��ς��ĉ~��`��LOOP
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_CircleCont );
    break;
  case BR_BALLEFF_MOVE_EMIT_FOLLOW:   //���˂ɓ����A���čs�� STOP
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_EmitFollow );
    break;
  case BR_BALLEFF_MOVE_OPENING_TOUCH: //�J�n���̃^�b�`     �@STOP
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_OpeningTouch );
    break;

  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �{�[���G�t�F�N�g  �G�t�F�N�g����I���m�F  �i����ɂ���Ă�LOOP����̂Œ��Ӂj
 *
 *	@param	const BR_BALLEFF_WORK *cp_wk ���[�N
 *
 *	@return TRUE����I��  FALSE���쒆
 */
//-----------------------------------------------------------------------------
BOOL BR_BALLEFF_IsMoveEnd( const BR_BALLEFF_WORK *cp_wk )
{ 
  return cp_wk->is_end;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �{�[���G�t�F�N�g  �A�j���ύX
 *
 *	@param	BR_BALLEFF_WORK *p_wk ���[�N
 *	@param	seq                   �A�j���V�[�P���X
 */
//-----------------------------------------------------------------------------
void BR_BALLEFF_SetAnmSeq( BR_BALLEFF_WORK *p_wk, int seq )
{ 
  int i;
  for( i = 0; i < BR_BALLEFF_CLWK_MAX; i++ )
  { 
    GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[i], seq );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������Ȃ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BallEff_Seq_Nop( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
  };

  BR_BALLEFF_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    { 
      int i;
      for( i = 0; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
      }
    }
    p_wk->is_end  = TRUE;
    *p_seq        = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���ˏ�ɏo��  STOP
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BallEff_Seq_Emit( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_EXIT,
  };

  BR_BALLEFF_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    { 
      int i;
      u16 angle;
      u16 angle_max;


      p_wk->cnt = 0;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        angle = BR_BALLEFF_EMIT_ANGLE_MIN(i,BR_BALLEFF_CIRCLE_USE_MAX);
        angle_max = BR_BALLEFF_EMIT_ANGLE_MAX(i,BR_BALLEFF_CIRCLE_USE_MAX);

        MOVE_EMIT_Init( &p_wk->move[i].emit, &p_wk->init_pos, 
            BR_BALLEFF_EMIT_R_MIN, BR_BALLEFF_EMIT_R_MAX, angle, angle_max, BR_BALLEFF_EMIT_SYNC );

        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }
    }
    *p_seq  = SEQ_MAIN;
    /* fallthr  */

  case SEQ_MAIN:
    { 
      int i;
      GFL_POINT pos;
      GFL_CLACTPOS  clpos;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        MOVE_EMIT_Main( &p_wk->move[i].emit, &pos, p_wk->cnt );
        clpos.x = pos.x;
        clpos.y = pos.y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }

      if( p_wk->cnt++ > BR_BALLEFF_EMIT_SYNC )
      { 
        *p_seq  = SEQ_EXIT;
      }
    }
    break;

  case SEQ_EXIT:
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_Nop );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����ɓ���  STOP
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BallEff_Seq_Line( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_END
  };

  BR_BALLEFF_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    { 
      int i;
      p_wk->cnt   = 0;

      //�������W
      for(i = 0; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        p_wk->now_pos[i]  = sc_line_start_pos;
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }

      //�擪
      MOVE_LINE_Init( &p_wk->move[0].line, &p_wk->now_pos[0], &sc_line_end_pos, 0, BR_BALLEFF_LINE_SYNC );

      //�ǔ�
      for( i = 1; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        MOVE_HOMING_Init( &p_wk->move[i].homing, &p_wk->now_pos[i-1], &p_wk->now_pos[i], BR_BALLEFF_HOMING_SPEED(i) );
      }
    }
    *p_seq  = SEQ_MAIN;
  /* fallthr */

  case SEQ_MAIN:
    { 
      int i;
      GFL_CLACTPOS  clpos;
      BOOL is_end = TRUE;

      //�擪����
      p_wk->is_end  = MOVE_LINE_Main( &p_wk->move[0].line, &p_wk->now_pos[0], p_wk->cnt );
      is_end  &= p_wk->is_end;

      //�ǔ�
      for( i = 1; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        is_end  &= MOVE_HOMING_Main( &p_wk->move[i].homing, &p_wk->now_pos[i] );
      }

      //�ݒ�
      for(i = 0; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        clpos.x = p_wk->now_pos[i].x;
        clpos.y = p_wk->now_pos[i].y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }

      p_wk->cnt++;

      if( is_end )
      { 
        *p_seq  = SEQ_END;
      }
    }
    break;
  case SEQ_END:
    BR_SEQ_SetNext( p_wk->p_seq, Br_BallEff_Seq_Nop );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ŏ��̉��o  STOP
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BallEff_Seq_Opening( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    OPENING_LINE_USE_MAX  = BR_BALLEFF_CIRCLE_USE_MAX,//�����͍~��Ă���
    //�����͉~�����
  };

  enum
  { 
    SEQ_LINE_INIT,
    SEQ_LINE_MAIN,

    SEQ_CIRCLE_INIT,
    SEQ_CIRCLE_MAIN,

    SEQ_END
  };

  BR_BALLEFF_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_LINE_INIT:
    { 
      int i;
      p_wk->cnt   = 0;

      //�������W
      for(i = 0; i < OPENING_LINE_USE_MAX; i++ )
      { 
        p_wk->now_pos[i]  = sc_opening_start_pos;
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }

      //�擪
      MOVE_LINE_Init( &p_wk->move[0].line, &p_wk->now_pos[0], &sc_opening_end_pos, 0, BR_BALLEFF_OPENING_SYNC );

      //�ǔ�
      for( i = 1; i < OPENING_LINE_USE_MAX; i++ )
      { 
        MOVE_HOMING_Init( &p_wk->move[i].homing, &p_wk->now_pos[i-1], &p_wk->now_pos[i], BR_BALLEFF_HOMING_SPEED_OP(i) );
      }
    }
    *p_seq  = SEQ_LINE_MAIN;
  /* fallthr */

  case SEQ_LINE_MAIN:
    { 
      int i;
      GFL_CLACTPOS  clpos;

      //�擪����
      p_wk->is_end_one[0]  = MOVE_LINE_Main( &p_wk->move[0].line, &p_wk->now_pos[0], p_wk->cnt );

      //�ǔ�
      for( i = 1; i < OPENING_LINE_USE_MAX; i++ )
      { 
        p_wk->is_end_one[i] = MOVE_HOMING_Main( &p_wk->move[i].homing, &p_wk->now_pos[i] );
      }

      //�ݒ�
      for(i = 0; i < OPENING_LINE_USE_MAX; i++ )
      { 
        clpos.x = p_wk->now_pos[i].x;
        clpos.y = p_wk->now_pos[i].y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }

      p_wk->cnt++;

      if( p_wk->is_end_one[0] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[0], FALSE );
        *p_seq  = SEQ_CIRCLE_INIT;
      }
    }
    break;

  case SEQ_CIRCLE_INIT:
    {
      int i = 0;
      p_wk->cnt = 0;
      
      //�~��\��
      for( i = OPENING_LINE_USE_MAX; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        MOVE_CIRCLE_Init( &p_wk->move[i].circle, &sc_opening_end_pos, BR_BALLEFF_CIRCLE_R, 0xFFFF*(i-OPENING_LINE_USE_MAX)/(BR_BALLEFF_CLWK_MAX - OPENING_LINE_USE_MAX ), BR_BALLEFF_CIRCLE_SYNC );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }
    }
    *p_seq  = SEQ_CIRCLE_MAIN;
    /* fallthr */

  case SEQ_CIRCLE_MAIN:
    { 
      int i;
      GFL_CLACTPOS  clpos;

      //�~��\��
      for( i = OPENING_LINE_USE_MAX; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        MOVE_CIRCLE_Main( &p_wk->move[i].circle, &p_wk->now_pos[i], p_wk->cnt );
      }

      //�ǔ��������Ă���z�͏I����������
      for( i = 1; i < OPENING_LINE_USE_MAX; i++ )
      { 
        p_wk->is_end_one[i] = MOVE_HOMING_Main( &p_wk->move[i].homing, &p_wk->now_pos[i] );
        if( p_wk->is_end_one[i] )
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
        }
      }

      //�ݒ�
      for(i = 0; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        clpos.x = p_wk->now_pos[i].x;
        clpos.y = p_wk->now_pos[i].y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }

      p_wk->is_end  = TRUE;
      p_wk->cnt++;
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �傫���~��`��  LOOP
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BallEff_Seq_BigCircle( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
  };

  BR_BALLEFF_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    { 
      int i;
      p_wk->cnt = 0;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        MOVE_CIRCLE_Init( &p_wk->move[i].circle, &p_wk->init_pos, BR_BALLEFF_CIRCLE_R, 0xFFFF*i/BR_BALLEFF_CIRCLE_USE_MAX, BR_BALLEFF_CIRCLE_SYNC );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }

      PMSND_PlaySE( BR_SND_SE_SEARCH );
    }
    *p_seq  = SEQ_MAIN;
    /* fallthr  */

  case SEQ_MAIN:
    { 
      int i;
      GFL_POINT pos;
      GFL_CLACTPOS  clpos;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        MOVE_CIRCLE_Main( &p_wk->move[i].circle, &pos, p_wk->cnt );
        clpos.x = pos.x;
        clpos.y = pos.y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }

      p_wk->cnt++;
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �~��`��  LOOP
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BallEff_Seq_Circle( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
  };

  BR_BALLEFF_WORK *p_wk = p_wk_adrs;

#if 0
  switch( *p_seq )
  { 
  case SEQ_INIT:
    { 
      int i;
      int idx;
      p_wk->cnt = 0;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        MOVE_ELLIPSE_Init( &p_wk->move[i].ellipse, &p_wk->init_pos, 10, 5, 0xFFFF*i/BR_BALLEFF_CIRCLE_USE_MAX, 0xFFFF * 45 /360, BR_BALLEFF_CIRCLE_SYNC );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }
      for( i = BR_BALLEFF_CIRCLE_USE_MAX; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        idx = i - BR_BALLEFF_CIRCLE_USE_MAX;
        MOVE_ELLIPSE_Init( &p_wk->move[i].ellipse, &p_wk->init_pos, 10, 5, 0xFFFF*idx/BR_BALLEFF_CIRCLE_USE_MAX, 0xFFFF * (45+90) /360, BR_BALLEFF_CIRCLE_SYNC );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }
    }
    *p_seq  = SEQ_MAIN;
    /* fallthr  */

  case SEQ_MAIN:
    { 
      int i;
      GFL_POINT pos;
      GFL_CLACTPOS  clpos;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        MOVE_ELLIPSE_Main( &p_wk->move[i].ellipse, &pos, p_wk->cnt );
        clpos.x = pos.x;
        clpos.y = pos.y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }

      p_wk->cnt++;
    }
    break;
  }
#else
  switch( *p_seq )
  { 
  case SEQ_INIT:
    { 
      int i;
      p_wk->cnt = 0;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        MOVE_CIRCLE_Init( &p_wk->move[i].circle, &p_wk->init_pos, 10, 0xFFFF*i/BR_BALLEFF_CIRCLE_USE_MAX, BR_BALLEFF_CIRCLE_SYNC );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }
    }
    *p_seq  = SEQ_MAIN;
    /* fallthr  */

  case SEQ_MAIN:
    { 
      int i;
      GFL_POINT pos;
      GFL_CLACTPOS  clpos;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        MOVE_CIRCLE_Main( &p_wk->move[i].circle, &pos, p_wk->cnt );
        clpos.x = pos.x;
        clpos.y = pos.y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }

      p_wk->cnt++;
    }
    break;
  }
#endif
}
//----------------------------------------------------------------------------
/**
 *	@brief  �~��`���i�ړ��j  LOOP
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BallEff_Seq_CircleCont( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 

  BR_BALLEFF_WORK *p_wk = p_wk_adrs;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���˂ɓ��������ƁA���čs��
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BallEff_Seq_EmitFollow( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT_EMIT,
    SEQ_MAIN_EMIT,
    SEQ_EXIT_EMIT,

    SEQ_INIT_FOLLOW,
    SEQ_MAIN_FOLLOW,
    SEQ_EXIT_FOLLOW,
  };

  BR_BALLEFF_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT_EMIT:
    { 
      int i;
      u16 angle;
      u16 angle_max;


      p_wk->cnt = 0;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        angle = BR_BALLEFF_EMIT_ANGLE_MIN(i,BR_BALLEFF_CIRCLE_USE_MAX);
        angle_max = BR_BALLEFF_EMIT_ANGLE_MAX(i,BR_BALLEFF_CIRCLE_USE_MAX);

        MOVE_EMIT_Init( &p_wk->move[i].emit, &p_wk->init_pos, 
            BR_BALLEFF_EMIT_R_MIN, BR_BALLEFF_EMIT_R_MAX, angle, angle_max, BR_BALLEFF_EMIT_FOLLOW_SYNC );

        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }
    }
    *p_seq  = SEQ_MAIN_EMIT;
    /* fallthr  */

  case SEQ_MAIN_EMIT:
    { 
      int i;
      GFL_CLACTPOS  clpos;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        MOVE_EMIT_Main( &p_wk->move[i].emit, &p_wk->now_pos[i], p_wk->cnt );
        clpos.x = p_wk->now_pos[i].x;
        clpos.y = p_wk->now_pos[i].y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }

      if( p_wk->cnt++ > BR_BALLEFF_EMIT_FOLLOW_SYNC )
      { 
        *p_seq  = SEQ_EXIT_EMIT;
      }
    }
    break;

  case SEQ_EXIT_EMIT:
    { 
      *p_seq  = SEQ_INIT_FOLLOW;
    }
    break;

  case SEQ_INIT_FOLLOW:
    { 
      int i;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        MOVE_HOMING_Init( &p_wk->move[i].homing, p_wk->cp_folow_pos, &p_wk->now_pos[i], BR_BALLEFF_HOMING_SPEED_MENU(i) );
      }
    }
    *p_seq  = SEQ_MAIN_FOLLOW;
    /* fallthrough */

  case SEQ_MAIN_FOLLOW:
    { 
      int i;
      BOOL is_end = TRUE;
      GFL_CLACTPOS  clpos;
      for( i = 0; i < BR_BALLEFF_CIRCLE_USE_MAX; i++ )
      { 
        is_end  &= MOVE_HOMING_Main( &p_wk->move[i].homing, &p_wk->now_pos[i] );
        clpos.x = p_wk->now_pos[i].x;
        clpos.y = p_wk->now_pos[i].y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }
      if( is_end )
      { 
        *p_seq  = SEQ_EXIT_FOLLOW;
      }
    }
    break;

  case SEQ_EXIT_FOLLOW:
    BR_SEQ_SetNext( p_seqwk, Br_BallEff_Seq_Nop );
    break;

  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�v�j���O�̃^�b�`�̓���
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BallEff_Seq_OpeningTouch( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_CIRCLE,
    SEQ_WAIT_CIRCLE,
    SEQ_END,
  };
   
  BR_BALLEFF_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_CIRCLE:
    {
      int i;
      GFL_CLACTPOS  clpos;
      u16 now_angle;

      //�{�[���ύX
      BR_BALLEFF_SetAnmSeq( p_wk, 0 );

      //�~��\��  �I�[�v�j���O�ɑ���
      for( i = BR_BALLEFF_CIRCLE_USE_MAX; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        now_angle = p_wk->move[i].circle.init_angle + p_wk->cnt / p_wk->move[i].circle.cnt_max;
        MOVE_EMIT_Init( &p_wk->move[i].emit, &sc_opening_end_pos, BR_BALLEFF_CIRCLE_R, 1, now_angle, now_angle + 0xF000, 30 );
        GFL_CLACT_WK_SetObjMode( p_wk->p_clwk[i], GX_OAM_MODE_NORMAL );
      }
      p_wk->cnt = 0;

      *p_seq  = SEQ_WAIT_CIRCLE;
    }
    /* fallthrough */

  case SEQ_WAIT_CIRCLE:
    { 
      int i;
      GFL_POINT pos;
      GFL_CLACTPOS  clpos;
      for( i = BR_BALLEFF_CIRCLE_USE_MAX; i < BR_BALLEFF_CLWK_MAX; i++ )
      { 
        MOVE_EMIT_Main( &p_wk->move[i].emit, &pos, p_wk->cnt );
        clpos.x = pos.x;
        clpos.y = pos.y;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, p_wk->draw );
      }

      if( p_wk->cnt++ > 60 )
      { 
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, Br_BallEff_Seq_Nop );
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���˂̓���  ������
 *
 *	@param	MOVE_EMIT *p_wk           ���[�N
 *	@param	GFL_POINT *cp_center_pos  ���S���W
 *	@param	r                         ���a
 *	@param  max_r                     �����I�����̔��a
 *	@param	init_angle                �ŏ��̊p�x
 *	@param  end_angle                 �����I�����̊p�x;
 *	@param	cnt_max                   ����
 */
//-----------------------------------------------------------------------------
static void MOVE_EMIT_Init( MOVE_EMIT *p_wk, const GFL_POINT *cp_center_pos, u16 r, u16 max_r, u16 init_angle, u16 end_angle, int cnt_max )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MOVE_EMIT) );
  p_wk->r           = r;
  p_wk->max_r       = max_r;
  p_wk->init_angle  = init_angle;
  p_wk->end_angle   = end_angle;
  p_wk->cnt_max     = cnt_max;
  p_wk->center_pos  = *cp_center_pos;

  MOVE_EMIT_Main( p_wk, &p_wk->now_pos, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���˂̓���  ���C������
 *
 *	@param	MOVE_CIRCLE *p_wk ���[�N
 *	@param	*p_now_pos        ���W
 *	@param	cnt               �V���N
 *
 *	@return TREU�œ����I��  FALSE�œ��쒆
 */
//-----------------------------------------------------------------------------
static BOOL MOVE_EMIT_Main( MOVE_EMIT *p_wk, GFL_POINT *p_now_pos, int cnt )
{ 

  const u16 diff_angle  = p_wk->end_angle - p_wk->init_angle;
  const s16 diff_r      = (s16)p_wk->max_r - (s16)p_wk->r;
  const s8  dir_r       = diff_r / MATH_IAbs(diff_r);

  u16 angle;
  s32 r;

  //���݂̃A���O�����擾
  angle = p_wk->init_angle + (diff_angle * cnt / p_wk->cnt_max);
  r     = p_wk->r + dir_r * ( MATH_IAbs(diff_r) * cnt / p_wk->cnt_max );
  r     = MATH_IMax( r, 0 );

  //NAGI_Printf( "ang%d r%d\n", angle, r );

  ///�p�x������W�����߂�
  p_wk->now_pos.x = (FX_CosIdx( angle ) * r) >> FX32_SHIFT;
  p_wk->now_pos.y = (FX_SinIdx( angle ) * r) >> FX32_SHIFT;

  //���S���W�𑫂�
  p_wk->now_pos.x += p_wk->center_pos.x;
  p_wk->now_pos.y += p_wk->center_pos.y;

  //�󂯎��o�b�t�@�Ɋi�[
  if( p_now_pos )
  { 
    *p_now_pos  = p_wk->now_pos;
  }

  return cnt == p_wk->cnt_max;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �~�^��  ������
 *
 *	@param	MOVE_CIRCLE *p_wk         ���[�N
 *	@param	GFL_POINT *cp_center_pos  ���S���W
 *	@param	r                         ���a
 *	@param	init_angle                �����p�x
 *	@param	cnt_max                   ���삷��V���N
 */
//-----------------------------------------------------------------------------
static void MOVE_CIRCLE_Init( MOVE_CIRCLE *p_wk, const GFL_POINT *cp_center_pos, u16 r, u16 init_angle, int cnt_max )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MOVE_CIRCLE) );
  p_wk->center_pos  = *cp_center_pos;
  p_wk->r           = r;
  p_wk->cnt_max     = cnt_max;
  p_wk->init_angle  = init_angle;

  MOVE_CIRCLE_Main( p_wk, NULL, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �~�^��  ���C������
 *
 *	@param	MOVE_CIRCLE *p_wk ���[�N
 *	@param	*p_now_pos        ���ݍ��W�󂯎��
 *	@param	cnt               �J�E���^
 *
 *	@return TRUE max�V���N�𒴂���  FALSE max�V���N�𒴂��ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL MOVE_CIRCLE_Main( MOVE_CIRCLE *p_wk, GFL_POINT *p_now_pos, int cnt )
{ 
  u16 angle;

  //���݂̃A���O�����擾
  angle = p_wk->init_angle + (0xFFFF * cnt / p_wk->cnt_max);

  ///�p�x������W�����߂�
  p_wk->now_pos.x = (FX_CosIdx( angle ) * p_wk->r) >> FX32_SHIFT;
  p_wk->now_pos.y = (FX_SinIdx( angle ) * p_wk->r) >> FX32_SHIFT;

  //���S���W�𑫂�
  p_wk->now_pos.x += p_wk->center_pos.x;
  p_wk->now_pos.y += p_wk->center_pos.y;


  //�󂯎��o�b�t�@�Ɋi�[
  if( p_now_pos )
  { 
    *p_now_pos  = p_wk->now_pos;
  }

  return cnt == p_wk->cnt_max;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����^��  ������
 *
 *	@param	MOVE_LINE *p_wk           ���[�N
 *	@param	GFL_POINT *cp_start_pos   �J�n�_
 *	@param	GFL_POINT *cp_end_pos     �I���_
 *	@param	wait_cnt                  �ҋ@����
 *	@param	cnt_max                   ���쎞��
 */
//-----------------------------------------------------------------------------
static void MOVE_LINE_Init( MOVE_LINE *p_wk, const GFL_POINT *cp_start_pos, const GFL_POINT *cp_end_pos, int wait_cnt, int cnt_max )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MOVE_LINE) );
  p_wk->start_pos = *cp_start_pos;
  p_wk->end_pos   = *cp_end_pos;
  p_wk->wait_cnt  = wait_cnt;
  p_wk->cnt_max   = cnt_max;

  MOVE_LINE_Main( p_wk, NULL, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����^��  ���C������
 *
 *	@param	MOVE_LINE *p_wk ���[�N
 *	@param	*p_now_pos      ���ݍ��W�̎󂯎��
 *	@param	cnt             �J�E���^
 *
 *	@return TRUE max�V���N�𒴂���  FALSE max�V���N�𒴂��ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL MOVE_LINE_Main( MOVE_LINE *p_wk, GFL_POINT *p_now_pos, int cnt )
{ 
  BOOL ret  = FALSE;
  cnt -= p_wk->wait_cnt;
  cnt = MATH_IMax( cnt, 0 );

  p_wk->now_pos.x = p_wk->start_pos.x + (p_wk->end_pos.x - p_wk->start_pos.x) * cnt / p_wk->cnt_max;
  p_wk->now_pos.y = p_wk->start_pos.y + (p_wk->end_pos.y - p_wk->start_pos.y) * cnt / p_wk->cnt_max;

  if( cnt >= p_wk->cnt_max )
  {
    p_wk->now_pos = p_wk->end_pos;
    ret = TRUE;
  }

  if( p_now_pos )
  { 
    *p_now_pos  = p_wk->now_pos;
  }

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �z�[�~���O  ������
 *
 *	@param	MOVE_HOMING *p_wk         ���[�N
 *	@param	GFL_POINT *cp_homing_pos  �ǔ�������W
 *	@param	speed                     �X�s�[�h
 */
//-----------------------------------------------------------------------------
static void MOVE_HOMING_Init( MOVE_HOMING *p_wk, const GFL_POINT *cp_homing_pos,const GFL_POINT *cp_init_pos, fx32 speed )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MOVE_HOMING) );
  p_wk->cp_homing_pos = cp_homing_pos;
  p_wk->now_pos.x       = FX32_CONST(cp_init_pos->x);
  p_wk->now_pos.y       = FX32_CONST(cp_init_pos->y);
  p_wk->now_pos.z       = 0;
  p_wk->speed           = speed;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �z�[�~���O  ���C������
 *
 *	@param	MOVE_HOMING *p_wk   ���[�N
 *	@param	*p_now_pos          ���W
 *
 *	@return TRUE�Ȃ�Βǔ����W�Ɠ����ꏊ  FALSE�Ȃ�Βǔ���
 */
//-----------------------------------------------------------------------------
static BOOL MOVE_HOMING_Main( MOVE_HOMING *p_wk, GFL_POINT *p_now_pos )
{
  //�����Ƒ���̍���
  VecFx32 diff;
  VecFx32 add;
  fx32  distance;

  diff.x  = FX32_CONST(p_wk->cp_homing_pos->x) - p_wk->now_pos.x;
  diff.y  = FX32_CONST(p_wk->cp_homing_pos->y) - p_wk->now_pos.y;
  diff.z  = 0;

  distance  = VEC_Mag( &diff );

  if( distance != 0 )
  { 
    add.x = FX_Div( FX_Mul( diff.x, p_wk->speed ), distance );
    add.y = FX_Div( FX_Mul( diff.y, p_wk->speed ), distance );

    if( MATH_IAbs(diff.x) > MATH_IAbs(add.x) )
    { 
      p_wk->now_pos.x += add.x;
    }
    else
    { 
      p_wk->now_pos.x = FX32_CONST(p_wk->cp_homing_pos->x);
    }

    if( MATH_IAbs(diff.y) > MATH_IAbs(add.y) )
    { 
      p_wk->now_pos.y += add.y;
    }
    else
    { 
      p_wk->now_pos.y = FX32_CONST(p_wk->cp_homing_pos->y);
    }

    //OS_Printf( "X%.3f Y%.3f S%.3f calc%.3f dist%.3f \n", FX_FX32_TO_F32(diff.x), FX_FX32_TO_F32(diff.y), FX_FX32_TO_F32(distance) , FX_FX32_TO_F32( FX_Div( FX_Mul( diff.y, p_wk->speed ), distance ) ));
  }

  if( p_now_pos )
  { 
    p_now_pos->x  = p_wk->now_pos.x >> FX32_SHIFT;
    p_now_pos->y  = p_wk->now_pos.y >> FX32_SHIFT;
  }

  return distance == 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ȉ~�O��  ������
 *
 *	@param	MOVE_ELLIPSE *p_wk        ���[�N
 *	@param	GFL_POINT *cp_center_pos  ���S���W
 *	@param	r_x                       ���a�iX�����j
 *	@param	r_y                       ���a�iY�����j
 *	@param	init_angle                �����p�x
 *  @param  ellipse_angle             �ȉ~�̊p�x
 *	@param	cnt_max                   ���V���N�łP�����邩
 *
 */
//-----------------------------------------------------------------------------
static void MOVE_ELLIPSE_Init( MOVE_ELLIPSE *p_wk, const GFL_POINT *cp_center_pos, u16 r_x, u16 r_y, u16 init_angle, u16 ellipse_angle, int cnt_max )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MOVE_ELLIPSE) );
  p_wk->center_pos  = *cp_center_pos;
  p_wk->r_x         = r_x;
  p_wk->r_y         = r_y;
  p_wk->init_angle  = init_angle;
  p_wk->ellipse_angle = ellipse_angle;
  p_wk->cnt_max     = cnt_max;

  MOVE_ELLIPSE_Main( p_wk, &p_wk->now_pos, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ȉ~�O��  ���C������
 *
 *	@param	MOVE_ELLIPSE *p_wk  ���[�N
 *	@param	*p_now_pos          ���݂̈ʒu�i�󂯎��j
 *	@param	cnt                 ���݂̃V���N
 */
//-----------------------------------------------------------------------------
static BOOL MOVE_ELLIPSE_Main( MOVE_ELLIPSE *p_wk, GFL_POINT *p_now_pos, int cnt )
{ 
  u16 angle;

  //���݂̃A���O�����擾
  angle = p_wk->init_angle + (0xFFFF * cnt / p_wk->cnt_max);

  ///�p�x������W�����߂�
  p_wk->now_pos.x = (FX_Mul( p_wk->r_x * FX_CosIdx( angle ), FX_CosIdx( p_wk->ellipse_angle )) -
                    FX_Mul( p_wk->r_y * FX_SinIdx( angle ), FX_SinIdx( p_wk->ellipse_angle ) ))
                    >> FX32_SHIFT;

  p_wk->now_pos.y = (FX_Mul( p_wk->r_x * FX_CosIdx( angle ), FX_SinIdx( p_wk->ellipse_angle )) +
                    FX_Mul( p_wk->r_y * FX_SinIdx( angle ), FX_CosIdx( p_wk->ellipse_angle ) ))
                    >> FX32_SHIFT;

  //���S���W�𑫂�
  p_wk->now_pos.x += p_wk->center_pos.x;
  p_wk->now_pos.y += p_wk->center_pos.y;


  //�󂯎��o�b�t�@�Ɋi�[
  if( p_now_pos )
  { 
    *p_now_pos  = p_wk->now_pos;
  }

  return cnt == p_wk->cnt_max;
}
