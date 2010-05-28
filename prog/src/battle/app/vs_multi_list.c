//============================================================================================
/**
 * @file		vs_multi_list.c
 * @brief		�}���`�o�g���J�n�O�|�P�������X�g
 * @author	Hiroyuki Nakamura
 * @date		10.01.15
 *
 *	���W���[�����FVSMULTILIST
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "pm_define.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "system/bgwinfrm.h"
#include "sound/pm_sndsys.h"
#include "pokeicon/pokeicon.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "font/font.naix"
#include "app/app_menu_common.h"
#include "msg/msg_b_plist.h"

#include "app_menu_common.naix"
#include "message.naix"

#include "vs_multi_list.h"
#include "b_plist_comm_def.h"
#include "b_plist_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================

// [ HEAPID_VS_MULTI_LIST ] ����m�ۗp��`
#define	HEAPID_VS_MULTI_LIST_L		( GFL_HEAP_LOWID(HEAPID_VS_MULTI_LIST) )

// BGWINFRM
enum {
	BGWF_POKE1_PLATE = 0,
	BGWF_POKE2_PLATE,
	BGWF_POKE3_PLATE,
	BGWF_POKE4_PLATE,
	BGWF_POKE5_PLATE,
	BGWF_POKE6_PLATE,

	BGWF_POKE1_PARAM,
	BGWF_POKE2_PARAM,
	BGWF_POKE3_PARAM,
	BGWF_POKE4_PARAM,
	BGWF_POKE5_PARAM,
	BGWF_POKE6_PARAM,

	BGWF_MAX,
};

// OBJ INDEX
enum {
	OBJ_POKEICON1 = 0,
	OBJ_POKEICON2,
	OBJ_POKEICON3,
	OBJ_POKEICON4,
	OBJ_POKEICON5,
	OBJ_POKEICON6,

	OBJ_ITEMICON1,
	OBJ_ITEMICON2,
	OBJ_ITEMICON3,
	OBJ_ITEMICON4,
	OBJ_ITEMICON5,
	OBJ_ITEMICON6,

	OBJ_STATUSICON1,
	OBJ_STATUSICON2,
	OBJ_STATUSICON3,
	OBJ_STATUSICON4,
	OBJ_STATUSICON5,
	OBJ_STATUSICON6,

	OBJ_HP_GAUGE1,
	OBJ_HP_GAUGE2,
	OBJ_HP_GAUGE3,
	OBJ_HP_GAUGE4,
	OBJ_HP_GAUGE5,
	OBJ_HP_GAUGE6,

	OBJ_MAX,
};

// OBJ RESOURCE
enum {
	// CHAR
	CHRRES_POKEICON1 = 0,
	CHRRES_POKEICON2,
	CHRRES_POKEICON3,
	CHRRES_POKEICON4,
	CHRRES_POKEICON5,
	CHRRES_POKEICON6,
	CHRRES_ITEMICON,
	CHRRES_STATUSICON,
	CHRRES_HP_GAUGE,
	CHRRES_MAX,

	// PALETTE
	PALRES_POKEICON = 0,
	PALRES_ITEMICON,
	PALRES_STATUSICON,
	PALRES_HP_GAUGE,
	PALRES_MAX,

	// CELL
	CELRES_POKEICON = 0,
	CELRES_ITEMICON,
	CELRES_STATUSICON,
	CELRES_HP_GAUGE,
	CELRES_MAX,
};

// BMPWIN INDEX
enum {
	BMPWIN_POKE1 = 0,
	BMPWIN_POKE2,
	BMPWIN_POKE3,
	BMPWIN_POKE4,
	BMPWIN_POKE5,
	BMPWIN_POKE6,

	BMPWIN_MAX
};

// ���[�N
typedef struct {
	VS_MULTI_LIST_PARAM * dat;

	GFL_TCB * vtask;				// TCB ( VBLANK )

	BGWINFRM_WORK * wfrm;		// �a�f�E�B���h�E�t���[��

	GFL_FONT * font;					// �ʏ�t�H���g
	GFL_FONT * nfnt;					// 8x8�t�H���g
	GFL_MSGDATA * mman;				// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;						// �P��Z�b�g
	PRINT_QUE * que;					// �v�����g�L���[

	PRINT_UTIL	win[BMPWIN_MAX];		// BMPWIN

	// �Z���A�N�^�[
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[OBJ_MAX];
	u32	chrRes[CHRRES_MAX];
	u32	palRes[PALRES_MAX];
	u32	celRes[CELRES_MAX];

	POKEMON_PARAM * pp[TEMOTI_POKEMAX];

	int	sub_seq;

	int	next_seq;
	int	fade_next;

	int	wait;

}VSMLIST_WORK;

typedef int (*pVSMLIST_FUNC)(VSMLIST_WORK*);

// ���C���V�[�P���X��`
enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_RELEASE,
	MAINSEQ_FADE,
	MAINSEQ_WAIT,
	MAINSEQ_MAIN,

	MAINSEQ_END,
};


#define	START_WAIT		( 128 )		// �J�n���̃t�F�[�h��̃E�F�C�g
#define	END_WAIT			( 128 )		// �I�����̃t�F�[�h�O�̃E�F�C�g

#define	NORMAL_PLATE_PX		( 0 )		// �ʏ�̃v���[�g�̎擾�w���W
#define	NORMAL_PLATE_PY		( 0 )		// �ʏ�̃v���[�g�̎擾�x���W
#define	NONE_PLATE_PX			( 0 )		// �󔒂̃v���[�g�̎擾�w���W
#define	NONE_PLATE_PY			( 18 )	// �󔒂̃v���[�g�̎擾�x���W

// �n�a�i�p���b�g�ʒu���T�C�Y
#define PALOFS_POKEICON     ( 0 )
#define PALSIZ_POKEICON     ( 3 )
#define PALOFS_STATUSICON   ( PALOFS_POKEICON + 0x20 * PALSIZ_POKEICON )
#define PALSIZ_STATUSICON   ( 1 )
#define PALOFS_ITEMICON     ( PALOFS_STATUSICON + 0x20 * PALSIZ_STATUSICON )
#define PALSIZ_ITEMICON     ( 1 )
#define PALOFS_HPGAUGE      ( PALOFS_ITEMICON + 0x20 * PALSIZ_ITEMICON )
#define PALSIZ_HPGAUGE      ( 1 )

// �n�a�i�������f�[�^
typedef struct {
	GFL_CLWK_DATA	dat;
	u32	chrRes;
	u32	palRes;
	u32	celRes;
}OBJ_ADD_PARAM;

#define FCOL_P09WN    ( PRINTSYS_LSB_Make(15,14,0) )    // �t�H���g�J���[�F�p���b�g�X����
#define FCOL_P09BLN   ( PRINTSYS_LSB_Make(10,11,0) )    // �t�H���g�J���[�F�p���b�g�X��
#define FCOL_P09RN    ( PRINTSYS_LSB_Make(12,13,0) )    // �t�H���g�J���[�F�p���b�g�X�Ԕ�


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT VSMListProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT VSMListProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT VSMListProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static int MainSeq_Init( VSMLIST_WORK * wk );
static int MainSeq_Release( VSMLIST_WORK * wk );
static int MainSeq_Fade( VSMLIST_WORK * wk );
static int MainSeq_Wait( VSMLIST_WORK * wk );
static int MainSeq_Main( VSMLIST_WORK * wk );

static int SetFadeIn( VSMLIST_WORK * wk, int next );
static int SetFadeOut( VSMLIST_WORK * wk, int next );
static int SetWait( VSMLIST_WORK * wk, int wait, int next );

static void SetVramBank(void);
static void InitBg(void);
static void ExitBg(void);
static void LoadBgGraphic( VSMLIST_WORK * wk );
static void InitMsg( VSMLIST_WORK * wk );
static void ExitMsg( VSMLIST_WORK * wk );

static void InitVBlank( VSMLIST_WORK * wk );
static void ExitVBlank( VSMLIST_WORK * wk );
static void VBlankTask( GFL_TCB * tcb, void * work );

static void GetPokemonParam( VSMLIST_WORK * wk );

static void InitBmp( VSMLIST_WORK * wk );
static void ExitBmp( VSMLIST_WORK * wk );
static void PrintUtilTrans( VSMLIST_WORK * wk );
static void PutPokeName( VSMLIST_WORK * wk, u32 idx );
static void PutPokeLv( VSMLIST_WORK * wk, u32 idx );
static void PutPokeHP( VSMLIST_WORK * wk, u32 idx );
static void PutPokeHPGage( VSMLIST_WORK * wk, u32 idx );

static void InitObj( VSMLIST_WORK * wk );
static void ExitObj( VSMLIST_WORK * wk );
static void CreateClactSys(void);
static void DeleteClactSys(void);
static void CreateClactResource( VSMLIST_WORK * wk );
static void DeleteClactResource( VSMLIST_WORK * wk );
static void AddObj( VSMLIST_WORK * wk );
static void DelObj( VSMLIST_WORK * wk );
static void MoveObjOne( VSMLIST_WORK * wk, u32 idx, s16 mv );
static void MoveObj( VSMLIST_WORK * wk, u8 start, s16 mv );

static void InitBgWinFrame( VSMLIST_WORK * wk );
static void ExitBgWinFrame( VSMLIST_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// PROC�f�[�^
const GFL_PROC_DATA VS_MULTI_LIST_ProcData = {
	VSMListProc_Init,
	VSMListProc_Main,
	VSMListProc_End
};

// ���C���V�[�P���X�֐��e�[�u��
static const pVSMLIST_FUNC MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Fade,
	MainSeq_Wait,
	MainSeq_Main,
};

// VRAM����U��
static const GFL_DISP_VRAM VramTbl = {
	GX_VRAM_BG_128_A,							// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,				// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_128_C,					// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_128_B,						// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_128_D,					// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,		// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_NONE,							// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE,					// �e�N�X�`���p���b�g�X���b�g

	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_128K		// �T�uOBJ�}�b�s���O���[�h
};

// �n�a�i�����f�[�^
static const OBJ_ADD_PARAM ObjAddParam[] =
{
	// �|�P�����A�C�R��
	{
		{ BPL_COMM_POKEICON_LPX, BPL_COMM_POKEICON1_PY, POKEICON_ANM_HPMAX, 1, 1 },
		CHRRES_POKEICON1, PALRES_POKEICON, CELRES_POKEICON
	},
	{
		{ BPL_COMM_POKEICON_LPX, BPL_COMM_POKEICON3_PY, POKEICON_ANM_HPMAX, 1, 1 },
		CHRRES_POKEICON2, PALRES_POKEICON, CELRES_POKEICON
	},
	{
		{ BPL_COMM_POKEICON_LPX, BPL_COMM_POKEICON5_PY, POKEICON_ANM_HPMAX, 1, 1 },
		CHRRES_POKEICON3, PALRES_POKEICON, CELRES_POKEICON
	},
	{
		{ BPL_COMM_POKEICON_RPX, BPL_COMM_POKEICON2_PY, POKEICON_ANM_HPMAX, 1, 1 },
		CHRRES_POKEICON4, PALRES_POKEICON, CELRES_POKEICON
	},
	{
		{ BPL_COMM_POKEICON_RPX, BPL_COMM_POKEICON4_PY, POKEICON_ANM_HPMAX, 1, 1 },
		CHRRES_POKEICON5, PALRES_POKEICON, CELRES_POKEICON
	},
	{
		{ BPL_COMM_POKEICON_RPX, BPL_COMM_POKEICON6_PY, POKEICON_ANM_HPMAX, 1, 1 },
		CHRRES_POKEICON6, PALRES_POKEICON, CELRES_POKEICON
	},
	// �A�C�e���A�C�R��
	{
		{ BPL_COMM_POKEICON_LPX+BPL_COMM_ITEMICON_X, BPL_COMM_POKEICON1_PY+BPL_COMM_ITEMICON_Y, 0, 0, 1 },
		CHRRES_ITEMICON, PALRES_ITEMICON, CELRES_ITEMICON
	},
	{
		{ BPL_COMM_POKEICON_LPX+BPL_COMM_ITEMICON_X, BPL_COMM_POKEICON3_PY+BPL_COMM_ITEMICON_Y, 0, 0, 1 },
		CHRRES_ITEMICON, PALRES_ITEMICON, CELRES_ITEMICON
	},
	{
		{ BPL_COMM_POKEICON_LPX+BPL_COMM_ITEMICON_X, BPL_COMM_POKEICON5_PY+BPL_COMM_ITEMICON_Y, 0, 0, 1 },
		CHRRES_ITEMICON, PALRES_ITEMICON, CELRES_ITEMICON
	},
	{
		{ BPL_COMM_POKEICON_RPX+BPL_COMM_ITEMICON_X, BPL_COMM_POKEICON2_PY+BPL_COMM_ITEMICON_Y, 0, 0, 1 },
		CHRRES_ITEMICON, PALRES_ITEMICON, CELRES_ITEMICON
	},
	{
		{ BPL_COMM_POKEICON_RPX+BPL_COMM_ITEMICON_X, BPL_COMM_POKEICON4_PY+BPL_COMM_ITEMICON_Y, 0, 0, 1 },
		CHRRES_ITEMICON, PALRES_ITEMICON, CELRES_ITEMICON
	},
	{
		{ BPL_COMM_POKEICON_RPX+BPL_COMM_ITEMICON_X, BPL_COMM_POKEICON6_PY+BPL_COMM_ITEMICON_Y, 0, 0, 1 },
		CHRRES_ITEMICON, PALRES_ITEMICON, CELRES_ITEMICON
	},
	// ��Ԉُ�A�C�R��
	{
		{ BPL_COMM_POKEICON_LPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON1_PY+BPL_COMM_STATUSICON_Y, 0, 2, 1 },
		CHRRES_STATUSICON, PALRES_STATUSICON, CELRES_STATUSICON
	},
	{
		{ BPL_COMM_POKEICON_LPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON3_PY+BPL_COMM_STATUSICON_Y, 0, 2, 1 },
		CHRRES_STATUSICON, PALRES_STATUSICON, CELRES_STATUSICON
	},
	{
		{ BPL_COMM_POKEICON_LPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON5_PY+BPL_COMM_STATUSICON_Y, 0, 2, 1 },
		CHRRES_STATUSICON, PALRES_STATUSICON, CELRES_STATUSICON
	},
	{
		{ BPL_COMM_POKEICON_RPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON2_PY+BPL_COMM_STATUSICON_Y, 0, 2, 1 },
		CHRRES_STATUSICON, PALRES_STATUSICON, CELRES_STATUSICON
	},
	{
		{ BPL_COMM_POKEICON_RPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON4_PY+BPL_COMM_STATUSICON_Y, 0, 2, 1 },
		CHRRES_STATUSICON, PALRES_STATUSICON, CELRES_STATUSICON
	},
	{
		{ BPL_COMM_POKEICON_RPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON6_PY+BPL_COMM_STATUSICON_Y, 0, 2, 1 },
		CHRRES_STATUSICON, PALRES_STATUSICON, CELRES_STATUSICON
	},
	// �g�o�Q�[�W
	{
		{ BPL_COMM_POKEICON_LPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON1_PY+BPL_COMM_HPGAUGE_Y, 0, 2, 1 },
		CHRRES_HP_GAUGE, PALRES_HP_GAUGE, CELRES_HP_GAUGE
	},
	{
		{ BPL_COMM_POKEICON_LPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON3_PY+BPL_COMM_HPGAUGE_Y, 0, 2, 1 },
		CHRRES_HP_GAUGE, PALRES_HP_GAUGE, CELRES_HP_GAUGE
	},
	{
		{ BPL_COMM_POKEICON_LPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON5_PY+BPL_COMM_HPGAUGE_Y, 0, 2, 1 },
		CHRRES_HP_GAUGE, PALRES_HP_GAUGE, CELRES_HP_GAUGE
	},
	{
		{ BPL_COMM_POKEICON_RPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON2_PY+BPL_COMM_HPGAUGE_Y, 0, 2, 1 },
		CHRRES_HP_GAUGE, PALRES_HP_GAUGE, CELRES_HP_GAUGE
	},
	{
		{ BPL_COMM_POKEICON_RPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON4_PY+BPL_COMM_HPGAUGE_Y, 0, 2, 1 },
		CHRRES_HP_GAUGE, PALRES_HP_GAUGE, CELRES_HP_GAUGE
	},
	{
		{ BPL_COMM_POKEICON_RPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON6_PY+BPL_COMM_HPGAUGE_Y, 0, 2, 1 },
		CHRRES_HP_GAUGE, PALRES_HP_GAUGE, CELRES_HP_GAUGE
	},
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F������
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT VSMListProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	VSMLIST_WORK * wk;

//	OS_Printf( "�����������@�}���`�o�g���J�n�O�|�P�������X�g�����J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_VS_MULTI_LIST, 0x18000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(VSMLIST_WORK), HEAPID_VS_MULTI_LIST );
	GFL_STD_MemClear( wk, sizeof(VSMLIST_WORK) );

	wk->dat = pwk;

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F���C��
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT VSMListProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	VSMLIST_WORK * wk = mywk;

	*seq = MainSeq[*seq]( wk );
	if( *seq == MAINSEQ_END ){
		return GFL_PROC_RES_FINISH;
	}else{
		BGWINFRM_MoveMain( wk->wfrm );
		PrintUtilTrans( wk );
		GFL_CLACT_SYS_Main();
	}
	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F�I��
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT VSMListProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_VS_MULTI_LIST );

//	OS_Printf( "�����������@�}���`�o�g���J�n�O�|�P�������X�g�����I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}


//============================================================================================
//	���C���V�[�P���X
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F������
 *
 * @param		wk		���[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Init( VSMLIST_WORK * wk )
{
	// ���荞�ݒ�~
//	sys_VBlankFuncChange( NULL, NULL );
//	sys_HBlankIntrStop();
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// ���ʂ����C����
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );

	GetPokemonParam( wk );

	SetVramBank();

	InitBg();
	LoadBgGraphic( wk );

	InitMsg( wk );
	InitBmp( wk );
	InitObj( wk );
	InitBgWinFrame( wk );

	// �ʐM�A�C�R���ݒ�
	GFL_NET_WirelessIconEasy_HoldLCD( TRUE, HEAPID_VS_MULTI_LIST );
	GFL_NET_ReloadIcon();

	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_OBJ, VISIBLE_ON );

	InitVBlank( wk );

	return SetFadeIn( wk, SetWait(wk,START_WAIT,MAINSEQ_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���
 *
 * @param		wk		���[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Release( VSMLIST_WORK * wk )
{
	ExitVBlank( wk );

	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_OBJ, VISIBLE_OFF );

	ExitBgWinFrame( wk );
	ExitObj( wk );
	ExitBmp( wk );
	ExitMsg( wk );

	ExitBg();

	return MAINSEQ_END;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�t�F�[�h�I���҂�
 *
 * @param		wk		���[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Fade( VSMLIST_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return wk->fade_next;
	}
	return MAINSEQ_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�E�F�C�g
 *
 * @param		wk		���[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Wait( VSMLIST_WORK * wk )
{
	if( wk->wait == 0 ){
		return wk->next_seq;
	}
	wk->wait--;
	return MAINSEQ_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���상�C��
 *
 * @param		wk		���[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Main( VSMLIST_WORK * wk )
{
	u32	i;

	switch( wk->sub_seq ){
	case 0:
	  if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
			break;
		}

		if( wk->dat->pos == VS_MULTI_LIST_POS_LEFT ){
			for( i=0; i<3; i++ ){
				BGWINFRM_MoveInit( wk->wfrm, BGWF_POKE4_PLATE+i, -1, 0, BPL_COMM_BSX_PLATE );
				BGWINFRM_MoveInit( wk->wfrm, BGWF_POKE4_PARAM+i, -1, 0, BPL_COMM_BSX_PLATE );
			}
		}else{
			for( i=0; i<3; i++ ){
				BGWINFRM_MoveInit( wk->wfrm, BGWF_POKE1_PLATE+i, 1, 0, BPL_COMM_BSX_PLATE );
				BGWINFRM_MoveInit( wk->wfrm, BGWF_POKE1_PARAM+i, 1, 0, BPL_COMM_BSX_PLATE );
			}
		}
		PMSND_PlaySE( SEQ_SE_SYS_03 );
		wk->sub_seq++;

	case 1:
		if( wk->dat->pos == VS_MULTI_LIST_POS_LEFT ){
			if( BGWINFRM_MoveCheck( wk->wfrm, BGWF_POKE4_PLATE ) == 0 ){
				wk->sub_seq++;
				PMSND_PlaySE( SEQ_SE_ROTATION_B );
				return SetWait( wk, END_WAIT, MAINSEQ_MAIN );
			}
			MoveObj( wk, 3, -8 );
		}else{
			if( BGWINFRM_MoveCheck( wk->wfrm, BGWF_POKE1_PLATE ) == 0 ){
				wk->sub_seq++;
				PMSND_PlaySE( SEQ_SE_ROTATION_B );
				return SetWait( wk, END_WAIT, MAINSEQ_MAIN );
			}
			MoveObj( wk, 0, 8 );
		}
		break;

	case 2:
		return SetFadeOut( wk, MAINSEQ_RELEASE );
	}

	return MAINSEQ_MAIN;
}


//============================================================================================
//	���C���V�[�P���X�֘A�ݒ菈��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�C���Z�b�g
 *
 * @param		wk		���[�N
 * @param		next	�t�F�[�h��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetFadeIn( VSMLIST_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_VS_MULTI_LIST );

	wk->fade_next = next;
	return MAINSEQ_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�A�E�g�Z�b�g
 *
 * @param		wk		���[�N
 * @param		next	�t�F�[�h��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetFadeOut( VSMLIST_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_VS_MULTI_LIST );

	wk->fade_next = next;
	return MAINSEQ_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�E�F�C�g�Z�b�g
 *
 * @param		wk		���[�N
 * @param		wait	�E�F�C�g
 * @param		next	�E�F�C�g��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetWait( VSMLIST_WORK * wk, int wait, int next )
{
	wk->wait = wait;
	wk->next_seq = next;
	return MAINSEQ_WAIT;
}


//============================================================================================
//	��ʊ֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�q�`�l�ݒ�
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetVramBank(void)
{
	GFL_DISP_SetBank( &VramTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBg(void)
{
	GFL_BG_Init( HEAPID_VS_MULTI_LIST );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// ���C����ʁF����
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0, HEAPID_VS_MULTI_LIST );
	}
	{	// ���C����ʁF�v���[�g��
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f���
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBg(void)
{
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

	GFL_BG_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�O���t�B�b�N�ǂݍ���
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void LoadBgGraphic( VSMLIST_WORK * wk )
{
  ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_B_PLIST_GRA, HEAPID_VS_MULTI_LIST_L );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    ah, NARC_b_plist_gra_b_plist_lz_NCGR, GFL_BG_FRAME2_M, 0, 0, TRUE, HEAPID_VS_MULTI_LIST_L );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_b_plist_gra_poke_sel_u_lz_NSCR, GFL_BG_FRAME2_M, 0, 0, TRUE, HEAPID_VS_MULTI_LIST_L );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_b_plist_gra_b_plist_NCLR, PALTYPE_MAIN_BG, 0, 0, HEAPID_VS_MULTI_LIST_L );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_b_plist_gra_b_plist_anm_lz_NSCR, GFL_BG_FRAME1_M, 0, 32*32*2, TRUE, HEAPID_VS_MULTI_LIST_L );

  GFL_ARC_CloseDataHandle( ah );

	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�֘A������
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitMsg( VSMLIST_WORK * wk )
{
	wk->mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_b_plist_dat, HEAPID_VS_MULTI_LIST );
	wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_VS_MULTI_LIST );
  wk->nfnt = GFL_FONT_Create( ARCID_FONT, NARC_font_num_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_VS_MULTI_LIST );
  wk->wset = WORDSET_Create( HEAPID_VS_MULTI_LIST );
  wk->que  = PRINTSYS_QUE_CreateEx( 2048, HEAPID_VS_MULTI_LIST );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�֘A���
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitMsg( VSMLIST_WORK * wk )
{
  GFL_MSG_Delete( wk->mman );
  GFL_FONT_Delete( wk->font );
  GFL_FONT_Delete( wk->nfnt );
  WORDSET_Delete( wk->wset );
  PRINTSYS_QUE_Delete( wk->que );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitVBlank( VSMLIST_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitVBlank( VSMLIST_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK����
 *
 * @param		tcb			GFL_TCB
 * @param		work		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask( GFL_TCB * tcb, void * work )
{
	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		POKEMON_PARAM�擾
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void GetPokemonParam( VSMLIST_WORK * wk )
{
	const POKEPARTY * ppL;
	const POKEPARTY * ppR;
	u32	i;

	if( wk->dat->pos == VS_MULTI_LIST_POS_LEFT ){
		ppL = wk->dat->myPP;
		ppR = wk->dat->ptPP;
	}else{
		ppL = wk->dat->ptPP;
		ppR = wk->dat->myPP;
	}

	for( i=0; i<3; i++ ){
		if( PokeParty_GetPokeCount( ppL ) > i ){
			wk->pp[i] = PokeParty_GetMemberPointer( ppL, i );
		}else{
			wk->pp[i] = NULL;
		}
		if( PokeParty_GetPokeCount( ppR ) > i ){
			wk->pp[3+i] = PokeParty_GetMemberPointer( ppR, i );
		}else{
			wk->pp[3+i] = NULL;
		}
	}
}


//============================================================================================
//	�a�l�o�֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o������
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitBmp( VSMLIST_WORK * wk )
{
	u32	i;

	GFL_BMPWIN_Init( HEAPID_VS_MULTI_LIST );

	for( i=0; i<BMPWIN_MAX; i++ ){
		wk->win[i].win = GFL_BMPWIN_Create(
											GFL_BG_FRAME0_M,
											BPL_COMM_WIN_P1_POKE1_PX, BPL_COMM_WIN_P1_POKE1_PY,
											BPL_COMM_WIN_P1_POKE1_SX, BPL_COMM_WIN_P1_POKE1_SY,
											BPL_COMM_WIN_P1_POKE1_PAL, GFL_BMP_CHRAREA_GET_B );
	}

	for( i=0; i<TEMOTI_POKEMAX; i++ ){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[i].win), 0 );
		if( wk->pp[i] != NULL ){
			PutPokeName( wk, i );
			PutPokeLv( wk, i );
			PutPokeHP( wk, i );
			PutPokeHPGage( wk, i );
		}else{
			GFL_BMPWIN_TransVramCharacter( wk->win[i].win );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o���
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitBmp( VSMLIST_WORK * wk )
{
	u32	i;

	for( i=0; i<BMPWIN_MAX; i++ ){
		GFL_BMPWIN_Delete( wk->win[i].win );
	}
	GFL_BMPWIN_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v�����g
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PrintUtilTrans( VSMLIST_WORK * wk )
{
	u32	i;

	PRINTSYS_QUE_Main( wk->que );
	for( i=0; i<BMPWIN_MAX; i++ ){
		PRINT_UTIL_Trans( &wk->win[i], wk->que );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�������\��
 *
 * @param		wk    ���[�N
 * @param		idx   �C���f�b�N�X
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PutPokeName( VSMLIST_WORK * wk, u32 idx )
{
  GFL_BMPWIN * win;
  STRBUF * exp;
  STRBUF * str;

  win = wk->win[idx].win;
  exp = GFL_STR_CreateBuffer( BUFLEN_POKEMON_NAME, HEAPID_VS_MULTI_LIST_L );
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_001 );

  WORDSET_RegisterPokeNickName( wk->wset, 0, wk->pp[idx] );
  WORDSET_ExpandStr( wk->wset, exp, str );

	PRINT_UTIL_PrintColor(
		&wk->win[idx], wk->que, BPL_COMM_P1_NAME_PX, BPL_COMM_P1_NAME_PY, exp, wk->font, FCOL_P09WN );

  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

  // ����
  if( PP_Get( wk->pp[idx], ID_PARA_nidoran_nickname, NULL ) == TRUE &&
			PP_Get( wk->pp[idx], ID_PARA_tamago_flag, NULL ) == 0 ){
		u32	sex = PP_GetSex( wk->pp[idx] );
    if( sex == PTL_SEX_MALE ){
      str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_502 );
      PRINT_UTIL_PrintColor(
				&wk->win[idx], wk->que,
				GFL_BMPWIN_GetSizeX(win)*8-PRINTSYS_GetStrWidth(str,wk->font,0),
				BPL_COMM_P1_NAME_PY,
				str, wk->font, FCOL_P09BLN );
      GFL_STR_DeleteBuffer( str );
    }else if( sex == PTL_SEX_FEMALE ){
      str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_503 );
      PRINT_UTIL_PrintColor(
				&wk->win[idx], wk->que,
				GFL_BMPWIN_GetSizeX(win)*8-PRINTSYS_GetStrWidth(str,wk->font,0),
				BPL_COMM_P1_NAME_PY,
				str, wk->font, FCOL_P09RN );
      GFL_STR_DeleteBuffer( str );
    }
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���x���\��
 *
 * @param		wk    ���[�N
 * @param		idx   �C���f�b�N�X
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PutPokeLv( VSMLIST_WORK * wk, u32 idx )
{
  STRBUF * exp;
  STRBUF * str;

  exp = GFL_STR_CreateBuffer( 32, HEAPID_VS_MULTI_LIST_L );
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_701 );

  WORDSET_RegisterNumber(
    wk->wset, 0, PP_Get(wk->pp[idx],ID_PARA_level,NULL), 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_ExpandStr( wk->wset, exp, str );
  PRINT_UTIL_PrintColor(
		&wk->win[idx], wk->que, BPL_COMM_P1_LV_PX, BPL_COMM_P1_LV_PY, exp, wk->nfnt, FCOL_P09WN );

  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g�o�\��
 *
 * @param		wk    ���[�N
 * @param		idx   �C���f�b�N�X
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PutPokeHP( VSMLIST_WORK * wk, u32 idx )
{
  STRBUF * exp;
  STRBUF * str;
	u8	px;
  u8  sx;

  exp = GFL_STR_CreateBuffer( 32, HEAPID_VS_MULTI_LIST_L );

  //�u�^�v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_702 );
  sx  = PRINTSYS_GetStrWidth( str, wk->nfnt, 0 );
  px  = BPL_COMM_P1_HP_PX - ( sx / 2 );
  PRINT_UTIL_PrintColor( &wk->win[idx], wk->que, px, BPL_COMM_P1_HP_PY, str, wk->nfnt, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );
  // HP
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_703 );
  WORDSET_RegisterNumber(
    wk->wset, 0, PP_Get(wk->pp[idx],ID_PARA_hp,NULL), 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_ExpandStr( wk->wset, exp, str );
  PRINT_UTIL_PrintColor(
    &wk->win[idx], wk->que,
    px-PRINTSYS_GetStrWidth(exp,wk->nfnt,0), BPL_COMM_P1_HP_PY, exp, wk->nfnt, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );
  // MHP
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_703 );
  WORDSET_RegisterNumber(
    wk->wset, 0, PP_Get(wk->pp[idx],ID_PARA_hpmax,NULL), 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_ExpandStr( wk->wset, exp, str );
  PRINT_UTIL_PrintColor(
    &wk->win[idx], wk->que, px+sx, BPL_COMM_P1_HP_PY, exp, wk->nfnt, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );

  GFL_STR_DeleteBuffer( exp );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g�o�Q�[�W�\��
 *
 * @param		wk    ���[�N
 * @param		idx   �C���f�b�N�X
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PutPokeHPGage( VSMLIST_WORK * wk, u32 idx )
{
  GFL_BMP_Fill(
		GFL_BMPWIN_GetBmp(wk->win[idx].win),
		BPL_COMM_P1_HPGAGE_PX, BPL_COMM_P1_HPGAGE_PY+3,
		BPL_COMM_HP_DOTTO_MAX, 1, BPL_COMM_HP_GAGE_COL_G1 );
  GFL_BMP_Fill(
		GFL_BMPWIN_GetBmp(wk->win[idx].win),
		BPL_COMM_P1_HPGAGE_PX, BPL_COMM_P1_HPGAGE_PY+4,
		BPL_COMM_HP_DOTTO_MAX, 1, BPL_COMM_HP_GAGE_COL_G2 );
}


//============================================================================================
//	�n�a�i�֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n�a�i������
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void InitObj( VSMLIST_WORK * wk )
{
	CreateClactSys();
	CreateClactResource( wk );
	AddObj( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n�a�i���
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ExitObj( VSMLIST_WORK * wk )
{
	DelObj( wk );
	DeleteClactResource( wk );
	DeleteClactSys();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�V�X�e���쐬
 *
 * @param		none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CreateClactSys(void)
{
	const GFL_CLSYS_INIT init = {
		0, 0,					// ���C���@�T�[�t�F�[�X�̍�����W
		0, 512,				// �T�u�@�T�[�t�F�[�X�̍�����W
		4,						// ���C�����OAM�Ǘ��J�n�ʒu	4�̔{��
		124,					// ���C�����OAM�Ǘ���				4�̔{��
		4,						// �T�u���OAM�Ǘ��J�n�ʒu		4�̔{��
		124,					// �T�u���OAM�Ǘ���					4�̔{��
		0,						// �Z��Vram�]���Ǘ���

		CHRRES_MAX,		// �o�^�ł���L�����f�[�^��
		PALRES_MAX,		// �o�^�ł���p���b�g�f�[�^��
		CELRES_MAX,		// �o�^�ł���Z���A�j���p�^�[����
		0,						// �o�^�ł���}���`�Z���A�j���p�^�[�����i�����󖢑Ή��j

		16,						// ���C�� CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
		16						// �T�u CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
	};
	GFL_CLACT_SYS_Create( &init, &VramTbl, HEAPID_VS_MULTI_LIST );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�V�X�e���폜
 *
 * @param		none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void DeleteClactSys(void)
{
	GFL_CLACT_SYS_Delete();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X�쐬
 *
 * @param		wk		���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CreateClactResource( VSMLIST_WORK * wk )
{
  ARCHANDLE * ah;
  u32 * res;
  u16 index;
  u16 i;

	// �|�P�����A�C�R��
	ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, HEAPID_VS_MULTI_LIST_L );
  // �L����
	res = &wk->chrRes[CHRRES_POKEICON1];
	for( i=0; i<TEMOTI_POKEMAX; i++ ){
    if( wk->pp[i] != NULL ){
			index =  POKEICON_GetCgxArcIndex( PP_GetPPPPointerConst(wk->pp[i]) );
		}else{
			index =  POKEICON_GetCgxArcIndexByMonsNumber( 0, 0, 0, 0 );
		}
		res[i] = GFL_CLGRP_CGR_Register( ah, index, FALSE, CLSYS_DRAW_MAIN, HEAPID_VS_MULTI_LIST );
	}
  // �p���b�g
	res = &wk->palRes[PALRES_POKEICON];
	*res = GFL_CLGRP_PLTT_RegisterComp(
					ah, POKEICON_GetPalArcIndex(), CLSYS_DRAW_MAIN, PALOFS_POKEICON, HEAPID_VS_MULTI_LIST );
  // �Z���E�A�j��
	res = &wk->celRes[CELRES_POKEICON];
	*res = GFL_CLGRP_CELLANIM_Register(
					ah, POKEICON_GetCellSubArcIndex(), POKEICON_GetAnmSubArcIndex(), HEAPID_VS_MULTI_LIST );

  GFL_ARC_CloseDataHandle( ah );

  ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_VS_MULTI_LIST_L );

	// ��Ԉُ�A�C�R��
  // �L����
	res = &wk->chrRes[CHRRES_STATUSICON];
	*res = GFL_CLGRP_CGR_Register(
					ah, NARC_app_menu_common_p_st_ijou_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_VS_MULTI_LIST );
  // �p���b�g
	res = &wk->palRes[PALRES_STATUSICON];
	*res = GFL_CLGRP_PLTT_Register(
					ah, NARC_app_menu_common_p_st_ijou_NCLR, CLSYS_DRAW_MAIN, PALOFS_STATUSICON, HEAPID_VS_MULTI_LIST );
  // �Z���E�A�j��
	res = &wk->celRes[CELRES_STATUSICON];
	*res = GFL_CLGRP_CELLANIM_Register(
					ah, NARC_app_menu_common_p_st_ijou_128k_NCER, NARC_app_menu_common_p_st_ijou_128k_NANR, HEAPID_VS_MULTI_LIST );

	// �A�C�e���A�C�R��
  // �L����
	res = &wk->chrRes[CHRRES_ITEMICON];
	*res = GFL_CLGRP_CGR_Register(
					ah, NARC_app_menu_common_item_icon_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_VS_MULTI_LIST );
  // �p���b�g
	res = &wk->palRes[PALRES_ITEMICON];
	*res = GFL_CLGRP_PLTT_Register(
					ah, NARC_app_menu_common_item_icon_NCLR, CLSYS_DRAW_MAIN, PALOFS_ITEMICON, HEAPID_VS_MULTI_LIST );
  // �Z���E�A�j��
	res = &wk->celRes[CELRES_ITEMICON];
	*res = GFL_CLGRP_CELLANIM_Register(
					ah, NARC_app_menu_common_item_icon_128k_NCER, NARC_app_menu_common_item_icon_128k_NANR, HEAPID_VS_MULTI_LIST );

	// �g�o�Q�[�W
	// �L����
	res = &wk->chrRes[CHRRES_HP_GAUGE];
	*res = GFL_CLGRP_CGR_Register(
					ah, APP_COMMON_GetHPBarBaseCharArcIdx(APP_COMMON_MAPPING_128K), FALSE, CLSYS_DRAW_MAIN, HEAPID_VS_MULTI_LIST );
  // �p���b�g
	res = &wk->palRes[PALRES_HP_GAUGE];
	*res = GFL_CLGRP_PLTT_Register(
					ah, APP_COMMON_GetHPBarBasePltArcIdx(), CLSYS_DRAW_MAIN, PALOFS_HPGAUGE, HEAPID_VS_MULTI_LIST );
  // �Z���E�A�j��
	res = &wk->celRes[CELRES_HP_GAUGE];
  *res = GFL_CLGRP_CELLANIM_Register(
					ah, APP_COMMON_GetHPBarBaseCellArcIdx(APP_COMMON_MAPPING_128K),
					APP_COMMON_GetHPBarBaseAnimeArcIdx(APP_COMMON_MAPPING_128K), HEAPID_VS_MULTI_LIST );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X�폜
 *
 * @param		wk		���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void DeleteClactResource( VSMLIST_WORK * wk )
{
	u32	i;

	for( i=0; i<CHRRES_MAX; i++ ){
		GFL_CLGRP_CGR_Release( wk->chrRes[i] );
	}
	for( i=0; i<PALRES_MAX; i++ ){
    GFL_CLGRP_PLTT_Release( wk->palRes[i] );
	}
	for( i=0; i<CELRES_MAX; i++ ){
    GFL_CLGRP_CELLANIM_Release( wk->celRes[i] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ǉ�
 *
 * @param		wk		���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void AddObj( VSMLIST_WORK * wk )
{
	u32	i;

	wk->clunit = GFL_CLACT_UNIT_Create( OBJ_MAX, 0, HEAPID_VS_MULTI_LIST );

	for( i=0; i<OBJ_MAX; i++ ){
		wk->clwk[i] = GFL_CLACT_WK_Create(
										wk->clunit,
										wk->chrRes[ObjAddParam[i].chrRes],
										wk->palRes[ObjAddParam[i].palRes],
										wk->celRes[ObjAddParam[i].celRes],
										&ObjAddParam[i].dat, CLSYS_DRAW_MAIN, HEAPID_VS_MULTI_LIST );
	}

	for( i=0; i<TEMOTI_POKEMAX; i++ ){
		if( wk->pp[i] == NULL ){
		  GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_POKEICON1+i], FALSE );
		  GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ITEMICON1+i], FALSE );
		  GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_HP_GAUGE1+i], FALSE );
		}else{
			u16	item;
			// �|�P�A�C�R��
			GFL_CLACT_WK_SetAutoAnmFlag( wk->clwk[OBJ_POKEICON1+i], TRUE );
			GFL_CLACT_WK_SetPlttOffs(
				wk->clwk[OBJ_POKEICON1+i],
				POKEICON_GetPalNumGetByPPP(PP_GetPPPPointerConst(wk->pp[i])), CLWK_PLTTOFFS_MODE_PLTT_TOP );
			// �A�C�e���A�C�R��
			item = PP_Get( wk->pp[i], ID_PARA_item, NULL );
			if( item == ITEM_DUMMY_DATA ){
			  GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ITEMICON1+i], FALSE );
			}else if( ITEM_CheckMail(item) == TRUE ){
				GFL_CLACT_WK_SetAnmFrame( wk->clwk[OBJ_ITEMICON1+i], 0 );
				GFL_CLACT_WK_SetAnmSeq( wk->clwk[OBJ_ITEMICON1+i], APP_COMMON_ITEMICON_MAIL );
			}else{
				GFL_CLACT_WK_SetAnmFrame( wk->clwk[OBJ_ITEMICON1+i], 0 );
				GFL_CLACT_WK_SetAnmSeq( wk->clwk[OBJ_ITEMICON1+i], APP_COMMON_ITEMICON_ITEM );
			}
		}
		// ��Ԉُ�A�C�R���@����Ԉُ�͂Ȃ��͂��Ȃ̂Ŕ�\���ɂ��Ă���
	  GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_STATUSICON1+i], FALSE );
	}

	if( wk->dat->pos == VS_MULTI_LIST_POS_LEFT ){
		s16	mv = ( BPL_COMM_BSX_PLATE * 8 );
		for( i=0; i<3; i++ ){
			MoveObjOne( wk, OBJ_POKEICON4+i, mv );			// �|�P�����A�C�R��
			MoveObjOne( wk, OBJ_ITEMICON4+i, mv );			// �A�C�e���A�C�R��
			MoveObjOne( wk, OBJ_STATUSICON4+i, mv );		// ��Ԉُ�A�C�R��
			MoveObjOne( wk, OBJ_HP_GAUGE4+i, mv );			// �g�o�Q�[�W
		}
	}else{
		s16	mv = -( BPL_COMM_BSX_PLATE * 8 );
		for( i=0; i<3; i++ ){
			MoveObjOne( wk, OBJ_POKEICON1+i, mv );			// �|�P�����A�C�R��
			MoveObjOne( wk, OBJ_ITEMICON1+i, mv );			// �A�C�e���A�C�R��
			MoveObjOne( wk, OBJ_STATUSICON1+i, mv );		// ��Ԉُ�A�C�R��
			MoveObjOne( wk, OBJ_HP_GAUGE1+i, mv );			// �g�o�Q�[�W
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�폜
 *
 * @param		wk		���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void DelObj( VSMLIST_WORK * wk )
{
	u32	i;

	for( i=0; i<OBJ_MAX; i++ ){
		GFL_CLACT_WK_Remove( wk->clwk[i] );
	}
	GFL_CLACT_UNIT_Delete( wk->clunit );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ړ��i�ʁj
 *
 * @param		wk		���[�N
 * @param		idx		�C���f�b�N�X
 * @param		mv		�ړ��l
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void MoveObjOne( VSMLIST_WORK * wk, u32 idx, s16 mv )
{
	GFL_CLACTPOS	pos;

	GFL_CLACT_WK_GetPos( wk->clwk[idx], &pos, CLSYS_DRAW_MAIN );
	pos.x += mv;
	GFL_CLACT_WK_SetPos( wk->clwk[idx], &pos, CLSYS_DRAW_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ړ��i�p�[�e�B�P�ʁj
 *
 * @param		wk		���[�N
 * @param		start	�C���f�b�N�X�J�n�l
 * @param		mv		�ړ��l
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void MoveObj( VSMLIST_WORK * wk, u8 start, s16 mv )
{
	GFL_CLACTPOS	pos;
	u32	i;

	for( i=start; i<start+3; i++ ){
		MoveObjOne( wk, OBJ_POKEICON1+i, mv );			// �|�P�����A�C�R��
		MoveObjOne( wk, OBJ_ITEMICON1+i, mv );			// �A�C�e���A�C�R��
		MoveObjOne( wk, OBJ_STATUSICON1+i, mv );		// ��Ԉُ�A�C�R��
		MoveObjOne( wk, OBJ_HP_GAUGE1+i, mv );			// �g�o�Q�[�W
	}
}


//============================================================================================
//	BGWINFRM
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�E�B���h�E�t���[��������
 *
 * @param		wk		���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void InitBgWinFrame( VSMLIST_WORK * wk )
{
	u16 * buf;
	u16 * scr;
	u16	i, j;

	wk->wfrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, BGWF_MAX, HEAPID_VS_MULTI_LIST );

	for( i=0; i<TEMOTI_POKEMAX; i++ ){
		BGWINFRM_Add( wk->wfrm, BGWF_POKE1_PLATE+i, GFL_BG_FRAME1_M, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
		BGWINFRM_Add( wk->wfrm, BGWF_POKE1_PARAM+i, GFL_BG_FRAME0_M, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );

		BGWINFRM_BmpWinOn( wk->wfrm, BGWF_POKE1_PARAM+i, wk->win[i].win );
	}

	scr = GFL_BG_GetScreenBufferAdrs( GFL_BG_FRAME1_M );
	buf = GFL_HEAP_AllocMemory( HEAPID_VS_MULTI_LIST_L, BPL_COMM_BSX_PLATE*BPL_COMM_BSY_PLATE*2 );

	// �ʏ�̃v���[�g��ݒ�
	for( i=0; i<BPL_COMM_BSY_PLATE; i++ ){
		GFL_STD_MemCopy16(
			&scr[(NORMAL_PLATE_PY+i)*32+NORMAL_PLATE_PX], &buf[i*BPL_COMM_BSX_PLATE], BPL_COMM_BSX_PLATE*2 );
	}
	for( i=0; i<TEMOTI_POKEMAX; i++ ){
		if( wk->pp[i] != NULL ){
			BGWINFRM_FrameSet( wk->wfrm, BGWF_POKE1_PLATE+i, buf );
		}
	}
	// �󔒂̃v���[�g��ݒ�
	for( i=0; i<BPL_COMM_BSY_PLATE; i++ ){
		GFL_STD_MemCopy16(
			&scr[(NONE_PLATE_PY+i)*32+NONE_PLATE_PX], &buf[i*BPL_COMM_BSX_PLATE], BPL_COMM_BSX_PLATE*2 );
	}
	for( i=0; i<TEMOTI_POKEMAX; i++ ){
		if( wk->pp[i] == NULL ){
			BGWINFRM_FrameSet( wk->wfrm, BGWF_POKE1_PLATE+i, buf );
		}
	}

	GFL_HEAP_FreeMemory( buf );

	GFL_BG_ClearScreen( GFL_BG_FRAME1_M );

	// �}���`�p�ɐF��ύX
	if( wk->dat->pos == VS_MULTI_LIST_POS_LEFT ){
		for( i=3; i<6; i++ ){
			if( wk->pp[i] != NULL ){
				BGWINFRM_PaletteChange(
					wk->wfrm, BGWF_POKE1_PLATE+i, 0, 0, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE, BPL_COMM_PAL_MN_PLATE );
			}
		}
	}else{
		for( i=0; i<3; i++ ){
			if( wk->pp[i] != NULL ){
				BGWINFRM_PaletteChange(
					wk->wfrm, BGWF_POKE1_PLATE+i, 0, 0, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE, BPL_COMM_PAL_MN_PLATE );
			}
		}
	}

	if( wk->dat->pos == VS_MULTI_LIST_POS_LEFT ){
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE1_PLATE, BPL_COMM_SCR_POKE1_PX, BPL_COMM_SCR_POKE1_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE2_PLATE, BPL_COMM_SCR_POKE3_PX, BPL_COMM_SCR_POKE3_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE3_PLATE, BPL_COMM_SCR_POKE5_PX, BPL_COMM_SCR_POKE5_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE1_PARAM, BPL_COMM_SCR_POKE1_PX, BPL_COMM_SCR_POKE1_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE2_PARAM, BPL_COMM_SCR_POKE3_PX, BPL_COMM_SCR_POKE3_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE3_PARAM, BPL_COMM_SCR_POKE5_PX, BPL_COMM_SCR_POKE5_PY );

		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE4_PLATE, BPL_COMM_SCR_POKE2_PX+BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE2_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE5_PLATE, BPL_COMM_SCR_POKE4_PX+BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE4_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE6_PLATE, BPL_COMM_SCR_POKE6_PX+BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE6_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE4_PARAM, BPL_COMM_SCR_POKE2_PX+BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE2_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE5_PARAM, BPL_COMM_SCR_POKE4_PX+BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE4_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE6_PARAM, BPL_COMM_SCR_POKE6_PX+BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE6_PY );
	}else{
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE1_PLATE, BPL_COMM_SCR_POKE1_PX-BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE1_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE2_PLATE, BPL_COMM_SCR_POKE3_PX-BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE3_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE3_PLATE, BPL_COMM_SCR_POKE5_PX-BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE5_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE1_PARAM, BPL_COMM_SCR_POKE1_PX-BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE1_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE2_PARAM, BPL_COMM_SCR_POKE3_PX-BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE3_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE3_PARAM, BPL_COMM_SCR_POKE5_PX-BPL_COMM_BSX_PLATE, BPL_COMM_SCR_POKE5_PY );

		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE4_PLATE, BPL_COMM_SCR_POKE2_PX, BPL_COMM_SCR_POKE2_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE5_PLATE, BPL_COMM_SCR_POKE4_PX, BPL_COMM_SCR_POKE4_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE6_PLATE, BPL_COMM_SCR_POKE6_PX, BPL_COMM_SCR_POKE6_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE4_PARAM, BPL_COMM_SCR_POKE2_PX, BPL_COMM_SCR_POKE2_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE5_PARAM, BPL_COMM_SCR_POKE4_PX, BPL_COMM_SCR_POKE4_PY );
		BGWINFRM_FramePut( wk->wfrm, BGWF_POKE6_PARAM, BPL_COMM_SCR_POKE6_PX, BPL_COMM_SCR_POKE6_PY );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�E�B���h�E�t���[�����
 *
 * @param		wk		���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ExitBgWinFrame( VSMLIST_WORK * wk )
{
	BGWINFRM_Exit( wk->wfrm );
}
