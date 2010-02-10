//======================================================================
/**
 * @file	scrcmd_battle.c
 * @brief	�X�N���v�g�R�}���h�F���s�����Ȃ�
 * @author  tamada GAMEFREAK inc.
 * @date	09.10.23
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

//#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "event_gameover.h" //EVENT_NormalLose
#include "event_battle.h"   //FIELD_BATTLE_IsLoseResult

#include "scrcmd_battle.h"

//======================================================================
// �g���[�i�[�t���O�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �g���[�i�[�t���O�̃Z�b�g(�g���[�i�[ID��n��)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerFlagSet( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  u16 flag = SCRCMD_GetVMWorkValue(core,work);  //�g���[�i�[ID��n���I�@���[�N�i���o�[��n���̂̓_���I
  SCRIPT_SetEventFlagTrainer( ev, flag );
  KAGAYA_Printf( "�g���[�i�[�t���O�Z�b�g ID=%d", flag );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �g���[�i�[�t���O�̃��Z�b�g(�g���[�i�[ID��n��)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerFlagReset( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  u16 flag = SCRCMD_GetVMWorkValue(core,work);  //�g���[�i�[ID��n���I�@���[�N�i���o�[��n���̂̓_���I
  SCRIPT_ResetEventFlagTrainer( ev, flag );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �g���[�i�[�t���O�̃`�F�b�N(�g���[�i�[ID��n��)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerFlagCheck( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  u16 flag = SCRCMD_GetVMWorkValue(core,work);  //�g���[�i�[ID��n���I ���[�N�i���o�[��n���̂̓_���I
  u16 * ret_wk = SCRCMD_GetVMWork(core,work);
  *ret_wk = SCRIPT_CheckEventFlagTrainer( ev, flag );
  //core->cmp_flag = SCRIPT_CheckEventFlagTrainer( ev, flag );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//
//  ���s�`�F�b�N�E�s�k�����֘A
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �쐶��s�k����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	VMCMD_RESULT_SUSPEND
 *
 * @todo  �����I�������X�N���v�g�ɉ���Y�ꂪ�Ȃ����A����
 *
 * @note
 * �X�N���v�g�������I�����A�s�k�����C�x���g�ւƑJ�ڂ���B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWildLose( VMHANDLE *core, void *wk )
{
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  call_event = EVENT_NormalLose( gsys );
  SCRIPT_EntryNextEvent( sc, call_event );

  VM_End( core );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief �쐶��s�k�`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWildLoseCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16 *ret_wk		= SCRCMD_GetVMWork( core, work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  BtlResult res = GAMEDATA_GetLastBattleResult(gdata);

  if (FIELD_BATTLE_IsLoseResult(res, BTL_COMPETITOR_WILD) == TRUE)
  {
    *ret_wk = SCR_BATTLE_RESULT_LOSE;
  } else {
    *ret_wk = SCR_BATTLE_RESULT_WIN;
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �g���[�i�[�s�k����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	VMCMD_RESULT_SUSPEND
 *
 * @todo  �����I�������X�N���v�g�ɉ���Y�ꂪ�Ȃ����A����
 *
 * @note
 * �X�N���v�g�������I�����A�s�k�����C�x���g�ւƑJ�ڂ���B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerLose( VMHANDLE *core, void *wk )
{
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  call_event = EVENT_NormalLose( gsys );
  SCRIPT_EntryNextEvent( sc, call_event );

  VM_End( core );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief �g���[�i�[�s�k�`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerLoseCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16 *ret_wk		= SCRCMD_GetVMWork( core, work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  BtlResult res = GAMEDATA_GetLastBattleResult(gdata);

  if (FIELD_BATTLE_IsLoseResult(res, BTL_COMPETITOR_TRAINER) == TRUE)
  {
    *ret_wk = SCR_BATTLE_RESULT_LOSE;
  } else {
    *ret_wk = SCR_BATTLE_RESULT_WIN;
  }

  return VMCMD_RESULT_CONTINUE;
}

#if 0
/*
 * �ꉞ�v��Ȃ��Ǝv��
 * scrcmd_encount.c �� EvCmdWildBattleRetryCheck()���`
 */
//--------------------------------------------------------------
/**
 * @brief �B���|�P�����퓬�@�Đ�s�`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 *
 * @todo  �{���ɂ���̂���������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSeacretPokeRetryCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	VMCMD_RESULT* win_flag	= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );
	u16* ret_wk		= SCRCMD_GetVMWork( core, work );
#if 0
	*ret_wk = BattleParam_IsSeacretPokeRetry(*win_flag);
	return 1;
#else //wb kari
  *ret_wk = 0;
  return VMCMD_RESULT_CONTINUE;
#endif
}

//--------------------------------------------------------------
/**
 * @brief �z�z�|�P�����퓬�@�Đ�s�`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 *
 * @todo  �{���ɂ���̂���������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdHaifuPokeRetryCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	VMCMD_RESULT* win_flag	= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );
	u16* ret_wk		= SCRCMD_GetVMWork( core, work );
#if 0
	*ret_wk = BattleParam_IsHaifuPokeRetry(*win_flag);
	return 1;
#else //wb kari
  *ret_wk = 0;
  return VMCMD_RESULT_CONTINUE;
#endif
}

#endif
