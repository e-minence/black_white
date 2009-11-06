//======================================================================
/**
 * @file	scrcmd_screeneffects.c
 * @brief	�X�N���v�g�R�}���h�F�V�X�e���Ǘ��f�[�^�֘A
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

#include "scrcmd_enviroments.h"

#include "system/rtc_tool.h"  //GFL_RTC_GetTimeZone
#include "field/zonedata.h"   //ZONEDATA_GetMessageArcID
#include "savedata/mystatus.h"  //MyStatus_�`

#include "savedata/save_control.h"



//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �����o�[�W�����̎擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetRomVersion( VMHANDLE *core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = PM_VERSION; //include/pm_version.h
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�\�����[�h�i�����E���ȁj�̐؂�ւ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChangeLangID( VMHANDLE *core, void *wk )
{
  u8 id = GFL_MSGSYS_GetLangID();
  if( id == 0 ){ //�Ђ炪��
    id = 1;
  }else if( id == 1 ){ //����
    id = 0;
  }
  GFL_MSGSYS_SetLangID( id );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�\�����[�h�i�����E���ȁj�̎擾
 * @retval  VMCMD_RESULT_CONTINUE
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetLangID( VMHANDLE *core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = GFL_MSGSYS_GetLangID();
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �����̎擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetRand( VMHANDLE *core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 limit = SCRCMD_GetVMWorkValue( core, wk );
  *ret_wk = GFUser_GetPublicRand( limit );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ���݂̃X�N���v�g�ł̃��b�Z�[�W�A�[�J�C�uID���擾
 * @todo
 * ���݂̃]�[��ID���烁�b�Z�[�W�w��ID�������������Ă��邪�A
 * �{���̓X�N���v�g�N�����Ɏg�p���郁�b�Z�[�W�A�[�J�C�uID��ۑ����Ă����A
 * ��������킽���悤�Ȏd�g�݂Ƃ���ׂ�
 * �i�����łȂ���common_scr�Ȃǂ̏ꍇ�ɖ��Ȃ��ƂɂȂ�j
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetNowMsgArcID( VMHANDLE * core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  { 
    GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
    PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
    u16 zone_id = PLAYERWORK_getZoneID( player );
    *ret_wk = ZONEDATA_GetMessageArcID( zone_id );
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �g���[�i�[�J�[�h�����N�̎擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo  �Ƃ肠�����o�b�W�̐���Ԃ��Ă���B
 * �g���[�i�[�J�[�h�̊�悪�ł����炻��ɂ��킹�Ė߂�l��������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetTrainerCardRank( VMHANDLE *core, void *wk )
{
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );

  *ret_wk = MyStatus_GetBadgeCount( GAMEDATA_GetMyStatus(gdata) );
  return VMCMD_RESULT_CONTINUE;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ���ԑт̎擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo 
 * ���ԑт��G�߂ŕς�邱�Ƃ��ǂ����邩���Ƌ��c�B
 * ����RTC�łȂ��C�x���g�ŕێ����Ă��鎞�ԑт��Q�Ƃ���̂��H
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetTimeZone( VMHANDLE *core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = GFL_RTC_GetTimeZone();
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �j���̎擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo  RTC���璼�ڎ擾���Ă��邪�A�{���ɂ���ł悢�̂��H��������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetWeek( VMHANDLE * core, void *wk )
{
  RTCDate now_date;
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );

  GFL_RTC_GetDate( &now_date );
  *ret_wk = now_date.week;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ���t�̎擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param  wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo  RTC���璼�ڎ擾���Ă��邪�A�{���ɂ���ł悢�̂��H��������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetDate( VMHANDLE * core, void *wk )
{
  RTCDate now_date;
  u16 *ret_month = SCRCMD_GetVMWork( core, wk ); // ������
  u16 *ret_day   = SCRCMD_GetVMWork( core, wk ); // ������

  GFL_RTC_GetDate( &now_date );
  *ret_month = now_date.month;
  *ret_day   = now_date.day;
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief ���݂̋G�߂��擾����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetSeasonID( VMHANDLE *core, void * wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  *ret_wk = GAMEDATA_GetSeasonID( gdata );
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief �����̒a�����̎擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBirthDay( VMHANDLE * core, void*wk )
{
  u16 *month_wk = SCRCMD_GetVMWork( core, wk );
  u16 *day_wk = SCRCMD_GetVMWork( core, wk );
  /* ������ */
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �����̐��ʂ��擾����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetMySex( VMHANDLE *core, void * wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
  *ret_wk = MyStatus_GetMySex( &player->mystatus );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �o�b�W�t���O�̎擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBadgeFlag( VMHANDLE * core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 badge_id = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );

  *ret_wk = MyStatus_GetBadgeFlag( GAMEDATA_GetMyStatus(gdata), badge_id );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �o�b�W�t���O�̃Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetBadgeFlag( VMHANDLE * core, void *wk )
{
  u16 badge_id = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );

  MyStatus_SetBadgeFlag( GAMEDATA_GetMyStatus(gdata), badge_id );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �o�b�W�t���O�擾���̃J�E���g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBadgeCount( VMHANDLE * core, void *wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );

  *ret_wk = MyStatus_GetBadgeCount( GAMEDATA_GetMyStatus(gdata) );
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * �Z�[�u�f�[�^�̏�Ԃ��擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo �������ރf�[�^�T�C�Y�ɉ�����, ���̒i�K��Ԃ�
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetSaveDataStatus( VMHANDLE * core, void *wk )
{
  GAMEDATA*        gdata = SCRCMD_WORK_GetGameData( wk );
  SAVE_CONTROL_WORK* scw = GAMEDATA_GetSaveControlWork( gdata );
  u16*         ret_exist = SCRCMD_GetVMWork( core, wk );  // �R�}���h��1����
  u16*           ret_new = SCRCMD_GetVMWork( core, wk );  // �R�}���h��2����
  u16*        ret_volume = SCRCMD_GetVMWork( core, wk );  // �R�}���h��3����
  u32         total_size = 0; // �Z�[�u�S�̂̃T�C�Y
  u32        actual_size = 0; // �Z�[�u�������T�C�Y

  // �Z�[�u�f�[�^�����݂��邩�ǂ���
  *ret_exist = SaveData_GetExistFlag( scw );

  // �v���C���̃f�[�^���V�K�f�[�^���ǂ���
  *ret_new   = SaveControl_NewDataFlagGet( scw );

  // @todo �������ރf�[�^�T�C�Y�ɉ�����, ���̒i�K��Ԃ�
  SaveControl_GetActualSize( scw, &actual_size, &total_size );
  *ret_volume = 0;

  return VMCMD_RESULT_CONTINUE;
} 

//--------------------------------------------------------------
/**
 * @brief ���[�vID�̃Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * @todo  �ǂ̃��C���[�ŕs���Ȓl�̃`�F�b�N�����邩��������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetWarpID( VMHANDLE * core, void *wk )
{
  GAMEDATA*        gamedata = SCRCMD_WORK_GetGameData( wk );
  u16 warp_id = SCRCMD_GetVMWorkValue( core, wk );

  GAMEDATA_SetWarpID( gamedata, warp_id );

  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief 
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetZukanFlag( VMHANDLE * core, void *wk )
{
  u16 set_mode = SCRCMD_GetVMWorkValue( core, wk );
  u16 monsno = SCRCMD_GetVMWorkValue( core, wk );

  /* ������ */
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief 
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZukanCount( VMHANDLE * core, void *wk )
{
  u16 get_mode = SCRCMD_GetVMWorkValue( core, wk );
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );

  /* ������ */
  return VMCMD_RESULT_CONTINUE;
}
