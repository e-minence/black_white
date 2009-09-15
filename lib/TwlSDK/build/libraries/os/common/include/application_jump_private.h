/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     application_jump_private.h

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

#ifndef _APPLICATION_JUMP_PRIVATE_H_
#define _APPLICATION_JUMP_PRIVATE_H_

#include <twl.h>

#ifdef __cplusplus
extern "C" {
#endif


// define data-------------------------------------------

// Hot/Cold�u�[�g����
#define OSi_GetMCUFreeRegisterValue()			( *(vu32*)HW_RESET_PARAMETER_BUF )

#define OS_MCU_RESET_VALUE_BUF_ENABLE_MASK		0x80000000
#define OS_MCU_RESET_VALUE_LEN					1

// �v�����[�h����rom�w�b�_��u���ꏊ
#define OS_TWL_HEADER_PRELOAD_MMEM			0x23e0000
#define OS_TWL_HEADER_PRELOAD_MMEM_END		(0x23e0000 + 0x4000)

// �����`���[�p�����[�^�ɃZ�b�g����u�[�g�^�C�v
typedef enum LauncherBootType {
    LAUNCHER_BOOTTYPE_ILLEGAL = 0,          // �s���ȏ��
    LAUNCHER_BOOTTYPE_ROM,                  // ROM ����N��
    LAUNCHER_BOOTTYPE_TEMP,                 // NAND��tmp�t�H���_���̃A�v�����N��
    LAUNCHER_BOOTTYPE_NAND,                 // NAND ���̃A�v�����N��
    LAUNCHER_BOOTTYPE_MEMORY,               // ��������̃A�v�����N��

	LAUNCHER_BOOTTYPE_MAX
}LauncherBootType;

// �����`���[�p�����[�^�@�t���O
typedef struct LauncherBootFlags {
	u16			isValid : 1;				// TRUE:valid, FALSE:invalid
	u16			bootType : 3;				// LauncherBootType�̒l���g�p
	u16			isLogoSkip : 1;				// ���S�f���X�L�b�v�v��
	u16			isInitialShortcutSkip : 1;	// ����N���V�[�P���X�X�L�b�v�v��
	u16			isAppLoadCompleted : 1;		// �A�v�����[�h�ς݂�����
	u16			isAppRelocate : 1;			// �A�v���Ĕz�u�v��
	u16			rsv : 8;
}LauncherBootFlags;


// �����`���[�p�����[�^�@�w�b�_
typedef struct LauncherParamHeader {
	u32			magicCode;					// SYSM_LAUNCHER_PARAM_MAGIC_CODE������
	u8			version;					// �^�C�v�ɂ����Body�𔻕ʂ���B
	u8			bodyLength;					// body�̒���
	u16			crc16;						// body��CRC16
}LauncherParamHeader;


// �����`���[�p�����[�^�@�{�f�B
typedef union LauncherParamBody {
	struct {								// ���Ƃ肠�����ŏ���TitleProperty�ƃt�H�[�}�b�g�����킹�Ă���
		OSTitleId			prevTitleID;	// ���Z�b�g�O�̃^�C�g��ID
		OSTitleId			bootTitleID;	// ���Z�b�g��Ƀ_�C���N�g�N������^�C�g��ID
		LauncherBootFlags	flags;			// ���Z�b�g���̃����`���[����t���O
		u8					rsv[ 6 ];		// �\��
	}v1;
}LauncherParamBody;


// �����`���[�p�����[�^
typedef struct LauncherParam {
	LauncherParamHeader	header;
	LauncherParamBody	body;
}LauncherParam;

// function's prototype------------------------------------

#ifdef SDK_ARM9
void OSi_InitPrevTitleId( void );
void OS_SetLauncherParamAndResetHardware( OSTitleId id, LauncherBootFlags *flag );


BOOL OSi_IsJumpFromSameMaker(void);
u32 OSi_GetInitialCode(void);
u32 OSi_GetPrevInitialCode(void);
BOOL OSi_CanArbitraryJumpTo(u32 initialCode);
BOOL OSi_JumpToArbitraryApplication(u32 initialCode);
OSTitleId OSi_GetPrevTitleId(void);

#else
BOOL OS_ReadLauncherParameter( LauncherParam *buf, BOOL *isHotstart );
#endif

#ifdef __cplusplus
}       // extern "C"
#endif

#endif  // _APPLICATION_JUMP_PRIVATE_H_
