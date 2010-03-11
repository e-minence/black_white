//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_seatemple.c
 *	@brief  �C���Ձ@�C�x���g�N��
 *	@author	tomoya takahashi 
 *	@date		2010.03.11
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "event_seatemple.h"

#include "savedata/situation.h"
#include "script.h"
#include "fieldmap.h"

#include "../../../resource/fldmapdata/script/seatemple_scr_def.h"


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�����N���X�N���v�gID
//=====================================
typedef struct
{
  u16 step;
  u16 scrid;
} STEP_DO_SCRIPT_ID;

static const STEP_DO_SCRIPT_ID sc_STEP_DO_SCRIPT_ID[] = {
  { 0, SCRID_STEP_000 },
  { 100, SCRID_STEP_100 },
  { 300, SCRID_STEP_300 },
  { 450, SCRID_STEP_450 },
  { 500, SCRID_STEP_500 },
};

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
 *	@brief  �C��_�a�@�����C�x���g�N��
 *
 *	@param	gsys        �Q�[���V�X�e��
 *	@param	fieldmap    �t�B�[���h�}�b�v
 *
 *	@return �C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_SeaTemple_GetStepEvent( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap )
{
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK* saveData = GAMEDATA_GetSaveControlWork(gdata);
  SITUATION* situation = SaveData_GetSituation( saveData );
  u16 count;
  int i;

  count = Situation_GetSeaTempleStepCount( situation );

  for( i=0; i<NELEMS(sc_STEP_DO_SCRIPT_ID); i++ ){

    if( sc_STEP_DO_SCRIPT_ID[i].step == count ){
      return SCRIPT_SetEventScript( gsys, sc_STEP_DO_SCRIPT_ID[i].scrid, NULL, FIELDMAP_GetHeapID( fieldmap ));

    }
  }

  return NULL;
}



