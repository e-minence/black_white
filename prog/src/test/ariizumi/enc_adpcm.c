//======================================================================
/**
 * @file  enc_adpcm.c
 * @brief ADPCM変換
 * @author  ariizumi
 * @data  09/11/11
 *
 * モジュール名：ENC_ADPCM
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "enc_adpcm.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum


//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct {
    s16 valprev;  /* Previous output value */
    s8  index;    /* Index into stepsize table */
}adpcm_state ;
//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
void adpcm_coder( s16 *indata, s8 *outdata, int len, adpcm_state *state);
void adpcm_decoder( s8 *indata, s16 *outdata, int len,adpcm_state *state);

static adpcm_state adpcmState;

void ENC_ADPCM_ResetParam( void )
{
  adpcmState.valprev = 0;
  adpcmState.index = 0;
}

//--------------------------------------------------------------
//  エンコードメイン
//--------------------------------------------------------------
const u32 ENC_ADPCM_EncodeData( void* src , const u32 srcSize , void* dst )
{
  int i;
  s16 *srcData = src;
  s8 *dstData = dst;
//  adpcm_state state = {0,0};
  
/*
  for( i=0;i<srcSize/2;i++ )
  {
    OS_TFPrintf(2,"%d\n",srcData[i]);
  }
  OS_TFPrintf(2,"------------------------------------------\n");
*/
  adpcm_coder( srcData , dstData , srcSize/2 , &adpcmState );
/*
  for( i=0;i<(srcSize+3)/4;i++ )
  {
    OS_TFPrintf(2,"%d\n",dstData[i]);
  }
  OS_TFPrintf(2,"------------------------------------------\n");
*/  
  return (srcSize+3)/4;
}

//--------------------------------------------------------------
//  デコードメイン
//--------------------------------------------------------------
const u32 ENC_ADPCM_DecodeData( void* src , const u32 srcSize , void* dst )
{
  int i;
  s8 *srcData = src;
  s16 *dstData = dst;
//  adpcm_state state = {0,0};
/*
  for( i=0;i<srcSize;i++ )
  {
    OS_TFPrintf(3,"%d\n",srcData[i]);
  }
  OS_TFPrintf(3,"------------------------------------------\n");
*/
  adpcm_decoder(  srcData , dstData , srcSize*2 , &adpcmState );
/*
  for( i=0;i<srcSize*2;i++ )
  {
    OS_TFPrintf(3,"%d\n",dstData[i]);
  }
  OS_TFPrintf(3,"------------------------------------------\n");
*/
  return srcSize*4;
}


//--------------------------------------------------------------
//  以下転載
//--------------------------------------------------------------
/* Intel ADPCM step variation table */
static int indexTable[16] = 
{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};

static int stepsizeTable[89] = 
{
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};
    
void adpcm_coder( s16 *indata, s8 *outdata, int len, adpcm_state *state)
{
  s16 *inp;     /* Input buffer pointer */
  s8 *outp;   /* output buffer pointer */
  int val;      /* Current input sample value */
  int sign;     /* Current adpcm sign bit */
  int delta;      /* Current adpcm output value */
  int diff;     /* Difference between val and valprev */
  int step;     /* Stepsize */
  int valpred;    /* Predicted output value */
  int vpdiff;     /* Current change to valpred */
  int index;      /* Current step change index */
  int outputbuffer;   /* place to keep previous 4-bit value */
  int bufferstep;   /* toggle between outputbuffer/output */

  outp = (s8 *)outdata;
  inp = indata;

  valpred = state->valprev;
  index = state->index;
  step = stepsizeTable[index];
  
  bufferstep = 1;

  for ( ; len > 0 ; len-- )
  {
    val = *inp++;

    /* Step 1 - compute difference with previous value */
    diff = val - valpred;
    sign = (diff < 0) ? 8 : 0;
    if ( sign )
    {
      diff = (-diff);
    }

    /* Step 2 - Divide and clamp */
    /* Note:
    ** This code *approximately* computes:
    **    delta = diff*4/step;
    **    vpdiff = (delta+0.5)*step/4;
    ** but in shift step bits are dropped. The net result of this is
    ** that even if you have fast mul/div hardware you cannot put it to
    ** good use since the fixup would be too expensive.
    */
    delta = 0;
    vpdiff = (step >> 3);
    
    if ( diff >= step )
    {
        delta = 4;
        diff -= step;
        vpdiff += step;
    }
    step >>= 1;
    if ( diff >= step  )
    {
        delta |= 2;
        diff -= step;
        vpdiff += step;
    }
    step >>= 1;
    if ( diff >= step )
    {
        delta |= 1;
        vpdiff += step;
    }

    /* Step 3 - Update previous value */
    if ( sign )
    {
      valpred -= vpdiff;
    }
    else
    {
      valpred += vpdiff;
    }

    /* Step 4 - Clamp previous value to 16 bits */
    if ( valpred > 32767 )
    {
      valpred = 32767;
    }
    else if ( valpred < -32768 )
    {
      valpred = -32768;
    }

    /* Step 5 - Assemble value, update index and step values */
    delta |= sign;
    
    index += indexTable[delta];
    if ( index < 0 )
    {
      index = 0;
    }
    if ( index > 88 )
    {
      index = 88;
    }
    step = stepsizeTable[index];

    /* Step 6 - Output value */
    if ( bufferstep ) 
    {
        outputbuffer = (delta << 4) & 0xf0;
    }
    else 
    {
        *outp++ = (delta & 0x0f) | outputbuffer;
    }
    bufferstep = !bufferstep;
  }

    /* Output last step, if needed */
    if ( !bufferstep )
      *outp++ = outputbuffer;
    
    state->valprev = valpred;
    state->index = index;
}

void adpcm_decoder( s8 *indata, s16 *outdata, int len,adpcm_state *state)
{
  s8 *inp;   /* Input buffer pointer */
  s16 *outp;    /* output buffer pointer */
  int sign;     /* Current adpcm sign bit */
  int delta;      /* Current adpcm output value */
  int step;     /* Stepsize */
  int valpred;    /* Predicted value */
  int vpdiff;     /* Current change to valpred */
  int index;      /* Current step change index */
  int inputbuffer;    /* place to keep next 4-bit value */
  int bufferstep;   /* toggle between inputbuffer/input */

  outp = outdata;
  inp = (s8*)indata;

  valpred = state->valprev;
  index = state->index;
  step = stepsizeTable[index];

  bufferstep = 0;
  
  for ( ; len > 0 ; len-- )
  {
    
    /* Step 1 - get the delta value */
    if ( bufferstep )
    {
      delta = inputbuffer & 0xf;
    }
    else
    {
      inputbuffer = *inp++;
      delta = (inputbuffer >> 4) & 0xf;
    }
    bufferstep = !bufferstep;

    /* Step 2 - Find new index value (for later) */
    index += indexTable[delta];
    if ( index < 0 )
    {
      index = 0;
    }
    if ( index > 88 )
    {
      index = 88;
    }

    /* Step 3 - Separate sign and magnitude */
    sign = delta & 8;
    delta = delta & 7;

    /* Step 4 - Compute difference and new predicted value */
    /*
    ** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
    ** in adpcm_coder.
    */
    vpdiff = step >> 3;
    if ( delta & 4 )
    {
      vpdiff += step;
    }
    if ( delta & 2 )
    {
      vpdiff += step>>1;
    }
    if ( delta & 1 )
    {
      vpdiff += step>>2;
    }

    if ( sign )
    {
      valpred -= vpdiff;
    }
    else
    {
      valpred += vpdiff;
    }

    /* Step 5 - clamp output value */
    if ( valpred > 32767 )
    {
      valpred = 32767;
    }
    else if ( valpred < -32768 )
    {
      valpred = -32768;
    }

    /* Step 6 - Update step value */
    step = stepsizeTable[index];

    /* Step 7 - Output value */
    *outp++ = valpred;
  }

  state->valprev = valpred;
  state->index = index;
}
