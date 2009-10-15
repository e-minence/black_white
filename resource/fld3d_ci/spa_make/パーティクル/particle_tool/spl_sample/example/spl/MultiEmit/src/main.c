/*-------------------------------------------
 * SPL library �T���v�� "MultiEmit"
 *
 * ��̃G�~�b�^�œ����ɕ����n�_����p�[�e�B�N����
 * ���o����T���v���ł��B 
 *
 * �G�~�b�^�𐶐�������A�����I�ȕ��o���~���܂��B
 * ����SPL_Calc�̑O��SPL_Emit��������SPL_EmitAt���Ăяo���܂��B
 *
 * (c)2004 Nintendo
 * 
 * 
 * $Log: main.c,v $
 * Revision 1.6  2005/02/18 08:31:21  okane_shinji
 * VRAM�����ѹ��ˤȤ�ʤ�����
 *
 * Revision 1.3  2004/12/02 08:01:22  okane_shinji
 * ver110�Ή�
 *
 * Revision 1.1  2004/09/15 01:29:53  okane_shinji
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

// �p�[�e�B�N���֌W
static SPLManager* spl_manager = NULL ;
static SPLEmitter* spl_emitter = NULL ;
static VecFx32 init_pos = {0,0,0} ;
void*  resPtcl ;

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
    spl_manager = SPL_Init( OS_AllocFromMain, 2, 100, 0, 1, 30 );

    // �p�[�e�B�N�����\�[�X��ǂݍ���
    resPtcl = PtclDemo_LoadFile( "data/output.spa" ) ;
    SPL_Load( spl_manager, resPtcl );

    // �e�N�X�`����ǂݍ���
    (void)SPL_LoadTexByVRAMManager( spl_manager ) ;
    (void)SPL_LoadTexPlttByVRAMManager( spl_manager ) ;

    // �G�~�b�^���쐬
    spl_emitter = SPL_Create( spl_manager, 0, &init_pos );

    // �����I�ȃp�[�e�B�N�����o���~����
    SPL_StopEmission( spl_emitter ); 

}

/*---------------------------------------------------
 * �`��
 *-------------------------------------------------*/
static void draw()
{
    MtxFx43 camera ;

    PtclDemo_Lookat( 0, 0, FX32_ONE * 20, 0, 0, 0, 0, FX32_ONE, 0, &camera ) ;

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

    const VecFx32 pos1 = { -FX32_ONE * 10, 0, 0 } ;
    const VecFx32 pos2 = {  FX32_ONE * 10, 0, 0 } ;

    // �w�肵���ʒu����p�[�e�B�N������o
    SPL_EmitAt( spl_manager, spl_emitter, &pos1 ) ; 
    SPL_EmitAt( spl_manager, spl_emitter, &pos2 ) ;     

    SPL_Calc( spl_manager ) ;
    
//    OS_Printf("�G�~�b�^��%d �p�[�e�B�N����%d\n", SPL_GetEmitterNum(spl_manager), SPL_GetWholeParticleNum( spl_manager ) ) ;

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