//==============================================================================
/**
 * @file	bb_disp.c
 * @brief	�ȒP�Ȑ���������
 * @author	goto
 * @date	2007.10.01(��)
 *
 * �����ɐF�X�ȉ�����������Ă��悢
 *
 */
//==============================================================================

#include <gflib.h>
#include "bb_common.h"
#include "print\gf_font.h"
#include "font/font.naix"

#include "wlmngm_tool.naix"
#include "system/bmp_winframe.h"

#include "communication/comm_info.h"
#define BB_PRINT_COL		( GF_PRINTCOLOR_MAKE( 1,  2, 15 ) )
#define BB_PRINT_COL_VIP	( GF_PRINTCOLOR_MAKE( 5,  6, 15 ) )

//==============================================================
// Prototype
//==============================================================
static inline void BB_inline_ModelDraw( BB_3D_MODEL* wk );
static inline void BB_inline_LightSet( void );
static void BB_disp_COAP_SimpleInit( TCATS_OBJECT_ADD_PARAM_S* coap, s16 x, s16 y, int d_area, int pal, int id );


//--------------------------------------------------------------
/**
 * @brief	���f���`��
 *
 * @param	obj	
 * @param	mat43	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_3DModelDraw( GFL_G3D_OBJSTATUS* obj, MtxFx43* mat43, VecFx32* pos )
{
	if( obj->draw_flg ){
		NNS_G3dGlbFlush();
		NNS_G3dGeTranslateVec( pos );
		NNS_G3dGeMultMtx43( mat43 );
		NNS_G3dGlbSetBaseScale( &obj->scale );	///< �g��𔽉f�����邽��
		NNS_G3dDraw( &obj->render );
	}
}


//--------------------------------------------------------------
/**
 * @brief	model �� �`��
 *
 * @param	obj	
 *
 * @retval	static inline	
 *
 */
//--------------------------------------------------------------
static inline void BB_inline_ModelDraw( BB_3D_MODEL* wk )
{    
	NNS_G3dGePushMtx();
	
	BB_disp_3DModelDraw( &wk->obj, &wk->tmp43, &wk->pos );
	
	NNS_G3dGePopMtx( 1 );
}

static inline void BB_inline_ModelDrawEx( BB_3D_MODEL* wk, BB_3D_MODEL* wk2 )
{    
	NNS_G3dGePushMtx();
	
	BB_disp_3DModelDraw( &wk2->obj, &wk->tmp43, &wk->pos );
	
	NNS_G3dGePopMtx( 1 );
}


//--------------------------------------------------------------
/**
 * @brief	Light �� �����Ă�
 *
 * @param	none	
 *
 * @retval	static inline	
 *
 */
//--------------------------------------------------------------
static inline void BB_inline_LightSet( void )
{
	VecFx32 vec = { BB_LIGHT_VX, BB_LIGHT_VY, BB_LIGHT_VZ };
	
	VEC_Normalize( &vec, &vec );
	NNS_G3dGlbLightVector( BB_LIGHT_NO, vec.x, vec.y, vec.z );

	NNS_G3dGlbLightColor( BB_LIGHT_NO, GX_RGB( 31, 31, 31 ) );

	NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
	NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );	
}



//--------------------------------------------------------------
/**
 * @brief	���f�� ���[�h
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Model_Load( BB_3D_MODEL* wk, ARCHANDLE* p_handle, int id )
{
	D3DOBJ_MdlLoadH( &wk->mdl, p_handle, id, HEAPID_BB );
}


//--------------------------------------------------------------
/**
 * @brief	���f�� �o�^
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Model_Init( BB_3D_MODEL* wk, int flag )
{
	D3DOBJ_Init( &wk->obj, &wk->mdl );
	D3DOBJ_SetMatrix( &wk->obj, BB_MODEL_OFS_X, BB_MODEL_OFS_Y, BB_MODEL_OFS_Z );
	
	if ( flag == 0 ){
		D3DOBJ_SetScale( &wk->obj, FX32_CONST(1.00f), FX32_CONST(1.00f), FX32_CONST(1.00f) );
	}
	else {
		D3DOBJ_SetScale( &wk->obj, FX32_CONST(1.50f), FX32_CONST(1.50f), FX32_CONST(1.50f) );
	}
    D3DOBJ_SetDraw( &wk->obj, TRUE );
    
    wk->pos.x = BB_MODEL_OFS_X;
    wk->pos.y = BB_MODEL_OFS_Y;
    wk->pos.z = BB_MODEL_OFS_Z;
    
    wk->get_pow_x = 0.0f;
    wk->get_pow_y = 0.0f;
    
    wk->bAnime = FALSE;
    wk->bInit  = TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	���f�� �j��
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Model_Delete( BB_3D_MODEL* wk )
{
	wk->bInit = FALSE;
	D3DOBJ_MdlDelete( &wk->mdl );
}


//--------------------------------------------------------------
/**
 * @brief	�`��
 *
 * @param	wk	
 *
 * @retval	none	
 */
//--------------------------------------------------------------
void BB_disp_Draw( BB_WORK* wk )
{
	GFL_G3D_DRAW_Start();
	GFL_G3D_DRAW_SetLookAt();
	
	BB_inline_LightSet();
	
	{
		int i;		
		for ( i = 0; i < BB_KAMI_HUBUKI_MAX; i++ ){
			BB_inline_ModelDraw( &wk->p_client->bb3d_kami[ i ] );
			D3DOBJ_AnmLoop( &wk->p_client->bb3d_kami[ i ].anm[ 0 ], FX32_CONST( 1 ) );
		}
	}
	
	switch ( wk->p_client->anime_type ){
	case eANM_CODE_DEFAULT:
		D3DOBJ_SetDraw( &wk->p_client->bb3d_mane[ 0 ].obj, TRUE );
		D3DOBJ_SetDraw( &wk->p_client->bb3d_mane[ 1 ].obj, FALSE );
		D3DOBJ_SetDraw( &wk->p_client->bb3d_mane[ 2 ].obj, FALSE );
		D3DOBJ_AnmLoop( &wk->p_client->bb3d_mane[ 0 ].anm[ wk->p_client->bb3d_mane[ 0 ].anime_no ], FX32_CONST( 1 ) );
		break;

	case eANM_CODE_FALL:
		D3DOBJ_SetDraw( &wk->p_client->bb3d_mane[ 0 ].obj, FALSE );
		D3DOBJ_SetDraw( &wk->p_client->bb3d_mane[ 1 ].obj, TRUE );
		D3DOBJ_SetDraw( &wk->p_client->bb3d_mane[ 2 ].obj, FALSE );
		D3DOBJ_AnmNoLoop( &wk->p_client->bb3d_mane[ 1 ].anm[ 0 ], FX32_CONST( 1 ) );
		break;

	case eANM_CODE_RECOVER:
		D3DOBJ_SetDraw( &wk->p_client->bb3d_mane[ 0 ].obj, FALSE );
		D3DOBJ_SetDraw( &wk->p_client->bb3d_mane[ 1 ].obj, FALSE );
		D3DOBJ_SetDraw( &wk->p_client->bb3d_mane[ 2 ].obj, TRUE );
		D3DOBJ_AnmNoLoop( &wk->p_client->bb3d_mane[ 2 ].anm[ 0 ], FX32_CONST( 1 ) );
		break;
	}		
	BB_inline_ModelDrawEx( &wk->p_client->bb3d_mane[ 0 ], &wk->p_client->bb3d_mane[ wk->p_client->anime_type ] );
	BB_inline_ModelDraw( &wk->p_client->bb3d_ball );
	BB_inline_ModelDraw( &wk->p_client->bb3d_spot );

	///< lv up�@�㏸
	if ( wk->p_client->bb3d_lvup.bInit ){
		BB_inline_ModelDraw( &wk->p_client->bb3d_lvup );		
		if ( wk->p_client->bb3d_lvup.bAnime ){
			BOOL bEnd[ 2 ];
			bEnd[ 0 ] = D3DOBJ_AnmNoLoop( &wk->p_client->bb3d_lvup.anm[ 0 ], FX32_CONST( 1 ) );
			bEnd[ 1 ] = D3DOBJ_AnmNoLoop( &wk->p_client->bb3d_lvup.anm[ 1 ], FX32_CONST( 1 ) );
			if ( bEnd[ 0 ] && bEnd[ 1 ] ){
				 wk->p_client->bb3d_lvup.bAnime = FALSE;
			}
		}
	}
	
	///< lv up �ʏ�
	if ( wk->p_client->bb3d_lvbs.bInit ){
		BB_inline_ModelDraw( &wk->p_client->bb3d_lvbs );		
		if ( wk->p_client->bb3d_lvbs.bAnime ){
		//	D3DOBJ_AnmLoop( &wk->p_client->bb3d_lvbs.anm[ 0 ], FX32_CONST( 1 ) );
			D3DOBJ_AnmLoop( &wk->p_client->bb3d_lvbs.anm[ 1 ], FX32_CONST( 1 ) );
		}
	}
	
	GFL_G3D_DRAW_End();

    GFL_CLACT_SYS_Main();
}


//--------------------------------------------------------------
/**
 * @brief	�܂˂˂̃A�j���ݒ�
 * @param	model	
 * @param	anime_no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void ManeneAnime_Set( BB_CLIENT* wk, int anime_no )
{
	int old = wk->bb3d_mane[ 0 ].anime_no;
	int now = anime_no;

	if ( now == old ){ return; }

	D3DOBJ_DelAnm( &wk->bb3d_mane[ 0 ].obj, &wk->bb3d_mane[ 0 ].anm[ old ] );

	wk->bb3d_mane[ 0 ].anime_no = anime_no;	///< anime �̕ύX

	D3DOBJ_AddAnm( &wk->bb3d_mane[ 0 ].obj, &wk->bb3d_mane[ 0 ].anm[ now ] );
}

//--------------------------------------------------------------
/**
 *
 * @param	model	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int ManeneAnime_Get( BB_WORK* wk )
{
	return wk->p_client->bb3d_mane[ 0 ].anime_no;
}


//--------------------------------------------------------------
/**
 * @brief	�܂˂˂̃A�j���t���O�ݒ�
 * @param	model	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void ManeneAnime_FlagSet( BB_WORK* wk, BOOL flag )
{
	wk->p_client->bb3d_mane[ 0 ].bAnime = flag;
}


//--------------------------------------------------------------
/**
 * @brief	�J���������Ă�
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_CameraSet( BB_WORK* wk )
{
	VecFx32	target = { BB_CAMERA_TX, BB_CAMERA_TY, BB_CAMERA_TZ };
	VecFx32	pos	   = { BB_CAMERA_PX, BB_CAMERA_PY, BB_CAMERA_PZ };
	
	GFC_InitCameraTC( &target, &pos, BB_CAMERA_PERSPWAY, GF_CAMERA_PERSPECTIV, FALSE, wk->sys.camera_p );
//	GFC_InitCameraTC( &target, &pos, BB_CAMERA_PERSPWAY, GF_CAMERA_PERSPECTIV, FALSE, wk->sys.camera_p );

	GFC_SetCameraClip( BB_CAMERA_NEAR_CLIP, BB_CAMERA_NEAR_FAR, wk->sys.camera_p );
	
	GFC_AttachCamera( wk->sys.camera_p );
}

//--------------------------------------------------------------
/**
 * @brief	�ȒP�ɏ�����
 *
 * @param	x	
 * @param	y	
 * @param	d_area	
 * @param	pal	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BB_disp_COAP_SimpleInit( TCATS_OBJECT_ADD_PARAM_S* coap, s16 x, s16 y, int d_area, int pal, int id )
{
	coap->x			 = x;
	coap->y			 = y;
	coap->z			 = 0;		
	coap->anm		 = 0;
	coap->pri		 = 0;
	coap->d_area	 = d_area;
	coap->bg_pri	 = 2;
	coap->vram_trans = 0;
	coap->pal		 = pal;
	coap->id[0]		 = id;
	coap->id[1]		 = id;
	coap->id[2]		 = id;
	coap->id[3]		 = id;
	coap->id[4]		 = CLACT_U_HEADER_DATA_NONE;
	coap->id[5]		 = CLACT_U_HEADER_DATA_NONE;	
}

static const s16 manene_pos[][ 3 ] = {
	{	///< �P�l�v���C
		0,0,0,
	},
	{	///< �Q�l�v���C
		128,0,0
	},
	{	///< �R�l�v���C
		85, 170,0,
	},
	{	///< �S�l�v���C
		64, 128, 192,
	},
};


//--------------------------------------------------------------
/**
 * @brief	�ԉ΂̓o�^
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Hanabi_OAM_Add( BB_CLIENT* wk )
{
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR			 csp = wk->sys->csp;
	CATS_RES_PTR			 crp = wk->sys->crp;
	PALETTE_FADE_PTR		 pfd = wk->sys->pfd;
	
	int i;
	
	for ( i = 0; i < BB_KAMI_MAX; i++ ){
		
		BB_disp_COAP_SimpleInit( &coap, i * 8, 20, CATS_D_AREA_MAIN, eBB_OAM_PAL_BD_HANABI, eBB_ID_HANABI );
		wk->cap_hanabi[ i ] = CATS_ObjectAdd_S( csp, crp, &coap );
		CATS_ObjectPaletteOffsetSetCap( wk->cap_hanabi[ i ], eBB_OAM_PAL_BD_HANABI );		
		GFL_CLACT_WK_SetAnmSeq( wk->cap_hanabi[ i ], ( i % 11 ) + 1 );
	}

	BB_disp_Hanabi_OAM_Enable( wk, FALSE, 0 );
}


//--------------------------------------------------------------
/**
 * @brief	��̓o�^
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Hand_Add( BB_CLIENT* wk )
{
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR			 csp = wk->sys->csp;
	CATS_RES_PTR			 crp = wk->sys->crp;
	PALETTE_FADE_PTR		 pfd = wk->sys->pfd;
	
	int i;
	s16 pos_x[] = {  72, 180,  24, 232 };
	
	for ( i = 0; i < BB_HAND_MAX; i++ ){
		BB_disp_COAP_SimpleInit( &coap, pos_x[ i ], BB_SURFACE_LIMIT, CATS_D_AREA_MAIN, eBB_OAM_PAL_BD_HAND, eBB_ID_HAND );
		coap.bg_pri = 0;
		coap.pri = 1;
		wk->cap_hand[ i ] = CATS_ObjectAdd_S( csp, crp, &coap );
		GFL_CLACT_WK_AddAnmFrame( wk->cap_hand[ i ], FX32_ONE*2 );
		CATS_ObjectPaletteOffsetSetCap( wk->cap_hand[ i ], eBB_OAM_PAL_BD_HAND );
	}
}


//--------------------------------------------------------------
/**
 * @brief	��̔j��
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Hand_Del( BB_CLIENT* wk )
{
	int i;
	
	for ( i = 0; i < BB_HAND_MAX; i++ ){
		GFL_CLACT_WK_Remove( wk->cap_hand[ i ] );
	}
}

void BB_disp_Hanabi_OAM_Enable( BB_CLIENT* wk, BOOL flag, int mode )
{
	int i;
	
	for ( i = 0; i < BB_KAMI_MAX; i++ ){		
		GFL_CLACT_WK_SetDrawEnable( wk->cap_hanabi[ i ], flag );
	}	
}

void BB_disp_Hanabi_OAM_Update( BB_CLIENT* wk )
{
	int i;
	
	for ( i = 0; i < BB_KAMI_MAX; i++ ){
		
		GFL_CLACT_WK_AddAnmFrame( wk->cap_hanabi[ i ], FX32_ONE*2 );
	}
}

void BB_disp_Hanabi_OAM_Del( BB_CLIENT* wk )
{
	int i;
	
	for ( i = 0; i < BB_KAMI_MAX; i++ ){
		
		GFL_CLACT_WK_Remove( wk->cap_hanabi[ i ] );
	}
}

void BB_disp_Manene_OAM_AnimeChangeCap( GFL_CLWK cap, int type, int anime )
{
	int anime_seq[][2] = {
		{ eANM_MANENE_ARUKU,  eANM_BALL_STOP },
		{ eANM_MANENE_ARUKU,  eANM_BALL_ARUKU },
		{ eANM_MANENE_ARUKU,  eANM_BALL_ARUKU2 },
		{ eANM_MANENE_OCHIRU, eANM_BALL_STOP },
	};
		
	GFL_CLACT_WK_SetAnmSeq( cap, anime_seq[ anime ][ type ] );
}

void BB_disp_Manene_OAM_AnimeChange( BB_CLIENT* wk, int netid, int anime )
{
	int anime_seq[][2] = {
		{ eANM_MANENE_ARUKU,  eANM_BALL_STOP },
		{ eANM_MANENE_ARUKU,  eANM_BALL_ARUKU },
		{ eANM_MANENE_ARUKU,  eANM_BALL_ARUKU2 },
		{ eANM_MANENE_OCHIRU, eANM_BALL_STOP },
	};
	
	int no = wk->netid_to_capid[ netid ];
	
	GFL_CLACT_WK_SetAnmSeq( wk->cap_mane[ no ], anime_seq[ anime ][ 0 ] );
	GFL_CLACT_WK_SetAnmSeq( wk->cap_ball[ no ], anime_seq[ anime ][ 1 ] );
}


int BB_disp_Manene_OAM_AnimeCheck( BB_CLIENT* wk, int netid )
{
	int anime_seq[][2] = {
		{ eANM_MANENE_ARUKU,  eANM_BALL_STOP },
		{ eANM_MANENE_ARUKU,  eANM_BALL_ARUKU },
		{ eANM_MANENE_ARUKU,  eANM_BALL_ARUKU2 },
		{ eANM_MANENE_OCHIRU, eANM_BALL_STOP },
	};	
	int no;
	int anime[ 2 ];
	
	no = wk->netid_to_capid[ netid ];
	
	anime[ 0 ] = CATS_ObjectAnimeSeqGetCap( wk->cap_mane[ no ] );
	anime[ 1 ] = CATS_ObjectAnimeSeqGetCap( wk->cap_ball[ no ] );
	
	{
		int i;
		
		for ( i = 0; i < 4; i++ ){
			
			if ( anime[ 0 ] == anime_seq[ i ][ 0 ]
			&& 	 anime[ 1 ] == anime_seq[ i ][ 1 ] ){ return i; }
		}
	}

	GF_ASSERT( 0 );
	
	return 0;
}


void BB_disp_Manene_OAM_Update( BB_CLIENT* wk )
{
	int i;
	int no = 0;		
	
	for ( i = 0; i < wk->comm_num; i++ ){		
		if ( i == wk->netid ){ continue; }		
		GFL_CLACT_WK_AddAnmFrame( wk->cap_mane[ no ], FX32_ONE*2 );
		GFL_CLACT_WK_AddAnmFrame( wk->cap_ball[ no ], FX32_ONE*2 );
		GFL_CLACT_WK_AddAnmFrame( wk->cap_kage[ no ], FX32_ONE*2 );
		no++;
	}	
}

void BB_disp_Manene_OAM_Del( BB_CLIENT* wk )
{
	int i;
	int no = 0;		
	
	for ( i = 0; i < wk->comm_num; i++ ){
		
		if ( i == wk->netid ){ continue; }
		
		GFL_CLACT_WK_Remove( wk->cap_mane[ no ] );
		GFL_CLACT_WK_Remove( wk->cap_ball[ no ] );
		GFL_CLACT_WK_Remove( wk->cap_kage[ no ] );
		
		no++;
	}
}

//--------------------------------------------------------------
/**
 * @brief	���b�Z�[�W�E�B���h�E�쐬
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_InfoWinAdd( BB_CLIENT* wk )
{
	GF_BGL_BmpWinInit( &wk->win );
	wk->win = GFL_BMPWIN_Create( GFL_BG_FRAME3_S, 2, 19, 28, 4, 14, GFL_BMP_CHRAREA_GET_F );
	GFL_BMPWIN_MakeScreen(wk->win);
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win), 0xFF );	
	{
		GFL_MSGDATA* man;
		STRBUF* str;
			
		man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_minigame_bb_dat, HEAPID_BB );
		str	= GFL_MSG_CreateString( man, 0 );
		
		GF_STR_PrintSimple( &wk->win, FONT_SYSTEM, str, 0, 0, 0, NULL );

		GFL_STR_DeleteBuffer( str );
		GFL_MSG_Delete( man );	
	}	
	BmpWinFrame_Write( wk->win, WINDOW_TRANS_ON, BB_BG_CGX_OFS, 13 );
	GF_BGL_BmpWinOn( &wk->win );
}


//--------------------------------------------------------------
/**
 * @brief	���b�Z�[�W�E�B���h�E�j��
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_InfoWinDel( BB_CLIENT* wk )
{
	if( GF_BGL_BmpWinAddCheck( &wk->win ) == TRUE ){
		BmpWinFrame_Clear( wk->win, WINDOW_TRANS_OFF );
		GF_BGL_BmpWinOff( &wk->win );
		GFL_BMPWIN_Delete( &wk->win );
	}
}

//--------------------------------------------------------------
/**
 * @brief	���낪��
 *
 * @param	wk	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_ColorChange( BB_WORK* wk, int id )
{
	PALETTE_FADE_PTR pfd	= wk->sys.pfd;

	/// 1 = �� 2 = �� = 3 = �� 4 = ��
	static int pal_pos[][ 4 ][ 4 + 1 ] = {
		/// comm_num = 1
		{
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		/// comm_num = 2 
		{
			{ 1, 2, 0, 0, 0 },
			{ 2, 1, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		/// comm_num = 3
		{
			{ 1, 2, 3, 0, 0 },
			{ 2, 1, 3, 0, 0 },
			{ 3, 1, 2, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		/// comm_num = 4
		{
			{ 1, 2, 3, 4, 0 },
			{ 2, 1, 3, 4, 0 },
			{ 3, 1, 2, 4, 0 },
			{ 4, 1, 2, 3, 0 },
		},
	};
	
	{
		int i;
		
		for ( i = 0; i < 4; i++ ){
			PaletteWorkSetEx_Arc( pfd, ARCID_BB_RES, NARC_manene_upper_bg_NCLR, HEAPID_BB, FADE_SUB_BG, 0x20,
								  ( i + 1 ) * 16, pal_pos[ wk->p_client->comm_num - 1 ][ id ][ i + 1 ] * 16 );
		}
	}
}

static int BR_print_x_Get( GFL_BMPWIN* win, STRBUF* str )
{
	int len = PRINTSYS_GetStrWidth( str, GFL_FONT* font/*FONT_SYSTEM*/, 0 );
	int x	= ( GF_BGL_BmpWinGet_SizeX( win ) * 8 - len ) / 2;
	
	return x;
}
void BB_disp_NameWinAdd( BB_WORK* bwk, BB_CLIENT* wk )
{
	int i;
	int no = 0;
	int ofs = BB_CGX_OFS;
	
	s16 win_dat[][ 3 ][ 4 ] = {
		{ 
			{ 12, 8, 8, 2 },
			{ 0,0,0,0 },
			{ 0,0,0,0 },
		},
		{
			{  4+1, 8, 8, 2 },
			{ 18+1, 8, 8, 2 },
			{ 0,0,0,0 },
		},
		{
			{  0+1, 8, 8, 2 },
			{ 11+1, 8, 8, 2 },
			{ 22+1, 8, 8, 2 },
		},
	};
	int x,y,w,h;
	MYSTATUS* ms;
	BOOL bVip;
	
	switch ( wk->comm_num ){
	case 2:
		GFL_ARCHDL_UTIL_TransVramScreen( wk->sys->p_handle_bb, NARC_manene_upper_bg_maku3_1_NSCR, GFL_BG_FRAME3_S, 0, 0, 0, HEAPID_BB );
		break;
	case 3:
		GFL_ARCHDL_UTIL_TransVramScreen( wk->sys->p_handle_bb, NARC_manene_upper_bg_maku3_2_NSCR, GFL_BG_FRAME3_S, 0, 0, 0, HEAPID_BB );
		break;
	case 4:
		GFL_ARCHDL_UTIL_TransVramScreen( wk->sys->p_handle_bb, NARC_manene_upper_bg_maku3_3_NSCR, GFL_BG_FRAME3_S, 0, 0, 0, HEAPID_BB );
		break;
	default:
		GF_ASSERT( 0 );
	}
	
	for ( i = 0; i < wk->comm_num; i++ ){
		
		if ( i == wk->netid ){ continue; }
		
		bVip = MNGM_ENRES_PARAM_GetVipFlag( &bwk->entry_param, i );
		
		//ms  = CommInfoGetMyStatus( i );
		ms = MNGM_ENRES_PARAM_GetMystatus( &bwk->entry_param, i );
		GF_BGL_BmpWinInit( wk->win_name[ no ] );
		
		x = win_dat[ wk->comm_num - 2 ][ no ][ 0 ];
		y = win_dat[ wk->comm_num - 2 ][ no ][ 1 ];
		w = win_dat[ wk->comm_num - 2 ][ no ][ 2 ];
		h = win_dat[ wk->comm_num - 2 ][ no ][ 3 ];
		wk->win_name[ no ] = GFL_BMPWIN_Create( GFL_BG_FRAME3_S, x, y, w, h, 14, GFL_BMP_CHRAREA_GET_F );
		GFL_BMPWIN_MakeScreen(wk->win_name[no]);
		ofs += ( w * h );

		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win_name[ no ]), 0xFF );
		
		{
			STRBUF* str;
			int px;

			str	= MyStatus_CreateNameString( ms, HEAPID_BB );
			
			px = BR_print_x_Get( wk->win_name[ no ], str );
			
		//	GF_STR_PrintSimple( wk->win_name[ no ], FONT_SYSTEM, str, 0, 0, 0, NULL );
			if ( bVip ){
				GF_STR_PrintExpand( wk->win_name[ no ], FONT_SYSTEM, str, px, 0, 0, BB_PRINT_COL_VIP, 0, 0, NULL );
			}
			else {
				GF_STR_PrintExpand( wk->win_name[ no ], FONT_SYSTEM, str, px, 0, 0, BB_PRINT_COL, 0, 0, NULL );
			}

			GFL_STR_DeleteBuffer( str );
		}

		GF_BGL_BmpWinOn( wk->win_name[ no ] );
		
		no++;
	}
}

void BB_disp_NameWinDel( BB_CLIENT* wk )
{
	int i;
	int no = 0;
	
	for ( i = 0; i < wk->comm_num; i++ ){
		
		if ( i == wk->netid ){ continue; }		
		

		if( GF_BGL_BmpWinAddCheck( &wk->win_name[ no ] ) == TRUE ){
			BmpWinFrame_Clear( wk->win_name[ no ], WINDOW_TRANS_OFF );
		    GF_BGL_BmpWinOff( &wk->win_name[ no ] );
			GFL_BMPWIN_Delete( &wk->win_name[ no ] );
		}
		
		no++;
	}	
	GFL_ARCHDL_UTIL_TransVramScreen( wk->sys->p_handle_bb, NARC_manene_upper_bg_maku3_NSCR, GFL_BG_FRAME3_S, 0, 0, 0, HEAPID_BB );
}


//--------------------------------------------------------------
/**
 * @brief	���\�[�X�̓ǂݍ���
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_ResourceLoad( BB_WORK* wk )
{	
	ARCHANDLE*		 hdl_bb = wk->sys.p_handle_bb;
	ARCHANDLE*		 hdl_cr = wk->sys.p_handle_cr;
	CATS_SYS_PTR	 csp	= wk->sys.csp;
	CATS_RES_PTR	 crp	= wk->sys.crp;
	PALETTE_FADE_PTR pfd	= wk->sys.pfd;
	
	ARCHANDLE* hdl;
	int nca;
	int nce;
	int ncg;
	int ncl;
	int id;
	int palnum;
	
	int dest_palno;
	
	// ----- ���� -----
	
	hdl = hdl_bb;
    nca = NARC_manene_upper_oam_NANR;
	nce = NARC_manene_upper_oam_NCER;
	ncg = NARC_manene_upper_oam_NCGR;
	ncl = NARC_manene_upper_oam_NCLR;
	id  = eBB_ID_OAM_S;
	palnum = eBB_OAM_PAL_TD_MAX;

	wk->cgr_id[id] = GFL_CLGRP_CGR_Register(hdl, ncg, FALSE, CLSYS_DRAW_SUB, HEAPID_BB);
	wk->cell_id[id] = GFL_CLGRP_CELLANIM_Register(hdl, nce, nca, HEAPID_BB);
	wk->pltt_id[id] = PLTTSLOT_ResourceSet(
		wk->plttslot, hdl, ncl, CLSYS_DRAW_SUB, palnum, HEAPID_BB);
	dest_palno = GFL_CLGRP_PLTT_GetAddr(wk->pltt_id[id], CLSYS_DRAW_SUB) / 0x20;
	PaletteWorkSet_VramCopy(pfd, FADE_SUB_OBJ, dest_palno*16, palnum * 0x20);
	
	// ----- ����� -----
	// ��
    nca = NARC_manene_eff_star_NANR;
	nce = NARC_manene_eff_star_NCER;
	ncg = NARC_manene_eff_star_NCGR;
	ncl = NARC_manene_eff_star_NCLR;
	id  = eBB_ID_STAR;
	palnum = 1;

	wk->cgr_id[id] = GFL_CLGRP_CGR_Register(hdl, ncg, FALSE, CLSYS_DRAW_MAIN, HEAPID_BB);
	wk->cell_id[id] = GFL_CLGRP_CELLANIM_Register(hdl, nce, nca, HEAPID_BB);
	wk->pltt_id[id] = PLTTSLOT_ResourceSet(
		wk->plttslot, hdl, ncl, CLSYS_DRAW_MAIN, palnum, HEAPID_BB);
	dest_palno = GFL_CLGRP_PLTT_GetAddr(wk->pltt_id[id], CLSYS_DRAW_MAIN) / 0x20;
	PaletteWorkSet_VramCopy(pfd, FADE_MAIN_OBJ, dest_palno*16, palnum * 0x20);
	
	// ����
    nca = NARC_manene_bottom_hakusyu_NANR;
	nce = NARC_manene_bottom_hakusyu_NCER;
	ncg = NARC_manene_bottom_hakusyu_NCGR ;
	ncl = NARC_manene_bottom_hakusyu_NCLR;
	id  = eBB_ID_HAND;
	palnum = 1;

	wk->cgr_id[id] = GFL_CLGRP_CGR_Register(hdl, ncg, FALSE, CLSYS_DRAW_MAIN, HEAPID_BB);
	wk->cell_id[id] = GFL_CLGRP_CELLANIM_Register(hdl, nce, nca, HEAPID_BB);
	wk->pltt_id[id] = PLTTSLOT_ResourceSet(
		wk->plttslot, hdl, ncl, CLSYS_DRAW_MAIN, palnum, HEAPID_BB);
	dest_palno = GFL_CLGRP_PLTT_GetAddr(wk->pltt_id[id], CLSYS_DRAW_MAIN) / 0x20;
	PaletteWorkSet_VramCopy(pfd, FADE_MAIN_OBJ, dest_palno*16, palnum * 0x20);
	
	// ��
    nca = NARC_manene_bottom_kami_NANR;
	nce = NARC_manene_bottom_kami_NCER;
	ncg = NARC_manene_bottom_kami_NCGR ;
	ncl = NARC_manene_bottom_kami_NCLR;
	id  = eBB_ID_KAMI;
	palnum = 1;

	wk->cgr_id[id] = GFL_CLGRP_CGR_Register(hdl, ncg, FALSE, CLSYS_DRAW_MAIN, HEAPID_BB);
	wk->cell_id[id] = GFL_CLGRP_CELLANIM_Register(hdl, nce, nca, HEAPID_BB);
	wk->pltt_id[id] = PLTTSLOT_ResourceSet(
		wk->plttslot, hdl, ncl, CLSYS_DRAW_MAIN, palnum, HEAPID_BB);
	dest_palno = GFL_CLGRP_PLTT_GetAddr(wk->pltt_id[id], CLSYS_DRAW_MAIN) / 0x20;
	PaletteWorkSet_VramCopy(pfd, FADE_MAIN_OBJ, dest_palno*16, palnum * 0x20);
	
	
	// �ԉ�
    nca = NARC_manene_bottom_hanabi_NANR;
	nce = NARC_manene_bottom_hanabi_NCER;
	ncg = NARC_manene_bottom_hanabi_NCGR ;
	ncl = NARC_manene_bottom_hanabi_NCLR;
	id  = eBB_ID_HANABI;
	palnum = 1;

	wk->cgr_id[id] = GFL_CLGRP_CGR_Register(hdl, ncg, FALSE, CLSYS_DRAW_MAIN, HEAPID_BB);
	wk->cell_id[id] = GFL_CLGRP_CELLANIM_Register(hdl, nce, nca, HEAPID_BB);
	wk->pltt_id[id] = PLTTSLOT_ResourceSet(
		wk->plttslot, hdl, ncl, CLSYS_DRAW_MAIN, palnum, HEAPID_BB);
	dest_palno = GFL_CLGRP_PLTT_GetAddr(wk->pltt_id[id], CLSYS_DRAW_MAIN) / 0x20;
	PaletteWorkSet_VramCopy(pfd, FADE_MAIN_OBJ, dest_palno*16, palnum * 0x20);
	
	// ���C�g
    nca = NARC_manene_bottom_oam_NANR;
	nce = NARC_manene_bottom_oam_NCER;
	ncg = NARC_manene_bottom_oam_NCGR;
	ncl = NARC_manene_bottom_oam_NCLR;
	id  = eBB_ID_LIGHT_M;
	palnum = 3;

	wk->cgr_id[id] = GFL_CLGRP_CGR_Register(hdl, ncg, FALSE, CLSYS_DRAW_MAIN, HEAPID_BB);
	wk->cell_id[id] = GFL_CLGRP_CELLANIM_Register(hdl, nce, nca, HEAPID_BB);
	wk->pltt_id[id] = PLTTSLOT_ResourceSet(
		wk->plttslot, hdl, ncl, CLSYS_DRAW_MAIN, palnum, HEAPID_BB);
	dest_palno = GFL_CLGRP_PLTT_GetAddr(wk->pltt_id[id], CLSYS_DRAW_MAIN) / 0x20;
	PaletteWorkSet_VramCopy(pfd, FADE_MAIN_OBJ, dest_palno*16, palnum * 0x20);
	
	// �y��
	{
		ARCHANDLE* res_hdl = GFL_ARC_OpenDataHandle( ARCID_WLMNGM_TOOL_GRA, HEAPID_BB );

	    nca = NARC_wlmngm_tool_touchpen_NANR;
		nce = NARC_wlmngm_tool_touchpen_NCER;
		ncg = NARC_wlmngm_tool_touchpen_NCGR;
		ncl = NARC_wlmngm_tool_touchpen_NCLR;
		id  = eBB_ID_PEN;
		palnum = 2;
		wk->cgr_id[id] = GFL_CLGRP_CGR_Register(res_hdl, ncg, FALSE, CLSYS_DRAW_MAIN, HEAPID_BB);
		wk->cell_id[id] = GFL_CLGRP_CELLANIM_Register(res_hdl, nce, nca, HEAPID_BB);
		wk->pltt_id[id] = PLTTSLOT_ResourceSet(
			wk->plttslot, res_hdl, ncl, CLSYS_DRAW_MAIN, palnum, HEAPID_BB);
		dest_palno = GFL_CLGRP_PLTT_GetAddr(wk->pltt_id[id], CLSYS_DRAW_MAIN) / 0x20;
		PaletteWorkSet_VramCopy(pfd, FADE_MAIN_OBJ, dest_palno*16, palnum * 0x20);

		GFL_ARC_CloseDataHandle( res_hdl );
	}
}


void BB_disp_Manene_Add( BB_WORK* bb_wk, BB_CLIENT* wk )
{	
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR			 csp = wk->sys->csp;
	CATS_RES_PTR			 crp = wk->sys->crp;
	PALETTE_FADE_PTR		 pfd = wk->sys->pfd;
	
	int i;
	int no = 0;		
	
	for ( i = 0; i < wk->comm_num; i++ ){
		
		wk->netid_to_capid[ i ] = 0xFF;
		
		if ( i == wk->netid ){ continue; }
		
		wk->netid_to_capid[ i ] = no;
		
		BB_disp_COAP_SimpleInit( &coap, manene_pos[ wk->comm_num - 1 ][ no ] + BB_OAM_OX, BB_MANE_PY,// + Action_MoveValue_2Y(),
							 	 CATS_D_AREA_SUB, eBB_OAM_PAL_TD_MANENE, eBB_ID_OAM_S );
		coap.pri = 1;
		wk->cap_mane[ no ] = CATS_ObjectAdd_S( csp, crp, &coap );
		
		
		BB_disp_COAP_SimpleInit( &coap, manene_pos[ wk->comm_num - 1 ][ no ] + BB_OAM_OX, BB_BALL_PY, CATS_D_AREA_SUB, eBB_OAM_PAL_TD_MANENE, eBB_ID_OAM_S );
		coap.pri = 2;
		wk->cap_ball[ no ] = CATS_ObjectAdd_S( csp, crp, &coap );
		CATS_ObjectPaletteOffsetSetCap( wk->cap_ball[ no ], eBB_OAM_PAL_TD_MANENE + NetID_To_PlayerNo( bb_wk, i ) );
		
		
		BB_disp_COAP_SimpleInit( &coap, manene_pos[ wk->comm_num - 1 ][ no ] + BB_OAM_OX, BB_BALL_PY, CATS_D_AREA_SUB, eBB_OAM_PAL_TD_MANENE, eBB_ID_OAM_S );
		coap.pri = 3;
		wk->cap_kage[ no ] = CATS_ObjectAdd_S( csp, crp, &coap );
		CATS_ObjectPaletteOffsetSetCap( wk->cap_kage[ no ], eBB_OAM_PAL_TD_MANENE + NetID_To_PlayerNo( bb_wk, i ) );
		GFL_CLACT_WK_SetAnmSeq( wk->cap_kage[ no ], eANM_KAGE );

		GFL_CLACT_WK_AddAnmFrame( wk->cap_mane[ no ], FX32_ONE*2 );
		GFL_CLACT_WK_AddAnmFrame( wk->cap_ball[ no ], FX32_ONE*2 );
		GFL_CLACT_WK_AddAnmFrame( wk->cap_kage[ no ], FX32_ONE*2 );
		BB_disp_Manene_OAM_AnimeChange( wk, i, eANM_CODE_STOP );		
		no++;
	}
}


//--------------------------------------------------------------
/**
 * @brief	�X�^�[�_�X�g
 *
 * @param	bb_wk	
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
GFL_CLWK BB_disp_Stardust_Add( BB_CLIENT* wk, s16 x, s16 y )
{	
	GFL_CLWK			 cap;
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR			 csp = wk->sys->csp;
	CATS_RES_PTR			 crp = wk->sys->crp;
	PALETTE_FADE_PTR		 pfd = wk->sys->pfd;
	
	BB_disp_COAP_SimpleInit( &coap, x, y, CATS_D_AREA_MAIN, eBB_OAM_PAL_BD_STAR, eBB_ID_STAR );
	coap.bg_pri = 0;
	coap.pri	= 0;

	cap = CATS_ObjectAdd_S( csp, crp, &coap );

	GFL_CLACT_WK_AddAnmFrame( cap, FX32_ONE*2 );
	
	return cap;
}


//--------------------------------------------------------------
/**
 * @brief	���C�g
 *
 * @param	wk
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Light_Add( BB_CLIENT* wk )
{
	int i;
	GFL_CLWK			 cap;
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR			 csp = wk->sys->csp;
	CATS_RES_PTR			 crp = wk->sys->crp;
	PALETTE_FADE_PTR		 pfd = wk->sys->pfd;
	
	for ( i = 0; i < BB_LIGHT_MAX; i++ ){
		BB_disp_COAP_SimpleInit( &coap, 0, 0, CATS_D_AREA_MAIN, eBB_OAM_PAL_BD_LIGHT, eBB_ID_LIGHT_M );
		coap.bg_pri = 2;
		coap.pri	= 20;
		wk->cap_light_m[ i ] = CATS_ObjectAdd_S( csp, crp, &coap );
		GFL_CLACT_WK_SetAnmSeq( wk->cap_light_m[ i ], 0 );	
		GFL_CLACT_WK_AddAnmFrame( wk->cap_light_m[ i ] , FX32_ONE);
		CATS_ObjectPaletteOffsetSetCap( wk->cap_light_m[ i ], eBB_OAM_PAL_BD_LIGHT );	
		GFL_CLACT_WK_SetObjMode( wk->cap_light_m[ i ], GX_OAM_MODE_XLU );
		GFL_CLACT_WK_SetDrawEnable( wk->cap_light_m[ i ], FALSE );

		BB_disp_COAP_SimpleInit( &coap, 0, 0, CATS_D_AREA_SUB, eBB_OAM_PAL_TD_MANENE, eBB_ID_OAM_S );
		coap.bg_pri = 2;
		coap.pri	= 20;
		wk->cap_light_s[ i ] = CATS_ObjectAdd_S( csp, crp, &coap );
		GFL_CLACT_WK_SetAnmSeq( wk->cap_light_s[ i ], eANM_SPOT_LIGHT );	
		GFL_CLACT_WK_AddAnmFrame( wk->cap_light_s[ i ] , FX32_ONE);
		CATS_ObjectPaletteOffsetSetCap( wk->cap_light_s[ i ], eBB_OAM_PAL_TD_MANENE );
		GFL_CLACT_WK_SetObjMode( wk->cap_light_s[ i ], GX_OAM_MODE_XLU );
		GFL_CLACT_WK_SetDrawEnable( wk->cap_light_s[ i ], FALSE );
	}
}


//--------------------------------------------------------------
/**
 * @brief	���C�g�폜
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Light_Del( BB_CLIENT* wk )
{
	int i;

	for ( i = 0; i < BB_LIGHT_MAX; i++ ){
		GFL_CLACT_WK_Remove( wk->cap_light_m[ i ] );
		GFL_CLACT_WK_Remove( wk->cap_light_s[ i ] );
	}
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Pen_Add( BB_CLIENT* wk )
{
	int i;
	GFL_CLWK			 cap;
	TCATS_OBJECT_ADD_PARAM_S coap;
	CATS_SYS_PTR			 csp = wk->sys->csp;
	CATS_RES_PTR			 crp = wk->sys->crp;
	PALETTE_FADE_PTR		 pfd = wk->sys->pfd;
	
	BB_disp_COAP_SimpleInit( &coap, 128, 128, CATS_D_AREA_MAIN, eBB_OAM_PAL_BD_PEN, eBB_ID_PEN );
	coap.bg_pri = 0;
	coap.pri	= 0;
	wk->cap_pen = CATS_ObjectAdd_S( csp, crp, &coap );
	GFL_CLACT_WK_SetAnmSeq( wk->cap_pen, 1 );
	GFL_CLACT_WK_SetDrawEnable( wk->cap_pen, TRUE );	
	GFL_CLACT_WK_AddAnmFrame( wk->cap_pen , FX32_ONE);
	CATS_ObjectPaletteOffsetSetCap( wk->cap_pen, eBB_OAM_PAL_BD_PEN );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_Pen_Del( BB_CLIENT* wk )
{
	GFL_CLACT_WK_Remove( wk->cap_pen );
}

//--------------------------------------------------------------
/**
 * @brief	BG �� �W�J
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BB_disp_BG_Load( BB_WORK* wk )
{
	ARCHANDLE*		 hdl_bb = wk->sys.p_handle_bb;
	ARCHANDLE*		 hdl_cr = wk->sys.p_handle_cr;
	
	CATS_SYS_PTR	 csp	= wk->sys.csp;
	CATS_RES_PTR	 crp	= wk->sys.crp;
	PALETTE_FADE_PTR pfd	= wk->sys.pfd;
	
	ARCHANDLE* hdl = hdl_bb;
	
	// ----- ���� -----	
	GFL_ARCHDL_UTIL_TransVramBgCharacter( hdl, NARC_manene_upper_bg_NCGR,   	GFL_BG_FRAME0_S, 0, 0, 0, HEAPID_BB );		///< �w�i
	GFL_ARCHDL_UTIL_TransVramBgCharacter( hdl, NARC_manene_upper_bg_maku_NCGR, GFL_BG_FRAME1_S, 0, 0, 0, HEAPID_BB );		///< ��
	GFL_ARCHDL_UTIL_TransVramScreen( hdl, NARC_manene_upper_bg_00_NSCR,  	GFL_BG_FRAME0_S, 0, 0, 0, HEAPID_BB );
	GFL_ARCHDL_UTIL_TransVramScreen( hdl, NARC_manene_upper_bg_maku1_NSCR, 	GFL_BG_FRAME1_S, 0, 0, 0, HEAPID_BB );
	GFL_ARCHDL_UTIL_TransVramScreen( hdl, NARC_manene_upper_bg_maku2_NSCR, 	GFL_BG_FRAME2_S, 0, 0, 0, HEAPID_BB );
	GFL_ARCHDL_UTIL_TransVramScreen( hdl, NARC_manene_upper_bg_maku3_NSCR, 	GFL_BG_FRAME3_S, 0, 0, 0, HEAPID_BB );
	PaletteWorkSet_Arc( pfd, ARCID_BB_RES, NARC_manene_upper_bg_NCLR, 	HEAPID_BB, FADE_SUB_BG, 0x20 * 5, 0 );	///< 4�{��

	// ----- ����� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter( hdl, NARC_manene_bottom_bg_NCGR,   	GFL_BG_FRAME3_M, 0, 0, 0, HEAPID_BB );		///< �w�i
	GFL_ARCHDL_UTIL_TransVramBgCharacter( hdl, NARC_manene_bottom_bg_maku_NCGR,GFL_BG_FRAME1_M, 0, 0, 0, HEAPID_BB );		///< ��
	GFL_ARCHDL_UTIL_TransVramScreen( hdl, NARC_manene_bottom_bg_NSCR,  		GFL_BG_FRAME3_M, 0, 0, 0, HEAPID_BB );
	GFL_ARCHDL_UTIL_TransVramScreen( hdl, NARC_manene_bottom_bg_maku1_NSCR, GFL_BG_FRAME1_M, 0, 0, 0, HEAPID_BB );
	GFL_ARCHDL_UTIL_TransVramScreen( hdl, NARC_manene_bottom_bg_maku2_NSCR, GFL_BG_FRAME2_M, 0, 0, 0, HEAPID_BB );
	PaletteWorkSet_Arc( pfd, ARCID_BB_RES, NARC_manene_bottom_bg_NCLR, 	HEAPID_BB, FADE_MAIN_BG, 0x20, 0 );

	// ----- �t�H���g -----
	PaletteWorkSet_Arc( pfd, ARCID_FONT, NARC_font_talk_ncrl, HEAPID_BB, FADE_SUB_BG, 0x20, 14 * 16);
	
	// ----- �E�B���h�E -----
	PaletteWorkSet_Arc( pfd, ARC_WINFRAME, MenuWinPalArcGet(), HEAPID_BB, FADE_SUB_BG, 0x20, 13 * 16);	
	BmpWinFrame_GraphicSet( GFL_BG_FRAME3_S, BB_BG_CGX_OFS, 13, 0, HEAPID_BB );
}
