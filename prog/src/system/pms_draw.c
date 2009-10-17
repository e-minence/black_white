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
  BOOL          b_useflag;
  PRINT_UTIL    print_util;
  GFL_BMPWIN*   win;
  GFL_CLWK*     clwk[2];
} PMS_DRAW_UNIT;

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
struct _PMS_DRAW_WORK {
  // [IN]
  HEAPID      heap_id;
  GFL_CLUNIT* clwk_unit;
  GFL_FONT*   font;
  PRINT_QUE*  print_que;
  // [PRIVATE]
  u8 unit_num;
  u8 padding[3];
  BOOL            b_print_end;  ///< �v�����g�I���t���O
  PMS_DRAW_UNIT*  unit;
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static void _unit_init( PMS_DRAW_UNIT* unit );
static void _unit_exit( PMS_DRAW_UNIT* unit );
static BOOL _unit_proc( PMS_DRAW_UNIT* unit, PRINT_QUE* que );
static void _unit_print( PMS_DRAW_UNIT* unit, PRINT_QUE* print_que, GFL_FONT* font, GFL_BMPWIN* win, PMS_DATA* pms, HEAPID heap_id );
static void _unit_clear( PMS_DRAW_UNIT* unit );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\���V�X�e�� ������
 *
 *	@param	GFL_CLUNIT* clunit
 *	@param	que
 *	@param	obj_pltt_ofs
 *	@param	id_max  �Ǘ�ID��
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
PMS_DRAW_WORK* PMS_DRAW_Init( GFL_CLUNIT* clunit, PRINT_QUE* que, GFL_FONT* font, u8 obj_pltt_ofs, u8 id_max, HEAPID heap_id )
{
  int i;
  PMS_DRAW_WORK* wk;

  // �q�[�v����
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(PMS_DRAW_WORK) );
  GFL_STD_MemClear( wk, sizeof(PMS_DRAW_WORK) );

  wk->heap_id     = heap_id;
  wk->clwk_unit   = clunit;
  wk->print_que   = que;
  wk->font        = font;
  wk->unit_num    = id_max;

  // ���j�b�g������
  wk->unit = GFL_HEAP_AllocClearMemory( heap_id, sizeof(PMS_DRAW_UNIT) *id_max );
  GFL_STD_MemClear( wk->unit, sizeof(PMS_DRAW_UNIT) * id_max );

  // OBJ ���\�[�X�擾

  for( i=0; i<wk->unit_num; i++ )
  {
    _unit_init( &wk->unit[i] );
  }

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\���V�X�e�� �又��
 *
 *	@param	PMS_DRAW_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Proc( PMS_DRAW_WORK* wk )
{
  int i;

  // �����Ƃ��Ȃ���Γ]���I��
  wk->b_print_end = TRUE;
  
  for( i=0; i<wk->unit_num; i++ )
  {
    if( _unit_proc( &wk->unit[i], wk->print_que ) )
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
 *	@param	PMS_DRAW_WORK* wk 
 *
 *	@retval
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

  // �q�[�v�J��
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�ɊȈՉ�b��\��
 *
 *	@param	PMS_DRAW_WORK* wk
 *	@param	win
 *	@param	pms
 *	@param	id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
// @TODO �揟���œ��삳����
void PMS_DRAW_Print( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, PMS_DATA* pms, u8 id )
{ 
  PMS_DRAW_UNIT* unit;

  GF_ASSERT( wk && win && pms );
  GF_ASSERT( id < wk->unit_num );

  _unit_print( &wk->unit[id], wk->print_que, wk->font, win, pms, wk->heap_id );
    
  // PROC��ʂ�܂ł͓]������Ȃ�
  wk->b_print_end = FALSE;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�̕\���I���`�F�b�N
 *
 *	@param	PMS_DRAW_WORK* wk
 *	@param	id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL PMS_DRAW_IsPrintEnd( PMS_DRAW_WORK* wk, u8 id )
{
  return wk->b_print_end;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�̕\���N���A
 *
 *	@param	PMS_DRAW_WORK* wk
 *	@param	id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Clear( PMS_DRAW_WORK* wk, u8 id )
{ 
  PMS_DRAW_UNIT* unit;

  GF_ASSERT( wk );
  GF_ASSERT( id < wk->unit_num );
  
  _unit_clear( &wk->unit[id] );
}


//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �\�����j�b�g ������
 *
 *	@param	PMS_DRAW_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _unit_init( PMS_DRAW_UNIT* unit )
{
  unit->b_useflag = FALSE;
  // @TODO �A�N�^�[������
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
  GFL_BMPWIN* win;

  win = unit->print_util.win;

  GF_ASSERT( win );
  GFL_BMPWIN_Delete( win );

  // @TODO �A�N�^�[���
  
  // @TODO OBJ�̋����I���ɒ���

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
static BOOL _unit_proc( PMS_DRAW_UNIT* unit, PRINT_QUE* que )
{
  BOOL doing;

  doing = PRINT_UTIL_Trans( &unit->print_util, que );
  
  if( doing == FALSE )
  {
    // @TODO �\�����I�����Ă���΃A�N�^�[�\��
  }

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
 *	@param	heap_id 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _unit_print( PMS_DRAW_UNIT* unit, PRINT_QUE* print_que, GFL_FONT* font, GFL_BMPWIN* win, PMS_DATA* pms, HEAPID heap_id )
{
  STRBUF* buf;

  GF_ASSERT( unit->b_useflag == FALSE );

  // �v�����g�ݒ菉����
  PRINT_UTIL_Setup( &unit->print_util, win );

  // �v�����g���N�G�X�g
  buf = PMSDAT_ToString( pms, heap_id );
  PRINT_UTIL_Print( &unit->print_util, print_que, 0, 0, buf, font );
  GFL_STR_DeleteBuffer( buf );
 
  unit->b_useflag = TRUE;
      
  // �]��
	GFL_BMPWIN_MakeScreen( unit->print_util.win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame( unit->print_util.win) );
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
static void _unit_clear( PMS_DRAW_UNIT* unit )
{
  GF_ASSERT( unit->b_useflag == FALSE );

  // �X�N���[�����N���A���ē]��
	GFL_BMPWIN_ClearScreen( unit->print_util.win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame( unit->print_util.win) );

  // @TODO �摜��\��

  unit->b_useflag = FALSE;

}
