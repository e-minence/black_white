/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS
  File:     os.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_H_
#define NITRO_OS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/os/common/system.h>
#include <nitro/os/common/interrupt.h>
#include <nitro/os/common/thread.h>
#include <nitro/os/common/event.h>
#include <nitro/os/common/context.h>
#include <nitro/os/common/spinLock.h>
#include <nitro/os/common/timer.h>
#include <nitro/os/common/systemCall.h>
#include <nitro/os/common/printf.h>
#include <nitro/os/common/emulator.h>
#include <nitro/os/common/message.h>
#include <nitro/os/common/mutex.h>
#include <nitro/os/common/exception.h>
#include <nitro/os/common/init.h>
#include <nitro/os/common/arena.h>
#include <nitro/os/common/alloc.h>
#include <nitro/os/common/tick.h>
#include <nitro/os/common/alarm.h>
#include <nitro/os/common/valarm.h>
#include <nitro/os/common/system.h>
#include <nitro/os/common/reset.h>
#include <nitro/os/common/ownerInfo.h>
#include <nitro/os/common/entropy.h>
#include <nitro/os/common/systemWork.h>
#include <nitro/os/common/resource.h>

//---- profiler
#include <nitro/os/common/profile.h>
#include <nitro/os/common/callTrace.h>
#include <nitro/os/common/functionCost.h>

#ifdef	SDK_ARM9
#include <nitro/os/ARM9/protectionRegion.h>
#include <nitro/os/ARM9/tcm.h>
#include <nitro/os/ARM9/cache.h>
#include <nitro/os/ARM9/protectionUnit.h>
#include <nitro/os/ARM9/vramExclusive.h>
#include <nitro/os/ARM9/china.h>
#include <nitro/os/ARM9/argument.h>
#else  //SDK_ARM7
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_H_ */
#endif
