//==============================================================================
/**
 * @file    event_comm_error.c
 * @brief   �t�B�[���h�ł̒ʐM�G���[��ʌĂяo��
 * @author  matsuda
 * @date    2009.09.14(��)
 */
//==============================================================================
#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap.h"
#include "gamesystem\gamesystem.h"

#include "./event_fieldmap_control.h"
#include "event_comm_error.h"
#include "system/net_err.h"


//==============================================================================
//  
//==============================================================================
FS_EXTERN_OVERLAY(fieldmap);

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT FieldCommErrorProc_Init(GFL_PROC * proc, int * seq , void *pwk, void *mywk);
static GFL_PROC_RESULT FieldCommErrorProc_Main(GFL_PROC * proc, int * seq , void *pwk, void *mywk);
static GFL_PROC_RESULT FieldCommErrorProc_Exit(GFL_PROC * proc, int * seq , void *pwk, void *mywk);

//==============================================================================
//  �f�[�^
//==============================================================================
static const GFL_PROC_DATA FieldCommErrorProc = {
  FieldCommErrorProc_Init,
  FieldCommErrorProc_Main,
  FieldCommErrorProc_Exit,
};



//==================================================================
/**
 * �t�B�[���h�ł̒ʐM�G���[��ʌĂяo���C�x���g
 *
 * @param   gsys		
 * @param   fieldmap		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_FieldCommErrorProc(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap)
{
  GAMESYSTEM_SetFieldCommErrorReq(gsys, FALSE);
  NetErr_App_ReqErrorDisp();
  return EVENT_FieldSubProc(gsys, fieldmap, FS_OVERLAY_ID(fieldmap), &FieldCommErrorProc, NULL);
}


//==============================================================================
//
//  �_�~�[PROC
//      PROC�J�ڂŃG���[��ʂ�\��������ׂ����̃_�~�[PROC
//
//==============================================================================
static GFL_PROC_RESULT FieldCommErrorProc_Init(GFL_PROC * proc, int * seq , void *pwk, void *mywk)
{
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT FieldCommErrorProc_Main(GFL_PROC * proc, int * seq , void *pwk, void *mywk)
{
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT FieldCommErrorProc_Exit(GFL_PROC * proc, int * seq , void *pwk, void *mywk)
{
  return GFL_PROC_RES_FINISH;
}
