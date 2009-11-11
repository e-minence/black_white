/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     NHTTP_bgnend.c

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
#include "NHTTP_bgnend.h"
#include "NHTTP_request.h"
#include "NHTTP_nonport.h"
#include "NHTTP_list.h"


/*-------------------------------------------------------------------------*

  Name:         NHTTPi_InitBgnEndInfo

  Description:  NHTTPBgnEndInfo�\���̂̏����ݒ�
                 
  Arguments:    bgnEndInfo_p  NHTTPBgnEndInfo�\���̂ւ̃|�C���^

  Returns:      
 *-------------------------------------------------------------------------*/
extern void NHTTPi_InitBgnEndInfo(NHTTPBgnEndInfo* bgnEndInfo_p)
{
    bgnEndInfo_p->alloc = NULL;
    bgnEndInfo_p->free = NULL;
    bgnEndInfo_p->isOpened = FALSE;
    DEBUG_PRINTF("[%s][%d] INVALID_SOCKET \n", __func__, __LINE__);
    bgnEndInfo_p->socket = INVALID_SOCKET;
    NHTTPi_SetSSLError(bgnEndInfo_p, NHTTP_SSL_ENONE);
    NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_NONE);
    bgnEndInfo_p->isThreadEnd = FALSE;
    bgnEndInfo_p->commThreadStack = NULL;
    bgnEndInfo_p->forDebugCallback = NULL;
}

/*-------------------------------------------------------------------------*

  Name:         NHTTPi_alloc

  Description:	NHTTP���C�u���������Ŏg�p����郁�����u���b�N�m�ۊ֐�
                 
  Arguments:	size	�m�ۃT�C�Y
  		align	�̈�̃A���C�����g

  Returns:	void*	�̈�̊m�ۂɐ��������ꍇ�擪�|�C���^��Ԃ�
  			�̈�̊m�ۂɎ��s�����ꍇ�� NULL��Ԃ�
 *-------------------------------------------------------------------------*/
extern void* NHTTPi_alloc(u32 size, int align)
{
    NHTTPAlloc	alloc = NHTTPi_GetSystemInfoP()->bgnEndInfo.alloc;

    if (alloc != NULL)
    {
        return alloc(size, align);
    }
    else
    {
        return NULL;
    }
}

/*-------------------------------------------------------------------------*

  Name:         NHTTPi_free

  Description:	NHTTP���C�u���������Ŏg�p����郁�����u���b�N����֐�
                 
  Arguments:	ptr_p	NHTTPi_alloc�Ŋm�ۂ��ꂽ�������u���b�N�̐擪�|�C���^

  Returns:
 *-------------------------------------------------------------------------*/
extern void NHTTPi_free(void* ptr_p)
{
    NHTTPFree	free = NHTTPi_GetSystemInfoP()->bgnEndInfo.free;

    if (free != NULL)
    {
        free(ptr_p);
    }
}

/*-------------------------------------------------------------------------*

  Name:         NHTTPi_IsOpened

  Description:	NHTTP�X���b�h����������Ă��邩�`�F�b�N����֐�
                 
  Arguments:	bgnEndInfo_p	NHTTPBgnEndInfo�\���̂ւ̃|�C���^

  Returns:	TRUE	�����ς�
  		FALSE	������
 *-------------------------------------------------------------------------*/
extern BOOL NHTTPi_IsOpened(NHTTPBgnEndInfo* bgnEndInfo_p)
{
    return (bgnEndInfo_p->isOpened);
}

/*-------------------------------------------------------------------------*

  Name:         NHTTPi_SetError

  Description:	NHTTP���C�u�������ōŌ�ɔ��������G���[�Z�b�g�֐�
                 
  Arguments:	bgnEndInfo_p	NHTTPBgnEndInfo�\���̂ւ̃|�C���^

  Returns:	TRUE	�����ς�
  		FALSE	������
 *-------------------------------------------------------------------------*/
extern void NHTTPi_SetError(NHTTPBgnEndInfo* bgnEndInfo_p, NHTTPError error)
{
    bgnEndInfo_p->error = error;
}

/*-------------------------------------------------------------------------*

  Name:         NHTTPi_SetSSLError

  Description:	NHTTP���C�u�������ōŌ�ɔ������� SSL�G���[�Z�b�g�֐�
                 
  Arguments:	o_bgnEndInfo_p	NHTTPBgnEndInfo�\���̂ւ̃|�C���^

  Returns:	TRUE	�����ς�
  		FALSE	������
 *-------------------------------------------------------------------------*/
extern void NHTTPi_SetSSLError(NHTTPBgnEndInfo* bgnEndInfo_p, int sslError)
{
    bgnEndInfo_p->sslError = sslError;
}

/*-------------------------------------------------------------------------*

  Name:         NHTTPi_GetSSLError

  Description:	NHTTP���C�u�������ōŌ�ɔ������� SSL�G���[�擾�֐�
                 
  Arguments:	bgnEndInfo_p	NHTTPBgnEndInfo�\���̂ւ̃|�C���^

  Returns:	TRUE	�����ς�
  		FALSE	������
 *-------------------------------------------------------------------------*/
extern int NHTTPi_GetSSLError(NHTTPBgnEndInfo* bgnEndInfo_p)
{
    return (bgnEndInfo_p->sslError);
}

/*-------------------------------------------------------------------------*

  Name:         NHTTPi_Startup

  Description:  NHTTP���C�u����������������
                 
  Arguments:	sysInfo_p	NHTTP�V�X�e�����|�C���^
                alloc		NHTTP���C�u���������Ŏg�p����郁�����u���b�N�m�ۊ֐��ւ̃|�C���^
                free		NHTTP���C�u���������Ŏg�p����郁�����u���b�N����֐��ւ̃|�C���^
                threadprio	HTTP���N�G�X�g�X���b�h�̗D��x(0����31�͈̔́A0���ō��D��x)

  Returns:      TRUE	�����ɐ�������
                FALSE	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern BOOL NHTTPi_Startup(NHTTPSysInfo* sysInfo_p, NHTTPAlloc alloc, NHTTPFree free, u32 threadprio)
{
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPListInfo*	listInfo_p = NHTTPi_GetListInfoP(sysInfo_p);
    NHTTPReqInfo*	reqInfo_p = NHTTPi_GetReqInfoP(sysInfo_p);
    NHTTPMutexInfo*	mutexInfo_p = NHTTPi_GetMutexInfoP(sysInfo_p);
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
        
    NHTTPi_ASSERT(!bgnEndInfo_p->isOpened);
    NHTTPi_ASSERT(alloc);
    NHTTPi_ASSERT(free);
    NHTTPi_ASSERT((threadprio >= 0) && (threadprio <= 31));
    
    // �O���[�o���ϐ���������
    bgnEndInfo_p->alloc = alloc;
    bgnEndInfo_p->free = free;
    bgnEndInfo_p->error = NHTTP_ERROR_NONE;
    bgnEndInfo_p->sslError = NHTTP_SSL_ENONE;
    bgnEndInfo_p->isThreadEnd = FALSE;
    bgnEndInfo_p->defaultRootCAId = 0;

    NHTTPi_InitListInfo(listInfo_p);
    NHTTPi_InitRequestInfo(reqInfo_p);
    NHTTPi_initLockReqList(mutexInfo_p);

    // �P�����ڑ��n���h�����X�g�̏�����
    NHTTPi_InitConnectionList();
    
    DEBUG_PRINTF("[%s][%d] INVALID_SOCKET \n", __func__, __LINE__);
    bgnEndInfo_p->socket = INVALID_SOCKET;
    
    // �X���b�h�p�Ƀ�������\�񂷂�
    bgnEndInfo_p->commThreadStack = (u32*)NHTTPi_alloc(NHTTP_COMMTHREAD_STACK, 8);
    if (!bgnEndInfo_p->commThreadStack)
    {
        DEBUG_PRINTF("[%s][line:%d] NHTTP_ERROR_ALLOC\n", __func__, __LINE__);
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_ALLOC);
        NHTTPi_exitLockReqList();
        return FALSE;
    }
    
    // �X���b�h�𐶐�
    if (NHTTPi_createCommThread(threadInfo_p, threadprio, bgnEndInfo_p->commThreadStack) == FALSE)
    {
        NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_SDK);
        NHTTPi_free(bgnEndInfo_p->commThreadStack);
        bgnEndInfo_p->commThreadStack = NULL;
        NHTTPi_exitLockReqList();
        return FALSE;
    }

#if defined(NHTTP_FOR_RVL)
    {
        s32 err = NCDGetCurrentIpConfig(&(bgnEndInfo_p->ipConfig));
        if( err < 0 )
        {
            OSReport("NCDGetCurrentIpConfig err = %d\n", err);
            OSHalt("NCDGetCurrentIpConfig");
        }
    }
#endif

    bgnEndInfo_p->isOpened = TRUE;
    return TRUE;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_CleanupAsync

  Description:  NHTTP���C�u�����̉����v������

  Arguments:	o_bgnEndInfo_p	NHTTPBgnEndInfo�\���̂ւ̃|�C���^
                i_callback	NHTTP���C�u�����̉�����������������ۂɌĂяo�����R�[���o�b�N�֐��ւ̃|�C���^

  Returns:      �Ȃ�
 *-------------------------------------------------------------------------*/
extern void NHTTPi_CleanupAsync(NHTTPSysInfo* sysInfo_p, NHTTPCleanupCallback callback)
{
    NHTTPBgnEndInfo*	bgnEndInfo_p = NHTTPi_GetBgnEndInfoP(sysInfo_p);
    NHTTPThreadInfo*	threadInfo_p = NHTTPi_GetThreadInfoP(sysInfo_p);
    
    DEBUG_PRINTF("[%s]\n", __func__);
    
    NHTTPi_ASSERT(bgnEndInfo_p->isOpened);

    NHTTPi_CheckCurrentThread(threadInfo_p, NHTTP_CURRENT_THREAD_IS_NOT_NHTTP_THREAD);
    
    // ���ݎ��s���̗v�������ׂĒ��f���A���������������
    NHTTPi_cancelAllRequests(sysInfo_p);
    NHTTPi_destroyCommThread(threadInfo_p, bgnEndInfo_p);
    NHTTPi_free(bgnEndInfo_p->commThreadStack);
    bgnEndInfo_p->commThreadStack = NULL;
    NHTTPi_exitLockReqList();
    bgnEndInfo_p->isOpened = FALSE;
    if (callback)
    {
        callback();
    }
    
    {
        u32	connectionListLength = NHTTPi_GetConnectionListLength();

        if (connectionListLength > 0)
        {
            PRINTF("*warning: %d connections rests! Please free connections.\n", connectionListLength);
        }
    }

    // ���N�G�X�g���ЂƂ�KeepAlive���Ƀ\�P�b�g��close����Ȃ��Ȃ�o�O�C��
    // KeepAlive��SSL�͂��蓾�Ȃ��̂�NHTTPi_SocClose�łȂ��Ă��ǂ�
    if (IS_SOCKET(bgnEndInfo_p->socket))
    {
        NHTTPi_SocClose(NULL, NULL, bgnEndInfo_p->socket);
        DEBUG_PRINTF("[%s][%d] INVALID_SOCKET \n", __func__, __LINE__);
        
        bgnEndInfo_p->socket = INVALID_SOCKET;
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_GetError

  Description:  NHTTP���C�u�������ōŌ�ɔ��������G���[�̏󋵂��擾����

  Arguments:	i_bgnEndInfo_p	NHTTPBgnEndInfo�\���̂ւ̃|�C���^

  Returns:      NHTTPErr	���̊֐����Ăяo�����O��NHTTP���C�u�������Ŕ��������G���[�̏�
 *-------------------------------------------------------------------------*/
extern NHTTPError NHTTPi_GetError(NHTTPBgnEndInfo* bgnEndInfo_p)
{
    NHTTPi_ASSERT(bgnEndInfo_p->isOpened);
    return bgnEndInfo_p->error;
}
