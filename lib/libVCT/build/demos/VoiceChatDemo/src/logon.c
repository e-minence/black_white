#include <nitro.h>
#include <dwc.h>
#include <string.h>

#include "main.h"
#include "init.h"
#include "utility.h"
#include "registerfriend.h"
#include "transport.h"
#include "dbs.h"

#include "logon.h"

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
#define GAME_NUM_MATCH_KEYS 3      // �}�b�`���C�N�p�ǉ��L�[��       // 
#define ConnectMaxVCT	8          //
#define FILTER_STRING "sample_filter"	// �t�B���^������. �K�X�ύX���Ă�������
#define FILTER_KEY "str_key"
//#define GAME_USE_STORAGE_SERVER
//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
extern GameSequence gameSeqList[GAME_MODE_NUM];
extern GameControl stGameCnt;  // �Q�[��������\����  
extern KeyControl stKeyCnt;    // �L�[���͐���\����  
u32  s_groupID = 0;
// �F�B���w��s�A�}�b�`���C�N�p�ǉ��L�[�f�[�^�\���� 
static GameMatchExtKeys stMatchKeys[GAME_NUM_MATCH_KEYS] = { 0, };


// GameSpy Status�f�[�^�Ƃ��ăZ�b�g����T���v���f�[�^
static const char gsStatusSample[16] = { 10, 9, 8, 0, 0, 1, 2, 3, };

static int stNumEntry  = 2;    // �l�b�g���[�N�\���l���w�� 
static int stServerIdx = 0;    // �ڑ��������Q�[���T�[�o�̗F�B���X�g�C���f�b�N�X 

char addFilter[128] = "";
static u8   userData[4]={ 0,5,120,254 };
extern DWCTopologyType s_topologyType;

static void SetGameMatchKeys(void);

//callback�v���g�^�C�v 
static void ConnectToAnybodyCallback(DWCError error, 
                                     BOOL cancel, 
                                     BOOL self,
                                     BOOL isServer,
                                     int  index,
                                     void* param);

static void ConnectToFriendsCallback(DWCError error, 
                                     BOOL cancel, 
                                     BOOL self,
                                     BOOL isServer,
                                     int  index,
                                     void* param);

static void SetupGameServerCallback(DWCError error,
                                    BOOL cancel,
                                    BOOL self,
                                    BOOL isServer,
                                    int index,
                                    void* param);
static void ConnectToGameServerCallback(DWCError error,
                                        BOOL cancel,
                                        BOOL self,
                                        BOOL isServer,
                                        int index,
                                        void* param);
static void NewClientCallback(int index, void* param);
static int  EvaluateAnybodyCallback(int index, void* param);
static int  EvaluateFriendCallback(int index, void* param);
static void GenericNewClientCallback(int index, void* param);
static void LoginToStorageCallback(DWCError error, void* param);
/*---------------------------------------------------------------------------*
  �Q�[����`�̃}�b�`���C�N�p�ǉ��L�[�Z�b�g�֐�
 *---------------------------------------------------------------------------*/
static void SetGameMatchKeys(void)
{
    int ranking = (int)((OS_GetTick()*1592653589UL+453816691UL) & 0xff);

    // �����L���O�𐮐��L�[�Ƃ��Đݒ�
    stMatchKeys[0].isStr  = 0;
    strcpy(stMatchKeys[0].keyStr, "game_rank");
    stMatchKeys[0].ivalue = ranking;
    stMatchKeys[0].keyID  =
        DWC_AddMatchKeyInt(stMatchKeys[0].keyID,
                           stMatchKeys[0].keyStr,
                           &stMatchKeys[0].ivalue);
    if (stMatchKeys[0].keyID == 0) OS_TPrintf("AddMatchKey failed 0.\n");

    // �}�b�`���C�N�ɗp����]���^�C�v�𕶎���L�[�Ƃ��Đݒ�
    stMatchKeys[1].isStr  = 1;
    strcpy(stMatchKeys[1].keyStr, FILTER_KEY);
    strcpy(stMatchKeys[1].svalue, FILTER_STRING);
    stMatchKeys[1].keyID  =
        DWC_AddMatchKeyString(stMatchKeys[1].keyID,
                              stMatchKeys[1].keyStr,
                              stMatchKeys[1].svalue);
    if (stMatchKeys[1].keyID == 0) OS_TPrintf("AddMatchKey failed 1.\n");

    // �Ƃ肠������������炢�����L�[��ݒ�
    stMatchKeys[2].isStr  = 0;
    strcpy(stMatchKeys[2].keyStr, "game_manner");
    stMatchKeys[2].ivalue = ranking%11;
    stMatchKeys[2].keyID  =
        DWC_AddMatchKeyInt(stMatchKeys[2].keyID,
                           stMatchKeys[2].keyStr,
                           &stMatchKeys[2].ivalue);
    if (stMatchKeys[2].keyID == 0) OS_TPrintf("AddMatchKey failed 2.\n");
}


/*---------------------------------------------------------------------------*
  ���O�C���ヂ�[�h�̒ǉ��\���p�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
void LogonModeDispCallback(int curIdx, void* param)
{
#pragma unused(param)

    // �����ȊO�̃G���g���[�l���ւ̃|�C���^��\������
    OS_TPrintf("\nnumber of entry = ");
    if (stNumEntry == 2){
        OS_TPrintf("   %d >>\n", stNumEntry);
    }
    else if (stNumEntry == DWC_MAX_CONNECTIONS){
        OS_TPrintf("<< %d\n", stNumEntry);
    }
    else {
        OS_TPrintf("<< %d >>\n", stNumEntry);
    }

    if (curIdx == 3){
        // �Q�[���T�[�o�ւ̐ڑ����́A�F�B���X�g�C���f�b�N�X���\������
        OS_TPrintf("GameServerIndex = ");
        if (DWC_GetNumFriend(stGameCnt.friendList.keyList, GAME_MAX_FRIEND_LIST) == 1){
            OS_TPrintf("   %d\n", stServerIdx);
        }
        else if (stServerIdx == 0){
            OS_TPrintf("   %d >>\n", stServerIdx);
        }
        else if (stServerIdx == GAME_MAX_FRIEND_LIST-1){
            OS_TPrintf("<< %d\n", stServerIdx);
        }
        else {
            OS_TPrintf("<< %d >>\n", stServerIdx);
        }
    }
}

/*---------------------------------------------------------------------------*
  �T�[�o�N���C�A���g�}�b�`���C�N���̐V�K�ڑ��N���C�A���g�ʒm�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void NewClientCallback(int index, void* param)
{
#pragma unused(param)

    // �V�K�ڑ��N���C�A���g�̐ڑ��������I���܂ł́A
    // �R���g���[��������֎~����
    stGameCnt.blocking = TRUE;
    
    OS_TPrintf("New client started to connect.\n");
    if (index != -1){
        OS_TPrintf("He is my friend [%d].\n", index);
    }
    else {
        OS_TPrintf("He is not my friend.\n");
    }
}


/*---------------------------------------------------------------------------*
  �F�B���w��s�A�}�b�`���C�N���v���C���[�]���R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static int  EvaluateAnybodyCallback(int index, void* param)
{
#pragma unused(param)
    const char* matchType = DWC_GetMatchStringValue(index, stMatchKeys[1].keyStr, "NONE");
    int rank;
    int diff;
    
    if (!strncmp(matchType, "eval_rank", strlen("eval_rank"))){
        // ������}�b�`���C�N�^�C�v�Ƃ���FILTER_STRING���Z�b�g���Ă���ꍇ
        rank = DWC_GetMatchIntValue(index, stMatchKeys[0].keyStr, -1);
        if (rank == -1){
            return 0;  // "game_rank"�L�[���Ȃ���΃}�b�`���C�N���Ȃ� 
        }

        diff = (stMatchKeys[0].ivalue > rank) ? stMatchKeys[0].ivalue-rank : rank-stMatchKeys[0].ivalue;

        OS_TPrintf("Player[%d]'s rank is %d (mine %d). %d point.\n",
                   index, rank, stMatchKeys[0].ivalue, 200-diff);

#if 0   // �]���̃e�X�g���s�������ꍇ�͂�����1�ɂ���
        // �����Ƃ̏��ʍ����߂��قǍ����_�ɂȂ�悤�ɁA200�_���_�œ_��t����B
        // 200�ʍ��ȏ�Ȃ�}�b�`���C�N���Ȃ��i0�ȉ��̒l��Ԃ��j
        return 200-diff;

#else   // ���i�̓}�b�`���C�N���Ȃ����Ƃ�����Ɩʓ|�Ȃ̂ł������ʂ� 
		return 300-diff;
#endif
    }
    else if (!strncmp(matchType, "NONE", strlen("NONE"))){
        return 1;	// "game_mtype"�L�[���Ȃ���΂Ƃ肠���������Ƀ}�b�`���C�N���� 

    }

    return 1;
}


/*---------------------------------------------------------------------------*
  �F�B�w��s�A�}�b�`���C�N���v���C���[�]���R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static int  EvaluateFriendCallback(int index, void* param)
{
#pragma unused(param)
    const char* matchType = DWC_GetMatchStringValue(index, stMatchKeys[1].keyStr, "NONE");
    
    if (!strncmp(matchType, FILTER_STRING, strlen(FILTER_STRING))){
        // ������}�b�`���C�N�^�C�v�Ƃ���FILTER_STRING���Z�b�g���Ă���ꍇ
        return 1;  // �}�b�`���C�NOK 
    }
    else {
        return 0;  // "game_mtype"�L�[���Ȃ���΃}�b�`���C�N���Ȃ� 
    }
}

/*---------------------------------------------------------------------------*
  ���O�C���チ�C���֐�
 *---------------------------------------------------------------------------*/
GameMode GameLogonMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_LOGIN];
    GameMode returnSeq = GAME_MODE_NUM;
    int curIdx = 0;
    BOOL boolRet;

    stNumEntry  = 2;
    stServerIdx = 0;

    // �f�o�b�O�\�����x���w��
    DWC_SetReportLevel(DWC_REPORTFLAG_ALL);

#if 0
// �T���v���̗F�B�o�^���\���e�X�g
{
    char tmpstr[DWC_FRIENDKEY_STRING_BUFSIZE];
    
    DWC_FriendKeyToString(tmpstr, DWC_CreateFriendKey(&stUserData));
    OS_TPrintf("friendKeyToken '%s'\n", tmpstr);
}
#endif

#ifdef GAME_USE_STORAGE_SERVER
// GameSpy�X�g���[�W�T�[�o�ւ̃Z�[�u�E���[�h���s�Ȃ��e�X�g�̂��߂̃��O�C������
{
    if (!DWC_LoginToStorageServerAsync(LoginToStorageCallback, NULL)){
        OS_TPrintf("DWC_LoginToStorageServerAsync() failed.\n");
    }
}
#endif

    while (1){
        dbs_Print( 0, 0, "s:%d", DWC_GetMatchState() );
        dbs_Print( 7, 0, "n:%d", DWC_GetNumConnectionHost() );
        dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel() );
        dbs_Print( 10,1, "p:%d", stGameCnt.userData.profileID );
	    dbs_Print( 30, 0, "%c",  LoopChar());
        dbs_Print( 0,2, "AID:%d(0x%08X)", DWC_GetMyAID(), DWC_GetAIDBitmap() );
        dbs_Print( 20, 2, "g:%d", s_groupID );

        ReadKeyData();  // �L�[�f�[�^�擾 

        DWC_ProcessFriendsMatch();  // DWC�ʐM�����X�V 

        HandleDWCError(&returnSeq);  // �G���[����

        if (stGameCnt.blocking){
            // DWC�������̓L�[������֎~����i�L�����Z���̂݉j
            if (stKeyCnt.trg & PAD_BUTTON_B){
                // B�{�^���Ń}�b�`���C�N���L�����Z������
                DWC_CloseAllConnectionsHard();
                stGameCnt.blocking = FALSE;
                returnSeq = GAME_MODE_LOGIN;
            }

            // V�u�����N�҂�����
            GameWaitVBlankIntr();
            continue;
        }

        // ���ɐi�ނׂ����[�h���Z�b�g����Ă����烋�[�v�𔲂���
        if (returnSeq != GAME_MODE_NUM) break;

        ////////// �ȉ��L�[���쏈��
        if (stKeyCnt.trg & PAD_BUTTON_A){
            // A�{�^���Ń��j���[����
            returnSeq = gameSeq->menuList[curIdx].mode;
            stGameCnt.blocking = gameSeq->menuList[curIdx].blocking;
            boolRet   = TRUE;

            switch (curIdx){
            case 0:  // �F�B���w��s�A�}�b�`���C�N�Ăяo�� 
                if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                    // X�{�^����������Ă��Ȃ���΁A�}�b�`���C�N�p�ǉ��L�[�f�[�^�ݒ�
                    SetGameMatchKeys();
                }
                boolRet = 
                DWC_ConnectToAnybodyAsync(s_topologyType,
                                          (u8)stNumEntry,
                                          addFilter,
                                          ConnectToAnybodyCallback,
                                          &returnSeq,
                                          GenericNewClientCallback,
                                          NULL,
                                          EvaluateAnybodyCallback,
                                          NULL,
                                          NULL,
                                          userData,
                                          NULL);
                break;
            case 1:  // �F�B�w��s�A�}�b�`���C�N�Ăяo�� 
                if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                    // X�{�^����������Ă��Ȃ���΁A�}�b�`���C�N�p�ǉ��L�[�f�[�^�ݒ�
                    SetGameMatchKeys();
                }
                // ����friendIdxList��NULL���w�肷��ƗF�B���X�g���̑S�����猟������
                boolRet =
                DWC_ConnectToFriendsAsync(s_topologyType,
                                          NULL,
                                          ConnectMaxVCT,
                                          (u8)stNumEntry,
                                          ConnectToFriendsCallback,
                                          &returnSeq,
                                          GenericNewClientCallback,
                                          NULL,
                                          EvaluateFriendCallback,
                                          NULL,
                                          NULL,
                                          userData,
                                          NULL);
                break;
            case 2:  // �T�[�oDS�Ƃ��ăT�[�o�N���C�A���g�}�b�`���C�N�J�n 
                boolRet =
                DWC_SetupGameServer(s_topologyType,
                                    (u8)stNumEntry,
                                    SetupGameServerCallback,
                                    &returnSeq,
                                    NewClientCallback,
                                    NULL,
                                    NULL,
                                    userData,
                                    NULL);
                break;
            case 3:  // �N���C�A���gDS�Ƃ��ăT�[�o�N���C�A���g�}�b�`���C�N�J�n 
                boolRet =
                DWC_ConnectToGameServerAsync(s_topologyType,
                                             stServerIdx,
                                             ConnectToGameServerCallback,
                                             &returnSeq,
                                             NewClientCallback,
                                             NULL,
                                             NULL,
                                             userData,
                                             NULL);
                break;
            case 4:
                if(s_groupID)
                {
                     boolRet =
                     DWC_ConnectToGameServerByGroupID( s_topologyType,  // �ڑ��`��
                                                       s_groupID,		// �O���[�vID
                                                       ConnectToGameServerCallback,
                                                       &returnSeq,
                                                       NewClientCallback,
                                                       NULL,
                                                       NULL,
                                                       userData,
                                                       NULL );
                }
                else
                {
                    boolRet = FALSE;
                }
                break;
            case 5:
                break;
            case 6:  // ���O�A�E�g 
                ShutdownInet();  // �l�b�g���[�N�ؒf 
                break;
            default:
                break;
            }
            if (!boolRet){
                // �}�b�`���C�N���J�n�ł��Ȃ������ꍇ�̓u���b�L���O��Ԃɂ��Ȃ��B
                // �G���[�����������O�C���O�A�ڑ���A��d�Ăяo��������
                // FALSE�ɂ͂Ȃ�Ȃ�
                OS_TPrintf("Can't call matching function.\n");
                returnSeq          = GAME_MODE_NUM;
                stGameCnt.blocking = 0;
            }
        }
        else if (stKeyCnt.trg & PAD_BUTTON_Y){
            if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                // X�{�^����������Ă��Ȃ���΁AY�{�^���Ō��݂̗F�B�̏�Ԃ��擾�\���istatusString�j
                DispFriendList(TRUE);
                OS_TPrintf("\n");
            }
            else {
                // X�{�^����������Ă���΁AY�{�^���Ō��݂̗F�B�̏�Ԃ��擾�\���istatusData�j
                DispFriendListData();
                OS_TPrintf("\n");
            }
        }
        else if (stKeyCnt.trg & PAD_BUTTON_SELECT){
#ifndef GAME_USE_STORAGE_SERVER  // GameSpy��Status�Z�b�g�������ꍇ
            if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                // X�{�^����������Ă��Ȃ���΁A�Z���N�g�{�^����GameSpy Status��������Z�b�g
                if (DWC_SetOwnStatusString("SampleString-00")){
                    OS_TPrintf("Set GsStatusString.\n");
                }
                else {
                    OS_TPrintf("Failed to set GsStatusString.\n");
                }
            }
            else {
                // X�{�^����������Ă���΁A�Z���N�g�{�^����GameSpy Status�f�[�^���Z�b�g
                if (DWC_SetOwnStatusData(gsStatusSample, sizeof(gsStatusSample))){
                    OS_TPrintf("Set GsStatusData.\n");
                }
                else {
                    OS_TPrintf("Failed to set GsStatusData.\n");
                }
            }

#else  // �X�g���[�W�T�[�o�ւ̃Z�[�u�������ꍇ
            // �Z���N�g�{�^���ŃX�g���[�W�T�[�o�Ƀe�X�g�L�[���Z�[�u
            char keyvalues[30];
            BOOL boolResult;

            if (stKeyCnt.cont & PAD_BUTTON_X){
                // X�{�^����������Ă����Private�f�[�^���Z�[�u
                snprintf(keyvalues, sizeof(keyvalues), "\\test_key_prv\\%lld", OS_TicksToSeconds(OS_GetTick()));
                boolResult = DWC_SavePrivateDataAsync(keyvalues, NULL);
            }
            else {
                // X�{�^����������Ă��Ȃ����Public�f�[�^���Z�[�u
                snprintf(keyvalues, sizeof(keyvalues), "\\test_key_pub\\%lld", OS_TicksToSeconds(OS_GetTick()));
                boolResult = DWC_SavePublicDataAsync(keyvalues, NULL);
            }

            if (boolResult){
                OS_TPrintf("Saved '%s'.\n", keyvalues);
            }
            else {
                OS_TPrintf("Can't save to storage server.\n");
            }
#endif
        }
        else if (stKeyCnt.trg & PAD_BUTTON_START){
#ifndef GAME_USE_STORAGE_SERVER  // GameSpy��Status�Z�b�g�������ꍇ
            char statusData[DWC_FRIEND_STATUS_STRING_LEN];
            
            if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                // X�{�^����������Ă��Ȃ���΁A
                // �X�^�[�g�{�^����GameSpy Status��������擾
                if (DWC_GetOwnStatusString(statusData)){
                    OS_TPrintf("Got my GsStatusString : %s\n", statusData);
                }
                else {
                    OS_TPrintf("Failed to get GsStatusString.\n");
                }
            }
            else {
                // X�{�^����������Ă���΁A
                // �X�^�[�g�{�^����GameSpy Status�f�[�^���擾
                int size, i;

                size = DWC_GetOwnStatusData(statusData);
                if (size != -1){
                    OS_TPrintf("Got my GsStatusData\n");
                    for (i = 0; i < size; i++){
                        OS_TPrintf("%02d, ", statusData[i]);
                    }
                    OS_TPrintf("\n");
                }
                else {
                    OS_TPrintf("Failed to get GsStatusData.\n");
                }
            }

#else  // �X�g���[�W�T�[�o����̃��[�h�������ꍇ 
            // �X�^�[�g�{�^���ŃX�g���[�W�T�[�o����e�X�g�f�[�^�����[�h 
            BOOL boolResult;

#if 1
            if (stKeyCnt.cont & PAD_BUTTON_X){
                // X�{�^����������Ă���Ύ�����Private�f�[�^�����[�h
                boolResult = DWC_LoadOwnPrivateDataAsync("\\test_key_prv\\test_key_pub", NULL);
            }
            else {
                // X�{�^����������Ă��Ȃ���Ύ�����Public�f�[�^�����[�h
                boolResult = DWC_LoadOwnPublicDataAsync("\\test_key_prv\\test_key_pub", NULL);
            }
#else
            // �F�B���X�g���̃z�X�g�̃f�[�^�����[�h
            boolResult = DWC_LoadOthersDataAsync("\\test_key", stServerIdx, NULL);
#endif
            if (!boolResult) OS_TPrintf("Can't load from storage server.\n");
#endif
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
            // �\���L�[���ŃG���g���[�l������ 
            if (stNumEntry > 2){
                stNumEntry--;
	            // ���j���[���X�g�ĕ\�� 
                DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
            }
        }
        else if (stKeyCnt.trg & PAD_KEY_RIGHT){
            // �\���L�[�E�ŃG���g���[�l������ 
            if (stNumEntry < DWC_MAX_CONNECTIONS){
                stNumEntry++;
	            // ���j���[���X�g�ĕ\�� 
                DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
            }
        }
        else if (stKeyCnt.trg & PAD_BUTTON_L){
            // LR�{�^���ŃT�[�o�N���C�A���g�}�b�`���C�N�̃T�[�oDS�̗F�B���X�g�C���f�b�N�X��I��
            if (curIdx == 3){
                if (stServerIdx > 0){
                    stServerIdx--;
		            // ���j���[���X�g�ĕ\�� 
                    DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
                }
            }
        }
        else if (stKeyCnt.trg & PAD_BUTTON_R){
            // LR�{�^���ŃT�[�o�N���C�A���g�}�b�`���C�N�̃T�[�oDS�̗F�B���X�g�C���f�b�N�X��I��
            if (curIdx == 3){
                if (stServerIdx < GAME_MAX_FRIEND_LIST-1){
                    stServerIdx++;
		            // ���j���[���X�g�ĕ\�� 
                    DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
                }
            }
        }
        ////////// �L�[���쏈�������܂�

        // V�u�����N�҂�����
        GameWaitVBlankIntr();
    }

    return returnSeq;
}


/*---------------------------------------------------------------------------*
  �F�B���w��ڑ��R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void ConnectToAnybodyCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer,
                                     int index, void* param)
{
    (void)self;
    (void)isServer;
    (void)index;
    (void)param;
    
    if (error == DWC_ERROR_NONE){
        if (!cancel){
            // ���m��ʐl�����Ƃ̃R�l�N�V�����ݗ�����
            OS_TPrintf("Succeeded to connect to anybody\n\n");

            // ��M�o�b�t�@�Z�b�g
            SetRecvBuffer();
        }
        else {
            OS_TPrintf("Canceled matching.\n");
        }
    }
    else {
        OS_TPrintf("Failed to connect to anybody. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // �u���b�L���O���� 
}


/*---------------------------------------------------------------------------*
  �F�B�w��ڑ��R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void ConnectToFriendsCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer,
                                     int index, void* param)
{
    (void)self;
    (void)isServer;
    (void)index;
    (void)param;

    if (error == DWC_ERROR_NONE){
        if (!cancel){
            // �F�B�Ƃ̃R�l�N�V�����ݗ�����
            OS_TPrintf("Succeeded to connect to friends\n\n");

            // ��M�o�b�t�@�Z�b�g
            SetRecvBuffer();
        }
        else {
            OS_TPrintf("Canceled matching.\n");
        }
    }
    else {
        OS_TPrintf("Failed to connect to friends. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // �u���b�L���O���� 
}


/*---------------------------------------------------------------------------*
  �T�[�o�N���C�A���g�}�b�`���C�N�E�T�[�o�N���R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void SetupGameServerCallback(DWCError error,
                                    BOOL cancel,
                                    BOOL self,
                                    BOOL isServer,
                                    int  index,
                                    void* param)
{
#ifdef SDK_FINALROM
#pragma unused(index)
#endif
#pragma unused(isServer)

    if (error == DWC_ERROR_NONE){
        if (!cancel){
            // �l�b�g���[�N�ɐV�K�N���C�A���g���������
            OS_TPrintf("A friend of mine joined the game.\n");
            OS_TPrintf("friendListIndex = %d.\n\n", index);

            // ��M�o�b�t�@�Z�b�g
            SetRecvBuffer();
        }
        else {
            if (self){
                // �������}�b�`���C�N���L�����Z������
                OS_TPrintf("Canceled matching.\n\n");
                // ���O�C�����Ԃɖ߂�
                *(GameMode *)param = GAME_MODE_LOGIN;
            }
            else {
                // �N���C�A���gDS���}�b�`���C�N���L�����Z������
                OS_TPrintf("Client (friendListIndex = %d) canceled matching.\n\n", index);

                if (DWC_GetNumConnectionHost() == 1){
                    // �܂��N�Ƃ��ڑ����Ă��Ȃ���΁A���̏�Ԃɐi�܂Ȃ�
                    return;
                }
            }
        }
    }
    else {
        OS_TPrintf("Game server error occured. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // �u���b�L���O���� 
}


/*---------------------------------------------------------------------------*
  �T�[�o�N���C�A���g�}�b�`���C�N�E�T�[�o�ڑ��R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void ConnectToGameServerCallback(DWCError error,
                                        BOOL cancel,
                                        BOOL self,
                                        BOOL isServer,
                                        int  index,
                                        void* param)
{

    if (error == DWC_ERROR_NONE){
        if (!cancel){
            if (self){
                // �������T�[�oDS�Ƃ����ɂł��Ă���l�b�g���[�N�ւ̐ڑ��ɐ��������ꍇ
                OS_TPrintf("Succeeded to join the game.\n\n");
            }
            else {
                // �l�b�g���[�N�ɐV�K�N���C�A���g����������ꍇ
                OS_TPrintf("New client joined the game.\n");
                if (index != -1){
                    OS_TPrintf("he is my friend [%d].\n\n", index);
                }
                else {
                    OS_TPrintf("he is not my friend.\n\n");
                }
            }

            // ��M�o�b�t�@�Z�b�g
            SetRecvBuffer();
        }
        else {
            if (self){
                // �������}�b�`���C�N���L�����Z������
                OS_TPrintf("Canceled matching.\n\n");
                // ���O�C�����Ԃɖ߂�
                *(GameMode *)param = GAME_MODE_LOGIN;
            }
            else {
                if (isServer){
                    // �T�[�oDS���}�b�`���C�N���L�����Z������ 
                    OS_TPrintf("GameServer canceled matching.\n\n");
                    // �}�b�`���C�N���I�����ă��O�C�����Ԃɖ߂� 
                    *(GameMode *)param = GAME_MODE_LOGIN;
                }
                else {
                    // ���̃N���C�A���gDS���}�b�`���C�N���L�����Z������ 
                    OS_TPrintf("Host (friendListIndex = %d) canceled matching.\n\n", index);
                }
            }
        }
    }
    else {
        OS_TPrintf("Failed to join the game. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // �u���b�L���O���� 
}

/*---------------------------------------------------------------------------*
  �V�K�ڑ��N���C�A���g�ʒm�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void GenericNewClientCallback(int index, void* param )
{
    (void)param;

    OS_TPrintf( "New Client is connecting(idx %d)...\n", index);
}

/*---------------------------------------------------------------------------*
  �X�g���[�W�T�[�o�ւ̃��O�C�������R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void LoginToStorageCallback(DWCError error, void* param)
{
#pragma unused(param)

    OS_TPrintf("Login to storage server: result %d\n", error);
}
