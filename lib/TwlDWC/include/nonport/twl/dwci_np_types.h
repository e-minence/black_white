/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWCI_NP_TYPES_H_
#define DWCI_NP_TYPES_H_

/// 1970�N����̌o�ߕb��
typedef s64 DWCUtcTime;

// Weak symbol
#define DWC_WEAK_SYMBOL  __declspec(weak)

// C�W����tm�ƌ݊����̂���^
typedef struct
{
    int	tm_sec;
    int	tm_min;
    int	tm_hour;
    int	tm_mday;
    int	tm_mon;
    int	tm_year;
    int	tm_wday;
    int	tm_yday;
    int	tm_isdst;
}
DWCTmTime;

#endif
