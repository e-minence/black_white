/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     d_nhttp.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-08#$
  $Rev: 1183 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include "nhttp.h"
#include "NHTTPlib_int.h"
#include "NHTTPlib_int_socket.h"
#include "d_nhttp_common.h"
#include "NHTTP_control.h"
#include "NHTTP_request.h"
#include "NHTTP_response.h"
#include "NHTTP_thread.h"
#include "NHTTP_nonport.h"


#if defined(NHTTP_FOR_RVL)
/* ���C�u�����o�[�W������������g�p���邽�߂̃w�b�_�ƃ}�N�� */
#include <revolution/revodefs.h>
#include <revolution/rexdefs.h>

REVOLUTION_LIB_VERSION( NHTTP );
REX_UNOFFICIAL_FUNC_VERSION( NHTTP );

/* �����������������f�o�b�O�o�͂��Ȃ����߂̃`�F�b�N�ϐ� */
static BOOL nhttpRegistered = FALSE;
static BOOL nhttpRegisteredUnofficial = FALSE;
#endif

/*-------------------------------------------------------------------------*
  Name:         NHTTP_Startup

  Description:  NHTTP���C�u����������������
                 
  Arguments:    Alloc           NHTTP���C�u���������Ŏg�p����郁�����u���b�N�m�ۊ֐��ւ̃|�C���^
                Free            NHTTP���C�u���������Ŏg�p����郁�����u���b�N����֐��ւ̃|�C���^
                threadprio      HTTP���N�G�X�g�X���b�h�̗D��x(0����31�͈̔́A0���ō��D��x)

  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTP_Startup(NHTTPAlloc alloc, NHTTPFree free, u32 threadprio)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();

#if defined(NHTTP_FOR_RVL)
   if( !nhttpRegistered )
    {
        OSRegisterVersion( __NHTTPVersion );
        nhttpRegistered = TRUE;
    }
#endif
    
    // NHTTP_bgnend.c �Q��
    return (NHTTPi_Startup(sysInfo_p, alloc, free, threadprio) ? 0 : -1);
}
/*-------------------------------------------------------------------------*
  Name:         NHTTP_CleanupAsync

  Description:  NHTTP���C�u�����̉����v������

  Arguments:    Callback	NHTTP���C�u�����̉�����������������ۂɌĂяo�����R�[���o�b�N�֐��ւ̃|�C���^

  Returns:      �Ȃ�
 *-------------------------------------------------------------------------*/
extern void NHTTP_CleanupAsync(NHTTPCleanupCallback callback)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    // NHTTP_bgnend.c �Q��
    NHTTPi_CleanupAsync(sysInfo_p, callback);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_Cleanup

  Description:  NHTTP���C�u�����̉����v������

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *-------------------------------------------------------------------------*/
extern void NHTTP_Cleanup(void)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    // NHTTP_bgnend.c �Q��
    NHTTPi_CleanupAsync(sysInfo_p, NULL);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetError

  Description:  NHTTP���C�u�������ōŌ�ɔ��������G���[�̏󋵂��擾����

  Arguments:    �Ȃ�

  Returns:      NHTTPError	���̊֐����Ăяo�����O��NHTTP���C�u�������Ŕ��������G���[�̏�
 *-------------------------------------------------------------------------*/
extern NHTTPError NHTTP_GetError(void)
{
    // NHTTP_bgnend.c �Q��
    return NHTTPi_GetError(&(NHTTPi_GetSystemInfoP()->bgnEndInfo));
}
/*-------------------------------------------------------------------------*
  Name:         NHTTP_SSLGetError

  Description:  NHTTP���C�u�������ōŌ�ɔ�������SSL�G���[�̏󋵂��擾����

  Arguments:    �Ȃ�

  Returns:      ���̊֐����Ăяo�����O��NHTTP���C�u�������Ŕ�������SSL�G���[�̏�
 *-------------------------------------------------------------------------*/
extern int NHTTP_SSLGetError(void)
{
    NHTTPBgnEndInfo*	bgnEndInfo_p = &(NHTTPi_GetSystemInfoP()->bgnEndInfo);
    NHTTPi_ASSERT(NHTTPi_IsOpened(bgnEndInfo_p));
    
    return NHTTPi_GetSSLError(bgnEndInfo_p);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_PostSendCallbackWrap

  Description:  [���d�l�݊��p] POST�f�[�^�̑��M���X�g���[�~���O�ōs�����߂̃R�[���o�b�N���b�v�֐�

  Arguments:    connection_p	�Ώۂ̐ڑ��n���h��
                
  Returns:      
 *-------------------------------------------------------------------------*/
static int NHTTPi_PostSendCallbackWrap(NHTTPMutexInfo* mutexInfo_p, NHTTPConnection* connection_p, void* arg_p)
{
    if (connection_p != NULL)
    {
        NHTTPReq*	req_p = NHTTPi_Connection2Request(mutexInfo_p, connection_p);

        if (req_p != NULL)
        {
            NHTTPPostSend	postSend = req_p->postSendCallback;

            if (postSend != NULL)
            {
                NHTTPPostSendArg*	postSendArg_p = (NHTTPPostSendArg*)arg_p;

                return postSend(postSendArg_p->label, &(postSendArg_p->buf), &(postSendArg_p->size), (int)(postSendArg_p->offset), NHTTP_GetUserParam(connection_p));
            }
        }
    }
    return -1;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_BufFullCallbackWrap

  Description:  [���d�l�݊��p] �{�f�B��M�o�b�t�@����ꂽ�ꍇ�ɃR�[�������R�[���o�b�N���b�v�֐�

  Arguments:    connection_p	�Ώۂ̐ڑ��n���h��
                
  Returns:      
 *-------------------------------------------------------------------------*/
static int NHTTPi_BufFullCallbackWrap(NHTTPMutexInfo* mutexInfo_p, NHTTPConnection* connection_p, void* arg_p)
{
    if (connection_p != NULL)
    {
        NHTTPRes*	res_p = NHTTPi_Connection2Response(mutexInfo_p, connection_p);

        if (res_p != NULL)
        {
            NHTTPBufFull	bufFull = res_p->bufFull;

            if (bufFull != NULL)
            {
                NHTTPBodyBufArg*	bodyBufArg_p = (NHTTPBodyBufArg*)arg_p;
                char*			oldBuf_p = bodyBufArg_p->buf;
                u32			contentLength = NHTTPi_GetVirtualContentLength(connection_p);

                bodyBufArg_p->buf = bufFull(&oldBuf_p, &(bodyBufArg_p->size), contentLength, NHTTPi_alloc, NHTTPi_free, NHTTP_GetUserParam(connection_p));
                if ((bodyBufArg_p->buf != NULL) && (oldBuf_p != NULL))
                {
                    bodyBufArg_p->offset = 0;
                }
            }
        }
    }
    return 0;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_CompleteCallbackWrap

  Description:  [���d�l�݊��p] HTTP�ʐM�I�����ɌĂяo�����R�[���o�b�N���b�v�֐�

  Arguments:    connection_p	�Ώۂ̐ڑ��n���h��
                
  Returns:      
 *-------------------------------------------------------------------------*/
static int NHTTPi_CompleteCallbackWrap(NHTTPMutexInfo* mutexInfo_p, NHTTPConnection* connection_p)
{
    if (connection_p != NULL)
    {
        NHTTPReqCallback	reqCallback = connection_p->reqCallback;

        if (reqCallback != NULL)
        {
            NHTTPRes*		res_p = NHTTPi_Connection2Response(mutexInfo_p, connection_p);

            if (res_p != NULL)
            {
                NHTTPError	result = NHTTP_GetConnectionError(connection_p);

                reqCallback(result, res_p, NHTTP_GetUserParam(connection_p));
            }
        }
    }
    return 0;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_TemplateConnectionCallback

  Description:  [���d�l�݊��p]�C�x���g�ʒm�R�[���o�b�N

  Arguments:    handle	�C�x���g�ʒm�Ώۂ̐ڑ��n���h��
  		event	�C�x���g���
                arg_p	�C�x���g���ƂɈقȂ�R�[���o�b�N���\���̂ւ̃|�C���^
                
  Returns:      
 *-------------------------------------------------------------------------*/
static int NHTTPi_TemplateConnectionCallback(NHTTPConnectionHandle handle, NHTTPConnectionEvent event, void* arg_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);
    int		ret = 0;

    switch (event)
    { 
    case NHTTP_EVENT_POST_SEND:
        ret = NHTTPi_PostSendCallbackWrap(mutexInfo_p, connection_p, arg_p);
        break;
    case NHTTP_EVENT_BODY_RECV_FULL:
        ret = NHTTPi_BufFullCallbackWrap(mutexInfo_p, connection_p, arg_p);
        break;
    case NHTTP_EVENT_BODY_RECV_DONE:
        //���d�l�ł͂Ȃ�
        break;
    case NHTTP_EVENT_COMPLETE:
        ret = NHTTPi_CompleteCallbackWrap(mutexInfo_p, connection_p);
        break;
    default:
        break;
    }
    return ret;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_CreateRequest

  Description:  [���d�l�݊��p] �����Ŏw�肳�ꂽ�p�����[�^���ݒ肳�ꂽNHTTPReq�I�u�W�F�N�g�𐶐�����

  Arguments:    url_p		�ڑ���URL(NULL�I�[ASCII������)�ւ̃|�C���^
                method		���s����HTTP���N�G�X�g���\�b�h
                buf_p		HTTP�{�f�B��M�o�b�t�@�ւ̃|�C���^
                len		HTTP�{�f�B��M�o�b�t�@�̒���
                callback	HTTP�ʐM�I�����R�[���o�b�N�֐��ւ̃|�C���^
                param_p		HTTP�ʐM�I�����R�[���o�b�N�֐��ɗ^������p�����[�^
                
  Returns:      NULL            �����Ɏ��s����
                NULL�ȊO        �m�ۂ��ꂽNHTTPReq�I�u�W�F�N�g�ւ̃|�C���^
 *-------------------------------------------------------------------------*/
extern NHTTPRequestHandle NHTTP_CreateRequest(const char*      url_p,
                                              NHTTPReqMethod   method,
                                              char*            buf_p,
                                              u32              len,
                                              NHTTPReqCallback callback,
                                              void*            param_p)
{
    return NHTTP_CreateRequestEx(url_p, method, buf_p, len, callback, param_p, (NHTTPBufFull)NULL, (NHTTPFreeBuf)NULL);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_CreateRequestEx

  Description:  [���d�l�݊��p] �����Ŏw�肳�ꂽ�p�����[�^���ݒ肳�ꂽNHTTPReq�I�u�W�F�N�g�𐶐�����

  Arguments:    url_p		�ڑ���URL(NULL�I�[ASCII������)�ւ̃|�C���^
                method		���s����HTTP���N�G�X�g���\�b�h
                buf_p		HTTP�{�f�B��M�o�b�t�@�ւ̃|�C���^
                len		HTTP�{�f�B��M�o�b�t�@�̒���
                callback	HTTP�ʐM�I�����R�[���o�b�N�֐��ւ̃|�C���^
                param_p		HTTP�ʐM�I�����R�[���o�b�N�֐��ɗ^������p�����[�^
                bufFull		HTTP�{�f�B��M�o�b�t�@���R�[���o�b�N�֐��|�C���^
                
  Returns:      NULL            �����Ɏ��s����
                NULL�ȊO        �m�ۂ��ꂽNHTTPReq�I�u�W�F�N�g�ւ̃|�C���^
 *-------------------------------------------------------------------------*/
extern NHTTPRequestHandle NHTTP_CreateRequestEx(const char*      url_p,
                                                NHTTPReqMethod   method,
                                                char*            buf_p,
                                                u32              len,
                                                NHTTPReqCallback callback,
                                                void*            param_p,
                                                NHTTPBufFull     bufFull,
                                                NHTTPFreeBuf     freeBuf)
{
    NHTTPConnectionHandle	handle = NHTTP_CreateConnection(url_p, method, buf_p, len, &NHTTPi_TemplateConnectionCallback, param_p);
    NHTTPReq*			req_p = NULL;

    if (handle != NULL)
    {
        NHTTPSysInfo*		sysInfo_p = NHTTPi_GetSystemInfoP();
        NHTTPMutexInfo*		mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
        NHTTPConnection*	connection_p = NHTTPi_GetConnection(mutexInfo_p, handle);

        req_p = NHTTPi_Connection2Request(mutexInfo_p, connection_p);

        // ���d�l�p�R�[���o�b�N�֐��̍Đݒ�
        if (req_p != NULL)
        {
            if (req_p->res_p != NULL)
            {
                connection_p->reqCallback = callback;
                req_p->res_p->bufFull = bufFull;
                req_p->res_p->freeBuf = freeBuf;
                return req_p;
            }
            else
            {
                // �G���[���͊m�ۂ����R�l�N�V�������폜
                NHTTPi_DestroyRequest(sysInfo_p, req_p);
                (void)NHTTPi_OmitConnectionList(mutexInfo_p, connection_p);
                NHTTPi_free(connection_p);
                connection_p = NULL;
                req_p = NULL;
            }
        }
    }
    return (NHTTPRequestHandle)req_p;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_DeleteRequest
  
  Description:  [���d�l�݊��p] �����Ŏw�肳�ꂽNHTTPReq�I�u�W�F�N�g���g�p���Ă��郁�������������
  
  Arguments:    req	�����Ώۂ�HTTPReqHandle
  
  Returns:      �Ȃ�
 *-------------------------------------------------------------------------*/
extern void NHTTP_DeleteRequest(NHTTPRequestHandle req)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_Request2Connection(mutexInfo_p, req);

    if (connection_p != NULL)
    {
        //�ڑ��n���h���̊J��
        NHTTP_DeleteConnection(connection_p);
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_AddHeaderField

  Description:  HTTP���N�G�X�g���s���ɑ��M����HTTP�w�b�_���ڂ�ǉ�����

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                label	�ǉ�����HTTP�w�b�_���ڂ̃��x��(NULL�I�[ASCII������)�ւ̃|�C���^
                value	�ǉ�����HTTP�w�b�_���ڂ̒l(NULL�I�[ASCII������)�ւ̃|�C���^

  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTP_AddHeaderField(NHTTPConnectionHandle handle, const char* label_p, const char* value_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    
    return NHTTP_AddRequestHeaderField(req_p, label_p, value_p);
}
/*-------------------------------------------------------------------------*
  Name:         NHTTP_AddRequestHeaderField

  Description:  [���d�l�݊��p] HTTP���N�G�X�g���s���ɑ��M����HTTP�w�b�_���ڂ�ǉ�����
 *-------------------------------------------------------------------------*/
extern int NHTTP_AddRequestHeaderField(NHTTPReq* req_p, const char* label_p, const char* value_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    
    if (req_p == NULL) return -1;
    
    // �G���[���̊֐�����\�����邽�ߔ����NHTTP_AddHeaderField���炱����Ɉړ�
    if (req_p->isStarted)
    {
        PRINTF("%s can be called before NHTTPStartConnection()\n", __func__);
        return -1;
    }

    // NHTTP_control.c �Q��
    return (NHTTPi_AddRequestHeaderField(req_p, bgnEndInfo_p, label_p, value_p) ? 0 : -1);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_AddPostDataAscii

  Description:  HTTP���N�G�X�g���s���ɑ��M����POST����(ASCII������)��ǉ�����

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                label	�ǉ�����POST���ڂ̃��x��(NULL�I�[ASCII������)�ւ̃|�C���^
                value	�ǉ�����POST���ڂ̒l(NULL�I�[ASCII������)�ւ̃|�C���^

  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTP_AddPostDataAscii(NHTTPConnectionHandle handle, const char* label_p, const char* value_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    
    return (NHTTPi_AddRequestPostDataAscii(req_p, bgnEndInfo_p, label_p, value_p) ? 0 : -1);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_AddRequestPostDataAscii

  Description:  [���d�l�݊��p]HTTP���N�G�X�g���s���ɑ��M����POST����(ASCII������)��ǉ�����
 *-------------------------------------------------------------------------*/
extern int NHTTP_AddRequestPostDataAscii(NHTTPReq* req_p, const char* label_p, const char* value_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    
    if (req_p == NULL) return -1;
    
    // �G���[���̊֐�����\�����邽�ߔ����NHTTP_AddPostDataAscii���炱����Ɉړ�
    if (req_p->isStarted)
    {
        PRINTF("%s can be called before NHTTPStartConnection()\n", __func__);
        return -1;
    }

    // NHTTP_control.c �Q��
    return (NHTTPi_AddRequestPostDataAscii(req_p, bgnEndInfo_p, label_p, value_p) ? 0 : -1);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPAddPostDataBinary

  Description:  HTTP���N�G�X�g���s���ɑ��M����POST����(�o�C�i���f�[�^)��ǉ�����

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                label	�ǉ�����POST���ڂ̃��x��(NULL�I�[ASCII������)�ւ̃|�C���^
                value	�ǉ�����POST���ڂ̒l(�o�C�i���f�[�^)�ւ̃|�C���^
                length	�ǉ�����POST���ڂ̒l�̒���(Byte�P��)

  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTP_AddPostDataBinary(NHTTPConnectionHandle handle, const char* label_p, const void* value_p, u32 length)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    return ( 0 == NHTTP_AddRequestPostDataBinary(req_p, label_p, (const char*)value_p, length) ? 0 : -1);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_AddRequestPostDataBinary

  Description:  [���d�l�݊��p] HTTP���N�G�X�g���s���ɑ��M����POST����(�o�C�i���f�[�^)��ǉ�����
 *-------------------------------------------------------------------------*/
extern int NHTTP_AddRequestPostDataBinary(NHTTPReq* req_p, const char* label_p, const void* value_p, u32 length)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    
    if (req_p == NULL) return -1;
        
    // �G���[���̊֐�����\�����邽�ߔ����NHTTP_AddPostDataBinary���炱����Ɉړ�
    if (req_p->isStarted)
    {
        PRINTF("%s can be called before NHTTPStartConnection()\n", __func__);
        return -1;
    }

    // NHTTP_control.c �Q��
    return (NHTTPi_AddRequestPostDataBinary(req_p, bgnEndInfo_p, label_p, (const char*)value_p, length) ? 0 : -1);
    
}

// 2006/09/28 noma
/*-------------------------------------------------------------------------*
  Name:         NHTTP_AddPostDataRaw

  Description:  HTTP���N�G�X�g���s����POST�Ő��̃f�[�^�𑗐M����

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                value_p	�ǉ�����POST���ڂ̒l(�o�C�i���f�[�^)�ւ̃|�C���^
                length	�ǉ�����POST���ڂ̒l�̒���(Byte�P��)

  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTP_AddPostDataRaw(NHTTPConnectionHandle handle, const void* value_p, u32 length)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    
    if (req_p == NULL) return -1;
    
    if (req_p->isStarted)
    {
        PRINTF("%s can be called before NHTTPStartConnection()\n", __func__);
        return -1;
    }
    
    // noma 2006.09.28
    // ���� NHTTPAddPostDataBinary �� NHTTPAddPostDataAscii ���Ă΂�Ă���
    if (req_p->listPost_p != NULL) {
        PRINTF("already called NHTTPAddPostDataAscii or NHTTPAddPostDataBinary (exclusive fucntion)\n");
        return -1;
    }
    
    req_p->isRawData = TRUE;
    req_p->rawPostData_p = value_p;
    req_p->rawPostDataLen = length;
    return 0;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_SetRequestPostDataCallback

  Description:  HTTP���N�G�X�g���s���ɁA
                POST�Ńf�[�^�𑗐M����ۂɌĂ΂��R�[���o�b�N���Z�b�g����

  Arguments:    req             �����Ώۂ�HTTPReq�I�u�W�F�N�g�ւ̃|�C���^
                callback        �f�[�^���M���ɌĂ΂��R�[���o�b�N�֐��ւ̃|�C���^
                
  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTP_SetRequestPostDataCallback(NHTTPRequestHandle req, NHTTPPostSend callback)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_Request2Connection(mutexInfo_p, req);

    if (connection_p == NULL) return -1;

    {
        NHTTPReq*	req_p = NHTTPi_GetRequest(mutexInfo_p, connection_p);

        if (req_p == NULL) return -1;
        if (req_p->isStarted) return -1;

        req_p->postSendCallback = callback;
    }
    return 0;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_SetPostDataEncoding

  Description:  �����������N�G�X�g��enctype�𖾎��I�Ɏw�肷��B

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                type	�w�肵�������N�G�X�g��enctype
                NHTTP_ENCODING_TYPE_AUTO,      ��������(�f�t�H���g) 
                NHTTP_ENCODING_TYPE_URL,       application/x-www-form-urlencoded 
                NHTTP_ENCODING_TYPE_MULTIPART  multipart/form-data 

  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTP_SetPostDataEncoding(NHTTPConnectionHandle handle, NHTTPEncodingType type)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    if (req_p == NULL) return -1;
    if (req_p->isStarted)
    {
        PRINTF("%s can be called before NHTTPStartConnection()\n", __func__);
        return -1;
    }
        
#if 1 // noma 2006.09.28
    // ���� NHTTPAddPostDataRaw ���Ă΂�Ă���
    if (req_p->isRawData)   return -1;
#endif
    
    req_p->encodingType = type;   
    return 0;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_SendRequestAsync

  Description:  �K�؂ɐݒ肳�ꂽNHTTPReq�I�u�W�F�N�g�ɏ]����HTTP���N�G�X�g�����s����

  Arguments:    req	�����Ώۂ�HTTPReq�I�u�W�F�N�g�ւ̃|�C���^
  
  Returns:      0�ȏ�	�����ɐ������� (�l�͎��s����HTTP���N�G�X�g�ɗ^������ID)
                0����	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTP_SendRequestAsync(NHTTPRequestHandle req)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_Request2Connection(mutexInfo_p, req);

    if (connection_p != NULL)
    {
        if (NHTTP_StartConnection(connection_p) == NHTTP_ERROR_NONE)
        {
            return connection_p->reqId;
        }
    }
    return -1;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_CancelRequestAsync

  Description:  NHTTPSendRequest�֐����瓾��ꂽID���w�肵�AHTTP���N�G�X�g�̒�~��v������

  Arguments:    id	���f���郊�N�G�X�g��id
  
  Returns:      0 	�����ɐ��������A���邢��ID�Ŏw�肳�ꂽHTTP���N�G�X�g�����݂��Ȃ�
                -1	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTP_CancelRequestAsync(int id)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();

    DEBUG_PRINTF("[%s] %d\n", __func__, id);
    
    // NHTTP_request.c �Q��
    return (NHTTPi_CancelRequestAsync(sysInfo_p, id) ? 0 : -1);
}

/*---------------------------------------------------------------------------*
  Name:         NHTTP_GetProgress

  Description:  ���ݐi�s����HTTP���N�G�X�g�����̐i�s�x(��M�o�C�g�������Content-Length�̒l)���擾����

  Arguments:    received_p	��M�ς�HTTP BODY�̈�T�C�Y���i�[����̈�ւ̃|�C���^
                contentlen_p	Content-Length���i�[����̈�ւ̃|�C���^

  Returns:      0 	HTTP���N�G�X�g�i�s�x�擾�ɐ�������
                -1	HTTP���N�G�X�g�L���[����ŁA����HTTP���N�G�X�g�����s����Ă��Ȃ�
 *---------------------------------------------------------------------------*/
extern int NHTTP_GetProgress(u32* received_p, u32* contentlen_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    // NHTTP_thread.c �Q��
    return (NHTTPi_GetProgress(sysInfo_p, received_p, contentlen_p) ? 0 : -1);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_DestroyResponse

  Description:  HTTP���X�|���X�I�u�W�F�N�g�̎g�p���郁������S�ĉ������

  Arguments:    res	�����Ώۂ�NHTTPResponseHandle
        
  Returns:      �Ȃ�
 *-------------------------------------------------------------------------*/
extern void NHTTP_DestroyResponse(NHTTPResponseHandle res)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_Response2Connection(mutexInfo_p, res);

    DEBUG_PRINTF("[%s]\n", __func__);
    
    if (connection_p != NULL)
    {
        NHTTPRes*	res_p = NHTTPi_Connection2Response(mutexInfo_p, connection_p);

        if (res_p != NULL)
        {
            NHTTPi_ASSERT(connection_p->req_p == NULL);
            NHTTPi_DestroyResponse(mutexInfo_p, connection_p->res_p);
        }
        // ���X�g����폜
         DEBUG_PRINTF("[%s] NHTTPi_OmitConnectionList\n", __func__);
         (void)NHTTPi_OmitConnectionList(mutexInfo_p, connection_p);
        NHTTPi_free(connection_p);
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetHeaderField

  Description:  HTTP�w�b�_�̏����A���x�������w�肵�Ď擾����

  Arguments:    res	�����Ώۂ�NHTTPResponseHandle
                label	�擾�����݂�w�b�_���̃��x����
                value	�擾���ʂ̃A�h���X���i�[���邽�߂̃|�C���^�ւ̃|�C���^

  Returns:      0�ȏ�	�擾����HTTP�w�b�_���̕�����
                0����	�w�肵�����x����HTTP�w�b�_���ɑ��݂��Ȃ�
 *-------------------------------------------------------------------------*/
extern int NHTTP_GetHeaderField(NHTTPConnectionHandle handle, const char* label_p, char** value_pp)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPRes*		res_p = NHTTPi_GetResponse(mutexInfo_p, handle);
    if (res_p == NULL) return -1;

    // NHTTP_response.c �Q��
    return NHTTP_GetResponseHeaderField(res_p, label_p, value_pp);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetHeaderAll

  Description:  HTTP�w�b�_�S�̂��擾����

  Arguments:    res	�����Ώۂ�NHTTPResponseHandle
                value	�擾���ʂ̃A�h���X���i�[���邽�߂̃|�C���^�ւ̃|�C���^

  Returns:      0�ȏ�	�擾����HTTP�w�b�_�̒���(Byte�P��)
                0����	HTTP�w�b�_���擾�ł��Ȃ�
 *-------------------------------------------------------------------------*/
extern int NHTTP_GetHeaderAll(NHTTPConnectionHandle handle, char** value_pp)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPRes*		res_p = NHTTPi_GetResponse(mutexInfo_p, handle);
    
    if (res_p == NULL) return -1;
    // NHTTP_response.c �Q��
    return NHTTPi_GetResponseHeaderAll(bgnEndInfo_p, res_p, value_pp);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetBodyAll

  Description:  HTTP�{�f�B�S�̂��擾����

  Arguments:    res	�����Ώۂ�NHTTPResponseHandle
                value	�擾���ʂ̃A�h���X���i�[���邽�߂̃|�C���^�ւ̃|�C���^

  Returns:      0�ȏ�	�擾����HTTP�{�f�B�̒���(Byte�P��)
                0����	HTTP�{�f�B���擾�ł��Ȃ�
 *-------------------------------------------------------------------------*/
extern int NHTTP_GetBodyAll(NHTTPResponseHandle res, char** value_pp)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPConnection*	connection_p = NHTTPi_Response2Connection(mutexInfo_p, res);

    if (connection_p != NULL)
    {
        u32	size = 0;

        return NHTTP_GetBodyBuffer(connection_p, value_pp, &size);
    }
    else
    {
        return -1;
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTP_GetResultCode

  Description:  HTTP���U���g���擾����

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
  
  Returns:      0�ȏ�	�擾����HTTP���U���g
                0����	HTTP���U���g���擾�ł��Ȃ�
 *-------------------------------------------------------------------------*/
extern int NHTTP_GetResultCode(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPRes*		res_p = NHTTPi_GetResponse(mutexInfo_p, handle);
    if (res_p == NULL) return -1;
    
    NHTTPi_ASSERT(NHTTPi_IsOpened(NHTTPi_GetBgnEndInfoP(NHTTPi_GetSystemInfoP())));
    
    if (!res_p->isHeaderParse) return -1;
    
    return res_p->httpStatus;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTP_SetVerifyOption

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                verifyOption
                 ���L�̃r�b�g�𗧂Ă邱�ƂŌ��ؓ��e���w��\ 
                 SSL_VERIFY_COMMON_NAME  �c �ؖ�����commonname��i_serverName�ƈ�v���邩���ׂ�
                                            �ؖ�����commonname�̓��C���h�J�[�h���T�|�[�g���Ă���
                 SSL_VERIFY_ROOT_CA      �c �ؖ�����ROOTCA�Ō��؂���
                 SSL_VERIFY_CHAIN        �c �ؖ����`�F�C�����������������؂���
                 SSL_VERIFY_DATE         �c �ؖ����̊����؂����������
                 
  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetVerifyOption(NHTTPConnectionHandle handle, u32 verifyOption)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    
    if (req_p == NULL) return -1;
    
    req_p->verifyOption = verifyOption;
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTP_SetProxy

  Description:  �w�肵��NHTTPReq�I�u�W�F�N�g�ɑ΂���proxy�T�[�o�[��ݒ肵�Ďg�p����

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                proxy   proxy�T�[�o�[ �A�h���X
                port    �|�[�g                
                
  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetProxy(NHTTPConnectionHandle handle, const char* proxy_p, int port, const char* username_p, const char* password_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    int			proxyLen = 0;
        
    if ((req_p == NULL) || (proxy_p == NULL))
    {
        return -1;
    }
    
    proxyLen = (int)NHTTPi_strnlen(proxy_p, LEN_PROXY_ADDR);
    if (proxyLen >= LEN_PROXY_ADDR)
    {
        PRINTF("proxy-address exceeded 255 characters\n");
        return -1;
    }
    NHTTPi_memclr(req_p->proxyAddr, LEN_PROXY_ADDR);
    NHTTPi_memcpy(req_p->proxyAddr, proxy_p, proxyLen);
    req_p->proxyPort = port;
    
    // ���[�U�[��
    if ((username_p != NULL) && (password_p != NULL))
    {
        int username_len = 0;
        int password_len = 0;
        username_len = (int)NHTTPi_strnlen(username_p, LEN_USERNAME);
        password_len = (int)NHTTPi_strnlen(password_p, LEN_PASSWORD);
        
        if (username_len >= LEN_USERNAME) {
            PRINTF("username exceeded 31 characters\n");
            return -1;
        }
        
        if (password_len >= LEN_PASSWORD) {
            PRINTF("password exceeded 31 characters\n");
            return -1;
        }
        
        {
            char tmpbuf[ LEN_USERNAME + LEN_PASSWORD + 1];
            NHTTPi_memclr(tmpbuf, sizeof(tmpbuf));
            NHTTPi_memcpy(tmpbuf, username_p, username_len);
            NHTTPi_memcpy(&(tmpbuf[username_len]), ":", 1);
            NHTTPi_memcpy(&(tmpbuf[(username_len+1)]), password_p, password_len);
            req_p->proxyUsernamePasswordLen = NHTTPi_Base64Encode(req_p->proxyUsernamePassword, tmpbuf);
        }
    }
    // �v���L�V�g�p
    req_p->isProxy = TRUE;
    
    PRINTF("Using proxy server %s:%d (%s/%s).\n",
           proxy_p, port,
           username_p ? username_p : "[no-auth]",
           password_p ? password_p : "[no-auth]");
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTP_SetProxyDefault

  Description:  �w�肵��NHTTPReq�I�u�W�F�N�g�ɑ΂��ăf�t�H���g��proxy�T�[�o�[��ݒ肵�Ďg�p����

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
  
  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetProxyDefault(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    int			err = NHTTP_ERROR_NONE;
    
#if defined(NHTTP_FOR_RVL)
    const NCDProxyServerProfile*	proxyServerProfile = NULL;

    // SSL�H
    if (req_p->isSSL)
    {
        proxyServerProfile = &(bgnEndInfo_p->ipConfig.proxy.ssl);
    }
    else
    {
        proxyServerProfile = &(bgnEndInfo_p->ipConfig.proxy.http);
    }

    // �v���L�V�ݒ肪���݂���
    if (proxyServerProfile->mode == NCD_PROXY_MODE_NORMAL)
    {
        const char* username_p = NULL, *password_p = NULL;
        if (proxyServerProfile->authType == NCD_PROXY_AUTHTYPE_BASIC)
        {
            username_p = (const char*)proxyServerProfile->username;
            password_p = (const char*)proxyServerProfile->password;
        }
        
        err = NHTTPSetProxy(req_p, (char*)proxyServerProfile->server,
                            proxyServerProfile->port,
                            username_p, password_p);
    }
#elif defined(NHTTP_FOR_NTR)
    DWCProxySetting proxySetting;
    if (DWC_GetProxySetting( &proxySetting ))
    {
        const char* username_p = NULL, *password_p = NULL;
        if (proxySetting.authType == DWC_PROXY_AUTHTYPE_BASIC)
        {
            username_p = (char*)proxySetting.authId;
            password_p = (char*)proxySetting.authPass;
        }
        err = NHTTP_SetProxy(req_p, (char*)proxySetting.hostName,
                             proxySetting.port,
                             username_p, password_p);
    }
#else
    err = NHTTP_ERROR_UNKNOWN;
#endif    
    if (err < NHTTP_ERROR_NONE)
    {
        PRINTF("NHTTPSetProxy failed.(%d)\n", err);
        return -1;
    }
    else
    {
        // �Z�b�g����
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         NHTTP_SetBasicAuthorization

  Description:  �w�肵��NHTTPReq�I�u�W�F�N�g�ɑ΂���Basic�F�؂̃��[�U�[���ƃp�X���[�h��ݒ肷��

  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                username ���[�U�[��
                password �p�X���[�h
  
  Returns:      0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetBasicAuthorization(NHTTPConnectionHandle handle, const char* username_p, const char* password_p)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);

    if ((req_p == NULL) || (username_p == NULL) || (password_p == NULL))
    {
        return -1;
    }
    
    // ���[�U�[��
    {
        int username_len = 0;
        int password_len = 0;
        
        username_len = (int)NHTTPi_strnlen(username_p, LEN_USERNAME + 1);
        password_len = (int)NHTTPi_strnlen(password_p, LEN_PASSWORD + 1);
        if ((username_len <= LEN_USERNAME) && (password_len <= LEN_PASSWORD))
        {
            char tmpbuf[ LEN_USERNAME + LEN_PASSWORD + 1];
            
            NHTTPi_memclr(tmpbuf, LEN_USERNAME + LEN_PASSWORD + 1);
            NHTTPi_memcpy(tmpbuf, username_p, username_len);
            NHTTPi_memcpy(&(tmpbuf[username_len]), ":", 1);
            NHTTPi_memcpy(&(tmpbuf[(username_len+1)]), password_p, password_len);
            req_p->basicUsernamePasswordLen = NHTTPi_Base64Encode(req_p->basicUsernamePassword, tmpbuf);
        }
        else
        {
            return -1;
        }
    }
    
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:        NHTTP_SetClientCert
  
  Description:  ASN.1�t�H�[�}�b�g�̃N���C�A���g�ؖ����Ɣ閧�����g�p����
    
  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
  
  Returns:       0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetClientCert(NHTTPConnectionHandle handle, const char* clientCertData_p, size_t clientCertSize, const char* privateKeyData_p, size_t privateKeySize)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);

    if (req_p == NULL)
    {
        return -1;
    }

    req_p->useClientCertDefault = FALSE;
    req_p->clientCertData_p = clientCertData_p;
    req_p->clientCertSize = clientCertSize;
    req_p->privateKeyData_p = privateKeyData_p;
    req_p->privateKeySize = privateKeySize;
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:        NHTTP_RemoveClientCert
  
  Description:  �N���C�A���g�ؖ����Ɣ閧���𖳌��ɂ���
  
  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
  
  Returns:       0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_RemoveClientCert(NHTTPConnectionHandle handle)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);

    if (req_p == NULL)
    {
        return -1;
    }

    req_p->useClientCertDefault = FALSE;
    req_p->clientCertData_p = NULL;
    req_p->clientCertSize = 0;
    req_p->privateKeyData_p = NULL;
    req_p->privateKeySize = 0;
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:        NHTTP_SetRootCA
  
  Description:  ASN.1�t�H�[�}�b�g��rootCA���g�p����
  
  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                rootCAData      �c ASN.1�`���̃f�[�^
                rootCASize      �c �f�[�^�T�C�Y 
  
  Returns:       0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetRootCA(NHTTPConnectionHandle handle, const char* rootCAData_p, size_t rootCASize)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    
    return NHTTP_SetRequestRootCA(req_p, rootCAData_p, rootCASize);
}

extern int NHTTP_SetRequestRootCA(NHTTPReq* req_p, const char* rootCAData_p, size_t rootCASize)
{
    if (req_p == NULL)
    {
        return -1;
    }

    req_p->useBuiltinRootCA = FALSE;
    req_p->rootCAId = 0;
    req_p->rootCAData_p = rootCAData_p;
    req_p->rootCASize = rootCASize;
    
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         NHTTP_DisableVerifyOptionForDebug
  
  Description:  �ؖ����̌��ؓ��e���ꕔ�����ɂ���B
                �f�o�b�O�p�r�̂݁B���̊֐��������N������Ԃ� ROM ��o�͂ł��Ȃ��B
  
  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
                verifyOption
                 ���L�̃r�b�g�𗧂Ă邱�ƂŖ����ɂ��錟�ؓ��e���w��\ 
                 SSL_VERIFY_COMMON_NAME  �c �ؖ�����commonname��i_serverName�ƈ�v���邩���ׂ�
                                            �ؖ�����commonname�̓��C���h�J�[�h���T�|�[�g���Ă���
                 SSL_VERIFY_ROOT_CA      �c �ؖ�����ROOTCA�Ō��؂���
                 SSL_VERIFY_CHAIN        �c �ؖ����`�F�C�����������������؂���
                 SSL_VERIFY_DATE         �c �ؖ����̊����؂����������
                 SSL_VERIFY_SUBJECT_ALT_NAME �c commonname�̌��؂��s���ۂɁA�ؖ�����subjectAltName��
                                            �L�q������ꍇ�͂������D�悵�Ďg�p����

  Returns:        0     �����ɐ�������
                 -1     �����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_DisableVerifyOptionForDebug(NHTTPConnectionHandle handle, u32 verifyOption)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    
    // �G���[���̊֐�����\�����邽�ߔ����NHTTP_AddHeaderField���炱����Ɉړ�
    if (req_p->isStarted)
    {
        PRINTF("%s can be called before NHTTPStartConnection()\n", __func__);
        return -1;
    }
    
#if defined(NHTTP_FOR_RVL)
    if( !nhttpRegisteredUnofficial )
    {
        OSRegisterVersion( __NHTTPUnofficialVersion );
        nhttpRegisteredUnofficial = TRUE;
    }
#endif
    
    // SSLDisableVerifyOptionForDebug�Z�b�g
    bgnEndInfo_p->forDebugCallback = NHTTPi_DisableVerifyOptionForDebug;
    
    if (req_p == NULL) return -1;
    
    req_p->disableVerifyOption = verifyOption;
    
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTP_SetBuiltinRootCA
  
  Description:  �w�肵��SSL�ڑ��őg�ݍ��݂�rootCA���g�p����
  
  Arguments:    handle          �c �����Ώۂ� NHTTPConnectionHandle
                rootCAId        �c �g�p����g�ݍ���rootCA ID
  
  Returns:       0      �����ɐ�������
                 -1     �����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetBuiltinRootCA(NHTTPConnectionHandle handle, u32 rootCAId)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);

    if (req_p == NULL)
    {
        return -1;
    }   
    
    req_p->useBuiltinRootCA = TRUE;
    req_p->rootCAId = rootCAId;
    req_p->rootCAData_p = NULL;
    req_p->rootCASize = 0;
    return 0;    
}

/*---------------------------------------------------------------------------*
  Name:         NHTTP_SetBuiltinClientCert
  
  Description:  �w�肵��SSL�ڑ��őg�ݍ��݂̃N���C�A���g�ؖ����Ɣ閧�����g�p����
  
  Arguments:    handle          �c �����Ώۂ� NHTTPConnectionHandle
                clientCertId    �c �g�p����g�ݍ��݃N���C�A���g�ؖ���ID
               
  Returns:       0      �����ɐ�������
                 -1     �����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetBuiltinClientCert(NHTTPConnectionHandle handle, u32 clientCertId)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPReq*		req_p = NHTTPi_GetRequest(mutexInfo_p, handle);

    if (req_p == NULL)
    {
        return -1;
    }
    
    req_p->clientCertId = clientCertId;
    req_p->useClientCertDefault = TRUE;
    req_p->clientCertData_p = NULL;
    req_p->clientCertSize = 0;
    req_p->privateKeyData_p = NULL;
    req_p->privateKeySize = 0;
    return 0;
}

#ifdef NHTTP_USE_NSSL
/*---------------------------------------------------------------------------*
  Name:        NHTTP_SetRootCADefault
  
  Description:  �f�t�H���g��rootCA���g�p����
  
  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
  
  Returns:       0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetRootCADefault(NHTTPConnectionHandle handle)
{
    return NHTTP_SetBuiltinRootCA(handle, NHTTP_SSL_ROOTCA_DEFAULT);
}

/*---------------------------------------------------------------------------*
  Name:        NHTTPSetClientCertDefault
  
  Description:  �f�t�H���g�̃N���C�A���g�ؖ����Ɣ閧�����g�p����
  
  Arguments:    handle	�����Ώۂ� NHTTPConnectionHandle
  
  Returns:       0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetClientCertDefault(NHTTPConnectionHandle handle)
{
    return NHTTP_SetBuiltinClientCert(handle, NHTTP_SSL_CLIENTCERT_DEFAULT);
}
#endif

/*---------------------------------------------------------------------------*
  Name:        NHTTP_SetBuiltinRootCAAsDefault
  
  Description:  �f�t�H���g��rootCA���g�p����
  
  Arguments:    rootCAId        �c �g�p����g�ݍ���rootCA ID
  
  Returns:       0 	�����ɐ�������
                -1	�����Ɏ��s����
 *---------------------------------------------------------------------------*/
extern int NHTTP_SetBuiltinRootCAAsDefault(u32 rootCAId)
{
    NHTTPSysInfo*	sysInfo_p = NHTTPi_GetSystemInfoP();
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    bgnEndInfo_p->defaultRootCAId = rootCAId;
    return 0;
}

