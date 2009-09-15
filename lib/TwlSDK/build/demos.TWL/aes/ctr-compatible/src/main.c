/*---------------------------------------------------------------------------*
  Project:  TwlSDK - aes - demos - ctr-compatible
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

/*---------------------------------------------------------------------------*
    �����֐��錾
 *---------------------------------------------------------------------------*/

static void         DEMO_InitInteruptSystem(void);
static void         DEMO_InitAllocSystem(void);
static void         DEMO_InitFileSystem(void);
static void         DEMO_AESCallback(AESResult result, void* arg);
static AESResult    DEMO_WaitAes(OSMessageQueue* pQ);
static void         DEMO_PrintText(const void* pvoid, u32 size);
static void         DEMO_PrintBytes(const void* pvoid, u32 size);
static void*        DEMO_LoadFile(u32* pSize, const char* path);

static AESResult    DEMO_CtrCompatible(
                        const AESKey*   pKey,
                        AESCounter*     pCounter,
                        void*           src,
                        u32             srcSize,
                        void*           dst );



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

    // AES API �̍ŏ��� AES_Init ���Ăяo���K�v������܂��B
    AES_Init();

    // CTR ���[�h�Í���
    {
        void* pPlaintext;
        void* pEncrypted;
        void* pKey;
        void* pCounter;
        u32 srcSize;

        // �Í������邽�߂̃f�[�^�����[�h���܂��B
        // ���̃T���v���ł� PC �ňÍ��������f�[�^�Ɣ�r���邽�߂�
        // �J�E���^�����l���t�@�C������ǂݍ���ł��܂����A
        // �ʏ�̓J�E���^�����l�ɂ͌Œ�l���g�p���Ă͂����܂���B
        pKey        = DEMO_LoadFile(NULL, "key.bin");
        pCounter    = DEMO_LoadFile(NULL, "counter.bin");
        pPlaintext  = DEMO_LoadFile(&srcSize, "sample16.txt");

        // �Í��������f�[�^���i�[���邽�߂̃o�b�t�@���m�ۂ��܂��B
        // �Í����Ńf�[�^�̃T�C�Y�͕ω����܂���B
        pEncrypted = OS_Alloc(srcSize);

        if( ! SDK_IS_VALID_POINTER(pEncrypted) )
        {
            OS_TPrintf("fail to OS_Alloc\n");
            return FALSE;
        }


        // �p�����[�^�̕\��
        OS_TPrintf("== CTR encrypt ==========================\n");
        OS_TPrintf("---- parameter -----------\n");
        OS_TPrintf("-- key\n");
        DEMO_PrintBytes(pKey, sizeof(AESKey));
        OS_TPrintf("-- counter initial value\n");
        DEMO_PrintBytes(pCounter, sizeof(AESCounter));

        OS_TPrintf("---- encrypt -------------\n");
        OS_TPrintf("-- plaintext (ascii)\n");
        DEMO_PrintText(pPlaintext, srcSize);
        OS_TPrintf("-- plaintext (hex)\n");
        DEMO_PrintBytes(pPlaintext, srcSize);


        // ��ʓI�� AES ���C�u�����ƌ݊��̂���Í������s���܂��B
        aesResult = DEMO_CtrCompatible(
                        pKey,               // ��
                        pCounter,           // �J�E���^�����l
                        pPlaintext,         // �Í�������f�[�^
                        srcSize,            // �Í�������f�[�^�̃T�C�Y
                        pEncrypted );       // �Í��������f�[�^���i�[����o�b�t�@
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("DEMO_CtrCompatible failed(%d)\n", aesResult);
            OS_Free(pEncrypted);
            OS_Free(pPlaintext);
            OS_Free(pCounter);
            OS_Free(pKey);
            return FALSE;
        }


        // ���ʂ̕\��
        OS_TPrintf("-- encrypted (hex)\n");
        DEMO_PrintBytes(pEncrypted, srcSize);

        // PC �ňÍ����������̂ƈ�v���邱�Ƃ��m�F
        {
            void* pEncryptedOnPC;
            u32 size;

            pEncryptedOnPC = DEMO_LoadFile(&size, "encrypted.bin");

            if( (srcSize == size) && MI_CpuComp8(pEncrypted, pEncryptedOnPC, size) == 0 )
            {
                OS_TPrintf("** pEncrypted == pEncryptedOnPC\n");
            }
            else
            {
                OS_TPrintf("** pEncrypted != pEncryptedOnPC\n");
            }

            OS_Free(pEncryptedOnPC);
        }

        OS_Free(pEncrypted);
        OS_Free(pPlaintext);
        OS_Free(pCounter);
        OS_Free(pKey);
    }

    // CTR ���[�h������
    {
        void* pEncrypted;
        void* pDecrypted;
        void* pKey;
        void* pCounter;
        u32 srcSize;

        // PC �ňÍ��������f�[�^�����[�h���܂��B
        // �T���v���ł͌����Í����� ROM �ɒu���Ă��܂���
        // ���ƈÍ����𓯂��o�H�Ŏ擾����ׂ��ł͂���܂���B
        pKey        = DEMO_LoadFile(NULL, "key.bin");
        pCounter    = DEMO_LoadFile(NULL, "counter.bin");
        pEncrypted  = DEMO_LoadFile(&srcSize, "encrypted.bin");

        // �����������f�[�^���i�[���邽�߂̃o�b�t�@���m�ۂ��܂��B
        // �������Ńf�[�^�̃T�C�Y�͕ω����܂���B
        pDecrypted = OS_Alloc(srcSize);

        if( ! SDK_IS_VALID_POINTER(pDecrypted) )
        {
            OS_TPrintf("fail to OS_Alloc\n");
            return FALSE;
        }


        // �p�����[�^�̕\��
        OS_TPrintf("== CTR decrypt ==========================\n");
        OS_TPrintf("---- parameter -----------\n");
        OS_TPrintf("-- key\n");
        DEMO_PrintBytes(pKey, sizeof(AESKey));
        OS_TPrintf("-- counter initial value\n");
        DEMO_PrintBytes(pCounter, sizeof(AESCounter));

        OS_TPrintf("---- decrypt -------------\n");
        OS_TPrintf("-- encrypted (hex)\n");
        DEMO_PrintBytes(pEncrypted, srcSize);


        // ��ʓI�� AES ���C�u�����ƌ݊��̂��镜�������s���܂��B
        aesResult = DEMO_CtrCompatible(
                        pKey,               // ���i�Í������Ɠ����łȂ���΂Ȃ�܂���j
                        pCounter,           // �J�E���^�����l�i�Í������Ɠ����łȂ���΂Ȃ�܂���j
                        pEncrypted,         // ����������f�[�^
                        srcSize,            // ����������f�[�^�̃T�C�Y
                        pDecrypted );       // �����������f�[�^���i�[����o�b�t�@
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("DEMO_CtrCompatible failed(%d)\n", aesResult);
            OS_Free(pDecrypted);
            OS_Free(pEncrypted);
            OS_Free(pCounter);
            OS_Free(pKey);
            return FALSE;
        }


        // ���ʂ̕\��
        OS_TPrintf("-- decrypted (hex)\n");
        DEMO_PrintBytes(pDecrypted, srcSize);

        OS_TPrintf("-- decrypted (ascii)\n");
        DEMO_PrintText(pDecrypted, srcSize);
        OS_TPrintf("\n");

        // �I���W�i���ƈ�v���邱�Ƃ��m�F
        {
            void* pOriginal;
            u32 size;

            pOriginal = DEMO_LoadFile(&size, "sample16.txt");

            if( (srcSize == size) && MI_CpuComp8(pDecrypted, pOriginal, size) == 0 )
            {
                OS_TPrintf("** pDecrypted == pOriginal\n");
                bSuccess = TRUE;
            }
            else
            {
                OS_TPrintf("** pDecrypted != pOriginal\n");
            }

            OS_Free(pOriginal);
        }


        OS_Free(pDecrypted);
        OS_Free(pEncrypted);
        OS_Free(pCounter);
        OS_Free(pKey);
    }

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
    DEMO_InitFileSystem();
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
  Name:         DEMO_CtrCompatible

  Description:  TWL-SDK �� AES ���C�u�����͈�ʓI�� AES ���C�u�����Əo�͌��ʂ�
                �݊���������܂���B
                ���̊֐��ł� AES �����̑O��œK�؂ȏ������s�����Ƃ�
                ��ʓI�� AES ���C�u�����ƌ݊��̂���Í���/���������s���܂��B
                �������AAES_Ctr �Ɣ�r���� srcSize ��
                AES_BLOCK_SIZE(=16) �̔{���łȂ���΂Ȃ�Ȃ��Ƃ���
                �����������܂��B

  Arguments:    pKey:       �g�p���錮���w�肵�܂��B
                pCounter:   �g�p����J�E���^�����l���w�肵�܂��B
                src:        �����܂��͈Í����ւ̃|�C���^���w�肵�܂��B
                            �|�C���^�� 4 byte �A���C�����g�łȂ���΂Ȃ�܂���B
                srcSize:    �����܂��͈Í����̃T�C�Y���w�肵�܂��B
                            16 �̔{���łȂ���΂Ȃ�܂���B
                dst:        �Í����܂��͕����������o���o�b�t�@�ւ̃|�C���^��
                            �w�肵�܂��B
                            �|�C���^�� 4 byte �A���C�����g�łȂ���΂Ȃ�܂���B

  Returns:      �������ʂ�Ԃ��܂��B
 *---------------------------------------------------------------------------*/
static AESResult DEMO_CtrCompatible(
    const AESKey*       pKey,
    AESCounter*         pCounter,
    void*               src,
    u32                 srcSize,
    void*               dst )
{
    AESKey          key;

    // �S�Ă̓��͂̃o�C�g�I�[�_�[�����ւ��܂��B
    {
        AES_ReverseBytes(pKey, &key, sizeof(key));
        AES_ReverseBytes(pCounter, pCounter, sizeof(*pCounter));
        AES_SwapEndianEach128(src, src, srcSize);
    }

    // AES ����
    {
        OSMessageQueue  msgQ;
        OSMessage       msgQBuffer[1];
        AESResult       aesResult;

        OS_InitMessageQueue(&msgQ, msgQBuffer, sizeof(msgQBuffer)/sizeof(*msgQBuffer));

        aesResult = AES_SetKey(&key);
        if( aesResult != AES_RESULT_SUCCESS )
        {
            return aesResult;
        }

        aesResult = AES_Ctr( pCounter,
                             src,
                             srcSize,
                             dst,
                             DEMO_AESCallback,
                             &msgQ );
        if( aesResult != AES_RESULT_SUCCESS )
        {
            return aesResult;
        }

        aesResult = DEMO_WaitAes(&msgQ);
        if( aesResult != AES_RESULT_SUCCESS )
        {
            return aesResult;
        }
    }

    // �o�͂̃o�C�g�I�[�_�[������ւ��܂��B
    {
        AES_SwapEndianEach128(dst, dst, srcSize);
    }

    return AES_RESULT_SUCCESS;
}


/*---------------------------------------------------------------------------*
  Name:         DEMO_LoadFile

  Description:  ���������m�ۂ��ăt�@�C����ǂݍ��݂܂��B

  Arguments:    pSize:  �ǂݍ��񂾃t�@�C���̃T�C�Y���i�[����o�b�t�@�ւ̃|�C���^�B
                        �T�C�Y���s�v�ȏꍇ�� NULL ���w�肵�܂��B
                path:   �ǂݍ��ރt�@�C���̃p�X�B

  Returns:      �t�@�C���̓��e���i�[����Ă���o�b�t�@�ւ̃|�C���^��Ԃ��܂��B
                ���̃o�b�t�@�͕s�v�ɂȂ����� OS_Free �ŉ������K�v������܂��B
                �ǂݍ��݂Ɏ��s�����ꍇ�� NULL ��Ԃ��܂��B
 *---------------------------------------------------------------------------*/
static void* DEMO_LoadFile(u32* pSize, const char* path)
{
    BOOL bSuccess;
    FSFile f;
    u32 fileSize;
    s32 readSize;
    void* pBuffer;

    FS_InitFile(&f);

    bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
    if( ! bSuccess )
    {
        OS_Warning("fail to FS_OpenFileEx(%s)\n", path);
        return NULL;
    }

    fileSize = FS_GetFileLength(&f);
    pBuffer = OS_Alloc(fileSize + 1);
    if( pBuffer == NULL )
    {
        (void)FS_CloseFile(&f);
        OS_Warning("fail to OS_Alloc(%d)\n", fileSize + 1);
        return NULL;
    }

    readSize = FS_ReadFile(&f, pBuffer, (s32)fileSize);
    if( readSize != fileSize )
    {
        OS_Free(pBuffer);
        (void)FS_CloseFile(&f);
        OS_Warning("fail to FS_ReadFile(%d)\n", fileSize + 1);
        return NULL;
    }

    bSuccess = FS_CloseFile(&f);
    SDK_ASSERT( bSuccess );

    ((char*)pBuffer)[fileSize] = '\0';

    if( pSize != NULL )
    {
        *pSize = fileSize;
    }

    return pBuffer;
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
  Name:         DEMO_PrintText

  Description:  �w�肳�ꂽ�����̕�������f�o�b�O�o�͂ɏo�͂��܂��B

  Arguments:    pvoid:  �Ώۂ̕�����
                size:   �Ώۂ̕�����̃o�C�g���B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void DEMO_PrintText(const void* pvoid, u32 size)
{
    static const u32 TMP_BUFFER_LEN = 128 - 1;
    char tmp_buffer[TMP_BUFFER_LEN + 1];
    const u8* p = (const u8*)pvoid;

    while( size >= TMP_BUFFER_LEN )
    {
        MI_CpuCopy8(p, tmp_buffer, TMP_BUFFER_LEN);
        tmp_buffer[TMP_BUFFER_LEN] = '\0';
        OS_PutString(tmp_buffer);

        size -= TMP_BUFFER_LEN;
        p    += TMP_BUFFER_LEN;
    }

    if( size > 0 )
    {
        MI_CpuCopy8(p, tmp_buffer, size);
        tmp_buffer[size] = '\0';
        OS_PutString(tmp_buffer);
    }
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

/*---------------------------------------------------------------------------*
  Name:         DEMO_InitFileSystem

  Description:  FS ������������ rom �ɃA�N�Z�X�ł���悤�ɂ��܂��B
                ���̊֐����Ăяo���O�� DEMO_InitInteruptSystem() ��
                DEMO_InitAllocSystem() ���Ă΂�Ă���K�v������܂��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void DEMO_InitFileSystem(void)
{
    void* p_table;
    u32 need_size;

    // ARM7�Ƃ̒ʐMFIFO���荞�݋���
    (void)OS_EnableIrqMask(OS_IE_SPFIFO_RECV);

    // �t�@�C���V�X�e��������
    FS_Init( FS_DMA_NOT_USE );

    // �t�@�C���e�[�u���L���b�V��
    need_size = FS_GetTableSize();
    p_table = OS_Alloc(need_size);
    SDK_POINTER_ASSERT(p_table);
    (void)FS_LoadTable(p_table, need_size);
}
