/*---------------------------------------------------------------------------*
  Project:  TwlSDK - nandApp - demos - sharedFont
  File:     main.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include <twl.h>
#include <twl/na.h>
#include <DEMO.h>

static void PrintBootType();

static void InitDEMOSystem();
static void InitInteruptSystem();
static void InitAllocSystem();
static void InitFileSystem();

// OS_Printf�̃t�b�N�֐�
#ifndef SDK_FINALROM
void OS_Printf(const char *fmt, ...)
{
    char dst[256];
    
    int     ret;
    va_list va;
    va_start(va, fmt);
    ret = OS_VSPrintf(dst, fmt, va);
    va_end(va);

    OS_PutString(dst);
}
#endif

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  ���C���֐��ł��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
void TwlMain(void)
{
    OS_Init();
    InitInteruptSystem();
    InitFileSystem();
    InitAllocSystem();
    InitDEMOSystem();
    OS_Printf("*** start sharedFont demo\n");

    PrintBootType();

    // �{�̓����t�H���g��ǂݍ��݂܂��B
    {
        BOOL bSuccess;
        int sizeTable;
        int sizeFont;
        void* pBufferTable;
        void* pBufferFont;

        // �{�̓����t�H���g API �����������܂��B
        bSuccess = NA_InitSharedFont();
        SDK_ASSERT( bSuccess );

        // �Ǘ����o�b�t�@�T�C�Y���擾
        sizeTable = NA_GetSharedFontTableSize();
        SDK_ASSERT( sizeTable >= 0 );
        OS_TPrintf("shared font table size = %d bytes\n", sizeTable);

        pBufferTable = OS_Alloc((u32)sizeTable);
        SDK_POINTER_ASSERT(pBufferTable);

        // �Ǘ��������[�h
        bSuccess = NA_LoadSharedFontTable(pBufferTable);
        SDK_ASSERT( bSuccess );

        // �t�H���g�T�C�Y���擾
        sizeFont = NA_GetSharedFontSize(NA_SHARED_FONT_WW_M);
        SDK_ASSERT( sizeFont >= 0 );
        OS_TPrintf("shared font(M) size = %d bytes\n", sizeFont);

        pBufferFont = OS_Alloc((u32)sizeFont);
        SDK_POINTER_ASSERT(pBufferFont);

        // �t�H���g�����[�h
        bSuccess = NA_LoadSharedFont(NA_SHARED_FONT_WW_M, pBufferFont);
        SDK_ASSERT( bSuccess );

        OS_TPrintf("shared font loaded\n");

        /*
        ���[�h�����t�H���g���g�p����ɂ� TWL-System ���K�v�ł�...
        {
            NNSG2dFont font;

            NNS_G2dFontInitUTF16(&font, pBufferFont);
            ...
        }
        */
    }

    OS_Printf("*** End of demo\n");
    DEMO_DrawFlip();

    OS_Terminate();
}


/*---------------------------------------------------------------------------*
  Name:         PrintBootType

  Description:  BootType �� print ���܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void PrintBootType()
{
    const OSBootType btype = OS_GetBootType();

    switch( btype )
    {
    case OS_BOOTTYPE_ROM:   OS_TPrintf("OS_GetBootType = OS_BOOTTYPE_ROM\n"); break;
    case OS_BOOTTYPE_NAND:  OS_TPrintf("OS_GetBootType = OS_BOOTTYPE_NAND\n"); break;
    default:
        {
            OS_Warning("unknown BootType(=%d)", btype);
        }
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         InitDEMOSystem

  Description:  �R���\�[���̉�ʏo�͗p�̕\���ݒ���s���܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void InitDEMOSystem()
{
    // ��ʕ\���̏������B
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayBitmap();
    DEMOHookConsole();
    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 0, 1));
    DEMOSetBitmapGroundColor(DEMO_RGB_CLEAR);
    DEMOStartDisplay();
}

/*---------------------------------------------------------------------------*
  Name:         InitInteruptSystem

  Description:  ���荞�݂����������܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void InitInteruptSystem()
{
    // �}�X�^�[���荞�݃t���O������
    (void)OS_EnableIrq();

    // IRQ ���荞�݂������܂�
    (void)OS_EnableInterrupts();
}

/*---------------------------------------------------------------------------*
  Name:         InitAllocSystem

  Description:  �q�[�v���쐬���� OS_Alloc ���g����悤�ɂ��܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void InitAllocSystem()
{
    void* newArenaLo;
    OSHeapHandle hHeap;

    // ���C���A���[�i�̃A���P�[�g�V�X�e����������
    newArenaLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetMainArenaLo(newArenaLo);

    // ���C���A���[�i��Ƀq�[�v���쐬
    hHeap = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, hHeap);
}

/*---------------------------------------------------------------------------*
  Name:         InitFileSystem

  Description:  FS ������������ rom �ɃA�N�Z�X�ł���悤�ɂ��܂��B
                ���̊֐����Ăяo���O�� InitInteruptSystem() ��
                �Ă΂�Ă���K�v������܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void InitFileSystem()
{
    // �t�@�C���V�X�e��������
    FS_Init( FS_DMA_NOT_USE );
}
