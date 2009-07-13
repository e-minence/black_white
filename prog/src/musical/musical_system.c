//======================================================================
/**
 * @file  musical_system.h
 * @brief �~���[�W�J���p �ėp��`
 * @author  ariizumi
 * @data  09/02/10
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "savedata/musical_save.h"
#include "musical/musical_system.h"
#include "musical/musical_dressup_sys.h"
#include "musical/musical_stage_sys.h"
#include "musical/comm/mus_comm_lobby.h"
#include "musical/comm/mus_comm_func.h"
#include "musical/stage/sta_acting.h"
#include "musical/dressup/dup_local_def.h"
#include "poke_tool/monsno_def.h"

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
  MPS_INIT_LOBBY,
  MPS_TERM_LOBBY,
  MPS_INIT_DRESSUP,
  MPS_TERM_DRESSUP,
  MPS_INIT_ACTING,
  MPS_INIT_ACTING_COMM,
  MPS_TERM_ACTING,
  
  MPS_WAIT_MEMBER_DRESSUP,
  MPS_SEND_MUS_POKE,
  MPS_POST_WAIT_MUS_POKE,
  MPS_POST_WAIT_START_ACTING,
  
  MPS_FINISH,
}MUSICAL_PROC_STATE;

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  GFL_PROCSYS *procSys;
  
  MUSICAL_PROC_STATE state;
  MUS_COMM_WORK     *commWork;
  DRESSUP_INIT_WORK *dupInitWork;
  STAGE_INIT_WORK *actInitWork;
  
  MUSICAL_POKE_PARAM *musPoke;
}MUSICAL_PROC_WORK;
//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
static GFL_PROC_RESULT MusicalProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MusicalProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MusicalProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk );


GFL_PROC_DATA Musical_ProcData =
{
  MusicalProc_Init,
  MusicalProc_Main,
  MusicalProc_Term
};



static const u16 musPokeArr[]=
{
  MONSNO_PIKATYUU,
  MONSNO_RAITYUU,
  MONSNO_PIPPI,
  MONSNO_PIKUSII,
  MONSNO_PURIN,
  MONSNO_EREBUU,
  494,
  495,
  
  1000, //�v�����Z�X
  0xFFFF,
};
//�~���[�W�J���̎Q�����i�����邩���ׂ�(��)
const BOOL  MUSICAL_SYSTEM_CheckEntryMusicalPokeNo( const u16 mons_no )
{
  u16 i=0;
  while( 0xFFFF != musPokeArr[i] )
  {
    if( mons_no == musPokeArr[i] )
    {
      return TRUE;
    }
    i++;
  }
  return FALSE;
}

const BOOL  MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara )
{
  u16 mons_no = PP_Get( pokePara, ID_PARA_monsno, NULL );
  return MUSICAL_SYSTEM_CheckEntryMusicalPokeNo( mons_no );
}

//�~���[�W�J���ł̔ԍ��ɕϊ�
const u16 MUSICAL_SYSTEM_ChangeMusicalPokeNumber( POKEMON_PARAM *pokePara )
{
  u16 i=0;
  int mons_no = PP_Get( pokePara, ID_PARA_monsno, NULL );

  while( 0xFFFF != musPokeArr[i] )
  {
    if( mons_no == musPokeArr[i] )
    {
      return i;
    }
    i++;
  }
  //�ꉞ�ŏI�ԍ��ɕϊ�
  return i-1;
}

//�~���[�W�J���p�p�����[�^�̏�����
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPoke( POKEMON_PARAM *pokePara , HEAPID heapId )
{
  int i;
  MUSICAL_POKE_PARAM *musPara;
  musPara = GFL_HEAP_AllocMemory( heapId , sizeof(MUSICAL_POKE_PARAM) );
  musPara->charaType = MUS_CHARA_INVALID;
  musPara->pokePara = pokePara;
  
  for( i=0;i<MUS_POKE_EQUIP_MAX;i++ )
  {
    musPara->equip[i].itemNo = MUSICAL_ITEM_INVALID;
    musPara->equip[i].angle = 0;
  }
  
  return musPara;
}



#pragma mark [>proc 
static GFL_PROC_RESULT MusicalProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MUSICAL_INIT_WORK *initWork = pwk;
  MUSICAL_PROC_WORK *work;
  
  GFL_OVERLAY_Load(FS_OVERLAY_ID(musical));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MUSICAL_PROC|HEAPDIR_MASK, 0x8000 );

  work = GFL_PROC_AllocWork( proc, sizeof(MUSICAL_PROC_WORK), HEAPID_MUSICAL_PROC );
  work->musPoke = MUSICAL_SYSTEM_InitMusPoke( initWork->pokePara , HEAPID_MUSICAL_PROC );
  work->dupInitWork = NULL;
  work->actInitWork = NULL;

  work->procSys = GFL_PROC_LOCAL_boot( HEAPID_MUSICAL_PROC );
  
  if( initWork->isComm == TRUE )
  {
    work->state = MPS_INIT_LOBBY;
    work->commWork = MUS_COMM_CreateWork( HEAPID_MUSICAL_PROC , initWork->gameComm , initWork->saveCtrl);
  }
  else
  {
    work->state = MPS_INIT_DRESSUP;
    work->commWork = NULL;
  }
  return GFL_PROC_RES_FINISH;
  
}

static GFL_PROC_RESULT MusicalProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MUSICAL_INIT_WORK *initWork = pwk;
  MUSICAL_PROC_WORK *work = mywk;
  
  if( work->commWork != NULL )
  {
    MUS_COMM_DeleteWork( work->commWork );
    work->commWork = NULL;
  }
  
  if( GFL_NET_IsExit() == FALSE )
  {
    return GFL_PROC_RES_CONTINUE;
  }
  
  if( work->dupInitWork != NULL )
  {
    MUSICAL_DRESSUP_DeleteInitWork( work->dupInitWork );
  }
  if( work->actInitWork != NULL )
  {
    MUSICAL_STAGE_DeleteStageWork( work->actInitWork );
  }
  GFL_PROC_LOCAL_Exit( work->procSys );
  GFL_HEAP_FreeMemory( work->musPoke );
  
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MUSICAL_PROC );
  
  if( initWork->isDebug == TRUE )
  {
    GFL_HEAP_FreeMemory( initWork->pokePara );
    GFL_HEAP_FreeMemory( initWork );
  }


  GFL_OVERLAY_Unload(FS_OVERLAY_ID(musical));
  
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT MusicalProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MUSICAL_INIT_WORK *initWork = pwk;
  MUSICAL_PROC_WORK *work = mywk;
  
  const BOOL isActiveProc = GFL_PROC_LOCAL_Main( work->procSys );
  
  switch( work->state )
  {
  case MPS_INIT_LOBBY:
    GFL_PROC_LOCAL_CallProc( work->procSys , NO_OVERLAY_ID, &MUS_LOBBY_ProcData, work->commWork );
    work->state = MPS_TERM_LOBBY;
    break;
    
  case MPS_TERM_LOBBY:
    if( isActiveProc == FALSE )
    {
      switch( MUS_COMM_GetMode( work->commWork ) )
      {
      case MCM_NONE:
        work->state = MPS_FINISH;
        break;
      
      default:  //��
        work->state = MPS_INIT_DRESSUP;
        break;
      }
    }
    break;
    
  case MPS_INIT_DRESSUP:
    work->dupInitWork = MUSICAL_DRESSUP_CreateInitWork( HEAPID_MUSICAL_PROC , work->musPoke , initWork->saveCtrl );
    GFL_PROC_LOCAL_CallProc( work->procSys , NO_OVERLAY_ID, &DressUp_ProcData, work->dupInitWork );
    work->state = MPS_TERM_DRESSUP;
    break;

  case MPS_TERM_DRESSUP:
    if( isActiveProc == FALSE )
    {
      if( initWork->isComm == TRUE )
      {
        if( MUS_COMM_SetCommGameState( work->commWork , MCGS_WAIT_DRESSUP ) == TRUE )
        {
          work->state = MPS_WAIT_MEMBER_DRESSUP;
          MUS_COMM_SendTimingCommand( work->commWork , MUS_COMM_TIMMING_DRESSUP_WAIT );
        }
      }
      else
      {
        work->state = MPS_INIT_ACTING;
      }
    }
    break;
  
    //�����o�[�҂�
  case MPS_WAIT_MEMBER_DRESSUP:
    if( MUS_COMM_CheckTimingCommand( work->commWork , MUS_COMM_TIMMING_DRESSUP_WAIT ) == TRUE )
    {
      work->state = MPS_SEND_MUS_POKE;
    }
    break;
    
    //MusPoke�𑗂�
  case MPS_SEND_MUS_POKE:
    if( MUS_COMM_Send_MusPokeData( work->commWork , work->musPoke ) == TRUE )
    {
      work->state = MPS_POST_WAIT_MUS_POKE;
    }
    break;

    //�S�������炤�̂�҂�
  case MPS_POST_WAIT_MUS_POKE:
    if( MUS_COMM_CheckAllPostPokeData( work->commWork ) == TRUE )
    {
      MUS_COMM_SendTimingCommand( work->commWork , MUS_COMM_TIMMING_START_ACTING );
      work->state = MPS_POST_WAIT_START_ACTING;
    }
    break;
  
    //�S�����낤�̂�҂�
  case MPS_POST_WAIT_START_ACTING:
    if( MUS_COMM_CheckTimingCommand( work->commWork , MUS_COMM_TIMMING_START_ACTING ) == TRUE )
    {
      work->state = MPS_INIT_ACTING_COMM;
    }
    break;

  case MPS_INIT_ACTING:
    {
      u8 ePos,i;
      OS_TPrintf("FreeHeap:[%x][%x]\n", 
          GFL_HEAP_GetHeapFreeSize( GFL_HEAPID_APP ) ,
          GFI_HEAP_GetHeapAllocatableSize( GFL_HEAPID_APP ) );
      work->actInitWork = MUSICAL_STAGE_CreateStageWork( HEAPID_MUSICAL_PROC , work->commWork );
      MUSICAL_STAGE_SetData_Player( work->actInitWork , 0 , work->musPoke );
      MUSICAL_STAGE_SetData_NPC( work->actInitWork , 1 , MONSNO_RAITYUU , HEAPID_MUSICAL_PROC );
      MUSICAL_STAGE_SetData_NPC( work->actInitWork , 2 , MONSNO_EREBUU  , HEAPID_MUSICAL_PROC );
      MUSICAL_STAGE_SetData_NPC( work->actInitWork , 3 , MONSNO_RUKARIO , HEAPID_MUSICAL_PROC );

      MUSICAL_STAGE_SetEquip( work->actInitWork , 1 , MUS_POKE_EQU_EAR_L , 7 , 0 );
      MUSICAL_STAGE_SetEquip( work->actInitWork , 1 , MUS_POKE_EQU_BODY  , 9 , 0 );

      MUSICAL_STAGE_SetEquip( work->actInitWork , 2 , MUS_POKE_EQU_HAND_R , 31 , 0 );
      MUSICAL_STAGE_SetEquip( work->actInitWork , 2 , MUS_POKE_EQU_HEAD   , 15 , 0 );

      MUSICAL_STAGE_SetEquip( work->actInitWork , 3 , MUS_POKE_EQU_HAND_R , 30 , 0 );
      MUSICAL_STAGE_SetEquip( work->actInitWork , 3 , MUS_POKE_EQU_HEAD   , 21 , 0 );
    }
    GFL_PROC_LOCAL_CallProc( work->procSys , NO_OVERLAY_ID, &MusicalStage_ProcData, work->actInitWork );
    work->state = MPS_TERM_ACTING;
    break;

  case MPS_INIT_ACTING_COMM:
    {
      u8 i;
      work->actInitWork = MUSICAL_STAGE_CreateStageWork( HEAPID_MUSICAL_PROC , work->commWork );
      for( i=0;i<MUSICAL_POKE_MAX;i++ )
      {
        MUSICAL_POKE_PARAM *musPoke = MUS_COMM_GetMusPokeParam( work->commWork , i );
        if( musPoke != NULL )
        {
          MUSICAL_STAGE_SetData_Comm( work->actInitWork , i , musPoke );
        }
        else
        {
          MUSICAL_STAGE_SetData_NPC( work->actInitWork , i , MONSNO_PIKATYUU  , HEAPID_MUSICAL_PROC );
        }
      }
      GFL_PROC_LOCAL_CallProc( work->procSys , NO_OVERLAY_ID, &MusicalStage_ProcData, work->actInitWork );
      work->state = MPS_TERM_ACTING;
    }
    
    break;

  case MPS_TERM_ACTING:
    if( isActiveProc == FALSE )
    {
      work->state = MPS_FINISH;
    }
    break;
    
  case MPS_FINISH:
    return GFL_PROC_RES_FINISH;
    break;
  }
  
  if( work->commWork != NULL )
  {
    MUS_COMM_UpdateComm( work->commWork );
  }
  
  return GFL_PROC_RES_CONTINUE;
}

