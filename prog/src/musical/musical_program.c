//======================================================================
/**
 * @file  musical_program.c
 * @brief �~���[�W�J���p ����
 * @author  ariizumi
 * @data  09/08/21
 *
 * ���W���[�����FMUSICAL_PROGRAM
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"

#include "musical_program.h"
#include "musical/musical_stage_sys.h"
#include "musical/mus_item_data.h"
#include "musical/stage/sta_local_def.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MUS_PROG_DATA_EQUIP_NUM (8)
#define MUS_PROG_DATA_POKE_NUM (3)

#define MUS_PROG_DATA_EQUIP_NONE (511)
#define MUS_PROG_DATA_EQUIP_RANDOM (510)
#define MUS_PROG_DATA_POKE_RANDOM (65535)
//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//�o���|�P�����f�[�^
typedef struct
{
  u16 itemNo;
  u8  itemPos;
  u8 pad;
}MUSICAL_PROGRAM_EQUIP_DATA;

//�o���|�P�f�[�^
typedef struct
{
  u16 monsno;
  u8  trainerType;
  u8  trainerName;
  MUSICAL_PROGRAM_EQUIP_DATA equipData[MUS_PROG_DATA_EQUIP_NUM];
}MUSICAL_PROGRAM_POKE_DATA;

//���ڂ̃f�[�^
typedef struct 
{
  u8 bgNo;
  u8 condition[MCT_DATA_MAX];
  u8 rewardType;
  u8 pad;
  u16 rewardValue;
  MUSICAL_PROGRAM_POKE_DATA pokeData[MUS_PROG_DATA_POKE_NUM];
  
}MUSICAL_PROGRAM_DATA;

//���ڂ̃��[�N
struct _MUSICAL_PROGRAM_WORK
{
  u8 condition[MCT_MAX];
  
  MUSICAL_PROGRAM_DATA *progData;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	�~���[�W�J���f�[�^�̓ǂݍ���
//--------------------------------------------------------------
MUSICAL_PROGRAM_WORK* MUSICAL_PROGRAM_InitProgramData( HEAPID heapId , MUSICAL_DISTRIBUTE_DATA *distData )
{
  u8 i;
  MUSICAL_PROGRAM_WORK *progWork = GFL_HEAP_AllocMemory( heapId , sizeof( MUSICAL_PROGRAM_WORK ) );
//  MUSICAL_PROGRAM_DATA *progData = GFL_HEAP_AllocMemory( heapId , sizeof( MUSICAL_PROGRAM_DATA ) );
  progWork->progData = distData->programData;

  for( i=0;i<MCT_MAX;i++ )
  {
    progWork->condition[i] = 0;
  }
  
  ARI_TPrintf("--Condition--\n");
  ARI_TPrintf("Cool   [%3d]\n",progWork->progData->condition[0]);
  ARI_TPrintf("Cute   [%3d]\n",progWork->progData->condition[1]);
  ARI_TPrintf("Elegant[%3d]\n",progWork->progData->condition[2]);
  ARI_TPrintf("Unique [%3d]\n",progWork->progData->condition[3]);
  ARI_TPrintf("Random [%3d]\n",progWork->progData->condition[4]);
  ARI_TPrintf("--Condition--\n");
  

  //�f�[�^���烏�[�N�փR�s�[
  for( i=0;i<MCT_MAX;i++ )
  {
    progWork->condition[i] = progWork->progData->condition[i];
  }
  //�����_�����̕��z
  {
    u8 point = progWork->progData->condition[MCT_RANDOM]/5;
    while( point > 0 )
    {
      u8 val = GFL_STD_MtRand( 3 )+1;
      if( val > point )
      {
        val = point;
      }
      
      progWork->condition[GFL_STD_MtRand(MCT_MAX)] += val*5;
      point -= val;
    }
  }
  ARI_TPrintf("--FinalCondition--\n");
  ARI_TPrintf("Cool   [%3d]\n",progWork->condition[0]);
  ARI_TPrintf("Cute   [%3d]\n",progWork->condition[1]);
  ARI_TPrintf("Elegant[%3d]\n",progWork->condition[2]);
  ARI_TPrintf("Unique [%3d]\n",progWork->condition[3]);
  ARI_TPrintf("--FinalCondition--\n");

  return progWork;
}

//--------------------------------------------------------------
//	�~���[�W�J���f�[�^�̔j��
//--------------------------------------------------------------
void MUSICAL_PROGRAM_TermProgramData( MUSICAL_PROGRAM_WORK *progWork )
{
  GFL_HEAP_FreeMemory( progWork );
}

//--------------------------------------------------------------
//	���_�z���̌���
//--------------------------------------------------------------
void MUSICAL_PROGRAM_CalcPokemonPoint( HEAPID heapId , MUSICAL_PROGRAM_WORK* progWork , STAGE_INIT_WORK *actInitWork )
{
  u8 conIdx;
  u8 pokeIdx;
  u8 pokeCondition[MUSICAL_POKE_MAX][MCT_MAX];
  u8 conditionMax[MCT_MAX];
  MUS_ITEM_DATA_SYS *itemDataSys = MUS_ITEM_DATA_InitSystem( heapId );
  
  for( conIdx=0;conIdx<MCT_MAX;conIdx++ )
  {
    for( pokeIdx = 0 ; pokeIdx < MUSICAL_POKE_MAX ; pokeIdx++ )
    {
      pokeCondition[pokeIdx][conIdx] = 0;
    }
    conditionMax[conIdx] = 0;
  }

  for( pokeIdx = 0 ; pokeIdx < MUSICAL_POKE_MAX ; pokeIdx++ )
  {
    u8 ePos;
    MUSICAL_POKE_PARAM *musPoke = actInitWork->musPoke[pokeIdx];
    for( ePos = 0 ; ePos < MUS_POKE_EQUIP_MAX ; ePos++ )
    {
      if( musPoke->equip[ePos].itemNo != MUSICAL_ITEM_INVALID )
      {
        const u8 conType = MUS_ITEM_DATA_GetItemConditionType( itemDataSys , musPoke->equip[ePos].itemNo );
        
        pokeCondition[pokeIdx][conType]++;
        conditionMax[conType]++;
      }
    }
  }
  
  for( pokeIdx = 0 ; pokeIdx < MUSICAL_POKE_MAX ; pokeIdx++ )
  {
    MUSICAL_POKE_PARAM *musPoke = actInitWork->musPoke[pokeIdx];
    ARI_TPrintf("Poke[%d]:",pokeIdx);
    for( conIdx=0;conIdx<MCT_MAX;conIdx++ )
    {
      const u8 addPoint = progWork->condition[conIdx] * pokeCondition[pokeIdx][conIdx] / conditionMax[conIdx];
      musPoke->point += addPoint;
      musPoke->conPoint[conIdx] = addPoint;
      ARI_TPrintf("[%3d]",addPoint);
    }
    ARI_TPrintf("Sum[%3d]\n",musPoke->point);
  }
  
  
  MUS_ITEM_DATA_ExitSystem( itemDataSys );
}

//--------------------------------------------------------------
//	�R���f�B�V�����z���̎擾
//--------------------------------------------------------------
const u8 MUSICAL_PROGRAM_GetConditionPoint( MUSICAL_PROGRAM_WORK* progWork , MUSICAL_CONDITION_TYPE conType )
{
  return progWork->condition[conType];
}

//--------------------------------------------------------------
//	�ō��R���f�B�V�����̎擾
//--------------------------------------------------------------
const u8 MUSICAL_PROGRAM_GetMaxConditionType( MUSICAL_PROGRAM_WORK* progWork )
{
  u8 i;
  u8 maxPoint = 0;
  u8 maxIdx = 0;
  for( i=0;i<MCT_MAX;i++ )
  {
    if( maxPoint < progWork->condition[i] )
    {
      maxPoint = progWork->condition[i];
      maxIdx = i;
    }
  }
  return maxIdx;
}

//--------------------------------------------------------------
//	�V���[�p�[�g�pNPC�쐬
//--------------------------------------------------------------
void MUSICAL_PROGRAM_SetData_NPC( MUSICAL_PROGRAM_WORK* progWork , STAGE_INIT_WORK *actInitWork , const u8 musicalIdx , const u8 NPCIdx , HEAPID heapId )
{
  u8 i;
  const MUSICAL_PROGRAM_POKE_DATA *pokeData = &progWork->progData->pokeData[NPCIdx];
  if( pokeData->monsno == MUS_PROG_DATA_POKE_RANDOM )
  {
    const u16 monsno = MUSICAL_SYSTEM_GetMusicalPokemonRandom();
    MUSICAL_STAGE_SetData_NPC( actInitWork , musicalIdx , monsno , heapId );
    ARI_TPrintf("monsno:[%d]\n",monsno);
  }
  else
  {
    MUSICAL_STAGE_SetData_NPC( actInitWork , musicalIdx , pokeData->monsno , heapId );
  }
  
  for( i=0;i<MUS_PROG_DATA_EQUIP_NUM;i++ )
  {
    if( pokeData->equipData[i].itemNo != MUS_PROG_DATA_EQUIP_RANDOM && 
        pokeData->equipData[i].itemNo != MUS_PROG_DATA_EQUIP_NONE )
    {
      MUSICAL_STAGE_SetEquip( actInitWork , musicalIdx , pokeData->equipData[i].itemPos , pokeData->equipData[i].itemNo , 0 , i );
    }
  }

}

//--------------------------------------------------------------
//	BG�ԍ��擾
//--------------------------------------------------------------
const u8 MUSICAL_PROGRAM_GetBgNo( MUSICAL_PROGRAM_WORK* progWork )
{
  return progWork->progData->bgNo;
}
