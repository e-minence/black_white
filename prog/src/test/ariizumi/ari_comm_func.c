//======================================================================
/**
 *
 * @file	ari_comm_func.c	
 * @brief	�ʐM�p�@�\�S
 * @author	ariizumi
 * @data	08.10.14
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "net/network_define.h"
#include "textprint.h"
#include "arc_def.h"


#include "ari_comm_func.h"
#include "ari_comm_def.h"
//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	FIELD_COMM_DATA
//======================================================================
struct _FIELD_COMM_DATA 
{
    u8	    seqNo;

    HEAPID   heapID;
    BOOL isFinishInitSystem;

    FIELD_COMM_TYPE commType;
    FIELD_COMM_MODE commMode;

    GFL_NETHANDLE   *selfHandle;    //���g�̒ʐM�n���h��
};

//======================================================================
//	proto
//======================================================================
FIELD_COMM_DATA	*f_comm = NULL;

FIELD_COMM_DATA *FieldComm_InitData( u32 heapID );
BOOL	FieldComm_InitSystem();
void	FieldComm_InitParent();
void	FieldComm_InitChild();

u8	FieldComm_UpdateSearchParent();
BOOL	FieldComm_GetSearchParentName( const u8 idx , STRBUF *name );
BOOL	FieldComm_IsValidParentData( u8 idx );

//����M�֌W
void	FieldComm_SendSelfData();

//�`�F�b�N�֐�
BOOL	FieldComm_IsFinish_InitSystem(); 
BOOL	FieldComm_IsFinish_ConnectParent();

//�e��R�[���o�b�N
void	InitCommLib_EndCallback( void* pWork );
void*	FieldComm_GetBeaconData(void);
int	FieldComm_GetBeaconSize(void);
BOOL	FieldComm_CheckConnectService(GameServiceID GameServiceID1, 
				      GameServiceID GameServiceID2);

//��M�p�R�[���o�b�N
void FieldComm_Post_FirstBeacon(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
void FieldComm_Post_StartMode(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
void FieldComm_Post_PlayerData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);

static const NetRecvFuncTable FieldCommPostTable[] = {
    { FieldComm_Post_FirstBeacon , NULL },
    { FieldComm_Post_StartMode   , NULL },
    { FieldComm_Post_PlayerData  , NULL },
};
//--------------------------------------------------------------
/**
 * �ʐM�p�f�[�^������
 * @param	heapID	�q�[�vID
 * @retval	FIELD_COMM_DATA
 */
//--------------------------------------------------------------
FIELD_COMM_DATA *FieldComm_InitData( u32 heapID )
{
    f_comm = GFL_HEAP_AllocClearMemory( heapID , sizeof( FIELD_COMM_DATA ) );
    
    f_comm->seqNo  = 0;
    f_comm->heapID = heapID;
    f_comm->commType = FCT_CHILD;
    f_comm->commMode = FCM_2_SINGLE;
    f_comm->isFinishInitSystem = FALSE;
    
    return f_comm;
}

//--------------------------------------------------------------
/**
 * �ʐM���C�u����������
 * @param	heapID	�q�[�vID
 * @retval	FIELD_COMM_DATA
 */
//--------------------------------------------------------------
BOOL	FieldComm_InitSystem()
{
    GFLNetInitializeStruct aGFLNetInit = {
  	FieldCommPostTable,	//NetSamplePacketTbl,  // ��M�֐��e�[�u��
	1,	//NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
	NULL,	// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
	NULL,	// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
	FieldComm_GetBeaconData,	// �r�[�R���f�[�^�擾�֐�  
	FieldComm_GetBeaconSize,	// �r�[�R���f�[�^�T�C�Y�擾�֐� 
	FieldComm_CheckConnectService,	// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
	NULL,	//FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	NULL,	// �ʐM�ؒf���ɌĂ΂��֐�
	NULL,	// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
	NULL,NULL,NULL,NULL,
#endif //GFL_NET_WIFI
	0x444,	//ggid  DP=0x333,RANGER=0x178,WII=0x346
	0,	    //���ɂȂ�heapid
	0,   //�ʐM�p��create�����HEAPID
	0,   //wifi�p��GameServiceID GameServiceID1, GameServiceID GameServiceID2create�����HEAPID
	GFL_WICON_POSX,GFL_WICON_POSY,	// �ʐM�A�C�R��XY�ʒu
	4,//_MAXNUM,	//�ő�ڑ��l��
	32,//_MAXSIZE,	//�ő呗�M�o�C�g��
	4,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
	TRUE,	    // CRC�v�Z
	FALSE,	    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
	FALSE,	    //wifi�ʐM���s�����ǂ���
	TRUE,	    // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	1//WB_NET_FIELDMOVE_SERVICEID	//GameServiceID
    };

    aGFLNetInit.baseHeapID = f_comm->heapID;
    aGFLNetInit.netHeapID = f_comm->heapID+1;
    aGFLNetInit.wifiHeapID = f_comm->heapID+2;

  

    GFL_NET_Init( &aGFLNetInit , InitCommLib_EndCallback , NULL );
    return TRUE;
}

//--------------------------------------------------------------
/**
 *  �e�@�ڑ������� 
 */
//--------------------------------------------------------------
void	FieldComm_InitParent()
{
    f_comm->commType = FCT_PARENT;
    GFL_NET_InitServer();
}

//--------------------------------------------------------------
/**
 *  �q�@�ڑ������� 
 */
//--------------------------------------------------------------
void	FieldComm_InitChild()
{
    f_comm->commType = FCT_CHILD;
    GFL_NET_StartBeaconScan();
}

//--------------------------------------------------------------
/**
 *  �e�@�T���A�b�v�f�[�g 
 * @param	
 * @retval	int �e�@��␔
 */
//--------------------------------------------------------------
u8	FieldComm_UpdateSearchParent()
{   
    int findNum = 0;

    while(TRUE){
	// TODO:�f�[�^�͕ύX����邩������Ȃ��̂ŁA�����Ń��[�J���ɕۑ����Ă����ׂ����H
	FIELD_COMM_BEACON *b_data = GFL_NET_GetBeaconData( findNum );
	if( b_data != NULL ){
	    findNum++;
	}
	else break;
    }
    
    OS_TPrintf("ARI_COMM FindParent [%d]\n",findNum);
    return findNum;
}

//--------------------------------------------------------------
/**
 *  �e�@�T�� ���O�擾 FieldComm_GetBeaconSize
 * @param	const u8 �C���f�b�N�X
 * @param	STRBUF* �i�[�ꏊ
 * @retval	int �e�@��␔
 */
//--------------------------------------------------------------
BOOL	FieldComm_GetSearchParentName( const u8 idx , STRBUF *name )
{
    const FIELD_COMM_BEACON *b_data = GFL_NET_GetBeaconData( idx );
    if( b_data == NULL )
    {
	OS_TPrintf("FieldComm Parent data not found!! idx[%d]\n",idx);
	return FALSE;
    }
    GFL_STR_SetStringCode( name , b_data->name );
    return TRUE;
}

//--------------------------------------------------------------
/**
 *  �e�@�T�� �f�[�^�̗L�����擾 
 * @param	const u8 �f�[�^�C���f�b�N�X
 * @retval	BOOL �L���E����
 */
//--------------------------------------------------------------
BOOL	FieldComm_IsValidParentData( const u8 idx )
{
    const FIELD_COMM_BEACON *b_data = GFL_NET_GetBeaconData( idx );
    if( b_data == NULL ){ return FALSE; }
    else		{ return TRUE;  }
}


//--------------------------------------------------------------
/**
 *  �e�@�T�� �e�@�ɐڑ� 
 * @param	const u8 �C���f�b�N�X
 * @retval	int �e�@��␔
 */
//--------------------------------------------------------------
void	FieldComm_ConnectParent( u8 idx )
{
    u8* macAddress = GFL_NET_GetBeaconMacAddress( idx );
    if( macAddress == NULL ){
	GF_ASSERT("macAddress is not found!!");
    }
    GFL_NET_ConnectToParent( macAddress );
}


//--------------------------------------------------------------
//����M�֌W
//--------------------------------------------------------------
//���g�̃f�[�^(���O�EID)�𑗂�B�r�[�R���̑��M�Ɠ����̃f�[�^�Ƃ���
void	FieldComm_SendSelfData()
{
    //�Ƃ肠�����r�[�R���𑗐M
    void *data = FieldComm_GetBeaconData();
//  const BOOL ret = GFL_NET_SendData( f_comm->selfHandle , 
//		    FC_CMD_FIRST_BEACON , FieldComm_GetBeaconSize() , data );
    const BOOL ret = GFL_NET_SendDataEx( f_comm->selfHandle , 
		    GFL_NET_SENDID_ALLUSER , FC_CMD_FIRST_BEACON , FieldComm_GetBeaconSize() ,
		    data , FALSE , TRUE , FALSE );
    if( ret == FALSE ){ OS_TPrintf("FieldComm Data send is failued!!\n"); }


}

//--------------------------------------------------------------
//	�e��`�F�b�N�֐�
//--------------------------------------------------------------
BOOL	FieldComm_IsFinish_InitSystem()
{
    return f_comm->isFinishInitSystem;
}

//  �e�@�ւ̐ڑ����������A�ʐM���ł����ԂɂȂ������H
BOOL	FieldComm_IsFinish_ConnectParent()
{
    switch( f_comm->seqNo )
    {
    case 0:
	if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
	{
	    f_comm->seqNo++;
	}
	break;
    case 1:
	f_comm->selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	if( GFL_NET_HANDLE_IsNegotiation( f_comm->selfHandle ) == TRUE )
	{
	    f_comm->seqNo++;
	}
	break;
    case 2:
	if( GFL_NET_IsSendEnable( f_comm->selfHandle ) == TRUE )
	{
	    f_comm->seqNo = 0;
	    return TRUE;
	}
	break;
    }
    return FALSE;
}

//--------------------------------------------------------------
//	�ȉ��A�e��R�[���o�b�N
//--------------------------------------------------------------

//--------------------------------------------------------------
/**
 * �ʐM���C�u���������������p�R�[���o�b�N
 * @param	void	*pWork(*FIELC_COMM_DATA)
 * @retval	void
 */
//--------------------------------------------------------------
void	InitCommLib_EndCallback( void* pWork )
{
    f_comm->isFinishInitSystem = TRUE;
}



//--------------------------------------------------------------
/**
 *	�e��r�[�R���p�R�[���o�b�N�֐� 
 */
//--------------------------------------------------------------
void*	FieldComm_GetBeaconData(void)
{
    static FIELD_COMM_BEACON testData = {
	L"�Ă��Ƃ悤" ,
	0x8000
    };

    OSOwnerInfo dsData;
    u8 i;
    OS_GetOwnerInfo( &dsData );
    
    testData.id = 0;
    for( i=0;i<5;i++ )
    {
	testData.name[i] = dsData.nickName[i];
	testData.id = (testData.id+dsData.nickName[i])%FIELD_COMM_ID_MAX;
    }
    testData.name[5] = 0xFFFF;
    
    return (void*)&testData;
}
int	FieldComm_GetBeaconSize(void)
{
    return sizeof( FIELD_COMM_BEACON );
}
BOOL	FieldComm_CheckConnectService(GameServiceID GameServiceID1, 
				      GameServiceID GameServiceID2)
{
    if( GameServiceID1 == GameServiceID2 ){ return TRUE; }
    return FALSE;
}


//��M�p�R�[���o�b�N
void FieldComm_Post_FirstBeacon(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    OS_TPrintf("FieldComm getData[FierstBeacon]\n");
}

void FieldComm_Post_StartMode(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    OS_TPrintf("FieldComm getData[StartMode]\n");
}

void FieldComm_Post_PlayerData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    OS_TPrintf("FieldComm getData[PlayerData]\n");
}



