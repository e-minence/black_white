//======================================================================
/**
 * @file  musical_event.c
 * @brief �~���[�W�J���Ǘ��C�x���g
 * @author  ariizumi
 * @data  09/09/08
 *
 * ���W���[�����FMUSICAL_
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc/fieldmap/zone_id.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_comm.h"
#include "poke_tool/monsno_def.h"
#include "field/fieldmap.h"
#include "field/event_fieldmap_control.h"
#include "field/event_mapchange.h"
#include "field/script.h"
#include "field/script_local.h"
#include "savedata/save_control.h"
#include "savedata/musical_save.h"
#include "sound/pm_sndsys.h"
#include "musical/musical_system.h"
#include "musical/musical_local.h"
#include "musical/musical_program.h"
#include "musical/musical_dressup_sys.h"
#include "musical/musical_stage_sys.h"
#include "musical/musical_shot_sys.h"
#include "musical/comm/mus_comm_lobby.h"
#include "musical/comm/mus_comm_func.h"
#include "musical/stage/sta_acting.h"
#include "musical/stage/sta_local_def.h"
#include "musical/dressup/dup_local_def.h"

#include "musical_event.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define

FS_EXTERN_OVERLAY(musical);

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MES_EXIT_FIELD,
  
  MES_INIT_MUSICAL,
  MES_TERM_MUSICAL,
  
  MES_INIT_DRESSUP,
  MES_TERM_DRESSUP,

  MES_INIT_ACTING,
  MES_TERM_ACTING,

  MES_INIT_MUSICAL_SHOT,
  MES_TERM_MUSICAL_SHOT,

  MES_RETURN_FIELD,
  MES_FINIHS_EVENT,

  MES_ENTER_WAITROOM_FIRST,
  MES_WAITROOM_FIRST,
  MES_EXIT_WAITROOM_FIRST,

  MES_ENTER_WAITROOM_SECOND,
  MES_WAITROOM_SECOND,
  MES_EXIT_WAITROOM_SECOND,

  MES_ENTER_WAITROOM_THIRD,
  MES_WAITROOM_THIRD,
  MES_EXIT_WAITROOM_THIRD,

}MUSICAL_EVENT_STATE;



//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  GAMESYS_WORK *gsys;

  MUSICAL_EVENT_STATE state;
  u8 subSeq;
  
  BOOL          isComm;
  POKEMON_PARAM *pokePara;
  SAVE_CONTROL_WORK *saveCtrl;
  GAME_COMM_SYS_PTR gameComm;
  
  MUSICAL_DISTRIBUTE_DATA *distData;
  MUS_COMM_WORK     *commWork;
  DRESSUP_INIT_WORK *dupInitWork;
  STAGE_INIT_WORK *actInitWork;
  MUS_SHOT_INIT_WORK *shotInitWork;
  
  MUSICAL_PROGRAM_WORK *progWork;
  MUSICAL_POKE_PARAM *musPoke;

  u8 musicalIndex[MUSICAL_POKE_MAX];

}MUSICAL_EVENT_WORK;


//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
static GMEVENT_RESULT MUSICAL_MainEvent( GMEVENT *event, int *seq, void *work );

static void MUSICAL_EVENT_InitMusical( MUSICAL_EVENT_WORK *evWork );
static void MUSICAL_EVENT_TermMusical( MUSICAL_EVENT_WORK *evWork );
static void MUSICAL_EVENT_InitDressUp( MUSICAL_EVENT_WORK *evWork );
static void MUSICAL_EVENT_TermDressUp( MUSICAL_EVENT_WORK *evWork );
static void MUSICAL_EVENT_InitActing( MUSICAL_EVENT_WORK *evWork );
static void MUSICAL_EVENT_TermActing( MUSICAL_EVENT_WORK *evWork );
static void MUSICAL_EVENT_InitMusicalShot( MUSICAL_EVENT_WORK *evWork );

static const BOOL MUSICAL_EVENT_InitField( GMEVENT *event, MUSICAL_EVENT_WORK *evWork );
static const BOOL MUSICAL_EVENT_ExitField( GMEVENT *event, MUSICAL_EVENT_WORK *evWork );
static const void MUSICAL_EVENT_JumpWaitingRoom( GMEVENT *event, MUSICAL_EVENT_WORK *evWork );
static const void MUSICAL_EVENT_JumpMusicalHall( GMEVENT *event, MUSICAL_EVENT_WORK *evWork );
static const void MUSICAL_EVENT_RunScript( GMEVENT *event, MUSICAL_EVENT_WORK *evWork , u16 scriptId );
//--------------------------------------------------------------
//  �C�x���g�쐬
//--------------------------------------------------------------
GMEVENT* MUSICAL_CreateEvent( GAMESYS_WORK * gsys , GAMEDATA *gdata , const BOOL isComm )
{
  GMEVENT *event;
  MUSICAL_EVENT_WORK *evWork;
  
  event = GMEVENT_Create(
      gsys, NULL, MUSICAL_MainEvent, sizeof(MUSICAL_EVENT_WORK) );
  evWork = GMEVENT_GetEventWork( event );
  
  evWork->gsys = gsys;
  evWork->isComm = isComm;

  evWork->saveCtrl = GAMEDATA_GetSaveControlWork( gdata );
  evWork->gameComm = GAMESYSTEM_GetGameCommSysPtr( gsys );
  //FIXME ����������
  evWork->pokePara = PP_Create(
      MONSNO_PIKUSII , 20, PTL_SETUP_POW_AUTO , HEAPID_PROC );

  evWork->dupInitWork = NULL;
  evWork->actInitWork = NULL;
  evWork->shotInitWork = NULL;

  evWork->state = MES_ENTER_WAITROOM_FIRST;
  evWork->subSeq = 0;
  
  //�~���[�W�J���̔z�u����ւ�
  {
    u8 i,j;
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      evWork->musicalIndex[i] = i;
    }
    for( j=0;j<10;j++ )
    {
      for( i=0;i<MUSICAL_POKE_MAX;i++ )
      {
        u8 swapIdx = GFL_STD_MtRand0(MUSICAL_POKE_MAX);
        u8 temp = evWork->musicalIndex[i];
        evWork->musicalIndex[i] = evWork->musicalIndex[swapIdx];
        evWork->musicalIndex[swapIdx] = temp;
      }
    }
  }

  return event;
}

//--------------------------------------------------------------
//  �C�x���g���C��
//--------------------------------------------------------------
static GMEVENT_RESULT MUSICAL_MainEvent( GMEVENT *event, int *seq, void *work )
{
  MUSICAL_EVENT_WORK *evWork = work;
  switch( evWork->state )
  {
  case MES_ENTER_WAITROOM_FIRST:
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_PROC );
    MUSICAL_EVENT_JumpWaitingRoom( event , evWork );
    evWork->state = MES_WAITROOM_FIRST;
    break;

  case MES_WAITROOM_FIRST:
    MUSICAL_EVENT_RunScript( event , evWork , 1 );
    evWork->state = MES_EXIT_FIELD;
    break;
    
  //�t�B�[���h���甲����
  //------------------------------
  case MES_EXIT_FIELD:
    {
      const BOOL isFinish = MUSICAL_EVENT_ExitField( event , evWork );
      if( isFinish == TRUE )
      {
        evWork->subSeq = 0;
        evWork->state = MES_INIT_MUSICAL;
      }
    }
    break;

  //�ŏ��̏�����
  //------------------------------
  case MES_INIT_MUSICAL:
    {
      MUSICAL_EVENT_InitMusical( evWork );
      evWork->state = MES_INIT_DRESSUP;
    }
    break;

  case MES_INIT_DRESSUP:
    PMSND_StopBGM();
    MUSICAL_EVENT_InitDressUp( evWork );
    GAMESYSTEM_CallProc( evWork->gsys , NO_OVERLAY_ID, &DressUp_ProcData, evWork->dupInitWork );
    evWork->state = MES_TERM_DRESSUP;
    break;
  
  case MES_TERM_DRESSUP:
    if( GAMESYSTEM_IsProcExists(evWork->gsys) == GFL_PROC_MAIN_NULL )
    {
      MUSICAL_EVENT_TermDressUp( evWork );
      evWork->state = MES_ENTER_WAITROOM_SECOND;
    }
    break;

  //�h���X�A�b�v���V���[�p�[�g�Ԃ̍T����
  //------------------------------
  case MES_ENTER_WAITROOM_SECOND:
    {
      const BOOL isFinish = MUSICAL_EVENT_InitField( event , evWork );
      if( isFinish == TRUE )
      {
        evWork->subSeq = 0;
        evWork->state = MES_WAITROOM_SECOND;
      }
    }
    break;
    
  case MES_WAITROOM_SECOND:
    MUSICAL_EVENT_RunScript( event , evWork , 2 );
    evWork->state = MES_EXIT_WAITROOM_SECOND;
    break;
    
  case MES_EXIT_WAITROOM_SECOND:
    {
      const BOOL isFinish = MUSICAL_EVENT_ExitField( event , evWork );
      if( isFinish == TRUE )
      {
        evWork->subSeq = 0;
        evWork->state = MES_INIT_ACTING;
      }
    }
    break;
    
  //�V���[�p�[�g
  //------------------------------
  case MES_INIT_ACTING:
    PMSND_StopBGM();
    MUSICAL_EVENT_InitActing( evWork );
    GAMESYSTEM_CallProc( evWork->gsys , NO_OVERLAY_ID, &MusicalStage_ProcData, evWork->actInitWork );
    evWork->state = MES_TERM_ACTING;
    break;
  
  case MES_TERM_ACTING:
    if( GAMESYSTEM_IsProcExists(evWork->gsys) == GFL_PROC_MAIN_NULL )
    {
      MUSICAL_EVENT_TermActing( evWork );
      evWork->state = MES_INIT_MUSICAL_SHOT;
    }
    break;

  //�~���[�W�J���V���b�g
  //------------------------------
  case MES_INIT_MUSICAL_SHOT:
    MUSICAL_EVENT_InitMusicalShot( evWork );
    GAMESYSTEM_CallProc( evWork->gsys , NO_OVERLAY_ID, &MusicalShot_ProcData, evWork->shotInitWork );
    evWork->state = MES_TERM_MUSICAL_SHOT;
    break;
    
  case MES_TERM_MUSICAL_SHOT:
    if( GAMESYSTEM_IsProcExists(evWork->gsys) == GFL_PROC_MAIN_NULL )
    {
      MUSICAL_EVENT_TermActing( evWork );
      evWork->state = MES_ENTER_WAITROOM_THIRD;
    }
    break;
    
  //�Ō�̍T����
  //------------------------------
  case MES_ENTER_WAITROOM_THIRD:
    {
      const BOOL isFinish = MUSICAL_EVENT_InitField( event , evWork );
      if( isFinish == TRUE )
      {
        evWork->subSeq = 0;
        evWork->state = MES_WAITROOM_THIRD;
      }
    }
    break;
    
  case MES_WAITROOM_THIRD:
    MUSICAL_EVENT_RunScript( event , evWork , 3 );
    //evWork->state = MES_EXIT_WAITROOM_THIRD;
    evWork->state = MES_TERM_MUSICAL;
    break;
    
  case MES_EXIT_WAITROOM_THIRD:
    {
      const BOOL isFinish = MUSICAL_EVENT_ExitField( event , evWork );
      if( isFinish == TRUE )
      {
        evWork->subSeq = 0;
        evWork->state = MES_TERM_MUSICAL;
      }
    }
    break;
    
  //���X�̊J��
  //------------------------------
  case MES_TERM_MUSICAL:
    MUSICAL_EVENT_TermMusical( evWork );
    MUSICAL_EVENT_JumpMusicalHall( event , evWork );
    evWork->state = MES_FINIHS_EVENT;
    break;
  
  //�t�B�[���h�֖߂�
  //------------------------------
  case MES_RETURN_FIELD:
    {
      const BOOL isFinish = MUSICAL_EVENT_InitField( event , evWork );
      if( isFinish == TRUE )
      {
        evWork->subSeq = 0;
        evWork->state = MES_FINIHS_EVENT;
      }
    }
    break;
  
  case MES_FINIHS_EVENT:
    //FIXME ����������
    GFL_HEAP_FreeMemory( evWork->pokePara );
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_PROC );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


#pragma mark [>MusicalFunc
//--------------------------------------------------------------
//  �~���[�W�J���̏�����
//--------------------------------------------------------------
static void MUSICAL_EVENT_InitMusical( MUSICAL_EVENT_WORK *evWork )
{
  GFL_OVERLAY_Load(FS_OVERLAY_ID(musical));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MUSICAL_STRM|HEAPDIR_MASK, 0x80000 );
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MUSICAL_PROC|HEAPDIR_MASK, 0x8000 );

  evWork->musPoke = MUSICAL_SYSTEM_InitMusPoke( evWork->pokePara , HEAPID_MUSICAL_PROC );
  evWork->distData = MUSICAL_SYSTEM_InitDistributeData( HEAPID_MUSICAL_PROC );
  MUSICAL_SYSTEM_LoadDistributeData( evWork->distData , HEAPID_MUSICAL_STRM );
  evWork->progWork = MUSICAL_PROGRAM_InitProgramData( HEAPID_MUSICAL_PROC , evWork->distData );
  evWork->commWork = NULL;
}

//--------------------------------------------------------------
//  �~���[�W�J���̊J��
//--------------------------------------------------------------
static void MUSICAL_EVENT_TermMusical( MUSICAL_EVENT_WORK *evWork )
{
  if( evWork->commWork != NULL )
  {
    MUS_COMM_DeleteWork( evWork->commWork );
    evWork->commWork = NULL;
  }
  if( evWork->dupInitWork != NULL )
  {
    MUSICAL_DRESSUP_DeleteInitWork( evWork->dupInitWork );
  }
  if( evWork->actInitWork != NULL )
  {
    MUSICAL_STAGE_DeleteStageWork( evWork->actInitWork );
  }
  if( evWork->shotInitWork != NULL )
  {
    GFL_HEAP_FreeMemory( evWork->shotInitWork->musShotData );
    GFL_HEAP_FreeMemory( evWork->shotInitWork );
  }
  MUSICAL_SYSTEM_TermDistributeData( evWork->distData );
  GFL_HEAP_FreeMemory( evWork->musPoke );
  
  MUSICAL_PROGRAM_TermProgramData( evWork->progWork );
  GFL_HEAP_DeleteHeap( HEAPID_MUSICAL_STRM );
  GFL_HEAP_DeleteHeap( HEAPID_MUSICAL_PROC );

  GFL_OVERLAY_Unload(FS_OVERLAY_ID(musical));
  
    
}

//--------------------------------------------------------------
//  �h���X�A�b�v�̏�����
//--------------------------------------------------------------
static void MUSICAL_EVENT_InitDressUp( MUSICAL_EVENT_WORK *evWork )
{
  evWork->dupInitWork = MUSICAL_DRESSUP_CreateInitWork( HEAPID_MUSICAL_PROC , evWork->musPoke , evWork->saveCtrl );
  evWork->dupInitWork->commWork = evWork->commWork;
}

//--------------------------------------------------------------
//  �h���X�A�b�v�̊J��
//--------------------------------------------------------------
static void MUSICAL_EVENT_TermDressUp( MUSICAL_EVENT_WORK *evWork )
{
  //���ݏ�������
}

//--------------------------------------------------------------
//  �V���[�p�[�g�̏�����
//--------------------------------------------------------------
static void MUSICAL_EVENT_InitActing( MUSICAL_EVENT_WORK *evWork )
{
  u8 i;
  evWork->actInitWork = MUSICAL_STAGE_CreateStageWork( HEAPID_MUSICAL_PROC , evWork->commWork );
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    if( evWork->musicalIndex[i] == 0 )
    {
      //�v���C���[
      MUSICAL_STAGE_SetData_Player( evWork->actInitWork , evWork->musicalIndex[i] , evWork->musPoke );
    }
    else
    {
      //NPC
      MUSICAL_PROGRAM_SetData_NPC( evWork->progWork , evWork->actInitWork , evWork->musicalIndex[i] , evWork->musicalIndex[i]-1 , HEAPID_MUSICAL_PROC );
    }
  }

  evWork->actInitWork->distData = evWork->distData;
  evWork->actInitWork->progWork = evWork->progWork;
  MUSICAL_PROGRAM_CalcPokemonPoint( HEAPID_MUSICAL_PROC , evWork->progWork , evWork->actInitWork );
}

//--------------------------------------------------------------
//  �V���[�p�[�g�̊J��
//--------------------------------------------------------------
static void MUSICAL_EVENT_TermActing( MUSICAL_EVENT_WORK *evWork )
{
  //���ݏ�������
}

//--------------------------------------------------------------
//  �~���[�W�J���V���b�g�̏�����
//--------------------------------------------------------------
static void MUSICAL_EVENT_InitMusicalShot( MUSICAL_EVENT_WORK *evWork )
{
  evWork->shotInitWork = GFL_HEAP_AllocMemory( HEAPID_MUSICAL_PROC , sizeof( MUS_SHOT_INIT_WORK ));
  evWork->shotInitWork->musShotData = GFL_HEAP_AllocClearMemory( HEAPID_MUSICAL_PROC , sizeof( MUSICAL_SHOT_DATA ));
  {
    u8 i;
    RTCDate date;
    MUSICAL_SHOT_DATA *shotData = evWork->shotInitWork->musShotData;
    GFL_RTC_GetDate( &date );
    shotData->bgNo = 0;
    shotData->year = date.year;
    shotData->month = date.month;
    shotData->day = date.day;
    shotData->title[0] = L'�|';
    shotData->title[1] = L'�P';
    shotData->title[2] = L'�b';
    shotData->title[3] = L'�^';
    shotData->title[4] = L'�[';
    shotData->title[5] = L'��';
    shotData->title[6] = L'��';
    shotData->title[7] = L'��';
    shotData->title[8] = L'�X';
    shotData->title[9] = L'�^';
    shotData->title[10] = L'�[';
    shotData->title[11] = L'��';
    shotData->title[12] = GFL_STR_GetEOMCode();
    
    
    //�X�|�b�g���C�g�̌v�Z
    {
      u8 maxPoint = 0;
      for( i=0;i<MUSICAL_POKE_MAX;i++ )
      {
        const u8 point = evWork->actInitWork->musPoke[i]->point;
        if( maxPoint < point )
        {
          shotData->spotBit = 0;
          maxPoint = point;
        }
        if( maxPoint == point )
        {
          shotData->spotBit += 1<<i;
        }
      }
    }
    
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      u8 j;
      MUSICAL_POKE_PARAM *musPoke = evWork->actInitWork->musPoke[i];
      shotData->shotPoke[i].monsno = musPoke->mcssParam.monsno;
      shotData->shotPoke[i].trainerName[0] = L'�g';
      shotData->shotPoke[i].trainerName[1] = L'��';
      shotData->shotPoke[i].trainerName[2] = L'�[';
      shotData->shotPoke[i].trainerName[3] = L'�i';
      shotData->shotPoke[i].trainerName[4] = L'�P'+i;
      shotData->shotPoke[i].trainerName[5] = 0;
      
      //�����ӏ��̏�����
      for( j=0;j<MUSICAL_ITEM_EQUIP_MAX;j++ )
      {
        shotData->shotPoke[i].equip[j].itemNo = MUSICAL_ITEM_INVALID;
        shotData->shotPoke[i].equip[j].angle = 0;
        shotData->shotPoke[i].equip[j].equipPos = MUS_POKE_EQU_INVALID;
      }

      for( j=0;j<MUS_POKE_EQUIP_MAX;j++ )
      {
        if( musPoke->equip[j].itemNo != MUSICAL_ITEM_INVALID )
        {
          const u8 idx = musPoke->equip[j].priority;
          shotData->shotPoke[i].equip[idx].itemNo = musPoke->equip[j].itemNo;
          shotData->shotPoke[i].equip[idx].angle = musPoke->equip[j].angle;
          shotData->shotPoke[i].equip[idx].equipPos = j;
        }
      }
    }
  }
  evWork->shotInitWork->isCheckMode = TRUE;
  {
    GAMEDATA *gameData = GAMESYSTEM_GetGameData(evWork->gsys);
    SAVE_CONTROL_WORK *saveWork = GAMEDATA_GetSaveControlWork(gameData);
    evWork->shotInitWork->musicalSave = MUSICAL_SAVE_GetMusicalSave(saveWork);
  }
}

#pragma mark [>FieldFunc
//--------------------------------------------------------------
//  �t�B�[���h���Ă�
//--------------------------------------------------------------
static const BOOL MUSICAL_EVENT_InitField( GMEVENT *event, MUSICAL_EVENT_WORK *evWork )
{
  switch( evWork->subSeq )
  {
  case 0:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(evWork->gsys));
    evWork->subSeq++;
    break;
  case 1:
    {
      FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( evWork->gsys );
      GMEVENT_CallEvent(event, EVENT_FieldFadeIn(evWork->gsys, fieldWork, 0));
      evWork->subSeq++;
    }
    break;
  case 2:
    return TRUE;
    break;
  }
  return FALSE;
}

//--------------------------------------------------------------
//  �t�B�[���h�𔲂���
//--------------------------------------------------------------
static const BOOL MUSICAL_EVENT_ExitField( GMEVENT *event, MUSICAL_EVENT_WORK *evWork )
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( evWork->gsys );
  switch( evWork->subSeq )
  {
  case 0:
    GMEVENT_CallEvent(event, EVENT_FieldFadeOut(evWork->gsys, fieldWork, 0));
    evWork->subSeq++;
    break;
  case 1:
    GMEVENT_CallEvent(event, EVENT_FieldClose(evWork->gsys, fieldWork));
    evWork->subSeq++;
    break;
  case 2:
    return TRUE;
    break;
  }
  return FALSE;
}

//--------------------------------------------------------------
//  �ҍ����ֈړ�
//--------------------------------------------------------------
static const void MUSICAL_EVENT_JumpWaitingRoom( GMEVENT *event, MUSICAL_EVENT_WORK *evWork )
{
  GMEVENT *newEvent;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( evWork->gsys );
  const VecFx32 pos = { FX32_CONST(264.0f) , FX32_CONST(0.0f) , FX32_CONST(312.0f) };
  
  newEvent = DEBUG_EVENT_ChangeMapPos( evWork->gsys, fieldWork ,
                ZONE_ID_C04R0202 , &pos , 0 );
  GMEVENT_CallEvent(event, newEvent);
}

//--------------------------------------------------------------
//  ���ֈړ�
//--------------------------------------------------------------
static const void MUSICAL_EVENT_JumpMusicalHall( GMEVENT *event, MUSICAL_EVENT_WORK *evWork )
{
  GMEVENT *newEvent;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( evWork->gsys );

  const VecFx32 pos = { FX32_CONST(12536.0f) , 0 , FX32_CONST(9432.0f) };
  
  newEvent = DEBUG_EVENT_ChangeMapPos( evWork->gsys, fieldWork ,
                ZONE_ID_C01 , &pos , 2 );
  GMEVENT_CallEvent(event, newEvent);
}

//--------------------------------------------------------------
//  �X�N���v�g���Ă�
//--------------------------------------------------------------
static const void MUSICAL_EVENT_RunScript( GMEVENT *event, MUSICAL_EVENT_WORK *evWork , u16 scriptId )
{
  GMEVENT *newEvent;
  SCRIPT_FLDPARAM fparam;

  newEvent = SCRIPT_SetEventScript( evWork->gsys, scriptId , NULL ,
                HEAPID_FIELDMAP );
  GMEVENT_CallEvent(event, newEvent);
}

