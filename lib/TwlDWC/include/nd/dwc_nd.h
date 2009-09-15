/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_ND_H_
#define DWC_ND_H_

#ifdef __cplusplus
extern "C"
{
#endif

    /* -------------------------------------------------------------------------
            define
       ------------------------------------------------------------------------- */

/// �t�@�C���擾���Ɏw�肷��t�@�C�����̍ő啶����
#define DWC_ND_FILENAME_LEN                 32

/// �t�@�C���擾�O�Ɏw�肷�鑮���̍ő啶����
#define DWC_ND_FILEATTR_LEN                 10

/// �t�@�C���Ɏw�肳��Ă���UTF16LE�ŕ\�����ꂽ�������̍ő啶����
#define DWC_ND_FILEEXPLSIN_LEN              50

/// �_�E�����[�h���C�u���������Ŏg�p����X���b�h�̗D��x
#define DWC_ND_THREAD_PRIORITY              17

/// DL�Q�[���R�[�h(gamecd)�̃o�C�g��
#define DWC_ND_LENGTH_GAMECODE				4

/// �閧��(passwd)�̃o�C�g��
#define DWC_ND_LENGTH_PASSWORD				16

    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */

    /**
     * �_�E�����[�h���C�u�����X�e�[�g
     *
     * �_�E�����[�h���C�u�����̏�Ԃ������񋓌^�ł��B
     */
    typedef enum
    {
        DWC_ND_STATE_NOTINITIALIZED,    ///< �_�E�����[�h���C�u��������������
        DWC_ND_STATE_READY,             ///< ���������������A���̃_�E�����[�h���C�u�����֐����Ăׂ���
        DWC_ND_STATE_BUSY,              ///< �񓯊����������s��
        DWC_ND_STATE_COMPLETE,          ///< �񓯊�����������Ɋ�������
        DWC_ND_STATE_ERROR,             ///< �񓯊��������ɃG���[���������A��������
        DWC_ND_STATE_MAX
    } DWCNdState;


    /**
     * �_�E�����[�h���C�u�����G���[�񋓎q
     *
     * �_�E�����[�h���C�u�����Ŕ��������G���[�������񋓌^�ł��B
     */
    typedef enum
    {
        DWC_ND_ERROR_NONE,     ///< ���s���̏�������������
        DWC_ND_ERROR_ALLOC,    ///< �������m�ۂɎ��s����
        DWC_ND_ERROR_BUSY,     ///< �{�����̊֐����Ăяo���Ȃ�STATE�ŌĂяo����
        DWC_ND_ERROR_HTTP,     ///< HTTP�ʐM�ɃG���[����������
        DWC_ND_ERROR_BUFFULL,  ///< (�����G���[)�_�E�����[�h�o�b�t�@������Ȃ�
        DWC_ND_ERROR_PARAM,	   ///< �s���ȃp�����[�^�ɂ��G���[����������
        DWC_ND_ERROR_CANCELED, ///< �����̃L�����Z������������
        DWC_ND_ERROR_DLSERVER, ///< �T�[�o���G���[�R�[�h��Ԃ��Ă���
        DWC_ND_ERROR_FATAL,	   ///< ����`�̒v���I�ȃG���[����������
        DWC_ND_ERROR_MAX
    } DWCNdError;

    /**
     * �_�E�����[�h���C�u�����i�s�񋓎q
     *
     * �_�E�����[�h���C�u�����ɂ���Ēʒm�R�[���o�b�N�֐����Ăяo���ꂽ�ہA
     * �ǂ̂悤�ȏ��������s���ł��������������񋓌^�ł��B
     */
    typedef enum
    {
        DWC_ND_CBREASON_INITIALIZE,		///< �_�E�����[�h���C�u�������������ɒʒm�R�[���o�b�N���Ăяo���ꂽ
        DWC_ND_CBREASON_GETFILELISTNUM, ///< �_�E�����[�h�\�ȃt�@�C���������_�E�����[�h���ɒʒm�R�[���o�b�N���Ăяo���ꂽ
        DWC_ND_CBREASON_GETFILELIST,    ///< �t�@�C�����ꗗ���_�E�����[�h���ɒʒm�R�[���o�b�N���Ăяo���ꂽ
        DWC_ND_CBREASON_GETFILE,        ///< �t�@�C�����_�E�����[�h���ɒʒm�R�[���o�b�N���Ăяo���ꂽ
        DWC_ND_CBREASON_CLEANUP,        ///< �N���[���A�b�v�������ɌĂяo���ꂽ
        DWC_ND_CBREASON_MAX
    } DWCNdCallbackReason;



    /* -------------------------------------------------------------------------
            struct
       ------------------------------------------------------------------------- */
    /**
     * �X�̃t�@�C�������i�[����\���̂ł��B
     * 
     * �_�E�����[�h���C�u�����ɂ���Ď擾�����t�@�C�������i�[����\���̂ł��B
     */
    typedef struct
    {
        char            name[DWC_ND_FILENAME_LEN+1];		    ///< �k�������I�[�����Œ�32������ASCII������ŕ\�����ꂽ�t�@�C����
        short           explain[DWC_ND_FILEEXPLSIN_LEN+1];      ///< �k�������I�[�����Œ�50������UTF16BE������ŕ\�����ꂽ�t�@�C���̐�����
        char            param1[DWC_ND_FILEATTR_LEN+1];		    ///< �k�������I�[�����Œ�10������ASCII������ŕ\�����ꂽ����������1
        char            param2[DWC_ND_FILEATTR_LEN+1];		    ///< �k�������I�[�����Œ�10������ASCII������ŕ\�����ꂽ����������2
        char            param3[DWC_ND_FILEATTR_LEN+1];		    ///< �k�������I�[�����Œ�10������ASCII������ŕ\�����ꂽ����������3
        unsigned int    size;						            ///< �t�@�C���T�C�Y
    }
    DWCNdFileInfo;


    /* -------------------------------------------------------------------------
            function - external
       ------------------------------------------------------------------------- */

    /**
     * �_�E�����[�h���C�u�����̊e��񓯊����������������Ƃ��ɌĂяo�����R�[���o�b�N�֐��ł��B
     *
     * �_�E�����[�h���C�u�����ɂ���ČĂяo�����ʒm�R�[���o�b�N�֐��^�ł��B�񓯊������̌��ʂ��ʒm����܂��B
     * �{�R�[���o�b�N��error�p�����[�^���ADWC_ND_ERROR_NONE�ȊO�̂Ƃ��ADWC_GetLastErrorEx�֐��ŃG���[�����擾���A�G���[�����^�C�v�ɉ����ăG���[���b�Z�[�W�ƂƂ��ɃG���[�R�[�h��\���A�y�у��C�u�����̏I�����s���Ă��������B
     *
     * �{�R�[���o�b�N�����_�E�����[�h���C�u�����̊֐����ĂԂ��Ƃ��ł��܂��B
     *
     * Param:  reason �ʒm�R�[���o�b�N�֐����Ăяo���ꂽ�ۂǂ̂悤�ȏ��������s���ł��������������l
     * Param:  error �ʒm�R�[���o�b�N�֐����Ăяo���ꂽ�ۂɔ��������G���[�������l
     * Param:  servererror �T�[�o����ʒm���ꂽ�G���[�R�[�h�B error��DWC_ND_ERROR_DLSERVER�ł͂Ȃ��Ƃ���-1�ƂȂ�܂��B
     *
     * See also:  DWC_NdProcess
     * See also:  DWC_NdInitAsync
     * See also:  DWC_NdCleanupAsync
     * See also:  DWCNdCallbackReason
     * See also:  DWCNdError
    */
    typedef void (*DWCNdCallback)( DWCNdCallbackReason reason, DWCNdError error, int servererror );


    BOOL        DWC_NdInitAsync             (DWCNdCallback callback, const char *gamecd, const char *passwd);
    void        DWC_NdProcess               (void);
    BOOL        DWC_NdCleanupAsync          (void);
    BOOL        DWC_NdSetAttr               (const char* attr1, const char* attr2, const char* attr3);
    BOOL        DWC_NdGetFileListNumAsync   (int* entrynum);
    BOOL        DWC_NdGetFileListAsync      (DWCNdFileInfo* filelist, unsigned int offset, unsigned int num);
    BOOL        DWC_NdGetFileAsync          (DWCNdFileInfo* fileinfo, char* buf, unsigned int size);
    BOOL        DWC_NdGetFileExAsync          (DWCNdFileInfo* fileinfo, char* buf, unsigned int size, const char*opt);
    BOOL        DWC_NdCancelAsync           (void);
    BOOL        DWC_NdGetProgress           (u32* received, u32* contentlen);

#ifdef __cplusplus
}
#endif

#endif // DWC_ND_H_
