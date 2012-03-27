//==============================================================================
/**
 * @file    g_power.c
 * @brief   G�p���[
 * @author  matsuda
 * @date    2010.02.07(��)
 */
//==============================================================================
#include <gflib.h>
#include "field/gpower_id.h"
#include "gamesystem/g_power.h"
#include "waza_tool/wazano_def.h"
#include "arc_def.h"
#include "power.naix"
#include "poke_tool/status_rcv.h"


//==============================================================================
//  �萔��`
//==============================================================================
///1�b�̃t���[����
#define ONE_SECOND_FRAME    (60)
///1���̃t���[����
#define ONE_MINUTE_FRAME    (ONE_SECOND_FRAME * 60)

#define GPOWER_DISTRIBUTION_POWER_NUM_BW (GPOWER_ID_DISTRIBUTION_END-GPOWER_ID_DISTRIBUTION_START+1) //BW�̔z�z�p���[�̐�

//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct{
  u32 life[GPOWER_TYPE_MAX];        ///<��������
  u8 occur_power[GPOWER_TYPE_MAX];  ///<��������G�p���[ID(TYPE���ɔ�������ID�������Ă���)
  u16 powerdata_data[GPOWER_TYPE_MAX];  ///<��������G�p���[��POWER_CONV_DATA.data
  u8 my_power_type;                 ///<�������������Ă���p���[�̃^�C�v
  u8 padding[3];
}GPOWER_SYSTEM;


//==============================================================================
//  ���[�J���ϐ�
//==============================================================================
static GPOWER_SYSTEM GPowerSys = {0};


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _OccurPowerClear(GPOWER_TYPE type);
static void _ErrorCheck(void);



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * G�p���[�V�X�e��������
 *
 * @param   none		
 */
//==================================================================
void GPower_SYSTEM_Init(void)
{
  GFL_STD_MemClear(&GPowerSys, sizeof(GPOWER_SYSTEM));
  GPOWER_Clear_AllPower();
}

//==================================================================
/**
 * G�p���[�V�X�e���X�V����
 *
 * @param   none		
 */
//==================================================================
void GPOWER_SYSTEM_Update(void)
{
  GPOWER_TYPE type;
  
  for(type = 0; type < GPOWER_TYPE_MAX; type++){
    if(GPowerSys.life[type] > 1){
      GPowerSys.life[type]--;
    }
  }
  _ErrorCheck();
}

//==================================================================
/**
 * ��������G�p���[��S�ăN���A
 */
//==================================================================
void GPOWER_Clear_AllPower(void)
{
  GPOWER_TYPE type;
  
  for(type = 0; type < GPOWER_TYPE_MAX; type++){
    _OccurPowerClear(type);
  }
  GPowerSys.my_power_type = GPOWER_TYPE_NULL;
}

//==================================================================
/**
 * �s��ID���`�F�b�N
 * GPOWER_IsEnableID()�Ƃ̈Ⴂ�� GPOWER_ID_NULL�����e���邩�ǂ���
 *
 * IsErrorID()�ł� NULL �����e����
 */
//==================================================================
BOOL GPOWER_IsErrorID( GPOWER_ID gpower_id )
{
  //GPOWER_ID_NULL == GPOWER_ID_DUMMY_END
  if( gpower_id == GPOWER_ID_NULL ) return FALSE; //NULL�͂��肦��l�Ȃ̂�OK

  if( ( gpower_id >= GPOWER_ID_MAX ) ||
      ( gpower_id >= GPOWER_ID_DUMMY_START && gpower_id < GPOWER_ID_DUMMY_END ) )
  {
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �����\�ȗL��ID���`�F�b�N
 */
//==================================================================
BOOL GPOWER_IsEnableID( GPOWER_ID gpower_id )
{
  //GPOWER_ID_NULL == GPOWER_ID_DUMMY_END
  if( ( gpower_id >= GPOWER_ID_MAX ) ||
      ( gpower_id >= GPOWER_ID_DUMMY_START && gpower_id <= GPOWER_ID_DUMMY_END ) )
  {
    return FALSE;
  }
  return TRUE;
}


//==================================================================
/**
 * BW�ł������\�ȗL��ID���`�F�b�N
 */
//==================================================================
BOOL GPOWER_IsEnableID_BW( GPOWER_ID gpower_id )
{
  if( GPOWER_IsEnableID( gpower_id ) == FALSE ){
    return FALSE;
  }
  return ( gpower_id < GPOWER_ID_DUMMY_START );
}

//==================================================================
/**
 * �z�z�p���[�Ɋ���U��ꂽ�A��ID���擾
 *
 * �z�z�p���[�łȂ��ꍇ GPOWER_DISTRIBUTION_ID_NULL ��Ԃ�
 */
//==================================================================
u8 GPOWER_GetDistributionPowerID( GPOWER_ID gpower_id )
{
  if( gpower_id >= GPOWER_ID_DISTRIBUTION_START && gpower_id <= GPOWER_ID_DISTRIBUTION_END ){
    return gpower_id - GPOWER_ID_DISTRIBUTION_START;
  }
  /*
   *  111128�Ɉ�xSWAN�ɂ͔z�z�p���[�͖������ƂɂȂ�����
   *  ����ς��肽���I�ƌ���ꂽ���̂��߂�indef�ɂ�镪��Ń\�[�X���c���Ă���
   *
   *  �Ǘ��\��SWAN�z�z�p���[���ݒ肳��Ȃ���΁AGPOWER_ID_DISTRIBUTION_START_SW�͒�`����Ȃ�
   *  by iwasawa
   */
#ifdef GPOWER_ID_DISTRIBUTION_START_SW
  if( gpower_id >= GPOWER_ID_DISTRIBUTION_START_SW && gpower_id <= GPOWER_ID_DISTRIBUTION_END_SW ){
    return ((gpower_id - GPOWER_ID_DISTRIBUTION_START)+GPOWER_DISTRIBUTION_POWER_NUM_BW );
  }
#endif
  return GPOWER_DISTRIBUTION_ID_NULL;
}

//==================================================================
/**
 * ��������G�p���[ID���Z�b�g
 *
 * @param   gpower_id		
 * @param   powerdata   �p���[�f�[�^�ւ̃|�C���^
 * @param   my_power    TRUE:�����̃p���[�ł���@FALSE:���l�̃p���[�ł���
 */
//==================================================================
void GPOWER_Set_OccurID(GPOWER_ID gpower_id, const POWER_CONV_DATA *powerdata, BOOL my_power)
{
  GPOWER_TYPE type;
  
  if( GPOWER_IsEnableID( gpower_id ) == FALSE ){
    GF_ASSERT_MSG(0, "gpower_id = %d", gpower_id);
    return;
  }
  type = powerdata[gpower_id].type;
  GPowerSys.occur_power[type] = gpower_id;
  GPowerSys.life[type] = powerdata[gpower_id].time * ONE_MINUTE_FRAME;
  GPowerSys.powerdata_data[type] = powerdata[gpower_id].data;
  if(GPowerSys.my_power_type == type){
    GPowerSys.my_power_type = GPOWER_TYPE_NULL;
  }
  if(my_power == TRUE){
    GPowerSys.my_power_type = type;
  }
  
  OS_TPrintf("GPower Set id=%d\n", gpower_id);
  
  //�����ɔ������Ȃ��p���[�̃`�F�b�N
  switch(type){
  case GPOWER_TYPE_ENCOUNT_UP:
    _OccurPowerClear(GPOWER_TYPE_ENCOUNT_DOWN);
    break;
  case GPOWER_TYPE_ENCOUNT_DOWN:
    _OccurPowerClear(GPOWER_TYPE_ENCOUNT_UP);
    break;
  case GPOWER_TYPE_EXP_UP:
    _OccurPowerClear(GPOWER_TYPE_EXP_DOWN);
    break;
  case GPOWER_TYPE_EXP_DOWN:
    _OccurPowerClear(GPOWER_TYPE_EXP_UP);
    break;
  }
}

//==================================================================
/**
 * �w�肵��G�p���[ID���������Ă��邩�`�F�b�N
 *
 * @param   gpower_id		G�p���[ID
 * @param   powerdata		�p���[�f�[�^�ւ̃|�C���^
 *
 * @retval  GPOWER_ID		GPOWER_ID_NULL���߂��Ă����ꍇ�F�p���[�͔������Ă��Ȃ�
 * @retval  GPOWER_ID		�w��ID�Ɠ���ID���߂��Ă����ꍇ�F�w��ID�̃p���[���������Ă���
 * @retval  GPOWER_ID		�w��ID�ƈႤID���߂��Ă����ꍇ�F�w��ID�Ɠ����^�C�v�̕ʃp���[���������Ă���
 */
//==================================================================
GPOWER_ID GPOWER_Check_OccurID(GPOWER_ID gpower_id, const POWER_CONV_DATA *powerdata)
{
  return GPowerSys.occur_power[powerdata[gpower_id].type];
}

//==================================================================
/**
 * �w�肵���^�C�v�̃p���[���������Ă��邩�`�F�b�N
 *
 * @param   type		    G�p���[�^�C�v
 *
 * @retval  GPOWER_ID		�������Ă���G�p���[ID (�������Ă��Ȃ��ꍇ��GPOWER_ID_NULL)
 */
//==================================================================
GPOWER_ID GPOWER_Check_OccurType(GPOWER_TYPE type)
{
  return GPowerSys.occur_power[type];
}

//==================================================================
/**
 * �w�肵���^�C�v�̃p���[���������Ă��邩�`�F�b�N
 *
 * @param   type		    G�p���[�^�C�v
 *
 * @retval  u32		0:�������Ă��Ȃ� 1�ȏ�:�c�胉�C�t(�b)
 */
//==================================================================
u32 GPOWER_Check_OccurTypeLife( GPOWER_TYPE type )
{
  u32 sec;
  if( GPowerSys.occur_power[type] == GPOWER_ID_NULL ){
    return 0;
  }
  sec = GPowerSys.life[type]/ONE_SECOND_FRAME;
  if( sec == 0){
    return 1; //1�b�ȉ���1�b�ƕԂ����Ƃɂ���
  }
  return sec;
}

//==================================================================
/**
 * �����̃p���[���������Ă��邩�`�F�b�N
 *
 * @retval  u32		0:�������Ă��Ȃ� 1�ȏ�:�c�胉�C�t(�b)
 */
//==================================================================
u32 GPOWER_Check_MyPower(void)
{
  u32 sec;
  if(GPowerSys.my_power_type == GPOWER_TYPE_NULL){
    return 0;
  }
  sec = GPowerSys.life[GPowerSys.my_power_type]/ONE_SECOND_FRAME;
  if( sec == 0){
    return 1; //1�b�ȉ���1�b�ƕԂ����Ƃɂ���
  }
  return sec;
}

//==================================================================
/**
 * Finish�҂��ɂȂ��Ă���G�p���[ID���擾����
 *
 * @retval  GPOWER_ID		G�p���[ID(Finish�҂���1�������ꍇ��GPOWER_ID_NULL)
 */
//==================================================================
GPOWER_ID GPOWER_Get_FinishWaitID(void)
{
  GPOWER_TYPE type;
  
  for(type = 0; type < GPOWER_TYPE_MAX; type++){
    if(GPowerSys.life[type] == 1){
      return GPowerSys.occur_power[type];
    }
  }
  
  return GPOWER_ID_NULL;
}

//==================================================================
/**
 * Finish�҂���G�p���[ID��Finish������
 *
 * @param   gpower_id		
 * @param   powerdata		
 */
//==================================================================
void GPOWER_Set_Finish(GPOWER_ID gpower_id, const POWER_CONV_DATA *powerdata)
{
  GF_ASSERT(GPowerSys.life[powerdata[gpower_id].type] == 1);
  _OccurPowerClear(powerdata[gpower_id].type);
}

//--------------------------------------------------------------
/**
 * ��������G�p���[���^�C�v�w�肵�ăN���A����
 * @param   type		G�p���[�^�C�v
 */
//--------------------------------------------------------------
static void _OccurPowerClear(GPOWER_TYPE type)
{
  if(type >= GPOWER_TYPE_MAX){
    GF_ASSERT_MSG(0, "type=%d\n", type);
    return;
  }
  GPowerSys.occur_power[type] = GPOWER_ID_NULL;
  GPowerSys.life[type] = 0;
  GPowerSys.powerdata_data[type] = 0;
  if(GPowerSys.my_power_type == type){
    GPowerSys.my_power_type = GPOWER_TYPE_NULL;
  }
  OS_TPrintf("GPower Clear type=%d\n", type);
}

//==================================================================
/**
 * �p���[�f�[�^��Alloc�������[�N�ɓǂݍ���
 *
 * @param   heap_id		�q�[�vID
 * @retval  POWER_CONV_DATA		�p���[�f�[�^�ւ̃|�C���^
 */
//==================================================================
POWER_CONV_DATA * GPOWER_PowerData_LoadAlloc(HEAPID heap_id)
{
  return GFL_ARC_LoadDataAlloc(ARCID_POWER, NARC_power_power_data_bin, heap_id);
}

//==================================================================
/**
 * �p���[�f�[�^��Unload����
 *
 * @param   powerdata		�p���[�f�[�^�ւ̃|�C���^
 */
//==================================================================
void GPOWER_PowerData_Unload(POWER_CONV_DATA *powerdata)
{
  GFL_HEAP_FreeMemory(powerdata);
}

//==================================================================
/**
 * G�p���[ID����G�p���[�^�C�v�����o��
 *
 * @param   powerdata		�p���[�f�[�^�ւ̃|�C���^
 * @param   gpower_id		G�p���[ID
 *
 * @retval  GPOWER_TYPE		G�p���[�^�C�v
 */
//==================================================================
GPOWER_TYPE GPOWER_ID_to_Type(const POWER_CONV_DATA *powerdata, GPOWER_ID gpower_id)
{
  return powerdata[gpower_id].type;
}

//==================================================================
/**
 * G�p���[ID����g�p�|�C���g(�f���_�}���)���擾����
 *
 * @param   powerdata		�p���[�f�[�^�ւ̃|�C���^
 * @param   gpower_id		G�p���[ID
 *
 * @retval  �g�p�|�C���g(�f���_�}���)
 */
//==================================================================
u16 GPOWER_ID_to_Point(const POWER_CONV_DATA *powerdata, GPOWER_ID gpower_id)
{
  return powerdata[gpower_id].point;
}

//G�p���[���o�O���Ă��Ȃ����ȈՓI�ȃ`�F�b�N
static void _ErrorCheck(void)
{
#ifdef PM_DEBUG
  GPOWER_TYPE type;
  int zero_count = 0;
  
  for(type = 0; type < GPOWER_TYPE_MAX; type++){
    if(GPowerSys.occur_power[type] == 0){
      zero_count++;
    }
  }
  if(zero_count == GPOWER_TYPE_MAX){
    GF_ASSERT(0);
  }
#endif
}


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * G�p���[�v�Z�F�G���J�E���g��
 *
 * @param   encount		���݂̃G���J�E���g��(���100%)
 *
 * @retval  u32		G�p���[�e����̃G���J�E���g��
 */
//==================================================================
u32 GPOWER_Calc_Encount(u32 encount)
{
  u32 calc;
  
  if(GPowerSys.occur_power[GPOWER_TYPE_ENCOUNT_UP] != GPOWER_ID_NULL){
    calc = (GPowerSys.powerdata_data[GPOWER_TYPE_ENCOUNT_UP] * encount) / 100;
    if(calc > 100){
      calc = 100;
    }
    return calc;
  }
  if(GPowerSys.occur_power[GPOWER_TYPE_ENCOUNT_DOWN] != GPOWER_ID_NULL){
    calc = (GPowerSys.powerdata_data[GPOWER_TYPE_ENCOUNT_DOWN] * encount) / 100;
    if(calc > 100){
      calc = 100;
    }
    return calc;
  }
  return encount;
}

//==================================================================
/**
 * G�p���[�v�Z�F�^�}�S�z��
 *
 * @param   add_count		���Z����l(����8�r�b�g����)
 *
 * @retval  u32		G�p���[�e����̉��Z�l(����8�r�b�g����)
 */
//==================================================================
u32 GPOWER_Calc_Hatch(u32 add_count)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_HATCH_UP] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_HATCH_UP] * add_count) / 100;
  }
  return add_count;
}

//==================================================================
/**
 * G�p���[�v�Z�F�Ȃ��x�ւ̉��Z�l
 *
 * @param   natsuki		����Ȃǂ̕␳����������̂Ȃ��x���Z�l
 *
 * @retval  s32		G�p���[�e����̂Ȃ��x���Z�l
 *
 * BTS825�΍� �Ȃ��x���Z�̎��̓f���p���[���e�������Ȃ�
 */
//==================================================================
s32 GPOWER_Calc_Natsuki(s32 natsuki)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_NATSUKI_UP] != GPOWER_ID_NULL &&
     natsuki > 0 ){
    return natsuki + GPowerSys.powerdata_data[GPOWER_TYPE_NATSUKI_UP];
  }
  return natsuki;
}

//==================================================================
/**
 * G�p���[�v�Z�F���i�̒l�i
 *
 * @param   price		���i�̒l�i
 *
 * @retval  u32		  G�p���[�e����̏��i�̒l�i
 */
//==================================================================
u32 GPOWER_Calc_Sale(u32 price)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_SALE] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_SALE] * price) / 100;
  }
  return price;
}

//==================================================================
/**
 * G�p���[�v�Z�F��ʔ���
 *
 * @retval  s32		��ʔ���ID (�������Ă��Ȃ��ꍇ��GPOWER_OCCUR_NONE)
 */
//==================================================================
s32 GPOWER_Calc_LargeAmountHappen(void)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_HAPPEN] != GPOWER_ID_NULL){
    return GPowerSys.powerdata_data[GPOWER_TYPE_HAPPEN];
  }
  return GPOWER_OCCUR_NONE;
}

//==================================================================
/**
 * G�p���[�v�Z�F�擪�ɂ���|�P������HP����
 *
 * @param   party		�莝���p�[�e�B�ւ̃|�C���^
 *
 * @retval  s32		�񕜂����|�P�����̎莝���ʒu(�p���[���������Ȃ������ꍇ��GPOWER_OCCUR_NONE)
 *                (�莝���S�ĉ񕜂̎���0)
 */
//==================================================================
s32 GPOWER_Calc_HPRestore(POKEPARTY *party)
{
  POKEMON_PARAM *pp;
  int pos, hp_max, hp;
  
  if(GPowerSys.occur_power[GPOWER_TYPE_HP_RESTORE] != GPOWER_ID_NULL){
    if(GPowerSys.powerdata_data[GPOWER_TYPE_HP_RESTORE] == 999){
      STATUS_RCV_PokeParty_RecoverAll(party);
      pos = 0;
    }
    else{
      pos = PokeParty_GetMemberTopIdxBattleEnable( party );
      pp = PokeParty_GetMemberPointer(party, pos);
      {
        BOOL flag = PP_FastModeOn( pp );
        {
          hp_max = PP_Get( pp, ID_PARA_hpmax, NULL );
          hp = PP_Get( pp, ID_PARA_hp, NULL );
          hp += GPowerSys.powerdata_data[GPOWER_TYPE_HP_RESTORE];
          if(hp > hp_max){
            hp = hp_max;
          }
          PP_Put( pp , ID_PARA_hp , hp );
        }
        PP_FastModeOff( pp, flag );
      }
    }
    _OccurPowerClear(GPOWER_TYPE_HP_RESTORE);   //���ʂ͈�x�Ȃ̂ŃN���A
    return pos;
  }
  return GPOWER_OCCUR_NONE;
}

//==================================================================
/**
 * G�p���[�v�Z�F�擪�ɂ���|�P������PP����
 *
 * @param   party		�莝���p�[�e�B�ւ̃|�C���^
 *
 * @retval  s32		�񕜂����|�P�����̎莝���ʒu(�p���[���������Ȃ������ꍇ��GPOWER_OCCUR_NONE)
 */
//==================================================================
s32 GPOWER_Calc_PPRestore(POKEPARTY *party)
{
  int pos;
  POKEMON_PARAM *pp;
  
  if(GPowerSys.occur_power[GPOWER_TYPE_PP_RESTORE] != GPOWER_ID_NULL){
    pos = PokeParty_GetMemberTopIdxBattleEnable( party );
    pp = PokeParty_GetMemberPointer(party, pos);
    {
      BOOL flag = PP_FastModeOn( pp );
      {
        u32 waza, i;
        for(i=0; i<PTL_WAZA_MAX; ++i)
        {
          waza = PP_Get( pp, ID_PARA_waza1+i, NULL );
          if( waza != WAZANO_NULL )
          {
            u32 upsPP = PP_Get( pp, ID_PARA_pp_count1+i, NULL );
            u32 maxPP = WAZADATA_GetMaxPP( waza, upsPP );
            u32 nowPP = PP_Get( pp, ID_PARA_pp1+i, NULL );
            nowPP += GPowerSys.powerdata_data[GPOWER_TYPE_PP_RESTORE];
            if(nowPP > maxPP){
              nowPP = maxPP;
            }
            PP_Put( pp, ID_PARA_pp1+i, nowPP );
          }
        }
      }
      PP_FastModeOff( pp, flag );
    }
    _OccurPowerClear(GPOWER_TYPE_PP_RESTORE);   //���ʂ͈�x�Ȃ̂ŃN���A
    return pos;
  }
  return GPOWER_OCCUR_NONE;
}

//==================================================================
/**
 * G�p���[�v�Z�F�l���o���l
 *
 * @param   exp		���l�́A�O���́A�w�K���u���z�A�̕␳��̊l���o���l
 *
 * @retval  u32		G�p���[�e����̊l���o���l
 */
//==================================================================
u32 GPOWER_Calc_Exp(u32 exp)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_EXP_UP] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_EXP_UP] * exp) / 100;
  }
  if(GPowerSys.occur_power[GPOWER_TYPE_EXP_DOWN] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_EXP_DOWN] * exp) / 100;
  }
  return exp;
}

//==================================================================
/**
 * G�p���[�v�Z�F�l����������
 *
 * @param   money		����������ʂȂǂ̕␳��̂�������
 *
 * @retval  u32		G�p���[�e����̂�������
 */
//==================================================================
u32 GPOWER_Calc_Money(u32 money)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_MONEY_UP] != GPOWER_ID_NULL){
    return (GPowerSys.powerdata_data[GPOWER_TYPE_MONEY_UP] * money) / 100;
  }
  return money;
}

//==================================================================
/**
 * G�p���[�v�Z�F�ߊl��
 *
 * @param   hokakuritu		�G��HP�ƃ{�[���ł̌v�Z��̕ߊl��
 *
 * @retval  fx32		G�p���[�e����̕ߊl��
 */
//==================================================================
fx32 GPOWER_Calc_Capture(fx32 hokakuritu)
{
  fx64 calc;
  
  if(GPowerSys.occur_power[GPOWER_TYPE_CAPTURE_UP] != GPOWER_ID_NULL){
    calc = GPowerSys.powerdata_data[GPOWER_TYPE_CAPTURE_UP];
    calc = (calc * hokakuritu) / 100;
    return (fx32)calc;
  }
  return hokakuritu;
}

//==================================================================
/**
 * G�p���[�v�Z�F�z�z�p���[
 */
//==================================================================
void GPOWER_Calc_Distribution(void)
{
  if(GPowerSys.occur_power[GPOWER_TYPE_DISTRIBUTION] != GPOWER_ID_NULL){
    GF_ASSERT(0); //�d�l�҂�
  }
}

//==================================================================
/**
 * G�p���[�v�Z�F�h�ꑐ�p���[ �C���^�[�o���v�Z
 *
 * @param   interval	���݂̒��I�C���^�[�o��
 *
 * @retval  u32		�p���[�e����̒��I�C���^�[�o��
 */
//==================================================================
u32 GPOWER_Calc_EffEncInterval( u32 interval )
{
  GPOWER_EFFENC_UP_LEVEL level;
  
  if(GPowerSys.occur_power[GPOWER_TYPE_EFFENC_UP] != GPOWER_ID_NULL){
    level = GPowerSys.powerdata_data[GPOWER_TYPE_EFFENC_UP];
    if( level >= GPOWER_EFFENC_UP_LV_MAX ){
      GF_ASSERT(0); //�f�[�^�ݒ�~�X
      return interval;
    }
    if( level >= GPOWER_EFFENC_UP_LV3 ){
      OS_Printf("EffEncInterval %d->%d\n", interval, interval/2);
      return interval/2;
    }
  }
  return interval;
}

//==================================================================
/**
 * G�p���[�v�Z�F�h�ꑐ�p���[ �m���A�b�v�v�Z
 *
 * @param   prob	���݂̒��I�m��(0-100%)
 *
 * @retval  u32		�p���[�e����̒��I�m��
 */
//==================================================================
u32 GPOWER_Calc_EffEncProb( u32 prob )
{
  GPOWER_EFFENC_UP_LEVEL level;
  static const up_tbl[GPOWER_EFFENC_UP_LV_MAX] = { 10, 15, 20 };

  if(GPowerSys.occur_power[GPOWER_TYPE_EFFENC_UP] != GPOWER_ID_NULL){
    level = GPowerSys.powerdata_data[GPOWER_TYPE_EFFENC_UP];
    if( level >= GPOWER_EFFENC_UP_LV_MAX ){
      GF_ASSERT(0); //�f�[�^�ݒ�~�X
      return prob;
    }
    OS_Printf("EffEncProb %d->", prob );
    prob += up_tbl[level];
    if( prob > 100 ){
      prob = 100;
    }
    OS_Printf("%d\n", prob );
    return prob;
  }
  return prob;
}

//==================================================================
/**
 * G�p���[�v�Z�F�����݂̂� �z�u���A�b�v�v�Z
 *
 * @param   prob	���݂̒��I�m��(0-100%)
 *
 * @retval  u32		�p���[�e����̒��I�m��
 */
//==================================================================
u32 GPOWER_Calc_AnimalTrailProb( u32 prob )
{
  GPOWER_ANIMAL_TRAIL_UP_LEVEL level;
  static const up_tbl[GPOWER_ANIMAL_TRAIL_UP_LV_MAX] = { 10, 20, 30, 50 };

  if(GPowerSys.occur_power[GPOWER_TYPE_ANIMAL_TRAIL_UP] != GPOWER_ID_NULL){
    level = GPowerSys.powerdata_data[GPOWER_TYPE_ANIMAL_TRAIL_UP];
    if( level >= GPOWER_ANIMAL_TRAIL_UP_LV_MAX ){
      GF_ASSERT(0); //�f�[�^�ݒ�~�X
      return prob;
    }
    OS_Printf("ATrailProb %d->", prob );
    prob += up_tbl[level];
    if( prob > 100 ){
      prob = 100;
    }
    OS_Printf("%d\n", prob );
    return prob;
  }
  return prob;
}

//==================================================================
/**
 * G�p���[�v�Z�F�����݂̂� �|�P�������I�ɓ�����܂ł̃g���C���v�Z
 *
 * @param   count	���݂̃g���C��
 *
 * @retval  u32		�p���[�e����̃g���C��(�p���[���L���Ȃ�count+2�̒l��Ԃ� 1�Ȃ�3)
 */
//==================================================================
u32 GPOWER_Calc_AnimalTrailPokeTryCount( u32 count )
{
  GPOWER_ANIMAL_TRAIL_UP_LEVEL level;

  if(GPowerSys.occur_power[GPOWER_TYPE_ANIMAL_TRAIL_UP] != GPOWER_ID_NULL){
    level = GPowerSys.powerdata_data[GPOWER_TYPE_ANIMAL_TRAIL_UP];
    if( level >= GPOWER_ANIMAL_TRAIL_UP_LV_MAX ){
      GF_ASSERT(0); //�f�[�^�ݒ�~�X
      return count;
    }
    OS_Printf("ATrailPokeTryCt %d->%d\n", count, count+2);
    return count+2;
  }
  return count;
}

//==================================================================
/**
 * G�p���[�v�Z�F ���A�|�P�p���[ �G���J�E���g���e�[�u���V�t�g���v�Z
 *
 * @param   calc_type	ENCPROB_CALCTYPE 
 *
 * @retval  u32		�p���[�e����̃V�t�g��(�����͏��0)
 */
//==================================================================
u32 GPOWER_Calc_RarePokeEncTblShift( u32 shift )
{
  GPOWER_RAREPOKE_UP_LEVEL level;

  if(GPowerSys.occur_power[GPOWER_TYPE_RAREPOKE_UP] != GPOWER_ID_NULL){
    level = GPowerSys.powerdata_data[GPOWER_TYPE_RAREPOKE_UP];
    if( level >= GPOWER_RAREPOKE_UP_LV_MAX ){
      GF_ASSERT(0); //�f�[�^�ݒ�~�X
      return shift;
    }
    OS_Printf("RarePokeEncTblShift %d->%d\n",shift,level);
    return level;
  }
  return shift;
}

//==================================================================
/**
 * G�p���[�v�Z�F���A�|�P�p���[ ���A���I�ɓ�����܂ł̃g���C���v�Z
 *
 * @param   count	���݂̃g���C��
 *
 * @retval  u32		�p���[�e����̃g���C��(�p���[���L���Ȃ�count+1�̒l��Ԃ� 1�Ȃ�2)
 */
//==================================================================
u32 GPOWER_Calc_RarePokeEncTryCount( u32 count )
{
  GPOWER_RAREPOKE_UP_LEVEL level;

  if(GPowerSys.occur_power[GPOWER_TYPE_RAREPOKE_UP] != GPOWER_ID_NULL){
    level = GPowerSys.powerdata_data[GPOWER_TYPE_RAREPOKE_UP];
    if( level >= GPOWER_RAREPOKE_UP_LV_MAX ){
      GF_ASSERT(0); //�f�[�^�ݒ�~�X
      return count;
    }
    if( level == GPOWER_RAREPOKE_UP_LV3 ){
      OS_Printf("RarePokeEncTryCt %d->%d\n",count,count+1);
      return count+1;
    }
  }
  return count;
}

