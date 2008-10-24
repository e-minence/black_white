//======================================================================
/**
 * @file	dlplay_comm_func.c	
 * @brief	DL�ʐM �e�q���ʂ̒ʐM�֌W
 * @author	ariizumi
 * @data	08/10/21
 */
//======================================================================
#include "gflib.h"
#include "system/gfl_use.h"
#include "net/network_define.h"
#include "system/main.h"


#include "dlplay_comm_func.h"
#include "../ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
//����M�֐��p
enum DLPLAY_COMM_COMMAND_TYPE
{
    DC_CMD_LARGE_PACKET = GFL_NET_CMD_COMMAND_MAX,	//��e�ʃf�[�^(�{�b�N�X�f�[�^��z��
	DC_CMD_CONNET_SIGN,		//�q�@���ڑ������Ƃ��ɑ���
};
//======================================================================
//	typedef struct
//======================================================================
//======================================================================
//	DLPLAY_COMM_DATA
//======================================================================
struct _DLPLAY_COMM_DATA 
{
	u8		seqNo_;

	HEAPID   heapID_;
	BOOL isFinishInitSystem_;
	BOOL isError_;
	BOOL isStartMode_;
	
	GFL_NETHANDLE   *selfHandle_;	//���g�̒ʐM�n���h��

	DLPLAY_LARGE_PACKET packetBuff_;	//����M���ʂ̃o�b�t�@�I��舵������

};

//======================================================================
//	proto
//======================================================================
DLPLAY_COMM_DATA* DLPlayComm_InitData( u32 heapID );
void	DLPlayComm_TermSystem( DLPLAY_COMM_DATA *d_comm );
BOOL	DLPlayComm_InitSystem( DLPLAY_COMM_DATA *d_comm);

void	DLPlayComm_InitParent( DLPLAY_COMM_DATA *d_comm );
void	DLPlayComm_InitChild( DLPLAY_COMM_DATA *d_comm , u8 *macAddress );
BOOL	DLPlayComm_IsFinish_ConnectParent( DLPLAY_COMM_DATA *d_comm );

void*	DLPlayComm_GetBeaconDataDummy(void);
int		DLPlayComm_GetBeaconSizeDummy(void);

//�e��`�F�b�N�֐�
BOOL	DLPlayComm_IsFinish_InitSystem( DLPLAY_COMM_DATA *d_comm );

//�e��R�[���o�b�N
void	DLPlayComm_InitEndCallback( void* pWork );
void	DLPlayComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);
void	DLPlayComm_DisconnectCallBack(GFL_NETHANDLE* pNet);

//����M�p�֐�
//��
void	DLPlayComm_Send_LargeData( DLPLAY_LARGE_PACKET *data , DLPLAY_COMM_DATA *d_comm );
void	DLPlayComm_Send_ConnectSign( DLPLAY_COMM_DATA *d_comm );
//��
void	DLPlayComm_Post_LargeData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
u8*		DLPlayComm_Post_LargeData_Buff( int netID, void* pWork , int size );
void	DLPlayComm_Post_ConnectSign(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


static const NetRecvFuncTable DLPlayCommPostTable[] = {
	{ DLPlayComm_Post_LargeData , DLPlayComm_Post_LargeData_Buff },
	{ DLPlayComm_Post_ConnectSign , NULL },
};
//--------------------------------------------------------------
/**
 * �ʐM�p�f�[�^������
 * @param	heapID	�q�[�vID
 * @retval	DLPLAY_COMM_DATA
 */
//--------------------------------------------------------------
DLPLAY_COMM_DATA* DLPlayComm_InitData( u32 heapID )
{
	u8 i=0;
	DLPLAY_COMM_DATA *d_comm;

	d_comm = GFL_HEAP_AllocClearMemory( heapID , sizeof( DLPLAY_COMM_DATA ) );
	
	d_comm->seqNo_  = 0;
	d_comm->heapID_ = heapID;
	d_comm->isFinishInitSystem_ = FALSE;
	d_comm->isError_ = FALSE;
	d_comm->isStartMode_ = FALSE;
#if 1
	{
		u8 i;
		for( i=0;i<TEST_DATA_LINE;i++ )
		{
			u16 j;
			for( j=0;j<TEST_DATA_NUM;j++ )
			{
				d_comm->packetBuff_.data_[i][j] =('A'+i);
			}
			d_comm->packetBuff_.data_[i][TEST_DATA_NUM-1] = '\0';
		}
	}
#endif
	return d_comm;
}

//--------------------------------------------------------------
/**
 * �ʐM���C�u�����J��
 * @param	heapID	�q�[�vID
 * @retval	DLPLAY_COMM_DATA
 */
//--------------------------------------------------------------
void	DLPlayComm_TermSystem( DLPLAY_COMM_DATA *d_comm )
{
	if( d_comm == NULL ){
		OS_TPrintf("FieldComm System is not init.\n");
		return;
	}
	GFL_HEAP_FreeMemory( d_comm );
	d_comm = NULL;
}

//--------------------------------------------------------------
/**
 * �ʐM���C�u����������
 * @param	heapID	�q�[�vID
 * @retval	DLPLAY_COMM_DATA
 */
//--------------------------------------------------------------
BOOL	DLPlayComm_InitSystem( DLPLAY_COMM_DATA *d_comm)
{
	GFLNetInitializeStruct aGFLNetInit = {
		DLPlayCommPostTable,	//NetSamplePacketTbl,  // ��M�֐��e�[�u��
		NELEMS(DLPlayCommPostTable), // ��M�e�[�u���v�f��
		NULL,	// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
		NULL,	// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
		DLPlayComm_GetBeaconDataDummy,	// �r�[�R���f�[�^�擾�֐�  
		DLPlayComm_GetBeaconSizeDummy,	// �r�[�R���f�[�^�T�C�Y�擾�֐� 
		NULL,	// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
		DLPlayComm_ErrorCallBack,	// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
		DLPlayComm_DisconnectCallBack,	// �ʐM�ؒf���ɌĂ΂��֐�
		NULL,	// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
		NULL,NULL,NULL,NULL,
#endif //GFL_NET_WIFI
		0x444,	//ggid  DP=0x333,RANGER=0x178,WII=0x346
		0,  //���ɂȂ�heapid
		HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
		HEAPID_WIFI,  //wifi�p��create�����HEAPID
		GFL_WICON_POSX,GFL_WICON_POSY,	// �ʐM�A�C�R��XY�ʒu
		2,//_MAXNUM,	//�ő�ڑ��l��
		48,//_MAXSIZE,	//�ő呗�M�o�C�g��
		2,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
		TRUE,		// CRC�v�Z
		FALSE,		// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
		FALSE,		//wifi�ʐM���s�����ǂ���
		FALSE,		// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
		1//WB_NET_FIELDMOVE_SERVICEID	//GameServiceID
	};

	aGFLNetInit.baseHeapID = d_comm->heapID_;
//	aGFLNetInit.netHeapID = d_comm->heapID_;


	GFL_NET_Init( &aGFLNetInit , DLPlayComm_InitEndCallback , (void*)d_comm );
	aGFLNetInit.netHeapID = d_comm->heapID_;
	return TRUE;
}

//--------------------------------------------------------------
/**						if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_START )
	
 *  �e�@�ڑ������� 
 */
//--------------------------------------------------------------
void	DLPlayComm_InitParent( DLPLAY_COMM_DATA *d_comm )
{
//	d_comm->commType = FCT_PARENT;
	GFL_NET_InitServer();
}

//--------------------------------------------------------------
/**
 *	�q�@�ڑ�������(�����Ȃ�ڑ�������
 */
//--------------------------------------------------------------
void	DLPlayComm_InitChild( DLPLAY_COMM_DATA *d_comm , u8 *macAddress )
{
//	d_comm->commType = FCT_PARENT;
	d_comm->seqNo_ = 0;
	GFL_NET_InitClientAndConnectToParent( macAddress );
}

//  �e�@�ւ̐ڑ����������A�ʐM���ł����ԂɂȂ������H
BOOL	DLPlayComm_IsFinish_ConnectParent( DLPLAY_COMM_DATA *d_comm )
{
	switch( d_comm->seqNo_ )
	{
	case 0:
		if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
		{
			d_comm->seqNo_++;
		}
		break;
	case 1:
		d_comm->selfHandle_ = GFL_NET_HANDLE_GetCurrentHandle();
		if( GFL_NET_HANDLE_IsNegotiation( d_comm->selfHandle_ ) == TRUE )
		{
			d_comm->seqNo_++;
		}
		break;
	case 2:
		if( GFL_NET_IsSendEnable( d_comm->selfHandle_ ) == TRUE )
		{
			d_comm->seqNo_ = 0;
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 *	�e��r�[�R���p�R�[���o�b�N�֐� 
 */
//--------------------------------------------------------------
void*	DLPlayComm_GetBeaconDataDummy(void)
{
	static u8 dummy[2];
	return (void*)&dummy;
}
int	DLPlayComm_GetBeaconSizeDummy(void)
{
	return sizeof(u8)*2;
}


//--------------------------------------------------------------
//	�e��`�F�b�N�֐�
//--------------------------------------------------------------
BOOL	DLPlayComm_IsFinish_InitSystem( DLPLAY_COMM_DATA *d_comm )
{
	return d_comm->isFinishInitSystem_;
}




//--------------------------------------------------------------
//	�ȉ��A�e��R�[���o�b�N
//--------------------------------------------------------------

//--------------------------------------------------------------
/**
 * �ʐM���C�u���������������p�R�[���o�b�N						if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_START )
	
 * @param	vo#include
 * @retval	void
 */
//--------------------------------------------------------------
void	DLPlayComm_InitEndCallback( void* pWork )
{
	DLPLAY_COMM_DATA *d_comm = (DLPLAY_COMM_DATA*)pWork;
	d_comm->isFinishInitSystem_ = TRUE;
}

//�G���[�擾�R�[���o�b�N
void	DLPlayComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
	DLPLAY_COMM_DATA *d_comm = (DLPLAY_COMM_DATA*)pWork;
	OS_TPrintf("FieldComm Error!![%d]\n",errNo);
	d_comm->isError_ = TRUE;
}

//�ؒf���m�p�R�[���o�b�N
void	DLPlayComm_DisconnectCallBack(GFL_NETHANDLE* pNet)
{
//	DLPLAY_COMM_DATA *d_comm = (DLPLAY_COMM_DATA*)pWork;
	OS_TPrintf("FieldComm Disconnect!!\n");
//	d_comm->isError_ = TRUE;
}


//--------------------------------------------------------------
//	����M�֐�
//--------------------------------------------------------------
void	DLPlayComm_Send_LargeData( DLPLAY_LARGE_PACKET *data , DLPLAY_COMM_DATA *d_comm )
{
	GFL_STD_MemCopy( (void*)data , (void*)&d_comm->packetBuff_ , sizeof( DLPLAY_LARGE_PACKET ) );
	{
		const BOOL ret = GFL_NET_SendDataEx( d_comm->selfHandle_ , 
				1/*�e�@*/ , DC_CMD_LARGE_PACKET , sizeof(DLPLAY_LARGE_PACKET) ,
				&d_comm->packetBuff_ , TRUE , TRUE , TRUE );
		if( ret == FALSE ){ OS_TPrintf("DLPlayComm LargeData send is failued!!\n"); }
	}
	
}

void DLPlayComm_Post_LargeData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	DLPLAY_COMM_DATA *d_comm = (DLPLAY_COMM_DATA*)pWork;
	ARI_TPrintf("DLPlayComm getData[LargeData]\n");
	{
		u8 i;
		for( i=0;i<TEST_DATA_LINE;i++)
		{
			OS_TPrintf("%2d:%s\n",i,d_comm->packetBuff_.data_[i]);
		}
	}
}

u8*	 DLPlayComm_Post_LargeData_Buff( int netID, void* pWork , int size )
{
	//if( GFL_NET_IsParentMachine() == FALSE ){return (void*)&temp;}
	//else
	{
		DLPLAY_COMM_DATA *d_comm = (DLPLAY_COMM_DATA*)pWork;
		ARI_TPrintf("DLPlayComm PostBuff id[%d] size[%d]\n",netID,size);
		return (u8*)&d_comm->packetBuff_ ;
	}
}

void	DLPlayComm_Send_ConnectSign( DLPLAY_COMM_DATA *d_comm )
{
	u8 dummy = 127;
	d_comm->selfHandle_ = GFL_NET_HANDLE_GetCurrentHandle();
	{
		const BOOL ret = GFL_NET_SendData( d_comm->selfHandle_ , DC_CMD_CONNET_SIGN , 1 , &dummy );
		if( ret == FALSE ){ OS_TPrintf("DLPlayComm ConnectSign send is failued!!\n"); }
	}

}

void	DLPlayComm_Post_ConnectSign(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	ARI_TPrintf("DLPlayComm getData[ConnectSign]\n");
}


