/*---------------------------------------------------------------------------*
  Project:  TwlSDK - camera - include
  File:     types.h

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef TWL_CAMERA_TYPES_H_
#define TWL_CAMERA_TYPES_H_

#define CAMERA_EXPOSURE_MIN    -5
#define CAMERA_EXPOSURE_MAX     6   // exclude
#define CAMERA_EXPOSURE_NUMS    (CAMERA_EXPOSURE_MAX - CAMERA_EXPOSURE_MIN)

#define CAMERA_SHARPNESS_MIN    -3
#define CAMERA_SHARPNESS_MAX     6  // exclude
#define CAMERA_SHARPNESS_NUMS   (CAMERA_SHARPNESS_MAX - CAMERA_SHARPNESS_MIN)

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

typedef enum
{
    CAMERA_SELECT_NONE  = 0,
    CAMERA_SELECT_IN    = (1<<0),
    CAMERA_SELECT_OUT   = (1<<1),
    CAMERA_SELECT_BOTH  = (CAMERA_SELECT_IN|CAMERA_SELECT_OUT)
}
CAMERASelect;

typedef enum
{
    CAMERA_CONTEXT_A     = (1<<0),
    CAMERA_CONTEXT_B     = (1<<1),
    CAMERA_CONTEXT_BOTH  = (CAMERA_CONTEXT_A|CAMERA_CONTEXT_B)
}
CAMERAContext;

typedef enum
{
    CAMERA_SIZE_VGA,
    CAMERA_SIZE_QVGA,
    CAMERA_SIZE_QQVGA,
    CAMERA_SIZE_CIF,
    CAMERA_SIZE_QCIF,
    CAMERA_SIZE_DS_LCD,
    CAMERA_SIZE_DS_LCDx4,

    CAMERA_SIZE_MAX
}
CAMERASize;

typedef enum
{
    CAMERA_EFFECT_NONE,
    CAMERA_EFFECT_MONO,
    CAMERA_EFFECT_SEPIA,
    CAMERA_EFFECT_NEGATIVE,
    CAMERA_EFFECT_NEGAFILM,
    CAMERA_EFFECT_SEPIA01,

    CAMERA_EFFECT_MAX
}
CAMERAEffect;

typedef enum
{
    CAMERA_FLIP_NONE,
    CAMERA_FLIP_VERTICAL,
    CAMERA_FLIP_HORIZONTAL,
    CAMERA_FLIP_REVERSE,

    CAMERA_FLIP_MAX
}
CAMERAFlip;

typedef enum
{
    CAMERA_PHOTO_MODE_NORMAL,
    CAMERA_PHOTO_MODE_PORTRAIT,
    CAMERA_PHOTO_MODE_LANDSCAPE,
    CAMERA_PHOTO_MODE_NIGHTVIEW,
    CAMERA_PHOTO_MODE_LETTER,

    CAMERA_PHOTO_MODE_MAX
}
CAMERAPhotoMode;

typedef enum
{
    CAMERA_WHITE_BALANCE_AUTO,

    CAMERA_WHITE_BALANCE_3200K,
    CAMERA_WHITE_BALANCE_4150K,
    CAMERA_WHITE_BALANCE_5200K,
    CAMERA_WHITE_BALANCE_6000K,
    CAMERA_WHITE_BALANCE_7000K,

    CAMERA_WHITE_BALANCE_MAX,

    // alias
    CAMERA_WHITE_BALANCE_NORMAL                     = CAMERA_WHITE_BALANCE_AUTO,
    CAMERA_WHITE_BALANCE_TUNGSTEN                   = CAMERA_WHITE_BALANCE_3200K,
    CAMERA_WHITE_BALANCE_WHITE_FLUORESCENT_LIGHT    = CAMERA_WHITE_BALANCE_4150K,
    CAMERA_WHITE_BALANCE_DAYLIGHT                   = CAMERA_WHITE_BALANCE_5200K,
    CAMERA_WHITE_BALANCE_CLOUDY                     = CAMERA_WHITE_BALANCE_6000K,
    CAMERA_WHITE_BALANCE_HORIZON                    = CAMERA_WHITE_BALANCE_6000K,
    CAMERA_WHITE_BALANCE_SHADE                      = CAMERA_WHITE_BALANCE_7000K
}
CAMERAWhiteBalance;

typedef enum
{
    CAMERA_FRAME_RATE_15,
    CAMERA_FRAME_RATE_15_TO_5,
    CAMERA_FRAME_RATE_15_TO_2,
    CAMERA_FRAME_RATE_8_5,
    CAMERA_FRAME_RATE_5,
    CAMERA_FRAME_RATE_20,
    CAMERA_FRAME_RATE_20_TO_5,
    CAMERA_FRAME_RATE_30,
    CAMERA_FRAME_RATE_30_TO_5,
    CAMERA_FRAME_RATE_15_TO_10,
    CAMERA_FRAME_RATE_20_TO_10,
    CAMERA_FRAME_RATE_30_TO_10,

    CAMERA_FRAME_RATE_MAX
}
CAMERAFrameRate;

typedef enum
{
    CAMERA_TEST_PATTERN_DISABLED,
    CAMERA_TEST_PATTERN_COLOR_BAR,
    CAMERA_TEST_PATTERN_NOISE,

    CAMERA_TEST_PATTERN_MAX
}
CAMERATestPattern;

/*===========================================================================*/

#ifdef  __cplusplus
}          /* extern "C" */
#endif

#endif /* TWL_CAMERA_TYPES_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
