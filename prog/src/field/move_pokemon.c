//============================================================================================
/**
 * @file	move_pokemon.c
 * @brief	�ړ��|�P�����֘A����
 * @author	Nozomu Saito -> Miyuki Iwasawa
 * @date	06.05.11-09.12.14
 */
//============================================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/rtc_tool.h"

#include "arc/fieldmap/zone_id.h"
#include "savedata/encount_sv.h"
#include "savedata/mystatus.h"
#include "savedata/zukan_savedata.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "battle/battle.h"
#include "gamesystem/btl_setup.h"

#include "field/weather_no.h"
#include "move_pokemon_def.h"
#include "move_pokemon.h"

#ifdef PM_DEBUG
//#include "field_common.h"
#endif

//#include "debug_saito.h"


/////////////////////////////////////////////////////////////////
//���e������`
#define	BRANCH_MAX		(6)

#define MOVE_POKE_RAIKAMI_LV	(40)
#define MOVE_POKE_KAZAKAMI_LV		(40)

typedef struct MP_LOC_DATA_tag
{
	u16 BranchNum;
	u16 ZoneIdx[BRANCH_MAX];
}MP_LOC_DATA;

typedef struct MP_TIME_LOC_DATA
{
  u16 num;
  u16 zone[4];
}MP_TIME_LOC_DATA;

enum{
	ZONE_IDX_R01,		//0
	ZONE_IDX_R02,		//1
	ZONE_IDX_R03,		//2
	ZONE_IDX_R04,		//3
	ZONE_IDX_R05,		//4
	ZONE_IDX_R06,		//5
	ZONE_IDX_R07,		//6
	ZONE_IDX_R08,		//7
	ZONE_IDX_R09,		//8
	ZONE_IDX_R10,		//9
	ZONE_IDX_R11,		//10
	ZONE_IDX_R12,		//11
	ZONE_IDX_R13,		//12
	ZONE_IDX_R14,		//13
	ZONE_IDX_R15,		//14
	ZONE_IDX_R16,		//15
	ZONE_IDX_R18,		//16
  ZONE_IDX_MAX,
  ZONE_IDX_HIDE = ZONE_IDX_MAX,
};

static const u16 DATA_ZoneTbl[MVPOKE_LOCATION_MAX+1] = {
	//
	ZONE_ID_R01,		//0
	ZONE_ID_R02,		//1
	ZONE_ID_R03,		//2
	ZONE_ID_R04,		//3
	ZONE_ID_R05,		//4
	ZONE_ID_R06,		//5
	ZONE_ID_R07,		//6
	ZONE_ID_R08,		//7
	ZONE_ID_R09,		//8
	ZONE_ID_R10,		//9
	ZONE_ID_R11,		//10
	ZONE_ID_R12,		//11
	ZONE_ID_R13,		//12
	ZONE_ID_R14,		//13
	ZONE_ID_R15,		//14
	ZONE_ID_R16,		//15
	ZONE_ID_R18,		//16
  MVPOKE_ZONE_NULL, //17
};

static const MP_TIME_LOC_DATA DATA_TimeLocation[] = {
 { 4, { ZONE_IDX_R01, ZONE_IDX_R02, ZONE_IDX_R03, ZONE_IDX_R18 }},
 { 3, { ZONE_IDX_R04, ZONE_IDX_R05, ZONE_IDX_R16, 0 }},
 { 3, { ZONE_IDX_R06, ZONE_IDX_R07, ZONE_IDX_R08, 0 }},
 { 3, { ZONE_IDX_R09, ZONE_IDX_R10, ZONE_IDX_R11, 0 }},
 { 4, { ZONE_IDX_R12, ZONE_IDX_R13, ZONE_IDX_R14, ZONE_IDX_R15 }},
};

////////////////////////////////////////////////////////////////
//�v���g�^�C�v
static inline u16 GetZoneID( u8 zone_idx );
static u8 MonsNoToMovePokeID(u16 monsno);

static void JumpMovePokeTimeLocation( ENC_SV_PTR data, u8 season, const u8 inTargetPoke );

static void UpdateData(	ENC_SV_PTR data,
						const u8 inTargetPoke,
						const u8  inZoneIdx, const int inZone);
static MPD_PTR GetMovePokeDataByMonsNo(ENC_SV_PTR inEncData, const int inMonsNo);

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����W�����v
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�f�[�^�|�C���^
 * @param season      �G��
 * @param	inTarget		�ړ��Ώ�
 *
 * @return
 */
//--------------------------------------------------------------------------------------------
void MP_JumpMovePokemon(ENC_SV_PTR inEncData, u8 season, const u8 inTarget)
{
  MPD_PTR	mpd = EncDataSave_GetMovePokeDataPtr( inEncData, inTarget );
  JumpMovePokeTimeLocation( inEncData, season, inTarget );
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����S���W�����v
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�f�[�^�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void MP_JumpMovePokemonAll( GAMEDATA* gdata )
{
	u8 i,season;
  ENC_SV_PTR enc_sv = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
  season = GAMEDATA_GetSeasonID( gdata );

  for(i=0;i<MOVE_POKE_MAX;i++){
		//�ړ������H
		if ( EncDataSave_IsMovePokeValid(enc_sv, i) ){
			//�W�����v
			MP_JumpMovePokemon(enc_sv, season, i);
		}
	}
}

//-----------------------------------------------------------------------------
/**
 * �ړ��|�P�����̗אڈړ�
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�f�[�^�|�C���^
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void MP_MovePokemonNeighboring( GAMEDATA* gdata )
{
	u8 i, season;
  ENC_SV_PTR enc_sv = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
  season = GAMEDATA_GetSeasonID( gdata );
	
  for(i=0;i<MOVE_POKE_MAX;i++){
    MPD_PTR	mpd = EncDataSave_GetMovePokeDataPtr( enc_sv, i );
		//�ړ������H
		if ( EncDataSave_IsMovePokeValid(enc_sv, i) == FALSE ){
      continue;
    }

		//�W�����v
		MP_JumpMovePokemon(enc_sv, season, i);
	  IWASAWA_Printf("%d:�W�����v���܂�\n",i);
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����]�[���擾
 *
 * @param	  inTarget  �ړ��|�P����ID
 *
 * @retval  MVPOKE_ZONE_NULL  �B��Ă���̂Ŗ���
 * @retval  ����ȊO�@�ړ��|�P����������]�[��ID
 */
//--------------------------------------------------------------------------------------------
u16 MP_GetMovePokeZoneID( ENC_SV_PTR inEncData, const u8 inTarget)
{
	return GetZoneID( EncDataSave_GetMovePokeZoneIdx( inEncData, inTarget) );
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����]�[��Idx���]�[��ID�ɕϊ�
 *
 * @param	  inZoneIdx  �ړ��|�P�����]�[��index
 *
 * @retval  MVPOKE_ZONE_NULL  �B��Ă���̂Ŗ���
 * @retval  ����ȊO�@�ړ��|�P����������]�[��ID
 */
//--------------------------------------------------------------------------------------------
u16 MP_MovePokeZoneIdx2ID( const u8 inZoneIdx )
{
	return GetZoneID( inZoneIdx );
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P���������邩���`�F�b�N
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�f�[�^�|�C���^
 *
 * @return BOOL		TRUE:����	FALSE:���Ȃ�
 */
//--------------------------------------------------------------------------------------------
BOOL MP_CheckMovePoke(ENC_SV_PTR inEncData)
{
	u8 i;
	for(i=0;i<MOVE_POKE_MAX;i++){
		if ( EncDataSave_IsMovePokeValid(inEncData, i) ){
			return TRUE;
		}
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ��l���]�[�������̍X�V
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�f�[�^�|�C���^
 * @param	inZone			�X�V�]�[��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void MP_UpdatePlayerZoneHist(ENC_SV_PTR inEncData, const int inZone)
{
	if (MP_CheckMovePoke(inEncData)){
		//�����X�V
		EncDataSave_UpdatePlayerZoneHist( inEncData, inZone );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �w��ړ��|�P������o�^
 *
 * @param	sv					�Z�[�u�f�[�^
 * @param	inTargetPoke		�Ώۈړ��|�P�����C���f�b�N�X
 * 
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void MP_AddMovePoke( GAMEDATA* gdata, const u8 inTargetPoke)
{
	POKEMON_PARAM *poke_param;
	MPD_PTR	 mpd;
	ENC_SV_PTR enc;
	MYSTATUS * my_st;
  SAVE_CONTROL_WORK* save;

	int monsno;
	u8 lv,move_type;
  
  save = GAMEDATA_GetSaveControlWork( gdata );
	enc = EncDataSave_GetSaveDataPtr( save );
	mpd = EncDataSave_GetMovePokeDataPtr(enc, inTargetPoke);
	
	switch(inTargetPoke){
	case MOVE_POKE_RAIKAMI:
		monsno = MONSNO_649;  //���C�J�~ MONSNO_RAIKAMI;
		lv = MOVE_POKE_RAIKAMI_LV;
    move_type = MVPOKE_TYPE_TIME;
		break;
	case MOVE_POKE_KAZAKAMI:
		monsno = MONSNO_648;  //�J�U�J�~ MONSNO_KAZAKAMI;
		lv = MOVE_POKE_KAZAKAMI_LV;
    move_type = MVPOKE_TYPE_TIME;
		break;
	default:
		GF_ASSERT(0);
		return ;
	}

	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_MONSNO, monsno);
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_LV, lv);
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_MV_TYPE, move_type);
	
	my_st =  GAMEDATA_GetMyStatus( gdata );

	poke_param = GFL_HEAP_AllocClearMemoryLo( HEAPID_WORLD, POKETOOL_GetWorkSize() );
	PP_SetupEx(poke_param, monsno, lv, MyStatus_GetID(my_st), PTL_SETUP_POW_AUTO,  PTL_SETUP_RND_AUTO );

	//��Ԉُ�
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_COND, 0);
	//�G���J�E���g�t���O
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_ENC, 1);
	//�p���[����
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_POW_RND,
											PP_Get( poke_param, ID_PARA_power_rnd, NULL ));
	//������
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_PER_RND,
											PP_Get( poke_param, ID_PARA_personal_rnd, NULL ));
	//����
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_SEIKAKU,
											PP_Get( poke_param, ID_PARA_seikaku, NULL ));
	//����
	//�g�o
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_HP,
											PP_Get( poke_param, ID_PARA_hpmax, NULL ));

  //�����t���O�Z�b�g
  ZUKANSAVE_SetPokeSee( ZUKAN_SAVEDATA_GetZukanSave( save ), poke_param );

	GFL_HEAP_FreeMemory(poke_param);

	//����ړ��|�P�����o���ꏊ����
  MP_JumpMovePokemon( enc, GAMEDATA_GetSeasonID( gdata ), inTargetPoke);
  
  //�X�e�[�^�X�X�V
  EncDataSave_SetMovePokeState( enc, inTargetPoke, MVPOKE_STATE_MOVE );
}

//--------------------------------------------------------------------------------------------
/**
 * �a�����莞�ړ��|�P������������
 *
 * @param	sv					�Z�[�u�f�[�^
 * @param	inTargetPoke		�Ώۈړ��|�P�����C���f�b�N�X
 * 
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void MP_RecoverMovePoke( GAMEDATA* gdata )
{
  int i;
  ENC_SV_PTR enc_sv = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );

  for( i = 0;i < MOVE_POKE_MAX;i++){
    if( EncDataSave_GetMovePokeState( enc_sv, i) == MVPOKE_STATE_DOWN ){
      MP_AddMovePoke( gdata, i);
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * �퓬��̏���
 *
 * @param	gdata			�t�B�[���h�V�X�e���|�C���^
 * @param	bsp	�o�g���p�����|�C���^
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void MP_SetAfterBattle(GAMEDATA * gdata, BATTLE_SETUP_PARAM *bsp)
{
	u16 hp;
	u8 cond;
  u8 move_poke_id;
	u16 monsno;
  u16 zone_id;
	
	ENC_SV_PTR enc_sv;
	MPD_PTR mpd;

	//�ړ��|�P�����Ɛ퓬���������`�F�b�N
  if( bsp->rule != BTL_RULE_SINGLE || bsp->competitor != BTL_COMPETITOR_WILD ){
    return;
  }

  zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork( gdata ));
  {
	  POKEMON_PARAM *poke_param;

	  poke_param = PokeParty_GetMemberPointer( BATTLE_PARAM_GetPokePartyPointer( bsp, BTL_CLIENT_ENEMY1 ), 0);

	  monsno = PP_Get(poke_param, ID_PARA_monsno, NULL);
	  hp = (u16)(PP_Get(poke_param,ID_PARA_hp, NULL));
	  cond = (u8)(PP_Get(poke_param,ID_PARA_condition, NULL));
  }

  enc_sv = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
	mpd = GetMovePokeDataByMonsNo( enc_sv, monsno);
	
	if(mpd == NULL){			//�쐶�|�P�����ƃG���J�E���g
		//30���̊m���ŁA3�C�̈ړ��|�P�����̃W�����v
		if ( GFUser_GetPublicRand0( 100 ) < 30 ){
      MP_JumpMovePokemonAll( gdata );
		}
    return;
  }
  //�ړ��|�P�����Ƃ̃G���J�E���g

	move_poke_id = MonsNoToMovePokeID(monsno);

	//�����Ăg�o��0
	if( (( bsp->result == BTL_RESULT_WIN )||(bsp->result == BTL_RESULT_DRAW)) && (hp == 0) ) {

		//�G���J�E���g�����ړ��|�P�����f�[�^���N���A
		EncDataSave_ClearMovePokeData(&mpd);

		//�|�����Z�b�g
		EncDataSave_SetMovePokeState( enc_sv, move_poke_id, MVPOKE_STATE_DOWN );

	//�ߊl����
	}else if( bsp->result == BTL_RESULT_CAPTURE ) {

		//�G���J�E���g�����ړ��|�P�����f�[�^���N���A
		EncDataSave_ClearMovePokeData(&mpd);

		//�ߊl�����Z�b�g
		EncDataSave_SetMovePokeState( enc_sv, move_poke_id, MVPOKE_STATE_GET );
	}else{
  	//�p�����[�^�̔��f
	  EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_HP, hp);
	  EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_COND, cond);

    //�ړ��|�P�����̃W�����v
    MP_JumpMovePokemonAll( gdata );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����]�[���V��ω��`�F�b�N
 *
 * @param	  zone_id		���݂̃]�[��ID
 *
 * @retval  MVPOKE_ZONE_NULL  �B��Ă���̂Ŗ���
 * @retval  ����ȊO�@�ړ��|�P����������]�[��ID
 */
//--------------------------------------------------------------------------------------------
u16 MP_CheckMovePokeWeather( GAMEDATA * gdata, u16 zone_id )
{
	ENC_SV_PTR enc;
	enc = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
	
  if ( EncDataSave_IsMovePokeValid( enc, MOVE_POKE_RAIKAMI) ){
    if( zone_id == MP_GetMovePokeZoneID(enc,MOVE_POKE_RAIKAMI)){
      return WEATHER_NO_RAIKAMI;
    }
	}
  if( EncDataSave_IsMovePokeValid( enc, MOVE_POKE_KAZAKAMI) ){
    if( zone_id == MP_GetMovePokeZoneID(enc,MOVE_POKE_KAZAKAMI)){
      return WEATHER_NO_KAZAKAMI;
    }
  }
  return WEATHER_NO_NONE; //�ύX�Ȃ� 
}

/////////////////////////////////////////////////////////////////////
//���[�J��

//--------------------------------------------------------------------------------------------
/*
 *  @brief  �]�[��index����]�[��ID���擾
 */
//--------------------------------------------------------------------------------------------
static inline u16 GetZoneID( u8 zone_idx )
{
  if( zone_idx > ZONE_IDX_MAX) {
    GF_ASSERT(0);
    return DATA_ZoneTbl[0];
  }
  return DATA_ZoneTbl[zone_idx];
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����i���o�[���ړ��|�P�����w��ID�ւ̕ϊ�
 *
 * @date	2009.06.24
 * @param	monsno	�ړ��|�P�����̃|�P�����i���o�[
 * @return	u8		�ړ��|�P�����w��ID
 */
//--------------------------------------------------------------------------------------------
static u8 MonsNoToMovePokeID(u16 monsno)
{
	switch (monsno) {
  case MONSNO_649:  //���C�J�~ MONSNO_RAIKAMI:
		return MOVE_POKE_RAIKAMI;
  case MONSNO_648:  //�J�U�J�~ MONSNO_KAZAKAMI:
		return MOVE_POKE_KAZAKAMI;
	}
	GF_ASSERT(0);
	return MOVE_POKE_MAX;
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����o���ꏊ���I(�J�~�V���[�Y)
 *
 * @param	data			�G���J�E���g�֘A�Z�[�u�f�[�^
 * @param	season		�G��
 * @param	inTargetPoke	�Ώۈړ��|�P�����C���f�b�N�X
 *
 * @return
 */
//--------------------------------------------------------------------------------------------
static void JumpMovePokeTimeLocation( ENC_SV_PTR data, u8 season, const u8 inTargetPoke )
{
	u16 move_poke_now_zone, timezone;
  const MP_TIME_LOC_DATA* loc;

	//������������ꏊ�͑ΏۊO�Ƃ���
	move_poke_now_zone = GetZoneID( EncDataSave_GetMovePokeZoneIdx(data,inTargetPoke) );
  //�^�C�����[�P�[�V�����擾
  timezone = PM_RTC_GetTimeZone( season );
  loc = &DATA_TimeLocation[timezone];

  if( move_poke_now_zone != MVPOKE_ZONE_NULL &&
      GFUser_GetPublicRand0(100) < 20){
    //�B���
		UpdateData(	data, inTargetPoke, ZONE_IDX_HIDE, GetZoneID(ZONE_IDX_HIDE));
		IWASAWA_Printf("�B�ꂽ %d TimeZone:%d\n", MVPOKE_ZONE_NULL, timezone);
		return;
  }

  while(1){
	  u8 zone_idx = loc->zone[GFUser_GetPublicRand0(loc->num)];
    u16 zone = GetZoneID( zone_idx );

		if( zone != move_poke_now_zone ){
			UpdateData(	data, inTargetPoke, zone_idx, zone);
			IWASAWA_Printf("%d�փW�����v TimeZone:%d\n",zone, timezone );
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �f�[�^�X�V
 *
 * @param	data			�G���J�E���g�֘A�Z�[�u�f�[�^
 * @param	inTargetPoke	�Ώۈړ��|�P�����C���f�b�N�X
 * @param	inZoneidx		�ړ��]�[���C���f�b�N�X
 * @param	inZone			�]�[���h�c	
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void UpdateData(	ENC_SV_PTR data,
						const u8 inTargetPoke,
						const u8  inZoneIdx, const int inZone)
{
	MPD_PTR mpd;
	mpd = EncDataSave_GetMovePokeDataPtr(data, inTargetPoke);
	//�X�V
	EncDataSave_SetMovePokeZoneIdx(data,inTargetPoke,inZoneIdx);
	//�ړ��|�P�����f�[�^������������
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_ZONE_ID, inZone);
}

//-----------------------------------------------------------------------------
/**
 * �����X�^�[�i���o�[����ړ��|�P�����f�[�^���擾
 *
 * @param	inEncData	�G���J�E���g�f�[�^�|�C���^
 * @param	inMonsNo	�����X�^�[�i���o�[
 *
 * @return	MPD_PTR		�ړ��|�P�����f�[�^�|�C���^ �Ȃ��ꍇ��NULL��Ԃ�
 *
*/ 
//-----------------------------------------------------------------------------
static MPD_PTR GetMovePokeDataByMonsNo(ENC_SV_PTR inEncData, const int inMonsNo)
{
	u8 i;
	MPD_PTR mpd;
	for(i=0;i<MOVE_POKE_MAX;i++){
		if ( EncDataSave_IsMovePokeValid(inEncData, i) ){
			mpd = EncDataSave_GetMovePokeDataPtr(inEncData, i);
			if (inMonsNo == EncDataSave_GetMovePokeDataParam(mpd, MP_PARAM_MONSNO)){
				return mpd;
			}
		}
	}
	return NULL;
}

#ifdef PM_DEBUG
//--------------------------------------------------------------------------------------------
/**
 * �f�[�^�X�V
 *
 * @param	
 *
 * @return
 */
//--------------------------------------------------------------------------------------------
void MP_DebugUpdateData(	ENC_SV_PTR data,
						const u8 inTargetPoke,
						const u8  inZoneIdx)
{
	MPD_PTR mpd;
	mpd = EncDataSave_GetMovePokeDataPtr(data, inTargetPoke);
	//�X�V
	EncDataSave_SetMovePokeZoneIdx(data,inTargetPoke,inZoneIdx);
	//�ړ��|�P�����f�[�^������������
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_ZONE_ID, GetZoneID( inZoneIdx ));
}
#endif
