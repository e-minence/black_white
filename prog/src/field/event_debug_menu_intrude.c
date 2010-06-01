//======================================================================
/**
 * @brief  �f�o�b�O���j���[�u�n�C�����N�v
 * @file   event_debug_intrude.c
 * @author tamada
 * @date   2010.05.29
 */
//======================================================================
#ifdef PM_DEBUG

#include <gflib.h> 
#include "print/global_msg.h" 
#include "field/field_msgbg.h"
#include "fieldmap.h"
#include "event_debug_local.h"
#include "pokemon_egg.h"

#include "arc/message.naix"
#include "msg/msg_debug_intrude.h"

#include "event_debug_menu_intrude.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"
#include "../../../resource/fldmapdata/flagwork/work_define.h"

#include "field/field_comm/intrude_field.h" //PALACE_MAP_LEN
#include "field/field_comm/intrude_mission.h" //MISSION_LIST_Create
//======================================================================
//======================================================================
static void clearTutorial( FIELDMAP_WORK * fieldmap );
static void dumpPlayerInfo( FIELDMAP_WORK * fieldmap );
static void resetMissionList( FIELDMAP_WORK * fieldmap );

//======================================================================
//======================================================================
//--------------------------------------------------------------
// �n�C�����N�f�o�b�O�@���j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER menuHeader =
{
  1,    //���X�g���ڐ�
  9,    //�\���ő區�ڐ�
  0,    //���x���\���w���W
  13,   //���ڕ\���w���W
  0,    //�J�[�\���\���w���W
  0,    //�\���x���W
  1,    //�\�������F
  15,   //�\���w�i�F
  2,    //�\�������e�F
  0,    //�����Ԋu�w
  1,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_LRKEY, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
// ���j���[���X�g
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST menuList[] = 
{
  {STR_INTRUDE_TUTORIAL_CLEAR,     (void*)clearTutorial},      // �`���[�g���A���N���A
  {STR_INTRUDE_DUMP_PLAYER,       (void*)dumpPlayerInfo},      // �v���C���[���
  {STR_INTRUDE_MISSION_RESET,     (void*)resetMissionList},      // �~�b�V�������X�g�č쐬
};

//--------------------------------------------------------------
// ���j���[�������\����
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER menuInitializer = 
{
  NARC_message_debug_intrude_dat,
  NELEMS(menuList),
  menuList,
  &menuHeader,
  1, 1, 20, 16,
  NULL,
  NULL,
};

//--------------------------------------------------------------
/// �n�C�����N�f�o�b�O�C�x���g���[�N
//--------------------------------------------------------------
typedef struct {

  HEAPID         heapID;
  GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
  FIELDMAP_WORK* fieldmap;
  FLDMENUFUNC*   menuFunc;
  u32            select;

} EVENT_WORK;

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef void (*CALL_FUNC)( FIELDMAP_WORK * );
//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �n�C�����N�f�o�b�O�C�x���g
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuIntrudeEvent( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK* work = wk;
  
  switch( *seq ) {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldmap, work->heapID,  &menuInitializer );
    (*seq)++;
    break;

  case 1:
    work->select = FLDMENUFUNC_ProcMenu( work->menuFunc );

    if( work->select != FLDMENUFUNC_NULL )
    {
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      (*seq)++; 
    }
    break; 

  case 2: 
    // �n�C�����N�f�o�b�O
    if ( work->select && work->select != FLDMENUFUNC_CANCEL )
    {
      CALL_FUNC func = (CALL_FUNC)work->select;
      func( work->fieldmap );

#if 0
      // �쐬
      if( work->select == 0 ) { 
        //�u�`���[�g���A���N���A�v
        clearTutorial( work->fieldmap );
      }
      else if( work->select == 1 ) { 
        dumpPlayerInfo( work->fieldmap );
      }
      else if( work->select == 2 ) { 
        resetMissionList( work->fieldmap );
      }
#endif
    }
    (*seq)++;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * @brief �n�C�����N�f�o�b�O�C�x���g����
 *
 * @parma gameSystem
 * @param wk FIELDMAP_WORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
GMEVENT* DEBUG_EVENT_FLDMENU_Intrude( GAMESYS_WORK* gameSystem, void* wk )
{
  GMEVENT* event;
  EVENT_WORK* work;
  FIELDMAP_WORK* fieldmap = (FIELDMAP_WORK*)wk;
  
  event = GMEVENT_Create(
      gameSystem, NULL, debugMenuIntrudeEvent, sizeof(EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
	work->gameSystem = gameSystem;
  work->gameData   = GAMESYSTEM_GetGameData( gameSystem ); 
	work->heapID     = FIELDMAP_GetHeapID( fieldmap );
	work->fieldmap   = fieldmap;

  return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static void clearTutorial( FIELDMAP_WORK * fieldmap )
{
  GAMESYS_WORK * gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  EVENTWORK * ev = GAMEDATA_GetEventWork( gamedata );

  EVENTWORK_SetEventFlag( ev, SYS_FLAG_PALACE_MISSION_START );
  EVENTWORK_SetEventFlag( ev, SYS_FLAG_PALACE_MISSION_CLEAR );
  EVENTWORK_SetEventFlag( ev, SYS_FLAG_PALACE_DPOWER );

  *( EVENTWORK_GetEventWorkAdrs( ev, WK_SCENE_PALACE01 ) ) = 1;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void dumpPlayerInfo( FIELDMAP_WORK * fieldmap )
{
  GAMESYS_WORK * gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  int i;
  for ( i = 0; i < PLAYER_MAX; i++ )
  {
    PLAYER_WORK * player = GAMEDATA_GetPlayerWork( gamedata, i );
    OS_Printf(">>>PLAYER_ID:%d\n", i);
    OS_Printf("ZONEID:%d POS(%d,%d,%d)\n", player->zoneID,
        FX_Whole(player->position.x), FX_Whole(player->position.y), FX_Whole(player->position.z) );
    OS_Printf("TRAINERID:%d, ProfID:%d ROM:%d\n", MyStatus_GetID(&player->mystatus),
        MyStatus_GetProfileID(&player->mystatus), MyStatus_GetRomCode(&player->mystatus) );
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void resetMissionList( FIELDMAP_WORK * fieldmap )
{
  GAMESYS_WORK * gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  OCCUPY_INFO *occupy = GAMEDATA_GetMyOccupyInfo( gamedata );
  MISSION_LIST_Create( occupy );
}

#endif //PM_DEBUG
