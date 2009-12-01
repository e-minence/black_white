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

#include "pl_boat_def.h"

#define WHISTLE_MARGINE (45)

#define PL_BOAT_MODE_TIME (10*30)    //180�b

#define NPC_TOTAL_NUM (15)
#define ROOM_NUM  (14)
#define DOUBLE_ROOM_NUM  (6)
#define LEFT_ROOM_NUM (8)
#define RIGHT_ROOM_NUM (6)

typedef struct PL_BOAT_ROOM_PRM_tag
{
  int TrID;       //�g���[�i�[�̏ꍇ�̓g���[�i�[�h�c
  int CbjCode;    //������
  int NpcCode;    //���b�Z�[�W�Ƃ��m�o�b�^�C�v�Ƃ��̎��ʗp
}PL_BOAT_ROOM_PRM;

typedef struct PL_BOAT_WORK_tag
{
  BOOL WhistleReq;
  int Time;
  int WhistleCount;   //�D�J�Ȃ�����
  PL_BOAT_ROOM_PRM RoomParam[ROOM_NUM];
  u8 TrNumLeft;
  u8 TrNumRight;
  u8 TrNumSingle;
  u8 TrNumDouble;
}PL_BOAT_WORK;

typedef struct ENTRY_WORK_tag
{
  u8 RoomIdx[ROOM_NUM];
  u8 RoomEntry[ROOM_NUM];
  u8 EntryCount;
  u8 LastIdx;
}ENTRY_WORK;


typedef enum {
  PL_BOAT_EVT_NONE,
  PL_BOAT_EVT_MSG,
  PL_BOAT_EVT_WHISTLE,
  PL_BOAT_EVT_END,
}PL_BOAT_EVT;

typedef enum {
  ROOM_SIDE_LEFT,
  ROOM_SIDE_RIGHT,
}ROOM_SIDE;

typedef enum {
  ROOM_TYPE_SINGLE,
  ROOM_TYPE_DOUBLE,
}ROOM_TYPE;

//�j�����Ƃ̃g���[�i�[���i���`���j
static const u8 WeekTrNum[7] = {
  4,3,4,5,4,6,7
};

//�������
typedef struct ROOM_ST_tag
{
  ROOM_SIDE Side;    //LEFT or RIGHT
  ROOM_TYPE Type;    //SINGLE or DOUBLE
}ROOM_ST;

static const ROOM_ST RoomSt[ROOM_NUM] = {
  {ROOM_SIDE_LEFT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_DOUBLE},

  {ROOM_SIDE_LEFT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_SINGLE},
};

//�_�u���^�����C���f�b�N�X�Q
static const u8 DoubleRoom[DOUBLE_ROOM_NUM] = {
  0,1,2,3,4,5
};

//�D�����̕����C���f�b�N�X�Q
static const u8 LeftRoom[LEFT_ROOM_NUM] = {
  0,1,2,3,6,7,8,9
};
//�D���E�̕����C���f�b�N�X�Q
static const u8 RightRoom[RIGHT_ROOM_NUM] = {
  4,5,10,11,12,13
};

static PL_BOAT_EVT GetEvt(PL_BOAT_WORK_PTR work);


static void InitEntryWork(ENTRY_WORK *work);
static int SearchRoomIdx(ENTRY_WORK *work, int inRoomIdx);
static int GetEntryItem(ENTRY_WORK *work, const int inIdx);
static BOOL DelEntry(ENTRY_WORK *work, const int inIdx);
static BOOL EntryRoom(ENTRY_WORK *work);

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
  default:
    GF_ASSERT(0);
    num = 0;
  }
  return num;
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
static void EntryTrainer(PL_BOAT_WORK *work)
{
  int tr_num;
  RTCDate date;
  u8 i;
  ENTRY_WORK entry_work;
  RTC_GetDate( &date );

  //�����̃G���g�����[�N��������
  InitEntryWork(&entry_work);
  //�����̃G���g��
  {
    BOOL rc;
    rc = EntryRoom(&entry_work);
    //�s���̎��Ԃ̂��߂̑Ώ���
    if (!rc)  InitEntryWork(&entry_work);   //��������Ԃő��s
  }

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
  //�g���[�i�������܂�����A���ꂼ��̕����̐��𐔂���
  for (i=0;i<tr_num;i++)
  {
    int room_idx;
    room_idx = entry_work.RoomEntry[i];
    if (RoomSt[room_idx].Side == ROOM_SIDE_LEFT) work->TrNumLeft++;
    else work->TrNumRight++;
    if (RoomSt[room_idx].Type == ROOM_TYPE_SINGLE) work->TrNumSingle++;
    else work->TrNumDouble++;
  }
 
}

//�G���g�����[�N�������֐�
static void InitEntryWork(ENTRY_WORK *work)
{
  int i;
  work->LastIdx = ROOM_NUM-1;
  work->EntryCount = 0;
  //�����C���f�b�N�X������
  for (i=0;i<ROOM_NUM;i++)
  {
    work->RoomIdx[i] = i;
    work->RoomEntry[i] = 0;
  }
}

//�e�[�u���̒��g���������Ďw��C���f�b�N�X�ꏊ��Ԃ�
static int SearchRoomIdx(ENTRY_WORK *work, const int inRoomIdx)
{
  int i;
  for (i=0;i<ROOM_NUM;i++){
    if (work->RoomIdx[i] == inRoomIdx) return i;
  }
  GF_ASSERT(0);
  return -1;
}

//�w��C���f�b�N�X�̗v�f���擾
static int GetEntryItem(ENTRY_WORK *work, const int inIdx)
{
  //���I�Ō���C���f�b�N�X�𒴂��Ă�����A�T�[�g
  if (inIdx >= work->LastIdx)
  {
    GF_ASSERT(0);
    return -1;
  }
  return work->RoomIdx[inIdx];
}

//���I�Ώۂ���w��C���f�b�N�X�̗v�f���͂���
static BOOL DelEntry(ENTRY_WORK *work, const int inIdx)
{
  //���I�Ō���C���f�b�N�X�𒴂��Ă�����A�T�[�g
  if (inIdx >= work->LastIdx)
  {
    GF_ASSERT(0);
    return FALSE;
  }

  if ( inIdx < 0 )
  {
    GF_ASSERT(0);
    return FALSE;
  }

  //���I�ΏۍŌ���̗v�f�ŏ㏑������
  work->RoomIdx[inIdx] = work->RoomIdx[work->LastIdx];
  
  //���ɍŌ����0�̂Ƃ��͗�O����
  if ( work->LastIdx == 0 )
  {
    GF_ASSERT(0);
    return FALSE;
  }

  //�Ō���C���f�b�N�X���f�N�������g
  work->LastIdx--;

  return TRUE;
}

//�����̃G���g�����Ԍ���
static BOOL EntryRoom(ENTRY_WORK *work)
{
  int i;
  int idx, room_idx, del_idx;
  BOOL rc;
  //�_�u���^�̕�����1�I��
  idx = GFUser_GetPublicRand(DOUBLE_ROOM_NUM);
  room_idx = DoubleRoom[idx];
  del_idx = SearchRoomIdx(work, room_idx);
  //�G���g���Ώۂ��璊�I�����C���f�b�N�b�X������
  rc = DelEntry(work, del_idx);
  if (!rc) return FALSE;
  //�G���g���e�[�u���Ɋi�[
  work->RoomEntry[work->EntryCount] = room_idx;
  work->EntryCount++;

  //�n�߂ɑI�񂾕������D���̍��E�ǂ���ɂ��邩�Ŏ��̕����̌����i��
  if ( RoomSt[idx].Side == ROOM_SIDE_LEFT ){    //���̂Ƃ�
    //�E�̕������璊�I����
    idx = GFUser_GetPublicRand(RIGHT_ROOM_NUM);
    room_idx = RightRoom[idx];
  }else{    //�E�̂Ƃ�
    //���̕������璊�I����
    idx = GFUser_GetPublicRand(LEFT_ROOM_NUM);
    room_idx = RightRoom[idx];
  }
  del_idx = SearchRoomIdx(work, room_idx);
  //�G���g���Ώۂ��璊�I�����C���f�b�N�b�X������
  rc = DelEntry(work, del_idx);
  if (!rc) return FALSE;
  //�G���g���e�[�u���Ɋi�[
  work->RoomEntry[work->EntryCount] = room_idx;
  work->EntryCount++;

  //�c��̕������G���g��
  for (i=0;i<ROOM_NUM-2;i++)
  {
    int denominator = (ROOM_NUM-2)-i;
    idx = GFUser_GetPublicRand(denominator);
    room_idx = GetEntryItem(work, idx);
    if (room_idx < 0) return FALSE;   //��O���N�����ꍇ
    rc = DelEntry(work, idx);
    if (!rc) return FALSE;
    //�G���g���e�[�u���Ɋi�[
    work->RoomEntry[work->EntryCount] = room_idx;
    work->EntryCount++;
  }

  return TRUE;
}

