/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS
  File:     os_attention.c

  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <os_attention.h>
#include <nitro/os/common/interrupt.h>

#ifdef SDK_TWLLTD


/*---------------------------------------------------------------------------*
 * �֐��錾
 *---------------------------------------------------------------------------*/
static u8*  OSi_LoadImage(SPEC_DEST dest, IMAGE_OBJ_INDEX index, u32 *p_size);
void OSi_WaitVBlank(void);
void OSi_VBlankIntr(void);
void OSi_PrepareAttention(void);
void OSi_ShowAttention(void);



/*---------------------------------------------------------------------------*
  Name:         OSi_LoadImage

  Description:  �C���[�W�t�@�C���̃��[�h(��) �����ۂɂ̓|�C���^��Ԃ�����

  Arguments:    dest    -   ����d�����n
                index   -   ���[�h����t�@�C���̃C���f�b�N�X.
                p_size  -   �t�@�C���T�C�Y���i�[���� u32 �ւ̃|�C���^.
                            �s�v�Ȃ� NULL ���w�肵�Ė��������邱�Ƃ��ł���.

  Returns:      �C���[�W�f�[�^�̐擪�A�h���X��Ԃ�
 *---------------------------------------------------------------------------*/
static u8 *OSi_LoadImage(SPEC_DEST dest, IMAGE_OBJ_INDEX index, u32 *p_size)
{
    extern u8   _binary_attention_limited_01nbfc_bin[];
    extern u8   _binary_attention_limited_01nbfc_bin_end[];
    extern u8   _binary_attention_limited_01nbfs_bin[];
    extern u8   _binary_attention_limited_01nbfs_bin_end[];
    extern u8   _binary_attention_limited_02nbfc_bin[];
    extern u8   _binary_attention_limited_02nbfc_bin_end[];
    extern u8   _binary_attention_limited_02nbfs_bin[];
    extern u8   _binary_attention_limited_02nbfs_bin_end[];
    extern u8   _binary_attention_limited_korea_01nbfc_bin[];
    extern u8   _binary_attention_limited_korea_01nbfc_bin_end[];
    extern u8   _binary_attention_limited_korea_01nbfs_bin[];
    extern u8   _binary_attention_limited_korea_01nbfs_bin_end[];
    extern u8   _binary_attention_limited_korea_02nbfc_bin[];
    extern u8   _binary_attention_limited_korea_02nbfc_bin_end[];
    extern u8   _binary_attention_limited_korea_02nbfs_bin[];
    extern u8   _binary_attention_limited_korea_02nbfs_bin_end[];
    extern u8   _binary_attention_limited_nbfp_bin[];
    extern u8   _binary_attention_limited_nbfp_bin_end[];

    static u8 *ptr_table[SPEC_DEST_NUM][IMAGE_OBJ_NUM] =
    {
        {
            _binary_attention_limited_01nbfc_bin,
            _binary_attention_limited_01nbfs_bin,
            _binary_attention_limited_02nbfc_bin,
            _binary_attention_limited_02nbfs_bin,
            _binary_attention_limited_nbfp_bin
        },
        {
            _binary_attention_limited_korea_01nbfc_bin,
            _binary_attention_limited_korea_01nbfs_bin,
            _binary_attention_limited_korea_02nbfc_bin,
            _binary_attention_limited_korea_02nbfs_bin,
            _binary_attention_limited_nbfp_bin
        }
    };

    static u8 *ptr_end_table[SPEC_DEST_NUM][IMAGE_OBJ_NUM] =
    {
        {
            _binary_attention_limited_01nbfc_bin_end,
            _binary_attention_limited_01nbfs_bin_end,
            _binary_attention_limited_02nbfc_bin_end,
            _binary_attention_limited_02nbfs_bin_end,
            _binary_attention_limited_nbfp_bin_end
        },
        {
            _binary_attention_limited_korea_01nbfc_bin_end,
            _binary_attention_limited_korea_01nbfs_bin_end,
            _binary_attention_limited_korea_02nbfc_bin_end,
            _binary_attention_limited_korea_02nbfs_bin_end,
            _binary_attention_limited_nbfp_bin_end
        }
    };

    if(p_size)
    {
        *p_size = (u32)(ptr_end_table[dest][index] - ptr_table[dest][index]);
    }

    return (u8 *)ptr_table[dest][index];
}

/*---------------------------------------------------------------------------*
  Name:         OSi_WaitVBlank

  Description:  wait for V blank interrupt.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_WaitVBlank(void)
{
#if 0
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
#else
    // �X���b�h������������Ă��Ȃ����߃��[�v�őҋ@
    while(1)
    {
        if(OS_GetIrqCheckFlag() & OS_IE_V_BLANK) break;
    }
    OS_ClearIrqCheckFlag(OS_IE_V_BLANK);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OSi_VBlankIntr

  Description:  V blank interrupt vector.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         OS_ShowAttentionOfLimitedRom

  Description:  show string of attention to run limited mode on NITRO.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void OS_ShowAttentionOfLimitedRom(void)
{
    /* ���� */
    OSi_PrepareAttention();

    /* �摜�f�[�^�̃��[�h */
    {
        SPEC_DEST   dest = (*(u8*)(HW_ROM_HEADER_BUF + 0x1d) & 0x40) ? SPEC_DEST_KOREA : SPEC_DEST_NONE;

        u32     plt_size;
        void    const *data_plt     = OSi_LoadImage(dest, IMAGE_OBJ_PAL, &plt_size);
        void    const *data_01_chr  = OSi_LoadImage(dest, IMAGE_OBJ_01_CHR, NULL);
        void    const *data_01_scr  = OSi_LoadImage(dest, IMAGE_OBJ_01_SCR, NULL);
        void    const *data_02_chr  = OSi_LoadImage(dest, IMAGE_OBJ_02_CHR, NULL);
        void    const *data_02_scr  = OSi_LoadImage(dest, IMAGE_OBJ_02_SCR, NULL);

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

/*---------------------------------------------------------------------------*
  Name:         OSi_PrepareAttention

  Description:  �x����ʕ\���̏���

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/
void OSi_PrepareAttention(void)
{
    u16 gx_powcnt = reg_GX_POWCNT;

    /* �\����~ */
    reg_GX_DISPCNT      = 0;
    reg_GXS_DB_DISPCNT  = 0;

    /* �p���[�R���g���[�������� */
    if(!(gx_powcnt & REG_GX_POWCNT_LCD_MASK))
    {
        /* LCD �C�l�[�u���� OFF -> ON �ɕύX����ꍇ�� 100ms �҂� */
        SVC_WaitByLoop(HW_CPU_CLOCK_ARM9 / 40);
    }

    reg_GX_POWCNT = (u16)(REG_GX_POWCNT_DSEL_MASK | REG_GX_POWCNT_E2DG_MASK |
                            REG_GX_POWCNT_E2DGB_MASK | REG_GX_POWCNT_LCD_MASK);

    /* �}�X�^�[�P�x������ */
    reg_GX_MASTER_BRIGHT        = (u16)(1 << REG_GX_MASTER_BRIGHT_E_MOD_SHIFT);
    reg_GXS_DB_MASTER_BRIGHT    = reg_GX_MASTER_BRIGHT;

    /* ���ʕ\������ */
    reg_GX_VRAMCNT_A = (u8)((1 << REG_GX_VRAMCNT_A_MST_SHIFT) | (1 << REG_GX_VRAMCNT_A_E_SHIFT));
    reg_G2_BG0CNT    = (u16)((GX_BG_SCRSIZE_TEXT_256x256 << REG_G2_BG0CNT_SCREENSIZE_SHIFT) |
                             (GX_BG_COLORMODE_16 << REG_G2_BG0CNT_COLORMODE_SHIFT) |
                             (GX_BG_SCRBASE_0xf000 << REG_G2_BG0CNT_SCREENBASE_SHIFT) |
                             (GX_BG_CHARBASE_0x00000 << REG_G2_BG0CNT_CHARBASE_SHIFT) |
                             (0 << REG_G2_BG0CNT_PRIORITY_SHIFT));
    reg_G2_BG0OFS    = 0;
    reg_GX_DISPCNT  |= ((GX_BGMODE_0 << REG_GX_DISPCNT_BGMODE_SHIFT) |
                        (GX_PLANEMASK_BG0 << REG_GX_DISPCNT_DISPLAY_SHIFT));

    /* ����ʕ\������ */
    reg_GX_VRAMCNT_C    = (u8)((4 << REG_GX_VRAMCNT_C_MST_SHIFT) | (1 << REG_GX_VRAMCNT_C_E_SHIFT));
    reg_G2S_DB_BG0CNT   = (u16)((GX_BG_SCRSIZE_TEXT_256x256 << REG_G2S_DB_BG0CNT_SCREENSIZE_SHIFT) |
                                (GX_BG_COLORMODE_16 << REG_G2S_DB_BG0CNT_COLORMODE_SHIFT) |
                                (GX_BG_SCRBASE_0xf000 << REG_G2S_DB_BG0CNT_SCREENBASE_SHIFT) |
                                (GX_BG_CHARBASE_0x00000 << REG_G2S_DB_BG0CNT_CHARBASE_SHIFT) |
                                (0 << REG_G2S_DB_BG0CNT_PRIORITY_SHIFT));
    reg_G2S_DB_BG0OFS   = 0;
    reg_GXS_DB_DISPCNT |= ((GX_BGMODE_0 << REG_GXS_DB_DISPCNT_BGMODE_SHIFT) |
                          ((GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ) << REG_GXS_DB_DISPCNT_DISPLAY_SHIFT));
}

/*---------------------------------------------------------------------------*
  Name:         OSi_ShowAttention

  Description:  �x����ʂ̕\��

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/
void OSi_ShowAttention(void)
{

    /* �\���J�n */
    reg_GX_DISPCNT      |= (u32)(GX_DISPMODE_GRAPHICS << REG_GX_DISPCNT_MODE_SHIFT);
    reg_GXS_DB_DISPCNT  |= (u32)(REG_GXS_DB_DISPCNT_MODE_MASK);

    /* ���荞�݊֘A �ݒ� */
    reg_GX_DISPSTAT     |= REG_GX_DISPSTAT_VBI_MASK;
    OS_SetIrqFunction(OS_IE_V_BLANK, OSi_VBlankIntr);

    /* ���[�v */
    while(1)
    {
        OSi_WaitVBlank();
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_IsLimitedRomRunningOnTwl

  Description:  check running platform (only for Nitro-TWL limited mode)

  Arguments:    None

  Returns:      TRUE  : running on TWL
                FALSE : running on NITRO
 *---------------------------------------------------------------------------*/
BOOL OS_IsLimitedRomRunningOnTwl(void)
{
#ifdef SDK_ARM9
    u8 rom9 = reg_SCFG_A9ROM;
#else // SDK_ARM7
    u8 rom9 = (u8)(*(u8*)(HW_PRV_WRAM_SYSRV + HWi_WSYS08_WRAMOFFSET) >> HWi_WSYS08_ROM_ARM9SEC_SHIFT);
#endif

    return (rom9 & (REG_SCFG_A9ROM_SEC_MASK | REG_SCFG_A9ROM_RSEL_MASK)) == REG_SCFG_A9ROM_SEC_MASK;
}


#endif  // #ifdef SDK_TWLLTD
/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
