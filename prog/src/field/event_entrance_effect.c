//============================================================================================
/**
 * @file event_entrance_effect.c
 * @brief
 * @date  2009.06.13
 * @author  tamada GAME FREAK inc.
 *
 */
//============================================================================================

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/field_const.h"

#include "fieldmap.h"
#include "field_player.h"
#include "field_buildmodel.h"

#include "event_mapchange.h"        //MAPCHANGE_setPlayerVanish
#include "event_fieldmap_control.h" //EVENT_FieldFadeOut
#include "event_fldmmdl_control.h"  //EVENT_PlayerOneStepAnime
#include "event_entrance_effect.h"

#include "event_camera_act.h" 
#include "field_bgm_control.h"


//============================================================================================
//============================================================================================
static GMEVENT_RESULT FieldDoorAnimeEvent(GMEVENT * event, int *seq, void * work);

static void makeRect(FLDHIT_RECT * rect, const VecFx32 * pos);
static void getPlayerFrontPos(FIELDMAP_WORK * fieldmap, VecFx32 * pos);
static G3DMAPOBJST * searchDoorObject(FIELD_BMODEL_MAN * bmodel_man, const VecFx32 * pos);


//============================================================================================
//============================================================================================
enum {
  //�A�j���f�[�^�̒�`���Ɉˑ����Ă���B
  ANM_INDEX_DOOR_OPEN = 0,
  ANM_INDEX_DOOR_CLOSE,
};

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
  //�ȉ��̓��[�N�������ɃZ�b�g�����
  GAMESYS_WORK * gsys;  //<<<�Q�[���V�X�e���ւ̃|�C���^
  LOCATION loc_req;     //<<<���̃}�b�v���w�����P�[�V�����w��iBGM�t�F�[�h�����̌W���j
  VecFx32 pos;          //<<<�h�A����������ꏊ

  //�ȉ��̓V�[�P���X���쒆�ɃZ�b�g�����
  FIELD_BMODEL * entry;
  G3DMAPOBJST * obj;
}FIELD_DOOR_ANIME_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT ExitEvent_DoorOut(GMEVENT * event, int *seq, void * work)
{
  enum {
    SEQ_DOOROUT_INIT = 0,
    SEQ_DOOROUT_OPENANIME_START,
    SEQ_DOOROUT_CAMERA_ACT,
    SEQ_DOOROUT_OPENANIME_WAIT,
    SEQ_DOOROUT_PLAYER_STEP,
    SEQ_DOOROUT_CLOSEANIME_START,
    SEQ_DOOROUT_CLOSEANIME_WAIT,
    SEQ_DOOROUT_END,
  };
	FIELD_DOOR_ANIME_WORK * fdaw = work;
	GAMESYS_WORK  * gsys = fdaw->gsys;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  FLDMAPPER * fldmapper = FIELDMAP_GetFieldG3Dmapper(fieldmap);
  FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager(fldmapper);

  switch (*seq)
  {
  case SEQ_DOOROUT_INIT:
    // �t�F�[�h�C������O��, �J�����̏����ݒ���s��
    EVENT_CAMERA_ACT_PrepareForDoorOut( fieldmap );
    //���@������
    MAPCHANGE_setPlayerVanish( fieldmap, TRUE );
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, fieldmap, 0));
    *seq = SEQ_DOOROUT_OPENANIME_START;
    break;

  case SEQ_DOOROUT_OPENANIME_START:
    fdaw->obj = searchDoorObject(bmodel_man, &fdaw->pos);
    if (fdaw->obj == NULL)
    { /* �G���[�悯�A�h�A���Ȃ��ꍇ */
      *seq = SEQ_DOOROUT_CAMERA_ACT;
      break;
    } 
    fdaw->entry = FIELD_BMODEL_Create( bmodel_man, fdaw->obj );
    FIELD_BMODEL_MAN_EntryBuildModel( bmodel_man, fdaw->entry );
    G3DMAPOBJST_changeViewFlag(fdaw->obj, FALSE);
    FIELD_BMODEL_SetAnime( fdaw->entry, ANM_INDEX_DOOR_OPEN, BMANM_REQ_START);
    *seq = SEQ_DOOROUT_CAMERA_ACT;
    break;

  case SEQ_DOOROUT_CAMERA_ACT:
    EVENT_CAMERA_ACT_CallDoorOutEvent( event, gsys, fieldmap );
    if (fdaw->obj == NULL)
    { /* �G���[�悯�A�h�A���Ȃ��ꍇ */
      *seq = SEQ_DOOROUT_PLAYER_STEP;
      break;
    } 
    *seq = SEQ_DOOROUT_OPENANIME_WAIT;
    break;

  case SEQ_DOOROUT_OPENANIME_WAIT:
    {
      if ( FIELD_BMODEL_GetAnimeStatus( fdaw->entry, ANM_INDEX_DOOR_OPEN) == TRUE)
      {
        FIELD_BMODEL_SetAnime( fdaw->entry, ANM_INDEX_DOOR_OPEN, BMANM_REQ_STOP);
        *seq = SEQ_DOOROUT_PLAYER_STEP;
        break;
      }
    }
    break;
  case SEQ_DOOROUT_PLAYER_STEP:
    EVENT_CAMERA_ACT_ResetCameraParameter( fieldmap );  // �J�����̐ݒ���f�t�H���g�ɖ߂�
    //���@�o���A����ړ��A�j��
    MAPCHANGE_setPlayerVanish( fieldmap, FALSE );
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gsys, fieldmap) );
    *seq = SEQ_DOOROUT_CLOSEANIME_START;
    break;

  case SEQ_DOOROUT_CLOSEANIME_START:
    if (fdaw->obj == NULL)
    { /* �G���[�悯�A�h�A���Ȃ��ꍇ */
      *seq = SEQ_DOOROUT_END;
      break;
    }
    //�h�A�����A�j���K�p
    {
      FIELD_BMODEL_SetAnime( fdaw->entry, ANM_INDEX_DOOR_OPEN, BMANM_REQ_END);
      FIELD_BMODEL_SetAnime( fdaw->entry, ANM_INDEX_DOOR_CLOSE, BMANM_REQ_START);
    }
    *seq = SEQ_DOOROUT_CLOSEANIME_WAIT;
    break;

  case SEQ_DOOROUT_CLOSEANIME_WAIT:
    {
      if (FIELD_BMODEL_GetAnimeStatus( fdaw->entry, ANM_INDEX_DOOR_OPEN) == TRUE)
      {
        FIELD_BMODEL_SetAnime( fdaw->entry, ANM_INDEX_DOOR_CLOSE, BMANM_REQ_END);
        *seq = SEQ_DOOROUT_END;
        break;
      }
    }
  case SEQ_DOOROUT_END:
    if (fdaw->entry)
    {
      FIELD_BMODEL_MAN_releaseBuildModel( bmodel_man, fdaw->entry );
      FIELD_BMODEL_Delete( fdaw->entry );
      fdaw->entry = NULL;
    }
    if (fdaw->obj)
    {
      G3DMAPOBJST_changeViewFlag(fdaw->obj, TRUE);
      fdaw->obj = NULL;
    }
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �h�A����łĂ����A�̉��o�C�x���g
 * @param gsys
 * @param fieldmap
 * @param loc
 * @return  GMEVENT
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldDoorOutAnime( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap )
{
  GMEVENT * event;
  FIELD_DOOR_ANIME_WORK * fdaw;
  event = GMEVENT_Create( gsys, NULL,ExitEvent_DoorOut, sizeof(FIELD_DOOR_ANIME_WORK) ); 

  fdaw = GMEVENT_GetEventWork( event );
  fdaw->gsys = gsys;
  //fdaw->loc_req = * loc;
  getPlayerFrontPos(fieldmap, &fdaw->pos);

  fdaw->entry = NULL;
  fdaw->obj = NULL;
  
  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT ExitEvent_DoorIn(GMEVENT * event, int *seq, void * work)
{
  enum {
    SEQ_DOORIN_OPENANIME_START = 0,
    SEQ_DOORIN_CAMERA_ACT,
    SEQ_DOORIN_OPENANIME_WAIT,
    SEQ_DOORIN_PLAYER_ONESTEP,
    SEQ_DOORIN_FADEOUT,
    SEQ_DOORIN_END,
  };

	FIELD_DOOR_ANIME_WORK * fdaw = work;
	GAMESYS_WORK  * gsys = fdaw->gsys;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  FLDMAPPER * fldmapper = FIELDMAP_GetFieldG3Dmapper(fieldmap);
  FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager(fldmapper);

  switch (*seq)
  {
  case SEQ_DOORIN_OPENANIME_START:
    fdaw->obj = searchDoorObject(bmodel_man, &fdaw->pos);
    if (fdaw->obj == NULL)
    { /* �G���[�悯�A�h�A���Ȃ��ꍇ */
      *seq = SEQ_DOORIN_CAMERA_ACT;
      break;
    } 
    fdaw->entry = FIELD_BMODEL_Create( bmodel_man, fdaw->obj );
    FIELD_BMODEL_MAN_EntryBuildModel( bmodel_man, fdaw->entry );
    G3DMAPOBJST_changeViewFlag(fdaw->obj, FALSE);
    FIELD_BMODEL_SetAnime( fdaw->entry, ANM_INDEX_DOOR_OPEN, BMANM_REQ_START);
    *seq = SEQ_DOORIN_CAMERA_ACT;
    break;

  case SEQ_DOORIN_CAMERA_ACT: 
    EVENT_CAMERA_ACT_CallDoorInEvent( event, gsys, fieldmap ); 
    if (fdaw->obj == NULL)
    { /* �G���[�悯�A�h�A���Ȃ��ꍇ */
      *seq = SEQ_DOORIN_PLAYER_ONESTEP;
      break;
    } 
    *seq = SEQ_DOORIN_OPENANIME_WAIT;
    break;

  case SEQ_DOORIN_OPENANIME_WAIT:
    if ( FIELD_BMODEL_GetAnimeStatus( fdaw->entry, ANM_INDEX_DOOR_OPEN) == TRUE)
    {
      FIELD_BMODEL_SetAnime( fdaw->entry, ANM_INDEX_DOOR_OPEN, BMANM_REQ_STOP);
      *seq = SEQ_DOORIN_PLAYER_ONESTEP;
      break;
    }
    break;

  case SEQ_DOORIN_PLAYER_ONESTEP:
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gsys, fieldmap) );
    *seq = SEQ_DOORIN_FADEOUT;
    break;

  case SEQ_DOORIN_FADEOUT:
    FIELD_BGM_CONTROL_FadeOut( gamedata, fdaw->loc_req.zone_id, 30 );
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut(gsys, fieldmap, 0) );
    *seq = SEQ_DOORIN_END;
    break;

  case SEQ_DOORIN_END:
    EVENT_CAMERA_ACT_ResetCameraParameter( fieldmap );  // �J�����̐ݒ���f�t�H���g�ɖ߂�
    if (fdaw->entry)
    {
      FIELD_BMODEL_MAN_releaseBuildModel( bmodel_man, fdaw->entry );
      FIELD_BMODEL_Delete( fdaw->entry );
      fdaw->entry = NULL;
    }
    if (fdaw->obj)
    {
      G3DMAPOBJST_changeViewFlag(fdaw->obj, TRUE);
      fdaw->obj = NULL;
    }
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �h�A�ɓ����A�̉��o�C�x���g
 * @param gsys
 * @param fieldmap
 * @param loc
 * @return  GMEVENT
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldDoorInAnime
( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, const LOCATION * loc )
{
  GMEVENT * event;
  FIELD_DOOR_ANIME_WORK * fdaw;
  event = GMEVENT_Create( gsys, NULL, ExitEvent_DoorIn, sizeof(FIELD_DOOR_ANIME_WORK) ); 

  fdaw = GMEVENT_GetEventWork( event );
  fdaw->gsys = gsys;
  fdaw->loc_req = * loc;
  getPlayerFrontPos(fieldmap, &fdaw->pos);

  fdaw->entry = NULL;
  fdaw->obj = NULL;
  
  return event;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void getPlayerFrontPos(FIELDMAP_WORK * fieldmap, VecFx32 * pos)
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldmap);
  u16 dir;

  FIELD_PLAYER_GetPos(player, pos);
  {
    MMDL *fmmdl = FIELD_PLAYER_GetMMdl( player );
    dir = MMDL_GetDirDisp( fmmdl );
  }
	switch( dir ) {
	case DIR_UP:		pos->z -= FIELD_CONST_GRID_FX32_SIZE; break;
	case DIR_DOWN:	pos->z += FIELD_CONST_GRID_FX32_SIZE; break;
	case DIR_LEFT:	pos->x -= FIELD_CONST_GRID_FX32_SIZE; break;
	case DIR_RIGHT:	pos->x += FIELD_CONST_GRID_FX32_SIZE; break;
	default:
                  GF_ASSERT(0);
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void makeRect(FLDHIT_RECT * rect, const VecFx32 * pos)
{
  enum { RECT_SIZE = (FIELD_CONST_GRID_SIZE * 2) << FX32_SHIFT };
  rect->top = pos->z - RECT_SIZE;
  rect->bottom = pos->z + RECT_SIZE;
  rect->left = pos->x - RECT_SIZE;
  rect->right = pos->x + RECT_SIZE;
}

//------------------------------------------------------------------
/**
 * @brief �w��ʒu�t�߂̃h�A�z�u���f������������
 * @param bmodel_man
 * @param pos
 * @return NULL   ������Ȃ�����
 * @return G3DMAPOBJST *  �������h�A�z�u���f���ւ̎Q��
 */
//------------------------------------------------------------------
static G3DMAPOBJST * searchDoorObject(FIELD_BMODEL_MAN * bmodel_man, const VecFx32 * pos)
{
  G3DMAPOBJST * entry = NULL;
  G3DMAPOBJST ** array;
  u32 result_num;

  {
    //������`���쐬
    FLDHIT_RECT rect;
    makeRect(&rect, pos);
    //��`�͈͓��̔z�u���f�����X�g�𐶐�����
    array = FIELD_BMODEL_MAN_CreateObjStatusList(bmodel_man, &rect, &result_num);
  }
  {
    int i;
    for (i = 0; i < result_num; i++)
    {
      if (FIELD_BMODEL_MAN_G3DMAPOBJSTisDoor(bmodel_man, array[i]) == TRUE)
      {
        //�擾�����z�u���f�����X�g����A�h�A�ł��邩�`�F�b�N
        entry = array[i];
        break;
      }
    }
  }
  //��`�͈͓��̔z�u���f�����X�g���������
  GFL_HEAP_FreeMemory(array);
  return entry;
}




