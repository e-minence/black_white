//==============================================================================
/**
 * @file	footprint_tool.h
 * @brief	���Ճ{�[�h�̃c�[���ނ̃w�b�_
 * @author	matsuda
 * @date	2008.01.22(��)
 */
//==============================================================================
#ifndef __FOOTPRINT_TOOL_H__
#define __FOOTPRINT_TOOL_H__

#include "print/printsys.h"


//==============================================================================
//	�萔��`
//==============================================================================
///FootprintTool_TouchUpdate�֐��̖߂�l
enum{
	FOOT_TOUCH_RET_EXIT = 0xfe,		//�u��߂�v�{�^����������
	FOOT_TOUCH_RET_NULL = 0xff,		//���������Ă��Ȃ�
};


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void FootPrintTool_NameDraw(GFL_MSGDATA *msgman, WORDSET *wordset, GFL_BMPWIN *win[], PRINT_UTIL print_util[], WFLBY_SYSTEM *wflby_sys, s32 user_id, PRINT_QUE *printQue, GFL_FONT *font_handle);
extern void FootPrintTool_NameErase(GFL_BMPWIN *win[], u32 user_index);
extern u16 FootprintTool_StampColorGet(int board_type, u32 oya_id);
extern int FootprintTool_InkPalTouchUpdate(STAMP_PARAM *my_stamp_array, int now_select_no);
extern BOOL FootprintTool_FootDispCheck(int monsno, int form_no, BOOL arceus_flg);
extern BOOL FootprintTool_FootHitSizeGet(int monsno, int form_no);


#endif	//__FOOTPRINT_TOOL_H__
