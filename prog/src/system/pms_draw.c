//=============================================================================
/**
 *
 *	@file		pms_draw.c
 *	@brief  �ȈՉ�b�\���V�X�e���i�ȈՉ�b+�f�R���Ǘ��j
 *	@author	hosaka genya
 *	@data		2009.10.14
 *
 */
//=============================================================================
#include <gflib.h>
#include "print/printsys.h"

#include "system/pms_draw.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "pmsi.naix"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	�v�f���Ƃ̊Ǘ��f�[�^
//==============================================================
typedef struct {
  PRINT_UTIL    print_util;
  GFL_CLWK*     clwk[PMS_WORD_MAX];
  int           pre_scrcnt_x; ///< �O�t���[����BG�X�N���[���J�E���^
  int           pre_scrcnt_y; ///< �O�t���[����BG�X�N���[���J�E���^
  u32           b_useflag:1;
  u32           b_visible:1;
  BOOL          b_clwk_deco[PMS_WORD_MAX]; ///< CLWK�f�R������t���O
} PMS_DRAW_UNIT;

//--------------------------------------------------------------
///	OBJ���\�[�X�Ǘ����[�N
//==============================================================
typedef struct {
  GFL_CLUNIT* clwk_unit;
  CLSYS_DRAW_TYPE vram_type;
  u32  obj_ncl;
  u32  obj_ncg;
  u32  obj_nce;
} PMS_DRAW_OBJ;


//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
struct _PMS_DRAW_WORK {
  // [IN]
  HEAPID      heap_id;
  GFL_FONT*   font;
  PRINT_QUE*  print_que;
  // [PRIVATE]
  PMS_DRAW_UNIT*  unit;
  PMS_DRAW_OBJ wk_obj;
  PRINTSYS_LSB print_color;
  BOOL    b_print_end;  ///< �v�����g�I���t���O
  BOOL    is_clwk_auto_scroll;  ///< CLWK �I�[�g�X�N���[���t���O
  u8      unit_num;
  u8      null_color;   ///< �󗓂�h��Ԃ��p���b�g�ԍ�
  u8      padding[2];
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static void _get_write_pos( GFL_BMPWIN* win, u8 width, u8 line, GFL_POINT* offset, GFL_POINT* out_pos );
static u32 _obj_get_ncer( CLSYS_DRAW_TYPE vram_type );
static u32 _obj_get_nanr( CLSYS_DRAW_TYPE vram_type );
static void _obj_loadres( PMS_DRAW_OBJ* obj, u8 pltt_ofs, HEAPID heap_id );
static void _obj_unloadres( PMS_DRAW_OBJ* obj );
static GFL_CLWK* _obj_create( PMS_DRAW_OBJ* obj, HEAPID heap_id );
static void _obj_set_deco( GFL_CLWK* act, GFL_BMPWIN* win, u8 width, u8 line, PMS_DECO_ID deco_id, GFL_POINT* offset );
static void _unit_init( PMS_DRAW_UNIT* unit, PMS_DRAW_OBJ* obj, HEAPID heap_id );
static void _unit_exit( PMS_DRAW_UNIT* unit );
static BOOL _unit_main( PMS_DRAW_UNIT* unit, PRINT_QUE* que, BOOL is_clwk_auto_scroll );
static void _unit_print( PMS_DRAW_UNIT* unit, PRINT_QUE* print_que, GFL_FONT* font, GFL_BMPWIN* win, PMS_DATA* pms, GFL_POINT* offset, PRINTSYS_LSB print_color, u8 null_color, HEAPID heap_id );
static void _unit_clear( PMS_DRAW_UNIT* unit, BOOL is_trans );
static void _unit_visible_set( PMS_DRAW_UNIT* unit, BOOL is_visible );
static CLSYS_DRAW_TYPE BGFrameToVramType( u8 frame );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\���V�X�e�� ������
 *
 *	@param	GFL_CLUNIT* clunit  �A�N�^�[���j�b�g
 *	@param  CLSYS_DRAW_TYPE vram_type �㉺��ʃt���O
 *	@param	que   �v�����g�L���[
 *	@param  font  �t�H���g
 *	@param	obj_pltt_ofs �p���b�g�I�t�Z�b�g
 *	@param	id_max  �Ǘ�����\�����j�b�g��
 *	@param	heap_id �q�[�vID
 *
 *	@retval ���[�N
 */
//-----------------------------------------------------------------------------
PMS_DRAW_WORK* PMS_DRAW_Init( GFL_CLUNIT* clunit, CLSYS_DRAW_TYPE vram_type, PRINT_QUE* que, GFL_FONT* font, u8 obj_pltt_ofs, u8 id_max, HEAPID heap_id )
{
  int i;
  PMS_DRAW_WORK* wk;

  // ����ʂ̓T�|�[�g�ΏۊO
  GF_ASSERT( vram_type != CLSYS_DRAW_MAX );

  // �q�[�v����
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(PMS_DRAW_WORK) );
  GFL_STD_MemClear( wk, sizeof(PMS_DRAW_WORK) );

  wk->heap_id     = heap_id;
  wk->print_que   = que;
  wk->font        = font;
  wk->unit_num    = id_max;
  wk->null_color  = PMS_DRAW_DEF_NULL_COLOR;  
  wk->is_clwk_auto_scroll = FALSE; ///< �f�t�H���g�͖���

  // �\�����j�b�g�������m��
  wk->unit = GFL_HEAP_AllocClearMemory( heap_id, sizeof(PMS_DRAW_UNIT) *id_max );
  GFL_STD_MemClear( wk->unit, sizeof(PMS_DRAW_UNIT) * id_max );

  // OBJ���\�[�X�Ǘ����[�N������
  wk->wk_obj.clwk_unit = clunit;
  wk->wk_obj.vram_type = vram_type;

  wk->print_color = PRINTSYS_LSB_Make( 0x1, 0x2, 0x0 );

  // OBJ���\�[�X���[�h
  _obj_loadres( &wk->wk_obj, obj_pltt_ofs, wk->heap_id );

  // �\�����j�b�g������
  for( i=0; i<wk->unit_num; i++ )
  {
    _unit_init( &wk->unit[i], &wk->wk_obj, wk->heap_id );
  }

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\���V�X�e�� �又��
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Main( PMS_DRAW_WORK* wk )
{
  int i;

  // �����Ƃ��Ȃ���Γ]���I��
  wk->b_print_end = TRUE;
  
  for( i=0; i<wk->unit_num; i++ )
  {
    if( _unit_main( &wk->unit[i], wk->print_que, wk->is_clwk_auto_scroll ) )
    {
      // ��ł��]�����I����ĂȂ����̂���������]���I���t���O��OFF
      wk->b_print_end = FALSE;
    }
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\���V�X�e�� �J������
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Exit( PMS_DRAW_WORK* wk )
{
  int i;

  // �\�����j�b�g���[�N�I������
  for( i=0; i<wk->unit_num; i++ )
  {
    _unit_exit( &wk->unit[i] );
  }
    
  // �\�����j�b�g���[�N���ꊇ�t���[
  GFL_HEAP_FreeMemory( wk->unit );

  _obj_unloadres( &wk->wk_obj );

  // �q�[�v�J��
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�ɊȈՉ�b��\��
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *	@param	win �\������BMPWIN
 *	@param	pms �\������ȈՉ�b�f�[�^
 *	@param	id �\�����j�b�g�Ǘ�ID
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Print( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, PMS_DATA* pms, u8 id )
{ 
  GFL_POINT offset = {0};
  PMS_DRAW_PrintOffset( wk, win, pms, id, &offset );
}
//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�ɊȈՉ�b��\��(�\���I�t�Z�b�g�w���)
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *	@param	win �\������BMPWIN
 *	@param	pms �\������ȈՉ�b�f�[�^
 *	@param	id �\�����j�b�g�Ǘ�ID
 *	@param  offset �\���I�t�Z�b�g�i�h�b�g�P�ʁj
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_PrintOffset( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, PMS_DATA* pms, u8 id, GFL_POINT* offset )
{ 
  PMS_DRAW_UNIT* unit;

  GF_ASSERT( wk && win && pms );
  GF_ASSERT( id < wk->unit_num );

//  HOSAKA_Printf("*** print id=%d ***\n", id);

  _unit_print( &wk->unit[id], wk->print_que, wk->font, win, pms, offset, wk->print_color, wk->null_color, wk->heap_id );
    
  // Main��ʂ�܂ł͓]������Ȃ�
  wk->b_print_end = FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�̕\���I���`�F�b�N
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *
 *	@retval	TRUE:�v�����g�I��
 */
//-----------------------------------------------------------------------------
BOOL PMS_DRAW_IsPrintEnd( PMS_DRAW_WORK* wk )
{
  return wk->b_print_end;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�̕\���N���A
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *	@param	id �\�����j�b�g�Ǘ�ID
 *	@param	is_trans TRUE�F�X�N���[���A�L������]������
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Clear( PMS_DRAW_WORK* wk, u8 id, BOOL is_trans )
{ 
  PMS_DRAW_UNIT* unit;

  GF_ASSERT( wk );
  GF_ASSERT( id < wk->unit_num );
  
  _unit_clear( &wk->unit[id], is_trans );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�̕\��|��\���ؑ�
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *	@param	id �\�����j�b�g�Ǘ�ID
 *	@param	is_visible TRUE�F�\��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_VisibleSet( PMS_DRAW_WORK* wk, u8 id, BOOL is_visible )
{ 
  PMS_DRAW_UNIT* unit;

  GF_ASSERT( wk );
  GF_ASSERT( id < wk->unit_num );
  
  _unit_visible_set( &wk->unit[id], is_visible );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��\�����j�b�g���\�������ǂ�����Ԃ�
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *	@param	id �\�����j�b�g�Ǘ�ID
 *
 *	@retval TRUE:�\���� FALSE:�N���A����Ă���
 */
//-----------------------------------------------------------------------------
BOOL PMS_DRAW_IsPrinting( PMS_DRAW_WORK* wk, u8 id )
{
  GF_ASSERT( wk );
  GF_ASSERT( id < wk->unit_num );

  return wk->unit[id].b_useflag;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �\�����j�b�g���㏑��(�o���v�����g�ς݂̂��̂Ɍ���)
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *	@param	id_src ����ւ���
 *	@param	id_dst ����ւ���
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Copy( PMS_DRAW_WORK* wk, u8 id_src, u8 id_dst )
{
  int i;
  PMS_DRAW_UNIT* src;
  PMS_DRAW_UNIT* dst;
  GFL_POINT pos_ofs;
  
  GF_ASSERT( wk );
  GF_ASSERT( id_src < wk->unit_num );
  GF_ASSERT( id_dst < wk->unit_num );

  src = &wk->unit[id_src];
  dst = &wk->unit[id_dst];

  // OBJ�ǐ��̂��߁ABMPWIN�̈ړ��ʂ𑪒肵�Ă���
  pos_ofs.x = GFL_BMPWIN_GetPosX( dst->print_util.win ) - GFL_BMPWIN_GetPosX( src->print_util.win );
  pos_ofs.y = GFL_BMPWIN_GetPosY( dst->print_util.win ) - GFL_BMPWIN_GetPosY( src->print_util.win );

  // BMPWIN�̕\�����㏑���]��
  GFL_BMP_Copy( GFL_BMPWIN_GetBmp(src->print_util.win), GFL_BMPWIN_GetBmp(dst->print_util.win) );
  GFL_BMPWIN_MakeTransWindow_VBlank( dst->print_util.win );

  // OBJ�㏑��
  for( i=0; i<PMS_WORD_MAX; i++ )
  {
    u8 srcf;
    CLSYS_DRAW_TYPE draw_type;
    GFL_CLACTPOS pos;

    srcf = GFL_BMPWIN_GetFrame( src->print_util.win );
    draw_type = BGFrameToVramType( srcf );

    // ����OBJ���W���擾���A�ړ��ʂ𑫂�����
    GFL_CLACT_WK_GetPos( src->clwk[i], &pos, draw_type );
//  HOSAKA_Printf("newpos=%d %d diff=%d %d \n",pos.x, pos.y, pos_ofs.x, pos_ofs.y);
    pos.x += pos_ofs.x * 8;
    pos.y += pos_ofs.y * 8;
  
    // ���W
    GFL_CLACT_WK_SetPos( dst->clwk[i], &pos, draw_type );
    // �A�j��
    GFL_CLACT_WK_SetAnmSeq( dst->clwk[i], GFL_CLACT_WK_GetAnmSeq( src->clwk[i] ) );
    // �\���t���O
    GFL_CLACT_WK_SetDrawEnable( dst->clwk[i], src->b_clwk_deco[i] );
  }

  // �����o�R�s�[
  // @TODO CLACT��BMPWIN��ێ����Ă����č\���̂��ۂ��ƃR�s�[����ق����ێ琫�������B
  dst->print_util.transReq = src->print_util.transReq;
  dst->b_useflag = src->b_useflag;
  dst->b_visible = src->b_visible;
  for( i=0; i<PMS_WORD_MAX; i++ )
  {
    dst->b_clwk_deco[i] = src->b_clwk_deco[i];
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�̋󗓂𖄂߂�p���b�g�ԍ��w��
 *
 *	@param	PMS_DRAW_WORK* wk�@���[�N
 *	@param	pltt_pos �p���b�g�ԍ�
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_SetNullColorPallet( PMS_DRAW_WORK* wk, u8 pltt_pos )
{
  GF_ASSERT( wk );

  wk->null_color = pltt_pos;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �����`��F���w��
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *	@param	color �`��F
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_SetPrintColor( PMS_DRAW_WORK* wk, PRINTSYS_LSB color )
{
  GF_ASSERT( wk );

  wk->print_color = color;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �A�N�^�[��BG�X�N���[���ɒǐ�������t���OON�^OFF (�f�t�H���g����)
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *	@param	is_clwk_auto_scroll TRUE:�ǐ�������
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_SetCLWKAutoScrollFlag( PMS_DRAW_WORK* wk, BOOL is_clwk_auto_scroll )
{
  GF_ASSERT( wk );

  wk->is_clwk_auto_scroll = is_clwk_auto_scroll;
}


//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  ���W�v�Z
 *
 *	@param	GFL_BMPWIN* win
 *	@param	width
 *	@param	line
 *	@param	offset
 *	@param	out_pos [OUT] ���W�o��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _get_write_pos( GFL_BMPWIN* win, u8 width, u8 line, GFL_POINT* offset, GFL_POINT* out_pos )
{
  out_pos->x = GFL_BMPWIN_GetPosX( win ) * 8 + width + offset->x;
  out_pos->y = GFL_BMPWIN_GetPosY( win ) * 8 + line * 16 + offset->y;

//  HOSAKA_Printf("pos=%d,%d line=%d width=%d offset=%d,%d \n",out_pos->x, out_pos->y, line, width, offset->x, offset->y );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  VRAM���[�h����Z���̎擾���\�[�X�𔻒�
 *
 *	@param	CLSYS_DRAW_TYPE vram_type 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u32 _obj_get_ncer( CLSYS_DRAW_TYPE vram_type )
{
	GXOBJVRamModeChar vrammode;
  
  if( vram_type == CLSYS_DRAW_MAIN )
  {
    vrammode = GX_GetOBJVRamModeChar();
  }
  else
  {
    vrammode = GXS_GetOBJVRamModeChar();
  }

  switch(vrammode){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		return NARC_pmsi_pms2_obj_dekome_32k_NCER;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		return NARC_pmsi_pms2_obj_dekome_64k_NCER;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		return NARC_pmsi_pms2_obj_dekome_128k_NCER;
	default:
		GF_ASSERT(0);	//��Ή��̃}�b�s���O���[�h
	}
  
  return NARC_pmsi_pms2_obj_dekome_128k_NCER;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  VRAM���[�h����Z���A�j���̎擾���\�[�X�𔻒�
 *
 *	@param	CLSYS_DRAW_TYPE vram_type 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u32 _obj_get_nanr( CLSYS_DRAW_TYPE vram_type )
{
	GXOBJVRamModeChar vrammode;
  
  if( vram_type == CLSYS_DRAW_MAIN )
  {
    vrammode = GX_GetOBJVRamModeChar();
  }
  else
  {
    vrammode = GXS_GetOBJVRamModeChar();
  }
	
  switch(vrammode){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		return NARC_pmsi_pms2_obj_dekome_32k_NANR;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		return NARC_pmsi_pms2_obj_dekome_64k_NANR;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		return NARC_pmsi_pms2_obj_dekome_128k_NANR;
	default:
		GF_ASSERT(0);	//��Ή��̃}�b�s���O���[�h
	}
  
  return NARC_pmsi_pms2_obj_dekome_128k_NANR;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  OBJ ���\�[�X������
 *
 *	@param	PMS_DRAW_OBJ* obj 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _obj_loadres( PMS_DRAW_OBJ* obj, u8 pltt_ofs, HEAPID heap_id )
{
  ARCHANDLE* handle;
  u32 res_ncer = _obj_get_ncer( obj->vram_type );
  u32 res_nanr = _obj_get_nanr( obj->vram_type );

  // �n���h���I�[�v��
  handle	= GFL_ARC_OpenDataHandle( ARCID_PMSI_GRAPHIC, heap_id );

	//���\�[�X�ǂ݂���
	obj->obj_ncl	= GFL_CLGRP_PLTT_RegisterEx( handle, NARC_pmsi_pms2_obj_dekome_NCLR, obj->vram_type, 0x20*pltt_ofs, 0, PMS_DRAW_OBJ_PLTT_NUM, heap_id );
  obj->obj_ncg = GFL_CLGRP_CGR_Register( handle, NARC_pmsi_pms2_obj_dekome_NCGR, FALSE, obj->vram_type, heap_id );
  obj->obj_nce = GFL_CLGRP_CELLANIM_Register( handle, res_ncer, res_nanr, heap_id );

  GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  OBJ ���\�[�X�J��
 *
 *	@param	PMS_DRAW_OBJ* obj 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _obj_unloadres( PMS_DRAW_OBJ* obj )
{
	GFL_CLGRP_PLTT_Release( obj->obj_ncl );
	GFL_CLGRP_CGR_Release( obj->obj_ncg );
	GFL_CLGRP_CELLANIM_Release( obj->obj_nce );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �A�N�^�[����
 *
 *	@param	PMS_DRAW_OBJ* obj 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* _obj_create( PMS_DRAW_OBJ* obj, HEAPID heap_id )
{
  GFL_CLWK* act;
  GFL_CLWK_DATA cldata = {0};
  
//  act = GFL_CLACT_WK_CreateAffine( obj->clwk_unit, 
  act = GFL_CLACT_WK_Create( obj->clwk_unit, 
      obj->obj_ncg, 
      obj->obj_ncl,
      obj->obj_nce,
      &cldata,
      obj->vram_type,
      heap_id
      );

  GFL_CLACT_WK_SetDrawEnable( act, FALSE );
  GFL_CLACT_WK_SetAutoAnmFlag( act, TRUE );
  
  // 30FPS�œ��삵�Ă����ꍇ�̓Z���A�j���{��
  if( GFL_UI_GetFrameRate() == GFL_UI_FRAMERATE_30 )
  {
    GFL_CLACT_WK_SetAutoAnmSpeed( act, FX32_ONE * 2 );
  }

  return act;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �f�R���A�N�^�[�\��
 *
 *	@param	GFL_CLWK* act
 *	@param	win
 *	@param	deco_id 
 *	@param	offset 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _obj_set_deco( GFL_CLWK* act, GFL_BMPWIN* win, u8 width, u8 line, PMS_DECO_ID deco_id, GFL_POINT* offset )
{
  u8 frame;
  GFL_POINT pos;
  GFL_CLACTPOS actpos;

  GF_ASSERT( deco_id > PMS_DECOID_NULL && deco_id < PMS_DECOID_MAX );

  _get_write_pos( win, width, line, offset, &pos );

  actpos.x = pos.x;
  actpos.y = pos.y;
  
  frame = GFL_BMPWIN_GetFrame( win );

  GFL_CLACT_WK_SetPos( act, &actpos, BGFrameToVramType( frame ) );
  GFL_CLACT_WK_SetBgPri( act, GFL_BG_GetPriority( frame ) );
  GFL_CLACT_WK_SetAnmSeq( act, deco_id-1 ); // �A�j����0�I���W��
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �\�����j�b�g ������
 *
 *	@param	PMS_DRAW_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _unit_init( PMS_DRAW_UNIT* unit, PMS_DRAW_OBJ* obj, HEAPID heap_id )
{
  int i;

  for( i=0; i<PMS_WORD_MAX; i++ )
  {
    // �A�N�^�[����
    unit->clwk[i] = _obj_create( obj, heap_id );
  }
  
  unit->b_useflag = FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �\�����j�b�g �폜
 *
 *	@param	PMS_DRAW_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _unit_exit( PMS_DRAW_UNIT* unit )
{
  int i;

  for( i=0; i<PMS_WORD_MAX; i++ )
  {
    // �A�N�^�[���
    GFL_CLACT_WK_Remove( unit->clwk[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �\�����j�b�g �又��
 *
 *	@param	PMS_DRAW_UNIT* unit
 *	@param	que 
 *
 *  @retval BOOL	�܂��]�����I����Ă��Ȃ��ꍇ��TRUE�^����ȊOFALSE
 */
//-----------------------------------------------------------------------------
static BOOL _unit_main( PMS_DRAW_UNIT* unit, PRINT_QUE* que, BOOL is_clwk_auto_scroll )
{
  BOOL doing;

  doing = !PRINT_UTIL_Trans( &unit->print_util, que );

  // �\�����I�����Ă���΃f�R���A�N�^�[�\��
  if( doing == FALSE && unit->b_visible )
  {
    int i;
    for( i=0; i<PMS_WORD_MAX; i++ )
    {
      if( unit->b_clwk_deco[i] )
      {
        GFL_CLACT_WK_SetDrawEnable( unit->clwk[i], TRUE );
      }
    }
  }

  // �X�N���[���J�E���^�̍���CLWK�ɔ��f
  if( is_clwk_auto_scroll )
  {
    int frame = GFL_BMPWIN_GetFrame(unit->print_util.win);
    GFL_CLACTPOS pos = {0};
    int sx;
    int sy;
    int i;

    sx = GFL_BG_GetScreenScrollX( frame );
    sy = GFL_BG_GetScreenScrollY( frame );

    if( sx != unit->pre_scrcnt_x )
    {
      pos.x = unit->pre_scrcnt_x - sx;
      unit->pre_scrcnt_x = sx;
    }

    if( sy != unit->pre_scrcnt_y )
    {
      pos.y = unit->pre_scrcnt_y - sy;
      unit->pre_scrcnt_y = sy;
    }

    // CLWK�ɒl�𗬂�
    if( pos.x != 0 || pos.y != 0 )
    {
      for( i=0; i<PMS_WORD_MAX; i++ )
      {
        GFL_CLACTPOS org;

        GFL_CLACT_WK_GetWldPos( unit->clwk[i], &org );
        HOSAKA_Printf("[%d]pos=%d \n",i, org.x);
        HOSAKA_Printf("[%d]pos=%d \n",i, org.y);
        org.x += pos.x;
        org.y += pos.y;
        GFL_CLACT_WK_SetWldPos( unit->clwk[i], &org );
      }
    }
  } // if( is_clwk_auto_scroll )

  return doing;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �\�����j�b�g �v�����g
 *
 *	@param	PMS_DRAW_UNIT* unit
 *	@param	print_que
 *	@param	font
 *	@param	win
 *	@param	pms
 *	@param  offset
 *	@param	heap_id 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _unit_print( PMS_DRAW_UNIT* unit, PRINT_QUE* print_que, GFL_FONT* font, GFL_BMPWIN* win, PMS_DATA* pms, GFL_POINT* offset, PRINTSYS_LSB print_color, u8 null_color, HEAPID heap_id )
{
  int i;
  STRBUF* buf;
  STRBUF* buf_src;
  u8 wordCount;
  BOOL null_req[ PMS_WORD_MAX ] = { 0 };

  GF_ASSERT_MSG( unit->b_useflag == FALSE, "PMS_DRAW_Print ��d�ďo!" );

  // �v�����g�ݒ菉����
  PRINT_UTIL_Setup( &unit->print_util, win );

  // �o�b�t�@�W�J�O�̕�����
  buf_src = PMSDAT_GetSourceString( pms, heap_id );

  wordCount = PRINTSYS_GetTagCount( buf_src );

//  HOSAKA_Printf("wordCount=%d\n", wordCount);

  for( i=0; i<wordCount; i++ )
  {
    // �f�R������
    if( PMSDAT_IsDecoID( pms, i ) )
    {
      unit->b_clwk_deco[i] = TRUE;
    }
    // �󔻒�
    else if( PMSDAT_GetWordNumber( pms, i) == PMS_WORD_NULL )
    {
      null_req[i] = TRUE;
    }
  }
  
  // �v�����g���N�G�X�g
//  buf = PMSDAT_ToString( pms, heap_id );
  buf = PMSDAT_ToStringWithWordCount( pms, heap_id, wordCount );

#if 0
  {
    u8 l,s,b;
    PRINTSYS_LSB_GetLSB(print_color,&l,&s,&b);
    HOSAKA_Printf("lsb= %d %d %d \n", l,s,b); 
  }
#endif 

// PRINT_UTIL_Print( &unit->print_util, print_que, offset->x, offset->y, buf, font );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(unit->print_util.win), null_color );
  PRINTSYS_PrintQueColor( print_que, GFL_BMPWIN_GetBmp(unit->print_util.win), offset->x, offset->y, buf, font, print_color );
	unit->print_util.transReq = TRUE;

  GFL_STR_DeleteBuffer( buf );
  
  for( i=0; i<wordCount; i++ )
  {
    // �f�R���A�N�^�[�ݒ�
    if( unit->b_clwk_deco[i] == TRUE )
    {
      u8 width, line;
      PMS_DECO_ID deco_id;
      
      width = PRINTSYS_GetTagWidth( buf_src, i, font, 0 );
      line  = PRINTSYS_GetTagLine( buf_src, i );
    
      deco_id = PMSDAT_GetWordNumber( pms, i );

//      HOSAKA_Printf("[%d]deco_id=%d\n",i, deco_id);

      _obj_set_deco( unit->clwk[i], win, width, line, deco_id, offset );
    }
    // ��\��
    else if( null_req[i] == TRUE )
    {
        u8 width, line;
        GFL_POINT pos;

        width = PRINTSYS_GetTagWidth( buf_src, i, font, 0 );
        line  = PRINTSYS_GetTagLine( buf_src, i );

//        HOSAKA_Printf("wordpos [%d] is null \n",i );

        // BMPWIN�����J���[�Ŗ��߂�
        GFL_BMP_Fill( GFL_BMPWIN_GetBmp(win), 
            width, line * 16,
            PMS_NULL_WIDTH, 16, null_color );
    }
  }
  
  // @TODO ��x�J�����Ă���̂ł��܂�������ǂ��Ȃ��B
  // ���ׂ�������悤�ł���΁APMSDAT_ToString�̒��ł��Ă��邱�Ƃ�������Ɏ����Ă���B
  // ��������PMSDAT�̕��Ōォ�烌�W�X�g����֐������B
  GFL_STR_DeleteBuffer( buf_src );
      
  // �X�N���[���]��
	GFL_BMPWIN_MakeScreen( unit->print_util.win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(unit->print_util.win) );

  unit->b_useflag = TRUE;
  unit->b_visible = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �\�����j�b�g �N���A
 *
 *	@param	PMS_DRAW_UNIT* unit 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _unit_clear( PMS_DRAW_UNIT* unit, BOOL is_trans )
{
  GF_ASSERT( unit->b_useflag == TRUE );

  // �L�������N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(unit->print_util.win), 0 );

  if( is_trans )
  {
    // VRAM�L�����N�^�𑦎��]��
    GFL_BMPWIN_TransVramCharacter( unit->print_util.win );
    // �X�N���[�����N���A���đ����]��
    GFL_BMPWIN_ClearTransWindow( unit->print_util.win );
  }
  
  // �摜��\��
  {
    int i;
    for( i=0; i<PMS_WORD_MAX; i++ )
    {
      GFL_CLACT_WK_SetDrawEnable( unit->clwk[i], FALSE );
      unit->b_clwk_deco[i] = FALSE;
    }
  }
  unit->pre_scrcnt_x = 0;
  unit->pre_scrcnt_y = 0;

  unit->b_useflag = FALSE;
  unit->b_visible = FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �\�����j�b�g �\��|��\���ؑ�
 *
 *	@param	PMS_DRAW_UNIT* unit 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _unit_visible_set( PMS_DRAW_UNIT* unit, BOOL is_visible )
{
  GF_ASSERT( unit->b_useflag == TRUE );

  unit->b_visible = is_visible;

  // �X�N���[���]��
  if( is_visible ){
	  GFL_BMPWIN_MakeScreen( unit->print_util.win );
  }else{
    // �X�N���[�����N���A���đ����]��
	  GFL_BMPWIN_ClearScreen( unit->print_util.win );
  } 
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(unit->print_util.win) );
  
  // �摜��\��
  {
    int i;
    for( i=0; i<PMS_WORD_MAX; i++ )
    {
      if( unit->b_clwk_deco[i] ){
        GFL_CLACT_WK_SetDrawEnable( unit->clwk[i], is_visible );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BG�t���[������OBJ�pVRAMTYPE���擾
 *
 *	@param	u8 frame 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static CLSYS_DRAW_TYPE BGFrameToVramType( u8 frame )
{
  GF_ASSERT( frame <= GFL_BG_FRAME3_S );

  if( frame >= GFL_BG_FRAME0_S )
  {
    return CLSYS_DRAW_SUB;
  }

  return CLSYS_DRAW_MAIN;
}



