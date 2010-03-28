//======================================================================
/**
 * @file	scrcmd_traintown.c
 *
 * @brief	�X�N���v�g�R�}���h�F�g���C���^�E���֘A
 * @date  2010.03.28
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

#include "scrcmd_traintown.h"

#include "ev_time.h"    //EVTIME_Get

#include "arc/fieldmap/buildmodel_outdoor.naix" //NARC_output_buildmodel_


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �g���C���^�E���̓d�ԗp���f��ID���擾
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetTrainModelID( VMHANDLE * core, void *wk )
{
  u16 train_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  static const BMODEL_ID trains[] = {
    NARC_output_buildmodel_outdoor_tt_train01_nsbmd,  //�G���[�悯�p
    NARC_output_buildmodel_outdoor_tt_train01_nsbmd,
    NARC_output_buildmodel_outdoor_tt_train02_nsbmd,
    NARC_output_buildmodel_outdoor_tt_train03_nsbmd,
    NARC_output_buildmodel_outdoor_tt_train04_nsbmd,
    NARC_output_buildmodel_outdoor_tt_train05_nsbmd,
    NARC_output_buildmodel_outdoor_tt_train06_nsbmd,
    NARC_output_buildmodel_outdoor_tt_train07_nsbmd,
    NARC_output_buildmodel_outdoor_tt_train08_nsbmd,
    NARC_output_buildmodel_outdoor_tt_train09_nsbmd,
    NARC_output_buildmodel_outdoor_tt_train10_nsbmd,
  };
  if ( train_id >= NELEMS(trains) )
  {
    GF_ASSERT( 0 );
    train_id = 0;
  }
  *ret_wk = trains[train_id];
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �g���C���^�E���F�d�Ԃ̎�ޒ��I�R�}���h
 *
 * @li  0		���g���g���C���i��p�j
 * @li  1		�V���O���g���C��
 * @li  2		�_�u���g���C��
 * @li  3		�}���`�g���C��
 * @li  4		�X�[�p�[�_�u���g���C��
 * @li  5		�X�[�p�[�V���O���g���C��
 * @li  6		�X�[�p�[�}���`�g���C��
 * @li  7		�s�s�g���C��
 * @li  8		�Ȃ�
 * @li  9		Wi-Fi�g���C��
 * @li  10		�ŐV�^�ԗ��i��p�j10/1(�V�����̑n��) 10/14(�S���̓�)
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetTrainTownCondition( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK * work = wk;
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  int month = EVTIME_GetMonth( gamedata );
  int   day = EVTIME_GetDay( gamedata );

  if ( (month == 10 && day == 1) || (month == 10 && day == 14) )
  {
    *ret_wk = 10;   //���ʂȓ����ŐV�^���A�g���C��
  }
  else
  {
    static const u8 ids[10] = {
      9,  //?0:�Â����A�g���C��
      1,  //?1:�V���O���g���C��
      3,  //?2:�_�u���g���C��
      5,  //?3:�}���`�g���C��
      4,  //?4:�X�[�p�[�_�u���g���C��
      2,  //?5:�X�[�p�[�V���O���g���C��
      6,  //?6:�X�[�p�[�}���`�g���C��
      8,  //?7:Wi-Fi�g���C��
      0,  //?8:���₷��
      7,  //?9:�Â����A�g���C��

    };
    *ret_wk = ids[day % 10];
  }

  return VMCMD_RESULT_CONTINUE;
}


