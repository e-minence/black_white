//=============================================================================================
/**
 * @file  btl_adapter.c
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o-�N���C�A���g�ԃA�_�v�^�[
 * @author  taya
 *
 * @date  2008.09.25  �쐬
 */
//=============================================================================================
#include <gflib.h>

#include "btl_common.h"

#include "btl_net.h"
#include "btl_server_cmd.h"
#include "btl_action.h"
#include "btl_adapter.h"

enum {
  PRINT_FLG = FALSE,
};


enum {
  DATA_BUFFER_SIZE = BTL_SERVER_CMD_QUE_SIZE + 4,
};

typedef enum {

  AS_FREE = 0,
  AS_CMD_RECEIVED,
  AS_WAIT_SEND_TO_CLIENT,
  AS_WAIT_RECV_FROM_CLIENT,
  AS_DONE,

}AdapterState;


typedef struct {

  struct {
    u16   cmd;
    u16   size;
  } header;

  u8    data[ DATA_BUFFER_SIZE ];

}SEND_DATA_BUFFER;

/**
 *
 */
struct _BTL_ADAPTER {
  GFL_NETHANDLE*  netHandle;

  const void* returnDataAdrs;
  u32     returnDataSize;
  u8      returnDataPreparedFlag;

  u8  myID;
  u8  myState;
  u8  processingCmd;

  SEND_DATA_BUFFER   sendDataBuffer;
};



//-------------------------------------------------------------------------
/**
 *  �쐬���ꂽ�A�_�v�^�Ǘ��p�z��
 * �i�P�}�V���ɓ���ID�̃A�_�v�^�͂P�݂̂Ƃ��������̕ێ���ȒP�ɂ��邽�߁j
 */
//-------------------------------------------------------------------------
static BTL_ADAPTER* AdappterTable[ BTL_CLIENT_MAX ];
static u8 CommMode;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL _StartToReception( BTL_ADAPTER* wk );
static BOOL _ReceptionClient( BTL_ADAPTER* wk );
static inline void sendBuf_store( SEND_DATA_BUFFER* buf, BtlAdapterCmd cmd, const void* data, u32 dataSize );
static inline u32 sendBuf_calcSize( const SEND_DATA_BUFFER* buf );
static inline BtlAdapterCmd sendBuf_getCmd( const SEND_DATA_BUFFER* buf );
static inline u32 sendBuf_getData( const SEND_DATA_BUFFER* buf, const void** pptr );



//=============================================================================================
/**
 * �V�X�e���������i�o�g���V�X�e���N������ɂP�񂾂��Ăяo���j
 */
//=============================================================================================
void BTL_ADAPTERSYS_Init( BtlCommMode mode )
{
  int i;

  for(i=0; i<BTL_CLIENT_MAX; i++)
  {
    AdappterTable[i] = NULL;
  }

  CommMode = mode;
}
//=============================================================================================
/**
 *  �V�X�e���I���i�o�g���V�X�e���I�����ɂP�񂾂��Ăяo���j
 */
//=============================================================================================
void BTL_ADAPTERSYS_Quit( void )
{
  // �Ƃ��ɂȂɂ�
}


BTL_ADAPTER*  BTL_ADAPTER_Create( GFL_NETHANDLE* netHandle, HEAPID heapID, u16 adapterID )
{
  BTL_ADAPTER* wk = NULL;

  if( AdappterTable[adapterID] == NULL )
  {
    wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_ADAPTER) );
    wk->netHandle = netHandle;
    wk->myID = adapterID;
    wk->myState = AS_FREE;
    wk->processingCmd = BTL_ACMD_NONE;
    wk->returnDataPreparedFlag = FALSE;
  }

  // ��ʐM���[�h�F����ID�͂P��������ăT�[�o�E�N���C�A���g�o������g����
  if( CommMode == BTL_COMM_NONE )
  {
    if( AdappterTable[adapterID] == NULL )
    {
      AdappterTable[adapterID] = wk;
    }

    return AdappterTable[ adapterID ];
  }
  // �ʐM���[�h�F����ID�̐������N�G�X�g�����邱�Ƃ͂��蓾�Ȃ�
  else
  {
    GF_ASSERT( AdappterTable[adapterID] == NULL );
    AdappterTable[adapterID] = wk;
    return AdappterTable[ adapterID ];
  }
}

void BTL_ADAPTER_Delete( BTL_ADAPTER* wk )
{
  GFL_HEAP_FreeMemory( wk );
}



//--------------------------------------------------------------------------------------
/**
 * [SERVER�p] BTL_ADAPTER_SetCmd �̑O�ɌĂяo��
 */
//--------------------------------------------------------------------------------------
void BTL_ADAPTERSYS_BeginSetCmd( void )
{
  if( CommMode != BTL_COMM_NONE )
  {
    BTL_NET_ClearRecvData();
  }
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER�p] BTL_ADAPTER_SetCmd �̌�ɌĂяo��
 */
//--------------------------------------------------------------------------------------
void BTL_ADAPTERSYS_EndSetCmd( void )
{
  if( CommMode != BTL_COMM_NONE )
  {

  }
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER�p] �N���C�A���g�ɑ΂���R�}���h���M���J�n����
 *
 * @param   wk        �A�_�v�^���W���[��
 * @param   cmd       �R�}���h�i���o�[
 * @param   sendData    ���M�f�[�^�A�h���X
 * @param sendDataSize  ���M�f�[�^�T�C�Y
 *
 */
//--------------------------------------------------------------------------------------
void BTL_ADAPTER_SetCmd( BTL_ADAPTER* wk, BtlAdapterCmd cmd, const void* sendData, u32 sendDataSize )
{
  GF_ASSERT(wk->myState == AS_FREE);

  wk->myState = AS_CMD_RECEIVED;
  wk->processingCmd = cmd;

  sendBuf_store( &wk->sendDataBuffer, cmd, sendData, sendDataSize );

  wk->returnDataAdrs = NULL;
  wk->returnDataSize = 0;
  wk->returnDataPreparedFlag = FALSE;

  BU_Printf(PRINT_FLG, " Adapter-%d, �R�}���h %d �𑗐M�J�n���܂�\n", wk->myID, wk->processingCmd );
}


//--------------------------------------------------------------------------------------
/**
 * [SERVER�p] ���M�����R�}���h�ɑ΂���N���C�A���g����̕ԐM��҂�
 *
 * @param   wk      �A�_�v�^���W���[��
 *
 * @retval  BOOL    �N���C�A���g����̕ԐM���󂯎������TRUE
 */
//--------------------------------------------------------------------------------------
BOOL BTL_ADAPTER_WaitCmd( BTL_ADAPTER* wk )
{
  switch( wk->myState ){
  case AS_CMD_RECEIVED:
    if( !_StartToReception(wk) ){
      break;
    }
    wk->myState = AS_WAIT_RECV_FROM_CLIENT;
    /* fallthru */
  case AS_WAIT_RECV_FROM_CLIENT:
    if( !_ReceptionClient(wk) )
    {
      break;
    }
    BU_Printf(PRINT_FLG, " [Adapter] Received from All Clients\n");
    wk->myState = AS_DONE;
    /*fallthru*/;
  case AS_DONE:
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER�p] �N���C�A���g����A���Ă����f�[�^�ւ̃|�C���^���擾
 *
 * @param   wk        �A�_�v�^���W���[��
 * @param   size      [out] �f�[�^�T�C�Y
 *
 * @retval  const void*   �f�[�^�|�C���^
 */
//--------------------------------------------------------------------------------------
const void* BTL_ADAPTER_GetReturnData( BTL_ADAPTER* wk, u32* size )
{
  GF_ASSERT(wk->returnDataPreparedFlag);

  if( size != NULL ){
    *size = wk->returnDataSize;
  }
  return wk->returnDataAdrs;
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER�p] �N���C�A���g����A���Ă����f�[�^�̃T�C�Y���擾
 *
 * @param   wk    �A�_�v�^���W���[��
 *
 * @retval  u32   �f�[�^�|�C���^
 */
//--------------------------------------------------------------------------------------
u32 BTL_ADAPTER_GetReturnDataSize( BTL_ADAPTER* wk )
{
  GF_ASSERT(wk->returnDataPreparedFlag);
  return wk->returnDataSize;
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER�p] �N���C�A���g�ɑ��M����R�}���h�����Z�b�g����
 *
 * @param   wk    �A�_�v�^���W���[��
 *
 */
//--------------------------------------------------------------------------------------
void BTL_ADAPTER_ResetCmd( BTL_ADAPTER *wk )
{
  GF_ASSERT(wk->myState == AS_FREE || wk->myState == AS_DONE);
  wk->myState = AS_FREE;

  if( CommMode != BTL_COMM_NONE )
  {
    BTL_NET_ClearRecvData();
  }
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------


static BOOL _StartToReception( BTL_ADAPTER* wk )
{
  // �X�^���h�A�����Ȃ珈���s�v
  if( CommMode != BTL_COMM_NONE )
  {
    SEND_DATA_BUFFER* sendBuf = &wk->sendDataBuffer;
    return BTL_NET_SendToClient( wk->myID, sendBuf, sendBuf_calcSize(sendBuf) );
  }
  else{
    return TRUE;
  }
}

static BOOL _ReceptionClient( BTL_ADAPTER* wk )
{
  if( CommMode != BTL_COMM_NONE )
  {
    if( BTL_NET_CheckReturnFromClient() )
    {
      wk->returnDataSize = BTL_NET_GetRecvClientData( wk->myID, &wk->returnDataAdrs );
      wk->returnDataPreparedFlag = TRUE;
      BU_Printf(PRINT_FLG, " [Adapter] Received from AdapterID=%d\n", wk->myID );
      return TRUE;
    }
    return FALSE;
  }
  else
  {
    return wk->returnDataPreparedFlag;
  }
}


//--------------------------------------------------------------------------------------
/**
 * [CLIENT�p] �A�_�v�^�R�}���h��M�`�F�b�N
 *
 * @param   wk
 *
 * @retval  BtlAdapterCmd
 */
//--------------------------------------------------------------------------------------
BtlAdapterCmd BTL_ADAPTER_RecvCmd( BTL_ADAPTER* wk )
{
  // �ʐM��
  if( CommMode != BTL_COMM_NONE )
  {
    if( BTL_NET_IsServerCmdReceived() )
    {
      SEND_DATA_BUFFER* sendBuf;
      BtlAdapterCmd cmd;

      BTL_NET_GetReceivedCmdData( (const void**)&sendBuf );
      cmd = sendBuf_getCmd( sendBuf );

      BTL_Printf("�T�[�o�R�}���h(%d)��M���܂���\n", cmd);

      return cmd;
    }
    else
    {
      return BTL_ACMD_NONE;
    }
  }
  // ��ʐM��
  else
  {
    if( wk->returnDataPreparedFlag == FALSE )
    {
      return sendBuf_getCmd( &wk->sendDataBuffer );
    }
    else
    {
      return BTL_ACMD_NONE;
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 * [CLIENT�p] �A�_�v�^�R�}���h���f�[�^����M
 *
 * @param   wk
 * @param   ppRecv
 *
 * @retval  u32
 */
//--------------------------------------------------------------------------------------
u32 BTL_ADAPTER_GetRecvData( BTL_ADAPTER* wk, const void** ppRecv )
{
  // �ʐM��
  if( CommMode != BTL_COMM_NONE )
  {
    SEND_DATA_BUFFER* sendBuf;

    BTL_NET_GetReceivedCmdData( (const void**)&sendBuf );
    return sendBuf_getData( sendBuf, ppRecv );
  }
  // ��ʐM��
  else
  {
    return sendBuf_getData( &wk->sendDataBuffer, ppRecv );
  }
}

//=============================================================================================
/**
 * �N���C�A���g���T�[�o�փR�}���h�ԐM
 *
 * @param   wk
 * @param   data    �ԐM�f�[�^�A�h���X
 * @param   size    �ԐM�f�[�^�T�C�Y
 *
 * @retval  BOOL    ������TRUE�i��������܂Ŗ��t���[���ĂԂ��Ɓj
 */
//=============================================================================================
BOOL BTL_ADAPTER_ReturnCmd( BTL_ADAPTER* wk, const void* data, u32 size )
{
  // �ʐM��
  if( CommMode != BTL_COMM_NONE )
  {
    return BTL_NET_ReturnToServer( data, size );
  }
  // ��ʐM��
  else
  {
    if( wk->returnDataPreparedFlag == FALSE )
    {
      wk->returnDataAdrs = data;
      wk->returnDataSize = size;
      wk->returnDataPreparedFlag = TRUE;
    }
    return TRUE;
  }
}




static inline void sendBuf_store( SEND_DATA_BUFFER* buf, BtlAdapterCmd cmd, const void* data, u32 dataSize )
{
  GF_ASSERT(dataSize < sizeof(buf->data));

  buf->header.cmd = cmd;
  buf->header.size = dataSize;
  GFL_STD_MemCopy( data, buf->data, dataSize );
}

static inline u32 sendBuf_calcSize( const SEND_DATA_BUFFER* buf )
{
  return sizeof(buf->header) + buf->header.size;
}

static inline BtlAdapterCmd sendBuf_getCmd( const SEND_DATA_BUFFER* buf )
{
  return buf->header.cmd;
}

static inline u32 sendBuf_getData( const SEND_DATA_BUFFER* buf, const void** pptr )
{
  *pptr = buf->data;
  return buf->header.size;
}

