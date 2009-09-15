/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     china.h

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-05-28#$
  $Rev: 10639 $
  $Author: mizutani_nakaba $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_ARM9_CHINA_H_
#define NITRO_OS_ARM9_CHINA_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#define OS_BURY_STRING_FORCHINA         "[SDK+NINTENDO:FORCHINA]"
#define OS_BUSY_STRING_LEN_FORCHINA     23

typedef enum
{
    OS_CHINA_ISBN_DISP,
    OS_CHINA_ISBN_NO_DISP,
    OS_CHINA_ISBN_CHECK_ROM
}OSChinaIsbn;

/*---------------------------------------------------------------------------*
  Name:         OS_InitChina

  Description:  SDK �� OS ���C�u����������������B
                �w���n�������ł���A�v���P�[�V������p�B
                OS_Init �֐��̑���ɓ��֐����g�p����B

  Arguments:    isbn    -   ISBN �ԍ����Ɋւ��镶����z����w�肷��B
                            {
                                char    ISBN[ 17 ] ,
                                char    �����o�L��[ 12 ] ,
                                char    �V�o����(��)[ 4 ] ,
                                char    �V�o����(�E)[ 3 ]
                            }

                param   -   ISBN �ԍ��\���Ɋւ��铮����w�肵�܂��B

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_InitChina(const char **isbn, OSChinaIsbn param);

/*---------------------------------------------------------------------------*
  Name:         OS_ShowAttentionChina

  Description:  �����Ή����S�A�y�ђ��Ӊ�ʂ����莞�ԕ\������B
                ���̊֐��̓��[�U�A�v���P�[�V�������ōĒ�`���邱�Ƃ��ł���B

  Arguments:    isbn    -   ISBN �ԍ����Ɋւ��镶����z��B
                param   -   ISBN �ԍ��\���Ɋւ��铮����w�肵�܂��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_ShowAttentionChina(const char **isbn, OSChinaIsbn param);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_OS_ARM9_CHINA_H_ */

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
