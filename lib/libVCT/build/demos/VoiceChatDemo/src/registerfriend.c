#include <nitro.h>
#include <dwc.h>

#include "main.h"
#include "init.h"
#include "utility.h"
#include "wc.h"
#include "screen.h"
#include "dbs.h"
#include "mp.h"

#include "registerfriend.h"

extern DWCUserData stUserData; //  ���[�U�f�[�^���i�[����\����  
extern GameSequence gameSeqList[GAME_MODE_NUM];
extern GameControl stGameCnt;  // �Q�[��������\���� 
extern KeyControl stKeyCnt;    // �L�[���͐���\����  
extern u64  friend_key;

// �F�B�o�^�� 
static char stFriendKey[sizeof(DWCFriendData)];

// �T���v��MP�����p�F�B�f�[�^ 
static DWCFriendData stSampleFriendData ATTRIBUTE_ALIGN(32);

// �f�[�^�ʐM�p�o�b�t�@ 
static u16* gSendBuf ATTRIBUTE_ALIGN(32);//���M�p�o�b�t�@ 

/*---------------------------------------------------------------------------*
  �F�B�o�^���C���֐�
  *---------------------------------------------------------------------------*/
GameMode GameRegisterFriendMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_REG_FRIEND];
    GameMode returnSeq = GAME_MODE_NUM;
    RegFriendParam cntParam;
    int curIdx = 0;
    int friendIdx;
    int maxFigure;
    int validFlg;

    int i, j;

    cntParam.step   = 0;
    cntParam.figure = 0;

    MI_CpuFill8( stFriendKey, '0', sizeof(stFriendKey));
    
    while (1){
		{
			char friendKeyString[DWC_FRIENDKEY_STRING_BUFSIZE];
			int i, x;
			DWC_FriendKeyToString(friendKeyString, friend_key);
	        dbs_Print( 0, 0, "friendkey:xxxx-xxxx-xxxx");
	        x = 10;
	        for(i = 0; i < 12; i++) {
				dbs_Print(x, 0, "%c", friendKeyString[i]);
				x++;
				if(i == 3 || i == 7) {
			        dbs_Print(x, 0, "-");
					x++;
				}
			}
		}
        ReadKeyData();// �L�[�f�[�^�擾 

        // ���ɐi�ނׂ����[�h���Z�b�g����Ă����甲���� 
        if (returnSeq != GAME_MODE_NUM){
            break;
        }
        
        ////////// �ȉ��L�[���쏈�� 
        if (cntParam.step == 0){
            // �R�}���h�I�� 
            if (stKeyCnt.trg & PAD_BUTTON_A){
	            // A�{�^���Ń��j���[���� 
                returnSeq = gameSeq->menuList[curIdx].mode;
                stGameCnt.blocking = gameSeq->menuList[curIdx].blocking;

                if ((curIdx == 0) || (curIdx == 1)){
                    // �F�B�o�^�����͂ɂ��F�B�o�^
                    cntParam.step   = 1;
                    cntParam.figure = 0;
                    cntParam.value  = 0;
                    
                    // ���j���[���X�g�ĕ\�� 
                    DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
                    
                    if (curIdx == 0){
                        maxFigure = 12;  // �F�B���͍ō��P�Q�� 
                    }
                    else {
                        maxFigure = 2;   // �F�B���X�g�C���f�b�N�X�͍ō��Q�� 
                    }
                }
                else if ( curIdx == 2 )
                {
					// MP�ʐM�ɂ��F�B������
					OS_TPrintf( "MP\n" );

		            // ���������� 
                    DWC_CleanupInet();
                    
		            // MP�ʐM�p�f�[�^�쐬 
                    DWC_CreateExchangeToken(&stUserData, &stSampleFriendData);
                    gSendBuf = (u16*)(&stSampleFriendData);
                    
                    mp_match(gSendBuf, stGameCnt.friendList.keyList);//MP communications

                    friendIdx = GetAvailableFriendListIndex();// �����MP�f�[�^���Ȃ� 
                    for(i = 0; i < friendIdx; i++){
                        for(j = i+1; j < friendIdx; j++){
                            if(DWC_IsEqualFriendData( (const DWCFriendData*)&stGameCnt.friendList.keyList[i], (const DWCFriendData*)&stGameCnt.friendList.keyList[j])){
                                MI_CpuClear8(&stGameCnt.friendList.keyList[j], sizeof(DWCFriendData));
                            }
                        }
                    }
                    // ���j���[���X�g�ĕ\�� 
                    DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
                }
            }
            else if (stKeyCnt.trg & PAD_KEY_UP){
	            // �\���L�[��ŃJ�[�\���ړ� 
                curIdx--;
                if (curIdx < 0) curIdx = gameSeq->numMenu-1;
	            // ���j���[���X�g�ĕ\�� 
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_DOWN){
	            // �\���L�[���ŃJ�[�\���ړ� 
                curIdx++;
                if (curIdx >= gameSeq->numMenu) curIdx = 0;
	            // ���j���[���X�g�ĕ\�� 
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
        }
        else {
	        // ���l���͒� 
            if (stKeyCnt.trg & PAD_BUTTON_A){
                // A�{�^���ŗF�B�̓o�^�E�폜���s�� 
                if (curIdx == 0){
                    // �F�B�o�^ 
                    if( DWC_CheckFriendKey( &stUserData, DWC_StringToFriendKey(stFriendKey)) )
                    {
//                        DWCFriendData* s_FriendList;
                        int friendIdx = -1;

                        // �F�B�o�^���ɂ��F�B�o�^ 
                        friendIdx = GetAvailableFriendListIndex();
                        DWC_CreateFriendKeyToken(&stGameCnt.friendList.keyList[friendIdx], 
                                		DWC_StringToFriendKey(stFriendKey));
                        validFlg = 1;
                    }
                    else {
                        OS_TPrintf( "Invalid Friend Code!\n" );
                        validFlg = 0;  // �v���t�@�C��ID=0�͖��� 
                    }
                }
                else {
                    // �F�B�폜
                    int friendIdx = (int)((stFriendKey[0]-48)*10+(stFriendKey[1]-48));
                    if ((friendIdx < GAME_MAX_FRIEND_LIST)&&
                        DWC_CanChangeFriendList()){
                        // �F�B���X�g����F�B���폜 
                        DWC_DeleteBuddyFriendData(&stGameCnt.friendList.keyList[friendIdx]);
                        validFlg = 1;
                    }
                    else {
                        validFlg = 0;  // �F�B���X�g�͈͊O 
                    }
                }

                if (validFlg){
                    // �L���ȓ��͂ł���΃R�}���h�I���ɖ߂� 
                    cntParam.step = 0;
                    // ���j���[���X�g�ĕ\�� 
                    DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
                }
            }
            else if (stKeyCnt.trg & PAD_BUTTON_B){
                MI_CpuFill8( stFriendKey, '0', sizeof(stFriendKey));
                // B�{�^���ŃR�}���h�I���ɖ߂� 
                cntParam.step = 0;
                // ���j���[���X�g�ĕ\�� 
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_UP){
                // �\���L�[�㉺�őI�𒆂̐��l�̑��� 
                stFriendKey[cntParam.figure]++;
                if(stFriendKey[cntParam.figure] > '9'){
                    stFriendKey[cntParam.figure] = '0';
                }
                // ���j���[���X�g�ĕ\�� 
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_DOWN){
                // �\���L�[�㉺�őI�𒆂̐��l�̑��� 
                stFriendKey[cntParam.figure]--;
                if(stFriendKey[cntParam.figure] < '0'){
                    stFriendKey[cntParam.figure] = '9';
                }
                // ���j���[���X�g�ĕ\�� 
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_LEFT){
                // �\���L�[���E�őI�������̈ړ� 
                if (cntParam.figure > 0 && cntParam.figure <= maxFigure-1 ) cntParam.figure--;
                // ���j���[���X�g�ĕ\�� 
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_RIGHT){
                // �\���L�[���E�őI�������̈ړ� 
                if (cntParam.figure >= 0 && cntParam.figure < maxFigure-1 ) cntParam.figure++;
                // ���j���[���X�g�ĕ\�� 
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
        }
        ////////// �L�[���쏈�������܂� 

        // V�u�����N�҂�����
        GameWaitVBlankIntr();
    }

    return returnSeq;
}

/*---------------------------------------------------------------------------*
  �F�B�o�^�E�������[�h�̒ǉ��\���p�R�[���o�b�N�֐� 
 *---------------------------------------------------------------------------*/
void RegFriendModeDispCallback(int curIdx, void* param)
{
    RegFriendParam cntParam;
    int i;

    if (!param){
        // �����param = NULL�ŌĂ΂�� 
        cntParam.step = 0;
    }
    else {
        cntParam = *(RegFriendParam *)param;
    }

    OS_TPrintf("\n");

    if (cntParam.step == 0){
        // �R�}���h�I�𒆂͗F�B���X�g��\�� 
        DispFriendList(TRUE);
    }
    else {
        // ���l���͒� 
        if (curIdx == 0){
            // �F�B�o�^�B�v���t�@�C��ID���͒� 
            OS_TPrintf("input> ");
            for(i = 0; i < 12; i++){
                OS_TPrintf("%c", stFriendKey[i]);
                if (i==3 || i==7)
                    OS_TPrintf("-");
            }
            OS_TPrintf("\n");

            // �I�𒆂̐����������ׂ̉�����\������ 
            for (i = 0; i < cntParam.figure; i++){
                OS_TPrintf(" ");
            }
            if(cntParam.figure > 3)
            	OS_TPrintf(" ");
            	
            if(cntParam.figure > 7)
            	OS_TPrintf(" ");

            OS_TPrintf("       -\n");
        }
        else {
            // �F�B�폜�B�F�B���X�g�C���f�b�N�X���͒� 
            DispFriendList(TRUE);  // �F�B���X�g��\�� 
            
            OS_TPrintf("Delete Index : ");
            for(i = 0; i < 2; i++){
                OS_TPrintf("%c", stFriendKey[i]);
            }
            OS_TPrintf("\n");

            for (i = 0; i < cntParam.figure; i++){
                OS_TPrintf(" ");
            }
            OS_TPrintf("               -\n");
        }
    }
}

