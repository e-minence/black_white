/*-------------------------------------------
 * SPL library �T���v�� "Simple"
 *
 * G3D���f����SPL�ɂ��p�[�e�B�N����
 * �������ĕ\������T���v���ł��B
 *
 * (c)2004 Nintendo
 * 
 * 
 * $Log: main.c,v $
 * Revision 1.6  2005/02/18 08:31:28  okane_shinji
 * VRAM�����ѹ��ˤȤ�ʤ�����
 *
 * Revision 1.5  2005/02/08 07:04:54  okane_shinji
 * (none)
 *
 * Revision 1.4  2004/12/27 04:32:20  okane_shinji
 * (none)
 *
 * Revision 1.3  2004/12/21 02:01:25  okane_shinji
 * (none)
 *
 * Revision 1.2  2004/12/02 08:00:07  okane_shinji
 * (none)
 *
 * Revision 1.1  2004/09/15 01:26:27  okane_shinji
 * �����o�^
 *
 *
 *
 *-------------------------------------------*/



#include<nitro.h>
#include<nnsys/fnd.h>
#include<nnsys/gfd.h>
#include<nnsys/g3d.h>

#include"ptcl_demo.h"
#include"spl.h"


// ���f���֌W
static NNSG3dRenderObj      object         ;
static NNSG3dResMdl*        model   = NULL ;
static NNSG3dResFileHeader* resFile = NULL ;

// �p�[�e�B�N���֌W
static SPLManager* spl_manager = NULL ;
static SPLEmitter* spl_emitter = NULL ;
static VecFx32 init_pos = {0,0,0} ;
void*  resPtcl ;

// �e�N�X�`���ǂݍ��݂����O�ł����Ȃ��ꍇ�̃R�[���o�b�N�֐���`��
#if 0
/*---------------------------------------------------
 * �e�N�X�`��VRAM�A�h���X��Ԃ����߂̃R�[���o�b�N�֐�
 *-------------------------------------------------*/
static u32 sAllocTex( u32 size, BOOL is4x4comp )
{
    NNSGfdTexKey key = NNS_GfdAllocTexVram( size, is4x4comp, 0 );
    return NNS_GfdGetTexKeyAddr( key ) ;
}

/*---------------------------------------------------
 * �e�N�X�`���p���b�gVRAM�A�h���X��Ԃ����߂̃R�[���o�b�N�֐�
 *-------------------------------------------------*/
static u32 sAllocTexPalette(  u32 size, BOOL is4x4comp )
{
    NNSGfdPlttKey key = NNS_GfdAllocPlttVram( size, is4x4comp, 0 );
    return NNS_GfdGetPlttKeyAddr( key ) ;
}
#endif



/*---------------------------------------------------
 * ������
 *-------------------------------------------------*/
static void init()
{
    // �e�평����
    PtclDemo_InitSystem() ;
    PtclDemo_InitVRAM() ;
    PtclDemo_InitMemory() ;
    PtclDemo_InitScreen() ;

    //////////////////////////////////////////////////
    // G3D�������ƃ��f���ǂݍ���

    // G3D������
    NNS_G3dInit();

    // VRAM�}�l�[�W�����Z�b�g
    NNS_GfdInitFrmTexVramManager(3, TRUE);
    NNS_GfdInitFrmPlttVramManager(0x8000, TRUE);
    
    // ���f���ǂݍ���
    resFile = PtclDemo_LoadFile( "data/mario.nsbmd" ) ;
    SDK_ASSERTMSG( resFile, "loading resource is failed." ); 

    // ���\�[�X�t�@�C����������
    if( !NNS_G3dResDefaultSetup(resFile) )
    {
        OS_Panic( "NNS_G3dResDefaultSetup failed" ) ;
    }
        
    // ���f���̎擾
    model = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(resFile), 0);
    SDK_ASSERTMSG( model, "getting model is failed." ); 

    // ���f�����֘A�t����
    NNS_G3dRenderObjInit(&object, model);

    // �������e�ƃJ�������Z�b�g
    {
        VecFx32 pos = { 0, FX32_ONE * 10, FX32_ONE * 50 } ;
        VecFx32 up  = { 0, FX32_ONE     , 0             } ;
        VecFx32 at  = { 0, 0            , 0             } ;
        
        NNS_G3dGlbPerspective( FX32_SIN30, FX32_COS30, FX32_ONE * 4 / 3, FX32_ONE, FX32_ONE * 5000 );
        NNS_G3dGlbLookAt( &pos, &up, &at );
    }


    //////////////////////////////////////////////////
    // SPL������

    // �p�[�e�B�N���}�l�[�W����������
    spl_manager = SPL_Init( OS_AllocFromMain, 1, 100, 0, 1, 30 );

    // �p�[�e�B�N�����\�[�X��ǂݍ���
    resPtcl = PtclDemo_LoadFile( "data/output.spa" ) ;
    SPL_Load( spl_manager, resPtcl );

    // �e�N�X�`����ǂݍ���
    (void)SPL_LoadTexByVRAMManager( spl_manager ) ;
    (void)SPL_LoadTexPlttByVRAMManager( spl_manager ) ;

    #if 0 
    // �R�[���o�b�N�֐���p���ăe�N�X�`���ǂݍ��݂����O�ł����Ȃ��ꍇ
    SPL_LoadTexByCallbackFunction( spl_manager, allocTex ) ;
    SPL_LoadTexPlttByCallbackFunction( spl_manager, sAllocTexPalette );
    #endif


    // �G�~�b�^���쐬
    spl_emitter = SPL_Create( spl_manager, 0, &init_pos );


}

/*---------------------------------------------------
 * �`��
 *-------------------------------------------------*/
static void draw()
{

        // �O���[�o���X�e�[�g��K�p
    NNS_G3dGlbFlush();
    
    // ���W��������
    PtclDemo_DrawAxis() ;


    // �p�[�e�B�N���`��
    SPL_Draw( spl_manager, NNS_G3dGlbGetSrtCameraMtx() );

    // �ӂ����уO���[�o���X�e�[�g��K�p���Ă��烂�f����`��
    NNS_G3dGlbFlush();
    NNS_G3dDraw(&object);
    
    NNS_G3dGlbFlush();
    
    
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