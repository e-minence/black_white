/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - multiboot
  File:     common.h

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
#if !defined(NITRO_MB_DEMO_MULTIBOOT_DATA_H_)
#define NITRO_MB_DEMO_MULTIBOOT_DATA_H_


#include <nitro.h>
#include <nitro/mb.h>


/*
 * multiboot �f���S�̂̋��ʒ�`.
 */


/******************************************************************************/
/* constant */

/*
 * �Q�[���O���[�v ID.
 *
 * MBGameRegistry �\���̂ɂ����ČX�̃t�@�C����
 * GGID ��ݒ�ł���悤�ɂȂ�������,
 * MB_Init() �ł̎w��͏����I�ɈӖ��𐬂��Ȃ��Ȃ邩������܂���.
 */
#define SDK_MAKEGGID_SYSTEM(num)    (0x003FFF00 | (num))
#define MY_GGID         SDK_MAKEGGID_SYSTEM(0x20)

/*
 * �e�@���z�M����`�����l��.
 *
 * �}���`�u�[�g�q�@�͑S�Ẳ\�ȃ`�����l�������񂵂܂��̂�
 * WM ���C�u�����������l(���� 1, 7, 13)�̂����ꂩ���g�p����悤�ɂ��Ă�������.
 *
 * ���[�U�A�v���P�[�V�����ɂ����ĒʐM�̍��G�������ꍇ�͉ςɂ��܂�.
 * �`�����l���ύX�̃^�C�~���O�ɂ��Ă̓A�v���P�[�V�����̍ٗʂɂȂ�܂���,
 * �Ⴆ��, ���X�|���X�̈����ɑ΂����[�U���u�ăX�^�[�g�v�������Ȃǂ�
 * �_�@��݂���Ƃ��������@���l�����܂�.
 */
#define PARENT_CHANNEL  13

/* �\������q�@���X�g�ő吔 */
#define DISP_LIST_NUM   (15)

/* MB ���C�u�����Ɋ��蓖�Ă� DMA �ԍ� */
#define MB_DMA_NO       2

/* ���̃f�����o�^����Q�[������ */
#define GAME_PROG_MAX   2

/* ���̃f�����񋟂���}���`�u�[�g�Q�[���v���O������� */
extern const MBGameRegistry mbGameList[GAME_PROG_MAX];

/* ���̃f���̃Q�[���V�[�P���X��Ԓl */
enum
{
    STATE_NONE,
    STATE_MENU,
    STATE_MB_PARENTINIT,
    STATE_MB_PARENT
};


/******************************************************************************/
/* variable */

/* MB ���C�u�����֊��蓖�Ă郏�[�N�̈� */
extern u32 cwork[MB_SYSTEM_BUF_SIZE / sizeof(u32)];

/* ���̃f���̃Q�[���V�[�P���X��� */
extern u8 prog_state;


/******************************************************************************/
/* function */

/* �L�[�g���K���o */
u16     ReadKeySetTrigger(u16 keyset);

/* min - max �͈̔͂�val�l�� offset�����[�e�[�V���� */
BOOL    RotateU8(u8 *val, u8 min, u8 max, s8 offset);

/* �e�@������ */
void    ParentInit(void);

/* 1 �t���[�����Ƃ̐e�@���C������ */
void    ParentMain(void);

/* �t�@�C���o�b�t�@�|�C���^�������� */
void    InitFileBuffer(void);

/* �e�@MP���M�T�C�Y���Z�b�g */
void    SetParentSendSize(u16 p_size);

/* �e�@MP���M�T�C�Y���擾 */
u16     GetParentSendSize(void);

/* �q�@�ő�ڑ������Z�b�g */
void    SetMaxEntry(u8 num);

/* �q�@�ő�ڑ������擾 */
u8      GetMaxEntry(void);

/* �e�@�̃I�[�g����ݒ� */
void    SetAutoOperation(u8 fAccept, u8 fSend, u8 fBoot);

#endif /* !defined(NITRO_MB_DEMO_MULTIBOOT_DATA_H_) */
