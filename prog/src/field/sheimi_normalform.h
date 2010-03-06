//============================================================================================
/**
 * @file  sheimi_normalform.h
 * @bfief �V�F�C�~���ԉz���t�H�����߂�
 * @author  Saito
 */
//============================================================================================
#pragma once

#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"

extern void SHEIMI_NFORM_ChangeNormal(POKEPARTY *ppt);
extern BOOL SHEIMI_NFORM_ChangeNormal_TimeUpdate(POKEPARTY *ppt, int min_diff, const RTCTime * now);


