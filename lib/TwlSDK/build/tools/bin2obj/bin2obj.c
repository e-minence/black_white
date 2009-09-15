/*---------------------------------------------------------------------------*
  Project:  TwlSDK - bin2obj
  File:     bin2obj.c

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include "bin2obj.h"

/*---------------------------------------------------------------------------*
  Name:         main

  Description:  bin2obj ���C��
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    Bin2ObjArgs args;
    BOOL    result;

    cook_args(&args, argc, argv);
    result = bin2obj(&args);
    free_args(&args);

    return result ? 0 : 1;
}

/*---------------------------------------------------------------------------*
  Name:         bin2obj

  Description:  �I�u�W�F�N�g�����o�͂���

  Arguments:    t     ������� bin2obj ����
  
  Returns:      TRUE ����  FALSE ���s
 *---------------------------------------------------------------------------*/
BOOL bin2obj(const Bin2ObjArgs * t)
{
    Object  object;

    //
    // �I�u�W�F�N�g�f�[�^�쐬
    //
    object_init(&object, t->machine, t->endian);

    if (!add_datasec(&object,
                     t->section_rodata, t->section_rwdata,
                     t->symbol_begin, t->symbol_end, t->binary_filename, t->writable, t->align))
    {
        return FALSE;
    }

    map_section(&object);

    //
    // �I�u�W�F�N�g�o��
    //
    if (!output_object(&object, t->object_filename))
    {
        return FALSE;
    }

    return TRUE;
}
