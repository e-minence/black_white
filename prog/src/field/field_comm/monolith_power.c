//==============================================================================
/**
 * @file    monolith_power.c
 * @brief   ���m���X�F�p���[���
 * @author  matsuda
 * @date    2009.11.20(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"


//==============================================================================
//  �f�[�^
//==============================================================================
///���m���X�p���[ROC�f�[�^
const GFL_PROC_DATA MonolithProcData = {
  MonolithProc_Init,
  MonolithProc_Main,
  MonolithProc_End,
};


