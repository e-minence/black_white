/*---------------------------------------------------------------------------*
  Project:  TwlSDK - camera - include
  File:     camera.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef TWL_CAMERA_CAMERA_H_
#define TWL_CAMERA_CAMERA_H_

#include <twl/misc.h>
#include <twl/types.h>
#include <twl/hw/ARM9/ioreg.h>

#include <twl/camera/ARM9/camera_api.h>

#include <nitro/math.h>

#define CAMERA_GET_MAX_LINES(width)     MATH_MIN((1024 / (width)), 16)
#define CAMERA_GET_LINE_BYTES(width)    ((width) << 1)
#define CAMERA_GET_FRAME_BYTES(width, height)   (CAMERA_GET_LINE_BYTES(width) * (height))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CAMERA_OUTPUT_YUV,
    CAMERA_OUTPUT_RGB
} CAMERAOutput;

typedef enum {
    CAMERA_INTR_VSYNC_NONE          = (0 << REG_CAM_CNT_IREQVS_SHIFT),
    CAMERA_INTR_VSYNC_NEGATIVE_EDGE = (2 << REG_CAM_CNT_IREQVS_SHIFT),
    CAMERA_INTR_VSYNC_POSITIVE_EDGE = (3 << REG_CAM_CNT_IREQVS_SHIFT)
} CAMERAIntrVsync;

/*---------------------------------------------------------------------------*
  Name:         CAMERA_Reset

  Description:  hardware reset before I2C access

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_ResetCore( void );
SDK_INLINE void CAMERA_Reset( void )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_ResetCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_IsBusy

  Description:  whether camera is busy

  Arguments:    None

  Returns:      TRUE if camera is busy
 *---------------------------------------------------------------------------*/
BOOL CAMERA_IsBusyCore( void );
SDK_INLINE BOOL CAMERA_IsBusy( void )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return CAMERA_IsBusyCore();
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_Start

  Description:  start to receive camera data

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_StartCaptureCore( void );
SDK_INLINE void CAMERA_StartCapture( void )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_StartCaptureCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_Stop

  Description:  stop to receive camera data

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_StopCaptureCore( void );
SDK_INLINE void CAMERA_StopCapture( void )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_StopCaptureCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_SetTrimmingParamsCenter

  Description:  set camera trimming parameters by centering
                NOTE: should call CAMERA_SetTrimming to enable trimming

  Arguments:    destWidth       width of image to output
                destHeight      height of image to output
                srcWidth        original width of image
                srcHeight       original height of image

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_SetTrimmingParamsCenterCore(u16 destWidth, u16 destHeight, u16 srcWidth, u16 srcHeight);
SDK_INLINE void CAMERA_SetTrimmingParamsCenter(u16 destWidth, u16 destHeight, u16 srcWidth, u16 srcHeight)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_SetTrimmingParamsCenterCore(destWidth, destHeight, srcWidth, srcHeight);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_SetTrimmingParams

  Description:  set camera trimming parameters
                NOTE: widht = x2 - x1;  height = y2 - y1;
                NOTE: should call CAMERA_SetTrimming to enable trimming

  Arguments:    x1      X of top-left trimming point (multiple of 2)
                y1      Y of top-left trimming point
                x2      X of bottom-right trimming point (multiple of 2)
                y2      Y of bottom-right trimming point

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_SetTrimmingParamsCore(u16 x1, u16 y1, u16 x2, u16 y2);
SDK_INLINE void CAMERA_SetTrimmingParams(u16 x1, u16 y1, u16 x2, u16 y2)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_SetTrimmingParamsCore(x1, y1, x2, y2);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_GetTrimmingParams

  Description:  Get camera trimming parameters
                NOTE: widht = x2 - x1;  height = y2 - y1;
                NOTE: should call CAMERA_SetTrimming to enable trimming

  Arguments:    x1      X of top-left trimming point (multiple of 2)
                y1      Y of top-left trimming point
                x2      X of bottom-right trimming point (multiple of 2)
                y2      Y of bottom-right trimming point

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_GetTrimmingParamsCore(u16* x1, u16* y1, u16* x2, u16* y2);
SDK_INLINE void CAMERA_GetTrimmingParams(u16* x1, u16* y1, u16* x2, u16* y2)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_GetTrimmingParamsCore(x1, y1, x2, y2);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_SetTrimming

  Description:  set trimming to be enabled/disabled

  Arguments:    enabled     TRUE if set trimming will be enabled

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_SetTrimmingCore( BOOL enabled );
SDK_INLINE void CAMERA_SetTrimming( BOOL enabled )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_SetTrimmingCore(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_IsTrimming

  Description:  get trimming to be enabled/disabled

  Arguments:    None

  Returns:      TRUE if set trimming will be enabled
 *---------------------------------------------------------------------------*/
BOOL CAMERA_IsTrimmingCore( void );
SDK_INLINE BOOL CAMERA_IsTrimming( void )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return CAMERA_IsTrimmingCore();
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_SetOutputFormat

  Description:  set CAMERA output format.

  Arguments:    output      one of CAMERAOutput to set.

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_SetOutputFormatCore( CAMERAOutput output );
SDK_INLINE void CAMERA_SetOutputFormat( CAMERAOutput output )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_SetOutputFormatCore(output);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_GetOutputFormat

  Description:  get CAMERA output format.

  Arguments:    None

  Returns:      one of CAMERAOutput
 *---------------------------------------------------------------------------*/
CAMERAOutput CAMERA_GetOutputFormatCore( void );
SDK_INLINE CAMERAOutput CAMERA_GetOutputFormat( void )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return CAMERA_GetOutputFormatCore();
    }
    return CAMERA_OUTPUT_YUV;
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_GetErrorStatus

  Description:  whether line buffer has occurred some errors or not

  Arguments:    None

  Returns:      TRUE if error has occurred
 *---------------------------------------------------------------------------*/
BOOL CAMERA_GetErrorStatusCore( void );
SDK_INLINE BOOL CAMERA_GetErrorStatus( void )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return CAMERA_GetErrorStatusCore();
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_ClearBuffer

  Description:  clear line buffer and error status

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_ClearBufferCore( void );
SDK_INLINE void CAMERA_ClearBuffer( void )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_ClearBufferCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_SetTransferLines

  Description:  set number of lines to store the buffer at once.

  Arguments:    lines   number of lines

  Returns:      None
 *---------------------------------------------------------------------------*/
void CAMERA_SetTransferLinesCore( int lines );
SDK_INLINE void CAMERA_SetTransferLines( int lines )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        CAMERA_SetTransferLinesCore(lines);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_GetTransferLines

  Description:  get number of lines to load the buffer at once.

  Arguments:    None

  Returns:      number of lines
 *---------------------------------------------------------------------------*/
int CAMERA_GetTransferLinesCore( void );
SDK_INLINE int CAMERA_GetTransferLines( void )
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return CAMERA_GetTransferLinesCore();
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_GetMaxLinesRound

  Description:  rounding CAMERA_GET_MAX_LINES.

  Arguments:    width    width of image
                height   height of image

  Returns:      max lines
 *---------------------------------------------------------------------------*/
int CAMERA_GetMaxLinesRoundCore(u16 width, u16 height);
SDK_INLINE int CAMERA_GetMaxLinesRound(u16 width, u16 height)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return CAMERA_GetMaxLinesRoundCore(width, height);
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         CAMERA_GetBytesAtOnce

  Description:  �J�����o�b�t�@����t���[���f�[�^����M����ۂ̈��̓]���T�C�Y�����߂�

  Arguments:    width    width of image

  Returns:      unit
 *---------------------------------------------------------------------------*/
u32 CAMERA_GetBytesAtOnceCore(u16 width);
SDK_INLINE u32 CAMERA_GetBytesAtOnce(u16 width)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return CAMERA_GetBytesAtOnceCore(width);
    }
    return 0;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

/* TWL_CAMERA_CAMERA_H_ */
#endif
