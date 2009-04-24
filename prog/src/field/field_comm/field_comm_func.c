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
#include "system/net_err.h"
#include "net/network_define.h"

#include "infowin/infowin.h"
#include "gamesystem/playerwork.h"
#include "field_comm_main.h"
#include "field_comm_func.h"
#include "field_comm_data.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
//�ʐM�̃p�P�b�g�o�b�t�@��
#define F_COMM_PACKET_BUFF_NUM (10)

//======================================================================
//	enum
//======================================================================
//����M�֐��p
typedef enum 
{
    FC_CMD_SELFDATA = GFL_NET_CMD_FIELD,	//���@�f�[�^
	FC_CMD_REQUEST_DATA,	//�f�[�^�v��
	FC_CMD_SELF_PROFILE,	//�L�����̊�{���
	FC_CMD_COMMON_FLG,		//�ėp�t���O
	FC_CMD_USERDATA,		//�s���p���[�U�[�f�[�^
}F_COMM_COMMAND_TYPE;

//�p�P�b�g�o�b�t�@�̑��M�^�C�v
typedef enum 
{
	FCPBT_FLG,
	FCPBT_REQ_DATA,

	FCPBT_INVALID,
}F_COMM_PACKET_BUFF_TYPE;

//======================================================================
//	typedef struct
//======================================================================
//���t���[�����L���
typedef struct
{
	u16 posX_;
	u16 posZ_;
	s8	posY_;		//�}�b�N�X�s���Ȃ̂ŁB�ꍇ�ɂ���Ă͂Ȃ���OK�H
	u8	dir_;		//�O���b�h�Ȃ̂ŏ㍶���E��0�`3�œ���(�������̂�0x4000�P��
	u16	zoneID_;	//�����͒ʐM�p��ID�Ƃ��ĕϊ����ė}������
	u8	talkState_;
}FIELD_COMM_PLAYER_PACKET;

//�đ��M�p�p�P�b�g���
typedef struct
{
	u8	type_;
	u8	id_;
	u16	value_;
	u8	sendID_;
}FIELD_COMM_PACKET_BUFF_DATA;

struct _FIELD_COMM_FUNC
{
	HEAPID	heapID_;
	FIELD_COMM_MODE commMode_;
	u8	seqNo_;
	
	BOOL	isInitCommSystem_;
	
	//����M�T�|�[�g�֌W
	FIELD_COMM_PLAYER_PACKET plPkt_;

	F_COMM_SYNC_TYPE	sendSyncType_;
	F_COMM_SYNC_TYPE	postSyncType_[FIELD_COMM_MEMBER_MAX];	//�����ʐM�p
	//�đ��M�p�֐�
	u8	pktBuffStart_;
	u8	pktBuffEnd_;
	FIELD_COMM_PACKET_BUFF_DATA pktBuffData_[F_COMM_PACKET_BUFF_NUM];

	//��b�֌W
	u8		talkID_;		//��b�Ώ�
	u8		talkPosX_;
	u8		talkPosZ_;		//��b�Ώۈʒu

	//�s���I���n
	u8		selectAction_;		//�e���I�񂾍s��
	BOOL	isActionReturn_;	//�s���I���̕Ԏ����͂������H
	BOOL	actionReturn_;		//�s���I���̌���

	BOOL	isGetUserData_;
};
//�r�[�R��
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
void	FIELD_COMM_FUNC_InitCommData_StartTalk( FIELD_COMM_FUNC *commFunc );

//�e��`�F�b�N�֐�
const BOOL FIELD_COMM_FUNC_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc );
const BOOL FIELD_COMM_FUNC_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc );
const FIELD_COMM_MODE FIELD_COMM_FUNC_GetCommMode( FIELD_COMM_FUNC *commFunc );
const int	FIELD_COMM_FUNC_GetMemberNum( FIELD_COMM_FUNC *commFunc );
const int	FIELD_COMM_FUNC_GetSelfIndex( FIELD_COMM_FUNC *commFunc );
const BOOL FIELD_COMM_FUNC_IsReserveTalk( FIELD_COMM_FUNC *commFunc );
void FIELD_COMM_FUNC_ResetReserveTalk( FIELD_COMM_FUNC *commFunc );
void FIELD_COMM_FUNC_GetTalkParterData_ID( u8 *ID , FIELD_COMM_FUNC *commFunc );
void FIELD_COMM_FUNC_GetTalkParterData_Pos( u8 *posX , u8 *posZ , FIELD_COMM_FUNC *commFunc );
const F_COMM_ACTION_LIST FIELD_COMM_FUNC_GetSelectAction( FIELD_COMM_FUNC *commFunc );
const BOOL FIELD_COMM_FUNC_IsActionReturn( FIELD_COMM_FUNC *commFunc );
const BOOL FIELD_COMM_FUNC_GetActionReturn( FIELD_COMM_FUNC *commFunc );
void FIELD_COMM_FUNC_ResetGetUserData( FIELD_COMM_FUNC *commFunc );
const BOOL FIELD_COMM_FUNC_IsGetUserData( FIELD_COMM_FUNC *commFunc );
//����M�֐�
const BOOL	FIELD_COMM_FUNC_Send_SelfData( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_SelfData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
const BOOL	FIELD_COMM_FUNC_Send_RequestData( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , FIELD_COMM_FUNC *commFunc );
static const BOOL	FIELD_COMM_FUNC_Send_RequestDataFunc( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , const BOOL isReSend , FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_RequestData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
const BOOL	FIELD_COMM_FUNC_Send_SelfProfile( const int sendNetID ,FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_SelfProfile( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
const BOOL	FIELD_COMM_FUNC_Send_CommonFlg( const F_COMM_COMMON_FLG flg , const u16 val , const u8 sendID );
static const BOOL	FIELD_COMM_FUNC_Send_CommonFlgFunc( const F_COMM_COMMON_FLG flg , const u16 val , const u8 sendID , BOOL isReSend , FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_CommonFlg( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
const BOOL	FIELD_COMM_FUNC_Send_UserData( F_COMM_USERDATA_TYPE type , const u8 sendID );
void	FIELD_COMM_FUNC_Post_UserData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
u8*		FIELD_COMM_FUNC_Post_UserData_Buff( int netID, void* pWork , int size );

//�����p�R�}���h
void FIELD_COMM_FUNC_Send_SyncCommand( const F_COMM_SYNC_TYPE type , FIELD_COMM_FUNC *commFunc );
const BOOL FIELD_COMM_FUNC_CheckSyncFinish( const F_COMM_SYNC_TYPE type , const u8 checkBit , FIELD_COMM_FUNC *commFunc );
const u8 FIELD_COMM_FUNC_GetBit_TalkMember( FIELD_COMM_FUNC *commFunc );

//�ʐM�o�b�t�@�p
void	FIELD_COMM_FUNC_SetPacketBuff( const F_COMM_PACKET_BUFF_TYPE type , const u8 id , const u8 value , const u8 sendID , FIELD_COMM_FUNC *commFunc );
const BOOL	FIELD_COMM_FUNC_SendPacketBuff( FIELD_COMM_FUNC *commFunc );

//�e��R�[���o�b�N
void	FIELD_COMM_FUNC_FinishInitCallback( void* pWork );
void	FIELD_COMM_FUNC_FinishTermCallback( void* pWork );
void*	FIELD_COMM_FUNC_GetBeaconData(void* pWork);		// �r�[�R���f�[�^�擾�֐�  
int		FIELD_COMM_FUNC_GetBeaconSize(void* pWork);		// �r�[�R���f�[�^�T�C�Y�擾�֐� 
BOOL	FIELD_COMM_FUNC_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 ); // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
void	FIELD_COMM_FUNC_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);		// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
void	FIELD_COMM_FUNC_DisconnectCallBack(void* pWork);	// �ʐM�ؒf���ɌĂ΂��֐�(�I����

static const NetRecvFuncTable FieldCommRecvTable[] = {
	{ FIELD_COMM_FUNC_Post_SelfData		,NULL },
	{ FIELD_COMM_FUNC_Post_RequestData	,NULL },
	{ FIELD_COMM_FUNC_Post_SelfProfile	,NULL },
	{ FIELD_COMM_FUNC_Post_CommonFlg	,NULL },
	{ FIELD_COMM_FUNC_Post_UserData		,FIELD_COMM_FUNC_Post_UserData_Buff	 },
};

static FIELD_COMM_FUNC	*commFunc_ = NULL;

//--------------------------------------------------------------
//	�V�X�e��������
//	���̎��_�ł͒ʐM�͊J�n���܂���
//--------------------------------------------------------------
FIELD_COMM_FUNC* FIELD_COMM_FUNC_InitSystem( HEAPID heapID )
{
	if( commFunc_ == NULL )
	{
		u8 i;
		commFunc_ = GFL_HEAP_AllocMemory( heapID , sizeof(FIELD_COMM_FUNC) );
		commFunc_->heapID_ = heapID;
		commFunc_->commMode_ = FIELD_COMM_MODE_NONE;
		commFunc_->sendSyncType_ = FCST_MAX;
		for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
		{
			commFunc_->postSyncType_[i] = FCST_MAX;
		}
		commFunc_->pktBuffStart_ = 0;
		commFunc_->pktBuffEnd_ = 0;
		for( i=0;i<F_COMM_PACKET_BUFF_NUM;i++ )
		{
			commFunc_->pktBuffData_[i].type_ = FCPBT_INVALID;
		}
	}
	if( GFL_NET_IsInit() && INFOWIN_IsInitComm() == FALSE )
	{
		commFunc_->isInitCommSystem_ = TRUE;
	}
	else
	{
		commFunc_->isInitCommSystem_ = FALSE;
	}
	//�ʐM������������Ă����Ȃ�A�C�R���������[�h����
	if( commFunc_->isInitCommSystem_ == TRUE )
		GFL_NET_ReloadIcon();
	return commFunc_;
	
#if 0
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
#endif
}

//--------------------------------------------------------------
//	�V�X�e���J��
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_TermSystem( FIELD_COMM_FUNC *commFunc )
{
	GFL_HEAP_FreeMemory( commFunc );
}


static void FIELD_COMM_FUNC_Connect(void* pWork,int hardID)
{
    if(GFL_NET_IsParentMachine()==FALSE){
        FIELD_COMM_FUNC *commFunc = pWork;
        commFunc->commMode_ = FIELD_COMM_MODE_TRY_CONNECT;
        commFunc->seqNo_ = 0;
        ARI_TPrintf("Connect!(Child)\n");
    }
}


//--------------------------------------------------------------
//	�ʐM�J�n
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_InitCommSystem( FIELD_COMM_FUNC *commFunc )
{
	GFLNetInitializeStruct aGFLNetInit = {
		FieldCommRecvTable,	//NetSamplePacketTbl,  // ��M�֐��e�[�u��
		NELEMS(FieldCommRecvTable), // ��M�e�[�u���v�f��
        FIELD_COMM_FUNC_Connect,    ///< �n�[�h�Őڑ��������ɌĂ΂��
        NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
        NULL,	// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
		NULL,	// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
		FIELD_COMM_FUNC_GetBeaconData,		// �r�[�R���f�[�^�擾�֐�  
		FIELD_COMM_FUNC_GetBeaconSize,		// �r�[�R���f�[�^�T�C�Y�擾�֐� 
		FIELD_COMM_FUNC_CheckConnectService,	// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
		FIELD_COMM_FUNC_ErrorCallBack,		// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂��
        NULL,  //FatalError
        FIELD_COMM_FUNC_DisconnectCallBack,	// �ʐM�ؒf���ɌĂ΂��֐�(�I����
		NULL,	// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g	
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
#if DEB_ARI&0
		0x346,	//ggid  DP=0x333,RANGER=0x178,WII=0x346
#else
		0x444,	//ggid  DP=0x333,RANGER=0x178,WII=0x346
#endif
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
#if DEB_ARI&0
		30//GameServiceID
#else
		WB_NET_FIELDMOVE_SERVICEID,	//GameServiceID
#endif
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
	};
	GFL_NET_Init( &aGFLNetInit , FIELD_COMM_FUNC_FinishInitCallback , (void*)commFunc );
	commFunc->commMode_ = FIELD_COMM_MODE_NONE;
}

//--------------------------------------------------------------
//	�ʐM�V�X�e���I��
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_TermCommSystem( FIELD_COMM_FUNC *commFunc )
{
	commFunc->isInitCommSystem_ = FALSE;
	GFL_NET_Exit( FIELD_COMM_FUNC_FinishTermCallback );
}

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
				//commFunc->commMode_ = FIELD_COMM_MODE_TRY_CONNECT;
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

	if( commFunc->commMode_ == FIELD_COMM_MODE_CONNECT )
	{
		//�o�b�t�@�̑��M����
		FIELD_COMM_FUNC_SendPacketBuff( commFunc );
	}

#if DEB_ARI
	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y )
	{
		commFunc->commMode_ = FIELD_COMM_MODE_NONE;

		FIELD_COMM_FUNC_TermCommSystem(commFunc);
		commFunc->isInitCommSystem_ = FALSE;
		NetErr_ErrorSet();	
		//FIELD_COMM_FUNC_Send_CommonFlg( FCCF_TEST , 0 , GFL_NET_SENDID_ALLUSER );
	}
#endif
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
	if( targetIdx != -1 && commFunc->commMode_ != FIELD_COMM_MODE_TRY_CONNECT)
	{
		//�r�[�R����������
		u8 *macAdr = GFL_NET_GetBeaconMacAddress(targetIdx);
		if( macAdr != NULL )
		{
			GFL_NET_ConnectToParent( macAdr ); 
			commFunc->commMode_ = FIELD_COMM_MODE_CONNECTING;
		//	commFunc->seqNo_ = 0;
		//	ARI_TPrintf("Connect!(Child)\n");
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
		//GFL_NET_Changeover(NULL);
        GFL_NET_StartBeaconScan();
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

//--------------------------------------------------------------
//	��b�J�n���ɏ������������
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_InitCommData_StartTalk( FIELD_COMM_FUNC *commFunc )
{
	commFunc->selectAction_ = FCAL_SELECT;
	commFunc->isActionReturn_ = FALSE;
	commFunc->actionReturn_ = FALSE;
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
	/*
	u8 i;
	u8 num = 1;//�����̕�
	for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
	{
		if( FIELD_COMM_DATA_GetCharaData_State(i) != FCCS_NONE )
			num++;
	}
	return num;
	*/
	return GFL_NET_GetConnectNum();
}

//--------------------------------------------------------------
// �����̃C���f�b�N�X(netID)���擾  
//--------------------------------------------------------------
const int	FIELD_COMM_FUNC_GetSelfIndex( FIELD_COMM_FUNC *commFunc )
{
	GFL_NETHANDLE *handle =GFL_NET_HANDLE_GetCurrentHandle();
	return GFL_NET_GetNetID( handle );
}

//--------------------------------------------------------------
// ��b�\��̃f�[�^�擾  
//--------------------------------------------------------------
void FIELD_COMM_FUNC_GetTalkParterData_ID( u8 *ID , FIELD_COMM_FUNC *commFunc )
{
	*ID = commFunc->talkID_;
}
void FIELD_COMM_FUNC_GetTalkParterData_Pos( u8 *posX , u8 *posZ , FIELD_COMM_FUNC *commFunc )
{
	*posX = commFunc->talkPosX_;
	*posZ = commFunc->talkPosZ_;
}

//--------------------------------------------------------------
// �e�̑I�񂾍s���̎擾  (FCAL_SELECT �͖���
//--------------------------------------------------------------
const F_COMM_ACTION_LIST FIELD_COMM_FUNC_GetSelectAction( FIELD_COMM_FUNC *commFunc )
{
	return (F_COMM_ACTION_LIST)commFunc->selectAction_;
}
//--------------------------------------------------------------
// �q���s����OK�������̕Ԏ��ƒ��M�m�F  
//--------------------------------------------------------------
const BOOL FIELD_COMM_FUNC_IsActionReturn( FIELD_COMM_FUNC *commFunc )
{
	return commFunc->isActionReturn_;
}
const BOOL FIELD_COMM_FUNC_GetActionReturn( FIELD_COMM_FUNC *commFunc )
{
	return commFunc->actionReturn_;
}

//--------------------------------------------------------------
// ���[�U�[�f�[�^�n  
//--------------------------------------------------------------
void FIELD_COMM_FUNC_ResetGetUserData( FIELD_COMM_FUNC *commFunc )
{
	commFunc->isGetUserData_ = FALSE;
}
const BOOL FIELD_COMM_FUNC_IsGetUserData( FIELD_COMM_FUNC *commFunc )
{
	return commFunc->isGetUserData_;
}

//======================================================================
//����M�֐�
//======================================================================
//--------------------------------------------------------------
// �����̃f�[�^���M  
//--------------------------------------------------------------
const BOOL	FIELD_COMM_FUNC_Send_SelfData( FIELD_COMM_FUNC *commFunc )
{
	PLAYER_WORK *plWork = NULL;
	const VecFx32 *pos;
	u16 dir;
	ZONEID zoneID;
	u8	talkState;

	//�f�[�^���W
	plWork = FIELD_COMM_DATA_GetSelfData_PlayerWork();
	zoneID = PLAYERWORK_getZoneID( plWork );
	pos = PLAYERWORK_getPosition( plWork );
	dir = PLAYERWORK_getDirection( plWork );
	talkState = FIELD_COMM_DATA_GetTalkState( FCD_SELF_INDEX );
	//�p�P�b�g�ɃZ�b�g
	commFunc->plPkt_.zoneID_ = zoneID;
	commFunc->plPkt_.posX_ = F32_CONST( pos->x );
	commFunc->plPkt_.posY_ = (int)F32_CONST( pos->y );
	commFunc->plPkt_.posZ_ = F32_CONST( pos->z );
	commFunc->plPkt_.dir_ = dir;
	commFunc->plPkt_.talkState_ = talkState;

	ARI_TPrintf("SEND[ ][%d][%d][%d][%x]\n",commFunc->plPkt_.posX_,commFunc->plPkt_.posY_,commFunc->plPkt_.posZ_,dir);
	{
		GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
		const BOOL ret = GFL_NET_SendDataEx( selfHandle , GFL_NET_SENDID_ALLUSER , 
				FC_CMD_SELFDATA , sizeof(FIELD_COMM_PLAYER_PACKET) , 
				(void*)&commFunc->plPkt_ , FALSE , TRUE , TRUE );
		if( ret == FALSE ){
			//�����͍đ������Ȃ��ŗǂ����ȁE�E�E
			//ARI_TPrintf("FieldComm SendSelfData is failue!\n");
		}
		return ret;
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
	dir = pkt->dir_;//�ʐM�L�����̓C���f�b�N�X��4�����Ȃ̂�

	ARI_TPrintf("POST[%d][%d][%d][%d][%x]\n",netID,pkt->posX_,pkt->posY_,pkt->posZ_,dir);
	
	//set
	plWork = FIELD_COMM_DATA_GetCharaData_PlayerWork(netID);
	PLAYERWORK_setPosition( plWork , &pos );
	PLAYERWORK_setDirection( plWork , dir );
	PLAYERWORK_setZoneID( plWork , pkt->zoneID_ );
	FIELD_COMM_DATA_SetCharaData_IsValid( netID , TRUE );
	FIELD_COMM_DATA_SetTalkState( netID , pkt->talkState_ );

//	ARI_TPrintf("FieldComm PostSelfData[%d]\n",netID);
}

//--------------------------------------------------------------
// ���̃v���C���[�Ƀf�[�^��v������
// Post�̊֐��ł��̂܂�"�v�����ꂽ�L����������"�f�[�^�𑗂�
// @param charaIdx �L�����ԍ�(=netID)
//--------------------------------------------------------------
const BOOL	FIELD_COMM_FUNC_Send_RequestData( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , FIELD_COMM_FUNC *commFunc )
{
	//�đ��@�\�������������߁A��i���܂��Ĉ����̒ǉ��ɑΉ�
	return FIELD_COMM_FUNC_Send_RequestDataFunc( charaIdx , reqType , FALSE , commFunc );
}
static const BOOL	FIELD_COMM_FUNC_Send_RequestDataFunc( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , const BOOL isReSend , FIELD_COMM_FUNC *commFunc )
{
	if( isReSend == FALSE )
	{
		BOOL ret = FIELD_COMM_FUNC_SendPacketBuff( commFunc );
		if( ret == FALSE )
		{
			//���M���s������o�b�t�@�ɂ��߂ďI���
			FIELD_COMM_FUNC_SetPacketBuff( FCPBT_REQ_DATA , reqType , 0 , charaIdx , commFunc );
			ARI_TPrintf("FieldComm SendRequestData is failue![Buffer]\n");
			return FALSE;
		}
	}
	{
		GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
		u8	type = reqType;
		const BOOL ret = GFL_NET_SendDataEx( selfHandle , charaIdx , 
				FC_CMD_REQUEST_DATA , 1 , 
				(void*)&type , FALSE , FALSE , FALSE );
			ARI_TPrintf("FieldComm Send RequestData[%d:%d].\n",charaIdx,type);
		if( ret == FALSE ){
			ARI_TPrintf("FieldComm SendRequestData is failue!\n");
			if( isReSend == FALSE )
			{
				//�o�b�t�@�ɃZ�b�g
				FIELD_COMM_FUNC_SetPacketBuff( FCPBT_REQ_DATA , reqType , 0 , charaIdx , commFunc );
			}
		}
		return ret;
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
const BOOL	FIELD_COMM_FUNC_Send_SelfProfile( const int sendNetID ,FIELD_COMM_FUNC *commFunc )
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
		return ret;
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

//--------------------------------------------------------------
// �ėp�t���O
//--------------------------------------------------------------
typedef struct
{
	u8	flg_;
	u16 value_;
}FIELD_COMM_COMMONFLG_PACKET;
const BOOL	FIELD_COMM_FUNC_Send_CommonFlg( const F_COMM_COMMON_FLG flg , const u16 val , const u8 sendID)
{
	//�đ��@�\�������������߁A��i���܂��Ĉ����̒ǉ��ɑΉ�
	return FIELD_COMM_FUNC_Send_CommonFlgFunc( flg , val , sendID , FALSE , commFunc_ );
}
static const BOOL	FIELD_COMM_FUNC_Send_CommonFlgFunc( const F_COMM_COMMON_FLG flg , const u16 val , const u8 sendID , BOOL isReSend , FIELD_COMM_FUNC *commFunc )
{
	//�܂��o�b�t�@�̏������s��
	if( isReSend == FALSE )
	{
		BOOL ret = FIELD_COMM_FUNC_SendPacketBuff( commFunc );
		if( ret == FALSE )
		{
			//���M���s������o�b�t�@�ɂ��߂ďI���
			ARI_TPrintf("FieldComm Send commonFlg is failue[Buffer]!\n");
			FIELD_COMM_FUNC_SetPacketBuff( FCPBT_FLG , flg , val , sendID , commFunc );
			return FALSE;
		}
	}
	{
		GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
		FIELD_COMM_COMMONFLG_PACKET pkt;
		pkt.flg_ = flg;
		pkt.value_ = val;
		{
			const BOOL ret = GFL_NET_SendDataEx( selfHandle , sendID , 
					FC_CMD_COMMON_FLG , sizeof( FIELD_COMM_COMMONFLG_PACKET ) , 
					(void*)&pkt , FALSE , FALSE , FALSE );
			ARI_TPrintf("FieldComm Send commonFlg[%d:%d:%d].\n",sendID,flg,val);
			if( ret == FALSE ){
				ARI_TPrintf("FieldComm Send commonFlg is failue!\n");
				if( isReSend == FALSE )
				{
					FIELD_COMM_FUNC_SetPacketBuff( FCPBT_FLG , flg , val , sendID , commFunc );
				}
			}
			return ret;
		}
	}
}
void	FIELD_COMM_FUNC_Post_CommonFlg( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	const FIELD_COMM_COMMONFLG_PACKET *pkt = (FIELD_COMM_COMMONFLG_PACKET*)pData;
	const F_COMM_TALK_STATE talkState = FIELD_COMM_DATA_GetTalkState( FCD_SELF_INDEX );
	ARI_TPrintf("FieldComm PostCommonFlg[%d:%d:%d]\n",netID,pkt->flg_,pkt->value_);
	switch( pkt->flg_ )
	{
	case FCCF_TALK_REQUEST:		//��b�v��
		if( talkState == FCTS_NONE )
		{	
			FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_RESERVE_TALK );
			commFunc->talkPosX_ = pkt->value_ & 0x00FF;
			commFunc->talkPosZ_ = pkt->value_>>8;
			commFunc->talkID_ = netID;
		}
		else
		{
			FIELD_COMM_FUNC_Send_CommonFlg( FCCF_TALK_UNPOSSIBLE , 0xf , netID );
		}
		break;

	case FCCF_TALK_UNPOSSIBLE:	//�v���ɑ΂��Ė���
		GF_ASSERT( talkState == FCTS_WAIT_TALK );
		FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_UNPOSSIBLE );
		break;

	case FCCF_TALK_ACCEPT:		//�v���ɑ΂��ċ���
		GF_ASSERT( talkState == FCTS_WAIT_TALK );
		FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_ACCEPT );
		commFunc->talkID_ = netID;
		break;

	case FCCF_ACTION_SELECT:	//�e���I�񂾍s��
		GF_ASSERT( pkt->value_ < FCAL_MAX );
		commFunc->selectAction_ = pkt->value_;
		break;
	
	case FCCF_ACTION_RETURN:	//�e�̑I�񂾍s���ɑ΂���q�̕Ԏ�
		commFunc->isActionReturn_ = TRUE;
		commFunc->actionReturn_ = pkt->value_;
		break;

	case FCCF_SYNC_TYPE:
		commFunc->postSyncType_[netID] = pkt->value_;
		break;

	case FCCF_TEST:
		break;

	default:
		OS_TPrintf("Invalid flg[%d]!\n!",pkt->flg_);
		GF_ASSERT( NULL );
		break;
	}
}

//--------------------------------------------------------------
// �s���p���[�U�[�f�[�^
//--------------------------------------------------------------
const BOOL	FIELD_COMM_FUNC_Send_UserData( F_COMM_USERDATA_TYPE type , const u8 sendID )
{
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	void* selfUserData = FIELD_COMM_DATA_GetSelfUserData( type );
	const u32 dataSize = FIELD_COMM_DATA_GetUserDataSize( type );
	
	{
		const BOOL ret = GFL_NET_SendDataEx( selfHandle , sendID , 
				FC_CMD_USERDATA , dataSize , 
				selfUserData , FALSE , FALSE , TRUE );
		ARI_TPrintf("FieldComm Send UserData[%d:%d:%d].\n",sendID,type,dataSize);
		if( ret == FALSE ){
			ARI_TPrintf("FieldComm Send UserData is failue!\n");
		}
		return ret;
	}
}
void	FIELD_COMM_FUNC_Post_UserData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	commFunc->isGetUserData_ = TRUE;
	ARI_TPrintf("FieldComm PostUserData[%d:%d]\n",netID,size);

}
u8*		FIELD_COMM_FUNC_Post_UserData_Buff( int netID, void* pWork , int size )
{
	void* userData = FIELD_COMM_DATA_GetPartnerUserData( FCUT_MAX );
	return (u8*)userData;
}

//======================================================================
//�����p�R�}���h
//======================================================================
void FIELD_COMM_FUNC_Send_SyncCommand( const F_COMM_SYNC_TYPE type , FIELD_COMM_FUNC *commFunc )
{
	commFunc->sendSyncType_ = type;
	FIELD_COMM_FUNC_Send_CommonFlg( FCCF_SYNC_TYPE , type , GFL_NET_SENDID_ALLUSER );
//	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
//	GFL_NET_TimingSyncStart( selfHandle , type );
}
const BOOL FIELD_COMM_FUNC_CheckSyncFinish( const F_COMM_SYNC_TYPE type , const u8 checkBit , FIELD_COMM_FUNC *commFunc )
{
	u8 i;
	ARI_TPrintf("SYNC CHECK [%d:%x]",type ,checkBit);
	for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
	{
		if( (1<<i)&checkBit )
		{
			ARI_TPrintf("[%d]",i);
			if( commFunc->sendSyncType_ != commFunc->postSyncType_[i] )
			{
				ARI_TPrintf("_FALSE_E\n");
				return FALSE;
			}
		}
	}

	commFunc->sendSyncType_ = FCST_MAX;
	for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
	{
		if( (1<<i)&checkBit )
		{
			commFunc->postSyncType_[i] = FCST_MAX;
		}
	}
	ARI_TPrintf("_TRUE_E\n");
	return TRUE;
//	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
//	return GFL_NET_IsTimingSync( selfHandle , type );
}

//--------------------------------------------------------------
// �����Ɖ�b�����ID��bit���擾  
//--------------------------------------------------------------
const u8 FIELD_COMM_FUNC_GetBit_TalkMember( FIELD_COMM_FUNC *commFunc )
{
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	const u8 selfID = GFL_NET_GetNetID( selfHandle );
	return (1<<selfID)|(1<<commFunc->talkID_);
}

//======================================================================
//�ʐM�o�b�t�@�p
//======================================================================
//--------------------------------------------------------------
// ���M�o�b�t�@�Z�b�g  
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_SetPacketBuff( const F_COMM_PACKET_BUFF_TYPE type , const u8 id , const u8 value , const u8 sendID , FIELD_COMM_FUNC *commFunc )
{
	//���ɖ߂��悤�Ɏ���Ă���
	u8 idx = commFunc->pktBuffEnd_;
	commFunc->pktBuffEnd_++;
	if( commFunc->pktBuffEnd_ >= F_COMM_PACKET_BUFF_NUM )
	{
		commFunc->pktBuffEnd_ -= F_COMM_PACKET_BUFF_NUM;
	}
	//�o�b�t�@�I�[�o�[�`�F�b�N
	if( commFunc->pktBuffStart_ == commFunc->pktBuffEnd_ )
	{
		GF_ASSERT_MSG(NULL,"Field comm packet buffer is over!!");
		//TODO:�^�C���A�E�g�ʐM�G���[�����ɂ��Ă��܂��H
		commFunc->pktBuffEnd_ = idx;
		return;
	}

	commFunc->pktBuffData_[commFunc->pktBuffEnd_].type_ = type;
	commFunc->pktBuffData_[commFunc->pktBuffEnd_].id_ = id;
	commFunc->pktBuffData_[commFunc->pktBuffEnd_].value_ = value;
	commFunc->pktBuffData_[commFunc->pktBuffEnd_].sendID_ = sendID;
	ARI_TPrintf("FieldComm PktBuff Set[%d-%d]\n",commFunc->pktBuffStart_, commFunc->pktBuffEnd_);
}

//--------------------------------------------------------------
// ���M�o�b�t�@���M  
//--------------------------------------------------------------
const BOOL	FIELD_COMM_FUNC_SendPacketBuff( FIELD_COMM_FUNC *commFunc )
{
	if( commFunc->pktBuffStart_ == commFunc->pktBuffEnd_ )
	{
		//�o�b�t�@����
		return TRUE;
	}

	while( commFunc->pktBuffStart_ != commFunc->pktBuffEnd_ )
	{
		const FIELD_COMM_PACKET_BUFF_DATA *pktBuff = &commFunc->pktBuffData_[commFunc->pktBuffStart_];
		BOOL ret;
		switch( pktBuff->type_ )
		{
		case FCPBT_FLG:
			ret = FIELD_COMM_FUNC_Send_CommonFlgFunc( pktBuff->id_ , pktBuff->value_ , pktBuff->sendID_ , TRUE , commFunc );
			break;
		case FCPBT_REQ_DATA:
			ret = FIELD_COMM_FUNC_Send_RequestDataFunc( pktBuff->sendID_ , pktBuff->id_ , TRUE , commFunc );
			break;
		}
		if( ret == FALSE )
		{
			//���M���s
			return FALSE;
		}
		else
		{
			//���M����
			ARI_TPrintf("FieldComm PktBuff Send[%d-%d]\n",commFunc->pktBuffStart_, commFunc->pktBuffEnd_);
			commFunc->pktBuffStart_++;
			if( commFunc->pktBuffStart_ >= F_COMM_PACKET_BUFF_NUM )
			{
				commFunc->pktBuffStart_ -= F_COMM_PACKET_BUFF_NUM;
			}

		}
	}
	return TRUE;
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
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	NetErr_ErrorSet();
	FIELD_COMM_FUNC_TermCommSystem(commFunc);
	commFunc->isInitCommSystem_ = FALSE;
}

//--------------------------------------------------------------
// �ʐM�ؒf���ɌĂ΂��֐�(�I����
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_DisconnectCallBack(void* pWork)
{
}

