//======================================================================
/*
 * @file	event_battle.c
 * @brief	�C�x���g�F�t�B�[���h�G���J�E���g�G�t�F�N�g
 * @author kagaya
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "event_encount_effect.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// ENCEFF_WORK
//--------------------------------------------------------------
typedef struct
{
  int count;
  FIELDMAP_WORK *fieldWork;
}ENCEFF_WORK;

//======================================================================
//  proto
//======================================================================
static GMEVENT_RESULT ev_encEffectFunc( GMEVENT *event, int *seq, void *wk );

//======================================================================
//  �C�x���g�@�G���J�E���g�G�t�F�N�g
//======================================================================
//--------------------------------------------------------------
/**
 * �C�x���g�����@�G���J�E���g�G�t�F�N�g
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_FieldEncountEffect(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GMEVENT *event;
  ENCEFF_WORK *work;
  
  event = GMEVENT_Create( gsys, NULL, ev_encEffectFunc, sizeof(ENCEFF_WORK) );
  
  work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(ENCEFF_WORK) );
  
  work->fieldWork = fieldWork;
  return( event );
}

//--------------------------------------------------------------
/**
 * �C�x���g�@�G���J�E���g�G�t�F�N�g
 * @param event GMEVENT
 * @param seq �C�x���g�V�[�P���X
 * @param wk �C�x���g���[�N
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ev_encEffectFunc( GMEVENT *event, int *seq, void *wk )
{
  ENCEFF_WORK *work = wk;
  
  switch( (*seq) )
  {
  case 0:
    GFL_FADE_SetMasterBrightReq(
        GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0, 16, -1 );
    (*seq)++;
  case 1:
    if( GFL_FADE_CheckFade() == FALSE ){
      GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 16, 0, -1 );
      (*seq)++;
    }
    break;
  case 2:
    if( GFL_FADE_CheckFade() == FALSE ){
      work->count++;
      
      if( work->count < 3 ){
        GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0, 16, -1 );
        (*seq) = 1;
      }else{
        #if 0
        GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, -1 );
        (*seq)++;
        #else //�P�x�ύX��Ή� �o�g�����
        return( GMEVENT_RES_FINISH );
        #endif
      }
    }
    break;
  case 3:
    if( GFL_FADE_CheckFade() == FALSE ){
      return( GMEVENT_RES_FINISH );
    }
    break;
  }
  
  return( GMEVENT_RES_CONTINUE );
}
