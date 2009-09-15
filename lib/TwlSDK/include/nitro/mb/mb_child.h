/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - include
  File:     mb_child.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MB_MB_CHILD_H_
#define NITRO_MB_MB_CHILD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/mb/mb.h>

/* ---------------------------------------------------------------------

        �^��`

   ---------------------------------------------------------------------*/

//---------------------------------------------------------
// �q�@�_�E�����[�h�̏��
//---------------------------------------------------------

typedef enum
{
    MB_COMM_CSTATE_NONE,               // �������O���
    MB_COMM_CSTATE_INIT_COMPLETE,      // MB�q�@�������������
    MB_COMM_CSTATE_CONNECT,            // �e�@�ւ̐ڑ��������������
    MB_COMM_CSTATE_CONNECT_FAILED,     // �e�@�ւ̐ڑ������s�������
    MB_COMM_CSTATE_DISCONNECTED_BY_PARENT,      // �e�@����ؒf���ꂽ���
    MB_COMM_CSTATE_REQ_ENABLE,         // MP���m�����ăf�[�^���N�G�X�g���\�ɂȂ������
    MB_COMM_CSTATE_REQ_REFUSED,        // ���N�G�X�g�ɑ΂��Đe�@����KICK���ꂽ���
    MB_COMM_CSTATE_DLINFO_ACCEPTED,    // �e�@�Ƀ��N�G�X�g���󗝂��ꂽ���
    MB_COMM_CSTATE_RECV_PROCEED,       // �_�E�����[�h�f�[�^�̎�M�J�n���
    MB_COMM_CSTATE_RECV_COMPLETE,      // �_�E�����[�h�f�[�^�̎�M�������
    MB_COMM_CSTATE_BOOTREQ_ACCEPTED,   // �e�@����̃u�[�g�v����M���
    MB_COMM_CSTATE_BOOT_READY,         // �e�@�Ƃ̒ʐM��ؒf���ău�[�g�����������
    MB_COMM_CSTATE_CANCELED,           // �r���ŃL�����Z������A�e�@�Ƃ̐ؒf�������������
    MB_COMM_CSTATE_CANCELLED = MB_COMM_CSTATE_CANCELED,
    MB_COMM_CSTATE_AUTHENTICATION_FAILED,       // 
    MB_COMM_CSTATE_MEMBER_FULL,        // �e�@�ւ̃G���g���[��������𒴂��Ă����ꍇ�̏��
    MB_COMM_CSTATE_GAMEINFO_VALIDATED, // �e�@�̃r�[�R����M���
    MB_COMM_CSTATE_GAMEINFO_INVALIDATED,        // ���Ɏ擾���Ă����e�@�̃r�[�R���̏�Ԃ��s���S�ȏ�ԂɂȂ����ꍇ
    MB_COMM_CSTATE_GAMEINFO_LOST,      // �e�@�̃r�[�R��������������Ԃ̒ʒm
    MB_COMM_CSTATE_GAMEINFO_LIST_FULL, // ����ȏ�e�@���擾�ł��Ȃ��Ȃ�����Ԃ̒ʒm
    MB_COMM_CSTATE_ERROR,              // �r���ŃG���[�������������

    MB_COMM_CSTATE_FAKE_END,           // �t�F�C�N�q�@�̊����ʒm(mb_fake_child�g�p���̂ݑJ�ڂ�����)

    /*  �����g�p�̗񋓒l�ł��B
       ���̏�Ԃɂ͑J�ڂ��܂���B */
    MB_COMM_CSTATE_WM_EVENT = 0x80000000
}
MBCommCState;

#define MBCommCStateCallback    MBCommCStateCallbackFunc
/*  �q�@�C�x���g�ʒm�R�[���o�b�N�^ */
typedef void (*MBCommCStateCallbackFunc) (u32 status, void *arg);


/* ---------------------------------------------------------------------
        �}���`�u�[�g���C�u����(MB)API - �q�@�p
   ---------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         MB_SetRejectGgid

  Description:  ��M�����ۂ��� GGID ���w�肵�܂��B
                enable �� TRUE ���w�肷��ƁA�z�M�e�@�� GGID �� mask �̘_���ς�
                ggid �� mask �̘_���ς���v�����ꍇ�� �z�M�e�@�̃r�[�R���͎�M���Ȃ��悤�ɂȂ�܂��B

  Arguments:    ggid         ���O�Ώۂ� GGID
                mask         GGID �̃}�X�N
                enable       �L���ɂ���Ȃ�� TRUE, �����ɂ���Ȃ�� FALSE.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MB_SetRejectGgid(u32 ggid, u32 mask, BOOL enable);

#ifdef __cplusplus
}
#endif


#endif // NITRO_MB_MB_CHILD_H_
