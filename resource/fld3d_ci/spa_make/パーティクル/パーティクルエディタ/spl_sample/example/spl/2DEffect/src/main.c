/*-------------------------------------------
 * SPL library サンプル "2DEffect"
 *
 * SPLエフェクトを2D OBJをもちいて描画するサンプルです
 *
 * (c)2004 Nintendo
 * 
 * 
 * $Log: main.c,v $
 * Revision 1.3  2005/02/18 04:15:16  okane_shinji
 * VRAMｴﾉﾍ�ｽ､ﾀｵ
 *
 * Revision 1.2  2004/12/28 02:42:35  okane_shinji
 * (none)
 *
 * Revision 1.1  2004/12/28 02:20:06  okane_shinji
 * 初版登録
 *
 * Revision 1.5  2004/12/27 04:33:50  okane_shinji
 * (none)
 *
 * Revision 1.4  2004/12/27 04:31:37  okane_shinji
 * 初登録
 *
 * 
 *-------------------------------------------*/


#include<nnsys.h>

#include"ptcl_demo.h"
#include"spl.h"

// パーティクル関係
static SPLManager *spl_manager = NULL  ;
static SPLEmitter *spl_emitter[4] ;
static VecFx32     init_pos    = {0,0,0} ;
void              *resPtcl ;

// 2D関係
static NNSG2dCellDataBank      *pCellBank; // セルバンクへのポインタ
static NNSG2dOamManagerInstance sObjOamManager;  // OBJ 出力用 OAM マネージャ
static NNSG2dImageProxy         sImageProxy;    // Cell 用キャラクタ/テクスチャプロキシ
static NNSG2dImagePaletteProxy  sPaletteProxy;  // Cell 用パレットプロキシ

#define NUM_OF_OAM                  128                     // OAM マネージャに割り当てる OAM の数
#define NUM_OF_AFFINE               ( NUM_OF_OAM / 4 )        // OAM マネージャに割り当てる Affine パラメータの数

// 現在表示中のエミッタ
static u16 suCurrentEmitter = 0 ;

// スケール
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
 * 初期化
 *-------------------------------------------------*/
static void init()
{
    NNSG2dCharacterData *pCharData;
    NNSG2dPaletteData   *pPlttData;
    u16 i ;

    void *pBuf ;

    // 各種初期化
    PtclDemo_InitSystem() ;
    PtclDemo_InitVRAM() ;
    PtclDemo_InitMemory() ;
    PtclDemo_InitScreen() ;

    // VRAMマネージャをセット
    NNS_GfdInitFrmTexVramManager(3, TRUE);
    NNS_GfdInitFrmPlttVramManager(0x8000, TRUE);

    // G2Dを初期化
    NNS_G2dInitOamManagerModule() ;
    NNS_G2dInitImageProxy( &sImageProxy );
    NNS_G2dInitImagePaletteProxy( &sPaletteProxy );
    (void)NNS_G2dGetNewOamManagerInstance( &sObjOamManager, 0, NUM_OF_OAM -1 , 0, NUM_OF_AFFINE - 1, NNS_G2D_OAMTYPE_MAIN ) ;

    //////////////////////////////////////////////////
    // CELLを読み込み

    // セルデータ読み込み
    (void)PtclDemo_LoadNCER( &pCellBank, "data/Cell_sample.NCER" );

    // 2Dデータ読み込み
    pBuf = PtclDemo_LoadNCGR( &pCharData, "data/Character_sample.NCGR" );
    NNS_G2dLoadImage2DMapping( pCharData, 0, NNS_G2D_VRAM_TYPE_2DMAIN, &sImageProxy );
    OS_Free( pBuf ); // VRAMにコピーしたので解放
    
    // パレットデータ読み込み
    pBuf = PtclDemo_LoadNCLR( &pPlttData, "data/Color_sample.NCLR" );
    NNS_G2dLoadPalette( pPlttData, 0, NNS_G2D_VRAM_TYPE_2DMAIN, &sPaletteProxy );
    OS_Free( pBuf ); // VRAMにコピーしたので解放


    //////////////////////////////////////////////////
    // SPL初期化

    // パーティクルマネージャを初期化
    spl_manager = SPL_Init( OS_AllocFromMain, 4, 200, 0, 1, 30 );

    // パーティクルリソースを読み込み
    resPtcl = PtclDemo_LoadFile( "data/output.spa" ) ;
    SPL_Load( spl_manager, resPtcl );

    // テクスチャを読み込み
    (void)SPL_LoadTexByVRAMManager( spl_manager ) ;
    (void)SPL_LoadTexPlttByVRAMManager( spl_manager ) ;

    // エミッタを作成
    spl_emitter[0] = SPL_Create( spl_manager, 0, &init_pos );
    spl_emitter[1] = SPL_Create( spl_manager, 1, &init_pos );
    spl_emitter[2] = SPL_Create( spl_manager, 2, &init_pos );
    spl_emitter[3] = SPL_Create( spl_manager, 3, &init_pos );
   

    // アフィンパラメータを登録する
    for( i = 0 ; i < 16 ; ++i )
    {
        MtxFx22 affine ;
        MTX_Identity22( &affine );
        MTX_Scale22( &affine, sScale[i], sScale[i] );
        NNS_G2dSetOamManagerAffine( NNS_G2D_OAMTYPE_MAIN, &affine, i );
    }

    
}

/*---------------------------------------------------
 * 描画
 *-------------------------------------------------*/
static void draw()
{

    unsigned short uUsedOAM = 0 ;
    const NNSG2dCellData* pCell;                      // 表示する Cell
    
    // テンポラリなOAMバッファ
    static GXOamAttr    temp[NUM_OF_OAM] ;
    int i = 0 ;

    // 先頭のエミッタを取得。
    SPLParticle* ptcl = SPL_GetHeadParticle( spl_emitter[suCurrentEmitter] ) ;

    if( ptcl == NULL )
    {
        return ;
    }

    // アクティブなパーティクルすべてに対して。
    while(1)
    {
        VecFx32 ptcl_pos ;
        NNSG2dFVec2 pos ;
        MtxFx22 affine ;
        u16 affineIdx = 0 ;
        u16 ptcl_age, ptcl_life ;
        
        // パーティクルの位置を取得する
        SPL_GetParticlePos( ptcl, &ptcl_pos ) ;
     
        // パーティクルの寿命と年齢を取得する
        ptcl_life = SPL_GetParticleLife( ptcl ) ;
        ptcl_age  = SPL_GetParticleAge( ptcl ) ;
        
        // 寿命と年齢から現在のスケールインデックスを計算する。
        affineIdx = (u16)( ( FX_Mul( FX_Div( FX32_ONE * 15, ptcl_life * FX32_ONE ), ptcl_age * FX32_ONE ) ) >> FX32_SHIFT ) ;

        // マトリクス作成
        MTX_Identity22( &affine );
        MTX_Scale22( &affine, FX_Div( FX32_ONE, sScale[affineIdx] ), FX_Div( FX32_ONE, sScale[affineIdx] ) );

        // 移動量のスケーリングと、画面中心への移動補正。
        pos.x = ( ptcl_pos.x << 2 ) + 128 * FX32_ONE;
        pos.y = ( ( -ptcl_pos.y) << 2 ) + 96  * FX32_ONE ;

        // セルを取得
        pCell = NNS_G2dGetCellDataByIdx( pCellBank, 0 );

        // Oamに書き出す
        uUsedOAM += NNS_G2dMakeCellToOams( &temp[i], (u16)( NUM_OF_OAM-uUsedOAM ), pCell, &affine, &pos, affineIdx, FALSE ) ;
                                
        // 次のパーティクルを調べる。
        ptcl = SPL_GetNextParticle( ptcl ) ;
        
        ++i ;

        if( ptcl == NULL )
        {
            break ;
        }

    }

    (void)NNS_G2dEntryOamManagerOam( &sObjOamManager, temp, uUsedOAM );
    
    // 2D描画のときはSPL_Drawは不要です。
    //SPL_Draw( spl_manager, &camera );

}

/*---------------------------------------------------
 * 計算
 *-------------------------------------------------*/
static void calc()
{
    SPL_Calc( spl_manager ) ;
    
    // Aボタンを押すと表示するエミッタを切り替える
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
 * メイン
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