/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     NHTTP_control.h

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
#include "NHTTP_control.h"
#include "d_nhttp_common.h"
#include "NHTTP_request.h"
#include "NHTTP_nonport.h"

/*-------------------------------------------------------------------------*
  Name:         addHdrList

  Description:  ���M���X�g��Header��ǉ�����
                 
  Arguments:    list_pp       �ǉ����郊�X�g�̃|�C���^
                error_p       �G���[�l�i�[�p�|�C���^
                label         �ǉ����郉�x��
                value         �ǉ�����v�f

  Returns:      TRUE	�����ɐ�������
                FALSE	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
static BOOL addHdrList(NHTTPi_DATALIST** list_pp, NHTTPBgnEndInfo* bgnEndInfo_p, const char* label_p, const char* value_p)
{
    const int listsize = sizeof(NHTTPi_DATALIST);
    
    int                 a_isFound = FALSE; // ������HeaderField�ɓ������x������������
    NHTTPi_DATALIST*    list_p;
    
    if (*list_pp)
    {
        // ������HeaderField�ɓ��� label�̂��̂��Ȃ����T��
        list_p = *list_pp;
        if (NHTTPi_compareToken(label_p, list_p->label_p) != 0)
        {
            for (list_p = list_p->next_p; list_p != *list_pp; list_p = list_p->next_p)
            {
                if (NHTTPi_compareToken(label_p, list_p->label_p) == 0)
                {
                    a_isFound = TRUE;
                    break;
                }
            }
        }
        else
        {
            a_isFound = TRUE;
        }
    }
    
    if (a_isFound)
    {
        // ����label������Ώ㏑��
        list_p->value_p = value_p;
    }
    else
    {
        // ���X�g�v�f����ǉ�
        list_p = (NHTTPi_DATALIST*)NHTTPi_alloc(listsize, 4);
        if (!list_p)
        {
            //DEBUG_PRINTF("[%s][line:%d] NHTTP_ERROR_ALLOC\n", __func__, __LINE__);
            PRINTF("failed to allocate memory\n");
            NHTTPi_SetError(bgnEndInfo_p, NHTTP_ERROR_ALLOC);
            return FALSE;
        }
        list_p->label_p = label_p;
        list_p->value_p = value_p;
        list_p->length = 0;
        list_p->isBinary = FALSE;
        
        if (*list_pp)
        {
            list_p->prev_p = (*list_pp)->prev_p;
            list_p->next_p = *list_pp;
            (*list_pp)->prev_p->next_p = list_p;
            (*list_pp)->prev_p = list_p;
        }
        else
        {
            list_p->next_p = list_p;
            list_p->prev_p = list_p;
            *list_pp = list_p;
        }
    }
    
    return TRUE;
}

/*-------------------------------------------------------------------------*
  Name:         incAscii

  Description:  ASCII�R�[�h��1�����C���N�������g����
                 
  Arguments:    o_c             �C���N�������g�����������̃|�C���^

  Returns:      �C���N�������g���������̃|�C���^
 *-------------------------------------------------------------------------*/
static char incAscii(u8 c)
{
    c++;
    if (c == 'z'+1)
    {
        c = '0';
    }
    else if (c == 'Z'+1)
    {
        c = 'a';
    }
    else if (c == '9'+1)
    {
        c = 'A';
    }

    return (char)c;
}

/*-------------------------------------------------------------------------*
  Name:         checkTagPost

  Description:  tagPost�Əd������t���[�Y����������tagPost��ύX
                 
  Arguments:    req     ���N�G�X�g�\���̂̃|�C���^
                value   �T���t���[�Y
                length  �t���[�Y�̒���
  
  Returns:      TRUE	�����ɐ�������
                FALSE	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
static BOOL checkTagPost(NHTTPReq* req_p, const char* value_p, u32 length)
{
    int  n;
    char c;
    
    if (NHTTPi_memfind(value_p, (int)length, &req_p->tagPost[2], LEN_POSTBOUND) < 0)
    {
        return TRUE;
    }
    
    // tagPost������    
    for (n = 2 + LEN_POSTBOUND - 1; n >= 2; n--)
    {
        for (c = req_p->tagPost[n];  ; )
        {
            c = incAscii((u8)c);
            req_p->tagPost[n] = c;
            
            if (c == NHTTPi_strMultipartBound[n])
            {
                break;
            }
            if (NHTTPi_memfind(value_p, (int)length, &req_p->tagPost[2], LEN_POSTBOUND) < 0)
            {
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_getHdrFromList

  Description:  ���X�g�̐擪�v�f���擾
                �擾���ꂽ�v�f�̓��X�g�����菜�����
                �������A�������̈�͉������Ȃ�
                
  Arguments:    o_list_pp       ���X�g
  
  Returns:      ���X�g�̐擪�v�f
 *-------------------------------------------------------------------------*/
extern NHTTPi_DATALIST* NHTTPi_getHdrFromList(NHTTPi_DATALIST** list_pp)
{
    NHTTPi_DATALIST* list_p = *list_pp;

    if (list_p)
    {
        if (list_p != list_p->prev_p)
        {
            list_p->prev_p->next_p = list_p->next_p;
            list_p->next_p->prev_p = list_p->prev_p;
            *list_pp = list_p->next_p;
        }
        else
        {
            *list_pp = NULL;
        }
    }
    
    return list_p;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_AddRequestHeaderField

  Description:  HTTP���N�G�X�g���s���ɑ��M����HTTP�w�b�_���ڂ�ǉ�����

  Arguments:    req_p		�����Ώۂ�HTTPReq�I�u�W�F�N�g�ւ̃|�C���^
                bgnEndInfo_p	NHTTPBgnEndInfo�\���̃|�C���^
                label_p		�ǉ�����HTTP�w�b�_���ڂ̃��x��(NULL�I�[ASCII������)�ւ̃|�C���^
                value_p		�ǉ�����HTTP�w�b�_���ڂ̒l(NULL�I�[ASCII������)�ւ̃|�C���^

  Returns:      TRUE	�����ɐ�������
                FALSE	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern BOOL NHTTPi_AddRequestHeaderField(NHTTPReq* req_p, NHTTPBgnEndInfo* bgnEndInfo_p, const char* label_p, const char* value_p)
{
    NHTTPi_ASSERT(NHTTPi_IsOpened(bgnEndInfo_p));
    NHTTPi_ASSERT(req_p);
    NHTTPi_ASSERT(label_p);
    NHTTPi_ASSERT(value_p);
    
    if (req_p->isStarted)
    {
        return FALSE;
    }

    return addHdrList(&req_p->listHeader_p, bgnEndInfo_p, label_p, value_p);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_AddRequestPostDataAscii

  Description:  HTTP���N�G�X�g���s���ɑ��M����POST����(ASCII������)��ǉ�����

  Arguments:    req_p		�����Ώۂ�HTTPReq�I�u�W�F�N�g�ւ̃|�C���^
                bgnEndInfo_p	NHTTPBgnEndInfo�\���̃|�C���^
                label_p		�ǉ�����POST���ڂ̃��x��(NULL�I�[ASCII������)�ւ̃|�C���^
                value_p		�ǉ�����POST���ڂ̒l(NULL�I�[ASCII������)�ւ̃|�C���^

  Returns:      TRUE	�����ɐ�������
                FALSE	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern BOOL NHTTPi_AddRequestPostDataAscii(NHTTPReq* req_p, NHTTPBgnEndInfo* bgnEndInfo_p, const char* label_p, const char* value_p)
{
    // ��s��998�����𒴂��Ȃ�����
    int rc = FALSE;
    u32 length = 0;
    
    NHTTPi_ASSERT(NHTTPi_IsOpened(bgnEndInfo_p));
    NHTTPi_ASSERT(req_p);
    
    if (req_p->isStarted)
    {
        return FALSE;
    }

#if 1 // 2006/09/28 noma
    // �� �� NHTTPAddPostDataRaw���Ă΂�Ă���
    if ( req_p->isRawData )
    {
        //DEBUG_PRINTF("[%s] error: ���� NHTTPAddPostDataRaw ���Ă΂�Ă���\n", __func__);
        PRINTF("already called NHTTPAddPostDataRaw (exclusive fucntion)\n");
        return FALSE;
    }
#endif
    
    if (value_p)
    {
        length = (u32)NHTTPi_strlen(value_p);
    }
    if (checkTagPost(req_p, value_p, length))
    {
        rc = addHdrList(&req_p->listPost_p, bgnEndInfo_p, label_p, value_p);
        if (rc)
        {
            req_p->listPost_p->prev_p->length = length;
        }
    }
    
    return rc;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_AddRequestPostDataBinary

  Description:  HTTP���N�G�X�g���s���ɑ��M����POST����(�o�C�i���f�[�^)��ǉ�����

  Arguments:    req_p		�����Ώۂ�HTTPReq�I�u�W�F�N�g�ւ̃|�C���^
                bgnEndInfo_p	NHTTPBgnEndInfo�\���̃|�C���^
                label_p		�ǉ�����POST���ڂ̃��x��(NULL�I�[ASCII������)�ւ̃|�C���^
                value_p		�ǉ�����POST���ڂ̒l(�o�C�i���f�[�^)�ւ̃|�C���^
                length		�ǉ�����POST���ڂ̒l�̒���(Byte�P��)

  Returns:      TRUE	�����ɐ�������
                FALSE	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern BOOL NHTTPi_AddRequestPostDataBinary(NHTTPReq* req_p, NHTTPBgnEndInfo* bgnEndInfo_p, const char* label_p, const char* value_p, u32 length)
{
    int rc;
	
    NHTTPi_ASSERT(NHTTPi_IsOpened(bgnEndInfo_p));
    NHTTPi_ASSERT(req_p);
    
    if (req_p->isStarted)
    {
        return FALSE;
    }
    
    // �� �� NHTTPAddPostDataRaw���Ă΂�Ă���
    if ( req_p->isRawData )
    {
        //DEBUG_PRINTF("[%s] error: ���� NHTTPAddPostDataRaw ���Ă΂�Ă���\n", __func__);
        PRINTF("already called NHTTPAddPostDataRaw (exclusive fucntion)\n");
        return FALSE;
    }

    rc = FALSE;
    if (checkTagPost(req_p, value_p, length))
    {
        rc = addHdrList(&req_p->listPost_p, bgnEndInfo_p, label_p, value_p);
        if (rc)
        {
            req_p->listPost_p->prev_p->length = length;
            req_p->listPost_p->prev_p->isBinary = TRUE;
        }
    }
    
    return rc;
}

