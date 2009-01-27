//=============================================================================================
/**
 * @file	btlv_scu.c
 * @brief	�|�P����WB �o�g�� �`�� ���ʐ��䃂�W���[��
 * @author	taya
 *
 * @date	2008.11.18	�쐬
 */
//=============================================================================================
#include <gflib.h>

#include "print/printsys.h"

#include "btl_common.h"
#include "btl_util.h"
#include "btl_string.h"
#include "btlv_common.h"
#include "btlv_core.h"
#include "btlv_scu.h"

#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"

#include "btl_effect.h"	//soga
#include "poke_tool/monsno_def.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	TEST_STATWIN_BGCOL = 7,
	TEST_STATWIN_BGCOL_FLASH = 4,
	TEST_TOKWIN_BGCOL = 6,
	TEST_TOKWIN_CHAR_WIDTH = 10,
	TEST_TOKWIN_DOT_WIDTH  = TEST_TOKWIN_CHAR_WIDTH*8,

	STRBUF_LEN = 512,
};


typedef struct {
	GFL_BMPWIN*				win;
	GFL_BMP_DATA*			bmp;
	const BTL_POKEPARAM*	bpp;
	BTLV_SCU*				parentWk;
	u16						hp;
	u8						pokePos;
}STATUS_WIN;

typedef struct {
	GFL_BMPWIN*			win;
	GFL_BMP_DATA*		bmp;
	BTLV_SCU*			parentWk;
}TOK_WIN;

//--------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------
struct _BTLV_SCU {

	GFL_BMPWIN*			win;
	GFL_BMP_DATA*		bmp;

	PRINT_QUE*			printQue;
	PRINT_UTIL			printUtil;
	GFL_FONT*			defaultFont;
	GFL_TCBLSYS*		tcbl;
	PRINT_STREAM*		printStream;
	STRBUF*				strBuf;
	u8					taskEndFlag[32];

	STATUS_WIN		statusWin[ BTL_POS_MAX ];
	TOK_WIN				tokWin[ BTL_POS_MAX ];

	BTL_PROC			proc;
	const BTLV_CORE*	vcore;
	const BTL_MAIN_MODULE*	mainModule;
	HEAPID				heapID;
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL btlin_wild_single( int* seq, void* wk_adrs );
static BOOL btlin_wild_double( int* seq, void* wk_adrs );
static void taskDamageEffect( GFL_TCBL* tcbl, void* wk_adrs );
static void taskDeadEffect( GFL_TCBL* tcbl, void* wk_adrs );
static void taskPokeInEffect( GFL_TCBL* tcbl, void* wk_adrs );
static void statwin_setupAll( BTLV_SCU* wk );
static void statwin_cleanupAll( BTLV_SCU* wk );
static void tokwin_setupAll( BTLV_SCU* wk );
static void tokwin_cleanupAll( BTLV_SCU* wk );
static void statwin_setup( STATUS_WIN* stwin, BTLV_SCU* wk, BtlPokePos pokePos );
static void statwin_cleanup( STATUS_WIN* stwin );
static void statwin_reset_data( STATUS_WIN* stwin );
static void statwin_disp_start( STATUS_WIN* stwin );
static void statwin_disp( STATUS_WIN* stwin );
static void statwin_hide( STATUS_WIN* stwin );
static BOOL statwin_erase( STATUS_WIN* stwin, u8 line );
static void statwin_update( STATUS_WIN* stwin, u16 hp, u8 col );
static void tokwin_setup( TOK_WIN* tokwin, BTLV_SCU* wk, u8 clientID );
static void tokwin_cleanup( TOK_WIN* tokwin );
static void tokwin_disp( TOK_WIN* tokwin );
static void tokwin_hide( TOK_WIN* tokwin );



BTLV_SCU*  BTLV_SCU_Create( const BTLV_CORE* vcore, const BTL_MAIN_MODULE* mainModule, GFL_TCBLSYS* tcbl, GFL_FONT* defaultFont, HEAPID heapID )
{
	BTLV_SCU* wk = GFL_HEAP_AllocMemory( heapID, sizeof(BTLV_SCU) );

	wk->vcore = vcore;
	wk->mainModule = mainModule;
	wk->heapID = heapID;

	wk->defaultFont = defaultFont;
	wk->printStream = NULL;
	wk->tcbl = tcbl;
	wk->strBuf = GFL_STR_CreateBuffer( STRBUF_LEN, heapID );

	wk->printQue = PRINTSYS_QUE_Create( wk->heapID );
	PRINT_UTIL_Setup( &wk->printUtil, wk->win );

//	GFL_ASSERT_SetLCDMode();

	return wk;
}

void BTLV_SCU_Setup( BTLV_SCU* wk )
{
	// �ʃt���[���ݒ�
	static const GFL_BG_BGCNT_HEADER bgcntText = {
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x10000, 0x8000,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	};
	// �ʃt���[���ݒ�
	static const GFL_BG_BGCNT_HEADER bgcntStat = {
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x08000, 0x8000,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	};
	// �ʃt���[���ݒ�
	static const GFL_BG_BGCNT_HEADER bgcntTok = {
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000, 0x8000,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	};

	GFL_BG_SetBGControl( GFL_BG_FRAME1_M,   &bgcntText,   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME2_M,   &bgcntTok,   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME3_M,   &bgcntStat,   GFL_BG_MODE_TEXT );

	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0, 0, wk->heapID );
//		void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
	GFL_BG_FillCharacter( GFL_BG_FRAME1_M, 0x00, 1, 0 );
	GFL_BG_FillCharacter( GFL_BG_FRAME1_M, 0xaa, 9, 1 );
	GFL_BG_FillScreen( GFL_BG_FRAME1_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

	GFL_BG_FillCharacter( GFL_BG_FRAME3_M, 0x00, 1, 0 );
	GFL_BG_FillCharacter( GFL_BG_FRAME3_M, 0xff, 1, 1 );
	GFL_BG_FillScreen( GFL_BG_FRAME2_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
//	GFL_BG_FillScreen( GFL_BG_FRAME3_M, 0x0001, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( GFL_BG_FRAME3_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );


	wk->win = GFL_BMPWIN_Create( GFL_BG_FRAME1_M, 1, 19, 30, 4, 0, GFL_BMP_CHRAREA_GET_F );
	wk->bmp = GFL_BMPWIN_GetBmp( wk->win );
	GFL_BMPWIN_MakeScreen( wk->win );
	GFL_BMPWIN_MakeFrameScreen( wk->win, 1, 0 );
	GFL_BMP_Clear( wk->bmp, 0x0f );
	GFL_BMPWIN_TransVramCharacter( wk->win );

	statwin_setupAll( wk );
	tokwin_setupAll( wk );

	GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME3_M );

//	GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
	GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_ON  );
	GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_ON  );
	GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_ON );
}

void BTLV_SCU_Delete( BTLV_SCU* wk )
{
	tokwin_cleanupAll( wk );
	statwin_cleanupAll( wk );

	GFL_BMPWIN_Delete( wk->win );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );

	PRINTSYS_QUE_Delete( wk->printQue );
	GFL_STR_DeleteBuffer( wk->strBuf );
	GFL_HEAP_FreeMemory( wk );
}



//=============================================================================================
/**
 * �o�g����ʏ����Z�b�g�A�b�v�����܂ł̉��o���J�n����
 *
 * @param   wk		
 *
 */
//=============================================================================================
void BTLV_SCU_StartBtlIn( BTLV_SCU* wk )
{
	// @@@ ������̓g���[�i�[�킩�ǂ����Ȃǂł������ʂ���K�v����
	if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE )
	{
		BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_wild_single );
	}
	else
	{
		BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_wild_double );
	}
}

//=============================================================================================
/**
 * �o�g����ʏ����Z�b�g�A�b�v�����܂ł̉��o���I������܂ő҂�
 *
 * @param   wk		
 *
 * @retval  BOOL		�I��������TRUE
 */
//=============================================================================================
BOOL BTLV_SCU_WaitBtlIn( BTLV_SCU* wk )
{
	return BTL_UTIL_CallProc( &wk->proc );
}
//--------------------------------------------------------------------------
/**
 * �퓬��ʃZ�b�g�A�b�v�����܂ł̉��o�i�쐶�^�V���O���j
 * @retval  BOOL		
 */
//--------------------------------------------------------------------------
static BOOL btlin_wild_single( int* seq, void* wk_adrs )
{
	BTLV_SCU* wk = wk_adrs;

	switch( *seq ){
	case 0:
		BTL_STR_MakeStringStd( wk->strBuf, BTL_STRID_STD_Encount );
		BTLV_SCU_StartMsg( wk, wk->strBuf );
		(*seq)++;
		break;
	case 1:
		if( BTLV_SCU_WaitMsg(wk) )
		{
			u8 plClientID, enClientID;

			plClientID = BTLV_CORE_GetPlayerClientID( wk->vcore );
			enClientID = BTL_MAIN_GetOpponentClientID( wk->mainModule, plClientID, 0 );

			statwin_disp_start( &wk->statusWin[ enClientID ] );
			(*seq)++;

			//soga
			{
				const BTL_POKEPARAM* bpp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, BTL_POS_2ND_0 );
				BTL_EFFECT_SetPokemon( BTL_POKEPARAM_GetSrcData( bpp ), POKE_MCSS_POS_BB );
			}
		}
		break;
	case 2:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
		{
			BTL_STR_MakeStringStd( wk->strBuf, BTL_STRID_STD_PutSingle );
			BTLV_SCU_StartMsg( wk, wk->strBuf );
			(*seq)++;
		}
		break;
	case 3:
		if( BTLV_SCU_WaitMsg(wk) )
		{
			u8 plClientID = BTLV_CORE_GetPlayerClientID( wk->vcore );
			statwin_disp_start( &wk->statusWin[ plClientID ] );
			(*seq)++;

			//soga
			{
				const BTL_POKEPARAM* bpp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, BTL_POS_1ST_0 );
				BTL_EFFECT_SetPokemon( BTL_POKEPARAM_GetSrcData( bpp ), POKE_MCSS_POS_AA );
			}
		}
		break;
	case 4:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
		{
			return TRUE;
		}
		break;
	}

	return FALSE;
}
//--------------------------------------------------------------------------
/**
 * �퓬��ʃZ�b�g�A�b�v�����܂ł̉��o�i�쐶�^�_�u���j
 * @retval  BOOL		TRUE�ŏI��
 */
//--------------------------------------------------------------------------
static BOOL btlin_wild_double( int* seq, void* wk_adrs )
{
	BTLV_SCU* wk = wk_adrs;

	switch( *seq ){
	case 0:
		BTL_STR_MakeStringStd( wk->strBuf, BTL_STRID_STD_Encount_Double );
		BTLV_SCU_StartMsg( wk, wk->strBuf );
		(*seq)++;
		break;
	case 1:
		if( BTLV_SCU_WaitMsg(wk) )
		{
				const BTL_POKEPARAM* bpp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, BTL_POS_2ND_0 );
				BTL_EFFECT_SetPokemon( BTL_POKEPARAM_GetSrcData( bpp ), POKE_MCSS_POS_B );
				(*seq)++;
		}
		break;
	case 2:
		if( !BTL_EFFECT_CheckExecute() )
		{
				const BTL_POKEPARAM* bpp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, BTL_POS_2ND_1 );
				BTL_EFFECT_SetPokemon( BTL_POKEPARAM_GetSrcData(bpp), POKE_MCSS_POS_D );
				(*seq)++;
		}
		break;
	case 3:
		if( !BTL_EFFECT_CheckExecute() )
		{
				BTL_STR_MakeStringStd( wk->strBuf, BTL_STRID_STD_PutDouble );
				BTLV_SCU_StartMsg( wk, wk->strBuf );
				(*seq)++;
		}
		break;
	case 4:
		if( BTLV_SCU_WaitMsg(wk) )
		{
				const BTL_POKEPARAM* bpp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, BTL_POS_1ST_0 );
				BTL_EFFECT_SetPokemon( BTL_POKEPARAM_GetSrcData(bpp), POKE_MCSS_POS_A );
				(*seq)++;
		}
		break;
	case 5:
		if( !BTL_EFFECT_CheckExecute() )
		{
				const BTL_POKEPARAM* bpp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, BTL_POS_1ST_1 );
				BTL_EFFECT_SetPokemon( BTL_POKEPARAM_GetSrcData(bpp), POKE_MCSS_POS_C );
				(*seq)++;
		}
		break;
	case 6:
		if( !BTL_EFFECT_CheckExecute() )
		{
			return TRUE;
		}
	}
	return FALSE;
}



//=============================================================================================
/**
 * �Ƃ������E�B���h�E�\���I��
 *
 * @param   wk		
 * @param   pos		
 *
 */
//=============================================================================================
void BTLV_SCU_DispTokWin( BTLV_SCU* wk, u8 clientID )
{
	tokwin_disp( &wk->tokWin[clientID] );
}
//=============================================================================================
/**
 * �Ƃ������E�B���h�E�\���I�t
 *
 * @param   wk		
 * @param   clientID		
 *
 */
//=============================================================================================
void BTLV_SCU_HideTokWin( BTLV_SCU* wk, u8 clientID )
{
	tokwin_hide( &wk->tokWin[clientID] );
}

//----------------------------------------------

//=============================================================================================
/**
 * ���b�Z�[�W�\���J�n
 *
 * @param   wk		
 * @param   str		
 *
 */
//=============================================================================================
void BTLV_SCU_StartMsg( BTLV_SCU* wk, const STRBUF* str )
{
	GF_ASSERT( wk->printStream == NULL );

	GFL_BMP_Clear( wk->bmp, 0x0f );

	wk->printStream = PRINTSYS_PrintStream(
				wk->win, 0, 0, str, wk->defaultFont, 0, wk->tcbl, BTLV_TASKPRI_MAIN_WINDOW,
				wk->heapID, 0x0f
	);
}

//=============================================================================================
/**
 * ���b�Z�[�W�\���I���҂�
 *
 * @param   wk		
 *
 * @retval  BOOL		
 */
//=============================================================================================
BOOL BTLV_SCU_WaitMsg( BTLV_SCU* wk )
{
	if( wk->printStream )
	{
		if( PRINTSYS_PrintStreamGetState( wk->printStream ) == PRINTSTREAM_STATE_DONE )
		{
			PRINTSYS_PrintStreamDelete( wk->printStream );
			wk->printStream = NULL;
		}
		return FALSE;
	}
	return TRUE;
}

//==============================================================================
typedef struct {

	STATUS_WIN*  statWin;
	fx32		hpVal;
	fx32		hpMinusVal;
	u16			hpEnd;
	u16			timer;
	u8*			endFlag;

}DMG_EFF_TASK_WORK;


//=============================================================================================
/**
 * ���U�G�t�F�N�g����
 *
 * @param   wk->scrnU		
 * @param   atClientID		
 * @param   defClientID		
 * @param   waza		
 * @param   affinity		
 *
 */
//=============================================================================================
void BTLV_SCU_StartWazaAct( BTLV_SCU* wk, BtlPokePos atPos, BtlPokePos defPos, WazaID waza, BtlTypeAff affinity )
{
	enum {
		DAMAGE_FRAME_MIN = 40,
	};

	GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskDamageEffect, sizeof(DMG_EFF_TASK_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
	DMG_EFF_TASK_WORK* twk = GFL_TCBL_GetWork( tcbl );

	twk->endFlag = &(wk->taskEndFlag[0]);
	twk->statWin = &wk->statusWin[defPos];
	twk->hpEnd = BTL_POKEPARAM_GetValue( twk->statWin->bpp, BPP_HP );
	twk->hpVal = FX32_CONST( twk->statWin->hp );
	{
		u16 damage = twk->statWin->hp - twk->hpEnd;
		twk->timer = (damage * 180) / 100;
		if( twk->timer < DAMAGE_FRAME_MIN )
		{
			twk->timer = DAMAGE_FRAME_MIN;
		}

		twk->hpMinusVal = FX32_CONST(twk->statWin->hp - twk->hpEnd) / twk->timer;
	}

	// �Z�G�t�F�N�g�o���Ă݂� soga
	{
		u8 atViewPos, defViewPos;

		atViewPos  = BTL_MAIN_BtlPosToViewPos( wk->mainModule, atPos );
		defViewPos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, defPos );
		BTL_EFFECT_AddByPos( atViewPos, defViewPos, waza );
	}

	*(twk->endFlag) = FALSE;
}

//=============================================================================================
/**
 * �킴�G�t�F�N�g�I���҂�
 *
 * @param   wk		
 *
 * @retval  BOOL		
 */
//=============================================================================================
BOOL BTLV_SCU_WaitWazaAct( BTLV_SCU* wk )
{
	return (wk->taskEndFlag[0]);
}
static void taskDamageEffect( GFL_TCBL* tcbl, void* wk_adrs )
{
	DMG_EFF_TASK_WORK* wk = wk_adrs;

	if( wk->timer )
	{
		u16 hp;
		u8 col;

		wk->timer--;
		wk->hpVal -= wk->hpMinusVal;
		hp = wk->hpVal >> FX32_SHIFT;
		col = ((wk->timer % 16) <= 7)? TEST_STATWIN_BGCOL : TEST_STATWIN_BGCOL_FLASH;
		statwin_update( wk->statWin, hp, col );
	}
	else
	{
		statwin_update( wk->statWin, wk->hpEnd, TEST_STATWIN_BGCOL );
		*(wk->endFlag) = TRUE;
		GFL_TCBL_Delete( tcbl );
	}
}

//------------------------------------
typedef struct {

	STATUS_WIN*  statWin;
	u16			timer;
	u16			line;
	u8*			endFlag;

}DEAD_EFF_WORK;



//=============================================================================================
/**
 * �|�P�����Ђ񂵃A�N�V�����J�n
 *
 * @param   wk			
 * @param   pos		�Ђ񂵂ɂȂ����|�P�����̈ʒuID	
 *
 */
//=============================================================================================
void BTLV_SCU_StartDeadAct( BTLV_SCU* wk, BtlPokePos pos )
{
	GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskDeadEffect, sizeof(DEAD_EFF_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
	DEAD_EFF_WORK* twk = GFL_TCBL_GetWork( tcbl );

	twk->statWin = &wk->statusWin[pos];
	twk->endFlag = &wk->taskEndFlag[0];
	twk->timer = 0;
	twk->line = 0;

	*(twk->endFlag) = FALSE;

	//soga
	BTL_EFFECT_DelPokemon( BTL_MAIN_BtlPosToViewPos(wk->mainModule, pos) );

}
//=============================================================================================
/**
 * �|�P�����Ђ񂵃A�N�V�����I���҂�
 *
 * @param   wk			
 *
 * @retval  BOOL		
 */
//=============================================================================================
BOOL BTLV_SCU_WaitDeadAct( BTLV_SCU* wk )
{
	return (wk->taskEndFlag[0]);
}

static void taskDeadEffect( GFL_TCBL* tcbl, void* wk_adrs )
{
	DEAD_EFF_WORK* wk = wk_adrs;

	if( ++(wk->timer) > 4 )
	{
		wk->timer = 0;
		wk->line++;
		if( statwin_erase(wk->statWin, wk->line) )
		{
			*(wk->endFlag) = TRUE;
			GFL_TCBL_Delete( tcbl );
		}
	}
}
//--------------------------------------------------------
typedef struct {

	STATUS_WIN*  statWin;
	u16			seq;
	u16			line;
	u8*			endFlag;

}POKEIN_ACT_WORK;


//=============================================================================================
/**
 * �|�P��������A�N�V�����J�n
 *
 * @param   wk		
 * @param   clientID		
 *
 */
//=============================================================================================
void BTLV_SCU_StartPokeIn( BTLV_SCU* wk, BtlPokePos pos )
{
	GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskPokeInEffect, sizeof(POKEIN_ACT_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
	POKEIN_ACT_WORK* twk = GFL_TCBL_GetWork( tcbl );

	twk->statWin = &wk->statusWin[ pos ];
	twk->endFlag = &wk->taskEndFlag[0];
	twk->seq = 0;

	*(twk->endFlag) = FALSE;

	//soga
	{
		const BTL_POKEPARAM* bpp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, pos );
		BTL_EFFECT_SetPokemon( BTL_POKEPARAM_GetSrcData( bpp ), BTL_MAIN_BtlPosToViewPos(wk->mainModule,pos) );
	}
}


BOOL BTLV_SCU_WaitPokeIn( BTLV_SCU* wk )
{
	return wk->taskEndFlag[0];
}

static void taskPokeInEffect( GFL_TCBL* tcbl, void* wk_adrs )
{
	POKEIN_ACT_WORK* wk = wk_adrs;

	switch( wk->seq ){
	case 0:
		statwin_reset_data( wk->statWin );
		wk->seq++;
		break;
	case 1:
		statwin_disp_start( wk->statWin );
		wk->seq++;
		break;
	case 2:
		*(wk->endFlag) = TRUE;
		GFL_TCBL_Delete( tcbl );
	}
}



//----------------------------

static void statwin_setupAll( BTLV_SCU* wk )
{
	int i;
//	u8 plClientID, enClientID, i;

	for(i=0; i<NELEMS(wk->statusWin); i++)
	{
		wk->statusWin[i].win = NULL;
	}

/*
	plClientID = BTLV_CORE_GetPlayerClientID( wk->vcore );
	enClientID = BTL_MAIN_GetOpponentClientID( wk->mainModule, plClientID, 0 );

	statwin_setup( &wk->statusWin[ plClientID ], wk, plClientID );
	statwin_setup( &wk->statusWin[ enClientID ], wk, enClientID );
*/
	statwin_setup( &wk->statusWin[ BTL_POS_1ST_0 ], wk, BTL_POS_1ST_0 );
	statwin_setup( &wk->statusWin[ BTL_POS_2ND_0 ], wk, BTL_POS_2ND_0 );

	if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_SINGLE )
	{
		statwin_setup( &wk->statusWin[ BTL_POS_1ST_1 ], wk, BTL_POS_1ST_1 );
		statwin_setup( &wk->statusWin[ BTL_POS_2ND_1 ], wk, BTL_POS_2ND_1 );
	}
}

static void statwin_cleanupAll( BTLV_SCU* wk )
{
	int i;
	for(i=0; i<NELEMS(wk->statusWin); i++)
	{
		if( wk->statusWin[i].win != NULL)
		{
			statwin_cleanup( &wk->statusWin[i] );
		}
	}
}

static void tokwin_setupAll( BTLV_SCU* wk )
{
	u8 plClientID, enClientID, i;

	for(i=0; i<NELEMS(wk->tokWin); i++)
	{
		wk->tokWin[i].win = NULL;
	}

	plClientID = BTLV_CORE_GetPlayerClientID( wk->vcore );
	enClientID = BTL_MAIN_GetOpponentClientID( wk->mainModule, plClientID, 0 );

	tokwin_setup( &wk->tokWin[ BTL_POS_1ST_0 ], wk, BTL_POS_1ST_0 );
	tokwin_setup( &wk->tokWin[ BTL_POS_2ND_0 ], wk, BTL_POS_2ND_0 );

	if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_SINGLE )
	{
		tokwin_setup( &wk->tokWin[ BTL_POS_1ST_1 ], wk, BTL_POS_1ST_1 );
		tokwin_setup( &wk->tokWin[ BTL_POS_2ND_1 ], wk, BTL_POS_2ND_1 );
	}
}

static void tokwin_cleanupAll( BTLV_SCU* wk )
{
	int i;
	for(i=0; i<NELEMS(wk->tokWin); i++)
	{
		if( wk->tokWin[i].win != NULL)
		{
			tokwin_cleanup( &wk->tokWin[i] );
		}
	}
}




static void statwin_setup( STATUS_WIN* stwin, BTLV_SCU* wk, BtlPokePos pokePos )
{
	static const struct {
		u8 x;
		u8 y;
	} winpos[] = {
		{  4,  2 },
		{ 18, 13 },

		{ 11,  3 },
		{ 10, 13 },
		{  0,  2 },
		{ 21, 14 },
	};

	u8 viewPos, px, py;

	TAYA_Printf("[STATWIN Setup] pokePos=%d\n", pokePos);
	stwin->pokePos = pokePos;
	stwin->parentWk = wk;

	viewPos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pokePos );
	px = winpos[viewPos].x;
	py = winpos[viewPos].y;

	stwin->win = GFL_BMPWIN_Create( GFL_BG_FRAME3_M, px, py, 10, 4, 0, GFL_BMP_CHRAREA_GET_F );
	stwin->bmp = GFL_BMPWIN_GetBmp( stwin->win );

	statwin_reset_data( stwin );
}

static void statwin_cleanup( STATUS_WIN* stwin )
{
	GFL_BMPWIN_Delete( stwin->win );
}

static void statwin_reset_data( STATUS_WIN* stwin )
{
	BTLV_SCU* wk = stwin->parentWk;

	stwin->bpp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, stwin->pokePos );
	stwin->hp = BTL_POKEPARAM_GetValue( stwin->bpp, BPP_HP );

	GFL_BMP_Clear( stwin->bmp, TEST_STATWIN_BGCOL );
	BTL_STR_MakeStatusWinStr( wk->strBuf, stwin->bpp, stwin->hp );
	PRINTSYS_Print( stwin->bmp, 0, 0, wk->strBuf, wk->defaultFont );
}


static void statwin_disp_start( STATUS_WIN* stwin )
{
	GFL_BMPWIN_TransVramCharacter( stwin->win );
	GFL_BMPWIN_MakeScreen( stwin->win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(stwin->win) );
}

static void statwin_disp( STATUS_WIN* stwin )
{
	GFL_BMPWIN_MakeScreen( stwin->win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(stwin->win) );
}

static void statwin_hide( STATUS_WIN* stwin )
{
	GFL_BMPWIN_ClearScreen( stwin->win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(stwin->win) );
}

// �P�s�������B�S����������TRUE
static BOOL statwin_erase( STATUS_WIN* stwin, u8 line )
{
	u8 px, py, width, height, frame;

	px     = GFL_BMPWIN_GetPosX( stwin->win );
	py     = GFL_BMPWIN_GetPosY( stwin->win );
	width  = GFL_BMPWIN_GetSizeX( stwin->win );
	height = GFL_BMPWIN_GetSizeY( stwin->win );
	frame  = GFL_BMPWIN_GetFrame( stwin->win );

	if( line > height )
	{
		line = height;
	}

	GFL_BG_FillScreen( frame, 0, px, py, width, line, GFL_BG_SCRWRT_PALIN );
	GFL_BG_LoadScreenReq( frame );

	return line == height;
}

static void statwin_update( STATUS_WIN* stwin, u16 hp, u8 col )
{
	BTLV_SCU* wk = stwin->parentWk;

	stwin->hp = hp;

	GFL_BMP_Clear( stwin->bmp, col );
	BTL_STR_MakeStatusWinStr( wk->strBuf, stwin->bpp, stwin->hp );
	PRINTSYS_Print( stwin->bmp, 0, 0, wk->strBuf, wk->defaultFont );
	GFL_BMPWIN_TransVramCharacter( stwin->win );
}

//----------------------------

static void tokwin_setup( TOK_WIN* tokwin, BTLV_SCU* wk, u8 clientID )
{
	static const struct {
		u8 x;
		u8 y;
	} winpos[2] = {
		{ 18,  3 },
		{  4, 14 },
	};

	u8 isPlayer, playerClientID, px, py;

	tokwin->parentWk = wk;

	playerClientID = BTLV_CORE_GetPlayerClientID( wk->vcore );
	isPlayer = !BTL_MAIN_IsOpponentClientID( wk->mainModule, playerClientID, clientID );
	px = winpos[isPlayer].x;
	py = winpos[isPlayer].y;

	tokwin->win = GFL_BMPWIN_Create( GFL_BG_FRAME2_M, px, py, TEST_TOKWIN_CHAR_WIDTH, 2, 0, GFL_BMP_CHRAREA_GET_F );
	tokwin->bmp = GFL_BMPWIN_GetBmp( tokwin->win );
	GFL_BMP_Clear( tokwin->bmp, TEST_TOKWIN_BGCOL );

	{
		const BTL_POKEPARAM* bpp;
		GFL_MSGDATA* msg;
		u16 tokusei;
		u16 xpos;

		msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_tokusei_dat, GFL_HEAP_LOWID(wk->heapID) );
		bpp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, clientID );
		tokusei = BTL_POKEPARAM_GetValue( bpp, BPP_TOKUSEI );
		GFL_MSG_GetString( msg, tokusei, wk->strBuf );
		xpos = (TEST_TOKWIN_DOT_WIDTH - PRINTSYS_GetStrWidth(wk->strBuf, wk->defaultFont, 0)) / 2;
		PRINTSYS_Print( tokwin->bmp, xpos, 2, wk->strBuf, wk->defaultFont );
		GFL_MSG_Delete( msg );
	}

	GFL_BMPWIN_TransVramCharacter( tokwin->win );
}

static void tokwin_cleanup( TOK_WIN* tokwin )
{
	GFL_BMPWIN_Delete( tokwin->win );
}

static void tokwin_disp( TOK_WIN* tokwin )
{
	GFL_BMPWIN_MakeScreen( tokwin->win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(tokwin->win) );
}

static void tokwin_hide( TOK_WIN* tokwin )
{
	GFL_BMPWIN_ClearScreen( tokwin->win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(tokwin->win) );
}
