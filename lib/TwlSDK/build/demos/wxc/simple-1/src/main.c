/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - simple-1
  File:     main.c

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-11-15#$
  $Rev: 2414 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    無線によるすれちがい通信を行うサンプルです。
    自動的に周辺のsimple-1デモと接続します。
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <nitro/wxc.h>
#include "font.h"
#include "user.h"
#include "print.h"

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
#define     KEY_REPEAT_START    25     // キーリピート開始までのフレーム数
#define     KEY_REPEAT_SPAN     10     // キーリピートの間隔フレーム数


/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/
// キー入力情報
typedef struct KeyInfo
{
    u16     cnt;                       // 未加工入力値
    u16     trg;                       // 押しトリガ入力
    u16     up;                        // 離しトリガ入力
    u16     rep;                       // 押し維持リピート入力

}
KeyInfo;

/*---------------------------------------------------------------------------*
    内部関数定義
 *---------------------------------------------------------------------------*/
static void ModeSelect(void);          // 親機/子機 選択画面
static void ModeError(void);           // エラー表示画面
static void VBlankIntr(void);          // Vブランク割込みハンドラ



// 汎用サブルーチン
static void KeyRead(KeyInfo * pKey);
static void InitializeAllocateSystem(void);


/*---------------------------------------------------------------------------*
    内部変数定義
 *---------------------------------------------------------------------------*/
static KeyInfo gKey;                   // キー入力
static s32 gFrame;                     // フレームカウンタ

u16 gScreen[32 * 32];           // 仮想スクリーン


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  初期化及びメインループ。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    // 各種初期化
    OS_Init();
    OS_InitTick();

    FX_Init();

    /* 表示設定初期化 */
    {
        GX_Init();
        GX_DispOff();
        GXS_DispOff();
        /* VRAM クリア */
        GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
        MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
        (void)GX_DisableBankForLCDC();
        MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
        MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
        MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
        MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);
        /*
         * VRAM-A  = main-BG
         * VRAM-B  = main-OBJ
         * VRAM-HI = sub-BG
         * VRAM-J  = sub-OBJ
         */
        GX_SetBankForBG(GX_VRAM_BG_128_A);
        GX_SetBankForOBJ(GX_VRAM_OBJ_128_B);
        GX_SetBankForSubBG(GX_VRAM_SUB_BG_48_HI);
        GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
        /*
         * メイン画面:
         *   BG0 = text-BG
         *   OBJ = 2D mode
         */
        GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
        GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
        GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
        G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                         GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000,
                         GX_BG_EXTPLTT_01);
        G2_SetBG0Priority(0);
        G2_BG0Mosaic(FALSE);
        G2_SetBG0Offset(0, 0);
        GX_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
        GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));
        /*
         * サブ画面:
         *   BG0 = text-BG
         *   OBJ = 2D mode
         */
        GXS_SetGraphicsMode(GX_BGMODE_0);
        GXS_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
        GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
        G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                          GX_BG_SCRBASE_0xb800, GX_BG_CHARBASE_0x00000,
                          GX_BG_EXTPLTT_01);
        G2S_SetBG0Priority(0);
        G2S_BG0Mosaic(FALSE);
        G2S_SetBG0Offset(0, 0);
        GXS_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
        GXS_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));
        /* 表示有効化 */
        GX_DispOn();
        GXS_DispOn();
    }
    /* Vブランク設定 */
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    /* メモリ割り当て初期化 */
    InitializeAllocateSystem();

    /* メインループ */
    for (gFrame = 0; TRUE; gFrame++)
    {
        /* キー入力情報取得 */
        KeyRead(&gKey);

        // スクリーンクリア
        ClearStringY(1);
        ClearStringY(2);
        
        /* 通信状態により処理を振り分け */
        switch (WXC_GetStateCode())
        {
        case WXC_STATE_END:
            ModeSelect();
            break;
        case WXC_STATE_ENDING:
            break;
        case WXC_STATE_ACTIVE:
            if(WXC_IsParentMode() == TRUE)
            {
                
                PrintString(9, 2, 0xf, "Now parent...");
            }
            else
            {
                PrintString(9, 2, 0xf, "Now child...");
            }
            break;
        }
        if (gKey.trg & PAD_BUTTON_START)
        {
            (void)WXC_End();
        }

        // Ｖブランク待ち
        OS_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeSelect

  Description:  親機/子機 選択画面での処理。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeSelect(void)
{
    PrintString(3, 1, 0xf, "Push A to start");

    if (gKey.trg & PAD_BUTTON_A)
    {
        User_Init();
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeError

  Description:  エラー表示画面での処理。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeError(void)
{
    PrintString(5, 10, 0x1, "======= ERROR! =======");
    PrintString(5, 13, 0xf, " Fatal error occured.");
    PrintString(5, 14, 0xf, "Please reboot program.");
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  Ｖブランク割込みハンドラ

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    DC_FlushRange(&(gScreen), sizeof(gScreen));
    GXS_LoadBG0Scr(&(gScreen), 0, sizeof(gScreen));

    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  キー入力情報を編集する。
                押しトリガ、離しトリガ、押し継続リピートを検出。

  Arguments:    pKey  - 編集するキー入力情報構造体。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void KeyRead(KeyInfo * pKey)
{
    static u16 repeat_count[12];
    int     i;
    u16     r;

    r = PAD_Read();
    pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < 12; i++)
    {
        if (r & (0x0001 << i))
        {
            if (!(pKey->cnt & (0x0001 << i)))
            {
                pKey->trg |= (0x0001 << i);     // 押しトリガ
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // 押し継続リピート
                    repeat_count[i] = KEY_REPEAT_START - KEY_REPEAT_SPAN;
                }
                else
                {
                    repeat_count[i]++;
                }
            }
        }
        else
        {
            if (pKey->cnt & (0x0001 << i))
            {
                pKey->up |= (0x0001 << i);      // 離しトリガ
            }
        }
    }
    pKey->cnt = r;                     // 未加工キー入力
}

/*---------------------------------------------------------------------------*
  Name:         InitializeAllocateSystem

  Description:  メインメモリ上のアリーナにてメモリ割当てシステムを初期化する。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void)
{
    void   *tempLo;
    OSHeapHandle hh;
    
    OS_Printf(" arena lo = %08x\n", OS_GetMainArenaLo());
    OS_Printf(" arena hi = %08x\n", OS_GetMainArenaHi());

    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
