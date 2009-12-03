/*
 *  @file   enc_pokeset.c
 *  @brief  �G���J�E���g����쐶�|�P�����f�[�^����
 *  @author Miyuki Iwasawa
 *  @date   09.10.08
 */

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

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
#include "savedata/encount_sv.h"

#include "enc_pokeset.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"

//======================================================================
//  struct
//======================================================================

///��ʔ����|�P�����f�[�^�e�[�u���^
typedef struct GENERATE_ENCOUNT{
  u16 zone_id;
  u16 mons_no;
  u8  form;
  u8  lv_min;
  u8  lv_max;
}GENERATE_ENCOUNT;

///�����_���|�P�������I�֐��^
typedef u32 (*ENC_PROB_CALC_FUNC)( u32 max );

typedef enum{
  EP_ITEM_RANGE_NORMAL = ITEM_RANGE_NORMAL,
  EP_ITEM_RANGE_HUKUGAN = ITEM_RANGE_HUKUGAN,
}ITEM_RANGE;

#define PPD_ITEM_SLOT_NUM (3) //�p�[�\�i���̃A�C�e���X���b�g��

///@todo �A�C�e������������m����` �r���S���Ȃ��Ȃ�ꍇ�A�ύX���K�v����
static u8 DATA_ItemRangeTable[][PPD_ITEM_SLOT_NUM] = {
   {50,5,0},  //�ʏ�
   {60,20,0}, //�ʏ�ӂ�����
   {45,5,5},  //�r���S
   {55,20,5}, //�r���S�ӂ�����
};

//��ʔ����f�[�^
#include "fieldmap/zone_id.h"
#include "generate_enc_def.h"
#include "generate_enc.cdat"

#define GENERATE_ENC_PROB (40*10)

////////////////////////////////////////////////////////////
//�v���g�^�C�v
static u32 eps_GetPercentRand( void );
static void efp_MonsSpaCheck( ENCPOKE_FLD_PARAM* efp, const WEATHER_NO weather );

static u32 eps_GetEncountTable( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* ioEfp, u16 zone_id, ENC_COMMON_DATA* outTable);

static void eps_SetPokeParam(const ENC_COMMON_DATA* data,ENC_POKE_PARAM* outPoke);
static u8 eps_LotFixTypeEncount(const ENCPOKE_FLD_PARAM* efp,const ENC_COMMON_DATA* enc_data,const u8 type );
static void eps_CheckSpaEncountLevel(const ENCPOKE_FLD_PARAM* efp,const ENC_COMMON_DATA* enc_tbl,ENC_POKE_PARAM* ioPoke );
static void eps_PokeLottery( const ENCPOKE_FLD_PARAM* efp, const ENC_COMMON_DATA* enc_tbl, ENC_POKE_PARAM* outPoke );

static void eps_EncPokeItemSet(POKEMON_PARAM* pp,
    POKEMON_PERSONAL_DATA* personal,ITEM_RANGE item_range,u8 bingo_f,u8 negau_hosei);
static u32 eps_EncPokeCalcPersonalRand(
    const ENCPOKE_FLD_PARAM* efp, const POKEMON_PERSONAL_DATA*  personal,const ENC_POKE_PARAM* poke );

//////////////////////////////////////////////////////
///���I�m���v�Z�֐��e�[�u��
static u32 eps_GetPercentRandGroundPokeTblNo( u32 max );
static u32 eps_GetPercentRandWaterPokeTblNo( u32 max );
static u32 eps_GetPercentRandFishingPokeTblNo( u32 max );
static u32 eps_GetPercentRandNumPokeTblNo( u32 max );

static const ENC_PROB_CALC_FUNC DATA_EncProbCalcFuncTable[ENCPROB_CALCTYPE_MAX] = {
  eps_GetPercentRandGroundPokeTblNo,
  eps_GetPercentRandWaterPokeTblNo,
  eps_GetPercentRandFishingPokeTblNo,
  eps_GetPercentRandNumPokeTblNo,
};

//---------------------------------------------------------------------------
/*
 *  @brief  �|�P�����G���J�E���g���t�B�[���h�p�����[�^�\���̃f�t�H���g�Z�b�g
 */
//---------------------------------------------------------------------------
void ENCPOKE_SetEFPStruct(ENCPOKE_FLD_PARAM* outEfp, const GAMEDATA* gdata,
    const ENCOUNT_LOCATION location, const ENCOUNT_TYPE enc_type, const WEATHER_NO weather)
{
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork((GAMEDATA*)gdata);
  POKEPARTY* party = GAMEDATA_GetMyPokemon( gdata );
  POKEMON_PARAM* pp;

  MI_CpuClear8(outEfp,sizeof(ENCPOKE_FLD_PARAM));

  outEfp->location = location;
  outEfp->enc_type = enc_type;

  outEfp->enc_save = EncDataSave_GetSaveDataPtr( save );

  //�v���C���[��ID�Z�b�g
  outEfp->my = GAMEDATA_GetMyStatus( (GAMEDATA*)gdata );
  outEfp->myID = MyStatus_GetID(outEfp->my);

  //�擪�|�P�����̃p�����[�^�`�F�b�N
  pp = PokeParty_GetMemberPointer( party, 0 );

  outEfp->mons_egg_f = PP_Get( pp, ID_PARA_tamago_flag, NULL );
  if(!outEfp->mons_egg_f)
  { //�����X�^�[No���擾
    outEfp->mons_no = PP_Get( pp, ID_PARA_monsno, NULL);
    outEfp->mons_item = PP_Get( pp, ID_PARA_item, NULL);
    outEfp->mons_spa = PP_Get( pp, ID_PARA_speabino, NULL);
    outEfp->mons_sex = PP_Get( pp, ID_PARA_sex, NULL);
    outEfp->mons_chr = PP_GetSeikaku( pp );

    //�������ʔ����`�F�b�N
    efp_MonsSpaCheck( outEfp, WEATHER_NO_SUNNY);
  }
  
  //////////////////////////////
  //�t���O�`�F�b�N
  outEfp->gameclear_f = EVENTWORK_CheckEventFlag(GAMEDATA_GetEventWork( (GAMEDATA*)gdata ),SYS_FLAG_GAME_CLEAR); 
  outEfp->gameclear_f = TRUE; //@todo ���@�N���A�t���O�����悤�ɂȂ�����폜

  //�ނ�퓬���ǂ���
  if( location == ENC_LOCATION_FISHING || location == ENC_LOCATION_FISHING_SP ){
    outEfp->fishing_f = TRUE;
  }
  //�X�v���[�`�F�b�N
  if ( !EncDataSave_CanUseSpray( EncDataSave_GetSaveDataPtr(save) ) ){
    outEfp->spray_f = TRUE;
  }
  //�����G���J�E���g�`�F�b�N
  if( outEfp->enc_type == ENC_TYPE_FORCE ){
    outEfp->enc_force_f = TRUE;
    outEfp->enc_type = ENC_TYPE_NORMAL;  //�����t���O�𗧂Ă���A���Normal�^�C�v�ŃG�~�����[�g
  }
  if( outEfp->enc_type == ENC_TYPE_EFFECT ){
    outEfp->enc_force_f = TRUE;
  }

  outEfp->companion_f = FALSE;
  outEfp->enc_double_f = FALSE;

  //�_�u���G���J�E���g�`�F�b�N
  if( outEfp->location == ENC_LOCATION_GROUND_H ){
    if(eps_GetPercentRand() < 10) {
      outEfp->enc_double_f = TRUE;
    }
  }
  //�X�v���[�y�ђ჌�x���G���J�E���g����`�F�b�N�ɗp���郌�x�����擾
  if( outEfp->spa_low_lv_rm || outEfp->spray_f){
    //�莝���̐킦���C�ڂ̃��x�����擾
    int idx = PokeParty_GetMemberTopIdxBattleEnable( party );
    pp = PokeParty_GetMemberPointer( party, idx );
    outEfp->spray_lv = PP_Get( pp, ID_PARA_level, NULL);
  }

  //�G���J�E���g������|�P������
  outEfp->enc_poke_num = outEfp->enc_double_f+1;
}

//--------------------------------------------------------------
/**
 * �����A����g�p�ɂ��m���ϓ�
 *
 * @param enc       �G���J�E���g�V�X�e���|�C���^
 * @param efp       �G���J�E���g���\���̃|�C���^
 * @param inProb    �v�Z���̊m��
 * @return  �ϓ���̊m��
 */
//--------------------------------------------------------------
u32 ENCPOKE_EncProbManipulation( const ENCPOKE_FLD_PARAM* efp, const GAMEDATA* gdata, const u32 inProb)
{
  u32 new_prob = inProb;

  //�����ɂ��m���ύX
  if( efp->spa_rate_up ){
    new_prob *= 2;
  }else if(efp->spa_rate_down){
    new_prob /= 2;
  }
  //�����A�C�e��(����߂̂��ӂ�/����߂̂�����)�ɂ��m���ϓ�
  if(efp->mons_item == ITEM_KIYOMENOOHUDA || efp->mons_item == ITEM_KIYOMENOOKOU){
    new_prob = (inProb * 2) / 3;
  }
  if(new_prob >= 100){
    new_prob = 100;
  }
  return new_prob;
}

//--------------------------------------------------------------
/**
 * @brief �G���J�E���g�e�[�u����ENCPOKE_FLD_PARAM����|�P�����p�����[�^���I
 * @param
 * @return  ���I���ꂽ�|�P������(0�Ȃ�G���J�E���g���s)
 */
//--------------------------------------------------------------
u32 ENCPOKE_GetEncountPoke( const ENCPOKE_FLD_PARAM *efp, const ENC_COMMON_DATA *enc_tbl, ENC_POKE_PARAM* outPokeTbl )
{
  int i;
  u32 num = 0;

  for(i = 0;i < efp->enc_poke_num;i++){
    eps_PokeLottery( efp, enc_tbl, &outPokeTbl[num] );
    if(!efp->enc_force_f){
      //�����ɂ�郌�x�����퓬���
      if(efp->spa_low_lv_rm && (efp->spray_lv - outPokeTbl[num].level) >= 5){
        continue;
      }
      //�X�v���[�`�F�b�N
      if(efp->spray_f && (outPokeTbl[num].level <= efp->spray_lv)){
        continue;
      }
    }
    num++;
  }
  return( num );
}

//--------------------------------------------------------------
/**
 * ROM�̃G���J�E���g�f�[�^�e�[�u������|�P�����𒊑I
 *
 * @param enc       �G���J�E���g�V�X�e���|�C���^
 * @param efp       �G���J�E���g���\���̃|�C���^
 * @param inProb    �v�Z���̊m��
 * @return  �ϓ���̊m��
 */
//--------------------------------------------------------------
int ENCPOKE_GetNormalEncountPokeData( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* efp, u16 zone_id, ENC_POKE_PARAM* outPokeTbl)
{
  u32 num = 0;
  ENC_COMMON_DATA enc_tbl[ENC_MONS_NUM_MAX];

  //ROM����G���J�E���g�f�[�^�e�[�u�����擾
  eps_GetEncountTable( inData, efp, zone_id, enc_tbl);

  //�G���J�E���g���I
  num = ENCPOKE_GetEncountPoke( efp, enc_tbl, outPokeTbl );

  return num;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�G���J�E���g�|�P�����̎擾
 *
 *	@param	cp_wfbc     WFBC���[�N
 *	@param  efp         �G���J�E���g�̏�
 *	@param	outPokeTbl  �o�͐�
 *
 * @return  ���I���ꂽ�|�P������(0�Ȃ�G���J�E���g���s)
 */
//-----------------------------------------------------------------------------
u32 ENCPOKE_GetWFBCEncountPoke( const FIELD_WFBC* cp_wfbc, ENCPOKE_FLD_PARAM* efp, ENC_POKE_PARAM* outPokeTbl )
{
  u32 num = 0;
  int i;
  int tbl_index;
  const FIELD_WFBC_PEOPLE* cp_people;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
  u32 rand;
  
  GF_ASSERT( cp_wfbc );
  GF_ASSERT( outPokeTbl );

  switch( efp->location )
  {
  case ENC_LOCATION_GROUND_L:
    tbl_index=  FIELD_WFBC_PEOPLE_ENC_POKE_GRASS_LOW;
    break;
  case ENC_LOCATION_GROUND_H:
    tbl_index=  FIELD_WFBC_PEOPLE_ENC_POKE_GRASS_HIGH;
    break;
  case ENC_LOCATION_WATER:
    tbl_index=  FIELD_WFBC_PEOPLE_ENC_POKE_WATER;
    break;

  // �Ȃ��͂��悤�Ȃ�
  case ENC_LOCATION_NONE:
    return 0;

  default:
    GF_ASSERT_MSG( 0, "WFBC enc not support enc_location = %d\n", efp->location );
    break;
  }
  

  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {

    cp_people  = FIELD_WFBC_GetPeople( cp_wfbc, i );
    if( cp_people )
    {
      cp_people_data = FIELD_WFBC_PEOPLE_GetPeopleData( cp_people );

      rand = GFUser_GetPublicRand( FIELD_WFBC_PEOPLE_ENC_POKE_PERCENT_MAX );

      TOMOYA_Printf( "percent %d\n", cp_people_data->enc_percent[ tbl_index ] );
      // �|�P�������I
      if( rand < cp_people_data->enc_percent[ tbl_index ] )
      {
        GFL_STD_MemClear( &outPokeTbl[0], sizeof(ENC_POKE_PARAM) );
        
        // �q�b�g
        outPokeTbl[0].monsNo  = cp_people_data->enc_monsno[ tbl_index ];
        outPokeTbl[0].level   = cp_people_data->enc_lv[ tbl_index ];

        TOMOYA_Printf( "monsNo %d level %d\n", outPokeTbl[0].monsNo, outPokeTbl[0].level );
        return 1;
      }
    }
  }


  return 0;
}

/*
 *  @brief  �G���J�E���g�|�P�����@POKEPARA����
 */
POKEMON_PARAM* ENCPOKE_PPCreate(const ENCPOKE_FLD_PARAM* efp, const ENC_POKE_PARAM* poke, int heapID)
{
  POKEMON_PARAM* pp = GFL_HEAP_AllocClearMemory( heapID, POKETOOL_GetWorkSize() );

  ENCPOKE_PPSetup( pp, efp, poke );

  return pp;
}

/*
 *  @brief  �G���J�E���g�|�P�����@POKEPARA����
 */
void ENCPOKE_PPSetup(POKEMON_PARAM* pp,const ENCPOKE_FLD_PARAM* efp, const ENC_POKE_PARAM* poke )
{
  u8 chr,sex;
  POKEMON_PERSONAL_DATA*  personal;

  PP_Setup( pp,poke->monsNo,poke->level,efp->myID );

  //�G���J�E���g�|�P�����̃p�[�\�i�����擾
  personal = POKE_PERSONAL_OpenHandle( poke->monsNo, poke->form, HEAPID_PROC );

  //�t�H�����Z�b�g
  PP_Put( pp, ID_PARA_form_no, poke->form );

  //�A�C�e���Z�b�g
  if(poke->item != ITEM_DUMMY_DATA){
    PP_Put( pp, ID_PARA_item, poke->item );
  }else if(efp->enc_type == ENC_TYPE_BINGO){
    eps_EncPokeItemSet(pp,personal,efp->spa_item_rate_up,TRUE,0); //@todo
  }else{
    eps_EncPokeItemSet(pp,personal,efp->spa_item_rate_up,FALSE,0);
  }

  { //�̗����Z�b�g
    u32 p_rnd = eps_EncPokeCalcPersonalRand( efp, personal, poke );

    PP_SetupEx( pp, poke->monsNo, poke->level, efp->myID, PTL_SETUP_POW_AUTO, p_rnd );
  }
  //�e�̐��ʂƖ��O��Put
  PP_Put( pp, ID_PARA_oyasex, MyStatus_GetMySex(efp->my) );
  PP_Put( pp, ID_PARA_oyaname_raw, (u32)MyStatus_GetMyName(efp->my) );

  //���i�������
  if( efp->spa_chr_fix )
  {
    PP_Put( pp, ID_PARA_seikaku, efp->mons_chr );
  }

  POKE_PERSONAL_CloseHandle( personal );
}


//===============================================================================
//�T�u�@�G���J�E�g�f�[�^�֘A
//===============================================================================
/*
 *  @brief  �������ʔ����`�F�b�N
 */
static void efp_MonsSpaCheck( ENCPOKE_FLD_PARAM* ioEfp, const WEATHER_NO weather )
{
  //������100%
  switch(ioEfp->mons_spa)
  {
  ///<�G���J�E���g��2�{(���肶������)
  case TOKUSYU_ARIZIGOKU:
  case TOKUSYU_NOOGAADO:
  case TOKUSYU_HAKKOU:
    ioEfp->spa_rate_up = TRUE;
    return;
  ///<�G���J�E���g��1/2(�������イ��)
  case TOKUSYU_AKUSYUU:
  case TOKUSYU_SIROIKEMURI:
  case TOKUSYU_HAYAASI:
    ioEfp->spa_rate_down = TRUE;
    return;
  ///<�����G���J�E���g��1/2�i���Ȃ����ꑼ)
  case TOKUSYU_SUNAGAKURE:
    if(weather == WEATHER_NO_STORM){
      ioEfp->spa_rate_down = TRUE;
    }
    return;
  ///<�����G���J�E���g��1/2�i�䂫�����ꑼ�j
  case TOKUSYU_YUKIGAKURE:
    if( weather == WEATHER_NO_SNOW || weather == WEATHER_NO_SNOWSTORM || weather == WEATHER_NO_ARARE ){
      ioEfp->spa_rate_down = TRUE;
    }
    return;
  ///<�ނ�G���J�E���g��2�{(�˂񂿂Ⴍ��)
  case TOKUSYU_NENTYAKU:
  case TOKUSYU_KYUUBAN:
    if(ioEfp->fishing_f){
      ioEfp->spa_rate_up = TRUE;
    }
    return;
  ///<�A�C�e������ɓ���₷��(�ӂ�����)
  case TOKUSYU_HUKUGAN:
    ioEfp->spa_item_rate_up = TRUE;
    return;
  ///<�莝���Ɠ������i(�V���N����)
  case TOKUSYU_SINKURO:
    ioEfp->spa_chr_fix = TRUE;
    return;
  }

  //������2/3
  if((ioEfp->mons_spa == TOKUSYU_MEROMEROBODHI) && (eps_GetPercentRand() < 67)){
    ioEfp->spa_sex_fix = TRUE;      ///<�莝���ƈقȂ鐫��(���������{�f�B��)
    return;
  }
  //������1/2
  if(eps_GetPercentRand() < 50){
    return;
  }
  switch(ioEfp->mons_spa)
  {
  ///<�n�K�l�^�C�v�Ƃ̃G���J�E���g���A�b�v
  case TOKUSYU_ZIRYOKU:
    ioEfp->spa_hagane_up = TRUE;
    return;
  ///<�d�C�^�C�v�Ƃ̃G���J�E���g���A�b�v
  case TOKUSYU_SEIDENKI:
    ioEfp->spa_denki_up = TRUE;
    return;
  ///<���x���̍����|�P�����ƃG���J�E���g���₷��(��邫��)
  case TOKUSYU_HARIKIRI:
  case TOKUSYU_YARUKI:
  case TOKUSYU_PURESSYAA:
    ioEfp->spa_high_lv_hit = TRUE;
    return;
  ///<���x���̒Ⴂ�|�P�����ƃG���J�E���g���Ȃ�(�Њd��)
  case TOKUSYU_IKAKU:
  case TOKUSYU_SURUDOIME:
    ioEfp->spa_low_lv_rm = TRUE;
    return;
  }
}

//--------------------------------------------------------------
/**
 * ��ʔ����`�F�b�N
 * @param inData    �G���J�E���g�f�[�^
 * @param location  �擾���郍�P�[�V����
 * @param type      �G���J�E���g�^�C�v
 * @param outTable  �擾���������X�^�[�f�[�^�e�[�u�����i�[����o�b�t�@�|�C���^
 * @retuen  �e�[�u����
 */
//--------------------------------------------------------------
static BOOL eps_CheckGeneratePoke( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* ioEfp, u16 zone_id, ENC_COMMON_DATA* outTable)
{
  const GENERATE_ENCOUNT* gene;
  u8  idx = EncDataSave_GetGenerateZoneIdx( ioEfp->enc_save );

  if( idx >= GENERATE_ENC_POKE_MAX ){
    idx = 0;
  }
  idx = 0;
  gene = &DATA_GenerateEncount[idx];

  if(!ioEfp->gameclear_f || gene->zone_id != zone_id){
    return FALSE;
  }
  if( ioEfp->location != ENC_LOCATION_GROUND_L && ioEfp->location != ENC_LOCATION_GROUND_H ){
    return FALSE;
  }
  if( GFUser_GetPublicRand0( 1000 ) >= GENERATE_ENC_PROB ){
    return FALSE;
  }
  outTable[0].monsNo = gene->mons_no;
  outTable[0].minLevel = gene->lv_min;
  outTable[0].maxLevel = gene->lv_max;
  outTable[0].form = gene->form;
  
  ioEfp->prob_calctype = ENCPROB_CALCTYPE_EQUAL;
  ioEfp->tbl_num = 1;
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�G���J�E���g�f�[�^�e�[�u�������P�[�V�����ƃ^�C�v����擾
 * @param inData    �G���J�E���g�f�[�^
 * @param location  �擾���郍�P�[�V����
 * @param type      �G���J�E���g�^�C�v
 * @param outTable  �擾���������X�^�[�f�[�^�e�[�u�����i�[����o�b�t�@�|�C���^
 * @retuen  �e�[�u����
 */
//--------------------------------------------------------------
static u32 eps_GetEncountTable( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* ioEfp, u16 zone_id, ENC_COMMON_DATA* outTable)
{
  u32 num = 0,calctype = ENCPROB_CALCTYPE_NORMAL;
  const ENC_COMMON_DATA* src;

  if(ioEfp->enc_type == ENC_TYPE_BINGO)
  {
    //�����Ńr���S�G���J�E���g�e�[�u�����擾
    ioEfp->prob_calctype = ENCPROB_CALCTYPE_EQUAL;
    ioEfp->tbl_num = ENC_MONS_NUM_BINGO;
    return ioEfp->tbl_num;
  }
  //@todo ��ʔ����`�F�b�N
  if( eps_CheckGeneratePoke( inData, ioEfp, zone_id, outTable)){
    return ioEfp->tbl_num;
  }

  //�e�[�u���擾
  if( ioEfp->location >= ENC_LOCATION_MAX ){
    GF_ASSERT_MSG(0,"GetEncountTable location error %d\n",ioEfp->location);
    ioEfp->location = ENC_LOCATION_GROUND_L;
  }
  switch(ioEfp->location)
  {
  case ENC_LOCATION_GROUND_L:
    num = ENC_MONS_NUM_GROUND_L;
    src = inData->groundLmons;
    break;
  case ENC_LOCATION_GROUND_H:
    num = ENC_MONS_NUM_GROUND_H;
    src = inData->groundHmons;
    break;
  case ENC_LOCATION_GROUND_SP:
    num = ENC_MONS_NUM_GROUND_SP;
    src = inData->groundSmons;
    break;
  case ENC_LOCATION_WATER:
    calctype = ENCPROB_CALCTYPE_WATER;
    num = ENC_MONS_NUM_WATER;
    src = inData->waterMons;
    break;
  case ENC_LOCATION_WATER_SP:
    calctype = ENCPROB_CALCTYPE_WATER;
    num = ENC_MONS_NUM_WATER_SP;
    src = inData->waterSpMons;
    break;
  case ENC_LOCATION_FISHING:
    calctype = ENCPROB_CALCTYPE_FISHING;
    num = ENC_MONS_NUM_FISHING;
    src = inData->fishingMons;
    break;
  case ENC_LOCATION_FISHING_SP:
    calctype = ENCPROB_CALCTYPE_FISHING;
    num = ENC_MONS_NUM_FISHING_SP;
    src = inData->fishingSpMons;
    break;
  }
  ioEfp->prob_calctype = calctype;
  ioEfp->tbl_num = num;
  MI_CpuCopy8(src,outTable,sizeof(ENC_COMMON_DATA)*num);
  return num;
}

/**
 *  @brief  �G���J�E�g�|�P�����p�����[�^�Z�b�g
 */
static void eps_SetPokeParam(const ENC_COMMON_DATA* data,ENC_POKE_PARAM* outPoke)
{
  u8 form_max;
  s16 lv_diff = data->maxLevel - data->minLevel;

  MI_CpuClear8(outPoke,sizeof(ENC_POKE_PARAM));

  if(lv_diff < 0){
    lv_diff = 0;
  }
  outPoke->level = data->minLevel + (eps_GetPercentRand() % (lv_diff+1));

  outPoke->monsNo = data->monsNo;

  form_max = POKETOOL_GetPersonalParam( data->monsNo, 0, POKEPER_ID_form_max );
  if(data->form == ENC_MONS_FORM_RND_CODE ){
    outPoke->form = GFUser_GetPublicRand0(form_max);  //�S�t�H�������烉���_��
  }else if(data->form >= form_max){
    outPoke->form = 0;
  }else{
    outPoke->form = data->form;
  }
}

/**
 *  @brief  �����ɂ�����^�C�v�̗D��G���J�E���g
 *
 *  �e�[�u��������w�肵���^�C�v�̂��̂����I��Œ��I
 */
static u8 eps_LotFixTypeEncount(const ENCPOKE_FLD_PARAM* efp,const ENC_COMMON_DATA* enc_data,const u8 type )
{
  int i,num = 0;
  u8  type1,type2;
  u8 idx_tbl[ENC_MONS_NUM_MAX];

  for(i = 0;i < efp->tbl_num;i++){
    if( enc_data[i].form == ENC_MONS_FORM_RND_CODE ){
      continue; //�t�H�����������_���R�[�h�̏ꍇ�͓����K�p���Ȃ�
    }
    type1 = POKETOOL_GetPersonalParam(enc_data[i].monsNo, enc_data[i].form, POKEPER_ID_type1 );
    type2 = POKETOOL_GetPersonalParam(enc_data[i].monsNo, enc_data[i].form, POKEPER_ID_type2 );
    if((type1 == type)||(type2 == type)){
      idx_tbl[num++] = i;
    }
  }
  if(num  == 0){
    return 0xFF;
  }
  return idx_tbl[eps_GetPercentRand()%num];
}

/*
 *  @brief  �B�������ɂ��G���J�E���g���x���␮
 *
 *  @param  ioMons  [io]�G���J�E���g�����X�^�[�p�����[�^�\����
 */
static void eps_CheckSpaEncountLevel(const ENCPOKE_FLD_PARAM* efp,const ENC_COMMON_DATA* enc_tbl,ENC_POKE_PARAM* ioPoke )
{
  int i;
  u8  max = ioPoke->level,lv_diff = FALSE;

  if( !efp->spa_high_lv_hit ){
    return; //�␮����
  }
  for(i = 0;i < efp->tbl_num;i++){
    //min��max�̍�������G���J�E���g�^�C�v���H
    if( (!lv_diff) && (enc_tbl[i].minLevel != enc_tbl[i].maxLevel)){
      lv_diff = TRUE;
    }
    if(enc_tbl[i].monsNo != ioPoke->monsNo){
      continue;
    }
    if(max < enc_tbl[i].maxLevel){
      max = enc_tbl[i].maxLevel;
    }
  }
  //�r���SorMinMax�ɍ�������G���J�E���g�^�C�v�̏ꍇ+5level�B�������A�e�[�u������maxlevel�͒����Ȃ�
  if((ioPoke->level + 5) <= max){
    if(efp->enc_type == ENC_TYPE_BINGO || lv_diff == TRUE){
      ioPoke->level += 5;
      return;
    }
  }
  ioPoke->level = max;
}

//--------------------------------------------------------------
/*
 *  @brief  �n���ꂽ�e�[�u������|�P��������̒��I
 */
//--------------------------------------------------------------
static void eps_PokeLottery( const ENCPOKE_FLD_PARAM* efp, const ENC_COMMON_DATA* enc_tbl, ENC_POKE_PARAM* outPoke )
{
  u8 idx = 0xFF;

  if(efp->spa_hagane_up){ //�n�K�l�^�C�v�D��G���J�E�g
    idx = eps_LotFixTypeEncount( efp, enc_tbl, POKETYPE_HAGANE );
  }else if(efp->spa_denki_up){  //�ł񂫃^�C�v�D��G���J�E���g
    idx = eps_LotFixTypeEncount( efp, enc_tbl, POKETYPE_DENKI );
  }
  if(idx == 0xFF){  //�^�C�v�D��G���J�E���g�Ɏ��s������w��̊m���v�Z
    idx = (DATA_EncProbCalcFuncTable[efp->prob_calctype])(efp->tbl_num);
  }
  //�G���J�E���g�����X�^�[�p�����[�^�Z�b�g
  eps_SetPokeParam(&enc_tbl[idx],outPoke);

  //�B�������ɂ��G���J�E���g���x���␳
  eps_CheckSpaEncountLevel(efp, enc_tbl, outPoke );
}

//===============================================================================
//�T�u�@POKEMON_PARAM����
//===============================================================================

/*
 *  @brief  �G���J�E���g�|�P�����A�C�e���Z�b�g
 */
static void eps_EncPokeItemSet(POKEMON_PARAM* pp,
    POKEMON_PERSONAL_DATA* personal,ITEM_RANGE item_range,u8 bingo_f,u8 negau_hosei)
{
  int i;
  u16 item[PPD_ITEM_SLOT_NUM];
  u8  rnd,tmp,range[PPD_ITEM_SLOT_NUM];

  for(i = 0;i < 3;i++){
    item[i] = POKE_PERSONAL_GetParam(personal,POKEPER_ID_item1+i);
  }
  if(item[0] == item[1] && item[1] == item[2]){
    PP_Put(pp,ID_PARA_item,item[0]);  //3�X���b�g���S�������A�C�e���Ȃ�K����������
    return;
  }
  MI_CpuCopy8(range,DATA_ItemRangeTable[item_range+bingo_f*2],sizeof(u8)*PPD_ITEM_SLOT_NUM);

  //�r���S�Ȃ烌���W�␳
  if(bingo_f){
    //@todo
    range[0] -= negau_hosei;
    range[2] += negau_hosei;
  }
  rnd = eps_GetPercentRand();

  tmp = 0;
  for(i = 0;i < PPD_ITEM_SLOT_NUM;i++){
    tmp += range[i];
    if( rnd < tmp ){
      PP_Put(pp,ID_PARA_item,item[i]);
      break;
    }
  }
}

/*
 *  @brief  �������v�Z
 *
 *  @todo
 */
static u32 eps_EncPokeCalcPersonalRand(
    const ENCPOKE_FLD_PARAM* efp, const POKEMON_PERSONAL_DATA*  personal,const ENC_POKE_PARAM* poke )
{
  u32 p_rnd;
  u8 sex_vec,sex;

  sex = PTL_SEX_UNKNOWN;
  sex_vec = POKE_PERSONAL_GetParam( (POKEMON_PERSONAL_DATA*)personal, POKEPER_ID_sex );
  if( PokePersonal_SexVecTypeGet( sex_vec ) != POKEPER_SEXTYPE_FIX &&
      efp->spa_sex_fix)
  {
    if(efp->mons_sex == PTL_SEX_MALE){
      sex = PTL_SEX_FEMALE;
    }else if(efp->mons_sex == PTL_SEX_FEMALE){
      sex = PTL_SEX_MALE;
    }
  }

  do{
    if(poke->rare){
      p_rnd = GFUser_GetPublicRand(GFL_STD_RAND_MAX);
      break;
    }else{
      p_rnd = GFUser_GetPublicRand(GFL_STD_RAND_MAX);
    }
    IWASAWA_Printf("PP_Rnd = %08x\n",p_rnd);
//    OS_Printf("PP_Rnd = %08x\n",p_rnd);

    //���i�͌��������瓱����Ȃ��Ȃ�܂���
#if 0
    if( efp->spa_chr_fix )
    {
      if( POKETOOL_GetSeikaku(p_rnd) == efp->mons_chr ){
        break;  //���i�w���v
      }
    }else if( sex != PTL_SEX_UNKNOWN )
#else
    if( sex != PTL_SEX_UNKNOWN )
#endif
    {
      if( POKETOOL_GetSex( poke->monsNo, poke->form, p_rnd) == sex ){
        break;  //���ʎw���v
      }
    }else
    {
      break;  //�w�薳��
    }
  }while(1);

  return p_rnd;
}

//===============================================================================
//�T�u�@�m���v�Z
//===============================================================================
//--------------------------------------------------------------
/**
 * �����_���m���擾
 * @retval  u32 �m��
 */
//--------------------------------------------------------------
static u32 eps_GetPercentRand( void )
{
  u32 per = (0xffff/100) + 1;
  u32 val = GFUser_GetPublicRand(0xffff) / per;
  return( val );
}

//--------------------------------------------------------------
/**
 * �����_���|�P�����Z�b�g�i���ނ�/����/���A�j
 * @param nothing
 * @return u8 �G���J�E���g�f�[�^�̃e�[�u���ԍ�
 */
//--------------------------------------------------------------
static u32 eps_GetPercentRandGroundPokeTblNo( u32 max )
{
  u32 i;

  i = eps_GetPercentRand();
  if( i < 20 )      return  0;    // 20%
  if( i >= 20 && i < 40 ) return  1;    // 20%
  if( i >= 40 && i < 50 ) return  2;    // 10%
  if( i >= 50 && i < 60 ) return  3;    // 10%
  if( i >= 60 && i < 70 ) return  4;    // 10%
  if( i >= 70 && i < 80 ) return  5;    // 10%
  if( i >= 80 && i < 85 ) return  6;    //  5%
  if( i >= 85 && i < 90 ) return  7;    //  5%
  if( i >= 90 && i < 94 ) return  8;    //  4%
  if( i >= 94 && i < 98 ) return  9;    //  4%
  if( i == 98 )     return  10;   //  1%
  return  11;               //  1%
}

//--------------------------------------------------------------
/**
 * �����_���|�P�����Z�b�g�i����j
 * @param nothing
 * @return u8 �G���J�E���g�f�[�^�̃e�[�u���ԍ�
 */
//--------------------------------------------------------------
static u32 eps_GetPercentRandWaterPokeTblNo( u32 max )
{
  u32 i;

  i = eps_GetPercentRand();

  if( i < 60 )      return  0;    // 60%
  if( i >= 60 && i < 90 ) return  1;    // 30%
  if( i >= 90 && i < 95 ) return  2;    //  5%
  if( i >= 95 && i < 99 ) return  3;    //  4%
  return  4;                //  1%
}

//--------------------------------------------------------------
/**
 * �����_���|�P�����Z�b�g�i�ނ�j
 * @param inFishingRod    �ފ�
 * @return u8 �G���J�E���g�f�[�^�̃e�[�u���ԍ�
 */
//--------------------------------------------------------------
static u32 eps_GetPercentRandFishingPokeTblNo( u32 max )
{
  u32 i;

  i = eps_GetPercentRand();

  if    ( i < 40 )    return 0; // 40%
  else if ( i < 80 )  return 1; // 40%
  else if ( i < 95 )  return 2; // 15%
  else if ( i < 99 )  return 3; // 4%
  return 4; // 1%
}

//--------------------------------------------------------------
/**
 * �����_���|�P�����Z�b�g�i1/�w�萔�j
 * @return u32 �G���J�E���g�f�[�^�̃e�[�u���ԍ�
 */
//--------------------------------------------------------------
static u32 eps_GetPercentRandNumPokeTblNo( u32 max )
{
  return eps_GetPercentRand() % max;
}

