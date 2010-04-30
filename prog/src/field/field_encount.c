//======================================================================
/**
 * @file  field_encount.c
 * @brief �t�B�[���h�@�G���J�E���g����
 * @author kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/rtc_tool.h"

#include "fieldmap.h"
#include "map_attr.h"

#include "encount_data.h"
#include "field_encount.h"

#include "battle/battle.h"
#include "gamesystem/btl_setup.h"
#include "gamesystem/g_power.h"
#include "savedata/record.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/tokusyu_def.h"
#include "poke_tool/poketype.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_personal.h"
#include "tr_tool/tr_tool.h"
#include "item/itemsym.h"
#include "field/zonedata.h"

#include "enc_pokeset.h"
#include "event_battle.h"
#include "field_encount_local.h"

#include "script_def.h"
#include "debug/debug_flg.h"
#include "sound/wb_sound_data.sadl"

#include "enceffno.h"

//======================================================================
//  define
//======================================================================
#define HEAPID_BTLPARAM (HEAPID_PROC) ///<�o�g���p�����^�pHEAPID

#define FIRST_STEP_ENCOUNT_PER  (1) //�ŏ��̈���ڂ̃G���J�E���g��

//--------------------------------------------------------------
/// �G���J�E���g�f�[�^�v�f
//--------------------------------------------------------------
//======================================================================
//  proto
//======================================================================
static ENCOUNT_LOCATION enc_GetLocation( FIELD_ENCOUNT *enc, ENCOUNT_TYPE enc_type, u8* prob_rev );
static u32 enc_GetLocationPercent( FIELD_ENCOUNT *enc,ENCOUNT_LOCATION location, u8 prob_rev );
static BOOL enc_CheckEncount( FIELD_ENCOUNT *enc, ENCOUNT_WORK* ewk, const u32 per );
static BOOL enc_CheckEncountWalk( FIELD_ENCOUNT *enc, u32 per );
static BOOL enc_CheckEncountWfbc( ENCOUNT_WORK* ewk, const FIELD_WFBC* cp_wfbc );

static void enc_CreateBattleParam( FIELD_ENCOUNT *enc, const ENCPOKE_FLD_PARAM* spa,
    BATTLE_SETUP_PARAM *bsp, HEAPID heapID, const ENC_POKE_PARAM* inPokeTbl );
static void enc_CreateBattleParamMovePoke( FIELD_ENCOUNT *enc, const ENCPOKE_FLD_PARAM* efp,
    BATTLE_SETUP_PARAM *bsp, HEAPID heapID, MPD_PTR mpd );

static BtlWeather btlparam_GetBattleWeather( FIELDMAP_WORK* fieldWork );

static u32 enc_GetPercentRand( void );

static int enc_GetWalkCount( FIELD_ENCOUNT *enc );
static void enc_AddWalkCount( FIELD_ENCOUNT *enc );
static void enc_ClearWalkCount( FIELD_ENCOUNT *enc );

static void encwork_SetPlayerPos( ENCOUNT_WORK* ewk, const FIELD_PLAYER* player);
static void encwork_AddPlayerWalkCount( ENCOUNT_WORK* ewk, const FIELD_PLAYER* player);
static u32 encwork_GetPlayerWalkCount( ENCOUNT_WORK* ewk );


//======================================================================
//  �t�B�[���h�G���J�E���g
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�G���J�E���g�@���[�N�쐬
 * @param fwork FIELDMAP_WORK
 * @retval FIELD_ENCOUNT*
 *
 * �t�B�[���h�G�t�F�N�g�Ɉˑ�����̂ŁA�t�B�[���h�G�t�F�N�g�R���g���[��������ɌĂяo������
 */
//--------------------------------------------------------------
FIELD_ENCOUNT * FIELD_ENCOUNT_Create( FIELDMAP_WORK *fwork )
{
  HEAPID heapID;
  FIELD_ENCOUNT *enc;

  heapID = FIELDMAP_GetHeapID( fwork );
  enc = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_ENCOUNT) );

  enc->fwork = fwork;
  enc->gsys = FIELDMAP_GetGameSysWork( enc->fwork );
  enc->gdata = GAMESYSTEM_GetGameData( enc->gsys );
  enc->encdata = EVENTDATA_GetEncountDataTable( GAMEDATA_GetEventData(enc->gdata) );

  enc->eff_enc = EFFECT_ENC_CreateWork( heapID );
  EFFECT_ENC_Init( enc, enc->eff_enc );

  return( enc );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�G���J�E���g�@�폜
 * @param enc FIELD_ENCOUNT*
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_ENCOUNT_Delete( FIELD_ENCOUNT *enc )
{
  EFFECT_ENC_End( enc, enc->eff_enc );
  EFFECT_ENC_DeleteWork(enc->eff_enc);

  GFL_HEAP_FreeMemory( enc );
}

//======================================================================
//  �t�B�[���h�G���J�E���g�@�`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �G���J�E���g�`�F�b�N(�ʏ�p)
 * @param enc FIELD_ENCOUNT
 * @param enc_mode  ENCOUNT_MODE_???
 * @retval  NULL  �G���J�E���g�Ȃ�
 * @retval  GMEVENT*  �G���J�E���g����
 */
//--------------------------------------------------------------
void* FIELD_ENCOUNT_CheckEncount( FIELD_ENCOUNT *enc, ENCOUNT_TYPE enc_type )
{
  u32 per,enc_num;
  BOOL ret = FALSE,force_f = FALSE;
  BATTLE_SETUP_PARAM* bp;
  ENCOUNT_WORK* ewk;
  ENCOUNT_LOCATION enc_loc;
  ENC_POKE_PARAM poke_tbl[FLD_ENCPOKE_NUM_MAX];
  ENCPOKE_FLD_PARAM fld_spa;
  MPD_PTR mpd = NULL;
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );

  ewk = GAMEDATA_GetEncountWork(enc->gdata);
  if( enc_type == ENC_TYPE_FORCE || enc_type == ENC_TYPE_EFFECT ){
    force_f = TRUE;
  }else{
    //�Ō�̃G���J�E���g����̃v���C���[�̕��������Z
    encwork_AddPlayerWalkCount( ewk, fplayer);
  }

#ifdef PM_DEBUG
  //�f�o�b�O�����G���J�E���gOff���[�`��
  if( enc_type != ENC_TYPE_FORCE && DEBUG_FLG_GetFlg(DEBUG_FLG_DisableEncount) ){
    return NULL;
  }
#endif

  //���P�[�V�����`�F�b�N
  {
    u8 prob_rev = 0;
    enc_loc = enc_GetLocation( enc, enc_type, &prob_rev );
    per = enc_GetLocationPercent( enc, enc_loc, prob_rev );
    if( enc_loc == ENC_LOCATION_NONE || per == 0){
      return NULL;
    }
  }
  
  //ENCPOKE_FLD_PARAM�쐬
  ENCPOKE_SetEFPStruct( &fld_spa, enc->gdata, enc_loc, enc_type,
      FIELD_WEATHER_GetWeatherNo(FIELDMAP_GetFieldWeather( enc->fwork )) );

  if( !force_f )
  {
    //��������ɂ��G���J�E���g���ϓ�
    per = ENCPOKE_EncProbManipulation( &fld_spa, enc->gdata, per);

    if( enc_CheckEncount(enc,ewk,per) == FALSE ){ //�G���J�E���g�`�F�b�N
      return NULL;
    }
  }

  if( enc_type == ENC_TYPE_EFFECT ){
    //�G�t�F�N�g�G���J�E���g���R�[�h�C���N�������g
    RECORD_Inc(GAMEDATA_GetRecordPtr( enc->gdata), RECID_EFFECT_ENCOUNT_NUM );
  }else{
    //�ړ��|�P�����`�F�b�N
    mpd = ENCPOKE_GetMovePokeEncountData( enc->encdata,
              &fld_spa, FIELDMAP_GetZoneID( enc->fwork ) );
  }
  if( mpd != NULL){
    //�o�g���p�����[�^�Z�b�g
    bp = BATTLE_PARAM_Create( HEAPID_BTLPARAM );
    enc_CreateBattleParamMovePoke( enc, &fld_spa, bp, HEAPID_BTLPARAM, mpd );

  }else{
    //�G���J�E���g�f�[�^����
    MI_CpuClear8(poke_tbl,sizeof(ENC_POKE_PARAM)*FLD_ENCPOKE_NUM_MAX);
    enc_num = ENCPOKE_GetNormalEncountPokeData( enc->encdata,
              &fld_spa, FIELDMAP_GetZoneID( enc->fwork ),poke_tbl );

    if( enc_num == 0 ){ //�G���J�E���g���s
      return NULL;
    }

    //�o�g���p�����[�^�Z�b�g
    bp = BATTLE_PARAM_Create( HEAPID_BTLPARAM );
    enc_CreateBattleParam( enc, &fld_spa, bp, HEAPID_BTLPARAM, poke_tbl );
  }

  // �v���C���[���W�X�V�������J�E���g�N���A
  encwork_SetPlayerPos( ewk, fplayer);

  //�G���J�E���g�C�x���g����
  return (void*)EVENT_WildPokeBattle( enc->gsys, enc->fwork, bp, FALSE, enc_type );
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�@�쐶�C�x���g�퓬�G���J�E���g
 * @param enc FIELD_ENCOUNT
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
void* FIELD_ENCOUNT_SetWildEncount( FIELD_ENCOUNT *enc, u16 mons_no, u8 mons_lv, u16 flags )
{
  BATTLE_SETUP_PARAM* bp;
  ENCOUNT_WORK* ewk;
  ENC_POKE_PARAM poke_tbl[FLD_ENCPOKE_NUM_MAX];

  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
  ENCPOKE_FLD_PARAM fld_spa;

  //ENCPOKE_FLD_PARAM�쐬
  ENCPOKE_SetEFPStruct( &fld_spa, enc->gdata, ENC_LOCATION_NONE, ENC_TYPE_NORMAL,
      FIELD_WEATHER_GetWeatherNo(FIELDMAP_GetFieldWeather( enc->fwork )) );

  //�G���J�E���g�|�P�����f�[�^����
  MI_CpuClear8(poke_tbl,sizeof(ENC_POKE_PARAM)*FLD_ENCPOKE_NUM_MAX);
  {
    ENC_POKE_PARAM* poke = &poke_tbl[0];
    poke->monsNo = mons_no;
    poke->level = mons_lv;

    if( flags & SCR_WILD_BTL_FLAG_ITEMNONE ){
      poke->item = 0xFFFF;  //�A�C�e�������i�V
    }

    //���A���I���@
    if( flags & SCR_WILD_BTL_FLAG_RARE ){
      poke->rare = POKE_RARE_SEL_MUST;             //�K�����A
    }else if(flags & SCR_WILD_BTL_FLAG_NO_RARE){
      poke->rare = POKE_RARE_SEL_NOT;         //�K�����A�ł͂Ȃ�
    }else poke->rare = POKE_RARE_SEL_DEF;       //�ʏ풊�I
    
    if( flags & SCR_WILD_BTL_FLAG_SPEABI3 ){
      poke->spabi_3rd = TRUE;
    }

    //���ʎw��
    if ( flags & SCR_WILD_BTL_FLAG_MALE ){
      poke->fixsex = POKE_SEX_SEL_MALE;        //�K���I�X�i���ʌŒ�|�P�����ȊO�j
    }else if ( flags & SCR_WILD_BTL_FLAG_FEMALE ){
      poke->fixsex = POKE_SEX_SEL_FEMALE;        //�K�����X�i���ʌŒ�|�P�����ȊO�j
    }
  }

  //�o�g���p�����[�^�Z�b�g
  bp = BATTLE_PARAM_Create( HEAPID_BTLPARAM );
  enc_CreateBattleParam( enc, &fld_spa, bp, HEAPID_BTLPARAM, poke_tbl );
 
  //�G���J�E���g���b�Z�[�W�ω�
  if( flags & SCR_WILD_BTL_FLAG_LEGEND ){
    BATTLE_PARAM_SetBtlStatusFlag( bp, BTL_STATUS_FLAG_LEGEND );
  }else if( flags & SCR_WILD_BTL_FLAG_WILD_TALK ){
    BATTLE_PARAM_SetBtlStatusFlag( bp, BTL_STATUS_FLAG_WILD_TALK );
  }
  //�V���E���p�߂܂���܂ŉ��x�ł������o�g��
  if( flags & SCR_WILD_BTL_FLAG_IMMORTAL ){
    BATTLE_PARAM_SetBtlStatusFlag( bp, BTL_STATUS_FLAG_LEGEND_EX );
  }
  //�G���J�E���g�C�x���g����
  return (void*)EVENT_WildPokeBattle( enc->gsys, enc->fwork, bp, TRUE, ENC_TYPE_NORMAL );
}

//--------------------------------------------------------------
/**
 * �G���J�E���g�`�F�b�N(�ނ�G���J�E���g)
 * @param enc FIELD_ENCOUNT
 * @param enc_mode  ENCOUNT_MODE_???
 * @retval  NULL  �G���J�E���g�Ȃ�
 * @retval  BATTLE_SETUP_PARAM*  �G���J�E���g����
 */
//--------------------------------------------------------------
void* FIELD_ENCOUNT_CheckFishingEncount( FIELD_ENCOUNT *enc, ENCOUNT_TYPE enc_type )
{
  u32 per,enc_num;
  BATTLE_SETUP_PARAM* bp;
  ENCOUNT_WORK* ewk;
  ENCOUNT_LOCATION enc_loc;
  ENC_POKE_PARAM poke_tbl[FLD_ENCPOKE_NUM_MAX];

  ENCPOKE_FLD_PARAM fld_spa;

  //���P�[�V�����`�F�b�N
  if( enc_type == ENC_TYPE_EFFECT ){
    enc_loc = ENC_LOCATION_FISHING_SP;
  }else{
    enc_loc = ENC_LOCATION_FISHING;
  }
  per = enc_GetLocationPercent( enc, enc_loc, 0 );
  if( per <= 0 ){
    return( NULL ); //�m��0
  }

  //ENCPOKE_FLD_PARAM�쐬
  ENCPOKE_SetEFPStruct( &fld_spa, enc->gdata, enc_loc, enc_type,
      FIELD_WEATHER_GetWeatherNo(FIELDMAP_GetFieldWeather( enc->fwork )) );

  //����/����/G�p���[�ɂ��G���J�E���g���ϓ��͒ނ�ɉe�����Ȃ�
  if( enc_type != ENC_TYPE_EFFECT ){
    if( enc_GetPercentRand() < per ){
      return( NULL );
    }
  }

  //�G���J�E���g�f�[�^����
  MI_CpuClear8(poke_tbl,sizeof(ENC_POKE_PARAM)*FLD_ENCPOKE_NUM_MAX);
  enc_num = ENCPOKE_GetNormalEncountPokeData( enc->encdata, &fld_spa, FIELDMAP_GetZoneID( enc->fwork), poke_tbl );

  if( enc_num == 0 ){ //�G���J�E���g���s
    return NULL;
  }

  //�o�g���p�����[�^�Z�b�g
  bp = BATTLE_PARAM_Create( HEAPID_BTLPARAM );
  enc_CreateBattleParam( enc, &fld_spa, bp, HEAPID_BTLPARAM, poke_tbl );
  
  //�ނ�퓬�t���O
  BATTLE_PARAM_SetBtlStatusFlag( bp, BTL_STATUS_FLAG_FISHING );
  bp->fieldSituation.bgAttr = BATTLE_BG_ATTR_WATER; //�ނ�퓬�͕K�����^�C�v

  return bp;
}


//----------------------------------------------------------------------------
/**
 * WFBC�G���J�E���g�`�F�b�N
 * @param enc           FIELD_ENCOUNT
 * @param cp_wfbcedata  wfbc���
 * @retval  NULL  �G���J�E���g�Ȃ�
 * @retval  GMEVENT*  �G���J�E���g����
 */
//-----------------------------------------------------------------------------
void* FIELD_ENCOUNT_CheckWfbcEncount( FIELD_ENCOUNT *enc, const FIELD_WFBC* cp_wfbcdata )
{
  u32 per,enc_num;
  BOOL ret = FALSE,force_f = FALSE;
  BATTLE_SETUP_PARAM* bp;
  ENCOUNT_WORK* ewk;
  ENCOUNT_LOCATION enc_loc;
  ENC_POKE_PARAM poke_tbl[FLD_ENCPOKE_NUM_MAX];
  ENCPOKE_FLD_PARAM fld_spa;
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );


  ewk = GAMEDATA_GetEncountWork(enc->gdata);

  //�Ō�̃G���J�E���g����̃v���C���[�̕��������Z
  encwork_AddPlayerWalkCount( ewk, fplayer);

  // �i�����̓G���J�E���g���Ȃ�
  if( FIELD_WFBC_GetMapMode( cp_wfbcdata ) == MAPMODE_INTRUDE ){
    return NULL;
  }

#ifdef PM_DEBUG
  //�f�o�b�O�����G���J�E���gOff���[�`��
  if( !force_f && DEBUG_FLG_GetFlg(DEBUG_FLG_DisableEncount) ){
    return NULL;
  }
#endif

  // �������݂̃G���J�E���g�`�F�b�N
  if( enc_CheckEncountWfbc( ewk, cp_wfbcdata ) == FALSE ){
    return NULL;
  }

  //���P�[�V�����`�F�b�N
  {
    u8 prob_rev = 0;
    enc_loc = enc_GetLocation( enc, ENC_TYPE_WFBC, &prob_rev );
  }

  //ENCPOKE_FLD_PARAM�쐬
  ENCPOKE_SetEFPStruct( &fld_spa, enc->gdata, enc_loc, ENC_TYPE_WFBC,
      FIELD_WEATHER_GetWeatherNo(FIELDMAP_GetFieldWeather( enc->fwork )) );

  //�G���J�E���g�f�[�^����
  MI_CpuClear8(poke_tbl,sizeof(ENC_POKE_PARAM)*FLD_ENCPOKE_NUM_MAX);
  enc_num = ENCPOKE_GetWFBCEncountPoke( cp_wfbcdata, &fld_spa, poke_tbl );

  if( enc_num == 0 ){ //�G���J�E���g���s
    return NULL;
  }

  //�o�g���p�����[�^�Z�b�g
  bp = BATTLE_PARAM_Create( HEAPID_BTLPARAM );
  enc_CreateBattleParam( enc, &fld_spa, bp, HEAPID_BTLPARAM, poke_tbl );

  // �v���C���[���W�X�V�������J�E���g�N���A
  encwork_SetPlayerPos( ewk, fplayer);

  //�G���J�E���g�C�x���g����
  return (void*)EVENT_WildPokeBattle( enc->gsys, enc->fwork, bp, FALSE, ENC_TYPE_WFBC );
}

/*
 *  @brief  �퓬�w�i/�V�� �Ȃ� FIELDMAP_WORK���Q�Ƃ��Č��肳���V�`���G�[�V�����f�[�^���擾����
 */
void BTL_FIELD_SITUATION_SetFromFieldStatus( BTL_FIELD_SITUATION* sit, GAMEDATA* gdata, FIELDMAP_WORK* fieldWork )
{
  u16 zone_id = FIELDMAP_GetZoneID( fieldWork );
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( fieldWork );

  //�퓬�w�i
  sit->bgType = ZONEDATA_GetBattleBGType(zone_id);
  {
    MAPATTR attr = FIELD_PLAYER_GetMapAttr( fplayer );
    sit->bgAttr = MAPATTR_GetBattleAttrID(MAPATTR_GetAttrValue(attr));
  }
  //�^�C���]�[���擾
  sit->zoneID = zone_id;
  {
    RTCTime time;
    GFL_RTC_GetTime(&time);
    sit->hour   = time.hour;
    sit->minute = time.minute;
  }

  //�V��
  sit->weather = btlparam_GetBattleWeather( fieldWork );

  sit->season = GAMEDATA_GetSeasonID( gdata );
}

//======================================================================
//  �T�u�@�G���J�E���g�m��
//======================================================================

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�ƃG���J�E���g�^�C�v����G���J�E���g���[�P�[�V�����擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
static ENCOUNT_LOCATION enc_GetLocation( FIELD_ENCOUNT *enc, ENCOUNT_TYPE enc_type ,u8* prob_rev)
{
  MAPATTR attr;
  VecFx32 pos;
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
  FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( enc->fwork );
  MAPATTR_FLAG attr_flag;

  *prob_rev = 0;
  attr = FIELD_PLAYER_GetMapAttr( fplayer );
  if(MAPATTR_IsEnable(attr) == FALSE){
    return ENC_LOCATION_NONE;
  }

  //�G���J�E���g�t���O�`�F�b�N
  attr_flag = MAPATTR_GetAttrFlag(attr);

  //�G���J�E���g�`�F�b�N
  if( enc_type != ENC_TYPE_EFFECT && (attr_flag & MAPATTR_FLAGBIT_ENCOUNT) == FALSE ){
    return ENC_LOCATION_NONE;
  }

  //�G���J�E���g�A�g���r���[�g�`�F�b�N
  {
    MAPATTR_VALUE attr_value = MAPATTR_GetAttrValue(attr);

    //�A�g���r���[�g�ɂ��G���J�E���g���␳�擾
    if( MAPATTR_VALUE_CheckLongGrass( attr_value )){
      *prob_rev += 10;
    }
    //���`�F�b�N
    if(attr_flag & MAPATTR_FLAGBIT_WATER){
      if( enc_type == ENC_TYPE_EFFECT ) return ENC_LOCATION_WATER_SP;
      return ENC_LOCATION_WATER;
    }
    if( enc_type == ENC_TYPE_EFFECT ) return ENC_LOCATION_GROUND_SP;

    //�n�ʃn�C���x��
    if( MAPATTR_VALUE_CheckEncountGrassHigh(attr_value) ){
      return ENC_LOCATION_GROUND_H;
    }
  }
  //���̑��͒n�ʃ��[���x���𓖂ĂĂ��� 091002
  return ENC_LOCATION_GROUND_L;
}

//--------------------------------------------------------------
/**
 * �G���J�E���g�������P�[�V��������擾
 * @param enc FIELD_ENCOUNT*
 * @param location ENCOUNT_LOCATION
 * @param prov_rev �A�g���r���[�g�ɂ��G���J�E���g���␳�l
 * @retval u32 �G���J�E���g�� 0=�G���J�E���g����
 */
//--------------------------------------------------------------
static u32 enc_GetLocationPercent( FIELD_ENCOUNT *enc,ENCOUNT_LOCATION location, u8 prob_rev )
{
  s32 per = 0;

  if(location >= ENC_LOCATION_MAX){
    return 0;
  }
  per = enc->encdata->prob[location];

  if( per > 0){ //���̊m��0��0�̂܂�
    per += prob_rev;
  }
  if( per < 0){
    return 0;
  }
  return per;
}

//======================================================================
//  �T�u�@�G���J�E���g�`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �G���J�E���g�`�F�b�N
 * @param enc FIELD_ENCOUNT
 * @param per �G���J�E���g�m��
 * @param attr MAPATTR
 * @retval BOOL TRUE=�G���J�E���g����
 */
//--------------------------------------------------------------
static BOOL enc_CheckEncount( FIELD_ENCOUNT *enc, ENCOUNT_WORK* ewk, u32 per )
{
  u32 calc_per,next_per;

  if( per > 100 ){ //100%
    per = 100;
  }
  //�ړ������ɂ��G���J�E���g���␳
  {
    u32 walk_ct = encwork_GetPlayerWalkCount( ewk );
    if(walk_ct == 0){
      return FALSE;
    }else if(walk_ct == 1){
      per = FIRST_STEP_ENCOUNT_PER;  //�ŏ��̈���ڂ͋�����1%
    }
  }

  if( enc_GetPercentRand() < per ){
    return( TRUE );
  }
  return( FALSE );
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�̃G���J�E���g�`�F�b�N
 *
 *	@param	ewk       �G���J�E���g���[�N
 *	@param	cp_wfbc   WFBC���
 *
 *	@retval TRUE  �G���J�E���g����
 *	@retval FALSE �G���J�E���g���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL enc_CheckEncountWfbc( ENCOUNT_WORK* ewk, const FIELD_WFBC* cp_wfbc )
{
  s32 walk_ct = encwork_GetPlayerWalkCount( ewk );
  s32 people_num = FIELD_WFBC_GetPeopleNum( cp_wfbc );
  s32 prob;
  fx32 calc_wk;
  s32 rand = GFUser_GetPublicRand(100);
  
  
  //�i���ނ�����������-1)�@�~�@��l���@�~�@4�@�~�@G�p���[�@���@�̊m���ŃG���J�E���g����B
  calc_wk = (walk_ct-1) << FX32_SHIFT;
  calc_wk = FX_Mul( calc_wk, FX_Sqrt( people_num<<FX32_SHIFT ) );
  calc_wk = FX_Mul( calc_wk, 4<<FX32_SHIFT );
  prob = calc_wk >> FX32_SHIFT;

  // x G�p���[
  prob = GPOWER_Calc_Encount( prob );

  TOMOYA_Printf( "wfbc encount par %d  rand %d\n", prob, rand );

  if( prob > rand ){
    return TRUE;
  }
  return FALSE;
}

//======================================================================
//  �o�g���p����
//======================================================================
//--------------------------------------------------------------
/**
 * �o�g���p�����쐬
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void enc_CreateBattleParam( FIELD_ENCOUNT *enc, const ENCPOKE_FLD_PARAM* efp,
    BATTLE_SETUP_PARAM *bsp, HEAPID heapID, const ENC_POKE_PARAM* inPokeTbl )
{
  //�G�l�~�[�p�[�e�B����
  {
    int i;
    BTL_FIELD_SITUATION sit;
    POKEPARTY* partyEnemy = PokeParty_AllocPartyWork( heapID );
    POKEMON_PARAM* pp = GFL_HEAP_AllocClearMemory( heapID, POKETOOL_GetWorkSize() );

    for(i = 0;i < ((u8)(efp->enc_double_f)+1);i++){
      ENCPOKE_PPSetup( pp, efp, &inPokeTbl[i] );
      PokeParty_Add( partyEnemy, pp );
    }

    BTL_FIELD_SITUATION_SetFromFieldStatus( &sit, enc->gdata, enc->fwork );
    BTL_SETUP_Wild( bsp, enc->gdata, partyEnemy, &sit, BTL_RULE_SINGLE+efp->enc_double_f, heapID );

    if( efp->location == ENC_LOCATION_GROUND_H ){
      BATTLE_PARAM_SetBtlStatusFlag( bsp, BTL_STATUS_FLAG_HIGH_LV_ENC );
    }
    GFL_HEAP_FreeMemory( pp );
    GFL_HEAP_FreeMemory( partyEnemy );
  }
}

//--------------------------------------------------------------
/**
 * �o�g���p�����쐬(�ړ��|�P�����p)
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void enc_CreateBattleParamMovePoke( FIELD_ENCOUNT *enc, const ENCPOKE_FLD_PARAM* efp,
    BATTLE_SETUP_PARAM *bsp, HEAPID heapID, MPD_PTR mpd )
{
  //�G�l�~�[�p�[�e�B����
  BTL_FIELD_SITUATION sit;
  POKEPARTY* partyEnemy = PokeParty_AllocPartyWork( heapID );
  POKEMON_PARAM* pp = GFL_HEAP_AllocClearMemoryLo( heapID, POKETOOL_GetWorkSize() );

  ENCPOKE_PPSetupMovePoke( pp, efp, mpd );
  PokeParty_Add( partyEnemy, pp );

  BTL_FIELD_SITUATION_SetFromFieldStatus( &sit, enc->gdata, enc->fwork );
  BTL_SETUP_Wild( bsp, enc->gdata, partyEnemy, &sit, BTL_RULE_SINGLE, heapID );

  //�ړ��|�P�����o�g���t���O�Z�b�g
  BATTLE_PARAM_SetBtlStatusFlag( bsp, BTL_STATUS_FLAG_MOVE_POKE );

  GFL_HEAP_FreeMemory( pp );
  GFL_HEAP_FreeMemory( partyEnemy );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�G���J�E���g�@�g���[�i�[�pBATTLE_SETUP_PARAM*�쐬
 * @param enc FIELD_ENCOUNT*
 * @parm  rule  BtlRule
 * @param tr_id �g���[�i�[ID
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_ENCOUNT_SetTrainerBattleParam(
    FIELD_ENCOUNT *enc, BATTLE_SETUP_PARAM *bp, int rule, int partner_id, int tr_id0, int tr_id1, HEAPID heapID )
{
  GAMESYS_WORK *gsys = enc->gsys;
  GAMEDATA *gdata = enc->gdata;
  BTL_FIELD_SITUATION sit;

  BATTLE_PARAM_Init(bp);

  BTL_FIELD_SITUATION_SetFromFieldStatus( &sit, enc->gdata, enc->fwork );

  //�g���[�i�[�^�C�v�ɂ���āA�o�g��BG�A�g���r���[�g���㏑��
  {
    BtlBgAttr attr = TT_TrainerTypeBtlBgAttrGet(TT_TrainerDataParaGet( tr_id0, ID_TD_tr_type ));
    if(attr != BATTLE_BG_ATTR_MAX){
      sit.bgAttr = attr;
    }
  }

  switch(rule){
  case BTL_RULE_DOUBLE:
    if( partner_id != TRID_NULL )
    { //AI�}���`
      BTL_SETUP_AIMulti_Trainer( bp, gdata, &sit, partner_id, tr_id0, tr_id1, heapID );
    }else
    { //�_�u��
      BTL_SETUP_Double_Trainer( bp, gdata, &sit, tr_id0, heapID );
    }
    break;
  case BTL_RULE_SINGLE:
    if( tr_id1 != TRID_NULL )
    { //�^�b�O
      BTL_SETUP_Tag_Trainer( bp, gdata, &sit, tr_id0, tr_id1, heapID );
    }else
    { //�V���O��
      BTL_SETUP_Single_Trainer( bp, gdata, &sit, tr_id0, heapID );
    }
    break;
  case BTL_RULE_TRIPLE:
    //�g���v��
    BTL_SETUP_Triple_Trainer( bp, gdata, &sit, tr_id0, heapID );
    break;
  case BTL_RULE_ROTATION:
    //���[�e�[�V����
    BTL_SETUP_Rotation_Trainer( bp, gdata, &sit, tr_id0, heapID );
    break;
  default:
    GF_ASSERT(0);
  }
}

static BtlWeather btlparam_GetBattleWeather( FIELDMAP_WORK* fieldWork )
{
  FIELD_WEATHER* fld_weather = FIELDMAP_GetFieldWeather( fieldWork );
  u32 weather = FIELD_WEATHER_GetWeatherNo( fld_weather );

  switch( weather )
  {
  case WEATHER_NO_SUNNY:
    return BTL_WEATHER_NONE;
  case WEATHER_NO_RAIN:
  case WEATHER_NO_RAIKAMI:
  case WEATHER_NO_KAZAKAMI:
    return BTL_WEATHER_RAIN;
  case WEATHER_NO_STORM:
  case WEATHER_NO_STORMHIGH:
    return BTL_WEATHER_SAND;
  case WEATHER_NO_SNOW:
  case WEATHER_NO_SNOWSTORM:
  case WEATHER_NO_ARARE:
    return BTL_WEATHER_SNOW;
  }
  return BTL_WEATHER_NONE;
}

//======================================================================
//  �p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �����_���m���擾
 * @retval  u32 �m��
 */
//--------------------------------------------------------------
static u32 enc_GetPercentRand( void )
{
  u32 per = (0xffff/100) + 1;
  u32 val = GFUser_GetPublicRand(0xffff) / per;
  return( val );
}


/**
 *  @brief  �v���C���[�|�W�V�����ۑ�
 */
static void encwork_SetPlayerPos( ENCOUNT_WORK* ewk, const FIELD_PLAYER* player)
{
  MMDL* mmdl = FIELD_PLAYER_GetMMdl( player );

  MI_CpuClear8( &ewk->player, sizeof(EWK_PLAYER) );

  ewk->player.pos_x = MMDL_GetGridPosX( mmdl );
  ewk->player.pos_y = MMDL_GetGridPosY( mmdl );
  ewk->player.pos_z = MMDL_GetGridPosZ( mmdl );
}

/*
 *  @brief  �Ō�̃G���J�E���g����̃v���C���[�̕��������Z
 *  ���Ō�ɃG���J�E���g�����O���b�h�ł̕����]���͕����ɐ����Ȃ�
 */
static void encwork_AddPlayerWalkCount( ENCOUNT_WORK* ewk, const FIELD_PLAYER* player)
{
  MMDL* mmdl = FIELD_PLAYER_GetMMdl( player );

  if(ewk->player.move_f){
    ++ewk->player.walk_ct;
    return;
  }
  if( MMDL_GetGridPosX( mmdl ) == ewk->player.pos_x &&
      MMDL_GetGridPosY( mmdl ) == ewk->player.pos_y &&
      MMDL_GetGridPosZ( mmdl ) == ewk->player.pos_z ){
    return;
  }
  ewk->player.move_f = TRUE;
  ++ewk->player.walk_ct;
}

/*
 *  @brief  �v���C���[���Ō�̃G���J�E���g��ɉ������������H
 *
 *  ���Ō�ɃG���J�E���g�����O���b�h�ł̕����]���͕����ɐ����Ȃ�
 */
static u32 encwork_GetPlayerWalkCount( ENCOUNT_WORK* ewk )
{
  return ewk->player.walk_ct;
}

/**
 *  @brief  �v���C���[�X�e�[�^�X�N���A
 */
static void encwork_PlayerStatusClear( ENCOUNT_WORK* ewk )
{
  MI_CpuClear8( &ewk->player, sizeof(EWK_PLAYER) );
}
