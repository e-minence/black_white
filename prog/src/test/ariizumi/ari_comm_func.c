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
    HEAPID   heapID;
    BOOL isFinishInitSystem;

    FIELD_COMM_TYPE commType;
    FIELD_COMM_MODE commMode;
};

//======================================================================
//	proto
//======================================================================
FIELD_COMM_DATA	*f_comm = NULL;

FIELD_COMM_DATA *FieldComm_InitData( u32 heapID );
BOOL	FieldComm_InitSystem();
void	FieldComm_InitParent();
void	FieldComm_InitChild();

int	fieldComm_UpdateSearchParent();

//�`�F�b�N�֐�
BOOL	FieldComm_IsFinish_InitSystem(); 

//�e��R�[���o�b�N
void	InitCommLib_EndCallback( void* pWork );
void*	FieldComm_GetBeaconData(void);
int	FieldComm_GetBeaconSize(void);
BOOL	FieldComm_CheckConnectService(GameServiceID GameServiceID1, 
				      GameServiceID GameServiceID2);

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
  	NULL,	//NetSamplePacketTbl,  // ��M�֐��e�[�u��
	1,	//NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
	NULL,	// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
	NULL,	// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
	FieldComm_GetBeaconData,	// �r�[�R���f�[�^�擾�֐�  
	FieldComm_GetBeaconSize,	// �r�[�R���f�[�^�T�C�Y�擾�֐� 
	FieldComm_CheckConnectService,	// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
	NULL,	//FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	NULL,	// �ʐM�ؒf���ɌĂ΂��֐�
	NULL,	// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
	0x666,	//ggid  DP=0x333,RANGER=0x178,WII=0x346
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
int	FieldComm_UpdateSearchParent()
{   
    int findNum = 0;

    while(TRUE){
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
//	�e��`�F�b�N�֐�
//--------------------------------------------------------------
BOOL	FieldComm_IsFinish_InitSystem()
{
    return f_comm->isFinishInitSystem;
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
	{ 0,1,2,3,4,5 } ,
	0x8000
    };
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
