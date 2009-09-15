/*---------------------------------------------------------------------------*
  Project:  TwlSDK - PRC - 
  File:     prc.c

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

#include <nitro.h>
#include <nitro/prc/common.h>

/*===========================================================================*
  Prototype Declarations
 *===========================================================================*/

/*===========================================================================*
  Variable Definitions
 *===========================================================================*/
static BOOL PRCi_Initialized = FALSE;

/*===========================================================================*
  Functions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         PRCi_Init

  Description:  �}�`�F�� API �����������܂�.
                �e�F���A���S���Y���̏��������[�`������ŏ��ɌĂяo����܂�.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void PRCi_Init(void)
{
    if (PRCi_Initialized == TRUE)
        return;

    // Initialize Something...

    PRCi_Initialized = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetEntryStrokes

  Description:  ���{DB�ƌ��{DB�G���g������A�_��f�[�^�𓾂܂�.

  Arguments:    strokes         ����ꂽ�_��f�[�^.
                                ���������Ă͂����܂���.
                prototypeList   ���{�p�^�[���̈ꗗ.
                entry           ���{DB�G���g��.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
PRC_GetEntryStrokes(PRCStrokes *strokes,
                    const PRCPrototypeList *prototypeList, const PRCPrototypeEntry *entry)
{
    if (entry != NULL)
    {
        strokes->points = (PRCPoint *)&prototypeList->pointArray[entry->pointIndex];
        strokes->size = entry->pointCount;
        strokes->capacity = entry->pointCount;
    }
    else
    {
        strokes->points = NULL;
        strokes->size = 0;
        strokes->capacity = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         PRC_CopyStrokes

  Description:  �_��f�[�^���f�B�[�v�R�s�[���܂�.

  Arguments:    srcstrokes         �R�s�[���� PRC_Strokes �\����
                dststrokes         �R�s�[��� PRC_Strokes �\����.

  Returns:      �R�s�[�ł����� TRUE.
 *---------------------------------------------------------------------------*/
BOOL PRC_CopyStrokes(const PRCStrokes *srcStrokes, PRCStrokes *dstStrokes)
{
    int     iPoint, size;
    PRCPoint *dstPoint;
    const PRCPoint *srcPoint;

    SDK_ASSERT(dstStrokes);
    SDK_ASSERT(srcStrokes);

    if (srcStrokes->size > dstStrokes->capacity)
    {
        return FALSE;
    }

    size = dstStrokes->size = srcStrokes->size;
    srcPoint = srcStrokes->points;
    dstPoint = dstStrokes->points;
    for (iPoint = 0; iPoint < size; iPoint++)
    {
        *dstPoint = *srcPoint;
        dstPoint++;
        srcPoint++;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
