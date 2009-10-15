/*-------------------------------------------
 * SPL library �T���v�� "2DEffect"
 *
 * SPL�G�t�F�N�g��2D OBJ���������ĕ`�悷��T���v���ł�
 *
 * (c)2004 Nintendo
 * 
 * 
 * $Log: main.c,v $
 * Revision 1.3  2005/02/18 04:15:16  okane_shinji
 * VRAM��������
 *
 * Revision 1.2  2004/12/28 02:42:35  okane_shinji
 * (none)
 *
 * Revision 1.1  2004/12/28 02:20:06  okane_shinji
 * ���œo�^
 *
 * Revision 1.5  2004/12/27 04:33:50  okane_shinji
 * (none)
 *
 * Revision 1.4  2004/12/27 04:31:37  okane_shinji
 * ���o�^
 *
 * 
 *-------------------------------------------*/


#include<nnsys.h>

#include"ptcl_demo.h"
#include"spl.h"

// �p�[�e�B�N���֌W
static SPLManager *spl_manager = NULL  ;
static SPLEmitter *spl_emitter[4] ;
static VecFx32     init_pos    = {0,0,0} ;
void              *resPtcl ;

// 2D�֌W
static NNSG2dCellDataBank      *pCellBank; // �Z���o���N�ւ̃|�C���^
static NNSG2dOamManagerInstance sObjOamManager;  // OBJ �o�͗p OAM �}�l�[�W��
static NNSG2dImageProxy         sImageProxy;    // Cell �p�L�����N�^/�e�N�X�`���v���L�V
static NNSG2dImagePaletteProxy  sPaletteProxy;  // Cell �p�p���b�g�v���L�V

#define NUM_OF_OAM                  128                     // OAM �}�l�[�W���Ɋ��蓖�Ă� OAM �̐�
#define NUM_OF_AFFINE               ( NUM_OF_OAM / 4 )        // OAM �}�l�[�W���Ɋ��蓖�Ă� Affine �p�����[�^�̐�

// ���ݕ\�����̃G�~�b�^
static u16 suCurrentEmitter = 0 ;

// �X�P�[��
static fx32 sScale[16] =
{
    FX_F32_TO_FX32( 1.0f ),
    FX_F32_TO_FX32( 1.1f ),
    FX_F32_TO_FX32( 1.3f ),
    FX_F32_TO_FX32( 1.6f ),
    FX_F32_TO_FX32( 2.0f ),
    FX_F32_TO_FX32( 2.5f ),
    FX_F32_TO_FX32( 3.1f ),
    FX_F32_TO_FX32( 3.8f ),
    FX_F32_TO_FX32( 4.6f ),
    FX_F32_TO_FX32( 5.5f ),
    FX_F32_TO_FX32( 6.0f ),
    FX_F32_TO_FX32( 7.1f ),
    FX_F32_TO_FX32( 8.2f ),
    FX_F32_TO_FX32( 9.5f ),
    FX_F32_TO_FX32( 10.9f ),
    FX_F32_TO_FX32( 13.4f ),

} ;


/*---------------------------------------------------
 * ������
 *-------------------------------------------------*/
static void init()
{
    NNSG2dCharacterData *pCharData;
    NNSG2dPaletteData   *pPlttData;
    u16 i ;

    void *pBuf ;

    // �e�평����
    PtclDemo_InitSystem() ;
    PtclDemo_InitVRAM() ;
    PtclDemo_InitMemory() ;
    PtclDemo_InitScreen() ;

    // VRAM�}�l�[�W�����Z�b�g
    NNS_GfdInitFrmTexVramManager(3, TRUE);
    NNS_GfdInitFrmPlttVramManager(0x8000, TRUE);

    // G2D��������
    NNS_G2dInitOamManagerModule() ;
    NNS_G2dInitImageProxy( &sImageProxy );
    NNS_G2dInitImagePaletteProxy( &sPaletteProxy );
    (void)NNS_G2dGetNewOamManagerInstance( &sObjOamManager, 0, NUM_OF_OAM -1 , 0, NUM_OF_AFFINE - 1, NNS_G2D_OAMTYPE_MAIN ) ;

    //////////////////////////////////////////////////
    // CELL��ǂݍ���

    // �Z���f�[�^�ǂݍ���
    (void)PtclDemo_LoadNCER( &pCellBank, "data/Cell_sample.NCER" );

    // 2D�f�[�^�ǂݍ���
    pBuf = PtclDemo_LoadNCGR( &pCharData, "data/Character_sample.NCGR" );
    NNS_G2dLoadImage2DMapping( pCharData, 0, NNS_G2D_VRAM_TYPE_2DMAIN, &sImageProxy );
    OS_Free( pBuf ); // VRAM�ɃR�s�[�����̂ŉ��
    
    // �p���b�g�f�[�^�ǂݍ���
    pBuf = PtclDemo_LoadNCLR( &pPlttData, "data/Color_sample.NCLR" );
    NNS_G2dLoadPalette( pPlttData, 0, NNS_G2D_VRAM_TYPE_2DMAIN, &sPaletteProxy );
    OS_Free( pBuf ); // VRAM�ɃR�s�[�����̂ŉ��


    //////////////////////////////////////////////////
    // SPL������

    // �p�[�e�B�N���}�l�[�W����������
    spl_manager = SPL_Init( OS_AllocFromMain, 4, 200, 0, 1, 30 );

    // �p�[�e�B�N�����\�[�X��ǂݍ���
    resPtcl = PtclDemo_LoadFile( "data/output.spa" ) ;
    SPL_Load( spl_manager, resPtcl );

    // �e�N�X�`����ǂݍ���
    (void)SPL_LoadTexByVRAMManager( spl_manager ) ;
    (void)SPL_LoadTexPlttByVRAMManager( spl_manager ) ;

    // �G�~�b�^���쐬
    spl_emitter[0] = SPL_Create( spl_manager, 0, &init_pos );
    spl_emitter[1] = SPL_Create( spl_manager, 1, &init_pos );
    spl_emitter[2] = SPL_Create( spl_manager, 2, &init_pos );
    spl_emitter[3] = SPL_Create( spl_manager, 3, &init_pos );
   

    // �A�t�B���p�����[�^��o�^����
    for( i = 0 ; i < 16 ; ++i )
    {
        MtxFx22 affine ;
        MTX_Identity22( &affine );
        MTX_Scale22( &affine, sScale[i], sScale[i] );
        NNS_G2dSetOamManagerAffine( NNS_G2D_OAMTYPE_MAIN, &affine, i );
    }

    
}

/*---------------------------------------------------
 * �`��
 *-------------------------------------------------*/
static void draw()
{

    unsigned short uUsedOAM = 0 ;
    const NNSG2dCellData* pCell;                      // �\������ Cell
    
    // �e���|������OAM�o�b�t�@
    static GXOamAttr    temp[NUM_OF_OAM] ;
    int i = 0 ;

    // �擪�̃G�~�b�^���擾�B
    SPLParticle* ptcl = SPL_GetHeadParticle( spl_emitter[suCurrentEmitter] ) ;

    if( ptcl == NULL )
    {
        return ;
    }

    // �A�N�e�B�u�ȃp�[�e�B�N�����ׂĂɑ΂��āB
    while(1)
    {
        VecFx32 ptcl_pos ;
        NNSG2dFVec2 pos ;
        MtxFx22 affine ;
        u16 affineIdx = 0 ;
        u16 ptcl_age, ptcl_life ;
        
        // �p�[�e�B�N���̈ʒu���擾����
        SPL_GetParticlePos( ptcl, &ptcl_pos ) ;
     
        // �p�[�e�B�N���̎����ƔN����擾����
        ptcl_life = SPL_GetParticleLife( ptcl ) ;
        ptcl_age  = SPL_GetParticleAge( ptcl ) ;
        
        // �����ƔN��猻�݂̃X�P�[���C���f�b�N�X���v�Z����B
        affineIdx = (u16)( ( FX_Mul( FX_Div( FX32_ONE * 15, ptcl_life * FX32_ONE ), ptcl_age * FX32_ONE ) ) >> FX32_SHIFT ) ;

        // �}�g���N�X�쐬
        MTX_Identity22( &affine );
        MTX_Scale22( &affine, FX_Div( FX32_ONE, sScale[affineIdx] ), FX_Div( FX32_ONE, sScale[affineIdx] ) );

        // �ړ��ʂ̃X�P�[�����O�ƁA��ʒ��S�ւ̈ړ��␳�B
        pos.x = ( ptcl_pos.x << 2 ) + 128 * FX32_ONE;
        pos.y = ( ( -ptcl_pos.y) << 2 ) + 96  * FX32_ONE ;

        // �Z�����擾
        pCell = NNS_G2dGetCellDataByIdx( pCellBank, 0 );

        // Oam�ɏ����o��
        uUsedOAM += NNS_G2dMakeCellToOams( &temp[i], (u16)( NUM_OF_OAM-uUsedOAM ), pCell, &affine, &pos, affineIdx, FALSE ) ;
                                
        // ���̃p�[�e�B�N���𒲂ׂ�B
        ptcl = SPL_GetNextParticle( ptcl ) ;
        
        ++i ;

        if( ptcl == NULL )
        {
            break ;
        }

    }

    (void)NNS_G2dEntryOamManagerOam( &sObjOamManager, temp, uUsedOAM );
    
    // 2D�`��̂Ƃ���SPL_Draw�͕s�v�ł��B
    //SPL_Draw( spl_manager, &camera );

}

/*---------------------------------------------------
 * �v�Z
 *-------------------------------------------------*/
static void calc()
{
    SPL_Calc( spl_manager ) ;
    
    // A�{�^���������ƕ\������G�~�b�^��؂�ւ���
    if( PtclDemo_KeyOneshot( PAD_BUTTON_A ) )
    {
        ++suCurrentEmitter ;
        
        if( suCurrentEmitter > 3 )
        {
            suCurrentEmitter = 0 ;
        }
    }
    
    PtclDemo_UpdateInput() ;
}

/*---------------------------------------------------
 * ���C��
 *-------------------------------------------------*/
void NitroMain()
{
    init() ;

    while(1)
    {
        draw();

        calc() ;

        SVC_WaitVBlankIntr(); 
        NNS_G2dApplyOamManagerToHW( &sObjOamManager );
        NNS_G2dResetOamManagerBuffer(  &sObjOamManager );
        
    }
}



// end of file