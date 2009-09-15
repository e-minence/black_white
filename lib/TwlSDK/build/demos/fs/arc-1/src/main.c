/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - demos - arc-1
  File:     main.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-02 #$
  $Rev: 8827 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/


#include <nitro.h>


// ���� ROM �A�[�J�C�u�\���́B
typedef struct MyRomArchive
{
    FSArchive   arc[1];
    u32         default_dma_no;
    u32         card_lock_id;

}
MyRomArchive;

// �񓯊���ROM�ǂݍ��݂����������Ƃ��̏����B
static void MyRom_OnReadDone(void *arc)
{
    // �A�[�J�C�u�֊����ʒm�B
    FS_NotifyArchiveAsyncEnd((FSArchive *)arc, FS_RESULT_SUCCESS);
}

// FS����A�[�J�C�u�ւ̃��[�h�A�N�Z�X�R�[���o�b�N�B
static FSResult MyRom_ReadCallback(FSArchive *arc, void *dst, u32 src, u32 len)
{
    MyRomArchive *const p_rom = (MyRomArchive *)arc;
    CARD_ReadRomAsync(p_rom->default_dma_no,
                      (const void *)(FS_GetArchiveBase(arc) + src), dst, len,
                      MyRom_OnReadDone, arc);
    return FS_RESULT_PROC_ASYNC;
}

// FS����A�[�J�C�u�ւ̃��C�g�R�[���o�b�N�B
// ���[�U�v���V�[�W����FS_RESULT_UNSUPPORTED��Ԃ��̂ŌĂ΂�Ȃ��B
static FSResult MyRom_WriteDummyCallback(FSArchive *arc, const void *src, u32 dst, u32 len)
{
    (void)arc;
    (void)src;
    (void)dst;
    (void)len;
    return FS_RESULT_FAILURE;
}

// ���[�U�v���V�[�W���B
// �ŏ��̃R�}���h�J�n�O����Ō�̃R�}���h������܂�ROM�����b�N�B
// ���C�g����̓T�|�[�g�O�Ƃ��ĉ�������B
// ����ȊO�̓f�t�H���g�̓���B
static FSResult MyRom_ArchiveProc(FSFile *file, FSCommandType cmd)
{
    MyRomArchive *const p_rom = (MyRomArchive *) FS_GetAttachedArchive(file);
    switch (cmd)
    {
    case FS_COMMAND_ACTIVATE:
        CARD_LockRom((u16)p_rom->card_lock_id);
        return FS_RESULT_SUCCESS;
    case FS_COMMAND_IDLE:
        CARD_UnlockRom((u16)p_rom->card_lock_id);
        return FS_RESULT_SUCCESS;
    case FS_COMMAND_WRITEFILE:
        return FS_RESULT_UNSUPPORTED;
    default:
        return FS_RESULT_PROC_UNKNOWN;
    }
}

// DMA �̎w�肨��уA�[�J�C�u�̓o�^ / ���[�h�B
static void MyRom_Create(MyRomArchive *p_rom, const char *name,
                         u32 base, const CARDRomRegion *fnt, const CARDRomRegion *fat)
{
    FS_InitArchive(p_rom->arc);
    p_rom->default_dma_no = FS_DMA_NOT_USE;
    p_rom->card_lock_id = (u32)OS_GetLockID();
    if (!FS_RegisterArchiveName(p_rom->arc, name, (u32)STD_GetStringLength(name)))
    {
        OS_TPanic("error! FS_RegisterArchiveName(%s) failed.\n", name);
    }
    else
    {
        FS_SetArchiveProc(p_rom->arc, MyRom_ArchiveProc,
                          FS_ARCHIVE_PROC_WRITEFILE | FS_ARCHIVE_PROC_ACTIVATE | FS_ARCHIVE_PROC_IDLE);
        if (!FS_LoadArchive(p_rom->arc, base,
                            fat->offset, fat->length, fnt->offset, fnt->length,
                            MyRom_ReadCallback, MyRom_WriteDummyCallback))
        {
            OS_TPanic("error! FS_LoadArchive() failed.\n");
        }
    }
}

// ROM�A�[�J�C�u�̃f�B���N�g����񋓁B
static void DumpRomDirectorySub(int tab, FSDirEntry *pe)
{
    FSFile  d;
    FS_InitFile(&d);
    OS_TPrintf("%*s%s/\n", tab, "", pe->name);
    if (FS_SeekDir(&d, &pe->dir_id))
    {
        tab += 4;
        while (FS_ReadDir(&d, pe))
        {
            if (pe->is_directory)
            {
                DumpRomDirectorySub(tab, pe);
            }
            else
            {
                OS_Printf("%*s%s\n", tab, "", pe->name);
            }
        }
    }
}
static void DumpRomDir(const char *path)
{
    FSDirEntry  entry;
    FSFile      dir;
    FS_InitFile(&dir);
    (void)FS_ChangeDir(path);
    (void)FS_FindDir(&dir, "");
    entry.name[0] = '\0';
    (void)FS_TellDir(&dir, &entry.dir_id);
    DumpRomDirectorySub(0, &entry);
}

void NitroMain(void)
{
    static MyRomArchive myrom_dup;
    static MyRomArchive myrom_sub;

    // OS�ƃ������A���P�[�^���������B
    OS_Init();
    {
        OSHeapHandle hh;
        void   *tmp;
        tmp = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
        OS_SetArenaLo(OS_ARENA_MAIN, tmp);
        hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
        if (hh < 0)
        {
            OS_TPanic("ARM9: Fail to create heap...\n");
        }
        (void)OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
    }
    (void)OS_EnableIrq();
    // FS���������B
    FS_Init(3);
    {
        u32     need_size = FS_GetTableSize();
        void   *p_table = OS_Alloc(need_size);
        SDK_ASSERT(p_table != NULL);
        (void)FS_LoadTable(p_table, need_size);
    }

    OS_TPrintf("\n"
               "++++++++++++++++++++++++++++++++++++++++\n"
               "test 1 : query default \"rom\" directories ... \n\n");

    DumpRomDir("rom:/");

    OS_TPrintf("\n"
               "++++++++++++++++++++++++++++++++++++++++\n"
               "test 2 : query duplication archive \"dup\" directories ... \n\n");
    {
        const u32 base = 0;
        const CARDRomRegion *fnt = CARD_GetRomRegionFNT();
        const CARDRomRegion *fat = CARD_GetRomRegionFAT();

        MyRom_Create(&myrom_dup, "dup", base, fnt, fat);
        DumpRomDir("dup:/");
    }

    OS_TPrintf("\n"
               "++++++++++++++++++++++++++++++++++++++++\n"
               "test 3 : query sub-program archive \"sub\" directories ... \n\n");
    {
        FSFile  sub_binary;
        FS_InitFile(&sub_binary);
        if (!FS_OpenFileEx(&sub_binary, "rom:/main.srl", FS_FILEMODE_R))
        {
            OS_Panic("failed to open sub-program binary");
        }
        else
        {
            const u32 base = FS_GetFileImageTop(&sub_binary);
            CARDRomHeader   header;
            (void)FS_ReadFile(&sub_binary, &header, sizeof(header));
            (void)FS_CloseFile(&sub_binary);

            MyRom_Create(&myrom_sub, "sub", base, &header.fnt, &header.fat);
            DumpRomDir("sub:/");
        }
    }

    OS_TPrintf("\n" "++++++++++++++++++++++++++++++++++++++++\n" "end\n\n");
    OS_Terminate();
}
