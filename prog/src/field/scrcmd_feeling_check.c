//======================================================================
/**
 * @file  scrcmd_feeling_check.c
 * @brief  �X�N���v�g�R�}���h�p�֐��F�����`�F�b�N
 * @date  2010.02.01
 *
 */
//======================================================================

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"
#include "system/main.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_def.h"
#include "script_local.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_feeling_check.h"

#include "ev_time.h"

#include "savedata/irc_compatible_savedata.h"

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �����`�F�b�N�F�{���̒��ǂ��̐l�̖��O���o�b�t�@�ɃZ�b�g����
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetFeelingCheckName( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );    
  u16 set_pos = SCRCMD_GetVMWorkValue( core, work );
  SAVE_CONTROL_WORK* p_sv_ctrl  = GAMEDATA_GetSaveControlWork( gdata );
  IRC_COMPATIBLE_SAVEDATA * p_sv  = IRC_COMPATIBLE_SV_GetSavedata( p_sv_ctrl );
  
  u32 sex;
  const STRCODE *cp_name;
  u32 rank;


  GF_ASSERT_MSG( IRC_COMPATIBLE_SV_GetRankNum( p_sv ), "�N�Ƃ������`�F�b�N�����Ă��܂���\n" );

  //�v���C���[�̃f�[�^���쐬���A���ǂ��̐l�̃C���f�b�N�X�����o��
  { 
    const RTCDate *cp_date  = EVTIME_GetRTCDate( gdata );
    OSOwnerInfo info;
    OS_GetOwnerInfo( &info );
    rank  = IRC_COMPATIBLE_SV_GetBestBioRhythm( p_sv, 
        info.birthday.month, info.birthday.day, cp_date );
  }

  //�C���f�b�N�X���璇�ǂ��̐l�̃f�[�^�����o��
  sex     = IRC_COMPATIBLE_SV_GetSex( p_sv, rank );
  sex     = (sex == IRC_COMPATIBLE_SEX_MALE) ? PTL_SEX_MALE: PTL_SEX_FEMALE;
  cp_name = IRC_COMPATIBLE_SV_GetPlayerName( p_sv, rank );


  //WORDSET�ɐݒ�
  { 
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    WORDSET *wordset    = SCRIPT_GetWordSet( sc );
    STRBUF * word = GFL_STR_CreateBuffer( IRC_COMPATIBLE_SV_DATA_NAME_LEN, GFL_HEAP_LOWID( GFL_HEAPID_APP ) );
    GFL_STR_SetStringCode( word, cp_name );
    WORDSET_RegisterWord( wordset, set_pos, word, sex, TRUE, PM_LANG );
    OS_TPrintf( "�����`�F�b�Npos%d rank%d sex%d\n", set_pos, rank, sex );
    GFL_STR_DeleteBuffer( word );
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �����`�F�b�N�F�����`�F�b�N�����l�����擾����
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetFeelingCheckCount( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );
  SAVE_CONTROL_WORK* p_sv_ctrl  = GAMEDATA_GetSaveControlWork( gdata );
  IRC_COMPATIBLE_SAVEDATA * p_sv  = IRC_COMPATIBLE_SV_GetSavedata( p_sv_ctrl );

  //�����L���O���肵�Ă���l��Ԃ�
  *ret_wk = IRC_COMPATIBLE_SV_GetRankNum( p_sv );

  return VMCMD_RESULT_CONTINUE;
}

