/*---------------------------------------------------------------------------*
  Project:  TwlSDK - aes - demos - ccm
  File:     main.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-08#$
  $Rev: 9567 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/
#include <twl.h>
#include <twl/aes.h>


/*---------------------------------------------------------------------------*
    �����ϐ�
 *---------------------------------------------------------------------------*/

typedef struct DEMOSizeData
{
    u32             aSize;
    u32             pSize;
    AESMacLength    macLength;
}
DEMOSizeData;


// AES �����̏I����҂��󂯂邽�߂̃��b�Z�[�W�L���[
#define DEMO_MESSAGE_Q_SIZE     1
static OSMessageQueue   sMsgQ;
static OSMessage        sMsgQBuffer[DEMO_MESSAGE_Q_SIZE];

// �Í�������T���v���̃f�[�^
static const char DEMO_TEXT[] =
    "These coded instructions, statements, and computer programs contain "
    "proprietary information of Nintendo of America Inc. and/or Nintendo "
    "Company Ltd., and are protected by Federal copyright law.  They may "
    "not be disclosed to third parties or copied or duplicated in any form, "
    "in whole or in part, without the prior written consent of Nintendo.";

// �Í����Ɏg�p����K���Ȍ�
static const AESKey DEMO_KEY =
    { 0xb3,0x2f,0x3a,0x91,0xe6,0x98,0xc2,0x76,
      0x70,0x6d,0xfd,0x71,0xbc,0xdd,0xb3,0x93, };



/*---------------------------------------------------------------------------*
    �����֐��錾
 *---------------------------------------------------------------------------*/

static void         DEMO_InitInteruptSystem(void);
static void         DEMO_InitAllocSystem(void);
static void         DEMO_AESCallback(AESResult result, void* arg);
static AESResult    DEMO_WaitAes(OSMessageQueue* pQ);
static void         DEMO_PrintBytes(const void* pvoid, u32 size);



/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         SampleMain

  Description:  �T���v���̖{�̂ł��B

  Arguments:    �Ȃ��B

  Returns:      �T���v���̏����������������ۂ��B
 *---------------------------------------------------------------------------*/
static BOOL SampleMain(void)
{
    const AESMacLength macLength    = AES_MAC_LENGTH_16;
    BOOL bSuccess = FALSE;

    AESResult aesResult;
    AESNonce nonce;
    void* pPlainText;
    void* pEncrypted;
    void* pDecrypted;
    void* pPlainTextA;
    void* pPlainTextP;
    void* pEncryptedA;
    void* pEncryptedC;
    void* pEncryptedM;
    u32 srcASize;
    u32 srcPSize;
    u32 macSize;
    DEMOSizeData* pSizeData;

    // AES �����̑҂��󂯂̂��߂̃��b�Z�[�W�L���[�����������܂��B
    OS_InitMessageQueue(&sMsgQ, sMsgQBuffer, DEMO_MESSAGE_Q_SIZE);

    // �T�C�Y�̌v�Z
    // srcASize �� AES_BLOCK_SIZE �̔{���łȂ���΂Ȃ�܂���B
    srcASize    = MATH_ROUNDUP(sizeof(DEMOSizeData), AES_BLOCK_SIZE);
    srcPSize    = (u32)STD_GetStringLength(DEMO_TEXT) + 1;
    macSize     = AES_GetMacLengthValue(macLength);

    // �������̊m�ۂƃ|�C���^�̌v�Z
    // �Í���/�������Ƃ��� Pdata �� Adata �͈ꑱ����
    // �o�b�t�@�Ɋi�[����Ă���K�v������܂��B
    pPlainText  = OS_Alloc(srcASize + srcPSize);
    pEncrypted  = OS_Alloc(srcASize + srcPSize + macSize);
    pDecrypted  = OS_Alloc(srcPSize);

    if( ! SDK_IS_VALID_POINTER(pPlainText)
     || ! SDK_IS_VALID_POINTER(pEncrypted)
     || ! SDK_IS_VALID_POINTER(pDecrypted) )
    {
        OS_TPrintf("fail to OS_Alloc\n");
        return FALSE;
    }

    pPlainTextA = pPlainText;
    pPlainTextP = (u8*)pPlainTextA + srcASize;
    pEncryptedA = pEncrypted;
    pEncryptedC = (u8*)pEncryptedA + srcASize;
    pEncryptedM = (u8*)pEncryptedC + srcPSize;

    // ���̃T���v���ł� Adata �Ƃ��Ċe�̈�̃T�C�Y���i�[���܂��B
    pSizeData = (DEMOSizeData*)pPlainTextA;
    pSizeData->aSize        = srcASize;
    pSizeData->pSize        = srcPSize;
    pSizeData->macLength    = macLength;
    MI_CpuClear8(pSizeData + 1, srcASize - sizeof(DEMOSizeData));

    // Adata �ƈꑱ���̃o�b�t�@�� Pdata ���R�s�[
    MI_CpuCopy8(DEMO_TEXT, (u8*)pPlainText + srcASize, srcPSize);

    // �������p�ɈÍ����p�� Adata ���R�s�[���܂��B
    // �Í��������ŏo�͂����̂� Pdata ���Í������� Cdata �݂̂ł��B
    // ���������ɐ������������s���ɂ͕ʓr Adata ���K�v�Ȃ��߃R�s�[���Ă��܂��B
    MI_CpuCopy8(pPlainText, pEncrypted, srcASize);


    // AES API �̍ŏ��� AES_Init ���Ăяo���K�v������܂��B
    AES_Init();

    // nonce �𐶐����邽�߂� AES_Rand ���g�p���܂��B
    // nonce �͔C�ӂ̐��l�ł��܂��܂��񂪁A
    // �ł��邾�������l���g��Ȃ��悤�ɂ��Ȃ���΂Ȃ�܂���B
    aesResult = AES_Rand(&nonce, sizeof(nonce));
    if( aesResult != AES_RESULT_SUCCESS )
    {
        OS_TPrintf("AES_Rand failed(%d)\n", aesResult);
        goto error_exit;
    }

    // �Í��̌���ݒ肵�܂��B
    aesResult = AES_SetKey(&DEMO_KEY);
    if( aesResult != AES_RESULT_SUCCESS )
    {
        OS_TPrintf("AES_SetKey failed(%d)\n", aesResult);
        goto error_exit;
    }


    // �p�����[�^�̕\��
    OS_TPrintf("---- parameter -----------\n");
    OS_TPrintf("-- key\n");
    DEMO_PrintBytes(&DEMO_KEY, sizeof(DEMO_KEY));
    OS_TPrintf("-- nonce\n");
    DEMO_PrintBytes(&nonce, sizeof(nonce));


    // �Í���
    {
        // AES CCM ���[�h�ł̈Í������s���܂��B
        aesResult = AES_CcmEncryptAndSign(
                        &nonce,             // nonce
                        pPlainText,         // Adata �� Pdata ���ꑱ���ɔz�u�����o�b�t�@
                        srcASize,           // Adata �̃T�C�Y
                        srcPSize,           // Pdata �̃T�C�Y
                        macLength,          // MAC �̃T�C�Y
                        pEncryptedC,        // �Í��������f�[�^���i�[����o�b�t�@
                        DEMO_AESCallback,   // �����I�����ɌĂяo�����R�[���o�b�N
                        &sMsgQ );           // �R�[���o�b�N�p�����[�^
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("AES_CcmEncryptAndSign failed(%d)\n", aesResult);
            goto error_exit;
        }

        // �Í����������I������̂�҂��܂��B
        aesResult = DEMO_WaitAes(&sMsgQ);
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("AES_CcmEncryptAndSign failed(%d)\n", aesResult);
            goto error_exit;
        }


        // ���ʂ̕\��
        OS_TPrintf("---- encrypt -------------\n");
        OS_TPrintf("-- plain text (ascii)\n");
        OS_PutString(pPlainTextP);   // OS_*Printf �ɂ� 256 �������������邽�� OS_PutString ���g�p
        OS_TPrintf("\n");

        OS_TPrintf("-- plain text (hex)\n");
        DEMO_PrintBytes(pPlainText, srcASize + srcPSize);

        OS_TPrintf("-- encrypted (hex)\n");
        DEMO_PrintBytes(pEncrypted, srcASize + srcPSize + macSize);
    }

    // ������
    {
        // AES CCM ���[�h�ł̕��������s���܂��B
        aesResult = AES_CcmDecryptAndVerify(
                        &nonce,             // nonce�i�Í������Ɠ����łȂ���΂Ȃ�܂���j
                        pEncrypted,         // Adata �� Cdata ���ꑱ���ɔz�u�����o�b�t�@
                        srcASize,           // Adata �̃T�C�Y
                        srcPSize,           // Cdata �̃T�C�Y
                        macLength,          // MAC �̃T�C�Y
                        pDecrypted,         // �����������f�[�^���i�[����o�b�t�@
                        DEMO_AESCallback,   // �����I�����ɌĂяo�����R�[���o�b�N
                        &sMsgQ );           // �R�[���o�b�N�p�����[�^
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("AES_CcmDecryptAndVerify failed(%d)\n", aesResult);
            goto error_exit;
        }

        // �������������I������̂�҂��܂��B
        aesResult = DEMO_WaitAes(&sMsgQ);

        // �Í��������f�[�^����₂���Ă����
        // �����œ����� aesResult �� AES_RESULT_VERIFICATION_FAILED �ɂȂ�܂��B
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("AES_CcmDecryptAndVerify failed(%d)\n", aesResult);
            goto error_exit;
        }


        // ���ʂ̕\��
        OS_TPrintf("---- decrypt -------------\n");
        OS_TPrintf("-- encrypted (hex)\n");
        DEMO_PrintBytes(pEncrypted, srcASize + srcPSize + macSize);

        OS_TPrintf("-- decrypted (hex)\n");
        DEMO_PrintBytes(pDecrypted, srcPSize);

        OS_TPrintf("-- decrypted (ascii)\n");
        OS_PutString(pDecrypted);   // OS_*Printf �ɂ� 256 �������������邽�� OS_PutString ���g�p
        OS_TPrintf("\n");
    }

    // �Í������ĕ������������̂��I���W�i���ƈ�v���邱�Ƃ��m�F
    if( MI_CpuComp8(pDecrypted, pPlainTextP, srcPSize) == 0 )
    {
        OS_TPrintf("** pDecrypted == pPlainTextP\n");
        bSuccess = TRUE;
    }
    else
    {
        OS_TPrintf("** pDecrypted != pPlainTextP\n");
    }

error_exit:
    OS_Free(pDecrypted);
    OS_Free(pEncrypted);
    OS_Free(pPlainText);

    return bSuccess;
}


/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  ���C���֐��ł�

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
void TwlMain(void)
{
    BOOL bSampleSucceeded = FALSE;

    // ������
    OS_Init();
    DEMO_InitInteruptSystem();
    DEMO_InitAllocSystem();
    OS_TPrintf("*** start aes demo\n");


    // �f�������s
    if( OS_IsRunOnTwl() )
    {
        bSampleSucceeded = SampleMain();
    }
    else
    {
        OS_Warning("demo is not run on TWL");
    }


    // �f���̎��s���ʂ�\��
    OS_TPrintf("*** End of demo\n");
    OS_TPrintf("demo %s\n", (bSampleSucceeded ? "succeeded": "failed"));
    GX_Init();
    *(GXRgb*)HW_BG_PLTT = (GXRgb)(bSampleSucceeded ? GX_RGB(0, 31, 0): GX_RGB(31, 0, 0));
    GX_DispOn();
    OS_Terminate();
}


/*---------------------------------------------------------------------------*
  Name:         DEMO_AESCallback

  Description:  AES �����I�����ɌĂяo�����R�[���o�b�N�ł��B
                AESCallback �^�ł��B

  Arguments:    result: AES �����̌��ʁB
                arg:    AES_Ctr �œn�����Ō�̈���

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void DEMO_AESCallback(AESResult result, void* arg)
{
    OSMessageQueue* pQ = (OSMessageQueue*)arg;
    (void)OS_SendMessage(pQ, (OSMessage)result, OS_MESSAGE_BLOCK);
}

/*---------------------------------------------------------------------------*
  Name:         DEMO_WaitAes

  Description:  AES �����̏I����҂��󂯌��ʂ�Ԃ��܂��B
                AES_Ctr �� DEMO_AESCallback ���w�肵�Ă��Ȃ���΂Ȃ�܂���B

  Arguments:    pQ: AES_Ctr �̍Ō�̈����Ƃ��ēn�������b�Z�[�W�L���[

  Returns:      AES �����̌��ʁB
 *---------------------------------------------------------------------------*/
static AESResult DEMO_WaitAes(OSMessageQueue* pQ)
{
    OSMessage msg;
    (void)OS_ReceiveMessage(pQ, &msg, OS_MESSAGE_BLOCK);
    return (AESResult)msg;
}

/*---------------------------------------------------------------------------*
  Name:         DEMO_PrintBytes

  Description:  �w�肳�ꂽ�o�C�i����� 16 �i�Ńf�o�b�O�o�͂ɏo�͂��܂��B

  Arguments:    pvoid:  �Ώۂ̃o�C�i����ւ̃|�C���^�B
                size:   �Ώۂ̃o�C�i����̃o�C�g���B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void DEMO_PrintBytes(const void* pvoid, u32 size)
{
    const u8* p = (const u8*)pvoid;
    u32 i;

    for( i = 0; i < size; ++i )
    {
        OS_TPrintf("%02X ", p[i]);
        if( i % 16 == 15 )
        {
            OS_TPrintf("\n");
        }
    }

    if( i % 16 != 0 )
    {
        OS_TPrintf("\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         DEMO_InitInteruptSystem

  Description:  ���荞�݂����������܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void DEMO_InitInteruptSystem(void)
{
    // �}�X�^�[���荞�݃t���O������
    (void)OS_EnableIrq();
}

/*---------------------------------------------------------------------------*
  Name:         DEMO_InitAllocSystem

  Description:  �q�[�v���쐬���� OS_Alloc ���g����悤�ɂ��܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void DEMO_InitAllocSystem(void)
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
