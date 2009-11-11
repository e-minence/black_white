/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     NHTTP_list.c

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
#include "NHTTP_list.h"
#include "NHTTP_bgnend.h"
#include "NHTTP_request.h"
#include "NHTTP_control.h"
#include "d_nhttp_private.h"


/*-------------------------------------------------------------------------*

  Name:         NHTTPi_InitListInfo

  Description:  NHTTPListInfo�\���̂̏����ݒ�
                 
  Arguments:    o_listInfo_p  NHTTPListInfo�\���̂ւ̃|�C���^

  Returns:      
 *-------------------------------------------------------------------------*/
extern void NHTTPi_InitListInfo(NHTTPListInfo* listInfo_p)
{
    listInfo_p->reqQueue = NULL;
    listInfo_p->idRequest = 0;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_setReqQueue

  Description:  �V�K�Ƀ��������m�ۂ��ăL���[���X�g�ɂȂ�
                ReqList�̓��b�N����Ă��Ȃ���΂Ȃ�Ȃ�
                
  Arguments:    req     ���N�G�X�g�\���̂̃|�C���^

  Returns:      >=0     id�𓾂�
                < 0	���s�i���������m�ۂł��Ȃ��j
 *-------------------------------------------------------------------------*/
extern int NHTTPi_setReqQueue(NHTTPListInfo* listInfo_p, NHTTPReq const* req_p)
{
    NHTTPi_REQLIST* list_p;
    int id = -1;
    
    list_p = (NHTTPi_REQLIST*)NHTTPi_alloc(sizeof(NHTTPi_REQLIST), 4);
    
    if (list_p)
    {
        // exec�ɐڑ�
        if (listInfo_p->reqQueue)
        {
            list_p->prev_p = listInfo_p->reqQueue->prev_p;
            list_p->next_p = (NHTTPi_REQLIST*)listInfo_p->reqQueue;
            listInfo_p->reqQueue->prev_p->next_p = list_p;
            listInfo_p->reqQueue->prev_p = list_p;
        }
        else
        {
            list_p->prev_p = list_p;
            list_p->next_p = list_p;
            listInfo_p->reqQueue = list_p;
        }
        
        list_p->id = listInfo_p->idRequest++;
        list_p->req_p = (NHTTPReq*)req_p;
        list_p->socket_p = (void*)-1;
        id = list_p->id;
        if (listInfo_p->idRequest < 0)
        {
            listInfo_p->idRequest = 0;
        }
    }
    
    return id;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_findReqQueue

  Description:  ���N�G�X�g���X�g����w�肵��id�̃��N�G�X�g��������
                ReqList�̓��b�N����Ă��Ȃ���΂Ȃ�Ȃ�
  
  Arguments:    id

  Returns:      ���N�G�X�g���X�g
                �擾�ł��Ȃ��ꍇ��NULL  
 *-------------------------------------------------------------------------*/
extern NHTTPi_REQLIST* NHTTPi_findReqQueue(NHTTPListInfo* listInfo_p, const int id)
{
    NHTTPi_REQLIST*	list_p = NULL;
    NHTTPi_REQLIST*	listRet_p = NULL;

    if (listInfo_p->reqQueue)
    {
        list_p = (NHTTPi_REQLIST*)(listInfo_p->reqQueue);
        if (list_p->id == id)
        {
            listRet_p = list_p;
        }
        else
        {
            for (list_p=list_p->next_p; list_p!=(listInfo_p->reqQueue); list_p=list_p->next_p)
            {
                if (list_p->id == id)
                {
                    listRet_p = list_p;
                    break;
                }
            }
        }
    }

    return listRet_p;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_freeReqQueue

  Description:  �w�肵��id���L���[���X�g������
                ���X�g����id�ɑΉ��������Ƃ��ĉ������
                ReqList�̓��b�N����Ă��Ȃ���΂Ȃ�Ȃ�
  
  Arguments:    id

  Returns:      TRUE	�����ɐ�������
                FALSE	�����Ɏ��s�����iid�̃��N�G�X�g�����݂��Ȃ��j
 *-------------------------------------------------------------------------*/
extern int NHTTPi_freeReqQueue(NHTTPListInfo* listInfo_p, NHTTPMutexInfo* mutexInfo_p, const int id)
{
    NHTTPi_REQLIST*     list_p;
    NHTTPRes*           res_p;
    void*               param_p;
    int                 rc = FALSE;
    NHTTPConnection*	connection_p = NULL;
    
    // exec������T��
    list_p = NHTTPi_findReqQueue(listInfo_p, id);
    if (list_p)
    {
        // �E�o
        if (listInfo_p->reqQueue != listInfo_p->reqQueue->prev_p)
        {
            list_p->prev_p->next_p = list_p->next_p;
            list_p->next_p->prev_p = list_p->prev_p;
            if (listInfo_p->reqQueue == list_p)
            {
                listInfo_p->reqQueue = list_p->next_p;
            }
        }
        else
        {
            listInfo_p->reqQueue = NULL;
        }

        res_p = list_p->req_p->res_p;
        param_p = res_p->param_p;
        connection_p = NHTTPi_Request2Connection(mutexInfo_p, list_p->req_p);
        NHTTPi_destroyRequestObject(mutexInfo_p, list_p->req_p);
        NHTTPi_free(list_p);

        /** NHTTPCreateConnection�g�p */
        if (connection_p != NULL)
        {
            connection_p->nhttpError = NHTTP_ERROR_CANCELED;
            NHTTPi_CompleteCallback(mutexInfo_p, connection_p);
            
            NHTTPi_DeleteConnection(connection_p);
        }
        rc = TRUE;
    }
    
    return rc;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_allFreeReqQueue

  Description:  �L���[���X�g���̗v�f�����ׂĉ��
                ReqList�̓��b�N����Ă��Ȃ���΂Ȃ�Ȃ�  
 *-------------------------------------------------------------------------*/
extern void NHTTPi_allFreeReqQueue(NHTTPListInfo* listInfo_p, NHTTPMutexInfo* mutexInfo_p)
{
    while (listInfo_p->reqQueue)
    {
        NHTTPi_freeReqQueue(listInfo_p, mutexInfo_p, listInfo_p->reqQueue->id);
    }
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_getReqFromReqQueue

  Description:  �L���[���X�g�̐擪���擾
                �擾���ꂽ�v�f�̓L���[���X�g�����菜�����
                �������A�������̈�͉������Ȃ�
                ReqList�̓��b�N����Ă��Ȃ���΂Ȃ�Ȃ�
  
  Returns:      �L���[���X�g�̐擪
                �擾�ł��Ȃ��ꍇ��NULL  
 *-------------------------------------------------------------------------*/
extern NHTTPi_REQLIST* NHTTPi_getReqFromReqQueue(NHTTPListInfo* listInfo_p)
{
    return (NHTTPi_REQLIST*)NHTTPi_getHdrFromList(
        (NHTTPi_DATALIST**)(void *)(&(listInfo_p->reqQueue)));
}
