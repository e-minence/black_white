//======================================================================
/**
 * @file  pleasure_boat.c
 * @brief �V���D
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "pleasure_boat.h"
#include "system/main.h"  //for HEAPID_APP_CONTROL
#include "../../../resource/fldmapdata/script/c03r0801_def.h"  //for SCRID_�`
#include "script.h"     //for SCRIPT_SetEventScript

#define WHISTLE_MARGINE (45)

#define PL_BOAT_MODE_TIME (10*30)    //180�b

#define NPC_TOTAL_NUM (15)

typedef struct PL_BOAT_WORK_tag
{
  BOOL WhistleReq;
  int Time;
  int WhistleCount;   //�D�J�Ȃ�����
}PL_BOAT_WORK;

typedef enum {
  PL_BOAT_EVT_NONE,
  PL_BOAT_EVT_MSG,
  PL_BOAT_EVT_WHISTLE,
  PL_BOAT_EVT_END,
}PL_BOAT_EVT;

static const u8 WeekTrNum[7] = {
  4,3,4,5,4,6,7
};

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
  //�C�x���g���ԃe�[�u�������[�h���邩��
  ;
  return work;
}

//--------------------------------------------------------------
/**
 * @brief	�I���֐�
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
 * @brief	����֐�
 * @param	work      PL_BOAT_WORK_PTR
 * @retval		none
*/
//--------------------------------------------------------------
void PL_BOAT_Main(PL_BOAT_WORK_PTR work)
{
  if ( work == NULL ) return;

  //�D�J�̃��N�G�X�g���������Ă���ꍇ�͂r�d�Đ�
  if (work->WhistleReq)
  {
    ;
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

  //���Ԍo��
  work->Time++;

  //�C�x���g�������Ԃ����`�F�b�N����
  {
    BOOL rc;  //�C�x���g�����t���O�Ƃ��Ďg�p
    evt_type = GetEvt(work);
    switch(evt_type){
    case PL_BOAT_EVT_MSG:
      //�D���A�i�E���X�p�X�N���v�g�R�[��
      ;
      rc = TRUE;
      break;
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
 * @retval  none
*/
//--------------------------------------------------------------
void PL_BOAT_AddTimeEvt(PL_BOAT_WORK_PTR work, const int inAddSec)
{
  int margine = work->Time%(WHISTLE_MARGINE*30);
  OS_Printf("now_time = %d::%d SEC\n",work->Time, work->Time/30);
  //���ԉ��Z
  margine += (inAddSec*30);
  if ( margine >= WHISTLE_MARGINE*30 )
  {
    //�D�J���钼�O�܂Ŏ��Ԃ�i�s
    work->Time = (work->WhistleCount+1)*(WHISTLE_MARGINE*30)-1;
  }
  else
  {
    work->Time += (inAddSec*30);
  }
  OS_Printf("add_after_time = %d::%d SEC\n",work->Time, work->Time/30);
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
  if ( work->Time >= PL_BOAT_MODE_TIME ){
    return PL_BOAT_EVT_END;
  }

  if ( work->Time % (WHISTLE_MARGINE*30) == 0 ){
    return PL_BOAT_EVT_WHISTLE;
  }
  return PL_BOAT_EVT_NONE;
}

//--------------------------------------------------------------
/**
 * @brief	�g���[�i�[���I
 * @param	 work      PL_BOAT_WORK_PTR
 * @retval none
*/
//--------------------------------------------------------------
static void EntryTrainer(void)
{
  int tr_num;
  RTCDate date;
  RTC_GetDate( &date ); 
  //�j���ʂɃg���[�i�[��������
  switch( date.week ){
  case RTC_WEEK_MONDAY:
    tr_num = WeekTrNum[0];
    break;
  case RTC_WEEK_TUESDAY:
    tr_num = WeekTrNum[1];
    break;
  case RTC_WEEK_WEDNESDAY:
    tr_num = WeekTrNum[2];
    break;
  case RTC_WEEK_THURSDAY:
    tr_num = WeekTrNum[3];
    break;
  case RTC_WEEK_FRIDAY:
    tr_num = WeekTrNum[4];
    break;
  case RTC_WEEK_SATURDAY:
    tr_num = WeekTrNum[5];
    break;
  case RTC_WEEK_SUNDAY:
    tr_num = WeekTrNum[6];
    break;
  default:
    GF_ASSERT(0);
    tr_num = 0;
  }


  //�e�[�u������g���[�i���G���g��
  ;
  //�e�[�u�������g���[�i�[���G���g��
  ;
 
}

