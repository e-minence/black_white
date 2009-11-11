#include <nitro.h>
#include <dwc.h>

#include "main.h"
#include "init.h"
#include "utility.h"
#include "font.h"
#include "screen.h"

#include "dbs.h"

#include "thread_statistics.h"

#include "gamemain.h"
#include "registerfriend.h"
#include "logon.h"
#include "transport.h"

#include "sound.h"
#include "phone.h"
#include "transceiver.h"
#include "conference.h"

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
#define NETCONFIG_USE_HEAP
#define GAME_FRAME       1         // �z�肷��Q�[���t���[���i1/60��1�Ƃ���j

//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
GameControl stGameCnt;  // �Q�[��������\���� 
KeyControl stKeyCnt;    // �L�[���͐���\���� 

// ���[�U�f�[�^���i�[����\���́B
DWCUserData stUserData;
DWCInetControl stConnCtrl;

// �f�o�b�O�o�͂̃I�[�o�[���C�h
static BOOL sPrintOverride;

DWCTopologyType s_topologyType = DWC_TOPOLOGY_TYPE_HYBRID;
static u8 stNumBackup = 0;  /* �o�b�N�A�b�v�f�o�C�X�̎�� */
static u16 card_lock_id;
extern u64  friend_key;

// Wi-Fi�R�l�N�V�����ݒ�̎g�p����
static int utilityLangage = DWC_LANGUAGE_JAPANESE;

//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static void InitGameData(void);

static GameMode StartIPMain(void);
static GameMode StopIPMain(void);

static void MatchModeDispCallback(int curIdx, void* param);

static void VBlankIntr(void);
static GameMode SetTopologyMain(void);
static const char* GetTopologyToString(void);
static char loopstr[] = "/|\\-";
static u32 loopcnt = 0;
static void DispErrorMessage(DWCError error, int errorCode, DWCErrorType errorType);
static GameMode BackupUserdataMain( void );
static void BackupModeDispCallback(int curIdx, void* param);
static GameMode NetConfigLanguageMain(void);
static GameMode NetConfigMain(void);
static const char *GetCardResultString(CARDResult result);
//----------------------------------------------------------------------------
// initialized variable
//----------------------------------------------------------------------------
// �Q�[���V�[�P���X���X�g
GameSequence gameSeqList[GAME_MODE_NUM] =
{
    {
        "GAME MAIN MODE",
        GameMain,
        NULL,
        6,
        {
            "Login", GAME_MODE_LOGIN, TRUE,
            "Register friend", GAME_MODE_REG_FRIEND, FALSE,
            "NetConfig", GAME_MODE_NETCONFIG_LANGUAGE, FALSE,
            "StartIP", GAME_MODE_STARTIP, FALSE,
            "StopIP", GAME_MODE_STOPIP, FALSE,
            "UserData Backup", GAME_MODE_BACKUP, FALSE,
        },
    },
    {
        "REGISTER FRIEND MODE",
        GameRegisterFriendMain,
        RegFriendModeDispCallback,
        4,
        {
            "Set(Manual)", GAME_MODE_NUM, FALSE,
            "Delete", GAME_MODE_NUM, FALSE,
            "Set(MP)", GAME_MODE_NUM, FALSE,
            "Return", GAME_MODE_MAIN, FALSE,
        },
    },
    {
        "GAME LOGON MODE",
        GameLogonMain,
        LogonModeDispCallback,
        7,
        {
            "Connect to anybody", GAME_MODE_CONNECTED, TRUE,
            "Connect to friends", GAME_MODE_CONNECTED, TRUE,
            "Setup game server", GAME_MODE_CONNECTED, TRUE,
            "Connect to game server", GAME_MODE_CONNECTED, TRUE,
            "GroupID reconnect", GAME_MODE_CONNECTED, TRUE,
            "Set Topology type", GAME_MODE_TOPOLOGY, FALSE,
            "Logout", GAME_MODE_MAIN, FALSE
        },
    },
    {
        "GAME CONNECTED MODE",
        GameConnectedMain,
        NULL,
        4,
        {
            "Start Phone",         GAME_MODE_PHONE, FALSE,
            "Start Transceiver",   GAME_MODE_TRANSCEIVER, FALSE,
            "Start Conference",    GAME_MODE_CONFER, FALSE,
            "Close Connections",   GAME_MODE_LOGIN, TRUE,
        },
    },
    {
        "LANGUAGE",
        NetConfigLanguageMain,
        NULL,
        8,
        {
            "Japanese", GAME_MODE_NETCONFIG, FALSE,
            "English", GAME_MODE_NETCONFIG, FALSE,
            "French", GAME_MODE_NETCONFIG, FALSE,
            "German", GAME_MODE_NETCONFIG, FALSE,
            "Italian", GAME_MODE_NETCONFIG, FALSE,
            "Spanish", GAME_MODE_NETCONFIG, FALSE,
            "Hangul", GAME_MODE_NETCONFIG, FALSE,
            "Return", GAME_MODE_MAIN, FALSE
        },
    },
    {
        "NET CONFIG GUI",
        NetConfigMain,        NULL,        0,
    },
    {
        "START TO CONNECT INTERNET",
        StartIPMain,        NULL,        0,
    },
    {
        "STOP TO CONNECT INTERNET",
        StopIPMain,        NULL,        0,
    },
    {
        "SET TOPOLOGY TYPE",
        SetTopologyMain,
        NULL,
        4,
        {
            "Hybrid Mode", GAME_MODE_NUM, FALSE,
            "Fullmesh Mode", GAME_MODE_NUM, FALSE,
            "Star Mode", GAME_MODE_NUM, FALSE,
            "return", GAME_MODE_LOGIN, FALSE,
        },
    },
    {
        "BACKUP USERDATA",
        BackupUserdataMain,
        BackupModeDispCallback,
        4,
        {
            "Load UserData", GAME_MODE_NUM, FALSE,
            "Save UserData", GAME_MODE_NUM, FALSE,
            "Report UserData", GAME_MODE_NUM, FALSE,
            "return", GAME_MODE_MAIN, FALSE,
        }
    },
    //
    // VoiceChat �ɂĒǉ�
    //
    {
        "Phone Mode",
        PhoneMain,
        NULL,
        9,
        {
            "", GAME_MODE_NUM, FALSE, 
            "", GAME_MODE_NUM, FALSE, 
            "", GAME_MODE_NUM, FALSE, 
            "", GAME_MODE_NUM, FALSE, 
            "", GAME_MODE_NUM, FALSE, 
            "", GAME_MODE_NUM, FALSE, 
            "", GAME_MODE_NUM, FALSE, 
            "", GAME_MODE_NUM, FALSE, 
            "Return", GAME_MODE_CONNECTED, FALSE,
        },
    },
    {
        "Transceiver Mode",
        TransceiverMain,
        TransceiverModeDispCallback,
        10,
        {
            "Set Server to aid:0", GAME_MODE_NUM, FALSE, 
            "Set Server to aid:1", GAME_MODE_NUM, FALSE, 
            "Set Server to aid:2", GAME_MODE_NUM, FALSE, 
            "Set Server to aid:3", GAME_MODE_NUM, FALSE, 
            "Set Server to aid:4", GAME_MODE_NUM, FALSE, 
            "Set Server to aid:5", GAME_MODE_NUM, FALSE, 
            "Set Server to aid:6", GAME_MODE_NUM, FALSE, 
            "Set Server to aid:7", GAME_MODE_NUM, FALSE, 
            "Startup Trans Server", GAME_MODE_NUM, FALSE, 
            "Return", GAME_MODE_CONNECTED, FALSE, 
        },
    },
    {
        "Conference Mode",
        ConferenceMain,
        ConferenceModeDispCallback,
        9,
        {
            "Add aid: 0", GAME_MODE_NUM, FALSE,
            "Add aid: 1", GAME_MODE_NUM, FALSE,
            "Add aid: 2", GAME_MODE_NUM, FALSE,
            "Add aid: 3", GAME_MODE_NUM, FALSE,
            "Add aid: 4", GAME_MODE_NUM, FALSE,
            "Add aid: 5", GAME_MODE_NUM, FALSE,
            "Add aid: 6", GAME_MODE_NUM, FALSE,
            "Add aid: 7", GAME_MODE_NUM, FALSE,
            "Return", GAME_MODE_CONNECTED, FALSE, 
        },
    },
};


// �T���v���ł̓Z�[�u�ł��Ȃ��̂ł�����Z�[�u�f�[�^�̑���Ƃ݂Ȃ�
GameSaveData saveData =
{
    {
        "Player1", "Player2", "Player3", "Player4"
    },
    0, FALSE,
    {
        0,
    }
};

static const struct
{
    CARDBackupType type;
    const char *comment;
} stBackupTypeTable[] = {
    { CARD_BACKUP_TYPE_EEPROM_4KBITS,   "EEPROM    4 kb" },
    { CARD_BACKUP_TYPE_EEPROM_64KBITS,  "EEPROM   64 kb" },
    { CARD_BACKUP_TYPE_EEPROM_512KBITS, "EEPROM  512 kb" },
    { CARD_BACKUP_TYPE_FLASH_2MBITS,    "FLASH     2 Mb" },
    { CARD_BACKUP_TYPE_FLASH_4MBITS,    "FLASH     4 Mb" },
} ;
enum { BACKUP_TABLE_MAX = sizeof(stBackupTypeTable) / sizeof(*stBackupTypeTable) } ;

/*---------------------------------------------------------------------------*
  �������m�ۊ֐�
 *---------------------------------------------------------------------------*/
static void*
AllocFunc( DWCAllocType name, u32   size, int align )
{
    void * ptr;
    OSIntrMode old;
    (void)name;
    (void)align;

    old = OS_DisableInterrupts();
    ptr = OS_AllocFromMain( size );
    OS_RestoreInterrupts( old );

    return ptr;
}

/*---------------------------------------------------------------------------*
  �������J���֐�
 *---------------------------------------------------------------------------*/
static void
FreeFunc( DWCAllocType name, void* ptr,  u32 size  )
{
    OSIntrMode old;
    (void)name;
    (void)size;

    if ( !ptr ) return;

    old = OS_DisableInterrupts();

    OS_FreeToMain( ptr );

    OS_RestoreInterrupts( old );
}

/** --------------------------------------------------------------------
  OS_TPrintf()�̃R���\�[���o�͑Ή�
  ----------------------------------------------------------------------*/
#ifdef SDK_FINALROM
#undef OS_TVPrintf
#undef OS_TPrintf
#endif
extern void OS_TVPrintf(const char *fmt, va_list vlist );
static char common_buffer[256];        // thread unsafe, but less use of stack

void OS_TVPrintf(const char *fmt, va_list vlist )
{
    (void)OS_VSNPrintf(common_buffer, sizeof(common_buffer), fmt, vlist);
#ifndef SDK_FINALROM
    OS_PutString(common_buffer);
#endif

    if ( sPrintOverride )
    {
        dbs_CVPrintf( NULL, fmt, vlist );
    }
}
void OS_TPrintf(const char *fmt, ...)
{
    va_list vlist;

    va_start(vlist, fmt);
    OS_TVPrintf(fmt, vlist);
    va_end(vlist);
}

char LoopChar()
{
    loopcnt++;
    return loopstr[((loopcnt)>>2)&3];
}
/*---------------------------------------------------------------------------*
  ���C�����[�`��
 *---------------------------------------------------------------------------*/
void NitroMain ( )
{
    int friendIdx = 0;
    int ret;
	
	OS_Init();
    CARD_Init();

    // �X�^�b�N���`�F�b�N�����ݒ�
    OS_SetThreadStackWarningOffset(OS_GetCurrentThread(), 0x100);

    Heap_Init();
	Alarm_Init();

    GX_Init();

    GX_DispOff();
    GXS_DispOff();

    
    // V�u�����N���荞�݋���
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);
	GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

    FS_Init( MI_DMA_MAX_NUM ); 

    // �o�͏�����
    InitScreen();
    dbs_DemoInit();
    sPrintOverride = TRUE; // OS_TPrintf()�̏o�͂�console�ɂȂ�. 
    
    // �\���J�n
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);              // Waiting the end of VBlank interrupt
    GX_DispOn();
    GXS_DispOn();

    // �L�[���͏�����
    InitKeyData(&stKeyCnt);

    InitGameData();

    // DWC���C�u����������
#if defined( USE_AUTHSERVER_PRODUCTION )
    ret = DWC_InitForProduction( GAME_NAME, INITIAL_CODE, AllocFunc, FreeFunc );
#else
    ret = DWC_InitForDevelopment( GAME_NAME, INITIAL_CODE, AllocFunc, FreeFunc );
#endif
    
    OS_TPrintf( "DWC_InitFor*() result = %d\n", ret );

    if ( ret == DWC_INIT_RESULT_DESTROY_OTHER_SETTING )
    {
        OS_TPrintf( "Wi-Fi setting might be broken.\n" );
    }

    // ���[�U�f�[�^�쐬
    DWC_CreateUserData( &stUserData );

    // ���[�U�f�[�^�\��
    DWC_ReportUserData( &stUserData );

    // �f�o�b�O�\�����x���w��
    DWC_SetReportLevel(DWC_REPORTFLAG_ALL);

    InitThreadStatistics();
        
    // ���C�����[�v
    while (1){
        // ���j���[�\��
        DispMenuMsgWithCursor(&gameSeqList[stGameCnt.mode], 0, NULL);

        // �V�[�P���X�ɑΉ����郋�[�v�֐����Ăяo��
        stGameCnt.mode = gameSeqList[stGameCnt.mode].seqFunc();
    }

	OS_Terminate();
}


/*---------------------------------------------------------------------------*
  �Q�[���f�[�^�ƃz�X�g��񏉊����֐�
 *---------------------------------------------------------------------------*/
static void InitGameData(void)
{

    // �Q�[������\���̏�����
    stGameCnt.mode     = GAME_MODE_MAIN;
    stGameCnt.blocking = FALSE;
    MI_CpuClear8(stGameCnt.friendStatus, sizeof(stGameCnt.friendStatus));

    // �Ƃ肠�����ǂ̖{�̂ł��C���f�b�N�X0�̃f�[�^���g���B
    stGameCnt.userData.playerIdx = 0;
    MI_CpuCopy32(saveData.playerData[stGameCnt.userData.playerIdx].playerName,
                 stGameCnt.userData.playerName, sizeof(stGameCnt.userData.playerName));
    stGameCnt.userData.profileID = saveData.profileID;
    stGameCnt.userData.isValidProfile = saveData.isValidProfile;
    MI_CpuCopy32(&saveData.friendList, &stGameCnt.friendList, sizeof(GameFriendList));
}


/*---------------------------------------------------------------------------*
  �l�b�g���[�N�ؒf�֐�
 *---------------------------------------------------------------------------*/
void ShutdownInet(void)
{
    // DWC���C�u�����I���BGameSpy�̃q�[�v�J��
    DWC_ShutdownFriendsMatch();
}

/*---------------------------------------------------------------------------*
  DWC���C�u�����G���[�����֐�
 *---------------------------------------------------------------------------*/
DWCError HandleDWCError(GameMode* gameMode)
{
    int errorCode;
    DWCError error;
    DWCErrorType errorType;

    // DWC�G���[�擾
    error = DWC_GetLastErrorEx(&errorCode, &errorType);

    // �G���[���Ȃ���΂����ɖ߂�
    if (error == DWC_ERROR_NONE) return DWC_ERROR_NONE;

    // �G���[�ɑΉ��������b�Z�[�W��\��
    DispErrorMessage(error, errorCode, errorType);

    DWC_ClearError();        // �G���[���N���A
    stGameCnt.blocking = 0;  // �u���b�L���O����

    switch (errorType){
    case DWC_ETYPE_LIGHT:
        // ���̂Ƃ���}�b�`���C�N���ɂ����o�Ȃ��G���[�^�C�v�Ȃ̂ŁA
        // �K�����O�C�����Ԃɖ߂�
        *gameMode = GAME_MODE_LOGIN;
        break;

    case DWC_ETYPE_SHUTDOWN_FM:
        // FriendsMatch�������I�����AGameSpy�̃q�[�v���������
        DWC_ShutdownFriendsMatch();

        // ���O�C���O�̏�Ԃɖ߂�
        *gameMode = GAME_MODE_MAIN;
        break;

    case DWC_ETYPE_DISCONNECT:
        // FriendsMatch�������I�����AGameSpy�̃q�[�v���������
        DWC_ShutdownFriendsMatch();

        // �C���^�[�l�b�g�ڑ������S�ɏ���������
        DWC_CleanupInet();

        // ���O�C���O�̏�Ԃɖ߂�
        *gameMode = GAME_MODE_MAIN;
        break;

    case DWC_ETYPE_FATAL:
        // Fatal Error �Ȃ̂Œ�~����
        OS_Panic("========== Stop process ==========\n");
        break;
    }

    return error;
}
/*---------------------------------------------------------------------------*
  �ʐM�G���[�\���֐�
 *---------------------------------------------------------------------------*/
static void DispErrorMessage(DWCError error, int errorCode, DWCErrorType errorType)
{
    if (error == DWC_ERROR_NONE) return;

    OS_TPrintf("ERROR: %d, code:%d, type:%d\n", error, -1*errorCode, errorType);

    switch (error){
    case DWC_ERROR_DS_MEMORY_ANY:
        OS_TPrintf("ERROR: DS_MEMORY_ANY.\n");
        break;
    case DWC_ERROR_AUTH_ANY:
        OS_TPrintf("ERROR: AUTH_ANY.\n");
        break;
    case DWC_ERROR_AUTH_OUT_OF_SERVICE:
        OS_TPrintf("ERROR: This game's network service has ended.\n");
        break;
    case DWC_ERROR_AUTH_STOP_SERVICE:
        OS_TPrintf("ERROR: Now server is temporarily unavailable.\nPlease re-login later.\n");
        break;
    case DWC_ERROR_AC_ANY:
        OS_TPrintf("ERROR: AC ANY.\n");
        break;
    case DWC_ERROR_NETWORK:
        OS_TPrintf("ERROR: Network error.\n");
        break;
    case DWC_ERROR_DISCONNECTED:
        OS_TPrintf("ERROR: Connection to AP was lost.\n");
        break;
    case DWC_ERROR_FATAL:
        OS_TPrintf("FATAL ERROR: Please turn the power off.\n");
        break;
    case DWC_ERROR_FRIENDS_SHORTAGE:
        OS_TPrintf("ERROR: Your friends seem not to login yet.\n");
        break;
    case DWC_ERROR_NOT_FRIEND_SERVER:
        OS_TPrintf("ERROR: He is not buddy or not game server.\n");
        break;
    case DWC_ERROR_SC_CONNECT_BLOCK:
        OS_TPrintf("ERROR: Game server is busy now.\n");
        break;
    case DWC_ERROR_SERVER_FULL:
        OS_TPrintf("ERROR: Game server is fully occupied.\n");
        break;
    case DWC_ERROR_NETWORK_LIGHT:
        OS_TPrintf("ERROR: Network light error.\n");
        break;
    default:
        OS_TPrintf("ERROR: Unexpected error(%d).\n", error);
        break;
    }

//    OS_TPrintf("error code = %d\n", -1*errorCode);
}


/*---------------------------------------------------------------------------*
  ���b�Z�[�W���X�g�\���i�J�[�\���t���j�֐�
 *---------------------------------------------------------------------------*/
void DispMenuMsgWithCursor(const GameSequence* gameSeq, int index, void* param)
{
    int i;

    OS_TPrintf("===============================\n");
    OS_TPrintf("%s\n", gameSeq->modeName);

    for (i = 0; i < gameSeq->numMenu; i++){
        if (i == index) OS_TPrintf("> ");
        else OS_TPrintf("  ");

        OS_TPrintf("%s\n", gameSeq->menuList[i].msg);
    }

    // ���[�h���Ƃ̒ǉ��\���p�R�[���o�b�N�֐����Ăяo��
    if (gameSeq->dispCallback) gameSeq->dispCallback(index, param);

    OS_TPrintf("===============================\n\n");
    OS_TPrintf("+++VALID CONSOLE %d +++\n", DWC_CheckValidConsole( &stUserData ) );
    Heap_Print();

}

/*---------------------------------------------------------------------------*
  �F�B���X�g�\���֐��iGameSpy Status������\���Łj
 *---------------------------------------------------------------------------*/
void DispFriendList(BOOL overLoad)
{
    u8   maxEntry = 0;
    u8   numEntry;
    char statusString[DWC_GS_STATUS_STRING_LEN];
    int  i;

    statusString[0] = '\0';

    for (i = 0; i < GAME_MAX_FRIEND_LIST; i++){
        if (!DWC_IsValidFriendData(&stGameCnt.friendList.keyList[i]))
            continue;
        
        if (overLoad){
            // �㏑���w��̏ꍇ�͗F�B�f�[�^�̍X�V���s��
            stGameCnt.friendStatus[i] =
                DWC_GetFriendStatusSC(&stGameCnt.friendList.keyList[i],
                                      &maxEntry, &numEntry, statusString);
        }

        if (maxEntry){
            // �F�B���T�[�o�N���C�A���g�^�̃T�[�o�̏ꍇ�́A
            // �i���ݐڑ��l���^�ő�ڑ��l���j��\������
            OS_TPrintf("Friend[%d] '%s' : profileID %u, status %d, server (%d/%d), statusString : %s\n",
                       i, stGameCnt.friendList.playerName[i],
                       DWC_GetGsProfileId(&stUserData, &stGameCnt.friendList.keyList[i]),
                       stGameCnt.friendStatus[i],
                       numEntry, maxEntry, statusString);
        }
        else {
            OS_TPrintf("Friend[%d] '%s' : profileID %u, status %d, statusString : %s\n",
                       i, stGameCnt.friendList.playerName[i],
                       DWC_GetGsProfileId(&stUserData, &stGameCnt.friendList.keyList[i]),
                       stGameCnt.friendStatus[i], statusString);
        }
    }
}

/*---------------------------------------------------------------------------*
  �F�B���X�g�\���֐��iGameSpy Status�f�[�^�\���Łj
 *---------------------------------------------------------------------------*/
void DispFriendListData(void)
{
    u8   maxEntry = 0;
    u8   numEntry;
    char statusData[DWC_GS_STATUS_STRING_LEN];
    int  size;
    int  i, j;

    for (i = 0; i < GAME_MAX_FRIEND_LIST; i++){
        if (!DWC_IsValidFriendData(&stGameCnt.friendList.keyList[i]))
            continue;
        
        // �F�B�f�[�^�̍X�V
        stGameCnt.friendStatus[i] =
                DWC_GetFriendStatusDataSC(&stGameCnt.friendList.keyList[i],
                                          &maxEntry, &numEntry, statusData, &size);
        
        if (maxEntry){
            // �F�B���T�[�o�N���C�A���g�^�̃T�[�o�̏ꍇ�́A
            // �i���ݐڑ��l���^�ő�ڑ��l���j��\������
            OS_TPrintf("Friend[%d] '%s' : profileID %u, status %d, server (%d/%d)\n",
                       i, stGameCnt.friendList.playerName[i],
                       DWC_GetGsProfileId(&stUserData, &stGameCnt.friendList.keyList[i]),
                       stGameCnt.friendStatus[i],
                       numEntry, maxEntry);
        }
        else {
            OS_TPrintf("Friend[%d] '%s' : profileID %u, status %d\n",
                       i, stGameCnt.friendList.playerName[i],
                       DWC_GetGsProfileId(&stUserData, &stGameCnt.friendList.keyList[i]),
                       stGameCnt.friendStatus[i]);
        }

        // GameSpy Status�������\������
        OS_TPrintf("StatusData : ");
        for (j = 0; j < size; j++){
            OS_TPrintf("%02d, ", statusData[j]);
        }
        OS_TPrintf("\n");
    }
}

/*---------------------------------------------------------------------------*
  �}�b�`���O���[�h�̒ǉ��\���p�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void MatchModeDispCallback(int curIdx, void* param)
{
#pragma unused(curIdx)
#pragma unused(param)
    
}

/*---------------------------------------------------------------------------*
  IP�̎擾
 *---------------------------------------------------------------------------*/
static GameMode StartIPMain(void)
{
	DWCApInfo apinfo;
	
    DWC_InitInet( &stConnCtrl );
    
    DWC_ConnectInetAsync();
    // ����ȃX�e�[�g�܂ő҂B
    while ( !DWC_CheckInet() )
    {
        DWC_ProcessInet();

        // V�u�����N�҂�����
        GameWaitVBlankIntr();
    }

    switch ( DWC_GetInetStatus() )
    {
    case DWC_CONNECTINET_STATE_ERROR:
        {
            // �G���[�\��
            DWCError err;
            int errcode;
            err = DWC_GetLastError( &errcode );

            OS_TPrintf("   Error occurred %d %d.\n", err, errcode );
        }
        DWC_ClearError();

        DWC_CleanupInet();
        break;
    case DWC_CONNECTINET_STATE_FATAL_ERROR:
        {
            // �G���[�\��
            DWCError err;
            int errcode;
            err = DWC_GetLastError( &errcode );

            OS_Panic("   Error occurred %d %d.\n", err, errcode );
        }
        break;

    case DWC_CONNECTINET_STATE_CONNECTED:
        OS_TPrintf("   CONNECTED!!!.\n");
		if(DWC_GetApInfo(&apinfo) == TRUE) {
			OS_TPrintf("DWCApInfo.aptype = %d\n", apinfo.aptype);
			OS_TPrintf("DWCApInfo.area   = %d\n", apinfo.area);
			OS_TPrintf("DWCApInfo.spotinfo = %s\n", apinfo.spotinfo);
			OS_TPrintf("DWCApInfo.essid = %s\n", apinfo.essid);
			OS_TPrintf("DWCApInfo.bssid = %02x:%02x:%02x:%02x:%02x:%02x\n", apinfo.bssid[0], apinfo.bssid[1], apinfo.bssid[2], apinfo.bssid[3], apinfo.bssid[4], apinfo.bssid[5]);
		}
		else {
			OS_TPrintf("Failed to get DWCApInfo\n");
		}
			
        break;
    }

    return GAME_MODE_MAIN;
}

static GameMode StopIPMain(void)
{
    DWC_CleanupInet();

    return GAME_MODE_MAIN;
}
/*---------------------------------------------------------------------------*
  Name:         GetCardResultString

  Description:  CARD �֐��̌��ʒl�𕶎���Ŏ擾

  Arguments:    result           CARDResult ���ʒl

  Returns:      ���ʒl��������镶����ւ̃|�C���^
 *---------------------------------------------------------------------------*/
static const char *GetCardResultString(CARDResult result)
{
    switch (result)
    {
    case CARD_RESULT_SUCCESS:
        return "success";
    case CARD_RESULT_FAILURE:
        return "failure";
    case CARD_RESULT_INVALID_PARAM:
        return "invalid param";
    case CARD_RESULT_UNSUPPORTED:
        return "unsupported";
    case CARD_RESULT_TIMEOUT:
        return "timeout";
    case CARD_RESULT_CANCELED:
        return "canceled";
    case CARD_RESULT_NO_RESPONSE:
        return "no response";
    case CARD_RESULT_ERROR:
        return "error";
    default:
        return "unknown error";
    }
}

/*---------------------------------------------------------------------------*
  ���[�U�[�f�[�^�o�b�N�A�b�v
 *---------------------------------------------------------------------------*/

static GameMode BackupUserdataMain( void )
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_BACKUP];
    int curIdx = 0;
    GameMode returnSeq = GAME_MODE_NUM;

    while (1){
        dbs_Print( 0, 0, "s:%d", DWC_GetMatchState() );
        dbs_Print( 7, 0, "n:%d", DWC_GetNumConnectionHost() );
        dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel() );
        dbs_Print( 10,1, "p:%d", stGameCnt.userData.profileID );
	    dbs_Print( 30, 0, "%c",  LoopChar());

        ReadKeyData();  // �L�[�f�[�^�擾

        HandleDWCError(&returnSeq);  // �G���[����

        // ���ɐi�ނׂ����[�h���Z�b�g����Ă����烋�[�v�𔲂���
        if (returnSeq != GAME_MODE_NUM) break;

        ////////// �ȉ��L�[���쏈��
        if (stKeyCnt.trg & PAD_BUTTON_A){
            // A�{�^���Ń��j���[����
            BOOL end = FALSE;
            BOOL error = FALSE;
            CARDResult last_result;
            CARD_LockBackup((u16)card_lock_id);
            end = CARD_IdentifyBackup(stBackupTypeTable[stNumBackup].type);
            if(!end)
            {
                error = TRUE;
                last_result = CARD_GetResultCode();
                OS_TPrintf("result:\"%s\"\n", GetCardResultString(last_result));
            }
            CARD_UnlockBackup(card_lock_id);


            switch (curIdx){
            case 0:  // Load
                if ( error ) {
                    OS_TPrintf("Error occur with %d\n", last_result);
                    continue;
                }
                CARD_LockBackup((u16)card_lock_id);
                if (CARD_IsBackupEeprom())
                {
                    CARD_ReadEeprom(0, &stUserData, sizeof(stUserData) );
					CARD_ReadEeprom(sizeof(stUserData), &stGameCnt.friendList, sizeof(GameFriendList));
                }
                else if (CARD_IsBackupFlash())
                {
                    CARD_ReadFlash(0, &stUserData, sizeof(stUserData) );
					CARD_ReadFlash(sizeof(stUserData), &stGameCnt.friendList, sizeof(GameFriendList));
                }
                CARD_UnlockBackup(card_lock_id);
                last_result = CARD_GetResultCode();

                if (last_result != CARD_RESULT_SUCCESS)
                {
                    OS_TPrintf("Error occur with %d\n", last_result);
                    continue;
                }
                else
                {
                    OS_TPrintf("Load OK!\n");
                    // ���[�h�ł�����t�����h�L�[���擾���Ă݂�
                    friend_key = DWC_CreateFriendKey( &stUserData );
                    returnSeq = GAME_MODE_MAIN;
                }
                break;
            case 1:  // Save
                if ( error ) {
                    OS_TPrintf("Error occur with %d\n", last_result);
                    continue;
                }
                CARD_LockBackup((u16)card_lock_id);
                if (CARD_IsBackupEeprom())
                {
                    CARD_WriteAndVerifyEeprom(0, &stUserData, sizeof(stUserData) );
					CARD_WriteAndVerifyEeprom(sizeof(stUserData), &stGameCnt.friendList, sizeof(GameFriendList));
                }
                else if (CARD_IsBackupFlash())
                {
                    CARD_WriteAndVerifyFlash(0, &stUserData, sizeof(stUserData) );
					CARD_WriteAndVerifyFlash(sizeof(stUserData), &stGameCnt.friendList, sizeof(GameFriendList));
                }
                CARD_UnlockBackup(card_lock_id);
                last_result = CARD_GetResultCode();

                if (last_result != CARD_RESULT_SUCCESS)
                {
                    OS_TPrintf("Error occur with %d\n", last_result);
                    continue;
                }
                else
                {
                    OS_TPrintf("Save OK!\n");
                    returnSeq = GAME_MODE_MAIN;
                }
                break;
            case 2: // report
                DWC_ReportUserData( &stUserData );
                break;
            case 3:
                returnSeq = GAME_MODE_MAIN;
                break;
            }
        }
        else if (stKeyCnt.trg & PAD_KEY_UP){
            // �\���L�[��ŃJ�[�\���ړ�
            curIdx--;
            if (curIdx < 0) curIdx = gameSeq->numMenu-1;
            // ���j���[���X�g�ĕ\��
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_DOWN){
            // �\���L�[���ŃJ�[�\���ړ�
            curIdx++;
            if (curIdx >= gameSeq->numMenu) curIdx = 0;
            // ���j���[���X�g�ĕ\��
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_LEFT){
            // �\���L�[���Ńo�b�N�A�b�v�f�o�C�X�̎�ޕύX
            stNumBackup--;
            stNumBackup %= BACKUP_TABLE_MAX;

            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_RIGHT){
            // �\���L�[�E�Ńo�b�N�A�b�v�f�o�C�X�̎�ޕύX
            stNumBackup++;
            stNumBackup %= BACKUP_TABLE_MAX;

            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        ////////// �L�[���쏈�������܂�

        // V�u�����N�҂�����
        GameWaitVBlankIntr();
    }

    return returnSeq;
}
/*---------------------------------------------------------------------------*
  Wi-Fi�R�l�N�V�����ݒ�̎g�p����I��
 *---------------------------------------------------------------------------*/
static GameMode NetConfigLanguageMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_NETCONFIG_LANGUAGE];
    int curIdx = 0;
    const int languageTable[] = {
        DWC_LANGUAGE_JAPANESE,
        DWC_LANGUAGE_ENGLISH,
        DWC_LANGUAGE_FRENCH,
        DWC_LANGUAGE_GERMAN,
        DWC_LANGUAGE_ITALIAN,
        DWC_LANGUAGE_SPANISH,
        DWC_LANGUAGE_HANGUL
    };

    while (1){
        dbs_Print( 0, 0, "s:%d", DWC_GetMatchState());
        dbs_Print( 7, 0, "n:%d", DWC_GetNumConnectionHost());
        dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel());
        dbs_Print(10, 1, "p:%d", stGameCnt.userData.profileID);
	    dbs_Print( 30, 0, "%c",  LoopChar());
        ReadKeyData();
        if (stKeyCnt.trg & PAD_BUTTON_A){
            utilityLangage = languageTable[curIdx];
            break;
        }
        else if (stKeyCnt.trg & PAD_KEY_UP){
            curIdx--;
            if (curIdx < 0) curIdx = gameSeq->numMenu - 1;
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_DOWN){
            curIdx++;
            if (curIdx >= gameSeq->numMenu) curIdx = 0;
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }

        // V�u�����N�҂�����
        GameWaitVBlankIntr();
    }

    return gameSeq->menuList[curIdx].mode;
}


/*---------------------------------------------------------------------------*
  Wi-Fi�R�l�N�V�����ݒ�
 *---------------------------------------------------------------------------*/
FS_EXTERN_OVERLAY(main_overlay_1);

static GameMode NetConfigMain(void)
{
    GameMode returnSeq = GAME_MODE_NETCONFIG_LANGUAGE;

    (void)FS_LoadOverlay( MI_PROCESSOR_ARM9, FS_OVERLAY_ID(main_overlay_1) ) ;

    DWC_CleanupInet(); // �l�b�g���[�N����ؒf����B
    sPrintOverride = FALSE; // OS_TPrintf()�̏o�͂��ꎞ�I�Ɍ��ɖ߂��B
    dbs_DemoFinalize();

#if defined( NETCONFIG_USE_HEAP )
    {
        void* work = OS_Alloc(DWC_UTILITY_WORK_SIZE);

        (void)DWC_StartUtility(
            work,
            utilityLangage,
            utilityLangage == DWC_LANGUAGE_JAPANESE ? DWC_UTILITY_TOP_MENU_FOR_JPN : DWC_UTILITY_TOP_MENU_COMMON
        );

        OS_Free(work);
    }
#else
    (void)DWC_StartUtilityEx(
        utilityLangage,
        utilityLangage == DWC_LANGUAGE_JAPANESE ? DWC_UTILITY_TOP_MENU_FOR_JPN : DWC_UTILITY_TOP_MENU_COMMON
    );
#endif

    (void)FS_UnloadOverlay( MI_PROCESSOR_ARM9, FS_OVERLAY_ID(main_overlay_1) ) ;

    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr( TRUE );

    dbs_DemoInit();
    sPrintOverride = TRUE; // OS_TPrintf()�̏o�͂�console�ɂȂ�.
    OS_WaitVBlankIntr();              // Waiting the end of VBlank interrupt
    GX_DispOn();
    GXS_DispOn();

    return returnSeq;
}

/*---------------------------------------------------------------------------*
  �ڑ��`�Ԑݒ�
 *---------------------------------------------------------------------------*/
static GameMode SetTopologyMain( void )
{
    GameSequence* gameSeq = &gameSeqList[GAME_MODE_TOPOLOGY];
    GameMode returnSeq = GAME_MODE_NUM;
    int curIdx = 0;
    
    while(1){
        ReadKeyData(); // �L�[�f�[�^�擾
//	    dbs_Print( 30, 0, "%c",  LoopChar());

        // ���ɐi�ނׂ����[�h���Z�b�g����Ă����甲����
        if(returnSeq != GAME_MODE_NUM) break;
        
        if(stKeyCnt.trg & PAD_BUTTON_A){
            // �ڑ��`�Ԍ���
            returnSeq = gameSeq->menuList[curIdx].mode;
            
            if(curIdx < 3){
                switch(curIdx){
                case 0: s_topologyType = DWC_TOPOLOGY_TYPE_HYBRID;   break;
                case 1: s_topologyType = DWC_TOPOLOGY_TYPE_FULLMESH; break;
                case 2: s_topologyType = DWC_TOPOLOGY_TYPE_STAR;     break;
                }
                OS_TPrintf("Set topology type[%s]!!\n\n", GetTopologyToString());
                DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
            }
        }
        else if(stKeyCnt.trg & PAD_KEY_UP){
            curIdx--;
            if(curIdx < 0) curIdx = gameSeq->numMenu - 1;
            // ���j���[���X�g�ĕ\��
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_DOWN){
            // �\���L�[���ŃJ�[�\���ړ�
            curIdx++;
            if (curIdx >= gameSeq->numMenu) curIdx = 0;
            // ���j���[���X�g�ĕ\��
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        
        // V�u�����N�҂�����
        GameWaitVBlankIntr();
    }
    
    return returnSeq;
}

/*---------------------------------------------------------------------------*
  �ڑ��`�Ԃ𕶎���Ŏ擾
 *---------------------------------------------------------------------------*/
static const char* GetTopologyToString(void)
{
    if(s_topologyType == DWC_TOPOLOGY_TYPE_HYBRID) return "Hybrid";
    else if(s_topologyType == DWC_TOPOLOGY_TYPE_FULLMESH) return "Fullmesh";
    else if(s_topologyType == DWC_TOPOLOGY_TYPE_STAR) return "Star";
    
    return "Unknown";  // ���Ȃ��Ǝv���̂���...
}

/*---------------------------------------------------------------------------*
  ���O�C���ヂ�[�h�̒ǉ��\���p�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void BackupModeDispCallback(int curIdx, void* param)
{
#pragma unused(param)
#pragma unused(curIdx)
    OS_TPrintf("\n BACKUP: %s\n", stBackupTypeTable[stNumBackup].comment);
}

/*---------------------------------------------------------------------------*
  �F�B���X�g�̋󂫃C���f�b�N�X�擾�֐�
 *---------------------------------------------------------------------------*/
int GetAvailableFriendListIndex(void)
{
    int i;

    for (i = 0; i < GAME_MAX_FRIEND_LIST; i++){
        if (!DWC_IsValidFriendData(&stGameCnt.friendList.keyList[i])){
            return i;
        }
    }

    return -1;
}

/*---------------------------------------------------------------------------*
  V�u�����N���荞�݊֐�
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    //---- upload pseudo screen to VRAM
    DC_FlushRange(gScreen, sizeof(gScreen));
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));
    
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  V�u�����N�҂��֐�
 *---------------------------------------------------------------------------*/
void GameWaitVBlankIntr(void)
{
    int i;

    dbs_DemoUpdate();

    // �z�肷��Q�[���t���[���ɍ��킹��V�u�����N�҂��̉񐔂�ς���
    for (i = 0; i < GAME_FRAME; i++){
        OS_WaitIrq(TRUE, OS_IE_V_BLANK);
    }

    Heap_Debug();
    dbs_DemoLoad();

    // �X�^�b�N���`�F�b�N
    OS_CheckStack(OS_GetCurrentThread());
}