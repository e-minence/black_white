//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_aura_main.c
 *	@brief	�ԊO���~�j�Q�[��	�I�[���`�F�b�N
 *	@author	Toru=Nagihashi
 *	@data		2009.05.11
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

//	archive
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_irc_aura.h"
#include "ircaura_gra.naix"

//	aura
#include "net_app/irc_aura.h"

#ifdef PM_DEBUG
//debug�p
#include "system/net_err.h"	//VRAM�ޔ�p�A�h���X��Ⴄ����
#include <wchar.h>					//wcslen
#endif

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#ifdef PM_DEBUG
#define DEBUG_AURA_MSG	//�f�o�b�O���b�Z�[�W���o��
#define DEBUG_ONLY_PLAY	//�f�o�b�O�p���Ńv���C���郂�[�h���o��
#endif //PM_DEBUG


//-------------------------------------
///	�p���b�g
//=====================================
enum{	
	// ���C�����BG
	AURA_BG_PAL_M_00 = 0,//GUIDE�pBG
	AURA_BG_PAL_M_01,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_02,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_03,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_04,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_05,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_06,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_07,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_08,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_09,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_10,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_11,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_12,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_13,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_14,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_15,		// INFOWIN


	// �T�u���BG
	AURA_BG_PAL_S_00 = 0,	//�t�H���g
	AURA_BG_PAL_S_01,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_02,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_03,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_04,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_05,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_06,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_07,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_08,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_09,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_10,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_11,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_12,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_13,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_14,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_15,		// �g�p���ĂȂ�

	// ���C�����OBJ
	AURA_OBJ_PAL_M_00 = 0,//GUIDE�pOBJ
	AURA_OBJ_PAL_M_01,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_02,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_03,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_04,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_05,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_06,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_07,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_08,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_09,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_10,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_11,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_12,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_13,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_14,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_M_15,		// �g�p���ĂȂ�


	// �T�u���OBJ
	AURA_OBJ_PAL_S_00 = 0,	//�g�p���ĂȂ�
	AURA_OBJ_PAL_S_01,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_02,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_03,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_04,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_05,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_06,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_07,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_08,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_09,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_10,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_11,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_12,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_13,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_14,		// �g�p���ĂȂ�
	AURA_OBJ_PAL_S_15,		// �g�p���ĂȂ�
};

//-------------------------------------
///	���o�[
//=====================================
#define INFOWIN_PLT_NO		(AURA_BG_PAL_M_15)
#define INFOWIN_BG_FRAME	(GFL_BG_FRAME0_M)

//-------------------------------------
///	����
//=====================================
#define TEXTSTR_PLT_NO				(AURA_BG_PAL_S_00)
#define TEXTSTR_BUFFER_LENGTH	(255)

//-------------------------------------
///	�ʒu
//=====================================
#define	MSGTEXT_WND_X	(1)
#define	MSGTEXT_WND_Y	(18)
#define	MSGTEXT_WND_W	(30)
#define	MSGTEXT_WND_H	(5)

#define DEBUGMSG_LEFT1	(0)
#define DEBUGMSG_LEFT2	(20)
//#define DEBUGMSG_LEFT3	(40)
//#define DEBUGMSG_LEFT4	(60)

#define DEBUGMSG_RIGHT1	(40)
#define DEBUGMSG_RIGHT2	(60)
//#define DEBUGMSG_RIGHT3	(120)
//#define DEBUGMSG_RIGHT4	(140)

#define DEBUGMSG2_TAB			(170)
#define DEBUGMSG3_TAB			(85)

//-------------------------------------
///	�J�E���g
//=====================================
#define TOUCH_COUNTER_MAX	(60*5)
#define TOUCH_COUNTER_SHAKE_SYNC	(30)	//�u�����ϑ����邽�߂�1��̃V���N
#define TOUCH_COUNTER_SHAKE_MAX	(TOUCH_COUNTER_MAX/TOUCH_COUNTER_SHAKE_SYNC)	//�u�����擾�����

//-------------------------------------
///		MSG_FONT
//=====================================
typedef enum {	
	MSG_FONT_TYPE_LARGE,
	MSG_FONT_TYPE_SMALL,
}MSG_FONT_TYPE;

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

//-------------------------------------
///	�f�o�b�O�p�l���Z�[�u�@�\
//=====================================
#ifdef DEBUG_ONLY_PLAY
#define DEBUG_GAME_NUM	(5)
#define DEBUG_PLAYER_SAVE_NUM	(DEBUG_GAME_NUM*2)
#endif //DEBUG_ONLY_PLAY

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
	int dummy;
} GRAPHIC_BG_WORK;
//-------------------------------------
///	3D�`���
//=====================================
typedef struct 
{
	GFL_G3D_CAMERA		*p_camera;
} GRAPHIC_3D_WORK;
//-------------------------------------
///	OBJ�֌W
//=====================================
typedef struct {
	GFL_CLUNIT *p_clunit;
	u32					reg_id[OBJREGID_MAX];
	GFL_CLWK	 *p_clwk[CLWKID_MAX];
} GRAPHIC_OBJ_WORK;
//-------------------------------------
///	�`��֌W
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_3D_WORK		g3d;
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
typedef struct {
	GFL_BMPWIN*				p_bmpwin;
	PRINT_UTIL        print_util;
	STRBUF*						p_strbuf;
} MSGWND_WORK;

#ifdef DEBUG_AURA_MSG
//-------------------------------------
///	�f�o�b�O�v�����g�p���
//=====================================
typedef struct {
	GFL_BMP_DATA *p_bmp;
	GFL_FONT*			p_font;

	BOOL	is_now_save;
	u8	frm;

  u8  *p_char_temp_area;      ///<�L�����N�^VRAM�ޔ��
  u16 *p_scrn_temp_area;      ///<�X�N���[��VRAM�ޔ��
  u16 *p_pltt_temp_area;      ///<�p���b�gVRAM�ޔ��

 	u8  font_col_bkup[3];
  u8  prioryty_bkup;
  u8  scroll_x_bkup;
  u8  scroll_y_bkup;

	HEAPID heapID;
} DEBUG_PRINT_WORK;

#define DEBUGPRINT_CHAR_TEMP_AREA (0x4000)
#define DEBUGPRINT_SCRN_TEMP_AREA (0x800)
#define DEBUGPRINT_PLTT_TEMP_AREA (0x20)
#define DEBUGPRINT_WIDTH  (32)
#define DEBUGPRINT_HEIGHT (18)

static DEBUG_PRINT_WORK *sp_dp_wk;
#endif //DEBUG_AURA_MSG

//-------------------------------------
///	�u���v��
//=====================================
typedef struct {
	GFL_POINT		shake[TOUCH_COUNTER_SHAKE_MAX];
	u16					shake_idx;
	u32					cnt;				//�J�E���^
	u32					shake_cnt;	//�u���v���p�J�E���^
	BOOL				is_end;			//1��擾�����t���O
}SHAKE_SEARCH_WORK;


//-------------------------------------
///	�I�[���`�F�b�N���C�����[�N
//=====================================
typedef struct _AURA_MAIN_WORK AURA_MAIN_WORK;
typedef void (*SEQ_FUNCTION)( AURA_MAIN_WORK *p_wk, u16 *p_seq );
struct _AURA_MAIN_WORK
{
	//�O���t�B�b�N���W���[��
	GRAPHIC_WORK		grp;
	MSG_WORK				msg;
	MSGWND_WORK			msgwnd;

	//�V�[�P���X�Ǘ�
	SEQ_FUNCTION		seq_function;
	u16		seq;
	BOOL is_end;

	u16							debug_player;			//���������肩
	u16							debug_game_cnt;	//���Q�[���ڂ�

	//����
	GFL_POINT		trg_left[DEBUG_PLAYER_SAVE_NUM];
	GFL_POINT		trg_right[DEBUG_PLAYER_SAVE_NUM];
	SHAKE_SEARCH_WORK	shake_left[DEBUG_PLAYER_SAVE_NUM];
	SHAKE_SEARCH_WORK	shake_right[DEBUG_PLAYER_SAVE_NUM];

	//����
	IRC_AURA_PARAM	*p_param;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//proc
static GFL_PROC_RESULT IRC_AURA_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_AURA_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_AURA_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
//grp
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
static GFL_CLWK* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//BG
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//obj
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id );
//3d
static void GRAPHIC_3D_Init( GRAPHIC_3D_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_3D_Exit( GRAPHIC_3D_WORK *p_wk );
static void GRAPHIC_3D_StartDraw( GRAPHIC_3D_WORK *p_wk );
static void GRAPHIC_3D_EndDraw( GRAPHIC_3D_WORK *p_wk );
static void Graphic_3d_SetUp( void );
//MSG_WORK
static void MSG_Init( MSG_WORK *p_wk, MSG_FONT_TYPE font, HEAPID heapID );
static void MSG_Exit( MSG_WORK *p_wk );
static BOOL MSG_Main( MSG_WORK *p_wk );
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk );
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk );
static const GFL_MSGDATA * MSG_GetMsgDataConst( const MSG_WORK *cp_wk );
static WORDSET * MSG_GetWordSet( const MSG_WORK *cp_wk );
//MSG_WINDOW
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, MSG_WORK *p_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y );
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y );
static void MSGWND_Clear( MSGWND_WORK* p_wk );
//SEQ
static void SEQ_Change( AURA_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function );
static void SEQ_End( AURA_MAIN_WORK *p_wk );
//SEQ_FUNC
static void SEQFUNC_StartGame( AURA_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_TouchLeft( AURA_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_TouchRight( AURA_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_Result( AURA_MAIN_WORK *p_wk, u16 *p_seq );
//�ėp
static BOOL TP_GetRectTrg( const GFL_RECT *cp_rect, GFL_POINT *p_trg );
static BOOL TP_GetRectCont( const GFL_RECT *cp_rect, GFL_POINT *p_cont );
static void DEBUGAURA_PRINT_UpDate( AURA_MAIN_WORK *p_wk );
static void TouchMarker_SetPos( AURA_MAIN_WORK *p_wk, const GFL_POINT *cp_pos );
static void TouchMarker_OffVisible( AURA_MAIN_WORK *p_wk );

//�u���v��
static void	SHAKESEARCH_Init( SHAKE_SEARCH_WORK *p_wk );
static void	SHAKESEARCH_Exit( SHAKE_SEARCH_WORK *p_wk );
static BOOL	SHAKESEARCH_Main( SHAKE_SEARCH_WORK *p_wk, const GFL_RECT	*cp_rect );
//DEBUG_PRINT
#ifdef DEBUG_AURA_MSG
static void DEBUGPRINT_Init( u8 frm, BOOL is_now_save, HEAPID heapID );
static void DEBUGPRINT_Exit( void );
static void DEBUGPRINT_Open( void );
static void DEBUGPRINT_Close( void );
static void DEBUGPRINT_Print( const u16 *cp_str, u16 x, u16 y );
static void DEBUGPRINT_PrintNumber( const u16 *cp_str, int number, u16 x, u16 y );
static void DEBUGPRINT_Clear( void );
#else
#define DEBUGPRINT_Init(...)				((void)0)
#define DEBUGPRINT_Exit(...)				((void)0)
#define DEBUGPRINT_Open(...)				((void)0)
#define DEBUGPRINT_Close(...)				((void)0)
#define DEBUGPRINT_Print(...)				((void)0)
#define DEBUGPRINT_PrintNumber(...) ((void)0)
#define DEBUGPRINT_Clear(...)				((void)0)
#endif //DEBUG_AURA_MSG

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�I�[���`�F�b�N�p�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA IrcAura_ProcData	= 
{	
	IRC_AURA_PROC_Init,
	IRC_AURA_PROC_Main,
	IRC_AURA_PROC_Exit,
};
//-------------------------------------
///	BG�V�X�e��
//=====================================
typedef enum 
{
	GRAPHIC_BG_FRAME_M_INFOWIN,
	GRAPHIC_BG_FRAME_M_GUIDE_L,
	GRAPHIC_BG_FRAME_M_GUIDE_R,
	GRAPHIC_BG_FRAME_S_TEXT,
	GRAPHIC_BG_FRAME_S_BACK,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	INFOWIN_BG_FRAME, GFL_BG_FRAME1_M, GFL_BG_FRAME2_M, GFL_BG_FRAME0_S, GFL_BG_FRAME1_S,
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
	// GRAPHIC_BG_FRAME_M_GUIDE_L
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_M_GUIDE_R
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_TEXT
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},

};

//-------------------------------------
///	�^�b�`�͈�
//=====================================
static const GFL_RECT	sc_left	=
{	
	0, 2*8, 11*8, 20*8
};
#if 0
static const GFL_RECT	sc_right	=
{	
	19, 2, 22, 20
};
#else	//2�_�^�b�`�Ȃ̂ŁA��̍��W�ƍ����W�̒��_
static const GFL_RECT	sc_right	=
{	
	11*8+1, 2*8, 22*8, 20*8
};
#endif


//=============================================================================
/**
 *			PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[���`�F�b�N	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_AURA_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	AURA_MAIN_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCAURA, 0x20000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(AURA_MAIN_WORK), HEAPID_IRCAURA );
	GFL_STD_MemClear( p_wk, sizeof(AURA_MAIN_WORK) );
	p_wk->p_param	= p_param;

	//���W���[��������
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCAURA );
	MSG_Init( &p_wk->msg, MSG_FONT_TYPE_LARGE, HEAPID_IRCAURA );
	INFOWIN_Init( INFOWIN_BG_FRAME, INFOWIN_PLT_NO, HEAPID_IRCAURA );
	MSGWND_Init( &p_wk->msgwnd, sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TEXT],
			MSGTEXT_WND_X, MSGTEXT_WND_Y, MSGTEXT_WND_W, MSGTEXT_WND_H, HEAPID_IRCAURA );

	{	
		int i;
		for( i = 0; i < DEBUG_PLAYER_SAVE_NUM; i++ )
		{	
			SHAKESEARCH_Init( &p_wk->shake_left[ i ] );
			SHAKESEARCH_Init( &p_wk->shake_right[ i ] );
		}
	}

	//�f�o�b�O
	DEBUGPRINT_Init( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], FALSE, HEAPID_IRCAURA );
	DEBUGPRINT_Open();


	SEQ_Change( p_wk, SEQFUNC_StartGame );
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[���`�F�b�N	���C���v���Z�X�j������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_AURA_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	AURA_MAIN_WORK	*p_wk;

	p_wk	= p_work;

	//�f�o�b�O�j��
	DEBUGPRINT_Close();
	DEBUGPRINT_Exit();

	//���W���[���j��
	MSGWND_Exit( &p_wk->msgwnd );
	INFOWIN_Exit();
	GRAPHIC_Exit( &p_wk->grp );
	MSG_Exit( &p_wk->msg );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_IRCAURA );
	
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[���`�F�b�N	���C���v���Z�X���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_AURA_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
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

	AURA_MAIN_WORK	*p_wk;
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
		GF_ASSERT_MSG( 0, "IRC_AURA_PROC_Main��SEQ�G���[ %d", *p_seq );
	}

	INFOWIN_Update();
	if( MSG_Main( &p_wk->msg ) )
	{	
		MSGWND_Main( &p_wk->msgwnd, &p_wk->msg );
	}
	GRAPHIC_Draw( &p_wk->grp );

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
		GX_VRAM_BG_128_A,						// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,						// ���C��2D�G���W����OBJ
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
//	GRAPHIC_3D_Init( &p_wk->g3d, heapID );

	//VBlackTask�o�^
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

//	GRAPHIC_3D_Exit( &p_wk->g3d );
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
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};	
		GFL_BG_SetBGMode( &sc_bg_sys_header );
	}

	//�a�f�R���g���[���ݒ�
	{
		int i;

		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{
			GFL_BG_SetBGControl( sc_bgcnt_frame[i], &sc_bgcnt_data[i], GFL_BG_MODE_TEXT );
			GFL_BG_ClearFrame( sc_bgcnt_frame[i] );
			GFL_BG_SetVisible( sc_bgcnt_frame[i], VISIBLE_ON );
		}
	}

	//�ǂݍ��ݐݒ�
	{	
		ARCHANDLE *p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCAURA_GRAPHIC, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircaura_gra_aura_bg_guide_NCLR,
				PALTYPE_MAIN_BG, PALTYPE_MAIN_BG*0x20, 0x20, heapID );
	
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_ircaura_gra_aura_bg_guide_NCGR,
				sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_GUIDE_L], 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_ircaura_gra_aura_bg_guide_l_NSCR,
				sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_GUIDE_L], 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_ircaura_gra_aura_bg_guide_r_NSCR,
				sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_GUIDE_R], 0, 0, FALSE, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}

	GFL_BG_SetVisible( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_GUIDE_R], VISIBLE_OFF );
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
			GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
		}
	}

	//���\�[�X�j��
	{	
		GFL_CLGRP_PLTT_Release( p_wk->reg_id[OBJREGID_TOUCH_PLT] );
		GFL_CLGRP_CGR_Release( p_wk->reg_id[OBJREGID_TOUCH_CHR] );
		GFL_CLGRP_CELLANIM_Release( p_wk->reg_id[OBJREGID_TOUCH_CEL] );
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

//=============================================================================
/**
 *					GRAPHIC_3D
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�RD���̏�����
 *
 *	@param	p_wk			���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_Init( GRAPHIC_3D_WORK *p_wk, HEAPID heapID )
{
	static const VecFx32 sc_CAMERA_PER_POS		= { 0,0,FX32_CONST(5) };
	static const VecFx32 sc_CAMERA_PER_UP			= { 0,FX32_ONE,0 };
	static const VecFx32 sc_CAMERA_PER_TARGET	= { 0,0,FX32_CONST( 0 ) };

	enum{	
		CAMERA_PER_FOVY	=	(40),
		CAMERA_PER_ASPECT =	(FX32_ONE * 4 / 3),
		CAMERA_PER_NEAR	=	(FX32_ONE * 1),
		CAMERA_PER_FER	=	(FX32_ONE * 800),
		CAMERA_PER_SCALEW	=(0),
	};

	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K,
			GFL_G3D_VMANLNK, GFL_G3D_PLT32K, 0, heapID, Graphic_3d_SetUp );
	p_wk->p_camera = GFL_G3D_CAMERA_CreatePerspective( CAMERA_PER_FOVY, CAMERA_PER_ASPECT,
				CAMERA_PER_NEAR, CAMERA_PER_FER, CAMERA_PER_SCALEW, 
				&sc_CAMERA_PER_POS, &sc_CAMERA_PER_UP, &sc_CAMERA_PER_TARGET, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�RD���̔j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_Exit( GRAPHIC_3D_WORK *p_wk )
{
	GFL_G3D_CAMERA_Delete( p_wk->p_camera );
	GFL_G3D_Exit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��J�n
 *	
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_StartDraw( GRAPHIC_3D_WORK *p_wk )
{	
	GFL_G3D_DRAW_Start();
	GFL_G3D_CAMERA_Switching( p_wk->p_camera );
	GFL_G3D_DRAW_SetLookAt();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��I��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_EndDraw( GRAPHIC_3D_WORK *p_wk )
{	
	GFL_G3D_DRAW_End();
}

//----------------------------------------------------------------------------
/**
 *	@brief	���Z�b�g�A�b�v�R�[���o�b�N�֐�
 */
//-----------------------------------------------------------------------------
static void Graphic_3d_SetUp( void )
{
	// �R�c�g�p�ʂ̐ݒ�(�\�����v���C�I���e�B�[)
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	G2_SetBG0Priority(0);

	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_HIGHLIGHT );
	G3X_AntiAlias( FALSE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );		// �A���t�@�u�����h�@�I��
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),31,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);

	// ���C�g�ݒ�
	{
		static const GFL_G3D_LIGHT sc_GFL_G3D_LIGHT[] = 
		{
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
		};
		int i;
		
		for( i=0; i<NELEMS(sc_GFL_G3D_LIGHT); i++ ){
			GFL_G3D_SetSystemLight( i, &sc_GFL_G3D_LIGHT[i] );
		}
	}

	//�����_�����O�X���b�v�o�b�t�@
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
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
				NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
		break;
	case MSG_FONT_TYPE_SMALL:
		p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_small_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
		break;
	default:
		GF_ASSERT_MSG( 0, "MSGFONT_ERRO %d", font );
	};

	p_wk->p_print_que = PRINTSYS_QUE_Create( heapID );

	p_wk->p_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_aura_dat, heapID );

	p_wk->p_wordset	= WORDSET_Create( heapID );

	{	
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], GX_RGB(31,31,31) );
	}
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
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe, u8 x, u8 y, u8 w, u8 h, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( bgframe, x, y, w, h, TEXTSTR_PLT_NO, GFL_BMP_CHRAREA_GET_B );
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( TEXTSTR_BUFFER_LENGTH, heapID );
	PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );
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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//������쐬
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//�\��
	PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font );
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
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y )
{
	const GFL_MSGDATA* cp_msgdata;
	WORDSET *p_wordset;
	
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//���W���[���擾
	p_wordset		= MSG_GetWordSet( cp_msg );
	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );

	//���l�����[�h�Z�b�g�ɓo�^
	WORDSET_RegisterNumber(	p_wordset, buff_id, number, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	
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
 *	@param	AURA_MAIN_WORK *p_wk	���[�N
 *	@param	seq_function					�V�[�P���X�֐�
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Change( AURA_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�I��
 *
 *	@param	AURA_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( AURA_MAIN_WORK *p_wk )
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
 *	@param	AURA_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_StartGame( AURA_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_INIT,
		SEQ_MAIN,
	};

	switch( *p_seq )
	{	
	case SEQ_INIT:

		DEBUGAURA_PRINT_UpDate( p_wk );

		if( p_wk->debug_player == 0 )
		{	
			MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, AURA_STR_000, 0, 0 );
		}
		else
		{	

			MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, AURA_DEBUG_001, 0, 0 );
		}

		{	
			int i;
			for( i = 0 ; i < DEBUG_PLAYER_SAVE_NUM; i++ )
			{
				p_wk->shake_left[i].cnt				= 0;
				p_wk->shake_left[i].shake_cnt	= 0;
				p_wk->shake_left[i].shake_idx	= 0;
			}
		}

		TouchMarker_OffVisible( p_wk );
		GFL_BG_SetVisible( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_GUIDE_R], VISIBLE_OFF );
		GFL_BG_SetVisible( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_GUIDE_L], VISIBLE_ON );

		*p_seq	= SEQ_MAIN;
		break;

	case SEQ_MAIN:
		if( TP_GetRectTrg( &sc_left, &p_wk->trg_left[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ] ) )
		{	
			DEBUGAURA_PRINT_UpDate( p_wk );
			MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, AURA_STR_001, 0, 0 );
			SEQ_Change( p_wk, SEQFUNC_TouchLeft );
		}

		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )
		{	
			if( p_wk->debug_game_cnt == 0 )
			{	
				p_wk->debug_game_cnt	= DEBUG_GAME_NUM-1;
			}
			else
			{	
				p_wk->debug_game_cnt--;
			}
			SEQ_Change( p_wk, SEQFUNC_StartGame);
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
		{	
			p_wk->debug_game_cnt++;
			p_wk->debug_game_cnt	%= DEBUG_GAME_NUM;
			SEQ_Change( p_wk, SEQFUNC_StartGame);
		}


		break;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	������^�b�`��
 *
 *	@param	AURA_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_TouchLeft( AURA_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_MAIN,
		SEQ_WAIT_RIGHT,
		SEQ_RET,
	};

	GFL_POINT	pos;

	TouchMarker_OffVisible( p_wk );

	switch( *p_seq )
	{	
	case SEQ_MAIN:
		if( TP_GetRectCont( &sc_left, &pos ) )
		{	
			//�v���I���҂�
			if( SHAKESEARCH_Main( &p_wk->shake_left[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ], &sc_left ) )
			{	
				DEBUGAURA_PRINT_UpDate( p_wk );
				MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, AURA_STR_002, 0, 0 );
				GFL_BG_SetVisible( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_GUIDE_R], VISIBLE_ON );
				*p_seq	= SEQ_WAIT_RIGHT;
			}

			TouchMarker_SetPos( p_wk, &pos );
		}
		else
		{	
			SHAKESEARCH_Init( &p_wk->shake_left[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ]);
			*p_seq	= SEQ_RET;
		}
		break;

	case SEQ_WAIT_RIGHT:
		if( TP_GetRectCont( &sc_right, &p_wk->trg_right[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ] ) )
		{	
			TouchMarker_SetPos( p_wk, &p_wk->trg_right[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ] );


			DEBUGAURA_PRINT_UpDate( p_wk );
			MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, AURA_STR_001, 0, 0 );
			SEQ_Change( p_wk, SEQFUNC_TouchRight );
		}
		else if( TP_GetRectCont( &sc_left, &pos ) )
		{	
			TouchMarker_SetPos( p_wk, &pos );
		}
		else
		{
			*p_seq	= SEQ_RET;
		}
		break;

	case SEQ_RET:
		SEQ_Change( p_wk, SEQFUNC_StartGame );
		break;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�E��^�b�`��
 *
 *	@param	AURA_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_TouchRight( AURA_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_MAIN,
		SEQ_RET,
	};


	GFL_POINT	pos;

	TouchMarker_OffVisible( p_wk );

	switch( *p_seq )
	{	
	case SEQ_MAIN:
		if( TP_GetRectCont( &sc_right, &pos ) )
		{	
			if( SHAKESEARCH_Main( &p_wk->shake_right[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ], &sc_right ) )
			{	
				int i;

				DEBUGAURA_PRINT_UpDate( p_wk );
				//MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, AURA_STR_003, 0, 0 );

				//0�Ȃ�Ύ����̔ԂȂ̂ŁA���͑���̔�
				if( p_wk->debug_player == 0 )
				{	
					p_wk->debug_player++;
					SEQ_Change( p_wk, SEQFUNC_StartGame );
				}
				else
				{
					MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, AURA_DEBUG_002, 0, 0 );
					SEQ_Change( p_wk, SEQFUNC_Result );
				}
			}

			TouchMarker_SetPos( p_wk, &pos );
		}
		else
		{	
			SHAKESEARCH_Init( &p_wk->shake_right[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ]);
			*p_seq	= SEQ_RET;
		}
		break;

	case SEQ_RET:
		SEQ_Change( p_wk, SEQFUNC_StartGame );
		break;
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	����
 *
 *	@param	AURA_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Result( AURA_MAIN_WORK *p_wk, u16 *p_seq )
{	
	if(	GFL_UI_TP_GetTrg()	)
	{	
		p_wk->debug_game_cnt++;
		p_wk->debug_game_cnt	%=	DEBUG_GAME_NUM;
		p_wk->debug_player		= 0;
		SEQ_Change( p_wk, SEQFUNC_StartGame );
	}
}
//=============================================================================
/**
 *				�ėp
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	��`����Trg�������ǂ���
 *
 *	@param	const GFL_RECT *cp_rect	��`
 *	@param	*p_trg									���W�󂯎��
 *
 *	@retval	TRUE�^�b�`����
 *	@retval	FALSE�^�b�`���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL TP_GetRectTrg( const GFL_RECT *cp_rect, GFL_POINT *p_trg )
{	
	u32 x, y;
	BOOL ret;

	//Cont���ŁA��`���̂Ƃ�
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		if( ((u32)( x - cp_rect->left) < (u32)(cp_rect->right - cp_rect->left))
				&	((u32)( y - cp_rect->top) < (u32)(cp_rect->bottom - cp_rect->top))
			)
		{
			//�󂯎�肪���݂����������ĕԂ�
			if( p_trg )
			{	
				p_trg->x	= x;
				p_trg->y	= y;
			}
			return TRUE;
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	��`���Ƀ^�b�`�������ǂ���
 *
 *	@param	const GFL_RECT *cp_rect	��`
 *	@param	*p_cont									���W�󂯎��
 *
 *	@retval	TRUE�^�b�`����
 *	@retval	FALSE�^�b�`���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL TP_GetRectCont( const GFL_RECT *cp_rect, GFL_POINT *p_cont )
{	
	u32 x, y;
	BOOL ret;

	//Cont���ŁA��`���̂Ƃ�
	if( GFL_UI_TP_GetPointCont( &x, &y ) )
	{	
		if( ((u32)( x - cp_rect->left) < (u32)(cp_rect->right - cp_rect->left))
				&	((u32)( y - cp_rect->top) < (u32)(cp_rect->bottom - cp_rect->top))
			)
		{
			//�󂯎�肪���݂����������ĕԂ�
			if( p_cont )
			{	
				p_cont->x	= x;
				p_cont->y	= y;
			}
			return TRUE;
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�v�����g���X�V
 *
 *	@param	AURA_MAIN_WORK *p_wk	���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void DEBUGAURA_PRINT_UpDate( AURA_MAIN_WORK *p_wk )
{	
	int i, j;
	int now_idx;

	DEBUGPRINT_Clear();

	DEBUGPRINT_PrintNumber( L"%d�Ԗڂ̃Q�[��", p_wk->debug_game_cnt, 0,  0 );
	DEBUGPRINT_PrintNumber( L"%d�l�ڂ̔Ԃł�", p_wk->debug_player, 100,  0 );


	//�v�����g
	OS_Printf( "����%d�Ԗڂ̃Q�[���@�v�����g�J�n����������������\n",p_wk->debug_game_cnt );
	for( j = 0; j < 2; j++ )
	{	
		now_idx	= p_wk->debug_game_cnt + (j*DEBUG_GAME_NUM);

		DEBUGPRINT_Print( L"���w�̍��W", DEBUGMSG_LEFT1+(DEBUGMSG2_TAB*j), 10 );
		DEBUGPRINT_Print( L"�E�w�̍��W", DEBUGMSG_RIGHT1+(DEBUGMSG2_TAB*j),10 );
	
		DEBUGPRINT_PrintNumber( L"X %d", p_wk->trg_left[now_idx].x, DEBUGMSG_LEFT1+(DEBUGMSG2_TAB*j), 20 );
		DEBUGPRINT_PrintNumber( L"Y %d", p_wk->trg_left[now_idx].y, DEBUGMSG_LEFT2+(DEBUGMSG2_TAB*j), 20 );
	
		DEBUGPRINT_PrintNumber( L"X %d", p_wk->trg_right[now_idx].x, DEBUGMSG_RIGHT1+(DEBUGMSG2_TAB*j), 20 );
		DEBUGPRINT_PrintNumber( L"Y %d", p_wk->trg_right[now_idx].y, DEBUGMSG_RIGHT2+(DEBUGMSG2_TAB*j), 20 );
	
		DEBUGPRINT_Print( L"���w�̃u��", DEBUGMSG_LEFT1+(DEBUGMSG2_TAB*j), 30 );
		DEBUGPRINT_Print( L"�E�w�̃u��", DEBUGMSG_RIGHT1+(DEBUGMSG2_TAB*j),30 );

		for( i = 0; i < TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			//DEBUGPRINT_PrintNumber( L"X %d", p_wk->shake_left.shake[i].x, DEBUGMSG_LEFT1, i*10+40 );
			DEBUGPRINT_PrintNumber( L"X %d ", p_wk->shake_left[now_idx].shake[i].x- p_wk->shake_left[now_idx].shake[0].x, DEBUGMSG_LEFT1+(DEBUGMSG2_TAB*j), i*10+40 );
			//DEBUGPRINT_PrintNumber( L"Y %d", p_wk->shake_left.shake[i].y, DEBUGMSG_LEFT3, i*10+40 );
			DEBUGPRINT_PrintNumber( L"Y %d ", p_wk->shake_left[now_idx].shake[i].y- p_wk->shake_left[now_idx].shake[0].y, DEBUGMSG_LEFT2+(DEBUGMSG2_TAB*j), i*10+40 );
	
			//DEBUGPRINT_PrintNumber( L"X %d", p_wk->shake_right.shake[i].x, DEBUGMSG_RIGHT1, i*10+40 );
			DEBUGPRINT_PrintNumber( L"X %d ", p_wk->shake_right[now_idx].shake[i].x - p_wk->shake_right[now_idx].shake[0].x, DEBUGMSG_RIGHT1+(DEBUGMSG2_TAB*j), i*10+40 );
			//DEBUGPRINT_PrintNumber( L"Y %d", p_wk->shake_right.shake[i].y, DEBUGMSG_RIGHT3, i*10+40 );
			DEBUGPRINT_PrintNumber( L"Y %d ", p_wk->shake_right[now_idx].shake[i].y - p_wk->shake_right[now_idx].shake[0].y,DEBUGMSG_RIGHT2+(DEBUGMSG2_TAB*j), i*10+40 );
		}
	

		OS_Printf( "��%d�l�ڂ̃f�[�^\n", j );
	
		OS_Printf("������W\n");
		OS_Printf("X %d Y %d\n", p_wk->trg_left[now_idx].x, p_wk->trg_left[now_idx].y);
	
		OS_Printf("�E����W\n");
		OS_Printf("X %d Y %d\n", p_wk->trg_right[now_idx].x, p_wk->trg_right[now_idx].y);
	
		OS_Printf("����u����\n");
		for( i = 0; i < TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			OS_Printf( "X %d (%d) Y %d (%d)\n", 
					p_wk->shake_left[now_idx].shake[i].x, 
					p_wk->shake_left[now_idx].shake[i].x - p_wk->shake_left[now_idx].shake[0].x, 
					p_wk->shake_left[now_idx].shake[i].y, 
					p_wk->shake_left[now_idx].shake[i].y - p_wk->shake_left[now_idx].shake[0].y );
		}
	
		OS_Printf("�E��u����\n");
		for( i = 0; i < TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			OS_Printf( "X %d (%d) Y %d (%d)\n", 
					p_wk->shake_right[now_idx].shake[i].x, 
					p_wk->shake_right[now_idx].shake[i].x - p_wk->shake_right[now_idx].shake[0].x, 
					p_wk->shake_right[now_idx].shake[i].y, 
					p_wk->shake_right[now_idx].shake[i].y - p_wk->shake_right[now_idx].shake[0].y );
		}
	}

	{	
		u16 idx1, idx2;
		s32	ofs1[4];
		s32	ofs2[4];

		idx1	= p_wk->debug_game_cnt + (0*DEBUG_GAME_NUM);;
		idx2	= p_wk->debug_game_cnt + (1*DEBUG_GAME_NUM);;

		//��---------------------
		DEBUGPRINT_PrintNumber( L"X %d", p_wk->trg_left[idx1].x - p_wk->trg_left[idx2].x,
				DEBUGMSG3_TAB, 20 );
		DEBUGPRINT_PrintNumber( L"Y %d", p_wk->trg_left[idx1].y - p_wk->trg_left[idx2].y,
				DEBUGMSG3_TAB+20, 20 );
		DEBUGPRINT_PrintNumber( L"X %d", p_wk->trg_right[idx1].x - p_wk->trg_right[idx2].x,
				DEBUGMSG3_TAB+40, 20 );
		DEBUGPRINT_PrintNumber( L"Y %d", p_wk->trg_right[idx1].y - p_wk->trg_right[idx2].y,
				DEBUGMSG3_TAB+60, 20 );

		for( i = 0; i < TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			ofs1[0]	= p_wk->shake_left[idx1].shake[i].x- p_wk->shake_left[idx1].shake[0].x;
			ofs1[1]	= p_wk->shake_left[idx1].shake[i].y- p_wk->shake_left[idx1].shake[0].y;
			ofs1[2]	= p_wk->shake_right[idx1].shake[i].x- p_wk->shake_right[idx1].shake[0].x;
			ofs1[3]	= p_wk->shake_right[idx1].shake[i].y- p_wk->shake_right[idx1].shake[0].y;
			ofs2[0]	= p_wk->shake_left[idx2].shake[i].x- p_wk->shake_left[idx2].shake[0].x;
			ofs2[1]	= p_wk->shake_left[idx2].shake[i].y- p_wk->shake_left[idx2].shake[0].y;
			ofs2[2]	= p_wk->shake_right[idx2].shake[i].x- p_wk->shake_right[idx2].shake[0].x;
			ofs2[3]	= p_wk->shake_right[idx2].shake[i].y- p_wk->shake_right[idx2].shake[0].y;

			DEBUGPRINT_PrintNumber( L"X %d ", ofs1[0]-ofs2[0], DEBUGMSG3_TAB, i*10+40 );
			DEBUGPRINT_PrintNumber( L"Y %d ", ofs1[1]-ofs2[1], DEBUGMSG3_TAB+20, i*10+40 );
			DEBUGPRINT_PrintNumber( L"X %d ", ofs1[2]-ofs2[2], DEBUGMSG3_TAB+40, i*10+40 );
			DEBUGPRINT_PrintNumber( L"Y %d ", ofs1[3]-ofs2[3], DEBUGMSG3_TAB+60, i*10+40 );
		}

		OS_Printf( "����\n" );
		OS_Printf("������W�̍�\n");
		OS_Printf("X %d Y %d\n", 
				p_wk->trg_left[idx1].x - p_wk->trg_left[idx2].x,
				p_wk->trg_left[idx1].y - p_wk->trg_left[idx2].y);
	
		OS_Printf("�E����W�̍�\n");
		OS_Printf("X %d Y %d\n", 
				p_wk->trg_right[idx1].x - p_wk->trg_right[idx2].x,
				p_wk->trg_right[idx1].y - p_wk->trg_right[idx2].y);

		OS_Printf( "����u���̍�\n" );
		for( i = 0; i < TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			ofs1[0]	= p_wk->shake_left[idx1].shake[i].x- p_wk->shake_left[idx1].shake[0].x;
			ofs1[1]	= p_wk->shake_left[idx1].shake[i].y- p_wk->shake_left[idx1].shake[0].y;
			ofs2[0]	= p_wk->shake_left[idx2].shake[i].x- p_wk->shake_left[idx2].shake[0].x;
			ofs2[1]	= p_wk->shake_left[idx2].shake[i].y- p_wk->shake_left[idx2].shake[0].y;
			OS_Printf( "X (%d) Y (%d)\n",ofs1[0]-ofs2[0], ofs1[1]-ofs2[1] );
		}

		OS_Printf( "�E��u���̍�\n" );
		for( i = 0; i < TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			ofs1[2]	= p_wk->shake_right[idx1].shake[i].x- p_wk->shake_right[idx1].shake[0].x;
			ofs1[3]	= p_wk->shake_right[idx1].shake[i].y- p_wk->shake_right[idx1].shake[0].y;
			ofs2[2]	= p_wk->shake_right[idx2].shake[i].x- p_wk->shake_right[idx2].shake[0].x;
			ofs2[3]	= p_wk->shake_right[idx2].shake[i].y- p_wk->shake_right[idx2].shake[0].y;
			OS_Printf( "X (%d) Y (%d)\n",ofs1[2]-ofs2[2], ofs1[3]-ofs2[3] );
		}

	}
	
	{
		GFL_POINT	temp;
		u32	checklist[9];		//����A�E��A�����A�E���A���A��A�E�A���A���S



		for( j = 0; j < 2; j++ )
		{	
			OS_Printf( "��%d�l�ڂ̍���u���v��\n", j );
			now_idx	= p_wk->debug_game_cnt + (j*DEBUG_GAME_NUM);
			GFL_STD_MemClear( checklist, sizeof(u32)*9 );

			for( i = 0; i < TOUCH_COUNTER_SHAKE_MAX; i++ )
			{	
				temp.x	= p_wk->shake_left[now_idx].shake[i].x - p_wk->shake_left[now_idx].shake[0].x;
				temp.y	= p_wk->shake_left[now_idx].shake[i].y - p_wk->shake_left[now_idx].shake[0].y;
				if( temp.x < 0 && temp.y > 0 )				//����
				{	
					checklist[0]++;
				}
				else if( temp.x > 0 && temp.y > 0 )		//�E��
				{	
					checklist[1]++;
				}
				else if( temp.x < 0 && temp.y < 0 )		//����
				{	
					checklist[2]++;
				}
				else if( temp.x > 0 && temp.y < 0 )		//�E��
				{	
					checklist[3]++;
				}
				else if( temp.x < 0 && temp.y == 0 )		//��
				{
					checklist[4]++;
				}
				else if( temp.x == 0 && temp.y > 0 )		//��
				{
					checklist[5]++;
				}
				else if( temp.x > 0 && temp.y == 0 )		//�E
				{
					checklist[6]++;
				}
				else if( temp.x == 0 && temp.y < 0 )	//��
				{
					checklist[7]++;
				}
				else if( temp.x == 0 && temp.y == 0 )	//���S
				{
					checklist[8]++;
				}

			}

			OS_Printf( "����@%d��\n", checklist[0] );
			OS_Printf( "�E��@%d��\n", checklist[1] );
			OS_Printf( "�����@%d��\n", checklist[2] );
			OS_Printf( "�E���@%d��\n", checklist[3] );
			OS_Printf( "���@�@%d��\n", checklist[4] );
			OS_Printf( "��@�@%d��\n", checklist[5] );
			OS_Printf( "�E�@�@%d��\n", checklist[6] );
			OS_Printf( "���@�@%d��\n", checklist[7] );
			OS_Printf( "���S�@%d��\n", checklist[8] );


			OS_Printf( "��%d�l�ڂ̉E��u���v��\n", j );
			GFL_STD_MemClear( checklist, sizeof(u32)*9 );
			for( i = 0; i < TOUCH_COUNTER_SHAKE_MAX; i++ )
			{	
				temp.x	= p_wk->shake_right[now_idx].shake[i].x - p_wk->shake_right[now_idx].shake[0].x;
				temp.y	= p_wk->shake_right[now_idx].shake[i].y - p_wk->shake_right[now_idx].shake[0].y;
				if( temp.x < 0 && temp.y > 0 )				//����
				{	
					checklist[0]++;
				}
				else if( temp.x > 0 && temp.y > 0 )		//�E��
				{	
					checklist[1]++;
				}
				else if( temp.x < 0 && temp.y < 0 )		//����
				{	
					checklist[2]++;
				}
				else if( temp.x > 0 && temp.y < 0 )		//�E��
				{	
					checklist[3]++;
				}
				else if( temp.x < 0 && temp.y == 0 )		//��
				{
					checklist[4]++;
				}
				else if( temp.x == 0 && temp.y > 0 )		//��
				{
					checklist[5]++;
				}
				else if( temp.x > 0 && temp.y == 0 )		//�E
				{
					checklist[6]++;
				}
				else if( temp.x == 0 && temp.y < 0 )	//��
				{
					checklist[7]++;
				}
				else if( temp.x == 0 && temp.y == 0 )	//���S
				{
					checklist[8]++;
				}

			}

			OS_Printf( "����@%d��\n", checklist[0] );
			OS_Printf( "�E��@%d��\n", checklist[1] );
			OS_Printf( "�����@%d��\n", checklist[2] );
			OS_Printf( "�E���@%d��\n", checklist[3] );
			OS_Printf( "���@�@%d��\n", checklist[4] );
			OS_Printf( "��@�@%d��\n", checklist[5] );
			OS_Printf( "�E�@�@%d��\n", checklist[6] );
			OS_Printf( "���@�@%d��\n", checklist[7] );
			OS_Printf( "���S�@%d��\n", checklist[8] );

		}
	}

	{	
		GFL_POINT	pos1, pos2;
		u32 ret;
		OS_Printf( "�����݂��̍��W�v��\n" );
		pos1	= p_wk->trg_left[p_wk->debug_game_cnt + (0*DEBUG_GAME_NUM)];
		pos2	= p_wk->trg_left[p_wk->debug_game_cnt + (1*DEBUG_GAME_NUM)];
		ret	= ((u32)(pos1.x-pos2.x)*(u32)(pos1.x-pos2.x))+((u32)(pos1.y-pos2.y)*(u32)(pos1.y-pos2.y));

		if( 0 < ret & ret < 8*8 )
		{	
			OS_Printf( "�����@100�_\n" );
		}
		else if( 8*8 < ret & ret < 16 * 16 )
		{	
			OS_Printf( "�����@80�_\n" );
		}
		else if( 16*16 < ret & ret < 24 * 24 )
		{	
			OS_Printf( "�����@50�_\n" );
		}
		else if( 24*24 < ret & ret < 32 * 32 )
		{	
			OS_Printf( "�����@30�_\n" );
		}
		else if( 32*32 < ret & ret < 40 * 40 )
		{	
			OS_Printf( "�����@10�_\n" );
		}
		else{	
			OS_Printf( "�����@0�_\n" );
		}
	}


	OS_Printf( "����%d�Ԗڂ̃Q�[���@�v�����g�I������������������\n", p_wk->debug_game_cnt );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�b�`�}�[�J�[�̈ʒu�����肵�\��
 *
 *	@param	AURA_MAIN_WORK *p_wk	���[�N
 *	@param	GFL_POINT *p_pos	�ʒu
 *
 */
//-----------------------------------------------------------------------------
static void TouchMarker_SetPos( AURA_MAIN_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	GFL_CLWK	*p_marker;
	p_marker	= GRAPHIC_GetClwk( &p_wk->grp, CLWKID_TOUCH );
	GFL_CLACT_WK_SetDrawEnable( p_marker, TRUE );

	{
		GFL_CLACTPOS clpos;
		clpos.x	= cp_pos->x;
		clpos.y	= cp_pos->y;
		GFL_CLACT_WK_SetPos( p_marker, &clpos, 0 );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�b�`�}�[�J�[�̕\����OFF
 *
 *	@param	AURA_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void TouchMarker_OffVisible( AURA_MAIN_WORK *p_wk )
{	
	GFL_CLWK	*p_marker;
	p_marker	= GRAPHIC_GetClwk( &p_wk->grp, CLWKID_TOUCH );
	GFL_CLACT_WK_SetDrawEnable( p_marker, FALSE );
}

//=============================================================================
/**
 *			�u���v��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�u���v��	������
 *
 *	@param	SHAKE_SEARCH_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void	SHAKESEARCH_Init( SHAKE_SEARCH_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(SHAKE_SEARCH_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�u���v��	�j��
 *
 *	@param	SHAKE_SEARCH_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void	SHAKESEARCH_Exit( SHAKE_SEARCH_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(SHAKE_SEARCH_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�u���v��	�v�����C��
 *
 *	@param	SHAKE_SEARCH_WORK *p_wk 
 *	@param	cp_rect�@�v������͈�
 *
 *	@retval	TRUE	�v���I��
 *	@retval	FALSE	�v����
 */
//-----------------------------------------------------------------------------
static BOOL	SHAKESEARCH_Main( SHAKE_SEARCH_WORK *p_wk, const GFL_RECT	*cp_rect )
{	

	//�I�����Ă�����TRUE
	if( p_wk->is_end )
	{	
		return TRUE;
	}

	//�v���I���҂�
	if( p_wk->cnt++ > TOUCH_COUNTER_MAX )
	{	
		TP_GetRectCont( cp_rect, &p_wk->shake[TOUCH_COUNTER_SHAKE_MAX-1] );
		p_wk->is_end	= TRUE;
		return TRUE;
	}
	else
	{	
		//�v��
		if( p_wk->shake_cnt++ > TOUCH_COUNTER_SHAKE_SYNC )
		{	
			p_wk->shake_cnt	= 0;
			TP_GetRectCont( cp_rect, &p_wk->shake[p_wk->shake_idx++] );
			GF_ASSERT_MSG( p_wk->shake_idx < TOUCH_COUNTER_SHAKE_MAX, 
					"�u���v���C���f�b�N�X�G���[�ł� %d\n" , p_wk->shake_idx );
		}
	}

	return FALSE;
}
 
#ifdef DEBUG_AURA_MSG
//=============================================================================
/**
 *				�f�o�b�O�v�����g�p���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�	������
 *
 *	@param	frm											�t���[��
 *	@param	heapID									�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Init( u8 frm, BOOL is_now_save, HEAPID heapID )
{	
	DEBUG_PRINT_WORK *p_wk;

	sp_dp_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_PRINT_WORK) );
	p_wk = sp_dp_wk;
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_PRINT_WORK) );
	p_wk->heapID						= heapID;
	p_wk->is_now_save				= is_now_save;
	p_wk->frm								= frm;

	//�f�o�b�O�v�����g�p�t�H���g
	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_small_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );	

	//�ޔ��G���A��NetEff����擾
	NetErr_GetTempArea( &p_wk->p_char_temp_area, &p_wk->p_scrn_temp_area, &p_wk->p_pltt_temp_area );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�	�j��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Exit( void )
{	
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;

	GFL_FONT_Delete( p_wk->p_font );
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_PRINT_WORK) );

	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�I�[�v��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Open( void )
{	
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;

	GF_ASSERT( p_wk );

	if( p_wk->is_now_save )
	{	
		//VRAM�̃f�[�^��ޔ�
		GFL_STD_MemCopy16(GFL_DISPUT_GetCgxPtr(p_wk->frm), p_wk->p_char_temp_area, DEBUGPRINT_CHAR_TEMP_AREA);
		GFL_STD_MemCopy16(GFL_DISPUT_GetScrPtr(p_wk->frm), p_wk->p_scrn_temp_area, DEBUGPRINT_SCRN_TEMP_AREA);
		GFL_STD_MemCopy16(GFL_DISPUT_GetPltPtr(p_wk->frm), p_wk->p_pltt_temp_area, DEBUGPRINT_PLTT_TEMP_AREA);	
		//Font�J���[�̑ޔ�
		GFL_FONTSYS_GetColor( &p_wk->font_col_bkup[0] ,
				&p_wk->font_col_bkup[1] ,
				&p_wk->font_col_bkup[2] );

		//�������ޔ�
		p_wk->prioryty_bkup = GFL_BG_GetPriority(p_wk->frm);
		p_wk->scroll_x_bkup = GFL_BG_GetScrollX(p_wk->frm);
		p_wk->scroll_y_bkup = GFL_BG_GetScrollY(p_wk->frm);
	}

	//��őޔ����������̂̐ݒ�
	GFL_BG_SetPriority( p_wk->frm , 0 );
	GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_X_SET , 0 );
	GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_Y_SET , 0 );

	//�f�o�b�O�v�����g�p�ݒ�
	//�X�N���[���̍쐬
  {
    u8 x,y;
		u16 buf;
    for( y = 0;y<DEBUGPRINT_HEIGHT;y++ )
    {
      for( x=0;x<DEBUGPRINT_WIDTH;x++ )
      {
        buf = x+y*DEBUGPRINT_WIDTH;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
      for( x=DEBUGPRINT_WIDTH;x<32;x++ )
      {
        buf = DEBUGPRINT_HEIGHT*DEBUGPRINT_WIDTH;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
    }
		for( y = DEBUGPRINT_HEIGHT;y<24;y++ )
    {
	 		buf = DEBUGPRINT_HEIGHT*DEBUGPRINT_WIDTH;
 			GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
			for( x=0;x<32;x++ )
      {
        buf = DEBUGPRINT_HEIGHT*DEBUGPRINT_WIDTH;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
		}
    GFL_BG_LoadScreenReq( p_wk->frm );
  }
  
  //�p���b�g�̍쐬
  {
    u16 col[4]={ 0xFFFF , 0x0000 , 0x7fff , 0x001f };
    GFL_BG_LoadPalette( p_wk->frm, col, sizeof(u16)*4, 0 );
  }

	//�������ނ��߂�BMP�쐬
	p_wk->p_bmp	= GFL_BMP_CreateInVRAM( GFL_DISPUT_GetCgxPtr(p_wk->frm), DEBUGPRINT_WIDTH, DEBUGPRINT_HEIGHT, GFL_BMP_16_COLOR, p_wk->heapID );
	GFL_STD_MemClear16( GFL_DISPUT_GetCgxPtr(p_wk->frm) , DEBUGPRINT_CHAR_TEMP_AREA );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�	�I��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Close( void )
{	
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;

	GF_ASSERT( p_wk );

	GFL_BMP_Delete( p_wk->p_bmp );
	if( p_wk->is_now_save )
	{	
		//������땜�A
		GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_X_SET , p_wk->scroll_x_bkup );
		GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_Y_SET , p_wk->scroll_y_bkup );
		GFL_BG_SetPriority( p_wk->frm , p_wk->prioryty_bkup );
		//Font�J���[�̕��A
		GFL_FONTSYS_SetColor( p_wk->font_col_bkup[0] ,
				p_wk->font_col_bkup[1] ,
				p_wk->font_col_bkup[2] );
		GFL_STD_MemCopy16(p_wk->p_char_temp_area, GFL_DISPUT_GetCgxPtr(p_wk->frm), DEBUGPRINT_CHAR_TEMP_AREA);
		GFL_STD_MemCopy16(p_wk->p_scrn_temp_area, GFL_DISPUT_GetScrPtr(p_wk->frm), DEBUGPRINT_SCRN_TEMP_AREA);
		GFL_STD_MemCopy16(p_wk->p_pltt_temp_area, GFL_DISPUT_GetPltPtr(p_wk->frm), DEBUGPRINT_PLTT_TEMP_AREA);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�ɏ�������
 *
 *	@param	u16 *cp_str							���C�h������
 *	@param	x												���WX
 *	@param	y												���WY
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Print( const u16 *cp_str, u16 x, u16 y )
{	
	STRBUF	*p_strbuf;
	STRCODE	str[128];
	u16	strlen;
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;

	GF_ASSERT(p_wk);

	//STRBUF�p�ɕϊ�
	strlen	= wcslen(cp_str);
	GFL_STD_MemCopy(cp_str, str, strlen*2);
	str[strlen]	= GFL_STR_GetEOMCode();

	//STRBUF�ɓ]��
	p_strbuf	= GFL_STR_CreateBuffer( strlen*2, p_wk->heapID );
	GFL_STR_SetStringCode( p_strbuf, str);

	//��������
	PRINTSYS_Print( p_wk->p_bmp, x, y, p_strbuf, p_wk->p_font );

	//�j��
	GFL_STR_DeleteBuffer( p_strbuf );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�ɐ��l��������������
 *
 *	@param	u16 *cp_str							���C�h������i%d��%f���g���Ă��������j
 *	@param	number									����
 *	@param	x												���WX
 *	@param	y												���WY
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_PrintNumber( const u16 *cp_str, int number, u16 x, u16 y )
{	
	u16	str[128];
	swprintf( str, 128, cp_str, number );
	DEBUGPRINT_Print( str, x, y );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈���N���A�[
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Clear( void )
{	
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;
	GF_ASSERT(p_wk);
	GFL_BMP_Clear( p_wk->p_bmp, 0 );
}
#endif //DEBUG_AURA_MSG 
