//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_bbd_color.c
 *	@brief  �r���{�[�h�@�J���[�@�f�[�^
 *	@author	tomoya takahashi
 *	@date		2010.01.05
 *
 *	���W���[�����FFLD_BBD_COLOR
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "field_bbd_color.h"

#include "arc/arc_def.h"
#include "debug_message.naix"
#include "arc/fieldmap/area_map_mmdl_color.naix"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	
//=====================================
typedef struct 
{
  fx32 norm_x;
  fx32 norm_y;
  fx32 norm_z;

  u16 def_r;
  u16 def_g;
  u16 def_b;

  u16 amb_r;
  u16 amb_g;
  u16 amb_b;

  u16 spq_r;
  u16 spq_g;
  u16 spq_b;

  u16 emi_r;
  u16 emi_g;
  u16 emi_b;

} FLD_BBD_COLOER_DATA;


//-------------------------------------
///	�r���{�[�h�J���[�ݒ�V�X�e��
//=====================================
struct _FLD_BBD_COLOR 
{
  ARCHANDLE* p_handle;
  FLD_BBD_COLOER_DATA buff;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�@�Ǘ��V�X�e������
 *
 *	@param	heapID    �q�[�vID
 *
 *	@return �V�X�e��
 */
//-----------------------------------------------------------------------------
FLD_BBD_COLOR* FLD_BBD_COLOR_Create( HEAPID heapID )
{
  FLD_BBD_COLOR* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_BBD_COLOR) );

  p_wk->p_handle = GFL_ARC_OpenDataHandle( ARCID_FLD_BBD_COLOR, heapID );
  
  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�@�Ǘ��V�X�e���j��
 *
 *	@param	p_wk    ���[�N
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_Delete( FLD_BBD_COLOR* p_wk )
{
  GFL_ARC_CloseDataHandle( p_wk->p_handle );
  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�@�Ǘ��V�X�e��  �ǂݍ���
 *
 *	@param	p_wk    ���[�N
 *	@param	idx     �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_Load( FLD_BBD_COLOR* p_wk, u32 idx )
{
  GFL_ARC_LoadDataByHandle( p_wk->p_handle, idx, &p_wk->buff );

  // �ǂݍ��ݏ��̏o��
  TOMOYA_Printf( "Normal x[0x%x] y[0x%x] z[0x%x]\n", p_wk->buff.norm_x, p_wk->buff.norm_y, p_wk->buff.norm_z );
  TOMOYA_Printf( "def r[%d] g[%d] b[%d]\n", p_wk->buff.def_r, p_wk->buff.def_g, p_wk->buff.def_b );
  TOMOYA_Printf( "amb r[%d] g[%d] b[%d]\n", p_wk->buff.amb_r, p_wk->buff.amb_g, p_wk->buff.amb_b );
  TOMOYA_Printf( "spc r[%d] g[%d] b[%d]\n", p_wk->buff.spq_r, p_wk->buff.spq_g, p_wk->buff.spq_b );
  TOMOYA_Printf( "emi r[%d] g[%d] b[%d]\n", p_wk->buff.emi_r, p_wk->buff.emi_g, p_wk->buff.emi_b );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�@�Ǘ��V�X�e��  �f�[�^�ݒ�
 *
 *	@param	cp_wk        ���[�N
 *	@param	p_bbdsys    �ݒ��r���{�[�h�V�X�e��
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_SetData( const FLD_BBD_COLOR* cp_wk, GFL_BBD_SYS* p_bbdsys )
{
  VecFx16 n;
  GXRgb def;
  GXRgb amb;
  GXRgb spq;
  GXRgb emi;
  
  // �e��f�[�^�ݒ�
  {
    // �@��
    n.x = cp_wk->buff.norm_x; 
    n.y = cp_wk->buff.norm_y;
    n.z = cp_wk->buff.norm_z; 
    VEC_Fx16Normalize( &n, &n );
    GFL_BBD_SetUseCustomVecN( p_bbdsys, &n );

    // �f�B�t���[�Y
    def = GX_RGB( cp_wk->buff.def_r, cp_wk->buff.def_g, cp_wk->buff.def_b );
    GFL_BBD_SetDiffuse( p_bbdsys, &def );

    // �A���r�G���g 
    amb = GX_RGB( cp_wk->buff.amb_r, cp_wk->buff.amb_g, cp_wk->buff.amb_b );
    GFL_BBD_SetAmbient( p_bbdsys, &amb );

    // �X�y�L�����[
    spq = GX_RGB( cp_wk->buff.spq_r, cp_wk->buff.spq_g, cp_wk->buff.spq_b );
    GFL_BBD_SetSpecular( p_bbdsys, &spq );

    // �G�~�b�V����
    emi = GX_RGB( cp_wk->buff.emi_r, cp_wk->buff.emi_g, cp_wk->buff.emi_b );
    GFL_BBD_SetEmission( p_bbdsys, &emi );
  }
}



#ifdef DEBUG_BBD_COLOR_CONTROL

#include "message.naix"

#include "print/wordset.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "msg/debug/msg_d_tomoya.h"

#include "font/font.naix"


enum
{
  DEBUG_BBD_COLOR_CONT_SELECT_NORMAL,
  DEBUG_BBD_COLOR_CONT_SELECT_DEF,
  DEBUG_BBD_COLOR_CONT_SELECT_AMB,
  DEBUG_BBD_COLOR_CONT_SELECT_SPQ,
  DEBUG_BBD_COLOR_CONT_SELECT_EMI,

  DEBUG_BBD_COLOR_CONT_SELECT_MAX,
};

//-------------------------------------
///	�f�o�b�N�p�f�[�^
//=====================================
typedef struct 
{
  s16   cont_select;
  u16   print_req;

  VecFx16 normal;
  u8 def_r;
  u8 def_g;
  u8 def_b;
  u8 amb_r;
  u8 amb_g;
  u8 amb_b;
  u8 spq_r;
  u8 spq_g;
  u8 spq_b;
  u8 emi_r;
  u8 emi_g;
  u8 emi_b;

  u16   rotate_xz;
  u16   rotate_y;

  GFL_BBD_SYS* bbd_sys;

  WORDSET*    p_wordset;
  GFL_FONT*   p_font;
  GFL_MSGDATA*  p_msgdata;
  STRBUF*     p_strbuff;
  STRBUF*     p_strbuff_tmp;

} DEBUG_BBD_COLOR_CONTROL_WORK;

static DEBUG_BBD_COLOR_CONTROL_WORK* p_DEBUG_BBD_COLOR_CONTROL_WK = NULL;

// �v���g�^�C�v
static void DEBUG_BBD_COLOR_InitContVec( DEBUG_BBD_COLOR_CONTROL_WORK* p_wk, const VecFx16* cp_vec );
static BOOL DEBUG_BBD_COLOR_ContVec( DEBUG_BBD_COLOR_CONTROL_WORK* p_wk, VecFx16* p_vec );
static BOOL DEBUG_BBD_COLOR_ContRgb( DEBUG_BBD_COLOR_CONTROL_WORK* p_wk, u8* r, u8* g, u8* b );


//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�̒����@�J�n
 *
 *	@param	heapID  �q�[�vID
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_DEBUG_Init( GFL_BBD_SYS* bbd_sys, const FLD_BBD_COLOR* cp_data, HEAPID heapID )
{
  GF_ASSERT( p_DEBUG_BBD_COLOR_CONTROL_WK == NULL );

  // �������m��
  p_DEBUG_BBD_COLOR_CONTROL_WK = GFL_HEAP_AllocClearMemory( heapID, sizeof(DEBUG_BBD_COLOR_CONTROL_WORK) );


  // ���[�h�Z�b�g�쐬
  p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset = WORDSET_Create( heapID );
  p_DEBUG_BBD_COLOR_CONTROL_WK->p_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_d_tomoya_dat, heapID );

  p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff    = GFL_STR_CreateBuffer( 256, heapID );
  p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp  = GFL_STR_CreateBuffer( 256, heapID );

  // �t�H���g�f�[�^
  p_DEBUG_BBD_COLOR_CONTROL_WK->p_font = GFL_FONT_Create(
    ARCID_FONT, NARC_font_large_gftr,
    GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

  p_DEBUG_BBD_COLOR_CONTROL_WK->print_req = TRUE;

  // BBDSYS�ۑ�
  p_DEBUG_BBD_COLOR_CONTROL_WK->bbd_sys = bbd_sys;

  // BBDSYS�̏����擾
  {
    // �@��
    p_DEBUG_BBD_COLOR_CONTROL_WK->normal.x = cp_data->buff.norm_x;
    p_DEBUG_BBD_COLOR_CONTROL_WK->normal.y = cp_data->buff.norm_y;
    p_DEBUG_BBD_COLOR_CONTROL_WK->normal.z = cp_data->buff.norm_z;
    VEC_Fx16Normalize( &p_DEBUG_BBD_COLOR_CONTROL_WK->normal, &p_DEBUG_BBD_COLOR_CONTROL_WK->normal );

    DEBUG_BBD_COLOR_InitContVec( p_DEBUG_BBD_COLOR_CONTROL_WK, &p_DEBUG_BBD_COLOR_CONTROL_WK->normal );


    // �e��F
    p_DEBUG_BBD_COLOR_CONTROL_WK->def_r = cp_data->buff.def_r;
    p_DEBUG_BBD_COLOR_CONTROL_WK->def_g = cp_data->buff.def_g;
    p_DEBUG_BBD_COLOR_CONTROL_WK->def_b = cp_data->buff.def_b;
    p_DEBUG_BBD_COLOR_CONTROL_WK->amb_r = cp_data->buff.amb_r;
    p_DEBUG_BBD_COLOR_CONTROL_WK->amb_g = cp_data->buff.amb_g;
    p_DEBUG_BBD_COLOR_CONTROL_WK->amb_b = cp_data->buff.amb_b;
    p_DEBUG_BBD_COLOR_CONTROL_WK->spq_r = cp_data->buff.spq_r;
    p_DEBUG_BBD_COLOR_CONTROL_WK->spq_g = cp_data->buff.spq_g;
    p_DEBUG_BBD_COLOR_CONTROL_WK->spq_b = cp_data->buff.spq_b;
    p_DEBUG_BBD_COLOR_CONTROL_WK->emi_r = cp_data->buff.emi_r;
    p_DEBUG_BBD_COLOR_CONTROL_WK->emi_g = cp_data->buff.emi_g;
    p_DEBUG_BBD_COLOR_CONTROL_WK->emi_b = cp_data->buff.emi_b;
  }


  GFL_UI_KEY_SetRepeatSpeed( 4,8 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�̒����@�I��
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_DEBUG_Exit( void )
{
  GF_ASSERT( p_DEBUG_BBD_COLOR_CONTROL_WK );
  
  // �t�H���g�f�[�^
  GFL_FONT_Delete( p_DEBUG_BBD_COLOR_CONTROL_WK->p_font );
  p_DEBUG_BBD_COLOR_CONTROL_WK->p_font = NULL;


  GFL_MSG_Delete( p_DEBUG_BBD_COLOR_CONTROL_WK->p_msgdata );
  p_DEBUG_BBD_COLOR_CONTROL_WK->p_msgdata = NULL;

  WORDSET_Delete( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset );
  p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset = NULL;

  GFL_STR_DeleteBuffer( p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff );
  p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff = NULL;
  GFL_STR_DeleteBuffer( p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
  p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp = NULL;

  GFL_UI_KEY_SetRepeatSpeed( 8,15 );

  GFL_HEAP_FreeMemory( p_DEBUG_BBD_COLOR_CONTROL_WK );
  p_DEBUG_BBD_COLOR_CONTROL_WK = NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�̒����@�Ǘ����C��
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_DEBUG_Control( void )
{
  GF_ASSERT( p_DEBUG_BBD_COLOR_CONTROL_WK );

  p_DEBUG_BBD_COLOR_CONTROL_WK->print_req = FALSE;


  // �㉺�ŁA�I�����ڂ�ύX
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ) 
  {
    p_DEBUG_BBD_COLOR_CONTROL_WK->cont_select --;
    if( p_DEBUG_BBD_COLOR_CONTROL_WK->cont_select < 0 ){
      p_DEBUG_BBD_COLOR_CONTROL_WK->cont_select += DEBUG_BBD_COLOR_CONT_SELECT_MAX;
    }
    p_DEBUG_BBD_COLOR_CONTROL_WK->print_req = TRUE;
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
  {
    p_DEBUG_BBD_COLOR_CONTROL_WK->cont_select = (p_DEBUG_BBD_COLOR_CONTROL_WK->cont_select + 1) % DEBUG_BBD_COLOR_CONT_SELECT_MAX;
    p_DEBUG_BBD_COLOR_CONTROL_WK->print_req = TRUE;
  }

  // �I�����ڂ��Ƃ̏���
  switch( p_DEBUG_BBD_COLOR_CONTROL_WK->cont_select )
  {
  case DEBUG_BBD_COLOR_CONT_SELECT_NORMAL:
    if( DEBUG_BBD_COLOR_ContVec( p_DEBUG_BBD_COLOR_CONTROL_WK, &p_DEBUG_BBD_COLOR_CONTROL_WK->normal ) )
    {
      p_DEBUG_BBD_COLOR_CONTROL_WK->print_req = TRUE;
    }
    break;

  case DEBUG_BBD_COLOR_CONT_SELECT_DEF:
    if( DEBUG_BBD_COLOR_ContRgb( p_DEBUG_BBD_COLOR_CONTROL_WK, &p_DEBUG_BBD_COLOR_CONTROL_WK->def_r, &p_DEBUG_BBD_COLOR_CONTROL_WK->def_g, &p_DEBUG_BBD_COLOR_CONTROL_WK->def_b ) )
    {
      p_DEBUG_BBD_COLOR_CONTROL_WK->print_req = TRUE;
    }
    break;

  case DEBUG_BBD_COLOR_CONT_SELECT_AMB:
    if( DEBUG_BBD_COLOR_ContRgb( p_DEBUG_BBD_COLOR_CONTROL_WK, &p_DEBUG_BBD_COLOR_CONTROL_WK->amb_r, &p_DEBUG_BBD_COLOR_CONTROL_WK->amb_g, &p_DEBUG_BBD_COLOR_CONTROL_WK->amb_b ) )
    {
      p_DEBUG_BBD_COLOR_CONTROL_WK->print_req = TRUE;
    }
    break;

  case DEBUG_BBD_COLOR_CONT_SELECT_SPQ:
    if( DEBUG_BBD_COLOR_ContRgb( p_DEBUG_BBD_COLOR_CONTROL_WK, &p_DEBUG_BBD_COLOR_CONTROL_WK->spq_r, &p_DEBUG_BBD_COLOR_CONTROL_WK->spq_g, &p_DEBUG_BBD_COLOR_CONTROL_WK->spq_b ) )
    {
      p_DEBUG_BBD_COLOR_CONTROL_WK->print_req = TRUE;
    }
    break;

  case DEBUG_BBD_COLOR_CONT_SELECT_EMI:
    if( DEBUG_BBD_COLOR_ContRgb( p_DEBUG_BBD_COLOR_CONTROL_WK, &p_DEBUG_BBD_COLOR_CONTROL_WK->emi_r, &p_DEBUG_BBD_COLOR_CONTROL_WK->emi_g, &p_DEBUG_BBD_COLOR_CONTROL_WK->emi_b ) )
    {
      p_DEBUG_BBD_COLOR_CONTROL_WK->print_req = TRUE;
    }
    break;


  default:
    break;
  }

  if( p_DEBUG_BBD_COLOR_CONTROL_WK->print_req )
  {
    GXRgb def;
    GXRgb amb;
    GXRgb spq;
    GXRgb emi;
    
    // �e��f�[�^�ݒ�
    {
      // �@��
      GFL_BBD_SetUseCustomVecN( p_DEBUG_BBD_COLOR_CONTROL_WK->bbd_sys, &p_DEBUG_BBD_COLOR_CONTROL_WK->normal );

      // �f�B�t���[�Y
      def = GX_RGB( p_DEBUG_BBD_COLOR_CONTROL_WK->def_r, p_DEBUG_BBD_COLOR_CONTROL_WK->def_g, p_DEBUG_BBD_COLOR_CONTROL_WK->def_b );
      GFL_BBD_SetDiffuse( p_DEBUG_BBD_COLOR_CONTROL_WK->bbd_sys, &def );

      // �A���r�G���g 
      amb = GX_RGB( p_DEBUG_BBD_COLOR_CONTROL_WK->amb_r, p_DEBUG_BBD_COLOR_CONTROL_WK->amb_g, p_DEBUG_BBD_COLOR_CONTROL_WK->amb_b );
      GFL_BBD_SetAmbient( p_DEBUG_BBD_COLOR_CONTROL_WK->bbd_sys, &amb );

      // �X�y�L�����[
      spq = GX_RGB( p_DEBUG_BBD_COLOR_CONTROL_WK->spq_r, p_DEBUG_BBD_COLOR_CONTROL_WK->spq_g, p_DEBUG_BBD_COLOR_CONTROL_WK->spq_b );
      GFL_BBD_SetSpecular(p_DEBUG_BBD_COLOR_CONTROL_WK->bbd_sys, &spq );

      // �G�~�b�V����
      emi = GX_RGB( p_DEBUG_BBD_COLOR_CONTROL_WK->emi_r, p_DEBUG_BBD_COLOR_CONTROL_WK->emi_g, p_DEBUG_BBD_COLOR_CONTROL_WK->emi_b );
      GFL_BBD_SetEmission( p_DEBUG_BBD_COLOR_CONTROL_WK->bbd_sys, &emi );
    }
  }
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̕`�揈��
 *
 *	@param	p_win 
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_DEBUG_PrintData( GFL_BMPWIN* p_win )
{
  static const u8 sc_CURSOR_Y[ DEBUG_BBD_COLOR_CONT_SELECT_MAX ] = 
  {
    0,
    48,
    80,
    112,
    144,
  };
  GF_ASSERT( p_DEBUG_BBD_COLOR_CONTROL_WK );
  GF_ASSERT( p_win );

  if( p_DEBUG_BBD_COLOR_CONTROL_WK->print_req )
  {
    // �r�b�g�}�b�v�N���A
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_win ), 15 );

    // �@��
    {
      WORDSET_RegisterHexNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 0, p_DEBUG_BBD_COLOR_CONTROL_WK->normal.x, 8, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterHexNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 1, p_DEBUG_BBD_COLOR_CONTROL_WK->normal.y, 8, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterHexNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 2, p_DEBUG_BBD_COLOR_CONTROL_WK->normal.z, 8, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      // �p�x���o��
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 3, p_DEBUG_BBD_COLOR_CONTROL_WK->rotate_y/182, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 4, p_DEBUG_BBD_COLOR_CONTROL_WK->rotate_xz/182, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );

      GFL_MSG_GetString( p_DEBUG_BBD_COLOR_CONTROL_WK->p_msgdata, BBD_COLOR_00, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      WORDSET_ExpandStr( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 16, sc_CURSOR_Y[DEBUG_BBD_COLOR_CONT_SELECT_NORMAL], p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_font );
    }

    // �f�B�t���[�Y
    {
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 0, p_DEBUG_BBD_COLOR_CONTROL_WK->def_r, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 1, p_DEBUG_BBD_COLOR_CONTROL_WK->def_g, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 2, p_DEBUG_BBD_COLOR_CONTROL_WK->def_b, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );

      GFL_MSG_GetString( p_DEBUG_BBD_COLOR_CONTROL_WK->p_msgdata, BBD_COLOR_01, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      WORDSET_ExpandStr( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 16, sc_CURSOR_Y[DEBUG_BBD_COLOR_CONT_SELECT_DEF], p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_font );
    }

    // �A���r�G���g
    {
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 0, p_DEBUG_BBD_COLOR_CONTROL_WK->amb_r, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 1, p_DEBUG_BBD_COLOR_CONTROL_WK->amb_g, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 2, p_DEBUG_BBD_COLOR_CONTROL_WK->amb_b, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );

      GFL_MSG_GetString( p_DEBUG_BBD_COLOR_CONTROL_WK->p_msgdata, BBD_COLOR_02, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      WORDSET_ExpandStr( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 16, sc_CURSOR_Y[DEBUG_BBD_COLOR_CONT_SELECT_AMB], p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_font );
    }

    // �X�y�L�����[
    {
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 0, p_DEBUG_BBD_COLOR_CONTROL_WK->spq_r, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 1, p_DEBUG_BBD_COLOR_CONTROL_WK->spq_g, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 2, p_DEBUG_BBD_COLOR_CONTROL_WK->spq_b, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );

      GFL_MSG_GetString( p_DEBUG_BBD_COLOR_CONTROL_WK->p_msgdata, BBD_COLOR_03, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      WORDSET_ExpandStr( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 16, sc_CURSOR_Y[DEBUG_BBD_COLOR_CONT_SELECT_SPQ], p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_font );
    }

    // �G�~�b�V����
    {
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 0, p_DEBUG_BBD_COLOR_CONTROL_WK->emi_r, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 1, p_DEBUG_BBD_COLOR_CONTROL_WK->emi_g, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
      WORDSET_RegisterNumber( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, 2, p_DEBUG_BBD_COLOR_CONTROL_WK->emi_b, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );

      GFL_MSG_GetString( p_DEBUG_BBD_COLOR_CONTROL_WK->p_msgdata, BBD_COLOR_04, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      WORDSET_ExpandStr( p_DEBUG_BBD_COLOR_CONTROL_WK->p_wordset, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff_tmp );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 16, sc_CURSOR_Y[DEBUG_BBD_COLOR_CONT_SELECT_EMI], p_DEBUG_BBD_COLOR_CONTROL_WK->p_strbuff, p_DEBUG_BBD_COLOR_CONTROL_WK->p_font );
    }


    // �J�[�\���̕`��
    {
      GFL_BMP_Fill( GFL_BMPWIN_GetBmp( p_win ), 1, sc_CURSOR_Y[p_DEBUG_BBD_COLOR_CONTROL_WK->cont_select], 8, 8, 1 );
    }

    GFL_BMPWIN_TransVramCharacter( p_win );
  }

}



//----------------------------------------------------------------------------
/**
 *	@brief  �x�N�g���Ǘ��̏�����
 *
 *	@param	p_wk      ���[�N
 *	@param	cp_vec    ����x�N�g��
 */
//-----------------------------------------------------------------------------
static void DEBUG_BBD_COLOR_InitContVec( DEBUG_BBD_COLOR_CONTROL_WORK* p_wk, const VecFx16* cp_vec )
{
  fx32 xz_dist;

  p_wk->rotate_xz = FX_Atan2Idx( cp_vec->x, cp_vec->z );
  xz_dist   = FX_Sqrt( FX_Mul( cp_vec->z, cp_vec->z ) + FX_Mul( cp_vec->x, cp_vec->x ) );
  p_wk->rotate_y  = FX_Atan2Idx( cp_vec->y, xz_dist );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �x�N�g������@���C��
 */
//-----------------------------------------------------------------------------
#define DEBUG_BBD_COLOR_CONT_VEC_ROTATE_ADD ( 364 )
static BOOL DEBUG_BBD_COLOR_ContVec( DEBUG_BBD_COLOR_CONTROL_WORK* p_wk, VecFx16* p_vec )
{
  BOOL change = FALSE;
  MtxFx33 mtx_xz, mtx_y;

  // �㉺���E��]
  // X ��
  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_X ){
    // Y�����́A+�X�O�x�`-90�x�܂ł����������Ȃ�
    if( ((p_wk->rotate_y+DEBUG_BBD_COLOR_CONT_VEC_ROTATE_ADD) <= ((0xffff/4))) ||
        ((p_wk->rotate_y+DEBUG_BBD_COLOR_CONT_VEC_ROTATE_ADD) > ((0xffff/4)*3)) ){
      p_wk->rotate_y += DEBUG_BBD_COLOR_CONT_VEC_ROTATE_ADD;
    }else{
      p_wk->rotate_y = (0xffff/4);
    }

    change = TRUE;
  }
  // B ��
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_B ){

    // Y�����́A+�X�O�x�`-90�x�܂ł����������Ȃ�
    if( (p_wk->rotate_y <= ((0xffff/4))) || (p_wk->rotate_y >= (((0xffff/4)*3) + DEBUG_BBD_COLOR_CONT_VEC_ROTATE_ADD)) ){
      p_wk->rotate_y -= DEBUG_BBD_COLOR_CONT_VEC_ROTATE_ADD;
    }else{
      p_wk->rotate_y = ((0xffff/4)*3);
    }
    change = TRUE;
  }
  // Y ��
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_Y ){

    p_wk->rotate_xz -= DEBUG_BBD_COLOR_CONT_VEC_ROTATE_ADD;
    change = TRUE;
  }
  // A �E
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A ){

    p_wk->rotate_xz += DEBUG_BBD_COLOR_CONT_VEC_ROTATE_ADD;
    change = TRUE;
  }

  if( change ){

    p_vec->y = FX_Mul( FX_SinIdx( p_wk->rotate_y ), FX32_ONE );
    p_vec->x = FX_Mul( FX_CosIdx( p_wk->rotate_y ), FX32_ONE );
    p_vec->z = FX_Mul( FX_CosIdx( p_wk->rotate_xz ), p_vec->x );
    p_vec->x = FX_Mul( FX_SinIdx( p_wk->rotate_xz ), p_vec->x );

    VEC_Fx16Normalize( p_vec, p_vec );
  }

  return change;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �F�̊Ǘ�
 */
//-----------------------------------------------------------------------------
static BOOL DEBUG_BBD_COLOR_ContRgb( DEBUG_BBD_COLOR_CONTROL_WORK* p_wk, u8* r, u8* g, u8* b )
{
  BOOL change = FALSE;

  // R
  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_Y ){
    (*r) = ((*r)+1) % 32;
    change = TRUE;
  }
  // G
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_X ){
    (*g) = ((*g)+1) % 32;
    change = TRUE;
  }
  // B
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A ){
    (*b) = ((*b)+1) % 32;
    change = TRUE;
  }
  return change;
}


#endif
