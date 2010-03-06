/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2010 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_ERROR_H_
#define DWC_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif


    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */

    /**
     * �G���[�񋓎q
     */
    typedef enum {
        DWC_ERROR_NONE = 0,            ///< �G���[�Ȃ�
        DWC_ERROR_DS_MEMORY_ANY,       ///< �{�̃Z�[�u�f�[�^�Ɋւ���G���[
        DWC_ERROR_AUTH_ANY,            ///< �F�؂Ɋւ���G���[
        DWC_ERROR_AUTH_OUT_OF_SERVICE, ///< �T�[�r�X�I��
        DWC_ERROR_AUTH_STOP_SERVICE,   ///< �T�[�r�X�ꎞ���f
        DWC_ERROR_AC_ANY,              ///< �����ڑ��Ɋւ���G���[
        DWC_ERROR_NETWORK,             ///< ���̑��̃l�b�g���[�N�G���[
        DWC_ERROR_GHTTP_ANY,           ///< HTTP�ʐM(GameSpy HTTP)�̃G���[
        DWC_ERROR_DISCONNECTED,        ///< �ؒf���ꂽ
        DWC_ERROR_FATAL,               ///< �v���I�ȃG���[
    
        // �V�[�P���X��̌y�x�̃G���[�iDWCErrorType = DWC_ETYPE_LIGHT�j
        DWC_ERROR_FRIENDS_SHORTAGE,    ///< �w��̐l���̗L���ȗF�B�����Ȃ��̂ɗF�B�w��s�A�}�b�`���C�N���n�߂悤�Ƃ���
        DWC_ERROR_NOT_FRIEND_SERVER,   ///< �T�[�o�N���C�A���g�}�b�`���C�N�ŁA�T�[�o�Ɏw�肵���F�B�����݂ɗF�B�ł͂Ȃ��A�������̓T�[�o�𗧂ĂĂ��Ȃ�
        DWC_ERROR_SERVER_FULL,         ///< �T�[�o�N���C�A���g�}�b�`���C�N�̃T�[�o������I�[�o�[
    	
    	// Nd���C�u�����p�̃G���[��ʒǉ�
    	DWC_ERROR_ND_ANY,             ///< �_�E�����[�h���C�u�����̃G���[
    	DWC_ERROR_ND_HTTP,            ///< �_�E�����[�h���C�u������HTTP�G���[
    	
    	// SVL���C�u�����p�̃G���[��ʒǉ�
    	DWC_ERROR_SVL_ANY,            ///< �T�[�r�X���P�[�^�̃G���[
    	DWC_ERROR_SVL_HTTP,           ///< �T�[�r�X���P�[�^���擾����HTTP�ʐM�G���[
        
        // �s��������`�F�b�N�p�̃G���[��ʒǉ�
        DWC_ERROR_PROF_ANY,           ///< �s��������`�F�b�N�ŃT�[�o�̉��������������Ƃ��ɔ�������G���[
        DWC_ERROR_PROF_HTTP,          ///< �s��������`�F�b�N��HTTP�G���[
    	
    	// gdb���C�u�����̃G���[
    	DWC_ERROR_GDB_ANY,            ///< GDB���C�u�����̃G���[�B�ʏ��DWC�ł͔������܂���B
        DWC_ERROR_SCL_ANY,            ///< SC���C�u�����̃G���[�B�ʏ��DWC�ł͔������܂���B
    
        DWC_ERROR_SC_CONNECT_BLOCK,   ///< �T�[�o�N���C�A���g�}�b�`���C�N�ŁA�N���C�A���g���T�[�o����ڑ������ۂ��ꂽ
        DWC_ERROR_NETWORK_LIGHT,	  ///< �y�x�Ƃ��Ĉ����l�b�g���[�N�G���[
    	
        DWC_ERROR_NUM
    } DWCError;
    
    
    /**
     * �G���[���o���ɕK�v�ȏ����������G���[�^�C�v
     */
    typedef enum {
        DWC_ETYPE_NO_ERROR = 0,   ///< �G���[�Ȃ�
        DWC_ETYPE_LIGHT,          ///< �Q�[���ŗL�̕\���݂̂ŁA�G���[�R�[�h�\���͕K�v����܂���B
                                  ///< DWC_ClearError�֐����Ăяo���΁A���A�\�ł��B
        
        DWC_ETYPE_SHOW_ERROR,     ///< �G���[�R�[�h��\�����Ă��������B
                                  ///< DWC_ClearError�֐����Ăяo���΁A���A�\�ł��B 
        
        DWC_ETYPE_SHUTDOWN_FM,    ///< DWC_ShutdownFriendsMatch�֐����Ăяo���āAFriendsMatch���C�u�������I������K�v������܂��B�G���[�R�[�h��\�����Ă��������B
        
        DWC_ETYPE_SHUTDOWN_GHTTP, ///< �ȉ��̊e���C�u��������֐���K�v�ɉ����ČĂяo���Ă��������B
                                  ///< DWC_RnkShutdown�֐�(�ėp�����L���O���C�u����)
                                  ///< �G���[�R�[�h��\�����Ă��������B
        
        DWC_ETYPE_SHUTDOWN_ND,    ///< DWC_NdCleanupAsync�֐����Ăяo���āA�_�E�����[�h���C�u�������I������K�v������܂��B�G���[�R�[�h��\�����Ă��������B
        DWC_ETYPE_DISCONNECT,     ///< �ȉ��̊e���C�u��������֐���K�v�ɉ����ČĂяo���Ă��������B
                                  ///< DWC_ShutdownFriendsMatch�֐�(FriendsMatch���C�u����)
                                  ///< DWC_NdCleanupAsync�֐�(�_�E�����[�h���C�u����)
                                  ///< DWC_RnkShutdown�֐�(�ėp�����L���O���C�u����)
                                  ///< ���̌�ADWC_CleanupInet���邢��DWC_CleanupInetAsync�֐��ŒʐM�̐ؒf���s���K�v������܂��B �G���[�R�[�h��\�����Ă��������B

        DWC_ETYPE_FATAL,          ///< FatalError�����Ȃ̂ŁANitro�ł́A�d��OFF�𑣂��K�v������܂��BTwl�ł̓��Z�b�g(�����`���[�ւ̖߂�)�𑣂��K�v������܂��B�G���[�R�[�h��\�����Ă��������B
        DWC_ETYPE_NUM
    } DWCErrorType;
    
    
    // �f�o�b�O�pdwc_base�G���[�R�[�h�e�v�f
    enum {
        // �ǂ̃V�[�P���X���ɋN�������G���[��
        DWC_ECODE_SEQ_LOGIN          = (-60000),  // ���O�C�������ł̃G���[
        DWC_ECODE_SEQ_FRIEND         = (-70000),  // �F�B�Ǘ������ł̃G���[
        DWC_ECODE_SEQ_MATCH          = (-80000),  // �}�b�`���C�N�����ł̃G���[
        DWC_ECODE_SEQ_ETC            = (-90000),  // ��L�ȊO�̏����ł̃G���[
    
        // GameSpy�̃G���[�̏ꍇ�A�ǂ�SDK���o�����G���[��
        DWC_ECODE_GS_GP              =  (-1000),  // GameSpy GP �̃G���[
        DWC_ECODE_GS_PERS            =  (-2000),  // GameSpy Persistent �̃G���[
        DWC_ECODE_GS_STATS           =  (-3000),  // GameSpy Stats �̃G���[
        DWC_ECODE_GS_QR2             =  (-4000),  // GameSpy QR2 �̃G���[
        DWC_ECODE_GS_SB              =  (-5000),  // GameSpy Server Browsing �̃G���[
        DWC_ECODE_GS_NN              =  (-6000),  // GameSpy Nat Negotiation �̃G���[
        DWC_ECODE_GS_GT2             =  (-7000),  // GameSpy gt2 �̃G���[
        DWC_ECODE_GS_HTTP            =  (-8000),  // GameSpy HTTP �̃G���[
        DWC_ECODE_GS_ETC             =  (-9000),  // GameSpy ���̑���SDK�̃G���[
    
        // �ʏ�G���[���
        DWC_ECODE_TYPE_NETWORK       =   ( -10),  // �l�b�g���[�N��Q
        DWC_ECODE_TYPE_SERVER        =   ( -20),  // GameSpy�T�[�o��Q
        DWC_ECODE_TYPE_DNS           =   ( -30),  // DNS��Q
        DWC_ECODE_TYPE_DATA          =   ( -40),  // �s���ȃf�[�^����M
        DWC_ECODE_TYPE_SOCKET        =   ( -50),  // �\�P�b�g�ʐM�G���[
        DWC_ECODE_TYPE_BIND          =   ( -60),  // �\�P�b�g�̃o�C���h�G���[
        DWC_ECODE_TYPE_TIMEOUT       =   ( -70),  // �^�C���A�E�g����
        DWC_ECODE_TYPE_PEER          =   ( -80),  // �P�΂P�̒ʐM�ŕs�
        DWC_ECODE_TYPE_CONN_OVER     =   (-100),  // �R�l�N�V�������I�[�o
        DWC_ECODE_TYPE_STATS_AUTH    =   (-200),  // STATS�T�[�o���O�C���G���[
        DWC_ECODE_TYPE_STATS_LOAD    =   (-210),  // STATS�T�[�o�f�[�^���[�h�G���[
        DWC_ECODE_TYPE_STATS_SAVE    =   (-220),  // STATS�T�[�o�f�[�^�Z�[�u�G���[
        DWC_ECODE_TYPE_NOT_FRIEND    =   (-400),  // �w�肳�ꂽ���肪���݂ɓo�^���ꂽ�F�B�ł͂Ȃ�
        DWC_ECODE_TYPE_OTHER         =   (-410),  // ���肪�ʐM�ؒf�����i��ɃT�[�o�N���C�A���g�}�b�`���C�N���j
        DWC_ECODE_TYPE_MUCH_FAILURE  =   (-420),  // NAT�l�S�V�G�[�V�����ɋK��񐔈ȏ㎸�s����
        DWC_ECODE_TYPE_SC_CL_FAIL    =   (-430),  ///< �T�[�o�N���C�A���g�}�b�`���C�N�ɂ�����N���C�A���g�̐ڑ����s
        DWC_ECODE_TYPE_FAILED_SERVER_MIGRATION =   (-431),  ///< �T�[�o�z�X�g���ؒf��A�T�[�o���Ɏ��s�������T�[�o��オ�ł��Ȃ������������̂ŁA�V�����T�[�o�z�X�g�ɐڑ��ł��Ȃ�����
        DWC_ECODE_TYPE_FAILED_CONNECT_GROUPID  =   (-432),  ///< GroupID���w�肵�Ă̐ڑ��Ɏ��s
        DWC_ECODE_TYPE_MATCH_NOTICE  =   (-440),  // �N���C�A���g���m�̐ڑ��ʒm�����s�����\������(�V�W���ł͌��ݒ�`�̂�)
        DWC_ECODE_TYPE_CLOSE         =   (-600),  // �R�l�N�V�����N���[�Y���̃G���[
        DWC_ECODE_TYPE_TRANS_HEADER  =   (-610),  // ���肦�Ȃ���M�X�e�[�g��Reliable�f�[�^����M����
        DWC_ECODE_TYPE_TRANS_BODY    =   (-620),  // ��M�o�b�t�@�I�[�o�[�t���[
        DWC_ECODE_TYPE_TRANS_SEND    =   (-630),  // Reliable���M�Ɏ��s
        DWC_ECODE_TYPE_TRANS_UNKNOWN_PEER =   (-640), ///< �������F�����Ă��Ȃ�gt2�R�l�N�V��������̎�M
        DWC_ECODE_TYPE_AC_FATAL      =   (-700),  // AC��������AC�ł̃G���[�O���[�v�B���̏ꍇ���ʌ��͐�p��state�l�B
        DWC_ECODE_TYPE_OPEN_FILE     =   (-800),  // GHTTP�t�@�C���I�[�v�����s
        DWC_ECODE_TYPE_INVALID_POST  =   (-810),  // GHTTP�����ȑ��M
        DWC_ECODE_TYPE_REQ_INVALID   =   (-820),  // GHTTP�t�@�C����������
        DWC_ECODE_TYPE_UNSPECIFIED   =   (-830),  // GHTTP�ڍוs���̃G���[
        DWC_ECODE_TYPE_BUFF_OVER     =   (-840),  // GHTTP�o�b�t�@�I�[�o�[�t���[
        DWC_ECODE_TYPE_PARSE_URL     =   (-850),  // GHTTP URL�̉�̓G���[
        DWC_ECODE_TYPE_BAD_RESPONSE  =   (-860),  // GHTTP�T�[�o����̃��X�|���X��̓G���[
        DWC_ECODE_TYPE_REJECTED      =   (-870),  // GHTTP�T�[�o����̃��N�G�X�g������
        DWC_ECODE_TYPE_FILE_RW       =   (-880),  // GHTTP���[�J���t�@�C��Read/Write�G���[
        DWC_ECODE_TYPE_INCOMPLETE    =   (-890),  // GHTTP�_�E�����[�h���f
        DWC_ECODE_TYPE_TO_BIG        =   (-900),  // GHTTP�t�@�C���T�C�Y���傫�����ă_�E�����[�h�s��
        DWC_ECODE_TYPE_ENCRYPTION    =   (-910),  // GHTTP�G���N���v�V�����G���[
    	
        // Fatal Error��p���
        DWC_ECODE_TYPE_ALLOC         =     (-1),  // �������m�ێ��s
        DWC_ECODE_TYPE_PARAM         =     (-2),  // �p�����[�^�G���[
        DWC_ECODE_TYPE_SO_SOCKET     =     (-3),  // SO�̃G���[��������GameSpy gt2�̃\�P�b�g�G���[
        DWC_ECODE_TYPE_NOT_INIT      =     (-4),  // ���C�u����������������Ă��Ȃ��B
        DWC_ECODE_TYPE_DUP_INIT      =     (-5),  // ���C�u��������x���������ꂽ�B
        DWC_ECODE_TYPE_WM_INIT       =     (-6),  // WM�̏������Ɏ��s�����B
        DWC_ECODE_TYPE_UNEXPECTED    =     (-9),  // �\�����Ȃ���ԁA��������Unkwon��GameSpy�G���[����
    
    	// 30000��G���[�ԍ��̒�`(�g���@�\�p)
    	DWC_ECODE_SEQ_ADDINS        =   (-30000), // �g���@�\�G���[
    	
    	DWC_ECODE_FUNC_ND            =   (-1000), // DWC_Nd�p�̈�
        DWC_ECODE_FUNC_PROF          =   (-3000), // �s��������`�F�b�N�p�̈�
    	
    	DWC_ECODE_TYPE_ND_SERVER     =   (  0),   // �T�[�o���G���[�R�[�h��Ԃ��Ă���
        DWC_ECODE_TYPE_ND_ALLOC      =   ( -1),   ///< �������m�ێ��s
        DWC_ECODE_TYPE_ND_FATAL      =   ( -9),   ///< ���̑��v���I�G���[
        DWC_ECODE_TYPE_ND_BUSY       =   (-10),   ///< ���̊֐����Ăяo���Ȃ��X�e�[�g�Ŋ֐����Ă�
    	//DWC_ECODE_TYPE_ND_STATE      =   (-10), // ���̊֐����Ăяo���Ȃ��X�e�[�g�Ŋ֐����Ă�(RVLDWC�̃}�[�W)    
        DWC_ECODE_TYPE_ND_HTTP       =   (-20),   ///< HTTP�ʐM�Ɏ��s
        DWC_ECODE_TYPE_ND_BUFFULL    =   (-30),   ///< HTTP�ʐM�o�b�t�@���s�\��
        //DWC_ECODE_TYPE_ND_CANCEL     =   (-40),   ///< �񓯊��������L�����Z������
        DWC_ECODE_TYPE_ND_PARAM      =   (-40),   ///< �֐��ɓn�����p�����[�^���s��
    
    	DWC_ECODE_TYPE_PROF_CONN     =   ( -10),// �s��������`�F�b�N�ŃT�[�o�Ƃ̐ڑ��ɖ�肪������
    	DWC_ECODE_TYPE_PROF_PARSE    =   ( -20),// �s��������`�F�b�N�ŃT�[�o�̉������������Ȃ�����
    	DWC_ECODE_TYPE_PROF_OTHER    =   ( -30) // �s��������`�F�b�N�ł��̑��̃G���[����������
    };


    /* -------------------------------------------------------------------------
            function - external
       ------------------------------------------------------------------------- */

    DWCError    DWC_GetLastError(int* errorCode);
    DWCError    DWC_GetLastErrorEx(int* errorCode, DWCErrorType* errorType);
    void        DWC_ClearError(void);



#ifdef __cplusplus
}
#endif


#endif // DWC_ERROR_H_
