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

//======================================================================
//  proto
//======================================================================
// SE�̒�~���Ď�
static void SoundSeFlag_CheckSeStop( SCRIPT_WORK * sc );
static void SoundSeFlag_SetPlay( SCRIPT_WORK * sc, u32 se_no );
static BOOL SoundSeFlag_IsSePlay( SCRIPT_WORK * sc );

//======================================================================
//  �R�}���h BGM
//======================================================================
//--------------------------------------------------------------
/**
 * BGM�ύX
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 * @note ���ݍĐ�����BGM��ޔ����A�w��BGM���Đ����Ă���B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmPlay( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 music = VMGetU16( core );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
  FIELD_SOUND_PushPlayEventBGM( fsnd, music );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * BGM��~
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmStop( VMHANDLE *core, void *wk )
{
  PMSND_StopBGM();
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �V�[�P���X���ꎞ��~�܂��͍ĊJ
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmPlayerPause( VMHANDLE *core, void *wk )
{
  BOOL flag = VMGetU8(core);
  PMSND_PauseBGM( flag );
  return VMCMD_RESULT_CONTINUE;
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

#if 0 //wb
//--------------------------------------------------------------
/**
 * �}�b�v�������BGM�w�肪�Z�b�g�����
 * ���]��BGM�̐���ȂǂɎg�p
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmSpecialSet( VMHANDLE *core, void *wk )
{
  Snd_FieldBgmSetSpecial( core->fsys, VMGetU16(core) );
  return VMCMD_RESULT_CONTINUE;
}
#endif

//--------------------------------------------------------------
/**
 * BGM�t�F�[�h�A�E�g�҂��@�E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitBgmFade( VMHANDLE *core, void *wk )
{
  if( PMSND_CheckFadeOnBGM() ){
    return( TRUE );
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * BGM�t�F�[�h�A�E�g�҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmFadeOut( VMHANDLE *core, void *wk )
{
  u16 vol    = VMGetU16(core);
  u16 frame  = VMGetU16(core);
  PMSND_FadeOutBGM( frame );
  VMCMD_SetWait( core, EvWaitBgmFade );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * BGM�t�F�[�h�C���҂�(�t�F�[�h�A�E�g�������̂��ĊJ����)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmFadeIn( VMHANDLE *core, void *wk )
{
  u16 frame = VMGetU16(core);
  PMSND_FadeInBGM( frame );
  VMCMD_SetWait( core, EvWaitBgmFade );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ���݂̃}�b�v��BGM���Đ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 * @note �Đ��Ƃ��邪�A���ۂɂ͑ޔ�����BGM�̕��A���s���Ă���B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmNowMapPlay( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
  FIELD_SOUND_PopBGM( fsnd );
  return VMCMD_RESULT_CONTINUE;
}

#if 0
//--------------------------------------------------------------
/**
 * BGM�̕��A�Y�ꂪ����΋������A�B�G���[���p�B
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBgmForcePop( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
  FIELD_SOUND_ForcePopBGM( fsnd );
  return VMCMD_RESULT_CONTINUE;
}
#endif

#if 0
VMCMD_RESULT EvCmdBgmNowMapPlay( VMHANDLE *core, void *wk )
{
#if 0
  int zone_id = core->fsys->location->zone_id;
  //u16 music  = Snd_PcBgmNoGet( core->fsys, Snd_FieldBgmNoGet(core->fsys,zone_id) );
  u16 music  = Snd_FieldBgmNoGetNonBasicBank( core->fsys,zone_id );
  Snd_BgmPlay( music );
#else
  {
	  u16 trackBit = 0xfcff;	// track 9,10 OFF
    SCRCMD_WORK *work = wk;
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
    SCRIPT_FLDPARAM *fparam = SCRIPT_GetMemberWork(
        sc, ID_EVSCR_WK_FLDPARAM );
    u16 zone_id = FIELDMAP_GetZoneID( fparam->fieldMap );
    u16 bgm = ZONEDATA_GetBGMID( zone_id, GAMEDATA_GetSeasonID(gdata) );
    
    if( bgm != 0 ){
      PMSND_PlayNextBGM_EX( bgm, trackBit, 60, 0 );
    }
  }
#endif
  return VMCMD_RESULT_CONTINUE;
}
#endif

#if 0 //wb
//--------------------------------------------------------------
/**
 * ���oBGM�Đ�(���C�o���A�T�|�[�g�A��Ă�)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlayerFieldDemoBgmPlay( VMHANDLE *core, void *wk )
{
  Snd_PlayerFieldDemoBgmPlay( SND_SCENE_FIELD, VMGetU16(core) );
  return VMCMD_RESULT_CONTINUE;
}
#endif

#if 0 //wb
//--------------------------------------------------------------
/**
 * �t�B�[���hBGM���Œ�ɂ���t���O�Z�b�g(�Z�[�u���Ȃ�)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCtrlBgmFlagSet( VMHANDLE *core, void *wk )
{
  Snd_CtrlBgmFlagSet( VMGetU8(core) );
  return VMCMD_RESULT_CONTINUE;
}
#endif

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
  u8* p_sound_se_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_SOUND_SE_FLAG );

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
  u8* p_sound_se_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_SOUND_SE_FLAG );
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
  u8* p_sound_se_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_SOUND_SE_FLAG );

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
  u8* p_sound_se_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_SOUND_SE_FLAG );

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
  u16 no = VMGetU16( core );
  
  {
    SCRCMD_WORK *work = wk;
    GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
    FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
    FIELD_SOUND_PushPlayJingleBGM( fsnd, no );
  }
  
  return VMCMD_RESULT_CONTINUE;
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
  if( PMSND_CheckPlayBGM() == FALSE ){
    SCRCMD_WORK *work = wk;
    GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
    FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
    FIELD_SOUND_PopBGM( fsnd );
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
 * @todo  �F�X���O���������͗l�Ȃ̂ŁA�T�E���h�Ƃ��荇�킹��
 *
  //�؃��b�v�Đ��e�X�g
  //no = MONSNO_PERAPPU;
  //�p�^�[�����w��ł���֐��ɒu��������\��
 * ���莝���̐擪�̃|�P�����̖�����炷������������A�t�H����������K�v������I
  //�t�B�[���h��ŁA�X�J�C�t�H�������o�����邱�Ƃ͂Ȃ��B
  //��ĉ��ɗa����ƃm�[�}���t�H�����ɂȂ�̂ŁA��ĉ��̖�����OK
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdVoicePlay( VMHANDLE *core, void *wk )
{
  u16 monsno  = SCRCMD_GetVMWorkValue(core, wk);
  u8 formno = VMGetU8(core);
  u8 ptn = VMGetU8(core);     //���͎̂ĂĂ���

  core->vm_register[0] = PMV_PlayVoice( monsno, formno );

  return VMCMD_RESULT_CONTINUE;
}

//return 1 = �I��
static BOOL EvWaitVoicePlay(VMHANDLE *core, void *wk)
{
  if (PMVOICE_CheckPlay( core->vm_register[0] ) == FALSE)
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
