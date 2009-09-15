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
#ifndef NITRO_OS_COMMON_SYSTEMWORK_H_
#define NITRO_OS_COMMON_SYSTEMWORK_H_


/* if include from Other Environment for exsample VC or BCB, */
/* please define SDK_FROM_TOOL                               */
#if !(defined(SDK_WIN32) || defined(SDK_FROM_TOOL))
//
//--------------------------------------------------------------------
#ifndef SDK_ASM
#include        <nitro/types.h>

#ifndef SDK_TWL
#include        <nitro/hw/common/mmap_shared.h>
#else //SDK_TWL
#include        <twl/hw/common/mmap_shared.h>
#endif

#include        <nitro/os/common/thread.h>
#include        <nitro/os/common/spinLock.h>
#include        <nitro/os/common/arena.h>

typedef union
{
    u32     b32;
    u16     b16;
}
OSDmaClearSrc;
typedef struct
{
    u8      bootCheckInfo[0x20];       // 000-01f:   32byte boot check info
    u32     resetParameter;            // 020-023:    4byte reset parameter
#ifdef SDK_TWL
    u8      bootSync[0x8];             // 024-02c:    8byte boot sync
#else
    u8      padding5[0x8];             // 024-02c:   (8byte)
#endif
    u32     romBaseOffset;             // 02c-02f:    4byte ROM offset of own program
    u8      cartridgeModuleInfo[12];   // 030-03b:   12byte cartridge module info
    u32     vblankCount;               // 03c-03f:    4byte V�u�����N�J�E���g
    u8      wmBootBuf[0x40];           // 040-07f:   64byte WM �̃}���`�u�[�g�p�o�b�t�@
#ifdef SDK_TWL
    u8      nvramUserInfo[0xe8];       // 080-168: 232bytes NVRAM user info
	u8		HW_secure_info[0x18];      // 18b-17f:  24bytes NAND �{�̃Z�L���A���(flags, validLangBitmap, region, serialNo, )
#else
    u8      nvramUserInfo[0x100];      // 080-17f: 256bytes NVRAM user info
#endif
    u8      isd_reserved1[0x20];       // 180-19f:  32bytes ISDebugger �\��
    u8      arenaInfo[0x48];           // 1a0-1e7:  72bytte �A���[�i���
    u8      real_time_clock[8];        // 1e8-1ef:   8bytes RTC
    u8      sys_conf[6];               // 1f0-1f5:   6bytes System config
    u8      printWindowArm9;           // 1f6-1f6:    1byte debug print window for ARM9
    u8      printWindowArm7;           // 1f7-1f7:    1byte debug print window for ARM7
    u8      printWindowArm9Err;        // 1f8-1f8:    1byte debug print window for ARM9 error
    u8      printWindowArm7Err;        // 1f9-1f9:    1byte debug print window for ARM7 error
#ifdef SDK_TWL
    u8      nandFirmHotStartFlag;      // 1fa-1fa:    1byte HotStartFlag from NAND frimware
    u8      REDLauncherVersion;        // 1fb-1fb:    1byte RED launcher version
    u32     preloadParameterAddr;      // 1fc-1ff:   4bytes preload Parameter Address
#else
    u8      padding1[6];               // 1fa-1ff:   (6byte)
#endif
    u8      rom_header[0x160];         // 200-35f: 352bytes ROM ���o�^�G���A���ޔ��o�b�t�@
    u8      isd_reserved2[32];         // 360-37f:  32bytes ISDebugger �\��
    u32     pxiSignalParam[2];         // 380-387:   8bytes Param for PXI Signal
    u32     pxiHandleChecker[2];       // 388-38f:   8bytes Flag  for PXI Command Handler Installed
    u32     mic_last_address;          // 390-393:   4bytes �}�C�N �ŐV�T���v�����O���ʂ̊i�[�A�h���X
    u16     mic_sampling_data;         // 394-395:   2bytes �}�C�N �P�̃T���v�����O����
    u16     wm_callback_control;       // 396-397:   2bytes WM �R�[���o�b�N�����p�p�����[�^
    u16     wm_rssi_pool;              // 398-399:   2bytes WM ��M���x�ɂ�闐����
    u8      ctrdg_SetModuleInfoFlag;   // 39a-39a:   1byte  set ctrdg module info flag
    u8      ctrdg_IsExisting;          // 39b-39b:   1byte  ctrdg exist flag
    u32     component_param;           // 39c-39f:   4bytes Component �����p�p�����[�^
    OSThreadInfo *threadinfo_mainp;    // 3a0-3a3:   4bytes ARM9 �X���b�h���ւ̃|�C���^ �����l0�ł��邱��
    OSThreadInfo *threadinfo_subp;     // 3a4-3a7:   4bytes ARM7 �X���b�h���ւ̃|�C���^ �����l0�ł��邱��
    u16     button_XY;                 // 3a8-3a9:   2bytes XY �{�^�����i�[�ʒu
    u8      touch_panel[4];            // 3aa-3ad:   4bytes �^�b�`�p�l�����i�[�ʒu
    u16     autoloadSync;              // 3ae-3af:   2bytes autoload sync between processors
    u32     lockIDFlag_mainp[2];       // 3b0-3b7:   8bytes lockID�Ǘ��t���O(ARM9�p)
    u32     lockIDFlag_subp[2];        // 3b8-3bf:   8bytes lockID�Ǘ��t���O(ARM7�p)
    struct OSLockWord lock_VRAM_C;     // 3c0-3c7:   8bytes           �b�E���b�N�o�b�t�@
    struct OSLockWord lock_VRAM_D;     // 3c8-3cf:   8bytes �u�q�`�l�|�c�E���b�N�o�b�t�@
    struct OSLockWord lock_WRAM_BLOCK0; // 3d0-3d7:   8bytes   �u���b�N�O�E���b�N�o�b�t�@
    struct OSLockWord lock_WRAM_BLOCK1; // 3d8-3df:   8bytes �b�o�t�������[�N�q�`�l�E�u���b�N�P�E���b�N�o�b�t�@
    struct OSLockWord lock_CARD;       // 3e0-3e7:   8bytes �J�[�h�E���b�N�o�b�t�@
    struct OSLockWord lock_CARTRIDGE;  // 3e8-3ef:   8bytes �J�[�g���b�W�E���b�N�o�b�t�@
    struct OSLockWord lock_INIT;       // 3f0-3f7:   8bytes ���������b�N�o�b�t�@
    u16     mmem_checker_mainp;        // 3f8-3f9:   2bytes MainMomory Size Checker for Main processor
    u16     mmem_checker_subp;         // 3fa-3fb:   2bytes MainMomory Size Checker for Sub processor
    u8      padding4[2];               // 3fc-3fd: (2bytes)
    u16     command_area;              // 3fe-3ff:   2bytes Command Area
}
OSSystemWork;

#define OS_GetSystemWork()      ((OSSystemWork *)HW_MAIN_MEM_SYSTEM)


#ifdef SDK_TWL
typedef struct
{
    struct OSLockWord lock_WRAM_ex;   // 000-003:   4bytes WRAM-A,B,C �p���b�N�o�b�t�@
    u32    reset_flag;                // 004-007:   4bytes ���Z�b�g�t���O(�n�[�h�E�F�A���Z�b�g)
    u8     padding[0x178];            // 008-17f:  (376bytes)	
}
OSSystemWork2;

#define OS_GetSystemWork2()      ((OSSystemWork2 *)HW_PSEG1_RESERVED_0)
#endif // SDK_TWL

#ifndef SDK_TWL
#define OS_IsCodecTwlMode()      (FALSE)
// (refer to include/twl/os/common/codecmode.h)
#endif

/* �u�[�g��� */
typedef u16 OSBootType;
#define OS_BOOTTYPE_ILLEGAL      0        // �s���ȏ��
#define OS_BOOTTYPE_ROM          1        // ROM ����N��
#define OS_BOOTTYPE_DOWNLOAD_MB  2        // �_�E�����[�h�����A�v�����N��
#define OS_BOOTTYPE_NAND         3        // NAND ���̃A�v�����N��
#define OS_BOOTTYPE_MEMORY       4        // 

/* ���̑��̏��ƕ������\���� */
typedef struct OSBootInfo
{
    OSBootType boot_type;              // 2
    
    // WMBssDesc ���� gameInfo�𔲂�������
    u16     length;                    // 4
    u16     rssi;                      // 6
    u16     bssid[3];                  // 12
    u16     ssidLength;                // 14
    u8      ssid[32];                  // 46
    u16     capaInfo;                  // 48
    struct
    {
        u16     basic;                 // 50
        u16     support;               // 52
    }
    rateSet;
    u16     beaconPeriod;              // 54
    u16     dtimPeriod;                // 56
    u16     channel;                   // 58
    u16     cfpPeriod;                 // 60
    u16     cfpMaxDuration;            // 62
    u16     rsv1;                      // 64
}
OSBootInfo;


/*---------------------------------------------------------------------------*
  Name:         OS_GetBootType

  Description:  �������g�̃u�[�g��ʂ��擾

  Arguments:    �Ȃ�
  
  Returns:      �������g�̃u�[�g��ʂ��w���AOSBootType �^��Ԃ��܂��B
 *---------------------------------------------------------------------------*/
OSBootType OS_GetBootType(void);

/*---------------------------------------------------------------------------*
  Name:         OS_GetBootInfo

  Description:  �������g�̃u�[�g��ʂɊւ�������擾

  Arguments:    �Ȃ�
  
  Returns:      �������g�̃u�[�g��ʂɊւ�������w��
 *---------------------------------------------------------------------------*/
const OSBootInfo *OS_GetBootInfo(void);

#endif // SDK_ASM

#endif // SDK_FROM_TOOL

/* NITRO_OS_COMMON_SYSTEMWORK_H_ */
#endif
