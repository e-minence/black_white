/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     NHTTP_request.c

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
#include "d_nhttp_common.h"
#include "NHTTPlib_int_socket.h"
#include "NHTTP_request.h"
#include "NHTTP_bgnend.h"
#include "NHTTP_list.h"
#include "NHTTP_nonport.h"

const char NHTTPi_strMultipartBound[] = STR_POSTBOUND;

/*-------------------------------------------------------------------------*

  Name:         NHTTPi_InitRequestInfo

  Description:  NHTTPReqInfo�\���̂̏����ݒ�
                 
  Arguments:    reqInfo_p  NHTTPReqInfo�\���̂ւ̃|�C���^

  Returns:      
 *-------------------------------------------------------------------------*/
extern void NHTTPi_InitRequestInfo(NHTTPReqInfo* reqInfo_p)
{
    reqInfo_p->reqCurrent = NULL;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_CreateRequest

  Description:  �����Ŏw�肳�ꂽ�p�����[�^���ݒ肳�ꂽNHTTPReq�I�u�W�F�N�g�𐶐�����

  Arguments:    url             �ڑ���URL(NULL�I�[ASCII������)�ւ̃|�C���^
                method          ���s����HTTP���N�G�X�g���\�b�h
                buf             HTTP�{�f�B��M�o�b�t�@�ւ̃|�C���^
                len             HTTP�{�f�B��M�o�b�t�@�̒���
                callback	HTTP�ʐM�I�����R�[���o�b�N�֐��ւ̃|�C���^
                param           HTTP�ʐM�I�����R�[���o�b�N�֐��ɗ^������p�����[�^
                
  Returns:      NULL            �����Ɏ��s����
                NULL�ȊO        �m�ۂ��ꂽNHTTPReq�I�u�W�F�N�g�ւ̃|�C���^
 *-------------------------------------------------------------------------*/
extern NHTTPReq* NHTTPi_CreateRequest(NHTTPBgnEndInfo*	bgnEndInfo_p,
                                     const char*	url_p,
                                     NHTTPReqMethod	method,
                                     char*		buf_p,
                                     u32		len,
                                     void*		param_p,
                                     NHTTPBufFull	bufFull,
                                     NHTTPFreeBuf	freeBuf)
{
    // �v���I�u�W�F�N�g������
    // �X���b�h�J�n�͂܂�
    int i;
    int suflen, urllen, prelen;
    int nhostenc, deccount;
    int declen, hostend;
    int port;
    int decode_urllen;
    
    char	a_url[LEN_URLBUF];
    char*	prefix_p;
    NHTTPReq*	req_p = NULL;

    NHTTPi_ASSERT(NHTTPi_IsOpened(bgnEndInfo_p));
    NHTTPi_ASSERT(url_p);
    
    switch (method)
    {
    case NHTTP_REQMETHOD_GET:
    case NHTTP_REQMETHOD_POST:
    case NHTTP_REQMETHOD_HEAD:
        break;
    default:
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_UNKNOWN);
        goto errend;
    }
    
    //
    // �������m��
    //
    req_p = (NHTTPReq*)NHTTPi_alloc(sizeof(NHTTPReq), 4);

    if (!req_p)
    {
        DEBUG_PRINTF("[%s][line:%d] NHTTP_ERROR_ALLOC\n", __func__, __LINE__);
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_ALLOC);
        goto errend;
    }
    
    NHTTPi_memclr(req_p, sizeof(NHTTPReq));
    
    req_p->res_p = (NHTTPRes*)NHTTPi_alloc(sizeof(NHTTPRes), 4);

    if (!req_p->res_p)
    {
        DEBUG_PRINTF("[%s][line:%d] NHTTP_ERROR_ALLOC\n", __func__, __LINE__);
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_ALLOC);
        goto errend;
    }
    
    NHTTPi_memclr(req_p->res_p, sizeof(NHTTPRes));
    req_p->res_p->recvBuf_p = buf_p;
    req_p->res_p->recvBufLen = len;
    req_p->res_p->bufFull = bufFull;
    req_p->res_p->freeBuf = freeBuf;

    //
    // URL�̉��
    //
    urllen = (int)NHTTPi_strnlen(url_p, sizeof(a_url) - 1);
    
    // "http://"
    if (urllen <= 7)
    {                
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_DNS);
        goto errend;
    }
    
    // �ꎞURL�̈�ɃR�s�[
    NHTTPi_memclr(a_url, sizeof(a_url));
    NHTTPi_memcpy(a_url, url_p, urllen);
    
    req_p->port = NHTTP_HTTP_PORT;
    suflen = 7;
    
    if (NHTTPi_strnicmp(a_url, "http://", 7) != 0)
    {
        if (NHTTPi_strnicmp(a_url, "https://", 8) != 0)
        {
            NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_DNS);
            goto errend;
        }        
        req_p->isSSL = TRUE;
        req_p->port = NHTTP_HTTPS_PORT;
        suflen = 8;
    }

    prefix_p = a_url + suflen;
    prelen = urllen - suflen;
    
    if (prelen <= 0)
    {
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_DNS);
        goto errend;
    }

    // host,port��������%xx���f�R�[�h��̕����������߂邽�߂�
    // host,port������%xx�̐������߂�
    // host,port�ȊO��%xx�͐ڑ���ɑf�ʂ�
    for (i = 0, nhostenc = 0, deccount = 0; (i < prelen) && (prefix_p[i] != '/'); i++)
    {
        if (deccount == 2)
        {
            deccount--;
        }
        else if (deccount == 1)
        {
            signed char c = (signed char)NHTTPi_strToHex(&prefix_p[i-1], 2);
            deccount--;
            if (c < 0)
            {
                NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_DNS);
                goto errend;
            }
            if (c == '/')
            { 
                nhostenc--;
                break;
            }
        }
        else
        {
            if (prefix_p[i] == '%')
            {
                deccount = 2;
                nhostenc++;
            }
        }
    }
    
    if (deccount)
    {
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_DNS);
        goto errend;
    }

    decode_urllen = suflen+prelen-nhostenc*2+1;
    
    // ���[�N�G���A���m�ۂ��āAhost,port��%xx�����f�R�[�h����URL���R�s�[
    req_p->URL_p = (char*)NHTTPi_alloc( (size_t)decode_urllen, 4 );

    if (!req_p->URL_p)
    {
        DEBUG_PRINTF("[%s][line:%d] NHTTP_ERROR_ALLOC\n", __func__, __LINE__);
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_ALLOC);
        goto errend;
    }
    NHTTPi_memclr(req_p->URL_p, decode_urllen);
    NHTTPi_memcpy(req_p->URL_p, a_url, suflen);
    
    for (i=0,declen=0,deccount=0,hostend=FALSE; i<prelen; i++)
    {
        if (deccount == 2)
        {
            deccount--;
        }
        else if (deccount == 1)
        {
            char c = (char)NHTTPi_strToHex(&prefix_p[i-1], 2);
            deccount--;
            req_p->URL_p[ suflen+declen-1 ] = c;
            if (c == '/')
            {
                hostend = TRUE;
            }
        }
        else
        {
            if (prefix_p[i] == '/')
            {
                hostend = TRUE;
            }
            if (!hostend & (prefix_p[i] == '%' ))
            {
                deccount = 2;
            }
            else
            {
                req_p->URL_p[ suflen+declen ] = prefix_p[i];
            }
            declen++;
        }
    }
    
    // URL_p�̏I�[��NULL����
    req_p->URL_p[ suflen + declen ] = 0;
    DEBUG_PRINTF("[%s] URL_p [%s]\n", __func__, req_p->URL_p);

    prefix_p = &req_p->URL_p[ suflen ];
    prelen = declen;
    
    // URL����z�X�g�����������o��
    for (i=0; i<prelen; i++)
    {
        if ((prefix_p[i] == '/') || (prefix_p[i] == ':'))
        {
            req_p->hostLen = i + suflen;
            break;
        }
    }
    
    if (i == prelen)
    {
        req_p->hostDescLen = req_p->hostLen = i + suflen;
    }
    else
    {
        if (prefix_p[i] == '/')
        {
            req_p->hostDescLen = req_p->hostLen;
        }
        else if (prefix_p[i] == ':')
        {
            // �|�[�g�ԍ������o��
            for ( ; i<prelen; i++ )
            {
                if (prefix_p[i] == '/')
                {
                    req_p->hostDescLen = i + suflen;
                    break;
                }
            }

            if (i == prelen)
            {
                req_p->hostDescLen = i + suflen;
            }
            else
            {
                port = NHTTPi_strtonum( &req_p->URL_p[req_p->hostLen+1], (int)(req_p->hostDescLen - (req_p->hostLen+1)) );

                if (port < 0)
                {
                    port = req_p->port;
                }
                else if (port > 65535)
                {
                    NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_DNS);
                    goto errend;
                }
                
                req_p->port = (unsigned short)port;
            }
        }
    }

    // ���[�N�G���A���m�ۂ���Host����ۑ�
    {
        size_t a_hostnameLen = (u32)(req_p->hostLen - (req_p->isSSL ? 8 : 7));
        req_p->hostname_p = (char*)NHTTPi_alloc(a_hostnameLen + 1, 4);
        
        if (!req_p->hostname_p)
        {
            DEBUG_PRINTF("[%s][line:%d] NHTTP_ERROR_ALLOC\n", __func__, __LINE__);
            NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_ALLOC);
            goto errend;
        }
        
        NHTTPi_memclr(req_p->hostname_p, (int)(a_hostnameLen + 1));
        NHTTPi_memcpy(req_p->hostname_p, (req_p->URL_p + (req_p->isSSL ? 8 : 7)), (int)a_hostnameLen);
        // hostname_p�̏I�[��NULL����
        //req_p->hostname_p[a_hostnameLen] = '\0';
        DEBUG_PRINTF("[%s] hostname [%s]\n", __func__, req_p->hostname_p);
    }
    
    // ���̑�
    NHTTPi_memcpy(req_p->tagPost, NHTTPi_strMultipartBound, sizeof(req_p->tagPost));
    //req_p->param = param;
    req_p->method = method;
//    req_p->pCallback = callback;

    // SSL
    req_p->sslId = 0;
    req_p->clientCertData_p = NULL;
    req_p->clientCertSize = 0;
    req_p->privateKeyData_p = NULL;
    req_p->privateKeySize = 0;
    req_p->rootCAData_p = NULL;
    req_p->rootCASize = 0;
    req_p->useClientCertDefault = FALSE;
    
    req_p->verifyOption = NHTTP_SSL_VERIFY_NONE;
    req_p->disableVerifyOption = NHTTP_SSL_VERIFY_NONE;

    // res�ɂ�param��n���Ă���
    req_p->res_p->param_p = param_p;

    // PROXY
    req_p->isProxy = FALSE;

    // soRecvSize�����ݒ�
    req_p->soRecvSize = 0;
    
    return req_p;
    
errend:
    
    if (req_p)
    {
        if (req_p->URL_p)
        {
            NHTTPi_free(req_p->URL_p);
        }

        if (req_p->hostname_p)
        {
            NHTTPi_free(req_p->hostname_p);
        }

        if (req_p->res_p)
        {
            NHTTPi_free(req_p->res_p);
        }
        
        NHTTPi_free(req_p);
    }
    
    return NULL;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_DestroyRequest
  Description:  �����Ŏw�肳�ꂽNHTTPReq�I�u�W�F�N�g���g�p���Ă��郁�������������
  Arguments:    req	�����Ώۂ�HTTPReq�I�u�W�F�N�g�ւ̃|�C���^
  Returns:      �Ȃ�
 *-------------------------------------------------------------------------*/
extern void NHTTPi_DestroyRequest(NHTTPSysInfo* sysInfo_p, NHTTPReq* req_p)
{
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);

    NHTTPi_ASSERT(NHTTPi_IsOpened(NHTTPi_GetBgnEndInfoP(sysInfo_p)));
    NHTTPi_ASSERT(req_p);
    NHTTPi_ASSERT(!req_p->isStarted);

    /** ���ݎQ�Ɖ��� */
    {
        NHTTPConnection* connection_p = NHTTPi_Request2Connection(mutexInfo_p, req_p);
        
        if (connection_p != NULL)
        {
            connection_p->res_p = NULL;
        }
    }
    
    NHTTPi_free(req_p->res_p);
    NHTTPi_destroyRequestObject(mutexInfo_p, req_p);
}

/*-------------------------------------------------------------------------*
  Name:         deleteStrList
  Description:  �w�肵�����X�g���폜 
  Arguments:    list_p
 *-------------------------------------------------------------------------*/
static void deleteStrList(NHTTPi_DATALIST* list_p)
{
    NHTTPi_DATALIST *p = NULL;
    
    while (list_p)
    {
        if (list_p != list_p->prev_p)
        {
            p = list_p->prev_p->prev_p;
            NHTTPi_free(list_p->prev_p);
            list_p->prev_p = p;
        }
        else
        {
            NHTTPi_free(list_p);
            list_p = NULL;
        }
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_destroyRequestObject
  Description:  �v���I�u�W�F�N�g��j��
                req�͎��s�����X�g�ɓ����Ă��Ă͂Ȃ�Ȃ�
                �ʏ�A���N�G�X�g���I������Ƃ��ɁA�ʐM�X���b�h���Ă�
  Arguments:    req
  Returns       TRUE
 *-------------------------------------------------------------------------*/
extern int NHTTPi_destroyRequestObject(NHTTPMutexInfo* mutexInfo_p, NHTTPReq* req_p)
{
    /* ���ݎQ�Ɖ��� */
    {
        NHTTPConnection* connection_p = NHTTPi_Request2Connection(mutexInfo_p, req_p);
        
        if (connection_p != NULL)
        {
            connection_p->req_p = NULL;
        }
    }
    
    deleteStrList(req_p->listHeader_p);
    deleteStrList(req_p->listPost_p);
    
    NHTTPi_free(req_p->URL_p);
    NHTTPi_free(req_p->hostname_p);
    NHTTPi_free(req_p);
    // req_p->res_p�͉�����Ȃ�
    return TRUE;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_SendRequestAsync
  Description:  �K�؂ɐݒ肳�ꂽNHTTPReq�I�u�W�F�N�g�ɏ]����HTTP���N�G�X�g�����s����
  Arguments:    req	�����Ώۂ�HTTPReq�I�u�W�F�N�g�ւ̃|�C���^
  Returns:      0�ȏ�	�����ɐ������� (�l�͎��s����HTTP���N�G�X�g�ɗ^������ID)
                0����	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTPi_SendRequestAsync(NHTTPSysInfo* sysInfo_p, NHTTPReq* req_p)
{
    // ���N�G�X�g���L���[���X�g�ɉ�����
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    int			id;

    NHTTPi_ASSERT(NHTTPi_IsOpened(bgnEndInfo_p));
    NHTTPi_ASSERT(req_p);

    // ���ɊJ�n����Ă���
    if (req_p->isStarted)
    {
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_UNKNOWN);
        return -1;
    }
    
    NHTTPi_lockReqList(mutexInfo_p);
    {
        NHTTPListInfo*	listInfo_p = NHTTPi_GetListInfoP(sysInfo_p);
        id = NHTTPi_setReqQueue(listInfo_p, req_p);
        if ( id >= 0 )
        {
            req_p->isStarted = TRUE;
            NHTTPi_kickCommThread(threadInfo_p);
        }
        else
        {
            DEBUG_PRINTF("[%s][line:%d] NHTTP_ERROR_ALLOC\n", __func__, __LINE__);
            NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_ALLOC);
        }
    }
    
    NHTTPi_unlockReqList(mutexInfo_p);
    
    return id;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_CancelRequestAsync
  Description:  NHTTP_SendRequest�֐����瓾��ꂽID���w�肵�AHTTP���N�G�X�g�̒�~��v������
  Arguments:    id	���f���郊�N�G�X�g��id
  Returns:      TRUE	�����ɐ��������A���邢��ID�Ŏw�肳�ꂽHTTP���N�G�X�g�����݂��Ȃ�
                FALSE	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern BOOL NHTTPi_CancelRequestAsync(NHTTPSysInfo* sysInfo_p, int id)
{
    // �X���b�h�ɒ��f�v���𔭍s����
    int rc = FALSE;
    NHTTPReqInfo*       	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPMutexInfo*		mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    volatile NHTTPi_REQLIST*	reqCurrent_p = reqInfo_p->reqCurrent;

    NHTTPi_ASSERT(NHTTPi_IsOpened(NHTTPi_GetBgnEndInfoP(sysInfo_p)));
    
    NHTTPi_lockReqList(mutexInfo_p);
    
    if (reqCurrent_p)
    {
        if (reqCurrent_p->id == id)
        {
            // ���f��v��
            if (!reqCurrent_p->req_p->doCancel)
            {
                reqCurrent_p->req_p->doCancel = TRUE;
                NHTTPi_SocCancel(mutexInfo_p, reqCurrent_p->req_p, (SOCKET)reqCurrent_p->socket_p);
                rc = TRUE;
            }
        }
    }
    
    if (!rc)
    {
        NHTTPListInfo*	listInfo_p = NHTTPi_GetListInfoP(sysInfo_p);
        rc = NHTTPi_freeReqQueue(listInfo_p, mutexInfo_p, id);
    }
    
    NHTTPi_unlockReqList(mutexInfo_p);

    return rc;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_cancelAllRequests
  Description:  �L���[���N���A���āA���s���̃��N�G�X�g�ɂ͒��f��v��
 *-------------------------------------------------------------------------*/
extern void NHTTPi_cancelAllRequests(NHTTPSysInfo* sysInfo_p)
{
    NHTTPReqInfo*		reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPListInfo*		listInfo_p = NHTTPi_GetListInfoP(sysInfo_p);
    NHTTPMutexInfo*		mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    volatile NHTTPi_REQLIST*	reqCurrent_p = reqInfo_p->reqCurrent;

    NHTTPi_lockReqList(mutexInfo_p);
    
    if (reqCurrent_p)
    {
        if (!reqCurrent_p->req_p->doCancel)
        {
            reqCurrent_p->req_p->doCancel = TRUE;
            NHTTPi_SocCancel(mutexInfo_p, reqCurrent_p->req_p, (SOCKET)reqCurrent_p->socket_p);
        }
    }

    NHTTPi_allFreeReqQueue(listInfo_p, mutexInfo_p);
    NHTTPi_unlockReqList(mutexInfo_p);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
