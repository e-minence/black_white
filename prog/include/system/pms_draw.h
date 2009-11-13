//============================================================================
/**
 *
 *	@file		pms_draw.h
 *	@brief  �ȈՉ�b�\���V�X�e���i�ȈՉ�b+�f�R���Ǘ��j
 *	@author	hosaka genya
 *	@data		2009.10.14
 *
 *	@note   BMPWIN�ɑ΂��āA�f�R�����܂񂾊ȈՉ�b��\�����܂��B
 *	@note   OBJ�̃L����VRAM���16 k�B�p���b�g��5�{����܂��B 
 *
 */
//============================================================================
#pragma once

#include <gflib.h>
#include "system/pms_data.h"
#include "print/printsys.h"

typedef struct _PMS_DRAW_WORK PMS_DRAW_WORK;

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  // �ȈՉ�b�\���V�X�e���Ŏg�p����p���b�g��
  PMS_DRAW_OBJ_PLTT_NUM = 5,
  // �f�t�H���g�̋󗓓h��Ԃ��p���b�g�ԍ�
  PMS_DRAW_DEF_NULL_COLOR = 0xF,
};


//=============================================================================
/**
 *								EXTERN�錾
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
extern PMS_DRAW_WORK* PMS_DRAW_Init( GFL_CLUNIT* clunit, CLSYS_DRAW_TYPE vram_type, PRINT_QUE* que, GFL_FONT* font, u8 obj_pltt_ofs, u8 id_max, HEAPID heap_id );
//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\���V�X�e�� �又��
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
extern void PMS_DRAW_Main( PMS_DRAW_WORK* wk );
//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\���V�X�e�� �J������
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
extern void PMS_DRAW_Exit( PMS_DRAW_WORK* wk );
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
extern void PMS_DRAW_Print( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, PMS_DATA* pms, u8 id );
//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�ɊȈՉ�b��\��
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
extern void PMS_DRAW_PrintOffset( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, PMS_DATA* pms, u8 id, GFL_POINT* offset );
//-----------------------------------------------------------------------------
/**
 *	@brief  �w��ID�̕\���I���`�F�b�N
 *
 *	@param	PMS_DRAW_WORK* wk ���[�N
 *
 *	@retval	TRUE:�v�����g�I��
 */
//-----------------------------------------------------------------------------
extern BOOL PMS_DRAW_IsPrintEnd( PMS_DRAW_WORK* wk );
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
extern void PMS_DRAW_Clear( PMS_DRAW_WORK* wk, u8 id, BOOL is_trans );
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
extern BOOL PMS_DRAW_IsPrinting( PMS_DRAW_WORK* wk, u8 id );
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
extern void PMS_DRAW_Copy( PMS_DRAW_WORK* wk, u8 id_src, u8 id_dst );
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
extern void PMS_DRAW_SetNullColorPallet( PMS_DRAW_WORK* wk, u8 pltt_pos );

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
extern void PMS_DRAW_SetPrintColor( PMS_DRAW_WORK* wk, PRINTSYS_LSB color );

