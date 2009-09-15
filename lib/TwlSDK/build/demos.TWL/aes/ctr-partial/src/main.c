/*---------------------------------------------------------------------------*
  Project:  TwlSDK - aes - demos - ctr-partial
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
    �����ϐ��錾
 *---------------------------------------------------------------------------*/

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
static AESResult    DEMO_CtrDecryptPartial(
                        const AESCounter*   pCounter,
                        const void*         pEncrypted,
                        u32                 decryptOffset,
                        u32                 decryptSize,
                        void*               dst,
                        AESCallback         callback,
                        void*               arg );



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
    BOOL bSuccess = FALSE;

    AESResult aesResult;
    AESCounter counter;
    const char* pPlainText = DEMO_TEXT;
    void* pEncrypted;
    void* pDecrypted;
    u32 srcSize;

    // AES �����̑҂��󂯂̂��߂̃��b�Z�[�W�L���[�����������܂��B
    OS_InitMessageQueue(&sMsgQ, sMsgQBuffer, DEMO_MESSAGE_Q_SIZE);

    // �Í���/�����������f�[�^���i�[���邽�߂̃o�b�t�@���m�ۂ��܂��B
    // �Í���/�������Ńf�[�^�̃T�C�Y�͕ω����܂���B
    srcSize = (u32)STD_GetStringLength(pPlainText) + 1;
    pEncrypted = OS_Alloc(srcSize);
    pDecrypted = OS_Alloc(srcSize);

    if( ! SDK_IS_VALID_POINTER(pEncrypted)
     || ! SDK_IS_VALID_POINTER(pDecrypted) )
    {
        OS_TPrintf("fail to OS_Alloc\n");
        return FALSE;
    }

    // AES API �̍ŏ��� AES_Init ���Ăяo���K�v������܂��B
    AES_Init();

    // �J�E���^�����l�𐶐����邽�߂� AES_Rand ���g�p���܂��B
    // �J�E���^�����l�͔C�ӂ̐��l�ł��܂��܂��񂪁A
    // �ł��邾�������l���g��Ȃ��悤�ɂ��Ȃ���΂Ȃ�܂���B
    aesResult = AES_Rand(&counter, sizeof(counter));
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
    OS_TPrintf("-- counter initial value\n");
    DEMO_PrintBytes(&counter, sizeof(counter));


    // �Í���
    {
        // AES CTR ���[�h�ł̈Í������s���܂��B
        aesResult = AES_CtrEncrypt( &counter,           // �J�E���^�����l
                                    pPlainText,         // �Í�������f�[�^
                                    srcSize,            // �Í�������f�[�^�̃T�C�Y
                                    pEncrypted,         // �Í��������f�[�^���i�[����o�b�t�@
                                    DEMO_AESCallback,   // �����I�����ɌĂяo�����R�[���o�b�N
                                    &sMsgQ );           // �R�[���o�b�N�p�����[�^
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("AES_CtrEncrypt failed(%d)\n", aesResult);
            goto error_exit;
        }

        // �Í����������I������̂�҂��܂��B
        aesResult = DEMO_WaitAes(&sMsgQ);
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("AES_CtrEncrypt failed(%d)\n", aesResult);
            goto error_exit;
        }


        // ���ʂ̕\��
        OS_TPrintf("---- encrypt -------------\n");
        OS_TPrintf("-- plain text (ascii)\n");
        OS_PutString(pPlainText);   // OS_*Printf �ɂ� 256 �������������邽�� OS_PutString ���g�p
        OS_TPrintf("\n");

        OS_TPrintf("-- plain text (hex)\n");
        DEMO_PrintBytes(pPlainText, srcSize);

        OS_TPrintf("-- encrypted (hex)\n");
        DEMO_PrintBytes(pEncrypted, srcSize);
    }

    // ������
    {
        // AES CTR ���[�h�ł̕������𕔕��I�ɍs���܂��B
        const u32 decryptOffset = 80;       // AES_BLOCK_SIZE �̔{���łȂ���΂Ȃ�܂���
        const u32 decryptSize   = 68;

        aesResult = DEMO_CtrDecryptPartial(
                        &counter,           // �J�E���^�����l�i�Í������Ɠ����łȂ���΂Ȃ�܂���j
                        pEncrypted,         // ����������f�[�^
                        decryptOffset,      // ���������J�n����I�t�Z�b�g�iAES_BLOCK_SIZE �̔{���łȂ���΂Ȃ�܂���j
                        decryptSize,        // ����������g�T�C�Y
                        pDecrypted,         // �����������f�[�^���i�[����o�b�t�@
                        DEMO_AESCallback,   // �����I�����ɌĂяo�����R�[���o�b�N
                        &sMsgQ );           // �R�[���o�b�N�p�����[�^
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("AES_CtrDecrypt failed(%d)\n", aesResult);
            goto error_exit;
        }

        // �������������I������̂�҂��܂��B
        aesResult = DEMO_WaitAes(&sMsgQ);
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("AES_CtrDecrypt failed(%d)\n", aesResult);
            goto error_exit;
        }

        // �e�L�X�g�Ƃ��ĕ\���������̂� NUL �I�[����
        ((char*)pDecrypted)[decryptSize] = '\0';

        // ���ʂ̕\��
        OS_TPrintf("---- decrypt -------------\n");
        OS_TPrintf("-- encrypted (hex)\n");
        DEMO_PrintBytes(pEncrypted, srcSize);

        OS_TPrintf("-- decrypted (hex)\n");
        DEMO_PrintBytes(pDecrypted, decryptSize);

        OS_TPrintf("-- decrypted (ascii)\n");
        OS_PutString(pDecrypted);   // OS_*Printf �ɂ� 256 �������������邽�� OS_PutString ���g�p
        OS_TPrintf("\n");

        // �Í������ĕ������������̂��I���W�i���ƈ�v���邱�Ƃ��m�F
        if( MI_CpuComp8(pDecrypted, pPlainText + decryptOffset, decryptSize) == 0 )
        {
            OS_TPrintf("** pDecrypted == pPlainText\n");
            bSuccess = TRUE;
        }
        else
        {
            OS_TPrintf("** pDecrypted != pPlainText\n");
        }
    }

error_exit:
    OS_Free(pDecrypted);
    OS_Free(pEncrypted);

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
  Name:         DEMO_CtrDecryptPartial

  Description:  CTR ���[�h�ł̈Í����𕔕��I�ɕ��������܂��B
                pEncrypted �� decryptOffset �o�C�g�ڂ��� decryptSize �o�C�g��
                ���������܂��B

  Arguments:    pCounter:       �Í������Ɏg�p�����J�E���^�����l���w�肵�܂��B
                pEncrypted:     �Í����̐擪�ւ̃|�C���^���w�肵�܂��B
                                �|�C���^�� 4 byte �A���C�����g�łȂ���΂Ȃ�܂���B
                decryptOffset:  ���������J�n����I�t�Z�b�g���w�肵�܂��B
                                AES_BLOCK_SIZE �̔{���łȂ���΂Ȃ�܂���B
                decryptSize:    ����������T�C�Y���w�肵�܂��B
                dst:            �Í����������o���o�b�t�@�ւ̃|�C���^���w�肵�܂��B
                                �|�C���^�� 4 byte �A���C�����g�łȂ���΂Ȃ�܂���B
                callback:       ���������������Ƃ��ɌĂяo�����R�[���o�b�N��
                                �w�肵�܂��BNULL ���w�肷�邱�Ƃ��ł��A���̏ꍇ
                                �R�[���o�b�N�͌Ăяo����܂���B
                arg:            ��L�R�[���o�b�N�ɓn�����p�����[�^���w�肵�܂��B
                                NULL ���w�肷�邱�Ƃ��ł��܂��B

  Returns:      �������ʂ�Ԃ��܂��B
 *---------------------------------------------------------------------------*/
static AESResult DEMO_CtrDecryptPartial(
    const AESCounter*   pCounter,
    const void*         pEncrypted,
    u32                 decryptOffset,
    u32                 decryptSize,
    void*               dst,
    AESCallback         callback,
    void*               arg )
{
    AESCounter counter;

    SDK_POINTER_ASSERT( pCounter );
    SDK_ASSERT( (decryptOffset % AES_BLOCK_SIZE) == 0 );

    // �����������̓J�E���^�����l���I�t�Z�b�g�Ԃ���Z���ĕ���������΂悢�B
    // �J�E���^�l�� AES_BLOCK_SIZE �ɂ� 1 ���Z�����B
    counter = *pCounter;
    AES_AddToCounter(&counter, decryptOffset / AES_BLOCK_SIZE);

    return AES_CtrDecrypt(
                &counter,
                (const u8*)pEncrypted + decryptOffset,
                decryptSize,
                dst,
                callback,
                arg );
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
