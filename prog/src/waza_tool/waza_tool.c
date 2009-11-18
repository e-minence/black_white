//============================================================================================
/**
 * @file  waza_tool.c
 * @bfief �Z�p�����[�^�c�[���Q�iWB�����Łj
 * @author  HisashiSogabe
 * @date  08.11.13
 */
//============================================================================================
#include  <gflib.h>

#include  "arc_def.h"

#include  "waza_tool/waza_tool.h"
#include  "waza_tool/wazano_def.h"
#include  "waza_tool/wazadata.h"

#include  "waza_tool_def.h"


/*------------------------------------------------------------------------------------*/
/*  structures                                                                        */
/*------------------------------------------------------------------------------------*/
struct _WAZA_DATA {

  u8  type;         ///< �^�C�v�i�݂��A�����A�ق̂��c�j
  u8  category;     ///< �J�e�S���i enum WazaCategory )
  u8  damageType;   ///< �_���[�W�^�C�v�i enum WazaDamageType )
  u8  power;        ///< �З�
  u8  hitPer;       ///< ������
  u8  basePP;       ///< ��{PPMax
  s8  priority;     ///< ���U�D��x
  u8  criticalRank; ///< �N���e�B�J�������N
  u8  hitCountMax;  ///< �ő�q�b�g��
  u8  hitCountMin;  ///< �ŏ��q�b�g��
  u8  shrinkPer;    ///< �Ђ�݊m��
  u8  sickID;       ///< ��Ԉُ�R�[�h�ienum WazaSick�j
  u8  sickPer;      ///< ��Ԉُ�̔�����
  u8  sickCont;     ///< ��Ԉُ�p���p�^�[���ienum WazaSickCont�j
  u8  sickTurnMin;  ///< ��Ԉُ�̌p���^�[���ŏ�
  u8  sickTurnMax;  ///< ��Ԉُ�̌p���^�[���ő�
  u8  rankEffType1; ///< �X�e�[�^�X�����N���ʂP
  s8  rankEffValue1;///< �X�e�[�^�X�����N���ʂP�����l
  u8  rankEffPer1;  ///< �X�e�[�^�X�����N���ʂP�̔�����
  u8  rankEffType2; ///< �X�e�[�^�X�����N���ʂQ
  s8  rankEffValue2;///< �X�e�[�^�X�����N���ʂQ�����l
  u8  rankEffPer2;  ///< �X�e�[�^�X�����N���ʂQ�̔�����
  u8  rankEffType3; ///< �X�e�[�^�X�����N���ʂR
  s8  rankEffValue3;///< �X�e�[�^�X�����N���ʂR�����l
  u8  rankEffPer3;  ///< �X�e�[�^�X�����N���ʂR�̔�����
  s8  damageRecoverRatio; ///< �_���[�W�񕜗�
  s8  HPRecoverRatio;     ///< HP�񕜗�
  u8  target;       ///< ���U���ʔ͈�( enum WazaTarget )
  u32 flags;        ///< �e��t���O

};

/*------------------------------------------------------------------------------------*/
/*  consts                                                                            */
/*------------------------------------------------------------------------------------*/
enum {
  HITRATIO_MUST = 101,    ///< hitPer�i�������j�����̒l�Ȃ�A�K�����U
  CRITICAL_MUST = 6,      ///< criticalRank �����̒l�Ȃ�A�K���N���e�B�J��
};

/*------------------------------------------------------------------------------------*/
/*  globals                                                                           */
/*------------------------------------------------------------------------------------*/

/**
 *  �ȈՔŃp�����[�^�擾�����ŁA���U�p�����[�^��ǂݍ��ނ��߂ɗ��p����e���|�������[�N
 */
static WAZA_DATA  gWazaData = {0};

static inline WAZA_DATA* loadWazaDataTmp( WazaID id )
{
  GFL_ARC_LoadData( &gWazaData, ARCID_WAZA_TBL, id );
  return &gWazaData;
}

//=============================================================================================
/**
 * ���U�p�����[�^�擾�i�ȈՔŁj
 *
 * @param   id
 * @param   param
 *
 * @retval  int
 */
//=============================================================================================
int WAZADATA_GetParam( WazaID wazaID, WazaDataParam param )
{
  GF_ASSERT(wazaID < WAZANO_MAX);
  GF_ASSERT(wazaID != WAZANO_NULL);
  GF_ASSERT(param < WAZAPARAM_MAX);

  {
    WAZA_DATA* wp = loadWazaDataTmp( wazaID );
    int result = WAZADATA_PTR_GetParam( wp, param );
    if( param == WAZAPARAM_TARGET ){
      TAYA_Printf("[WAZADATA] ID=%d, target=%d\n", wazaID, result);
    }
    return result;
  }
}
//=============================================================================================
/**
 * ���U�p�����[�^�擾�i�|�C���^�Łj
 *
 * @param   id
 * @param   param
 *
 * @retval  int
 */
//=============================================================================================
int WAZADATA_PTR_GetParam( const WAZA_DATA* wazaData, WazaDataParam param )
{
  switch( param ){
  case WAZAPARAM_TYPE:                ///< �^�C�v�i�݂��A�����A�ق̂��c�j
    return wazaData->type;
  case WAZAPARAM_CATEGORY:            ///< �J�e�S���i enum WazaCategory )
    return wazaData->category;
  case WAZAPARAM_DAMAGE_TYPE:         ///< �_���[�W�^�C�v�i enum WazaDamageType )
    return wazaData->damageType;
  case WAZAPARAM_POWER:               ///< �З�
    return wazaData->power;
  case WAZAPARAM_HITPER:              ///< ������
    if( wazaData->hitPer != HITRATIO_MUST ){
      return wazaData->hitPer;
    }else{
      return 100;
    }
  case WAZAPARAM_BASE_PP:             ///< ��{PPMax
    return wazaData->basePP;
  case WAZAPARAM_PRIORITY:            ///< ���U�D��x
    return wazaData->priority;
  case WAZAPARAM_CRITICAL_RANK:       ///< �N���e�B�J�������N
    return wazaData->criticalRank;
  case WAZAPARAM_HITCOUNT_MAX:        ///< �ő�q�b�g��
    return wazaData->hitCountMax;
  case WAZAPARAM_HITCOUNT_MIN:        ///< �ŏ��q�b�g��
    return wazaData->hitCountMin;
  case WAZAPARAM_SHRINK_PER:          ///< �Ђ�݊m��
    return wazaData->shrinkPer;
  case WAZAPARAM_SICK:                ///< ��Ԉُ�R�[�h�ienum WazaSick�j
    return wazaData->sickID;
  case WAZAPARAM_SICK_PER:            ///< ��Ԉُ�̔�����
    return wazaData->sickPer;
  case WAZAPARAM_SICK_CONT:           ///< ��Ԉُ�p���p�^�[���ienum WazaSickCont�j
    return wazaData->sickCont;
  case WAZAPARAM_SICK_TURN_MIN:       ///< ��Ԉُ�̌p���^�[���ŏ�
    return wazaData->sickTurnMin;
  case WAZAPARAM_SICK_TURN_MAX:       ///< ��Ԉُ�̌p���^�[���ő�
    return wazaData->sickTurnMax;
  case WAZAPARAM_RANKTYPE_1:          ///< �X�e�[�^�X�����N���ʂP
    return wazaData->rankEffType1;
  case WAZAPARAM_RANKVALUE_1:         ///< �X�e�[�^�X�����N���ʂP�����l
    return wazaData->rankEffValue1;
  case WAZAPARAM_RANKPER_1:           ///< �X�e�[�^�X�����N���ʂP�̔�����
    return wazaData->rankEffPer1;
  case WAZAPARAM_RANKTYPE_2:          ///< �X�e�[�^�X�����N���ʂQ
    return wazaData->rankEffType2;
  case WAZAPARAM_RANKVALUE_2:         ///< �X�e�[�^�X�����N���ʂQ�����l
    return wazaData->rankEffValue2;
  case WAZAPARAM_RANKPER_2:           ///< �X�e�[�^�X�����N���ʂQ�̔�����
    return wazaData->rankEffPer2;
  case WAZAPARAM_RANKTYPE_3:          ///< �X�e�[�^�X�����N���ʂR
    return wazaData->rankEffType3;
  case WAZAPARAM_RANKVALUE_3:         ///< �X�e�[�^�X�����N���ʂR�����l
    return wazaData->rankEffValue3;
  case WAZAPARAM_RANKPER_3:           ///< �X�e�[�^�X�����N���ʂR�̔�����
    return wazaData->rankEffPer3;
  case WAZAPARAM_DAMAGE_RECOVER_RATIO:///< �_���[�W�񕜗�
    if( wazaData->damageRecoverRatio > 0 ){
      return wazaData->damageRecoverRatio;
    }else{
      return 0;
    }
  case WAZAPARAM_DAMAGE_REACTION_RATIO:
    if( wazaData->damageRecoverRatio < 0 ){
      return wazaData->damageRecoverRatio * -1;
    }else{
      return 0;
    }
  case WAZAPARAM_HP_RECOVER_RATIO:    ///< HP�񕜗�
    if( wazaData->HPRecoverRatio > 0 ){
      return wazaData->HPRecoverRatio;
    }else{
      return 0;
    }
  case WAZAPARAM_HP_REACTION_RATIO:
    if( wazaData->HPRecoverRatio < 0 ){
      return wazaData->HPRecoverRatio * -1;
    }else{
      return 0;
    }
  case WAZAPARAM_TARGET:             ///< ���U���ʔ͈�( enum WazaTarget )

    return wazaData->target;

  default:
    GF_ASSERT(0);
    break;
  }
  return 0;
}
//=============================================================================================
/**
 * ���U�t���O�擾�i�ȈՔŁj
 *
 * @param   id
 * @param   flag
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL WAZADATA_GetFlag( WazaID wazaID, WazaFlag flag )
{
  GF_ASSERT(wazaID<WAZANO_MAX);
  {
    WAZA_DATA* wp = loadWazaDataTmp( wazaID );
    return WAZADATA_PTR_GetFlag( wp, flag );
  }
}
//=============================================================================================
/**
 * ���U�t���O�擾�i�|�C���^�Łj
 *
 * @param   wazaData
 * @param   flag
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL WAZADATA_PTR_GetFlag( const WAZA_DATA* wazaData, WazaFlag flag )
{
  GF_ASSERT(flag<WAZAFLAG_MAX);
  return (wazaData->flags & (1<<flag)) != 0;
}

//=============================================================================================
/**
 * ���U�ɂ���Ĕ��������Ԉُ�̌p���p�����[�^���擾
 *
 * @param   id
 *
 * @retval  WAZA_SICKCONT_PARAM
 */
//=============================================================================================
WAZA_SICKCONT_PARAM WAZADATA_GetSickCont( WazaID id )
{
  WAZA_DATA* wp = loadWazaDataTmp( id );

  WAZA_SICKCONT_PARAM  param;

  if( wp->sickCont )
  {
    param.type = wp->sickCont;
    param.turnMin = wp->sickTurnMin;
    param.turnMax = wp->sickTurnMax;
  }
  return param;
}
//=============================================================================================
/**
 * �����N���ʂ̎�ސ���Ԃ�
 *
 * @param   id
 *
 * @retval  u8
 */
//=============================================================================================
u8 WAZADATA_GetRankEffectCount( WazaID id )
{
  WAZA_DATA* wp = loadWazaDataTmp( id );
  u8 cnt = 0;

  do {
    if( wp->rankEffType1 == WAZA_RANKEFF_NULL ){ break; }
    ++cnt;
    if( wp->rankEffType2 == WAZA_RANKEFF_NULL ){ break; }
    ++cnt;
    if( wp->rankEffType3 == WAZA_RANKEFF_NULL ){ break; }
    ++cnt;
  }while( 0 );

  return cnt;
}
//=============================================================================================
/**
 * �����N���ʂ��擾
 *
 * @param   id        [in] ���UID
 * @param   idx       [in] ���Ԗڂ̃����N���ʂ��H�i0�`�j
 * @param   volume    [out] �����N���ʂ̒��x�i+�Ȃ�A�b�v, -�Ȃ�_�E��, �߂�l==WAZA_RANKEFF_NULL�̎��̂�0�j
 *
 * @retval  WazaRankEffect    �����N����ID
 */
//=============================================================================================
WazaRankEffect WAZADATA_GetRankEffect( WazaID id, u32 idx, int* volume )
{
  GF_ASSERT(idx < WAZA_RANKEFF_NUM_MAX);
  {
    WAZA_DATA* wp = loadWazaDataTmp( id );
    WAZA_SICKCONT_PARAM  param;
    u8* ptr;

    ptr = &wp->rankEffType1;
    ptr += idx;
    if( *ptr != WAZA_RANKEFF_NULL )
    {
      *volume = *((s8*)(ptr+1));
      return *ptr;
    }
    *volume = 0;
    return WAZA_RANKEFF_NULL;
  }
}
//============================================================================================
/**
 *  PPMax���擾
 *
 * @param[in] id        �擾����Z�i���o�[
 * @param[in] maxupcnt  �}�b�N�X�A�b�v���g�p������
 *
 * @retval  u8    PPMax
 */
//============================================================================================
u8 WAZADATA_GetMaxPP( WazaID id, u8 maxupcnt )
{
  u8  ppmax;

  if( maxupcnt > 3 ){
    maxupcnt = 3;
  }

  ppmax = WAZADATA_GetParam( id, WAZAPARAM_BASE_PP );
  ppmax += ( ppmax * 20 * maxupcnt ) / 100;

  return  ppmax;
}
//=============================================================================================
/**
 * �V����ʂ�Ԃ�
 *
 * @param   id
 *
 * @retval  BtlWeather
 */
//=============================================================================================
WazaWeather WAZADATA_GetWeather( WazaID id )
{
  switch( id ){
  case WAZANO_AMAGOI:     return WAZA_WEATHER_RAIN;
  case WAZANO_NIHONBARE:  return WAZA_WEATHER_SHINE;
  case WAZANO_SUNAARASI:  return WAZA_WEATHER_SAND;
  case WAZANO_ARARE:      return WAZA_WEATHER_SNOW;

  default:
    return WAZA_WEATHER_NONE;
  }
}
//------------------------------------------------
/**
 *  ���U�J�e�S���擾
 */
//------------------------------------------------
WazaCategory  WAZADATA_GetCategory( WazaID id )
{
  return WAZADATA_GetParam( id, WAZAPARAM_CATEGORY );
}
//------------------------------------------------
/**
 *  ���U�^�C�v�擾
 */
//------------------------------------------------
PokeType WAZADATA_GetType( WazaID id )
{
  return WAZADATA_GetParam( id, WAZAPARAM_TYPE );
}
//------------------------------------------------
/**
 *  ���U�З͒l�擾
 */
//------------------------------------------------
u16 WAZADATA_GetPower( WazaID id )
{
  return WAZADATA_GetParam( id, WAZAPARAM_POWER );
}
//------------------------------------------------
/**
 *  ���U�_���[�W���ގ擾
 */
//------------------------------------------------
WazaDamageType WAZADATA_GetDamageType( WazaID id )
{
  return WAZADATA_GetParam( id, WAZAPARAM_DAMAGE_TYPE );
}
//------------------------------------------------
/**
 *  �K���t���O�擾�i�΂߂������Ȃǂ�TRUE��Ԃ��j
 */
//------------------------------------------------
BOOL WAZADATA_IsAlwaysHit( WazaID id )
{
  return WAZADATA_GetParam(id, WAZAPARAM_HITPER) == HITRATIO_MUST;
}
//------------------------------------------------
/**
 *  �_���[�W���U���ǂ�������
 */
//------------------------------------------------
BOOL WAZADATA_IsDamage( WazaID id )
{
  return WAZADATA_GetParam(id, WAZAPARAM_POWER) != 0;
}




//============================================================================================
/**
 *  �Z�f�[�^�e�[�u������l���擾�i@todo ���ԁB��ŏ����j
 *
 * @param[in] waza_no   �擾�������Z�i���o�[
 * @param[in] id      �擾�������l�̎��
 */
//============================================================================================
u32 WT_WazaDataParaGet( int waza_no, int id )
{
  switch( id ){
  case ID_WTD_kind:
    id = WAZAPARAM_DAMAGE_TYPE;
    break;
  case ID_WTD_damage:
    id = WAZAPARAM_POWER;
    break;
  case ID_WTD_wazatype:
    id = WAZAPARAM_TYPE;;
    break;
  case ID_WTD_hitprobability:
    id = WAZAPARAM_HITPER;
    break;
  case ID_WTD_pp:
    id = WAZAPARAM_BASE_PP;
    break;
  case ID_WTD_attackrange:
    id = WAZAPARAM_TARGET;
    break;
  case ID_WTD_attackpri:
    id = WAZAPARAM_PRIORITY;
    break;

  case ID_WTD_flag:
  case ID_WTD_addeffect:
  case ID_WTD_battleeffect:
  case ID_WTD_ap_no:
  case ID_WTD_contype:
    return 0;
  }
  return WAZADATA_GetParam( waza_no, id );;
}

