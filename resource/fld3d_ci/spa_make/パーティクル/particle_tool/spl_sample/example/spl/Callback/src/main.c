/*-------------------------------------------
 * SPL library �T���v�� "Callback"
 *
 * �R�[���o�b�N�֐���p�����T���v���ł��B
 * 
 * �R�[���o�b�N�֐��̒��Ńp�[�e�B�N����
 * ���̈ʒu�ɓ��B������A���̃p�[�e�B�N�����I��
 * �����܂��B�܂��AA�{�^�����������Ƃ�
 * �R�[���o�b�N�֐����Ăяo�����ǂ����؂�ւ��邱�Ƃ��ł��܂��B
 *
 * (c)2004 Nintendo
 * 
 * 
 * $Log: main.c,v $
 * Revision 1.4  2005/02/18 08:31:28  okane_shinji
 * VRAM�����ѹ��ˤȤ�ʤ�����
 *
 * Revision 1.3  2005/02/09 00:15:54  okane_shinji
 * �R�[���o�b�N�֐��̎d�l�ύX�ɑΉ�
 *
 * Revision 1.2  2004/12/02 07:59:54  okane_shinji
 * ver110�Ή�
 *
 * Revision 1.1  2004/09/15 01:29:43  okane_shinji
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

BOOL callback_enable = FALSE ;

/*---------------------------------------------------
 * �R�[���o�b�N�֐�
 * ���̊֐��̓G�~�b�^emi��Calc������Ăяo����܂��B
 *-------------------------------------------------*/
static void sCallbackFunc( SPLEmitter* emi, unsigned int uFlag )
{
    // �擪�̃G�~�b�^���擾
    SPLParticle* ptcl = SPL_GetHeadParticle( emi ) ;

    // �R�[���o�b�N�̌Ăяo���ʒu���m�F
    if( uFlag != SPL_EMITTER_CALLBACK_BACK )
    {
        return ;
    }

    // �G�~�b�^�ɑ�����p�[�e�B�N���𒲂ׂĂ����āA�͈͊O�ɂ���Ύ������I��
    while(1)
    {
        VecFx32 ptcl_pos ;
        
        // �p�[�e�B�N���̈ʒu���擾����
        SPL_GetParticlePos( ptcl, &ptcl_pos ) ;
        
        // �p�[�e�B�N����y�ʒu������l(FX32_ONE)�ȏ�ł���΃p�[�e�B�N�����I������
        if( ptcl_pos.y > FX32_ONE )
        {
            SPL_TerminateParticleLife( ptcl ) ;
        }

        // ���̃p�[�e�B�N���𒲂ׂ�B
        ptcl = SPL_GetNextParticle( ptcl ) ;
    
        if( ptcl == NULL )
        {
            break ;
        }
    }
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
    spl_emitter = SPL_Create( spl_manager, 0, &init_pos );

    // �G�~�b�^�ɃR�[���o�b�N�֐������蓖�Ă�
    SPL_SetCallbackFunc( spl_emitter, sCallbackFunc ) ;
    callback_enable = TRUE ;

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

    // A�{�^���������ꂽ��R�[���o�b�N�̗L�� / ������؂�ւ���
    if( PtclDemo_KeyOneshot( PAD_BUTTON_A ) )
    {
        if( callback_enable )
        {
            SPL_SetCallbackFunc( spl_emitter, NULL ) ;
        } else {
            SPL_SetCallbackFunc( spl_emitter, sCallbackFunc ) ;
        }
        callback_enable = !callback_enable ;  
    }
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

        PtclDemo_UpdateInput() ;

        G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
        SVC_WaitVBlankIntr(); 
    }
}



// end of file