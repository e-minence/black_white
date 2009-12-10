//======================================================================
/**
 * @file  gym_dragon.h
 * @brief  �h���S���W��
 * @author  Saito
 */
//======================================================================
#pragma once

extern void GYM_DRAGON_Setup(FIELDMAP_WORK *fieldWork);
extern void GYM_DRAGON_End(FIELDMAP_WORK *fieldWork);
extern void GYM_DRAGON_Move(FIELDMAP_WORK *fieldWork);
extern GMEVENT *GYM_DRAGON_CreateGmkEvt(GAMESYS_WORK *gsys, const int inDragonIdx, const int inArmIdx);
extern GMEVENT *GYM_DRAGON_CreateJumpDownEvt(GAMESYS_WORK *gsys, const int inDir);

