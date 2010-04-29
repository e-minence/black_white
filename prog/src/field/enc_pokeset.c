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
#include "gamesystem/g_power.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/tokusyu_def.h"
#include "poke_tool/poketype.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_personal.h"
#include "tr_tool/tr_tool.h"
#include "item/itemsym.h"
#include "savedata/encount_sv.h"
#include "move_pokemon_def.h"
#include "move_pokemon.h"
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

///�A�C�e������������m����`
static const u8 DATA_ItemRangeTable[][PPD_ITEM_SLOT_NUM] = {
   {50,5,0},  //�ʏ�
   {60,20,0}, //�ʏ�+�ӂ�����
   {50,5,1},  //�n�C���x���G���J�E���g��
   {60,20,5}, //�n�C���x���G���J�E���g��+�ӂ�����
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
static BOOL encpoke_CheckEncountAvoid( const ENCPOKE_FLD_PARAM *efp, u8 level );
static const GENERATE_ENCOUNT* encpoke_GetGenerateData( const GAMEDATA* gdata );
static BOOL eps_CheckGeneratePoke( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* ioEfp, u16 zone_id, ENC_COMMON_DATA* outTable);

static u32 eps_GetEncountTable( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* ioEfp, u16 zone_id, ENC_COMMON_DATA* outTable);

static void eps_SetPokeParam(const ENC_COMMON_DATA* data,ENC_POKE_PARAM* outPoke);
static u8 eps_LotFixTypeEncount(const ENCPOKE_FLD_PARAM* efp,const ENC_COMMON_DATA* enc_data,const u8 type );
static void eps_CheckSpaEncountLevel(const ENCPOKE_FLD_PARAM* efp,const ENC_COMMON_DATA* enc_tbl,ENC_POKE_PARAM* ioPoke );
static void eps_PokeLottery( const ENCPOKE_FLD_PARAM* efp, const ENC_COMMON_DATA* enc_tbl, ENC_POKE_PARAM* outPoke );

static void eps_EncPokeItemSet(POKEMON_PARAM* pp,
    POKEMON_PERSONAL_DATA* personal,ITEM_RANGE item_range, BOOL itemlv_up);
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

  outEfp->gdata = gdata;
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
    efp_MonsSpaCheck( outEfp, weather );
  }
  
  //////////////////////////////
  //�t���O�`�F�b�N
  outEfp->gameclear_f = EVENTWORK_CheckEventFlag(GAMEDATA_GetEventWork( (GAMEDATA*)gdata ),SYS_FLAG_GAME_CLEAR); 

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
    if(PokeParty_GetPokeCountBattleEnable( party ) >= 2 && eps_GetPercentRand() < 40) {
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
  //G�p���[�ϓ�
  new_prob = GPOWER_Calc_Encount( new_prob );

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
    if( encpoke_CheckEncountAvoid( efp, outPokeTbl[num].level)){
      continue;
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

//--------------------------------------------------------------
/**
 * �ړ��|�P�����G���J�E�g���I
 *
 * @param enc       �G���J�E���g�V�X�e���|�C���^
 * @param efp       �G���J�E���g���\���̃|�C���^
 * @param inProb    �v�Z���̊m��
 * @return  �ϓ���̊m��
 */
//--------------------------------------------------------------
MPD_PTR ENCPOKE_GetMovePokeEncountData( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* efp, u16 zone_id )
{
	MPD_PTR temp[MOVE_POKE_MAX];
	u8 i, enc_count;

	//�����ꏊ�ɂ���ړ��|�P�������擾
	enc_count = 0;
	for( i = 0; i < MOVE_POKE_MAX; i++){
    MPD_PTR mpd;
		if( !EncDataSave_IsMovePokeValid( efp->enc_save,i) ){
      continue;
    }
    mpd = EncDataSave_GetMovePokeDataPtr( efp->enc_save, i );
		if( zone_id != EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_ZONE_ID )){
      continue;
		}
    if( encpoke_CheckEncountAvoid( efp,
          EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_LV))){
      continue;
    }
		temp[enc_count++] = mpd;
	}

	if (enc_count == 0 || GFUser_GetPublicRand0(1000) < 500){
		return NULL;
	}
	
  //�����_���őI�o
	return temp[ GFUser_GetPublicRand0(enc_count) ];
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
  u32 total_par;
  u32 calc_par;
  
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

  // �܂��A���IPERCENT���������߂�B
  // parcent�Ƃ������A�S�|�P�����̑��ΓI�ȏo�����v�Z
  total_par = 0;
  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    cp_people  = FIELD_WFBC_GetPeople( cp_wfbc, i );
    if( cp_people )
    {
      cp_people_data = FIELD_WFBC_PEOPLE_GetPeopleData( cp_people );
      total_par += cp_people_data->enc_percent[ tbl_index ];
    }
  }

  GF_ASSERT( total_par > 0 );

  // ����̃|�P��������
  rand = GFUser_GetPublicRand( total_par );
  calc_par = 0;
  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {

    cp_people  = FIELD_WFBC_GetPeople( cp_wfbc, i );
    if( cp_people )
    {
      cp_people_data = FIELD_WFBC_PEOPLE_GetPeopleData( cp_people );

      calc_par += cp_people_data->enc_percent[ tbl_index ];

      // �|�P�������I
      if( rand < calc_par )
      {
        TOMOYA_Printf( "rand %d total_par %d local_par %d\n", rand, total_par, cp_people_data->enc_percent[ tbl_index ] );
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

//  PP_Setup( pp,poke->monsNo,poke->level,efp->myID );

  //�G���J�E���g�|�P�����̃p�[�\�i�����擾
  personal = POKE_PERSONAL_OpenHandle( poke->monsNo, poke->form, HEAPID_PROC );

  { //�̗����Z�b�g
    u32 p_rnd = eps_EncPokeCalcPersonalRand( efp, personal, poke );

    PP_SetupEx( pp, poke->monsNo, poke->level, efp->myID, PTL_SETUP_POW_AUTO, p_rnd );
  }

  //�t�H�����Z�b�g
  PP_Put( pp, ID_PARA_form_no, poke->form );

  //��3���������Z�b�g
  if( poke->spabi_3rd ){
    PP_SetTokusei3( pp, poke->monsNo, poke->form);
  }

  //�A�C�e���Z�b�g
  if( poke->item <= ITEM_DATA_MAX ){
    if(poke->item != ITEM_DUMMY_DATA){
      PP_Put( pp, ID_PARA_item, poke->item );
    }else{
      eps_EncPokeItemSet(pp,personal,efp->spa_item_rate_up, efp->location == ENC_LOCATION_GROUND_H);
    }
  }

  //�e�̐��ʂƖ��O��Put
  PP_Put( pp, ID_PARA_oyasex, MyStatus_GetMySex(efp->my) );
  PP_Put( pp, ID_PARA_oyaname_raw, (u32)MyStatus_GetMyName(efp->my) );

  //���i�������
  if( efp->spa_chr_fix )
  {
    PP_Put( pp, ID_PARA_seikaku, efp->mons_chr );
  }

  PP_Renew( pp );
  POKE_PERSONAL_CloseHandle( personal );
}

/*
 *  @brief  �G���J�E���g�ړ��|�P�����@POKEPARA����
 */
void ENCPOKE_PPSetupMovePoke(POKEMON_PARAM* pp,const ENCPOKE_FLD_PARAM* efp, MPD_PTR mpd )
{
  {
	  u16 monsno = EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_MONSNO);
	  u8 lv = EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_LV);
	  u32 pow_rnd = EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_POW_RND);
	  u32 per_rnd = EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_PER_RND);

    PP_SetupEx( pp, monsno, lv, efp->myID, pow_rnd, per_rnd );
  }
	PP_Put( pp, ID_PARA_seikaku, EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_SEIKAKU) );		// ���i
	PP_Put( pp, ID_PARA_condition, EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_COND) );		// �R���f�B�V����
	PP_Put( pp, ID_PARA_hp, EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_HP) );		// HP
  
  PP_Renew( pp );
}

/*
 *  @brief  ���ݑ�ʔ������N���Ă���]�[��ID��Ԃ�
 *
 *  @retval 0xFFFF  ��ʔ������N���Ă��Ȃ�(�N���A�O)
 *  @retval ����ȊO  ��ʔ������N���Ă���]�[��ID
 */
u16 ENCPOKE_GetGenerateZone( const GAMEDATA* gdata )
{
  const GENERATE_ENCOUNT* gene = encpoke_GetGenerateData( gdata );

  if(gene == NULL){
    return 0xFFFF;
  }
  return gene->zone_id;
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
  ///<�莝���Ɠ������i(�V���N����)
  case TOKUSYU_SINKURO:
    ioEfp->spa_chr_fix = TRUE;
    return;
  }
}

//--------------------------------------------------------------
/*
 *  @brief  ���x�����퓬����`�F�b�N
 *
 *  @retval NULL  ��ʔ������N���Ă��Ȃ�(�N���A�O)
 *  @retval ����ȊO  �������Ă����ʔ����p�����[�^
 */
//--------------------------------------------------------------
static BOOL encpoke_CheckEncountAvoid( const ENCPOKE_FLD_PARAM *efp, u8 level )
{
  if(efp->enc_force_f){
    return FALSE;
  }
  //�����ɂ�郌�x�����퓬���
  if(efp->spa_low_lv_rm && (efp->spray_lv - level) >= 5){
    return TRUE;
  }
  //�X�v���[�`�F�b�N
  if(efp->spray_f && (level <= efp->spray_lv)){
    return TRUE;
  } 
  return FALSE;
}

//--------------------------------------------------------------
/*
 *  @brief  ���݋N���Ă����ʔ����f�[�^��Ԃ�
 *
 *  @retval NULL  ��ʔ������N���Ă��Ȃ�(�N���A�O)
 *  @retval ����ȊO  �������Ă����ʔ����p�����[�^
 */
//--------------------------------------------------------------
static const GENERATE_ENCOUNT* encpoke_GetGenerateData( const GAMEDATA* gdata )
{
  const GENERATE_ENCOUNT* gene;
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork((GAMEDATA*)gdata);
  BOOL gameclear_f = EVENTWORK_CheckEventFlag(GAMEDATA_GetEventWork( (GAMEDATA*)gdata ),SYS_FLAG_GAME_CLEAR ); 
  u8  idx;
 
  if( !gameclear_f ){
    return NULL;
  }
  idx = EncDataSave_GetGenerateZoneIdx( EncDataSave_GetSaveDataPtr( save ) );
 
  if( idx >= GENERATE_ENC_POKE_MAX ){
    idx = 0;
  }
  return &DATA_GenerateEncount[idx];
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
  const GENERATE_ENCOUNT* gene = encpoke_GetGenerateData( ioEfp->gdata );
  
  if( gene == NULL || gene->zone_id != zone_id ){
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
  //MinMax�ɍ�������G���J�E���g�^�C�v�̏ꍇ+5level�B�������A�e�[�u������maxlevel�͒����Ȃ�
  if((ioPoke->level + 5) <= max){
    if(lv_diff == TRUE){
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
    POKEMON_PERSONAL_DATA* personal,ITEM_RANGE item_range,BOOL itemlv_up)
{
  int i;
  u16 item[PPD_ITEM_SLOT_NUM];
  u8  rnd,tmp;
  const u8  *range;

  for(i = 0;i < 3;i++){
    item[i] = POKE_PERSONAL_GetParam(personal,POKEPER_ID_item1+i);
  }
  if(item[0] == item[1]){
    PP_Put(pp,ID_PARA_item,item[0]);  //2�X���b�g�������A�C�e���Ȃ�K����������
    return;
  }
  range = DATA_ItemRangeTable[item_range+itemlv_up*2];
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
  u8 sex_vec;
  PtlSexSpec sex_spec;
  PtlRareSpec rare_spec;

  sex_spec = PTL_SEX_SPEC_UNKNOWN;
  sex_vec = POKE_PERSONAL_GetParam( (POKEMON_PERSONAL_DATA*)personal, POKEPER_ID_sex );

  if( PokePersonal_SexVecTypeGet( sex_vec ) != POKEPER_SEXTYPE_FIX )
  {
    if ( efp->spa_sex_fix )
    {
      //�ِ��𒊑I���邽�߂̏���
      if(efp->mons_sex == PTL_SEX_MALE){
        sex_spec = PTL_SEX_SPEC_FEMALE;
      }else if(efp->mons_sex == PTL_SEX_FEMALE){
        sex_spec = PTL_SEX_SPEC_MALE;
      }
    }

    //���ʂ��O���w�肵���ꍇ�̏����i�ِ����o�����������D�悷�邽�߁A�ِ���������ŏ������ď㏑������j
    if( poke->fixsex == POKE_SEX_SEL_MALE){
      sex_spec = PTL_SEX_SPEC_MALE;
    }else if( poke->fixsex == POKE_SEX_SEL_FEMALE){
      sex_spec = PTL_SEX_SPEC_FEMALE;
    }
  }

  //@todo�������[�v�����p�ӂ��Ȃ��悤�ɏ�����ύX����\��
#if 0
  do{
    p_rnd = GFUser_GetPublicRand(GFL_STD_RAND_MAX);
    IWASAWA_Printf("PP_Rnd = %08x\n",p_rnd);

    //���A�֎~���Ă�ꍇ�̏���
    if ( poke->rare == POKE_RARE_SEL_NOT )
    {
      //���I�����������̓��A���H
      if ( POKETOOL_CheckRare( efp->myID, p_rnd ) ) continue; //���A���o���̂ōĒ��I
    }

    //���ʂ����Ă���ꍇ�̏���
    if( sex != PTL_SEX_UNKNOWN )
    {
      if( POKETOOL_GetSex( poke->monsNo, poke->form, p_rnd) == sex ){
        break;  //���ʎw���v
      }
    }else
    {
      break;  //�w�薳��
    }
  }while(1);  
#else
  //���A�w��i���A�w��͂a�v�ł͂���܂���j
  if ( poke->rare == POKE_RARE_SEL_NOT ) rare_spec = PTL_RARE_SPEC_FALSE; //���A�֎~
  else  rare_spec = PTL_RARE_SPEC_BOTH; //�ǂ���ł���

  p_rnd = POKETOOL_CalcPersonalRandSpec( efp->myID, poke->monsNo, poke->form,
                                         sex_spec, PTL_TOKUSEI_SPEC_BOTH, rare_spec );
  
  IWASAWA_Printf("EncountMons sex_spec = %d, rare_spec = %d, p_rnd = %d\n",sex_spec, rare_spec, p_rnd & 0x000000FF);
#endif
  //���A�֎~���Ă�ꍇ�̏���
  if ( poke->rare == POKE_RARE_SEL_NOT ){
    return p_rnd;
  }
  { //HGSS�ȑO��背�A���o���₷�����鏈��

    /*
     *  (my_id�̏��16bit,����16bit,p_rnd�̏��16bit,����16bit��4��xor�����l)��8�����Ȃ烌�A
     *  �Ȃ̂ŁA���A�ɂȂ�m���͂��̂܂܂���1/8192�B�����{��1/4096�ɂ������B
     *
     *�@�����ŁA
     *  (my_id���16bit�A����16bit�Ap_rnd�̉���16bit��3��xor�����l)�̍ŏ��bit�ƁA
     *  p_rnd�̍ŏ�ʃr�b�g�������l�ɂȂ�悤�ɁAp_rnd��32bit�ڂ��㏑�����A
     *  ��r�l�̍ő傪0xFFFF����0x7FFF�ɂȂ�悤�ɂ���B
     *
     *  �ƁA�m����8/65536����8/32768�ɂȂ�̂ŁA���A���������{�ɂȂ�  by �X�{���� 10.04.2
     */
    u32 bit = ((efp->myID >> 16) ^ (efp->myID & 0xFFFF) ^ (p_rnd & 0xFFFF));
    if( bit & 0x8000 == 0x8000 ){
      p_rnd |= 0x80000000;  //32bit�ڂ�1�ɂ��� 
    }else{
      p_rnd &= 0x7FFFFFFF;  //32bit�ڂ�0�ɂ���
    }
  }
  IWASAWA_Printf("EncountMons p_rnd final = %d\n",p_rnd & 0x000000FF);
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

