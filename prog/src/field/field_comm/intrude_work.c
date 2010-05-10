//==============================================================================
/**
 * @file    intrude_work.c
 * @brief   �N���V�X�e�����[�N�ɊO������G��ׂ̊֐��Q
 * @author  matsuda
 * @date    2009.10.10(�y)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_comm.h"
#include "savedata/mystatus.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "field/intrude_comm.h"
#include "intrude_comm_command.h"
#include "net_app/union/comm_player.h"
#include "intrude_types.h"
#include "intrude_comm_command.h"
#include "intrude_main.h"
#include "bingo_types.h"
#include "intrude_work.h"
#include "field/field_status.h"
#include "field/field_status_local.h"
#include "fieldmap/zone_id.h"
#include "field/zonedata.h"
#include "field/eventwork.h"
#include "../../../../resource/fldmapdata/flagwork/flag_define.h" //SYS_FLAG_


//==============================================================================
//  �f�[�^
//==============================================================================
#include "palace_zone_id_tbl.cdat"


//==================================================================
/**
 * �N���V�X�e���Ƃ��ď펞�ʐM���N�����Ă��悢��Ԃ��`�F�b�N����
 *
 * @param   gsys		
 * @param   zone_id	
 *
 * @retval  BOOL		    TRUE:�펞�ʐM���N�����Ă��悢
 * @retval  BOOL		    TRUE:�N�����Ă͂����Ȃ�
 */
//==================================================================
BOOL Intrude_Check_AlwaysBoot(GAMESYS_WORK *gsys)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  
  if(GAMEDATA_GetIntrudeReverseArea(gamedata) == TRUE){
    //���t�B�[���h�ɂ��āA�ʐM�G���[�X�e�[�^�X�������c���Ă���ꍇ��
    //�N���Ƃ��ăG���[�����A�ؒf��̃C�x���g�����Ȃǂ��c���Ă���̂�
    //�펞�ʐM�������Ȃ�
    if(GameCommSys_GetLastStatus(game_comm) != GAME_COMM_LAST_STATUS_NULL){
      return FALSE;
    }
    
    //�`���[�g���A�����������Ă��Ȃ��āA���t�B�[���h�ɂ���ꍇ�́u�q�v�ɂȂ�Ȃ��悤��
    //�펞�ʐM���N�����Ă͂����Ȃ�
    if(Intrude_CheckTutorialComplete(gamedata) == FALSE){
      return FALSE;
    }
  }
  

  return TRUE;
}

//==================================================================
/**
 * �N���ʐM������Ɍq�����Ă��邩���ׂ�
 *
 * @param   game_comm		
 *
 * @retval  INTRUDE_COMM_SYS_PTR	�q�����Ă���Ȃ� intcomm
 * @retval  INTRUDE_COMM_SYS_PTR	�q�����Ă��Ȃ��Ȃ� NULL
 *
 * �ʐM����̕Ԏ��҂��A�e�̕Ԏ��҂��A�Ȃǂ��Ă��鎞�ɁA�r���ŃG���[���������Ă��Ȃ���
 * �Ď����鏈�������ʉ��B
 */
//==================================================================
INTRUDE_COMM_SYS_PTR Intrude_Check_CommConnect(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
  
  if(NetErr_App_CheckError() || GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION 
      || GameCommSys_CheckSystemWaiting(game_comm) == TRUE || intcomm == NULL){
    return NULL;
  }
  return intcomm;
}

//==================================================================
/**
 * �A�N�V�����X�e�[�^�X��ݒ�
 *
 * @param   game_comm		
 * @param   action		
 */
//==================================================================
void IntrudeWork_SetActionStatus(GAME_COMM_SYS_PTR game_comm, INTRUDE_ACTION action)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  Intrude_SetActionStatus(intcomm, action);
  intcomm->send_status = TRUE;
}

//==================================================================
/**
 * ���[�v����v���C���[�����ݒ�
 *
 * @param   game_comm		
 * @param   town_tblno		
 */
//==================================================================
void Intrude_SetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm, int net_id)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  intcomm->warp_player_netid = net_id;
}

//==================================================================
/**
 * ���[�v��̃v���C���[������擾
 *
 * @param   game_comm		
 *
 * @retval  int		�X�ԍ�(PALACE_TOWN_DATA_NULL�̏ꍇ�͖���)
 */
//==================================================================
int Intrude_GetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return GAMEDATA_GetIntrudeMyID( GameCommSys_GetGameData(game_comm) );
  }
  return intcomm->warp_player_netid;
}

//==================================================================
/**
 * ���[�v����X�ԍ���ݒ�
 *
 * @param   game_comm		
 * @param   town_tblno		
 */
//==================================================================
void Intrude_SetWarpTown(GAME_COMM_SYS_PTR game_comm, int town_tblno)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  intcomm->warp_town_tblno = town_tblno;
}

//==================================================================
/**
 * ���[�v��̊X�ԍ����擾
 *
 * @param   game_comm		
 *
 * @retval  int		�X�ԍ�(PALACE_TOWN_DATA_NULL�̏ꍇ�͖���)
 */
//==================================================================
int Intrude_GetWarpTown(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return PALACE_TOWN_DATA_NULL;
  }
  return intcomm->warp_town_tblno;
}

//==================================================================
/**
 * ����w��̃^�C�~���O�R�}���h����M�ł��Ă��邩�m�F���܂�
 *
 * @param   intcomm		
 * @param   timing_no		    �����ԍ�
 * @param   target_netid		���������NetID
 *
 * @retval  BOOL		TRUE:������芮���B�@FALSE:�����҂�
 */
//==================================================================
BOOL Intrude_GetTargetTimingNo(INTRUDE_COMM_SYS_PTR intcomm, u8 timing_no, u8 target_netid)
{
  u8 recv_timing_no = intcomm->recv_target_timing_no;
  
  if(timing_no == recv_timing_no && target_netid == intcomm->recv_target_timing_netid){
    intcomm->recv_target_timing_no = 0;
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �w�肵���p���X�G���A�̐苒�����擾����
 *
 * @param   intcomm		
 * @param   palace_area		  
 *
 * @retval  OCCUPY_INFO *		
 */
//==================================================================
OCCUPY_INFO * Intrude_GetOccupyInfo(INTRUDE_COMM_SYS_PTR intcomm, int palace_area)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  OCCUPY_INFO *occupy;
  
  if(palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  }
  else{
    occupy = GAMEDATA_GetOccupyInfo(gamedata, palace_area);
  }
  
  return occupy;
}

//==================================================================
/**
 * �w�肵��NetID��MYSTATUS���擾����
 *
 * @param   intcomm		
 * @param   net_id	  
 *
 * @retval  MYSTATUS *		
 */
//==================================================================
MYSTATUS * Intrude_GetMyStatus(INTRUDE_COMM_SYS_PTR intcomm, int net_id)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  MYSTATUS *myst;
  
  if(net_id == GAMEDATA_GetIntrudeMyID(gamedata)){
    myst = GAMEDATA_GetMyStatus(gamedata);
  }
  else{
    myst = GAMEDATA_GetMyStatusPlayer(gamedata, net_id);
  }
  
  return myst;
}

//==================================================================
/**
 * �w��p���X�G���A�̃~�b�V�����I����⃊�X�g�̃|�C���^���擾
 *
 * @param   intcomm		
 * @param   palace_area		�p���X�G���A
 *
 * @retval  MISSION_CHOICE_LIST *		�~�b�V�����I����⃊�X�g�ւ̃|�C���^
 */
//==================================================================
MISSION_CHOICE_LIST * Intrude_GetChoiceList(INTRUDE_COMM_SYS_PTR intcomm, int palace_area)
{
  return &intcomm->mission.list[palace_area];
}

//==================================================================
/**
 * ������������p���X�G���ANo���擾����
 *
 * @param   intcomm		
 *
 * @retval  u8		�p���X�G���ANo
 */
//==================================================================
u8 Intrude_GetPalaceArea(INTRUDE_COMM_SYS_PTR intcomm)
{
  return intcomm->intrude_status_mine.palace_area;
}

//==================================================================
/**
 * WFBC�p�����[�^����M���������擾
 *
 * @param   intcomm		
 *
 * @retval  BOOL		TRUE:��M�����@FALSE:��M���Ă��Ȃ�
 */
//==================================================================
BOOL Intrude_GetRecvWfbc(INTRUDE_COMM_SYS_PTR intcomm)
{
  return intcomm->wfbc_recv;
}

//==================================================================
/**
 * WFBC�ʐM���[�N���擾
 *
 * @param   intcomm		
 *
 * @retval  WFBC_COMM_DATA *		WFBC�ʐM���[�N�ւ̃|�C���^
 */
//==================================================================
WFBC_COMM_DATA * Intrude_GetWfbcCommData(INTRUDE_COMM_SYS_PTR intcomm)
{
  return &intcomm->wfbc_comm_data;
}

//==================================================================
/**
 * ���������݂���N�����PM_VERSION���擾
 *
 * @param   game_comm		
 *
 * @retval  int		����������ROM��PM_VERSION
 *                (�ʐM���Ă��Ȃ��A�N�����Ă��Ȃ��A�ʐM�G���[���͎�����PM_VERSION)
 */
//==================================================================
u8 Intrude_GetRomVersion(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  const MYSTATUS *myst;
  u8 palace_area;
  
  if(intcomm == NULL){
    return GET_VERSION();
  }
  
  palace_area = Intrude_GetPalaceArea(intcomm);
  myst = Intrude_GetMyStatus(intcomm, palace_area);
  return MyStatus_GetRomCode( myst );
}

//==================================================================
/**
 * ���������݂���N����̋G�߂��擾
 *
 * @param   game_comm		
 *
 * @retval  u8		����������ROM�̋G��ID
 *                (�ʐM���Ă��Ȃ��A�N�����Ă��Ȃ��A�ʐM�G���[���͎�����ROM�̋G��)
 */
//==================================================================
u8 Intrude_GetSeasonID(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  GAMEDATA *gamedata = GameCommSys_GetGameData(game_comm);
  u8 palace_area;
  
  if(intcomm == NULL){
    return GAMEDATA_GetSeasonID(gamedata);
  }
  
  palace_area = Intrude_GetPalaceArea(intcomm);
  if(palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    return GAMEDATA_GetSeasonID(gamedata);
  }
  return intcomm->intrude_status[palace_area].season;
}

//==================================================================
/**
 * �ׂ̃p���X�������̃p���X���𒲂ׂ�
 *
 * @param   game_comm		
 * @param   gamedata		
 * @param   dir		      ���ׂ�ׂ̌���(DIR_LEFT or DIR_RIGHT)
 *
 * @retval  BOOL		    TRUE�F�����̃p���X�@FALSE:�����ł͂Ȃ��l�̃p���X
 */
//==================================================================
BOOL Intrude_CheckNextPalaceAreaMine(GAME_COMM_SYS_PTR game_comm, const GAMEDATA *gamedata, u16 dir)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  int palace_area;
  
  if(intcomm == NULL){
    return TRUE;
  }
  
  palace_area = intcomm->intrude_status_mine.palace_area;
  if(dir == DIR_LEFT){
    palace_area--;
    if(palace_area < 0){
      palace_area = intcomm->connect_map_count; //intcomm->member_num - 1;
    }
  }
  else{
    palace_area++;
    if(palace_area >= intcomm->connect_map_count + 1){ //intcomm->member_num){
      palace_area = 0;
    }
  }
  
  if(palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �����o�[�W���������������N���Ƃ��Ă�ROM�o�[�W�������擾����
 *    ���g�p�ꏊ�ɂ͒��ӂ��邱�ƁI
 *
 * @param   pm_version		
 * @param   trainer_id		
 *
 * @retval  u8		ROM�o�[�W����
 *
 * �����o�[�W�����̏ꍇ�̓g���[�i�[ID���獕�����A�ǂ��炩�̃o�[�W������Ԃ��܂�
 */
//==================================================================
u8 Intrude_GetIntrudeRomVersion(u8 pm_version, u32 trainer_id)
{
  if(pm_version != VERSION_BLACK && pm_version != VERSION_WHITE){
    if(trainer_id & (1 << 16)){
      pm_version = VERSION_WHITE;
    }
    else{
      pm_version = VERSION_BLACK;
    }
  }
  return pm_version;
}

//==================================================================
/**
 * �O���[�X�P�[���ɕύX����K�v������}�b�v���𒲂ׂ�
 *
 * @param   game_comm		
 * @param   gamedata		
 *
 * @retval  GRAYSCALE_TYPE	�O���[�X�P�[���^�C�v
 */
//==================================================================
GRAYSCALE_TYPE Intrude_CheckGrayScaleMap(GAME_COMM_SYS_PTR game_comm, GAMEDATA *gamedata)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  u8 invasion_netid, pm_version;
  const MYSTATUS *myst;
  
  if (FIELD_STATUS_GetMapMode( GAMEDATA_GetFieldStatus( gamedata) ) != MAPMODE_INTRUDE ) {
    return GRAYSCALE_TYPE_NULL;
  }
  
  if(intcomm != NULL){  //intcomm������ꍇ��intcomm���玝���Ă���
    invasion_netid = Intrude_GetPalaceArea(intcomm);
  }
  else{
    invasion_netid = GameCommStatus_GetPlayerStatus_InvasionNetID(game_comm, GAMEDATA_GetIntrudeMyID(gamedata));
  }
  
  if(invasion_netid == GAMEDATA_GetIntrudeMyID(gamedata)){
    return GRAYSCALE_TYPE_NULL;
  }
  
  myst = GAMEDATA_GetMyStatusPlayer(gamedata, invasion_netid);
  pm_version = Intrude_GetIntrudeRomVersion(MyStatus_GetRomCode(myst), MyStatus_GetID(myst));
  switch(pm_version){
  case VERSION_WHITE:
    return GRAYSCALE_TYPE_WHITE;
  case VERSION_BLACK:
    return GRAYSCALE_TYPE_BLACK;
  }
  return GRAYSCALE_TYPE_NEXT;
}

//==================================================================
/**
 * �p���X�ŒN���ƌq�����Ă��邩���擾����
 *
 * @param   game_comm		
 *
 * @retval  BOOL		    TRUE:�p���X�ŒN���Ɛڑ����
 * @retval  BOOL		    FALSE:�N�Ƃ��q�����Ă��Ȃ�
 */
//==================================================================
BOOL Intrude_CheckPalaceConnect(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return FALSE;
  }
  if(intcomm->recv_profile > 1 && GFL_NET_GetConnectNum() > 1){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �p���X�ɐڑ����Ă��悢ZONE�ɂ��邩�𔻒�
 *
 * @param   zone_id		
 *
 * @retval  BOOL		  TRUE:�ڑ����Ă悢
 */
//==================================================================
BOOL Intrude_CheckZonePalaceConnect(u16 zone_id)
{
  int i;
  
  for(i = 0; i < NELEMS(PalaceZoneIDTbl); i++){
    if(PalaceZoneIDTbl[i] == zone_id){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �`���[�g���A����S�Ă��Ȃ��Ă��邩�`�F�b�N����
 *
 * @param   gamedata		
 *
 * @retval  BOOL		TRUE:�S�Ă��Ȃ����@FALSE:���Ȃ��Ă��Ȃ��̂��c���Ă���
 */
//--------------------------------------------------------------
BOOL Intrude_CheckTutorialComplete(GAMEDATA *gamedata)
{
  EVENTWORK *evwork = GAMEDATA_GetEventWork(gamedata);
  static const u16 check_flag_array[] = {
    SYS_FLAG_PALACE_MISSION_START,
    SYS_FLAG_PALACE_MISSION_CLEAR,
    SYS_FLAG_PALACE_DPOWER,
  };
  int i;
  
  for(i = 0; i < NELEMS(check_flag_array); i++){
    if(EVENTWORK_CheckEventFlag(evwork, check_flag_array[i]) == FALSE){
      return FALSE;
    }
  }
  return TRUE;
}
