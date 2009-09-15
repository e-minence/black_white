/*---------------------------------------------------------------------------*
  Project:  TwlSDK - aes - demos - ccm-compatible
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

#define DEMO_CCM_COMPATIBLE_2_MAX_ADATA_SIZE    0xFEFF
#define DEMO_CCM_COMPATIBLE_6_MAX_ADATA_SIZE    AES_ADATA_SIZE_MAX  // �{���� 0xFFFFFFFF
#define DEMO_CCM_COMPATIBLE_2_HEADER_SIZE       2
#define DEMO_CCM_COMPATIBLE_6_HEADER_SIZE       6
#define DEMO_CCM_COMPATIBLE_6_MAGIC_NUMBER      0xFFFE

static u32  DEMO_CalcAdataHeaderSize(u32 adataSize);
static void DEMO_MakeAdataHeader(void* pv, u32 adataSize);
static AESResult    DEMO_CcmEncryptCompatible(
                        const AESKey*   pKey,
                        const AESNonce* pNonce,
                        void*           pWorkBuffer,
                        const void*     pAdata,
                        u32             adataSize,
                        const void*     pPdata,
                        u32             pdataSize,
                        AESMacLength    macLength,
                        void*           dst );
static AESResult    DEMO_CcmDecryptCompatible(
                        const AESKey*   pKey,
                        const AESNonce* pNonce,
                        void*           pWorkBuffer,
                        const void*     pAdata,
                        u32             adataSize,
                        const void*     pCdata,
                        u32             cdataSize,
                        AESMacLength    macLength,
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

    // CCM ���[�h�Í���
    {
        const AESMacLength macLength = AES_MAC_LENGTH_16;

        const u32 macSize = AES_GetMacLengthValue(macLength);
        void* pAdata;
        void* pPdata;
        void* pEncrypted;
        void* pKey;
        void* pNonce;
        u32 adataSize;
        u32 pdataSize;

        // �Í������邽�߂̃f�[�^�����[�h���܂��B
        // ���̃T���v���ł� PC �ňÍ��������f�[�^�Ɣ�r���邽�߂�
        // Nonce ���t�@�C������ǂݍ���ł��܂����A
        // �ʏ�� Nonce �ɂ͌Œ�l���g�p���Ă͂����܂���B
        pKey    = DEMO_LoadFile(NULL, "key.bin");
        pNonce  = DEMO_LoadFile(NULL, "nonce.bin");
        pAdata  = DEMO_LoadFile(&adataSize, "sample_adata.txt");
        pPdata  = DEMO_LoadFile(&pdataSize, "sample_pdata.txt");

        if( (pdataSize % AES_BLOCK_SIZE) != 0 )
        {
            OS_TPrintf("pdataSize(=%d) must be multiple of 16.\n", pdataSize);
            return FALSE;
        }


        // �Í��������f�[�^���i�[���邽�߂̃o�b�t�@���m�ۂ��܂��B
        // MAC �̕������傫���o�b�t�@���K�v�ł��B
        pEncrypted = OS_Alloc(pdataSize + macSize);

        if( ! SDK_IS_VALID_POINTER(pEncrypted) )
        {
            OS_TPrintf("fail to OS_Alloc\n");
            return FALSE;
        }


        // �p�����[�^�̕\��
        OS_TPrintf("== CCM encrypt ==========================\n");
        OS_TPrintf("---- parameter -----------\n");
        OS_TPrintf("-- key\n");
        DEMO_PrintBytes(pKey, sizeof(AESKey));
        OS_TPrintf("-- nonce\n");
        DEMO_PrintBytes(pNonce, sizeof(AESNonce));

        OS_TPrintf("---- encrypt -------------\n");
        OS_TPrintf("-- Adata (ascii)\n");
        DEMO_PrintText(pAdata, adataSize);
        OS_TPrintf("-- Pdata (ascii)\n");
        DEMO_PrintText(pPdata, pdataSize);
        OS_TPrintf("\n");
        OS_TPrintf("-- Adata (hex)\n");
        DEMO_PrintBytes(pAdata, adataSize);
        OS_TPrintf("-- Pdata (hex)\n");
        DEMO_PrintBytes(pPdata, pdataSize);


        // ��ʓI�� AES ���C�u�����ƌ݊��̂���Í������s���܂��B
        if( adataSize <= DEMO_CCM_COMPATIBLE_6_MAX_ADATA_SIZE )
        {
            const u32 headerSize = DEMO_CalcAdataHeaderSize(adataSize);
            const u32 workSize   = headerSize + adataSize + pdataSize;
            void* pWorkBuffer    = OS_Alloc(workSize);

            aesResult = DEMO_CcmEncryptCompatible(
                            pKey,               // ��
                            pNonce,             // Nonce
                            pWorkBuffer,        // ��Ɨp�o�b�t�@
                            pAdata,             // Adata ���i�[����Ă���o�b�t�@�ւ̃|�C���^
                            adataSize,          // Adata �̃T�C�Y
                            pPdata,             // Pdata ���i�[����Ă���o�b�t�@�ւ̃|�C���^
                            pdataSize,          // Pdata �̃T�C�Y
                            macLength,          // MAC �̃T�C�Y
                            pEncrypted );       // �Í��������f�[�^���i�[����o�b�t�@

            OS_Free(pWorkBuffer);
        }
        else
        {
            OS_TPrintf("Too huge Adata size(=%d)\n", adataSize);
            OS_Free(pEncrypted);
            OS_Free(pPdata);
            OS_Free(pAdata);
            OS_Free(pNonce);
            OS_Free(pKey);
            return FALSE;
        }
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("DEMO_CcmEncryptCompatible failed(%d)\n", aesResult);
            OS_Free(pEncrypted);
            OS_Free(pPdata);
            OS_Free(pAdata);
            OS_Free(pNonce);
            OS_Free(pKey);
            return FALSE;
        }


        // ���ʂ̕\��
        OS_TPrintf("-- encrypted (hex)\n");
        DEMO_PrintBytes(pEncrypted, pdataSize);

        OS_TPrintf("-- mac (hex)\n");
        DEMO_PrintBytes((u8*)pEncrypted + pdataSize, macSize);

        // PC �ňÍ����������̂ƈ�v���邱�Ƃ��m�F
        {
            void* pEncryptedOnPC;
            u32 size;

            pEncryptedOnPC = DEMO_LoadFile(&size, "encrypted.bin");

            if( (pdataSize + macSize == size) && MI_CpuComp8(pEncrypted, pEncryptedOnPC, size) == 0 )
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
        OS_Free(pPdata);
        OS_Free(pAdata);
        OS_Free(pNonce);
        OS_Free(pKey);
    }

    // CCM ���[�h������
    {
        const AESMacLength macLength = AES_MAC_LENGTH_16;

        const u32 macSize = AES_GetMacLengthValue(macLength);
        void* pAdata;
        void* pEncrypted;
        void* pDecrypted;
        void* pKey;
        void* pNonce;
        u32 adataSize;
        u32 cdataSize;
        u32 srcFileSize;

        // PC �ňÍ��������f�[�^�����[�h���܂��B
        // �T���v���ł͌����Í����� ROM �ɒu���Ă��܂���
        // ���ƈÍ����𓯂��o�H�Ŏ擾����ׂ��ł͂���܂���B
        pKey        = DEMO_LoadFile(NULL, "key.bin");
        pNonce      = DEMO_LoadFile(NULL, "nonce.bin");
        pAdata      = DEMO_LoadFile(&adataSize, "sample_adata.txt");
        pEncrypted  = DEMO_LoadFile(&srcFileSize, "encrypted.bin");
        cdataSize = srcFileSize - macSize;

        // �����������f�[�^���i�[���邽�߂̃o�b�t�@���m�ۂ��܂��B
        pDecrypted = OS_Alloc(cdataSize);

        if( ! SDK_IS_VALID_POINTER(pDecrypted) )
        {
            OS_TPrintf("fail to OS_Alloc\n");
            return FALSE;
        }

        // �p�����[�^�̕\��
        OS_TPrintf("== CCM decrypt ==========================\n");
        OS_TPrintf("---- parameter -----------\n");
        OS_TPrintf("-- key\n");
        DEMO_PrintBytes(pKey, sizeof(AESKey));
        OS_TPrintf("-- nonce\n");
        DEMO_PrintBytes(pNonce, sizeof(AESNonce));

        OS_TPrintf("---- decrypt -------------\n");
        OS_TPrintf("-- Adata (ascii)\n");
        DEMO_PrintText(pAdata, adataSize);
        OS_TPrintf("\n");
        OS_TPrintf("-- Adata (hex)\n");
        DEMO_PrintBytes(pAdata, adataSize);
        OS_TPrintf("-- encrypted (hex)\n");
        DEMO_PrintBytes(pEncrypted, cdataSize + macSize);

        // ��ʓI�� AES ���C�u�����ƌ݊��̂��镜�������s���܂��B
        if( adataSize <= DEMO_CCM_COMPATIBLE_6_MAX_ADATA_SIZE )
        {
            const u32 headerSize = DEMO_CalcAdataHeaderSize(adataSize);
            const u32 workSize   = headerSize + adataSize + cdataSize + macSize;
            void* pWorkBuffer    = OS_Alloc(workSize);

            aesResult = DEMO_CcmDecryptCompatible(
                            pKey,               // ��
                            pNonce,             // Nonce
                            pWorkBuffer,        // ��Ɨp�o�b�t�@
                            pAdata,             // Adata ���i�[����Ă���o�b�t�@�ւ̃|�C���^
                            adataSize,          // Adata �̃T�C�Y
                            pEncrypted,         // �Í����� MAC ���i�[����Ă���o�b�t�@�ւ̃|�C���^
                            cdataSize,          // �Í����̃T�C�Y
                            macLength,          // MAC �̃T�C�Y
                            pDecrypted );       // �����������f�[�^���i�[����o�b�t�@

            OS_Free(pWorkBuffer);
        }
        else
        {
            OS_TPrintf("Too huge Adata size(=%d)\n", adataSize);
            OS_Free(pEncrypted);
            OS_Free(pAdata);
            OS_Free(pNonce);
            OS_Free(pKey);
            return FALSE;
        }
        // �Í��������f�[�^����₂���Ă����
        // �����œ����� aesResult �� AES_RESULT_VERIFICATION_FAILED �ɂȂ�܂��B
        if( aesResult != AES_RESULT_SUCCESS )
        {
            OS_TPrintf("DEMO_CcmDecryptCompatible failed(%d)\n", aesResult);
            OS_Free(pDecrypted);
            OS_Free(pEncrypted);
            OS_Free(pAdata);
            OS_Free(pNonce);
            OS_Free(pKey);
            return FALSE;
        }


        // ���ʂ̕\��
        OS_TPrintf("-- decrypted (hex)\n");
        DEMO_PrintBytes(pDecrypted, cdataSize);

        OS_TPrintf("-- decrypted (ascii)\n");
        DEMO_PrintText(pDecrypted, cdataSize);
        OS_TPrintf("\n");

        // �I���W�i���ƈ�v���邱�Ƃ��m�F
        {
            void* pOriginal;
            u32 size;

            pOriginal = DEMO_LoadFile(&size, "sample_pdata.txt");

            if( (cdataSize == size) && MI_CpuComp8(pDecrypted, pOriginal, size) == 0 )
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
        OS_Free(pAdata);
        OS_Free(pNonce);
        OS_Free(pKey);
    }

    return bSuccess;
}

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  ���C���֐��ł��B

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
  Name:         DEMO_CalcAdataHeaderSize

  Description:  TWL-SDK �� AES ���C�u�����͈�ʓI�� AES ���C�u������
                �o�͌��ʂɌ݊���������܂���B
                �݊��������Ă��Ȃ����̂̈�Ƃ���
                CCM �w�b�_�̒���ɔz�u����� Adata �̃T�C�Y�������I��
                �t������Ȃ��Ƃ������̂�����܂��B
                ���̊֐��� Adata �̃T�C�Y���i�[����̂ɕK�v�ȗ̈�T�C�Y��
                �v�Z���܂��B

  Arguments:    adataSize:      Adata �̃T�C�Y�B

  Returns:      Adata �̃T�C�Y���i�[����̂ɕK�v�ȗ̈�T�C�Y��Ԃ��܂��B
 *---------------------------------------------------------------------------*/
static u32 DEMO_CalcAdataHeaderSize(u32 adataSize)
{
    return (u32)( (adataSize <= DEMO_CCM_COMPATIBLE_2_MAX_ADATA_SIZE)
                    ? DEMO_CCM_COMPATIBLE_2_HEADER_SIZE:
                      DEMO_CCM_COMPATIBLE_6_HEADER_SIZE );
}



/*---------------------------------------------------------------------------*
  Name:         DEMO_MakeAdataHeader

  Description:  TWL-SDK �� AES ���C�u�����͈�ʓI�� AES ���C�u������
                �o�͌��ʂɌ݊���������܂���B
                �݊��������Ă��Ȃ����̂̈�Ƃ���
                CCM �w�b�_�̒���ɔz�u����� Adata �̃T�C�Y�������I��
                �t������Ȃ��Ƃ������̂�����܂��B
                ���̊֐��� Adata �̃T�C�Y���K�肳��Ă���t�H�[�}�b�g��
                �o�b�t�@�ɏ����o���܂��B

  Arguments:    pv:             Adata �̃T�C�Y�������o���o�b�t�@�ւ̃|�C���^�B
                adataSize:      Adata �̃T�C�Y�B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void DEMO_MakeAdataHeader(void* pv, u32 adataSize)
{
    u8* p = (u8*)pv;

    if( adataSize <= DEMO_CCM_COMPATIBLE_2_MAX_ADATA_SIZE )
    {
        MI_StoreBE16(p, (u16)adataSize);
    }
    else
    {
        MI_StoreBE16(p + 0, DEMO_CCM_COMPATIBLE_6_MAGIC_NUMBER);
        MI_StoreBE32(p + 2, adataSize);
    }
}



/*---------------------------------------------------------------------------*
  Name:         DEMO_CcmEncryptCompatible

  Description:  TWL-SDK �� AES ���C�u�����͈�ʓI�� AES ���C�u������
                �o�͌��ʂɌ݊���������܂���B
                ���̊֐��ł� AES �����̑O��œK�؂ȏ������s�����Ƃ�
                ��ʓI�� AES ���C�u�����ƌ݊��̂���Í������s���܂��B
                �������AAES_CcmEncryptAndSign �Ɣ�r���� pdataSize ��
                AES_BLOCK_SIZE(=16) �̔{���łȂ���΂Ȃ�Ȃ��Ƃ���
                �����������܂��B�܂� adataSize �̐����� 16 �̔{���ł͂Ȃ��A
                adataSize �ɂ���� 16 �̔{�� - 2 �܂��� 16 �̔{�� - 4 �ɂȂ�܂��B

  Arguments:    pKey:           �g�p���錮���w�肵�܂��B
                pNonce:         �g�p���� Nonce ���w�肵�܂��B
                pWorkBuffer:    �֐������Ŏg�p�����Ɨp�o�b�t�@�ւ̃|�C���^��
                                �n���܂��B
                                adataSize + pdataSize
                                          + DEMO_CalcAdataHeaderSize(adataSize)
                                �̃T�C�Y���K�v�ł��B
                                �|�C���^�� 4 byte �A���C�����g�łȂ���΂Ȃ�܂���B
                pAdata          Adata �ւ̃|�C���^���w�肵�܂��B
                adataSize:      Adata �̃T�C�Y���w�肵�܂��B
                                16 �̔{�� - DEMO_CalcAdataHeaderSize(adataSize)
                                �łȂ���΂Ȃ�܂���B
                pPdata:         Pdata �ւ̃|�C���^���w�肵�܂��B
                pdataSize:      Pdata �̃T�C�Y���w�肵�܂��B
                                16 �̔{���łȂ���΂Ȃ�܂���B
                macLength:      MAC �̃T�C�Y���w�肵�܂��B
                dst:            �Í����������o���o�b�t�@�ւ̃|�C���^���w�肵�܂��B
                                pdataSize + DEMO_CalcAdataHeaderSize(adataSize)
                                �̃T�C�Y���K�v�ł��B
                                �|�C���^�� 4 byte �A���C�����g�łȂ���΂Ȃ�܂���B

  Returns:      �������ʂ�Ԃ��܂��B
 *---------------------------------------------------------------------------*/
AESResult DEMO_CcmEncryptCompatible(
    const AESKey*   pKey,
    const AESNonce* pNonce,
    void*           pWorkBuffer,
    const void*     pAdata,
    u32             adataSize,
    const void*     pPdata,
    u32             pdataSize,
    AESMacLength    macLength,
    void*           dst )
{
    const u32 headerSize = DEMO_CalcAdataHeaderSize(adataSize);
    AESKey    key;
    AESNonce  nonce;

    SDK_POINTER_ASSERT(pKey);
    SDK_POINTER_ASSERT(pNonce);
    SDK_POINTER_ASSERT(pWorkBuffer);
    SDK_POINTER_ASSERT(pAdata);
    SDK_POINTER_ASSERT(pPdata);
    SDK_POINTER_ASSERT(dst);
    SDK_MAX_ASSERT( adataSize, AES_ADATA_SIZE_MAX );
    SDK_MAX_ASSERT( pdataSize, AES_PDATA_SIZE_MAX );
    SDK_ASSERT( (adataSize >  DEMO_CCM_COMPATIBLE_2_MAX_ADATA_SIZE) || ((adataSize % 16) == 14) );
    SDK_ASSERT( (adataSize <= DEMO_CCM_COMPATIBLE_2_MAX_ADATA_SIZE) || ((adataSize % 16) == 10) );
    SDK_ASSERT( (pdataSize % 16) ==  0 );

    // ���̓f�[�^���쐬���܂��B
    {
        const u32 offsetHeader = 0;
        const u32 offsetAdata  = offsetHeader + headerSize;
        const u32 offsetPdata  = offsetAdata  + adataSize;

        u8* pWork = (u8*)pWorkBuffer;
        DEMO_MakeAdataHeader(pWork + offsetHeader, adataSize);  // Adata �T�C�Y
        MI_CpuCopy(pAdata, pWork + offsetAdata, adataSize);     // Adata
        MI_CpuCopy(pPdata, pWork + offsetPdata, pdataSize);     // Pdata
    }

    // �S�Ă̓��͂̃o�C�g�I�[�_�[�����ւ��܂��B
    {
        const u32 workSize = headerSize + adataSize + pdataSize;

        AES_ReverseBytes(pKey, &key, sizeof(key));                  // ��
        AES_ReverseBytes(pNonce, &nonce, sizeof(nonce));            // Nonce
        AES_SwapEndianEach128(pWorkBuffer, pWorkBuffer, workSize);  // Adata �T�C�Y�AAdata�APdata
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

        aesResult = AES_CcmEncryptAndSign(
                        &nonce,
                        pWorkBuffer,
                        adataSize + headerSize,
                        pdataSize,
                        macLength,
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
        void* mac = (u8*)dst + pdataSize;
        u32 macSize = AES_GetMacLengthValue(macLength);

        AES_SwapEndianEach128(dst, dst, pdataSize);     // Cdata
        AES_ReverseBytes(mac, mac, macSize);            // MAC
    }

    return AES_RESULT_SUCCESS;
}



/*---------------------------------------------------------------------------*
  Name:         DEMO_CcmDecryptCompatible

  Description:  TWL-SDK �� AES ���C�u�����͈�ʓI�� AES ���C�u������
                �o�͌��ʂɌ݊���������܂���B
                ���̊֐��ł� AES �����̑O��œK�؂ȏ������s�����Ƃ�
                ��ʓI�� AES ���C�u�����ƌ݊��̂��镜�������s���܂��B
                �������AAES_CcmDecryptAndVerify �Ɣ�r���� cdataSize ��
                AES_BLOCK_SIZE(=16) �̔{���łȂ���΂Ȃ�Ȃ��Ƃ���
                �����������܂��B�܂� adataSize �̐����� 16 �̔{���ł͂Ȃ��A
                adataSize �ɂ���� 16 �̔{�� - 2 �܂��� 16 �̔{�� - 4 �ɂȂ�܂��B

  Arguments:    pKey:           �g�p���錮���w�肵�܂��B
                pNonce:         �g�p���� Nonce ���w�肵�܂��B
                pWorkBuffer:    �֐������Ŏg�p�����Ɨp�o�b�t�@�ւ̃|�C���^��
                                �n���܂��B
                                adataSize + pdataSize
                                          + DEMO_CalcAdataHeaderSize(adataSize)
                                          + AES_GetMacLengthValue(macLength)
                                �̃T�C�Y���K�v�ł��B
                                �|�C���^�� 4 byte �A���C�����g�łȂ���΂Ȃ�܂���B
                pAdata          Adata �ւ̃|�C���^���w�肵�܂��B
                adataSize:      Adata �̃T�C�Y���w�肵�܂��B
                                16 �̔{�� - DEMO_CalcAdataHeaderSize(adataSize)
                                �łȂ���΂Ȃ�܂���B
                pPdata:         �Í����� MAC ���i�[���ꂽ�o�b�t�@�ւ̃|�C���^���w�肵�܂��B
                pdataSize:      �Í����̃T�C�Y���w�肵�܂��B
                                16 �̔{���łȂ���΂Ȃ�܂���B
                macLength:      MAC �̃T�C�Y���w�肵�܂��B
                dst:            �Í����������o���o�b�t�@�ւ̃|�C���^���w�肵�܂��B
                                �|�C���^�� 4 byte �A���C�����g�łȂ���΂Ȃ�܂���B

  Returns:      �������ʂ�Ԃ��܂��B
 *---------------------------------------------------------------------------*/
AESResult DEMO_CcmDecryptCompatible(
    const AESKey*   pKey,
    const AESNonce* pNonce,
    void*           pWorkBuffer,
    const void*     pAdata,
    u32             adataSize,
    const void*     pCdata,
    u32             cdataSize,
    AESMacLength    macLength,
    void*           dst )
{
    const u32 headerSize = DEMO_CalcAdataHeaderSize(adataSize);
    const u32 macSize    = AES_GetMacLengthValue(macLength);
    AESKey    key;
    AESNonce  nonce;

    SDK_POINTER_ASSERT(pKey);
    SDK_POINTER_ASSERT(pNonce);
    SDK_POINTER_ASSERT(pWorkBuffer);
    SDK_POINTER_ASSERT(pAdata);
    SDK_POINTER_ASSERT(pCdata);
    SDK_POINTER_ASSERT(dst);
    SDK_MAX_ASSERT( adataSize, AES_ADATA_SIZE_MAX );
    SDK_MAX_ASSERT( cdataSize, AES_PDATA_SIZE_MAX );
    SDK_ASSERT( (adataSize >  DEMO_CCM_COMPATIBLE_2_MAX_ADATA_SIZE) || ((adataSize % 16) == 14) );
    SDK_ASSERT( (adataSize <= DEMO_CCM_COMPATIBLE_2_MAX_ADATA_SIZE) || ((adataSize % 16) == 10) );
    SDK_ASSERT( (cdataSize % 16) ==  0 );

    // ���̓f�[�^���쐬���܂��B
    {
        const u32 offsetHeader = 0;
        const u32 offsetAdata  = offsetHeader + headerSize;
        const u32 offsetCdata  = offsetAdata  + adataSize;

        u8* pWork = (u8*)pWorkBuffer;
        DEMO_MakeAdataHeader(pWork + offsetHeader, adataSize);          // Adata �T�C�Y
        MI_CpuCopy(pAdata, pWork + offsetAdata, adataSize);             // Adata
        MI_CpuCopy(pCdata, pWork + offsetCdata, cdataSize + macSize);   // Cdata�AMAC
    }

    // �S�Ă̓��͂̃o�C�g�I�[�_�[�����ւ��܂��B
    {
        const u32 workSize = headerSize + adataSize + cdataSize + macSize;

        AES_ReverseBytes(pKey, &key, sizeof(key));                  // ��
        AES_ReverseBytes(pNonce, &nonce, sizeof(nonce));            // Nonce
        AES_SwapEndianEach128(pWorkBuffer, pWorkBuffer, workSize);  // Adata �T�C�Y�AAdata�ACdata�AMAC
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

        aesResult = AES_CcmDecryptAndVerify(
                        &nonce,
                        pWorkBuffer,
                        adataSize + headerSize,
                        cdataSize,
                        macLength,
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
        AES_SwapEndianEach128(dst, dst, cdataSize);     // Pdata
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
