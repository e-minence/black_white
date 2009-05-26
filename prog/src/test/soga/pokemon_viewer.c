//============================================================================================
/**
 * @file	pokemon_viewer.c
 * @brief	MCS�𗘗p���ă|�P����������r���[���[
 * @author	soga
 * @date	2009.04.01
 */
//============================================================================================

#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>

#include "system/main.h"
#include "arc_def.h"

#include "battle/btlv/btlv_effect.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "gf_mcs.h"

#include "print/printsys.h"
#include "print/wordset.h"
#include "msg/msg_d_soga.h"
#include "font/font.naix"
#include "message.naix"

#include "pokemon_viewer_def.h"


//============================================================================================
//	�萔��`
//============================================================================================

#define	PAD_BUTTON_EXIT	( PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START )

#define	CAMERA_SPEED		( FX32_ONE * 2 )
#define	MOVE_SPEED			( FX32_ONE >> 2 )
#define	ROTATE_SPEED		( 65536 / 128 ) 
#define	PHI_MAX				( ( 65536 / 360 ) * 89 )
#define	DEFAULT_PHI			( 65536 / 12 )
#define	DEFAULT_THETA		( 65536 / 4 )
#define	DEFAULT_SCALE		( FX32_ONE * 16  )
#define	SCALE_SPEED			( FX32_ONE )
#define	SCALE_MAX			( FX32_ONE * 200 )

#define	HEAD_SIZE			( 4 )

#define	MCS_READ_CH			( 0 )
#define	MCS_WRITE_CH		( 0 )

#define	MONS_INFO_X			( 74 )
#define	MONS_INFO_Y			( 96 - 6 )

#define	NEW_POKEMON_START	( 500 )
#define	NEW_POKEMON_END	( NEW_POKEMON_START + NEW_POKEMON_COUNT - 1 )

enum{
	BACK_COL = 0,
	SHADOW_COL = 2,
	LETTER_COL_NORMAL = 1,
	LETTER_COL_SELECT,
	LETTER_COL_CURSOR,
};

#define G2D_BACKGROUND_COL	( GX_RGB( 31, 31, 31 ) )
#define G2D_FONT_COL				( GX_RGB(  0,  0,  0 ) )
#define G2D_FONT_SELECT_COL	( GX_RGB(  0, 31,  0 ) )
#define G2D_FONT_CURSOR_COL	( GX_RGB( 31,  0,  0 ) )
#define G2D_AA_BGCOL_ON			( GX_RGB( 31,  0,  0 ) )
#define G2D_BB_BGCOL_ON			( GX_RGB(  0, 31,  0 ) )
#define G2D_A_BGCOL_ON			( GX_RGB(  0,  0, 31 ) )
#define G2D_B_BGCOL_ON			( GX_RGB( 31, 31,  0 ) )
#define G2D_C_BGCOL_ON			( GX_RGB( 31,  0, 31 ) )
#define G2D_D_BGCOL_ON			( GX_RGB(  0, 31, 31 ) )
#define G2D_AA_BGCOL_OFF		( GX_RGB( 15,  0,  0 ) )
#define G2D_BB_BGCOL_OFF		( GX_RGB(  0, 15,  0 ) )
#define G2D_A_BGCOL_OFF			( GX_RGB(  0,  0, 15 ) )
#define G2D_B_BGCOL_OFF			( GX_RGB( 15, 15,  0 ) )
#define G2D_C_BGCOL_OFF			( GX_RGB( 15,  0, 15 ) )
#define G2D_D_BGCOL_OFF			( GX_RGB(  0, 15, 15 ) )


enum{
	BGCOL_AA = 4,
	BGCOL_BB,
	BGCOL_A,
	BGCOL_B,
	BGCOL_C,
	BGCOL_D,
};

typedef struct{
	u8 posx;
	u8 posy;
	u8 sizx;
	u8 sizy;

	u8 palnum;
	u8 msgx;
	u8 msgy;
	u8 padding;
}BMP_CREATE_TABLE;

enum{
	SEND_NCBR = 0,
	SEND_NCBR_OK,
	SEND_NCLR,
	SEND_NCLR_OK,
	SEND_NCER,
	SEND_NCER_OK,
	SEND_NMCR,
	SEND_NMCR_OK,
	SEND_NANR,
	SEND_NANR_OK,
	SEND_NMAR,
	SEND_NMAR_OK,
	SEND_NCEC,
	SEND_NCEC_OK,
	SEND_POS,
	SEND_POS_OK,
	SEND_CLEAR,
	SEND_CLEAR_OK,
	SEND_IDLE,
};

enum{
	RESOURCE_NCBR,
	RESOURCE_NCLR,
	RESOURCE_NCER,
	RESOURCE_NMCR,
	RESOURCE_NANR,
	RESOURCE_NMAR,
	RESOURCE_NCEC,
	RESOURCE_MAX,
};

enum{
	SEQ_IDLE = 0,
	SEQ_LOAD_POS_DATA,
	SEQ_LOAD_RESOURCE_DATA,
	SEQ_CLEAR,
};

enum{
	BMPWIN_AA = 0,
	BMPWIN_BB,
	BMPWIN_A,
	BMPWIN_B,
	BMPWIN_C,
	BMPWIN_D,

	BMPWIN_MAX
};

//============================================================================================
//	�\���̒�`
//============================================================================================

typedef struct
{
	HEAPID								heapID;
	GFL_MSGDATA*					msg;
	GFL_FONT*							font;

	int										seq_no;
	int										mcs_enable;
	POKEMON_PARAM*				pp;
	int										mons_no[ BTLV_MCSS_POS_MAX ];
	fx32									scale[ BTLV_MCSS_PROJ_MAX ][ BTLV_MCSS_POS_MAX ];
	GFL_BMPWIN*						bmpwin[ BMPWIN_MAX ];
	GFL_BMPWIN*						bmpwin2;
	int										key_repeat_speed;
	int										key_repeat_wait;
	int										read_position;
	int										read_resource;
	int										add_pokemon_req;
	int										edit_mode;
	int										edit_pos;
	int										edit_type;
	int										edit_keta;
	BTLV_MCSS_PROJECTION	proj;

	void*									resource_data[ RESOURCE_MAX ][ BTLV_MCSS_POS_MAX ];
}POKEMON_VIEWER_WORK;

//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

static	void	PokemonViewerSequence( POKEMON_VIEWER_WORK *pvw );
static	BOOL	PokemonViewerSubSequence( POKEMON_VIEWER_WORK *pvw );
static	void	PokemonViewerRead( POKEMON_VIEWER_WORK *pvw );
static	void	PokemonViewerPositionLoad( POKEMON_VIEWER_WORK *pvw );
static	void	PokemonViewerResourceLoad( POKEMON_VIEWER_WORK *pvw );
static	void	PokemonViewerAddPokemon( POKEMON_VIEWER_WORK *pvw );
static	void	PokemonViewerCameraWork( POKEMON_VIEWER_WORK *pvw );

static	void	TextPrint( POKEMON_VIEWER_WORK *pvw, int num, int bmpwin_num );
static	void	PokemonViewerDrawInfo( POKEMON_VIEWER_WORK *pvw, BtlvMcssPos pos );
static	void	PokemonViewerPP_Put( POKEMON_VIEWER_WORK *pvw, int mons_no );

static	void	MoveCamera( POKEMON_VIEWER_WORK *pvw );

static	void	set_pokemon( POKEMON_VIEWER_WORK *pvw, BtlvMcssPos pos );
#if 0
static	void	del_pokemon( POKEMON_VIEWER_WORK *pvw );
#endif

//============================================================================================
//	�f�[�^
//============================================================================================

static	const	int	pokemon_pos_table[][2]={ 
	{ BTLV_MCSS_POS_AA, BTLV_MCSS_POS_BB },
	{ BTLV_MCSS_POS_A, BTLV_MCSS_POS_B },
	{ BTLV_MCSS_POS_C, BTLV_MCSS_POS_D }
};

static	const	u32	btlv_mcss_pos_msg[BMPWIN_MAX]={
	PVMSG_POS_AA,
	PVMSG_POS_BB,
	PVMSG_POS_A,
	PVMSG_POS_B,
	PVMSG_POS_C,
	PVMSG_POS_D,
};

static const BMP_CREATE_TABLE bmp_create_table[] = {
	//AA
	{ 11, 12, 10, 12, BGCOL_AA, 12, 40, 0 }, 
	//BB
	{ 11,  0, 10, 12, BGCOL_BB, 12, 40, 0 }, 
	//A
	{  0, 12, 11, 12, BGCOL_A,  20, 40, 0 }, 
	//B
	{ 21,  0, 11, 12, BGCOL_B,  20, 40, 0 }, 
	//C
	{ 21, 12, 11, 12, BGCOL_C,  20, 40, 0 }, 
	//D
	{  0,  0, 11, 12, BGCOL_D,  20, 40, 0 }, 
};

static const GFL_UI_TP_HITTBL TP_HitTbl[] = {
	{  96, 191,  88, 167 },
	{   0,  95,  88, 167 },
	{  96, 191,   0,  87 },
	{   0,  95, 168, 255 },
	{  96, 191, 168, 255 },
	{   0,  95,   0,  87 },
	{ GFL_UI_TP_HIT_END, 0, 0, 0 },
};

FS_EXTERN_OVERLAY(battle);

//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT PokemonViewerProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	POKEMON_VIEWER_WORK *pvw;
	static const GFL_DISP_VRAM dispvramBank = {
		GX_VRAM_BG_128_D,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_01_AB,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g			
		GX_OBJVRAMMODE_CHAR_1D_64K,		// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};		

	GFL_OVERLAY_Load(FS_OVERLAY_ID(battle));

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SOGABE_DEBUG, 0xc0000 );
	pvw = GFL_PROC_AllocWork( proc, sizeof( POKEMON_VIEWER_WORK ), HEAPID_SOGABE_DEBUG );
	MI_CpuClearFast( pvw, sizeof( POKEMON_VIEWER_WORK ) );
	pvw->heapID = HEAPID_SOGABE_DEBUG;
		
	GFL_DISP_SetBank( &dispvramBank );

	//VRAM�N���A
	MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
	MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
	MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
	MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	MI_CpuFill16((void*)HW_BG_PLTT, 0x0000, HW_BG_PLTT_SIZE);

	G2_BlendNone();
	GFL_BG_Init( pvw->heapID );
	GFL_BMPWIN_Init( pvw->heapID );

	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_3D,
		};
		GFL_BG_SetBGMode( &sysHeader );
	}
		
	{
		///< main
		GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

		///< sub
		GFL_BG_SetVisible( GFL_BG_FRAME0_S,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME1_S,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME2_S,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_S,   VISIBLE_OFF );
		
	}		
	GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

	//3D�֘A������
	{
		GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0, pvw->heapID, NULL );
		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
		G3X_AlphaBlend( TRUE );
		G3X_EdgeMarking( TRUE );
		G3X_AntiAlias( TRUE );
		GFL_BG_SetBGControl3D( 1 );
	}

	//�퓬�G�t�F�N�g������
	{
		BTLV_EFFECT_Init( 0, &dispvramBank, pvw->heapID );
	}

//	set_pokemon( pvw );

	//2D��ʏ�����
	{
		GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME2_M
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME1_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME2_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
		};
		GFL_BG_SetBGControl(GFL_BG_FRAME1_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME1_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME3_M, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME1_S, &TextBgCntDat[3], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME1_S );
		GFL_BG_SetBGControl(GFL_BG_FRAME2_S, &TextBgCntDat[4], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_S );
	}

	{
		int	i;

		//�t�H���g�p���b�g�쐬���]��
		{
			static	u16 plt[16] = {
				G2D_BACKGROUND_COL, G2D_FONT_COL, G2D_FONT_SELECT_COL, G2D_FONT_CURSOR_COL,
				G2D_AA_BGCOL_ON, G2D_BB_BGCOL_ON, G2D_A_BGCOL_ON, G2D_B_BGCOL_ON, G2D_C_BGCOL_ON, G2D_D_BGCOL_ON,
				G2D_AA_BGCOL_OFF, G2D_BB_BGCOL_OFF, G2D_A_BGCOL_OFF, G2D_B_BGCOL_OFF, G2D_C_BGCOL_OFF, G2D_D_BGCOL_OFF };
//			GFL_BG_LoadPalette( GFL_BG_FRAME1_S, &plt, 16*2, 0 );
				PaletteWorkSet( BTLV_EFFECT_GetPfd(), &plt, FADE_SUB_BG, 0, 16 * 2 );
		}

		//���b�Z�[�W�n������
		GFL_FONTSYS_Init();
		pvw->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_soga_dat, pvw->heapID );
		pvw->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, TRUE, pvw->heapID );

		for( i = 0 ; i < BMPWIN_MAX ; i++ ){
			pvw->bmpwin[ i ] = GFL_BMPWIN_Create( GFL_BG_FRAME1_S,
												 bmp_create_table[ i ].posx,
												 bmp_create_table[ i ].posy,
												 bmp_create_table[ i ].sizx,
												 bmp_create_table[ i ].sizy,
												 0,
												 GFL_BG_CHRAREA_GET_B );
			TextPrint( pvw, i, i );
		}

		pvw->bmpwin2 = GFL_BMPWIN_Create( GFL_BG_FRAME2_S, 0, 0, 32, 24, 0, GFL_BG_CHRAREA_GET_B );

	}

	//�E�C���h�}�X�N�ݒ�i��ʗ��[�̃G�b�W�}�[�L���O�̃S�~�������j
	{
		G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 |
							   GX_WND_PLANEMASK_BG1 |
							   GX_WND_PLANEMASK_BG2 |
							   GX_WND_PLANEMASK_BG3 |
							   GX_WND_PLANEMASK_OBJ,
							   FALSE );
		G2_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, FALSE );
		G2_SetWnd0Position( 1, 1, 255, 191 );
		GX_SetVisibleWnd( GX_WNDMASK_W0 );
	}

	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x0000 );
	
	//�t�F�[�h�C��
	GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, 2 );

	GFL_UI_KEY_GetRepeatSpeed( &pvw->key_repeat_speed, &pvw->key_repeat_wait );
	GFL_UI_KEY_SetRepeatSpeed( pvw->key_repeat_speed / 4, pvw->key_repeat_wait );

	//POKEMON_PARAM����
	pvw->pp = GFL_HEAP_AllocMemory( pvw->heapID, POKETOOL_GetWorkSize() );
	PP_SetupEx( pvw->pp, 0, 0, 0, 0, 255 );

	{	
		int pos;

		for ( pos = BTLV_MCSS_POS_AA ; pos < BTLV_MCSS_POS_D + 1 ; pos++ )
		{	
			pvw->scale[ BTLV_MCSS_PROJ_PERSPECTIVE ][ pos ] = BTLV_MCSS_GetPokeDefaultScaleEx( BTLV_EFFECT_GetMcssWork(), pos,
																																												 BTLV_MCSS_PROJ_PERSPECTIVE );
			pvw->scale[ BTLV_MCSS_PROJ_ORTHO ][ pos ] = BTLV_MCSS_GetPokeDefaultScaleEx( BTLV_EFFECT_GetMcssWork(), pos,
																																									 BTLV_MCSS_PROJ_ORTHO );
		}
	}

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT PokemonViewerProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	int pad = GFL_UI_KEY_GetCont();
	int trg = GFL_UI_KEY_GetTrg();
	int rep = GFL_UI_KEY_GetRepeat();
	POKEMON_VIEWER_WORK *pvw = mywk;
	int	i, j;

	if( pvw->mcs_enable ){
		MCS_Main();
	}
	else
	{	
		if( PokemonViewerSubSequence( pvw ) == TRUE )
		{	
			return GFL_PROC_RES_CONTINUE;	
		}
	}

	if( ( trg & PAD_BUTTON_START ) && ( pvw->edit_mode == 0 ) )
	{	
		if( pvw->mcs_enable )
		{
			pvw->seq_no = SEQ_IDLE;
			MCS_Exit();
			pvw->mcs_enable = 0;
			for( i = 0 ; i < BTLV_MCSS_POS_MAX ; i++ )
			{
				for( j = 0 ; j < RESOURCE_MAX ; j++ )
				{
					if( pvw->resource_data[ j ][ i ] )
					{
						if( j == RESOURCE_NCBR )
						{
							BTLV_EFFECT_DelPokemon( i );
						}
						pvw->resource_data[ j ][ i ] = NULL;
					}
				}
			}
		}
		else
		{
			if( MCS_Init( pvw->heapID ) == FALSE )
			{
				pvw->mcs_enable = 1;
			}
		}
	}
	if( (trg & PAD_BUTTON_SELECT ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) )
	{
		PokemonViewerCameraWork( pvw );
	}

	if( (trg & PAD_BUTTON_DEBUG ) )
	{	
		int	mcss_pos;
		VecFx32	pos,target;
		VecFx32	scale_value[] = {
			{ FX32_ONE * 16 * 2, FX32_ONE * 16 * 2, 0 },
			{ FX32_ONE * 16, FX32_ONE * 16, 0 }
		};

		BTLV_CAMERA_GetDefaultCameraPosition( &pos, &target );
		BTLV_CAMERA_MoveCameraInterpolation( BTLV_EFFECT_GetCameraWork(), &pos, &target, 20, 0, 20 );

		for( mcss_pos = BTLV_MCSS_POS_AA ; mcss_pos < BTLV_MCSS_POS_MAX ; mcss_pos++ )
		{
			if( BTLV_EFFECT_CheckExistPokemon( mcss_pos ) == TRUE )
			{
				BTLV_MCSS_MoveScale( BTLV_EFFECT_GetMcssWork(),
									 mcss_pos,
									 EFFTOOL_CALCTYPE_INTERPOLATION,
									 &scale_value[ mcss_pos & 1 ],
									 10, 1, 0 );
			}
		}
	}

	{
		int hit = GFL_UI_TP_HitTrg( TP_HitTbl );
		if( hit != GFL_UI_TP_HIT_NONE ){
			if( BTLV_EFFECT_CheckExistPokemon( hit ) == TRUE ){
				BTLV_MCSS_SetVanishFlag( BTLV_EFFECT_GetMcssWork(), hit, BTLV_MCSS_VANISH_FLIP );
				TextPrint( pvw, hit, hit );
			}
		}
	}

	PokemonViewerSequence( pvw );

	MoveCamera( pvw );
	BTLV_EFFECT_Main();

	if( pad == PAD_BUTTON_EXIT ){
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, 2 );
		return GFL_PROC_RES_FINISH;	
	}
	else{
		return GFL_PROC_RES_CONTINUE;	
	}
}
//--------------------------------------------------------------------------
/**
 * PROC Exit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT PokemonViewerProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	POKEMON_VIEWER_WORK *pvw = mywk;
	int	i;

	if( GFL_FADE_CheckFade() == TRUE ){
		return GFL_PROC_RES_CONTINUE;	
	}

	GFL_UI_KEY_SetRepeatSpeed( pvw->key_repeat_speed, pvw->key_repeat_wait );

	GFL_HEAP_FreeMemory( pvw->pp );

	BTLV_EFFECT_Exit();

	GFL_G3D_Exit();

	GFL_MSG_Delete( pvw->msg );
	GFL_FONT_Delete( pvw->font );

	for( i = 0 ; i < BMPWIN_MAX ; i++ ){
		GFL_BMPWIN_Delete( pvw->bmpwin[ i ] );
	}
	GFL_BMPWIN_Delete( pvw->bmpwin2 );

	GFL_BG_Exit();
	GFL_BMPWIN_Exit();

	GFL_PROC_FreeWork( proc );

	GFL_HEAP_DeleteHeap( HEAPID_SOGABE_DEBUG );

	GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA		PokemonViewerProcData = {
	PokemonViewerProcInit,
	PokemonViewerProcMain,
	PokemonViewerProcExit,
};

//======================================================================
//	���C���V�[�P���X
//======================================================================
static	void	PokemonViewerSequence( POKEMON_VIEWER_WORK *pvw )
{
	switch( pvw->seq_no ){
	default:
	case SEQ_IDLE:
		PokemonViewerAddPokemon( pvw );
		PokemonViewerRead( pvw );
		break;
	case SEQ_LOAD_POS_DATA:
		PokemonViewerPositionLoad( pvw );
		break;
	case SEQ_LOAD_RESOURCE_DATA:
		PokemonViewerResourceLoad( pvw );
		break;
	case SEQ_CLEAR:
		{
			int head = SEND_IDLE;
			int	mcss_pos;
			int	res;

			MCS_Write( MCS_WRITE_CH, &head, 4 );
			pvw->seq_no = SEQ_IDLE;

			for( mcss_pos = BTLV_MCSS_POS_AA ; mcss_pos < BTLV_MCSS_POS_MAX ; mcss_pos++ ){
				if( BTLV_EFFECT_CheckExistPokemon( mcss_pos ) == TRUE ){
					BTLV_EFFECT_DelPokemon( mcss_pos );
					for( res = RESOURCE_NCBR ; res < RESOURCE_MAX ; res++ ){	
						//���\�[�X���̂̉���́AMCSS_Del�̒��ł���Ă���̂ŁANULL�����������s��
						pvw->resource_data[ res ][ mcss_pos ] = NULL;
					}
				}
				if( mcss_pos < BMPWIN_MAX ){
					TextPrint( pvw, mcss_pos, mcss_pos );
				}
			}
		}
		break;
	}
}

//======================================================================
//	�����ɓ����Ă���|�P�������{�����郂�[�h
//======================================================================
static	BOOL	PokemonViewerSubSequence( POKEMON_VIEWER_WORK *pvw )
{	
	BOOL	ret = FALSE;
	if( pvw->edit_mode )
	{	
		int rep = GFL_UI_KEY_GetRepeat();
		int cont = GFL_UI_KEY_GetCont();
		int trg = GFL_UI_KEY_GetTrg();
		
		if( trg & PAD_BUTTON_X )
		{	
			pvw->edit_type ^= 1;
		}
		if( pvw->edit_type == 0 )
		{	
			if( rep & PAD_KEY_UP )
			{	
				pvw->mons_no[ pvw->edit_pos ]++;
			}
			if( rep & PAD_KEY_DOWN )
			{	
				pvw->mons_no[ pvw->edit_pos ]--;
			}
			if( rep & PAD_KEY_LEFT )
			{	
				pvw->mons_no[ pvw->edit_pos ] -= 10;
			}
			if( rep & PAD_KEY_RIGHT )
			{	
				pvw->mons_no[ pvw->edit_pos ] += 10;
			}
			if( rep & PAD_BUTTON_L )
			{	
				pvw->mons_no[ pvw->edit_pos ] -= 100;
			}
			if( rep & PAD_BUTTON_R )
			{	
				pvw->mons_no[ pvw->edit_pos ] += 100;
			}
#if 0
			if( pvw->mons_no[ pvw->edit_pos ] < 0 )
			{	
				pvw->mons_no[ pvw->edit_pos ] += NEW_POKEMON_END + 1;
			}
			if( pvw->mons_no[ pvw->edit_pos ] > NEW_POKEMON_END )
			{	
				pvw->mons_no[ pvw->edit_pos ] -= NEW_POKEMON_END + 1;
			}
#else
			if( pvw->mons_no[ pvw->edit_pos ] < 0 )
			{	
				pvw->mons_no[ pvw->edit_pos ] += MONSNO_END + 1;
			}
			if( pvw->mons_no[ pvw->edit_pos ] > MONSNO_END )
			{	
				pvw->mons_no[ pvw->edit_pos ] -= MONSNO_END + 1;
			}
#endif
			if( rep != 0 )
			{	
				if( BTLV_EFFECT_CheckExistPokemon( pvw->edit_pos ) == TRUE ){
						BTLV_EFFECT_DelPokemon( pvw->edit_pos );
				}
				PokemonViewerPP_Put( pvw, pvw->mons_no[ pvw->edit_pos ] );
				set_pokemon( pvw, pvw->edit_pos );
			}
		}
		else
		{	
			if( rep & PAD_KEY_UP )
			{	
				pvw->scale[ pvw->proj ][ pvw->edit_pos ] += ( 1 << ( pvw->edit_keta * 4 ) );
			}
			if( rep & PAD_KEY_DOWN )
			{	
				pvw->scale[pvw->proj ][ pvw->edit_pos ] -= ( 1 << ( pvw->edit_keta * 4 ) );
			}
			if( ( rep & PAD_KEY_LEFT ) && ( pvw->edit_keta < 7 ) )
			{	
				pvw->edit_keta++;
			}
			if( ( rep & PAD_KEY_RIGHT ) && ( pvw->edit_keta > 0 ) )
			{	
				pvw->edit_keta--;
			}
			if( trg & PAD_BUTTON_SELECT )
			{	
				pvw->scale[ pvw->proj ][ pvw->edit_pos ] = BTLV_MCSS_GetPokeDefaultScaleEx( BTLV_EFFECT_GetMcssWork(), pvw->edit_pos,
																																									  pvw->proj );
			}
			if( trg & PAD_BUTTON_Y )
			{	
				VecFx32	scale;
				pvw->proj ^= 1;
				if( pvw->proj )
				{	
					BTLV_MCSS_SetOrthoMode( BTLV_EFFECT_GetMcssWork() );
				}
				else
				{	
					BTLV_MCSS_ResetOrthoMode( BTLV_EFFECT_GetMcssWork() );
				}
				VEC_Set( &scale,
								 pvw->scale[ pvw->proj ][ pvw->edit_pos ],
								 pvw->scale[ pvw->proj ][ pvw->edit_pos ],
								 FX32_ONE );
				BTLV_MCSS_SetScale( BTLV_EFFECT_GetMcssWork(), pvw->edit_pos, &scale );
			}
			if( rep != 0 )
			{	
				if( BTLV_EFFECT_CheckExistPokemon( pvw->edit_pos ) == TRUE ){
					VecFx32	scale;
					VEC_Set( &scale,
									 pvw->scale[ pvw->proj ][ pvw->edit_pos ],
									 pvw->scale[ pvw->proj ][ pvw->edit_pos ],
									 FX32_ONE );
					BTLV_MCSS_SetScale( BTLV_EFFECT_GetMcssWork(), pvw->edit_pos, &scale );
				}
			}
		}
		if( trg & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
		{	
			pvw->edit_mode = 0;
			GFL_BG_SetVisible( GFL_BG_FRAME1_S,   VISIBLE_ON );
			GFL_BG_SetVisible( GFL_BG_FRAME2_S,   VISIBLE_OFF );
		}
		if( cont != 0)
		{	
			ret = TRUE;
		}
	}
	else
	{	
		int hit = GFL_UI_TP_HitTrg( TP_HitTbl );
		if( hit != GFL_UI_TP_HIT_NONE ){
			pvw->edit_mode = 1;
			pvw->edit_pos = hit;
			pvw->edit_type = 0;
			pvw->edit_keta = 0;
			pvw->proj = BTLV_MCSS_PROJ_ORTHO;
			PokemonViewerPP_Put( pvw, pvw->mons_no[ hit ] );
			GFL_BG_SetVisible( GFL_BG_FRAME1_S,   VISIBLE_OFF );
			GFL_BG_SetVisible( GFL_BG_FRAME2_S,   VISIBLE_ON );
			ret = TRUE;
		}
	}

	if( ret == TRUE )
	{	
		PokemonViewerDrawInfo( pvw, pvw->edit_pos );
	}

	return ret;
}

//======================================================================
//	MCS����̓ǂݍ��݃`�F�b�N
//======================================================================
static	void	PokemonViewerRead( POKEMON_VIEWER_WORK *pvw )
{
	u32	size;
	u32	head;

	if( pvw->mcs_enable == 0 ){
		return;
	}

	size = MCS_CheckRead();

	if( size ){
		MCS_Read( &head, HEAD_SIZE );
		if( head == SEND_CLEAR ){
			pvw->seq_no = SEQ_CLEAR;
		}
		else if( head == SEND_POS ){
			pvw->seq_no = SEQ_LOAD_POS_DATA;
		}
		else{
			pvw->seq_no = SEQ_LOAD_RESOURCE_DATA;
		}
		if( head == SEND_NCEC ){
			pvw->add_pokemon_req = 1;
		}
		pvw->read_resource = head / 2;
		head++;
		MCS_Write( MCS_WRITE_CH, &head, 4 );
	}
}

//======================================================================
//	MCS����̃|�W�V�����f�[�^�ǂݍ��݃`�F�b�N
//======================================================================
static	void	PokemonViewerPositionLoad( POKEMON_VIEWER_WORK *pvw )
{
	u32	size;
	u32	head;
	int	res;

	size = MCS_CheckRead();

	if( size ){
		MCS_Read( &pvw->read_position, size );
		head = SEND_IDLE;
		MCS_Write( MCS_WRITE_CH, &head, 4 );
		pvw->seq_no = SEQ_IDLE;
		if( BTLV_EFFECT_CheckExistPokemon( pvw->read_position ) == TRUE ){
			BTLV_EFFECT_DelPokemon( pvw->read_position );
			for( res = RESOURCE_NCBR ; res < RESOURCE_MAX ; res++ ){	
				//���\�[�X���̂̉���́AMCSS_Del�̒��ł���Ă���̂ŁANULL�����������s��
				pvw->resource_data[ res ][ pvw->read_position ] = NULL;
			}
		}
	}
}

//======================================================================
//	MCS����̃V�[�P���X�ǂݍ��݃`�F�b�N
//======================================================================
static	void	PokemonViewerResourceLoad( POKEMON_VIEWER_WORK *pvw )
{
	u32	size;
	u32	head;

	size = MCS_CheckRead();

	if( size ){
		pvw->resource_data[ pvw->read_resource ][ pvw->read_position ] = GFL_HEAP_AllocClearMemory( pvw->heapID, size );
		MCS_Read( pvw->resource_data[ pvw->read_resource ][ pvw->read_position ], size );
		head = SEND_IDLE;
		MCS_Write( MCS_WRITE_CH, &head, 4 );
		pvw->seq_no = SEQ_IDLE;
	}
}

//======================================================================
//	�e�L�X�g�\��
//======================================================================
static	void	TextPrint( POKEMON_VIEWER_WORK *pvw, int num, int bmpwin_num )
{
	int	flag = 0;
	STRBUF	*strbuf;

	if( BTLV_EFFECT_CheckExistPokemon( num ) == TRUE ){
	   flag = BTLV_MCSS_GetVanishFlag( BTLV_EFFECT_GetMcssWork(), num );
	}

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp( pvw->bmpwin[ bmpwin_num ] ), bmp_create_table[ bmpwin_num ].palnum + 6 * flag );

	GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, bmp_create_table[ bmpwin_num ].palnum + 6 * flag );

	strbuf = GFL_MSG_CreateString( pvw->msg,  btlv_mcss_pos_msg[ num ] );
	PRINTSYS_Print( GFL_BMPWIN_GetBmp( pvw->bmpwin[ bmpwin_num ] ),
					bmp_create_table[ bmpwin_num ].msgx,
					bmp_create_table[ bmpwin_num ].msgy,
					strbuf, pvw->font );
	GFL_HEAP_FreeMemory( strbuf );

	GFL_BMPWIN_TransVramCharacter( pvw->bmpwin[ bmpwin_num ] );
	GFL_BMPWIN_MakeScreen( pvw->bmpwin[ bmpwin_num ] );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME1_S );
}

//======================================================================
//	�e�L�X�g�\��
//======================================================================
static	void	PokemonViewerDrawInfo( POKEMON_VIEWER_WORK *pvw, BtlvMcssPos pos )
{	
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp( pvw->bmpwin2 ), 0 );

	if( pvw->edit_type == 0 )
	{	
		STRBUF	*str_src;
		STRBUF	*str_dst = GFL_STR_CreateBuffer( 100, pvw->heapID );
		WORDSET*  mons_info = WORDSET_Create( pvw->heapID );

		WORDSET_RegisterNumber( mons_info, 0, pvw->mons_no[ pos ], 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
		if( pvw->mons_no[ pos ] <= MONSNO_END )
		{	
			WORDSET_RegisterPokeMonsName( mons_info, 1, pvw->pp );
		}
		str_src = GFL_MSG_CreateString( pvw->msg,  PVMSG_MONSNAME );
		WORDSET_ExpandStr( mons_info, str_dst, str_src );
		PRINTSYS_Print( GFL_BMPWIN_GetBmp( pvw->bmpwin2 ), MONS_INFO_X, MONS_INFO_Y, str_dst, pvw->font );
		GFL_HEAP_FreeMemory( str_src );
		GFL_HEAP_FreeMemory( str_dst );
		WORDSET_Delete( mons_info );
	}
	else
	{	
		STRBUF	*strbuf;
		int			keta, ofsx = 0;
		u32			num;

		for( keta = 7 ; keta >= 0 ; keta-- ){
			if( keta == pvw->edit_keta )
			{
				GFL_FONTSYS_SetColor( LETTER_COL_SELECT, SHADOW_COL, BACK_COL );
			}
			else
			{
				GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );
			}
			num = ( pvw->scale[ pvw->proj ][ pos ] & ( 0x0000000f << ( keta * 4 ) ) ) >> ( keta * 4 );
			strbuf = GFL_MSG_CreateString( pvw->msg,  EVMSG_NUM0 + num );
			PRINTSYS_Print( GFL_BMPWIN_GetBmp( pvw->bmpwin2 ), MONS_INFO_X + ofsx, MONS_INFO_Y, strbuf, pvw->font );
			GFL_STR_DeleteBuffer( strbuf );
			ofsx += 8;
		}
	}

	GFL_BMPWIN_TransVramCharacter( pvw->bmpwin2 );
	GFL_BMPWIN_MakeScreen( pvw->bmpwin2 );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
}

//======================================================================
//	POKEMON_PARAM�Z�b�g
//======================================================================
static	void	PokemonViewerPP_Put( POKEMON_VIEWER_WORK *pvw, int mons_no )
{	
#if 0
	if( mons_no >= NEW_POKEMON_START )
	{	
		mons_no -= NEW_POKEMON_START;
		mons_no += MONSNO_END + 1;
	}
	else if( mons_no > MONSNO_END )
	{	
		mons_no = 0;
	}
#else
	if( mons_no < 0 )
	{	
		mons_no = MONSNO_END;
	}
	else if( mons_no > MONSNO_END )
	{	
		mons_no = 0;
	}
#endif
	PP_Put( pvw->pp, ID_PARA_monsno, mons_no );
}

//======================================================================
//	�J�����̐���
//======================================================================
static	void	MoveCamera( POKEMON_VIEWER_WORK *pvw )
{
	int pad = GFL_UI_KEY_GetCont();
	VecFx32	pos,ofsx,ofsz,camPos, camTarget;
	fx32	xofs=0,yofs=0,zofs=0;		

	if( pad & PAD_BUTTON_L ){
		if( pad & PAD_KEY_LEFT ){
			BTLV_CAMERA_MoveCameraAngle( BTLV_EFFECT_GetCameraWork(), 0, -ROTATE_SPEED );
		}
		if( pad & PAD_KEY_RIGHT ){
			BTLV_CAMERA_MoveCameraAngle( BTLV_EFFECT_GetCameraWork(), 0,  ROTATE_SPEED );
		}
		if( pad & PAD_KEY_UP ){
			BTLV_CAMERA_MoveCameraAngle( BTLV_EFFECT_GetCameraWork(), -ROTATE_SPEED, 0 );
		}
		if( pad & PAD_KEY_DOWN ){
			BTLV_CAMERA_MoveCameraAngle( BTLV_EFFECT_GetCameraWork(),  ROTATE_SPEED, 0 );
		}
	}
	else{
		BTLV_CAMERA_GetCameraPosition( BTLV_EFFECT_GetCameraWork(), &camPos, &camTarget );

		if( pad & PAD_KEY_LEFT ){
			xofs = -MOVE_SPEED;
		}
		if( pad & PAD_KEY_RIGHT ){
			xofs = MOVE_SPEED;
		}

		if( pad & PAD_BUTTON_R ){
			if( pad & PAD_KEY_UP ){
				zofs = MOVE_SPEED;
			}
			if( pad & PAD_KEY_DOWN ){
				zofs = -MOVE_SPEED;
			}
		}
		else{
			if( pad & PAD_KEY_UP ){
				yofs = MOVE_SPEED;
			}
			if( pad & PAD_KEY_DOWN ){
				yofs = -MOVE_SPEED;
			}
		}
		pos.x = camPos.x - camTarget.x;
		pos.y = 0;
		pos.z = camPos.z - camTarget.z;
		VEC_Normalize( &pos, &pos );

		ofsx.x = FX_MUL( pos.z, xofs );
		ofsx.y = 0;
		ofsx.z = -FX_MUL( pos.x, xofs );

		ofsz.x = -FX_MUL( pos.x, zofs );
		ofsz.y = yofs;
		ofsz.z = -FX_MUL( pos.z, zofs );

		camPos.x += ofsx.x + ofsz.x;
		camPos.y += ofsx.y + ofsz.y;
		camPos.z += ofsx.z + ofsz.z;
		camTarget.x += ofsx.x + ofsz.x;
		camTarget.y += ofsx.y + ofsz.y;
		camTarget.z += ofsx.z + ofsz.z;

		BTLV_CAMERA_MoveCameraPosition( BTLV_EFFECT_GetCameraWork(), &camPos, &camTarget );
	}

}

static	void	PokemonViewerAddPokemon( POKEMON_VIEWER_WORK *pvw )
{
	MCSS_ADD_DEBUG_WORK madw;

	if( pvw->add_pokemon_req ){
		pvw->add_pokemon_req = 0;
		madw.ncbr = pvw->resource_data[ RESOURCE_NCBR ][ pvw->read_position ];
		madw.nclr = pvw->resource_data[ RESOURCE_NCLR ][ pvw->read_position ];
		madw.ncer = pvw->resource_data[ RESOURCE_NCER ][ pvw->read_position ];
		madw.nmcr = pvw->resource_data[ RESOURCE_NMCR ][ pvw->read_position ];
		madw.nanr = pvw->resource_data[ RESOURCE_NANR ][ pvw->read_position ];
		madw.nmar = pvw->resource_data[ RESOURCE_NMAR ][ pvw->read_position ];
		madw.ncec = pvw->resource_data[ RESOURCE_NCEC ][ pvw->read_position ];

		BTLV_EFFECT_SetPokemonDebug( &madw, pvw->read_position );
	}
}

static	void	PokemonViewerCameraWork( POKEMON_VIEWER_WORK *pvw )
{
	int	mcss_pos;
	VecFx32	pos,target;
	VecFx32	scale_value[] = {
		{ FX32_ONE * 16, FX32_ONE * 16, 0 },
		{ FX32_ONE * 16 / 2, FX32_ONE * 16 / 2, 0 },
	};

	BTLV_CAMERA_GetDefaultCameraPosition( &pos, &target );
	target.x += FX32_ONE * 4;
	target.y += FX32_ONE * 1;
	target.z += FX32_ONE * 16;
	pos.x += FX32_ONE * 4;
	pos.y += FX32_ONE * 1;
	pos.z += FX32_ONE * 16;
	BTLV_CAMERA_MoveCameraInterpolation( BTLV_EFFECT_GetCameraWork(), &pos, &target, 32, 0, 32 );

	for( mcss_pos = BTLV_MCSS_POS_AA ; mcss_pos < BTLV_MCSS_POS_MAX ; mcss_pos++ ){
		if( BTLV_EFFECT_CheckExistPokemon( mcss_pos ) == TRUE ){
			BTLV_MCSS_MoveScale( BTLV_EFFECT_GetMcssWork(),
								 mcss_pos,
								 EFFTOOL_CALCTYPE_INTERPOLATION,
								 &scale_value[ mcss_pos & 1 ],
								 16, 1, 0 );
		}
	}
}
 
static	void	set_pokemon( POKEMON_VIEWER_WORK *pvw, BtlvMcssPos pos )
{
	int mons_no = PP_Get( pvw->pp, ID_PARA_monsno, NULL );

	if( mons_no != 0 )
	{	
		BTLV_EFFECT_SetPokemon( pvw->pp, pos );
	}
}

#if 0
static	void	del_pokemon( POKEMON_VIEWER_WORK *pvw )
{
	BTLV_MCSS_Del( BTLV_EFFECT_GetMcssWork(), BTLV_MCSS_POS_AA );
	BTLV_MCSS_Del( BTLV_EFFECT_GetMcssWork(), BTLV_MCSS_POS_BB );
}
#endif
