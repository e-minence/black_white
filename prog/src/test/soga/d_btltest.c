//============================================================================================
/**
 * @file	d_btltest.c
 * @brief	デバッグ用関数
 * @author	soga
 * @date	2008.09.09
 */
//============================================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include <textprint.h>

#include <nnsys/mcs.h>

#include "system/main.h"
#include "arc_def.h"

#include "system\gfl_use.h"
#include "battle/btlv/btlv_effect.h"
#include "pokegra/pokegra_wb.naix"
#include "battgra/battgra_wb.naix"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "test/performance.h"

//#define MCS_ENABLE		//MCSを使用する
//#define POKEGRA_CHECK		//ポケモングラフィックチェックモード実装
//#define	SCALE_CHECK		//透視射影と正射影でスケールの誤差を修正するテストモードの起動
#define BTLV_MCSS_1vs1		//1vs1描画

#define	PAD_BUTTON_EXIT	( PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START )

#define	 NIN_SPL_MAX	(3)

#define	CAMERA_SPEED		( FX32_ONE * 2 )
#define	MOVE_SPEED			( FX32_ONE >> 2 )
#define	ROTATE_SPEED		( 65536 / 128 ) 
#define	PHI_MAX				( ( 65536 / 360 ) * 89 )
#define	DEFAULT_PHI			( 65536 / 12 )
#define	DEFAULT_THETA		( 65536 / 4 )
#define	DEFAULT_SCALE		( FX32_ONE * 16  )
#define	SCALE_SPEED			( FX32_ONE )
#define	SCALE_MAX			( FX32_ONE * 200 )

#define	SWITCH_TIME			( 60 * 5 )

#define	INTERVAL		(16)
#define	CH_MAX			(1)			//1:モノラル 2:ステレオ
#define	STRM_BUF_SIZE	( INTERVAL * 2 * CH_MAX * 32 )
#define	SWAV_HEAD_SIZE	(18)

// Windowsアプリケーションとの識別で使用するチャンネル値です
//static const u16 MCS_CHANNEL0 = 0;
//static const u16 MCS_CHANNEL1 = 1;

enum{
	MCS_CHANNEL0 = 0,
	MCS_CHANNEL1,
	MCS_CHANNEL2,
	MCS_CHANNEL3,
	MCS_CHANNEL4,
	MCS_CHANNEL5,
	MCS_CHANNEL6,
	MCS_CHANNEL7,
	MCS_CHANNEL8,
	MCS_CHANNEL9,
	MCS_CHANNEL10,
	MCS_CHANNEL11,
	MCS_CHANNEL12,
	MCS_CHANNEL13,
	MCS_CHANNEL14,
	MCS_CHANNEL15,

	MCS_CHANNEL_END,
};

#define	MCS_CH_MAX		( MCS_CHANNEL_END )
#define	MCS_SEND_SIZE	( 0x1800 )

#define G2D_BACKGROUND_COL	(0x0000)
#define G2D_FONT_COL		(0x7fff)
#define G2D_FONTSELECT_COL	(0x001f)

enum{
	BMPWIN_MONSNO = 0,
	BMPWIN_PROJECTION,
	BMPWIN_SCALE_E,
	BMPWIN_SCALE_M,

	BMPWIN_MAX
};

//32バイトアライメントでヒープからの取り方がわからないので、とりあえず静的に
static	u8				strmBuffer[STRM_BUF_SIZE] ATTRIBUTE_ALIGN(32);

typedef struct
{
	int					phi;
	int					theta;
	int					distance;
	int					pos_x;
	int					pos_y;
}MCS_WORK;

typedef struct
{
	int					seq_no;
	BTLV_CAMERA_WORK	*bcw;
//	BTLV_EFFECT_WORK	*bew;
	HEAPID				heapID;
	NNSSndStrm			strm;
	u8					FS_strmBuffer[STRM_BUF_SIZE];
	int					FSReadPos;
	u32					strmReadPos;
	u32					strmWritePos;
	int					visible_flag;
	int					timer_flag;
	int					timer;

	//MCS
    u8*					mcsWorkMem;
    NNSMcsDeviceCaps	deviceCaps;
    NNSMcsRecvCBInfo	recvCBInfo;
	void*				printBuffer;
	void*				recvBuf;
	int					mcs_idle;
	int					mcs_enable;
	MCS_WORK			mw;
	POKEMON_PARAM		*pp;
	int					mons_no;
	GFL_BMPWIN			*bmpwin[ BMPWIN_MAX ];
	GFL_TEXT_PRINTPARAM	*textParam;
	int					position;
	BTLV_MCSS_WORK		*bmw;
	int					key_repeat_speed;
	int					key_repeat_wait;
	int					ortho_mode;
	VecFx32				scale;
}SOGA_WORK;

static	void	TextPrint( SOGA_WORK *wk, int num, int bmpwin_num );
static	void	NumPrint( SOGA_WORK *wk, int num, int bmpwin_num );
static	void	Num16Print( SOGA_WORK *wk, int num, int bmpwin_num );

//ストリーム再生
static	void	StrmSetUp(SOGA_WORK *sw);
static	void	StrmCBWaveDataStock(NNSSndStrmCallbackStatus status,
									int numChannels,
									void *buffer[],
									u32	len,
									NNSSndStrmFormat format,
									void *arg);
static	void	StrmBufferCopy(SOGA_WORK *sw,int size);

//MCS
static BOOL MCS_Init( SOGA_WORK *sw );
static void MCS_Exit( SOGA_WORK *sw );
static void MCS_Read( SOGA_WORK *sw );
static void MCS_Write( SOGA_WORK *sw );
static void MCS_DataRecvCallback( const void* pRecv, u32 recvSize, u32 userData, u32 offset, u32 totalSize );
static void MCS_VBlankIntr( GFL_TCB *tcb, void *work );

//Capture
static void Capture_VBlankIntr( GFL_TCB *tcb, void *work );

typedef struct
{
	s32	x[2];
	s32	y[2];
	s32	percent;
	int	update;
}PERFORMANCE_PARAM;

extern	PERFORMANCE_PARAM	per_para;

static	void	MoveCamera( SOGA_WORK *wk );

static	void	set_pokemon( SOGA_WORK *wk );
static	void	del_pokemon( SOGA_WORK *wk );

static	const	int	pokemon_pos_table[][2]={ 
	{ BTLV_MCSS_POS_AA, BTLV_MCSS_POS_BB },
	{ BTLV_MCSS_POS_A, BTLV_MCSS_POS_B },
	{ BTLV_MCSS_POS_C, BTLV_MCSS_POS_D }
};

static	const	char	num_char_table[][1]={
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F"
};

static	const	char	ProjectionText[2][12]={
	"Perspective",
	"Ortho",
};

FS_EXTERN_OVERLAY(battle);

//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugBattleTestProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	SOGA_WORK* wk;
	u8 chNoList[1]={0};

	GFL_OVERLAY_Load(FS_OVERLAY_ID(battle));

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SOGABE_DEBUG, 0xc0000 );
	wk = GFL_PROC_AllocWork( proc, sizeof( SOGA_WORK ), HEAPID_SOGABE_DEBUG );
	MI_CpuClearFast( wk, sizeof( SOGA_WORK ) );
	wk->heapID = HEAPID_SOGABE_DEBUG;
		
	{
		static const GFL_DISP_VRAM dispvramBank = {
			GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
			GX_VRAM_TEX_01_BC,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG,			// テクスチャパレットスロット			
			GX_OBJVRAMMODE_CHAR_1D_64K,		// メインOBJマッピングモード
			GX_OBJVRAMMODE_CHAR_1D_32K,		// サブOBJマッピングモード
		};		
		GFL_DISP_SetBank( &dispvramBank );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
		MI_CpuFill16((void*)HW_BG_PLTT, 0x0000, HW_BG_PLTT_SIZE);

	}	

	GX_SetBankForLCDC( GX_VRAM_LCDC_D );
	
	G2_BlendNone();
	GFL_BG_Init( wk->heapID );
	GFL_BMPWIN_Init( wk->heapID );

	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_3D,
//			GX_DISPMODE_VRAM_C, GX_BGMODE_4, GX_BGMODE_3, GX_BG0_AS_3D,
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
		GFL_BG_SetVisible( GFL_BG_FRAME1_S,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME2_S,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_S,   VISIBLE_OFF );
		
	}		
	GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

	//3D関連初期化
	{
		int i;
		u32 fog_table[8];

		GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0, wk->heapID, NULL );
		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
		G3X_AlphaBlend( TRUE );
		G3X_EdgeMarking( TRUE );
		G3X_AntiAlias( TRUE );
		/*
		G3X_SetFog( TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0020, 0 );
		G3X_SetFogColor( GX_RGB( 31, 31, 31 ), 0 );

        for ( i=0; i<8; i++ ){
            fog_table[i] = (u32)(((i*16)<<0) | ((i*16+4)<<8) | ((i*16+8)<<16) | ((i*16+12)<<24));
        }
		G3X_SetFogTable( &fog_table[0] );
		*/
		GFL_BG_SetBGControl3D( 1 );
	}

	wk->seq_no = 0;
#if 1
	{
		BTLV_EFFECT_Init( 0, wk->heapID );
		wk->bcw = BTLV_EFFECT_GetCameraWork();
	}

	set_pokemon( wk );

#else
	{
		wk->bew = BTLV_EFFECT_Init( 0, wk->heapID );
		wk->bcw = BTLV_EFFECT_GetCameraWork( wk->bew );
	}

	set_pokemon( wk );

#endif

	//2D画面初期化
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
		};
		GFL_BG_SetBGControl(GFL_BG_FRAME1_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME1_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME3_M, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_M );

#ifndef	POKEGRA_CHECK
		GFL_ARC_UTIL_TransVramBgCharacter(ARCID_BATTGRA,NARC_battgra_wb_batt_bg1_NCGR,GFL_BG_FRAME1_M,0,0,0,wk->heapID);
		GFL_ARC_UTIL_TransVramScreen(ARCID_BATTGRA,NARC_battgra_wb_batt_bg1_NSCR,GFL_BG_FRAME1_M,0,0,0,wk->heapID);
		GFL_ARC_UTIL_TransVramBgCharacter(ARCID_BATTGRA,NARC_battgra_wb_batt_bg2_NCGR,GFL_BG_FRAME2_M,0,0,0,wk->heapID);
		GFL_ARC_UTIL_TransVramScreen(ARCID_BATTGRA,NARC_battgra_wb_batt_bg2_NSCR,GFL_BG_FRAME2_M,0,0,0,wk->heapID);
		GFL_ARC_UTIL_TransVramPalette(ARCID_BATTGRA,NARC_battgra_wb_batt_bg_NCLR,PALTYPE_MAIN_BG,0,0x100,wk->heapID);
#endif
	}

#ifdef	POKEGRA_CHECK
	{
		static const GFL_TEXT_PRINTPARAM default_param = { NULL, 0, 0, 1, 1, 1, 0, GFL_TEXT_WRITE_16 };
		wk->bmpwin[ BMPWIN_PROJECTION ] = GFL_BMPWIN_Create( GFL_BG_FRAME2_M, 1, 1, 7, 1, 0, GFL_BG_CHRAREA_GET_B );
		wk->bmpwin[ BMPWIN_MONSNO ] = GFL_BMPWIN_Create( GFL_BG_FRAME2_M, 1, 2, 5, 1, 0, GFL_BG_CHRAREA_GET_B );
		wk->bmpwin[ BMPWIN_SCALE_E ] = GFL_BMPWIN_Create( GFL_BG_FRAME2_M, 1, 4, 8, 1, 0, GFL_BG_CHRAREA_GET_B );
		wk->bmpwin[ BMPWIN_SCALE_M ] = GFL_BMPWIN_Create( GFL_BG_FRAME2_M, 1, 5, 8, 1, 0, GFL_BG_CHRAREA_GET_B );

		wk->textParam = GFL_HEAP_AllocMemory( wk->heapID, sizeof( GFL_TEXT_PRINTPARAM ) );
		*wk->textParam = default_param;

		//フォントパレット作成＆転送
		{
			static	u16 plt[16] = { G2D_BACKGROUND_COL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			GFL_BG_LoadPalette( GFL_BG_FRAME2_M, &plt, 16*2, 0 );
		}

		wk->bmw = BTLV_MCSS_Init( NULL, wk->heapID );
		BTLV_MCSS_SetOrthoMode( wk->bmw );
		wk->ortho_mode = 1;
	}
#endif

	//ウインドマスク設定（画面両端のエッジマーキングのゴミを消す）
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

#if 0
	//ストリーム再生初期化
    NNS_SndInit();
    NNS_SndStrmInit( &wk->strm );
	NNS_SndStrmAllocChannel( &wk->strm, CH_MAX, chNoList);

	StrmSetUp(wk);

	NNS_SndStrmStart(&wk->strm);
#endif

	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x0000 );
	
	//キャプチャセット
	GFUser_VIntr_CreateTCB( Capture_VBlankIntr, NULL, 0 );

	//フェードイン
	GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, 2 );

	GFL_UI_KEY_GetRepeatSpeed( &wk->key_repeat_speed, &wk->key_repeat_wait );
	GFL_UI_KEY_SetRepeatSpeed( wk->key_repeat_speed / 4, wk->key_repeat_wait );

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugBattleTestProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	int pad = GFL_UI_KEY_GetCont();
	int trg = GFL_UI_KEY_GetTrg();
	int rep = GFL_UI_KEY_GetRepeat();
	int tp = GFL_UI_TP_GetTrg();
	SOGA_WORK* wk = mywk;

#ifdef MCS_ENABLE
	if( wk->mcs_enable ){
		NNS_McsPollingIdle();
		MCS_Read( wk );
		MCS_Write( wk );
	}
#endif

#ifndef	POKEGRA_CHECK
	//画面切り替え実験
	if( wk->timer_flag ){
		wk->timer++;
	}
	if(wk->timer > SWITCH_TIME ){
		wk->timer=0;
		wk->visible_flag ^= 1;
		if( wk->visible_flag ){
			GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_OFF );
			GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_ON );
			GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_ON );
		}
		else{
			GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
			GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
			GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
		}
	}
#endif

#if 0
	StrmBufferCopy(wk,0);
#endif

#ifdef MCS_ENABLE
	if( ( ( trg & PAD_BUTTON_START ) ||
		  ( wk->mcs_idle ) ) &&
		  ( wk->timer_flag == 0 ) ){
		if( wk->mcs_enable ){
			MCS_Exit( wk );
		}
		else{
			wk->mcs_idle = MCS_Init( wk );
		}
	}
#else
	if( trg & PAD_BUTTON_START ){
		VecFx32	pos,target;
		BTLV_CAMERA_GetDefaultCameraPosition( &pos, &target );
//		BTLV_CAMERA_MoveCameraPosition( wk->bcw, &pos, &target );
		BTLV_CAMERA_MoveCameraInterpolation( wk->bcw, &pos, &target, 20, 0, 20 );
	}
#endif

#ifdef	POKEGRA_CHECK
	if( wk->timer_flag ){
		if( wk->timer_flag & 2){
			MoveCamera( wk );
		}
		else{
			BOOL draw = FALSE;
			BOOL pos = FALSE;
			int	mons_no = wk->mons_no;
			int	position = wk->position;
#ifndef SCALE_CHECK
			VecFx32 scale_m, scale_e;
	
			BTLV_MCSS_GetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ], &scale_m );
			BTLV_MCSS_GetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ], &scale_e );
#endif
	
			if( trg & PAD_BUTTON_R ){
				if( mons_no + 100 <= MONSNO_MAX ){
					mons_no += 100;
				}
				else{
					mons_no += 100;
					mons_no -= MONSNO_MAX;
				}
			}
			if( trg & PAD_BUTTON_X ){
				if( mons_no + 10 <= MONSNO_MAX ){
					mons_no += 10;
				}
				else{
					mons_no += 10;
					mons_no -= MONSNO_MAX;
				}
			}
			if( trg & PAD_BUTTON_A ){
				if( mons_no + 1 <= MONSNO_MAX ){
					mons_no += 1;
				}
				else{
					mons_no += 1;
					mons_no -= MONSNO_MAX;
				}
			}
			if( trg & PAD_BUTTON_L ){
				if( mons_no - 100 > 0 ){
					mons_no -= 100;
				}
				else{
					mons_no -= 100;
					mons_no += MONSNO_MAX;
				}
			}
			if( trg & PAD_BUTTON_Y ){
				if( mons_no - 10 > 0 ){
					mons_no -= 10;
				}
				else{
					mons_no -= 10;
					mons_no += MONSNO_MAX;
				}
			}
			if( trg & PAD_BUTTON_B ){
				if( mons_no - 1 > 0 ){
					mons_no -= 1;
				}
				else{
					mons_no -= 1;
					mons_no += MONSNO_MAX;
				}
			}
			if( trg & PAD_KEY_LEFT ){
				position--;
				if( position < 0 ){
					position = 2;
				}
				pos = TRUE;
			}
			if( trg & PAD_KEY_RIGHT ){
				position++;
				if( position > 2 ){
					position = 0;
				}
				pos = TRUE;
			}
#ifndef SCALE_CHECK
			if( rep & PAD_KEY_UP ){
				scale_m.x--;
				scale_m.y--;
				scale_m.z--;
				scale_e.x--;
				scale_e.y--;
				scale_e.z--;
				draw = TRUE;
				if( scale_m.x < 0 ){
					scale_m.x = 0;
					scale_m.y = 0;
					scale_m.z = 0;
				}
				if( scale_e.x < 0 ){
					scale_e.x = 0;
					scale_e.y = 0;
					scale_e.z = 0;
				}
			}
			if( rep & PAD_KEY_DOWN ){
				scale_m.x++;
				scale_m.y++;
				scale_m.z++;
				scale_e.x++;
				scale_e.y++;
				scale_e.z++;
				draw = TRUE;
			}
#else
			if( rep & PAD_KEY_UP ){
				wk->scale.x--;
				wk->scale.y--;
				wk->scale.z--;
				draw = TRUE;
				if( wk->scale.x < 0 ){
					wk->scale.x = 0;
					wk->scale.y = 0;
					wk->scale.z = 0;
				}
			}
			if( rep & PAD_KEY_DOWN ){
				wk->scale.x++;
				wk->scale.y++;
				wk->scale.z++;
				draw = TRUE;
			}
#endif
			if( pad & PAD_BUTTON_START ){
//				BTLV_MCSS_SetMepachiFlag( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ], BTLV_MCSS_MEPACHI_ON );
//				BTLV_MCSS_SetMepachiFlag( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ], BTLV_MCSS_MEPACHI_ON );
//				BTLV_MCSS_SetVanishFlag( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ], BTLV_MCSS_VANISH_ON );
//				BTLV_MCSS_SetVanishFlag( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ], BTLV_MCSS_VANISH_ON );
//				G3X_AntiAlias( FALSE );
			}
			else{
//				BTLV_MCSS_SetMepachiFlag( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ], BTLV_MCSS_MEPACHI_OFF );
//				BTLV_MCSS_SetMepachiFlag( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ], BTLV_MCSS_MEPACHI_OFF );
//				BTLV_MCSS_SetVanishFlag( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ], BTLV_MCSS_VANISH_OFF );
//				BTLV_MCSS_SetVanishFlag( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ], BTLV_MCSS_VANISH_OFF );
//				G3X_AntiAlias( TRUE );
			}
			if( tp == TRUE ){
				wk->ortho_mode ^= 1;
				TextPrint( wk, wk->ortho_mode, BMPWIN_PROJECTION );
				if( wk->ortho_mode ){
					BTLV_MCSS_SetOrthoMode( wk->bmw );
				}
				else{
					BTLV_MCSS_ResetOrthoMode( wk->bmw );
#ifdef SCALE_CHECK
//					BTLV_MCSS_SetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ], &wk->scale );
//					BTLV_MCSS_SetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ], &wk->scale );
#endif
				}
			}
			if( ( mons_no != wk->mons_no ) ||
				( draw == TRUE ) ||
				( position != wk->position ) ){
				if( draw == FALSE ){
					del_pokemon( wk );
					wk->mons_no = mons_no;
					wk->position = position;
					set_pokemon( wk );
#ifndef SCALE_CHECK
					if( pos == TRUE ){
						BTLV_MCSS_GetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ], &scale_m );
						BTLV_MCSS_GetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ], &scale_e );
					}
#endif
				}
#ifndef SCALE_CHECK
				BTLV_MCSS_SetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ], &scale_m );
				BTLV_MCSS_SetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ], &scale_e );
				NumPrint( wk, wk->mons_no, BMPWIN_MONSNO );
				Num16Print( wk, scale_m.x, BMPWIN_SCALE_M );
				Num16Print( wk, scale_e.x, BMPWIN_SCALE_E );
#else
				BTLV_MCSS_SetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ], &wk->scale );
				BTLV_MCSS_SetScale( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ], &wk->scale );
				NumPrint( wk, wk->mons_no, BMPWIN_MONSNO );
				Num16Print( wk, wk->scale.x, BMPWIN_SCALE_M );
				Num16Print( wk, wk->scale.x, BMPWIN_SCALE_E );
#endif
			}
		}
//		if( trg & PAD_BUTTON_DEBUG ){
//			wk->timer_flag ^= 2;
//		}
	}
	else{
		MoveCamera( wk );

		if( (trg & PAD_BUTTON_X ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_AA2BBGANSEKI );
		}
		if( (trg & PAD_BUTTON_Y ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_BB2AAGANSEKI );
		}
		if( (trg & PAD_BUTTON_A ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_AA2BBMIZUDEPPOU );
		}
		if( (trg & PAD_BUTTON_B ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_BB2AAMIZUDEPPOU );
		}
		if( (trg & PAD_BUTTON_SELECT ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_A2BGANSEKI );
		}
	}

	if( trg & PAD_BUTTON_SELECT ){
#if 0
		wk->timer_flag ^= 1;
		if( wk->timer_flag ){
			VecFx32	scale;

#ifdef BTLV_MCSS_1vs1
			BTLV_EFFECT_DelPokemon( BTLV_MCSS_POS_AA );
			BTLV_EFFECT_DelPokemon( BTLV_MCSS_POS_BB );
#else 
			BTLV_EFFECT_DelPokemon( BTLV_MCSS_POS_A );
			BTLV_EFFECT_DelPokemon( BTLV_MCSS_POS_B );
			BTLV_EFFECT_DelPokemon( BTLV_MCSS_POS_C );
			BTLV_EFFECT_DelPokemon( BTLV_MCSS_POS_D );
#endif
			wk->mons_no = MONSNO_HUSIGIDANE;
			wk->position = 0;
			wk->scale.x = 0x0800;
			wk->scale.y = 0x0800;
			wk->scale.z = 0x0800;
			set_pokemon( wk );
			TextPrint( wk, wk->ortho_mode, BMPWIN_PROJECTION );
			NumPrint( wk, wk->mons_no, BMPWIN_MONSNO );
			BTLV_MCSS_GetScale( wk->bmw, BTLV_MCSS_POS_BB, &scale );
			Num16Print( wk, scale.x, BMPWIN_SCALE_E );
			BTLV_MCSS_GetScale( wk->bmw, BTLV_MCSS_POS_AA, &scale );
			Num16Print( wk, scale.x, BMPWIN_SCALE_M );
			GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_ON );
			DEBUG_PerformanceSetActive( FALSE );
		}
		else{
			del_pokemon( wk );
			set_pokemon( wk );
			GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
			DEBUG_PerformanceSetActive( TRUE );
		}
#endif
	}
#else
	MoveCamera( wk );

#if 0
	if( (trg & PAD_BUTTON_X ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
		BTLV_EFFECT_Add( BTLV_EFFECT_A2BGANSEKI );
	}
	if( (trg & PAD_BUTTON_Y ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
		BTLV_EFFECT_Add( BTLV_EFFECT_B2AGANSEKI );
	}
	if( (trg & PAD_BUTTON_A ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
		BTLV_EFFECT_Add( BTLV_EFFECT_A2BMIZUDEPPOU );
	}
	if( (trg & PAD_BUTTON_B ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
		BTLV_EFFECT_Add( BTLV_EFFECT_B2AMIZUDEPPOU );
	}
	if( trg & PAD_BUTTON_SELECT ){
		wk->timer_flag ^= 1;
	}
#endif
		if( (trg & PAD_BUTTON_X ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_AA2BBGANSEKI );
		}
		if( (trg & PAD_BUTTON_Y ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_BB2AAGANSEKI );
		}
		if( (trg & PAD_BUTTON_A ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_AA2BBMIZUDEPPOU );
		}
		if( (trg & PAD_BUTTON_B ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_BB2AAMIZUDEPPOU );
		}
		if( (trg & PAD_BUTTON_SELECT ) && ( BTLV_EFFECT_CheckExecute() == FALSE ) ){
			BTLV_EFFECT_Add( BTLV_EFFECT_A2BGANSEKI );
		}
#endif

#ifdef	POKEGRA_CHECK
	if( wk->timer_flag ){
		BTLV_MCSS_Main( wk->bmw );
		BTLV_MCSS_Draw( wk->bmw );
	}
#endif

	BTLV_EFFECT_Main();

	if( pad == PAD_BUTTON_EXIT ){
//		NNS_SndStrmStop(&wk->strm);
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
static GFL_PROC_RESULT DebugBattleTestProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	SOGA_WORK* wk = mywk;

	if( GFL_FADE_CheckFade() == TRUE ){
		return GFL_PROC_RES_CONTINUE;	
	}

	GFL_UI_KEY_SetRepeatSpeed( wk->key_repeat_speed, wk->key_repeat_wait );

#ifdef	POKEGRA_CHECK
	{
		int	i;

		for( i = 0 ; i < BMPWIN_MAX ; i++ ){
			GFL_BMPWIN_Delete( wk->bmpwin[ i ] );
		}
	}
	GFL_HEAP_FreeMemory( wk->textParam );
	BTLV_MCSS_Exit( wk->bmw );
#endif

	BTLV_EFFECT_Exit();
//	BTLV_EFFECT_Exit( wk->bew );

//    NNS_SndStrmFreeChannel( &wk->strm );

	GFL_G3D_Exit();

	GFL_BG_Exit();
	GFL_BMPWIN_Exit();

	GFL_PROC_FreeWork( proc );

	GFL_HEAP_DeleteHeap( HEAPID_SOGABE_DEBUG );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA		DebugBattleTestProcData = {
	DebugBattleTestProcInit,
	DebugBattleTestProcMain,
	DebugBattleTestProcExit,
};

//======================================================================
//	テキスト表示
//======================================================================
static	void	TextPrint( SOGA_WORK *wk, int num, int bmpwin_num )
{
	wk->textParam->writex = 0;
	wk->textParam->writey = 0;
	wk->textParam->bmp = GFL_BMPWIN_GetBmp( wk->bmpwin[ bmpwin_num ] );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->bmpwin[ bmpwin_num ] ), 0 );
	GFL_TEXT_PrintSjisCode( &ProjectionText[ num ][ 0 ], wk->textParam );
	GFL_BMPWIN_TransVramCharacter( wk->bmpwin[ bmpwin_num ] );
	GFL_BMPWIN_MakeScreen( wk->bmpwin[ bmpwin_num ] );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
}

//======================================================================
//	数字表示
//======================================================================
static	void	NumPrint( SOGA_WORK *wk, int num, int bmpwin_num )
{
	char	num_char[ 4 ] = "\0\0\0\0";
	int		num_100,num_10,num_1;

	num_100 = num / 100;
	num_10  = ( num - ( num_100 * 100 ) ) / 10;
	num_1   = num - ( num_100 * 100 ) - (num_10 * 10 );

	num_char[ 0 ] = num_char_table[ num_100 ][ 0 ];
	num_char[ 1 ] = num_char_table[ num_10  ][ 0 ];
	num_char[ 2 ] = num_char_table[ num_1   ][ 0 ];

	wk->textParam->writex = 0;
	wk->textParam->writey = 0;
	wk->textParam->bmp = GFL_BMPWIN_GetBmp( wk->bmpwin[ bmpwin_num ] );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->bmpwin[ bmpwin_num ] ), 0 );
	GFL_TEXT_PrintSjisCode( &num_char[0], wk->textParam );
	GFL_BMPWIN_TransVramCharacter( wk->bmpwin[ bmpwin_num ] );
	GFL_BMPWIN_MakeScreen( wk->bmpwin[ bmpwin_num ] );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
}

static	void	Num16Print( SOGA_WORK *wk, int num, int bmpwin_num )
{
	int		i;
	char	num_char[ 9 ] = "\0\0\0\0\0\0\0\0\0";

	for( i = 0 ; i < 8 ; i++ ){
		num_char[ i ] = num_char_table[ ( num >> ( 28 - 4 * i ) ) & 0x0000000f ][ 0 ];
	}

	wk->textParam->writex = 0;
	wk->textParam->writey = 0;
	wk->textParam->bmp = GFL_BMPWIN_GetBmp( wk->bmpwin[ bmpwin_num ] );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->bmpwin[ bmpwin_num ] ), 0 );
	GFL_TEXT_PrintSjisCode( &num_char[0], wk->textParam );
	GFL_BMPWIN_TransVramCharacter( wk->bmpwin[ bmpwin_num ] );
	GFL_BMPWIN_MakeScreen( wk->bmpwin[ bmpwin_num ] );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
}

//======================================================================
//	カメラの制御
//======================================================================
static	void	MoveCamera( SOGA_WORK *wk )
{
	int pad = GFL_UI_KEY_GetCont();
	VecFx32	pos,ofsx,ofsz,camPos, camTarget;
	fx32	xofs=0,yofs=0,zofs=0;		

	if( pad & PAD_BUTTON_L ){
		if( pad & PAD_KEY_LEFT ){
			BTLV_CAMERA_MoveCameraAngle( wk->bcw, 0, -ROTATE_SPEED );
		}
		if( pad & PAD_KEY_RIGHT ){
			BTLV_CAMERA_MoveCameraAngle( wk->bcw, 0,  ROTATE_SPEED );
		}
		if( pad & PAD_KEY_UP ){
			BTLV_CAMERA_MoveCameraAngle( wk->bcw, -ROTATE_SPEED, 0 );
		}
		if( pad & PAD_KEY_DOWN ){
			BTLV_CAMERA_MoveCameraAngle( wk->bcw,  ROTATE_SPEED, 0 );
		}
	}
	else{
		BTLV_CAMERA_GetCameraPosition( wk->bcw, &camPos, &camTarget );

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

		BTLV_CAMERA_MoveCameraPosition( wk->bcw, &camPos, &camTarget );
		OS_TPrintf("pos_x:%08x pos_y:%08x pos_z:%08x tar_x:%08x tar_y:%08x tar_z:%08x\n",
			camPos.x,
			camPos.y,
			camPos.z,
			camTarget.x,
			camTarget.y,
			camTarget.z );
	}

}
 
static	void	set_pokemon( SOGA_WORK *wk )
{
	//POKEMON_PARAM生成
	POKEMON_PARAM	*pp = GFL_HEAP_AllocMemory( wk->heapID, POKETOOL_GetWorkSize() );
	PP_SetupEx( pp, 0, 0, 0, 0, 255 );
	
	if( wk->timer_flag ){
		PP_Put( pp, ID_PARA_monsno, wk->mons_no );
		PP_Put( pp, ID_PARA_id_no, 0x10 );
		BTLV_MCSS_Add( wk->bmw, pp, pokemon_pos_table[ wk->position ][ 0 ] );
		BTLV_MCSS_Add( wk->bmw, pp, pokemon_pos_table[ wk->position ][ 1 ] );
	}
	else{
#ifdef BTLV_MCSS_1vs1
//1vs1
		PP_Put( pp, ID_PARA_monsno, MONSNO_RIZAADON );
		PP_Put( pp, ID_PARA_id_no, 0x10 );
		BTLV_EFFECT_SetPokemon( pp, BTLV_MCSS_POS_AA );
		BTLV_EFFECT_SetPokemon( pp, BTLV_MCSS_POS_BB );
#else
//2vs2
		PP_Put( pp, ID_PARA_monsno, MONSNO_AUSU + 1 );
		PP_Put( pp, ID_PARA_id_no, 0x10 );
		BTLV_EFFECT_SetPokemon( pp, BTLV_MCSS_POS_A );
		BTLV_EFFECT_SetPokemon( pp, BTLV_MCSS_POS_B );
		PP_Put( pp, ID_PARA_monsno, MONSNO_AUSU + 2 );
		BTLV_EFFECT_SetPokemon( pp, BTLV_MCSS_POS_C );
		BTLV_EFFECT_SetPokemon( pp, BTLV_MCSS_POS_D );
#endif
	}
	GFL_HEAP_FreeMemory( pp );
}

static	void	del_pokemon( SOGA_WORK *wk )
{
	BTLV_MCSS_Del( wk->bmw, pokemon_pos_table[ wk->position ][ 0 ] );
	BTLV_MCSS_Del( wk->bmw, pokemon_pos_table[ wk->position ][ 1 ] );
}

#if 0
//======================================================================
//	カメラの初期化
//======================================================================
static	void	InitCamera( CAMERA_WORK *cw, HEAPID heapID )
{
//	VecFx32	cam_pos = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 16.5f ), FX_F32_TO_FX32( 38.0f ) };
	VecFx32	cam_pos = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 7.8f ), FX_F32_TO_FX32( 21.0f ) };

	cw->target.x = FX_F32_TO_FX32( 0.0f );
	cw->target.y = FX_F32_TO_FX32( 2.6f );
	cw->target.z = FX_F32_TO_FX32( 0.0f );
	cw->camUp.x	= 0;
	cw->camUp.y	= FX32_ONE;
	cw->camUp.z	= 0;

	cw->phi		= FX_Atan2Idx( cam_pos.y, cam_pos.z );
	cw->theta	= FX_Atan2Idx( cam_pos.y, cam_pos.x );
	cw->scale	= VEC_Mag( &cam_pos );

	CalcCamera( cw );

	if( cw->camera == NULL ){
		cw->camera = GFL_G3D_CAMERA_Create( GFL_G3D_PRJPERS, 
											FX32_SIN13,
											FX32_COS13,
//											FX32_ONE * 4 / 3,
											FX_F32_TO_FX32( 1.33f ),
											NULL,
											FX32_ONE,
											FX32_ONE * 1000,
											NULL,
											&cw->camPos,
											&cw->camUp,
											&cw->target,
											heapID );
	}
}

//======================================================================
//	カメラの制御
//======================================================================
static	void	MoveCamera( SOGA_WORK *wk )
{
	CAMERA_WORK	*cw = &wk->cw;
	int pad = GFL_UI_KEY_GetCont();
	VecFx32	pos,ofsx,ofsz;
	fx32	xofs=0,yofs=0,zofs=0;		

	if( pad & PAD_BUTTON_Y ){
		if( pad & PAD_KEY_LEFT ){
			cw->theta -= ROTATE_SPEED;
		}
		if( pad & PAD_KEY_RIGHT ){
			cw->theta += ROTATE_SPEED;
		}
		if( pad & PAD_KEY_UP ){
			cw->phi -= ROTATE_SPEED;
			if( cw->phi <= -PHI_MAX ){
				cw->phi = -PHI_MAX;
			}
		}
		if( pad & PAD_KEY_DOWN ){
			cw->phi += ROTATE_SPEED;
			if( cw->phi >= PHI_MAX ){
				cw->phi = PHI_MAX;
			}
		}
	}
	else{
		if( pad & PAD_KEY_LEFT ){
			xofs = -MOVE_SPEED;
		}
		if( pad & PAD_KEY_RIGHT ){
			xofs = MOVE_SPEED;
		}
		if( pad & PAD_KEY_UP ){
			yofs = MOVE_SPEED;
		}
		if( pad & PAD_KEY_DOWN ){
			yofs = -MOVE_SPEED;
		}
		if( pad & PAD_BUTTON_X ){
			zofs = MOVE_SPEED;
		}
		if( pad & PAD_BUTTON_B ){
			zofs = -MOVE_SPEED;
		}
		if( wk->mw.pos_x ){
			xofs = MOVE_SPEED * wk->mw.pos_x;
			wk->mw.pos_x = 0;
		}
		if( wk->mw.pos_y ){
			yofs = MOVE_SPEED * wk->mw.pos_y;
			wk->mw.pos_y = 0;
		}
		if( wk->mw.distance ){
			zofs = MOVE_SPEED * (wk->mw.distance / 50);
			wk->mw.distance = 0;
		}
		pos.x = cw->camPos.x - cw->target.x;
		pos.y = 0;
		pos.z = cw->camPos.z - cw->target.z;
		VEC_Normalize( &pos, &pos );

		ofsx.x = FX_MUL( pos.z, xofs );
		ofsx.y = 0;
		ofsx.z = -FX_MUL( pos.x, xofs );

		ofsz.x = -FX_MUL( pos.x, zofs );
		ofsz.y = yofs;
		ofsz.z = -FX_MUL( pos.z, zofs );

		cw->target.x += ofsx.x + ofsz.x;
		cw->target.y += ofsx.y + ofsz.y;
		cw->target.z += ofsx.z + ofsz.z;
	}
	if( pad & PAD_BUTTON_L ){
		if( cw->scale > 0 ){
			cw->scale -= SCALE_SPEED;
		}
	}
	if( pad & PAD_BUTTON_R ){
		if( cw->scale < SCALE_MAX ){
			cw->scale += SCALE_SPEED;
		}
	}
	if( wk->mw.theta ){
		cw->theta -= wk->mw.theta * 32 ;
		wk->mw.theta = 0;
	}

	if( wk->mw.phi ){
		cw->phi -= wk->mw.phi * 32 ;
		if( cw->phi <= -PHI_MAX ){
			cw->phi = -PHI_MAX;
		}
		if( cw->phi >= PHI_MAX ){
			cw->phi = PHI_MAX;
		}
		wk->mw.phi = 0;
	}

	CalcCamera( cw );
	GFL_G3D_CAMERA_SetPos( cw->camera, &cw->camPos );
	GFL_G3D_CAMERA_SetTarget( cw->camera, &cw->target );
	GFL_G3D_CAMERA_Switching( cw->camera );
}

//======================================================================
//	カメラ位置の計算
//======================================================================
static	void	CalcCamera( CAMERA_WORK *cw )
{
	MtxFx43	scale,trans;
	VecFx32	pos;

	//phiとthetaとscaleからcamPosを計算
	pos.x = FX_MUL( FX_CosIdx( cw->theta & 0xffff ), FX_CosIdx( cw->phi & 0xffff ) );
	pos.y = FX_SinIdx( cw->phi & 0xffff );
	pos.z = FX_MUL( FX_SinIdx( cw->theta & 0xffff ), FX_CosIdx( cw->phi & 0xffff ) );
	MTX_Scale43( &scale, cw->scale, cw->scale, cw->scale );
	MTX_MultVec43( &pos, &scale, &pos );
	MTX_Identity43( &trans );
//	MTX_TransApply43( &trans, &trans, cw->target.x,	cw->target.y, cw->target.z );
	MTX_MultVec43( &pos, &trans, &cw->camPos );
}
#endif

//---------------------------------------------------------------------------
//	ストリーム再生関数
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static	void	StrmSetUp(SOGA_WORK *sw)
{
	sw->FSReadPos=SWAV_HEAD_SIZE;
	sw->strmReadPos=0;
	sw->strmWritePos=0;

	MI_CpuClearFast( &strmBuffer[0], STRM_BUF_SIZE );
    
	NNS_SndStrmSetup( &sw->strm,
					  NNS_SND_STRM_FORMAT_PCM8,
					  &strmBuffer[0],
					  STRM_BUF_SIZE,
					  NNS_SND_STRM_TIMER_CLOCK/8000,
					  INTERVAL,
					  StrmCBWaveDataStock,
					  sw);
}

//---------------------------------------------------------------------------
// 波形データを補充するコールバックが割り込み禁止で呼ばれるので、
// FS_〜が中から呼べないので、この関数でReadしておく
//---------------------------------------------------------------------------
static	void	StrmBufferCopy(SOGA_WORK *sw,int size)
{
	FSFile	file;
	s32		ret;

    FS_InitFile(&file);
	FS_OpenFile(&file,"/pm_battle.swav");
	FS_SeekFile(&file,sw->FSReadPos,FS_SEEK_SET);

	if(size){
		ret=FS_ReadFile(&file,&sw->FS_strmBuffer[0],size);
		sw->FSReadPos+=size;
	}
	else{
		while(sw->strmReadPos!=sw->strmWritePos){
			ret=FS_ReadFile(&file,&sw->FS_strmBuffer[sw->strmWritePos],32);
			if(ret==0){
				sw->FSReadPos=SWAV_HEAD_SIZE;
				FS_SeekFile(&file,sw->FSReadPos,FS_SEEK_SET);
				continue;
			}
			sw->FSReadPos+=32;
			sw->strmWritePos+=32;
			if(sw->strmWritePos>=STRM_BUF_SIZE){
				sw->strmWritePos=0;
			}
		}
	}
	FS_CloseFile(&file);
}

//---------------------------------------------------------------------------
// 波形データを補充するコールバック
//---------------------------------------------------------------------------
static	void	StrmCBWaveDataStock(NNSSndStrmCallbackStatus status,
									int numChannels,
									void *buffer[],
									u32	len,
									NNSSndStrmFormat format,
									void *arg)
{
	SOGA_WORK *sw=(SOGA_WORK *)arg;
	int	i;
	u8	*strBuf;

	strBuf=buffer[0];
	
	for(i=0;i<len;i++){
		strBuf[i]=sw->FS_strmBuffer[i+sw->strmReadPos];
	}

	sw->strmReadPos+=len;
	if(sw->strmReadPos>=STRM_BUF_SIZE){
		sw->strmReadPos=0;
	}
	DC_FlushRange(strBuf,len);
}

//
//	MCS制御実験
//
static BOOL MCS_Init( SOGA_WORK *wk )
{
	if(	wk->mcs_enable ) return FALSE;

	wk->mw.phi = 0;
	wk->mw.theta = 0;

	// mcsの初期化
	wk->mcsWorkMem = GFL_HEAP_AllocMemory( wk->heapID, NNS_MCS_WORKMEM_SIZE ); // MCSのワーク用メモリを確保
	OS_TPrintf("size:%08x\n",NNS_MCS_WORKMEM_SIZE);
	NNS_McsInit( wk->mcsWorkMem );

	GFUser_VIntr_CreateTCB( MCS_VBlankIntr, NULL, 0 );

	// デバイスのオープン
	if( NNS_McsGetMaxCaps() > 0 && NNS_McsOpen( &wk->deviceCaps ) )
	{
		wk->printBuffer = GFL_HEAP_AllocMemory( wk->heapID, 1024 );        // プリント用のバッファの確保
		wk->recvBuf = GFL_HEAP_AllocMemory( wk->heapID, 1024 );       // 受信用バッファの確保

        NNS_NULL_ASSERT(wk->printBuffer);
        NNS_NULL_ASSERT(wk->recvBuf);

        // OS_Printfによる出力
        OS_Printf("device open\n");

        // mcs文字列出力の初期化
		NNS_McsInitPrint( wk->printBuffer, 1024 );

        // NNS_McsPrintfによる出力
        // このタイミングでmcsサーバが接続していれば、コンソールに表示されます。
		(void)NNS_McsPrintf("device ID %08X\n", wk->deviceCaps.deviceID );

        // 読み取り用バッファの登録
		NNS_McsRegisterStreamRecvBuffer(MCS_CHANNEL0, wk->recvBuf, 1024 );

        // 受信コールバック関数の登録
//		NNS_McsRegisterRecvCallback( &wk->recvCBInfo, MCS_CHANNEL1, MCS_DataRecvCallback, (u32)&wk->mw );

		wk->mcs_enable = 1;

        return FALSE;
    }
	OS_Printf("device open fail.\n");
	return TRUE;
}

static void MCS_Exit( SOGA_WORK *wk )
{
//	NNS_McsUnregisterRecvResource(MCS_CHANNEL1);
	NNS_McsUnregisterRecvResource(MCS_CHANNEL0);

	GFL_HEAP_FreeMemory( wk->recvBuf );
	GFL_HEAP_FreeMemory( wk->printBuffer );

	// NNS_McsPutStringによる出力
	(void)NNS_McsPutString("device close\n");

	// デバイスをクローズ
	(void)NNS_McsClose();

	wk->mcs_enable = 0;
	wk->mcs_idle = 0;
}

static void MCS_Read( SOGA_WORK *wk )
{
	int			len;
	MCS_WORK	buf;

	// 受信可能なデータサイズの取得
	len = NNS_McsGetStreamReadableSize( MCS_CHANNEL0 );

	if( len > 0 ){
		u32 readSize;
		// データの読み取り
		if( NNS_McsReadStream( MCS_CHANNEL0, &buf, sizeof(MCS_WORK), &readSize)){
			wk->mw = buf;
        }
	}
}

static void MCS_Write( SOGA_WORK *wk )
{
#if 0 
	if( per_para.update ){
		per_para.update = 0;
		// データの書き込み
		NNS_McsWriteStream( MCS_CHANNEL0, &per_para, sizeof(PERFORMANCE_PARAM) - 4 );
	}
#else
//	NNS_McsWriteStream( MCS_CHANNEL0, (const void *)HW_LCDC_VRAM_D, 256*192*2 );
	int ch;

	for( ch = 0 ; ch < MCS_CH_MAX ; ch++ ){
		NNS_McsWriteStream( MCS_CHANNEL0 + ch, (const void *)(HW_LCDC_VRAM_D + MCS_SEND_SIZE * ch), MCS_SEND_SIZE );
	}
#endif
}

/*---------------------------------------------------------------------------*
  Name:         DataRecvCallback

  Description:  PC側からデータを受信したときに呼ばれるコールバック関数です。

                登録するコールバック関数内ではデータの送受信を行わないでください。
                また、割り込みが禁止されている場合があるため、
                割り込み待ちループも行わないでください。

  Arguments:    recv:       受信したデータの一部あるいは全部を格納している
                            バッファへのポインタ。
                recvSize:   recvによって示されるバッファに格納されている
                            データのサイズ。
                userData:   NNS_McsRegisterRecvCallback()の引数userDataで
                            指定した値。
                offset:     受信したデータの全部がrecvによって示されるバッファに
                            格納されている場合は0。
                            受信したデータの一部が格納されている場合は、
                            受信したデータ全体に対する0を基準としたオフセット位置。
                totalSize:  受信したデータの全体のサイズ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void
MCS_DataRecvCallback(
    const void* pRecv,
    u32         recvSize,
    u32         userData,
    u32         offset,
    u32         totalSize
)
{
    MCS_WORK *mw = (MCS_WORK *)userData;

	OS_Printf( " Callback OK!\n");

	// 受信バッファチェック
	if (pRecv != NULL && recvSize == sizeof(MCS_WORK) && offset == 0)
	{
        mw = (MCS_WORK *)pRecv;
    }
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  Vブランク割り込みハンドラです。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void MCS_VBlankIntr( GFL_TCB *tcb, void *work )
{
    NNS_McsVBlankInterrupt();
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  Vブランク割り込みハンドラです。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void Capture_VBlankIntr( GFL_TCB *tcb, void *work )
{
	//---------------------------------------------------------------------------
	// Execute capture
	// Blend rendered image and displayed image (VRAM), and output to VRAM-C
	GX_SetCapture(GX_CAPTURE_SIZE_256x192,			// Capture size
				  GX_CAPTURE_MODE_A,				// Capture mode
				  GX_CAPTURE_SRCA_2D3D,				// Blend src A
				  GX_CAPTURE_SRCB_VRAM_0x00000,		// Blend src B
				  GX_CAPTURE_DEST_VRAM_D_0x00000,	// Output VRAM
				  16,								// Blend parameter for src A
				  16);								// Blend parameter for src B
	//---------------------------------------------------------------------------
}

