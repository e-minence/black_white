/*-------------------------------------------
 * SPL library �T���v�� "FreeRes"
 *
 * �������ɓǂݍ��񂾃��\�[�X�̂���
 * �e�N�X�`�����������������T���v���ł��B
 * �e�N�X�`����VRAM�ɓǂݍ��܂ꂽ���
 * �g�p����Ȃ��̂ŁA���̂悤�Ȃ��Ƃ��\�ł��B
 *
 * (c)2004 Nintendo
 * 
 * 
 * $Log: main.c,v $
 * Revision 1.3  2005/02/18 08:31:28  okane_shinji
 * VRAM�����ѹ��ˤȤ�ʤ�����
 *
 * Revision 1.2  2004/12/02 07:59:50  okane_shinji
 * ver110�Ή�
 *
 * Revision 1.1  2004/09/15 01:28:53  okane_shinji
 * �����o�^
 *
 *
 *
 *-------------------------------------------*/



#include<nitro.h>
#include<nnsys/fnd.h>
#include<nnsys/gfd.h>
//#include<nnsys/g3d.h>

#include"ptcl_demo.h"
#include"spl.h"

// �p�[�e�B�N���֌W
static SPLManager* spl_manager = NULL ;
static SPLEmitter* spl_emitter = NULL ;
static VecFx32 init_pos = {0,0,0} ;
void*  resPtcl ;

// �q�[�v�p�̃�����
#define HeapBufElementNum (4096/sizeof(u32))
static u32 sHeapBuf[HeapBufElementNum];

// �q�[�v
NNSFndHeapHandle hFrmHeap;

/*---------------------------------------------------
 * �t���[���q�[�v�Ƀt�@�C�����}�b�s���O����
 *-------------------------------------------------*/
static void* loadfile_to_frameheap( const char *path )
{
    FSFile file;
    void*  memory;

    FS_InitFile( &file );
    if ( FS_OpenFile( &file, path ) )
    {
        u32 fileSize = FS_GetLength( &file );

        memory = NNS_FndAllocFromFrmHeap( hFrmHeap, fileSize );
        if ( memory == NULL )
        {
            OS_Printf( "no enough memory.\n" ) ;
        }
        else
        {
            if ( FS_ReadFile( &file, memory, fileSize ) != fileSize )   // �t�@�C���T�C�Y���ǂݍ��߂Ă��Ȃ��ꍇ
            {
                memory = NULL;
                OS_Printf( "file reading failed.\n" ) ;
            }
        }
        ( void )FS_CloseFile( &file ) ;
    }
    return memory;
}

/*---------------------------------------------------
 * ������
 *-------------------------------------------------*/
static void init()
{
    int i ;
    u32 tex_size ;
    u32* p ;

    // �e�평����
    PtclDemo_InitSystem() ;
    PtclDemo_InitVRAM() ;
    PtclDemo_InitMemory() ;
    PtclDemo_InitScreen() ;
    // �t���[���q�[�v��������
    hFrmHeap = NNS_FndCreateFrmHeap(  sHeapBuf, sizeof( sHeapBuf )  ) ;

    OS_Printf( "�m�ے���̃t���[���q�[�v : 0x%x �` 0x%x ( size : %d ) ( �� : %d ) \n" , NNS_FndGetHeapStartAddress( hFrmHeap ) , NNS_FndGetHeapEndAddress( hFrmHeap ), (u32)NNS_FndGetHeapEndAddress( hFrmHeap ) - (u32)NNS_FndGetHeapStartAddress( hFrmHeap ), NNS_FndGetAllocatableSizeForFrmHeap( hFrmHeap ) ) ;

    // VRAM�}�l�[�W�����Z�b�g
    NNS_GfdInitFrmTexVramManager(3, TRUE);
    NNS_GfdInitFrmPlttVramManager(0x8000, TRUE);

    //////////////////////////////////////////////////
    // SPL������

    // �p�[�e�B�N���}�l�[�W����������
    spl_manager = SPL_Init( OS_AllocFromMain, 1, 100, 0, 1, 30 );

    // �p�[�e�B�N�����\�[�X��ǂݍ���
    resPtcl = loadfile_to_frameheap( "data/output.spa" ) ;
    SPL_Load( spl_manager, resPtcl );
    DC_StoreAll();

    // �g���Ă��Ȃ��̈���J��
    (void)NNS_FndAdjustFrmHeap( hFrmHeap ) ;
    OS_Printf( "�m�ے���̃t���[���q�[�v : 0x%x �` 0x%x ( size : %d ) ( �� : %d ) \n" , NNS_FndGetHeapStartAddress( hFrmHeap ) , NNS_FndGetHeapEndAddress( hFrmHeap ), (u32)NNS_FndGetHeapEndAddress( hFrmHeap ) - (u32)NNS_FndGetHeapStartAddress( hFrmHeap ), NNS_FndGetAllocatableSizeForFrmHeap( hFrmHeap )  ) ;

    // �e�N�X�`����ǂݍ���
    (void)SPL_LoadTexByVRAMManager( spl_manager ) ;
    (void)SPL_LoadTexPlttByVRAMManager( spl_manager ) ;

    // �G�~�b�^���쐬
    spl_emitter = SPL_Create( spl_manager, 0, &init_pos );

    //////////////////////////////////////////////////
    // �������Ƀ}�b�s���O���ꂽ���\�[�X�̂����A
    // �e�N�X�`���̈������

    // �t���[���q�[�v�̂����e�N�X�`���g�p������؂���B
    (void)NNS_FndResizeForMBlockFrmHeap( hFrmHeap, sHeapBuf, SPL_GetTexOffsetOnResource( resPtcl ) + 48 ) ;

    // ���\�[�X�̃e�N�X�`���̈������������i���̗p�r�Ɏg�p����j�B
    // p�͂��Ƃ��ƃe�N�X�`���̂������̈�ł��邪�A
    // ���̑��̗p�r�Ɏg�p�ł���悤�Ȃ�B
    tex_size = SPL_GetTexSizeOnResource( resPtcl ) ; 
    p        = (u32*)NNS_FndAllocFromFrmHeap( hFrmHeap, tex_size );
    for( i = 0 ; i < tex_size / 4 ; i = i + 1 )
    {
        // ���̑��̗p�r�B�B
        p[i] = 0 ;
    }


}

/*---------------------------------------------------
 * �`��
 *-------------------------------------------------*/
static void draw()
{
    MtxFx43 camera ;

    PtclDemo_Lookat( 0, 0, FX32_ONE * 10, 0, 0, 0, 0, FX32_ONE, 0, &camera ) ;

    // ���W��������
    PtclDemo_DrawAxis() ;

    // �p�[�e�B�N���`��
    SPL_Draw( spl_manager, &camera );
}

/*---------------------------------------------------
 * �v�Z
 *-------------------------------------------------*/
static void calc()
{
    SPL_Calc( spl_manager ) ;
}

/*---------------------------------------------------
 * ���C��
 *-------------------------------------------------*/
void NitroMain()
{
    init() ;

    while(1)
    {
        G3X_Reset();

        draw();

        calc() ;

        G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
        SVC_WaitVBlankIntr(); 
    }
}



// end of file