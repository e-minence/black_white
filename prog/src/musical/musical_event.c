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
#include "arc/fieldmap/fldmmdl_objcode.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_comm.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/natsuki.h"
#include "field/fieldmap.h"
#include "field/event_fieldmap_control.h"
#include "field/event_mapchange.h"
#include "field/script.h"
#include "field/script_local.h"
#include "field/field_sound.h"
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
#include "test/ariizumi/ari_debug.h"

#include "message.naix"

#include "../resource/fldmapdata/script/c04r0202_def.h"
#include "../resource/fldmapdata/script/musical_scr_local.h"

#include "musical/musical_event.h"

#include "debug/debug_flg.h"
//======================================================================
//  define
//======================================================================
#pragma mark [> define

FS_EXTERN_OVERLAY(musical);

#define HEAPID_PROC_WRAPPER  (HEAPID_PROC)


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

  MES_ENTER_WAITROOM_FIRST_BEF_COMM,
  MES_ENTER_WAITROOM_FIRST_BEF_COMM2,
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
struct _MUSICAL_EVENT_WORK
{
  GAMESYS_WORK *gsys;
  GAMEDATA *gameData;
  MUSICAL_SCRIPT_WORK *scriptWork;

  MUSICAL_EVENT_STATE state;
  u8 subSeq;
  
  BOOL          isComm;
  POKEMON_PARAM *pokePara;
  SAVE_CONTROL_WORK *saveCtrl;    //������GAMEDATA�g��!
  GAME_COMM_SYS_PTR gameComm;
  
  MUSICAL_DISTRIBUTE_DATA *distData;
  MUS_COMM_WORK     *commWork;
  DRESSUP_INIT_WORK *dupInitWork;
  STAGE_INIT_WORK *actInitWork;
  MUS_SHOT_INIT_WORK *shotInitWork;
  
  MUSICAL_PROGRAM_WORK *progWork;
  MUSICAL_POKE_PARAM *musPoke;

  MUSICAL_SAVE *musSave;

  u8 musicalIndex[MUSICAL_POKE_MAX];  //�L������IDX�ɑΉ����������ʒu
  u8 rankIndex[MUSICAL_POKE_MAX];     //�Q���ԍ������ʏ��œ����Ă���
  u8 selfIdx;

};


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
static void MUSICAL_EVENT_SetSaveData( MUSICAL_EVENT_WORK *evWork );
static void MUSICAL_EVENT_CalcFanState( MUSICAL_EVENT_WORK *evWork );

static const BOOL MUSICAL_EVENT_InitField( GMEVENT *event, MUSICAL_EVENT_WORK *evWork );
static const BOOL MUSICAL_EVENT_ExitField( GMEVENT *event, MUSICAL_EVENT_WORK *evWork );
static const void MUSICAL_EVENT_JumpWaitingRoom( GMEVENT *event, MUSICAL_EVENT_WORK *evWork );
static const void MUSICAL_EVENT_JumpMusicalHall( GMEVENT *event, MUSICAL_EVENT_WORK *evWork );
static const void MUSICAL_EVENT_RunScript( GMEVENT *event, MUSICAL_EVENT_WORK *evWork , u16 scriptId );
//--------------------------------------------------------------
//  �C�x���g�쐬
//--------------------------------------------------------------
GMEVENT* MUSICAL_CreateEvent( GAMESYS_WORK * gsys , GAMEDATA *gdata , const u8 pokeIdx , const BOOL isComm , MUSICAL_SCRIPT_WORK *scriptWork )
{
  GMEVENT *event;
  MUSICAL_EVENT_WORK *evWork;
  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, GFL_HEAP_LOWID(HEAPID_MUSICAL_STRM) , 0x40000 );

  ARI_TPrintf("CreateMusicalEvent[%d]\n",isComm);
  
  event = GMEVENT_Create(
      gsys, NULL, MUSICAL_MainEvent, sizeof(MUSICAL_EVENT_WORK) );
  evWork = GMEVENT_GetEventWork( event );
  
  evWork->gsys = gsys;
  evWork->gameData = gdata;
  evWork->isComm = isComm;

  evWork->saveCtrl = GAMEDATA_GetSaveControlWork( gdata );
  evWork->gameComm = GAMESYSTEM_GetGameCommSysPtr( gsys );
  evWork->scriptWork = scriptWork;
  evWork->scriptWork->eventWork = evWork;
  evWork->commWork = scriptWork->commWork;
  
  {
    STRBUF *str = GFL_STR_CreateBuffer( SAVELEN_POKEMON_NAME+EOM_SIZE , HEAPID_PROC_WRAPPER );
    POKEPARTY *party = GAMEDATA_GetMyPokemon( evWork->gameData );
    evWork->pokePara = PokeParty_GetMemberPointer( party , pokeIdx );
    //�r�[�R������
    PP_Get( evWork->pokePara , ID_PARA_nickname , str );
    GAMEBEACON_Set_Musical( str );
    GFL_STR_DeleteBuffer( str );
  }

  evWork->dupInitWork = NULL;
  evWork->actInitWork = NULL;
  evWork->shotInitWork = NULL;
  evWork->musSave = MUSICAL_SAVE_GetMusicalSave( evWork->saveCtrl );
  
  evWork->distData = MUSICAL_SYSTEM_InitDistributeData( HEAPID_PROC_WRAPPER );
  if( evWork->isComm == FALSE || 
      MUS_COMM_GetMode( evWork->scriptWork->commWork ) == MCM_PARENT )
  {
    MUSICAL_SYSTEM_LoadDistributeData( evWork->distData , evWork->saveCtrl , MUSICAL_SAVE_GetProgramNumber(evWork->musSave) , HEAPID_MUSICAL_STRM );
  }
  
  if( evWork->isComm == TRUE )
  {
    evWork->state = MES_ENTER_WAITROOM_FIRST_BEF_COMM;
    evWork->subSeq = 0;
    MUS_COMM_InitMusical( evWork->commWork , GAMEDATA_GetMyStatus(evWork->gameData) , evWork->gameComm , evWork->distData , HEAPID_PROC_WRAPPER );
  }
  else
  {
    //�~���[�W�J���̔z�u����ւ�
    //�ʐM���͕ʂōs�������̂��擾����
    {
      u8 i,j;
      const u16 sumPoint = MUSICAL_SAVE_GetSumPoint(evWork->musSave);
      for( i=0;i<MUSICAL_POKE_MAX;i++ )
      {
        evWork->musicalIndex[i] = i;
      }
      for( j=0;j<10;j++ )
      {
        for( i=0;i<MUSICAL_POKE_MAX;i++ )
        {
          u8 swapIdx = GFUser_GetPublicRand0(MUSICAL_POKE_MAX);
          u8 temp = evWork->musicalIndex[i];
          evWork->musicalIndex[i] = evWork->musicalIndex[swapIdx];
          evWork->musicalIndex[swapIdx] = temp;
        }
      }
      
      evWork->selfIdx = evWork->musicalIndex[0];
      
      //�}�b�v�J�ڑO�ɉ��ڂ̃f�[�^�����K�v(NPC�L�������o������
      evWork->progWork = MUSICAL_PROGRAM_InitProgramData( HEAPID_PROC_WRAPPER , evWork->distData , sumPoint );

    }
    evWork->state = MES_ENTER_WAITROOM_FIRST;
    evWork->subSeq = 0;
  }
  

  return event;
}

//--------------------------------------------------------------
//  �C�x���g���C��
//--------------------------------------------------------------
static GMEVENT_RESULT MUSICAL_MainEvent( GMEVENT *event, int *seq, void *work )
{
  MUSICAL_EVENT_WORK *evWork = work;
  
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
  {
    static u8 befState = 0xFF;
    if( befState != evWork->state )
    {
      ARI_TPrintf("MusEvState[%d]->[%d]\n",befState,evWork->state);
      befState = evWork->state;
    }
  }
#endif
  switch( evWork->state )
  {
  case MES_ENTER_WAITROOM_FIRST_BEF_COMM:
    if( MUS_COMM_IsPostAllMyStatus( evWork->scriptWork->commWork ) == TRUE )
    {
      //�}�b�v�J�ڑO�ɉ��ڂ̃f�[�^�����K�v(NPC�L�������o������
      u32 conPointArr = 0;
      u32 npcArr = 0;
      if( MUS_COMM_GetMode( evWork->scriptWork->commWork ) == MCM_PARENT )
      {
        const u16 sumPoint = MUSICAL_SAVE_GetSumPoint(evWork->musSave);
        evWork->progWork = MUSICAL_PROGRAM_InitProgramData( HEAPID_PROC_WRAPPER , evWork->distData , sumPoint );
        conPointArr = MUSICAL_PROGRAM_GetConditionPointArr( evWork->progWork );
        npcArr = MUSICAL_PROGRAM_GetNpcArr( evWork->progWork );
      }
      MUS_COMM_StartSendProgram_Data( evWork->scriptWork->commWork , conPointArr , npcArr );
      evWork->state = MES_ENTER_WAITROOM_FIRST_BEF_COMM2;
    }
    break;
  case MES_ENTER_WAITROOM_FIRST_BEF_COMM2:
    if( MUS_COMM_IsPostProgramData(evWork->scriptWork->commWork) == TRUE )
    {
      u8 i;
      evWork->state = MES_ENTER_WAITROOM_FIRST;
      //������IDX�̎擾
      for( i=0;i<MUSICAL_POKE_MAX;i++ )
      {
        evWork->musicalIndex[i] = MUS_COMM_GetMusicalIndex( evWork->scriptWork->commWork , i );
      }
      evWork->selfIdx = MUS_COMM_GetSelfMusicalIndex( evWork->scriptWork->commWork );
      MUSICAL_EVENT_CalcProgramData( evWork );
    }
    break;
    
  case MES_ENTER_WAITROOM_FIRST:
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( GFL_HEAPID_APP );
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_PROC );
    MUSICAL_EVENT_JumpWaitingRoom( event , evWork );
    evWork->state = MES_WAITROOM_FIRST;
    break;

  case MES_WAITROOM_FIRST:
    MUSICAL_EVENT_InitMusical( evWork );
    MUSICAL_EVENT_RunScript( event , evWork , SCRID_C04R0202_MUS_1ST );
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
      evWork->state = MES_INIT_DRESSUP;
    }
    break;

  case MES_INIT_DRESSUP:
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
    MUSICAL_EVENT_RunScript( event , evWork , SCRID_C04R0202_MUS_2ND );
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
    MUSICAL_EVENT_InitActing( evWork );
    ARI_TPrintf("InitAct[%8x][%8x][%8x]\n",evWork->gsys,&MusicalStage_ProcData,evWork->actInitWork );
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
        MUSICAL_EVENT_CalcFanState( evWork );
        MUSICAL_EVENT_SetSaveData( evWork );
      }
    }
    break;
    
  case MES_WAITROOM_THIRD:
    MUSICAL_EVENT_RunScript( event , evWork , SCRID_C04R0202_MUS_3RD );
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
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_PROC );
    GFL_HEAP_DeleteHeap( HEAPID_MUSICAL_STRM );
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
  evWork->musPoke = MUSICAL_SYSTEM_InitMusPoke( evWork->pokePara , HEAPID_PROC_WRAPPER );
  
  if( evWork->isComm == FALSE )
  {
  }
  else
  {
    if( MUS_COMM_GetMode( evWork->scriptWork->commWork ) == MCM_PARENT )
    {
    }
    MUS_COMM_StartSendProgram_Script( evWork->scriptWork->commWork );
  }
}

//--------------------------------------------------------------
//  �~���[�W�J���̊J��
//--------------------------------------------------------------
static void MUSICAL_EVENT_TermMusical( MUSICAL_EVENT_WORK *evWork )
{
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
  evWork->scriptWork->eventWork = NULL;

}

//--------------------------------------------------------------
//  �h���X�A�b�v�̏�����
//--------------------------------------------------------------
static void MUSICAL_EVENT_InitDressUp( MUSICAL_EVENT_WORK *evWork )
{
  evWork->dupInitWork = MUSICAL_DRESSUP_CreateInitWork( HEAPID_PROC_WRAPPER , evWork->musPoke , evWork->saveCtrl );
  evWork->dupInitWork->commWork = evWork->commWork;
}

//--------------------------------------------------------------
//  �h���X�A�b�v�̊J��
//--------------------------------------------------------------
static void MUSICAL_EVENT_TermDressUp( MUSICAL_EVENT_WORK *evWork )
{
  if( evWork->isComm == TRUE )
  {
    MUS_COMM_StartSendPoke( evWork->commWork , evWork->musPoke );
  }
}

//--------------------------------------------------------------
//  �V���[�p�[�g�̏�����
//--------------------------------------------------------------
static void MUSICAL_EVENT_InitActing( MUSICAL_EVENT_WORK *evWork )
{
  u8 i;
  evWork->actInitWork = MUSICAL_STAGE_CreateStageWork( HEAPID_PROC_WRAPPER , evWork->commWork );
  if( evWork->isComm == FALSE )
  {
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      if( i == 0 )
      {
        //�v���C���[
        MUSICAL_STAGE_SetData_Player( evWork->actInitWork , evWork->musicalIndex[i] , evWork->musPoke );
      }
      else
      {
        //NPC
        MUSICAL_PROGRAM_SetData_NPC( evWork->progWork , evWork->actInitWork , evWork->musicalIndex[i] , i-1 , HEAPID_PROC_WRAPPER );
      }
    }
  }
  else
  {
    u8 npcIdx = 0;
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      MUSICAL_POKE_PARAM *musPoke = MUS_COMM_GetMusPokeParam( evWork->commWork , i );
      if( musPoke != NULL )
      {
        MUSICAL_STAGE_SetData_Comm( evWork->actInitWork , i , musPoke );
      }
      else
      {
        MUSICAL_PROGRAM_SetData_NPC( evWork->progWork , evWork->actInitWork , i , npcIdx , HEAPID_PROC_WRAPPER );
        npcIdx++;
      }
    }
  }

  evWork->actInitWork->distData = evWork->distData;
  evWork->actInitWork->progWork = evWork->progWork;
  MUSICAL_PROGRAM_CalcPokemonPoint( HEAPID_PROC_WRAPPER , evWork->progWork , evWork->actInitWork );
}

//--------------------------------------------------------------
//  �V���[�p�[�g�̊J��
//--------------------------------------------------------------
static void MUSICAL_EVENT_TermActing( MUSICAL_EVENT_WORK *evWork )
{
  if( evWork->isComm == TRUE )
  {
    //�ʐM���[�N����A�s�[���{�[�i�X�̎擾
    u8 i,j;
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      MUSICAL_POKE_PARAM *musPoke = evWork->actInitWork->musPoke[i];
      for( j=0;j<MUS_COMM_APPEALBONUS_NUM;j++ )
      {
        const u8 val = MUS_COMM_GetAppealBonus( evWork->commWork , i , j );
        if( val != MUS_COMM_APPEALBONUS_INVALID )
        {
          musPoke->isApeerBonus[val] = TRUE;
        }
      }
    }
  }
  //�A�s�[���{�[�i�X�̔��f
  {
    MUS_ITEM_DATA_SYS *itemDataSys = MUS_ITEM_DATA_InitSystem( HEAPID_PROC );
    u8 i,j;
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      MUSICAL_POKE_PARAM *musPoke = evWork->actInitWork->musPoke[i];
      for( j=0;j<MUS_POKE_EQUIP_MAX;j++ )
      {
        if( musPoke->isApeerBonus[j] == TRUE )
        {
          const u8 conType = MUS_ITEM_DATA_GetItemConditionType( itemDataSys , musPoke->equip[j].itemNo );
          const u8 bonus = MUSICAL_PROGRAM_GetConOnePoint( evWork->progWork , conType );
          musPoke->point += bonus;
          ARI_TPrintf("AddAppealBonus[%d:%d(%d)]\n",i,bonus,musPoke->point);
        }
      }
    }
    
    MUS_ITEM_DATA_ExitSystem( itemDataSys );
  }
}

//--------------------------------------------------------------
//  �~���[�W�J���V���b�g�̏�����
//--------------------------------------------------------------
static void MUSICAL_EVENT_InitMusicalShot( MUSICAL_EVENT_WORK *evWork )
{
  evWork->shotInitWork = GFL_HEAP_AllocMemory( HEAPID_PROC_WRAPPER , sizeof( MUS_SHOT_INIT_WORK ));
  evWork->shotInitWork->musShotData = GFL_HEAP_AllocClearMemory( HEAPID_PROC_WRAPPER , sizeof( MUSICAL_SHOT_DATA ));
  evWork->shotInitWork->isLoadOverlay = FALSE;
  {
    u8 i;
    RTCDate date;
    MUSICAL_SHOT_DATA *shotData = evWork->shotInitWork->musShotData;
    GFL_RTC_GetDate( &date );
    shotData->bgNo = MUSICAL_PROGRAM_GetBgNo( evWork->progWork );
    shotData->year = date.year;
    shotData->month = date.month;
    shotData->day = date.day;
    shotData->title[0] = L'�X';
    shotData->title[1] = L'�g';
    shotData->title[2] = L'��';
    shotData->title[3] = L'�[';
    shotData->title[4] = L'�~';
    shotData->title[5] = L'��';
    shotData->title[6] = L'�O';
    shotData->title[7] = L'��';
    shotData->title[8] = L'��';
    shotData->title[9] = L'��';
    shotData->title[10] = GFL_STR_GetEOMCode();
    
    //@TODO
    shotData->musVer = MUSICAL_VERSION;
    shotData->pmVersion = VERSION_BLACK;
    shotData->pmLang = LANG_JAPAN;
    
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
      //FIXME ���ʂƂ�
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
  
  //����̂��߂̏���
  //���ʂ̊m��
  {
    u8 i,j;
    u8 pointArr[MUSICAL_POKE_MAX];
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      pointArr[i] = evWork->actInitWork->musPoke[i]->point;
      evWork->rankIndex[i] = i;
    }
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      for( j=0;j<MUSICAL_POKE_MAX-(i+1);j++ )
      {
        if( pointArr[j+1] > pointArr[j] )
        {
          const u8 tp = pointArr[j+1];
          const u8 ti = evWork->rankIndex[j+1];
          pointArr[j+1] = pointArr[j];
          evWork->rankIndex[j+1] = evWork->rankIndex[j];
          pointArr[j] = tp;
          evWork->rankIndex[j] = ti;
        }
      }
    }
    ARI_TPrintf("RANK:");
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      ARI_TPrintf("[%d:%d]",pointArr[i],evWork->rankIndex[i]);
    }
    ARI_TPrintf("\n");
  }
}

//--------------------------------------------------------------
//  �Z�[�u�ɕۑ�������̐ݒ�
//--------------------------------------------------------------
static void MUSICAL_EVENT_SetSaveData( MUSICAL_EVENT_WORK *evWork )
{
  //�Q���񐔁E�g�b�v��
  MUSICAL_SAVE_AddEntryNum( evWork->musSave );

  if( evWork->selfIdx == evWork->rankIndex[0] )
  {
    MUSICAL_SAVE_AddTopNum( evWork->musSave );
  }
  
  //�ō��_
  {
    const u8 point = MUSICAL_EVENT_GetPoint( evWork , evWork->selfIdx );
    MUSICAL_SAVE_SetBefPoint( evWork->musSave , point );
  }

  //�R���f�B�V�����̕ۑ�
  {
    u8 i;
    u8 conTemp[MCT_MAX] = {0};
    MUS_ITEM_DATA_SYS *itemDataSys = MUS_ITEM_DATA_InitSystem( HEAPID_PROC );
    MUSICAL_POKE_PARAM *musPoke = evWork->actInitWork->musPoke[evWork->selfIdx];
    for( i = 0 ; i < MUS_POKE_EQUIP_MAX ; i++ )
    {
      if( musPoke->equip[i].itemNo != MUSICAL_ITEM_INVALID )
      {
        const u8 conType = MUS_ITEM_DATA_GetItemConditionType( itemDataSys , musPoke->equip[i].itemNo );
        conTemp[conType]++;
      }
    }
    for( i = 0 ; i < MCT_MAX ; i++ )
    {
      MUSICAL_SAVE_SetBefCondition( evWork->musSave , i , conTemp[i] );
    }
    MUS_ITEM_DATA_ExitSystem( itemDataSys );
  }
  
  //�Ȃ��x�㏸
  {
    const ZONEID zoneId = GAMEDATA_GetMyPlayerWork(evWork->gameData)->zoneID;
    NATSUKI_Calc( evWork->pokePara , CALC_NATSUKI_MUSICAL , zoneId , HEAPID_PROC );
    
  }
}

//--------------------------------------------------------------
//  �t�@���̐��l�v�Z
//--------------------------------------------------------------
static void MUSICAL_EVENT_CalcFanState( MUSICAL_EVENT_WORK *evWork )
{
  u8 i,j;
  u8 num;
  //�Ƃ肠����������
  for( i=0;i<MUS_SAVE_FAN_NUM;i++ )
  {
    MUSICAL_FAN_STATE* fanState = MUSICAL_SAVE_GetFanState( evWork->musSave , i );
    
    fanState->type = MCT_MAX;
    fanState->giftType = MUSICAL_GIFT_TYPE_NONE;
    fanState->giftValue = 0;
  }
  
  //�l���v�Z
  if( evWork->isComm == FALSE )
  {
    //��ʐM�Ȃ̂�1�l
    num = 1;
  }
  else
  {
    num = GFL_NET_GetConnectNum();
  }
  {
    //�g�b�v�����Ή�
    const u16 selfPoint = MUSICAL_EVENT_GetPoint( evWork , evWork->selfIdx );
    const u16 topPoint  = MUSICAL_EVENT_GetPoint( evWork , evWork->rankIndex[0] );
    if( selfPoint == topPoint )
    {
      //�g�b�v�Ȃ̂Œǉ�
      num += 1;
    }
  }
  {
    const MUSICAL_CONDITION_TYPE conType = MUSICAL_PROGRAM_GetMaxConditionType( evWork->progWork );
    
    //�t�@���̏��Ԃ̃V���b�t��
    u8 fanTempArr[MUS_SAVE_FAN_NUM];
    for( i=0;i<MUS_SAVE_FAN_NUM;i++ )
    {
      fanTempArr[i] = i;
    }
    for( j=0;j<10;j++ )
    {
      for( i=0;i<MUS_SAVE_FAN_NUM;i++ )
      {
        u8 swapIdx = GFUser_GetPublicRand0(MUS_SAVE_FAN_NUM);
        u8 temp = fanTempArr[i];
        fanTempArr[i] = fanTempArr[swapIdx];
        fanTempArr[swapIdx] = temp;
      }
    }
    
    for( i=0;i<num;i++ )
    {
      const u8 idx = fanTempArr[i];
      MUSICAL_FAN_STATE* fanState = MUSICAL_SAVE_GetFanState( evWork->musSave , idx );
      const u8 point = MUSICAL_EVENT_GetPoint( evWork , evWork->selfIdx );
      ARI_TPrintf("Fan[%d]",idx);
      //�����ڂ̌���
      fanState->type = conType; //�^�C�v��n��
      ARI_TPrintf("[%d]",fanState->type);
      //���蕨�̒��I
      ARI_TPrintf("[%3d/100:",point);
      if( GFUser_GetPublicRand0(100) < point )
      {
        u16 itemIdx;
        //@todo ���������I���[�`��
        for( itemIdx = 0 ; itemIdx<100 ; itemIdx++ )
        {
          if( MUSICAL_SAVE_ChackHaveItem( evWork->musSave , itemIdx ) == FALSE )
          {
            u8 j;
            BOOL isSame = FALSE;  //���łɒ��I���ꂽ���H
            for( j=0;j<i;j++ )
            {
              MUSICAL_FAN_STATE* befFanState = MUSICAL_SAVE_GetFanState( evWork->musSave , j );
              if( befFanState->giftType == MUSICAL_GIFT_TYPE_GOODS &&
                  befFanState->giftValue == itemIdx )
              {
                isSame = TRUE;
              }
            }
            if( isSame == FALSE )
            {
              fanState->giftType = MUSICAL_GIFT_TYPE_GOODS;
              fanState->giftValue = itemIdx;
              break;
            }
          }
        }
        //�O�b�Y�������I
        if( fanState->giftType == MUSICAL_GIFT_TYPE_NONE )
        {
          fanState->giftType = MUSICAL_GIFT_TYPE_ITEM;
          fanState->giftValue = 92; //���̋�
        }
        ARI_TPrintf("%d:%d",fanState->giftType,fanState->giftValue);
      }
      ARI_TPrintf("]\n");
    }
  }
}

#pragma mark [>FieldFunc
//--------------------------------------------------------------
//  �t�B�[���h���Ă�
//--------------------------------------------------------------
static const BOOL MUSICAL_EVENT_InitField( GMEVENT *event, MUSICAL_EVENT_WORK *evWork )
{
  GAMEDATA *gameData = GAMESYSTEM_GetGameData(evWork->gsys);
  FIELD_SOUND *fieldSound = GAMEDATA_GetFieldSound( gameData );
  switch( evWork->subSeq )
  {
  case 0:
    GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(evWork->gsys, FSND_FADE_NONE, FSND_FADE_NORMAL));
    evWork->subSeq++; 
    break; 
  case 1:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(evWork->gsys));
    evWork->subSeq++;
    break;
  case 2:
    {
      //FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( evWork->gsys );
      //GMEVENT_CallEvent(event, EVENT_FieldFadeIn(evWork->gsys, fieldWork, 0));
      evWork->subSeq++;
    }
    break;
  case 3:
    GMEVENT_CallEvent(event, EVENT_FSND_WaitBGMFade(evWork->gsys));
    evWork->subSeq++;
    break;
  case 4:
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
//  �t�B�[���h�𔲂���
//--------------------------------------------------------------
static const BOOL MUSICAL_EVENT_ExitField( GMEVENT *event, MUSICAL_EVENT_WORK *evWork )
{
  GAMEDATA *gameData = GAMESYSTEM_GetGameData(evWork->gsys);
  FIELD_SOUND *fieldSound = GAMEDATA_GetFieldSound( gameData );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( evWork->gsys );
  switch( evWork->subSeq )
  {
  case 0:
    //GMEVENT_CallEvent(event, EVENT_FieldFadeOut(evWork->gsys, fieldWork, 0));
    evWork->subSeq++;
    break;
  case 1:
    GMEVENT_CallEvent(event, EVENT_FSND_PushBGM(evWork->gsys, 30));
    evWork->subSeq++;
    break; 
  case 2:
    GMEVENT_CallEvent(event, EVENT_FieldClose(evWork->gsys, fieldWork));
    evWork->subSeq++;
    break;
  case 3:
    GMEVENT_CallEvent(event, EVENT_FSND_WaitBGMFade(evWork->gsys));
    evWork->subSeq++;
    break;
  case 4:
    return TRUE;
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
  const VecFx32 pos = { FX32_CONST(40.0f) , FX32_CONST(0.0f) , FX32_CONST(360.0f) };
  
  newEvent = EVENT_ChangeMapPos( evWork->gsys, fieldWork ,
                ZONE_ID_C04R0202 , &pos , 0, FALSE );
  GMEVENT_CallEvent(event, newEvent);
}

//--------------------------------------------------------------
//  ���ֈړ�
//--------------------------------------------------------------
static const void MUSICAL_EVENT_JumpMusicalHall( GMEVENT *event, MUSICAL_EVENT_WORK *evWork )
{
  GMEVENT *newEvent;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( evWork->gsys );


  //const VecFx32 pos = { FX32_CONST(280.0f) , FX32_CONST(0.0f) , FX32_CONST(120.0f) };
  //��ROM�p
  const VecFx32 pos     = { FX32_CONST(232.0f) , FX32_CONST(0.0f) , FX32_CONST(200.0f) };
  const VecFx32 posComm = { FX32_CONST(168.0f) , FX32_CONST(0.0f) , FX32_CONST(200.0f) };
  
  if( evWork->isComm == TRUE )
  {
    newEvent = EVENT_ChangeMapPos( evWork->gsys, fieldWork , ZONE_ID_C04R0201 , &posComm , 2, FALSE );
  }
  else
  {
    newEvent = EVENT_ChangeMapPos( evWork->gsys, fieldWork , ZONE_ID_C04R0201 , &pos , 2, FALSE );
  }
  GMEVENT_CallEvent(event, newEvent);
}

//--------------------------------------------------------------
//  �X�N���v�g���Ă�
//--------------------------------------------------------------
static const void MUSICAL_EVENT_RunScript( GMEVENT *event, MUSICAL_EVENT_WORK *evWork , u16 scriptId )
{
  if( DEBUG_FLG_GetFlg( DEBUG_FLG_MusicalEventSkip ) == FALSE )
  {
    GMEVENT *newEvent;
    SCRIPT_FLDPARAM fparam;
    SCRIPT_WORK *scWork;

    newEvent = SCRIPT_SetEventScript( evWork->gsys, scriptId , NULL , HEAPID_FIELDMAP );
    scWork = SCRIPT_GetScriptWorkFromEvent( newEvent );
    
    GMEVENT_CallEvent(event, newEvent);
  }
}



#pragma mark [>outer func
//�����̎Q���ԍ��擾
const u8 MUSICAL_EVENT_GetSelfIndex( MUSICAL_EVENT_WORK *evWork )
{
  return evWork->selfIdx;
}

//�ʒu�ɑΉ������Q���ԍ��擾
const u8 MUSICAL_EVENT_GetPosIndex( MUSICAL_EVENT_WORK *evWork , const u8 pos )
{
  u8 i;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    if( evWork->musicalIndex[i] == pos )
    {
      return i;
    }
  }
  return 0;
}

//�ō��R���f�B�V�����̎擾
const u8 MUSICAL_EVENT_GetMaxCondition( MUSICAL_EVENT_WORK *evWork )
{
  return  MUSICAL_PROGRAM_GetMaxConditionType( evWork->progWork );
}

//���݉��ږ��擾
STRBUF* MUSICAL_EVENT_CreateStr_ProgramTitle( MUSICAL_EVENT_WORK *evWork , HEAPID heapId )
{
  STRBUF *strBuf;
  GFL_MSGDATA *msgHandle = GFL_MSG_Construct( evWork->distData->messageData , heapId );
  strBuf = GFL_MSG_CreateString( msgHandle , 0 );
  GFL_MSG_Delete( msgHandle );
  return strBuf;
}

//�e��]���_�擾
const u8 MUSICAL_EVENT_GetPoint( MUSICAL_EVENT_WORK *evWork , const u8 idx )
{
  const MUSICAL_POKE_PARAM *musPoke = evWork->actInitWork->musPoke[idx];
  return musPoke->point;
}

const u8 MUSICAL_EVENT_GetMaxPoint( MUSICAL_EVENT_WORK *evWork )
{
  u8 i;
  u8 maxPoint = 0;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    const MUSICAL_POKE_PARAM *musPoke = evWork->actInitWork->musPoke[i];
    if( maxPoint < musPoke->point )
    {
      maxPoint = musPoke->point;
    }
  }
  return maxPoint;
}

const u8 MUSICAL_EVENT_GetMinPoint( MUSICAL_EVENT_WORK *evWork )
{
  u8 i;
  u8 minPoint = 0xFF;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    const MUSICAL_POKE_PARAM *musPoke = evWork->actInitWork->musPoke[i];
    if( minPoint > musPoke->point )
    {
      minPoint = musPoke->point;
    }
  }
  return minPoint;
}

//�w�菇�ʂ̃L�����̈ʒu
const u8 MUSICAL_EVENT_GetPosToRank( MUSICAL_EVENT_WORK *evWork , const u8 idx )
{
  return evWork->rankIndex[idx];
}

//�L�����ʂ̃R���f�B�V�����]���_�̍������l���擾
const u8 MUSICAL_EVENT_GetMaxPointCondition( MUSICAL_EVENT_WORK *evWork , const u8 idx )
{
  u8 i;
  u8 maxPoint = 0;
  u8 maxType = 0;
  BOOL isSame = FALSE;  //�g�b�v�����_��
  for( i=0;i<MCT_MAX;i++ )
  {
    const MUSICAL_POKE_PARAM *musPoke = evWork->actInitWork->musPoke[idx];
    if( maxPoint < musPoke->conPoint[i] )
    {
      isSame = FALSE;
      maxType = i;
      maxPoint = musPoke->conPoint[i];
    }
    else
    if( maxPoint == musPoke->conPoint[i] )
    {
      isSame = TRUE;
    }
  }
  
  if( isSame == TRUE )
  {
    return MUSICAL_CONDITION_NONE;
  }
  
  return maxType;
}

const u8 MUSICAL_EVENT_GetPosObjView( MUSICAL_EVENT_WORK *evWork , const u8 pos )
{
  u8 i;
  u8 idx = MUSICAL_EVENT_GetPosIndex( evWork , pos );
  
  if( pos == evWork->selfIdx )
  {
    return NONDRAW;
  }
  else
  if( evWork->isComm == TRUE )
  {
    MYSTATUS *commMyStatus = MUS_COMM_GetPlayerMyStatus( evWork->commWork , idx );
    if( commMyStatus != NULL )
    {
      const u32 sex = MyStatus_GetMySex( commMyStatus );
      if( sex == PTL_SEX_MALE )
      {
        return HERO;
      }
      else
      {
        return HEROINE;
      }
    }
  }
  
  return MUSICAL_PROGRAM_GetNpcObjId( evWork->progWork , idx-1 );
}

void MUSICAL_EVENT_SetPosCharaName_Wordset( MUSICAL_EVENT_WORK *evWork , const u8 pos , WORDSET *wordSet , const u8 wordIdx )
{
  u8 i;
  BOOL isSet = FALSE;
  u8 idx = MUSICAL_EVENT_GetPosIndex( evWork , pos );

  if( pos == evWork->selfIdx )
  {
    MYSTATUS *myStatus = GAMEDATA_GetMyStatus( evWork->gameData );
    WORDSET_RegisterPlayerName( wordSet , wordIdx , myStatus );
    isSet = TRUE;
  }
  else
  if( evWork->isComm == TRUE )
  {
    MYSTATUS *commMyStatus = MUS_COMM_GetPlayerMyStatus( evWork->commWork , idx );
    if( commMyStatus != NULL )
    {
      WORDSET_RegisterPlayerName( wordSet , wordIdx , commMyStatus );
      isSet = TRUE;
    }
  }
  
  if( isSet == FALSE )
  {
    const u8 nameIdx = MUSICAL_PROGRAM_GetNpcNameIdx( evWork->progWork , idx-1 );
    GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_extra_dat , HEAPID_PROC );
    STRBUF * tmpBuf = GFL_MSG_CreateString( msgHandle , nameIdx );
    
    //@TODO �C�O�Œj���Ή�
    WORDSET_RegisterWord( wordSet, wordIdx, tmpBuf, 0, TRUE, PM_LANG );

    GFL_STR_DeleteBuffer( tmpBuf );
    GFL_MSG_Delete( msgHandle );
  }
}


//���ڃf�[�^��M��ɐ��l���v�Z����
void MUSICAL_EVENT_CalcProgramData( MUSICAL_EVENT_WORK *evWork )
{
  u32 conPointArr = MUS_COMM_GetConditionPointArr( evWork->commWork );
  u32 npcArr = MUS_COMM_GetNpcArr( evWork->commWork );
  evWork->progWork = MUSICAL_PROGRAM_InitProgramData( HEAPID_PROC_WRAPPER , evWork->distData , 0 );
  MUSICAL_PROGRAM_SetConditionPointArr( evWork->progWork , conPointArr );
  MUSICAL_PROGRAM_SetNpcArr( evWork->progWork , npcArr );
}
