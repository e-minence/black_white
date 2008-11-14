//======================================================================
/**
 * @file	field_comm_func.c
 * @brief	�t�B�[���h�ʐM�@�ʐM�@�\��
 * @author	ariizumi
 * @data	08/11/13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "net/network_define.h"

#include "gamesystem/playerwork.h"
#include "field_comm_func.h"
#include "field_comm_data.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
//�ʐM���
typedef enum
{
	FIELD_COMM_MODE_NONE,	//�ʐM�Ȃ�
	FIELD_COMM_MODE_PARENT,	//�e�@(�N���҂��E�N����
	FIELD_COMM_MODE_CHILD,	//�q�@(�N����
	FIELD_COMM_MODE_SEARCH,	//�T����(�N����T����
}FIELD_COMM_MODE;

//����M�֐��p
enum FIELD_COMM_COMMAND_TYPE
{
    FC_CMD_SELFDATA = GFL_NET_CMD_COMMAND_MAX,	//��e�ʃf�[�^(�{�b�N�X�f�[�^��z��
};

//======================================================================
//	typedef struct
//======================================================================
struct _FIELD_COMM_FUNC
{
	HEAPID	heapID_;
	FIELD_COMM_MODE commMode_;
	
	BOOL	isInitCommSystem_;

};

//======================================================================
//	proto
//======================================================================

FIELD_COMM_FUNC* FieldCommFunc_InitSystem( HEAPID heapID );
void	FieldCommFunc_TermSystem( FIELD_COMM_FUNC *commFunc );
void	FieldCommFunc_InitCommSystem( FIELD_COMM_FUNC *commFunc );
void	FieldCommFunc_TermCommSystem( FIELD_COMM_FUNC *commFunc );

void	FieldCommFunc_StartCommChild( FIELD_COMM_FUNC *commFunc );
void	FieldCommFunc_StartCommParent( FIELD_COMM_FUNC *commFunc );
void	FieldCommFunc_StartCommChangeover( FIELD_COMM_FUNC *commFunc );

//�e��`�F�b�N�֐�
const BOOL FieldCommFunc_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc );
const BOOL FieldCommFunc_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc );
const int	FieldCommFunc_GetSelfIndex( FIELD_COMM_FUNC *commFunc );

//����M�֐�
void	FieldCommFunc_Send_SelfData( FIELD_COMM_FUNC *commFunc );
void	FieldCommFunc_Post_SelfData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );

//�e��R�[���o�b�N
void	FieldCommFunc_FinishInitCallback( void* pWork );
void	FieldCommFunc_FinishTermCallback( void* pWork );
void*	FieldCommFunc_GetBeaconData(void);		// �r�[�R���f�[�^�擾�֐�  
int		FieldCommFunc_GetBeaconSize(void);		// �r�[�R���f�[�^�T�C�Y�擾�֐� 
BOOL	FieldCommFunc_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 ); // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
void	FieldCommFunc_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);		// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
void	FieldCommFunc_DisconnectCallBack(void* pWork);	// �ʐM�ؒf���ɌĂ΂��֐�(�I����

static const NetRecvFuncTable FieldCommRecvTable[] = {
	{ FieldCommFunc_Post_SelfData ,NULL }
};


//--------------------------------------------------------------
//	�V�X�e��������
//	���̎��_�ł͒ʐM�͊J�n���܂���
//--------------------------------------------------------------
FIELD_COMM_FUNC* FieldCommFunc_InitSystem( HEAPID heapID )
{
	FIELD_COMM_FUNC	*commFunc;
	
	commFunc = GFL_HEAP_AllocMemory( heapID , sizeof(FIELD_COMM_FUNC) );
	commFunc->heapID_ = heapID;
	//commFunc->isInitCommSystem_ = FALSE;
	commFunc->isInitCommSystem_ = GFL_NET_IsInit();
	return commFunc;
}

//--------------------------------------------------------------
//	�V�X�e���J��
//--------------------------------------------------------------
void	FieldCommFunc_TermSystem( FIELD_COMM_FUNC *commFunc )
{
	GFL_HEAP_FreeMemory( commFunc );
}

//--------------------------------------------------------------
//	�ʐM�J�n
//--------------------------------------------------------------
void	FieldCommFunc_InitCommSystem( FIELD_COMM_FUNC *commFunc )
{
	GFLNetInitializeStruct aGFLNetInit = {
		FieldCommRecvTable,	//NetSamplePacketTbl,  // ��M�֐��e�[�u��
		NELEMS(FieldCommRecvTable), // ��M�e�[�u���v�f��
        NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
        NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
        NULL,	// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
		NULL,	// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
		FieldCommFunc_GetBeaconData,		// �r�[�R���f�[�^�擾�֐�  
		FieldCommFunc_GetBeaconSize,		// �r�[�R���f�[�^�T�C�Y�擾�֐� 
		FieldCommFunc_CheckConnectService,	// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
		FieldCommFunc_ErrorCallBack,		// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
		FieldCommFunc_DisconnectCallBack,	// �ʐM�ؒf���ɌĂ΂��֐�(�I����
		NULL,	// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
		NULL,NULL,NULL,NULL,
#endif //GFL_NET_WIFI
		0x444,	//ggid  DP=0x333,RANGER=0x178,WII=0x346
		GFL_HEAPID_APP,  //���ɂȂ�heapid
		HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
		HEAPID_WIFI,  //wifi�p��create�����HEAPID
		HEAPID_NETWORK,	//
		GFL_WICON_POSX,GFL_WICON_POSY,	// �ʐM�A�C�R��XY�ʒu
		4,//_MAXNUM,	//�ő�ڑ��l��
		48,//_MAXSIZE,	//�ő呗�M�o�C�g��
		4,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
		TRUE,		// CRC�v�Z
		FALSE,		// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
		GFL_NET_TYPE_WIRELESS,		//�ʐM�^�C�v�̎w��
		TRUE,		// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
		WB_NET_FIELDMOVE_SERVICEID	//GameServiceID
	};

	GFL_NET_Init( &aGFLNetInit , FieldCommFunc_FinishInitCallback , (void*)commFunc );
}

//--------------------------------------------------------------
//	�ʐM�V�X�e���I��(�ʐM�̐ؒf�͂��܂���
//--------------------------------------------------------------
void	FieldCommFunc_TermCommSystem( FIELD_COMM_FUNC *commFunc )
{
	GFL_NET_Exit( FieldCommFunc_FinishTermCallback );
}

//--------------------------------------------------------------
//	�e�@�ʐM�̊J�n
//--------------------------------------------------------------
void	FieldCommFunc_StartCommParent( FIELD_COMM_FUNC *commFunc )
{
	GFL_NET_InitServer();
}

//--------------------------------------------------------------
//	�q�@�ʐM�̊J�n
//--------------------------------------------------------------
void	FieldCommFunc_StartCommChild( FIELD_COMM_FUNC *commFunc )
{
	GFL_NET_StartBeaconScan();
}

//--------------------------------------------------------------
//	�T���ʐM�̊J�n(�e�q���ݒʐM
//--------------------------------------------------------------
void	FieldCommFunc_StartCommChangeover( FIELD_COMM_FUNC *commFunc )
{
	GFL_NET_ChangeoverConnect(NULL);
}


//======================================================================
//	�e��`�F�b�N�֐�
//======================================================================
//--------------------------------------------------------------
// �ʐM���C�u�����@�������E�J���p  
//--------------------------------------------------------------
const BOOL FieldCommFunc_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc )
{
	return commFunc->isInitCommSystem_;
}
const BOOL FieldCommFunc_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc )
{
	return !commFunc->isInitCommSystem_;
}

//--------------------------------------------------------------
// �����̃C���f�b�N�X(netID)���擾  
//--------------------------------------------------------------
const int	FieldCommFunc_GetSelfIndex( FIELD_COMM_FUNC *commFunc )
{
	GFL_NETHANDLE *handle =GFL_NET_HANDLE_GetCurrentHandle();
	return GFL_NET_GetNetID( handle );
}

//======================================================================
//����M�֐�
//======================================================================
typedef struct
{
	u16 posX_;
	u16 posZ_;
	u8	posY_;		//�}�b�N�X�s���Ȃ̂ŁB�ꍇ�ɂ���Ă͂Ȃ���OK�H
	u8	dir_;
	u16	zoneID_;	//�����͒ʐM�p��ID�Ƃ��ĕϊ����ė}������
}FIELD_COMM_PLAYER_PACKET;
//--------------------------------------------------------------
// �����̃f�[�^���M  
//--------------------------------------------------------------
void	FieldCommFunc_Send_SelfData( FIELD_COMM_FUNC *commFunc )
{
	FIELD_COMM_PLAYER_PACKET plPkt;
	PLAYER_WORK *plWork = NULL;
	const VecFx32 *pos;
	u16 dir;
	ZONEID zoneID;

	//�f�[�^���W
	plWork = FieldCommData_GetSelfData_PlayerWork();
	zoneID = PLAYERWORK_getZoneID( plWork );
	pos = PLAYERWORK_getPosition( plWork );
	dir = PLAYERWORK_getDirection( plWork );
	
	//�p�P�b�g�ɃZ�b�g
	plPkt.zoneID_ = zoneID;
	plPkt.posX_ = F32_CONST( pos->x );
	plPkt.posY_ = F32_CONST( pos->y );
	plPkt.posZ_ = F32_CONST( pos->z );
	//plPkt.dir_ = dir>>8;
	plPkt.dir_ = dir;

	ARI_TPrintf("SEND[ ][%d][%d][%d][%d]\n",plPkt.posX_,plPkt.posY_,plPkt.posZ_,dir);
	{
		GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
		const BOOL ret = GFL_NET_SendDataEx( selfHandle , GFL_NET_SENDID_ALLUSER , 
				FC_CMD_SELFDATA , sizeof(FIELD_COMM_PLAYER_PACKET) , 
				(void*)&plPkt , FALSE , TRUE , FALSE );
		if( ret == FALSE ){
			ARI_TPrintf("FieldComm SendSelfData is failue!\n");
		}
	}
}

//--------------------------------------------------------------
// �����̃f�[�^��M(�p�͑��̃v���C���[�̎�M  
//--------------------------------------------------------------
void	FieldCommFunc_Post_SelfData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
	//�����̃f�[�^���ꉞ�Z�b�g���Ă���(�����p�Ɏg��
	PLAYER_WORK *plWork;
	const FIELD_COMM_PLAYER_PACKET *pkt = (FIELD_COMM_PLAYER_PACKET*)pData;
	VecFx32 pos;
	u16 dir;
	
	//�p�P�b�g�f�[�^��߂�
	pos.x = FX32_CONST( pkt->posX_ );
	pos.y = FX32_CONST( pkt->posY_ );
	pos.z = FX32_CONST( pkt->posZ_ );
	//dir	= pkt->dir_<<8;
	dir = pkt->dir_;

	ARI_TPrintf("POST[%d][%d][%d][%d][%d]\n",netID,pkt->posX_,pkt->posY_,pkt->posZ_,dir);
	
	//set
	plWork = FieldCommData_GetCharaData_PlayerWork(netID);
	PLAYERWORK_setPosition( plWork , &pos );
	PLAYERWORK_setDirection( plWork , dir );
	PLAYERWORK_setZoneID( plWork , pkt->zoneID_ );
	FieldCommData_SetCharaData_IsValid( netID , TRUE );

	ARI_TPrintf("FieldComm PostSelfData[%d]\n",netID);
}

//======================================================================
//�e��R�[���o�b�N
//======================================================================
//--------------------------------------------------------------
// �ʐM���C�u�����@�������E�J���p  
//--------------------------------------------------------------
void	FieldCommFunc_FinishInitCallback( void* pWork )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	commFunc->isInitCommSystem_ = TRUE;
}
void	FieldCommFunc_FinishTermCallback( void* pWork )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	commFunc->isInitCommSystem_ = FALSE;
}

//--------------------------------------------------------------
// �r�[�R���f�[�^�擾�֐�  
//--------------------------------------------------------------
void*	FieldCommFunc_GetBeaconData(void)
{
	static u8 dummyData[2] = {1,27};
	return (void*)&dummyData;
}

//--------------------------------------------------------------
// �r�[�R���f�[�^�T�C�Y�擾�֐� 
//--------------------------------------------------------------
int		FieldCommFunc_GetBeaconSize(void)
{
	return 2;
}

//--------------------------------------------------------------
// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
//--------------------------------------------------------------
BOOL	FieldCommFunc_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 )
{
	return TRUE;
}

//--------------------------------------------------------------
// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
//--------------------------------------------------------------
void	FieldCommFunc_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
}

//--------------------------------------------------------------
// �ʐM�ؒf���ɌĂ΂��֐�(�I����
//--------------------------------------------------------------
void	FieldCommFunc_DisconnectCallBack(void* pWork)
{
}

