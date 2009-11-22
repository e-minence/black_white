//=============================================================================
/**
 * @file	  gtsnego.h
 * @bfief	  GTS�l�S�V�G�[�V�����֘A�̃w�b�_�[
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/11/15
 */
//=============================================================================


#pragma once

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

//�e���[�N
typedef struct {
  int selectLV;
  int selectType;
} EVENT_GTSNEGO_USER_DATA;

typedef struct {
  GAMESYS_WORK * gsys;
  EVENT_GTSNEGO_USER_DATA aUser[2];
  MYSTATUS* pStatus[2];
  SAVE_CONTROL_WORK* ctrl;
  void* fieldmap;
} EVENT_GTSNEGO_WORK;

typedef struct _GTSNEGO_WORK  GTSNEGO_WORK;

extern const GFL_PROC_DATA GtsNego_ProcData;








