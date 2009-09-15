/*---------------------------------------------------------------------------*
  Project:  TwlSDK - nandApp - demos - SubBanner
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

#define MENU_ELEMENT_NUM 4  // ���j���[�̍��ڐ�
#define MENU_TEXT_SIZE 8

#define COLOR_WHITE GX_RGBA(31, 31, 31, 1)
#define COLOR_RED   GX_RGBA(31,  0,  0, 1)
#define COLOR_GREEN GX_RGBA( 0, 31,  0, 1)
#define COLOR_BLUE  GX_RGBA( 0,  0, 31, 1)
#define COLOR_BLACK GX_RGBA( 0,  0,  0, 1)

// ���j���[�v�f���W
typedef struct MenuPos
{
    BOOL   enable;
    int    x;
    int    y;
}MenuPos;

// ���j���[�\���p�����[�^�\����
typedef struct MenuParam
{
    int      num;
    GXRgb    normal_color;
    GXRgb    select_color;
    GXRgb    disable_color;
    u16      padding;
    MenuPos *pos;
    const char **str_elem;
}MenuParam;

static const char *pStrMenu[ MENU_ELEMENT_NUM ] = 
{
    "write sub banner file",
    "edit sub banner file",
    "delete sub banner file",
    "return to launcher",
};

static MenuPos menuPos[] =
{
    { TRUE,  3,   8 },
    { TRUE,  3,   9 },
    { TRUE,  3,   10 },
    { TRUE,  3,   11 },
};

static const MenuParam menuParam =
{
    MENU_ELEMENT_NUM,
    COLOR_BLACK,
    COLOR_GREEN,
    COLOR_RED,
    0,
    &menuPos[ 0 ],
    (const char **)&pStrMenu,
};

static void PrintBootType();
static void InitDEMOSystem();
static void InitInteruptSystem();
static void InitAllocSystem();
static void InitFileSystem();

#define MESSAGE_SIZE 50

static char message[MESSAGE_SIZE]="";

/*---------------------------------------------------------------------------*
  Name:         EditSubBanner

  Description:  �T�u�o�i�[��RGB�F�����ւ��A���������Ƀt���b�v�����܂�

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static BOOL EditSubBanner(NASubBanner* banner)
{
    int i;
    NASubBannerAnime* anime = &banner->anime;
    
    if (!NA_LoadSubBanner(banner))
    {
        return FALSE;
    }

    // �p���b�g�̃f�[�^��RGB�����[�e�[�V����������
    for (i = 0; i < NA_SUBBANNER_PALETTE_NUM; i++)
    {
        anime->pltt[0][i] = (GXRgb)((GX_RGB_R_MASK & anime->pltt[0][i]) << 5 |  // R->G
                                    (GX_RGB_G_MASK & anime->pltt[0][i]) << 5 |  // G->B
                                    (GX_RGB_B_MASK & anime->pltt[0][i]) >> 10); // B->R
    }
    anime->control[0].frameInfo.flipH = !anime->control[0].frameInfo.flipH;

    NA_MakeSubBannerHeader(banner);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  ���C���֐��ł��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
void TwlMain(void)
{
    int cursor = 0;
    int i;

    OS_Init();
    InitInteruptSystem();
    InitFileSystem();
    InitAllocSystem();
    InitDEMOSystem();

    // print usage
    PrintBootType();
    OS_TPrintf("=================================\n");
    OS_TPrintf("USAGE: SubBanner demo\n");
    OS_TPrintf(" UP DOWN : select menu\n");
    OS_TPrintf(" A       : execute menu\n");
    OS_TPrintf("=================================\n");

    for (;;)
    {
        DEMOReadKey();
        if( DEMO_IS_TRIG(PAD_KEY_DOWN) )
        {   // �J�[�\���̈ړ�
            if( ++cursor == MENU_ELEMENT_NUM )
            {
                cursor=0;
            }
        }
        if( DEMO_IS_TRIG(PAD_KEY_UP) )
        {
            if( --cursor & 0x80 )
            {
                cursor=MENU_ELEMENT_NUM - 1;
            }
        }

        // ���j���[�̕`��
        {
            const MenuParam *param = &menuParam;

            DEMOFillRect(0, 0, 256, 192, COLOR_WHITE);
            DEMOSetBitmapGroundColor(COLOR_WHITE);
            DEMOSetBitmapTextColor( COLOR_BLUE );
            DEMODrawText( 1 * MENU_TEXT_SIZE, 0 * MENU_TEXT_SIZE, "SubBanner");

            for( i = 0; i < param->num; i++)
            {
                DEMOSetBitmapTextColor( i == cursor ? param->select_color : param->normal_color );
                DEMODrawText( param->pos[i].x * MENU_TEXT_SIZE, param->pos[i].y * MENU_TEXT_SIZE,
                            ( i == cursor ? "=>%s" : "  %s" ), param->str_elem[i] );
            }

            DEMOSetBitmapTextColor( COLOR_RED );
            DEMODrawText( 1 * MENU_TEXT_SIZE, 3 * MENU_TEXT_SIZE, message);
        }

        if( DEMO_IS_TRIG(PAD_BUTTON_A) )
        {    // ���j���[���ڂւ̕���
             switch( cursor )
             {
                 NASubBanner subBanner;
                 FSFile file[1];
                 s32 readLen;
               case 0:
                 //subbanner��������
                 FS_InitFile(file);
                 if (FS_OpenFileEx(file, "rom:/sub_banner.bnr", FS_FILEMODE_R) )
                 {
                     readLen = FS_ReadFile(file, &subBanner, sizeof(NASubBanner));
                     (void)FS_CloseFile(file);
                     if( readLen == sizeof(NASubBanner) )
                     {
                         // ����
                         if ( NA_SaveSubBanner( &subBanner ) )
                         {
                             (void)STD_CopyLString( message, "write succeed.", MESSAGE_SIZE );
                         }
                         else
                         {
                             (void)STD_CopyLString( message, "write failed.", MESSAGE_SIZE );
                         }
                     }
                     else
                     {
                         (void)STD_CopyLString( message, "read rom failed.", MESSAGE_SIZE );
                     }
                 }
                 else
                 {
                     (void)STD_CopyLString( message, "open rom failed.", MESSAGE_SIZE );
                 }
                 break;
               case 1:
                 if (EditSubBanner( &subBanner ))
                 {   // ����
                     if ( NA_SaveSubBanner( &subBanner ) )
                     {
                         (void)STD_CopyLString( message, "edit succeed.", MESSAGE_SIZE );
                     }
                     else
                     {
                         (void)STD_CopyLString( message, "edit failed.", MESSAGE_SIZE );
                     }
                 }
                 else
                 {   // ���s
                     (void)STD_CopyLString( message, "load failed.", MESSAGE_SIZE );
                 }
                 break;
               case 2:
                 //subbanner����
                 if ( NA_DeleteSubBanner(&subBanner) )
                 {
                     (void)STD_CopyLString( message, "delete succeed", MESSAGE_SIZE );
                 }
                 else
                 {
                     (void)STD_CopyLString( message, "delete failed", MESSAGE_SIZE );
                 }
                 break;
               case 3:
                 (void)OS_JumpToSystemMenu();
                 //�ċN��
                 break;
             }
        }
        DEMO_DrawFlip();
        OS_WaitVBlankIntr();
    }
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
      case OS_BOOTTYPE_ROM:   OS_TPrintf("OS_GetBootType:OS_BOOTTYPE_ROM\n"); break;
      case OS_BOOTTYPE_NAND:  OS_TPrintf("OS_GetBootType:OS_BOOTTYPE_NAND\n"); break;
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
