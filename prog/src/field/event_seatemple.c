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

#include "sound/pm_sndsys.h"

#include "event_seatemple.h"
#include "event_mapchange.h"

#include "savedata/situation.h"
#include "script.h"
#include "fieldmap.h"

#include "field_task.h"
#include "field_task_manager.h"
#include "field_task_player_rot.h"

#include "field_task_player_drawoffset.h"
#include "field_diving_data.h"  //DIVINGSPOT_GetZoneID

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

#define SEATEMPLE_STEP_MAX (500)
static const STEP_DO_SCRIPT_ID sc_STEP_DO_SCRIPT_ID[] = {
  { 0,   SCRID_SEATEMPLE_STEP_000 },
  { 100, SCRID_SEATEMPLE_STEP_100 },
  { 300, SCRID_SEATEMPLE_STEP_300 },
  { 450, SCRID_SEATEMPLE_STEP_450 },
  { SEATEMPLE_STEP_MAX, SCRID_SEATEMPLE_STEP_500 },
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

  //BTS:6673 �J�E���g��500���傫���Ƃ��́A��ɍŌ�̃C�x���g�𔭓�
  if( count > SEATEMPLE_STEP_MAX ){
    return SCRIPT_SetEventScript( gsys, SCRID_SEATEMPLE_STEP_500, NULL, FIELDMAP_GetHeapID( fieldmap ));
  }

  return NULL;
}



//-----------------------------------------------------------------------------
/**
 *					Diving�J�ڃC�x���g
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	Diving�J�ڃ��[�N
//=====================================
typedef struct {
  GAMESYS_WORK* p_gsys;
  FIELDMAP_WORK* p_fieldmap;

  BOOL kurukuru;
  s32 timing;
} EV_DIVING_WORK;

//-------------------------------------
///	���@�����艉�o
//=====================================
#define DIVE_HERO_Y        (-64<<FX32_SHIFT)
#define DIVE_HERO_FRAME    (40)

//-------------------------------------
///	SE�Đ��^�C�~���O
//=====================================
#define DIVE_SE_TIMING  ( 8 )

//-------------------------------------
///	�t�F�[�h�J�n�^�C�~���O
//=====================================
#define DIVE_FADE_START_TIMING  ( 24 )

//----------------------------------------------------------------------------
/**
 *	@brief  Diving�@���~�C�x���g
 *
 *  @retval GMEVENT_RES_CONTINUE = 0, ///<�C�x���g�p����
 *  @retval GMEVENT_RES_FINISH,       ///<�C�x���g�I��
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_DivingDown( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EV_DIVING_WORK* p_wk = p_work;
  

  switch( (*p_seq) ){

  // ���@��������
  case 0:
    {
      FIELD_TASK* p_move;
      FIELD_TASK_MAN* p_task_man = FIELDMAP_GetTaskManager( p_wk->p_fieldmap );
      VecFx32 vec;
      
      VEC_Set( &vec, 0, DIVE_HERO_Y, 0 );

      p_move     = FIELD_TASK_TransDrawOffset( p_wk->p_fieldmap, DIVE_HERO_FRAME, &vec );
      FIELD_TASK_MAN_AddTask( p_task_man, p_move, NULL );

      p_wk->timing = DIVE_SE_TIMING;
      
      (*p_seq)++;
    }
    break;

  // �^�C�~���O���݂āASE�Đ�
  case 1:
    p_wk->timing --;
    if( p_wk->timing <= 0 ){
      PMSND_PlaySE( SEQ_SE_FLD_123 );
      (*p_seq)++;
    }
    break;

  case 2:
    if( PMSND_CheckPlaySE_byPlayerID( PMSND_GetSE_DefaultPlayerID(SEQ_SE_FLD_123) ) == FALSE )
    {
      // ���@���_�C�r���O��Ԃɂ���
      {
        FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
        const MMDL* cp_mmdl = FIELD_PLAYER_GetMMdl( p_player );
        
        FIELD_PLAYER_SetMoveForm( p_player, PLAYER_MOVE_FORM_DIVING );
      }
      
      // �}�b�v�W�����v
      {
        u16 connect_zone_id;
        BOOL result;
        GMEVENT * p_sub_event;
        result = DIVINGSPOT_Check( p_wk->p_fieldmap, &connect_zone_id );
        GF_ASSERT( result );
        p_sub_event = EVENT_ChangeMapBySeaTempleDown( p_wk->p_gsys, 
            connect_zone_id );
        GMEVENT_CallEvent(p_event, p_sub_event);
      }
      (*p_seq)++;
    }
    break;

  case 3:
    {
      FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
      MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_player );
      // ������
      MMDL_SetAcmd( p_mmdl, AC_WALK_U_16F );
      (*p_seq)++;
    } 
    break;


  // �����҂�
  case 4:
    {
      FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
      MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_player );
      if( MMDL_CheckEndAcmd( p_mmdl ) == FALSE ){
        break;
      }
    }
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Diving�@�㏸�C�x���g
 *
 *  @retval GMEVENT_RES_CONTINUE = 0, ///<�C�x���g�p����
 *  @retval GMEVENT_RES_FINISH,       ///<�C�x���g�I��
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_DivingUp( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EV_DIVING_WORK* p_wk = p_work;
  FIELD_TASK_MAN* p_taskMan  = FIELDMAP_GetTaskManager( p_wk->p_fieldmap );
  

  switch( (*p_seq) ){

    // �N���N���J�n
  case 0:
    // SE�Đ�
    PMSND_PlaySE( SEQ_SE_FLD_123 );
    p_wk->timing = DIVE_FADE_START_TIMING;
    if( p_wk->kurukuru ){
      FIELD_TASK* rot_up;
      FIELD_TASK* rot;
      FIELD_FOG_WORK* p_fog = FIELDMAP_GetFieldFog( p_wk->p_fieldmap );

      rot_up   = FIELD_TASK_PlayerRotate_SpeedUp( p_wk->p_fieldmap, 20, 3 );
      rot      = FIELD_TASK_PlayerRotate( p_wk->p_fieldmap, 12, 3 );

      FIELD_TASK_MAN_AddTask( p_taskMan, rot_up, NULL );
      FIELD_TASK_MAN_AddTask( p_taskMan, rot, rot_up );

      // �t�H�O���������B
      FIELD_FOG_FADE_Init( p_fog, 32527, FIELD_FOG_GetSlope(p_fog), DIVE_FADE_START_TIMING+2 );
    }
    (*p_seq)++;
    break;

  case 1:

    // ���@��Ԃ�g���ɖ߂�
    {
      FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
      FIELD_PLAYER_SetMoveForm( p_player, PLAYER_MOVE_FORM_SWIM );
    }
    (*p_seq)++;
    break;

  case 2:
    p_wk->timing --;
    if( p_wk->timing <= 0 )
    {
      // �}�b�v�W�����v
      GMEVENT* p_sub_event;
      p_sub_event = EVENT_ChangeMapBySeaTempleUp( p_wk->p_gsys );

      GMEVENT_CallEvent(p_event, p_sub_event);
      (*p_seq)++;
    }
    break;

  // �����҂�
  case 3:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Diving�@���~�C�x���g
 *
 *	@param	gsys        �Q�[���V�X�e��
 *	@param	fieldmap    �t�B�[���h�}�b�v
 *
 *	@return ���~�C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_SeaTemple_GetDivingDownEvent( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap )
{
  GMEVENT* p_event;
  EV_DIVING_WORK* p_wk;
  
  p_event = GMEVENT_Create( gsys, NULL, EVENT_DivingDown, sizeof(EV_DIVING_WORK) );


  p_wk = GMEVENT_GetEventWork( p_event );
  p_wk->p_gsys      = gsys;
  p_wk->p_fieldmap  = fieldmap;
  p_wk->kurukuru = FALSE;

  return p_event;
}


//----------------------------------------------------------------------------
/**
 *	@brief  Diving�@�㏸�@�C�x���g
 *
 *	@param	gsys          �Q�[���V�X�e��
 *	@param	fieldmap      �t�B�[���h�}�b�v
 *  @param  kurukuru      �N���N��
 * 
 *	@return �㏸�C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_SeaTemple_GetDivingUpEvent( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BOOL kurukuru )
{
  GMEVENT* p_event;
  EV_DIVING_WORK* p_wk;
  
  p_event = GMEVENT_Create( gsys, NULL, EVENT_DivingUp, sizeof(EV_DIVING_WORK) );


  p_wk = GMEVENT_GetEventWork( p_event );
  p_wk->p_gsys      = gsys;
  p_wk->p_fieldmap  = fieldmap;
  p_wk->kurukuru    = kurukuru;

  return p_event;
}



