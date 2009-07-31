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
#include "sound/pm_sndsys.h"

//	module
#include "infowin/infowin.h"

#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/bmp_winframe.h"

#include "net_app/irc_appbar.h"

//	archive
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_irc_aura.h"
#include "irccompatible_gra.naix"

//	aura
#include "net_app/irc_aura.h"

// proc
#include "net_app/irc_result.h"

#ifdef PM_DEBUG
//debug�p
#include "system/net_err.h"	//VRAM�ޔ�p�A�h���X��Ⴄ����
#include <wchar.h>					//wcslen
#endif


FS_EXTERN_OVERLAY(irc_result);

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#ifdef PM_DEBUG
#define DEBUG_IRC_COMPATIBLE_ONLYPLAY


static GFL_POINT s_pos	= {0};
#endif //PM_DEBUG

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
static s_is_debug_only_play	= 0;
#define DEBUG_ONLYPLAY_IF			if(s_is_debug_only_play){	
#define DEBUG_ONLYPLAY_ELSE		}else{
#define DEBUG_ONLYPLAY_ENDIF	}

#define DEBUG_AURA_MSG	//�f�o�b�O���b�Z�[�W���o��

#else
#define s_is_debug_only_play	(0)
#define DEBUG_ONLYPLAY_IF			/*  */
#define DEBUG_ONLYPLAY_ELSE		/*  */
#define DEBUG_ONLYPLAY_ENDIF	/*  */
#endif 


//-------------------------------------
///	�_��
//=====================================
//����ŏ��̍��W�̋����ɂ��_��
#define VAL_POS_SCORE_00 (100)
#define VAL_POS_SCORE_01 (80)
#define VAL_POS_SCORE_02 (50)
#define VAL_POS_SCORE_03 (30)
#define VAL_POS_SCORE_04 (10)
#define VAL_POS_SCORE_05 (0)

#define RANGE_POS_SCORE_00 (8*8)
#define RANGE_POS_SCORE_01 (16*16)
#define RANGE_POS_SCORE_02 (24*24)
#define RANGE_POS_SCORE_03 (32*32)
#define RANGE_POS_SCORE_04 (40*40)

//�u���̍��W�ɂ��_��
#define VAL_SHAKE_SCORE_00 (100)
#define VAL_SHAKE_SCORE_01 (80)
#define VAL_SHAKE_SCORE_02 (50)
#define VAL_SHAKE_SCORE_03 (30)
#define VAL_SHAKE_SCORE_04 (0)

#define RANGE_SHAKE_SCORE_00(x) ( x == 0 )
#define RANGE_SHAKE_SCORE_01(x) ( x == 1 )
#define RANGE_SHAKE_SCORE_02(x) ( x == 2 )
#define RANGE_SHAKE_SCORE_03(x) ( 3 <= x && x <= 5 )
#define RANGE_SHAKE_SCORE_04(x) ( x <= 6)

//-------------------------------------
///	�p���b�g
//=====================================
enum{	
	// ���C�����BG
	AURA_BG_PAL_M_00 = 0,//�w�i
	AURA_BG_PAL_M_01,		// 
	AURA_BG_PAL_M_02,		// 
	AURA_BG_PAL_M_03,		// 
	AURA_BG_PAL_M_04,		// 
	AURA_BG_PAL_M_05,		// 
	AURA_BG_PAL_M_06,		// 
	AURA_BG_PAL_M_07,		// 
	AURA_BG_PAL_M_08,		// �w�i
	AURA_BG_PAL_M_09,		// �K�C�h
	AURA_BG_PAL_M_10,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_11,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_12,		// �g�p���ĂȂ�
	AURA_BG_PAL_M_13,		// APPBAR
	AURA_BG_PAL_M_14,		// �t�H���g�pPLT
	AURA_BG_PAL_M_15,		// INFOWIN


	// �T�u���BG
	AURA_BG_PAL_S_00 = 0,	//�w�i
	AURA_BG_PAL_S_01,		// 
	AURA_BG_PAL_S_02,		// 
	AURA_BG_PAL_S_03,		// 
	AURA_BG_PAL_S_04,		// 
	AURA_BG_PAL_S_05,		// 
	AURA_BG_PAL_S_06,		// 
	AURA_BG_PAL_S_07,		// 
	AURA_BG_PAL_S_08,		// �w�i
	AURA_BG_PAL_S_09,		// �K�C�h
	AURA_BG_PAL_S_10,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_11,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_12,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_13,		// �g�p���ĂȂ�
	AURA_BG_PAL_S_14,		// �t�H���g�pPLT
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
	AURA_OBJ_PAL_M_13,		// APPBAR
	AURA_OBJ_PAL_M_14,		// APPBAR
	AURA_OBJ_PAL_M_15,		// APPBAR


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
///	����
//=====================================
#define TEXTSTR_PLT_NO				(AURA_BG_PAL_M_14)
#define TEXTSTR_BUFFER_LENGTH	(255)

//-------------------------------------
///	�ʒu
//=====================================
#define	MSGWND_TEXT_X	(1)
#define	MSGWND_TEXT_Y	(18)
#define	MSGWND_TEXT_W	(30)
#define	MSGWND_TEXT_H	(5)

#define	MSGWND_RETURN_X	(1)
#define	MSGWND_RETURN_Y	(22)
#define	MSGWND_RETURN_W	(30)
#define	MSGWND_RETURN_H	(2)

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
#define TOUCH_COUNTER_MAX	(150)
#define TOUCH_COUNTER_SHAKE_SYNC	(15)	//�u�����ϑ����邽�߂�1��̃V���N
#define TOUCH_COUNTER_SHAKE_MAX	(TOUCH_COUNTER_MAX/TOUCH_COUNTER_SHAKE_SYNC)	//�u�����擾�����
#define RESULT_SEND_CNT	(COMPATIBLE_IRC_SENDATA_CNT)

//-------------------------------------
///	��
//=====================================
#define CIRCLE_MAX		(3)

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
///	���b�Z�[�W�\���E�B���h�E
//=====================================
enum {
	MSGWNDID_TEXT,

	MSGWNDID_MAX,
};

//-------------------------------------
///	�^�b�`�G�t�F�N�g
//=====================================
typedef enum {
	TOUCHEFFID_LEFT,
	TOUCHEFFID_RIGHT,


	TOUCHEFFID_MAX,
}TOUCHEFFID;

//-------------------------------------
///	�l���ʉ�ʂ̒萔
//=====================================
enum
{	
	AOR_MSGWNDID_TITLE,
	AOR_MSGWNDID_MSG,
	AOR_MSGWNDID_DEBUG,

	AOR_MSGWNDID_MAX,
};

#define AOR_MSGWND_TITLE_X	(2)
#define AOR_MSGWND_TITLE_Y	(3)
#define AOR_MSGWND_TITLE_W	(20)
#define AOR_MSGWND_TITLE_H	(2)
#define AOR_MSGWND_MSG_X		(6)
#define AOR_MSGWND_MSG_Y		(9)
#define AOR_MSGWND_MSG_W		(20)
#define AOR_MSGWND_MSG_H		(6)
#define AOR_MSGWND_DEBUG_X	(2)
#define AOR_MSGWND_DEBUG_Y	(16)
#define AOR_MSGWND_DEBUG_W	(28)
#define AOR_MSGWND_DEBUG_H	(4)

//-------------------------------------
///	���f����
//=====================================
#define G3D_MDL_MAX					(2)

//-------------------------------------
///	�f�o�b�O�p�l���Z�[�u�@�\
//=====================================
#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
#define DEBUG_GAME_NUM	(1)
#define DEBUG_PLAYER_SAVE_NUM	(DEBUG_GAME_NUM*2)
#else
#define DEBUG_PLAYER_SAVE_NUM (1)
#endif //DEBUG_IRC_COMPATIBLE_ONLYPLAY

//-------------------------------------
///	3D���\�[�X
//=====================================
typedef enum
{	
	G3D_MDL_ANM_ICA,
	G3D_MDL_ANM_IMA,
	G3D_MDL_ANM_MAX
}G3D_MDL_ANM;
typedef struct 
{
	u16 imdID;
	u16 anmID[G3D_MDL_ANM_MAX];
} G3D_MDL_SETUP;

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

#define BACKOBJ_ONE_MOVE_SYNC_MIN	(110)
#define BACKOBJ_ONE_MOVE_SYNC_MAX	(160)
#define BACKOBJ_ONE_MOVE_SYNC_DIF	(BACKOBJ_ONE_MOVE_SYNC_MAX-BACKOBJ_ONE_MOVE_SYNC_MIN)

//-------------------------------------
///	CLWK�擾
//=====================================
typedef enum
{	
	CLWKID_BACKOBJ_TOP_S,
	CLWKID_BACKOBJ_END_S = CLWKID_BACKOBJ_TOP_S + BACKOBJ_CLWK_MAX,
	
	CLWKID_MAX
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
///	3D���\�[�X
//=====================================
typedef struct 
{
	GFL_G3D_RES	*p_res;
	GFL_G3D_RES *p_anm_res[G3D_MDL_ANM_MAX];
	GFL_G3D_RND	*p_rnd;
	GFL_G3D_OBJ	*p_obj;
	GFL_G3D_ANM	*p_anm[G3D_MDL_ANM_MAX];
	GFL_G3D_OBJSTATUS	status;
	BOOL	is_visible;
} G3D_MDL_WORK;

//-------------------------------------
///	3D�`���
//=====================================
typedef struct 
{
	GFL_G3D_CAMERA		*p_camera;
	G3D_MDL_WORK			mdl[G3D_MDL_MAX];
	fx32							alpha;
} GRAPHIC_3D_WORK;
#if 0
//-------------------------------------
///	�~
//=====================================
enum {	
	CIRCLE_VTX_MAX	=	60,	//���_��
};
typedef struct 
{
	VecFx16	*p_vtx;
	u32			vtx_num;
	GXRgb	vtx_color;
	u8	alpha;
	u8	dummy[3];
	GFL_G3D_OBJSTATUS	status;
	BOOL	is_visible;
} CIRCLE_WORK;
typedef struct{	
	CIRCLE_WORK				c[CIRCLE_MAX];
	u16								rot[CIRCLE_MAX];
	u16								rot_add[CIRCLE_MAX];
}TOUCH_EFFECT_WORK;
#endif
typedef struct {
	//TOUCH_EFFECT_WORK	wk[TOUCHEFFID_MAX];
	G3D_MDL_WORK			*p_mdl[TOUCHEFFID_MAX];
	GFL_POINT					left;
} TOUCH_EFFECT_SYS;

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
	TOUCH_EFFECT_SYS		touch_eff;

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
} MSGWND_WORK;

//-------------------------------------
///	�u���v��
//=====================================
typedef struct 
{
	GFL_POINT		shake[TOUCH_COUNTER_SHAKE_MAX];
	u16					shake_idx;
	u32					cnt;				//�J�E���^
	u32					shake_cnt;	//�u���v���p�J�E���^
	BOOL				is_end;			//1��擾�����t���O
}SHAKE_SEARCH_WORK;

//-------------------------------------
///	�I�[���p�l�b�g
//=====================================
//�f�[�^
typedef struct 
{
	GFL_POINT	trg_left;
	GFL_POINT	trg_right;
	SHAKE_SEARCH_WORK	shake_left;
	SHAKE_SEARCH_WORK	shake_right;
} AURANET_RESULT_DATA;
//�l�b�g���C��
typedef struct 
{
	COMPATIBLE_IRC_SYS	*p_irc;
	u32 seq;
	AURANET_RESULT_DATA	result_recv;	//��M�o�b�t�@
	AURANET_RESULT_DATA	result_recv_my;	//��M�o�b�t�@
	AURANET_RESULT_DATA	result_send;	//���M�o�b�t�@
	BOOL is_recv;
} AURANET_WORK;
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
} BACKOBJ_WORK;
//-------------------------------------
///	�l���щ��
//=====================================
typedef struct 
{
	u32	seq;
	const MSG_WORK *cp_msg;
	const SHAKE_SEARCH_WORK *cp_search;
	MSGWND_WORK			msgwnd[AOR_MSGWNDID_MAX];
} AURA_ONLYRESULT_WORK;


#ifdef DEBUG_AURA_MSG
//-------------------------------------
///	�f�o�b�O�v�����g�p���
//=====================================
typedef struct 
{
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
///	�I�[���`�F�b�N���C�����[�N
//=====================================
typedef struct _AURA_MAIN_WORK AURA_MAIN_WORK;
typedef void (*SEQ_FUNCTION)( AURA_MAIN_WORK *p_wk, u16 *p_seq );
struct _AURA_MAIN_WORK
{
	//�O���t�B�b�N���W���[��
	GRAPHIC_WORK		grp;
	MSG_WORK				msg;
	MSGWND_WORK			msgwnd[MSGWNDID_MAX];
	BACKOBJ_WORK		backobj;

	//�V�[�P���X�Ǘ�
	SEQ_FUNCTION		seq_function;
	u16		seq;
	u16		cnt;
	BOOL is_end;
	BOOL is_next_proc;

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
	u16							debug_player;			//���������肩
	u16							debug_game_cnt;	//���Q�[���ڂ�
#endif //DEBUG_IRC_COMPATIBLE_ONLYPLAY

	//�l�b�g
	AURANET_WORK	net;

	//���ʕ\��
	AURA_ONLYRESULT_WORK	onlyresult;

	//����ʃo�[
	APPBAR_WORK	*p_appbar;

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
static GFL_CLUNIT * GRAPHIC_GetClunit( const GRAPHIC_WORK *cp_wk );
static TOUCH_EFFECT_SYS * GRAPHIC_GetTouchEffWk( GRAPHIC_WORK *p_wk );
static void GRAPHIC_WriteBmpwinFrame( GRAPHIC_WORK *p_wk, GFL_BMPWIN *p_bmpwin );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//BG
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_WriteBmpwinFrame( GRAPHIC_BG_WORK *p_wk, GFL_BMPWIN *p_bmpwin );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//obj
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT * GRAPHIC_OBJ_GetClunit( const GRAPHIC_OBJ_WORK *cp_wk );
//3d
static void GRAPHIC_3D_Init( GRAPHIC_3D_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_3D_Exit( GRAPHIC_3D_WORK *p_wk );
static void GRAPHIC_3D_StartDraw( GRAPHIC_3D_WORK *p_wk );
static void GRAPHIC_3D_EndDraw( GRAPHIC_3D_WORK *p_wk );
static G3D_MDL_WORK *GRAPHIC_3D_GetObj( const GRAPHIC_3D_WORK *cp_wk, u16 idx );
static void Graphic_3d_SetUp( void );
//3d_mdl
static void G3DMDL_Load( G3D_MDL_WORK *p_wk, ARCHANDLE *p_handle, const G3D_MDL_SETUP *cp_setup, HEAPID heapID );
static void G3DMDL_UnLoad( G3D_MDL_WORK *p_wk );
static void G3DMDL_Draw( G3D_MDL_WORK *p_wk );
static void G3DMDL_SetPos( G3D_MDL_WORK *p_wk, const VecFx32 *cp_trans );
static void G3DMDL_SetScale( G3D_MDL_WORK *p_wk, const VecFx32 *cp_scale );
static void G3DMDL_SetVisible( G3D_MDL_WORK *p_wk, BOOL is_visible );
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
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, const MSG_WORK *cp_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y );
static void MSGWND_PrintCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID );
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y );
static void MSGWND_PrintPlayerName( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, const MYSTATUS *cp_status, u16 x, u16 y );
static void MSGWND_Clear( MSGWND_WORK* p_wk );
static GFL_BMPWIN *MSGWND_GetBmpWin( const MSGWND_WORK* cp_wk );
//ONLYRESULT
#if 0
static void AURA_ONLYRESULT_Init( AURA_ONLYRESULT_WORK* p_wk, u8 frm, const MSG_WORK *cp_msg, const SHAKE_SEARCH_WORK *cp_search,  HEAPID heapID );
static void AURA_ONLYRESULT_Exit( AURA_ONLYRESULT_WORK* p_wk );
static BOOL AURA_ONLYRESULT_Main( AURA_ONLYRESULT_WORK* p_wk );
#endif
//SEQ
static void SEQ_Change( AURA_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function );
static void SEQ_End( AURA_MAIN_WORK *p_wk );
//SEQ_FUNC
static void SEQFUNC_StartGame( AURA_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_TouchLeft( AURA_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_TouchRight( AURA_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_Result( AURA_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_SceneError( AURA_MAIN_WORK *p_wk, u16 *p_seq );
//net
static void AURANET_Init( AURANET_WORK *p_wk, COMPATIBLE_IRC_SYS *p_irc );
static void AURANET_Exit( AURANET_WORK *p_wk );
static BOOL AURANET_SendResultData( AURANET_WORK *p_wk, const AURANET_RESULT_DATA *cp_data );
static void AURANET_GetResultData( const AURANET_WORK *cp_wk, AURANET_RESULT_DATA *p_data );
//net_recv
static void NETRECV_Result( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle );
static u8* NETRECV_GetBufferAddr(int netID, void* pWork, int size);
//�~�v���~�e�B�u
#if 0
static void CIRCLE_Init( CIRCLE_WORK *p_wk, u32 vtx_num, HEAPID heapID );
static void CIRCLE_Exit( CIRCLE_WORK *p_wk );
static void CIRCLE_Draw( CIRCLE_WORK *p_wk );
static void CIRCLE_SetVtxColor( CIRCLE_WORK *p_wk, GXRgb color );
static void CIRCLE_SetAlpha( CIRCLE_WORK *p_wk, u8 alpha );
static void CIRCLE_SetTrans( CIRCLE_WORK *p_wk, const VecFx32 *cp_trans );
static void CIRCLE_SetScale( CIRCLE_WORK *p_wk, const VecFx32 *cp_scale );
static void CIRCLE_SetRot( CIRCLE_WORK *p_wk, const MtxFx33 *cp_rot );
static void CIRCLE_SetVisible( CIRCLE_WORK *p_wk, BOOL is_visible );
static void CIRCLE_GetTrans( const CIRCLE_WORK *cp_wk, VecFx32 *p_trans );
static void CIRCLE_GetScale( const CIRCLE_WORK *cp_wk, VecFx32 *p_scale );
static void CIRCLE_GetRot( const CIRCLE_WORK *cp_wk, MtxFx33 *p_rot );
static BOOL CIRCLE_GetVisible( const CIRCLE_WORK *cp_wk );
static void Circle_DrawLine( VecFx16 *p_start, VecFx16 *p_end );
#endif
//���o
static void TOUCH_EFFECT_Init( TOUCH_EFFECT_SYS *p_sys, const GRAPHIC_3D_WORK *cp_g3d, HEAPID heapID );
static void TOUCH_EFFECT_Exit( TOUCH_EFFECT_SYS *p_sys );
static void TOUCH_EFFECT_Draw( TOUCH_EFFECT_SYS *p_wk );
static void TOUCH_EFFECT_SetVisible( TOUCH_EFFECT_SYS *p_wk, TOUCHEFFID id, BOOL is_visible );
static void TOUCH_EFFECT_SetPos( TOUCH_EFFECT_SYS *p_wk, TOUCHEFFID id, u32 x, u32 y );

//�ėp
static BOOL TP_GetRectTrg( const GFL_RECT *cp_rect, GFL_POINT *p_trg );
static BOOL TP_GetRectCont( const GFL_RECT *cp_rect, GFL_POINT *p_cont );
static u8		CalcScore( AURA_MAIN_WORK *p_wk );
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID );
typedef enum
{	
	GUIDE_VISIBLE_LEFT	= 1<<0,
	GUIDE_VISIBLE_RIGHT	= 1<<1,
	GUIDE_VISIBLE_BOTH	= GUIDE_VISIBLE_LEFT|GUIDE_VISIBLE_RIGHT,
}GUIDE_VISIBLE_MASK;
static void GUIDE_SetVisible( GUIDE_VISIBLE_MASK msk, HEAPID heapID );

//BACKOBJ
static void BACKOBJ_Init( BACKOBJ_WORK *p_wk, const GRAPHIC_WORK *cp_grp, BACKOBJ_MOVE_TYPE type, BACKOBJ_COLOR color, u32 clwk_ofs, int sf_type );
static void BACKOBJ_Exit( BACKOBJ_WORK *p_wk );
static void BACKOBJ_Main( BACKOBJ_WORK *p_wk );
static void BACKOBJ_StartEmit( BACKOBJ_WORK *p_wk, const GFL_POINT *cp_pos );
//BACKOBJ_ONE
static void BACKOBJ_ONE_Init( BACKOBJ_ONE *p_wk, GFL_CLWK *p_clwk, BACKOBJ_COLOR color, int sf_type );
static void BACKOBJ_ONE_Main( BACKOBJ_ONE *p_wk );
static void BACKOBJ_ONE_Start( BACKOBJ_ONE *p_wk, const GFL_POINT *cp_start, const GFL_POINT *cp_end, u32 sync );
static BOOL BACKOBJ_ONE_IsMove( const BACKOBJ_ONE *cp_wk );

#ifdef DEBUG_AURA_MSG
static void DEBUGAURA_PRINT_UpDate( AURA_MAIN_WORK *p_wk );
#else
#define DEBUGAURA_PRINT_UpDate( ... )	((void)0)
#endif //DEBUG_AURA_MSG


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
static STRBUF * DEBUGPRINT_CreateWideChar( const u16 *cp_str, HEAPID heapID );
static STRBUF * DEBUGPRINT_CreateWideCharNumber( const u16 *cp_str, int number, HEAPID heapID );

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
	GRAPHIC_BG_FRAME_M_GUIDE,
	GRAPHIC_BG_FRAME_M_BACK,
	GRAPHIC_BG_FRAME_S_ROGO,
	GRAPHIC_BG_FRAME_S_TEXT,
	GRAPHIC_BG_FRAME_S_BACK,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	GFL_BG_FRAME1_M, GFL_BG_FRAME2_M, GFL_BG_FRAME3_M, GFL_BG_FRAME0_S, GFL_BG_FRAME1_S,GFL_BG_FRAME2_S,
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
	// GRAPHIC_BG_FRAME_M_GUIDE
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_M_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_ROGO
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_TEXT
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
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

//-------------------------------------
///	NET
//=====================================
enum
{	
	NETRECV_RESULT	= GFL_NET_CMD_IRCAURA,
};
static const NetRecvFuncTable	sc_recv_tbl[]	=
{
	{	
		NETRECV_Result, NETRECV_GetBufferAddr
	}
};

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

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
		s_is_debug_only_play	= p_wk->p_param->is_only_play;
#endif// DEBUG_IRC_COMPATIBLE_ONLYPLAY

	//���W���[��������
	AURANET_Init( &p_wk->net, p_wk->p_param->p_irc );
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCAURA );
	MSG_Init( &p_wk->msg, MSG_FONT_TYPE_LARGE, HEAPID_IRCAURA );
	{
		GAME_COMM_SYS_PTR comm	= NULL;
		if( p_wk->p_param->p_gamesys )
		{	
			comm	= GAMESYSTEM_GetGameCommSysPtr(p_wk->p_param->p_gamesys);
		}
		INFOWIN_Init( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_INFOWIN], AURA_BG_PAL_M_15, comm, HEAPID_IRCAURA );
	}
	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_TEXT], sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TEXT],
			MSGWND_TEXT_X, MSGWND_TEXT_Y, MSGWND_TEXT_W, MSGWND_TEXT_H, AURA_BG_PAL_S_08, HEAPID_IRCAURA );
	BmpWinFrame_Write( p_wk->msgwnd[MSGWNDID_TEXT].p_bmpwin, WINDOW_TRANS_ON, 
					GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->grp.bg.frame_char), AURA_BG_PAL_S_06 );

	{	
		int i;
		for( i = 0; i < DEBUG_PLAYER_SAVE_NUM; i++ )
		{	
			SHAKESEARCH_Init( &p_wk->shake_left[ i ] );
			SHAKESEARCH_Init( &p_wk->shake_right[ i ] );
		}
	}

	BACKOBJ_Init( &p_wk->backobj, &p_wk->grp, BACKOBJ_MOVE_TYPE_RAIN, BACKOBJ_COLOR_RED, CLWKID_BACKOBJ_TOP_S, CLSYS_DEFREND_SUB );

	//APPBAR
	{	
		GFL_CLUNIT	*p_unit	= GRAPHIC_GetClunit( &p_wk->grp );
		p_wk->p_appbar	= APPBAR_Init( APPBAR_OPTION_MASK_CLOSE, p_unit, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_INFOWIN], AURA_BG_PAL_M_13, AURA_OBJ_PAL_M_13, APP_COMMON_MAPPING_128K, HEAPID_IRCAURA );
	}

DEBUG_ONLYPLAY_IF
	DEBUGPRINT_Init( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], FALSE, HEAPID_IRCAURA );
	DEBUGPRINT_Open();
DEBUG_ONLYPLAY_ENDIF

	//�I�[���V�[���Z�b�g
	COMPATIBLE_IRC_SetScene( p_wk->p_param->p_irc, COMPATIBLE_SCENE_AURA );

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



DEBUG_ONLYPLAY_IF
	DEBUGPRINT_Close();
	DEBUGPRINT_Exit();
DEBUG_ONLYPLAY_ENDIF

	//APPBAR
	APPBAR_Exit( p_wk->p_appbar );

	BACKOBJ_Exit( &p_wk->backobj );

	//���W���[���j��
	{	
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Exit( &p_wk->msgwnd[i] );
		}
	}
	INFOWIN_Exit();
	GRAPHIC_Exit( &p_wk->grp );
	MSG_Exit( &p_wk->msg );
	AURANET_Exit( &p_wk->net );

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
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Main( &p_wk->msgwnd[i], &p_wk->msg );
		}
	}	
	BACKOBJ_Main( &p_wk->backobj );
	GRAPHIC_Draw( &p_wk->grp );

	//�V�[�����p���I�ɑ���
	COMPATIBLE_IRC_SendSceneContinue( p_wk->p_param->p_irc );

	APPBAR_Main( p_wk->p_appbar );

#ifdef PM_DEBUG
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
	{	
		s_pos.y		+= 1;
		NAGI_Printf( "Dpos X%d Y%d\n", s_pos.x, s_pos.y );
	}
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
	{	
		s_pos.y		-= 1;
		NAGI_Printf( "Dpos X%d Y%d\n", s_pos.x, s_pos.y );
	}
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
	{	
		s_pos.x		-= 1;
		NAGI_Printf( "Dpos X%d Y%d\n", s_pos.x, s_pos.y );
	}
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
	{	
		s_pos.x		+= 1;
		NAGI_Printf( "Dpos X%d Y%d\n", s_pos.x, s_pos.y );
	}

#endif 


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
		GX_VRAM_OBJ_64_E,						// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_128_D,       // �T�u2D�G���W����OBJ
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
	GRAPHIC_3D_Init( &p_wk->g3d, heapID );

	TOUCH_EFFECT_Init( &p_wk->touch_eff, &p_wk->g3d, heapID );

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

	TOUCH_EFFECT_Exit( &p_wk->touch_eff );

	GRAPHIC_3D_Exit( &p_wk->g3d );
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

	GRAPHIC_3D_StartDraw( &p_wk->g3d );
/*	//NNS�n��3D�`��
	//�Ȃ�
	NNS_G3dGeFlushBuffer();
	//SDK�n��3D�`��

	TOUCH_EFFECT_Draw( &p_wk->touch_eff );
*/
	GRAPHIC_3D_EndDraw( &p_wk->g3d );

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
 *	@brief	�^�b�`���o���[�N�擾
 *
 *	@param	const GRAPHIC_WORK *cp_wk	���[�N
 *	@param	id												ID
 *
 *	@return	�^�b�`���o���[�N�擾
 */
//-----------------------------------------------------------------------------
static TOUCH_EFFECT_SYS * GRAPHIC_GetTouchEffWk( GRAPHIC_WORK *p_wk )
{	
	return &p_wk->touch_eff;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g�`��
 *
 *	@param	const GRAPHIC_WORK *cp_wk	���[�N
 *	@param	*p_bmpwin									BMPWIN
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_WriteBmpwinFrame( GRAPHIC_WORK *p_wk, GFL_BMPWIN *p_bmpwin )
{	
	GRAPHIC_BG_WriteBmpwinFrame( &p_wk->bg, p_bmpwin );
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
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
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
		ARCHANDLE	*p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCCOMPATIBLE, heapID );

		//�p���b�g
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_irccompatible_gra_aura_bg_NCLR,
				PALTYPE_MAIN_BG, AURA_BG_PAL_M_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_irccompatible_gra_aura_bg_NCLR,
				PALTYPE_SUB_BG, AURA_BG_PAL_S_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_irccompatible_gra_aura_bg_guide_NCLR,
				PALTYPE_MAIN_BG, AURA_BG_PAL_M_09*0x20, 0, heapID );

		//�L����
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_aura_bg_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_aura_bg_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_title_rogo_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_ROGO], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_aura_bg_guide_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_GUIDE], 0, 0, FALSE, heapID );
	
		//�X�N���[��
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_aura_bg_hert_s_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_aura_bg_hert_s_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_title_aura_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_ROGO], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_aura_bg_guide_l_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_GUIDE], 0, 0, FALSE, heapID );	

		GUIDE_SetVisible( GUIDE_VISIBLE_LEFT, heapID );

		//���N
		GFL_BG_FillCharacter( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], 0, 1, 0 );
		p_wk->frame_char	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_irccompatible_gra_ue_frame_NCGR, sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], 0, FALSE, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}

	//3d�͔�����
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG2,GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3, 13, 0 );
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

	//�A���t�@
	G2_BlendNone();

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
 *	@brief	���g����������
 *
 *	@param	GRAPHIC_BG_WORK *p_wk	���[�N
 *	@param	*p_bmpwin							��������BMPWIN
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_WriteBmpwinFrame( GRAPHIC_BG_WORK *p_wk, GFL_BMPWIN *p_bmpwin )
{	
	BmpWinFrame_Write( p_bmpwin, WINDOW_TRANS_ON, 
					GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_char), AURA_BG_PAL_M_01 );
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

		p_wk->reg_id[OBJREGID_TOUCH_PLT]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_irccompatible_gra_aura_obj_NCLR, CLSYS_DRAW_SUB, AURA_OBJ_PAL_S_00*0x20, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_irccompatible_gra_aura_obj_NCGR, FALSE, CLSYS_DRAW_SUB, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_irccompatible_gra_aura_obj_NCER, NARC_irccompatible_gra_aura_obj_NANR, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK�쐬
	{	
		int i;
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		for( i = CLWKID_BACKOBJ_TOP_S; i < CLWKID_BACKOBJ_END_S; i++ )
		{	
			p_wk->p_clwk[i]	= GFL_CLACT_WK_Create( p_wk->p_clunit, 
					p_wk->reg_id[OBJREGID_TOUCH_CHR],
					p_wk->reg_id[OBJREGID_TOUCH_PLT],
					p_wk->reg_id[OBJREGID_TOUCH_CEL],
					&cldata,
					CLSYS_DEFREND_SUB,
					heapID
					);
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
			GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[i], 3 );
			GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[i], TRUE );
			GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[i], 0 );
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
	//�Z���j��
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
	static const VecFx32 sc_CAMERA_PER_POS		= { 0,0,-FX32_CONST( 70 ) };
	static const VecFx32 sc_CAMERA_PER_UP			= { 0,FX32_ONE,0 };
	static const VecFx32 sc_CAMERA_PER_TARGET	= { 0,0,FX32_CONST( 0 ) };

	enum{	
		CAMERA_PER_FOVY	=	(40),
		CAMERA_PER_ASPECT =	(FX32_ONE * 4 / 3),
		CAMERA_PER_NEAR	=	(FX32_ONE * 1),
		CAMERA_PER_FER	=	(FX32_ONE * 1000),
		CAMERA_PER_SCALEW	=(0),
	};

	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K,
			GFL_G3D_VMANLNK, GFL_G3D_PLT32K, 0, heapID, Graphic_3d_SetUp );

#if 0
	//�ˉe
	p_wk->p_camera = GFL_G3D_CAMERA_CreatePerspective( CAMERA_PER_FOVY, CAMERA_PER_ASPECT,
				CAMERA_PER_NEAR, CAMERA_PER_FER, CAMERA_PER_SCALEW, 
				&sc_CAMERA_PER_POS, &sc_CAMERA_PER_UP, &sc_CAMERA_PER_TARGET, heapID );
#else
	//���ˉe
	p_wk->p_camera = GFL_G3D_CAMERA_CreateOrtho( 
		// const fx32 top, const fx32 bottom, const fx32 left, const fx32 right, 
			FX32_CONST(24), -FX32_CONST(24), -FX32_CONST(32), FX32_CONST(32),
				CAMERA_PER_NEAR, CAMERA_PER_FER, CAMERA_PER_SCALEW, 
			&sc_CAMERA_PER_POS, &sc_CAMERA_PER_UP, &sc_CAMERA_PER_TARGET, heapID );
#endif

	//�ǂݍ���
	{	
		static const G3D_MDL_SETUP	sc_aura_setup	=
		{	
			NARC_irccompatible_gra_aura_nsbmd,
			{	
				NARC_irccompatible_gra_aura_nsbca,
				NARC_irccompatible_gra_aura_nsbma,
			}
		};
		ARCHANDLE	*p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCCOMPATIBLE, heapID );

		G3DMDL_Load( &p_wk->mdl[0], p_handle, &sc_aura_setup, heapID );
		G3DMDL_Load( &p_wk->mdl[1], p_handle, &sc_aura_setup, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}
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
	//���
	{	
		int i;
		for( i = 0; i < G3D_MDL_MAX; i++ )
		{	
			G3DMDL_UnLoad( &p_wk->mdl[i] );
		}
	}

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

	//�`��
	{	
		int i;
		for( i = 0; i < G3D_MDL_MAX; i++ )
		{	
			G3DMDL_Draw( &p_wk->mdl[i] );
		}
	}
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
 *	@brief	���f���擾
 *
 *	@param	const GRAPHIC_3D_WORK *cp_wk	���[�N
 *
 *	@return	���f��
 */
//-----------------------------------------------------------------------------
static G3D_MDL_WORK *GRAPHIC_3D_GetObj( const GRAPHIC_3D_WORK *cp_wk, u16 idx )
{	
	return (G3D_MDL_WORK*)(&cp_wk->mdl[ idx ]);
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
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
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
 *				3DMDL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	3D���f���ǂݍ���
 *
 *	@param	G3D_MDL_WORK *p_wk	���[�N
 *	@param	*p_handle						�n���h��
 *	@param	dataID							�f�[�^ID
 *	@param	heapID							�q�[�vID
 */
//-----------------------------------------------------------------------------
static void G3DMDL_Load( G3D_MDL_WORK *p_wk, ARCHANDLE *p_handle, const G3D_MDL_SETUP *cp_setup, HEAPID heapID )
{	
	//���[�N�N���A
	GFL_STD_MemClear(p_wk, sizeof(G3D_MDL_WORK));

	//���f���ǂݍ���
	{	
		BOOL			result;

		GFL_G3D_RES	*p_tex	= NULL;
		GFL_G3D_RES	*p_mdl	= NULL;

		p_wk->p_res	= GFL_G3D_CreateResourceHandle( p_handle, cp_setup->imdID );
		GF_ASSERT( p_wk->p_res != NULL );

		if( GFL_G3D_CheckResourceType( p_wk->p_res, GFL_G3D_RES_CHKTYPE_TEX ) ){
			p_tex	= p_wk->p_res;
			result	= GFL_G3D_TransVramTexture( p_tex );
			GF_ASSERT( result );
		}

		if( GFL_G3D_CheckResourceType( p_wk->p_res, GFL_G3D_RES_CHKTYPE_MDL ) )
		{
			int i;

			p_mdl	= p_wk->p_res;
			p_wk->p_rnd	= GFL_G3D_RENDER_Create( p_mdl, 0, p_tex );	

			// �A�j���[�V�����쐬
			for( i = 0; i<G3D_MDL_ANM_MAX; i++ )
			{
				p_wk->p_anm_res[i]	= GFL_G3D_CreateResourceHandle( p_handle, cp_setup->anmID[i] );

				p_wk->p_anm[i]			= GFL_G3D_ANIME_Create( p_wk->p_rnd, p_wk->p_anm_res[i], 0 );
			}

			//	OBJ�쐬
			p_wk->p_obj	= GFL_G3D_OBJECT_Create( p_wk->p_rnd, p_wk->p_anm, G3D_MDL_ANM_MAX );

			// �A�j���[�V�������f
			for( i=0; i<G3D_MDL_ANM_MAX; i++ )
			{
				GFL_G3D_OBJECT_EnableAnime( p_wk->p_obj, i );
			}
		}

		//�A���t�@��ۑ����Ă���
		//p_wk->alpha	= NNS_G3dMdlGetMdlAlpha( G3D_OBJECT_GetMdl( p_wk->p_obj ), 0 );
	}

	//���W
	{	
		MTX_Identity33( &p_wk->status.rotate );
		VEC_Set( &p_wk->status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���
 *
 *	@param	G3D_MDL_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void G3DMDL_UnLoad( G3D_MDL_WORK *p_wk )
{	
	int i;

	//OBJ�j��
	GFL_G3D_OBJECT_Delete( p_wk->p_obj );
	for( i = 0; i<G3D_MDL_ANM_MAX; i++ )
	{
		if( p_wk->p_anm[i] )
		{	
			GFL_G3D_ANIME_Delete( p_wk->p_anm[i] );
		}
	}
	GFL_G3D_RENDER_Delete( p_wk->p_rnd );

	//���\�[�X�j��
	for( i = 0; i<G3D_MDL_ANM_MAX; i++ )
	{
		GFL_G3D_DeleteResource( p_wk->p_anm_res[i] );
	}
	GFL_G3D_DeleteResource( p_wk->p_res );

	//�N���A
	GFL_STD_MemClear(p_wk, sizeof(G3D_MDL_WORK));
}
//----------------------------------------------------------------------------
/**
 *	@brief	�`��
 *
 *	@param	G3D_MDL_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void G3DMDL_Draw( G3D_MDL_WORK *p_wk )
{	
	if( p_wk->is_visible )
	{	
		int i;
		for( i = 0; i<G3D_MDL_ANM_MAX; i++ )
		{
			GFL_G3D_OBJECT_LoopAnimeFrame( p_wk->p_obj, i, FX32_ONE);
		}

		GFL_G3D_DRAW_DrawObject( p_wk->p_obj, &p_wk->status );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���W�ݒ�
 *
 *	@param	G3D_MDL_WORK *p_wk	���[�N
 *	@param	VecFx32 *cp_trans		���W
 */
//-----------------------------------------------------------------------------
static void G3DMDL_SetPos( G3D_MDL_WORK *p_wk, const VecFx32 *cp_trans )
{	
	p_wk->status.trans	= *cp_trans;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�P�[���ݒ�
 *
 *	@param	G3D_MDL_WORK *p_wk	���[�N
 *	@param	VecFx32 *cp_scale		�X�P�[��
 */
//-----------------------------------------------------------------------------
static void G3DMDL_SetScale( G3D_MDL_WORK *p_wk, const VecFx32 *cp_scale )
{	
	p_wk->status.scale	= *cp_scale;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�\���t���O�ݒ�
 *
 *	@param	G3D_MDL_WORK *p_wk	���[�N
 *	@param	is_visible				TRUE�ŕ`��FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
static void G3DMDL_SetVisible( G3D_MDL_WORK *p_wk, BOOL is_visible )
{	
	p_wk->is_visible	= is_visible;
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
/*		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
*/	}
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
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( bgframe, x, y, w, h, plt, GFL_BMP_CHRAREA_GET_B );
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( TEXTSTR_BUFFER_LENGTH, heapID );
	PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );
	MSGWND_Clear( p_wk );
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
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, const MSG_WORK *cp_msg )
{	
	return PRINT_UTIL_Trans( &p_wk->print_util, cp_msg->p_print_que );
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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0xF );	

	//������쐬
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//�L���[���N���A
	PRINTSYS_QUE_Clear( p_que );

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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0xF );	

	//������쐬
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//�Z���^�[�ʒu�v�Z
	x	= GFL_BMPWIN_GetSizeX( p_wk->p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_wk->p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_wk->p_strbuf, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_wk->p_strbuf, p_font )/2;

	//�L���[���N���A
	PRINTSYS_QUE_Clear( p_que );

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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0xF );	

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

		//�L���[���N���A
		PRINTSYS_QUE_Clear( p_que );

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
 *	@param	x									�J�n�ʒuX
 *	@param	y									�J�n�ʒuY
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintPlayerName( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, const MYSTATUS *cp_status, u16 x, u16 y )
{	
	const GFL_MSGDATA* cp_msgdata;
	WORDSET *p_wordset;
	
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0xF );	

	//���W���[���擾
	p_wordset		= MSG_GetWordSet( cp_msg );
	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );

	//���l�����[�h�Z�b�g�ɓo�^
	WORDSET_RegisterPlayerName( p_wordset, 0, cp_status );

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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0xF );	
	GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BMPWIN�擾
 *
 *	@param	const MSGWND_WORK* cp_wk	���[�N
 *
 *	@return	BMPWIN
 */
//-----------------------------------------------------------------------------
static GFL_BMPWIN *MSGWND_GetBmpWin( const MSGWND_WORK* cp_wk )
{	
	return cp_wk->p_bmpwin;
}
//=============================================================================
/**
 *				�l���ʉ��
 */
//=============================================================================
#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	�l���ʉ��	������
 *
 *	@param	AURA_ONLYRESULT_WORK* p_wk	���[�N
 *	@param	frm									�g�p�t���[��
 *	@param	MSG_WORK *cp_wk			���b�Z�[�W
 *	@param	heapID							�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void AURA_ONLYRESULT_Init( AURA_ONLYRESULT_WORK* p_wk, u8 frm, const MSG_WORK *cp_msg, const SHAKE_SEARCH_WORK *cp_search,  HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(AURA_ONLYRESULT_WORK) );
	p_wk->cp_msg		= cp_msg;
	p_wk->cp_search	= cp_search;

	MSGWND_Init( &p_wk->msgwnd[AOR_MSGWNDID_TITLE], frm, AOR_MSGWND_TITLE_X, AOR_MSGWND_TITLE_Y,
				AOR_MSGWND_TITLE_W, AOR_MSGWND_TITLE_H, heapID );
	MSGWND_Init( &p_wk->msgwnd[AOR_MSGWNDID_MSG], frm, AOR_MSGWND_MSG_X, AOR_MSGWND_MSG_Y,
				AOR_MSGWND_MSG_W, AOR_MSGWND_MSG_H, heapID );

	MSGWND_Init( &p_wk->msgwnd[AOR_MSGWNDID_DEBUG], frm, AOR_MSGWND_DEBUG_X, AOR_MSGWND_DEBUG_Y,
				AOR_MSGWND_DEBUG_W, AOR_MSGWND_DEBUG_H, heapID );

	MSGWND_Print( &p_wk->msgwnd[AOR_MSGWNDID_TITLE], cp_msg, AURA_RES_000, 0, 0 );


	//������쐬
	{
		u32 msg_idx;
		u32	shake_abs;
		int i;

		shake_abs	= 0;
		for( i = 1; i <TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			shake_abs	+= MATH_IAbs( cp_search->shake[i].x - cp_search->shake[0].x );
			shake_abs	+= MATH_IAbs( cp_search->shake[i].y - cp_search->shake[0].y );
		}

		if( 40 <= shake_abs )
		{	
			msg_idx	= 0;
		}
		else if( 20 < shake_abs && shake_abs < 40 )
		{	
			msg_idx	= 1;
		}
		else if( shake_abs <= 20 )
		{	
			msg_idx	= 2;
		}
		else
		{	
			GF_ASSERT( 0 );
		}
		OS_Printf( "�u�����̐�Βl %d\n", shake_abs );

		MSGWND_Print( &p_wk->msgwnd[AOR_MSGWNDID_MSG], cp_msg, AURA_RESMSG_000 + msg_idx, 0, 0 );
	}

	//�f�o�b�O������쐬
	{	
		MSGWND_WORK* p_msgwnd;
		PRINT_QUE*	p_que;
		GFL_FONT*		p_font;
		int i;
		u32 shake_abs;
		u32 shake_sum;
		STRBUF *p_strbuf;
		
		static const u16 *scp_sec[]	=
		{	
			L"0.5s",
			L"1.0s",
			L"1.5s",
			L"2.0s",
			L"2.5s",
			L"3.0s",
			L"3.5s",
			L"4.0s",
			L"4.5s",
			L"5.0s",
		};

		p_msgwnd	= &p_wk->msgwnd[AOR_MSGWNDID_DEBUG];
		p_font	= GFL_FONT_Create( ARCID_FONT,
					NARC_font_small_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );		
		shake_sum	= 0;
		for( i = 0; i < TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			shake_abs	= MATH_IAbs( cp_search->shake[i].x - cp_search->shake[0].x );
			shake_abs	+= MATH_IAbs( cp_search->shake[i].y - cp_search->shake[0].y );
			shake_sum	+= shake_abs;

			p_strbuf	= DEBUGPRINT_CreateWideChar( scp_sec[i], heapID ); 
			PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 0, p_strbuf, p_font );
			GFL_STR_DeleteBuffer( p_strbuf );

			p_strbuf	= DEBUGPRINT_CreateWideCharNumber( L" %d", shake_abs, heapID ); 
			PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 10, p_strbuf, p_font );
			GFL_STR_DeleteBuffer( p_strbuf );
		}

		p_strbuf	= DEBUGPRINT_CreateWideChar( L"���v", heapID ); 
		PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 0, p_strbuf, p_font );
		GFL_STR_DeleteBuffer( p_strbuf );


		p_strbuf	= DEBUGPRINT_CreateWideCharNumber( L" %d", shake_sum, heapID ); 
		PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 10, p_strbuf, p_font );
		GFL_STR_DeleteBuffer( p_strbuf );

		GFL_BMPWIN_MakeTransWindow( p_msgwnd->p_bmpwin );
		GFL_FONT_Delete( p_font );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�l���ʉ��	�j��
 *
 *	@param	AURA_ONLYRESULT_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void AURA_ONLYRESULT_Exit( AURA_ONLYRESULT_WORK* p_wk )
{	
	int i;
	for( i = 0; i < AOR_MSGWNDID_MAX; i++ )
	{	
		MSGWND_Exit( &p_wk->msgwnd[i] );
	}
	GFL_STD_MemClear( p_wk, sizeof(AURA_ONLYRESULT_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�l���ʉ��	���C������
 *
 *	@param	AURA_ONLYRESULT_WORK* p_wk ���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@retval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL AURA_ONLYRESULT_Main( AURA_ONLYRESULT_WORK* p_wk )
{	

	{	
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Main( &p_wk->msgwnd[i], p_wk->cp_msg );
		}
	}

	if( GFL_UI_TP_GetTrg() )
	{	
		return TRUE;
	}

	return FALSE;
}
#endif
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
		SEQ_END,
	};
	GFL_POINT		*p_trg_left;

DEBUG_ONLYPLAY_IF
	p_trg_left	= &p_wk->trg_left[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ];
DEBUG_ONLYPLAY_ELSE
	p_trg_left	= &p_wk->trg_left[0];
DEBUG_ONLYPLAY_ENDIF

	switch( *p_seq )
	{	
	case SEQ_INIT:

		if( p_wk->p_param->p_gamesys )
		{	
			MSGWND_PrintPlayerName( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, 
					AURA_STR_000, p_wk->p_param->p_you_status,  0, 0 );
		}
		else
		{	
			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, 
					AURA_STR_000, 0, 0 );
		}

DEBUG_ONLYPLAY_IF
		DEBUGAURA_PRINT_UpDate( p_wk );
		if( p_wk->debug_player != 0 )
		{	
			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, AURA_DEBUG_001, 0, 0 );
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
DEBUG_ONLYPLAY_ENDIF

		//�^�b�`���oOFF
		{	
			int i;
			TOUCH_EFFECT_SYS *p_touch;
			p_touch	= GRAPHIC_GetTouchEffWk( &p_wk->grp );
			for( i = 0; i < TOUCHEFFID_MAX; i++ )
			{	
				TOUCH_EFFECT_SetVisible( p_touch, i, FALSE );
			}
		}

		GUIDE_SetVisible( GUIDE_VISIBLE_LEFT, HEAPID_IRCAURA );

		*p_seq	= SEQ_MAIN;
		break;

	case SEQ_MAIN:
		if( TP_GetRectTrg( &sc_left, p_trg_left ) )
		{	
			GUIDE_SetVisible( 0, HEAPID_IRCAURA );

			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, AURA_STR_001, 0, 0 );
		
			//���^�b�`���oON
			{	
				TOUCH_EFFECT_SYS *p_touch;
				p_touch	= GRAPHIC_GetTouchEffWk( &p_wk->grp );
				TOUCH_EFFECT_SetVisible( p_touch, TOUCHEFFID_LEFT, TRUE );
				TOUCH_EFFECT_SetPos( p_touch, TOUCHEFFID_LEFT, p_trg_left->x, p_trg_left->y );
			}


			*p_seq	= SEQ_END;
		}

		break;

	case SEQ_END:
		SEQ_Change( p_wk, SEQFUNC_TouchLeft );
		break;
	}

	if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_CLOSE )
	{
		PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );	
		p_wk->p_param->result	= IRCAURA_RESULT_RETURN;
		SEQ_End( p_wk );
	}	
	

DEBUG_ONLYPLAY_IF
DEBUG_ONLYPLAY_ELSE
	//�V�[�����قȂ�`�F�b�N
	if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) != 0 )
	{	
		SEQ_Change( p_wk, SEQFUNC_SceneError );
	}
DEBUG_ONLYPLAY_ENDIF

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
	GFL_POINT		*p_trg_right;
	SHAKE_SEARCH_WORK	*p_shake_left;
	GFL_POINT	pos;

DEBUG_ONLYPLAY_IF
	p_trg_right		= &p_wk->trg_right[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ];
	p_shake_left	= &p_wk->shake_left[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ];
DEBUG_ONLYPLAY_ELSE
	p_trg_right		= &p_wk->trg_right[0];
	p_shake_left	= &p_wk->shake_left[0];
DEBUG_ONLYPLAY_ENDIF


	switch( *p_seq )
	{	
	case SEQ_MAIN:
		if( TP_GetRectCont( &sc_left, &pos ) )
		{	
			//�v���I���҂�
			if( SHAKESEARCH_Main( p_shake_left, &sc_left ) )
			{	
				MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, AURA_STR_002, 0, 0 );
				GUIDE_SetVisible( GUIDE_VISIBLE_BOTH, HEAPID_IRCAURA );

				*p_seq	= SEQ_WAIT_RIGHT;
			}


			//���^�b�`���oON
			{	
				TOUCH_EFFECT_SYS *p_touch;
				p_touch	= GRAPHIC_GetTouchEffWk( &p_wk->grp );
				TOUCH_EFFECT_SetVisible( p_touch, TOUCHEFFID_LEFT, TRUE );
				TOUCH_EFFECT_SetPos( p_touch, TOUCHEFFID_LEFT, pos.x, pos.y );
			}

		}
		else
		{	
			SHAKESEARCH_Init( p_shake_left);
			*p_seq	= SEQ_RET;
		}
		break;

	case SEQ_WAIT_RIGHT:
		if( TP_GetRectCont( &sc_right, p_trg_right ) )
		{	
			GUIDE_SetVisible( 0, HEAPID_IRCAURA );


			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, AURA_STR_001, 0, 0 );
			SEQ_Change( p_wk, SEQFUNC_TouchRight );

			//�E�^�b�`���oON
			{	
				TOUCH_EFFECT_SYS *p_touch;
				p_touch	= GRAPHIC_GetTouchEffWk( &p_wk->grp );
				TOUCH_EFFECT_SetVisible( p_touch, TOUCHEFFID_RIGHT, TRUE );
				TOUCH_EFFECT_SetPos( p_touch, TOUCHEFFID_RIGHT, p_trg_right->x, p_trg_right->y );
			}
		}
		else if( TP_GetRectCont( &sc_left, &pos ) )
		{	
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

	if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_CLOSE )
	{
		PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
		p_wk->p_param->result	= IRCAURA_RESULT_RETURN;
		SEQ_End( p_wk );
	}

DEBUG_ONLYPLAY_IF
DEBUG_ONLYPLAY_ELSE
	//�V�[�����قȂ�`�F�b�N
	if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) != 0 )
	{	
		SEQ_Change( p_wk, SEQFUNC_SceneError );
	}
DEBUG_ONLYPLAY_ENDIF
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
	SHAKE_SEARCH_WORK	*p_shake_right;
DEBUG_ONLYPLAY_IF
	p_shake_right	= &p_wk->shake_right[ p_wk->debug_game_cnt + (p_wk->debug_player*DEBUG_GAME_NUM) ];
DEBUG_ONLYPLAY_ELSE
	p_shake_right	= &p_wk->shake_right[0];
DEBUG_ONLYPLAY_ENDIF


	switch( *p_seq )
	{	
	case SEQ_MAIN:
		if( TP_GetRectCont( &sc_right, &pos ) )
		{	
			if( SHAKESEARCH_Main( p_shake_right, &sc_right ) )
			{	
				int i;

DEBUG_ONLYPLAY_IF
				//0�Ȃ�Ύ����̔ԂȂ̂ŁA���͑���̔�
				if( p_wk->debug_player == 0 )
				{	
					p_wk->debug_player++;
					SEQ_Change( p_wk, SEQFUNC_StartGame );
				}
				else
				{
					MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, AURA_DEBUG_002, 0, 0 );
					SEQ_Change( p_wk, SEQFUNC_Result );
				}
DEBUG_ONLYPLAY_ELSE
				MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, AURA_STR_003, 0, 0 );
				SEQ_Change( p_wk, SEQFUNC_Result );
DEBUG_ONLYPLAY_ENDIF
			}

			//�E�^�b�`���oON
			{	
				TOUCH_EFFECT_SYS *p_touch;
				p_touch	= GRAPHIC_GetTouchEffWk( &p_wk->grp );
				TOUCH_EFFECT_SetVisible( p_touch, TOUCHEFFID_RIGHT, TRUE );
				TOUCH_EFFECT_SetPos( p_touch, TOUCHEFFID_RIGHT, pos.x, pos.y );
			}

		}
		else
		{	
			//�E�^�b�`���oON
			{	
				TOUCH_EFFECT_SYS *p_touch;
				p_touch	= GRAPHIC_GetTouchEffWk( &p_wk->grp );
				TOUCH_EFFECT_SetVisible( p_touch, TOUCHEFFID_RIGHT, FALSE );
			}
			SHAKESEARCH_Init( p_shake_right );
			*p_seq	= SEQ_RET;
		}
		break;

	case SEQ_RET:
		SEQ_Change( p_wk, SEQFUNC_StartGame );
		break;
	}


	if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_CLOSE )
	{
		PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
		p_wk->p_param->result	= IRCAURA_RESULT_RETURN;
		SEQ_End( p_wk );
	}

DEBUG_ONLYPLAY_IF
DEBUG_ONLYPLAY_ELSE
	//�V�[�����قȂ�`�F�b�N
	if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) != 0 )
	{	
		SEQ_Change( p_wk, SEQFUNC_SceneError );
	}
DEBUG_ONLYPLAY_ENDIF
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
DEBUG_ONLYPLAY_IF
	enum
	{	
		SEQ_INIT,
		SEQ_MAIN,
	};

	switch( *p_seq )
	{	
	case SEQ_INIT:
		p_wk->p_param->score	= CalcScore( p_wk );
		DEBUGAURA_PRINT_UpDate( p_wk );
		OS_Printf( "�I�[���`�F�b�N %d�_\n", p_wk->p_param->score );
		*p_seq	= SEQ_MAIN;
		break;

	case SEQ_MAIN:
		if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_CLOSE )
		{
			SEQ_End( p_wk );
		}
		break;
	}
DEBUG_ONLYPLAY_ELSE
	enum{	
		SEQ_RESULT,
		SEQ_MSG_PRINT,
		SEQ_SENDRESULT,
		SEQ_SCENE,
		SEQ_TIMING,
		SEQ_CALC,
		SEQ_MSG_PRINT_END,
		SEQ_END,

	};
	AURANET_RESULT_DATA result;

	switch( *p_seq )
	{	
	case SEQ_RESULT:
		if( COMPATIBLE_IRC_TimingSyncWait( p_wk->p_param->p_irc, COMPATIBLE_TIMING_NO_AURA_RESULT ) )
		{	
			*p_seq	= SEQ_MSG_PRINT;
		}
		break;

	case SEQ_MSG_PRINT:
		MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, AURA_STR_004, 0, 0 );
		*p_seq	= SEQ_SENDRESULT;
		break;

	case SEQ_SENDRESULT:
		result.trg_left			=	p_wk->trg_left[0];
		result.trg_right		=	p_wk->trg_right[0];
		result.shake_left		= p_wk->shake_left[0];
		result.shake_right	=	p_wk->shake_right[0];
		if( AURANET_SendResultData( &p_wk->net, &result ) )
		{	
			*p_seq	= SEQ_SCENE;
		}

		break;

	case SEQ_SCENE:
		//���̃V�[����ݒ�
		COMPATIBLE_IRC_SetScene(  p_wk->p_param->p_irc, COMPATIBLE_SCENE_RESULT );
		if( COMPATIBLE_IRC_SendScene( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_TIMING;
		}
		break;

	case SEQ_TIMING:
		if( COMPATIBLE_IRC_TimingSyncWait( p_wk->p_param->p_irc, COMPATIBLE_TIMING_NO_AURA_END ) )
		{	
			*p_seq	= SEQ_MSG_PRINT_END;
		}
		break;

	case SEQ_MSG_PRINT_END:
		if( p_wk->cnt >= RESULT_SEND_CNT )
		{	
			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, AURA_STR_005, 0, 0 );
			*p_seq	= SEQ_CALC;
		}
		break;

	case SEQ_CALC:
		p_wk->p_param->score	= CalcScore( p_wk );
		p_wk->p_param->result	= IRCAURA_RESULT_CLEAR;
		*p_seq	= SEQ_END;
		break;

	case SEQ_END:
		SEQ_End( p_wk );
		return;	//���̃A�T�[�g��ʉ߂��Ȃ�����
	}

DEBUG_ONLYPLAY_IF
DEBUG_ONLYPLAY_ELSE
	//�V�[�����قȂ�`�F�b�N
	if( *p_seq < SEQ_SCENE && COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) != 0 )
	{	
		SEQ_Change( p_wk, SEQFUNC_SceneError );
	}
DEBUG_ONLYPLAY_ENDIF

	//�߂�{�^��
	if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_CLOSE )
	{
		PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );	
		p_wk->p_param->result	= IRCAURA_RESULT_RETURN;
		SEQ_End( p_wk );
	}	

	//�ԊO���J�n�҂�
	if( SEQ_MSG_PRINT <= *p_seq && *p_seq <=  SEQ_MSG_PRINT_END )
	{	
		if( p_wk->cnt <= RESULT_SEND_CNT )
		{	
			p_wk->cnt++;
		}
	}

DEBUG_ONLYPLAY_ENDIF
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�[���G���[�V�[�P���X
 *
 *	@param	AURA_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_SceneError( AURA_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum
	{	
		SEQ_JUNCTION,
		SEQ_INIT,
		SEQ_MSG_PRINT,
		SEQ_TIMING,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_JUNCTION:
		NAGI_Printf( "ErrorScene!\n" );
		//��������Ȃ��
		if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) > 0 )
		{	
			*p_seq	= SEQ_INIT;
		}
		//���肪��Ȃ��
		else if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) < 0 )
		{	
			p_wk->p_param->score	= 0;
			p_wk->p_param->result	= IRCAURA_RESULT_RESULT;
			SEQ_End( p_wk );
		}
		else
		{	
			GF_ASSERT( 0 );
		}
		break;
	case SEQ_INIT:
		p_wk->p_param->score	= 0;
		p_wk->p_param->result	= IRCAURA_RESULT_RESULT;
		*p_seq	= SEQ_MSG_PRINT;
		break;
	case SEQ_MSG_PRINT:
		MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, AURA_STR_006, 0, 0 );
		*p_seq	= SEQ_TIMING;
		break;
	case SEQ_TIMING:
		if( COMPATIBLE_IRC_TimingSyncWait( p_wk->p_param->p_irc, COMPATIBLE_TIMING_NO_SCENE_ERROR ) )
		{	
			*p_seq	= SEQ_END;
		}
		break;
	case SEQ_END:
		SEQ_End( p_wk );
		break;
	}
}

//=============================================================================
/**
 *					NET
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	NET������
 *
 *	@param	AURANET_WORK *p_wk	���[�N
 *	@param	COMPATIBLE_IRC_SYS	�ԊO��
 *
 */
//-----------------------------------------------------------------------------
static void AURANET_Init( AURANET_WORK *p_wk, COMPATIBLE_IRC_SYS *p_irc )
{	
	GFL_STD_MemClear( p_wk, sizeof(AURANET_WORK) );
	p_wk->p_irc	= p_irc;

DEBUG_ONLYPLAY_IF
DEBUG_ONLYPLAY_ELSE
	COMPATIBLE_IRC_AddCommandTable( p_irc, GFL_NET_CMD_IRCAURA, sc_recv_tbl, NELEMS(sc_recv_tbl), p_wk );
DEBUG_ONLYPLAY_ENDIF
}
//----------------------------------------------------------------------------
/**
 *	@brief	NET�j��
 *
 *	@param	AURANET_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void AURANET_Exit( AURANET_WORK *p_wk )
{	
DEBUG_ONLYPLAY_IF
DEBUG_ONLYPLAY_ELSE
	COMPATIBLE_IRC_DelCommandTable( p_wk->p_irc, GFL_NET_CMD_IRCAURA );
DEBUG_ONLYPLAY_ENDIF

	GFL_STD_MemClear( p_wk, sizeof(AURANET_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ʏ��𑊎�֑���M
 *
 *	@param	AURANET_WORK *p_wk	���[�N
 *	@param	AURANET_RESULT_DATA *cp_data	�f�[�^
 *
 *	@retval	TRUE�Ȃ�΁A���肩���M
 *	@retval	FALSE�Ȃ�Ύ�M�҂�
 */
//-----------------------------------------------------------------------------
static BOOL AURANET_SendResultData( AURANET_WORK *p_wk, const AURANET_RESULT_DATA *cp_data )
{	
	enum
	{	
		SEQ_BUFF_SAVE,
		SEQ_SEND_DATA,
		SEQ_RECV_WAIT,
	};

	switch( p_wk->seq )
	{	
	case SEQ_BUFF_SAVE:
		p_wk->result_send	= *cp_data;
		p_wk->seq = SEQ_SEND_DATA;
		NAGI_Printf( "���ʃf�[�^���M�J�n\n" );
		break;

	case SEQ_SEND_DATA:
		{	
			NetID	netID;
			netID	= GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
			if( netID == 0 )
			{	
				netID	= 1;
			}else{	
				netID	= 0;
			}
			if( COMPATIBLE_IRC_SendDataEx( p_wk->p_irc, NETRECV_RESULT,
					sizeof(AURANET_RESULT_DATA), &p_wk->result_send, netID, FALSE, FALSE, TRUE ) )
			{	
				NAGI_Printf( "���ʃf�[�^���M�����A����҂�\n" );
				p_wk->seq	= SEQ_RECV_WAIT;
			}
		}
		break;

	case SEQ_RECV_WAIT:
		if( p_wk->is_recv )
		{
			NAGI_Printf( "���ʃf�[�^��M����\n" );
			p_wk->is_recv	= FALSE;
			p_wk->seq	= 0;
			return TRUE;
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ʃf�[�^�󂯎��
 *
 *	@param	AURANET_WORK *p_wk	���[�N
 *	@param	*p_data							�f�[�^�󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void AURANET_GetResultData( const AURANET_WORK *cp_wk, AURANET_RESULT_DATA *p_data )
{	
	*p_data	= cp_wk->result_recv;
}
//=============================================================================
/**
 *				NETRECV
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���ʃf�[�^�󂯎���M�R�[���o�b�N
 *
 *	@param	const int netID	�l�b�gID
 *	@param	int size				�T�C�Y
 *	@param	void* p_data		�f�[�^
 *	@param	p_work					���[�N
 *	@param	p_net_handle		�n���h��
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void NETRECV_Result( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle )
{	
	AURANET_WORK *p_wk	= p_work;

	if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
	{
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return;	//�����̃f�[�^�͖���
	}

//	GFL_STD_MemCopy( cp_data, &p_wk->result_recv, sizeof(AURANET_RESULT_DATA) );
	NAGI_Printf("���ʃf�[�^�󂯎�芮��\n" );
	p_wk->is_recv		= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ʃf�[�^�󂯎��o�b�t�@�|�C���^
 *
 *	@param	netID
 *	@param	pWork
 *	@param	size
 *
 *	@return	�o�b�t�@�|�C���^
 */
//-----------------------------------------------------------------------------
static u8* NETRECV_GetBufferAddr(int netID, void* p_work, int size)
{	
	AURANET_WORK *p_wk	= p_work;

	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return (u8*)&p_wk->result_recv_my;
	}
	else
	{	
		return (u8*)&p_wk->result_recv;
	}

}

//=============================================================================
/**
 *				�~�v���~�e�B�u
 */
//=============================================================================
#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	�~�v���~�e�B�u���쐬
 *
 *	@param	CIRCLE_WORK *p_wk		���[�N
 *	@param	vtx_num							���_��
 *	@param	HEAPID							�������m�ۗpID
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_Init( CIRCLE_WORK *p_wk, u32 vtx_num, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(CIRCLE_WORK) );

	//������
	p_wk->vtx_num	= vtx_num;
	CIRCLE_SetAlpha( p_wk, 31 );
	{	
		MtxFx33	rot;
		MTX_Identity33( &rot );
		CIRCLE_SetRot( p_wk, &rot );
	}

	//���_�쐬
	p_wk->p_vtx	= GFL_HEAP_AllocMemory( heapID, sizeof(VecFx16)*p_wk->vtx_num );
	GFL_STD_MemClear( p_wk->p_vtx, sizeof(VecFx16)*p_wk->vtx_num );

	//�~�̒��_���W�쐬
	{	
		int i;
		for( i = 0; i < p_wk->vtx_num; i++ )
		{	
			p_wk->p_vtx[i].x	=	FX_CosIdx( ( i * 360 / p_wk->vtx_num) * 0xFFFF / 360 );
			p_wk->p_vtx[i].y	=	FX_SinIdx( ( i * 360 / p_wk->vtx_num) * 0xFFFF / 360 );
			p_wk->p_vtx[i].z	=	0;
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�~�v���~�e�B�u��j��
 *
 *	@param	CIRCLE_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_Exit( CIRCLE_WORK *p_wk )
{	
	GFL_HEAP_FreeMemory( p_wk->p_vtx );
	GFL_STD_MemClear( p_wk, sizeof(CIRCLE_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�~��`��
 *
 *	@param	CIRCLE_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_Draw( CIRCLE_WORK *p_wk )
{	
	if( p_wk->is_visible )
	{	
		G3_PushMtx();

		//��]�ݒ�
		G3_MultMtx33( &p_wk->status.rotate );

		//�ړ��ݒ�
		G3_Translate( p_wk->status.trans.x, p_wk->status.trans.y, p_wk->status.trans.z );

		//�T�C�Y�ݒ�
		G3_Scale( p_wk->status.scale.x, p_wk->status.scale.y, p_wk->status.scale.z);

		//�}�e���A���ݒ�
		G3_MaterialColorDiffAmb(GX_RGB(16, 16, 16), GX_RGB(16, 16, 16), FALSE );
		G3_MaterialColorSpecEmi(GX_RGB( 16, 16, 16 ), GX_RGB( 31,31,31 ), FALSE );
		G3_PolygonAttr( GX_LIGHTMASK_0123,GX_POLYGONMODE_MODULATE,GX_CULL_BACK,0,p_wk->alpha,0 );
	
		//�`��
		G3_Begin( GX_BEGIN_TRIANGLES );
		{
			int i;
			G3_Color(p_wk->vtx_color);
			for( i = 0; i < p_wk->vtx_num - 1; i++ )
			{	
				Circle_DrawLine( &p_wk->p_vtx[i], &p_wk->p_vtx[i+1] );
			}
			Circle_DrawLine( &p_wk->p_vtx[i], &p_wk->p_vtx[0] );
		}
		G3_End();
	
		G3_PopMtx(1);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���_�J���[�ݒ�
 *
 *	@param	CIRCLE_WORK *p_wk	���[�N
 *	@param	color							�J���[
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_SetVtxColor( CIRCLE_WORK *p_wk, GXRgb color )
{	
	p_wk->vtx_color	=	color;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A���t�@�����Ă�
 *
 *	@param	CIRCLE_WORK *p_wk	���[�N
 *	@param	alpha							�A���t�@	�i0 ~ 31�j
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_SetAlpha( CIRCLE_WORK *p_wk, u8 alpha )
{	
	p_wk->alpha	= alpha;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�g�����X���[�V�����ݒ�
 *
 *	@param	CIRCLE_WORK *p_wk		���[�N
 *	@param	VecFx32 *cp_trans		�ړ�
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_SetTrans( CIRCLE_WORK *p_wk, const VecFx32 *cp_trans )
{	
	p_wk->status.trans	= *cp_trans;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�P�[���l�ݒ�
 *
 *	@param	CIRCLE_WORK *p_wk		���[�N
 *	@param	VecFx32 *cp_scale		�X�P�[��
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_SetScale( CIRCLE_WORK *p_wk, const VecFx32 *cp_scale )
{	
	p_wk->status.scale	= *cp_scale;
}
//----------------------------------------------------------------------------
/**
 *	@brief	��]�s��ݒ�
 *
 *	@param	CIRCLE_WORK *p_wk	���[�N
 *	@param	MtxFx33 *cp_rot		��]�s��
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_SetRot( CIRCLE_WORK *p_wk, const MtxFx33 *cp_rot )
{	
	p_wk->status.rotate	= *cp_rot;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\���t���O�ݒ�
 *
 *	@param	CIRCLE_WORK *p_wk	���[�N
 *	@param	is_visible				TRUE�Ȃ�Ε\���@FALSE�Ȃ�Δ�\��
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_SetVisible( CIRCLE_WORK *p_wk, BOOL is_visible )
{	
	p_wk->is_visible	= is_visible;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ړ��擾	
 *
 *	@param	const CIRCLE_WORK *cp_wk	���[�N
 *	@param	*p_trans									�ړ��󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_GetTrans( const CIRCLE_WORK *cp_wk, VecFx32 *p_trans )
{	
	*p_trans	= cp_wk->status.trans;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�g��k���擾
 *
 *	@param	const CIRCLE_WORK *cp_wk	���[�N
 *	@param	*p_scale									�g�k�󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_GetScale( const CIRCLE_WORK *cp_wk, VecFx32 *p_scale )
{	
	*p_scale	= cp_wk->status.scale;
}
//----------------------------------------------------------------------------
/**
 *	@brief	��]�s��擾
 *
 *	@param	const CIRCLE_WORK *cp_wk	���[�N
 *	@param	*p_rot										��]�s��󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_GetRot( const CIRCLE_WORK *cp_wk, MtxFx33 *p_rot )
{	
	*p_rot	= cp_wk->status.rotate;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�\���t���O�擾
 *
 *	@param	const CIRCLE_WORK *cp_wk	���[�N
 *
 *	@return	TRUE�Ȃ�Ε\����	FALSE�Ȃ�Δ�\��
 */
//-----------------------------------------------------------------------------
static BOOL CIRCLE_GetVisible( const CIRCLE_WORK *cp_wk )
{	
	return cp_wk->is_visible;
}
//----------------------------------------------------------------------------
/**
 *	@brief	����`��
 *
 *	@param	VecFx16 *p_start		�J�n���W
 *	@param	*p_end							�I�����W
 *
 */
//-----------------------------------------------------------------------------
static void Circle_DrawLine( VecFx16 *p_start, VecFx16 *p_end )
{	
#if 1
	G3_Vtx( p_start->x, p_start->y, p_start->z);
	G3_Vtx( p_end->x, p_end->y, p_end->z);
	G3_Vtx( p_start->x, p_start->y, p_start->z);
#else
	G3_Vtx( 0, 0, 0);
	G3_Vtx( p_start->x, p_start->y, p_start->z);
	G3_Vtx( p_end->x, p_end->y, p_end->z);
#endif
}
#endif
//=============================================================================
/**
 *				���o
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�^�b�`���o�A������
 *
 *	@param	TOUCH_EFFECT_SYS *p_wk		���[�N
 *	@param	heapID										�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void TOUCH_EFFECT_Init( TOUCH_EFFECT_SYS *p_sys, const GRAPHIC_3D_WORK *cp_g3d, HEAPID heapID )
{	
	GFL_STD_MemClear( p_sys, sizeof(TOUCH_EFFECT_SYS) );

	{	
		int i, j;
		VecFx32	scale;
		for( j = 0; j < TOUCHEFFID_MAX; j++ )
		{	
			p_sys->p_mdl[j]	= GRAPHIC_3D_GetObj( cp_g3d, j );
			G3DMDL_SetVisible( p_sys->p_mdl[j], FALSE );
#if 0
			TOUCH_EFFECT_WORK	*p_wk	= &p_sys->wk[j];
			for( i = 0; i < CIRCLE_MAX; i++ )
			{	
				CIRCLE_Init( &p_wk->c[i], CIRCLE_VTX_MAX, heapID );
				CIRCLE_SetVtxColor( &p_wk->c[i], GX_RGB(0,31,31) );
				p_wk->rot[i]	= 0;
				p_wk->rot_add[i]	= 0x1F;
				VEC_Set( &scale, FX_SinIdx(p_wk->rot[i])/4,
							FX_SinIdx(p_wk->rot[i])/4, 0 );
				CIRCLE_SetScale( &p_wk->c[i], &scale );
				CIRCLE_SetVisible( &p_wk->c[i], FALSE );
			}
#endif
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�j��
 *
 *	@param	TOUCH_EFFECT_SYS *p_sys		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void TOUCH_EFFECT_Exit( TOUCH_EFFECT_SYS *p_sys )
{	
	{	
		int i,j;
		for( j = 0; j < TOUCHEFFID_MAX; j++ )
		{	

#if 0
			TOUCH_EFFECT_WORK	*p_wk	= &p_sys->wk[j];
			for( i = 0; i < CIRCLE_MAX; i++ )
			{	
				CIRCLE_Exit( &p_wk->c[i] );
			}
#endif
		}
	}
	GFL_STD_MemClear( p_sys, sizeof(TOUCH_EFFECT_SYS) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^�b�`���o	�`��
 *
 *	@param	TOUCH_EFFECT_SYS *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void TOUCH_EFFECT_Draw( TOUCH_EFFECT_SYS *p_sys )
{	
	int i, j;
	VecFx32	scale;

	for( j = 0; j < TOUCHEFFID_MAX; j++ )
	{	

		G3DMDL_Draw( p_sys->p_mdl[i] );
#if 0
		TOUCH_EFFECT_WORK	*p_wk	= &p_sys->wk[j];

		for( i = 0; i < CIRCLE_MAX; i++ )
		{	
			if( CIRCLE_GetVisible( &p_wk->c[i] ) )
			{	
				p_wk->rot[i]	+= p_wk->rot_add[i];
			}
			{
				VEC_Set( &scale, FX_SinIdx(p_wk->rot[i])/4,
							FX_SinIdx(p_wk->rot[i])/4, 0 );
				CIRCLE_SetScale( &p_wk->c[i], &scale );
			}

			CIRCLE_Draw( &p_wk->c[i] );
		}
#endif
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�b�`���o	�\���ݒ�
 *
 *	@param	TOUCH_EFFECT_SYS *p_wk	���[�N
 *	@param	is_visible							�\��ON,OFF
 *
 */
//-----------------------------------------------------------------------------
static void TOUCH_EFFECT_SetVisible( TOUCH_EFFECT_SYS *p_sys, TOUCHEFFID id, BOOL is_visible )
{	
	int i;

	G3DMDL_SetVisible( p_sys->p_mdl[id], is_visible );
#if 0
	TOUCH_EFFECT_WORK	*p_wk	= &p_sys->wk[id];

	for( i = 0; i < CIRCLE_MAX; i++ )
	{	
		CIRCLE_SetVisible( &p_wk->c[i], is_visible );
		if( !is_visible )
		{	
			p_wk->rot[i]	= 0;
		}
	}
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	���W�ݒ�
 *
 *	@param	TOUCH_EFFECT_SYS *p_wk		���[�N
 *	@param	x													LCD���WX
 *	@param	y													LCD���WY
 *
 */
//-----------------------------------------------------------------------------
static void TOUCH_EFFECT_SetPos( TOUCH_EFFECT_SYS *p_sys, TOUCHEFFID id, u32 x, u32 y )
{	
	enum
	{	
		AURA_DISTANCE_OFS	= 64,
	};

//	TOUCH_EFFECT_WORK	*p_wk	= &p_sys->wk[id];
	VecFx32	near;
	VecFx32	far;
	VecFx32	v;
	int i;

	switch( id )
	{
	case TOUCHEFFID_LEFT:
		NNS_G3dScrPosToWorldLine( x, y, &near, &far );
		VEC_Subtract( &far, &near, &v );
		VEC_Normalize( &v, &v );
		VEC_MultAdd( FX32_CONST(AURA_DISTANCE_OFS), &v, &near, &near );
		G3DMDL_SetPos( p_sys->p_mdl[id], &near );

#if 0
		for( i = 0; i < CIRCLE_MAX; i++ )
		{	
			CIRCLE_SetTrans( &p_wk->c[i], &near );
		}
#endif
		p_sys->left.x	= x;
		p_sys->left.y	= y;
		break;

	case TOUCHEFFID_RIGHT:
		{
			GFL_POINT	pos;
			pos.x		= p_sys->left.x + (x - p_sys->left.x) * 2;
			pos.y		= p_sys->left.y + (y - p_sys->left.y) * 2;
	//		NAGI_Printf( "X ��%d ��%d �E%d", p_wk->left.x, x, pos.x );
		//	NAGI_Printf( "Y ��%d ��%d �E%d", p_wk->left.y, y, pos.y );
			NNS_G3dScrPosToWorldLine( pos.x, pos.y, &near, &far );
			VEC_Subtract( &far, &near, &v );
			VEC_Normalize( &v, &v );
			VEC_MultAdd( FX32_CONST(AURA_DISTANCE_OFS), &v, &near, &near );
			G3DMDL_SetPos( p_sys->p_mdl[id], &near );

#if 0
			for( i = 0; i < CIRCLE_MAX; i++ )
			{	
				CIRCLE_SetTrans( &p_wk->c[i], &near );
			}
#endif
		}
		break;
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

#ifdef DEBUG_AURA_MSG
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
		u32 pos_score;
		OS_Printf( "�����݂��̍��W�v��\n" );
		pos1	= p_wk->trg_left[p_wk->debug_game_cnt + (0*DEBUG_GAME_NUM)];
		pos2	= p_wk->trg_left[p_wk->debug_game_cnt + (1*DEBUG_GAME_NUM)];
		ret	= ((u32)(pos1.x-pos2.x)*(u32)(pos1.x-pos2.x))+((u32)(pos1.y-pos2.y)*(u32)(pos1.y-pos2.y));
		if( 0 < ret & ret <= RANGE_POS_SCORE_00 )
		{	
			pos_score	= VAL_POS_SCORE_00;
		}
		else if( RANGE_POS_SCORE_00 < ret & ret <= RANGE_POS_SCORE_01 )
		{	
			pos_score	= VAL_POS_SCORE_01;
		}
		else if( RANGE_POS_SCORE_01 < ret & ret <= RANGE_POS_SCORE_02 )
		{	
			pos_score	= VAL_POS_SCORE_02;
		}
		else if( RANGE_POS_SCORE_02 < ret & ret <= RANGE_POS_SCORE_03 )
		{	
			pos_score	= VAL_POS_SCORE_03;
		}
		else if( RANGE_POS_SCORE_03 < ret & ret <= RANGE_POS_SCORE_04 )
		{	
			pos_score	= VAL_POS_SCORE_04;
		}
		else{	
			pos_score	= VAL_POS_SCORE_05;
		}
		OS_Printf( "�����@%d�_\n", pos_score );	}

	{
		GFL_POINT temp1, temp2;
		u32 shake1, shake2;
		u8 idx1, idx2;
		u16 ret;
		u16 score;
		OS_Printf( "�����݂��̃u���v�Z����\n" );

		idx1	= p_wk->debug_game_cnt + (0*DEBUG_GAME_NUM);
		idx2	= p_wk->debug_game_cnt + (1*DEBUG_GAME_NUM);

		score	= 0;
		for( i = 1; i <TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			temp1.x	= p_wk->shake_left[idx1].shake[i].x - p_wk->shake_left[idx1].shake[0].x;
			temp1.y	= p_wk->shake_left[idx1].shake[i].y - p_wk->shake_left[idx1].shake[0].y;
			temp2.x	= p_wk->shake_left[idx2].shake[i].x - p_wk->shake_left[idx2].shake[0].x;
			temp2.y	= p_wk->shake_left[idx2].shake[i].y - p_wk->shake_left[idx2].shake[0].y;

			shake1	= MATH_IAbs( temp1.x ) + MATH_IAbs( temp1.y );
			shake2	= MATH_IAbs( temp2.x ) + MATH_IAbs( temp2.y );

			if( shake1 > shake2 )
			{	
				ret	= shake1 - shake2;
			}
			else
			{	
				ret	= shake2 - shake1;
			}

			if( RANGE_SHAKE_SCORE_00(ret) )
			{	
				score	+= VAL_SHAKE_SCORE_00;
				OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_00 );
			}
			else if( RANGE_SHAKE_SCORE_01(ret) )
			{	
				score	+= VAL_SHAKE_SCORE_01;
				OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_01 );
			}
			else if( RANGE_SHAKE_SCORE_02(ret) )
			{	
				score	+= VAL_SHAKE_SCORE_02;
				OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_02 );
			}
			else if( RANGE_SHAKE_SCORE_03(ret) )
			{	
				score	+= VAL_SHAKE_SCORE_03;
				OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_03 );
			}
			else if( RANGE_SHAKE_SCORE_04(ret) )
			{	
#if VAL_SHAKE_SCORE_04
					score	+= VAL_SHAKE_SCORE_04;
#endif //VAL_SHAKE_SCORE_04
					OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_04 );
			}
		}

		OS_Printf( "�u������ %d�_\n", score/9 );
	}


	OS_Printf( "����%d�Ԗڂ̃Q�[���@�v�����g�I������������������\n", p_wk->debug_game_cnt );
}
#endif //DEBUG_AURA_MSG
//----------------------------------------------------------------------------
/**
 *	@brief	�X�R�A�v�Z
 *
 *	@param	AURA_MAIN_WORK *p_wk	���[�N
 *
 *	@return	�X�R�A
 */
//-----------------------------------------------------------------------------
static u8	 CalcScore( AURA_MAIN_WORK *p_wk )
{	
	AURANET_RESULT_DATA	my;
	AURANET_RESULT_DATA	you;

	u32	pos_score;
	u32 shake_score;
DEBUG_ONLYPLAY_IF
	my.trg_left		= p_wk->trg_left[0];
	my.trg_right	= p_wk->trg_right[0];
	my.shake_left	= p_wk->shake_left[0];
	my.shake_right= p_wk->shake_right[0];

	you.trg_left		= p_wk->trg_left[p_wk->debug_game_cnt + (1*DEBUG_GAME_NUM)];
	you.trg_right		= p_wk->trg_right[p_wk->debug_game_cnt + (1*DEBUG_GAME_NUM)];
	you.shake_left	= p_wk->shake_left[p_wk->debug_game_cnt + (1*DEBUG_GAME_NUM)];
	you.shake_right	= p_wk->shake_right[p_wk->debug_game_cnt + (1*DEBUG_GAME_NUM)];
DEBUG_ONLYPLAY_ELSE
	my.trg_left		= p_wk->trg_left[0];
	my.trg_right	= p_wk->trg_right[0];
	my.shake_left	= p_wk->shake_left[0];
	my.shake_right= p_wk->shake_right[0];

	AURANET_GetResultData( &p_wk->net, &you );

DEBUG_ONLYPLAY_ENDIF
	//���W�X�R�A
	{	
		GFL_POINT	pos1, pos2;
		u32 ret;
		OS_Printf( "�����݂��̍��W�v��\n" );
		pos1	= my.trg_left;
		pos2	= you.trg_left;
		ret	= ((u32)(pos1.x-pos2.x)*(u32)(pos1.x-pos2.x))+((u32)(pos1.y-pos2.y)*(u32)(pos1.y-pos2.y));
		if( 0 < ret & ret <= RANGE_POS_SCORE_00 )
		{	
			pos_score	= VAL_POS_SCORE_00;
		}
		else if( RANGE_POS_SCORE_00 < ret & ret <= RANGE_POS_SCORE_01 )
		{	
			pos_score	= VAL_POS_SCORE_01;
		}
		else if( RANGE_POS_SCORE_01 < ret & ret <= RANGE_POS_SCORE_02 )
		{	
			pos_score	= VAL_POS_SCORE_02;
		}
		else if( RANGE_POS_SCORE_02 < ret & ret <= RANGE_POS_SCORE_03 )
		{	
			pos_score	= VAL_POS_SCORE_03;
		}
		else if( RANGE_POS_SCORE_03 < ret & ret <= RANGE_POS_SCORE_04 )
		{	
			pos_score	= VAL_POS_SCORE_04;
		}
		else{	
			pos_score	= VAL_POS_SCORE_05;
		}
		OS_Printf("�����̍�����W\n");
		OS_Printf("X %d Y %d\n", 
				pos1.x,
				pos1.y);
	
		OS_Printf("����̍�����W\n");
		OS_Printf("X %d Y %d\n", 
				pos2.x,
				pos2.y);
		OS_Printf( "�����@%d�_\n", pos_score );
	}

	//�u���X�R�A
	{	
		int i;
		GFL_POINT temp1, temp2;
		u32 shake1, shake2;
		u16 ret;
		shake_score	= 0;
		for( i = 1; i <TOUCH_COUNTER_SHAKE_MAX; i++ )
		{	
			temp1.x	= my.shake_left.shake[i].x - my.shake_left.shake[0].x;
			temp1.y	= my.shake_left.shake[i].y - my.shake_left.shake[0].y;
			temp2.x	= you.shake_left.shake[i].x - you.shake_left.shake[0].x;
			temp2.y	= you.shake_left.shake[i].y - you.shake_left.shake[0].y;

			OS_Printf( "�����̃u��[%d] X %d Y %d\n", i, temp1.x, temp1.y );
			OS_Printf( "����̃u��[%d] X %d Y %d\n", i, temp2.x, temp2.y );
			


			shake1	= MATH_IAbs( temp1.x ) + MATH_IAbs( temp1.y );
			shake2	= MATH_IAbs( temp2.x ) + MATH_IAbs( temp2.y );

			if( shake1 > shake2 )
			{	
				ret	= shake1 - shake2;
			}
			else
			{	
				ret	= shake2 - shake1;
			}

			if( RANGE_SHAKE_SCORE_00(ret) )
			{	
				shake_score	+= VAL_SHAKE_SCORE_00;
				OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_00 );
			}
			else if( RANGE_SHAKE_SCORE_01(ret) )
			{	
				shake_score	+= VAL_SHAKE_SCORE_01;
				OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_01 );
			}
			else if( RANGE_SHAKE_SCORE_02(ret) )
			{	
				shake_score	+= VAL_SHAKE_SCORE_02;
				OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_02 );
			}
			else if( RANGE_SHAKE_SCORE_03(ret) )
			{	
				shake_score	+= VAL_SHAKE_SCORE_03;
				OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_03 );
			}
			else if( RANGE_SHAKE_SCORE_04(ret) )
			{	
#if VAL_SHAKE_SCORE_04
					shake_score	+= VAL_SHAKE_SCORE_04;
#endif //VAL_SHAKE_SCORE_04
					OS_Printf( "�u��[%d]�@%d�_���Z\n", i, VAL_SHAKE_SCORE_04 );
			}
		}
		shake_score	/= 9;
		OS_Printf( "�u���@%d�_\n", shake_score );
	}

	return (pos_score + shake_score)/2;
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

//----------------------------------------------------------------------------
/**
 *	@brief	�K�C�hBG�̕\���ݒ�
 *
 *	@param	GUIDE_VISIBLE_MASK msk	�}�X�N�l
 *	@param	heapID									�ǂݍ��݃e���|�����p
 */
//-----------------------------------------------------------------------------
static void GUIDE_SetVisible( GUIDE_VISIBLE_MASK msk, HEAPID heapID )
{	
	enum
	{	
		GUIDE_DST_L_X	= 0,
		GUIDE_DST_L_Y	= 0,
		GUIDE_DST_L_W	= 16,
		GUIDE_DST_L_H	= 24,

		GUIDE_DST_R_X	= 16,
		GUIDE_DST_R_Y	= 0,
		GUIDE_DST_R_W	= 16,
		GUIDE_DST_R_H	= 24,
	};

	ARCHANDLE	*p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCCOMPATIBLE, heapID );

	if( msk & GUIDE_VISIBLE_LEFT )
	{	
		ARCHDL_UTIL_TransVramScreenEx( p_handle, NARC_irccompatible_gra_aura_bg_guide_l_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_GUIDE],
				0, 0, 0, 16, 24, 
				GUIDE_DST_L_X, GUIDE_DST_L_Y, GUIDE_DST_L_W, GUIDE_DST_L_H,
				AURA_BG_PAL_M_09, FALSE, heapID );
	}
	else
	{	
		GFL_BG_FillScreen( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_GUIDE], 0,
				GUIDE_DST_L_X,GUIDE_DST_L_Y,GUIDE_DST_L_W,GUIDE_DST_L_H,
				AURA_BG_PAL_M_09 );
	}

	if( msk & GUIDE_VISIBLE_RIGHT )
	{	
		ARCHDL_UTIL_TransVramScreenEx( p_handle, NARC_irccompatible_gra_aura_bg_guide_r_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_GUIDE],
				0, 0, 0, 16, 24, 
				GUIDE_DST_R_X, GUIDE_DST_R_Y, GUIDE_DST_R_W, GUIDE_DST_R_H,
				AURA_BG_PAL_M_09, FALSE, heapID );
	}
	else
	{	
		GFL_BG_FillScreen( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_GUIDE], 0,
				GUIDE_DST_R_X,GUIDE_DST_R_Y,GUIDE_DST_R_W,GUIDE_DST_R_H,
				AURA_BG_PAL_M_09 );
	}

	GFL_BG_LoadScreenReq(sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_GUIDE]);

	GFL_ARC_CloseDataHandle( p_handle );
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
						BACKOBJ_ONE_Start( &p_wk->wk[i], &end, &start, sync );
					}
					else
					{	
						BACKOBJ_ONE_Start( &p_wk->wk[i], &start, &end, sync );
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
					u16 rot	= GFUser_GetPublicRand(0xFFFF);
					start.x	= (FX_SinIdx(rot) * 256) >> FX32_SHIFT;
					start.y	= (FX_CosIdx(rot) * 256) >> FX32_SHIFT;
					end.x		= 128;
					end.y		= 96;
					sync	= BACKOBJ_ONE_MOVE_SYNC_MIN + GFUser_GetPublicRand(BACKOBJ_ONE_MOVE_SYNC_DIF);
					BACKOBJ_ONE_Start( &p_wk->wk[i], &start, &end, sync );
					break;
				}
			}
		}
		break;
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
static void BACKOBJ_StartEmit( BACKOBJ_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	int i;
	u32 sync;
	u16 rot;
	GFL_POINT	end;
	for( i = 0; i < BACKOBJ_CLWK_MAX; i++ )
	{	
		if( !BACKOBJ_ONE_IsMove( &p_wk->wk[i] ) )
		{
			rot	= GFUser_GetPublicRand(0xFFFF);
			end.x	= (FX_SinIdx(rot) * 256) >> FX32_SHIFT;
			end.y	= (FX_CosIdx(rot) * 256) >> FX32_SHIFT;
			sync	= BACKOBJ_ONE_MOVE_SYNC_MIN + GFUser_GetPublicRand(BACKOBJ_ONE_MOVE_SYNC_DIF);
			BACKOBJ_ONE_Start( &p_wk->wk[i], cp_pos, &end, sync );
		}
	}
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
static void BACKOBJ_ONE_Start( BACKOBJ_ONE *p_wk, const GFL_POINT *cp_start, const GFL_POINT *cp_end, u32 sync )
{	
	p_wk->is_req		= TRUE;
	VEC_Set( &p_wk->start, FX32_CONST(cp_start->x), FX32_CONST(cp_start->y), 0 );
	VEC_Set( &p_wk->end, FX32_CONST(cp_end->x), FX32_CONST(cp_end->y), 0 );
	p_wk->sync_now	= 0;
	p_wk->sync_max	= sync;
	GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
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

		if( p_wk->shake_cnt == 0 )
		{	
			TP_GetRectCont( cp_rect, &p_wk->shake[p_wk->shake_idx++] );
			GF_ASSERT_MSG( p_wk->shake_idx < TOUCH_COUNTER_SHAKE_MAX, 
					"�u���v���C���f�b�N�X�G���[�ł� %d\n" , p_wk->shake_idx );
		}

		//�v��
		if( p_wk->shake_cnt++ > TOUCH_COUNTER_SHAKE_SYNC )
		{	
			p_wk->shake_cnt	= 0;
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

	//���C�h������STRBUF�ɕϊ�
	p_strbuf	= DEBUGPRINT_CreateWideChar( cp_str, p_wk->heapID );

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

//----------------------------------------------------------------------------
/**
 *	@brief	Wide�������STRBUF�Ɋi�[
 *
 *	@param	const u16 *cp_char	���C�h������
 *	@param	heapID							�q�[�vID
 *
 *	@return	STRBUF
 */
//-----------------------------------------------------------------------------
static STRBUF * DEBUGPRINT_CreateWideChar( const u16 *cp_str, HEAPID heapID )
{	
	STRBUF	*p_strbuf;
	STRCODE	str[128];
	u16	strlen;

	//STRBUF�p�ɕϊ�
	strlen	= wcslen(cp_str);
	GFL_STD_MemCopy(cp_str, str, strlen*2);
	str[strlen]	= GFL_STR_GetEOMCode();

	//STRBUF�ɓ]��
	p_strbuf	= GFL_STR_CreateBuffer( strlen*2, heapID );
	GFL_STR_SetStringCode( p_strbuf, str);

	return p_strbuf;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���l��Wide�������STRBUF�Ɋi�[
 *
 *	@param	const u16 *cp_char	���C�h������
 *	@param	number							���l
 *	@param	heapID							�q�[�vID
 *
 *	@return	STRBUF
 */
//-----------------------------------------------------------------------------
static STRBUF * DEBUGPRINT_CreateWideCharNumber( const u16 *cp_str, int number, HEAPID heapID )
{	
	u16	str[128];
	swprintf( str, 128, cp_str, number ); 
	return DEBUGPRINT_CreateWideChar( str, heapID );
}
//#endif //DEBUG_AURA_MSG 

