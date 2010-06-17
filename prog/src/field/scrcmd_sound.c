//======================================================================
/**
 * @file  scr_sound.c
 * @brief  �X�N���v�g�R�}���h�F�T�E���h�֘A
 * @author  Satoshi Nohara
 * @date  06.06.26
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "fieldmap.h"
#include "field/zonedata.h"
#include "field_sound.h"

#include "sound/pm_voice.h"
#include "sound/pm_wb_voice.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_sound.h"

#include "gamesystem/iss_sys.h"
#include "gamesystem/iss_switch_sys.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
typedef struct VOICE_PLAY_WORK_tag
{
  u16 MonsNo;
  u16 FormNo;
  BOOL Mine;
  SCRCMD_WORK* scr_work;
}VOICE_PLAY_WORK;

//======================================================================
//  proto
//======================================================================
// SE�̒�~���Ď�
static void SoundSeFlag_CheckSeStop( SCRIPT_WORK * sc );
static void SoundSeFlag_SetPlay( SCRIPT_WORK * sc, u32 se_no );
static BOOL SoundSeFlag_IsSePlay( SCRIPT_WORK * sc );

static GMEVENT_RESULT PlayVoiceEvent( GMEVENT* event, int* seq, void* wk );

//======================================================================
//  �R�}���h BGM
//======================================================================

//--------------------------------------------------------------
/**
 * BGM �ύX
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 * @note ���ݍĐ�����BGM��j�����A�w��BGM���Đ����Ă���B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmPlay( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*    sc = SCRCMD_WORK_GetScriptWork( work );
  u16       soundIdx = VMGetU16( core );

  {
    GMEVENT* event;
    event = EVENT_FSND_PlayEventBGM( gsys, soundIdx );
    SCRIPT_CallEvent( sc, event );
  }

  // �t�B�[���h�T�E���h�B
  // ��SE��~�B
  {
    GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
    FIELD_SOUND* fsound = GAMEDATA_GetFieldSound( gdata );
    FSND_PauseEnvSE( fsound, FSND_ENVSE_PAUSE_BGM_CHANGE );
  }
  
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * BGM�ύX ( FO�t���[�����w��ver. )
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 * @note ���ݍĐ�����BGM��j�����A�w��BGM���Đ����Ă���B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmPlayEx( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work      = wk;
  GAMESYS_WORK* gsys      = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*  script    = SCRCMD_WORK_GetScriptWork( work );
  u16           sound_idx = VMGetU16( core ); // �R�}���h������: BGM
  u16           frame     = VMGetU16( core ); // �R�}���h������: FO�t���[����

  {
    GMEVENT* event;
    event = EVENT_FSND_PlayEventBGMEx( gsys, sound_idx, frame );
    SCRIPT_CallEvent( script, event );
  }

  // �t�B�[���h�T�E���h�B
  // ��SE��~�B
  {
    GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
    FIELD_SOUND* fsound = GAMEDATA_GetFieldSound( gdata );
    FSND_PauseEnvSE( fsound, FSND_ENVSE_PAUSE_BGM_CHANGE );
  }
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * BGM �ύX ( �����ȍĐ� )
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 * @note ���ݍĐ�����BGM��j�����A����BGM���Đ����Ă���B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmPlaySilent( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work   = wk;
  GAMESYS_WORK* gsys   = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*  script = SCRCMD_WORK_GetScriptWork( work );
  u16           frame  = VMGetU16( core ); // �R�}���h������: FO�t���[����

  {
    GMEVENT* event;
    event = EVENT_FSND_PlayEventBGMSilent( gsys, frame );
    SCRIPT_CallEvent( script, event );
  }

  // �t�B�[���h�T�E���h�B
  // ��SE��~�B
  {
    GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
    FIELD_SOUND* fsound = GAMEDATA_GetFieldSound( gdata );
    FSND_PauseEnvSE( fsound, FSND_ENVSE_PAUSE_BGM_CHANGE );
  }
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * BGM�I���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmPlayCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 music  = VMGetU16( core );
  u16 *ret_wk  = SCRCMD_GetVMWork( core, work );
  *ret_wk = PMSND_CheckPlayBGM();
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * BGM�I���҂� �E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitBgm( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*    sc = SCRCMD_WORK_GetScriptWork( work );

  if( PMSND_CheckPlayBGM() == FALSE )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * BGM�I���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmWait( VMHANDLE *core, void *wk )
{
  VMCMD_SetWait( core, EvWaitBgm );
  return VMCMD_RESULT_SUSPEND; 
}

//--------------------------------------------------------------
/**
 * BGM �̃{�����[����������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmVolumeDown( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*   work = wk;
  GAMESYS_WORK*  gameSystem = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*      gameData   = GAMESYSTEM_GetGameData( gameSystem );
  FIELDMAP_WORK* fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );
  FIELD_SOUND*   fieldSound = GAMEDATA_GetFieldSound( gameData );
  u16 volume, frame;

  volume = SCRCMD_GetVMWorkValue( core, work ); // ������: �{�����[��
  frame  = SCRCMD_GetVMWorkValue( core, work ); // ������: �t���[����

  FIELD_SOUND_ChangePlayerVolume( fieldSound, volume, frame );  
  SCREND_CHK_SetBitOn( SCREND_CHK_BGM_VOLUME_DOWN );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * BGM �̃{�����[���𕜋A������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmVolumeRecover( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*   work = wk;
  GAMESYS_WORK*  gameSystem = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*      gameData   = GAMESYSTEM_GetGameData( gameSystem );
  FIELDMAP_WORK* fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );
  FIELD_SOUND*   fieldSound = GAMEDATA_GetFieldSound( gameData );
  u16 frame;

  frame  = SCRCMD_GetVMWorkValue( core, work ); // ������: �t���[����

  FIELD_SOUND_ChangePlayerVolume( fieldSound, 127, frame );  
  SCREND_CHK_SetBitOff( SCREND_CHK_BGM_VOLUME_DOWN );

  return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BGM �{�����[�����쏈���̑΃`�F�b�N
 *
 *	@param	end_check   ���[�N
 *	@param	seq         �V�[�P���X
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
BOOL SCREND_CheckEndBGMVolumeDown( SCREND_CHECK *end_check, int *seq )
{
  GAMEDATA*    gameData   = GAMESYSTEM_GetGameData( end_check->gsys );
  FIELD_SOUND* fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch( *seq ){
  case 0:
    FIELD_SOUND_ChangePlayerVolume( fieldSound, 127, 0 );  
    (*seq) ++;
    break;
  case 1:
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���݂̃}�b�v��BGM���Đ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 * @note �Đ�����BGM��j����, �t�B�[���h��BGM���Đ�����
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmNowMapPlay( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*   work = wk;
  GAMESYS_WORK*  gsys     = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*      gdata    = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_SOUND*   fsound   = GAMEDATA_GetFieldSound( gdata );
  SCRIPT_WORK*   sc       = SCRCMD_WORK_GetScriptWork( work );

  {
    GMEVENT* event;
    u8 seasonID;
    u16 zoneID;
    u32 soundIdx;

    seasonID = GAMEDATA_GetSeasonID( gdata );
    zoneID   = FIELDMAP_GetZoneID( fieldmap );
    soundIdx = FSND_GetFieldBGM( gdata, zoneID, seasonID );

    event = EVENT_FSND_ChangeBGM( gsys, soundIdx, FSND_FADE_LONG, FSND_FADE_NORMAL );

    SCRIPT_CallEvent( sc, event );
    FSND_RePlayEnvSE( fsound, FSND_ENVSE_PAUSE_BGM_CHANGE ); // �������A
  }
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ���݂̃}�b�v��BGM���Đ� ( FO�t���[�����w��ver. )
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 * @note �Đ�����BGM��j����, �t�B�[���h��BGM���Đ�����
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmNowMapPlayEx( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*   work     = wk;
  GAMESYS_WORK*  gsys     = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*      gdata    = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_SOUND*   fsnd     = GAMEDATA_GetFieldSound( gdata );
  SCRIPT_WORK*   script   = SCRCMD_WORK_GetScriptWork( work );
  u16            frame    = VMGetU16( core ); // �R�}���h������: FO�t���[����

  {
    GMEVENT* event;
    u8 seasonID;
    u16 zoneID;
    u32 soundIdx;

    seasonID = GAMEDATA_GetSeasonID( gdata );
    zoneID   = FIELDMAP_GetZoneID( fieldmap );
    soundIdx = FSND_GetFieldBGM( gdata, zoneID, seasonID );
    event    = EVENT_FSND_ChangeBGM( gsys, soundIdx, frame, FSND_FADE_NORMAL );
    SCRIPT_CallEvent( script, event );
    FSND_RePlayEnvSE( fsnd, FSND_ENVSE_PAUSE_BGM_CHANGE ); // �������A
  }
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ���݂�BGM��ޔ���, �C�x���gBGM���Đ�����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 * @note �Đ�����BGM��j����, �t�B�[���h��BGM���Đ�����
 *
 * ���ʏ�̃C�x���gBGM�̓t�B�[���hBGM�̑ޔ����s��Ȃ��ōĐ�����B
 * �@�C�x���gBGM �� �o�g�� �� �t�B�[���hBGM �̗��ꂪ�K�v�ȏ�ʂŎg�p����B
 */
//-------------------------------------------------------------- 
VMCMD_RESULT EvCmdPlayTempEventBGM( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SCRIPT_WORK*         sc = SCRCMD_WORK_GetScriptWork( work );
  u16            soundIdx = SCRCMD_GetVMWorkValue( core, work );
  GMEVENT* event;

  //event = EVENT_FSND_PushPlayNextBGM( gsys, soundIdx, FSND_FADE_FAST, FSND_FADE_NONE );
  event = EVENT_FSND_PushPlayEventBGM( gsys, soundIdx ); // 100413 �����Đ��ɕύX
  SCRIPT_CallEvent( sc, event );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ���݂�BGM��Push����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * �g�p�ӏ��͂ł��邾�����肵�����̂ŁA�R�}���h�̗��p�ӏ��͋���
 * (10.04.7���� 3D�f���Ăяo���̂�)
 */
//-------------------------------------------------------------- 
VMCMD_RESULT EvCmdBgmPush( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*       sc   = SCRCMD_WORK_GetScriptWork( work );

  u16 fade_frame = SCRCMD_GetVMWorkValue( core, work );
  
  SCRIPT_CallEvent( sc, EVENT_FSND_PushBGM( gsys, fade_frame ));

  SCREND_CHK_SetBitOn( SCREND_CHK_BGM_PUSH_POP );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ���݂�BGM��Pop����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * �g�p�ӏ��͂ł��邾�����肵�����̂ŁA�R�}���h�̗��p�ӏ��͋���
 * (10.04.7���� 3D�f���Ăяo���̂�)
 */
//-------------------------------------------------------------- 
VMCMD_RESULT EvCmdBgmPop( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*       sc   = SCRCMD_WORK_GetScriptWork( work );

  u16 fadeout_frame = SCRCMD_GetVMWorkValue( core, work );
  u16 fadein_frame = SCRCMD_GetVMWorkValue( core, work );
  
  SCRIPT_CallEvent( sc, EVENT_FSND_PopBGM( gsys, fadeout_frame, fadein_frame ));
  
  SCREND_CHK_SetBitOff( SCREND_CHK_BGM_PUSH_POP );
  return VMCMD_RESULT_SUSPEND;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BGM Push/Pop�����̑΃`�F�b�N
 *
 *	@param	end_check   ���[�N
 *	@param	seq         �V�[�P���X
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
BOOL SCREND_CheckEndBGMPushPop( SCREND_CHECK *end_check, int *seq )
{
  SCRIPT_WORK *sc = end_check->ScrWk;
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );

  switch( *seq ){
  case 0:
    SCRIPT_CallEvent( sc, EVENT_FSND_PopBGM( end_check->gsys, 0, 0 ));
    (*seq) ++;
    break;
  case 1:
    return TRUE;
  }
  return FALSE;
}




//======================================================================
//  �R�}���h�@EnvSE
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ��SE�̒�~�󋵃N���A
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdEnvSeBGMPlayClear( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*   work     = wk;
  GAMESYS_WORK*  gsys     = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*      gdata    = GAMESYSTEM_GetGameData( gsys );
  FIELD_SOUND*   fsnd     = GAMEDATA_GetFieldSound( gdata );

  FSND_RePlayEnvSE( fsnd, FSND_ENVSE_PAUSE_BGM_CHANGE ); // �������A

  return VMCMD_RESULT_CONTINUE;
}




//======================================================================
//  �R�}���h�@SE
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �Ď����Ă���SE���Ƃ܂��Ă��Ȃ����`�F�b�N�@�Ƃ܂��Ă�����A�t���O�𗎂Ƃ�
 */
//-----------------------------------------------------------------------------
static void SoundSeFlag_CheckSeStop( SCRIPT_WORK * sc )
{
  int i;
  u8* p_sound_se_flag = SCRIPT_GetSoundSeFlag( sc );

  // SE��~�`�F�b�N
  for( i=0; i<SEPLAYER_MAX; i++ )
  {
    if( (*p_sound_se_flag) & (1<<i) )
    {
      // ��~�`�F�b�N
      if( PMSND_CheckPlaySE_byPlayerID(i) == FALSE )
      {
        // ��~���Ă���̂Ńt���O�𗎂Ƃ�
        (*p_sound_se_flag) &= ~(1<<i);
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  SE�̍Đ��ݒ�
 *	
 *	@param	sc
 *	@param	se_no 
 */
//-----------------------------------------------------------------------------
static void SoundSeFlag_SetPlay( SCRIPT_WORK * sc, u32 se_no )
{
  u8* p_sound_se_flag = SCRIPT_GetSoundSeFlag( sc );
  SEPLAYER_ID player_id = PMSND_GetSE_DefaultPlayerID( se_no );

  (*p_sound_se_flag) |= (1<<player_id);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Ď�SE�̍Đ��`�F�b�N
 *
 *	@param	sc    ���[�N
 *
 *	@retval TRUE  �Đ���
 *	@retval FLASE �Đ��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL SoundSeFlag_IsSePlay( SCRIPT_WORK * sc )
{
  int i;
  u8* p_sound_se_flag = SCRIPT_GetSoundSeFlag( sc );

  // SE��~�`�F�b�N
  for( i=0; i<SEPLAYER_MAX; i++ )
  {
    if( (*p_sound_se_flag) & (1<<i) )
    {
      // ��~�`�F�b�N
      if( PMSND_CheckPlaySE_byPlayerID(i) == TRUE )
      {
        return TRUE;
      }
    }
  }

  // �t���O�̃N���A
  (*p_sound_se_flag) = 0;

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  SE�̒�~
 *	
 *	@param	sc
 *	@param	se_no 
 */
//-----------------------------------------------------------------------------
static void SoundSeFlag_StopSe( SCRIPT_WORK * sc )
{
  int i;
  u8* p_sound_se_flag = SCRIPT_GetSoundSeFlag( sc );

  // SE��~�`�F�b�N
  for( i=0; i<SEPLAYER_MAX; i++ )
  {
    if( (*p_sound_se_flag) & (1<<i) )
    {
      // ��~�`�F�b�N
      PMSND_StopSE_byPlayerID( i );
    }
  }

  // �t���O�̃N���A
  (*p_sound_se_flag) = 0;
}

//--------------------------------------------------------------
/**
 * �r�d��炷
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSePlay( VMHANDLE *core, void *wk )
{
  u16 se_no = SCRCMD_GetVMWorkValue(core,wk);
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *scriptwk = SCRCMD_WORK_GetScriptWork(work);

#if 0
  Snd_SePlay( SCRCMD_VMGetWorkValue(core,wk) );
#else //wb

  // �Đ�����SE���Ƃ܂��Ă�����A�Ď��t���O�𗎂Ƃ�
  SoundSeFlag_CheckSeStop( scriptwk );
  
  // �Đ�
  PMSND_PlaySE( se_no );

  // �Ď��J�n
  SoundSeFlag_SetPlay( scriptwk, se_no );  
#endif
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �r�d���~�߂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSeStop( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *scriptwk = SCRCMD_WORK_GetScriptWork(work);
#if 0
  Snd_SeStopBySeqNo( VMGetWorkValue(core), 0 );
#else //wb
  SoundSeFlag_StopSe( scriptwk );
#endif
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * SE�I���҂� �E�F�C�g����
 * @param 
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitSe(VMHANDLE *core, void *wk)
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *scriptwk = SCRCMD_WORK_GetScriptWork(work);
#if 0
  //if( Snd_SePlayCheckAll() == 0 ){
  if( Snd_SePlayCheck(core->reg[0]) == 0 ){
    return TRUE;
  }
#else //wb
  if( SoundSeFlag_IsSePlay(scriptwk) == FALSE ){
    return TRUE;
  }
#endif
  return FALSE;
}

//--------------------------------------------------------------
/**
 * SE�I���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 * @li    �J�Ȃǂ̃��[�v���ŁA�������[�v�ɂȂ��Ă��܂��̂ŁASE�i���o�[�w�肵�āA������`�F�b�N�I
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSeWait(VMHANDLE *core, void *wk)
{
#if 0
  //���z�}�V���̔ėp���W�X�^��BGM�i���o�[���i�[
  core->reg[0] = VMGetWorkValue(core);
  VMCMD_SetWait( core, EvWaitSe );
#else //wb
  VMCMD_SetWait( core, EvWaitSe );
#endif
  return VMCMD_RESULT_SUSPEND;
}

//======================================================================
//  ME
//======================================================================
//--------------------------------------------------------------
/**
 * ME�Đ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMePlay(VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*    sc = SCRCMD_WORK_GetScriptWork( work );
  u16      sound_idx = VMGetU16( core );

  {
    GMEVENT* event;
    event = EVENT_FSND_PushPlayJingleBGM( gsys, sound_idx );
    SCRIPT_CallEvent( sc, event );
  }

  /* BTS:5951 �Ώ����ɁABGMPush���Ă���̂ŁA�����͂���Ȃ����Ƃ����o
     * tomoya
  // �t�B�[���h�T�E���h�B
  // ��SE��~�B
  {
    GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
    FIELD_SOUND* fsound = GAMEDATA_GetFieldSound( gdata );
    FSND_PauseEnvSE( fsound );
  }
  */
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ME�I���҂� �E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitMe( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work  = wk;
  GAMESYS_WORK* gsys  = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK*  sc    = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*     gdata = GAMESYSTEM_GetGameData( gsys );
  FIELD_SOUND*  fsnd  = GAMEDATA_GetFieldSound( gdata );

  if( (PMSND_CheckPlayBGM() == FALSE) && 
      (FIELD_SOUND_HaveRequest(fsnd) == FALSE) )
  {
    GMEVENT* event;
    event = EVENT_FSND_PopBGM( gsys, FSND_FADE_NONE, FSND_FADE_FAST );
    SCRIPT_CallEvent( sc, event );

    /* BTS:5951 �Ώ����ɁABGMPush���Ă���̂ŁA�����͂���Ȃ����Ƃ����o
     * tomoya
    // �t�B�[���h�T�E���h�B
    // ��SE�Ďn���B
    {
      GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND* fsound = GAMEDATA_GetFieldSound( gdata );
      FSND_RePlayEnvSE( fsound );
    }
    */

    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ME�I���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMeWait(VMHANDLE *core, void *wk )
{
  VMCMD_SetWait( core, EvWaitMe );
  return VMCMD_RESULT_SUSPEND;
}

//======================================================================
//  ����
//======================================================================
//--------------------------------------------------------------
/**
 * ������炷
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdVoicePlay( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK* work = wk;
  SCRIPT_WORK*    sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys   = SCRCMD_WORK_GetGameSysWork( work );
  u16 monsno = SCRCMD_GetVMWorkValue(core, wk);
  u16 formno = SCRCMD_GetVMWorkValue(core, wk);
  {
    VOICE_PLAY_WORK* vpw;
    GMEVENT* event;
    event = GMEVENT_Create( gsys, NULL, PlayVoiceEvent, sizeof(VOICE_PLAY_WORK) );
    vpw = GMEVENT_GetEventWork( event );
    vpw->MonsNo = monsno;
    vpw->FormNo = formno;
    vpw->Mine = FALSE;
    vpw->scr_work = work;
    SCRIPT_CallEvent( sc, event );
  }

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ������炷 ( �y���b�v�^���f�[�^�Đ�ver. )
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdVoicePlay_forMine( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK* work = wk;
  SCRIPT_WORK*    sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys   = SCRCMD_WORK_GetGameSysWork( work );
  u16 monsno = SCRCMD_GetVMWorkValue(core, wk);
  u16 formno = SCRCMD_GetVMWorkValue(core, wk);
  {
    VOICE_PLAY_WORK* vpw;
    GMEVENT* event;
    event = GMEVENT_Create( gsys, NULL, PlayVoiceEvent, sizeof(VOICE_PLAY_WORK) );
    vpw = GMEVENT_GetEventWork( event );
    vpw->MonsNo = monsno;
    vpw->FormNo = formno;
    vpw->Mine = TRUE;
    vpw->scr_work = work;
    SCRIPT_CallEvent( sc, event );
  }

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
///@return TRUE = �I��
//--------------------------------------------------------------
static BOOL EvWaitVoicePlay(VMHANDLE *core, void *wk)
{
  SCRCMD_WORK*  work = wk;
  u32 player = SCRCMD_WORK_GetPMVoiceIndex( work );
  if (PMVOICE_CheckPlay( player ) == FALSE)
  {
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------
/**
 * �����I���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdVoiceWait( VMHANDLE *core, void *wk )
{
  VMCMD_SetWait( core, EvWaitVoicePlay );
  return VMCMD_RESULT_SUSPEND;
}


//======================================================================
// ISS
//======================================================================

//--------------------------------------------------------------
/**
 * @brief ISS-S �̎w��X�C�b�`�� ON �ɂ���
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @return VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdIssSwitchOn( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*    work  = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*   gsys  = SCRCMD_WORK_GetGameSysWork( work );
  ISS_SYS*        iss   = GAMESYSTEM_GetIssSystem( gsys );
  ISS_SWITCH_SYS* iss_s = ISS_SYS_GetIssSwitchSystem( iss );
  u16             idx   = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h������

  ISS_SWITCH_SYS_SwitchOn( iss_s, idx );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ISS-S �̎w��X�C�b�`�� OFF �ɂ���
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @return VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdIssSwitchOff( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*    work  = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*   gsys  = SCRCMD_WORK_GetGameSysWork( work );
  ISS_SYS*        iss   = GAMESYSTEM_GetIssSystem( gsys );
  ISS_SWITCH_SYS* iss_s = ISS_SYS_GetIssSwitchSystem( iss );
  u16             idx   = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h������

  ISS_SWITCH_SYS_SwitchOff( iss_s, idx );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ISS-S �̎w��X�C�b�`�̏�Ԃ��`�F�b�N����
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @return VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdIssSwitchCheck( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*     work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*    gsys = SCRCMD_WORK_GetGameSysWork( work );
  ISS_SYS*          iss = GAMESYSTEM_GetIssSystem( gsys );
  ISS_SWITCH_SYS* iss_s = ISS_SYS_GetIssSwitchSystem( iss );
  u16*           ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h������
  u16               idx = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h������

  *ret_wk = ISS_SWITCH_SYS_IsSwitchOn( iss_s, idx );
  return VMCMD_RESULT_CONTINUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief �����Đ��C�x���g
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PlayVoiceEvent( GMEVENT* event, int* seq, void* wk )
{
  u32 player;
  VOICE_PLAY_WORK* vpw = wk;
  //�a�f�l���[�h���Ȃ烍�[�h���I���̂�҂�
  if ( PMSND_IsLoading() ) return GMEVENT_RES_CONTINUE;

  if (vpw->Mine) player = PMV_PlayVoice_forMine( vpw->MonsNo, vpw->FormNo );
  else player = PMV_PlayVoice( vpw->MonsNo, vpw->FormNo );
  
  SCRCMD_WORK_SetPMVoiceIndex( vpw->scr_work, player );

  return GMEVENT_RES_FINISH;
}

//======================================================================
//  �؃��b�v
//======================================================================
#if 0 //wb
//--------------------------------------------------------------
/**
 * �y���b�v�f�[�^�����邩�`�F�b�N
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPerapDataCheck( VMHANDLE *core, void *wk )
{
  u16* ret_wk  = SCRCMD_GetVMWork( core, work );

  if( Snd_PerapVoiceCheck(SaveData_GetPerapVoice(core->fsys->savedata)) == TRUE ){
    *ret_wk = TRUE;
    return VMCMD_RESULT_CONTINUE;
  }

  *ret_wk = FALSE;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �y���b�v�^���J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPerapRecStart( VMHANDLE *core, void *wk )
{
  u16* ret_wk      = SCRCMD_GetVMWork( core, work );

  if( Snd_PerapVoiceRecStart() == MIC_RESULT_SUCCESS ){
    *ret_wk = TRUE;  //����
    return VMCMD_RESULT_CONTINUE;
  }

  *ret_wk = FALSE;  //���s
  return VMCMD_RESULT_CONTINUE;

}

//--------------------------------------------------------------
/**
 * �y���b�v�^����~
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPerapRecStop( VMHANDLE *core, void *wk )
{
  Snd_PerapVoiceRecStop();
  return VMCMD_RESULT_SUSPEND;

}

//--------------------------------------------------------------
/**
 * �y���b�v�^�������f�[�^���Z�[�u
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPerapSave( VMHANDLE *core, void *wk )
{
  Snd_PerapVoiceDataSave( SaveData_GetPerapVoice(core->fsys->savedata) );
  return VMCMD_RESULT_SUSPEND;

}
#endif

//======================================================================
//  ���̑�
//======================================================================
#if 0 //wb
//--------------------------------------------------------------
/**
 * �N���C�}�b�N�X���o�T�E���h�f�[�^�ǉ����[�h
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSndClimaxDataLoad( VMHANDLE *core, void *wk )
{
  Snd_DataSetByScene( SND_SCENE_SUB_CLIMAX, 0, 0 );  //�T�E���h�f�[�^���[�h(BGM���p��)
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �����{�����[���Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSndInitialVolSet( VMHANDLE *core, void *wk )
{
  u16 no  = VMGetWorkValue(core);
  u16 vol = VMGetWorkValue(core);

  Snd_PlayerSetInitialVolumeBySeqNo( no, vol );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �f�����o�T�E���h�f�[�^���[�h
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSndDemo01DataLoad( VMHANDLE *core, void *wk )
{
  if( sys.cont & PAD_KEY_UP ){
    //Snd_DataSetByScene( SND_SCENE_DEMO01, SEQ_PL_CHO01_2_SIM, 1 );
    //Snd_DataSetByScene( SND_SCENE_DEMO01, SEQ_PL_TOWN01, 1 );
    //Snd_DataSetByScene( SND_SCENE_DEMO01, SEQ_PL_TOWN01, 1 );
    Snd_DataSetByScene( SND_SCENE_DEMO01, SEQ_PL_TOWN02, 1 );    //�f�B���CTr����
  }else{
    //Snd_DataSetByScene( SND_SCENE_DEMO01, SEQ_PL_TOWN01, 1 );
    Snd_DataSetByScene( SND_SCENE_DEMO01, SEQ_PL_TOWN02, 1 );    //�f�B���CTr����
  }
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�T�E���h�f�[�^���[�h
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSndFieldDataLoad( VMHANDLE *core, void *wk )
{
  u16 bgm = SCRCMD_GetVMWorkValue( core, work );
  Snd_DataSetByScene( SND_SCENE_FIELD, bgm, 1 );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief  ����BGM�i���o�[�擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSndNowBgmNoGet( VMHANDLE *core, void *wk )
{
  u16* ret_wk  = SCRCMD_GetVMWork( core, work );
  *ret_wk    = Snd_NowBgmNoGet();
  return VMCMD_RESULT_CONTINUE;
}
#endif
