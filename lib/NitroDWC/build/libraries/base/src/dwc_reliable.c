/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     dwc_reliable.c

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#include <nitro.h>

// the original header
//---------------------------------------------------------
#include <base/dwc_report.h>
#include <base/dwc_error.h>
#include <base/dwc_memfunc.h>

#include <base/dwc_base_gamespy.h>
#include <dwci_reliable.h>

#define DWC_ASSERT          SDK_ASSERT
#define DWC_ASSERTMSG       SDK_ASSERTMSG
#define DWCi_Np_CpuCopy32   MI_CpuCopy8

/* -----------------------------------------------------------------
  �����ϐ�
  -------------------------------------------------------------------*/
const int       RETRY_MAX_COUNT = 3;
const int       QUEUE_MAX_HEAP  = 1024 * 32;
const int       QUEUE_MAX_ACCEPT_BORDER  = QUEUE_MAX_HEAP - (QUEUE_MAX_HEAP >> 2);

typedef struct _tagDWCiSendQueue
{
    GT2Connection               connection;
    void*                       buffer;
    u32                         size;
    struct _tagDWCiSendQueue*   next;
}
DWCiSendQueue;

static struct
{
    DWCiSendQueue*  queue;
    int             usedheap;
}
s_queueinfo;


/*
 * �L���[�����������܂�
 */
void DWCi_InitReliableQueue()
{
    s_queueinfo.queue = NULL;
    s_queueinfo.usedheap = 0;
}

/*
 * �L���[�ɂ��܂����f�[�^���N���A���܂�
 */
void DWCi_CleanupReliableQueue()
{
    DWCiSendQueue*   it;

    for ( it = s_queueinfo.queue; it != NULL;  )
    {
        s_queueinfo.queue = it->next;
        DWC_Free( DWC_ALLOCTYPE_BASE, it->buffer, it->size );
        DWC_Free( DWC_ALLOCTYPE_BASE, it, 0 );
        it = s_queueinfo.queue;
    }

    s_queueinfo.queue = NULL;
    s_queueinfo.usedheap = 0;
}

/*
 * �L���[�Ƀf�[�^��ǉ��ł����Ԃ��ǂ�����Ԃ��܂�
 */
BOOL DWCi_IsReliableQueueFree()
{

    if ( s_queueinfo.usedheap >= QUEUE_MAX_ACCEPT_BORDER )
    {
        return FALSE;
    }

    return TRUE;
}

/*
 * �L���[�ɂ��܂����f�[�^���������܂�
 *
 * @param connection �ʐM�Ɏg�p����gt2connection
 *
 * @retval TRUE     �L���[����
 * @retval FALSE    �L���[�ɂ܂����܂��Ă���
 */
BOOL DWCi_ProcessReliableQueue()
{
    return DWCi_ProcessReliableQueueEx( gt2Send );
}

BOOL DWCi_ProcessReliableQueueEx( DWCiGT2SendFunc func )
{
    DWCiSendQueue*   it;

    // �L���[�ɂ��܂��Ă�����̂�����ΐ�ɑ��M����
    for ( it = s_queueinfo.queue; it != NULL; it = it->next )
    {
        int retrycount;

        for ( retrycount = 0; retrycount < RETRY_MAX_COUNT; retrycount++ )
        {
            GT2Result res = func( it->connection, it->buffer, (int)it->size, TRUE );

            if ( res == GT2Success )
            {
                s_queueinfo.usedheap -= it->size;

                DWC_Free( DWC_ALLOCTYPE_BASE, it->buffer, it->size );
                it->buffer = NULL;
                it->size = 0;
                it->connection = NULL;

                break;
            }
        }

        if ( retrycount == RETRY_MAX_COUNT )
        {
            // ���g���C�񐔂��K��l�ɒB�����獡��̌Ăяo���ł̑��M����߂�
            DWC_Printf( DWC_REPORTFLAG_TRANSPORT, "DWCi_ProcessReliableQueue() retry count exceeded!\n" );
            break;
        }
    }

    // ���M�ς݂̃A�C�e�����L���[�������
    for ( it = s_queueinfo.queue; it != NULL;  )
    {
        if ( it->buffer == NULL )
        {
            s_queueinfo.queue = it->next;
            DWC_Free( DWC_ALLOCTYPE_BASE, it, 0 );
            it = s_queueinfo.queue;
        }
        else
        {
            break;
        }
    }

    return s_queueinfo.queue == NULL;
}


/*
 * �L���[��Reliable���M����f�[�^��ǉ����܂�
 *
 * �o�b�t�@�̓��e�͓����ɃR�s�[�����̂ł��̊֐����Ăяo�������
 * ������Ă��\���܂���B
 *
 * @param buffer    �o�b�t�@
 * @param size      �o�b�t�@�̃T�C�Y
 *
 * @retval  TRUE    �ǉ��ł���
 * @retval  FALSE   �������[�s���ɂ��ǉ��ł��Ȃ�����
 *                  �f�o�b�O�r���h�ł̓A�T�[�g�����
 */
BOOL DWCi_InsertReliableQueue( GT2Connection connection, const u8* buffer, int size )
{
    DWCiSendQueue*   it;
    DWCiSendQueue**  target = &s_queueinfo.queue;
    
    // �g�p�ʐ����ȏ�̃������[��������玸�s
    if ( s_queueinfo.usedheap + size > QUEUE_MAX_HEAP )
    {
        DWC_ASSERTMSG( s_queueinfo.usedheap + size < QUEUE_MAX_HEAP, "DWCi_InsertReliableQueue() exceeds to heap limit.\n" );
        return FALSE;
    }

    // �����̃A�C�e�����擾����
    for ( it = s_queueinfo.queue; it != NULL; it = it->next )
    {
        target = &it->next;
    }

    *target = DWC_Alloc( DWC_ALLOCTYPE_BASE, sizeof(DWCiSendQueue) );
    if ( (*target) == NULL )
    {
        // �������[�̊m�ۂɎ��s����
        DWC_ASSERTMSG( (*target)->buffer != NULL, "DWCi_InsertReliableQueue() failed to allocate from heap.\n" );
        return FALSE;
    }

    (*target)->buffer = DWC_Alloc( DWC_ALLOCTYPE_BASE, (u32)size );
    if ( (*target)->buffer == NULL)
    {
        // �������[�̊m�ۂɎ��s����
        DWC_Free( DWC_ALLOCTYPE_BASE, (*target), sizeof(DWCiSendQueue) );
        (*target) = NULL;
        DWC_ASSERTMSG( (*target)->buffer != NULL, "DWCi_InsertReliableQueue() failed to allocate from heap.\n" );
        return FALSE;
    }

    (*target)->connection = connection;
    (*target)->size = (u32)size;
    (*target)->next = NULL;
    DWCi_Np_CpuCopy32( buffer, (*target)->buffer, (u32)size );

    s_queueinfo.usedheap += size;
    
    return TRUE;
}
