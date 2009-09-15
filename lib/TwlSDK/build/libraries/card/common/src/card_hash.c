/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_hash.c

  Copyright 2008-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/card_rom.h"


// �T�C�Y���傫��MI_CpuCopy��NTR���Ń����N���Ȃ��悤�ALIMITED�������g�p�B
#if defined(SDK_TWLLTD)
#define CARDi_CopyMemory MI_CpuCopy
#else
#define CARDi_CopyMemory MI_CpuCopy8
#endif


// HMAC-SHA1�̌v�Z������������ꍇ�͂��̃t���O��L���ɁB
#if defined(SDK_TWL)
#define CARD_SUPPORT_SHA1OPTIMIZE
#endif

#include <twl/ltdmain_begin.h>

/*---------------------------------------------------------------------------*/
/* constants */

// �����_��CARD���C�u�������Ó��Ɣ��f�����萔�B
// �p�t�H�[�}���X�v���̌��ʂɂ���ēK�X�ύX���Ă��悢�B
static const u32    CARD_ROM_HASH_BLOCK_MAX = 4;
static const u32    CARD_ROM_HASH_SECTOR_MAX = 32;

#if defined(CARD_SUPPORT_SHA1OPTIMIZE)
static SVCSHA1Context   sha1_ipad_def;
static SVCSHA1Context   sha1_opad_def;
#endif

static u8 CARDiHmacKey[] ATTRIBUTE_ALIGN(4) =
{
    0x21, 0x06, 0xc0, 0xde, 0xba, 0x98, 0xce, 0x3f,
    0xa6, 0x92, 0xe3, 0x9d, 0x46, 0xf2, 0xed, 0x01,
    0x76, 0xe3, 0xcc, 0x08, 0x56, 0x23, 0x63, 0xfa,
    0xca, 0xd4, 0xec, 0xdf, 0x9a, 0x62, 0x78, 0x34,
    0x8f, 0x6d, 0x63, 0x3c, 0xfe, 0x22, 0xca, 0x92,
    0x20, 0x88, 0x97, 0x23, 0xd2, 0xcf, 0xae, 0xc2,
    0x32, 0x67, 0x8d, 0xfe, 0xca, 0x83, 0x64, 0x98,
    0xac, 0xfd, 0x3e, 0x37, 0x87, 0x46, 0x58, 0x24,
};

/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARD_CalcRomHashBufferLength

  Description:  ROM�n�b�V���R���e�L�X�g�ɕK�v�ȃo�b�t�@�T�C�Y���擾�B

  Arguments:    header   : �Ǘ����ׂ�ROM����ێ�����ROM�w�b�_�o�b�t�@

  Returns:      ROM�n�b�V���R���e�L�X�g�ɕK�v�ȃo�b�t�@�T�C�Y�B
 *---------------------------------------------------------------------------*/
u32 CARD_CalcRomHashBufferLength(const CARDRomHeaderTWL *header)
{
    u32     length = 0;
    length += MATH_ROUNDUP(header->digest_tabel2.length, 32);
    length += MATH_ROUNDUP(header->digest_table1_size * CARD_ROM_HASH_SECTOR_MAX, 32);
    length += MATH_ROUNDUP(MATH_ROUNDUP(CARD_ROM_HASH_SIZE * header->digest_table2_sectors, CARD_ROM_PAGE_SIZE * 3) * CARD_ROM_HASH_BLOCK_MAX, 32);
    length += MATH_ROUNDUP(sizeof(CARDRomHashSector) * CARD_ROM_HASH_SECTOR_MAX, 32);
    length += MATH_ROUNDUP(sizeof(CARDRomHashBlock) * CARD_ROM_HASH_BLOCK_MAX, 32);
    return length;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CompareHash

  Description:  �n�b�V���ɂ�鐳�����`�F�b�N�B(HMAC-SHA1)

  Arguments:    hash : ��r��ƂȂ�n�b�V���l
                src  : �`�F�b�N�Ώۂ̃C���[�W
                len�@: �`�F�b�N�Ώۂ̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_CompareHash(const void *hash, void *buffer, u32 length)
{
#if defined(CARD_SUPPORT_SHA1OPTIMIZE)
    // �O�����Čv�Z�ς݂�ipad/opad���\���̂��ƃR�s�[�B
    u8      tmphash[CARD_ROM_HASH_SIZE];
    const u32  *h1 = (const u32 *)hash;
    const u32  *h2 = (const u32 *)tmphash;
    u32         dif = 0;
    SVCSHA1Context  tmpcxt;
    CARDi_CopyMemory(&sha1_ipad_def, &tmpcxt, sizeof(tmpcxt));
    SVC_SHA1Update(&tmpcxt, buffer, length);
    SVC_SHA1GetHash(&tmpcxt, tmphash);
    CARDi_CopyMemory(&sha1_opad_def, &tmpcxt, sizeof(tmpcxt));
    SVC_SHA1Update(&tmpcxt, tmphash, sizeof(tmphash));
    SVC_SHA1GetHash(&tmpcxt, tmphash);
    dif |= (h1[0] ^ h2[0]);
    dif |= (h1[1] ^ h2[1]);
    dif |= (h1[2] ^ h2[2]);
    dif |= (h1[3] ^ h2[3]);
    dif |= (h1[4] ^ h2[4]);
    if (dif != 0)
#else
    u8      tmphash[CARD_ROM_HASH_SIZE];
#ifdef SDK_TWL
    if (OS_IsRunOnTwl())
    {
        SVC_CalcHMACSHA1(tmphash, buffer, length, CARDiHmacKey, sizeof(CARDiHmacKey));
    }
    else
#endif
    {
        MATH_CalcHMACSHA1(tmphash, buffer, length, CARDiHmacKey, sizeof(CARDiHmacKey));
    }
    if (MI_CpuComp8(hash, tmphash, sizeof(tmphash)) != 0)
#endif
    {
        // �J�[�h�������I���������x������Ă���ꍇ��
        // ���̏�ŋ�����~�����x���o��&�f�[�^�N���A�ɂƂǂ߂�B
        if ((OS_GetBootType() == OS_BOOTTYPE_ROM) && CARD_IsPulledOut())
        {
            OS_TWarning("ROM-hash comparation error. (CARD removal)\n");
            MI_CpuFill(buffer, 0, length);
        }
        else
        {
            OS_TPanic("ROM-hash comparation error!\n");
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetHashSectorIndex

  Description:  �w���ROM�I�t�Z�b�g��������Z�N�^�ԍ����擾�B

  Arguments:    context : CARDRomHashContext�\����
                offset  : ROM�I�t�Z�b�g

  Returns:      �w���ROM�I�t�Z�b�g��������Z�N�^�ԍ��B
 *---------------------------------------------------------------------------*/
SDK_INLINE u32 CARDi_GetHashSectorIndex(const CARDRomHashContext *context, u32 offset)
{
    offset -= context->area_ntr.offset;
    if (offset >= context->area_ntr.length)
    {
        offset += (context->area_ntr.offset - context->area_ltd.offset);
        if (offset < context->area_ltd.length)
        {
            offset += context->area_ntr.length;
        }
        else
        {
            OS_TPanic("specified ROM address is outof-range.(unsafe without secure hash)\n");
        }
    }
    return offset / context->bytes_per_sector;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_StartLoading

  Description:  ���f�B�A���[�h�҂��Z�N�^���ЂƂ����]���J�n�B

  Arguments:    context : CARDRomHashContext�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_StartLoading(CARDRomHashContext *context)
{
    // ���ݔ񓯊����������s���Ă��Ȃ���΂����ŊJ�n�B
    void   *buffer = NULL;
    u32     offset = 0;
    u32     length = 0;
    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
    if (context->recent_load == NULL)
    {
        if (context->loading_block)
        {
            // �񓯊�DMA�]�����ł���悤�Ɏ��ۂ�葽���o�b�t�@������Ă���B
            // �Q�Ɛ�͎��ۂɕK�v�Ƃ���e�[�u���ʒu�փI�t�Z�b�g�����킹��B
            CARDRomHashBlock   *block = context->loading_block;
            u32                 pos = block->offset;
            u32                 mod = (pos & (CARD_ROM_PAGE_SIZE - 1UL));
            block->hash = &block->hash_aligned[mod];
            context->recent_load = block;
            buffer = block->hash_aligned;
            offset = pos - mod;
            length = MATH_ROUNDUP(mod + CARD_ROM_HASH_SIZE * context->sectors_per_block, CARD_ROM_PAGE_SIZE);
        }
        else if (context->loading_sector)
        {
            CARDRomHashSector  *sector = context->loading_sector;
            context->recent_load = sector;
            buffer = sector->image;
            offset = sector->offset;
            length = context->bytes_per_sector;
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
    // �񓯊��]������T�|�[�g�����ݗ��p�s�\�Ȃ�΂����œ����]���B
    if (buffer != NULL)
    {
        if ((*context->ReadAsync)(context->userdata, buffer, offset, length) == 0)
        {
            (void)(*context->ReadSync)(context->userdata, buffer, offset, length);
            CARD_NotifyRomHashReadAsync(context);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_NotifyRomHashReadAsync

  Description:  �f�o�C�X�񓯊����[�h�̊�����ʒm�B

  Arguments:    context : CARDRomHashContext�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_NotifyRomHashReadAsync(CARDRomHashContext *context)
{
    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
    // ���[�h�҂����猟�ؑ҂��փ��X�g�ړ��B
    if (context->recent_load == context->loading_sector)
    {
        CARDRomHashSector  *sector = context->loading_sector;
        context->loading_sector = sector->next;
        sector->next = context->loaded_sector;
        context->loaded_sector = sector;
    }
    else
    {
        CARDRomHashBlock   *block = context->loading_block;
        context->loading_block = block->next;
        block->next = context->loaded_block;
        context->loaded_block = block;
    }
    context->recent_load = NULL;
    (void)OS_RestoreInterrupts(bak_cpsr);
    // ���ؑ҂����X�g���ǉ����ꂽ���Ƃ�҂��󂯃X���b�h�֒ʒm�B
    OS_WakeupThreadDirect(context->loader);
    // ����Ȃ玟�̃��[�h�҂����X�g��]���J�n�B
    CARDi_StartLoading(context);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_TouchRomHashBlock

  Description:  �w��̃I�t�Z�b�g���܂܂��n�b�V���u���b�N�������[�h�B
                ���łɃ��[�h�ς݂Ȃ牽�����Ȃ��B

  Arguments:    context : CARDRomHashContext�\���́B
                sector  : �A�N�Z�X����Z�N�^�B

  Returns:      �Y������n�b�V���u���b�N���B
 *---------------------------------------------------------------------------*/
static CARDRomHashBlock* CARDi_TouchRomHashBlock(CARDRomHashContext *context, u32 sector)
{
    // �u���b�N�̈��NTR�̈��LTD�̈悪�A�����Ĉ�����_�ɒ��ӁB
    // (�]�v�ȃp�f�B���O��A���C�����g��}�����Ă͂Ȃ�Ȃ�)
    u32     index = sector / context->sectors_per_block;
    // �Y������u���b�N��񂪂��łɃ��[�h�ς݂�����B
    CARDRomHashBlock  **pp = &context->valid_block;
    CARDRomHashBlock   *block = NULL;
    for (;; pp = &(*pp)->next)
    {
        block = *pp;
        if (block->index == index)
        {
            // �^�b�`���ꂽ�u���b�N���̓��X�g�̐擪�Ɉړ����Ă����B
            *pp = block->next;
            block->next = context->valid_block;
            context->valid_block = block;
            break;
        }
        // ���[�h����Ă��Ȃ���΍ł��Â��u���b�N����j�����čă��[�h�B
        else if (block->next == NULL)
        {
            // ���[�h�҂����X�g�̏I�[�ֈړ�����B
            OSIntrMode  bak_cpsr = OS_DisableInterrupts();
            CARDRomHashBlock  **tail = pp;
            CARDRomHashBlock   *loading = block;
            // ���łɓ���u���b�N�����[�h�҂������ؑ҂��Ȃ牽�����Ȃ��B
            for (pp = &context->loaded_block; *pp; pp = &(*pp)->next)
            {
                if ((*pp)->index == index)
                {
                    block = (*pp);
                    loading = NULL;
                    break;
                }
            }
            if (loading)
            {
                for (pp = &context->loading_block; *pp; pp = &(*pp)->next)
                {
                    if ((*pp)->index == index)
                    {
                        block = (*pp);
                        loading = NULL;
                        break;
                    }
                }
                if (loading)
                {
                    *tail = NULL;
                    *pp = loading;
                    loading->index = index;
                    loading->offset = context->sector_hash.offset + index * (CARD_ROM_HASH_SIZE * context->sectors_per_block);
                }
            }
            (void)OS_RestoreInterrupts(bak_cpsr);
            if (loading)
            {
                // �񓯊����[�h�̊J�n�^�C�~���O���m�F�B
                CARDi_StartLoading(context);
            }
            break;
        }
    }
    return block;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_TouchRomHashSector

  Description:  �w��̃Z�N�^���u���b�N��Ƀ��[�h�B
                ���łɃ��[�h�ς݂Ȃ牽�����Ȃ��B

  Arguments:    context : CARDRomHashContext�\���́B
                block  : CARDRomHashBlock�\���́B
                offset : �A�N�Z�X����ROM�I�t�Z�b�g�B

  Returns:      �Y������Z�N�^�C���[�W�܂���NULL�B
 *---------------------------------------------------------------------------*/
static void* CARDi_TouchRomHashSector(CARDRomHashContext *context, u32 offset)
{
    void               *image = NULL;
    CARDRomHashSector **pp = &context->valid_sector;
    CARDRomHashSector  *sector = NULL;
    u32                 index = CARDi_GetHashSectorIndex(context, offset);
    for (pp = &context->valid_sector; ; pp = &(*pp)->next)
    {
        // �Y������Z�N�^��񂪂��łɃ��[�h�ς݂Ȃ炻�̃C���[�W��Ԃ��B
        if ((*pp)->index == index)
        {
            // �^�b�`���ꂽ�Z�N�^���̓��X�g�̐擪�Ɉړ����Ă����B
            sector = *pp;
            *pp = (*pp)->next;
            sector->next = context->valid_sector;
            context->valid_sector = sector;
            image = sector->image;
            break;
        }
        // ���[�h����Ă��Ȃ���΍ł��Â��Z�N�^����j�����čă��[�h�B
        else if ((*pp)->next == NULL)
        {
            // ���[�h�҂����X�g�̏I�[�ֈړ�����B
            OSIntrMode  bak_cpsr = OS_DisableInterrupts();
            CARDRomHashSector  *loading = *pp;
            *pp = NULL;
            for (pp = &context->loading_sector; *pp; pp = &(*pp)->next)
            {
            }
            *pp = loading;
            loading->index = index;
            loading->offset = MATH_ROUNDDOWN(offset, context->bytes_per_sector);
            (void)OS_RestoreInterrupts(bak_cpsr);
            // ���������؂̂��߂ɕK�v�ƂȂ�u���b�N���Ɍ������Ă����B
            (void)CARDi_TouchRomHashBlock(context, index);
            // �񓯊����[�h�̊J�n�^�C�~���O���m�F�B
            CARDi_StartLoading(context);
            break;
        }
    }
    return image;
}

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
                             void *userdata)
{
    // �r���h���ƂɈقȂ�萔�B
    const u32   bytes_per_sector = header->digest_table1_size;
    const u32   sectors_per_block = header->digest_table2_sectors;
    const u32   master_hash_size = header->digest_tabel2.length;

    // �K�v�ȉϒ����������m�ۂł��Ă��邩�v�Z�B
    u8     *lo = (u8 *)MATH_ROUNDUP((u32)buffer, 32);
    u8     *hi = (u8 *)MATH_ROUNDDOWN((u32)&lo[length], 32);
    u8     *cur = lo;
    context->master_hash = (u8 *)cur;
    cur += MATH_ROUNDUP(master_hash_size, 32);
    context->images = (u8 *)cur;
    cur += MATH_ROUNDUP(bytes_per_sector * CARD_ROM_HASH_SECTOR_MAX, 32);
    context->hashes = (u8 *)cur;
    cur += MATH_ROUNDUP(MATH_ROUNDUP(CARD_ROM_HASH_SIZE * sectors_per_block, CARD_ROM_PAGE_SIZE * 3) * CARD_ROM_HASH_BLOCK_MAX, 32);
    context->sectors = (CARDRomHashSector *)cur;
    cur += MATH_ROUNDUP(sizeof(*context->sectors) * CARD_ROM_HASH_SECTOR_MAX, 32);
    context->blocks = (CARDRomHashBlock *)cur;
    cur += MATH_ROUNDUP(sizeof(*context->blocks) * CARD_ROM_HASH_BLOCK_MAX, 32);
    if (cur > hi)
    {
        OS_TPanic("cannot allocate memory for ROM-hash from ARENA");
    }
    else
    {
        // �m�ۉ\�Ȃ��{�ݒ���������B
        MI_CpuClear32(lo, (u32)(cur - lo));
        context->area_ntr = header->digest_area_ntr;
        context->area_ltd = header->digest_area_ltd;
        context->sector_hash = header->digest_tabel1;
        context->block_hash = header->digest_tabel2;
        context->bytes_per_sector = bytes_per_sector;
        context->sectors_per_block = sectors_per_block;
        context->block_max = CARD_ROM_HASH_BLOCK_MAX;
        context->sector_max = CARD_ROM_HASH_SECTOR_MAX;
        // �f�o�C�X���[�_���������B
        context->ReadSync = sync;
        context->ReadAsync = async;
        context->userdata = userdata;
#if defined(CARD_SUPPORT_SHA1OPTIMIZE)
        // �n�b�V���v�Z�p��ipad/opad�����O�v�Z���Ă��܂��B
        {
            u8      ipad[MATH_HASH_BLOCK_SIZE];
            u8      opad[MATH_HASH_BLOCK_SIZE];
            int     i;
            for (i = 0; i < MATH_HASH_BLOCK_SIZE; ++i)
            {
                ipad[i] = (u8)(CARDiHmacKey[i] ^ 0x36);
                opad[i] = (u8)(CARDiHmacKey[i] ^ 0x5c);
            }
            SVC_SHA1Init(&sha1_ipad_def);
            SVC_SHA1Init(&sha1_opad_def);
            SVC_SHA1Update(&sha1_ipad_def, ipad, sizeof(ipad));
            SVC_SHA1Update(&sha1_opad_def, opad, sizeof(opad));
        }
#endif
        // ���������}�X�^�[�n�b�V�������[�h���Đ���������B
        (void)(*context->ReadSync)(context->userdata, context->master_hash, context->block_hash.offset, context->block_hash.length); 
        CARDi_CompareHash(header->digest_tabel2_digest, context->master_hash, context->block_hash.length);
        // �Z�N�^��񃊃X�g���������B
        {
            CARDRomHashSector  *sectors = context->sectors;
            int     sector_index = 0;
            int     i;
            for (i = 0; i < context->sector_max; ++i)
            {
                sectors[i].next = &sectors[i - 1];
                sectors[i].image = &context->images[i * context->bytes_per_sector];
                sectors[i].index = 0xFFFFFFFF;
            }
            context->sectors[0].next = NULL;
            context->valid_sector = &context->sectors[context->sector_max - 1];
            context->loading_sector = NULL;
            context->loaded_sector = NULL;
        }
        // �u���b�N��񃊃X�g���������B
        {
            CARDRomHashBlock   *blocks = context->blocks;
            const u32           unit = MATH_ROUNDUP(CARD_ROM_HASH_SIZE * sectors_per_block, CARD_ROM_PAGE_SIZE * 3);
            int     i;
            for (i = 0; i < context->block_max; ++i)
            {
                blocks[i].next = &blocks[i + 1];
                blocks[i].index = 0xFFFFFFFF;
                blocks[i].hash_aligned = &context->hashes[i * unit];
            }
            context->valid_block = &context->blocks[0];
            context->blocks[context->block_max - 1].next = NULL;
            context->loading_block = NULL;
            context->loaded_block = NULL;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomHashImageDirect

  Description:  �n�b�V���R���e�L�X�g�փL���b�V�������ɓ]����֒��ڃR�s�[�B

  Arguments:    context : CARDRomHashContext�\���́B
                buffer  : �]����o�b�t�@�B(4�o�C�g��������Ă���K�v������)
                offset  : �A�N�Z�X����ROM�I�t�Z�b�g�B
                length  : �]���T�C�Y�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_ReadRomHashImageDirect(CARDRomHashContext *context, void *buffer, u32 offset, u32 length)
{
    const u32   sectunit = context->bytes_per_sector;
    const u32   blckunit = context->sectors_per_block;
    u32         position = offset;
    u32         end = length + offset;
    u32         sector = CARDi_GetHashSectorIndex(context, position);
    while (position < end)
    {
        // ����擾�\�ȍŏ��P�ʂ̃C���[�W�𓯊��ǂݍ��݁B
        u32     available = (u32)(*context->ReadSync)(context->userdata, buffer, position, end - position);
        // ����̌��ؒ��ɕK�v�ƂȂ肻���ȃu���b�N��O�����ăA�N�Z�X�B
        (void)CARDi_TouchRomHashBlock(context, sector);
        // ����ɕK�v�ƂȂ镪�̓ǂݍ��ݏ�����v���B
        if (context->ReadAsync && (position + available < end))
        {
            (void)(*context->ReadAsync)(context->userdata, NULL, position + available, end - (position + available));
        }
        // �擾�����C���[�W�̐����������؁B
        while (available >= sectunit)
        {
            CARDRomHashBlock   *block = CARDi_TouchRomHashBlock(context, sector);
            u32                 slot = sector - block->index * blckunit;
            while ((slot < blckunit) && (available >= sectunit))
            {
                // �K�v�Ȃ炱���Ńu���b�N�P�ʂ̃n�b�V���e�[�u�������؁B
                if (block != context->valid_block)
                {
                    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
                    while (context->loading_block)
                    {
                        OS_SleepThread(NULL);
                    }
                    if (block == context->loaded_block)
                    {
                        context->loaded_block = block->next;
                    }
                    (void)OS_RestoreInterrupts(bak_cpsr);
                    CARDi_CompareHash(&context->master_hash[block->index * CARD_ROM_HASH_SIZE],
                                      block->hash, CARD_ROM_HASH_SIZE * blckunit);
                    block->next = context->valid_block;
                    context->valid_block = block;
                }
                // �C���[�W�̃n�b�V�����v�Z�B
                CARDi_CompareHash(&block->hash[slot * CARD_ROM_HASH_SIZE], buffer, sectunit);
                position += sectunit;
                available -= sectunit;
                buffer = ((u8 *)buffer) + sectunit;
                slot += 1;
                sector += 1;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomHashImageCaching

  Description:  �n�b�V���R���e�L�X�g�փL���b�V�����]����֒��ڃR�s�[�B

  Arguments:    context : CARDRomHashContext�\���́B
                buffer  : �]����o�b�t�@�B
                offset  : �A�N�Z�X����ROM�I�t�Z�b�g�B
                length  : �]���T�C�Y�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_ReadRomHashImageCaching(CARDRomHashContext *context, void *buffer, u32 offset, u32 length)
{
    while (length > 0)
    {
        // ���̃Z�N�^�����������؍ς݂�����B
        void   *image = CARDi_TouchRomHashSector(context, offset);
        if (image)
        {
            // ���؍ς݂Ȃ炻�̂܂܃������R�s�[���Ď��ցB
            u32     max = context->bytes_per_sector;
            u32     mod = offset - MATH_ROUNDDOWN(offset, max);
            u32     len = (u32)MATH_MIN(length, (max - mod));
            CARDi_CopyMemory((u8*)image + mod, buffer, len);
            buffer = (u8 *)buffer + len;
            length -= len;
            offset += len;
        }
        else
        {
            // �q�b�g���Ȃ���΁A�S�Ă̖����؃��X�g���܂Ƃ߂ď������邽��
            // �e�ʂ̋������肱���ŃZ�N�^���ǂݗv�����Ă����B
            // �֘A����u���b�N�̃n�b�V���������I�ɐ�ǂ݂���A
            // �\�Ȃ炷�łɔ񓯊��]�����J�n����Ă���B
            u32     hit = MATH_ROUNDDOWN(offset, context->bytes_per_sector);
            while (context->valid_sector && context->valid_block)
            {
                hit += context->bytes_per_sector;
                if (hit >= offset + length)
                {
                    break;
                }
                (void)CARDi_TouchRomHashSector(context, hit);
            }
            for (;;)
            {
                // ���[�h������Ԃ̌��ؑ҂����X�g����������܂őҋ@�B
                OSIntrMode          bak_cpsr = OS_DisableInterrupts();
                CARDRomHashBlock   *block = NULL;
                CARDRomHashSector  *sector = NULL;
                while ((context->loading_sector && !context->loaded_sector) ||
                        (context->loading_block && !context->loaded_block))
                {
                    OS_SleepThread(NULL);
                }
                // �ˑ��֌W���l�����ăZ�N�^���u���b�N���ɏ�������B
                block = context->loaded_block;
                if (block)
                {
                    context->loaded_block = block->next;
                }
                else
                {
                    sector = context->loaded_sector;
                    if (sector)
                    {
                        context->loaded_sector = sector->next;
                    }
                }
                (void)OS_RestoreInterrupts(bak_cpsr);
                // �u���b�N�܂��̓Z�N�^�̃n�b�V�����v�Z���Ă���ʂ̃e�[�u���Ɣ�r���؁B
                if (block)
                {
                    u32     len = CARD_ROM_HASH_SIZE * context->sectors_per_block;
                    CARDi_CompareHash(&context->master_hash[block->index * CARD_ROM_HASH_SIZE], block->hash, len);
                    // �^�b�`���ꂽ�u���b�N���̓��X�g�̐擪�Ɉړ����Ă����B
                    block->next = context->valid_block;
                    context->valid_block = block;
                }
                else if (sector)
                {
                    CARDRomHashBlock   *block = CARDi_TouchRomHashBlock(context, sector->index);
                    u32                 slot = sector->index - block->index * context->sectors_per_block;
                    CARDi_CompareHash(&block->hash[slot * CARD_ROM_HASH_SIZE], sector->image, context->bytes_per_sector);
                    // �^�b�`���ꂽ�Z�N�^���̓��X�g�̐擪�Ɉړ����Ă����B
                    sector->next = context->valid_sector;
                    context->valid_sector = sector;
                }
                // �񓯊��Ɍ��؂��ׂ����̂��������Ȃ�΍���̃��[�v�͏I���B
                else
                {
                    break;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadRomHashImage

  Description:  �w��̃I�t�Z�b�g����n�b�V�����؍ς݂�ROM�C���[�W��ǂݏo���B

  Arguments:    context : CARDRomHashContext�\���́B
                offset  : �A�N�Z�X����ROM�I�t�Z�b�g�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_ReadRomHashImage(CARDRomHashContext *context, void *buffer, u32 offset, u32 length)
{
    // DMA����̒ʒm����������B
    context->loader = OS_GetCurrentThread();
    context->recent_load = NULL;
    // �Z�N�^�L���b�V���̔����ȏ����C�Ɏ����悤�ȑ�K�͂ȃ��[�h�Ȃ�
    // �_�C���N�g�]�����[�h���̗p���č��������L���b�V�����ی삷��B
    if ((length >= context->bytes_per_sector * (CARD_ROM_HASH_SECTOR_MAX / 2))
        // TODO:
        //   ����ł͂ЂƂ܂��J�[�h�u�[�g�Ń_�C���N�g�]�����[�h���g�p���Ȃ��B
        //   (�]�����Ɠ]����̃��[�h���E�����͓��ɍl�����Ȃ��Ƃ��悢��
        //    ROM�]���̏ꍇ�͓����łɂȂ�Ƃ������Đ��\�ቺ���邩������Ȃ��̂�
        //    �K�v�ɉ����ď�����ǉ����ׂ�)
        && (OS_GetBootType() != OS_BOOTTYPE_ROM))
    {
        // �Z�N�^���E�ɐ������Ă��Ȃ���[�����ƏI�[�����𕪊��B
        const u32   sectmask = (u32)(context->bytes_per_sector - 1UL);
        const u32   headlen = (u32)((context->bytes_per_sector - offset) & sectmask);
        const u32   bodylen = (u32)((length - headlen) & ~sectmask);
        // ��[�����B
        if (headlen > 0)
        {
            CARDi_ReadRomHashImageCaching(context, buffer, offset, headlen);
            offset += headlen;
            length -= headlen;
            buffer = ((u8 *)buffer) + headlen;
        }
        // ���ԕ����B
        if (bodylen > 0)
        {
            CARDi_ReadRomHashImageDirect(context, buffer, offset, bodylen);
            offset += bodylen;
            length -= bodylen;
            buffer = ((u8 *)buffer) + bodylen;
        }
    }
    // �_�C���N�g�]�����[�h�Ŏg�p�ł��Ȃ������[���������B
    CARDi_ReadRomHashImageCaching(context, buffer, offset, length);
}
#include <twl/ltdmain_end.h>
