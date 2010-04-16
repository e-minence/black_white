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
#include "field/game_beacon_search.h"


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
static GMEVENT_RESULT _EventCommErrorExit( GMEVENT *event, int *seq, void *wk );

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
GMEVENT * EVENT_FieldCommErrorProc(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap)
{
  GAME_COMM_NO comm_no = GameCommSys_GetLastCommNo(GAMESYSTEM_GetGameCommSysPtr(gsys));

  if(comm_no == GAME_COMM_NO_INVASION){
    if(GAMEDATA_GetIntrudeReverseArea(GAMESYSTEM_GetGameData(gsys)) == TRUE){
      //���t�B�[���h�N�����̓G���[���o��
      NetErr_App_ReqErrorDisp();
      return EVENT_FieldSubProc(gsys, fieldmap, FS_OVERLAY_ID(fieldmap), &FieldCommErrorProc, gsys);
    }
    else{
    	return GMEVENT_Create( gsys, NULL, _EventCommErrorExit, 0 );
    }
  }
  else{ //�N���ȊO�ł͒ʏ�ʂ�G���[��ʂ�\��
    NetErr_App_ReqErrorDisp();
    return EVENT_FieldSubProc(gsys, fieldmap, FS_OVERLAY_ID(fieldmap), &FieldCommErrorProc, gsys);  }
}

//--------------------------------------------------------------
/**
 * �ʐM�G���[��ʂ��o�����ɁA�ʐM�I�����A�G���[��������������C�x���g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event dis_wk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT _EventCommErrorExit( GMEVENT *event, int *seq, void *wk )
{
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	
  OS_TPrintf("_EventCommErrorExit seq=%d\n", *seq);
	switch( *seq ){
	case 0:
    if(GFL_NET_IsInit() )
    { 
      GFL_NET_Exit(NULL);
    }
    (*seq)++;
    break;
  case 1:
    if(GFL_NET_IsExit() == TRUE){
      NetErr_ErrWorkInit();
      GAMESYSTEM_SetFieldCommErrorReq(gsys, FALSE);
      (*seq)++;
    }
    break;
  case 2:
    if(GAMESYSTEM_CommBootAlways( gsys ) == TRUE){  //�Ăя펞�ʐM���N��
      (*seq)++;
    }
    else{
      return GMEVENT_RES_FINISH;
    }
    break;
  case 3:
    {
      GAME_BEACON_SYS_PTR gbs_ptr = GameCommSys_GetAppWork(game_comm);
      if ( gbs_ptr != NULL && GameCommSys_BootCheck( game_comm ) != GAME_COMM_NO_NULL ){
        GameBeacon_SetErrorWait(gbs_ptr);
        return GMEVENT_RES_FINISH;
      }
    }
    break;
	}
	
	return GMEVENT_RES_CONTINUE;
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
  GAMESYS_WORK *gsys = pwk;
  
  GAMESYSTEM_SetFieldCommErrorReq(gsys, FALSE);
  return GFL_PROC_RES_FINISH;
}
