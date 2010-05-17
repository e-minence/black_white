////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �o������C�x���g���ʃw�b�_
 * @file   entrance_event_common.h
 * @author obata
 * @date   2010.05.17
 */ 
////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "gamesystem/gamesystem.h"   // for GMEVENT, GAMESYS_WORK
#include "gamesystem/gamedata_def.h" // for GAMEDATA
#include "field/fieldmap_proc.h"     // for FIELDMAP_WORK
#include "field/eventdata_type.h"    // for EXIT_TYPE
#include "field/location.h"          // for LOCATION
#include "event_field_fade.h"        // for FIELD_FADE_TYPE


typedef struct {

  GMEVENT*        parentEvent;
  GAMESYS_WORK*   gameSystem;
  GAMEDATA*       gameData;
  FIELDMAP_WORK*  fieldmap;
  EXIT_TYPE       exit_type_in;     // ������ EXIT_TYPE
  EXIT_TYPE       exit_type_out;    // �o���� EXIT_TYPE
  u16             prev_zone_id;     // �J�ڑO�̃]�[��ID
  LOCATION        nextLocation;     // �J�ڐ�̃��P�[�V����
  FIELD_FADE_TYPE fadeout_type;     // �t�F�[�h�A�E�g�^�C�v
  FIELD_FADE_TYPE fadein_type;      // �t�F�[�h�C���^�C�v
  BOOL            season_disp_flag; // �G�߂�\�����邩�ǂ���
  u8              start_season;     // �\������G�߂̊J�n�ʒu
  u8              end_season;       // �\������G�߂̏I���ʒu
  BOOL            BGM_standby_flag; // In �C�x���g�� BGM �̕ύX�������s�������ǂ���
  BOOL            BGM_fadeout_flag; // In �C�x���g�� BGM �̃t�F�[�h�A�E�g���s�������ǂ���

} ENTRANCE_EVDATA;
