/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CRYPTO - demos
  File:     main.c

  Copyright 2008-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::$
  $Rev:$
  $Author:$
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  CRYPTO ���C�u���� RSA�Í��֐��Q�̓���m�F�f��
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/crypto.h>
#include <nitro/std.h>
#include <string.h>

//#define PRINT_ALLOCATION
#define PUBKEY_USE_MODULOUS_ONLY   0

static void InitializeAllocateSystem(void);
static void VBlankIntr(void);
static void DisplayInit(void);
static void FillScreen(u16 col);
static BOOL RsaTest(void);
static u32 GetStringLength(char* str);
static void PrintBinary(u8* ptr, int len);

/*---------------------------------------------------------------------------*
    �ϐ���`
 *---------------------------------------------------------------------------*/

#ifdef PRINT_ALLOCATION
static u32 total_allocated_size = 0;
#endif

#if (PUBKEY_USE_MODULOUS_ONLY == 0)
static const unsigned char rsa512_pub[]={
    0x30, 0x5C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05,
    0x00, 0x03, 0x4B, 0x00, 0x30, 0x48, 0x02, 0x41, 0x00, 0xAA, 0x3D, 0x39, 0x23, 0x81, 0x1E, 0x04,
    0xDB, 0x25, 0x91, 0xE4, 0x15, 0xA5, 0x3A, 0x9A, 0xCF, 0x64, 0x7F, 0x2F, 0xD6, 0x03, 0xB1, 0x6C,
    0xA4, 0x1E, 0x6A, 0x4D, 0x7C, 0x9A, 0x02, 0x7D, 0x9A, 0x8E, 0xF9, 0x8F, 0x61, 0x9C, 0x0E, 0x42,
    0x49, 0x58, 0xBB, 0x96, 0xB1, 0x1F, 0x7C, 0xCA, 0x7B, 0x2A, 0x75, 0x3A, 0x6C, 0x82, 0x4F, 0xF9,
    0xF5, 0xB2, 0x11, 0xB7, 0xC4, 0xCC, 0xF9, 0x6D, 0xE7, 0x02, 0x03, 0x01, 0x00, 0x01
};
static const int rsa512_pub_len = 0x5E;

static const unsigned char rsa512_sec[]={
    0x30, 0x82, 0x01, 0x3A, 0x02, 0x01, 0x00, 0x02, 0x41, 0x00, 0xAA, 0x3D, 0x39, 0x23, 0x81, 0x1E,
    0x04, 0xDB, 0x25, 0x91, 0xE4, 0x15, 0xA5, 0x3A, 0x9A, 0xCF, 0x64, 0x7F, 0x2F, 0xD6, 0x03, 0xB1,
    0x6C, 0xA4, 0x1E, 0x6A, 0x4D, 0x7C, 0x9A, 0x02, 0x7D, 0x9A, 0x8E, 0xF9, 0x8F, 0x61, 0x9C, 0x0E,
    0x42, 0x49, 0x58, 0xBB, 0x96, 0xB1, 0x1F, 0x7C, 0xCA, 0x7B, 0x2A, 0x75, 0x3A, 0x6C, 0x82, 0x4F,
    0xF9, 0xF5, 0xB2, 0x11, 0xB7, 0xC4, 0xCC, 0xF9, 0x6D, 0xE7, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02,
    0x40, 0x75, 0x8E, 0xA1, 0x8E, 0x6A, 0xF3, 0x17, 0xD7, 0x5D, 0x32, 0x49, 0x96, 0xC8, 0xEA, 0x97,
    0x4C, 0xEF, 0xD1, 0x04, 0x6F, 0x7D, 0x4D, 0x60, 0x00, 0x55, 0x20, 0x83, 0x31, 0xB0, 0x08, 0x58,
    0x42, 0xEA, 0x75, 0x28, 0xB7, 0xB7, 0x44, 0xA7, 0x03, 0xF8, 0xEC, 0x80, 0x67, 0xD2, 0xF5, 0x44,
    0x14, 0x77, 0x7D, 0x3F, 0xE7, 0x12, 0x2C, 0xAB, 0xE8, 0xBA, 0x52, 0x0E, 0xD5, 0x1F, 0xE5, 0xC1,
    0x81, 0x02, 0x21, 0x00, 0xD1, 0xE4, 0x7B, 0xDE, 0x44, 0xE2, 0x4E, 0x03, 0xE5, 0x74, 0xCD, 0x89,
    0xFD, 0x15, 0x8A, 0xD8, 0xAE, 0xF4, 0x67, 0x87, 0xCF, 0x65, 0x52, 0x3C, 0x23, 0x13, 0x2B, 0xE9,
    0x97, 0x33, 0x26, 0x69, 0x02, 0x21, 0x00, 0xCF, 0xA2, 0xCA, 0xF1, 0x9F, 0xB0, 0x49, 0x1E, 0x4E,
    0x64, 0x5F, 0xC7, 0x21, 0xE0, 0x71, 0x7E, 0x93, 0x7A, 0x5B, 0x20, 0xB1, 0x89, 0xAF, 0xF9, 0xC8,
    0x96, 0x23, 0xBB, 0x72, 0xFC, 0x87, 0xCF, 0x02, 0x21, 0x00, 0xA8, 0xF7, 0x3C, 0x58, 0x44, 0x2F,
    0xC2, 0x0A, 0x14, 0xEF, 0xA0, 0x7F, 0x13, 0x04, 0x02, 0x90, 0x48, 0xD7, 0x6B, 0x78, 0xC3, 0x16,
    0x97, 0xCA, 0xDD, 0x99, 0x93, 0x62, 0x2A, 0x5B, 0xFC, 0xF1, 0x02, 0x20, 0x11, 0x44, 0x4E, 0x70,
    0x2D, 0x81, 0x71, 0x73, 0x2D, 0xBD, 0xB7, 0x21, 0x4E, 0x35, 0xE5, 0xFA, 0x4A, 0xB5, 0x60, 0x22,
    0xA5, 0xE0, 0xF7, 0x5B, 0x64, 0x4C, 0xE8, 0x07, 0xCC, 0x96, 0x27, 0x8D, 0x02, 0x20, 0x42, 0xEA,
    0x98, 0x73, 0x96, 0x3E, 0xAC, 0x03, 0x3B, 0x19, 0xD0, 0xB7, 0x29, 0x11, 0x94, 0x25, 0x0E, 0x98,
    0xC4, 0x7C, 0xA5, 0x95, 0x64, 0x02, 0x34, 0xB8, 0x4B, 0x66, 0x1E, 0x7B, 0x99, 0xB4
};
static const int rsa512_sec_len = 0x13E;
#else
//modulous only
static const unsigned char rsa512_pub[]={
    0x30, 0x48, 0x02, 0x41, 0x00, 0xAE, 0x53, 0x85, 0xED, 0x1F, 0xF0, 0x0E, 0x54, 0xF4, 0x4F, 0x69,
    0xE1, 0x96, 0xFE, 0xCE, 0x15, 0xA9, 0x00, 0x7B, 0xF7, 0xDA, 0x86, 0xA5, 0xF8, 0xF0, 0x00, 0x8F,
    0xF6, 0xE6, 0xE3, 0xD4, 0x39, 0xEE, 0x6D, 0xED, 0xB8, 0xE9, 0xC7, 0x1B, 0xC3, 0xBD, 0x01, 0x6D,
    0x57, 0xD7, 0x85, 0xE0, 0x80, 0x7A, 0x1B, 0xE3, 0x45, 0x47, 0xCC, 0x17, 0x8C, 0xCC, 0x69, 0x9B,
    0x7D, 0xB3, 0x94, 0xD5, 0x25, 0x02, 0x03, 0x01, 0x00, 0x01
};
static const int rsa512_pub_len = 0x4A;

static const unsigned char rsa512_sec[]={
    0x30, 0x82, 0x01, 0x3B, 0x02, 0x01, 0x00, 0x02, 0x41, 0x00, 0xAE, 0x53, 0x85, 0xED, 0x1F, 0xF0, 
	0x0E, 0x54, 0xF4, 0x4F, 0x69, 0xE1, 0x96, 0xFE, 0xCE, 0x15, 0xA9, 0x00, 0x7B, 0xF7, 0xDA, 0x86, 
	0xA5, 0xF8, 0xF0, 0x00, 0x8F, 0xF6, 0xE6, 0xE3, 0xD4, 0x39, 0xEE, 0x6D, 0xED, 0xB8, 0xE9, 0xC7, 
	0x1B, 0xC3, 0xBD, 0x01, 0x6D, 0x57, 0xD7, 0x85, 0xE0, 0x80, 0x7A, 0x1B, 0xE3, 0x45, 0x47, 0xCC, 
	0x17, 0x8C, 0xCC, 0x69, 0x9B, 0x7D, 0xB3, 0x94, 0xD5, 0x25, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 
	0x41, 0x00, 0x88, 0x07, 0xDD, 0x06, 0x00, 0x5A, 0x0E, 0x94, 0xF4, 0x5F, 0xD3, 0xBB, 0x99, 0x5C, 
	0x13, 0xEC, 0x8B, 0x08, 0x39, 0xB4, 0x5A, 0xED, 0x9F, 0x2D, 0xF5, 0xFA, 0x3A, 0x5C, 0xEC, 0x26, 
	0xF4, 0x3F, 0x46, 0xB6, 0x6C, 0x03, 0x0B, 0x85, 0x99, 0x95, 0x81, 0x2C, 0xEC, 0xE2, 0xA1, 0x92, 
	0xED, 0xBA, 0xAB, 0x16, 0xDC, 0xE0, 0xFD, 0xF7, 0xA6, 0x93, 0x4D, 0xA8, 0xEA, 0x67, 0xD4, 0x42, 
	0x1B, 0xB1, 0x02, 0x21, 0x00, 0xD6, 0x98, 0xF1, 0x7C, 0x33, 0xC9, 0x13, 0x2C, 0xB3, 0xC5, 0xB0, 
	0x37, 0x7B, 0xF9, 0x4A, 0xB4, 0x60, 0xFE, 0x0E, 0xB9, 0x5E, 0x96, 0xFC, 0x94, 0xD2, 0x6C, 0x7F, 
	0xE1, 0x14, 0xA6, 0xF7, 0xB3, 0x02, 0x21, 0x00, 0xCF, 0xF5, 0x91, 0xE2, 0x9B, 0x8E, 0xE0, 0xB5, 
	0x2D, 0x76, 0xB2, 0x93, 0x85, 0x30, 0xAE, 0x51, 0x69, 0x05, 0xC3, 0x20, 0x78, 0x88, 0x3D, 0x74, 
	0x4D, 0x10, 0xA1, 0x62, 0x84, 0xF8, 0x13, 0xC7, 0x02, 0x21, 0x00, 0xD0, 0x49, 0x9E, 0x89, 0xC8, 
	0xD1, 0xAF, 0x94, 0xC1, 0xE0, 0x6D, 0x0B, 0x4D, 0x7F, 0x47, 0xAD, 0x10, 0x3F, 0x47, 0x17, 0x50, 
	0xE2, 0x07, 0xC2, 0x83, 0x02, 0xE0, 0x94, 0x1C, 0xB1, 0x7A, 0xD5, 0x02, 0x20, 0x72, 0x95, 0xFE, 
	0x27, 0x5D, 0xCD, 0x71, 0x55, 0x83, 0x52, 0x86, 0xFD, 0x30, 0x03, 0x6D, 0xD9, 0x9F, 0xC3, 0xB4, 
	0x0B, 0x9D, 0x3E, 0x7C, 0x96, 0x2B, 0xAC, 0x81, 0x30, 0x36, 0xD2, 0x36, 0x9B, 0x02, 0x20, 0x67, 
	0xC7, 0xBC, 0x06, 0x91, 0x3C, 0xF7, 0xEF, 0x7F, 0x32, 0x58, 0xD0, 0xC0, 0xA8, 0x46, 0x72, 0x8F, 
	0xFB, 0x49, 0xC7, 0x29, 0x4E, 0xD6, 0x28, 0xB9, 0xF6, 0x26, 0x3D, 0xBD, 0xA1, 0xB7, 0xEA
};
static const int rsa512_sec_len = 0x13F;
#endif


/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         MyAlloc

  Description:  OS_Alloc �̃��b�p�[�֐�
 *---------------------------------------------------------------------------*/
static void* MyAlloc(u32 size)
{
    void* ptr;
#ifdef PRINT_ALLOCATION
    ptr = OS_Alloc(size+4);
    *(u32*)ptr = size;
    (u8*)ptr += 4;
    total_allocated_size += size;
    OS_TPrintf("Allocate %d bytes from %p: total %d bytes\n", size, ptr, total_allocated_size);
#else
    ptr = OS_Alloc(size);
#endif
    return ptr;
}

/*---------------------------------------------------------------------------*
  Name:         MyFree

  Description:  OS_Free �̃��b�p�[�֐�
 *---------------------------------------------------------------------------*/
static void MyFree(void* ptr)
{
#ifdef PRINT_ALLOCATION
    u32 size;
    (u8*)ptr -= 4;
    size = *(u32*)ptr;
    total_allocated_size -= size;
    OS_TPrintf("Free %d bytes from %p: total %d bytes\n", size, ptr, total_allocated_size);
#endif
    OS_Free(ptr);
}

/*---------------------------------------------------------------------------*
  Name:         MyRealloc

  Description:  realloc�֐�
 *---------------------------------------------------------------------------*/
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

static void* MyRealloc(void* ptr, u32 new_size, u32 old_size)
{
    void *buf;
#ifdef PRINT_ALLOCATION
    u32 size;

	buf = OS_Alloc(new_size+4);
    *(u32*)buf = new_size;
    (u8*)buf += 4;
    total_allocated_size += new_size;
    OS_TPrintf("Reallocate %d bytes from %p: total %d bytes\n", new_size, ptr, total_allocated_size);

    (u8*)ptr -= 4;
    size = *(u32*)ptr;
    total_allocated_size -= size;
    OS_TPrintf("Free(for realloc) %d bytes from %p: total %d bytes\n", size, ptr, total_allocated_size);
	SDK_ASSERT(size == old_size);
    OS_Free(ptr);
#else
    if ((buf = OS_Alloc(new_size)) == NULL)
    {
        return NULL;
    }

    MI_CpuCopy8(ptr, buf, MIN(new_size, old_size));
    OS_Free(ptr);
#endif

    return buf;
}

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
    InitializeAllocateSystem();

    DisplayInit();

    if (RsaTest())
    {
        // ����
        OS_TPrintf("------ Test Succeeded ------\n");
        FillScreen(GX_RGB(0, 31, 0));
    }
    else
    {
        // ���s
        OS_TPrintf("****** Test Failed ******\n");
        FillScreen(GX_RGB(31, 0, 0));
    }

    // ���C�����[�v
    while (TRUE)
    {
        // �u�u�����N�҂�
        OS_WaitVBlankIntr();
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
  Name:         InitializeAllocateSystem

  Description:  ���C����������̃A���[�i�ɂă����������ăV�X�e��������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void)
{
    void *tempLo;
    OSHeapHandle hh;

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

    OS_WaitVBlankIntr();              // Waiting the end of VBlank interrupt
    GX_DispOn();

}


/*---------------------------------------------------------------------------*
  Name:         FillScreen

  Description:  ��ʂ�h��Ԃ�

  Arguments:    col - FillColor.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FillScreen(u16 col)
{
    MI_CpuFill16((void *)HW_LCDC_VRAM_C, col, 256 * 192 * 2);
}

/*---------------------------------------------------------------------------*
  Name:         RsaTest

  Description:  �d�q�����֐��̃e�X�g���[�`��

  Arguments:    None.

  Returns:      �e�X�g������������ true.
 *---------------------------------------------------------------------------*/
#define PrintResultEq( a, b, f ) \
    { OS_TPrintf( ((a) == (b)) ? "[--OK--] " : "[**NG**] " ); (f) = (f) && ((a) == (b)); }

#define CheckAndPrintErr( result, str ) \
    { if(OS_IsRunOnTwl() == TRUE){      \
          if(result < 0) { OS_TPrintf( "Error: %d (%s)\n", result, str ); return FALSE;}  \
      }else{                            \
          if(result >= 0){ OS_TPrintf( "Error: %d (%s)\n", result, str ); return FALSE;}  \
      }  \
    }

static BOOL RsaTest(void)
{
    BOOL flag = TRUE;

    // �d�q�����̓���e�X�g���s��
    {
        char teststr[] = "Nintendo";
		char encstr[128], decstr[128];
        s32                          result;
		s32                          len;
        CRYPTORSAContext             context;
        CRYPTORSAEncryptInitParam    encinitparam;
        CRYPTORSAEncryptParam        encparam;
        CRYPTORSADecryptInitParam    decinitparam;
        CRYPTORSADecryptParam        decparam;

        CRYPTO_SetMemAllocator(MyAlloc, MyFree, MyRealloc);

        /* RSA encode */
        encinitparam.key     = (void*)rsa512_pub;
        encinitparam.key_len = rsa512_pub_len;
        result = CRYPTO_RSA_EncryptInit(&context, &encinitparam);
        CheckAndPrintErr(result, "CRYPTO_RSA_EncryptInit");

        encparam.in      = teststr;
        encparam.in_len  = strlen((char*)teststr);
        encparam.out     = encstr;
        encparam.out_size = sizeof(encstr);
        len = CRYPTO_RSA_Encrypt(&context, &encparam);
        CheckAndPrintErr(len, "CRYPTO_RSA_Encrypt");

        result = CRYPTO_RSA_EncryptTerminate(&context);
        CheckAndPrintErr(result, "CRYPTO_RSA_EncryptTerminate");

        /* RSA decode */
        decinitparam.key     = (void*)rsa512_sec;
        decinitparam.key_len = rsa512_sec_len;
        result = CRYPTO_RSA_DecryptInit(&context, &decinitparam);
        CheckAndPrintErr(result, "CRYPTO_RSA_DecryptInit");

        decparam.in      = encstr;
        decparam.in_len  = (u32)len;
        decparam.out     = decstr;
        decparam.out_size = sizeof(decstr);
        result = CRYPTO_RSA_Decrypt(&context, &decparam);
        CheckAndPrintErr(result, "CRYPTO_RSA_Decrypt");

        result = CRYPTO_RSA_DecryptTerminate(&context);
        CheckAndPrintErr(result, "CRYPTO_RSA_DecryptTerminate");

		if(OS_IsRunOnTwl() == TRUE){
			if( STD_CompareString(teststr, decstr) ){
				OS_TPrintf( "Error: decrypted string is not match .\n" );
				return FALSE;
			}
		}
	}


    return flag;
}


/*---------------------------------------------------------------------------*
  Name:         GetStringLength

  Description:  �����񒷂𓾂�

  Arguments:    str - ������ւ̃|�C���^.

  Returns:      ������.
 *---------------------------------------------------------------------------*/
static u32 GetStringLength(char* str)
{
    u32 i;
    for (i = 0; ; i++)
    {
        if (*(str++) == '\0')
        {
            return i;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         PrintBinary

  Description:  �o�C�i����o��

  Arguments:    ptr - �o�͂��郁�����ʒu.
                len - �o�͂��钷��

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintBinary(u8* ptr, int len)
{
#ifndef SDK_FINALROM
    int i;
    for (i = 0; i < len; i++)
    {
        if (i != 0)
        {
            OS_PutString(":");
        }
        OS_TPrintf("%02x", ptr[i]);
    }
#else
#pragma unused(ptr,len)
#endif
    return;
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
