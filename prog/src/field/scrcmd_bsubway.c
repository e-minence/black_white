//======================================================================
/**
 * @file  scrcmd_bsybway.c
 * @brief  �o�g���T�u�E�F�C�@�X�N���v�g�R�}���h��p�\�[�X
 * @author  Miyuki Iwasawa
 * @date  2006.05.23
 *
 * 2007.05.24  Satoshi Nohara*
 *
 * @note pl����ڐA kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "bsubway_scr.h"
#include "bsubway_scr_common.h"
#include "scrcmd_bsubway.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
static BOOL bsway_CheckEntryPokeNum(
    u16 num, GAMESYS_WORK *gsys, BOOL item_flag );

//======================================================================
//  proto
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�p���[�N�쐬�Ə�����
 *  @param  init  ���������[�h�w��
 *          BSWAY_PLAY_NEW:new game
 *          BSWAY_PLAY_CONTINUE:new game
 *  @param  mode  �v���C���[�h�w��:BSWAY_MODE_�`
 *  @return 0
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayWorkCreate( VMHANDLE* core, void *wk )
{
  u16 init = VMGetU16(core);
  u16 mode = VMGetU16(core);
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  BSUBWAY_SCRWORK **bsw_scr;
  
  bsw_scr = FIELDMAP_GetBSubwayScriptWork( fparam->fieldMap );
  *bsw_scr = BSUBWAY_SCRWORK_CreateWork( gsys, init, mode );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C����p���[�N�|�C���^������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayWorkClear(VMHANDLE* core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  BSUBWAY_SCRWORK **bsw_scr;
  
  bsw_scr = FIELDMAP_GetBSubwayScriptWork( fparam->fieldMap );
  BSUBWAY_SCRWORK_ClearWork( bsw_scr );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�p���[�N���
 *  @return 0
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayWorkRelease(VMHANDLE* core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  BSUBWAY_SCRWORK **bsw_scr;
  
  bsw_scr = FIELDMAP_GetBSubwayScriptWork( fparam->fieldMap );
  BSUBWAY_SCRWORK_ReleaseWork( *bsw_scr );
  *bsw_scr = NULL;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�p�R�}���h�Q�Ăяo���C���^�[�t�F�[�X
 *  @param  com_id    u16:�R�}���hID
 *  @param  retwk_id  u16:�Ԃ�l���i�[���郏�[�N��ID
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayTool( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  u16 com_id = VMGetU16( core );
  u16 param = SCRCMD_GetVMWorkValue( core, work );
  u16 retwk_id = VMGetU16( core );
  u16 *ret_wk = SCRIPT_GetEventWork( sc, gdata, retwk_id );
  BSUBWAY_SCRWORK *bsw_scr,**bsw_scr_base;
  void **buf;
  
  bsw_scr_base = FIELDMAP_GetBSubwayScriptWork( fparam->fieldMap );
  bsw_scr = *bsw_scr_base;
   
  switch( com_id ){
  //���[�N��ˑ�
  //�Q���\�ȃ|�P�������̃`�F�b�N
  case BSWAY_TOOL_CHK_ENTRY_POKE_NUM:
    if( param == 0 ){
      *ret_wk = bsway_CheckEntryPokeNum( bsw_scr->member_num, gsys, 1 );
    }else{
      *ret_wk = bsway_CheckEntryPokeNum( param, gsys, 1 );
    }
    break;
  //���Z�b�g
  case BSWAY_TOOL_SYSTEM_RESET:
    #if 0 //wb null
    TowerScrTools_SystemReset();
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  //�v���C�f�[�^�N���A
  case BSWAY_TOOL_CLEAR_PLAY_DATA:
    BSUBWAY_PLAYDATA_Init( bsw_scr->playData );
    break;
  //�Z�[�u���Ă��邩�H
  case BSWAY_TOOL_IS_SAVE_DATA_ENABLE:
    *ret_wk = BSUBWAY_PLAYDATA_GetSaveFlag( bsw_scr->playData );
    break;
  //�E�o�p�Ɍ��݂̃��P�[�V�������L������
  case BSWAY_TOOL_PUSH_NOW_LOCATION:
    #if 0 //wb null
    TowerScrTools_PushNowLocation(core->fsys);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  //�L�^�������P�[�V�����ɖ߂��B
  case BSWAY_TOOL_POP_NOW_LOCATION:
    #if 0 // wb null
    TowerScrTools_PopNowLocation(core->fsys);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  //�A�����R�[�h���擾
  case BSWAY_TOOL_GET_RENSHOU_RECORD:
    #if 0 //wb null
    *ret_wk = TowerScrTools_GetRenshouRecord(core->fsys->savedata,param);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_TOOL_WIFI_RANK_DOWN:
    #if 0 // wb null 
    *ret_wk = TowerScr_SetWifiRank(NULL,core->fsys->savedata,2);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_TOOL_GET_WIFI_RANK:
    #if 0
    *ret_wk = TowerScr_SetWifiRank(NULL,core->fsys->savedata,0);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_TOOL_SET_WIFI_UPLOAD_FLAG:
    #if 0
    TowerScrTools_SetWifiUploadFlag(core->fsys->savedata,param);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_TOOL_GET_WIFI_UPLOAD_FLAG:
    #if 0
    *ret_wk = TowerScrTools_GetWifiUploadFlag(core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_TOOL_SET_NG_SCORE:
    #if 0
    *ret_wk = TowerScrTools_SetNGScore(core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_TOOL_IS_PLAYER_DATA_ENABLE:
    #if 0
    *ret_wk = TowerScrTools_IsPlayerDataEnable(core->fsys->savedata);
    break;
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
  case BSWAY_TOOL_WIFI_CONNECT:
    #if 0
    EventCmd_BTowerWifiCall(core->event_work,param,retwk_id,*ret_wk);
    return 1;
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
  //���[�N�ˑ�
  //�|�P�������X�g��ʌĂяo��
  case BSWAY_SUB_SELECT_POKE:
    #if 0
    buf = GetEvScriptWorkMemberAdrs(core->fsys, ID_EVSCR_SUBPROC_WORK);
    TowerScr_SelectPoke(wk,core->event_work,buf);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    return 1;
  //�I�����ꂽ�|�P�����擾
  case BSWAY_SUB_GET_ENTRY_POKE:
    #if 0
    buf = GetEvScriptWorkMemberAdrs(core->fsys, ID_EVSCR_SUBPROC_WORK);
    *ret_wk = TowerScr_GetEntryPoke(wk,buf,core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  //�Q���w�肵���莝���|�P�����̏����`�F�b�N
  case BSWAY_SUB_CHK_ENTRY_POKE:
    #if 0
    *ret_wk = TowerScr_CheckEntryPoke(wk,core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_IS_CLEAR:
    #if 0
    *ret_wk = TowerScr_IsClear(wk);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_SET_LOSE_SCORE:
    #if 0
    TowerScr_SetLoseScore(wk,core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_SET_CLEAR_SCORE:
    #if 0
    TowerScr_SetClearScore(wk,core->fsys->savedata,core->fsys->fnote);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_SAVE_REST_PLAY_DATA:
    #if 0
    TowerScr_SaveRestPlayData(wk);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_CHOICE_BTL_SEVEN:
    #if 0
    TowerScr_ChoiceBtlSeven(wk);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_GET_ENEMY_OBJ:
    #if 0
    *ret_wk = TowerScr_GetEnemyObj(wk,param);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  //���݂̃v���C���[�h���擾
  case BSWAY_SUB_GET_PLAY_MODE:
    #if 0
    *ret_wk = (u16)TowerScr_GetPlayMode(wk);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_GET_LEADER_CLEAR_FLAG:
    #if 0
    *ret_wk = TowerScr_GetLeaderClearFlag(wk);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_GOODS_FLAG_SET:
    #if 0
    TowerScr_GoodsFlagSet(wk,core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_LEADER_RIBBON_SET:
    #if 0
    *ret_wk = TowerScr_LeaderRibbonSet(wk,core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_RENSHOU_RIBBON_SET:
    #if 0
    *ret_wk = TowerScr_RenshouRibbonSet(wk,core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_SET_PARTNER_NO:
    #if 0
    wk->partner = param;
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_GET_PARTNER_NO:
    #if 0
    *ret_wk = wk->partner;
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_BTL_TRAINER_SET:
    #if 0
    TowerScr_BtlTrainerNoSet(wk,core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_GET_SELPOKE_IDX:
    #if 0
    *ret_wk = wk->member[param];
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  //(BTS�ʐM142)�ύX�̑Ώ�
  case BSWAY_SUB_WIFI_RANK_UP:
    #if 0
    *ret_wk = TowerScr_SetWifiRank(wk,core->fsys->savedata,1);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_SUB_GET_MINE_OBJ:
    #if 0
    *ret_wk = btltower_GetMineObj(wk,param);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  //�v���C�����_���V�[�h���X�V����
  case BSWAY_SUB_UPDATE_RANDOM:
    #if 0
    *ret_wk = TowerScr_PlayRandUpdate(wk,core->fsys->savedata);
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  case BSWAY_DEB_IS_WORK_NULL:
    #if 0
    if(wk == NULL){
      *ret_wk = 1;
    }else{
      *ret_wk = 0;
    }
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
    break;
  //58:��M�o�b�t�@�N���A
  case BSWAY_SUB_RECV_BUF_CLEAR:
    #if 0
    MI_CpuClear8(wk->recv_buf,BSWAY_SIO_BUF_LEN);
    break;
    #else
    GF_ASSERT( 0 && "WB���쐬�R�}���h�ł�" );
    #endif
  default:
    OS_Printf( "�n���ꂽcom_id = %d\n", com_id );
    GF_ASSERT( 0 && "com_id�����Ή��ł��I" );
    *ret_wk = 0;
    break;
  }
  
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * �Q���\�ȃ|�P�������̃`�F�b�N
 * @param  num      �Q���ɕK�v�ȃ|�P������
 * @param  item_flag  �A�C�e���`�F�b�N���邩�t���O
 * @retval BOOL TRUE=�Q���\
 */
//--------------------------------------------------------------
static BOOL bsway_CheckEntryPokeNum(
    u16 num, GAMESYS_WORK *gsys, BOOL item_flag )
{
  return( TRUE );
}
