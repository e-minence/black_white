/*---------------------------------------------
  Utility program codes for particle demo .
  system.
 
 $Log: ptcl_demo_system.c,v $
 Revision 1.2  2004/12/27 04:32:20  okane_shinji
 (none)

 Revision 1.1  2004/09/15 01:23:09  okane_shinji
 �����o�^

 
 *---------------------------------------------*/

#include<nnsys.h>
#include"ptcl_demo.h"

#define DEFAULT_DMA_NUMBER      MI_DMA_MAX_NUM

// �m�ۂ���q�[�v�T�C�Y
#define   MAIN_HEAP_SIZE    0x64000

/*-----------------------------------------------------------------
  Name       : PtclDemo_VBlankIntr( void )
  Description: V�u�����N
  Returns    : �Ȃ�
 *-----------------------------------------------------------------*/
static void PtclDemo_VBlankIntr( void )
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*-----------------------------------------------------------------
  Name       : PtclDemo_InitSystem( void )
  Description: �V�X�e�������������܂�
  Returns    : �Ȃ�
 *-----------------------------------------------------------------*/
void PtclDemo_InitSystem( void )
{
    OS_Init();
    FX_Init();
    GX_SetPower(GX_POWER_ALL);
    GX_Init();
    OS_InitTick();
    GX_DispOff();
    GXS_DispOff();

    OS_SetIrqFunction(OS_IE_V_BLANK, PtclDemo_VBlankIntr);

    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr(TRUE);

    FS_Init( FS_DMA_NOT_USE ); // Irq�֌W�̊֐��̌��Ɏ����Ă��Ȃ��Ƃ����Ȃ��͗l
    (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);

}

/*-----------------------------------------------------------------
  Name       : PtclDemo_InitVRAM( void )
  Description: VRAM�����������܂�
  Returns    : �Ȃ�
 *-----------------------------------------------------------------*/
void PtclDemo_InitVRAM( void )
{
    GX_SetBankForLCDC( GX_VRAM_LCDC_ALL );

    MI_CpuClearFast( (void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE );
    
    (void)GX_DisableBankForLCDC();

    MI_CpuFillFast ( (void*)HW_OAM    , 192, HW_OAM_SIZE    );
    MI_CpuFillFast ( (void*)HW_DB_OAM , 192, HW_DB_OAM_SIZE );

    MI_CpuClearFast( (void*)HW_PLTT   , HW_PLTT_SIZE    );
    MI_CpuClearFast( (void*)HW_DB_PLTT, HW_DB_PLTT_SIZE );
}

/*-----------------------------------------------------------------
  Name       : PtclDemo_InitMemory( void )
  Description: �����������������܂�
  Returns    : �Ȃ�
 *-----------------------------------------------------------------*/

void PtclDemo_InitMemory( void )
{
    void*          heapStart;
    void*          nstart;
    OSHeapHandle   handle;

    //---- MainRAM �A���[�i�ɑ΂��� ���������蓖�ăV�X�e��������
    nstart = OS_InitAlloc( OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 2 );
    OS_SetMainArenaLo( nstart );

    //---- �A���[�i����q�[�v�p�̈�m��
    heapStart = OS_AllocFromMainArenaLo( MAIN_HEAP_SIZE, 32 );

    //---- �q�[�v�쐬
    handle = OS_CreateHeap( OS_ARENA_MAIN, heapStart, (void*)( (u32)heapStart + MAIN_HEAP_SIZE ) );

    //---- �J�����g�q�[�v�ݒ�
    (void)OS_SetCurrentHeap( OS_ARENA_MAIN, handle );

}

/*-----------------------------------------------------------------
  Name       : PtclDemo_LoadFile( const char *path )
  Description: �t�@�C����ǂݍ���Ń������Ƀ}�b�s���O���܂��B
  Returns    : �}�b�s���O���ꂽ�������̃A�h���X�B
 *-----------------------------------------------------------------*/
void* PtclDemo_LoadFile( const char *path )
{

    FSFile file;
    void*  memory;

    FS_InitFile( &file );
    if ( FS_OpenFile( &file, path ) )
    {
        u32 fileSize = FS_GetLength( &file );

        memory = OS_Alloc( fileSize );
        if ( memory == NULL )
        {
            OS_Printf( "no enough memory.\n" ) ;
        }
        else
        {
            if ( FS_ReadFile( &file, memory, fileSize ) != fileSize )   // �t�@�C���T�C�Y���ǂݍ��߂Ă��Ȃ��ꍇ
            {
                OS_Free( memory );
                memory = NULL;
                OS_Printf( "file reading failed.\n" ) ;
            }
        }
        ( void )FS_CloseFile( &file ) ;
    }
    return memory;

}








// end of file