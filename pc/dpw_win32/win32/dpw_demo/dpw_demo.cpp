
#include "stdafx.h"

#include "../../nitro/libdpw/dpw_bt/include/dpw_bt.h"
#include "../../nitro/libdpw/dpw_tr/include/dpw_tr.h"

#define TEST_KEY 0x100000000UL
#define MAIL_ADDR       "wfc-mail@nintendo.com" // ���̃��[���A�h���X�Ƀ��[�������M����܂�

typedef void (*TestProc)();

static char s_currentProxy[256] = {0};

// Dpw_Tr_Init�̑���ɂ�������g�p���Ă��������B
static void MyDpw_Tr_Init(int pid) {

	Dpw_Tr_Init(pid, TEST_KEY | pid);
}

// HTTP�v���L�V���Z�b�g����BNULL��������""�ŉ���
static void SetProxy(const char* proxy)
{
	if(proxy)
	{
		strcpy(s_currentProxy, proxy);
	}
	else
	{
		strcpy(s_currentProxy, "");
	}
	ghttpSetProxy(s_currentProxy);
}

// �񓯊��֐����I������܂ő҂�
static s32 WaitForAsync()
{
	while(!Dpw_Tr_IsAsyncEnd())
	{
		Dpw_Tr_Main();
		Sleep(10);
	}
	// ���N�G�X�g���I�������ēx�v���L�V���Z�b�g���Ȃ���΂Ȃ�Ȃ��B
	ghttpSetProxy(s_currentProxy);
	return Dpw_Tr_GetAsyncResult();
}

// �v���t�B�[�����Z�b�g����
static void SetProfile(Dpw_Common_Profile* profile)
{
	memset(profile, 0, sizeof(*profile));
	profile->version = 12;
	profile->language = 1;
	profile->countryCode = 103;
	profile->localCode = 1;
	profile->playerId = 87654321;
	profile->playerName[0] = 0x66;      // �T
	profile->playerName[1] = 0x79;      // �g
	profile->playerName[2] = 0x68;      // �V
	profile->playerName[3] = 0xffff;    // �I�[
	profile->flag = 0;
	strcpy(profile->mailAddr, MAIL_ADDR);
	profile->mailAddrAuthPass = DPW_MAIL_ADDR_AUTH_DEBUG_PASSWORD;   // �f�o�b�O�p�p�X���[�h���g�p����B
	profile->mailAddrAuthVerification = 456;
	profile->mailRecvFlag = DPW_PROFILE_MAILRECVFLAG_EXCHANGE;
}

// Dpw_Tr_Data���Z�b�g����
static void SetTrData(Dpw_Tr_Data* upload_data)
{
	// �R�N�[��
	u8 data[] = {57,148,138,197,0,0,19,71,0,225,226,116,104,187,11,169,205,198,239,184,227,255,202,161,239,133,127,140,240,161,240,216,218,41,84,70,114,199,108,49,9,11,13,170,10,159,215,139,122,119,196,182,144,222,10,165,255,5,39,199,92,169,30,208,159,170,35,150,233,137,68,57,61,192,222,48,184,97,214,129,199,149,235,2,51,78,168,8,50,63,184,113,88,235,164,121,39,174,75,187,206,137,149,99,23,87,77,12,166,84,227,11,122,23,165,0,177,112,190,172,49,209,41,5,54,142,254,210,176,165,17,74,129,129,171,153,64,114,145,74,27,162,201,134,219,36,246,122,83,172,14,162,147,57,250,245,81,152,113,165,218,37,51,51,228,198,16,92,111,223,224,104,234,160,34,71,70,131,62,212,7,7,90,177,247,109,244,134,227,190,220,50,54,197,124,119,9,133,210,16,1,10,178,178,29,251,90,150,53,231,242,188,35,194,198,246,18,178,21,67,39,33,129,179,97,4,0,76,14,66,18,45,12,186,7,205};

	memset(upload_data, 0, sizeof(*upload_data));
	memcpy(&upload_data->postData, data, sizeof(upload_data->postData));
	upload_data->postSimple.characterNo = 14;    // �R�N�[��
	upload_data->postSimple.gender = DPW_TR_GENDER_FEMALE;
	upload_data->postSimple.level = 20;
	upload_data->wantSimple.characterNo = 14;    // �R�N�[��
	upload_data->wantSimple.gender = DPW_TR_GENDER_FEMALE;
	upload_data->wantSimple.level_min = 0;
	upload_data->wantSimple.level_max = 0;
	upload_data->name[0] = 0x66; // �T
	upload_data->name[1] = 0x79; // �g
	upload_data->name[2] = 0x68; // �V
	upload_data->name[3] = 0xffff;
	upload_data->trainerID = 1 << 12;
	upload_data->countryCode = 103;
	upload_data->langCode = 1;
	upload_data->localCode = 1;
	upload_data->trainerType = 3;
	upload_data->versionCode = 12;
	upload_data->gender = 1;
}

static BOOL RequestCheckServerState()
{
	// �T�[�o�X�e�[�^�X�`�F�b�N
	{
		Dpw_Tr_GetServerStateAsync();
		if(WaitForAsync() != DPW_TR_STATUS_SERVER_OK )
		{
			puts("Invalid server status.");
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL RequestSetProfile()
{
	// �v���t�B�[���Z�b�g
	{
		Dpw_Common_Profile profile;
		Dpw_Common_ProfileResult profile_result;
		SetProfile(&profile);
		Dpw_Tr_SetProfileAsync(&profile, &profile_result);
		if(WaitForAsync() != 0 )
		{
			puts("Failed to set profile.");
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL RequestUpload()
{
	s32 result = 0;

	// �a����
	{
		Dpw_Tr_Data data;
		SetTrData(&data);
		Dpw_Tr_UploadAsync(&data);
		result = WaitForAsync();
		if(result == 0 )
		{
			// �a����̂��m��
			Dpw_Tr_UploadFinishAsync();
			if(WaitForAsync() != 0 )
			{
				puts("Failed to uploadFinish.");
				return FALSE;
			}
		}
		else if(result == DPW_TR_ERROR_ILLIGAL_REQUEST)
		{
			puts("Already uploaded.");
		}
		else
		{
			puts("Failed to upload.");
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL RequestPickup()
{
	// �������
	{
		Dpw_Tr_Data data;
		Dpw_Tr_DownloadAsync(&data);
		if(WaitForAsync() != 0 )
		{
			puts("Failed to download.");
			return FALSE;
		}

		Dpw_Tr_ReturnAsync();
		if(WaitForAsync() != 0 )
		{
			puts("Failed to return.");

			// �������ꂽ���̂̍폜������
			Dpw_Tr_DeleteAsync();
			if(WaitForAsync() != 0 )
			{
				puts("Failed to delete.");
				return FALSE;
			}
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL RequestTrade()
{
	s32 result = 0;
	Dpw_Tr_Data match_data_buf[DPW_TR_DOWNLOADMATCHDATA_MAX];	// �f�[�^�̌������ʂ�����o�b�t�@�B
	// �������g���[�h
	{
		// ����
		Dpw_Tr_PokemonSearchDataEx search_data;
		
		search_data.characterNo = 14;
		search_data.gender = DPW_TR_GENDER_FEMALE;
		search_data.level_min = 20;
		search_data.level_max = 20;	// 0���w�肷��ƁA����͐ݒ肵�Ȃ����ƂɂȂ�
        search_data.maxNum = DPW_TR_DOWNLOADMATCHDATA_MAX;
        search_data.countryCode = 103;
		
		Dpw_Tr_DownloadMatchDataExAsync(&search_data, match_data_buf);
		result = WaitForAsync();
		if(result < 0 )
		{
			puts("Failed to search.");
			return FALSE;
		}
		else if(result == 0)
		{
			puts("No such pokemon.");
			return FALSE;
		}

		printf("Found %d pokemons.\n", result);
	}
	{
		// �g���[�h
		Dpw_Tr_Data trade_data;
		Dpw_Tr_Data download_buf;

		SetTrData(&trade_data);
		Dpw_Tr_TradeAsync(match_data_buf[0].id, &trade_data, &download_buf);

		if(WaitForAsync() < 0 )
		{
			puts("Failed to trade.");
			return FALSE;
		}
		
		Dpw_Tr_TradeFinishAsync();
		if(WaitForAsync() != 0 )
		{
			puts("Failed to trade finish.");
			return FALSE;
		}

	}
	return TRUE;
}

static BOOL RequestPickupTraded()
{
	s32 result = 0;

	// �������ꂽ���̂��������
	{
		Dpw_Tr_Data data;
		Dpw_Tr_GetUploadResultAsync(&data);

		result = WaitForAsync();
		if(WaitForAsync() == 0 )
		{
			puts("Not traded.");
			return TRUE;
		}
		else if(result == 1)
		{
			puts("Traded.");
		}
		else
		{
			puts("Failed to upload result.");
			return FALSE;
		}

		Dpw_Tr_DeleteAsync();
		if(WaitForAsync() != 0 )
		{
			puts("Failed to delete.");
			return FALSE;
		}
	}
	return TRUE;
}


// GTS�������C��
static void TestUploadDownload()
{
	if(!RequestCheckServerState())
	{
		return;
	}

	if(!RequestSetProfile())
	{
		return;
	}

	if(!RequestUpload())
	{
		return;
	}

	if(!RequestPickup())
	{
		return;
	}
}

static void TestUpload()
{
	if(!RequestCheckServerState())
	{
		return;
	}

	if(!RequestSetProfile())
	{
		return;
	}

	if(!RequestUpload())
	{
		return;
	}
}

static void TestTrade()
{
	if(!RequestCheckServerState())
	{
		return;
	}

	if(!RequestSetProfile())
	{
		return;
	}

	if(!RequestUpload())
	{
		return;
	}

	if(!RequestTrade())
	{
		return;
	}
}

static void TestPickupTraded()
{
	if(!RequestCheckServerState())
	{
		return;
	}

	if(!RequestSetProfile())
	{
		return;
	}

	if(!RequestPickupTraded())
	{
		return;
	}
}

static void StartTest(s32 pid, TestProc testProc, const char* proxy)
{
	static int count = 1;
	MyDpw_Tr_Init(pid);

	SetProxy(proxy);

	testProc();

	Dpw_Tr_End();

	printf("Finished Test %d.\n", count++);
}

int _tmain(int argc, _TCHAR* argv[])
{
	// HTTP�v���L�V���K�v�ȏꍇ�̓v���L�V�̃A�h���X��"<server>[:port]"�̗l�Ɏw�肵�Ă��������B
	//const char* proxy = "proxy.server.com:8080";
	const char* proxy = "";

	// �a���Ĉ������
	StartTest(100000, TestUploadDownload, proxy);

	// �a����
	StartTest(100000, TestUpload, proxy);

	// ���ŗa�����|�P�����ƌ�������
	StartTest(100001, TestTrade, proxy);

	// �������ꂽ�|�P�������������
	StartTest(100000, TestPickupTraded, proxy);

	puts("Completed.");
	return 0;
}
