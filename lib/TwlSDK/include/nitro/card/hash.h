/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - include
  File:     hash.h

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
#ifndef NITRO_CARD_HASH_H_
#define NITRO_CARD_HASH_H_


#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/card/types.h>
#include <nitro/mi/device.h>


#ifdef __cplusplus
extern "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* constants */

#define CARD_ROM_HASH_SIZE      20


/*---------------------------------------------------------------------------*/
/* declarations */

struct CARDRomHashSector;
struct CARDRomHashBlock;
struct CARDRomHashContext;

// �e�Z�N�^�̃C���[�W�L���b�V�����Ǘ�����\���́B
typedef struct CARDRomHashSector
{
    struct CARDRomHashSector   *next;
    u32                         index;
    u32                         offset;
    void                       *image;
}
CARDRomHashSector;

// �e�Z�N�^�̃C���[�W�L���b�V���ƃn�b�V�����Ǘ�����u���b�N�\���́B
// NTR�̈�̍Ō�̃u���b�N�T�C�Y���[������LTD�̈�̐擪���A������邽��
// ���҂̍��݂���u���b�N���ő�1�������݂�����_�ɗ��ӂ��Ă������ƁB
typedef struct CARDRomHashBlock
{
    struct CARDRomHashBlock    *next;
    u32                         index;
    u32                         offset;
    u8                         *hash;
    u8                         *hash_aligned;
}
CARDRomHashBlock;

// SRL�t�@�C���̃n�b�V���Ǘ��\���́B
// ���v�T�C�Y�̓v���O�������ƂɈقȂ�ÓI�ɎZ�o�ł��Ȃ�����
// ���������ɃA���[�i����K�ʂ������I�Ɋm�ۂ���\��B
// �K�v�ƂȂ郁�����͈ȉ��̒ʂ�B
//   - �}�X�^�[�n�b�V���e�[�u��:
//       ROM�w�b�_�ƃn�b�V����r���Đ������𔻒肷��K�v��A
//       ����������ROM����ꊇ���[�h���ď풓�����Ă����B
//       (ROM�T�C�Y/�Z�N�^�T�C�Y/�Z�N�^�P��)*20(SHA1)�o�C�g�K�v�ŁA
//       1Gbits:1024�o�C�g:32�Z�N�^�Ȃ�Ζ�80kB�ƂȂ�B
//   - �u���b�N�L���b�V��:
//       �u���b�N�P�ʂ�ROM�C���[�W�L���b�V���Ƃ��̃n�b�V�����Ǘ�����B
//       �u���b�N���E���܂��������U�I�ȃA�N�Z�X���l������
//       ��ɕ�����ێ��ł��郊�X�g�\���ɂ��Ă����K�v������B
//       (20 * �Z�N�^�P�� + ��) �̍\���̂����X�g�����̕������K�v�B
//   - �Z�N�^�L���b�V��:
//       ���ۂ̃C���[�W�L���b�V����ێ�����B
//       �u���b�N���S�Z�N�^�����܂˂��Q�Ƃ���Ƃ͌���Ȃ�����
//       �Z�N�^���ʓr���X�g�\���Ƃ��ĊǗ�����K�v������B
typedef struct CARDRomHashContext
{
    // ROM�w�b�_����擾�����{�ݒ�
    CARDRomRegion       area_ntr;
    CARDRomRegion       area_ltd;
    CARDRomRegion       sector_hash;
    CARDRomRegion       block_hash;
    u32                 bytes_per_sector;
    u32                 sectors_per_block;
    u32                 block_max;
    u32                 sector_max;

    // �f�[�^�ƃn�b�V�������[�h����f�o�C�X�C���^�t�F�[�X
    void                   *userdata;
    MIDeviceReadFunction    ReadSync;
    MIDeviceReadFunction    ReadAsync;

    // ���[�h�������̃X���b�h�B
    OSThread           *loader;
    void               *recent_load;

    // �Z�N�^�ƃu���b�N�̃L���b�V��
    CARDRomHashSector  *loading_sector; // ���f�B�A���[�h�҂��Z�N�^
    CARDRomHashSector  *loaded_sector;  // �n�b�V�����ؑ҂��Z�N�^
    CARDRomHashSector  *valid_sector;   // ���������؍ς݃Z�N�^
    CARDRomHashBlock   *loading_block;  // ���f�B�A���[�h�҂��u���b�N
    CARDRomHashBlock   *loaded_block;   // �n�b�V�����ؑ҂��u���b�N
    CARDRomHashBlock   *valid_block;    // ���������؍ς݃u���b�N
    // �A���[�i����m�ۂ����z��
    u8                 *master_hash;    // �u���b�N�̃n�b�V���z��
    u8                 *images;         // �Z�N�^�C���[�W
    u8                 *hashes;         // �u���b�N���̃n�b�V���z��
    CARDRomHashSector  *sectors;        // �Z�N�^���
    CARDRomHashBlock   *blocks;         // �u���b�N���
}
CARDRomHashContext;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARD_InitRomHashContext

  Description:  ROM�n�b�V���R���e�L�X�g���������B

  Arguments:    context  : ���������ׂ�CARDRomHashContext�\����
                header   : �Ǘ����ׂ�ROM����ێ�����ROM�w�b�_�o�b�t�@
                           (���̊֐����ł̂ݎQ�Ƃ���邽�ߕێ�����K�v�͂Ȃ�)
                buffer   : �R���e�L�X�g�����Ŏg�p����Ǘ����p�o�b�t�@
                length   : �Ǘ����p�o�b�t�@�̃T�C�Y
                           (CARD_CalcRomHashBufferLength�֐��Ōv�Z�\)
                sync     : �f�o�C�X�������[�h�R�[���o�b�N�֐�
                async    : �f�o�C�X�񓯊����[�h�R�[���o�b�N�֐�(��Ή��Ȃ�NULL)
                userdata : �f�o�C�X���[�h�R�[���o�b�N�ɗ^����C�ӂ̃��[�U��`����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_InitRomHashContext(CARDRomHashContext *context, const CARDRomHeaderTWL *header,
                             void *buffer, u32 length,
                             MIDeviceReadFunction sync, MIDeviceReadFunction async,
                             void *userdata);

/*---------------------------------------------------------------------------*
  Name:         CARD_CalcRomHashBufferLength

  Description:  ROM�n�b�V���R���e�L�X�g�ɕK�v�ȃo�b�t�@�T�C�Y���擾�B

  Arguments:    header   : �Ǘ����ׂ�ROM����ێ�����ROM�w�b�_�o�b�t�@

  Returns:      ROM�n�b�V���R���e�L�X�g�ɕK�v�ȃo�b�t�@�T�C�Y�B
 *---------------------------------------------------------------------------*/
u32 CARD_CalcRomHashBufferLength(const CARDRomHeaderTWL *header);

/*---------------------------------------------------------------------------*
  Name:         CARDi_NotifyRomHashReadAsync

  Description:  �f�o�C�X�񓯊����[�h�����̊������n�b�V���R���e�L�X�g�֒ʒm�B

  Arguments:    context : CARDRomHashContext�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_NotifyRomHashReadAsync(CARDRomHashContext *context);

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadRomHashImage

  Description:  �w��̃I�t�Z�b�g����n�b�V�����؍ς݂�ROM�C���[�W��ǂݏo���B

  Arguments:    context : CARDRomHashContext�\���́B
                offset  : �A�N�Z�X����ROM�I�t�Z�b�g�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_ReadRomHashImage(CARDRomHashContext *context, void *buffer, u32 offset, u32 length);


#ifdef __cplusplus
} // extern "C"
#endif


#endif // NITRO_CARD_HASH_H_
