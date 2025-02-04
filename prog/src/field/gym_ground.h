//======================================================================
/**
 * @file  gym_ground.h
 * @brief  �n�ʃW��
 * @author  Saito
 */
//======================================================================
#pragma once

extern void GYM_GROUND_Setup(FIELDMAP_WORK *fieldWork);
extern void GYM_GROUND_End(FIELDMAP_WORK *fieldWork);
extern void GYM_GROUND_Move(FIELDMAP_WORK *fieldWork);

extern GMEVENT *GYM_GROUND_CreateLiftMoveEvt(GAMESYS_WORK *gsys, const int inLiftIdx);
extern GMEVENT *GYM_GROUND_CreateExitLiftMoveEvt(GAMESYS_WORK *gsys, const BOOL inExit);
extern void GYM_GROUND_SetEnterPos(GAMESYS_WORK *gsys);
