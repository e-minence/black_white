//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file     config_panel2.c
 *  @brief    �R���t�B�O���
 *  @author   Toru=Nagihashi
 *  @data     2009.09.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//library
#include <gflib.h>

//system
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID
#include "system/bmp_winframe.h"
#include "sound/pm_sndsys.h"
#include "gamesystem/msgspeed.h"
#include "system/ds_system.h"
#include "app/app_printsys_common.h"

//module
#include "app/app_menu_common.h"
#include "app/app_taskmenu.h"

//ui_common
#include "ui/touchbar.h"

//archive
#include "arc_def.h"
#include "config_gra.naix"

//print
#include "font/font.naix"
#include "message.naix"
#include "script_message.naix"
#include "msg/msg_config.h"
#include "msg/script/msg_common_scr.h"
#include "print/gf_font.h"
#include "print/printsys.h"

//mine
#include "app/config_panel.h"
#include "config_snd.h"
#include "savedata/config.h"

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================
//-------------------------------------
/// �f�o�b�O
//=====================================
#ifdef PM_DEBUG
#define GAMESYS_NONE_MOVE //�Q�[���V�X�e�����Ȃ��Ă�����
#endif //PM_DEBUG

#define SCROLL_MOVE_NONE  //�X�N���[���Ȃ���

//-------------------------------------
/// �؂�ւ��}�N��
//=====================================

//-------------------------------------
/// �p���b�g
//=====================================
enum
{
  // ���C�����BG                           0~0xD                   0xE 0xF
  CONFIG_BG_PAL_M_00 = 0,// BG�p�p���b�g�擪  �o�[���邢�F    �t�H���g���I��OFF�F
  CONFIG_BG_PAL_M_01,   //                    �o�[���ʂ̐F
  CONFIG_BG_PAL_M_02,   //
  CONFIG_BG_PAL_M_03,   //                    �o�[�Â��F      �t�H���g���I��OFF�F
  CONFIG_BG_PAL_M_04,   //
  CONFIG_BG_PAL_M_05,   //                    �o�[��Ԃ��炢
  CONFIG_BG_PAL_M_06,   //
  CONFIG_BG_PAL_M_07,   //  �I��F�o�[�p���b�g�t�F�[�h        �t�H���g�I��ON�F
  CONFIG_BG_PAL_M_08,   //  TITLE
  CONFIG_BG_PAL_M_09,   //  CONFIRMWND
  CONFIG_BG_PAL_M_10,   //
  CONFIG_BG_PAL_M_11,   //
  CONFIG_BG_PAL_M_12,   //                                    �t�H���g�I��OFF�F
  CONFIG_BG_PAL_M_13,   //  TASKWND
  CONFIG_BG_PAL_M_14,   //
  CONFIG_BG_PAL_M_15,   //  CONFIRMTEXT


  // �T�u���BG
  CONFIG_BG_PAL_S_00 = 0,// �w�i
  CONFIG_BG_PAL_S_01,   //
  CONFIG_BG_PAL_S_02,   //
  CONFIG_BG_PAL_S_03,   //
  CONFIG_BG_PAL_S_04,   //  FRM
  CONFIG_BG_PAL_S_05,   //
  CONFIG_BG_PAL_S_06,   //
  CONFIG_BG_PAL_S_07,   //
  CONFIG_BG_PAL_S_08,   //
  CONFIG_BG_PAL_S_09,   //
  CONFIG_BG_PAL_S_10,   //
  CONFIG_BG_PAL_S_11,   //
  CONFIG_BG_PAL_S_12,   //
  CONFIG_BG_PAL_S_13,   //
  CONFIG_BG_PAL_S_14,   //
  CONFIG_BG_PAL_S_15,   //

  // ���C�����OBJ
  CONFIG_OBJ_PAL_M_00 = 0,//  OBJ�p�p���b�g�擪 ���邢�F
  CONFIG_OBJ_PAL_M_01,    //                    ���ʂ̐F
  CONFIG_OBJ_PAL_M_02,    //
  CONFIG_OBJ_PAL_M_03,    //                    �Â��F
  CONFIG_OBJ_PAL_M_04,    //
  CONFIG_OBJ_PAL_M_05,    //
  CONFIG_OBJ_PAL_M_06,    //  �����΁[
  CONFIG_OBJ_PAL_M_07,    //  �I��F
  CONFIG_OBJ_PAL_M_08,    //  �����o�[���ʑf��
  CONFIG_OBJ_PAL_M_09,    //
  CONFIG_OBJ_PAL_M_10,    //
  CONFIG_OBJ_PAL_M_11,    //
  CONFIG_OBJ_PAL_M_12,    //
  CONFIG_OBJ_PAL_M_13,    //
  CONFIG_OBJ_PAL_M_14,    //
  CONFIG_OBJ_PAL_M_15,    //


};

//-------------------------------------
/// OBJ���\�[�X�o�^ID
//=====================================
enum
{
  OBJRESID_ITEM_PLT,
  OBJRESID_ITEM_CHR,
  OBJRESID_ITEM_CEL,
  OBJRESID_BAR_CHR,
  OBJRESID_BAR_CEL,
  OBJRESID_MAX
};
//-------------------------------------
/// �R���t�B�O�̃��j���[
//=====================================
typedef enum
{
  CONFIG_LIST_MSGSPEED,
  CONFIG_LIST_BTLEFF,
  CONFIG_LIST_BTLRULE,
  CONFIG_LIST_SND,
  CONFIG_LIST_STR,
//  CONFIG_LIST_WIRELESS,
  CONFIG_LIST_REPORT,


  CONFIG_LIST_MAX,  //�ő�

  CONFIG_LIST_INIT    = CONFIG_LIST_MSGSPEED-1, //����
  CONFIG_LIST_DECIDE  = CONFIG_LIST_MAX,  //��
  CONFIG_LIST_CANCEL, //��
} CONFIG_LIST;

//-------------------------------------
/// �R���t�B�O�A�C�e���̐�
//=====================================
typedef enum
{
  CONFIG_ITEM_MSG_SLOW,
  CONFIG_ITEM_MSG_NORMAL,
  CONFIG_ITEM_MSG_FAST,
  CONFIG_ITEM_BTLEFF_YES,
  CONFIG_ITEM_BTLEFF_NO,
  CONFIG_ITEM_BTLRULE_IREKAE,
  CONFIG_ITEM_BTLRULE_KACHINUKI,
  CONFIG_ITEM_SND_STEREO,
  CONFIG_ITEM_SND_MONO,
  CONFIG_ITEM_STR_HIRAGANA,
  CONFIG_ITEM_STR_KANJI,
//  CONFIG_ITEM_WIRELESS_YES,
//  CONFIG_ITEM_WIRELESS_NO,
  CONFIG_ITEM_REPORT_YES,
  CONFIG_ITEM_REPORT_NO,

  CONFIG_ITEM_MAX,
  CONFIG_ITEM_NONE  = CONFIG_ITEM_MAX,
}CONFIG_ITEM;

//-------------------------------------
/// CLWK��ID
//=====================================
typedef enum
{
  CLWKID_CHECK,
  CLWKID_DOWNBAR,

  CLWKID_ITEM_TOP,
  CLWKID_ITEM_END = CLWKID_ITEM_TOP + CONFIG_ITEM_MAX,

  CLWKID_MAX
}CLWKID;
//-------------------------------------
/// BMPWINID
//=====================================
typedef enum
{
  BMPWINID_ITEM_TOP,
  BMPWINID_ITEM_END = BMPWINID_ITEM_TOP + CONFIG_ITEM_MAX - 1,
  BMPWINID_ITEM_MAX,
  BMPWINID_LIST_TOP = BMPWINID_ITEM_MAX,
  BMPWINID_LIST_END = BMPWINID_LIST_TOP + CONFIG_LIST_MAX - 1,
  BMPWINID_LIST_MAX,
  BMPWINID_CONFIG_MENU_MAX  = BMPWINID_LIST_MAX,
  BMPWINID_TITLE  = BMPWINID_CONFIG_MENU_MAX,
  BMPWINID_TEXT,
  BMPWINID_CONFIRM,

  BMPWINID_MAX
}BMPWINID;

//-------------------------------------
/// SCROLL
//=====================================
#define SCROLL_WINDOW_OFS_CHR   (2)       //�E�B���h�EBG�̊J�n�I�t�Z�b�g
#define SCROLL_WINDOW_OFS_DOT   (SCROLL_WINDOW_OFS_CHR*GFL_BG_1CHRDOTSIZ)       //�E�B���h�EBG�̊J�n�I�t�Z�b�g
#define SCROLL_FONT_OFS_CHR ((4+SCROLL_WINDOW_OFS_DOT))       //�E�B���h�E�ƃt�H���g�̍��I�t�Z�b�g
#define SCROLL_START_OFS_Y  (-(36-SCROLL_WINDOW_OFS_DOT))   //�J�n�I�t�Z�b�g
#define SCROLL_WINDOW_H_CHR (3)                         //�P�̃E�B���h�E�̕��i�L�����P�ʁj
#define SCROLL_WINDOW_H_DOT (SCROLL_WINDOW_H_CHR*GFL_BG_1CHRDOTSIZ)   //�P�̃E�B���h�E�̕��i�h�b�g�P�ʁj
#define SCROLL_DISTANCE     (SCROLL_WINDOW_H_DOT*CONFIG_LIST_MAX)     //�X�N���[�����鋗��
#define SCROLL_START        (SCROLL_START_OFS_Y)                                //�J�n
#define SCROLL_END          (-(156-(SCROLL_WINDOW_OFS_DOT+SCROLL_DISTANCE)))    //�I��
#define SCROLL_DISPLAY      (4) //��ʂ��猩����E�B���h�E�̐�
#define SCROLL_START_OBJ    (SCROLL_WINDOW_OFS_DOT-SCROLL_START_OFS_Y)  //�J�n
#define SCROLL_END_OBJ      (156-(SCROLL_START_OBJ+SCROLL_DISTANCE)+36)   //�I��
#define SCROLL_CONT_SYNC_MAX  (10)  //�������ςȂ��̈ړ��Ԋu
#define SCROLL_TOP_BAR_Y    (3*GFL_BG_1CHRDOTSIZ)
#define SCROLL_APP_BAR_Y    (192-3*GFL_BG_1CHRDOTSIZ)

#define SCROLL_CHAGEPLT_SAFE_SYNC (3)   //���V���N�����Ȃ��OK��

//-------------------------------------
/// UI�̎��
//=====================================
typedef enum
{
  UI_INPUT_NONE,      //���͂Ȃ�
  UI_INPUT_SLIDE,     //�^�b�`�X���C�h
  UI_INPUT_FLICK,     //�^�b�`�͂���
  UI_INPUT_TOUCH,     //�^�b�`�g���K�[
  UI_INPUT_TRG_UP,    //�L�[��
  UI_INPUT_TRG_DOWN,  //�L�[��
  UI_INPUT_TRG_RIGHT, //�L�[�E
  UI_INPUT_TRG_LEFT,  //�L�[��
  UI_INPUT_TRG_DECIDE,//�L�[����
  UI_INPUT_TRG_CANCEL,//�L�[�L�����Z��
  UI_INPUT_CONT_UP,   //�L�[��
  UI_INPUT_CONT_DOWN, //�L�[��
  UI_INPUT_TRG_Y, //�L�[Y
  UI_INPUT_TRG_X, //�L�[X
  UI_INPUT_KEY,    //�L�[���[�h�؂�ւ�
} UI_INPUT;

//-------------------------------------
/// UI�̏��
//=====================================
typedef enum
{
  UI_INPUT_PARAM_TRGPOS,
  UI_INPUT_PARAM_SLIDEPOS,
  UI_INPUT_PARAM_SLIDENOW,
} UI_INPUT_PARAM;


//-------------------------------------
/// UI
//=====================================
#define UI_SLIDE_DISTANCE_START (1) //���̋����ȏ�Ȃ�΃X���C�h����

//-------------------------------------
/// �ʒu
//=====================================
//�A�v���P�[�V�����o�[
#define APPBAR_MENUBAR_X  (0)
#define APPBAR_MENUBAR_Y  (21)
#define APPBAR_MENUBAR_W  (32)
#define APPBAR_MENUBAR_H  (3)

#define APPBAR_WIN_X  (14)
#define APPBAR_WIN_W  (9)

//�^�C�g���o�[
#define TITLEBAR_MENUBAR_X  (0)
#define TITLEBAR_MENUBAR_Y  (0)
#define TITLEBAR_MENUBAR_W  (32)
#define TITLEBAR_MENUBAR_H  (3)

//����E�B���h�E
#define INFOWND_X   (1)
#define INFOWND_Y   (19)
#define INFOWND_W   (30)
#define INFOWND_H   (4)

//�^�C�g������
#define TITLEWND_X  (0)
#define TITLEWND_Y  (0)
#define TITLEWND_W  (16)
#define TITLEWND_H  (3)

//�m�F
#define CONFIRMWND_X  (1)
#define CONFIRMWND_Y  (1)
#define CONFIRMWND_W  (30)
#define CONFIRMWND_H  (4)

//�`�F�b�N�{�^��
#define CLWK_CHECK_X  (88)
#define CLWK_CHECK_Y  (172)

//-------------------------------------
/// PARETTLE_FADE
//=====================================
#define PLTFADE_SELECT_ADD  (0x400)
#define PLTFADE_SELECT_STARTCOLOR GX_RGB( 7, 13, 20 )
#define PLTFADE_SELECT_ENDCOLOR GX_RGB( 12, 25, 30 )

//-------------------------------------
/// APP
//=====================================
typedef enum
{
  APPBAR_WIN_DECIDE,
  APPBAR_WIN_CANCEL,
  APPBAR_WIN_EXIT,
  APPBAR_WIN_MAX,
  APPBAR_WIN_NULL = APPBAR_WIN_MAX,
}APPBAR_WIN_LIST;

//-------------------------------------
/// CONFIRM
//=====================================
typedef enum
{
  CONFIRM_SELECT_NULL,
  CONFIRM_SELECT_YES,
  CONFIRM_SELECT_NO,
} CONFIRM_SELECT;

enum
{
  CONFIRM_WIN_YES,
  CONFIRM_WIN_NO,
  CONFIRM_WIN_MAX,
};

//-------------------------------------
/// ETC
//=====================================
#define APPBAR_BG_CHARAAREA_SIZE  (4*GFL_BG_1CHRDATASIZ)
#define TITLEBAR_BG_CHARAAREA_SIZE  (32*4*GFL_BG_1CHRDATASIZ)


//=============================================================================
/**
 *          �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// �R���t�B�O�p�����[�^
//=====================================
typedef struct
{
  u8 param[CONFIG_LIST_MAX];
  u8 change;
  CONFIG *p_savedata;
}CONFIG_PARAM;
//-------------------------------------
/// BG���[�N
//=====================================
typedef struct
{
  GFL_BMPWIN  *p_bmpwin[BMPWINID_MAX];
  u16                   pltfade_cnt[2];
  GXRgb                 trans_color[2];
  void                  *ncg_buf;
  NNSG2dCharacterData   *ncg_data;

  GXRgb                 plt_color[0x10];

} GRAPHIC_BG_WORK;
//-------------------------------------
/// OBJ���[�N
//=====================================
typedef struct
{
  GFL_CLUNIT *p_clunit;
  u32         reg_id[OBJRESID_MAX];
  GFL_CLWK   *p_clwk[CLWKID_MAX];
  u16         anm_idx;
  u16         dummy;
} GRAPHIC_OBJ_WORK;
//-------------------------------------
/// GRAPHIC���[�N
//=====================================
typedef struct
{
  GRAPHIC_BG_WORK   bg;
  GRAPHIC_OBJ_WORK  obj;
  GFL_TCB           *p_vblank_task;
} GRAPHIC_WORK;
//-------------------------------------
/// �V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;  //�֐��^��邽�ߑO���錾
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param );
struct _SEQ_WORK
{
  SEQ_FUNCTION  seq_function;
  BOOL is_end;
  int seq;
  void *p_param;
};
//-------------------------------------
/// UI
//=====================================
typedef struct
{
  UI_INPUT  input;
  GFL_POINT tp_pos;
  GFL_POINT slide;
  GFL_POINT slide_start;
  BOOL      is_start_slide;
} UI_WORK;
//-------------------------------------
/// ���b�Z�[�W�E�B���h�E
//=====================================
typedef struct
{
  GFL_MSGDATA       *p_msg;
  GFL_FONT          *p_font;
  PRINT_STREAM      *p_stream;
  GFL_TCBLSYS       *p_tcbl;
  GFL_BMPWIN*       p_bmpwin;
  STRBUF*           p_strbuf;
  u16               clear_chr;
  u16               heapID;
  PRINT_UTIL        util;
  PRINT_QUE         *p_que;
  u32               print_update;
  APP_PRINTSYS_COMMON_WORK  trg_work;
  u8                padding[3];
} MSGWND_WORK;
//-------------------------------------
/// �ŏI�m�F��� CONFIRM
//=====================================
typedef struct
{
  MSGWND_WORK msg;
  APP_TASKMENU_WORK     *p_menu;
  APP_TASKMENU_ITEMWORK item[ CONFIRM_WIN_MAX ];
  u16 select;
  u16 seq;
  APP_TASKMENU_INITWORK init;
  APP_TASKMENU_RES      *p_res;
} CONFIRM_WORK;

//-------------------------------------
/// APPBAR
//=====================================
typedef struct
{
  GFL_FONT        *p_font;
  PRINT_QUE       *p_que;
  TOUCHBAR_WORK   *p_touch;
  APPBAR_WIN_LIST select;
  BOOL            is_decide;
  APP_TASKMENU_RES *p_res;
  GFL_MSGDATA     *p_msg;
  APP_TASKMENU_ITEMWORK item[ APPBAR_WIN_MAX-1 ];
  APP_TASKMENU_WIN_WORK *p_win[ APPBAR_WIN_MAX-1 ];
  GAMEDATA        *p_gdata;
} APPBAR_WORK;
//-------------------------------------
/// SCROLL
//=====================================
typedef struct
{
  u16 font_frm;
  u16 back_frm;
  CONFIG_LIST select;
  GFL_CLWK *p_item[CONFIG_ITEM_MAX];
  int obj_y_ofs;
  u32 cont_sync;
  u32 dir_bit;
  int pre_y;
  CONFIG_PARAM  now;
  BOOL is_info_update;
  BOOL pre_decide_draw;
} SCROLL_WORK;
//-------------------------------------
/// �R���t�B�O���C�����[�N
//=====================================
typedef struct
{
  //�O���t�B�b�N�V�X�e��
  GRAPHIC_WORK  graphic;

  //�V�[�P���X�V�X�e��
  SEQ_WORK      seq;

  //UI���[�N
  UI_WORK       ui;

  //�^�b�`�o�[
  APPBAR_WORK   appbar;

  //�X�N���[��
  SCROLL_WORK   scroll;

  //���ʉ�����E�B���h�E
  MSGWND_WORK   info;

  //�ŏI�m�F�I��
  CONFIRM_WORK  confirm;

  //�ȑO�̐ݒ���
  CONFIG_PARAM  pre;

  //���ʂŎg���t�H���g
  GFL_FONT      *p_font;

  //���ʂŎg���L���[
  PRINT_QUE     *p_que;

  //���ʂŎg�����b�Z�[�W�f�[�^
  GFL_MSGDATA   *p_msg;

  //���ʃ^�X�N���j���[�p���\�[�X
  APP_TASKMENU_RES  *p_menures;
  APP_TASKMENU_RES  *p_confirmres;

  //���ʂŎg���^�X�N�V�X�e��
  GFL_TCBLSYS       *p_tcbl;

  //����
  CONFIG_PANEL_PARAM  *p_param;
} CONFIG_WORK;

//=============================================================================
/**
 *          �v���g�^�C�v�錾
 *              ���W���[�������啶��  XXXX_Init
 *                ���\�[�X���O���[�o���֐��i�p�u���b�N�j
 *              ���W���[������������  Xxxx_Init
 *                �����W���[�����֐��i�v���C�x�[�g�j
*/
//=============================================================================
//-------------------------------------
/// �v���Z�X
//=====================================
static GFL_PROC_RESULT CONFIG_PROC_Init
  ( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs);
static GFL_PROC_RESULT CONFIG_PROC_Main
  ( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs);
static GFL_PROC_RESULT CONFIG_PROC_Exit
  ( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs);
//-------------------------------------
/// �O���t�B�b�N
//=====================================
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Main( GRAPHIC_WORK *p_wk );
static GFL_CLWK * GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT *GRAPHIC_GetUnit( const GRAPHIC_WORK *cp_wk );
static GFL_BMPWIN * GRAPHIC_GetBmpwin( const GRAPHIC_WORK *cp_wk, BMPWINID id );
static void GRAPHIC_PrintBmpwin( GRAPHIC_WORK *p_wk );
static void GRAPHIC_StartPalleteFade( GRAPHIC_WORK *p_wk );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//-------------------------------------
/// BG
//=====================================
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_Main( GRAPHIC_BG_WORK *p_wk );
static GFL_BMPWIN *GRAPHIC_BG_GetBmpwin( const GRAPHIC_BG_WORK *cp_wk, BMPWINID id );
static void GRAPHIC_BG_PrintBmpwin( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_StartPalletFade( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
static void Graphic_Bg_PalletFadeMain( u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end );
//-------------------------------------
/// OBJ
//=====================================
static void GRAPHIC_OBJ_Init
  ( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT *GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk );
//-------------------------------------
/// UI
//=====================================
static void UI_Init( UI_WORK *p_wk, HEAPID heapID );
static void UI_Exit( UI_WORK *p_wk );
static void UI_Main( UI_WORK *p_wk );
static UI_INPUT UI_GetInput( const UI_WORK *cp_wk );
static void UI_GetParam( const UI_WORK *cp_wk, UI_INPUT_PARAM param, GFL_POINT *p_data );
//-------------------------------------
/// MSGWND
//=====================================
static void MSGWND_Init( MSGWND_WORK* p_wk, GFL_BMPWIN *p_bmpwin, GFL_FONT *p_font, GFL_MSGDATA *p_msg, GFL_TCBLSYS *p_tcbl, PRINT_QUE *p_que, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static void MSGWND_Print( MSGWND_WORK* p_wk, u32 strID, int wait, BOOL is_stream );
static void MSGWND_PrintEx( MSGWND_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, int wait, BOOL is_stream );
static BOOL MSGWND_IsEndMsg( MSGWND_WORK* p_wk );
//-------------------------------------
/// APPBAR
//=====================================
static void APPBAR_Init( APPBAR_WORK *p_wk, GFL_CLUNIT *p_clunit, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, APP_TASKMENU_RES *p_res, GAMEDATA *p_gdata,HEAPID heapID );
static void APPBAR_Exit( APPBAR_WORK *p_wk );
static void APPBAR_Main( APPBAR_WORK *p_wk, const UI_WORK *cp_ui, const SCROLL_WORK *cp_scroll );
static void APPBAR_MoveMain( APPBAR_WORK *p_wk );
static BOOL APPBAR_IsDecide( const APPBAR_WORK *cp_wk, APPBAR_WIN_LIST *p_select );
static BOOL APPBAR_IsWaitEffect( APPBAR_WORK *p_wk );
static void APPBAR_StopEffect( APPBAR_WORK *p_wk );
static void APPBAR_ReWrite( APPBAR_WORK *p_wk, HEAPID heapID );
//-------------------------------------
/// SCROLL
//=====================================
static void SCROLL_Init( SCROLL_WORK *p_wk, u8 font_frm, u8 back_frm, const GRAPHIC_WORK *cp_grp, const CONFIG_PARAM *cp_param, HEAPID heapID );
static void SCROLL_Exit( SCROLL_WORK *p_wk );
static void SCROLL_Main( SCROLL_WORK *p_wk, const UI_WORK *cp_ui, MSGWND_WORK *p_msg, GRAPHIC_WORK *p_graphic, APPBAR_WORK *p_appbar );
static CONFIG_LIST SCROLL_GetSelect( const SCROLL_WORK *cp_wk );
static void SCROLL_GetConfigParam( const SCROLL_WORK *cp_wk, CONFIG_PARAM *p_param );
static void SCROLL_SetConfigParamWireless( SCROLL_WORK *p_wk, u16 param );
static void SCROLL_SetConfigParamReport( SCROLL_WORK *p_wk, u16 param );
static void Scroll_ChangePlt( SCROLL_WORK *p_wk, BOOL is_decide_draw );
static void Scroll_Move( SCROLL_WORK *p_wk, int y_add );
static void Scroll_MoveRange( SCROLL_WORK *p_wk, int y_add, int min, int max );
static void Scroll_TouchItem( SCROLL_WORK *p_wk, const GFL_POINT *cp_pos );
static void Scroll_SelectItem( SCROLL_WORK *p_wk, s8 dir );
static CONFIG_LIST Scroll_PosToList( const SCROLL_WORK *cp_wk, const GFL_POINT *cp_pos );
static void Scroll_ChangePlt_Safe_Main( SCROLL_WORK *p_wk );
static void Scroll_ChangePlt_Safe_Check( SCROLL_WORK *p_wk, int y );
static BOOL Scroll_ChangePlt_Safe_IsOk( const SCROLL_WORK *cp_wk );
//-------------------------------------
/// CONFIRM
//=====================================
static void CONFIRM_Init( CONFIRM_WORK *p_wk, GFL_BMPWIN *p_bmpwin, GFL_FONT *p_font, GFL_MSGDATA *p_msg, GFL_TCBLSYS *p_tcbl, APP_TASKMENU_RES *p_res, PRINT_QUE *p_que, HEAPID heapID );
static void CONFIRM_Exit( CONFIRM_WORK *p_wk );
static void CONFIRM_Main( CONFIRM_WORK *p_wk );
static void CONFIRM_Start( CONFIRM_WORK *p_wk, int wait );
static CONFIRM_SELECT CONFIRM_Select( const CONFIRM_WORK *cp_wk );
//�ؒf�ł��Ȃ����b�Z�[�W
static void CONFIRM_PrintErrMessage( CONFIRM_WORK *p_wk, u32 strID, int wait );
static void CONFIRM_PrintMessage( CONFIRM_WORK *p_wk, u32 strID, int wait );
static BOOL CONFIRM_IsEndMessage( CONFIRM_WORK *p_wk );
//-------------------------------------
/// CONFIG_PARAM
//=====================================
static void CONFIGPARAM_Init( CONFIG_PARAM *p_wk, CONFIG *p_savedata );
static void CONFIGPARAM_Exit( CONFIG_PARAM *p_wk );
static void CONFIGPARAM_Save( const CONFIG_PARAM *cp_wk );
static void CONFIGPARAM_SetItem( CONFIG_PARAM *p_wk, CONFIG_ITEM item );
static void CONFIGPARAM_AddItem( CONFIG_PARAM *p_wk, CONFIG_LIST list, s8 dir );
static BOOL CONFIGPARAM_IsItemSetting( const CONFIG_PARAM *cp_wk, CONFIG_ITEM item );
static int CONFIGPARAM_GetMsgSpeed( const CONFIG_PARAM *cp_wk );
static CONFIG_ITEM CONFIGPARAM_GetChangeTiming( CONFIG_PARAM *p_wk );
static BOOL CONFIGPARAM_Compare( const CONFIG_PARAM *cp_wk1, const CONFIG_PARAM *cp_wk2 );
//-------------------------------------
/// SEQ
//=====================================
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param, SEQ_FUNCTION seq_function );
static void SEQ_Exit( SEQ_WORK *p_wk );
static void SEQ_Main( SEQ_WORK *p_wk );
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk );
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function );
static void SEQ_End( SEQ_WORK *p_wk );
//-------------------------------------
/// SEQFUNC
//=====================================
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_Decide( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_SetPreConfig( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_SetNowConfig( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
//-------------------------------------
/// ETC
//=====================================
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID );
static BOOL COLLISION_IsRectXPos( const GFL_RECT *cp_rect, const GFL_POINT *cp_pos );
static void _sound_mode_set( SCROLL_WORK *p_wk, s8 dir, int flag );

//=============================================================================
/**
 *          �O�����J
 */
//=============================================================================
//-------------------------------------
/// �R���t�B�O�v���Z�X
//=====================================
const GFL_PROC_DATA ConfigPanelProcData =
{
  CONFIG_PROC_Init,
  CONFIG_PROC_Main,
  CONFIG_PROC_Exit
};

//=============================================================================
/**
 *          �f�[�^
 */
//=============================================================================
//-------------------------------------
/// BG�ݒ�
//=====================================
enum
{
  GRAPHIC_BG_FRAME_BAR_M,       //�o�[��^�C�g��
  GRAPHIC_BG_FRAME_FONT_M,      //���C���t�H���g
  GRAPHIC_BG_FRAME_WND_M,       //�E�B���h�E
  GRAPHIC_BG_FRAME_DECIDE_M,    //�ŏI�m�F�E�B���h�E

  GRAPHIC_BG_FRAME_TEXT_S,      //�E�B���h�E
  GRAPHIC_BG_FRAME_BACK_S,      //�w�i

  GRAPHIC_BG_FRAME_MAX
};
static const struct
{
  u32                 frame;
  GFL_BG_BGCNT_HEADER bgcnt_header;
  u32                 mode;
} sc_bgsetup[GRAPHIC_BG_FRAME_MAX]  =
{
  //MAIN
  //GRAPHIC_BG_FRAME_BAR_M
  {
    GFL_BG_FRAME0_M,
    {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
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
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
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
      GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
    },
    GFL_BG_MODE_TEXT
  },
  //GRAPHIC_BG_FRAME_DECIDE_M
  {
    GFL_BG_FRAME3_M,
    {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
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
  {
    GFL_BG_FRAME1_S,
    {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    },
    GFL_BG_MODE_TEXT
  },

};
//�t���[���擾�p�}�N��
#define GRAPHIC_BG_GetFrame( x )  (sc_bgsetup[ x ].frame)

//-------------------------------------
/// �����p��BMPWIN�쐬�f�[�^
//    �ꊇ�ō쐬����Ƒ���Ȃ��̂Ōʂɍ����
//    1024�L�����ȓ��ɂ��Ă��܂�
//=====================================
static const struct
{
  u32     strID;  //�����ԍ�
  u8      x;      //BMPWIN���WX
  u8      y;      //BMPWIN���WY
  u8      w;      //BMPWIN���W��
  u8      h;      //BMPWIN���W����
} sc_config_menu_bmpwin_data[BMPWINID_CONFIG_MENU_MAX]  =
{

  //������
  {
    mes_config_sm01_00,
    //15,5,5,2
    14,5,6,2
  },
  //����
  {
    mes_config_sm01_01,
    //21,5,5,2
    20,5,6,2
  },
  //����
  {
    mes_config_sm01_02,
    26,5,6,2
    //27,5,5,2  //�{���̒l���P
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
#if 0
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
#endif
  //����
  {
    mes_config_sm07_00,
//    15,23,6,2
    15,20,6,2
  },
  //�����Ȃ�
  {
    mes_config_sm07_01,
//    24,23,7,2
    24,20,7,2
  },
  //���b�̑���
  {
    mes_config_menu01,
    1,5,12,2
  },
  //���퓬�A�j��
  {
    mes_config_menu02,
    1,8,10,2
  },
  //�������̃��[��
  {
    mes_config_menu03,
    1,11,12,2
  },
  //���T�E���h
  {
    mes_config_menu04,
    1,14,6,2
  },
  //���������[�h
  {
    mes_config_menu05,
    1,17,8,2
  },
#if 0
  //������
  {
    mes_config_menu06,
    1,20,10,2
  },
#endif
  //�����C�����X�ڑ��O���|�[�g
  {
    mes_config_menu07,
//    1,23,10,2
      1,20,10,2
  },
};

//-------------------------------------
/// ���j���[�̏��
//=====================================
static const struct
{
  u16 infoID;   //�Ή�����������b�Z�[�W
  u16 max;      //���ڂ̍ő吔
  u8 item[3]; //����
} sc_list_info[CONFIG_LIST_MAX] =
{
  //CONFIG_LIST_MSGSPEED,
  {
    mes_config_comment01,
    MSGSPEED_CONFIG_MAX,
    {
      CONFIG_ITEM_MSG_SLOW,
      CONFIG_ITEM_MSG_NORMAL,
      CONFIG_ITEM_MSG_FAST,
    },
  },
  //CONFIG_LIST_BTLEFF,
  {
    mes_config_comment02,
    WAZAEFF_MODE_MAX,
    {
      CONFIG_ITEM_BTLEFF_YES,
      CONFIG_ITEM_BTLEFF_NO,
    },
  },
  //CONFIG_LIST_BTLRULE,
  {
    mes_config_comment03,
    BATTLERULE_MAX,
    {
      CONFIG_ITEM_BTLRULE_IREKAE,
      CONFIG_ITEM_BTLRULE_KACHINUKI,
    },
  },
  //CONFIG_LIST_SND,
  {
    mes_config_comment04,
    SOUNDMODE_MAX,
    {
      CONFIG_ITEM_SND_STEREO,
      CONFIG_ITEM_SND_MONO,
    },
  },
  //CONFIG_LIST_STR,
  {
    mes_config_comment05,
    MOJIMODE_MAX,
    {
      CONFIG_ITEM_STR_HIRAGANA,
      CONFIG_ITEM_STR_KANJI,
    },
  },
#if 0
  //CONFIG_LIST_WIRELESS,
  {
    mes_config_comment06,
    NETWORK_SEARCH_MAX,
    {
      CONFIG_ITEM_WIRELESS_YES,
      CONFIG_ITEM_WIRELESS_NO,
    },
  },
#endif
  //CONFIG_LIST_REPORT,
  {
    mes_config_comment07,
    WIRELESSSAVE_MAX,
    {
      CONFIG_ITEM_REPORT_YES,
      CONFIG_ITEM_REPORT_NO,
    }
  },
};

//-------------------------------------
/// �A�C�e���̏��
//=====================================
static const struct
{
  CONFIG_LIST list;     //�Ή����郊�X�g
  u32         idx;      //���X�g�̃C���f�b�N�X
  GFL_POINT   obj_pos;  //OBJ�̈ʒu
  struct
  {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
  } touch_pos;//�^�b�`���W
}sc_item_info[CONFIG_ITEM_MAX]  =
{
  //  CONFIG_ITEM_MSG_SLOW,
  {
    CONFIG_LIST_MSGSPEED,
    MSGSPEED_SLOW,
    {
      15*8,0
    },
    {
      14*8,0,6*8,24
    },
  },
  //  CONFIG_ITEM_MSG_NORMAL,
  {
    CONFIG_LIST_MSGSPEED,
    MSGSPEED_NORMAL,
    {
      21*8-3,0
    },
    {
      20*8,0,6*8,24,
    }
  },
  //  CONFIG_ITEM_MSG_FAST,
  {
    CONFIG_LIST_MSGSPEED,
    MSGSPEED_FAST,
    {
      27*8-5,0
    },
    {
      26*8,0,6*8,24,
    }
  },
  //  CONFIG_ITEM_BTLEFF_YES,
  {
    CONFIG_LIST_BTLEFF,
    WAZAEFF_MODE_ON,
    {
      15*8,3*8
    },
    {
      14*8,3*8,9*8,24
    }
  },
  //  CONFIG_ITEM_BTLEFF_NO,
  {
    CONFIG_LIST_BTLEFF,
    WAZAEFF_MODE_OFF,
    {
      24*8,3*8
    },
    {
      23*8,3*8,9*8,24
    }
  },
  //  CONFIG_ITEM_BTLRULE_IREKAE,
  {
    CONFIG_LIST_BTLRULE,
    BATTLERULE_IREKAE,
    {
      15*8,6*8
    },
    {
      14*8,6*8,9*8,24
    }
  },
  //  CONFIG_ITEM_BTLRULE_KACHINUKI,
  {
    CONFIG_LIST_BTLRULE,
    BATTLERULE_KACHINUKI,
    {
      24*8,6*8
    },
    {
      23*8,6*8,9*8,24
    }
  },
  //  CONFIG_ITEM_SND_STEREO,
  {
    CONFIG_LIST_SND,
    SOUNDMODE_STEREO,
    {
      15*8,9*8
    },
    {
      14*8,9*8,9*8,24
    }
  },
  //  CONFIG_ITEM_SND_MONO,
  {
    CONFIG_LIST_SND,
    SOUNDMODE_MONO,
    {
      24*8,9*8
    },
    {
      23*8,9*8,9*8,24
    }
  },
  //  CONFIG_ITEM_STR_HIRAGANA,
  {
    CONFIG_LIST_STR,
    MOJIMODE_HIRAGANA,
    {
      15*8,12*8
    },
    {
      14*8,12*8,9*8,24
    }
  },
  //  CONFIG_ITEM_STR_KANJI,
  {
    CONFIG_LIST_STR,
    MOJIMODE_KANJI,
    {
      24*8,12*8
    },
    {
      23*8,12*8,9*8,24
    }
  },
#if 0
  //  CONFIG_ITEM_WIRELESS_YES,
  {
    CONFIG_LIST_WIRELESS,
    NETWORK_SEARCH_ON,
    {
      15*8,15*8
    },
    {
      14*8,15*8,9*8,24
    }
  },
  //  CONFIG_ITEM_WIRELESS_NO,
  {
    CONFIG_LIST_WIRELESS,
    NETWORK_SEARCH_OFF,
    {
      24*8,15*8
    },
    {
      23*8,15*8,9*8,24
    }
  },
#endif
  //  CONFIG_ITEM_REPORT_YES,
  {
    CONFIG_LIST_REPORT,
    WIRELESSSAVE_ON,
    {
      //15*8,18*8
      15*8,15*8
    },
    {
      //14*8,18*8,9*8,24
      14*8,15*8,9*8,24
    }
  },
  //  CONFIG_ITEM_REPORT_NO,
  {
    CONFIG_LIST_REPORT,
    WIRELESSSAVE_OFF,
    {
      //24*8,18*8
      24*8,15*8
    },
    {
      //23*8,18*8,9*8,24
      23*8,15*8,9*8,24
    }
  },
} ;

//-------------------------------------
/// APPBAR��̌���A��߂�{�^��
//=====================================
static const GFL_RECT sc_appbar_rect[APPBAR_WIN_MAX-1]  =
{
  {
    (APPBAR_WIN_X)*8, (APPBAR_MENUBAR_Y)*8,
    (APPBAR_WIN_X+APPBAR_WIN_W)*8, (APPBAR_MENUBAR_Y+APP_TASKMENU_PLATE_HEIGHT)*8
  },
  {
    (APPBAR_WIN_X+APPBAR_WIN_W)*8, (APPBAR_MENUBAR_Y)*8,
    (APPBAR_WIN_X+APPBAR_WIN_W*2)*8, (APPBAR_MENUBAR_Y+APP_TASKMENU_PLATE_HEIGHT)*8
  }
};


//=============================================================================
/**
 *          �v���Z�X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �R���t�B�O�v���Z�X������
 *
 *  @param  GFL_PROC *p_proc�v���Z�X
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param_adrs   �������[�N
 *  @param  *p_wk_adrs      ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CONFIG_PROC_Init( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs)
{
  //�I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

  //�q�[�v�쐬
  GFL_HEAP_CreateHeap(GFL_HEAPID_APP,HEAPID_CONFIG,0x21000);

  //�v���Z�X���[�N�쐬
  {
    CONFIG_WORK *p_wk;
    p_wk  = GFL_PROC_AllocWork( p_proc, sizeof(CONFIG_WORK), HEAPID_CONFIG);
    GFL_STD_MemClear( p_wk, sizeof(CONFIG_WORK) );
    p_wk->p_param = p_param_adrs;
    p_wk->p_param->is_exit  = FALSE;

    //�ŏ���GRAPHIC������(�p���b�g�ǂݍ��݂ŁA���L�̂��̂��㏑������邽��)
    GRAPHIC_Init( &p_wk->graphic, HEAPID_CONFIG );


    //���ʃ��W���[��������
    p_wk->p_font    = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
                        GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_CONFIG );
    p_wk->p_que     = PRINTSYS_QUE_Create( HEAPID_CONFIG );
    p_wk->p_menures = APP_TASKMENU_RES_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M),
                        CONFIG_BG_PAL_M_13, p_wk->p_font, p_wk->p_que, HEAPID_CONFIG );
    p_wk->p_confirmres  = APP_TASKMENU_RES_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M),
                        CONFIG_BG_PAL_M_09, p_wk->p_font, p_wk->p_que, HEAPID_CONFIG );
    p_wk->p_msg   = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                        NARC_message_config_dat, HEAPID_CONFIG );

    p_wk->p_tcbl    = GFL_TCBL_Init( HEAPID_CONFIG, HEAPID_CONFIG, 32, 32 );

    //���W���[��������
    {
      GAMEDATA *p_gdata = GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys );
      SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_gdata );
      CONFIG * p_sv = SaveData_GetConfig(p_sv_ctrl);
      CONFIGPARAM_Init( &p_wk->pre, p_sv );
    }
    SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeOut );  //�ŏ���FadeOut�V�[�P���X


    {
      GAMEDATA *p_gdata;
#ifdef GAMESYS_NONE_MOVE
      if( p_wk->p_param->p_gamesys == NULL )
      {
        OS_Printf( "\n!!!! GameSysPtr == NULL  !!!!\n" );
        p_gdata = NULL;
      }
      else
      {
        p_gdata = GAMESYSTEM_GetGameData(p_wk->p_param->p_gamesys);
      }
#else
      p_gdata = GAMESYSTEM_GetGameData(p_wk->p_param->p_gamesys);
#endif //GAMESYS_NONE_MOVE

      APPBAR_Init( &p_wk->appbar,
        GRAPHIC_GetUnit(&p_wk->graphic),
        p_wk->p_font,
        p_wk->p_que,
        p_wk->p_msg,
        p_wk->p_menures,
        p_gdata,
        HEAPID_CONFIG );
    }
    SCROLL_Init( &p_wk->scroll,
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_FONT_M),
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_WND_M),
        &p_wk->graphic,
        &p_wk->pre,
        HEAPID_CONFIG);
    UI_Init( &p_wk->ui, HEAPID_CONFIG );
    MSGWND_Init( &p_wk->info,
        GRAPHIC_GetBmpwin( &p_wk->graphic, BMPWINID_TEXT ),
        p_wk->p_font,
        p_wk->p_msg,
        p_wk->p_tcbl,
        p_wk->p_que,
        HEAPID_CONFIG );
    CONFIRM_Init( &p_wk->confirm,
        GRAPHIC_GetBmpwin( &p_wk->graphic, BMPWINID_CONFIRM ),
        p_wk->p_font,
        p_wk->p_msg,
        p_wk->p_tcbl,
        p_wk->p_confirmres,
        p_wk->p_que,
        HEAPID_CONFIG );
  }

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �R���t�B�O�v���Z�X�j��
 *
 *  @param  GFL_PROC *p_proc�v���Z�X
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param_adrs   �������[�N
 *  @param  *p_wk_adrs      ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CONFIG_PROC_Exit( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs)
{
  {
    CONFIG_WORK *p_wk = p_wk_adrs;

    //���W���[���j��
    CONFIRM_Exit( &p_wk->confirm );
    MSGWND_Exit( &p_wk->info );
    UI_Exit( &p_wk->ui );
    SCROLL_Exit( &p_wk->scroll );
    APPBAR_Exit( &p_wk->appbar );
    SEQ_Exit( &p_wk->seq );
    CONFIGPARAM_Exit( &p_wk->pre );

    //���ʃ��W���[���j��
    GFL_TCBL_Exit( p_wk->p_tcbl );
    GFL_MSG_Delete( p_wk->p_msg );
    APP_TASKMENU_RES_Delete( p_wk->p_confirmres );
    APP_TASKMENU_RES_Delete( p_wk->p_menures );
    PRINTSYS_QUE_Delete( p_wk->p_que );
    GFL_FONT_Delete( p_wk->p_font );

    //GRAPHIC�j��
    GRAPHIC_Exit( &p_wk->graphic );
  }

  //���[�N�G���A���
  GFL_PROC_FreeWork( p_proc );

  //�q�[�v���
  GFL_HEAP_DeleteHeap( HEAPID_CONFIG );

  //�I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �R���t�B�O�v���Z�X���C������
 *
 *  @param  GFL_PROC *p_proc�v���Z�X
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param_adrs   �������[�N
 *  @param  *p_wk_adrs      ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CONFIG_PROC_Main( GFL_PROC *p_proc,int *p_seq, void *p_param_adrs, void *p_wk_adrs)
{
  CONFIG_WORK *p_wk = p_wk_adrs;

  //�V�[�P���X
  SEQ_Main( &p_wk->seq );

  //�`��
  GRAPHIC_Main( &p_wk->graphic );

  //�v�����g
  PRINTSYS_QUE_Main( p_wk->p_que );

  //�^�X�N
  GFL_TCBL_Main( p_wk->p_tcbl );

  //�A�v���o�[
  APPBAR_MoveMain( &p_wk->appbar );

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
 *            GRAPHIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�N�ݒ�
 *
 *  @param  GRAPHIC_WORK *p_wk  ���[�N
 *  @param  heapID              �q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID )
{
  static const GFL_DISP_VRAM sc_vramSetTable =
  {
    GX_VRAM_BG_128_A,           // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_128_C,       // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_128_B,          // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_NONE,           // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_NONE,       // �e�N�X�`���p���b�g�X���b�g
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

  //�f�B�X�v���CON
  GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
  GFL_DISP_SetDispOn();

  //�\��
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();

  //�t�H���g������
  GFL_FONTSYS_Init();
  GFL_FONTSYS_SetColor( 0xF, 0xE, 0 );

  //���W���[��������
  GRAPHIC_BG_Init( &p_wk->bg, heapID );
  GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );

  //VBlankTask�o�^
  p_wk->p_vblank_task = GFUser_VIntr_CreateTCB(Graphic_VBlankTask, p_wk, 0 );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�N�j��
 *
 *  @param  GRAPHIC_WORK *p_wk  ���[�N
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

  //�f�t�H���g�F�֖߂�
  GFL_FONTSYS_SetDefaultColor();

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
 *  @brief  �O���t�B�b�N�`��
 *
 *  @param  GRAPHIC_WORK *p_wk  ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Main( GRAPHIC_WORK *p_wk )
{
  GRAPHIC_OBJ_Main( &p_wk->obj );
  GRAPHIC_BG_Main( &p_wk->bg );
}
//----------------------------------------------------------------------------
/**
 *  @brief  CLWK�擾
 *
 *  @param  const GRAPHIC_WORK *cp_wk ���[�N
 *  @param  id                        CLWKID
 *
 *  @return CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK * GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id )
{
  return GRAPHIC_OBJ_GetClwk( &cp_wk->obj, id );
}

//----------------------------------------------------------------------------
/**
 *  @brief  CLUNIT�擾
 *
 *  @param  const GRAPHIC_WORK *cp_wk   ���[�N
 *
 *  @return GFL_CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT *GRAPHIC_GetUnit( const GRAPHIC_WORK *cp_wk )
{
  return GRAPHIC_OBJ_GetUnit( &cp_wk->obj );
}
//----------------------------------------------------------------------------
/**
 *  @brief  BMPWIN�擾
 *
 *  @param  const GRAPHIC_WORK *cp_wk ���[�N
 *  @param  id                        BMPWIN��ID
 *
 *  @return BMPWIN
 */
//-----------------------------------------------------------------------------
static GFL_BMPWIN * GRAPHIC_GetBmpwin( const GRAPHIC_WORK *cp_wk, BMPWINID id )
{
  return GRAPHIC_BG_GetBmpwin( &cp_wk->bg, id );
}
//----------------------------------------------------------------------------
/**
 *  @brief  BMPWIN�ɕ����`��
 *
 *  @param  GRAPHIC_WORK *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_PrintBmpwin( GRAPHIC_WORK *p_wk )
{
  GRAPHIC_BG_PrintBmpwin( &p_wk->bg );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �t�F�[�h�J�n
 *
 *  @param  GRAPHIC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_StartPalleteFade( GRAPHIC_WORK *p_wk )
{
  GRAPHIC_BG_StartPalletFade( &p_wk->bg );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�NVBLANK�֐�
 *
 *  @param  GRAPHIC_WORK *p_wk  ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work )
{
  GRAPHIC_WORK *p_wk  = p_work;
  GRAPHIC_BG_VBlankFunction( &p_wk->bg );
  GRAPHIC_OBJ_VBlankFunction( &p_wk->obj );
}

//=============================================================================
/**
 *            BG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  BG  ������
 *
 *  @param  GRAPHIC_BG_WORK *p_wk ���[�N
 *  @param  heapID                �q�[�vID
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
    static const GFL_BG_SYS_HEADER sc_bgsys_header  =
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
  GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M), VISIBLE_OFF );

  GFL_BG_FillCharacter( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), 0, 1,  0 );
  GFL_BG_FillCharacter( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M), 0, 1,  0 );


  //�f�ޓǂݍ���
  {
    ARCHANDLE * p_handle  = GFL_ARC_OpenDataHandle( ARCID_CONFIG_GRA, heapID );

    //���C�����-----------------------------------
    //PLT
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_config_gra_est_bar_01_NCLR,
        PALTYPE_MAIN_BG, CONFIG_BG_PAL_M_00*0x20, 0, heapID );

    //titlebar--------------
    //CHR
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_config_gra_ue_bar_01_NCGR,
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_config_gra_ue_bar_01_NSCR,
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, 0, FALSE, heapID );

    //WND------------------
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_config_gra_est_bar_01_NCGR,
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_WND_M), 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_config_gra_est_bar_01_NSCR,
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_WND_M), 0, 0, FALSE, heapID );

    //CONFIRM-------------
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_config_gra_ue_flame_01_NCGR,
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M), 1, 0, 0, heapID );

    GFL_ARCHDL_UTIL_TransVramPaletteEx( p_handle, NARC_config_gra_est_uegamen_01_NCLR,
        PALTYPE_MAIN_BG, 4*0x20, CONFIG_BG_PAL_M_15*0x20, 0x20, heapID );

    //TITLE
    p_wk->ncg_buf = GFL_ARCHDL_UTIL_LoadBGCharacter( p_handle ,
      NARC_config_gra_ue_bar_window_NCGR, FALSE, &p_wk->ncg_data, heapID );


    //�T�u���-----------------------------------
    //PLT
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_config_gra_est_uegamen_01_NCLR,
        PALTYPE_SUB_BG, CONFIG_BG_PAL_S_00*0x20, 0, heapID );

    //BACK---------
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_config_gra_est_uegamen_01_NCGR,
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_S), 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_config_gra_est_uegamen_01_NSCR,
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_S), 0, 0, FALSE, heapID );

    //TEXT-----------
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_config_gra_ue_flame_01_NCGR,
        GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), 1, 0, 0, heapID );


    GFL_ARC_CloseDataHandle( p_handle );
  }

  //�p���b�g�ǂ݂��ݕۑ�
  {
    void *p_buff;
    u16 *p_raw_data;
    NNSG2dPaletteData* p_plt_data;

    p_buff  = GFL_ARC_UTIL_LoadPalette( ARCID_CONFIG_GRA, NARC_config_gra_est_bar_01_NCLR, &p_plt_data, heapID );
    p_raw_data  = p_plt_data->pRawData;
    GFL_STD_MemCopy( &p_raw_data[ 0x10 * CONFIG_BG_PAL_M_07] , p_wk->plt_color, sizeof(GXRgb)*0x10 );
    GFL_HEAP_FreeMemory( p_buff );
  }

  //BMPWIN�쐬
  {
    int i;
    for( i = 0; i < BMPWINID_CONFIG_MENU_MAX; i++ )
    {
      p_wk->p_bmpwin[i] = GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_FONT_M),
          sc_config_menu_bmpwin_data[i].x, sc_config_menu_bmpwin_data[i].y,
          sc_config_menu_bmpwin_data[i].w, sc_config_menu_bmpwin_data[i].h,
          0, GFL_BMP_CHRAREA_GET_B );
    }
    p_wk->p_bmpwin[BMPWINID_TEXT] = GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S),
        INFOWND_X, INFOWND_Y, INFOWND_W, INFOWND_H,
        CONFIG_BG_PAL_S_04, GFL_BMP_CHRAREA_GET_B );

    p_wk->p_bmpwin[BMPWINID_CONFIRM]  = GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M),
        CONFIRMWND_X, CONFIRMWND_Y, CONFIRMWND_W, CONFIRMWND_H,
        CONFIG_BG_PAL_M_15, GFL_BMP_CHRAREA_GET_B );

    p_wk->p_bmpwin[BMPWINID_TITLE]  = GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M),
        TITLEWND_X, TITLEWND_Y, TITLEWND_W, TITLEWND_H,
        CONFIG_BG_PAL_M_08, GFL_BMP_CHRAREA_GET_B );

  }
  GRAPHIC_BG_PrintBmpwin( p_wk );

  GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), FALSE );
}
//----------------------------------------------------------------------------
/**
 *  @brief  BG  �j��
 *
 *  @param  GRAPHIC_BG_WORK *p_wk   ���[�N
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
    GFL_HEAP_FreeMemory( p_wk->ncg_buf );
    GFL_BG_FillCharacterRelease( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), 1, 0 );
    GFL_BG_FillCharacterRelease( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M), 1, 0 );
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
 *  @brief  BG  ���C������
 *
 *  @param  GRAPHIC_BG_WORK *p_wk   ���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Main( GRAPHIC_BG_WORK *p_wk )
{
  //�I��F�̃p���b�g�t�F�[�h
  Graphic_Bg_PalletFadeMain( &p_wk->trans_color[0], &p_wk->pltfade_cnt[0],
      PLTFADE_SELECT_ADD, CONFIG_BG_PAL_M_07, 1, p_wk->plt_color[0xb], p_wk->plt_color[0xc] );
}
//----------------------------------------------------------------------------
/**
 *  @brief  BG  BMPWIN�擾
 *
 *  @param  const GRAPHIC_BG_WORK *cp_wk  ���[�N
 *  @param  id  BMPWIN��ID
 *
 *  @return BMPWIN
 */
//-----------------------------------------------------------------------------
static GFL_BMPWIN *GRAPHIC_BG_GetBmpwin( const GRAPHIC_BG_WORK *cp_wk, BMPWINID id )
{
  return cp_wk->p_bmpwin[ id ];
}
//----------------------------------------------------------------------------
/**
 *  @brief  BMPWIN�ɕ�����\��t����
 *
 *  @param  GRAPHIC_BG_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_PrintBmpwin( GRAPHIC_BG_WORK *p_wk )
{
  //BMPWIN�쐬��������\��t����
  {
    int i;
    GFL_FONT  *p_font;
    STRBUF    *p_strbuf;
    GFL_MSGDATA *p_msg;
    s32 x;

    p_font  = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
        GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_CONFIG );

    p_msg   = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
        NARC_message_config_dat, HEAPID_CONFIG );


    for( i = 0; i < BMPWINID_CONFIG_MENU_MAX; i++ )
    {
      x = 0;
      //�����̑����������イ���イ�Ȃ̂ŁA�E�̂�񂩂�O��Ă���̂�
      //���P�L����������聦�P�A�E�ɏC�����Ă��܂�
      switch(i )
      {
      case CONFIG_ITEM_MSG_SLOW:
        x = 7;
        break;
      case CONFIG_ITEM_MSG_NORMAL:
        x = 4;
        break;
      case CONFIG_ITEM_MSG_FAST:
        x = 2;
        break;
      }

      p_strbuf  = GFL_MSG_CreateString( p_msg, sc_config_menu_bmpwin_data[i].strID );

      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );

      PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), x, 0, p_strbuf, p_font );

      GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->p_bmpwin[i] );

      GFL_STR_DeleteBuffer(p_strbuf);
    }

    //�^�C�g��
    //BMPWIN�ɊG�̃f�[�^�]��
    {
      u32 srcAdr, dstAdr;
      srcAdr  = (u32)(p_wk->ncg_data->pRawData);
      dstAdr  = (u32)(GFL_BMP_GetCharacterAdrs(GFL_BMPWIN_GetBmp( p_wk->p_bmpwin[BMPWINID_TITLE] )));
      GFL_STD_MemCopy32( (void*)srcAdr, (void*)dstAdr, TITLEWND_W*TITLEWND_H*0x20 );
    }

    //�����\��
    p_strbuf  = GFL_MSG_CreateString( p_msg, mes_config_title );
    {
      //�Z���^�[����
      int x;
      x = GFL_BMPWIN_GetSizeX( p_wk->p_bmpwin[BMPWINID_TITLE] )*4;
      x -= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
      PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[BMPWINID_TITLE]), x, 4, p_strbuf, p_font, PRINTSYS_LSB_Make(0xf,0xE,1) );
    }
    GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->p_bmpwin[BMPWINID_TITLE] );
    GFL_STR_DeleteBuffer(p_strbuf);


    GFL_MSG_Delete( p_msg );
    GFL_FONT_Delete( p_font );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  Pallet�t�F�[�h�J�n
 *
 *  @param  GRAPHIC_BG_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_StartPalletFade( GRAPHIC_BG_WORK *p_wk )
{
  p_wk->pltfade_cnt[0]  = 0;
}
//----------------------------------------------------------------------------
/**
 *  @brief  BG  VBlank�֐�
 *
 *  @param  GRAPHIC_BG_WORK *p_wk   ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk )
{
  GFL_BG_VBlankFunc();
}
//----------------------------------------------------------------------------
/**
 *  @brief  BG�@�p���b�g�t�F�[�h
 *
 *  @param  u16 *p_buff �F�ۑ��o�b�t�@�iVBlank�]���̂��߁j
 *  @param  *p_cnt      �J�E���^�o�b�t�@
 *  @param  add         �J�E���^���Z�l
 *  @param  plt_num     �p���b�g�c�ԍ�
 *  @param  plt_col     �p���b�g���ԍ�
 *  @param  start       �J�n�F
 *  @param  end         �I���F
 */
//-----------------------------------------------------------------------------
static void Graphic_Bg_PalletFadeMain( u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{
  //�p���b�g�A�j��
  if( *p_cnt + add >= 0x10000 )
  {
    *p_cnt = *p_cnt+add-0x10000;
  }
  else
  {
    *p_cnt += add;
  }
  {
    //1�`0�ɕϊ�
    const fx16 cos = (FX_CosIdx(*p_cnt)+FX16_ONE)/2;
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

    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        plt_num * 32 + plt_col *2 ,
                                        p_buff, 2 );
  }

}
//=============================================================================
/**
 *        OBJ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  OBJ�`�� ������
 *
 *  @param  GRAPHIC_OBJ_WORK *p_wk      ���[�N
 *  @param  GFL_DISP_VRAM* cp_vram_bank �o���N�e�[�u��
 *  @param  heapID                      �q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID )
{
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );

  //�V�X�e���쐬
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, cp_vram_bank, heapID );
  p_wk->p_clunit  = GFL_CLACT_UNIT_Create( 128, 0, heapID );
  GFL_CLACT_UNIT_SetDefaultRend( p_wk->p_clunit );

  //�\��
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  //�ǂݍ���
  {
    ARCHANDLE *p_handle;

    p_handle  = GFL_ARC_OpenDataHandle( ARCID_CONFIG_GRA, heapID );

    p_wk->reg_id[OBJRESID_ITEM_PLT] = GFL_CLGRP_PLTT_Register( p_handle,
        NARC_config_gra_est_obj_01_NCLR, CLSYS_DRAW_MAIN, CONFIG_OBJ_PAL_M_00*0x20, heapID );

    p_wk->reg_id[OBJRESID_ITEM_CHR] = GFL_CLGRP_CGR_Register( p_handle,
        NARC_config_gra_est_obj_01_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

    p_wk->reg_id[OBJRESID_ITEM_CEL] = GFL_CLGRP_CELLANIM_Register( p_handle,
        NARC_config_gra_est_obj_01_NCER, NARC_config_gra_est_obj_01_NANR, heapID );

    p_wk->reg_id[OBJRESID_BAR_CHR]  = GFL_CLGRP_CGR_Register( p_handle,
        NARC_config_gra_shita_bar_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

    p_wk->reg_id[OBJRESID_BAR_CEL]  = GFL_CLGRP_CELLANIM_Register( p_handle,
        NARC_config_gra_shita_bar_NCER, NARC_config_gra_shita_bar_NANR, heapID );

    GFL_ARC_CloseDataHandle( p_handle );
  }

  //CLWK�쐬
  {
    int i;
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.bgpri    = 2;
    cldata.softpri  = 2;
    for( i = CLWKID_ITEM_TOP; i < CLWKID_ITEM_END; i++ )
    {
      cldata.pos_x  = sc_item_info[i-CLWKID_ITEM_TOP].obj_pos.x;
      cldata.pos_y  = sc_item_info[i-CLWKID_ITEM_TOP].obj_pos.y;
      p_wk->p_clwk[i] =   GFL_CLACT_WK_Create( p_wk->p_clunit,
          p_wk->reg_id[OBJRESID_ITEM_CHR],
          p_wk->reg_id[OBJRESID_ITEM_PLT],
          p_wk->reg_id[OBJRESID_ITEM_CEL],
          &cldata,
          CLSYS_DEFREND_MAIN,
          heapID );
      GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[i], FALSE );
    }

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x    = 128;
    cldata.pos_y    = 96;
    cldata.bgpri    = 2;
    cldata.softpri  = 1;
    p_wk->p_clwk[CLWKID_DOWNBAR]  =   GFL_CLACT_WK_Create( p_wk->p_clunit,
          p_wk->reg_id[OBJRESID_BAR_CHR],
          p_wk->reg_id[OBJRESID_ITEM_PLT],
          p_wk->reg_id[OBJRESID_BAR_CEL],
          &cldata,
          CLSYS_DEFREND_MAIN,
          heapID );
    GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk[CLWKID_DOWNBAR], CONFIG_OBJ_PAL_M_06, CLWK_PLTTOFFS_MODE_PLTT_TOP );
  }

  if( GFL_NET_IsInit() )
  { 
    GFL_NET_ReloadIconTopOrBottom( TRUE, heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  OBJ�`�� �j��
 *
 *  @param  GRAPHIC_OBJ_WORK *p_wk  ���[�N
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
    GFL_CLGRP_PLTT_Release( p_wk->reg_id[OBJRESID_ITEM_PLT] );
    GFL_CLGRP_CGR_Release( p_wk->reg_id[OBJRESID_ITEM_CHR] );
    GFL_CLGRP_CELLANIM_Release( p_wk->reg_id[OBJRESID_ITEM_CEL] );
    GFL_CLGRP_CGR_Release( p_wk->reg_id[OBJRESID_BAR_CHR] );
    GFL_CLGRP_CELLANIM_Release( p_wk->reg_id[OBJRESID_BAR_CEL] );
  }


  //�V�X�e���j��
  GFL_CLACT_UNIT_Delete( p_wk->p_clunit );
  GFL_CLACT_SYS_Delete();
  GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  OBJ�`�� ���C������
 *
 *  @param  GRAPHIC_OBJ_WORK *p_wk  ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk )
{
  if( p_wk->anm_idx++ > 8*12 )
  {
    p_wk->anm_idx = 0;
  }

  //OBJ�̃A�j���𓯊�������
  {
    int i;
    for( i = CLWKID_ITEM_TOP; i < CLWKID_ITEM_END; i++ )
    {
      GFL_CLACT_WK_SetAnmIndex( p_wk->p_clwk[ i ], p_wk->anm_idx/12 );
    }
  }


  GFL_CLACT_SYS_Main();

}
//----------------------------------------------------------------------------
/**
 *  @brief  OBJ�`�� V�u�����N����
 *
 *  @param  GRAPHIC_OBJ_WORK *p_wk  ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk )
{
  GFL_CLACT_SYS_VBlankFunc();
}
//----------------------------------------------------------------------------
/**
 *  @brief  OBJ�`�� CLWK�擾
 *
 *  @param  const GRAPHIC_OBJ_WORK *cp_wk ���[�N
 *  @param  id                            CLWK��ID
 *
 *  @return CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id )
{
  GF_ASSERT( id < CLWKID_MAX );
  return cp_wk->p_clwk[id];
}
//----------------------------------------------------------------------------
/**
 *  @brief  OBJ�`�� CLUNIT�̎擾
 *
 *  @param  const GRAPHIC_OBJ_WORK *cp_wk   ���[�N
 *
 *  @return CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT *GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk )
{
  return cp_wk->p_clunit;
}
//=============================================================================
/**
 *          UI
 *            UI�͓��͂𒊏ۉ����ĕԂ�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  ���[�U�[���͏�����
 *
 *  @param  UI_WORK *p_wk ���[�N
 *  @param  heapID        �q�[�vID
 */
//-----------------------------------------------------------------------------
static void UI_Init( UI_WORK *p_wk, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���[�U�[���͔j��
 *
 *  @param  UI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UI_Exit( UI_WORK *p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���[�U�[���̓��C������
 *
 *  @param  UI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UI_Main( UI_WORK *p_wk )
{
  u32 x, y;

  //��[���Z�b�g
  p_wk->input = UI_INPUT_NONE;

  //�^�b�`����
  if( GFL_UI_TP_GetPointTrg( &x, &y ) )
  {
    p_wk->input = UI_INPUT_TOUCH;
    p_wk->tp_pos.x  = x;
    p_wk->tp_pos.y  = y;
    p_wk->slide_start.x = x;
    p_wk->slide_start.y = y;
    p_wk->is_start_slide  = TRUE;

    if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_KEY )
    {
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    }
  }
  else if( GFL_UI_TP_GetPointCont( &x, &y ) && p_wk->is_start_slide )
  {
    if( MATH_IAbs( y - p_wk->tp_pos.y ) >= UI_SLIDE_DISTANCE_START )
    {
      p_wk->input = UI_INPUT_SLIDE;
      p_wk->slide.x   = x - p_wk->slide_start.x;
      p_wk->slide.y   = y - p_wk->slide_start.y;
      p_wk->slide_start.x = x;
      p_wk->slide_start.y = y;
    }
  }
  else
  {
    p_wk->is_start_slide  = FALSE;
  }

  
  //�L�[����
  { 
    if( GFL_UI_KEY_GetTrg() )
    { 
      if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_TOUCH && !(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_B|PAD_BUTTON_Y|PAD_BUTTON_X) ) )
      {
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        p_wk->input = UI_INPUT_KEY;

        PMSND_PlaySE( CONFIG_SE_MOVE );
        //�L�[���͂������ɏI��
        return ;
      } 
    }

    if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
    {
      p_wk->input = UI_INPUT_TRG_UP;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
    {
      p_wk->input = UI_INPUT_TRG_DOWN;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
    {
      p_wk->input = UI_INPUT_TRG_LEFT;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
    {
      p_wk->input = UI_INPUT_TRG_RIGHT;
    }
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
    {
      p_wk->input = UI_INPUT_CONT_UP;
    }
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
    {
      p_wk->input = UI_INPUT_CONT_DOWN;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      p_wk->input = UI_INPUT_TRG_DECIDE;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    {
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      p_wk->input = UI_INPUT_TRG_CANCEL;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
    {
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      p_wk->input = UI_INPUT_TRG_Y;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY ); 
      p_wk->input = UI_INPUT_TRG_X;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���͂��擾
 *
 *  @param  const UI_WORK *cp_wk  ���[�N
 *  @param  *p_data   ��ޕʂ̎󂯎��f�[�^
 *    UI_INPUT_TOUCH�Ȃ�� TRG���W
 *    UI_INPUT_SLIDE�Ȃ�� �ړ��I�t�Z�b�g
 *
 *  @return ���͂̎��
 */
//-----------------------------------------------------------------------------
static UI_INPUT UI_GetInput( const UI_WORK *cp_wk )
{
  return cp_wk->input;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �����擾
 *
 *  @param  const UI_WORK *cp_wk  ���[�N
 *  @param  param                 ���̎��
 *  @param  *p_data               ���󂯎��
 */
//-----------------------------------------------------------------------------
static void UI_GetParam( const UI_WORK *cp_wk, UI_INPUT_PARAM param, GFL_POINT *p_data )
{
  if( p_data )
  {
    switch( param )
    {
    case UI_INPUT_PARAM_TRGPOS:
      *p_data = cp_wk->tp_pos;
      break;

    case UI_INPUT_PARAM_SLIDEPOS:
      *p_data = cp_wk->slide;
      break;

    case UI_INPUT_PARAM_SLIDENOW:
      *p_data = cp_wk->slide_start;
      break;
    }
  }
}
//=============================================================================
/**
 *          MSGWND
 */
//=============================================================================
//-------------------------------------
/// 
//=====================================
typedef enum
{
  MSGWND_PRINT_TYPE_NONE,
  MSGWND_PRINT_TYPE_STREAM, //�v�����g�X�g���[��
  MSGWND_PRINT_TYPE_FILL,   //�ꊇ�v�����g
} MSGWND_PRINT_TYPE;

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  ������
 *
 *  @param  MSGWND_WORK* p_wk ���[�N
 *  @param  bgframe     BG�t���[��
 *  @param  x           X
 *  @param  y           Y
 *  @param  w           ��
 *  @param  h           ����
 *  @param  plt         �p���b�g�ԍ�
 *  @param  heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
static void MSGWND_Init( MSGWND_WORK* p_wk, GFL_BMPWIN *p_bmpwin, GFL_FONT *p_font, GFL_MSGDATA *p_msg, GFL_TCBLSYS *p_tcbl, PRINT_QUE *p_que, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
  p_wk->clear_chr = 0x4;
  p_wk->p_bmpwin  = p_bmpwin;
  p_wk->heapID    = heapID;

  p_wk->p_font    = p_font;
  p_wk->p_msg     = p_msg;
  p_wk->p_strbuf  = GFL_STR_CreateBuffer( 255, heapID );
  p_wk->p_tcbl    = p_tcbl;
  p_wk->p_stream  = NULL;
  p_wk->p_que     = p_que;
  p_wk->print_update  = MSGWND_PRINT_TYPE_NONE;

  APP_PRINTSYS_COMMON_PrintStreamInit( &p_wk->trg_work, APP_PRINTSYS_COMMON_TYPE_BOTH|APP_PRINTSYS_COMMON_TYPE_NOTSKIP );
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_OFF,1, GFL_BMPWIN_GetPalette(p_bmpwin) );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );

  GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  �j��
 *
 *  @param  MSGWND_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MSGWND_Exit( MSGWND_WORK* p_wk )
{
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }

  GFL_STR_DeleteBuffer( p_wk->p_strbuf );
  GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  �v�����g�J�n
 *
 *  @param  MSGWND_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MSGWND_Print( MSGWND_WORK* p_wk, u32 strID, int wait, BOOL is_stream )
{
  MSGWND_PrintEx( p_wk, p_wk->p_msg, strID, wait, is_stream );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  �v�����g�J�n  MSGDATA�w���
 *
 *  @param  MSGWND_WORK* p_wk ���[�N
 *  @param  *p_msg  ���b�Z�[�W
 *  @param  strID ����
 *  @param  wait  ���b�Z�[�W�X�s�[�h
 *  @param  is_stream TRUE�Ȃ�΃X�g���[��  is_stream�Ȃ�Έꊇ�\��
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintEx( MSGWND_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, int wait, BOOL is_stream )
{ 
  //��[����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );

  //������쐬
  GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //�X�g���[���j��
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }

  //�����`�悪�Ⴄ
  if( !is_stream )
  { 
    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );

    p_wk->print_update  = MSGWND_PRINT_TYPE_FILL;
  }
  else
  { 

    //�X�g���[���J�n
    p_wk->p_stream  = PRINTSYS_PrintStream( p_wk->p_bmpwin, 0, 0, p_wk->p_strbuf,
        p_wk->p_font, wait, p_wk->p_tcbl, 0, p_wk->heapID, p_wk->clear_chr );

    p_wk->print_update  = MSGWND_PRINT_TYPE_STREAM;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E�X�g���[���I���҂�
 *
 *  @param  const MSGWND_WORK* p_wk  ���[�N
 *
 *  @return TRUE�ŃX�g���[���I��  FALSE��������
 */
//-----------------------------------------------------------------------------
static BOOL MSGWND_IsEndMsg( MSGWND_WORK* p_wk )
{
  switch( p_wk->print_update )
  { 
  default:
  case MSGWND_PRINT_TYPE_NONE:
    return TRUE;

  case MSGWND_PRINT_TYPE_FILL:
    return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );

  case MSGWND_PRINT_TYPE_STREAM:
    return APP_PRINTSYS_COMMON_PrintStreamFunc( &p_wk->trg_work, p_wk->p_stream ); 
  }
}
//=============================================================================
/**
 *          APPBAR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  APPBAR������
 *
 *  @param  APPBAR_WORK *p_wk ���[�N
 *  @param  *p_clunit ���j�b�g
 *  @param  *p_font   �t�H���g
 *  @param  p_que     �v�����g�L���[
 *  @param  p_msg     ���b�Z�[�W
 *  @param  p_res     �^�X�N���j���[�p�f��
 *  @param  p_gdata   Y�{�^���o�^�̂��߂ɃQ�[���f�[�^�ێ�
 *  @param  heapID    �q�[�v
 *
 */
//-----------------------------------------------------------------------------
static void APPBAR_Init( APPBAR_WORK *p_wk, GFL_CLUNIT *p_clunit, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, APP_TASKMENU_RES *p_res, GAMEDATA *p_gdata, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk, sizeof(APPBAR_WORK) );
  p_wk->select  = APPBAR_WIN_NULL;
  p_wk->p_font  = p_font;
  p_wk->p_que   = p_que;
  p_wk->p_res   = p_res;
  p_wk->p_msg   = p_msg;
  p_wk->p_gdata = p_gdata;

  {
    TOUCHBAR_ITEM_ICON  item[]  =
    {
      {
        TOUCHBAR_ICON_CLOSE,
        {
          TOUCHBAR_ICON_X_02,
          TOUCHBAR_ICON_Y
        }
      },
      {
        TOUCHBAR_ICON_CHECK,
        {
          TOUCHBAR_ICON_X_01,
          TOUCHBAR_ICON_Y_CHECK
        }
      }
    };
    TOUCHBAR_SETUP setup;

    GFL_STD_MemClear( &setup, sizeof(TOUCHBAR_SETUP) );
    setup.p_item    = item;
    setup.item_num  = NELEMS(item);
    setup.p_unit    = p_clunit;
    setup.obj_plt   = CONFIG_OBJ_PAL_M_08;
    setup.mapping   = APP_COMMON_MAPPING_128K;
    setup.is_notload_bg = TRUE;

    p_wk->p_touch = TOUCHBAR_Init( &setup, heapID );
  }

  APPBAR_ReWrite( p_wk, heapID );

  //Y�{�^���o�^����Ă���΁A�A�C�R����ON

#ifdef GAMESYS_NONE_MOVE
  if( p_wk->p_gdata == NULL )
  {
    OS_Printf( "\n!!!! GameSysPtr == NULL  !!!!\n" );
    return ;
  }
#endif //GAMESYS_NONE_MOVE

  if( GAMEDATA_GetShortCut( p_wk->p_gdata, SHORTCUT_ID_CONFIG ))
  {
    TOUCHBAR_SetFlip( p_wk->p_touch, TOUCHBAR_ICON_CHECK, TRUE );
  }
  else
  {
    TOUCHBAR_SetFlip( p_wk->p_touch, TOUCHBAR_ICON_CHECK, FALSE );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  APPBAR�j��
 *
 *  @param  APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void APPBAR_Exit( APPBAR_WORK *p_wk )
{

  TOUCHBAR_Exit( p_wk->p_touch );

  {
    int i;
    for( i = 0; i < APPBAR_WIN_MAX-1; i++ )
    {
      GFL_STR_DeleteBuffer( p_wk->item[i].str );
      APP_TASKMENU_WIN_Delete( p_wk->p_win[i] );
    }
  }

  GFL_STD_MemClear( p_wk, sizeof(APPBAR_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  APPBAR���C������
 *
 *  @param  APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void APPBAR_Main( APPBAR_WORK *p_wk, const UI_WORK *cp_ui, const SCROLL_WORK *cp_scroll )
{
  GFL_POINT   pos;
  UI_INPUT    input;
  CONFIG_LIST select;
  BOOL is_update  = FALSE;


  //���莞�͓��͂ł��Ȃ�
  if( p_wk->is_decide )
  { 
    input = UI_INPUT_NONE;
  }
  else
  { 
    //���͎擾
    input = UI_GetInput( cp_ui );
  }

  //�X�N���[���̑I���擾
  select  = SCROLL_GetSelect( cp_scroll );

  switch( input )
  {
  case UI_INPUT_KEY:
    //�L�[�ؑ֎���TOUCHBAR_Main�̏����������Ȃ�
    return;

  case UI_INPUT_TOUCH:
    UI_GetParam( cp_ui, UI_INPUT_PARAM_TRGPOS, &pos );
    //�^�b�`�����Ȃ�΁A
    //�����Ă���΁A�I��
    if( COLLISION_IsRectXPos( &sc_appbar_rect[APPBAR_WIN_DECIDE], &pos )
        && p_wk->select != APPBAR_WIN_DECIDE )
    {
      GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), FALSE );
      PMSND_PlaySE( CONFIG_SE_DECIDE );
      p_wk->select = APPBAR_WIN_DECIDE;
      p_wk->is_decide = TRUE;
    }
    else if(  COLLISION_IsRectXPos( &sc_appbar_rect[APPBAR_WIN_CANCEL], &pos )
        && p_wk->select != APPBAR_WIN_CANCEL )
    {
      GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), FALSE );
      PMSND_PlaySE( CONFIG_SE_CANCEL );
      p_wk->select  = APPBAR_WIN_CANCEL;
      p_wk->is_decide = TRUE;
    }
    else if( p_wk->select != APPBAR_WIN_NULL )
    {
      p_wk->select  = APPBAR_WIN_NULL;
    }
    is_update = TRUE;
    break;

  case UI_INPUT_TRG_UP:
    /* fallthrough */
  case UI_INPUT_CONT_UP:
    /* fallthrough */
  case UI_INPUT_TRG_LEFT:
    /* fallthrough */
  case UI_INPUT_TRG_DOWN:
    /* fallthrough */
  case UI_INPUT_CONT_DOWN:
    /* fallthrough */
  case UI_INPUT_TRG_RIGHT:
    switch( select )
    {
    case CONFIG_LIST_DECIDE:
      p_wk->select  = APPBAR_WIN_DECIDE;
      break;
    case CONFIG_LIST_CANCEL:
      p_wk->select  = APPBAR_WIN_CANCEL;
      break;
    default:
      p_wk->select  = APPBAR_WIN_NULL;
    }
    is_update = TRUE;
    break;

  case UI_INPUT_TRG_DECIDE:
    if( p_wk->select != APPBAR_WIN_NULL )
    {
      if( p_wk->select == APPBAR_WIN_CANCEL )
      { 
        PMSND_PlaySE( CONFIG_SE_CANCEL );
      }
      else
      { 
        PMSND_PlaySE( CONFIG_SE_DECIDE );
      }
      p_wk->is_decide = TRUE;
      is_update = TRUE;
    }
    else
    { 
      p_wk->select    = APPBAR_WIN_DECIDE;
      PMSND_PlaySE( CONFIG_SE_DECIDE );
      p_wk->is_decide = TRUE;
      is_update = TRUE;
    }
    break;

  case UI_INPUT_TRG_CANCEL:
    PMSND_PlaySE( CONFIG_SE_CANCEL );
    p_wk->select    = APPBAR_WIN_CANCEL;
    p_wk->is_decide = TRUE;
    is_update = TRUE;
    break;
  }

  //�I����
  if( is_update )
  {
    if( p_wk->select == APPBAR_WIN_NULL )
    {
      APP_TASKMENU_WIN_SetActive( p_wk->p_win[APPBAR_WIN_DECIDE], FALSE );
      APP_TASKMENU_WIN_SetActive( p_wk->p_win[APPBAR_WIN_CANCEL], FALSE );
    }
    else
    {
      if( p_wk->is_decide )
      {
        APP_TASKMENU_WIN_SetActive( p_wk->p_win[p_wk->select], FALSE );
        APP_TASKMENU_WIN_SetDecide( p_wk->p_win[p_wk->select], TRUE );
        APP_TASKMENU_WIN_SetActive( p_wk->p_win[p_wk->select^1], FALSE );
      }
      else
      {
        APP_TASKMENU_WIN_SetActive( p_wk->p_win[p_wk->select], TRUE );
        APP_TASKMENU_WIN_SetActive( p_wk->p_win[p_wk->select^1], FALSE );
      }
    }
  }

  //�^�b�`�o�[���C��
  TOUCHBAR_Main( p_wk->p_touch );

  switch( TOUCHBAR_GetTrg( p_wk->p_touch ) )
  {
  case TOUCHBAR_ICON_CHECK :

#ifdef GAMESYS_NONE_MOVE
    if( p_wk->p_gdata == NULL )
    {
      OS_Printf( "\n!!!! GameSysPtr == NULL  !!!!\n" );
      break;
    }
#endif //GAMESYS_NONE_MOVE

    if( TOUCHBAR_GetFlip( p_wk->p_touch, TOUCHBAR_ICON_CHECK ) )
    {
      GAMEDATA_SetShortCut( p_wk->p_gdata, SHORTCUT_ID_CONFIG, TRUE );
    }
    else
    {
      GAMEDATA_SetShortCut( p_wk->p_gdata, SHORTCUT_ID_CONFIG, FALSE );
    }
    p_wk->select = APPBAR_WIN_NULL;
    break;

  case TOUCHBAR_ICON_CLOSE:
    p_wk->select  = APPBAR_WIN_EXIT;
    p_wk->is_decide = TRUE;
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���상�C��
 *
 *  @param  APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void APPBAR_MoveMain( APPBAR_WORK *p_wk )
{ 
  //���W���[�����C�����s
  {
    int i;
    for( i = 0; i < APPBAR_WIN_MAX-1; i++ )
    {
      APP_TASKMENU_WIN_Update( p_wk->p_win[i] );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  APPBAR�̑I������擾
 *
 *  @param  const APPBAR_WORK *cp_wk  ���[�N
 *  @param  select                    ����I�񂾂�
 *
 *  @return TRUE�Ō���  FALSE�ł܂�
 */
//-----------------------------------------------------------------------------
static BOOL APPBAR_IsDecide( const APPBAR_WORK *cp_wk, APPBAR_WIN_LIST *p_select )
{
  if( p_select )
  {
    *p_select = cp_wk->select;
  }
  return cp_wk->is_decide;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���o�҂�
 *
 *  @param  const APPBAR_WORK *cp_wkt   ���[�N
 *
 *  @return TRUE�ŉ��o�I��  FALSE�ŉ��o��
 */
//-----------------------------------------------------------------------------
static BOOL APPBAR_IsWaitEffect( APPBAR_WORK *p_wk )
{ 
  BOOL ret  = FALSE;
  if( p_wk->is_decide && p_wk->select != APPBAR_WIN_NULL )
  { 
    ret = APP_TASKMENU_WIN_IsFinish( p_wk->p_win[p_wk->select] );

    if( ret )
    { 
      APPBAR_StopEffect( p_wk );
    }
  }
  return ret;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���o������
 *
 *  @param  APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void APPBAR_StopEffect( APPBAR_WORK *p_wk )
{ 
  {
    int i;
    for( i = 0; i < APPBAR_WIN_MAX-1; i++ )
    {
      APP_TASKMENU_WIN_ResetDecide( p_wk->p_win[i] );
      APP_TASKMENU_WIN_SetActive( p_wk->p_win[i], FALSE );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ĕ`��
 *
 *  @param  APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void APPBAR_ReWrite( APPBAR_WORK *p_wk, HEAPID heapID )
{
  //����A��߂�{�^���쐬
  {
    int i;

    for( i = 0; i < APPBAR_WIN_MAX-1; i++ )
    {
      if( p_wk->item[i].str != NULL )
      {
        GFL_STR_DeleteBuffer( p_wk->item[i].str );
      }
      p_wk->item[i].str       = GFL_MSG_CreateString( p_wk->p_msg, mes_config_menu08 + i );
      p_wk->item[i].msgColor  = APP_TASKMENU_ITEM_MSGCOLOR;
      p_wk->item[i].type      = APP_TASKMENU_WIN_TYPE_NORMAL+i;
      if( p_wk->p_win[i] )
      {
          APP_TASKMENU_WIN_Delete( p_wk->p_win[i] );
      }
      p_wk->p_win[i] = APP_TASKMENU_WIN_Create( p_wk->p_res, &p_wk->item[i],
                            APPBAR_WIN_X + APPBAR_WIN_W*i, APPBAR_MENUBAR_Y, APPBAR_WIN_W, heapID );
    }
  }
}
//=============================================================================
/**
 *          SCROLL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���Ǘ�������
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 *  @param  font_frm          ����BG
 *  @param  back_frm          �w�iBG
 *  @param  heapID            �q�[�vID
 */
//-----------------------------------------------------------------------------
static void SCROLL_Init( SCROLL_WORK *p_wk, u8 font_frm, u8 back_frm, const GRAPHIC_WORK *cp_grp, const CONFIG_PARAM *cp_param, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk,sizeof(SCROLL_WORK) );
  p_wk->font_frm  = font_frm;
  p_wk->back_frm  = back_frm;
  p_wk->obj_y_ofs = SCROLL_WINDOW_OFS_DOT;
  p_wk->now       = *cp_param;

  {
    int i;
    for( i = 0; i < CONFIG_ITEM_MAX; i++ )
    {
      p_wk->p_item[i] = GRAPHIC_GetClwk( cp_grp, CLWKID_ITEM_TOP + i );
    }
  }
#ifdef SCROLL_MOVE_NONE
  //�J�n�I�t�Z�b�g�܂ł��炷
  Scroll_MoveRange( p_wk, -8, -192, 192 );
#else
  //�J�n�I�t�Z�b�g�܂ł��炷
  Scroll_Move( p_wk, SCROLL_START_OFS_Y );
#endif
  //�L�[���[�h���L�[�Ȃ�΃J�[�\���ʒu������
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_TOUCH )
  {
    p_wk->select    = CONFIG_LIST_INIT;
    Scroll_ChangePlt( p_wk, FALSE );
    p_wk->is_info_update = FALSE;
  }
  else
  {
    p_wk->select    = CONFIG_LIST_MSGSPEED;
    Scroll_ChangePlt( p_wk, TRUE );
    p_wk->is_info_update = TRUE;
  }

}
//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���Ǘ��j��
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SCROLL_Exit( SCROLL_WORK *p_wk )
{
  GFL_STD_MemClear( p_wk,sizeof(SCROLL_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���Ǘ����C������
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 *  @param  UI_WORK *cp_ui    UI���[�N
 *  @param  MSGWND_WORK       MSG���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SCROLL_Main( SCROLL_WORK *p_wk, const UI_WORK *cp_ui, MSGWND_WORK *p_msg, GRAPHIC_WORK *p_graphic, APPBAR_WORK *p_appbar )
{
  UI_INPUT input;
  GFL_POINT trg_pos;
  GFL_POINT slide_pos;
  GFL_POINT slide_now;
  int y, bar_top;
  BOOL is_bmpprint_decide;

  //���͎擾
  input = UI_GetInput( cp_ui );

  //�擪�o�[�v�Z
  y = GFL_BG_GetScrollY( p_wk->back_frm );
  bar_top = (y-SCROLL_START)/SCROLL_WINDOW_H_DOT;

  //���͂ɂ�鑀��
  switch( input )
  {
  case UI_INPUT_KEY:
    //�L�[�ؑ֎�
    p_wk->select  = MATH_CLAMP( p_wk->select, CONFIG_LIST_MSGSPEED, CONFIG_LIST_CANCEL );
    Scroll_ChangePlt( p_wk, TRUE );
    p_wk->is_info_update  = TRUE;
    break;

  case UI_INPUT_SLIDE:
#ifndef SCROLL_MOVE_NONE
    UI_GetParam( cp_ui, UI_INPUT_PARAM_SLIDEPOS, &slide_pos );
    UI_GetParam( cp_ui, UI_INPUT_PARAM_SLIDENOW, &slide_now );
    UI_GetParam( cp_ui, UI_INPUT_PARAM_TRGPOS, &trg_pos );
    if( SCROLL_TOP_BAR_Y < trg_pos.y && trg_pos.y < SCROLL_APP_BAR_Y
       && SCROLL_TOP_BAR_Y < slide_now.y && slide_now.y < SCROLL_APP_BAR_Y )
    {
      Scroll_Move( p_wk, -slide_pos.y );
      Scroll_ChangePlt_Safe_Check( p_wk, -slide_pos.y );
      //�㉺�Ɉړ�����������Ƃ����������Ă��܂��̂ŁA�`�F�b�N
      if( Scroll_ChangePlt_Safe_IsOk(p_wk) )
      {
        Scroll_ChangePlt( p_wk, FALSE );
        GRAPHIC_StartPalleteFade( p_graphic );
      }
    }
#endif
    break;

  case UI_INPUT_CONT_UP:
    //���Ԋu���ƂɈړ����邽�߁A���Ԋu�ɒB���Ă��Ȃ����break
    //�B���Ă���΁A����case�̏��������s
    if( p_wk->cont_sync++ < SCROLL_CONT_SYNC_MAX )
    {
      break;
    }
    /* fallthrough */

  case UI_INPUT_TRG_UP:
    p_wk->cont_sync = 0;

    if( p_wk->select == CONFIG_LIST_MSGSPEED)
    {
      p_wk->select  = CONFIG_LIST_CANCEL;
    }
    else
    {
      p_wk->select--;
    }

#ifndef SCROLL_MOVE_NONE
    //�X�N���[�����鋗���ɂȂ����瓮��
    if( p_wk->select < bar_top )
    {
      Scroll_Move( p_wk, -SCROLL_WINDOW_H_DOT );
    }
#endif
    p_wk->is_info_update  = TRUE;
    PMSND_PlaySE( CONFIG_SE_MOVE );
    Scroll_ChangePlt( p_wk, TRUE );
    GRAPHIC_StartPalleteFade( p_graphic );
    break;

  case UI_INPUT_CONT_DOWN:
    //���Ԋu���ƂɈړ����邽�߁A���Ԋu�ɒB���Ă��Ȃ����break
    //�B���Ă���΁A����case�̏��������s
    if( p_wk->cont_sync++ < SCROLL_CONT_SYNC_MAX )
    {
      break;
    }
    /* fallthrough */

  case UI_INPUT_TRG_DOWN:
    p_wk->cont_sync = 0;
    if( p_wk->select == CONFIG_LIST_CANCEL )
    {
      p_wk->select =  CONFIG_LIST_MSGSPEED;
    }
    else
    {
      p_wk->select++;
    }
#ifndef SCROLL_MOVE_NONE
    //�X�N���[�����鋗���ɂȂ����瓮��
    if( p_wk->select > bar_top+SCROLL_DISPLAY )
    {
      Scroll_Move( p_wk, SCROLL_WINDOW_H_DOT );
    }
#endif
    p_wk->is_info_update  = TRUE;
    PMSND_PlaySE( CONFIG_SE_MOVE );
    Scroll_ChangePlt( p_wk, TRUE );
    GRAPHIC_StartPalleteFade( p_graphic );
    break;

  case UI_INPUT_TOUCH:
    UI_GetParam( cp_ui, UI_INPUT_PARAM_TRGPOS, &trg_pos );

    if( SCROLL_TOP_BAR_Y <= trg_pos.y && trg_pos.y <= SCROLL_APP_BAR_Y )
    {
      //���W����I���ւ̕ϊ�
      p_wk->select  = Scroll_PosToList( p_wk, &trg_pos );
      //���ڂ̃^�b�`
      Scroll_TouchItem( p_wk, &trg_pos );
      Scroll_ChangePlt( p_wk, FALSE );
      GRAPHIC_StartPalleteFade( p_graphic );
      p_wk->is_info_update  = TRUE;
      is_bmpprint_decide  = FALSE;
    }
    else
    {
      //�͈͊O���^�b�`
      Scroll_ChangePlt( p_wk, FALSE );

      GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), FALSE );
      GRAPHIC_StartPalleteFade( p_graphic );
      is_bmpprint_decide  = FALSE;
    }
    break;

  case UI_INPUT_TRG_RIGHT:
    if( p_wk->select < CONFIG_LIST_MAX )
    {
      PMSND_PlaySE( CONFIG_SE_MOVE );
      Scroll_SelectItem( p_wk, 1 );
      Scroll_ChangePlt( p_wk, TRUE );
      GRAPHIC_StartPalleteFade( p_graphic );
      is_bmpprint_decide  = TRUE;
    }
    else if( p_wk->select == CONFIG_LIST_DECIDE )
    {
      p_wk->select++;
      PMSND_PlaySE( CONFIG_SE_MOVE );
      p_wk->select  = MATH_CLAMP( p_wk->select, CONFIG_LIST_MSGSPEED, CONFIG_LIST_CANCEL );
    }
    break;

  case UI_INPUT_TRG_LEFT:
    if( p_wk->select < CONFIG_LIST_MAX )
    {
      PMSND_PlaySE( CONFIG_SE_MOVE );
      Scroll_SelectItem( p_wk, -1 );
      Scroll_ChangePlt( p_wk, TRUE );
      GRAPHIC_StartPalleteFade( p_graphic );
      is_bmpprint_decide  = TRUE;
    }
    else if( p_wk->select == CONFIG_LIST_CANCEL )
    {
      p_wk->select--;
      PMSND_PlaySE( CONFIG_SE_MOVE );
      p_wk->select  = MATH_CLAMP( p_wk->select, CONFIG_LIST_MSGSPEED, CONFIG_LIST_CANCEL );
    }
    break;

  case UI_INPUT_TRG_Y:
    Scroll_ChangePlt( p_wk, TRUE );
    break;

  case UI_INPUT_TRG_DECIDE:
  case UI_INPUT_TRG_CANCEL:
    Scroll_ChangePlt( p_wk, FALSE );
    break;
  }

  //���ڂɕύX���������ꍇ�̍X�V
  {
    CONFIG_ITEM item  = CONFIGPARAM_GetChangeTiming( &p_wk->now );
    switch( item )
    {
    case CONFIG_ITEM_STR_HIRAGANA:
    case CONFIG_ITEM_STR_KANJI:
      //�����ύX�̂���PRINT���Ȃ���
      GRAPHIC_PrintBmpwin( p_graphic );
      APPBAR_ReWrite( p_appbar, HEAPID_CONFIG );
      Scroll_ChangePlt( p_wk, is_bmpprint_decide );
      p_wk->is_info_update  = TRUE;
      break;
    case CONFIG_ITEM_MSG_SLOW:
    case CONFIG_ITEM_MSG_NORMAL:
    case CONFIG_ITEM_MSG_FAST:
      //���b�Z�[�W���x��ύX�����Ƃ��͕�����₷���悤��
      //���ʍX�V
      p_wk->is_info_update  = TRUE;
      break;
    }
  }

  //���ʃ��b�Z�[�W�X�V
  if( p_wk->is_info_update )
  {
    if( CONFIG_LIST_MSGSPEED <= p_wk->select && p_wk->select < CONFIG_LIST_MAX )
    {
      int speed;
      BOOL is_stream;
      if( p_wk->select == CONFIG_LIST_MSGSPEED )
      { 
        is_stream = TRUE;
        speed = CONFIGPARAM_GetMsgSpeed(&p_wk->now);
      }
      else
      { 
        is_stream = FALSE;
        speed = 0;
      }
      GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), TRUE );
      MSGWND_Print( p_msg, sc_list_info[ p_wk->select ].infoID, speed, is_stream );
    }
    else
    {
      GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), FALSE );
    }
    p_wk->is_info_update  = FALSE;
  }


  MSGWND_IsEndMsg( p_msg );

  //�p���b�g�؂�ւ��ɂ�邿�������h�~���C��
  Scroll_ChangePlt_Safe_Main( p_wk );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���݉���I�����Ă��邩�擾
 *
 *  @param  const SCROLL_WORK *cp_wk  ���[�N
 *
 *  @return ���ݑI�����Ă������
 */
//-----------------------------------------------------------------------------
static CONFIG_LIST SCROLL_GetSelect( const SCROLL_WORK *cp_wk )
{
  return cp_wk->select;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �R���t�B�O�p�����[�^�󂯎��
 *
 *  @param  const SCROLL_WORK *cp_wk  ���[�N
 *  @param  *p_param                  �󂯎��
 */
//-----------------------------------------------------------------------------
static void SCROLL_GetConfigParam( const SCROLL_WORK *cp_wk, CONFIG_PARAM *p_param )
{
  *p_param  = cp_wk->now;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �R���t�B�O�p�����[�^�Ƀ��C�����X�ݒ�
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 *  @param  param             NETWORK_SEARCH_ON  or NETWORK_SEARCH_OFF
 */
//-----------------------------------------------------------------------------
static void SCROLL_SetConfigParamWireless( SCROLL_WORK *p_wk, u16 param )
{ 
 // p_wk->now.param[CONFIG_LIST_WIRELESS] = param;
  Scroll_ChangePlt( p_wk, p_wk->pre_decide_draw );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �R���t�B�O�p�����[�^�Ƀ��|�[�g���M�ݒ�
 *
 *	@param	SCROLL_WORK *p_wk ���[�N
 *	@param	param             WIRELESSSAVE_OFF or WIRELESSSAVE_ON
 */
//-----------------------------------------------------------------------------
static void SCROLL_SetConfigParamReport( SCROLL_WORK *p_wk, u16 param )
{ 
  p_wk->now.param[CONFIG_LIST_REPORT] = param;
  Scroll_ChangePlt( p_wk, p_wk->pre_decide_draw );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �p���b�g�œh��
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 *  @param  is_decide_draw    ����F��h�邩�ǂ���
 */
//-----------------------------------------------------------------------------
static void Scroll_ChangePlt( SCROLL_WORK *p_wk, BOOL is_decide_draw )
{
  enum
  {
    SCROLL_SELECT_PLT = CONFIG_BG_PAL_M_07,
  };

  static const u8 sc_bright_plt[] =
  {
    CONFIG_BG_PAL_M_03,CONFIG_BG_PAL_M_01,CONFIG_BG_PAL_M_00,CONFIG_BG_PAL_M_01, CONFIG_BG_PAL_M_03,
  };

  int i;
  int bar_top;
  int start;
  int y;

  //���莞�Ń��X�g�Ȃ��̂Ƃ�
  if( p_wk->select > CONFIG_LIST_MAX )
  {
    is_decide_draw  = FALSE;
  }

  p_wk->pre_decide_draw  = is_decide_draw;

  //���W�擾
  y = GFL_BG_GetScrollY( p_wk->back_frm );

  //�擪�o�[
  bar_top = (y-SCROLL_START)/SCROLL_WINDOW_H_DOT;

  //��[�S���ʂ�
  GFL_BG_ChangeScreenPalette( p_wk->back_frm, 0, 0,
      32, 32, CONFIG_BG_PAL_M_05 );

  //�ʏ�̃��X�g��h��
  for( i = 0; i < NELEMS(sc_bright_plt); i++ )
  {
    //�o�[���ʂ�
    start = SCROLL_WINDOW_OFS_CHR +  ( bar_top +i ) * SCROLL_WINDOW_H_CHR;
    GFL_BG_ChangeScreenPalette( p_wk->back_frm, 0, start,
        32, SCROLL_WINDOW_H_CHR, sc_bright_plt[i] );
  }

  //�t�H���g��h��
  //���X�g���ڂ̕���
  for( i = BMPWINID_ITEM_TOP; i < BMPWINID_ITEM_MAX; i++ )
  {
    //�I�𒆂ł����
    if( sc_item_info[i].list == p_wk->select && is_decide_draw )
    {
      //ON��OFF��
      if( CONFIGPARAM_IsItemSetting( &p_wk->now, i ) )
      {
        GFL_BG_ChangeScreenPalette( p_wk->font_frm,
            sc_config_menu_bmpwin_data[i].x,
            sc_config_menu_bmpwin_data[i].y,
            sc_config_menu_bmpwin_data[i].w,
            sc_config_menu_bmpwin_data[i].h,
            CONFIG_BG_PAL_M_07
            );
      }
      else
      {
        GFL_BG_ChangeScreenPalette( p_wk->font_frm,
            sc_config_menu_bmpwin_data[i].x,
            sc_config_menu_bmpwin_data[i].y,
            sc_config_menu_bmpwin_data[i].w,
            sc_config_menu_bmpwin_data[i].h,
            CONFIG_BG_PAL_M_12
            );
      }
    }
    else
    {
      //ON��OFF��
      if( CONFIGPARAM_IsItemSetting( &p_wk->now, i ) )
      {
        GFL_BG_ChangeScreenPalette( p_wk->font_frm,
            sc_config_menu_bmpwin_data[i].x,
            sc_config_menu_bmpwin_data[i].y,
            sc_config_menu_bmpwin_data[i].w,
            sc_config_menu_bmpwin_data[i].h,
            CONFIG_BG_PAL_M_00
            );
      }
      else
      {
        GFL_BG_ChangeScreenPalette( p_wk->font_frm,
            sc_config_menu_bmpwin_data[i].x,
            sc_config_menu_bmpwin_data[i].y,
            sc_config_menu_bmpwin_data[i].w,
            sc_config_menu_bmpwin_data[i].h,
            CONFIG_BG_PAL_M_03
            );
      }
    }
  }
  //���X�g�^�C�g���̕���
  for( i = BMPWINID_LIST_TOP; i < BMPWINID_LIST_MAX; i++ )
  {
  //�I�𒆂ł����
    if( ((i - BMPWINID_LIST_TOP)== p_wk->select) && is_decide_draw )
    {
      GFL_BG_ChangeScreenPalette( p_wk->font_frm,
          sc_config_menu_bmpwin_data[i].x,
          sc_config_menu_bmpwin_data[i].y,
          sc_config_menu_bmpwin_data[i].w,
          sc_config_menu_bmpwin_data[i].h,
          SCROLL_SELECT_PLT
          );
    }
    else
    {
      GFL_BG_ChangeScreenPalette( p_wk->font_frm,
          sc_config_menu_bmpwin_data[i].x,
          sc_config_menu_bmpwin_data[i].y,
          sc_config_menu_bmpwin_data[i].w,
          sc_config_menu_bmpwin_data[i].h,
          CONFIG_BG_PAL_M_00
          );
    }
  }


  //����̃��X�g��h��
  if( is_decide_draw )
  {
    start = SCROLL_WINDOW_OFS_CHR + ( p_wk->select ) * SCROLL_WINDOW_H_CHR;
    GFL_BG_ChangeScreenPalette( p_wk->back_frm, 0, start,
          32, SCROLL_WINDOW_H_CHR, SCROLL_SELECT_PLT );
  }

  //�]��
  GFL_BG_LoadScreenV_Req( p_wk->back_frm );
  GFL_BG_LoadScreenV_Req( p_wk->font_frm );

  //OBJ�̃p���b�g�ύX
  {

    enum
    {
      SCROLL_SELECT_PLT = CONFIG_OBJ_PAL_M_07,
    };

    int i, j;

    for( i = 0; i < CONFIG_ITEM_MAX; i++ )
    {
      //�I�𒆂ł����
      if( sc_item_info[i].list == p_wk->select && is_decide_draw )
      {
        //ON��OFF��
        if( CONFIGPARAM_IsItemSetting( &p_wk->now, i ) )
        {
          GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_item[i], 3 );
        }
        else
        {
          GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_item[i], 2 );
        }
      }
      else
      {
        //ON��OFF��
        if( CONFIGPARAM_IsItemSetting( &p_wk->now, i ) )
        {
          GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_item[i], 0 );
        }
        else
        {
          GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_item[i], 1 );
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���ړ�
 *
 *  @param  SCROLL_WORK *p_wk   ���[�N
 *  @param  y_add               Y�ړ��l
 */
//-----------------------------------------------------------------------------
static void Scroll_Move( SCROLL_WORK *p_wk, int y_add )
{
  Scroll_MoveRange( p_wk, y_add, SCROLL_START, SCROLL_END );

}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�N���[���ړ�  �����W�w���
 *
 *	@param	SCROLL_WORK *p_wk ���[�N
 *	@param	y_add Y�ړ��l
 *	@param	min   �ŏ�
 *	@param	max   �ő�
 */
//-----------------------------------------------------------------------------
static void Scroll_MoveRange( SCROLL_WORK *p_wk, int y_add, int min, int max )
{ 
  int y;


  //BG�̈ړ�

  //����
  y = GFL_BG_GetScrollY( p_wk->back_frm );
  y += y_add;
  y = MATH_CLAMP( y, min, max );

  //�ړ�
  GFL_BG_SetScrollReq( p_wk->font_frm, GFL_BG_SCROLL_Y_SET, y + SCROLL_FONT_OFS_CHR );
  GFL_BG_SetScrollReq( p_wk->back_frm, GFL_BG_SCROLL_Y_SET, y );

  //OBJ�̈ړ�
  {
    int i;
    GFL_CLACTPOS  pos;

    //�ړ�
    p_wk->obj_y_ofs -= y_add;
    p_wk->obj_y_ofs = MATH_CLAMP( p_wk->obj_y_ofs, SCROLL_END_OBJ, SCROLL_START_OBJ );

    //�ݒ�
    for( i = 0; i < CONFIG_ITEM_MAX; i++ )
    {
      pos.x = sc_item_info[i].obj_pos.x;
      pos.y = sc_item_info[i].obj_pos.y + p_wk->obj_y_ofs;
      GFL_CLACT_WK_SetPos( p_wk->p_item[i], &pos, CLSYS_DEFREND_MAIN );
    }
  }


}
//----------------------------------------------------------------------------
/**
 *  @brief  �^�b�`�ō��ڂ�I�����鏈��
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 *  @param  GFL_POINT *cp_pos �^�b�`���W
 */
//-----------------------------------------------------------------------------
static void Scroll_TouchItem( SCROLL_WORK *p_wk, const GFL_POINT *cp_pos )
{
  int i;
  GFL_RECT  rect;

  //�^�b�`�͈͓��Ȃ��
    //���ڂ��`�F�b�N
    for( i = 0; i < CONFIG_ITEM_MAX; i++ )
    {
      rect.top    = sc_item_info[i].touch_pos.y + p_wk->obj_y_ofs;
      rect.left   = sc_item_info[i].touch_pos.x;
      rect.bottom = rect.top + sc_item_info[i].touch_pos.h;
      rect.right  = sc_item_info[i].touch_pos.x + sc_item_info[i].touch_pos.w;

      //������ӏ����^�b�`�͈͂ɓ����Ă��邩�`�F�b�N
      if( SCROLL_TOP_BAR_Y <= rect.top && rect.top <= SCROLL_APP_BAR_Y
          && SCROLL_TOP_BAR_Y <= rect.bottom && rect.bottom <= SCROLL_APP_BAR_Y )
      {

        //��`
        if( COLLISION_IsRectXPos( &rect, cp_pos ))
        {
          PMSND_PlaySE( CONFIG_SE_MOVE );
          CONFIGPARAM_SetItem( &p_wk->now, i );
          _sound_mode_set( p_wk, p_wk->now.param[3], 0 );  // �X�e���I�E���m�����ݒ�
          break;
        }
      }
    }
}

//----------------------------------------------------------------------------------
/**
 * @brief �T�E���h�o�̓��[�h��ݒ肷��i�X�e���I�����m�����j
 *
 * @param   p_wk    
 * @param   dir   0:stereo 1:mono
 * @param   flag  0:�J�[�\���ʒu�𔻒肷�� 1:�����I�ɃZ�b�g�i�L�����Z�����p�j 
 */
//----------------------------------------------------------------------------------
static void _sound_mode_set( SCROLL_WORK *p_wk, s8 dir, int flag )
{
  OS_Printf("line=%d,dir=%d\n", p_wk->select, dir);
  if(p_wk->select==3 || flag==1){
    if(dir==0){
      PMSND_SetStereo( TRUE );
    }else{
      PMSND_SetStereo( FALSE );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���ڂ�I���i�E�����ɂP���炷�j
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 *  @param  s8 dir            ���Ȃ�ΉE  ���Ȃ�΍�
 */
//-----------------------------------------------------------------------------
static void Scroll_SelectItem( SCROLL_WORK *p_wk, s8 dir )
{
  //�I�����Ă���Ƃ�
  if( p_wk->select < CONFIG_LIST_MAX )
  {
    CONFIGPARAM_AddItem( &p_wk->now, p_wk->select, dir );
    _sound_mode_set( p_wk,p_wk->now.param[p_wk->select],0 );  // �X�e���I�E���m�����ݒ�
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���W���烊�X�g�ւ̕ϊ�
 *
 *  @param  const SCROLL_WORK *cp_wk  ���[�N
 *  @param  GFL_POINT *cp_pos         �^�b�`���W
 *
 *  @return �I�����Ă��郊�X�g
 */
//-----------------------------------------------------------------------------
static CONFIG_LIST Scroll_PosToList( const SCROLL_WORK *cp_wk, const GFL_POINT *cp_pos )
{
  int y;
  CONFIG_LIST list;

  y = GFL_BG_GetScrollY( cp_wk->back_frm );
  list  = (y + cp_pos->y - SCROLL_WINDOW_OFS_DOT) / SCROLL_WINDOW_H_DOT;


  //�^�b�`�͈͓��Ȃ�v�Z
  if( CONFIG_LIST_MSGSPEED <= list && list < CONFIG_LIST_MAX )
  {
    return list;
  }
  else
  {
    //�͈͊O�Ȃ�Ό��݂̂��̂�Ԃ�
    return cp_wk->select;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �X���C�h���̃p���b�g���肩�����������`�F�b�N  ���t���[������
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Scroll_ChangePlt_Safe_Main( SCROLL_WORK *p_wk )
{
  p_wk->dir_bit <<= 1;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X���C�h���̃p���b�g���肩�����������`�F�b�N  �`�F�b�N���C������
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Scroll_ChangePlt_Safe_Check( SCROLL_WORK *p_wk, int y )
{
  //�O��̕����Ɠ����Ȃ��
  if( p_wk->pre_y / MATH_IAbs(p_wk->pre_y) == y / MATH_IAbs(y) )
  {
    p_wk->dir_bit |= 0x1;
  }
  p_wk->pre_y = y;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �X���C�h���̃p���b�g���肩�����������`�F�b�N  ���v���ǂ���
 *
 *  @param  SCROLL_WORK *p_wk ���[�N
 *  @return TRUE�͂肩���Ă�OK  FALSE������������ꍇ
 */
//-----------------------------------------------------------------------------
static BOOL Scroll_ChangePlt_Safe_IsOk( const SCROLL_WORK *cp_wk )
{
  int i;
  int cnt = 0;

  int now_bit = cp_wk->dir_bit & 0x1;

  for( i = 1; i < SCROLL_CHAGEPLT_SAFE_SYNC; i++ )
  {
    if( (cp_wk->dir_bit >> i) & 0x1 == now_bit )
    {
      cnt++;
    }
  }

  return cnt == SCROLL_CHAGEPLT_SAFE_SYNC-1;

}
//=============================================================================
/**
 *          CONFIRM
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �ŏI�m�F�I������
 *
 *  @param  CONFIRM_WORK *p_wk  ���[�N
 *  @param  *p_bmpwin           �����pBMPWIN
 *  @param  *p_font             �t�H���g
 *  @param  *p_msg              ���b�Z�[�W
 *  @param  *p_tcbl             �X�g���[���p�^�X�N
 *  @param  *p_res              �I���E�B���h�E�p���\�[�X
 *  @param  heapID              �q�[�vID
 */
//-----------------------------------------------------------------------------
static void CONFIRM_Init( CONFIRM_WORK *p_wk, GFL_BMPWIN *p_bmpwin, GFL_FONT *p_font, GFL_MSGDATA *p_msg, GFL_TCBLSYS *p_tcbl, APP_TASKMENU_RES *p_res, PRINT_QUE *p_que, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk, sizeof(CONFIRM_WORK) );
  p_wk->select  = CONFIRM_SELECT_NULL;
  p_wk->p_res   = p_res;

  //�ŏI�m�F���b�Z�[�W�쐬
  MSGWND_Init( &p_wk->msg,
      p_bmpwin,
      p_font,
      p_msg,
      p_tcbl,
      p_que,
      heapID );

  //�͂��A�������E�B���h�E�쐬
  {
    int i;
    for( i = 0; i < CONFIRM_WIN_MAX; i++ )
    {
      p_wk->item[i].str       = GFL_MSG_CreateString( p_msg, mes_config_comment21 + i );
      p_wk->item[i].msgColor  = APP_TASKMENU_ITEM_MSGCOLOR;
      p_wk->item[i].type      = APP_TASKMENU_WIN_TYPE_NORMAL;
    }
  }

  //�^�X�N���j���[�������\���̐ݒ�
  {
    p_wk->init.heapId   = heapID;
    p_wk->init.itemNum  = CONFIRM_WIN_MAX;
    p_wk->init.itemWork = p_wk->item;
    p_wk->init.posType   = ATPT_RIGHT_DOWN;
    p_wk->init.charPosX = 32;
    p_wk->init.charPosY = 12;
    p_wk->init.w        = APP_TASKMENU_PLATE_WIDTH;
    p_wk->init.h        = APP_TASKMENU_PLATE_HEIGHT;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ŏI�m�F�I��  �j��
 *
 *  @param  CONFIRM_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void CONFIRM_Exit( CONFIRM_WORK *p_wk )
{
  //�͂��A�������E�B���h�E���
  {
    int i;
    for( i = 0; i < CONFIRM_WIN_MAX; i++ )
    {
      GFL_STR_DeleteBuffer( p_wk->item[i].str );
    }
  }

  //���b�Z�[�W�j��
  MSGWND_Exit( &p_wk->msg );

  GFL_STD_MemClear( p_wk, sizeof(CONFIRM_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ŏI�m�F�I�����C������
 *
 *  @param  CONFIRM_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void CONFIRM_Main( CONFIRM_WORK *p_wk )
{
  enum
  {
    SEQ_PRINT_WAIT,
    SEQ_SELECT_INIT,
    SEQ_SELECT_WAIT,
    SEQ_SELECT_EXIT,
  };

  switch( p_wk->seq )
  {
  case SEQ_PRINT_WAIT:
    if( MSGWND_IsEndMsg( &p_wk->msg ) )
    {
      p_wk->seq = SEQ_SELECT_INIT;
    }
    break;

  case SEQ_SELECT_INIT:
    p_wk->p_menu  = APP_TASKMENU_OpenMenu( &p_wk->init, p_wk->p_res );
    p_wk->seq = SEQ_SELECT_WAIT;
    break;

  case SEQ_SELECT_WAIT:
    APP_TASKMENU_UpdateMenu( p_wk->p_menu );
    if( APP_TASKMENU_IsFinish( p_wk->p_menu ) )
    {
      //�^�X�N���j���[�j��
      APP_TASKMENU_CloseMenu( p_wk->p_menu );
      p_wk->select  = CONFIRM_SELECT_YES + APP_TASKMENU_GetCursorPos( p_wk->p_menu );
      p_wk->seq     = SEQ_SELECT_EXIT;
    }
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *  @brief  �J�n
 *
 *  @param  CONFIRM_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void CONFIRM_Start( CONFIRM_WORK *p_wk, int wait )
{
  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_OBJ , 8 );
  GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M), TRUE );

  MSGWND_Print( &p_wk->msg, mes_config_comment20, wait, FALSE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �I���擾
 *
 *  @param  const CONFIRM_WORK *cp_wk   ���[�N
 *
 *  @return CONFIRM_SELECT��
 */
//-----------------------------------------------------------------------------
static CONFIRM_SELECT CONFIRM_Select( const CONFIRM_WORK *cp_wk )
{
  return cp_wk->select;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�\��
 *
 *  @param  CONFIRM_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void CONFIRM_PrintErrMessage( CONFIRM_WORK *p_wk, u32 strID, int wait )
{ 
  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_OBJ , 8 );
  GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M), TRUE );

  { 
    GFL_MSGDATA *p_msg  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_common_scr_dat, GFL_HEAP_LOWID(HEAPID_CONFIG) );
    MSGWND_PrintEx( &p_wk->msg, p_msg, strID, wait, TRUE );

    GFL_MSG_Delete( p_msg );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�\��
 *
 *  @param  CONFIRM_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void CONFIRM_PrintMessage( CONFIRM_WORK *p_wk, u32 strID, int wait )
{ 
  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_OBJ , 8 );
  GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M), TRUE );
  MSGWND_Print( &p_wk->msg, strID, wait, TRUE );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�I���҂�
 *
 *  @param  CONFIRM_WORK *p_wk ���[�N
 *  @retval TRUE�I��  FALSE������
 */
//-----------------------------------------------------------------------------
static BOOL CONFIRM_IsEndMessage( CONFIRM_WORK *p_wk )
{ 

  if( MSGWND_IsEndMsg( &p_wk->msg ) )
  {
    G2_BlendNone();
    GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_DECIDE_M), FALSE );
    return TRUE;
  }

  return FALSE;
}
//=============================================================================
/**
 *          CONFIG_PARAM
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �ݒ���  ������
 *
 *  @param  CONFIG_PARAM *p_wk    ���[�N
 *  @param  CONFIG *cp_savedata   �R���t�B�O�Z�[�u�f�[�^
 */
//-----------------------------------------------------------------------------
static void CONFIGPARAM_Init( CONFIG_PARAM *p_wk, CONFIG *p_savedata )
{
  GFL_STD_MemClear( p_wk, sizeof(CONFIG_PARAM) );
  p_wk->p_savedata  = p_savedata;
  p_wk->change      = CONFIG_ITEM_NONE;

  p_wk->param[CONFIG_LIST_MSGSPEED] = CONFIG_GetMsgSpeed(p_savedata);
  p_wk->param[CONFIG_LIST_BTLEFF]   = CONFIG_GetWazaEffectMode(p_savedata);
  p_wk->param[CONFIG_LIST_BTLRULE]  = CONFIG_GetBattleRule(p_savedata);
  p_wk->param[CONFIG_LIST_SND]      = CONFIG_GetSoundMode(p_savedata);
  p_wk->param[CONFIG_LIST_STR]      = CONFIG_GetMojiMode(p_savedata);
//  p_wk->param[CONFIG_LIST_WIRELESS] = CONFIG_GetNetworkSearchMode(p_savedata);
  p_wk->param[CONFIG_LIST_REPORT]   = CONFIG_GetWirelessSaveMode(p_savedata);

  { 
    int i;
    for( i  = 0; i < CONFIG_LIST_MAX; i++ )
    { 
      OS_TFPrintf( 3, "config[%d] %d\n", i, p_wk->param[i] );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ݒ���  �j��
 *
 *  @param  CONFIG_PARAM *p_wk    ���[�N
 */
//-----------------------------------------------------------------------------
static void CONFIGPARAM_Exit( CONFIG_PARAM *p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(CONFIG_PARAM) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ݒ���  �Z�[�u�f�[�^�֔��f
 *
 *  @param  const CONFIG_PARAM *cp_wk ���[�N
 *  @param  *p_savedata               �Z�[�u�f�[�^
 */
//-----------------------------------------------------------------------------
static void CONFIGPARAM_Save( const CONFIG_PARAM *cp_wk )
{
  CONFIG_SetMsgSpeed( cp_wk->p_savedata, cp_wk->param[CONFIG_LIST_MSGSPEED] );
  CONFIG_SetWazaEffectMode( cp_wk->p_savedata, cp_wk->param[CONFIG_LIST_BTLEFF] );
  CONFIG_SetBattleRule( cp_wk->p_savedata, cp_wk->param[CONFIG_LIST_BTLRULE] );
  CONFIG_SetSoundMode( cp_wk->p_savedata, cp_wk->param[CONFIG_LIST_SND] );
  CONFIG_SetMojiMode( cp_wk->p_savedata, cp_wk->param[CONFIG_LIST_STR] );
//  CONFIG_SetNetworkSearchMode( cp_wk->p_savedata, cp_wk->param[CONFIG_LIST_WIRELESS] );
  CONFIG_SetWirelessSaveMode( cp_wk->p_savedata, cp_wk->param[CONFIG_LIST_REPORT] );

 { 
    int i;
    OS_TFPrintf( 3, "�Z�[�u\n" );
    for( i  = 0; i < CONFIG_LIST_MAX; i++ )
    { 
      OS_TFPrintf( 3, "config[%d] %d\n", i, cp_wk->param[i] );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ݒ���  ���ڂ�ݒ�
 *
 *  @param  CONFIG_PARAM *p_wk  ���[�N
 *  @param  item                ����
 */
//-----------------------------------------------------------------------------
static void CONFIGPARAM_SetItem( CONFIG_PARAM *p_wk, CONFIG_ITEM item )
{
  if( p_wk->param[ sc_item_info[ item ].list ] != sc_item_info[ item ].idx )
  {
    p_wk->param[ sc_item_info[ item ].list ]  = sc_item_info[ item ].idx;

    //�����A���b�Z�[�W�X�s�[�h�̃R���t�B�O���ύX�ɂ��ݒ�
    p_wk->change  = item;
    CONFIGPARAM_Save( p_wk );
  }
 { 
    int i;
    OS_TFPrintf( 3, "SetItem\n" );
    for( i  = 0; i < CONFIG_LIST_MAX; i++ )
    { 
      OS_TFPrintf( 3, "config[%d] %d\n", i, p_wk->param[i] );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ݒ���  ���ڂ��P���炷
 *
 *  @param  CONFIG_PARAM *p_wk  ���[�N
 *  @param  list                ���j���[
 *  @param  dir                 ���炷�I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
static void CONFIGPARAM_AddItem( CONFIG_PARAM *p_wk, CONFIG_LIST list, s8 dir )
{
  s8 now_idx;
  CONFIG_ITEM now, next;

  //���݂̃A�C�e�����擾
  {
    int i;
    now = -1;
    for( i = 0; i < sc_list_info[list].max; i++ )
    {
      now = sc_list_info[list].item[i];
      if( sc_item_info[ now ].idx == p_wk->param[list] )
      {
        break;
      }
    }
    GF_ASSERT( now != -1 );
  }

  //���������߂āA�ő�ŏ��`�F�b�N����
  now_idx = now - sc_list_info[ list ].item[0];
  now_idx += dir;
  now_idx = MATH_CLAMP( now_idx, 0, sc_list_info[list].max-1 );

  //��������ITEM�֕ϊ�
  next    = sc_list_info[ list ].item[ now_idx ];

  //CONFIG�p�֕ϊ�
  if( p_wk->param[ list ] != sc_item_info[ next ].idx )
  {
    p_wk->param[ list ] = sc_item_info[ next ].idx;

    //�����A���b�Z�[�W�X�s�[�h�̃R���t�B�O���ύX�ɂ��ݒ�
    p_wk->change  = next;
    CONFIGPARAM_Save( p_wk );
  }
 { 
    int i;
    OS_TFPrintf( 3, "AddItem\n" );
    for( i  = 0; i < CONFIG_LIST_MAX; i++ )
    { 
      OS_TFPrintf( 3, "config[%d] %d\n", i, p_wk->param[i] );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ݒ���  �w�肵�����ڂ��ݒ肳��Ă��邩�`�F�b�N
 *
 *  @param  const CONFIG_PARAM *cp_wk ���[�N
 *  @param  item                      ���ׂ鍀��
 *
 *  @return TRUE�Ȃ�ΐݒ肳��Ă���  FALSE�Ȃ�΂���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL CONFIGPARAM_IsItemSetting( const CONFIG_PARAM *cp_wk, CONFIG_ITEM item )
{
  return cp_wk->param[ sc_item_info[item].list ] == sc_item_info[item].idx;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�X�s�[�h�擾
 *
 *  @param  const CONFIG_PARAM *cp_wk   ���[�N
 *
 *  @return ���b�Z�[�W�X�s�[�h
 */
//-----------------------------------------------------------------------------
static int CONFIGPARAM_GetMsgSpeed( const CONFIG_PARAM *cp_wk )
{
  return MSGSPEED_GetWait();
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���ڂ�ς����Ƃ��A�P�V���N�����ς������ڂ�Ԃ�
 *
 *  @param  CONFIG_PARAM *p_wk  ���[�N
 *
 *  @return �ς�������
 */
//-----------------------------------------------------------------------------
static CONFIG_ITEM CONFIGPARAM_GetChangeTiming( CONFIG_PARAM *p_wk )
{
  CONFIG_ITEM change  = p_wk->change;
  p_wk->change  = CONFIG_ITEM_NONE;
  return change;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �p�����[�^���m���r����
 *
 *  @param  const CONFIG_PARAM *cp_wk1  ��r�P
 *  @param  CONFIG_PARAM *cp_wk2        ��r�Q
 *
 *  @return TRUE�Ȃ�Γ���
 */
//-----------------------------------------------------------------------------
static BOOL CONFIGPARAM_Compare( const CONFIG_PARAM *cp_wk1, const CONFIG_PARAM *cp_wk2 )
{
  return GFL_STD_MemComp( cp_wk1->param, cp_wk2->param, sizeof(u8) * CONFIG_LIST_MAX ) == 0;
}
//=============================================================================
/**
 *            SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ ������
 *
 *  @param  SEQ_WORK *p_wk  ���[�N
 *  @param  *p_param        �p�����[�^
 *  @param  seq_function    �V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param, SEQ_FUNCTION seq_function )
{
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );

  //������
  p_wk->p_param = p_param;

  //�Z�b�g
  SEQ_SetNext( p_wk, seq_function );
}
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ �j��
 *
 *  @param  SEQ_WORK *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQ_Exit( SEQ_WORK *p_wk )
{
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ ���C������
 *
 *  @param  SEQ_WORK *p_wk ���[�N
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
 *  @brief  SEQ �I���擾
 *
 *  @param  const SEQ_WORK *cp_wk   ���[�N
 *
 *  @return TRUE�Ȃ�ΏI��  FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk )
{
  return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ ���̃V�[�P���X��ݒ�
 *
 *  @param  SEQ_WORK *p_wk  ���[�N
 *  @param  seq_function    �V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function )
{
  p_wk->seq_function  = seq_function;
  p_wk->seq = 0;
}
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ �I��
 *
 *  @param  SEQ_WORK *p_wk  ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( SEQ_WORK *p_wk )
{
  p_wk->is_end  = TRUE;
}
//=============================================================================
/**
 *          SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �t�F�[�hOUT
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
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

  CONFIG_WORK *p_wk = p_param;

  switch( *p_seq )
  {
  case SEQ_FADEOUT_START:
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;

  case SEQ_FADEOUT_WAIT:
    if( !GFL_FADE_CheckFade() )
    {
      *p_seq  = SEQ_END;
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
 *  @brief  �t�F�[�hIN
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
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

  CONFIG_WORK *p_wk = p_param;

  switch( *p_seq )
  {
  case SEQ_FADEIN_START:
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    *p_seq  = SEQ_FADEIN_WAIT;
    break;

  case SEQ_FADEIN_WAIT:
    if( !GFL_FADE_CheckFade() )
    {
      *p_seq  = SEQ_EXIT;
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
 *  @brief  �ݒ��ʃ��C������
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
  enum
  { 
    SEQ_MAIN,
    SEQ_START_NOT_REPORT_MESSAGE,
    SEQ_START_NOT_WIRELESS_MESSAGE,
    SEQ_START_PARENTAL_CONTROL_MESSAGE,
    SEQ_START_NOT_DISCONNECT_MESSAGE,
    SEQ_END_MESSAGE,
  };

  CONFIG_WORK *p_wk = p_param;

  //���W���[�����C��
  switch( *p_seq )
  { 
  case SEQ_MAIN:
    UI_Main( &p_wk->ui );
    //���肵�Ă��Ȃ��Ƃ��ɓ���
    if( !APPBAR_IsDecide( &p_wk->appbar, NULL ) )
    {
      SCROLL_Main( &p_wk->scroll, &p_wk->ui, &p_wk->info, &p_wk->graphic, &p_wk->appbar );
    }
    APPBAR_Main(  &p_wk->appbar, &p_wk->ui, &p_wk->scroll );

    //��ԕψ�
    {
      APPBAR_WIN_LIST select;
      if( APPBAR_IsDecide( &p_wk->appbar, &select ) )
      {
        switch( select )
        {
        case APPBAR_WIN_DECIDE:
          if( APPBAR_IsWaitEffect( &p_wk->appbar ) )
          { 
            SEQ_SetNext( p_seqwk, SEQFUNC_SetNowConfig );
          }
          break;

        case APPBAR_WIN_EXIT:
          { 
            CONFIG_PARAM  now;
            p_wk->p_param->is_exit  = TRUE;
            SCROLL_GetConfigParam( &p_wk->scroll, &now );
            //���̐ݒ肪�O�̐ݒ�ƈႤ�Ȃ�΁A�ŏI�m�F��ʂ��o��
            if( CONFIGPARAM_Compare( &p_wk->pre, &now ) )
            {
              SEQ_SetNext( p_seqwk, SEQFUNC_SetPreConfig );
            }
            else
            {
              APPBAR_StopEffect( &p_wk->appbar );
              SEQ_SetNext( p_seqwk, SEQFUNC_Decide );
            }
          }
          break;
        case APPBAR_WIN_CANCEL:
          if( APPBAR_IsWaitEffect( &p_wk->appbar ) )
          {
            CONFIG_PARAM  now;
            SCROLL_GetConfigParam( &p_wk->scroll, &now );
            //���̐ݒ肪�O�̐ݒ�ƈႤ�Ȃ�΁A�ŏI�m�F��ʂ��o��
            if( CONFIGPARAM_Compare( &p_wk->pre, &now ) )
            {
              SEQ_SetNext( p_seqwk, SEQFUNC_SetPreConfig );
            }
            else
            {
              APPBAR_StopEffect( &p_wk->appbar );
              SEQ_SetNext( p_seqwk, SEQFUNC_Decide );
            }
          }
          break;
        }
      }
    }
    //���|�[�g�ł��Ȃ����b�Z�[�W
    { 
      GAMEDATA *p_gdata = GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys );
      SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_gdata );
      BOOL is_over_write  = SaveControl_IsOverwritingOtherData( p_sv_ctrl );
      CONFIG_PARAM config;
      SCROLL_GetConfigParam( &p_wk->scroll, &config );
      if( config.param[CONFIG_LIST_REPORT] == WIRELESSSAVE_OFF && is_over_write )
      { 
        *p_seq  = SEQ_START_NOT_REPORT_MESSAGE;
      }
    }
#if 0
    //�ڑ��֎~���b�Z�[�W
    { 
      const BOOL is_wire  = DS_SYSTEM_IsAvailableWireless();
      CONFIG_PARAM config;
      SCROLL_GetConfigParam( &p_wk->scroll, &config );
      if( config.param[CONFIG_LIST_WIRELESS] == NETWORK_SEARCH_ON && !is_wire )
      { 
        *p_seq  = SEQ_START_NOT_WIRELESS_MESSAGE;
      }
    }
    //�y�A�����^���R���g���[�������b�Z�[�W
    { 
      const BOOL is_ugc = DS_SYSTEM_IsRestrictUGC();
      CONFIG_PARAM config;
      SCROLL_GetConfigParam( &p_wk->scroll, &config );
      if( config.param[CONFIG_LIST_WIRELESS] == NETWORK_SEARCH_ON && is_ugc )
      { 
        *p_seq  = SEQ_START_PARENTAL_CONTROL_MESSAGE;
      }
    }

    //�ؒf�֎~���b�Z�[�W
    {
      //�p���X�ɂ���Ȃ�ΐؒf�֎~
      GAME_COMM_SYS_PTR p_comm = GAMESYSTEM_GetGameCommSysPtr( p_wk->p_param->p_gamesys );
      GAME_COMM_NO no = GameCommSys_BootCheck(p_comm);
      if( no == GAME_COMM_NO_INVASION )
      { 
        CONFIG_PARAM config;
        SCROLL_GetConfigParam( &p_wk->scroll, &config );
        if( config.param[CONFIG_LIST_WIRELESS] == NETWORK_SEARCH_OFF && 0 )
        { 
          *p_seq  = SEQ_START_NOT_DISCONNECT_MESSAGE;
        }
      }
    }
#endif
    break;

  case SEQ_START_NOT_REPORT_MESSAGE:
    {
      int speed;
      CONFIG_PARAM config;

      SCROLL_GetConfigParam( &p_wk->scroll, &config );
      speed = CONFIGPARAM_GetMsgSpeed(&config);
      CONFIRM_PrintMessage( &p_wk->confirm, mes_config_comment10, speed );
      SCROLL_SetConfigParamReport( &p_wk->scroll, WIRELESSSAVE_ON );
    }
    *p_seq  = SEQ_END_MESSAGE;
    break;

  case SEQ_START_NOT_WIRELESS_MESSAGE:
    {
      int speed;
      CONFIG_PARAM config;

      SCROLL_GetConfigParam( &p_wk->scroll, &config );
      speed = CONFIGPARAM_GetMsgSpeed(&config);
      CONFIRM_PrintErrMessage( &p_wk->confirm, msg_common_wireless_off_keywait, speed );
      SCROLL_SetConfigParamWireless( &p_wk->scroll, NETWORK_SEARCH_OFF );
    }
    *p_seq  = SEQ_END_MESSAGE;
    break;
    
  case SEQ_START_PARENTAL_CONTROL_MESSAGE:
    {
      int speed;
      CONFIG_PARAM config;

      SCROLL_GetConfigParam( &p_wk->scroll, &config );
      speed = CONFIGPARAM_GetMsgSpeed(&config);
      CONFIRM_PrintErrMessage( &p_wk->confirm, msg_common_wireless_off_keywait, speed );
      SCROLL_SetConfigParamWireless( &p_wk->scroll, NETWORK_SEARCH_OFF );
    }
    *p_seq  = SEQ_END_MESSAGE;
    break;

  case SEQ_START_NOT_DISCONNECT_MESSAGE:
    {
      int speed;
      CONFIG_PARAM config;

      SCROLL_GetConfigParam( &p_wk->scroll, &config );
      speed = CONFIGPARAM_GetMsgSpeed(&config);
      CONFIRM_PrintMessage( &p_wk->confirm, mes_config_comment30, speed );
      SCROLL_SetConfigParamWireless( &p_wk->scroll, NETWORK_SEARCH_ON );
    }
    *p_seq  = SEQ_END_MESSAGE;
    break;

  case SEQ_END_MESSAGE:
    if( CONFIRM_IsEndMessage( &p_wk->confirm ) )
    { 
      *p_seq  = SEQ_MAIN;
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ŏI�m�F����
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Decide( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
  enum
  {
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_YES,
    SEQ_NO,
  };

  CONFIG_WORK *p_wk = p_param;

  switch( *p_seq )
  {
  case SEQ_INIT:
    {
      int speed;
      CONFIG_PARAM config;

      SCROLL_GetConfigParam( &p_wk->scroll, &config );
      speed = CONFIGPARAM_GetMsgSpeed(&config);
      CONFIRM_Start( &p_wk->confirm, speed );
      GFL_BG_SetVisible( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TEXT_S), FALSE );
    }
    *p_seq  = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    switch( CONFIRM_Select( &p_wk->confirm ) )
    {
    case CONFIRM_SELECT_YES:
      *p_seq  = SEQ_YES;
      break;
    case CONFIRM_SELECT_NO:
      *p_seq  = SEQ_NO;
      break;
    }
    break;

  case SEQ_YES:
    SEQ_SetNext( p_seqwk, SEQFUNC_SetNowConfig );
    break;

  case SEQ_NO:
    SEQ_SetNext( p_seqwk, SEQFUNC_SetPreConfig );
    break;
  }

  CONFIRM_Main( &p_wk->confirm );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ύX�O�̐ݒ蔽�f����
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_SetPreConfig( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
  CONFIG_WORK *p_wk = p_param;

  //�ݒ�ύX
  CONFIGPARAM_Save( &p_wk->pre );
  _sound_mode_set( &p_wk->scroll, p_wk->pre.param[3], 1 );  // �X�e���I�E���m�����ݒ�
  

  SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ύX��̐ݒ蔽�f����
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_SetNowConfig( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
  CONFIG_WORK *p_wk = p_param;

  //�ݒ�ύX
  {
    CONFIG_PARAM config;
    SCROLL_GetConfigParam( &p_wk->scroll, &config );
    CONFIGPARAM_Save( &config );
  }

  PMSND_PlaySE( CONFIG_SE_SAVE );

  SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
}
//=============================================================================
/**
 *    ETC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  Screen�f�[�^�́@VRAM�]���g���Łi�ǂݍ��񂾃X�N���[���̈ꕔ�͈͂��o�b�t�@�ɒ������j
 *
 *  @param  ARCHANDLE *handle �n���h��
 *  @param  datID             �f�[�^ID
 *  @param  frm               �t���[��
 *  @param  chr_ofs           �L�����I�t�Z�b�g
 *  @param  src_x             �]���������W
 *  @param  src_y             �]����Y���W
 *  @param  src_w             �]������      //�f�[�^�̑S�̂̑傫��
 *  @param  src_h             �]��������    //�f�[�^�̑S�̂̑傫��
 *  @param  dst_x             �]����X���W
 *  @param  dst_y             �]����Y���W
 *  @param  dst_w             �]���敝
 *  @param  dst_h             �]���捂��
 *  @param  plt               �p���b�g�ԍ�
 *  @param  compressedFlag    ���k�t���O
 *  @param  heapID            �ꎞ�o�b�t�@�p�q�[�vID
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
    GF_ASSERT(0); //�X�N���[���f�[�^����Ȃ���
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

      p_scr16 = (u16 *)&p_scr_data->rawData;
      for( i = 0; i < src_w * src_h ; i++ )
      {
        p_scr16[i]  += chr_ofs;
      }
    }
    else
    {
      GF_ASSERT(0); //256�ł͍���ĂȂ�
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
    GF_ASSERT(0); //�o�b�t�@���Ȃ�
  }

  GFL_HEAP_FreeMemory( p_src_arc );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ��`�Ɠ_�̏Փˌ��m
 *
 *  @param  const GFL_RECT *cp_rect   ��`
 *  @param  GFL_POINT *cp_pos         �_
 *
 *  @return TRUE�Ńq�b�g  FALSE�Ńq�b�g���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL COLLISION_IsRectXPos( const GFL_RECT *cp_rect, const GFL_POINT *cp_pos )
{
  return ( ((u32)( cp_pos->x - cp_rect->left) <= (u32)(cp_rect->right - cp_rect->left))
            & ((u32)( cp_pos->y - cp_rect->top) <= (u32)(cp_rect->bottom - cp_rect->top)));
}
