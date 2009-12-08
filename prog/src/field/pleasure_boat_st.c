//======================================================================
/**
 * @file  pleasure_boat_st.c
 * @brief   �V���D�풓��
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "gamesystem/game_event.h"
#include "pleasure_boat.h"
#include "pleasure_boat_def.h"

#include "arc/fieldmap/fldmmdl_objcode.h"  //for OBJCODE

#include "demo\demo3d.h"

typedef struct DEMO_EVT_WORK_tag
{
  DEMO3D_PARAM Param;
  PL_BOAT_WORK_PTR Ptr;
  u16 *RetWk;
}DEMO_EVT_WORK;


static GMEVENT_RESULT DemoCallEvt( GMEVENT* event, int* seq, void* work );


//--------------------------------------------------------------
/**
 * @brief�@�����̐l�������擾�@�n�a�i�R�[�h
 * @note    �]�[���`�F���W���ɃX�N���v�g����R�[�������
 * @param   work      PL_BOAT_WORK_PTR
 * @apram   inRoomIdx �����ԍ�
 * @retval  int       �n�a�i�R�[�h
*/
//--------------------------------------------------------------
int PL_BOAT_GetObjCode(PL_BOAT_WORK_PTR work, const int inRoomIdx)
{
  if ( work==NULL ) return BOY1;
  return work->RoomParam[inRoomIdx].ObjCode;
}

//--------------------------------------------------------------
/**
 * @brief�@�����̐l�����_�u���o�g�����邩���`�F�b�N
 * @param   work      PL_BOAT_WORK_PTR
 * @apram   inRoomIdx �����ԍ�
 * @retval  BOOL       TRUE�Ń_�u���o�g��
*/
//--------------------------------------------------------------
BOOL PL_BOAT_CheckDblBtl(PL_BOAT_WORK_PTR work, const int inRoomIdx)
{
  BOOL rc = FALSE;

  if ( work==NULL ) return rc;

  if ( (inRoomIdx == 2)&&(work->DblBtl[0]) ) rc = TRUE;
  else if ( (inRoomIdx == 7)&&(work->DblBtl[1]) ) rc = TRUE;
    
  return rc;
}

//--------------------------------------------------------------
/**
 * @brief   �b�f���R�[���C�x���g�쐬
 * @param   work      PL_BOAT_WORK_PTR
 * @retval  event     �C�x���g�|�C���^
*/
//--------------------------------------------------------------
GMEVENT * PL_BOAT_CreateDemoEvt(GAMESYS_WORK *gsys, PL_BOAT_WORK_PTR work, u16 *ret_wk)
{
  DEMO_EVT_WORK *evt_work;
  GMEVENT *event;
  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, DemoCallEvt, sizeof(DEMO_EVT_WORK) );
  
  evt_work = GMEVENT_GetEventWork(event);
  evt_work->Ptr = work;
  evt_work->RetWk = ret_wk;
  //���݂̑D���o�ߎ��Ԃ��擾���ăf���p�����[�^�ɃZ�b�g
  evt_work->Param.start_frame = PL_BOAT_GetTime(work);
  OS_Printf("start_frame = %d\n",evt_work->Param.start_frame);
  //�f���h�c�Z�b�g
  evt_work->Param.demo_id = 0;

  return event;
}

//--------------------------------------------------------------
/**
 * @brief   �b�f���R�[���C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
*/
//--------------------------------------------------------------
static GMEVENT_RESULT DemoCallEvt( GMEVENT* event, int* seq, void* work )
{
  DEMO_EVT_WORK *evt_work;
  evt_work = GMEVENT_GetEventWork(event);

  switch(*seq){
  case 0:
    //�f���v���b�N
    GMEVENT_CallProc( event, FS_OVERLAY_ID(demo3d), &Demo3DProcData, &evt_work->Param );
    (*seq)++;
    break;
  case 1:
    //�f������f�[�^�擾
    if ( evt_work->Param.result == DEMO3D_RESULT_FINISH ) *(evt_work->RetWk) = 1;
    else *(evt_work->RetWk) = 0;

    //�o�ߎ��ԃZ�b�g
    PL_BOAT_SetTime(evt_work->Ptr, evt_work->Param.end_frame);

    OS_Printf("end_frame = %d\n",evt_work->Param.end_frame);
    //�C�x���g�I��
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}



