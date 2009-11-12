/*--------------------------------------------------------------------------
  Project:  sc_sample
  File:     sctest.c
 
  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009/10/05#$
  $Rev:  $
  $Author: niwa_kazutomo $
  *-------------------------------------------------------------------------*/

#include "sctest.h"
#include "atlas_ninTest1_v1.h"

//----------------------------------
// define
//----------------------------------

#define GAME_ID      2502       // �Q�[��ID
#define TIMEOUT_MS   100  // HTTP�ʐM�̃^�C���A�E�g����
#define WAIT_MS      50         // �R�[���o�b�N���A���Ă������|�[�����O����E�F�C�g����
#define RESEND_MS    1000       // �f�[�^�����̍đ��M����܂ł̖��ʐM����
#define PLAYER_NUM   2          // �v���C���[��
#define TEAM_NUM     0          // �`�[����

//----------------------------------
// prototype
//----------------------------------

/**
 * �Z�b�V�����쐬�R�[���o�b�N�֐��B
 *
 * Param:  theResult ����
 * Param:  theUserData ���[�U��`�f�[�^
 */
void ScCreateSessionCallback( DWCScResult theResult, void* theUserData );

/**
 * ���|�[�g���M�ӎv�ʒm�R�[���o�b�N�֐��B
 *
 * Param:  theResult ����
 * Param:  theUserData ���[�U��`�f�[�^
 */
void ScSetReportIntentionCallback( DWCScResult theResult, void* theUserData );

/**
 * ���|�[�g���M�R�[���o�b�N�֐��B
 *
 * Param:  theResult ����
 * Param:  theUserData ���[�U��`�f�[�^
 */
void ScSubmitReportCallback(DWCScResult theResult, void* theUserData);

/**
 * �}�b�`���C�N��������Ɏ����̃v���C���[�f�[�^�𑗐M����B
 */
void ScSendPlayerData();

/**
 * �}�b�`���C�N��������̃v���C���[�f�[�^����M����B
 */
void ScReceivePlayerData();

/**
 * �}�b�`���C�N��������Ƀf�[�^�𑗐M����B
 *
 * Param:  data ���M����f�[�^
 * Param:  size ���M����f�[�^�̃T�C�Y
 */
void ScSendData( const void* data, int size );

//----------------------------------
// enum
//----------------------------------

/**
 * �ʐM�g�[�N��
 */
typedef enum CommunicateToken
{
    TOKEN_START,    // �N���C�A���g���[�h�ł̓���J�n��v������
    TOKEN_ACK       // ��M��������
} CommunicateToken;

/**
 * �X�e�[�^�X
 */
typedef enum ScStatus
{
    STATUS_NONE,            // ��������
    STATUS_INITIALIZED,     // �������ς�
    STATUS_CREATE_SESSION,  // �Z�b�V�����쐬�ς�
    STATUS_CREATE_REPORT,   // ���|�[�g�쐬�ς�
    STATUS_SUBMIT_REPORT,   // ���|�[�g���M�ς�
    STATUS_DESTROY          // �I���ς�
} ScStatus;

//----------------------------------
// struct
//----------------------------------

/**
 * ���[�L���O������
 */
typedef struct ScWork
{
    ScStatus status;

    PlayerData myPlayerData;
    PlayerData otherPlayerData;
    int waitCount;

    BOOL startCompetition;
    BOOL ackReceived;
    BOOL otherPlayerDataReceived;
    BOOL error;
} ScWork;

//----------------------------------
// variable
//----------------------------------

static ScWork work;

//----------------------------------
// function
//----------------------------------

/**
 * ����������
 *
 * Return value:  ����������
 */
BOOL ScInitialize()
{
    work.status = STATUS_NONE;

    work.startCompetition = FALSE;
    work.ackReceived = FALSE;
    work.otherPlayerDataReceived = FALSE;
    work.error = FALSE;

    return TRUE;
}

/**
 * ���C������
 *
 * Param:  host �z�X�g���[�h�œ��삷�邩
 * Return value:  ����������
 */
BOOL ScMain( BOOL host )
{
    if( host )
    {
        do {
            u8 data = (u8)TOKEN_START;
            ScSendData( &data, sizeof(u8) );
            OS_Sleep( RESEND_MS );
            DWC_ProcessFriendsMatch();
        } while( !work.ackReceived );
        work.ackReceived = FALSE;
    }

    // ���C�u������������
    work.waitCount = 0;
    if( DWC_ScInitialize( GAME_ID ) != DWC_SC_RESULT_NO_ERROR )
    {
        ScFinalize();
        return FALSE;
    }
    work.status = STATUS_INITIALIZED;

    if( DWC_ScGetLoginCertificate( &work.myPlayerData.mCertificate ) != DWC_SC_RESULT_NO_ERROR )
    {
        ScFinalize();
        return FALSE;
    }

    if( host )
    {
        // �z�X�g�̏ꍇ�̃R���y�e�B�V��������������
        work.waitCount++;
        if( DWC_ScCreateSessionAsync( ScCreateSessionCallback, TIMEOUT_MS, NULL ) != DWC_SC_RESULT_NO_ERROR )
        {
            ScFinalize();
            return FALSE;
        }
        while( work.waitCount > 0 ){ OS_Sleep( WAIT_MS ); DWC_ScProcess(); }
        if( work.error )
        {
            ScFinalize();
            return FALSE;
        }
        work.status = STATUS_CREATE_SESSION;
        MI_CpuCopy( DWC_ScGetSessionId(), work.myPlayerData.mSessionId, DWC_SC_SESSION_GUID_SIZE );
    }

    MI_CpuCopy( DWC_ScGetConnectionId(), work.myPlayerData.mConnectionId, DWC_SC_CONNECTION_GUID_SIZE );

    if( host )
    {
        // �ΐ푊��Ƀv���C���[���𑗐M����
        ScSendPlayerData();
    }
    else
    {
        // �ΐ푊�肩��v���C���[�����擾����
        ScReceivePlayerData();

        //�z�X�g�ȊO�̐l�́A�z�X�g��CreateSession����sessionId��ݒ肷��
        if( DWC_ScSetSessionId( work.otherPlayerData.mSessionId ) != DWC_SC_RESULT_NO_ERROR )
        {
            ScFinalize();
            return FALSE;
        }
    }

    // ���M���郌�|�[�g�̃^�C�v���o�b�N�G���h�ɒʒm
    work.waitCount++;
    if( DWC_ScSetReportIntentionAsync( host, ScSetReportIntentionCallback, TIMEOUT_MS, &work.myPlayerData ) != DWC_SC_RESULT_NO_ERROR )
    {
        ScFinalize();
        return FALSE;
    }
    while( work.waitCount > 0 ){ OS_Sleep( WAIT_MS ); DWC_ScProcess(); }
    if( work.error )
    {
        ScFinalize();
        return FALSE;
    }

    if( !host )
    {
        // �ΐ푊��Ƀv���C���[���𑗐M����
        ScSendPlayerData();
    }
    else
    {
        // �ΐ푊�肩��v���C���[�����擾����
        ScReceivePlayerData();
    }

    // ���|�[�g���쐬
    if( DWC_ScCreateReport( ATLAS_RULE_SET_VERSION, PLAYER_NUM, TEAM_NUM, &work.myPlayerData.mReport ) != DWC_SC_RESULT_NO_ERROR )
    {
        ScFinalize();
        return FALSE;
    }
    work.status = STATUS_CREATE_REPORT;

    // ���|�[�g�ɃO���[�o���f�[�^����������
    if( DWC_ScReportBeginGlobalData( work.myPlayerData.mReport ) != DWC_SC_RESULT_NO_ERROR )
    {
        ScFinalize();
        return FALSE;
    }

    // ���|�[�g�Ƀv���C���[�f�[�^����������
    if( DWC_ScReportBeginPlayerData( work.myPlayerData.mReport ) != DWC_SC_RESULT_NO_ERROR )
    {
        ScFinalize();
        return FALSE;
    }

    {
        // �����̃��|�[�g���쐬
        if( DWC_ScReportBeginNewPlayer( work.myPlayerData.mReport ) != DWC_SC_RESULT_NO_ERROR )
        {
            ScFinalize();
            return FALSE;
        }

        if( DWC_ScReportSetPlayerData(
                work.myPlayerData.mReport,
                (u32)(host ? 0 : 1),
                work.myPlayerData.mConnectionId,
                0,
                host ? DWC_SC_GAME_RESULT_WIN : DWC_SC_GAME_RESULT_LOSS,
                work.myPlayerData.mCertificate,
                work.myPlayerData.mStatsAuthdata) != DWC_SC_RESULT_NO_ERROR )
        {
            ScFinalize();
            return FALSE;
        }
    }

    {
        // ����v���C���[�̃��|�[�g���쐬
        if( DWC_ScReportBeginNewPlayer( work.myPlayerData.mReport ) != DWC_SC_RESULT_NO_ERROR )
        {
            ScFinalize();
            return FALSE;
        }

        if( DWC_ScReportSetPlayerData(
                work.myPlayerData.mReport,
                (u32)(host ? 0 : 1),
                work.otherPlayerData.mConnectionId,
                0,
                host ? DWC_SC_GAME_RESULT_LOSS : DWC_SC_GAME_RESULT_WIN,
                work.otherPlayerData.mCertificate,
                work.otherPlayerData.mStatsAuthdata) != DWC_SC_RESULT_NO_ERROR )
        {
            ScFinalize();
            return FALSE;
        }
    }

    // ���|�[�g�Ƀ`�[���f�[�^����������
    if( DWC_ScReportBeginTeamData( work.myPlayerData.mReport ) != DWC_SC_RESULT_NO_ERROR )
    {
        ScFinalize();
        return FALSE;
    }

    // ���|�[�g�̏I�[��錾����
    if( DWC_ScReportEnd( work.myPlayerData.mReport, host, DWC_SC_GAME_STATUS_COMPLETE ) != DWC_SC_RESULT_NO_ERROR )
    {
        ScFinalize();
        return FALSE;
    }

    // ���|�[�g�𑗐M����
    work.waitCount++;
    if( DWC_ScSubmitReportAsync(
            work.myPlayerData.mReport,
            host,
            ScSubmitReportCallback,
            TIMEOUT_MS,
            NULL) != DWC_SC_RESULT_NO_ERROR )
    {
        ScFinalize();
        return FALSE;
    }
    while( work.waitCount > 0 ){ OS_Sleep( WAIT_MS ); DWC_ScProcess(); }
    if( work.error )
    {
        ScFinalize();
        return FALSE;
    }
    work.status = STATUS_SUBMIT_REPORT;

    ScFinalize();
    return TRUE;
}

/**
 * �I������
 */
void ScFinalize()
{
    if( work.status >= STATUS_CREATE_REPORT )
    {
        OS_TPrintf( "destroy report\n" );
        DWC_ScDestroyReport( work.myPlayerData.mReport );
    }

    if( work.status >= STATUS_INITIALIZED )
    {
        OS_TPrintf( "sc shutdown\n" );
        DWC_ScShutdown();
    }
    work.status = STATUS_DESTROY;
}

/**
 * �Z�b�V�����쐬�R�[���o�b�N�֐��B
 *
 * Param:  theResult ����
 * Param:  theUserData ���[�U��`�f�[�^
 */
void ScCreateSessionCallback( DWCScResult theResult, void* theUserData )
{
    (void)theUserData;

    if (theResult == DWC_SC_RESULT_NO_ERROR)
    {
        OS_TPrintf( "[CreateSessionCallback]Session ID: %s\n", DWC_ScGetSessionId() );
        OS_TPrintf( "[CreateSessionCallback]Connection ID: %s\n", DWC_ScGetConnectionId() );
    }
    else
    {
        work.error = TRUE;
    }

    work.waitCount--;
}

/**
 * ���|�[�g���M�ӎv�ʒm�R�[���o�b�N�֐��B
 *
 * Param:  theResult ����
 * Param:  theUserData ���[�U��`�f�[�^
 */
void ScSetReportIntentionCallback(DWCScResult theResult, void* theUserData)
{
    PlayerData* myPlayerData = (PlayerData*)theUserData;

    if (theResult == DWC_SC_RESULT_NO_ERROR)
    {
        STD_CopyMemory( myPlayerData->mConnectionId, DWC_ScGetConnectionId(), DWC_SC_CONNECTION_GUID_SIZE );
        OS_TPrintf( "[SetReportIntentionCallback]Connection ID: %s\n", DWC_ScGetConnectionId() );
    }
    else
    {
        work.error = TRUE;
    }
    work.waitCount--;
}

/**
 * ���|�[�g���M�R�[���o�b�N�֐��B
 *
 * Param:  theResult ����
 * Param:  theUserData ���[�U��`�f�[�^
 */
void ScSubmitReportCallback(DWCScResult theResult, void* theUserData)
{
    (void)theUserData;

    if (theResult == DWC_SC_RESULT_NO_ERROR)
    {
        OS_TPrintf( "[SubmitReportCallback]success\n" );
    }
    else
    {
        work.error = TRUE;
    }
    work.waitCount--;
}

/**
 * �}�b�`���C�N��������Ɏ����̃v���C���[�f�[�^�𑗐M����B
 */
void ScSendPlayerData()
{
    OS_TPrintf( "wait to send PlayerData...\n" );
    do {
        ScSendData( &work.myPlayerData, sizeof(PlayerData) );
        OS_Sleep( RESEND_MS );
        DWC_ProcessFriendsMatch();
    } while( !work.ackReceived );
    work.ackReceived = FALSE;
    OS_TPrintf( "ok. PlayerData has received.\n" );
}

/**
 * �}�b�`���C�N��������̃v���C���[�f�[�^����M����B
 */
void ScReceivePlayerData()
{
    OS_TPrintf( "wait to revive PlayerData...\n" );
    while( !work.otherPlayerDataReceived )
    {
        OS_Sleep( WAIT_MS );
        DWC_ProcessFriendsMatch();
    }
    OS_TPrintf( "ok. PlayerData has received.\n" );
}

/**
 * �}�b�`���C�N��������Ƀf�[�^�𑗐M����B
 *
 * Param:  data ���M����f�[�^
 * Param:  size ���M����f�[�^�̃T�C�Y
 */
void ScSendData( const void* data, int size )
{
    u8* pAidList;
    int numHost;
    int i;

    numHost = DWC_GetAIDList(&pAidList);

    for (i = 0; i < numHost; i++){
        if (pAidList[i] == DWC_GetMyAID()) continue;

        DWC_SendReliable( pAidList[i], data, size );
    }
}

/**
 * DWC����̃f�[�^��M�R�[���o�b�N�֐��B
 *
 * Param:  aid �f�[�^���M��aid
 * Param:  buffer ��M�o�b�t�@�A�h���X
 * Param:  size ��M�T�C�Y
 * Param:  param �����f�[�^
 */
void ScReceiveDataEvent( u8 aid, u8* buffer, int size, void* param )
{
    (void)aid;
    (void)param;

    if ( size == sizeof(PlayerData) )
    {
        if( !work.otherPlayerDataReceived )
        {
            u8 data = (u8)TOKEN_ACK;
            ScSendData( &data, sizeof(u8) );

            MI_CpuCopy( (PlayerData*)buffer, &work.otherPlayerData, sizeof(PlayerData) );

            OS_TPrintf( "[ReceiveDataEvent]Session ID: %s\n", work.otherPlayerData.mSessionId );
            OS_TPrintf( "[ReceiveDataEvent]Connection ID: %s\n", work.otherPlayerData.mConnectionId );

            work.otherPlayerDataReceived = TRUE;
        }
    }
    else if ( size == sizeof(u8) )
    {
        if( ((u8*)buffer)[0] == TOKEN_START )
        {
            if( !work.startCompetition )
            {
                u8 data = (u8)TOKEN_ACK;
                ScSendData( &data, sizeof(u8) );

                work.startCompetition = TRUE;

                OS_TPrintf("COMPETITION START(CLIENT)\n");
                if( ScMain(FALSE) )
                {
                    OS_TPrintf("COMPETITION SUCCESS\n");
                }
                else
                {
                    int error;
                    DWC_GetLastError( &error );
                    OS_TPrintf("COMPETITION TEST ERROR : %d\n", error);
                }
            }
        }
        else if( ((u8*)buffer)[0] == TOKEN_ACK )
        {
            work.ackReceived = TRUE;
        }
    }
    else
    {
        OS_TPrintf( "invalid recv data size\n" );
    }
}
