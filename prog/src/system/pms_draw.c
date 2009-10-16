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
///	��񕪂̊Ǘ��f�[�^
//==============================================================
typedef struct {
  GFL_CLWK* clwk[2];
} PMS_DRAW_UNIT;

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
struct _PMS_DRAW_WORK {
  // [IN]
  GFL_CLUNIT* clwk_unit;
  PRINT_QUE*  print_que;
  // [PRIVATE]
  u8 unit_num;
  u8 padding[3];
  PMS_DRAW_UNIT*  unit;
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static void _unit_init( PMS_DRAW_UNIT* unit );
static void _unit_exit( PMS_DRAW_UNIT* unit );

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
PMS_DRAW_WORK* PMS_DRAW_Init( GFL_CLUNIT* clunit, PRINT_QUE* que, u8 obj_pltt_ofs, u8 id_max, HEAPID heap_id )
{
  int i;
  PMS_DRAW_WORK* wk;

  // �q�[�v����
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(PMS_DRAW_WORK) );
  GFL_STD_MemClear( wk, sizeof(PMS_DRAW_WORK) );

  wk->clwk_unit   = clunit;
  wk->print_que   = que;
  wk->unit_num    = id_max;

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
//inline BOOL PRINT_UTIL_Trans( PRINT_UTIL* wk, PRINT_QUE* que )

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

  // OBJ�̋����I���ɒ���

  for( i=0; i<wk->unit_num; i++ )
  {
    _unit_exit( &wk->unit[i] );
    GFL_HEAP_FreeMemory( wk->unit );
  }

  // �q�[�v�J��
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�ɊȈՉ�b��\��
 *
 *	@param	PMS_DRAW_WORK* wk
 *	@param	win
 *	@param	data
 *	@param	id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Print( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, PMS_DRAW_DATA* data, u8 id )
{ 
  // �揟���œ��삳����
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

}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�̕\���N���A
 *
 *	@param	PMS_DRAW_WORK* wk
 *	@param	win
 *	@param	id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Clear( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, u8 id )
{ 
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
  // @TODO �A�N�^�[���
  GFL_HEAP_FreeMemory( unit );
}


