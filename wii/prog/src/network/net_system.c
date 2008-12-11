//=============================================================================
/**
 * @file	net_system.c
 * @brief	�ʐM�V�X�e��
 * @author	GAME FREAK Inc.
 * @date	2006.12.05
 */
//=============================================================================


#include "net_def.h"
#include "net_local.h"
#include "net_system.h"
#include "net_command.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"

#define _SENDRECV_LIMIT  (3)  // ���M�Ǝ�M�̐������ꂽ�ꍇ���M��}������

//==============================================================================
// extern�錾
//==============================================================================




//==============================================================================
// ��`
//==============================================================================

//�f�[�^�V�F�A�����O�ʐM�̃w�b�_�[  MP���̎q�@�̃w�b�_�[
const int _DS_CRCNO1 = 0;
const int _DS_CRCNO2 = 1;
const int _DS_INCNO = 2;
const int _DS_HEADERNO = 3;
const int _DS_HEADER_MAX = 4;
const int _DS_CRC_MAX = 2;

//MP���̐e�@�̃w�b�_�[


const int _MP_CONNECTBIT = 1;
const int _MP_SIZEO1 = 2;
const int _MP_SIZEO2 = 3;
const int _MP_CRCNO1 = 4;
const int _MP_CRCNO2 = 5;
const int _MP_INCNO = 6;
const int _MP_DATA_HEADER_POS = 7;



/// �N���C�A���g���M�p�L���[�̖{��
#define _SENDQUEUE_NUM_MAX  (100)
/// �T�[�o�[���M�p�L���[�̖{��
#define _SENDQUEUE_SERVER_NUM_MAX	  (180)

// �q�@RING���M�W��
#define _SEND_RINGBUFF_FACTOR_CHILD  (15)
// �e�@RING���M�W��
#define _SEND_RINGBUFF_FACTOR_PARENT  (2)

//#define _MIDDLE_BUFF_NUM  (4)  ///DS�p�~�h���o�b�t�@�ɂǂ̒��x���߂���̂�

// ����������Ă��Ȃ��C�e���[�^�[�̐�
#define _NULL_ITERATE (-1)
// ���肦�Ȃ�ID
#define _INVALID_ID  (GFL_NET_NETID_INVALID)
// ���M�f�[�^���܂����炦�Ă��Ȃ�����
//#define _NODATA_SEND  (0x0e)

//VBlank�����̃^�X�N�̃v���C�I���e�B�[
#define _PRIORITY_VBLANKFUNC (0)

/** @brief �ʐM�擪�o�C�g��BIT�̈Ӗ� */
#define _SEND_NONE  (0x01)  ///< ���ő����ꍇ
#define _SEND_NEXT  (0x02)  ///< ���ő���Ȃ��ꍇ
#define _MP_DATA_HEADER (0x08)  ///< MP�f�[�^�̏ꍇBITON  DS���ɂ�OFF
#define _INVALID_HEADER  (0xff)  ///< �����������M�f�[�^���܂��Z�b�g����Ă��Ȃ��ꍇ


typedef enum{   // ���M���
	_SEND_CB_NONE,		   // �Ȃɂ����Ă��Ȃ�
	_SEND_CB_DSDATA_PACK,	// DS��p �f�[�^��~��
	_SEND_CB_FIRST_SEND,	 // ����̒��ł̍ŏ��̑��M
	_SEND_CB_FIRST_SENDEND,  // �ŏ��̑��M�̃R�[���o�b�N������
} _sendCallbackCondition_e;




//==============================================================================
// ���[�N
//==============================================================================


static _COMM_WORK_SYSTEM* _pComm = NULL;  ///<�@���[�N�\���̂̃|�C���^

// ���M�������Ƃ��m�F���邽�߂̃t���O
static volatile u8 _sendServerCallBack = _SEND_CB_FIRST_SENDEND;
// ����
static volatile u8 _sendCallBack = _SEND_CB_FIRST_SENDEND;

static void _debugSetSendCallBack(u8 data,int line)
{
	const int test = line;	//���[�j���O���
//	NET_PRINT("scb: %d\n",line);
	_sendCallBack = data;
}

#define   _setSendCallBack(state)  _debugSetSendCallBack(state, __LINE__)


#pragma mark [>NewFuncList

//==============================================================================
// static�錾
//==============================================================================
static void _commCommandInit(void);
static void _dataDsStep( MPDSContext *DSContext );
static BOOL _sendDataSet(BOOL bDataShare);
static void _recvDataServerFunc(void);
static BOOL _setSendDataSystem(u8* pSendBuff);

//==============================================================================
/**
 * @brief   �ő�ڑ��l���𓾂�
 * @return  �ő�ڑ��l��
 */
//==============================================================================

static int _getUserMaxNum(void)
{
	return GFI_NET_GetConnectNumMax();
}

//==============================================================================
/**
 * @brief   �q�@���Mbyte���𓾂܂�
 * @retval  �q�@���M�T�C�Y
 */
//==============================================================================

static int _getUserMaxSendByte(void)
{
	return GFI_NET_GetSendSizeMax();
}

//==============================================================================
/**
 * @brief   �e�@���Mbyte���𓾂܂�
 * @retval  �e�@���M�T�C�Y
 */
//==============================================================================

static int _getUserMaxSendByteParent(void)
{
	return GFI_NET_GetSendSizeMax()*GFI_NET_GetConnectNumMax()+_MP_DATA_HEADER_POS;
}

//==============================================================================
/**
 * @brief   �e�q���ʁA�ʐM�̏��������܂Ƃ߂�
 * @param   packetSizeMax   �m�ۂ������p�P�b�g�T�C�Y
 * @param   heapID		  �m�ۂ��Ă�����heapID
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================

BOOL _commSystemInit(int packetSizeMax, HEAPID heapID)
{
//	void* pWork;
//	int i;

	if(_pComm==NULL){
		int machineMax = _getUserMaxNum();
		int parentSize = _getUserMaxSendByteParent();

		NET_PRINT("_COMM_WORK_SYSTEM size %d \n", sizeof(_COMM_WORK_SYSTEM));
		_pComm = (_COMM_WORK_SYSTEM*)GFL_HEAP_AllocClearMemory(heapID, sizeof(_COMM_WORK_SYSTEM));

		_pComm->packetSizeMax = packetSizeMax + 64;

		_pComm->pRecvBufRing = (u8*)GFL_HEAP_AllocClearMemory(heapID, _pComm->packetSizeMax*2); ///< �q�@���󂯎��o�b�t�@
		_pComm->pTmpBuff = (u8*)GFL_HEAP_AllocClearMemory(heapID, _pComm->packetSizeMax*2);  ///< ��M�󂯓n���̂��߂̈ꎞ�o�b�t�@
		_pComm->pServerRecvBufRing = (u8*)GFL_HEAP_AllocClearMemory(heapID, machineMax * _pComm->packetSizeMax);   ///< �󂯎��o�b�t�@���o�b�N�A�b�v����
		// �L���[�̏�����

		_pComm->sSendBuf = (u8*)GFL_HEAP_AllocClearMemory(heapID, _getUserMaxSendByte());
		_pComm->sSendBufRing = (u8*)GFL_HEAP_AllocClearMemory(heapID, _getUserMaxSendByte()*_SEND_RINGBUFF_FACTOR_CHILD);
		_pComm->sSendServerBuf = (u8*)GFL_HEAP_AllocClearMemory(heapID, parentSize);
		_pComm->sSendServerBufRing = (u8*)GFL_HEAP_AllocClearMemory(heapID, parentSize * _SEND_RINGBUFF_FACTOR_PARENT);


		GFL_NET_QueueManagerInitialize(&_pComm->sendQueueMgr, _SENDQUEUE_NUM_MAX, &_pComm->sendRing, heapID);
		GFL_NET_QueueManagerInitialize(&_pComm->sendQueueMgrServer, _SENDQUEUE_SERVER_NUM_MAX, &_pComm->sendServerRing, heapID);

		_commCommandInit();
	}

	_pComm->bitmap = 0;
	_pComm->bWifiConnect = FALSE;
	return TRUE;
}


//==============================================================================
/**
 * @brief   �e�q���ʁA�ʐM�̃R�}���h�Ǘ��̏��������܂Ƃ߂�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _commCommandInit(void)
{
	//void* pWork;
	int i;
	int machineMax = _getUserMaxNum();
	int parentSize = _getUserMaxSendByteParent();
	int childSize = _getUserMaxSendByte();

	// �e�@�݂̂̑���M
	{
		NET_PRINT("packet %d %d\n",_pComm->packetSizeMax,machineMax);
		MI_CpuFill8(_pComm->pServerRecvBufRing, 0, _pComm->packetSizeMax * machineMax);
		for(i = 0; i< machineMax;i++){
			GFL_NET_RingInitialize(&_pComm->recvServerRing[i],
								   &_pComm->pServerRecvBufRing[i*_pComm->packetSizeMax],
								   _pComm->packetSizeMax);
		}

	}
	MI_CpuFill8(_pComm->sSendServerBufRing, 0, parentSize * _SEND_RINGBUFF_FACTOR_PARENT);
	GFL_NET_RingInitialize(&_pComm->sendServerRing, _pComm->sSendServerBufRing,
						   parentSize * _SEND_RINGBUFF_FACTOR_PARENT);
	for(i = 0; i < parentSize; i++){
		_pComm->sSendServerBuf[i] = GFL_NET_CMD_NONE;
	}
	// �q�@�̑���M
	MI_CpuFill8(_pComm->sSendBufRing, 0, childSize*_SEND_RINGBUFF_FACTOR_CHILD);
	GFL_NET_RingInitialize(&_pComm->sendRing, _pComm->sSendBufRing, childSize*_SEND_RINGBUFF_FACTOR_CHILD);

	for(i = 0;i < childSize;i++){
		_pComm->sSendBuf[i] = GFL_NET_CMD_NONE;
	}
	_pComm->sSendBuf[_DS_HEADERNO] = _INVALID_HEADER;
	MI_CpuFill8(_pComm->pRecvBufRing, 0, _pComm->packetSizeMax*2);
	GFL_NET_RingInitialize(&_pComm->recvRing, _pComm->pRecvBufRing, _pComm->packetSizeMax*2);

	_pComm->bNextSendData = FALSE;
	_pComm->bNextSendDataServer = FALSE;
	for(i = 0; i< GFL_NET_MACHINE_MAX;i++){
		_pComm->bFirstCatch[i] = TRUE;
		_pComm->recvCommServer[i].valCommand = GFL_NET_CMD_NONE;
		_pComm->recvCommServer[i].realSize = 0xffff;
		_pComm->recvCommServer[i].tblSize = 0xffff;
		_pComm->recvCommServer[i].pRecvBuff = NULL;
		_pComm->recvCommServer[i].dataPoint = 0;
		_pComm->countSendRecvServer[i] = 0;
	}
	_pComm->countSendRecv = 0;
	_pComm->bWifiSendRecv = TRUE;
	_pComm->recvCommClient.valCommand = GFL_NET_CMD_NONE;
	_pComm->recvCommClient.realSize = 0xffff;
	_pComm->recvCommClient.tblSize = 0xffff;
	_pComm->recvCommClient.pRecvBuff = NULL;
	_pComm->recvCommClient.dataPoint = 0;

	_pComm->bFirstCatchP2C = TRUE;
	//_pComm->bSendNoneSend = TRUE;

	_sendServerCallBack = _SEND_CB_FIRST_SENDEND;
	_setSendCallBack( _SEND_CB_FIRST_SENDEND);

	// �L���[�̃��Z�b�g
	GFL_NET_QueueManagerReset(&_pComm->sendQueueMgr);
	GFL_NET_QueueManagerReset(&_pComm->sendQueueMgrServer);
	_pComm->bResetState = FALSE;
	_pComm->bDSSendOK = TRUE;

	NET_PRINT(" �ʐM�f�[�^���Z�b�g\n");

}

void	GFL_NET_UpdateSystem( MPDSContext *DSContext )
{
	_dataDsStep( DSContext );
	_recvDataServerFunc();  // �T�[�o�[���̎�M����
	
}


//==============================================================================
/**
 * @brief   �ʐM�R�[���o�b�N�̍Ō�ɌĂ� �R�}���h������
 * @param   netID   �l�b�gID
 * @retval  none
 */
//==============================================================================

static void _endCallBack(int command,int size,void* pTemp, _RECV_COMMAND_PACK* pRecvComm)
{
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

	GFL_NETHANDLE* pNetHandle = GFL_NET_GetNetHandle(GFL_NET_SystemGetCurrentID());
	GFI_NET_COMMAND_CallBack(pRecvComm->sendID, pRecvComm->recvBit, command, size, pTemp, pNetHandle);
	if(GFL_NET_SystemGetCurrentID()==GFL_NET_NO_PARENTMACHINE){
		pNetHandle = GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER);
		if(command < GFL_NET_CMD_COMMAND_MAX){  // �e�@�ɑ���R�}���h�𐧌�����
			GFI_NET_COMMAND_CallBack(pRecvComm->sendID, pRecvComm->recvBit, command, size, pTemp, pNetHandle);
		}
	}
	pRecvComm->valCommand = GFL_NET_CMD_NONE;
	pRecvComm->realSize = 0xffff;
	pRecvComm->tblSize = 0xffff;
	pRecvComm->pRecvBuff = NULL;
	pRecvComm->dataPoint = 0;
}

//==============================================================================
/**
 * @brief   ��M�����f�[�^���v���Z�X���ɏ�������
 * @param   pRing  �����O�o�b�t�@�̃|�C���^
 * @param   netID	 �������Ă���netID
 * @param   pTemp	�R�}���h���������邽�߂�temp�o�b�t�@
 * @retval  none
 */
//==============================================================================

static void _recvDataFuncSingle(RingBuffWork* pRing, int netID, u8* pTemp, _RECV_COMMAND_PACK* pRecvComm)
{
	int mcSize = _getUserMaxSendByte();
	int size;
	u16 command;
	int bkPos;
	int realbyte;
	int sendID;
	int recvID;
	u8* pB;
//	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();


	while( GFL_NET_RingDataSize(pRing) != 0 ){
		bkPos = pRing->startPos;
		if(pRecvComm->valCommand != GFL_NET_CMD_NONE){
			command = pRecvComm->valCommand;
		}
		else{
            if( GFL_NET_RingDataSize(pRing) < 2 ){  // �c��f�[�^�������ꍇ����
                GFL_NET_RingGetByte(pRing);
                break;
            }
            command = GFL_NET_RingGetShort(pRing);
            if(command == GFL_NET_CMD_NONE){
				continue;
			}
		}
		bkPos = pRing->startPos;
		pRecvComm->valCommand = command;
		if(pRecvComm->tblSize != 0xffff){
			size = pRecvComm->tblSize;
		}
		else{
			if(_pComm->bError){
				return;
			}
            if( GFL_NET_RingDataSize(pRing) < _GFL_NET_QUEUE_HEADERBYTE ){  // �c��f�[�^���w�b�_�[�����������玟��
				pRing->startPos = bkPos;
				break;
			}
			size = GFL_NET_RingGetShort(pRing);
			pRecvComm->realSize = size;
			pRecvComm->tblSize = size;
			pRecvComm->sendID = netID;
			pRecvComm->recvBit = GFL_NET_RingGetByte(pRing);
		}
		bkPos = pRing->startPos; // �i�߂�
		
		pB = NULL;
		if(GFI_NET_COMMAND_CreateBuffCheck(command) && GFI_NET_COMMAND_RecvCheck(pRecvComm->recvBit)){  // ��M�o�b�t�@������ꍇ
			if(pRecvComm->pRecvBuff == NULL){
				pRecvComm->pRecvBuff = (u8*)GFI_NET_COMMAND_CreateBuffStart(command, pRecvComm->sendID, pRecvComm->tblSize);
			}
			pB = &pRecvComm->pRecvBuff[pRecvComm->dataPoint];
		}
		else{
			pRecvComm->pRecvBuff = pTemp;
			pB = pTemp;
		}
		if(GFI_NET_COMMAND_RecvCheck(pRecvComm->recvBit) == FALSE){
			pB = NULL;
		}

		if(GFI_NET_COMMAND_CreateBuffCheck(command)==FALSE){
			realbyte = GFL_NET_RingGets(pRing, pTemp, pRecvComm->realSize , pRecvComm->realSize );
		}
		else{
			realbyte = GFL_NET_RingGets(pRing, pTemp, pRecvComm->realSize, mcSize - _DS_HEADER_MAX);
		}
		pRecvComm->realSize -= realbyte;

		size = realbyte;
		if(pRecvComm->tblSize > pRecvComm->dataPoint){
			size = pRecvComm->tblSize - pRecvComm->dataPoint;
			if(size > realbyte){
				size = realbyte;
			}
			if(pB){
				MI_CpuCopy8(pTemp, pB, size);
			}
		}
		pRecvComm->dataPoint += size;
		if(pRecvComm->realSize == 0 ){
			_endCallBack(command, pRecvComm->tblSize, pRecvComm->pRecvBuff, pRecvComm);
		}
		else{   // �܂��͂��Ă��Ȃ��傫���f�[�^�̏ꍇ�ʂ���
			NET_PRINT("�����҂� command %d size %d %d\n",command,size,pRecvComm->realSize);
			if(GFI_NET_COMMAND_CreateBuffCheck(command)==FALSE){
				pRecvComm->realSize += realbyte;
				pRing->startPos = bkPos;
			}
			break;
		}
	}
}

//==============================================================================
/**
 * @brief   ��M�����f�[�^���v���Z�X���ɏ�������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _recvDataServerFunc(void)
{
	int id;
	int size;
	u8 command;
	int machineMax;

	if(!_pComm){
		return;
	}

	machineMax = _getUserMaxNum();

	for(id = 0; id < machineMax; id++){
		GFL_NET_RingEndChange(&_pComm->recvServerRing[id]);

		if(GFL_NET_RingDataSize(&_pComm->recvServerRing[id]) > 0){
#if 1
//			NET_PRINT("�e�@���q�@%d�����\n",id);
#endif
			_recvDataFuncSingle(&_pComm->recvServerRing[id], id, _pComm->pTmpBuff, &_pComm->recvCommServer[id]);
		}
	}
}

//==============================================================================
/**
 * @brief   DS�f�[�^����
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _dataDsStep( MPDSContext *DSContext )
{
	u8 i;
	s32 result;
	ShareData sendData;
	MPDSDataSet recvDataSet;
	
	(void)NETMemSet( &sendData, 0, sizeof(sendData) );

	// ���̃X���b�h�Ƌ��L����O���[�o���ϐ��ɐG��ۂɂ͔r�����s��
	EnterCriticalSection(); // �r���J�n

	//if(_pComm->bDSSendOK)
	{
		if(_sendDataSet(TRUE))
		{
			// �������K�v����@@OO
		}
	}

	sendData.data[_DS_HEADERNO] = _INVALID_HEADER;
	result = MPDSTryStep( DSContext, _pComm->sSendBuf, &recvDataSet );

	if( result == MP_RESULT_OK )
	{
		_setSendCallBack( _SEND_CB_FIRST_SENDEND );
		_pComm->bDSSendOK=TRUE;
		// ���L�f�[�^�̓��e�����߂���
		for ( i = 0; i < MY_MAX_NODES+1; i++ )
		{
			u8* adr = (u8*)MPDSGetData( DSContext, &recvDataSet, (u32)i );
			if ( adr != NULL )
			{
				// i �Ԗڂ̋��L���肩��̃f�[�^�����L�ł���
				u8 *pHeader;
				pHeader = adr;
				adr += _DS_HEADER_MAX;
				if(pHeader[_DS_HEADERNO] == _INVALID_HEADER){
					continue;
				}
				if((_pComm->bFirstCatch[i]) && (pHeader[_DS_HEADERNO] & _SEND_NEXT)){ // �܂������f�[�^������������Ƃ��Ȃ���ԂȂ̂ɘA���f�[�^������
					NET_PRINT("�A���f�[�^ parent %d \n",i);
					continue;
				}
				{//_DS_CRC_MAX
					u16 crc = GFL_STD_CrcCalc(&pHeader[_DS_CRC_MAX], MY_DS_LENGTH-_DS_CRC_MAX);
					GF_ASSERT(pHeader[_DS_CRCNO1] == (crc & 0xff));
					GF_ASSERT(pHeader[_DS_CRCNO2] == ((crc & 0xff00) >> 8));
				}
				//NET_PRINT("RING %d %d\n",i,GFL_NET_RingDataSize(&_pComm->recvServerRing[i]));
				//DEBUG_DUMP((u8*)adr, MY_DS_LENGTH - _DS_HEADER_MAX ,"DB");
				GFL_NET_RingPuts(&_pComm->recvServerRing[i], adr, MY_DS_LENGTH - _DS_HEADER_MAX);

				_pComm->bFirstCatch[i] = FALSE;
				
			}
		}
	}
	LeaveCriticalSection(); // �r���I��
	
/*
	int index = 0;
	int mcSize;
	int machineMax;


	if (WH_GetSystemState() != WH_SYSSTATE_DATASHARING){
		return;
	}
	if(_pComm->bDSSendOK){
		if(_sendDataSet(TRUE)){
			// �������K�v����@@OO
		}
	}
	if (WH_StepDS(_pComm->sSendBuf) == TRUE) {  //������ꂽ
		_setSendCallBack( _SEND_CB_FIRST_SENDEND );
		//	   _pComm->countSendRecv++;  //wifi client
		_pComm->bDSSendOK=TRUE;
		{
			mcSize = _getUserMaxSendByte();
			machineMax = _getUserMaxNum();
			for(index = 0; index < machineMax ; index++){ // ��M�R�[���o�b�N
				u8 *pHeader;
				u8* adr = (u8*)WH_GetSharedDataAdr(index);
				if(adr==NULL){
					continue;
				}
				pHeader = adr;
				adr += _DS_HEADER_MAX;
				if(pHeader[_DS_HEADERNO] == _INVALID_HEADER){
					continue;
				}
				if((_pComm->bFirstCatch[index]) && (pHeader[_DS_HEADERNO] & _SEND_NEXT)){ // �܂������f�[�^������������Ƃ��Ȃ���ԂȂ̂ɘA���f�[�^������
					NET_PRINT("�A���f�[�^ parent %d \n",index);
					continue;
				}
				{//_DS_CRC_MAX
					u16 crc = GFL_STD_CrcCalc(&pHeader[_DS_CRC_MAX], mcSize-_DS_CRC_MAX);
					GF_ASSERT(pHeader[_DS_CRCNO1] == (crc & 0xff));
					GF_ASSERT(pHeader[_DS_CRCNO2] == ((crc & 0xff00) >> 8));
				}
				NET_PRINT("RING %d %d\n",index,GFL_NET_RingDataSize(&_pComm->recvServerRing[index]));
				DEBUG_DUMP((u8*)adr, mcSize - _DS_HEADER_MAX ,"DB");
				GFL_NET_RingPuts(&_pComm->recvServerRing[index], adr, mcSize - _DS_HEADER_MAX);

				_pComm->bFirstCatch[index] = FALSE;
			}
		}
	}
	else{  //�����ł��Ȃ�����=�󂯎��ĂȂ�
		_pComm->bDSSendOK=FALSE;
		//		NET_PRINT("DS����FALSE\n");
	}
	*/
}


//==============================================================================
/**
 * @brief   ����f�[�^�̃Z�b�g���t���O�����čs��
 * @param   BOOL   bDataShare �f�[�^���Ȃ��ꍇ����̃f�[�^�𑗂铯���ʐM�̏ꍇ
 * @retval  TRUE   ���M�������ł���
 * @retval  FALSE  ���M�������ł��Ȃ�����
 */
//==============================================================================

static BOOL _sendDataSet(BOOL bDataShare)
{
	BOOL bSend = FALSE;

	if(_sendCallBack != _SEND_CB_FIRST_SENDEND){  // 1�����������M�������Ă��Ȃ�
		//NET_PRINT("�����Ƃ��ĂȂ� %d _sendCallBack\n",_sendCallBack);
		return FALSE;
	}
	if(bDataShare){
		if( _pComm->countSendRecv > _SENDRECV_LIMIT ){  //���肷��
			NET_PRINT("�e�̓����҂�\n");
			return FALSE;
		}
		_setSendDataSystem(_pComm->sSendBuf);  // ����f�[�^�������O�o�b�t�@���獷���ւ���
		_setSendCallBack( _SEND_CB_NONE );
		bSend=TRUE;
	}
	else{
		if(_setSendDataSystem(_pComm->sSendBuf)){  //����f�[�^�����݂���Ƃ������t���O�𗧂Ă�
			_setSendCallBack( _SEND_CB_NONE );
			bSend=TRUE;
		}
	}
	return bSend;
}

//==============================================================================
/**
 * @brief   ���M�L���[�ɂ��������̂𑗐M�o�b�t�@�ɓ����
 * @param   none
 * @retval  none
 */
//==============================================================================

static BOOL _setSendDataSystem(u8* pSendBuff)
{
	int i;
	int mcSize = _getUserMaxSendByte();
	GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
	
	if(_pComm->bNextSendData == FALSE){  // ���ő����ꍇ
		pSendBuff[_DS_HEADERNO] = _SEND_NONE;
	}
	else{
		pSendBuff[_DS_HEADERNO] = _SEND_NEXT;  // ���ő���Ȃ��ꍇ
	}
	_pComm->bNextSendData = FALSE;
	if(GFL_NET_QueueIsEmpty(&_pComm->sendQueueMgr) 
		|| (pInit->bNetType==GFL_NET_TYPE_IRC || pInit->bNetType==GFL_NET_TYPE_IRC_WIRELESS) )
	{
		pSendBuff[_DS_HEADERNO] = _INVALID_HEADER;  // ����ۂȂ牽������Ȃ�
		return FALSE;  // ������̂���������
	}
	else
	{
		SEND_BUFF_DATA buffData;
		buffData.size = mcSize - _DS_HEADER_MAX;
		buffData.pData = &pSendBuff[_DS_HEADER_MAX];
		if(!GFL_NET_QueueGetData(&_pComm->sendQueueMgr, &buffData)){
			_pComm->bNextSendData = TRUE;
		}
		GFL_NET_QueueInc(&_pComm->sendQueueMgr, &pSendBuff[_DS_INCNO]);
		{
			u16 crc = GFL_STD_CrcCalc(&pSendBuff[_DS_CRC_MAX], mcSize - _DS_CRC_MAX);
			pSendBuff[_DS_CRCNO1] = crc & 0xff;
			pSendBuff[_DS_CRCNO2] = (crc & 0xff00) >> 8;
		}
		DEBUG_DUMP((u8*)pSendBuff, buffData.size ,"Pkt");
		NET_PRINT("���M\n");
	}
	return TRUE;
}

#ifdef GFL_NET_DEBUG

//==============================================================================
/**
 * @brief   �f�o�b�O�p�Ƀ_���v��\������
 * @param   adr		   �\���������A�h���X
 * @param   length		����
 * @param   pInfoStr	  �\�����������b�Z�[�W
 * @retval  �T�[�r�X�ԍ�
 */
//==============================================================================

void GFL_NET_SystemDump_Debug(u8* adr, int length, char* pInfoStr)
{
	int i,j = 0;

	NET_PRINT("%s \n",pInfoStr);
	while(length){
		NET_PRINT(">> ");
		for(i = 0 ; i < 8 ; i++){
			NET_PRINT("%2x ",adr[j]);
			j++;
			if(j == length){
				break;
			}
		}
		NET_PRINT("\n");
		if(j == length){
			break;
		}
	}
	NET_PRINT(" --end\n");
}

#endif


//==============================================================================
/**
 * @brief   �����̋@��ID��Ԃ�
 * @param
 * @retval  �����̋@��ID  �Ȃ����Ă��Ȃ��ꍇGFL_NET_PARENT_NETID
 */
//==============================================================================

u16 GFL_NET_SystemGetCurrentID(void)
{
	//�e�@�Ȃ̂�0�ł���
	return 0;
}

//==============================================================================
/**
 * @brief   �G���[�ɂ���ꍇTRUE
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemSetError(void)
{
	_pComm->bError=TRUE;
}


//==============================================================================
/**
 * @brief   ���M���\�b�h
 * @param   command	comm_sharing.h�ɒ�`�������x��
 * @param   data	   ���M�������f�[�^ �Ȃ�����NULL
 * @param   size	   ���M��	�R�}���h�����̏ꍇ0
 * @param   bFast	   ���M��	�R�}���h�����̏ꍇ0
 * @param   myID	   ���M����l�̃n���h��no
 * @param   sendBit	 ���鑊���BIT   �S���Ȃ�0xff
 * @retval  ���M�L���[�ɓ��������ǂ���
 */
//==============================================================================

BOOL GFL_NET_SystemSendData(int command, const void* data, int size, BOOL bFast, int myID, int sendBit, BOOL bSendBuffLock)
{
	int bSave = !bSendBuffLock;
	int cSize;  // = GFI_NET_COMMAND_GetPacketSize(command);
	SEND_QUEUE_MANAGER* pMgr;
	GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();

	DEBUG_DUMP((u8*)data, size ,"pkt");

	if(_pComm==NULL){
		return FALSE;
	}
	cSize = size;
	if(cSize >= _pComm->packetSizeMax){  // �傫���R�}���h�͕ۑ����Ȃ� SendDataEx���g���K�v������
#if PM_DEBUG
		GF_ASSERT(0);
#endif
		bSave = FALSE;
	}
	if((pInit->bMPMode) && (myID == GFL_NET_NETID_SERVER)){
		pMgr = &_pComm->sendQueueMgrServer;
	}
	else{
		pMgr = &_pComm->sendQueueMgr;
	}

#if GFL_IRC_DEBUG
	OS_TPrintf("aaa que put size = %d, fast = %d, myID = %d\n", cSize, bFast, myID);
	{
		int i;
		u8 *buf;
		buf = (u8*)data;
	//	OS_TPrintf("que buf print\n");
	//	for(i = 0; i < cSize; i++){
	//		OS_TPrintf("%d, ", buf[i]);
	//	}
	//	OS_TPrintf("\n");
	}
#endif
	if(GFL_NET_QueuePut(pMgr, command, (u8*)data, cSize, bFast, bSave, sendBit)){
		return TRUE;
	}
	NET_PRINT("-�L���[����-");
	GFL_NET_SystemSetError();
	return FALSE;
}


//==============================================================================
/**
 * @brief   ����̃R�}���h�𑗐M���I�������ǂ����𒲂ׂ�
 * @param   command ���ׂ�R�}���h
 * @retval  �R�}���h���݂�����TRUE
 */
//==============================================================================


BOOL GFL_NET_SystemIsSendCommand(int command,int myID)
{
	GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
	if((pInit->bMPMode) && (myID == 0)){
		return GFL_NET_QueueIsCommand(&_pComm->sendQueueMgrServer, command);
	}
	return GFL_NET_QueueIsCommand(&_pComm->sendQueueMgr, command);

}

//==============================================================================
/**
 * @brief   ���M�o�b�t�@�c��e��
 * @retval  �T�C�Y
 */
//==============================================================================

int GFL_NET_SystemGetSendRestSize(void)
{
	return GFL_NET_RingDataRestSize(&_pComm->sendRing);
}

//==============================================================================
/**
 * @brief   �T�[�o���̑��M�o�b�t�@�c��e��
 * @retval  �T�C�Y
 */
//==============================================================================

int GFL_NET_SystemGetSendRestSize_ServerSide(void)
{
	return GFL_NET_RingDataRestSize(&_pComm->sendServerRing);
}


//==============================================================================
/**
 * @brief   �ʐM�\��ԂȂ̂��ǂ�����Ԃ� �������R�}���h�ɂ��l�S�V�G�[�V�������܂��̏��
 * @param   �e�q�@��netID
 * @retval  TRUE  �ʐM�\	FALSE �ʐM�ؒf
 */
//==============================================================================
BOOL GFL_NET_SystemIsConnect(u16 netID)
{
	//�����炭�Ǝ��������K�v�E�E�E
	return TRUE;
/*
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

	if(!_pComm){
		return FALSE;
	}
	if( pNetIni->bNetType == GFL_NET_TYPE_WIFI ){
#if GFL_NET_WIFI
		//#if 0 //wifi
		if(_pComm->bWifiConnect){
			int id = mydwc_getaid();
			if(-1 != id){
				if(netID == 0){
					return TRUE;
				}
				else if(netID == 1){
					return TRUE;
				}
			}
		}
#endif
		return FALSE;
	}
	
	if(pNetIni->bNetType == GFL_NET_TYPE_IRC || pNetIni->bNetType == GFL_NET_TYPE_IRC_WIRELESS){
#if GFL_NET_IRC
		if(GFL_NET_IRC_IsConnect() == TRUE){
			return TRUE;
		}
#endif
		return FALSE;
	}
	
	if(!GFL_NET_SystemIsInitialize()){
		return FALSE;
	}
	if (!GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID())) {
		return FALSE;
	}
	if(GFL_NET_SystemGetCurrentID()==netID){// ������ONLINE
		return TRUE;
	}
	else if(GFL_NET_SystemGetCurrentID()==GFL_NET_NO_PARENTMACHINE){  // �e�@�̂ݎq�@����LIB�œ�����
		u16 bitmap = GFL_NET_WL_GetBitmap();
		if( bitmap & (1<<netID)){
			return TRUE;
		}
	}
	else if(_pComm->bitmap & (1<<netID)){
		return TRUE;
	}
	return FALSE;
	*/
}

#pragma mark [>OldFuncList
#if 0
//==============================================================================
// static�錾
//==============================================================================

static void _dataMpStep(void);
static void _dataDsStep(void);

static void _dataMpServerStep(void);
static void _sendCallbackFunc(BOOL result);
static void _sendServerCallback(BOOL result);
static void _commRecvCallback(u16 aid, u16 *data, u16 size);
static void _commRecvParentCallback(u16 aid, u16 *data, u16 size);

static void _recvDataFunc(void);
static void _setSendDataServer(u8* pSendBuff);

static BOOL _sendQueueSet(void);
static BOOL _sendServerQueueSet(void);
static void _queueSet(int restSize);
static void _queueSetServer(int restSize);
static void _spritDataSendFunc(void);

static void _commRecvIrcCallback(u16 aid, u16 *data, u16 size);
static void _dataIrcStep(void);
static void _update_irc_data(void *data, int size);



//==============================================================================
/**
 * @brief   �w�肳�ꂽ�q�@�̗̈���N���A�[����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _clearChildBuffers(int netID)
{
	//	_pComm->recvDSCatchFlg[netID] = 0;  // �ʐM������������Ƃ��L�� DS�����p
	GF_ASSERT(netID < GFL_NET_MACHINE_MAX);

	_pComm->bFirstCatch[netID] = TRUE;  // �R�}���h���͂��߂Ă���������p
	_pComm->countSendRecvServer[netID] = 0;  // SERVER��M

	GFL_NET_RingInitialize(&_pComm->recvServerRing[netID],
						   &_pComm->pServerRecvBufRing[netID * _pComm->packetSizeMax],
						   _pComm->packetSizeMax);

	_pComm->recvCommServer[netID].valCommand = GFL_NET_CMD_NONE;
	_pComm->recvCommServer[netID].realSize = 0xffff;
	_pComm->recvCommServer[netID].tblSize = 0xffff;
	_pComm->recvCommServer[netID].pRecvBuff = NULL;
	_pComm->recvCommServer[netID].dataPoint = 0;
}

//==============================================================================
/**
 * @brief   �ڑ����̏���
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _connectFunc(void)
{
	int i;

	for(i = 0 ; i < GFL_NET_MACHINE_MAX ; i++){
		if((!GFL_NET_SystemIsConnect(i)) && !_pComm->bFirstCatch[i]){
			if(!GFL_NET_SystemGetAloneMode()){
				NET_PRINT("��ڑ��ɂȂ������� %d\n",i);
				_clearChildBuffers(i);  // ��ڑ��ɂȂ�������
			}
		}
	}

}

#if GFL_NET_WIFI

//==============================================================================
/**
 * @brief   WiFiGame�̏��������s��
 * @param   packetSizeMax �p�P�b�g�̃T�C�Y�}�b�N�X
 * @param   regulationNo  �Q�[���̎��
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemWiFiModeInit(int packetSizeMax, HEAPID heapIDSys, HEAPID heapIDWifi)
{
	BOOL ret = TRUE;
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

	if(!GFL_NET_IsInitIchneumon()){
		return FALSE;
	}



	//sys_CreateHeapLo( HEAPID_BASE_APP, heapIDWifi, _HEAPSIZE_WIFI);
	_commSystemInit(packetSizeMax, heapIDSys);
	mydwc_startConnect( GFI_NET_GetMyDWCUserData() ,GFI_NET_GetMyDWCFriendData(),
						heapIDWifi, pNetIni->maxBeaconNum);
	//	mydwc_setFetalErrorCallback(CommFatalErrorFunc);   //@@OO �G���[�����ǉ��K�v 07/02/22
	mydwc_setReceiver( _commRecvParentCallback, _commRecvCallback );
	//	GFL_NET_SystemSetTransmissonTypeDS();
	return TRUE;
}

//==============================================================================
/**
 * @brief   wifi�̃Q�[�����J�n����
 * @param   target:   ���Ȃ�e�A�O�ȏ�Ȃ�Ȃ��ɂ����e�@�̔ԍ�
 * @retval  TRUE	  ����
 * @retval  FALSE	 ���s
 */
//==============================================================================
int GFL_NET_SystemWifiApplicationStart( int target )
{
	if( mydwc_getFriendStatus(target) != DWC_STATUS_MATCH_SC_SV ){
		return FALSE;
	}
	//	mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );
	return TRUE;
}

#endif //GFL_NET_WIFI

#ifdef GFL_NET_IRC
static void _irc_commRecvCallback(u16 aid, u16 *data, u16 size)
{
	_commRecvIrcCallback(aid, data, size);
#if 0
	if(GFL_NET_IRC_IsSender() == TRUE){
		//�e�@�Ƃ��Ă̎�M����
		_commRecvParentCallback(aid, data, size);
	}
	else{
		//�q�@�Ƃ��Ă̎�M����
		_commRecvCallback(aid, data, size);
	}
#endif
}

//==============================================================================
/**
 * @brief   IRC�ʐM�V�X�e���̏��������s��
 * @param   bTGIDChange	�V�K�̃Q�[���̏������̏ꍇTRUE �Â��r�[�R���ł̌듮���h�����ߗp
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemIrcModeInit(BOOL bChangeTGID)
{
	GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();

	pInit->bTGIDChange = bChangeTGID;
	GFL_NET_IRC_RecieveFuncSet(_irc_commRecvCallback);
	_commSystemInit(GFL_NET_IRC_SEND_MAX, pInit->netHeapID);
	_clearChildBuffers(0);	//��check�@�b���3���Œ�ŃN���A�B
	_clearChildBuffers(1);	//			�{���Ȃ�ڑ��������ɐڑ����ꂽ�q�@�����N���A����
	_clearChildBuffers(2);

//	_setSendCallBack( _SEND_CB_NONE );
	return TRUE;
}
#endif	//GFL_NET_IRC


//==============================================================================
/**
 * @brief   �e�@�̏��������s��
 * @param   id
 * @retval  none
 */
//==============================================================================
static void _connectCallBackP(int id)
{
	GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
	GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

	_clearChildBuffers(id);
	if(pInit->connectHardFunc){  // �R�[���o�b�N�Ăяo��
		pInit->connectHardFunc(pNet->pWork, id);
	}
}

//==============================================================================
/**
 * @brief   �e�@�̏��������s��
 * @param   id
 * @retval  none
 */
//==============================================================================
static void _connectCallBackC(int id)
{
	GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
	GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

	if(pInit->connectHardFunc){  // �R�[���o�b�N�Ăяo��
		pInit->connectHardFunc(pNet->pWork, id);
	}
}

//==============================================================================
/**
 * @brief   �e�@�̏��������s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *					  NULL�̏ꍇ���łɏ������ς݂Ƃ��ē���
 * @param   bTGIDChange	�V�K�̃Q�[���̏������̏ꍇTRUE �Â��r�[�R���ł̌듮���h�����ߗp
 * @param   packetSizeMax  �傫���p�P�b�g������ꍇ�A�������[���傫���Ȃ�
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFI_NET_SystemParentModeInit(BOOL bChangeTGID, int packetSizeMax)
{
	GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
	BOOL bRet = FALSE;

	pInit->bTGIDChange = bChangeTGID;
	// WH_ParentDataInit();
	WHSetConnectCallBack(_connectCallBackP);
	WH_SetReceiver(_commRecvParentCallback);
	_commSystemInit(packetSizeMax, _GFL_NET_GetNETInitStruct()->netHeapID);
	if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
		if(WH_StartMeasureChannel()){
			bRet = TRUE;
		}
	}
	return bRet;
}

//==============================================================================
/**
 * @brief   �e�@�̊m�����s��
 * @param   channelChange  �e�@�̃`�����l����ύX����ꍇTRUE ��{�͕ύX��
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFI_NET_SystemParentModeInitProcess(BOOL channelChange)
{
	return GFL_NET_WL_ParentConnect(channelChange);
}


//==============================================================================
/**
 * @brief   �q�@�̏��������s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *					  NULL�̏ꍇ�͂��łɏ������ς݂Ƃ��Ĉ���
 * @param   regulationNo  �Q�[���̎��
 * @param   bBconInit  �r�[�R���f�[�^������������̂��ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemChildModeInit(BOOL bBconInit, int packetSizeMax)
{
	BOOL ret = TRUE;

	WHSetConnectCallBack(_connectCallBackC);
	ret = GFL_NET_WLChildInit(bBconInit);
	if(ret==TRUE){
		GFL_NET_WLSetRecvCallback( _commRecvCallback );
		_commSystemInit(packetSizeMax, _GFL_NET_GetNETInitStruct()->netHeapID);
		_setSendCallBack( _SEND_CB_FIRST_SENDEND );
	}
	return ret;
}

//==============================================================================
/**
 * @brief   �q�@�̏��������s��+ �ڑ��ɂ����s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *					  NULL�̏ꍇ�͂��łɏ������ς݂Ƃ��Ĉ���
 * @param   regulationNo  �Q�[���̎��
 * @param   bBconInit  �r�[�R���f�[�^������������̂��ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemChildModeInitAndConnect(BOOL bInit,u8* pMacAddr,int packetSizeMax,_PARENTFIND_CALLBACK pCallback)
{
	BOOL ret = TRUE;
	GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
	u16 mode[]={WH_CONNECTMODE_DS_CHILD,WH_CONNECTMODE_MP_CHILD};

	GFL_NET_WLSetRecvCallback( _commRecvCallback );
	if (WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
		(void)WH_EndScan();
		return FALSE;
	}
	else if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
		if(bInit){
			_commSystemInit(packetSizeMax, _GFL_NET_GetNETInitStruct()->netHeapID);
			_setSendCallBack( _SEND_CB_FIRST_SENDEND );
		}
		WH_ChildConnectAuto(mode[pInit->bMPMode], pMacAddr, 0);
		GFI_NET_BeaconSetScanCallback(pCallback);
		return TRUE;
	}
	return FALSE;
}

//==============================================================================
/**
 * @brief   �ʐM�ؒf���s��
 * @param   none
 * @retval  none
 */
//==============================================================================
void GFL_NET_SystemFinalize(void)
{
	BOOL bEnd = FALSE;
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

	if(_pComm){
		if( pNetIni->bNetType == GFL_NET_TYPE_WIFI ){
#if GFL_NET_WIFI
			mydwc_Logout();  // �ؒf
#endif
		}
		else if(pNetIni->bNetType == GFL_NET_TYPE_IRC){
#if GFL_NET_IRC
			IRC_Shutdown();
#endif
		}
		else{
			WH_Finalize();
		}
		NET_PRINT("�ؒf----�J������--\n");
		GFL_HEAP_FreeMemory(_pComm->pRecvBufRing);
		GFL_HEAP_FreeMemory(_pComm->pTmpBuff);
		GFL_HEAP_FreeMemory(_pComm->pServerRecvBufRing);
		GFL_NET_QueueManagerFinalize(&_pComm->sendQueueMgrServer);
		GFL_NET_QueueManagerFinalize(&_pComm->sendQueueMgr);
		GFL_HEAP_FreeMemory(_pComm->sSendBuf);
		GFL_HEAP_FreeMemory(_pComm->sSendBufRing);
		GFL_HEAP_FreeMemory(_pComm->sSendServerBuf);
		GFL_HEAP_FreeMemory(_pComm->sSendServerBufRing);
		GFL_HEAP_FreeMemory(_pComm);
		_pComm = NULL;
	}
}

//==============================================================================
/**
 * @brief   �q�@ index�ڑ�
 * @param   index   �e�̃��X�g��index
 * @retval  �q�@�ڑ���e�@�ɑ�������TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemChildIndexConnect(u16 index,_PARENTFIND_CALLBACK pCallback,GFL_NETHANDLE* pHandle)
{
	return GFL_NET_WLChildIndexConnect(index, pCallback, pHandle);
}


//==============================================================================
/**
 * @brief   �I���R�}���h����M�����烂�[�h�ɉ����ď������s�������ؒf����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _autoExitSystemFunc(void)
{
	if(!GFL_NET_WLIsAutoExit()){
		return;
	}
	if(GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE){   // �������e�̏ꍇ�݂�Ȃɋt�ԐM����
		if(GFL_NET_WLIsChildsConnecting()){
			return;
		}
		GFL_NET_SystemFinalize();  // �I�������ɓ���
	}
	else{   //�q�@�̏ꍇ
		GFL_NET_SystemFinalize();  // �I�������ɓ���
	}
}

//==============================================================================
/**
 * @brief   �ʐM�f�[�^�̍X�V����  �f�[�^�����W
 *	main.c   ����  vblank��ɂ����ɌĂ΂��
 * @param   none
 * @retval  �f�[�^�V�F�A�����O���������Ȃ������ꍇFALSE
 */
//==============================================================================


BOOL GFL_NET_SystemUpdateData(void)
{
	int j;

	if(_pComm != NULL){

		if( _pComm->bWifiConnect ){
#if GFL_NET_WIFI
			GFL_NET_WirelessIconEasy_SetLevel(WM_LINK_LEVEL_3 - DWC_GetLinkLevel());
#endif
		}
		else if(GFL_NET_WLIsInitialize()){
			GFL_NET_WirelessIconEasy_SetLevel(WM_LINK_LEVEL_3 - WM_GetLinkLevel());
		}

		if(!_pComm->bShutDown){
			GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
			if(!pInit->bMPMode && (pInit->bNetType == GFL_NET_TYPE_WIRELESS)){
				_dataDsStep();
				_recvDataServerFunc();  // �T�[�o�[���̎�M����
			}
			else if(pInit->bNetType == GFL_NET_TYPE_IRC || pInit->bNetType == GFL_NET_TYPE_IRC_WIRELESS){
#if GFL_NET_IRC
				_dataIrcStep();
				_recvDataServerFunc();  // �T�[�o�[���̎�M����
				GFL_NET_IRC_CommandContinue();
#endif
			}
			else{
				_dataMpStep();
				_recvDataFunc();	// �q�@�Ƃ��Ă̎󂯎�菈��
				if((GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE) && (GFL_NET_SystemIsConnect(GFL_NET_NO_PARENTMACHINE)) || GFL_NET_SystemGetAloneMode() ){
					// �T�[�o�[�Ƃ��Ă̏���
					_dataMpServerStep();
				}
				if((GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE) || GFL_NET_SystemGetAloneMode() ){
					_recvDataServerFunc();  // �T�[�o�[���̎�M����
				}
			}
		}
		CommMpProcess(_pComm->bitmap);
		if(GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE){
			_connectFunc();
		}
		_autoExitSystemFunc();  // �����ؒf _pComm=NULL�ɂȂ�̂Œ���
	}
	else{
		CommMpProcess(0);
	}
	return TRUE;
}


//==============================================================================
/**
 * @brief   �ʐM�o�b�t�@���N���A�[����
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemReset(void)
{
	if(_pComm){
		_commCommandInit();
	}
}

//==============================================================================
/**
 * @brief   DS���[�h�ŒʐM�o�b�t�@���N���A�[����
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemResetDS(void)
{
	if(_pComm){
		_commCommandInit();
	}
}

//==============================================================================
/**
 * @brief   �ʐM�o�b�t�@���N���A�[����+�r�[�R���̏��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemResetBattleChild(void)
{
	if(_pComm){
		_commCommandInit();
		GFL_NET_WLChildBconDataInit();
	}
}


//==============================================================================
/**
 * @brief   �f�[�^�̎��W
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _updateMpData(void)
{
	u16 i;
	u8 *adr;
	int size;

	{
		int mcSize = _getUserMaxSendByte();
		if(GFL_NET_SystemGetAloneMode()){   // alone���[�h�̏ꍇ
			if(_sendCallBack == _SEND_CB_NONE){
				_setSendCallBack( _SEND_CB_FIRST_SEND );
				_sendCallbackFunc(TRUE);
				// �q�@�̂ӂ�����镔��		  // �e�@�͎����ŃR�[���o�b�N���Ă�
				_commRecvParentCallback(GFL_NET_NO_PARENTMACHINE, (u16*)_pComm->sSendBuf,
										mcSize);
				//_pComm->sendSwitch = 1 - _pComm->sendSwitch;
				_pComm->countSendRecv++; // MP���M�e
				return;
			}
		}
		if(GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID())){
			if(!GFL_NET_SystemIsConnect(GFL_NET_SystemGetCurrentID())){
				return;
			}
			if(_sendCallBack == _SEND_CB_NONE){
				// �q�@�f�[�^���M
				if(GFL_NET_SystemGetCurrentID() != GFL_NET_NO_PARENTMACHINE){
					if(!GFL_NET_WL_SendData(_pComm->sSendBuf,
											mcSize, _sendCallbackFunc)){
						NET_PRINT("failed WH_SendData\n");
					}
					else{
						_setSendCallBack( _SEND_CB_FIRST_SEND );
						_pComm->countSendRecv++; // MP���M
					}
				}
				else{		// �T�[�o�[�Ƃ��Ă̏��� �����ȊO�̒N���ɂȂ����Ă��鎞
					_setSendCallBack( _SEND_CB_FIRST_SEND );
					_sendCallbackFunc(TRUE);
					// �q�@�̂ӂ�����镔��		  // �e�@�͎����ŃR�[���o�b�N���Ă�
					_commRecvParentCallback(GFL_NET_NO_PARENTMACHINE, (u16*)_pComm->sSendBuf,
											mcSize);
					_pComm->countSendRecv++; // MP���M
				}
			}
		}
	}
}

//==============================================================================
/**
 * @brief   DS�f�[�^����
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _dataDsStep(void)
{
	int index = 0;
	int mcSize;
	int machineMax;


	if (WH_GetSystemState() != WH_SYSSTATE_DATASHARING){
		return;
	}
	if(_pComm->bDSSendOK){
		if(_sendDataSet(TRUE)){
			// �������K�v����@@OO
		}
	}
	if (WH_StepDS(_pComm->sSendBuf) == TRUE) {  //������ꂽ
		_setSendCallBack( _SEND_CB_FIRST_SENDEND );
		//	   _pComm->countSendRecv++;  //wifi client
		_pComm->bDSSendOK=TRUE;
		{
			mcSize = _getUserMaxSendByte();
			machineMax = _getUserMaxNum();
			for(index = 0; index < machineMax ; index++){ // ��M�R�[���o�b�N
				u8 *pHeader;
				u8* adr = (u8*)WH_GetSharedDataAdr(index);
				if(adr==NULL){
					continue;
				}
				pHeader = adr;
				adr += _DS_HEADER_MAX;
				if(pHeader[_DS_HEADERNO] == _INVALID_HEADER){
					continue;
				}
				if((_pComm->bFirstCatch[index]) && (pHeader[_DS_HEADERNO] & _SEND_NEXT)){ // �܂������f�[�^������������Ƃ��Ȃ���ԂȂ̂ɘA���f�[�^������
					NET_PRINT("�A���f�[�^ parent %d \n",index);
					continue;
				}
				{//_DS_CRC_MAX
					u16 crc = GFL_STD_CrcCalc(&pHeader[_DS_CRC_MAX], mcSize-_DS_CRC_MAX);
					GF_ASSERT(pHeader[_DS_CRCNO1] == (crc & 0xff));
					GF_ASSERT(pHeader[_DS_CRCNO2] == ((crc & 0xff00) >> 8));
				}
				NET_PRINT("RING %d %d\n",index,GFL_NET_RingDataSize(&_pComm->recvServerRing[index]));
				DEBUG_DUMP((u8*)adr, mcSize - _DS_HEADER_MAX ,"DB");
				GFL_NET_RingPuts(&_pComm->recvServerRing[index], adr, mcSize - _DS_HEADER_MAX);

				_pComm->bFirstCatch[index] = FALSE;
			}
		}
	}
	else{  //�����ł��Ȃ�����=�󂯎��ĂȂ�
		_pComm->bDSSendOK=FALSE;
		//		NET_PRINT("DS����FALSE\n");
	}
}

//==============================================================================
/**
 * @brief   DS�f�[�^����(�ԊO���p)
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _dataIrcStep(void)
{
#if GFL_NET_IRC
	GFL_NET_IRC_Move();
	if(GFL_NET_IRC_IsConnectSystem() == TRUE){
		int mcSize = _getUserMaxSendByte();
		
		if(GFL_NET_IRC_SendCheck() == TRUE){
			if(_sendDataSet(FALSE)){
				_update_irc_data(_pComm->sSendBuf, mcSize);
				_setSendCallBack( _SEND_CB_FIRST_SENDEND );
			//?	_pComm->countSendRecv++;
			}
		}
	}
#endif
}

//==============================================================================
/**
 * @brief   �ʐM����M�������ɌĂ΂��R�[���o�b�N IRC�p
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================
static void _commRecvIrcCallback(u16 aid, u16 *data, u16 size)
{
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();
	u8 *pHeader = (u8*)data;
	u8* adr = ((u8*)data) + _DS_HEADER_MAX;
	int i,ringRestSize,ringRestSize2;
	int mcSize = _getUserMaxSendByte();
	RingBuffWork* pRing;
	int index = aid;// - 1;
	
//	GF_ASSERT(aid != 0);
	
#if GFL_IRC_DEBUG
	{
		int i;
		u8 *buf;
		buf = (u8*)data;
		OS_TPrintf("aaa irc recieve buf print : aid=%d,index=%d,size=%d\n", aid, index, size);
		for(i = 0; i < size; i++){
			OS_TPrintf("%d, ", buf[i]);
		}
		OS_TPrintf("\n");
	}
#endif

	if(pHeader[_DS_HEADERNO] == _INVALID_HEADER){
		return;
	}

//?	_pComm->countSendRecvServer[aid]--;  //SERVER��M

	if((_pComm->bFirstCatch[index]) && (pHeader[_DS_HEADERNO] & _SEND_NEXT)){ // �܂������f�[�^������������Ƃ��Ȃ���ԂȂ̂ɘA���f�[�^������
		NET_PRINT("�A���f�[�^ parent %d \n",index);
		return;
	}
	{//_DS_CRC_MAX
		u16 crc = GFL_STD_CrcCalc(&pHeader[_DS_CRC_MAX], mcSize-_DS_CRC_MAX);
		GF_ASSERT(pHeader[_DS_CRCNO1] == (crc & 0xff));
		GF_ASSERT(pHeader[_DS_CRCNO2] == ((crc & 0xff00) >> 8));
	}
	GFL_NET_RingPuts(&_pComm->recvServerRing[index], adr, mcSize - _DS_HEADER_MAX);


	_pComm->bFirstCatch[index] = FALSE;  //�����M�t���O��OFF����
}

//--------------------------------------------------------------
/**
 * @brief   IRC:���M�o�b�t�@�̃f�[�^�𑗐M
 *
 * @param   none		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void _update_irc_data(void *data, int size)
{
	int i;
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

	if(GFL_NET_IRC_SendTurnGet() == FALSE || GFL_NET_IRC_IsConnectSystem() == FALSE){
		GF_ASSERT(0);
		return;
	}
	
	if(_sendCallBack == _SEND_CB_NONE){
		// �q�@�f�[�^���M
		IRC_PRINT("IRC Send : size=%d\n", size);
	#if 0
		if(GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE){
			//���M�҂��e�@�̏ꍇ�́A�q�@�֒��ڑ��M�����ɁA�e�@��M�R�[���o�b�N�։񂵁A
			//�e�@�̑��M�f�[�^�Ƃ��Ďq�@�֗���
			_commRecvIrcCallback(GFL_NET_PARENT_NETID+1, data, size);
//			_commRecvParentCallback(GFL_NET_PARENT_NETID+1, data, size);
//			_commRecvCallback(GFL_NET_PARENT_NETID, data, size);
		}
		else{
			GFL_NET_IRC_Send(data, size, 0);
		}
	#else
		GFL_NET_IRC_Send(data, size, 0);
		//���M�����������e�@����ēx�z�M���ꂽ�����U������ׁA�����Ŏ�M���Ăяo��
		_commRecvIrcCallback(GFL_NET_IRC_System_GetCurrentAid(), data, size);
	#endif
		_sendServerCallBack = _SEND_CB_FIRST_SENDEND;
//?		_pComm->countSendRecv++; // MP���M

		//_commRecvCallback(GFL_NET_IRC_GetCurrentAid(), data, size);
	}
}


//==============================================================================
/**
 * @brief   �f�[�^���M����
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _dataMpStep(void)
{
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

	if( pNetIni->bNetType == GFL_NET_TYPE_WIFI ){
#if GFL_NET_WIFI
		if( _pComm->bWifiConnect ){
			int mcSize = _getUserMaxSendByte();

			if(mydwc_canSendToServer()){
				if(_sendDataSet(_pComm->bWifiSendRecv)){
					if( mydwc_sendToServer( _pComm->sSendBuf, mcSize )){
						_setSendCallBack( _SEND_CB_FIRST_SENDEND );
						_pComm->countSendRecv++;  //wifi client
					}
				}
			}
		}
#endif
	}
	else if(((GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID())) &&
			 (GFL_NET_SystemIsConnect(GFL_NET_SystemGetCurrentID()))) || GFL_NET_SystemGetAloneMode()){
		_sendDataSet(TRUE);
		_updateMpData();	 // �f�[�^����M
	}
}

//==============================================================================
/**
 * @brief   ����RINGBUFF����q�@�S���ɋt���M���邽��buff�ɃR�s�[
 * @param   none
 * @retval  ��������TRUE
 */
//==============================================================================
/*
static volatile int _debugCounter = 0;

static BOOL _copyDSData(void)
{
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();
	int mcSize;
	int machineMax;
	int i,num,bSend = FALSE;
	u8* pBuff;

	mcSize = _getUserMaxSendByte();
	machineMax = _getUserMaxNum();

	/// ����RINGBUFF����q�@�S���ɋt���M���邽��buff�ɃR�s�[
	for(i = 0; i < machineMax; i++){
//		GFL_NET_RingEndChange(&_pComm->recvMidRing[i]);
		pBuff = &_pComm->sSendServerBuf[_MP_DATA_HEADER_POS + (i * mcSize)];
		pBuff[0] = _INVALID_HEADER;
		num = GFL_NET_RingGets(&_pComm->recvMidRing[i] ,
							   pBuff,
							   mcSize);
		if(pBuff[0] != _INVALID_HEADER){
			bSend = TRUE;
		}
		if(i==0){
			_pComm->sSendServerBuf[2] = num;
		}
		if(i==1){
			_pComm->sSendServerBuf[3] = num;
		}
	}
	if(bSend == FALSE){
		_pComm->sSendServerBuf[0] = _INVALID_HEADER;
		return FALSE;
	}
	_debugCounter++;
	_pComm->sSendServerBuf[0] = _SEND_NONE + (_debugCounter * 0x10);
	if( pNetIni->bNetType){
#if GFL_NET_WIFI
		_pComm->sSendServerBuf[1] = mydwc_GetBitmap();
#endif
	}
	else{
		_pComm->sSendServerBuf[1] = WH_GetBitmap();
	}
//	OS_TPrintf("_copyDSDataTRU\n");
//	_pComm->sSendServerBuf[2] = mcSize;
	return TRUE;
}
 */

//==============================================================================
/**
 * @brief   �f�[�^���M����  �T�[�o�[��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _updateMpDataServer(void)
{
	int i;
	int debug=0;
	int mcSize ,machineMax;

	machineMax = _getUserMaxNum();

	if((_sendServerCallBack == _SEND_CB_DSDATA_PACK) && (GFL_NET_SystemGetConnectNum()>1)){
		_sendServerCallBack = _SEND_CB_FIRST_SEND;
		if( (GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID()))  && !GFL_NET_SystemGetAloneMode()){
			//DEBUG_DUMP(_pComm->sSendServerBuf,32,"mi");
			if(!GFL_NET_WL_SendData(_pComm->sSendServerBuf,
									_getUserMaxSendByteParent(),
									_sendServerCallback)){
				_sendServerCallBack = _SEND_CB_DSDATA_PACK;
				NET_PRINT("���M���s%d\n",__LINE__);
			}
		}
		else if((_GFL_NET_GetNETInitStruct())->bNetType == GFL_NET_TYPE_IRC 
				|| (_GFL_NET_GetNETInitStruct())->bNetType == GFL_NET_TYPE_IRC_WIRELESS){
			if(GFL_NET_IRC_SendCheck() == FALSE){
				_sendServerCallBack = _SEND_CB_DSDATA_PACK;
				NET_PRINT("���M���s%d\n",__LINE__);
			}
			else{
				int size;
				size = _pComm->sSendServerBuf[_MP_SIZEO1]*256 + _pComm->sSendServerBuf[_MP_SIZEO2];
				if(size == 0){
					return;
				}
				GFL_NET_IRC_Send(
					(u8*)(_pComm->sSendServerBuf), //_getUserMaxSendByteParent(), 
							GFI_NET_GetSendSizeMax(), 100);
			}
		}

		// ���M����
		if((_sendServerCallBack == _SEND_CB_FIRST_SEND) ){

			for(i = 0; i < machineMax; i++){
				if(GFL_NET_SystemIsConnect(i)){
					_pComm->countSendRecvServer[i]++; // �eMP���M
				}
				else if(GFL_NET_SystemGetAloneMode() && (i == 0)){
					_pComm->countSendRecvServer[i]++; // �eMP���M
				}
			}
			// �e�@���g�Ɏq�@�̓����������邽�߂����ŃR�[���o�b�N���Ă�
			_commRecvCallback(GFL_NET_NO_PARENTMACHINE,
							  (u16*)_pComm->sSendServerBuf,
							  _getUserMaxSendByteParent());
		}

		if( !(GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID()))  || GFL_NET_SystemGetAloneMode() || (_GFL_NET_GetNETInitStruct())->bNetType == GFL_NET_TYPE_IRC || (_GFL_NET_GetNETInitStruct())->bNetType == GFL_NET_TYPE_IRC_WIRELESS){
			// ���荞�݂������󋵂ł��������ׂ����ŃJ�E���g
			_sendServerCallBack = _SEND_CB_FIRST_SENDEND;
		}
	}
}

//==============================================================================
/**
 * @brief   �f�[�^�𑗐M�������Ă��Ȃ����ǂ�����������
 * @param   none
 * @retval  none
 */
//==============================================================================

static BOOL _checkSendRecvLimit(void)
{
	int i;

	for(i = 0; i < GFL_NET_MACHINE_MAX; i++){
		if(GFL_NET_SystemIsConnect(i)){
			if(_pComm->countSendRecvServer[i] > _SENDRECV_LIMIT){ // ���M�������̏ꍇ
				return FALSE;
			}
		}
	}
	return TRUE;
}

//==============================================================================
/**
 * @brief   �f�[�^���M����  �T�[�o�[��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _dataMpServerStep(void)
{
	int i;
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

	if( pNetIni->bNetType == GFL_NET_TYPE_WIFI){
#if GFL_NET_WIFI
		if( GFL_NET_SystemIsConnect(GFL_NET_NO_PARENTMACHINE) ){

			GF_ASSERT(!pNetIni->bMPMode);
			if((_sendServerCallBack == _SEND_CB_FIRST_SENDEND) && (mydwc_canSendToClient())){
				if( _pComm->bWifiSendRecv ){  // ����������Ă���ꍇ
					if(_checkSendRecvLimit()){
						//	  _copyDSData();  //DS�ʐM�Ȃ�R�s�[
						_sendServerCallBack = _SEND_CB_DSDATA_PACK;
					}
				}
				else{
					//			  if(_copyDSData()){  //DS�ʐM�Ȃ�R�s�[
					//		   _sendServerCallBack = _SEND_CB_DSDATA_PACK;
					//	  }
				}
			}
			if(_sendServerCallBack==_SEND_CB_DSDATA_PACK){
				if( mydwc_sendToClient( _pComm->sSendServerBuf, _getUserMaxSendByteParent() )){
					OHNO_SP_PRINT("send %d \n",_pComm->sSendServerBuf[0]);
					_sendServerCallBack = _SEND_CB_FIRST_SENDEND;
					_pComm->countSendRecvServer[0]++; // wifi server
					_pComm->countSendRecvServer[1]++; // wifi server
				}
				else{
					NET_PRINT("mydwc_sendToClient�Ɏ��s\n");
				}
			}
		}
#endif  //wifi����
	}
	else if(pNetIni->bNetType == GFL_NET_TYPE_IRC 
			|| pNetIni->bNetType == GFL_NET_TYPE_IRC_WIRELESS){
#if GFL_NET_IRC
		if(_sendServerCallBack == _SEND_CB_FIRST_SENDEND){
			_setSendDataServer(_pComm->sSendServerBuf);  // ����f�[�^�������O�o�b�t�@���獷���ւ���
			_sendServerCallBack = _SEND_CB_DSDATA_PACK;
		}
		// �ŏ��̑��M����
		_updateMpDataServer();
#endif
	}
	else if((GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID())) || (GFL_NET_SystemGetAloneMode()) ){
		if(!_checkSendRecvLimit()){
			return;
		}
		if(_sendServerCallBack == _SEND_CB_FIRST_SENDEND){
			_setSendDataServer(_pComm->sSendServerBuf);  // ����f�[�^�������O�o�b�t�@���獷���ւ���
			_sendServerCallBack = _SEND_CB_DSDATA_PACK;
		}
		// �ŏ��̑��M����
		_updateMpDataServer();
	}
}



//==============================================================================
/**
 * @brief   �ʐM����M�������ɌĂ΂��R�[���o�b�N MP�p
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================
static void _commRecvCallback(u16 aid, u16 *data, u16 size)
{
	u8* adr = (u8*)data;
	u8* adr2 = (u8*)data;
	int i;
	int recvSize = size;
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

	OS_TPrintf("_commRecvCallback %d %d\n",aid, size);

#if GFL_IRC_DEBUG
	{
		int i;
		u8 *buf;
		buf = (u8*)data;
		OS_TPrintf("aaa child recieve buf print\n");
		for(i = 0; i < size; i++){
			OS_TPrintf("%d, ", buf[i]);
		}
		OS_TPrintf("\n");
	}
#endif

	_pComm->countSendRecv--;  //��M
#ifdef PM_DEBUG
	_pComm->countRecvNum++;
#endif
	if(adr==NULL){
		return;
	}
	{
		int mcSize = _getUserMaxSendByte()+4;
		DEBUG_DUMP(&adr[mcSize], 8,"cr1");
		DEBUG_DUMP(&adr[0], 8,"cr0");
	}

	if(adr[0] == _INVALID_HEADER){
		return;
	}

#ifdef WIFI_DUMP_TEST
	DEBUG_DUMP(&adr[0], _SEND_BUFF_SIZE_CHILD,"cr0");
	DEBUG_DUMP(&adr[_SEND_BUFF_SIZE_CHILD], _SEND_BUFF_SIZE_CHILD,"cr1");
#endif

	_pComm->bFirstCatchP2C = FALSE;

	_pComm->bitmap = adr[_MP_CONNECTBIT];


	recvSize = adr[_MP_SIZEO1]*256;  // MP��M�T�C�Y
	recvSize += adr[_MP_SIZEO2];  // MP��M�T�C�Y
	
	if((_pComm->bFirstCatchP2C) && (adr[0] & _SEND_NEXT)){
		// �܂������f�[�^������������Ƃ��Ȃ���ԂȂ̂ɘA���f�[�^������
		NET_PRINT("�A���f�[�^ child %d \n",aid);
		DEBUG_DUMP((u8*)data,24,"cr");
		return;
	}
	if(recvSize!=0){
		DEBUG_DUMP(&adr[0], _getUserMaxSendByte(),"MP");
	}
	adr += _MP_DATA_HEADER_POS;	  // �w�b�_�[�P�o�C�g�ǂݔ�΂� + Bitmap�Ł[�� + �T�C�Y�f�[�^
	{
		u16 crc = GFL_STD_CrcCalc(adr, recvSize);
		OS_TPrintf("recieve crc = %x, size = %d\n", crc, recvSize);
		GF_ASSERT(adr2[_MP_CRCNO1] == (crc & 0xff));
		GF_ASSERT(adr2[_MP_CRCNO2] == ((crc & 0xff00) >> 8));
	}
	GFL_NET_RingPuts(&_pComm->recvRing , adr, recvSize);
}

//==============================================================================
/**
 * @brief   �e�@���Ŏq�@�̒ʐM����M�������ɌĂ΂��R�[���o�b�N MP�p
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================

static void _commRecvParentCallback(u16 aid, u16 *data, u16 size)
{
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();
	u8* adr = (u8*)data;
	int i,ringRestSize,ringRestSize2;
	int mcSize = _getUserMaxSendByte();
	RingBuffWork* pRing;

#if GFL_IRC_DEBUG
	{
		int i;
		u8 *buf;
		buf = (u8*)data;
		OS_TPrintf("aaa parent recieve buf print\n");
		for(i = 0; i < size; i++){
			OS_TPrintf("%d, ", buf[i]);
		}
		OS_TPrintf("\n");
	}
#endif

	_pComm->countSendRecvServer[aid]--;  //SERVER��M
	if(adr==NULL){
		//	GF_ASSERT_MSG(0,"callback");
		return;
	}
	if(adr[0] == _INVALID_HEADER){
		return;
	}

//#ifdef WIFI_DUMP_TEST
	if(aid == 1){
		DEBUG_DUMP(&adr[0], _getUserMaxSendByte(),"pr1");
	}
	else{
		DEBUG_DUMP(&adr[0], _getUserMaxSendByte(),"pr0");
	}
//#endif

	if((_pComm->bFirstCatch[aid]) && (adr[0] & _SEND_NEXT)){
		// �܂������f�[�^������������Ƃ��Ȃ���ԂȂ̂ɘA���f�[�^������
		NET_PRINT("�A���f�[�^ parent %d \n",aid);
		i = 0;
		DEBUG_DUMP(adr,12,"�A���f�[�^");
		return;
	}
	_pComm->bFirstCatch[aid] = FALSE;  //�����M�t���O��OFF����

	adr++;
	pRing = &_pComm->recvServerRing[aid];
	mcSize -= 1;
	GFL_NET_RingPuts(pRing , adr, mcSize);
}

//==============================================================================
/**
 * @brief   �ʐM�𑗐M�����Ƃ��ɌĂ΂��R�[���o�b�N
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================




static void _sendCallbackFunc(BOOL result)
{
	if((result) && (_sendCallBack == _SEND_CB_FIRST_SEND)){
		_setSendCallBack( _SEND_CB_FIRST_SENDEND );
	}
	else{
		GF_ASSERT_MSG(0,"send failed");
	}
}

//==============================================================================
/**
 * @brief   �ʐM�𑗐M�����Ƃ��ɌĂ΂��R�[���o�b�N
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================

static void _sendServerCallback(BOOL result)
{
	int i;
	int mcSize = _getUserMaxSendByte();
	int machineMax = _getUserMaxNum();
	GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();

	if(result){
		_sendServerCallBack = _SEND_CB_FIRST_SENDEND;
		//		NET_PRINT("���Mcallback server��\n");
	}
	else{
		GF_ASSERT_MSG(0,"send failed");
	}

	for(i = 0; i < machineMax; i++){
		if(!pInit->bMPMode){			 // ������
			_pComm->sSendServerBuf[_MP_DATA_HEADER_POS+(i*mcSize)] = _INVALID_HEADER;
		}
	}
}

//==============================================================================
/**
 * @brief   �q�@���̃A�N�V�������s��ꂽ���Ƃ�ݒ�
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemActionCommandSet(void)
{
	//	_pComm->actionCount = _ACTION_COUNT_MOVE;
}

//==============================================================================
/**
 * @brief   ���M�L���[�ɂ��������̂𑗐M�o�b�t�@�ɓ���� �T�[�o�[MP�ʐM�p
 * @param   pSendBuff ����鑗�M�o�b�t�@
 * @retval  none
 */
//==============================================================================

static void _setSendDataServer(u8* pSendBuff)
{
	int i;

	pSendBuff[0] = _MP_DATA_HEADER;

	if(_pComm->bNextSendDataServer != FALSE){
		pSendBuff[0] |= _SEND_NEXT;  // ���ő���Ȃ��ꍇ
	}
	{
		u16 bitmap;
		int size;
		
		if((_GFL_NET_GetNETInitStruct())->bNetType == GFL_NET_TYPE_IRC
				|| (_GFL_NET_GetNETInitStruct())->bNetType == GFL_NET_TYPE_IRC_WIRELESS){
			bitmap = 0x3;
		}
		else{
			bitmap = GFL_NET_WL_GetBitmap();
		}
		
		pSendBuff[_MP_CONNECTBIT] = bitmap;

		{
			SEND_BUFF_DATA buffData;
			buffData.size = _getUserMaxSendByteParent() - _MP_DATA_HEADER_POS;
			buffData.pData = &pSendBuff[_MP_DATA_HEADER_POS];
			if(GFL_NET_QueueGetData(&_pComm->sendQueueMgrServer, &buffData)){
				_pComm->bNextSendDataServer = FALSE;
				size = (_getUserMaxSendByteParent() - _MP_DATA_HEADER_POS) - buffData.size;
			}
			else{
				_pComm->bNextSendDataServer = TRUE;
				size = _getUserMaxSendByteParent() - _MP_DATA_HEADER_POS;
			}
			pSendBuff[_MP_SIZEO1] = (size >> 8) & 0xff;
			pSendBuff[_MP_SIZEO2] = size & 0xff;
			GFL_NET_QueueInc(&_pComm->sendQueueMgr, &pSendBuff[_MP_INCNO]);
			{
				u16 crc = GFL_STD_CrcCalc(&pSendBuff[_MP_DATA_HEADER_POS], size);
				pSendBuff[_MP_CRCNO1] = crc & 0xff;
				pSendBuff[_MP_CRCNO2] = (crc & 0xff00) >> 8;
			}
		}
	}
}


//==============================================================================
/**
 * @brief   ��M�����f�[�^���v���Z�X���ɏ�������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _recvDataFunc(void)
{
	int id = GFL_NET_NETID_SERVER;
	int size;
	u8 command;
	int bkPos;

	if(!_pComm){
		return;
	}

	GFL_NET_RingEndChange(&_pComm->recvRing);
	if(GFL_NET_RingDataSize(&_pComm->recvRing) > 0){
		_recvDataFuncSingle(&_pComm->recvRing, id, _pComm->pTmpBuff, &_pComm->recvCommClient);
	}
}

//==============================================================================
/**
 * @brief   �ʐM�\��Ԃ̐l����Ԃ�
 * @param   none
 * @retval  �ڑ��l��
 */
//==============================================================================
int GFL_NET_SystemGetConnectNum(void)
{
	int num = 0,i;

	for(i = 0; i < GFL_NET_MACHINE_MAX; i++){
		if(GFL_NET_SystemIsConnect(i)){
			num++;
		}
	}
	return num;
}

//==============================================================================
/**
 * @brief   ���������Ă��邩�ǂ�����Ԃ�
 * @param   none
 * @retval  �������I����Ă�����TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemIsInitialize(void)
{
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

	if(_pComm){
		if( pNetIni->bNetType == GFL_NET_TYPE_WIFI || pNetIni->bNetType == GFL_NET_TYPE_IRC || pNetIni->bNetType == GFL_NET_TYPE_IRC_WIRELESS){
			return TRUE;
		}
	}
	return GFL_NET_WLIsInitialize();
}


//==============================================================================
/**
 * @brief   WH���C�u�����Ł@�ʐM��Ԃ�BIT���m�F
 * @param   none
 * @retval  �ڑ����킩��BIT�z��
 */
//==============================================================================

BOOL GFL_NET_SystemIsChildsConnecting(void)
{
	return GFL_NET_WLIsChildsConnecting();
}

//==============================================================================
/**
 * @brief   �G���[��Ԃ��ǂ���
 * @param   none
 * @retval  �G���[�̎�TRUE
 */
//==============================================================================

BOOL GFL_NET_SystemIsError(void)
{
	if(GFL_NET_SystemGetAloneMode()){  // ��l���[�h�̏ꍇ�����̓G���[�ɂ��Ȃ�
		return FALSE;
	}
	if(_pComm){
		if(_pComm->bError){
			//			CommStateSetErrorCheck(TRUE,TRUE);
			return TRUE;
		}
	}
	return GFL_NET_WLIsError();
}

//==============================================================================
/**
 * @brief   ��l�ʐM���[�h��ݒ�
 * @param   bAlone	��l�ʐM���[�h
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemSetAloneMode(BOOL bAlone)
{
	if(_pComm){
		_pComm->bAlone = bAlone;
	}
}

//==============================================================================
/**
 * @brief   ��l�ʐM���[�h���ǂ������擾
 * @param   none
 * @retval  ��l�ʐM���[�h�̏ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_SystemGetAloneMode(void)
{
	if(_pComm){
		return _pComm->bAlone;
	}
	return FALSE;
}

//==============================================================================
/**
 * @brief   �����I���R�}���h��M
 * @param   netID  �ʐMID
 * @param   size   �T�C�Y
 * @param   pData  �f�[�^
 * @param   pWork  ���[�N
 * @return  none
 */
//==============================================================================

void GFL_NET_SystemRecvAutoExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	NET_PRINT("CommRecvAutoExit ��M \n");
	if(!GFL_NET_WLIsAutoExit()){
		if(GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE){   // �������e�̏ꍇ�݂�Ȃɋt�ԐM����
			GFL_NET_SendData(pNetHandle,GFL_NET_CMD_EXIT_REQ, 0, NULL);
		}
	}
	GFL_NET_WLSetAutoExit();
}


//==============================================================================
/**
 * @brief   �L���[������ۂ��ǂ��� �T�[�o�[��
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//==============================================================================

BOOL GFL_NET_SystemIsEmptyQueue_ServerSize(void)
{
	return GFL_NET_QueueIsEmpty(&_pComm->sendQueueMgrServer);

}

//==============================================================================
/**
 * @brief   �L���[������ۂ��ǂ���
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//==============================================================================

BOOL GFL_NET_SystemIsEmptyQueue(void)
{
	return GFL_NET_QueueIsEmpty(&_pComm->sendQueueMgr);

}

//==============================================================================
/**
 * @brief   wifi�ڑ��������ǂ�����ݒ肷��
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//==============================================================================

void GFL_NET_SystemSetWifiConnect(BOOL bConnect)
{
	// �ʐM�ҋ@��Ԃɂ��邩�ǂ�����`����
	_pComm->bWifiConnect = bConnect;
}

//==============================================================================
/**
 * @brief   VCHAT��Ԃ��ǂ���
 * @param   none
 * @retval  VCHAT��ԂȂ�TRUE
 */
//==============================================================================

BOOL GFL_NET_SystemIsVChat(void)
{
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

#if GFL_NET_WIFI
	if( pNetIni->bNetType == GFL_NET_TYPE_WIFI ){
		return mydwc_IsVChat();
	}
#endif
	return FALSE;
}

//==============================================================================
/**
 * @brief   WIFI�ʐM�𓯊��ʐM���邩�A�񓯊��ʐM���邩�̐؂�ւ����s��
 * @param   TRUE ���� FALSE �񓯊�
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemSetWifiBothNet(BOOL bFlg)
{
	GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

#if GFL_NET_WIFI
	if( pNetIni->bNetType == GFL_NET_TYPE_WIFI ){
		_pComm->bWifiSendRecv = bFlg;
		if(bFlg){
			_pComm->countSendRecv = 0;
			_pComm->countSendRecvServer[0] = 0;
			_pComm->countSendRecvServer[1] = 0;
		}
		NET_PRINT("oo�����؂�ւ� %d\n",bFlg);
	}
#endif
}


//==============================================================================
/**
 * @brief   �ʐM���~�߂�
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemShutdown(void)
{
	if(_pComm){
		_pComm->bShutDown = TRUE;
	}
}

//==============================================================================
/**
 * @brief   �T�[�o�[���̒ʐM�L���[�����Z�b�g����
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemResetQueue_Server(void)
{
	GFL_NET_QueueManagerReset(&_pComm->sendQueueMgrServer);
}

//==============================================================================
/**
 * @brief   �f�[�^�V�F�A�����O�������������ǂ���
 * @retval  TRUE  �f�[�^�V�F�A�����O����
 * @retval  FALSE �f�[�^�V�F�A�����O���s
 */
//==============================================================================

BOOL GFL_NET_SystemCheckDataSharing(void)
{
	if(_pComm){
		GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
		if(!pInit->bMPMode && (pInit->bNetType == GFL_NET_TYPE_WIRELESS) ){
			return _pComm->bDSSendOK;
		}
	}
	return TRUE;
}


#endif