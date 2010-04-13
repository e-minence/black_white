//======================================================================
/**
 * @file  dressup_system.h
 * @brief �~���[�W�J���̃X�e�[�WProc
 * @author  ariizumi
 * @data  09/03/02
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "musical/musical_system.h"
#include "musical/musical_stage_sys.h"
#include "musical/musical_shot_sys.h"

#include "musical/stage/sta_local_def.h"
#include "musical/stage/sta_acting.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  enum
//======================================================================
enum  //���C���V�[�P���X
{
  STA_SEQ_INIT_ACTING,  //�X�e�[�W���Z����
  STA_SEQ_LOOP_ACTING,
  STA_SEQ_TERM_ACTING,
};

//======================================================================
//  typedef struct
//======================================================================

typedef struct
{
  STAGE_INIT_WORK *actInitWork;
  ACTING_WORK     *actWork;
  MUS_COMM_WORK   *commWork;
}STAGE_LOCAL_WORK;

//======================================================================
//  proto
//======================================================================

static GFL_PROC_RESULT MusicalStageProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MusicalStageProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MusicalStageProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA MusicalStage_ProcData =
{
  MusicalStageProc_Init,
  MusicalStageProc_Main,
  MusicalStageProc_Term
};

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static GFL_PROC_RESULT MusicalStageProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  int ePos;
  STAGE_LOCAL_WORK *work;
/*
  ARI_TPrintf("FreeHeap:[%x][%x]\n", 
      GFL_HEAP_GetHeapFreeSize( GFL_HEAPID_APP ) ,
      GFI_HEAP_GetHeapAllocatableSize( GFL_HEAPID_APP ) );
*/
  //�X�g���[�~���O�z�M������ăe�X�g
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MUSICAL_STAGE, 0x90000 );

  //TODO �f�[�^�̓n���������I
  work = GFL_PROC_AllocWork( proc, sizeof(STAGE_LOCAL_WORK), HEAPID_MUSICAL_STAGE );
  if( pwk == NULL )
  {
    GFL_OVERLAY_Load(FS_OVERLAY_ID(musical_shot));
    work->actInitWork = MUSICAL_STAGE_CreateStageWork( HEAPID_MUSICAL_STAGE , NULL );
    MUSICAL_STAGE_SetData_NPC( work->actInitWork , 0 , MONSNO_PIKATYUU , 0 , 60 , HEAPID_MUSICAL_STAGE );
    MUSICAL_STAGE_SetData_NPC( work->actInitWork , 1 , MONSNO_PURUNSU , 0 , 120 , HEAPID_MUSICAL_STAGE );
    MUSICAL_STAGE_SetData_NPC( work->actInitWork , 2 , MONSNO_509  , 0 , NPC_APPEAL_TIME_ACTION , HEAPID_MUSICAL_STAGE );
    MUSICAL_STAGE_SetData_NPC( work->actInitWork , 3 , MONSNO_WARUBIARU , 0 , NPC_APPEAL_TIME_DISTURB_H , HEAPID_MUSICAL_STAGE );
    
    MUSICAL_STAGE_SetEquip( work->actInitWork , 0 , MUS_POKE_EQU_HAND_R , 19 , 0 , 0 );
    MUSICAL_STAGE_SetEquip( work->actInitWork , 0 , MUS_POKE_EQU_HAND_L  , 19 , 0 , 1 );
    MUSICAL_STAGE_SetEquip( work->actInitWork , 0 , MUS_POKE_EQU_HEAD  , 16 , 0 , 2 );

    MUSICAL_STAGE_SetEquip( work->actInitWork , 1 , MUS_POKE_EQU_EAR_L , 7 , 0 , 0 );
    MUSICAL_STAGE_SetEquip( work->actInitWork , 1 , MUS_POKE_EQU_BODY  , 9 , 0 , 1 );
    MUSICAL_STAGE_SetEquip( work->actInitWork , 1 , MUS_POKE_EQU_HAND_R , 24 , 0 , 2 );
    MUSICAL_STAGE_SetEquip( work->actInitWork , 1 , MUS_POKE_EQU_HAND_L  , 43 , 0 , 3 );

    MUSICAL_STAGE_SetEquip( work->actInitWork , 2 , MUS_POKE_EQU_HAND_R , 31 , 0 , 0 );
    MUSICAL_STAGE_SetEquip( work->actInitWork , 2 , MUS_POKE_EQU_HAND_L  , 30 , 0 , 1 );
    MUSICAL_STAGE_SetEquip( work->actInitWork , 2 , MUS_POKE_EQU_HEAD   , 15 , 0 , 2 );

    MUSICAL_STAGE_SetEquip( work->actInitWork , 3 , MUS_POKE_EQU_HAND_R , 30 , 0 , 0 );
    MUSICAL_STAGE_SetEquip( work->actInitWork , 3 , MUS_POKE_EQU_HAND_L  , 30 , 0 , 1 );
    MUSICAL_STAGE_SetEquip( work->actInitWork , 3 , MUS_POKE_EQU_HEAD   , 21 , 0 , 2 );
    //�������v���[���[��
    work->actInitWork->musPoke[1]->charaType = MUS_CHARA_PLAYER;

    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MUSICAL_STRM|HEAPDIR_MASK, 0x80000 );
    work->actInitWork->distData = MUSICAL_SYSTEM_InitDistributeData( GFL_HEAPID_APP );
    MUSICAL_SYSTEM_LoadDistributeData( work->actInitWork->distData , NULL , NULL, 0 , HEAPID_MUSICAL_STRM );

    work->actInitWork->progWork = MUSICAL_PROGRAM_InitProgramData( HEAPID_MUSICAL_STAGE , work->actInitWork->distData , 0 );
    MUSICAL_PROGRAM_CalcPokemonPoint( HEAPID_MUSICAL_STAGE , work->actInitWork->progWork , work->actInitWork );

    work->actInitWork->musPoke[0]->point = 75;
    work->actInitWork->musPoke[1]->point = 45;
    work->actInitWork->musPoke[2]->point = 30;
    work->actInitWork->musPoke[3]->point = 0;
  }
  else
  {
    work->actInitWork = pwk;
  }
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static GFL_PROC_RESULT MusicalStageProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  STAGE_LOCAL_WORK *work = mywk;
  
  if( pwk == NULL )
  {
    MUSICAL_PROGRAM_TermProgramData( work->actInitWork->progWork );
    MUSICAL_SYSTEM_TermDistributeData( work->actInitWork->distData );
    GFL_HEAP_DeleteHeap( HEAPID_MUSICAL_STRM );
    //�������v���[���[�ɉ������̂Ŗ߂�
    work->actInitWork->musPoke[1]->charaType = MUS_CHARA_NPC;
    MUSICAL_STAGE_DeleteStageWork( work->actInitWork );
    GFL_OVERLAY_Unload(FS_OVERLAY_ID(musical_shot));
  }
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MUSICAL_STAGE );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  ���[�v
//--------------------------------------------------------------
static GFL_PROC_RESULT MusicalStageProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  STAGE_LOCAL_WORK *work = mywk;
  switch( *seq )
  {
  case STA_SEQ_INIT_ACTING:
    work->actWork = STA_ACT_InitActing( work->actInitWork , HEAPID_MUSICAL_STAGE );
    *seq = STA_SEQ_LOOP_ACTING;
	ARI_TPrintf("HEAP[%x]\n",GFL_HEAP_GetHeapFreeSize( GFL_HEAPID_APP ));
    break;
    
  case STA_SEQ_LOOP_ACTING:
    {
      ACTING_RETURN ret;
      ret = STA_ACT_LoopActing( work->actWork );
      if( ret == ACT_RET_GO_END )
      {
        *seq = STA_SEQ_TERM_ACTING;
      }
    }
    break;

  case STA_SEQ_TERM_ACTING:
    STA_ACT_TermActing( work->actWork );
    return GFL_PROC_RES_FINISH;
    break;
  }
  
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
//  InitWork�쐬
//--------------------------------------------------------------
STAGE_INIT_WORK* MUSICAL_STAGE_CreateStageWork( HEAPID heapId , MUS_COMM_WORK *commWork )
{
  u8 i;
  STAGE_INIT_WORK *initWork;
  initWork = GFL_HEAP_AllocMemory( heapId , sizeof(STAGE_INIT_WORK) );
  initWork->commWork = commWork;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    initWork->musPoke[i] = NULL;
  }
  return initWork;
}

//--------------------------------------------------------------
//  InitWork�폜
//--------------------------------------------------------------
void MUSICAL_STAGE_DeleteStageWork( STAGE_INIT_WORK *initWork )
{
  u8 i;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    if( initWork->musPoke[i] != NULL )
    {
      if( initWork->musPoke[i]->charaType == MUS_CHARA_NPC )
      {
        GFL_HEAP_FreeMemory( initWork->musPoke[i] );
      }
    }
  }
  GFL_HEAP_FreeMemory( initWork );
}

//--------------------------------------------------------------
//  �L�����Z�b�g(�v���C���[
//--------------------------------------------------------------
void MUSICAL_STAGE_SetData_Player( STAGE_INIT_WORK *initWork , const u8 idx , MUSICAL_POKE_PARAM *musPara )
{
  GF_ASSERT_MSG( idx < MUSICAL_POKE_MAX , "Musical invalid index [%d]\n",idx);
  GF_ASSERT_MSG( initWork->musPoke[idx] == NULL , "Musical index[%d] is not NULL\n",idx);

  initWork->musPoke[idx] = musPara;
  initWork->musPoke[idx]->charaType = MUS_CHARA_PLAYER;
}

//--------------------------------------------------------------
//  �L�����Z�b�g(NPC
//--------------------------------------------------------------
void MUSICAL_STAGE_SetData_NPC( STAGE_INIT_WORK *initWork , const u8 idx , u16 pokeID , const u8 sex , u16 npcAppealTime , HEAPID heapId )
{
  u8 ePos;
  GF_ASSERT_MSG( idx < MUSICAL_POKE_MAX , "Musical invalid index [%d]\n",idx);
  GF_ASSERT_MSG( initWork->musPoke[idx] == NULL , "Musical index[%d] is not NULL\n",idx);

  initWork->musPoke[idx] = MUSICAL_SYSTEM_InitMusPokeParam( pokeID , 0 , 0 , 0 , heapId );
  initWork->musPoke[idx]->charaType = MUS_CHARA_NPC;
  initWork->musPoke[idx]->npcAppealTime = npcAppealTime;
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
  {
    const u8 tempType = GFL_STD_MtRand(3);
    switch( tempType )
    {
      case 0:
        initWork->musPoke[idx]->npcAppealTime = NPC_APPEAL_TIME_ACTION;
        break;
      case 1:
        initWork->musPoke[idx]->npcAppealTime = NPC_APPEAL_TIME_DISTURB_H;
        break;
      case 2:
        initWork->musPoke[idx]->npcAppealTime = GFL_STD_MtRand(60*5)+60;
        break;
    }
    ARI_TPrintf("Set NPC[%d] appeal type[%d]\n",idx,initWork->musPoke[idx]->npcAppealTime);
  }
#endif
}

//--------------------------------------------------------------
//  �L�����Z�b�g(�ʐM
//--------------------------------------------------------------
void MUSICAL_STAGE_SetData_Comm( STAGE_INIT_WORK *initWork , const u8 idx , MUSICAL_POKE_PARAM *musPara )
{
  GF_ASSERT_MSG( idx < MUSICAL_POKE_MAX , "Musical invalid index [%d]\n",idx);
  GF_ASSERT_MSG( initWork->musPoke[idx] == NULL , "Musical index[%d] is not NULL\n",idx);

  initWork->musPoke[idx] = musPara;
  initWork->musPoke[idx]->charaType = MUS_CHARA_COMM;
}

//--------------------------------------------------------------
//  �����ʐݒ�(�f�o�b�O�p�H
//--------------------------------------------------------------
void MUSICAL_STAGE_SetEquip( STAGE_INIT_WORK *initWork , const u8 idx , MUS_POKE_EQUIP_POS ePos , u16 itemId , s16 angle , u8 pri )
{
  MUSICAL_POKE_PARAM *initPara = initWork->musPoke[idx];
  GF_ASSERT_MSG( idx < MUSICAL_POKE_MAX , "Musical invalid index [%d]\n",idx);

  initPara->equip[ePos].itemNo = itemId;
  initPara->equip[ePos].angle  = angle;
  initPara->equip[ePos].priority  = pri;
}

