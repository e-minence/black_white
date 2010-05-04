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

#include "pl_boat_def.h"
#include "demo\demo3d.h"
#include "demo\demo3d_demoid.h"   //for DEMO3D_ID_�`

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

#if 0
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
#endif

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
  DEMO3D_PARAM_SetFromRTC( &evt_work->Param, gsys, DEMO3D_ID_C_CRUISER, 0);
  evt_work->Param.start_frame = PL_BOAT_GetTime( work );
  NOZOMU_Printf("start_frame = %d\n",evt_work->Param.start_frame);

  return event;
}

//--------------------------------------------------------------
/**
 * @brief	�I���֐�
 * @note  �S�ŏ�������Ă΂��ꍇ������̂ŁA�풓�̈�ɂ���
 * @param	work      PL_BOAT_WORK_PTR
 * @retval		none
*/
//--------------------------------------------------------------
void PL_BOAT_End(PL_BOAT_WORK_PTR *work)
{
  if ( *work != NULL )
  {
    GFL_HEAP_FreeMemory( *work );
    *work = NULL;
  }
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
    if ( evt_work->Param.result == DEMO3D_RESULT_FINISH )
      *(evt_work->RetWk) = PL_BOAT_DEMO_FINISHED;
    else *(evt_work->RetWk) = PL_BOAT_DEMO_USER_END;

    //�o�ߎ��ԃZ�b�g
    PL_BOAT_SetTime(evt_work->Ptr, evt_work->Param.end_frame);

    NOZOMU_Printf("end_frame = %d\n",evt_work->Param.end_frame);
    //�C�x���g�I��
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief�@���Ԏ擾
 * @note    �A�v���R�[������Ƃ��ɌĂ�
 * @param   work      PL_BOAT_WORK_PTR
 * @retval  int       �o�ߎ���(�V���N)
*/
//--------------------------------------------------------------
int PL_BOAT_GetTime(PL_BOAT_WORK_PTR work)
{
  return work->Time;
}

//--------------------------------------------------------------
/**
 * @brief�@���ԃZ�b�g �D�J�̉񐔂��v�Z����
 * @note    �A�v������߂��Ă���Ƃ��ɌĂ�
 * @param   work      PL_BOAT_WORK_PTR
 * @param   inTime    �o�ߎ���(�V���N)
 * @retval  none
*/
//--------------------------------------------------------------
void PL_BOAT_SetTime(PL_BOAT_WORK_PTR work, const int inTime)
{
  int w_count;
  work->Time = inTime;
  //�D�J�񐔂��v�Z
  w_count = work->Time / (WHISTLE_MARGINE*30);

  if ( work->WhistleCount > w_count )
  {
    GF_ASSERT_MSG(0,"whistle count error %d > %d",work->WhistleCount, w_count);
  }
  else
  {
    work->WhistleCount = w_count;
  }
}

