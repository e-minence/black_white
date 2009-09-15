/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - demos - UnitTours/DEMOLib
  File:     DEMOHostio.h

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-05-08 #$
  $Rev: 5892 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/
#ifndef DEMO_HOSTIO_H_
#define DEMO_HOSTIO_H_

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         DEMOMountHostIO

  Description:  IS-TWL-DEBUGGER��HostI/O�t�@�C���V�X�e����
                "hostio:" �Ƃ������O��FS�Ƀ}�E���g����B

  Arguments:    basepath    ��ƂȂ�f�o�b�O�z�X�g���̃��[�g�f�B���N�g���B
                            FS�̃p�X���� "hostio:/relpath" �Ǝw�肵���ꍇ
                            �z�X�g���� "basepath/relpath" ���Ӗ�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOMountHostIO(const char *basepath);


#ifdef __cplusplus
}/* extern "C" */
#endif

/* DEMO_HOSTIO_H_ */
#endif
