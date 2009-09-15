/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - multiboot-wfs - parent
  File:     main.c

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-08#$
  $Rev: 9555 $
  $Author: kitase_hirotake $
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    WBT ���쓮���Ė����}���`�u�[�g���̎q�@���t�@�C���V�X�e����
    ���p���邽�߂̃T���v���v���O�����ł��B

    HOWTO:
        1. �e�@�Ƃ��ĒʐM���J�n���邽�߂ɂ`�{�^���������ĉ������B
           ���ӂɓ��� multiboot-wfs �f���̎q�@�������莟��A�����I�ɂ��̎q�@��
           �ʐM���J�n���܂��B�����ɒʐM�ł���q�@��15��܂łł��B
        2. �ڑ����̂��܂��܂ȋ@�\�ɂ��ẮA�e�@ / �q�@�̉�ʕ\����
           �\�[�X�R�[�h���̒��߂��Q�Ƃ��������B
 *---------------------------------------------------------------------------*/


#ifdef SDK_TWL
#include    <twl.h>
#else
#include    <nitro.h>
#endif
#include <nitro/wm.h>
#include <nitro/wbt.h>
#include <nitro/fs.h>

#include    "mbp.h"
#include    "wfs.h"
#include    "wh.h"

#include    "util.h"
#include    "common.h"


/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    /* �T���v���p�`��t���[�����[�N������ */
    UTIL_Init();

    /*
     * �t�@�C���V�X�e��������.
     * �e�@�̏ꍇ�ɂ͗L���� DMA �`�����l�����w�肷�邱�Ƃ��\�ł�.
     * �q�@�̏ꍇ�ɂ͎g�p����Ȃ��̂ŒP�ɖ�������܂�.
     */
    FS_Init(FS_DMA_NOT_USE);

    /* LCD�\���J�n */
    GX_DispOn();
    GXS_DispOn();

    /* ��ԑJ�ڃ��C�����[�v */
    for (;;)
    {
        /* �O��� WFS �����łɋN�����ł���Έ�x�I�� */
        if (WFS_GetStatus() != WFS_STATE_STOP)
        {
            WFS_End();
            (void)WH_End();
            while (WH_GetSystemState() != WH_SYSSTATE_STOP) {}
        }

        /* DS �_�E�����[�h�v���C���J�n */
        ModeMultiboot();
        
        /* WH ���W���[���̏����� */ 
        ModeInitialize();
        /* �e�@�I�������܂őҋ@ */
        ModeWorking();
        
        /* ���[�h��ݒ肵�Đe�@���J�n */
        ModeSelect();
        /* �e�@�I�������܂őҋ@ */
        ModeWorking();
        
        /* �N��������e�@��ʂ�\�� */
        ModeParent();
        
        /* �G���[�Ȃ炱���Œ�~ */
        ModeError();
    }
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
