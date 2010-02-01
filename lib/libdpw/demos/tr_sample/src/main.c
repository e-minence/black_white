/*---------------------------------------------------------------------------*
  Project:  DP WiFi Library
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*! @file
	@brief	DPW TR �T���v��
	
	@author	kitayama(kitayama_shigetoshi@nintendo.co.jp)
	
	@version beta10	(2006/5/23)
		@li Dpw_Tr_Data::friend_key�̍폜�ɑΉ����܂����B
	@version beta9	(2006/5/22)
		@li Dpw_Tr_Data::friend_key�̒ǉ��ɑΉ����܂����B
	@version beta7	(2006/5/15)
		@li Dpw_Tr_Data::gender���ǉ����ꂽ���Ƃɂ��ύX�ɑΉ����܂����B
	@version beta6	(2006/5/8)
		@li Dpw_Tr_PokemonSearchData ���ǉ����ꂽ���Ƃɂ��ύX�ɑΉ����܂����B
	@version beta4	(2006/4/17)
		@li DPW���C�u�������g�p����O�ɁAWiFi�R�l�N�V�����ւ̃��O�C�����s���A�����ؒf����悤�ɂ��܂����B
	@version beta3	(2006/4/10)
		@li Dpw_Tr_Data �̕ύX�ɑΉ����܂����B
	@version beta2	(2006/3/27)
		@li Dpw_Tr_Data::mail �Ƀf�[�^�����Ă����������폜���܂����B
	@version beta1	(2006/3/17)
		@li ���ł������[�X���܂����B
*/

#include <nitro.h>
#include <dwc.h>

#include <wchar.h>

#include "dbs.h"
#include "init.h"
#include "userdata.h"
#include "dpw_tr.h"

/*-----------------------------------------------------------------------*
					�^�E�萔�錾
 *-----------------------------------------------------------------------*/

#define GAME_PRODUCTID   0			// ���̃T���v�����g�p����v���_�N�gID
#define GAME_NAME        "gmtest"	// ���̃T���v�����g�p����Q�[����
#define GAME_SECRET_KEY  "HA6zkS"	// ���̃T���v�����g�p����V�[�N���b�g�L�[

#define GAME_FRAME		2			// �z�肷��Q�[���t���[���i1/60��1�Ƃ���j

#define SEARCH_PID		100000		// �u�T���v�p��PID
#define SEARCH_KEY		123456789012 // �u�T���v�p�̃t�����h�L�[
#define MAIL_ADDR       "wfc-mail@nintendo.com" // ���̃��[���A�h���X�Ƀ��[�������M����܂�

typedef enum
{
	SAMPLE_START,
	SAMPLE_CONNECT_INET,
	SAMPLE_LOGIN,
	SAMPLE_LOGIN_WAIT,
	SAMPLE_TR_INIT,
	SAMPLE_TR_GET_SERVER_STATE,
	SAMPLE_TR_GET_SERVER_STATE_WAIT,
	SAMPLE_TR_INIT_SERVER,
	SAMPLE_TR_SETPROFILE1,
	SAMPLE_TR_SETPROFILE1_WAIT,
	SAMPLE_TR_UPLOAD_FOR_CANCEL,
	SAMPLE_TR_CANCEL,
	SAMPLE_TR_CANCEL_WAIT,
	SAMPLE_TR_UPLOAD,
	SAMPLE_TR_UPLOAD_WAIT,
	SAMPLE_TR_UPLOAD_FINISH,
	SAMPLE_TR_UPLOAD_FINISH_WAIT,
	SAMPLE_TR_DOWNLOAD,
	SAMPLE_TR_DOWNLOAD_WAIT,
	SAMPLE_TR_SETPROFILE2,
	SAMPLE_TR_SETPROFILE2_WAIT,
	SAMPLE_TR_DOWNLOAD_MATCH_DATA,
	SAMPLE_TR_DOWNLOAD_MATCH_DATA_WAIT,
	SAMPLE_TR_TRADE,
	SAMPLE_TR_TRADE_WAIT,
	SAMPLE_TR_TRADE_FINISH,
	SAMPLE_TR_TRADE_FINISH_WAIT,
	SAMPLE_TR_GET_UPLOAD_RESULT,
	SAMPLE_TR_GET_UPLOAD_RESULT_WAIT,
	SAMPLE_TR_DELETE,
	SAMPLE_TR_DELETE_WAIT,
	SAMPLE_TR_END,
	SAMPLE_ERROR_OCCURED,
	SAMPLE_END
} DpwTrSampleState;

/*-----------------------------------------------------------------------*
					�֐��v���g�^�C�v�錾
 *-----------------------------------------------------------------------*/

//--- Auto Function Prototype --- Don't comment here.
void NitroMain();
static void GameWaitVBlankIntr(void);
static void VBlankIntr(void);
static void set_upload_data(void);
static void set_trade_data(void);
static void disp_tr_data(const Dpw_Tr_Data* data);
static void fill_random_data(void *addr, u32 size);
static void login_callback(DWCError error, int profileID, void* param);
//--- End of Auto Function Prototype

/*-----------------------------------------------------------------------*
					�O���[�o���ϐ���`
 *-----------------------------------------------------------------------*/

static u8 s_Work[DWC_INIT_WORK_SIZE] ATTRIBUTE_ALIGN(32);
static DWCFriendsMatchControl stFMCtrl;
static DWCInetControl stConnCtrl;

// �L�[���
static u16 Trg;
static u16 Cont;

static BOOL is_logined;						// ���O�C�����Ă��邩
static int pid;								// �v���t�@�C��ID

static MATHRandContext16 rand_cont16;		// �����̏��

static DpwTrSampleState app_state;	// �A�v���P�[�V�����̏��
static Dpw_Tr_Data upload_data;		// �A�b�v���[�h����f�[�^�B
static Dpw_Tr_Data trade_data;		// �g���[�h����f�[�^�B
static Dpw_Tr_Data download_buf;	// �f�[�^���_�E�����[�h����o�b�t�@�B
static Dpw_Tr_Data match_data_buf[DPW_TR_DOWNLOADMATCHDATA_MAX];	// �f�[�^�̌������ʂ�����o�b�t�@�B
static s32 match_id;				// �u�T���v�̎��ɏ����Ƀq�b�g���������ID
static Dpw_Common_Profile profile1; // �a���鑤�̌l���
static Dpw_Common_Profile profile2; // �T�����̌l���
static Dpw_Common_ProfileResult profile_result; // �l���o�^�̌���

static DPW_SERVER_TYPE server_type = DPW_SERVER_DEBUG;

/*-----------------------------------------------------------------------*
					�O���[�o���֐���`
 *-----------------------------------------------------------------------*/

void NitroMain()
{
	// �F�؃T�[�o���瑗���Ă����g�[�N��
	u8 token[TOKEN_LEN] = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde";
	
	//================ ������
	//---- OS ������
	OS_Init();
	OS_InitTick();
	OS_InitAlarm();
	RTC_Init();
	Heap_Init();
	MATH_InitRand16(&rand_cont16, 0x12345678);

	// �X�^�b�N���`�F�b�N�����ݒ�
	OS_SetThreadStackWarningOffset(OS_GetCurrentThread(), 0x100);

	//---- GX ������
	GX_Init();
	GX_DispOff();
	GXS_DispOff();

	//================ �ݒ�
	//---- �d���S�ăI��
	GX_SetPower(GX_POWER_ALL);

	Cont = PAD_Read();

	//---- VRAM �N���A
	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	(void)GX_DisableBankForLCDC();

	//---- OAM�ƃp���b�g�N���A
	MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
	MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);

	// �o�͏�����
	dbs_DemoInit();
	DTDB_SetPrintOverride(TRUE);

	//----  V�u�����N��������
	OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
	(void)OS_EnableIrqMask(OS_IE_V_BLANK);
	(void)OS_EnableIrq();
	(void)OS_EnableInterrupts();
	(void)GX_VBlankIntr(TRUE);
	GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

	// DWC�̏�����
	DWC_SetReportLevel(DWC_REPORTFLAG_ALL);
#if defined(_NITRO) && SDK_VERSION_MAJOR > 4
    DWC_InitForDevelopment("syachi2ds", 'IRAJ', DTDB_GetAllocFunc(), DTDB_GetFreeFunc());
#else
    DWC_Init(s_Work);
    DWC_SetMemFunc(DTDB_GetAllocFunc(), DTDB_GetFreeFunc());
#endif

	// �A�v���P�[�V�����̏�����
	app_state = SAMPLE_START;

	// �f�[�^��ݒ肷��
	set_upload_data();
	set_trade_data();
	
	//---- �\���J�n
	OS_WaitIrq(TRUE, OS_IE_V_BLANK);
	GX_DispOn();
	GXS_DispOn();
	
	while (1)
	{
		u16 keyData;

		{	// �J�E���g�A�b�v
			static u8 count;
			dbs_Print(0, 0, "DPW TR SAMPLE " __DATE__ "     %02x", count++);
		}

		GameWaitVBlankIntr();

		//---- �p�b�h�f�[�^�ǂݍ���
		keyData = PAD_Read();
		Trg = (u16) (keyData & (keyData ^ Cont));
		Cont = keyData;

		// Dpw_Tr_Main() �����͗�O�I�ɂ��ł��Ăׂ�
		Dpw_Tr_Main();

		switch (app_state)
		{
		case SAMPLE_START:
			DWC_InitInet(&stConnCtrl);
			DWC_ConnectInetAsync();

			app_state = SAMPLE_CONNECT_INET;
			break;
		case SAMPLE_CONNECT_INET:
			DWC_ProcessInet();

			if (DWC_CheckInet())
			{
				switch (DWC_GetInetStatus())
				{
				case DWC_CONNECTINET_STATE_ERROR:
					{
						// �G���[�\��
						DWCError err;
						int errcode;
						err = DWC_GetLastError(&errcode);

						OS_TPrintf("   Error occurred %d %d.\n", err, errcode);
					}
					DWC_ClearError();
					DWC_CleanupInet();
					app_state = SAMPLE_START;	// �ڑ�������
					break;
				case DWC_CONNECTINET_STATE_FATAL_ERROR:
					{
						// �G���[�\��
						DWCError err;
						int errcode;
						err = DWC_GetLastError(&errcode);

						OS_Panic("   Error occurred %d %d.\n", err, errcode);
					}
					break;

				case DWC_CONNECTINET_STATE_CONNECTED:
			        {	// �ڑ����\������B�X�܂̏ꍇ�͓X�܏����\������B
						DWCApInfo apinfo;
			
						DWC_GetApInfo(&apinfo);
			
			            OS_TPrintf("   Connected to AP type %d.\n", apinfo.aptype);

			            if (apinfo.aptype == DWC_APINFO_TYPE_SHOP)
			            {
							OS_TPrintf("<Shop AP Data>\n");
			                OS_TPrintf("area code: %d.\n", apinfo.area);
			                OS_TPrintf("spotinfo : %s.\n", apinfo.spotinfo);
			            }
			        }
					app_state = SAMPLE_LOGIN;
					break;
				}
			}
			break;
		case SAMPLE_LOGIN:
		    // ���[�U�f�[�^�ǂݍ��݁B�Q�[���ɑg�ݍ��ލۂ́A�Q�[���ŗL�̏������s���Ă��������B
		    DTUD_Init();
			
		    // �t�����h�}�b�`���C�u�����������B�Q�[���ɑg�ݍ��ލۂ́A�Q�[���ŗL�̏������s���Ă��������B
#if defined(_NITRO) && SDK_VERSION_MAJOR > 4
            DWC_InitFriendsMatch(DTUD_GetUserData(),
                                 GAME_PRODUCTID, GAME_SECRET_KEY,
                                 0, 0,
                                 NULL, 0 );
#else
            DWC_InitFriendsMatch(&stFMCtrl, DTUD_GetUserData(),
                                 GAME_PRODUCTID, GAME_NAME, GAME_SECRET_KEY,
                                 0, 0,
                                 NULL, 0 );
#endif
			
			is_logined = FALSE;
			
            // �F�ؗp�֐����g���ă��O�C��
            DWC_LoginAsync(L"�T�g�V", NULL, login_callback, NULL);
			
			app_state = SAMPLE_LOGIN_WAIT;
			break;
		case SAMPLE_LOGIN_WAIT:
		    DWC_ProcessFriendsMatch();  // DWC�ʐM�����X�V
			
			if (is_logined)
			{
		        // ingamesn�`�F�b�N�̌��ʂ��擾����
			    if( DWC_GetIngamesnCheckResult() == DWC_INGAMESN_INVALID )
		        {
		            // �s�K�؂Ȗ��O�Ɣ��f���ꂽ�ꍇ�͓��ʂȏ������K�v
				    OS_TPrintf(" BAD ingamesn is detected by NAS.\n");
		        }
				
				// �����Ƀ��O�I�t
	            DWC_ShutdownFriendsMatch();
				
				if (pid == SEARCH_PID) {
					// �擾����PID���u�T���v�p��PID�Ƃ��Ԃ��Ă��܂����Ƃ��́A�T���v�������s�ł��Ȃ�
				    OS_TPrintf(" error: get same pid as \"search\" side.\n");
					app_state = SAMPLE_ERROR_OCCURED;
					break;
				}
				
				app_state = SAMPLE_TR_INIT;
			}
			break;
		case SAMPLE_TR_INIT:
			// Dpw_Tr�̏��������s���B
			// �A�b�v���[�h�p��PID�ŏ���������B
			Dpw_Tr_Init(pid, DWC_CreateFriendKey(DTUD_GetUserData()), server_type);
			OS_TPrintf(" init with upload pid=%d.\n", pid);
			app_state = SAMPLE_TR_GET_SERVER_STATE;
			break;
		case SAMPLE_TR_GET_SERVER_STATE:
			// �T�[�o�[�̏�Ԃ��擾����
			Dpw_Tr_GetServerStateAsync();
			app_state = SAMPLE_TR_GET_SERVER_STATE_WAIT;
			break;
		case SAMPLE_TR_GET_SERVER_STATE_WAIT:
			if (Dpw_Tr_IsAsyncEnd())
			{
				s32 result = Dpw_Tr_GetAsyncResult();

				if (result < 0)
				{	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				}
				else
				{
					switch (result)
					{
					case DPW_TR_STATUS_SERVER_OK:		// ����ɓ��삵�Ă���
						OS_TPrintf(" server is up!\n");
						app_state = SAMPLE_TR_SETPROFILE1;
						break;
					case DPW_TR_STATUS_SERVER_STOP_SERVICE:	// �T�[�r�X��~��
						OS_TPrintf(" server stop service.\n");
						app_state = SAMPLE_ERROR_OCCURED;
						break;
					case DPW_TR_STATUS_SERVER_FULL:		// �T�[�o�[�����t
						OS_TPrintf(" server full.\n");
						app_state = SAMPLE_ERROR_OCCURED;
						break;
					}
				}
			}
			break;
		case SAMPLE_TR_INIT_SERVER:
            /*
			// �T�[�o�[������������B
			// ���̍�Ƃ́A�{�ԃT�[�o�[�ł͔�펖�Ԃɂ����s���Ȃ��B
			// �e�X�g�T�[�o�[�ł̓f�o�b�O�p�̊֐���p���ĔC�ӂ̃^�C�~���O�ōs�����Ƃ��ł���B
            if (Dpw_Tr_Db_InitServer())
            {
                OS_TPrintf(" server init success.\n");
                app_state = SAMPLE_TR_UPLOAD_FOR_CANCEL;
            }
            else
            {
                OS_TPrintf(" server init failed.\n");
                app_state = SAMPLE_ERROR_OCCURED;
            }
            */
			break;
        case SAMPLE_TR_SETPROFILE1:
            memset(&profile1, 0, sizeof(profile1));
			profile1.version = 10;
            profile1.language = 1;
            profile1.countryCode = 103;
            profile1.localCode = 1;
            profile1.playerId = 87654321;
        	profile1.playerName[0] = 0x66;      // �T
        	profile1.playerName[1] = 0x79;      // �g
        	profile1.playerName[2] = 0x68;      // �V
        	profile1.playerName[3] = 0xffff;    // �I�[
            profile1.flag = 0;
            strcpy(profile1.mailAddr, MAIL_ADDR);
            profile1.mailAddrAuthPass = DPW_MAIL_ADDR_AUTH_DEBUG_PASSWORD;   // �f�o�b�O�p�p�X���[�h���g�p����B
            profile1.mailAddrAuthVerification = 456;
            profile1.mailRecvFlag = DPW_PROFILE_MAILRECVFLAG_EXCHANGE;
            
			Dpw_Tr_SetProfileAsync(&profile1, &profile_result);
			
			app_state = SAMPLE_TR_SETPROFILE1_WAIT;
            break;
        case SAMPLE_TR_SETPROFILE1_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
                    
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
                    OS_TPrintf(" setProfile result. code:%u, mailAddrAuthResult:%u\n", profile_result.code, profile_result.mailAddrAuthResult);
                    if(profile_result.code == DPW_PROFILE_RESULTCODE_SUCCESS && profile_result.mailAddrAuthResult == DPW_PROFILE_AUTHRESULT_SUCCESS)
                    {
                        // �f�o�b�O�p�p�X���[�h���g���Ă���̂ŕK����������͂�
    					app_state = SAMPLE_TR_UPLOAD_FOR_CANCEL;
                    } else {
                        OS_TPrintf(" setProfile failed.");
					    app_state = SAMPLE_ERROR_OCCURED;
                    }
				}
			}
            break;
		case SAMPLE_TR_UPLOAD_FOR_CANCEL:
			// �u�a����v�����̊J�n�B
			// �f�[�^���A�b�v���[�h����B
			Dpw_Tr_UploadAsync(&upload_data, token, TOKEN_LEN);
			app_state = SAMPLE_TR_CANCEL;
			break;
		case SAMPLE_TR_CANCEL:
			// �A�b�v���[�h���L�����Z�����Ă݂�
			Dpw_Tr_CancelAsync();
			app_state = SAMPLE_TR_CANCEL_WAIT;
			break;
		case SAMPLE_TR_CANCEL_WAIT:
			if (Dpw_Tr_IsAsyncEnd())
			{
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result == DPW_TR_ERROR_CANCEL)
				{	// �L�����Z���ł����Ƃ�
					OS_TPrintf(" cancel upload sccess.\n");
					app_state = SAMPLE_TR_UPLOAD;
				}
				else if (result == 0)
				{
					// �L�����Z���ł����������Ԃ��Ă����ꍇ�́A�������I������
					// ���܂��Ă���B�A�b�v���[�h���I�����Ă�����̂Ƃ��đ��s�B
					OS_TPrintf(" cannot cancel, upload data. ");
					app_state = SAMPLE_TR_UPLOAD_FINISH;
				}
				else
				{	// �L�����Z���ȊO�̃G���[�̂Ƃ��B
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				}
			}
			break;
		case SAMPLE_TR_UPLOAD:
			// �u�a����v�����̊J�n�B
			// �f�[�^���A�b�v���[�h����B
			Dpw_Tr_UploadAsync(&upload_data, token, TOKEN_LEN);
			app_state = SAMPLE_TR_UPLOAD_WAIT;
			break;
		case SAMPLE_TR_UPLOAD_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
					// �A�b�v���[�h�����B�Z�[�u����B
					OS_TPrintf(" upload success.\n");
					app_state = SAMPLE_TR_UPLOAD_FINISH;
				}
			}
			break;
		case SAMPLE_TR_UPLOAD_FINISH:
			// �A�b�v���[�h�̊������T�[�o�֒ʒm����B
			Dpw_Tr_UploadFinishAsync();
			app_state = SAMPLE_TR_UPLOAD_FINISH_WAIT;
			break;
		case SAMPLE_TR_UPLOAD_FINISH_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
					OS_TPrintf(" upload finish success.\n");
					app_state = SAMPLE_TR_DOWNLOAD;
				}
			}
			break;
		case SAMPLE_TR_DOWNLOAD:
			// �A�b�v���[�h�����f�[�^���m�F����B
			Dpw_Tr_DownloadAsync(&download_buf);
			app_state = SAMPLE_TR_DOWNLOAD_WAIT;
			break;
		case SAMPLE_TR_DOWNLOAD_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
					disp_tr_data(&download_buf);
					
					// �A�b�v���[�h�����f�[�^�Ɣ�r���邽�߂Ɉꕔ���킹��
					download_buf.id = 0;
					MI_CpuClear8(&download_buf.postDate, sizeof(Dpw_Tr_Date));
					
					// �A�b�v���[�h�����f�[�^�Ɣ�r����
					if (memcmp(&download_buf, &upload_data, sizeof(Dpw_Tr_Data)) == 0) {
						OS_TPrintf("  download correct data.\n");
						app_state = SAMPLE_TR_SETPROFILE2;
					} else {
						OS_TPrintf("  download incorrect data.\n");
						app_state = SAMPLE_ERROR_OCCURED;
					}
					
					// �u�T���v�V�[�P���X�ɓ��邽�߁A��x�I������
					Dpw_Tr_End();
					OS_TPrintf(" call tr end.\n");
				}
			}
			break;
        case SAMPLE_TR_SETPROFILE2:
			// �u�T���v�V�[�P���X�ɓ��邽�߁A�ʂ�PID�ŏ�����������
			Dpw_Tr_Init(SEARCH_PID, SEARCH_KEY, server_type);
			OS_TPrintf(" init with search pid=%d.\n", SEARCH_PID);
			
            memset(&profile2, 0, sizeof(profile2));
			profile2.version = 10;
            profile2.language = 1;
            profile2.countryCode = 103;
            profile2.localCode = 1;
            profile2.playerId = 12345678;
        	profile2.playerName[0] = 0x5c;      // �J
        	profile2.playerName[1] = 0x6a;      // �X
        	profile2.playerName[2] = 0x90;      // �~
        	profile2.playerName[3] = 0xffff;    // �I�[
            profile2.flag = 0;
            strcpy(profile2.mailAddr, MAIL_ADDR);
            profile2.mailAddrAuthPass = DPW_MAIL_ADDR_AUTH_DEBUG_PASSWORD;   // �f�o�b�O�p�p�X���[�h���g�p����B
            profile2.mailAddrAuthVerification = 123;
            profile2.mailRecvFlag = DPW_PROFILE_MAILRECVFLAG_EXCHANGE;
            
			Dpw_Tr_SetProfileAsync(&profile2, &profile_result);
			
			app_state = SAMPLE_TR_SETPROFILE2_WAIT;
            break;
        case SAMPLE_TR_SETPROFILE2_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
                    
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
                    OS_TPrintf(" setProfile result. code:%u, mailAddrAuthResult:%u\n", profile_result.code, profile_result.mailAddrAuthResult);
                    if(profile_result.code == DPW_PROFILE_RESULTCODE_SUCCESS && profile_result.mailAddrAuthResult == DPW_PROFILE_AUTHRESULT_SUCCESS)
                    {
                        // �f�o�b�O�p�p�X���[�h���g���Ă���̂ŕK����������͂�
    					app_state = SAMPLE_TR_DOWNLOAD_MATCH_DATA;
                    } else {
                        OS_TPrintf(" setProfile failed.");
					    app_state = SAMPLE_ERROR_OCCURED;
                    }
				}
			}
            break;
		case SAMPLE_TR_DOWNLOAD_MATCH_DATA:
			// �����ɍ������|�P��������������B
			// �����ɃA�b�v���[�h�����|�P�����̂��̂�n���Ă���̂ŁA�A�b�v���[�h�����|�P���������������͂��B
			/* // �]���̌���
            {
				Dpw_Tr_PokemonSearchData search_data;
				
				search_data.characterNo = upload_data.postSimple.characterNo;
				search_data.gender = upload_data.postSimple.gender;
				search_data.level_min = upload_data.postSimple.level;
				search_data.level_max = 0;	// 0���w�肷��ƁA����͐ݒ肵�Ȃ����ƂɂȂ�
				
				Dpw_Tr_DownloadMatchDataAsync(&search_data, DPW_TR_DOWNLOADMATCHDATA_MAX, match_data_buf);
			}
            */
            // ���R�[�h�t���̌���
			{
				Dpw_Tr_PokemonSearchDataEx search_data;
				
				search_data.characterNo = upload_data.postSimple.characterNo;
				search_data.gender = upload_data.postSimple.gender;
				search_data.level_min = upload_data.postSimple.level;
				search_data.level_max = 0;	// 0���w�肷��ƁA����͐ݒ肵�Ȃ����ƂɂȂ�
                search_data.maxNum = DPW_TR_DOWNLOADMATCHDATA_MAX;
                search_data.countryCode = 103;
				
				Dpw_Tr_DownloadMatchDataExAsync(&search_data, match_data_buf);
			}
			app_state = SAMPLE_TR_DOWNLOAD_MATCH_DATA_WAIT;
			break;
		case SAMPLE_TR_DOWNLOAD_MATCH_DATA_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
					OS_TPrintf(" download match data success.\n");
					OS_TPrintf("  match data num: %d\n", result);
					
					if (result > 0) {
						// ��������ID���o���Ă���
						match_id = match_data_buf[0].id;
						
						disp_tr_data(&match_data_buf[0]);
					
						// �A�b�v���[�h�����f�[�^�Ɣ�r���邽�߂Ɉꕔ���킹��
						match_data_buf[0].id = 0;
						MI_CpuClear8(&match_data_buf[0].postDate, sizeof(Dpw_Tr_Date));
					
						// �A�b�v���[�h�����f�[�^�Ɣ�r����
						if (memcmp(&match_data_buf[0], &upload_data, sizeof(Dpw_Tr_Data)) == 0) {
							OS_TPrintf("  download correct match data.\n");
							app_state = SAMPLE_TR_TRADE;
						} else {
							OS_TPrintf("  download incorrect data. May be renamed NG or hangeul name.\n");
							app_state = SAMPLE_TR_TRADE;
						}
					} else {
						// �����Ɉ�̂��q�b�g���Ȃ�����
						OS_TPrintf(" no match data.");
						app_state = SAMPLE_ERROR_OCCURED;
					}
				}
			}
			break;
		case SAMPLE_TR_TRADE:
			// �������������ID���w�肵�āA�f�[�^���g���[�h����B
			Dpw_Tr_TradeAsync(match_id, &trade_data, &download_buf, token, TOKEN_LEN);
			app_state = SAMPLE_TR_TRADE_WAIT;
			break;
		case SAMPLE_TR_TRADE_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
					// �g���[�h�����������B�Z�[�u���s���B
					OS_TPrintf(" trade success.\n");
					
					disp_tr_data(&download_buf);
			
					// �A�b�v���[�h�����f�[�^�Ɣ�r���邽�߂Ɉꕔ���킹��
					download_buf.id = 0;
					download_buf.isTrade = 0;
					MI_CpuClear8(&download_buf.postDate, sizeof(Dpw_Tr_Date));
					MI_CpuClear8(&download_buf.tradeDate, sizeof(Dpw_Tr_Date));
					
					// ��������Ă���ׂ������ȊO��upload_data�ɍ��킹��
					download_buf.postSimple = upload_data.postSimple;
					download_buf.wantSimple = upload_data.wantSimple;
					
					if (memcmp(&download_buf, &upload_data, sizeof(Dpw_Tr_Data)) == 0) {
						OS_TPrintf("  get correct trade result.\n");
						app_state = SAMPLE_TR_TRADE_FINISH;
					} else {
						OS_TPrintf("  get incorrect upload result. May be renamed NG or hangeul name.\n");
						app_state = SAMPLE_TR_TRADE_FINISH;
					}
				}
			}
			break;
		case SAMPLE_TR_TRADE_FINISH:
			// �T�[�o�[�Ɍ����̏I����ʒm����
			Dpw_Tr_TradeFinishAsync();
			app_state = SAMPLE_TR_TRADE_FINISH_WAIT;
			break;
		case SAMPLE_TR_TRADE_FINISH_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
					OS_TPrintf(" trade finish success.\n");
					
					// �u�a����v���ɖ߂邽�߁ADpw_Tr���I������B
					Dpw_Tr_End();
					OS_TPrintf(" call tr end.\n");
					
					app_state = SAMPLE_TR_GET_UPLOAD_RESULT;
				}
			}
			break;
		case SAMPLE_TR_GET_UPLOAD_RESULT:
			// �u�������ꂽ�|�P�������������v�V�[�P���X�ɖ߂邽�߁A������������
			Dpw_Tr_Init(pid, DWC_CreateFriendKey(DTUD_GetUserData()), server_type);
			OS_TPrintf(" init with upload pid=%d.\n", pid);
			
			// �a�������ʂ��擾����
			Dpw_Tr_GetUploadResultAsync(&download_buf);
			app_state = SAMPLE_TR_GET_UPLOAD_RESULT_WAIT;
			break;
		case SAMPLE_TR_GET_UPLOAD_RESULT_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
					OS_TPrintf(" get upload result success.\n");
					
					if (result == 0) {
						// 0���Ԃ����Ƃ��͌�������Ă��Ȃ��B
						OS_TPrintf("  data is not exchanged. I might trade other user's pokemon or my last uploaded pokemon.\n");
						app_state = SAMPLE_ERROR_OCCURED;
					} else if (result == 1) {
						// 1���Ԃ����Ƃ��͌�������Ă���B�f�[�^�̍폜�֐i��
						OS_TPrintf("  data is exchanged.\n");
						
						disp_tr_data(&download_buf);
						
						// �f�[�^����������Ă��邩�m�F���邽�߂Ɉꕔ���킹��
						download_buf.id = 0;
						download_buf.isTrade = 0;
						MI_CpuClear8(&download_buf.postDate, sizeof(Dpw_Tr_Date));
						MI_CpuClear8(&download_buf.tradeDate, sizeof(Dpw_Tr_Date));
						
						// ��������Ă���ׂ������ȊO��trade_data�ɍ��킹��
						download_buf.postSimple = trade_data.postSimple;
						download_buf.wantSimple = trade_data.wantSimple;
						
						if (memcmp(&download_buf, &trade_data, sizeof(Dpw_Tr_Data)) == 0) {
							OS_TPrintf("  get correct upload result.\n");
							app_state = SAMPLE_TR_DELETE;
						} else {
							OS_TPrintf("  get incorrect upload result. May be renamed NG or hangeul name.\n");
							app_state = SAMPLE_TR_DELETE;
						}
					}
				}
			}
			break;
		case SAMPLE_TR_DELETE:
			// �T�[�o�[��̃f�[�^���폜����
			Dpw_Tr_DeleteAsync();
			app_state = SAMPLE_TR_DELETE_WAIT;
			break;
		case SAMPLE_TR_DELETE_WAIT:
			if (Dpw_Tr_IsAsyncEnd()) {
				s32 result = Dpw_Tr_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
					// �폜�����������B
					OS_TPrintf(" delete success.\n");
					app_state = SAMPLE_TR_END;
				}
			}
			break;
		case SAMPLE_TR_END:
			// Dpw_Tr���C�u�������I������B
			Dpw_Tr_End();
			OS_TPrintf("DPW TR sample end.\n");
			app_state = SAMPLE_END;
			break;
		case SAMPLE_ERROR_OCCURED:
			break;
		case SAMPLE_END:
			break;
		}
	}
}

/*-----------------------------------------------------------------------*
					���[�J���֐���`
 *-----------------------------------------------------------------------*/

// V�u�����N�҂��֐�
static void GameWaitVBlankIntr(void)
{
	int i;

	dbs_DemoUpdate();

	// �z�肷��Q�[���t���[���ɍ��킹��V�u�����N�҂��̉񐔂�ς���
	for (i = 0; i < GAME_FRAME; i++)
	{
		OS_WaitIrq(TRUE, OS_IE_V_BLANK);
	}

	Heap_Debug();
	dbs_DemoLoad();

	// �X�^�b�N���`�F�b�N
	OS_CheckStack(OS_GetCurrentThread());
}

// V�u�����N���荞�݊֐�
static void VBlankIntr(void)
{
	//---- ���荞�݃`�F�b�N�t���O
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

static void set_upload_data(void) {
    // ���ʂ̃R�N�[��
    u8 data[] = {57,148,138,197,0,0,19,71,0,225,226,116,104,187,11,169,205,198,239,184,227,255,202,161,239,133,127,140,240,161,240,216,218,41,84,70,114,199,108,49,9,11,13,170,10,159,215,139,122,119,196,182,144,222,10,165,255,5,39,199,92,169,30,208,159,170,35,150,233,137,68,57,61,192,222,48,184,97,214,129,199,149,235,2,51,78,168,8,50,63,184,113,88,235,164,121,39,174,75,187,206,137,149,99,23,87,77,12,166,84,227,11,122,23,165,0,177,112,190,172,49,209,41,5,54,142,254,210,176,165,17,74,129,129,171,153,64,114,145,74,27,162,201,134,219,36,246,122,83,172,14,162,147,57,250,245,81,152,113,165,218,37,51,51,228,198,16,92,111,223,224,104,234,160,34,71,70,131,62,212,7,7,90,177,247,109,244,134,227,190,220,50,54,197,124,119,9,133,210,16,1,10,178,178,29,251,90,150,53,231,242,188,35,194,198,246,18,178,21,67,39,33,129,179,97,4,0,76,14,66,18,45,12,186,7,205};
    
    // �n���O���q�R�U���f�[�^
    //u8 data[] = {174,106,98,162,0,0,77,246,179,93,203,190,210,194,64,248,47,105,156,88,150,156,1,34,224,20,141,26,178,219,122,50,117,32,98,25,232,134,220,118,9,230,21,104,110,236,146,244,208,67,165,85,236,108,155,39,125,17,41,34,148,48,236,143,166,141,41,33,188,104,185,65,169,214,205,53,185,192,49,230,55,212,84,32,222,230,82,1,194,151,97,58,230,39,89,157,68,137,166,59,42,228,77,141,193,68,20,72,122,207,209,184,176,172,74,184,230,68,127,6,173,101,73,124,88,138,103,199,85,69,134,160,222,179,139,157,252,208,36,22,82,127,44,17,50,191,69,94,69,52,161,219,82,95,0,22,6,81,70,132,142,205,89,108,163,19,87,22,46,109,36,36,4,209,161,119,10,82,138,121,16,61,35,235,212,240,209,42,133,87,161,159,4,109,38,193,216,123,89,70,136,247,29,108,224,69,5,184,76,17,249,63,186,234,58,20,115,149,73,47,88,164,179,209,38,149,105,178,113,94,18,222,19,210,250,63};

    memset(&upload_data, 0, sizeof(Dpw_Tr_Data));
    memcpy(&upload_data.postData, data, sizeof(upload_data.postData));
	upload_data.postSimple.characterNo = 14;    // �R�N�[��
	//upload_data.postSimple.characterNo = 390;   // �q�R�U��
	upload_data.postSimple.gender = DPW_TR_GENDER_FEMALE;
	upload_data.postSimple.level = 1;
	upload_data.wantSimple.characterNo = 14;    // �R�N�[��
	//upload_data.wantSimple.characterNo = 390;   // �q�R�U��
	upload_data.wantSimple.gender = DPW_TR_GENDER_FEMALE;
	upload_data.wantSimple.level_min = 0;
	upload_data.wantSimple.level_max = 0;
	upload_data.name[0] = 0x66; // �T
	upload_data.name[1] = 0x79; // �g
	upload_data.name[2] = 0x68; // �V
	upload_data.name[3] = 0xffff;
	upload_data.trainerID = 1 << 12;    // �u�����͏��4bit�ɉ����ĕϊ�����̂ł��̃e�X�g
	upload_data.countryCode = 103;
    upload_data.langCode = 1;
	upload_data.localCode = 1;
	upload_data.trainerType = 3;
	upload_data.versionCode = 10;
	upload_data.gender = 1;
	upload_data.favorite = 2;
	upload_data.countryCount = 2;
	upload_data.nature = 2;
}

static void set_trade_data(void) {
    // ���ʂ̃R�N�[��
    u8 data[] = {57,148,138,197,0,0,19,71,0,225,226,116,104,187,11,169,205,198,239,184,227,255,202,161,239,133,127,140,240,161,240,216,218,41,84,70,114,199,108,49,9,11,13,170,10,159,215,139,122,119,196,182,144,222,10,165,255,5,39,199,92,169,30,208,159,170,35,150,233,137,68,57,61,192,222,48,184,97,214,129,199,149,235,2,51,78,168,8,50,63,184,113,88,235,164,121,39,174,75,187,206,137,149,99,23,87,77,12,166,84,227,11,122,23,165,0,177,112,190,172,49,209,41,5,54,142,254,210,176,165,17,74,129,129,171,153,64,114,145,74,27,162,201,134,219,36,246,122,83,172,14,162,147,57,250,245,81,152,113,165,218,37,51,51,228,198,16,92,111,223,224,104,234,160,34,71,70,131,62,212,7,7,90,177,247,109,244,134,227,190,220,50,54,197,124,119,9,133,210,16,1,10,178,178,29,251,90,150,53,231,242,188,35,194,198,246,18,178,21,67,39,33,129,179,97,4,0,76,14,66,18,45,12,186,7,205};
    
    // �n���O���q�R�U���f�[�^
    //u8 data[] = {174,106,98,162,0,0,77,246,179,93,203,190,210,194,64,248,47,105,156,88,150,156,1,34,224,20,141,26,178,219,122,50,117,32,98,25,232,134,220,118,9,230,21,104,110,236,146,244,208,67,165,85,236,108,155,39,125,17,41,34,148,48,236,143,166,141,41,33,188,104,185,65,169,214,205,53,185,192,49,230,55,212,84,32,222,230,82,1,194,151,97,58,230,39,89,157,68,137,166,59,42,228,77,141,193,68,20,72,122,207,209,184,176,172,74,184,230,68,127,6,173,101,73,124,88,138,103,199,85,69,134,160,222,179,139,157,252,208,36,22,82,127,44,17,50,191,69,94,69,52,161,219,82,95,0,22,6,81,70,132,142,205,89,108,163,19,87,22,46,109,36,36,4,209,161,119,10,82,138,121,16,61,35,235,212,240,209,42,133,87,161,159,4,109,38,193,216,123,89,70,136,247,29,108,224,69,5,184,76,17,249,63,186,234,58,20,115,149,73,47,88,164,179,209,38,149,105,178,113,94,18,222,19,210,250,63};
    
    memset(&trade_data, 0, sizeof(Dpw_Tr_Data));
    memcpy(&trade_data.postData, data, sizeof(trade_data.postData));
	trade_data.postSimple.characterNo = 14;    // �R�N�[��
	//trade_data.postSimple.characterNo = 390;   // �q�R�U��
	trade_data.postSimple.gender = DPW_TR_GENDER_FEMALE;
	trade_data.postSimple.level = 20;
	trade_data.wantSimple.characterNo = 14;    // �R�N�[��
	//trade_data.wantSimple.characterNo = 390;   // �q�R�U��
	trade_data.wantSimple.gender = DPW_TR_GENDER_NONE;
	trade_data.wantSimple.level_min = 0;
	trade_data.wantSimple.level_max = 0;
	trade_data.name[0] = 0x5c;      // �J
	trade_data.name[1] = 0x6a;      // �X
	trade_data.name[2] = 0x90;      // �~
	trade_data.name[3] = 0xffff;    // �I�[
	trade_data.trainerID = 2 << 12;    // �u�����͏��4bit�ɉ����ĕϊ�����̂ł��̃e�X�g
	trade_data.countryCode = 103;
	trade_data.localCode = 1;
    trade_data.langCode = 8;
	trade_data.trainerType = 4;
	trade_data.versionCode = 10;
	trade_data.gender = 2;
	trade_data.favorite = 1;
	trade_data.countryCount = 1;
	trade_data.nature = 1;
}

static void disp_tr_data(const Dpw_Tr_Data* data) {
	
	OS_TPrintf(" is trade  : %d\n", data->isTrade);
	OS_TPrintf(" id        : %d\n", data->id);
	OS_TPrintf(" post time : %d/%d/%d %d:%d:%d \n",
			   data->postDate.year,
			   data->postDate.month,
			   data->postDate.day,
			   data->postDate.hour,
			   data->postDate.minutes,
			   data->postDate.sec);
	OS_TPrintf(" trade time: %d/%d/%d %d:%d:%d \n",
			   data->tradeDate.year,
			   data->tradeDate.month,
			   data->tradeDate.day,
			   data->tradeDate.hour,
			   data->tradeDate.minutes,
			   data->tradeDate.sec);
}

static void fill_random_data(void *addr, u32 size)
{
	u32 i;

	for (i = 0; i < size; i++)
	{
		((u8 *) addr)[i] = (u8) MATH_Rand16(&rand_cont16, 0x100);
	}
}

static void login_callback(DWCError error, int profileID, void* param)
{
#pragma unused(param)
	
	if (error == DWC_ERROR_NONE)
	{
        OS_TPrintf(" Login succeeded. profileID = %u\n", profileID);
		
		// PID���擾
		pid = profileID;
		
        // stUserData���X�V����Ă��邩�ǂ������m�F�B
        if ( DTUD_CheckUpdateUserData() )
        {
            DTUD_SaveUserData();
        }
		
		is_logined = TRUE;
	}
	else
	{
        OS_TPrintf(" Login Error\n");
	}
}
