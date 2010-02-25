//======================================================================
/**
 * @file	scrcmd_enviroments.c
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
#include "savedata/config.h" // CONFIG
#include "savedata/box_savedata.h"  //BOX_MANAGER
#include "savedata/trainercard_data.h" // TR_CARD_SV_PTR

#include "savedata/save_control.h"
#include "savedata/zukan_savedata.h"

#include "field/map_matrix.h"   //MAP_MATRIX
#include "map_replace.h"    //MAPREPLACE_ChangeFlag
#include "field/field_const.h"  //GRID_TO_FX32

#include "../../../resource/fldmapdata/flagwork/flag_define.h"  //for SYS_FLAG_
#include "report.h" //REPORT_SAVE_TYPE_VAL


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
  *ret_wk = GET_VERSION(); //include/pm_version.h
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
  // �ݒ��ʂƐH���Ⴄ���ۂ��N���Ă����̂ŁA�Z�[�u�f�[�^������悤��
  // �C�����܂����B���؋�
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  SAVE_CONTROL_WORK * sv = GAMEDATA_GetSaveControlWork( gdata );
  CONFIG *config  = SaveData_GetConfig( sv );

  MOJIMODE id = CONFIG_GetMojiMode( config );
  if( id == MOJIMODE_HIRAGANA ){ //�Ђ炪��
    id = MOJIMODE_KANJI;
  }else if( id == MOJIMODE_KANJI ){ //����
    id = MOJIMODE_HIRAGANA;
  }
  CONFIG_SetMojiMode( config, id );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�\�����[�h�i�����E���ȁj�̎擾
 * @retval  VMCMD_RESULT_CONTINUE
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetLangID( VMHANDLE *core, void *wk )
{
  // �ݒ��ʂƐH���Ⴄ���ۂ��N���Ă����̂ŁA�Z�[�u�f�[�^������悤��
  // �C�����܂����B���؋�
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  SAVE_CONTROL_WORK * sv = GAMEDATA_GetSaveControlWork( gdata );
  CONFIG *config  = SaveData_GetConfig( sv );

  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = CONFIG_GetMojiMode( config );
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

  *ret_wk = MISC_GetBadgeCount( GAMEDATA_GetMiscWork(gdata) );
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

  *ret_wk = MISC_GetBadgeFlag( GAMEDATA_GetMiscWork(gdata), badge_id );
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

  MISC_SetBadgeFlag( GAMEDATA_GetMiscWork(gdata), badge_id );
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

  *ret_wk = MISC_GetBadgeCount( GAMEDATA_GetMiscWork(gdata) );
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
  if ( actual_size * REPORT_SAVE_TYPE_VAL < total_size )
  {
    *ret_volume = 0;  //�ӂ�
  } else {
    *ret_volume = 1;  //��������
  }

  return VMCMD_RESULT_CONTINUE;
} 

//======================================================================
//
//
//  �}�b�v�ڑ��֘A�Ȃ�
//  
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �]�[��ID���擾����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZoneID( VMHANDLE *core, void * wk )
{
  u16 *ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = SCRCMD_WORK_GetZoneID( wk );
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

//--------------------------------------------------------------
/**
 * @brief ����ڑ���̐ݒ�F���݈ʒu���L��
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetSpLocationHere( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA*        gamedata = SCRCMD_WORK_GetGameData( wk );
  LOCATION spLoc;
  PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gamedata );
  const VecFx32 * vec = PLAYERWORK_getPosition( player );
  LOCATION_SetDirect( &spLoc, PLAYERWORK_getZoneID( player ), 
      PLAYERWORK_getDirection_Type( player ), vec->x, vec->y, vec->z );
  GAMEDATA_SetSpecialLocation( gamedata, &spLoc );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ����ڑ���̐ݒ�F���ڎw��
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetSpLocationDirect( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA*        gamedata = SCRCMD_WORK_GetGameData( wk );
  LOCATION spLoc;

  u16 zone_id = SCRCMD_GetVMWorkValue( core, work );
  u16 dir = SCRCMD_GetVMWorkValue( core, work );
  u16 gx = SCRCMD_GetVMWorkValue( core, work );
  u16 gy = SCRCMD_GetVMWorkValue( core, work );
  u16 gz = SCRCMD_GetVMWorkValue( core, work );

  LOCATION_SetDirect( &spLoc, zone_id, dir, 
      GRID_TO_FX32( gx ), GRID_TO_FX32( gy ), GRID_TO_FX32( gz ) );
  GAMEDATA_SetSpecialLocation( gamedata, &spLoc );
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief �}�b�v�u���������N�G�X�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdChangeMapReplaceFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = wk;
  GAMESYS_WORK * gamesys = SCRCMD_WORK_GetGameSysWork( wk );
  GAMEDATA*        gamedata = SCRCMD_WORK_GetGameData( wk );

  u16 id = SCRCMD_GetVMWorkValue( core, work );
  u16 on_off = SCRCMD_GetVMWorkValue( core, work );
  u16 direct = SCRCMD_GetVMWorkValue( core, work );

  MAPREPLACE_ChangeFlag( gamedata, id, on_off );
  if ( direct )
  {
    MAP_MATRIX * pMat = GAMEDATA_GetMapMatrix( gamedata );
    MAP_MATRIX_CheckReplace( pMat, gamesys );
  }
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
 * @todo
 * ������t���O�𐳊m�ɃZ�b�g���邽�߂ɂ�POKEMON_PARAM���K�v�����A
 * ���݂��Ȃ��̂ŃR�}���h�����Ő������Ă���B
 * POKEMON_PARAM�ˑ��̂��̂��X�N���v�g���w�肵�Ȃ��悤�Ȏd�g�݂��K�v�B
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetZukanFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA* gamedata = SCRCMD_WORK_GetGameData( work );
  ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  HEAPID     heap_id = SCRCMD_WORK_GetHeapID( work );
  u16 set_mode = SCRCMD_GetVMWorkValue( core, work ); // ����1
  u16 monsno = SCRCMD_GetVMWorkValue( core, work ); // ����2

  POKEMON_PARAM * pp = PP_Create( monsno, 1, PTL_SETUP_ID_AUTO, heap_id );
  switch( set_mode )
  {
  case ZUKANCTRL_MODE_SEE:  // ����
    ZUKANSAVE_SetPokeSee( zukan, pp );
    break;
  case ZUKANCTRL_MODE_GET:  // �߂܂���
    GF_ASSERT(0); //�|�P�����Q�b�g�ɂ͑Ή����Ȃ��B�V�X�e���ōs���ׂ��B
    break;
  default:
    OS_Printf( "==============================================================\n" );
    OS_Printf( "�X�N���v�g: �}�ӃZ�b�g�R�}���h�Ɏw�肷������Ɍ�肪����܂��B\n" );
    OS_Printf( "==============================================================\n" );
    break;
  } 
  GFL_HEAP_FreeMemory( pp );
  
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief   �}�Ӄt���O�̎擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZukanFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA* gamedata = SCRCMD_WORK_GetGameData( work );
  const ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  u16 get_mode = SCRCMD_GetVMWorkValue( core, work ); // ����1
  u16 monsno = SCRCMD_GetVMWorkValue( core, work ); // ����2
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );

  switch( get_mode )
  {
  case ZUKANCTRL_MODE_SEE:  // ����
    *ret_wk = ZUKANSAVE_GetPokeSeeFlag( zukan, monsno );
    break;
  case ZUKANCTRL_MODE_GET:  // �߂܂���
    *ret_wk = ZUKANSAVE_GetPokeGetFlag( zukan, monsno );
    break;
  default:
    GF_ASSERT(0);
    *ret_wk = FALSE;
    break;
  } 
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief �}�Ӄt���O�̃J�E���g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZukanCount( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK* work = wk;
  GAMEDATA* gamedata = SCRCMD_WORK_GetGameData( work );
  const ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  u16 get_mode = SCRCMD_GetVMWorkValue( core, work );
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );

  switch( get_mode )
  {
  case ZUKANCTRL_MODE_SEE:  // ������
    *ret_wk = ZUKANSAVE_GetPokeSeeCount( zukan );
    break;
  case ZUKANCTRL_MODE_GET:  // �߂܂�����
    *ret_wk = ZUKANSAVE_GetPokeGetCount( zukan );
    break;
  default:
    *ret_wk = 0;
    OS_Printf( "================================================================\n" );
    OS_Printf( "�X�N���v�g: �}�ӃJ�E���g�R�}���h�Ɏw�肷������Ɍ�肪����܂��B\n" );
    OS_Printf( "================================================================\n" );
    break;
  } 
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ������̋@�\����F�S���}�Ӄt���O�̃Z�b�g
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetZukanZenkokuFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*      work = wk;
  GAMEDATA*     gamedata = SCRCMD_WORK_GetGameData( work );
  ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  ZUKANSAVE_SetZenkokuZukanFlag( zukan );
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief ������̋@�\����F�S���}�Ӄt���O�̎擾
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZukanZenkokuFlag( VMHANDLE * core, void *wk )
{
  u16 *           ret_wk = SCRCMD_GetVMWork( core, wk );
  SCRCMD_WORK*      work = wk;
  GAMEDATA*     gamedata = SCRCMD_WORK_GetGameData( work );
  ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  *ret_wk = ZUKANSAVE_GetZenkokuZukanFlag( zukan );
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
/**
 * @brief ������̋@�\����F�t�H�����ڍ׉�ʒǉ�
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetZukanGraphicFlag( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*      work = wk;
  GAMEDATA*     gamedata = SCRCMD_WORK_GetGameData( work );
  ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gamedata );
  ZUKANSAVE_SetGraphicVersionUpFlag( zukan );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ���C�����X�Z�[�u���[�h���擾����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param  wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetWirelessSaveMode( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork( gdata );
  CONFIG*          config = SaveData_GetConfig( save );
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );  // �R�}���h������
  WIRELESSSAVE_MODE mode;

  // ���C�����X�Z�[�u���[�h�擾
  mode = CONFIG_GetWirelessSaveMode( config );
  OBATA_Printf( "=====================\n" );
  OBATA_Printf( "wirelesssavemode = %d\n", mode );
  OBATA_Printf( "=====================\n" );

  // �Ή�����萔��Ԃ�
  switch( mode )
  {
  case WIRELESSSAVE_OFF:  *ret_wk = WIRELESS_SAVEMODE_OFF;  break;
  case WIRELESSSAVE_ON:   *ret_wk = WIRELESS_SAVEMODE_ON;   break;
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �V���[�Y�Q�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param  wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetShoes( VMHANDLE *core, void *wk )
{
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  EVENTWORK_SetEventFlag( evwork, SYS_FLAG_RUNNINGSHOES );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �{�b�N�X�ǉ��ǎ����Z�b�g����
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBoxWallPaper( VMHANDLE * core, void * wk )
{
  u16 flag = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  BOX_MANAGER * box = GAMEDATA_GetBoxManager( gamedata );
  if ( flag == SCR_BOX_EX_WALLPAPER1 || flag == SCR_BOX_EX_WALLPAPER2 )
  {
    BOXDAT_SetExWallPaperFlag( box, flag );
  }
  else
  {
    GF_ASSERT( 0 );
  }
  return VMCMD_RESULT_CONTINUE;
}
SDK_COMPILER_ASSERT( SCR_BOX_EX_WALLPAPER1 == BOX_EX_WALLPAPER_SET_FLAG_1 );
SDK_COMPILER_ASSERT( SCR_BOX_EX_WALLPAPER2 == BOX_EX_WALLPAPER_SET_FLAG_2 );



//--------------------------------------------------------------
/**
 * @brief �g���[�i�[�^�C�v���ύX����Ă��邩�ǂ������`�F�b�N����.
 *        �ύX����Ă��Ȃ��ꍇ, 
 *        �� �ύX�ς݂ɐݒ肵, 
 *        �� �f�t�H���g�Őݒ肳�ꂽ�g���[�i�[�^�C�v���擾����.
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param  wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckPlayerViewChange( VMHANDLE * core, void * wk ) {
  SCRCMD_WORK* work       = (SCRCMD_WORK*)wk;
  u16*         retWk_disp = SCRCMD_GetVMWork( core, work ); // ������: �\������Ă��邩�ǂ����̊i�[��
  u16*         retWk_type = SCRCMD_GetVMWork( core, work ); // ������: ���g�̃g���[�i�[�^�C�v�̊i�[��
  GAMEDATA*    gameData   = SCRCMD_WORK_GetGameData( work );
  PLAYER_WORK* player     = GAMEDATA_GetMyPlayerWork( gameData );
  MYSTATUS*    mystatus   = &( player->mystatus );
  SAVE_CONTROL_WORK* saveWork   = GAMEDATA_GetSaveControlWork( gameData );
  TR_CARD_SV_PTR     trCardSave = TRCSave_GetSaveDataPtr( saveWork );

  // ���g�̃g���[�i�[�^�C�v���擾
  *retWk_type = MyStatus_GetTrainerView( mystatus );

  // �\������Ă��邩�ǂ������擾
  *retWk_disp = TRCSave_GetTrainerViewChange( trCardSave );

  // �\������Ă����ԂɕύX
  TRCSave_SetTrainerViewChange( trCardSave );
  
  return VMCMD_RESULT_CONTINUE;
}
