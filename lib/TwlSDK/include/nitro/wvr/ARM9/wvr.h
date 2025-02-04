/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WVR - include
  File:     wvr.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_WVR_ARM9_WVR_H_
#define NITRO_WVR_ARM9_WVR_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    <nitro/gx/gx_vramcnt.h>

// 非同期 API のコールバック関数型
typedef void (*WVRCallbackFunc) (void *arg, WVRResult result);


/*---------------------------------------------------------------------------*
  Name:         WVR_StartUpAsync

  Description:  無線ライブラリを駆動開始する。
                強制停止するまで、指定したVRAM( C or D )へのアクセスは禁止になる。

  Arguments:    vram        -   ARM7に割り当てるVRAMバンクを指定。
                callback    -   処理完了時のコールバック関数を指定。
                arg         -   コールバック関数に渡す引数を指定。

  Returns:      処理結果を返す。
 *---------------------------------------------------------------------------*/
WVRResult WVR_StartUpAsync(GXVRamARM7 vram, WVRCallbackFunc callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         WVR_TerminateAsync

  Description:  無線ライブラリを強制停止する。
                非同期処理完了後、VRAM( C or D )へのアクセスは許可される。

  Arguments:    callback    -   処理完了時のコールバック関数を指定。
                arg         -   コールバック関数に渡す引数を指定。

  Returns:      処理結果を返す。
 *---------------------------------------------------------------------------*/
WVRResult WVR_TerminateAsync(WVRCallbackFunc callback, void *arg);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_WVR_ARM9_WVR_H_ */

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
