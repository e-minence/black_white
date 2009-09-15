/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     application_jump.h

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef _APPLICATION_JUMP_H_
#define _APPLICATION_JUMP_H_

#include <twl.h>

#ifdef __cplusplus
extern "C" {
#endif


// define data-------------------------------------------

#define OS_MCU_RESET_VALUE_BUF_HOTBT_MASK		0x00000001
#define OS_MCU_RESET_VALUE_OFS					0

typedef enum OSAppJumpType{
    OS_APP_JUMP_NORMAL = 0,
    OS_APP_JUMP_TMP = 1
}OSAppJumpType;

// TMP�A�v���̃p�X
#define OS_TMP_APP_PATH			"nand:/<tmpjump>"


// function's prototype------------------------------------
#ifdef SDK_ARM9
/*---------------------------------------------------------------------------*
  Name:         OS_JumpToSystemMenu

  Description:  �n�[�h�E�F�A���Z�b�g���s���A�V�X�e�����j���[�ɃW�����v���܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_JumpToSystemMenu( void );

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToInternetSetting

  Description:  �n�[�h�E�F�A���Z�b�g���s���ATWL �{�̐ݒ�̃C���^�[�l�b�g-�ڑ��ݒ�
                �ɃW�����v���܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_JumpToInternetSetting(void);

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToEULAViewer

  Description:  �n�[�h�E�F�A���Z�b�g���s���ATWL �{�̐ݒ�̃C���^�[�l�b�g-���p�K��
                �ɃW�����v���܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_JumpToEULAViewer(void);

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToWirelessSetting

  Description:  �n�[�h�E�F�A���Z�b�g���s���ATWL �{�̐ݒ�� �����ʐM �ɃW�����v���܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_JumpToWirelessSetting(void);

/*---------------------------------------------------------------------------*
  Name:         OS_RebootSystem

  Description:  �n�[�h�E�F�A���Z�b�g���s���A�������g���N�����܂��B
  
  Arguments:    None
 
  Returns:      FALSE �c NITRO ��Ŏ��s�A�������͍ċN�������Ɏ��s����
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_RebootSystem( void );

/*---------------------------------------------------------------------------*
  Name:         OS_IsBootFromSystemMenu

  Description:  �V�X�e�����j���[����N�����ꂽ���𔻒�B
  
  Arguments:    �Ȃ�
 
  Returns:      TRUE �V�X�e�����j���[����N��
                FALSE ����ȊO�̃A�v���P�[�V��������N��
 *---------------------------------------------------------------------------*/
BOOL OS_IsBootFromSystemMenu(void);

/*---------------------------------------------------------------------------*
  Name:         OS_IsTemporaryApplication

  Description:  �������g�� TMP �A�v���ł��邩���`�F�b�N���܂��B
  
  Arguments:    None
 
  Returns:      TRUE  �c TMP �A�v��
                FALSE �c TMP �A�v���ȊO
 *---------------------------------------------------------------------------*/
BOOL OS_IsTemporaryApplication(void);

/*---------------------------------------------------------------------------*
  Name:         OS_IsRebooted

  Description:  OS_RebootSystem �ɂ��ċN�����s��ꂽ�����`�F�b�N���܂�
  
  Arguments:    None
 
  Returns:      TRUE  �c 1��ȏ�ċN�����s��ꂽ
                FALSE �c ����N��
 *---------------------------------------------------------------------------*/
BOOL OS_IsRebooted( void );

// ---- These are internal functions, so don't use for application.
BOOL OS_ReturnToPrevApplication( void );
BOOL OSi_CanApplicationJumpTo( OSTitleId titleID );
OSTitleId OSi_GetPrevTitleId( void );
BOOL OS_DoApplicationJump( OSTitleId id, OSAppJumpType jumpType );
#endif

#ifdef __cplusplus
}       // extern "C"
#endif

#endif  // _APPLICATION_JUMP_H_
