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

//#define FASE_MAX    (5)
//#define FIRST_FASE_TIME (10)
//#define FASE_TIME (45)

#define PL_BOAT_MODE_TIME (10*30)    //180�b

typedef struct PL_BOAT_WORK_tag
{
//  int NowFase;
//  int FaseRestTime;
  BOOL WhistleReq;
//  s64 BaseSec;

  int Time;
}PL_BOAT_WORK;

typedef enum {
  PL_BOAT_EVT_NONE,
  PL_BOAT_EVT_MSG,
  PL_BOAT_EVT_WHISTLE,
  PL_BOAT_EVT_END,
}PL_BOAT_EVT;

#if 0
static int GetDiffTime(PL_BOAT_WORK_PTR work);
static int GetNowFaseTime(PL_BOAT_WORK_PTR work);
#endif

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

//  work->NowFase = 0;
//  work->FaseRestTime = FIRST_FASE_TIME;
  work->WhistleReq = FALSE;
//  work->BaseSec = GFL_RTC_GetDateTimeBySecond();

  work->Time = 0;

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

#if 0
u32 PL_BOAT_GetMsgCntIdx()
{
  //���ݎ��Ԃ���擾�ł��郁�b�Z�[�W�̊Ǘ��ԍ���Ԃ�
  ;
}
#endif

#if 0
//--------------------------------------------------------------
/**
 * @brief	���Ԍo�߃t���b�V��
 * @param	work      PL_BOAT_WORK_PTR
 * @retval	none
*/
//--------------------------------------------------------------
void PL_BOAT_FlushTime(PL_BOAT_WORK_PTR work)
{
  int diff;

  //���݂܂ł̏���ԎZ�o
  diff = GetDiffTime(work);

  if (diff < work->FaseRestTime)
  {
    work->FaseRestTime -= diff;
  }
  else
  {
    work->FaseRestTime = 0;
  }
  //���ԏ㏑��
  work->BaseSec = GFL_RTC_GetDateTimeBySecond();
}

//--------------------------------------------------------------
/**
 * @brief	�o�ߎ��Ԏ擾
 * @param	work      PL_BOAT_WORK_PTR
 * @retval	int     �o�ߎ��ԁi���ԍ����j
*/
//--------------------------------------------------------------
static int GetDiffTime(PL_BOAT_WORK_PTR work)
{
  s64 sec;
  int diff;
  int fase_time;
  
  fase_time = GetNowFaseTime(work);
  sec = GFL_RTC_GetDateTimeBySecond();
  OS_Printf("sec = %d\n",sec);
  //�����v�Z
  diff = work->BaseSec - sec;
  //��c������͂��肦�Ȃ����O�̂���
  if (diff < 0){
    GF_ASSERT(0);
    diff = fase_time;
  }

  if (diff > fase_time) diff = fase_time;

  return diff;
}

//--------------------------------------------------------------
/**
 * @brief	���݂̃t�F�[�Y�̍ő�o�ߕb��
 * @param	work      PL_BOAT_WORK_PTR
 * @retval	int     �t�F�[�Y�̍ő�b��
*/
//--------------------------------------------------------------
static int GetNowFaseTime(PL_BOAT_WORK_PTR work)
{
  int fase_time;
  //���݂̃t�F�[�Y�ŕ���
  if (work->NowFase == 0) fase_time = FIRST_FASE_TIME;
  else fase_time = FASE_TIME;

  return fase_time;
}
#endif

static PL_BOAT_EVT GetEvt(PL_BOAT_WORK_PTR work)
{
  //���ݎ��ԂŃC�x���g���������邩�𒲂ׂ�
  if ( work->Time >= PL_BOAT_MODE_TIME ){
    return PL_BOAT_EVT_END;
  }

  if ( work->Time % (36*30) == 0 ){
    return PL_BOAT_EVT_WHISTLE;
  }
  return PL_BOAT_EVT_NONE;
}
