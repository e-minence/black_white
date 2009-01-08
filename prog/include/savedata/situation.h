//==============================================================================
/**
 * @file	situation.h
 * @brief	状況データアクセスのヘッダ
 * @author	matsuda
 * @date	2009.01.08(木)
 */
//==============================================================================
#ifndef __SITUATION_H__
#define __SITUATION_H__

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK参照のため
#include "gamesystem/playerwork.h"
#include "field/location.h"


//==============================================================================
//	型定義
//==============================================================================
///状況データの不定形アクセス型
typedef struct _SITUATION SITUATION;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern int Situation_GetWorkSize(void);
extern void Situation_Init(SITUATION * st);
extern LOCATION * Situation_GetStartLocation(SITUATION * st);
extern LOCATION * Situation_GetEntranceLocation(SITUATION * st);
extern LOCATION * Situation_GetSpecialLocation(SITUATION * st);
extern SITUATION * SaveData_GetSituation(SAVE_CONTROL_WORK * sv);
extern void SaveData_SituationDataUpdate(SAVE_CONTROL_WORK *sv, const PLAYER_WORK *pw);
extern void SaveData_SituationDataLoad(SAVE_CONTROL_WORK *sv, PLAYER_WORK *pw);


#endif	//__SITUATION_H__
