/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - 
  File:     g3_util.h

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

#ifndef NITRO_G3_UTIL_H_
#define NITRO_G3_UTIL_H_

#include <nitro/gx/gxcommon.h>
#include <nitro/gx/g3.h>
#include <nitro/fx/fx_const.h>
#include <nitro/fx/fx_mtx43.h>
#include <nitro/fx/fx_mtx44.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------
void    G3i_FrustumW_(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, BOOL draw,
                      MtxFx44 *mtx);
void    G3i_PerspectiveW_(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f, fx32 scaleW,
                          BOOL draw, MtxFx44 *mtx);
void    G3i_OrthoW_(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, BOOL draw,
                    MtxFx44 *mtx);
void    G3i_LookAt_(const VecFx32 *camPos, const VecFx32 *camUp, const VecFx32 *target, BOOL draw,
                    MtxFx43 *mtx);
void    G3_RotX(fx32 s, fx32 c);
void    G3_RotY(fx32 s, fx32 c);
void    G3_RotZ(fx32 s, fx32 c);
void    G3_LoadTexMtxTexCoord(const MtxFx44 *mtx);
void    G3_LoadTexMtxEnv(const MtxFx44 *mtx);

void    G3BS_LoadTexMtxTexCoord(GXDLInfo *info, const MtxFx44 *mtx);
void    G3CS_LoadTexMtxTexCoord(GXDLInfo *info, const MtxFx44 *mtx);
void    G3B_LoadTexMtxTexCoord(GXDLInfo *info, const MtxFx44 *mtx);
void    G3C_LoadTexMtxTexCoord(GXDLInfo *info, const MtxFx44 *mtx);
void    G3BS_LoadTexMtxEnv(GXDLInfo *info, const MtxFx44 *mtx);
void    G3B_LoadTexMtxEnv(GXDLInfo *info, const MtxFx44 *mtx);
void    G3CS_LoadTexMtxEnv(GXDLInfo *info, const MtxFx44 *mtx);
void    G3C_LoadTexMtxEnv(GXDLInfo *info, const MtxFx44 *mtx);

//----------------------------------------------------------------------------
// Implementation of inline function
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         G3_Frustum

  Description:  Computes a 4x4 perspective projection matrix from a specified
                view volume, and Load it to the current projection matrix.

  Arguments:    t            top coordinate of view volume at the near clipping plane
                b            bottom coordinate of view volume at the near clipping plane
                l            left coordinate of view volume at near clipping plane
                r            right coordinate of view volume at near clipping plane
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void G3_Frustum(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, MtxFx44 *mtx)
{
    G3i_FrustumW_(t, b, l, r, n, f, FX32_ONE, TRUE, mtx);
    // GX_MTXMODE_PROJECTION now
}


/*---------------------------------------------------------------------------*
  Name:         G3_Perspective

  Description:  Computes a 4x4 perspective projection matrix from field of
                view and aspect ratio, and Load it to the current projection
                matrix.

  Arguments:    fovySin      sine value of fovy
                fovyCos      cosine value of fovy
                aspect       ratio of view window width:height (X / Y)
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
static  inline
    void G3_Perspective(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f, MtxFx44 *mtx)
{
    G3i_PerspectiveW_(fovySin, fovyCos, aspect, n, f, FX32_ONE, TRUE, mtx);
    // GX_MTXMODE_PROJECTION now
}


/*---------------------------------------------------------------------------*
  Name:         G3_Ortho

  Description:  Computes a 4x4 orthographic projection matrix, and Load it
                to the current projection matrix.

  Arguments:    t            top coordinate of parallel view volume
                b            bottom coordinate of parallel view volume
                l            left coordinate of parallel view volume
                r            right coordinate of parallel view volume
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void G3_Ortho(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, MtxFx44 *mtx)
{
    G3i_OrthoW_(t, b, l, r, n, f, FX32_ONE, TRUE, mtx);
    // GX_MTXMODE_PROJECTION now
}


/*---------------------------------------------------------------------------*
  Name:         G3_FrustumW

  Description:  Computes a 4x4 perspective projection matrix from a specified
                view volume, and Load it to the current projection matrix.

  Arguments:    t            top coordinate of view volume at the near clipping plane
                b            bottom coordinate of view volume at the near clipping plane
                l            left coordinate of view volume at near clipping plane
                r            right coordinate of view volume at near clipping plane
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void
G3_FrustumW(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, MtxFx44 *mtx)
{
    G3i_FrustumW_(t, b, l, r, n, f, scaleW, TRUE, mtx);
    // GX_MTXMODE_PROJECTION now
}


/*---------------------------------------------------------------------------*
  Name:         G3_PerspectiveW

  Description:  Computes a 4x4 perspective projection matrix from field of
                view and aspect ratio, and Load it to the current projection
                matrix.

  Arguments:    fovySin      sine value of fovy
                fovyCos      cosine value of fovy
                aspect       ratio of view window width:height (X / Y)
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                scaleW       W scale parameter that adjust precision of view volume.
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void
G3_PerspectiveW(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f, fx32 scaleW, MtxFx44 *mtx)
{
    G3i_PerspectiveW_(fovySin, fovyCos, aspect, n, f, scaleW, TRUE, mtx);
    // GX_MTXMODE_PROJECTION now
}


/*---------------------------------------------------------------------------*
  Name:         G3_OrthoW

  Description:  Computes a 4x4 orthographic projection matrix, and Load it
                to the current projection matrix.

  Arguments:    t            top coordinate of parallel view volume
                b            bottom coordinate of parallel view volume
                l            left coordinate of parallel view volume
                r            right coordinate of parallel view volume
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                scaleW       W scale parameter that adjust precision of view volume.
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void
G3_OrthoW(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, MtxFx44 *mtx)
{
    G3i_OrthoW_(t, b, l, r, n, f, scaleW, TRUE, mtx);
    // GX_MTXMODE_PROJECTION now
}


/*---------------------------------------------------------------------------*
  Name:         G3_LookAt

  Description:  Computes a matrix to transform points to camera coordinates,
                and load it to the current position/vector matrix.

  Arguments:    camPos       camera position
                camUp        camera 'up' direction
                target       camera aim point
                mtx          a 4x3 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void
G3_LookAt(const VecFx32 *camPos, const VecFx32 *camUp, const VecFx32 *target, MtxFx43 *mtx)
{
    G3i_LookAt_(camPos, camUp, target, TRUE, mtx);
    // GX_MTXMODE_POSITION_VECTOR now
}


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
