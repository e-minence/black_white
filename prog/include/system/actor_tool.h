//==============================================================================
/**
 * @file	actor_tool.h
 * @brief	�A�N�^�[����p�c�[���̃w�b�_
 * @author	matsuda
 * @date	2008.12.25(��)
 */
//==============================================================================
#ifndef __ACTOR_TOOL_H__
#define __ACTOR_TOOL_H__

#include <obj_graphic_man.h>

//==============================================================================
//	�^��`
//==============================================================================
///�p���b�g�X���b�g�Ǘ����[�N�̕s��`�A�N�Z�X�|�C���^
typedef struct _PLTTSLOT_SYSTEM * PLTTSLOT_SYS_PTR;


//==============================================================================
//	�O���֐��錾
//==============================================================================
#if 1
extern PLTTSLOT_SYS_PTR PLTTSLOT_Init(int heap_id, int main_num, int sub_num);
extern void PLTTSLOT_Exit(PLTTSLOT_SYS_PTR pssp);
extern u32 PLTTSLOT_ResourceSet(PLTTSLOT_SYS_PTR pssp, ARCHANDLE *handle, u32 data_id, 
	GFL_VRAM_TYPE vram_type, int pltt_num, int heap_id);
extern u32 PLTTSLOT_GetPalNo(PLTTSLOT_SYS_PTR pssp, u32 index, GFL_VRAM_TYPE vram_type);
extern void PLTTSLOT_ResourceFree(PLTTSLOT_SYS_PTR pssp, u32 index, GFL_VRAM_TYPE vram_type);
#endif

#endif	//__ACTOR_TOOL_H__
