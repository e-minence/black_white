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
#include "net/net_irc.h"

#if PM_DEBUG
#define DELIVDEBUG_PRINT (0)
#else
#define DELIVDEBUG_PRINT (0)
#endif


typedef enum
{
  DELIVERY_IRC_FLAG_NONE,       //�Ȃɂ��Ȃ�
  DELIVERY_IRC_FLAG_NO_REQUEST_DATA,    //���N�G�X�g�ɑΉ�����t�@�C�����Ȃ�
} DELIVERY_IRC_FLAG;

typedef struct 
{
  u32 LangCode;
  u32 version;
} DELIVERY_IRC_REQUEST ;


//�r�[�R���P�̂̒��g
typedef struct
{
  u8 data[DELIVERY_IRC_MAX_NUM];   // �S�̂̃f�[�^��RC4���������Ă���
} DELIVERY_IRC;


typedef struct{
  u32 key;
  u16 crc;
  u16 dummy;
} CRCCCTI_STRCT;

typedef void (StateFunc)(DELIVERY_IRC_WORK* pState);

//���[�J�����[�N
struct _DELIVERY_IRC_LOCALWORK{
  DELIVERY_IRC_INIT aInit;   //�������\���̂̃R�s�[
  DELIVERY_IRC aRecv;  //�z�M����A�󂯎��\����
  DELIVERY_DATA* pDevData;
  u32 key;
  u32 recvFlag;
  u16 bNego;
  u16 crc;
  u8 end;
  u8 bSend;
  u8 bRequestExist;     ///<�@���N�G�X�g���ꂽ�f�[�^�����݂������ǂ���
  u8 dataIdx;
  StateFunc* state;      ///< �n���h���̃v���O�������

  DELIVERY_IRC_REQUEST  request;  ///<�f�[�^��M���́A���N�G�X�g���M�o�b�t�@�@�f�[�^���M���̓��N�G�X�g��M�o�b�t�@
};

#define _BCON_GET_NUM (16)

#define _TIMING_START (23)
#define _TIMING_START2 (24)
#define _TIMING_END (35)

static void _RecvDeliveryData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static u8* _getDeliveryData(int netID, void* pWk, int size);
static void _Recvcrc16Data(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _RequestData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _RecvFlag(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);

#define _NET_CMD(x) (x<<8)  //�l�b�g���[�N�T�[�r�XID����R�}���h�ւ̕ϊ��}�N��

enum{
  _DELIVERY_DATA, //���̒l�ɏ�L�}�N���𑫂�
  _CRCCCTI_DATA,
  _REQUEST_DATA,
  _FLAG,
};

static const NetRecvFuncTable _CommPacketTbl[] = {
  {_RecvDeliveryData,         _getDeliveryData},    ///��M�f�[�^
  {_Recvcrc16Data,         NULL},    ///��M�f�[�^
  {_RequestData,         NULL},    ///����R�[�h
  {_RecvFlag,         NULL},    ///�t���O
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
  const CRCCCTI_STRCT* pcrcst = pData;
  DELIVERY_IRC_WORK *pWork = pWk;

  pWork->crc = pcrcst->crc;
  pWork->key = pcrcst->key;
}



static void _RequestData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  const DELIVERY_IRC_REQUEST * pRequest = pData;
  DELIVERY_IRC_WORK *pWork = pWk;
  
  pWork->request = *pRequest;
}

static void _RecvFlag(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  const u32* pU32 = pData;
  DELIVERY_IRC_WORK *pWork = pWk;
  
  pWork->recvFlag = pU32[0];
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

      if( pWork->recvFlag == DELIVERY_IRC_FLAG_NO_REQUEST_DATA )
      { 
        //�Ή����錾�ꂪ�Ȃ�����
        pWork->end = DELIVERY_IRC_NOTDATA;
      }
      else
      { 
        //�Ή����錾�ꂪ������

        pWork->end = DELIVERY_IRC_SUCCESS;

        GFL_STD_MemCopy( pWork->aRecv.data, pWork->aInit.data[0].pData,pWork->aInit.data[0].datasize);
        if(pWork->crc != GFL_STD_CrcCalc( pWork->aInit.data[0].pData, pWork->aInit.data[0].datasize) ){
          pWork->end = DELIVERY_IRC_FAILED;
        }
        GFL_STD_CODED_Coded( pWork->aInit.data[0].pData,pWork->aInit.data[0].datasize, pWork->key);
      }
    }
    else{
      if( pWork->bRequestExist == TRUE )
      { 
        pWork->end = DELIVERY_IRC_SUCCESS;
      }
      else
      { 
        pWork->end = DELIVERY_IRC_NOTDATA;
      }
    }
    _CHANGE_STATE(_nop);
  }
}


//NET�I��
static void _sendInit6Parent(DELIVERY_IRC_WORK* pWork)
{
  if( GFL_NET_IRC_IsConnect() )
    GFL_NET_Exit(NULL);
  _CHANGE_STATE(_sendInit7);
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
    if(GFL_NET_IsParentMachine()){
      GFL_NET_IRC_ChangeTimeoutTime(IRC_TIMEOUT_STANDARD);  //�I�����ɂ͕ύX����
      _CHANGE_STATE(_sendInit6Parent);
    }
    else{
      _CHANGE_STATE(_sendInit6);
    }
  }
}

//�I���^�C�~���O
static void _sendInit4(DELIVERY_IRC_WORK* pWork)
{
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),  _TIMING_END, pWork->aInit.NetDevID);
  _CHANGE_STATE(_sendInit5);
}


//����
static void _sendInit3(DELIVERY_IRC_WORK* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_START2, pWork->aInit.NetDevID) ){
    if(pWork->bSend){
      if( pWork->bRequestExist == TRUE )
      { 
        DELIVERY_DATA *pData = &pWork->aInit.data[pWork->dataIdx];
        if( GFL_NET_SendDataEx( GFL_NET_HANDLE_GetCurrentHandle() , GFL_NET_SENDID_ALLUSER ,
              _DELIVERY_DATA + _NET_CMD( pWork->aInit.NetDevID ), pData->datasize ,
              pData->pData , FALSE , FALSE , TRUE )){
          _CHANGE_STATE(_sendInit4);
        }
      }
      else
      { 
        _CHANGE_STATE(_sendInit4);
      }
    }
    else{
      _CHANGE_STATE(_sendInit4);
    }
  }
}






static void _sendInit26(DELIVERY_IRC_WORK* pWork)

{
  CRCCCTI_STRCT crcstr;
  
  //����R�[�h���������̂�CRC�𑗂�
  if( pWork->bRequestExist == TRUE )
  { 
#if DELIVDEBUG_PRINT
    OS_TPrintf( "�q�@�֓n���f�[�^��%d�Ԃł�\n", pWork->dataIdx );
#endif
    crcstr.crc = GFL_STD_CrcCalc( pWork->pDevData->pData, pWork->pDevData->datasize);
    crcstr.key = pWork->key;
    if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle() ,
                          _CRCCCTI_DATA + _NET_CMD( pWork->aInit.NetDevID ), sizeof(CRCCCTI_STRCT) ,  &crcstr )){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),  _TIMING_START2, pWork->aInit.NetDevID);
      pWork->bNego=FALSE;
      _CHANGE_STATE(_sendInit3);
    }
  }
  else
  { 
#if DELIVDEBUG_PRINT
    OS_TPrintf( "�q�@�֓n���f�[�^���Ȃ�����\n" );
#endif
    //�Ȃ������̂łȂ���t���O��n��
    pWork->recvFlag = DELIVERY_IRC_FLAG_NO_REQUEST_DATA;
    if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle() ,
                          _FLAG + _NET_CMD( pWork->aInit.NetDevID ),
                          sizeof(u32) ,  &pWork->recvFlag )){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),  _TIMING_START2, pWork->aInit.NetDevID);
      
      pWork->bNego=FALSE;
      _CHANGE_STATE(_sendInit3);
    }
  }
}



static void _sendInit25(DELIVERY_IRC_WORK* pWork)

{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_START, pWork->aInit.NetDevID) ){
    if( pWork->bSend ){

      //�󂯎��������R�[�h�̃f�[�^�𑗂�
      //�����Ȃ��ꍇ�͂Ȃ���Ƃ����t���O��n��

      u16 crc=0;
      DELIVERY_DATA *pData  = NULL;

#if DELIVDEBUG_PRINT
      OS_TPrintf( "�q�@���烊�N�G�X�g�����Ƃ�@����%d ver%d\n", pWork->request.LangCode, pWork->request.version );
#endif
      //�Ή����錾��R�[�h���T�[�`
      { 
        int i;
        pWork->bRequestExist = FALSE;
        for( i = 0; i < pWork->aInit.dataNum; i++ )
        { 
          pData = &pWork->aInit.data[i];
          if( (pData->LangCode == pWork->request.LangCode )
              && (pData->version & pWork->request.version) )  
          { 
            pWork->dataIdx  = i;
            pWork->bRequestExist = TRUE;
            break;
          }
        }
      }
      _CHANGE_STATE(_sendInit26);
      //����R�[�h���������̂ňÍ�������
      if( pWork->bRequestExist == TRUE )
      { 
        pWork->key = GFUser_GetPublicRand(0);
        GFL_STD_CODED_Coded( pData->pData, pData->datasize, pWork->key);
        pWork->pDevData = pData;
      }
    }
    else{
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),  _TIMING_START2, pWork->aInit.NetDevID);
      _CHANGE_STATE(_sendInit3);
    }
  }
}


//�q�@���e�@�ɗ~�����t�@�C���̃��N�G�X�g�𑗂�
static void _sendInitLang(DELIVERY_IRC_WORK* pWork)
{
  if( !pWork->bSend ){
    pWork->request.LangCode = pWork->aInit.data[0].LangCode;
    pWork->request.version  = pWork->aInit.data[0].version;
    if( GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle() ,
                          _REQUEST_DATA + _NET_CMD( pWork->aInit.NetDevID ),
                          sizeof(DELIVERY_IRC_REQUEST) ,  &pWork->request )){
      _CHANGE_STATE(_sendInit25);
    }
  }
  else{
    _CHANGE_STATE(_sendInit25);
  }
}

//�^�C�~���O
static void _sendInit2(DELIVERY_IRC_WORK* pWork)
{
  if( GFL_NET_GetConnectNum() > 1 ){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),  _TIMING_START, pWork->aInit.NetDevID);
    _CHANGE_STATE(_sendInitLang);
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
    SYACHI_NETWORK_GGID,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    2,     // �ő�ڑ��l��
    64,  //�ő呗�M�o�C�g��
    16,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_IRC_BATTLE,  //GameServiceID
#ifdef BUGFIX_AF_BTS7976_20100806
    0xffffffff, // �ԊO���^�C���A�E�g����
#else
    0xfffe, // �ԊO���^�C���A�E�g����
#endif
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
  pWork->bRequestExist  = 0;
  pWork->dataIdx        = 0;
  pWork->recvFlag       = 0;
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
  pWork->bRequestExist  = 0;
  pWork->dataIdx        = 0;
  pWork->recvFlag       = 0;
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

