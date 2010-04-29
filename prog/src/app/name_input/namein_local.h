//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		namein_local.h
 *	@brief  ���[�J���w�b�_
 *	@author	Toru=Nagihashi
 *	@date		2010.03.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//  lib
#include <gflib.h>

//  SYSTEM
#include "system/main.h"  //HEAPID
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "system/bmp_winframe.h"
#include "system/nsbtx_to_clwk.h"
#include "sound/pm_sndsys.h"
#include "gamesystem/msgspeed.h"
#include "system/bmp_oam.h"

//  module
#include "app/app_menu_common.h"
#include "pokeicon/pokeicon.h"

//  archive
#include "arc_def.h"
#include "message.naix"
#include "namein_gra.naix"
#include "font/font.naix"
#include "fieldmap/fldmmdl_mdlres.naix" //�t�B�[���h�l��OBJ
#include "msg/msg_namein.h"
#include "wifi_unionobj_plt.cdat" //���j�I��OBJ�̃p���b�g�ʒu
#include "wifi_unionobj.naix"

//  print
#include "print/str_tool.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_msg.h"

//  savedata
#include "savedata/misc.h"

//  se
#include "sound/sound_manager.h"

//  mine
#include "namein_snd.h"     //�T�E���h��`
#include "namein_graphic.h" //�O���t�B�b�N�ݒ�
#include "namein_data.h"    //���O���̓f�[�^�i�L�[�z��ƕϊ��j
#include "app/name_input.h" //�O���Q��

#include "debug/debug_str_conv.h"

//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================
//-------------------------------------
/// �}�N��
//=====================================
//��荞��
#define MATH_ROUND(x,min,max)   (x < min? max: x > max ? min: x)

//-------------------------------------
/// �q�[�v�T�C�Y
//=====================================
#define NAMEIN_HEAP_HIGH_SIZE (0x70000)
#define NAMEIN_HEAP_LOW_SIZE  (0x40000)

//-------------------------------------
/// BG�t���[��
//=====================================
enum
{
  //���C��
  BG_FRAME_STR_M  = GFL_BG_FRAME0_M,    //���͒��̕�����
  BG_FRAME_KEY_M  = GFL_BG_FRAME1_M,    //�L�[�z��
  BG_FRAME_BTN_M  = GFL_BG_FRAME2_M,    //���ʃ{�^��
  BG_FRAME_FONT_M = GFL_BG_FRAME3_M,    //�t�H���g

  //�T�u
  BG_FRAME_TEXT_S = GFL_BG_FRAME0_S,    //�e�L�X�g��
  BG_FRAME_BACK_S = GFL_BG_FRAME1_S,    //�w�i
} ;

//-------------------------------------
/// �p���b�g
//=====================================
enum
{
  //���C��BG
  PLT_BG_STR_M        = 0,  //���͕�����
  PLT_BG_KEY_NORMAL_M = 1,  //�w�i��{�F�p���b�g
  PLT_BG_KEY_MOVE_M   = 2,  //�w�i�ړ����p���b�g
  PLT_BG_KEY_DECIDE_M = 3,  //�w�i����p�p���b�g
  PLT_BG_KEY_PRESS_M  = 4,  //���������p�p���b�g

  PLT_BG_FONT_M       = 15, //�t�H���g�F

  //���C��OBJ
  PLT_OBJ_CURSOR_M    = 0,  //������̉��̃o�[OBJ
  PLT_OBJ_ICON_M      = 2,  //�A�C�R��
  PLT_OBJ_SEX_M      = 3,  //���ʕ����F

  //�T�uBG
  PLT_BG_BACK_S       = 0,  //�w�i
  PLT_BG_FRAME_S      = 14,  //WINDOW�t���[��
  PLT_BG_FONT_S       = 15, //�t�H���g�F

  //�T�uOBJ
  PLT_OBJ_S           = 0,
} ;

//-------------------------------------
/// ���\�[�X�C���f�b�N�X
//=====================================
typedef enum
{
  RESID_OBJ_COMMON_PLT,
  RESID_OBJ_COMMON_CHR,
  RESID_OBJ_COMMON_CEL,
  RESID_OBJ_MAX,
} RESID;

//-------------------------------------
/// OBJ
//=====================================
//�o�[�̐�
#define OBJ_BAR_NUM (16)
//�J�[�\���̐�
enum
{
  OBJ_CURSOR_L,
  OBJ_CURSOR_R,
  OBJ_CURSOR_MAX,
};

//-------------------------------------
/// CLWK
//=====================================
typedef enum
{
  CLWKID_BAR_TOP,
  CLWKID_BAR_END  = CLWKID_BAR_TOP + OBJ_BAR_NUM,
  CLWKID_PC,
  CLWKID_CURSOR_TOP,
  CLWKID_CURSOR_END = CLWKID_CURSOR_TOP + OBJ_CURSOR_MAX,
  CLWKID_MAX,
} CLWKID;

//-------------------------------------
/// STRINPUT
//=====================================
//������
#define STRINPUT_STR_LEN      (10)     //�ő���͕�����
#define STRINPUT_CHANGE_LEN   (3)      //�ϊ�������
//BMPWIN�T�C�Y
#define STRINPUT_BMPWIN_X     (8)
#define STRINPUT_BMPWIN_Y     (2)
#define STRINPUT_BMPWIN_W     (24)
#define STRINPUT_BMPWIN_H     (2)
//�����`��ʒu
#define STRINPUT_STR_START_X  (0)
#define STRINPUT_STR_OFFSET_X (14)
#define STRINPUT_STR_Y        (0)
//BAR�̈ʒu
#define STRINPUT_BAR_START_X  (STRINPUT_BMPWIN_X*8)
#define STRINPUT_BAR_OFFSET_X (STRINPUT_STR_OFFSET_X)
#define STRINPUT_BAR_Y        (36)
//���ꕶ���ϊ�
typedef enum
{ 
  STRINPUT_SP_CHANGE_DAKUTEN,     //�É������_
  STRINPUT_SP_CHANGE_HANDAKUTEN,  //�É��������_
  STRINPUT_SP_CHANGE_DAKU_SEION,  //���_���É�
  STRINPUT_SP_CHANGE_HAN_SEION,   //�����_���É�
  STRINPUT_SP_CHANGE_HANSEION,    //�����_���É�
  STRINPUT_SP_CHANGE_DAKUTEN2,    //�����_�����_
  STRINPUT_SP_CHANGE_HANDAKUTEN2, //���_�������_
} STRINPUT_SP_CHANGE;

//-------------------------------------
/// KEYMAP
//=====================================
//���A����
#define KEYMAP_KANA_WIDTH     (13)
#define KEYMAP_KANA_HEIGHT    (5)
#define KEYMAP_QWERTY_WIDTH   (11)
#define KEYMAP_QWERTY_HEIGHT  (4)

//���[�h
typedef enum
{ 
  NAMEIN_INPUTTYPE_KANA = NAMEIN_KEYMAPTYPE_KANA,   //����
  NAMEIN_INPUTTYPE_KATA = NAMEIN_KEYMAPTYPE_KATA,   //�J�i
  NAMEIN_INPUTTYPE_ABC  = NAMEIN_KEYMAPTYPE_ABC,      //ABC
  NAMEIN_INPUTTYPE_KIGOU= NAMEIN_KEYMAPTYPE_KIGOU,    //1/��
  NAMEIN_INPUTTYPE_QWERTY = NAMEIN_KEYMAPTYPE_QWERTY, //۰ς�

  NAMEIN_INPUTTYPE_MAX  =NAMEIN_KEYMAPTYPE_MAX,
}NAMEIN_INPUTTYPE;

//�L�[���
typedef enum
{ 
  KEYMAP_KEYTYPE_NONE,    //�Ȃɂ��Ȃ�
  KEYMAP_KEYTYPE_STR,     //��������
  KEYMAP_KEYTYPE_KANA,    //���ȃ��[�h
  KEYMAP_KEYTYPE_KATA,    //�J�i���[�h
  KEYMAP_KEYTYPE_ABC,     //�A���t�@�x�b�g���[�h
  KEYMAP_KEYTYPE_KIGOU,   //�L�����[�h
  KEYMAP_KEYTYPE_QWERTY,  //���[�}�����[�h
  KEYMAP_KEYTYPE_DELETE,  //����
  KEYMAP_KEYTYPE_DECIDE,  //��߂�
  KEYMAP_KEYTYPE_SHIFT,   //�V�t�g
  KEYMAP_KEYTYPE_SPACE,   //�X�y�[�X
  KEYMAP_KEYTYPE_DAKUTEN,   //���_
  KEYMAP_KEYTYPE_HANDAKUTEN,    //�����_
}KEYMAP_KEYTYPE;

//�L�[�ړ��ۑ��o�b�t�@
enum
{
  //kana
  KEYMAP_KEYMOVE_BUFF_DELETE, //��������o��Ƃ��i�㉺���ʁj
  KEYMAP_KEYMOVE_BUFF_DICEDE, //����肩��o��Ƃ��i�㉺���ʁj

  //qwerty
  KEYMAP_KEYMOVE_BUFF_Q_U,  //Q�����ɂ����Ƃ�
  KEYMAP_KEYMOVE_BUFF_A_D,  //A���牺�ɂ����Ƃ�
  KEYMAP_KEYMOVE_BUFF_L_U,  //L�����ɂ����Ƃ�
  KEYMAP_KEYMOVE_BUFF_BOU_U,//�[�����ɂ����Ƃ�
  KEYMAP_KEYMOVE_BUFF_LSHIFT_D, //���V�t�g���牺�ɂ����Ƃ�
  KEYMAP_KEYMOVE_BUFF_SPACE_U,  //�X�y�[�X�����ɂ����Ƃ�
  KEYMAP_KEYMOVE_BUFF_SPACE_D,  //�X�y�[�X���牺�ɂ����Ƃ�

  KEYMAP_KEYMOVE_BUFF_MAX,
};
//�L�[�ړ��f�t�H���g�l
static const u8 sc_keymove_default[KEYMAP_KEYMOVE_BUFF_MAX] =
{ 
  5,9,  0,1,9,8,0,4,4
};

//-------------------------------------
/// KEYBOARD
//=====================================
//�L�[�{�[�hBMPWIN�T�C�Y
#define KEYBOARD_BMPWIN_X   (3)
#define KEYBOARD_BMPWIN_Y   (5)
#define KEYBOARD_BMPWIN_W   (26)
#define KEYBOARD_BMPWIN_H   (15)
//���[�h�ؑֈړ�
#define KEYBOARD_CHANGEMOVE_START_Y     (0)
#define KEYBOARD_CHANGEMOVE_END_Y       (-48)
#define KEYBOARD_CHANGEMOVE_SYNC        (7)
#define KEYBOARD_CHANGEMOVE_START_ALPHA (16)
#define KEYBOARD_CHANGEMOVE_END_ALPHA   (0)
//���
typedef enum
{ 
  KEYBOARD_STATE_WAIT,  //�����҂���
  KEYBOARD_STATE_INPUT, //����
  KEYBOARD_STATE_MOVE,  //�ړ���
}KEYBOARD_STATE;
//���͏��
typedef enum
{ 
  KEYBOARD_INPUT_NONE,        //���͂��Ă��Ȃ�
  KEYBOARD_INPUT_STR,         //��������
  KEYBOARD_INPUT_CHANGESTR,   //�ϊ���������
  KEYBOARD_INPUT_DAKUTEN,     //���_
  KEYBOARD_INPUT_HANDAKUTEN,  //�����_
  KEYBOARD_INPUT_BACKSPACE,   //��O�ɖ߂�
  KEYBOARD_INPUT_CHAGETYPE,   //���̓^�C�v�ύX
  KEYBOARD_INPUT_EXIT,        //�I��  
  KEYBOARD_INPUT_SHIFT,       //�V�t�g
  KEYBOARD_INPUT_SPACE,       //�X�y�[�X
  KEYBOARD_INPUT_AUTOCHANGE,   //R�{�^���p�����ϊ�
}KEYBOARD_INPUT;

//-------------------------------------
/// KEYANM
//=====================================
//�J�E���g
#define KEYANM_DECIDE_CNT (10)
//���̃A�j����
typedef enum
{
  KEYANM_TYPE_NONE,
  KEYANM_TYPE_MOVE,
  KEYANM_TYPE_DECIDE
} KEYANM_TYPE;

//-------------------------------------
/// MSGWND
//=====================================
//�t���[����ǂݍ��񂾃L�����ʒu
#define MSGWND_FRAME_CHR  (1)
//BMPWIN�̕�
#define MSGWND_BMPWIN_X (1)
#define MSGWND_BMPWIN_Y (19)
#define MSGWND_BMPWIN_W (30)
#define MSGWND_BMPWIN_H (4)

//-------------------------------------
/// ICON
//=====================================
//�A�C�R���̍��W
#define ICON_POS_X  (46-12)
#define ICON_POS_Y  (20)
#define ICON_POS_SEX_X  (46)
#define ICON_WIDTH  (32)
#define ICON_HEIGHT (32)

//�A�C�R���̓���
#define ICON_MOVE_SYNC  (8)
#define ICON_MOVE_Y     (12)
#define ICON_MOVE_START ((0*0xffff) / 360)
#define ICON_MOVE_END   ((180*0xffff) / 360)
#define ICON_MOVE_DIF   ( ICON_MOVE_END - ICON_MOVE_START )

//�A�C�R���̎��
typedef enum
{
  ICON_TYPE_HERO,
  ICON_TYPE_RIVAL,
  ICON_TYPE_PERSON,
  ICON_TYPE_POKE,
  ICON_TYPE_BOX,
  ICON_TYPE_HATENA,
} ICON_TYPE;

//-------------------------------------
/// ���̑�
//=====================================
#define NAMEIN_DEFAULT_NAME_MAX (2)

//=============================================================================
/**
 *          �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// �V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;  //�֐��^��邽�ߑO���錾
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param );
struct _SEQ_WORK
{
  SEQ_FUNCTION  seq_function;   //���s���̃V�[�P���X�֐�
  BOOL is_end;                  //�V�[�P���X�V�X�e���I���t���O
  int seq;                      //���s���̃V�[�P���X�֐��̒��̃V�[�P���X
  void *p_param;                //���s���̃V�[�P���X�֐��ɓn�����[�N
};
//-------------------------------------
/// BG���\�[�X�Ǘ�
//=====================================
typedef struct 
{
  int dummy;
} BG_WORK;
//-------------------------------------
/// OBJ���\�[�X�Ǘ�
//=====================================
typedef struct 
{
  u32         res[RESID_OBJ_MAX];   //���\�[�X
  GFL_CLWK    *p_clwk[CLWKID_MAX];  //CLWK
} OBJ_WORK;
//-------------------------------------
/// KEYANM
//=====================================
typedef struct 
{
  u16   plt_normal[0x10]; //�ʏ펞�̌��̃p���b�g
  u16   plt_flash[0x10];  //���莞�̌��̃p���b�g
  u16   color[0x10];      //�F�o�b�t�@
  u16   cnt[0x10];        //�J�E���g
  u16   decide_cnt;       //����A�j����ԃJ�E���g
  

  GFL_RECT    range;      //�{�^���̋�`
  KEYANM_TYPE type;       //�A�j���̎��
  BOOL        is_start;   //�J�n�t���O
  BOOL        is_shift;   //�V�t�g�����t���O
  NAMEIN_INPUTTYPE mode;  //���[�h
  GFL_CLWK    *p_cursor[OBJ_CURSOR_MAX];  //�J�[�\��OBJ
  GFL_CLACTPOS  cursor_pos[OBJ_CURSOR_MAX];//OBJ�p���W
} KEYANM_WORK;

//-------------------------------------
/// STRINPUT ���͂���������
//=====================================
typedef struct 
{
  STRCODE           input_str[STRINPUT_STR_LEN+1];    //�m�蕶����
  STRCODE           change_str[STRINPUT_CHANGE_LEN+1];//�ϊ�������
  u16               input_idx;    //�m�蕶���񕶎����iEOM�̂����j
  u16               change_idx;   //�ϊ������񕶎����iEOM�̂����j
  u16               strlen;       //EOM�������ő啶����
  u16               dummy;
  BOOL              is_update;    //�����`��A�b�v�f�[�g
  PRINT_UTIL        util;         //�����`��UTIL
  GFL_FONT          *p_font;      //�t�H���g
  PRINT_QUE         *p_que;       //�����L���[
  STRBUF            *p_strbuf;    //�����o�b�t�@
  GFL_BMPWIN        *p_bmpwin;    //BMPWIN
  GFL_CLWK          *p_clwk[OBJ_BAR_NUM];   //�o�[��OBJ

  NAMEIN_STRCHANGE  *p_changedata[NAMEIN_STRCHANGETYPE_MAX];  //�ϊ��f�[�^
  NAMEIN_STRCHANGE_EX *p_changedata_ex;

} STRINPUT_WORK;
//-------------------------------------
/// �L�[�z��
//=====================================
typedef struct 
{
  u8 data[KEYMAP_KEYMOVE_BUFF_MAX];
} KEYMOVE_BUFF;
typedef BOOL (*GET_KEYRECT_FUNC)( const GFL_POINT *cp_cursor, GFL_RECT *p_rect );
typedef void (*GET_KEYMOVE_FUNC)( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff );
typedef KEYMAP_KEYTYPE (*GET_KEYTYPE_FUNC)( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos );
typedef struct 
{
  NAMEIN_KEYMAP     *p_key; //���[�h�ɂ���Ă��ƂȂ�L�[�z����

  KEYMOVE_BUFF      buff;   //�L�[�ړ��o�b�t�@

  //�ȉ��֐��^�́A���[�h�ɂ���āA
  //�L�[�z�񂪈قȂ���i�L�[�̈ʒu�A�J�[�\���̈ړ����@�A�L�[�̎�ޔ��ʁj��
  //�֐��؂�ւ��ɂ���Ď��������邽�߂̌^
  GET_KEYRECT_FUNC  get_keyrect;    //�L�[�̈ʒu�i��`�j
  GET_KEYMOVE_FUNC  get_movecursor; //�J�[�\���̈ړ�
  GET_KEYTYPE_FUNC  get_keytype;    //�L�[�̎��

  u16               w;  //�z��ő�WIDTH
  u16               h;  //�z��ő�HEIGHT
} KEYMAP_WORK;
//-------------------------------------
/// ���b�Z�[�W�E�B���h�E
//=====================================
typedef struct 
{
  GFL_MSGDATA       *p_msg;     //���b�Z�[�W�f�[�^
  GFL_FONT          *p_font;    //�t�H���g
  PRINT_UTIL        util;       //�����`��
  PRINT_QUE         *p_que;     //�����L���[
  GFL_BMPWIN*       p_bmpwin;   //BMPWIN
  WORDSET           *p_word;    //�P��
  STRBUF*           p_strbuf;   //�����o�b�t�@
  u16               clear_chr;  //�N���A�L����
  u16               dummy;
} MSGWND_WORK;
//-------------------------------------
/// �v�����g�X�g���[��
//=====================================
typedef struct 
{
  PRINT_STREAM      *p_stream;
  GFL_TCBLSYS       *p_tcblsys;
  SEQ_FUNCTION      next_seq_function;
  u16               wait_max;
  u16               wait_count;
} PS_WORK;
//-------------------------------------
/// �L�[�{�[�h
//=====================================
typedef struct 
{
  KEYMAP_WORK     keymap;     //�L�[���
  KEYANM_WORK     keyanm;     //�L�[�A�j��
  GFL_POINT       cursor;     //�J�[�\���ʒu
  GFL_BMPWIN      *p_bmpwin;  //BMPWIN
  KEYBOARD_STATE  state;      //���
  KEYBOARD_INPUT  input;      //���͎��
  PRINT_UTIL      util;       //�����`��
  GFL_FONT        *p_font;    //�t�H���g
  PRINT_QUE       *p_que;     //�����L���[
  STRCODE         code;       //���͂����R�[�h
  HEAPID          heapID;     //�q�[�vID(Main���œǂ݂��݂����邽��)
  NAMEIN_INPUTTYPE mode;      //���̓��[�h�i���ȁEABC�Ƃ��j
  BOOL            is_shift;   //�V�t�g�������Ă��邩
  //�ȉ��A���[�h�ؑ։��o�p
  u16             change_move_cnt;  //���[�h�ؑւ̂Ƃ��̃A�j���J�E���g
  u16             change_move_seq;  //���[�h�ؑւ̂Ƃ��̃V�[�P���X
  BOOL            is_change_anm;    //���[�h�ؑւ̃A�j�����N�G�X�g
  NAMEIN_INPUTTYPE change_mode;     //���[�h�ؑւ�����̓��[�h�i���ȁEABC�Ƃ��j
  BOOL            is_btn_move;      //�{�^�������삷�邩

  void              *p_scr_adrs[2];
  NNSG2dScreenData  *p_scr[2];

  NAMEIN_KEYMAP_HANDLE *p_keymap_handle;

} KEYBOARD_WORK;
//-------------------------------------
/// �A�C�R��
//=====================================
typedef struct 
{
  GFL_CLWK  *p_clwk;  //�A�C�R����CLWK
  ICON_TYPE type;     //�A�C�R���̎��
  u32       plt;      //�p���b�g�o�^ID
  u32       ncg;      //�L�����o�^ID
  u32       cel;      //�Z���o�^ID
  BOOL      is_trg;   //�g���K�[����
  u32       seq;      //���C���V�[�P���X
  BOOL      is_move_start;//�ړ��J�n�t���O
  u16       sync;         //�ړ��V���N

  //�ȉ����ʕ����pOBJ
  u32               font_plt;
  STRBUF            *p_strbuf;  //�����o�b�t�@
  BMPOAM_ACT_PTR		p_bmpoam_wk;  //���ʗp
  GFL_BMP_DATA      *p_bmp;     //�����pBMP
  BOOL  is_trans_req;
  PRINT_QUE *p_que;
} ICON_WORK;
//-------------------------------------
/// ���C�����[�N
//=====================================
typedef struct 
{
  //�O���t�B�b�N�ݒ�
  NAMEIN_GRAPHIC_WORK *p_graphic;

  //OBJ���\�[�X
  OBJ_WORK      obj;

  //BG���\�[�X
  BG_WORK       bg;

  //���͕�����
  STRINPUT_WORK strinput;

  //�L�[�{�[�h
  KEYBOARD_WORK keyboard;

  //�L�[�}�b�v�f�[�^
  NAMEIN_KEYMAP_HANDLE *p_keymap_handle;

  //���ʃE�B���h�E
  MSGWND_WORK   msgwnd;

  //���ʃE�B���h�E�̃v�����g�X�g���[��
  PS_WORK       ps;

  //���������̃A�C�R��
  ICON_WORK     icon;

  //�V�[�P���X
  SEQ_WORK      seq;

  //���ʂŎg���t�H���g
  GFL_FONT      *p_font;

  //���ʂŎg���L���[
  PRINT_QUE     *p_que;

  //���ʂŎg�����b�Z�[�W
  GFL_MSGDATA   *p_msg;

  //���ʂŎg���P��
  WORDSET       *p_word;

  //���ʂŎg��BMPOAM
  BMPOAM_SYS_PTR p_bmpoam_sys;

  //����
  NAMEIN_PARAM  *p_param;

  //�s�������̏ꍇ�̃��b�Z�[�W
  BOOL  is_illegal_msg;

} NAMEIN_WORK;
