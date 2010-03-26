/*---------------------------------------------------------------------------*
  Pokemon Network Library Test Client

  --����--
  [2010/03/17] Shinichiro Yamashita
  �E�����R�����g��V�K�ǉ�
  �Egds_header�C���ɔ����A�T���v�����C���E�X�V
  �Ecrc�l���v�Z����悤�ɏC��
 *---------------------------------------------------------------------------*/

#include <dwc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "main.h"
#include "init.h"
#include "utility.h"
#include "screen.h"

#include "dbs.h"
#include "crc.h"

// poke_net �֘A
#include "../../poke_net/poke_net_gds.h"	// POKE_NET���C�u�����̃w�b�_
#include "../../poke_net/poke_net_opt.h"	// �o�g���r�f�IID�̕ϊ����\�b�h���錾���Ă���܂��B

/** --------------------------------------------------------------------
  defines
  ----------------------------------------------------------------------*/
#define INITIAL_CODE    'NTRJ'     // ���̃T���v�����d�l����C�j�V�����R�[�h
#define GAME_NAME        "poke_net_test" // ���̃T���v�����g�p����Q�[����
#define GAME_FRAME       1

// �T�[�o��URL�܂���IP�A�h���X�ƃ|�[�g�ԍ����`
#define SYACHI_SERVER_URL	("pkgdstest.nintendo.co.jp")
#define SYACHI_SERVER_PORT	(12401)
//#define SYACHI_SERVER_URL	("192.168.0.133")
//#define SYACHI_SERVER_PORT	(23457)

// Auth�\���̂ɐݒ肷��l���`
#define	AUTH_PID				(2345678)	// PID
#define AUTH_ROMCODE			(12)		// ROM �R�[�h
#define AUTH_LANGCODE			(0)			// ����R�[�h
#define BATTLEID_AUTO_ENCODE

// ���̒l��PID�Ƃ��đ��M����ƁA�F�X�ȃ`�F�b�N�����������̂Œ��ӁI
#define TRAFFIC_CHECK_PID		(9999999)

// �o�g���r�f�I�o�[�W�����B�V�o�[�W�������o�Ȃ�����100�B
#define	BATTLEVIDEO_SERVER_VER		(100)

// �C�x���g�o�g���r�f�I��ID�֕ϊ�����B
#define TO_EVENT_BATTLEVIDEO_ID(x)	(x + 900000000000ULL)

// *
// * �G���[�R�[�h�̃��b�Z�[�W��`
// *
const char* sErrorMessageList[] = {
	"NONE",							// [ 0] POKE_NET_GDS_LASTERROR_NONE
	"NOTINITIALIZED",				// [ 1] POKE_NET_GDS_LASTERROR_NOTINITIALIZED
	"ILLEGALREQUEST",				// [ 2] POKE_NET_GDS_LASTERROR_ILLEGALREQUEST
	"CREATESOCKET",					// [ 3] POKE_NET_GDS_LASTERROR_CREATESOCKET
	"IOCTRLSOCKET",	                // [ 4] POKE_NET_GDS_LASTERROR_IOCTRLSOCKET
	"NETWORKSETTING",				// [ 5] POKE_NET_GDS_LASTERROR_NETWORKSETTING
	"CREATETHREAD",					// [ 6] POKE_NET_GDS_LASTERROR_CREATETHREAD
	"CONNECT",						// [ 7] POKE_NET_GDS_LASTERROR_CONNECT
	"SENDREQUEST",					// [ 8] POKE_NET_GDS_LASTERROR_SENDREQUEST
	"RECVRESPONSE",					// [ 9] POKE_NET_GDS_LASTERROR_RECVRESPONSE
	"CONNECTTIMEOUT",				// [10] POKE_NET_GDS_LASTERROR_CONNECTTIMEOUT
	"SENDTIMEOUT",					// [11] POKE_NET_GDS_LASTERROR_SENDTIMEOUT
	"RECVTIMEOUT",					// [12] POKE_NET_GDS_LASTERROR_RECVTIMEOUT
	"ABORTED"						// [13] POKE_NET_GDS_LASTERROR_ABORTED
};

/** --------------------------------------------------------------------
  globals
  ----------------------------------------------------------------------*/
KeyControl		g_KeyCtrl;

/** --------------------------------------------------------------------
  statics
  ----------------------------------------------------------------------*/
static u8 s_Work[ DWC_INIT_WORK_SIZE ] ATTRIBUTE_ALIGN( 32 );
static DWCInetControl			s_ConnCtrl;
static int  s_state  = state_init;
static POKE_NET_REQUESTCOMMON_AUTH sReqAuth;
#define BUFF_SIZE (32 * 1024)
static u8 sReqBuff[BUFF_SIZE] = "";
static u8 sResBuff[BUFF_SIZE] = "";
static DWCApInfoType sApInfoType;
const wchar_t g_TrainerName[]	= L"��܂҂Ȃ�";				// �v���t�B�[���ƃ~���[�W�J���f�[�^�Ɏg�p
const wchar_t g_TrainerName2[]  = L"�~�j�X�J�[�g����������";	// �o�g���{�̂̃g���[�i�[���Ɏg�p(���\�[�X�ł͖��g�p)
const wchar_t g_MusicalTitle[]	= L"abc�}�[�`���i���ȃ\�i�^";	// �~���[�W�J���^�C�g��

/** --------------------------------------------------------------------
  static functions
  ----------------------------------------------------------------------*/
static void initFunctions(void);
static void update( void );
static int update_select_connect(GameSequence* seq);
static int update_init(GameSequence*);
static int update_connect(GameSequence*);
static int update_select_pid(GameSequence*);
static int update_input_pid(GameSequence*);
static int update_gds_init(GameSequence*);
static int update_menu(GameSequence*);
static int update_gds_response(int seqNo);
// ���j���[�A�C�e��
static int update_debug_message(GameSequence*);
static int update_musical_update(GameSequence*);
static int update_musical_download(GameSequence*);
static int update_battle_upload(GameSequence*);
static int update_battle_search(GameSequence*);
static int update_battle_rank_search(GameSequence*);
static int update_battle_download(GameSequence*);
static int update_event_battle_download(GameSequence*);
static int update_event_battle_bookmark(GameSequence*);
static int update_error(GameSequence*);
static int update_query_disconnect(GameSequence*);
static int update_disconnect(GameSequence*);
// ����
static void initInput(InputElement* input, u32 min, u32 max, u32 first);
static int  processInput(GameSequence* seq, s16 x, s16 y);
static void initInputSequence(InputSequence* input);
static int processSceneSequence(GameSequence* input);
static s16 dispInput(InputElement* input, s16 x, s16 y);
static int getResultValue(GameSequence* seq, int no);
// ���[�e�B���e�B
static void nextGameSequence(int seqNo);
static BOOL processMenuSequence(GameSequence* seq, s16 x, s16 y);
static int  processSceneElement(GameSequence* seq, s16 x, s16 y);
static s16  DispTitle( const char* title, s16 x, s16 y);
static s16  DispSubTitle( const char* title, s16 x, s16 y);
static void GameWaitVBlankIntr(void);

// �v���t�B�[���쐬
void MakeProfileData(GDS_PROFILE* _pProfile);
// PMS�f�[�^�쐬
void MakePMSData(PMS_DATA* _pPMS);
// �~���[�W�J���V���b�g�쐬
void MakeMusicalshotData(MUSICAL_SHOT_DATA *_pMusicalData, int _MonsNoInc);
// �o�g���r�f�I�f�[�^�쐬
void MakeBattleVideoHeader(BATTLE_REC_HEADER *_bhead, BOOL bSubway, int _MonsNoInc);	// �w�b�_
void MakeBattleVideoData(BATTLE_REC_WORK *_brec, int _MonsNoInc);						// �{��
// �I�[����(0xFFFF)�������܂��B
// �o�b�t�@�I�[�o�[�t���[�ɒ��ӂ��Ă��������B
// (�K���o�b�t�@�̍Ō�̗v�f�ɂ�0������悤�ɂ��Ă��������B)
void SetGDS_EOM(STRCODE* _pStr, int _Size);

// ���j���[�A�C�e��
GameSequence sGameSeqs[] = {
    // select_connect
    {
        "SELECT AP",
        1,
        update_select_connect,
        {
            {   TYPE_SELECT, 4,
                {
                    { "Connection1",},
                    { "Connection2",},
                    { "Connection3",},
                    { "Any",},
                },
            },
        },
    },
    // init
    {
        "INIT",
        1,
        update_init,
        {
            { TYPE_NORMAL, 1, },
        },
    },
    // connect
    {
        "CONNECTING",
        1,
        update_connect,
        {
            { TYPE_NORMAL, 1, },
        },
    },
    // select_pid
    {
        "SELECT USE PID",
        1,
        update_select_pid,
        {
            {   TYPE_SELECT, 3,
                {
                    {"USE DEFAULT PID",},
                    {"INPUT PID",},
                    {"USE DEBUG PID",},
                },
            },
        },
    },
    // input pid
    {
        "INPUT PID",
        1,
        update_input_pid,
        {
            {   TYPE_INPUT, 1,
                {
                    {"INPUT PID",},
                },
            },
        },
    },
    // gds_init
    {
        "GDS INIT",
        1,
        update_gds_init,
        {
            { TYPE_NORMAL, 1, },
        },
    },
    // menu
    {
        "MENU",
        1,
        update_menu,
        {
            {   TYPE_SELECT, 9,
                {
                    {"test request",},
                    {"musical update",},
                    {"musical download",},
                    {"battle upload",},
                    {"battle search",},
                    {"battle rank search",},
                    {"battle download",},
                    {"event battle download",},
                    {"battle bookmark",},
                },
            },
        },
    },
    // debug_message
    {
        "DEBUG MESSAGE",
        1,
        update_debug_message,
        {
            { TYPE_NORMAL, 1, }, 
        },
    },
    // musical_update
    {
        "MUSICAL UPDATE",
        1,
        update_musical_update,
        {
            {   TYPE_INPUT, 1, 
                {
                    {"INPUT NO",},
                }
            },
        },
    },
    
    // musical_download
    {
        "MUSICAL DOWNLOAD",
        1,
        update_musical_download,
        {
            {   TYPE_INPUT, 1,
                {
                    {"INPUT NO",},
                },
            },
        },
    },
    // battle_upload
    {
        "BATTLE UPLOAD",
        2,
        update_battle_upload,
        {
            {   TYPE_INPUT, 1,
                {
                    {"INPUT NO",},
                },
            },
            {   TYPE_SELECT, 2,
                {
                    {"BATTLE SUBWAY",},
                    {"OTHER",},
                },
            },
        },
    },
    // battle_search
    {
        "BATTLE SEARCH",
        2,
        update_battle_search,
        {
            {   TYPE_INPUT, 1,
                {
                    {"INPUT NO",},
                },
            },
            {
                TYPE_SELECT, 3,
                {
                    {"NO TARGET"},
                    {"BATTLE SUBWAY",},
                    {"REMOTE BATTLE",},
                },
            },
        },
    },
    // battle_rank_search
    {
        "BATTLE RANK SEARCH",
        1,
        update_battle_rank_search,
        {
            {   TYPE_SELECT, 3,
                {
                    {"LATEST 30",},
                    {"SUBWAY",},
                    {"COMM",},
                },
            },
        },
    },
    // battle_download
    {
        "BATTLE DOWNLOAD",
        1,
        update_battle_download,
        {
            {   TYPE_INPUT, 1,
                {
                    {"INPUT NO",},
                },
            },
        },
    },
    // event_battle_download
    {
        "EVENT BATTLE DOWNLOAD",
        1,
        update_event_battle_download,
        {
            {   TYPE_INPUT, 1,
                {
                    {"INPUT NO",},
                },
            },
        },
    },
    // event_battle_bookmark
    {
        "EVENT BATTLE BOOKMARK",
        1,
        update_event_battle_bookmark,
        {
            {   TYPE_INPUT, 1,
                {
                    {"INPUT NO",},
                },
            },
        },
    },
    // error
    {
        "ERROR",
        1,
        update_error,
        {
            { TYPE_NORMAL, 1, },
        },
    },
    // query disconnect
    {
        "DISCONNECT?",
        1,
        update_query_disconnect,
        {
            {   TYPE_SELECT, 2,
                {
                    {"YES",},
                    {"NO",},
                },
            },
        },
    },
    // disconnect
    {
        "DISCONNECT",
        1,
        update_disconnect,
        {
            { TYPE_NORMAL, 1, },
        },
    },
};
GameSequence* sCurSeq = NULL;

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

/** --------------------------------------------------------------------
  
  ----------------------------------------------------------------------*/
/** --------------------------------------------------------------------
  �ڑ��� AP �I��
  ----------------------------------------------------------------------*/
static int update_select_connect(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
        int ret = getResultValue(seq, 0);

        switch (ret) {
            case 0:  sApInfoType = DWC_APINFO_TYPE_USER0; break;    
            case 1:  sApInfoType = DWC_APINFO_TYPE_USER1; break;    
            case 2:  sApInfoType = DWC_APINFO_TYPE_USER2; break;    
            default: sApInfoType = DWC_APINFO_TYPE_FREESPOT; break;    
        }

        // ���łɏ�����
        DWC_InitInet( &s_ConnCtrl );
        
        //DWC_SetAuthServer( DWC_CONNECTINET_AUTH_RELEASE );
        // �ڑ����ݒ�
        if (sApInfoType != DWC_APINFO_TYPE_FREESPOT) {
            DWC_SetConnectApType(sApInfoType);
        }

        DWC_ConnectInetAsync();

        return state_connect;
    }

    return state_select_connect;
}

/** --------------------------------------------------------------------
  �������
  ----------------------------------------------------------------------*/
static int update_init(GameSequence* seq)
{
#pragma unused(seq)
    DWC_InitInet( &s_ConnCtrl );
        
    //DWC_SetAuthServer( DWC_CONNECTINET_AUTH_RELEASE );
    // �ڑ����ݒ�
    DWC_SetConnectApType(DWC_APINFO_TYPE_USER0);
        
    DWC_ConnectInetAsync();

    return state_connect;
}

/** --------------------------------------------------------------------
  �ڑ���
  ----------------------------------------------------------------------*/
static int update_connect(GameSequence* seq)
{
#pragma unused(seq)
    DWC_ProcessInet();

    if ( DWC_CheckInet() )
    {
        int status;

        status = DWC_GetLastError(NULL);

        if ( status == DWC_ERROR_NONE )
        {
        	switch ( DWC_GetInetStatus() )
	        {
		        case DWC_CONNECTINET_STATE_ERROR:{
			    // �G���[�\��
			    DWCError err;
			    int errcode;

			    err = DWC_GetLastError( &errcode );
			    OS_TPrintf("   Error occurred %d %d.\n", err, errcode );

			    DWC_ClearError();
			    DWC_CleanupInet();
		    }
            break;
		    case DWC_CONNECTINET_STATE_FATAL_ERROR:{
			    // �G���[�\��
			    DWCError err;
			    int errcode;
			    err = DWC_GetLastError( &errcode );

			    OS_Panic("   Error occurred %d %d.\n", err, errcode );
		    }
            break;

		    case DWC_CONNECTINET_STATE_CONNECTED:
			    OS_TPrintf("   CONNECTED!!!.\n");
			    return state_select_pid;
	        }
        }
    }

    return state_connect;
}

/** --------------------------------------------------------------------
  PID �g�p���@�I��
  ---------------------------------------------------------------------*/
static int update_select_pid(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
        int no = getResultValue(seq, 0);
        int next = state_gds_init;
        if (no == 0) sReqAuth.PID = AUTH_PID;
        else if( no == 2) sReqAuth.PID = TRAFFIC_CHECK_PID;
        else {
            next = state_input_pid;
        }
        return next;
    }

    return state_select_pid;
}

/** --------------------------------------------------------------------
  PID ����
  ---------------------------------------------------------------------*/
static int update_input_pid(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
        sReqAuth.PID = getResultValue(seq, 0);
        return state_gds_init;
    }
    return state_select_pid;
}

/** --------------------------------------------------------------------
  GDS ���C�u����������
  ---------------------------------------------------------------------*/
static int update_gds_init(GameSequence* seq)
{
#pragma unused(seq)
    OS_Printf("GDS ���C�u������������ ... ");

	sReqAuth.ROMCode	= AUTH_ROMCODE;
	sReqAuth.LangCode	= AUTH_LANGCODE;

    if (! POKE_NET_GDS_Initialize(&sReqAuth, SYACHI_SERVER_URL, SYACHI_SERVER_PORT)) {
        OS_Printf("GDS ���C�u�����̏������Ɏ��s !\n");
        OS_Halt();
    }

    OS_Printf("GDS ���C�u��������������I��!\n");

    return state_menu;
}

/** --------------------------------------------------------------------
  ���j���[
  ----------------------------------------------------------------------*/
static int update_menu(GameSequence* seq)
{
    if (seq->result == INPUT_CANCEL) return state_query_disconnect;

    return (getResultValue(seq, 0) + state_menu + 1); 
}

/** --------------------------------------------------------------------
  ���̃��j���[
  ----------------------------------------------------------------------*/
static void nextGameSequence(int seqNo)
{
    sCurSeq = &sGameSeqs[seqNo];
}

/** --------------------------------------------------------------------
  ���j���[����
  ----------------------------------------------------------------------*/
static BOOL processMenuSequence(GameSequence* seq, s16 x, s16 y)
{
    InputSequence* input = &seq->inputList[seq->currentElement];
    
    PrintString( x,y++, 0xf, "=============================");
    {
        int i;
        for (i = 0; i < input->numElements; ++i) {
            if (input->currentElement == i) PrintString(x, y, 0xf, ">");
            else              PrintString(x, y, 0xf, " ");
            PrintString((s16)(x+1), y++, 0xf, "%s", input->elements[i].head);
        }
    }
    PrintString( x,y++, 0xf, "=============================");

    // �L�[����
    if (g_KeyCtrl.trg & PAD_KEY_DOWN) {
        if (input->currentElement == (input->numElements - 1)) {
            input->currentElement = 0;
        } else {
            ++input->currentElement;
        }
    }
    else if (g_KeyCtrl.trg & PAD_KEY_UP) {
        if (input->currentElement == 0) {
            input->currentElement = input->numElements - 1;
        } else {
            --input->currentElement;
        }
    }
    if (g_KeyCtrl.trg & PAD_BUTTON_A) {
        input->result =input->currentElement;
        return INPUT_CONFIRM; 
    }
    else if (g_KeyCtrl.trg & PAD_BUTTON_B) {
        return INPUT_CANCEL; 
    }

    return INPUT_NOW;
}

/** --------------------------------------------------------------------
  �f�o�b�O���b�Z�[�W
  ----------------------------------------------------------------------*/
static int update_debug_message(GameSequence* seq)
{
#pragma unused(seq)
    char str[] = "TestMessage for GDS";
    
    OS_TPrintf("test message send to GDS... > %s\n", str);

    POKE_NET_GDS_DebugMessageRequest(str, sResBuff);

    OS_TPrintf("test message send done.\n");

    return update_gds_response(state_menu);
}

/** --------------------------------------------------------------------
  �~���[�W�J���V���b�g�A�b�v���[�h
  ----------------------------------------------------------------------*/
static int update_musical_update(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
        GDS_PROFILE profile = {0};
        int no = getResultValue(seq, 0);
        MakeProfileData(&profile);

        MI_CpuClear8(sReqBuff, sizeof(sReqBuff));
        MI_CpuClear8(sResBuff, sizeof(sResBuff));

        // �v���t�B�[�����R�s�[
	    memcpy(&((MUSICAL_SHOT_SEND*)sReqBuff)->profile, &profile, sizeof(GDS_PROFILE));

	    // �~���[�W�J���V���b�g�f�[�^���쐬
	    MakeMusicalshotData(&((MUSICAL_SHOT_SEND*)sReqBuff)->mus_shot, (int)no);

	    // �ʐM
	    POKE_NET_GDS_MusicalShotRegist((MUSICAL_SHOT_SEND*)sReqBuff ,sResBuff);

        return update_gds_response(state_menu);
    }

    return state_menu;
}

/** --------------------------------------------------------------------
  �~���[�W�J���V���b�g�_�E�����[�h
  ----------------------------------------------------------------------*/
static int update_musical_download(GameSequence* seq)
{
    // ���͏���
    if (seq->result == INPUT_CONFIRM) {
        int no = getResultValue(seq, 0);
        GDS_PROFILE profile = {0};
        MakeProfileData(&profile);
        
        MI_CpuClear8(sReqBuff, sizeof(sReqBuff));
        MI_CpuClear8(sResBuff, sizeof(sResBuff));
        
        POKE_NET_GDS_MusicalShotGet(no, sResBuff); 
        return update_gds_response(state_menu);
    }

    return state_menu;
}

/** --------------------------------------------------------------------
  �o�g���r�f�I�A�b�v���[�h
  ----------------------------------------------------------------------*/
static int update_battle_upload(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
        int no   = getResultValue(seq, 0),
            type = getResultValue(seq, 1);
        GDS_PROFILE profile = {0};
        MakeProfileData(&profile);

        MI_CpuClear8(sReqBuff, sizeof(sReqBuff));
        MI_CpuClear8(sResBuff, sizeof(sResBuff));

        // �v���t�B�[�����R�s�[
	    memcpy(&((BATTLE_REC_SEND*)sReqBuff)->profile, &profile, sizeof(GDS_PROFILE));

	    // �o�g���r�f�I�w�b�_���쐬
	    MakeBattleVideoHeader(&((BATTLE_REC_SEND*)sReqBuff)->head, (type == 0), no);

	    // �o�g���r�f�I�{�̂��쐬
	    MakeBattleVideoData(&((BATTLE_REC_SEND*)sReqBuff)->rec, 1);

	    // �ʐM
	    POKE_NET_GDS_BattleDataRegist((BATTLE_REC_SEND*)sReqBuff ,sResBuff);

        return update_gds_response(state_menu);
    }

    return state_menu;
}

/** --------------------------------------------------------------------
  �o�g���r�f�I���킵������
  ----------------------------------------------------------------------*/
static int update_battle_search(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
    	int battleMode;

	    // ���������p�̍\����
	    BATTLE_REC_SEARCH_SEND Serch = {0};

        memset(sReqBuff ,0x00 ,sizeof(sReqBuff));
	    memset(sResBuff ,0x00 ,sizeof(sResBuff));

        Serch.monsno = (u16)getResultValue(seq, 0);

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
        // �ȉ���ς���ƌ��ʂ��ς��܂�
        battleMode = getResultValue(seq, 1); 
	    if(battleMode == 0)
	    {
		    Serch.battle_mode = 0;
		    Serch.battle_mode_bit_mask = 0;
	    }
	    else if(battleMode == 1)
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
	    POKE_NET_GDS_BattleDataSearchCondition(&Serch, sResBuff);

        return update_gds_response(state_menu);
    }

    return state_menu;
}

/** --------------------------------------------------------------------
  �o�g���r�f�I�����L���O���� 
  ----------------------------------------------------------------------*/
static int update_battle_rank_search(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
        u64 no = getResultValue(seq, 0);

	    // ���������p�̍\����
	    BATTLE_REC_RANKING_SEARCH_SEND Serch = {0};

	    memset(sReqBuff ,0x00 ,sizeof(sReqBuff));
	    memset(sResBuff ,0x00 ,sizeof(sResBuff));

	    if(no == 0)
	    {
		    // �ŐV30��
		    POKE_NET_GDS_BattleDataSearchRankingLatest(BATTLEVIDEO_SERVER_VER, sResBuff);
	    }
	    else if(no == 1)
	    {
		    // �o�g���T�u�E�F�C�����L���O
		    POKE_NET_GDS_BattleDataSearchRankingSubway(BATTLEVIDEO_SERVER_VER, sResBuff);
	    }
	    else
	    {
		    // �ʐM�ΐ탉���L���O
		    POKE_NET_GDS_BattleDataSearchRankingComm(BATTLEVIDEO_SERVER_VER, sResBuff);
    	}

        return update_gds_response(state_menu);
    }

    return state_menu;
}

/** --------------------------------------------------------------------
  �o�g���r�f�I�_�E�����[�h
  ----------------------------------------------------------------------*/
static int update_battle_download(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
        u64 no = getResultValue(seq, 0);

        memset(sReqBuff ,0x00 ,sizeof(sReqBuff));
	    memset(sResBuff ,0x00 ,sizeof(sResBuff));

#ifdef BATTLEID_AUTO_ENCODE
    	// �Í���
	    no = POKE_NET_EncodePlayDataID(no);
#endif

	    // �ʐM
	    POKE_NET_GDS_BattleDataGet(no, BATTLEVIDEO_SERVER_VER, sResBuff);

        return update_gds_response(state_menu);
    }

    return state_menu;
}

/** --------------------------------------------------------------------
  �C�x���g�o�g���r�f�I�_�E�����[�h
  ----------------------------------------------------------------------*/
static int update_event_battle_download(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
        u64 no = getResultValue(seq, 0);

    	memset(sReqBuff ,0x00 ,sizeof(sReqBuff));
	    memset(sResBuff ,0x00 ,sizeof(sResBuff));

	    // �C�x���g�r�f�I�p��ID�ϊ�
	    no = TO_EVENT_BATTLEVIDEO_ID(no);

#ifdef BATTLEID_AUTO_ENCODE
	    // �Í���
	    no = POKE_NET_EncodePlayDataID(no);
#endif
	    // �ʐM
	    POKE_NET_GDS_BattleDataGet(no, BATTLEVIDEO_SERVER_VER, sResBuff);

        return update_gds_response(state_menu);
    }

    return state_menu;
}

/** --------------------------------------------------------------------
  �o�g���r�f�I���C�ɓ���o�^
  ----------------------------------------------------------------------*/
static int update_event_battle_bookmark(GameSequence* seq)
{
    if (seq->result == INPUT_CONFIRM) {
        u64 no = getResultValue(seq, 0);

        memset(sReqBuff ,0x00 ,sizeof(sReqBuff));
	    memset(sResBuff ,0x00 ,sizeof(sResBuff));

#ifdef BATTLEID_AUTO_ENCODE
	    // �Í���
	    no = POKE_NET_EncodePlayDataID(no);
#endif

	    // �ʐM
	    POKE_NET_GDS_BattleDataFavorite((u64)no, sResBuff);

        return update_gds_response(state_menu);
    }

    return state_menu;
}

/** --------------------------------------------------------------------
  �G���[�����҂�
  ----------------------------------------------------------------------*/
static int update_error(GameSequence* seq )
{
#pragma unused(seq)
    int errcode;
    DWCError err;
    DWCErrorType errtype;
    
    if ( g_KeyCtrl.trg & PAD_BUTTON_START )
    {
        // �X�^�[�g�{�^���ŃG���[����
        err = DWC_GetLastErrorEx(&errcode, &errtype);

        DWC_ClearError();
        
        // �{�T���v���ł̓��O�C���₻�̑��}�b�`���C�N���͍s��Ȃ��̂�
        // �ʐM����ؒf���邾��
        return state_disconnect;
    }

    return state_error;
}

/** --------------------------------------------------------------------
  �ؒf�₢���킹
  ----------------------------------------------------------------------*/
static int update_query_disconnect(GameSequence* seq )
{
    if (seq->result == INPUT_CANCEL || getResultValue(seq, 0) == 1) 
        return state_menu;

    return state_disconnect;
}

/** --------------------------------------------------------------------
  �ؒf
  ----------------------------------------------------------------------*/
static int update_disconnect(GameSequence* seq )
{
#pragma unused(seq)
    if ( DWC_CleanupInetAsync() )
    {
        return state_select_connect;
    }

    return state_disconnect;
}

/** --------------------------------------------------------------------
  GDS ���X�|���X����
  ----------------------------------------------------------------------*/
static int update_gds_response(int seqNo)
{
	// �o�̓��b�Z�[�W
	const char* messageList[] = {
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

    int lastState = POKE_NET_GDS_GetStatus();
    BOOL loop = TRUE;
    while (loop) {
        int state = POKE_NET_GDS_GetStatus();
        if (lastState == state) {
            OS_Sleep(10);
            continue;
        }

        OS_TPrintf("state changed.\n");
        OS_TPrintf("status: %s\n", messageList[state]);
        
        if (state == POKE_NET_GDS_STATUS_INACTIVE ||
            state == POKE_NET_GDS_STATUS_INITIALIZED ||
            state == POKE_NET_GDS_STATUS_ABORTED ||
            state == POKE_NET_GDS_STATUS_FINISHED ||
            state == POKE_NET_GDS_STATUS_ERROR )
        {
            loop = FALSE;
        }

        // �ʐM����
        if (state == POKE_NET_GDS_STATUS_FINISHED) {
            POKE_NET_GDS_DEBUG_PrintResponse((POKE_NET_RESPONSE*)POKE_NET_GDS_GetResponse(), POKE_NET_GDS_GetResponseSize());
        }
        else if (state == POKE_NET_GDS_STATUS_ERROR) {
            OS_TPrintf("GDS ERROR OCCURED!!!\n");
            OS_TPrintf("errorCode:%d\n", POKE_NET_GDS_GetLastErrorCode());
            OS_TPrintf("lastErrorMsg:%s\n", sErrorMessageList[POKE_NET_GDS_GetLastErrorCode()]);
        }
        
        lastState = state;
        
        GameWaitVBlankIntr();
    }
    return seqNo;
}

/** -------------------------------------------------------------------------
  ---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * �^�C�g���\��
 *---------------------------------------------------------------------------*/
static s16 DispTitle( const char* title, s16 x, s16 y)
{
    PrintString( x,y++, 0xf, "=============================");
    PrintString((s16)(x+1), y++, 0xf, "%s", title);
    PrintString( x,y++, 0xf, "=============================");

    return y;
}

/*---------------------------------------------------------------------------*
 * �T�u�^�C�g���\��
 *---------------------------------------------------------------------------*/
static s16 DispSubTitle( const char* title, s16 x, s16 y)
{
    PrintString( x,y++, 0xf, "-----------------");
    PrintString((s16)(x+1), y++, 0xf, "%s", title);
    PrintString( x,y++, 0xf, "-----------------");

    return y;
}

/** --------------------------------------------------------------------
  ���t���[���Ăяo���A��ԕ\���֐�
  ----------------------------------------------------------------------*/
void
drawConsole( void )
{
    static int i = 0;
    char loop[] = "/|\\-";

    dbs_Print( 30,0, "%c", loop[((i)>>2)&3] );

    i++;
}

/** --------------------------------------------------------------------
  �G���g���|�C���g
  ----------------------------------------------------------------------*/
void NitroMain ()
{
    initFunctions();

    // �f�o�b�O�\�����x���w��
    DWC_SetReportLevel((u32)( DWC_REPORTFLAG_ALL &
                              ~DWC_REPORTFLAG_QR2_REQ ));

    DWC_InitForDevelopment( GAME_NAME, INITIAL_CODE, DTDB_GetAllocFunc(), DTDB_GetFreeFunc() );

    // �ŏ��̏����̐ݒ�
    nextGameSequence(state_select_connect);

    while (1)
    {
        drawConsole();

        ReadKeyData();

        update();

        dbs_DemoUpdate();
        OS_WaitIrq(TRUE, OS_IE_V_BLANK);
        Heap_Debug();
        dbs_DemoLoad();
    }

    OS_Terminate();
}

/** --------------------------------------------------------------------
  ����������
  ----------------------------------------------------------------------*/
static void
initFunctions( void )
{
	OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    RTC_Init();
    CARD_Init();
    OS_SetThreadStackWarningOffset(OS_GetCurrentThread(), 0x100);
    PXI_Init();
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

    Heap_Init();
    Heap_SetDebug( TRUE );

    // �o�͏�����
    InitScreen();
    dbs_DemoInit();
    DTDB_SetPrintOverride( TRUE );

    // �L�[���͏�����
    InitKeyData( &g_KeyCtrl );

    // �\���J�n
    OS_WaitVBlankIntr();              // Waiting the end of VBlank interrupt
    GX_DispOn();
    GXS_DispOn();
}

/** --------------------------------------------------------------------
  1�t���[��1��Ă΂��֐�
  ----------------------------------------------------------------------*/
static void update( void )
{
    int errcode;
    DWCError err;
    DWCErrorType errtype;
    
    sCurSeq->result = processSceneSequence(sCurSeq);
    nextGameSequence(sCurSeq->fpResult(sCurSeq));

    err = DWC_GetLastErrorEx(&errcode, &errtype);

    if ( err != DWC_ERROR_NONE && s_state != state_error )
    {
        // �G���[�����������ꍇ�A�����ł͏�Ԃ��G���[�����҂��ɂ��邾��
        OS_TPrintf( "\n*******************************\n" );
        OS_TPrintf( "ERROR : %d : %d\n", err, errcode );
        OS_TPrintf( "*******************************\n\n" );
        DWC_ClearError();
        nextGameSequence(state_disconnect);
    }
}

/*---------------------------------------------------------------------------*
  V�u�����N�҂��֐�
 *---------------------------------------------------------------------------*/
static void GameWaitVBlankIntr(void)
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

// **                 
// ** �v���t�B�[���쐬
// **                 
void MakeProfileData(GDS_PROFILE* _pProfile)
{
	MATHCRC16Table crctable;
	
	MI_CpuClear8(_pProfile, sizeof(GDS_PROFILE));

	// 7�����܂�(8�����ڂ�NULL)
	memcpy(
		&_pProfile->name, 
		g_TrainerName,
		sizeof(g_TrainerName)
	);										// �v���C���[����		[�u��] SOAP�𗘗p����
	SetGDS_EOM(_pProfile->name, PLW_TRAINER_NAME_SIZE);

	_pProfile->player_id = 0x1234;			// �v���C���[ID			[ �| ] 0 - 0xffffffff
	_pProfile->player_sex = 0;				// �v���C���[����		[����] 0 - 1
	_pProfile->birthday_month = 4;			// �a����				[����] 1 - 12
	_pProfile->trainer_view = 0;			// �v���C���[�̌�����	[����] 0 - 15

	_pProfile->country_code = 0;			// �Z��ł��鍑�R�[�h	[����] 0 - 233
	_pProfile->local_code = 0;				// �Z��ł���n���R�[�h	[����] * - *

	_pProfile->version_code = 12;			// �o�[�W�����R�[�h		[����] 20, 21	(WHITE=20, BLACK=21)
	_pProfile->language = 0;				// ����R�[�h			[����] 1 - 8

	_pProfile->egg_flag = 0;				// �^�}�S�t���O			[����] 0 - 1
	_pProfile->form_no = 0;					// �t�H����No			[����] * - *
	_pProfile->mons_sex = 0;				// �����X�^�[����		[����] 0 - 1
	_pProfile->monsno = 25;					// �����X�^�[No			[����] 1 - ��

	// MESSAGE_FLAG_NORMAL(�ȈՉ�b) / MESSAGE_FLAG_EVENT(�t���[���[�h)
	_pProfile->message_flag = MESSAGE_FLAG_NORMAL;	//				[����] 0 - 0

	// �ȈՉ�b�ݒ�
	MakePMSData(&_pProfile->self_introduction);

	// MATH_CalcCRC16CCITT											[����] 
	MATH_CRC16CCITTInitTable(&crctable);
	_pProfile->crc.crc16ccitt_hash = MATH_CalcCRC16CCITT(&crctable, _pProfile, sizeof(GDS_PROFILE) - GDS_CRC_SIZE);
}


// **                 
// ** PMS�f�[�^�쐬
// **                 
void MakePMSData(PMS_DATA* _pPMS)
{
	_pPMS->sentence_type = 0;	// ���̓^�C�v		[����] 0 - 4
	_pPMS->sentence_id = 0;		// �^�C�v��ID		[����] 0 - 19
	_pPMS->word[0] = 0;			// �P��ID			[����] 0 - 1735
	_pPMS->word[1] = 0;
}


// **                 
// ** �~���[�W�J���V���b�g�쐬
// **                 
void MakeMusicalshotData(MUSICAL_SHOT_DATA *_pMusicalData, int _MonsNoInc)
{
    int i;
	MI_CpuClear8(_pMusicalData, sizeof(MUSICAL_SHOT_DATA));

	//�~���[�W�J���^�C�g��(���{18�E�C�O36�{EOM
	memcpy(&_pMusicalData->title, g_MusicalTitle, sizeof(g_MusicalTitle));
	SetGDS_EOM(_pMusicalData->title, MUSICAL_PROGRAM_NAME_MAX);
	
	_pMusicalData->bgNo		= 1;			// �w�i�ԍ�					[����] 0 - 4
	_pMusicalData->spotBit	= 0x2;			// �X�|�b�g���C�g�Ώ�(bit)(�g�b�v�������|�P����)
											//							[����] 0 - 15
	_pMusicalData->year		= 2010 - 2000;	// �N						[����] 0 - 99
	_pMusicalData->month	= 4;			// ��						[����] 1 - 12
	_pMusicalData->day		= 19;			// ��						[����] 1 - 31
	_pMusicalData->player	= 1;			// �����̔ԍ�				[����] 0 - 3
	_pMusicalData->musVer	= 0;			// �~���[�W�J���o�[�W����	[����] 0 - 0
	_pMusicalData->pmVersion= 0;			// PM_VERSION				[����] 20, 21	(WHITE=20, BLACK=21)
	_pMusicalData->pmLang	= 0;			// PM_LANG					[����] 1 - 8

	for(i = 0;i < MUSICAL_POKE_MAX; i++)
	{
		_pMusicalData->shotPoke[i].monsno	= (u16)(_MonsNoInc + i);// �|�P�����ԍ�	[ �| ] �ʃT�[�o�ɂă`�F�b�N	// �|�P�����ԍ�
		_pMusicalData->shotPoke[i].sex		= 0;					// ����			[ �| ]
		_pMusicalData->shotPoke[i].rare		= 0;					// ���A�t���O	[ �| ]
		_pMusicalData->shotPoke[i].form		= 0;					// �t�H�����ԍ�	[ �| ]

		// �g���[�i�[��
		// 7�����܂�(8�����ڂ�NULL)
		memcpy(
			&_pMusicalData->shotPoke[i].trainerName,
			g_TrainerName,
			sizeof(g_TrainerName)
		);
		SetGDS_EOM(_pMusicalData->shotPoke[i].trainerName, MUSICAL_TRAINER_NAME_LEN);

		// �����O�b�Y
		// �ő�W�܂ő����\(�����ł͂P�����ݒ肵�Ȃ�)
		_pMusicalData->shotPoke[i].equip[0].itemNo	= (u16)i;		    // �O�b�Y�ԍ�	[����] 0 - 99, 65535	(65535�̓f�[�^�����̏ꍇ)
		_pMusicalData->shotPoke[i].equip[0].angle	= (s16)(i * 10);	// �p�x			[����] -32767 - 32768
		_pMusicalData->shotPoke[i].equip[0].equipPos= (u8)i;		    // �����ӏ�		[����] 0 - 8, 10		(10�̓f�[�^�����̏ꍇ)
	}
}


// **                 
// ** �o�g���r�f�I�f�[�^�쐬
// ** (�w�b�_)
// **                 
void MakeBattleVideoHeader(BATTLE_REC_HEADER* _pHeader, BOOL bSubway, int _MonsNoInc)
{
    int i;
	MATHCRC16Table crctable;

	// ������
	MI_CpuClear8(_pHeader, sizeof(BATTLE_REC_HEADER));

	for(i = 0; i < HEADER_MONSNO_MAX; i++)
	{
		_pHeader->monsno[i]	        	= (u16)(_MonsNoInc + i);	// �|�P�����ԍ�			[����] 1 - ��
		_pHeader->form_no_and_sex[i]	= 0;				        // �|�P�����t�H�����ԍ�	[����]
	}
	
	_pHeader->battle_counter	= 0;								// �A����				[����] 0 - 9999

	// �퓬���[�h													// �퓬���[�h			[����] �\����ނ̌Œ�l
	if(bSubway)
	{
		// �����L���O��ʂ̓o�g���T�u�E�F�C
		_pHeader->mode = BATTLEMODE_COMMUNICATION_SUBWAY << BATTLEMODE_COMMUNICATION_BIT;
	}
	else
	{
		// �����L���O��ʂ͒ʐM�ΐ�
		_pHeader->mode = BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT;
	}
	
	_pHeader->secure		= 0;		// (TRUE:���S�ۏ�, FALSE:���Đ�)[����] 0 - 0
	_pHeader->magic_key		= REC_OCC_MAGIC_KEY;	// �}�W�b�N�L�[		[����] 0xe281 - 0xe281
	_pHeader->server_vesion	= 100;		// �T�[�o�o�[�W����				[����] 100 - 100
	_pHeader->data_number	= 0;		// �T�[�o���ŃZ�b�g�����		[ �| ] 
	//_pHeader->work[]			= 0;	// �\��							[ �| ] 

	// MATH_CalcCRC16CCITT
	MATH_CRC16CCITTInitTable(&crctable);
	_pHeader->crc.crc16ccitt_hash = MATH_CalcCRC16CCITT(&crctable, _pHeader, sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE);
}


// **                 
// ** �o�g���r�f�I�f�[�^�쐬
// ** (�{��)
// **                 
void MakeBattleVideoData(BATTLE_REC_WORK* _pData, int _MonsNoInc)
{
    int i;
	MATHCRC16Table crctable;

	// ������
	MI_CpuClear8(_pData, sizeof(BATTLE_REC_WORK));

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
	_pData->setupSubset.fieldSituation.weather	= BTL_WEATHER_NONE;				// �V��							[����] 0 - 5
	_pData->setupSubset.fieldSituation.season	= 0;							// �l�G							[����] 0 - 3
	_pData->setupSubset.fieldSituation.zoneID	= (ZONEID)0;					// �R�����g����					[����] - - -
	_pData->setupSubset.fieldSituation.hour		= 0;							// �R�����g����					[����] 0 - 23
	_pData->setupSubset.fieldSituation.minute	= 0;							// �R�����g����					[����] 0 - 59

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
	_pData->setupSubset.competitor	= 0;	// �ΐ�҃^�C�v�i�Q�[�����g���[�i�[�A�ʐM�ΐ�j-> enum BtlCompetitor @ battle/battle.h		[����] 2 - 3
	_pData->setupSubset.myCommPos	= 0;	// �ʐM�ΐ펞�̎����̗����ʒu�i�}���`�̎��A0,2 vs 1,3 �ɂȂ�A0,1�������^2,3���E���ɂȂ�j	[����] 0 - 3
	_pData->setupSubset.rule		= 0;	// ���[���i�V���O���E�_�u���E�g���v���E���[�e�j-> enum BtlRule @ battle/battle.h			[����] 0 - 3
	_pData->setupSubset.fMultiMode	= 0;	// �}���`�o�g���t���O�i���[���͕K���_�u���j		[����] 0 - 1

	// �N���C�A���g������e�̕ۑ��o�b�t�@
	// BTLREC_OPERATION_BUFFER �\����
	_pData->opBuffer.size		= 0;		// �T�C�Y�FBTLREC_OPERATION_BUFFER_SIZE			[����] 0 - 0xc00
	_pData->opBuffer.buffer[0]	= 0;		// �o�b�t�@										[����] 

	// �o�g���Q���v���C���[���̏��
	for(i = 0; i < BTL_CLIENT_MAX; i++)
	{
        int j = 0;

		// �|�P�����p�[�e�B�f�[�^
		// REC_POKEPARTY �\����
		_pData->rec_party[i].PokeCountMax = 0;	// �ێ��o����|�P�������̍ő�				[����] 6 - 6
		_pData->rec_party[i].PokeCount = 0;		// ���ݕێ����Ă���|�P������				[����] 1 - 6
		
		// �|�P�����f�[�^
		// REC_POKEPARTY :: REC_POKEPARA �\����												[ �| ] �ʃT�[�o�ɂă`�F�b�N
		for(j = 0; j < TEMOTI_POKEMAX; j++)
		{
			// �|�P�����̃X�e�[�^�X���
			_pData->rec_party[i].member[j].monsno = (u16)(_MonsNoInc + j);
		}

		// �v���C���[���
		// BTLREC_CLIENT_STATUS �\����
		_pData->clientStatus[i].type = BTLREC_CLIENTSTATUS_PLAYER;	// �N���C�A���g�^�C�v	[����] 0 - 2 

		if(_pData->clientStatus[i].type == BTLREC_CLIENTSTATUS_PLAYER)
		{
			// MYSTATUS �\����
			memcpy(
				&_pData->clientStatus[i].player.name,
				g_TrainerName,
				sizeof(g_TrainerName)
			);													// �v���C���[��
			SetGDS_EOM(_pData->clientStatus[i].player.name, PERSON_NAME_SIZE + EOM_SIZE);

			_pData->clientStatus[i].player.id = 0;				// �g���[�i�[ID				[�u��] SOAP�𗘗p����
			_pData->clientStatus[i].player.profileID = 0;		// �v���t�@�C��ID			[����] 
			_pData->clientStatus[i].player.nation = 0;			// �Z��ł��鍑�R�[�h		[����] 
			_pData->clientStatus[i].player.area = 0;			// �Z��ł���n���R�[�h		[����] 
			_pData->clientStatus[i].player.sex = 0;				// ����						[����] 0 - 1
			_pData->clientStatus[i].player.region_code = 0;		// ����R�[�h				[����] 1 - 8
			_pData->clientStatus[i].player.trainer_view = 0;	// ������					[����] 0 - 15
			_pData->clientStatus[i].player.rom_code = 0;		// �o�[�W�����R�[�h			[����] 20, 21	(WHITE=20, BLACK=21)
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

			memcpy(
				&_pData->clientStatus[i].trainer.name,
				g_TrainerName2,
				sizeof(g_TrainerName2)
			);													// �v���C���[��
			SetGDS_EOM(_pData->clientStatus[i].trainer.name, BUFLEN_PERSON_NAME);

			MakePMSData(&_pData->clientStatus[i].trainer.win_word);		// �퓬�I�����������b�Z�[�W <-8byte
			MakePMSData(&_pData->clientStatus[i].trainer.lose_word);	// �퓬�I�����������b�Z�[�W <-8byte
		}
	}

	_pData->magic_key			= 0;	// �}�W�b�N�L�[										[����] 0xe281 - 0xe281
	//_pData->padding = 0;				// 													[����]
	
	// MATH_CalcCRC16CCITT
	MATH_CRC16CCITTInitTable(&crctable);
	_pData->crc.crc16ccitt_hash = MATH_CalcCRC16CCITT(&crctable, _pData, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE);
}


// **                 
// ** �I�[����(0xFFFF)�������܂��B
// ** �o�b�t�@�I�[�o�[�t���[�ɒ��ӂ��Ă��������B
// ** (�K���o�b�t�@�̍Ō�̗v�f�ɂ�0������悤�ɂ��Ă��������B)
// **                 
void SetGDS_EOM(STRCODE* _pStr, int _Size)
{
	int i = 0;
	for(i = 0; i < _Size; i++)
	{
		if(_pStr[i] == 0)
		{
			_pStr[i] = 0xFFFF;
			break;
		}
	}
}


/** --------------------------------------------------------------------
  �V�[������
  ----------------------------------------------------------------------*/
static int processSceneElement(GameSequence* seq, s16 x, s16 y)
{
    InputSequence* input = &seq->inputList[seq->currentElement];

    if (input->type == TYPE_SELECT) {
        return processMenuSequence(seq, x, y);
    }
    else if (input->type == TYPE_INPUT) {
        //processInput(&input->elements[seq->currentElement], x, y);
        return processInput(seq, x, y);
    }
    else if (input->type == TYPE_NORMAL) {
        return INPUT_CONFIRM;    
    }

    // �����ɂ͂��Ȃ�
    return INPUT_NOW;
}

/** --------------------------------------------------------------------
  ���͊֘A
  ----------------------------------------------------------------------*/
const int MAX_KETA = 8;

/** --------------------------------------------------------------------
  ���͏���������
  ----------------------------------------------------------------------*/
static void initInput(InputElement* input, u32 min, u32 max, u32 first)
{
#pragma unused(min)
#pragma unused(max)
    MI_CpuClear8(input->buf, sizeof(input->buf));
    input->inputKeta = 0;
    input->inputValue = first;
    snprintf(input->buf, 20, "%08d", first);
}

/** --------------------------------------------------------------------
  ���͏���
  ----------------------------------------------------------------------*/
static int processInput(GameSequence* seq, s16 x, s16 y)
{
    InputSequence* inputSeq = &seq->inputList[seq->currentElement];
    InputElement* input = &inputSeq->elements[0];

    // ��ԕ\��
    (void)dispInput(input, x, y);

    // ����
    if (g_KeyCtrl.trg & PAD_KEY_UP) {
        if (input->buf[input->inputKeta] == '9') {
            input->buf[input->inputKeta] = '0'; 
        } else {
            input->buf[input->inputKeta]++; 
        }
    }
    else if (g_KeyCtrl.trg & PAD_KEY_DOWN) {
        if (input->buf[input->inputKeta] == '0') {
            input->buf[input->inputKeta] = '9'; 
        } else {
            input->buf[input->inputKeta]--; 
        }
    }
    else if (g_KeyCtrl.trg & PAD_KEY_LEFT) {
        if (input->inputKeta == 0) {
            input->inputKeta = MAX_KETA - 1;
        } else {
            input->inputKeta--;
        }
    }
    else if (g_KeyCtrl.trg & PAD_KEY_RIGHT) {
        if (input->inputKeta == (MAX_KETA - 1)) {
            input->inputKeta = 0;
        } else {
            input->inputKeta++;
        }
    }
    if (g_KeyCtrl.trg & PAD_BUTTON_A) {
        input->inputValue = (u32)atoi(input->buf);
        inputSeq->result = (int)input->inputValue;
        return INPUT_CONFIRM;
    }
    else if (g_KeyCtrl.trg & PAD_BUTTON_B) {
        return INPUT_CANCEL;
    }
    return INPUT_NOW; 
}

/** --------------------------------------------------------------------
  ���̓V�[�P���X������
  ----------------------------------------------------------------------*/
static void initInputSequence(InputSequence* input)
{
    input->currentElement = 0;
    {
        int i;
        for (i = 0; i < MAX_INPUT_ELEMENTS; ++i) {
            initInput(&input->elements[i], 0, 0, 0);
        }
    }
}

/** --------------------------------------------------------------------
  ���̓V�[�P���X����
  ----------------------------------------------------------------------*/
static int processSceneSequence(GameSequence* seq)
{
    InputSequence* input = &seq->inputList[seq->currentElement];
    initInputSequence(input);

    while (1) {
    	int ret;
        s16 x = 0, y = 0;

        drawConsole();
        ReadKeyData();

        ClearScreen();

        y = (s16)(DispTitle(seq->modeName, x, y) + 1);
        ret = processSceneElement(seq, x, y);
	    switch (ret) {
	        case INPUT_CONFIRM:
	            {
                    // TYPE_NORMAL �̏ꍇ�͂����I���
	                if (input->type == TYPE_NORMAL ||
                        seq->currentElement >= (seq->numElements - 1)) {
                        return ret;
	                } else {
	                    // ���̓��͂�
	                    seq->currentElement++;
	                }
	            }
	            break;

	        case INPUT_CANCEL:
	            {
	                if (seq->currentElement == 0) {
                        return ret;
	                } else {
	                    // �ЂƂ߂�
	                    InputElement* elem;
	                    seq->currentElement--;
	                    elem = &input->elements[seq->currentElement];
	                    initInput(elem, 0, 0, elem->inputValue);
	                }
	            }
	            break;

            // INPUT_NOW �͏������s
	    }
    	
        dbs_DemoUpdate();
        OS_WaitIrq(TRUE, OS_IE_V_BLANK);
        Heap_Debug();
        dbs_DemoLoad();
    }

    // �����ɂ͂��܂���
    return INPUT_CONFIRM;
}

/** --------------------------------------------------------------------
  ���͏�ԕ\��
  ----------------------------------------------------------------------*/
static s16 dispInput(InputElement* input, s16 x, s16 y)
{
    int i = 0;

    // �^�C�g���\��
    y = (s16)(DispSubTitle(input->head, x, y) + 1);

    // ���ڕ\��
    x = 3;
    for (i = 0; i < MAX_KETA; ++i) {
        PrintCharacter(x, y, 0xf, input->buf[i]);
        if (i == input->inputKeta) {
            PrintString(x, (s16)(y+1), 0xf, "^");
        }
        x++;
    }

    return (s16)(y + 2);
}

/** --------------------------------------------------------------------
  ���͌��ʎ擾
  ----------------------------------------------------------------------*/
static int getResultValue(GameSequence* seq, int no)
{
    if (no < 0 || no >= seq->numElements) return -1;

    return (seq->inputList[no].result);
}




