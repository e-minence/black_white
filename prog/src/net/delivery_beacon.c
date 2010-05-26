//=============================================================================
/**
 * @file	  delivery_beacon.c
 * @bfief	  �r�[�R���z�M
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  10/02/15
 */
//=============================================================================

#include <gflib.h>


#include "net/network_define.h"

#include "system/main.h"
#include "system/gfl_use.h"
#include "system/net_err.h"

#include "net/delivery_beacon.h"
#include "net/wih.h"
#include "net/net_whpipe.h"

#if PM_DEBUG
#define DELIVDEBUG_PRINT (0)
#else
#define DELIVDEBUG_PRINT (0)
#endif



#define _BEACON_CHANGE_COUNT (0)

//�r�[�R���P�̂̒��g
typedef struct 
{
  u16 crc16;      // CRC16-CCITT ���������� 0x1021 �����l0xffff�o��XOR�Ȃ� ������  ����CRC�̓r�[�R���P�̗p
  u8 data[DELIVERY_BEACON_ONCE_NUM];   // �S�̂̃f�[�^��RC4���������Ă���
  u8 count;       //����ڂ̃r�[�R����
  u8 countMax;       //�����A������̂�
  u8 ConfusionID;         // ���̔z�M�Ȃ̂�  �����z�M���ɂ��Ԃ点�Ȃ��悤��
  u8 LangCode;     //����R�[�h
  u32 version;      //�o�[�W�����̃r�b�g
} DELIVERY_BEACON;


typedef void (StateFunc)(DELIVERY_BEACON_WORK* pState);

//���[�J�����[�N
struct _DELIVERY_BEACON_LOCALWORK{
  DELIVERY_BEACON_INIT aInit;   //�������\���̂̃R�s�[
  DELIVERY_BEACON aSendRecv[DELIVERY_IRC_SEND_DATA_MAX][DELIVERY_BEACON_MAX_NUM];  //�z�M����A�󂯎��\����
  int nowCount;
  StateFunc* state;      ///< �n���h���̃v���O�������
};

#define _BCON_GET_NUM (16)



static void _netConnectFunc(void* pWork,int hardID);
static void _netNegotiationFunc(void* pWork,int NetID);
static void* _netBeaconGetFunc(void* pWork);
static int _netBeaconGetSizeFunc(void* pWork);
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo, void* pWork);
static void _endCallBack(void* pWork);
static void _RecvDummyData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);



static const NetRecvFuncTable _CommPacketTbl[] = {
  {_RecvDummyData,         NULL},    ///DUMMY
};


static GFLNetInitializeStruct aGFLNetInit = {
  _CommPacketTbl,  // ��M�֐��e�[�u��
  NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
  _netConnectFunc,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  _netNegotiationFunc,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
  _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
  _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
  NULL,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  _endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  NULL,   ///< DWC�`���̗F�B���X�g
  NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  0,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
  SYACHI_NETWORK_GGID,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_NETWORK,  //IRC�p��create�����HEAPID
  GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
  1,                            // �ő�ڑ��l��
  88,                  //�ő呗�M�o�C�g��
  _BCON_GET_NUM,    // �ő�r�[�R�����W��
  TRUE,     // CRC�v�Z
  FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIRELESS,  //�ʐM���
  TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_DEBUG_OHNO_SERVICEID,  //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
  500,
  0,
};


static void _netConnectFunc(void* pWork,int hardID)
{}
static void _RecvDummyData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{}
static void _netNegotiationFunc(void* pWork,int NetID)
{}
static void _endCallBack(void* pWork)
{}



//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------

static void* _netBeaconGetFunc(void* pWk)
{
  DELIVERY_BEACON_WORK* pWork=pWk;
  int no  = pWork->nowCount%DELIVERY_BEACON_MAX_NUM;
  int num = pWork->nowCount/DELIVERY_BEACON_MAX_NUM;

  if( num >= pWork->aInit.dataNum )
  { 
    pWork->nowCount = 0;
    num = 0;
  }

  return &pWork->aSendRecv[num][no];
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _netBeaconGetSizeFunc(void* pWork)
{
  return sizeof(DELIVERY_BEACON);
}

///< �r�[�R���f�[�^�擾�֐�
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo, void* pWork)
{
  if(myNo != beaconNo){
    return FALSE;
  }
  return TRUE;
}



/**
 * @brief   �X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @retval  none
 */

static void _changeState(DELIVERY_BEACON_WORK* pWork, StateFunc state)
{
  pWork->state = state;
}

/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   line �s
 * @retval  none
 */
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(DELIVERY_BEACON_WORK* pWork,StateFunc state, int line)
{
  NET_PRINT("deli %d\n",line);
  _changeState(pWork, state);
}
#endif

#ifdef GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pWork ,state,  __LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pWork ,state )
#endif //GFL_NET_DEBUG


//�f�[�^���r�[�R���̌`�ɕ���
static void _beaconDataDiv(DELIVERY_BEACON_WORK* pWork)
{
  int i, j;
  int max;

  for(j = 0; j < pWork->aInit.dataNum ; j++){
    max = (pWork->aInit.data[j].datasize/DELIVERY_BEACON_ONCE_NUM)+1;
    GF_ASSERT(pWork->aInit.data[j].datasize < (DELIVERY_BEACON_MAX_NUM*DELIVERY_BEACON_ONCE_NUM));
#if DELIVDEBUG_PRINT
    OS_TPrintf("%d %d\n",pWork->aInit.data[j].datasize, (DELIVERY_BEACON_MAX_NUM*DELIVERY_BEACON_ONCE_NUM));
#endif
    for(i = 0; i < DELIVERY_BEACON_MAX_NUM ; i++){
      DELIVERY_BEACON* pBeacon;
      DELIVERY_BEACON_INIT* pInit;
      DELIVERY_DATA *pInitData;
      u8* pData;
      u16 crc;

      pInit = &pWork->aInit;
      pBeacon = &pWork->aSendRecv[j][i];
      pInitData   = &pInit->data[j];

      GFL_STD_MemCopy( &pInitData->pData[DELIVERY_BEACON_ONCE_NUM*i] , pBeacon->data,  DELIVERY_BEACON_ONCE_NUM);
      pBeacon->count = i+1;
      pBeacon->countMax = max;
      pBeacon->ConfusionID = pInit->ConfusionID;

      pData = (u8*)pBeacon->data;

      crc = GFL_STD_CrcCalc( pData, DELIVERY_BEACON_ONCE_NUM);
      pBeacon->crc16 = crc;
      pBeacon->version  = pInitData->version;
      pBeacon->LangCode = pInitData->LangCode;
    }
  }

}

//���Ԋu�Ńr�[�R�����ꂩ�����M
static void _sendLoop(DELIVERY_BEACON_WORK* pWork)
{
  if( WHGetBeaconSendNum() > _BEACON_CHANGE_COUNT ){
    pWork->nowCount++;
    NET_WHPIPE_BeaconSetInfo();
#if DELIVDEBUG_PRINT
    OS_TPrintf("���ꂩ��%d\n",pWork->nowCount);
#endif
  }
}
//�T�[�o�N��
static void _sendInit1(DELIVERY_BEACON_WORK* pWork)
{
  GFL_NET_InitServer();
  _CHANGE_STATE( _sendLoop );
}
//���������� 
static void _sendInit(void* pWK)
{
  DELIVERY_BEACON_WORK* pWork = pWK;
  _CHANGE_STATE(_sendInit1);
}
//�������Ȃ�
static void _not(DELIVERY_BEACON_WORK* pWork)
{
}

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���z�M������
 * @param   DELIVERY_BEACON_INIT �������\���� ���[�J���ϐ��ł����v
 * @retval  DELIVERY_BEACON_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------

DELIVERY_BEACON_WORK* DELIVERY_BEACON_Init(DELIVERY_BEACON_INIT* pInit)
{
  int i;
  DELIVERY_BEACON_WORK*  pWork = GFL_HEAP_AllocClearMemory(pInit->heapID,sizeof(DELIVERY_BEACON_WORK));

  GFL_STD_MemCopy(pInit, &pWork->aInit, sizeof(DELIVERY_BEACON_INIT));
  return pWork;
}

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���z�M�J�n
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------
BOOL DELIVERY_BEACON_SendStart(DELIVERY_BEACON_WORK* pWork)
{

  if( GFL_NET_IsInit() ){  // �����ʐM���Ă���ꍇ�I��
    return FALSE;
  }
  aGFLNetInit.gsid = pWork->aInit.NetDevID;
  _beaconDataDiv( pWork );

  GFL_NET_Init(&aGFLNetInit, _sendInit, pWork);

  _CHANGE_STATE( _not );
  return TRUE;
}




//�i�[�ꏊ�쐬
static void _beaconAlloc(DELIVERY_BEACON_WORK* pWork)
{
  if(pWork->aInit.data[0].datasize){
    int max = (pWork->aInit.data[0].datasize/DELIVERY_BEACON_ONCE_NUM)+1;
#if DELIVDEBUG_PRINT
    OS_TPrintf("%d %d\n",pWork->aInit.data[0].datasize , (DELIVERY_BEACON_MAX_NUM*DELIVERY_BEACON_ONCE_NUM));
#endif
    GF_ASSERT(pWork->aInit.data[0].datasize < (DELIVERY_BEACON_MAX_NUM*DELIVERY_BEACON_ONCE_NUM));
  }
}


//�f�[�^�擾����
static BOOL  _recvCheck(DELIVERY_BEACON_WORK* pWork)
{
  int i;
  int max = pWork->aSendRecv[0][0].countMax;
  u8 ConfusionID = pWork->aSendRecv[0][0].ConfusionID;  //0�̃r�[�R�����Ɠ����������낦��

  if(max == 0){
    return FALSE;
  }
  for(i = 0;i < max;i++){
    DELIVERY_BEACON* pBeacon = &pWork->aSendRecv[0][i];
    if(max != pBeacon->countMax){
      return  FALSE;
    }
    if(ConfusionID != pBeacon->ConfusionID){
      // ID���������������
      GFL_STD_MemClear( pBeacon,  sizeof(DELIVERY_BEACON));
      return  FALSE;
    }
  }
  return TRUE;
}

//�r�[�R���X�L����
static void  _recvLoop(DELIVERY_BEACON_WORK* pWork)
{
  DELIVERY_BEACON* pBeacon;
//  DELIVERY_BEACON_INIT* pInit;
  int i,index;
  int max;
//  pInit = &pWork->aInit;

  for(i = 0;i < _BCON_GET_NUM;i++){
    DELIVERY_BEACON* pBeacon = GFL_NET_WLGetUserBss(i);
    u8* pData;
    u16 crc;

    if(pBeacon==NULL){
      continue;
    }
  
    //����`�F�b�N
    if( (pBeacon->LangCode != pWork->aInit.data[0].LangCode) )
    { 
      continue;
    }

    //�o�[�W�����`�F�b�N
    if( (pBeacon->version & pWork->aInit.data[0].version) == 0 )
    { 
      continue;
    }

    index = pBeacon->count - 1;
    if(index >= DELIVERY_BEACON_MAX_NUM){
#if DELIVDEBUG_PRINT
      OS_TPrintf("DELIVERY_BEACON_MAX_NUM\n");
#endif
      continue;
    }
    if(pWork->aSendRecv[0][index].count != 0){
      continue;  //�����E���Ă���
    }
    pData = (u8*)pBeacon->data;
    crc = GFL_STD_CrcCalc( pData, DELIVERY_BEACON_ONCE_NUM);
    if(crc != pBeacon->crc16){
#if DELIVDEBUG_PRINT
      OS_TPrintf("�Ԉ�����f�[�^\n");
#endif
      continue; //�Ԉ�����f�[�^
    }
    //�擾
#if DELIVDEBUG_PRINT
    OS_TPrintf("�擾%d \n",index);
#endif
    GFL_NET_WLFIXScan( i ); //�X�L���������肷��

    GFL_STD_MemCopy( pBeacon, &pWork->aSendRecv[0][index],  sizeof(DELIVERY_BEACON));
  }

}
//   �A�������r�[�R���f�[�^������
static void _CreateRecvData(DELIVERY_BEACON_WORK* pWork)
{
  int i;
  
  for(i = 0; i < DELIVERY_BEACON_MAX_NUM ; i++){
    DELIVERY_BEACON* pBeacon;
    DELIVERY_BEACON_INIT* pInit;
    u8* pData;
    u16 block = i*DELIVERY_BEACON_ONCE_NUM;

    pInit = &pWork->aInit;
    pBeacon = &pWork->aSendRecv[0][i];

    if(pInit->data[0].datasize > (block + DELIVERY_BEACON_ONCE_NUM)){
      GFL_STD_MemCopy( pBeacon->data, &pInit->data[0].pData[i*DELIVERY_BEACON_ONCE_NUM],  DELIVERY_BEACON_ONCE_NUM);
    }
    else{
      GFL_STD_MemCopy( pBeacon->data , &pInit->data[0].pData[i*DELIVERY_BEACON_ONCE_NUM],  (pInit->data[0].datasize - block));
    }
  }
}


//�r�[�R���X�L�����J�n
static void _recvInit1(DELIVERY_BEACON_WORK* pWork)
{
  GFL_NET_StartBeaconScan();
  _CHANGE_STATE(_recvLoop);
}
//���������� 
static void _recvInit(void* pWK)
{
  DELIVERY_BEACON_WORK* pWork = pWK;
  _CHANGE_STATE(_recvInit1);
}


//--------------------------------------------------------------
/**
 * @brief   �r�[�R����M�J�n
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 * @retval  �J�n�ł�����TRUE
 */
//--------------------------------------------------------------
BOOL DELIVERY_BEACON_RecvStart(DELIVERY_BEACON_WORK* pWork)
{

  if( GFL_NET_IsInit() ){  // �����ʐM���Ă���ꍇ�I��
    return FALSE;
  }
  aGFLNetInit.gsid = pWork->aInit.NetDevID;
  _beaconAlloc( pWork );
  GFL_NET_Init(&aGFLNetInit, _recvInit, pWork);
  _CHANGE_STATE( _not );
  return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief   �r�[�R�����P�ł����邩�ǂ���
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 * @retval  ����ꍇTRUE
 */
//--------------------------------------------------------------
BOOL DELIVERY_BEACON_RecvSingleCheck(DELIVERY_BEACON_WORK* pWork)
{
  int i;

  for(i = 0;i < DELIVERY_BEACON_MAX_NUM;i++){
    DELIVERY_BEACON* pBeacon = &pWork->aSendRecv[0][i];
    if(0 != pBeacon->countMax){
      return TRUE;
    }
  }
  return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief   �r�[�R����M�������ǂ���
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 * @retval  �󂯎�ꂽ�ꍇTRUE
 */
//--------------------------------------------------------------
BOOL DELIVERY_BEACON_RecvCheck(DELIVERY_BEACON_WORK* pWork)
{
  BOOL bRet = _recvCheck(pWork);

  if(bRet){
    _CreateRecvData(pWork);
  }
  return bRet;
}


//--------------------------------------------------------------
/**
 * @brief   �r�[�R���z�����C��
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------

void DELIVERY_BEACON_Main(DELIVERY_BEACON_WORK*  pWork)
{
  pWork->state(pWork);
}



//--------------------------------------------------------------
/**
 * @brief   �r�[�R���z�M�I��
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------

void DELIVERY_BEACON_End(DELIVERY_BEACON_WORK*  pWork)
{
  GFL_HEAP_FreeMemory(pWork);
  GFL_NET_Exit(NULL);
}

