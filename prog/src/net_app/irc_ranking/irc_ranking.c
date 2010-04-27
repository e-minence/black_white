//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_ranking.c
 *	@brief	�ԊO���~�j�Q�[�������L���O���
 *	@author	Toru=Nagihashi
 *	@data		2009.07.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//	constant
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	archive
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_irc_ranking.h"
#include "ircranking_gra.naix"
#include "font/font.naix"

//	print
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

//	module
#include "infowin/infowin.h"
#include "sound/pm_sndsys.h"
#include "net_app/irc_appbar.h"
#include "pokeicon/pokeicon.h"

//	common
#include "app/app_menu_common.h"

//sound
#include "../irc_compatible/irc_compatible_snd.h"

//	save
#include "savedata/irc_compatible_savedata.h"
#include "savedata/save_tbl.h"

//	mine
#include "net_app/irc_ranking.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�}�N��
//=====================================
#ifdef PM_DEBUG
//#define DEBUG_RANKING_TICK
#endif

//�^�C��
#ifdef DEBUG_RANKING_TICK

static OSTick s_DEBUG_TICK_DRAW_start;
#define DEBUG_TICK_DRAW_Print		NAGI_Printf("line[%d] time=%dmicro\n",__LINE__,OS_TicksToMicroSeconds(OS_GetTick() - s_DEBUG_TICK_DRAW_start) )
#define DEBUG_TICK_DRAW_Start		s_DEBUG_TICK_DRAW_start = OS_GetTick()

#else
		
#define DEBUG_TICK_DRAW_Print
#define DEBUG_TICK_DRAW_Start

#endif

//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	// ���C�����BG
	RANKING_BG_PAL_M_00 = 0,// 
	RANKING_BG_PAL_M_01,		// 
	RANKING_BG_PAL_M_02,		//
	RANKING_BG_PAL_M_03,		// 
	RANKING_BG_PAL_M_04,		// 
	RANKING_BG_PAL_M_05,		// 
	RANKING_BG_PAL_M_06,		// 
	RANKING_BG_PAL_M_07,		// 
	RANKING_BG_PAL_M_08,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_09,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_10,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_11,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_12,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_13,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_14,		// �t�H���g
	RANKING_BG_PAL_M_15,		//


	// �T�u���BG
	RANKING_BG_PAL_S_00 = 0,	// 
	RANKING_BG_PAL_S_01,		//  
	RANKING_BG_PAL_S_02,		//  
	RANKING_BG_PAL_S_03,		//  
	RANKING_BG_PAL_S_04,		//  
	RANKING_BG_PAL_S_05,		//  
	RANKING_BG_PAL_S_06,		//  
	RANKING_BG_PAL_S_07,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_08,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_09,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_10,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_11,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_12,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_13,		// �A�v���o�[
	RANKING_BG_PAL_S_14,		// �t�H���g
	RANKING_BG_PAL_S_15,		// �g�p���ĂȂ�

	// ���C�����OBJ
	RANKING_OBJ_PAL_POKEICON_M = 0,// 

	// �T�u���BG
	RANKING_OBJ_PAL_POKEICON_S = 0,// 
};

//-------------------------------------
///	SCROLL
//=====================================

#define SCROLL_BAR_WIDTH		  (32)
#define SCROLL_BAR_HEIGHT			(3)

#define SCROLL_BAR_FONT_WIDTH (25)

#define SCROLL_BAR_FONT_RANK_X		(3)
#define SCROLL_BAR_FONT_PLAYER_X	(6)
#define SCROLL_BAR_FONT_SCORE_X		(16)
#define SCROLL_BAR_FONT_COUNT_X		(22)

#define SCROLL_BAR_ICON_X		(29*8)
#define SCROLL_BAR_ICON_Y   (8)

#define SCROLL_INIT_POS_Y   (SCROLL_BAR_HEIGHT*GFL_BG_1CHRDOTSIZ) //���ւ̂��߂ɍŏ��Ɉʒu�����炵�Ă���
#define SCROLL_BAR_START_RANK_M   (-1)  //0�L�����ڂ��牽�Ԗڂ̃����N��\����邩
#define SCROLL_BAR_START_RANK_S   (5)   //


#define SCROLL_BAR_VISIBLE_DISPLAY  (13)    //����ʂɕ`�悳���o�[�̐�


#define SCROLL_WRITE_POS_START_M	(8)	  //�ǂ̈ʒu���璣��n�߂邩
#define SCROLL_WRITE_POS_END_M	  (24+3)	//�ǂ̈ʒu�܂Œ��邩
#define SCROLL_WRITE_POS_START_S	(1)	  //�ǂ̈ʒu���璣��n�߂邩
#define SCROLL_WRITE_POS_END_S	  (25+3)	//�ǂ̈ʒu�܂Œ��邩

#define SCROLL_FONT_Y_OFS	(1+4)	//����Y�ʒu

#define SCROLL_NEWRANK_NULL	(0xFFFF)


typedef enum
{
  RANKBAR_DISPLAY_M,
  RANKBAR_DISPLAY_S,
  RANKBAR_DISPLAY_NUM,
} RANKBAR_DISPLAY;

//-------------------------------------
///		BMPWIN
//=====================================
enum
{	
	BMPWIN_ID_TITLE,
	BMPWIN_ID_RANK,
	BMPWIN_ID_PLAYER,
	BMPWIN_ID_SCORE,
	BMPWIN_ID_COUNT,
	BMPWIN_ID_MAX,
} ;

#define BMPWIN_TITLE_X				(3)
#define BMPWIN_TITLE_Y				(1)
#define BMPWIN_TITLE_W				(26)
#define BMPWIN_TITLE_H				(2)

#define BMPWIN_RANK_X					(1)
#define BMPWIN_RANK_Y					(5)
#define BMPWIN_RANK_W					(6)
#define BMPWIN_RANK_H					(2)

#define BMPWIN_PLAYER_X				(8)
#define BMPWIN_PLAYER_Y				(5)
#define BMPWIN_PLAYER_W				(6)
#define BMPWIN_PLAYER_H				(2)

#define BMPWIN_SCORE_X				(15)
#define BMPWIN_SCORE_Y				(5)
#define BMPWIN_SCORE_W				(6)
#define BMPWIN_SCORE_H				(2)

#define BMPWIN_COUNT_X				(24)
#define BMPWIN_COUNT_Y				(5)
#define BMPWIN_COUNT_W				(6)
#define BMPWIN_COUNT_H				(2)

#define BMPWIN_BAR_X					(0)
#define BMPWIN_BAR_Y					(21)
#define BMPWIN_BAR_W					(32)
#define BMPWIN_BAR_H					(3)


//-------------------------------------
///	UI
//=====================================
#define UI_FLIK_RELEASE_SYNC	(1)		//�͂����Ƃ��A�����Ă��牽�V���N�L����

//-------------------------------------
///	ACLR
//=====================================
#define	ACLR_SCROLL_MOVE_MIN					(-FX32_CONST(8))
#define	ACLR_SCROLL_MOVE_MAX					(FX32_CONST(8))

#define ACLR_SCROLL_DISTANCE_MIN			(FX32_CONST(1))	//MIN�ȉ����Ɣ������Ȃ�
#define ACLR_SCROLL_DISTANCE_MAX			(FX32_CONST(60))	//�ȏ�͐؎̂�
#define ACLR_SCROLL_DISTANCE_DIF			(ACLR_SCROLL_DISTANCE_MAX-ACLR_SCROLL_DISTANCE_MIN)

#define ACLR_SCROLL_SYNC_MIN					(FX32_CONST(0))		//�ȉ��͐؂�̂�
#define ACLR_SCROLL_SYNC_MAX					(FX32_CONST(13))	//MAX�ȏゾ�Ɣ������Ȃ�
#define ACLR_SCROLL_SYNC_DIF					(ACLR_SCROLL_SYNC_MAX-ACLR_SCROLL_SYNC_MIN)

#define ACLR_SCROLL_ACLR_MIN					(FX32_CONST(0))
#define ACLR_SCROLL_ACLR_MAX					(FX32_CONST(40))
#define ACLR_SCROLL_ACLR_DIF					(ACLR_SCROLL_ACLR_MAX-ACLR_SCROLL_ACLR_MIN)

#define ACLR_SCROLL_DEC_RATE					(FX32_CONST(0.05))	//������

#define ACLR_SCROLL_KEY_MIN						(FX32_CONST(20))	
#define ACLR_SCROLL_KEY_MAX						(FX32_CONST(60))
#define ACLR_SCROLL_KEY_DIF						(ACLR_SCROLL_KEY_MAX-ACLR_SCROLL_KEY_MIN)

//�L�[�ł̈ړ��p���[
#define ACLR_SCROLL_KEY_MOVE_INIT_DISTANCE	(ACLR_SCROLL_DISTANCE_MIN+FX32_ONE)
#define ACLR_SCROLL_KEY_MOVE_INIT_SYNC			((ACLR_SCROLL_SYNC_MAX-FX32_ONE)>>FX32_SHIFT)
#define ACLR_SCROLL_KEY_MOVE_ACLR_SYNC			((ACLR_SCROLL_SYNC_MAX-FX32_ONE)>>FX32_SHIFT)

#define	ACLR_SCROLL_NEWRANK_OFS				(192)

//-------------------------------------
///	DIV
//=====================================
#define DRAG_MOVE_DIV_ADD_RATE				(FX32_CONST(0.04f))
#define DRAG_MOVE_DIV_MAX							(20)
#define DRAG_MOVE_INIT_DISTANCE				(1)


//-------------------------------------
///	OBJ�o�^ID
//=====================================
enum {
	OBJREGID_TOUCH_PLT,
	OBJREGID_TOUCH_CHR,
	OBJREGID_TOUCH_CEL,

	OBJREGID_MAX
};
//-------------------------------------
///	CLWK�擾
//=====================================
typedef enum{	
	CLWKID_TOUCH,
	
	CLWKID_MAX
}CLWKID;




//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�����L���O�f�[�^
//=====================================
typedef struct 
{
	STRBUF	*p_name;
	u32			score			:7;
	u32			play_cnt	:10;
	u32			plt				:8;
	u32			rank			:7;
  u8      mons_no;
  u8      form_no;
  u8      sex;
  u8      egg;
} RANKING_ONE_DATA;
typedef struct
{ 
  u32               rank_num;
  RANKING_ONE_DATA  data[0];
} RANKING_DATA;
//-------------------------------------
///	BG���[�N
//=====================================
typedef struct 
{
	ARCHANDLE		*p_handle;
	GFL_ARCUTIL_TRANSINFO	bar_char_m;
	GFL_ARCUTIL_TRANSINFO	bar_char_s;
	GFL_BMPWIN	*p_bmpwin[BMPWIN_ID_MAX];
} GRAPHIC_BG_WORK;
//-------------------------------------
///	OBJ���[�N
//=====================================
typedef struct 
{
	GFL_CLUNIT *p_clunit;
	u32					reg_id[OBJREGID_MAX];
	GFL_CLWK	 *p_clwk[CLWKID_MAX];
} GRAPHIC_OBJ_WORK;
//-------------------------------------
///	SCROLL���[�N
//====================================
typedef struct 
{
  u16   rank_screen_main[SCROLL_BAR_HEIGHT*SCROLL_BAR_WIDTH];
  u16   rank_screen_sub[SCROLL_BAR_HEIGHT*SCROLL_BAR_WIDTH];
} BAR_BUFF;

//�����N�P�̃��[�N
typedef struct 
{
  const RANKING_ONE_DATA *cp_data;
  GFL_BMP_DATA  *p_bmp;     //�t�H���g�f�[�^  �����o�b�t�@
  GFL_BMPWIN    *p_bmpwin[ RANKBAR_DISPLAY_NUM ];  //�]����
  GFL_CLWK      *p_icon[ CLSYS_DEFREND_NUM ];    //�|�P�����A�C�R��
  u32           chr_idx;    //�|�P�A�C�R�����\�[�X
  u32           cel_idx;    //�|�P�A�C�R�����\�[�X
  BAR_BUFF      *p_buff;
} RANKBAR_WORK;

//�X�N���[�����[�N
typedef struct 
{
	u8	bar_frm_m;			//���ʃo�[�\����
	u8	font_frm_m;			//���ʃt�H���g�\����
	u8	bar_frm_s;			//����ʃo�[�\����
	u8	font_frm_s;			//����ʃo�[�\����
	const GRAPHIC_BG_WORK	*cp_bg;		//�o�[�p�L�����󂯎��̂���
	const RANKING_DATA		*cp_data;	//�����L���O�̃f�[�^
  BAR_BUFF      barbuff;        //�o�[�̃X�N���[���f�[�^
  u32           icon_plt_idx;   //�ۂ��A�C�R���̃p���b�g

  s32           y;  //�X�N���[���ʒu
  s32           dir;//�ړ�����

  BOOL          is_update;

	GFL_FONT			*p_font;
  GFL_MSGDATA		*p_msg;
	WORDSET				*p_wordset;

  RANKBAR_WORK  rankbar[ IRC_COMPATIBLE_SV_RANKING_MAX ];

  GFL_TCB	      *p_vblank_task;
} SCROLL_WORK;
//-------------------------------------
///	�����x���[�N
//=====================================
typedef struct 
{
	fx32	distance_rate;
	fx32	sync_rate;
	fx32	aclr;
	BOOL	is_update;	//�ݒ肵���̂Ōv�ZUPDATE
	s8		dir;				//����
	u8		dummy;
} ACLR_WORK;

//-------------------------------------
///	�O���t�B�b�N���[�N
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_OBJ_WORK	obj;
	GFL_TCB	*p_vblank_task;
} GRAPHIC_WORK;
//-------------------------------------
///	�^�b�`
//=====================================
typedef struct 
{
	GFL_POINT start;
	GFL_POINT end;
  GFL_POINT drag_start;
  GFL_POINT drag_pos;
	u32	flik_sync;
	u32	release_sync;
	u32 key_cont_sync;
} UI_WORK;
//-------------------------------------
///	�V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param );
struct _SEQ_WORK
{
	SEQ_FUNCTION	seq_function;
	BOOL is_end;
	int seq;
	void *p_param;
}; 

//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct _IRC_RANKING_WORK
{
	//���W���[��
	GRAPHIC_WORK	grp;
	SEQ_WORK			seq;
	SCROLL_WORK		scroll;
	UI_WORK				ui;
	ACLR_WORK			aclr;
  APPBAR_WORK		*p_appbar;
  GFL_FONT      *p_font;
  PRINT_QUE     *p_que;

  GAMEDATA      *p_gamedata;

	//�f�[�^
	RANKING_DATA	*p_rank_data;

	//etc
	s16	move_new_sync;
	u16 dummy;
}IRC_RANKING_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT IRC_RANKING_PROC_Init(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RANKING_PROC_Exit(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RANKING_PROC_Main(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
//-------------------------------------
///	SEQ
//=====================================
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param, SEQ_FUNCTION seq_function );
static void SEQ_Exit( SEQ_WORK *p_wk );
static void SEQ_Main( SEQ_WORK *p_wk );
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk );
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function );
static void SEQ_End( SEQ_WORK *p_wk );
//-------------------------------------
///	SEQFUNC
//=====================================
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_MoveNew( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
//-------------------------------------
///	BG
//=====================================
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, GAMEDATA  *p_gamedata, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
static GFL_ARCUTIL_TRANSINFO GRAPHIC_BG_GetTransInfo( const GRAPHIC_BG_WORK *cp_wk, BOOL is_m );
//-------------------------------------
///	OBJ
//=====================================
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT * GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk );
//-------------------------------------
///	GRAPHIC
//=====================================
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, GAMEDATA  *p_gamedata, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
static const GRAPHIC_BG_WORK *GRAPHIC_GetBgWorkConst( const GRAPHIC_WORK *cp_wk );
static GFL_CLWK	* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT * GRAPHIC_GetUnit( const GRAPHIC_WORK *cp_wk );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//-------------------------------------
///	SCROLL	
//=====================================
static void SCROLL_Init( SCROLL_WORK *p_wk, u8 bar_frm_m, u8 font_frm_m, u8 bar_frm_s, u8 font_frm_s, const GRAPHIC_BG_WORK *cp_bg, GFL_CLUNIT *p_clunit, const RANKING_DATA* cp_data, HEAPID heapID );
static void SCROLL_Exit( SCROLL_WORK *p_wk );
static void SCROLL_Main( SCROLL_WORK *p_wk );
static void SCROLL_AddPos( SCROLL_WORK *p_wk, s16 y );
static s16 SCROLL_GetPosY( const SCROLL_WORK *cp_wk );
static s16 SCROLL_GetNewRankPosY( const SCROLL_WORK *cp_wk );
static s16 SCROLL_GetBottomLimitPosY( const SCROLL_WORK *cp_wk );
static void Scroll_VBlankTask( GFL_TCB *p_tcb, void *p_wk_adrs );
//-------------------------------------
///	RANKBAR
//=====================================
static void RANKBAR_Init( RANKBAR_WORK *p_wk, const RANKING_ONE_DATA *cp_data, GFL_CLUNIT *p_clunit, u32 icon_plt_idx , GFL_MSGDATA	*p_msg, GFL_FONT *p_font, WORDSET *p_wordset, BAR_BUFF *p_bar, HEAPID heapID );
static void RANKBAR_Exit( RANKBAR_WORK *p_wk );
static void RANKBAR_Trans( RANKBAR_WORK *p_wk, RANKBAR_DISPLAY display, u8 font_frm, u8 bar_frm, u16 y, u8 font_plt );
static void RANKBAR_Clear( RANKBAR_WORK *p_wk, RANKBAR_DISPLAY display );
static void RANKBAR_Move( RANKBAR_WORK *p_wk );
//-------------------------------------
///	BAR_BUFF
//=====================================
static void BARBUFF_Init( BAR_BUFF *p_wk, const GRAPHIC_BG_WORK *cp_bg, HEAPID heapID );
static void BARBUFF_Exit( BAR_BUFF *p_wk );
static void BARBUFF_Trans( BAR_BUFF *p_wk, u8 frm, u16 y );
static void BARBUFF_Clear( BAR_BUFF *p_wk, u8 frm, u16 y );
//-------------------------------------
///	RANKING_DATA
//=====================================
static RANKING_DATA	*RANKING_DATA_Create( GAMEDATA *p_gamedata, HEAPID heapID );
static void RANKING_DATA_Delete( RANKING_DATA	*p_data );
static u32 RANKING_DATA_GetRankNum( const RANKING_DATA *cp_wk );
//-------------------------------------
///	TOUCH
//=====================================
static void UI_Init( UI_WORK *p_wk, HEAPID heapID );
static void UI_Exit( UI_WORK *p_wk );
static void UI_Main( UI_WORK *p_wk );
static BOOL UI_GetDrag( UI_WORK *p_wk, GFL_POINT *p_start, GFL_POINT *p_end, GFL_POINT *p_drag_pos );
static BOOL UI_GetFlik( UI_WORK *p_wk, GFL_POINT *p_start, GFL_POINT *p_end, VecFx32 *p_vec, u32 *p_sync );
static int UI_IsContKey( const UI_WORK *cp_wk, u32 *p_sync );
//-------------------------------------
///	ACLR
//=====================================
static void ACLR_Init( ACLR_WORK *p_wk );
static void ACLR_Exit( ACLR_WORK *p_wk );
static void ACLR_Main( ACLR_WORK *p_wk );
static void ACLR_SetAclr( ACLR_WORK *p_wk, fx32 distance, u32 sync );
static void ACLR_Stop( ACLR_WORK *p_wk );
static s16 ACLR_GetScrollAdd( ACLR_WORK *p_wk );
static BOOL ACLR_IsExist( const ACLR_WORK *cp_wk );
//-------------------------------------
///	ETC
//=====================================
static void PRINT_PrintCenter( GFL_BMPWIN *p_bmpwin, STRBUF *p_strbuf, GFL_FONT *p_font );
static void PRINT_PrintNameCenter( GFL_BMPWIN *p_bmpwin, STRBUF *p_strbuf, const MYSTATUS *cp_status, GFL_FONT *p_font );
static void BMP_Copy( const GFL_BMP_DATA *cp_src, GFL_BMP_DATA *p_dst, u16 src_x, u16 src_y, u16 dst_x, u16 dst_y, u16 w, u16 h );
static s32 GFL_POINT_Distance( const GFL_POINT *cp_a, const GFL_POINT *cp_b );
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h,  u8 plt, BOOL compressedFlag, HEAPID heapID );

//=============================================================================
/**
 *						DATA
 */
//=============================================================================
//-------------------------------------
///	�f�[�^
//=====================================
enum
{	
	GRAPHIC_BG_FRAME_TOP_M,
	GRAPHIC_BG_FRAME_BAR_M,
	GRAPHIC_BG_FRAME_FONT_M,
	GRAPHIC_BG_FRAME_BACK_M,

	GRAPHIC_BG_FRAME_INFO_S,
	GRAPHIC_BG_FRAME_BAR_S,
	GRAPHIC_BG_FRAME_FONT_S,
	GRAPHIC_BG_FRAME_BACK_S,

	GRAPHIC_BG_FRAME_MAX
};
static const struct 
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
}	sc_bgsetup[GRAPHIC_BG_FRAME_MAX]	=
{	
	{	
		GFL_BG_FRAME0_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME1_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME2_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME3_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},


	//BOTTOM
	{	
		GFL_BG_FRAME0_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME1_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME2_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME3_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	}
};
//�t���[���擾�p�}�N��
#define GRAPHIC_BG_GetFrame( x )	(sc_bgsetup[ x ].frame)

//=============================================================================
/**
 *						GLOBAL
 */
//=============================================================================
//-------------------------------------
///	PROC�f�[�^�O�����J
//=====================================
const GFL_PROC_DATA	IrcRanking_ProcData	=
{
	IRC_RANKING_PROC_Init,
	IRC_RANKING_PROC_Main,
	IRC_RANKING_PROC_Exit,
} ;

//=============================================================================
/**
 *						PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	PROC������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					����
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RANKING_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_RANKING_WORK	*p_wk;
	IRC_RANKING_PARAM	*p_rank_param	= p_param;


	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCRANKING, 0x60000 );

	//���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(IRC_RANKING_WORK), HEAPID_IRCRANKING );
	GFL_STD_MemClear( p_wk, sizeof(IRC_RANKING_WORK) );


  if( p_rank_param->p_gamesys )
  { 
    p_wk->p_gamedata  = GAMESYSTEM_GetGameData(p_rank_param->p_gamesys);
  }
  else
  { 
    p_wk->p_gamedata  = GAMEDATA_Create( GFL_HEAPID_APP );

  }

	//�p�����[�^�����Ƃ�
  p_wk->p_que   = PRINTSYS_QUE_Create( HEAPID_IRCRANKING );
  p_wk->p_font  = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
				GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_IRCRANKING ); 

	//�f�[�^�쐬
	p_wk->p_rank_data	= RANKING_DATA_Create( p_wk->p_gamedata, HEAPID_IRCRANKING );

	//���W���[��������
	GRAPHIC_Init( &p_wk->grp, p_wk->p_gamedata, HEAPID_IRCRANKING );
	SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeOut );
	UI_Init( &p_wk->ui, HEAPID_IRCRANKING );
	ACLR_Init( &p_wk->aclr );
	SCROLL_Init( &p_wk->scroll,
			GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M),
			GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_FONT_M),
			GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S),
			GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_FONT_S),
			GRAPHIC_GetBgWorkConst(&p_wk->grp),
      GRAPHIC_GetUnit(&p_wk->grp),
			p_wk->p_rank_data,
			HEAPID_IRCRANKING
			);

  p_wk->p_appbar	= APPBAR_Init( APPBAR_OPTION_MASK_RETURN, GRAPHIC_GetUnit( &p_wk->grp ), GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_INFO_S), RANKING_BG_PAL_S_13, 0, APP_COMMON_MAPPING_128K, p_wk->p_font, p_wk->p_que, HEAPID_IRCRANKING );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					����
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RANKING_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_RANKING_WORK	*p_wk	= p_work;
  IRC_RANKING_PARAM	*p_rank_param	=p_param;

	//���W���[���j��
	APPBAR_Exit( p_wk->p_appbar );
	SCROLL_Exit( &p_wk->scroll );
	ACLR_Exit( &p_wk->aclr );
	UI_Exit( &p_wk->ui );
	SEQ_Exit( &p_wk->seq );
	GRAPHIC_Exit( &p_wk->grp );

	//�f�[�^�j��
	RANKING_DATA_Delete( p_wk->p_rank_data );

  GFL_FONT_Delete( p_wk->p_font );
  PRINTSYS_QUE_Delete( p_wk->p_que );

  if( p_rank_param->p_gamesys == NULL )
  { 
    GAMEDATA_Delete( p_wk->p_gamedata );
  }

	//���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap(HEAPID_IRCRANKING );


	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					����
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RANKING_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_RANKING_WORK	*p_wk	= p_work;

	//�V�[�P���X
	SEQ_Main( &p_wk->seq );

	//�`��
	GRAPHIC_Draw( &p_wk->grp );

	//�I��
	if( SEQ_IsEnd( &p_wk->seq ) )
	{	
		return GFL_PROC_RES_FINISH;
	}
	else
	{	
		return GFL_PROC_RES_CONTINUE;
	}
}
//=============================================================================
/**
 *						SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	������
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param, SEQ_FUNCTION seq_function )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );

	//������
	p_wk->p_param	= p_param;

	//�Z�b�g
	SEQ_SetNext( p_wk, seq_function );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void SEQ_Exit( SEQ_WORK *p_wk )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Main( SEQ_WORK *p_wk )
{	
	if( !p_wk->is_end )
	{	
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_param );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I���擾
 *
 *	@param	const SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk )
{	
	return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I��
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( SEQ_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}
//=============================================================================
/**
 *					SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�hOUT
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{	
	enum
	{	
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_END,
	};

	IRC_RANKING_WORK	*p_wk	= p_param;

	switch( *p_seq )
	{	
	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		//�V�K����������A�ړ��V�[�P���X��
		if( SCROLL_GetNewRankPosY( &p_wk->scroll ) != SCROLL_NEWRANK_NULL )
		{	
			SEQ_SetNext( &p_wk->seq, SEQFUNC_MoveNew );
		}
		else
		{	
			SEQ_SetNext( p_seqwk, SEQFUNC_Main );
		}
		break;

	default:
		GF_ASSERT(0);
	}
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�hIN
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	IRC_RANKING_WORK	*p_wk	= p_param;

	switch( *p_seq )
	{	
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
		SEQ_End( p_seqwk );
		break;

	default:
		GF_ASSERT(0);
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���J�n
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{	
	IRC_RANKING_WORK	*p_wk	= p_param;
	u32 sync;

	//�L�[���͂ɂ��ړ�
	if( UI_IsContKey( &p_wk->ui, &sync ) & PAD_KEY_UP )
	{	
		fx32 rate;
		if( ACLR_SCROLL_KEY_MIN < FX32_CONST(sync) )
		{	
			rate	= FX_Div( (FX32_CONST(sync) - ACLR_SCROLL_KEY_MIN), ACLR_SCROLL_KEY_DIF );
			ACLR_SetAclr( &p_wk->aclr, FX_Mul( rate	, ACLR_SCROLL_DISTANCE_MAX ), ACLR_SCROLL_KEY_MOVE_ACLR_SYNC );
		}
		else
		{	
			ACLR_SetAclr( &p_wk->aclr, ACLR_SCROLL_KEY_MOVE_INIT_DISTANCE, ACLR_SCROLL_KEY_MOVE_INIT_SYNC );
		}
	}
	else if( UI_IsContKey( &p_wk->ui, &sync ) & PAD_KEY_DOWN )
	{	
		fx32 rate;
		if( ACLR_SCROLL_KEY_MIN < FX32_CONST(sync) )
		{	
			rate	= FX_Div( (FX32_CONST(sync) - ACLR_SCROLL_KEY_MIN), ACLR_SCROLL_KEY_DIF );
			ACLR_SetAclr( &p_wk->aclr, -FX_Mul( rate, ACLR_SCROLL_DISTANCE_MAX ), ACLR_SCROLL_KEY_MOVE_ACLR_SYNC );
		}
		else
		{	
			ACLR_SetAclr( &p_wk->aclr, -ACLR_SCROLL_KEY_MOVE_INIT_DISTANCE, ACLR_SCROLL_KEY_MOVE_INIT_SYNC );
		}
	}
	else
	//�^�b�`�ɂ��ړ�
	{	
    GFL_POINT drag_now;
		VecFx32 dist;
		fx32 distance;
		u32		sync;

		static const VecFx32 sc_up_norm	= 
		{
			0, FX32_ONE, 0
		};

		//�h���b�O�ɂ��ړ�
		if( UI_GetDrag( &p_wk->ui, NULL, NULL, &drag_now ) )
		{	
      SCROLL_AddPos( &p_wk->scroll, -drag_now.y );
			ACLR_Stop( &p_wk->aclr );
		}
		//�͂����ɂ��ړ�		
		else if( UI_GetFlik( &p_wk->ui, NULL, NULL, &dist, &sync ) )
		{	
			distance	= VEC_DotProduct( &sc_up_norm, &dist );
			ACLR_SetAclr( &p_wk->aclr, distance, sync );
		}
	}

	//�����\�Ȃ�Έړ�
	if( ACLR_IsExist(&p_wk->aclr) )
	{	
		SCROLL_AddPos( &p_wk->scroll, ACLR_GetScrollAdd( &p_wk->aclr ) );
	}



	//�߂�
	if( APPBAR_GetTrg( p_wk->p_appbar ) == APPBAR_ICON_RETURN
			|| GFL_UI_KEY_GetCont() & PAD_BUTTON_B )
	{	
		PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );	
		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
	}

	//���W���[�����C��
	SCROLL_Main( &p_wk->scroll );
	UI_Main( &p_wk->ui );
	ACLR_Main( &p_wk->aclr );
	APPBAR_Main( p_wk->p_appbar );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V�K�����N�܂ňړ����鏈��
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�p�����[�^
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MoveNew( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{	
	IRC_RANKING_WORK	*p_wk	= p_param;
	s16 new_pos;
	s16 now_pos;
	s16 limit_pos;

	new_pos		= SCROLL_GetNewRankPosY( &p_wk->scroll );
	GF_ASSERT( new_pos != SCROLL_NEWRANK_NULL );
	new_pos		-= ACLR_SCROLL_NEWRANK_OFS;
	new_pos		= MATH_CLAMP( new_pos, 0, new_pos );
	now_pos		= SCROLL_GetPosY( &p_wk->scroll );
	limit_pos	= SCROLL_GetBottomLimitPosY( &p_wk->scroll );

	//�V�K�����̏ꏊ�ɂ��邩�A�Ō�܂ł���������
	if( new_pos  > now_pos && now_pos != limit_pos )
	{	
		fx32 rate;
		p_wk->move_new_sync++;
		if( ACLR_SCROLL_KEY_MIN < FX32_CONST(p_wk->move_new_sync) )
		{	
			rate	= FX_Div( (FX32_CONST(p_wk->move_new_sync) - ACLR_SCROLL_KEY_MIN), ACLR_SCROLL_KEY_DIF );
			ACLR_SetAclr( &p_wk->aclr, FX_Mul( rate, ACLR_SCROLL_DISTANCE_MAX ), ACLR_SCROLL_KEY_MOVE_ACLR_SYNC );
		}
		else
		{	
			ACLR_SetAclr( &p_wk->aclr, ACLR_SCROLL_KEY_MOVE_INIT_DISTANCE, ACLR_SCROLL_KEY_MOVE_INIT_SYNC );
		}
	}
	else
	{	
		ACLR_Stop( &p_wk->aclr );
		SEQ_SetNext( &p_wk->seq, SEQFUNC_Main );
	}

	//�����\�Ȃ�Έړ�
	if( ACLR_IsExist(&p_wk->aclr) )
	{	
		SCROLL_AddPos( &p_wk->scroll, ACLR_GetScrollAdd( &p_wk->aclr ) );
	}


	//�߂�
	if( APPBAR_GetTrg( p_wk->p_appbar ) == APPBAR_ICON_RETURN
			|| GFL_UI_KEY_GetCont() & PAD_BUTTON_B )
	{	
		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
	}
	//���W���[�����C��
	SCROLL_Main( &p_wk->scroll );
	ACLR_Main( &p_wk->aclr );
	APPBAR_Main( p_wk->p_appbar );
}
//=============================================================================
/**
 *						BG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG	������
 *
 *	@param	GRAPHIC_BG_WORK *p_wk	���[�N
 *	@param	heapID								�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, GAMEDATA  *p_gamedata, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );

	//������
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );

	//�O���t�B�b�N���[�h�ݒ�
	{	
		static const GFL_BG_SYS_HEADER sc_bgsys_header	=
		{	
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};
		GFL_BG_SetBGMode( &sc_bgsys_header );
	}

	//BG�ʐݒ�
	{	
		int i;
		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{	
			GFL_BG_SetBGControl( sc_bgsetup[i].frame, &sc_bgsetup[i].bgcnt_header, sc_bgsetup[i].mode );
			GFL_BG_ClearFrame( sc_bgsetup[i].frame );
			GFL_BG_SetVisible( sc_bgsetup[i].frame, VISIBLE_ON );
		}
	}

	//�ǂݍ���
	{	
		p_wk->p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCRANKING_GRAPHIC, heapID );


		GFL_ARCHDL_UTIL_TransVramPalette( p_wk->p_handle, NARC_ircranking_gra_bg_NCLR,
				PALTYPE_MAIN_BG, RANKING_BG_PAL_M_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_wk->p_handle, NARC_ircranking_gra_back_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_M), 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_wk->p_handle, NARC_ircranking_gra_top_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M), 0, 0, FALSE, heapID );


		GFL_ARCHDL_UTIL_TransVramScreen( p_wk->p_handle, NARC_ircranking_gra_back_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_M), 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_wk->p_handle, NARC_ircranking_gra_top_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M), 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_wk->p_handle, NARC_ircranking_gra_bg_NCLR,
				PALTYPE_SUB_BG, RANKING_BG_PAL_S_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_wk->p_handle, NARC_ircranking_gra_top_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_S), 0, 0, FALSE, heapID );


		GFL_ARCHDL_UTIL_TransVramScreen( p_wk->p_handle, NARC_ircranking_gra_top2_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_S), 0, 0, FALSE, heapID );




		GFL_BG_FillCharacter( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, 1,  0 );
		GFL_BG_FillCharacter( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S), 0, 1,  0 );

		p_wk->bar_char_m	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_wk->p_handle, 
				NARC_ircranking_gra_frame_NCGR, GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, FALSE, heapID );
		p_wk->bar_char_s	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_wk->p_handle, 
				NARC_ircranking_gra_frame_NCGR, GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S), 0, FALSE, heapID );

		//�t�H���g�p�p���b�g
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, RANKING_BG_PAL_M_14*0x20, 0x20, heapID );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, RANKING_BG_PAL_S_14*0x20, 0x20, heapID );

		GFL_ARC_CloseDataHandle( p_wk->p_handle );

	}

	//BMPWIN
	{
		GFL_MSGDATA	*p_msg;
		GFL_FONT		*p_font;
		STRBUF			*p_strbuf;
		int i;
		u16 x, y;

		//�쐬
		p_msg				= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
				NARC_message_irc_ranking_dat, heapID );
		p_font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
				GFL_FONT_LOADTYPE_FILE, FALSE, heapID );


		p_wk->p_bmpwin[BMPWIN_ID_TITLE]	= GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M),
				BMPWIN_TITLE_X,BMPWIN_TITLE_Y,BMPWIN_TITLE_W,BMPWIN_TITLE_H,RANKING_BG_PAL_M_14,
			GFL_BMP_CHRAREA_GET_B );
		p_wk->p_bmpwin[BMPWIN_ID_RANK]	= GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M),
				BMPWIN_RANK_X,BMPWIN_RANK_Y,BMPWIN_RANK_W,BMPWIN_RANK_H,RANKING_BG_PAL_M_14,
			GFL_BMP_CHRAREA_GET_B );
		p_wk->p_bmpwin[BMPWIN_ID_PLAYER]	= GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M),
				BMPWIN_PLAYER_X,BMPWIN_PLAYER_Y,BMPWIN_PLAYER_W,BMPWIN_PLAYER_H,RANKING_BG_PAL_M_14,
			GFL_BMP_CHRAREA_GET_B );
		p_wk->p_bmpwin[BMPWIN_ID_SCORE]	= GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M),
				BMPWIN_SCORE_X,BMPWIN_SCORE_Y,BMPWIN_SCORE_W,BMPWIN_SCORE_H,RANKING_BG_PAL_M_14,
			GFL_BMP_CHRAREA_GET_B );
		p_wk->p_bmpwin[BMPWIN_ID_COUNT]	= GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M),
				BMPWIN_COUNT_X,BMPWIN_COUNT_Y,BMPWIN_COUNT_W,BMPWIN_COUNT_H,RANKING_BG_PAL_M_14,
			GFL_BMP_CHRAREA_GET_B );

		for( i = 0; i < BMPWIN_ID_MAX; i++ )
		{	
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0xF );	
		}


		//�����`��
		p_strbuf	= GFL_MSG_CreateString( p_msg, RANKING_TITLE_000 );
    { 
      MYSTATUS  *p_mystatus = NULL;
      { 
        p_mystatus =GAMEDATA_GetMyStatus(p_gamedata);
      }
      PRINT_PrintNameCenter( p_wk->p_bmpwin[BMPWIN_ID_TITLE], p_strbuf, p_mystatus, p_font );
    }
		GFL_STR_DeleteBuffer( p_strbuf );

		p_strbuf	= GFL_MSG_CreateString( p_msg, RANKING_CAPTION_000 );
		PRINT_PrintCenter( p_wk->p_bmpwin[BMPWIN_ID_RANK], p_strbuf, p_font );
		GFL_STR_DeleteBuffer( p_strbuf );

		p_strbuf	= GFL_MSG_CreateString( p_msg, RANKING_CAPTION_001 );
		PRINT_PrintCenter( p_wk->p_bmpwin[BMPWIN_ID_PLAYER], p_strbuf, p_font );
		GFL_STR_DeleteBuffer( p_strbuf );

		p_strbuf	= GFL_MSG_CreateString( p_msg, RANKING_CAPTION_002 );
		PRINT_PrintCenter( p_wk->p_bmpwin[BMPWIN_ID_SCORE], p_strbuf, p_font );
		GFL_STR_DeleteBuffer( p_strbuf );

		p_strbuf	= GFL_MSG_CreateString( p_msg, RANKING_CAPTION_003 );
		PRINT_PrintCenter( p_wk->p_bmpwin[BMPWIN_ID_COUNT], p_strbuf, p_font );
		GFL_STR_DeleteBuffer( p_strbuf );

		//�j��
		GFL_FONT_Delete( p_font );
		GFL_MSG_Delete( p_msg );

		//�]��
		for( i = 0; i < BMPWIN_ID_MAX; i++ )
		{	
			GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[i] );
			GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin[i] );
		}
		GFL_BG_LoadScreenReq(GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M) );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	�j��
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk )
{	
	
	//BMPWIN�j��
	{	
		int i;
		for( i = 0; i < BMPWIN_ID_MAX; i++ )
		{	
			GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
		}
	}

	//�j��
	{	
		GFL_BG_FreeCharacterArea(GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M),
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->bar_char_m),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->bar_char_m));

		GFL_BG_FreeCharacterArea(GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S),
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->bar_char_s),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->bar_char_s));

		GFL_BG_FillCharacterRelease( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S), 1, 0 );
		GFL_BG_FillCharacterRelease( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 1, 0 );
	}

	//BG�ʔj��
	{	
		int i;
		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{	
			GFL_BG_FreeBGControl( sc_bgsetup[i].frame );
		}
	}

	//�I��
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	VBlank�֐�
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk )
{	
	GFL_BG_VBlankFunc();
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g�����X�C���t�H�擾
 *
 *	@param	const GRAPHIC_BG_WORK *cp_wk	���[�N
 *	@param	is_m													TRUE�Ȃ�Ώ��ʗp
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static GFL_ARCUTIL_TRANSINFO GRAPHIC_BG_GetTransInfo( const GRAPHIC_BG_WORK *cp_wk, BOOL is_m )
{	
	if( is_m )
	{	
		return cp_wk->bar_char_m;
	}
	else
	{	
		return cp_wk->bar_char_s;
	}
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

#if 0
	//���\�[�X�ǂݍ���
	{	
		ARCHANDLE *p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCAURA_GRAPHIC, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_PLT]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_ircaura_gra_aura_obj_touch_NCLR, CLSYS_DRAW_MAIN, AURA_OBJ_PAL_M_00*0x20, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_ircaura_gra_aura_obj_touch_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_ircaura_gra_aura_obj_touch_NCER, NARC_ircaura_gra_aura_obj_touch_NANR, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK�쐬
	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		p_wk->p_clwk[CLWKID_TOUCH]	= GFL_CLACT_WK_Create( p_wk->p_clunit, 
				p_wk->reg_id[OBJREGID_TOUCH_CHR],
				p_wk->reg_id[OBJREGID_TOUCH_PLT],
				p_wk->reg_id[OBJREGID_TOUCH_CEL],
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID
				);
	}
#endif


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
#if 0
	//CLWK�j��
	{	
		int i;
		for( i = 0; i < CLWKID_MAX; i++ )
		{	
			GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
		}
	}

	//���\�[�X�j��
	{	
		GFL_CLGRP_PLTT_Release( p_wk->reg_id[OBJREGID_TOUCH_PLT] );
		GFL_CLGRP_CGR_Release( p_wk->reg_id[OBJREGID_TOUCH_CHR] );
		GFL_CLGRP_CELLANIM_Release( p_wk->reg_id[OBJREGID_TOUCH_CEL] );
	}
#endif 

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
 *	@brief	CL���j�b�g�̎擾
 *
 *	@param	const GRAPHIC__OBJ_WORK *cp_wk	���[�N
 *
 *	@return	GFL_CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT * GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk )
{	
	return cp_wk->p_clunit;
}
//=============================================================================
/**
 *						GRAPHIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�ݒ�
 *
 *	@param	GRAPHIC_WORK *p_wk	���[�N
 *	@param	heapID							�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, GAMEDATA  *p_gamedata, HEAPID heapID )
{	
	static const GFL_DISP_VRAM sc_vramSetTable =
	{
		GX_VRAM_BG_128_A,					// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,					// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_128_D,       // �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,						// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,				// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,		
		GX_OBJVRAMMODE_CHAR_1D_128K,		
	};

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );

	//���W�X�^������
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	//VRAM�N���A�[
	GFL_DISP_ClearVRAM( 0 );

	//VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &sc_vramSetTable );

	// �f�B�X�v���CON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );
	GFL_DISP_SetDispOn();

	//	�\��
	GFL_DISP_GX_InitVisibleControl();

	//	�t�H���g������
	GFL_FONTSYS_Init();

	//	���W���[��������
	GRAPHIC_BG_Init( &p_wk->bg, p_gamedata, heapID );
	GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );

	//VBlankTask�o�^
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_VBlankTask, p_wk, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�j��
 *
 *	@param	GRAPHIC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk )
{	
	//VBLANKTask����
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	//���W���[���j��
	GRAPHIC_OBJ_Exit( &p_wk->obj );
	GRAPHIC_BG_Exit( &p_wk->bg );

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�`��
 *
 *	@param	GRAPHIC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk )
{	
	GRAPHIC_OBJ_Main( &p_wk->obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG���[�N�擾
 *
 *	@param	const GRAPHIC_WORK *cp_wk		���[�N
 *
 *	@return	BG���[�N
 */
//-----------------------------------------------------------------------------
static const GRAPHIC_BG_WORK *GRAPHIC_GetBgWorkConst( const GRAPHIC_WORK *cp_wk )
{	
	return &cp_wk->bg;
}
//----------------------------------------------------------------------------
/**
 *	@brief	CLWK�擾
 *
 *	@param	const GRAPHIC_WORK *cp_wk	���[�N
 *	@param	id												CLWKID
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK	* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id )
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
static GFL_CLUNIT * GRAPHIC_GetUnit( const GRAPHIC_WORK *cp_wk )
{	
	return GRAPHIC_OBJ_GetUnit( &cp_wk->obj );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�NVBLANK�֐�
 *
 *	@param	GRAPHIC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work )
{	
	GRAPHIC_WORK *p_wk	= p_work;
	GRAPHIC_BG_VBlankFunction( &p_wk->bg );
	GRAPHIC_OBJ_VBlankFunction( &p_wk->obj );
}
//=============================================================================
/**
 *	SCROLL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SCROLL	������
 *
 *	@param	SCROLL_WORK *p_wk	���[�N
 *	@param	bar_frm						�g��BG��
 *	@param	font_frm					������BG��
 *	@param	bar_frm						�g��BG��
 *	@param	font_frm					������BG��
 *	@param	*p_bg							BG�V�X�e��
 *	@param	cp_data						�\������f�[�^
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void SCROLL_Init( SCROLL_WORK *p_wk, u8 bar_frm_m, u8 font_frm_m, u8 bar_frm_s, u8 font_frm_s, const GRAPHIC_BG_WORK *cp_bg, GFL_CLUNIT *p_clunit, const RANKING_DATA* cp_data, HEAPID heapID )
{	
	//�N���A�[
	GFL_STD_MemClear( p_wk, sizeof(SCROLL_WORK) );
	p_wk->cp_bg				= cp_bg;
	p_wk->bar_frm_m		= bar_frm_m;
	p_wk->font_frm_m	= font_frm_m;
	p_wk->bar_frm_s		= bar_frm_s;
	p_wk->font_frm_s	= font_frm_s;
	p_wk->cp_data			=	cp_data;
  p_wk->y           = SCROLL_INIT_POS_Y;

  GFL_BG_SetScroll( p_wk->bar_frm_m, GFL_BG_SCROLL_Y_SET, p_wk->y );
  GFL_BG_SetScroll( p_wk->font_frm_m, GFL_BG_SCROLL_Y_SET, p_wk->y );
  GFL_BG_SetScroll( p_wk->bar_frm_s, GFL_BG_SCROLL_Y_SET, p_wk->y );
  GFL_BG_SetScroll( p_wk->font_frm_s, GFL_BG_SCROLL_Y_SET, p_wk->y );

	//�����L���O�p�t�H���g�쐬
	p_wk->p_font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, TRUE, heapID );

	//�����L���O�p���b�Z�[�W�}�l�[�W��
	p_wk->p_msg				= GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_ranking_dat, heapID );

	//�P��Z�b�g�쐬
	p_wk->p_wordset	= WORDSET_Create( heapID );

  //�o�[�̋��ʑf�ލ쐬
  { 
    BARBUFF_Init( &p_wk->barbuff, cp_bg, heapID );

    {
      ARCHANDLE* p_handle  = GFL_ARC_OpenDataHandle( ARCID_POKEICON, heapID );
      p_wk->icon_plt_idx  = GFL_CLGRP_PLTT_RegisterComp( p_handle, POKEICON_GetPalArcIndex(), CLSYS_DRAW_MAX, RANKING_OBJ_PAL_POKEICON_M*0x20, heapID );
      GFL_ARC_CloseDataHandle( p_handle );
    }
  }

  //�o�[�f�[�^�쐬
  { 
    int i;

    for( i = 0; i < RANKING_DATA_GetRankNum( cp_data ); i++ )
    { 
      RANKBAR_Init( &p_wk->rankbar[i], &cp_data->data[i], p_clunit, p_wk->icon_plt_idx, p_wk->p_msg, p_wk->p_font, p_wk->p_wordset, &p_wk->barbuff, heapID );
    }
  }


  //���񏑂�����
  { 
    int i;
    int cnt = SCROLL_BAR_START_RANK_M;
    for( i = SCROLL_WRITE_POS_START_M; i < SCROLL_WRITE_POS_END_M; i+=SCROLL_BAR_HEIGHT )
    { 
      if( 0 <= cnt && cnt < RANKING_DATA_GetRankNum( cp_data ) )
      { 
        RANKBAR_Trans( &p_wk->rankbar[cnt], RANKBAR_DISPLAY_M, p_wk->font_frm_m, p_wk->bar_frm_m, i, RANKING_BG_PAL_M_14 );
      }
      cnt++;
    }
    cnt = SCROLL_BAR_START_RANK_S;
    for( i = SCROLL_WRITE_POS_START_S; i < SCROLL_WRITE_POS_END_S; i+=SCROLL_BAR_HEIGHT )
    { 
      if( 0 <= cnt && cnt < RANKING_DATA_GetRankNum( cp_data ) )
      { 
        RANKBAR_Trans( &p_wk->rankbar[cnt], RANKBAR_DISPLAY_S, p_wk->font_frm_s, p_wk->bar_frm_s, i, RANKING_BG_PAL_S_14 );
      }
      cnt++;
    }
  }

  GFL_BG_LoadScreenReq(p_wk->bar_frm_m);
  GFL_BG_LoadScreenReq(p_wk->font_frm_m);
  GFL_BG_LoadScreenReq(p_wk->bar_frm_s);
  GFL_BG_LoadScreenReq(p_wk->font_frm_s);


  //VBlankTask�o�^
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Scroll_VBlankTask, p_wk, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SCROLL	�j��
 *
 *	@param	SCROLL_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SCROLL_Exit( SCROLL_WORK *p_wk )
{	
	//VBLANKTask����
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

  //�o�[�f�[�^�j��
  { 
    int i;

    for( i = 0; i < RANKING_DATA_GetRankNum( p_wk->cp_data ); i++ )
    { 
      RANKBAR_Exit( &p_wk->rankbar[i] );
    }
  }

  //�o�[���ʑf�ޔj��
  { 
    GFL_CLGRP_PLTT_Release( p_wk->icon_plt_idx );

    BARBUFF_Exit( &p_wk->barbuff );
  }

  //�P��Z�b�g�j��
	WORDSET_Delete( p_wk->p_wordset );

	//���b�Z�[�W�j��
	GFL_MSG_Delete( p_wk->p_msg );

	//�t�H���g�j��
	GFL_FONT_Delete( p_wk->p_font );

	//�N���A�[
	GFL_STD_MemClear( p_wk, sizeof(SCROLL_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SCROLL	���C������
 *
 *	@param	SCROLL_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SCROLL_Main( SCROLL_WORK *p_wk )
{	
  //���ֈʒu���`�F�b�N
  if( (p_wk->y - SCROLL_INIT_POS_Y) %  (SCROLL_BAR_HEIGHT*GFL_BG_1CHRDOTSIZ) == 0 )
  { 
    p_wk->is_update  = TRUE;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	SCROLL	�ʒu
 *
 *	@param	SCROLL_WORK *p_wk	���[�N
 *	@param	y									Y
 *
 */
//-----------------------------------------------------------------------------
static void SCROLL_AddPos( SCROLL_WORK *p_wk, s16 y )
{	
  s16 temp  = p_wk->y + y;

  //Y���傫���Ƃ��C��
  if( SCROLL_INIT_POS_Y > temp )
  { 
    temp  = SCROLL_INIT_POS_Y;
  }
  if( temp > SCROLL_GetBottomLimitPosY( p_wk ) )
  { 
    temp  = SCROLL_GetBottomLimitPosY( p_wk )-1;
  }

  if( SCROLL_INIT_POS_Y <= temp && temp < SCROLL_GetBottomLimitPosY( p_wk ) )
  {
    p_wk->dir = y / MATH_IAbs(y);
    p_wk->y = temp;
    GFL_BG_SetScroll( p_wk->bar_frm_m, GFL_BG_SCROLL_Y_SET, p_wk->y );
    GFL_BG_SetScroll( p_wk->font_frm_m, GFL_BG_SCROLL_Y_SET, p_wk->y );
    GFL_BG_SetScroll( p_wk->bar_frm_s, GFL_BG_SCROLL_Y_SET, p_wk->y );
    GFL_BG_SetScroll( p_wk->font_frm_s, GFL_BG_SCROLL_Y_SET, p_wk->y );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[���ʒu���擾
 *
 *	@param	const SCROLL_WORK *cp_wk	���[�N
 *
 *	@return	�X�N���[���ʒu
 */
//-----------------------------------------------------------------------------
static s16 SCROLL_GetPosY( const SCROLL_WORK *cp_wk )
{	
	return cp_wk->y;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V�K�����N�C���̍��W���擾
 *
 *	@param	const SCROLL_WORK *cp_wk	���[�N 
 *
 *	@return	�V�K�����N�C���̍��W
 */
//-----------------------------------------------------------------------------
static s16 SCROLL_GetNewRankPosY( const SCROLL_WORK *cp_wk )
{	
  /*
	int i;
	BOOL is_exist	= FALSE;
	for( i = 0; i < cp_wk->data_len; i++ )
	{	
		if( cp_wk->cp_data[i].plt == 5 )
		{	
			is_exist	= TRUE;
			break;
		}
	}
	if( !is_exist )
	{	
		return SCROLL_NEWRANK_NULL;
	}

	return i * SCROLL_BAR_HEIGHT * GFL_BG_1CHRDOTSIZ;

  */
  return 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ړ������l���擾
 *
 *	@param	const SCROLL_WORK *cp_wk ���[�N
 *
 *	@return	�ړ������l
 */
//-----------------------------------------------------------------------------
static s16 SCROLL_GetBottomLimitPosY( const SCROLL_WORK *cp_wk )
{	
  s16 num = (RANKING_DATA_GetRankNum( cp_wk->cp_data ) - SCROLL_BAR_VISIBLE_DISPLAY);

  if( 0 < num )
  { 
    return (num * SCROLL_BAR_HEIGHT)*GFL_BG_1CHRDOTSIZ + SCROLL_INIT_POS_Y;
  }
  else
  { 
    return 0;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  VBLANK�^�X�N
 *
 *	@param	GFL_TCB *p_tcb  �^�X�N
 *	@param	*p_work         ���[�N
 */
//-----------------------------------------------------------------------------
static void Scroll_VBlankTask( GFL_TCB *p_tcb, void *p_wk_adrs )
{ 
  SCROLL_WORK *p_wk = p_wk_adrs;
#if 0
  if( p_wk->is_update )
  { 
    if( p_wk->dir > 0 )
    { 
      //��ʏ㕔�̈ʒu
      s32 display_up_pos  = ;
      s32 rank;

      rank  = ;
      if( 0 <= cnt && cnt < RANKING_DATA_GetRankNum( cp_data ) )
      { 
        RANKBAR_Trans( &p_wk->rankbar[cnt], RANKBAR_DISPLAY_M, p_wk->font_frm_m, p_wk->bar_frm_m, i, RANKING_BG_PAL_M_14 );
      }
      if( 0 <= cnt && cnt < RANKING_DATA_GetRankNum( cp_data ) )
      { 
        RANKBAR_Trans( &p_wk->rankbar[cnt], RANKBAR_DISPLAY_S, p_wk->font_frm_s, p_wk->bar_frm_s, i, RANKING_BG_PAL_S_14 );
      }
    }
    else
    { 
      //��ʉ����̈ʒu
      s32 display_bottom_pos  = ;
      s32 rank;

      if( 0 <= cnt && cnt < RANKING_DATA_GetRankNum( cp_data ) )
      { 
        RANKBAR_Trans( &p_wk->rankbar[cnt], RANKBAR_DISPLAY_M, p_wk->font_frm_m, p_wk->bar_frm_m, i, RANKING_BG_PAL_M_14 );
      }

      if( 0 <= cnt && cnt < RANKING_DATA_GetRankNum( cp_data ) )
      { 
        RANKBAR_Trans( &p_wk->rankbar[cnt], RANKBAR_DISPLAY_S, p_wk->font_frm_s, p_wk->bar_frm_s, i, RANKING_BG_PAL_S_14 );
      }
    }

    p_wk->is_update = FALSE;
  }
#endif

}
//=============================================================================
/**
 *	RANKBAR_WORK
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �o�[�P���쐬
 *
 *	@param	RANKBAR_WORK *p_wk        ���[�N
 *	@param	RANKING_ONE_DATA *cp_data �f�[�^
 *	@param  GFL_CLUNIT                ���j�b�g
 *	@param  plt_idx                   �|�P�A�C�R���p���b�g�o�^�C���f�b�N�X
 *	@param	heapID                    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void RANKBAR_Init( RANKBAR_WORK *p_wk, const RANKING_ONE_DATA *cp_data, GFL_CLUNIT *p_clunit, u32 icon_plt_idx , GFL_MSGDATA	*p_msg, GFL_FONT *p_font, WORDSET *p_wordset, BAR_BUFF *p_bar, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(RANKBAR_WORK) );
  p_wk->cp_data = cp_data;
  p_wk->p_buff  = p_bar;

  //BMP�쐬
  p_wk->p_bmp = GFL_BMP_Create( SCROLL_BAR_FONT_WIDTH, SCROLL_BAR_HEIGHT, GFL_BMP_16_COLOR, heapID );

  
  //��������
  { 
    STRBUF* p_rank_buf	= GFL_STR_CreateBuffer( 32, heapID );
    STRBUF* p_score_buf	= GFL_STR_CreateBuffer( 32, heapID );
    STRBUF* p_count_buf	= GFL_STR_CreateBuffer( 32, heapID );
    STRBUF* p_strbuf		= GFL_STR_CreateBuffer( 32, heapID );

    GFL_MSG_GetString( p_msg, RANKING_RANK_000, p_rank_buf );
    GFL_MSG_GetString( p_msg, RANKING_SCORE_000, p_score_buf );
    GFL_MSG_GetString( p_msg, RANKING_COUNT_000, p_count_buf );
    WORDSET_RegisterNumber( p_wordset, 0, cp_data->rank, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( p_wordset, 2, cp_data->score, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( p_wordset, 3, cp_data->play_cnt, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );

    //�����`��
    //�����N
    WORDSET_ExpandStr( p_wordset, p_strbuf, p_rank_buf );
    PRINTSYS_Print( p_wk->p_bmp, SCROLL_BAR_FONT_RANK_X*GFL_BG_1CHRDOTSIZ+3, SCROLL_FONT_Y_OFS, p_strbuf, p_font );
    //���O
    PRINTSYS_Print( p_wk->p_bmp, SCROLL_BAR_FONT_PLAYER_X*GFL_BG_1CHRDOTSIZ+3, SCROLL_FONT_Y_OFS, cp_data->p_name, p_font );
    //�_��
    WORDSET_ExpandStr( p_wordset, p_strbuf, p_score_buf );
    PRINTSYS_Print( p_wk->p_bmp, SCROLL_BAR_FONT_SCORE_X*GFL_BG_1CHRDOTSIZ-3, SCROLL_FONT_Y_OFS, p_strbuf, p_font );
    //��
    WORDSET_ExpandStr( p_wordset, p_strbuf, p_count_buf );
    PRINTSYS_Print( p_wk->p_bmp, SCROLL_BAR_FONT_COUNT_X*GFL_BG_1CHRDOTSIZ+4, SCROLL_FONT_Y_OFS, p_strbuf, p_font );

    GFL_STR_DeleteBuffer( p_strbuf );
    GFL_STR_DeleteBuffer( p_count_buf );
    GFL_STR_DeleteBuffer( p_score_buf );
    GFL_STR_DeleteBuffer( p_rank_buf );
  }

  //�|�P�A�C�R�����\�[�X�ǂݍ���
  { 
    ARCDATID  cgx_dat = POKEICON_GetCgxArcIndexByMonsNumber( cp_data->mons_no, cp_data->form_no, cp_data->sex, cp_data->egg );
    ARCDATID  cel_dat = POKEICON_GetCellArcIndex();
    ARCDATID  anm_dat = POKEICON_GetAnmArcIndex();

    ARCHANDLE* p_handle  = GFL_ARC_OpenDataHandle( ARCID_POKEICON, heapID );
    p_wk->chr_idx = GFL_CLGRP_CGR_Register( p_handle, cgx_dat, FALSE, CLSYS_DRAW_MAX, heapID );
    p_wk->cel_idx = GFL_CLGRP_CELLANIM_Register( p_handle, cel_dat, anm_dat, heapID );

    GFL_ARC_CloseDataHandle( p_handle );
  }

  //OBJ�쐬
  { 
    int i;
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.anmseq = POKEICON_ANM_HPMAX;
    cldata.bgpri  = 1;

    for( i = 0; i < CLSYS_DEFREND_NUM; i++  )
    {
      p_wk->p_icon[i]  = GFL_CLACT_WK_Create( p_clunit,
          p_wk->chr_idx,
          icon_plt_idx,
          p_wk->cel_idx,
          &cldata,
          i,
          heapID
          );

      GFL_CLACT_WK_SetPlttOffs( p_wk->p_icon[i], 
          POKEICON_GetPalNum( cp_data->mons_no, cp_data->form_no, cp_data->sex, cp_data->egg )
          ,CLWK_PLTTOFFS_MODE_OAM_COLOR );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[i], FALSE );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�[�P���j��
 *
 *	@param	RANKBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void RANKBAR_Exit( RANKBAR_WORK *p_wk )
{ 
  //OBJ�j��
  { 
    int i;
    for( i = 0; i < CLSYS_DEFREND_NUM; i++  )
    {
      GFL_CLACT_WK_Remove( p_wk->p_icon[i] );
    }
  }
  
  //���\�[�X�j��
  { 
    GFL_CLGRP_CGR_Release( p_wk->chr_idx );
    GFL_CLGRP_CELLANIM_Release( p_wk->cel_idx );
  }

  GFL_BMP_Delete( p_wk->p_bmp );

  if( p_wk->p_bmpwin[ RANKBAR_DISPLAY_M ] )
  { 
    RANKBAR_Clear( p_wk, RANKBAR_DISPLAY_M );
  }
  if( p_wk->p_bmpwin[ RANKBAR_DISPLAY_S ] )
  { 
    RANKBAR_Clear( p_wk, RANKBAR_DISPLAY_S );
  }

  GFL_STD_MemClear( p_wk, sizeof(RANKBAR_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �]��  
 *
 *	@param	RANKBAR_WORK *p_wk  ���[�N
 *	@param	y                   Y�ʒu
 */
//-----------------------------------------------------------------------------
static void RANKBAR_Trans( RANKBAR_WORK *p_wk, RANKBAR_DISPLAY display, u8 font_frm, u8 bar_frm, u16 y, u8 font_plt )
{ 
  if( p_wk->p_bmpwin[ display ] )
  { 
    RANKBAR_Clear( p_wk, display );
  }

  p_wk->p_bmpwin[ display ]  = GFL_BMPWIN_Create( font_frm, 0, y, SCROLL_BAR_FONT_WIDTH, SCROLL_BAR_HEIGHT, font_plt, GFL_BMP_CHRAREA_GET_B );
  GFL_BMP_Copy( p_wk->p_bmp, GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[ display ]) );
  GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[ display ] );
	GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin[ display ] );

  BARBUFF_Trans( p_wk->p_buff, bar_frm, y );

  { 
    GFL_CLACTPOS pos;
    pos.x   = SCROLL_BAR_ICON_X;
    pos.y   = SCROLL_BAR_ICON_Y + y * GFL_BG_1CHRDOTSIZ;

    if( bar_frm < GFL_BG_FRAME0_S )
    { 
      GFL_CLACT_WK_SetPos( p_wk->p_icon[CLSYS_DEFREND_MAIN], &pos, CLSYS_DEFREND_MAIN );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[CLSYS_DEFREND_MAIN], TRUE );
    }
    else
    { 
      GFL_CLACT_WK_SetPos( p_wk->p_icon[CLSYS_DEFREND_SUB], &pos, CLSYS_DEFREND_SUB );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[CLSYS_DEFREND_SUB], TRUE );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����
 *
 *	@param	RANKBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void RANKBAR_Clear( RANKBAR_WORK *p_wk, RANKBAR_DISPLAY display )
{
  GF_ASSERT( p_wk->p_bmpwin[ display ] );

  if( GFL_BMPWIN_GetFrame(p_wk->p_bmpwin[ display ]) < GFL_BG_FRAME0_S )
  { 
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[CLSYS_DEFREND_MAIN], FALSE );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[CLSYS_DEFREND_SUB], FALSE );
  }

  BARBUFF_Clear( p_wk->p_buff,
      GFL_BMPWIN_GetFrame(p_wk->p_bmpwin[ display ]), 
      GFL_BMPWIN_GetPosY(p_wk->p_bmpwin[ display ] ) );

  GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin[ display ] );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin[ display ] );
  p_wk->p_bmpwin[ display ]  = NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ړ�
 *
 *	@param	RANKBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void RANKBAR_Move( RANKBAR_WORK *p_wk )
{ 

}
//=============================================================================
/**
 *  BARBUFF
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �o�[�������ݗp���[�N  ������
 *
 *	@param	BAR_BUFF *p_wk  ���[�N
 *	@param	heapID          �q�[�vID
 */
//-----------------------------------------------------------------------------
static void BARBUFF_Init( BAR_BUFF *p_wk, const GRAPHIC_BG_WORK *cp_bg, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(BAR_BUFF) );

  //bar�p�X�N���[�����Ă�
  { 
    int i, j;
    void *p_src_arc;
    NNSG2dScreenData* p_scr_data;
    u16 *p_scr;

    u16 start_char_num_m	= GRAPHIC_BG_GetTransInfo( cp_bg, TRUE );
		u16 start_char_num_s	= GRAPHIC_BG_GetTransInfo( cp_bg, FALSE );

    //�ǂݍ���
    p_src_arc = GFL_ARC_UTIL_LoadScreen(ARCID_IRCRANKING_GRAPHIC, NARC_ircranking_gra_frame_NSCR, FALSE, &p_scr_data, heapID );

    p_scr = (u16*)p_scr_data->rawData;

    for( i = 0; i < SCROLL_BAR_HEIGHT; i++ )
    {
      for( j = 0; j < SCROLL_BAR_WIDTH; j++ )
      { 
        p_wk->rank_screen_main[i*SCROLL_BAR_WIDTH+j] = 
          p_scr[ i * SCROLL_BAR_WIDTH + j ] + start_char_num_m;
        p_wk->rank_screen_sub[i*SCROLL_BAR_WIDTH+j] = 
          p_scr[ i * SCROLL_BAR_WIDTH + j ] + start_char_num_s;
      }
    }

    GFL_HEAP_FreeMemory( p_src_arc );
  }


}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�[�������ݗp���[�N  �j��
 *
 *	@param	BAR_BUFF *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void BARBUFF_Exit( BAR_BUFF *p_wk )
{ 

  GFL_STD_MemClear( p_wk, sizeof(BAR_BUFF) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�[�������ݗp���[�N  �]��
 *
 *	@param	BAR_BUFF *p_wk  ���[�N
 *	@param  frm             BG��
 *	@param	y               �]���ʒu
 */
//-----------------------------------------------------------------------------
static void BARBUFF_Trans( BAR_BUFF *p_wk, u8 frm, u16 y )
{ 
  u16 *p_buff;

  if( frm < GFL_BG_FRAME0_S )
  { 
    p_buff  = p_wk->rank_screen_main;
  }
  else
  { 
    p_buff  = p_wk->rank_screen_sub;
  }

  GFL_BG_WriteScreen( frm, p_buff, 0, y, SCROLL_BAR_WIDTH, SCROLL_BAR_HEIGHT );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�[�������ݗp���[�N  �N���A�[
 *
 *	@param	BAR_BUFF *p_wk  ���[�N
 *	@arapm  frm             BG��
 *	@param	y               �N���A�ʒu
 */
//-----------------------------------------------------------------------------
static void BARBUFF_Clear( BAR_BUFF *p_wk, u8 frm, u16 y )
{ 
  GFL_BG_FillScreen( frm, 0, 0, y, SCROLL_BAR_WIDTH, SCROLL_BAR_HEIGHT, GFL_BG_SCRWRT_PALNL );
}
//=============================================================================
/**
 *	RANKING_DATA
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����L���O�f�[�^�쐬
 *
 *	@param	u16 data_len	�f�[�^��
 *	@param	heapID				�q�[�vID
 *
 *	@return	�f�[�^
 */
//-----------------------------------------------------------------------------
static RANKING_DATA	*RANKING_DATA_Create( GAMEDATA *p_gamedata, HEAPID heapID )
{	
	RANKING_DATA	          *p_wk;
  SAVE_CONTROL_WORK       *p_sv_ctrl;
  IRC_COMPATIBLE_SAVEDATA * p_sv;
  u16 rank_max;
  u32 size;

#ifdef PM_DEBUG
  if( p_gamedata == NULL )
  { 
    p_sv_ctrl = SaveControl_GetPointer();
  }
  else
#endif
  { 
    p_sv_ctrl = GAMEDATA_GetSaveControlWork( p_gamedata );
  }

  p_sv	= IRC_COMPATIBLE_SV_GetSavedata( p_sv_ctrl );
  rank_max	= IRC_COMPATIBLE_SV_GetRankNum( p_sv );


  size  = sizeof( RANKING_DATA ) + sizeof(RANKING_ONE_DATA) * rank_max;

	//�m��
	p_wk	= GFL_HEAP_AllocMemory( heapID, size );
	GFL_STD_MemClear( p_wk, size );
  p_wk->rank_num  = rank_max;

	//�f�[�^�ǂݍ���
  if( p_wk->rank_num > 0 )
	{	

		u8	new_rank;
		int i;
		u8	pre_score	= 128;	//100�ȏ�̂�����
		u8	pre_rank	= 0;
    u8  sex;
    RANKING_ONE_DATA  *p_data;

		new_rank	= IRC_COMPATIBLE_SV_GetNewRank( p_sv );
		for( i = 0; i < p_wk->rank_num; i++ )
		{	
      p_data  = &p_wk->data[i];

			//���O
			p_data->p_name	= GFL_STR_CreateBuffer( IRC_COMPATIBLE_SV_DATA_NAME_LEN, heapID );
			GFL_STR_SetStringCode( p_data->p_name, IRC_COMPATIBLE_SV_GetPlayerName( p_sv, i ) );	
			//�X�R�A
			p_data->score	= IRC_COMPATIBLE_SV_GetScore( p_sv, i );
				
			//��
			p_data->play_cnt	= IRC_COMPATIBLE_SV_GetPlayCount( p_sv, i );
			//�����N
			//�_�������Ȃ炻�̂Ƃ��̃����N
			//�����Ȃ�Γ������N
			if( p_data->score < pre_score )
			{	
				p_data->rank	= i+1;
			}
			else if( p_data->score == pre_score )
			{	
				p_data->rank	= pre_rank;
			}
			else
			{	
				GF_ASSERT(0);
			}
			//�p���b�g
      sex = IRC_COMPATIBLE_SV_GetSex( p_sv, i );
			if( new_rank == i )
			{	
				p_data->plt		= 5 + !sex * 5;
			}else if( p_data->rank == 1 )
			{	
				p_data->plt		= 2 + !sex * 5;
			}
			else if( p_data->rank == 2 )
			{	
				p_data->plt		= 3 + !sex * 5;
			}
			else if( p_data->rank == 3 )
			{	
				p_data->plt		= 4 + !sex * 5;
			}
			else
			{	
				p_data->plt		= 1 + !sex * 5;
			}

			//�O�̐l�̃X�R�A�ۑ�
			pre_score	= p_data->score;
			pre_rank	= p_data->rank;
		}
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����L���O�f�[�^�j��
 *
 *	@param	RANKING_DATA	*p_data		�f�[�^
 *
 */
//-----------------------------------------------------------------------------
static void RANKING_DATA_Delete( RANKING_DATA	*p_wk )
{
  u32 rank_max;
  int i;

  RANKING_ONE_DATA  *p_data;

  if( p_wk->rank_num > 0 )
  { 
    for( i = 0; i < p_wk->rank_num; i++ )
    {	
      p_data  = &p_wk->data[i];
      if( p_data->p_name )
      {	
        GFL_STR_DeleteBuffer( p_data->p_name );
      }
    }
  }

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����N�̐��擾
 *
 *	@param	const RANKING_DATA	*cp_wk  ���[�N
 *
 *	@return �����N�̐����擾
 */
//-----------------------------------------------------------------------------
static u32 RANKING_DATA_GetRankNum( const RANKING_DATA	*cp_wk )
{ 
  return cp_wk->rank_num;
}
//=============================================================================
/**
 *	UI
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	UI������
 *
 *	@param	UI_WORK *p_wk	���[�N
 *	@param	heapID				�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void UI_Init( UI_WORK *p_wk, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	UI�j��
 *
 *	@param	UI_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void UI_Exit( UI_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	UI���C������
 *
 *	@param	UI_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void UI_Main( UI_WORK *p_wk )
{	
	u32 x, y;
	//�^�b�`�̏���
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		p_wk->start.x		= x;
		p_wk->start.y		= y;
		p_wk->end.x		= x;
		p_wk->end.y		= y;
    p_wk->drag_start.x  = x;
    p_wk->drag_start.y  = y;
    p_wk->drag_pos.x  = 0;
    p_wk->drag_pos.y  = 0;

		p_wk->flik_sync	= 0;
	}
	else if(GFL_UI_TP_GetPointCont( &x, &y ) )
	{	
    if( MATH_IAbs( y - p_wk->start.y ) >= DRAG_MOVE_INIT_DISTANCE )
    {
      p_wk->drag_pos.x  = x - p_wk->drag_start.x;
      p_wk->drag_pos.y  = y - p_wk->drag_start.y;
      p_wk->drag_start.x  = x;
      p_wk->drag_start.y  = y;
    }

		p_wk->end.x		= x;
		p_wk->end.y		= y;

		p_wk->flik_sync++;
		p_wk->release_sync	= 0;
	}
	else
	{	
		p_wk->release_sync++;
	}

	//�L�[�̏���
	if( GFL_UI_KEY_GetTrg() )
	{
		p_wk->key_cont_sync	= 0;
	}
	else if( GFL_UI_KEY_GetCont() )
	{	
		p_wk->key_cont_sync++;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	UI	�h���b�O���擾
 *
 *	@param	const UI_WORK *p_wk	���[�N
 *	@param	*p_start						�J�n�_
 *	@param	*p_end							�I���_
 *	@param	*p_vec							�x�N�g��
 *
 */
//-----------------------------------------------------------------------------
static BOOL UI_GetDrag( UI_WORK *p_wk, GFL_POINT *p_start, GFL_POINT *p_end, GFL_POINT *p_drag_pos )
{	
	if( GFL_UI_TP_GetCont() )
	{	
		if( p_start )
		{	
			*p_start	= p_wk->start;
		}
		if( p_end )
		{	
			*p_end	  = p_wk->end;
		}
		if( p_drag_pos )
		{	
      *p_drag_pos = p_wk->drag_pos;
		}
		return TRUE;
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	UI	�͂������擾
 *
 *	@param	const UI_WORK *p_wk	���[�N
 *	@param	*p_start	�J�n�_
 *	@param	*p_end		�I���_
 *	@param	*p_vec		�x�N�g��
 *	@param	*p_sync		�͂����ɂ��������V���N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL UI_GetFlik( UI_WORK *p_wk, GFL_POINT *p_start, GFL_POINT *p_end, VecFx32 *p_vec, u32 *p_sync )
{	
	if( !GFL_UI_KEY_GetCont() && p_wk->release_sync < UI_FLIK_RELEASE_SYNC )
	{	
		//1�񂵂��擾�ł��Ȃ��悤��
		p_wk->release_sync	= UI_FLIK_RELEASE_SYNC;
		if( p_start )
		{	
			*p_start	= p_wk->start;
		}
		if( p_end )
		{	
			*p_end	= p_wk->end;
		}
		if( p_vec )
		{	
			VecFx32	s, e;
			VEC_Set( &s, FX32_CONST( p_wk->start.x ), FX32_CONST( p_wk->start.y ), 0 );
			VEC_Set( &e, FX32_CONST( p_wk->end.x ), FX32_CONST( p_wk->end.y ), 0 );
			VEC_Subtract( &s, &e, p_vec );
		}
		if( p_sync )
		{	
			*p_sync	= p_wk->flik_sync;
		}
		return TRUE;
	}
	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�L�[�����Â����擾
 *
 *	@param	const UI_WORK *cp_wk	���[�N
 *	@param	*p_sync								���������Ă��鎞��
 *
 *	@return	TRUE�Ȃ牟���Ă���
 */
//-----------------------------------------------------------------------------
static int UI_IsContKey( const UI_WORK *cp_wk, u32 *p_sync )
{	
	if( p_sync )
	{	
		*p_sync	= cp_wk->key_cont_sync;
	}
	return GFL_UI_KEY_GetCont();
}
//=============================================================================
/**
 *	ACLR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	������
 *
 *	@param	ACLR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_Init( ACLR_WORK *p_wk )
{	
	GFL_STD_MemClear(p_wk, sizeof(ACLR_WORK));
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	�j��
 *
 *	@param	ACLR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_Exit( ACLR_WORK *p_wk )
{	
	GFL_STD_MemClear(p_wk, sizeof(ACLR_WORK));
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	���C������
 *
 *	@param	ACLR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_Main( ACLR_WORK *p_wk )
{	
	if( p_wk->is_update )
	{
		p_wk->is_update	= FALSE;
		p_wk->aclr	= p_wk->distance_rate	+ p_wk->sync_rate / 2;
		p_wk->aclr	= FX_Mul( p_wk->aclr, ACLR_SCROLL_ACLR_DIF ) + ACLR_SCROLL_ACLR_MIN;
	}
	//NAGI_Printf( "now aclr %d\n", p_wk->aclr );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	���������߂�l���Z�b�g
 *
 *	@param	ACLR_WORK *p_wk	���[�N
 *	@param	distance				����
 *	@param	sync						�V���N
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_SetAclr( ACLR_WORK *p_wk, fx32 distance, u32 sync )
{	
	//����E�����ȊO�Ȃ�΂͂���
	if( ACLR_SCROLL_DISTANCE_MIN < MATH_IAbs(distance) )
	{	
		p_wk->dir	= distance/ MATH_IAbs(distance);
		distance	= MATH_IAbs( distance );
		distance	= MATH_CLAMP( distance, ACLR_SCROLL_DISTANCE_MIN, ACLR_SCROLL_DISTANCE_MAX );
	}
	else
	{
		return ;
	}
	if( FX32_CONST(sync) < ACLR_SCROLL_SYNC_MAX ) 
	{	
		sync			= MATH_CLAMP( FX32_CONST(sync), ACLR_SCROLL_SYNC_MIN, ACLR_SCROLL_SYNC_MAX );
	}
	else
	{
		return ;
	}

	//���ꂼ��̃��[�g���v�Z
	p_wk->distance_rate	=	FX_Div((distance - ACLR_SCROLL_DISTANCE_MIN), ACLR_SCROLL_DISTANCE_DIF);
	//sync��MIN�̕����ǂ��l�i=Rate�ł�MAX�ɋ߂��������j�̂Ŕ��]����
	p_wk->sync_rate	=	FX_Div((sync - ACLR_SCROLL_SYNC_MIN), ACLR_SCROLL_SYNC_DIF);
	p_wk->sync_rate	= FX32_ONE - p_wk->sync_rate;

	//OS_Printf( "����%f\n �V���N%f\n", FX_FX32_TO_F32(distance), FX_FX32_TO_F32(sync) );
	//OS_Printf( "rate dis%f sync%f\n", FX_FX32_TO_F32(p_wk->distance_rate), FX_FX32_TO_F32(p_wk->sync_rate) );

	p_wk->is_update	= TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	��~����
 *
 *	@param	p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_Stop( ACLR_WORK *p_wk )
{	
	p_wk->aclr	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	�X�N���[�������x���擾
 *
 *	@param	const ACLR_WORK *cp_wk	���[�N
 *
 *	@return	�X�N���[�������x
 */
//-----------------------------------------------------------------------------
static s16 ACLR_GetScrollAdd( ACLR_WORK *p_wk )
{	
	
	fx32 scroll_add;
	fx32 aclr_dec;

	//�X�N���[�����x
	scroll_add	= MATH_CLAMP( p_wk->aclr*p_wk->dir, ACLR_SCROLL_MOVE_MIN, ACLR_SCROLL_MOVE_MAX );

	//����
	aclr_dec		= FX_Mul(p_wk->aclr, ACLR_SCROLL_DEC_RATE);
	p_wk->aclr	-= aclr_dec;
	if( -FX32_ONE <p_wk->aclr && p_wk->aclr < FX32_ONE)
	{	
		p_wk->aclr	= 0;
	}
	//OS_Printf("�X�N���[��Add %d\n",  scroll_add>>FX32_SHIFT );

	return (scroll_add >> FX32_SHIFT);
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	���������݂��邩
 *
 *	@param	const ACLR_WORK *cp_wk	���[�N
 *
 *	@return	TRUE�Ȃ�Ή���������
 */
//-----------------------------------------------------------------------------
static BOOL ACLR_IsExist( const ACLR_WORK *cp_wk )
{	
	return cp_wk->aclr != 0;
}
//=============================================================================
/**
 *	ETC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���^�[�ʒu�փv�����g
 *
 *	@param	GFL_BMPWIN *p_bmpwin	BMPWIN
 *	@param	*p_strbuf							����
 *	@param	*p_font								�t�H���g
 *
 */
//-----------------------------------------------------------------------------
static void PRINT_PrintCenter( GFL_BMPWIN *p_bmpwin, STRBUF *p_strbuf, GFL_FONT *p_font )
{	
	u16 x, y;

	x	= GFL_BMPWIN_GetSizeX( p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_strbuf, p_font )/2;

	PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_bmpwin), x, y, p_strbuf, p_font );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �Z���^�[�ʒu�ւ̃v�����g
 *
 *	@param	GFL_BMPWIN *p_bmpwin	BMPWIN
 *	@param	*p_strbuf							����
 *	@param	MYSTATUS *cp_status   �X�e�[�^�X
 *	@param	*p_font								�t�H���g
 */
//-----------------------------------------------------------------------------
static void PRINT_PrintNameCenter( GFL_BMPWIN *p_bmpwin, STRBUF *p_strbuf, const MYSTATUS *cp_status, GFL_FONT *p_font )
{ 
  if( cp_status == NULL )
  { 
	u16 x, y;

	x	= GFL_BMPWIN_GetSizeX( p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_strbuf, p_font )/2;

	PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_bmpwin), x, y, p_strbuf, p_font );
  }
  else
  { 

	u16 x, y;
  STRBUF *p_dst;
  WORDSET *p_word = WORDSET_Create( GFL_HEAP_LOWID(HEAPID_IRCRANKING) );

  p_dst = GFL_STR_CreateBuffer( 128, GFL_HEAP_LOWID(HEAPID_IRCRANKING) );
  WORDSET_RegisterPlayerName( p_word, 0, cp_status );
  WORDSET_ExpandStr( p_word, p_dst, p_strbuf );

	x	= GFL_BMPWIN_GetSizeX( p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_dst, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_dst, p_font )/2;


	PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_bmpwin), x, y, p_dst, p_font );

  GFL_STR_DeleteBuffer( p_dst );
  WORDSET_Delete( p_word );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	BMP�̃L�����P�ʃR�s�[�����iPRINT�̓h�b�g�P�ʂŃR�s�[����̂ł�葁���j
 *
 *	@param	const GFL_BMP_DATA *cp_src	�]����
 *	@param	*p_dst	�]����
 *	@param	src_x		�]����X���W�i�ȉ����ׂăL�����P�ʁj
 *	@param	src_y		�]����Y���W
 *	@param	dst_x		�]����X���W	
 *	@param	dst_y		�]����Y���W
 *	@param	w				�]����
 *	@param	h				�]������
 *
 */
//-----------------------------------------------------------------------------
static void BMP_Copy( const GFL_BMP_DATA *cp_src, GFL_BMP_DATA *p_dst, u16 src_x, u16 src_y, u16 dst_x, u16 dst_y, u16 w, u16 h )
{	
	const u8	*cp_src_chr		= (const u8 *)GFL_BMP_GetCharacterAdrs( (GFL_BMP_DATA *)(cp_src) );
	u8	*p_dst_chr					= (u8 *)GFL_BMP_GetCharacterAdrs( p_dst );
	const u8 *cp_src_chr_ofs;
	u8 *p_dst_chr_ofs;

	cp_src_chr_ofs	= cp_src_chr + ((w * src_y) + src_x)*GFL_BG_1CHRDATASIZ;
	p_dst_chr_ofs		= p_dst_chr	+ ((w * dst_y) + dst_x)*GFL_BG_1CHRDATASIZ;

	GFL_STD_MemCopy32( cp_src_chr_ofs, p_dst_chr_ofs, w * h * GFL_BG_1CHRDATASIZ );
}

//----------------------------------------------------------------------------
/**
 *	@brief	POINT�ŋ����𑪂�
 *
 *	@param	const GFL_POINT *cp_a
 *	@param	GFL_POINT *cp_b 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static s32 GFL_POINT_Distance( const GFL_POINT *cp_a, const GFL_POINT *cp_b )
{	
	VecFx32	a;
	VecFx32	b;

	VEC_Set( &a, FX32_CONST(cp_a->x), FX32_CONST(cp_a->y), 0);
	VEC_Set( &b, FX32_CONST(cp_b->x), FX32_CONST(cp_b->y), 0);

	return VEC_Distance( &a, &b ) >> FX32_SHIFT;
}
//----------------------------------------------------------------------------
/**
 *	@brief	Screen�f�[�^�́@VRAM�]���g���Łi�ǂݍ��񂾃X�N���[���̈ꕔ�͈͂��o�b�t�@�ɒ������j
 *
 *	@param	ARCHANDLE *handle	�n���h��
 *	@param	datID							�f�[�^ID
 *	@param	frm								�t���[��
 *	@param	chr_ofs						�L�����I�t�Z�b�g
 *	@param	src_x							�]���������W
 *	@param	src_y							�]����Y���W
 *	@param	src_w							�]������			//�f�[�^�̑S�̂̑傫��
 *	@param	src_h							�]��������		//�f�[�^�̑S�̂̑傫��
 *	@param	dst_x							�]����X���W
 *	@param	dst_y							�]����Y���W
 *	@param	dst_w							�]���敝
 *	@param	dst_h							�]���捂��
 *	@param	plt								�p���b�g�ԍ�
 *	@param	compressedFlag		���k�t���O
 *	@param	heapID						�ꎞ�o�b�t�@�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID )
{	
	void *p_src_arc;
	NNSG2dScreenData* p_scr_data;

	//�ǂݍ���
	p_src_arc = GFL_ARCHDL_UTIL_Load( handle, datID, compressedFlag, GetHeapLowID(heapID) );

	//�A���p�b�N
	if(!NNS_G2dGetUnpackedScreenData( p_src_arc, &p_scr_data ) )
	{	
		GF_ASSERT(0);	//�X�N���[���f�[�^����Ȃ���
	}

	//�G���[
	GF_ASSERT( src_w * src_h * 2 <= p_scr_data->szByte );

	//�L�����I�t�Z�b�g�v�Z
	if( chr_ofs )
	{	
		int i;
		if( GFL_BG_GetScreenColorMode( frm ) == GX_BG_COLORMODE_16 )
		{
			u16 *p_scr16;
			
			p_scr16	=	(u16 *)&p_scr_data->rawData;
			for( i = 0; i < src_w * src_h ; i++ )
			{
				p_scr16[i]	+= chr_ofs;
			}	
		}
		else
		{	
			GF_ASSERT(0);	//256�ł͍���ĂȂ�
		}
	}

	//��������
	if( GFL_BG_GetScreenBufferAdrs( frm ) != NULL )
	{	
		GFL_BG_WriteScreenExpand( frm, dst_x, dst_y, dst_w, dst_h,
				&p_scr_data->rawData, src_x, src_y, src_w, src_h );	
		GFL_BG_ChangeScreenPalette( frm, dst_x, dst_y, dst_w, dst_h, plt );
		GFL_BG_LoadScreenReq( frm );
	}
	else
	{	
		GF_ASSERT(0);	//�o�b�t�@���Ȃ�
	}

	GFL_HEAP_FreeMemory( p_src_arc );
}	
