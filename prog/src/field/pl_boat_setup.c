//======================================================================
/**
 * @file  pl_boat_setup.c
 * @brief �V���D
 * @author  Saito
 */
//======================================================================

#include <gflib.h>
#include "pleasure_boat.h"
#include "pleasure_boat_def.h"
#include "pl_boat_def.h"
#include "pl_boat_setup.h"

#include "arc/fieldmap/fldmmdl_objcode.h"  //for OBJCODE
#include "msg/script/msg_c03r0801.h"
#include "tr_tool/trno_def.h"

#include "system/gfl_use.h"


#define DOUBLE_ROOM_NUM  (6)
#define LEFT_ROOM_NUM (8)
#define RIGHT_ROOM_NUM (7)

#define TRAINER_MAX     (10)
#define SHORT_TALKER_MAX     (15)
#define LONG_TALKER_MAX     (5)

#define ENTRY_TR_MAX    (7)
#define ENTRY_LONG_TALKER_MAX (2)
#define ENTRY_SHORT_TALKER_MAX (10)

typedef struct ENTRY_WORK_tag
{
  u8 RoomIdx[ROOM_NUM];
  u8 RoomEntry[ROOM_NUM];
  u8 EntryCount;
  u8 LastIdx;
  u8 Trainer[ENTRY_TR_MAX];
  u8 LongTalker[ENTRY_LONG_TALKER_MAX];
  u8 ShortTalker[ENTRY_SHORT_TALKER_MAX];
//  BOOL DblBtl[DBL_BTL_ROOM_NUM];
}ENTRY_WORK;

typedef struct TR_DATA_tag
{
  int ObjCode;
  int TrID;
  int Msg[2];
}TR_DATA;

typedef struct NONE_TR_DATA_tag
{
  int ObjCode;
  int Msg[2];
}NONE_TR_DATA;

typedef enum {
  ROOM_SIDE_LEFT,
  ROOM_SIDE_RIGHT,
}ROOM_SIDE;

typedef enum {
  ROOM_TYPE_SINGLE,
  ROOM_TYPE_DOUBLE,
}ROOM_TYPE;

//�������
typedef struct ROOM_ST_tag
{
  ROOM_SIDE Side;    //LEFT or RIGHT
  ROOM_TYPE Type;    //SINGLE or DOUBLE
}ROOM_ST;

static const ROOM_ST RoomSt[ROOM_NUM] = 
{
  {ROOM_SIDE_LEFT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_SINGLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_SINGLE},

  {ROOM_SIDE_LEFT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_LEFT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_DOUBLE},
  {ROOM_SIDE_RIGHT, ROOM_TYPE_DOUBLE},
};

//�j�����Ƃ̃g���[�i�[���i���`���j
static const u8 WeekTrNum[7] = {
  4,3,4,5,4,6,7
};

//�_�u���^�����C���f�b�N�X�Q
static const u8 DoubleRoom[DOUBLE_ROOM_NUM] = {
  9,10,11,12,13,14
};

//�D�����̕����C���f�b�N�X�Q
static const u8 LeftRoom[LEFT_ROOM_NUM] = {
  0,1,2,3,9,10,11,12
};
//�D���E�̕����C���f�b�N�X�Q
static const u8 RightRoom[RIGHT_ROOM_NUM] = {
  4,5,6,7,8,13,14
};

#include "../../../resource/pl_boat/pl_boat_data.cdat"

static void InitEntryWork(ENTRY_WORK *work);
static int SearchRoomIdx(ENTRY_WORK *work, int inRoomIdx);
static int GetEntryItem(ENTRY_WORK *work, const int inIdx);
static BOOL DelEntry(ENTRY_WORK *work, const int inIdx);
static BOOL EntryRoom(ENTRY_WORK *work);
static void CreateTrTbl(const int inTblLen, const int inNum, u8 *workTbl, u8 *outTbl);

//--------------------------------------------------------------
/**
 * @brief	�g���[�i�[���I
 * @param	 work      PL_BOAT_WORK_PTR
 * @retval none
*/
//--------------------------------------------------------------
void PL_BOAT_SETUP_EntryTrainer(PL_BOAT_WORK *work)
{
  int tr_num;
  RTCDate date;
  u8 i;
  ENTRY_WORK entry_work;

  u8 tr_tbl[TRAINER_MAX];
  u8 long_tbl[LONG_TALKER_MAX];
  u8 short_tbl[SHORT_TALKER_MAX];

  GFL_RTC_GetDate( &date );

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

  work->TotalTrNum = tr_num;

  OS_Printf("TrainerNum = %d\n",tr_num);

  //�����̃G���g�����[�N��������
  InitEntryWork(&entry_work);
  //�����̃G���g��
  {
    BOOL rc;
    rc = EntryRoom(&entry_work);
    //�s���̎��Ԃ̂��߂̑Ώ���
    if (!rc)  InitEntryWork(&entry_work);   //��������Ԃő��s
  }

  //�e�[�u������g���[�i���G���g��
  CreateTrTbl(
      TRAINER_MAX, tr_num, tr_tbl, entry_work.Trainer
      );
/**  
  //�_�u���o�g���𒊑I����ꍇ�A�g���[�i�[�e�[�u���̐擪�̂��̂��㏑��
  if(1){//@todo
    entry_work.Trainer[0] = TRAINER_MAX-1;
  }
*/  
   //�e�[�u�������g���[�i�[�i���b�j���G���g��
  CreateTrTbl(
      LONG_TALKER_MAX, ENTRY_LONG_TALKER_MAX, long_tbl, entry_work.LongTalker
      );
  //�e�[�u�������g���[�i�[�i�ʏ�j���G���g��
  CreateTrTbl(
      SHORT_TALKER_MAX, ROOM_NUM-(tr_num+ENTRY_LONG_TALKER_MAX), short_tbl, entry_work.ShortTalker
      );
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

  {
    int count = 0;
/**    
    //�_�u���o�g�������Z�b�g
    for (i=0;i<DBL_BTL_ROOM_NUM;i++)
    {
      work->DblBtl[i] = entry_work.DblBtl[i];
    }
*/    
    //�����Ƀg���[�i�[���i�[
    for (i=0;i<tr_num;i++)
    {
      u8 room_idx = entry_work.RoomEntry[count++];
      u8 entry_id = entry_work.Trainer[i];
      work->RoomParam[room_idx].EntryID = entry_id;
      work->RoomParam[room_idx].TrID = TrData[entry_id].TrID;
      work->RoomParam[room_idx].ObjCode = TrData[entry_id].ObjCode;
      work->RoomParam[room_idx].NpcType = NPC_TYPE_TRAINER;
      work->RoomParam[room_idx].NpcMsg[0] = TrData[entry_id].Msg[0];
      work->RoomParam[room_idx].NpcMsg[1] = TrData[entry_id].Msg[1];
    }
    //�����ɔ�g���[�i�[�i���b�j���i�[
    for(i=0;i<ENTRY_LONG_TALKER_MAX;i++)
    {
      u8 room_idx = entry_work.RoomEntry[count++];
      u8 entry_id = entry_work.LongTalker[i];
      work->RoomParam[room_idx].EntryID = i;
      work->RoomParam[room_idx].TrID = 0;
      work->RoomParam[room_idx].ObjCode = LongTalkerData[entry_id].ObjCode;
      work->RoomParam[room_idx].NpcType = NPC_TYPE_LONG_TALKER;
      work->RoomParam[room_idx].NpcMsg[0] = LongTalkerData[entry_id].Msg[0];
      work->RoomParam[room_idx].NpcMsg[1] = LongTalkerData[entry_id].Msg[1];
    }
    //�����ɔ�g���[�i�[�i�ʏ�j���i�[
    for(i=0;i<ROOM_NUM-(tr_num+ENTRY_LONG_TALKER_MAX);i++)
    {
      u8 room_idx = entry_work.RoomEntry[count++];
      u8 entry_id = entry_work.ShortTalker[i];
      work->RoomParam[room_idx].EntryID = i;
      work->RoomParam[room_idx].TrID = 0;
      work->RoomParam[room_idx].ObjCode = ShortTalkerData[entry_id].ObjCode;
      work->RoomParam[room_idx].NpcType = NPC_TYPE_SHORT_TALKER;
      work->RoomParam[room_idx].NpcMsg[0] = ShortTalkerData[entry_id].Msg[0];
      work->RoomParam[room_idx].NpcMsg[1] = ShortTalkerData[entry_id].Msg[1];
    }
  }

  //�_���v
  for (i=0;i<ROOM_NUM;i++)
  {
    //u8 room_idx = entry_work.RoomEntry[i];
    u8 room_idx = i;
    OS_Printf("room %d\n",room_idx);
    OS_Printf("OBJCODE %d\n",work->RoomParam[room_idx].ObjCode);
    OS_Printf("NPC TYPE %d\n",work->RoomParam[room_idx].NpcType);
    OS_Printf("ENTRY_ID %d\n",work->RoomParam[room_idx].EntryID);
    OS_Printf("MSG %d,%d\n",
        work->RoomParam[room_idx].NpcMsg[0],work->RoomParam[room_idx].NpcMsg[1]);
  }
 
}

//�����_���e�[�u���쐬
static void CreateTrTbl(const int inTblLen, const int inNum, u8 *workTbl, u8 *outTbl)
{
  int num;
  u8 i;

  for (i=0;i<inTblLen;i++)
  {
    workTbl[i] = i;
  }

  num = inNum;
  for (i=0;i<inNum;i++)
  {
    u8 idx;
    idx = GFUser_GetPublicRand(num);
    outTbl[i] = workTbl[idx];
    //�g�p�����f�[�^�𖖔��f�[�^�ŏ㏑��
    workTbl[idx] = workTbl[num-1];
    num--;
  }
}

//�G���g�����[�N�������֐�
static void InitEntryWork(ENTRY_WORK *work)
{
  int i;
  work->LastIdx = ROOM_NUM-1;
  work->EntryCount = 0;
/**  
  //�_�u���o�g�������N���A
  for (i=0;i<DBL_BTL_ROOM_NUM;i++)
  {
    work->DblBtl[i] = FALSE;
  }
*/
  //�����C���f�b�N�X������
  for (i=0;i<ROOM_NUM;i++)
  {
    work->RoomIdx[i] = i;
    work->RoomEntry[i] = 0;
  }
  //�g���[�i�[�e�[�u��������
  for(i=0;i<ENTRY_TR_MAX;i++)
  {
    work->Trainer[i] = i;
  }
  //��g���[�i�[(���b)�e�[�u��������
  for(i=0;i<ENTRY_LONG_TALKER_MAX;i++)
  {
    work->LongTalker[i] = i;
  }
  //��g���[�i�[�e�[�u��������
  for(i=0;i<ENTRY_SHORT_TALKER_MAX;i++)
  {
    work->ShortTalker[i] = i;
  }
}

//�e�[�u���̒��g���������Ďw��C���f�b�N�X�ꏊ��Ԃ�
static int SearchRoomIdx(ENTRY_WORK *work, const int inRoomIdx)
{
  int i;
  for (i=0;i<ROOM_NUM;i++){
    if (work->RoomIdx[i] == inRoomIdx) return i;
  }
  GF_ASSERT_MSG(0,"roomidx=%d",inRoomIdx);
  return -1;
}

//�w��C���f�b�N�X�̗v�f���擾
static int GetEntryItem(ENTRY_WORK *work, const int inIdx)
{
  //���I�Ō���C���f�b�N�X�𒴂��Ă�����A�T�[�g
  if (inIdx > work->LastIdx)
  {
    GF_ASSERT_MSG(0,"idx=%d  last=%d\n",inIdx, work->LastIdx);
    return -1;
  }
  return work->RoomIdx[inIdx];
}

//���I�Ώۂ���w��C���f�b�N�X�̗v�f���͂���
static BOOL DelEntry(ENTRY_WORK *work, const int inIdx)
{
  //���I�Ō���C���f�b�N�X�𒴂��Ă�����A�T�[�g
  if (inIdx > work->LastIdx)
  {
    GF_ASSERT_MSG(0,"idx=%d  last=%d\n",inIdx, work->LastIdx);
    return FALSE;
  }

  if ( inIdx < 0 )
  {
    GF_ASSERT(0);
    return FALSE;
  }

  //���I�ΏۍŌ���̗v�f�ŏ㏑������
  work->RoomIdx[inIdx] = work->RoomIdx[work->LastIdx];
  
  //�Ō���C���f�b�N�X���f�N�������g
  if ( work->LastIdx > 0 ) work->LastIdx--;

  return TRUE;
}

//�����̃G���g�����Ԍ���
static BOOL EntryRoom(ENTRY_WORK *work)
{
  int i;
  int idx, room_idx, del_idx;
  int entry_room_num;
  BOOL rc;
  BOOL left,right;
  left = FALSE;
  right = FALSE;

  entry_room_num = ROOM_NUM;
/**
  //�_�u���o�g���ł��镔����I��
  if(1)    //�_�u���o�g�����I���� @todo
  {
    if ( GFUser_GetPublicRand(2) ){
      room_idx = 2; //�O����
      left = TRUE;
      work->DblBtl[0] = TRUE;
    }else{
      room_idx = 7; //������
      right = TRUE;
      work->DblBtl[1] = TRUE;
    }
    del_idx = SearchRoomIdx(work, room_idx);
    //�G���g���Ώۂ��璊�I�����C���f�b�N�b�X������
    rc = DelEntry(work, del_idx);
    if (!rc) return FALSE;
    //�G���g���e�[�u���Ɋi�[
    work->RoomEntry[work->EntryCount] = room_idx;
    work->EntryCount++;

    entry_room_num--;   //���I�����f�N�������g
  }
*/
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

  entry_room_num--;   //���I�����f�N�������g

  if (RoomSt[idx].Side == ROOM_SIDE_LEFT) left = TRUE;
  else right = TRUE;

  //�����܂ł̒��I�ŁA���T�C�h�̕��������I����Ă��Ȃ��ꍇ�́A���I����Ă��Ȃ���������I��
  if ( (!right) || (!left) )
  {
    if ( right == FALSE )    //�E���܂����I����ĂȂ�
    {
      //�E�̕������璊�I����
      idx = GFUser_GetPublicRand(RIGHT_ROOM_NUM);
      room_idx = RightRoom[idx];
    }else    //�����܂����I����ĂȂ�
    {
      //���̕������璊�I����
      idx = GFUser_GetPublicRand(LEFT_ROOM_NUM);
      room_idx = LeftRoom[idx];
    }
    del_idx = SearchRoomIdx(work, room_idx);
    //�G���g���Ώۂ��璊�I�����C���f�b�N�b�X������
    rc = DelEntry(work, del_idx);
    if (!rc) return FALSE;
    //�G���g���e�[�u���Ɋi�[
    work->RoomEntry[work->EntryCount] = room_idx;
    work->EntryCount++;

    entry_room_num--;   //���I�����f�N�������g
  }

  //�c��̕������G���g��
  for (i=0;i<entry_room_num;i++)
  {
    int denominator = entry_room_num-i;
    idx = GFUser_GetPublicRand(denominator);
    room_idx = GetEntryItem(work, idx);
    OS_Printf("idx = %d  room = %d\n",idx, room_idx);
    if (room_idx < 0) return FALSE;   //��O���N�����ꍇ
    rc = DelEntry(work, idx);
    if (!rc) return FALSE;
    //�G���g���e�[�u���Ɋi�[
    work->RoomEntry[work->EntryCount] = room_idx;
    work->EntryCount++;
  }

  return TRUE;
}


