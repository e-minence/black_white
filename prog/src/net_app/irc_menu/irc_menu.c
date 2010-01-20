//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_meu.c
 *	@brief	�ԊO���~�j�Q�[���@���j���[���
 *	@author	Toru=Nagihashi
 *	@data		2009.05.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>

//	system
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"

//	module
#include "infowin/infowin.h"

#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/bmp_winframe.h"
#include "net_app/compatible_irc_sys.h"

#include "net_app/irc_appbar.h"

//sound
#include "../irc_compatible/irc_compatible_snd.h"

//	archive
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_irc_compatible.h"
#include "irccompatible_gra.naix"

#include "net_app/irc_menu.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#ifdef PM_DEBUG
static int sc_debug_num = FX32_ONE;

#endif

#define DEG_TO_IDX(x) ( 0xFFFF * x / 360 )


//-------------------------------------
///	���y
//=====================================
#define MENU_SE_DECIDE	(SEQ_SE_DECIDE1)
#define MENU_SE_CANCEL	(SEQ_SE_CANCEL1)


//-------------------------------------
///	�p���b�g
//=====================================
enum{	
	// ���C�����BG
	IRC_MENU_BG_PAL_M_00 = 0,//�w�i
	IRC_MENU_BG_PAL_M_01,		//
	IRC_MENU_BG_PAL_M_02,		//
	IRC_MENU_BG_PAL_M_03,		// 
	IRC_MENU_BG_PAL_M_04,		//	�o�[�̐F������
	IRC_MENU_BG_PAL_M_05,		//�@INFOWIN
	IRC_MENU_BG_PAL_M_06,		// �w�i�����܂�	
	IRC_MENU_BG_PAL_M_07,		// �w�i
	IRC_MENU_BG_PAL_M_08,		// �t�H���g
	IRC_MENU_BG_PAL_M_09,		// �o�[�̐F�����邢
	IRC_MENU_BG_PAL_M_10,		// �o�[�̐F�����炢
	IRC_MENU_BG_PAL_M_11,		// �o�[�̐F�㕁��
	IRC_MENU_BG_PAL_M_12,		// �o�[�̐F�㖾�邢
	IRC_MENU_BG_PAL_M_13,		// �o�[�̐F��Â�
	IRC_MENU_BG_PAL_M_14,		//APPBAR
	IRC_MENU_BG_PAL_M_15,		// APPBAR

	// �T�u���BG
	IRC_MENU_BG_PAL_S_00 = 0,	//�w�i
	IRC_MENU_BG_PAL_S_01,		// 
	IRC_MENU_BG_PAL_S_02,		// 
	IRC_MENU_BG_PAL_S_03,		// 
	IRC_MENU_BG_PAL_S_04,		// 
	IRC_MENU_BG_PAL_S_05,		//
	IRC_MENU_BG_PAL_S_06,		// �w�i�����܂�	//���ʗp�o�[���t�H���g
	IRC_MENU_BG_PAL_S_07,		// �w�i
	IRC_MENU_BG_PAL_S_08,		// �w�i
	IRC_MENU_BG_PAL_S_09,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_10,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_11,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_12,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_13,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_14,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_15,		// �g�p���ĂȂ�

	// ���C�����OBJ
	IRC_MENU_OBJ_PAL_M_00 = 0,//
	IRC_MENU_OBJ_PAL_M_01,		// 
	IRC_MENU_OBJ_PAL_M_02,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_03,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_04,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_05,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_06,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_07,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_08,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_09,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_10,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_11,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_12,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_13,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_M_14,		//	
	IRC_MENU_OBJ_PAL_M_15,		// 

	// �T�u���OBJ
	IRC_MENU_OBJ_PAL_S_00 = 0,	//
	IRC_MENU_OBJ_PAL_S_01,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_02,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_03,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_04,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_05,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_06,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_07,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_08,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_09,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_10,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_11,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_12,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_13,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_14,		// �g�p���ĂȂ�
	IRC_MENU_OBJ_PAL_S_15,		// �g�p���ĂȂ�
};

//-------------------------------------
///	����
//=====================================
#define TEXTSTR_PLT_NO				(IRC_MENU_BG_PAL_S_00)
#define TEXTSTR_BUFFER_LENGTH	(255)

//-------------------------------------
///	�ʒu
//=====================================
#define	MSGWND_MSG_X	(1)
#define	MSGWND_MSG_Y	(18)
#define	MSGWND_MSG_W	(30)
#define	MSGWND_MSG_H	(5)

#define	MSGWND_GAME_X	(3)
#define	MSGWND_GAME_Y	(4)
#define	MSGWND_GAME_W	(26)
#define	MSGWND_GAME_H	(5)

#define	MSGWND_RANKING_X	(3)
#define	MSGWND_RANKING_Y	(14)
#define	MSGWND_RANKING_W	(26)
#define	MSGWND_RANKING_H	(5)

#define	MSGWND_TITLE_X	(6)
#define	MSGWND_TITLE_Y	(3)
#define	MSGWND_TITLE_W	(20)
#define	MSGWND_TITLE_H	(5)


//-------------------------------------
///	�J�E���g
//=====================================
#define TOCH_COUNTER_MAX	(30*5)
#define RESULT_SEND_CNT	(COMPATIBLE_IRC_SENDATA_CNT)

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
///	�w�i����
//=====================================
#define BGMOVE_MOVE_SYNC        (120)
#define BGMOVE_MOVE_START_Y     (0)
#define BGMOVE_MOVE_END_Y       (11*8)
#define BGMOVE_MOVE_VELOCITY_Y  (FX32_CONST(0))

//-------------------------------------
///	CLWK�擾
//=====================================
typedef enum
{	
  CLWKID_BUTTERFLY_TOP,
  CLWKID_BUTTERFLY_END  = CLWKID_BUTTERFLY_TOP + IRC_COMPATIBLE_SV_RANKING_MAX,
  CLWKID_BUTTERFLY_SP,
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
	GFL_ARCUTIL_TRANSINFO	frame_char2;	//��p
	GFL_TCB						*p_vblank_task;
} GRAPHIC_BG_WORK;
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
#if 0
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
#endif
//-------------------------------------
///	�{�^��
//=====================================
#define BUTTON_MAX	(4)
typedef struct
{	
	u16				strID;
	u16				x;			//�J�n�_X
	u16				y;			//�J�n�_Y
	u16				w;			//�J�n�_����̕�
	u16				h;
	u16				plt;
} BUTTON_SETUP;
typedef struct 
{
	GFL_BUTTON_MAN			*p_btn;
	GFL_BMPWIN					*p_bmpwin[BUTTON_MAX];
	GFL_UI_TP_HITTBL		hit_tbl[BUTTON_MAX+1];//+1�͏I���R�[�h��
	const	 BUTTON_SETUP *cp_btn_setup_tbl;
	u16			btn_num;
	u16			frm;
	u16			select_btn_id;
	u16			is_touch;
} BUTTON_WORK;
//-------------------------------------
///	�����x
//=====================================
typedef struct {
	int now_val;		//���݂̒l
	int start_val;		//�J�n�̒l
	int end_val;		//�I���̒l
	fx32 add_val;		//���Z�l(�덷�������Ȃ��悤�ɂ�������fx)
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_VELOCITY_WORK;
//-------------------------------------
///	�R���Ȑ�
//=====================================
typedef struct {
	VecFx32 now_val;	//���݂̍��W
	VecFx32	start_pos;	//�J�n���W
	VecFx32 ctrl_pos0;	//����_�O
	VecFx32 ctrl_pos1;	//����_�P
	VecFx32 end_pos;	//�I�����W
	int sync_now;		//���݂̃V���N
	int sync_max;		//�V���N�ő吔
} PROGVAL_CATMULLROM_WORK;
//-------------------------------------
///	�w�ʓ���
//=====================================
typedef struct 
{
  PROGVAL_VELOCITY_WORK  vel;
  BOOL is_start;
} BGMOVE_WORK;

//-------------------------------------
///	������
//=====================================
typedef struct 
{
  GFL_CLWK  *p_clwk;  //�`��
  VecFx32 pre_pos;    //3D���W
  VecFx32 shake_pos;  //�h���N��

  VecFx32 pre_rot_pos;
  fx32    angle_max;  //�p�x�ő�
  fx32    move_sync;  //�i�ޑ���
  fx32    angle_speed;//�p���x
  s8      angle_dir;
  u8      dummy;

  u16     target_x;   //�ړI���WX
  u16     target_y;   //�ړI���WY

  PROGVAL_CATMULLROM_WORK cutmullrom;
} BUTTERFLY_WORK;
typedef struct
{ 
  BUTTERFLY_WORK  wk[IRC_COMPATIBLE_SV_RANKING_MAX];
  u32 cnt;
  u16 idx;
  u16 idx_max;
} BUTTERFLY_SYS;

//-------------------------------------
///	�����f�f���j���[���C�����[�N
//=====================================
typedef struct _IRC_MENU_MAIN_WORK IRC_MENU_MAIN_WORK;
typedef void (*SEQ_FUNCTION)( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
struct _IRC_MENU_MAIN_WORK
{
	//���W���[��
	GRAPHIC_WORK		grp;
	GRAPHIC_BG_WORK	bg;
	MSG_WORK				msg;
	MSGWND_WORK			msgwnd;
	BUTTON_WORK			btn;
	//BACKOBJ_WORK		backobj;
  BGMOVE_WORK     bgmove;
  BUTTERFLY_SYS   butterfly;
	
	//�����
	APPBAR_WORK			*p_appbar;

	MSGWND_WORK			msgtitle;	//�^�C�g�����b�Z�[�W
	

	//�V�[�P���X�Ǘ�
	SEQ_FUNCTION		seq_function;
	u16		seq;
	u16		cnt;
	BOOL	is_end;

	//�p�����[�^
	IRC_MENU_PARAM	*p_param;

	//���̑�
	u32		start_ms;
	u32		select;
	u32		now_ms;
	BOOL	is_send;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//proc
static GFL_PROC_RESULT IRC_MENU_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_MENU_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_MENU_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
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
static void Graphic_BG_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//obj
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT * GRAPHIC_OBJ_GetClunit( const GRAPHIC_OBJ_WORK *cp_wk );
//MSG_WORK
static void MSG_Init( MSG_WORK *p_wk, HEAPID heapID );
static void MSG_Exit( MSG_WORK *p_wk );
static BOOL MSG_Main( MSG_WORK *p_wk );
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk );
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk );
static const GFL_MSGDATA * MSG_GetMsgDataConst( const MSG_WORK *cp_wk );
static WORDSET * MSG_GetWordSet( const MSG_WORK *cp_wk );
//MSG_WINDOW
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, u8 plt, HEAPID heapID );
static void MSGWND_InitEx( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, u8 plt, u16 clear_chr, u8 dir, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, MSG_WORK *p_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y );
static void MSGWND_PrintCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID );
static void MSGWND_PrintColor( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y, PRINTSYS_LSB color );
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y );
static GFL_BMPWIN * MSGWND_GetBmpWin( const MSGWND_WORK *cp_wk );
//SEQ
static void SEQ_Change( IRC_MENU_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function );
static void SEQ_End( IRC_MENU_MAIN_WORK *p_wk );
//SEQ_FUNC
static void SEQFUNC_Connect( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_Select( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_NextProc( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_DisConnect( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_End( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_SceneError( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
//BTN
static void BUTTON_Init( BUTTON_WORK *p_wk, u8 frm, const	 BUTTON_SETUP *cp_btn_setup_tbl, u8 tbl_max, const MSG_WORK *cp_msg, HEAPID heapID );
static void BUTTON_Exit( BUTTON_WORK *p_wk );
static void BUTTON_Main( BUTTON_WORK *p_wk );
static BOOL BUTTON_IsTouch( const BUTTON_WORK *cp_wk, u32 *p_btnID );
static void Button_TouchCallBack( u32 btnID, u32 event, void *p_param );
//BACKOBJ
#if 0
static void BACKOBJ_Init( BACKOBJ_WORK *p_wk, const GRAPHIC_WORK *cp_grp, BACKOBJ_MOVE_TYPE type, BACKOBJ_COLOR color, u32 clwk_ofs, int sf_type );
static void BACKOBJ_Exit( BACKOBJ_WORK *p_wk );
static void BACKOBJ_Main( BACKOBJ_WORK *p_wk );
static void BACKOBJ_StartEmit( BACKOBJ_WORK *p_wk, const GFL_POINT *cp_pos );
//BACKOBJ_ONE
static void BACKOBJ_ONE_Init( BACKOBJ_ONE *p_wk, GFL_CLWK *p_clwk, BACKOBJ_COLOR color, int sf_type );
static void BACKOBJ_ONE_Main( BACKOBJ_ONE *p_wk );
static void BACKOBJ_ONE_Start( BACKOBJ_ONE *p_wk, const GFL_POINT *cp_start, const GFL_POINT *cp_end, u32 sync );
static BOOL BACKOBJ_ONE_IsMove( const BACKOBJ_ONE *cp_wk );
#endif
//������
static void BUTTERFLY_Init( BUTTERFLY_WORK *p_wk, GFL_CLWK *p_clwk, u32 move_sync, fx32 angle_speed, fx32 angle_max, BOOL is_blue, HEAPID heapID );
static void BUTTERFLY_Exit( BUTTERFLY_WORK *p_wk );
static void BUTTERFLY_Main( BUTTERFLY_WORK *p_wk );
static void Butterfly_SetTarget( BUTTERFLY_WORK *p_wk );
static BOOL Butterfly_IsArriveTarget( const BUTTERFLY_WORK *cp_wk );

static void BUTTERFLY_SYS_Init( BUTTERFLY_SYS *p_wk, const GRAPHIC_WORK *cp_grp, const IRC_COMPATIBLE_SAVEDATA *cp_sv, HEAPID heapID );
static void BUTTERFLY_SYS_Exit( BUTTERFLY_SYS *p_wk );
static void BUTTERFLY_SYS_Main( BUTTERFLY_SYS *p_wk );
//BGMOVE
static void BGMOVE_Init( BGMOVE_WORK *p_wk, HEAPID heapID );
static void BGMOVE_Exit( BGMOVE_WORK *p_wk );
static void BGMOVE_Main( BGMOVE_WORK *p_wk );
static void BGMOVE_Start( BGMOVE_WORK *p_wk );
//�ėp
static void MainModules( IRC_MENU_MAIN_WORK *p_wk );
static BOOL TP_GetRectTrg( const BUTTON_SETUP *cp_btn );

//�����x
static void PROGVAL_VEL_Init( PROGVAL_VELOCITY_WORK* p_wk, int start, int end, int sync );
static BOOL	PROGVAL_VEL_Main( PROGVAL_VELOCITY_WORK* p_wk );
//�R���Ȑ�
static void PROGVAL_CATMULLROM_Init( PROGVAL_CATMULLROM_WORK* p_wk, const VecFx32 *cp_start_pos, const VecFx32 *cp_ctrl_pos0, const VecFx32 *cp_ctrl_pos1, const VecFx32 *cp_end_pos, int sync );
static BOOL PROGVAL_CATMULLROM_Main( PROGVAL_CATMULLROM_WORK* p_wk );

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�ԊO���~�j�Q�[�����j���[�p�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA IrcMenu_ProcData	= 
{	
	IRC_MENU_PROC_Init,
	IRC_MENU_PROC_Main,
	IRC_MENU_PROC_Exit,
};
//-------------------------------------
///	BG�V�X�e��
//=====================================
typedef enum 
{
	GRAPHIC_BG_FRAME_M_INFOWIN,
	GRAPHIC_BG_FRAME_M_TEXT,
	GRAPHIC_BG_FRAME_M_BTN,
	GRAPHIC_BG_FRAME_M_BACK,
	GRAPHIC_BG_FRAME_S_ROGO,
	GRAPHIC_BG_FRAME_S_TEXT,
	GRAPHIC_BG_FRAME_S_BACK,
	GRAPHIC_BG_FRAME_S_TITLE,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	GFL_BG_FRAME0_M, GFL_BG_FRAME1_M, GFL_BG_FRAME2_M, GFL_BG_FRAME3_M, GFL_BG_FRAME0_S, GFL_BG_FRAME1_S, GFL_BG_FRAME2_S, GFL_BG_FRAME3_S,
};
static const u32 sc_bgcnt_mode[ GRAPHIC_BG_FRAME_MAX ] = 
{
	GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_AFFINE,
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
	// GRAPHIC_BG_FRAME_M_TEXT
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_M_BTN
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
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
		0, 0, 0x1000, 0,
		GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_TEXT
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_TITLE
	{
		0, 0, 0x1000, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
		GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ256_128x128,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},

};

//-------------------------------------
///	�{�^���͈�
//=====================================
enum{	
	BTNID_COMATIBLE,
	BTNID_RANKING,

	BTNID_MAX,
	BTNID_NULL	= BTNID_MAX,
};
static const BUTTON_SETUP	sc_btn_setp_tbl[BTNID_MAX]	=
{	
	//list�I��
	{	
		COMPATI_LIST_000,
		MSGWND_GAME_X,
		MSGWND_GAME_Y,
		MSGWND_GAME_W,
		MSGWND_GAME_H,
		IRC_MENU_BG_PAL_M_11,
	},
	//�����L���O�I��
	{	
		COMPATI_LIST_001,
		MSGWND_RANKING_X,
		MSGWND_RANKING_Y,
		MSGWND_RANKING_W,
		MSGWND_RANKING_H,
		IRC_MENU_BG_PAL_M_04,
	},
};

//=============================================================================
/**
 *			PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO���~�j�Q�[�����j���[	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_MENU_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_MENU_MAIN_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCCOMPATIBLE,  0x40000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(IRC_MENU_MAIN_WORK), HEAPID_IRCCOMPATIBLE );
	GFL_STD_MemClear( p_wk, sizeof(IRC_MENU_MAIN_WORK) );
	p_wk->p_param	= p_param;

	//���W���[��������
	MSG_Init( &p_wk->msg, HEAPID_IRCCOMPATIBLE );
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCCOMPATIBLE );
	GRAPHIC_BG_Init( &p_wk->bg, HEAPID_IRCCOMPATIBLE );
	{
		GAME_COMM_SYS_PTR comm	= NULL;
		if( p_wk->p_param->p_gamesys )
		{	
			comm	= GAMESYSTEM_GetGameCommSysPtr(p_wk->p_param->p_gamesys);
		}
		INFOWIN_Init( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_INFOWIN], IRC_MENU_BG_PAL_M_05, comm, HEAPID_IRCCOMPATIBLE );
	}
	
	BUTTON_Init( &p_wk->btn, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BTN],
			sc_btn_setp_tbl, NELEMS(sc_btn_setp_tbl), &p_wk->msg, HEAPID_IRCCOMPATIBLE );

	MSGWND_Init( &p_wk->msgwnd, sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TEXT],
			MSGWND_MSG_X, MSGWND_MSG_Y, MSGWND_MSG_W, MSGWND_MSG_H, IRC_MENU_BG_PAL_S_08, HEAPID_IRCCOMPATIBLE );
	BmpWinFrame_Write( p_wk->msgwnd.p_bmpwin, WINDOW_TRANS_ON, 
					GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->bg.frame_char2), IRC_MENU_BG_PAL_S_06 );

	MSGWND_InitEx( &p_wk->msgtitle, sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE],
			MSGWND_TITLE_X, MSGWND_TITLE_Y, MSGWND_TITLE_W, MSGWND_TITLE_H, IRC_MENU_BG_PAL_S_08, 1, GFL_BMP_CHRAREA_GET_B, HEAPID_IRCCOMPATIBLE );
	MSGWND_PrintCenter( &p_wk->msgtitle, &p_wk->msg, COMPATI_TITLE_000 );
	
//	NAGI_Printf( "text chr num = %d\n", GFL_BMPWIN_GetChrNum( p_wk->msgwnd.p_bmpwin ) );
//	NAGI_Printf( "title chr num = %d\n", GFL_BMPWIN_GetChrNum( p_wk->msgtitle.p_bmpwin ) );
	GFL_BG_SetScaleReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_SCALE_X_SET, TITLE_STR_SCALE_X );
	GFL_BG_SetScaleReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_SCALE_Y_SET, TITLE_STR_SCALE_Y );
	GFL_BG_SetRotateCenterReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_CENTER_X_SET, (MSGWND_TITLE_X + MSGWND_TITLE_W/2)*8 );
	GFL_BG_SetRotateCenterReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_CENTER_Y_SET, (MSGWND_TITLE_Y + MSGWND_TITLE_H/2)*8 );


	//BACKOBJ_Init( &p_wk->backobj, &p_wk->grp, BACKOBJ_MOVE_TYPE_RAIN, BACKOBJ_COLOR_RED, CLWKID_BACKOBJ_TOP, CLSYS_DEFREND_SUB );
  //
  BGMOVE_Init( &p_wk->bgmove, HEAPID_IRCCOMPATIBLE );
  { 
    GAMEDATA *p_data  = NULL;
    SAVE_CONTROL_WORK *p_sv_ctrl  = NULL;
    IRC_COMPATIBLE_SAVEDATA *p_sv = NULL;
    if( p_wk->p_param->p_gamesys )
    { 
      p_data  = GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys );
      p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_data );
      p_sv = IRC_COMPATIBLE_SV_GetSavedata( p_sv_ctrl );
    }
    BUTTERFLY_SYS_Init( &p_wk->butterfly, &p_wk->grp, p_sv, HEAPID_IRCCOMPATIBLE );
  }

	{	
		GFL_CLUNIT	*p_unit	= GRAPHIC_GetClunit( &p_wk->grp );
		p_wk->p_appbar	= APPBAR_Init( APPBAR_OPTION_MASK_RETURN, p_unit, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_INFOWIN], IRC_MENU_BG_PAL_M_14, IRC_MENU_OBJ_PAL_M_00, APP_COMMON_MAPPING_128K, MSG_GetFont(&p_wk->msg ), MSG_GetPrintQue(&p_wk->msg ), HEAPID_IRCCOMPATIBLE );
	}

	switch( p_wk->p_param->mode )
	{	
	case IRCMENU_MODE_INIT:
		SEQ_Change( p_wk, SEQFUNC_Select );
		break;

	case IRCMENU_MODE_RETURN:
		SEQ_Change( p_wk, SEQFUNC_DisConnect );
		break;

	case IRCMENU_MODE_RANKING_RETURN:
		SEQ_Change( p_wk, SEQFUNC_Select );
		break;

	default:
		GF_ASSERT( 0 );
	}
		
	//���j���[�V�[���Z�b�g
	COMPATIBLE_IRC_SetScene( p_wk->p_param->p_irc, COMPATIBLE_SCENE_MENU );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO���~�j�Q�[�����j���[	���C���v���Z�X�j������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_MENU_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_MENU_MAIN_WORK	*p_wk;

	p_wk	= p_work;

	//APPBAR
	APPBAR_Exit( p_wk->p_appbar );	

	//���W���[���j��
  BUTTERFLY_SYS_Exit( &p_wk->butterfly );
  BGMOVE_Exit( &p_wk->bgmove );
	//BACKOBJ_Exit( &p_wk->backobj );
	BUTTON_Exit( &p_wk->btn );

	MSGWND_Exit( &p_wk->msgtitle );
	MSGWND_Exit( &p_wk->msgwnd );
	INFOWIN_Exit();
	GRAPHIC_BG_Exit( &p_wk->bg );
	GRAPHIC_Exit( &p_wk->grp );
	MSG_Exit( &p_wk->msg );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_IRCCOMPATIBLE );
	
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO���~�j�Q�[�����j���[	���C���v���Z�X���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_MENU_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
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

	IRC_MENU_MAIN_WORK	*p_wk;
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
      GFL_CLWK *p_clwk  = GRAPHIC_GetClwk( &p_wk->grp, CLWKID_BUTTERFLY_SP );
      GFL_CLACT_WK_SetAutoAnmFlag( p_clwk, TRUE );
      BGMOVE_Start( &p_wk->bgmove );
			*p_seq	= SEQ_MAIN;
		}
		break;

	case SEQ_MAIN:
    
    BUTTERFLY_SYS_Main( &p_wk->butterfly );
    BGMOVE_Main( &p_wk->bgmove );
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
		GF_ASSERT_MSG( 0, "IRC_MENU_PROC_Main��SEQ�G���[ %d", *p_seq );
	}

	MainModules( p_wk );
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
		GX_VRAM_SUB_OBJ_128_D,       // �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,						// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,				// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,		
		GX_OBJVRAMMODE_CHAR_1D_128K,		
	};	

	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );

	// �f�B�X�v���CON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	// VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &sc_vramSetTable );

	//���W���[��
	GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );

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
 *	@param	id	CLWK��ID
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
			GFL_BG_SetBGControl( sc_bgcnt_frame[i], &sc_bgcnt_data[i], sc_bgcnt_mode[i] );
			GFL_BG_ClearFrame( sc_bgcnt_frame[i] );
			GFL_BG_SetVisible( sc_bgcnt_frame[i], VISIBLE_ON );
		}
	}

	//�ǂݍ��ݐݒ�
	{	
		ARCHANDLE	*p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCCOMPATIBLE, heapID );

		//�p���b�g
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_irccompatible_gra_aura_bg_NCLR,
				PALTYPE_MAIN_BG, IRC_MENU_BG_PAL_M_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_irccompatible_gra_aura_bg_NCLR,
				PALTYPE_SUB_BG, IRC_MENU_BG_PAL_S_00*0x20, 0, heapID );

		//�L����
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_aura_bg_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_aura_bg_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_title_rogo_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_ROGO], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_shita_frame_01_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BTN], 0, 0, FALSE, heapID );
	
		//�X�N���[��
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_aura_bg_block_b_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_aura_bg_block_b_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_title_aishou_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_ROGO], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_shita_frame_01_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BTN], 0, 0, FALSE, heapID );

		//test
/*		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_result_bg_03_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_result_bg_03_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], 0, 0, FALSE, heapID );
*/
		//���N
		GFL_BG_FillCharacter( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], 0, 1, 0 );
		p_wk->frame_char2	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_irccompatible_gra_ue_frame_NCGR, sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], 0, FALSE, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}
	
	//VBlackTask�o�^
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_BG_VBlankTask, p_wk, 0 );
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
	//�^�X�N�j��
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	//���\�[�X�j��
	{	
		GFL_BG_FreeCharacterArea(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT],
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_char2),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->frame_char2));
		GFL_BG_FillCharacterRelease(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT],1,0);
	}

	//BG�ݒ�j��
	{	
		int i;
		// BG�R���g���[���j��
		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{
			GFL_BG_FreeBGControl( sc_bgcnt_frame[i] );
		}
	}

	// BG�V�X�e���j��
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();
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
static void Graphic_BG_VBlankTask( GFL_TCB *p_tcb, void *p_work )
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
				NARC_irccompatible_gra_aura_obj_NCLR, CLSYS_DRAW_SUB, IRC_MENU_OBJ_PAL_S_00*0x20, heapID );

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
		for( i = CLWKID_BUTTERFLY_TOP; i < CLWKID_BUTTERFLY_END; i++ )
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
      GFL_CLACT_WK_SetAffineParam( p_wk->p_clwk[i], CLSYS_AFFINETYPE_DOUBLE );
		}

    cldata.pos_x  = 129;
    cldata.pos_y  = 104;
    cldata.anmseq = 15;
    p_wk->p_clwk[CLWKID_BUTTERFLY_SP]	= GFL_CLACT_WK_Create( p_wk->p_clunit, 
        p_wk->reg_id[OBJREGID_TOUCH_CHR],
        p_wk->reg_id[OBJREGID_TOUCH_PLT],
        p_wk->reg_id[OBJREGID_TOUCH_CEL],
        &cldata,
        CLSYS_DEFREND_SUB,
					heapID
        );
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CLWKID_BUTTERFLY_SP], TRUE );
    GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[CLWKID_BUTTERFLY_SP], 2 );
    GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[CLWKID_BUTTERFLY_SP], FALSE );
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
 *					MSG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W��ݒ�
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *	@param	heapID					�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSG_Init( MSG_WORK *p_wk, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSG_WORK) );

	GFL_FONTSYS_Init();

	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
    NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

	p_wk->p_print_que = PRINTSYS_QUE_Create( heapID );

	p_wk->p_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_compatible_dat, heapID );

	p_wk->p_wordset	= WORDSET_Create( heapID );

	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
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
 *	@param	plt								�p���b�g�ԍ�
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, u8 plt, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
	p_wk->clear_chr	= 0xf;
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
 *	@brief	���b�Z�[�W�\���ʂɕ�����\��
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

	//�L���[���N���A
	PRINTSYS_QUE_Clear( p_que );

	//�\��
	PRINT_UTIL_PrintColor( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font,PRINTSYS_LSB_Make(0xf,0xe,1) );
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
 *	@param	color							�F
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintColor( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y, PRINTSYS_LSB color )
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
	PRINT_UTIL_PrintColor( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font, color );
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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );	

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
 *	@brief	BMPWIN���擾
 *
 *	@param	const MSGWND_WORK *cp_wk	���[�N
 *
 *	@return	BMPWIN
 */
//-----------------------------------------------------------------------------
static GFL_BMPWIN * MSGWND_GetBmpWin( const MSGWND_WORK *cp_wk )
{	
	return cp_wk->p_bmpwin;
}

//=============================================================================
/**
 *				SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ�`�F���W
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	SEQ_FUNCTION										SEQ�֐�
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Change( IRC_MENU_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�I��
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( IRC_MENU_MAIN_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}
//=============================================================================
/**
 *				SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ڑ��V�[�P���X
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Connect( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_BOOT,
		SEQ_MSG_STARTNET,
		SEQ_CONNECT,
		SEQ_MSG_PRINT,
		SEQ_SEND_STATUS,
		SEQ_TIMING_START,
		SEQ_CHANGE_SELECT,
		SEQ_SENDMENU,
		SEQ_RECVMENU,
		SEQ_SENDRETURNMENU,
		SEQ_RECVRETURNMENU,
		SEQ_SCENE,
		SEQ_TIMING_END,
		SEQ_MSG_CONNECT,
		SEQ_NEXTPROC,
	};

	switch( *p_seq )
	{	
	case SEQ_BOOT:
		COMPATIBLE_IRC_Cancel( p_wk->p_param->p_irc );
		*p_seq	= SEQ_MSG_STARTNET;
		break;

	case SEQ_MSG_STARTNET:
		if(COMPATIBLE_IRC_InitWait( p_wk->p_param->p_irc ) )
		{	
			PMSND_PlaySE( IRCCOMMON_SE_IRC );
			*p_seq	= SEQ_CONNECT;
		}
		break;

	case SEQ_CONNECT:
		if( COMPATIBLE_IRC_ConnextWait( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_MSG_PRINT;
		}

		if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_RETURN )
		{
			PMSND_PlaySystemSE( MENU_SE_CANCEL );
			COMPATIBLE_IRC_Cancel( p_wk->p_param->p_irc );
			SEQ_Change( p_wk, SEQFUNC_End );
		}
		break;

	case SEQ_MSG_PRINT:
		MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, COMPATI_STR_004, 0, 0  );
		*p_seq	= SEQ_SEND_STATUS;
		break;

	case SEQ_SEND_STATUS:
		if(COMPATIBLE_MENU_SendStatusData( p_wk->p_param->p_irc, p_wk->p_param->p_gamesys ) )
		{	
			COMPATIBLE_MENU_GetStatusData( p_wk->p_param->p_irc, p_wk->p_param->p_you_status  );
			*p_seq	= SEQ_TIMING_START;
		}

		if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_RETURN )
		{
			PMSND_PlaySystemSE( MENU_SE_CANCEL );
			COMPATIBLE_IRC_Cancel( p_wk->p_param->p_irc );
			SEQ_Change( p_wk, SEQFUNC_End );
		}
		break;

	case SEQ_TIMING_START:
		if( COMPATIBLE_IRC_TimingSyncWait( p_wk->p_param->p_irc, COMPATIBLE_TIMING_NO_MENU_START ) )
		{	
			*p_seq	= SEQ_SENDMENU;
		}
		break;
		
	case SEQ_SENDMENU:
		p_wk->is_send	= TRUE;
		if( COMPATIBLE_MENU_SendMenuData( p_wk->p_param->p_irc, p_wk->now_ms, p_wk->select ) )
		{	
			*p_seq	= SEQ_RECVMENU;
		}
		break;

	case SEQ_RECVMENU:
		if( COMPATIBLE_MENU_RecvMenuData( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_SENDRETURNMENU;
		}
		break;

	case SEQ_SENDRETURNMENU:
		if( COMPATIBLE_MENU_SendReturnMenu( p_wk->p_param->p_irc ) )
		{
			*p_seq	= SEQ_RECVRETURNMENU;
		}
		break;

	case SEQ_RECVRETURNMENU:
		if( COMPATIBLE_MENU_RecvReturnMenu( p_wk->p_param->p_irc ) )
		{
			*p_seq	= SEQ_SCENE;
		}
		break;


		
	case SEQ_SCENE:
		COMPATIBLE_IRC_SetScene( p_wk->p_param->p_irc, COMPATIBLE_SCENE_RHYTHM );
		if( COMPATIBLE_IRC_SendScene( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_TIMING_END;
		}
		break;

	case SEQ_TIMING_END:
		if( COMPATIBLE_IRC_TimingSyncWait( p_wk->p_param->p_irc, COMPATIBLE_TIMING_NO_MENU_END ) )
		{	
			*p_seq	= SEQ_MSG_CONNECT;
		}
		break;

	case SEQ_MSG_CONNECT:
		if( p_wk->cnt >= RESULT_SEND_CNT )
		{	
			PMSND_PlaySE( IRCMENU_SE_IRC_ON );
			MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, COMPATI_STR_002, 0, 0  );
			*p_seq	= SEQ_NEXTPROC;
		}
		break;

	case SEQ_NEXTPROC:
		p_wk->p_param->select	= IRCMENU_SELECT_COMPATIBLE;
		SEQ_Change( p_wk, SEQFUNC_NextProc );
		return;	//���̃A�T�[�g��ʉ߂��Ȃ�����
	};

	//�ԊO���J�n�҂�
	if( SEQ_MSG_PRINT <= *p_seq && *p_seq <=  SEQ_MSG_CONNECT )
	{	
		if( p_wk->cnt <= RESULT_SEND_CNT )
		{	
			p_wk->cnt++;
		}
	}

	if( *p_seq < SEQ_SCENE && COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) != 0 )
	{	
		SEQ_Change( p_wk, SEQFUNC_SceneError );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I���V�[�P���X
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Select( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_INIT,
		SEQ_SELECT,
		SEQ_MSG,
		SEQ_NEXTPROC,	
	};
	u32 ret;
#if 0
	//�������ȊO��M���āA����PROC�����߂鏈��
	if( *p_seq != SEQ_INIT  )
	{	
		if( COMPATIBLE_MENU_RecvMenuData( p_wk->p_param->p_irc ) )
		{	
			u32 you_ms;
			u32 you_select;
			COMPATIBLE_MENU_GetMenuData( p_wk->p_param->p_irc, &you_ms, &you_select );
			OS_TPrintf( "YOU PROC%d MS%d\n", you_select, you_ms );
			OS_TPrintf( "MY PROC%d MS%d\n", p_wk->select, p_wk->now_ms );
			//�������I��łȂ�������A����ɏ]��
			if( p_wk->select == BTNID_NULL )
			{	
				p_wk->select	= you_select;
			}
			else
			{
				//�������I��ł�����A�b�����������ɂ������������Ȃ�ΐe�@�ɏ]��
				if( p_wk->now_ms < you_ms )
				{	
	
				}
				else if( p_wk->now_ms > you_ms )
				{	
					p_wk->select	= you_select;
				}
				else
				{	
					if( GFL_NET_IsParentMachine() )
					{	
	
					}
					else
					{	
							p_wk->select	= you_select;
					}
				}
			}
			COMPATIBLE_MENU_SendReturnMenu( p_wk->p_param->p_irc );
			if( p_wk->is_send )
			{	
				*p_seq	= SEQ_RECVMENU;
			}
			else
			{	
				*p_seq	= SEQ_NEXTPROC;
			}
		}
	}
#endif
	switch( *p_seq )
	{	
	case SEQ_INIT:
		p_wk->select	= BTNID_NULL;
		p_wk->is_send	= FALSE;
		p_wk->start_ms	= OS_TicksToMilliSeconds32( OS_GetTick() );
		*p_seq	= SEQ_SELECT;
		break;

	case SEQ_SELECT:
		BUTTON_Main( &p_wk->btn );
		if( BUTTON_IsTouch( &p_wk->btn,  &p_wk->select ) )
		{	
			switch( p_wk->select )
			{	
			case BTNID_COMATIBLE:
				PMSND_PlaySE( IRCMENU_SE_DECIDE );
				*p_seq	= SEQ_MSG;
				break;
			case BTNID_RANKING:
				PMSND_PlaySE( IRCMENU_SE_DECIDE );
				p_wk->p_param->select	= IRCMENU_SELECT_RANKING;
				SEQ_Change( p_wk, SEQFUNC_NextProc );
				break;
			};
		}
		p_wk->now_ms	= OS_TicksToMilliSeconds32( OS_GetTick() ) - p_wk->start_ms;
		break;

	case SEQ_MSG:
		MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, COMPATI_STR_003, 0, 0  );
		SEQ_Change( p_wk, SEQFUNC_Connect );
		break;
#if 0
	case SEQ_SENDMENU:
		p_wk->is_send	= TRUE;
		if( COMPATIBLE_MENU_SendMenuData( p_wk->p_param->p_irc, p_wk->now_ms, p_wk->select ) )
		{	
			*p_seq	= SEQ_RECVMENU;
		}
		break;

	case SEQ_RECVMENU:
		if( COMPATIBLE_MENU_RecvReturnMenu( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_NEXTPROC;
		}
		break;

	case SEQ_NEXTPROC:
		SEQ_Change( p_wk, SEQFUNC_NextProCOMPATIBLE_IRC_CompScene(c );
		break;
#endif
	};

	if( (APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_RETURN) && *p_seq >= SEQ_SELECT )
	{
		COMPATIBLE_IRC_Cancel( p_wk->p_param->p_irc );
		PMSND_PlaySystemSE( MENU_SE_CANCEL );
		SEQ_Change( p_wk, SEQFUNC_End );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	����PROC�ɂƂԂ��߁A�I��
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_NextProc( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_INIT:
		*p_seq	= SEQ_END;
		break;

	case SEQ_END:
		SEQ_End( p_wk );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ؒf�V�[�P���X
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DisConnect( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_SCENE,
		SEQ_NET_DISCONNECT,
		SEQ_NET_EXIT,
		SEQ_CHANGE_MENU,
	};

	switch( *p_seq )
	{	
	case SEQ_SCENE:
		COMPATIBLE_IRC_ResetScene( p_wk->p_param->p_irc );
		*p_seq	= SEQ_NET_DISCONNECT;
		break;

	case SEQ_NET_DISCONNECT:
		if( COMPATIBLE_IRC_DisConnextWait( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_NET_EXIT;
		}
		break;

	case SEQ_NET_EXIT:
		if( COMPATIBLE_IRC_ExitWait( p_wk->p_param->p_irc) )
		{	
			*p_seq	= SEQ_CHANGE_MENU;
		}
		break;

	case SEQ_CHANGE_MENU:
		SEQ_Change( p_wk, SEQFUNC_Select );
		break;
	};
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I���V�[�P���X
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_JUNCTION,
		SEQ_NET_DISCONNECT,
		SEQ_NET_EXIT,
		SEQ_END,
	};

	switch( *p_seq )
	{
	case SEQ_JUNCTION:
		if( COMPATIBLE_IRC_IsConnext(p_wk->p_param->p_irc) )
		{	
			*p_seq	= SEQ_NET_DISCONNECT;
		}
		else if( COMPATIBLE_IRC_IsInit(p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_NET_EXIT;
		}
		else
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_NET_DISCONNECT:
		if( COMPATIBLE_IRC_DisConnextWait( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_NET_EXIT;
		}
		break;

	case SEQ_NET_EXIT:
		if( COMPATIBLE_IRC_ExitWait( p_wk->p_param->p_irc) )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		p_wk->p_param->select	= IRCMENU_SELECT_RETURN;
		SEQ_End( p_wk );
		break;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�G���[�V�[�P���X
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_SceneError( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	//��������͂��肦�Ȃ�
	if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) > 0 )
	{	
		GF_ASSERT(0);
	}

	//�ڑ����Ă�����
	if( COMPATIBLE_IRC_IsConnext( p_wk->p_param->p_irc ) )
	{	
		//���֔��
		COMPATIBLE_IRC_SetScene( p_wk->p_param->p_irc, COMPATIBLE_SCENE_RHYTHM );	
		p_wk->p_param->select	= IRCMENU_SELECT_COMPATIBLE;
		SEQ_Change( p_wk, SEQFUNC_NextProc );
	}
	else
	{	
		//�ڑ����Ă��Ȃ��Ȃ�΁A�I���ɖ߂�
		SEQ_Change( p_wk, SEQFUNC_Select );
	}

}
//=============================================================================
/**
 *			adapter
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�؂�ւ����ɏ�����郂�W���[��	���C������
 *
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MainModules( IRC_MENU_MAIN_WORK *p_wk )
{	
	INFOWIN_Update();

	//���b�Z�[�W����
	if( MSG_Main( &p_wk->msg ) )
	{	
		MSGWND_Main( &p_wk->msgwnd, &p_wk->msg );
		MSGWND_Main( &p_wk->msgtitle, &p_wk->msg );
	}
	//�P�����~
	//BACKOBJ_Main( &p_wk->backobj );

	//APPBAR
	APPBAR_Main( p_wk->p_appbar );

	GRAPHIC_Draw( &p_wk->grp );
}

//=============================================================================
/**
 *						BUTTON
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^��������	
 *
 *	@param	BUTTON_WORK *p_wk				���[�N
 *	@param	frm											�{�^����\������t���[��
 *	@param	GFL_RECT *cp_rect_tbl		�{�^���̐ݒ�e�[�u��(���̂������Ă�������)
 *	@param	tbl_max									�e�[�u���̌�
 *	@param	cp_msg									�����Ǘ�
 *	@param	frame_char							�t���[���Ɏg���L����
 *	@param	plt											�p���b�g
 *	@param	HEAPID									�q�[�vID
 */
//-----------------------------------------------------------------------------
static void BUTTON_Init( BUTTON_WORK *p_wk, u8 frm, const	 BUTTON_SETUP *cp_btn_setup_tbl, u8 tbl_max, const MSG_WORK *cp_msg, HEAPID heapID )
{	
	//�G���[
	GF_ASSERT_MSG( tbl_max < BUTTON_MAX, "�{�^�����������ł�BUTTON_MAX�̒�`��ς��Ă�������", tbl_max );

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(BUTTON_WORK) );
	p_wk->cp_btn_setup_tbl	= cp_btn_setup_tbl;
	p_wk->btn_num			= tbl_max;
	p_wk->frm					= frm;

	//HITTBL���쐬�i�L�����P�ʂ��h�b�g�P�ʂɁj
	{	
		int i;
		const	 BUTTON_SETUP *cp_setup;
		for( i = 0; i < p_wk->btn_num; i++ )
		{	
			cp_setup	= &cp_btn_setup_tbl[i];
			p_wk->hit_tbl[i].rect.top			= (cp_setup->y)*8+1;
			p_wk->hit_tbl[i].rect.bottom	= (cp_setup->y + cp_setup->h)*8-1;
			p_wk->hit_tbl[i].rect.left		= (cp_setup->x)*8+1;
			p_wk->hit_tbl[i].rect.right		= (cp_setup->x + cp_setup->w)*8-1;
		}
		p_wk->hit_tbl[i].rect.top	= GFL_UI_TP_HIT_END;
	}

	//�{�^���}�l�[�W���쐬
	p_wk->p_btn	= GFL_BMN_Create( p_wk->hit_tbl, Button_TouchCallBack, p_wk, heapID );

	//BMPWIN�쐬�����e����������
	{	
		int i;
		const	 BUTTON_SETUP *cp_setup;
		STRBUF *p_strbuf;
		for( i = 0; i < p_wk->btn_num; i++ )
		{	
			//BMPWIN�쐬
			cp_setup	= &cp_btn_setup_tbl[i];
			p_wk->p_bmpwin[i]	= GFL_BMPWIN_Create( frm, cp_setup->x+4,
					cp_setup->y+1, cp_setup->w-8, cp_setup->h-2, cp_setup->plt, GFL_BMP_CHRAREA_GET_B );

			//BMPWIN�]��
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0x4 );
			GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );

			//�����쐬�`��
			p_strbuf	= GFL_MSG_CreateString( MSG_GetMsgDataConst(cp_msg), cp_setup->strID );
			GFL_FONTSYS_SetColor( 0xf, 0xe, 4 );

			//�����ɔz�u
			{	
				u32	w;
				u32 h;
				w	= PRINTSYS_GetStrWidth( p_strbuf, MSG_GetFont(cp_msg), 0 );
				h	= PRINTSYS_GetStrHeight( p_strbuf, MSG_GetFont(cp_msg) );
				PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 
					(cp_setup->w-8)*4-w/2, (cp_setup->h-2)*4-h/2, p_strbuf, MSG_GetFont(cp_msg) );
			}
			GFL_STR_DeleteBuffer( p_strbuf );

			GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[i] );

		}
		GFL_FONTSYS_SetDefaultColor();
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���j��
 *
 *	@param	BUTTON_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void BUTTON_Exit( BUTTON_WORK *p_wk )
{	
	//BMPWIN�j��
	{	
		int i;
		for( i = 0; i < p_wk->btn_num; i++ )
		{	
			GFL_BMPWIN_Delete(p_wk->p_bmpwin[i]);
		}
	}

	//BMN�j��
	GFL_BMN_Delete( p_wk->p_btn );

	GFL_STD_MemClear( p_wk, sizeof(BUTTON_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����C������
 *
 *	@param	BUTTON_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void BUTTON_Main( BUTTON_WORK *p_wk )
{
	p_wk->is_touch	= GFL_BMN_Main( p_wk->p_btn );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����^�b�`���ꂽ���ǂ���
 *
 *	@param	const BUTTON_WORK *cp_wk	���[�N
 *	@param	*p_btnID									�^�b�`���ꂽ�{�^��ID�󂯎��
 *
 *	@retval	TRUE�Ȃ�Ή����ꂽ
 *	@retval	FALSE�Ȃ�Ή�����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BUTTON_IsTouch( const BUTTON_WORK *cp_wk, u32 *p_btnID )
{	
	if( cp_wk->is_touch )
	{	
		int i;
		if( p_btnID )
		{	
			*p_btnID	= cp_wk->select_btn_id;
		}

		for( i = 0; i < cp_wk->btn_num; i++ )
		{	
			u8 active_plt;
			u8 frm;
			u8 x, y, w, h;
			GFL_BMPWIN	*p_bmpwin	= cp_wk->p_bmpwin[i];
			frm	= GFL_BMPWIN_GetFrame(p_bmpwin);
			x		= cp_wk->cp_btn_setup_tbl[i].x;
			y		= cp_wk->cp_btn_setup_tbl[i].y;
			w		= cp_wk->cp_btn_setup_tbl[i].w;
			h		= cp_wk->cp_btn_setup_tbl[i].h;
			switch( i )
			{
			case BTNID_COMATIBLE:
				active_plt		= IRC_MENU_BG_PAL_M_12;
				break;
			case BTNID_RANKING:
				active_plt		= IRC_MENU_BG_PAL_M_09;
				break;
			}

			if( i == cp_wk->select_btn_id )
			{	
				//�I�΂ꂽ�{�^��
				GFL_BG_ChangeScreenPalette( frm, x, y, w, h, active_plt );
				GFL_BG_LoadScreenReq( frm );
			}
			else
			{
				//�I�΂�Ȃ������{�^��
				GFL_BG_LoadScreenReq( frm );
			}
		}


		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���������ꂽ���̃R�[���o�b�N
 *
 *	@param	u32 btnID	�{�^��ID
 *	@param	event			�C�x���g
 *	@param	*p_param	Create�œn���ꂽ�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void Button_TouchCallBack( u32 btnID, u32 event, void *p_param )
{	
	BUTTON_WORK *p_wk;
	p_wk	= p_param;

	if( event == GFL_BMN_EVENT_TOUCH )
	{	
		p_wk->select_btn_id	= btnID;
	}
}
#if 0
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
#endif
//=============================================================================
/**
 *    BGMOVE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  BG���[�N  ������
 *
 *	@param	BGMOVE_WORK *p_wk   ���[�N
 *	@param	heapID              �q�[�vID
 */
//-----------------------------------------------------------------------------
static void BGMOVE_Init( BGMOVE_WORK *p_wk, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(BGMOVE_WORK) );

  GFL_BG_SetScrollReq( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_ROGO ], GFL_BG_SCROLL_Y_SET, BGMOVE_MOVE_END_Y );
  GFL_BG_SetScrollReq( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TITLE ], GFL_BG_SCROLL_Y_SET,BGMOVE_MOVE_END_Y );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BG���[�N  �j��
 *
 *	@param	BGMOVE_WORK *p_wk   ���[�N
 */
//-----------------------------------------------------------------------------
static void BGMOVE_Exit( BGMOVE_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_wk, sizeof(BGMOVE_WORK) );
}
static int test = 0;
//----------------------------------------------------------------------------
/**
 *	@brief  BG���[�N  ���C��
 *
 *	@param	BGMOVE_WORK *p_wk   ���[�N
 */
//-----------------------------------------------------------------------------
static void BGMOVE_Main( BGMOVE_WORK *p_wk )
{ 

  if( p_wk->is_start )
  {
  
    if( PROGVAL_VEL_Main(&p_wk->vel)  )
    { 
      p_wk->is_start = FALSE;
    }

    GFL_BG_SetScrollReq( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_ROGO ], GFL_BG_SCROLL_Y_SET, BGMOVE_MOVE_END_Y - p_wk->vel.now_val );
    GFL_BG_SetScrollReq( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TITLE ], GFL_BG_SCROLL_Y_SET, BGMOVE_MOVE_END_Y - p_wk->vel.now_val );

    test++;
  }

  NAGI_Printf( "bgmove %d\n", test );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BG���[�N  �J�n
 *
 *	@param	BGMOVE_WORK *p_wk   ���[�N
 */
//-----------------------------------------------------------------------------
static void BGMOVE_Start( BGMOVE_WORK *p_wk )
{ 
  test  = 0;
  p_wk->is_start  = TRUE;
  PROGVAL_VEL_Init( &p_wk->vel, BGMOVE_MOVE_START_Y, BGMOVE_MOVE_END_Y, BGMOVE_MOVE_SYNC );
}
//=============================================================================
/**
 *      ������
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ��1�C�̓��� ������
 *
 *	@param	BUTTERFLY_WORK *p_wk  ���[�N
 *	@param	*p_clwk               CLWK
 *	@param	speed                 �i�s���x
 *	@param	angle_speed           �p���x
 *	@param	angle_max             �p�x�ő�
 *	@param	heapID                �q�[�vID
 */
//-----------------------------------------------------------------------------
static void BUTTERFLY_Init( BUTTERFLY_WORK *p_wk, GFL_CLWK *p_clwk, u32 move_sync, fx32 angle_speed, fx32 angle_max, BOOL is_blue, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(BUTTERFLY_WORK) );
  p_wk->p_clwk      = p_clwk;
  p_wk->angle_max   = angle_max;
  p_wk->move_sync   = move_sync;
  p_wk->angle_speed = angle_speed;
  p_wk->angle_dir   = 1;

  //�F�̐ݒ�
  {
    u16 anmseq;
    if( is_blue )
    { 
      anmseq  = 13;
    }
    else
    { 
      anmseq  = 12;
    }
    GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, anmseq );
  }

  //�����͉�ʊO�ɂ���
  { 
    GFL_CLACTPOS  pos;
#if 1
    switch( GFUser_GetPublicRand0(4) )
    { 
    case 0: //��
      pos.x = -16;
      pos.y = GFUser_GetPublicRand0( 192 );
      break;
    case 1: //�E
      pos.x = 256+16;
      pos.y = GFUser_GetPublicRand0( 192 );
      break;
    case 2: //��
      pos.x = GFUser_GetPublicRand0( 256 );
      pos.y = -16;
      break;
    case 3: //��
      pos.x = GFUser_GetPublicRand0( 256 );
      pos.y = 192 + 16;
      break;
    }
#else
    pos.x = 128;
    pos.y = 96;
#endif
    GFL_CLACT_WK_SetPos( p_wk->p_clwk, &pos, CLSYS_DEFREND_SUB );
  }

  //�����̖ړI�n�ݒ�i��L�����l�ݒ��ɍs�����Ɓj
  Butterfly_SetTarget( p_wk );

  { 
    GFL_CLACTPOS  pos;
    GFL_CLACT_WK_GetPos( p_wk->p_clwk, &pos, CLSYS_DEFREND_SUB );
    NAGI_Printf( "����\n" );
    NAGI_Printf( "�@�������W X=%d Y=%d\n", pos.x, pos.y );
    NAGI_Printf( "�@�ڕW���W X=%d Y=%d\n", p_wk->target_x, p_wk->target_y );
    NAGI_Printf( "�@���x     0x%x\n", move_sync );
    NAGI_Printf( "�@�p���x 0x%x\n", p_wk->angle_speed );
    NAGI_Printf( "�@�ő�p 0x%x\n", p_wk->angle_max );
  }

  GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
  GFL_CLACT_WK_SetAnmFrame( p_wk->p_clwk, GFUser_GetPublicRand0(FX32_ONE*2) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���P�C�̓���  �j��
 *
 *	@param	BUTTERFLY_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BUTTERFLY_Exit( BUTTERFLY_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_wk, sizeof(BUTTERFLY_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���P�C�̓���  ���C��
 *
 *	@param	BUTTERFLY_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BUTTERFLY_Main( BUTTERFLY_WORK *p_wk )
{
  VecFx32 shake;

  //�h��鏈��
  { 
    p_wk->shake_pos.x = 0;
    p_wk->shake_pos.z = 0;
    p_wk->shake_pos.y += p_wk->angle_speed * p_wk->angle_dir;
    if( p_wk->shake_pos.y > p_wk->angle_max/2 )
    { 
      p_wk->shake_pos.y = p_wk->angle_max/2;
      p_wk->angle_dir = -1;
    }
    if( p_wk->shake_pos.y < -p_wk->angle_max/2 )
    { 
      p_wk->shake_pos.y = -p_wk->angle_max/2;
      p_wk->angle_dir   = 1;
    }
  }

  //�ړ�
  p_wk->pre_pos = p_wk->cutmullrom.now_val;
  //�ړI�n�܂ŗ�����A�ēx�ړI�n��ݒ�
  if( PROGVAL_CATMULLROM_Main( &p_wk->cutmullrom ) )
  { 
    Butterfly_SetTarget( p_wk );
  }

   //��]
  { 
    VecFx32 vec_move;

    VEC_Subtract( &p_wk->cutmullrom.now_val, &p_wk->pre_pos, &vec_move );
    vec_move.z  = 0;
    VEC_Normalize( &vec_move, &vec_move );

    //���
    { 
      VecFx32 vec0;
      MtxFx33 rot;

      VEC_Set( &vec0, FX32_ONE, 0, 0 );
      GFL_CALC3D_MTX_GetRotVecToVec( &vec0, &vec_move, &rot );
      MTX_MultVec33( &p_wk->shake_pos, &rot, &shake );
    }

    //OBJ�̉�]�̂��߂ɁA�ړ������������p�x�����o��
    { 

      VecFx32 vec0;
      VecFx32 vec_dir;
      u16 obj_angle;

      VEC_Set( &vec0, 0, -FX32_ONE, 0 );

      vec_dir.x = p_wk->cutmullrom.now_val.x + shake.x;
      vec_dir.y = p_wk->cutmullrom.now_val.y + shake.y;
      vec_dir.z = 0;

      //�O��Ɠ������W�ł͂Ȃ��Ƃ�������]

      VEC_Subtract( &vec_dir, &p_wk->pre_rot_pos, &vec_dir );
      VEC_Normalize( &vec_dir, &vec_dir );
      if( vec_dir.x != 0 && vec_dir.y != 0 )
      { 
        obj_angle = FX_AcosIdx( GFL_CALC3D_VEC_GetCos( &vec_dir, &vec0 ) );

        { 
          VecFx32 vec_norm;
          VEC_Set( &vec_norm, 0, 0, FX32_ONE );
          //cos�ł�0�`180�܂ł������߂��Ȃ��̂ō��E���ʂ���
          if( GFL_CALC3D_VEC_CalcRotVectorLR( &vec_norm, &vec0, &vec_dir ) < 0 )
          { 
            obj_angle = DEG_TO_IDX(360) - obj_angle;
          }
        }

        GFL_CLACT_WK_SetRotation( p_wk->p_clwk, obj_angle );
      }
    }
  }


  //�ݒ�
  { 
    GFL_CLACTPOS  pos;

    pos.x = (p_wk->cutmullrom.now_val.x + shake.x) >> FX32_SHIFT;
    pos.y = (p_wk->cutmullrom.now_val.y + shake.y) >> FX32_SHIFT;
    GFL_CLACT_WK_SetPos( p_wk->p_clwk, &pos, CLSYS_DEFREND_SUB );
  }

  { 
    p_wk->pre_rot_pos.x = p_wk->cutmullrom.now_val.x + shake.x;
    p_wk->pre_rot_pos.y = p_wk->cutmullrom.now_val.y + shake.y;
    p_wk->pre_rot_pos.z = 0;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  ���P�C�̓���  �^�[�Q�b�g�ݒ�
 *
 *	@param	BUTTERFLY_WORK *p_wk  ���[�N
 *	@param	x   X���W�ݒ�
 *	@param	y   Y���W�ݒ�
 */
//-----------------------------------------------------------------------------
static void Butterfly_SetTarget( BUTTERFLY_WORK *p_wk )
{ 
  //p_wk->target_x  = x;
  //p_wk->target_y  = y;
  //NAGI_Printf( "Set Target X=%d Y=%d\n", x, y );

  { 
    GFL_CLACTPOS  pos;
    VecFx32 start_pos;
    VecFx32 end_pos;
    VecFx32 ctrl_pos0;
    VecFx32 ctrl_pos1;

    GFL_CLACT_WK_GetPos( p_wk->p_clwk, &pos, CLSYS_DEFREND_SUB );
    VEC_Set( &start_pos, pos.x << FX32_SHIFT, pos.y << FX32_SHIFT, 0 );
    VEC_Set( &end_pos, GFUser_GetPublicRand0( 256 ) << FX32_SHIFT, GFUser_GetPublicRand0( 192 ) << FX32_SHIFT, 0 );
    VEC_Set( &ctrl_pos0, GFUser_GetPublicRand0( 256 ) << FX32_SHIFT, GFUser_GetPublicRand0( 192 ) << FX32_SHIFT, 0 );
    VEC_Set( &ctrl_pos1, GFUser_GetPublicRand0( 256 ) << FX32_SHIFT, GFUser_GetPublicRand0( 192 ) << FX32_SHIFT, 0 );

    PROGVAL_CATMULLROM_Init( &p_wk->cutmullrom, &start_pos, &ctrl_pos0, &ctrl_pos1, &end_pos, p_wk->move_sync );
  }

#if 0
  //�ړI�n�܂ł̊p�x���v�Z
  { 
    VecFx32 dir;

    GFL_CLACTPOS  pos;
    GFL_CLACT_WK_GetPos( p_wk->p_clwk, &pos, CLSYS_DEFREND_SUB );

    VEC_Set( &dir, FX32_CONST(p_wk->target_x - pos.x), FX32_CONST(p_wk->target_y - pos.y), 0 );
    VEC_Normalize( &dir, &dir );
    OS_Printf( "dir X=%f Y=%f\n", FX_FX32_TO_F32(dir.x), FX_FX32_TO_F32(dir.y) );

    { 
      VecFx32 vec0;       //0�x�̃x�N�g��
      VEC_Set( &vec0, FX32_ONE, 0, 0 );
      p_wk->angle_std = FX_AcosIdx( GFL_CALC3D_VEC_GetCos( &dir, &vec0 ) );
      { 
        VecFx32 vec_norm;
        VEC_Set( &vec_norm, 0, 0, FX32_ONE );
        //cos�ł�0�`180�܂ł������߂��Ȃ��̂ō��E���ʂ���
        if( GFL_CALC3D_VEC_CalcRotVectorLR( &vec_norm, &vec0, &dir ) < 0 )
        { 
          p_wk->angle_std = DEG_TO_IDX(360) - p_wk->angle_std;
        }
      }
      NAGI_Printf( "�ڕW�p�x %d\n", 360 * p_wk->angle_std / 0xFFFF  );
    }
  }
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��1�C�̓��� �����`�F�b�N
 *
 *	@param	const BUTTERFLY_WORK *cp_wk ���[�N
 *
 *	@return TRUE�œ���  FALSE�ł܂�
 */
//-----------------------------------------------------------------------------
static BOOL Butterfly_IsArriveTarget( const BUTTERFLY_WORK *cp_wk )
{
	s32 vx, vy, r;
  GFL_CLACTPOS  pos;

  GFL_CLACT_WK_GetPos( cp_wk->p_clwk, &pos, CLSYS_DEFREND_SUB );

	vx	= cp_wk->target_x - pos.x;
	vy	= cp_wk->target_y - pos.y;
	r	= 20;
	
	return vx * vx + vy * vy <= r * r;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �������V�X�e��  ������
 *
 *	@param	BUTTERFLY_SYS *p_wk   ���[�N
 *	@param	GRAPHIC_WORK *cp_grp  �O���t�B�b�N�V�X�e��
 *	@param	heapID  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void BUTTERFLY_SYS_Init( BUTTERFLY_SYS *p_wk, const GRAPHIC_WORK *cp_grp, const IRC_COMPATIBLE_SAVEDATA *cp_sv, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(BUTTERFLY_SYS) );
  if( cp_sv == NULL )
  { 
    p_wk->idx_max = 30;
  }
  else
  { 
    p_wk->idx_max = IRC_COMPATIBLE_SV_GetRankNum( cp_sv );
  }

  //���̐ݒ�
  { 
    GFL_CLWK *p_clwk;
    u32 sync;
    u16 angle_speed;
    u16 angle_max;
    BOOL is_blue;

    int i;
    for( i = 0; i < p_wk->idx_max; i++ )
    { 
      p_clwk      = GRAPHIC_GetClwk( cp_grp, CLWKID_BUTTERFLY_TOP + i );
      sync        = 1400 + GFUser_GetPublicRand0( 200 );
      angle_speed = 0;//FX32_CONST(0.1f); + GFUser_GetPublicRand0( FX32_CONST(0.2f) );
      angle_max   = 0;//FX32_CONST(160.0f); + GFUser_GetPublicRand0( FX32_CONST(40.0f) );
      if( cp_sv == NULL )
      {
        is_blue = GFUser_GetPublicRand0( 2 );
      }
      else
      { 
        is_blue     = IRC_COMPATIBLE_SV_GetSex( cp_sv, i ) == IRC_COMPATIBLE_SEX_MALE;
      }
      BUTTERFLY_Init( &p_wk->wk[i], p_clwk, sync, angle_speed, angle_max, is_blue, heapID );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �������V�X�e��  �j��
 *
 *	@param	BUTTERFLY_SYS *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BUTTERFLY_SYS_Exit( BUTTERFLY_SYS *p_wk )
{ 
  { 
    int i;
    for( i = 0; i < p_wk->idx_max; i++ )
    { 
      BUTTERFLY_Exit( &p_wk->wk[i] );
    }
  }

  GFL_STD_MemClear( p_wk, sizeof(BUTTERFLY_SYS) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �������V�X�e��  ���C������
 *
 *	@param	BUTTERFLY_SYS *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BUTTERFLY_SYS_Main( BUTTERFLY_SYS *p_wk )
{ 
  { 
    int i;
    for( i = 0; i < p_wk->idx; i++ )
    { 
      BUTTERFLY_Main( &p_wk->wk[i] );
    }
  }

  //���X�ɒ��������Ă�������
  if( p_wk->cnt++> 0 )
  { 
    p_wk->cnt = 0;
    if( p_wk->idx < p_wk->idx_max )
    { 
      p_wk->idx++;
    }
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
static BOOL TP_GetRectTrg( const BUTTON_SETUP *cp_btn )
{	
	u32 x, y;

	//Cont���ŁA��`���̂Ƃ�
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		if( ((u32)( x - cp_btn->x*8) < (u32)(cp_btn->w*8))
				&	((u32)( y - cp_btn->y*8) < (u32)(cp_btn->h*8))
			)
		{
			return TRUE;
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���������^��������
 *
 *	@param	PROGVAL_VELOCITY_WORK* p_wk	���[�N
 *	@param	start						�J�n���W
 *	@param	end							�I�����W
 *	@param	sync						������V���N
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void PROGVAL_VEL_Init( PROGVAL_VELOCITY_WORK* p_wk, int start, int end, int sync )
{
	p_wk->now_val	= start;
	p_wk->start_val	= start;
	p_wk->end_val	= end;
	p_wk->sync_max	= sync;
	if( sync ) {
		p_wk->add_val	= FX32_CONST(p_wk->end_val - p_wk->start_val) / sync;
		p_wk->sync_now	= 0;
	}else{
		//	sync == 0 �̏ꍇ�͑������I��
		p_wk->sync_now	= p_wk->sync_max-2;	//(p_wk->sync_max-1)��艺�̒l
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���������^�����C�������i���t���[����Ԃ��Ɓj
 *
 *	@param	PROGVAL_VELOCITY_WORK* p_wk ���[�N
 *
 *	@return	TRUE�Ȃ珈���I���AFALSE�Ȃ珈�����B
 */
//-----------------------------------------------------------------------------
BOOL PROGVAL_VEL_Main( PROGVAL_VELOCITY_WORK* p_wk )
{
	if( p_wk->sync_now < (p_wk->sync_max-1) ) {	//	�Ȃ�-1���Ƃ����ƁAelse�̒����ӂ��߂Ă�sync������
		p_wk->sync_now++;
		p_wk->now_val	= p_wk->start_val + ((p_wk->add_val * (p_wk->sync_now)) >> FX32_SHIFT);
		return FALSE;
	}else{
		p_wk->now_val	= p_wk->end_val;
		return TRUE;
	}
}

//=============================================================================
/**
 *			�R���Ȑ��@CatmullRom�Ȑ���
 */
//=============================================================================
//----------------------------------------------------------------------------
/*
 *	@brief	�R���Ȑ�CatmullRom�Ȑ���	������
 *
 *	@param	p_wk						���[�N
 *	@param	start_pos					�J�n���W
 *	@param	ctrl_pos0					����_�P
 *	@param	ctrl_pos1					����_�Q
 *	@param	end_pos						�I�����W
 *	@param	sync						������V���N
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void PROGVAL_CATMULLROM_Init( PROGVAL_CATMULLROM_WORK* p_wk, const VecFx32 *cp_start_pos, const VecFx32 *cp_ctrl_pos0, const VecFx32 *cp_ctrl_pos1, const VecFx32 *cp_end_pos, int sync )
{
/*	GF_ASSERT( cp_start_pos != NULL
			&& cp_ctrl_pos0 != NULL
			&& cp_ctrl_pos1 != NULL 
			&& cp_end_pos != NULL );
*/
	p_wk->now_val	= *cp_start_pos;
	p_wk->start_pos	= *cp_start_pos;
	p_wk->ctrl_pos0	= *cp_ctrl_pos0;
	p_wk->ctrl_pos1	= *cp_ctrl_pos1;
	p_wk->end_pos	= *cp_end_pos;
	p_wk->sync_now	= 0;
	p_wk->sync_max	= sync;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�R���Ȑ�CatmullRom�Ȑ���	���C��������
 *
 *	@param	p_wk	���[�N
 *
 *	@returnTRUE�Ȃ珈���I���AFALSE�Ȃ珈����
 */
//-----------------------------------------------------------------------------
static BOOL PROGVAL_CATMULLROM_Main( PROGVAL_CATMULLROM_WORK* p_wk )
{
	if( p_wk->sync_now < (p_wk->sync_max-1) ) {	//	�Ȃ�-1���Ƃ����ƁAelse�̒����ӂ��߂Ă�sync������
		int	sync1div3	= (p_wk->sync_max-1) * 1 / 3;
		int sync2div3	= (p_wk->sync_max-1) * 2 / 3;
		fx32 t1, t2, t3;
		fx32 mp0, mp1, mp2, mp3;

		//	�n�_����P0�܂ł̋Ȑ�
		if( p_wk->sync_now < sync1div3 ) {
			t1	= FX_Div( FX32_CONST(p_wk->sync_now), FX32_CONST(sync1div3));
			t2	= FX_Mul( t1, t1 );	//	t*t
			t3	= FX_Mul( t2, t1 );	//	t*t*t

			mp0	= 0;
			mp1	=(   t2 - 3*t1 + 2*FX32_ONE) / 2;
			mp2	=(-2*t2 + 4*t1) / 2;
			mp3	=(   t2 -   t1) / 2;

			p_wk->now_val.x	= FX_Mul(p_wk->start_pos.x, mp1) + FX_Mul(p_wk->ctrl_pos0.x, mp2)
				+ FX_Mul(p_wk->ctrl_pos1.x, mp3);
			p_wk->now_val.y	= FX_Mul(p_wk->start_pos.y, mp1) + FX_Mul(p_wk->ctrl_pos0.y, mp2)
				+ FX_Mul(p_wk->ctrl_pos1.y, mp3);

		//	P0����P1�܂ł̋Ȑ�
		}else if( p_wk->sync_now < sync2div3 ) {
			t1	= FX_Div( FX32_CONST(p_wk->sync_now-sync1div3), FX32_CONST(sync2div3-sync1div3));
			t2	= FX_Mul( t1, t1 );	//	t*t
			t3	= FX_Mul( t2, t1 );	//	t*t*t

			mp0	=(  -t3 + 2*t2 - t1) / 2;
			mp1	=( 3*t3 - 5*t2 + 2*FX32_ONE) / 2;
			mp2	=(-3*t3 + 4*t2 + t1) / 2;
			mp3	=(   t3 -   t2 ) / 2;

			p_wk->now_val.x	= FX_Mul(p_wk->start_pos.x, mp0) + FX_Mul(p_wk->ctrl_pos0.x, mp1)
				+ FX_Mul(p_wk->ctrl_pos1.x, mp2) + FX_Mul(p_wk->end_pos.x,   mp3);
			p_wk->now_val.y	= FX_Mul(p_wk->start_pos.y, mp0) + FX_Mul(p_wk->ctrl_pos0.y, mp1)
				+ FX_Mul(p_wk->ctrl_pos1.y, mp2) + FX_Mul(p_wk->end_pos.y,   mp3);

		//	P1����I�_�܂ł̋Ȑ�
		}else if( sync2div3 <= p_wk->sync_now ) {
			t1	= FX_Div( FX32_CONST(p_wk->sync_now-sync2div3), FX32_CONST(p_wk->sync_max-sync2div3));
			t2	= FX_Mul( t1, t1 );	//	t*t
			t3	= FX_Mul( t2, t1 );	//	t*t*t

			mp0	=(   t2 - t1) /2;
			mp1	=(-2*t2		   + 2*FX32_ONE) / 2;
			mp2	=(   t2 + t1) / 2;
			mp3	=0;	

			p_wk->now_val.x	= FX_Mul(p_wk->ctrl_pos0.x, mp0)
				+ FX_Mul(p_wk->ctrl_pos1.x, mp1) + FX_Mul(p_wk->end_pos.x, mp2);
			p_wk->now_val.y	= FX_Mul(p_wk->ctrl_pos0.y, mp0)
				+ FX_Mul(p_wk->ctrl_pos1.y, mp1) + FX_Mul(p_wk->end_pos.y, mp2);
			p_wk->now_val.z	= FX_Mul(p_wk->ctrl_pos0.z, mp0)
				+ FX_Mul(p_wk->ctrl_pos1.z, mp1) + FX_Mul(p_wk->end_pos.z, mp2);
		}

		p_wk->sync_now++;

		return FALSE;
	}else{
		p_wk->now_val	= p_wk->end_pos;
		return TRUE;
	}

	return TRUE;
}

