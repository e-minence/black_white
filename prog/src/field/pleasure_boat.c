//======================================================================
/**
 * @file  pleasure_boat.c
 * @brief �V���D
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "pleasure_boat.h"
#include "pleasure_boat_def.h"
#include "system/main.h"  //for HEAPID_APP_CONTROL
#include "../../../resource/fldmapdata/script/c03r0801_def.h"  //for SCRID_�`
#include "script.h"     //for SCRIPT_SetEventScript

#include "pl_boat_def.h"

#include "sound/pm_sndsys.h"

#include "pl_boat_setup.h"

#define PL_BOAT_MODE_TIME (180*30)    //180�b

#define WHISTLE_COUNT_MAX (4)

//SE
#define PL_BOAT_SE_WHISTLE  (SEQ_SE_FLD_78)   //�D�J

typedef enum {
  PL_BOAT_EVT_NONE,
  PL_BOAT_EVT_WHISTLE,
  PL_BOAT_EVT_END,
}PL_BOAT_EVT;

FS_EXTERN_OVERLAY(pl_boat_setup);

static PL_BOAT_EVT GetEvt(PL_BOAT_WORK_PTR work);

//--------------------------------------------------------------
/**
 * @brief	�������֐�
 * @param	void
 * @retval		PL_BOAT_WORK_PTR
 */
//--------------------------------------------------------------
PL_BOAT_WORK_PTR PL_BOAT_Init(void)
{
  PL_BOAT_WORK_PTR work;

  work = GFL_HEAP_AllocClearMemory(HEAPID_APP_CONTROL, sizeof(PL_BOAT_WORK));

  work->WhistleReq = FALSE;
  work->Time = 0;
  work->WhistleCount = 0;
  work->TrNumLeft = 0;
  work->TrNumRight = 0;
  work->TrNumSingle = 0;
  work->TrNumDouble = 0;
  work->GameEnd = FALSE;
  GFL_OVERLAY_Load( FS_OVERLAY_ID(pl_boat_setup) );
  PL_BOAT_SETUP_EntryTrainer(work);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(pl_boat_setup) );

  return work;
}

//--------------------------------------------------------------
/**
 * @brief	����֐�
 * @param	work      PL_BOAT_WORK_PTR
 * @retval		none
*/
//--------------------------------------------------------------
void PL_BOAT_Main(PL_BOAT_WORK_PTR work)
{
  if ( work == NULL ) return;

  if ( work->GameEnd ) return;

  //�D�J�̃��N�G�X�g���������Ă���ꍇ�͂r�d�Đ�
  if (work->WhistleReq)
  {
    PMSND_PlaySE(PL_BOAT_SE_WHISTLE);
    work->WhistleCount++;
    work->WhistleReq = FALSE;
  }
}

//--------------------------------------------------------------
/**
 * @brief	���Ԍo�߃`�F�b�N
 * @param	 work      PL_BOAT_WORK_PTR
 * @retval GMEVENT *event    �����C�x���g�|�C���^	
*/
//--------------------------------------------------------------
GMEVENT *PL_BOAT_CheckEvt(GAMESYS_WORK *gsys, PL_BOAT_WORK_PTR work)
{
  PL_BOAT_EVT evt_type;
  GMEVENT *event;

  if ( work == NULL ) return NULL;

  if ( work->GameEnd ) return NULL;

  //���Ԍo��
  work->Time++;

  //�C�x���g�������Ԃ����`�F�b�N����
  {
    BOOL rc;  //�C�x���g�����t���O�Ƃ��Ďg�p
    evt_type = GetEvt(work);
    switch(evt_type){
    case PL_BOAT_EVT_WHISTLE:
      //�D�J���N�G�X�g
      work->WhistleReq = TRUE;
      //�D�J�̓C�x���g�Ŗ炷�킯�ł͂Ȃ��̂Ń��^�[���R�[�h��FALSE��Ԃ�
      rc = FALSE;
      break;
    case PL_BOAT_EVT_END:
      //���Ԗ����@�I���X�N���v�g�R�[��
      event = SCRIPT_SetEventScript( gsys, SCRID_PRG_C03R0801_TIMEUP, NULL, GFL_HEAP_LOWID(HEAPID_FIELDMAP) );
      rc = TRUE;
      break;
    default:
      rc = FALSE;
    }
  
    if (rc) return event;
  }

  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief	�����I�Ɏ��Ԍo�߂�����@�D�J���Ԃ��܂������ꍇ�͋D�J���Ԓ��O�܂ł̐�����������
 * @param   work      PL_BOAT_WORK_PTR
 * @param   inAddSec  ���Z���ԁi�b�j
 * @param   inWhistle   ���Z���ʌ�A�D�J���Ȃ鎞�Ԃ̏ꍇ�A�D�J��炷���ǂ����@0�F�炳�Ȃ��@1�F�炷
 * @retval  none
*/
//--------------------------------------------------------------
void PL_BOAT_AddTimeEvt(PL_BOAT_WORK_PTR work, const int inAddSec, const int inWhistle)
{
  int margine = work->Time%(WHISTLE_MARGINE*30);
  OS_Printf("now_time = %d::%d SEC\n",work->Time, work->Time/30);
  //���ԉ��Z
  margine += (inAddSec*30);
  if ( margine >= WHISTLE_MARGINE*30 )
  {
    if ( inWhistle )
    {
      //�D�J���钼�O�܂Ŏ��Ԃ�i�s
      work->Time = (work->WhistleCount+1)*(WHISTLE_MARGINE*30)-1;
    }
    else
    {
      //�D�J����������܂Ŏ��Ԃ�i�s
      work->Time = (work->WhistleCount+1)*(WHISTLE_MARGINE*30);
      work->WhistleCount++;
    }
  }
  else
  {
    work->Time += (inAddSec*30);
  }
  OS_Printf("add_after_time = %d::%d SEC\n",work->Time, work->Time/30);
}

//--------------------------------------------------------------
/**
 * @brief�@�g���[�i�[���̎擾
 * @param   work      PL_BOAT_WORK_PTR
 * @param   inSearchType    pl_boat_def.h�@�Q��
 * @retval  int �g���[�i�[��
*/
//--------------------------------------------------------------
int PL_BOAT_GetTrNum(PL_BOAT_WORK_PTR work, const int inSearchType)
{
  int num;
  switch(inSearchType){
  case PL_TR_SEARCH_TYPE_LEFT:
    num = work->TrNumLeft;
    break;
  case PL_TR_SEARCH_TYPE_RIGHT:
    num = work->TrNumRight;
    break;
  case PL_TR_SEARCH_TYPE_SINGLE:
    num = work->TrNumSingle;
    break;
  case PL_TR_SEARCH_TYPE_DOUBLE:
    num = work->TrNumDouble;
    break;
  case PL_TR_SEARCH_TYPE_TOTAL: 
    num = work->TotalTrNum;
    break;
  case PL_TR_SEARCH_TYPE_WIN:
    {
      int i;
      num = 0;
      for(i=0;i<ROOM_NUM;i++)
      {
        if ( work->RoomParam[i].BattleEnd ) num++;
      }
    }
    break;
  default:
    GF_ASSERT(0);
    num = 0;
  }
  return num;
}

//--------------------------------------------------------------
/**
 * @brief�@�����̐l�������擾�@NPC�̎��
 * @param   work      PL_BOAT_WORK_PTR
 * @apram   inRoomIdx �����ԍ�
 * @retval  int       
*/
//--------------------------------------------------------------
int PL_BOAT_GetNpcType(PL_BOAT_WORK_PTR work, const int inRoomIdx)
{
  return work->RoomParam[inRoomIdx].NpcType;
}

//--------------------------------------------------------------
/**
 * @brief�@�����̐l�������擾�@�g���[�i�[�h�c
 * @param   work      PL_BOAT_WORK_PTR
 * @apram   inRoomIdx �����ԍ�
 * @retval  int       �g���[�i�[�h�c
*/
//--------------------------------------------------------------
int PL_BOAT_GetTrID(PL_BOAT_WORK_PTR work, const int inRoomIdx)
{
  return work->RoomParam[inRoomIdx].TrID;
}

//--------------------------------------------------------------
/**
 * @brief�@�����̐l�������擾�@���b�Z�[�W�h�c
 * @param   work      PL_BOAT_WORK_PTR
 * @apram   inRoomIdx �����ԍ�
 * @param   inMsgKind   ���b�Z�[�W��ށ@0:�ʏ�@1�F�ΐ��i�g���[�i�[�łȂ��m�o�b�̂Ƃ��͒ʏ�j
 * @retval  int       ���b�Z�[�W�h�c
*/
//--------------------------------------------------------------
int PL_BOAT_GetMsg(PL_BOAT_WORK_PTR work, const int inRoomIdx, const inMsgKind)
{
/**  
  if ( PL_BOAT_CheckDblBtl(work, inRoomIdx) )
  {
    int msg;
    if (inMsgKind == PL_BOAT_BTL_BEFORE_PAIR) msg = msg_c03r0801_babygirl2_01;
    else if ( inMsgKind == PL_BOAT_BTL_AFTER_PAIR ) msg = msg_c03r0801_babygirl2_02;
    else  msg = work->RoomParam[inRoomIdx].NpcMsg[inMsgKind];

    return msg;
  }else
  {
    return work->RoomParam[inRoomIdx].NpcMsg[inMsgKind];
  }
*/
  return work->RoomParam[inRoomIdx].NpcMsg[inMsgKind];
}

//--------------------------------------------------------------
/**
 * @brief�@�����̐l�����𑀍� �퓬�������`�F�b�N
 * @param   work      PL_BOAT_WORK_PTR
 * @apram   inRoomIdx �����ԍ�
 * @retval  BOOL       TRUE�Ő퓬����
*/
//--------------------------------------------------------------
BOOL PL_BOAT_CheckBtlFlg(PL_BOAT_WORK_PTR work, const int inRoomIdx)
{
  return work->RoomParam[inRoomIdx].BattleEnd;
}

//--------------------------------------------------------------
/**
 * @brief�@�����̐l�����𑀍� �퓬�������Ƃɂ���
 * @param   work      PL_BOAT_WORK_PTR
 * @apram   inRoomIdx �����ԍ�
 * @param   inFlg     TRUE�Ő퓬����
 * @retval  none
*/
//--------------------------------------------------------------
void PL_BOAT_SetBtlFlg(PL_BOAT_WORK_PTR work, const int inRoomIdx, const BOOL inFlg)
{
  work->RoomParam[inRoomIdx].BattleEnd = inFlg;
}

//--------------------------------------------------------------
/**
 * @brief	�C�x���g�擾
 * @param	 work      PL_BOAT_WORK_PTR
 * @retval PL_BOAT_EVT    �����C�x���g�^�C�v
*/
//--------------------------------------------------------------
static PL_BOAT_EVT GetEvt(PL_BOAT_WORK_PTR work)
{
  //���ݎ��ԂŃC�x���g���������邩�𒲂ׂ�
  if ( (work->Time >= PL_BOAT_MODE_TIME)&&(work->WhistleCount >= WHISTLE_COUNT_MAX) ){
    return PL_BOAT_EVT_END;
  }

  if ( work->Time % (WHISTLE_MARGINE*30) == 0 ){
    return PL_BOAT_EVT_WHISTLE;
  }
  return PL_BOAT_EVT_NONE;
}

//--------------------------------------------------------------
/**
 * @brief�@�Q�[���I��
 * @param   work      PL_BOAT_WORK_PTR
 * @retval  none
*/
//--------------------------------------------------------------
void PL_BOAT_EndGame(PL_BOAT_WORK_PTR work)
{
  work->GameEnd = TRUE;
}

