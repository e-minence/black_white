//============================================================================================
/**
 * @file  record.c
 * @brief �Q�[�����J�E���g�����֘A
 * @author  tamada
 * @date  2006.03.28
 */
//============================================================================================

#include <gflib.h>
#include "savedata/save_tbl.h"
#include "savedata/record.h"
#include "savedata/th_rank_def.h"


//============================================================================================
//============================================================================================
typedef struct{
  u16 crc16ccitt_hash;  ///<CRC
  u16 coded_number;   ///<�Í����L�[
}RECORD_CRC;

struct RECORD{
  u32 large_rec[LARGE_REC_NUM]; //(70+1)x4 byte = 284 byte
  u16 small_rec[SMALL_REC_NUM]; //(51+26)x2 byte = 154 byte
  u16 padding;
  //����͕K���Ō�
  RECORD_CRC crc;   //CRC & �Í����L�[   4 byte
};
#ifdef _NITRO
// �\���̂��z��̃T�C�Y�ƂȂ��Ă��邩�`�F�b�N
SDK_COMPILER_ASSERT(sizeof(RECORD) == 284+154+4+2); //+=�v���`�i�ǉ���
#endif

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void Record_Coded(RECORD *rec, int id);
static void Record_Decoded(RECORD *rec, int id);


//============================================================================================
//
//  �Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief ���[�N�T�C�Y�擾
 * @return  int   �T�C�Y�i�o�C�g�P�ʁj
 */
//----------------------------------------------------------
int RECORD_GetWorkSize(void)
{
  return sizeof(RECORD);
}

//----------------------------------------------------------
/**
 * @brief ���R�[�h������
 * @param rec   ���R�[�h�f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
void RECORD_Init(RECORD * rec)
{
  MI_CpuClear32(rec, sizeof(RECORD));
  
  rec->crc.coded_number = OS_GetVBlankCount() | (OS_GetVBlankCount() << 8);
  Record_Coded(rec, RECID_REPORT_COUNT_EX);
  
}

//----------------------------------------------------------
/**
 * @brief
 * @param sv
 * @return  RECORD�ւ̃|�C���^
 */
//----------------------------------------------------------
RECORD * SaveData_GetRecord(SAVE_CONTROL_WORK * sv)
{
  return SaveControl_DataPtrGet(sv, GMDATA_ID_RECORD);
}

//============================================================================================
//
//
//      �����֐�
//
//
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   ���R�[�h�Í���
 * @param   rec   
 */
//--------------------------------------------------------------
static void Record_Coded(RECORD *rec, int id)
{
  if(id == RECID_WALK_COUNT){
    return;   //�����J�E���^�͈Í������Ȃ�
  }
  
  //�`�F�b�N�T���쐬(CRC�ɂ������������ǃZ�[�u�f�[�^�̃|�C���^���Ȃ��̂�)
  rec->crc.crc16ccitt_hash //-sizeof(u32)�͕����J�E���^��
    = GFL_STD_CODED_CheckSum(&rec->large_rec[1], 
    sizeof(RECORD) - sizeof(RECORD_CRC) - sizeof(u32)) & 0xffff;

  GFL_STD_CODED_Coded(&rec->large_rec[1], sizeof(RECORD) - sizeof(RECORD_CRC) - sizeof(u32), 
    rec->crc.crc16ccitt_hash + (rec->crc.coded_number << 16));
}

//--------------------------------------------------------------
/**
 * @brief   ���R�[�h������
 * @param   rec   
 */
//--------------------------------------------------------------
static void Record_Decoded(RECORD *rec, int id)
{
  if(id == RECID_WALK_COUNT){
    return;   //�����J�E���^�͈Í������Ȃ�
  }
  
  //-sizeof(u32)�͕����J�E���^��
  GFL_STD_CODED_Decoded(&rec->large_rec[1], sizeof(RECORD) - sizeof(RECORD_CRC) - sizeof(u32), 
    rec->crc.crc16ccitt_hash + (rec->crc.coded_number << 16));
}

//----------------------------------------------------------
/**
 * @brief �l�̎擾
 * @param rec   RECORD�ւ̃|�C���^
 * @param id    ���R�[�h���ڎw��ID
 * @return  u32   ���݂̒l
 */
//----------------------------------------------------------
static u32 get_value(const RECORD * rec, int id)
{
  if (id < LARGE_REC_MAX) {
    return rec->large_rec[id];
  } else if (id < SMALL_REC_MAX) {
    return rec->small_rec[id - LARGE_REC_MAX];
  }
  GF_ASSERT_MSG(0, "record: id error\n");
  return 0;
}

//----------------------------------------------------------
/**
 * @brief �l���Z�b�g����
 * @param rec   RECORD�ւ̃|�C���^
 * @param id    ���R�[�h���ڎw��ID
 * @param value �Z�b�g����l
 * @return  u32   ���݂̒l
 */
//----------------------------------------------------------
static u32 set_value(RECORD * rec, int id, u32 value)
{
  if (id < LARGE_REC_MAX) {
    rec->large_rec[id] = value;
  } else if (id < SMALL_REC_MAX) {
    rec->small_rec[id - LARGE_REC_MAX] = value;
  } else {
    GF_ASSERT_MSG(0, "record: id error\n");
  }
  return get_value(rec, id);
}

//----------------------------------------------------------
/**
 * @brief ���R�[�h���ڂ��Ƃ̏�����擾����
 * @param id    ���R�[�h���ڎw��ID
 * @return  u32   ����̒l
 */
//----------------------------------------------------------
static u32 get_limit(int id)
{
  static u8 limit[SMALL_REC_MAX] = {
    1,  //RECID_WALK_COUNT
    //4byte
    1,0,1,1,1,1,1,0,0,0,1,1,1,1,1,  //1-16
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  //17-32
    1,1,1,1,1,1,1,1,0,0,0,  //33-43
    0,0,0,0,0,0,0,0,0,0,0,0,0,  //44-56 RECID_GTS_PUT�`
    1,1,1,1,1,1,1,1,1,1,1,1,1,0,  //57-70 RECID_LEADERHOUSE_BATTLE�`
    
    //2byte
    1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,  //44-59
    0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,  //60-75
    0,0,0,0,0,0,0,0,0,1,  //76-85
    0,0,0,0,0,0,0,      //RECID_GTS_SUCCESS�`RECID_GURUGURU_COUNT
  };

  if (id < LARGE_REC_MAX) {
    if(limit[id]){
      GF_ASSERT_MSG( 0, "record: max over id %d 99999999\n", id );
      return 999999999;
    }else{
      GF_ASSERT_MSG( 0, "record: max over id %d 999999\n", id );
      return 999999;
    }
  } else if (id < SMALL_REC_MAX) {
    if(limit[id]){
      return 0xffff;
    }else{
      GF_ASSERT_MSG( 0, "record: max over id %d 9999\n", id );
      return 9999;
    }
  }
  return 0;
}


//============================================================================================
//
//
//        ����֐�
//
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief ���R�[�h���Z�b�g����
 * @param rec   RECORD�ւ̃|�C���^
 * @param id    ���ڂ��w�肷��ID
 * @param value �Z�b�g����l
 * @return  u32   ���̍��ڂ̒l
 */
//----------------------------------------------------------
u32 RECORD_Set(RECORD * rec, int id, u32 value)
{
  u32 limit = get_limit(id);
    u32 ret;
  
  Record_Decoded(rec, id);
  
  if (value < limit) {
    ret = set_value(rec, id, value);
  } else {
    ret = set_value(rec, id, limit);
  }
  
  Record_Coded(rec, id);
    return ret;
}

//----------------------------------------------------------
/**
 * @brief �傫����΃��R�[�h���X�V����
 * @param rec   RECORD�ւ̃|�C���^
 * @param id    ���ڂ��w�肷��ID
 * @param value �Z�b�g����l
 * @return  u32   ���̍��ڂ̒l
 */
//----------------------------------------------------------
u32 RECORD_SetIfLarge(RECORD * rec, int id, u32 value)
{
  u32 limit = get_limit(id);
  u32 now;
    u32 ret;

  Record_Decoded(rec, id);

  now = get_value(rec, id);
  ret = now;
  
  if(value > limit){
    value = limit;  //����`�F�b�N
  }
  if (now < value) {
    ret = set_value(rec, id, value);
  } else {
    if(now > limit){
      ret = set_value(rec, id, limit);
    }
  }

  Record_Coded(rec, id);
    return ret;
}

//----------------------------------------------------------
/**
 * @brief ���R�[�h���C���N�������g����
 * @param rec   RECORD�ւ̃|�C���^
 * @param id    ���ڂ��w�肷��ID
 * @param value �Z�b�g����l
 * @return  u32   ���̍��ڂ̒l
 */
//----------------------------------------------------------
u32 RECORD_Inc(RECORD * rec, int id)
{
  u32 limit = get_limit(id);
  u32 now;
    u32 ret;
    
  Record_Decoded(rec, id);

    now = get_value(rec, id);
  if (now + 1 < limit) {
    ret = set_value(rec, id, now + 1);
  } else {
    ret =  set_value(rec, id, limit);
  }

  Record_Coded(rec, id);
    return ret;
}

//----------------------------------------------------------
/**
 * @brief ���R�[�h�ɒl��������
 * @param rec   RECORD�ւ̃|�C���^
 * @param id    ���ڂ��w�肷��ID
 * @param value ������l
 * @return  u32   ���̍��ڂ̒l
 */
//----------------------------------------------------------
u32 RECORD_Add(RECORD * rec, int id, u32 add_value)
{
  u32 limit = get_limit(id);
  u32 now;
    u32 ret;
    
  Record_Decoded(rec, id);

    now = get_value(rec, id);
  if (now + add_value < limit) {
    ret = set_value(rec, id, now + add_value);
  } else {
    ret = set_value(rec, id, limit);
  }

  Record_Coded(rec, id);
    return ret;
}

//----------------------------------------------------------
/**
 * @brief ���R�[�h���擾����
 * @param rec   RECORD�ւ̃|�C���^
 * @param id    ���ڂ��w�肷��ID
 * @return  u32   ���̍��ڂ̒l
 */
//----------------------------------------------------------
u32 RECORD_Get(RECORD * rec, int id)
{
  u32 limit = get_limit(id);
  u32 value;

  Record_Decoded(rec, id);
  
  value = get_value(rec, id);

  Record_Coded(rec, id);

  if(value > limit){
    return limit;
  }else{
    return value;
  }
}

//----------------------------------------------------------
/**
 * @brief �X�R�A�̉��Z( WB�ł͑��݂��Ȃ��j
 * @param rec   RECORD�ւ̃|�C���^
 * @param id    �X�R�A�w���ID
 */
//----------------------------------------------------------
void RECORD_Score_Add(RECORD * rec, int id)
{

  return ;

#if 0
  u32 score;
  
  GF_ASSERT(id < SCOREID_MAX);

  //�J���X�g�`�F�b�N����
  score = RECORD_Get(rec, RECID_SCORE);
  if ( score+get_score_number(id) > SCORE_MAX ){
    RECORD_Set(rec, RECID_SCORE, SCORE_MAX);
  }else{
    RECORD_Add(rec, RECID_SCORE, get_score_number(id));
  }
#endif
}



//----------------------------------------------------------
/**
 * @brief �����N�̃Z�b�g(�㏑���`�F�b�N����)
 * @param rec     RECORD�ւ̃|�C���^
 * @param inRank  �����N th_rank_def.h �Q��
 * @return  none
 */
//----------------------------------------------------------
void RECORD_SetThRank(RECORD * rec, u8 inRank)
{
  
  if (inRank < TH_RANK_NUM)
  {
    RECORD_SetIfLarge( rec, RECID_TRIALHOUSE_RANK, inRank );
  }
  else GF_ASSERT_MSG( 0,"rank error %d", inRank );
}

//----------------------------------------------------------
/**
 * @brief �����N�̎擾
 * @param rec   RECORD�ւ̃|�C���^
 * @return  u8    ���݂̃����N    th_rank_def.h �Q��
 */
//----------------------------------------------------------
u8 RECORD_GetThRank(RECORD * rec)
{
  return RECORD_Get( rec, RECID_TRIALHOUSE_RANK );
//  return rec->Rank;
}

//----------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�̃X�R�A�̃Z�b�g(�㏑���`�F�b�N����)
 * @param rec     RECORD�ւ̃|�C���^
 * @param inScore
 * @return  none
 */
//----------------------------------------------------------
void RECORD_SetThScore(RECORD * rec, u32 inScore)
{
  if (inScore < TH_SCORE_MAX)
  {
    RECORD_SetIfLarge( rec, RECID_TRIALHOUSE_SCORE, inScore );
  }
  else GF_ASSERT_MSG( 0,"th score over %d", inScore );
}

//----------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�̃X�R�A�̎擾
 * @param rec     RECORD�ւ̃|�C���^
 * @return  u32 �X�R�A
 */
//----------------------------------------------------------
u32 RECORD_GetThScore(RECORD * rec)
{
  return RECORD_Get( rec, RECID_TRIALHOUSE_SCORE);
//  return rec->TrialScore;
}

#ifdef PM_DEBUG
//----------------------------------------------------------
/**
 * @brief �X�R�A�̃Z�b�g
 * @param rec   RECORD�ւ̃|�C���^
 * @param inScore �Z�b�g����X�R�A
 */
//----------------------------------------------------------
void RECORD_Score_DebugSet(RECORD * rec, const u32 inScore)
{
  u32 score;

  score = inScore;
  
  //�J���X�g�`�F�b�N����
  if ( score > SCORE_MAX ){
    score = SCORE_MAX;
  }

  RECORD_Set(rec, RECID_REPORT_COUNT_EX, score);
}

#endif  //PM_DEBUG


