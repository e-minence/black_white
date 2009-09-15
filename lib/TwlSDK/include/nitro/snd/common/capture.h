/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include - snd - common
  File:     capture.h

  Copyright 2004-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_SND_COMMON_CAPTURE_H_
#define NITRO_SND_COMMON_CAPTURE_H_

#include <nitro/types.h>

#if !(defined(SDK_WIN32) || defined(SDK_FROM_TOOL))
#ifdef SDK_ARM7
#include <nitro/hw/ARM7/ioreg_SND.h>
#ifndef SDK_TWL
#include <nitro/hw/ARM7/mmap_global.h>
#else //SDK_TWL
#include <twl/hw/ARM7/mmap_global.h>
#endif
#endif /* SDK_ARM7 */
#endif /* SDK_FROM_TOOL */

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
    macro definition
 ******************************************************************************/

#define SND_CAPTURE_NUM 2

#ifndef SDK_TEG

#define SND_CAPTURE_0_OUT_CHANNEL 1
#define SND_CAPTURE_1_OUT_CHANNEL 3

#define SND_CAPTURE_0_IN_CHANNEL 0
#define SND_CAPTURE_1_IN_CHANNEL 2

#define SND_CAPTURE_LEN_MAX  0xffff

#else

#define SND_CAPTURE_0_OUT_CHANNEL 3
#define SND_CAPTURE_1_OUT_CHANNEL 1

#define SND_CAPTURE_0_IN_CHANNEL 2
#define SND_CAPTURE_1_IN_CHANNEL 0

#define SND_CAPTURE_LEN_MAX  0x0fff

#endif

#define SND_CAPTURE_DAD_MASK 0x07fffffc

#define SND_CAPTURE_REG_OFFSET 0x8

/******************************************************************************
    enum definition
 ******************************************************************************/

#ifndef SDK_TEG
typedef enum
{
    SND_CAPTURE_0,
    SND_CAPTURE_1
}
SNDCapture;
#else
typedef enum
{
    SND_CAPTURE_1,
    SND_CAPTURE_0
}
SNDCapture;
#endif

typedef enum
{
    SND_CAPTURE_FORMAT_PCM16,
    SND_CAPTURE_FORMAT_PCM8
}
SNDCaptureFormat;

typedef enum
{
    SND_CAPTURE_REPEAT_YES,
    SND_CAPTURE_REPEAT_NO
}
SNDCaptureRepeat;

typedef enum
{
    SND_CAPTURE_IN_MIXER,
    SND_CAPTURE_IN_CHANNEL,

    SND_CAPTURE_IN_MIXER_L = SND_CAPTURE_IN_MIXER,
    SND_CAPTURE_IN_MIXER_R = SND_CAPTURE_IN_MIXER,
    SND_CAPTURE_IN_CHANNEL0 = SND_CAPTURE_IN_CHANNEL,
    SND_CAPTURE_IN_CHANNEL2 = SND_CAPTURE_IN_CHANNEL
}
SNDCaptureIn;

typedef enum
{
    SND_CAPTURE_OUT_NORMAL,
    SND_CAPTURE_OUT_CHANNEL_MIX,

    SND_CAPTURE_OUT_CHANNEL0_MIX = SND_CAPTURE_OUT_CHANNEL_MIX,
    SND_CAPTURE_OUT_CHANNEL2_MIX = SND_CAPTURE_OUT_CHANNEL_MIX
}
SNDCaptureOut;

/******************************************************************************
    inline function
 ******************************************************************************/

#ifdef SDK_ARM7
#if !(defined(SDK_WIN32) || defined(SDK_FROM_TOOL))

/*---------------------------------------------------------------------------*
  Name:         SND_StartCapture

  Description:  Start capture

  Arguments:    capture     : target capture

  Returns:      None
 *---------------------------------------------------------------------------*/
static  inline void SND_StartCapture(SNDCapture capture)
{
    REGType8v *reg = (REGType8v *)(REG_SNDCAP0CNT_ADDR + capture);

    *reg |= REG_SND_SNDCAP0CNT_E_MASK;
}

/*---------------------------------------------------------------------------*
  Name:         SND_StartCaptureBoth

  Description:  Start capture

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static  inline void SND_StartCaptureBoth(void)
{
    reg_SND_SNDCAPCNT |= REG_SND_SNDCAPCNT_CAP0_E_MASK | REG_SND_SNDCAPCNT_CAP1_E_MASK;
}

/*---------------------------------------------------------------------------*
  Name:         SND_StopCapture

  Description:  Stop capture

  Arguments:    capture     : target capture

  Returns:      None
 *---------------------------------------------------------------------------*/
static  inline void SND_StopCapture(SNDCapture capture)
{
    (*(REGType8v *)(REG_SNDCAP0CNT_ADDR + capture)) = 0;
}

#else

void    SND_StartCapture(SNDCapture capture);
void    SND_StartCaptureBoth(void);
void    SND_StopCapture(SNDCapture capture);

#endif /* SDK_FROM_TOOL */

#endif /* SDK_ARM7 */

/******************************************************************************
    public function declaration
 ******************************************************************************/

#ifdef SDK_ARM7

void    SND_SetupCapture(SNDCapture capture,
                         SNDCaptureFormat format,
                         void *buffer_addr,
                         int length, BOOL repeat, SNDCaptureIn in, SNDCaptureOut out);

BOOL    SND_IsCaptureActive(SNDCapture capture);

#endif /* SDK_ARM7 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NITRO_SND_COMMON_CAPTURE_H_ */
