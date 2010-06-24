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
  u32 large_rec[LARGE_REC_NUM]; //(70+1)x4 byte  = 284 byte
  u16 small_rec[SMALL_REC_NUM]; //    96x2 byte  = 192 byte
  //����͕K���Ō�
  RECORD_CRC crc;   //CRC & �Í����L�[   4 byte
};
#ifdef _NITRO
// �\���̂��z��̃T�C�Y�ƂȂ��Ă��邩�`�F�b�N
SDK_COMPILER_ASSERT(sizeof(RECORD) == 284+192+4); // LARGE_REC_NUMx4 + SMALL_REC_NUMx2 + 4(CRC)
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
  Record_Coded(rec, RECID_REPORT_COUNT);
  
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


#define RECODE_MAXTBL_MAX ( 6 )
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
    RECID_WALK_COUNT_FLAG,      
    RECID_REPORT_COUNT_FLAG,    
    RECID_CLEAR_TIME_FLAG,      
    RECID_RIDE_CYCLE_FLAG,      
    RECID_BATTLE_COUNT_FLAG,    
    RECID_BTL_ENCOUNT_FLAG,     
    RECID_BTL_TRAINER_FLAG,     
    RECID_CAPTURE_POKE_FLAG,    
    RECID_FISHING_SUCCESS_FLAG, 
    RECID_TAMAGO_HATCHING_FLAG, 
    RECID_POKE_EVOLUTION_FLAG,  
    RECID_RECOVERY_PC_FLAG,     
    RECID_COMM_TRADE_FLAG,      
    RECID_COMM_BATTLE_FLAG,     
    RECID_COMM_BTL_WIN_FLAG,    
    RECID_COMM_BTL_LOSE_FLAG,   
    RECID_WIFI_TRADE_FLAG,      
    RECID_WIFI_BATTLE_FLAG,     
    RECID_WIFI_BTL_WIN_FLAG,    
    RECID_WIFI_BTL_LOSE_FLAG,   
    RECID_WIFI_BTL_DRAW_FLAG,   
    RECID_SHOPPING_CNT_FLAG,    
    RECID_SHOPPING_MONEY_FLAG,  
    RECID_WATCH_TV_FLAG,        
    RECID_SODATEYA_CNT_FLAG,    
    RECID_KILL_POKE_FLAG,       
    RECID_DAYMAX_EXP_FLAG,      
    RECID_DAYCNT_EXP_FLAG,      
    RECID_GTS_PUT_FLAG,         
    RECID_MAIL_WRITE_FLAG,      
    RECID_NICKNAME_FLAG,        
    RECID_PREMIUM_BALL_FLAG,    
    RECID_LEADERHOUSE_BATTLE_FLAG,
    RECID_WIN_BP_FLAG,          
    RECID_USE_BP_FLAG,          
    RECID_SURECHIGAI_NUM_FLAG,  
    RECID_IRC_TRADE_FLAG,       
    RECID_IRC_BATTLE_FLAG,      
    RECID_IRC_BTL_WIN_FLAG,     
    RECID_IRC_BTL_LOSE_FLAG,    
    RECID_PALACE_TIME_FLAG,     
    RECID_PALACE_MISSION_FLAG,  

    RECID_LARGE_42_FLAG,  
    RECID_LARGE_43_FLAG,  
    RECID_LARGE_44_FLAG,  
    RECID_LARGE_45_FLAG,  
    RECID_LARGE_46_FLAG,  
    RECID_LARGE_47_FLAG,  
    RECID_LARGE_48_FLAG,  
    RECID_LARGE_49_FLAG,  
    RECID_LARGE_50_FLAG,  
    RECID_LARGE_51_FLAG,  
    RECID_LARGE_52_FLAG,  
    RECID_LARGE_53_FLAG,  
    RECID_LARGE_54_FLAG,  
    RECID_LARGE_55_FLAG,  
    RECID_LARGE_56_FLAG,  
    RECID_LARGE_57_FLAG,  
    RECID_LARGE_58_FLAG,  
    RECID_LARGE_59_FLAG,  
    RECID_LARGE_60_FLAG,  
    RECID_LARGE_61_FLAG,  
    RECID_LARGE_62_FLAG,  
    RECID_LARGE_63_FLAG,  
    RECID_LARGE_64_FLAG,  
    RECID_LARGE_65_FLAG,  
    RECID_LARGE_66_FLAG,  
    RECID_LARGE_67_FLAG,  
    RECID_LARGE_68_FLAG,  
    RECID_LARGE_69_FLAG,  
    RECID_LARGE_70_FLAG,  

    RECID_DENDOU_CNT_FLAG,        
    RECID_RECOVERY_HOME_FLAG,     
    RECID_WAZA_HANERU_FLAG,       
    RECID_WAZA_WARUAGAKI_FLAG,    
    RECID_WAZA_MUKOU_FLAG,        
    RECID_TEMOTI_KIZETU_FLAG,     
    RECID_TEMOTI_MAKIZOE_FLAG,    
    RECID_NIGERU_SIPPAI_FLAG,     
    RECID_NIGERARETA_FLAG,        
    RECID_FISHING_FAILURE_FLAG,   
    RECID_DAYMAX_KILL_FLAG,       
    RECID_DAYCNT_KILL_FLAG,       
    RECID_DAYMAX_CAPTURE_FLAG,    
    RECID_DAYCNT_CAPTURE_FLAG,    
    RECID_DAYMAX_TRAINER_BATTLE_FLAG, 
    RECID_DAYCNT_TRAINER_BATTLE_FLAG, 
    RECID_DAYMAX_EVOLUTION_FLAG,  
    RECID_DAYCNT_EVOLUTION_FLAG,  
    RECID_FOSSIL_RESTORE_FLAG,    
    RECID_GURUGURU_COUNT_FLAG,    
    RECID_RANDOM_FREE_SINGLE_WIN_FLAG,  
    RECID_RANDOM_FREE_SINGLE_LOSE_FLAG, 
    RECID_RANDOM_FREE_DOUBLE_WIN_FLAG,  
    RECID_RANDOM_FREE_DOUBLE_LOSE_FLAG, 
    RECID_RANDOM_FREE_ROTATE_WIN_FLAG,  
    RECID_RANDOM_FREE_ROTATE_LOSE_FLAG, 
    RECID_RANDOM_FREE_TRIPLE_WIN_FLAG,  
    RECID_RANDOM_FREE_TRIPLE_LOSE_FLAG, 
    RECID_RANDOM_FREE_SHOOTER_WIN_FLAG, 
    RECID_RANDOM_FREE_SHOOTER_LOSE_FLAG,
    RECID_RANDOM_RATE_SINGLE_WIN_FLAG,  
    RECID_RANDOM_RATE_SINGLE_LOSE_FLAG, 
    RECID_RANDOM_RATE_DOUBLE_WIN_FLAG,  
    RECID_RANDOM_RATE_DOUBLE_LOSE_FLAG, 
    RECID_RANDOM_RATE_ROTATE_WIN_FLAG,  
    RECID_RANDOM_RATE_ROTATE_LOSE_FLAG, 
    RECID_RANDOM_RATE_TRIPLE_WIN_FLAG,  
    RECID_RANDOM_RATE_TRIPLE_LOSE_FLAG, 
    RECID_RANDOM_RATE_SHOOTER_WIN_FLAG, 
    RECID_RANDOM_RATE_SHOOTER_LOSE_FLAG,
    RECID_USE_SHOOTER_COUNT_FLAG,       
    RECID_EFFECT_ENCOUNT_NUM_FLAG,      
    RECID_AFFINITY_CHECK_NUM_FLAG,      
    RECID_MUSICAL_PLAY_NUM_FLAG,        
    RECID_MUSICAL_WINNER_NUM_FLAG,      
    RECID_MUSICAL_COMM_NUM_FLAG,        
    RECID_MUSICAL_COMM_WINNER_NUM_FLAG, 
    RECID_MUSICAL_TOTAL_POINT_FLAG,     
    RECID_PDW_SLEEP_POKEMON_FLAG,       
    RECID_POKESHIFTER_COUNT_FLAG,       
    RECID_TRIALHOUSE_COUNT_FLAG,        
    RECID_SURECHIGAI_THANKS_FLAG,       
    RECID_TRIALHOUSE_RANK_FLAG,         
    RECID_TRIALHOUSE_SCORE_FLAG,        

    RECID_SMALL_54_FLAG, 
    RECID_SMALL_55_FLAG, 
    RECID_SMALL_56_FLAG, 
    RECID_SMALL_57_FLAG, 
    RECID_SMALL_58_FLAG, 
    RECID_SMALL_59_FLAG, 
    RECID_SMALL_60_FLAG, 
    RECID_SMALL_61_FLAG, 
    RECID_SMALL_62_FLAG, 
    RECID_SMALL_63_FLAG, 
    RECID_SMALL_64_FLAG, 
    RECID_SMALL_65_FLAG, 
    RECID_SMALL_66_FLAG, 
    RECID_SMALL_67_FLAG, 
    RECID_SMALL_68_FLAG, 
    RECID_SMALL_69_FLAG, 
    RECID_SMALL_70_FLAG, 
    RECID_SMALL_71_FLAG, 
    RECID_SMALL_72_FLAG, 
    RECID_SMALL_73_FLAG, 
    RECID_SMALL_74_FLAG, 
    RECID_SMALL_75_FLAG, 
    RECID_SMALL_76_FLAG, 
    RECID_SMALL_77_FLAG, 
    RECID_SMALL_78_FLAG, 
    RECID_SMALL_79_FLAG, 
    RECID_SMALL_80_FLAG, 
    RECID_SMALL_81_FLAG, 
    RECID_SMALL_82_FLAG, 
    RECID_SMALL_83_FLAG, 
    RECID_SMALL_84_FLAG, 
    RECID_SMALL_85_FLAG, 
    RECID_SMALL_86_FLAG, 
    RECID_SMALL_87_FLAG, 
    RECID_SMALL_88_FLAG, 
    RECID_SMALL_89_FLAG, 
    RECID_SMALL_90_FLAG, 
    RECID_SMALL_91_FLAG, 
    RECID_SMALL_92_FLAG, 
    RECID_SMALL_93_FLAG, 
    RECID_SMALL_94_FLAG, 
    RECID_SMALL_95_FLAG, 
  };
  static const u32 max_tbl[]={
    999999999,
    999999,
    65535,
    9999,
    999,
    6,
  };
  int index = limit[id];

  // id����`�͈͂𒴂��Ă���
  if(index>=SMALL_REC_MAX){
    return 0;
  }
  // max_tbl�ň�������鐔�l�͈̔͂𒴂��Ă���
  if( index>= RECODE_MAXTBL_MAX){
    return 0;
  }

  // ����RECORD��`�ň�����ő�l
  return max_tbl[index];
  
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
 * @brief ���R�[�h���C���N�������g����
 * @param rec   RECORD�ւ̃|�C���^
 * @param id    ���ڂ��w�肷��ID
 * @param value �Z�b�g����l
 * @return  u32   ���̍��ڂ̒l
 */
//----------------------------------------------------------
u32 RECORD_Inc(RECORD * rec, int id)
{
  return RECORD_Add( rec, id, 1 );
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


// ���R�[�h�f�[�^1���X�V�����p�e�[�u��
static const u16 oneday_update_tbl[][2]={
  { RECID_DAYMAX_EXP,            RECID_DAYCNT_EXP },    // 1���ɉ҂����o���l�̍ő�l
  { RECID_DAYMAX_KILL,           RECID_DAYCNT_KILL},    // 1���ɓ|�����|�P�����̍ő吔
  { RECID_DAYMAX_CAPTURE,        RECID_DAYCNT_CAPTURE,},// 1���ɕߊl�����|�P�����̍ő吔
  { RECID_DAYMAX_TRAINER_BATTLE, RECID_DAYCNT_TRAINER_BATTLE }, // 1���Ƀg���[�i�[��������ő��
  { RECID_DAYMAX_EVOLUTION,      RECID_DAYCNT_EVOLUTION }, //1���Ƀ|�P�����i���������ő��
};

//=============================================================================================
/**
 * @brief 1���Ɉ��s���f�[�^�X�V����
 *
 * @param   rec   
 */
//=============================================================================================
void RECORD_1day_Update( RECORD *rec )
{
  int i;
  u32 ret;

  // ���R�[�h�f�[�^1���X�V����
  for(i=0;i<NELEMS(oneday_update_tbl);i++){
    // �ő�l�𒴂��Ă���ꍇ�͓o�^
    ret=RECORD_SetIfLarge( rec, oneday_update_tbl[i][0], 
                           RECORD_Get(rec, oneday_update_tbl[i][1]));
    // ����̒l�̓��Z�b�g
    RECORD_Set( rec, oneday_update_tbl[i][1], 0);
  }


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

  RECORD_Set(rec, RECID_REPORT_COUNT, score);
}


static const u16 debug_tbl[][2]={
  {    0,    1},
  { 1000, 1001},
  { 9999, 1000},
  { 5000, 4999},
  { 5000, 5000},
};

void RECORD_DebugSet( RECORD *rec )
{
  int i;
  for(i=0;i<NELEMS(oneday_update_tbl);i++){
    RECORD_Set(rec, oneday_update_tbl[i][0], debug_tbl[i][0]);
    RECORD_Set(rec, oneday_update_tbl[i][1], debug_tbl[i][1]);
  }
}


#endif  //PM_DEBUG


