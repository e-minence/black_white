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


//==============================================================================
//  �萔��`
//==============================================================================
///HEAPID_UNION�Ŋm�ۂ���q�[�v�T�C�Y
#define HEAP_SIZE_UNION               (0x6000)

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
static void UnionComm_BeaconSearch(UNION_SYSTEM_PTR unisys);
static BOOL UnionBeacon_SetReceiveData(UNION_SYSTEM_PTR unisys, const UNION_BEACON *beacon, const u8 *beacon_mac_address);
static void* UnionComm_GetBeaconData(void* pWork);
static void UnionComm_SetBeaconParam(UNION_SYSTEM_PTR unisys, UNION_BEACON *beacon);
static int UnionComm_GetBeaconSize(void *pWork);
static BOOL UnionComm_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 );
static void UnionComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);
static void UnionComm_DisconnectCallBack(void* pWork);


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
	TRUE,		// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	WB_NET_UNION,	//GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
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
  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UNION, HEAP_SIZE_UNION );
  unisys = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(UNION_SYSTEM));
  unisys->uniparent = uniparent;
  UnionMyComm_Init(&unisys->my_situation.mycomm);
  
  GFL_OVERLAY_Load( FS_OVERLAY_ID( union_room ) );
  
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
 * �����������R�[���o�b�N
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
 * �I�������R�[���o�b�N
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
 * �X�V����
 *
 * @param   pWork		UNION_SYSTEM_PTR
 */
//==================================================================
void UnionComm_Update(int *seq, void *pwk, void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  UnionComm_BeaconSearch(unisys);   //�r�[�R���T�[�`
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
  
  if(situ->calling_pc != NULL){
    GFL_STD_MemCopy(situ->calling_pc->mac_address, beacon->connect_mac_address, 6);
  }
  
  beacon->pm_version = PM_VERSION;
  beacon->language = PM_LANG;
  beacon->union_status = situ->union_status;
  beacon->appeal_no = situ->appeal_no;
  
  MyStatus_CopyNameStrCode(unisys->uniparent->mystatus, beacon->name, PERSON_NAME_SIZE + EOM_SIZE);
  
  beacon->trainer_view = MyStatus_GetTrainerView( unisys->uniparent->mystatus );
  beacon->sex = MyStatus_GetMySex(unisys->uniparent->mystatus);
  
  
  //���M�f�[�^����
  beacon->data_valid = UNION_BEACON_VALID;
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

  NetErr_ErrorSet();
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
      situ->calling_pc = calling_pc;
    }
    break;
  case UNION_MYSITU_PARAM_IDX_ANSWER_PC:
    {
      UNION_BEACON_PC *answer_pc = work;
      situ->answer_pc = answer_pc;
    }
    break;
  case UNION_MYSITU_PARAM_IDX_CONNECT_PC:
    {
      UNION_BEACON_PC *connect_pc = work;
      situ->connect_pc = connect_pc;
    }
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
  UnionMyComm_Init(&situ->mycomm);
}

//==================================================================
/**
 * UNION_MY_COMM�\���̂�������
 *
 * @param   mycomm		
 */
//==================================================================
void UnionMyComm_Init(UNION_MY_COMM *mycomm)
{
  GFL_STD_MemClear(mycomm, sizeof(UNION_MY_COMM));
  mycomm->mainmenu_select = UNION_MSG_MENU_SELECT_NULL;
  mycomm->submenu_select = UNION_MSG_MENU_SELECT_NULL;
  mycomm->mainmenu_yesno_result = 0xff;
}
