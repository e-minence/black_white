//==============================================================================
/**
 * @file    colosseum.h
 * @brief   �R���V�A���̃w�b�_
 * @author  matsuda
 * @date    2009.07.17(��)
 */
//==============================================================================
#pragma once

#include "savedata/mystatus.h"
#include "field\fieldmap_proc.h"
#include "gamesystem/game_data.h"
#include "net_app/union/comm_player.h"


//==============================================================================
//  �^��`
//==============================================================================
///COMM_PLAYER_SYS�\���̂̕s��`�|�C���^�^
typedef struct _COLOSSEUM_SYSTEM * COLOSSEUM_SYSTEM_PTR;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern COLOSSEUM_SYSTEM_PTR Colosseum_InitSystem(GAMEDATA *game_data, FIELD_MAIN_WORK *fieldWork, MYSTATUS *myst, BOOL intrude);
extern void Colosseum_ExitSystem(COLOSSEUM_SYSTEM_PTR clsys);
extern void Colosseum_CommReadySet(COLOSSEUM_SYSTEM_PTR clsys, BOOL flag);
extern void Colosseum_Mine_SetStandingPostion(COLOSSEUM_SYSTEM_PTR clsys, int stand_position);
extern u8 Colosseum_Mine_GetStandingPostion(COLOSSEUM_SYSTEM_PTR clsys);
extern void Colosseum_Mine_SetAnswerStandingPosition(COLOSSEUM_SYSTEM_PTR clsys, int result);
extern u8 Colosseum_Mine_GetAnswerStandingPosition(COLOSSEUM_SYSTEM_PTR clsys);
extern BOOL Colosseum_Parent_SetStandingPosition(COLOSSEUM_SYSTEM_PTR clsys, int net_id, u8 stand_position);
extern void Colosseum_Parent_SendAnswerStandingPosition(COLOSSEUM_SYSTEM_PTR clsys);
extern void Colosseum_SetCommPlayerPos(COLOSSEUM_SYSTEM_PTR clsys, int net_id, const COMM_PLAYER_PACKAGE *pack);
extern BOOL Colosseum_GetCommPlayerPos(COLOSSEUM_SYSTEM_PTR clsys, int net_id, COMM_PLAYER_PACKAGE *dest);

