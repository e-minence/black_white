/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tools - defval
  File:     defval.h

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef	 DEFVAL_H_
#define	 DEFVAL_H_

#include <stdio.h>
#include <stdlib.h>                    // getenv()
#include <ctype.h>                     // isspace()
#include <string.h>                    // strlen(), strcasecmp()
#include <getopt.h>                    // getopt()
#include "misc.h"                      // BOOL

//----------------------------------------------------------------------------
//  定義値保持リスト
//
typedef struct tDefineValue
{
    struct tDefineValue *next;
    char   *name;
    char   *value;
}
tDefineValue;

//----------------------------------------------------------------------------
// set_defval.c
//
extern tDefineValue *gDefineValueTop;
BOOL    add_dvalue_by_equality(const char *equality);
BOOL    add_dvalue_from_file(const char *filename);

//----------------------------------------------------------------------------
// get_defval.c
//
tDefineValue *get_dvalue_listptr(const char *name);
BOOL    puts_modified_dvalue(FILE * fp, const char *str);

#endif //DEFVAL_H_
