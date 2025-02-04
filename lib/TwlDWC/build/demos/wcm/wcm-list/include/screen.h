 /*---------------------------------------------------------------------------*
  Project:  TwlSDK - WCM - demos - wcm-list-2
  File:     screen.h

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
#ifndef SCREEN_H_
#define SCREEN_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/
#include <nitro/types.h>

/*---------------------------------------------------------------------------*
    �֐� ��`
 *---------------------------------------------------------------------------*/
void    InitScreen(void);
void    ClearScreen(void);
void    PutMainScreen(s32 x, s32 y, u8 palette, char* text, ...);
void    PutSubScreen(s32 x, s32 y, u8 palette, char* text, ...);
void    UpdateScreen(void);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* SCREEN_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
