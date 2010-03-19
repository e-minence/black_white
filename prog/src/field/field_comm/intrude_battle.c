//==============================================================================
/**
 * @file    intrude_battle.c
 * @brief   �N���F�ʐM�ΐ�PROC
 * @author  matsuda
 * @date    2009.11.16(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "battle/battle.h"
#include "gamesystem/gamesystem.h"
#include "intrude_battle.h"
#include "gamesystem/btl_setup.h"
#include "intrude_types.h"
#include "intrude_main.h"
#include "intrude_comm_command.h"
#include "intrude_work.h"
#include "sound/pm_sndsys.h"
#include "field/intrude_snd_def.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�N���ʐM�ΐ퐧��\����
typedef struct{
  BATTLE_SETUP_PARAM para;
  u32 work;
  GFL_PROCSYS *procsys;
}INTRUDE_BATTLE_SYS;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT IntrudeBattleProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT IntrudeBattleProc_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT IntrudeBattleProc_End(GFL_PROC * proc, int * seq, void * pwk, void * mywk);


//==============================================================================
//  �f�[�^
//==============================================================================
const GFL_PROC_DATA IntrudeBattleProcData = {
  IntrudeBattleProc_Init,
  IntrudeBattleProc_Main,
  IntrudeBattleProc_End,
};


//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT IntrudeBattleProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  INTRUDE_BATTLE_SYS *ibs;
  
  ibs = GFL_PROC_AllocWork(proc, sizeof(INTRUDE_BATTLE_SYS), HEAPID_PROC);
  GFL_STD_MemClear(ibs, sizeof(INTRUDE_BATTLE_SYS));

  ibs->procsys = GFL_PROC_LOCAL_boot( HEAPID_PROC );
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT IntrudeBattleProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  INTRUDE_BATTLE_SYS *ibs = mywk;
  INTRUDE_BATTLE_PARENT *ibp = pwk;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(ibp->gsys);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(ibp->gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  GFL_PROC_MAIN_STATUS proc_status;

  intcomm = Intrude_Check_CommConnect(game_comm);

  proc_status = GFL_PROC_LOCAL_Main( ibs->procsys );
 
  switch(*seq){
  case 0:
    BTL_SETUP_Single_Comm(&ibs->para, gamedata, GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, HEAPID_PROC );
    ibs->para.musicDefault = SND_INTRUDE_BATTLE_BGM;
    ibs->para.commNetIDBit = (1 << GFL_NET_SystemGetCurrentID()) | (1 << ibp->target_netid);
    (*seq)++;
    break;
  case 1:
    if(IntrudeSend_TargetTiming(intcomm, ibp->target_netid, INTRUDE_TIMING_BATTLE_COMMAND_ADD_BEFORE) == TRUE){
      OS_TPrintf("�퓬�p�ʐM�R�}���hAdd�O�̓������\n");
      (*seq)++;
    }
    break;
  case 2:
    if(Intrude_GetTargetTimingNo(intcomm, INTRUDE_TIMING_BATTLE_COMMAND_ADD_BEFORE, ibp->target_netid) == TRUE){
      OS_TPrintf("�퓬�p�ʐM�R�}���hAdd�O�̓�����芮��\n");
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);
      (*seq)++;
    }
    break;
  case 3:
    if(IntrudeSend_TargetTiming(intcomm, ibp->target_netid, INTRUDE_TIMING_BATTLE_COMMAND_ADD_AFTER) == TRUE){
      OS_TPrintf("�퓬�p�ʐM�R�}���h�e�[�u����Add�����̂œ������\n");
      (*seq) ++;
    }
    break;
  case 4:
    if(Intrude_GetTargetTimingNo(intcomm, INTRUDE_TIMING_BATTLE_COMMAND_ADD_AFTER, ibp->target_netid) == TRUE){
      OS_TPrintf("�퓬�p�ʐM�R�}���h�e�[�u����Add��̓�����芮��\n");
      (*seq) ++;
    }
    break;
  case 5:
    OS_TPrintf("�o�g��PROC�Ăяo��\n");
    GFL_PROC_LOCAL_CallProc( ibs->procsys, NO_OVERLAY_ID, &BtlProcData, &ibs->para );
    (*seq)++;
    break;
  case 6:
    if(proc_status != GFL_PROC_MAIN_NULL){
      break;
    }
    OS_TPrintf("�o�g������\n");
    GFL_NET_DelCommandTable(GFL_NET_CMD_BATTLE);
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
    
    BATTLE_PARAM_Release( &ibs->para );
    return GFL_PROC_RES_FINISH;
  }
  
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT IntrudeBattleProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  INTRUDE_BATTLE_SYS *ibs = mywk;

  GFL_PROC_LOCAL_Exit( ibs->procsys );

  GFL_PROC_FreeWork(proc);
  return GFL_PROC_RES_FINISH;
}

