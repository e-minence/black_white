//======================================================================
/**
 * @file  united_nations_st.c
 * @brief   ���A�풓��
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "arc/fieldmap/fldmmdl_objcode.h"  //for OBJCODE
#include "savedata/un_savedata.h"
#include "savedata/un_savedata_local.h"
#include "un_roominfo_def.h"
#include "united_nations.h"

#include "gamesystem/game_event.h" //for GMEVENT_RESULT

#include "app/un_select.h"  //for overlay and UN_SELECT_PARAM 

typedef struct UN_COUNT_SEL_WORK_tag
{
  UN_SELECT_PARAM Param;
  u16 *OutFloor;
  u16 *CountryCode;
  u16 *Ret;
}UN_COUNT_SEL_WORK;

static GMEVENT_RESULT CallAppEvt( GMEVENT* event, int* seq, void* work );

//--------------------------------------------------------------
/**
 * @brief�@�����̐l�������擾�@�n�a�i�R�[�h
 * @note    �]�[���`�F���W���ɃX�N���v�g����R�[�������
 * @param   work      ���[�N�|�C���^
 * @apram   inObjIdx �C���f�b�N�X�ԍ� 0�`UN_ROOM_OBJ_MAX
 * @retval  u32       �n�a�i�R�[�h
*/
//--------------------------------------------------------------
u32 UN_GetRoomObjCode(UNSV_WORK* work, const u32 inObjIdx)
{
  if (inObjIdx >= work->ObjNum)
  {
    if (inObjIdx >= UN_ROOM_OBJ_MAX)
    {
      GF_ASSERT_MSG(0,"idx error idx=%d",inObjIdx);
      return BOY2;
    }
    //������ȓ��ł���΁A�\���͂��Ȃ��͂������A�萔��Ԃ��Ă���
    return BOY2;
  }
  
  return work->UnObjCode[inObjIdx];
}

//--------------------------------------------------------------
/**
 * @brief�@�������擾
 * @note  �@�]�[���`�F���W���ɃX�N���v�g����R�[���ŕ����̂��q�̐����擾����̂ŏ풓�ɒu��
 * @note    �擾�ł�����́A�q���A�����̍��R�[�h�A�t���A
 * @param   work      ���[�N�|�C���^
 * @param   inType    �擾���^�C�v  un_roominfo_def.h �Q��
 * @retval  u32       �擾����
*/
//--------------------------------------------------------------
u32 UN_GetRoomInfo(UNSV_WORK* work, const u32 inType)
{
  u32 val;
  switch(inType){
  case UN_ROOMINFO_TYPE_OBJNUM:
    val = work->ObjNum;
    break;
  case UN_ROOMINFO_TYPE_COUNTRY:
    val = work->TargetCountryCode;
    break;
  case UN_ROOMINFO_TYPE_FLOOR:
    val = work->Floor;
    break;
  default:
    GF_ASSERT_MSG(0,"info code error %d",inType);
    val = 0;
  }
  return val;
}

//--------------------------------------------------------------
/**
 * @brief�@���I���A�v���Ăяo��
 * @param   gsys      �Q�[���V�X�e���|�C���^
 * @param   inFloor     ���݃t���A�@�G���g�����X�ɂ���Ƃ���1�ȉ����w��
 * @param   outFloor    �t���A�i�[�o�b�t�@
 * @param   outCountry  ���R�[�h�i�[�o�b�t�@
 * @param   ret_wk      ���ʊi�[
 * @retval  GMEVENT   �C�x���g�|�C���^
*/
//--------------------------------------------------------------
GMEVENT * UN_CreateAppEvt(GAMESYS_WORK *gsys, u16 inFloor, u16 *outFloor, u16 *outCountry, u16 *ret_wk)
{
  UN_COUNT_SEL_WORK *evt_work;
  GMEVENT *event;
  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, CallAppEvt, sizeof(UN_COUNT_SEL_WORK) );
  
  evt_work = GMEVENT_GetEventWork(event);
  evt_work->Param.InFloor = inFloor;

  return event;
}

//--------------------------------------------------------------
/**
 * @brief   ���I���A�v���R�[���C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
*/
//--------------------------------------------------------------
static GMEVENT_RESULT CallAppEvt( GMEVENT* event, int* seq, void* work )
{
  UN_COUNT_SEL_WORK *evt_work;
  evt_work = GMEVENT_GetEventWork(event);

  switch(*seq){
  case 0:
    //�f���v���b�N
    GMEVENT_CallProc( event, FS_OVERLAY_ID(un_select), &UNSelectProcData, &evt_work->Param );
    (*seq)++;
    break;
  case 1:
    //�f������f�[�^�擾
    if ( evt_work->Param.Decide == TRUE )
    {
      //���肵��
      *(evt_work->Ret) = TRUE;
      *(evt_work->OutFloor) = evt_work->Param.OutFloor;
      *(evt_work->CountryCode) = evt_work->Param.CountryCode;
    }
    else
    {
      //�L�����Z������
      *(evt_work->Ret) = FALSE;
      *(evt_work->OutFloor) = 0;
      *(evt_work->CountryCode) = 0;
    }

    //�C�x���g�I��
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}


