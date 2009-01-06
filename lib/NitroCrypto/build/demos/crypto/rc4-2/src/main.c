/*---------------------------------------------------------------------------*
  Project:  NitroCrypto - build - demos
  File:     main.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.3  2006/02/24 01:18:58  okubata_ryoma
  NitroCrypto �̐V�݁@Crypt ���� Crypto ��

  Revision 1.1  2005/12/27 12:40:20  seiki_masashi
  RC4 �֐��̃f���̒ǉ�

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  CRYPTO ���C�u���� RC4 �A���S���Y���̓���m�F�f��
  ���x�v�����s��
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitroCrypto/crypto.h>



static void VBlankIntr(void);
static void DisplayInit(void);
static void CompareRC4Speed(void);

/*---------------------------------------------------------------------------*
    �ϐ���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    // �e�평����
    OS_Init();
    OS_InitTick();

    DisplayInit();

    CompareRC4Speed();

    // ���C�����[�v
    while (TRUE)
    {
        // �u�u�����N�҂�
        SVC_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃x�N�g���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    // IRQ �`�F�b�N�t���O���Z�b�g
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         DisplayInit

  Description:  Graphics Initialization

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DisplayInit(void)
{

    GX_Init();
    FX_Init();

    GX_DispOff();
    GXS_DispOff();

    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);         // to generate VBlank interrupt request
    (void)OS_EnableIrq();


    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);

    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette
    MI_DmaFill32(3, (void *)HW_LCDC_VRAM_C, 0x7FFF7FFF, 256 * 192 * sizeof(u16));


    GX_SetBankForOBJ(GX_VRAM_OBJ_256_AB);       // Set VRAM-A,B for OBJ

    GX_SetGraphicsMode(GX_DISPMODE_VRAM_C,      // VRAM mode.
                       (GXBGMode)0,    // dummy
                       (GXBG0As)0);    // dummy

    GX_SetVisiblePlane(GX_PLANEMASK_OBJ);       // make OBJ visible
    GX_SetOBJVRamModeBmp(GX_OBJVRAMMODE_BMP_1D_128K);   // 2D mapping OBJ

    SVC_WaitVBlankIntr();              // Waiting the end of VBlank interrupt
    GX_DispOn();

}


/*---------------------------------------------------------------------------*
  Name:         CompareRC4Speed

  Description:  RC4 �֐��̃e�X�g���[�`��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#define DATA_SIZE (1024*1024)
static void CompareRC4Speed(void)
{
    static u8 data[DATA_SIZE] ATTRIBUTE_ALIGN(32);
    static u8 orig[DATA_SIZE] ATTRIBUTE_ALIGN(32);

    u8 key[] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    };

    int i;
    MATHRandContext32 rand;
    OSTick start, end;

    OS_TPrintf("Initialize Array...\n");
    // data �̏�����
    MATH_InitRand32(&rand, 0);
    for (i = 0; i < DATA_SIZE; i++)
    {
        data[i] = orig[i] = (u8)MATH_Rand32(&rand, 0x100);
    }

    OS_TPrintf("Start...\n");

    OS_TPrintf("RC4 Encoding:\t\t");
    start = OS_GetTick();
    CRYPTO_RC4(key, sizeof(key), data, sizeof(data));
    end   = OS_GetTick();
    OS_Printf("%lld us/MB\n", OS_TicksToMicroSeconds(end - start));

    OS_TPrintf("RC4 Decoding:\t\t");
    start = OS_GetTick();
    CRYPTO_RC4(key, sizeof(key), data, sizeof(data));
    end   = OS_GetTick();
    OS_Printf("%lld us/MB\n", OS_TicksToMicroSeconds(end - start));

    for (i = 0; i < DATA_SIZE; i++)
    {
        if ( data[i] != orig[i] )
        {
            OS_Panic("Decoding Failed!");
        }
    }

    OS_TPrintf("RC4Fast Encoding:\t");
    start = OS_GetTick();
    CRYPTO_RC4Fast(key, sizeof(key), data, sizeof(data));
    end   = OS_GetTick();
    OS_Printf("%lld us/MB\n", OS_TicksToMicroSeconds(end - start));

    OS_TPrintf("RC4Fast Decoding:\t");
    start = OS_GetTick();
    CRYPTO_RC4Fast(key, sizeof(key), data, sizeof(data));
    end   = OS_GetTick();
    OS_Printf("%lld us/MB\n", OS_TicksToMicroSeconds(end - start));

    for (i = 0; i < DATA_SIZE; i++)
    {
        if ( data[i] != orig[i] )
        {
            OS_Panic("Decoding Failed!");
        }
    }

    OS_TPrintf("Done.\n");

    return;
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
