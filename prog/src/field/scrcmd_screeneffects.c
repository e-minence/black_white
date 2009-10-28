//======================================================================
/**
 * @file	scrcmd_screeneffects.c
 *
 * @brief	�X�N���v�g�R�}���h�F�t�B�[���h��ʉ��o�n
 * @date  2009.09.22
 * @author	tamada GAMEFREAK inc.
 *
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "script_local.h" //SCRIPT_GetEvent

#include "scrcmd_screeneffects.h"

#include "event_pc_recovery.h"  //EVENT_PcRecoveryAnime
#include "field_player.h"   //FIELD_PLAYER
#include "fieldmap.h"       //FIELDMAP_GetFieldPlayer

#include "field_g3d_mapper.h" //FLDMAPPPER
#include "field_buildmodel.h"
#include "field_bmanime_tool.h"

#include "field/field_const.h"
#include "sound/pm_sndsys.h"      //PMSND_PlaySE, PMSND_CheckPlaySE

static BOOL EvWaitDispFade( VMHANDLE *core, void *wk );

//======================================================================
//  ��ʃt�F�[�h
//======================================================================
//--------------------------------------------------------------
/**
 * ��ʃt�F�[�h
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDispFadeStart( VMHANDLE *core, void *wk )
{
  u16 mode = VMGetU16( core );
  u16 start_evy = VMGetU16( core );
  u16 end_evy = VMGetU16( core );
  u16 wait = VMGetU16( core );
  GFL_FADE_SetMasterBrightReq( mode, start_evy, end_evy, wait );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ��ʃt�F�[�h�I���`�F�b�N �E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitDispFade( VMHANDLE *core, void *wk )
{
  if( GFL_FADE_CheckFade() == TRUE ){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * ��ʃt�F�[�h�I���`�F�b�N
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDispFadeCheck( VMHANDLE *core, void *wk )
{
  VMCMD_SetWait( core, EvWaitDispFade );
  return VMCMD_RESULT_SUSPEND;
}


//======================================================================
//
//    �A�j���[�V�������o
//
//======================================================================
//--------------------------------------------------------------
/**
 * �|�P�Z���񕜃A�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo  �z�u���f�����������A���̈ʒu�ɃA�j����K�p�������B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPokecenRecoverAnime( VMHANDLE * core, void *wk )
{
  VecFx32 pos;
  GMEVENT * call_event;
  GMEVENT * parent;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 pokecount = SCRCMD_GetVMWorkValue( core, wk );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  FIELD_PLAYER_GetDirPos( fld_player, DIR_UP, &pos );
  pos.z -= 48*FX32_ONE;  // TEMP: C01�|�P�Z���Ńe�X�g
  //PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
  
  parent = SCRIPT_GetEvent( sc );
  call_event = EVENT_PcRecoveryAnime( gsys, parent, &pos, pokecount );
  SCRIPT_CallEvent( sc, call_event );
  return VMCMD_RESULT_SUSPEND;
}

//======================================================================
//
//
//    �h�A�A�j��
//
//
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef u16 MEMKEY;

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  MEMKEY key;
  void * memory;
}MEM_RES;

//--------------------------------------------------------------
/**
 * @brief �h�A�A�j�����m�ۂ��邽�߂̃L�[
 * @todo  �L�[�ŊǗ����郁�����@�\��p�ӂ��A������Ɉڍs����
 */
//--------------------------------------------------------------
static MEM_RES memRes = { 0, NULL };

//--------------------------------------------------------------
//--------------------------------------------------------------
static void reserveKey(void * mem, MEMKEY key)
{
  GF_ASSERT_MSG(memRes.key == 0, "�A�j���Ǘ��̈悪�m�ۂł��܂���!!\n");
  memRes.key = key;
  memRes.memory = mem;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void releaseKey(MEMKEY key)
{
  GF_ASSERT_MSG( memRes.key == key, "�s���ȊǗ��ԍ��ł�\n");
  memRes.key = 0;
  memRes.memory = NULL;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void * getMemory(MEMKEY key)
{
  GF_ASSERT_MSG( memRes.key == key, "�s���ȊǗ��ԍ��ł�\n");
  return memRes.memory;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL EvWaitDoorAnime( VMHANDLE *core, void *wk );
enum { DOOR_ANIME_KEY_01 = 0x03fa };

//--------------------------------------------------------------
/**
 * @brief �h�A�A�j������F�n���h����������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDoorAnimeCreate( VMHANDLE * core, void *wk )
{
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );
  MEMKEY key;
  VecFx32 pos;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMAPPER * mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
  FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( mapper );

  BMANIME_CONTROL_WORK * ctrl;

  pos.x = GRID_TO_FX32( gx );
  pos.y = 0;
  pos.z = GRID_TO_FX32( gz );

  ctrl = BMANIME_CTRL_Create( bmodel_man, &pos );

  reserveKey( ctrl, DOOR_ANIME_KEY_01 );

  *ret_wk = DOOR_ANIME_KEY_01;

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �h�A�A�j������F�n���h���폜����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDoorAnimeDelete( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );

  BMANIME_CONTROL_WORK * ctrl;

  ctrl = getMemory( anime_id );
  BMANIME_CTRL_Delete( ctrl );
  releaseKey( anime_id );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �h�A�A�j������F�A�j���Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDoorAnimeSet( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 anime_type = SCRCMD_GetVMWorkValue( core, wk );
  BMANIME_CONTROL_WORK * ctrl;
  u16 seNo;

  ctrl = getMemory( anime_id );
  BMANIME_CTRL_SetAnime( ctrl, anime_type );
  if( BMANIME_CTRL_GetSENo( ctrl, anime_type, &seNo) )
  {
    PMSND_PlaySE( seNo );
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �h�A�A�j������F�A�j���E�F�C�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo
 * �I�u�W�F�N�g�Ǘ��@�\���ǂ����邩�H�l����
 * �L�[�����������X�g�\���Ń��[�N�Ǘ�����H
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDoorAnimeWait( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );
  VMCMD_SetWait( core, EvWaitDoorAnime );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief   �h�A�A�j���I���҂��֐�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param   wk      SCRCMD_WORK�ւ̃|�C���^
 * @return  BOOL  TRUE�̂Ƃ��I��
 */
//--------------------------------------------------------------
static BOOL EvWaitDoorAnime( VMHANDLE *core, void *wk )
{
  BMANIME_CONTROL_WORK * ctrl;
  ctrl = getMemory( DOOR_ANIME_KEY_01 );  //�蔲��
  if ( BMANIME_CTRL_WaitAnime( ctrl ) == TRUE && PMSND_CheckPlaySE() == FALSE )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


