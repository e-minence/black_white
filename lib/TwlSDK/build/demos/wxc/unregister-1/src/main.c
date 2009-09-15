/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - unregister-1
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
    �����ɂ�邷�ꂿ�����ʐM���s���T���v���ł��B
    �����I�Ɏ��ӂ�unregister-1�f���Ɛڑ����܂��B
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <nitro/wxc.h>
#include "user.h"
#include "font.h"
#include "text.h"

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
#define     KEY_REPEAT_START    25     // �L�[���s�[�g�J�n�܂ł̃t���[����
#define     KEY_REPEAT_SPAN     10     // �L�[���s�[�g�̊Ԋu�t���[����


/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
// �L�[���͏��
typedef struct KeyInfo
{
    u16     cnt;                       // �����H���͒l
    u16     trg;                       // �����g���K����
    u16     up;                        // �����g���K����
    u16     rep;                       // �����ێ����s�[�g����

}
KeyInfo;

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void ModeSelect(void);          // �e�@/�q�@ �I�����
static void VBlankIntr(void);          // V�u�����N�����݃n���h��



// �ėp�T�u���[�`��
static void KeyRead(KeyInfo * pKey);
static void ClearString(int vram_num);
static void PrintString(s16 x, s16 y, u8 palette, char *text, ...);
static void InitializeAllocateSystem(void);


/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static KeyInfo gKey;                   // �L�[����
static s32 gFrame;                     // �t���[���J�E���^

#define VRAM_SIZE 2*32*32
static u8 g_screen[NUM_OF_SCREEN][VRAM_SIZE] ATTRIBUTE_ALIGN(32);

static TEXT_CTRL textctrl[NUM_OF_SCREEN];       // �e�L�X�g��ʂ̐�
TEXT_CTRL *tc[NUM_OF_SCREEN];          // ��L���|�C���^�ň���

static int vram_num[2];                // �㉺�e�X�N���[���̑Ώۃe�L�X�g�ԍ�

#define TEXT_HEAPBUF_SIZE 0x10000      // 2005/01/12 ���X��ڂɒ�`�������܂����B(�ƈ�)
static u8 text_heap_buffer[TEXT_HEAPBUF_SIZE] ATTRIBUTE_ALIGN(32);      // �e�L�X�g�\���p�o�b�t�@


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    int     i;
    // �e�평����
    OS_Init();
    OS_InitTick();

    (void)init_text_buf_sys((void *)&(text_heap_buffer[0]),
                            (void *)&(text_heap_buffer[TEXT_HEAPBUF_SIZE]));


    for (i = 0; i < NUM_OF_SCREEN; i++)
    {
        tc[i] = &(textctrl[i]);
        init_text(tc[i], (u16 *)&(g_screen[i]), FONT_YELLOW /* pal no. */ );
    }

    FX_Init();

    /* �\���ݒ菉���� */
	{
		GX_Init();
		GX_DispOff();
		GXS_DispOff();
        /* VRAM �N���A */
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
		 * ���C�����:
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
		 * �T�u���:
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
        /* �\���L���� */
		GX_DispOn();
		GXS_DispOn();
	}
    /* V�u�����N�ݒ� */
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    /* ���������蓖�ď����� */
    InitializeAllocateSystem();

    vram_num[0] = 0;                   /* ��X�N���[���̃e�L�X�g�ԍ� */
    vram_num[1] = 1;                   /* ���X�N���[���̃e�L�X�g�ԍ� */

    /* ���C�����[�v */
    for (gFrame = 0; TRUE; gFrame++)
    {
        text_buf_to_vram(tc[vram_num[0]]);
        text_buf_to_vram(tc[vram_num[1]]);

        /* �L�[���͏��擾 */
        KeyRead(&gKey);

        /* �ʐM��Ԃɂ�菈����U�蕪�� */
        switch (WXC_GetStateCode())
        {
        case WXC_STATE_END:
            ModeSelect();
            break;
        case WXC_STATE_READY:
	        PrintString(9, 11, 0xf, "WXC ready.");
            break;
        case WXC_STATE_ACTIVE:
			if (WXC_IsParentMode())
			{
	            PrintString(9, 11, 0xf, "Now parent...");
			}
			else
			{
				PrintString(9, 11, 0xf, "Now child...");
			}
            break;
        case WXC_STATE_ENDING:
	        PrintString(9, 11, 0xf, "WXC ending...");
            break;
        }
        if (gKey.trg & PAD_BUTTON_START)
        {
            //********************************
            (void)WXC_End();
            //********************************
        }

        // �u�u�����N�҂�
        OS_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeSelect

  Description:  �e�@/�q�@ �I����ʂł̏����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeSelect(void)
{
    PrintString(3, 10, 0xf, "Push A to start");

    if (gKey.trg & PAD_BUTTON_A)
    {
        /* ���ꂿ�����ʐM�̏�����, �o�^, �N�� */
        User_Init();
    }
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃n���h��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    DC_FlushRange(&(g_screen[vram_num[0]]), VRAM_SIZE);
    GX_LoadBG0Scr(&(g_screen[vram_num[0]]), 0, VRAM_SIZE);

    DC_FlushRange(&(g_screen[vram_num[1]]), VRAM_SIZE);
    GXS_LoadBG0Scr(&(g_screen[vram_num[1]]), 0, VRAM_SIZE);

    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  �L�[���͏���ҏW����B
                �����g���K�A�����g���K�A�����p�����s�[�g�����o�B

  Arguments:    pKey  - �ҏW����L�[���͏��\���́B

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
                pKey->trg |= (0x0001 << i);     // �����g���K
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // �����p�����s�[�g
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
                pKey->up |= (0x0001 << i);      // �����g���K
            }
        }
    }
    pKey->cnt = r;                     // �����H�L�[����
}

/*---------------------------------------------------------------------------*
  Name:         ClearString

  Description:  ���z�X�N���[�����N���A����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ClearString(int vram_num)
{
    MI_DmaClear32(0, (void *)&(g_screen[vram_num]), VRAM_SIZE);
}


/*---------------------------------------------------------------------------*
  Name:         PrintString

  Description:  ���z�X�N���[���ɕ������z�u����B�������32�����܂ŁB

  Arguments:    x       - ������̐擪��z�u���� x ���W( �~ 8 �h�b�g )�B
                y       - ������̐擪��z�u���� y ���W( �~ 8 �h�b�g )�B
                palette - �����̐F���p���b�g�ԍ��Ŏw��B
                text    - �z�u���镶����B�I�[������NULL�B
                ...     - ���z�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintString(s16 x, s16 y, u8 palette, char *text, ...)
{
    va_list vlist;
    char    temp[32 + 2];
    s32     i;

    va_start(vlist, text);
    (void)OS_VSNPrintf(temp, 32, text, vlist);
    va_end(vlist);
    *(u16 *)(&temp[31]) = 0x0000;
    for (i = 0;; i++)
    {
        if (temp[i] == 0x00)
        {
            break;
        }
        tc[1]->screen[(y * 32) + x + i] = (u16)((palette << 12) | temp[i]);
    }
}


/*---------------------------------------------------------------------------*
  Name:         InitializeAllocateSystem

  Description:  ���C����������̃A���[�i�ɂă����������ăV�X�e��������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void)
{
    void   *tempLo;
    OSHeapHandle hh;

    mprintf(" arena lo = %08x\n", OS_GetMainArenaLo());
    mprintf(" arena hi = %08x\n", OS_GetMainArenaHi());

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
