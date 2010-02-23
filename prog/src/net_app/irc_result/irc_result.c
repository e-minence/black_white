//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_result_main.c
 *	@brief	�ԊO���~�j�Q�[��	���ʕ\��
 *	@author	Toru=Nagihashi
 *	@data		2009.05.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>

//	constant
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	module
#include "infowin/infowin.h"

#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/bmp_winframe.h"

//	archive
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_irc_result.h"
#include "irccompatible_gra.naix"

#include "net_app/irc_appbar.h"

//	result
#include "net_app/irc_result.h"

//savedata
#include "savedata/irc_compatible_savedata.h"

//sound
#include "../irc_compatible/irc_compatible_snd.h"

#ifdef PM_DEBUG
//debug�p
#include "system/net_err.h"	//VRAM�ޔ�p�A�h���X��Ⴄ����
#include <wchar.h>					//wcslen,swprintf 
#endif

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#ifdef PM_DEBUG
#endif //PM_DEBUG

//#define OBJNUMBER_SCALE	//�X�P�[������

//-------------------------------------
///	�}�N��
//=====================================

//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	// ���C�����BG
	RESULT_BG_PAL_M_00 = 0,// �w�i
	RESULT_BG_PAL_M_01,		//  
	RESULT_BG_PAL_M_02,		// 
	RESULT_BG_PAL_M_03,		// 
	RESULT_BG_PAL_M_04,		// 
	RESULT_BG_PAL_M_05,		// 
	RESULT_BG_PAL_M_06,		// 
	RESULT_BG_PAL_M_07,		// 
	RESULT_BG_PAL_M_08,		// �n�[�g
	RESULT_BG_PAL_M_09,		// �g�p���ĂȂ�
	RESULT_BG_PAL_M_10,		// �g�p���ĂȂ�
	RESULT_BG_PAL_M_11,		// �g�p���ĂȂ�
	RESULT_BG_PAL_M_12,		// �g�p���ĂȂ�
	RESULT_BG_PAL_M_13,		// APPBAR
	RESULT_BG_PAL_M_14,		// �t�H���g
	RESULT_BG_PAL_M_15,		// INFOWIN


	// �T�u���BG
	RESULT_BG_PAL_S_00 = 0,	//�w�i
	RESULT_BG_PAL_S_01,		// 
	RESULT_BG_PAL_S_02,		// 
	RESULT_BG_PAL_S_03,		// 
	RESULT_BG_PAL_S_04,		// 
	RESULT_BG_PAL_S_05,		// 
	RESULT_BG_PAL_S_06,		// 
	RESULT_BG_PAL_S_07,		// 
	RESULT_BG_PAL_S_08,		// �w�i
	RESULT_BG_PAL_S_09,		// �g�p���ĂȂ�
	RESULT_BG_PAL_S_10,		// �g�p���ĂȂ�
	RESULT_BG_PAL_S_11,		// �g�p���ĂȂ�
	RESULT_BG_PAL_S_12,		// �g�p���ĂȂ�
	RESULT_BG_PAL_S_13,		// �g�p���ĂȂ�
	RESULT_BG_PAL_S_14,		// �t�H���g
	RESULT_BG_PAL_S_15,		// �g�p���ĂȂ�

	// ���C�����OBJ
	RESULT_OBJ_PAL_M_00 = 0,// 
	RESULT_OBJ_PAL_M_01,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_02,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_03,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_04,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_05,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_06,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_07,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_08,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_09,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_10,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_11,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_12,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_13,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_14,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_M_15,		// �ʐM�A�C�R��


	// �T�u���OBJ
	RESULT_OBJ_PAL_S_00 = 0,	//�g�p���ĂȂ�
	RESULT_OBJ_PAL_S_01,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_02,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_03,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_04,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_05,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_06,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_07,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_08,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_09,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_10,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_11,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_12,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_13,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_14,		// �g�p���ĂȂ�
	RESULT_OBJ_PAL_S_15,		// �g�p���ĂȂ�
};

//-------------------------------------
///	���o�[
//=====================================

//-------------------------------------
///	����
//=====================================
#define TEXTSTR_PLT_NO				(RESULT_BG_PAL_S_14)
#define TEXTSTR_BUFFER_LENGTH	(255)

//-------------------------------------
///	�ʒu
//=====================================
#define	MSGWND_SUB_X	(1)
#define	MSGWND_SUB_Y	(18)
#define	MSGWND_SUB_W	(30)
#define	MSGWND_SUB_H	(5)

#define	MSGWND_TITLE_X	(6)
#define	MSGWND_TITLE_Y	(3)
#define	MSGWND_TITLE_W	(20)
#define	MSGWND_TITLE_H	(5)
//-------------------------------------
///	��
//=====================================
#define CLWK_NUMBER_MAX				(3)

//-------------------------------------
///	�J�E���^
//=====================================
#define SEQFUNC_DECIDEHEART_WAIT	(60)
#define HEART_SCALSE_WAIT	(180)

//-------------------------------------
///		MSG_FONT
//=====================================
typedef enum
{	
	MSG_FONT_TYPE_LARGE,
}MSG_FONT_TYPE;

//-------------------------------------
///	OBJ�o�^ID
//=====================================
enum 
{
	OBJREGID_PLT_M,
	OBJREGID_CHR_M,
	OBJREGID_CEL_M,
	OBJREGID_MAX
};

//-------------------------------------
///	MSGWNDID
//=====================================
typedef enum
{
	MSGWNDID_SUB,

	MSGWNDID_MAX
} MSGWNDID;

//-------------------------------------
///	OBJ�̐ݒ�l
//=====================================
#define SCALE_MIN					(0x11)
#define NUMBER_SCALE_SYNC	(60)

#define NUMBERSCROLL_ALPHA_SYNC	(30)
#define NUMBERSCROLL_ALPHA			(16)

//-------------------------------------
///		BACKOBJ
//=====================================
//���OBJ�̐�
#define BACKOBJ_CLWK_MAX	(32)
//
typedef enum
{
	BACKOBJ_MOVE_TYPE_RAIN,		//�J�̂悤�ɏォ��~�蒍��
	BACKOBJ_MOVE_TYPE_EMITER,	//���o
	BACKOBJ_MOVE_TYPE_GATHER,	//�W��
} BACKOBJ_MOVE_TYPE;

//OBJ�̃p���b�g�ƑΉ����Ă���
typedef enum
{
	BACKOBJ_COLOR_RED			= 1,
	BACKOBJ_COLOR_ORANGE	= 5,
	BACKOBJ_COLOR_YELLOW	= 6,
	BACKOBJ_COLOR_YEGREEN	= 7,
	BACKOBJ_COLOR_GREEN		= 8,
	BACKOBJ_COLOR_WATER		= 9,
	BACKOBJ_COLOR_BLUE		= 0xA,
} BACKOBJ_COLOR;

//�P��OBJ�������܂ł̃V���N
#define BACKOBJ_MOVE_SYNC	(10)

#define BACKOBJ_ONE_MOVE_SYNC_MIN	(150)
#define BACKOBJ_ONE_MOVE_SYNC_MAX	(160)
#define BACKOBJ_ONE_MOVE_SYNC_DIF	(BACKOBJ_ONE_MOVE_SYNC_MAX-BACKOBJ_ONE_MOVE_SYNC_MIN)

enum
{
	BACKOBJ_SYS_MAIN,
	BACKOBJ_SYS_NUM,
} ;

//-------------------------------------
///	CLWKID
//=====================================
typedef enum
{	
	CLWKID_NUMBER_TOP,
	CLWKID_NUMBER_END	= CLWKID_NUMBER_TOP+CLWK_NUMBER_MAX,

	CLWKID_BACKOBJ_TOP_M,
	CLWKID_BACKOBJ_END_M	= CLWKID_BACKOBJ_TOP_M + BACKOBJ_CLWK_MAX,

	CLWKID_ORIGIN,

	CLWKID_MAX,
}CLWKID;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	BG�֌W
//=====================================
typedef struct
{
	GFL_ARCUTIL_TRANSINFO	frame_char;
} GRAPHIC_BG_WORK;

//-------------------------------------
///	OBJ�֌W
//=====================================
typedef struct 
{
	GFL_CLUNIT	*p_clunit;
	u32					reg_id[OBJREGID_MAX];
	GFL_CLWK		*p_clwk[CLWKID_MAX];
} GRAPHIC_OBJ_WORK;

//-------------------------------------
///	�`��֌W
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_OBJ_WORK	obj;
	GFL_TCB						*p_vblank_task;
} GRAPHIC_WORK;
//-------------------------------------
///	�����Ǘ�
//=====================================
typedef struct 
{
	GFL_FONT*				  p_font;
  GFL_MSGDATA*			p_msg;
  PRINT_QUE*        p_print_que;
	WORDSET*					p_wordset;
} MSG_WORK;

//-------------------------------------
///	���b�Z�[�W�\���E�B���h�E
//=====================================
typedef struct
{
	GFL_BMPWIN*				p_bmpwin;
	PRINT_UTIL        print_util;
	STRBUF*						p_strbuf;
	u16								clear_chr;
	u16								dummy;
} MSGWND_WORK;
//-------------------------------------
///	BACKOBJ	�w�ʂ҂��҂�
//=====================================
typedef struct
{	
	GFL_CLWK					*p_clwk;
	int								sf_type;
	BOOL							is_req;
	VecFx32						start;
	VecFx32						end;
	u32								sync_now;
	u32								sync_max;
} BACKOBJ_ONE;
typedef struct 
{
	BACKOBJ_ONE				wk[BACKOBJ_CLWK_MAX];
	BACKOBJ_MOVE_TYPE	type;
	BACKOBJ_COLOR			color;
	u32								sync_now;	//�P�̃��[�N���J�n����܂ł̃V���N
	u32								sync_max;
	BOOL							is_start;
} BACKOBJ_WORK;
//-------------------------------------
///	BG���o
//=====================================
typedef struct 
{
  u16 frm;
  u32 sync;
  u16 plt_original[0x10];
  u16 plt_buff[0x10];
  u32 cnt;
  BOOL is_end;
  BOOL is_start;
  u16 rank;
  u16 now_color_idx;
  u32 seq;
  BOOL is_lie;
} HEARTEFF_WORK;

//-------------------------------------
///	OBJ����
//=====================================
typedef struct
{
	GFL_CLWK *p_clwk[CLWK_NUMBER_MAX];
	u16		sync[CLWK_NUMBER_MAX];
	u16		sync_max[CLWK_NUMBER_MAX];
	fx32	scale_add[CLWK_NUMBER_MAX];
	BOOL	start[CLWK_NUMBER_MAX];
	u8		number[CLWK_NUMBER_MAX];	//3��
	u32		is_end[CLWK_NUMBER_MAX];
} OBJNUMBER_WORK;

//-------------------------------------
///	��������]
//=====================================
typedef struct 
{
	u32		frm;
	BOOL	is_move_req;
	BOOL	is_vanish_req;
	fx32	alpha;
	fx32	alpha_end;
	fx32	alpha_add;
	u32		sync;
	BOOL	is_vanish;
} NUMBERSCROLL_WORK;

//-------------------------------------
///	���ʕ\�����C�����[�N
//=====================================
typedef struct _RESULT_MAIN_WORK RESULT_MAIN_WORK;
typedef void (*SEQ_FUNCTION)( RESULT_MAIN_WORK *p_wk, u16 *p_seq );
struct _RESULT_MAIN_WORK
{
	//�O���t�B�b�N���W���[��
	GRAPHIC_WORK		grp;
	MSG_WORK				msg;
	MSGWND_WORK			msgwnd[MSGWNDID_MAX];

	MSGWND_WORK			msgtitle;	//�^�C�g�����b�Z�[�W

	OBJNUMBER_WORK	number;

	HEARTEFF_WORK		heart;

	//����ʃo�[
	APPBAR_WORK			*p_appbar;

	//�w�ʂ҂��҂�
	//BACKOBJ_WORK		backobj[BACKOBJ_SYS_NUM];

	//�V�[�P���X�Ǘ�
	SEQ_FUNCTION		seq_function;
	u16		seq;
	BOOL	is_end;

	//���̑��ėp
	u32		cnt;

	//����
	IRC_RESULT_PARAM	*p_param;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//proc
static GFL_PROC_RESULT IRC_RESULT_PROC_Init(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RESULT_PROC_Exit(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RESULT_PROC_Main(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
//grp
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
static GFL_CLWK* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT * GRAPHIC_GetClunit( const GRAPHIC_WORK *cp_wk );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//BG
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//obj
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, 
		const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT * GRAPHIC_OBJ_GetClunit( const GRAPHIC_OBJ_WORK *cp_wk );
//MSG_WORK
static void MSG_Init( MSG_WORK *p_wk, MSG_FONT_TYPE font, HEAPID heapID );
static void MSG_Exit( MSG_WORK *p_wk );
static BOOL MSG_Main( MSG_WORK *p_wk );
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk );
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk );
static const GFL_MSGDATA * MSG_GetMsgDataConst( const MSG_WORK *cp_wk );
static WORDSET * MSG_GetWordSet( const MSG_WORK *cp_wk );
//MSG_WINDOW
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe, u8 x, u8 y, u8 w, u8 h, u8 plt, HEAPID heapID );
static void MSGWND_InitEx( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, u8 plt, u16 clear_chr, u8 dir, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, MSG_WORK *p_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk,
		const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y );
static void MSGWND_PrintCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID );
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, 
		u32 strID, u16 number, u16 buff_id );
static void MSGWND_PrintBothNameCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, const COMPATIBLE_STATUS *cp_my, const COMPATIBLE_STATUS *cp_you, HEAPID heapID );
static void MSGWND_Clear( MSGWND_WORK* p_wk );
//SEQ
static void SEQ_Change( RESULT_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function );
static void SEQ_End( RESULT_MAIN_WORK *p_wk );
//SEQ_FUNC
static void SEQFUNC_StartGame( RESULT_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_DecideScore( RESULT_MAIN_WORK *p_wk, u16 *p_seq );
//OBJNUMBER
static void OBJNUMBER_Init( OBJNUMBER_WORK *p_wk, const GRAPHIC_WORK *cp_grp, int number );
static void OBJNUMBER_Start( OBJNUMBER_WORK *p_wk, int fig );
static BOOL OBJNUMBER_Main( OBJNUMBER_WORK *p_wk );
static BOOL OBJNUMBER_IsEnd( const OBJNUMBER_WORK *cp_wk, u16 fig );
static u8 OBJNUMBER_GetFig( const OBJNUMBER_WORK *cp_wk, int fig );
static s16 OBJNUMBER_GetFigPosX( const OBJNUMBER_WORK *cp_wk, int fig );
static void ObjNumber_SetNumber( OBJNUMBER_WORK *p_wk, int number );
//SCALEBG
static void HEARTEFF_Init( HEARTEFF_WORK *p_wk, u16 frm, u8 score, HEAPID heapID );
static void HEARTEFF_Exit( HEARTEFF_WORK *p_wk );
static void HEARTEFF_Main( HEARTEFF_WORK *p_wk );
static void HEARTEFF_Start( HEARTEFF_WORK *p_wk );
static BOOL HEARTEFF_IsEnd( const HEARTEFF_WORK *cp_wk );
static void HeartEff_MainPltFade( u16 *p_buff, u16 cnt, u16 cnt_max, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end );
//BACKOBJ
static void BACKOBJ_Init( BACKOBJ_WORK *p_wk, const GRAPHIC_WORK *cp_grp, BACKOBJ_MOVE_TYPE type, BACKOBJ_COLOR color, u32 clwk_ofs, int sf_type );
static void BACKOBJ_Exit( BACKOBJ_WORK *p_wk );
static void BACKOBJ_Main( BACKOBJ_WORK *p_wk );
static void BACKOBJ_StartGather( BACKOBJ_WORK *p_wk, BOOL is_start );
static BOOL BACKOBJ_IsEnd( const BACKOBJ_WORK *cp_wk );
//BACKOBJ_ONE
static void BACKOBJ_ONE_Init( BACKOBJ_ONE *p_wk, GFL_CLWK *p_clwk, BACKOBJ_COLOR color, int sf_type );
static void BACKOBJ_ONE_Main( BACKOBJ_ONE *p_wk );
static void BACKOBJ_ONE_Start( BACKOBJ_ONE *p_wk, const GFL_POINT *cp_start, const GFL_POINT *cp_end, BACKOBJ_COLOR color, u32 sync );
static BOOL BACKOBJ_ONE_IsMove( const BACKOBJ_ONE *cp_wk );

typedef enum
{ 
  SCORE_RANK_WORST,
  SCORE_RANK_LOW_02,
  SCORE_RANK_LOW_01,
  SCORE_RANK_LOW_00,
  SCORE_RANK_HIGHT,
  SCORE_RANK_BEST,

  SCORE_RANK_MAX
}SCORE_RANK;
static SCORE_RANK UTIL_GetScoreRank( u8 score, BOOL *p_is_nearly );


//�ėp
//�ėp�I�ȓ���
//�Ԃ������̓���
//DEBUG_PRINT

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	���ʕ\���p�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA IrcResult_ProcData	= 
{	
	IRC_RESULT_PROC_Init,
	IRC_RESULT_PROC_Main,
	IRC_RESULT_PROC_Exit,
};
//-------------------------------------
///	BG�V�X�e��
//=====================================
typedef enum 
{
	GRAPHIC_BG_FRAME_M_INFOWIN,
	GRAPHIC_BG_FRAME_M_BOTTLE,
	GRAPHIC_BG_FRAME_M_BACK,
	GRAPHIC_BG_FRAME_M_HEART,
	GRAPHIC_BG_FRAME_S_ROGO,
	GRAPHIC_BG_FRAME_S_TEXT,
	GRAPHIC_BG_FRAME_S_BACK,
	GRAPHIC_BG_FRAME_S_TITLE,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	GFL_BG_FRAME0_M, GFL_BG_FRAME1_M, GFL_BG_FRAME2_M, GFL_BG_FRAME3_M,
	GFL_BG_FRAME0_S, GFL_BG_FRAME1_S, GFL_BG_FRAME2_S, GFL_BG_FRAME3_S,
};
static const GFL_BG_BGCNT_HEADER sc_bgcnt_data[ GRAPHIC_BG_FRAME_MAX ] = 
{
	// GRAPHIC_BG_FRAME_M_INFOWIN	
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_M_BOTTLE
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_M_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_M_HEART
	{
		0, 0, 0x0800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_ROGO
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_TEXT
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_TITLE
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
		GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ256_128x128,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
};
static const u32 sc_bgmode[ GRAPHIC_BG_FRAME_MAX ] =
{	
	GFL_BG_MODE_TEXT,GFL_BG_MODE_TEXT,GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT,
	GFL_BG_MODE_TEXT,GFL_BG_MODE_TEXT,GFL_BG_MODE_TEXT, GFL_BG_MODE_AFFINE,
};

//=============================================================================
/**
 *			PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���ʕ\��	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RESULT_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	RESULT_MAIN_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCRESULT, 0x20000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(RESULT_MAIN_WORK), HEAPID_IRCRESULT );
	GFL_STD_MemClear( p_wk, sizeof(RESULT_MAIN_WORK) );
	p_wk->p_param	= p_param;

	//���W���[��������
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCRESULT );
	MSG_Init( &p_wk->msg, MSG_FONT_TYPE_LARGE, HEAPID_IRCRESULT );
	{
		GAME_COMM_SYS_PTR comm	= NULL;
		if( p_wk->p_param->p_gamesys )
		{	
			comm	= GAMESYSTEM_GetGameCommSysPtr(p_wk->p_param->p_gamesys);
		}
		//INFOWIN_Init( GFL_BG_FRAME0_M, RESULT_BG_PAL_M_15, comm, HEAPID_IRCRESULT );
	}

	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_SUB], sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TEXT],
			MSGWND_SUB_X, MSGWND_SUB_Y, MSGWND_SUB_W, MSGWND_SUB_H, RESULT_BG_PAL_S_05, HEAPID_IRCRESULT );
	BmpWinFrame_Write( p_wk->msgwnd[MSGWNDID_SUB].p_bmpwin, WINDOW_TRANS_ON, 
			GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->grp.bg.frame_char), RESULT_BG_PAL_S_06 );

	//�^�C�g��������쐬
	MSGWND_InitEx( &p_wk->msgtitle, sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE],
			MSGWND_TITLE_X, MSGWND_TITLE_Y, MSGWND_TITLE_W, MSGWND_TITLE_H, RESULT_BG_PAL_S_08, 1, GFL_BMP_CHRAREA_GET_B, HEAPID_IRCRESULT );
	MSGWND_PrintCenter( &p_wk->msgtitle, &p_wk->msg, RESULT_TITLE_000 );
	GFL_BG_SetScaleReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_SCALE_X_SET, TITLE_STR_SCALE_X );
	GFL_BG_SetScaleReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_SCALE_Y_SET, TITLE_STR_SCALE_Y );
	GFL_BG_SetRotateCenterReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_CENTER_X_SET, (MSGWND_TITLE_X + MSGWND_TITLE_W/2)*8 );
	GFL_BG_SetRotateCenterReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_CENTER_Y_SET, (MSGWND_TITLE_Y + MSGWND_TITLE_H/2)*8 );

	//OBJ���l
	OBJNUMBER_Init( &p_wk->number, &p_wk->grp, p_wk->p_param->score );

	//BACKOBJ_Init( &p_wk->backobj[BACKOBJ_SYS_MAIN], &p_wk->grp, BACKOBJ_MOVE_TYPE_GATHER, BACKOBJ_COLOR_RED, CLWKID_BACKOBJ_TOP_M, CLSYS_DEFREND_MAIN );

	//�������b�Z�[�W
	if( p_wk->p_param->p_gamesys )
	{	
    COMPATIBLE_STATUS my_status;
    COMPATIBLE_IRC_GetStatus( p_wk->p_param->p_gamesys, &my_status );

		MSGWND_PrintBothNameCenter( &p_wk->msgwnd[MSGWNDID_SUB], &p_wk->msg, 
				RESULT_STR_000, &my_status, p_wk->p_param->p_you_status, HEAPID_IRCRESULT );
	}

	//APPBAR
	{	
		GFL_CLUNIT	*p_unit	= GRAPHIC_GetClunit( &p_wk->grp );
		p_wk->p_appbar	= APPBAR_Init( APPBAR_OPTION_MASK_RETURN, p_unit, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_INFOWIN], RESULT_BG_PAL_M_13, RESULT_OBJ_PAL_M_13, APP_COMMON_MAPPING_128K, MSG_GetFont(&p_wk->msg ), MSG_GetPrintQue(&p_wk->msg ), HEAPID_IRCRESULT );
	}

	//�n�[�g
	HEARTEFF_Init( &p_wk->heart, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_HEART], p_wk->p_param->score, HEAPID_IRCRESULT );


	//�����ŒʐM����ƁA�ʐM�I���̑���ɑ���A���j�b�g�G���[�ɂȂ�
	//
#if 0
	if( p_wk->p_param->p_irc )
	{	
		COMPATIBLE_IRC_SetScene( p_wk->p_param->p_irc, COMPATIBLE_SCENE_RESULT );
	}
#endif

	SEQ_Change( p_wk, SEQFUNC_StartGame );
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���ʕ\��	���C���v���Z�X�j������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RESULT_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	RESULT_MAIN_WORK	*p_wk;

	p_wk	= p_work;


	HEARTEFF_Exit( &p_wk->heart );

	//���W���[���j��
	MSGWND_Exit( &p_wk->msgtitle );
	{		
		int i;
		for( i = 0; i < BACKOBJ_SYS_NUM; i++ )
		{	
			//BACKOBJ_Exit( &p_wk->backobj[i] );
		}
	}

	//APPBAR
	APPBAR_Exit( p_wk->p_appbar );

	//���W���[���j��
	{	
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Exit( &p_wk->msgwnd[i] );
		}
	}
	//INFOWIN_Exit();
	GRAPHIC_Exit( &p_wk->grp );
	MSG_Exit( &p_wk->msg );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_IRCRESULT );
	
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���ʕ\��	���C���v���Z�X���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RESULT_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_MAIN,
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};

	RESULT_MAIN_WORK	*p_wk;
	p_wk	= p_work;

	switch( *p_seq )
	{	
	case SEQ_INIT:
		*p_seq	= SEQ_FADEOUT_START;
		break;

	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_MAIN;
		}
		break;

	case SEQ_MAIN:

		p_wk->seq_function( p_wk, &p_wk->seq );
		if( p_wk->is_end )
		{	
			*p_seq	= SEQ_FADEIN_START;
		}

		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
		{	
			SEQ_Change( p_wk, SEQFUNC_StartGame );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
		{
			SEQ_End( p_wk );
		}

		break;

	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;
	
	case SEQ_EXIT:
		return GFL_PROC_RES_FINISH;

	default:
		GF_ASSERT_MSG( 0, "IRC_RESULT_PROC_Main��SEQ�G���[ %d", *p_seq );
	}

	//INFOWIN_Update();
	if( MSG_Main( &p_wk->msg ) )
	{
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Main( &p_wk->msgwnd[i], &p_wk->msg );
			MSGWND_Main( &p_wk->msgtitle, &p_wk->msg );
		}
	}


	//BACKOBJ_Main( &p_wk->backobj[BACKOBJ_SYS_MAIN] );
	GRAPHIC_Draw( &p_wk->grp );
	HEARTEFF_Main( &p_wk->heart );
	APPBAR_Main( p_wk->p_appbar );

	//�����Ń`�F�b�N���Ă��x��Ă���̂͑���Ȃ̂Ői�߂邱�Ƃ͂��Ȃ�
#if 0
	if( !COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) )
	{	
		SEQ_End( p_wk );
	}
#endif

	//�V�[�����p���I�ɑ���
	if( p_wk->p_param->p_irc )
	{	
		COMPATIBLE_IRC_SendSceneContinue( p_wk->p_param->p_irc );
	}

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *				GRAPHIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�`��֌W������
 *
 *	@param	GRAPHIC_WORK* p_wk	���[�N
 *	@param	heapID					�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Init( GRAPHIC_WORK* p_wk, HEAPID heapID )
{
	static const GFL_DISP_VRAM sc_vramSetTable =
	{
		GX_VRAM_BG_128_A,					// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,					// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,						// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,				// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,		
		GX_OBJVRAMMODE_CHAR_1D_128K,		
	};

	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );

	//VRAM�N���A�[
	GFL_DISP_ClearVRAM( 0 );

	// VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &sc_vramSetTable );

	// �f�B�X�v���CON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	//�\��
	GFL_DISP_GX_InitVisibleControl();

	//�`�惂�W���[��
	GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );
	GRAPHIC_BG_Init( &p_wk->bg, heapID );

	//VBlankTask�o�^
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_VBlankTask, p_wk, 0 );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�`��֌W�j��
 *
 *	@param	GRAPHIC_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Exit( GRAPHIC_WORK* p_wk )
{
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	GRAPHIC_BG_Exit( &p_wk->bg );
	GRAPHIC_OBJ_Exit( &p_wk->obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��֌W�`�揈��
 *
 *	@param	GRAPHIC_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Draw( GRAPHIC_WORK* p_wk )
{
	GRAPHIC_OBJ_Main( &p_wk->obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	CLWK�擾
 *
 *	@param	const GRAPHIC_WORK *cp_wk	���[�N
 *	@param	id												CLWK��ID
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id )
{	
	return GRAPHIC_OBJ_GetClwk( &cp_wk->obj, id );
}

//----------------------------------------------------------------------------
/**
 *	@brief	CLUNIT�擾
 *
 *	@param	const GRAPHIC_WORK *cp_wk		���[�N
 *
 *	@return	CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT * GRAPHIC_GetClunit( const GRAPHIC_WORK *cp_wk )
{	
	return GRAPHIC_OBJ_GetClunit( &cp_wk->obj );
}
//----------------------------------------------------------------------------
/**
 *	@brief	VBlank�p�^�X�N
 *
 *	@param	GFL_TCB *p_tcb	�^�X�NControl�u���b�N
 *	@param	*p_work					���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work )
{	
	GRAPHIC_WORK* p_wk	= p_work;
	GRAPHIC_BG_VBlankFunction( &p_wk->bg );
	GRAPHIC_OBJ_VBlankFunction( &p_wk->obj );
}

//=============================================================================
/**
 *					GRAPHIC_BG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG�`��	������
 *
 *	@param	GRAPHIC_BG_WORK* p_wk	���[�N
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK* p_wk, HEAPID heapID )
{
	int i;

	//�a�f�V�X�e���N��
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );

	//�a�f���[�h�ݒ�
	{
		static const GFL_BG_SYS_HEADER sc_bg_sys_header = 
		{
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_1,GX_BG0_AS_2D
		};	
		GFL_BG_SetBGMode( &sc_bg_sys_header );
	}

	//�a�f�R���g���[���ݒ�
	{
		int i;

		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{
			GFL_BG_SetBGControl( sc_bgcnt_frame[i], &sc_bgcnt_data[i], sc_bgmode[i] );
			GFL_BG_ClearFrame( sc_bgcnt_frame[i] );
			GFL_BG_SetVisible( sc_bgcnt_frame[i], VISIBLE_ON );
		}
	}

	//�ǂݍ��ݐݒ�
	{	
		ARCHANDLE	*p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCCOMPATIBLE, heapID );

		//�p���b�g
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_irccompatible_gra_result_bg_03_NCLR,
				PALTYPE_MAIN_BG, RESULT_BG_PAL_M_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_irccompatible_gra_result_bg_03_NCLR,
				PALTYPE_SUB_BG, RESULT_BG_PAL_S_00*0x20, 0, heapID );

		//�L����
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_aura_bg_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_aura_bg_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_title_rogo_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_ROGO], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_result_bg_01_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BOTTLE], 0, 0, FALSE, heapID );
	
		//�X�N���[��
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_aura_bg_block_b_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_aura_bg_block_b_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_title_ketsuka_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_ROGO], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_result_bg_01_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BOTTLE], 0, 0, FALSE, heapID );


		//���N
		GFL_BG_FillCharacter( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], 0, 1, 0 );
		p_wk->frame_char	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_irccompatible_gra_ue_frame_NCGR, sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], 0, FALSE, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�`��	�j��
 *
 *	@param	GRAPHIC_BG_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK* p_wk )
{	
	int i;
	//�ǂݍ��ݔj��
	{	
		GFL_BG_FreeCharacterArea(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT],
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_char),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->frame_char));
		GFL_BG_FillCharacterRelease(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], 1,0);
	}

	// BG�R���g���[���j��
	for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
	{
		GFL_BG_FreeBGControl( sc_bgcnt_frame[i] );
	}

	// BG�V�X�e���j��
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�`��	VBlack�֐�
 *
 *	@param	GRAPHIC_BG_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk )
{	
	GFL_BG_VBlankFunc();
}

//=============================================================================
/**
 *				OBJ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	������
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk			���[�N
 *	@param	GFL_DISP_VRAM* cp_vram_bank	�o���N�e�[�u��
 *	@param	heapID											�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );

	//�V�X�e���쐬
	GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, cp_vram_bank, heapID );
	p_wk->p_clunit	= GFL_CLACT_UNIT_Create( 128, 0, heapID );
	GFL_CLACT_UNIT_SetDefaultRend( p_wk->p_clunit );

	//�\��
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

	//���\�[�X�ǂݍ���
	{	
		ARCHANDLE *p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCCOMPATIBLE, heapID );

		p_wk->reg_id[OBJREGID_PLT_M]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_irccompatible_gra_aura_obj_NCLR, CLSYS_DRAW_MAIN, RESULT_OBJ_PAL_M_00*0x20, heapID );

		p_wk->reg_id[OBJREGID_CHR_M]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_irccompatible_gra_aura_obj_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

		p_wk->reg_id[OBJREGID_CEL_M]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_irccompatible_gra_aura_obj_NCER, 
				NARC_irccompatible_gra_aura_obj_NANR, heapID );


		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK�쐬
	{	
		int i;
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		for( i = CLWKID_NUMBER_TOP; i < CLWKID_NUMBER_END; i++  )
		{	
			p_wk->p_clwk[i]	= GFL_CLACT_WK_Create( p_wk->p_clunit, 
				p_wk->reg_id[OBJREGID_CHR_M],
				p_wk->reg_id[OBJREGID_PLT_M],
				p_wk->reg_id[OBJREGID_CEL_M],
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID
				);
		}

#if 0
		for( i = CLWKID_BACKOBJ_TOP_M; i < CLWKID_BACKOBJ_END_M; i++  )
		{	
			p_wk->p_clwk[i]	= GFL_CLACT_WK_Create( p_wk->p_clunit, 
					p_wk->reg_id[OBJREGID_CHR_M],
					p_wk->reg_id[OBJREGID_PLT_M],
					p_wk->reg_id[OBJREGID_CEL_M],
					&cldata,
					CLSYS_DEFREND_MAIN,
					heapID
					);
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
			GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[i], 2 );
			GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[i], TRUE );
			GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[i], 0 );
		}
#endif
	}

	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		cldata.pos_x	= 128;
		cldata.pos_y	= 96;
		p_wk->p_clwk[CLWKID_ORIGIN]	= GFL_CLACT_WK_Create( p_wk->p_clunit, 
				p_wk->reg_id[OBJREGID_CHR_M],
				p_wk->reg_id[OBJREGID_PLT_M],
				p_wk->reg_id[OBJREGID_CEL_M],
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID
				);
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CLWKID_ORIGIN], FALSE );
		GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[CLWKID_ORIGIN], 0 );
		GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk[CLWKID_ORIGIN], 0 );
		GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[CLWKID_ORIGIN], TRUE );
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[CLWKID_ORIGIN], 11 );
	}

	//CLWK�̐ݒ�
	{	
		int i;
		GFL_CLACTPOS	clpos	=
		{	
			128, 92
		};

		for( i = CLWKID_NUMBER_TOP; i < CLWKID_NUMBER_END; i++  )
		{	
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
			GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[i], 0 );
			GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk[i], 0 );
		}
	}


}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	�j��
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk )
{	
	//CLWK�j��
	{	
		int i;
		for( i = 0; i < CLWKID_MAX; i++ )
		{	
			if( p_wk->p_clwk[i] )
			{	
				GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
			}
		}
	}

	//���\�[�X�j��
	{	
		GFL_CLGRP_PLTT_Release( p_wk->reg_id[OBJREGID_PLT_M] );
		GFL_CLGRP_CGR_Release( p_wk->reg_id[OBJREGID_CHR_M] );
		GFL_CLGRP_CELLANIM_Release( p_wk->reg_id[OBJREGID_CEL_M] );

	}

	//�V�X�e���j��
	GFL_CLACT_UNIT_Delete( p_wk->p_clunit );
	GFL_CLACT_SYS_Delete();
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	���C������
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_Main();
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	V�u�����N����
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_VBlankFunc();
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	CLWK�擾
 *
 *	@param	const GRAPHIC_OBJ_WORK *cp_wk	���[�N
 *	@param	id														CLWK��ID
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id )
{	
	GF_ASSERT( id < CLWKID_MAX );
	return cp_wk->p_clwk[id];
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	CLUNIT�擾
 *
 *	@param	const GRAPHIC_OBJ_WORK *cp_wk		���[�N
 *
 *	@return	CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT * GRAPHIC_OBJ_GetClunit( const GRAPHIC_OBJ_WORK *cp_wk )
{	
	return cp_wk->p_clunit;
}
//=============================================================================
/**
 *					MSG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W��ݒ�
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *	@param	font						�t�H���g�^�C�v
 *	@param	heapID					�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSG_Init( MSG_WORK *p_wk, MSG_FONT_TYPE font, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSG_WORK) );

	GFL_FONTSYS_Init();

	switch( font )
	{	
	case MSG_FONT_TYPE_LARGE:
		p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
		break;
	default:
		GF_ASSERT_MSG( 0, "MSGFONT_ERRO %d", font );
	};

	p_wk->p_print_que = PRINTSYS_QUE_Create( heapID );

	p_wk->p_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_result_dat, heapID );

	p_wk->p_wordset	= WORDSET_Create( heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W��j��
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *	
 */
//-----------------------------------------------------------------------------
static void MSG_Exit( MSG_WORK *p_wk )
{	
	WORDSET_Delete( p_wk->p_wordset );

	GFL_MSG_Delete( p_wk->p_msg );

	PRINTSYS_QUE_Delete( p_wk->p_print_que );

	GFL_FONT_Delete( p_wk->p_font );
	GFL_STD_MemClear( p_wk, sizeof(MSG_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W	���C������
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *
 * @retval  BOOL	�������I�����Ă����TRUE�^����ȊO��FALSE
 *
 */
//-----------------------------------------------------------------------------
static BOOL MSG_Main( MSG_WORK *p_wk )
{	
	return PRINTSYS_QUE_Main( p_wk->p_print_que );
}

//----------------------------------------------------------------------------
/**
 *	@brief	FONT���擾
 *
 *	@param	const MSG_WORK *cp_wk		���[�N
 *
 *	@return	FONT
 */
//-----------------------------------------------------------------------------
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_font;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PRINTQUE���擾
 *
 *	@param	const MSG_WORK *cp_wk		���[�N
 *
 *	@return	PRINTQUE
 */
//-----------------------------------------------------------------------------
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_print_que;
}
//----------------------------------------------------------------------------
/**
 *	@brief	MSGDATA���擾
 *
 *	@param	const MSG_WORK *cp_wk		���[�N
 *
 *	@return	MSGDATA
 */
//-----------------------------------------------------------------------------
static const GFL_MSGDATA * MSG_GetMsgDataConst( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_msg;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WORDSET���擾
 *
 *	@param	const MSG_WORK *cp_wk ���[�N
 *
 *	@return	WORDSET
 */
//-----------------------------------------------------------------------------
static WORDSET * MSG_GetWordSet( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_wordset;
}

//=============================================================================
/**
 *			MSGWND
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\����	������
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	bgframe						�\������BG��
 *	@param	x									�J�nX�ʒu�i�L�����P�ʁj
 *	@param	y									�J�nY�ʒu�i�L�����P�ʁj
 *	@param	w									���i�L�����P�ʁj
 *	@param	h									�����i�L�����P�ʁj
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe, u8 x, u8 y, u8 w, u8 h, u8 plt, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
	p_wk->clear_chr	= 0xF;
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( bgframe, x, y, w, h, plt, GFL_BMP_CHRAREA_GET_B );
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( TEXTSTR_BUFFER_LENGTH, heapID );
	PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\����	������
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	bgframe						�\������BG��
 *	@param	x									�J�nX�ʒu�i�L�����P�ʁj
 *	@param	y									�J�nY�ʒu�i�L�����P�ʁj
 *	@param	w									���i�L�����P�ʁj
 *	@param	h									�����i�L�����P�ʁj
 *	@param	plt								�p���b�g�ԍ�
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_InitEx( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, u8 plt, u16 clear_chr, u8 dir, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
	p_wk->clear_chr	= clear_chr;
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( bgframe, x, y, w, h, plt, dir );
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( TEXTSTR_BUFFER_LENGTH, heapID );
	PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\����	�j��
 *
 *	@param	MSGWND_WORK* p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Exit( MSGWND_WORK* p_wk )
{	
	GFL_STR_DeleteBuffer( p_wk->p_strbuf );
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\����	���C������
 *
 *	@param	MSGWND_WORK *p_wk		���[�N
 *	@param	MSG_WORK	*p_msg		���b�Z�[�W
 *
 *	@retval  BOOL	�]�����I����Ă���ꍇ��TRUE�^�I����Ă��Ȃ��ꍇ��FALSE
 *
 */
//-----------------------------------------------------------------------------
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, MSG_WORK *p_msg )
{	
	return PRINT_UTIL_Trans( &p_wk->print_util, p_msg->p_print_que );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\���ʂɕ�����\��
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	MSG_WORK *cp_msg	�����Ǘ�
 *	@param	strID							����ID
 *	@param	x									�J�n�ʒuX
 *	@param	y									�J�n�ʒuY
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y )
{	
	const GFL_MSGDATA* cp_msgdata;
	PRINT_QUE*	p_que;
	GFL_FONT*		p_font;

	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );
	p_que		= MSG_GetPrintQue( cp_msg );
	p_font	= MSG_GetFont( cp_msg );

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//������쐬
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//�\��
	PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\���ʂ̒����ɕ�����\��
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	MSG_WORK *cp_msg	�����Ǘ�
 *	@param	strID							����ID
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID )
{	
	const GFL_MSGDATA* cp_msgdata;
	PRINT_QUE*	p_que;
	GFL_FONT*		p_font;
	u16 x, y;

	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );
	p_que		= MSG_GetPrintQue( cp_msg );
	p_font	= MSG_GetFont( cp_msg );

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//������쐬
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//�Z���^�[�ʒu�v�Z
	x	= GFL_BMPWIN_GetSizeX( p_wk->p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_wk->p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_wk->p_strbuf, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_wk->p_strbuf, p_font )/2;

	//�\��
	PRINT_UTIL_PrintColor( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font,PRINTSYS_LSB_Make(0xf,0xe,4) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\���ʂɐ��l��������\��
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	MSG_WORK *cp_msg	�����Ǘ�
 *	@param	strID							����ID
 *	@param	number						���l
 *	@param	buff_id						���[�h�Z�b�g�o�^�o�b�t�@
 *	@param	x									�J�n�ʒuX
 *	@param	y									�J�n�ʒuY
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id )
{
	const GFL_MSGDATA* cp_msgdata;
	WORDSET *p_wordset;
	
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//���W���[���擾
	p_wordset		= MSG_GetWordSet( cp_msg );
	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );

	//���l�����[�h�Z�b�g�ɓo�^
	WORDSET_RegisterNumber(	p_wordset, buff_id, number, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

	//���̕�����ɐ��l��K�p
	{	
		STRBUF	*p_strbuf;
		p_strbuf	= GFL_MSG_CreateString( cp_msgdata, strID );
		WORDSET_ExpandStr( p_wordset, p_wk->p_strbuf, p_strbuf );
		GFL_STR_DeleteBuffer( p_strbuf );
	}

	//�\��
	{	
		PRINT_QUE*	p_que;
		GFL_FONT*		p_font;	
		u16 x, y;

		p_que		= MSG_GetPrintQue( cp_msg );
		p_font	= MSG_GetFont( cp_msg );

		x	= 0;
		y	= 0;

		PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\���ʂɐ��l��������\��
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	MSG_WORK *cp_msg	�����Ǘ�
 *	@param	strID							����ID
 *	@param	cp_status					�v���C���[�̏��
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintBothNameCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, const COMPATIBLE_STATUS *cp_my, const COMPATIBLE_STATUS *cp_you, HEAPID heapID )
{	
	const GFL_MSGDATA* cp_msgdata;
	WORDSET *p_wordset;
	u16 x, y;
	
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

	//���W���[���擾
	p_wordset		= MSG_GetWordSet( cp_msg );
	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );

  //���[�h�Z�b�g�ɓo�^
  { 
    WORDSET_RegisterPlayerName( p_wordset, 0, (const MYSTATUS*)cp_you->my_status );
  }
  //���[�h�Z�b�g�ɓo�^
  { 
    WORDSET_RegisterPlayerName( p_wordset, 1, (const MYSTATUS*)cp_my->my_status );
  }

	//���̕�����ɐ��l��K�p
	{	
		STRBUF	*p_strbuf;
		p_strbuf	= GFL_MSG_CreateString( cp_msgdata, strID );
		WORDSET_ExpandStr( p_wordset, p_wk->p_strbuf, p_strbuf );
		GFL_STR_DeleteBuffer( p_strbuf );
	}

	//�\��
	{	
		PRINT_QUE*	p_que;
		GFL_FONT*		p_font;	
		p_que		= MSG_GetPrintQue( cp_msg );
		p_font	= MSG_GetFont( cp_msg );

		//�Z���^�[�ʒu�v�Z
#if 0
		x	= GFL_BMPWIN_GetSizeX( p_wk->p_bmpwin )*4;
		y	= GFL_BMPWIN_GetSizeY( p_wk->p_bmpwin )*4;
		x	-= PRINTSYS_GetStrWidth( p_wk->p_strbuf, p_font, 0 )/2;
		y	-= PRINTSYS_GetStrHeight( p_wk->p_strbuf, p_font )/2;
#else
		x	= 0;
		y	= 0;
#endif
		PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	��ʃN���A
 *
 *	@param	MSGWND_WORK* p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Clear( MSGWND_WORK* p_wk )
{	
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	
	GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
}


//=============================================================================
/**
 *				SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�V�[�P���X�ύX
 *
 *	@param	RESULT_MAIN_WORK *p_wk	���[�N
 *	@param	seq_function					�V�[�P���X�֐�
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Change( RESULT_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�I��
 *
 *	@param	RESULT_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( RESULT_MAIN_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}

//=============================================================================
/**
 *			SEQ_FUNCTION
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���J�n����
 *
 *	@param	RESULT_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_StartGame( RESULT_MAIN_WORK *p_wk, u16 *p_seq )
{	
	IRC_COMPATIBLE_SAVEDATA *p_sv;
	u8 score				= p_wk->p_param->score;
	
	if( p_wk->p_param->is_only_play )
	{	

		//BACKOBJ_StartGather( &p_wk->backobj[BACKOBJ_SYS_MAIN], FALSE );
		SEQ_Change( p_wk, SEQFUNC_DecideScore );
		return;
	}

	if( p_wk->p_param->p_you_status )
	{	
    COMPATIBLE_STATUS *p_you;
    MYSTATUS  *p_mystatus;

		p_sv	= IRC_COMPATIBLE_SV_GetSavedata( SaveControl_GetPointer() );
		p_you	= p_wk->p_param->p_you_status;
    p_mystatus  = (MYSTATUS*)p_you->my_status;

		//�Z�[�u����
		IRC_COMPATIBLE_SV_AddRanking(
        p_sv,
        MyStatus_GetMyName( p_mystatus ),
        score, 
        MyStatus_GetMySex( p_mystatus ) == PTL_SEX_MALE, 
        p_you->barth_month, 
        p_you->barth_day, 
        MyStatus_GetID( p_mystatus ) );
	}

	//BACKOBJ_StartGather( &p_wk->backobj[BACKOBJ_SYS_MAIN], FALSE );
	SEQ_Change( p_wk, SEQFUNC_DecideScore );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�R�A����
 *
 *	@param	RESULT_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq									�V�[�P���X
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DecideScore( RESULT_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum
	{	
#if 0
		SEQ_START_GATHER,	//�W�܂�J�n
		SEQ_WAIT_GATHER,	//�W�܂蒆
		SEQ_START_FLASHIN,//�t���b�V������J�n
		SEQ_WAIT_FLASHIN,	//�t���b�V�����蒆
		SEQ_START_FLASHOUT,//�t���b�V���o�J�n
		SEQ_WAIT_FLASHOUT,	//�t���b�V���o��
#endif
    SEQ_INIT,
		SEQ_START_SCALE,	//�n�[�g�g��J�n
		SEQ_WAIT_SCALE,		//�n�[�g�g�咆
		SEQ_MAIN,
		SEQ_END_MSG,
    SEQ_WAIT,
		SEQ_END_WAIT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
#if 0
	case SEQ_START_GATHER:
		if( p_wk->cnt++ > 200 )
		{	
			p_wk->cnt	= 0;
			BACKOBJ_StartGather( &p_wk->backobj[BACKOBJ_SYS_MAIN], FALSE );
			*p_seq	= SEQ_WAIT_GATHER;
		}
		break;

	case SEQ_WAIT_GATHER:
		if( BACKOBJ_IsEnd( &p_wk->backobj[BACKOBJ_SYS_MAIN] ) )
		{
			*p_seq	= SEQ_START_FLASHIN;
		}
		break;


	case SEQ_START_FLASHIN://�t���b�V������J�n
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, -2 );
		*p_seq	= SEQ_WAIT_FLASHIN;
		break;

	case SEQ_WAIT_FLASHIN:	//�t���b�V�����蒆
		if( !GFL_FADE_CheckFade() )
		{	
			GFL_CLWK*	p_clwk;
			//�n�[�g�̌�������
			p_clwk	= GRAPHIC_GetClwk( &p_wk->grp, CLWKID_ORIGIN );
			GFL_CLACT_WK_SetDrawEnable( p_clwk, FALSE );
			*p_seq	= SEQ_START_FLASHOUT;
		}
		break;

	case SEQ_START_FLASHOUT://�t���b�V���o�J�n
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, -2 );
		*p_seq	= SEQ_WAIT_FLASHOUT;
		break;

	case SEQ_WAIT_FLASHOUT:	//�t���b�V���o��
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_START_SCALE;
		}
		break;
#endif
  case SEQ_INIT:
    if( p_wk->cnt++ > 30 )
    { 
      p_wk->cnt = 0;
      *p_seq  = SEQ_START_SCALE;
    }
    break;

	case SEQ_START_SCALE:
		HEARTEFF_Start( &p_wk->heart );
		PMSND_PlaySE( IRCRESULT_SE_HEART_UP );
		*p_seq	= SEQ_WAIT_SCALE;
		break;

	case SEQ_WAIT_SCALE:
		if( HEARTEFF_IsEnd(&p_wk->heart) )
		{	
			PMSND_PlaySE( IRCRESULT_SE_HEART_ON );
			OBJNUMBER_Start( &p_wk->number, 0 );
			OBJNUMBER_Start( &p_wk->number, 1 );
			OBJNUMBER_Start( &p_wk->number, 2 );
			*p_seq	= SEQ_MAIN;
		}
		break;

	case SEQ_MAIN:
		if( OBJNUMBER_IsEnd( &p_wk->number, 0 ) 
		 && OBJNUMBER_IsEnd( &p_wk->number, 1 )
		 && OBJNUMBER_IsEnd( &p_wk->number, 2 ) )
		{	
			*p_seq	= SEQ_END_MSG;
		}		
		break;
		
	case SEQ_END_MSG:
		MSGWND_PrintNumber( &p_wk->msgwnd[MSGWNDID_SUB], &p_wk->msg, RESULT_STR_001,
				p_wk->p_param->score, 2 );
    *p_seq  = SEQ_WAIT;
    break;

  case SEQ_WAIT:
    if( p_wk->cnt++ > 50 )
    { 
      p_wk->cnt = 0;
      switch( UTIL_GetScoreRank(p_wk->p_param->score,NULL) )
      { 
      case SCORE_RANK_BEST:
        PMSND_PlayBGM( SEQ_ME_AISHOU_H );
        break;
      case SCORE_RANK_HIGHT:
        PMSND_PlayBGM( SEQ_ME_AISHOU_M );
        break;
      case SCORE_RANK_LOW_00:
      case SCORE_RANK_LOW_01:
      case SCORE_RANK_LOW_02:
        PMSND_PlayBGM( SEQ_ME_AISHOU_L );
        break;
      case SCORE_RANK_WORST:
        PMSND_PlayBGM( SEQ_ME_AISHOU_L );
        break;
      }
      *p_seq		= SEQ_END_WAIT;
    }
		break;

	case SEQ_END_WAIT:
    if( !PMSND_CheckPlayBGM() )
    { 
      if( p_wk->cnt++ > 30 )
      { 
        p_wk->cnt = 0;
        *p_seq  = SEQ_END;
      }
    }
		break;

  case SEQ_END:
    if( GFL_UI_TP_GetTrg() )
    { 
      SEQ_End( p_wk );
    }
    break;
	}

	if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_RETURN )
	{	
		PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );	
		SEQ_End( p_wk );
	}

	OBJNUMBER_Main( &p_wk->number );
}

//=============================================================================
/**
 *			OBJ_NUMBER
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	������
 *
 *	@param	OBJNUMBER_WORK *p_wk	���[�N
 *	@param	*p_grp								�`�惏�[�N
 *	@param	number								����
 *
 */
//-----------------------------------------------------------------------------
static void OBJNUMBER_Init( OBJNUMBER_WORK *p_wk, const GRAPHIC_WORK *cp_grp, int number )
{	
	GFL_STD_MemClear( p_wk, sizeof(OBJNUMBER_WORK) );
	{	
		int i;
		GFL_CLACTPOS	clpos;
		GFL_CLSCALE		scale;

		scale.x	= SCALE_MIN;
		scale.y	= SCALE_MIN;
		//0��1���P��2���E�E�E
		for( i = 0; i < CLWK_NUMBER_MAX; i++ )
		{	
			p_wk->p_clwk[i]	= GRAPHIC_GetClwk( cp_grp, CLWKID_NUMBER_TOP + i );

			//100�����Ȃ�2���Ȃ̂ŃZ���^�����O
			if( number < 100 )
			{	
				//�Z���^�����O
				clpos.x	= 256/2 + 36 * (i-1) - 36/2;
			}
      else if( 0 <= number && number < 10 )
      { 
        //�P��
        //�Z���^�����O
				clpos.x	= 256/2 + 36 * (i-1) - 36;
      }
			else
			{
				//�ʏ폈��
				clpos.x	= 256/2 + 36 * (i-1);
			}
			clpos.y	= 92 - 8;
			GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, 0 );

#ifdef OBJNUMBER_SCALE
			GFL_CLACT_WK_SetAffineParam( p_wk->p_clwk[i], NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE );
			GFL_CLACT_WK_SetScale( p_wk->p_clwk[i],	&scale );
#endif
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );


			p_wk->sync[i]			= 0;
			p_wk->sync_max[i]	= NUMBER_SCALE_SYNC;
			p_wk->scale_add[i]	= (FX32_ONE - SCALE_MIN) / p_wk->sync_max[i];

		}

		ObjNumber_SetNumber( p_wk, number );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���\���J�n
 *
 *	@param	OBJNUMBER_WORK *p_wk	���[�N
 *	@param	number							�\������ԍ�
 *	@param	fig									��	1�`3
 *
 */
//-----------------------------------------------------------------------------
static void OBJNUMBER_Start( OBJNUMBER_WORK *p_wk, int fig )
{
	GF_ASSERT( fig < CLWK_NUMBER_MAX );
	if( p_wk->is_end[ fig ] == FALSE )
	{	
		//3�̌���0�̂Ƃ��͂��Ȃ�
		if( fig == 0 && p_wk->number[CLWK_NUMBER_MAX -1 - fig] == 0 )
		{	
			p_wk->is_end[fig]	= TRUE;
		}
		else if( (fig == 0 && p_wk->number[CLWK_NUMBER_MAX -1 - fig] == 0)
        && (fig == 1 && p_wk->number[CLWK_NUMBER_MAX -1 - fig] == 0) )
		{	
      //�R�̌����O�łQ�̌����O�̂Ƃ������Ȃ�
			p_wk->is_end[fig]	= TRUE;
		}
		else
		{	
#ifdef OBJNUMBER_SCALE
			p_wk->start[ fig ]	= TRUE;
#else
			p_wk->is_end[fig]	= TRUE;
#endif
			NAGI_Printf( "start! ID%d\n", fig );
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[ fig ], TRUE );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C��
 *
 *	@param	OBJNUMBER_WORK *p_wk	���[�N
 *	@param	number							�\������ԍ�
 *	@param	sync								�V���N
 *
 *	@return	TRUE�����I��
 *	@return	FALSE������
 */
//-----------------------------------------------------------------------------
static BOOL OBJNUMBER_Main( OBJNUMBER_WORK *p_wk )
{	
#ifdef OBJNUMBER_SCALE
	int i;
	GFL_CLSCALE		scale;
	for( i = 0; i < CLWK_NUMBER_MAX; i++ )
	{	
		if( p_wk->start[i] )
		{	
			GFL_CLACT_WK_GetScale( p_wk->p_clwk[i],	&scale );
			scale.x	+= p_wk->scale_add[i];
			scale.y	+= p_wk->scale_add[i];
			

			if( p_wk->sync[i]++ > p_wk->sync_max[i] )
			{	
				scale.x	= FX32_ONE;
				scale.y	= FX32_ONE;
				p_wk->start[i]	= FALSE;
				p_wk->is_end[i]	= TRUE;
			}

			GFL_CLACT_WK_SetScale( p_wk->p_clwk[i],	&scale );
		}
	}
#endif
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�����m
 *
 *	@param	const OBJNUMBER_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�ŏI��	FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL OBJNUMBER_IsEnd( const OBJNUMBER_WORK *cp_wk, u16 fig )
{	
	GF_ASSERT( fig < CLWK_NUMBER_MAX );
	return cp_wk->is_end[ fig ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	���̐������擾
 *
 *	@param	const OBJNUMBER_WORK *cp_wk	���[�N
 *	@param	fig		���[�P
 *
 *	@return	���̐���
 */
//-----------------------------------------------------------------------------
static u8 OBJNUMBER_GetFig( const OBJNUMBER_WORK *cp_wk, int fig )
{	
	GF_ASSERT( fig < CLWK_NUMBER_MAX );
	//NAGI_Printf( "�� num%d keta%d\n",  cp_wk->number[ fig ], fig );
	return cp_wk->number[ fig ];
}
//----------------------------------------------------------------------------
/**
 *	@brief	���̈ʒuX���擾
 *
 *	@param	const OBJNUMBER_WORK *cp_wk	���[�N
 *	@param	fig		���[�P
 *
 *	@return	���̈ʒuX
 */
//-----------------------------------------------------------------------------
static s16 OBJNUMBER_GetFigPosX( const OBJNUMBER_WORK *cp_wk, int fig )
{	
	GFL_CLACTPOS	pos;

	GF_ASSERT( fig < CLWK_NUMBER_MAX );


	GFL_CLACT_WK_GetPos( cp_wk->p_clwk[ fig ], &pos, 0 );
	//NAGI_Printf( "�ʒu x%d keta%d\n",  pos.x, fig );
	return pos.x;
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�ɔԍ����Z�b�g
 *
 *	@param	OBJNUMBER_WORK *p_wk	���[�N
 *	@param	number							�ԍ�
 *
 */
//-----------------------------------------------------------------------------
static void ObjNumber_SetNumber( OBJNUMBER_WORK *p_wk, int number )
{	
	static const u32 sc_num_table[] = 
	{
		1,
		10,
		100,
		1000,
	};
	u8 fig;
	u8 n;
	int i;

	//���������ׂ�
	n		= number;
	fig	= 0;
	if( n == 0 )
	{	
		fig	= 1;
	}
	else
	{	
		while( 1 )
		{
			if( n == 0 )
			{	
				break;
			}
			n	/= 10;
			fig++;
		}
	}
	

	//��[����
	for( i = 0; i < CLWK_NUMBER_MAX; i++ )
	{
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[i], 0 );
	}

	//OBJ�̃A�j�������̐��l�ɍ��킹��
	for( i = 0; i < /*fig*/CLWK_NUMBER_MAX; i++ )
	{
		p_wk->number[i]	= number % sc_num_table[ i+1 ] / sc_num_table[ i ];
		//+1�̓Z���̏���
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[CLWK_NUMBER_MAX-1-i], p_wk->number[i] + 1 );
	}
}
//=============================================================================
/**
 *			SCALEBG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�g��k��BG	������
 *
 *	@param	HEARTEFF_WORK *p_wk	���[�N
 *	@param	frm									BG�t���[��
 *	@param	min									�ŏ�
 *	@param	max									�ő�
 *	@param	heapID							�q�[�vID
 */
//-----------------------------------------------------------------------------
static void HEARTEFF_Init( HEARTEFF_WORK *p_wk, u16 frm, u8 score, HEAPID heapID )
{	

	GFL_STD_MemClear( p_wk, sizeof(HEARTEFF_WORK) );
	p_wk->frm	  = frm;

  p_wk->rank = UTIL_GetScoreRank( score, &p_wk->is_lie );
  if( p_wk->is_lie )
  { 
    p_wk->rank++;
  }

	{	
		ARCHANDLE	*p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCCOMPATIBLE, heapID );
	
    //�ǂݍ���
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_result_bg_03_NCGR,
        sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_HEART], 0, 0, FALSE, heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_result_bg_03_NSCR,
        sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_HEART], 0, 0, FALSE, heapID );	
	
		GFL_ARC_CloseDataHandle( p_handle );
	}

  //�p���b�g�t�F�[�h����F��ۑ����Ă���
  { 
    void *p_buff;
    NNSG2dPaletteData *p_plt;
    const u16 *cp_plt_adrs;

    //���Ƃ̃p���b�g����F����ۑ�
    p_buff  = GFL_ARC_UTIL_LoadPalette( ARCID_IRCCOMPATIBLE, NARC_irccompatible_gra_result_bg_03_NCLR, &p_plt, heapID );
    cp_plt_adrs = p_plt->pRawData;
    GFL_STD_MemCopy( cp_plt_adrs, p_wk->plt_original, sizeof(u16) * 0x10 );
    GFL_STD_MemCopy( (u8*)cp_plt_adrs + RESULT_BG_PAL_M_08 * 0x20, p_wk->plt_original, sizeof(u16) * 0x10 );

    //�p���b�g�j��
    GFL_HEAP_FreeMemory( p_buff );
  }

  //�F�ȊO�𔒂��h��Ԃ�
  { 
    int i;

    GFL_STD_MemCopy( p_wk->plt_original, p_wk->plt_buff, 0x10 * 2 );
    for( i = 0; i < 0xE; i++ )
    { 
      p_wk->plt_buff[i] = GX_RGB( 31, 31, 31 );
    }
  }

  //�]��
  { 
    int i;
    for( i = 0; i < 0x10; i++ )
    { 
      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN, RESULT_BG_PAL_M_08 * 0x20 + i * 2, &p_wk->plt_buff[i], sizeof(u16) );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g��k��BG	�j��
 *
 *	@param	HEARTEFF_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void HEARTEFF_Exit( HEARTEFF_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(HEARTEFF_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g��k��BG	���C������
 *
 *	@param	HEARTEFF_WORK *p_wk	���[�N 
 *
 */
//-----------------------------------------------------------------------------
static void HEARTEFF_Main( HEARTEFF_WORK *p_wk )
{	
  static const u8 sc_color_tbl[SCORE_RANK_MAX][2]  =
  { 
    { 0,0 }, 
    { 0xD, 0xC }, 
    { 0xB, 0xA }, 
    { 0x9, 0x8 }, 
    { 0x7, 0x6 }, 
    { 0x3, 0x5 }, 
  };

  enum
  { 
    SEQ_NORMAL,
    SEQ_LIE_WAIT,
    SEQ_LIE,

    ONE_SYNC  = 40,
  };

  if( p_wk->is_start )
  { 
    int i;
    u8  plt;

    switch( p_wk->seq )
    { 
    case SEQ_NORMAL:
      //�ŏI�I���`�F�b�N
      if( p_wk->now_color_idx > p_wk->rank )
      { 
        //�R�𒅂��ꍇ
        if( p_wk->is_lie )
        { 
          p_wk->seq = SEQ_LIE_WAIT;
        }
        else
        { 
          p_wk->is_end    = TRUE;
          p_wk->is_start  = FALSE;
        }
      }
      else
      { 
        //�F���A�b�v
        for( i = 0; i < 2; i++ )
        {     
          plt = sc_color_tbl[ p_wk->now_color_idx ][i];
          HeartEff_MainPltFade( &p_wk->plt_buff[ plt ], p_wk->cnt, ONE_SYNC, RESULT_BG_PAL_M_08, plt, GX_RGB( 31, 31, 31 ), p_wk->plt_original[ plt ] );
      }

        //�ʃ`�F�b�N
        if( p_wk->cnt++ >= ONE_SYNC )
        { 
          p_wk->cnt = 0;
          p_wk->now_color_idx++;
        }
      }
      break;

    case SEQ_LIE_WAIT:
      if( p_wk->cnt++ >= (ONE_SYNC+20) )
      { 
        p_wk->cnt = 0;
        p_wk->seq = SEQ_LIE;
      }
      break;

    case SEQ_LIE:
      //�F���A�b�v
      for( i = 0; i < 2; i++ )
      {     
        plt = sc_color_tbl[ p_wk->now_color_idx-1 ][i];
        HeartEff_MainPltFade( &p_wk->plt_buff[ plt ], p_wk->cnt, ONE_SYNC, RESULT_BG_PAL_M_08, plt, p_wk->plt_original[ plt ],  GX_RGB( 31, 31, 31 ) );
      }
      //�ʃ`�F�b�N
      if( p_wk->cnt++ >= ONE_SYNC )
      { 
        p_wk->cnt = 0;
        p_wk->is_end    = TRUE;
        p_wk->is_start  = FALSE;
      }
      break;
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g��k��BG	�g�k�J�n
 *
 *	@param	HEARTEFF_WORK *p_wk	���[�N
 *	@param	rate	0�Ȃ��min�̂܂�FX32_ONE�Ȃ��max
 *	@param	sync	�V���N
 */
//-----------------------------------------------------------------------------
static void HEARTEFF_Start( HEARTEFF_WORK *p_wk )
{	
  p_wk->is_start      = TRUE;
  p_wk->is_end        = FALSE;
  p_wk->now_color_idx = 1;
  p_wk->seq           = 0;
  p_wk->cnt           = 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g��k��BG�@�I���`�F�b�N
 *
 *	@param	const HEARTEFF_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL HEARTEFF_IsEnd( const HEARTEFF_WORK *cp_wk )
{	
		return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �]������
 *
 *	@param	u16 *p_buff �F�΂��ӂ�
 *	@param	*p_cnt    �J�E���g�o�b�t�@
 *	@param	cnt_max   �J�E���g�ő�
 *	@param	plt_num   �]���ʒu
 *	@param	plt_col   �]���ʒu��
 *	@param	start     �J�n�̐F
 *	@param	end       �ړI�̐F
 */
//-----------------------------------------------------------------------------
static void HeartEff_MainPltFade( u16 *p_buff, u16 cnt, u16 cnt_max, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{ 
  {
    //1�`0�ɕϊ�
    const u8 start_r  = (start & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 start_g  = (start & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 start_b  = (start & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    const u8 end_r  = (end & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 end_g  = (end & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 end_b  = (end & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    const u8 r = start_r + (end_r-start_r) * cnt / cnt_max;
    const u8 g = start_g + (end_g-start_g) * cnt / cnt_max;
    const u8 b = start_b + (end_b-start_b) * cnt / cnt_max;

    BOOL ret;
    
    *p_buff = GX_RGB(r, g, b);
    
    ret = NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        plt_num * 32 + plt_col * 2 ,
                                        p_buff, 2 );

    GF_ASSERT( ret );
  }
}

//=============================================================================
/**
 *			BACKOBJ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	BACKOBJ_WORK *p_wk		���[�N
 *	@param	GRAPHIC_WORK *cp_grp	CLWK�󂯎��p�O���t�B�b�N
 *	@param	type									����^�C�v
 *	@param	color									�F
 *	@param	clwk_ofs							CLWK�I�t�Z�b�g���Ԃ���g���n�߂邩
 */
//-----------------------------------------------------------------------------
static void BACKOBJ_Init( BACKOBJ_WORK *p_wk, const GRAPHIC_WORK *cp_grp, BACKOBJ_MOVE_TYPE type, BACKOBJ_COLOR color, u32 clwk_ofs, int sf_type )
{	
	GFL_STD_MemClear( p_wk, sizeof(BACKOBJ_WORK) );
	p_wk->type	= type;
	p_wk->color	= color;
	p_wk->sync_now	= 0;
	p_wk->sync_max	= BACKOBJ_MOVE_SYNC;
	{	
		int i;
		GFL_CLWK *p_clwk;
		for( i = 0; i < BACKOBJ_CLWK_MAX; i++ )
		{	
			p_clwk	= GRAPHIC_GetClwk( cp_grp, clwk_ofs + i );
			BACKOBJ_ONE_Init( &p_wk->wk[i], p_clwk, color, sf_type );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�j��
 *
 *	@param	BACKOBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void BACKOBJ_Exit( BACKOBJ_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(BACKOBJ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 *
 *	@param	BACKOBJ_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BACKOBJ_Main( BACKOBJ_WORK *p_wk )
{	
	int i;
	u32 sync;
	u16 rot;
	GFL_POINT	start;
	GFL_POINT	end;

	if( p_wk->is_start )
	{	

		//�^�C�v�ʈړ�
		switch( p_wk->type )
		{	
		case BACKOBJ_MOVE_TYPE_RAIN:		//�J
			if( p_wk->sync_now++ > p_wk->sync_max )
			{	
				p_wk->sync_now	= 0;
				for( i = 0; i < BACKOBJ_CLWK_MAX; i++ )
				{	
					if( !BACKOBJ_ONE_IsMove( &p_wk->wk[i] ) )
					{	
						u8	up_or_down	= GFUser_GetPublicRand(2);

						start.x	= GFUser_GetPublicRand(256);
						start.y	= -36;
						end.x		= start.x;
						end.y		= 192 + 36;
						sync	= BACKOBJ_ONE_MOVE_SYNC_MIN + GFUser_GetPublicRand(BACKOBJ_ONE_MOVE_SYNC_DIF);
						if( up_or_down )
						{	
							BACKOBJ_ONE_Start( &p_wk->wk[i], &end, &start, BACKOBJ_COLOR_RED, sync );
						}
						else
						{	
							BACKOBJ_ONE_Start( &p_wk->wk[i], &start, &end, BACKOBJ_COLOR_RED, sync );
						}
						break;
					}
				}
			}
			break;

		case BACKOBJ_MOVE_TYPE_EMITER:	//���o
			/* BACKOBJ_StartEmit�ōs��  */
			break;

		case BACKOBJ_MOVE_TYPE_GATHER:	//�W��
			if( p_wk->sync_now++ > p_wk->sync_max )
			{	
				p_wk->sync_now	= 0;
				for( i = 0; i < BACKOBJ_CLWK_MAX; i++ )
				{	
					if( !BACKOBJ_ONE_IsMove( &p_wk->wk[i] ) )
					{	
						BACKOBJ_COLOR	color;
						u16 rot	= GFUser_GetPublicRand(0xFFFF);
						start.x	= (FX_SinIdx(rot) * 256) >> FX32_SHIFT;
						start.y	= (FX_CosIdx(rot) * 256) >> FX32_SHIFT;
						end.x		= 128;
						end.y		= 96;
						sync	= BACKOBJ_ONE_MOVE_SYNC_MIN + GFUser_GetPublicRand(BACKOBJ_ONE_MOVE_SYNC_DIF);

						{
							static const BACKOBJ_COLOR	sc_color_tbl[]	=
							{	
								BACKOBJ_COLOR_RED,
								BACKOBJ_COLOR_ORANGE,
								BACKOBJ_COLOR_YELLOW,
								BACKOBJ_COLOR_YEGREEN,
								BACKOBJ_COLOR_GREEN,
								BACKOBJ_COLOR_WATER,
								BACKOBJ_COLOR_BLUE,
							};
							color	= sc_color_tbl[ GFUser_GetPublicRand(NELEMS(sc_color_tbl)) ];
						}
						BACKOBJ_ONE_Start( &p_wk->wk[i], &start, &end, color, sync );
						break;
					}
				}
			}
			break;
		}

	}


	//���[�N����
	{	
		for( i = 0; i < BACKOBJ_CLWK_MAX; i++ )
		{	
			BACKOBJ_ONE_Main( &p_wk->wk[i] );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���o�J�n
 *
 *	@param	BACKOBJ_WORK *p_wk	���[�N
 *	@param	GFL_POINT *cp_pos		���o�J�n���W
 */
//-----------------------------------------------------------------------------
static void BACKOBJ_StartGather( BACKOBJ_WORK *p_wk, BOOL is_start )
{
	p_wk->is_start	= is_start;
}
//----------------------------------------------------------------------------
/**
 *	@brief	����OBJ�����I��������
 *
 *	@param	const BACKOBJ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�Ώ����I��	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL BACKOBJ_IsEnd( const BACKOBJ_WORK *cp_wk )
{	
	int i;

	for( i = 0; i < BACKOBJ_CLWK_MAX; i++ )
	{	
		if( BACKOBJ_ONE_IsMove( &cp_wk->wk[i] ) )
		{	
			return FALSE;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�R�A��]���t��
 *
 *	@param	u8 score  �X�R�A
 *
 *	@return �X�R�A�̕]��
 */
//-----------------------------------------------------------------------------
static SCORE_RANK UTIL_GetScoreRank( u8 score, BOOL *p_is_nearly )
{ 

  enum
  { 
    SCORE_RANK_BORDER_NONE    = 0xFFFF,

    SCORE_RANK_BORDER_BEST    = 100,
    SCORE_RANK_BORDER_HEIGHT  = 80,
    SCORE_RANK_BORDER_LOW_01  = 60,
    SCORE_RANK_BORDER_LOW_02  = 30,
    SCORE_RANK_BORDER_WORST   = 0,

    SCORE_RANK_NEARLY = 3
  };
  u16 border;
  SCORE_RANK  ret;


  if(score == SCORE_RANK_BORDER_BEST )
  { 
    border  = SCORE_RANK_BORDER_NONE;
    ret     = SCORE_RANK_BEST;
  }
  else if( SCORE_RANK_BORDER_HEIGHT <= score )
  { 
    border  = SCORE_RANK_BORDER_BEST;
    ret =  SCORE_RANK_HIGHT;
  }
  else if( SCORE_RANK_BORDER_LOW_01 <= score )
  { 
    border  = SCORE_RANK_BORDER_HEIGHT;
    ret =  SCORE_RANK_LOW_00;
  }
  else if( SCORE_RANK_BORDER_LOW_02 <= score )
  { 
    border  = SCORE_RANK_BORDER_LOW_01;
    ret =  SCORE_RANK_LOW_01;
  }
  else if( SCORE_RANK_BORDER_WORST < score )
  { 
    border  = SCORE_RANK_BORDER_LOW_02;
    ret =  SCORE_RANK_LOW_02;
  }
  else if( SCORE_RANK_BORDER_WORST == score )
  { 
    border  = SCORE_RANK_BORDER_WORST;
    ret =  SCORE_RANK_WORST;
  }
  else
  { 
    border  = SCORE_RANK_BORDER_BEST;
    GF_ASSERT(0);
    ret =  SCORE_RANK_LOW_02;
  }

  if( p_is_nearly )
  { 
    if( score + SCORE_RANK_NEARLY > border )
    { 
      *p_is_nearly = TRUE;
    }
    else
    { 
      *p_is_nearly = FALSE;
    }
  }

  return ret;
}

//=============================================================================
/**
 *		�w��OBJ1��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	������
 *
 *	@param	BACKOBJ_ONE *p_wk	���[�N
 *	@param	*p_clwk						�A�N�^�[
 *	@param	color							�F
 */
//-----------------------------------------------------------------------------
static void BACKOBJ_ONE_Init( BACKOBJ_ONE *p_wk, GFL_CLWK *p_clwk, BACKOBJ_COLOR color, int sf_type )
{	
	GFL_STD_MemClear( p_wk, sizeof(BACKOBJ_ONE) );
	p_wk->p_clwk	= p_clwk;
	p_wk->sf_type	= sf_type;

	GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk, color, CLWK_PLTTOFFS_MODE_PLTT_TOP );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 *
 *	@param	BACKOBJ_ONE *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BACKOBJ_ONE_Main( BACKOBJ_ONE *p_wk )
{	
	if( p_wk->is_req )
	{	
		GFL_CLACTPOS pos;
		VecFx32	v;
		VecFx32	v2;
		fx32 mag;
		VEC_Subtract( &p_wk->end, &p_wk->start, &v );
		mag	= VEC_Mag( &v );
		VEC_Normalize( &v, &v );
		mag = mag * p_wk->sync_now / p_wk->sync_max;
		VEC_MultAdd( mag, &v, &p_wk->start, &v2 );

		pos.x	= v2.x >> FX32_SHIFT;
		pos.y	= v2.y >> FX32_SHIFT;

		if( p_wk->sync_now++ > p_wk->sync_max )
		{	
			pos.x	= p_wk->end.x;
			pos.y	= p_wk->end.y;
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );

			p_wk->is_req	= FALSE;
		}

		GFL_CLACT_WK_SetPos( p_wk->p_clwk, &pos, p_wk->sf_type );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ړ��J�n
 *
 *	@param	BACKOBJ_ONE *p_wk		���[�N
 *	@param	GFL_POINT *cp_start	�J�n���W
 *	@param	GFL_POINT *cp_end		�I�����W
 *	@param	sync								�ړ��V���N
 */
//-----------------------------------------------------------------------------
static void BACKOBJ_ONE_Start( BACKOBJ_ONE *p_wk, const GFL_POINT *cp_start, const GFL_POINT *cp_end, BACKOBJ_COLOR color, u32 sync )
{	
	p_wk->is_req		= TRUE;
	VEC_Set( &p_wk->start, FX32_CONST(cp_start->x), FX32_CONST(cp_start->y), 0 );
	VEC_Set( &p_wk->end, FX32_CONST(cp_end->x), FX32_CONST(cp_end->y), 0 );
	p_wk->sync_now	= 0;
	p_wk->sync_max	= sync;
	GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
	GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk, color, CLWK_PLTTOFFS_MODE_PLTT_TOP );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���쒆�t���O�擾
 *
 *	@param	const BACKOBJ_ONE *cp_wk	���[�N
 *
 *	@return	TRUE�Ȃ�Γ��쒆	FALSE�Ȃ�Γ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BACKOBJ_ONE_IsMove( const BACKOBJ_ONE *cp_wk )
{	
	return cp_wk->is_req;
}

//=============================================================================
/**
 *				�ėp
 */
//=============================================================================
 
