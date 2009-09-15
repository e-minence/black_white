/*---------------------------------------------------------------------------*
  Project:  TwlSDK - screenshot test - Ext
  File:     ext_keycontrol.h

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
#ifndef EXT_KEYCONTROL_H_
#define EXT_KEYCONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    u16     key;                       // �L�[���
    u16     count;                     // �L�[��Ԃ�ێ����鎞�ԁB0�Ȃ�ΏI�[�Ƃ݂Ȃ�
}
EXTKeys;

void    EXT_AutoKeys(const EXTKeys *sequence, u16 *cont, u16 *trig);


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif /* EXT_KEYCONTROL_H_ */
