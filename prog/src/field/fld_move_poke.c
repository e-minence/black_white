//============================================================================================
/**
 * @file	fld_move_poke.c
 * @brief	�t�B�[���h�ړ��|�P�����֘A����
 * @author	Nozomu Saito
 * @date	06.05.13
 */
//============================================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/rtc_tool.h"

#include "arc/fieldmap/zone_id.h"
#include "savedata/encount_sv.h"
#include "savedata/mystatus.h"

#include "battle/battle.h"
#include "gamesystem/btl_setup.h"
#include "poke_tool/poketype.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "move_pokemon_def.h"
#include "move_pokemon.h"
#include "fld_move_poke.h"

static void MovePokeLocation(ENC_SV_PTR inEncData);
static void JumpMovePokeAffterBattle(ENC_SV_PTR inEncData, const int inZoneID);
static MPD_PTR GetMovePokeDataByMonsNo(ENC_SV_PTR inEncData, const int inMonsNo);

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
void FLD_MP_SetAfterBattle(GAMEDATA * gdata, BATTLE_SETUP_PARAM *bsp)
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
			JumpMovePokeAffterBattle( enc_sv, zone_id );
		}
    return;
  }
  //�ړ��|�P�����Ƃ̃G���J�E���g

	move_poke_id = MP_MonsNoToMovePokeID(monsno);

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
		JumpMovePokeAffterBattle( enc_sv, zone_id);
  }
}

//-----------------------------------------------------------------------------
/**
 * �퓬��̈ړ��|�P�����W�����v
 *
 * @param	inEncData			�G���J�E���g�f�[�^�|�C���^
 * @param	inZoneID			�]�[��ID
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
static void JumpMovePokeAffterBattle(ENC_SV_PTR inEncData, const int inZoneID)
{
	int zone;
	u8 i;
	for(i=0;i<MOVE_POKE_MAX;i++){
		//�ړ����ŁA���A��l���Ɠ����]�[���ɂ��邩�H
		if ( EncDataSave_IsMovePokeValid(inEncData, i) ){
			zone = MP_GetMovePokeZone(EncDataSave_GetMovePokeZoneIdx(inEncData, i));
			if (inZoneID == zone){
				//�W�����v
				MP_JumpMovePokemon(inEncData, i);
			}
		}
	}
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

