/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - include
  File:     types.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-27#$
  $Rev: 9427 $
  $Author: yosizaki $

 *---------------------------------------------------------------------------*/
#ifndef NITRO_CARD_TYPES_H_
#define NITRO_CARD_TYPES_H_


#include <nitro/misc.h>
#include <nitro/types.h>


#ifdef __cplusplus
extern "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

// ROM�̈���\����
typedef struct CARDRomRegion
{
    u32     offset;
    u32     length;
}
CARDRomRegion;

// ROM�w�b�_�\����
typedef struct CARDRomHeader
{

    // 0x000-0x020 [�V�X�e���\��̈�]
    char    game_name[12];          // �\�t�g�^�C�g����
    u32     game_code;              // �C�j�V�����R�[�h
    u16     maker_code;             // ���[�J�[�R�[�h
    u8      product_id;             // �{�̃R�[�h
    u8      device_type;            // �f�o�C�X�^�C�v
    u8      device_size;            // �f�o�C�X�e��
    u8      reserved_A[9];          // �V�X�e���\�� A
    u8      game_version;           // �\�t�g�o�[�W����
    u8      property;               // �����g�p�t���O
    // 0x020-0x040 [�풓���W���[���p�p�����[�^]
    void   *main_rom_offset;        // ARM9 �]���� ROM �I�t�Z�b�g
    void   *main_entry_address;     // ARM9 ���s�J�n�A�h���X(������)
    void   *main_ram_address;       // ARM9 �]���� RAM �I�t�Z�b�g
    u32     main_size;              // ARM9 �]���T�C�Y
    void   *sub_rom_offset;         // ARM7 �]���� ROM �I�t�Z�b�g
    void   *sub_entry_address;      // ARM7 ���s�J�n�A�h���X(������)
    void   *sub_ram_address;        // ARM7 �]���� RAM �I�t�Z�b�g
    u32     sub_size;               // ARM7 �]���T�C�Y

    // 0x040-0x050 [�t�@�C���e�[�u���p�p�����[�^]
    CARDRomRegion fnt;              // �t�@�C���l�[���e�[�u��
    CARDRomRegion fat;              // �t�@�C���A���P�[�V�����e�[�u��

    // 0x050-0x060 [�I�[�o�[���C�w�b�_�e�[�u���p�p�����[�^]
    CARDRomRegion main_ovt;         // ARM9 �I�[�o�[���C�w�b�_�e�[�u��
    CARDRomRegion sub_ovt;          // ARM7 �I�[�o�[���C�w�b�_�e�[�u��

    // 0x060-0x070 [�V�X�e���\��̈�]
    u8      rom_param_A[8];         // �}�X�N ROM �R���g���[���p�����[�^ A
    u32     banner_offset;          // �o�i�[�t�@�C�� ROM �I�t�Z�b�g
    u16     secure_crc;             // �Z�L���A�̈� CRC
    u8      rom_param_B[2];         // �}�X�N ROM �R���g���[���p�����[�^ B

    // 0x070-0x078 [�I�[�g���[�h�p�����[�^]
    void   *main_autoload_done;     // ARM9 �I�[�g���[�h�t�b�N�A�h���X
    void   *sub_autoload_done;      // ARM7 �I�[�g���[�h�t�b�N�A�h���X

    // 0x078-0x0C0 [�V�X�e���\��̈�]
    u8      rom_param_C[8];         // �}�X�N ROM �R���g���[���p�����[�^ C
    u32     rom_size;               // �A�v���P�[�V�����ŏI ROM �I�t�Z�b�g
    u32     header_size;            // ROM �w�b�_�T�C�Y
    u32     main_module_param_offset;  // Offset for table of ARM9 module parameters
    u32     sub_module_param_offset;   // Offset for table of ARM7 module parameters

    u16     normal_area_rom_offset; // undeveloped
    u16     twl_ltd_area_rom_offset;// undeveloped
    u8      reserved_B[0x2C];       // �V�X�e���\�� B

    // 0x0C0-0x160 [�V�X�e���\��̈�]
    u8      logo_data[0x9C];        // NINTENDO ���S�C���[�W�f�[�^
    u16     logo_crc;               // NINTENDO ���S CRC
    u16     header_crc;             // ROM ���o�^�f�[�^ CRC

}
CARDRomHeader;

typedef CARDRomHeader CARDRomHeaderNTR;

// TWL�g��ROM�w�b�_���
typedef struct CARDRomHeaderTWL
{
    CARDRomHeaderNTR    ntr;
    u8                  debugger_reserved[0x20];    // �f�o�b�K�\��
    u8                  config1[0x34];              // �����g�p�t���O�Q
    // 0x1B4 - accessControl
    struct {
        u32     :5;
        u32     game_card_on :1;            // NAND�A�v���ŃQ�[���J�[�h�d��ON�i�m�[�}�����[�h�j
		u32     :2;
        u32     game_card_nitro_mode :1;    // NAND�A�v���ŃQ�[���J�[�hNTR�݊��̈�փA�N�Z�X
        u32     :0;
    }access_control;
    u8                  reserved_0x1B8[8];          // �\�� (all 0)
    u32                 main_ltd_rom_offset;
    u8                  reserved_0x1C4[4];          // �\�� (all 0)
    void               *main_ltd_ram_address;
    u32                 main_ltd_size;
    u32                 sub_ltd_rom_offset;
    u8                  reserved_0x1D4[4];          // �\�� (all 0)
    void               *sub_ltd_ram_address;
    u32                 sub_ltd_size;
    CARDRomRegion       digest_area_ntr;
    CARDRomRegion       digest_area_ltd;
    CARDRomRegion       digest_tabel1;
    CARDRomRegion       digest_tabel2;
    u32                 digest_table1_size;
    u32                 digest_table2_sectors;
    u8                  config2[0xF8];              // �����g�p�t���O�Q
    u8                  main_static_digest[0x14];
    u8                  sub_static_digest[0x14];
    u8                  digest_tabel2_digest[0x14];
    u8                  banner_digest[0x14];
    u8                  main_ltd_static_digest[0x14];
    u8                  sub_ltd_static_digest[0x14];
}
CARDRomHeaderTWL;


SDK_COMPILER_ASSERT(sizeof(CARDRomHeader) == 0x160);
SDK_COMPILER_ASSERT(sizeof(CARDRomHeaderTWL) == 0x378);


/*---------------------------------------------------------------------------*/
/* constants */

// ROM �G���A�T�C�Y
#define CARD_ROM_PAGE_SIZE	512

// �ꍇ�ɂ����NTR�̈�I�[(CARDRomHeader.rom_size)�ɕt������Ă���
// DS�_�E�����[�h�v���C�����f�[�^�̃T�C�Y�B
#define CARD_ROM_DOWNLOAD_SIGNATURE_SIZE 136

// CARD���C�u�����֐���������
typedef enum CARDResult
{
    CARD_RESULT_SUCCESS = 0,
    CARD_RESULT_FAILURE,
    CARD_RESULT_INVALID_PARAM,
    CARD_RESULT_UNSUPPORTED,
    CARD_RESULT_TIMEOUT,
    CARD_RESULT_ERROR,
    CARD_RESULT_NO_RESPONSE,
    CARD_RESULT_CANCELED
}
CARDResult;

#define CARD_ROM_HEADER_EXE_NTR_OFF 0x01
#define CARD_ROM_HEADER_EXE_TWL_ON  0x02


/*---------------------------------------------------------------------------*/
/* types */

/*---------------------------------------------------------------------------*
  Name:         CARD_IsExecutableOnNTR

  Description:  �v���O�����́uDS���[�h�N���\�v�t���O�𔻒肷��B

  Arguments:    header : ���肷�ׂ��v���O������ROM�w�b�_���B

  Returns:      DS���[�h�ŋN���\�ȃv���O������ROM�w�b�_�ł����TRUE�B
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_IsExecutableOnNTR(const CARDRomHeader *header)
{
    return ((header->product_id & CARD_ROM_HEADER_EXE_NTR_OFF) == 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_IsExecutableOnTWL

  Description:  �v���O�����́uTWL���[�h�N���\�v�t���O�𔻒肷��B

  Arguments:    header : ���肷�ׂ��v���O������ROM�w�b�_���B

  Returns:      TWL���[�h�ŋN���\�ȃv���O������ROM�w�b�_�ł����TRUE�B
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_IsExecutableOnTWL(const CARDRomHeader *header)
{
    return ((header->product_id & CARD_ROM_HEADER_EXE_TWL_ON) != 0);
}


#ifdef __cplusplus
} // extern "C"
#endif


#endif // NITRO_CARD_TYPES_H_
