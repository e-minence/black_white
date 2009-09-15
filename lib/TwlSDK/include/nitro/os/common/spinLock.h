/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     spinLock.h

  Copyright 2003-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_SPINLOCK_H_
#define NITRO_OS_SPINLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>

#ifndef SDK_TWL
#ifdef	SDK_ARM9
#include	<nitro/hw/ARM9/mmap_global.h>
#else  //SDK_ARM7
#include	<nitro/hw/ARM7/mmap_global.h>
#endif
#else // SDK_TWL
#include    <twl/hw/common/mmap_shared.h>
#ifdef	SDK_ARM9
#include	<twl/hw/ARM9/mmap_global.h>
#else  //SDK_ARM7
#include	<twl/hw/ARM7/mmap_global.h>
#endif
#endif //SDK_TWL

//======================================================================
//                      ���b�N�ϐ�
//
//�E�J�[�h�^�J�[�g���b�W�^�b�o�t�ԋ��L�������[�N�q�`�l�^�u�q�`�l�|�b�^�c
//  �Ȃǂ̂b�o�t�Ԃł̋��L���X�[�X��A�b�o�t�ԑ��M�e�h�e�n ���̐�L���\�[�X
//  �̔r�����䂽�߂ɂ��̊֐������g�p�������B
//======================================================================

// ���b�N�h�c

#define OS_UNLOCK_ID            0      // ���b�N�ϐ������b�N����Ă��Ȃ��ꍇ�̂h�c
#define OS_MAINP_LOCKED_FLAG    0x40   // ���C���v���Z�b�T�ɂ�郍�b�N�m�F�t���O
#define OS_MAINP_LOCK_ID_START  0x40   // ���C���v���Z�b�T�p���b�N�h�c�̊��蓖�ĊJ�n�ԍ�
#define OS_MAINP_LOCK_ID_END    0x6f   //                               ���蓖�ďI���ԍ�
#define OS_MAINP_DBG_LOCK_ID    0x70   //                               �f�o�b�K�\��ԍ�
#define OS_MAINP_SYSTEM_LOCK_ID 0x7f   //                               �V�X�e���\��ԍ�
#define OS_SUBP_LOCKED_FLAG     0x80   //   �T�u�v���Z�b�T�ɂ�郍�b�N�m�F�t���O
#define OS_SUBP_LOCK_ID_START   0x80   //   �T�u�v���Z�b�T�p���b�N�h�c�̊��蓖�ĊJ�n�ԍ�
#define OS_SUBP_LOCK_ID_END     0xaf   //                               ���蓖�ďI���ԍ�
#define OS_SUBP_DBG_LOCK_ID     0xb0   //                               �f�o�b�K�\��ԍ�
#define OS_SUBP_SYSTEM_LOCK_ID  0xbf   //                               �V�X�e���\��ԍ�

#define OS_LOCK_SUCCESS         0      // ���b�N����
#define OS_LOCK_ERROR           (-1)   // ���b�N�G���[

#define OS_UNLOCK_SUCCESS       0      // ���b�N��������
#define OS_UNLOCK_ERROR         (-2)   // ���b�N�����G���[

#define OS_LOCK_FREE            0      // ���b�N������

#define OS_LOCK_ID_ERROR        (-3)   // ���b�N�h�c�G���[


//---- structure of lock variable 
typedef volatile struct OSLockWord
{
    u32     lockFlag;
    u16     ownerID;
    u16     extension;
}
OSLockWord;

/*---------------------------------------------------------------------------*
  Name:         OS_InitLock

  Description:  �V�X�e���̃��b�N�ϐ��⋤�L���\�[�X�ւ̃A�N�Z�X��
                �̐ݒ�����������܂��B

                �J�[�g���b�W�̔r������̂��߂̗̈�̓N���A����܂��B
                (���̗̈���f�o�b�K���g�p���邽��)

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void    OS_InitLock(void);

/*---------------------------------------------------------------------------*
  Name:         OS_LockByWord
                OS_LockCartridge
                OS_LockCard

  Description:  �X�s�����b�N�֐�

                �v���Z�b�T�Ԃ⃂�W���[���Ԃŋ��L���\�[�X��r�����䂷�邽�߂�
                ���b�N�ϐ��̃X�s�����b�N���s���܂��B
                ���b�N����������܂Ŏ��s�������܂��B
                �v���Z�b�T�Ԃ̋��L���\�[�X�͕K�����b�N��Ɏg�p���ĉ������B
                �v���Z�b�T��L�̃��\�[�X�̓^�C�~���O�I�ɒ����ł���̂ł����
                ���b�N���Ȃ��Ă��\���܂���B
                ��L���\�[�X�̓f�o�b�O���Ɍ��肵�ă��b�N���邱�Ƃ��ł��܂��B

  Arguments:    lockID              ���b�NID
                lockp               ���b�N�ϐ��̃|�C���^

  Returns:      >0                  ���b�N����Ă���l
                OS_LOCK_SUCCESS     (=0) ���b�N����
 *---------------------------------------------------------------------------*/
s32     OS_LockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_LockCartridge(u16 lockID);
s32     OS_LockCard(u16 lockID);

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockByWord
                OS_UnlockCartridge
                OS_UnlockCard

  Description:  ���b�N�̉���
                ���b�N���������A���L���\�[�X�̃A�N�Z�X�����T�u�v���Z�b�T�ɓn���܂��B
                ���b�N���Ă��Ȃ����W���[�������s�����ꍇ�ɂ͉������ꂸ�A
                 OS_UNLOCK_ERROR ���Ԃ���܂��B

  Arguments:    lockID              ���b�NID
                lockp               ���b�N�ϐ��̃|�C���^

  Returns:      OS_UNLOCK_SUCCESS   ���b�N��������
                OS_UNLOCK_ERROR     ���b�N�����G���[
 *---------------------------------------------------------------------------*/
s32     OS_UnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_UnlockCartridge(u16 lockID);
s32     OS_UnlockCard(u16 lockID);

//---- �݊����̂��߂ɈȑO�̖��̂̂��̂��c���Ă��܂��B('UnLock' <-> 'Unlock')
//     ISD���C�u�����������̊֐����Ă�ł��܂��B�܂��Ainline �ł͂���܂���B
s32     OS_UnLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_UnLockCartridge(u16 lockID);
s32     OS_UnLockCard(u16 lockID);

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockByWord
                OS_TryLockCartridge
                OS_TryLockCard

  Description:  ���b�N�̎��s

                �P�񂾂��X�s�����b�N�����s���܂��B
                �v���Z�b�T�Ԃ̋��L���\�[�X�͕K�����b�N��Ɏg�p���ĉ������B
                �v���Z�b�T��L�̃��\�[�X�̓^�C�~���O�I�ɒ����ł���̂ł����
                ���b�N���Ȃ��Ă��\���܂���B
                ��L���\�[�X�̓f�o�b�O���Ɍ��肵�ă��b�N���邱�Ƃ��ł��܂��B

  Arguments:    lockID              ���b�NID
                lockp               ���b�N�ϐ��̃|�C���^
                CtrlFuncp           ���\�[�X����֐��̃|�C���^

  Returns:      >0                  ���b�N���i���O�Ɋi�[����Ă���ID�j
                OS_LOCK_SUCCESS     ���b�N����
 *---------------------------------------------------------------------------*/
s32     OS_TryLockByWord(u16 lockID, OSLockWord *lockp, void (*crtlFuncp) (void));
s32     OS_TryLockCartridge(u16 lockID);
s32     OS_TryLockCard(u16 lockID);

/*---------------------------------------------------------------------------*
  Name:         OS_ReadOwnerOfLockWord
                OS_ReadOwnerOfLockCartridge
                OS_ReadOwnerOfLockCard

  Description:  ���b�N�ϐ��̏��L���W���[��ID�̓ǂݍ���

                ���b�N�ϐ��̏��L���W���[��ID ��ǂݍ��݂܂��B
                ���W���[��ID ����0 �̏ꍇ�͂��̎��_�łǂ���̃v���Z�b�T����
                ���L���������Ă���̂����m�F�ł��܂��B
                ���L���\�[�X�̏ꍇ�́u���C���v���Z�b�T�������L���������Ă����ԁv
                �݂̂����荞�݂��֎~���邱�Ƃɂ���Ĉێ����邱�Ƃ��ł��܂��B
                ���̑��̏�Ԃ̓T�u�v���Z�b�T���ω������Ă��܂��\��������܂��B
                ���L���W���[��ID �� 0 �ł����Ă����b�N�ϐ�����������Ă���Ƃ͌���܂���B

                ���A���A���C���������ɑ΂��Ă̓L���b�V���o�R�łȂ����
                �o�C�g�A�N�Z�X�͂ł��Ȃ����Ƃɒ��ӂ��ĉ������B
                �ł��̂ŁA���C����������ł͊�{�I�� OS_ReadOwnerOfLockWord() ���g�p���ĉ������B

  Arguments:    lockp       ���b�N�ϐ��̃|�C���^

  Returns:      ���L���W���[��ID
 *---------------------------------------------------------------------------*/
u16     OS_ReadOwnerOfLockWord(OSLockWord *lockp);
#define OS_ReadOwnerOfLockCartridge()  OS_ReadOwnerOfLockWord( (OSLockWord *)HW_CTRDG_LOCK_BUF )
#define OS_ReadOwnerOfLockCard()       OS_ReadOwnerOfLockWord( (OSLockWord *)HW_CARD_LOCK_BUF  )

/*---------------------------------------------------------------------------*
  Name:         OS_GetLockID

  Description:  lockID ���擾���܂��B

  Arguments:    �Ȃ�

  Returns:      OS_LOCK_ID_ERROR  ... ID�擾�Ɏ��s
                (ARM9�̏ꍇ)
                0x40�`0x6f        ... lockID
                (ARM7�̏ꍇ)
                0x80�`0xaf        ... lockID

                ID��48��ނ܂ł������蓖�Ă邱�Ƃ��ł��܂���B
                ���W���[�����ɂĕ����̃��b�N�ϐ��𐧌䂷��ꍇ��
                �ł��邾��1��ID���g�p����悤�ɂ��ĉ������B
 *---------------------------------------------------------------------------*/
s32     OS_GetLockID(void);


/*---------------------------------------------------------------------------*
  Name:         OS_ReleaseLockID

  Description:  ���b�NID��������܂��B

  Arguments:    lockID : ������悤�Ƃ��郍�b�NID

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void    OS_ReleaseLockID(u16 lockID);


#ifdef SDK_TWL
//================================================================
//   private function.
//   �ȉ��� �����֐��Ȃ̂ŃA�v���P�[�V��������͎g�p���Ȃ��ł��������B
#define OSi_SYNCTYPE_SENDER 0
#define OSi_SYNCTYPE_RECVER 1

#define OSi_SYNCVAL_NOT_READY 0
#define OSi_SYNCVAL_READY     1

void OSi_SyncWithOtherProc( int type, void* syncBuf );

static inline void OSi_SetSyncValue( u8 n )
{
	*(vu8*)(HW_INIT_LOCK_BUF+4) = n;
}
static inline u8 OSi_GetSyncValue( void )
{
	return *(vu8*)(HW_INIT_LOCK_BUF+4);
}

#endif


//--------------------------------------------------------------------------------
#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_SPINLOCK_H_ */
#endif
