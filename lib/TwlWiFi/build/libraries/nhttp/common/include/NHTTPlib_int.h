/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     NHTTPlib.int.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-07-01#$
  $Rev: 1192 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/
#ifndef __NHTTP_NHTTPLIB_INT_H__
#define __NHTTP_NHTTPLIB_INT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "d_def.h"
#if	defined(NHTTP_FOR_RVL)
#include <types.h>
#include <revolution/ncd.h>
#include <ssl.h>
typedef OSMutex NHTTPMutex;
typedef SSLId   NHTTPSSLId;
#define NHTTP_SSL_ENONE                 SSL_ENONE
#define NHTTP_SSL_VERIFY_NONE           SSL_VERIFY_NONE
#define NHTTP_SSL_ROOTCA_DEFAULT        SSL_ROOTCA_NINTENDO_0
#define NHTTP_SSL_CLIENTCERT_DEFAULT    SSL_CLIENTCERT_NINTENDO_0
#elif defined(NHTTP_FOR_NTR)

#include <nitro/types.h>

#if defined(NHTTP_USE_NSSL)
#include <twlWiFi/nssl.h>
typedef OSMutex NHTTPMutex;
typedef NSSLId  NHTTPSSLId;
#define NHTTP_SSL_ENONE                 NSSL_ENONE
#define NHTTP_SSL_VERIFY_NONE           NSSL_VERIFY_NONE
#define NHTTP_SSL_ROOTCA_DEFAULT        NSSL_ROOTCA_NINTENDO_2
#define NHTTP_SSL_CLIENTCERT_DEFAULT    0 //TODO
#elif defined(NHTTP_USE_CPSSSL)
typedef OSMutex NHTTPMutex;
typedef void*   NHTTPSSLId;
#define NHTTP_SSL_ENONE                 0
#define NHTTP_SSL_VERIFY_NONE           0
#endif

#else
//gen rvliopTypes.h��ǂݍ��܂Ȃ��悤��
#define REVOLUTION_RVLIOPTYPES_H_
#include <iop_types.h>
#include <revolution/utl/signal.h>
#endif

/*---------------------------------------------------------------------------*
 * Definitions
 *---------------------------------------------------------------------------*/

#define LOWERCASE( CHR ) ( (((CHR) >= 'A') & ((CHR) <= 'Z'))? ((CHR)-('A'-'a')) : (CHR) )
#define MIN( AA, BB )	 ( ((AA)>(BB))? (BB) : (AA) )

// ���M�o�b�t�@�̑傫��
// >=14 �ł��邱��
#define LEN_COMMBUF     (1024 * 16)

// URL �p�o�b�t�@�̑傫��
#define LEN_URLBUF      512

// multipart-mixed�̃f���~�^������
// �͂��߂�2������"--"�ł��邱��
#define STR_POSTBOUND   "--t9Sf4yfjf1RtvDu3AA"
// �͂��߂�"--"��������������
#define LEN_POSTBOUND   18
// PROXY�A�h���X�̒���
#define LEN_PROXY_ADDR  256
// host���̍ő咷
#define LEN_HOST_NAME   256
// Basic�F�؂Ŏg�p���郆�[�U�[���̍ő咷
#define LEN_USERNAME  32
// Basic�F�؂Ŏg�p����p�X���[�h�̍ő咷
#define LEN_PASSWORD  32
// Basic�F�؂Ŏg�p���郆�[�U�[���ƃp�X���[�h��Base64Encode�����ۂ̍ő咷
#define LEN_BASIC_USERNAME_PASSWORD ((4*(LEN_USERNAME + LEN_PASSWORD + 1 + 2))/3 + 1)
// SORecv�p�ꎞ��M�o�b�t�@�T�C�Y
#define	LEN_TMP_BUF	(32768)

/*---------------------------------------------------------------------------*
 * Types/Declarations
 *---------------------------------------------------------------------------*/
// �\�P�b�gwrapper�Ŏg�������G���[�R�[�h
enum _NHTTPi_NB_ERROR {
    NHTTPi_NB_SUCCESS                   = 0,
    NHTTPi_NB_ERROR_TIMEOUT             = -1000,
    NHTTPi_NB_ERROR_NETWORK             = -1001,
    NHTTPi_NB_ERROR_CANCELED            = -1002,
    NHTTPi_NB_ERROR_BUFFERFULL          = -1003,
    NHTTPi_NB_ERROR_SSL_ROOTCA          = -1004,
    NHTTPi_NB_ERROR_SSL_CLIENTCERT      = -1005
};  

typedef enum {
    NHTTP_CURRENT_THREAD_IS_NHTTP_THREAD,
    NHTTP_CURRENT_THREAD_IS_NOT_NHTTP_THREAD
} NHTTPThreadIs;
   
typedef struct _NHTTPi_REQLIST {
    struct _NHTTPi_REQLIST*     prev_p;
    struct _NHTTPi_REQLIST*     next_p;
    int                         id;
    NHTTPReq*                   req_p;
    void*                       socket_p;
} NHTTPi_REQLIST;

typedef struct _NHTTPi_DATALIST {
    struct _NHTTPi_DATALIST*    prev_p;
    struct _NHTTPi_DATALIST*    next_p;
    const char*                 label_p;
    const char*                 value_p;
    u32                         length;
    BOOL                        isBinary;
} NHTTPi_DATALIST;

typedef struct _NHTTPi_HDRBUFLIST {
    struct _NHTTPi_HDRBUFLIST*  next_p;
    char                        block[ NHTTP_HDRRECVBUF_BLOCKLEN ];
} NHTTPi_HDRBUFLIST;

typedef struct _NHTTTPi_PostDataBuf {
    char*                       value_p;
    u32                         length;
} NHTTTPi_PostDataBuf;
    
typedef struct _NHTTPConnection	NHTTPConnection;
struct _NHTTPConnection
{
    NHTTPConnectionStatus       status;
    NHTTPError                  nhttpError;
    int                         sslError;

    BOOL                        running;

    /* ���d�l�̊e�탊�\�[�X�n���h�� */
    NHTTPReq                    *req_p;
    NHTTPRes                    *res_p;
    int                         reqId;
    NHTTPConnectionCallback     connectionCallback;
    NHTTPConnection*            next_p;
    NHTTTPi_PostDataBuf         postDataBuf;
    
    // ���d�l�݊� NHTTPBufFull�R�[���o�b�N�֐��p
    u32                         virtualContentLength;   //chunked size�܂�
    // ���d�l�݊� _NHTTPReq����ړ�
    NHTTPReqCallback            reqCallback;
    u16                         padding[6];

    // SORec�p�ꎞ��M�o�b�t�@
    u16                         tmpBuf[(LEN_TMP_BUF/sizeof(u16))] ATTRIBUTE_ALIGN(32);
    u32                         tmpBufOffset;
    u32                         tmpBufRest;
    
    int                         ref;
};

struct _NHTTPRes {
    volatile int        headerLen;      // �w�b�_(�擪����\r\n\r\n�܂�)�̒���[byte]
    volatile int        bodyLen;        // �{�f�B�̒���[byte]
    volatile int        totalBodyLen;   // �{�f�B�̒���[byte]
    volatile int        contentLength;
    int                 isSuccess;
    int                 isHeaderParse;
    int                 httpStatus;     // httpStatus
    unsigned long       recvBufLen;     // recvBuf_p�̑S��
    char*               allHeader_p;
    char*               foundHeader_p;
    char*               recvBuf_p;      // �G���e�B�e�B�{�f�B��M�o�b�t�@
    NHTTPBufFull	bufFull;	// ��M�o�b�t�@���R�[���o�b�N�֐�
    NHTTPFreeBuf	freeBuf;	// ��M�o�b�t�@����R�[���o�b�N�֐�
    NHTTPi_HDRBUFLIST*  hdrBufBlock_p;
    char                hdrBufFirst[ NHTTP_HDRRECVBUF_INILEN ];
    void*               param_p;          // createRequest�Őݒ肵��param
};

struct _NHTTPReq {
    volatile BOOL       doCancel;	// ���f�I���v��

    BOOL                isStarted;      // ���N�G�X�g���s��
    BOOL                isSSL;          // TRUE�Ȃ� https:// �ڑ�
    BOOL                isProxy;        // PROXY�o�R�ł̐ڑ�
    BOOL                isRawData;      // POST�f�[�^�����f�[�^
    
    int                 hostLen;        // URL_p�� top����host���I���܂ł̕�����
    int                 hostDescLen;    // URL_p�� top����ŏ��́u/�v�܂ł̕�����
                                        //  http://abcd.efghi.com:8080/ijk.html
                                        //  |                    |    |
                                        //  0              nLenHost nLenHostDesc
    NHTTPReqMethod      method;         // ���N�G�X�g���\�b�h
    int                 port;           // �|�[�g
    char*               URL_p;          // ���N�G�X�gURL
    char*               hostname_p;     // �z�X�g��
    NHTTPRes*           res_p;          
    NHTTPi_DATALIST*    listHeader_p;   
    NHTTPi_DATALIST*    listPost_p;
    
    char                tagPost[ LEN_POSTBOUND + 2 ];
    
    // Basic�F��
    char                basicUsernamePassword[ LEN_BASIC_USERNAME_PASSWORD];
    u16                 padding1;
    int                 basicUsernamePasswordLen;
    
    // SSL�ڑ����Ɏg�p
    NHTTPSSLId          sslId;
    const char*         clientCertData_p;
    size_t              clientCertSize;
    const char*         privateKeyData_p;
    size_t              privateKeySize;
    const char*         rootCAData_p;
    size_t              rootCASize;
    int                 useBuiltinRootCA;
    int                 useClientCertDefault;
    u32                 verifyOption;
    NHTTPEncodingType   encodingType;

    // 2007.06.15�ǉ�
    u32                 disableVerifyOption;    // SSLDisableVerifyOptionForDebug
    u32                 rootCAId;               // SSLSetBuiltinRootCA
    u32                 clientCertId;           // SSLSetBuiltinClientCert
    
    // PROXY�o�R�ł̐ڑ��Ŏg�p
    char                proxyAddr[ LEN_PROXY_ADDR ];
    int                 proxyPort;
    char                proxyUsernamePassword[ LEN_BASIC_USERNAME_PASSWORD];
    u16                 padding2;
    int                 proxyUsernamePasswordLen;

    // ���d�l�Ή��̂���NHTTPConnection����ړ�
    u32                 soRecvSize;
    
    // ��POST�f�[�^
    const char*         rawPostData_p;
    u32                 rawPostDataLen;
    
    NHTTPPostSend       postSendCallback;    // POST���M�R�[���o�b�N�֐�
};

enum {
    NHTTPi_CommThreadSendData_begin_e,  // �\�P�b�g���J���Ƃ���܂Ŗ߂�
    NHTTPi_CommThreadSendData_error_e,  // ���M���s
    
    NHTTPi_CommThreadSendData_end_e     // �ʏ�I��
};

#define SOCKET 			int
    
       
    
#ifdef __cplusplus
}
#endif

#endif // __NHTTP_NHTTPLIB_INT_H__
