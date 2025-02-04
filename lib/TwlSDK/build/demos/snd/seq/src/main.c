/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SND - demos - seq
  File:     main.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-02#$
  $Rev: 8827 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// USAGE:
//  A : start sequence
//  B : stop sequence
//---------------------------------------------------------------------------

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

#include "seq.h"
#include "channel.h"

#define MIDI_BUF_SIZE 8*1024
#define CHANNEL_NUM 4
#define ORIGINAL_KEY 67

u16     Cont;
u16     Trg;

u8      midifile_buf[MIDI_BUF_SIZE];

static SeqHandle handle;

void    VBlankIntr(void);
void    FileLoad(void *buf, const char *filename);
void    MidiCallback(const u8 *midi_event);


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  メイン関数

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{
    // 初期化
    OS_Init();
    GX_Init();
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    FS_Init(FS_DMA_NOT_USE);
    SND_Init();

    SeqInit(SEQ_CLOCK_INTERVAL_NITRO_VBLANK, MidiCallback);
    ChInit();

    // Vブランク割り込み設定
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr(TRUE);

    // print usage
    OS_Printf("=================================\n");
    OS_Printf("USAGE:\n");
    OS_Printf(" A : start sequence\n");
    OS_Printf(" B : stop sequence\n");
    OS_Printf("=================================\n");

    // MIDIファイルをロード
    FileLoad(midifile_buf, "/elise.mid");

    while (1)
    {
        u16     ReadData;

        OS_WaitVBlankIntr();

        // ＡＲＭ７コマンド応答受信
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
        {
        }

        // キー入力読み込み
        ReadData = PAD_Read();
        Trg = (u16)(ReadData & (ReadData ^ Cont));
        Cont = ReadData;

        if (Trg & PAD_BUTTON_A)
        {
            ChAllNoteOff();
            (void)SeqPlay(&handle, midifile_buf);
        }

        if (Trg & PAD_BUTTON_B)
        {
            (void)SeqStop(&handle);
            ChAllNoteOff();
        }

        (void)SeqMain(&handle);

        // コマンドフラッシュ（フラッシュして即座に実行を要求）
        (void)SND_FlushCommand(SND_COMMAND_NOBLOCK | SND_COMMAND_IMMEDIATE);
    }
}

void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking VBlank interrupt
}

void FileLoad(void *buf, const char *filename)
{
    FSFile  file;
    u32     file_size;

    FS_InitFile(&file);
    (void)FS_OpenFileEx(&file, filename, FS_FILEMODE_R);
    file_size = FS_GetFileLength(&file);
    (void)FS_ReadFile(&file, buf, (s32)file_size);
    (void)FS_CloseFile(&file);
}

/*---------------------------------------------------------------------------*
  Name:         MidiCallback

  Description:  SeqMain() でシーケンス処理中に
                MIDIイベントが発生した時に呼び出される関数

  Arguments:    midi_event - 発生したMIDIイベント列

  Returns:      None
 *---------------------------------------------------------------------------*/
void MidiCallback(const u8 *midi_event)
{
    static u8 running_status;
    u8      midi_buf[4];
    int     i;

    // MIDIイベント別のバイトサイズ
    const u8 midi_byte_size[8] = {
        /*  8x,9x,Ax,Bx,Cx,Dx,Ex,Fx */
        2, 2, 2, 2, 1, 1, 2, 0
    };

    // ランニングステータスのチェック
    if (midi_event[0] < 0x80)
    {
        midi_buf[0] = running_status;
        for (i = 0; i < midi_byte_size[(midi_buf[0] >> 4) - 8]; i++)
        {
            midi_buf[i + 1] = midi_event[i];
        }
    }
    else
    {
        running_status = midi_event[0];
        midi_buf[0] = midi_event[0];
        for (i = 0; i < midi_byte_size[(midi_buf[0] >> 4) - 8]; i++)
        {
            midi_buf[i + 1] = midi_event[i + 1];
        }
    }

    ChSetEvent(midi_buf);
}
