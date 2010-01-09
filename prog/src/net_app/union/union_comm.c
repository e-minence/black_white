//==============================================================================
/**
 * @file    union_comm.c
 * @brief   ���j�I�����[���F�ʐM����
 * @author  matsuda
 * @date    2009.07.02(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "union_types.h"
#include "union_local.h"
#include "savedata/mystatus.h"
#include "net_app/union/union_main.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "union_comm_command.h"
#include "union_msg.h"
#include "comm_player.h"
#include "colosseum_comm_command.h"
#include "colosseum_tool.h"
#include "union_subproc.h"
#include "union_chat.h"
#include "net/net_whpipe.h"


//==============================================================================
//  �萔��`
//==============================================================================
///HEAPID_UNION�Ŋm�ۂ���q�[�v�T�C�Y
#define HEAP_SIZE_UNION               (0x8a00)  //(0x9800)  //(0xa000)

///�ʐM�X�e�[�^�X
enum{
  UNION_COMM_STATUS_NULL,         ///<�������삵�Ă��Ȃ�
  UNION_COMM_STATUS_INIT_START,   ///<����������
  UNION_COMM_STATUS_INIT,         ///<����������
  UNION_COMM_STATUS_UPDATE,       ///<���C���X�V���s��
  UNION_COMM_STATUS_EXIT_START,   ///<�I�������J�n
  UNION_COMM_STATUS_EXIT,         ///<�I����������
};


//--------------------------------------------------------------
//  �I�[�o�[���CID
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(union_room);


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void UnionComm_InitCallback(void *pWork);
static void	UnionComm_ExitCallback(void* pWork);
static void UnionComm_MinigameUpdate(UNION_SYSTEM_PTR unisys);
static void UnionComm_Colosseum_Update(UNION_SYSTEM_PTR unisys);
static void UnionComm_BeaconSearch(UNION_SYSTEM_PTR unisys);
static BOOL UnionBeacon_SetReceiveData(UNION_SYSTEM_PTR unisys, const UNION_BEACON *beacon, const u8 *beacon_mac_address);
static void* UnionComm_GetBeaconData(void* pWork);
static void UnionComm_SetBeaconParam(UNION_SYSTEM_PTR unisys, UNION_BEACON *beacon);
static int UnionComm_GetBeaconSize(void *pWork);
static BOOL UnionComm_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 );
static void UnionComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);
static void UnionComm_DisconnectCallBack(void* pWork);
static void UnionChat_InitLog(UNION_CHAT_LOG *log);


//==============================================================================
//  �f�[�^
//==============================================================================
static const GFLNetInitializeStruct aGFLNetInit = {
	Union_CommPacketTbl,  // ��M�֐��e�[�u��
	UNION_CMD_NUM,  			// ��M�e�[�u���v�f��
	NULL,		///< �n�[�h�Őڑ��������ɌĂ΂��
	NULL,		///< �l�S�V�G�[�V�����������ɃR�[��
	NULL,		// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
	NULL,		// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
	UnionComm_GetBeaconData,		// �r�[�R���f�[�^�擾�֐�  
	UnionComm_GetBeaconSize,		// �r�[�R���f�[�^�T�C�Y�擾�֐� 
	UnionComm_CheckConnectService,		// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
	UnionComm_ErrorCallBack,		// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂��
	NULL,		//FatalError
	UnionComm_DisconnectCallBack,		// �ʐM�ؒf���ɌĂ΂��֐�(�I����
	NULL,		// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
	NULL,		///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
	NULL,		///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
	NULL,		///< wifi�t�����h���X�g�폜�R�[���o�b�N
	NULL,		///< DWC�`���̗F�B���X�g	
	NULL,		///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
	0,			///< DWC�ւ�HEAP�T�C�Y
	TRUE,		///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
	0x122,		//ggid  DP=0x333,RANGER=0x178,WII=0x346
	GFL_HEAPID_APP,		//���ɂȂ�heapid
	HEAPID_NETWORK + HEAPDIR_MASK,		//�ʐM�p��create�����HEAPID
	HEAPID_WIFI + HEAPDIR_MASK,		//wifi�p��create�����HEAPID
	HEAPID_NETWORK + HEAPDIR_MASK,		//
	GFL_WICON_POSX,GFL_WICON_POSY,	// �ʐM�A�C�R��XY�ʒu
	UNION_CONNECT_PLAYER_NUM,      //_MAXNUM,	//�ő�ڑ��l��
	48,     //_MAXSIZE,	//�ő呗�M�o�C�g��
	UNION_RECEIVE_BEACON_MAX,  // �ő�r�[�R�����W��
	TRUE,		// CRC�v�Z
	FALSE,		// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
	GFL_NET_TYPE_WIRELESS,		//�ʐM�^�C�v�̎w��
	FALSE,		// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	WB_NET_UNION,	//GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
    0,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
};


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ���j�I���V�X�e��������
 *
 * @retval  UNION_SYSTEM_PTR		�����������j�I���V�X�e�����[�N�ւ̃|�C���^
 */
//==================================================================
static UNION_SYSTEM_PTR Union_InitSystem(UNION_PARENT_WORK *uniparent)
{
  UNION_SYSTEM_PTR unisys;
  
  GFL_HEAP_CreateHeap( HEAPID_PROC, HEAPID_UNION, HEAP_SIZE_UNION );
  unisys = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(UNION_SYSTEM));
  unisys->uniparent = uniparent;
  UnionMySituation_Clear(unisys);
  unisys->my_situation.chat_upload = TRUE;
  UnionChat_InitLog(&unisys->chat_log);
  unisys->alloc.regulation = Regulation_AllocWork(HEAPID_UNION);
  
  GFL_OVERLAY_Load( FS_OVERLAY_ID( union_room ) );
  unisys->overlay_load = TRUE;
  
  return unisys;
}

//==================================================================
/**
 * ���j�I���V�X�e���I��
 *
 * @param   unisys		���j�I���V�X�e�����[�N�ւ̃|�C���^
 */
//==================================================================
static void Union_ExitSystem(UNION_SYSTEM_PTR unisys)
{
  if(unisys->alloc.regulation != NULL){
    GFL_HEAP_FreeMemory(unisys->alloc.regulation);
  }
  GFL_HEAP_FreeMemory(unisys);
  GFL_HEAP_DeleteHeap(HEAPID_UNION);
  
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( union_room ) );
}



//==============================================================================
//  game_comm
//==============================================================================
//==================================================================
/**
 * ����������
 *
 * @retval  UNION_SYSTEM_PTR		���j�I���V�X�e���ւ̃|�C���^
 */
//==================================================================
void * UnionComm_Init(int *seq, void *pwk)
{
	UNION_SYSTEM_PTR unisys;
	
	unisys = Union_InitSystem(pwk);
	
	unisys->comm_status = UNION_COMM_STATUS_INIT_START;
	GFL_NET_Init(&aGFLNetInit, UnionComm_InitCallback, unisys);

	return unisys;
}

//==================================================================
/**
 * �����������m�F
 *
 * @param   pWork		
 *
 * @retval  BOOL		TRUE:����������     FALSE:�����������҂�
 */
//==================================================================
BOOL UnionComm_InitWait(int *seq, void *pwk, void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;

  if(unisys->comm_status >= UNION_COMM_STATUS_INIT){
    GF_ASSERT(unisys->comm_status < UNION_COMM_STATUS_UPDATE);
    unisys->comm_status = UNION_COMM_STATUS_UPDATE;
//    GFL_NET_StartBeaconScan();
    GFL_NET_Changeover(NULL);
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �����������R�[���o�b�N�@���ؒf���ĊJ�̃R�[���o�b�N�Ƃ��Ă��g�p����̂Œ��g�̕ύX�ɂ͒���
 *
 * @param   pWork		UNION_SYSTEM_PTR
 */
//--------------------------------------------------------------
static void UnionComm_InitCallback(void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  unisys->comm_status = UNION_COMM_STATUS_INIT;
}

//==================================================================
/**
 * �I������
 *
 * @param   unisys		
 */
//==================================================================
BOOL UnionComm_Exit(int *seq, void *pwk, void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  unisys->finish_exe = TRUE;
  
  GF_ASSERT(unisys->comm_status == UNION_COMM_STATUS_UPDATE);
  if(unisys->comm_status < UNION_COMM_STATUS_UPDATE){
    return FALSE; //�ʏ킠�肦�Ȃ����ǔO�̂���Update�ɂȂ�܂ő҂悤�ɂ���
  }
  
  unisys->comm_status = UNION_COMM_STATUS_EXIT_START;
	GFL_NET_Exit(UnionComm_ExitCallback);
	return TRUE;
}

//==================================================================
/**
 * �I���҂�
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:�I�������B�@FALSE:�I���҂���
 */
//==================================================================
BOOL UnionComm_ExitWait(int *seq, void *pwk, void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;

  if(unisys->comm_status == UNION_COMM_STATUS_EXIT){
    Union_ExitSystem(unisys);
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �I�������R�[���o�b�N ���ؒf���ĊJ�̃R�[���o�b�N�Ƃ��Ă��g�p����̂Œ��g�̕ύX�ɂ͒���
 *
 * @param   pWork		UNION_SYSTEM_PTR
 */
//--------------------------------------------------------------
static void	UnionComm_ExitCallback(void* pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  OS_TPrintf("ExitCallback\n");
  unisys->comm_status = UNION_COMM_STATUS_EXIT;
}

//==================================================================
/**
 * �ؒf���ĊJ���N�G�X�g
 *
 * @param   unisys		
 *
 * �ڑ����Ă���l�Ƃ̒ʐM���I�����A�����̃r�[�R������M��Ԃɖ߂�
 */
//==================================================================
void UnionComm_Req_ShutdownRestarts(UNION_SYSTEM_PTR unisys)
{
  if(unisys->restart_seq != 0){
    GF_ASSERT(0); //���Ƀ��X�^�[�g�������s���Ă���
    return;
  }
  unisys->restart_seq = 1;  //LOCALSEQ_MODE_CHECK
  unisys->restart_mode = UNION_RESTART_SHUTDOWN_RESTARTS;
  OS_TPrintf("�ؒf���ĊJ���N�G�X�g���󂯕t���܂���\n");
}

//==================================================================
/**
 * �ؒf���N�G�X�g(UnionComm_Req_ShutdownRestarts��ؒf�܂ł̏����Ŋ���)
 *
 * @param   unisys		
 *
 * �ڑ����Ă���l�Ƃ̒ʐM���I������
 */
//==================================================================
void UnionComm_Req_Shutdown(UNION_SYSTEM_PTR unisys)
{
  if(unisys->restart_seq != 0){
    GF_ASSERT(0); //���Ƀ��X�^�[�g�������s���Ă���
    return;
  }
  unisys->restart_seq = 1;  //LOCALSEQ_MODE_CHECK
  unisys->restart_mode = UNION_RESTART_SHUTDOWN;
  OS_TPrintf("�ؒf���N�G�X�g���󂯕t���܂���\n");
}

//==================================================================
/**
 * �ĊJ���N�G�X�g(UnionComm_Req_Shutdown�Őؒf������Ɏg�p����)
 *
 * @param   unisys		
 *
 * �r�[�R������M���J�n����
 */
//==================================================================
void UnionComm_Req_Restarts(UNION_SYSTEM_PTR unisys)
{
  if(unisys->restart_seq != 0){
    GF_ASSERT(0); //���Ƀ��X�^�[�g�������s���Ă���
    return;
  }
  unisys->restart_seq = 1;  //LOCALSEQ_MODE_CHECK
  unisys->restart_mode = UNION_RESTART_RESTARTS;
  OS_TPrintf("�ĊJ���N�G�X�g���󂯕t���܂���\n");
}

//==================================================================
/**
 * �u�ؒf���ĊJ�v�������s���Ă��邩���ׂ�
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:���s���@FALSE:�������Ă��Ȃ�
 */
//==================================================================
BOOL UnionComm_Check_ShutdownRestarts(UNION_SYSTEM_PTR unisys)
{
  if(unisys->restart_seq == 0){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �ؒf���ĊJ����
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:�u�ؒf���ĊJ�����v���s��
 */
//--------------------------------------------------------------
static BOOL UnionComm_ShutdownRestarts(UNION_SYSTEM_PTR unisys)
{
  enum{
    LOCALSEQ_NULL,
    LOCALSEQ_MODE_CHECK,
    LOCALSEQ_SHUTDOWN,
    LOCALSEQ_SHUTDOWN_WAIT,
    LOCALSEQ_RESTART,
    LOCALSEQ_RESTART_WAIT,
  };
  
  switch(unisys->restart_seq){
  case LOCALSEQ_NULL:
    if(unisys->restart_mode == UNION_RESTART_SHUTDOWN){
      return TRUE;  //�ؒf�݂̂ŏI��点�Ă����ԂȂ̂ŒʐM�͎n�܂��Ă��Ȃ��BTRUE��Ԃ�
    }
    return FALSE;
  case LOCALSEQ_MODE_CHECK:
    switch(unisys->restart_mode){
    case UNION_RESTART_SHUTDOWN_RESTARTS:
    case UNION_RESTART_SHUTDOWN:
      unisys->restart_seq = LOCALSEQ_SHUTDOWN;
      break;
    case UNION_RESTART_RESTARTS:
      unisys->restart_seq = LOCALSEQ_RESTART;
      break;
    default:
      unisys->restart_seq = 0;
      GF_ASSERT(0);
      break;
    }
    break;
  case LOCALSEQ_SHUTDOWN:
    OS_TPrintf("restart �ؒf���Đڑ��J�n\n");
    unisys->comm_status = UNION_COMM_STATUS_EXIT_START;
  	GFL_NET_Exit(UnionComm_ExitCallback);
    unisys->restart_seq++;
    break;
  case LOCALSEQ_SHUTDOWN_WAIT:
    if(unisys->comm_status == UNION_COMM_STATUS_EXIT){
      OS_TPrintf("restart �ؒf����\n");
      if(unisys->restart_mode == UNION_RESTART_SHUTDOWN){
        unisys->restart_seq = LOCALSEQ_NULL;
      }
      else{
        unisys->restart_seq++;
      }
    }
    break;
  case LOCALSEQ_RESTART:
    OS_TPrintf("restart �������J�n\n");
  	unisys->comm_status = UNION_COMM_STATUS_INIT_START;
  	GFL_NET_Init(&aGFLNetInit, UnionComm_InitCallback, unisys);
    unisys->restart_seq++;
    break;
  case LOCALSEQ_RESTART_WAIT:
    if(unisys->comm_status >= UNION_COMM_STATUS_INIT){
      OS_TPrintf("restart ����������\n");
      unisys->comm_status = UNION_COMM_STATUS_UPDATE;
      GFL_NET_Changeover(NULL);
      unisys->restart_seq = LOCALSEQ_NULL;
    }
    break;
  }
  
  return TRUE;
}

//==================================================================
/**
 * �X�V����
 *
 * @param   pWork		UNION_SYSTEM_PTR
 */
//==================================================================
void UnionComm_Update(int *seq, void *pwk, void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  if(UnionComm_ShutdownRestarts(unisys) == TRUE){
    return; //�ؒf���ĊJ �̏��������s��
  }
  
  if(unisys->send_beacon_update == TRUE){
    GFI_NET_BeaconSetInfo();
    unisys->send_beacon_update = FALSE;
  }

  UnionComm_BeaconSearch(unisys);   //�r�[�R���T�[�`
  
  UnionComm_Colosseum_Update(unisys); //�R���V�A���X�V
  
  UnionComm_MinigameUpdate(unisys); //�~�j�Q�[���X�V
}

//--------------------------------------------------------------
/**
 * �~�j�Q�[���X�V����
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionComm_MinigameUpdate(UNION_SYSTEM_PTR unisys)
{
  if(GFL_NET_GetConnectNum() > 1){
    //������]NG�̕Ԏ�������Ȃ瑗�M
    if(unisys->minigame_entry_req_answer_ng_bit > 0){
      if(UnionSend_MinigameEntryReqAnswerNG(unisys->minigame_entry_req_answer_ng_bit) == TRUE){
        unisys->minigame_entry_req_answer_ng_bit = 0;
      }
    }
    //������]OK�̕Ԏ�������Ȃ瑗�M
    if(unisys->minigame_entry_req_answer_ok_bit > 0){
      if(UnionSend_MinigameEntryReqAnswerOK(unisys->minigame_entry_req_answer_ok_bit) == TRUE){
        unisys->minigame_entry_req_answer_ok_bit = 0;
      }
    }
  }
  else{
    unisys->minigame_entry_req_answer_ng_bit = 0;
    unisys->minigame_entry_req_answer_ok_bit = 0;
  }
}

//--------------------------------------------------------------
/**
 * �R���V�A���̍X�V����
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionComm_Colosseum_Update(UNION_SYSTEM_PTR unisys)
{
  if(unisys->colosseum_sys == NULL || unisys->colosseum_sys->comm_ready == FALSE){
    return;
  }
  
  //PROC���t�B�[���h�̂Ƃ��̂ݓ��삷�鏈��
  if(Union_FieldCheck(unisys) == TRUE){
    if(UnionSubProc_IsExits(unisys) == FALSE){  //�T�uPROC���s���łȂ����Ɏ��s���鏈��
      //�ʐM�v���C���[�A�N�^�[�Ǘ��V�X�e������������Ă���Ȃ玩�����W�̓]�����s��
      CommPlayer_Update(unisys->colosseum_sys->cps);
      if(CommPlayer_Mine_DataUpdate(
          unisys->colosseum_sys->cps, &unisys->colosseum_sys->send_mine_package) == TRUE){
        ColosseumSend_PosPackage(&unisys->colosseum_sys->send_mine_package);
      }
    }
    
    //�ʐM�v���C���[�̍��W���f
    ColosseumTool_CommPlayerUpdate(unisys->colosseum_sys);
  }

  //�����ʒu�ւ̕Ԏ����N�G�X�g�����܂��Ă���Ȃ瑗�M���s��
  Colosseum_Parent_SendAnswerStandingPosition(unisys->colosseum_sys);
  //�퓬���������̕Ԏ����N�G�X�g�����܂��Ă���Ȃ瑗�M���s��
  Colosseum_Parent_BattleReadyAnswer(unisys->colosseum_sys);
}

//--------------------------------------------------------------
/**
 * �r�[�R���T�[�`
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionComm_BeaconSearch(UNION_SYSTEM_PTR unisys)
{
  int i;
  UNION_BEACON *bcon_buff;
  const u8 *mac_address_ptr;
  
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
  	bcon_buff = GFL_NET_GetBeaconData(i);
  	if(bcon_buff != NULL){
      mac_address_ptr = GFL_NET_GetBeaconMacAddress(i);
      if(mac_address_ptr != NULL){
        UnionBeacon_SetReceiveData(unisys, bcon_buff, mac_address_ptr);
      }
  	}
  }
}

//--------------------------------------------------------------
/**
 * ��M�r�[�R������M�p�o�b�t�@�ɃZ�b�g����
 *
 * @param   unisys		
 * @param   beacon		
 *
 * @retval  BOOL		TRUE:�����B�@FALSE:���s
 */
//--------------------------------------------------------------
static BOOL UnionBeacon_SetReceiveData(UNION_SYSTEM_PTR unisys, const UNION_BEACON *beacon, const u8 *beacon_mac_address)
{
  int i;
  UNION_BEACON_PC *dest;
  
  dest = unisys->receive_beacon;
  
  if(beacon->data_valid != UNION_BEACON_VALID){
    return FALSE; //�L���ȃr�[�R���ł͂Ȃ�
  }
  
  //���Ɏ�M�ς݂̃r�[�R���f�[�^���m�F
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
    if(dest[i].beacon.data_valid == UNION_BEACON_VALID
        && GFL_STD_MemComp(beacon_mac_address, dest[i].mac_address, 6) == 0){
      GFL_STD_MemCopy(beacon, &dest[i].beacon, sizeof(UNION_BEACON));
      //��x�ɓ����}�V������̘A����M�̉\�����l��new�̏ꍇ�̓t���O��ς��Ȃ�
      if(dest[i].update_flag != UNION_BEACON_RECEIVE_NEW){
        dest[i].update_flag = UNION_BEACON_RECEIVE_UPDATE;
      }
      dest[i].life = UNION_CHAR_LIFE;   //�r�[�R���̍X�V���������̂Ŏ��������ɖ߂�
//      OS_TPrintf("life�� %d\n", OS_GetVBlankCount());
      return TRUE;
    }
  }

  //�V�K�̃r�[�R���f�[�^�Ȃ̂ŋ󂫗̈�ɃZ�b�g����
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
    if(dest[i].beacon.data_valid != UNION_BEACON_VALID){
      GFL_STD_MemCopy(beacon, &dest[i].beacon, sizeof(UNION_BEACON));
      GFL_STD_MemCopy(beacon_mac_address, dest[i].mac_address, 6);
      dest[i].buffer_no = i;
      dest[i].update_flag = UNION_BEACON_RECEIVE_NEW;
      dest[i].life = UNION_CHAR_LIFE;
      return TRUE;
    }
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �r�[�R���f�[�^�擾�֐�
 *
 * @param   pWork		UNION_SYSTEM_PTR
 *
 * @retval  void*		�r�[�R���f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void* UnionComm_GetBeaconData(void* pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  UnionComm_SetBeaconParam(unisys, &unisys->my_beacon);
  return &unisys->my_beacon;
}

//==================================================================
/**
 * ���M����r�[�R���f�[�^���쐬����
 *
 * @param   beacon		�r�[�R���p�����[�^�����
 */
//==================================================================
static void UnionComm_SetBeaconParam(UNION_SYSTEM_PTR unisys, UNION_BEACON *beacon)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  
  GFL_STD_MemClear(beacon, sizeof(UNION_BEACON));
  
  if(situ->mycomm.connect_pc != NULL){
    if(GFL_NET_IsParentMachine() == TRUE){
      OS_GetMacAddress(beacon->connect_mac_address);
    }
    else{
      GFL_STD_MemCopy(situ->mycomm.connect_pc->mac_address, beacon->connect_mac_address, 6);
    }
  }
  else if(situ->mycomm.calling_pc != NULL){
    GFL_STD_MemCopy(situ->mycomm.calling_pc->mac_address, beacon->connect_mac_address, 6);
  }
  
  beacon->connect_num = GFL_NET_GetConnectNum();
  beacon->pm_version = PM_VERSION;
  beacon->language = PM_LANG;
  beacon->union_status = situ->union_status;
  beacon->appeal_no = situ->appeal_no;
  beacon->play_category = situ->play_category;
  
  MyStatus_CopyNameStrCode(unisys->uniparent->mystatus, beacon->name, PERSON_NAME_SIZE + EOM_SIZE);
  
  beacon->trainer_view = MyStatus_GetTrainerView( unisys->uniparent->mystatus );
  beacon->sex = MyStatus_GetMySex(unisys->uniparent->mystatus);

  beacon->pmsdata = situ->chat_pmsdata;
  beacon->pms_rand = situ->chat_pms_rand;
  
  beacon->party = situ->mycomm.party;
  
  //���M�f�[�^����
  beacon->data_valid = UNION_BEACON_VALID;
  unisys->send_beacon_update = TRUE;
}

//--------------------------------------------------------------
/**
 * �r�[�R���f�[�^�T�C�Y�擾�֐�
 *
 * @param   pWork		UNION_SYSTEM_PTR
 *
 * @retval  int		�r�[�R���f�[�^�T�C�Y
 */
//--------------------------------------------------------------
static int UnionComm_GetBeaconSize(void *pWork)
{
  return sizeof(UNION_BEACON);
}

//--------------------------------------------------------------
/**
 * �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
 *
 * @param   GameServiceID1		�T�[�r�XID�P
 * @param   GameServiceID2		�T�[�r�XID�Q
 *
 * @retval  BOOL		TRUE:�ڑ�OK�@�@FALSE:�ڑ�NG
 */
//--------------------------------------------------------------
static BOOL UnionComm_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 )
{
  return (GameServiceID1==GameServiceID2);
}

//--------------------------------------------------------------
/**
 * �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
 *
 * @param   pNet		�l�b�g�n���h��
 * @param   errNo		�G���[�ԍ�
 * @param   pWork		UNION_SYSTEM_PTR
 */
//--------------------------------------------------------------
static void UnionComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
	UNION_SYSTEM_PTR unisys = pWork;

//  NetErr_ErrorSet();
}

//--------------------------------------------------------------
/**
 * �ʐM�ؒf���ɌĂ΂��֐�(�I����
 *
 * @param   pWork		UNION_SYSTEM_PTR
 */
//--------------------------------------------------------------
static void UnionComm_DisconnectCallBack(void* pWork)
{
  OS_TPrintf("�ʐM�ؒf�R�[���o�b�N UnionComm_DisconnectCallBack\n");
}

//==================================================================
/**
 * �r�[�R����M�o�b�t�@All�N���A
 *
 * @param   unisys		
 */
//==================================================================
void UnionBeacon_ClearAllReceiveData(UNION_SYSTEM_PTR unisys)
{
  OS_TPrintf("�r�[�R����M�o�b�t�@�S�N���A\n");
  GFL_STD_MemClear(unisys->receive_beacon, sizeof(UNION_BEACON_PC) * UNION_RECEIVE_BEACON_MAX);
}

//==============================================================================
//  �ʐM���N�G�X�g
//==============================================================================
//==================================================================
/**
 * �����V�`���G�[�V�����̃Z�b�g
 *
 * @param   unisys		
 * @param   index		  �V�`���G�[�V�����p�����[�^�̃f�[�^Index
 * @param   work		  �eIndex�ŕK�v�Ƃ��郏�[�N�ւ̃|�C���^
 */
//==================================================================
void UnionMySituation_SetParam(UNION_SYSTEM_PTR unisys, UNION_MYSITU_PARAM_IDX index, void *work)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  switch(index){
  case UNION_MYSITU_PARAM_IDX_CALLING_PC:
    {
      UNION_BEACON_PC *calling_pc = work;
      situ->mycomm.calling_pc = calling_pc;
    }
    break;
  case UNION_MYSITU_PARAM_IDX_ANSWER_PC:
    {
      UNION_BEACON_PC *answer_pc = work;
      situ->mycomm.answer_pc = answer_pc;
    }
    break;
  case UNION_MYSITU_PARAM_IDX_CONNECT_PC:
    {
      UNION_BEACON_PC *connect_pc = work;
      situ->mycomm.connect_pc = connect_pc;
    }
    break;
  case UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY:
    situ->play_category = (u32)work;
    break;
  }
}

//==================================================================
/**
 * �����V�`���G�[�V�����̃N���A
 *
 * @param   unisys		
 */
//==================================================================
void UnionMySituation_Clear(UNION_SYSTEM_PTR unisys)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  
  GFL_STD_MemClear(situ, sizeof(UNION_MY_SITUATION));
  PMSDAT_SetupDefaultUnionMessage(&situ->chat_pmsdata);
  situ->chat_pms_rand = GFUser_GetPublicRand(0xffff);
  UnionMyComm_Init(unisys, &situ->mycomm);
}

//==================================================================
/**
 * UNION_MY_COMM�\���̂̒����烁�j���[�̕Ԏ��֘A�̎�M�o�b�t�@�݂̂�������
 *
 * @param   mycomm		
 */
//==================================================================
void UnionMyComm_InitMenuParam(UNION_MY_COMM *mycomm)
{
  mycomm->mainmenu_select = UNION_MENU_SELECT_NULL;
  mycomm->submenu_select = UNION_MENU_SELECT_NULL;
  mycomm->mainmenu_yesno_result = 0xff;
}

//==================================================================
/**
 * UNION_MY_COMM�\���̂�������
 *
 * @param   mycomm		
 */
//==================================================================
void UnionMyComm_Init(UNION_SYSTEM_PTR unisys, UNION_MY_COMM *mycomm)
{
  GFL_STD_MemClear(mycomm, sizeof(UNION_MY_COMM));
  UnionMyComm_InitMenuParam(mycomm);
  
  //UNION_MEMBER�̎������𖄂߂�
  {
    UNION_MEMBER *member = &mycomm->party.member[0];
    
    OS_GetMacAddress(member->mac_address);
    member->trainer_view = MyStatus_GetTrainerView( unisys->uniparent->mystatus );
    member->sex = MyStatus_GetMySex(unisys->uniparent->mystatus);
    member->occ = TRUE;
  }
}

//==================================================================
/**
 * ���j�I���p�[�e�B�Ƀv���C���[��o�^����@���p�����[�^�w��
 *
 * @param   mycomm		      
 * @param   mac_address		  
 * @param   trainer_view		
 * @param   sex		
 *
 * @retval  BOOL		
 */
//==================================================================
BOOL UnionMyComm_PartyAddParam(UNION_MY_COMM *mycomm, const u8 *mac_address, u8 trainer_view, u8 sex)
{
  int i;
  UNION_MEMBER *member;
  
  for(i = 0; i < UNION_CONNECT_PLAYER_NUM; i++){
    member = &mycomm->party.member[i];
    if(member->occ == TRUE && GFL_STD_MemComp(mac_address, member->mac_address, 6) == 0){
      return i;   //���ɓo�^�ς�
    }
  }
  
  for(i = 0; i < UNION_CONNECT_PLAYER_NUM; i++){
    member = &mycomm->party.member[i];
    if(member->occ == FALSE){
      GFL_STD_MemCopy(mac_address, member->mac_address, 6);
      member->trainer_view = trainer_view;
      member->sex = sex;
      member->occ = TRUE;
      return i;
    }
  }
  
  GF_ASSERT(0); //���Ƀ����o�[�����܂��Ă���
  return 0;
}

//==================================================================
/**
 * ���j�I���p�[�e�B�Ƀv���C���[��o�^����   ��UNION_BEACON_PC�w��
 *
 * @param   unisys		
 * @param   pc		    �o�^�Ώۂ�PC�ւ̃|�C���^
 *
 * @retval  TRUE:�o�^�����@�@FALSE:���s
 */
//==================================================================
BOOL UnionMyComm_PartyAdd(UNION_MY_COMM *mycomm, const UNION_BEACON_PC *pc)
{
  return UnionMyComm_PartyAddParam(mycomm, pc->mac_address, pc->beacon.trainer_view, pc->beacon.sex);
}

//==================================================================
/**
 * ���j�I���p�[�e�B����v���C���[���폜����
 *
 * @param   unisys		
 * @param   pc		    �폜�Ώۂ�PC�ւ̃|�C���^
 */
//==================================================================
void UnionMyComm_PartyDel(UNION_MY_COMM *mycomm, const UNION_BEACON_PC *pc)
{
  int i;
  UNION_MEMBER *member;
  
  for(i = 0; i < UNION_CONNECT_PLAYER_NUM - 1; i++){
    member = &mycomm->party.member[i];
    if(member->occ == TRUE && GFL_STD_MemComp(pc->mac_address, member->mac_address, 6) == 0){
      GFL_STD_MemClear(member, sizeof(UNION_MEMBER));
      OS_TPrintf("�����o�[�폜 %d\n", i);
      return;
    }
  }
  
  GF_ASSERT(0); //�p�[�e�B�ɑ��݂��Ă��Ȃ�PC
}

//--------------------------------------------------------------
/**
 * �`���b�g���O�Ǘ��f�[�^��������
 *
 * @param   log		�`���b�g���O�Ǘ��f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void UnionChat_InitLog(UNION_CHAT_LOG *log)
{
  int i;
  
  GFL_STD_MemClear(log, sizeof(UNION_CHAT_LOG));
  for(i = 0; i < UNION_CHAT_LOG_MAX; i++){
    GFL_STD_MemFill(log->chat[i].mac_address, 0xff, 6);
  }
  log->chat_log_count = -1;
  log->chat_view_no = -1;
  log->old_chat_view_no = -1;
}

//==================================================================
/**
 * �����̃`���b�g�f�[�^���Z�b�g����
 *
 * @param   unisys		
 * @param   pmsdata		
 */
//==================================================================
void UnionChat_SetMyPmsData(UNION_SYSTEM_PTR unisys, const PMS_DATA *pmsdata)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  situ->chat_pmsdata = *pmsdata;
  situ->chat_pms_rand++;
  situ->chat_upload = TRUE;
}
