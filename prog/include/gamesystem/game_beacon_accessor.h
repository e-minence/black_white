//==============================================================================
/**
 * @file    game_beacon_accessor.h
 * @brief   �r�[�R�����̒l�擾�ׂ̈̃A�N�Z�T
 * @author  matsuda
 * @date    2010.01.19(��)
 */
//==============================================================================
#pragma once

#include "gamesystem/game_beacon.h"


//==============================================================================
//  �O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//  �r�[�R���e�[�u������n
//--------------------------------------------------------------
extern int GAMEBEACON_Stack_Update(GAMEBEACON_INFO_TBL *stack_infotbl);
extern BOOL GAMEBEACON_Stack_GetInfo(GAMEBEACON_INFO_TBL *stack_infotbl, GAMEBEACON_INFO *dest_info, u16 *time);

extern int GAMEBEACON_InfoTbl_SetBeacon(GAMEBEACON_INFO_TBL *infotbl, const GAMEBEACON_INFO *set_info, u16 time, BOOL push_out);
extern BOOL GAMEBEACON_InfoTbl_GetBeacon(GAMEBEACON_INFO_TBL *infotbl, GAMEBEACON_INFO *dest_info, u16 *time, int log_no);

extern GAMEBEACON_INFO * GAMEBEACON_Alloc(HEAPID heap_id);
extern GAMEBEACON_INFO_TBL * GAMEBEACON_InfoTbl_Alloc(HEAPID heap_id);

//--------------------------------------------------------------
//  �A�N�Z�T
//--------------------------------------------------------------
extern const STRCODE * GAMEBEACON_Get_PlayerName(const GAMEBEACON_INFO *info);
extern void GAMEBEACON_Get_FavoriteColor(GXRgb *dest_buf, const GAMEBEACON_INFO *info);
extern u8 GAMEBEACON_Get_TrainerView(const GAMEBEACON_INFO *info);
extern u8 GAMEBEACON_Get_Sex(const GAMEBEACON_INFO *info);
extern GPOWER_ID GAMEBEACON_Get_GPowerID(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_ZoneID(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_ThanksRecvCount(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_SuretigaiCount(const GAMEBEACON_INFO *info);
extern GAMEBEACON_DETAILS_NO GAMEBEACON_Get_DetailsNo(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Details_BattleTrainer(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Details_BattleMonsNo(const GAMEBEACON_INFO *info);
extern GAMEBEACON_ACTION GAMEBEACON_Get_Action_ActionNo(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_Action_ItemNo(const GAMEBEACON_INFO *info);
extern const STRCODE * GAMEBEACON_Get_Action_Nickname(const GAMEBEACON_INFO *info);
