/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tcl - demos.TWL - tcl-2
  File:     main.c

  Copyright 2008-2009 Nintendo.  All rights reserved.

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
#include <twl/tcl.h>
#include <twl/ssp/ARM9/jpegenc.h>
#include <twl/ssp/ARM9/exifenc.h>

#include "DEMO.h"
#include "screen.h"

/*---------------------------------------------------------------------------*
    �萔 ��`
 *---------------------------------------------------------------------------*/
#define BMP_HEADER_SIZE 54 // BMP�t�@�C���̃w�b�_�T�C�Y�iWindows bitmap�j

/*---------------------------------------------------------------------------*
    �����֐� ��`
 *---------------------------------------------------------------------------*/
void VBlankIntr(void);
void Print(void);
void BMPToRGB( const u8* p_bmp, u16* p_rgb );

/*---------------------------------------------------------------------------*
    �����ϐ� ��`
 *---------------------------------------------------------------------------*/
int  state = 0;
int  num_pict = 0;
int  rest_pict = 0;

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃n���h��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

/*---------------------------------------------------------------------------*
  Name:         Print

  Description:  �f�o�O���`��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void Print(void)
{
    ClearScreen();

    PutScreen(0, 0, 0xff, "TCL-2 Write Picture Demo");

    PutScreen(2, 4, 0xff, "Num Pictures       : %d", num_pict );
    PutScreen(2, 3, 0xff, "Rest Num Pictures  : %d", rest_pict );

    switch( state )
    {
    case 0: PutScreen( 1, 8, 0xff, "A Button > Write Picture" ); break;
    case 1: PutScreen( 1, 8, 0xff, "Now Writing..." );           break;
    case 2: PutScreen( 1, 8, 0xff, "Finish!" );                  break;
    case 3: PutScreen( 1, 8, 0xff, "Not enable Write Picture" ); break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         BMPToRGB

  Description:  24bitBPM��RGB555�ɕϊ�

  Arguments:    p_bmp ��Bitmap�摜
                p_rgb �VRGB�摜

  Returns:      None.
 *---------------------------------------------------------------------------*/
void BMPToRGB( const u8* p_bmp, u16* p_rgb )
{
    int h, w;
    int b, g, r;

    for ( h = 0; h < TCL_JPEG_HEIGHT; ++h )
    {
        for ( w = 0; w < TCL_JPEG_WIDTH; ++w )
        {
            b = ( p_bmp[ 3 * ( h * TCL_JPEG_WIDTH + w ) + 0 ] >> 3 ) & 0x1f;
            g = ( p_bmp[ 3 * ( h * TCL_JPEG_WIDTH + w ) + 1 ] >> 3 ) & 0x1f;
            r = ( p_bmp[ 3 * ( h * TCL_JPEG_WIDTH + w ) + 2 ] >> 3 ) & 0x1f;

            p_rgb[ ( TCL_JPEG_HEIGHT - h - 1 ) * TCL_JPEG_WIDTH + w ] = (u16)( 0x8000 |
                                                                               ( b << GX_RGBA_B_SHIFT ) |
                                                                               ( g << GX_RGBA_G_SHIFT ) |
                                                                               ( r << GX_RGBA_R_SHIFT ) );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void TwlMain(void)
{
    OSHeapHandle handle;
    void* nstart;

    void* pTableBuf;
    void* pWorkBuf;

    FSResult fs_result;
    TCLResult result;

    // TCL�A�N�Z�T
    TCLAccessor accessor;

    // ���������I�u�W�F�N�g
    TCLSearchObject search_obj;

    BOOL write_enable = TRUE;
    int count;

    // ����������
    DEMOInitCommon();
    DEMOInitVRAM();

    OS_InitTick();
    OS_InitAlarm();
    RTC_Init();
    FS_Init( FS_DMA_NOT_USE );

    InitScreen();
    ClearScreen();

    // GX�֘A�ݒ�
    GX_SetBankForBG( GX_VRAM_BG_128_A );

    GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BG0_AS_2D );

    GX_SetVisiblePlane( GX_PLANEMASK_BG3 );
    G2_SetBG3Priority( 0 );
    G2_BG3Mosaic( FALSE );

    G2_SetBG3ControlDCBmp( GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000 );

    // �q�[�v�쐬
    nstart = OS_InitAlloc( OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1 );
    OS_SetMainArenaLo( nstart );

    handle = OS_CreateHeap( OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi() );
    if (handle < 0)
    {
        OS_Panic("ARM9: Fail to create heap...");
    }
    handle = OS_SetCurrentHeap( OS_ARENA_MAIN, handle );


    // TCL������
    // 32byte�A���C�����g���K�v
    pTableBuf = OS_Alloc( TCL_GetTableBufferSize() );
    pWorkBuf = OS_Alloc( TCL_GetWorkBufferSize() );
    if( pTableBuf == NULL || pWorkBuf == NULL )
    {
        OS_Panic("Cannot allocate memory!");
    }

    result = TCL_LoadTable( &accessor,
                            pTableBuf,
                            TCL_GetTableBufferSize(),
                            pWorkBuf,
                            TCL_GetWorkBufferSize(),
                            &fs_result );

#if 1

    // �Ǘ��t�@�C���̓ǂݍ��݂Ɏ��s�����ꍇ�Ɉȍ~�̏������I������

    if ( result != TCL_RESULT_SUCCESS )
    {
        // ����ȊO�̃G���[�̏ꍇ��DSi�J�����̋N���𑣂��ď����I��
        //OS_TPrintf( "����炵��������" );

        OS_Panic("failed TCL_LoadTable : %d\n", result );
    }

#else

    // �Ǘ��t�@�C���̓ǂݍ��݂Ɏ��s�����ꍇ�ɕ��������݂�

    switch( result )
    {
    case TCL_RESULT_SUCCESS: break; // ����
    case TCL_RESULT_ERROR_EXIST_OTHER_FILE:
    case TCL_RESULT_ERROR_ALREADY_MANAGED:
        OS_TPrintf("failed TCL_LoadTable : %d\n", result );

        // �����o���������s���Ȃ��̂Ŏ���ۑ�����ēx�v�Z����
        result = TCL_RepairTable( &accessor, &fs_result );

        if( result != TCL_RESULT_SUCCESS )
        {
            OS_TPrintf("failed TCL_RepairTable : %d\n", result );

            // �摜�����o���������o���Ȃ���ԂȂ̂ŊǗ��t�@�C������蒼��
            result = TCL_CreateTable( &accessor,
                                      pTableBuf,
                                      TCL_GetTableBufferSize(),
                                      pWorkBuf,
                                      TCL_GetWorkBufferSize(),
                                      &fs_result );

            switch( result )
            {
            case TCL_RESULT_SUCCESS:
                // �e�[�u���쐬����
                break;
            case TCL_RESULT_ERROR_OVER_NUM_PICTURES:
                // 500���ȏ�ʐ^�����݂������A�e�[�u���쐬�͐���
                OS_TPrintf("failed TCL_CreateTable : %d", result );
                break;
            case TCL_RESULT_ERROR_NO_NEXT_INDEX:
                // �ۑ��͏o���Ȃ������s�\�ȃG���[
                write_enable = FALSE;
                OS_TPrintf("failed TCL_CreateTable : %d", result );
                break;
            default:
                // ���s�s�\�ȃG���[
                OS_Panic("failed TCL_CreateTable : %d", result );
                break;
            }
        }
        break;
    default:
        OS_TPrintf("failed TCL_LoadTable : %d\n", result );

        // �ǂݍ��ݏ������s���Ȃ��̂ŊǗ��t�@�C������蒼��
        result = TCL_CreateTable( &accessor,
                                  pTableBuf,
                                  TCL_GetTableBufferSize(),
                                  pWorkBuf,
                                  TCL_GetWorkBufferSize(),
                                  &fs_result );

        switch( result )
        {
        case TCL_RESULT_SUCCESS: break;
        case TCL_RESULT_ERROR_OVER_NUM_PICTURES:
            OS_TPrintf("failed TCL_CreateTable : %d", result );
            break;
        case TCL_RESULT_ERROR_NO_NEXT_INDEX:
            write_enable = FALSE;
            OS_TPrintf("failed TCL_CreateTable : %d", result );
            break;
        default:
            OS_Panic("failed TCL_CreateTable : %d", result );
            break;
        }
        break;
    }

#endif

    // ���Ɖ����ʐ^��ۑ��ł��邩
    rest_pict = TCL_CalcNumEnableToTakePictures( &accessor );

    // SearchObject��������
    TCL_InitSearchObject( &search_obj, TCL_SEARCH_CONDITION_PICTURE );

    // �S���ŉ����ʐ^�����邩
    num_pict = TCL_GetNumPictures( &accessor, &search_obj );

    Print();

    DEMOStartDisplay();

    // �e�L�X�g�\�����X�V
    UpdateScreen();

    while (1)
    {
        // �p�b�h���̓ǂݍ��݂Ƒ���
        DEMOReadKey();

        if( DEMO_IS_TRIG( PAD_BUTTON_A ) )
        {
            if( rest_pict == 0 || !write_enable )
            {
                // ����ȏ�ۑ��o���Ȃ�
                count = 0;
                state = 3;
                Print();
            }
            else if( state == 0 )
            {
                count = 0;
                ++state;
                Print();
            }
        }
        else if( state == 1 )
        {
            // jpeg�摜���i�[����o�b�t�@
            u8*  pJpegBuf = NULL;
            u8*  pData    = NULL;
            u8*  pTmpBuf  = NULL;
            u16* pRGBBuf  = NULL;
            u32  length;
            FSFile file;
            const u32 option = SSP_JPEG_THUMBNAIL | SSP_JPEG_RGB555;

            // BMP�摜�擾
            FS_InitFile( &file );
            if( !FS_OpenFileEx( &file, "rom:/demo.bmp", FS_FILEMODE_R ) )
            {
                OS_Panic("failed FS_OpenFileEx : %d ", FS_GetResultCode( &file ) );
            }

            length = FS_GetFileLength( &file );
            pData = OS_Alloc( length );
            if( pData == NULL )
            {
                OS_Panic("Cannot allocate memory ... pData!");
            }

            if( FS_ReadFile( &file, pData, (s32)length ) == -1 )
            {
                OS_Panic("failed FS_ReadFile");
            }
            ( void ) FS_CloseFile( &file );

            pData += BMP_HEADER_SIZE;

            // BMP�摜��RGB555�ɕϊ�
            pRGBBuf = OS_Alloc( TCL_JPEG_WIDTH * TCL_JPEG_HEIGHT * sizeof(u16) );
            if( pRGBBuf == NULL )
            {
                OS_Panic("Cannot allocate memory ... pRGBBuf!");
            }

            BMPToRGB( pData, pRGBBuf );
            pData -= BMP_HEADER_SIZE;

            OS_Free( pData );

            // JPEG�G���R�[�h
            length = TCL_GetJpegEncoderBufferSize( option );
            pTmpBuf = OS_Alloc( length );
            if( pTmpBuf == NULL )
            {
                OS_Panic("Cannot allocate memory ... pTmpBuf!");
            }
            OS_TPrintf("SSP_GetJpegEncoderBufferSize() = %d\n", length );

            pJpegBuf = OS_Alloc( TCL_MAX_JPEG_SIZE );
            if( pJpegBuf == NULL )
            {
                OS_Panic("Cannot allocate memory ... pJpegBuf!");
            }

            // �G���R�[�h�{��������
            result = TCL_EncodeAndWritePicture( &accessor,
                                                pRGBBuf ,
                                                pJpegBuf,
                                                TCL_MAX_JPEG_SIZE ,
                                                pTmpBuf ,
                                                TCL_JPEG_DEFAULT_QUALITY ,
                                                option ,
                                                &fs_result );

            if( result != TCL_RESULT_SUCCESS )
            {
                OS_TPrintf("failed TCL_WritePicture : %d\n", result );

                // ����ȏ�ۑ��o���Ȃ�
                if( result == TCL_RESULT_ERROR_NO_NEXT_INDEX ) write_enable = FALSE;
            }
            else
            {
                OS_TPrintf("success TCL_WritePicture! %s\n" , TCL_GetLastWrittenPicturePath() );
            }

            OS_Free( pJpegBuf );
            OS_Free( pTmpBuf );
            OS_Free( pRGBBuf );

            // ���Ɖ����ʐ^��ۑ��ł��邩
            rest_pict = TCL_CalcNumEnableToTakePictures( &accessor );

            // �S���ŉ����ʐ^�����邩
            num_pict = TCL_GetNumPictures( &accessor, &search_obj );

            ++state;
            Print();
        }
        else if( state == 2 || state == 3 )
        {
            ++count;
            if( count > 180 )
            {
                state = 0;
                Print();
            }
        }

        OS_WaitVBlankIntr();

        // �e�L�X�g�\�����X�V
        UpdateScreen();
    }

    OS_Free( pWorkBuf );
    OS_Free( pTableBuf );

    OS_Terminate();
}
