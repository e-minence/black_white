/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     systemWork.h

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
#ifndef TWL_OS_COMMON_SYSTEMWORK_H_
#define TWL_OS_COMMON_SYSTEMWORK_H_


/* if include from Other Environment for exsample VC or BCB, */
/* please define SDK_FROM_TOOL                               */
#if !(defined(SDK_WIN32) || defined(SDK_FROM_TOOL))
//
//--------------------------------------------------------------------
#ifndef SDK_ASM

#include        <twl/types.h>
#include        <twl/hw/common/mmap_shared.h>
#ifdef SDK_TWL
#ifdef SDK_ARM9
#include        <twl/hw/ARM9/mmap_global.h>
#else //SDK_ARM7
#include        <twl/hw/ARM7/mmap_global.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_TWL

// �}�E���g�f�o�C�X
typedef enum OSMountDevice {
    OS_MOUNT_DEVICE_SD   = 0,
    OS_MOUNT_DEVICE_NAND = 1,
    OS_MOUNT_DEVICE_MAX  = 2
}OSMountDevice;


// �}�E���g�Ώ�
typedef enum OSMountTarget {
    OS_MOUNT_TGT_ROOT = 0,
    OS_MOUNT_TGT_FILE = 1,
    OS_MOUNT_TGT_DIR  = 2,
    OS_MOUNT_TGT_MAX  = 3
}OSMountTarget;


// �p�[�~�b�V����
typedef enum OSMountPermission {
    OS_MOUNT_USR_X = 0x01,
    OS_MOUNT_USR_W = 0x02,
    OS_MOUNT_USR_R = 0x04
}OSMountPermission;


// ���\�[�X�̔z�u��
typedef enum OSMountResource {
    OS_MOUNT_RSC_MMEM = 0,
    OS_MOUNT_RSC_WRAM = 1
}OSMountResource;


#define OS_MOUNT_PARTITION_MAX_NUM      3           // �}�E���g�\�ȃp�[�e�B�V������Max.No.
#define OS_MOUNT_DRIVE_START            'A'         // �h���C�u���̐擪�i�啶��'A'-'Z'�̂ݎw��\�j
#define OS_MOUNT_DRIVE_END              'Z'         // �h���C�u���̍Ō�
#define OS_MOUNT_ARCHIVE_NAME_LEN       16          // �A�[�J�C�u����Max.length
#define OS_MOUNT_PATH_LEN               64          // �p�X��Max.length
#define OS_MOUNT_INFO_MAX               (size_t)((HW_TWL_FS_BOOT_SRL_PATH_BUF - HW_TWL_FS_MOUNT_INFO_BUF) / sizeof(OSMountInfo))

#define OS_TITLEIDLIST_MAX              118         // �^�C�g��ID���X�g�̍ő�ێ���


// �A�[�J�C�u�}�E���g���\����
typedef struct OSMountInfo {
    u8      drive[ 1 ];
    u8      device : 3;
    u8      target : 2;
    u8      partitionIndex : 2;
    u8      resource : 1;
    u8      userPermission : 3;                         // ���[�U�[��RW�\���ǂ������w�肷��
    u8      rsv_A : 5;
    u8      rsv_B;
    char    archiveName[ OS_MOUNT_ARCHIVE_NAME_LEN ];   // \0�I�[���݂̃T�C�Y
    char    path[ OS_MOUNT_PATH_LEN ];                  // \0�I�[���݂̃T�C�Y
}OSMountInfo;   // 84bytes


// �^�C�g��ID���X�g�\����
typedef struct OSTitleIDList {
	u8		num;
	u8		rsv[ 15 ];
	u8		publicFlag [ 16 ];	// public�Z�[�u�f�[�^�����邩�ǂ����̃t���O
	u8		privateFlag[ 16 ];	// private�Z�[�u�f�[�^�����邩�ǂ����̃t���O
	u8		appJumpFlag[ 16 ];	// �A�v���W�����v�\���ǂ����̃t���O
	u8		sameMakerFlag[ 16 ];	// �������[�J�[���ǂ����̃t���O
	u64		TitleID[ OS_TITLEIDLIST_MAX ];
}OSTitleIDList; // 1024bytes

typedef struct OSHotBootStatus {
	u8		isDisable :1;
	u8		rsv :7;
}OSHotBootStatus;

/*---------------------------------------------------------------------------*
  Name:         OS_GetMountInfo

  Description:  �}�E���g�����擾���܂��B

  Arguments:    �Ȃ�

  Returns:      �}�E���g��񃊃X�g�̐擪�|�C���^��Ԃ��܂��B
 *---------------------------------------------------------------------------*/
static inline const OSMountInfo *OS_GetMountInfo( void )
{
    return (const OSMountInfo *)HW_TWL_FS_MOUNT_INFO_BUF;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetBootSRLPath

  Description:  �������g��SRL�p�X�����擾���܂��B

  Arguments:    �Ȃ�

  Returns:      �������g��SRL�p�X�ւ̃|�C���^��Ԃ��܂��B
 *---------------------------------------------------------------------------*/
static inline const char *OS_GetBootSRLPath( void )
{
    return (const char *)HW_TWL_FS_BOOT_SRL_PATH_BUF;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetTitleId

  Description:  get application's title ID

  Arguments:    None

  Returns:      title ID as u64 value
 *---------------------------------------------------------------------------*/
static inline u64 OS_GetTitleId( void )
{
    return *(u64 *)(HW_TWL_ROM_HEADER_BUF + 0x230);
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetMakerCode

  Description:  get application's MakerCode

  Arguments:    None

  Returns:      maker code as u16 value
 *---------------------------------------------------------------------------*/
static inline u16 OS_GetMakerCode( void )
{
    return *(u16 *)(HW_TWL_ROM_HEADER_BUF + 0x10);
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetSystemMenuVersionInfoContentID

  Description:  �V�X�e�����j���[�o�[�W������contentID�����擾���܂��B

  Arguments:    �Ȃ�

  Returns:      contentID�̐擪�|�C���^��Ԃ��܂��B(NULL�I�[����j
 *---------------------------------------------------------------------------*/
static inline const u8 *OSi_GetSystemMenuVersionInfoContentID( void )
{
    return (const u8 *)HW_SYSM_VER_INFO_CONTENT_ID;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetSystemMenuVersionLastGameCode

  Description:  �V�X�e�����j���[�o�[�W�����̃Q�[���R�[�h�̍ŏIbyte���擾���܂��B

  Arguments:    �Ȃ�

  Returns:      �Q�[���R�[�h�̍ŏIbyte�ւ̃|�C���^��Ԃ��܂��B
 *---------------------------------------------------------------------------*/
static inline u8 OSi_GetSystemMenuVersionInfoLastGameCode( void )
{
    return *(u8 *)HW_SYSM_VER_INFO_CONTENT_LAST_INITIAL_CODE;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_IsEnableHotBoot

  Description:  HotBoot�t���O�Z�b�g�\�H

  Arguments:    �Ȃ�

  Returns:      TRUE : HotBoot�Z�b�g�}�����A�@FALSE�FHotBoot�Z�b�gOK
 *---------------------------------------------------------------------------*/
static inline BOOL OSi_IsEnableHotBoot( void )
{
	return ( (OSHotBootStatus *)HW_SYSM_DISABLE_SET_HOTBOOT )->isDisable ? 0 : 1;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_SetEnableHotBoot

  Description:  HotBoot�t���O�Z�b�g�ې���

  Arguments:    isEnable -> TRUE : HotBoot�Z�b�gOK�A�@FALSE�FHotBoot�Z�b�g�}��

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
static inline void OSi_SetEnableHotBoot( BOOL isEnable )
{
	( (OSHotBootStatus *)HW_SYSM_DISABLE_SET_HOTBOOT )->isDisable = isEnable ? 0 : 1;
}


#endif // SDK_TWL

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // SDK_ASM

#endif // SDK_FROM_TOOL

/* TWL_OS_COMMON_SYSTEMWORK_H_ */
#endif
