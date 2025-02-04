/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos - OS - forChina-fs
  File:     os_china_override.c

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-05-28 #$
  $Rev: 10641 $
  $Author: mizutani_nakaba $
 *---------------------------------------------------------------------------*/


#include <nitro.h>


/*---------------------------------------------------------------------------*/
/* constants */

/* 起動表示のフェードおよび停止フレーム数 */
#define     OSi_FORCHINA_FADE_SPAN          32
#define     OSi_FORCHINA_DISP_SPAN          128

/* テーブルで管理する各種データイメージのインデックス */
typedef enum
{
    IMAGE_NOTES_CHR,
    IMAGE_NOTES_SCR,
    IMAGE_NOTES_DIGIT,
    IMAGE_LOGO_CHR,
    IMAGE_LOGO_SCR,
    IMAGE_LOGO_PAL,
    IMAGE_MAX
}
ImageIndex;


/*---------------------------------------------------------------------------*/
/* declarations */

static u8 *LoadImage(ImageIndex index, u32 *p_size);
static void WaitForNextFrame(void);
static void VBlankIntr(void);
static void SetISBNString(const char **isbn, BOOL disp_isbn);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         LoadImage

  Description:  充分なサイズのメモリを確保して指定したファイルをロード.

  Arguments:    index   -   ロードするファイルのインデックス.
                p_size  -   ファイルサイズを格納する u32 へのポインタ.
                            不要なら NULL を指定して無視させることができる.

  Returns:      呼び出しの内部で確保され, ファイルがロードされたメモリ.
                OS_ShowAttentionChina() の終了時には
                OS_ClearAlloc() でヒープごと破棄するため,
                ここで返されるメモリの解放処理は明示的に行わなくても良い.
 *---------------------------------------------------------------------------*/
static u8 *LoadImage(ImageIndex index, u32 *p_size)
{
    void   *ptr = NULL;
    /* *INDENT-OFF* */
    static const char *(path_table[IMAGE_MAX]) =
    {
        "notes_forChina_chrData.bin",
        "notes_forChina_scrData.bin",
        "digit_chrData.bin",
        "logo_forChina_chrData.bin",
        "logo_forChina_scrData.bin",
        "logo_forChina_palData.bin",
    };
    /* *INDENT-ON* */ 
    const char *path = path_table[index];

    FSFile  file[1];

    FS_InitFile(file);
    if (!FS_OpenFileEx(file, path, FS_FILEMODE_R))
    {
        OS_TPanic("failed to open file \"%s\"", path);
    }
    else
    {
        u32     len = FS_GetFileLength(file);
        ptr = OS_Alloc(len);
        if (!ptr)
        {
            OS_TPanic("failed to allocate memory for image \"%s\"", path);
        }
        else
        {
            if (p_size)
            {
                *p_size = len;
            }
            if (FS_ReadFile(file, ptr, (int)len) != len)
            {
                OS_TPanic("failed to read file \"%s\"", path);
            }
        }
        (void)FS_CloseFile(file);
    }

    return (u8 *)ptr;
}

/*---------------------------------------------------------------------------*
  Name:         WaitForNextFrame

  Description:  本体開閉時の処理とVブランク待機, およびサウンド更新処理。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WaitForNextFrame(void)
{
    /* フタ閉じ確認 */
    if (PAD_DetectFold())
    {
        PM_GoSleepMode(PM_TRIGGER_COVER_OPEN | PM_TRIGGER_CARD, 0, 0);
    }
    OS_WaitVBlankIntr();
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  排他起動制御画面における V ブランク割り込みベクトル。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         SetISBNString

  Description:  ISBN 番号等を スクリーンデータ内の所定の位置に反映させる。

  Arguments:    isbn    -   ISBN 番号等に関する文字列配列。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetISBNString(const char **isbn, BOOL disp_isbn)
{
    s32     i, j;
    const struct
    {
        u8      x, y;                  /* 表示先頭オフセット */
        u8      length;                /* 表示文字数 */
    }
    pos[] =
    {
        {
        80, 142, 17}
        ,                              /* ISBN */
        {
        125, 157, 12}
        ,                              /* 合同登記号 */
        {
        106, 173, 4}
        ,                              /* 新出音管(左) */
        {
        139, 173, 3}
        ,                              /* 新出音管(右) */
    }
    ;
    const int   count = sizeof(pos) / sizeof(*pos); /* 総行数 */
    const int   digit_num = 12;                     /* 桁表示キャラクタ数 ("0123456789- ") */
    u16         pos_x;
    u16         pos_y;
    u16         index;
    u16         width;

    GXOamAttr *dst = (GXOamAttr *)HW_DB_OAM;
    for (i = disp_isbn ? 0 : 1; i < count; i++)
    {
        pos_x = pos[i].x;
        pos_y = pos[i].y;

        for (j = 0; j < pos[i].length; ++j)
        {
            if (isbn[i][j] == '-')
            {
                index = 10;
                width = 5;
            }
            else if ((isbn[i][j] >= '0') && (isbn[i][j] <= '9'))
            {
                index = (u16)(isbn[i][j] - '0');
                width = 7;
            }
            else
            {
                index = 11;
                width = 7;
            }

            dst[0].attr01 = (u32)((pos_x << 16) | (pos_y << 0));
            dst[0].attr2  = index;
            dst[1].attr01 = (u32)((pos_x << 16) | (pos_y + 8 << 0));
            dst[1].attr2  = (u16)(index + digit_num);

            pos_x   += width;
            dst     += 2;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_ShowAttentionChina

  Description:  中国対応ロゴ、及び注意画面を所定時間表示する。

  Arguments:    isbn    -   ISBN 番号等に関する文字列配列。
                param   -   ISBN 番号表示に関する動作を指定します。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_ShowAttentionChina(const char **isbn, OSChinaIsbn param)
{
    /* レジスタ退避用構造体 */
    struct
    {
        u32     gx_dispcnt;
        u32     gxs_dispcnt;
        u16     gx_powcnt;
        u16     gx_dispstat;
        u16     gx_bright;
        u16     gxs_bright;
        u16     gx_bg0cnt;
        u16     gxs_bg0cnt;
        u32     gx_bg0ofs;
        u32     gxs_bg0ofs;
        u8      gx_vramcnt_a;
        u8      gx_vramcnt_c;
        u8      gx_vramcnt_d;
        u8      reserved[1];

        OSIrqFunction irqFunction;
        OSIrqMask irqMask;
        BOOL    irq;
        OSIntrMode interrupts;

    }
    shelter;

    /* 前処理 */
    {
        /* 各レジスタ退避 */
        shelter.gx_dispstat = reg_GX_DISPSTAT;
        shelter.gx_dispcnt = reg_GX_DISPCNT;
        shelter.gxs_dispcnt = reg_GXS_DB_DISPCNT;
        shelter.gx_bright = reg_GX_MASTER_BRIGHT;
        shelter.gxs_bright = reg_GXS_DB_MASTER_BRIGHT;
        shelter.gx_powcnt = reg_GX_POWCNT;
        shelter.gx_vramcnt_a = reg_GX_VRAMCNT_A;
        shelter.gx_vramcnt_c = reg_GX_VRAMCNT_C;
        shelter.gx_vramcnt_d = reg_GX_VRAMCNT_D;
        shelter.gx_bg0cnt = reg_G2_BG0CNT;
        shelter.gxs_bg0cnt = reg_G2S_DB_BG0CNT;
        shelter.gx_bg0ofs = reg_G2_BG0OFS;
        shelter.gxs_bg0ofs = reg_G2S_DB_BG0OFS;

        /* 表示停止 */
        reg_GX_DISPCNT = 0;
        reg_GXS_DB_DISPCNT = 0;

        /* パワーコントロール初期化 */
        if (!(shelter.gx_powcnt & REG_GX_POWCNT_LCD_MASK))
        {
            /* LCD イネーブルを OFF -> ON に変更する場合は 100ms 待つ */
            SVC_WaitByLoop(HW_CPU_CLOCK_ARM9 / 40);
        }
        reg_GX_POWCNT = (u16)(REG_GX_POWCNT_DSEL_MASK | REG_GX_POWCNT_E2DG_MASK |
                              REG_GX_POWCNT_E2DGB_MASK | REG_GX_POWCNT_LCD_MASK);

        /* マスター輝度初期化 */
        reg_GX_MASTER_BRIGHT = (u16)((1 << REG_GX_MASTER_BRIGHT_E_MOD_SHIFT) | 16);
        reg_GXS_DB_MASTER_BRIGHT = reg_GX_MASTER_BRIGHT;

        /* FS のために IRQ 割り込みを有効化 */
        shelter.irq = OS_EnableIrq();
        shelter.interrupts = OS_EnableInterrupts();

        /* メモリ管理および FS の初期化 */
        {
            void   *tmp;
            OSHeapHandle hh;
            tmp = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
            OS_SetArenaLo(OS_ARENA_MAIN, tmp);
            hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
            hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
            FS_Init(FS_DMA_NOT_USE);
        }

        /* Logo 画面表示準備 */
        {
            reg_GX_VRAMCNT_A =
                (u8)((1 << REG_GX_VRAMCNT_A_MST_SHIFT) | (1 << REG_GX_VRAMCNT_A_E_SHIFT));
            reg_G2_BG0CNT =
                (u16)((GX_BG_SCRSIZE_TEXT_256x256 << REG_G2_BG0CNT_SCREENSIZE_SHIFT) |
                      (GX_BG_COLORMODE_256 << REG_G2_BG0CNT_COLORMODE_SHIFT) | (GX_BG_SCRBASE_0xf000
                                                                                <<
                                                                                REG_G2_BG0CNT_SCREENBASE_SHIFT)
                      | (GX_BG_CHARBASE_0x00000 << REG_G2_BG0CNT_CHARBASE_SHIFT) | (GX_BG_EXTPLTT_01
                                                                                    <<
                                                                                    REG_G2_BG0CNT_BGPLTTSLOT_SHIFT)
                      | (0 << REG_G2_BG0CNT_PRIORITY_SHIFT));
            reg_G2_BG0OFS = 0;
            reg_GX_DISPCNT |= ((GX_BGMODE_0 << REG_GX_DISPCNT_BGMODE_SHIFT) |
                               (GX_PLANEMASK_BG0 << REG_GX_DISPCNT_DISPLAY_SHIFT));

            /* キャラクタデータをVRAMにロード */
            {
                void   *const src = LoadImage(IMAGE_LOGO_CHR, NULL);
                MI_UncompressLZ16(src, (u32 *)HW_BG_VRAM);
            }
            /* スクリーンデータをVRAMにロード */
            {
                u16     *src = (u16*)LoadImage(IMAGE_LOGO_SCR, NULL);
                u16     *dst;
                s32     i;
                s32     j;

                // 12x12(96x96pixel) のスクリーンデータを中心に配置
                SVC_CpuClearFast(0, (u32 *)(HW_BG_VRAM + 0xf000), 32*24*2);
                dst = (u16*)(HW_BG_VRAM + 0xf000 + ((6*32+10)*2));

                for (i = 0; i < 12; i++, dst += 32)
                {
                    for (j = 0; j < 12; j++, src++)
                    {
                        dst[j] = *src;
                    }
                }
            }
            /* パレットデータをVRAMにロード */
            {
                u32     size;
                void   *const src = LoadImage(IMAGE_LOGO_PAL, &size);
                SVC_CpuCopyFast(src, (u32 *)(HW_BG_PLTT), size);
            }
        }

        /* Notes 画面表示準備 */
        {
            reg_GX_VRAMCNT_C =
                (u8)((4 << REG_GX_VRAMCNT_C_MST_SHIFT) | (1 << REG_GX_VRAMCNT_C_E_SHIFT));
            reg_GX_VRAMCNT_D =
                (u8)((4 << REG_GX_VRAMCNT_D_MST_SHIFT) | (1 << REG_GX_VRAMCNT_D_E_SHIFT));
            reg_G2S_DB_BG0CNT =
                (u16)((GX_BG_SCRSIZE_TEXT_256x256 << REG_G2S_DB_BG0CNT_SCREENSIZE_SHIFT) |
                      (GX_BG_COLORMODE_16 << REG_G2S_DB_BG0CNT_COLORMODE_SHIFT) |
                      (GX_BG_SCRBASE_0xf000 << REG_G2S_DB_BG0CNT_SCREENBASE_SHIFT) |
                      (GX_BG_CHARBASE_0x00000 << REG_G2S_DB_BG0CNT_CHARBASE_SHIFT) |
                      (GX_BG_EXTPLTT_01 << REG_G2S_DB_BG0CNT_BGPLTTSLOT_SHIFT) | (0 <<
                                                                                  REG_G2S_DB_BG0CNT_PRIORITY_SHIFT));
            reg_G2S_DB_BG0OFS = 0;
            reg_GXS_DB_DISPCNT |= ((GX_BGMODE_0 << REG_GXS_DB_DISPCNT_BGMODE_SHIFT) |
                                   ((GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ) <<
                                    REG_GXS_DB_DISPCNT_DISPLAY_SHIFT));

            /* キャラクタデータをVRAMにロード */
            {
                u32     size;
                void   *src;
                /* BG テキスト */
                src = LoadImage(IMAGE_NOTES_CHR, &size);
                MI_UncompressLZ16(src, (u32 *)(HW_DB_BG_VRAM));
                /* OBJ テキスト */
                src = LoadImage(IMAGE_NOTES_DIGIT, &size);
                MI_UncompressLZ16(src, (u32 *)(HW_DB_OBJ_VRAM));
                SVC_CpuClear(0xC0, (void *)HW_DB_OAM, HW_OAM_SIZE, 32);
            }
            /* スクリーンデータをVRAMにロード */
            {
                u32     size;
                void   *const src = LoadImage(IMAGE_NOTES_SCR, &size);
                SVC_CpuCopyFast(src, (u32 *)(HW_DB_BG_VRAM + 0xf000), size);
            }

            // ISBN文字列の表示設定
            SetISBNString(isbn, (BOOL)(param != OS_CHINA_ISBN_NO_DISP));

            /* パレット編集 */
            *((u16 *)(HW_DB_BG_PLTT + 0)) = 0x7fff;
            *((u16 *)(HW_DB_BG_PLTT + 2)) = 0x0000;
            *((u16 *)(HW_DB_OBJ_PLTT + 0)) = 0x7fff;
            *((u16 *)(HW_DB_OBJ_PLTT + 2)) = 0x0000;
        }

        /* 表示開始 */
        reg_GX_DISPCNT |= (u32)(GX_DISPMODE_GRAPHICS << REG_GX_DISPCNT_MODE_SHIFT);
        reg_GXS_DB_DISPCNT |= (u32)(REG_GXS_DB_DISPCNT_MODE_MASK);

        /* 割り込み関連 設定 */
        reg_GX_DISPSTAT |= REG_GX_DISPSTAT_VBI_MASK;
        shelter.irqFunction = OS_GetIrqFunction(OS_IE_V_BLANK);
        OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
        shelter.irqMask = OS_EnableIrqMask(OS_IE_V_BLANK);

    }

    /* 主処理 */
    {
        s32     i;
        u16     pad_old;
        u16     pad;

        /* フェードイン */
        for (i = 0; i < OSi_FORCHINA_FADE_SPAN; i++)
        {
            reg_GX_MASTER_BRIGHT = (u16)((1 << REG_GX_MASTER_BRIGHT_E_MOD_SHIFT) |
                                         16 - ((i + 1) * 16 / OSi_FORCHINA_FADE_SPAN));
            reg_GXS_DB_MASTER_BRIGHT = reg_GX_MASTER_BRIGHT;
            WaitForNextFrame();
        }

        /* ボタン入力によりスキップ */
        pad_old = PAD_Read();
        for (i = 0; i < OSi_FORCHINA_DISP_SPAN; i++)
        {
            pad = PAD_Read();
            if ((param == OS_CHINA_ISBN_CHECK_ROM) && (~pad_old & pad))
            {
                /* 任意のボタン押しトリガを検出した場合、スキップ */
                break;
            }
            pad_old = pad;
            WaitForNextFrame();

            // 検閲用ROMではボタン押しまで抜けない
            if(param == OS_CHINA_ISBN_CHECK_ROM) i = 0;
        }

        /* フェードアウト */
        for (i = 0; i < OSi_FORCHINA_FADE_SPAN; i++)
        {
            reg_GX_MASTER_BRIGHT = (u16)((1 << REG_GX_MASTER_BRIGHT_E_MOD_SHIFT) |
                                         (i + 1) * 16 / OSi_FORCHINA_FADE_SPAN);
            reg_GXS_DB_MASTER_BRIGHT = reg_GX_MASTER_BRIGHT;
            WaitForNextFrame();
        }
    }

    /* 後処理 */
    {

        /* FS およびメモリ管理の終了 */
        {
            FS_End();
            OS_ClearAlloc(OS_ARENA_MAIN);
        }

        /* 表示停止 */
        reg_GX_DISPCNT &= ~(GX_DISPMODE_GRAPHICS << REG_GX_DISPCNT_MODE_SHIFT);
        reg_GXS_DB_DISPCNT &= ~REG_GXS_DB_DISPCNT_MODE_MASK;

        /* 割り込み関連 復帰 */
        (void)OS_RestoreInterrupts(shelter.interrupts);
        (void)OS_RestoreIrq(shelter.irq);
        (void)OS_SetIrqMask(shelter.irqMask);
        OS_SetIrqFunction(OS_IE_V_BLANK, shelter.irqFunction);
        reg_GX_DISPSTAT &= ~REG_GX_DISPSTAT_VBI_MASK;

        /* 使用した VRAM をクリア ( サイズが大きいので初期値の復帰はできない ) */
        SVC_CpuClearFast(0, (u32 *)HW_BG_VRAM, 0x10000);
        SVC_CpuClearFast(0, (u32 *)HW_BG_PLTT, 0x200);
        SVC_CpuClearFast(0, (u32 *)HW_DB_BG_VRAM, 0x10000);
        SVC_CpuClearFast(0, (u32 *)HW_DB_BG_PLTT, 0x04);
        SVC_CpuClearFast(0, (u32 *)HW_DB_OAM, HW_OAM_SIZE);

        /* 各レジスタ復帰 */
        reg_G2S_DB_BG0OFS = shelter.gxs_bg0ofs;
        reg_G2_BG0OFS = shelter.gx_bg0ofs;
        reg_G2S_DB_BG0CNT = shelter.gxs_bg0cnt;
        reg_G2_BG0CNT = shelter.gx_bg0cnt;
        reg_GX_VRAMCNT_D = shelter.gx_vramcnt_d;
        reg_GX_VRAMCNT_C = shelter.gx_vramcnt_c;
        reg_GX_VRAMCNT_A = shelter.gx_vramcnt_a;
        reg_GX_POWCNT = shelter.gx_powcnt;
        if (!(shelter.gx_powcnt & REG_GX_POWCNT_LCD_MASK))
        {
            /* LCD イネーブルを ON -> OFF に変更した場合は 100ms 待つ */
            SVC_WaitByLoop(HW_CPU_CLOCK_ARM9 / 40);
        }
        reg_GXS_DB_MASTER_BRIGHT = shelter.gxs_bright;
        reg_GX_MASTER_BRIGHT = shelter.gx_bright;
        reg_GXS_DB_DISPCNT = shelter.gxs_dispcnt;
        reg_GX_DISPCNT = shelter.gx_dispcnt;
        reg_GX_DISPSTAT = shelter.gx_dispstat;
    }
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
