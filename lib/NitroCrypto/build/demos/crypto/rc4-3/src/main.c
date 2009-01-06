/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CRYPTO - demos
  File:     main.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.3  2006/03/09 05:02:00  okubata_ryoma
  add InitTick

  Revision 1.2  2006/03/08 09:04:27  seiki_masashi
  �R�����g�̒ǋL

  Revision 1.1  2006/03/08 08:53:41  seiki_masashi
  rc4-3 �f���̒ǉ�

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  CRYPTO ���C�u���� RC4 �A���S���Y�����g�p�����U���ϐ��̂���Í����̃f��
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitroCrypto/crypto.h>

#include "prng.h"
#include "rc4enc.h"

#define TEST_BUFFER_SIZE 256

static void InitializeAllocateSystem(void);
static void VBlankIntr(void);
static void DisplayInit(void);
static void PRNGInit(void);
static void MicFullCallback(MICResult result, void *arg);
static void FillScreen(u16 col);
static BOOL RC4EncText(void);
static BOOL CompareBinary(void* ptr1, void* ptr2, u32 n);
static u32 GetStringLength(char* str);
static void PrintBinary(u8* ptr, u32 len);

/*---------------------------------------------------------------------------*
    �ϐ���`
 *---------------------------------------------------------------------------*/
static u8 micData[1024] ATTRIBUTE_ALIGN(32);
static MICAutoParam micAutoParam;
static volatile BOOL bufferFullFlag;

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
    InitializeAllocateSystem();

    DisplayInit();

    PRNGInit();

    if (RC4EncText())
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
    // PRNG �̓�����Ԃ����݂̃V�X�e����Ԃŝ��a����B
    // PRNGInit() �ł̎�̏����������ł��\���ł��邪�A�����S�������߂邽�߂�
    // ����I�ȝ��a���s���B
    // ���̌Ăяo�������ł͏\���ȝ��a���s���Ȃ��̂ŁA�K������I�ɌĂяo�����ƁB
    ChurnRandomPool();

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

    SVC_WaitVBlankIntr();              // Waiting the end of VBlank interrupt
    GX_DispOn();

}

/*---------------------------------------------------------------------------*
  Name:         PRNGInit

  Description:  Pseudo Random Number Generator Initialization

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PRNGInit(void)
{
    MICResult result;

    // 1k�o�C�g�̃}�C�N�T���v�����O�f�[�^����͐��S�r�b�g���x�̃G���g���s�[��
    // �擾�\�ł��낤�Ƒz�肵�A������[������������̏������Ɏg�p����

    // �}�C�N�̏������Ǝ����T���v�����O�J�n
    MIC_Init();
    (void)PM_SetAmp(PM_AMP_ON);

    micAutoParam.type = MIC_SAMPLING_TYPE_8BIT;
    micAutoParam.buffer = (void *)micData;
    micAutoParam.size = sizeof(micData);
    micAutoParam.rate = MIC_SAMPLING_RATE_8K;
    micAutoParam.loop_enable = TRUE;
    micAutoParam.full_callback = MicFullCallback;

    bufferFullFlag = FALSE;
    result = MIC_StartAutoSampling(&micAutoParam);
    if (result != MIC_RESULT_SUCCESS)
    {
        bufferFullFlag = TRUE;
        OS_Panic("MIC_StartAutoSampling failed. result = %d\n", result);
    }

    // �}�C�N�̃T���v�����O���o�b�t�@���ꏄ����܂őҋ@
    while (bufferFullFlag == FALSE)
    {
        ;
    }

    // �}�C�N�̃T���v�����O���ʂ�p���ė����̎��������
    AddRandomSeed(micData, sizeof(micData));
}

/*---------------------------------------------------------------------------*
  Name:         MicFullCallback

  Description:  �����T���v�����O�̃o�b�t�@�t���R�[���o�b�N
 *---------------------------------------------------------------------------*/
static void MicFullCallback(MICResult result, void *arg)
{
#pragma unused(result)
#pragma unused(arg)
    bufferFullFlag = TRUE;
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
  Name:         RC4EncText

  Description:  RC4 �A���S���Y���̃e�X�g���[�`��

  Arguments:    None.

  Returns:      �e�X�g������������ true.
 *---------------------------------------------------------------------------*/
#define PrintResultEq( a, b, f ) \
    { OS_TPrintf( ((a) == (b)) ? "[--OK--] " : "[**NG**] " ); (f) = (f) && ((a) == (b)); }
#define PrintResultBinaryEq( a, b, l, f ) \
    { OS_TPrintf( (CompareBinary((a), (b), (l))) ? "[--OK--] " : "[**NG**] " ); (f) = (f) && (CompareBinary((a), (b), (l))); }

static BOOL RC4EncText(void)
{
    int i;
    int testcase = 1;
    BOOL flag = TRUE;

    // RC4Enc �ŃG���R�[�h�ƃf�R�[�h���s���A���ʂ���v���邩���m�F����
    {
        char* d[] = {
            "\x01\x23\x45\x67\x89\xab\xcd\xef\x01\x23\x45\x67\x89\xab\xcd\xef",
            "Copyright 2006 Nintendo.  All rights reserved.",
        };
        char* key[] = {
            "\x01\x23\x45\x67\x89\xab\xcd\xef\x01\x23\x45\x67",
            "Nintendo DS.",
        };

        for (i = 0; i < sizeof(d) / sizeof(char*); i++)
        {
            RC4EncoderContext enc_context;
            RC4DecoderContext dec_context;
            u8 result[TEST_BUFFER_SIZE];
            u8 result2[TEST_BUFFER_SIZE];
            u32 len;
            u32 encoded_len;
            u32 decoded_len;

            InitRC4Encoder(&enc_context, key[i]);
            len = GetStringLength(d[i]);
            encoded_len = EncodeRC4(&enc_context, d[i], len, result, sizeof(result));

            PrintResultEq(encoded_len, len + RC4ENC_ADDITIONAL_SIZE, flag);
            OS_TPrintf("RC4Enc: Test Case %d: encryption phase: length check\n", testcase);

            PrintBinary(result, encoded_len);
            OS_TPrintf("\n");

            InitRC4Decoder(&dec_context, key[i]);
            decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, sizeof(result2));

            PrintResultEq(decoded_len, len, flag);
            OS_TPrintf("RC4Enc: Test Case %d: decryption phase: length check\n", testcase);

            PrintResultBinaryEq(d[i], result2, len, flag);
            OS_TPrintf("RC4Enc: Test Case %d: decryption phase: binary comparing\n", testcase);

            testcase++;
        }
    }

    {
        char* d = "abcdefghijklmnopqrstuvwxyz";
        char* key = "123456789012";
        u8 result[TEST_BUFFER_SIZE];
        u8 result2[TEST_BUFFER_SIZE];
        u8 result_backup[TEST_BUFFER_SIZE];
        u32 len;
        u32 encoded_len;
        u32 decoded_len;
        RC4EncoderContext enc_context;
        RC4DecoderContext dec_context;

        // �ȉ��A�G���R�[�h���̃o�b�t�@�T�C�Y�`�F�b�N�̊m�F

        InitRC4Encoder(&enc_context, key);
        len = GetStringLength(d);

        encoded_len = EncodeRC4(&enc_context, d, len, result, 0);
        PrintResultEq(encoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        encoded_len = EncodeRC4(&enc_context, d, len, result, 1);
        PrintResultEq(encoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        encoded_len = EncodeRC4(&enc_context, d, len, result, len + RC4ENC_ADDITIONAL_SIZE - 1);
        PrintResultEq(encoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        encoded_len = EncodeRC4(&enc_context, d, len, result, len + RC4ENC_ADDITIONAL_SIZE);
        PrintResultEq(encoded_len, len + RC4ENC_ADDITIONAL_SIZE, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        // �ȉ��A�f�R�[�h���̃o�b�t�@�T�C�Y�`�F�b�N�̊m�F

        InitRC4Decoder(&dec_context, key);

        decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, 0);
        PrintResultEq(decoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, 1);
        PrintResultEq(decoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, len - 1);
        PrintResultEq(decoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, len);
        PrintResultEq(decoded_len, len, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        // �ȉ��A�G���R�[�h���ꂽ�f�[�^�̈ꕔ��ύX�����ꍇ�A
        // �f�R�[�h���ɃG���[�ɂȂ邩�̃`�F�b�N

        // �G���R�[�h�f�[�^�̃o�b�N�A�b�v���쐬
        MI_CpuCopy8(result, result_backup, encoded_len);

        result[0] ^= 1;
        decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, len);
        PrintResultEq(decoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        MI_CpuCopy8(result_backup, result, encoded_len); // �o�b�N�A�b�v���畜��

        result[3] ^= 0xff;
        decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, len);
        PrintResultEq(decoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        MI_CpuCopy8(result_backup, result, encoded_len); // �o�b�N�A�b�v���畜��

        result[4] ^= 1;
        decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, len);
        PrintResultEq(decoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        MI_CpuCopy8(result_backup, result, encoded_len); // �o�b�N�A�b�v���畜��

        result[4+len] ^= 0x55;
        decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, len);
        PrintResultEq(decoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);

        MI_CpuCopy8(result_backup, result, encoded_len); // �o�b�N�A�b�v���畜��

        result[encoded_len-1] ^= 0x80;
        decoded_len = DecodeRC4(&dec_context, result, encoded_len, result2, len);
        PrintResultEq(decoded_len, 0, flag);
        OS_TPrintf("RC4Enc: Test Case %d\n", testcase++);
    }





    return flag;
}


/*---------------------------------------------------------------------------*
  Name:         CompareBinary

  Description:  ���������e��r

  Arguments:    ptr1, ptr2 - ��r���郁�����ʒu.
                n - ��r���钷��

  Returns:      ��v������ TRUE.
 *---------------------------------------------------------------------------*/
static BOOL CompareBinary(void* ptr1, void* ptr2, u32 n)
{
    u32 i;
    u8* p1 = (u8*)ptr1;
    u8* p2 = (u8*)ptr2;

    for (i = 0; i < n; i++)
    {
        if (*(p1++) != *(p2++))
        {
            return FALSE;
        }
    }
    return TRUE;
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
static void PrintBinary(u8* ptr, u32 len)
{
#ifndef SDK_FINALROM
    u32 i;
    for (i = 0; i < len; i++)
    {
        if (i != 0)
        {
            OS_PutString(", ");
        }
        OS_TPrintf("0x%02x", ptr[i]);
    }
#else
#pragma unused(ptr,len)
#endif
    return;
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
