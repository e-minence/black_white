/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - SOC - demos
  File:     server_thread.h

  Copyright 2006-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitroWiFi.h>

/*---------------------------------------------------------------------------*
  Name:         CreateServerThread

  Description:  �T�[�o�X���b�h���쐬.

  Arguments:    None.

  Returns:      �Ȃ�.
 *---------------------------------------------------------------------------*/
void    CreateServerThread(void);

/*---------------------------------------------------------------------------*
  Name:         ServerThread

  Description:  http �N���C�A���g�Ƃ̒ʐM.

  Arguments:    arg.

  Returns:      �Ȃ�.
 *---------------------------------------------------------------------------*/
void    ServerThread(void* arg);
