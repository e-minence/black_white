
#include "common_def.h"
#include "ds_net.h"
#include "network/net_system.h"
#include "draw.h"
//#include "net_command.h"
//DS����̉�͂Ƃ��E�Ԏ��Ƃ����s��
MATHCRC16Table *crcTable;
PLAYER_STATE	plState_[MY_MAX_NODES];


//����M�֐��p
typedef enum 
{
    FC_CMD_SELFDATA = GFL_NET_CMD_COMMAND_MAX,  //���@�f�[�^
    FC_CMD_REQUEST_DATA,    //�f�[�^�v��
    FC_CMD_SELF_PROFILE,    //�L�����̊�{���
    FC_CMD_COMMON_FLG,      //�ėp�t���O
    FC_CMD_USERDATA,        //�s���p���[�U�[�f�[�^
    FC_CMD_MAX,
}F_COMM_COMMAND_TYPE;

static void RecvTestA(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void RecvTestA(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{ NET_PRINT( "CallFunc A\n" ); }

static void FieldComm_Post_CharaData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);

static const NetRecvFuncTable FieldCommPostTable[FC_CMD_MAX] = {
	{FieldComm_Post_CharaData,			NULL},
	{RecvTestA,							NULL},
	{RecvTestA,							NULL},
	{RecvTestA,							NULL},
	{RecvTestA,							NULL},
};

static const GFLNetInitializeStruct netInitStruct =
{
	FieldCommPostTable,	//NetSamplePacketTbl,  // ��M�֐��e�[�u��
	FC_CMD_MAX,// ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,	// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
	NULL,	// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
	NULL,	// �r�[�R���f�[�^�擾�֐�  
	NULL,	// �r�[�R���f�[�^�T�C�Y�擾�֐� 
	NULL,	// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
	NULL,   ///< �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ʐM���I��������K�v������
    NULL,  ///< �ʐM�s�\�G���[�������B �d���ؒf����K�v������
	NULL,	// �ʐM�ؒf���ɌĂ΂��֐�
	NULL,	// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
	0x346,	//ggid  DP=0x333,RANGER=0x178,WII=0x346
	0,  //���ɂȂ�heapid
	0,  //�ʐM�p��create�����HEAPID
	0,  //wifi�p��create�����HEAPID
	0,0,	// �ʐM�A�C�R��XY�ʒu
	MY_MAX_NODES,//_MAXNUM,	//�ő�ڑ��l��
	MY_DS_LENGTH,//_MAXSIZE,	//�ő呗�M�o�C�g��
	4,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
	TRUE,		// CRC�v�Z
	FALSE,		// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
	FALSE,		//wifi�ʐM���s�����ǂ���
	TRUE,		// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	30//WB_NET_FIELDMOVE_SERVICEID	//GameServiceID
	
};

void	MPDS_Init(void)
{
	HEAPID heapID = 0;
	u8 i;
	
	for( i=0;i<MY_MAX_NODES;i++ )
	{
		MPDS_InitPlayerState(i);
	}
	
	crcTable = (MATHCRC16Table*)mpAlloc(sizeof(MATHCRC16Table));
	MATH_CRC16CCITTInitTable( crcTable );
	GFL_NET_Init( &netInitStruct,NULL,NULL);
	_commSystemInit( MY_DS_LENGTH , heapID );

}

const BOOL MPDS_PostShareData( const u8 *adr , const u8 aid )
{
}

void* GFL_HEAP_AllocClearMemory(int work, int size)
{
	void *adr = mpAlloc((u32)size);
	NETMemSet(adr,0,(u32)size);
	return adr;
}

//���t���[�����L���
typedef struct
{
	u16 posX_;
	u16 posZ_;
	s8  posY_;      //�}�b�N�X�s���Ȃ̂ŁB�ꍇ�ɂ���Ă͂Ȃ���OK�H
	u8  dir_;       //�O���b�h�Ȃ̂ŏ㉺���E��0�`3�œ���
	u16 zoneID_;    //�����͒ʐM�p��ID�Ƃ��ĕϊ����ė}������
	u8  talkState_;
}FIELD_COMM_PLAYER_PACKET;

static void FieldComm_Post_CharaData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	//�����̃f�[�^���ꉞ�Z�b�g���Ă���(�����p�Ɏg��
	const FIELD_COMM_PLAYER_PACKET *pkt = (FIELD_COMM_PLAYER_PACKET*)pData;
	u16 tempX,tempY,tempZ;
	const u8 dirTable[2][4]=
	{
		{ ANIM_UP_STOP,ANIM_DOWN_STOP,ANIM_LEFT_STOP,ANIM_RIGHT_STOP, },
		{ ANIM_UP_WALK,ANIM_DOWN_WALK,ANIM_LEFT_WALK,ANIM_RIGHT_WALK, },
	};
	PLAYER_STATE	*plData = MPDS_GetPlayerState( netID );
	
//	OSReport("POST[%d][%d][%d][%d][%d]\n",netID,pkt->posX_,pkt->posY_,pkt->posZ_,pkt->dir_);
EnterCriticalSection();
	
	plData->isValid_ = TRUE;
	
	//���W�n���Ⴄ�̂Œ��ӁI
	tempX = NETReadSwappedBytes16( &pkt->posX_ );
	tempY = NETReadSwappedBytes16( &pkt->posZ_ );
	tempZ = 0;
//	tempZ = pkt->posY_;

	if( tempX != plData->posX_ ||
		tempY != plData->posY_ ||
		tempZ != plData->posZ_ )
	{
		plData->newAnim_ = dirTable[1][pkt->dir_];
	}
	else
	{
		plData->newAnim_ = dirTable[0][pkt->dir_];
	}
	plData->posX_ = tempX;
	plData->posY_ = tempY;
	plData->posZ_ = tempZ;

LeaveCriticalSection();
}

PLAYER_STATE*	MPDS_GetPlayerState( const u8 idx )
{
	return &plState_[idx];
}

void		MPDS_InitPlayerState( const u8 idx )
{
	plState_[idx].isValid_ = FALSE;
	plState_[idx].oldAnim_ = ANIM_TYPE_MAX;
	plState_[idx].newAnim_ = ANIM_TYPE_MAX;
}
