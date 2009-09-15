/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - 
  File:     g3b.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <nitro/gx/g3imm.h>
#include <nitro/mi/dma.h>
#include "../include/gxasm.h"

#ifdef SDK_NITRO
#include <nitro/hw/ARM9/ioreg_G3X.h>
#else
#include <twl/hw/ARM9/ioreg_G3X.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         G3_LoadMtx44

  Description:  Loads a 4x4 matrix to the current matrix.

  Arguments:    m            a pointer to a constant 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_LoadMtx44(const MtxFx44 *m)
{
    SDK_NULL_ASSERT(m);
#if 1
    reg_G3X_GXFIFO = G3OP_MTX_LOAD_4x4;
    GX_SendFifo64B(&m->_00, (void *)&reg_G3X_GXFIFO);
#else
    reg_G3_MTX_LOAD_4x4 = (u32)m->_00;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_01;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_02;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_03;

    reg_G3_MTX_LOAD_4x4 = (u32)m->_10;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_11;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_12;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_13;

    reg_G3_MTX_LOAD_4x4 = (u32)m->_20;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_21;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_22;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_23;

    reg_G3_MTX_LOAD_4x4 = (u32)m->_30;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_31;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_32;
    reg_G3_MTX_LOAD_4x4 = (u32)m->_33;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3_LoadMtx43

  Description:  Loads a 4x3 matrix to the current matrix.

  Arguments:    m            a pointer to a constant 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_LoadMtx43(const MtxFx43 *m)
{
    SDK_NULL_ASSERT(m);
#if 1
    reg_G3X_GXFIFO = G3OP_MTX_LOAD_4x3;
    GX_SendFifo48B(&m->_00, (void *)&reg_G3X_GXFIFO);
#else
    reg_G3_MTX_LOAD_4x3 = (u32)m->_00;
    reg_G3_MTX_LOAD_4x3 = (u32)m->_01;
    reg_G3_MTX_LOAD_4x3 = (u32)m->_02;

    reg_G3_MTX_LOAD_4x3 = (u32)m->_10;
    reg_G3_MTX_LOAD_4x3 = (u32)m->_11;
    reg_G3_MTX_LOAD_4x3 = (u32)m->_12;

    reg_G3_MTX_LOAD_4x3 = (u32)m->_20;
    reg_G3_MTX_LOAD_4x3 = (u32)m->_21;
    reg_G3_MTX_LOAD_4x3 = (u32)m->_22;

    reg_G3_MTX_LOAD_4x3 = (u32)m->_30;
    reg_G3_MTX_LOAD_4x3 = (u32)m->_31;
    reg_G3_MTX_LOAD_4x3 = (u32)m->_32;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3_MultMtx44

  Description:  Multiplies the current matrix by a 4x4 matrix from the left.

  Arguments:    m            a pointer to a constant 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_MultMtx44(const MtxFx44 *m)
{
    SDK_NULL_ASSERT(m);
#if 1
    reg_G3X_GXFIFO = G3OP_MTX_MULT_4x4;
    GX_SendFifo64B(&m->_00, (void *)&reg_G3X_GXFIFO);
#else
    reg_G3_MTX_MULT_4x4 = (u32)m->_00;
    reg_G3_MTX_MULT_4x4 = (u32)m->_01;
    reg_G3_MTX_MULT_4x4 = (u32)m->_02;
    reg_G3_MTX_MULT_4x4 = (u32)m->_03;

    reg_G3_MTX_MULT_4x4 = (u32)m->_10;
    reg_G3_MTX_MULT_4x4 = (u32)m->_11;
    reg_G3_MTX_MULT_4x4 = (u32)m->_12;
    reg_G3_MTX_MULT_4x4 = (u32)m->_13;

    reg_G3_MTX_MULT_4x4 = (u32)m->_20;
    reg_G3_MTX_MULT_4x4 = (u32)m->_21;
    reg_G3_MTX_MULT_4x4 = (u32)m->_22;
    reg_G3_MTX_MULT_4x4 = (u32)m->_23;

    reg_G3_MTX_MULT_4x4 = (u32)m->_30;
    reg_G3_MTX_MULT_4x4 = (u32)m->_31;
    reg_G3_MTX_MULT_4x4 = (u32)m->_32;
    reg_G3_MTX_MULT_4x4 = (u32)m->_33;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3_MultMtx43

  Description:  Multiplies the current matrix by a 4x3 matrix from the left.

  Arguments:    m            a pointer to a constant 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_MultMtx43(const MtxFx43 *m)
{
    SDK_NULL_ASSERT(m);
#if 1
    reg_G3X_GXFIFO = G3OP_MTX_MULT_4x3;
    GX_SendFifo48B(&m->_00, (void *)&reg_G3X_GXFIFO);
#else
    reg_G3_MTX_MULT_4x3 = (u32)m->_00;
    reg_G3_MTX_MULT_4x3 = (u32)m->_01;
    reg_G3_MTX_MULT_4x3 = (u32)m->_02;

    reg_G3_MTX_MULT_4x3 = (u32)m->_10;
    reg_G3_MTX_MULT_4x3 = (u32)m->_11;
    reg_G3_MTX_MULT_4x3 = (u32)m->_12;

    reg_G3_MTX_MULT_4x3 = (u32)m->_20;
    reg_G3_MTX_MULT_4x3 = (u32)m->_21;
    reg_G3_MTX_MULT_4x3 = (u32)m->_22;

    reg_G3_MTX_MULT_4x3 = (u32)m->_30;
    reg_G3_MTX_MULT_4x3 = (u32)m->_31;
    reg_G3_MTX_MULT_4x3 = (u32)m->_32;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3_MultMtx33

  Description:  Multiplies the current matrix by a 3x3 matrix from the left.

  Arguments:    m            a pointer to a constant 3x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_MultMtx33(const MtxFx33 *m)
{
    SDK_NULL_ASSERT(m);
#if 1
    reg_G3X_GXFIFO = G3OP_MTX_MULT_3x3;
    GX_SendFifo36B(&m->_00, (void *)&reg_G3X_GXFIFO);
#else
    reg_G3_MTX_MULT_3x3 = (u32)m->_00;
    reg_G3_MTX_MULT_3x3 = (u32)m->_01;
    reg_G3_MTX_MULT_3x3 = (u32)m->_02;

    reg_G3_MTX_MULT_3x3 = (u32)m->_10;
    reg_G3_MTX_MULT_3x3 = (u32)m->_11;
    reg_G3_MTX_MULT_3x3 = (u32)m->_12;

    reg_G3_MTX_MULT_3x3 = (u32)m->_20;
    reg_G3_MTX_MULT_3x3 = (u32)m->_21;
    reg_G3_MTX_MULT_3x3 = (u32)m->_22;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3_Shininess

  Description:  Sets up the shininess table.

  Arguments:    table        a pointer to the shininess data(32 words)

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_Shininess(const u32 *table)
{
#if 1
    SDK_NULL_ASSERT(table);

    reg_G3X_GXFIFO = G3OP_SHININESS;
    GX_SendFifo128B(&table[0], (void *)&reg_G3X_GXFIFO);
#else
    int     i;
    SDK_NULL_ASSERT(table);
    for (i = 0; i < 32; ++i)
    {
        reg_G3_SHININESS = *(table + i);
    }
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3_MultTransMtx33

  Description:  multiple and translate matrix together to current matrix.

  Arguments:    mtx        a constant 3x3 matrix
                trans      a pointer to a constant transration vector.
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_MultTransMtx33(const MtxFx33 *mtx, const VecFx32 *trans)
{
    SDK_NULL_ASSERT(mtx);
    SDK_NULL_ASSERT(trans);

    reg_G3X_GXFIFO = G3OP_MTX_MULT_4x3;
    GX_SendFifo36B(&mtx->_00, (void *)&reg_G3X_GXFIFO);
    reg_G3X_GXFIFO = (u32)trans->x;
    reg_G3X_GXFIFO = (u32)trans->y;
    reg_G3X_GXFIFO = (u32)trans->z;
}
