//=============================================================================
/**
 * @file	  delivery_irc.c
 * @bfief	  �ԊO���z�M
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  10/02/27
 */
//=============================================================================

#include <gflib.h>

#include "net/network_define.h"

#include "system/main.h"
#include "system/gfl_use.h"
#include "system/net_err.h"

#include "net/delivery_irc.h"


//�r�[�R���P�̂̒��g
typedef struct
{
  u8 data[DELIVERY_IRC_MAX_NUM];   // �S�̂̃f�[�^��RC4���������Ă���
} DELIVERY_IRC;


typedef void (StateFunc)(DELIVERY_IRC_WORK* pState);

//���[�J�����[�N
struct _DELIVERY_IRC_LOCALWORK{
  DELIVERY_IRC_INIT aInit;   //�������\���̂̃R�s�[
//   DELIVERY_IRC aSend;  //�z�M����A�󂯎��\����
  DELIVERY_IRC aRecv;  //�z�M����A�󂯎��\����
  u8 end;
  u8 bSend;
  u16 bNego;
  u16 crc;
  StateFunc* state;      ///< �n���h���̃v���O�������
};

#define _BCON_GET_NUM (16)

#define _TIMING_START (23)
#define _TIMING_START2 (24)
#define _TIMING_END (35)

static void _RecvDeliveryData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static u8* _getDeliveryData(int netID, void* pWk, int size);
static void _Recvcrc16Data(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);

#define _NET_CMD(x) (x<<8)  //�l�b�g���[�N�T�[�r�XID����R�}���h�ւ̕ϊ��}�N��

enum{
  _DELIVERY_DATA, //���̒l�ɏ�L�}�N���𑫂�
  _CRCCCTI_DATA,
};

static const NetRecvFuncTable _CommPacketTbl[] = {
  {_RecvDeliveryData,         _getDeliveryData},    ///��M�f�[�^
  {_Recvcrc16Data,         NULL},    ///��M�f�[�^
};



static void _netNegotiationFunc(void* pWk,int NetID)
{
  DELIVERY_IRC_WORK *pWork = pWk;
  pWork->bNego = TRUE;


}


/**
 * @brief   �X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @retval  none
 */

static void _changeState(DELIVERY_IRC_WORK* pWork, StateFunc state)
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
static void _changeStateDebug(DELIVERY_IRC_WORK* pWork,StateFunc state, int line)
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


// ��M
static void _RecvDeliveryData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  //���łɎ�M�ς�
/*
  DELIVERY_IRC_WORK *pWork = pWk;
  int i,j;
  const u8* pU8 = pData;
  
    for(j=0;j<size;){
      for(i=0;i<16;i++){
        OS_TPrintf("%x ",pU8[j]);
        j++;
      }
      OS_TPrintf("\n");
    }
  
  GFL_STD_MemCopy( pData, pWork->aInit.pData,pWork->aInit.datasize);

  OS_TPrintf("_RecvDeliveryData %d %d\n",netID,size);
*/
}


static void _Recvcrc16Data(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  const u16* pU16 = pData;
  DELIVERY_IRC_WORK *pWork = pWk;
  
  pWork->crc = pU16[0];
  OS_TPrintf("crc16\n");
}


//------------------------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@��Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static u8* _getDeliveryData(int netID, void* pWk, int size)
{
  DELIVERY_IRC_WORK *pWork = pWk;
  return (u8*)pWork->aRecv.data;
}

//�Ȃɂ����Ȃ�
static void _nop(DELIVERY_IRC_WORK* pWork)
{
}

//����
static void _sendInit7(DELIVERY_IRC_WORK* pWork)
{
  if( GFL_NET_IsInit() == FALSE ){
    if(!pWork->bSend){
      pWork->end = DELIVERY_IRC_SUCCESS;
      
      GFL_STD_MemCopy( pWork->aRecv.data, pWork->aInit.pData,pWork->aInit.datasize);
      if(pWork->crc != GFL_STD_CrcCalc( pWork->aInit.pData, pWork->aInit.datasize) ){
        pWork->end = DELIVERY_IRC_FAILED;
      }
    }
    else{
      pWork->end = DELIVERY_IRC_SUCCESS;
    }
    _CHANGE_STATE(_nop);
  }
}

//NET�I��
static void _sendInit6(DELIVERY_IRC_WORK* pWork)
{
  GFL_NET_Exit(NULL);
  _CHANGE_STATE(_sendInit7);
}

//�I���^�C�~���O
static void _sendInit5(DELIVERY_IRC_WORK* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_END, pWork->aInit.NetDevID) ){
    _CHANGE_STATE(_sendInit6);
  }
}

//�I���^�C�~���O
static void _sendInit4(DELIVERY_IRC_WORK* pWork)
{
  u16 crc = GFL_STD_CrcCalc( pWork->aInit.pData, pWork->aInit.datasize);

  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),  _TIMING_END, pWork->aInit.NetDevID);
  _CHANGE_STATE(_sendInit5);
}


//����
static void _sendInit3(DELIVERY_IRC_WORK* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_START2, pWork->aInit.NetDevID) ){
    if(pWork->bSend){
      if( GFL_NET_SendDataEx( GFL_NET_HANDLE_GetCurrentHandle() , GFL_NET_SENDID_ALLUSER ,
                              _DELIVERY_DATA + _NET_CMD( pWork->aInit.NetDevID ), pWork->aInit.datasize ,
                              pWork->aInit.pData , FALSE , FALSE , TRUE )){
        _CHANGE_STATE(_sendInit4);
      }
    }
    else{
      _CHANGE_STATE(_sendInit4);
    }
  }
}

static void _sendInit25(DELIVERY_IRC_WORK* pWork)

{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_START, pWork->aInit.NetDevID) ){
    if( pWork->bSend ){
      u16 crc=0;
      crc = GFL_STD_CrcCalc( pWork->aInit.pData, pWork->aInit.datasize);
      if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle() , _CRCCCTI_DATA + _NET_CMD( pWork->aInit.NetDevID ), sizeof(u16) ,  &crc )){
        GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),  _TIMING_START2, pWork->aInit.NetDevID);
        pWork->bNego=FALSE;
        _CHANGE_STATE(_sendInit3);
      }
    }
    else{
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),  _TIMING_START2, pWork->aInit.NetDevID);
      _CHANGE_STATE(_sendInit3);
    }
  }
}


//�^�C�~���O
static void _sendInit2(DELIVERY_IRC_WORK* pWork)
{
  if( GFL_NET_GetConnectNum() > 1 ){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),  _TIMING_START, pWork->aInit.NetDevID);
    _CHANGE_STATE(_sendInit25);
  }
}

//�ڑ�
static void _sendInit1(DELIVERY_IRC_WORK* pWork)
{
  GFL_NET_ChangeoverConnect( NULL );
  _CHANGE_STATE(_sendInit2);
}

//����������
static void _sendInit(DELIVERY_IRC_WORK* pWork)
{
  if( GFL_NET_IsInit() == TRUE ){
    _CHANGE_STATE(_sendInit1);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO���z�M������
 * @param   DELIVERY_IRC_INIT �������\���� ���[�J���ϐ��ł����v
 * @retval  DELIVERY_IRC_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------

DELIVERY_IRC_WORK* DELIVERY_IRC_Init(DELIVERY_IRC_INIT* pInit)
{
  int i;
  DELIVERY_IRC_WORK*  pWork = GFL_HEAP_AllocClearMemory(pInit->heapID,sizeof(DELIVERY_IRC_WORK));

  GFL_STD_MemCopy(pInit, &pWork->aInit, sizeof(DELIVERY_IRC_INIT));
  return pWork;
}


static BOOL _initNet(DELIVERY_IRC_WORK* pWork)
{
  static GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CommPacketTbl), //��M�e�[�u���v�f��
    NULL,   ///< �n�[�h�Őڑ��������ɌĂ΂��
    _netNegotiationFunc,   ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    NULL,//IrcBattleBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    NULL,//IrcBattleBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    NULL,//IrcBattleBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    NULL,//FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL, //_endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
    SYASHI_NETWORK_GGID,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    2,     // �ő�ڑ��l��
    64,  //�ő呗�M�o�C�g��
    16,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_IRC_BATTLE,  //GameServiceID
    0xfffe, // �ԊO���^�C���A�E�g����
    0,//MP�ʐM�e�@���M�o�C�g��
    0,//dummy
  };

  if( GFL_NET_IsInit() ){  // �����ʐM���Ă���ꍇ�I��
    return FALSE;
  }
  aGFLNetInit.gsid = pWork->aInit.NetDevID;

  GFL_NET_Init(&aGFLNetInit, NULL, pWork);

  return TRUE;
}




//--------------------------------------------------------------
/**
 * @brief   �ԊO���z�M�J�n
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------
BOOL DELIVERY_IRC_SendStart(DELIVERY_IRC_WORK* pWork)
{
  BOOL bRet = _initNet(pWork);
  pWork->bSend=TRUE;
  _CHANGE_STATE(_sendInit);
  return bRet;

}


//--------------------------------------------------------------
/**
 * @brief   IRC��M�J�n
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 * @retval  �J�n�ł�����TRUE
 */
//--------------------------------------------------------------
BOOL DELIVERY_IRC_RecvStart(DELIVERY_IRC_WORK* pWork)
{

  BOOL bRet = _initNet(pWork);
  pWork->bSend = FALSE;
  _CHANGE_STATE(_sendInit);
  return bRet;
}

//--------------------------------------------------------------
/**
 * @brief   IRC��M�������ǂ���
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 * @retval  �󂯎�ꂽ�ꍇDELIVERY_IRC_SUCCESS ���sDELIVERY_IRC_FAILED
 */
//--------------------------------------------------------------
int DELIVERY_IRC_RecvCheck(DELIVERY_IRC_WORK* pWork)
{
  return pWork->end;
}


//--------------------------------------------------------------
/**
 * @brief   IRC�z�����C��
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------

void DELIVERY_IRC_Main(DELIVERY_IRC_WORK*  pWork)
{
  if( pWork->state )
  { 
    pWork->state(pWork);
  }
}



//--------------------------------------------------------------
/**
 * @brief   IRC�z�M�I��
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------

void DELIVERY_IRC_End(DELIVERY_IRC_WORK*  pWork)
{
  GFL_HEAP_FreeMemory(pWork);
  GFL_NET_Exit(NULL);
}

