/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - wxc-dataShare
  File:     disp.c

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

//----------------------------------------------------------------------
// ������\���̊�{�I�ȋ@�\.
//----------------------------------------------------------------------

#include <nitro.h>
#include "disp.h"


//============================================================================
//  �v���g�^�C�v�錾
//============================================================================
static void VramClear(void);
static void ObjInitForPrintStr(void);
static void BgInitForPrintStr(void);


//============================================================================
//  �萔��`
//============================================================================

#define BGSTR_MAX_LENGTH        32

//============================================================================
//  �ϐ���`
//============================================================================

/* ���z�X�N���[�� */
static u16 vscr[32 * 32];

/* V �u�����N�]���p�e���|���� OAM */
static GXOamAttr oamBak[128];


/* �e��`��p */
extern const u32 sampleFontCharData[8 * 0xe0];
extern const u16 samplePlttData[16][16];


//============================================================================
//  �֐���`
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         BgInitForPrintStr

  Description:  BG�����`��̏����� (BG0�Œ�, VRAM-C, BG���[�h0)

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void BgInitForPrintStr(void)
{
    GX_SetBankForBG(GX_VRAM_BG_128_C);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf800,      /* SCR �x�[�X�u���b�N 31 */
                     GX_BG_CHARBASE_0x00000,    /* CHR �x�[�X�u���b�N 0 */
                     GX_BG_EXTPLTT_01);
    G2_SetBG0Priority(0);
    G2_BG0Mosaic(FALSE);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    G2_SetBG0Offset(0, 0);

    GX_LoadBG0Char(sampleFontCharData, 0, sizeof(sampleFontCharData));
    GX_LoadBGPltt(samplePlttData, 0, sizeof(samplePlttData));

    MI_CpuFillFast((void *)vscr, 0, sizeof(vscr));
    DC_FlushRange(vscr, sizeof(vscr));
    /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(vscr, 0, sizeof(vscr));
}

/*---------------------------------------------------------------------------*
  Name:         ObjInitForPrintStr

  Description:  OBJ�̏����� (VRAM-B, 2D���[�h)

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void ObjInitForPrintStr(void)
{
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_B);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    MI_DmaFill32(3, oamBak, 0xc0, sizeof(oamBak));

    GX_LoadOBJ(sampleFontCharData, 0, sizeof(sampleFontCharData));
    GX_LoadOBJPltt(samplePlttData, 0, sizeof(samplePlttData));

}

/*---------------------------------------------------------------------------*
  Name:         VramClear

  Description:  VRAM�N���A

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void VramClear(void)
{
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();
    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);
}

/*---------------------------------------------------------------------------*
  Name:         DispInit

  Description:  �`�揉����

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void DispInit(void)
{
    /* �`��ݒ菉���� */
    VramClear();

    // OBJ������
    ObjInitForPrintStr();

    // BG������
    BgInitForPrintStr();

    GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);

}


/*---------------------------------------------------------------------------*
  Name:         DispOn

  Description:  ��ʕ\��

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void DispOn(void)
{
    /* �\���J�n */
    GX_DispOn();
    GXS_DispOn();
}

/*---------------------------------------------------------------------------*
  Name:         DispOff

  Description:  ��ʔ�\��

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void DispOff(void)
{
    /* �\���J�n */
    GX_DispOff();
    GXS_DispOff();
}




/*---------------------------------------------------------------------------*
  Name:         DispVBlankFunc

  Description:  �`��VBlank����

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void DispVBlankFunc(void)
{
    //---- �n�`�l�̍X�V
    DC_FlushRange(oamBak, sizeof(oamBak));
    /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
    // DC_WaitWriteBufferEmpty();
    MI_DmaCopy32(3, oamBak, (void *)HW_OAM, sizeof(oamBak));

    //---- BG�X�N���[���X�V
    DC_FlushRange(vscr, sizeof(vscr));
    /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(vscr, 0, sizeof(vscr));
}

/*---------------------------------------------------------------------------*
  Name:         BgPutStringN

  Description:  BG��N�����o��

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void BgPutStringN(s32 x, s32 y, s32 palette, const char *text, s32 num)
{
    s32 i;
    if (num > BGSTR_MAX_LENGTH)
    {
        num = BGSTR_MAX_LENGTH;
    }

    for (i = 0; i < num; i++)
    {
        if (text[i] == '\0')
        {
            break;
        }
        BgPutChar(x + i, y, palette, text[i]);
    }
}

/*---------------------------------------------------------------------------*
  Name:         BgPutChar

  Description:  BG�P�����o��

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void BgPutChar(s32 x, s32 y, s32 palette, s8 c)
{
    vscr[((y * 32) + x) % (32 * 32)] = (u16)((palette << 12) | c);
}

/*---------------------------------------------------------------------------*
  Name:         BgPutString

  Description:  BG 32�����o��

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void BgPutString(s32 x, s32 y, s32 palette, const char *text)
{
    BgPutStringN(x, y, palette, text, BGSTR_MAX_LENGTH);
}

/*---------------------------------------------------------------------------*
  Name:         BgPrintStr

  Description:  BG �t�H�[�}�b�g�t�o��

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void BgPrintStr(s32 x, s32 y, s32 palette, const char *text, ...)
{
    char temp[(BGSTR_MAX_LENGTH + 1) * 2];
    va_list vlist;

    MI_CpuClear8(temp, sizeof(temp));
    va_start(vlist, text);
    (void)vsnprintf(temp, sizeof(temp) - 1, text, vlist);
    va_end(vlist);
    BgPutString(x, y, palette, temp);
}

/*---------------------------------------------------------------------------*
  Name:         BgClear

  Description:  BG�N���A

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void BgClear(void)
{
    MI_CpuClearFast(vscr, sizeof(vscr));
}
