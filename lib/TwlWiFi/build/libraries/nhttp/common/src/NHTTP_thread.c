/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     NHTTP_thread.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include "nhttp.h"
#include "NHTTPlib_int.h"
#include "NHTTPlib_int_socket.h"
#include "d_nhttp_common.h"
#include "d_nhttp_private.h"
#include "NHTTP_bgnend.h"
#include "NHTTP_list.h"
#include "NHTTP_response.h"
#include "NHTTP_request.h"
#include "NHTTP_thread.h"
#include "NHTTP_control.h"
#include "NHTTP_recvbuf.h"
#include "NHTTP_nonport.h"


/*---------------------------------------------------------------------------*
    Constants
 *---------------------------------------------------------------------------*/
static const char STR_POST_DISPOS[] = "Content-Disposition: form-data; name=\"";
static const char STR_POST_TYPE_BIN[] = "Content-Type: application/octet-stream\r\nContent-Transfer-Encoding: binary\r\n";
static const char STR_POST_TYPE_URLENCODE[] = "Content-Type: application/x-www-form-urlencoded\r\n";
static const char STR_POST_TYPE_MULTIPART[] = "Content-Type: multipart/form-data; boundary=";

#define TMP_PROXY_HEADER_BUF_SIZE       (512)   // PROXY�T�[�o�[�ڑ�
#define TMP_CONTENT_LENGTH_BUF_SIZE     (12)    // ContentLength�𕶎���ɒu�������邽�߂̈ꎞ�o�b�t�@�T�C�Y

/*---------------------------------------------------------------------------*
    Forward references for static functions
 *---------------------------------------------------------------------------*/
static int  NHTTPi_CheckHeaderEnd               ( char* buf_p, int bufwp );
static int  NHTTPi_SaveBuf                      ( NHTTPReq* req_p, char* commBuf_p, const SOCKET socket, int* bufwp_p, const char* src_p, const int srclen);
static int  NHTTPi_SkipLineBuf                  ( NHTTPMutexInfo* mutexInfo_p, NHTTPReq* req, const SOCKET socket );
static BOOL NHTTPi_GetPostContentlength         ( NHTTPMutexInfo* mutexInfo_p, NHTTPReq* req_p, const char* label_p, u32* contentLength, const int sendtype );
static int  NHTTPi_SendPostData                 ( NHTTPMutexInfo* mutexInfo_p, NHTTPReq* req_p, char* commBuf_p, const char* label_p, const SOCKET socket, int* bufwp_p, const int sendtype );
static BOOL NHTTPi_BufFull                      ( NHTTPMutexInfo* mutexInfo_p, NHTTPRes* res_p );
static int  NHTTPi_SendData                     ( NHTTPThreadData* threadData_p, const char* src_p, int srclen );       // �f�[�^���M
static int  NHTTPi_SendProxyAuthorization       ( NHTTPThreadData* threadData_p );      // �v���L�V�F�ؑ��M
static int  NHTTPi_SendBasicAuthorization       ( NHTTPThreadData* threadData_p );      // �x�[�V�b�N�F�ؑ��M
static int  NHTTPi_SendProxyConnectMethod       ( NHTTPThreadData* threadData_p );      // PROXY CONNECT���\�b�h���M
static int  NHTTPi_RecvProxyConnectHeader       ( NHTTPThreadData* threadData_p );      // PROXY CONNECT�w�b�_�[��M
static int  NHTTPi_SendHeaderList               ( NHTTPThreadData* threadData_p );      // �t���w�b�_���M
static int  NHTTPi_SendProcPostDataRaw          ( NHTTPThreadData* threadData_p );      // �R�[���o�b�N�ɂ�鐶�f�[�^���M
static int  NHTTPi_SendProcPostDataBinary       ( NHTTPThreadData* threadData_p );      // �R�[���o�b�N�ɂ��o�C�i���f�[�^���M
static int  NHTTPi_SendProcPostDataAscii        ( NHTTPThreadData* threadData_p );      // �R�[���o�b�N�ɂ��A�X�L�[�f�[�^���M
static void NHTTPi_ThreadInit                   ( NHTTPThreadData* threadData_p );      // �X���b�h����������
static void NHTTPi_ThreadReqEnd                 ( NHTTPThreadData* threadData_p );      // �X���b�h���ł̃��N�G�X�g�I������
static BOOL NHTTPi_ThreadExecReqQueue           ( NHTTPThreadData* threadData_p );      // �L���[���X�g�̐擪������Ă��āA���s���ɂ���
static BOOL NHTTPi_ThreadHostAddrProc           ( NHTTPThreadData* threadData_p );      // �z�X�g�A�h���X�ݒ�
static BOOL NHTTPi_ThreadConnectProc            ( NHTTPThreadData* threadData_p );      // �z�X�g�ɐڑ�
static int  NHTTPi_ThreadProxyProc              ( NHTTPThreadData* threadData_p );      // �v���L�V�T�[�o�[�ڑ�
static int  NHTTPi_ThreadSendProc               ( NHTTPThreadData* threadData_p );      // HTTP�w�b�_�[���M
static int  NHTTPi_ThreadRecvHeaderProc         ( NHTTPThreadData* threadData_p );      // HTTP�w�b�_�[��M
static int  NHTTPi_ThreadParseHeaderProc        ( NHTTPThreadData* threadData_p );      // HTTP�w�b�_�[���
static int  NHTTPi_ThreadRecvBodyProc           ( NHTTPThreadData* threadData_p );      // HTTP�{�f�B��M

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_InitThreadInfo
  Description:  NHTTPThreadInfo�\���̏����ݒ�֐�
  Arguments:    threadInfo_p	NHTTPThreadInfo�\���̂ւ̃|�C���^
  Returns:      
 *-------------------------------------------------------------------------*/
extern void NHTTPi_InitThreadInfo(NHTTPThreadInfo* threadInfo_p)
{
    NHTTPi_IsCreateCommThreadMessageQueueOff(threadInfo_p);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_IsCreateCommThreadMessageQueueOn

  Description:  �X���b�h�̃��b�Z�[�W�L���[�̐����t���O ON�֐�
  
  Arguments:    
                
  Returns:
 *-------------------------------------------------------------------------*/
extern void NHTTPi_IsCreateCommThreadMessageQueueOn(NHTTPThreadInfo* threadInfo_p)
{
    threadInfo_p->isCreateCommThreadMessageQueue = TRUE;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_IsCreateCommThreadMessageQueueOff

  Description:  �X���b�h�̃��b�Z�[�W�L���[�̐����t���O OFF�֐�
  
  Arguments:    
                
  Returns:
 *-------------------------------------------------------------------------*/
extern void NHTTPi_IsCreateCommThreadMessageQueueOff(NHTTPThreadInfo* threadInfo_p)
{
    threadInfo_p->isCreateCommThreadMessageQueue = FALSE;
}
/*-------------------------------------------------------------------------*
  Name:         NHTTPi_IsCreateCommThreadMessageQueue

  Description:  �X���b�h�̃��b�Z�[�W�L���[�̐����t���O�擾�֐�
  
  Arguments:    
                
  Returns:      �X���b�h�̃��b�Z�[�W�L���[�������ς݂Ȃ�	TRUE
  		�X���b�h�̃��b�Z�[�W�L���[���������Ȃ�		FALSE
 *-------------------------------------------------------------------------*/
extern int NHTTPi_IsCreateCommThreadMessageQueue(NHTTPThreadInfo* threadInfo_p)
{
    return threadInfo_p->isCreateCommThreadMessageQueue;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_GetProgress
  Description:  ���ݐi�s����HTTP���N�G�X�g�����̐i�s�x(��M�o�C�g�������Content-Length�̒l)���擾����
  Arguments:    sysInfo_p
                received_p	��M�ς�HTTP BODY�̈�T�C�Y���i�[����̈�ւ̃|�C���^
                contentlen_p	Content-Length���i�[����̈�ւ̃|�C���^
  Returns:      TRUE	HTTP���N�G�X�g�i�s�x�擾�ɐ�������
                FALSE	HTTP���N�G�X�g�L���[����ŁA����HTTP���N�G�X�g�����s����Ă��Ȃ�
 *-------------------------------------------------------------------------*/
extern BOOL NHTTPi_GetProgress(NHTTPSysInfo* sysInfo_p, u32* received_p, u32* contentlen_p)
{
    BOOL			rc = FALSE;
    NHTTPMutexInfo*		mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReqInfo*		reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    volatile NHTTPi_REQLIST*	reqCurrent_p = reqInfo_p->reqCurrent;

    NHTTPi_ASSERT(NHTTPi_IsOpened(NHTTPi_GetBgnEndInfoP(sysInfo_p)));
    NHTTPi_ASSERT(received_p);
    NHTTPi_ASSERT(contentlen_p);
    
    *received_p = 0;
    *contentlen_p = 0;

    NHTTPi_lockReqList(mutexInfo_p);
    {
        if (reqCurrent_p)
        {
            if (reqCurrent_p->req_p->res_p->contentLength != 0)
            {
                *received_p = (u32)reqCurrent_p->req_p->res_p->bodyLen;
                *contentlen_p = (u32)((reqCurrent_p->req_p->res_p->contentLength == 0xffffffff)? 0 : reqCurrent_p->req_p->res_p->contentLength);
            }
            rc = TRUE;
        }
        else
        {
            NHTTPListInfo*	listInfo_p = NHTTPi_GetListInfoP(sysInfo_p);
            rc = (listInfo_p->reqQueue != NULL);
        }
    }
    NHTTPi_unlockReqList(mutexInfo_p);
    
    return rc;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_CheckHeaderEnd
  Description:  �w�b�_�[�I�[�iCR,LF,CRLF�j�𒲂ׂ�
  Arguments:    buf_p   �����Ώۃo�b�t�@�̃|�C���^
                bufwp   �����J�n�ʒu
  Returns:      TRUE    �w�b�_�[�I�[������
                FALSE   �w�b�_�[�I�[���Ȃ�
 *---------------------------------------------------------------------------*/
static int NHTTPi_CheckHeaderEnd(char* buf_p, const int bufwp)
{
    if ((buf_p[(bufwp - 2) & 3] == '\r') &&
        (buf_p[(bufwp - 1) & 3] == '\r'))
    {
        DEBUG_PRINTF("[%s]:CR\n", __func__);
        return TRUE;
    }
    else if ((buf_p[(bufwp - 2) & 3] == '\n') &&
             (buf_p[(bufwp - 1) & 3] == '\n'))
    {
        DEBUG_PRINTF("[%s]:LF\n", __func__);
        return TRUE;
    }
    else if ((buf_p[(bufwp - 4) & 3] == '\r') &&
             (buf_p[(bufwp - 3) & 3] == '\n') &&
             (buf_p[(bufwp - 2) & 3] == '\r') &&
             (buf_p[(bufwp - 1) & 3] == '\n'))
    {
        DEBUG_PRINTF("[%s]:CRLF\n", __func__);
        return TRUE;
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SaveBuf
  Description:  ���M�f�[�^��commBuf_p�ɒ��߁A�����ς��ɂȂ����瑗�M
  Arguments:    req_p           ���N�G�X�g�\���̂̃|�C���^
                commBuf_p       ���M�f�[�^�~�σo�b�t�@
                socket          �\�P�b�gID
                bufwp_p         ���M�f�[�^�̒~�ϗ�
                src_p           ���M���e
                srclen          ���M���e�̃T�C�Y
  Returns:      0�ȉ�           �G���[�A�܂��̓L�����Z��
                ���̒l          ���M���e�̃T�C�Y
 *---------------------------------------------------------------------------*/
static int NHTTPi_SaveBuf(NHTTPReq* req_p, char* commBuf_p, const SOCKET socket, int* bufwp_p, const char* src_p, const int srclen)
{
    int		n;
    int		len;
    int		sendbyte;
    
    for (len = srclen; len > 0;)
    {
        if (req_p->doCancel)
        {
            DEBUG_PRINTF("[%s][%d] �L�����Z������������܂���\n", __func__, __LINE__);
            return -1;
        }
        
        n = LEN_COMMBUF - *bufwp_p;
        n = MIN(len, n);
        NHTTPi_memcpy(&commBuf_p[*bufwp_p], src_p, n);
        *bufwp_p += n;
        src_p += n;
        len -= n;
        
        if (*bufwp_p == LEN_COMMBUF)
        {
            sendbyte = NHTTPi_SocSend(req_p, socket, commBuf_p, LEN_COMMBUF, 0);
            if (sendbyte <= 0)
            {
                return sendbyte;
            }
            *bufwp_p -= sendbyte;
        }
    }
    
    return srclen;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SkipLineBuf
  Description:  ���s�̎��܂Ŏ�M���ēǂݔ�΂�
  Arguments:    req_p   ���N�G�X�g�|�C���^
                socket  �\�P�b�gID
  Returns:      0�ȉ�   �G���[
                ���̒l  ��M�o�C�g��
 *---------------------------------------------------------------------------*/
static int NHTTPi_SkipLineBuf(NHTTPMutexInfo* mutexInfo_p, NHTTPReq* req_p, const SOCKET socket)
{
    char buf[2];
    int p;
    int recvbyte;
    int rc = 0;

    buf[0] = 0;
    buf[1] = 0;
    for (p=0; !((buf[p&1] == '\r') && (buf[(p-1)&1] == '\n')); p++)
    {
        recvbyte = NHTTPi_SocRecv(mutexInfo_p, req_p, socket, &buf[p&1], 1, 0);
        if (recvbyte <= 0) return recvbyte;
        rc += recvbyte;
    }
    return rc;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_GetPostContentlength
  Description:  POST�f�[�^���M�̃T�C�Y����
  Arguments:    mutexInfo_p
                req_p           ���N�G�X�g�|�C���^
                label_p         �����Ώۂ�POST�f�[�^�̃��x��
                contentLength   �������ʂ̃T�C�Y
                sendtype        POST�̃^�C�v
  Returns:      TRUE    ��������
                FALSE   �������s
 *---------------------------------------------------------------------------*/
static BOOL NHTTPi_GetPostContentlength(NHTTPMutexInfo* mutexInfo_p, NHTTPReq* req_p, const char* label_p, u32* contentLength, const int sendtype)
{
    int                 offset = 0;
    u32                 length = 0;
    char*               value_p = NULL;
    NHTTPConnection*	connection_p = NHTTPi_Request2Connection(mutexInfo_p, req_p);
    DEBUG_PRINTF("[%s]\n", __func__);

    if (connection_p == NULL) return FALSE;
    
    connection_p->postDataBuf.value_p = NULL;
    
    while (1)
    {
        // �����ɃL�����Z���̃`�F�b�N������ĉ����������߂Ă݂�
        if (req_p->doCancel)
        {
            DEBUG_PRINTF("%s �L�����Z������������܂���\n", __func__);
            return FALSE;
        }

        /** NHTTPCreateConnection�g�p */
        connection_p->postDataBuf.length = 0;
        if (NHTTPi_PostSendCallback(mutexInfo_p, connection_p, label_p, offset) < 0)
        {
            return FALSE;
        }
        else
        {
            value_p = connection_p->postDataBuf.value_p;
            length = connection_p->postDataBuf.length;
        }

        if (length == 0)
        {
            break;
        }
        if (value_p == NULL)
        {
            return FALSE;
        }
        offset += length;
        
        switch (sendtype)
        {
        case SEND_TYPE_RAW_e:
        case SEND_TYPE_MULTIPART_e:
            *contentLength += length;
            break;
        case SEND_TYPE_URLENCODE_e:
            *contentLength += NHTTPi_getUrlEncodedSize2(value_p, (int)length);
            break;
        }
    }
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SendPostData
  Description:  POST�f�[�^���M
  Arguments:    mutexInfo_p
                req_p           ���N�G�X�g�|�C���^
                commBuf_p       ���M�o�b�t�@
                label_p         ���MPOST�f�[�^�̃��x��
                socket          �\�P�b�gID
                bufwp_p         ���M�o�b�t�@�ʒu�̃|�C���^
                sendtype        POST�̃^�C�v
  Returns:      NHTTP_SEND_ERROR_POST_CALLBACK_e        �G���[
                NHTTP_SEND_NOTCONN_e                    ���ڑ�
                NHTTP_SEND_SUCCESS_e                    ���� 
 *---------------------------------------------------------------------------*/
static int NHTTPi_SendPostData(NHTTPMutexInfo* mutexInfo_p, NHTTPReq* req_p, char* commBuf_p, const char* label_p, const SOCKET socket, int* bufwp_p, const int sendtype)
{
    int			offset = 0;
    u32			length = 0;
    char*		value_p = NULL;
    int			stat = 0;
    char		urlencode[3];
    int			p,n;
    NHTTPConnection*	connection_p = NHTTPi_Request2Connection(mutexInfo_p, req_p);
    
    DEBUG_PRINTF("[%s]\n", __func__);

    if (connection_p == NULL) return NHTTP_SEND_ERROR_POST_CALLBACK_e;

    connection_p->postDataBuf.value_p = NULL;
    
    while (1)
    {
        // �����ɃL�����Z���̃`�F�b�N������ĉ����������߂Ă݂�
        if (req_p->doCancel)
        {
            DEBUG_PRINTF("%s �L�����Z������������܂���\n", __func__);
            return NHTTP_SEND_ERROR_POST_CALLBACK_e;
        }

        connection_p->postDataBuf.length = 0;
        if (NHTTPi_PostSendCallback(mutexInfo_p, connection_p, label_p, offset) < 0)
        {
            return NHTTP_SEND_ERROR_POST_CALLBACK_e;
        }
        else
        {
            value_p = connection_p->postDataBuf.value_p;
            length = connection_p->postDataBuf.length;
        }

        if (length == 0)
        {
            break;
        }
        if (value_p == NULL)
        {
            return NHTTP_SEND_ERROR_POST_CALLBACK_e;
        }
        offset += length;
        // ���M
        switch (sendtype)
        {
        case SEND_TYPE_MULTIPART_e:
        case SEND_TYPE_RAW_e:
            stat = NHTTPi_SaveBuf(req_p, commBuf_p, socket, bufwp_p, value_p, (int)length);
            if (stat < 0)
            {
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return NHTTP_SEND_ERROR_e;
            }
            else if (stat == 0)
            {
                return NHTTP_SEND_NOTCONN_e;
            }
            break;
        case SEND_TYPE_URLENCODE_e:
            for (p=0; p < length; p++)
            {
                NHTTPi_memclr(urlencode, 3);
                n = NHTTPi_encodeUrlChar( urlencode, *(value_p + p) );
                stat = NHTTPi_SaveBuf(req_p, commBuf_p, socket, bufwp_p, urlencode, n);
                if (stat < 0)
                {
                    DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                    return NHTTP_SEND_ERROR_e;
                }
                else if (stat == 0)
                {
                    return NHTTP_SEND_NOTCONN_e;
                }
            }
            break;
        }
    }

    return NHTTP_SEND_SUCCESS_e;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_BufFull
  Description:  ��M�o�b�t�@�Ċm��
  Arguments:    osRvlInfo_p
                res_p
  Returns:      TRUE    �Ċm�ہA�܂��͎�M�o�b�t�@�������ς��ł͖���
                FALSE   �Ċm�ۂł��Ȃ�
 *---------------------------------------------------------------------------*/
static BOOL NHTTPi_BufFull(NHTTPMutexInfo* mutexInfo_p, NHTTPRes* res_p)
{
    BOOL	ret = FALSE;
    BOOL	full = NHTTPi_isRecvBufFull(res_p, res_p->bodyLen);

    DEBUG_PRINTF("[%s]: %d(%d) %d %d(%d,%d)\n",
                 __func__,
                 (res_p->recvBufLen <= 0),
                 res_p->recvBufLen,
                 (res_p->recvBuf_p == NULL),
                 full,
                 res_p->recvBufLen,
                 res_p->bodyLen);
    
    if ((res_p->recvBufLen <= 0) || (res_p->recvBuf_p == NULL) || (full))
    {
        NHTTPConnection*	connection_p = NHTTPi_Response2Connection(mutexInfo_p, res_p);
        
        /** NHTTPCreateConnection�g�p */
        if (connection_p != NULL)
        {
            NHTTPi_BufferFullCallback(mutexInfo_p, connection_p);
            if ((res_p->recvBuf_p != NULL) &&
                (res_p->recvBufLen > 0) &&
                (NHTTPi_isRecvBufFull(res_p, res_p->bodyLen) == FALSE))
            {
                ret = TRUE;
            }
        }
    }
    else if (full == FALSE)
    {
        DEBUG_PRINTF("[%d]:buf is not full\n", __LINE__);
        ret = TRUE;
    }
    DEBUG_PRINTF("========= end : %d\n", ret);
    
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SendData
  Description:  �f�[�^���M
  Arguments:    threadData_p
                src_p
                srclen
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int NHTTPi_SendData(NHTTPThreadData* threadData_p, const char* src_p, int srclen)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    char*		commBuf_p = (char*)&(threadInfo_p->commBuf[0]);
    int                 stat = 0;

    if (srclen == 0 ) {
        // ����T�C�Y���O�̏ꍇ�͑��M���Ȃ�
        // �����̂܂܏������i�ނ�NHTTP_SEND_NOTCONN_e�ɂȂ��Ă��܂�����
        return NHTTP_SEND_SUCCESS_e;
    }
    
    stat =  NHTTPi_SaveBuf( req_p, commBuf_p, bgnEndInfo_p->socket, &(threadData_p->sendBufwp), src_p, srclen);
    DEBUG_PRINTF("[%s][%d] [%s] stat [%d] srclen [%d]\n", __func__, threadData_p->id, src_p, stat, srclen);
    
    if (stat < 0)
    {
        return NHTTP_SEND_ERROR_e;
    }
    else if (stat == 0)
    {
        return NHTTP_SEND_NOTCONN_e;
    }
    else
    {
        return NHTTP_SEND_SUCCESS_e;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SendProxyAuthorization
  Description:  �v���L�V�F�ؑ��M
  Arguments:    threadData_p
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int NHTTPi_SendProxyAuthorization(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    int                 sendStatus;

    // �T�C�Y���O�̏ꍇ�͑��M����K�v�Ȃ�
    if (req_p->proxyUsernamePasswordLen == 0)
    {
        return NHTTP_SEND_SUCCESS_e;
    }
        
    sendStatus = NHTTPi_SendData( threadData_p, "Proxy-Authorization: Basic ", 27);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, req_p->proxyUsernamePassword, req_p->proxyUsernamePasswordLen);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    
    return NHTTP_SEND_SUCCESS_e;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SendBasicAuthorization
  Description:  �x�[�V�b�N�F�ؑ��M
  Arguments:    threadData_p
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int NHTTPi_SendBasicAuthorization(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    int                 sendStatus;

    // �T�C�Y���O�̏ꍇ�͑��M����K�v�Ȃ�
    if (req_p->basicUsernamePasswordLen == 0)
    {
        return NHTTP_SEND_SUCCESS_e;
    }
    
    sendStatus = NHTTPi_SendData(threadData_p, "Authorization: Basic ", 21);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, req_p->basicUsernamePassword, req_p->basicUsernamePasswordLen);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    
    return NHTTP_SEND_SUCCESS_e;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SendProxyConnectMethod
  Description:  PROXY CONNECT���\�b�h���M
  Arguments:    threadData_p
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int  NHTTPi_SendProxyConnectMethod(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    char*		commBuf_p = (char*)&(threadInfo_p->commBuf[0]);
    int                 sendStatus = NHTTP_SEND_SUCCESS_e;
    char                portstr[5];    // �ő�ł�5��
    int                 portlen = 0;
    
    DEBUG_PRINTF("[%s] CONNECT port test %d\n",__func__, req_p->port);
    
    portlen = NHTTPi_intToStr(portstr, (u32)(req_p->port));

    // CONNECT���\�b�h�𑗐M
    sendStatus = NHTTPi_SendData(threadData_p, "CONNECT ", 8);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, &req_p->URL_p[8], req_p->hostLen - 8 );
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, ":", 1);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, portstr, portlen);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, " HTTP/" NHTTP_STR_HTTPVER "\r\n", 11 );
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "Host: ", 6);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, &req_p->URL_p[8], req_p->hostLen - 8);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, ":", 1);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, portstr, portlen );
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "Content-Length: 0\r\nPragma: no-cache\r\n", 37);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    
    // �F�ؗp�̃��[�U���ƃp�X���[�h�𑗐M
    sendStatus = NHTTPi_SendProxyAuthorization(threadData_p);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    
    // �w�b�_�I�[
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (threadData_p->sendBufwp > 0)
    {
        int stat = NHTTPi_SocSend(req_p, bgnEndInfo_p->socket, commBuf_p, threadData_p->sendBufwp, 0);
        if (stat < 0)
        {
            return NHTTP_SEND_ERROR_e;
        }
        if (stat == 0)
        {
            return NHTTP_SEND_NOTCONN_e;
        }
    }
    
    threadData_p->sendBufwp = 0;
    NHTTPi_memclr(commBuf_p, LEN_COMMBUF);
    
    return NHTTP_SEND_SUCCESS_e;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_RecvProxyConnectHeader
  Description:  PROXY CONNECT�w�b�_�[��M
  Arguments:    threadData_p
  Returns:      TRUE  ����
                FALSE ���s
 *---------------------------------------------------------------------------*/
static int  NHTTPi_RecvProxyConnectHeader(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    char*		commBuf_p = (char*)&(threadInfo_p->commBuf[0]);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPRes*           res_p = req_p->res_p;

    char                tmpbuf[TMP_PROXY_HEADER_BUF_SIZE];
    int                 i = 0;
    int                 recvbyte = 0;
    int                 endFlag = FALSE;
    int                 connectFlag = FALSE;
    int                 totalRecvbyte = 0;

    (void)threadData_p;
    
    DEBUG_PRINTF("[%s]\n",__func__);

    while (1)
    {
        // ��M�ł��邾�����Ă��܂�
        recvbyte = NHTTPi_SocRecv(mutexInfo_p, req_p, bgnEndInfo_p->socket, &(tmpbuf[totalRecvbyte]), (TMP_PROXY_HEADER_BUF_SIZE - totalRecvbyte), 0);
        totalRecvbyte += recvbyte;
        // �ꉞ���g�𑍃`�F�b�N����
        // HTTP/1.0 or HTTP/1.1 200
        res_p->httpStatus = NHTTPi_strToInt(&tmpbuf[9], 3);
        if ((NHTTPi_strnicmp(tmpbuf, "HTTP/", 5) == 0) &&
            (tmpbuf[8] == ' ') &&
            (res_p->httpStatus == 200))
        {
            connectFlag = TRUE;
        }
        
        // ���s�R�[�h���A���Q��܂܂�Ă��邩����
        for (i = 0, endFlag = FALSE; i < totalRecvbyte; i++)
        {
            if ((i > 1) &&
                (tmpbuf[i - 1] == '\r') &&
                (tmpbuf[i] == '\r'))
            {
                DEBUG_PRINTF("[%s]:CR %d\n", __func__,i);
                endFlag = TRUE;
            }
            else if ((i > 1) &&
                     (tmpbuf[i - 1] == '\n') &&
                     (tmpbuf[i] == '\n'))
            {
                DEBUG_PRINTF("[%s]:LF %d\n", __func__,i);
                endFlag = TRUE;
            } else if ((i > 3) &&
                       (tmpbuf[i - 3] == '\r') &&
                       (tmpbuf[i - 2] == '\n') &&
                       (tmpbuf[i - 1] == '\r') &&
                       (tmpbuf[i] == '\n'))
            {
                DEBUG_PRINTF("[%s]:CRLF %d\n", __func__,i);
                endFlag = TRUE;
            }
        }
        
        // �I���`�F�b�N
        if (endFlag)
        {
            if (connectFlag)
            {
                break;
            }
            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
            return FALSE;
        }

        // error
        // ��M�ł��Ȃ�����
        if (recvbyte < 0)
        {
            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
            return FALSE;
        }

        // �o�b�t�@�����ς�
        if (totalRecvbyte >= TMP_PROXY_HEADER_BUF_SIZE)
        {
            recvbyte = NHTTPi_SocRecv(mutexInfo_p, req_p, bgnEndInfo_p->socket, commBuf_p, 1, 0);
            // ��M���s
            if (recvbyte < 0)
            {
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return FALSE;
            }
            // �܂���M PROXY�ڑ��̕ԓ��w�b�_������Ȃł������P���Ȃ��̂ŃG���[
            else if (recvbyte != 0)
            {
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return FALSE;
            }
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SendHeaderList
  Description:  �t���w�b�_���M
  Arguments:    threadData_p
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int NHTTPi_SendHeaderList(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPi_DATALIST*    datalist_p;
    int                 sendStatus;
    
    DEBUG_PRINTF("[%s][%d]\n", __func__, threadData_p->id);
    // �t���w�b�_
    for (datalist_p = NHTTPi_getHdrFromList(&req_p->listHeader_p); datalist_p;  datalist_p = NHTTPi_getHdrFromList(&req_p->listHeader_p))
    {
        sendStatus = NHTTPi_SendData(threadData_p, datalist_p->label_p, NHTTPi_strlen(datalist_p->label_p));
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        sendStatus = NHTTPi_SendData(threadData_p,  ": ", 2);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        sendStatus = NHTTPi_SendData(threadData_p, datalist_p->value_p, NHTTPi_strlen(datalist_p->value_p));
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        
        NHTTPi_free(datalist_p);
    }
    
    return NHTTP_SEND_SUCCESS_e;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SendProcPostDataRaw
  Description:  �R�[���o�b�N�ɂ�鐶�f�[�^���M
  Arguments:    threadData_p
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int NHTTPi_SendProcPostDataRaw(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    char*		commBuf_p = (char*)&(threadInfo_p->commBuf[0]);
    u32                 contentLength = 0;
    int                 contentLengthBufLen = 0;
    char                contentLengthBuf[TMP_CONTENT_LENGTH_BUF_SIZE];
    int                 sendStatus;
    
    // �T�C�Y�𒲍�
    if (!req_p->rawPostData_p)
    {
        if (!NHTTPi_GetPostContentlength(mutexInfo_p, req_p, NULL, &contentLength, SEND_TYPE_RAW_e))
        {
            return NHTTP_SEND_ERROR_POST_CALLBACK_e;
        }
    }
    else
    {
        contentLength = req_p->rawPostDataLen;
    }
    contentLengthBufLen = NHTTPi_intToStr(contentLengthBuf, contentLength);
    
    
    sendStatus = NHTTPi_SendData(threadData_p, "Content-Length: ", 16);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, contentLengthBuf, contentLengthBufLen);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    
    // ���f�[�^���M
    if (!req_p->rawPostData_p)
    {
        int result = NHTTPi_SendPostData(mutexInfo_p, req_p, commBuf_p, NULL, bgnEndInfo_p->socket, &threadData_p->sendBufwp, SEND_TYPE_RAW_e);
        if (result != NHTTP_SEND_SUCCESS_e)
        {
            return result;
        }
    }
    else
    {
        sendStatus = NHTTPi_SendData(threadData_p, req_p->rawPostData_p, (int)req_p->rawPostDataLen);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    }
    
    return NHTTP_SEND_SUCCESS_e;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SendProcPostDataBinary
  Description:  �R�[���o�b�N�ɂ��o�C�i���f�[�^���M
  Arguments:    threadData_p
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int NHTTPi_SendProcPostDataBinary(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    char*		commBuf_p = (char*)&(threadInfo_p->commBuf[0]);
    u32                 contentLength = 0;
    int                 contentLengthBufLen = 0;
    char                contentLengthBuf[TMP_CONTENT_LENGTH_BUF_SIZE];
    int                 sendStatus;
    NHTTPi_DATALIST*    datalist_p;
   
    // �o�C�i���f�[�^����@���@multipart/form-data
    for (datalist_p = req_p->listPost_p; datalist_p; datalist_p = datalist_p->next_p)
    {
        contentLength += sizeof(req_p->tagPost) + 2;	// [bound]CRLF
        contentLength += (sizeof(STR_POST_DISPOS) - 1) + NHTTPi_strlen(datalist_p->label_p) + 1 + 2;	// [label]'"'CRLF
        if (datalist_p->isBinary)
        {
            contentLength += (sizeof(STR_POST_TYPE_BIN) - 1);
        }
        contentLength += 2;	// CRLF
        
        if (!datalist_p->value_p)
        {
            if (!NHTTPi_GetPostContentlength(mutexInfo_p, req_p, datalist_p->label_p, &contentLength, SEND_TYPE_MULTIPART_e))
            {
                return NHTTP_SEND_ERROR_POST_CALLBACK_e;
            }
        }
        else
        {
            contentLength += datalist_p->length;
        }
        
        contentLength += 2;	// CRLF
        if (datalist_p == req_p->listPost_p->prev_p)
        {
            break;
        }
    }
    contentLength += sizeof(req_p->tagPost) + 2 + 2;	// [bound]"--"CRLF
    contentLengthBufLen = NHTTPi_intToStr(contentLengthBuf, contentLength);
                                
    sendStatus = NHTTPi_SendData(threadData_p, STR_POST_TYPE_MULTIPART, (sizeof(STR_POST_TYPE_MULTIPART) - 1));
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, &req_p->tagPost[2], sizeof(req_p->tagPost) - 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 

    sendStatus = NHTTPi_SendData(threadData_p, "Content-Length: ", 16);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, contentLengthBuf, contentLengthBufLen);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 

    //==========================================================================
    // �f�[�^�{�̂𑗐M
    for (datalist_p = req_p->listPost_p; datalist_p; datalist_p = datalist_p->next_p)
    {
        sendStatus = NHTTPi_SendData(threadData_p, req_p->tagPost, sizeof(req_p->tagPost));
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        sendStatus = NHTTPi_SendData(threadData_p, STR_POST_DISPOS, (sizeof(STR_POST_DISPOS) - 1));
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        sendStatus = NHTTPi_SendData(threadData_p, datalist_p->label_p, NHTTPi_strlen(datalist_p->label_p));
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        sendStatus = NHTTPi_SendData(threadData_p, "\"" "\r\n", 3);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 

        if (datalist_p->isBinary)
        {
            sendStatus = NHTTPi_SendData(threadData_p, STR_POST_TYPE_BIN, (sizeof(STR_POST_TYPE_BIN) - 1));
            if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        }
        sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        
        if (!datalist_p->value_p)
        {
            int result = NHTTPi_SendPostData(mutexInfo_p, req_p, commBuf_p, datalist_p->label_p, bgnEndInfo_p->socket, &threadData_p->sendBufwp, SEND_TYPE_MULTIPART_e);
            if (result != NHTTP_SEND_SUCCESS_e)
            {
                return result;
            }
        }
        else
        {
            sendStatus = NHTTPi_SendData(threadData_p, datalist_p->value_p, (int)datalist_p->length);
            if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        }
        
        sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        
        if (datalist_p == req_p->listPost_p->prev_p)
            break;
    }

    sendStatus = NHTTPi_SendData(threadData_p, req_p->tagPost, sizeof(req_p->tagPost));
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "--" "\r\n", 4);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 

    return NHTTP_SEND_SUCCESS_e;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_SendProcPostDataAscii
  Description:  �R�[���o�b�N�ɂ��A�X�L�[�f�[�^���M
  Arguments:    threadData_p
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int NHTTPi_SendProcPostDataAscii(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    char*		commBuf_p = (char*)&(threadInfo_p->commBuf[0]);
    
    u32                 contentLength = 0;
    int                 tmpBufLen = 0;
    char                tmpBuf[TMP_CONTENT_LENGTH_BUF_SIZE];
    int                 sendStatus;
    NHTTPi_DATALIST*    datalist_p;

    // �o�C�i���f�[�^�Ȃ��@���@application/x-www-form-urlencoded
    // ex: label1=test+data%0D%0A&label2=testdata2
    for (datalist_p = req_p->listPost_p; datalist_p; datalist_p = datalist_p->next_p)
    {
        contentLength += NHTTPi_getUrlEncodedSize(datalist_p->label_p);
        contentLength += 1;	// '='

        if (!datalist_p->value_p)
        {
            if (!NHTTPi_GetPostContentlength(mutexInfo_p, req_p, datalist_p->label_p, &contentLength, SEND_TYPE_URLENCODE_e))
            {
                return NHTTP_SEND_ERROR_POST_CALLBACK_e;
            }
        }
        else
        {
            contentLength += NHTTPi_getUrlEncodedSize(datalist_p->value_p);
        }
        if (datalist_p == req_p->listPost_p->prev_p)
            break;
        contentLength += 1;	// '&'
    }

    tmpBufLen = NHTTPi_intToStr(tmpBuf, contentLength);
    
    sendStatus = NHTTPi_SendData(threadData_p, STR_POST_TYPE_URLENCODE, (sizeof(STR_POST_TYPE_URLENCODE) - 1));
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "Content-Length: ", 16);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, tmpBuf, tmpBufLen);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    sendStatus = NHTTPi_SendData(threadData_p, "\r\n", 2);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    
    //==========================================================================
    // �f�[�^�{�̂𑗐M
    {
        int	p;
        for (datalist_p = req_p->listPost_p; datalist_p; datalist_p = datalist_p->next_p)
        {
            // label
            for (p=0; datalist_p->label_p[p]; p++)
            {
                tmpBufLen = NHTTPi_encodeUrlChar(tmpBuf, datalist_p->label_p[p]);
                sendStatus = NHTTPi_SendData(threadData_p, tmpBuf, tmpBufLen);
                if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
            }
            sendStatus = NHTTPi_SendData(threadData_p, "=", 1);
            if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
            
            if (!datalist_p->value_p)
            {
                int result = NHTTPi_SendPostData(mutexInfo_p, req_p, commBuf_p, datalist_p->label_p, bgnEndInfo_p->socket, &threadData_p->sendBufwp, SEND_TYPE_URLENCODE_e);
                if (result != NHTTP_SEND_SUCCESS_e)
                {
                    return result;
                }
            }
            else
            {
                for (p=0; datalist_p->value_p[p]; p++)
                {
                    tmpBufLen = NHTTPi_encodeUrlChar(tmpBuf, datalist_p->value_p[p]);
                    sendStatus = NHTTPi_SendData(threadData_p, tmpBuf, tmpBufLen);
                    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
                }
            }
            if (datalist_p == req_p->listPost_p->prev_p)
            {
                break;
            }
            sendStatus = NHTTPi_SendData(threadData_p, "&", 1);
            if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        }
    }
    return NHTTP_SEND_SUCCESS_e;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadInit
  Description:  HTTP�ʐM�X���b�h�@�g�p�ϐ�������
  Arguments:    threadData_p
 *---------------------------------------------------------------------------*/
static void NHTTPi_ThreadInit(NHTTPThreadData* threadData_p)
{
    DEBUG_PRINTF("[%s]\n", __func__);
    
    threadData_p->id = -1;
    NHTTPi_memclr(threadData_p->preHostname, LEN_HOST_NAME);
    NHTTPi_memclr(threadData_p->tmpBodyBuf, TMP_BODY_BUF_SIZE);
    threadData_p->hostaddr = 0xffffffff;
    threadData_p->preHostaddr = 0xffffffff;
    threadData_p->sendBufwp = 0;    
    threadData_p->isKeepAlive = FALSE;
    threadData_p->isChunked = FALSE;    
    threadData_p->restartConnect = FALSE;
    threadData_p->contentLength = 0;
    threadData_p->result = NHTTP_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadReqEnd
  Description:  HTTP�ʐM�X���b�h�@�I������
  Arguments:    threadData_p 
 *---------------------------------------------------------------------------*/
static void NHTTPi_ThreadReqEnd(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPRes*           res_p = req_p->res_p;
    NHTTPConnection*    connection_p = NHTTPi_Request2Connection(mutexInfo_p, req_p);
    
    DEBUG_PRINTF("[%s][%d] finalizing communication\n", __func__, threadData_p->id);
    
    // �L�����Z���`�F�b�N
    if (req_p->doCancel)
    {
        threadData_p->result = NHTTP_ERROR_CANCELED;
        // > Keep-Alive�����ɂȂ��Ă���\�P�b�g(isKeepAlive == TRUE)��
        // > NHTTPi_CleanupAsync()�֐����Ăяo���Ă��N���[�Y����Ȃ��B
        // �o�O���C��
        threadData_p->isKeepAlive = FALSE;
        DEBUG_PRINTF("[%s][%d] request was canceled\n", __func__, threadData_p->id );
        DEBUG_PRINTF("[%s][%d] isKeepAlive = %d \n", __func__,  __LINE__, threadData_p->isKeepAlive);
    }

    if (!threadData_p->isKeepAlive && IS_SOCKET(bgnEndInfo_p->socket))
    {
        DEBUG_PRINTF("NHTTPi_SocClose [%d]\n", __LINE__);
        if (NHTTPi_SocClose( mutexInfo_p, req_p, bgnEndInfo_p->socket ) < 0)
        {
            threadData_p->result = NHTTP_ERROR_NETWORK;
        }
        DEBUG_PRINTF("[%s][%d] INVALID_SOCKET (keep %d)\n", __func__,  __LINE__, threadData_p->isKeepAlive);
        bgnEndInfo_p->socket = INVALID_SOCKET;
    }
    
    if (threadData_p->result == NHTTP_ERROR_NONE)
    {
        res_p->isSuccess = TRUE;
    }
    else
    {
        res_p->isSuccess = FALSE;
        // �G���[�����Z�b�g
        NHTTPi_SetError(bgnEndInfo_p, threadData_p->result);
        DEBUG_PRINTF("[%s][%d] API error = %d\n",
                        __func__, threadData_p->id, threadData_p->result );
        
        if (res_p->recvBuf_p == &(threadData_p->tmpBodyBuf[0]))
        {
            res_p->recvBuf_p = NULL;
            res_p->recvBufLen = 0;
        }
    }
    
    // NHTTPGetConnectionError�p
    if (connection_p != NULL)
    {
        connection_p->nhttpError = threadData_p->result;
    }
            
    // ���ׂĂ̏I���������������Ă��烊�N�G�X�g�폜
    NHTTPi_lockReqList(mutexInfo_p);
    {
        NHTTPi_free((void*)reqInfo_p->reqCurrent);
        reqInfo_p->reqCurrent = NULL;
    }
    NHTTPi_unlockReqList(mutexInfo_p);
    
    NHTTPi_destroyRequestObject(mutexInfo_p, req_p);
                
    if ((connection_p != NULL) && (res_p->isSuccess != FALSE))
    {
        connection_p->status = NHTTP_STATUS_COMPLETE;
    }
    
    NHTTPi_CompleteCallback(mutexInfo_p, connection_p);
    
    DEBUG_PRINTF("[%s][%d] complete threadData_p->result %d \n\n",
                    __func__, threadData_p->id, threadData_p->result);
    
    if (connection_p != NULL)
    {
        NHTTPi_NotifyCompletion(connection_p);
    }
    
    NHTTPi_DeleteConnection(connection_p);
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadExecReqQueue
  Description:  �L���[���X�g�̐擪������Ă��āA���s���ɂ���
                �L���[���X�g���󂾂�����Akick�����܂ő҂�
  Returns:      TRUE �L���[���X�g�̐擪���擾�ł��A���s����
                FALSE �L���[���X�g�����kick�����܂ő҂�
 *---------------------------------------------------------------------------*/
static BOOL NHTTPi_ThreadExecReqQueue(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);

    DEBUG_PRINTF("[%s]\n", __func__);
        
    NHTTPi_lockReqList(mutexInfo_p);
    {
        // �L���[���X�g�̐擪������Ă��āA���s���ɂ���
        NHTTPListInfo*	listInfo_p = NHTTPi_GetListInfoP(sysInfo_p);
        NHTTPi_REQLIST*	list_p = NHTTPi_getReqFromReqQueue(listInfo_p);

        if (list_p)
        {
            NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
            
            threadData_p->id = list_p->id;
            reqInfo_p->reqCurrent = list_p;
        }
        else
        {
            threadData_p->id = -1;
        }
    }
    NHTTPi_unlockReqList(mutexInfo_p);
    
    if (threadData_p->id < 0)
    {
        NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
        // �L���[���X�g���󂾂�����Akick�����܂ő҂�
        NHTTPi_idleCommThread(threadInfo_p);
        return FALSE;
    }
    
    return TRUE;
}
/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadHostAddrProc
  Description:  �z�X�g�A�h���X�ݒ�
                �O��̃z�X�g�A�h���X�Ɠ���̏ꍇ�͑O��̃z�X�g�A�h���X��ݒ�
  Arguments:    threadData_p
  Returns:      TRUE  ����
                FALSE ���s
 *---------------------------------------------------------------------------*/
static BOOL NHTTPi_ThreadHostAddrProc(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    
    // �ڑ���z�X�g��
    char* hostname_p = req_p->hostname_p;
    
    // PROXY���ݒ肳��Ă���ꍇ��PROXY�T�[�o�[���ڑ���ƂȂ�
    if (req_p->isProxy)
    {
        hostname_p = req_p->proxyAddr;
    }
    
    // RVL��SO�ł͖��O�����ɃL���b�V���������Ȃ��̂�
    // Keep-Alive�w�莞�̓z�X�g�����O��̂��̂ƈ�v�����ꍇ�Ƀz�X�g�����������Ȃ�
    if ((NHTTPi_strlen(hostname_p) == 0) ||
        (NHTTPi_strcmp(hostname_p, threadData_p->preHostname) != 0))
    {
        DEBUG_PRINTF("[%s][%d] �O��̃z�X�g���ƕs��v\n", __func__, threadData_p->id);
        // �z�X�g��������
        threadData_p->hostaddr = NHTTPi_resolveHostname(req_p, hostname_p);
        if (!threadData_p->hostaddr)
        {
            if (req_p->isProxy)
            {
                threadData_p->result = NHTTP_ERROR_DNS_PROXY;
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return FALSE;
            }
            else
            {
                threadData_p->result = NHTTP_ERROR_DNS;
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return FALSE;
            }
        }
    }
    else
    {
        // �z�X�g�����O��̂��̂ƈ�v�����̂ŁA�z�X�g�A�h���X�����l
        DEBUG_PRINTF("[%s][%d] �z�X�g�����O��̂��̂ƈ�v�����̂ŁA�z�X�g�A�h���X�����l\n", __func__, threadData_p->id);
        threadData_p->hostaddr = threadData_p->preHostaddr;
    }
    
    NHTTPi_memclr(threadData_p->preHostname, LEN_HOST_NAME);
    NHTTPi_memcpy(threadData_p->preHostname, hostname_p, NHTTPi_strnlen(hostname_p, LEN_HOST_NAME - 1));

    // PROXY���ݒ肳��Ă���ꍇ�̓|�[�g�ԍ���PROXY�̐ݒ���g�p����B
    threadData_p->port = req_p->port;
    if (req_p->isProxy)
    {
        threadData_p->port = req_p->proxyPort;
    }
 
    // SSL�Ȃ疈��ڑ��������B
    // (�O��̗v���ɂ�����CA������̂��̂Ɠ��ꂩ�ǂ����A���C�u�������ŕۏ؂ł��Ȃ�����)
    // �ڑ���A�h���X�E�|�[�g�ԍ����قȂ�ꍇ���ڑ��������B
    if ((threadData_p->hostaddr != threadData_p->preHostaddr) ||
        (threadData_p->port != threadData_p->prePort) ||
        (req_p->isSSL == TRUE))
    {
        threadData_p->isKeepAlive = FALSE;
        DEBUG_PRINTF("[%s][%d] isKeepAlive = %d \n", __func__,  __LINE__, threadData_p->isKeepAlive);        
    }

    // �z�X�g�A�h���X���o���Ă���
    threadData_p->preHostaddr = threadData_p->hostaddr;
    // �|�[�g�ԍ����o���Ă���
    threadData_p->prePort = threadData_p->port;
    
    return TRUE;
}
/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadConnectProc
  Description:  �z�X�g�ɐڑ�
                KeepAlive���͐ڑ��ς݂Ȃ��߁A�قڏ�������
  Arguments:    threadData_p
  Returns:      TRUE  ����
                FALSE ���s
 *---------------------------------------------------------------------------*/
static BOOL NHTTPi_ThreadConnectProc(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);

    // �{���ɐڑ����p������Ă��邩���ׂ�
    // �p������Ă��Ȃ��̂ł����isKeepAlive�͖����ɂ���
    // �Đڑ����Ȃ���΂Ȃ�Ȃ�
    if (threadData_p->isKeepAlive == TRUE) {
        if (NHTTPi_checkKeepAlive(bgnEndInfo_p->socket) == -1) {
            threadData_p->isKeepAlive = FALSE;
        }
    }
        
    if (threadData_p->isKeepAlive == FALSE)
    {
        // �\�P�b�g���L���Ȃ�ΏI��
        if (IS_SOCKET(bgnEndInfo_p->socket))
        {
            DEBUG_PRINTF("NHTTPi_SocClose [%d]\n", __LINE__);
            if (NHTTPi_SocClose(mutexInfo_p, req_p, bgnEndInfo_p->socket) < 0)
            {
                // �I�����s
                DEBUG_PRINTF("[%s][%d] INVALID_SOCKET \n", __func__, __LINE__);
                bgnEndInfo_p->socket = INVALID_SOCKET;
                threadData_p->result = NHTTP_ERROR_NETWORK;
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return FALSE;
            }
        }

        // �\�P�b�g���J��
        bgnEndInfo_p->socket = NHTTPi_SocOpen(req_p);
            
        if (!IS_SOCKET(bgnEndInfo_p->socket))
        {
            threadData_p->result = NHTTP_ERROR_SOCKET;
            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
            return FALSE;
        }
        
        NHTTPi_lockReqList(mutexInfo_p);
        {
            reqInfo_p->reqCurrent->socket_p = (void*)bgnEndInfo_p->socket;
        }
        NHTTPi_unlockReqList(mutexInfo_p);

        // �L�����Z���`�F�b�N
        if (req_p->doCancel)
        {
            DEBUG_PRINTF("[%s][%d] �L�����Z������������܂���\n", __func__, __LINE__);
            return FALSE;
        }
                   
        DEBUG_PRINTF("[%s][%d] Open TCP socket %d hostaddr %lx port %d\n",
                        __func__, threadData_p->id, bgnEndInfo_p->socket, threadData_p->hostaddr, threadData_p->port);
        if (NHTTPi_SocConnect(bgnEndInfo_p, mutexInfo_p, req_p, bgnEndInfo_p->socket, threadData_p->hostaddr, threadData_p->port) < 0)
        {
            // �ڑ����s
            if (req_p->isProxy)
            {
                threadData_p->result = NHTTP_ERROR_CONNECT_PROXY;
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return FALSE;
            }
            else if (NHTTPi_GetSSLError(bgnEndInfo_p) != NHTTP_SSL_ENONE)
            {
                threadData_p->result = NHTTP_ERROR_SSL;
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return FALSE;
            }
            else
            {
                threadData_p->result = NHTTP_ERROR_CONNECT;
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return FALSE;
            }
        }
            
        DEBUG_PRINTF("[%s][%d] Connect TCP socket %d hostaddr %lx port %d\n",
                        __func__, threadData_p->id, bgnEndInfo_p->socket, threadData_p->hostaddr, threadData_p->port);
    }
    else
    {
        NHTTPi_lockReqList(mutexInfo_p);
        {
            DEBUG_PRINTF("[%s][%d] KeepAlive TCP socket %d hostaddr %lx port %d\n",
                            __func__, threadData_p->id, bgnEndInfo_p->socket, threadData_p->hostaddr, threadData_p->port);
            reqInfo_p->reqCurrent->socket_p = (void*)bgnEndInfo_p->socket;
        }
        NHTTPi_unlockReqList(mutexInfo_p);
    }
    
    return TRUE;
}
         
/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadProxyProc
  Description:  �v���L�V�T�[�o�[�ڑ�
  Arguments:    threadData_p
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int NHTTPi_ThreadProxyProc(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;

    int                 sendStatus = 0;
    
    threadData_p->result = NHTTP_ERROR_NETWORK;
            
    // ���M�o�b�t�@�ʒu��������
    threadData_p->sendBufwp = 0;
#if defined(NHTTP_USE_NSSL) || defined(NHTTP_USE_CPSSSL)
    // PROXY�o�R��SSL�ڑ��͍ŏ���CONNECT���\�b�h�𑗂�K�v������
    if (req_p->isSSL && req_p->isProxy)
    {
        int SSLConnectResult = NHTTPi_NB_SUCCESS;
            
        DEBUG_PRINTF("[%s][%d] \n", __func__, threadData_p->id);
        /////////////////////////////////////////////////////
        //
        // PROXY�o�R�̂��߂�CONNECT���\�b�h�𑗐M
        //
        sendStatus = NHTTPi_SendProxyConnectMethod(threadData_p);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        
        /////////////////////////////////////////////////////
        //
        //  PROXY�T�[�o�[�̕ԓ��҂�
        //
        if (NHTTPi_RecvProxyConnectHeader(threadData_p) == FALSE)
        {
            return NHTTP_SEND_ERROR_e;
        }
        
        // SSL�ڑ��J�n
        DEBUG_PRINTF("[%s][%d] SSL connection start\n", __func__, threadData_p->id);
        SSLConnectResult = NHTTPi_SocSSLConnect(bgnEndInfo_p, mutexInfo_p, req_p, bgnEndInfo_p->socket);
        // �ڑ�����
        if (SSLConnectResult == NHTTPi_NB_SUCCESS)
        {
            DEBUG_PRINTF("[%s][%d] SSL connection OK\n", __func__, threadData_p->id);
        }
        // ROOTCA�̐ݒ�Ɏ��s����
        else if (SSLConnectResult == NHTTPi_NB_ERROR_SSL_ROOTCA)
        {
            if (NHTTPi_GetSSLError(bgnEndInfo_p) != NHTTP_SSL_ENONE)
            {
                threadData_p->result = NHTTP_ERROR_SSL_ROOTCA;
            }
            return NHTTP_SEND_ERROR_e;
        }
        // CLIENTCERT�̐ݒ�Ɏ��s����
        else if (SSLConnectResult == NHTTPi_NB_ERROR_SSL_CLIENTCERT)
        {
            if (NHTTPi_GetSSLError(bgnEndInfo_p) != NHTTP_SSL_ENONE)
            {
                threadData_p->result = NHTTP_ERROR_SSL_CLIENTCERT;
            }
            return NHTTP_SEND_ERROR_e;
        }
        // ���̑��̃G���[
        else
        {
            DEBUG_PRINTF("[%s][%d] SSL connection NG\n", __func__, threadData_p->id);
            if (NHTTPi_GetSSLError(bgnEndInfo_p) != NHTTP_SSL_ENONE)
            {
                threadData_p->result = NHTTP_ERROR_SSL;
            }
            return NHTTP_SEND_ERROR_e;
        }
    }
#else
    (void)sendStatus;
#endif
    return NHTTP_SEND_SUCCESS_e;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadSendProc
  Description:  HTTP�w�b�_�[���M
  Arguments:    threadData_p
  Returns:      NHTTP_SEND_SUCCESS_e    ����
                NHTTP_SEND_NOTCONN_e    �܂��ڑ��ł��Ă��Ȃ�
                NHTTP_SEND_ERROR_e      �G���[
 *---------------------------------------------------------------------------*/
static int NHTTPi_ThreadSendProc( NHTTPThreadData* threadData_p )
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_Request2Connection(mutexInfo_p, req_p);
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    char*		commBuf_p = (char*)&(threadInfo_p->commBuf[0]);
    int                 urlLength = (int)NHTTPi_strlen( req_p->URL_p );
    int                 sendStatus = NHTTP_SEND_SUCCESS_e;
    BOOL                isMultipartForm = FALSE;
    NHTTPi_DATALIST*    datalist_p;
    
    DEBUG_PRINTF("[%s][%d]\n", __func__, threadData_p->id);
    
    threadData_p->result = NHTTP_ERROR_NETWORK;
    
    if (connection_p != NULL)
    {
        connection_p->status = NHTTP_STATUS_SENDING;
    }
    
    // ���M�o�b�t�@�ʒu��������
    threadData_p->sendBufwp = 0;
    
    // HTTP���\�b�h���M
    switch (req_p->method)
    {
    case NHTTP_REQMETHOD_GET:
        sendStatus = NHTTPi_SendData(threadData_p, "GET ", 4);
        break;
    case NHTTP_REQMETHOD_POST:
        sendStatus = NHTTPi_SendData(threadData_p, "POST ", 5);
        break;
    case NHTTP_REQMETHOD_HEAD:
        sendStatus = NHTTPi_SendData(threadData_p, "HEAD ", 5);
        break;
    default:
        break;
    }
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 

    // PROXY�o�R�̏ꍇ��SSL�ł͂Ȃ��ꍇ��URL���ׂ�SEND����K�v������
    if (req_p->isProxy && !req_p->isSSL)
    {
        sendStatus = NHTTPi_SendData(threadData_p,  req_p->URL_p, urlLength );
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    }
    else
    {
        if (urlLength > req_p->hostDescLen)
        {
            sendStatus = NHTTPi_SendData(threadData_p, &req_p->URL_p[req_p->hostDescLen], urlLength - req_p->hostDescLen);
            if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        }
        else
        {
            sendStatus = NHTTPi_SendData(threadData_p, "/", 1);
            if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        }
    }
    sendStatus = NHTTPi_SendData(threadData_p, " HTTP/" NHTTP_STR_HTTPVER "\r\n", 11);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    
    // HOST�s
    {
        int n = req_p->isSSL? 8 : 7;

        sendStatus = NHTTPi_SendData(threadData_p, "Host: ", 6);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        sendStatus = NHTTPi_SendData(threadData_p, &req_p->URL_p[n], req_p->hostLen - n);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
        sendStatus = NHTTPi_SendData(threadData_p,  "\r\n", 2);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    }
    
    // SSL�͂��ł�PROXY Basic�F�؂𑗐M�ς݂Ȃ̂ŁASSL�ʐM�ȊO�͂����ő��M����
    if (req_p->isProxy && !req_p->isSSL)
    {
        // �F�ؗp�̃��[�U���ƃp�X���[�h�𑗐M
        sendStatus = NHTTPi_SendProxyAuthorization(threadData_p);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    }
            
    // Basic�F��
    sendStatus = NHTTPi_SendBasicAuthorization(threadData_p);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
                        
    // �t���w�b�_
    sendStatus = NHTTPi_SendHeaderList(threadData_p);
    if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 

    // POST�p�w�b�_
    if (req_p->method == NHTTP_REQMETHOD_POST)
    {
        int postResult = NHTTP_SEND_SUCCESS_e;
        if (req_p->isRawData)
        {
            DEBUG_PRINTF("[%s][%d] NHTTP_ENCODING_TYPE_RAW\n", __func__, threadData_p->id);
            postResult = NHTTPi_SendProcPostDataRaw(threadData_p);
        }
        else
        {
            NHTTPEncodingType a_encodingType = req_p->encodingType;
            // ��������
            if (a_encodingType == NHTTP_ENCODING_TYPE_AUTO)
            {
                DEBUG_PRINTF("[%s][%d] NHTTP_ENCODING_TYPE_AUTO\n", __func__, threadData_p->id);
                // �o�C�i���f�[�^���܂܂�邩�H
                for (isMultipartForm = FALSE, datalist_p = req_p->listPost_p; datalist_p; datalist_p = datalist_p->next_p)
                {
                    if (datalist_p->isBinary)
                    {
                        isMultipartForm = TRUE;
                        break;
                    }
                    if (datalist_p == req_p->listPost_p->prev_p)
                    {
                        break;
                    }
                }
            }
            // multipart/form-data
            else if (a_encodingType == NHTTP_ENCODING_TYPE_MULTIPART)
            {
                DEBUG_PRINTF("[%s][%d] NHTTP_ENCODING_TYPE_MULTIPART\n", __func__, threadData_p->id);
                isMultipartForm = TRUE;
            }
            // application/x-www-form-urlencoded
            else
            {
                DEBUG_PRINTF("[%s][%d] NHTTP_ENCODING_TYPE_URL\n", __func__, threadData_p->id);
                isMultipartForm = FALSE;
            }

            if (isMultipartForm)
            {
                postResult = NHTTPi_SendProcPostDataBinary(threadData_p);
            }
            else
            {
                postResult = NHTTPi_SendProcPostDataAscii(threadData_p);
            }
        }

        // ���M�Ɏ��s
        if (postResult != NHTTP_SEND_SUCCESS_e)
        {
            if (postResult == NHTTP_SEND_ERROR_POST_CALLBACK_e)
            {
                // �X�g���[�~���O���M��POST�f�[�^���M�̃R�[���o�b�N�����̒l��Ԃ����ꍇ�̃G���[
                // �������ɂ��킹��NHTTP_ERROR_SOCKET��ݒ�
                threadData_p->result = NHTTP_ERROR_SOCKET;
            }
            return postResult;
        }
    }
    else
    {
        sendStatus = NHTTPi_SendData(threadData_p,  "\r\n", 2);
        if (sendStatus != NHTTP_SEND_SUCCESS_e) return sendStatus; 
    }

    sendStatus = NHTTP_SEND_SUCCESS_e;
    // �c�肪����Α��M
    if (threadData_p->sendBufwp > 0)
    {
        int	stat = NHTTPi_SocSend(req_p, bgnEndInfo_p->socket, commBuf_p, threadData_p->sendBufwp, 0);

        // ���M�o�b�t�@�ʒu��������
        threadData_p->sendBufwp = 0;
        NHTTPi_memclr(commBuf_p, LEN_COMMBUF);
        
        if (stat < 0)
        {
            sendStatus =  NHTTP_SEND_ERROR_e;
        }
        if (stat == 0)
        {
            sendStatus = NHTTP_SEND_NOTCONN_e;
        }
    }
    
    // ���M�o�b�t�@�ʒu��������
    threadData_p->sendBufwp = 0;
    NHTTPi_memclr(commBuf_p, LEN_COMMBUF);
    
    return sendStatus;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadRecvHeaderProc
  Description:  HTTP�w�b�_�[��M
  Arguments:    threadData_p
  Returns:      TRUE  ����
                FALSE ���s                
 *---------------------------------------------------------------------------*/
static int NHTTPi_ThreadRecvHeaderProc(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPRes*           res_p = req_p->res_p;
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_Request2Connection(mutexInfo_p, req_p);
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    
    char                tmpBuf[4] = {0,};
    int                 recvbyte = 0;
    NHTTPi_HDRBUFLIST*  headerBuf_p = NULL;

    
    if (connection_p != NULL)
    {
        connection_p->status = NHTTP_STATUS_RECEIVING_HEADER;
    }

    res_p->headerLen = 0;
    NHTTPi_memclr(threadData_p->tmpHeaderBuf, TMP_HEADER_BUF_SIZE);
    
    
    // ��d���s�܂�recv����
    for (headerBuf_p = res_p->hdrBufBlock_p, threadData_p->recvBufwp = 0; ; )
    {
        // �L�����Z���`�F�b�N
        if (req_p->doCancel)
        {
            DEBUG_PRINTF("[%s][%d] �L�����Z������������܂���\n", __func__, __LINE__);
            return FALSE;
        }
                
        if (threadData_p->recvBufwp < NHTTP_HDRRECVBUF_INILEN)
        {
            recvbyte = NHTTPi_SocRecv(mutexInfo_p, req_p, bgnEndInfo_p->socket, &res_p->hdrBufFirst[threadData_p->recvBufwp], 1, 0);
            tmpBuf[threadData_p->recvBufwp & 3] = res_p->hdrBufFirst[threadData_p->recvBufwp];
        }
        else
        {

            int wp = threadData_p->recvBufwp & NHTTP_HDRRECVBUF_BLOCKMASK;

            if (wp == 0)
            {
                        
                if (headerBuf_p)
                {
                    headerBuf_p->next_p = (NHTTPi_HDRBUFLIST*)NHTTPi_alloc( sizeof(NHTTPi_HDRBUFLIST), 4);
                    headerBuf_p = headerBuf_p->next_p;
                }
                else
                {
                    headerBuf_p = (NHTTPi_HDRBUFLIST*)NHTTPi_alloc( sizeof(NHTTPi_HDRBUFLIST), 4);
                    res_p->hdrBufBlock_p = headerBuf_p;
                }
                        
                if (!headerBuf_p)
                {
                    PRINTF("NHTTP_ERROR_ALLOC function : %s line : %d\n", __func__, __LINE__);
                    threadData_p->result = NHTTP_ERROR_ALLOC;
                    DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                    return FALSE;
                }
                headerBuf_p->next_p = NULL;
            }
            recvbyte = NHTTPi_SocRecv(mutexInfo_p, req_p, bgnEndInfo_p->socket, &headerBuf_p->block[wp], 1, 0);
            tmpBuf[threadData_p->recvBufwp & 3] = headerBuf_p->block[wp];
        }
             
        if (recvbyte <= 0)
        {
            threadData_p->result = NHTTP_ERROR_NETWORK;
            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
            return FALSE;
        }
                
        threadData_p->recvBufwp += recvbyte;
                
        //if ( threadData_p->recvBufwp >= 4 )
        {
            if (NHTTPi_CheckHeaderEnd(tmpBuf, threadData_p->recvBufwp))
            {
                // �w�b�_�I���
                res_p->headerLen = threadData_p->recvBufwp;
                break;
            }
        }
    } // for (headerBuf_p = res_p->hdrBufBlock_p, threadData_p->recvBufwp = 0; ; )
            
    if (res_p->headerLen == 0)
    {
        // �G���[�l�Z�b�g�i�Ȃ񂩓��e�ɂ�����Ȃ��C�����܂����cDS�ƈ�v�����邽�߁j
        threadData_p->result = NHTTP_ERROR_HTTPPARSE;
        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
        return FALSE;
    }
     
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadParseHeaderProc
  Description:  HTTP�w�b�_�[���
  Arguments:    threadData_p
  Returns:      TRUE  ����
                FALSE ���s                
 *---------------------------------------------------------------------------*/
static int NHTTPi_ThreadParseHeaderProc(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPRes*           res_p = req_p->res_p;
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    char*		commBuf_p = (char*)&(threadInfo_p->commBuf[0]);
    int                 labelend;
    int                 n;
    
    // HTTP�w�b�_�����
    // �Ƃ肠����Status-Line����status code���Ƃ��Ă���
    if (!NHTTPi_loadFromHdrRecvBuf(res_p, threadData_p->tmpHeaderBuf, 0, 14))
    {
        threadData_p->result = NHTTP_ERROR_HTTPPARSE;
        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
        return FALSE;
    }
            
    //DEBUG_PRINTF("[%d] threadData_p->tmpHeaderBuf [%s]\n", id, threadData_p->tmpHeaderBuf);
            
    if (NHTTPi_strnicmp(threadData_p->tmpHeaderBuf, "HTTP/", 5) != 0)
    {
        threadData_p->result = NHTTP_ERROR_HTTPPARSE;
        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
        return FALSE;
    }
    if (threadData_p->tmpHeaderBuf[8] != ' ')
    {
        threadData_p->result = NHTTP_ERROR_HTTPPARSE;
        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
        return FALSE;
    }
    res_p->httpStatus = NHTTPi_strToInt(&threadData_p->tmpHeaderBuf[9], 3);

    DEBUG_PRINTF("[%s][%d] httpStatus %d\n", __func__, threadData_p->id, res_p->httpStatus);
    
    if (res_p->httpStatus < 0)
    {
        threadData_p->result = NHTTP_ERROR_HTTPPARSE;
        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
        return FALSE;
    }
            
    if (NHTTPi_findNextLineHdrRecvBuf(res_p, 12, res_p->headerLen, &labelend, NULL) < 0)
    {
        threadData_p->result = NHTTP_ERROR_HTTPPARSE;
        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
        return FALSE;
    }
            
    // Content-Length���Ȃ����connection reset�����܂Ŏ�M
    threadData_p->contentLength = NHTTPi_getHeaderValue(res_p, "Content-Length", &n);		// ���̎��_�ł͕�����

    DEBUG_PRINTF("[%s][%d] threadData_p->contentLength %d\n", __func__, threadData_p->id, threadData_p->contentLength);
    
    if (threadData_p->contentLength == 0)
    {
        threadData_p->result = NHTTP_ERROR_NONE; // ��������ERROR_NONE
        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
        return FALSE;
    }
    
    if (threadData_p->contentLength > LEN_COMMBUF)
    {
        threadData_p->result = NHTTP_ERROR_HTTPPARSE;
        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
        return FALSE;
    }
            
    if (threadData_p->contentLength > 0)
    {
        if (!NHTTPi_loadFromHdrRecvBuf(res_p, commBuf_p, n, threadData_p->contentLength))
        {
            threadData_p->result = NHTTP_ERROR_HTTPPARSE;
            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
            return FALSE;
        }
        threadData_p->contentLength = NHTTPi_strToInt( commBuf_p, threadData_p->contentLength );
        if (threadData_p->contentLength < 0)
        {
            threadData_p->result = NHTTP_ERROR_HTTPPARSE;
            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
            return FALSE;
        }
        res_p->contentLength = threadData_p->contentLength;
    }
    else
    {
        // Content-Length�Ȃ�
        res_p->contentLength = 0xffffffff;
    }

    // Connection��Keep-Alive���ǂ���
    // SSL�Ȃ疳��
    //DEBUG_PRINTF("Connection��Keep-Alive���ǂ���\n");
            
    if (req_p->isSSL)
    {
        threadData_p->isKeepAlive = FALSE;
    }
    else
    {
        int connectionHeaderStrLen = NHTTPi_getHeaderValue(res_p, "Connection", &n);
        if (connectionHeaderStrLen == 0)
        {
            threadData_p->result = NHTTP_ERROR_HTTPPARSE;
            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
            threadData_p->isKeepAlive = FALSE;
            return FALSE;
        }
        if (connectionHeaderStrLen > LEN_COMMBUF)
        {
            threadData_p->isKeepAlive = FALSE;
        }
        else
        {
            if (connectionHeaderStrLen > 0)
            {
                if (NHTTPi_compareTokenN_HdrRecvBuf(res_p, n, n+connectionHeaderStrLen, "Keep-Alive", 0) == 0)
                {
                    threadData_p->isKeepAlive = TRUE;
                }
                else
                {
                    threadData_p->isKeepAlive = FALSE;
                }
            }
            else
            {
                threadData_p->isKeepAlive = FALSE;
            }
        }
        DEBUG_PRINTF("[%s][%d] connectionHeaderStrLen %d LEN_COMMBUF %d isKeepAlive = %d \n",
                     __func__,  __LINE__, connectionHeaderStrLen, LEN_COMMBUF, threadData_p->isKeepAlive);
    }
    
    // Transfer-Encoding��chunked���ǂ���
    //DEBUG_PRINTF("Transfer-Encoding��chunked���ǂ���\n");
            
    threadData_p->isChunked = NHTTPi_getHeaderValue(res_p, "Transfer-Encoding", &n);
    if (threadData_p->isChunked == 0)
    {
        threadData_p->result = NHTTP_ERROR_HTTPPARSE;
        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
        return FALSE;
    }
    if (threadData_p->isChunked > LEN_COMMBUF)
    {
        threadData_p->isChunked = FALSE;
    }
    else
    {
        threadData_p->isChunked = (threadData_p->isChunked > 0) ? (NHTTPi_compareTokenN_HdrRecvBuf(res_p, n, n+threadData_p->isChunked, "chunked", ';') == 0): FALSE;
    }

    DEBUG_PRINTF("[%s][%d] header parsed\n", __func__, threadData_p->id);
    
    // NHTTP_REQMETHOD_HEAD���ɐ���Ɋ��������ꍇ�ł� NHTTP_ERROR_HTTPPARSE �ɂȂ��Ă����o�O�C��
    // PARSE�����������̂ŃG���[������
    threadData_p->result = NHTTP_ERROR_NONE;
    // BODY��M�Ɏ��s���Ă��w�b�_�[�̃p�[�X���������Ă���΃w�b�_�[���m�F�ł���悤�ɂ���
    res_p->isHeaderParse = TRUE;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_ThreadRecvBodyProc
  Description:  HTTP�{�f�B��M
  Arguments:    threadData_p 
  Returns:      TRUE  ����
                FALSE ���s                
 *---------------------------------------------------------------------------*/
static int NHTTPi_ThreadRecvBodyProc(NHTTPThreadData* threadData_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPReq*           req_p = reqInfo_p->reqCurrent->req_p;
    NHTTPRes*           res_p = req_p->res_p;
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_Request2Connection(mutexInfo_p, req_p);
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    char*		commBuf_p = (char*)&(threadInfo_p->commBuf[0]);
    int                 recvbyte = 0;
    char                tmpBuf[2];
    int                 n;
    
    DEBUG_PRINTF("[%s][%d] contentLength %d HttpStatus %d \n", __func__, threadData_p->id, threadData_p->contentLength, res_p->httpStatus);
    

    // �T�[�o�[�͈ȉ��̏����ł̓��b�Z�[�W�{�f�B���܂�ł͂Ȃ�Ȃ�����
    // �N���C�A���g�̓{�f�B��M���X�L�b�v����K�v������
    // HEAD���N�G�X�g
    // 1xx 204 304���X�|���X
    if ((req_p->method == NHTTP_REQMETHOD_HEAD) ||
        (res_p->httpStatus == 204) ||
        (res_p->httpStatus == 304) ||
        (res_p->httpStatus >= 100 && res_p->httpStatus < 200))
    {
        return TRUE;
    }
            
    DEBUG_PRINTF("[%s][%d]  BODY��M\n", __func__, threadData_p->id );

    NHTTPi_SetVirtualContentLength(connection_p, 0);
    
    if (connection_p != NULL)
    {
        connection_p->status = NHTTP_STATUS_RECEIVING_BODY;
    }
    
    if (threadData_p->contentLength >= 0)
    {

        DEBUG_PRINTF("[%s][%d] contentLength >= 0 \n", __func__, threadData_p->id );
        
        NHTTPi_SetVirtualContentLength(connection_p, (u32)threadData_p->contentLength);
        
        // Content-Length����
        for ( ; (threadData_p->contentLength > 0); )
        {
            if (threadData_p->result != NHTTP_ERROR_BUFFULL)
            {
                if (NHTTPi_BufFull(mutexInfo_p, res_p) == FALSE)
                {
                    threadData_p->result = NHTTP_ERROR_BUFFULL;
                    // �ǂݎ̂ėp�o�b�t�@�Z�b�g
                    res_p->recvBuf_p = &(threadData_p->tmpBodyBuf[0]);
                    res_p->recvBufLen = TMP_BODY_BUF_SIZE;
                }
            }
                    
            if (threadData_p->result == NHTTP_ERROR_BUFFULL)
            {
                recvbyte = NHTTPi_RecvBufN(mutexInfo_p, req_p, bgnEndInfo_p->socket, 0, threadData_p->contentLength, 0);
            }
            else
            {
                recvbyte = NHTTPi_RecvBufN(mutexInfo_p, req_p, bgnEndInfo_p->socket, res_p->bodyLen, threadData_p->contentLength, 0);
            }

            if (recvbyte < 0)
            {
                DEBUG_PRINTF("[%s][%d] NHTTPi_SocRecv error = %d\n", __func__, threadData_p->id, recvbyte);
                
                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                return FALSE;
            }
            else if (recvbyte == 0)
            {
                DEBUG_PRINTF("[%s][%d] Connection Closed.\n", __func__, threadData_p->id);
                break;
            }
            DEBUG_PRINTF("[%s][%d] body %d received %d\n",  __func__, threadData_p->id, threadData_p->contentLength, recvbyte);
            
            if (threadData_p->result != NHTTP_ERROR_BUFFULL)
            {
                res_p->bodyLen += recvbyte;
                res_p->totalBodyLen += recvbyte;
            }
            threadData_p->contentLength -= recvbyte;
        }
                
        if (threadData_p->result != NHTTP_ERROR_BUFFULL)
        {
            if (threadData_p->contentLength != 0)
            {
                threadData_p->result = NHTTPi_isRecvBufFull(res_p, res_p->bodyLen)? NHTTP_ERROR_BUFFULL : NHTTP_ERROR_NETWORK;
            }
            else
            {
                threadData_p->result = NHTTP_ERROR_NONE;
            }
        }
    }
    else
    {
        threadData_p->result = NHTTP_ERROR_NETWORK;

        DEBUG_PRINTF("[%s][%d] threadData_p->contentLength < 0 \n", __func__, threadData_p->id );           

        // Content-Length�Ȃ�
        if (threadData_p->isChunked)
        {
            // Transfer-Encoding: chunked
            int chunkSize = -1;

            DEBUG_PRINTF("[%s][%d] chunked \n", __func__, threadData_p->id);
            
            while (1)
            {
                // chunk size�𓾂�
                // size=0�Ȃ�last chunk
                tmpBuf[0] = 0;
                tmpBuf[1] = 0;
                for (threadData_p->recvBufwp = 0; threadData_p->recvBufwp < LEN_COMMBUF; threadData_p->recvBufwp++)
                {
                    recvbyte = NHTTPi_SocRecv(mutexInfo_p, req_p, bgnEndInfo_p->socket, &(commBuf_p[threadData_p->recvBufwp]), 1, 0);
                    if (recvbyte < 0)
                    {
                        DEBUG_PRINTF("[%s][%d] NHTTPi_SocRecv error = %d\n", __func__, threadData_p->id, recvbyte);
                        DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                        return FALSE;
                    }
                                
                    tmpBuf[threadData_p->recvBufwp & 1] = commBuf_p[threadData_p->recvBufwp];
                    if ((tmpBuf[threadData_p->recvBufwp & 1] == ';') ||
                        ((tmpBuf[threadData_p->recvBufwp & 1] == '\n') && (tmpBuf[ (threadData_p->recvBufwp-1) & 1 ] == '\r')))
                    {
                        if (tmpBuf[threadData_p->recvBufwp & 1] == '\n')
                        {
                            n = threadData_p->recvBufwp - 1;
                        }
                        else
                        {
                            n = threadData_p->recvBufwp;
                            if (NHTTPi_SkipLineBuf(mutexInfo_p, req_p, bgnEndInfo_p->socket) <= 0)
                            {
                                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                                return FALSE;
                            }
                        }
                        
                        if (n == 0)
                        {
                            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                            return FALSE;
                        }
                                    
                        chunkSize = NHTTPi_strToHex(commBuf_p, n);
                        if (chunkSize < 0)
                        {
                            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                            return FALSE;
                        }
                                    
                        break;
                    }
                }
                if (threadData_p->recvBufwp == LEN_COMMBUF)
                {
                    DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                    threadData_p->result = NHTTP_ERROR_HTTPPARSE;
                    return FALSE;
                }

                if (chunkSize > 0)
                {
                    NHTTPi_SetVirtualContentLength(connection_p, (u32)chunkSize);
                            
                    while (chunkSize > 0)
                    {
                        if (threadData_p->result != NHTTP_ERROR_BUFFULL)
                        {
                            if (NHTTPi_BufFull(mutexInfo_p, res_p) == FALSE)
                            {
                                threadData_p->result = NHTTP_ERROR_BUFFULL;
                                // �ǂݎ̂ėp�o�b�t�@�Z�b�g
                                res_p->recvBuf_p = &(threadData_p->tmpBodyBuf[0]);
                                res_p->recvBufLen = TMP_BODY_BUF_SIZE;
                            }
                        }
                        if (threadData_p->result == NHTTP_ERROR_BUFFULL)
                        {
                            recvbyte = NHTTPi_RecvBufN(mutexInfo_p, req_p, bgnEndInfo_p->socket, 0, chunkSize, 0);
                        }
                        else
                        {
                            recvbyte = NHTTPi_RecvBufN(mutexInfo_p, req_p, bgnEndInfo_p->socket, res_p->bodyLen, chunkSize, 0);
                        }
                        if (recvbyte <= 0)
                        {
                            DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                            return FALSE;
                        }

                        res_p->bodyLen += recvbyte;
                        res_p->totalBodyLen += recvbyte;
                        chunkSize -= recvbyte;
                                    
                        if (chunkSize == 0)
                        {
                            // CRLF��ǂݎ̂Ă�
                            recvbyte = NHTTPi_SocRecv(mutexInfo_p, req_p, bgnEndInfo_p->socket, commBuf_p, 2, 0);
                            if (recvbyte <= 0)
                            {
                                DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                                return FALSE;
                            }
                        }
                    }
                }
                else
                {
                    // last chunk
                    NHTTPi_SkipLineBuf(mutexInfo_p, req_p, bgnEndInfo_p->socket);
                    threadData_p->result = NHTTP_ERROR_NONE;
                    break;
                }
            } // while (1)
        }
        else
        {

            DEBUG_PRINTF("[%s][%d] connection reset�����܂Ŏ�M\n", __func__, threadData_p->id);
            
            // connection reset�����܂Ŏ�M
            for ( ; ; )
            {

                if (NHTTPi_BufFull(mutexInfo_p, res_p) == FALSE)
                {
                    threadData_p->result = NHTTP_ERROR_BUFFULL;
                    // �ǂݎ̂ėp�o�b�t�@�Z�b�g
                    res_p->recvBuf_p = &(threadData_p->tmpBodyBuf[0]);
                    res_p->recvBufLen = TMP_BODY_BUF_SIZE;
                }
                if (threadData_p->result == NHTTP_ERROR_BUFFULL)
                {
                    recvbyte = NHTTPi_RecvBuf(mutexInfo_p, req_p, bgnEndInfo_p->socket, 0, 0);
                }
                else
                {
                    recvbyte = NHTTPi_RecvBuf(mutexInfo_p, req_p, bgnEndInfo_p->socket, res_p->bodyLen, 0);
                }
                if (recvbyte < 0)
                {
                    DEBUG_PRINTF("[%s][%d] NHTTPi_SocRecv error = %d\n", __func__, threadData_p->id, recvbyte);
                    DEBUG_PRINTF("[%s]****** line %d comm end ******\n", __func__, __LINE__);
                    return FALSE;
                }
                else if (recvbyte == 0)
                {
                    DEBUG_PRINTF("[%s][%d] Connection Closed.\n", __func__, threadData_p->id);
                    if (threadData_p->result != NHTTP_ERROR_BUFFULL)
                    {
                        threadData_p->result = NHTTP_ERROR_NONE;
                    }
                    break;
                }
                DEBUG_PRINTF("[%s][%d] body none received %d\n", __func__, threadData_p->id, recvbyte);
                res_p->bodyLen += recvbyte;
                res_p->totalBodyLen += recvbyte;
                            
            } // for ( ; !NHTTPi_isRecvBufFull( res_p, res_p->bodyLen ); )
        } // if (isChunked)
    } // if (threadData_p->contentLength < 0)

    /** Body��M�����R�[���o�b�N */
    {
        NHTTPConnection*	connection_p = NHTTPi_Response2Connection(mutexInfo_p, res_p);
        if ((threadData_p->result == NHTTP_ERROR_NONE) && (connection_p != NULL))
        {
            NHTTPi_ReceivedCallback(mutexInfo_p, connection_p);
        }
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_CommThreadProc
  Description:  HTTP�ʐM�X���b�h
                NHTTPi_Startup() ���� NHTTPi_CleanupAsync() �܂ő��݂���
  Arguments:    param
 *---------------------------------------------------------------------------*/
extern void NHTTPi_CommThreadProcMain(void)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*    bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPThreadData     threadData;

    // �X���b�h�ϐ�������
    NHTTPi_ThreadInit(&threadData);

    while (!bgnEndInfo_p->isThreadEnd)
    {
        if (threadData.restartConnect == FALSE)
        {
            // �L���[���X�g�̐擪������Ă��āA���s���ɂ���
            // �L���[���X�g���󂾂�����Akick�����܂ő҂�
            if (NHTTPi_ThreadExecReqQueue(&threadData) == FALSE)
            {
                continue;
            }
            
            // �L�����Z���`�F�b�N
            if (reqInfo_p->reqCurrent->req_p->doCancel)
            {
                DEBUG_PRINTF("[%s][%d] �L�����Z������������܂���\n", __func__, __LINE__);
                NHTTPi_ThreadReqEnd(&threadData); // �� comm_end���������s���Đ擪�ɖ߂�
                continue;            
            }
            
            // �z�X�g�������v���Z�X
            if (NHTTPi_ThreadHostAddrProc(&threadData) == FALSE)
            {
                NHTTPi_ThreadReqEnd(&threadData); // �� comm_end���������s���Đ擪�ɖ߂�
                continue;            
            }
        }
        
        if (threadData.restartConnect == TRUE)
        {
            threadData.restartConnect = FALSE;
            // �� comm_begin:
            // comm_begin�Ɠ��l�̖������ʂ������߁A
            // SEND���ɖ��ڑ��̏ꍇ�� restartConnect ��TRUE�ɂ��ăR�R�ɖ߂��Ă��܂�
        }
        
        // �ڑ��v���Z�X
        if (NHTTPi_ThreadConnectProc(&threadData) == FALSE)
        {
            NHTTPi_ThreadReqEnd(&threadData); // �� comm_end���������s���Đ擪�ɖ߂�
            continue;
        }

        // PROXY�֘A����
        switch (NHTTPi_ThreadProxyProc(&threadData))
        {
        case NHTTP_SEND_SUCCESS_e:            // �E���̃X�e�b�v��
            break;
        case NHTTP_SEND_NOTCONN_e:            // �E�ڑ����m�����Ă��Ȃ�
            threadData.restartConnect = TRUE; // ���̓X�L�b�v����comm_begin����ڑ�����蒼��
            continue;
            break;
        case NHTTP_SEND_ERROR_e:              // �E�G���[
            NHTTPi_ThreadReqEnd(&threadData); // �� comm_end���������s���Đ擪�ɖ߂�
            continue;
            break;
        }

        // HTTP���N�G�X�g���M
        switch (NHTTPi_ThreadSendProc(&threadData))
        {
        case NHTTP_SEND_SUCCESS_e:            // �E���̃X�e�b�v��
            break;
        case NHTTP_SEND_NOTCONN_e:            // �E�ڑ����m�����Ă��Ȃ�
            threadData.restartConnect = TRUE; // ���̓X�L�b�v����comm_begin����ڑ�����蒼��
            continue;
            break;
        case NHTTP_SEND_ERROR_e:              // �E�G���[
        case NHTTP_SEND_ERROR_POST_CALLBACK_e: 
            NHTTPi_ThreadReqEnd(&threadData); // �� comm_end���������s���Đ擪�ɖ߂�
            continue;
            break;
        }
        
        // �L�����Z���`�F�b�N
        if (reqInfo_p->reqCurrent->req_p->doCancel)
        {
            DEBUG_PRINTF("[%s][%d] �L�����Z������������܂���\n", __func__, __LINE__);
            NHTTPi_ThreadReqEnd(&threadData); // �� comm_end���������s���Đ擪�ɖ߂�
            continue;            
        }

        // �w�b�_�[��M
        if (NHTTPi_ThreadRecvHeaderProc(&threadData) == FALSE)
        {
            NHTTPi_ThreadReqEnd(&threadData); // �� comm_end���������s���Đ擪�ɖ߂�
            continue;
        }

        // �w�b�_�[���
        if (NHTTPi_ThreadParseHeaderProc(&threadData) == FALSE)
        {
            NHTTPi_ThreadReqEnd(&threadData); // �� comm_end���������s���Đ擪�ɖ߂�
            continue;
        }

        // �{�f�B��M
        if (NHTTPi_ThreadRecvBodyProc(&threadData) == FALSE)
        {
            NHTTPi_ThreadReqEnd(&threadData); // �� comm_end���������s���Đ擪�ɖ߂�
            continue;
        }
        
        // �� comm_end:
        // �I��
        NHTTPi_ThreadReqEnd(&threadData); 
    }
}
