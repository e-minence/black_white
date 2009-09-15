/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_FRIEND_H_
#define DWC_FRIEND_H_


#ifdef __cplusplus
extern "C"
{
#endif

/// DWC_GetOwnStatusString�֐���DWC_SetOwnStatusString�֐���DWC_GetFriendStatusSC�֐��Ŏw��ł���X�e�[�^�X������̒���(NULL�I�[�܂�)
#define DWC_FRIEND_STATUS_STRING_LEN (256)    // GP_LOCATION_STRING_LEN

//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------

    /// �F�B�̒ʐM��ԁiGPEnum��Status�{�����Ē�`���Ďg�p�j
    enum
    {
        DWC_STATUS_OFFLINE = 0,    ///< �I�t���C��
        DWC_STATUS_ONLINE,         ///< �I�����C���iGP�T�[�o�Ƀ��O�C���ς݁j
        DWC_STATUS_PLAYING,        ///< �Q�[���v���C��
        DWC_STATUS_MATCH_ANYBODY,  ///< �F�B���w��s�A�}�b�`���C�N��
        DWC_STATUS_MATCH_FRIEND,   ///< �F�B�w��s�A�}�b�`���C�N��
        DWC_STATUS_MATCH_SC_CL,    ///< �T�[�o�N���C�A���g�}�b�`���C�N���̃N���C�A���g
        DWC_STATUS_MATCH_SC_SV,    ///< �T�[�o�N���C�A���g�}�b�`���C�N���̃T�[�o
        DWC_STATUS_NUM
    };

    /// �F�B�Ǘ��������
    typedef enum
    {
        DWC_FRIEND_STATE_INIT = 0,      ///< �������
        DWC_FRIEND_STATE_PERS_LOGIN,    ///< Persistent�T�[�o���O�C�����F�B���X�g����������
        DWC_FRIEND_STATE_LOGON,         ///< Persistent�T�[�o���O�C����
        DWC_FRIEND_STATE_NUM
    } DWCFriendState;

    /// ���[�J���F�B���X�g��GP�o�f�B���X�g�Ƃ̓����������
    enum
    {
        DWC_BUDDY_UPDATE_STATE_WAIT = 0,  ///< �o�f�B���X�g�_�E�����[�h�����҂�
        DWC_BUDDY_UPDATE_STATE_CHECK,     ///< �o�f�B���X�g�`�F�b�N��
        DWC_BUDDY_UPDATE_STATE_PSEARCH,   ///< �v���t�@�C���T�[�`��
        DWC_BUDDY_UPDATE_STATE_COMPLETE,  ///< �o�f�B���X�g������������
        DWC_BUDDY_UPDATE_STATE_NUM
    };


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------

    /**
     * �F�B���X�g���������R�[���o�b�N�^
     *
     * �F�B���X�g�������������������Ƃ��ɌĂяo����܂��B
     * 
     * �F�B���X�g�̓��������̓��e�ɂ��ẮADWC_UpdateServersAsync�֐����Q�Ƃ��Ă��������B
     * 
     * �{�R�[���o�b�N�֐���DWC_UpdateServersAsync�֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B 
     * 
     * Param:    error       DWC�G���[��ʁB���e�̏ڍׂ́ADWC_GetLastErrorEx���Q�Ƃ��Ă��������B
     * Param:    isChanged   TRUE:�����������ɗF�B���X�g������������ꂽ
     *                      FALSE:�F�B���X�g�̏��������Ȃ�
     * Param:    param       DWC_UpdateServersAsync�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void (*DWCUpdateServersCallback)(DWCError error, BOOL isChanged, void* param);

    /**
     * �F�B��ԕω��ʒm�R�[���o�b�N�^
     *
     * �F�B�̏�Ԃ��ω������Ƃ��ɌĂяo����܂��B
     *
     * �v���C���[�̒ʐM��Ԃ́Astatus�ŕ\�����status�t���O�ƁAstatusString�ŕ\�����status�����񂩂琬���Ă���A
     * GameSpy�T�[�o��ŊǗ�����Ă��܂��B status�t���O��DWC���C�u�����������I�ɃZ�b�g���܂����A
     * status�������DWC_SetOwnStatusString / DWC_SetOwnStatusData�֐���p���ăA�v���P�[�V�������ݒ肷�邱�Ƃ��ł��܂��B
     *
     * �{�R�[���o�b�N�֐���DWC_UpdateServersAsync�֐�/DWC_SetFriendStatusCallback�֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B 
     * 
     * 
     * status�̒l�́A�ȉ��̂����ꂩ�ɂȂ�܂��B
     * 			
     * 			    
     * 			      DWC_STATUS_OFFLINE
     * 			      �I�t���C���������I�t���C���̎��A�܂��͑���ƗF�B�֌W���������Ă��Ȃ��Ƃ�
     * 			    
     * 			    
     * 			      DWC_STATUS_ONLINE
     * 			      �I�����C���iWi-Fi�R�l�N�V�����T�[�o�Ƀ��O�C���ς݁j
     * 			    
     * 			    
     * 			      DWC_STATUS_PLAYING
     * 			      �E�N���C�A���g�Ƃ��ăQ�[���v���C���i�}�b�`���C�N�^�C�v�̎�ނɂ��Ȃ��j�E�F�B�w��}�b�`���C�N���̓T�[�o�N���C�A���g�}�b�`���C�N�ŁA��x�ł��T�[�o��オ�N��������̃T�[�o�Ƃ��ăQ�[���v���C����L2��Ԃ͋��ʂ��āA���̑���ɑ΂��Đڑ��ɍs�����Ƃ͏o���Ȃ����������Ă���B(PLAYING��Ԃ͎�t�\�ȏ�Ԃł͖���)
     * 			    
     * 			    
     * 			      DWC_STATUS_MATCH_ANYBODY
     * 			      �E�F�B���w��s�A�}�b�`���C�N���Őڑ�������������E�F�B���w��s�A�}�b�`���C�N���ŃT�[�o�Ƃ��ăQ�[���v���C��
     * 			    
     * 			    
     * 			      DWC_STATUS_MATCH_FRIEND
     * 			      �E�F�B�w��s�A�}�b�`���C�N���Őڑ�������������E�F�B�w��s�A�}�b�`���C�N���ŃT�[�o�Ƃ��ăQ�[���v���C��
     * 			    
     * 			    
     * 			      DWC_STATUS_MATCH_SC_CL
     * 			      �E�T�[�o�N���C�A���g�}�b�`���C�N���̃N���C�A���g�z�X�g�Ƃ��āA�T�[�o�Ɛڑ��������E�O���[�vID�ڑ��̃N���C�A���g�z�X�g�Ƃ��āA�T�[�o�Ɛڑ�������
     * 			    
     * 			    
     * 			      DWC_STATUS_MATCH_SC_SV
     * 			      �E�T�[�o�N���C�A���g�}�b�`���C�N���̃T�[�o�z�X�g�Ƃ��ăQ�[���v���C��
     *	            
     *	        
     * 
     *	
     * Param:    index           ��Ԃ��ω������F�B�̗F�B���X�g�C���f�b�N�X
     * Param:    status          �F�B�̏�ԁBDWC_STATUS_* �̗񋓎q
     * Param:    statusString    �Q�[���Œ�`����GameSpy status������B
     *                          ������͍ő�255�����{NULL�I�[�B
     * Param:    param           DWC_UpdateServersAsync�֐�/DWC_SetFriendStatusCallback�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     * 
     */
    typedef void (*DWCFriendStatusCallback)(int index, u8 status, const char* statusString, void* param);

    /**
     * �F�B���X�g�폜�R�[���o�b�N�^
     *
     * DWC_UpdateServersAsync�֐��̏������ɁA�F�B���X�g���ɓ����F�B���������A
     * ���C�u�������ŏ��������Ƃ��ɌĂяo����܂��B
     * 
     * �{�R�[���o�b�N�֐���DWC_UpdateServersAsync�֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B 
     *
     * Param:    deletedIndex    �폜���ꂽ�F�B�̗F�B���X�g�C���f�b�N�X
     * Param:    srcIndex        �����F�B���Ɣ��肳�ꂽ�F�B���X�g�C���f�b�N�X
     * Param:    param           DWC_UpdateServersAsync�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void (*DWCDeleteFriendListCallback)(int deletedIndex, int srcIndex, void* param);

    /**
     * �o�f�B�����R�[���o�b�N�^
     *
     * GameSpy�T�[�o��ŗF�B�֌W�����������Ƃ��ɌĂяo����܂��B
     * �������ADWC_UpdateServersAsync�֐��ďo���ォ��A�F�B���X�g������������������܂ł̊Ԃ͌Ăяo����܂���B
     *
     * �{�R�[���o�b�N�֐���DWC_SetBuddyFriendCallback�֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B 
     *
     * Param:    index   �F�B�֌W�����������F�B���X�g�̃C���f�b�N�X
     * Param:    param   DWC_SetBuddyFriendCallback�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void (*DWCBuddyFriendCallback)(int index, void* param);

//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
    u8      DWC_GetFriendStatus         ( const DWCFriendData* friendData, char* statusString );
    u8      DWC_GetFriendStatusSC       ( const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusString );
    u8      DWC_GetFriendStatusData     ( const DWCFriendData* friendData, char* statusData, int* size );
    u8      DWC_GetFriendStatusDataSC   ( const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusData, int* size );
    int     DWC_GetNumFriend            ( const DWCFriendData friendList[], int friendListLen );
    BOOL    DWC_SetOwnStatusString      ( const char* statusString );
    BOOL    DWC_GetOwnStatusString      ( char* statusString );
    BOOL    DWC_SetOwnStatusData        ( const char* statusData, u32 size );
    int     DWC_GetOwnStatusData        ( char* statusData );
    BOOL    DWC_CanChangeFriendList     ( void );
    void    DWC_DeleteBuddyFriendData   ( DWCFriendData* friendData );
    BOOL    DWC_SetBuddyFriendCallback  ( DWCBuddyFriendCallback callback, void* param );
    BOOL    DWC_SetFriendStatusCallback ( DWCFriendStatusCallback callback, void* param );

#ifdef __cplusplus
}
#endif


#endif // DWC_FRIEND_H_
