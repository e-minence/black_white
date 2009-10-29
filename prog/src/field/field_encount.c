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

//======================================================================
//  define
//======================================================================
#define FENCOUNT_PL_NULL ///<PL��������

#define DEBUG_WB_FORCE_GROUND //�G���J�E���g�f�[�^��n��Œ肷��

#define CALC_SHIFT (8) ///<�v�Z�V�t�g�l
#define WALK_COUNT_GLOBAL (8) ///<�G���J�E���g���Ȃ�����
#define WALK_COUNT_MAX (0xffff) ///<�����J�E���g�ő�
#define NEXT_PERCENT (40) ///<�G���J�E���g�����{�m��
#define WALK_NEXT_PERCENT (5) ///<�����J�E���g���s�Ŏ��̏����ɐi�ފm��

#define LONG_GRASS_PERCENT (30) ///<�������̒��ɂ���Ƃ��̉��Z�m��
#define CYCLE_PERCENT (30) ///<���]�Ԃɏ���Ă���Ƃ��̉��Z�m��

#define HEAPID_BTLPARAM (HEAPID_PROC) ///<�o�g���p�����^�pHEAPID

//--------------------------------------------------------------
/// �G���J�E���g�f�[�^�v�f
//--------------------------------------------------------------
#ifdef DEBUG_ONLY_FOR_kagaya
//#define DEBUG_ENCOUNT_CHECKOFF_ATTR
#endif

//======================================================================
//  struct
//======================================================================
struct _TAG_FIELD_ENCOUNT
{
  FIELDMAP_WORK *fwork;
  GAMESYS_WORK *gsys;
  GAMEDATA *gdata;
  ENCOUNT_DATA* encdata;
};


//======================================================================
//  proto
//======================================================================
static ENCOUNT_LOCATION enc_GetAttrLocation( FIELD_ENCOUNT *enc );
static u32 enc_GetLocationPercent( FIELD_ENCOUNT *enc,ENCOUNT_LOCATION location );
static BOOL enc_CheckEncount( FIELD_ENCOUNT *enc, ENCOUNT_WORK* ewk, const u32 per );
static BOOL enc_CheckEncountWalk( FIELD_ENCOUNT *enc, u32 per );

static void enc_CreateBattleParam( FIELD_ENCOUNT *enc, const ENCPOKE_FLD_PARAM* spa,
    BATTLE_SETUP_PARAM *bsp, HEAPID heapID, const ENC_POKE_PARAM* inPokeTbl );

static void BATTLE_PARAM_SetUpBattleSituation( BATTLE_SETUP_PARAM* bp, FIELDMAP_WORK* fieldWork );

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
  GFL_HEAP_FreeMemory( enc );
}

//======================================================================
//  �t�B�[���h�G���J�E���g�@�`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �G���J�E���g�`�F�b�N
 * @param enc FIELD_ENCOUNT
 * @param enc_mode  ENCOUNT_MODE_???
 * @retval  NULL  �G���J�E���g�Ȃ�
 * @retval  GMEVENT*  �G���J�E���g����
 */
//--------------------------------------------------------------
void* FIELD_ENCOUNT_CheckEncount( FIELD_ENCOUNT *enc, ENCOUNT_TYPE enc_type )
{
  u32 per,enc_num;
  BOOL ret = FALSE;
  BATTLE_SETUP_PARAM* bp;
  ENCOUNT_WORK* ewk;
  ENCOUNT_LOCATION enc_loc;
  ENC_POKE_PARAM poke_tbl[FLD_ENCPOKE_NUM_MAX];

  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
  ENCPOKE_FLD_PARAM fld_spa;

  //�Ō�̃G���J�E���g����̃v���C���[�̕��������Z
  ewk = GAMEDATA_GetEncountWork(enc->gdata);
  encwork_AddPlayerWalkCount( ewk, fplayer);

#ifdef PM_DEBUG
  if( enc_type != ENC_TYPE_FORCE && DEBUG_FLG_GetFlg(DEBUG_FLG_DisableEncount) ){
    return NULL;
  }
#endif

  //���P�[�V�����`�F�b�N
  enc_loc = enc_GetAttrLocation( enc );
  per = enc_GetLocationPercent( enc, enc_loc );
  if( per <= 0 ){
    return( NULL ); //�m��0
  }

  //ENCPOKE_FLD_PARAM�쐬
  ENCPOKE_SetEFPStruct( &fld_spa, enc->gdata, enc_loc, enc_type, FALSE );

  if( enc_type != ENC_TYPE_FORCE )
  {
    //��������ɂ��G���J�E���g���ϓ�
    per = ENCPOKE_EncProbManipulation( &fld_spa, enc->gdata, per);

    if( enc_CheckEncount(enc,ewk,per) == FALSE ){ //�G���J�E���g�`�F�b�N
      return NULL;
    }
  }

  { //�ړ��|�P�����`�F�b�N

  }

  //�G���J�E���g�f�[�^����
  MI_CpuClear8(poke_tbl,sizeof(ENC_POKE_PARAM)*FLD_ENCPOKE_NUM_MAX);
  enc_num = ENCPOKE_GetNormalEncountPokeData( enc->encdata, &fld_spa, poke_tbl );

  if( enc_num == 0 ){ //�G���J�E���g���s
    return NULL;
  }

  //�o�g���p�����[�^�Z�b�g
  bp = BATTLE_PARAM_Create( HEAPID_BTLPARAM );
  enc_CreateBattleParam( enc, &fld_spa, bp, HEAPID_BTLPARAM, poke_tbl );

  // �v���C���[���W�X�V�������J�E���g�N���A
  encwork_SetPlayerPos( ewk, fplayer);

  //�G���J�E���g�C�x���g����
  return (void*)EVENT_WildPokeBattle( enc->gsys, enc->fwork, bp );
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�@�쐶�퓬�G���J�E���g
 * @param enc FIELD_ENCOUNT
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
void* FIELD_ENCOUNT_WildEncount( FIELD_ENCOUNT *enc, u16 mons_no, u8 mons_lv, u16 flags )
{
  BATTLE_SETUP_PARAM* bp;
  ENCOUNT_WORK* ewk;
  ENCOUNT_LOCATION enc_loc;
  ENC_POKE_PARAM poke_tbl[FLD_ENCPOKE_NUM_MAX];

  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
  ENCPOKE_FLD_PARAM fld_spa;

  //���P�[�V�����`�F�b�N
  enc_loc = enc_GetAttrLocation( enc );

  //ENCPOKE_FLD_PARAM�쐬
  ENCPOKE_SetEFPStruct( &fld_spa, enc->gdata, enc_loc, ENC_TYPE_NORMAL, FALSE );

  //�G���J�E���g�|�P�����f�[�^����
  MI_CpuClear8(poke_tbl,sizeof(ENC_POKE_PARAM)*FLD_ENCPOKE_NUM_MAX);
  {
    ENC_POKE_PARAM* poke = &poke_tbl[0];
    poke->monsNo = mons_no;
    poke->level = mons_lv;

    if( flags & SCR_WILD_BTL_FLAG_RARE ){
      poke->rare = TRUE;
    }
  }

  //�o�g���p�����[�^�Z�b�g
  bp = BATTLE_PARAM_Create( HEAPID_BTLPARAM );
  enc_CreateBattleParam( enc, &fld_spa, bp, HEAPID_BTLPARAM, poke_tbl );

  //�G���J�E���g�C�x���g����
  return (void*)EVENT_WildPokeBattle( enc->gsys, enc->fwork, bp );
}

//======================================================================
//  �T�u�@�G���J�E���g�m��
//======================================================================

//--------------------------------------------------------------
/**
 * �A�g���r���[�g����G���J�E���g���[�P�[�V�����擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
static ENCOUNT_LOCATION enc_GetAttrLocation( FIELD_ENCOUNT *enc )
{
  MAPATTR attr;
  VecFx32 pos;
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
  FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( enc->fwork );
  MAPATTR_FLAG attr_flag;

  attr = FIELD_PLAYER_GetMapAttr( fplayer );
  if(MAPATTR_IsEnable(attr) == FALSE){
    return ENC_LOCATION_ERR;
  }

  //�G���J�E���g�t���O�`�F�b�N
  attr_flag = MAPATTR_GetAttrFlag(attr);

#ifdef DEBUG_ENCOUNT_CHECKOFF_ATTR
  attr_flag |= MAPATTR_FLAGBIT_ENCOUNT;
#endif
  if((attr_flag & MAPATTR_FLAGBIT_ENCOUNT) == FALSE ){
    return ENC_LOCATION_ERR;
  }

  //�G���J�E���g�A�g���r���[�g�`�F�b�N
  {
    MAPATTR_VALUE attr_value = MAPATTR_GetAttrValue(attr);
#ifdef DEBUG_WB_FORCE_GROUND
    attr_flag = 0; //@todo ���@�n��ŌŒ�
#endif

    //���`�F�b�N
    if(attr_flag & MAPATTR_FLAGBIT_WATER){
      return ENC_LOCATION_WATER;
    }
    //�n�ʃn�C���x��
    if( MAPATTR_VALUE_CheckEncountGrassB(attr_value) ){
      return ENC_LOCATION_GROUND_H;
    }
  }
  //@todo ���̑��͍��̂Ƃ���n�ʃ��[���x���𓖂ĂĂ��� 091002
  return ENC_LOCATION_GROUND_L;
}

//--------------------------------------------------------------
/**
 * �G���J�E���g�������P�[�V��������擾
 * @param enc FIELD_ENCOUNT*
 * @param attr �`�F�b�N����MAPATTR
 * @param outEncLocation �ɃG���J�E���g�ꏊ���i�[����ENCOUNT_LOCATION
 * @retval u32 �G���J�E���g�� 0=�G���J�E���g����
 */
//--------------------------------------------------------------
static u32 enc_GetLocationPercent( FIELD_ENCOUNT *enc,ENCOUNT_LOCATION location )
{
  u32 per = 0;

  if(location >= ENC_LOCATION_MAX){
    return 0;
  }
  return enc->encdata->prob[location];
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
      per = 2;  //�ŏ��̈���ڂ͋�����2%
    }
  }

  if( enc_GetPercentRand() < per ){
    return( TRUE );
  }
#if 0   //091006���_�ł͕����ɂ��G���J�E���g���␳�͂Ȃ�
  calc_per = per << CALC_SHIFT;

  if( enc_CheckEncountWalk(enc,calc_per) == FALSE ){
    enc_AddWalkCount( enc ); //�����J�E���g

    if( enc_GetPercentRand() >= WALK_NEXT_PERCENT ){ //5%�Ŏ��̏�����
      return( FALSE );
    }
  }

  next_per = NEXT_PERCENT; //�G���J�E���g��{�m��

  { //���t�ɂ��m���ϓ�
  }

  if( next_per > 100 ){ //100%�����`�F�b�N
    next_per = 100;
  }

  if( enc_GetPercentRand() < next_per ){ //�m���`�F�b�N
    if( enc_GetPercentRand() >= per ){
      return( TRUE );
    }
  }
#endif
  return( FALSE );
}

#if 0
//--------------------------------------------------------------
/**
 * �G���J�E���g�`�F�b�N�@�����`�F�b�N
 * @param enc FIELD_ENCOUNT
 * @param per �G���J�E���g�m��
 * @retval BOOL TRUE=�G���J�E���g����
 */
//--------------------------------------------------------------
static BOOL enc_CheckEncountWalk( FIELD_ENCOUNT *enc, u32 per )
{
  per = (per/10) >> CALC_SHIFT;

  if( per > WALK_COUNT_GLOBAL ){
    per = WALK_COUNT_GLOBAL;
  }

  per = WALK_COUNT_GLOBAL - per;

  if( enc_GetWalkCount(enc) >= per ){
    return( TRUE );
  }

  return( FALSE );
}
#endif

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
  GAMEDATA *gdata = enc->gdata;

  //�G�l�~�[�p�[�e�B����
  {
    int i;
    POKEPARTY* partyEnemy = PokeParty_AllocPartyWork( heapID );
    POKEMON_PARAM* pp = GFL_HEAP_AllocClearMemory( heapID, POKETOOL_GetWorkSize() );
    MYSTATUS* myStatus = GAMEDATA_GetMyStatus( gdata );

    for(i = 0;i < ((u8)(efp->enc_double_f)+1);i++){
      ENCPOKE_PPSetup( pp, efp, &inPokeTbl[i] );
      PokeParty_Add( partyEnemy, pp );
    }
    GFL_HEAP_FreeMemory( pp );

    BP_SETUP_Wild( bsp, gdata, heapID, BTL_RULE_SINGLE+efp->enc_double_f,partyEnemy, efp->land_form, efp->weather );
  }
  BATTLE_PARAM_SetUpBattleSituation( bsp, enc->fwork );
}

//--------------------------------------------------------------
/**
 * �o�g���p�����쐬�i�g���[�i�[��j
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void enc_CreateTrainerBattleParam(
    FIELD_ENCOUNT *enc,
    BATTLE_SETUP_PARAM *param, const HEAPID heapID,
    TrainerID tr_id0, TrainerID tr_id1 )
{
  GAMESYS_WORK *gsys = enc->gsys;
  GAMEDATA *gdata = enc->gdata;

  BATTLE_PARAM_Init(param);
  
  if( (tr_id1 != 0) && tr_id0 != tr_id1 ){ //�V���O��
    BTL_SETUP_Single_Trainer(
        param, gdata, NULL, BTL_LANDFORM_GRASS, BTL_WEATHER_NONE, tr_id0 );
  }else if( tr_id0 == tr_id1 ){ //�_�u��
    BTL_SETUP_Double_Trainer(
        param, gdata, NULL, BTL_LANDFORM_GRASS, BTL_WEATHER_NONE, tr_id0 );
  }else{
    BTL_SETUP_Single_Trainer(
        param, gdata, NULL, BTL_LANDFORM_GRASS, BTL_WEATHER_NONE, tr_id0 );
  }
  
  { //�ΐ푊��̎莝���|�P��������
    param->partyEnemy1 = PokeParty_AllocPartyWork( heapID );
    GF_ASSERT( param->partyEnemy1 != NULL );
    TT_EncountTrainerDataMake( param, heapID );
  }
  BATTLE_PARAM_SetUpBattleSituation( param, enc->fwork );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�G���J�E���g�@�g���[�i�[�pBATTLE_SETUP_PARAM*�쐬
 * @param enc FIELD_ENCOUNT*
 * @param tr_id �g���[�i�[ID
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_ENCOUNT_SetTrainerBattleParam(
    FIELD_ENCOUNT *enc, BATTLE_SETUP_PARAM *setup, int tr_id0, int tr_id1, HEAPID heapID )
{
  KAGAYA_Printf( "�g���[�i�[�o�g���p�����쐬 HEAPID=%d\n", heapID );
  enc_CreateTrainerBattleParam( enc, setup, heapID, tr_id0, tr_id1 );
}

//--------------------------------------------------------------
/*
 *  @brief  �퓬�w�iID�擾
 */
//--------------------------------------------------------------
static BtlLandForm btlparam_GetBattleLandForm( FIELDMAP_WORK* fieldWork )
{
  u8  bg_type;
  MAPATTR attr;
  u16 zone_id = FIELDMAP_GetZoneID( fieldWork );
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( fieldWork );
  MAPATTR_FLAG attr_flag;

  attr = FIELD_PLAYER_GetMapAttr( FIELDMAP_GetFieldPlayer( fieldWork ) );
  bg_type = ZONEDATA_GetBattleBGType(zone_id);

/*
  BTL_LANDFORM_GRASS,   ///< ���ނ�
  BTL_LANDFORM_SAND,    ///< ���n
  BTL_LANDFORM_SEA,     ///< �C
  BTL_LANDFORM_RIVER,   ///< ��
  BTL_LANDFORM_SNOW,    ///< �ጴ
  BTL_LANDFORM_CAVE,    ///< ���A
  BTL_LANDFORM_ROCK,    ///< ���
  BTL_LANDFORM_ROOM,    ///< ����
*/
  return BTL_LANDFORM_GRASS;
}

static BtlWeather btlparam_GetBattleWeather( FIELDMAP_WORK* fieldWork )
{
  FIELD_WEATHER* fld_weather = FIELDMAP_GetFieldWeather( fieldWork );
  u32 weather = FIELD_WEATHER_GetWeatherNo( fld_weather );

  switch( weather )
  {
  case WEATHER_NO_SUNNY:
    return BTL_WEATHER_SHINE;
  case WEATHER_NO_RAIN:
  case WEATHER_NO_STORM:
  case WEATHER_NO_SPARK:
    return BTL_WEATHER_RAIN;
  case WEATHER_NO_SNOW:
  case WEATHER_NO_SNOWSTORM:
  case WEATHER_NO_ARARE:
    return BTL_WEATHER_SNOW;
  case WEATHER_NO_MIRAGE:
    return BTL_WEATHER_MIST;
  }
  return BTL_WEATHER_NONE;
}

/*
 *  @brief  �퓬�w�i/�V�� �Ȃ� FIELDMAP_WORK���Q�Ƃ��Č��肳���f�[�^���Z�b�g����
 *
 *  ��BP_SETUP_XXXX�n�̊֐����ĂсA�o�g���̊�{�p�����[�^���Z�b�g������ŌĂяo���Ă�������
 *
 *  @todo ������
 */
static void BATTLE_PARAM_SetUpBattleSituation( BATTLE_SETUP_PARAM* bp, FIELDMAP_WORK* fieldWork )
{
  u16 zone_id = FIELDMAP_GetZoneID( fieldWork );
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( fieldWork );

  { //�퓬�w�i
    u8 bg_type = ZONEDATA_GetBattleBGType(zone_id);
    if( FIELD_PLAYER_GetMoveForm( fplayer ) == PLAYER_MOVE_FORM_SWIM ){
      bg_type = 0;
    }
    bp->bgType = bg_type;
    bp->landForm = btlparam_GetBattleLandForm( fieldWork );
  }
  //�^�C���]�[���擾
  bp->timezone = GFL_RTC_GetTimeZone();  //@todo EVTIME����̎擾�ɕύX�\��

#if 0 //BGID��`���o����������\��
  if( bg_type == "���A��������"){
    bp->time_zone = TIMEZONE_NIGHT;
  }
#endif

  //�V��
  bp->weather = btlparam_GetBattleWeather( fieldWork );
 
  //BGM(�{���̓C�x���g�i�s/���݃}�b�v�Ȃǂ����Č��߂�)
  bp->musicDefault = SEQ_BGM_VS_NORAPOKE;   ///< �f�t�H���g����BGM�i���o�[
  bp->musicPinch = SEQ_BGM_BATTLEPINCH;     ///< �s���`����BGM�i���o�[

  //�G���J�E���g�G�t�F�N�g
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
