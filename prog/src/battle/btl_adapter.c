//=============================================================================================
/**
 * @file	btl_adapter.c
 * @brief	�|�P����WB �o�g���V�X�e��	�T�[�o-�N���C�A���g�ԃA�_�v�^�[
 * @author	taya
 *
 * @date	2008.09.25	�쐬
 */
//=============================================================================================
#include <gflib.h>

#include "btl_common.h"

#include "btl_net.h"
#include "btl_adapter.h"
#include "btl_server_cmd.h"


enum {
	DATA_BUFFER_SIZE = BTL_SERVER_CMD_QUE_SIZE + 4,
};

typedef enum {

	AS_FREE = 0,
	AS_CMD_RECEIVED,
	AS_WAIT_SEND_TO_CLIENT,
	AS_RECEPTION_CLIENT,
	AS_DONE,

}AdapterState;


/**
 *
 */
struct _BTL_ADAPTER {
	GFL_NETHANDLE*	netHandle;

	const void*	returnDataAdrs;
	u32			returnDataSize;
	u8			returnDataPreparedFlag;

	const void*	sendDataAdrs;
	u32			sendDataSize;

	u8	myID;
	u8	myState;
	u8	processingCmd;
	u8	commMode;

	u32	dataBufPtr;
	u8	dataBuffer[ DATA_BUFFER_SIZE ];
};



//-------------------------------------------------------------------------
/**
 *	�쐬���ꂽ�A�_�v�^�Ǘ��p�z��
 * �i�P�}�V���ɓ���ID�̃A�_�v�^�͂P�݂̂Ƃ��������̕ێ���ȒP�ɂ��邽�߁j
 */
//-------------------------------------------------------------------------
static BTL_ADAPTER* AdappterTable[ BTL_CLIENT_MAX ];


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void _StartToReception( BTL_ADAPTER* wk );
static BOOL _WaitToReception(  BTL_ADAPTER* wk );
static BOOL _ReceptionClient( BTL_ADAPTER* wk );
static void datBuf_init( BTL_ADAPTER* wk );
static void datBuf_write( BTL_ADAPTER* wk, const void* src, u32 size );



//=============================================================================================
/**
 * �V�X�e���������i�o�g���V�X�e���N������ɂP�񂾂��Ăяo���j
 */
//=============================================================================================
void BTL_ADAPTER_InitSystem( void )
{
	int i;
	for(i=0; i<BTL_CLIENT_MAX; i++)
	{
		AdappterTable[i] = NULL;
	}
}
//=============================================================================================
/**
 * 	�V�X�e���I���i�o�g���V�X�e���I�����ɂP�񂾂��Ăяo���j
 */
//=============================================================================================
void BTL_ADAPTER_QuitSystem( void )
{
	// �Ƃ��ɂȂɂ�
}


BTL_ADAPTER*  BTL_ADAPTER_Create( BtlCommMode commMode, GFL_NETHANDLE* netHandle, HEAPID heapID, u16 adapterID )
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
		wk->commMode = commMode;

		wk->sendDataAdrs = NULL;
		wk->sendDataSize = 0;

		wk->dataBufPtr = 0;
	}


	// ��ʐM���[�h�F����ID�͂P��������ăT�[�o�E�N���C�A���g�o������g����
	if( commMode == BTL_COMM_NONE )
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
 * [SERVER�p]	�N���C�A���g�ɑ΂���R�}���h���M���J�n����
 *
 * @param   wk				�A�_�v�^���W���[��
 * @param   cmd				�R�}���h�i���o�[
 * @param   sendData		���M�f�[�^�A�h���X
 * @param	sendDataSize	���M�f�[�^�T�C�Y
 *
 */
//--------------------------------------------------------------------------------------
void BTL_ADAPTER_SetCmd( BTL_ADAPTER* wk, BtlAdapterCmd cmd, const void* sendData, u32 sendDataSize )
{
	GF_ASSERT(wk->myState == AS_FREE);

	wk->myState = AS_CMD_RECEIVED;
	wk->processingCmd = cmd;

	datBuf_init( wk );
	datBuf_write( wk, &cmd, sizeof(cmd) );
	datBuf_write( wk, sendData, sendDataSize );

	wk->sendDataAdrs = sendData;
	wk->sendDataSize = sendDataSize;

	wk->returnDataAdrs = NULL;
	wk->returnDataSize = 0;
	wk->returnDataPreparedFlag = FALSE;

//	BTL_Printf(" [AD] %d, �R�}���h %d �𑗐M�J�n���܂�\n", wk->myID, wk->processingCmd);
}


//--------------------------------------------------------------------------------------
/**
 * [SERVER�p]	���M�����R�}���h�ɑ΂���N���C�A���g����̕ԐM��҂�
 *
 * @param   wk			�A�_�v�^���W���[��
 *
 * @retval  BOOL		�N���C�A���g����̕ԐM���󂯎������TRUE
 */
//--------------------------------------------------------------------------------------
BOOL BTL_ADAPTER_WaitCmd( BTL_ADAPTER* wk )
{
	switch( wk->myState ){
	case AS_CMD_RECEIVED:
		_StartToReception( wk );
		wk->myState = AS_WAIT_SEND_TO_CLIENT;
		/* fallthru */
	case AS_WAIT_SEND_TO_CLIENT:
		if( !_WaitToReception(wk) )
		{
			break;
		}
		wk->myState = AS_RECEPTION_CLIENT;
		/* fallthru */
	case AS_RECEPTION_CLIENT:
		if( !_ReceptionClient(wk) )
		{
			break;
		}
		wk->myState = AS_DONE;
		/*fallthru*/;
	case AS_DONE:
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER�p]	�N���C�A���g����A���Ă����f�[�^�ւ̃|�C���^���擾
 *
 * @param   wk				�A�_�v�^���W���[��
 *
 * @retval  const void*		�f�[�^�|�C���^
 */
//--------------------------------------------------------------------------------------
const void* BTL_ADAPTER_GetReturnData( BTL_ADAPTER* wk )
{
	GF_ASSERT(wk->returnDataPreparedFlag);
	return wk->returnDataAdrs;
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER�p]	�N���C�A���g����A���Ă����f�[�^�̃T�C�Y���擾
 *
 * @param   wk		�A�_�v�^���W���[��
 *
 * @retval  u32		�f�[�^�|�C���^
 */
//--------------------------------------------------------------------------------------
u32 BTL_ADAPTER_GetReturnDataSize( BTL_ADAPTER* wk )
{
	GF_ASSERT(wk->returnDataPreparedFlag);
	return wk->returnDataSize;
}

//--------------------------------------------------------------------------------------
/**
 * [SERVER�p]	�N���C�A���g�ɑ��M����R�}���h�����Z�b�g����
 *
 * @param   wk		�A�_�v�^���W���[��
 *
 */
//--------------------------------------------------------------------------------------
void BTL_ADAPTER_ResetCmd( BTL_ADAPTER *wk )
{
	GF_ASSERT(wk->myState == AS_FREE || wk->myState == AS_DONE);
	wk->myState = AS_FREE;
}

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------


static void _StartToReception( BTL_ADAPTER* wk )
{
	// �X�^���h�A�����Ȃ珈���s�v
	if( wk->commMode != BTL_COMM_NONE )
	{
		BTL_NET_SendToClient( wk->dataBuffer, wk->dataBufPtr );
	}
}
static BOOL _WaitToReception(  BTL_ADAPTER* wk )
{
	// �X�^���h�A�����Ȃ珈���s�v
	if( wk->commMode != BTL_COMM_NONE )
	{
		return BTL_NET_CheckSendToClient();
	}
	else
	{
		return TRUE;
	}
}

static BOOL _ReceptionClient( BTL_ADAPTER* wk )
{
	if( wk->commMode != BTL_COMM_NONE )
	{
		return BTL_NET_CheckReturnFromClient();
	}
	else
	{
		return wk->returnDataPreparedFlag;
	}
}


//--------------------------------------------------------------------------------------
/**
 * [CLIENT�p]	�A�_�v�^�R�}���h��M�`�F�b�N
 *
 * @param   wk		
 *
 * @retval  BtlAdapterCmd		
 */
//--------------------------------------------------------------------------------------
BtlAdapterCmd BTL_ADAPTER_RecvCmd( BTL_ADAPTER* wk )
{
	// �ʐM��
	if( wk->commMode != BTL_COMM_NONE )
	{
		if( BTL_NET_IsServerCmdReceived() )
		{
			u32* p;
			BTL_NET_GetReceivedCmdData( (const void**)&p );
			BTL_Printf("[BTLADP] �T�[�o�R�}���h[%d]��M���܂���\n", *p);
			return *p;
		}
		else
		{
			BTL_Printf("[BTLADP] �T�[�o�R�}���h��M�҂��Ł[��\n");
			return BTL_ACMD_NONE;
		}
	}
	// ��ʐM��
	else
	{
		if( wk->returnDataPreparedFlag == FALSE )
		{
			return wk->processingCmd;
		}
		else
		{
			return BTL_ACMD_NONE;
		}
	}
}

//--------------------------------------------------------------------------------------
/**
 * [CLIENT�p]	�A�_�v�^�R�}���h���f�[�^����M
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
	if( wk->commMode != BTL_COMM_NONE )
	{
		return BTL_NET_GetReceivedCmdData( ppRecv );
	}
	// ��ʐM��
	else
	{
		*ppRecv = wk->sendDataAdrs;
		return wk->sendDataSize;
	}
}


//--------------------------------------------------------------------------------------
/**
 * 
 *
 * @param   wk		
 * @param   data		
 * @param   size		
 *
 */
//--------------------------------------------------------------------------------------
void BTL_ADAPTER_ReturnCmd( BTL_ADAPTER* wk, const void* data, u32 size )
{
	// �ʐM��
	if( wk->commMode != BTL_COMM_NONE )
	{
		BTL_NET_ReturnToServer( data, size );
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
	}
}






//--------------------------------------------------------------------------
/**
 * �f�[�^�o�b�t�@������
 *
 * @param   wk		
 *
 */
//--------------------------------------------------------------------------
static void datBuf_init( BTL_ADAPTER* wk )
{
	wk->dataBufPtr = 0;
}

//--------------------------------------------------------------------------
/**
 * �f�[�^�o�b�t�@��������
 *
 * @param   wk			
 * @param   src			
 * @param   size		
 *
 */
//--------------------------------------------------------------------------
static void datBuf_write( BTL_ADAPTER* wk, const void* src, u32 size )
{
	GF_ASSERT_MSG( (wk->dataBufPtr+size) <= DATA_BUFFER_SIZE, "data buffer size over : dataBufPtr=%d, size=%d",
				wk->dataBufPtr, size );

	GFL_STD_MemCopy( src, &wk->dataBuffer[wk->dataBufPtr], size );
	wk->dataBufPtr += size;
}


