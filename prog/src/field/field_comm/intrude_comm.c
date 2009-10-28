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
#include "intrude_types.h"
#include "intrude_comm_command.h"
#include "palace_sys.h"
#include "intrude_main.h"
#include "savedata/mystatus.h"
#include "bingo_system.h"
#include "intrude_field.h"
#include "intrude_mission.h"


//--------------------------------------------------------------
//  �f�o�b�O�p�ϐ��錾
//--------------------------------------------------------------
#ifdef PM_DEBUG
u8 debug_palace_comm_seq = 0;
#endif

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void  IntrudeComm_FinishInitCallback( void* pWork );
static void  IntrudeComm_FinishTermCallback( void* pWork );
static void * IntrudeComm_GetBeaconData(void* pWork);
static void IntrudeComm_CreateBeaconData(GBS_BEACON *beacon);
static BOOL  IntrudeComm_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 );
static void  IntrudeComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);
static void  IntrudeComm_DisconnectCallBack(void* pWork);
static void IntrudeComm_HardConnect(void* pWork,int hardID);


//==============================================================================
//  �f�[�^
//==============================================================================
static const GFLNetInitializeStruct aGFLNetInit = {
  Intrude_CommPacketTbl, //NetSamplePacketTbl,  // ��M�֐��e�[�u��
  INTRUDE_CMD_NUM, // ��M�e�[�u���v�f��
  IntrudeComm_HardConnect,    ///< �n�[�h�Őڑ��������ɌĂ΂��
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
  0x222,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_NETWORK, //
  GFL_WICON_POSX,GFL_WICON_POSY,  // �ʐM�A�C�R��XY�ʒu
  4,//_MAXNUM,  //�ő�ڑ��l��
  48,//_MAXSIZE,  //�ő呗�M�o�C�g��
  4,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
  TRUE,   // CRC�v�Z
  FALSE,    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIRELESS,    //�ʐM�^�C�v�̎w��
  TRUE,   // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_FIELDMOVE_SERVICEID, //GameServiceID
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
  
  intcomm = GFL_HEAP_AllocClearMemory(HEAPID_APP_CONTROL, sizeof(INTRUDE_COMM_SYS));
  intcomm->game_comm = invalid_parent->game_comm;
	intcomm->comm_status = INTRUDE_COMM_STATUS_INIT_START;
  intcomm->palace = PALACE_SYS_Alloc(HEAPID_APP_CONTROL, intcomm->game_comm);
  intcomm->cps = CommPlayer_Init(FIELD_COMM_MEMBER_MAX, invalid_parent->gsys, HEAPID_APP_CONTROL);
  intcomm->member_num = 1;
  Intrude_InitTalkWork(intcomm, INTRUDE_NETID_NULL);
  Bingo_InitBingoSystem(Bingo_GetBingoSystemWork(intcomm));
  MISSION_Init(&intcomm->mission);

  GAMEDATA_SetIntrudeMyID(gamedata, 0);

  GFL_NET_Init( &aGFLNetInit, IntrudeComm_FinishInitCallback, intcomm );
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
  
  switch(*seq){
  case 0:
    if(intcomm->comm_status >= INTRUDE_COMM_STATUS_INIT){
      if(invalid_parent->my_invasion == TRUE){
        OS_TPrintf("�e�Ƃ��ċN��\n");
        GFL_NET_ChangeoverConnect(NULL);
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

  //�ʐM�G���[�`�F�b�N
  if(NetErr_App_CheckError() == TRUE){
    intcomm->comm_status = INTRUDE_COMM_STATUS_ERROR;
    GameCommSys_ExitReq(intcomm->game_comm);
    return;
  }
  
  switch(*seq){
  case 0:
    //��check �n�[�h�ڑ��R�[���o�b�N���Ă΂�Ȃ��Ƃ�������̂ŁA
    //        �ڑ��l���������Ă����瓯�l�̂��Ƃ�����b�菈��
    if(GFL_NET_SystemGetConnectNum() > 1){
      if(intcomm->comm_status < INTRUDE_COMM_STATUS_HARD_CONNECT){
        intcomm->comm_status = INTRUDE_COMM_STATUS_HARD_CONNECT;
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
        OS_TPrintf("�l�S�V�G�[�V�������M\n");
        (*seq)++;
      }
    }
    else if(intcomm->comm_status == INTRUDE_COMM_STATUS_BOOT_CHILD){
      //��check�@�����Ɏq�̏ꍇ�A��莞�Ԍo�߂��Ă��e�Ƃ̐ڑ����m���ł��Ȃ�������
      //         �ʐM��������߂鏈��������
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
    //�����v���t�B�[���������̎�M�o�b�t�@�ɃZ�b�g
    Intrude_SetSendProfileBuffer(intcomm);  //���M�o�b�t�@�Ɍ��݂̃f�[�^���Z�b�g
    Intrude_SetProfile(intcomm, GFL_NET_SystemGetCurrentID(), &intcomm->send_profile);
    IntrudeSend_Profile(intcomm);           //���M
    intcomm->comm_status = INTRUDE_COMM_STATUS_UPDATE;
    (*seq)++;
    break;

  case 3: //�ʏ�X�V
    Intrude_Main(intcomm);
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

  if(NetErr_App_CheckError() == TRUE){
    return TRUE;
  }

  switch(*seq){
  case 0:
    intcomm->comm_status = INTRUDE_COMM_STATUS_EXIT_START;
    if(1){  //GFL_NET_IsParentMachine() == TRUE){
      if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), INTRUDE_CMD_SHUTDOWN, 0, NULL) == TRUE){
        (*seq)++;
      }
    }
    else{
      (*seq)++;
    }
    break;
  case 1:
    GFL_NET_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle() , INTRUDE_TIMING_EXIT );
    (*seq)++;
    break;
  case 2:
    if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), INTRUDE_TIMING_EXIT) == TRUE){
      OS_TPrintf("�ؒf��������\n");
      (*seq)++;
    }
    else{
      OS_TPrintf("�ؒf�����҂�\n");
    }
    break;
  case 3:
    if(GFL_NET_IsParentMachine() == TRUE){
      if(GFL_NET_GetConnectNum() <= 1){ //�e�͎�����l�ɂȂ��Ă���I������
        (*seq)++;
      }
      else{
        OS_TPrintf("�e�F�q�̏I���҂� �c��=%d\n", GFL_NET_GetConnectNum() - 1);
      }
    }
    else{
      (*seq)++;
    }
    break;
  case 4:
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

  switch(*seq){
  case 0:
    if(intcomm->comm_status == INTRUDE_COMM_STATUS_EXIT || NetErr_App_CheckError() == TRUE){
      CommPlayer_Exit(intcomm->cps);
      PALACE_SYS_Free(intcomm->palace);
      GFL_HEAP_FreeMemory(intcomm);
      GFL_HEAP_FreeMemory(pwk);
      if(NetErr_App_CheckError() == TRUE){
        GAMESYSTEM_SetFieldCommErrorReq(invalid_parent->gsys, TRUE);
      }
      return TRUE;
    }
    break;
  }
  return FALSE;
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
  
  IntrudeField_ConnectMap(fieldWork, invalid_parent->gsys, intcomm);
  CommPlayer_Pop(intcomm->cps);
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
  
  intcomm->connect_map_count = 0;
  CommPlayer_Push(intcomm->cps);
  PALACE_DEBUG_DeleteNumberAct(intcomm->palace);
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

  IntrudeComm_CreateBeaconData(&intcomm->send_beacon);
  return &intcomm->send_beacon;
}

//--------------------------------------------------------------
/**
 * ���M����r�[�R���f�[�^���쐬����
 *
 * @param   beacon		
 */
//--------------------------------------------------------------
static void IntrudeComm_CreateBeaconData(GBS_BEACON *beacon)
{
  beacon->gsid = WB_NET_FIELDMOVE_SERVICEID;
  beacon->member_num = GFL_NET_GetConnectNum();
  beacon->member_max = FIELD_COMM_MEMBER_MAX;
  beacon->error = GFL_NET_SystemGetErrorCode();
  beacon->beacon_type = GBS_BEACONN_TYPE_PLACE;
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
static BOOL  IntrudeComm_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 )
{
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
  OS_TPrintf("intrude comm �G���[�I\n");
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

//--------------------------------------------------------------
/**
 * �n�[�h�ڑ����ɌĂ΂��R�[���o�b�N
 *
 * @param   pWork		
 * @param   hardID		
 */
//--------------------------------------------------------------
static void IntrudeComm_HardConnect(void* pWork,int hardID)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  
  OS_TPrintf("HardConnect Callback hardID=%d\n", hardID);
  if(intcomm->comm_status < INTRUDE_COMM_STATUS_HARD_CONNECT){
    intcomm->comm_status = INTRUDE_COMM_STATUS_HARD_CONNECT;
  }
}

