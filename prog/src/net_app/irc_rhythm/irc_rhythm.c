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

//	module
#include "infowin/infowin.h"

#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

//	archive
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_irc_rhythm.h"
#include "ircrhythm_gra.naix"

//	rhythm
#include "net_app/irc_rhythm.h"
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
//#define DEBUG_RHYTHM_MSG	//�f�o�b�O���b�Z�[�W���o��
//#define DEBUG_ONLY_PLAY	//�f�o�b�O�p���Ńv���C���郂�[�h���o��
#endif //PM_DEBUG


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
	RHYTHM_BG_PAL_M_00 = 0,//�w�i�pBG
	RHYTHM_BG_PAL_M_01,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_02,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_03,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_04,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_05,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_06,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_07,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_08,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_09,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_10,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_11,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_12,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_13,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_M_14,		// �t�H���g
	RHYTHM_BG_PAL_M_15,		// INFOWIN


	// �T�u���BG
	RHYTHM_BG_PAL_S_00 = 0,	//�g�p���Ă��Ȃ�
	RHYTHM_BG_PAL_S_01,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_02,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_03,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_04,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_05,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_06,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_07,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_08,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_09,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_10,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_11,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_12,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_13,		// �g�p���ĂȂ�
	RHYTHM_BG_PAL_S_14,		//	�t�H���g
	RHYTHM_BG_PAL_S_15,		// �g�p���ĂȂ�

	// ���C�����OBJ
	RHYTHM_OBJ_PAL_M_00 = 0,// �^�b�`����OBJ
	RHYTHM_OBJ_PAL_M_01,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_02,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_03,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_04,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_05,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_06,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_07,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_08,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_09,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_10,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_11,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_12,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_13,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_14,		// �g�p���ĂȂ�
	RHYTHM_OBJ_PAL_M_15,		// �g�p���ĂȂ�


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
#define INFOWIN_PLT_NO		(RHYTHM_BG_PAL_M_15)
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

#define	MSGWND_RETURN_X	(1)
#define	MSGWND_RETURN_Y	(22)
#define	MSGWND_RETURN_W	(30)
#define	MSGWND_RETURN_H	(2)

//-------------------------------------
///	�J�E���g
//=====================================
#define TOUCHMARKER_VISIBLE_CNT	(10)

//-------------------------------------
///	�v�����
//=====================================
#define RHYTHMSEARCH_DATA_MAX	(15)	//�v���񐔍ő�
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
///	MSGWNDID
//=====================================
typedef enum {
	MSGWNDID_TEXT,
	MSGWNDID_RETURN,

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
	int dummy;
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
} MSGWND_WORK;

//-------------------------------------
///	�T�[�`��
//=====================================
typedef struct
{
	//�ǂ�����������
	GFL_POINT		pos;
	//����������
	u32					prog_ms;	//�o�ߎ���
	u32					diff_ms;	//���̎���
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
///	�l���щ��
//=====================================
typedef struct 
{
	u32	seq;
	const MSG_WORK *cp_msg;
	const RHYTHMSEARCH_WORK *cp_search;
	MSGWND_WORK			msgwnd[ROR_MSGWNDID_MAX];
} RHYTHM_ONLYRESULT_WORK;

#ifdef DEBUG_RHYTHM_MSG
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
#endif //DEBUG_RHYTHM_MSG

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
	RHYTHM_ONLYRESULT_WORK	onlyresult;

	//�V�[�P���X�Ǘ�
	SEQ_FUNCTION		seq_function;
	u16		seq;
	BOOL is_end;
	BOOL is_next_proc;

	//�v��
	RHYTHMSEARCH_WORK	search;

	//���̑�
	u32								marker_cnt;	//�}�[�J�[�\������

	//����
	IRC_RHYTHM_PARAM	*p_param;

#ifdef DEBUG_ONLY_PLAY
	//�f�o�b�O�p
	RHYTHMSEARCH_WORK	search2;
	u16							debug_player;			//���������肩
	u16							debug_game_cnt;	//���Q�[���ڂ�
#endif //DEBUG_ONLY_PLAY
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
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, const MSG_WORK *cp_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y );
static void MSGWND_PrintCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID );
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y );
static void MSGWND_Clear( MSGWND_WORK* p_wk );
//ONLYRESULT
static void RHYTHM_ONLYRESULT_Init( RHYTHM_ONLYRESULT_WORK* p_wk, u8 frm, const MSG_WORK *cp_msg, const RHYTHMSEARCH_WORK *cp_search,  HEAPID heapID );
static void RHYTHM_ONLYRESULT_Exit( RHYTHM_ONLYRESULT_WORK* p_wk );
static BOOL RHYTHM_ONLYRESULT_Main( RHYTHM_ONLYRESULT_WORK* p_wk );
//SEQ
static void SEQ_Change( RHYTHM_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function );
static void SEQ_End( RHYTHM_MAIN_WORK *p_wk );
//SEQ_FUNC
static void SEQFUNC_StartGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_MainGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_Result( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
//RHYTHMSEARCH
static void RHYTHMSEARCH_Init( RHYTHMSEARCH_WORK *p_wk );
static void RHYTHMSEARCH_Exit( RHYTHMSEARCH_WORK *p_wk );
static void RHYTHMSEARCH_Start( RHYTHMSEARCH_WORK *p_wk );
static BOOL RHYTHMSEARCH_IsEnd( const RHYTHMSEARCH_WORK *cp_wk );
static void RHYTHMSEARCH_SetData( RHYTHMSEARCH_WORK *p_wk, const GFL_POINT *cp_pos );
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
static void TouchMarker_SetPos( RHYTHM_MAIN_WORK *p_wk, const GFL_POINT *cp_pos );
static void TouchMarker_Main( RHYTHM_MAIN_WORK *p_wk );
static u8		CalcScore( const RHYTHM_MAIN_WORK *cp_wk );
static BOOL TouchReturnBtn( void );

#ifdef DEBUG_RHYTHM_MSG
static void DEBUGRHYTHM_PRINT_UpDate( RHYTHM_MAIN_WORK *p_wk );
#else
#define DEBUGRHYTHM_PRINT_UpDate(...)	((void)0)
#endif

//DEBUG_PRINT
#ifdef DEBUG_RHYTHM_MSG
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
#endif //DEBUG_RHYTHM_MSG
static STRBUF * DEBUGPRINT_CreateWideChar( const u16 *cp_str, HEAPID heapID );
static STRBUF * DEBUGPRINT_CreateWideCharNumber( const u16 *cp_str, int number, HEAPID heapID );

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
	GRAPHIC_BG_FRAME_M_TEXT	= GRAPHIC_BG_FRAME_M_INFOWIN,
	GRAPHIC_BG_FRAME_M_BACK,
	GRAPHIC_BG_FRAME_S_TEXT,
	GRAPHIC_BG_FRAME_S_BACK,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	INFOWIN_BG_FRAME, GFL_BG_FRAME1_M, GFL_BG_FRAME0_S, GFL_BG_FRAME1_S,
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
	// GRAPHIC_BG_FRAME_M_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
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

	//���W���[��������
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCRHYTHM );
	MSG_Init( &p_wk->msg, MSG_FONT_TYPE_LARGE, HEAPID_IRCRHYTHM );
	INFOWIN_Init( INFOWIN_BG_FRAME, INFOWIN_PLT_NO, GAMESYSTEM_GetGameCommSysPtr(p_wk->p_param->p_gamesys), HEAPID_IRCRHYTHM );
	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_TEXT], sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TEXT],
			MSGWND_TEXT_X, MSGWND_TEXT_Y, MSGWND_TEXT_W, MSGWND_TEXT_H, HEAPID_IRCRHYTHM );
	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_RETURN], sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT],
			MSGWND_RETURN_X, MSGWND_RETURN_Y, MSGWND_RETURN_W, MSGWND_RETURN_H, HEAPID_IRCRHYTHM );
	MSGWND_PrintCenter( &p_wk->msgwnd[MSGWNDID_RETURN], &p_wk->msg, RHYTHM_BTN_000 );

	RHYTHMNET_Init( &p_wk->net, p_wk->p_param->p_irc );

	RHYTHMSEARCH_Init( &p_wk->search );

	//�f�o�b�O
	DEBUGPRINT_Init( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], FALSE, HEAPID_IRCRHYTHM );
	DEBUGPRINT_Open();


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

	//�f�o�b�O�j��
	DEBUGPRINT_Close();
	DEBUGPRINT_Exit();

	RHYTHMNET_Exit( &p_wk->net );

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

#if 0
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
		{	
			SEQ_Change( p_wk, SEQFUNC_StartGame );
		}
#endif

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

	INFOWIN_Update();
	if( MSG_Main( &p_wk->msg ) )
	{
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Main( &p_wk->msgwnd[i], &p_wk->msg );
		}
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

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCRHYTHM_GRAPHIC, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircrhythm_gra_rhythm_bg_back_NCLR,
				PALTYPE_MAIN_BG, PALTYPE_MAIN_BG*0x20, 0x20, heapID );
	
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_ircrhythm_gra_rhythm_bg_back_NCGR,
				sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_ircrhythm_gra_rhythm_bg_back_NSCR,
				sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );

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

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCRHYTHM_GRAPHIC, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_PLT]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_ircrhythm_gra_rhythm_obj_touch_NCLR, CLSYS_DRAW_MAIN, RHYTHM_OBJ_PAL_M_00*0x20, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_ircrhythm_gra_rhythm_obj_touch_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_ircrhythm_gra_rhythm_obj_touch_NCER, NARC_ircrhythm_gra_rhythm_obj_touch_NANR, heapID );

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


		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CLWKID_TOUCH], FALSE );
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
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_rhythm_dat, heapID );

	p_wk->p_wordset	= WORDSET_Create( heapID );

	{	
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], GX_RGB(31,31,31) );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_TEXT], GX_RGB(31,31,31) );
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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//������쐬
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//�Z���^�[�ʒu�v�Z
	x	= GFL_BMPWIN_GetSizeX( p_wk->p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_wk->p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_wk->p_strbuf, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_wk->p_strbuf, p_font )/2;

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
 *				�l���ʉ��
 */
//=============================================================================
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
					NARC_font_small_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );		
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
	RHYTHMSEARCH_WORK	*p_search;
	p_search	= &p_wk->search;

	MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_STR_000, 0, 0 );


#ifdef DEBUG_ONLY_PLAY
	DEBUGRHYTHM_PRINT_UpDate( p_wk );
	if( p_wk->debug_player == 0 )
	{
		p_search	= &p_wk->search;
	}
	else
	{	
		p_search	= &p_wk->search2;
		MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_DEBUG_001, 0, 0 );
	}
#endif // DEBUG_ONLY_PLAY


	RHYTHMSEARCH_Start( p_search );
	SEQ_Change( p_wk, SEQFUNC_MainGame );

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
	RHYTHMSEARCH_WORK	*p_search;
	p_search	= &p_wk->search;

#ifdef DEBUG_ONLY_PLAY
	if( p_wk->debug_player == 0 )
	{	
		p_search	= &p_wk->search;
	}
	else
	{	
		p_search	= &p_wk->search2;
	}
#endif //DEBUG_ONLY_PLAY


	TouchMarker_Main( p_wk );
	for( i = 0; i< NELEMS(sc_diamond_pos); i++ )
	{
		if( TP_GetDiamondTrg( &sc_diamond_pos[i], NULL ) )
		{	
			TouchMarker_SetPos( p_wk, &sc_diamond_pos[i] );
			RHYTHMSEARCH_SetData( p_search, &sc_diamond_pos[i] );
			break;
		}
	}

	//�v���I���`�F�b�N
	if( RHYTHMSEARCH_IsEnd( p_search ) )
	{	
		MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_STR_001, 0, 0 );
		SEQ_Change( p_wk, SEQFUNC_Result );

#ifdef DEBUG_ONLY_PLAY
		DEBUGRHYTHM_PRINT_UpDate( p_wk );
		if( p_wk->debug_player == 0 )
		{	
			p_wk->debug_player	= 1;
			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_DEBUG_001, 0, 0 );
			SEQ_Change( p_wk, SEQFUNC_StartGame );
		}
		else
		{	
			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_DEBUG_002, 0, 0 );
		SEQ_Change( p_wk, SEQFUNC_Result );
		}
#endif //DEBUG_ONLY_PLAY
	}


	if( TouchReturnBtn() )
	{
		p_wk->p_param->result	= IRCRHYTHM_RESULT_RETURN;
		SEQ_End( p_wk );
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
#ifdef DEBUG_ONLY_PLAY
	if(	GFL_UI_KEY_GetTrg() & PAD_BUTTON_A	)
	{	

		RHYTHMSEARCH_Init( &p_wk->search );
		SEQ_Change( p_wk, SEQFUNC_StartGame );

		RHYTHMSEARCH_Init( &p_wk->search2 );
		p_wk->debug_player		= 0;

	}
#else
	enum
	{	
		SEQ_SENDRESULT,
		SEQ_CALC,
		SEQ_END,
#if 0
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_ONLYRESULT_INIT,
		SEQ_ONLYRESULT_MAIN,
		SEQ_ONLYRESULT_EXIT,
		SEQ_NEXTPROC,
#endif
	};

	switch( *p_seq )
	{	
	case SEQ_SENDRESULT:
		if( RHYTHMNET_SendResultData( &p_wk->net, &p_wk->search ) )
		{	
			*p_seq	= SEQ_CALC;
		}
		break;

	case SEQ_CALC:
		p_wk->p_param->score	= CalcScore( p_wk );
		p_wk->p_param->result	= IRCRHYTHM_RESULT_CLEAR;
	//	*p_seq	= SEQ_FADEIN_START;
		*p_seq	= SEQ_END;
		break;

	case SEQ_END:
		SEQ_End( p_wk );
		break;
#if 0
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_ONLYRESULT_INIT;
		}
		break;

	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_ONLYRESULT_MAIN;
		}
		break;

	case SEQ_ONLYRESULT_INIT:
		//��ʏ�̂��̂�����
		GFL_BG_SetVisible( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], FALSE );
		MSGWND_Clear( &p_wk->msgwnd[MSGWNDID_TEXT] );
		MSGWND_Clear( &p_wk->msgwnd[MSGWNDID_RETURN] );

		//�V���Ɍ��ʉ�ʍ쐬
		RHYTHM_ONLYRESULT_Init( &p_wk->onlyresult, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT], &p_wk->msg, &p_wk->search, HEAPID_IRCRHYTHM );

		*p_seq	= SEQ_FADEOUT_START;
		break;

	case SEQ_ONLYRESULT_MAIN:
		if( RHYTHM_ONLYRESULT_Main( &p_wk->onlyresult ) )
		{	
			*p_seq	= SEQ_ONLYRESULT_EXIT;
		}
		break;

	case SEQ_ONLYRESULT_EXIT:
		RHYTHM_ONLYRESULT_Exit( &p_wk->onlyresult );
		*p_seq	= SEQ_NEXTPROC;
		break;

	case SEQ_NEXTPROC:
		p_wk->is_next_proc	= TRUE;
		SEQ_End( p_wk );
		break;
#endif
	}
#endif //DEBUG_ONLY_PLAY
	TouchMarker_Main( p_wk );

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

	//�I�������P
	//�^�b�`�񐔂�10��ȏ�ŁA���^�b�`��Ԃ��P�b�ȏ�
	if( cp_wk->data_idx >= 10 )
	{	
		if( OS_TicksToMilliSeconds(OS_GetTick()) - cp_wk->start_time 
			  - cp_wk->data[cp_wk->data_idx-1].prog_ms >= 1000 ) 
		{	
			return TRUE;
		}
	}

	//�I�������Q
	//�^�b�`�񐔂�15��
	if( cp_wk->data_idx == RHYTHMSEARCH_DATA_MAX )
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
	GF_ASSERT( p_wk->data_idx < RHYTHMSEARCH_DATA_MAX );

	p_wk->data[ p_wk->data_idx ].pos						= *cp_pos;
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
	switch( p_wk->seq )
	{	
	case 0:
		p_wk->result_send	= *cp_data;
		p_wk->seq = 1;
		NAGI_Printf( "���ʃf�[�^���M�J�n\n" );
		break;

	case 1:
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
				p_wk->seq	= 2;
			}
		}
		break;

	case 2:
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
 *	@brief	�^�b�`�}�[�J�[�̕\����OFF
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void TouchMarker_Main( RHYTHM_MAIN_WORK *p_wk )
{	
	GFL_CLWK	*p_marker;
	p_marker	= GRAPHIC_GetClwk( &p_wk->grp, CLWKID_TOUCH );

	if( GFL_CLACT_WK_GetDrawEnable( p_marker ) )
	{	
		if( p_wk->marker_cnt++ > TOUCHMARKER_VISIBLE_CNT )
		{	
			GFL_CLACT_WK_SetDrawEnable( p_marker, FALSE );
		}
	}
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
 *	@brief	�߂�{�^�������������ǂ���
 *
 *	@param	void 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL TouchReturnBtn( void )
{	

	u32 x;
	u32 y;
	if( GFL_UI_TP_GetPointTrg( &x, &y) )
	{	
		if( ((u32)( x - MSGWND_RETURN_X*8) < (u32)(MSGWND_RETURN_W*8))
				&	((u32)( y - MSGWND_RETURN_Y*8) < (u32)(MSGWND_RETURN_H*8))
			){
			return TRUE;
		}

	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�b�`�}�[�J�[�̍��W��ݒ肵�\��
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	���[�N
 *	@param	GFL_POINT *cp_pos				���W
 *
 */
//-----------------------------------------------------------------------------
static void TouchMarker_SetPos( RHYTHM_MAIN_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	GFL_CLACTPOS clpos;
	GFL_CLWK	*p_marker;
	p_marker	= GRAPHIC_GetClwk( &p_wk->grp, CLWKID_TOUCH );
	clpos.x	= cp_pos->x+1;
	clpos.y	= cp_pos->y+1;
	GFL_CLACT_WK_SetPos( p_marker, &clpos, 0 );
	GFL_CLACT_WK_SetDrawEnable( p_marker, TRUE );
	p_wk->marker_cnt	= 0;
}

#ifdef DEBUG_RHYTHM_MSG
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�v�����g���X�V
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void DEBUGRHYTHM_PRINT_UpDate( RHYTHM_MAIN_WORK *p_wk )
{
	enum{	
		DEBUGMSG_TAB	= 90,
		DEBUGMSG_TAB2	= 170,
	};

	int i,j;

	RHYTHMSEARCH_WORK	*p_search;
	p_search	= &p_wk->search;

	DEBUGPRINT_Clear();

	OS_Printf( "�����Y���`�F�b�N�\���X�^�[�g����������\n" );
	for( j = 0; j < 2; j++ )
	{	
		if( j == 0 )
		{	
			p_search	= &p_wk->search;
		}
		else
		{	
			p_search	= &p_wk->search2;
		}

		OS_Printf( "��%d�l�ڂ̕\��\n", j );
		{	
			u32 ret;

			DEBUGPRINT_Print( L"�^�b�`�J�n����I���܂�", 0,  0 );
			ret	= p_search->data[p_search->data_idx-1].prog_ms - p_search->data[0].prog_ms;
			DEBUGPRINT_PrintNumber( L"�~���b %d", ret, DEBUGMSG_TAB*j,  10 );
			DEBUGPRINT_PrintNumber( L"�� %d", p_search->data_idx, 45+DEBUGMSG_TAB*j, 10 );
			OS_Printf( "�p������ %d, �^�b�`�� %d\n", ret, p_search->data_idx );
		}

		{	
			DEBUGPRINT_Print( L"�^�b�`�Ԋu", 0,  30 );
			for( i = 0; i < p_search->data_idx; i++ )
			{	
				if( i == 10 )
				{	
					break;
				}
				DEBUGPRINT_PrintNumber( L"�Ԋu[%d]", i, DEBUGMSG_TAB*j,  40+i*10 );
				DEBUGPRINT_PrintNumber( L"%d", p_search->data[i].diff_ms, 30+DEBUGMSG_TAB*j,  40+i*10 );
				OS_Printf( "�Ԋu[%d] %d\n", i, p_search->data[i].diff_ms );
			}
		}
	}

	OS_Printf( "�����̕\��\n" );
	//��
	{	
		s32 player1;
		s32 player2;
		player1	= p_wk->search.data[p_wk->search.data_idx-1].prog_ms - p_wk->search.data[0].prog_ms;
		player2	= p_wk->search2.data[p_wk->search2.data_idx-1].prog_ms - p_wk->search2.data[0].prog_ms;
		DEBUGPRINT_PrintNumber( L"�o�߂̍� %d", MATH_IAbs(player1 - player2), DEBUGMSG_TAB2,  10 );
		OS_Printf( "�o�ߎ��Ԃ̍� %d\n", MATH_IAbs(player1 - player2) );


		for( i = 0; i < p_search->data_idx; i++ )
		{	
			if( i == 10 )
			{	
				break;
			}
			DEBUGPRINT_PrintNumber( L"��[%d]", i, DEBUGMSG_TAB2,  40+i*10 );
			player1	= p_wk->search.data[i].diff_ms;
			player2	= p_wk->search2.data[i].diff_ms;
			DEBUGPRINT_PrintNumber( L"%d", MATH_IAbs(player1-player2), 30+DEBUGMSG_TAB2,  40+i*10 );
			OS_Printf( "�Ԋu�̍�[%d] %d\n", i, MATH_IAbs(player1 - player2) );
		}
	}

	OS_Printf( "�����ʂ̕\��\n" );
	//����
	{	
		s32 player1;
		s32 player2;
		u32	prog;
		u32	score;
		player1	= p_wk->search.data[p_wk->search.data_idx-1].prog_ms - p_wk->search.data[0].prog_ms;
		player2	= p_wk->search2.data[p_wk->search2.data_idx-1].prog_ms - p_wk->search2.data[0].prog_ms;
		prog	= MATH_IAbs(player1 - player2);
		if( prog  == 0 )
		{	
			score	= 100;
		}
		else if( prog  < 500 )
		{	
			score	= 90;
		}
		else if( prog  < 1000 )
		{
			score	= 80;
		}
		else if( prog  < 3000 )
		{
			score	= 70;
		}
		else if( prog  < 5000 )
		{
			score	= 60;
		}
		else if( prog  < 10000 )
		{
			score	= 50;
		}
		else if( prog  < 15000 )
		{
			score	= 40;
		}
		else if( prog  < 20000 )
		{
			score	= 30;
		}
		else if( prog  < 25000 )
		{
			score	= 20;
		}
		else if( prog  < 30000 )
		{
			score	= 10;
		}
		else
		{
			score	= 0;
		}

		OS_Printf( "�o�ߎ��� �_%d\n", score );
		score	= 0;
		for( i = 1; i < 10; i++ )
		{	
			player1	= p_wk->search.data[i].diff_ms;
			player2	= p_wk->search2.data[i].diff_ms;
			prog	= MATH_IAbs(player1 - player2);

			if( prog  == RANGE_RHYTHMSCORE_DIFF_00 )
			{	
				score	+= VAL_RHYTHMSCORE_DIFF_00;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_01 )
			{	
				score	+= VAL_RHYTHMSCORE_DIFF_01;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_02 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_02;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_03 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_03;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_04 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_04;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_05 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_05;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_06 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_06;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_07 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_07;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_08 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_08;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_09 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_09;
			}
			else
			{
#if VAL_RHYTHMSCORE_DIFF_10
				score	+= VAL_RHYTHMSCORE_DIFF_10;
#endif
			}
		}
		OS_Printf( "�Ԋu �_%d\n", score/9 );
		
	}
	
	OS_Printf( "�����Y���`�F�b�N�\���I������������\n" );

}
#endif //DEBUG_RHYTHM_MSG
 
#ifdef DEBUG_RHYTHM_MSG
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
#endif //DEBUG_RHYTHM_MSG

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
//#endif //DEBUG_RHYTHM_MSG 
