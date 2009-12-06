//==============================================================================
/**
 * @file    game_beacon_search.c
 * @brief   �ʐM�Q�[�������Ă���r�[�R�����T�[�`����
 * @author  matsuda
 * @date    2009.05.11(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "net/network_define.h"
#include "net/wih_dwc.h"
#include "system/net_err.h"
#include "gamesystem/gamesystem.h"

#include "field/game_beacon_search.h"
#include "gamesystem/game_comm.h"
#include "field/field_beacon_message.h"
#include "field/intrude_comm.h"


//==============================================================================
//  �萔��`
//==============================================================================
///���݂̃X�e�[�^�X
typedef enum{
  GBS_STATUS_NULL,    ///<�������삵�Ă��Ȃ�
  GBS_STATUS_INIT,    ///<����������
  GBS_STATUS_UPDATE,  ///<���C���������s��
}GBS_STATUS;

//==============================================================================
//  �\���̒�`
//==============================================================================
///�q�b�g�����r�[�R���̎�����̏��
typedef struct{
  GameServiceID gsid; ///<Game Service ID
  u8 member_num;      ///<���݂̎Q���l��
  u8 member_max;      ///<�ő�l��
  u8 macAddress[6];
}GBS_TARGET_INFO;

///�ʐM�Q�[���r�[�R���T�[�`�V�X�e�����[�N�\����
typedef struct _GAME_BEACON_SYS
{
  GBS_STATUS status;        ///<TRUE:�������ς�
  BOOL fatal_err;   ///<TRUE:�G���[����
  GBS_BEACON beacon_send_data;    ///<���M�r�[�R���f�[�^
  GBS_TARGET_INFO target_info;    ///<�r�[�R���Ō���������̏��
  
  FIELD_BEACON_MSG_SYS *fbmSys;
  WIH_DWC_WORK* pWDWork;
  
}GAME_BEACON_SYS;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void GameBeacon_InitCallback(void *pWork);
static void	GameBeacon_ExitCallback(void* pWork);
static GBS_TARGET_INFO * GameBeacon_UpdateBeacon(GAME_BEACON_SYS_PTR gbs);
static GBS_BEACON * GameBeacon_BeaconSearch(GAME_BEACON_SYS_PTR gbs, int *hit_index);
static void* GameBeacon_GetBeaconData(void* pWork);
static BOOL GameBeacon_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 );
static void GameBeacon_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);
static void GameBeacon_DisconnectCallBack(void* pWork);
static const GBS_BEACON * GameBeacon_CompareBeacon( const GBS_BEACON *beacon_a , const GBS_BEACON *beacon_b );
static void GameBeacon_SetBeaconParam(GBS_BEACON *beacon);


//==============================================================================
//  �f�[�^
//==============================================================================
///
static const GFLNetInitializeStruct aGFLNetInit = {
	NULL,		//NetSamplePacketTbl,  // ��M�֐��e�[�u��
	0,			// ��M�e�[�u���v�f��
	NULL,		///< �n�[�h�Őڑ��������ɌĂ΂��
	NULL,		///< �l�S�V�G�[�V�����������ɃR�[��
	NULL,		// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
	NULL,		// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
	GameBeacon_GetBeaconData,		// �r�[�R���f�[�^�擾�֐�  
	GameBeacon_GetBeaconSize,		// �r�[�R���f�[�^�T�C�Y�擾�֐� 
	GameBeacon_CheckConnectService,		// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
	GameBeacon_ErrorCallBack,		// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂��
	NULL,		//FatalError
	GameBeacon_DisconnectCallBack,		// �ʐM�ؒf���ɌĂ΂��֐�(�I����
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
	0x222,		//ggid  DP=0x333,RANGER=0x178,WII=0x346
	GFL_HEAPID_APP | HEAPDIR_MASK,		//���ɂȂ�heapid
	HEAPID_NETWORK + HEAPDIR_MASK,		//�ʐM�p��create�����HEAPID
	HEAPID_WIFI + HEAPDIR_MASK,		//wifi�p��create�����HEAPID
	HEAPID_NETWORK + HEAPDIR_MASK,		//
	GFL_WICON_POSX,GFL_WICON_POSY,	// �ʐM�A�C�R��XY�ʒu
	4,//_MAXNUM,	//�ő�ڑ��l��
	48,//_MAXSIZE,	//�ő呗�M�o�C�g��
	4,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
	TRUE,		// CRC�v�Z
	FALSE,		// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
	GFL_NET_TYPE_WIRELESS_SCANONLY,		//�ʐM�^�C�v�̎w��
	FALSE,		// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	WB_NET_FIELDMOVE_SERVICEID,	//GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
    0,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
};



//==================================================================
/**
 * ����������
 *
 * @retval  GAME_BEACON_SYS_PTR		�Q�[���r�[�R���V�X�e���ւ̃|�C���^
 */
//==================================================================
void * GameBeacon_Init(int *seq, void *pwk)
{
	GAME_BEACON_SYS_PTR gbs;
  GAMESYS_WORK *gsys = pwk;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
	
	gbs = GFL_HEAP_AllocClearMemory(GFL_HEAP_LOWID(HEAPID_PROC), sizeof(GAME_BEACON_SYS));
	gbs->fbmSys = FIELD_BEACON_MSG_InitSystem( GAMEDATA_GetFieldBeaconMessageData(gamedata) , GFL_HEAP_LOWID(HEAPID_PROC) );
	return gbs;
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
BOOL GameBeacon_InitWait(int *seq, void *pwk, void *pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;
  GAMESYS_WORK *gsys = pwk;

  switch(*seq){
  case 0:
    if(GAMESYSTEM_CheckFieldMapWork(gsys) == TRUE){
    	GFL_HEAP_DEBUG_PrintExistMemoryBlocks(HEAPID_NETWORK_FIX);
    	GFL_NET_Init(&aGFLNetInit, GameBeacon_InitCallback, gbs);
    	(*seq)++;
    }
  	break;
  case 1:
    if(gbs->status >= GBS_STATUS_INIT){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �����������R�[���o�b�N
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//--------------------------------------------------------------
static void GameBeacon_InitCallback(void *pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;
  
  gbs->status = GBS_STATUS_INIT;
  //�S���̃r�[�R����T�m����
  gbs->pWDWork = WIH_DWC_AllBeaconStart(aGFLNetInit.maxBeaconNum, GFL_HEAP_LOWID(HEAPID_PROC));
}

//==================================================================
/**
 * �I������
 *
 * @param   gbs		
 *
 * �����̊֐��ďo����A�K��GameBeacon_ExitWait�ŏI���҂��̊m�F�����Ă�������
 */
//==================================================================
BOOL GameBeacon_Exit(int *seq, void *pwk, void *pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;
  
  OS_TPrintf("GameBeaconExit\n");
  WIH_DWC_AllBeaconEnd(gbs->pWDWork);
	GFL_NET_Exit(GameBeacon_ExitCallback);
	return TRUE;
}

//==================================================================
/**
 * �I���҂�
 *
 * @param   gbs		
 *
 * @retval  BOOL		TRUE:�I�������B�@FALSE:�I���҂���
 */
//==================================================================
BOOL GameBeacon_ExitWait(int *seq, void *pwk, void *pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;

  if(gbs->status == GBS_STATUS_NULL){
    FIELD_BEACON_MSG_ExitSystem( gbs->fbmSys );
    GFL_HEAP_FreeMemory(gbs);
    OS_TPrintf("GameBeaconWait...Finish\n");
    return TRUE;
  }
  OS_TPrintf("GameBeaconWait...\n");
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �I�������R�[���o�b�N
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//--------------------------------------------------------------
static void	GameBeacon_ExitCallback(void* pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;
  
  OS_TPrintf("GameBeacon:ExitCallback\n");
  gbs->status = GBS_STATUS_NULL;
}

//==================================================================
/**
 * �X�V����
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//==================================================================
void GameBeacon_Update(int *seq, void *pwk, void *pWork)
{
  GAMESYS_WORK *gsys = pwk;
  GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAME_BEACON_SYS_PTR gbs = pWork;
  GBS_TARGET_INFO *target;
  
  target = GameBeacon_UpdateBeacon(gbs);
#ifdef PM_DEBUG
  if(target != NULL && GameCommSys_GetCommGameNo(gcsp) != GAME_COMM_NO_DEBUG_SCANONLY){
#else
  if(target != NULL){ //�Ώۂ���������
#endif
    switch(target->gsid){
    case WB_NET_FIELDMOVE_SERVICEID:  //�N��(�p���X)
      {
        FIELD_INVALID_PARENT_WORK *invalid_parent;
        int i;
        
        invalid_parent = GFL_HEAP_AllocClearMemory(
            GFL_HEAP_LOWID(HEAPID_PROC), sizeof(FIELD_INVALID_PARENT_WORK));
        invalid_parent->gsys = gsys;
        invalid_parent->game_comm = gcsp;
        for(i = 0; i < 6; i++){
          invalid_parent->parent_macAddress[i] = target->macAddress[i];
        }
        GameCommSys_ChangeReq(gcsp, GAME_COMM_NO_INVASION, invalid_parent);
        OS_TPrintf("�p���X�e�����������ׁA�ʐM��N���ɐ؂�ւ��܂�\n");
      }
      break;
    default:
      OS_TPrintf("�t�B�[���h����q��gsid�ł͖����ׁA���� gsid = %d\n", target->gsid);
      break;
    }
  }
  WIH_DWC_MainLoopScanBeaconData();
}

//--------------------------------------------------------------
/**
 * �Q�[���r�[�R���F�A�b�v�f�[�g
 * 
 *
 * @param   gbs		
 *
 * @retval  �Ώۂ��������Ă���΃^�[�Q�b�g���̃|�C���^���Ԃ�
 * @retval  �������Ă��Ȃ��ꍇ�ANULL
 */
//--------------------------------------------------------------
static GBS_TARGET_INFO * GameBeacon_UpdateBeacon(GAME_BEACON_SYS_PTR gbs)
{
  GBS_BEACON *beacon;
  GBS_TARGET_INFO * target = &gbs->target_info;
  
  if(gbs->status == GBS_STATUS_NULL){
    return NULL;
  }
  if(gbs->status == GBS_STATUS_INIT){
    GFL_NET_StartBeaconScan();
    gbs->status = GBS_STATUS_UPDATE;
    return NULL;
  }
  
  //�r�[�R���T�[�`
  if(target->gsid == 0){ //�N�������Ă��Ȃ��ꍇ�ɃT�[�`������
    int hit_index;
    
    beacon = GameBeacon_BeaconSearch(gbs, &hit_index);
    if(beacon != NULL){
      if( beacon->beacon_type == GBS_BEACONN_TYPE_PLACE ){
        int i;
        u8 *mac_address;
        
        GFL_STD_MemClear(target, sizeof(GBS_TARGET_INFO));
        target->gsid = beacon->gsid;
        target->member_num = beacon->member_num;
        target->member_max = beacon->member_max;
        mac_address = GFL_NET_GetBeaconMacAddress(hit_index);
        for(i = 0; i < 6; i++){
          target->macAddress[i] = mac_address[i];
        }
      }
    }
  }
  
  if(target->gsid != 0){
    //OS_TPrintf("beacon target ���� gsid = %d\n", target->gsid);
    return target;
  }
  return NULL;
}

//--------------------------------------------------------------
/**
 * �r�[�R���T�[�`
 *
 * @param   gbs		
 * @param   hit_index       ���������ꍇ�A�r�[�R���o�b�t�@index����������
 *
 * @retval  GBS_BEACON *		���������r�[�R���f�[�^�ւ̃|�C���^(���������ꍇ��NULL)
 */
//--------------------------------------------------------------
static GBS_BEACON * GameBeacon_BeaconSearch(GAME_BEACON_SYS_PTR gbs, int *hit_index)
{
  int i;
  GBS_BEACON *bcon_buff;
  int target_index = -1;
  
  for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++){
  	bcon_buff = GFL_NET_GetBeaconData(i);
  	if(bcon_buff != NULL )
  	{
      FIELD_BEACON_MSG_CheckBeacon( gbs->fbmSys , bcon_buff , GFL_NET_GetBeaconMacAddress(i) );
    	if(bcon_buff->member_num <= bcon_buff->member_max){
    		//OS_TPrintf("�r�[�R����M�@%d�� gsid = %d\n", i, bcon_buff->gsid);
    		if(target_index == -1){
          target_index = i;
        }
        else{
          if(GameBeacon_CompareBeacon(bcon_buff, GFL_NET_GetBeaconData(target_index)) == bcon_buff){
            target_index = i;
          }
        }
      }
  	}
  }
  
  if(target_index != -1){
    *hit_index = target_index;
    return GFL_NET_GetBeaconData(target_index);
  }
  return NULL;
}

//--------------------------------------------------------------
/**
 * �r�[�R���f�[�^�擾�֐�
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 *
 * @retval  void*		�r�[�R���f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void* GameBeacon_GetBeaconData(void* pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;
  
  GameBeacon_SetBeaconParam(&gbs->beacon_send_data);
  FIELD_BEACON_MSG_SetBeaconMessage( gbs->fbmSys , &gbs->beacon_send_data );
  return &gbs->beacon_send_data;
}

//==================================================================
/**
 * ���M����r�[�R���f�[�^���쐬����
 *
 * @param   beacon		�r�[�R���p�����[�^�����
 */
//==================================================================
static void GameBeacon_SetBeaconParam(GBS_BEACON *beacon)
{
  GFL_STD_MemClear(beacon, sizeof(GBS_BEACON));
  beacon->gsid = aGFLNetInit.gsid;
  beacon->member_num = GFL_NET_GetConnectNum();
  beacon->member_max = 4;
  beacon->error = GFL_NET_SystemGetErrorCode();
  beacon->beacon_type = GBS_BEACONN_TYPE_NONE;
}

//--------------------------------------------------------------
/**
 * �r�[�R���f�[�^�T�C�Y�擾�֐�
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 *
 * @retval  int		�r�[�R���f�[�^�T�C�Y
 */
//--------------------------------------------------------------
int GameBeacon_GetBeaconSize(void *pWork)
{
  return sizeof(GBS_BEACON);
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
static BOOL GameBeacon_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 )
{
  return (GameServiceID1==GameServiceID2);
}

//--------------------------------------------------------------
/**
 * �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
 *
 * @param   pNet		�l�b�g�n���h��
 * @param   errNo		�G���[�ԍ�
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//--------------------------------------------------------------
static void GameBeacon_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
	GAME_BEACON_SYS_PTR gbs = pWork;

//  NetErr_ErrorSet();
  gbs->fatal_err = TRUE;
}

//--------------------------------------------------------------
/**
 * �ʐM�ؒf���ɌĂ΂��֐�(�I����
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//--------------------------------------------------------------
static void GameBeacon_DisconnectCallBack(void* pWork)
{
  OS_TPrintf("GameBeacon:�ʐM�ؒf�R�[���o�b�N GameBeacon_DisconnectCallBack\n");
}

//--------------------------------------------------------------
/**
 * �r�[�R����r(�l���������A�d�g�󋵂Ȃǂ��������ėD�悷��r�[�R����Ԃ�)
 *
 * @param   beacon_a		��r�r�[�R��A
 * @param   beacon_b		��r�r�[�R��B
 *
 * @retval  const GBS_BEACON *		�D�悷��r�[�R���̃|�C���^��Ԃ�
 */
//--------------------------------------------------------------
static const GBS_BEACON * GameBeacon_CompareBeacon( const GBS_BEACON *beacon_a , const GBS_BEACON *beacon_b )
{
  //��check�@�d�g�󋵂̉����͌�œ����
  
  if(beacon_a->error > 0){
    return beacon_b;
  }
  if(beacon_a->member_num >= beacon_b->member_num){
    return beacon_a;  //�l������������D��
  }
  return beacon_b;
}


