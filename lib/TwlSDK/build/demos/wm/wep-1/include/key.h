/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - demos - wep-1
  File:     key.h

  Copyright 2006-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef __KEY_H__
#define __KEY_H__

#define KEY_REPEAT_START 25            // �L�[���s�[�g�J�n�܂ł̃t���[����
#define KEY_REPEAT_SPAN  10            // �L�[���s�[�g�̊Ԋu�t���[����

typedef struct KeyInfo
{
    u16     cnt;                       // �����H���͒l
    u16     trg;                       // �����g���K����
    u16     up;                        // �����g���K����
    u16     rep;                       // �����ێ����s�[�g����
}
KeyInfo;

extern void updateKeys(void);
extern KeyInfo *getKeyInfo(void);

#endif
