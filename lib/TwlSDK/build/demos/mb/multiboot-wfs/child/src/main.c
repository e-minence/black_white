/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - multiboot-wfs - child
  File:     main.c

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-29#$
  $Rev: 8760 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    WBT ���쓮���Ė����}���`�u�[�g���̎q�@���t�@�C���V�X�e����
    ���p���邽�߂̃T���v���v���O�����ł��B

    HOWTO:
        1. �q�@�Ƃ��ĒʐM���J�n���邽�߂ɂa�{�^���������ĉ������B
           ���ӂɓ��� wbt-fs �f���̐e�@�������莟��A�����I�ɂ��̐e�@��
           �ʐM���J�n���܂��B
        2. �ڑ����̂��܂��܂ȋ@�\�ɂ��ẮA�e�@ / �q�@�̉�ʕ\����
           �\�[�X�R�[�h���̒��߂��Q�Ƃ��������B
 *---------------------------------------------------------------------------*/


#ifdef SDK_TWL
#include	<twl.h>
#else
#include	<nitro.h>
#endif
#include <nitro/wm.h>
#include <nitro/wbt.h>
#include <nitro/fs.h>

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

    /* WH ���W���[���̏����� */ 
    ModeInitialize();

    /* ���C�����[�v */
    for (;;)
    {
        
        /* �O��� WFS �����łɋN�����ł���Έ�x�I�� */
        if (WFS_GetStatus() != WFS_STATE_STOP)
        {
            WFS_End();
            WH_Reset();
        }

        /* �q�@�I�������܂őҋ@ */
        ModeWorking();
        
        /* �q�@���J�n */
        ModeSelect();
        /* �q�@�I�������܂őҋ@ */
        ModeWorking();
        
        /* �N��������q�@��ʂ�\�� */
        ModeChild();
        
        if (WH_GetSystemState() == WH_SYSSTATE_FATAL)
        {
            /* �G���[�Ȃ炱���Œ�~ */
            ModeError();
        }
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
