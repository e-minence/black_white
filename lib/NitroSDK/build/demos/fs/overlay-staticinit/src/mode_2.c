/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - demos - overlay-staticinit
  File:     mode_2.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mode_2.c,v $
  Revision 1.2  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.1  2005/01/19 07:44:45  yosizaki
  initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include	"mode.h"
#include	"draw.h"

/*
 * NitroStaticInit() を static initializer に指定するには
 * このヘッダをインクルードします.
 */
#include <nitro/sinit.h>


/*****************************************************************************/
/* constant */


/*****************************************************************************/
/* variable */

/* タッチペン押下状態 */
static BOOL is_tp_on;
static Point bak_pos, cur_pos;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         MyUpdateFrame

  Description:  現在のモードで 1 フレーム分だけ内部状態を更新します.

  Arguments:    frame_count      現在の動作フレームカウント.
                input            入力情報の配列.
                player_count     現在のプレーヤー総数. (input の有効要素数)
                own_player_id    自身のプレーヤー番号.

  Returns:      現在のモードが今回のフレームで終了するならば FALSE を,
                そうでなければ TRUE を返します.
 *---------------------------------------------------------------------------*/
static BOOL MyUpdateFrame(int frame_count,
                          const InputData * input, int player_count, int own_player_id)
{
    (void)frame_count;
    (void)player_count;

    /* タッチペンの入力状態を更新 */
    bak_pos = cur_pos;
    is_tp_on = IS_INPUT_(input[own_player_id], PAD_BUTTON_TP, push);
    if (is_tp_on)
    {
        cur_pos.x = input[own_player_id].tp.x;
        cur_pos.y = input[own_player_id].tp.y;
    }
    /* タッチペン以外の何かを押したら終了 */
    return !IS_INPUT_(input[own_player_id], PAD_ALL_MASK, push);
}

/*---------------------------------------------------------------------------*
  Name:         MyDrawFrame

  Description:  現在のモードで内部状態をもとに描画更新します.

  Arguments:    frame_count      現在の動作フレームカウント.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyDrawFrame(int frame_count)
{
    /* 初回フレームは画面をクリア */
    if (frame_count == 0)
    {
        FillRect(0, 0, HW_LCD_WIDTH, HW_LCD_HEIGHT, GX_RGB(0, 0, 0));
        SetTextColor(GX_RGB(31, 31, 31));
        DrawText(0, 10, "%s", __FILE__);
        DrawText(30, 40, "touch screen : draw line");
        DrawText(30, 50, "press any key to return");
    }
    /* 現在位置と軌跡を描画 */
    FillRect(cur_pos.x - 2, cur_pos.y - 2, 4, 4, GX_RGB(0, 31, 0));
    if (is_tp_on)
        DrawLine(bak_pos.x, bak_pos.y, cur_pos.x, cur_pos.y, GX_RGB(16, 16, 31));
}

/*---------------------------------------------------------------------------*
  Name:         MyEndFrame

  Description:  現在のモードを終了します.

  Arguments:    p_next_mode      次のモードを明示的に指定する場合は.
                                 このポインタが指す先にIDを上書きします.
                                 特に指定しない場合, 現在のモードを呼び出した
                                 モードが選択されます.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyEndFrame(FSOverlayID *p_next_mode)
{
    (void)p_next_mode;
}

/*---------------------------------------------------------------------------*
  Name:         NitroStaticInit

  Description:  static initializer としての自動初期化関数.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void NitroStaticInit(void)
{
    UpdateFrame = MyUpdateFrame;
    DrawFrame = MyDrawFrame;
    EndFrame = MyEndFrame;

    /* モードごとに必要な初期化処理をここで行います. */

    is_tp_on = FALSE;
    cur_pos.x = HW_LCD_WIDTH / 2;
    cur_pos.y = HW_LCD_HEIGHT / 2;
    bak_pos = cur_pos;
    SetClearColor(RGB555_CLEAR);
}
