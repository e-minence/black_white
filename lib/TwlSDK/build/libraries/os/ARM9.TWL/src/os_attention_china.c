/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS
  File:     os_attentionLimitedChina.c

  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-07-10#$
  $Rev: 10883 $
  $Author: mizutani_nakaba $
 *---------------------------------------------------------------------------*/

#include <os_attention.h>
#include <nitro/os/common/interrupt.h>

#ifdef SDK_TWLLTD


/*---------------------------------------------------------------------------*
 * �֐��錾
 *---------------------------------------------------------------------------*/
static u8*  OSi_LoadImageChina(IMAGE_OBJ_INDEX index, u32 *p_size);
extern void OSi_WaitVBlank(void);
extern void OSi_VBlankIntr(void);
extern void OSi_PrepareAttention(void);
extern void OSi_ShowAttention(void);



/*---------------------------------------------------------------------------*
  Name:         OSi_LoadImage

  Description:  �C���[�W�t�@�C���̃��[�h(��) �����ۂɂ̓|�C���^��Ԃ�����

  Arguments:    index   -   ���[�h����t�@�C���̃C���f�b�N�X.
                p_size  -   �t�@�C���T�C�Y���i�[���� u32 �ւ̃|�C���^.
                            �s�v�Ȃ� NULL ���w�肵�Ė��������邱�Ƃ��ł���.

  Returns:      �C���[�W�f�[�^�̐擪�A�h���X��Ԃ�
 *---------------------------------------------------------------------------*/
static u8 *OSi_LoadImageChina(IMAGE_OBJ_INDEX index, u32 *p_size)
{
    extern u8   _binary_attention_limited_china_01nbfc_bin[];
    extern u8   _binary_attention_limited_china_01nbfc_bin_end[];
    extern u8   _binary_attention_limited_china_01nbfs_bin[];
    extern u8   _binary_attention_limited_china_01nbfs_bin_end[];
    extern u8   _binary_attention_limited_china_02nbfc_bin[];
    extern u8   _binary_attention_limited_china_02nbfc_bin_end[];
    extern u8   _binary_attention_limited_china_02nbfs_bin[];
    extern u8   _binary_attention_limited_china_02nbfs_bin_end[];
    extern u8   _binary_attention_limited_china_nbfp_bin[];
    extern u8   _binary_attention_limited_china_nbfp_bin_end[];

    static u8 *ptr_table[IMAGE_OBJ_NUM] =
    {
        _binary_attention_limited_china_01nbfc_bin,
        _binary_attention_limited_china_01nbfs_bin,
        _binary_attention_limited_china_02nbfc_bin,
        _binary_attention_limited_china_02nbfs_bin,
        _binary_attention_limited_china_nbfp_bin
    };

    static u8 *ptr_end_table[IMAGE_OBJ_NUM] =
    {
        _binary_attention_limited_china_01nbfc_bin_end,
        _binary_attention_limited_china_01nbfs_bin_end,
        _binary_attention_limited_china_02nbfc_bin_end,
        _binary_attention_limited_china_02nbfs_bin_end,
        _binary_attention_limited_china_nbfp_bin_end
    };

    if(p_size)
    {
        *p_size = (u32)(ptr_end_table[index] - ptr_table[index]);
    }

    return (u8 *)ptr_table[index];
}

/*---------------------------------------------------------------------------*
  Name:         OS_ShowAttentionOfLimitedRomChina

  Description:  show string of attention to run limited mode on NITRO for China.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void OS_ShowAttentionOfLimitedRomChina(void)
{
    /* ���� */
    OSi_PrepareAttention();

    /* �摜�f�[�^�̃��[�h */
    {
        u32     plt_size;
        void    const *data_plt     = OSi_LoadImageChina(IMAGE_OBJ_PAL, &plt_size);
        void    const *data_01_chr  = OSi_LoadImageChina(IMAGE_OBJ_01_CHR, NULL);
        void    const *data_01_scr  = OSi_LoadImageChina(IMAGE_OBJ_01_SCR, NULL);
        void    const *data_02_chr  = OSi_LoadImageChina(IMAGE_OBJ_02_CHR, NULL);
        void    const *data_02_scr  = OSi_LoadImageChina(IMAGE_OBJ_02_SCR, NULL);

        /* ���ʗp�f�[�^�� VRAM-A �Ƀ��[�h */
        MI_UncompressLZ16(data_01_chr, (u32 *)HW_BG_VRAM);
        MI_UncompressLZ16(data_01_scr, (u32 *)(HW_BG_VRAM + 0xf000));

        /* ����ʗp�f�[�^�� VRAM-C �Ƀ��[�h */
        MI_UncompressLZ16(data_02_chr, (u32 *)HW_DB_BG_VRAM);
        MI_UncompressLZ16(data_02_scr, (u32 *)(HW_DB_BG_VRAM + 0xf000));

        /* �p���b�g�f�[�^�� �W���p���b�gRAM �Ƀ��[�h */
        SVC_CpuCopyFast(data_plt, (u32 *)(HW_BG_PLTT),    plt_size);
        SVC_CpuCopyFast(data_plt, (u32 *)(HW_DB_BG_PLTT), plt_size);
    }

    /* �\�� �����̒��Ń��[�v */
    OSi_ShowAttention();
}


#endif  // #ifdef SDK_TWLLTD
/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
