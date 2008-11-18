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
#include "field_comm_main.h"
#include "field_comm_func.h"
#include "field_comm_data.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
//����M�֐��p
enum FIELD_COMM_COMMAND_TYPE
{
    FC_CMD_SELFDATA = GFL_NET_CMD_COMMAND_MAX,	//���@�f�[�^
	FC_CMD_REQUEST_DATA,	//�f�[�^�v��
	FC_CMD_SELF_PROFILE,	//�L�����̊�{���
};

//======================================================================
//	typedef struct
//======================================================================
struct _FIELD_COMM_FUNC
{
	HEAPID	heapID_;
	FIELD_COMM_MODE commMode_;
	
	u8	seqNo_;
	BOOL	isInitCommSystem_;
};

typedef struct
{
	u8	mode_:1;	//0:�ҋ@ 1:�T��
	u8	memberNum_:3;	//�l��

	u8	pad_:4;
}FIELD_COMM_BEACON;

//======================================================================
//	proto
//======================================================================

FIELD_COMM_FUNC* FIELD_COMM_FUNC_InitSystem( HEAPID heapID );
void	FIELD_COMM_FUNC_TermSystem( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_InitCommSystem( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_TermCommSystem( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_UpdateSystem( FIELD_COMM_FUNC *commFunc );
static	void FIELD_COMM_FUNC_UpdateSearchParent( FIELD_COMM_FUNC *commFunc );
static	u8	FIELD_COMM_FUNC_CompareBeacon( const FIELD_COMM_BEACON *firstBcn , const FIELD_COMM_BEACON *secondBcn );

void	FIELD_COMM_FUNC_StartCommWait( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_StartCommSearch( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_StartCommChangeover( FIELD_COMM_FUNC *commFunc );

//�e��`�F�b�N�֐�
const BOOL FIELD_COMM_FUNC_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc );
const BOOL FIELD_COMM_FUNC_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc );
const FIELD_COMM_MODE FIELD_COMM_FUNC_GetCommMode( FIELD_COMM_FUNC *commFunc );
const int	FIELD_COMM_FUNC_GetMemberNum( FIELD_COMM_FUNC *commFunc );
const int	FIELD_COMM_FUNC_GetSelfIndex( FIELD_COMM_FUNC *commFunc );

//����M�֐�
void	FIELD_COMM_FUNC_Send_SelfData( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_SelfData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
void	FIELD_COMM_FUNC_Send_RequestData( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_RequestData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
void	FIELD_COMM_FUNC_Send_SelfProfile( const int sendNetID ,FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_SelfProfile( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );

//�e��R�[���o�b�N
void	FIELD_COMM_FUNC_FinishInitCallback( void* pWork );
void	FIELD_COMM_FUNC_FinishTermCallback( void* pWork );
void*	FIELD_COMM_FUNC_GetBeaconData(void* pWork);		// �r�[�R���f�[�^�擾�֐�  
int		FIELD_COMM_FUNC_GetBeaconSize(void* pWork);		// �r�[�R���f�[�^�T�C�Y�擾�֐� 
BOOL	FIELD_COMM_FUNC_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 ); // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
void	FIELD_COMM_FUNC_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);		// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
void	FIELD_COMM_FUNC_DisconnectCallBack(void* pWork);	// �ʐM�ؒf���ɌĂ΂��֐�(�I����

static const NetRecvFuncTable FieldCommRecvTable[] = {
	{ FIELD_COMM_FUNC_Post_SelfData ,NULL },
	{ FIELD_COMM_FUNC_Post_RequestData ,NULL },
	{ FIELD_COMM_FUNC_Post_SelfProfile ,NULL },
};


//--------------------------------------------------------------
//	�V�X�e��������
//	���̎��_�ł͒ʐM�͊J�n���܂���
//--------------------------------------------------------------
FIELD_COMM_FUNC* FIELD_COMM_FUNC_InitSystem( HEAPID heapID )
{
	FIELD_COMM_FUNC	*commFunc;
	
	commFunc = GFL_HEAP_AllocMemory( heapID , sizeof(FIELD_COMM_FUNC) );
	commFunc->heapID_ = heapID;
	commFunc->isInitCommSystem_ = GFL_NET_IsInit();
	//�ʐM�X�e�[�g�̃o�b�N�A�b�v���`�F�b�N
	if( FIELD_COMM_DATA_IsExistSystem() == TRUE )
		commFunc->commMode_ = (FIELD_COMM_MODE)FIELD_COMM_DATA_GetFieldCommMode();
	else
		commFunc->commMode_ = FIELD_COMM_MODE_NONE;
	//�ʐM������������Ă����Ȃ�A�C�R���������[�h����
	if( commFunc->isInitCommSystem_ == TRUE )
		GFL_NET_ReloadIcon();
	return commFunc;
}

//--------------------------------------------------------------
//	�V�X�e���J��
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_TermSystem( FIELD_COMM_FUNC *commFunc )
{
	//�ʐM�̃X�e�[�g��ۑ�
	if( FIELD_COMM_DATA_IsExistSystem() == TRUE )
		FIELD_COMM_DATA_SetFieldCommMode(commFunc->commMode_);
	GFL_HEAP_FreeMemory( commFunc );
}

//--------------------------------------------------------------
//	�ʐM�J�n
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_InitCommSystem( FIELD_COMM_FUNC *commFunc )
{
	GFLNetInitializeStruct aGFLNetInit = {
		FieldCommRecvTable,	//NetSamplePacketTbl,  // ��M�֐��e�[�u��
		NELEMS(FieldCommRecvTable), // ��M�e�[�u���v�f��
        NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
        NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
        NULL,	// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
		NULL,	// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
		FIELD_COMM_FUNC_GetBeaconData,		// �r�[�R���f�[�^�擾�֐�  
		FIELD_COMM_FUNC_GetBeaconSize,		// �r�[�R���f�[�^�T�C�Y�擾�֐� 
		FIELD_COMM_FUNC_CheckConnectService,	// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
		FIELD_COMM_FUNC_ErrorCallBack,		// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
		FIELD_COMM_FUNC_DisconnectCallBack,	// �ʐM�ؒf���ɌĂ΂��֐�(�I����
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

	GFL_NET_Init( &aGFLNetInit , FIELD_COMM_FUNC_FinishInitCallback , (void*)commFunc );
}

//--------------------------------------------------------------
//	�ʐM�V�X�e���I��(�ʐM�̐ؒf�͂��܂���
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_TermCommSystem( FIELD_COMM_FUNC *commFunc )
{
	GFL_NET_Exit( FIELD_COMM_FUNC_FinishTermCallback );
}
extern void FIELD_COMM_DATA_SetCharaData_State( const idx , const F_COMM_CHARA_STATE state );

//--------------------------------------------------------------
//	�ʐM�V�X�e���X�V(�r�[�R���̑҂�����
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_UpdateSystem( FIELD_COMM_FUNC *commFunc )
{
	//�҂��󂯑��ł��r�[�R�����`�F�b�N���Ă݂�
	//if( commFunc->commMode_ == FIELD_COMM_MODE_SEARCH )
	//	�҂������E�T�����Őe�@�ł͂Ȃ���
	if( commFunc->commMode_ == FIELD_COMM_MODE_SEARCH ||
		commFunc->commMode_ == FIELD_COMM_MODE_WAIT	)
	{
		//�q�@�Ɛe�@�ŏ����𕪂��適����������@�������̂Œf�O�E�E�E
	//	if( GFL_NET_IsParentMachine() == FALSE )
		{
			//�q�@�̏ꍇ�̓r�[�R���̃`�F�b�N
			FIELD_COMM_FUNC_UpdateSearchParent( commFunc );
		}
	//	else
		{
			if( FIELD_COMM_FUNC_GetMemberNum(commFunc) > 1 )
			{
				//�e�@�̏ꍇ�͎q�@��������ڑ���Ԃ�
				ARI_TPrintf("Connect!(Parent)\n");
				commFunc->commMode_ = FIELD_COMM_MODE_CONNECT;
			}
		}
	}
	if( commFunc->commMode_ == FIELD_COMM_MODE_TRY_CONNECT )
	{
		switch( commFunc->seqNo_ )
		{
		case 0:
			if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
			{
				commFunc->seqNo_++;
			}
			break;
		case 1:
			{
				GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
				if( GFL_NET_HANDLE_IsNegotiation( selfHandle ) == TRUE )
				{
					commFunc->seqNo_++;
				}
			}
			break;
		case 2:
			{
				GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
				if( GFL_NET_IsSendEnable( selfHandle ) == TRUE )
				{
					commFunc->commMode_ = FIELD_COMM_MODE_CONNECT;
				}
			}
		}
	}
}

//--------------------------------------------------------------
//	�q�@�Őe�@��T���Ă�����
//--------------------------------------------------------------
static	void FIELD_COMM_FUNC_UpdateSearchParent( FIELD_COMM_FUNC *commFunc )
{
	u8 bcnIdx = 0;
	int targetIdx = -1;
	FIELD_COMM_BEACON *bcnData;
	const FIELD_COMM_BEACON *selfBcn = FIELD_COMM_FUNC_GetBeaconData((void*)commFunc);
	while( GFL_NET_GetBeaconData(bcnIdx) != NULL )
	{
		bcnData = GFL_NET_GetBeaconData( bcnIdx );
		if( selfBcn->mode_ == 1 || bcnData->mode_ == 1 )
		{
			//�ڑ������𖞂������B
			if( targetIdx == -1 )
			{
				targetIdx = bcnIdx;
			}
			else
			{
				//���łɑ��̃r�[�R�����ڑ����ɂ���̂Ŕ�r
				const FIELD_COMM_BEACON *compBcn = GFL_NET_GetBeaconData(targetIdx);
				const u8 result = FIELD_COMM_FUNC_CompareBeacon( bcnData , compBcn );
				if( result == 1 )
				{
					targetIdx = bcnIdx;
				}
			}
		}
		bcnIdx++;
	}
		if( targetIdx != -1 )
	{
		//�r�[�R����������
		u8 *macAdr = GFL_NET_GetBeaconMacAddress(targetIdx);
		if( macAdr != NULL )
		{
			GFL_NET_ConnectToParent( macAdr ); 
			commFunc->commMode_ = FIELD_COMM_MODE_TRY_CONNECT;
			commFunc->seqNo_ = 0;
			ARI_TPrintf("Connect!(Child)\n");
		}
	}
}
	
//--------------------------------------------------------------
//	�r�[�R���̔�r(�l���������E���ɑ҂��󂯒���D�悷��
//	@return 0:�G���[ 1:��P�����̃r�[�R�� 2:��Q�����̃r�[�R��
//--------------------------------------------------------------
static	u8	FIELD_COMM_FUNC_CompareBeacon( const FIELD_COMM_BEACON *firstBcn , const FIELD_COMM_BEACON *secondBcn )
{
	//�l���}�b�N�X�`�F�b�N
	if( firstBcn->memberNum_ == FIELD_COMM_MEMBER_MAX &&
		secondBcn->memberNum_ == FIELD_COMM_MEMBER_MAX )
		return 0;
	
	if(	secondBcn->memberNum_ == FIELD_COMM_MEMBER_MAX ||
		firstBcn->memberNum_ > secondBcn->memberNum_ )
		return 1;
	if( firstBcn->memberNum_ == FIELD_COMM_MEMBER_MAX ||
		firstBcn->memberNum_ < secondBcn->memberNum_ )
		return 2;
	
	if( firstBcn->mode_ > secondBcn->mode_ )
		return 1;
	if( firstBcn->mode_ < secondBcn->mode_ )
		return 2;

	return (GFUser_GetPublicRand(2)+1);
}

//--------------------------------------------------------------
//	�ʐM�̊J�n(�N����t���
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_StartCommWait( FIELD_COMM_FUNC *commFunc )
{
	if( commFunc->commMode_ == FIELD_COMM_MODE_NONE )
	{
		GFL_NET_Changeover(NULL);
	}
	commFunc->commMode_ = FIELD_COMM_MODE_WAIT;
}

//--------------------------------------------------------------
//	�ʐM�̊J�n(�N����T�����
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_StartCommSearch( FIELD_COMM_FUNC *commFunc )
{
	if( commFunc->commMode_ == FIELD_COMM_MODE_NONE )
	{
		GFL_NET_Changeover(NULL);
	}
	commFunc->commMode_ = FIELD_COMM_MODE_SEARCH;
}

//--------------------------------------------------------------
//	�T���ʐM�̊J�n(�e�q���ݒʐM�E���g�p
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_StartCommChangeover( FIELD_COMM_FUNC *commFunc )
{
	//GFL_NET_ChangeoverConnect(NULL);
}


//======================================================================
//	�e��`�F�b�N�֐�
//======================================================================
//--------------------------------------------------------------
// �ʐM���C�u�����@�������E�J���p  
//--------------------------------------------------------------
const BOOL FIELD_COMM_FUNC_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc )
{
	return commFunc->isInitCommSystem_;
}
const BOOL FIELD_COMM_FUNC_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc )
{
	return !commFunc->isInitCommSystem_;
}

//--------------------------------------------------------------
//	�ʐM��ԁH���擾  
//--------------------------------------------------------------
const FIELD_COMM_MODE FIELD_COMM_FUNC_GetCommMode( FIELD_COMM_FUNC *commFunc )
{
	return commFunc->commMode_;
}
//--------------------------------------------------------------
//	�ڑ��l�����擾  
//--------------------------------------------------------------
const int	FIELD_COMM_FUNC_GetMemberNum( FIELD_COMM_FUNC *commFunc )
{
	u8 i;
	u8 num = 1;//�����̕�
	for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
	{
		if( FIELD_COMM_DATA_GetCharaData_State(i) != FCCS_NONE )
			num++;
	}
	return num;
//	return GFL_NET_GetConnectNum();
}

//--------------------------------------------------------------
// �����̃C���f�b�N�X(netID)���擾  
//--------------------------------------------------------------
const int	FIELD_COMM_FUNC_GetSelfIndex( FIELD_COMM_FUNC *commFunc )
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
void	FIELD_COMM_FUNC_Send_SelfData( FIELD_COMM_FUNC *commFunc )
{
	FIELD_COMM_PLAYER_PACKET plPkt;
	PLAYER_WORK *plWork = NULL;
	const VecFx32 *pos;
	u16 dir;
	ZONEID zoneID;

	//�f�[�^���W
	plWork = FIELD_COMM_DATA_GetSelfData_PlayerWork();
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

//	ARI_TPrintf("SEND[ ][%d][%d][%d][%d]\n",plPkt.posX_,plPkt.posY_,plPkt.posZ_,dir);
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
// �����̃f�[�^��M(�v�͑��̃v���C���[�̎�M  
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_Post_SelfData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
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

//	ARI_TPrintf("POST[%d][%d][%d][%d][%d]\n",netID,pkt->posX_,pkt->posY_,pkt->posZ_,dir);
	
	//set
	plWork = FIELD_COMM_DATA_GetCharaData_PlayerWork(netID);
	PLAYERWORK_setPosition( plWork , &pos );
	PLAYERWORK_setDirection( plWork , dir );
	PLAYERWORK_setZoneID( plWork , pkt->zoneID_ );
	FIELD_COMM_DATA_SetCharaData_IsValid( netID , TRUE );

//	ARI_TPrintf("FieldComm PostSelfData[%d]\n",netID);
}

//--------------------------------------------------------------
// ���̃v���C���[�Ƀf�[�^��v������
// Post�̊֐��ł��̂܂�"�v�����ꂽ�L����������"�f�[�^�𑗂�
// @param charaIdx �L�����ԍ�(=netID)
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_Send_RequestData( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , FIELD_COMM_FUNC *commFunc )
{
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	u8	type = reqType;
	const BOOL ret = GFL_NET_SendDataEx( selfHandle , charaIdx , 
			FC_CMD_REQUEST_DATA , 1 , 
			(void*)&type , FALSE , FALSE , FALSE );
		ARI_TPrintf("FieldComm Send RequestData[%d:%d].\n",charaIdx,type);
	if( ret == FALSE ){
		ARI_TPrintf("FieldComm SendRequestData is failue!\n");
	}
}
void	FIELD_COMM_FUNC_Post_RequestData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	const u8 type = *(u8*)pData;
	ARI_TPrintf("FieldComm PostReqData[%d:%d]\n",netID,type);
	switch( type )
	{
	case FCRT_PROFILE:
		FIELD_COMM_FUNC_Send_SelfProfile( netID , commFunc );
		break;
	default:
		OS_TPrintf("Invalid Type[%d]!\n!",type);
		GF_ASSERT( NULL );
		break;
	}
}

//--------------------------------------------------------------
// �����̃v���t�B�[���̑��M(�ŏ��ɑ��镨 ID�Ƃ��L�����̊�{���
// �v�����ꂽ���肾���ɑ���
//--------------------------------------------------------------
typedef struct
{
	u16 ID_;
	u8	sex_:1;
	u8	regionCode_:7;
}FIELD_COMM_CHARA_PROFILE;
void	FIELD_COMM_FUNC_Send_SelfProfile( const int sendNetID ,FIELD_COMM_FUNC *commFunc )
{
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	FIELD_COMM_CHARA_PROFILE profile;
	//FIXME:ID�Ƃ��̐����������ė�����@���킩��Ȃ��̂ŉ�����
	profile.ID_ = 1000+GFL_NET_GetNetID( selfHandle );
	profile.sex_ = 0;
	profile.regionCode_ = 0;
	{
		const BOOL ret = GFL_NET_SendDataEx( selfHandle , sendNetID , 
				FC_CMD_SELF_PROFILE , sizeof( FIELD_COMM_CHARA_PROFILE ) , 
				(void*)&profile , FALSE , FALSE , FALSE );
		ARI_TPrintf("FieldComm Send SelfProfile[%d:%d].\n",sendNetID,profile.ID_);
		if( ret == FALSE ){
			ARI_TPrintf("FieldComm Send SelfProfile is failue!\n");
		}
	}
}
void	FIELD_COMM_FUNC_Post_SelfProfile( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	const FIELD_COMM_CHARA_PROFILE *prof = (FIELD_COMM_CHARA_PROFILE*)pData;
	PLAYER_WORK *plWork = FIELD_COMM_DATA_GetCharaData_PlayerWork( netID );
	ARI_TPrintf("FieldComm Post SelfProfile[%d:%d]\n",netID,prof->ID_);
	
	plWork->mystatus.id = prof->ID_;
	plWork->mystatus.sex = prof->sex_;
	plWork->mystatus.region_code = prof->regionCode_;
	FIELD_COMM_DATA_SetCharaData_State( netID , FCCS_EXIST_DATA );

}

//======================================================================
//�e��R�[���o�b�N
//======================================================================
//--------------------------------------------------------------
// �ʐM���C�u�����@�������E�J���p  
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_FinishInitCallback( void* pWork )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	commFunc->isInitCommSystem_ = TRUE;
}
void	FIELD_COMM_FUNC_FinishTermCallback( void* pWork )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	commFunc->isInitCommSystem_ = FALSE;
}

//--------------------------------------------------------------
// �r�[�R���f�[�^�擾�֐�  
//--------------------------------------------------------------
void*	FIELD_COMM_FUNC_GetBeaconData(void* pWork)
{
	static FIELD_COMM_BEACON beacon;
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	
	if( commFunc->commMode_ == FIELD_COMM_MODE_WAIT )
		beacon.mode_ = 0;
	else
		beacon.mode_ = 1;
	beacon.memberNum_ = FIELD_COMM_FUNC_GetMemberNum(commFunc);

	return (void*)&beacon;
}

//--------------------------------------------------------------
// �r�[�R���f�[�^�T�C�Y�擾�֐� 
//--------------------------------------------------------------
int		FIELD_COMM_FUNC_GetBeaconSize(void *pWork)
{
	return sizeof( FIELD_COMM_BEACON );
}

//--------------------------------------------------------------
// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
//--------------------------------------------------------------
BOOL	FIELD_COMM_FUNC_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 )
{
	return (GameServiceID1==GameServiceID2);
}

//--------------------------------------------------------------
// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
}

//--------------------------------------------------------------
// �ʐM�ؒf���ɌĂ΂��֐�(�I����
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_DisconnectCallBack(void* pWork)
{
}

