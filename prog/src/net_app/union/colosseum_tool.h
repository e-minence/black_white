//==============================================================================
/**
 * @file    colosseum_tool.h
 * @brief   �R���V�A���c�[���ށF�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2009.07.18(�y)
 */
//==============================================================================
#pragma once

//==============================================================================
//  �O���֐��錾
//==============================================================================
extern BOOL ColosseumTool_AllReceiveCheck_BasicStatus(COLOSSEUM_SYSTEM_PTR clsys);
extern BOOL ColosseumTool_AllReceiveCheck_TrainerCard(COLOSSEUM_SYSTEM_PTR clsys);
extern BOOL ColosseumTool_CheckStandingPosition(FIELD_MAIN_WORK *fieldWork, int entry_num, int *stand_pos);
extern void ColosseumTool_CommPlayerUpdate(COLOSSEUM_SYSTEM_PTR clsys);
