/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FX - 
  File:     fx_trig.c

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

//-----------------------------------------------------------------------------
//  A Margin of Error:
//  the worst case: about 0.000000025 (sin(1.25pi), cos(1.75pi))
//  average       : about 0.000000001
//
//  |(sin(dx) - FX_SinFx64c(x) / 4294967296.0) / sin(dx)| < 0.00004 in the worst case(around cos(pi/2))
//-----------------------------------------------------------------------------

#include <nitro/fx/fx_trig.h>
#include <nitro/fx/fx_const.h>

#define SDK_FOUR_PI     ((fx64c) 0x0000000145f306ddLL)

#define SDK_SINCOEFF_1  ((u64) 3373259426LL)    // pi/4
#define SDK_SINCOEFF_2  ((u64) 346799334LL)     // ((pi/4)^3)/6
#define SDK_SINCOEFF_3  ((u64) 132467588LL)     // ((pi./4)^2)/20
#define SDK_SINCOEFF_4  ((u64) 63079804LL)      // ((pi/4)^2)/42
#define SDK_SINCOEFF_5  ((u64) 36796552LL)      // ((pi/4)^2)/72

#define SDK_COSCOEFF_1  ((u64) 1324675879LL)    // ((pi/4)^2)/2
#define SDK_COSCOEFF_2  ((u64) 220779313LL)     // ((pi/4)^2)/12
#define SDK_COSCOEFF_3  ((u64) 88311725LL)      // ((pi/4)^2)/30
#define SDK_COSCOEFF_4  ((u64) 47309853LL)      // ((pi/4)^2)/56

static u64 FX_SinFx64c_internal(u64 y);
static u64 FX_CosFx64c_internal(u64 y);

#include <nitro/code32.h>              // Always generate ARM binary for efficiency
// calc: rad - (rad^3)/3! + (rad^5)/5! - (rad^7)/7! + (rad^9)/9!
static u64 FX_SinFx64c_internal(u64 y)
{
    u64     tmp;
    u64     yy;
    if (y == 0x100000000LL)
    {
        return (u64)FX64C_SQRT1_2;
    }
    yy = y * y >> 32;

    tmp = FX64C_ONE - (SDK_SINCOEFF_5 * yy >> 32);
    tmp = FX64C_ONE - ((SDK_SINCOEFF_4 * yy >> 32) * tmp >> 32);
    tmp = FX64C_ONE - ((SDK_SINCOEFF_3 * yy >> 32) * tmp >> 32);
    tmp = SDK_SINCOEFF_1 - ((SDK_SINCOEFF_2 * yy >> 32) * tmp >> 32);

    return tmp * y >> 32;
}

// calc: 1 - (rad^2)/2! + (rad^4)/4! - (rad^6)/6! + (rad^8)/8!
static u64 FX_CosFx64c_internal(u64 y)
{
    u64     tmp;
    u64     yy;
    if (y == 0x100000000LL)
    {
        return (u64)FX64C_SQRT1_2;
    }
    yy = y * y >> 32;

    tmp = FX64C_ONE - (SDK_COSCOEFF_4 * yy >> 32);
    tmp = FX64C_ONE - ((SDK_COSCOEFF_3 * yy >> 32) * tmp >> 32);
    tmp = FX64C_ONE - ((SDK_COSCOEFF_2 * yy >> 32) * tmp >> 32);
    tmp = FX64C_ONE - ((SDK_COSCOEFF_1 * yy >> 32) * tmp >> 32);

    return tmp;
}

#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         FX_SinFx64c

  Description:  Returns sine of 'rad', computing a Taylor series expansion
                of it.

  Arguments:    rad          a value in fx32 format

  Returns:      sine of 'rad' in fx64c format
 *---------------------------------------------------------------------------*/
fx64c FX_SinFx64c(fx32 rad)
{
    fx64c   y;
    fx64c   rval;
    int     n;

    if (rad < 0)
    {
        return -FX_SinFx64c(-rad);
    }
    y = (fx64c)((SDK_FOUR_PI * rad) >> 12);
    n = (int)(y >> 32);
    y = y & 0xffffffff;

    if (n & 1)
    {
        y = 0x100000000LL - y;
    }
    if ((n + 1) & 2)
    {
        rval = (fx64c)FX_CosFx64c_internal((u64)y);
    }
    else
    {
        rval = (fx64c)FX_SinFx64c_internal((u64)y);
    }
    if ((n & 7) > 3)
    {
        rval = -rval;
    }
    return rval;
}


/*---------------------------------------------------------------------------*
  Name:         FX_CosFx64c

  Description:  Returns cosine of 'rad', computing a Taylor series expansion
                of it.

  Arguments:    rad          a value in fx32 format

  Returns:      cosine of 'rad' in fx64c format
 *---------------------------------------------------------------------------*/
fx64c FX_CosFx64c(fx32 rad)
{
    fx64c   y;
    fx64c   rval;
    int     n;

    if (rad < 0)
    {
        return FX_CosFx64c(-rad);
    }
    y = (fx64c)((SDK_FOUR_PI * rad) >> 12);
    n = (int)(y >> 32);
    y = y & 0xffffffff;

    if (n & 1)
    {
        y = 0x100000000LL - y;
    }
    if ((n + 1) & 2)
    {
        rval = (fx64c)FX_SinFx64c_internal((u64)y);
    }
    else
    {
        rval = (fx64c)FX_CosFx64c_internal((u64)y);
    }
    if (((n + 2) & 7) > 3)
    {
        rval = -rval;
    }
    return rval;
}
