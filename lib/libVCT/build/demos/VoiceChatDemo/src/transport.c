#include <nitro.h>
#include <nitro/os.h>
#include <dwc.h>

#include "main.h"
#include "init.h"
#include "utility.h"

#include "dbs.h"
#include "screen.h"
#include "sound.h"

#include "transport.h"
#include "demo_util.h"

extern GameSequence gameSeqList[GAME_MODE_NUM];
extern GameControl stGameCnt;  // �Q�[��������\���� 
extern KeyControl stKeyCnt;    // �L�[���͐���\���� 
extern u32  s_groupID;

/** -------------------------------------------------------------------------
  DWC Transport
  ---------------------------------------------------------------------------*/
const int		SIZE_SEND_BUFFER = 512;
const int		SIZE_RECV_BUFFER = 4 * 1024;

static u8		sSendBuffer[ SIZE_SEND_BUFFER ];
static u8		sRecvBuffer[ MAX_PLAYERS ][ SIZE_RECV_BUFFER ];

static int      stExchangeProfile = 0;

#define DEMO_EXCHANGE_MAGIC_TOKEN '_Vdm'

/** -------------------------------------------------------------------------
  DWC Transport�p�@��M�o�b�t�@��ݒ�  
  ---------------------------------------------------------------------------*/
void
SetRecvBuffer( void )
{
	int i;
    int j = 0;
	int connectingNum = DWC_GetNumConnectionHost()	;

    OS_TPrintf( "connection host = %d\n", connectingNum );
    
    for ( i = 0; i < connectingNum; ++i )
    {
		if ( i == DWC_GetMyAID() )
        {
            j++;
			continue;
        }

        DWC_SetRecvBuffer( (u8)i, &sRecvBuffer[i-j], SIZE_RECV_BUFFER );
    }
    // �O���[�vID ���ۑ�
    s_groupID = DWC_GetGroupID();
}

/** -------------------------------------------------------------------------
  ���M�����R�[���o�b�N  
  ---------------------------------------------------------------------------*/
static void
SendDoneCallback( int size, u8 aid, void* param )
{
#pragma unused(size, aid, param)
}

/*---------------------------------------------------------------------------*
  DWC ��M����
  ---------------------------------------------------------------------------*/
static void 
UserRecvCallback( u8 aid, u8* buffer, int size, void* param )
{
#pragma unused(param)
    BOOL flag;
    
    flag = VCT_HandleData(aid, buffer, size);

    if (flag == FALSE) {
        //
        // 
        // VCT_HandleData�̖߂�l��FALSE�̏ꍇ�AVoiceChat�ȊO�̃f�[�^�A�܂���
        // VoiceChat������������Ă��Ȃ���Ԃł��B
        //
        // �f���ł̓N���C�A���g�̏�Ԃ����Ƃ肵�Ă��܂�
        // 
        //
        client_info *info = (client_info*)buffer;
        if (info->magic != DEMO_EXCHANGE_MAGIC_TOKEN) {
            return;
        }

        UpdateClientInfo(aid, info);
    }
}

/*---------------------------------------------------------------------------*
  �ڑ������チ�C���֐�
 *---------------------------------------------------------------------------*/
GameMode GameConnectedMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_CONNECTED];
    GameMode returnSeq = GAME_MODE_NUM;
    int curIdx = 0;

    // ���|�[�g���x����������
    DWC_SetReportLevel(0);
    
    // ���M�R�[���o�b�N�̐ݒ�	
    DWC_SetUserSendCallback( SendDoneCallback, NULL ); 

    // ��M�R�[���o�b�N�̐ݒ�	
    DWC_SetUserRecvCallback( UserRecvCallback, NULL ); 
    
    // ��M�o�b�t�@�̐ݒ�	
    SetRecvBuffer();

    // �R�l�N�V�����N���[�Y�R�[���o�b�N��ݒ�
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback, &returnSeq);

#if 0
	// �p�P�b�g���X����ݒ�
	if ( DWC_GetMyAID() != 0 )
    {
		DWC_SetSendDrop( 30, 0 );
		DWC_SetRecvDrop( 30, 0 );        
    }
#endif
#if 0
    // �x����ݒ�i�P��: ms�j
	if ( DWC_GetMyAID() != 0 )
    {
		DWC_SetSendDelay( 500, 0 );
		DWC_SetRecvDelay( 500, 0 );        
    }
#endif    

    (void)PM_SetBackLight( PM_LCD_ALL, PM_BACKLIGHT_ON );
    OS_TPrintf("AID bitmap = %x\n", DWC_GetAIDBitmap());
    OS_TPrintf( "my aid = %d\n", DWC_GetMyAID() );

    // �v���t�B�[��������������
    //
    BroadCastStatus(DEMO_STATUS_JOIN);
    
    DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
    
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
            // DWC�������̓L�[������֎~����

            // V�u�����N�҂�����
            GameWaitVBlankIntr();
            continue;
	    }

        // ���ɐi�ނׂ����[�h���Z�b�g����Ă����烋�[�v�𔲂���
        if (DWC_GetNumConnectionHost() <= 1){
            DWC_CloseAllConnectionsHard();
            stGameCnt.blocking = 0;
            returnSeq = gameSeq->menuList[3].mode;
        }
        if (returnSeq != GAME_MODE_NUM) break;

        ////////// �ȉ��L�[���쏈��
        if (stKeyCnt.trg & PAD_BUTTON_A){
            // A�{�^���Ń��j���[����
            returnSeq = gameSeq->menuList[curIdx].mode;
            stGameCnt.blocking = gameSeq->menuList[curIdx].blocking;

            switch (curIdx) {
            case 3:
                DWC_CloseAllConnectionsHard();
                
                if (DWC_GetNumConnectionHost() <= 1){
                    // ���ɐڑ��z�X�g����0�Ȃ炷���ɏI�����A�R�[���o�b�N�͌Ă΂�Ȃ�
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
        ////////// �L�[���쏈�������܂�

        // V�u�����N�҂�����
        GameWaitVBlankIntr();
    }

    return returnSeq;
}

