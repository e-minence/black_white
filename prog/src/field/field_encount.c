//======================================================================
/**
 * @file  field_encount.c
 * @brief	�t�B�[���h�@�G���J�E���g����
 * @author kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "fieldmap.h"
#include "map_attr.h"

#include "field_encount.h"

#include "battle/battle.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"

#include "sound/wb_sound_data.sadl"

//======================================================================
//  define
//======================================================================
#define FENCOUNT_PL_NULL ///<PL��������

#define CALC_SHIFT (8) ///<�v�Z�V�t�g�l
#define WALK_COUNT_GLOBAL (8) ///<�G���J�E���g���Ȃ�����
#define WALK_COUNT_MAX (0xffff) ///<�����J�E���g�ő�
#define NEXT_PERCENT (40) ///<�G���J�E���g�����{�m��
#define	WALK_NEXT_PERCENT	(5) ///<�����J�E���g���s�Ŏ��̏����ɐi�ފm��

#define LONG_GRASS_PERCENT (30) ///<�������̒��ɂ���Ƃ��̉��Z�m��
#define CYCLE_PERCENT (30) ///<���]�Ԃɏ���Ă���Ƃ��̉��Z�m��

#define HEAPID_BTLPARAM (GFL_HEAPID_APP) ///<�o�g���p�����^�pHEAPID

//--------------------------------------------------------------
/// �G���J�E���g�f�[�^�v�f
//--------------------------------------------------------------
#if 0
#define ENC_MONS_NUM_NORMAL (12)
#else //5��ROM�A�V�|�P����ʂ茩����ׁA�ꎞ�I�ɑ��₷
#define ENC_MONS_NUM_NORMAL (20)
#endif
#define ENC_MONS_NUM_GENERATE (2)
#define ENC_MONS_NUM_NOON (2)
#define ENC_MONS_NUM_NIGHT (2)
#define ENC_MONS_NUM_SWAY_GRASS (4)
#define ENC_FORM_PROB_NUM (5)
#define ENC_MONS_NUM_AGB (2)
#define ENC_MONS_NUM_SEA (5)
#define ENC_MONS_NUM_ROCK (5)
#define ENC_MONS_NUM_FISH (5)

#define ENC_MONS_NUM_MAX ENC_MONS_NUM_NORMAL ///<�G���J�E���g�|�P�����ő�

#ifdef DEBUG_ONLY_FOR_kagaya
//#define DEBUG_ENCOUNT_CHECKOFF_ATTR
#endif

//--------------------------------------------------------------
//  ENCOUNT_LOCATION
//--------------------------------------------------------------
typedef enum
{
  ENCOUNT_LOCATION_GROUND = 0,
  ENCOUNT_LOCATION_WATER,
  ENCOUNT_LOCATION_FISHING,
}ENCOUNT_LOCATION;

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// FIELD_ENCOUNT
//--------------------------------------------------------------
struct _TAG_FIELD_ENCOUNT
{
  FIELDMAP_WORK *fwork;
  GAMESYS_WORK *gsys;
  GAMEDATA *gdata;
  BATTLE_SETUP_PARAM btl_setup_param;
};

//--------------------------------------------------------------
/// ENC_COMMON_DATA
//--------------------------------------------------------------
typedef struct _TAG_ENC_COMMON_DATA
{
	int MonsNo;
	u16 LvMax;
	u16 LvMin;
}ENC_COMMON_DATA;

//--------------------------------------------------------------
/// GROUND_ENC_MONSTER_DAT
//--------------------------------------------------------------
typedef struct _TAG_ENC_MONSTER_DAT
{
	char Level;
	int MonsterNo;
}GROUND_ENC_MONSTER_DAT;

//--------------------------------------------------------------
/// NON_GROUND_ENC_MONSTER_DAT
//--------------------------------------------------------------
typedef struct _TAG_NON_GROUND_ENC_MONSTER_DAT
{
	char MaxLevel;
	char MinLevel;
	int MonsterNo;
}NON_GROUND_ENC_MONSTER_DAT;

//--------------------------------------------------------------
/// ENCOUNT_DATA
//--------------------------------------------------------------
typedef struct _TAG_ENCOUNT_DATA
{
	int EncProbGround;
	GROUND_ENC_MONSTER_DAT NormalEnc[ENC_MONS_NUM_NORMAL];
	int GenerateEnc[ENC_MONS_NUM_GENERATE]; //��ʔ���
	int NoonEnc[ENC_MONS_NUM_NOON]; //���G���J�E���g
	int NightEnc[ENC_MONS_NUM_NIGHT]; //��G���J�E���g
	int SwayEnc[ENC_MONS_NUM_SWAY_GRASS]; //�h�ꑐ�G���J�E���g
  
  //�C���f�b�N�X�@0:�V�[�E�V�@1�F�V�[�h���S�@2�`4�F���g�p
	int FormProb[ENC_FORM_PROB_NUM];
	
  //�A���m�[���o���e�[�u���i0�F�A���m�[���퓬�����@1�`8�F�e�[�u���ԍ��j
  int AnnoonTable;
  
	int RubyEnc[ENC_MONS_NUM_AGB];
	int SapphireEnc[ENC_MONS_NUM_AGB];
	int EmeraldEnc[ENC_MONS_NUM_AGB];
	int FireEnc[ENC_MONS_NUM_AGB];
	int LeafEnc[ENC_MONS_NUM_AGB];
	int EncProbSea;
	NON_GROUND_ENC_MONSTER_DAT	EncSea[ENC_MONS_NUM_SEA];
	int EncProbRock;
	NON_GROUND_ENC_MONSTER_DAT	EncRock[ENC_MONS_NUM_ROCK];
	int EncProbFish1;
	NON_GROUND_ENC_MONSTER_DAT EncFish1[ENC_MONS_NUM_FISH];
	int EncProbFish2;
	NON_GROUND_ENC_MONSTER_DAT EncFish2[ENC_MONS_NUM_FISH];
	int EncProbFish3;
	NON_GROUND_ENC_MONSTER_DAT EncFish3[ENC_MONS_NUM_FISH];
}ENCOUNT_DATA;

//--------------------------------------------------------------
/// ENC_FLD_SPA
//--------------------------------------------------------------
typedef struct _TAG_ENC_FLD_SPA
{
	u32 TrainerID; //�g���[�i�[�h�c
	BOOL SprayCheck; //�X�v���[�`�F�b�N���邩�̃t���O	TRUE:�`�F�b�N����
	BOOL EncCancelSpInvalid;//�퓬����������	TRUE:����	FALSE:�L��		(����ł́A���܂�������E���܂��~�c�p)
	u8 SpMyLv; //�X�v���[�`�F�b�N�Ɏg�����x��
	u8 Egg; //�^�}�S�t���O
	u8 Spa; //����
	u8 FormProb[2]; //�V�[�E�V�E�V�[�h���S�p�t�H�����ύX�m���@0�F�V�[�E�V�@1�F�V�[�h���S
	u8 AnnoonTblType; //�A���m�[���e�[�u���^�C�v
}ENC_FLD_SPA;

//======================================================================
//  proto
//======================================================================
static u32 enc_GetAttrPercent( FIELD_ENCOUNT *enc,
    MAPATTR attr, ENCOUNT_LOCATION *outEncLocation );

static BOOL enc_CheckEncount(
    FIELD_ENCOUNT *enc, const u32 per, const MAPATTR attr );
static BOOL enc_CheckEncountWalk( FIELD_ENCOUNT *enc, u32 per );

static void enc_AddPartyPokemon(
    POKEPARTY *party, u32 monsno, u32 level, u32 id, HEAPID heapID );
static const void enc_FreeBattleSetupParam( BATTLE_SETUP_PARAM *param );

static BOOL enc_SetEncountData(
    FIELD_ENCOUNT *enc,
    BATTLE_SETUP_PARAM *param,
    const ENC_FLD_SPA *inFldSpa,
    const ENC_COMMON_DATA *enc_data, u32 location, HEAPID heapID );

static u32 enc_GetPercentRand( void );
static const ENCOUNT_DATA * enc_GetEncountData( FIELD_ENCOUNT *enc );

static void enc_SetSpaStruct(
    FIELD_ENCOUNT *enc,
    const POKEPARTY *party,
    const ENCOUNT_DATA* inData, ENC_FLD_SPA *outSpa );

static int enc_GetWalkCount( FIELD_ENCOUNT *enc );
static void enc_AddWalkCount( FIELD_ENCOUNT *enc );
static void enc_ClearWalkCount( FIELD_ENCOUNT *enc );

static const ENCOUNT_DATA dataTestEncountData;

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

//--------------------------------------------------------------
/**
 * �t�B�[���h�G���J�E���g�@BATTLE_SETUP_PARAM�擾
 * @param enc FIELD_ENCOUNT*
 * @param setup �R�s�[��BATTLE_SETUP_PARAM
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_ENCOUNT_GetBattleSetupParam(
    FIELD_ENCOUNT *enc, BATTLE_SETUP_PARAM *setup )
{
  *setup = enc->btl_setup_param;
}

//======================================================================
//  �t�B�[���h�G���J�E���g�@�`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �G���J�E���g�`�F�b�N
 * @param enc FIELD_ENCOUNT
 * @retval BOOL TRUE=�G���J�E���g����
 */
//--------------------------------------------------------------
BOOL FIELD_ENCOUNT_CheckEncount( FIELD_ENCOUNT *enc )
{
  u32 per;
  BOOL ret = FALSE;
  BOOL companion;
  int gx,gz;
  VecFx32 pos;
  
  MAPATTR attr;
  
  ENCOUNT_LOCATION enc_loc;
  
	ENC_COMMON_DATA enc_data[ENC_MONS_NUM_MAX];
  
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
  FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( enc->fwork );
  
  const ENCOUNT_DATA *data = enc_GetEncountData( enc );

  BATTLE_SETUP_PARAM *setup;
	ENC_FLD_SPA fld_spa;
  
  FIELD_PLAYER_GetPos( fplayer, &pos );
  attr = MAPATTR_GetAttribute( mapper, &pos );
  
  if( attr == MAPATTR_ERROR ){
    return( FALSE );
  }

  //090713 ROM�p ���A�g���r���[�g�G���J�E���g����
  #if 1 
  {
    MAPATTR_FLAG attr_flag;
    attr_flag = MAPATTR_GetAttrFlag( attr );
    if( (attr_flag & MAPATTR_FLAGBIT_WATER) ){ //��
      return( FALSE );
    }
  }
  #endif
  
  //�G���J�E���g�m���擾
  per = enc_GetAttrPercent( enc, attr, &enc_loc );
  
  if( per == 0 ){
    return( FALSE ); //�m��0
  }
  
  { //�莝���|�P�����擾
  }
  
  { //ENC_FLD_SPA�쐬
    enc_SetSpaStruct(
        enc, GAMEDATA_GetMyPokemon(enc->gdata), data, &fld_spa );
  }

  { //�X�v���[�`�F�b�N
  }
  
  { //�����ɂ��m���ύX
  }
  
  { //�r�[�h���ɂ��m���ύX
  }
  
  { //�����A�C�e���i����߂̂��ӂ��j�ɂ��m���ϓ�
  }
  
  if( enc_CheckEncount(enc,per,attr) == TRUE ){ //�G���J�E���g�`�F�b�N
    ret = TRUE;
  }
  
  { //�h�ꑐ�G���J�E���g�`�F�b�N
  }
  
  if( ret == FALSE ){
    return( FALSE );
  }
  
  { //�A���������
    companion = FALSE;
  }

  { //�ړ��|�P�����`�F�b�N
  }
  
  { //�o�g���p�����^�̍쐬
  }
  
  { //�莝���|�P�����ȂǃZ�b�g
  }
  
  ret = FALSE;
  setup = &enc->btl_setup_param;
  
  if( enc_loc == ENCOUNT_LOCATION_GROUND ) //��
  { //���G���J�E���g�f�[�^�쐬
    int i; 
    BOOL book_get;
    
		for( i = 0; i < ENC_MONS_NUM_NORMAL; i++ ){
			enc_data[i].MonsNo = data->NormalEnc[i].MonsterNo;
			enc_data[i].LvMax = data->NormalEnc[i].Level;
			enc_data[i].LvMin = data->NormalEnc[i].Level;
    }
    
    //�S���}�Ӄt���O
    book_get = FALSE;
     
    //����e�[�u����������
    //��ʔ����e�[�u����������
		//���R�e�[�u����������
		//AGB�X���b�g�e�[�u����������
    
    if( companion != FALSE ){ //�A�����
      //�܂�
    }else{ //�ʏ�ΐ�
      ret = enc_SetEncountData(
          enc, setup, &fld_spa, enc_data, enc_loc, HEAPID_BTLPARAM );
    }
  }
  else if( enc_loc == ENCOUNT_LOCATION_WATER ) //��
  { //���G���J�E���g�f�[�^�쐬
    int i;
    for( i = 0; i < ENC_MONS_NUM_SEA; i++ ){
			enc_data[i].MonsNo = data->EncSea[i].MonsterNo;
			enc_data[i].LvMax = data->EncSea[i].MaxLevel;
			enc_data[i].LvMin = data->EncSea[i].MinLevel;
    }
     
    ret = enc_SetEncountData(
          enc, setup, &fld_spa, enc_data, enc_loc, HEAPID_BTLPARAM );
  }
  else //�G���[
  {
    GF_ASSERT( 0 && "ENCOUNT LOCATION ERROR" );
    return( FALSE );
  }
  
  if( ret == FALSE ){ //�G���J�E���g����
    enc_FreeBattleSetupParam( setup );
  }
  
  // �����J�E���g�N���A
  enc_ClearWalkCount( enc );
  
  return( ret );
}

//======================================================================
//  �T�u�@�G���J�E���g�m��
//======================================================================
//--------------------------------------------------------------
/**
 * �G���J�E���g�����A�g���r���[�g����擾
 * @param enc FIELD_ENCOUNT*
 * @param attr �`�F�b�N����MAPATTR
 * @param outEncLocation �ɃG���J�E���g�ꏊ���i�[����ENCOUNT_LOCATION
 * @retval u32 �G���J�E���g�� 0=�G���J�E���g����
 */
//--------------------------------------------------------------
static u32 enc_GetAttrPercent( FIELD_ENCOUNT *enc,
    MAPATTR attr, ENCOUNT_LOCATION *outEncLocation )
{
  u32 per = 0;
  MAPATTR_FLAG attr_flag;
  
  attr_flag = MAPATTR_GetAttrFlag( attr );
  
#ifdef DEBUG_ENCOUNT_CHECKOFF_ATTR
  attr_flag |= MAPATTR_FLAGBIT_ENCOUNT;
#endif
  
  if( (attr_flag & MAPATTR_FLAGBIT_ENCOUNT) ){ //�G���J�E���g�t���O
    const ENCOUNT_DATA *data = enc_GetEncountData( enc );
    
    if( (attr_flag & MAPATTR_FLAGBIT_WATER) ){ //��
      *outEncLocation = ENCOUNT_LOCATION_WATER;
      per = data->EncProbSea;
    }else{ //��
      *outEncLocation = ENCOUNT_LOCATION_GROUND;
      per = data->EncProbGround;
    }
  }
  
  return( per );
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
static BOOL enc_CheckEncount(
    FIELD_ENCOUNT *enc, u32 per, const MAPATTR attr )
{
  u32 calc_per,next_per;
  
  if( per > 100 ){ //100%
    per = 100;
  }
  
  calc_per = per << CALC_SHIFT;
  
  if( enc_CheckEncountWalk(enc,calc_per) == FALSE ){
    enc_AddWalkCount( enc ); //�����J�E���g
    
    if( enc_GetPercentRand() >= WALK_NEXT_PERCENT ){ //5%�Ŏ��̏�����
      return( FALSE );
    }
  }
  
  next_per = NEXT_PERCENT; //�G���J�E���g��{�m��
  
  { //�m���ϓ�
    MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
    
    if( MAPATTR_VALUE_CheckLongGrass(val) == TRUE ){
      next_per += LONG_GRASS_PERCENT; //�������̏ꍇ��+30%
    }else{
      #ifndef FENCOUNT_PL_NULL
		  if ( Player_FormGet(fsys->player) == HERO_FORM_CYCLE ){
			    next_per += CYCLE_PERCENT; //���]�Ԃ̏ꍇ��+30%
      }
      #endif
    }
  }
    
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
  
  return( FALSE );
}

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
static void enc_CreateBattleParam(
    FIELD_ENCOUNT *enc,
    BATTLE_SETUP_PARAM *param, HEAPID heapID,
    u32 monsNo, u32 lv, u32 id )
{
  GAMESYS_WORK *gsys = enc->gsys;
  GAMEDATA *gdata = enc->gdata;
  
  { //�e�ݒ�
    param->engine = BTL_ENGINE_ALONE;
    param->rule = BTL_RULE_SINGLE;
    param->competitor = BTL_COMPETITOR_WILD;
    param->netHandle = NULL;
    param->commMode = BTL_COMM_NONE;
    param->commPos = 0;
    param->netID = 0;
  }

  { //�v���C���[�p�[�e�B�ǉ�
    param->partyPlayer = PokeParty_AllocPartyWork( heapID );
    PokeParty_Copy( GAMEDATA_GetMyPokemon(gdata), param->partyPlayer );
  }
  
  { //��̖ڂ̃|�P�����ǉ�
    param->partyEnemy1 = PokeParty_AllocPartyWork( heapID );
    
    //MonsNo 0 ���Ώ�
    if( monsNo == 0 ){
#ifdef DEBUG_ONLY_FOR_kagaya
      GF_ASSERT( 0 );
#endif
      monsNo = 513;
    }
    
    enc_AddPartyPokemon( param->partyEnemy1, monsNo, lv, id, heapID );
  }
  
  { //2vs2���̖���AI�i�s�v�Ȃ�null�j
    param->partyPartner = NULL;
  }

  { //2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j
    param->partyEnemy2 = NULL;
  }
  
  { //�v���C���[�X�e�[�^�X
    param->statusPlayer = SaveData_GetMyStatus( SaveControl_GetPointer() );
  }
  
  { //BGM�ݒ�
    //�f�t�H���g����BGM�i���o�[
//  param->musicDefault = SEQ_WB_BA_TEST_250KB;
    param->musicDefault = SEQ_VS_NORAPOKE;
    //�s���`����BGM�i���o�[
    param->musicPinch = SEQ_WB_BA_PINCH_TEST_150KB;
  }
}

//--------------------------------------------------------------
/**
 * PP�ǉ�
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void enc_AddPartyPokemon(
    POKEPARTY *party, u32 monsNo, u32 lv, u32 id, HEAPID heapID ) 
{
  POKEMON_PARAM *pp = PP_Create( monsNo, lv, id, heapID );
  PokeParty_Add( party, pp );
  GFL_HEAP_FreeMemory( pp );
}

//--------------------------------------------------------------
/**
 * �o�g���Z�b�g�A�b�v�J��
 * @param
 * @retval
 */
//--------------------------------------------------------------
static const void enc_FreeBattleSetupParam( BATTLE_SETUP_PARAM *param)
{
  if (param->partyPlayer)
  {
    GFL_HEAP_FreeMemory(param->partyPlayer);
  }
  if (param->partyPartner)
  {
    GFL_HEAP_FreeMemory(param->partyPartner);
  }
  if (param->partyEnemy1)
  {
    GFL_HEAP_FreeMemory(param->partyEnemy1);
  }
  if (param->partyEnemy2)
  {
    GFL_HEAP_FreeMemory(param->partyEnemy2);
  }
}

//--------------------------------------------------------------
/**
 * �����_���|�P�����Z�b�g�i���ނ�/����/���A�j
 * @param nothing
 * @return u8 �G���J�E���g�f�[�^�̃e�[�u���ԍ�
 */
//--------------------------------------------------------------
static u32 enc_GetRandWildGroundPokeTblNo( void )
{
	u32	i;
  
	i = enc_GetPercentRand();
#if 0
	if( i < 20 )			return	0;		// 20%
	if( i >= 20 && i < 40 )	return	1;		// 20%
	if( i >= 40 && i < 50 )	return	2;		// 10%
	if( i >= 50 && i < 60 )	return	3;		// 10%
	if( i >= 60 && i < 70 )	return	4;		// 10%
	if( i >= 70 && i < 80 )	return	5;		// 10%
	if( i >= 80 && i < 85 )	return	6;		//  5%
	if( i >= 85 && i < 90 )	return	7;		//  5%
	if( i >= 90 && i < 94 )	return	8;		//  4%
	if( i >= 94 && i < 98 )	return	9;		//  4%
	if( i == 98 )			return	10;		//  1%
	return	11;								//  1%
#else
  //���� �V�|�P��S�Č�����ׂɖ����
  i %= ENC_MONS_NUM_NORMAL;
  return( i );
#endif
}

//--------------------------------------------------------------
/**
 * �����_���|�P�����Z�b�g�i����j
 * @param nothing
 * @return u8 �G���J�E���g�f�[�^�̃e�[�u���ԍ�
 */
//--------------------------------------------------------------
static u32 enc_GetRandWildWaterPokeTblNo( void )
{
	u32	i;
  
	i = enc_GetPercentRand();

	if( i < 60 )			return	0;		// 60%
	if( i >= 60 && i < 90 )	return	1;		// 30%
	if( i >= 90 && i < 95 )	return	2;		//  5%
	if( i >= 95 && i < 99 )	return	3;		//  4%
	return	4;								//  1%
}

typedef enum
{
	FISHINGROD_BAD,			///<�ڂ�̂肴��
	FISHINGROD_GOOD,		///<�����肴��
	FISHINGROD_GREAT,		///<�������肴��
}FISHINGROD_TYPE;

//--------------------------------------------------------------
/**
 * �����_���|�P�����Z�b�g�i�ނ�j
 * @param	inFishingRod		�ފ�
 * @return u8 �G���J�E���g�f�[�^�̃e�[�u���ԍ�		
 */
//--------------------------------------------------------------
static u8 enc_GetRandFishingPokeTblNo( const FISHINGROD_TYPE inFishingRod )
{
	u8	i;
	u8	p = 0;

	i = enc_GetPercentRand();
   
	switch( inFishingRod ){
	case FISHINGROD_BAD:		// �{���̒ފ�
		if		( i < 60 )	p = 0;	// 60%
		else if ( i < 90 )	p = 1;	// 30%
		else if ( i < 95 )	p = 2;	// 5%
		else if ( i < 99 )	p = 3;	// 4%	
		else				p = 4;	// 1%
		break;
	case FISHINGROD_GOOD:		// �ǂ��ފ�
		if		( i < 40 )	p = 0;	// 40%
		else if ( i < 80 )	p = 1;	// 40%
		else if ( i < 95 )	p = 2;	// 15%
		else if ( i < 99 )	p = 3;	// 4%
		else				p = 4;	// 1%
		break;
	case FISHINGROD_GREAT:		// �����ފ�
		if		( i < 40 )	p = 0;	// 40%
		else if ( i < 80 )	p = 1;	// 40%
		else if ( i < 95 )	p = 2;	// 15%
		else if ( i < 99 )	p = 3;	// 4%
		else				p = 4;	// 1%
		break;
	default:
		GF_ASSERT(0&&"unknown fishing rod");
	}

	return	p;
}

//--------------------------------------------------------------
/**
 * ���x���ݒ�
 * @param	inData  ���ʃG���J�E���g�f�[�^�e�[�u��
 * @param	inFldSpa  �����`�F�b�N�E�t�H�����ύX�p�\���̂ւ̃|�C���^
 * @return	u8  ���x��
 */
//--------------------------------------------------------------
static u8 enc_GetEncountPokeLv(
    const ENC_COMMON_DATA *inData, const ENC_FLD_SPA *inFldSpa )
{
	u8	size;
	u8	plus;
	u8	min, max;
	u8	spa;

	// Max Lv �� Min Lv ��菬�����ꍇ�̏C������
	if( inData->LvMax >= inData->LvMin ){
		min = inData->LvMin;
		max = inData->LvMax;
	}else{
		min = inData->LvMax;
		max = inData->LvMin;
	}
  
	size = max - min + 1;		// �␳�T�C�Y
	plus = GFUser_GetPublicRand( size );	// �␳�l
  
	//�u��邫�v�u�v���b�V���[�v�u�͂肫��v�̓��������`�F�b�N
  //���x���̍����|�P�����ƂT���ŃG���J�E���g
	// �擪�̃|�P�������^�}�S�łȂ�
#if 0
	if( inFldSpa->Egg == 0 ){
		if( (inFldSpa->Spa == TOKUSYU_HARIKIRI)||
			(inFldSpa->Spa == TOKUSYU_YARUKI)||
			(inFldSpa->Spa == TOKUSYU_PURESSYAA) ){
			// 1/2�œ�������
			if( gf_p_rand(2) == 0 ){
				return 	( min + plus );		// ����������
			}
			return	( max );	//���������B���x����max�ɂ���
		}
	}
#endif
	return	( min + plus );		// �G���J�E���g���x��
}

//--------------------------------------------------------------
/**
 * 
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
static u8 enc_GetMaxLvMonsTblNo(
    const ENC_COMMON_DATA *inEncCommonData,
    const ENC_FLD_SPA *inFldSpa, const u8 inTblNo)
{

	if( inFldSpa->Egg == 0 ){
		//��邫�A�͂肫��A�v���b�V���[
    //���������`�F�b�N���x���̍����|�P�����ƂT���ŃG���J�E���g
#if 0
		if( (inFldSpa->Spa == TOKUSYU_YARUKI) ||
        (inFldSpa->Spa == TOKUSYU_HARIKIRI) ||
        (inFldSpa->Spa == TOKUSYU_PURESSYAA) )
    {
			if( (enc_GetPercentRand() % 50) ) //5���Ŕ���
      {
        u8 i;
        u8 tbl_no = inTblNo; //�e�[�u���ԍ��ۑ�
       
	  		//���������X�^�[�i���o�[�̃e�[�u��������
  			for( i = 0; i < ENC_MONS_NUM_NORMAL; i++ )
        {
	  			if( inEncCommonData[i].MonsNo ==  //���������X�^�[�i���o�[���H
              inEncCommonData[tbl_no].MonsNo ){
	  				if( inEncCommonData[i].LvMax >  //���x����r
                inEncCommonData[tbl_no].LvMax ){
				  		tbl_no = i; //�e�[�u���ԍ��̍X�V
			  		}
			  	}
			  }
			  return tbl_no;
      }
		}
#endif
	}
  
	return inTblNo;
}

//--------------------------------------------------------------
/**
 * �G���J�E���g�f�[�^�Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL enc_SetEncountData(
    FIELD_ENCOUNT *enc,
    BATTLE_SETUP_PARAM *param,
    const ENC_FLD_SPA *inFldSpa,
    const ENC_COMMON_DATA *enc_data, u32 location, HEAPID heapID )
{
  BOOL result = FALSE;
  u32 lv = 0, no = 0;
  
  switch( location )
  {
  case ENCOUNT_LOCATION_GROUND:
    { //����傭�����ɂ��A�͂��˃^�C�v�̃G���J�E���g���̌���
    }
    
    { //�����ł񂫓����ɂ��A�ł񂫃^�C�v�̃G���J�E���g���̌���
    }
    
    if( result == FALSE ){
      no = enc_GetRandWildGroundPokeTblNo();
    }
    
		//�B�������ɂ��ő僌�x���|�P�����I�o(�n��)
    no = enc_GetMaxLvMonsTblNo( enc_data, inFldSpa, no );
    lv = enc_data[no].LvMax;
    break;
  case ENCOUNT_LOCATION_WATER:
    { //����傭�����ɂ��A�͂��˃^�C�v�̃G���J�E���g���̌���
    }

    { //����傭�����ɂ��A�͂��˃^�C�v�̃G���J�E���g���̌���
    }

    if( result == FALSE ){
			//�����ɂ��w��^�C�v�Ƃ̃G���J�E���g�Ɏ��s�B�ʏ�̃G���J�E���g
      no = enc_GetRandWildWaterPokeTblNo();
    }
    
    lv = enc_GetEncountPokeLv( &enc_data[no], inFldSpa );
    break;
  case ENCOUNT_LOCATION_FISHING:
    { //����傭�����ɂ��A�͂��˃^�C�v�̃G���J�E���g���̌���
    }

    { //����傭�����ɂ��A�͂��˃^�C�v�̃G���J�E���g���̌���
    }
    
    if( result == FALSE ){
			//�����ɂ��w��^�C�v�Ƃ̃G���J�E���g�Ɏ��s�B�ʏ�̃G���J�E���g
      no = enc_GetRandWildWaterPokeTblNo();
    }
    
    lv = enc_GetEncountPokeLv( &enc_data[no], inFldSpa );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  { //�����ɂ�郌�x�����퓬���
  }
  
  { //�X�v���[�`�F�b�N
  }
  
  enc_CreateBattleParam(
      enc, param, heapID, enc_data[no].MonsNo, lv, inFldSpa->TrainerID );
  return( TRUE );
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

//--------------------------------------------------------------
/**
 * �G���J�E���g�f�[�^�擾 ��
 * @param
 * @retval
 */
//--------------------------------------------------------------
static const ENCOUNT_DATA * enc_GetEncountData( FIELD_ENCOUNT *enc )
{
  return( &dataTestEncountData );
}

//--------------------------------------------------------------
/**
 * �����A�X�v���[�`�F�b�N�Ȃǂ̏����W�v���č\���̂ɃZ�b�g
 * @param	fsys			�t�B�[���h�V�X�e���|�C���^
 * @param	inPokeParam		�����̃|�P�����p����
 * @param	inData			�G���J�E���g�f�[�^
 * @param	outSpa			���\���̃|�C���^
 * @return	none
 */
//--------------------------------------------------------------
static void enc_SetSpaStruct(
    FIELD_ENCOUNT *enc,
    const POKEPARTY *party,
    const ENCOUNT_DATA* inData, ENC_FLD_SPA *outSpa  )
{
	//�擪�|�P�����̃^�}�S�`�F�b�N
#if 0
	if( PokeParaGet( inPokeParam, ID_PARA_tamago_flag, NULL ) == 0 ){
		// �擪�̃|�P�������^�}�S�łȂ�
		outSpa->Egg = 0;
    // ����\�͎擾
		outSpa->Spa = PokeParaGet(inPokeParam, ID_PARA_speabino, NULL);
	}else{
		// �擪�̃|�P�������^�}�S
		outSpa->Egg = 1;
		outSpa->Spa = TOKUSYU_MAX;
	}
#else
  {
    outSpa->Egg = 0; //�^�}�S�ł͂Ȃ�
		outSpa->Spa = 0; //��������
  }
#endif
  
	outSpa->SpMyLv = 0;
	outSpa->SprayCheck = FALSE;
	outSpa->EncCancelSpInvalid = FALSE;
	
	if( inData != NULL ){
		//�V�[�E�V�E�V�[�h���S�t�H�����ύX�m���Z�b�g
		outSpa->FormProb[0] = inData->FormProb[0];	//�V�[�E�V
		outSpa->FormProb[1] = inData->FormProb[1];	//�V�[�h���S
		
    //�A���m�[��
		if (inData->AnnoonTable != 0){
			GF_ASSERT( inData->AnnoonTable <= 8 && "annoon_table_over" );
			//�A���m�[���e�[�u���^�C�v�Z�b�g
			outSpa->AnnoonTblType = inData->AnnoonTable-1;
		}else{
			outSpa->AnnoonTblType = 0;
		}
	}
  
	//�g���[�i�[�h�c�̃Z�b�g
#if 0
	outSpa->TrainerID = MyStatus_GetID( SaveData_GetMyStatus(fsys->savedata) );
#else
  outSpa->TrainerID = 3539; //kari
#endif
}

//--------------------------------------------------------------
/**
 * �����J�E���g�擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
static int enc_GetWalkCount( FIELD_ENCOUNT *enc )
{
  return( GAMEDATA_GetFieldMapWalkCount(enc->gdata) );
}

//--------------------------------------------------------------
/**
 * �����J�E���g����
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void enc_AddWalkCount( FIELD_ENCOUNT *enc )
{
  int count = GAMEDATA_GetFieldMapWalkCount( enc->gdata );
  if( count < WALK_COUNT_MAX ){ count++; }
  GAMEDATA_SetFieldMapWalkCount( enc->gdata, count );
}

//--------------------------------------------------------------
/**
 * �����J�E���g�N���A
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void enc_ClearWalkCount( FIELD_ENCOUNT *enc )
{
  GAMEDATA_SetFieldMapWalkCount( enc->gdata, 0 );
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
//  ���G���J�E���g�f�[�^  
//--------------------------------------------------------------
static const ENCOUNT_DATA dataTestEncountData =
{
  30, //�G���J�E���g��
  { //�ʏ�G���J�E���g�|�P����
    {
      5,  //���x��
      494,//�s���{�[
    },
    {
      5,  //���x��
      495,//���O�����[
    },
    {
      5,  //���x��
      496,//�S���_���}
    },
    {
      5,  //���x��
      497,//���j���K�l
    },
    {
      5,  //���x��
      498,//���j�O���X
    },
/*
    {
      5,  //���x��
      499,//���j�o�[��
    },
*/
    {
      5,  //���x��
      500,//�n�S�����V
    },
    {
      5,  //���x��
      501,//�n�L�V�[�h
    },
/*
    {
      5,  //���x��
      502,//�I�m�b�N�X
    },
*/
    {
      5,  //���x��
      503,//�J�[�����g
    },
/*
    {
      5,  //���x��
      504,//�o���r�[�i
    },
*/
    {
      5,  //���x��
      505,//�o�g���[�h
    },
    {
      5,  //���x��
      506,//�v�����X
    },
    {
      5,  //���x��
      507,//�R�o�g
    },
    {
      5,  //���x��
      508,//���C�u��
    },
    {
      5,  //���x��
      509,//�o�N�p�N
    },
    {
      5,  //���x��
      510,//�����R�l
    },
    {
      5,  //���x��
      511,//�I�[�o�g
    },
    {
      5,  //���x��
      512,//�p���`�[�k
    },
    {
      5,  //���x��
      513,//���R�E����
    },
  },

  { //��ʔ���
    5, 5,
  },
  { //���G���J�E���g
    5, 5,
  },
  { //��G���J�E���g
    5, 5,
  },
  { //�h�ꑐ
    0, 0, 0, 0,
  },
  
  { //�t�H�����ύX�m��
    0,0,0,0,0,
  },
  
  0, //�A���m�[���o���e�[�u��
  
  { //AGB�X���b�g ���r�[�G���J�E���g�f�[�^
    0, 0,
  },
  { //AGB�X���b�g �T�t�@�C�A�G���J�E���g�f�[�^
    0, 0, 
  },
  { //AGB�X���b�g �G�������h�G���J�E���g�f�[�^
    0, 0,
  },
  { //AGB�X���b�g �ԃG���J�E���g�f�[�^
    0, 0,
  },
  { //AGB�X���b�g �΃G���J�E���g�f�[�^
    0, 0,
  },
  
  30, //�C�G���J�E���g��
  { //�C�G���J�E���g�|�P����
    {
      10,
      5,
      494,//�s���{�[
    },
    {
      10,
      5,
      495,//���O�����[
    },
    {
      10,
      5,
      496,//�S���_���}
    },
    {
      10,
      5,
      497,//���j���K�l
    },
    {
      10,
      5,
      498,//���j�O���X
    },
  },
  
  30, //��ӂ��G���J�E���g��
  { //��ӂ��G���J�E���g�|�P����
    {
      10,
      5,
      494,//�s���{�[
    },
    {
      10,
      5,
      495,//���O�����[
    },
    {
      10,
      5,
      496,//�S���_���}
    },
    {
      10,
      5,
      497,//���j���K�l
    },
    {
      10,
      5,
      498,//���j�O���X
    },
  },
  
  30, //�{���ފƃG���J�E���g��
  { //�{���ފƃG���J�E���g�|�P����
    {
      10,
      5,
      494,//�s���{�[
    },
    {
      10,
      5,
      495,//���O�����[
    },
    {
      10,
      5,
      496,//�S���_���}
    },
    {
      10,
      5,
      497,//���j���K�l
    },
    {
      10,
      5,
      498,//���j�O���X
    },
  },
  
  30, //�����ފƃG���J�E���g��
  { //�����ފƃG���J�E���g�|�P����
    {
      10,
      5,
      494,//�s���{�[
    },
    {
      10,
      5,
      495,//���O�����[
    },
    {
      10,
      5,
      496,//�S���_���}
    },
    {
      10,
      5,
      497,//���j���K�l
    },
    {
      10,
      5,
      498,//���j�O���X
    },
  },
  
  30, //�����ފƃG���J�E���g��
  { //�����ފƃG���J�E���g�|�P����
    {
      10,
      5,
      494,//�s���{�[
    },
    {
      10,
      5,
      495,//���O�����[
    },
    {
      10,
      5,
      496,//�S���_���}
    },
    {
      10,
      5,
      497,//���j���K�l
    },
    {
      10,
      5,
      498,//���j�O���X
    },
  },
};
