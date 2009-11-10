/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright $(dwc_packed_year) Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/
#ifndef DWCi_GHTTP_H_
#define DWCi_GHTTP_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// GHTTP�̒ʐM���
    typedef enum
    {

        DWC_GHTTP_SOCKET_INIT,          ///< �\�P�b�g��������
        DWC_GHTTP_HOST_LOOKUP,			///< �z�X�g����IP�ɕϊ���
        DWC_GHTTP_LOOKUP_PENDING,       ///< DNS�����҂�
        DWC_GHTTP_CONNECTING,			///< �\�P�b�g�ڑ������҂�
        DWC_GHTTP_SECURING_SESSION,     ///< ���S�ȃ`�����l�����m����
        DWC_GHTTP_SENDING_REQUEST,	    ///< ���N�G�X�g���M��
        DWC_GHTTP_POSTING,				///< �f�[�^���M���i���M���Ȃ��ꍇ�̓X�L�b�v����܂��j
        DWC_GHTTP_WAITING,				///< ���X�|���X�҂�
        DWC_GHTTP_RECEIVING_STATUS,     ///< ���X�|���X�̏�Ԃ���M��
        DWC_GHTTP_RECEIVING_HEADERS,    ///< �w�b�_����M��
        DWC_GHTTP_RECEIVING_FILE,       ///< �t�@�C������M��

        DWC_GHTTP_FALSE = 0             ///< �ʐM���s

    } DWCGHTTPState;

    /**
     *  ���N�G�X�g�G���[�l
     *
     *  @version 1.4.0 �萔�̒l��1�����炵��DWC_GHTTP_IN_ERROR��-1�ɂȂ�悤�ɂ��܂���
     */
    typedef enum
    {

        DWC_GHTTP_ERROR_START           = -8,   ///< -8,
        DWC_GHTTP_FAILED_TO_OPEN_FILE,          ///< �t�@�C�����J���̂Ɏ��s
        DWC_GHTTP_INVALID_POST,                 ///< �����ȑ��M
        DWC_GHTTP_INSUFFICIENT_MEMORY,          ///< �������s��
        DWC_GHTTP_INVALID_FILE_NAME,	        ///< �����ȃt�@�C����
        DWC_GHTTP_INVALID_BUFFER_SIZE,          ///< �����ȃo�b�t�@�T�C�Y
        DWC_GHTTP_INVALID_URL,                  ///< ������URL
        DWC_GHTTP_UNSPECIFIED_ERROR,            ///< �ڍוs���̃G���[

        DWC_GHTTP_IN_ERROR              = -1    ///< �G���[������

    } DWCGHTTPRequestError;

/// �ʐM����
    typedef enum
    {

        DWC_GHTTP_SUCCESS,          ///< ����
        DWC_GHTTP_OUT_OF_MEMORY,    ///< ���������蓖�Ď��s
        DWC_GHTTP_BUFFER_OVERFLOW,  ///< �������ꂽ�o�b�t�@�����������邽�߁A�t�@�C���̎擾���s
        DWC_GHTTP_PARSE_URL_FAILED, ///< URL��̓G���[
        DWC_GHTTP_HOST_LOOKUP_FAILED,///< �z�X�g���������s
        DWC_GHTTP_SOCKET_FAILED,    ///< �\�P�b�g�̍쐬�A�������A�ǂݏo���A�������ݎ��s
        DWC_GHTTP_CONNECT_FAILED,   ///< HTTP�T�[�o�ւ̐ڑ����s
        DWC_GHTTP_BAD_RESPONSE,     ///< HTTP�T�[�o����̃��X�|���X�̉�̓G���[
        DWC_GHTTP_REQUEST_REJECTED, ///< HTTP�T�[�o�̃��N�G�X�g����
        DWC_GHTTP_UNAUTHORIZED,     ///< �t�@�C���擾������
        DWC_GHTTP_FORBIDDEN,        ///< HTTP�T�[�o�̃t�@�C�����M����
        DWC_GHTTP_FILE_NOT_FOUND,   ///< HTTP�T�[�o��̃t�@�C���������s
        DWC_GHTTP_SERVER_ERROR,     ///< HTTP�T�[�o�����G���[
        DWC_GHTTP_FILE_WRITE_FAILED,///< ���[�J���t�@�C���ւ̏������݃G���[
        DWC_GHTTP_FILE_READ_FAILED, ///< ���[�J���t�@�C������̓ǂݏo���G���[
        DWC_GHTTP_FILE_INCOMPLETE,  ///< �_�E�����[�h�̒��f
        DWC_GHTTP_FILE_TOO_BIG,     ///< �t�@�C�����傫�����邽�߃_�E�����[�h�s�\
        DWC_GHTTP_ENCRYPTION_ERROR, ///< �Í����G���[
        DWC_GHTTP_REQUEST_CANCELLED,///< ���N�G�X�g���L�����Z�����ꂽ
        DWC_GHTTP_NUM,

        DWC_GHTTP_MEMORY_ERROR = DWC_GHTTP_NUM + 1           ///< ���������蓖�Ď��s


    } DWCGHTTPResult;

// �֐��^�錾
//---------------------------------------------------------

    /**
     * DWC_PostGHTTPData / DWC_GetGHTTPData / DWC_GetGHTTPDataEx�֐��ɂ��f�[�^��
     * �A�b�v���[�h�A�_�E�����[�h�����������ۂɌĂяo����܂��B
     *
     * �{�R�[���o�b�N�֐���DWC_PostGHTTPData / DWC_GetGHTTPData /
     * DWC_GetGHTTPDataEx�֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B
     *
     * @param   buf     ��M�f�[�^�i�[�o�b�t�@�B�A�b�v���[�h�̎���NULL
     * @param   len     ��M�f�[�^�̃T�C�Y�B�A�b�v���[�h�̎���0
     * @param   result  �A�b�v���[�h�A�_�E�����[�h�̌��ʁB�G���[�����������ꍇ�́A
     *                  DWC�̃G���[�����C�u�����ɂ���ăZ�b�g����Ă��܂��B
     *                  DWC�̃G���[��DWC_GetLastErrorEx�֐��Ŏ擾�ł��܂��B
     * @param   param   DWC_PostGHTTPData / DWC_GetGHTTPData / DWC_GetGHTTPDataEx
     *                  �Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     * @sa      DWC_PostGHTTPData
     * @sa      DWC_GetGHTTPData
     * @sa      DWC_GetGHTTPDataEx
     */
    typedef void (*DWCGHTTPCompletedCallback)(const char* buf, int len, DWCGHTTPResult result, void* param);

    /**
     * DWC_GetGHTTPDataEx�֐����Ăяo�������GHTTP���C�u�����̌��݂̏�Ԃ�ʒm���܂��B
     *
     * ���N�G�X�g���M���A�f�[�^��M���ȂǁA�_�E�����[�h�V�[�P���X�̏�Ԃ��ω�����
     * �x�ɌĂяo����܂��B�{�R�[���o�b�N�֐���DWC_GetGHTTPDataEx�֐��ɂ���Đݒ�
     * ���邱�Ƃ��ł��܂��B
     *
     * @param   state           GHTTP�̒ʐM���
     *                          DWC_GetGHTTPState�֐��̕Ԃ�l�Ɠ����l��Ԃ�
     * @param   buf             ��M�f�[�^�i�[�o�b�t�@
     * @param   len             ��M�f�[�^�̃T�C�Y
     * @param   bytesReceived   ��M���ꂽ���o�C�g��
     * @param   totalSize       �t�@�C���̃g�[�^���T�C�Y�B�s���ȏꍇ��-1
     * @param   param           DWC_GetGHTTPDataEx�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     * @sa  DWC_GetGHTTPDataEx
     * @sa  DWC_GetGHTTPState
     */
    typedef void (*DWCGHTTPProgressCallback)(DWCGHTTPState state, const char* buf, int len, int bytesReceived, int totalSize, void* param);


// �^�錾
//---------------------------------------------------------

    typedef void*   DWCGHTTPPost;


// �֐��錾
//---------------------------------------------------------

    BOOL	DWC_InitGHTTP           ( const char* gamename  );
    BOOL	DWC_ShutdownGHTTP       ( void );
    BOOL	DWC_ProcessGHTTP        ( void );
    void    DWC_GHTTPNewPost        ( DWCGHTTPPost* post );
    BOOL    DWC_GHTTPPostAddString  ( DWCGHTTPPost* post, const char* key, const char* value );
    BOOL    DWC_GHTTPPostAddFileFromMemory( DWCGHTTPPost *post, const char *key, const char *buffer, int bufferlen, const char *filename, const char *contentType );
    void    DWC_CancelGHTTPRequest  ( int req );
    DWCGHTTPState DWC_GetGHTTPState ( int req );
    int  	DWC_PostGHTTPData       ( const char* url, DWCGHTTPPost* post, DWCGHTTPCompletedCallback completedCallback, void* param );
    int     DWC_GetGHTTPData        ( const char* url, DWCGHTTPCompletedCallback completedCallback, void* param );
    int 	DWC_GetGHTTPDataEx      ( const char* url, int bufferlen, BOOL buffer_clear, DWCGHTTPProgressCallback progressCallback, DWCGHTTPCompletedCallback completedCallback, void* param );
    int 	DWC_GetGHTTPDataEx2     ( const char* url, int bufferlen, BOOL buffer_clear, DWCGHTTPPost *post, DWCGHTTPProgressCallback progressCallback, DWCGHTTPCompletedCallback completedCallback, void* param );

// �^�錾
//---------------------------------------------------------

    typedef struct
    {

        void* param;
        DWCGHTTPCompletedCallback completedCallback;
        DWCGHTTPProgressCallback  progressCallback;
        BOOL buffer_clear;

    }
    DWCGHTTPParam;

	// windows�p�f�o�b�O���
#ifdef _WIN32
	typedef enum DWC_GHTTP_ERROR_TYPE
	{
		DWC_GHTTP_ERROR_TYPE_CONNECTION,
		DWC_GHTTP_ERROR_TYPE_SERVER
	}
	DWC_GHTTP_ERROR_TYPE;

	typedef struct DWC_GhttpDebugInfo
	{
		DWC_GHTTP_ERROR_TYPE errorType;
		int debugBufLen;
		char debugBuf[4096];
	}
	DWC_GhttpDebugInfo;
#endif // _WIN32

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DWCi_GHTTP_H
