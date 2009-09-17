//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file			config_panel2.c
 *	@brief		�R���t�B�O���
 *	@author		Toru=Nagihashi
 *	@data			2009.09.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//library
#include <gflib.h>

//system
#include "system/gfl_use.h"
#include "system/main.h"	//HEAPID
#include "system/wipe.h"

//module
#include "app/app_menu_common.h"

//archive
#include "arc_def.h"
#include "config_gra.naix"

//print
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_config.h"
#include "print/gf_font.h"
#include "print/printsys.h"

//mine
#include "app/config_panel.h"
#include "savedata/config.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	// ���C�����BG
	CONFIG_BG_PAL_M_00 = 0,// BG�p�p���b�g�擪
	CONFIG_BG_PAL_M_01,		// 
	CONFIG_BG_PAL_M_02,		//
	CONFIG_BG_PAL_M_03,		// 
	CONFIG_BG_PAL_M_04,		// 
	CONFIG_BG_PAL_M_05,		// 
	CONFIG_BG_PAL_M_06,		// 
	CONFIG_BG_PAL_M_07,		// 
	CONFIG_BG_PAL_M_08,		//  
	CONFIG_BG_PAL_M_09,		//  
	CONFIG_BG_PAL_M_10,		//  
	CONFIG_BG_PAL_M_11,		//  
	CONFIG_BG_PAL_M_12,		//  
	CONFIG_BG_PAL_M_13,		//  
	CONFIG_BG_PAL_M_14,		// 
	CONFIG_BG_PAL_M_15,		//	�^�b�`�o�[


	// �T�u���BG
	CONFIG_BG_PAL_S_00 = 0,//
	CONFIG_BG_PAL_S_01,		// 
	CONFIG_BG_PAL_S_02,		// 
	CONFIG_BG_PAL_S_03,		// 
	CONFIG_BG_PAL_S_04,		// 
	CONFIG_BG_PAL_S_05,		// 
	CONFIG_BG_PAL_S_06,		// 
	CONFIG_BG_PAL_S_07,		//  
	CONFIG_BG_PAL_S_08,		//  
	CONFIG_BG_PAL_S_09,		//  
	CONFIG_BG_PAL_S_10,		//  
	CONFIG_BG_PAL_S_11,		//  
	CONFIG_BG_PAL_S_12,		//  
	CONFIG_BG_PAL_S_13,		// 
	CONFIG_BG_PAL_S_14,		//  
	CONFIG_BG_PAL_S_15,		// 
};

//-------------------------------------
///	OBJ���\�[�X�o�^ID
//=====================================
enum 
{
	OBJRESID_TOUCH_PLT,
	OBJRESID_TOUCH_CHR,
	OBJRESID_TOUCH_CEL,

	OBJRESID_MAX
};
//-------------------------------------
///	CLWK��ID
//=====================================
typedef enum
{	
	CLWKID_DECIDE,
	CLWKID_CANCEL,
	CLWKID_CHECK,
	
	CLWKID_MAX
}CLWKID;
//-------------------------------------
///	BMPWINID
//=====================================
typedef enum
{	
	BMPWINID_MSGSPEED,
	BMPWINID_MSGSPEED_SLOW,
	BMPWINID_MSGSPEED_NORMAL,
	BMPWINID_MSGSPEED_FAST,
	BMPWINID_BTLEFF,
	BMPWINID_BTLEFF_ON,
	BMPWINID_BTLEFF_OFF,
	BMPWINID_BTLRULE,
	BMPWINID_BTLRULE_IREKAE,
	BMPWINID_BTLRULE_KACHINUKI,
	BMPWINID_SND,
	BMPWINID_SND_STEREO,
	BMPWINID_SND_MONO,
	BMPWINID_STR,
	BMPWINID_STR_HERAGANA,
	BMPWINID_STR_KANJI,
	BMPWINID_WIRELESS,
	BMPWINID_WIRELESS_ON,
	BMPWINID_WIRELESS_OFF,
	BMPWINID_REPORT,
	BMPWINID_REPORT_WRITE,
	BMPWINID_REPORT_NONE,
	BMPWINID_CONFIG_MENU_MAX,
	BMPWINID_TITLE	= BMPWINID_CONFIG_MENU_MAX,
	BMPWINID_DECIDE,
	BMPWINID_CANCEL,
	
	BMPWINID_MAX
}BMPWINID;
//-------------------------------------
///	UI�̑I�����Ă������
//=====================================
typedef enum 
{
	UI_SELECT_MSGSPEED,
	UI_SELECT_BTLEFF,
	UI_SELECT_BTLRULE,
	UI_SELECT_SND,
	UI_SELECT_STR,
	UI_SELECT_WIRELESS,
	UI_SELECT_REPORT,
	UI_SELECT_MAX
} UI_SELECT;

//-------------------------------------
///	BG�]�����
//=====================================
typedef enum
{
	BGTRANSINFOID_APPBAR,
	BGTRANSINFOID_TITLEBAR,

	BGTRANSINFOID_MAX
} BGTRANSINFOID;
//-------------------------------------
///	SCROLL
//=====================================
#define SCROLL_WINDOW_OFS_CHR		(2)				//�E�B���h�EBG�̊J�n�I�t�Z�b�g
#define SCROLL_WINDOW_OFS_DOT		(SCROLL_WINDOW_OFS_CHR*GFL_BG_1CHRDOTSIZ)				//�E�B���h�EBG�̊J�n�I�t�Z�b�g
#define SCROLL_FONT_OFS_CHR	((4+SCROLL_WINDOW_OFS_DOT))				//�E�B���h�E�ƃt�H���g�̍��I�t�Z�b�g
#define SCROLL_START_OFS_Y	(-(36-SCROLL_WINDOW_OFS_DOT))		//�J�n�I�t�Z�b�g
#define SCROLL_WINDOW_H_CHR	(3)													//�P�̃E�B���h�E�̕��i�L�����P�ʁj
#define SCROLL_WINDOW_H_DOT	(SCROLL_WINDOW_H_CHR*GFL_BG_1CHRDOTSIZ)		//�P�̃E�B���h�E�̕��i�h�b�g�P�ʁj
#define SCROLL_DISTANCE			(SCROLL_WINDOW_H_DOT*UI_SELECT_MAX)			//�X�N���[�����鋗��
#define SCROLL_START				(SCROLL_START_OFS_Y)											//�J�n
#define SCROLL_END					(-(156-(SCROLL_WINDOW_OFS_DOT+SCROLL_DISTANCE)))		//�I��

//-------------------------------------
///	UI�̎��
//=====================================
typedef enum
{
	UI_INPUT_NONE,			//���͂Ȃ�
	UI_INPUT_SLIDE,			//�^�b�`�X���C�h
	UI_INPUT_FLICK,			//�^�b�`�͂���
	UI_INPUT_TOUCH,			//�^�b�`�g���K�[
	UI_INPUT_TRG_UP,		//�L�[��
	UI_INPUT_TRG_DOWN,	//�L�[��
	UI_INPUT_TRG_RIGHT,	//�L�[�E
	UI_INPUT_TRG_LEFT,	//�L�[��
	UI_INPUT_TRG_DECIDE,//�L�[����
	UI_INPUT_TRG_CANCEL,//�L�[�L�����Z��
} UI_INPUT;

//-------------------------------------
///	UI
//=====================================
#define	UI_SLIDE_DISTANCE	(1)	//���̋����ȏ�Ȃ�΃X���C�h����

//-------------------------------------
///	ETC
//=====================================
#define APPBAR_BG_CHARAAREA_SIZE	(8*8*GFL_BG_1CHRDATASIZ)
#define TITLEBAR_BG_CHARAAREA_SIZE	(32*4*GFL_BG_1CHRDATASIZ)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�R���t�B�O�p�����[�^
//=====================================
typedef struct
{
  u8 msg_spd;
  u8 btl_eff;
  u8 btl_rule;
  u8 sound;
  u8 moji_mode;
	u8 wireless;
	u8 report;
	u8 dummy;
}CONFIG_PARAM;
//-------------------------------------
///	BG���[�N
//=====================================
typedef struct 
{
	GFL_ARCUTIL_TRANSINFO	transinfo[BGTRANSINFOID_MAX];
	u32										transfrm[BGTRANSINFOID_MAX];
	GFL_BMPWIN	*p_bmpwin[BMPWINID_MAX];
} GRAPHIC_BG_WORK;
//-------------------------------------
///	OBJ���[�N
//=====================================
typedef struct 
{
	GFL_CLUNIT *p_clunit;
	u32					reg_id[OBJRESID_MAX];
	GFL_CLWK	 *p_clwk[CLWKID_MAX];
} GRAPHIC_OBJ_WORK;
//-------------------------------------
///	GRAPHIC���[�N
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_OBJ_WORK	obj;
	GFL_TCB						*p_vblank_task;
} GRAPHIC_WORK;
//-------------------------------------
///	�V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;	//�֐��^��邽�ߑO���錾
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param );
struct _SEQ_WORK
{
	SEQ_FUNCTION	seq_function;
	BOOL is_end;
	int seq;
	void *p_param;
};
//-------------------------------------
///	UI
//=====================================
typedef struct 
{
	UI_INPUT	input;
	GFL_POINT	tp_pos;
	GFL_POINT	slide;
} UI_WORK;
//-------------------------------------
///	APPBAR
//=====================================
typedef struct 
{
	GFL_CLWK	*p_cancel;
	GFL_CLWK	*p_decide;
	GFL_CLWK	*p_check;
} APPBAR_WORK;
//-------------------------------------
///	SCROLL
//=====================================
typedef struct 
{
	u16	font_frm;
	u16	back_frm;
} SCROLL_WORK;
//-------------------------------------
///	SETTING
//=====================================
typedef struct 
{
	CONFIG_PARAM	now;	//���݂̐ݒ���
} SETTING_WORK;
//-------------------------------------
///	�R���t�B�O���C�����[�N
//=====================================
typedef struct 
{
	//�O���t�B�b�N�V�X�e��
	GRAPHIC_WORK	graphic;

	//�V�[�P���X
	SEQ_WORK			seq;

	//UI���[�N
	UI_WORK				ui;

	//�^�b�`�o�[
	APPBAR_WORK		appbar;

	//�X�N���[��
	SCROLL_WORK		scroll;

	//�ݒ胏�[�N
	SETTING_WORK	setting;

	//�ȑO�̐ݒ���
	CONFIG_PARAM	pre;	

} CONFIG_WORK;

//-------------------------------------
///	�ʒu
//=====================================
//�A�v���P�[�V�����o�[
#define APPBAR_MENUBAR_X	(0)
#define APPBAR_MENUBAR_Y	(21)
#define APPBAR_MENUBAR_W	(32)
#define APPBAR_MENUBAR_H	(3)

//�^�C�g���o�[
#define TITLEBAR_MENUBAR_X	(0)
#define TITLEBAR_MENUBAR_Y	(0)
#define TITLEBAR_MENUBAR_W	(32)
#define TITLEBAR_MENUBAR_H	(3)

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT CONFIG_PROC_Init
	( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs);
static GFL_PROC_RESULT CONFIG_PROC_Main
	( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs);
static GFL_PROC_RESULT CONFIG_PROC_Exit
	( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs);

//-------------------------------------
///	�O���t�B�b�N
//=====================================
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Main( GRAPHIC_WORK *p_wk );
static GFL_CLWK	* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//-------------------------------------
///	BG
//=====================================
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//-------------------------------------
///	OBJ
//=====================================
static void GRAPHIC_OBJ_Init
	( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id );
//-------------------------------------
///	UI
//=====================================
static void UI_Init( UI_WORK *p_wk, HEAPID heapID );
static void UI_Exit( UI_WORK *p_wk );
static void UI_Main( UI_WORK *p_wk, const SCROLL_WORK *cp_scroll );
static UI_INPUT UI_GetInput( const UI_WORK *cp_wk, GFL_POINT *p_data );
//-------------------------------------
///	APPBAR
//=====================================
static void APPBAR_Init
	( APPBAR_WORK *p_wk, GFL_CLWK *p_decide, GFL_CLWK *p_cancel, GFL_CLWK *p_check, HEAPID heapID );
static void APPBAR_Exit( APPBAR_WORK *p_wk );
static void APPBAR_Main( APPBAR_WORK *p_wk, const UI_WORK *cp_ui );
//-------------------------------------
///	SCROLL
//=====================================
static void SCROLL_Init( SCROLL_WORK *p_wk, u8 font_frm, u8 back_frm, HEAPID heapID );
static void SCROLL_Exit( SCROLL_WORK *p_wk );
static void SCROLL_Main( SCROLL_WORK *p_wk, const UI_WORK *cp_ui );
static void Scroll_Move( SCROLL_WORK *p_wk, int y_add );
//-------------------------------------
///	SETTING
//=====================================
static void SETTING_Init( SETTING_WORK *p_wk, HEAPID heapID );
static void SETTING_Exit( SETTING_WORK *p_wk );
static void SETTING_Main( SETTING_WORK *p_wk, const UI_WORK *cp_ui );
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
//-------------------------------------
///	CONFIG_PARAM
//=====================================
static void CONFIGPARAM_Create( CONFIG_PARAM *p_wk, const CONFIG *cp_savedata );
static void CONFIGPARAM_Decide( const CONFIG_PARAM *cp_wk, CONFIG *p_savedata );
static void CONFIGPARAM_Set( CONFIG_PARAM *p_wk );
//-------------------------------------
///	ETC
//=====================================
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID );

//=============================================================================
/**
 *					�O�����J
 */
//=============================================================================
//-------------------------------------
///	�R���t�B�O�v���Z�X
//=====================================
const GFL_PROC_DATA ConfigPanelProcData = 
{	
	CONFIG_PROC_Init,
	CONFIG_PROC_Main,
	CONFIG_PROC_Exit
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	BG�ݒ�
//=====================================
enum
{	
	GRAPHIC_BG_FRAME_BAR_M,				//�o�[��^�C�g��
	GRAPHIC_BG_FRAME_FONT_M,			//���C���t�H���g
	GRAPHIC_BG_FRAME_WND_M,				//�E�B���h�E
	GRAPHIC_BG_FRAME_BACK_M,			//�w�i

	GRAPHIC_BG_FRAME_BACK_S,			//�w�i

	GRAPHIC_BG_FRAME_MAX
};
static const struct 
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
}	sc_bgsetup[GRAPHIC_BG_FRAME_MAX]	=
{	
	//MAIN
	//GRAPHIC_BG_FRAME_BAR_M
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
	//GRAPHIC_BG_FRAME_FONT_M
	{	
		GFL_BG_FRAME1_M,
		{
			0, 0, 0x1000, 0,
			GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	//GRAPHIC_BG_FRAME_WND_M
	{	
		GFL_BG_FRAME2_M,
		{
			0, 0, 0x1000, 0,
			GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	//GRAPHIC_BG_FRAME_BACK_M
	{	
		GFL_BG_FRAME3_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},

	//SUB
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
};
//�t���[���擾�p�}�N��
#define GRAPHIC_BG_GetFrame( x )	(sc_bgsetup[ x ].frame)

//-------------------------------------
///	�����p��BMPWIN�쐬�f�[�^
//		�ꊇ�ō쐬����Ƒ���Ȃ��̂Ōʂɍ����
//		1024�L�����ȓ��ɂ��Ă��܂�
//=====================================
static const struct 
{
	u16			strID;	//�����ԍ�
	u8			x;			//BMPWIN���WX
	u8			y;			//BMPWIN���WY
	u8			w;			//BMPWIN���W��
	u8			h;			//BMPWIN���W����
} sc_config_menu_bmpwin_data[BMPWINID_CONFIG_MENU_MAX]	=
{	
	//���b�̑���
	{	
		mes_config_menu01,
		1,5,10,2
	},
	//������
	{	
		mes_config_sm01_00,
		15,5,4,2
	},
	//����
	{	
		mes_config_sm01_01,
		21,5,4,2
	},
	//����
	{	
		mes_config_sm01_02,
		27,5,4,2
	},
	//���퓬�A�j��
	{	
		mes_config_menu02,
		1,8,8,2
	},
	//�݂�
	{	
		mes_config_sm02_00,
		15,8,4,2
	},
	//�݂Ȃ�
	{	
		mes_config_sm02_01,
		24,8,6,2
	},
	//�������̃��[��
	{	
		mes_config_menu03,
		1,11,10,2
	},
	//���ꂩ��
	{	
		mes_config_sm03_00,
		15,11,8,2,
	},
	//�����ʂ�
	{	
		mes_config_sm03_01,
		24,11,8,2
	},
	//���T�E���h
	{	
		mes_config_menu04,
		1,14,6,2
	},
	//�X�e���I
	{	
		mes_config_sm04_00,
		15,14,6,2
	},
	//���m����
	{	
		mes_config_sm04_01,
		24,14,6,2
	},
	//���������[�h
	{	
		mes_config_menu05,
		1,17,8,2
	},
	//�Ђ炪��
	{	
		mes_config_sm05_00,
		15,17,8,2
	},
	//����
	{	
		mes_config_sm05_01,
		24,17,8,2
	},
	//������
	{	
		mes_config_menu06,
		1,20,10,2
	},
	//�I��
	{	
		mes_config_sm06_00,
		15,20,8,2
	},
	//�I�t
	{	
		mes_config_sm06_01,
		24,20,8,2
	},
	//�����C�����X�ڑ��O���|�[�g
	{	
		mes_config_menu07,
		1,23,10,2
	},
	//����
	{	
		mes_config_sm07_00,
		15,23,6,2
	},
	//�����Ȃ�
	{	
		mes_config_sm07_01,
		24,23,6,2
	},
};

//=============================================================================
/**
 *					�v���Z�X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�R���t�B�O�v���Z�X������
 *
 *	@param	GFL_PROC *p_proc�v���Z�X
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param_adrs		�������[�N
 *	@param	*p_wk_adrs			���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CONFIG_PROC_Init( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs)
{	
	//�q�[�v�쐬
  GFL_HEAP_CreateHeap(GFL_HEAPID_APP,HEAPID_CONFIG,0x20000);

	//�v���Z�X���[�N�쐬
	{	
		CONFIG_WORK	*p_wk;
		p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(CONFIG_WORK), HEAPID_CONFIG);
		GFL_STD_MemClear( p_wk, sizeof(CONFIG_WORK) );

		//���W���[��������
		GRAPHIC_Init( &p_wk->graphic, HEAPID_CONFIG );
		SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeOut );
		APPBAR_Init( &p_wk->appbar, 
				GRAPHIC_GetClwk(&p_wk->graphic, CLWKID_DECIDE),
				GRAPHIC_GetClwk(&p_wk->graphic, CLWKID_CANCEL),
				GRAPHIC_GetClwk(&p_wk->graphic, CLWKID_CHECK),
				HEAPID_CONFIG );
		SCROLL_Init( &p_wk->scroll, 
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_FONT_M),
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_WND_M),
				HEAPID_CONFIG);
		UI_Init( &p_wk->ui, HEAPID_CONFIG );
	}


  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�R���t�B�O�v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc�v���Z�X
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param_adrs		�������[�N
 *	@param	*p_wk_adrs			���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CONFIG_PROC_Exit( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs)
{	
	CONFIG_WORK	*p_wk	= p_wk_adrs;

	//���W���[���j��
	UI_Exit( &p_wk->ui );
	SCROLL_Exit( &p_wk->scroll );
	APPBAR_Exit( &p_wk->appbar );
	SEQ_Exit( &p_wk->seq );
	GRAPHIC_Exit( &p_wk->graphic );

 	//���[�N�G���A���
  GFL_PROC_FreeWork( p_proc );

	//�q�[�v���
  GFL_HEAP_DeleteHeap( HEAPID_CONFIG );

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�R���t�B�O�v���Z�X���C������
 *
 *	@param	GFL_PROC *p_proc�v���Z�X
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param_adrs		�������[�N
 *	@param	*p_wk_adrs			���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CONFIG_PROC_Main( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs)
{
	CONFIG_WORK	*p_wk	= p_wk_adrs;

	//�V�[�P���X
	SEQ_Main( &p_wk->seq );

	//�`��
	GRAPHIC_Main( &p_wk->graphic );

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
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID )
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
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	//	�\��
	GFL_DISP_GX_InitVisibleControl();

	//	�t�H���g������
	GFL_FONTSYS_Init();

	//	���W���[��������
	GRAPHIC_BG_Init( &p_wk->bg, heapID );
	GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );

	//	�ݒ�
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG3,GX_BLEND_PLANEMASK_BG0, 13, 8 );

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

	//���W�X�^������
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

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
static void GRAPHIC_Main( GRAPHIC_WORK *p_wk )
{	
	GRAPHIC_OBJ_Main( &p_wk->obj );
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
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID )
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

	GFL_BG_FillCharacter( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, 1,  0 );


	//���C���f�ޓǂݍ���
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_CONFIG_GRA, heapID );
		
		//PLT
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_config_gra_est_bar_01_NCLR,
				PALTYPE_MAIN_BG, CONFIG_BG_PAL_M_00*0x20, 0, heapID );

		//titlebar--------------
		//CHR
		p_wk->transinfo[BGTRANSINFOID_TITLEBAR]	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan
			( p_handle, NARC_config_gra_ue_bar_01_NCGR, 
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 
				TITLEBAR_BG_CHARAAREA_SIZE, FALSE, heapID );	

		GF_ASSERT( p_wk->transinfo[BGTRANSINFOID_TITLEBAR] != GFL_ARCUTIL_TRANSINFO_FAIL );

		//�X�N���[���̓�������ɒu���āA�㕔32*3������������
		ARCHDL_UTIL_TransVramScreenEx( p_handle, 
				NARC_config_gra_ue_bar_01_NSCR, 
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M),
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->transinfo[BGTRANSINFOID_TITLEBAR]), 0, 0, 32, 3, 
				TITLEBAR_MENUBAR_X, TITLEBAR_MENUBAR_Y, TITLEBAR_MENUBAR_W, TITLEBAR_MENUBAR_H,
				CONFIG_BG_PAL_M_08, FALSE, heapID );
		p_wk->transfrm[BGTRANSINFOID_TITLEBAR]	= GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M);
		GFL_BG_FillScreen( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, 0, 3, 32, 21, 0 );

		//WND------------------
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_config_gra_est_bar_01_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_WND_M), 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_config_gra_est_bar_01_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_WND_M), 0, 0, FALSE, heapID );

		//BACK------------------
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_config_gra_est_back_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_M), 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_config_gra_est_back_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_M), 0, 0, FALSE, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}

	//APPBAR�p�f�ޓǂݍ���
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );
		//PLT
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, APP_COMMON_GetBarPltArcIdx(),
				PALTYPE_MAIN_BG, CONFIG_BG_PAL_M_15*0x20, APP_COMMON_BAR_PLT_NUM*0x20, heapID );
		//CHR
		p_wk->transinfo[BGTRANSINFOID_APPBAR]	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan
			( p_handle, APP_COMMON_GetBarCharArcIdx(), 
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 
				APPBAR_BG_CHARAAREA_SIZE, FALSE, heapID );
		p_wk->transfrm[BGTRANSINFOID_APPBAR]	= GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M);

		GF_ASSERT( p_wk->transinfo[BGTRANSINFOID_APPBAR] != GFL_ARCUTIL_TRANSINFO_FAIL );

		//�X�N���[���̓�������ɒu���āA����32*3������������
		ARCHDL_UTIL_TransVramScreenEx( p_handle, 
				APP_COMMON_GetBarScrnArcIdx(), 
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M),
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->transinfo[BGTRANSINFOID_APPBAR]), 0, 21, 32, 24, 
				APPBAR_MENUBAR_X, APPBAR_MENUBAR_Y, APPBAR_MENUBAR_W, APPBAR_MENUBAR_H,
				CONFIG_BG_PAL_M_15, FALSE, heapID );
		
		GFL_ARC_CloseDataHandle( p_handle );
	}

	//BMPWIN�쐬��������\��t����
	{	
		int i;
		GFL_FONT	*p_font;
		STRBUF		*p_strbuf;
		GFL_MSGDATA	*p_msg;

		p_font	= GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr, 
				GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

		p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
				NARC_message_config_dat, heapID );

		GFL_FONTSYS_SetColor( 0xF, 0xE, 0 );
		for( i = 0; i < BMPWINID_CONFIG_MENU_MAX; i++ )
		{	
			p_strbuf	= GFL_MSG_CreateString( p_msg, sc_config_menu_bmpwin_data[i].strID );
			p_wk->p_bmpwin[i]	= GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_FONT_M),
				sc_config_menu_bmpwin_data[i].x, sc_config_menu_bmpwin_data[i].y,
				sc_config_menu_bmpwin_data[i].w, sc_config_menu_bmpwin_data[i].h, 
				0, GFL_BMP_CHRAREA_GET_B );

			PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0, 0, p_strbuf, p_font );

			GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->p_bmpwin[i] );

			GFL_STR_DeleteBuffer(p_strbuf);
		}
		GFL_FONTSYS_SetDefaultColor();

		GFL_MSG_Delete( p_msg );
		GFL_FONT_Delete( p_font );
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
		for( i = 0; i < BMPWINID_MAX; i++ )
		{	
			if( p_wk->p_bmpwin[i] )
			{	
				GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
			}
		}
	}

	//���\�[�X�j��
	{	
		int i;
		for( i = 0; i < BGTRANSINFOID_MAX; i++ )
		{	
			GFL_BG_FreeCharacterArea(p_wk->transfrm[i],
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->transinfo[i]),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->transinfo[i]));
		}

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
 *					UI
 *						UI�͓��͂𒊏ۉ����ĕԂ�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[���͏�����
 *
 *	@param	UI_WORK *p_wk	���[�N
 *	@param	heapID				�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UI_Init( UI_WORK *p_wk, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[���͔j��
 *
 *	@param	UI_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void UI_Exit( UI_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[���̓��C������
 *
 *	@param	UI_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void UI_Main( UI_WORK *p_wk, const SCROLL_WORK *cp_scroll )
{	
	u32 x, y;

	//��[���Z�b�g
	p_wk->input	= UI_INPUT_NONE;

	//�L�[����
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
	{	
		p_wk->input	= UI_INPUT_TRG_UP;
	}
	else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
	{	
		p_wk->input	= UI_INPUT_TRG_DOWN;
	}
	else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
	{	
		p_wk->input	= UI_INPUT_TRG_LEFT;
	}
	else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
	{	
		p_wk->input	= UI_INPUT_TRG_RIGHT;
	}

	//�^�b�`����
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		p_wk->input	= UI_INPUT_TOUCH;
		p_wk->tp_pos.x	= x;
		p_wk->tp_pos.y	= y;
	}
	else if( GFL_UI_TP_GetPointCont( &x, &y ) )
	{	
		if( MATH_IAbs( y - p_wk->tp_pos.y ) >= UI_SLIDE_DISTANCE )
		{	
			p_wk->input	= UI_INPUT_SLIDE;
			p_wk->slide.x		= x - p_wk->tp_pos.x;
			p_wk->slide.y		= y - p_wk->tp_pos.y;
			p_wk->tp_pos.x	= x;
			p_wk->tp_pos.y	= y;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���͂��擾
 *
 *	@param	const UI_WORK *cp_wk	���[�N
 *	@param	*p_data		��ޕʂ̎󂯎��f�[�^
 *		UI_INPUT_TOUCH�Ȃ�� TRG���W
 *		UI_INPUT_SLIDE�Ȃ�� �ړ��I�t�Z�b�g
 *
 *	@return	���͂̎��
 */
//-----------------------------------------------------------------------------
static UI_INPUT UI_GetInput( const UI_WORK *cp_wk, GFL_POINT *p_data )
{	
	if(p_data)
	{	
		switch( cp_wk->input )
		{	
		case UI_INPUT_TOUCH:
			*p_data	= cp_wk->tp_pos;
			break;
	
		case UI_INPUT_SLIDE:
			*p_data	= cp_wk->slide;
			break;
		}
	}

	return cp_wk->input;
}

//=============================================================================
/**
 *					APPBAR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR������
 *
 *	@param	APPBAR_WORK *p_wk	���[�N
 *	@param	*p_decide	����pCLWK
 *	@param	*p_cancel	�L�����Z���pCLWK
 *	@param	heapID		�q�[�v
 *
 */
//-----------------------------------------------------------------------------
static void APPBAR_Init
	( APPBAR_WORK *p_wk, GFL_CLWK *p_decide, GFL_CLWK *p_cancel, GFL_CLWK *p_check, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(APPBAR_WORK) );
	p_wk->p_decide	= p_decide;
	p_wk->p_cancel	= p_cancel;
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR�j��
 *
 *	@param	APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void APPBAR_Exit( APPBAR_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(APPBAR_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR���C������
 *
 *	@param	APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void APPBAR_Main( APPBAR_WORK *p_wk, const UI_WORK *cp_ui )
{	

}
//=============================================================================
/**
 *					SCROLL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[���Ǘ�������
 *
 *	@param	SCROLL_WORK *p_wk	���[�N
 *	@param	font_frm					����BG
 *	@param	back_frm					�w�iBG
 *	@param	heapID						�q�[�vID
 */
//-----------------------------------------------------------------------------
static void SCROLL_Init( SCROLL_WORK *p_wk, u8 font_frm, u8 back_frm, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk,sizeof(SCROLL_WORK) );
	p_wk->font_frm	= font_frm;
	p_wk->back_frm	= back_frm;

	//�J�n�I�t�Z�b�g�܂ł��炷
	Scroll_Move( p_wk, SCROLL_START_OFS_Y );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[���Ǘ��j��
 *
 *	@param	SCROLL_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void SCROLL_Exit( SCROLL_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk,sizeof(SCROLL_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[���Ǘ����C������
 *
 *	@param	SCROLL_WORK *p_wk	���[�N
 *	@param	UI_WORK *cp_ui		UI���[�N
 */
//-----------------------------------------------------------------------------
static void SCROLL_Main( SCROLL_WORK *p_wk, const UI_WORK *cp_ui )
{	
	UI_INPUT input;
	GFL_POINT pos;
	
	//���͎擾
	input	= UI_GetInput( cp_ui, &pos );

	//�X���C�h�ړ�
	switch( input )
	{	
	case UI_INPUT_SLIDE:
		Scroll_Move( p_wk, -pos.y );
		break;

	case UI_INPUT_TRG_UP:
		Scroll_Move( p_wk, SCROLL_WINDOW_H_DOT );
		break;

	case UI_INPUT_TRG_DOWN:
		Scroll_Move( p_wk, -SCROLL_WINDOW_H_DOT );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[���ړ�
 *
 *	@param	SCROLL_WORK *p_wk		���[�N
 *	@param	y_add								Y�ړ��l
 */
//-----------------------------------------------------------------------------
static void Scroll_Move( SCROLL_WORK *p_wk, int y_add )
{	
	int y;

	//����
	y	= GFL_BG_GetScrollY( p_wk->back_frm );
	y	+= y_add;
	y	= MATH_CLAMP( y, SCROLL_START, SCROLL_END );

	//�ړ�
	GFL_BG_SetScroll( p_wk->font_frm, GFL_BG_SCROLL_Y_SET, y + SCROLL_FONT_OFS_CHR );
	GFL_BG_SetScroll( p_wk->back_frm, GFL_BG_SCROLL_Y_SET, y );

	//��������
	{	
		static const u8 sc_bright_plt[]	=  
		{	
			3, 1, 0, 1, 3,
		};

		int i;
		int bar_top;
		int start, end;

		//�擪�o�[
		bar_top	= (y-SCROLL_START)/GFL_BG_1CHRDOTSIZ;

		//��[�S��4�łʂ�
		GFL_BG_ChangeScreenPalette( p_wk->back_frm, 0, 0,
					32, 32, 4 );
		GFL_BG_ChangeScreenPalette( p_wk->font_frm, 0, 0,
					32, 32, 4 );
		
		for( i = 0; i < NELEMS(sc_bright_plt); i++ )
		{	
			//�o�[���ʂ�
			start	= SCROLL_WINDOW_OFS_CHR + bar_top + ( i ) * SCROLL_WINDOW_H_CHR;
			end		= SCROLL_WINDOW_OFS_CHR + bar_top + ( i + 1 ) * SCROLL_WINDOW_H_CHR;
			GFL_BG_ChangeScreenPalette( p_wk->back_frm, 0, start,
					32, end, sc_bright_plt[i] );
			//�t�H���g���ʂ�
			start	= SCROLL_WINDOW_OFS_CHR*2 + bar_top + ( i ) * SCROLL_WINDOW_H_CHR;
			end		= SCROLL_WINDOW_OFS_CHR*2 + bar_top + ( i + 1 ) * SCROLL_WINDOW_H_CHR;
			GFL_BG_ChangeScreenPalette( p_wk->font_frm, 0, start,
					32, end, sc_bright_plt[i] );
		}

		//NAGI_Printf( "bar_top %d \n", bar_top );
		GFL_BG_LoadScreenReq( p_wk->back_frm );
		GFL_BG_LoadScreenReq( p_wk->font_frm );
	}
	
	//NAGI_Printf( "YOFS %d add %d\n", y, y_add );
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

	CONFIG_WORK	*p_wk	= p_param;

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
		SEQ_SetNext( p_seqwk, SEQFUNC_Main );
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

	CONFIG_WORK	*p_wk	= p_param;

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
	CONFIG_WORK	*p_wk	= p_param;

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
	{	
			SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
	}

	//���W���[�����C��
	SCROLL_Main( &p_wk->scroll, &p_wk->ui );
	UI_Main( &p_wk->ui, &p_wk->scroll );
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

