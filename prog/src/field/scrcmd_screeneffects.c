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

#include "field/field_const.h"
#include "sound/pm_sndsys.h"      //PMSND_PlaySE, PMSND_CheckPlaySE

#include "event_fieldmap_control.h"   //for EVENT_Field�`
#include "system/main.h"  //for HEAPID_PROC

#include "event_pokecen_pc.h"  // for EVENT_PokecenPcOn, EVENT_PokecenPcOff

#define BRIGHT_FADE_SPPED (1)

typedef enum {
  FADE_IN,
  FADE_OUT,
}FADE_IO;

typedef struct BRIGHT_CNT_WORK_tag
{
  GAMESYS_WORK* gsys;
  FIELDMAP_WORK* fieldmap;
	FIELD_FADE_TYPE fade_type;
  FIELD_FADE_WAIT_FLAG wait_flag;
  FADE_IO   fade_io;
  int Seq;
}BRIGHT_CNT_WORK;

static BOOL EvWaitDispFade( VMHANDLE *core, void *wk );
static BOOL EvWaitMapFade( VMHANDLE *core, void *wk );
static void CreateBrightFadeTask( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, FADE_IO fade_io,
                             FIELD_FADE_TYPE type, FIELD_FADE_WAIT_FLAG wait );
static void BrightCntTcb( GFL_TCB* tcb, void* work );

#ifdef PM_DEBUG
extern BOOL MapFadeReqFlg;  //�}�b�v�t�F�[�h���N�G�X�g�t���O  �錾���@script.c
#endif

extern void FIELDMAP_InitBG( FIELDMAP_WORK* fieldWork );
extern void FIELDMAP_InitBGMode( void );
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
 * �}�b�v�J�ڃt�F�[�h�I���`�F�b�N �E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitMapFade( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  if( FIELDMAP_CheckMapFadeReqFlg( fieldWork ) == TRUE ){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �}�b�v�`�F���W�p�@�G�߃C��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT 
 *
 * @note
 * �R���e�B�j���[���Ɏg�p����ƁA�t�F�[�h�A�E�g���Ă��Ȃ����߂�
 * Assert�ɂȂ��Ă��܂��B���̎g�������̂��߂̃R�}���h�B
 * ���݂̃Q�[�����G�߂�\�����Ă���t�F�[�h�C������B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_SeasonIn_Force( VMHANDLE *core, void *wk )
{
  GMEVENT* event;
  SCRCMD_WORK*   work       = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scriptWork = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*  gameSystem = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*      gameData   = GAMESYSTEM_GetGameData( gameSystem );
  FIELDMAP_WORK* fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );
  u16            season     = GAMEDATA_GetSeasonID( gameData );
  
  event = EVENT_FieldFadeIn_Season( gameSystem, fieldmap, season, season ); 
  SCRIPT_CallEvent( scriptWork, event );
#ifdef PM_DEBUG
  //GF_ASSERT_MSG(MapFadeReqFlg,"ERROR:NOT CALL MAP_FADE_OUT");
  MapFadeReqFlg = FALSE;
#endif
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �}�b�v�`�F���W�p�@�u���b�N�C��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT 
 *
 * @note
 * �R���e�B�j���[���Ɏg�p����ƁA�t�F�[�h�A�E�g���Ă��Ȃ����߂�
 * Assert�ɂȂ��Ă��܂��B���̎g�������̂��߂̃R�}���h
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_BlackIn_Force( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  CreateBrightFadeTask( gsys, fieldWork, FADE_IN, FIELD_FADE_BLACK, FIELD_FADE_WAIT );
#ifdef PM_DEBUG
  //GF_ASSERT_MSG(MapFadeReqFlg,"ERROR:NOT CALL MAP_FADE_OUT");
  MapFadeReqFlg = FALSE;
#endif
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * �}�b�v�`�F���W�p�@�u���b�N�C��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_BlackIn( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  CreateBrightFadeTask( gsys, fieldWork, FADE_IN, FIELD_FADE_BLACK, FIELD_FADE_WAIT );
#ifdef PM_DEBUG
  GF_ASSERT_MSG(MapFadeReqFlg,"ERROR:NOT CALL MAP_FADE_OUT");
  MapFadeReqFlg = FALSE;
#endif
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �}�b�v�`�F���W�p�@�u���b�N�A�E�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_BlackOut( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  CreateBrightFadeTask( gsys, fieldWork, FADE_OUT, FIELD_FADE_BLACK, FIELD_FADE_WAIT );
#ifdef PM_DEBUG
  GF_ASSERT_MSG(!MapFadeReqFlg,"ERROR:ALREADY CALLED MAP_FADE_OUT");
  MapFadeReqFlg = TRUE;
#endif  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �}�b�v�`�F���W�p�@�z���C�g�C��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_WhiteIn( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  CreateBrightFadeTask( gsys, fieldWork, FADE_IN, FIELD_FADE_WHITE, FIELD_FADE_WAIT );
#ifdef PM_DEBUG
  GF_ASSERT_MSG(MapFadeReqFlg,"ERROR:NOT CALL MAP_FADE_OUT");
  MapFadeReqFlg = FALSE;
#endif  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �}�b�v�`�F���W�p�@�z���C�g�A�E�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_WhiteOut( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  CreateBrightFadeTask( gsys, fieldWork, FADE_OUT, FIELD_FADE_WHITE, FIELD_FADE_WAIT );
#ifdef PM_DEBUG
  GF_ASSERT_MSG(!MapFadeReqFlg,"ERROR:ALREADY CALLED MAP_FADE_OUT");
  MapFadeReqFlg = TRUE;
#endif  
  return VMCMD_RESULT_CONTINUE;
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

//--------------------------------------------------------------
/**
 * �}�b�v�J�ڗp�t�F�[�h�I���`�F�b�N
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFadeCheck( VMHANDLE *core, void *wk )
{
  VMCMD_SetWait( core, EvWaitMapFade );
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
  GMEVENT * call_event;
  GMEVENT * parent;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 pokecount = SCRCMD_GetVMWorkValue( core, wk );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  
  parent = SCRIPT_GetEvent( sc );
  call_event = EVENT_PcRecoveryAnime( gsys, parent, pokecount );
  SCRIPT_CallEvent( sc, call_event );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * PC�N���A�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo  �z�u���f�����������A���̈ʒu�ɃA�j����K�p�������B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPokecenPcOn( VMHANDLE * core, void *wk )
{
  GMEVENT* event;
  GMEVENT* parent;
  SCRCMD_WORK*       work = wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*         sc = SCRCMD_WORK_GetScriptWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  
  parent = SCRIPT_GetEvent( sc );
  event = EVENT_PokecenPcOn( parent, gsys, fieldmap );
  SCRIPT_CallEvent( sc, event );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * PC�N�����A�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo  �z�u���f�����������A���̈ʒu�ɃA�j����K�p�������B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPokecenPcRun( VMHANDLE * core, void *wk )
{
  GMEVENT* event;
  GMEVENT* parent;
  SCRCMD_WORK*       work = wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*         sc = SCRCMD_WORK_GetScriptWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  
  parent = SCRIPT_GetEvent( sc );
  event = EVENT_PokecenPcRun( parent, gsys, fieldmap );
  SCRIPT_CallEvent( sc, event );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * PC��~�A�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo  �z�u���f�����������A���̈ʒu�ɃA�j����K�p�������B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPokecenPcOff( VMHANDLE * core, void *wk )
{
  GMEVENT* event;
  GMEVENT* parent;
  SCRCMD_WORK*       work = wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*         sc = SCRCMD_WORK_GetScriptWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  
  parent = SCRIPT_GetEvent( sc );
  event = EVENT_PokecenPcOff( parent, gsys, fieldmap );
  SCRIPT_CallEvent( sc, event );
  return VMCMD_RESULT_SUSPEND;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static FIELD_BMODEL_MAN * getBModelMan( SCRCMD_WORK * wk )
{
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( wk );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMAPPER * mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
  FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( mapper );
  return bmodel_man;
}

//--------------------------------------------------------------
/**
 * @brief �z�u���f�����f�[�^�̎擾
 */
//--------------------------------------------------------------
static G3DMAPOBJST * getBModel( FIELD_BMODEL_MAN * man, u16 bm_id, u16 gx, u16 gz )
{
  VecFx32 pos;

  pos.x = GRID_TO_FX32( gx );
  pos.y = 0;
  pos.z = GRID_TO_FX32( gz );

  return FIELD_BMODEL_MAN_SearchObjStatusPos( man, bm_id, &pos );
}

//--------------------------------------------------------------
/**
 * @brief �z�u���f�����f�[�^�̃A�j������F�Đ��I����Ԃɂ���
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBModelDirectAnimeSetFinished( VMHANDLE * core, void *wk )
{
  u16 bm_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );

  SCRCMD_WORK *work = wk;
  FIELD_BMODEL_MAN * bmodel_man = getBModelMan( work );
  G3DMAPOBJST * entry = getBModel( bmodel_man, bm_id, gx, gz );

  if ( entry )
  {
    G3DMAPOBJST_setAnime( bmodel_man, entry, BMANM_INDEX_DOOR_OPEN, BMANM_REQ_REVERSE_START );
    G3DMAPOBJST_setAnime( bmodel_man, entry, BMANM_INDEX_DOOR_OPEN, BMANM_REQ_STOP );
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �z�u���f�����f�[�^�̃A�j������F���[�v�Đ���Ԃɂ���
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBModelDirectAnimeSetLoop( VMHANDLE * core, void *wk )
{
  u16 bm_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );

  SCRCMD_WORK *work = wk;
  FIELD_BMODEL_MAN * bmodel_man = getBModelMan( work );
  G3DMAPOBJST * entry = getBModel( bmodel_man, bm_id, gx, gz );

  if ( entry )
  {
    G3DMAPOBJST_setAnime( bmodel_man, entry, BMANM_INDEX_DOOR_OPEN, BMANM_REQ_LOOP );
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �z�u���f�����f�[�^�̕\������ON/OFF
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBModelDirectChangeViewFlag( VMHANDLE * core, void *wk )
{
  u16 bm_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );
  u16 flag = SCRCMD_GetVMWorkValue( core, wk );

  SCRCMD_WORK *work = wk;
  FIELD_BMODEL_MAN * bmodel_man = getBModelMan( work );
  G3DMAPOBJST * entry = getBModel( bmodel_man, bm_id, gx, gz );

  if (entry)
  {
    G3DMAPOBJST_changeViewFlag( entry, flag );
  }

  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//
//
//    �z�u���f���A�j��
//
//
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16 bmodel_unique_key;

//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL evWaitBModelAnime( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief �z�u���f���A�j������F�n���h����������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBModelAnimeCreate( VMHANDLE * core, void *wk )
{
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 bm_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );
  VecFx32 pos;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMAPPER * mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
  FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( mapper );

  FIELD_BMODEL * bmodel;

  pos.x = GRID_TO_FX32( gx );
  pos.y = 0;
  pos.z = GRID_TO_FX32( gz );

  bmodel = FIELD_BMODEL_Create_Search( bmodel_man, bm_id, &pos );
  *ret_wk = FIELD_BMODEL_MAN_GetUniqKey( bmodel_man, bmodel );


#ifdef PM_DEBUG
  
  OS_TPrintf( "======= EV_SEQ_BMODEL_ANIME_CREATE INFO ======\n" );
  FIELD_BMODEL_DEBUG_ScriptPrintInfo( bmodel ); 
  OS_TPrintf( "======= EV_SEQ_BMODEL_ANIME_CREATE INFO END ======\n" );
  
#endif

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �z�u���f���A�j������F�n���h���폜����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBModelAnimeDelete( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );

  FIELD_BMODEL_MAN * bmodel_man = getBModelMan( wk );
  FIELD_BMODEL * bmodel;

  bmodel = FIELD_BMODEL_MAN_GetBModelByUniqKey( bmodel_man, anime_id );
  if ( bmodel )
  {
    FIELD_BMODEL_Delete( bmodel );
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �z�u���f���A�j������F�A�j���Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBModelAnimeSet( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 anime_type = SCRCMD_GetVMWorkValue( core, wk );
  FIELD_BMODEL_MAN * bmodel_man = getBModelMan( wk );
  FIELD_BMODEL * bmodel;
  u16 seNo;

  bmodel = FIELD_BMODEL_MAN_GetBModelByUniqKey( bmodel_man, anime_id );
  if (bmodel != NULL)
  {
    FIELD_BMODEL_SetAnime( bmodel, anime_type, BMANM_REQ_START );
    if( FIELD_BMODEL_GetCurrentSENo( bmodel, &seNo ) )
    {
      PMSND_PlaySE( seNo );
    }
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �z�u���f���A�j������F�A�j���Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBModelAnimeStop( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );
  FIELD_BMODEL_MAN * bmodel_man = getBModelMan( wk );
  FIELD_BMODEL * bmodel;
  u16 seNo;

  bmodel = FIELD_BMODEL_MAN_GetBModelByUniqKey( bmodel_man, anime_id );
  if (bmodel != NULL)
  {
    FIELD_BMODEL_StopCurrentAnime( bmodel );
  }

  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief �z�u���f���A�j������F�A�j���E�F�C�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo
 * �I�u�W�F�N�g�Ǘ��@�\���ǂ����邩�H�l����
 * �L�[�����������X�g�\���Ń��[�N�Ǘ�����H
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBModelAnimeWait( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );
  bmodel_unique_key = anime_id;
  VMCMD_SetWait( core, evWaitBModelAnime );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief   �z�u���f���A�j���I���҂��֐�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param   wk      SCRCMD_WORK�ւ̃|�C���^
 * @return  BOOL  TRUE�̂Ƃ��I��
 */
//--------------------------------------------------------------
static BOOL evWaitBModelAnime( VMHANDLE *core, void *wk )
{
  FIELD_BMODEL_MAN * bmodel_man = getBModelMan( wk );
  FIELD_BMODEL * bmodel;
  bmodel = FIELD_BMODEL_MAN_GetBModelByUniqKey( bmodel_man, bmodel_unique_key );  //�蔲��
  if ( bmodel == NULL ) return TRUE;
  if ( FIELD_BMODEL_WaitCurrentAnime( bmodel ) == FALSE)
  {
    return FALSE;
  }

  if ( FIELD_BMODEL_CheckCurrentSE( bmodel ) == FALSE )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//======================================================================
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�u���C�g�l�X�t�F�[�h�^�X�N����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param fade_io   �C���@or �A�E�g
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���
 * @return	none
 */
//------------------------------------------------------------------
static void CreateBrightFadeTask( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, FADE_IO fade_io,
                             FIELD_FADE_TYPE type, FIELD_FADE_WAIT_FLAG wait )
{
  BRIGHT_CNT_WORK *task_wk;
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  //���[�N�쐬
  task_wk = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(BRIGHT_CNT_WORK) );
  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, BrightCntTcb, task_wk, 0 );
  
  //���N�G�X�g�t���O���Ă�
  FIELDMAP_SetMapFadeReqFlg( fieldmap, TRUE );

  task_wk->Seq  = 0;
  task_wk->gsys      = gsys;
  task_wk->fieldmap  = fieldmap;
	task_wk->fade_type = type;
  task_wk->wait_flag = wait;
  task_wk->fade_io = fade_io;
}

//--------------------------------------------------------------
/**
 * �u���C�g�l�X�t�F�[�h�^�X�N
 * @param   tcb        �^�X�N�|�C���^
 * @param   work      ���[�N 
 * @retval  none
 */
//--------------------------------------------------------------
static void BrightCntTcb( GFL_TCB* tcb, void* work )
{
  BRIGHT_CNT_WORK *wk = work;
  switch(wk->Seq){
  case 0:
    {
      int mode;
      int start, end;
      int fade_speed;

      fade_speed = BRIGHT_FADE_SPPED;
      if ( wk->fade_io == FADE_IN )
      {
        //�t�F�[�h�C���̂Ƃ��͂a�f����������
        
        start = 16;
        end = 0;
        
        // BG���[�h�ݒ�ƕ\���ݒ�̕��A
        {
          int mv = GFL_DISP_GetMainVisible();
          FIELDMAP_InitBGMode();
          GFL_DISP_GX_SetVisibleControlDirect( mv );
        }
        FIELDMAP_InitBG(wk->fieldmap);
      }
      else
      {
        start = 0;
        end = 16;
      }

      if ( wk->fade_type ==  FIELD_FADE_BLACK)
      {
        mode = GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB;
      }
      else
      {
        mode = GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN | GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB;
      }
      GFL_FADE_SetMasterBrightReq(mode, start, end, fade_speed);
    }
    wk->Seq++;
    break;
  case 1:
    {
      BOOL rc = FALSE;
      if( wk->wait_flag == FIELD_FADE_NO_WAIT ) rc = TRUE;
		  if( GFL_FADE_CheckFade() == FALSE ) rc = TRUE;

      if (rc) { //�t�F�[�h�I��
        FIELDMAP_SetMapFadeReqFlg( wk->fieldmap, FALSE ); //���N�G�X�g����
        GFL_TCB_DeleteTask( tcb );
        GFL_HEAP_FreeMemory( work );
      }
    }
		break;
  }
}

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�vBG������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_InitBG( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  // BG���[�h�ݒ�ƕ\���ݒ�̕��A
  {
    int mv = GFL_DISP_GetMainVisible();
    FIELDMAP_InitBGMode();
    GFL_DISP_GX_SetVisibleControlDirect( mv );
  }
  FIELDMAP_InitBG(fieldmap);

  return VMCMD_RESULT_SUSPEND;
}



