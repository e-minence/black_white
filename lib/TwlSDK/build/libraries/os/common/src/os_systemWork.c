/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - libraries
  File:     os_systemWork.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-02-01#$
  $Rev: 4001 $
  $Author: nakasima $
 *---------------------------------------------------------------------------*/

#include <nitro/os.h>
#ifdef SDK_TWL
#include    <nitro/os/common/emulator.h>
#include    <twl/os/common/codecmode.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_GetBootType

  Description:  自分自身のブート種別を取得

  Arguments:    なし
  
  Returns:      自分自身のブート種別を指す、OSBootType 型を返します。
 *---------------------------------------------------------------------------*/
OSBootType OS_GetBootType(void)
{
    return OS_GetBootInfo()->boot_type;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetBootInfo

  Description:  自分自身のブート種別に関する情報を取得

  Arguments:    なし
  
  Returns:      自分自身のブート種別に関する情報を指す、OSBootInfo 構造体を返す
 *---------------------------------------------------------------------------*/
const OSBootInfo *OS_GetBootInfo(void)
{
	return ((const OSBootInfo *)HW_WM_BOOT_BUF);
}

//---- internal

#ifdef SDK_TWL

BOOL OSi_IsCodecTwlMode(void)
{
    static BOOL retval;
    static BOOL initialized = FALSE;

    if ( initialized == FALSE )
    {
        retval = ((OS_IsRunOnTwl() == TRUE) && ((*((u8*)(HW_TWL_ROM_HEADER_BUF + 0x01bf)) & 0x01) == OS_CODECMODE_TWL));
    }
    initialized = TRUE;

    return retval;
}

#endif // SDK_TWL

