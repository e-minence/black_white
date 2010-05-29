//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_rhythm_main.c
 *	@brief	�ԊO���~�j�Q�[��	���Y���`�F�b�N
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
#include "msg/msg_irc_rhythm.h"
#include "irccompatible_gra.naix"

//	rhythm
#include "net_app/irc_rhythm.h"
// proc
#include "net_app/irc_result.h"

//sound
#include "../irc_compatible/irc_compatible_snd.h"

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
//-------------------------------------
///	�f�o�b�O�p��`
//=====================================
#ifdef PM_DEBUG
#endif //PM_DEBUG

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
static BOOL s_is_debug_only_play	= 0;
#endif

//-------------------------------------
///	�_��
//=====================================
//�Ԋu�̎��Ԃɂ�链�_
#define VAL_RHYTHMSCORE_PROC_00	(100)
#define VAL_RHYTHMSCORE_PROC_01	(90)
#define VAL_RHYTHMSCORE_PROC_02	(80)
#define VAL_RHYTHMSCORE_PROC_03	(70)
#define VAL_RHYTHMSCORE_PROC_04	(60)
#define VAL_RHYTHMSCORE_PROC_05	(50)
#define VAL_RHYTHMSCORE_PROC_06	(40)
#define VAL_RHYTHMSCORE_PROC_07	(30)
#define VAL_RHYTHMSCORE_PROC_08	(20)
#define VAL_RHYTHMSCORE_PROC_09	(10)
#define VAL_RHYTHMSCORE_PROC_10	(0)

#define RANGE_RHYTHMSCORE_PROC_00	(0)
#define RANGE_RHYTHMSCORE_PROC_01	(500)
#define RANGE_RHYTHMSCORE_PROC_02	(1000)
#define RANGE_RHYTHMSCORE_PROC_03	(3000)
#define RANGE_RHYTHMSCORE_PROC_04	(5000)
#define RANGE_RHYTHMSCORE_PROC_05	(10000)
#define RANGE_RHYTHMSCORE_PROC_06	(15000)
#define RANGE_RHYTHMSCORE_PROC_07	(20000)
#define RANGE_RHYTHMSCORE_PROC_08	(25000)
#define RANGE_RHYTHMSCORE_PROC_09	(30000)

//�Ԋu�̎��Ԃɂ�链�_
#define VAL_RHYTHMSCORE_DIFF_00	(100)
#define VAL_RHYTHMSCORE_DIFF_01	(90)
#define VAL_RHYTHMSCORE_DIFF_02	(80)
#define VAL_RHYTHMSCORE_DIFF_03	(70)
#define VAL_RHYTHMSCORE_DIFF_04	(60)
#define VAL_RHYTHMSCORE_DIFF_05	(50)
#define VAL_RHYTHMSCORE_DIFF_06	(40)
#define VAL_RHYTHMSCORE_DIFF_07	(30)
#define VAL_RHYTHMSCORE_DIFF_08	(20)
#define VAL_RHYTHMSCORE_DIFF_09	(10)
#define VAL_RHYTHMSCORE_DIFF_10	(0)

#define RANGE_RHYTHMSCORE_DIFF_00	(0)
#define RANGE_RHYTHMSCORE_DIFF_01	(50)
#define RANGE_RHYTHMSCORE_DIFF_02	(100)
#define RANGE_RHYTHMSCORE_DIFF_03	(200)
#define RANGE_RHYTHMSCORE_DIFF_04	(300)
#define RANGE_RHYTHMSCORE_DIFF_05	(400)
#define RANGE_RHYTHMSCORE_DIFF_06	(500)
#define RANGE_RHYTHMSCORE_DIFF_07	(600)
#define RANGE_RHYTHMSCORE_DIFF_08	(1000)
#define RANGE_RHYTHMSCORE_DIFF_09	(15000)


//-------------------------------------
///	�p���b�g
//=====================================
enum{	
	// ���C�����BG
	RHYTHM_BG_PAL_M_00 = 0,//�w�i�pB
	RHYTHM_BG_PAL_M_01,		// 
	RHYTHM_BG_PAL_M_02,		// 
	RHYTHM_BG_PAL_M_03,		// 
	RHYTHM_BG_PAL_M_04,		// 
	RHYTHM_BG_PAL_M_05,		// INFOWIN
	RHYTHM_BG_PAL_M_06,		// �w�i�p�����܂�
	RHYTHM_BG_PAL_M_07,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_08,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_09,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_10,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_11,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_12,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_13,		// FONT
	RHYTHM_BG_PAL_M_14,		// APPBAR
	RHYTHM_BG_PAL_M_15,		// APPBAR


	// �T�u���BG
	RHYTHM_BG_PAL_S_00 = 0,	//�w�i�p
	RHYTHM_BG_PAL_S_01,		//
	RHYTHM_BG_PAL_S_02,		//
	RHYTHM_BG_PAL_S_03,		//
	RHYTHM_BG_PAL_S_04,		//
	RHYTHM_BG_PAL_S_05,		//
	RHYTHM_BG_PAL_S_06,		// �w�i�p			//���ʗp�o�[���t�H���g
	RHYTHM_BG_PAL_S_07,		// �w�i
	RHYTHM_BG_PAL_S_08,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_09,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_10,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_11,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_12,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_13,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_14,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_15,		// �g�p���ĂȂ�

	// ���C�����OBJ
	RHYTHM_OBJ_PAL_M_00 = 0,// OBJ�p
	RHYTHM_OBJ_PAL_M_01,		// 
	RHYTHM_OBJ_PAL_M_02,		// 
	RHYTHM_OBJ_PAL_M_03,		// 
	RHYTHM_OBJ_PAL_M_04,		// 
	RHYTHM_OBJ_PAL_M_05,		// 
	RHYTHM_OBJ_PAL_M_06,		// �����܂Ń^�b�`OBJ
	RHYTHM_OBJ_PAL_M_07,		// �����p
	RHYTHM_OBJ_PAL_M_08,		// 
	RHYTHM_OBJ_PAL_M_09,		// 
	RHYTHM_OBJ_PAL_M_10,		// OBJ�p
	RHYTHM_OBJ_PAL_M_11,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_12,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_13,		// APPBAR
	RHYTHM_OBJ_PAL_M_14,		// APPBAR
	RHYTHM_OBJ_PAL_M_15,		// APPBAR


	// �T�u���OBJ
	RHYTHM_OBJ_PAL_S_00 = 0,	//�g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_01,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_02,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_03,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_04,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_05,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_06,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_07,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_08,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_09,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_10,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_11,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_12,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_13,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_14,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_S_15,		// �g�p���ĂȂ�
};

//-------------------------------------
///	���o�[
//=====================================
#define INFOWIN_PLT_NO		(RHYTHM_BG_PAL_M_05)
#define INFOWIN_BG_FRAME	(GFL_BG_FRAME0_M)

//-------------------------------------
///	����
//=====================================
#define TEXTSTR_PLT_NO				(RHYTHM_BG_PAL_S_14)
#define TEXTSTR_BUFFER_LENGTH	(255)

//-------------------------------------
///	�ʒu
//=====================================
#define	MSGWND_TEXT_X	(1)
#define	MSGWND_TEXT_Y	(18)
#define	MSGWND_TEXT_W	(30)
#define	MSGWND_TEXT_H	(5)

#define	MSGWND_TITLE_X	(6)
#define	MSGWND_TITLE_Y	(3)
#define	MSGWND_TITLE_W	(20)
#define	MSGWND_TITLE_H	(5)

#define	MSGWND_START_X	(8)
#define	MSGWND_START_Y	(10)
#define	MSGWND_START_W	(16)
#define	MSGWND_START_H	(2)

//-------------------------------------
///	�J�E���g
//=====================================
#define TOUCHMARKER_VISIBLE_CNT	(10)
#define RESULT_SEND_CNT	(COMPATIBLE_IRC_SENDATA_CNT)

//-------------------------------------
///	�v�����
//=====================================
#define RHYTHMSEARCH_DATA_MAX	(15)	//�v���񐔍ő�
#define RHYTHMSEARCH_DATA_MIN	(10)	//�v���񐔍ŏ�
#define TOUCH_DIAMOND_W	(48)				//�^�b�`��
#define TOUCH_DIAMOND_H	(48)

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
	OBJREGID_TOUCH_PLT_M,
	OBJREGID_TOUCH_CHR_M,
	OBJREGID_TOUCH_CEL_M,
	OBJREGID_TOUCH_PLT_S,
	OBJREGID_TOUCH_CHR_S,
	OBJREGID_TOUCH_CEL_S,

  OBJREGID_NUM_PLT_M,
  OBJREGID_NUM_CHR_M,
  OBJREGID_NUM_CEL_M,

	OBJREGID_MAX
};

//-------------------------------------
///		BACKOBJ
//=====================================
//���OBJ�̐�
#define BACKOBJ_CLWK_MAX	(30)
//���o���鐔
#define BACKOBJ_EMIT_NUM	(12)
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
	BACKOBJ_COLOR_RED			= 0,
	BACKOBJ_COLOR_ORANGE	= 1,
	BACKOBJ_COLOR_YELLOW	= 2,
	BACKOBJ_COLOR_YEGREEN	= 3,
	BACKOBJ_COLOR_GREEN		= 4,
	BACKOBJ_COLOR_WATER		= 5,
	BACKOBJ_COLOR_BLUE		= 6,
} BACKOBJ_COLOR;

//�P��OBJ�������܂ł̃V���N
#define BACKOBJ_MOVE_SYNC	(10)

#define BACKOBJ_ONE_MOVE_SYNC_MIN	(110)
#define BACKOBJ_ONE_MOVE_SYNC_MAX	(160)
#define BACKOBJ_ONE_MOVE_SYNC_DIF	(BACKOBJ_ONE_MOVE_SYNC_MAX-BACKOBJ_ONE_MOVE_SYNC_MIN)

#define BACKOBJ_ONE_EMIT_SYNC_MIN	(40)
#define BACKOBJ_ONE_EMIT_SYNC_MAX	(45)
#define BACKOBJ_ONE_EMIT_SYNC_DIF	(BACKOBJ_ONE_EMIT_SYNC_MAX-BACKOBJ_ONE_EMIT_SYNC_MIN)

enum
{
	BACKOBJ_SYS_MAIN,
	BACKOBJ_SYS_NUM,
} ;

//-------------------------------------
///	�J����ǃ_�E���̒萔
//=====================================
#define COUNTDOWN_NUM_OBJ_MAX   (3)
#define COUNTDOWN_ONE_COUNT_MAX (60)

//-------------------------------------
///	CLWK�擾
//=====================================
typedef enum
{	
	
	CLWKID_BACKOBJ_TOP_M,
	CLWKID_BACKOBJ_END_M = CLWKID_BACKOBJ_TOP_M + BACKOBJ_CLWK_MAX,

  CLWKID_NUM_TOP_M,
  CLWKID_NUM_END_M  = CLWKID_NUM_TOP_M + COUNTDOWN_NUM_OBJ_MAX,

	CLWKID_MAX
}CLWKID;

//-------------------------------------
///	MSGWNDID
//=====================================
typedef enum {
	MSGWNDID_TEXT,

	MSGWNDID_MAX
} MSGWNDID;

//-------------------------------------
///	�l���ʉ�ʂ̒萔
//=====================================
enum
{	
	ROR_MSGWNDID_TITLE,
	ROR_MSGWNDID_MSG,
	ROR_MSGWNDID_DEBUG,

	ROR_MSGWNDID_MAX,
};

#define ROR_MSGWND_TITLE_X	(2)
#define ROR_MSGWND_TITLE_Y	(3)
#define ROR_MSGWND_TITLE_W	(20)
#define ROR_MSGWND_TITLE_H	(2)
#define ROR_MSGWND_MSG_X		(6)
#define ROR_MSGWND_MSG_Y		(9)
#define ROR_MSGWND_MSG_W		(20)
#define ROR_MSGWND_MSG_H		(6)
#define ROR_MSGWND_DEBUG_X	(2)
#define ROR_MSGWND_DEBUG_Y	(16)
#define ROR_MSGWND_DEBUG_W	(28)
#define ROR_MSGWND_DEBUG_H	(4)

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
	GFL_ARCUTIL_TRANSINFO	frame_char;		//���ʗp���b�Z�[�W�t���[��
} GRAPHIC_BG_WORK;

//-------------------------------------
///	OBJ�֌W
//=====================================
typedef struct {
	GFL_CLUNIT *p_clunit;
	u32				reg_id[OBJREGID_MAX];
	GFL_CLWK	 *p_clwk[CLWKID_MAX];
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
///	�T�[�`��
//=====================================
typedef struct
{
	//�ǂ�����������
//	GFL_POINT		pos;
	//����������
	u32					prog_ms;	//�o�ߎ���
	s32					diff_ms;	//���̎���
} RHYTHMSEARCH_DATA;
typedef struct
{	
	RHYTHMSEARCH_DATA	data[RHYTHMSEARCH_DATA_MAX];
	u32			data_idx;
	OSTick	start_time;
} RHYTHMSEARCH_WORK;

//-------------------------------------
///	RHYTHM�p�l�b�g
//=====================================
//�l�b�g���C��
typedef struct {
	COMPATIBLE_IRC_SYS	*p_irc;
	u32 seq;
	RHYTHMSEARCH_WORK	result_recv;
	RHYTHMSEARCH_WORK	result_recv_my;
	RHYTHMSEARCH_WORK	result_send;
	BOOL is_recv;
} RHYTHMNET_WORK;

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
	u32								sync_now;	//�P�̃��[�N���J�n����܂ł̃V���N
	u32								sync_max;
} BACKOBJ_WORK;
#if 0
//-------------------------------------
///	�l���щ��
//=====================================
typedef struct 
{
	u32	seq;
	const MSG_WORK *cp_msg;
	const RHYTHMSEARCH_WORK *cp_search;
	MSGWND_WORK			msgwnd[ROR_MSGWNDID_MAX];
} RHYTHM_ONLYRESULT_WORK;
#endif
//-------------------------------------
///	�J�E���g
//=====================================
typedef struct 
{
	GFL_CLWK					*p_clwk[COUNTDOWN_NUM_OBJ_MAX];
  u32               cnt;
  u32               cnt_idx;  //����J�E���g������
  BOOL              is_start;
  BOOL              is_end;
} COUNTDOWN_WORK;

//-------------------------------------
///	�p���b�g�t�F�[�h���[�N
//=====================================
#define PLTFADE_MAX (1)
typedef struct 
{
  u16   plt_buff[PLTFADE_MAX];
  u16   plt_cnt[PLTFADE_MAX];
  BOOL  plt_start[PLTFADE_MAX];
  BOOL  plt_flip[PLTFADE_MAX];
  BOOL  is_start;
} PLTFADE_WORK;


//-------------------------------------
///	���Y���`�F�b�N���C�����[�N
//=====================================
typedef struct _RHYTHM_MAIN_WORK RHYTHM_MAIN_WORK;
typedef void (*SEQ_FUNCTION)( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
struct _RHYTHM_MAIN_WORK
{
	//�O���t�B�b�N���W���[��
	GRAPHIC_WORK		grp;
	MSG_WORK				msg;
	MSGWND_WORK			msgwnd[MSGWNDID_MAX];

	//�l�b�g
	RHYTHMNET_WORK	net;

	//���ʕ\��
	//RHYTHM_ONLYRESULT_WORK	onlyresult;

	//�w�ʂ҂��҂�
	BACKOBJ_WORK		backobj[BACKOBJ_SYS_NUM];

	//����ʃo�[
	APPBAR_WORK			*p_appbar;

  //�J�E���g�_�E��
  COUNTDOWN_WORK  cntdown;

  PLTFADE_WORK    pltfade;

	MSGWND_WORK			msgtitle;	//�^�C�g�����b�Z�[�W
	MSGWND_WORK			msgstart;	//�^�b�`�X�^�[�g

	//�V�[�P���X�Ǘ�
	SEQ_FUNCTION		seq_function;
	u16		seq;
	u16		cnt;
	BOOL is_end;
	BOOL is_next_proc;

	//�v��
	RHYTHMSEARCH_WORK	search;

	//���̑�
	u32		marker_cnt;	//�}�[�J�[�\������
  BOOL  is_sub_seq; //�T�u�V�[�P���X����J�n
  u16   sub_seq;    //�������Ȃ��瑊��Ɛڑ��ł���悤�ɂ��邽��

	//����
	IRC_RHYTHM_PARAM	*p_param;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//proc
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
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
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
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
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, const MSG_WORK *cp_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y );
static void MSGWND_PrintCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID );
static void MSGWND_PrintCenterColor( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, PRINTSYS_LSB lsb );
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y );
static void MSGWND_PrintPlayerName( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, const COMPATIBLE_STATUS *cp_status, u16 x, u16 y, HEAPID heapID );
static void MSGWND_Clear( MSGWND_WORK* p_wk );
//ONLYRESULT
#if 0
static void RHYTHM_ONLYRESULT_Init( RHYTHM_ONLYRESULT_WORK* p_wk, u8 frm, const MSG_WORK *cp_msg, const RHYTHMSEARCH_WORK *cp_search,  HEAPID heapID );
static void RHYTHM_ONLYRESULT_Exit( RHYTHM_ONLYRESULT_WORK* p_wk );
static BOOL RHYTHM_ONLYRESULT_Main( RHYTHM_ONLYRESULT_WORK* p_wk );
#endif
static void COUNTDOWN_Init( COUNTDOWN_WORK *p_wk, const GRAPHIC_WORK *cp_grp, HEAPID heapID );
static void COUNTDOWN_Exit( COUNTDOWN_WORK *p_wk );
static void COUNTDOWN_Main( COUNTDOWN_WORK *p_wk );
static void COUNTDOWN_Start( COUNTDOWN_WORK *p_wk );
static BOOL COUNTDOWN_IsEnd( const COUNTDOWN_WORK *cp_wk );
//SEQ
static void SEQ_Change( RHYTHM_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function );
static void SEQ_End( RHYTHM_MAIN_WORK *p_wk );
//SEQ_FUNC
static void SEQFUNC_StartGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_CountDown( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_MainGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_Result( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_SceneError( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
//RHYTHMSEARCH
static void RHYTHMSEARCH_Init( RHYTHMSEARCH_WORK *p_wk );
static void RHYTHMSEARCH_Exit( RHYTHMSEARCH_WORK *p_wk );
static void RHYTHMSEARCH_Start( RHYTHMSEARCH_WORK *p_wk );
static BOOL RHYTHMSEARCH_IsEnd( const RHYTHMSEARCH_WORK *cp_wk );
static void RHYTHMSEARCH_SetData( RHYTHMSEARCH_WORK *p_wk, const GFL_POINT *cp_pos );
static BACKOBJ_COLOR RHYTHMSEARCH_PlaySEByRhythmSpeed( const RHYTHMSEARCH_WORK *cp_wk );
//net
static void RHYTHMNET_Init( RHYTHMNET_WORK *p_wk, COMPATIBLE_IRC_SYS *p_irc );
static void RHYTHMNET_Exit( RHYTHMNET_WORK *p_wk );
static BOOL RHYTHMNET_SendResultData( RHYTHMNET_WORK *p_wk, const RHYTHMSEARCH_WORK *cp_data );
static void RHYTHMNET_GetResultData( const RHYTHMNET_WORK *cp_wk, RHYTHMSEARCH_WORK *p_data );
//net_recv
static void NETRECV_Result( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle );
static u8* NETRECV_GetBufferAddr(int netID, void* pWork, int size);
//�ėp
static BOOL TP_GetDiamondTrg( const GFL_POINT *cp_diamond, GFL_POINT *p_trg );
static u8		CalcScore( const RHYTHM_MAIN_WORK *cp_wk );
static void RHYTHM_MainPltAnm( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end );
//BACKOBJ
static void BACKOBJ_Init( BACKOBJ_WORK *p_wk, const GRAPHIC_WORK *cp_grp, BACKOBJ_MOVE_TYPE type, u32 clwk_ofs, int sf_type );
static void BACKOBJ_Exit( BACKOBJ_WORK *p_wk );
static void BACKOBJ_Main( BACKOBJ_WORK *p_wk );
static void BACKOBJ_StartEmit( BACKOBJ_WORK *p_wk, const GFL_POINT *cp_pos, BACKOBJ_COLOR color );
//BACKOBJ_ONE
static void BACKOBJ_ONE_Init( BACKOBJ_ONE *p_wk, GFL_CLWK *p_clwk, int sf_type );
static void BACKOBJ_ONE_Main( BACKOBJ_ONE *p_wk );
static void BACKOBJ_ONE_Start( BACKOBJ_ONE *p_wk, const GFL_POINT *cp_start, const GFL_POINT *cp_end, BACKOBJ_COLOR color, u32 sync );
static BOOL BACKOBJ_ONE_IsMove( const BACKOBJ_ONE *cp_wk );
//PLTFADE_WORK
static void PLTFADE_Init( PLTFADE_WORK *p_wk, HEAPID heapID );
static void PLTFADE_Exit( PLTFADE_WORK *p_wk );
static void PLTFADE_Main( PLTFADE_WORK *p_wk );
static void PLTFADE_Start( PLTFADE_WORK *p_wk );

#ifdef PM_DEBUG
static STRBUF * DEBUGPRINT_CreateWideChar( const u16 *cp_str, HEAPID heapID );
static STRBUF * DEBUGPRINT_CreateWideCharNumber( const u16 *cp_str, int number, HEAPID heapID );
#endif

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	���Y���`�F�b�N�p�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA IrcRhythm_ProcData	= 
{	
	IRC_RHYTHM_PROC_Init,
	IRC_RHYTHM_PROC_Main,
	IRC_RHYTHM_PROC_Exit,
};
//-------------------------------------
///	BG�V�X�e��
//=====================================
typedef enum 
{
	GRAPHIC_BG_FRAME_M_INFOWIN,
	GRAPHIC_BG_FRAME_M_BACK,
	GRAPHIC_BG_FRAME_M_TEXT,
	GRAPHIC_BG_FRAME_S_ROGO,
	GRAPHIC_BG_FRAME_S_TEXT,
	GRAPHIC_BG_FRAME_S_BACK,
	GRAPHIC_BG_FRAME_S_TITLE,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	INFOWIN_BG_FRAME, GFL_BG_FRAME1_M, GFL_BG_FRAME2_M, GFL_BG_FRAME0_S, GFL_BG_FRAME1_S, GFL_BG_FRAME2_S,GFL_BG_FRAME3_S
};
static const u32 sc_bgcnt_mode[ GRAPHIC_BG_FRAME_MAX ] = 
{
	GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_TEXT, GFL_BG_MODE_AFFINE,
};
static const GFL_BG_BGCNT_HEADER sc_bgcnt_data[ GRAPHIC_BG_FRAME_MAX ] = 
{
	// INFOWIN_BG_FRAME	
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GFL_BG_FRAME1_M
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
	},
	// GFL_BG_FRAME2_M
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x18000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
	// GFL_BG_FRAME0_S
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GFL_BG_FRAME1_S
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
	},
	// GFL_BG_FRAME2_S
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
	},
	// GFL_BG_FRAME3_S
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
		GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ256_128x128,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
};

//-------------------------------------
///	�^�b�`�͈�
//=====================================
static const GFL_POINT	sc_diamond_pos[]	=
{	
	{	
		31, 47
	},
	{	
		79, 47
	},
	{	
		127, 47
	},
	{	
		175, 47
	},
	{	
		223, 47
	},
	{	
		55, 71
	},
	{	
		103, 71
	},
	{	
		151, 71
	},
	{	
		199, 71
	},
	{	
		31, 95
	},
	{	
		79, 95
	},
	{	
		127, 95
	},
	{	
		175, 95
	},
	{	
		223, 95
	},
	{	
		55, 119
	},
	{	
		103, 119
	},
	{	
		151, 119
	},
	{	
		199, 119
	},
	{	
		31, 143
	},
	{	
		79, 143
	},
	{	
		127, 143
	},
	{	
		175, 143
	},
	{	
		223, 143
	},

};

//-------------------------------------
///	NET
//=====================================
enum
{	
	NETRECV_RESULT	= GFL_NET_CMD_IRCRHYTHM,
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
 *	@brief	���Y���`�F�b�N	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	RHYTHM_MAIN_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCRHYTHM, 0x20000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(RHYTHM_MAIN_WORK), HEAPID_IRCRHYTHM );
	GFL_STD_MemClear( p_wk, sizeof(RHYTHM_MAIN_WORK) );
	p_wk->p_param	= p_param;

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
	s_is_debug_only_play	= p_wk->p_param->is_only_play;
#endif

	//���W���[��������
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCRHYTHM );
	MSG_Init( &p_wk->msg, MSG_FONT_TYPE_LARGE, HEAPID_IRCRHYTHM );
	{
		GAME_COMM_SYS_PTR comm	= NULL;
		if( p_wk->p_param->p_gamesys )
		{	
			comm	= GAMESYSTEM_GetGameCommSysPtr(p_wk->p_param->p_gamesys);
		}
		//INFOWIN_Init( INFOWIN_BG_FRAME, INFOWIN_PLT_NO, comm, HEAPID_IRCRHYTHM );
	}
	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_TEXT], sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TEXT],
			MSGWND_TEXT_X, MSGWND_TEXT_Y, MSGWND_TEXT_W, MSGWND_TEXT_H, RHYTHM_BG_PAL_S_08, HEAPID_IRCRHYTHM );
	BmpWinFrame_Write( p_wk->msgwnd[MSGWNDID_TEXT].p_bmpwin, WINDOW_TRANS_ON, 
					GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->grp.bg.frame_char), RHYTHM_BG_PAL_S_06 );

  PLTFADE_Init( &p_wk->pltfade, HEAPID_IRCRHYTHM );

  if( p_wk->p_param->p_gamesys )
  {	
    MSGWND_PrintPlayerName( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, 
        RHYTHM_STR_000, p_wk->p_param->p_you_status,  0, 0, HEAPID_IRCRHYTHM );
  }
  else
  {	
      MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_STR_000, 0, 0 );
  }

  COUNTDOWN_Init( &p_wk->cntdown, &p_wk->grp, HEAPID_IRCRHYTHM );

	//�^�C�g��������쐬
	MSGWND_InitEx( &p_wk->msgtitle, sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE],
			MSGWND_TITLE_X, MSGWND_TITLE_Y, MSGWND_TITLE_W, MSGWND_TITLE_H, RHYTHM_BG_PAL_S_08, 1, GFL_BMP_CHRAREA_GET_B, HEAPID_IRCRHYTHM );
	MSGWND_PrintCenter( &p_wk->msgtitle, &p_wk->msg, RHYTHM_TITLE_000 );
	GFL_BG_SetScaleReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_SCALE_X_SET, TITLE_STR_SCALE_X );
	GFL_BG_SetScaleReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_SCALE_Y_SET, TITLE_STR_SCALE_Y );
	GFL_BG_SetRotateCenterReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_CENTER_X_SET, (MSGWND_TITLE_X + MSGWND_TITLE_W/2)*8 );
	GFL_BG_SetRotateCenterReq( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TITLE], GFL_BG_CENTER_Y_SET, (MSGWND_TITLE_Y + MSGWND_TITLE_H/2)*8 );

  //�^�b�`�X�^�[�g������쐬
	MSGWND_InitEx( &p_wk->msgstart, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT],
			MSGWND_START_X, MSGWND_START_Y, MSGWND_START_W, MSGWND_START_H, RHYTHM_BG_PAL_M_13, 0, GFL_BMP_CHRAREA_GET_F, HEAPID_IRCRHYTHM );
	//MSGWND_PrintCenterColor( &p_wk->msgstart, &p_wk->msg, RHYTHM_STR_005, PRINTSYS_LSB_Make(0xf,2,0) );

  //���Y���p�l�b�g�쐬
	RHYTHMNET_Init( &p_wk->net, p_wk->p_param->p_irc );

  //���Y�����m�쐬
	RHYTHMSEARCH_Init( &p_wk->search );

	BACKOBJ_Init( &p_wk->backobj[BACKOBJ_SYS_MAIN], &p_wk->grp, BACKOBJ_MOVE_TYPE_EMITER, CLWKID_BACKOBJ_TOP_M, CLSYS_DEFREND_MAIN );

	{	
		GFL_CLUNIT	*p_unit	= GRAPHIC_GetClunit( &p_wk->grp );
		p_wk->p_appbar	= APPBAR_Init( APPBAR_OPTION_MASK_RETURN, p_unit, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_INFOWIN], RHYTHM_BG_PAL_M_14, RHYTHM_OBJ_PAL_M_13, APP_COMMON_MAPPING_128K, MSG_GetFont(&p_wk->msg ), MSG_GetPrintQue(&p_wk->msg ), HEAPID_IRCRHYTHM );
	}

	//���Y���V�[���Z�b�g
	COMPATIBLE_IRC_SetScene( p_wk->p_param->p_irc, COMPATIBLE_SCENE_RHYTHM );

  //�P�x�ύX
  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BD, -8 );

	SEQ_Change( p_wk, SEQFUNC_StartGame );
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���`�F�b�N	���C���v���Z�X�j������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	RHYTHM_MAIN_WORK	*p_wk;

	p_wk	= p_work;

#if 0
	//���̃v���b�N�\��
	if( p_wk->is_next_proc )
	{	
		if( p_wk->p_param->p_gamesys == NULL )
		{	
			GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(irc_result), &IrcResult_ProcData,p_wk->p_param );
		}
		else
		{	
			GAMESYSTEM_SetNextProc( p_wk->p_param->p_gamesys, FS_OVERLAY_ID(irc_result), &IrcResult_ProcData,p_wk->p_param );
		}
	}
#endif


	{		
		int i;
		for( i = 0; i < BACKOBJ_SYS_NUM; i++ )
		{	
			BACKOBJ_Exit( &p_wk->backobj[i] );
		}
	}

  G2_BlendNone();

	//APPBAR
	APPBAR_Exit( p_wk->p_appbar );	

	RHYTHMNET_Exit( &p_wk->net );

  COUNTDOWN_Exit( &p_wk->cntdown );

  PLTFADE_Exit( &p_wk->pltfade );
	MSGWND_Exit( &p_wk->msgstart );
	MSGWND_Exit( &p_wk->msgtitle );
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
	GFL_HEAP_DeleteHeap( HEAPID_IRCRHYTHM );
	
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���`�F�b�N	���C���v���Z�X���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
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

	RHYTHM_MAIN_WORK	*p_wk;
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

    PLTFADE_Main( &p_wk->pltfade );
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
		GF_ASSERT_MSG( 0, "IRC_RHYTHM_PROC_Main��SEQ�G���[ %d", *p_seq );
	}

	//INFOWIN_Update();
	if( MSG_Main( &p_wk->msg ) )
	{
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Main( &p_wk->msgwnd[i], &p_wk->msg );
			MSGWND_Main( &p_wk->msgtitle, &p_wk->msg );
			MSGWND_Main( &p_wk->msgstart, &p_wk->msg );
		}
	}

	GRAPHIC_Draw( &p_wk->grp );

	{		
		int i;
		BACKOBJ_Main( &p_wk->backobj[BACKOBJ_SYS_MAIN] );
	}

	//APPBAR
	APPBAR_Main( p_wk->p_appbar );

	//�V�[�����p���I�ɑ���
	COMPATIBLE_IRC_SendSceneContinue( p_wk->p_param->p_irc );

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
			GFL_BG_SetBGControl( sc_bgcnt_frame[i], &sc_bgcnt_data[i], sc_bgcnt_mode[i] );
			GFL_BG_ClearFrame( sc_bgcnt_frame[i] );
			GFL_BG_SetVisible( sc_bgcnt_frame[i], VISIBLE_ON );
		}
	}

	//�ǂݍ��ݐݒ�
	{	
		ARCHANDLE *p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCCOMPATIBLE, heapID );
		//�p���b�g
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_irccompatible_gra_aura_bg_NCLR,
				PALTYPE_MAIN_BG, RHYTHM_BG_PAL_M_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_irccompatible_gra_aura_bg_NCLR,
				PALTYPE_SUB_BG, RHYTHM_BG_PAL_S_00*0x20, 0, heapID );



		//�L����
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_rhythm_bg_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_aura_bg_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_irccompatible_gra_title_rogo_NCGR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_ROGO], 0, 0, FALSE, heapID );
	
		//�X�N���[��
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_rhythm_bg_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_aura_bg_block_b_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_irccompatible_gra_title_rhythm_NSCR,
				sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_ROGO], 0, 0, FALSE, heapID );

		//���N
		GFL_BG_FillCharacter( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], 0, 1, 0 );
		p_wk->frame_char	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_irccompatible_gra_ue_frame_NCGR, sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], 0, FALSE, heapID );

		GFL_ARC_CloseDataHandle( p_handle );

    //�^�b�`�X�^�[�g�p�t�H���g
    GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_MAIN_BG, RHYTHM_BG_PAL_M_13*0x20, 0x20, heapID );

    GFL_BG_FillCharacter( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_TEXT ], 0, 1, 0 );
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

	//���\�[�X�j��
	{	
		GFL_BG_FillCharacterRelease(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_TEXT],1,0);
		GFL_BG_FreeCharacterArea(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT],
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_char),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->frame_char));
		GFL_BG_FillCharacterRelease(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT],1,0);
	}

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

		p_wk->reg_id[OBJREGID_TOUCH_PLT_M]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_irccompatible_gra_rhythm_aura_NCLR, CLSYS_DRAW_MAIN, RHYTHM_OBJ_PAL_M_00*0x20, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CHR_M]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_irccompatible_gra_rhythm_aura_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CEL_M]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_irccompatible_gra_rhythm_aura_NCER, NARC_irccompatible_gra_rhythm_aura_NANR, heapID );

    p_wk->reg_id[OBJREGID_NUM_PLT_M]  = GFL_CLGRP_PLTT_RegisterEx( p_handle, 
				NARC_irccompatible_gra_aura_obj_NCLR, CLSYS_DRAW_MAIN, RHYTHM_OBJ_PAL_M_07*0x20, 0xC,1, heapID );
    p_wk->reg_id[OBJREGID_NUM_CHR_M]  = GFL_CLGRP_CGR_Register( p_handle,
				NARC_irccompatible_gra_aura_obj_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );
    p_wk->reg_id[OBJREGID_NUM_CEL_M]  = GFL_CLGRP_CELLANIM_Register( p_handle,
        NARC_irccompatible_gra_aura_obj_NCER, NARC_irccompatible_gra_aura_obj_NANR, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_PLT_S]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_irccompatible_gra_rhythm_aura_NCLR, CLSYS_DRAW_SUB, RHYTHM_OBJ_PAL_S_00*0x20, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CHR_S]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_irccompatible_gra_rhythm_aura_NCGR, FALSE, CLSYS_DRAW_SUB, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CEL_S]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_irccompatible_gra_rhythm_aura_NCER, NARC_irccompatible_gra_rhythm_aura_NANR, heapID );
		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK�쐬
	{	
		int i;
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		for( i = CLWKID_BACKOBJ_TOP_M; i < CLWKID_BACKOBJ_END_M; i++  )
		{	
			p_wk->p_clwk[i]	= GFL_CLACT_WK_Create( p_wk->p_clunit, 
					p_wk->reg_id[OBJREGID_TOUCH_CHR_M],
					p_wk->reg_id[OBJREGID_TOUCH_PLT_M],
					p_wk->reg_id[OBJREGID_TOUCH_CEL_M],
					&cldata,
					CLSYS_DEFREND_MAIN,
					heapID
					);
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
			GFL_CLACT_WK_SetBgPri( p_wk->p_clwk[i], 3 );
			GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[i], TRUE );
			GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[i], 0 );
		}

    cldata.pos_x  = 128;
    cldata.pos_y  = 96;
    for( i = CLWKID_NUM_TOP_M; i < CLWKID_NUM_END_M; i++  )
		{	
      cldata.anmseq = 1+(i-CLWKID_NUM_TOP_M);
			p_wk->p_clwk[i]	= GFL_CLACT_WK_Create( p_wk->p_clunit, 
					p_wk->reg_id[OBJREGID_NUM_CHR_M],
					p_wk->reg_id[OBJREGID_NUM_PLT_M],
					p_wk->reg_id[OBJREGID_NUM_CEL_M],
					&cldata,
					CLSYS_DEFREND_MAIN,
					heapID
					);
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
      GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk[i], 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );
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
		GFL_CLGRP_PLTT_Release( p_wk->reg_id[OBJREGID_TOUCH_PLT_M] );
		GFL_CLGRP_CGR_Release( p_wk->reg_id[OBJREGID_TOUCH_CHR_M] );
		GFL_CLGRP_CELLANIM_Release( p_wk->reg_id[OBJREGID_TOUCH_CEL_M] );
		GFL_CLGRP_PLTT_Release( p_wk->reg_id[OBJREGID_TOUCH_PLT_S] );
		GFL_CLGRP_CGR_Release( p_wk->reg_id[OBJREGID_TOUCH_CHR_S] );
		GFL_CLGRP_CELLANIM_Release( p_wk->reg_id[OBJREGID_TOUCH_CEL_S] );

    GFL_CLGRP_PLTT_Release( p_wk->reg_id[OBJREGID_NUM_PLT_M] );
		GFL_CLGRP_CGR_Release( p_wk->reg_id[OBJREGID_NUM_CHR_M] );
		GFL_CLGRP_CELLANIM_Release( p_wk->reg_id[OBJREGID_NUM_CEL_M] );
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
	case MSG_FONT_TYPE_SMALL:
		p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_small_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
		break;
	default:
		GF_ASSERT_MSG( 0, "MSGFONT_ERRO %d", font );
	};

	p_wk->p_print_que = PRINTSYS_QUE_Create( heapID );

	p_wk->p_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_rhythm_dat, heapID );

	p_wk->p_wordset	= WORDSET_Create( heapID );

	{	
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
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
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe, u8 x, u8 y, u8 w, u8 h, u8 plt, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
	p_wk->clear_chr	= 0xF;
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( bgframe, x, y, w, h, plt, GFL_BMP_CHRAREA_GET_B );
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( TEXTSTR_BUFFER_LENGTH, heapID );
	PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );
	MSGWND_Clear( p_wk );
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
  MSGWND_PrintCenterColor( p_wk, cp_msg, strID, PRINTSYS_LSB_Make(0xf,0xe,4) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\���ʂ̒����ɕ�����\��  �F�w��
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	MSG_WORK *cp_msg	�����Ǘ�
 *	@param	strID							����ID
 *	@param  color             �����F�w��
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintCenterColor( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, PRINTSYS_LSB lsb )
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
	PRINT_UTIL_PrintColor( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font, lsb );
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
static void MSGWND_PrintPlayerName( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, const COMPATIBLE_STATUS *cp_status, u16 x, u16 y, HEAPID heapID )
{
  const GFL_MSGDATA* cp_msgdata;
  WORDSET *p_wordset;

  //��[����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin),p_wk->clear_chr );

  //���W���[���擾
  p_wordset   = MSG_GetWordSet( cp_msg );
  cp_msgdata  = MSG_GetMsgDataConst( cp_msg );

  //���[�h�Z�b�g�ɓo�^
  { 
    WORDSET_RegisterPlayerName( p_wordset, 0, (const MYSTATUS*)cp_status->my_status );
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
 *				�l���ʉ��
 */
//=============================================================================
#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	�l���ʉ��	������
 *
 *	@param	RHYTHM_ONLYRESULT_WORK* p_wk	���[�N
 *	@param	frm									�g�p�t���[��
 *	@param	MSG_WORK *cp_wk			���b�Z�[�W
 *	@param	heapID							�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHM_ONLYRESULT_Init( RHYTHM_ONLYRESULT_WORK* p_wk, u8 frm, const MSG_WORK *cp_msg, const RHYTHMSEARCH_WORK *cp_search,  HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(RHYTHM_ONLYRESULT_WORK) );
	p_wk->cp_msg		= cp_msg;
	p_wk->cp_search	= cp_search;

	MSGWND_Init( &p_wk->msgwnd[ROR_MSGWNDID_TITLE], frm, ROR_MSGWND_TITLE_X, ROR_MSGWND_TITLE_Y,
				ROR_MSGWND_TITLE_W, ROR_MSGWND_TITLE_H, heapID );
	MSGWND_Init( &p_wk->msgwnd[ROR_MSGWNDID_MSG], frm, ROR_MSGWND_MSG_X, ROR_MSGWND_MSG_Y,
				ROR_MSGWND_MSG_W, ROR_MSGWND_MSG_H, heapID );

	MSGWND_Init( &p_wk->msgwnd[ROR_MSGWNDID_DEBUG], frm, ROR_MSGWND_DEBUG_X, ROR_MSGWND_DEBUG_Y,
				ROR_MSGWND_DEBUG_W, ROR_MSGWND_DEBUG_H, heapID );

	MSGWND_Print( &p_wk->msgwnd[ROR_MSGWNDID_TITLE], cp_msg, RHYTHM_RES_000, 0, 0 );


	//������쐬
	{
		u32 msg_idx;
		u32 ms_sum;
		int i;

		ms_sum	= 0;
		for( i = 1; i <10; i++ )
		{	
			ms_sum	+= cp_search->data[i].diff_ms;
		}
		ms_sum	/=	9;

		if( 40 <= ms_sum )
		{	
			msg_idx	= 0;
		}
		else if( 20 < ms_sum && ms_sum < 40 )
		{	
			msg_idx	= 1;
		}
		else if( ms_sum <= 20 )
		{	
			msg_idx	= 2;
		}
		else
		{	
			GF_ASSERT( 0 );
		}
		OS_Printf( "���̕��ϒl %d\n", ms_sum );

		MSGWND_Print( &p_wk->msgwnd[ROR_MSGWNDID_MSG], cp_msg, RHYTHM_RESMSG_000 + msg_idx, 0, 0 );
	}

	//�f�o�b�O������쐬
	{	
		MSGWND_WORK* p_msgwnd;
		PRINT_QUE*	p_que;
		GFL_FONT*		p_font;
		int i;
		u32 ms;
		u32 ms_sum;
		STRBUF *p_strbuf;

		p_msgwnd	= &p_wk->msgwnd[ROR_MSGWNDID_DEBUG];
		p_font	= GFL_FONT_Create( ARCID_FONT,
					NARC_font_small_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );		
		ms_sum	= 0;
		for( i = 1; i < 10; i++ )
		{	
			ms	= cp_search->data[i].diff_ms;
			ms_sum	+= ms;

			p_strbuf	= DEBUGPRINT_CreateWideCharNumber( L"%d���",i, heapID ); 
			PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 0, p_strbuf, p_font );
			GFL_STR_DeleteBuffer( p_strbuf );

			p_strbuf	= DEBUGPRINT_CreateWideCharNumber( L" %d", ms, heapID ); 
			PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 10, p_strbuf, p_font );
			GFL_STR_DeleteBuffer( p_strbuf );
		}

		p_strbuf	= DEBUGPRINT_CreateWideChar( L"����", heapID ); 
		PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 0, p_strbuf, p_font );
		GFL_STR_DeleteBuffer( p_strbuf );


		p_strbuf	= DEBUGPRINT_CreateWideCharNumber( L" %d", ms_sum/9, heapID ); 
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
 *	@param	RHYTHM_ONLYRESULT_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHM_ONLYRESULT_Exit( RHYTHM_ONLYRESULT_WORK* p_wk )
{	
	int i;
	for( i = 0; i < ROR_MSGWNDID_MAX; i++ )
	{	
		MSGWND_Exit( &p_wk->msgwnd[i] );
	}
	GFL_STD_MemClear( p_wk, sizeof(RHYTHM_ONLYRESULT_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�l���ʉ��	���C������
 *
 *	@param	RHYTHM_ONLYRESULT_WORK* p_wk ���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@retval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL RHYTHM_ONLYRESULT_Main( RHYTHM_ONLYRESULT_WORK* p_wk )
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
 *    COUNTDOWN_WORK
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �J�E���g�_�E��  ������
 *
 *	@param	COUNTDOWN_WORK *p_wk   ���[�N
 *	@param	GRAPHIC_WORK *cp_grp  �O���t�B�b�N
 *	@param  HEAPID heapID         �q�[�vID
 */
//-----------------------------------------------------------------------------
static void COUNTDOWN_Init( COUNTDOWN_WORK *p_wk, const GRAPHIC_WORK *cp_grp, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(COUNTDOWN_WORK) );

  { 
    int i;
    for( i = 0; i < COUNTDOWN_NUM_OBJ_MAX; i++ )
    { 
      p_wk->p_clwk[0]  = GRAPHIC_GetClwk( cp_grp, CLWKID_NUM_TOP_M + i );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �J�E���g�_�E��  �j��
 *
 *	@param	COUNTDOWN_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void COUNTDOWN_Exit( COUNTDOWN_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_wk, sizeof(COUNTDOWN_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �J�E���g�_�E��  ���C������
 *
 *	@param	COUNTDOWN_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void COUNTDOWN_Main( COUNTDOWN_WORK *p_wk )
{ 
  if( p_wk->is_start )
  { 
    if( p_wk->cnt == 0 )
    { 
      PMSND_PlaySE( SEQ_SE_SYS_43 );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[0], TRUE );
      GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[0], (4-p_wk->cnt_idx) );
    }

    if( p_wk->cnt++ >= COUNTDOWN_ONE_COUNT_MAX )
    { 
      p_wk->cnt = 0;
      p_wk->cnt_idx++;
    }

    if( p_wk->cnt_idx == 3 )
    { 
      PMSND_PlaySE( SEQ_SE_SYS_44 );
      p_wk->is_start = FALSE;
      p_wk->is_end    = TRUE;
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[0], FALSE );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �J�E���g�_�E��  �J�n����
 *
 *	@param	COUNTDOWN_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void COUNTDOWN_Start( COUNTDOWN_WORK *p_wk )
{ 
  p_wk->cnt       = 0;
  p_wk->cnt_idx   = 0;
  p_wk->is_start  = TRUE;
}
//----------------------------------------------------------------------------
/** 
 *	@brief  �J�E���g�_�E��  �I�����m
 *
 *	@param	const COUNTDOWN_WORK *cp_wk  ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL COUNTDOWN_IsEnd( const COUNTDOWN_WORK *cp_wk )
{ 
  return cp_wk->is_end;

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
 *	@param	RHYTHM_MAIN_WORK *p_wk	���[�N
 *	@param	seq_function					�V�[�P���X�֐�
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Change( RHYTHM_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�I��
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( RHYTHM_MAIN_WORK *p_wk )
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
 *	@param	RHYTHM_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_StartGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq )
{	
  enum
  { 
    SEQ_INIT,
    SEQ_TOUCH_WAIT,
    SEQ_MAIN,
    SEQ_EXIT,
  };

  switch( *p_seq )
  { 
  case SEQ_INIT:
    p_wk->cnt = 0;
    *p_seq  = SEQ_TOUCH_WAIT;
    break;

  case SEQ_TOUCH_WAIT:
//    if( GFL_UI_TP_GetTrg() )
    { 
      GFL_BG_SetVisible( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT ], FALSE );
      *p_seq  = SEQ_EXIT;
    }
    break;

  case SEQ_EXIT:
    COUNTDOWN_Start( &p_wk->cntdown );
    SEQ_Change( p_wk, SEQFUNC_CountDown );	
    break;
  }
  if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_RETURN )
	{
		p_wk->p_param->result	= IRCRHYTHM_RESULT_RETURN;
		SEQ_End( p_wk );
	}
	
	//�V�[�����قȂ�`�F�b�N
	if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) != 0 )
	{	
		SEQ_Change( p_wk, SEQFUNC_SceneError );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�n�O�J�E���g�_�E������
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_CountDown( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq )
{ 
  COUNTDOWN_Main( &p_wk->cntdown );
  if( COUNTDOWN_IsEnd( &p_wk->cntdown ) )
  { 
    G2_BlendNone();
    RHYTHMSEARCH_Start( &p_wk->search );
    PLTFADE_Start( &p_wk->pltfade );
    SEQ_Change( p_wk, SEQFUNC_MainGame );	
  }

  if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_RETURN )
	{
		p_wk->p_param->result	= IRCRHYTHM_RESULT_RETURN;
		SEQ_End( p_wk );
	}

  //�V�[�����قȂ�`�F�b�N
	if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) != 0 )
	{	
		SEQ_Change( p_wk, SEQFUNC_SceneError );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MainGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq )
{	
	int i;
	GFL_POINT	trg_pos;
	RHYTHMSEARCH_WORK	*p_search;
	p_search	= &p_wk->search;


	for( i = 0; i< NELEMS(sc_diamond_pos); i++ )
	{
		if( TP_GetDiamondTrg( &sc_diamond_pos[i], &trg_pos ) )
		{	
      BACKOBJ_COLOR color;
			color = RHYTHMSEARCH_PlaySEByRhythmSpeed( p_search );
			BACKOBJ_StartEmit( &p_wk->backobj[BACKOBJ_SYS_MAIN], &trg_pos, color );

      if( p_wk->sub_seq == 0 )
      {
        RHYTHMSEARCH_SetData( p_search, &sc_diamond_pos[i] );
      }
		}
	}

	//�v���I���`�F�b�N
	if( RHYTHMSEARCH_IsEnd( p_search ) )
	{
    if( p_wk->is_sub_seq == FALSE )
    { 
      MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_STR_001, 0, 0 );
      PMSND_PlaySE( IRCCOMMON_SE_IRC );
      p_wk->is_sub_seq  = TRUE;
    }

    if( p_wk->is_sub_seq )
    { 
      SEQFUNC_Result( p_wk, &p_wk->sub_seq );
    }
	}

  if( p_wk->is_sub_seq == FALSE )
  { 
    if( APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_RETURN )
    {
      p_wk->p_param->result	= IRCRHYTHM_RESULT_RETURN;
      SEQ_End( p_wk );
    }

    //�V�[�����قȂ�`�F�b�N
    if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) != 0 )
    {	
      SEQ_Change( p_wk, SEQFUNC_SceneError );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	����
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Result( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq )
{	

	enum
	{	
		SEQ_RESULT,
		SEQ_MSG_PRINT,
		SEQ_SENDRESULT,
		SEQ_SCENE,
		SEQ_TIMING,
		SEQ_MSG_PRINT_END,
		SEQ_CALC,
		SEQ_END,
	};

  if( *p_seq < SEQ_MSG_PRINT_END && APPBAR_GetTrg(p_wk->p_appbar) == APPBAR_ICON_RETURN )
  {
    COMPATIBLE_IRC_Cancel( p_wk->p_param->p_irc );
    p_wk->p_param->result	= IRCRHYTHM_RESULT_RETURN;
    SEQ_End( p_wk );
    PMSND_StopSE_byPlayerID( SEPLAYER_SE2 );
    return;
  }	

	switch( *p_seq )
	{	
	case SEQ_RESULT:
		if( COMPATIBLE_IRC_TimingSyncWait( p_wk->p_param->p_irc, COMPATIBLE_TIMING_NO_RHYTHM_RESULT ) )
		{	
			*p_seq	= SEQ_MSG_PRINT;
		}

		break;

	case SEQ_MSG_PRINT:
		MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_STR_002, 0, 0 );
		*p_seq	= SEQ_SENDRESULT;
		break;

	case SEQ_SENDRESULT:
		if( RHYTHMNET_SendResultData( &p_wk->net, &p_wk->search ) )
		{	
			*p_seq	= SEQ_SCENE;
		}
		break;

	case SEQ_SCENE:
		//���̃V�[����ݒ�
		COMPATIBLE_IRC_SetScene(  p_wk->p_param->p_irc, COMPATIBLE_SCENE_AURA );
		if( COMPATIBLE_IRC_SendScene( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_TIMING;
		}
		break;

	case SEQ_TIMING:
		if( COMPATIBLE_IRC_TimingSyncWait( p_wk->p_param->p_irc, COMPATIBLE_TIMING_NO_RHYTHM_END ) )
		{	
			*p_seq	= SEQ_MSG_PRINT_END;
		}
		break;

	case SEQ_MSG_PRINT_END:
		if( p_wk->cnt >= RESULT_SEND_CNT )
		{	
      PMSND_StopSE_byPlayerID( SEPLAYER_SE2 );
			PMSND_PlaySE( IRCMENU_SE_IRC_ON );
			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_STR_003, 0, 0 );
			*p_seq	= SEQ_CALC;
		}
		break;

	case SEQ_CALC:
		p_wk->p_param->score	= CalcScore( p_wk );
    { 
      RHYTHMSEARCH_WORK you;
      RHYTHMNET_GetResultData( &p_wk->net, &you );
      p_wk->p_param->cnt_diff  = MATH_IAbs( (s32)p_wk->search.data_idx - (s32)you.data_idx );
    }
		p_wk->p_param->result	= IRCRHYTHM_RESULT_CLEAR;
		*p_seq	= SEQ_END;
		break;

	case SEQ_END:
		SEQ_End( p_wk );
		return;	//���L�̃A�T�[�g��ʉ߂��Ȃ�����
	}

	//���b�Z�[�W�I���҂�
	if( SEQ_MSG_PRINT <= *p_seq && *p_seq <=  SEQ_MSG_PRINT_END )
	{	
		if( p_wk->cnt <= RESULT_SEND_CNT )
		{	
			p_wk->cnt++;
		}
	}

	//�V�[�����قȂ�`�F�b�N
	if( *p_seq < SEQ_SCENE && COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) != 0 )
	{	
		SEQ_Change( p_wk, SEQFUNC_SceneError );
	}



}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�[���G���[�V�[�P���X
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	���C�����[�N
 *	@param	*p_seq								�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_SceneError( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq )
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
			p_wk->p_param->score	= 0;
			p_wk->p_param->result	= IRCRHYTHM_RESULT_RETURN;
			SEQ_End( p_wk );
		}
		//���肪��Ȃ��
		else if( COMPATIBLE_IRC_CompScene( p_wk->p_param->p_irc ) < 0 )
		{	
			*p_seq	= SEQ_INIT;
		}
		else
		{
			GF_ASSERT(0);
		}
		break;
	case SEQ_INIT:
		p_wk->p_param->score	= 0;
		p_wk->p_param->result	= IRCRHYTHM_RESULT_RESULT;
		*p_seq	= SEQ_MSG_PRINT;
		break;
	case SEQ_MSG_PRINT:
		MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_STR_004, 0, 0 );
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
 *			RHYTHMSEARCH
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���T�[�`�p�v��	������
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMSEARCH_Init( RHYTHMSEARCH_WORK *p_wk )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(RHYTHMSEARCH_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���T�[�`�p�v��	�j��
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMSEARCH_Exit( RHYTHMSEARCH_WORK *p_wk )
{	
	//�j��
	GFL_STD_MemClear( p_wk, sizeof(RHYTHMSEARCH_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���T�[�`�p�v��	�v���J�n
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMSEARCH_Start( RHYTHMSEARCH_WORK *p_wk )
{	
	if( OS_IsTickAvailable() )
	{	
		OS_InitTick();
	}
	//���Ԍv�����J�n
	p_wk->start_time	=		OS_TicksToMilliSeconds(OS_GetTick());
}
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���T�[�`�p�v��	�v���I������
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk		���[�N
 *
 *	@retval	TRUE�v���I��
 *	@retval	FALSE�v���I��
 */
//-----------------------------------------------------------------------------
static BOOL RHYTHMSEARCH_IsEnd( const RHYTHMSEARCH_WORK *cp_wk )
{	
  //5�b�ȏ�o�߂����Ƃ� -> �d�l�ύX�@�P�O��ȏ�^�b�`����
  //if( OS_TicksToMilliSeconds(OS_GetTick()) - cp_wk->start_time >= 5000 ) 
  if( cp_wk->data_idx >= RHYTHMSEARCH_DATA_MIN )
  {	
    return TRUE;
  }
	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���T�[�`�p�v��	�f�[�^�ݒ�
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk	���[�N
 *	@param	GFL_POINT *cp_pos				���W
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMSEARCH_SetData( RHYTHMSEARCH_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	if( p_wk->data_idx < RHYTHMSEARCH_DATA_MAX )
	{	
		//	p_wk->data[ p_wk->data_idx ].pos						= *cp_pos;
		p_wk->data[ p_wk->data_idx ].prog_ms				= OS_TicksToMilliSeconds(OS_GetTick())
			- p_wk->start_time;

		//�ŏ��́A����0
		if( p_wk->data_idx == 0 )
		{	
			p_wk->data[ p_wk->data_idx ].diff_ms	= 0;
		}
		else	//���݂��̍�
		{	
			p_wk->data[ p_wk->data_idx ].diff_ms	= p_wk->data[ p_wk->data_idx ].prog_ms
				- p_wk->data[ p_wk->data_idx-1 ].prog_ms;
		}

		//�Z�b�g�����̂Ŏ��֐i�߂�
		p_wk->data_idx++;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���T�[�`	���̃��Y���X�s�[�h�ɂ�����SE���Ȃ炷
 *
 *	@param	const RHYTHMSEARCH_WORK *cp_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static BACKOBJ_COLOR RHYTHMSEARCH_PlaySEByRhythmSpeed( const RHYTHMSEARCH_WORK *cp_wk )
{	
	u32 se;
  BACKOBJ_COLOR color = BACKOBJ_COLOR_YEGREEN;

	if( cp_wk->data_idx < 2 )
	{	
		se	= IRCRHYTHM_SE_TAP_N;
    color = BACKOBJ_COLOR_YEGREEN;
	}
	else
	{	
		const s32 now	= cp_wk->data[ cp_wk->data_idx - 1 ].diff_ms;
		const s32 pre	= cp_wk->data[ cp_wk->data_idx - 2 ].diff_ms;
		const s32 diff	= now - pre;
		if( 0 <= MATH_IAbs(diff) && MATH_IAbs(diff) < RANGE_RHYTHMSCORE_DIFF_01 )
		{	
			//��̓���
			se = IRCRHYTHM_SE_TAP_N;
      color = BACKOBJ_COLOR_YEGREEN;
			NAGI_Printf( "����SE %d\n", se );
		}
		else if( diff < 0 )
		{	
			//����
			if( MATH_IAbs(diff) < RANGE_RHYTHMSCORE_DIFF_03 )
			{	
				se = IRCRHYTHM_SE_TAP_F1;
        color = BACKOBJ_COLOR_ORANGE;
			}
			else
			{	
				se	=IRCRHYTHM_SE_TAP_F2;
        color = BACKOBJ_COLOR_RED;
			}
		}
		else
		{	
			//�x��
			if( MATH_IAbs(diff) < RANGE_RHYTHMSCORE_DIFF_03 )
			{	
				se	= IRCRHYTHM_SE_TAP_S1;
        color = BACKOBJ_COLOR_WATER;
			}
			else
			{	
				se	= IRCRHYTHM_SE_TAP_S2;
        color = BACKOBJ_COLOR_BLUE;
			}
		}
		NAGI_Printf( "SE %d  diff %d\n", se, diff );
	}
	PMSND_PlaySE( se );
  return color;
}

//=============================================================================
/**
 *							NET
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	NET������
 *
 *	@param	RHYTHMNET_WORK *p_wk	���[�N
 *	@param	*p_irc								�ԊO���V�X�e��
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMNET_Init( RHYTHMNET_WORK *p_wk, COMPATIBLE_IRC_SYS *p_irc )
{	
	GFL_STD_MemClear( p_wk, sizeof(RHYTHMNET_WORK) );
	p_wk->p_irc	= p_irc;

	COMPATIBLE_IRC_AddCommandTable( p_irc, GFL_NET_CMD_IRCRHYTHM, sc_recv_tbl, NELEMS(sc_recv_tbl), p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	NET�j��
 *
 *	@param	RHYTHMNET_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMNET_Exit( RHYTHMNET_WORK *p_wk )
{	
	COMPATIBLE_IRC_DelCommandTable( p_wk->p_irc, GFL_NET_CMD_IRCRHYTHM );
	GFL_STD_MemClear( p_wk, sizeof(RHYTHMNET_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���ʏ��𑊎�֑���M
 *
 *	@param	NET_WORK *p_wk	���[�N
 *	@param	NET_RESULT_DATA *cp_data	�f�[�^
 *
 *	@retval	TRUE�Ȃ�΁A���肩���M
 *	@retval	FALSE�Ȃ�Ύ�M�҂�
 */
//-----------------------------------------------------------------------------
static BOOL RHYTHMNET_SendResultData( RHYTHMNET_WORK *p_wk, const RHYTHMSEARCH_WORK *cp_data )
{	
	enum 
	{	
		SEQ_DATA_SEND_INIT,
		SEQ_DATA_SEND_MAIN,
		SEQ_DATA_SEND_WAIT,
	};

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
	if( s_is_debug_only_play )
	{	
		return TRUE;
	}
#endif

	switch( p_wk->seq )
	{	
	case SEQ_DATA_SEND_INIT:
		p_wk->result_send	= *cp_data;
		p_wk->seq = SEQ_DATA_SEND_MAIN;
		NAGI_Printf( "���ʃf�[�^���M�J�n\n" );
		break;

	case SEQ_DATA_SEND_MAIN:
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
					sizeof(RHYTHMSEARCH_WORK), &p_wk->result_send, netID, FALSE, FALSE, TRUE ) )
			{	
				NAGI_Printf( "���ʃf�[�^���M�����A����҂�\n" );
				p_wk->seq	= SEQ_DATA_SEND_WAIT;
			}
		}
		break;

	case SEQ_DATA_SEND_WAIT:
		if( p_wk->is_recv )
		{
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
 *	@param	NET_WORK *p_wk	���[�N
 *	@param	*p_data							�f�[�^�󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMNET_GetResultData( const RHYTHMNET_WORK *cp_wk, RHYTHMSEARCH_WORK *p_data )
{	
	*p_data	= cp_wk->result_recv;
}
//=============================================================================
/**
 *					NETRECV
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
	RHYTHMNET_WORK *p_wk	= p_work;

	if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
	{
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return;	//�����̃f�[�^�͖���
	}

//	GFL_STD_MemCopy( cp_data, &p_wk->result_recv, sizeof(RHYTHMNET_RESULT_DATA) );
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
	RHYTHMNET_WORK *p_wk	= p_work;

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
 *				�ėp
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Ђ��`����Trg�������ǂ���	(W��H�̏ꍇ�̂�)
 *
 *	@param	const GFL_POINT *cp_point	���W
 *	@param	w													��
 *	@param	h													����
 *	@param	*p_trg										���W�󂯎��
 *
 *	@retval	TRUE�^�b�`����
 *	@retval	FALSE�^�b�`���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL TP_GetDiamondTrg( const GFL_POINT *cp_diamond, GFL_POINT *p_trg )
{	
	u32 x, y;
	BOOL ret;

	//Cont���ŁA�H�`���̂Ƃ�
	//	�����蔻��̕��@�́A�x�N�g���̉E���ɓ_������Ƃ��`��ӂ̐������s��
	//
	//
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		//�S���_���쐬
		GFL_POINT	top, left, right, bottom;
		int i;
		int dx, dy;
		u8 ret;
		GFL_POINT	v[5];	//���A��A�E�A���A���̏��ɒ��_

		v[0].x	= cp_diamond->x - TOUCH_DIAMOND_W/2;
		v[0].y	= cp_diamond->y;
		v[1].x	= cp_diamond->x;
		v[1].y	= cp_diamond->y - TOUCH_DIAMOND_H/2;
		v[2].x	= cp_diamond->x + TOUCH_DIAMOND_W/2;
		v[2].y	= cp_diamond->y;
		v[3].x	= cp_diamond->x;
		v[3].y	= cp_diamond->y + TOUCH_DIAMOND_H/2;
		v[4]		= v[0];

		dx	= x;
		dy	= y;

		ret = 0;
		for( i = 0; i < 4; i++ )
		{	
			if( (v[i+1].x - v[i].x)*(dy-v[i].y) - (dx-v[i].x)*(v[i+1].y-v[i].y) > 0 )
			{	
				ret++;
			}
		}

		if( ret == 4)
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
 *	@brief	�X�R�A�v�Z
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	���[�N
 *
 *	@return	�X�R�A
 */
//-----------------------------------------------------------------------------
static u8	CalcScore( const RHYTHM_MAIN_WORK *cp_wk )
{	
	RHYTHMSEARCH_WORK	my;
	RHYTHMSEARCH_WORK	you;

	u32	prog_score;
	u32	diff_score;

	my	= cp_wk->search;
	RHYTHMNET_GetResultData( &cp_wk->net, &you );

	OS_Printf( "�����ʂ̕\��\n" );
	//�o�ߎ��Ԃ̔���
	{	
		s32 player1;
		s32 player2;
		u32	prog;
		player1	= my.data[my.data_idx-1].prog_ms - my.data[0].prog_ms;
		player2	= you.data[you.data_idx-1].prog_ms - you.data[0].prog_ms;
		prog	= MATH_IAbs(player1 - player2);
		if( prog  == RANGE_RHYTHMSCORE_PROC_00 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_00;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_01 )
		{	
			prog_score	= VAL_RHYTHMSCORE_PROC_01;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_02 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_02;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_03 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_03;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_04 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_04;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_05 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_05;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_06 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_06;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_07 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_07;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_08 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_08;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_09 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_09;
		}
		else
		{
#if VAL_RHYTHMSCORE_PROC_10
			prog_score	= VAL_RHYTHMSCORE_PROC_10;
#endif 
		}
		OS_Printf( "�o�ߎ��� %d�_", prog_score );
	}

	//���Ԃ̍��̔���
	{	
		int i;
		s32 player1;
		s32 player2;
		u32	prog;
		diff_score	= 0;
		for( i = 1; i < 10; i++ )
		{	
			player1	= my.data[i].diff_ms;
			player2	= you.data[i].diff_ms;
			prog	= MATH_IAbs(player1 - player2);

			if( prog  == RANGE_RHYTHMSCORE_DIFF_00 )
			{	
				diff_score	+= VAL_RHYTHMSCORE_DIFF_00;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_01 )
			{	
				diff_score	+= VAL_RHYTHMSCORE_DIFF_01;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_02 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_02;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_03 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_03;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_04 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_04;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_05 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_05;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_06 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_06;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_07 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_07;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_08 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_08;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_09 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_09;
			}
			else
			{
#if VAL_RHYTHMSCORE_DIFF_10
				diff_score	+= VAL_RHYTHMSCORE_DIFF_10;
#endif
			}
		}
		OS_Printf( "�Ԋu %d�_\n", diff_score/9 );
	}
	return (prog_score + (diff_score/9) )/2;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�t�F�[�h
 *
 *	@param	NNS_GFD_DST_TYPE type �]����
 *	@param	*p_buff               �o�b�t�@
 *	@param	cnt                   �J�E���g
 *	@param	plt_num               �p���b�g�c
 *	@param	plt_col               �p���b�g��
 *	@param	start                 �J�n�F
 *	@param	end                   �I���F
 */
//-----------------------------------------------------------------------------
static void RHYTHM_MainPltAnm( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{ 
  //0�`�P�ɕϊ�
  const fx16 cos = (FX_CosIdx(cnt)+FX16_ONE)/2;
  const u8 start_r  = (start & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
  const u8 start_g  = (start & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
  const u8 start_b  = (start & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
  const u8 end_r  = (end & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
  const u8 end_g  = (end & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
  const u8 end_b  = (end & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

  const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
  const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
  const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);

  *p_buff = GX_RGB(r, g, b);

  NNS_GfdRegisterNewVramTransferTask( type,
      plt_num * 32 + plt_col * 2,
      p_buff, 2 );
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
 *	@param	clwk_ofs							CLWK�I�t�Z�b�g���Ԃ���g���n�߂邩
 */
//-----------------------------------------------------------------------------
static void BACKOBJ_Init( BACKOBJ_WORK *p_wk, const GRAPHIC_WORK *cp_grp, BACKOBJ_MOVE_TYPE type, u32 clwk_ofs, int sf_type )
{	
	GFL_STD_MemClear( p_wk, sizeof(BACKOBJ_WORK) );
	p_wk->type	= type;
	p_wk->sync_now	= 0;
	p_wk->sync_max	= BACKOBJ_MOVE_SYNC;
	{	
		int i;
		GFL_CLWK *p_clwk;
		for( i = 0; i < BACKOBJ_CLWK_MAX; i++ )
		{	
			p_clwk	= GRAPHIC_GetClwk( cp_grp, clwk_ofs + i );
			BACKOBJ_ONE_Init( &p_wk->wk[i], p_clwk, sf_type );
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
					BACKOBJ_COLOR	color;

					start.x	= GFUser_GetPublicRand(256);
					start.y	= -36;
					end.x		= start.x;
					end.y		= 192 + 36;
					sync	= BACKOBJ_ONE_MOVE_SYNC_MIN + GFUser_GetPublicRand(BACKOBJ_ONE_MOVE_SYNC_DIF);

					//�F�͐����F
					if( GFUser_GetPublicRand(2) )
					{	
						color	= BACKOBJ_COLOR_WATER;
					}
					else
					{	
						color	= BACKOBJ_COLOR_YELLOW;
					}
					
					if( up_or_down )
					{	
						BACKOBJ_ONE_Start( &p_wk->wk[i], &end, &start, color, sync );
					}
					else
					{	
						BACKOBJ_ONE_Start( &p_wk->wk[i], &start, &end, color, sync );
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
					BACKOBJ_COLOR color;
					start.x	= (FX_SinIdx(rot) * 256) >> FX32_SHIFT;
					start.y	= (FX_CosIdx(rot) * 256) >> FX32_SHIFT;
					end.x		= 128;
					end.y		= 96;
					sync	= BACKOBJ_ONE_MOVE_SYNC_MIN + GFUser_GetPublicRand(BACKOBJ_ONE_MOVE_SYNC_DIF);


					//�F�͐����F
					if( GFUser_GetPublicRand(2) )
					{	
						color	= BACKOBJ_COLOR_WATER;
					}
					else
					{	
						color	= BACKOBJ_COLOR_YELLOW;
					}

					BACKOBJ_ONE_Start( &p_wk->wk[i], &start, &end, color, sync );
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
static void BACKOBJ_StartEmit( BACKOBJ_WORK *p_wk, const GFL_POINT *cp_pos, BACKOBJ_COLOR color )
{	
	int i, j;
	u32 sync;
	u16 rot;
	GFL_POINT	end;

	VecFx32	start_v;
	VecFx32	end_v;
	VecFx32	v;


	start_v.x	= cp_pos->x << FX32_SHIFT;
	start_v.y	= cp_pos->y << FX32_SHIFT;
	start_v.z	= 0;

	for( j = 0; j < BACKOBJ_EMIT_NUM; j++ )
	{	
		for( i = 0; i < BACKOBJ_CLWK_MAX; i++ )
		{	
			if( !BACKOBJ_ONE_IsMove( &p_wk->wk[i] ) )
			{
				rot	= GFUser_GetPublicRand(0xFFFF);
				end.x	= (FX_SinIdx(rot) * 256) >> FX32_SHIFT;
				end.y	= (FX_CosIdx(rot) * 256) >> FX32_SHIFT;

				end_v.x	= end.x << FX32_SHIFT;
				end_v.y	= end.y << FX32_SHIFT;
				end_v.z	= 0;

				VEC_Subtract( &end_v, &start_v, &v );
				VEC_Normalize( &v, &v );

				end.x	= (v.x * 256) >> FX32_SHIFT;
				end.y	= (v.y * 256) >> FX32_SHIFT;

				end.x += cp_pos->x;
				end.y	+= cp_pos->y;

				sync	= BACKOBJ_ONE_EMIT_SYNC_MIN + GFUser_GetPublicRand(BACKOBJ_ONE_EMIT_SYNC_DIF);
				BACKOBJ_ONE_Start( &p_wk->wk[i], cp_pos, &end, color, sync );
				break;
			}
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
 */
//-----------------------------------------------------------------------------
static void BACKOBJ_ONE_Init( BACKOBJ_ONE *p_wk, GFL_CLWK *p_clwk,int sf_type )
{	
	GFL_STD_MemClear( p_wk, sizeof(BACKOBJ_ONE) );
	p_wk->p_clwk	= p_clwk;
	p_wk->sf_type	= sf_type;

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

//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�t�F�[�h  �쐬
 *
 *	@param	PLTFADE_WORK *p_wk  ���[�N
 *	@param	heapID              �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PLTFADE_Init( PLTFADE_WORK *p_wk, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(PLTFADE_WORK) );
  { 
/*    int i;
    for( i = 0; i < PLTFADE_MAX; i++ )
    {
      p_wk->plt_start[0]  = TRUE;
    }
 */   p_wk->plt_start[0]  = TRUE;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�t�F�[�h  �I��
 *
 *	@param	PLTFADE_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PLTFADE_Exit( PLTFADE_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_wk, sizeof(PLTFADE_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�t�F�[�h  ���C������
 *
 *	@param	PLTFADE_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PLTFADE_Main( PLTFADE_WORK *p_wk )
{ 
  if( p_wk->is_start )
  { 
    static const int add = 0x400;
    int i;

    for( i = 0; i < PLTFADE_MAX; i++ )
    {
      if( p_wk->plt_start[i] )
      { 
        BOOL is_end;
/*
        if( p_wk->plt_flip[i] )
        { 
          is_end  = p_wk->plt_cnt[i] < 0x10000/2;
        }
        else
        { 
          is_end  = p_wk->plt_cnt[i] >= 0x10000/2;
        }
*/
//        is_end  = p_wk->plt_cnt[i] + add >= 0x10000;

  /*      if( is_end )
        { 
          p_wk->plt_start[i]  = FALSE;
          p_wk->plt_flip[i] ^= TRUE;
          p_wk->plt_start[  (i+1) % PLTFADE_MAX  ]  = TRUE;
        }
*/
        if( p_wk->plt_cnt[i] + add >= 0x10000 )
        {
          p_wk->plt_cnt[i] = p_wk->plt_cnt[i]+add-0x10000;
        }
        else
        {
          p_wk->plt_cnt[i] += add;
        }
        RHYTHM_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_MAIN, &p_wk->plt_buff[i], p_wk->plt_cnt[i], 
            5, 3+i, GX_RGB( 31,22,26 ), GX_RGB( 30,15,21 ) );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �p���b�g�t�F�[�h  �J�n
 *
 *	@param	PLTFADE_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PLTFADE_Start( PLTFADE_WORK *p_wk )
{ 
  p_wk->is_start  = TRUE;
}

#ifdef PM_DEBUG
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
#endif
