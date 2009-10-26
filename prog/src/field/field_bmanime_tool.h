//============================================================================================
/**
 * @file field_bmanime_tool.h
 * @brief �z�u���f���A�j���R���g���[���[
 * @date  2009.10.25
 * @author  tamada GAME FREAK inc.
 *
 */
//============================================================================================
#pragma once

#include <gflib.h>
#include "field_buildmodel.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _BMANIME_CONTROL_WORK BMANIME_CONTROL_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
  //�A�j���f�[�^�̒�`���Ɉˑ����Ă���B
  ANM_INDEX_DOOR_OPEN = 0,
  ANM_INDEX_DOOR_CLOSE,
};

//============================================================================================
//============================================================================================
extern BMANIME_CONTROL_WORK * BMANIME_CTRL_Create(FIELD_BMODEL_MAN * bmodel_man, const VecFx32 * pos);
extern void BMANIME_CTRL_Delete(BMANIME_CONTROL_WORK * ctrl);
extern void BMANIME_CTRL_SetAnime(BMANIME_CONTROL_WORK * ctrl, u32 anm_idx);
extern BOOL BMANIME_CTRL_WaitAnime(BMANIME_CONTROL_WORK * ctrl);
