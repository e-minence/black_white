/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - demos - wireless-all
  File:     main.c

  Copyright 2006-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#include "common.h"

/*
 * �ȉ��̃��C�����X�@�\�����ׂĎ�������A�v���P�[�V�����̃T���v��.
 *
 * �EDS�_�E�����[�h�v���C�̃N���[���u�[�g
 * �E�f�[�^�V�F�A�����O
 * �E���C�����X�o�R�̃t�@�C���V�X�e���g�p
 *
 * MB���C�u�����̃T���v���̓}���`�u�[�g�@�\���g�����߁A
 * �����ʐM��(�������邢�͗L��)�ǂ����̊J���@�ނ𕡐��K�v�Ƃ��܂��B
 * $NitroSDK/bin/ARM9-TS/Release/ �f�B���N�g���ɂ���
 * mb_child.bin �v���O�������ŏI���@�ɂ�����}���`�u�[�g�q�@�Ɠ�����
 * �@�\��񋟂���T���v���ɂȂ��Ă��܂��̂ŁA
 * ���̃o�C�i�����T���v���v���O�����Ɠ������@�ő��̋@�ނɓǂݍ��݁A
 * �ꏏ�Ɏ��s���Ă��������B
 *
 */

/******************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         NitroMain / TwlMain

  Description:  ���C�����[�`��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef SDK_TWL
void TwlMain(void)
#else
void NitroMain(void)
#endif
{
    /* ���ʂ̃V�X�e������������ */
    InitCommon();

    /* �S�̂̏�ԑJ�� */
    for (;;)
    {
        /* �e�@�Ȃ�DS�_�E�����[�h�v���C���� */
        if (!MB_IsMultiBootChild())
        {
            ExecuteDownloadServer();
        }
        /* ���C�����X�̏������Ɛe�q�ʂ̋N������ */
        if (!MB_IsMultiBootChild())
        {
#if !defined(MBP_USING_MB_EX)
            if (!WH_Initialize())
            {
                OS_Panic("WH_Initialize failed.");
            }
#endif
            WaitWHState(WH_SYSSTATE_IDLE);
            StartWirelessParent();
        }
        else
        {
            if (!WH_Initialize())
            {
                OS_Panic("WH_Initialize failed.");
            }
            WaitWHState(WH_SYSSTATE_IDLE);
            StartWirelessChild();
        }
        /* �f�[�^�V�F�A�����O�̃��C������ */
        ExecuteDataSharing();
        (void)WH_End();
        WaitWHState(WH_SYSSTATE_STOP);
    }

    /* ����͂����֋y�т܂��� */
    OS_Terminate();
}
