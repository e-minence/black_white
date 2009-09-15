/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_MATCH_H_
#define DWC_MATCH_H_


#ifdef __cplusplus
extern "C"
{
#endif

//----------------------------------------------------------------------------
// preprocessor switch
//----------------------------------------------------------------------------

// �T�[�o�N���C�A���g�}�b�`���C�N�ȊO�ł̓}�b�`���C�N���I�������QR2���I������d�l
#define DWC_QR2_ALIVE_DURING_MATCHING

// �F�B���X�g�ɗF�B�����Ȃ��Ă��ꍇ�ɂ���Ă̓}�b�`���C�N�J�n��OK����
#define DWC_MATCH_ACCEPT_NO_FRIEND

// �F�B����̗F�B�w��s�A�}�b�`���C�N��L���ɂ���
#define DWC_LIMIT_FRIENDS_MATCH_VALID

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------

/// �ő哯���ڑ���
#define DWC_MAX_CONNECTIONS 32

    /**
     * �Q�[����`�L�[�̊J�n�l
     *
     */
#define DWC_QR2_GAME_KEY_START 100

    /**
     * DWC�̗\��L�[��
     *
     */
#define DWC_QR2_RESERVED_KEYS  (100-50)     // 50 is originaly from NUM_RESERVED_KEYS in qr2regkeys.h
#define DWC_MAX_REGISTERED_KEYS 254         // 254 is originaly from NUM_RESERVED_KEYS in qr2regkeys.h

    /**
     * �Q�[���ōő�g�p�ł���L�[�̐��i154�j
     *
     */
#define DWC_QR2_GAME_RESERVED_KEYS (DWC_MAX_REGISTERED_KEYS-DWC_QR2_RESERVED_KEYS-DWC_QR2_RESERVED_KEYS)

//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------

    /// �}�b�`���C�N��ԗ񋓎q
    typedef enum
    {
        DWC_MATCH_STATE_INIT = 0,           ///< �������

        // �N���C�A���g���̏��
        DWC_MATCH_STATE_CL_WAITING,         ///< �҂����
        DWC_MATCH_STATE_CL_SEARCH_HOST,     ///< �󂫃z�X�g������ԁi�F�B���w��̎��̂݁j
        DWC_MATCH_STATE_CL_WAIT_RESV,       ///< �\��ɑ΂���T�[�o����̕ԓ��҂�
        DWC_MATCH_STATE_CL_SEARCH_EVAL_HOST,///< �]�����鑊��z�X�g��������
        DWC_MATCH_STATE_CL_NN,              ///< NAT�l�S�V�G�[�V������
        DWC_MATCH_STATE_CL_GT2,             ///< GT2�R�l�N�V�����m����
        DWC_MATCH_STATE_CL_CANCEL_SYN,      ///< �T�[�o�N���C�A���g�Ń}�b�`���C�N�L�����Z������������
        DWC_MATCH_STATE_CL_SYN,             ///< �}�b�`���C�N��������������
        DWC_MATCH_STATE_CL_SVDOWN_1,        ///< �T�[�o�_�E�����o��
        DWC_MATCH_STATE_CL_SVDOWN_2,        ///< �T�[�o�_�E�����o�� 2
        DWC_MATCH_STATE_CL_SVDOWN_3,        ///< �T�[�o�_�E�����o�� 3
        DWC_MATCH_STATE_CL_SEARCH_GROUPID_HOST, ///< �O���[�vID ����z�X�g������

        // �T�[�o���̏��
        DWC_MATCH_STATE_SV_WAITING,         ///< �҂����
        DWC_MATCH_STATE_SV_OWN_NN,          ///< �N���C�A���g�Ƃ�NAT�l�S�V�G�[�V������
        DWC_MATCH_STATE_SV_OWN_GT2,         ///< �N���C�A���g�Ƃ�GT2�R�l�N�V�����m����
        DWC_MATCH_STATE_SV_WAIT_CL_LINK,    ///< �N���C�A���g���m�̐ڑ������҂�
        DWC_MATCH_STATE_SV_CANCEL_SYN,      ///< �T�[�o�N���C�A���g�Ń}�b�`���C�N�L�����Z����������SYN-ACK�҂�
        DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT, ///< �T�[�o�N���C�A���g�Ń}�b�`���C�N�L�����Z�����������I���҂�
        DWC_MATCH_STATE_SV_SYN,             ///< �}�b�`���C�N�I����������SYN-ACK�҂�
        DWC_MATCH_STATE_SV_SYN_WAIT,        ///< �}�b�`���C�N�I�����������I���҂�

        // ���ʏ��
        DWC_MATCH_STATE_WAIT_CLOSE,         ///< �R�l�N�V�����N���[�Y�����҂�
        DWC_MATCH_STATE_SEARCH_OWN,         ///< ���z�X�g��񌟍����i�ȑO�̖��̂�DWC_MATCH_STATE_CL_SEARCH_OWN)
        DWC_MATCH_STATE_SV_SYN_CLOSE_WAIT,	///< CLOSE �R�}���h���M��̑҂�����

        DWC_MATCH_STATE_NUM
    } DWCMatchState;

    /**
     * �}�b�`���C�N�ڑ������Ɏg����o�b�t�@��
     *
     */
#define DWC_CONNECTION_USERDATA_LEN 4

    /// �}�b�`���C�N�^�C�v�񋓎q
    enum
    {
        DWC_MATCH_TYPE_ANYBODY = 0,  ///< �F�B���w��s�A�}�b�`���C�N
        DWC_MATCH_TYPE_FRIEND,		 ///< �F�B�w��s�A�}�b�`���C�N
        DWC_MATCH_TYPE_SC_SV,        ///< �T�[�o�N���C�A���g�}�b�`���C�N�̃T�[�o��
        DWC_MATCH_TYPE_SC_CL,        ///< �T�[�o�N���C�A���g�}�b�`���C�N�̃N���C�A���g��
        DWC_MATCH_TYPE_NUM
    };

    /// ���ߐ؂�R�[���o�b�N�񋓎q
    typedef enum
    {
        DWC_SUSPEND_SUCCESS = 0,	///< ����I��
        DWC_SUSPEND_TIMEOUT,		///< �^�C���A�E�g����(���݁A�g�p����܂���)
        DWC_SUSPEND_NUM
    }DWCSuspendResult;

    /// �ڑ��`�ԗ񋓎q
    typedef enum
    {
        DWC_TOPOLOGY_TYPE_HYBRID = 0,	///< �n�C�u���b�h�^
        DWC_TOPOLOGY_TYPE_STAR,			///< �X�^�[�^
        DWC_TOPOLOGY_TYPE_FULLMESH,		///< �t�����b�V���^
        DWC_TOPOLOGY_TYPE_NUM
    }DWCTopologyType;

//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
    /**
     * �}�b�`���C�N�����R�[���o�b�N�^
     *
     * DWC_ConnectToAnybodyAsync / DWC_ConnectToFriendsAsync / DWC_SetupGameServer /
     * DWC_ConnectToGameServerAsync / DWC_ConnectToGameServerByGroupID
     * �֐��ďo����A�P��
     * �R�l�N�V�������m�����ꂽ���A�܂��̓G���[�A�L�����Z���������������ɌĂ΂�܂��B
     *
     * �G���[�������́Aerror, param�ȊO�̊e�����̒l�͕s��ł��B
     *
     * �{�R�[���o�b�N�֐���
     * DWC_ConnectToAnybodyAsync / DWC_ConnectToFriendsAsync / DWC_SetupGameServer /
     * DWC_ConnectToGameServerAsync / DWC_ConnectToGameServerByGroupID
     * �֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B
     *
     * �����̎�蓾��l�̑g�ݍ��킹
     *
     * �Eerror == DWC_ERROR_NONE && cancel == FALSE �̏ꍇ
     * 	
     *			
     *				self    
     *				isServer
     *				���e    
     *			
     *			
     *				TRUE    
     *				TRUE    
     *				
     *				    �Ȃ�
     *				
     *            
     *            
     *				TRUE    
     *				FALSE   
     *				
     *				    �������N���C�A���g�ŁA�T�[�o�Ƃ��̃l�b�g���[�N�ւ̐ڑ��ɐ��������Bindex�� -1�B
     *				
     *            
     *            
     *				FALSE   
     *				TRUE    
     *				�Ȃ�    
     *            
     *            
     *				FALSE   
     *				FALSE   
     *				
     *				    �����̓T�[�o���N���C�A���g�ŁA���̃N���C�A���g���T�[�o�Ƃ��̃l�b�g���[�N�ւ̐ڑ��ɐ��������B
     *				    index�͂��̃N���C�A���g�̗F�B���X�g�C���f�b�N�X�ŁA�F�B�łȂ���� -1�B
     *				
     *           
     *   
     *   
     *
     *   �Eerror == DWC_ERROR_NONE && cancel == TRUE �̏ꍇ
     *   
     *   		
     *   			self    
     *   			isServer
     *   			���e    
     *			
     *			
     *				TRUE    
     *				TRUE    
     *				
     *				    �������T�[�o�ŁA�����Ń}�b�`���C�N���L�����Z�������Bindex�� -1�B
     *				
     *            
     *            
     *				TRUE    
     *				FALSE   
     *				
     *				    �������N���C�A���g�ŁA�����Ń}�b�`���C�N���L�����Z�������Bindex�� -1�B
     *				
     *            
     *            
     *				FALSE   
     *				TRUE    
     *				
     *				    �������N���C�A���g�ŁA�T�[�o���}�b�`���C�N���L�����Z�������Bindex�̓T�[�o�̗F�B���X�g�C���f�b�N�X�B
     *				
     *            
     *            
     *				FALSE   
     *				FALSE   
     *				
     *				    �����̓T�[�o���N���C�A���g�ŁA���̃N���C�A���g���}�b�`���C�N���L�����Z�������B
     *				    index�͂��̃N���C�A���g�̗F�B���X�g�C���f�b�N�X�ŁA�F�B�łȂ���� -1�B
     *            
     *	
     *
     * �����͒��̃T�[�o/�N���C�A���g�́AGameSpy�T�[�o�ł͂Ȃ�Wii/DS�[���̂��Ƃ��w���܂��B
     *
     * Param:    error       DWC�G���[��ʁB���e�̏ڍׂ́ADWC_GetLastErrorEx���Q�Ƃ��Ă��������B
     * Param:    cancel      TRUE:�L�����Z���Ń}�b�`���C�N�I��
     *                      FALSE:�L�����Z���ł͂Ȃ�
     * Param:    self        TRUE: �������T�[�o�ւ̐ڑ��ɐ����A�������͐ڑ����L�����Z�������B
     *                      FALSE:���̃z�X�g���T�[�o�ւ̐ڑ��ɐ����A�������͐ڑ����L�����Z�������B
     * Param:    isServer    TRUE: self = FALSE �̎��ɏ�L�̓�������������z�X�g���T�[�o�ł���
     *                      FALSE:self = FALSE �̎��ɏ�L�̓�������������z�X�g���N���C�A���g�ł��邩�Aself = TRUE
     * Param:    index       self = FALSE �̎��ɁA��L�̓�������������z�X�g�̗F�B���X�g�C���f�b�N�X�B
     *                      �z�X�g���F�B�łȂ����Aself = TRUE �̎���-1�ƂȂ�B
     * Param:    param       �R�[���o�b�N�p�p�����[�^
     *
     * See also:   DWC_ConnectToAnybodyAsync
     * See also:   DWC_ConnectToFriendsAsync
     * See also:   DWC_SetupGameServer
     * See also:   DWC_ConnectToGameServerAsync
     * See also:   DWC_ConnectToGameServerByGroupID
     *
     */
    typedef void (*DWCMatchedSCCallback)(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int index, void* param);

    /**
     * �V�K�ڑ��N���C�A���g�ڑ��J�n�ʒm�R�[���o�b�N�^
     *
     * �}�b�`���C�N�ŁA���ł��Ă���O���[�v�ɐV���ɕʂ̃N��
     * �C�A���g���ڑ����J�n�������ɌĂ΂��
     *
     * Param:    index   �V�K�ڑ��N���C�A���g�̗F�B���X�g�C���f�b�N�X�B
     *                  �V�K�ڑ��N���C�A���g���F�B�łȂ����-1�ƂȂ�B
     * Param:    param   �R�[���o�b�N�p�p�����[�^
     *
     * See also:   DWC_ConnectToAnybodyAsync
     * See also:   DWC_ConnectToFriendsAsync
     * See also:   DWC_SetupGameServer
     * See also:   DWC_ConnectToGameServerAsync
     * See also:   DWC_ConnectToGameServerByGroupID
     *
     */
    typedef void (*DWCNewClientCallback)(int index, void* param);

    /**
     * �v���C���[�]���R�[���o�b�N�^
     *
     * �F�B�w��E�F�B���w��s�A�}�b�`���C�N���ɁA�}�b�`���C�N�Ώۃv���C���[��������x�ɌĂ΂��v���C���[�]���R�[���o�b�N�֐��ł��B
     * �{�֐��Őݒ肵���Ԃ�l�̑傫���v���C���[�قǐڑ���Ƃ��đI�΂��\���������Ȃ�A
     * 0�ȉ��̒l��Ԃ����ꍇ�́A���̃v���C���[�̓}�b�`���C�N�̑Ώۂ���O����܂��B
     * �������A�]���l�̐�Βl�͏d�v�ł͂Ȃ��A���������v���C���[�̕]���l���ɂ��I�΂�₷�������܂�܂��B
     * �Ⴆ�΁A�]���l�����ꂼ��1, 2, 3, 10���ƂȂ��Ă����ꍇ�ł��A10���̃z�X�g�����|�I�ɑI�΂�₷���Ȃ�Ƃ����킯�ł͂���܂���B
     *
     * �܂��A���C�u���������ŁA�Ԃ�l��8,388,607�i0x007fffff�j�ȉ��̐��̐��ł������ꍇ�́A8�r�b�g���V�t�g���A
     * ����8�r�b�g�ɗ�����t�����邱�ƂŁA�����]���l�ƂȂ鑊��ł�������x���U���Đڑ������݂�d�l�ƂȂ��Ă��܂��B
     * �Ȃ��A�Ԃ�l��8,388,607���傫���ꍇ�́A 8,388,607�Ƃ��Ĉ����܂��B
     *
     * �v���C���[�̕]���Ɏg���}�b�`���C�N�w�W�L�[���擾����ɂ́ADWC_GetMatchIntValue / DWC_GetMatchStringValue�֐����g�p���܂��B
     * �����̊֐��́A�{�֐����ł����g�p�ł��܂���B
     * �ǉ������w�W�L�[���T�[�o�ɔ��f�����̂Ɏ��Ԃ�������̂ŁA�A�v���P�[�V�������Ŏw�W�L�[���Z�b�g���Ă��Ă��A
     * �w�W�L�[���Ȃ��Ɣ��f����ăf�t�H���g�l�i�f�t�H���g������j���Ԃ���邱�Ƃ�����܂��B
     *
     * �{�R�[���o�b�N�֐���DWC_ConnectToAnybodyAsync / DWC_ConnectToFriendsAsync�֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B 
     *
     * Param:    index   ���C�u�������g�p����]���Ώۃv���C���[�̃C���f�b�N�X�B
     *	                �}�b�`���C�N�w�W�L�[���擾����DWC_GetMatchIntValue / DWC_GetMatchStringValue�֐��ɓn���Ă��������B
     * Param:    param   DWC_ConnectToAnybodyAsync / DWC_ConnectToFriendsAsync�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     * Return value:   �]���l�B�傫���l�قǂ��̃v���C���[�ɐڑ������݂�m�����オ��܂��B
     *	        �Ƃ肤��l�͈̔͂�int�ł����A0x007fffff���傫�Ȑ��̐��́A�S��0x007fffff�Ɠ����Ɉ����܂��B
     *	        0�ȉ��̒l��Ԃ����ꍇ�́A���̃v���C���[�ւ͐ڑ����܂���B 
     *
     */
    typedef int (*DWCEvalPlayerCallback)(int index, void* param);

    /**
     * �}�b�`���C�N��������R�[���o�b�N�^
     *
     * �V�K�ڑ��N���C�A���g�̎Q�������肷��ŏI�i�K�ŁA
     * �A�v���P�[�V�������ɔ��f�����߂邽�߂ɌĂяo�����R�[���o�b�N�ł��B
     * (�l������t�ł���ꍇ��A��t���ۏ�ԂȂǂ̏ꍇ�͂��̃R�[���o�b�N�͌Ăяo����܂���)
     * �V�K�ڑ��N���C�A���g���󂯓���邩�ǂ����̔��f�́A
     * newClientUserData��������擾�ł���V�K�ڑ��N���C�A���g�̐ڑ����[�U�f�[�^�A
     * �y��DWC_GetConnectionUserData�Ŏ擾�ł���A
     * �ڑ��ς݃N���C�A���g�̐ڑ����[�U�f�[�^���l�����Ĕ��f���Ă��������B
     * �N���C�A���g���� DWC_ConnectToGameServerAsync �� DWC_ConnectToGameServerByGroupID
     * �Őڑ����悤�Ƃ��āA�T�[�o���ŌĂ΂ꂽ���̊֐���FALSE��Ԃ����ꍇ�A
     * �N���C�A���g���ł̓G���[�Ƃ���DWC_ERROR_SC_CONNECT_BLOCK���Z�b�g����܂��B
     *
     * Param:    newClientUserData  �V�K�ڑ��N���C�A���g���A
     *                             DWC_ConnectToAnybodyAsync / DWC_ConnectToFriendsAsync /
     *                             DWC_SetupGameServer / DWC_ConnectToGameServerAsync /
     *                             DWC_ConnectToGameServerByGroupID�֐���
     *                             connectionUserData�ɐݒ肵���l���i�[���Ă���o�b�t�@�ւ̃|�C���^�B
     *                             u8[DWC_CONNECTION_USERDATA_LEN]���̃T�C�Y�B
     * Param:    param              �R�[���o�b�N�p�p�����[�^
     *
     * Return value:  TRUE  �V�K�ڑ��N���C�A���g���󂯓����B
     * Return value:  FALSE �V�K�ڑ��N���C�A���g�̎󂯓�������ۂ���B
     *
     */
    typedef BOOL (*DWCConnectAttemptCallback)(u8* newClientUserData, void* param);

    /**
     * ���ߐ؂芮���R�[���o�b�N�^
     *
     * DWC_RequestSuspendMatchAsync �Ŏw�肳��A���ߐ؂菈�����I������ƌĂ΂�܂��B
     * result �� DWC_SUSPEND_SUCCESS �̎��͒��ߐ؂��Ԃ� suspend �ɂȂ��Ă��܂��B
     * result �� DWC_SUSPEND_SUCCESS �ȊO�̏ꍇ�͒��ߐ؂菈���Ɏ��s���Ă��܂����A
     * suspend �ɂ͎Q�l�̂��߂� DWC_RequestSuspendMatchAsync �œn���ꂽ�l�����̂܂ܓn����܂��B
     *
     *
     * Param:    result  ���ߐ؂菈���̌��ʁB
     *                  DWC_SUSPEND_SUCCESS:���ߐ؂��Ԃ�suspend�ɂȂ��Ċ��������B
     *                  DWC_SUSPEND_SUCCESS�ȊO:���ߐ؂菈���Ɏ��s�����B
     * Param:    suspend TRUE:��t���ۏ��
     *                  FALSE:��t�\���
     *                  DWC_RequestSuspendMatchAsync �Ŏw�肳�ꂽ�l�����̂܂ܓn�����B
     * Param:    data    �R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void (*DWCSuspendCallback)(DWCSuspendResult result, BOOL suspend, void* data);

//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    BOOL    DWC_RegisterMatchStatus  ( void );
#endif
    BOOL    DWC_CancelMatch          ( void );
    BOOL    DWC_CancelMatchAsync     ( void );
    BOOL    DWC_IsValidCancelMatch   ( void );
    u8      DWC_AddMatchKeyInt          ( u8 keyID, const char* keyString, const int* valueSrc );
    u8      DWC_AddMatchKeyString       ( u8 keyID, const char* keyString, const char* valueSrc );
    int     DWC_GetMatchIntValue        ( int index, const char* keyString, int idefault );
    const char* DWC_GetMatchStringValue ( int index, const char* keyString, const char* sdefault );
    int     DWC_GetLastMatchType     ( void );
    DWCMatchState DWC_GetMatchState  ( void );

    u32 DWC_GetGroupID(void);

    // ���ߐ؂菈��
    BOOL DWC_RequestSuspendMatchAsync(BOOL suspend, DWCSuspendCallback callback, void* data);
    BOOL DWC_GetSuspendMatch(void);

    // ConnUserData �֘A
    BOOL DWC_GetConnectionUserData(u8 aid, u8* userData);

    // �f�o�b�O�p
    void DWC_SetDebugMeshNNRetryMax(int retryMax);
    int DWC_GetDebugMeshNNRetryMax(void);

    BOOL DWC_SetServerSearchRatio(int percentage);
    int DWC_GetServerSearchRatio();
    BOOL DWC_SetServerLockEnabled(BOOL enabled);
    BOOL DWC_GetServerLockEnabled(void);

#ifdef __cplusplus
}
#endif


#endif // DWC_MATCH_H_
