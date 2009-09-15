/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include - twl - HW
  File:     mmap_shared.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-31#$
  $Rev: 9171 $
  $Author: yada $
 *---------------------------------------------------------------------------*/
#ifndef TWL_HW_COMMON_MMAP_SHARED_H_
#define TWL_HW_COMMON_MMAP_SHARED_H_
#ifdef  __cplusplus
extern  "C" {
#endif
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------*
    MEMORY MAP of MAIN MEMORY SHARED AREA
 *----------------------------------------------------------------------*/
/*
  4K -+------------------------------+- 0x03000000
      |                              |
      | System Shared                |
      |                              |
  3K -+------------------------------+- 0x02fffc00
      | Download Parameter           |
      +------------------------------+- 0x02fffbe0
      | Card Rom Header              |
      +------------------------------+- 0x02fffa80
      | Unused                       |
      +------------------------------+- 0x02fffa00
      | RED Reserved                 |
  2K -+------------------------------+- 0x02fff800
      | System Shared 2              |
      +------------------------------+- 0x02fff680
      |                              |
      | Shared Arena                 |
      |                              |
   0 -+------------------------------+- 0x02fff000
      | Booted application RomHeader |
 -4K -+------------------------------+- 0x02ffe000
      | Booted application SRL Path  |
     -+------------------------------+- 0x02ffdfc0
      | FS Mount Parameter           |
     -+------------------------------+- 0x02ffdc00
      | TitleID list for system use  |
     -+------------------------------+- 0x02ffd800
      | SD NAND Context buf          |
     -+------------------------------+- 0x02ffd7bc
      | SysMenuVer. Info             |
     -+------------------------------+- 0x02ffd7b3
      | Reserved.                    |
     -+------------------------------+- 0x02ffd000
      | TWL CARD ROM HEADER.         |
-12K -+------------------------------+- 0x02ffc000
*/

/*----------------------------------------------------------------------*
    TWL CARD ROM HEADER
 *----------------------------------------------------------------------*/
#define HW_TWL_CARD_ROM_HEADER_BUF              (HW_MAIN_MEM + 0x00ffc000)
#define HW_TWL_CARD_ROM_HEADER_BUF_SIZE         0x1000
#define HW_TWL_CARD_ROM_HEADER_BUF_END          (HW_TWL_CARD_ROM_HEADER_BUF + HW_TWL_CARD_ROM_HEADER_BUF_SIZE)

/*----------------------------------------------------------------------*
    TWL SHARED RESERVED AREA
 *----------------------------------------------------------------------*/
#define HW_TWL_SHARED_RESERVED					HW_TWL_CARD_ROM_HEADER_BUF_END
#define HW_TWL_SHARED_RESERVED_END				HW_SYSM_VER_INFO_CONTENT_ID

/*----------------------------------------------------------------------*
    SystemMenuVerionInfo content info
 *----------------------------------------------------------------------*/
#define HW_SYSM_VER_INFO_CONTENT_ID					(HW_MAIN_MEM + 0x00ffd7b0 ) // 8bytes��contentID, NULL�I�[����
#define HW_SYSM_VER_INFO_CONTENT_LAST_INITIAL_CODE	(HW_MAIN_MEM + 0x00ffd7b9 )

/*----------------------------------------------------------------------*
    Disable set HotBoot flag
 *----------------------------------------------------------------------*/
#define HW_SYSM_DISABLE_SET_HOTBOOT				(HW_MAIN_MEM + 0x00ffd7ba )

/*----------------------------------------------------------------------*
    Nand context of SD driver
 *----------------------------------------------------------------------*/
#define HW_SD_NAND_CONTEXT_BUF					(HW_MAIN_MEM + 0x00ffd7bc )	// �����`���[������p����SD�h���C�o�pNAND�R���e�L�X�g�f�[�^
#define HW_SD_NAND_CONTEXT_BUF_END				(HW_MAIN_MEM + 0x00ffd800 )

/*----------------------------------------------------------------------*
    TitleID list for system use
 *----------------------------------------------------------------------*/
#define HW_OS_TITLE_ID_LIST                     (HW_MAIN_MEM + 0x00ffd800 ) // ���[�U�[�pTitleID���X�g
#define HW_OS_TITLE_ID_LIST_SIZE                0x400

/*----------------------------------------------------------------------*
    FS PARAMETER from SystemMenu
 *----------------------------------------------------------------------*/
#define HW_TWL_FS_MOUNT_INFO_BUF                (HW_MAIN_MEM + 0x00ffdc00 ) // �}�E���g���i�T�C�Y��0x3c0�m�ہB11�܂Ń}�E���g�w��\�B11*84+1=0x39d�j
#define HW_TWL_FS_BOOT_SRL_PATH_BUF             (HW_MAIN_MEM + 0x00ffdfc0 ) // �N��SRL�̃p�X

/*----------------------------------------------------------------------*
    MEMORY MAP of temporary area for sheltering each extended rom header
 *----------------------------------------------------------------------*/
#define HW_TWL_ROM_HEADER_BUF                   (HW_MAIN_MEM + 0x00ffe000)
#define HW_TWL_ROM_HEADER_BUF_SIZE              0x1000
#define HW_TWL_ROM_HEADER_BUF_END               (HW_TWL_ROM_HEADER_BUF + HW_TWL_ROM_HEADER_BUF_SIZE)

//#define HW_TWL_PRIMARY_ROM_HEADER_BUF           (HW_MAIN_MEM + 0x00ffd000)
//#define HW_TWL_PRIMARY_ROM_HEADER_BUF_SIZE      0x1000
//#define HW_TWL_PRIMARY_ROM_HEADER_BUF_END       (HW_TWL_PRIMARY_ROM_HEADER_BUF + HW_TWL_PRIMARY_ROM_HEADER_BUF_SIZE)

//#define HW_TWL_SECONDARY_ROM_HEADER_BUF         (HW_MAIN_MEM + 0x00ffc000 )
//#define HW_TWL_SECONDARY_ROM_HEADER_BUF_SIZE    0x1000
//#define HW_TWL_SECONDARY_ROM_HEADER_BUF_END     (HW_TWL_SECONDARY_ROM_HEADER_BUF + HW_TWL_SECONDARY_ROM_HEADER_BUF_SIZE)

/*----------------------------------------------------------------------*
    MEMORY MAP of SHARED AREA (4K bytes)
 *----------------------------------------------------------------------*/
#define HW_SHARED_ARENA_LO_DEFAULT      HW_MAIN_MEM_SHARED
#define HW_SHARED_ARENA_SIZE_DEFAULT    (HW_SHARED_ARENA_HI_DEFAULT - HW_SHARED_ARENA_LO_DEFAULT)   // 1664 bytes
#define HW_SHARED_ARENA_HI_DEFAULT      HW_PSEG1_RESERVED_0

#define HW_PSEG1_RESERVED_0             (HW_PSEG1_RESERVED_0_END - HW_PSEG1_RESERVED_0_SIZE)
#define HW_PSEG1_RESERVED_0_SIZE        0x180   // 384 bytes
#define HW_PSEG1_RESERVED_0_END         HW_RED_RESERVED

#define HW_RED_RESERVED                 (HW_MAIN_MEM + 0x00fff800)
#define HW_RED_RESERVED_SIZE            0x200   // 512 bytes
#define HW_RED_RESERVED_END             (HW_RED_RESERVED + HW_RED_RESERVED_SIZE)

#define HW_PSEG1_RESERVED_1             HW_RED_RESERVED_END
#define HW_PSEG1_RESERVED_1_SIZE        (HW_PSEG1_RESERVED_1_END - HW_PSEG1_RESERVED_1) // 128 bytes
#define HW_PSEG1_RESERVED_1_END         HW_CARD_ROM_HEADER

#define HW_CARD_ROM_HEADER              (HW_MAIN_MEM + 0x00fffa80)
#define HW_CARD_ROM_HEADER_SIZE         0x160   // 352 bytes
#define HW_CARD_ROM_HEADER_END          (HW_CARD_ROM_HEADER + HW_CARD_ROM_HEADER_SIZE)

#define HW_DOWNLOAD_PARAMETER           (HW_MAIN_MEM + 0x00fffbe0)
#define HW_DOWNLOAD_PARAMETER_SIZE      0x20    // 32 bytes
#define HW_DOWNLOAD_PARAMETER_END       (HW_DOWNLOAD_PARAMETER + HW_DOWNLOAD_PARAMETER_SIZE)

#define HW_MAIN_MEM_SYSTEM              (HW_MAIN_MEM + 0x00fffc00)
#define HW_MAIN_MEM_SYSTEM_SIZE         (HW_MAIN_MEM_SYSTEM_END - HW_MAIN_MEM_SYSTEM)   // 1024 byte
#define HW_MAIN_MEM_SYSTEM_END          HW_MAIN_MEM_SHARED_END

/*----------------------------------------------------------------------*
    MEMORY MAP of SYSTEM SHARED AREA2 (384 bytes)
 *----------------------------------------------------------------------*/
#define HW_WRAM_EX_LOCK_BUF             (HW_MAIN_MEM + 0x00fff680)      // WRAM lock buffer (END-0x180)
#define HW_WRAM_EX_LOCK_BUF_END         (HW_MAIN_MEM + 0x00fff684)      //                  (END-0x17c)
#define HW_RESET_LOCK_FLAG_BUF          (HW_MAIN_MEM + 0x00fff684)      //                  (END-0x17c)
#define HW_RESET_LOCK_FLAG_BUF_END      (HW_MAIN_MEM + 0x00fff688)      //                  (END-0x17c)

/*----------------------------------------------------------------------*
    MEMORY MAP of SYSTEM SHARED AREA (1K bytes)
 *----------------------------------------------------------------------*/
#define HW_BOOT_CHECK_INFO_BUF          (HW_MAIN_MEM + 0x00fffc00)      // Boot check info (END-0x400)
#define HW_BOOT_CHECK_INFO_BUF_END      (HW_MAIN_MEM + 0x00fffc20)      //                 (END-0x3e0)

#define HW_RESET_PARAMETER_BUF          (HW_MAIN_MEM + 0x00fffc20)      // reset parameter (END-0x3e0)

#define HW_BOOT_SHAKEHAND_9             (HW_MAIN_MEM + 0x00fffc24)      // to shake hand with ARM7 boot sequence
#define HW_BOOT_SHAKEHAND_7             (HW_MAIN_MEM + 0x00fffc26)      // to shake hand with ARM9 boot sequence
#define HW_BOOT_SYNC_PHASE              (HW_MAIN_MEM + 0x00fffc28)      // to synchronize ARM7/ARM9 boot sequences
                                                                        // 0x02fffc2a - 0x02fffc2b reserved

#define HW_ROM_BASE_OFFSET_BUF          (HW_MAIN_MEM + 0x00fffc2c)      // ROM offset of own program (END-0x3d4)
#define HW_ROM_BASE_OFFSET_BUF_END      (HW_MAIN_MEM + 0x00fffc30)      //                           (END-0x3d0)

#define HW_CTRDG_MODULE_INFO_BUF        (HW_MAIN_MEM + 0x00fffc30)      // Cartridge module info (END-0x3d0)
#define HW_CTRDG_MODULE_INFO_BUF_END    (HW_MAIN_MEM + 0x00fffc3c)      //                       (END-0x3c4)

#define HW_VBLANK_COUNT_BUF             (HW_MAIN_MEM + 0x00fffc3c)      // VBlank counter (END-0x3c4)

#define HW_WM_BOOT_BUF                  (HW_MAIN_MEM + 0x00fffc40)      // WM buffer for Multi-Boot (END-0x3c0)
#define HW_WM_BOOT_BUF_END              (HW_MAIN_MEM + 0x00fffc80)      //                          (END-0x380)

#define HW_NVRAM_USER_INFO              (HW_MAIN_MEM + 0x00fffc80)      // NVRAM user info           (END-0x380)
#define HW_NVRAM_USER_INFO_END          (HW_MAIN_MEM + 0x00fffd68)      //                           (END-0x298)
#define HW_HW_SECURE_INFO				(HW_MAIN_MEM + 0x00fffd68)      // �{�̃Z�L���A���          (END-0x298)
#define HW_HW_SECURE_INFO_END			(HW_MAIN_MEM + 0x00fffd80)      //                           (END-0x280)

#define HW_BIOS_EXCP_STACK_MAIN         (HW_MAIN_MEM + 0x00fffd80)      // MAINP�f�o�b�K���j�^��O�n���h�� (END-0x280)
#define HW_BIOS_EXCP_STACK_MAIN_END     (HW_MAIN_MEM + 0x00fffd9c)      //                                 (END-0x264)
#define HW_EXCP_VECTOR_MAIN             (HW_MAIN_MEM + 0x00fffd9c)      // MAINP�pHW_EXCP_VECTOR_BUF       (END-0x264)

#define HW_ARENA_INFO_BUF               (HW_MAIN_MEM + 0x00fffda0)      // �A���[�i���\����(27F_FDA0�`27F_FDE7) (END-0x260)
#define HW_REAL_TIME_CLOCK_BUF          (HW_MAIN_MEM + 0x00fffde8)      // RTC

#define HW_SYS_CONF_BUF                 (HW_MAIN_MEM + 0x00fffdf0)      // System config data (END-0x210)
#define HW_SYS_CONF_BUF_END             (HW_MAIN_MEM + 0x00fffdf6)      //                    (END-0x20a)

#define HW_PRINT_OUTPUT_ARM9            (HW_MAIN_MEM + 0x00fffdf6)      // debug print window for ARM9
#define HW_PRINT_OUTPUT_ARM7            (HW_MAIN_MEM + 0x00fffdf7)      // debug print window for ARM7
#define HW_PRINT_OUTPUT_ARM9ERR         (HW_MAIN_MEM + 0x00fffdf8)      // debug print window for ARM9 error
#define HW_PRINT_OUTPUT_ARM7ERR         (HW_MAIN_MEM + 0x00fffdf9)      // debug print window for ARM7 error

#define HW_NAND_FIRM_HOTSTART_FLAG      (HW_MAIN_MEM + 0x00fffdfa)      // debug print window for ARM7 error
#define HW_TWL_RED_LAUNCHER_VER			(HW_MAIN_MEM + 0x00fffdfb)      // RED�����`���[�o�[�W����
#define HW_PRELOAD_PARAMETER_ADDR       (HW_MAIN_MEM + 0x00fffdfc)      // preload parameter address

#define HW_ROM_HEADER_BUF               (HW_MAIN_MEM + 0x00fffe00)      // �q�n�l���o�^�G���A�f�[�^�E�o�b�t�@ (END-0x200)
#define HW_ROM_HEADER_BUF_END           (HW_MAIN_MEM + 0x00ffff60)      //                                    (END-0x0a0)
#define HW_ISD_RESERVED                 (HW_MAIN_MEM + 0x00ffff60)      // IS DEBUGGER Reserved (END-0xa0)
#define HW_ISD_RESERVED_END             (HW_MAIN_MEM + 0x00ffff80)      //                      (END-0x80)

#define HW_PXI_SIGNAL_PARAM_ARM9        (HW_MAIN_MEM + 0x00ffff80)      // PXI Signal Param for ARM9
#define HW_PXI_SIGNAL_PARAM_ARM7        (HW_MAIN_MEM + 0x00ffff84)      // PXI Signal Param for ARM7
#define HW_PXI_HANDLE_CHECKER_ARM9      (HW_MAIN_MEM + 0x00ffff88)      // PXI Handle Checker for ARM9
#define HW_PXI_HANDLE_CHECKER_ARM7      (HW_MAIN_MEM + 0x00ffff8c)      // PXI Handle Checker for ARM7

#define HW_MIC_LAST_ADDRESS             (HW_MAIN_MEM + 0x00ffff90)      // MIC �ŐV�T���v�����O�f�[�^�i�[�A�h���X
#define HW_MIC_SAMPLING_DATA            (HW_MAIN_MEM + 0x00ffff94)      // MIC �P�̃T���v�����O����

#define HW_WM_CALLBACK_CONTROL          (HW_MAIN_MEM + 0x00ffff96)      // WM �R�[���o�b�N�����p�p�����[�^
#define HW_WM_RSSI_POOL                 (HW_MAIN_MEM + 0x00ffff98)      // WM ��M���x�ɂ�闐����

#define HW_SET_CTRDG_MODULE_INFO_ONCE   (HW_MAIN_MEM + 0x00ffff9a)      // set ctrdg module info flag
#define HW_IS_CTRDG_EXIST               (HW_MAIN_MEM + 0x00ffff9b)      // ctrdg exist flag

#define HW_COMPONENT_PARAM              (HW_MAIN_MEM + 0x00ffff9c)      // Component �����p�p�����[�^

#define HW_THREADINFO_MAIN              (HW_MAIN_MEM + 0x00ffffa0)      // ThreadInfo for Main processor
#define HW_THREADINFO_SUB               (HW_MAIN_MEM + 0x00ffffa4)      // ThreadInfo for Sub  processor
#define HW_BUTTON_XY_BUF                (HW_MAIN_MEM + 0x00ffffa8)      // buffer for X and Y button
#define HW_TOUCHPANEL_BUF               (HW_MAIN_MEM + 0x00ffffaa)      // buffer for touch panel
#define HW_AUTOLOAD_SYNC_BUF            (HW_MAIN_MEM + 0x00ffffae)      // buffer for autoload sync

#define HW_LOCK_ID_FLAG_MAIN            (HW_MAIN_MEM + 0x00ffffb0)      // lockID flag for Main processor
#define HW_LOCK_ID_FLAG_SUB             (HW_MAIN_MEM + 0x00ffffb8)      // lockID flag for Sub processor

// SpinLock Mutex
#define HW_VRAM_C_LOCK_BUF              (HW_MAIN_MEM + 0x00ffffc0)      // �u�q�`�l�|�b�E���b�N�o�b�t�@ (END-0x40)
#define HW_VRAM_D_LOCK_BUF              (HW_MAIN_MEM + 0x00ffffc8)      // �u�q�`�l�|�c�E���b�N�o�b�t�@ (END-0x38)
#define HW_WRAM_BLOCK0_LOCK_BUF         (HW_MAIN_MEM + 0x00ffffd0)      // �b�o�t�������[�N�q�`�l�E�u���b�N�O�E���b�N�o�b�t�@ (END-0x30)
#define HW_WRAM_BLOCK1_LOCK_BUF         (HW_MAIN_MEM + 0x00ffffd8)      // �b�o�t�������[�N�q�`�l�E�u���b�N�P�E���b�N�o�b�t�@ (END-0x28)
#define HW_CARD_LOCK_BUF                (HW_MAIN_MEM + 0x00ffffe0)      // �J�[�h�E���b�N�o�b�t�@ (END-0x20)
#define HW_CTRDG_LOCK_BUF               (HW_MAIN_MEM + 0x00ffffe8)      // �J�[�g���b�W�E���b�N�o�b�t�@ (END-0x18)
#define HW_INIT_LOCK_BUF                (HW_MAIN_MEM + 0x00fffff0)      // ���������b�N�o�b�t�@ (END-0x10)

#define HW_MMEMCHECKER_MAIN             (HW_MAIN_MEM + 0x00fffff8)      // MainMomory Size Checker for Main processor (END-8)
#define HW_MMEMCHECKER_SUB              (HW_MAIN_MEM + 0x00fffffa)      // MainMomory Size Checker for Sub processor  (END-6)

#define HW_CHIPTYPE_FLAG				(HW_MAIN_MEM + 0x00fffffc) 		// chiptype flag (END-4)

#define HW_CMD_AREA                     (HW_MAIN_MEM + 0x00fffffe)      // ���C���������R�}���h���s�G���A�i�g�p�֎~�G���A�j(END-2)

//----------------------------------------------------------------------
//---- Lock area
#define HW_SHARED_LOCK_BUF              (HW_VRAM_C_LOCK_BUF)
#define HW_SHARED_LOCK_BUF_END          (HW_INIT_LOCK_BUF + 8)

/*----------------------------------------------------------------------*
    �f�o�b�K�̏�Ԃ��m�F���邽�߂̋��L���������
  [TODO] TWL���[�h�œ��삷�鎞�ɂ́A���̗̈�̓V�X�e���\���Ԃł�
     �Ȃ��Ȃ�̂ŁA�f�o�b�K���C�u���������C�����Ă��炤�K�v������܂��B
 *----------------------------------------------------------------------*/
#define HW_CHECK_DEBUGGER_SW     0x027ffc10     // (u16)debugger check switch. if 0 check buf1, else buf2.
#define HW_CHECK_DEBUGGER_BUF1   0x027ff814     // (u16)debugger checker. 1 if run on debugger.
#define HW_CHECK_DEBUGGER_BUF2   0x027ffc14     // (u16)debugger checker. 1 if run on debugger.

/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}   /* extern "C" */
#endif
#endif  /* TWL_HW_COMMON_MMAP_SHARED_H_ */
