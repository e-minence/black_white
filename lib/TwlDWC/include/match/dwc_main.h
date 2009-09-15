/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_MAIN_H_
#define DWC_MAIN_H_

#include <match/dwc_transport.h>

#ifdef __cplusplus
extern "C"
{
#endif

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------

/// �v���C���[���i�Q�[�����X�N���[���l�[���j�̍ő咷�i���C�h����25�����{"\0\0"�j
#define DWC_MAX_PLAYER_NAME 26

// �ߋ��Ɍ�����֐��錾���Ȃ���Ă����̂ł��̌݊�����ۂ��߂ɒ�`
#define DWC_CloseConnectionsBitmapFromServer DWC_CloseConnectionHardBitmapFromServer

//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
    /// DWC�l�b�g���[�N��ԗ񋓎q
    typedef enum
    {
        DWC_STATE_INIT = 0,         ///< �������
        DWC_STATE_AVAILABLE_CHECK,  ///< �Q�[���̗��p�\�󋵂��`�F�b�N��
        DWC_STATE_LOGIN,            ///< ���O�C��������
        DWC_STATE_ONLINE,           ///< ���O�C��������I�����C�����
        DWC_STATE_MATCHING,         ///< �}�b�`���C�N������
        DWC_STATE_CONNECTED,        ///< �}�b�`���C�N������A���b�V���^�l�b�g���[�N�\�z�ςݏ��
        DWC_STATE_NUM
    } DWCState;


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
    /**
     * �R�l�N�V�����N���[�Y�R�[���o�b�N�^
     *
     * �ʐM����Ƃ̐ڑ����ؒf���邽�тɌĂяo����܂��B
     *
     * DWC_SetConnectionClosedCallback�֐��Ŏw�肵�܂��B
     *
     * Param:    error   DWC�G���[���
     * Param:    isLocal TRUE:�������N���[�Y�����B
     *                  FALSE:���̒N�����N���[�Y�����B
     *                  �}�b�`���C�N���ɉ����̂Ȃ��z�X�g��ؒf���鎞��TRUE�ɂȂ�B
     * Param:    isServer    TRUE: �T�[�o�z�X�g���ؒf�������Ƃ������B
     *                            �������T�[�o��isLoacal = TRUE �̎���TRUE�ɂȂ�B
     *                            �n�C�u���b�h�^�������̓t�����b�V���^�̂Ƃ��̓T�[�o��オ��������B�X�^�[�^�̂Ƃ��͔������Ȃ��B
     *                      FALSE:�T�[�o�z�X�g�ȊO�̃z�X�g���ؒf�������Ƃ�����
     * Param:    aid     �N���[�Y�����v���C���[��AID
     * Param:    index   �N���[�Y�����v���C���[�̗F�B���X�g�C���f�b�N�X�B
     *                  �N���[�Y�����v���C���[���F�B�łȂ����-1�ƂȂ�B
     * Param:    param   �R�[���o�b�N�p�p�����[�^
     *
     * See also:   DWC_SetConnectionClosedCallback
     *
     */
    typedef void (*DWCConnectionClosedCallback)(DWCError error, BOOL isLocal, BOOL isServer, u8 aid, int index, void* param);


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------

    /// FriendsMatch����\����
    typedef void*   DWCFriendsMatchControl;


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------

    void    DWC_InitFriendsMatch        ( DWCFriendsMatchControl* dwccnt, DWCUserData* userdata, int productID, const char* gameName, const char* secretKey, int sendBufSize, int recvBufSize, DWCFriendData friendList[], int friendListLen );
    void    DWC_ShutdownFriendsMatch    ( void);
    void    DWC_ProcessFriendsMatch     ( void);
    BOOL    DWC_LoginAsync              ( const u16*  ingamesn, const char* reserved, DWCLoginCallback callback, void* param );
    BOOL    DWC_UpdateServersAsync      ( const char* playerName, DWCUpdateServersCallback updateCallback, void* updateParam, DWCFriendStatusCallback statusCallback, void* statusParam, DWCDeleteFriendListCallback deleteCallback, void* deleteParam );
    BOOL    DWC_ConnectToAnybodyAsync   ( DWCTopologyType topology, u8  maxEntry, const char* addFilter, DWCMatchedSCCallback matchedCallback, void* matchedParam, DWCNewClientCallback newClientCallback, void* newClientParam, DWCEvalPlayerCallback evalCallback, void* evalParam, DWCConnectAttemptCallback attemptCallback, u8* connectionUserData, void* attemptParam );
    BOOL    DWC_ConnectToFriendsAsync   ( DWCTopologyType topology, const u8 friendIdxList[], int friendIdxListLen, u8  maxEntry, DWCMatchedSCCallback matchedCallback, void* matchedParam, DWCNewClientCallback newClientCallback, void* newClientParam, DWCEvalPlayerCallback evalCallback, void* evalParam, DWCConnectAttemptCallback attemptCallback, u8* connectionUserData, void* attemptParam );
    BOOL    DWC_SetupGameServer         ( DWCTopologyType topology, u8  maxEntry, DWCMatchedSCCallback matchedCallback, void* matchedParam, DWCNewClientCallback newClientCallback, void* newClientParam, DWCConnectAttemptCallback attemptCallback, u8* connectionUserData, void* attemptParam );
    BOOL    DWC_ConnectToGameServerAsync( DWCTopologyType topology, int serverIndex, DWCMatchedSCCallback matchedCallback, void* matchedParam, DWCNewClientCallback newClientCallback, void* newClientParam, DWCConnectAttemptCallback attemptCallback, u8* connectionUserData, void* attemptParam );
    BOOL    DWC_ConnectToGameServerByGroupID( DWCTopologyType topology, u32 groupID, DWCMatchedSCCallback matchedCallback, void* matchedParam, DWCNewClientCallback newClientCallback, void* newClientParam, DWCConnectAttemptCallback attemptCallback, u8* connectionUserData, void* attemptParam);
    BOOL    DWC_SetConnectionClosedCallback( DWCConnectionClosedCallback callback, void* param );
    int		DWC_CloseConnectionHardFromServer( u8 aid );
    int		DWC_CloseConnectionHardBitmapFromServer( u32* bitmap );
    int     DWC_CloseAllConnectionsHard ( void );
    int     DWC_GetNumConnectionHost    ( void );
    u8      DWC_GetMyAID                ( void );
    int     DWC_GetAIDList              ( u8** aidList );
    u32     DWC_GetAIDBitmap            ( void );
    BOOL    DWC_IsValidAID              ( u8 aid );
    DWCState DWC_GetState               ( void );
    int     DWC_GetLastSocketError      ( void );

    u8      DWC_GetServerAID(void);
    BOOL    DWC_IsServerMyself(void);
    u32     DWC_GetDirectConnectedAIDBitmap(void);

    /**
     * ������AID�������l
     *
     */
#define DWC_INVALID_AID		(0xff)


#ifdef __cplusplus
}
#endif


#endif // DWC_MAIN_H_
