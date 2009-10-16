//============================================================================
/**
 *
 *	@file		pms_draw.h
 *	@brief  �ȈՉ�b�\���V�X�e���i�ȈՉ�b+�f�R���Ǘ��j
 *	@author	hosaka genya
 *	@data		2009.10.14
 *
 */
//============================================================================
#pragma once

#include <gflib.h>
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
  PMS_DRAW_OBJ_PLTT_NUM = 2,
};

//--------------------------------------------------------------
///	
//==============================================================
typedef struct {
  int d;
} PMS_DRAW_DATA;


//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================

extern PMS_DRAW_WORK* PMS_DRAW_Init( GFL_CLUNIT* clunit, PRINT_QUE* que, u8 obj_pltt_ofs, u8 id_max, HEAPID heap_id );
extern void PMS_DRAW_Proc( PMS_DRAW_WORK* wk );
extern void PMS_DRAW_Exit( PMS_DRAW_WORK* wk );
extern void PMS_DRAW_Print( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, PMS_DRAW_DATA* data, u8 id );
extern BOOL PMS_DRAW_IsPrintEnd( PMS_DRAW_WORK* wk, u8 id );
extern void PMS_DRAW_Clear( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, u8 id );

