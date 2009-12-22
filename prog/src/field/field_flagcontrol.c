//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_flagcontrol.h
 *	@brief  �t�B�[���h�@�t���O�Ǘ�
 *	@author	tomoya takahashi
 *	@date		2009.11.19
 *
 *	���W���[�����FFIELD_FLAGCONT
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"


#include "field/zonedata.h"
#include "field_status_local.h"
#include "move_pokemon.h"
#include "effect_encount.h"
#include "field_flagcontrol.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  �t���O�������F�����ă]�[�����܂������Ƃ�  (�K���Ă΂��)
 *
 *	@param	gdata �Q�[���f�[�^
 *	@param  fieldWork �t�B�[���h�}�b�v���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_FLAGCONT_INIT_WalkStepOver(GAMEDATA * gdata, FIELDMAP_WORK* fieldWork)
{
	//�ړ��|�P��������
	{
		ENC_SV_PTR data;
		data = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
		MP_UpdatePlayerZoneHist(data, FIELDMAP_GetZoneID( fieldWork ));		//�]�[���������X�V
    
    //�����Ĉړ������ꍇ�͈ړ��|�P�����̈ړ�����
		MP_MovePokemonNeighboring( data );//�ړ��|�P�����אڈړ�
	}
  
  { //�C�x���g�t���O
    EVENTWORK *event_flag = GAMEDATA_GetEventWork( gdata );
    EVENTWORK_ResetEventFlag( event_flag, SYS_FLAG_KAIRIKI );
  }
  
/* PLATUNUM
	//--�`���m�[�g�p�X�V����
	FldFlgInit_FnoteTownDataSet( fsys );

	//���]�ԃQ�[�g�ʉ߂����t���O�̃N���A(�Q�[�g���ł̂ݗL��)
	SysFlag_BicycleGatePassage( SaveData_GetEventWork(fsys->savedata), SYSFLAG_MODE_RESET );

	//�����肫�t���O�̃N���A
	SysFlag_KairikiReset( SaveData_GetEventWork(fsys->savedata) );
	//�����t���O�̃Z�b�g
	ARRIVEDATA_SetArriveFlag(fsys, fsys->location->zone_id);
	//�r�[�h���N���A
	EncDataSave_SetVidro( EncDataSave_GetSaveDataPtr(fsys->savedata), VIDRO_NONE );
	//�G���J�E���g�����N���A
	fsys->encount.walk_count = 0;
	//�T�t�@���ɂ��Ȃ��Ƃ��̏���
	if ( !SysFlag_SafariCheck(SaveData_GetEventWork(fsys->savedata)) ){
	}
*/
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���O�������F���C�v�̓���}�b�v�W�����v�̂Ƃ�  (�K���Ă΂��)
 *
 *	@param	fsys  �t�B�[���h�}�b�v���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_FLAGCONT_INIT_MapJump(GAMEDATA * gdata, u16 zone_id)
{
  FIELD_STATUS * fldstatus = GAMEDATA_GetFieldStatus( gdata );

	//�_���W�����łȂ��ꍇ�A�t���b�V����ԃt���O�Ɩ�������ԃt���O���N���A
	if (!ZONEDATA_IsDungeon(zone_id)) 
  {
    // �t���b�V��OFF
		FIELD_STATUS_SetFieldSkillFlash(fldstatus, FALSE);
	}
	//�ړ��|�P��������
	{
		ENC_SV_PTR data;
		data = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
		MP_UpdatePlayerZoneHist(data, zone_id);		//�]�[���������X�V
	  //�ړ��|�P�����S���W�����v
	  MP_JumpMovePokemonAll( data );
	}

  { //�C�x���g�t���O
    EVENTWORK *eventwork = GAMEDATA_GetEventWork( gdata );
    EVENTWORK_ResetEventFlag( eventwork, SYS_FLAG_KAIRIKI );
  }


/* PLATINUM
	if (fsys->scope_mode_flag == TRUE) {
		//�]�������[�h�̏ꍇ�͍X�V�������s��Ȃ�
		return;
	}

	//--�`���m�[�g�p�X�V����
	if( FldFlgInit_FnoteTownDataSet( fsys ) == FALSE ){
		FldFlgInit_FnoteMapChgDataSet( fsys );
	}

	//���]�ԃQ�[�g�ʉ߂����t���O�̃N���A(�Q�[�g���ł̂ݗL��)
	SysFlag_BicycleGatePassage( SaveData_GetEventWork(fsys->savedata), SYSFLAG_MODE_RESET );

	//�T�C�N�����O���[�h���[�h�̃N���A
	SysFlag_CyclingRoadReset( SaveData_GetEventWork(fsys->savedata) );
	//�����肫�t���O�̃N���A
	SysFlag_KairikiReset( SaveData_GetEventWork(fsys->savedata) );
	//�����t���O�̃Z�b�g
	ARRIVEDATA_SetArriveFlag(fsys, fsys->location->zone_id);
	//�r�[�h���N���A
	EncDataSave_SetVidro( EncDataSave_GetSaveDataPtr(fsys->savedata), VIDRO_NONE );
	//�G���J�E���g�����N���A
	fsys->encount.walk_count = 0;
	//�_���W�����łȂ��ꍇ�A�t���b�V����ԃt���O�Ɩ�������ԃt���O���N���A
	if (!ZoneData_IsDungeon(fsys->location->zone_id)) {
		EVENTWORK * ev = SaveData_GetEventWork(fsys->savedata);
		SysFlag_FlashReset(ev);
		SysFlag_KiribaraiReset(ev);
	}
	//���@��Ԃ̐ݒ�
	{
		PLAYER_SAVE_DATA * jikisave = Situation_GetPlayerSaveData(SaveData_GetSituation(fsys->savedata));
		if (jikisave->form == HERO_FORM_CYCLE
				&& ZoneData_GetEnableBicycleFlag(fsys->location->zone_id) == FALSE) {
			jikisave->form = HERO_FORM_NORMAL;
		} else if (jikisave->form == HERO_FORM_SWIM) {
			jikisave->form = HERO_FORM_NORMAL;
		}
	}

	//--�^�E���}�b�v����
	if (ZoneData_IsSinouField(fsys->location->zone_id)) {
		//�t�B�[���h�ȊO�ł͑��Ղ̍X�V�͂Ȃ��I
		TOWN_MAP_FOOTMARK * footmark;
		footmark = Situation_GetTMFootMark(SaveData_GetSituation(fsys->savedata));
		TMFootMark_SetNoDir(footmark, fsys->location->grid_x, fsys->location->grid_z);
	}
//*/
}

//----------------------------------------------------------------------------
/**
 * @brief	�t���O�������F����Ԃ��g�����Ƃ��ŗL�ōs������
 *
 * @param	fsys		�t�B�[���h�V�X�e���|�C���^
 *
 */
//-----------------------------------------------------------------------------

void FIELD_FLAGCONT_INIT_FlySky(GAMEDATA * gdata, u16 zone_id)
{
  //�G�t�F�N�g�G���J�E���g�@�����J�E���^�[�N���A��
  EFFECT_ENC_MapChangeUpdate( gdata );
  
/*  PLATINUM
	//�T�t�@���t���O�N���A
	SysFlag_SafariReset( SaveData_GetEventWork(fsys->savedata) );
*/
}

//----------------------------------------------------------------------------
/**
 * @brief	�t���O�������F�e���|�[�g���g�����Ƃ��ŗL�ōs������
 *
 * @param	fsys		�t�B�[���h�V�X�e���|�C���^
 */
//-----------------------------------------------------------------------------
void FIELD_FLAGCONT_INIT_Teleport(GAMEDATA * gdata, u16 zone_id)
{
  //�G�t�F�N�g�G���J�E���g�@�����J�E���^�[�N���A��
  EFFECT_ENC_MapChangeUpdate( gdata );
/*  PLATINUM
	//�T�t�@���t���O�N���A
	SysFlag_SafariReset( SaveData_GetEventWork(fsys->savedata) );
*/
}

//----------------------------------------------------------------------------
/**
 * @brief	�t���O�������F���Ȃʂ��̂Ђ��E���Ȃ��ق���g�����Ƃ��ŗL�ōs������
 *
 * @param	fsys		�t�B�[���h�V�X�e���|�C���^
 */
//-----------------------------------------------------------------------------

void FIELD_FLAGCONT_INIT_Escape(GAMEDATA * gdata, u16 zone_id)
{
  //�G�t�F�N�g�G���J�E���g�@�����J�E���^�[�N���A��
  EFFECT_ENC_MapChangeUpdate( gdata );
/*  PLATINUM
	//�T�t�@���t���O�N���A
	SysFlag_SafariReset( SaveData_GetEventWork(fsys->savedata) );
*/
}


//----------------------------------------------------------------------------
/**
 * @brief	�t���O�������F�Q�[���I�[�o�[
 *
 * @param	fsys		�t�B�[���h�V�X�e���|�C���^
 */
//-----------------------------------------------------------------------------
void FIELD_FLAGCONT_INIT_GameOver(GAMEDATA * gdata, u16 zone_id)
{
  //�G�t�F�N�g�G���J�E���g�@�����J�E���^�[�N���A��
  EFFECT_ENC_MapChangeUpdate( gdata );
/*  PLATINUM
	EVENTWORK* ev = SaveData_GetEventWork( fsys->savedata );

	//�A������t���O�A�g���[�i�[ID�i�[���[�N�̃N���A
	SysFlag_PairReset( ev );
	SysWork_PairTrainerIDSet( ev, 0 );
//*/
}


//----------------------------------------------------------------------------
/**
 * @brief	�t���O�������F�R���e�B�j���[
 *
 * @param	fsys		�t�B�[���h�V�X�e���|�C���^
 */
//-----------------------------------------------------------------------------
void FIELD_FLAGCONT_INIT_Continue(GAMEDATA * gdata, u16 zone_id)
{
  //�ړ��|�P��������
	{
		ENC_SV_PTR data = EncDataSave_GetSaveDataPtr( GAMEDATA_GetSaveControlWork( gdata ) );
	  //�ړ��|�P�����S���W�����v
	  MP_JumpMovePokemonAll( data );
	}
}

