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
	@brief	DPW BT �T���v��
	
	@author	kitayama(kitayama_shigetoshi@nintendo.co.jp)
	
	@version beta10	(2006/5/23)
		@li Dpw_Bt_Player::friend_key, Dpw_Bt_Leader::friend_key�̍폜�ɑΉ����܂����B
	@version beta9	(2006/5/22)
		@li Dpw_Bt_Player::friend_key, Dpw_Bt_Leader::friend_key�̒ǉ��ɑΉ����܂����B
	@version beta4	(2006/4/17)
		@li DPW���C�u�������g�p����O�ɁAWiFi�R�l�N�V�����ւ̃��O�C�����s���A�����ؒf����悤�ɂ��܂����B
		@li �������擾����֐� DWC_GetDateTime() �̎g�p���ǉ����܂����B
	@version beta3	(2006/4/10)
		@li Dpw_Bt_Player, Dpw_Bt_Leader, Dpw_Bt_Room �̕ύX�ɑΉ����܂����B
	@version beta1	(2006/3/17)
		@li ���ł������[�X���܂����B
*/

#include <nitro.h>
#include <dwc.h>

#include <wchar.h>

#include "dbs.h"
#include "init.h"
#include "userdata.h"
#include "dpw_bt.h"

/*-----------------------------------------------------------------------*
					�^�E�萔�錾
 *-----------------------------------------------------------------------*/

#define GAME_PRODUCTID   0			// ���̃T���v�����g�p����v���_�N�gID
#define GAME_NAME        "gmtest"	// ���̃T���v�����g�p����Q�[����
#define GAME_SECRET_KEY  "HA6zkS"	// ���̃T���v�����g�p����V�[�N���b�g�L�[

#define GAME_FRAME		2		// �z�肷��Q�[���t���[���i1/60��1�Ƃ���j

#define BT_ROOM_RANK	1		// �A�N�Z�X���郉���N
#define MAIL_ADDR       "wfc-mail@nintendo.com" // ���[�����M��

typedef enum
{
	SAMPLE_START,
	SAMPLE_CONNECT_INET,
	SAMPLE_LOGIN,
	SAMPLE_LOGIN_WAIT,
	SAMPLE_BT_INIT,
    SAMPLE_BT_SETPROFILE,
    SAMPLE_BT_SETPROFILE_WAIT,
	SAMPLE_BT_GET_SERVER_STATE,
	SAMPLE_BT_GET_SERVER_STATE_WAIT,
	//SAMPLE_BT_INIT_SERVER,
	SAMPLE_BT_GET_ROOM_NUM,
	SAMPLE_BT_GET_ROOM_NUM_WAIT,
	SAMPLE_BT_UPLOAD_PLAYER_FOR_CANCEL,
	SAMPLE_BT_CANCEL,
	SAMPLE_BT_CANCEL_WAIT,
	SAMPLE_BT_UPLOAD_PLAYER,
	SAMPLE_BT_UPLOAD_PLAYER_WAIT,
	//SAMPLE_BT_UPDATE_SERVER,
	SAMPLE_BT_DOWNLOAD_ROOM,
	SAMPLE_BT_DOWNLOAD_ROOM_WAIT,
	SAMPLE_BT_END,
	SAMPLE_ERROR_OCCURED,
	SAMPLE_END
} DpwBtSampleState;

/*-----------------------------------------------------------------------*
					�֐��v���g�^�C�v�錾
 *-----------------------------------------------------------------------*/

//--- Auto Function Prototype --- Don't comment here.
void NitroMain();
static void GameWaitVBlankIntr(void);
static void VBlankIntr(void);
static void set_upload_player(void);
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

static DpwBtSampleState app_state;			// �A�v���P�[�V�����̃X�e�[�g
static s32 room_num;						// BT_ROOM_RANK�̕����̐�
static Dpw_Bt_Player upload_player;			// �A�b�v���[�h����v���C���[�f�[�^
static Dpw_Bt_Room download_room;			// �_�E�����[�h���郋�[���̃f�[�^
static Dpw_Common_Profile profile;          // �o�^����l���
static Dpw_Common_ProfileResult profile_result; // �l���o�^API�̌���

static DPW_SERVER_TYPE server_type = DPW_SERVER_DEBUG; // �A�N�Z�X����T�[�o

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
	
	// �A�b�v���[�h����v���C���[�̉��f�[�^��ݒ肷��B
	set_upload_player();

	//---- �\���J�n
	OS_WaitIrq(TRUE, OS_IE_V_BLANK);
	GX_DispOn();
	GXS_DispOn();

	while (1)
	{
		u16 keyData;

		{	// �J�E���g�A�b�v
			static u8 count;
			dbs_Print(0, 0, "DPW BT SAMPLE " __DATE__ "     %02x", count++);
		}

		GameWaitVBlankIntr();

		//---- �p�b�h�f�[�^�ǂݍ���
		keyData = PAD_Read();
		Trg = (u16) (keyData & (keyData ^ Cont));
		Cont = keyData;

		// Dpw_Bt_Main() �����͗�O�I�ɂ��ł��Ăׂ�
		Dpw_Bt_Main();

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
				
				app_state = SAMPLE_BT_INIT;
			}
			break;
		case SAMPLE_BT_INIT:
			// Dpw_Bt�̏��������s���B
			Dpw_Bt_Init(pid, DWC_CreateFriendKey(DTUD_GetUserData()), server_type);
			app_state = SAMPLE_BT_GET_SERVER_STATE;
			break;
		case SAMPLE_BT_GET_SERVER_STATE:
			// �T�[�o�[�̏�Ԃ��擾����
			Dpw_Bt_GetServerStateAsync();
			app_state = SAMPLE_BT_GET_SERVER_STATE_WAIT;
			break;
		case SAMPLE_BT_GET_SERVER_STATE_WAIT:
			if (Dpw_Bt_IsAsyncEnd())
			{
				s32 result = Dpw_Bt_GetAsyncResult();

				if (result < 0)
				{	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				}
				else
				{
					switch (result)
					{
					case DPW_BT_STATUS_SERVER_OK:		// ����ɓ��삵�Ă���
						OS_TPrintf(" server is up!\n");
						app_state = SAMPLE_BT_SETPROFILE;
						break;
					case DPW_BT_STATUS_SERVER_STOP_SERVICE:	// �T�[�r�X��~��
						OS_TPrintf(" server stop service.\n");
						app_state = SAMPLE_ERROR_OCCURED;
						break;
					case DPW_BT_STATUS_SERVER_FULL:		// �T�[�o�[�����t
						OS_TPrintf(" server full.\n");
						app_state = SAMPLE_ERROR_OCCURED;
						break;
					}
				}
			}
			break;
            /*
		case SAMPLE_BT_INIT_SERVER:
			// �T�[�o�[������������B
			// ���̍�Ƃ́A�{�ԃT�[�o�ł͔�펖�Ԃɂ����s���Ȃ��B
			// �e�X�g�T�[�o�ł̓f�o�b�O�p�̊֐���p���ĔC�ӂ̃^�C�~���O�ōs�����Ƃ��ł���B
			if (Dpw_Bt_Db_InitServer())
			{
				OS_TPrintf(" server init success.\n");
				app_state = SAMPLE_BT_GET_ROOM_NUM;
			}
			else
			{
				OS_TPrintf(" server update failed.\n");
				app_state = SAMPLE_ERROR_OCCURED;
			}
			break;
                */
		case SAMPLE_BT_SETPROFILE:
            memset(&profile, 0, sizeof(profile));
			profile.version = 10;
            profile.language = 8;
            profile.countryCode = 110;
            profile.localCode = 1;
            profile.playerId = 12345678;
        	profile.playerName[0] = 0x30AB;      // �J
        	profile.playerName[1] = 0x30B9;      // �X
        	profile.playerName[2] = 0x30DF;      // �~
        	profile.playerName[3] = 0xffff;    // �I�[
            //profile.playerName[0] = 0x5c;      // �J
        	//profile.playerName[1] = 0x6a;      // �X
        	//profile.playerName[2] = 0x90;      // �~
        	//profile.playerName[3] = 0xffff;    // �I�[
            profile.flag = 0;
            strcpy(profile.mailAddr, MAIL_ADDR);
            profile.mailAddrAuthPass = DPW_MAIL_ADDR_AUTH_DEBUG_PASSWORD;   // �f�o�b�O�p�p�X���[�h���g�p����B
            profile.mailAddrAuthVerification = 123;
            profile.mailRecvFlag = DPW_PROFILE_MAILRECVFLAG_EXCHANGE;
            
			Dpw_Bt_SetProfileAsync(&profile, &profile_result);
			
			app_state = SAMPLE_BT_SETPROFILE_WAIT;
			break;
		case SAMPLE_BT_SETPROFILE_WAIT:
			if (Dpw_Bt_IsAsyncEnd()) {
				s32 result = Dpw_Bt_GetAsyncResult();
				
				if (result < 0) {	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
                    
					app_state = SAMPLE_ERROR_OCCURED;
				} else {
                    OS_TPrintf(" setProfile result. code:%u, mailAddrAuthResult:%u\n", profile_result.code, profile_result.mailAddrAuthResult);
                    if(profile_result.code == DPW_PROFILE_RESULTCODE_SUCCESS && profile_result.mailAddrAuthResult == DPW_PROFILE_AUTHRESULT_SUCCESS)
                    {
                        // �f�o�b�O�p�p�X���[�h���g���Ă���̂ŕK����������͂�
    					app_state = SAMPLE_BT_GET_ROOM_NUM;
                    } else {
                        OS_TPrintf(" setProfile failed.");
					    app_state = SAMPLE_ERROR_OCCURED;
                    }
				}
			}
			break;
		case SAMPLE_BT_GET_ROOM_NUM:
			// �w�肵�������N�̃��[�������擾����
			Dpw_Bt_GetRoomNumAsync(BT_ROOM_RANK);
			app_state = SAMPLE_BT_GET_ROOM_NUM_WAIT;
			break;
		case SAMPLE_BT_GET_ROOM_NUM_WAIT:
			if (Dpw_Bt_IsAsyncEnd())
			{
				s32 result = Dpw_Bt_GetAsyncResult();

				if (result < 0)
				{	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				}
				else
				{
					room_num = result;		// ���[������ۑ�����
					OS_TPrintf(" room num = %d\n", room_num);
					app_state = SAMPLE_BT_UPLOAD_PLAYER_FOR_CANCEL;
				}
			}
			break;
		case SAMPLE_BT_UPLOAD_PLAYER_FOR_CANCEL:
			// �V�������v���C���[�̃f�[�^���A�b�v���[�h����B
			// �T�[�o�[���A�b�v�f�[�g�����Ƃ��ɂ��̕����̃��[�_�[�ɂȂ�͂��B
			// �ԍ��̈�ԑ傫�������փA�b�v���[�h����
			OS_TPrintf(" upload to rank = %d, room_num = %d.\n", BT_ROOM_RANK, room_num);
			Dpw_Bt_UploadPlayerAsync(BT_ROOM_RANK, room_num, 7, &upload_player, token, TOKEN_LEN);
			app_state = SAMPLE_BT_CANCEL;
			break;
		case SAMPLE_BT_CANCEL:
			// �v���C���[�̃A�b�v���[�h���L�����Z�����Ă݂�
			Dpw_Bt_CancelAsync();
			app_state = SAMPLE_BT_CANCEL_WAIT;
			break;
		case SAMPLE_BT_CANCEL_WAIT:
			if (Dpw_Bt_IsAsyncEnd())
			{
				s32 result = Dpw_Bt_GetAsyncResult();
				
				if (result == DPW_BT_ERROR_CANCEL)
				{	// �L�����Z���ł����Ƃ�
					OS_TPrintf(" cancel upload player success.\n");
					app_state = SAMPLE_BT_UPLOAD_PLAYER;
				}
				else if (result == 0)
				{
					// �L�����Z���ł����������Ԃ��Ă����ꍇ�́A�������I������
					// ���܂��Ă���B�A�b�v���[�h���I�����Ă�����̂Ƃ��đ��s�B
					OS_TPrintf(" cannot cancel, upload player. ");
					app_state = SAMPLE_BT_DOWNLOAD_ROOM;
				}
				else
				{	// �L�����Z���ȊO�̃G���[�̂Ƃ��B
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				}
			}
			break;
		case SAMPLE_BT_UPLOAD_PLAYER:
			// �V�������v���C���[�̃f�[�^���A�b�v���[�h����B
			// �T�[�o�[���A�b�v�f�[�g�����Ƃ��ɂ��̕����̃��[�_�[�ɂȂ�͂��B
			// �ԍ��̈�ԑ傫�������փA�b�v���[�h����
			OS_TPrintf(" upload to rank = %d, room_num = %d.\n", BT_ROOM_RANK, room_num);
			Dpw_Bt_UploadPlayerAsync(BT_ROOM_RANK, room_num, 7, &upload_player, token, TOKEN_LEN);
			app_state = SAMPLE_BT_UPLOAD_PLAYER_WAIT;
			break;
		case SAMPLE_BT_UPLOAD_PLAYER_WAIT:
			if (Dpw_Bt_IsAsyncEnd())
			{
				s32 result = Dpw_Bt_GetAsyncResult();

				if (result < 0)
				{	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				}
				else
				{
					OS_TPrintf(" upload player.\n");
					app_state = SAMPLE_BT_DOWNLOAD_ROOM;
				}
			}
			break;
            /*
		case SAMPLE_BT_UPDATE_SERVER:
			// �T�[�o�[���A�b�v�f�[�g����B
			// ���̍�Ƃ́A�{�ԃT�[�o�ł͂P���P�����I�ɍs���邪�A
			// �e�X�g�T�[�o�ł̓f�o�b�O�p�̊֐���p���ĔC�ӂ̃^�C�~���O�ŃA�b�v�f�[�g����B
			if (Dpw_Bt_Db_UpdateServer())
			{
				OS_TPrintf(" server update success.\n");
				app_state = SAMPLE_BT_DOWNLOAD_ROOM;
			}
			else
			{
				OS_TPrintf(" server update failed.\n");
				app_state = SAMPLE_ERROR_OCCURED;
			}
			break;
                */
		case SAMPLE_BT_DOWNLOAD_ROOM:
			// ���[�����_�E�����[�h����
			OS_TPrintf(" download from rank = %d, room_num = %d.\n", BT_ROOM_RANK, room_num);
			Dpw_Bt_DownloadRoomAsync(BT_ROOM_RANK, room_num, &download_room);
			app_state = SAMPLE_BT_DOWNLOAD_ROOM_WAIT;
			break;
		case SAMPLE_BT_DOWNLOAD_ROOM_WAIT:
			if (Dpw_Bt_IsAsyncEnd())
			{
				s32 result = Dpw_Bt_GetAsyncResult();

				if (result < 0)
				{	// �G���[�̂Ƃ�
					OS_TPrintf(" error %d", result);
					app_state = SAMPLE_ERROR_OCCURED;
				}
				else
				{
					OS_TPrintf(" download room.\n");
					{
						RTCDate date;
						RTCTime time;
						
						// ���Ԃ��擾����
						DWC_GetDateTime(&date, &time);
						
						OS_TPrintf(" current time is %02d/%02d/%02d %02d:%02d:%02d(GMT).\n",
								   date.year, date.month, date.day, time.hour, time.minute, time.second);
					}
					/*
					// �_�E�����[�h�����f�[�^���A�b�v���[�h�����f�[�^�Ɣ�r����B
					// �V���ŃA�b�v���[�h�����̂ŁA���[�_�[�ɂȂ��Ă���͂��B
					if (memcmp(&download_room.player[6], &upload_player, sizeof(Dpw_Bt_Player)) == 0)
					{
						OS_TPrintf(" download correct leader player data.\n");
					}
					else
					{
						OS_TPrintf(" download incorrect leader player data.\n");
					}
					
					// ���[�_�[�̗����f�[�^���X�V����Ă��邩�𒲂ׂ�B
					if (memcmp(&download_room.leader[29], &upload_player.playerName, sizeof(Dpw_Bt_Leader)) == 0)
					{
						OS_TPrintf(" download correct leader data.\n");
					}
					else
					{
						OS_TPrintf(" download incorrect leader data.\n");
					}
*/
					app_state = SAMPLE_BT_END;
				}
			}
			break;
		case SAMPLE_BT_END:
			// Dpw_Bt���C�u�������I������B
			Dpw_Bt_End();
			OS_TPrintf("DPW BT sample end.\n");
			app_state = SAMPLE_END;
			break;
		case SAMPLE_ERROR_OCCURED:
			OS_TPrintf("DPW BT error occured.\n");
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

static void set_upload_player(void) {
	
    // �R�C�L���O�A�|�b�`���}�Ƃ��̕��ʂ̃f�[�^
    u8 data[] = {129,0,0,0,150,0,0,0,0,0,0,0,54,12,238,45,34,213,25,43,245,97,234,21,0,0,0,0,0,0,0,1,33,70,100,0,85,0,94,0,161,0,97,0,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,
    			 137,1,0,0,250,0,54,0,65,0,56,0,236,18,227,64,163,190,169,16,220,8,120,10,0,0,0,0,0,0,0,1,67,71,142,0,116,0,114,0,148,0,143,0,255,255,0,0,0,0,0,0,0,0,255,255,0,0,0,0,
    			 14,0,0,0,106,0,0,0,0,0,0,0,0,0,0,0,57,148,138,197,0,0,0,0,0,0,0,0,0,0,0,1,61,70,100,0,96,0,241,0,161,0,255,255,0,0,0,0,0,0,0,0,0,0,255,255,0,0,0,0,
    			 25,0,78,0,241,0,255,255,0,0,0,0,0,0,0,0,10,1,0,0,176,238,83,172,1,0,4,0,83,4,255,255,0,24,0,0,0,0,89,4,255,255,1,0,0,0,115,4,255,255,2,0,0,0,123,5,255,255,215,3};
    memcpy(upload_player.pokemon, data, sizeof(data));
	upload_player.countryCode = 110;
    upload_player.langCode = 8;
    upload_player.ngname_f = 0;
	upload_player.playerName[0] = 0x30AB;      // �J
	upload_player.playerName[1] = 0x30B9;      // �X
	upload_player.playerName[2] = 0x30DF;      // �~
	upload_player.playerName[3] = 0xffff;    // �I�[
	upload_player.result = (u16) (7 * 1000 + 500);
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

