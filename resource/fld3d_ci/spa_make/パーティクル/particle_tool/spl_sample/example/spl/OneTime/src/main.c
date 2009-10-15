/*-------------------------------------------
 * SPL library �T���v�� "OneTime"
 *
 * ���ŃG�~�b�^�̕�������o�O�ɕύX����T���v���ł�
 *
 * �G�~�b�^�̕��o������ύX����R�[���o�b�N�֐����쐬���A
 * SPL_CreateWithInitialize�ō쐬���܂��B
 *
 * (c)2004 Nintendo
 * 
 * 
 * $Log: main.c,v $
 * Revision 1.3  2005/02/18 08:31:28  okane_shinji
 * VRAM�����ѹ��ˤȤ�ʤ�����
 *
 * Revision 1.2  2004/12/28 02:22:56  okane_shinji
 * (none)
 *
 * Revision 1.1  2004/12/28 02:00:21  okane_shinji
 * (none)
 *
 * Revision 1.2  2004/12/02 07:30:12  okane_shinji
 * ���o�^
 *
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

// �p�[�e�B�N���֌W
static SPLManager* spl_manager = NULL ;
static SPLEmitter* spl_emitter = NULL ;
static VecFx32 init_pos = {0,0,0} ;
void*  resPtcl ;

/*---------------------------------------------------
 * �쐬���ꂽ����̃G�~�b�^�������Ƃ��ēn�����
 *-------------------------------------------------*/
 
static void CallbackFunc( SPLEmitter* pEmitter )
{
    VecFx16 vec = {FX16_ONE,0,0} ;
    
    // ������ύX����
    SPL_SetEmitterAxis( pEmitter, &vec ); 
}


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

    // VRAM�}�l�[�W�����Z�b�g
    NNS_GfdInitFrmTexVramManager(3, TRUE);
    NNS_GfdInitFrmPlttVramManager(0x8000, TRUE);

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

    // �G�~�b�^���쐬
    (void)SPL_CreateWithInitialize( spl_manager, 0, CallbackFunc ) ;

}

/*---------------------------------------------------
 * �`��
 *-------------------------------------------------*/
static void draw()
{
    MtxFx43 camera ;

    PtclDemo_Lookat( 0, 0, FX32_ONE * 50, 0, 0, 0, 0, FX32_ONE, 0, &camera ) ;

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