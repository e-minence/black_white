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

//�����̃��W���[��
#include "br_util.h"
#include "br_inner.h"

//�A�[�J�C�u
#include "msg/msg_battle_rec.h"
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
 *	@param	BR_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_strbuf         STRBUF  �����j�����Ă����܂��܂���
 *	@param	*p_font           �t�H���g
 */
//-----------------------------------------------------------------------------
void BR_MSGWIN_PrintBuf( BR_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font )
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
 *	@param	BR_MSGWIN_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param	*p_font           �t�H���g
 *	@param  lsb               �F�w��
 */
//-----------------------------------------------------------------------------
void BR_MSGWIN_PrintColor( BR_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font, PRINTSYS_LSB lsb )
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
 *	@param	BR_MSGWIN_WORK* p_wk   ���[�N
 *
 *	@return TRUE�Ȃ�Ώ�������  FALSE�Ȃ�΂܂�
 */
//-----------------------------------------------------------------------------
BOOL BR_MSGWIN_PrintMain( BR_MSGWIN_WORK* p_wk )
{ 
	return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
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

typedef BOOL (*BR_LIST_MOVE_FUNC)( BR_LIST_WORK *p_wk, s8 value );
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
  s8    dumy[3];
  
  BR_LIST_MOVE_FUNC move_callback;
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
    p_wk->param = *cp_param;
    p_wk->min       = 0;
    p_wk->max       = cp_param->list_max;
    p_wk->line_max  = cp_param->h / cp_param->str_line;
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
      p_wk->hittbl[ i ].rect.bottom  = (p_wk->param.y + i * (p_wk->param.str_line+1))*8;
    }
    GF_ASSERT( i < BR_LIST_HITTBL_MAX )
    p_wk->hittbl[ i ].rect.left    = GFL_UI_TP_HIT_NONE;
    p_wk->hittbl[ i ].rect.top     = GFL_UI_TP_HIT_NONE;
    p_wk->hittbl[ i ].rect.right   = GFL_UI_TP_HIT_NONE;
    p_wk->hittbl[ i ].rect.bottom  = GFL_UI_TP_HIT_NONE;

    p_wk->move_callback = Br_TouchList_MoveCallback;
  }
  else if( p_wk->param.type == BR_LIST_TYPE_CURSOR )
  { 
    p_wk->move_callback = Br_CursorList_MoveCallback;
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

  //���X�g�����삵�Ȃ�������A��������
  { 
    if( !BR_LIST_IsMoveEnable( p_wk ) )
    { 
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[BR_LIST_CLWK_ALLOW_D], FALSE );
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
        Br_List_Write( p_wk, p_wk->list );
      }

      //��ԏ�܂ł�����A���̖���
      //��ԉ��܂ł�����A���̖�������
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

  //���X�g�I��
  { 
    p_wk->select_param  = BR_LIST_SELECT_NONE;
    if( p_wk->param.type == BR_LIST_TYPE_TOUCH )
    {
      int ret;
      ret = GFL_UI_TP_HitTrg( p_wk->hittbl );
      if( ret != GFL_UI_TP_HIT_NONE )
      { 
        p_wk->select_param  = p_wk->param.cp_list[ ret + p_wk->list ].param;
      }
    }
    else if( p_wk->param.type == BR_LIST_TYPE_CURSOR )
    { 
      GF_ASSERT( p_wk->cursor + p_wk->list < p_wk->param.list_max  );
      p_wk->select_param  = p_wk->param.cp_list[ p_wk->cursor + p_wk->list ].param;
    }
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
  return cp_wk->line_max < cp_wk->param.list_max;
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
  if( p_wk->list == 0  )
  {
    //���X�g�㕔�̏ꍇ�J�[�\���������ɓ����܂Ń��X�g�͓����Ȃ�
    if(p_wk->cursor == p_wk->line_max/2 && value == 1 )
    {
      p_wk->list  += 1;
      return TRUE;
    }
    else
    { 
      if( 0 <= p_wk->cursor + value && p_wk->cursor + value <= p_wk->line_max/2 )
      { 
        p_wk->cursor  += value;
        return TRUE;
      }
    }
  }
  else if( p_wk->list + p_wk->line_max ==  p_wk->param.list_max )
  { 
    //���X�g�����̏ꍇ
    if( p_wk->cursor == p_wk->line_max/2 && value == -1 )
    { 
      p_wk->list  += -1;
      return TRUE;
    }
    else
    {
      if( p_wk->line_max/2 <= p_wk->cursor + value && p_wk->cursor + value < p_wk->line_max )
      { 
        p_wk->cursor  += value;
        return TRUE;
      }
    }
  }
  else
  { 
    //���̑��̓��X�g������
    p_wk->list  += value;
    return TRUE;
  }

  return FALSE;
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

  BR_PROFILE_MSGWINID_M_MAX,

} BR_PROFILE_MSGWINID_M;
//-------------------------------------
///	�v���t�B�[�����[�N
//=====================================
struct _BR_PROFILE_WORK
{ 
  BR_MSGWIN_WORK    *p_msgwin[ BR_PROFILE_MSGWINID_M_MAX ];
  BR_RES_WORK       *p_res;
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
      11,
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
  };


  BR_PROFILE_WORK * p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BR_PROFILE_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BR_PROFILE_WORK) );
  p_wk->p_res = p_res;

 
	BR_RES_LoadBG( p_wk->p_res, BR_RES_BG_RECORD_M_PROFILE, heapID );

 //���b�Z�[�WWIN�쐬
  {
    int i;
    GFL_FONT *p_font;
    GFL_MSGDATA *p_msg;

    p_font  = BR_RES_GetFont( p_res );
    p_msg   = BR_RES_GetMsgData( p_res );

    for( i = 0; i < BR_PROFILE_MSGWINID_M_MAX; i++ )
    { 
      p_wk->p_msgwin[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_que, heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin[i], p_msg, sc_msgwin_data[i].msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }


  //@todo �����̌����ځ{�|�P����
  { 

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


  BR_RES_UnLoadBG( p_wk->p_res, BR_RES_BG_RECORD_M_PROFILE );

  GFL_HEAP_FreeMemory( p_wk );
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

  return ret;
}
