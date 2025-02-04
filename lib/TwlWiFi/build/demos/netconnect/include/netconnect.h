/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - demos - netconnect
  File:     netconnect.h

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITROWIFI_DEMOS_NETCONNECT_H_
#define NITROWIFI_DEMOS_NETCONNECT_H_

#ifdef __cplusplus

extern "C"
{
#endif

void    NcGlobalInit(void);
void    NcStart(const char* apClass);
void    NcFinish(void);
void    NcSetDevice(u8 deviceId);
void*   NcAlloc(u32 name, s32 size);
void    NcFree(u32 name, void* ptr, s32 size);

#ifdef __cplusplus

} /* extern "C" */
#endif

#endif /* NITROWIFI_DEMOS_NETCOONECT_H_ */
