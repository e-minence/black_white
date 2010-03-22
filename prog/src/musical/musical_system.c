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

static const u16 musPokeArr[]=
{
  MONSNO_PIKATYUU,
  MONSNO_PIKUSII,
  MONSNO_WARUBIARU,
  MONSNO_509,
  MONSNO_PURUNSU,
  MONSNO_MUNNA,  
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
  //return TRUE;    //�� FIXME
  return FALSE;
}

const BOOL  MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara )
{
  u16 mons_no = PP_Get( pokePara, ID_PARA_monsno, NULL );
  //FIXME �^�}�S�Ƃ�
  return MUSICAL_SYSTEM_CheckEntryMusicalPokeNo( mons_no );
}

//�~���[�W�J���ł̔ԍ��ɕϊ�
const u16 MUSICAL_SYSTEM_ChangeMusicalPokeNumber( const u16 mons_no )
{
  u16 i=0;

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

//�����_���Ń~���[�W�J���Q���\�ȃ|�P������Ԃ�
const u16 MUSICAL_SYSTEM_GetMusicalPokemonRandom( void )
{
  u16 i=0;

  while( 0xFFFF != musPokeArr[i] )
  {
    i++;
  }
  return musPokeArr[ GFUser_GetPublicRand0(i) ];
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
  musPara->mcssParam.sex    = PP_Get( pokePara, ID_PARA_form_no,	NULL );
  musPara->mcssParam.form = PP_Get( pokePara, ID_PARA_sex,	NULL );
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

void MUSICAL_SYSTEM_LoadDistributeData( MUSICAL_DISTRIBUTE_DATA *distData , SAVE_CONTROL_WORK *saveWork , const u8 programNo , HEAPID heapId )
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
    distSave = MUSICAL_DIST_SAVE_LoadData( saveWork , heapId );
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

#if PM_DEBUG
const BOOL MUSICAL_SAVE_CreateDummyData( MUSICAL_SHOT_DATA* shotData , const u16 monsNo , const HEAPID heapId )
{
  u8 i;
  RTCDate date;
  POKEMON_PERSONAL_DATA *perData;

  if( MUSICAL_SYSTEM_CheckEntryMusicalPokeNo(monsNo) == FALSE )
  {
    return FALSE;
  }
  perData = POKE_PERSONAL_OpenHandle( monsNo , 0 ,heapId );

  GFL_RTC_GetDate( &date );
  shotData->bgNo = 0;
  shotData->year = date.year;
  shotData->month = date.month;
  shotData->day = date.day;
  shotData->title[0] = L'�_';
  shotData->title[1] = L'�~';
  shotData->title[2] = L'�[';
  shotData->title[3] = L'�V';
  shotData->title[4] = L'��';
  shotData->title[5] = L'�b';
  shotData->title[6] = L'�g';
  shotData->title[7] = L'�f';
  shotData->title[8] = L'�[';
  shotData->title[9] = L'�^';
  shotData->title[10] = GFL_STR_GetEOMCode();
  shotData->player = 0;
  
  shotData->musVer = MUSICAL_VERSION;
  shotData->pmVersion = VERSION_BLACK;
  shotData->pmLang = LANG_JAPAN;
  shotData->spotBit = 1;
  
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    u8 j;
    shotData->shotPoke[i].monsno = monsNo;
    switch( POKE_PERSONAL_GetParam(perData,POKEPER_ID_sex) )
    {
    case POKEPER_SEX_MALE:
      shotData->shotPoke[i].sex = PTL_SEX_MALE;
      break;
    case POKEPER_SEX_FEMALE:
      shotData->shotPoke[i].sex = PTL_SEX_FEMALE;
      break;
    case POKEPER_SEX_UNKNOWN:
      shotData->shotPoke[i].sex = PTL_SEX_UNKNOWN;
      break;
    default:
      shotData->shotPoke[i].sex = PTL_SEX_MALE;
      break;
    }
    shotData->shotPoke[i].rare = 0;
    shotData->shotPoke[i].form = 0;
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
  }
  
  POKE_PERSONAL_CloseHandle( perData );
  return TRUE;
}
#endif