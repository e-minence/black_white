//============================================================================================
/**
 * @file  waza_tool.c
 * @bfief �Z�p�����[�^�c�[���Q�iWB�����Łj
 * @author  HisashiSogabe,  Masao Taya
 * @date  08.11.13
 */
//============================================================================================
#include  <gflib.h>

#include  "arc_def.h"

#include  "waza_tool/wazano_def.h"
#include  "waza_tool/wazadata.h"

#include  "waza_tool_def.h"

/*------------------------------------------------------------------------------------*/
/*  consts                                                                            */
/*------------------------------------------------------------------------------------*/
enum {
  RANK_STORE_MAX = 3,
};
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
  u8  hitCountMin : 4;  ///< �ŏ��q�b�g��
  u8  hitCountMax : 4;  ///< �ő�q�b�g��

  u16 sickID;       ///< ��Ԉُ�R�[�h�ienum WazaSick�j
  u8  sickPer;      ///< ��Ԉُ�̔�����
  u8  sickCont;     ///< ��Ԉُ�p���p�^�[���ienum WazaSickCont�j

  u8  sickTurnMin;  ///< ��Ԉُ�̌p���^�[���ŏ�
  u8  sickTurnMax;  ///< ��Ԉُ�̌p���^�[���ő�
  u8  criticalRank; ///< �N���e�B�J�������N
  u8  shrinkPer;    ///< �Ђ�݊m��

  u16 AISeqNo;            ///< AI�p�V�[�P���X�i���o�[
  s8  damageRecoverRatio; ///< �_���[�W�񕜗�
  s8  HPRecoverRatio;     ///< HP�񕜗�

  u8  target;             ///< ���U���ʔ͈�( enum WazaTarget )

  u8  rankEffType[WAZA_RANKEFF_NUM_MAX]; ///< �X�e�[�^�X�����N����
  s8  rankEffValue[WAZA_RANKEFF_NUM_MAX];///< �X�e�[�^�X�����N�����l
  u8  rankEffPer[WAZA_RANKEFF_NUM_MAX];  ///< �X�e�[�^�X�����N������

  u32 flags;        ///< �e��t���O

};

typedef struct
{ 
  WAZA_DATA** wd;
  WazaID*     wazaID;
  int         cache_size;
}WAZA_DATA_CACHE;

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
static WAZA_DATA_CACHE* wd_cache = NULL;

static inline WAZA_DATA* getCacheData( WazaID id )
{ 
  int i;

  for( i = 0 ; i < wd_cache->cache_size ; i++ )
  { 
    if( wd_cache->wazaID[ i ] == WAZANO_NULL )
    { 
      wd_cache->wazaID[ i ] = id;
      GFL_ARC_LoadData( wd_cache->wd[ i ], ARCID_WAZA_TBL, id );
      break;
    }
    if( wd_cache->wazaID[ i ] == id )
    { 
      break;
    }
  }
  if( i == wd_cache->cache_size )
  { 
    i--;
    wd_cache->wazaID[ i ] = id;
    GFL_ARC_LoadData( wd_cache->wd[ i ], ARCID_WAZA_TBL, id );
  }

  return wd_cache->wd[ i ];
}

static inline WAZA_DATA* getCacheDataByHandle( ARCHANDLE* handle, WazaID id )
{ 
  int i;

  for( i = 0 ; i < wd_cache->cache_size ; i++ )
  { 
    if( wd_cache->wazaID[ i ] == WAZANO_NULL )
    { 
      wd_cache->wazaID[ i ] = id;
      GFL_ARC_LoadDataByHandle( handle, id, wd_cache->wd[ i ] );
      break;
    }
    if( wd_cache->wazaID[ i ] == id )
    { 
      break;
    }
  }
  if( i == wd_cache->cache_size )
  { 
    i--;
    wd_cache->wazaID[ i ] = id;
    GFL_ARC_LoadDataByHandle( handle, id, wd_cache->wd[ i ] );
  }

  return wd_cache->wd[ i ];
}

static inline WAZA_DATA* loadWazaDataTmp( WazaID id )
{
  if( wd_cache )
  { 
    return getCacheData( id );
  }
  GFL_ARC_LoadData( &gWazaData, ARCID_WAZA_TBL, id );
  return &gWazaData;
}

static inline WAZA_DATA* loadWazaDataTmpHandle( ARCHANDLE* handle, WazaID id )
{
  if( wd_cache )
  { 
    return getCacheDataByHandle( handle, id );
  }
  GFL_ARC_LoadDataByHandle( handle, id, &gWazaData );
  return &gWazaData;
}

//=============================================================================================
/**
 * WAZA_DATA�\���̂̃T�C�Y�擾
 */
//=============================================================================================
u32 WAZADATA_GetWorkSize( void )
{ 
  return sizeof( WAZA_DATA );
}

//=============================================================================================
/**
 * ���U�p�����[�^�擾�p�n���h���I�[�v��
 *
 * @param   arcID
 * @param   heapID
 *
 * @retval  ARCHANDLE
 */
//=============================================================================================
ARCHANDLE*  WAZADATA_OpenDataHandle( HEAPID heapID )
{ 
  return  GFL_ARC_OpenDataHandle( ARCID_WAZA_TBL, heapID );
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
    return result;
  }
}

//=============================================================================================
/**
 * ���U�p�����[�^�擾�i�n���h���Łj
 *
 * @param   id
 * @param   param
 *
 * @retval  int
 */
//=============================================================================================
int WAZADATA_HANDLE_GetParam( ARCHANDLE* handle, WazaID wazaID, WazaDataParam param )
{
  GF_ASSERT(handle != NULL);
  GF_ASSERT(wazaID < WAZANO_MAX);
  GF_ASSERT(wazaID != WAZANO_NULL);
  GF_ASSERT(param < WAZAPARAM_MAX);

  {
    WAZA_DATA* wp = loadWazaDataTmpHandle( handle, wazaID );
    int result = WAZADATA_PTR_GetParam( wp, param );
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
    if( wazaData->criticalRank != CRITICAL_MUST ){
      return wazaData->criticalRank;
    }else{
      return 0;
    }
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

  case WAZAPARAM_AI_SEQNO:
    return wazaData->AISeqNo;

  case WAZAPARAM_RANKTYPE_1:          ///< �X�e�[�^�X�����N���ʂP
  case WAZAPARAM_RANKTYPE_2:          ///< �X�e�[�^�X�����N���ʂQ
  case WAZAPARAM_RANKTYPE_3:          ///< �X�e�[�^�X�����N���ʂR
    return wazaData->rankEffType[ param - WAZAPARAM_RANKTYPE_1 ];

  case WAZAPARAM_RANKVALUE_1:         ///< �X�e�[�^�X�����N���ʂP�����l
  case WAZAPARAM_RANKVALUE_2:         ///< �X�e�[�^�X�����N���ʂQ�����l
  case WAZAPARAM_RANKVALUE_3:         ///< �X�e�[�^�X�����N���ʂR�����l
    return wazaData->rankEffValue[ param - WAZAPARAM_RANKVALUE_1 ];

  case WAZAPARAM_RANKPER_1:           ///< �X�e�[�^�X�����N���ʂP�̔�����
  case WAZAPARAM_RANKPER_2:           ///< �X�e�[�^�X�����N���ʂQ�̔�����
  case WAZAPARAM_RANKPER_3:           ///< �X�e�[�^�X�����N���ʂR�̔�����
    return wazaData->rankEffPer[ param - WAZAPARAM_RANKPER_1 ];



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
  TAYA_Printf("FlagID=%d, bits=%08x\n", flag, wazaData->flags);
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
  u32 i, cnt = 0;
  for(i=0; i<RANK_STORE_MAX; ++i){
    if( wp->rankEffType[i] != WAZA_RANKEFF_NULL ){
      ++cnt;
    }
  }
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
    WazaRankEffect  eff;

    eff = wp->rankEffType[ idx ];
    if( eff != WAZA_RANKEFF_NULL ){
      *volume = wp->rankEffValue[ idx ];
    }
    else{
      *volume = 0;
    }
    return eff;
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
  WAZA_DATA* wp = loadWazaDataTmp( id );
  return (wp->hitPer == HITRATIO_MUST);
}
//------------------------------------------------
/**
 *  �K�N���e�B�J���t���O�擾
 */
//------------------------------------------------
BOOL WAZADATA_IsMustCritical( WazaID id )
{
  WAZA_DATA* wp = loadWazaDataTmp( id );
  return (wp->criticalRank == CRITICAL_MUST);
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


//=============================================================================================
/**
 * �L���b�V���̐���
 *
 * @param[in] size    �L���b�V���T�C�Y�i�P�ŋZ1����WAZA_DATA���m�ۂ��܂��j
 * @param[in] heapID  �q�[�vID
 */
//=============================================================================================
void  WAZADATA_CreateCache( int size, HEAPID heapID )
{ 
  int i;

  //���łɃL���b�V������������Ă�����A�T�[�g
  GF_ASSERT( wd_cache == NULL );

  wd_cache              = GFL_HEAP_AllocMemory( heapID, sizeof( WAZA_DATA_CACHE ) );
  wd_cache->wd          = GFL_HEAP_AllocMemory( heapID, sizeof( WAZA_DATA* ) * size );
  wd_cache->wazaID      = GFL_HEAP_AllocMemory( heapID, sizeof( WazaID ) * size );
  wd_cache->cache_size  = size;

  for( i = 0 ; i < size ; i++ )
  { 
    wd_cache->wd[ i ] = GFL_HEAP_AllocMemory( heapID, sizeof( WAZA_DATA ) );
    wd_cache->wazaID[ i ] = WAZANO_NULL;
  }
}

//=============================================================================================
/**
 * �L���b�V���̔j��
 */
//=============================================================================================
void  WAZADATA_DeleteCache( void )
{ 
  int i;

  for( i = 0 ; i < wd_cache->cache_size ; i++ )
  { 
    GFL_HEAP_FreeMemory( wd_cache->wd[ i ] );
  }
  GFL_HEAP_FreeMemory( wd_cache->wazaID );
  GFL_HEAP_FreeMemory( wd_cache->wd );
  GFL_HEAP_FreeMemory( wd_cache );
  wd_cache = NULL;
}

//=============================================================================================
/**
 * �L���b�V��������Ă��邩�`�F�b�N
 *
 * @retval  TRUE:�����ς� FALSE:������
 */
//=============================================================================================
BOOL  WAZADATA_CheckCache( void )
{ 
  return ( wd_cache != NULL );
}

