/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     d_nhttp_private.h

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
#include "d_nhttp_common.h"
#include "NHTTP_bgnend.h"
#include "NHTTP_request.h"
#include "NHTTP_response.h"
#include "NHTTP_nonport.h"
#include "d_nhttp_private.h"

/*-------------------------------------------------------------------------*
  Name:         NHTTP_CreateConnection

  Description:  �V�K�̐ڑ��n���h���𐶐�
                 
  Arguments:    url_p		- �ڑ���� URL ������
                method		- �ڑ����\�b�h
                buf_p		- �{�f�B��M�o�b�t�@ �܂��� NULL
                len		- �{�f�B��M�o�b�t�@�� �܂��� 0
                callback	- �C�x���g�ʒm�R�[���o�b�N �܂��� NULL
                userParam_p	- �C�ӂ̃��[�U��`���� �܂��� NULL
                
  Returns:      �V�K�ɐ������ꂽ�ڑ��n���h���A�܂��� NULL
 *-------------------------------------------------------------------------*/
extern NHTTPConnectionHandle NHTTP_CreateConnection(const char* url_p, NHTTPReqMethod method, char* buf_p, u32 len, NHTTPConnectionCallback callback, void* userParam_p)
{
    NHTTPConnection*	connection_p = NULL;
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);

    DEBUG_PRINTF("[%s]\n", __func__);

    //
    // �������m��
    //
    // �ꎞ��M�o�b�t�@(tmpBuf)�p�� 32�o�C�g�A���C�����g
    connection_p = (NHTTPConnectionHandle)NHTTPi_alloc(sizeof(NHTTPConnection), 32);
    
    // �m�ێ��s
    if (!connection_p)
    {
        DEBUG_PRINTF("[%s][line:%d] NHTTP_ERROR_ALLOC\n", __func__, __LINE__);
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_ALLOC);
        return NULL;
    }

    // ���ڑ��n���h���\���̂̏�����
    NHTTPi_memclr(connection_p, sizeof(NHTTPConnection));
    
    DEBUG_PRINTF("[%s] : connection %p %p\n", __func__,
                 connection_p,
                 connection_p->tmpBuf);
    
    connection_p->req_p = NHTTPi_CreateRequest(bgnEndInfo_p, url_p, method, buf_p, len, userParam_p, NULL, NULL);

    // NHTTP_CreatecondRequest���s
    if (!connection_p->req_p)
    {
        NHTTPi_free(connection_p);
        return NULL;
    }
    connection_p->res_p = connection_p->req_p->res_p;
    // �X�e�[�^�X��҂���Ԃ�
    connection_p->status = NHTTP_STATUS_IDLE;
    // callback�ݒ�
    connection_p->connectionCallback = callback;
    // postDataBuf�����ݒ�
    connection_p->postDataBuf.value_p = NULL;
    connection_p->postDataBuf.length = 0;
    
    connection_p->reqId = -1;
    // ���X�g�ɓo�^
    NHTTPi_CommitConnectionList(mutexInfo_p, connection_p);
    // �G���[��BUSY�ɐݒ�
    connection_p->nhttpError = NHTTP_ERROR_BUSY;
    // SSL�G���[�͂܂������̂�ENONE�ɐݒ�
    connection_p->sslError = NHTTP_SSL_ENONE;
    
#if 0
//noma 2006.11.08
// ���d�l�Ή�
    // soRecvSize�����ݒ�
    connection_p->soRecvSize = 0;
#endif
    
    connection_p->running = TRUE;
    
    NHTTPi_SetVirtualContentLength(connection_p, 0);
    connection_p->reqCallback = NULL;
    // �ꎞ��M�o�b�t�@�����ݒ�
    connection_p->tmpBufOffset = 0;
    connection_p->tmpBufRest = 0;
    NHTTPi_memclr(&(connection_p->tmpBuf[0]), LEN_TMP_BUF);
    
    connection_p->ref = 1;
    
    return (void*)connection_p;
}

/*---------------------------------------------------------------------------*
  Name        : NHTTP_DeleteConnection
  
  Description : �ڑ��n���h�������
  
  Arguments   : handle  - �ڑ��n���h��
  
  Returns     : �ڑ��n���h�����J���ł���� 0, �����łȂ���� -1
 *---------------------------------------------------------------------------*/
extern NHTTPError NHTTP_DeleteConnection(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);
    
    DEBUG_PRINTF("[%s]\n", __func__);

    if (connection_p == NULL)  return NHTTP_ERROR_SYSTEM;

    return NHTTPi_DeleteConnection(connection_p);
}

extern NHTTPError NHTTPi_DeleteConnection(NHTTPConnection* connection_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    
    DEBUG_PRINTF("[%s]\n", __func__);

    if (connection_p == NULL)  return NHTTP_ERROR_SYSTEM;

    // �v�����Ȃ�L�����Z��
    if (connection_p->ref > 0 && connection_p->reqId >= 0)
    {
        NHTTPi_CancelRequestAsync(sysInfo_p, connection_p->reqId);
        connection_p->reqId = -1;
    }

    if (NHTTPi_decRef(mutexInfo_p, connection_p) > 0)
    {
        return NHTTP_ERROR_NONE;
    }
    
    if (connection_p->req_p != NULL)
    {
        // ���N�G�X�g�J�n��̓��N�G�X�g�̍폜�̓X���b�h�ɔC����
        if (!connection_p->req_p->isStarted)
        {
            NHTTPi_DestroyRequest(sysInfo_p, connection_p->req_p);
        }
    }
    if (connection_p->res_p != NULL)
    {
        // ���N�G�X�g�J�n�O�@�܂��̓��N�G�X�g������
        if (connection_p->req_p != NULL)
        {
            // �҂��Ă���폜
            NHTTP_WaitForConnection(connection_p);
        }
        NHTTPi_DestroyResponse(mutexInfo_p, connection_p->res_p);
    }
    // ���X�g����폜
    (void)NHTTPi_OmitConnectionList(mutexInfo_p, connection_p);
    NHTTPi_free(connection_p);

    return NHTTP_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
  Name        : NHTTP_WaitForConnection
  
  Description : �ʐM����Ԃ̊�����҂�
  
  Arguments   : handle  - �ڑ��n���h��
  
  Returns     : 
 *---------------------------------------------------------------------------*/
extern NHTTPError NHTTP_WaitForConnection(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);

    DEBUG_PRINTF("[%s]\n", __func__);
    
    if (connection_p == NULL) return NHTTP_ERROR_SYSTEM;
    
    {
        NHTTPConnectionStatus status = NHTTP_GetConnectionStatus(handle);

        NHTTPi_CheckCurrentThread(threadInfo_p, NHTTP_CURRENT_THREAD_IS_NOT_NHTTP_THREAD);
        
        if ((status != NHTTP_STATUS_ERROR) &&
            (status != NHTTP_STATUS_IDLE) &&
            (NHTTP_GetConnectionError(handle) == NHTTP_ERROR_BUSY))
        {
            NHTTPi_WaitForCompletion(connection_p);
        }
    }
    return NHTTP_ERROR_NONE;
}

/*-------------------------------------------------------------------------*
  Name        : NHTTP_StartConnection
  
  Description : ���݂̐ݒ�Őڑ��n���h���̒ʐM���J�n����
                �J�n���ꂽ�ڑ��n���h���̓^�X�N���X�g�̏I�[�ɒǉ�����
                NHTTP���C�u���������̒ʐM�X���b�h�̊Ǘ����ɒu�����
                
  Arguments   : handle  - �ڑ��n���h��
  
  Returns     : �ʐM���J�n�ł���� 0, �����łȂ���� -1
 *-------------------------------------------------------------------------*/
extern NHTTPError NHTTP_StartConnection(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);
    
    DEBUG_PRINTF("[%s]\n", __func__);
    
    if (connection_p == NULL) return NHTTP_ERROR_SYSTEM; 
    if (connection_p->req_p == NULL) return NHTTP_ERROR_SYSTEM; 
    
    connection_p->reqId = NHTTPi_SendRequestAsync(sysInfo_p, connection_p->req_p);
    if (connection_p->reqId >= 0)
    {
        /* ���N�G�X�g�n���h���͒ʐM�J�n�ƂƂ��Ƀ��[�U�Ǘ��𗣂�� */
        connection_p->status = NHTTP_STATUS_CONNECTING;

        NHTTPi_addRef(mutexInfo_p, connection_p);

        // �����ݎQ�Ƃł��Ȃ��Ȃ�̂�NULL�����Ă͑ʖ�
        //connection_p->req_p = NULL;
    }
    
    return NHTTP_ERROR_NONE;
}

/*-------------------------------------------------------------------------*
  Name        : NHTTP_CancelConnection
  
  Description : �ڑ��n���h���̒ʐM�𒆎~����
  
  Arguments   : handle  - �ڑ��n���h��
  
  Returns     : �ʐM�����~�ł���� 0, �����łȂ���� -1
 *-------------------------------------------------------------------------*/
extern NHTTPError NHTTP_CancelConnection(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*    connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);
    
    DEBUG_PRINTF("[%s][%d]\n", __func__, __LINE__);
    
    if (connection_p == NULL)  return NHTTP_ERROR_SYSTEM; 
    
    if (connection_p->reqId >= 0)
    {
        NHTTPi_CancelRequestAsync(sysInfo_p, connection_p->reqId);
        // ���ĊJ�ł���킯�ł͂Ȃ��̂�-1�ɂ���K�v�͂Ȃ�
        //connection_p->reqId = -1;
        return NHTTP_ERROR_NONE;
    }
    else
    {
        return NHTTP_ERROR_SYSTEM;
    }
}

/*---------------------------------------------------------------------------*
  Name        : NHTTP_GetConnectionStatus

  Description : �w�肳�ꂽ�ڑ��n���h���̏�Ԃ��擾

  Arguments   : handle        - �ڑ��n���h��
 *---------------------------------------------------------------------------*/
extern NHTTPConnectionStatus NHTTP_GetConnectionStatus(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);

    if (connection_p == NULL)
    {
        return NHTTP_STATUS_ERROR;
    }
    else
    {
        return connection_p->status;
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetBodyBuffer

  Description:  HTTP�{�f�B�o�b�t�@�擪�|�C���^�擾

  Arguments:    handle  - �ڑ��n���h��
                value	- �擾���ʂ̃A�h���X���i�[���邽�߂̃|�C���^�ւ̃|�C���^

  Returns:      0�ȏ�	�擾����HTTP�{�f�B�̒���(Byte�P��)
                0����	HTTP�{�f�B���擾�ł��Ȃ�
 *-------------------------------------------------------------------------*/
extern int NHTTP_GetBodyBuffer(NHTTPConnectionHandle handle, char** value_pp, u32* size_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);
    
    if (connection_p != NULL)
    {
        NHTTPRes* res_p = NHTTPi_Connection2Response(mutexInfo_p, connection_p);
        
        if (res_p != NULL)
        {
            *value_pp = res_p->recvBuf_p;
            *size_p = res_p->recvBufLen;
            return res_p->bodyLen;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetUserParam

  Description:  ���[�U�[�p�����[�^�[���擾
  
  Arguments:    handle  - �ڑ��n���h��
  
  Returns:      �p�����[�^�̃|�C���^
 *-------------------------------------------------------------------------*/
extern void* NHTTP_GetUserParam(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);

    if (connection_p != NULL)
    {
        NHTTPRes* res_p = NHTTPi_Connection2Response(mutexInfo_p, connection_p);
        
        if (res_p != NULL)
        {
            return res_p->param_p;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetConnectionError

  Description:  �G���[���擾
  
  Arguments:    handle  - �ڑ��n���h��
  
  Returns:      �G���[�l
 *-------------------------------------------------------------------------*/
extern NHTTPError NHTTP_GetConnectionError(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);

    if (connection_p != NULL)
    {
        return connection_p->nhttpError;
    }
    else
    {
        return NHTTP_ERROR_SYSTEM;
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetConnectionSSLError

  Description:  SSL�G���[���擾
  
  Arguments:    handle  - �ڑ��n���h��
  
  Returns:      SSL�G���[�l 
 *-------------------------------------------------------------------------*/
extern int NHTTP_GetConnectionSSLError(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);
    
    if (connection_p != NULL)
    {
       return connection_p->sslError;
    }
    else
    {
        return -1;
    }
}
/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetConnectionSSLError

  Description:  �i�s�x(��M�o�C�g�������Content-Length�̒l)���擾����
  
  Arguments:    handle  - �ڑ��n���h��
  
  Returns:      HTTP���N�G�X�g�i�s�x�擾�ɐ����ł���� 0, �����łȂ���� -1
 *-------------------------------------------------------------------------*/
extern NHTTPError NHTTP_GetConnectionProgress(NHTTPConnectionHandle handle, u32* received_p, u32* contentlen_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);
    
    if (connection_p != NULL)
    {
        NHTTPRes* res_p = NHTTPi_Connection2Response(mutexInfo_p, connection_p);
        
        if (res_p != NULL)
        {
            *received_p = (u32)res_p->totalBodyLen;
//            *received = (u32)res_p->bodyLen;
            *contentlen_p = (u32)((res_p->contentLength == 0xffffffff)? 0 : res_p->contentLength);
            return NHTTP_ERROR_NONE;
        }
        else
        {
            return NHTTP_ERROR_SYSTEM;
        }
    }
    else
    {
        return NHTTP_ERROR_SYSTEM;
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_CallAlloc

  Description:  ���[�U�[�w��� Alloc�֐��Ăяo��
  
  Arguments:    size  - �m�ۗv���T�C�Y
                align - �o�b�t�@�A���C�����g
  
  Returns:      �����ł���΃|�C���^��, �����łȂ���� NULL
 *-------------------------------------------------------------------------*/
extern void* NHTTP_CallAlloc(u32 size, int align)
{
    return NHTTPi_alloc(size, align);
}
/*-------------------------------------------------------------------------*
  Name:         NHTTP_CallFree

  Description:  ���[�U�[�w��� free�֐��Ăяo��
  
  Arguments:    ptr - NHTTPCallAlloc�Ŋm�ۂ����̈�̐擪�|�C���^
  
  Returns:      
 *-------------------------------------------------------------------------*/
extern void NHTTP_CallFree(void* ptr_p)
{
    NHTTPi_free(ptr_p);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_SetSocketBufferSize

  Description:  Socket���g�p��������o�b�t�@�T�C�Y�̎w��
  
  Arguments:    size - Socket���g�p��������o�b�t�@�T�C�Y
  
  Returns:      
 *-------------------------------------------------------------------------*/
extern NHTTPError NHTTP_SetSocketBufferSize(NHTTPConnectionHandle handle, u32 size)
{
#if 1
//noma 2006.11.08
// ���d�l�Ή�
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);

    if (req_p == NULL)
    {
        return NHTTP_ERROR_SYSTEM;
    }

    req_p->soRecvSize = size;
#else
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);
   
    if (connection_p == NULL)  return NHTTP_ERROR_SYSTEM;
    
    connection_p->soRecvSize = size;
#endif
    
    return NHTTP_ERROR_NONE;
}
