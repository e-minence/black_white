#include <nitro.h>
#include <dwc.h>

#include "main.h"
#include "init.h"
#include "utility.h"
#include "dbs.h"

#include "gamemain.h"

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
#define FILTER_STRING "sample_filter"	// �t�B���^������. �K�X�ύX���Ă�������
#define FILTER_KEY "str_key"
#define APP_CONNECTION_KEEPALIVE_TIME 300000 // �L�[�v�A���C�u����
#define KEEPALIVE_INTERVAL (APP_CONNECTION_KEEPALIVE_TIME/5) // �L�[���͂�҂����f�[�^��]�����鎞��

//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
extern GameSequence gameSeqList[GAME_MODE_NUM];
extern GameControl stGameCnt;  // �Q�[��������\���� 
extern KeyControl stKeyCnt;    // �L�[���͐���\���� 

extern DWCUserData stUserData;  // �{�̌ŗL�̃��[�UID(�{���̓��[�U�̓��͂ł�������Q�[�����ƂɊ��蓖�Ă�����̂ł������肷��l)
extern GameSaveData saveData;  
extern char addFilter[128];
extern u32  s_groupID;
u64 friend_key;

//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static void LoginCallback(DWCError error, int profileID, void* param);
static void UpdateServersCallback(DWCError error, BOOL isChanged, void* param);
static void FriendStatusCallback(int index, u8 status, const char* statusString, void* param);
static void DeleteFriendListCallback(int deletedIndex, int srcIndex,void* param);
static void SaveToServerCallback(BOOL success, BOOL isPublic, void* param);
static void LoadFromServerCallback(BOOL success, int profileID, char* data, int len, void* param);
static void BuddyFriendCallback(int index, void* param);
/*---------------------------------------------------------------------------*
  �I�t���C�������C���֐�
 *---------------------------------------------------------------------------*/
GameMode GameMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_MAIN];
    GameMode returnSeq = GAME_MODE_NUM;
    int curIdx = 0;

    while (1){
        dbs_Print( 0, 0, "s:%d", DWC_GetMatchState() );
        dbs_Print( 7, 0, "n:%d", DWC_GetNumConnectionHost() );
        dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel() );
        dbs_Print( 10,1, "p:%d", stGameCnt.userData.profileID );
	    dbs_Print( 30, 0, "%c",  LoopChar());
        dbs_Print( 20, 2, "g:%d", s_groupID );

        ReadKeyData();  // �L�[�f�[�^�擾 

        DWC_ProcessFriendsMatch();  // DWC�ʐM�����X�V 

        HandleDWCError(&returnSeq);  // �G���[����

        if (stGameCnt.blocking){
            // DWC�������̓L�[������֎~����

            // V�u�����N�҂�����
            GameWaitVBlankIntr();
            continue;
        }

        // ���ɐi�ނׂ����[�h���Z�b�g����Ă�����F�؊���
        if (returnSeq == GAME_MODE_LOGIN){
            // ���O�C�����I������Ă����ꍇ
            if (stGameCnt.userData.isValidProfile){
               // �F�ؐ����Ńv���t�@�C��ID���擾�ł����ꍇ�̓��[�v�𔲂���
                 break;
            }
            else {
                // �F�؎��s�̏ꍇ�̓��[�h��i�߂Ȃ��悤�ɂ���
                returnSeq = GAME_MODE_NUM;
            }
        }
        else if (returnSeq != GAME_MODE_NUM){
            // ����ȊO�̏ꍇ�͕K��������
            break;
        }
            
        ////////// �ȉ��L�[���쏈��
        if (stKeyCnt.trg & PAD_BUTTON_A){
           // A�{�^���Ń��j���[����
            returnSeq = gameSeq->menuList[curIdx].mode;
            stGameCnt.blocking = gameSeq->menuList[curIdx].blocking;

            switch (returnSeq){
            case GAME_MODE_LOGIN:  // ���O�C�� 
                if (DWC_CheckHasProfile(&stUserData))
                {
                    OS_TPrintf("DWC_CheckHasProfile:TRUE\n");
                    if (DWC_CheckValidConsole(&stUserData))
                    {
                        OS_TPrintf("DWC_CheckValidConsole:TRUE\n");
                    }
                    else
                    {
                        OS_TPrintf("DWC_CheckValidConsole:FALSE\n");
                        // ���̂܂܃��O�C�������ɐi�߂Ă��F�؃G���[�ɂȂ邽�߁A
                        // �ʏ�͂��̂܂ܐ�ɐi�߂Ă͂����܂���B
                    }
                }
                else
                {
                    OS_TPrintf("DWC_CheckHasProfile:FALSE\n");
                }
                DWC_InitFriendsMatch(&stUserData, GAME_PRODUCTID,
                                     GAME_SECRET_KEY, 0, 0,
                                     stGameCnt.friendList.keyList, GAME_MAX_FRIEND_LIST);

                // �F�ؗp�֐����g���ă��O�C��
                if (!DWC_LoginAsync(L"Name", NULL, LoginCallback, &returnSeq)){
                    // ���O�C�����J�n�ł��Ȃ������ꍇ�̓u���b�L���O��Ԃɂ��Ȃ��B
                    // �G���[����������d���O�C���łȂ����FALSE�ɂ͂Ȃ�Ȃ��B
                    OS_TPrintf("Can't call DWC_LoginAsync().\n");
                    returnSeq          = GAME_MODE_NUM;
                    stGameCnt.blocking = 0;
                }
                break;
            default:
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
        else if ( stKeyCnt.trg & PAD_BUTTON_Y){
            Heap_Dump();
        }
        ////////// �L�[���쏈�������܂�

        // V�u�����N�҂�����
        GameWaitVBlankIntr();
    }

    return returnSeq;
}

/*---------------------------------------------------------------------------*
  ���O�C���R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void LoginCallback(DWCError error, int profileID, void *param)
{
    BOOL result;
	RTCTime time;
	RTCDate date;
    
    if (error == DWC_ERROR_NONE){
        // �t�B���^��������쐬����
        char* filterString = FILTER_STRING;
		OS_SNPrintf(addFilter, sizeof(addFilter), "%s='%s'", FILTER_KEY, filterString);

        // �F�ؐ����A�v���t�@�C��ID�擾
        OS_TPrintf("Login succeeded. profileID = %u\n\n", profileID);

        // ingamesn�`�F�b�N�̌��ʂ��擾����
		if(DWC_GetIngamesnCheckResult() == DWC_INGAMESN_INVALID)
			OS_TPrintf("BAD ingamesn is detected by NAS.\n");
		else if(DWC_GetIngamesnCheckResult() == DWC_INGAMESN_VALID)
			OS_TPrintf("GOOD ingamesn is detected by NAS.\n");
		else
			OS_TPrintf("ingamesn is not checked yet.\n");
		
		// �F�؃T�[�o�̎�����\������
		if(DWC_GetDateTime(&date, &time) == TRUE)
			OS_TPrintf("NasTime = %02d/%02d/%02d %02d:%02d:%02d\n", date.year+2000, date.month, date.day, time.hour, time.minute, time.second);
		else
			DWC_Printf(DWC_REPORTFLAG_AUTH, "DWC_GetNasTime failed\n");


        // stUserData���X�V����Ă��邩�ǂ������m�F�B
        if ( DWC_CheckDirtyFlag( &stUserData ) )
        {
            DWC_ClearDirtyFlag( &stUserData );

            OS_TPrintf("*******************************\n");
            OS_TPrintf("You must save the DWCUserData!!\n");
            OS_TPrintf("*******************************\n");

            // �K�����̃^�C�~���O�Ń`�F�b�N���āAdirty flag���L���ɂȂ��Ă�����A
            // DWCUserData��DS�J�[�h�̃o�b�N�A�b�v�ɕۑ�����悤�ɂ��Ă��������B
            // saveUserDataToDSCard( &stUserData );
        }
        
        stGameCnt.userData.profileID = profileID;
        stGameCnt.userData.isValidProfile = TRUE;


        // �F�B�o�^���쐬
        friend_key = DWC_CreateFriendKey( &stUserData );
		if( friend_key ){
            OS_TPrintf("friend_key = %lld\n", friend_key);
        }
        
        // �F�B���X�g���������J�n
        result = DWC_UpdateServersAsync(NULL,
                                        UpdateServersCallback, param,
                                        FriendStatusCallback, param,
                                        DeleteFriendListCallback, param);
        if (result == FALSE){
           // �Ă�ł�������ԁi���O�C�����������Ă��Ȃ���ԁj�ŌĂ񂾎��̂�
            // FALSE���Ԃ��Ă���̂ŁA���ʂ�TRUE
            OS_Panic("--- DWC_UpdateServersAsync error teminated.\n");
            *(GameMode *)param = GAME_MODE_NUM;
            stGameCnt.blocking  = 0;
        }
        else {
            // GameSpy�T�[�o��o�f�B�����R�[���o�b�N��o�^����
            DWC_SetBuddyFriendCallback(BuddyFriendCallback, NULL);

            // �X�g���[�W�T�[�o�Z�[�u�E���[�h�����ʒm�R�[���o�b�N��o�^����
            DWC_SetStorageServerCallback(SaveToServerCallback,
                                         LoadFromServerCallback);
        }

		// �L�[�v�A���C�u���Ԃ̐ݒ�        
        DWC_SetConnectionKeepAliveTime(APP_CONNECTION_KEEPALIVE_TIME);

    }
    else {
        // �F�؎��s
        OS_TPrintf("Login failed. %d\n\n", error);
        stGameCnt.blocking = FALSE;  // �u���b�L���O����
    }
}

/*---------------------------------------------------------------------------*
  GameSpy�o�f�B�����R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void BuddyFriendCallback(int index, void* param)
{
#pragma unused(param)

    OS_TPrintf("I was authorized by friend[%d].\n", index);
}


/*---------------------------------------------------------------------------*
  �F�B���X�g�������������R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void UpdateServersCallback(DWCError error, BOOL isChanged, void* param)
{
#pragma unused(param)

    if (error == DWC_ERROR_NONE){
        // �F�B���X�g������������
        OS_TPrintf("Updating servers succeeded.\n");

        if (isChanged){
            // �F�B���X�g���ύX����Ă�����Z�[�u����
            // �i�Z�[�u�̂���j
            MI_CpuCopy32(&stGameCnt.friendList, &saveData.friendList, sizeof(GameFriendList));
        }

        // �F�B���X�g��\������
        DispFriendList(TRUE);
    }
    else {
        // ���s���Ă����ɉ������Ȃ�
        OS_TPrintf("Failed to update GameSpy servers. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // �u���b�L���O���� 
}

/*---------------------------------------------------------------------------*
   �F�B��ԕω��ʒm�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void FriendStatusCallback(int index, u8 status, const char* statusString, void* param)
{
#pragma unused(param)
#ifdef SDK_FINALROM
#pragma unused(index, status, statusString)
#endif

    OS_TPrintf("Friend[%d] changed status -> %d (statusString : %s).\n",
               index, status, statusString);
}


/*---------------------------------------------------------------------------*
  �F�B���X�g�폜�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void DeleteFriendListCallback(int deletedIndex, int srcIndex, void* param)
{
#pragma unused(param)
#ifdef SDK_FINALROM
#pragma unused(deletedIndex, srcIndex)
#endif
#if 0
     // �F�B�f�[�^�̍폜�E�O�l�߂ɍ��킹�āA����ɑΉ�����f�[�^���O�ɋl�߂�
    if (index < GAME_MAX_FRIEND_LIST-1){
        // �F�B�̒ʐM��Ԃ�O�ɋl�߂�
        MI_CpuCopy8(&stGameCnt.friendStatus[index+1], &stGameCnt.friendStatus[index],
                    (u32)(GAME_MAX_FRIEND_LIST-index-1));

        // �F�B�̒ʐM��Ԃ�O�ɋl�߂�
        MI_CpuCopy8(&stGameCnt.friendList.playerName[index+1],
                    &stGameCnt.friendList.playerName[index],
                    (u32)(GAME_MAX_PLAYER_NAME*(GAME_MAX_FRIEND_LIST-index-1)));
    }

    stGameCnt.friendStatus[GAME_MAX_FRIEND_LIST-1] = DWC_STATUS_OFFLINE;
    MI_CpuClear32(&stGameCnt.friendList.playerName[GAME_MAX_FRIEND_LIST-1],
                  GAME_MAX_PLAYER_NAME);

#else
    OS_TPrintf("friend[%d] was deleted (equal friend[%d]).\n",
               deletedIndex, srcIndex);
#endif
}


/*---------------------------------------------------------------------------*
  �X�g���[�W�T�[�o�ւ̃f�[�^�Z�[�u�����R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void SaveToServerCallback(BOOL success, BOOL isPublic, void* param)
{
#pragma unused(param)

    OS_TPrintf("Saved data to server.\n");
    OS_TPrintf("result %d, isPublic %d.\n", success, isPublic);
}


/*---------------------------------------------------------------------------*
  �X�g���[�W�T�[�o����̃f�[�^���[�h�����R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void LoadFromServerCallback(BOOL success, int profileID, char* data, int len, void* param)
{
#pragma unused(param)

    OS_TPrintf("Saved data to server.\n");
    OS_TPrintf("result %d, index %d, data '%s', len %d\n",
               success, profileID, data, len);
}


