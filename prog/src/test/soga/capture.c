//============================================================================================
/**
 * @file	capture.c
 * @brief	MCS�𗘗p������ʃL���v�`������
 * @author	soga
 * @date	2009.03.24
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
#include "battle/battgra_wb.naix"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#define MCS_ENABLE		//MCS���g�p����
#define BTLV_MCSS_1vs1		//1vs1�`��

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

// Windows�A�v���P�[�V�����Ƃ̎��ʂŎg�p����`�����l���l�ł�
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
	HEAPID				heapID;
	NNSSndStrm			strm;
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
	int					position;
	BTLV_MCSS_WORK		*bmw;
	int					key_repeat_speed;
	int					key_repeat_wait;
	int					ortho_mode;
	VecFx32				scale;
}SOGA_WORK;

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
static GFL_PROC_RESULT CaptureTestProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
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
			GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
			GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
			GX_VRAM_SUB_BG_32_H,			// �T�u2D�G���W����BG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
			GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
			GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
			GX_VRAM_TEX_01_BC,				// �e�N�X�`���C���[�W�X���b�g
			GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g			
			GX_OBJVRAMMODE_CHAR_1D_64K,		// ���C��OBJ�}�b�s���O���[�h
			GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
		};		
		GFL_DISP_SetBank( &dispvramBank );

		//VRAM�N���A
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

	//3D�֘A������
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
		};
		GFL_BG_SetBGControl(GFL_BG_FRAME1_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME1_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME3_M, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_M );

		GFL_ARC_UTIL_TransVramBgCharacter(ARCID_BATTGRA,NARC_battgra_wb_batt_bg1_NCGR,GFL_BG_FRAME1_M,0,0,0,wk->heapID);
		GFL_ARC_UTIL_TransVramScreen(ARCID_BATTGRA,NARC_battgra_wb_batt_bg1_NSCR,GFL_BG_FRAME1_M,0,0,0,wk->heapID);
		GFL_ARC_UTIL_TransVramBgCharacter(ARCID_BATTGRA,NARC_battgra_wb_batt_bg2_NCGR,GFL_BG_FRAME2_M,0,0,0,wk->heapID);
		GFL_ARC_UTIL_TransVramScreen(ARCID_BATTGRA,NARC_battgra_wb_batt_bg2_NSCR,GFL_BG_FRAME2_M,0,0,0,wk->heapID);
		GFL_ARC_UTIL_TransVramPalette(ARCID_BATTGRA,NARC_battgra_wb_batt_bg_NCLR,PALTYPE_MAIN_BG,0,0x100,wk->heapID);
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
	
	//�L���v�`���Z�b�g
	GFUser_VIntr_CreateTCB( Capture_VBlankIntr, NULL, 0 );

	//�t�F�[�h�C��
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
static GFL_PROC_RESULT CaptureTestProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	int pad = GFL_UI_KEY_GetCont();
	int trg = GFL_UI_KEY_GetTrg();
	int rep = GFL_UI_KEY_GetRepeat();
	int tp = GFL_UI_TP_GetTrg();
	SOGA_WORK* wk = mywk;

	if( wk->mcs_enable ){
		NNS_McsPollingIdle();
		MCS_Read( wk );
		MCS_Write( wk );
	}

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

	MoveCamera( wk );

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
static GFL_PROC_RESULT CaptureTestProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	SOGA_WORK* wk = mywk;

	if( GFL_FADE_CheckFade() == TRUE ){
		return GFL_PROC_RES_CONTINUE;	
	}

	GFL_UI_KEY_SetRepeatSpeed( wk->key_repeat_speed, wk->key_repeat_wait );

	BTLV_EFFECT_Exit();

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
const GFL_PROC_DATA		CaptureTestProcData = {
	CaptureTestProcInit,
	CaptureTestProcMain,
	CaptureTestProcExit,
};

//======================================================================
//	�J�����̐���
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
//		OS_TPrintf("pos_x:%08x pos_y:%08x pos_z:%08x tar_x:%08x tar_y:%08x tar_z:%08x\n",
//			camPos.x,
//			camPos.y,
//			camPos.z,
//			camTarget.x,
//			camTarget.y,
//			camTarget.z );
	}

}
 
static	void	set_pokemon( SOGA_WORK *wk )
{
	//POKEMON_PARAM����
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
		PP_Put( pp, ID_PARA_monsno, MONSNO_ARUSEUSU + 1 );
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
//	�J�����̏�����
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
//	�J�����̐���
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
//	�J�����ʒu�̌v�Z
//======================================================================
static	void	CalcCamera( CAMERA_WORK *cw )
{
	MtxFx43	scale,trans;
	VecFx32	pos;

	//phi��theta��scale����camPos���v�Z
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

//
//	MCS�������
//
static BOOL MCS_Init( SOGA_WORK *wk )
{
	if(	wk->mcs_enable ) return FALSE;

	wk->mw.phi = 0;
	wk->mw.theta = 0;

	// mcs�̏�����
	wk->mcsWorkMem = GFL_HEAP_AllocMemory( wk->heapID, NNS_MCS_WORKMEM_SIZE ); // MCS�̃��[�N�p���������m��
	OS_TPrintf("size:%08x\n",NNS_MCS_WORKMEM_SIZE);
	NNS_McsInit( wk->mcsWorkMem );

	GFUser_VIntr_CreateTCB( MCS_VBlankIntr, NULL, 0 );

	// �f�o�C�X�̃I�[�v��
	if( NNS_McsGetMaxCaps() > 0 && NNS_McsOpen( &wk->deviceCaps ) )
	{
		wk->printBuffer = GFL_HEAP_AllocMemory( wk->heapID, 1024 );        // �v�����g�p�̃o�b�t�@�̊m��
		wk->recvBuf = GFL_HEAP_AllocMemory( wk->heapID, 1024 );       // ��M�p�o�b�t�@�̊m��

        NNS_NULL_ASSERT(wk->printBuffer);
        NNS_NULL_ASSERT(wk->recvBuf);

        // OS_Printf�ɂ��o��
        OS_Printf("device open\n");

        // mcs������o�͂̏�����
		NNS_McsInitPrint( wk->printBuffer, 1024 );

        // NNS_McsPrintf�ɂ��o��
        // ���̃^�C�~���O��mcs�T�[�o���ڑ����Ă���΁A�R���\�[���ɕ\������܂��B
		(void)NNS_McsPrintf("device ID %08X\n", wk->deviceCaps.deviceID );

        // �ǂݎ��p�o�b�t�@�̓o�^
		NNS_McsRegisterStreamRecvBuffer(MCS_CHANNEL0, wk->recvBuf, 1024 );

        // ��M�R�[���o�b�N�֐��̓o�^
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

	// NNS_McsPutString�ɂ��o��
	(void)NNS_McsPutString("device close\n");

	// �f�o�C�X���N���[�Y
	(void)NNS_McsClose();

	wk->mcs_enable = 0;
	wk->mcs_idle = 0;
}

static void MCS_Read( SOGA_WORK *wk )
{
	int			len;
	MCS_WORK	buf;

	// ��M�\�ȃf�[�^�T�C�Y�̎擾
	len = NNS_McsGetStreamReadableSize( MCS_CHANNEL0 );

	if( len > 0 ){
		u32 readSize;
		// �f�[�^�̓ǂݎ��
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
		// �f�[�^�̏�������
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

  Description:  PC������f�[�^����M�����Ƃ��ɌĂ΂��R�[���o�b�N�֐��ł��B

                �o�^����R�[���o�b�N�֐����ł̓f�[�^�̑���M���s��Ȃ��ł��������B
                �܂��A���荞�݂��֎~����Ă���ꍇ�����邽�߁A
                ���荞�ݑ҂����[�v���s��Ȃ��ł��������B

  Arguments:    recv:       ��M�����f�[�^�̈ꕔ���邢�͑S�����i�[���Ă���
                            �o�b�t�@�ւ̃|�C���^�B
                recvSize:   recv�ɂ���Ď������o�b�t�@�Ɋi�[����Ă���
                            �f�[�^�̃T�C�Y�B
                userData:   NNS_McsRegisterRecvCallback()�̈���userData��
                            �w�肵���l�B
                offset:     ��M�����f�[�^�̑S����recv�ɂ���Ď������o�b�t�@��
                            �i�[����Ă���ꍇ��0�B
                            ��M�����f�[�^�̈ꕔ���i�[����Ă���ꍇ�́A
                            ��M�����f�[�^�S�̂ɑ΂���0����Ƃ����I�t�Z�b�g�ʒu�B
                totalSize:  ��M�����f�[�^�̑S�̂̃T�C�Y�B

  Returns:      �Ȃ��B
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

	// ��M�o�b�t�@�`�F�b�N
	if (pRecv != NULL && recvSize == sizeof(MCS_WORK) && offset == 0)
	{
        mw = (MCS_WORK *)pRecv;
    }
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V�u�����N���荞�݃n���h���ł��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void MCS_VBlankIntr( GFL_TCB *tcb, void *work )
{
    NNS_McsVBlankInterrupt();
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V�u�����N���荞�݃n���h���ł��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
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

