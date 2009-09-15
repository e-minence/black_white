/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FX - 
  File:     fx_mtx43.h

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

#ifndef NITRO_FX_MTX43_H_
#define NITRO_FX_MTX43_H_

#include <nitro/fx/fx.h>
#include <nitro/mi/memory.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

SDK_DECL_INLINE void MTX_Identity43(MtxFx43 *pDst);
SDK_DECL_INLINE void MTX_Copy43(const MtxFx43 *pSrc, MtxFx43 *pDst);
SDK_DECL_INLINE void MTX_Copy43To33(const MtxFx43 *pSrc, MtxFx33 *pDst);
SDK_DECL_INLINE void MTX_Copy43To44(const MtxFx43 *pSrc, MtxFx44 *pDst);
SDK_DECL_INLINE void MTX_Transpose43(const MtxFx43 *pSrc, MtxFx43 *pDst);
void    MTX_TransApply43(const MtxFx43 *pSrc, MtxFx43 *pDst, fx32 x, fx32 y, fx32 z);
SDK_DECL_INLINE void MTX_Scale43(MtxFx43 *pDst, fx32 x, fx32 y, fx32 z);
void    MTX_ScaleApply43(const MtxFx43 *pSrc, MtxFx43 *pDst, fx32 x, fx32 y, fx32 z);
SDK_DECL_INLINE void MTX_RotX43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal);
SDK_DECL_INLINE void MTX_RotY43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal);
SDK_DECL_INLINE void MTX_RotZ43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal);
void    MTX_RotAxis43(MtxFx43 *pDst, const VecFx32 *vec, fx32 sinVal, fx32 cosVal);
int     MTX_Inverse43(const MtxFx43 *pSrc, MtxFx43 *pDst);
void    MTX_Concat43(const MtxFx43 *a, const MtxFx43 *b, MtxFx43 *ab);
void    MTX_MultVec43(const VecFx32 *vec, const MtxFx43 *m, VecFx32 *dst);
void    MTX_LookAt(const VecFx32 *camPos, const VecFx32 *camUp, const VecFx32 *target, MtxFx43 *mtx);

void    MTX_Identity43_(register MtxFx43 *pDst);
void    MTX_Copy43To44_(register const MtxFx43 *pSrc, register MtxFx44 *pDst);
void    MTX_Transpose43_(register const MtxFx43 *pSrc, register MtxFx43 *pDst);
void    MTX_Scale43_(register MtxFx43 *pDst, register fx32 x, register fx32 y, register fx32 z);
void    MTX_RotX43_(register MtxFx43 *pDst, register fx32 sinVal, register fx32 cosVal);
void    MTX_RotY43_(register MtxFx43 *pDst, register fx32 sinVal, register fx32 cosVal);
void    MTX_RotZ43_(register MtxFx43 *pDst, register fx32 sinVal, register fx32 cosVal);

//----------------------------------------------------------------------------
// Implementation of inline function
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         MTX_Identity43

  Description:  Sets a matrix to identity.

  Arguments:    pDst         a pointer to a 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Identity43(MtxFx43 *pDst)
{
    SDK_NULL_ASSERT(pDst);
    MTX_Identity43_(pDst);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_Copy43

  Description:  Copies a 4x3 matrix to a 4x3 matrix.

  Arguments:    pSrc         a pointer to a 4x3 matrix
                pDst         a pointer to a 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Copy43(const MtxFx43 *pSrc, MtxFx43 *pDst)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);
    MI_Copy48B(pSrc, pDst);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_Copy43To33

  Description:  Copies a 4x3 matrix to a 3x3 matrix.

  Arguments:    pSrc         a pointer to a 4x3 matrix
                pDst         a pointer to a 3x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Copy43To33(const MtxFx43 *pSrc, MtxFx33 *pDst)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);
    MI_Copy36B(pSrc, pDst);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_Copy43To44

  Description:  Copies a 4x3 matrix to a 4x4 matrix.

  Arguments:    pSrc         a pointer to a 4x3 matrix
                pDst         a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Copy43To44(const MtxFx43 *pSrc, MtxFx44 *pDst)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);
    MTX_Copy43To44_(pSrc, pDst);
}


/*---------------------------------------------------------------------------*
  Name:         MTX_Transpose43

  Description:  Computes the transpose of a 4x3 matrix.
                Since matrices are 3x4, fourth row is lost and becomes (0,0,0).

  Arguments:    pSrc         a pointer to a 4x3 matrix
                pDst         a pointer to a 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Transpose43(const MtxFx43 *pSrc, MtxFx43 *pDst)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    MTX_Transpose43_(pSrc, pDst);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_Scale43

  Description:  Sets a scaling matrix.

  Arguments:    pDst         a pointer to a 4x3 matrix
                x            x scale factor
                y            y scale factor
                z            z scale factor
  
  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Scale43(MtxFx43 *pDst, fx32 x, fx32 y, fx32 z)
{
    SDK_NULL_ASSERT(pDst);
    MTX_Scale43_(pDst, x, y, z);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_RotX43

  Description:  Sets a rotation matrix about the X axis.

  Arguments:    pDst         a pointer to a 4x3 matrix
                sinVal       sine of an angle of rotation
                cosVal       cosine of an angle of rotation
  
  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_RotX43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal)
{
    SDK_NULL_ASSERT(pDst);
    MTX_RotX43_(pDst, sinVal, cosVal);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_RotY43

  Description:  Sets a rotation matrix about the Y axis.

  Arguments:    pDst         a pointer to a 4x3 matrix
                sinVal       sine of an angle of rotation
                cosVal       cosine of an angle of rotation
  
  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_RotY43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal)
{
    SDK_NULL_ASSERT(pDst);
    MTX_RotY43_(pDst, sinVal, cosVal);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_RotZ43

  Description:  Sets a rotation matrix about the Z axis.

  Arguments:    pDst         a pointer to a 4x3 matrix
                sinVal       sine of an angle of rotation
                cosVal       cosine of an angle of rotation
  
  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_RotZ43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal)
{
    SDK_NULL_ASSERT(pDst);
    MTX_RotZ43_(pDst, sinVal, cosVal);
}


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
