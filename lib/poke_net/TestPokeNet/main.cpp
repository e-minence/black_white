/*---------------------------------------------------------------------------*
  Pokemon Network Library Test Client

  --����--
  [2010/03/25] Shinichiro Yamashita
  �Egds_header�C���ɔ����A�T���v�����C���E�X�V

  [2010/03/17] Shinichiro Yamashita
  �Egds_header�C���ɔ����A�T���v�����C���E�X�V
  �Ecrc�l���v�Z����悤�ɏC��

  [2010/02/15] Shinichiro Yamashita
  small fix

  [2010/02/08] Shinichiro Yamashita
  �V�K�쐬
 *---------------------------------------------------------------------------*/


// ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
// *                                                                  *
// * ��1.�u�o�g���r�f�I�v��ID�ɂ���                                 *
// *  �N���C�A���g���ɂ�DB���ID���G���R�[�h�����l��n���g�p����B    *
// *  ���N�G�X�g�Ƃ��ēn���r�f�IID�͑S�ăG���R�[�h�ςݐ��l���w�肷��B*
// *                                                                  *
// * ��2.�u�C�x���g�r�f�I�v��ID�ɂ���                               *
// *  DB���ID��+900000000000ULL�����l�B                              *
// *                                                                  *
// * ��3�D�o�g���r�f�I�̃A�b�v���[�h���Ƀ����L���O�e�[�u����          *
// *  ���܂��Ă��Ȃ��ꍇ�́A���̏�ő}�������B                      *
// *  (�ʏ�A�����L���O�e�[�u���̑���͊m�菈�����ɍs����B)        *
// *                                                                  *
// ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **


//====================================
// �C���N���[�h
//====================================

//#define ___POKE_NET_BUILD_WINDOWS___
#include "main.h"

#include "../poke_net/poke_net_gds.h"	// POKE_NET���C�u�����̃w�b�_
#include "../poke_net/poke_net_opt.h"	// �o�g���r�f�IID�̕ϊ����\�b�h���錾���Ă���܂��B
#include "Nitro/crc.h"	// CRC�v�Z


//====================================
// �}�N����`
//====================================

// �T�[�o��URL�܂���IP�A�h���X�ƃ|�[�g�ԍ����w�肵�Ă��������B
#define SYACHI_SERVER_URL	("pkgdstest.nintendo.co.jp")
#define SYACHI_SERVER_PORT	(12401)
//#define SYACHI_SERVER_URL	("192.168.0.133")
//#define SYACHI_SERVER_PORT	(23457)

// BATTLEID_AUTO_ENCODE�}�N�����L���̏ꍇ�ABattleID��DB��̒l���w�肵�Ă��������B
#define BATTLEID_AUTO_ENCODE

// ���̒l��PID�Ƃ��đ��M����ƁA�F�X�ȃ`�F�b�N�����������̂Œ��ӁI
#define TRAFFIC_CHECK_PID		(9999999)

// Auth�\���̂ɐݒ肷��l���`
//#define	AUTH_PID				(2345678)	// PID
#define	AUTH_PID				(TRAFFIC_CHECK_PID)	// PID
#define AUTH_ROMCODE			(20)		// ROM �R�[�h
#define AUTH_LANGCODE			(1)			// ����R�[�h

// �o�g���r�f�I�o�[�W�����B�V�o�[�W�������o�Ȃ�����100�B
#define	BATTLEVIDEO_SERVER_VER		(100)

// �C�x���g�o�g���r�f�I��ID�֕ϊ�����B
#define TO_EVENT_BATTLEVIDEO_ID(x)	(x + 900000000000ULL)


//====================================
// �ϐ�����`
//====================================

const wchar_t g_TrainerName[]	= L"���p";									// �v���t�B�[���ƃ~���[�W�J���f�[�^�ƃo�g���{�̂Ɏg�p
const wchar_t g_TrainerNmae2[]  = L"�~�j�X�J�[�g��舤�����߂ċ���";			// �o�g���{�̂̃g���[�i�[���Ɏg�p(���\�[�X�ł͖��g�p)
const wchar_t g_MusicalTitle[]	= L"�������[���܂�ǂ��B�G���[�[�̂䂤����";	// �~���[�W�J���^�C�g��

// *
// * �G���[�R�[�h�̃��b�Z�[�W��`
// *
const char* g_ErrorMessageList[] = {
	"NONE:�G���[�Ȃ�",									// [ 0] POKE_NET_GDS_LASTERROR_NONE
	"NOTINITIALIZED:����������Ă��Ȃ�",				// [ 1] POKE_NET_GDS_LASTERROR_NOTINITIALIZED
	"ILLEGALREQUEST:���N�G�X�g���ُ�",					// [ 2] POKE_NET_GDS_LASTERROR_ILLEGALREQUEST
	"CREATESOCKET:�\�P�b�g�����Ɏ��s",					// [ 3] POKE_NET_GDS_LASTERROR_CREATESOCKET
	"IOCTRLSOCKET:�\�P�b�g�̃m���u���b�L���O���Ɏ��s",	// [ 4] POKE_NET_GDS_LASTERROR_IOCTRLSOCKET
	"NETWORKSETTING:�l�b�g���[�N�ݒ莸�s",				// [ 5] POKE_NET_GDS_LASTERROR_NETWORKSETTING
	"CREATETHREAD:�X���b�h�������s",					// [ 6] POKE_NET_GDS_LASTERROR_CREATETHREAD
	"CONNECT:�ڑ����s",									// [ 7] POKE_NET_GDS_LASTERROR_CONNECT
	"SENDREQUEST:���N�G�X�g�G���[",						// [ 8] POKE_NET_GDS_LASTERROR_SENDREQUEST
	"RECVRESPONSE:���X�|���X�G���[",					// [ 9] POKE_NET_GDS_LASTERROR_RECVRESPONSE
	"CONNECTTIMEOUT:�ڑ��^�C���A�E�g",					// [10] POKE_NET_GDS_LASTERROR_CONNECTTIMEOUT
	"SENDTIMEOUT:���M�^�C���A�E�g",						// [11] POKE_NET_GDS_LASTERROR_SENDTIMEOUT
	"RECVTIMEOUT:��M�^�C���A�E�g",						// [12] POKE_NET_GDS_LASTERROR_RECVTIMEOUT
	"ABORTED:���f�I��"									// [13] POKE_NET_GDS_LASTERROR_ABORTED
};

// *
// * ���s����^�X�N ��
// *
enum EnumAction
{
	ACTION_HELP							= -2,	// �w���v
	ACTION_EXIT							= -1,	// �I��

	ACTION_DEBUG_MSG					= 0,	// �ڑ��e�X�g�p���b�Z�[�W���M
	ACTION_MUSICAL_UPLOAD				= 1,	// �~���[�W�J���V���b�g�A�b�v���[�h
	ACTION_MUSICAL_DOWNLOAD				= 2,	// �~���[�W�J���V���b�g�_�E�����[�h
	ACTION_BATTLEVIDEO_UPLOAD			= 3,	// �o�g���r�f�I�A�b�v���[�h
	ACTION_BATTLEVIDEO_SEARCH			= 4,	// �o�g���r�f�I����
	ACTION_BATTLEVIDEO_RANKING			= 5,	// �����L���O����
	ACTION_BATTLEVIDEO_DOWNLOAD			= 6,	// �o�g���r�f�I�_�E�����[�h
	ACTION_BATTLEVIDEO_DOWNLOAD_EVENT	= 7,	// �C�x���g�o�g���r�f�I�_�E�����[�h
	ACTION_BATTLEVIDEO_FAVORITE			= 8,	// �o�g���r�f�I���C�ɓ���o�^

	ACTION_END_DUMMY
};


//====================================
// �v���g�^�C�v�錾
//====================================

// �v���t�B�[���쐬
void MakeProfileData(GDS_PROFILE* _pProfile);

// PMS�f�[�^�쐬
void MakePMSData(PMS_DATA* _pPMS);

// �~���[�W�J���V���b�g�쐬
void MakeMusicalshotData(MUSICAL_SHOT_DATA *_pMusicalData, int _MonsNoInc = 1);

// �o�g���r�f�I�f�[�^�쐬
void MakeBattleVideoHeader(BATTLE_REC_HEADER *_bhead, BOOL bSubway, int _MonsNoInc = 1);	// �w�b�_
void MakeBattleVideoData(BATTLE_REC_WORK *_brec, int _MonsNoInc = 1);						// �{��

// �I�[����(0xFFFF)�������܂��B
// �o�b�t�@�I�[�o�[�t���[�ɒ��ӂ��Ă��������B
// (�K���o�b�t�@�̍Ō�̗v�f�ɂ�0������悤�ɂ��Ă��������B)
void SetGDS_EOM(STRCODE* _pStr);


//====================================
// ���C���֐�
//====================================
void main(int argc ,char *argv[])
{
	const int BufferSize = 1024 * 64;

	// ���C�u�����ɐݒ肷��o�b�t�@
	BYTE ReqBuff[BufferSize];
	BYTE ResBuff[BufferSize];

	// ��������͗p
	char tmpStr[1024] = {0};

	// ���ړI�Ɏg�p�����B
	s64 tmpNo = 0;

	// �o�̓��b�Z�[�W
	const char* MessageList[] = {
		"-inactive\n",		// [0] POKE_NET_GDS_STATUS_INACTIVE		// ��ғ���
		"-initialized\n",	// [1] POKE_NET_GDS_STATUS_INITIALIZED	// �������ς�
		"-request\n",		// [2] POKE_NET_GDS_STATUS_REQUEST		// ���N�G�X�g���s
		"-netsetting\n",	// [3] POKE_NET_GDS_STATUS_NETSETTING	// �l�b�g���[�N�ݒ蒆
		"-connecting\n",	// [4] POKE_NET_GDS_STATUS_CONNECTING	// �ڑ���
		"-sending\n",		// [5] POKE_NET_GDS_STATUS_SENDING		// ���M��
		"-receiving\n",		// [6] POKE_NET_GDS_STATUS_RECEIVING	// ��M��
		"-aborted\n",		// [7] POKE_NET_GDS_STATUS_ABORTED		// ���f
		"-finished\n",		// [8] POKE_NET_GDS_STATUS_FINISHED		// ����I��
		"-error"			// [9] POKE_NET_GDS_STATUS_ERROR		// �G���[�I��
	};

	// ====================================
	// Winsock������
	// ====================================
	WSADATA basedata;
	WSAStartup(MAKEWORD(1, 1) ,&basedata);

	// ====================================
	// �F�؍\����
	// ====================================
	POKE_NET_REQUESTCOMMON_AUTH ReqAuth = {0};
	ReqAuth.PID			= AUTH_PID;
	ReqAuth.ROMCode		= AUTH_ROMCODE;
	ReqAuth.LangCode	= AUTH_LANGCODE;

	// ====================================
	// �v���t�B�[���쐬
	// ====================================
	GDS_PROFILE Profile = {0};
	MakeProfileData(&Profile);

	// ====================================
	// �m�F�\��
	// ====================================
	printf("To => %s:%d\n", SYACHI_SERVER_URL, SYACHI_SERVER_PORT);
	
	// ====================================
	// ���C�����[�v
	// ====================================
	long InputReqNo;
	BOOL MainLoopFlag = TRUE;
	while(MainLoopFlag)
	{
		// ���N�G�X�g�̑I��
		printf("?Input send request no(-1:exit -2:help) >");

		// ���͂����߂�
		scanf("%d" ,&InputReqNo);

		// ***************************************************
		// *                                                 *
		// ** exit                                          **
		// *                                                 *
		// ***************************************************
		if(InputReqNo == ACTION_EXIT)
		{
			break;
		}

		// ***************************************************
		// *                                                 *
		// ** help�\��                                      **
		// *                                                 *
		// ***************************************************
		else if(InputReqNo == ACTION_HELP)
		{
			printf("%3d:�ڑ��e�X�g�p���N�G�X�g\n",				(int)ACTION_DEBUG_MSG					);

			printf("%3d:�~���[�W�J���V���b�g�A�b�v���[�h\n",	(int)ACTION_MUSICAL_UPLOAD				);
			printf("%3d:�~���[�W�J���V���b�g�_�E�����[�h\n",	(int)ACTION_MUSICAL_DOWNLOAD			);

			printf("%3d:�o�g���r�f�I�A�b�v���[�h\n",			(int)ACTION_BATTLEVIDEO_UPLOAD			);
			printf("%3d:�o�g���r�f�I���킵������\n",			(int)ACTION_BATTLEVIDEO_SEARCH			);
			printf("%3d:�o�g���r�f�I�����L���O����\n",			(int)ACTION_BATTLEVIDEO_RANKING			);
			printf("%3d:�o�g���r�f�I�_�E�����[�h\n",			(int)ACTION_BATTLEVIDEO_DOWNLOAD		);
			printf("%3d:�C�x���g�o�g���r�f�I�_�E�����[�h\n",	(int)ACTION_BATTLEVIDEO_DOWNLOAD_EVENT	);
			printf("%3d:�o�g���r�f�I���C�ɓ���o�^\n",			(int)ACTION_BATTLEVIDEO_FAVORITE		);

			printf("\n");

			continue;
		}

		// ====================================
		// ���C�u����������
		// ====================================
		if(!POKE_NET_GDS_Initialize(&ReqAuth, SYACHI_SERVER_URL, SYACHI_SERVER_PORT))
		{
			// ���C�u�����������Ɏ��s
			printf("POKE_NET_GDS_Initialize() Failed!\n");
			continue;
		}

		// ====================================
		// ���N�G�X�g�o�^
		// ====================================
		FillMemory(tmpStr,  sizeof(tmpStr), 0);
		FillMemory(ReqBuff, sizeof(ReqBuff), 0);
		FillMemory(ResBuff, sizeof(ResBuff), 0);


		// ***************************************************
		// *                                                 *
		// ** �ڑ��e�X�g�p���N�G�X�g                        **
		// **                                               **
		// ** ���M�������b�Z�[�W�����̂܂ܕԐM����܂��B    **
		// *                                                 *
		// ***************************************************
		if(InputReqNo == ACTION_DEBUG_MSG)
		{
			printf("[%d]DEBUG_MSG >> Input Message >", ACTION_DEBUG_MSG);
			rewind(stdin);

			fgets(tmpStr, sizeof(tmpStr), stdin);
			if(tmpStr[0] == NULL)
			{
				Sleep(10);
				continue;
			}

			// �����̉��s�O��
			int Length = strlen(tmpStr);
			if(tmpStr[Length - 1] == '\n')
			{
				tmpStr[Length - 1] = '\0';
			}

			// ���M
			POKE_NET_GDS_DebugMessageRequest(tmpStr, ResBuff);
		}

		// ***************************************************
		// *                                                 *
		// ** �~���[�W�J���V���b�g�A�b�v���[�h              **
		// *                                                 *
		// ***************************************************
		else if(InputReqNo == ACTION_MUSICAL_UPLOAD)
		{
			memset(ReqBuff ,0x00 ,sizeof(ReqBuff));
			memset(ResBuff ,0x00 ,sizeof(ResBuff));

			printf("[%d]MUSICAL_UPLOAD >> �쐬����|�PNo�����(-1:cancel) >", ACTION_MUSICAL_UPLOAD);

			// ���͂��ꂽ�ԍ�=tmpNo�Ƃ���
			// (tmpNo+0),(tmpNo+1),(tmpNo+2),(tmpNo+3)���|�P�����ԍ��ƂȂ�܂��B
			scanf("%I64d" ,&tmpNo);
			if(tmpNo == -1)
			{
				Sleep(10);
				continue;
			}

			// �v���t�B�[�����R�s�[
			memcpy(&((MUSICAL_SHOT_SEND*)ReqBuff)->profile, &Profile, sizeof(GDS_PROFILE));

			// �~���[�W�J���V���b�g�f�[�^���쐬
			MakeMusicalshotData(&((MUSICAL_SHOT_SEND*)ReqBuff)->mus_shot, (int)tmpNo);

			// �ʐM
			POKE_NET_GDS_MusicalShotRegist((MUSICAL_SHOT_SEND*)ReqBuff ,ResBuff);
		}

		// ***************************************************
		// *                                                 *
		// ** �~���[�W�J���V���b�g�_�E�����[�h              **
		// *                                                 *
		// ***************************************************
		else if(InputReqNo == ACTION_MUSICAL_DOWNLOAD)
		{
			memset(ReqBuff ,0x00 ,sizeof(ReqBuff));
			memset(ResBuff ,0x00 ,sizeof(ResBuff));

			printf("[%d]MUSICAL_DOWNLOAD >> ��������|�PNo�����(-1:cancel) >", ACTION_MUSICAL_DOWNLOAD);

			scanf("%I64d" ,&tmpNo);
			if(tmpNo == -1)
			{
				Sleep(10);
				continue;
			}

			POKE_NET_GDS_MusicalShotGet((int)tmpNo ,ResBuff);
		}

		// ***************************************************
		// *                                                 *
		// ** �o�g���f�[�^�A�b�v���[�h                      **
		// *                                                 *
		// ***************************************************
		else if(InputReqNo == ACTION_BATTLEVIDEO_UPLOAD)
		{
			int tmpNo2 = 0;

			memset(ReqBuff ,0x00 ,sizeof(ReqBuff));
			memset(ResBuff ,0x00 ,sizeof(ResBuff));

			printf("[%d]BATTLEVIDEO_UPLOAD >> �쐬����|�PNo�����(-1:cancel) >", ACTION_BATTLEVIDEO_UPLOAD);

			scanf("%I64d" ,&tmpNo);
			if(tmpNo == -1)
			{
				Sleep(10);
				continue;
			}

			// �v���t�B�[�����R�s�[
			memcpy(&((BATTLE_REC_SEND*)ReqBuff)->profile, &Profile, sizeof(GDS_PROFILE));

			printf("[%d]BATTLEVIDEO_UPLOAD >> �����L���O��ʂ����(0:�o�g���T�u�E�F�C, Other:�ʐM�ΐ�) >", ACTION_BATTLEVIDEO_UPLOAD);
			scanf("%d" ,&tmpNo2);
			
			// �o�g���r�f�I�w�b�_���쐬
			MakeBattleVideoHeader(&((BATTLE_REC_SEND*)ReqBuff)->head, (tmpNo2 == 0), (int)tmpNo);

			// �o�g���r�f�I�{�̂��쐬
			MakeBattleVideoData(&((BATTLE_REC_SEND*)ReqBuff)->rec);

			// �ʐM
			POKE_NET_GDS_BattleDataRegist((BATTLE_REC_SEND*)ReqBuff ,ResBuff);
		}

		// ***************************************************
		// *                                                 *
		// ** �o�g���f�[�^����                              **
		// **                                               **
		// ** SEARCHTYPE_CONDITION�ɂ��ĥ��               **
		// ** �����Ώۃe�[�u���ɃC�x���g�ƃ����L���O�̈��  **
		// ** �܂܂�܂���B                                **
		// *                                                 *
		// ***************************************************
		else if(InputReqNo == ACTION_BATTLEVIDEO_SEARCH)
		{
			int tmp = 0;

			memset(ReqBuff ,0x00 ,sizeof(ReqBuff));
			memset(ResBuff ,0x00 ,sizeof(ResBuff));

			// ���������p�̍\����
			BATTLE_REC_SEARCH_SEND Serch = {0};

			// ��������|�P�����ԍ�����͂�����
			printf("[%d]BATTLEVIDEO_SEARCH >> ��������|�PNo�����(0:No target.) >", ACTION_BATTLEVIDEO_SEARCH);

			scanf("%d" ,&(Serch.monsno));
			if(Serch.monsno == 0)
			{
				Serch.monsno = BATTLE_REC_SEARCH_MONSNO_NONE;
			}

			// ���^�n���R�[�h
			Serch.country_code = BATTLE_REC_SEARCH_COUNTRY_CODE_NONE;
			Serch.local_code = BATTLE_REC_SEARCH_LOCAL_CODE_NONE;

			// �o�g���r�f�I�̃o�[�W����
			Serch.server_version = BATTLEVIDEO_SERVER_VER;

			// �퓬���[�h����
			printf("[%d]BATTLEVIDEO_SEARCH >> �������郉���L���O��ʂ����(0:No target, 1:�o�g���T�u�E�F�C, Other:�ʐM�ΐ�) >", ACTION_BATTLEVIDEO_SEARCH);
			scanf("%d" ,&tmp);
			if(tmp == 0)
			{
				Serch.battle_mode = 0;
				Serch.battle_mode_bit_mask = 0;
			}
			else if(tmp == 1)
			{
				// �o�g���T�u�E�F�C���w��
				Serch.battle_mode = BATTLEMODE_COMMUNICATION_SUBWAY << BATTLEMODE_COMMUNICATION_BIT;
				Serch.battle_mode_bit_mask = BATTLEMODE_COMMUNICATION_MASK;
			}
			else
			{
				// �ʐM�ΐ���w��
				Serch.battle_mode = BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT;
				Serch.battle_mode_bit_mask = BATTLEMODE_COMMUNICATION_MASK;
			}

			// �ʐM
			POKE_NET_GDS_BattleDataSearchCondition(&Serch, ResBuff);
		}

		// ***************************************************
		// *                                                 *
		// ** �o�g���f�[�^�����L���O����                    **
		// **                                               **
		// ** �S�āA�����ňȉ��̃��N�G�X�g�𗘗p���܂��B    **
		// ** POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH        **
		// *                                                 *
		// ***************************************************
		else if(InputReqNo == ACTION_BATTLEVIDEO_RANKING)
		{
			memset(ReqBuff ,0x00 ,sizeof(ReqBuff));
			memset(ResBuff ,0x00 ,sizeof(ResBuff));

			// ���������p�̍\����
			BATTLE_REC_RANKING_SEARCH_SEND Serch = {0};

			// �Ώۃ����L���O��I��
			printf("[%d]BATTLEVIDEO_RANKING >> �������郉���L���O�����(0:Latest30, 1:Subway, Other:Comm) >", ACTION_BATTLEVIDEO_RANKING);

			scanf("%I64d" ,&tmpNo);
			if(tmpNo == 0)
			{
				// �ŐV30��
				POKE_NET_GDS_BattleDataSearchRankingLatest(BATTLEVIDEO_SERVER_VER, ResBuff);
			}
			else if(tmpNo == 1)
			{
				// �o�g���T�u�E�F�C�����L���O
				POKE_NET_GDS_BattleDataSearchRankingSubway(BATTLEVIDEO_SERVER_VER, ResBuff);
			}
			else
			{
				// �ʐM�ΐ탉���L���O
				POKE_NET_GDS_BattleDataSearchRankingComm(BATTLEVIDEO_SERVER_VER, ResBuff);
			}
		}

		// ***************************************************
		// *                                                 *
		// ** �o�g���f�[�^�_�E�����[�h                      **
		// **                                               **
		// ** ����ACTION_BATTLEVIDEO_DOWNLOAD_EVENT�Ƃ�     **
		// ** �Ⴂ�́AID�{900000000000ULL�̗L�������ł��B   **
		// *                                                 *
		// ***************************************************
		else if(InputReqNo == ACTION_BATTLEVIDEO_DOWNLOAD)
		{
			memset(ReqBuff ,0x00 ,sizeof(ReqBuff));
			memset(ResBuff ,0x00 ,sizeof(ResBuff));

			printf("[%d]BATTLEVIDEO_DOWNLOAD >> Input BattleVideoID(-1:cancel) >", ACTION_BATTLEVIDEO_DOWNLOAD);

			scanf("%I64d" ,&tmpNo);
			if(tmpNo == -1)
			{
				Sleep(10);
				continue;
			}

#ifdef BATTLEID_AUTO_ENCODE
			// �Í���
			tmpNo = POKE_NET_EncodePlayDataID(tmpNo);
#endif

			// �ʐM
			POKE_NET_GDS_BattleDataGet((u64)tmpNo, BATTLEVIDEO_SERVER_VER, ResBuff);
		}

		// ***************************************************
		// *                                                 *
		// ** �C�x���g�o�g���f�[�^�_�E�����[�h              **
		// **                                               **
		// ** ���ACTION_BATTLEVIDEO_DOWNLOAD�Ƃ̈Ⴂ�́A   **
		// ** ID�{900000000000ULL�̗L�������ł��B           **
		// *                                                 *
		// ***************************************************
		else if(InputReqNo == ACTION_BATTLEVIDEO_DOWNLOAD_EVENT)
		{
			memset(ReqBuff ,0x00 ,sizeof(ReqBuff));
			memset(ResBuff ,0x00 ,sizeof(ResBuff));

			printf("[%d]MUSICAL_DOWNLOAD >> Input EventBattleVideoID(-1:cancel) >", ACTION_BATTLEVIDEO_DOWNLOAD_EVENT);
			scanf("%I64d" ,&tmpNo);

			if(tmpNo == -1)
			{
				Sleep(10);
				continue;
			}

			// �C�x���g�r�f�I�p��ID�ϊ�
			tmpNo = TO_EVENT_BATTLEVIDEO_ID(tmpNo);

#ifdef BATTLEID_AUTO_ENCODE
			// �Í���
			tmpNo = POKE_NET_EncodePlayDataID(tmpNo);
#endif

			// �ʐM
			POKE_NET_GDS_BattleDataGet((u64)tmpNo, BATTLEVIDEO_SERVER_VER, ResBuff);
		}

		// ***************************************************
		// *                                                 *
		// ** �o�g���f�[�^���C�ɓ���o�^                    **
		// *                                                 *
		// ***************************************************
		else if(InputReqNo == ACTION_BATTLEVIDEO_FAVORITE)
		{
			memset(ReqBuff ,0x00 ,sizeof(ReqBuff));
			memset(ResBuff ,0x00 ,sizeof(ResBuff));

			printf("[%d]MUSICAL_DOWNLOAD >> Input BattleVideoID(-1:cancel) >", ACTION_BATTLEVIDEO_FAVORITE);

			scanf("%I64d" ,&tmpNo);
			if(tmpNo == -1)
			{
				Sleep(10);
				continue;
			}

#ifdef BATTLEID_AUTO_ENCODE
			// �Í���
			tmpNo = POKE_NET_EncodePlayDataID(tmpNo);
#endif

			// �ʐM
			POKE_NET_GDS_BattleDataFavorite((u64)tmpNo, ResBuff);
		}

		// ***************************************************
		// *                                                 *
		// ** ��L�ȊO�̒l                                  **
		// *                                                 *
		// ***************************************************
		else
		{
			// ====================================
			// ������ăX�^�[�g�ɖ߂�
			// ====================================
			POKE_NET_GDS_Release();

			printf("�s���Ȓl�ł��B\n");
			Sleep(10);
			continue;
		}

		// ====================================
		// �X�e�[�^�X�擾�^�\�����[�v
		// ====================================
		int LastState = POKE_NET_GDS_GetStatus();
		int State = 0;
		BOOL LoopFlag = TRUE;
		while(LoopFlag)
		{
			State = POKE_NET_GDS_GetStatus();
			if(LastState == State)
			{
				// �X�e�[�^�X�ɕω��͖���
				Sleep(10);
				continue;
			}

			// 
			// �� �ȍ~�̓X�e�[�^�X�ɕω����������ꍇ ��
			//

			// ���݂̃X�e�[�^�X�\��
			printf(MessageList[State]);

			if(State == POKE_NET_GDS_STATUS_INACTIVE
				|| State == POKE_NET_GDS_STATUS_INITIALIZED
				|| State == POKE_NET_GDS_STATUS_ABORTED
				|| State == POKE_NET_GDS_STATUS_FINISHED
				|| State == POKE_NET_GDS_STATUS_ERROR
				)
			{
				LoopFlag = FALSE;
			}

			// ***************************************************
			// *                                                 *
			// ** �ʐM�����������B                              **
			// *                                                 *
			// ***************************************************
			if(State == POKE_NET_GDS_STATUS_FINISHED)
			{
				// ���X�|���X�\��(DEBUG)
				POKE_NET_GDS_DEBUG_PrintResponse((POKE_NET_RESPONSE*)POKE_NET_GDS_GetResponse(), POKE_NET_GDS_GetResponseSize());
			}

			// ***************************************************
			// *                                                 *
			// ** �ʐM���ɃG���[���N�����I                      **
			// *                                                 *
			// ***************************************************
			else if(State == POKE_NET_GDS_STATUS_ERROR)
			{
				// �G���[���\��
				printf("lasterrorcode:%d\n", POKE_NET_GDS_GetLastErrorCode());
				printf("LastErrorMsg:%s\n", g_ErrorMessageList[POKE_NET_GDS_GetLastErrorCode()]);
			}

			LastState = State;
			Sleep(10);
		}

		// ====================================
		// ���
		// ====================================
		POKE_NET_GDS_Release();
	}

	// ====================================
	// Winsock�I��
	// ====================================
	WSACleanup();
}


// **
// ** �v���t�B�[���쐬
// **                 
void MakeProfileData(GDS_PROFILE* _pProfile)
{
	FillMemory(_pProfile, sizeof(GDS_PROFILE), 0);

	// 7�����܂�(8�����ڂ�NULL)
	wcscpy((wchar_t*)_pProfile->name, g_TrainerName);	// �v���C���[����		[�u��] SOAP�𗘗p����
	//_pProfile->name[wcslen(g_TrainerName)] = 0xFFFF;
	SetGDS_EOM(_pProfile->name);

	_pProfile->player_id = 0;				// �v���C���[ID			[ �| ] 0 - 0xffffffff
	_pProfile->player_sex = 0;				// �v���C���[����		[����] 0 - 1
	_pProfile->birthday_month = 1;			// �a����				[����] 1 - 12
	_pProfile->trainer_view = 0;			// �v���C���[�̌�����	[����] 0 - 15

	_pProfile->country_code = 0;			// �Z��ł��鍑�R�[�h	[����] 0 - 233
	_pProfile->local_code = 0;				// �Z��ł���n���R�[�h	[����] * - *

	_pProfile->version_code = AUTH_ROMCODE;	// �o�[�W�����R�[�h		[����] 20, 21	(WHITE=20, BLACK=21)
	_pProfile->language = AUTH_LANGCODE;	// ����R�[�h			[����] 1 - 8

	_pProfile->egg_flag = 0;				// �^�}�S�t���O			[����] 0 - 1
	_pProfile->form_no = 0;					// �t�H����No			[����] * - *
	_pProfile->mons_sex = 0;				// �����X�^�[����		[����] 0 - 1
	_pProfile->monsno = 1;					// �����X�^�[No			[����] 1 - ��

	// MESSAGE_FLAG_NORMAL(�ȈՉ�b) / MESSAGE_FLAG_EVENT(�t���[���[�h)
	_pProfile->message_flag = MESSAGE_FLAG_NORMAL;	//				[����] 0 - 0

	// �ȈՉ�b�ݒ�
	MakePMSData(&_pProfile->self_introduction);

	// MATH_CalcCRC16CCITT											[����] 
	MATHCRC16Table crctable;
	MATH_CRC16CCITTInitTable(&crctable);
	_pProfile->crc.crc16ccitt_hash = MATH_CalcCRC16CCITT(&crctable, _pProfile, sizeof(GDS_PROFILE) - GDS_CRC_SIZE);
}


// **                 
// ** PMS�f�[�^�쐬
// **                 
void MakePMSData(PMS_DATA* _pPMS)
{
	_pPMS->sentence_type = 0;	// ���̓^�C�v				[����] 0 - 4
	_pPMS->sentence_id = 0;		// �^�C�v��ID				[����] 0 - 19
	_pPMS->word[0] = 0;			// �P��ID					[����] 0 - 1735
	_pPMS->word[1] = 0;
}


// **                 
// ** �~���[�W�J���V���b�g�쐬
// **                 
void MakeMusicalshotData(MUSICAL_SHOT_DATA *_pMusicalData, int _MonsNoInc)
{
	FillMemory(_pMusicalData, sizeof(MUSICAL_SHOT_DATA), 0);

	//�~���[�W�J���^�C�g��(���{18�E�C�O36�{EOM							[����] ��ڂ͌Œ�
	wcscpy((wchar_t*)_pMusicalData->title, g_MusicalTitle);
	//_pMusicalData->title[wcslen(g_MusicalTitle)] = 0xFFFF;
	SetGDS_EOM(_pMusicalData->title);
	
	_pMusicalData->bgNo		= 0;			// �w�i�ԍ�					[����] 0 - 4
	_pMusicalData->spotBit	= 0;			// �X�|�b�g���C�g�Ώ�(bit)(�g�b�v�������|�P����)
											//							[����] 0 - 15
	_pMusicalData->year		= 0;			// �N						[����] 0 - 99
	_pMusicalData->month	= 1;			// ��						[����] 1 - 12
	_pMusicalData->day		= 1;			// ��						[����] 1 - 31
	_pMusicalData->player	= 0;			// �����̔ԍ�				[����] 0 - 3
	_pMusicalData->musVer	= 0;			// �~���[�W�J���o�[�W����	[����] 0 - 0
	_pMusicalData->pmVersion= AUTH_ROMCODE;	// PM_VERSION				[����] 20, 21	(WHITE=20, BLACK=21)
	_pMusicalData->pmLang	= AUTH_LANGCODE;// PM_LANG					[����] 1 - 8

	for(int i = 0;i < MUSICAL_POKE_MAX; i++)
	{
		_pMusicalData->shotPoke[i].monsno	= _MonsNoInc + i;	// �|�P�����ԍ�	[ �| ] �ʃT�[�o�ɂă`�F�b�N
		_pMusicalData->shotPoke[i].sex		= 0;				// ����			[ �| ]
		_pMusicalData->shotPoke[i].rare		= 0;				// ���A�t���O	[ �| ]
		_pMusicalData->shotPoke[i].form		= 0;				// �t�H�����ԍ�	[ �| ]

		// �g���[�i�[��															[ �| ]
		// 7�����܂�(8�����ڂ�NULL)
		wcscpy((wchar_t*)_pMusicalData->shotPoke[i].trainerName, g_TrainerName);
		//_pMusicalData->shotPoke[i].trainerName[wcslen(g_TrainerName)] = 0xFFFF;
		SetGDS_EOM(_pMusicalData->shotPoke[i].trainerName);

		// �����O�b�Y
		// �ő�W�܂ő����\(�����ł͂P�����ݒ肵�Ȃ�)
		_pMusicalData->shotPoke[i].equip[0].itemNo	= 0;		// �O�b�Y�ԍ�	[����] 0 - 99, 65535	(65535�̓f�[�^�����̏ꍇ)
		_pMusicalData->shotPoke[i].equip[0].angle	= i * 10;	// �p�x			[����] -32767 - 32768
		_pMusicalData->shotPoke[i].equip[0].equipPos= 0;		// �����ӏ�		[����] 0 - 8, 10		(10�̓f�[�^�����̏ꍇ)
	}
}


// **                 
// ** �o�g���r�f�I�f�[�^�쐬
// ** (�w�b�_)
// **                 
void MakeBattleVideoHeader(BATTLE_REC_HEADER* _pHeader, BOOL bSubway, int _MonsNoInc)
{
	// ������
	FillMemory(_pHeader, sizeof(BATTLE_REC_HEADER), 0);

	for(int i = 0; i < HEADER_MONSNO_MAX; i++)
	{
		_pHeader->monsno[i] = _MonsNoInc + i;	// �����X�^�[No			[����] 1 - ��
		_pHeader->form_no_and_sex[i] = 0;		// �|�P�����t�H�����ԍ�	[����]
	}
	
	_pHeader->battle_counter = 0;				// �A����				[����] 0 - 9999

	// �퓬���[�h														[����] �\����ނ̌Œ�l
	if(bSubway)
	{
		// �����L���O��ʂ̓o�g���T�u�E�F�C
		_pHeader->mode = BATTLEMODE_COMMUNICATION_SUBWAY << BATTLEMODE_COMMUNICATION_BIT;
		//_pHeader->mode = 0x1234;
	}
	else
	{
		// �����L���O��ʂ͒ʐM�ΐ�
		_pHeader->mode = BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT;
		//_pHeader->mode = 0x1234;
	}
	
	_pHeader->secure		= 0;		// (TRUE:���S�ۏ�, FALSE:���Đ�)[����] 0 - 0
	_pHeader->magic_key		= REC_OCC_MAGIC_KEY;	// �}�W�b�N�L�[		[����] 0xe281 - 0xe281
	_pHeader->server_vesion	= 100;		// �T�[�o�o�[�W����				[����] 100 - 100
	_pHeader->data_number	= 0;		// �T�[�o���ŃZ�b�g�����		[ �| ] 
	//_pHeader->work[]		= 0;		// �\��							[ �| ] 

	// MATH_CalcCRC16CCITT												[����] 
	MATHCRC16Table crctable;
	MATH_CRC16CCITTInitTable(&crctable);
	_pHeader->crc.crc16ccitt_hash = MATH_CalcCRC16CCITT(&crctable, _pHeader, sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE);
}


// **                 
// ** �o�g���r�f�I�f�[�^�쐬
// ** (�{��)
// **                 
void MakeBattleVideoData(BATTLE_REC_WORK* _pData, int _MonsNoInc)
{
	// ������
	FillMemory(_pData, sizeof(BATTLE_REC_WORK), 0);

	// �o�g����ʃZ�b�g�A�b�v�p�����[�^�̘^��f�[�^�p�T�u�Z�b�g

	// �����̎�
	// BTLREC_SETUP_SUBSET �\����
	_pData->setupSubset.randomContext.x		= 0;	// [����]
	_pData->setupSubset.randomContext.mul	= 0;	// [����]
	_pData->setupSubset.randomContext.add	= 0;	// [����]

	// �o�g���V�`���G�[�V����
	// BTLREC_SETUP_SUBSET :: BTL_FIELD_SITUATION �\����
	_pData->setupSubset.fieldSituation.bgType	= BATTLE_BG_TYPE_GRASS;			// �퓬�w�i						[����] 0 - 10
	_pData->setupSubset.fieldSituation.bgAttr	= BATTLE_BG_ATTR_NORMAL_GROUND;	// �퓬�w�i�w��A�g���r���[�g	[����] 0 - 12
	_pData->setupSubset.fieldSituation.weather	= BTL_WEATHER_NONE;				// �V��							[����] 0 - 4
	_pData->setupSubset.fieldSituation.season	= 0;							// �l�G							[����] 0 - 3
	_pData->setupSubset.fieldSituation.zoneID	= (ZONEID)0;					// �R�����g����					[����] - - -
	_pData->setupSubset.fieldSituation.hour		= 23;							// �R�����g����					[����] 0 - 23
	_pData->setupSubset.fieldSituation.minute	= 59;							// �R�����g����					[����] 0 - 59

	// �ݒ�f�[�^
	// BTLREC_SETUP_SUBSET :: CONFIG �\����
	_pData->setupSubset.config.msg_speed		= 0;// MSG����̑��x						[����] 0 - 2
	_pData->setupSubset.config.sound_mode		= 0;// �T�E���h�o��							[����] 0 - 1
	_pData->setupSubset.config.battle_rule		= 0;// �퓬���[��							[����] 0 - 1
	_pData->setupSubset.config.wazaeff_mode		= 0;// �킴�G�t�F�N�g						[����] 0 - 1
	_pData->setupSubset.config.moji_mode		= 0;// �Ђ炪�ȁ^����						[����] 0 - 1
	_pData->setupSubset.config.wirelesssave_mode= 0;// ���C�����X�ŃZ�[�u���͂��ނ��ǂ���	[����] 0 - 1
	_pData->setupSubset.config.network_search	= 0;// �Q�[�����Ƀr�[�R���T�[�`���邩�ǂ���	[����] 0 - 1

	_pData->setupSubset.musicDefault= 0;	// �f�t�H���gBGM								[����] 0 - 65535
	_pData->setupSubset.musicPinch	= 0;	// �s���`��BGM									[����] 0 - 65535
	_pData->setupSubset.debugFlagBit= 0;	// �f�o�b�O�@�\Bit�t���O -> enum BtlDebugFlag @ battle/battle.h								[����] 0 - 0
	_pData->setupSubset.competitor	= 2;	// �ΐ�҃^�C�v�i�Q�[�����g���[�i�[�A�ʐM�ΐ�j-> enum BtlCompetitor @ battle/battle.h		[����] 2 - 3
	_pData->setupSubset.myCommPos	= 0;	// �ʐM�ΐ펞�̎����̗����ʒu�i�}���`�̎��A0,2 vs 1,3 �ɂȂ�A0,1�������^2,3���E���ɂȂ�j	[����] 0 - 3
	_pData->setupSubset.rule		= 0;	// ���[���i�V���O���E�_�u���E�g���v���E���[�e�j-> enum BtlRule @ battle/battle.h			[����] 0 - 3
	_pData->setupSubset.fMultiMode	= 0;	// �}���`�o�g���t���O�i���[���͕K���_�u���j		[����] 0 - 1

	// �N���C�A���g������e�̕ۑ��o�b�t�@
	// BTLREC_OPERATION_BUFFER �\����
	_pData->opBuffer.size = 0;		// �T�C�Y�FBTLREC_OPERATION_BUFFER_SIZE					[����] 0 - 0xc00
	memset(_pData->opBuffer.buffer, 0, BTLREC_OPERATION_BUFFER_SIZE);		// �o�b�t�@		[����] 

	// �o�g���Q���v���C���[���̏��
	for(int i = 0; i < BTL_CLIENT_MAX; i++)
	{
		// �|�P�����p�[�e�B�f�[�^
		// REC_POKEPARTY �\����
		_pData->rec_party[i].PokeCountMax = 6;	// �ێ��o����|�P�������̍ő�				[����] 6 - 6
		_pData->rec_party[i].PokeCount = 1;		// ���ݕێ����Ă���|�P������				[����] 1 - 6
		
		// �|�P�����f�[�^
		// REC_POKEPARTY :: REC_POKEPARA �\����												[ �| ] �ʃT�[�o�ɂă`�F�b�N
		for(int j = 0; j < TEMOTI_POKEMAX; j++)
		{
			// �|�P�����̃X�e�[�^�X���
			_pData->rec_party[i].member[j].monsno = _MonsNoInc + j;
		}

		// �v���C���[���
		// BTLREC_CLIENT_STATUS �\����
		_pData->clientStatus[i].type = BTLREC_CLIENTSTATUS_PLAYER;	// �N���C�A���g�^�C�v	[����] 0 - 2 

		if(_pData->clientStatus[i].type == BTLREC_CLIENTSTATUS_PLAYER)
		{
			// MYSTATUS �\����
			wcscpy((wchar_t*)_pData->clientStatus[i].player.name, g_TrainerName);	// �v���C���[��	[�u��] SOAP�𗘗p����
			//_pData->clientStatus[i].player.name[wcslen(g_TrainerName)] = 0xFFFF;
			SetGDS_EOM(_pData->clientStatus[i].player.name);

			_pData->clientStatus[i].player.id = 0;				// �g���[�i�[ID				[����] 
			_pData->clientStatus[i].player.profileID = 0;		// �v���t�@�C��ID			[����] 
			_pData->clientStatus[i].player.nation = 0;			// �Z��ł��鍑�R�[�h		[����] 
			_pData->clientStatus[i].player.area = 0;			// �Z��ł���n���R�[�h		[����] 
			_pData->clientStatus[i].player.sex = 0;				// ����						[����] 0 - 1
			_pData->clientStatus[i].player.region_code = 1;		// ����R�[�h				[����] 1 - 8
			_pData->clientStatus[i].player.trainer_view = 0;	// ������					[����] 0 - 15
			_pData->clientStatus[i].player.rom_code = 20;		// �o�[�W�����R�[�h			[����] 20, 21	(WHITE=20, BLACK=21)
		}
		else if(_pData->clientStatus[i].type == BTLREC_CLIENTSTATUS_TRAINER)
		{
			// BTLREC_TRAINER_STATUS �\����
			_pData->clientStatus[i].trainer.ai_bit = 0;					//					[����] 0 - 0x3fff	(�����̉\������)
			_pData->clientStatus[i].trainer.tr_id = 0;					//					[����] 0 - 304		(�����̉\������)
			_pData->clientStatus[i].trainer.tr_type = 0;				//					[ �| ] 0 - 82		(�����̉\������)
			memset(
				&_pData->clientStatus[i].trainer.use_item,
				0,
				BSP_TRAINERDATA_ITEM_MAX * sizeof(u16)
			);															// �g�p����A�C�e�� [����] 0 - 0
			wcscpy(
				(wchar_t*)_pData->clientStatus[i].trainer.name,
				g_TrainerNmae2
			);															// �g���[�i�[��		[�u��] SOAP�𗘗p����
			//_pData->clientStatus[i].trainer.name[wcslen(g_TrainerNmae2)] = 0xFFFF;
			SetGDS_EOM(_pData->clientStatus[i].trainer.name);

			MakePMSData(&_pData->clientStatus[i].trainer.win_word);		// �퓬�I�����������b�Z�[�W <-8byte
			MakePMSData(&_pData->clientStatus[i].trainer.lose_word);	// �퓬�I�����������b�Z�[�W <-8byte
		}
	}

	_pData->magic_key			= REC_OCC_MAGIC_KEY;	// �}�W�b�N�L�[						[����] 0xe281 - 0xe281
	//_pData->padding = 0;				// 													[����]
	
	// MATH_CalcCRC16CCITT																	[����] 
	MATHCRC16Table crctable;
	MATH_CRC16CCITTInitTable(&crctable);
	_pData->crc.crc16ccitt_hash = MATH_CalcCRC16CCITT(&crctable, _pData, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE);
}


// **                 
// ** �I�[����(0xFFFF)�������܂��B
// ** �o�b�t�@�I�[�o�[�t���[�ɒ��ӂ��Ă��������B
// ** (�K���o�b�t�@�̍Ō�̗v�f�ɂ�0������悤�ɂ��Ă��������B)
// **                 
void SetGDS_EOM(STRCODE* _pStr)
{
	_pStr[wcslen((wchar_t*)_pStr)] = 0xFFFF;
}


