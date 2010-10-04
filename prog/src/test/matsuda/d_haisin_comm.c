//==============================================================================
/**
 * @file    d_haisin_comm.c
 * @brief   �ȒP�Ȑ���������
 * @author  matsuda
 * @date    2010.08.18(��)
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
#include "field/intrude_comm.h"
#include "net/net_whpipe.h"
#include "savedata/mystatus.h"

#include "field/fieldmap.h"
#include "field/fieldmap_call.h"
#include "field/field_comm/intrude_work.h"
#include "fieldmap/zone_id.h"

#include "d_haisin_comm.h"



//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void GameBeacon_InitCallback(void *pWork);
static void	GameBeacon_ExitCallback(void* pWork);
static void* GameBeacon_GetBeaconData(void* pWork);
static BOOL GameBeacon_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2, void* pWork );
static void GameBeacon_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);
static void GameBeacon_SetBeaconParam(GBS_BEACON *beacon);
static int GameBeacon_GetBeaconSize(void *pWork);


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
	NULL,		// �ʐM�ؒf���ɌĂ΂��֐�(�I����
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
  SYACHI_NETWORK_GGID,  //ggid  
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
	GFL_NET_TYPE_WIRELESS,  //GFL_NET_TYPE_WIRELESS_SCANONLY,		//�ʐM�^�C�v�̎w��
	FALSE,		// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	WB_NET_FIELDMOVE_SERVICEID,	//GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
    0,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
};


typedef struct{
  GBS_BEACON beacon_send_data;
  int seq;
  int g_power_id;
  int counter;
  s32 beacon_space_time;
}HAISIN_WORK;

static HAISIN_WORK HaisinWork;


//==================================================================
/**
 * ����������
 */
//==================================================================
void HaisinBeacon_Init(int g_power_id, s32 beacon_space_time)
{
  GFL_STD_MemClear(&HaisinWork, sizeof(HAISIN_WORK));
  HaisinWork.g_power_id = g_power_id;
  HaisinWork.beacon_space_time = beacon_space_time * 60;
  
  GAMEBEACON_Set_DistributionGPower(g_power_id);
  GameBeacon_SetBeaconParam(&HaisinWork.beacon_send_data);
  GAMEBEACON_SendDataCopy(&HaisinWork.beacon_send_data.info);
	GFL_NET_Init(&aGFLNetInit, NULL, NULL);
}

BOOL HaisinBeacon_InitWait(void)
{
  switch(HaisinWork.seq){
  case 0:
    if(GFL_NET_IsInit() == TRUE){
      HaisinWork.seq++;
    }
    break;
  case 1:
    GFL_NET_InitServer();
    //GFL_NET_Changeover(NULL);
    HaisinWork.seq++;
    return TRUE;
  }
  return FALSE;
}

void HaisinBeacon_Exit(void)
{
  GFL_NET_Exit(NULL);
}

BOOL HaisinBeacon_ExitWait(void)
{
  if(GFL_NET_IsExit() == TRUE){
    return TRUE;
  }
  return FALSE;
}

void HaisinBeacon_Update(void)
{
  HaisinWork.counter++;
  if(HaisinWork.counter % HaisinWork.beacon_space_time == 0){
    HaisinWork.counter = 0;
    GAMEBEACON_Set_DistributionGPower(HaisinWork.g_power_id);
    if(GFL_NET_IsInit() == TRUE){
      NET_WHPIPE_BeaconSetInfo();
    }
  }
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
  GameBeacon_SetBeaconParam(&HaisinWork.beacon_send_data);
  GAMEBEACON_SendDataCopy(&HaisinWork.beacon_send_data.info);
  HaisinWork.beacon_send_data.beacon_type = GBS_BEACONN_TYPE_INFO;
  return &HaisinWork.beacon_send_data;
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
  beacon->member_num = 1;//GFL_NET_GetConnectNum();
  beacon->member_max = 3;
  beacon->error = 0;//GFL_NET_SystemGetErrorCode();
  beacon->beacon_type = GBS_BEACONN_TYPE_NONE;
  beacon->trainer_id = 0x1328;//MyStatus_GetID( GAMEDATA_GetMyStatus(gamedata) );
  beacon->isTwl = OS_IsRunOnTwl();
  beacon->restrictPhoto = FALSE;
  beacon->pad_detect_fold = TRUE; //PAD_DetectFold(); �p���X�Ɉ���������Ȃ��悤�ɊW������
  if(beacon->isTwl == TRUE){
  #ifdef SDK_TWL
    beacon->restrictPhoto = OS_IsRestrictPhotoExchange();
  #endif
  }
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
static int GameBeacon_GetBeaconSize(void *pWork)
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
static BOOL GameBeacon_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2, void* pWork )
{
  // CGEAR�ŐF�X�ȃr�[�R�����擾�������̂�TRUE�݂̂ɂ��� k.ohno 2010.01.08
  return TRUE;
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
}

