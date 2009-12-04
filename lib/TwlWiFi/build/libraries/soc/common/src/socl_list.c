/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - SOC - libraries
  File:     socl_list.c

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
#include <nitroWiFi/socl.h>

SOCLSocket*         SOCLiSocketList = NULL;
SOCLSocket*         SOCLiSocketListTrash = NULL;

static SOCLSocket **     SOCLi_SocketGetNextPtr(SOCLSocket ** start, SOCLSocket* socket);
static void SOCLi_SocketRegisterList(SOCLSocket ** next, SOCLSocket* socket);
static void SOCLi_SocketUnregisterList(SOCLSocket ** next, SOCLSocket* socket);

/*---------------------------------------------------------------------------*
  Name:         SOCLi_SocketRegister

  Description:  �\�P�b�g�����X�g�֓o�^����

  Arguments:    socket  �\�P�b�g

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void SOCLi_SocketRegister(SOCLSocket* socket)
{
    SOCLi_SocketRegisterList(&SOCLiSocketList, socket);
}

static void SOCLi_SocketRegisterList(SOCLSocket ** next, SOCLSocket* socket)
{
    socket->next = *next;
    *next = socket;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_SocketRegisterTrash

  Description:  �\�P�b�g��p�����X�g�֓o�^����

  Arguments:    socket  �\�P�b�g

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void SOCLi_SocketRegisterTrash(SOCLSocket* socket)
{
    SOCLi_SocketRegisterList(&SOCLiSocketListTrash, socket);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_SocketUnregister

  Description:  �\�P�b�g�����X�g����폜����

  Arguments:    socket  �\�P�b�g

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void SOCLi_SocketUnregister(SOCLSocket* socket)
{
    SOCLi_SocketUnregisterList(&SOCLiSocketList, socket);
}

static void SOCLi_SocketUnregisterList(SOCLSocket ** next, SOCLSocket* socket)
{
    OSIntrMode      enable = OS_DisableInterrupts();
    
    next = SOCLi_SocketGetNextPtr(next, socket);

    if (next)
    {
        *next = socket->next;
    }
    (void) OS_RestoreInterrupts(enable);
}

static SOCLSocket ** SOCLi_SocketGetNextPtr(SOCLSocket ** next, SOCLSocket* socket)
{
    OSIntrMode      enable = OS_DisableInterrupts();
    SOCLSocket*     t;

    for (t = *next; t; t = t->next)
    {
        if (t == socket)
        {
            (void) OS_RestoreInterrupts(enable);
            return next;
        }

        next = &t->next;
    }
    (void) OS_RestoreInterrupts(enable);
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_SocketUnregisterTrash

  Description:  �\�P�b�g��p�����X�g����폜����

  Arguments:    socket  �\�P�b�g

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void SOCLi_SocketUnregisterTrash(SOCLSocket* socket)
{
    SOCLi_SocketUnregisterList(&SOCLiSocketListTrash, socket);
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_SocketIsInvalid

  Description:  �\�P�b�g���\�P�b�g���X�g�ɓo�^����Ă��鐳���Ȃ��̂��̊m�F

  Arguments:    socket  �\�P�b�g

  Returns:      0 �ȊO �o�^����Ă���  0 �o�^����Ă��Ȃ�
 *---------------------------------------------------------------------------*/
int SOCL_SocketIsInvalid(SOCLSocket* socket)
{
    return((int)socket <= 0) || !SOCLi_SocketGetNextPtr(&SOCLiSocketList, socket);
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_SocketIsInTrash

  Description:  �\�P�b�g���p���\�P�b�g���X�g�ɓo�^����Ă��邩�̊m�F

  Arguments:    socket  �\�P�b�g

  Returns:      0 �ȊO �o�^����Ă���  0 �o�^����Ă��Ȃ�
 *---------------------------------------------------------------------------*/
int SOCL_SocketIsInTrash(SOCLSocket* socket)
{
    return SOCLi_SocketGetNextPtr(&SOCLiSocketListTrash, socket) != NULL;
}
