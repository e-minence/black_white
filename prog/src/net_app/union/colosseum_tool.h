//==============================================================================
/**
 * @file    colosseum_tool.h
 * @brief   コロシアムツール類：オーバーレイに配置
 * @author  matsuda
 * @date    2009.07.18(土)
 */
//==============================================================================
#pragma once

//==============================================================================
//  外部関数宣言
//==============================================================================
extern BOOL ColosseumTool_AllReceiveCheck_BasicStatus(COLOSSEUM_SYSTEM_PTR clsys);
extern BOOL ColosseumTool_AllReceiveCheck_TrainerCard(COLOSSEUM_SYSTEM_PTR clsys);
extern BOOL ColosseumTool_AllReceiveCheck_Pokeparty(COLOSSEUM_SYSTEM_PTR clsys);
extern BOOL ColosseumTool_ReceiveCheck_StandPos(COLOSSEUM_SYSTEM_PTR clsys);
extern BOOL ColosseumTool_ReceiveCheck_StandingPos(COLOSSEUM_SYSTEM_PTR clsys);
extern void ColosseumTool_Clear_ReceiveStandingPos(COLOSSEUM_SYSTEM_PTR clsys);
extern void ColosseumTool_Clear_ReceivePokeParty(COLOSSEUM_SYSTEM_PTR clsys, BOOL except_for_mine);
extern BOOL ColosseumTool_CheckStandingPosition(FIELD_MAIN_WORK *fieldWork, int entry_num, int *stand_pos);
extern void ColosseumTool_CommPlayerUpdate(COLOSSEUM_SYSTEM_PTR clsys);
