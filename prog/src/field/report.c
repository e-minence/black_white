//============================================================================================
/**
 * @file		report.c
 * @brief		���|�[�g�����
 * @author	Hiroyuki Nakamura
 * @date		10.02.18
 *
 *	���W���[�����FREPORT
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "pm_define.h"
#include "system/gfl_use.h"
#include "system/main.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "pokeicon/pokeicon.h"
#include "font/font.naix"
#include "field/zonedata.h"
#include "field/eventwork.h"
#include "savedata/playtime.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"

#include "report.h"
#include "report_gra.naix"

#include "message.naix"
#include "msg/msg_report.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �^�C�g��
#define	BMPWIN_TITLE_FRM	( GFL_BG_FRAME1_S )
#define	BMPWIN_TITLE_PX		( 1 )
#define	BMPWIN_TITLE_PY		( 0 )
#define	BMPWIN_TITLE_SX		( 15 )
#define	BMPWIN_TITLE_SY		( 3 )
#define	BMPWIN_TITLE_PAL	( 2 )
// ���t
#define	BMPWIN_DATE_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_DATE_PX		( 3 )
#define	BMPWIN_DATE_PY		( 7 )
#define	BMPWIN_DATE_SX		( 10 )
#define	BMPWIN_DATE_SY		( 2 )
#define	BMPWIN_DATE_PAL		( 2 )
// ����
#define	BMPWIN_DATE2_FRM	( GFL_BG_FRAME1_S )
#define	BMPWIN_DATE2_PX		( 14 )
#define	BMPWIN_DATE2_PY		( 7 )
#define	BMPWIN_DATE2_SX		( 5 )
#define	BMPWIN_DATE2_SY		( 2 )
#define	BMPWIN_DATE2_PAL	( 2 )
// �ꏊ
#define	BMPWIN_PLACE_FRM	( GFL_BG_FRAME1_S )
#define	BMPWIN_PLACE_PX		( 3 )
#define	BMPWIN_PLACE_PY		( 9 )
#define	BMPWIN_PLACE_SX		( 15 )
#define	BMPWIN_PLACE_SY		( 2 )
#define	BMPWIN_PLACE_PAL	( 2 )
// �W���o�b�W
#define	BMPWIN_BADGE_FRM	( GFL_BG_FRAME1_S )
#define	BMPWIN_BADGE_PX		( 3 )
#define	BMPWIN_BADGE_PY		( 16 )
#define	BMPWIN_BADGE_SX		( 12 )
#define	BMPWIN_BADGE_SY		( 2 )
#define	BMPWIN_BADGE_PAL	( 2 )
// �}��
#define	BMPWIN_ZUKAN_FRM	( GFL_BG_FRAME1_S )
#define	BMPWIN_ZUKAN_PX		( 16 )
#define	BMPWIN_ZUKAN_PY		( 16 )
#define	BMPWIN_ZUKAN_SX		( 12 )
#define	BMPWIN_ZUKAN_SY		( 2 )
#define	BMPWIN_ZUKAN_PAL	( 2 )
// �v���C����
#define	BMPWIN_TIME_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_TIME_PX		( 3 )
#define	BMPWIN_TIME_PY		( 18 )
#define	BMPWIN_TIME_SX		( 16 )
#define	BMPWIN_TIME_SY		( 2 )
#define	BMPWIN_TIME_PAL		( 2 )
// �O��̃��|�[�g
#define	BMPWIN_REPORT_FRM	( GFL_BG_FRAME1_S )
#define	BMPWIN_REPORT_PX	( 1 )
#define	BMPWIN_REPORT_PY	( 21 )
#define	BMPWIN_REPORT_SX	( 30 )
#define	BMPWIN_REPORT_SY	( 3 )
#define	BMPWIN_REPORT_PAL	( 2 )

// BMPWIN INDEX
enum {
	BMPWIN_TITLE = 0,		// �^�C�g��
	BMPWIN_DATE,				// ���t
	BMPWIN_DATE2,				// ����
	BMPWIN_PLACE,				// �ꏊ
	BMPWIN_BADGE,				// �W���o�b�W
	BMPWIN_ZUKAN,				// �}��
	BMPWIN_TIME,				// �v���C����
	BMPWIN_REPORT,			// �O��̃��|�[�g
	BMPWIN_MAX
};

enum {
	// �L�������\�[�X
	CHRRES_POKE1 = 0,
	CHRRES_POKE2,
	CHRRES_POKE3,
	CHRRES_POKE4,
	CHRRES_POKE5,
	CHRRES_POKE6,
	CHRRES_TIME,
	CHRRES_MAX,
	// �p���b�g���\�[�X
	PALRES_POKE,
	PALRES_TIME,
	PALRES_MAX,
	// �Z�����\�[�X
	CELRES_POKE,
	CELRES_TIME,
	CELRES_MAX,
};

// OBJ ID
enum {
	OBJID_POKE1 = 0,
	OBJID_POKE2,
	OBJID_POKE3,
	OBJID_POKE4,
	OBJID_POKE5,
	OBJID_POKE6,
	OBJID_TIME01,
	OBJID_TIME02,
	OBJID_TIME03,
	OBJID_TIME04,
	OBJID_TIME05,
	OBJID_TIME06,
	OBJID_TIME07,
	OBJID_TIME08,
	OBJID_TIME09,
	OBJID_TIME10,
	OBJID_MAX,
};

#define FCOL_P02BN		( PRINTSYS_LSB_Make(15,14,0) )	// �t�H���g�J���[�F�O�Q����
#define FCOL_P02WN		( PRINTSYS_LSB_Make(13,12,0) )	// �t�H���g�J���[�F�O�Q����

// �|�P�����A�C�R���\�����W
#define	POKEICON_PX		( 40 )
#define	POKEICON_PY		( 104 )
#define	POKEICON_SX		( 32 )
// �Z�[�u�󋵃}�[�N�\�����W
#define	TIMEMARK_PX		( 74 )
#define	TIMEMARK_PY		( 180 )
#define	TIMEMARK_SX		( 12 )

#define	TIMEMARK_WAIT	( 8 )		// �^�C�}�[�A�C�R���̃E�F�C�g

#define	OBJ_PUT_BIT		( 1 )														// OBJ�\���`�F�b�N�r�b�g
#define	BMP_PUT_BIT		( 2 )														// BMP�\���`�F�b�N�r�b�g
#define	GRA_PUT_BIT		( OBJ_PUT_BIT | BMP_PUT_BIT )		// �O���t�B�b�N�\���`�F�b�N�r�b�g


// ���[�N
struct _REPORT_WORK {
	GAMESYS_WORK * gameSys;				// �Q�[���V�X�e��
	SAVE_CONTROL_WORK * sv;

	PRINT_QUE * que;							// �v�����g�L���[
	PRINT_UTIL	win[BMPWIN_MAX];	// BMPWIN
	GFL_FONT * font;

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[OBJID_MAX];
	u32	chrRes[CHRRES_MAX];
	u32	palRes[PALRES_MAX];
	u32	celRes[CELRES_MAX];

	// �Z�[�u��
	GFL_TCB * vtask;			// TCB ( VBLANK )
	BOOL	save_active;		// �Z�[�u����
	u32	actualSize;				// �Z�[�u����T�C�Y
	u32	totalSize;				// �Z�[�u�f�[�^�T�C�Y
	u32	overSize;					// �Z�[�u�����T�C�Y
	u32	timeSize;					// �J�E���^�T�C�Y
	u16	objCount;					// OBJ�ύX�J�E���^
	u16	objWait;					// OBJ�ύX�E�F�C�g

	int	seq;

	HEAPID heapID;
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void InitBg( HEAPID heapID );
static void ExitBg(void);
static void LoadBgGraphic( HEAPID heapID );
static void InitBmp( REPORT_WORK * wk );
static void ExitBmp( REPORT_WORK * wk );
static void InitObj( REPORT_WORK * wk );
static void ExitObj( REPORT_WORK * wk );

static void VBlankTask_SaveTimeMarkObj( GFL_TCB * tcb, void * work );


//============================================================================================
//	�O���[�o��
//============================================================================================

// BMPWIN DATA
static const u8 BmpWinData[][6] =
{
	{	// �^�C�g��
		BMPWIN_TITLE_FRM, BMPWIN_TITLE_PX, BMPWIN_TITLE_PY,
		BMPWIN_TITLE_SX, BMPWIN_TITLE_SY, BMPWIN_TITLE_PAL
	},
	{	// ���t
		BMPWIN_DATE_FRM, BMPWIN_DATE_PX, BMPWIN_DATE_PY,
		BMPWIN_DATE_SX, BMPWIN_DATE_SY, BMPWIN_DATE_PAL
	},
	{	// ����
		BMPWIN_DATE2_FRM, BMPWIN_DATE2_PX, BMPWIN_DATE2_PY,
		BMPWIN_DATE2_SX, BMPWIN_DATE2_SY, BMPWIN_DATE2_PAL
	},
	{	// �ꏊ
		BMPWIN_PLACE_FRM, BMPWIN_PLACE_PX, BMPWIN_PLACE_PY,
		BMPWIN_PLACE_SX, BMPWIN_PLACE_SY, BMPWIN_PLACE_PAL
	},
	{	// �W���o�b�W
		BMPWIN_BADGE_FRM, BMPWIN_BADGE_PX, BMPWIN_BADGE_PY,
		BMPWIN_BADGE_SX, BMPWIN_BADGE_SY, BMPWIN_BADGE_PAL
	},
	{	// �}��
		BMPWIN_ZUKAN_FRM, BMPWIN_ZUKAN_PX, BMPWIN_ZUKAN_PY,
		BMPWIN_ZUKAN_SX, BMPWIN_ZUKAN_SY, BMPWIN_ZUKAN_PAL
	},
	{	// �v���C����
		BMPWIN_TIME_FRM, BMPWIN_TIME_PX, BMPWIN_TIME_PY,
		BMPWIN_TIME_SX, BMPWIN_TIME_SY, BMPWIN_TIME_PAL
	},
	{	// �O��̃��|�[�g
		BMPWIN_REPORT_FRM, BMPWIN_REPORT_PX, BMPWIN_REPORT_PY,
		BMPWIN_REPORT_SX, BMPWIN_REPORT_SY, BMPWIN_REPORT_PAL
	}
};

// OBJ DATA
static const GFL_CLWK_DATA	PokeIconData = { POKEICON_PX, POKEICON_PY, POKEICON_ANM_DEATH, 0, 0 };
static const GFL_CLWK_DATA	TimeMarkData = { TIMEMARK_PX, TIMEMARK_PY, 0, 0, 0 };



//--------------------------------------------------------------------------------------------
/**
 * @brief		���|�[�g����ʏ�����
 *
 * @param		gs			GAMESYS_WORK
 * @param		heapID	�q�[�v�h�c
 *
 * @return	���|�[�g����ʃ��[�N
 */
//--------------------------------------------------------------------------------------------
REPORT_WORK * REPORT_Init( GAMESYS_WORK * gs, HEAPID heapID )
{
	REPORT_WORK * wk;

	wk = GFL_HEAP_AllocMemory( heapID, sizeof(REPORT_WORK) );

	wk->gameSys     = gs;
	wk->heapID      = heapID;
	wk->save_active = FALSE;
	wk->seq         = 0;

	InitBg( wk->heapID );
	LoadBgGraphic( wk->heapID );
	InitObj( wk );
	InitBmp( wk );

	GFL_NET_ReloadIconTopOrBottom( FALSE, heapID );

	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask_SaveTimeMarkObj, wk, 0 );

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���|�[�g����ʍ폜
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void REPORT_Exit( REPORT_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );

	ExitBmp( wk );
	ExitObj( wk );
	ExitBg();

	GFL_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���|�[�g����ʃA�b�v�f�[�g����
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void REPORT_Update( REPORT_WORK * wk )
{
	if( ( wk->seq & OBJ_PUT_BIT ) == 0 ){
		if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
			POKEPARTY * party;
			u32	i;
			party = GAMEDATA_GetMyPokemon( GAMESYSTEM_GetGameData(wk->gameSys) );
			for( i=OBJID_POKE1; i<OBJID_POKE1+PokeParty_GetPokeCount(party); i++ ){
				GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], TRUE );
			}
			wk->seq |= OBJ_PUT_BIT;
		}
	}

//	OS_Printf( "���|�[�g�F�A�b�v�f�[�g���܂���\n" );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���|�[�g����ʕ`�揈��
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void REPORT_Draw( REPORT_WORK * wk )
{
	u32	i;

	if( ( wk->seq & BMP_PUT_BIT ) == 0 ){
		if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
			GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
			GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
			GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );
			for( i=0; i<BMPWIN_MAX; i++ ){
				GFL_BMPWIN_MakeScreen( wk->win[i].win );
			}
			GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );
			wk->seq |= BMP_PUT_BIT;
		}
	}

	PRINTSYS_QUE_Main( wk->que );
	for( i=0; i<BMPWIN_MAX; i++ ){
		PRINT_UTIL_Trans( &wk->win[i], wk->que );
	}

//	OS_Printf( "���|�[�g�FDRAW���܂���\n" );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�������I���҂�
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @retval	"TRUE = �������I��"
 * @retval	"FALSE = ��������"
 */
//--------------------------------------------------------------------------------------------
BOOL REPORT_CheckInit( REPORT_WORK * wk )
{
	if( wk->seq == GRA_PUT_BIT ){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�T�C�Y�ݒ�
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void REPORT_SetSaveSize( REPORT_WORK * wk )
{
	wk->sv = GAMEDATA_GetSaveControlWork( GAMESYSTEM_GetGameData(wk->gameSys) );
	SaveControl_GetActualSize( wk->sv, &wk->actualSize, &wk->totalSize );
	wk->timeSize  = ((wk->actualSize * 2)<<8) / 10;
	wk->overSize  = wk->timeSize;
	wk->objCount  = 0;
	wk->objWait   = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�^�C�v�擾�i�������񏑂����j
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @retval	"TRUE = ��������"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL REPORT_CheckSaveType( REPORT_WORK * wk )
{
	if( wk->actualSize >= ( wk->totalSize / REPORT_SAVE_TYPE_VAL ) ){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�J�n
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void REPORT_StartSave( REPORT_WORK * wk )
{
	u32	i;

	GFL_BMPWIN_ClearTransWindow_VBlank( wk->win[BMPWIN_REPORT].win );

	for( i=OBJID_TIME01; i<=OBJID_TIME10; i++ ){
		GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], TRUE );
	}

	wk->save_active = TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�I��
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @retval	"TRUE = �I��"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL REPORT_EndSave( REPORT_WORK * wk )
{
	if( wk->objCount == 10 ){
		wk->save_active = FALSE;
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�����I��
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void REPORT_BreakSave( REPORT_WORK * wk )
{
	wk->save_active = FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f������
 *
 * @param		heapID		�q�[�v�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBg( HEAPID heapID )
{
	GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );

	{	// �w�i
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x00000, 0x2800,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_S );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S, 0x20, 0, heapID );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	}
	{	// �t�H���g��
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x04000, 0x4000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
			};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME1_S );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME1_S, 0x20, 0, heapID );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME1_S );
	}
	{	// ���b�Z�[�W��
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x04000, 0x4000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_S );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME2_S, 0x20, 0, heapID );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�폜
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBg(void)
{
	GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�O���t�B�b�N�ǂݍ���
 *
 * @param		heapID		�q�[�v�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void LoadBgGraphic( HEAPID heapID )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_REPORT_GRA, heapID );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_report_gra_base_bg_NCLR, PALTYPE_SUB_BG, 0, 0x20*3, heapID );
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0 );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_report_gra_base_bg_lz_NCGR, GFL_BG_FRAME0_S, 0, 0, TRUE, heapID );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_report_gra_base_bg_lz_NSCR, GFL_BG_FRAME0_S, 0, 0, TRUE, heapID );

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o������
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBmp( REPORT_WORK * wk )
{
	GFL_MSGDATA * mman;
	WORDSET * wset;
	STRBUF * exp;
	STRBUF * str;
	GAMEDATA * gd;
	SAVE_CONTROL_WORK * sv;
	u32	i;

	wk->que = PRINTSYS_QUE_Create( wk->heapID );
//  PRINTSYS_QUE_ForceCommMode( wk->que, TRUE );      // �e�X�g

	for( i=0; i<BMPWIN_MAX; i++ ){
		wk->win[i].win = GFL_BMPWIN_Create(
											BmpWinData[i][0],
											BmpWinData[i][1],
											BmpWinData[i][2],
											BmpWinData[i][3],
											BmpWinData[i][4],
											BmpWinData[i][5],
											GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[i].win), 0 );
	}

	wk->font = GFL_FONT_Create(
							ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	mman = GFL_MSG_Create(
					GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_report_dat, wk->heapID );
	wset = WORDSET_Create( wk->heapID );
	exp  = GFL_STR_CreateBuffer( 256, wk->heapID );

	gd = GAMESYSTEM_GetGameData( wk->gameSys );
	sv = GAMEDATA_GetSaveControlWork( gd );

	// �^�C�g��
	str = GFL_MSG_CreateString( mman, REPORT_STR_01 );
	WORDSET_RegisterPlayerName( wset, 0, GAMEDATA_GetMyStatus(gd) );
	WORDSET_ExpandStr( wset, exp, str );
	PRINT_UTIL_PrintColor( &wk->win[BMPWIN_TITLE], wk->que, 0, 4, exp, wk->font, FCOL_P02WN );
	GFL_STR_DeleteBuffer( str );

	// ���t
	str = GFL_MSG_CreateString( mman, REPORT_STR_02 );
	{
		RTCDate	date;
		RTC_GetDate( &date );
		WORDSET_RegisterNumber( wset, 0, date.year, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wset, 1, date.month, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wset, 2, date.day, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wset, exp, str );
	PRINT_UTIL_PrintColor( &wk->win[BMPWIN_DATE], wk->que, 0, 0, exp, wk->font, FCOL_P02BN );
	GFL_STR_DeleteBuffer( str );
	// ����
	str = GFL_MSG_CreateString( mman, REPORT_STR_03 );
	{
		RTCTime	time;
		RTC_GetTime( &time );
		WORDSET_RegisterNumber( wset, 0, time.hour, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wset, 1, time.minute, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wset, exp, str );
	PRINT_UTIL_PrintColor( &wk->win[BMPWIN_DATE2], wk->que, 0, 0, exp, wk->font, FCOL_P02BN );
	GFL_STR_DeleteBuffer( str );

	// �ꏊ
	str = GFL_MSG_CreateString( mman, REPORT_STR_04 );
	{
		u16	num = ZONEDATA_GetPlaceNameID( PLAYERWORK_getZoneID(GAMESYSTEM_GetMyPlayerWork(wk->gameSys)) );
		WORDSET_RegisterPlaceName( wset, 0, num );
	}
	WORDSET_ExpandStr( wset, exp, str );
	PRINT_UTIL_PrintColor( &wk->win[BMPWIN_PLACE], wk->que, 0, 0, exp, wk->font, FCOL_P02BN );
	GFL_STR_DeleteBuffer( str );

	// �W���o�b�W
	str = GFL_MSG_CreateString( mman, REPORT_STR_05 );
	{
		int	num = MISC_GetBadgeCount( GAMEDATA_GetMiscWork(gd) );
		WORDSET_RegisterNumber( wset, 0, num, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wset, exp, str );
	PRINT_UTIL_PrintColor( &wk->win[BMPWIN_BADGE], wk->que, 0, 0, exp, wk->font, FCOL_P02BN );
	GFL_STR_DeleteBuffer( str );

	// �}��
	{
		EVENTWORK * ev = GAMEDATA_GetEventWork( GAMESYSTEM_GetGameData(wk->gameSys) );
		if( EVENTWORK_CheckEventFlag( ev, SYS_FLAG_ZUKAN_GET ) == TRUE ){
			str = GFL_MSG_CreateString( mman, REPORT_STR_07 );
			{
				u16	num = ZUKANSAVE_GetZukanPokeSeeCount( GAMEDATA_GetZukanSave(gd), wk->heapID );
				WORDSET_RegisterNumber( wset, 0, num, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
			}
			WORDSET_ExpandStr( wset, exp, str );
			PRINT_UTIL_PrintColor( &wk->win[BMPWIN_ZUKAN], wk->que, 0, 0, exp, wk->font, FCOL_P02BN );
			GFL_STR_DeleteBuffer( str );
		}
	}

	// �v���C����
	str = GFL_MSG_CreateString( mman, REPORT_STR_09 );
	{
		PLAYTIME * ptime = GAMEDATA_GetPlayTimeWork( gd );
		WORDSET_RegisterNumber( wset, 0, PLAYTIME_GetHour(ptime), 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wset, 1, PLAYTIME_GetMinute(ptime), 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wset, exp, str );
	PRINT_UTIL_PrintColor( &wk->win[BMPWIN_TIME], wk->que, 0, 0, exp, wk->font, FCOL_P02BN );
	GFL_STR_DeleteBuffer( str );

	// �O��̃��|�[�g
	// �O��̃Z�[�u�f�[�^�������̂��̂ł���Ƃ��̂ݕ\��
	if( SaveControl_IsOverwritingOtherData(sv) == FALSE && SaveData_GetExistFlag(sv) == TRUE ){
		PLAYTIME * ptime = SaveData_GetPlayTime( sv );
		str = GFL_MSG_CreateString( mman, REPORT_STR_11 );
		WORDSET_RegisterNumber( wset, 0, PLAYTIME_GetSaveYear(ptime), 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wset, 1, PLAYTIME_GetSaveMonth(ptime), 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wset, 2, PLAYTIME_GetSaveDay(ptime ), 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wset, 3, PLAYTIME_GetSaveHour(ptime), 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wset, 4, PLAYTIME_GetSaveMinute(ptime), 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
		WORDSET_ExpandStr( wset, exp, str );
		PRINT_UTIL_PrintColor( &wk->win[BMPWIN_REPORT], wk->que, 0, 4, exp, wk->font, FCOL_P02WN );
		GFL_STR_DeleteBuffer( str );
	}

	GFL_STR_DeleteBuffer( exp );
	WORDSET_Delete( wset );
	GFL_MSG_Delete( mman );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o�폜
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBmp( REPORT_WORK * wk )
{
	u32	i;

	GFL_FONT_Delete( wk->font );

	for( i=0; i<BMPWIN_MAX; i++ ){
		GFL_BMPWIN_Delete( wk->win[i].win );
	}

	PRINTSYS_QUE_Delete( wk->que );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n�a�i������
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitObj( REPORT_WORK * wk )
{
  ARCHANDLE * ah;
	POKEPARTY * party;
	GFL_CLWK_DATA	dat;
	u32	max;
	u32	i;

	party = GAMEDATA_GetMyPokemon( GAMESYSTEM_GetGameData(wk->gameSys) );
	max   = PokeParty_GetPokeCount( party );

	wk->clunit = GFL_CLACT_UNIT_Create( OBJID_MAX, 0, wk->heapID );

	ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, wk->heapID );

	// �L����
	for( i=CHRRES_POKE1; i<CHRRES_POKE1+max; i++ ){
		POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, i );
		wk->chrRes[i] = GFL_CLGRP_CGR_Register(
											ah, POKEICON_GetCgxArcIndex(PP_GetPPPPointerConst(pp)),
											FALSE, CLSYS_DRAW_SUB, wk->heapID );
	}
	// �p���b�g
  wk->palRes[PALRES_POKE] = GFL_CLGRP_PLTT_RegisterComp(
															ah, POKEICON_GetPalArcIndex(), CLSYS_DRAW_SUB, 0, wk->heapID );
	// �Z���E�A�j��
  wk->celRes[CELRES_POKE] = GFL_CLGRP_CELLANIM_Register(
															ah, POKEICON_GetCellArcIndex(), POKEICON_GetAnmArcIndex(), wk->heapID );

  GFL_ARC_CloseDataHandle( ah );

	ah = GFL_ARC_OpenDataHandle( ARCID_REPORT_GRA, wk->heapID );
	// �L����
	wk->chrRes[CHRRES_TIME] = GFL_CLGRP_CGR_Register(
															ah, NARC_report_gra_time_obj_lz_NCGR,
															TRUE, CLSYS_DRAW_SUB, wk->heapID );
	// �p���b�g
  wk->palRes[PALRES_TIME] = GFL_CLGRP_PLTT_RegisterEx(
															ah, NARC_report_gra_time_obj_NCLR, CLSYS_DRAW_SUB, 0x20*3, 0, 1, wk->heapID );
	// �Z���E�A�j��
  wk->celRes[CELRES_TIME] = GFL_CLGRP_CELLANIM_Register(
															ah, NARC_report_gra_time_obj_NCER, NARC_report_gra_time_obj_NANR, wk->heapID );
  GFL_ARC_CloseDataHandle( ah );


	dat = PokeIconData;
	for( i=OBJID_POKE1; i<OBJID_POKE1+max; i++ ){
		POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, i );
		wk->clwk[i] = GFL_CLACT_WK_Create(
										wk->clunit,
										wk->chrRes[CHRRES_POKE1+i-OBJID_POKE1],
										wk->palRes[PALRES_POKE],
										wk->celRes[CELRES_POKE], 
										&dat, CLSYS_DRAW_SUB, wk->heapID );
		GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], FALSE );
		dat.pos_x += POKEICON_SX;

		GFL_CLACT_WK_SetPlttOffs(
			wk->clwk[i], POKEICON_GetPalNumGetByPPP(PP_GetPPPPointerConst(pp)), CLWK_PLTTOFFS_MODE_PLTT_TOP );
	}

	dat = TimeMarkData;
	for( i=OBJID_TIME01; i<=OBJID_TIME10; i++ ){
		wk->clwk[i] = GFL_CLACT_WK_Create(
										wk->clunit,
										wk->chrRes[CHRRES_TIME],
										wk->palRes[PALRES_TIME],
										wk->celRes[CELRES_TIME], 
										&dat, CLSYS_DRAW_SUB, wk->heapID );
		GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], FALSE );
		dat.pos_x += TIMEMARK_SX;
	}

	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n�a�i�폜
 *
 * @param		wk		���|�[�g����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitObj( REPORT_WORK * wk )
{
	POKEPARTY * party;
	u32	i;

	party = GAMEDATA_GetMyPokemon( GAMESYSTEM_GetGameData(wk->gameSys) );

	for( i=OBJID_POKE1; i<OBJID_POKE1+PokeParty_GetPokeCount(party); i++ ){
		GFL_CLACT_WK_Remove( wk->clwk[i] );
		GFL_CLGRP_CGR_Release( wk->chrRes[CHRRES_POKE1+i-OBJID_POKE1] );
	}
	for( i=OBJID_TIME01; i<=OBJID_TIME10; i++ ){
		GFL_CLACT_WK_Remove( wk->clwk[i] );
	}
	GFL_CLGRP_CGR_Release( wk->chrRes[CHRRES_TIME] );

	GFL_CLGRP_PLTT_Release( wk->palRes[PALRES_POKE] );
	GFL_CLGRP_PLTT_Release( wk->palRes[PALRES_TIME] );
	GFL_CLGRP_CELLANIM_Release( wk->celRes[CELRES_POKE] );
	GFL_CLGRP_CELLANIM_Release( wk->celRes[CELRES_TIME] );

	GFL_CLACT_UNIT_Delete( wk->clunit );

	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );		// SUB DISP OBJ ON
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK����
 *
 * @param		tcb		GFL_TCB
 * @param		work	���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask_SaveTimeMarkObj( GFL_TCB * tcb, void * work )
{
	REPORT_WORK * wk;
	u32	now_size;
	
	wk = work;

	if( wk->save_active == FALSE ){ return; }

	now_size = SaveControl_GetSaveAsyncMain_WritingSize( wk->sv ) << 8;

	if( wk->objWait != TIMEMARK_WAIT ){
		wk->objWait++;
	}

	if( now_size >= wk->overSize ){
		if( wk->objWait == TIMEMARK_WAIT ){
			if( wk->objCount < 10 ){
				wk->overSize += wk->timeSize;
				wk->objWait = 0;
				GFL_CLACT_WK_SetAnmFrame( wk->clwk[OBJID_TIME01+wk->objCount], 0 );
				GFL_CLACT_WK_SetAnmSeq( wk->clwk[OBJID_TIME01+wk->objCount], 1 );
				GFL_CLACT_WK_SetAutoAnmFlag( wk->clwk[OBJID_TIME01+wk->objCount], TRUE );
				wk->objCount++;
				OS_Printf( "objCount = %d\n", wk->objCount );
			}
		}
	}

	GFL_CLACT_SYS_Main();
	GFL_CLACT_SYS_VBlankFunc();
}
