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
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "battle/battle.h"
#include "gamesystem/btl_setup.h"

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

static const u16 DATA_ZoneTbl[LOCATION_MAX+1] = {
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

static const MP_LOC_DATA MovePokeLocationTbl[LOCATION_MAX] =
{
	{2,{1,15,0xffff,0xffff,0xffff,0xffff}},		//0
	{2,{0,2,0xffff,0xffff,0xffff,0xffff}},		//1
	{3,{1,3,7,0xffff,0xffff,0xffff}},			//2
	{3,{2,4,7,0xffff,0xffff,0xffff}},			//3
	{2,{3,5,0xffff,0xffff,0xffff,0xffff}},		//4
	{2,{4,6,0xffff,0xffff,0xffff,0xffff}},		//5
	{2,{5,7,0xffff,0xffff,0xffff,0xffff}},		//6
	{3,{2,3,8,0xffff,0xffff,0xffff}},			//7
	{3,{7,9,11,0xffff,0xffff,0xffff}},			//8
	{3,{8,10,11,0xffff,0xffff,0xffff}},			//9
	{1,{9,0xffff,0xffff,0xffff,0xffff,0xffff}},	//10
	{4,{8,9,12,13,0xffff,0xffff}},				//11
	{2,{11,13,0xffff,0xffff,0xffff,0xffff}},	//12
	{3,{11,12,15,0xffff,0xffff,0xffff}},		//13
	{2,{13,15,0xffff,0xffff,0xffff,0xffff}},	//14
	{2,{0,14,0xffff,0xffff,0xffff,0xffff}},		//15
	{2,{0,14,0xffff,0xffff,0xffff,0xffff}},		//16
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

static void JumpMovePokeLocation(	ENC_SV_PTR data, const u8 inTargetPoke, const int inPlayerOldZone);
static void JumpMovePokeTimeLocation( ENC_SV_PTR data, const u8 inTargetPoke );

static void MovePokeLocation(ENC_SV_PTR data, const u8 inTargetPoke, const int inPlayerOldZone);
static void UpdateData(	ENC_SV_PTR data,
						const u8 inTargetPoke,
						const u8  inZoneIdx, const int inZone);
static MPD_PTR GetMovePokeDataByMonsNo(ENC_SV_PTR inEncData, const int inMonsNo);

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����W�����v
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�f�[�^�|�C���^
 * @param	inTarget		�ړ��Ώ�
 *
 * @return
 */
//--------------------------------------------------------------------------------------------
void MP_JumpMovePokemon(ENC_SV_PTR inEncData, const u8 inTarget)
{
  MPD_PTR	mpd = EncDataSave_GetMovePokeDataPtr( inEncData, inTarget );

  if(EncDataSave_GetMovePokeDataParam( mpd, MP_PARAM_MV_TYPE ) == MVPOKE_TYPE_NORMAL)
  {
	  int player_old_zone;
	  player_old_zone = EncDataSave_GetPlayerOldZone(inEncData);
	  JumpMovePokeLocation(inEncData, inTarget, player_old_zone);
  }else{
    JumpMovePokeTimeLocation( inEncData, inTarget );
  }
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
void MP_JumpMovePokemonAll(ENC_SV_PTR inEncData)
{
	u8 i;
	for(i=0;i<MOVE_POKE_MAX;i++){
		//�ړ������H
		if ( EncDataSave_IsMovePokeValid(inEncData, i) ){
			//�W�����v
			MP_JumpMovePokemon(inEncData, i);
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
void MP_MovePokemonNeighboring(ENC_SV_PTR inEncData)
{
	u8 i;
	
  for(i=0;i<MOVE_POKE_MAX;i++){
    MPD_PTR	mpd = EncDataSave_GetMovePokeDataPtr( inEncData, i );
		//�ړ������H
		if ( EncDataSave_IsMovePokeValid(inEncData, i) == FALSE ){
      continue;
    }

		//�^�C���ړ���16����1�̊m���ŃW�����v
    if( EncDataSave_GetMovePokeDataParam( mpd, MP_PARAM_MV_TYPE ) == MVPOKE_TYPE_TIME || 
		    GFUser_GetPublicRand0(16) == 0){
			IWASAWA_Printf("%d:�W�����v���܂�\n",i);
			//�W�����v
			MP_JumpMovePokemon(inEncData, i);
		}else{
			IWASAWA_Printf("%d:�אڈړ����܂�\n",i);
			//�אڈړ�
			MovePokeLocation(inEncData, i, EncDataSave_GetPlayerOldZone(inEncData) );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����]�[���擾
 *
 * @param	inIndex		�]�[���e�[�u���C���f�b�N�X
 *
 * @retval  MVPOKE_ZONE_NULL  �B��Ă���̂Ŗ���
 * @retval  ����ȊO�@�ړ��|�P����������]�[��ID
 */
//--------------------------------------------------------------------------------------------
u16 MP_GetMovePokeZone(const u8 inIndex)
{
	return GetZoneID( inIndex );
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
	ENC_SV_PTR data;
	MYSTATUS * my_st;

	int monsno;
	u8 lv,move_type;
	
	data = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
	mpd = EncDataSave_GetMovePokeDataPtr(data, inTargetPoke);
	
	switch(inTargetPoke){
	case MOVE_POKE_RAIKAMI:
		monsno = MONSNO_RAIKAMI;
		lv = MOVE_POKE_RAIKAMI_LV;
    move_type = MVPOKE_TYPE_TIME;
		break;
	case MOVE_POKE_KAZAKAMI:
		monsno = MONSNO_KAZAKAMI;
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
	//�g�o
	EncDataSave_SetMovePokeDataParam(mpd, MP_PARAM_HP,
											PP_Get( poke_param, ID_PARA_hpmax, NULL ));

	GFL_HEAP_FreeMemory(poke_param);

	//����ړ��|�P�����o���ꏊ����
  MP_JumpMovePokemon( data, inTargetPoke);
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
      EncDataSave_SetMovePokeState( enc_sv, i, MVPOKE_STATE_NONE );
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
      MP_JumpMovePokemonAll( enc_sv );
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
    MP_JumpMovePokemonAll( enc_sv );
  }
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
	case MONSNO_RAIKAMI:
		return MOVE_POKE_RAIKAMI;
	case MONSNO_KAZAKAMI:
		return MOVE_POKE_KAZAKAMI;
	}
	GF_ASSERT(0);
	return MOVE_POKE_MAX;
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����o���ꏊ���I(�m�[�}��)
 *
 * @param	data			�G���J�E���g�֘A�Z�[�u�f�[�^
 * @param	inTargetPoke	�Ώۈړ��|�P�����C���f�b�N�X
 * @param	inPlayerZone	���]�[��
 *
 * @return
 */
//--------------------------------------------------------------------------------------------
static void JumpMovePokeLocation(ENC_SV_PTR data, const u8 inTargetPoke, const int inPlayerOldZone)
{
	u8 zone_idx;
	int move_poke_now_zone;
	int zone;
	
  //��l�����O�X�񂢂��]�[����
	//������������ꏊ�͑ΏۊO�Ƃ���
	move_poke_now_zone = GetZoneID( EncDataSave_GetMovePokeZoneIdx(data,inTargetPoke));
	
  while(1){
    zone_idx =  GFUser_GetPublicRand0( LOCATION_MAX);
		zone = GetZoneID( zone_idx );
		if ((zone != inPlayerOldZone)&&(zone != move_poke_now_zone)){
			UpdateData(	data, inTargetPoke, zone_idx, zone);
			IWASAWA_Printf("%d�փW�����v player_old_zone:%d\n",zone,inPlayerOldZone);
			break;
		}
	}	
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����o���ꏊ���I(�J�~�V���[�Y)
 *
 * @param	data			�G���J�E���g�֘A�Z�[�u�f�[�^
 * @param	inTargetPoke	�Ώۈړ��|�P�����C���f�b�N�X
 * @param	inPlayerZone	���]�[��
 *
 * @return
 */
//--------------------------------------------------------------------------------------------
static void JumpMovePokeTimeLocation( ENC_SV_PTR data, const u8 inTargetPoke )
{
	u16 move_poke_now_zone;
  const MP_TIME_LOC_DATA* loc;

	//������������ꏊ�͑ΏۊO�Ƃ���
	move_poke_now_zone = GetZoneID( EncDataSave_GetMovePokeZoneIdx(data,inTargetPoke) );
  //�^�C�����[�P�[�V�����擾
  loc = &DATA_TimeLocation[GFL_RTC_GetTimeZone()];

  if( move_poke_now_zone != MVPOKE_ZONE_NULL &&
      GFUser_GetPublicRand0(100) < 20){
    //�B���
		UpdateData(	data, inTargetPoke, ZONE_IDX_HIDE, GetZoneID(ZONE_IDX_HIDE));
		IWASAWA_Printf("�B�ꂽ %d TimeZone:%d\n", MVPOKE_ZONE_NULL,GFL_RTC_GetTimeZone());
		return;
  }

  while(1){
	  u8 zone_idx = loc->zone[GFUser_GetPublicRand0(loc->num)];
    u16 zone = GetZoneID( zone_idx );

		if( zone != move_poke_now_zone ){
			UpdateData(	data, inTargetPoke, zone_idx, zone);
			IWASAWA_Printf("%d�փW�����v TimeZone:%d\n",zone,GFL_RTC_GetTimeZone());
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��|�P�����אڈړ�
 *
 * @param	data			�G���J�E���g�֘A�Z�[�u�f�[�^
 * @param	inTargetPoke	�Ώۈړ��|�P�����C���f�b�N�X
 * @param	inPlayerZone	���]�[��	
 *
 * @return
 */
//--------------------------------------------------------------------------------------------
static void MovePokeLocation(ENC_SV_PTR data, const u8 inTargetPoke, const int inPlayerOldZone)
{
	u8 zone_idx;
	int zone;
	const MP_LOC_DATA *loc_data;
	loc_data = &(MovePokeLocationTbl[EncDataSave_GetMovePokeZoneIdx(data,inTargetPoke)]);
	
	if (loc_data->BranchNum == 1){	//�ψڐ悪1�����Ȃ��ꍇ
		zone_idx = loc_data->ZoneIdx[0];
		zone = GetZoneID( zone_idx );
		if (zone == inPlayerOldZone){	//��l���̑O�X��̃]�[����������A�W�����v
			IWASAWA_Printf("��l���̋����W�������̂ŃW�����v�ɕύX\n");
			JumpMovePokeLocation(data, inTargetPoke, inPlayerOldZone);
		}else{
			IWASAWA_Printf("%d:%d�ɗאڈړ�\n",inTargetPoke,zone);
			UpdateData(	data, inTargetPoke, zone_idx, zone);
		}
	}else{							//�ψڐ悪��������ꍇ
		u8 idx;
		while(1){
			//�אڏꏊ�������_���Ō���
			idx = GFUser_GetPublicRand0(loc_data->BranchNum);
			zone_idx = loc_data->ZoneIdx[idx];
			zone = GetZoneID( zone_idx );
			//��l���̑O�X�񂢂��]�[���ȊO��I�o���Ĉړ�
			if (zone != inPlayerOldZone){
				IWASAWA_Printf("%d:%d�ɗאڈړ�\n",inTargetPoke,zone);
				UpdateData(	data, inTargetPoke, zone_idx, zone);
				break;
			}
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
