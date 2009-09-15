/*---------------------------------------------------------------------------*
  Project:  TwlSDK - nandApp - demos - backup
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
#include <DEMO.h>

static void PrintBootType();
static void InitDEMOSystem();
static void InitInteruptSystem();
static void InitAllocSystem();
static void InitFileSystem();

static char* LoadFile(const char* path);
static BOOL SaveFile(const char* path, void* pData, u32 size);

static void PrintDirectory(const char* path);
static void CreateTree(const char* arc_name);
static void DeleteData(char *path);
static BOOL WriteData(const char *path, void* pData, u32 size);
static void ReadData(const char* arc_name);

static char* GetTestData(char *out, u32 size);
static void DrawString(const char *fmt, ...);
static void PrintTree(const char* path, u32 space);

static const u32 BUF_SIZE = 256;

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  ���C���֐��ł��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
void TwlMain(void)
{
    OS_Init();
    RTC_Init();
    InitInteruptSystem();
    InitFileSystem();
    InitAllocSystem();
    InitDEMOSystem();
    DrawString("*** start nandApp demo\n");

    DrawString("Build:%s %s\n", __DATE__, __TIME__);
    PrintBootType();
    DrawString("\n");
    DrawString("A - Delete, Create, Read\n");
    DrawString("B - Print Directory Tree\n");


    // NAND �A�v���̃o�b�N�A�b�v�f�[�^�� NAND ��ɕۑ�����܂��B
    // ���̂��ߎg�p���� API �� CARD API �ł͂Ȃ� FS API �ƂȂ�A
    // �t�@�C���V�X�e���Ƃ��ăA�N�Z�X���邱�Ƃ��ł��܂��B
    //
    // �{�v���O�����ł�
    // 1.�t�@�C���̓ǂݍ��� 
    // 2.�o�b�N�A�b�v�f�[�^�̏��� 
    // 3.�f�B���N�g���c���[�E�t�@�C���̍쐬
    // �̗�������܂��B 
    
    // �udataPub�v�� SD �J�[�h�ɃR�s�[�����
    // �o�b�N�A�b�v�f�[�^�̈�̃A�[�J�C�u���ł��B
    // ��ʓI�ȃf�[�^�͂�������g�p���܂��B

    // �udataPrv�v�� SD �J�[�h�ɃR�s�[����Ȃ�
    // �o�b�N�A�b�v�f�[�^�̈�̃A�[�J�C�u���ł��B
    // �R�s�[���ꂽ���Ȃ��f�[�^��ۑ�����̂Ɏg�p���܂��B
    {
        u32 mode = 0;
        // HW���Z�b�g��L���ɂ��邽�ߏI�����Ȃ�
        for (;;)
        {
            // �t���[���X�V
            DEMOReadKey();
            if(DEMO_IS_TRIG(PAD_BUTTON_A))
            {
                // ��ʂ̃N���A
                DrawString("");
                DEMOFillRect(0, 0, 256, 192, GX_RGBA(0, 0, 0, 1));
                switch(mode)
                {
                    case 0: // �t�@�C���ǂݍ��݂̎��s 
                        DrawString("Read\n");
                        ReadData("dataPub:");
                        ReadData("dataPrv:");
                        break;
                    case 1: // �����̎��s 
                        DrawString("Delete\n");
                        DeleteData("dataPub:");
                        DeleteData("dataPrv:");
                        break;
                    case 2: // �f�B���N�g���E�t�@�C���쐬�̎��s 
                        DrawString("Create\n");
                        CreateTree("dataPub:");
                        CreateTree("dataPrv:");
                        break;
                }
                mode = (mode + 1) % 3;
            }else if(DEMO_IS_TRIG(PAD_BUTTON_B))
            {
                // �f�B���N�g���c���[�̕\��
                DrawString("");
                DEMOFillRect(0, 0, 256, 192, GX_RGBA(0, 0, 0, 1));
                DrawString("Tree\ndataPub:\n");
                PrintTree("dataPub:", 1);
                DrawString("\ndataPrv:\n");
                PrintTree("dataPrv:", 1);
            }
            DEMO_DrawFlip();
            OS_WaitVBlankIntr();
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         LoadFile

  Description:  �����Ń��������m�ۂ��t�@�C����ǂݍ��݂܂��B

  Arguments:    path:   �ǂݍ��ރt�@�C���̃p�X�B

  Returns:      �t�@�C�������݂���Ȃ�t�@�C���̓��e���ǂݍ��܂ꂽ
                �����Ŋm�ۂ����o�b�t�@�ւ̃|�C���^��Ԃ��܂��B
                ���̃|�C���^�� FS_Free �ŉ������K�v������܂��B
 *---------------------------------------------------------------------------*/
static char* LoadFile(const char* path)
{
    FSFile f;
    BOOL bSuccess;
    char* pBuffer;
    u32 fileSize;
    s32 readSize;

    FS_InitFile(&f);

    bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
    if( ! bSuccess )
    {
        return NULL;
    }

    fileSize = FS_GetFileLength(&f);
    pBuffer = (char*)OS_Alloc(fileSize + 1);
    SDK_POINTER_ASSERT(pBuffer);

    readSize = FS_ReadFile(&f, pBuffer, (s32)fileSize);
    SDK_ASSERT( readSize == fileSize );

    bSuccess = FS_CloseFile(&f);
    SDK_ASSERT( bSuccess );

    pBuffer[fileSize] = '\0';
    return pBuffer;
}

/*---------------------------------------------------------------------------*
  Name:         SaveFile

  Description:  �t�@�C�����쐬���f�[�^���������݂܂��B
                �r���̃f�B���N�g���͍쐬���܂���B

  Arguments:    path:   �쐬����t�@�C���̃p�X�B
                pData:  �������ރf�[�^�B
                size:   �������ރf�[�^�̃T�C�Y�B

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
static BOOL SaveFile(const char* path, void* pData, u32 size)
{
    FSFile f;
    BOOL bSuccess;
    FSResult fsResult;
    s32 writtenSize;

    FS_InitFile(&f);

    (void)FS_CreateFile(path, (FS_PERMIT_R|FS_PERMIT_W));
    bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_W);
    if (bSuccess == FALSE)
    {
        FSResult res = FS_GetArchiveResultCode(path);
        DrawString("Failed create file:%d\n", res);
        return FALSE;
    }
    SDK_ASSERT( bSuccess );

    fsResult = FS_SetFileLength(&f, 0);
    SDK_ASSERT( fsResult == FS_RESULT_SUCCESS );

    writtenSize = FS_WriteFile(&f, pData, (s32)size);
    SDK_ASSERT( writtenSize == size );

    bSuccess = FS_CloseFile(&f);
    SDK_ASSERT( bSuccess );
    return TRUE;
}




/*---------------------------------------------------------------------------*
  Name:         CreateTree

  Description:  �f�B���N�g���c���[���쐬���� 
                
  Arguments:    arc_name: �ΏۃA�[�J�C�u�B 

  Returns:      �Ȃ��B 
 *---------------------------------------------------------------------------*/
static void CreateTree(const char* arc_name){
    char *dir_path[] = {
        "/",
        "/testDir/", 
        "/testDir2/test/", 
    };
    char *filename = "test";
    
    u32 PATH_COUNT = 3;
    u32 FILE_COUNT = 2;
    char buf[BUF_SIZE];
    BOOL bSuccess;

    
    DrawString("Create:%s\n", arc_name);
    // �f�B���N�g���̐���
    {
        u32 i = 0, j = 0;
        for(i = 1; i < PATH_COUNT; ++i)
        {
            (void)STD_TSNPrintf(buf, BUF_SIZE, "%s%s", arc_name, dir_path[i]);

            bSuccess = FS_CreateDirectoryAuto(buf, FS_PERMIT_W | FS_PERMIT_R);
            if(!bSuccess)
            {
                DrawString("Failed Create Directory\n");
                return;
            }
            DrawString("  %s\n", buf);
        }
    }

    // �t�@�C������
    {
        u32 i = 0, j = 0;
        for(i = 0; i < PATH_COUNT; ++i)
        {
            for(j = 0; j < FILE_COUNT; ++j)
            {
                char data[BUF_SIZE];
                (void)STD_TSNPrintf(buf, BUF_SIZE, "%s%s%s%d", arc_name, dir_path[i], filename, j);
                (void)SaveFile(buf, GetTestData(data, BUF_SIZE), BUF_SIZE);
                DrawString("  %s\n", buf);
            }
        }
    }
    DrawString("\n");    
}


/*---------------------------------------------------------------------------*
  Name:         DeleteData

  Description:  �f�B���N�g�����̃t�@�C���E�f�B���N�g����S�č폜����
                
  Arguments:    path: �Ώۃf�B���N�g���̃p�X�B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void DeleteData(char *path)
{
    FSFile fp;
    FSDirectoryEntryInfo entry;
    char buf[BUF_SIZE];
    BOOL bSuccess;
    BOOL bDeleted = FALSE;
    
    DrawString("Delete:%s\n", path);
    FS_InitFile(&fp);
    bSuccess = FS_OpenDirectory(&fp, path, FS_PERMIT_W | FS_PERMIT_R);
    if(!bSuccess)
    {
        DrawString("Failed Open Directory\n");
        return;
    }

    while(FS_ReadDirectory(&fp, &entry))
    {
        if(!STD_StrCmp(".", entry.longname) || !STD_StrCmp("..", entry.longname))
            continue;

        (void)STD_TSNPrintf(buf, BUF_SIZE, "%s/%s", path, entry.longname);
        if(entry.attributes & FS_ATTRIBUTE_IS_DIRECTORY)
        {
            // �f�B���N�g���̏ꍇ
            bSuccess = FS_DeleteDirectoryAuto(buf);
        }
        else
        {
            // �t�@�C���̏ꍇ
            bSuccess = FS_DeleteFile(buf);
        }
        if(!bSuccess)
        {
            DrawString("Failed Delete %s\n", buf);
            continue;
        }
        else
        {
            DrawString("  %s\n", buf);
            bDeleted = TRUE;
        }
    }
    
    if(!bDeleted)
    {
        DrawString("No File\n");
    }
    
    bSuccess = FS_CloseDirectory(&fp);
    SDK_ASSERT(bSuccess);
    DrawString("\n");
}

/*---------------------------------------------------------------------------*
  Name:         GetTestData

  Description:  �e�X�g�t�@�C���ɏ������ރf�[�^�̍쐬�B
                
  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static char* GetTestData(char* out, u32 size)
{
    RTCResult rtcResult;
    RTCDate date;
    RTCTime time;

    // �t�@�C���ɏ������ޓ��e�𐶐����܂��B
    rtcResult = RTC_GetDateTime(&date, &time);
    SDK_ASSERT( rtcResult == RTC_RESULT_SUCCESS );

    (void)STD_TSNPrintf(out, size,
        "Hello. %04d/%02d/%02d %02d:%02d:%02d\n",
        date.year + 2000,
        date.month,
        date.day,
        time.hour,
        time.minute,
        time.second );
    
    return out;
}

/*---------------------------------------------------------------------------*
  Name:         ReadData

  Description:  �f�B���N�g�����𑖍����A�ŏ��Ɍ��������t�@�C���̓��e��\������
                
  Arguments:    path: �Ώۃf�B���N�g���̃p�X�B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void ReadData(const char* arc_name)
{
    FSFile fp;
    FSDirectoryEntryInfo entry;
    
    char buf[BUF_SIZE];
    BOOL bSuccess;
    BOOL bRead = FALSE;
    
    DrawString("Read:%s\n", arc_name);
    // �f�B���N�g���̃I�[�v��
    FS_InitFile(&fp);
    bSuccess = FS_OpenDirectory(&fp, arc_name, FS_PERMIT_W | FS_PERMIT_R);
    if(!bSuccess)
    {
        DrawString("Failed Open Directory\n");
        return;
    }    

    // �f�B���N�g���𑀍삵�āA�ŏ��Ɍ��������t�@�C���̓��e��\��
    while(FS_ReadDirectory(&fp, &entry))
    {
        if(!STD_StrCmp(".", entry.longname) || !STD_StrCmp("..", entry.longname))
            continue;

        (void)STD_TSNPrintf(buf, BUF_SIZE, "%s/%s", arc_name, entry.longname);
        if(!(entry.attributes & FS_ATTRIBUTE_IS_DIRECTORY))
        {
            // �t�@�C���̏ꍇ
            char *data = NULL;
            data = LoadFile(buf);
            SDK_POINTER_ASSERT(buf);
            
            DrawString("%s\n%s\n", buf, data);
            OS_Free(data);
            bRead = TRUE;
            break;
        }
    }
    if(!bRead)
    {
        DrawString("No File\n");
    }
    
    bSuccess = FS_CloseDirectory(&fp);
    SDK_ASSERT(bSuccess);
    DrawString("\n");
}

/*---------------------------------------------------------------------------*
  Name:         DrawString

  Description:  DEMODrawString���g�����`��B
                
  Arguments:    fmt: DEMODrawString�ɏ����B
                     �������Afmt��""�̏ꍇ�A�J�[�\���ʒu��0,0�ɖ߂��B
  
  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void DrawString(const char* fmt, ...)
{
    static s32 x = 0, y = 0;
    char dst[256];
    int     ret;
    va_list va;
    va_start(va, fmt);
    ret = OS_VSPrintf(dst, fmt, va);
    va_end(va);

    if(fmt[0] == '\0')
    {
        x = y = 0;
        return;
    }    
    DEMODrawText(x, y, dst);
    {
        s32 i, max = STD_StrLen(dst) - 1;
        u32 cr = 0;
        for(i = max; i >= 0; --i)
        {
            if(dst[i] == '\n')
            {
                x = (cr == 0) ? (max - i) * 8 : x;
                cr++;
            }
        }
        y += cr * 8;
    }
    
}


/*---------------------------------------------------------------------------*
  Name:         PrintTree

  Description:  �f�B���N�g���c���[��\������
                
  Arguments:    path: ���[�g�̃p�X�B
                space: �ċA�Ăяo���p�B �ʏ��0���w��B 
  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void PrintTree(const char* path, u32 space)
{
    FSFile f;
    FSDirectoryEntryInfo entry;
    BOOL bSuccess;
    char buf[BUF_SIZE];

    FS_InitFile(&f);
    bSuccess = FS_OpenDirectory(&f, path, FS_PERMIT_R);
    if(!bSuccess)
    {
        DrawString("Failed Open Directory\n");
        return;
    }    
    while( FS_ReadDirectory(&f, &entry) )
    {
        MI_CpuFill8(buf, ' ', space);
        buf[space] = '\0';
        // ������e���w���G���g���̓X�L�b�v���܂�
        if( (STD_StrCmp(entry.longname, ".")  == 0)
         || (STD_StrCmp(entry.longname, "..") == 0) )
        {
             continue;
        }
        if( (entry.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 )
        {
            // �f�B���N�g��
            (void)STD_StrCat(buf, entry.longname);
            DrawString("%s\n", buf);
            
            // �p�X���������čċA�Ăяo��
            (void)STD_StrCpy(buf, path);
            (void)STD_StrCat(buf, "/");
            (void)STD_StrCat(buf, entry.longname);
            PrintTree(buf, space + 1);
        }
        else
        {
            // �t�@�C��
            (void)STD_StrCat(buf, entry.longname);
            DrawString("%s\n", buf);
        }
    }
    bSuccess = FS_CloseDirectory(&f);
    SDK_ASSERT( bSuccess );
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
    case OS_BOOTTYPE_ROM:   DrawString("OS_GetBootType = OS_BOOTTYPE_ROM\n"); break;
    case OS_BOOTTYPE_NAND:  DrawString("OS_GetBootType = OS_BOOTTYPE_NAND\n"); break;
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
    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 31, 1));
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

  Description:  FS �����������܂��B
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
