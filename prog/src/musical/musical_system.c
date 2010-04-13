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

#include "arc_def.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "snd_strm.naix"

#include "savedata/musical_save.h"
#include "savedata/musical_dist_save.h"
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
  MPS_INIT_DRESSUP_SEND_MUSICAL_IDX,
  MPS_INIT_DRESSUP_SEND_STRM,
  MPS_INIT_DRESSUP,
  MPS_TERM_DRESSUP,
  MPS_INIT_ACTING,
  MPS_INIT_ACTING_COMM,
  MPS_TERM_ACTING,
  
  MPS_WAIT_MEMBER_DRESSUP,
  MPS_SEND_MUS_POKE,
  MPS_POST_WAIT_MUS_POKE,
  MPS_POST_WAIT_ALL_MUS_POKE,
  MPS_POST_WAIT_START_ACTING,
  
  //�J�n�O�����f�[�^
  MPS_SEND_PROGRAM_SIZE,
  MPS_WAIT_SEND_PROGRAM_SIZE,
  MPS_WAIT_SEND_PROGRAM_DATA,
  MPS_WAIT_SEND_MESSAGE_SIZE,
  MPS_WAIT_SEND_MESSAGE_DATA,
  MPS_WAIT_SEND_SCRIPT_SIZE,
  MPS_WAIT_SEND_SCRIPT_DATA,
  
  MPS_FINISH,
}MUSICAL_PROC_STATE;



//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  GFL_PROCSYS *procSys;
  
  MUSICAL_DISTRIBUTE_DATA *distData;
  
  MUSICAL_PROC_STATE state;
  MUS_COMM_WORK     *commWork;
  DRESSUP_INIT_WORK *dupInitWork;
  STAGE_INIT_WORK *actInitWork;
  
  MUSICAL_PROGRAM_WORK *progWork;
  MUSICAL_POKE_PARAM *musPoke;
}MUSICAL_PROC_WORK;
//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
/*
static GFL_PROC_RESULT MusicalProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MusicalProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MusicalProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA Musical_ProcData =
{
  MusicalProc_Init,
  MusicalProc_Main,
  MusicalProc_Term
};
*/

//�~���[�W�J���̎Q�����i�����邩���ׂ�
const BOOL  MUSICAL_SYSTEM_CheckEntryMusicalPokeNo( const u16 mons_no )
{
  return TRUE;//�S���o���I
}

const BOOL  MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara )
{
  //�^�}�S�Ƃ�
  if( PP_Get( pokePara , ID_PARA_tamago_exist , NULL ) == TRUE )
  {
    return FALSE;
  }
  
  return TRUE;
}

//�~���[�W�J���ł̔ԍ��ɕϊ�
const u16 MUSICAL_SYSTEM_ChangeMusicalPokeNumber( const u16 mons_no )
{
  return mons_no;
}

//�~���[�W�J���p�p�����[�^�̏�����
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPoke( POKEMON_PARAM *pokePara , HEAPID heapId )
{
  int i;
  MUSICAL_POKE_PARAM *musPara;
  musPara = GFL_HEAP_AllocClearMemory( heapId , sizeof(MUSICAL_POKE_PARAM) );
  musPara->charaType = MUS_CHARA_INVALID;
  musPara->pokePara = pokePara;
  musPara->point = 0;
  
  for( i=0;i<MUS_POKE_EQUIP_MAX;i++ )
  {
    musPara->equip[i].itemNo = MUSICAL_ITEM_INVALID;
    musPara->equip[i].angle = 0;
    musPara->isApeerBonus[i] = FALSE;
  }
  
  musPara->mcssParam.monsno = PP_Get( pokePara, ID_PARA_monsno,	NULL );
  musPara->mcssParam.sex    = PP_Get( pokePara, ID_PARA_sex,	NULL );
  musPara->mcssParam.form   = PP_Get( pokePara, ID_PARA_form_no,	NULL );
  musPara->mcssParam.rare   = PP_CheckRare( pokePara );
  
  return musPara;
}
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPokeParam( u16 monsno , u8 sex , u8 form , u8 rare , HEAPID heapId )
{
  int i;
  MUSICAL_POKE_PARAM *musPara;
  musPara = GFL_HEAP_AllocClearMemory( heapId , sizeof(MUSICAL_POKE_PARAM) );
  musPara->charaType = MUS_CHARA_INVALID;
  musPara->pokePara = NULL;
  musPara->point = 0;
  
  for( i=0;i<MUS_POKE_EQUIP_MAX;i++ )
  {
    musPara->equip[i].itemNo = MUSICAL_ITEM_INVALID;
    musPara->equip[i].angle = 0;
    musPara->isApeerBonus[i] = FALSE;
  }
  for( i=0;i<MCT_MAX;i++ )
  {
    musPara->conPoint[i] = 0;
  }
  musPara->npcAppealTime = NPC_APPEAL_TIME_NONE;
  
  musPara->mcssParam.monsno = monsno;
  musPara->mcssParam.sex    = sex;
  musPara->mcssParam.form = form;
  musPara->mcssParam.rare   = rare;
  
  return musPara;
}

#pragma mark [>distribute data
MUSICAL_DISTRIBUTE_DATA* MUSICAL_SYSTEM_InitDistributeData( HEAPID workHeapId )
{
  MUSICAL_DISTRIBUTE_DATA *distData = GFL_HEAP_AllocMemory( workHeapId , sizeof( MUSICAL_DISTRIBUTE_DATA ) );

  distData->programData = NULL;
  distData->messageData = NULL;
  distData->scriptData = NULL;
  distData->midiData = NULL;
  distData->midiSeqData = NULL;
  distData->midiBnkData = NULL;
  distData->midiWaveData = NULL;

  return distData;
}

void MUSICAL_SYSTEM_TermDistributeData( MUSICAL_DISTRIBUTE_DATA *distData )
{
  if( distData->programData != NULL )
  {
    GFL_HEAP_FreeMemory( distData->programData );
  }
  if( distData->messageData != NULL )
  {
    GFL_HEAP_FreeMemory( distData->messageData );
  }
  if( distData->scriptData != NULL )
  {
    GFL_HEAP_FreeMemory( distData->scriptData );
  }
  //Seq�EBnk�EWave�̓p�b�N���Ă���
  if( distData->midiData != NULL )
  {
    GFL_HEAP_FreeMemory( distData->midiData );
  }

  GFL_HEAP_FreeMemory( distData );
}

void MUSICAL_SYSTEM_LoadDistributeData( MUSICAL_DISTRIBUTE_DATA *distData , SAVE_CONTROL_WORK *saveWork , GAMEDATA *gamedata, const u8 programNo , HEAPID heapId )
{
  ARCHANDLE *arcHandle;
  MUSICAL_DIST_SAVE *distSave;
  MUS_DIST_MIDI_HEADER midiSizeHeader;
  MUS_DIST_MIDI_HEADER *pMidiSizeHeader;
  distData->programNo = programNo;
  if( programNo >= MUS_PROGRAM_LOCAL_NUM )
  {
    void *musArc;
    u32 arcSize;
    distSave = MUSICAL_DIST_SAVE_LoadData( gamedata , heapId );
    musArc = MUSICAL_DIST_SAVE_GetMusicalArc( distSave );
    arcSize = MUSICAL_DIST_SAVE_GetMusicalArcSize( distSave );
    arcHandle = GFL_ARC_OpenDataHandleByMemory( musArc , arcSize , GFL_HEAP_LOWID(heapId) );
  }
  else
  {
    arcHandle = GFL_ARC_OpenDataHandle( ARCID_MUSICAL_PROGRAM_01+programNo , GFL_HEAP_LOWID(heapId) );
  }
  distData->programData = GFL_ARCHDL_UTIL_LoadEx( arcHandle , MUSICAL_ARCDATAID_PROGDATA , FALSE , heapId , &distData->programDataSize );
  distData->messageData = GFL_ARCHDL_UTIL_LoadEx( arcHandle , MUSICAL_ARCDATAID_GMMDATA , FALSE , heapId , &distData->messageDataSize );
  distData->scriptData = GFL_ARCHDL_UTIL_LoadEx( arcHandle , MUSICAL_ARCDATAID_SCRIPTDATA , FALSE , heapId , &distData->scriptDataSize );

  midiSizeHeader.seqSize  = GFL_ARC_GetDataSizeByHandle( arcHandle , MUSICAL_ARCDATAID_SSEQDATA );
  midiSizeHeader.bankSize = GFL_ARC_GetDataSizeByHandle( arcHandle , MUSICAL_ARCDATAID_SBNKDATA );
  midiSizeHeader.waveSize = GFL_ARC_GetDataSizeByHandle( arcHandle , MUSICAL_ARCDATAID_SWAVDATA );
  
  distData->midiDataSize = sizeof(MUS_DIST_MIDI_HEADER) + midiSizeHeader.seqSize + midiSizeHeader.bankSize + midiSizeHeader.waveSize;
  
  distData->midiData = GFL_HEAP_AllocClearMemory( heapId , distData->midiDataSize );
  
  pMidiSizeHeader = distData->midiData;
  distData->midiSeqData  = (void*)((u32)distData->midiData + sizeof(MUS_DIST_MIDI_HEADER));
  distData->midiBnkData  = (void*)((u32)distData->midiData + sizeof(MUS_DIST_MIDI_HEADER) + midiSizeHeader.seqSize );
  distData->midiWaveData = (void*)((u32)distData->midiData + sizeof(MUS_DIST_MIDI_HEADER) + midiSizeHeader.seqSize + midiSizeHeader.bankSize );
  
  pMidiSizeHeader->seqSize  = midiSizeHeader.seqSize;
  pMidiSizeHeader->bankSize = midiSizeHeader.bankSize;
  pMidiSizeHeader->waveSize = midiSizeHeader.waveSize;
  
  GFL_ARC_LoadDataByHandle( arcHandle , MUSICAL_ARCDATAID_SSEQDATA , distData->midiSeqData );
  GFL_ARC_LoadDataByHandle( arcHandle , MUSICAL_ARCDATAID_SBNKDATA , distData->midiBnkData );
  GFL_ARC_LoadDataByHandle( arcHandle , MUSICAL_ARCDATAID_SWAVDATA , distData->midiWaveData );
  
  GFL_ARC_CloseDataHandle( arcHandle );
  if( programNo >= MUS_PROGRAM_LOCAL_NUM )
  {
    MUSICAL_DIST_SAVE_UnloadData( distSave );
  }

  ARI_TPrintf("MusicalSystem LoadMidiData[%d][%d][%d].\n",pMidiSizeHeader->seqSize,pMidiSizeHeader->bankSize,pMidiSizeHeader->waveSize);
}
