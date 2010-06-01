//==============================================================================
/**
 * @file    intrude_comm.c
 * @brief   �N���ʐM���C��
 * @author  matsuda
 * @date    2009.09.02(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "savedata/mystatus.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "field/intrude_comm.h"
#include "intrude_comm_command.h"
#include "net_app/union/comm_player.h"
#include "field/fieldmap_proc.h"
#include "field/field_player.h"
#include "intrude_types.h"
#include "intrude_comm_command.h"
#include "intrude_main.h"
#include "savedata/mystatus.h"
#include "intrude_field.h"
#include "intrude_mission.h"
#include "net/net_whpipe.h"
#include "gamesystem/game_beacon_accessor.h"
#include "intrude_work.h"
#include "field/zonedata.h"
#include "field/fieldmap_call.h"  //FIELDMAP_IsReady


//==============================================================================
//  �萔��`
//==============================================================================
///�~�b�V�����J�n�O�܂ł̃^�C���A�E�g����(�b)
#define MISSION_START_TIMEOUT_SEC    (60 * 3)
///�~�b�V�����J�n�O�܂ł̃^�C���A�E�g���ԂŌx�����b�Z�[�W��\������������(�b)
#define MISSION_START_TIMEOUT_MSG_ON_SEC    (MISSION_START_TIMEOUT_SEC - 10)

///������l�ɂȂ����ꍇ�A�ʐM�I���֑J�ڂ���܂ł̃^�C���A�E�g
#define OTHER_PLAYER_TIMEOUT    (60 * 3)

///�ؒf�����҂��̃^�C���A�E�g
#define COMM_EXIT_WAIT_TIMEOUT     (60*5)

///�q�@�Ƃ��ė����オ�����ꍇ�A���̎��Ԃ��o�߂��Ă��e�@�ɐڑ��o���Ȃ������ꍇ�A�I������
#define PARENT_SEARCH_TIMEOUT     (60 * 10)


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void  IntrudeComm_FinishInitCallback( void* pWork );
static void  IntrudeComm_FinishTermCallback( void* pWork );
static void * IntrudeComm_GetBeaconData(void* pWork);
static void IntrudeComm_CreateBeaconData(GAMEDATA *gamedata, GBS_BEACON *beacon, int max_member_num);
static BOOL IntrudeComm_DiffSendBeacon(GAMEDATA *gamedata, GBS_BEACON *beacon);
static BOOL  IntrudeComm_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2, void* pWork );
static void  IntrudeComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);
static void  IntrudeComm_DisconnectCallBack(void* pWork);
static void IntrudeComm_HardConnect(void* pWork,int hardID);
static void _SetScanBeaconData(WMBssDesc* pBss, void *pWork, u16 level);


//==============================================================================
//  �f�[�^
//==============================================================================
static const GFLNetInitializeStruct aGFLNetInit = {
  Intrude_CommPacketTbl, //NetSamplePacketTbl,  // ��M�֐��e�[�u��
  INTRUDE_CMD_NUM, // ��M�e�[�u���v�f��
  NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL, // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL, // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  IntrudeComm_GetBeaconData,    // �r�[�R���f�[�^�擾�֐�
  GameBeacon_GetBeaconSize,    // �r�[�R���f�[�^�T�C�Y�擾�֐�
  IntrudeComm_CheckConnectService,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  IntrudeComm_ErrorCallBack,    // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂��
  NULL,  //FatalError
  IntrudeComm_DisconnectCallBack, // �ʐM�ؒf���ɌĂ΂��֐�(�I����
  NULL, // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  NULL,   ///< DWC�`���̗F�B���X�g
  NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  0,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
  SYACHI_NETWORK_GGID,  //ggid  
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_NETWORK, //
  GFL_WICON_POSX,GFL_WICON_POSY,  // �ʐM�A�C�R��XY�ʒu
  3,//4,//_MAXNUM,  //�ő�ڑ��l��
  48,//_MAXSIZE,  //�ő呗�M�o�C�g��
  4,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
  TRUE,   // CRC�v�Z
  FALSE,    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIRELESS,    //�ʐM�^�C�v�̎w��
  TRUE,   // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_PALACE_SERVICEID, //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
    0,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
};


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �ʐM�V�X�e��������
 *
 * @param   seq		
 * @param   pwk		
 *
 * @retval  void *		
 */
//==================================================================
void * IntrudeComm_InitCommSystem( int *seq, void *pwk )
{
  INTRUDE_COMM_SYS_PTR intcomm;
  FIELD_INVALID_PARENT_WORK *invalid_parent = pwk;
  GAMEDATA *gamedata = GameCommSys_GetGameData(invalid_parent->game_comm);
  MYSTATUS *myst = GAMEDATA_GetMyStatus(gamedata);
  NetID net_id;
  int i, max_member_num = FIELD_COMM_MEMBER_MAX;

  if(invalid_parent->my_invasion == TRUE && Intrude_CheckTutorialComplete(gamedata) == FALSE){
    max_member_num = 2; //�`���[�g���A�������Ȃ��܂ł͍ő��l�ڑ�
  }
  
  intcomm = GFL_HEAP_AllocClearMemory(HEAPID_APP_CONTROL, sizeof(INTRUDE_COMM_SYS));
  intcomm->game_comm = invalid_parent->game_comm;
	intcomm->comm_status = INTRUDE_COMM_STATUS_INIT_START;
  intcomm->cps = CommPlayer_Init(FIELD_COMM_MEMBER_MAX, invalid_parent->gsys, HEAPID_APP_CONTROL);
  intcomm->member_num = 1;
  intcomm->connect_map_count = 1; //�p���X�}�b�v�ɓ��������_��1�͎����ŘA�����Ă����
  Intrude_InitTalkWork(intcomm, INTRUDE_NETID_NULL);
  MISSION_Init(&intcomm->mission);
  IntrudeComm_CreateBeaconData(gamedata, &intcomm->send_beacon, max_member_num);
  FIELD_WFBC_COMM_DATA_Init(&intcomm->wfbc_comm_data);
  for(i = 0; i < INTRUDE_BCON_PLAYER_PRINT_SEARCH_MAX; i++){
    intcomm->search_child[i] = GFL_STR_CreateBuffer(PERSON_NAME_SIZE+EOM_SIZE, HEAPID_APP_CONTROL);
  }

  GAMEDATA_SetIntrudeMyID(gamedata, 0);
  GAMEDATA_SetIntrudePalaceArea(gamedata, 0);
  GAMEDATA_SetIntrudeNum(gamedata, 1);
  
  //�����Ӑ}���Ȃ��^�C�~���O�ŃA�N�Z�X���Ă���肪�����悤��
  //NetID��MYSTATSU�G���A�ɂ͎�����MyStatus���R�s�[���Ă���
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    MYSTATUS *dest_myst = GAMEDATA_GetMyStatusPlayer(gamedata, net_id);
    MyStatus_Copy(myst, dest_myst);
  }
  
  return intcomm;
}

//==================================================================
/**
 * �ʐM�V�X�e���������҂�
 * @param   pWork		
 * @retval  BOOL		TRUE:�����������@�@FALSE:�������҂�
 */
//==================================================================
BOOL  IntrudeComm_InitCommSystemWait( int *seq, void *pwk, void *pWork )
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  FIELD_INVALID_PARENT_WORK *invalid_parent = pwk;
  GAMEDATA *gamedata = GameCommSys_GetGameData(invalid_parent->game_comm);
  
  if(GAMEDATA_GetIsSave(gamedata) == TRUE){
    return FALSE;
  }
  
  switch(*seq){
  case 0:
    {
      GFLNetInitializeStruct *pNetInitData;
      
      pNetInitData = GFL_HEAP_AllocMemory(
        GFL_HEAP_LOWID(HEAPID_APP_CONTROL), sizeof(GFLNetInitializeStruct));
      *pNetInitData = aGFLNetInit;
      
      if(invalid_parent->my_invasion == TRUE && Intrude_CheckTutorialComplete(gamedata) == FALSE){
        pNetInitData->maxConnectNum = 2; //�`���[�g���A�������Ȃ��܂ł͍ő��l�ڑ�
      }
      GFL_NET_Init( pNetInitData, IntrudeComm_FinishInitCallback, intcomm );
      
      GFL_HEAP_FreeMemory(pNetInitData);
    }
    (*seq)++;
    break;
  case 1:
    if(intcomm->comm_status >= INTRUDE_COMM_STATUS_INIT){
      if(invalid_parent->my_invasion == TRUE){
        OS_TPrintf("�e�Ƃ��ċN��\n");
        GFL_NET_ChangeoverParent(NULL);
        intcomm->comm_status = INTRUDE_COMM_STATUS_BOOT_PARENT;
      }
      else{
        OS_TPrintf("�q�Ƃ��ċN��\n");
        GFL_NET_InitClientAndConnectToParent( invalid_parent->parent_macAddress );
        intcomm->comm_status = INTRUDE_COMM_STATUS_BOOT_CHILD;
      }
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//==================================================================
/**
 * �ʐM�V�X�e���X�V
 *
 * @param   seq		
 * @param   pwk		
 * @param   pWork		
 */
//==================================================================
void  IntrudeComm_UpdateSystem( int *seq, void *pwk, void *pWork )
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  FIELD_INVALID_PARENT_WORK *invalid_parent = pwk;
  GAMEDATA *gamedata = GameCommSys_GetGameData(invalid_parent->game_comm);

  if(intcomm != NULL && intcomm->comm_status >= INTRUDE_COMM_STATUS_UPDATE){
    //�G���[�ł����Ă��Ăё�����K�v������ׁA�ŏ��ɃR�[��
    FIELD_WFBC_COMM_DATA_Oya_Main(
      &intcomm->wfbc_comm_data, GAMEDATA_GetMyWFBCCoreData(gamedata), 
      intcomm->recv_profile);
  }
  
  //�ʐM�G���[�`�F�b�N
  if(NetErr_App_CheckError() || intcomm->error == TRUE){
    intcomm->comm_status = INTRUDE_COMM_STATUS_ERROR;
    intcomm->error = TRUE;
    GameCommSys_ExitReq(intcomm->game_comm);
    return;
  }

  //�X���[�v�ɓ��낤�Ƃ��Ă���ꍇ�͐ؒf
  if(GFL_UI_CheckCoverAndBatt() == TRUE){
    GameCommSys_ExitReq(intcomm->game_comm);
    return;
  }
  
  if(intcomm->comm_status == INTRUDE_COMM_STATUS_UPDATE){
    if(intcomm->exit_recv == TRUE){
      GameCommSys_ExitReq(intcomm->game_comm);
      return;
    }
  }
  
  IntrudeComm_DiffSendBeacon(gamedata, &intcomm->send_beacon);
  
  switch(*seq){
  case 0:
    if(GFL_NET_SystemGetConnectNum() > 1){
      if(intcomm->comm_status < INTRUDE_COMM_STATUS_HARD_CONNECT){
        intcomm->comm_status = INTRUDE_COMM_STATUS_HARD_CONNECT;
        GFL_NET_SetNoChildErrorCheck(TRUE);
      }
    }

    if(intcomm->comm_status == INTRUDE_COMM_STATUS_HARD_CONNECT){
      if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
        GAMEDATA_SetIntrudeMyID(gamedata, GFL_NET_SystemGetCurrentID());
        {//������NetID�̉ӏ��Ɏ�����MyStatus���R�s�[���Ă���
          MYSTATUS *net_myst = GAMEDATA_GetMyStatusPlayer(gamedata, GFL_NET_SystemGetCurrentID());
          MyStatus_Copy(GAMEDATA_GetMyStatus(gamedata), net_myst);
        }
        intcomm->intrude_status_mine.palace_area = GFL_NET_SystemGetCurrentID();
        intcomm->intrude_status_mine.pm_version = GET_VERSION();
        intcomm->intrude_status_mine.season = GAMEDATA_GetSeasonID(gamedata);
        GAMEDATA_SetIntrudePalaceArea(gamedata, intcomm->intrude_status_mine.palace_area);
        if(Intrude_CheckTutorialComplete(gamedata) == FALSE){
          intcomm->intrude_status_mine.tutorial = TRUE;
        }
        OS_TPrintf("�l�S�V�G�[�V�������M\n");
        GFL_NET_SetNoChildErrorCheck(TRUE);
        (*seq)++;
      }
    }
    else if(intcomm->comm_status == INTRUDE_COMM_STATUS_BOOT_CHILD){
      //�q�̏ꍇ�A��莞�Ԍo�߂��Ă��e�Ƃ̐ڑ����m���ł��Ȃ�������ʐM��������߂�
      intcomm->exit_wait++;
      if(intcomm->exit_wait > PARENT_SEARCH_TIMEOUT){
        intcomm->exit_wait = 0;
        GameCommSys_ExitReq(intcomm->game_comm);
        return;
      }
    }
    break;
  case 1:
    if( GFL_NET_HANDLE_IsNegotiation( GFL_NET_HANDLE_GetCurrentHandle() ) == TRUE ){
      intcomm->comm_status = INTRUDE_COMM_STATUS_BASIC_SEND;
      OS_TPrintf("�l�S�V�G�[�V��������\n");
      (*seq)++;
    }
    break;
  case 2: //�ŏ��ɂ���肷��K�v������f�[�^������΂����ŁB
    //WFBC������
    FIELD_WFBC_COMM_DATA_Init(&intcomm->wfbc_comm_data);
    //�����v���t�B�[���������̎�M�o�b�t�@�ɃZ�b�g
    Intrude_SetSendProfileBuffer(intcomm);  //���M�o�b�t�@�Ɍ��݂̃f�[�^���Z�b�g
    Intrude_SetProfile(intcomm, GFL_NET_SystemGetCurrentID(), &intcomm->send_profile);
    (*seq)++;
    break;
  case 3:
    if(IntrudeSend_Profile(intcomm) == TRUE){
      intcomm->comm_status = INTRUDE_COMM_STATUS_UPDATE;
      intcomm->mission_start_timeout = GFL_RTC_GetTimeBySecond();
      (*seq)++;
    }
    break;

  case 4: //�ʏ�X�V
    //�\�t�B�[���h�ɂ��ċG�߂��ς���Ă���ΐؒf����
    if(GAMEDATA_GetIntrudeReverseArea(gamedata) == FALSE 
        && intcomm->intrude_status_mine.season != GAMEDATA_GetSeasonID(gamedata)
        && GAMESYSTEM_CheckFieldMapWork(invalid_parent->gsys) == TRUE){
      OS_TPrintf("�G�߂��ς�������ߐؒf���܂�\n");
      GameCommSys_ExitReq(intcomm->game_comm);
      break;
    }

    Intrude_Main(intcomm);
    
    if(MISSION_CheckResultTimeout(&intcomm->mission) == TRUE){
      OS_TPrintf("�~�b�V�������s�ɂ��ؒf\n");
      GameCommSys_ExitReq(intcomm->game_comm);
      break;
    }
    else if(GFL_NET_IsParentMachine() == TRUE && MISSION_RecvCheck(&intcomm->mission) == FALSE){
      s32 start_time = intcomm->mission_start_timeout;
      s32 now_time = GFL_RTC_GetTimeBySecond();
      if(now_time < start_time){  //��荞�݂�����
        now_time += GFL_RTC_TIME_SECOND_MAX;
      }
      if(now_time - start_time >= MISSION_START_TIMEOUT_SEC){
        OS_TPrintf("�~�b�V�����J�n�O�̃^�C���A�E�g�ɂ��ؒf\n");
        GameCommSys_ExitReq(intcomm->game_comm);
        break;
      }
      else if(now_time - start_time > MISSION_START_TIMEOUT_MSG_ON_SEC
          && intcomm->mission_start_timeout_warning == 0){
        if(IntrudeSend_TimeoutWarning() == TRUE){
          intcomm->mission_start_timeout_warning = TRUE;
        }
      }
    }
    else if(GFL_NET_IsParentMachine() == TRUE && MISSION_CheckSuccessTimeEnd(&intcomm->mission) == TRUE){
      OS_TPrintf("�~�b�V�����������ă^�C���A�E�g�ɂ��ؒf\n");
      GameCommSys_ExitReq(intcomm->game_comm);
    }
    else{
      intcomm->mission_start_timeout_warning = FALSE;
    }
    
    //������l�ɂȂ����ꍇ�A�ʐM�I���֑J�ڂ���܂ł̃^�C���A�E�g
    if(Intrude_OtherPlayerExistence() == FALSE){
      intcomm->other_player_timeout++;
      if(intcomm->other_player_timeout > OTHER_PLAYER_TIMEOUT){
        OS_TPrintf("��O�G���[ int\n");
        GameCommSys_ExitReq(intcomm->game_comm);
        break;
      }
    }
    else{
      intcomm->other_player_timeout = 0;
    }
    
    break;
  }
}

//==================================================================
/**
 * �ʐM�V�X�e���I��
 *
 * @param   seq		
 * @param   pwk		
 * @param   pWork		
 *
 * @retval  BOOL		TRUE:�I�����������@FALSE:�I��������
 */
//==================================================================
BOOL  IntrudeComm_TermCommSystem( int *seq, void *pwk, void *pWork )
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  FIELD_INVALID_PARENT_WORK *invalid_parent = pwk;
  GAMEDATA *gamedata = GameCommSys_GetGameData(invalid_parent->game_comm);
  enum{
    SEQ_INIT,
    SEQ_TIMING_START,
    SEQ_TIMING_WAIT,
    SEQ_EXIT_WAIT,
    SEQ_FINISH,
  };

  if(GAMEDATA_GetIsSave(gamedata) == TRUE){
    return FALSE;
  }

  if(intcomm->error == TRUE || NetErr_App_CheckError()){
    GFL_NET_Exit( IntrudeComm_FinishTermCallback );
    return TRUE;
  }

  switch(*seq){
  case SEQ_INIT:
    intcomm->comm_status = INTRUDE_COMM_STATUS_EXIT_START;
    //������l�Ȃ瑦�I��
    if(GFL_NET_GetConnectNum() <= 1){
      *seq = SEQ_FINISH;
    }
    else{
      if(IntrudeSend_Shutdown(intcomm) == TRUE){
        (*seq)++;
      }
    }
    break;
  case SEQ_TIMING_START:
    intcomm->exit_wait = 0;
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), INTRUDE_TIMING_EXIT, WB_NET_PALACE_SERVICEID);
    (*seq)++;
    break;
  case SEQ_TIMING_WAIT:
    intcomm->exit_wait++;
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), INTRUDE_TIMING_EXIT, WB_NET_PALACE_SERVICEID) == TRUE
        || intcomm->exit_wait > COMM_EXIT_WAIT_TIMEOUT){
      GFL_NET_SetNoChildErrorCheck(FALSE);  //�ؒf����
      OS_TPrintf("�ؒf�������� timeout�c��=%d\n", COMM_EXIT_WAIT_TIMEOUT - intcomm->exit_wait);
      (*seq)++;
    }
    else{
      OS_TPrintf("�ؒf�����҂� timeout�c��=%d\n", COMM_EXIT_WAIT_TIMEOUT - intcomm->exit_wait);
    }
    break;
  case SEQ_EXIT_WAIT:
    intcomm->exit_wait++;
    if(GFL_NET_IsParentMachine() == TRUE){
      //�e�͎�����l�ɂȂ��Ă���I������
      if(GFL_NET_GetConnectNum() <= 1 || intcomm->exit_wait > COMM_EXIT_WAIT_TIMEOUT){
        (*seq)++;
      }
      else{
        OS_TPrintf("�e�F�q�̏I���҂� �c��=%d timeout�c��=%d\n", 
          GFL_NET_GetConnectNum() - 1, COMM_EXIT_WAIT_TIMEOUT - intcomm->exit_wait);
      }
    }
    else{
      (*seq)++;
    }
    break;
  case SEQ_FINISH:
    GFL_NET_Exit( IntrudeComm_FinishTermCallback );
    return TRUE;
  }
  
  return FALSE;
}

//==================================================================
/**
 * �ʐM�V�X�e���I���҂��m�F
 * @param   pWork		
 * @retval  BOOL		TRUE:�I���B�@FALSE:�I���҂�
 */
//==================================================================
BOOL  IntrudeComm_TermCommSystemWait( int *seq, void *pwk, void *pWork )
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  FIELD_INVALID_PARENT_WORK *invalid_parent = pwk;
  GAME_COMM_SYS_PTR game_comm = invalid_parent->game_comm;
  GAMEDATA *gamedata = GameCommSys_GetGameData(game_comm);
  int i;
  BOOL exit_ok = FALSE;
  
  if(GAMEDATA_GetIsSave(gamedata) == TRUE){
    return FALSE;
  }
  
  if(intcomm->comm_status == INTRUDE_COMM_STATUS_EXIT || GFL_NET_IsExit() == TRUE){
    if(GAMEDATA_GetIntrudeReverseArea(gamedata) == FALSE){  //�\�t�B�[���h�ɂ���ꍇ�͑����
      NetErr_ErrWorkInit();
      exit_ok = TRUE;
    }
    else{ //���t�B�[���h�ɂ���ꍇ�̓p���X�֖߂��Ă�����
      if(ZONEDATA_IsPalace( PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(gamedata)) ) == TRUE
          && GAMESYSTEM_CheckFieldMapWork(invalid_parent->gsys) == TRUE){
        exit_ok = TRUE;
      }
    }
  }

  if(exit_ok == TRUE){
    OS_TPrintf("intcomm free\n");
    COMM_PLAYER_SUPPORT_Init(GAMEDATA_GetCommPlayerSupportPtr(gamedata));
    FIELD_WFBC_COMM_DATA_Exit(&intcomm->wfbc_comm_data);
    GAMEDATA_ClearPalaceWFBCCoreData( gamedata );
    //GAMEDATA_SetIntrudeReverseArea(gamedata, FALSE);
    CommPlayer_Exit(invalid_parent->gsys, intcomm->cps);
    
    //�ؒf���鎞�̏�Ԃ�LAST_STATUS�ɃZ�b�g
    if(intcomm->error == TRUE || NetErr_App_CheckError()){
      GameCommSys_SetLastStatus(game_comm, GAME_COMM_LAST_STATUS_INTRUDE_ERROR);
    }
    else if(MISSION_CheckRecvResult(&intcomm->mission) == TRUE){
      if(MISSION_CheckResultMissionMine(intcomm, &intcomm->mission) == TRUE){
        //�������B����
        GameCommSys_SetLastStatus(game_comm, GAME_COMM_LAST_STATUS_INTRUDE_MISSION_SUCCESS);
      }
      else{
        MISSION_RESULT *mresult = MISSION_GetResultData(&intcomm->mission);
        int i;
        
        for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
          if(mresult->achieve_netid[i] != INTRUDE_NETID_NULL){
            break;
          }
        }
        if(i < FIELD_COMM_MEMBER_MAX){
          //���ɒB���҂�����
          GameCommSys_SetLastStatus(game_comm, GAME_COMM_LAST_STATUS_INTRUDE_MISSION_FAIL);
        }
        else{ //�B���҂����Ȃ��̂Ń^�C���A�E�g
          GameCommSys_SetLastStatus(game_comm, GAME_COMM_LAST_STATUS_INTRUDE_MISSION_TIMEOUT);
        }
      }
    }
    else{ //�G���[�ł��Ȃ��~�b�V�����̏I���ł��Ȃ��B�ޏo�ɂ��I��
      if(intcomm->recv_profile > 1){ //�N���ƌq�����Ă�����ԂŏI��
        GameCommSys_SetLastStatus(game_comm, GAME_COMM_LAST_STATUS_INTRUDE_WAYOUT);
      }
      else{  //�N�Ƃ��ڑ����Ă��Ȃ���ԂŏI��
        ;
      }
    }

    if(GAMEDATA_GetIntrudeReverseArea(gamedata) == FALSE){
      GAMEDATA_SetIntrudeMyID(gamedata, 0); //���ɕ\�ɂ���ꍇ�͂��̃^�C�~���O�ŃZ�b�g
      GAMEDATA_SetIntrudePalaceArea(gamedata, 0);
      GAMEDATA_SetIntrudeNum(gamedata, 1);
    }
    
    for(i = 0; i < INTRUDE_BCON_PLAYER_PRINT_SEARCH_MAX; i++){
      GFL_STR_DeleteBuffer(intcomm->search_child[i]);
    }
    GFL_HEAP_FreeMemory(intcomm);
    GFL_HEAP_FreeMemory(pwk);
    if((intcomm->error == TRUE || NetErr_App_CheckError()) 
        && GAMEDATA_GetIntrudeReverseArea(gamedata) == TRUE){
      GAMESYSTEM_SetFieldCommErrorReq(invalid_parent->gsys, TRUE);
    }
    
    GameCommInfo_QueAllClear(game_comm);
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �r�[�R�����X�L�����������ɌĂ΂��R�[���o�b�N�֐�
 *
 * @param   pBss		
 */
//--------------------------------------------------------------
static void _SetScanBeaconData(WMBssDesc* pBss, void *pWork, u16 level)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  GBS_BEACON *bcon_buff;
  GameServiceID id;
  
  if(intcomm->search_count >= INTRUDE_BCON_PLAYER_PRINT_SEARCH_MAX || GFL_NET_GetConnectNum() > 1
      || intcomm->error == TRUE || NetErr_App_CheckError()){
    return;
  }
  
  if(WH_BeaconFirstCheck(pBss) == FALSE){
    return;
  }
  if(GFL_NET_WL_scanCheck(pBss, WL_SCAN_CHK_BIT_ALL ^ WL_SCAN_CHK_BIT_BEACON_COMP) == FALSE){
    return;
  }

  bcon_buff = GFL_NET_WLGetDirectGFBss(pBss, &id);

  if(id == WB_NET_FIELDMOVE_SERVICEID && bcon_buff->pad_detect_fold == FALSE){
    int i;
    for(i = 0; i < intcomm->search_count; i++){
      if(intcomm->search_child_trainer_id[i] == bcon_buff->trainer_id){
        return;
      }
    }

    if(Intrude_CheckZonePalaceConnect(bcon_buff->info.zone_id) == FALSE){
      return;
    }
    GAMEBEACON_Get_PlayerNameToBuf(
      &bcon_buff->info, intcomm->search_child[intcomm->search_count]);
	  intcomm->search_child_sex[intcomm->search_count] = GAMEBEACON_Get_Sex(&bcon_buff->info);
	  intcomm->search_child_lang[intcomm->search_count] = GAMEBEACON_Get_PmLanguage(&bcon_buff->info);
	  
  	intcomm->search_child_trainer_id[intcomm->search_count] = bcon_buff->trainer_id;
  	intcomm->search_count++;
  }
}

//==================================================================
/**
 * �t�B�[���h�쐬���̃R�[���o�b�N
 *
 * @param   pwk		
 * @param   app_work		
 * @param   fieldWork		
 */
//==================================================================
void IntrudeComm_FieldCreate(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork)
{
  INTRUDE_COMM_SYS_PTR intcomm = app_work;
  FIELD_INVALID_PARENT_WORK *invalid_parent = pwk;
  GAMEDATA *gamedata;
  int net_id, my_net_id;
  
  if(intcomm == NULL){
    return;
  }
  
  gamedata = GameCommSys_GetGameData(invalid_parent->game_comm);
  my_net_id = GAMEDATA_GetIntrudeMyID(gamedata);
  
  CommPlayer_Pop(intcomm->cps);
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(net_id == my_net_id){
      continue;
    }
    if(CommPlayer_CheckOcc(intcomm->cps, net_id) == TRUE){
      intcomm->intrude_status[net_id].player_pack.vanish 
        = intcomm->intrude_status[net_id].force_vanish;
      CommPlayer_SetParam(intcomm->cps, net_id, &intcomm->intrude_status[net_id].player_pack);
    }
  }
  
  //�}�b�v�؂�ւ��ŒʐM�v���C���[�̈ʒu���f�̒��߁AStatus�̍X�V���s��
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(net_id != my_net_id && (intcomm->recv_profile & (1 << net_id))){
      Intrude_SetPlayerStatus(intcomm, net_id, &intcomm->intrude_status[net_id], FALSE);
    }
  }
}

//==================================================================
/**
 * �t�B�[���h�폜���̃R�[���o�b�N
 *
 * @param   pwk		
 * @param   app_work		
 * @param   fieldWork		
 */
//==================================================================
void IntrudeComm_FieldDelete(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork)
{
  INTRUDE_COMM_SYS_PTR intcomm = app_work;
  
  if(intcomm == NULL){
    return;
  }
  
//  intcomm->connect_map_count = 0;
  CommPlayer_Push(intcomm->cps);
}

//--------------------------------------------------------------
/**
 * �ʐM���C�u���������������R�[���o�b�N
 *
 * @param   pWork		
 */
//--------------------------------------------------------------
static void  IntrudeComm_FinishInitCallback( void* pWork )
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  
	intcomm->comm_status = INTRUDE_COMM_STATUS_INIT;
  OS_TPrintf("�����������R�[���o�b�N\n");
  WHSetWIHDWCBeaconGetFunc(_SetScanBeaconData);
}

//--------------------------------------------------------------
/**
 * �ʐM���C�u�����I�������R�[���o�b�N
 *
 * @param   pWork		
 */
//--------------------------------------------------------------
static void  IntrudeComm_FinishTermCallback( void* pWork )
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  
  intcomm->comm_status = INTRUDE_COMM_STATUS_EXIT;
  OS_TPrintf("�ؒf�����R�[���o�b�N\n");
}

//--------------------------------------------------------------
/**
 * ���M����r�[�R���f�[�^�擾
 *
 * @param   pWork		
 *
 * @retval  void *		�r�[�R���f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void * IntrudeComm_GetBeaconData(void* pWork)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;

  return &intcomm->send_beacon;
}

//--------------------------------------------------------------
/**
 * ���M����r�[�R���f�[�^���쐬����
 *
 * @param   beacon		
 * @param   max_member_num    �ڑ��o����ő�l��(-1�̏ꍇ�͒l�X�V���Ȃ�)
 */
//--------------------------------------------------------------
static void IntrudeComm_CreateBeaconData(GAMEDATA *gamedata, GBS_BEACON *beacon, int max_member_num)
{
  
  beacon->gsid = WB_NET_PALACE_SERVICEID;
  beacon->member_num = GFL_NET_GetConnectNum();
  if(max_member_num != -1){
    beacon->member_max = max_member_num;
  }
  beacon->error = GFL_NET_SystemGetErrorCode();
  beacon->beacon_type = GBS_BEACONN_TYPE_PALACE;
  MyStatus_Copy(GAMEDATA_GetMyStatus(gamedata), &beacon->intrude_myst);
}

//--------------------------------------------------------------
/**
 * ���M����r�[�R���f�[�^�ɍX�V������΃r�[�R�����M�f�[�^�ύX���s��
 *
 * @param   beacon		
 *
 * @retval  BOOL		
 */
//--------------------------------------------------------------
static BOOL IntrudeComm_DiffSendBeacon(GAMEDATA *gamedata, GBS_BEACON *beacon)
{
  int member_num, error;
  
  if(beacon->member_num != GFL_NET_GetConnectNum()
      || beacon->error != GFL_NET_SystemGetErrorCode()){
    IntrudeComm_CreateBeaconData(gamedata, beacon, -1);
    NET_WHPIPE_BeaconSetInfo();
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
 *
 * @param   GameServiceID1		
 * @param   GameServiceID2		
 *
 * @retval  BOOL		TRUE:�ڑ�OK  FALSE:�ڑ�NG
 */
//--------------------------------------------------------------
static BOOL  IntrudeComm_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2, void* pWork )
{
//  OS_TPrintf("GameServiceID1 = %d, GameServiceID2 = %d\n", GameServiceID1, GameServiceID2);
  return (GameServiceID1==GameServiceID2);
}

//--------------------------------------------------------------
/**
 * �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
 *
 * @param   pNet		
 * @param   errNo		
 * @param   pWork		
 */
//--------------------------------------------------------------
static void  IntrudeComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;

//  NetErr_ErrorSet();
//  intcomm->comm_status = INTRUDE_COMM_STATUS_ERROR;
  OS_TPrintf("intrude comm �G���[�I errNo = %d\n");
  intcomm->error = TRUE;
}

//--------------------------------------------------------------
/**
 * �ʐM�ؒf���ɌĂ΂��֐�(�I����
 *
 * @param   pWork		
 */
//--------------------------------------------------------------
static void  IntrudeComm_DisconnectCallBack(void* pWork)
{
  OS_TPrintf("�ؒf�R�[���o�b�N�Ă΂ꂽ\n");
}

