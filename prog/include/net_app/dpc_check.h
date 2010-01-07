//=============================================================================
/**
 * @file	  dpc_check.h
 * @bfief	  �f�W�^���v���[���[�J�[�h����
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  10/01/07
 */
//=============================================================================




#pragma once

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"


typedef struct _DPC_CHECK_WORK DPC_CHECK_WORK;

extern const GFL_PROC_DATA DPC_CheckProcData;


typedef enum  {
  DPC_CHECK_RETURNMODE_OK,         // �o�g���\
  DPC_CHECK_RETURNMODE_END,        // �����؂�
  DPC_CHECK_RETURNMODE_FAILED,     // �_�E�����[�h���s
  DPC_CHECK_RETURNMODE_NOACCOUNT,  // ��D���N���u�T�[�o�ɓo�^������

  DPC_CHECK_RETURNMODE_MAX,
}DPC_CHECK_RETURNMODE;

typedef struct{
  DPC_CHECK_RETURNMODE mode;
  GAMEDATA* gameData;
  HEAPID heapID;
} DPC_CHECK_INITWORK;

