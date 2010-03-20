//======================================================================
/**
 * @file  fskill_sorawotobu.c
 * @brief  ������Ƃ�
 * @author  Saito
 * @date  2009.11.19
 */
//======================================================================

#include <gflib.h>
#include "system/gfl_use.h"

#include "system/main.h"  //HEAPID_PROC
#include "fieldmap.h"
//#include "weather.h"

#include "field_skill.h"
#include "eventwork.h"

//#include "script.h"
#include "fskill_sorawotobu.h"
//#include "fldmmdl.h"

#include "fld3d_ci.h"
#include "event_mapchange.h"
#include "event_fieldmap_control.h"
#include "field/zonedata.h"    //for ZONEDATA_�`

#include "../../../resource/fld3d_ci/fldci_id_def.h"

#define FLYSKY_CAM_MOVE_FRAME (10)

typedef struct SORAWOTOBU_WORK_tag{
  GAMESYS_WORK* gsys;
//  GAMEDATA* gdata;
 
  const VecFx32 *Watch;
  int ZoneID;
//  VecFx32 Pos;
  LOCATION  Location;
  int OutCutinNo;
  int InCutinNo;
}SORAWOTOBU_WORK;

static GMEVENT_RESULT FSkillSorawotobuEvent(GMEVENT * event, int * seq, void *work);

//------------------------------------------------------------------
/*
 *  @brief  ������ƂԃC�x���g�N��
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldSkillSorawotobu(
    GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, const int inZoneID)
{
	GMEVENT * event = GMEVENT_Create( gsys, NULL, FSkillSorawotobuEvent, sizeof(SORAWOTOBU_WORK));
	SORAWOTOBU_WORK * wk = GMEVENT_GetEventWork(event);
  
  MI_CpuClear8(wk,sizeof(SORAWOTOBU_WORK));
  wk->gsys = gsys;
//  wk->gdata = GAMESYSTEM_GetGameData(gsys);

  wk->ZoneID = inZoneID;

  //���ʂŃJ�b�g�C���i���o�[����
  {
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    if ( MyStatus_GetMySex( GAMEDATA_GetMyStatus( gdata ) ) == PTL_SEX_MALE )
    {
      wk->OutCutinNo = FLDCIID_FLY_OUT;
      wk->InCutinNo = FLDCIID_FLY_IN;
    }
    else
    {
      wk->OutCutinNo = FLDCIID_FLY_OUT;
      wk->InCutinNo = FLDCIID_FLY_IN;
    }
  }
  LOCATION_SetDefaultPos(&wk->Location, inZoneID);
/**
  if( ZONEDATA_IsRailMap(inZoneID) )
  {
    // ���[���p������
    ZONEDATA_GetStartRailPos(inZoneID, &wk->Pos);
  }
  else
  {
    // �O���b�h�p������
    ZONEDATA_GetStartPos(inZoneID, &wk->Pos);
  }
*/
	return event;
}

static GMEVENT_RESULT FSkillSorawotobuEvent(GMEVENT * event, int * seq, void *work)
{
	SORAWOTOBU_WORK * wk = work;
  FIELDMAP_WORK *fieldWork;
  FLD3D_CI_PTR ciPtr;
  FIELD_CAMERA * camera;
  FLDNOGRID_MAPPER* mapper;

  fieldWork = GAMESYSTEM_GetFieldMapWork(wk->gsys);
  camera = FIELDMAP_GetFieldCamera(fieldWork);
  ciPtr = FIELDMAP_GetFld3dCiPtr(fieldWork);
  mapper = FIELDMAP_GetFldNoGridMapper(fieldWork);

  switch(*seq){
  case 0:
    //�J�����G���A������t�b�N
    FIELD_CAMERA_SetCameraAreaActive( camera, FALSE );
    //���݃E�H�b�`�^�[�Q�b�g��ۑ�
    wk->Watch = FIELD_CAMERA_GetWatchTarget(camera);
    FIELD_CAMERA_StopTraceRequest(camera);
    (*seq)++;
    break;
  case 1:
    if ( (wk->Watch != NULL) && FIELD_CAMERA_CheckTrace(camera) ){
      break;
    }
    //���[���V�X�e���X�V�X�g�b�v
    FLDNOGRID_MAPPER_SetRailCameraActive( mapper, FALSE );
    //�J�����p�[�W
    FIELD_CAMERA_FreeTarget(camera);
    //���݂̃J�����ݒ��ۑ�
    FIELD_CAMERA_SetRecvCamParam(camera);
    //�J���������уJ�����֐��`�ړ����N�G�X�g
    {
      FLD_CAM_MV_PARAM param;
      VecFx32 player_vec;
      FIELD_PLAYER * player;
      player = FIELDMAP_GetFieldPlayer(fieldWork);
      //��l����3�c���W���擾
      FIELD_PLAYER_GetPos( player, &player_vec );
      param.Core.AnglePitch = 9688;
      param.Core.AngleYaw = 0;
      param.Core.Distance = 970752;
      param.Core.Shift.x = 0;
      param.Core.Shift.y = 16384;
      param.Core.Shift.z = 0;
      param.Core.TrgtPos = player_vec;
      param.Core.Fovy = 3640;
      param.Chk.Shift = TRUE;
      param.Chk.Pitch = TRUE;
      param.Chk.Yaw = FALSE;
      param.Chk.Dist = TRUE;
      param.Chk.Fovy = TRUE;
      param.Chk.Pos = TRUE;
      FIELD_CAMERA_SetLinerParam(camera, &param, FLYSKY_CAM_MOVE_FRAME);
    }
    (*seq)++;
    /*none break*/
  case 2:
    //�J�����ړ��҂�
    if ( !FIELD_CAMERA_CheckMvFunc(camera) )
    {
      GMEVENT *child;
      //�J�b�g�C�����o�J�n
      child = FLD3D_CI_CreateCutInEvt(wk->gsys, ciPtr, wk->OutCutinNo);
      GMEVENT_CallEvent(event, child);
      (*seq)++;
    }
    break;
  case 3:
    //���@��\��
    {
      MMDL * mmdl;
      FIELD_PLAYER *fld_player;
      fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      mmdl = FIELD_PLAYER_GetMMdl( fld_player );
      MMDL_SetStatusBitVanish(mmdl, TRUE);
    }
    //�J�������A�p�����[�^�N���A�i���A���Ȃ��Ń}�b�v�J�ڂ���̂Łj
    FIELD_CAMERA_ClearRecvCamParam(camera);
    //�u���b�N�A�E�g���N�G�X�g
    GMEVENT_CallEvent(event, EVENT_FlySkyBrightOut(wk->gsys, fieldWork, FIELD_FADE_BLACK, FIELD_FADE_WAIT));
    (*seq)++;
    break;
  case 4:
    //�m�[�t�F�[�h�}�b�v�`�F���W
    {
      GMEVENT *child; 
      child = EVENT_ChangeMapSorawotobu(wk->gsys, fieldWork,
          wk->ZoneID, &wk->Location, DIR_DOWN );
      GMEVENT_CallEvent(event, child);
      (*seq)++;
    }
    break;
  case 5:
    //�n���\���I�t
    {
       FIELD_PLACE_NAME * p_sys = FIELDMAP_GetPlaceNameSys( fieldWork );
       FIELD_PLACE_NAME_Hide( p_sys );
    }
    //�J�����G���A������t�b�N
    FIELD_CAMERA_SetCameraAreaActive( camera, FALSE );
    //���݃E�H�b�`�^�[�Q�b�g��ۑ�
    wk->Watch = FIELD_CAMERA_GetWatchTarget(camera);
    FIELD_CAMERA_StopTraceRequest(camera);
    (*seq)++;
    break;
  case 6:
    if ( (wk->Watch != NULL) && FIELD_CAMERA_CheckTrace(camera) ){
      break;
    }
    //���[���V�X�e���X�V�X�g�b�v
    FLDNOGRID_MAPPER_SetRailCameraActive( mapper, FALSE );
    //�J�����p�[�W
    FIELD_CAMERA_FreeTarget(camera);
    //���݂̃J�����ݒ��ۑ�
    FIELD_CAMERA_SetRecvCamParam(camera);
    //�J���������уJ�����֐��`�ړ����N�G�X�g
    {
      FLD_CAM_MV_PARAM param;
      VecFx32 player_vec;
      FIELD_PLAYER * player;
      player = FIELDMAP_GetFieldPlayer(fieldWork);
      //��l����3�c���W���擾
      FIELD_PLAYER_GetPos( player, &player_vec );
      param.Core.AnglePitch = 9688;
      param.Core.AngleYaw = 0;
      param.Core.Distance = 970752;
      param.Core.Shift.x = 0;
      param.Core.Shift.y = 16384;
      param.Core.Shift.z = 0;
      param.Core.TrgtPos = player_vec;
      param.Core.Fovy = 3640;
      param.Chk.Shift = TRUE;
      param.Chk.Pitch = TRUE;
      param.Chk.Yaw = FALSE;
      param.Chk.Dist = TRUE;
      param.Chk.Fovy = TRUE;
      param.Chk.Pos = TRUE;
      FIELD_CAMERA_SetLinerParam(camera, &param, 1);
    }
    (*seq)++;
    /*none break*/
  case 7:
    //�J�����ړ��҂�
    if ( !FIELD_CAMERA_CheckMvFunc(camera) )
    {
      //���@��\��
      {
        MMDL * mmdl;
        FIELD_PLAYER *fld_player;
        fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
        mmdl = FIELD_PLAYER_GetMMdl( fld_player );
        MMDL_SetStatusBitVanish(mmdl, TRUE);
      }
      //�u���b�N�C�����N�G�X�g
      GMEVENT_CallEvent(event, EVENT_FlySkyBrightIn(wk->gsys, fieldWork, FIELD_FADE_BLACK, FIELD_FADE_WAIT));
      (*seq)++;
    }
    break;
  case 8:
    {
      GMEVENT *child;
      //�J�b�g�C�����o�J�n
      child = FLD3D_CI_CreateCutInEvt(wk->gsys, ciPtr, wk->InCutinNo);
      GMEVENT_CallEvent(event, child);
      (*seq)++;
    }
    break;
  case 9:
    //�n���\���J�n
    {
       FIELD_PLACE_NAME * p_sys = FIELDMAP_GetPlaceNameSys( fieldWork );
       FIELD_PLACE_NAME_DisplayForce( p_sys, wk->ZoneID );
    }

    {
      FLD_CAM_MV_PARAM_CHK chk;
      chk.Shift = TRUE;
      chk.Pitch = TRUE;
      chk.Yaw = FALSE;
      chk.Dist = TRUE;
      chk.Fovy = TRUE;
      chk.Pos = TRUE;
      //�J�����߂�
      FIELD_CAMERA_RecvLinerParam(camera, &chk, FLYSKY_CAM_MOVE_FRAME);
    }
    (*seq)++;
    /*none break*/
  case 10:
    //�J�����߂��҂�
    if ( !FIELD_CAMERA_CheckMvFunc(camera) ){
      //�J�����o�C���h
      if (wk->Watch != NULL){
        FIELD_CAMERA_BindTarget(camera, wk->Watch);
      }
      FIELD_CAMERA_RestartTrace(camera);
      //���[���V�X�e���X�V�J�n
      FLDNOGRID_MAPPER_SetRailCameraActive( mapper, TRUE );
      //���A�f�[�^�̃N���A
      FIELD_CAMERA_ClearRecvCamParam(camera);
      //�J�����G���A������t�b�N����
      FIELD_CAMERA_SetCameraAreaActive( camera, TRUE );
      //�n�a�i��~����
      ;
      return GMEVENT_RES_FINISH;
    }
  }

  return GMEVENT_RES_CONTINUE;
}

