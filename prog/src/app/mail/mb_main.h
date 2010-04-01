//============================================================================================
/**
 * @file  mb_main.h
 * @brief ���[���{�b�N�X��� ���C������
 * @author  Hiroyuki Nakamura
 * @date  09.01.31
 */
//============================================================================================
#ifndef __MB_MAIN_H__
#define __MB_MAIN_H__

#include "savedata/mail_util.h"
#include "system/bmp_winframe.h"

#include "print/wordset.h"
#include "system/bgwinfrm.h"
#include "system/cursor_move.h"
#include "system/bmp_menuwork.h"
#include "system/bmp_menu.h"

#include "system/pms_draw.h"
#include "app/mailbox.h"
#include "app/app_taskmenu.h"

//============================================================================================
//  �萔��`
//============================================================================================

// �a�f�t���[��
#define MBMAIN_BGF_MSG_M    ( GFL_BG_FRAME0_M )
#define MBMAIN_BGF_STR_M    ( GFL_BG_FRAME1_M )
#define MBMAIN_BGF_BTN_M    ( GFL_BG_FRAME2_M )
#define MBMAIN_BGF_BG_M     ( GFL_BG_FRAME3_M )
#define MBMAIN_BGF_MAILMES_S  ( GFL_BG_FRAME0_S )
#define MBMAIN_BGF_MAIL_S   ( GFL_BG_FRAME1_S )
#define MBMAIN_BGF_STR_S    ( GFL_BG_FRAME2_S )
#define MBMAIN_BGF_BG_S     ( GFL_BG_FRAME3_S )

// �a�f�p���b�g
#define MBMAIN_MBG_PAL_YNWIN  ( 10 )    // �͂��E�������E�B���h�i�Q�{�j
#define MBMAIN_MBG_PAL_SELBOX ( 12 )
#define MBMAIN_MBG_PAL_TALKWIN  ( 13 )
#define MBMAIN_MBG_PAL_TALKFNT  ( 14 )
#define MBMAIN_MBG_PAL_SYSFNT ( 15 )
#define MBMAIN_SBG_PAL_MAILMSE  ( 1 )   // ���[�����b�Z�[�W
#define MBMAIN_SBG_PAL_SYSFNT ( 15 )

// ��b�E�B���h�E��`
#define MBMAIN_TALKWIN_CGX_SIZ  ( TALK_WIN_CGX_SIZ )
#define MBMAIN_TALKWIN_CGX_POS  ( 1024 - MBMAIN_TALKWIN_CGX_SIZ )
// �͂��E�������E�B���h�E��`
#define MBMAIN_YNWIN_CGX_SIZ  ( TOUCH_SW_USE_CHAR_NUM )
#define MBMAIN_YNWIN_CGX_POS  ( MBMAIN_TALKWIN_CGX_POS - MBMAIN_YNWIN_CGX_SIZ )
// SELBOX�E�B���h�E��`
#define MBMAIN_SELBOX_CGX_SIZ ( 13 * 13 )
#define MBMAIN_SELBOX_CGX_NUM ( MBMAIN_YNWIN_CGX_POS - MBMAIN_SELBOX_CGX_SIZ )
#define MBMAIN_SELBOX_FRM_NUM ( MBMAIN_SELBOX_CGX_NUM - SBOX_WINCGX_SIZ )


#define MBMAIN_MAILLIST_MAX   ( 10 )    // �P�y�[�W�ɕ\�����郁�[����
#define MBMAIN_MAILLIST_NULL  ( 0xff )  // ���[�����X�g�̃_�~�[�f�[�^

// BGWINFRM
enum {
  MBMAIN_BGWF_BUTTON1 = 0,
  MBMAIN_BGWF_SELMAIL = MBMAIN_BGWF_BUTTON1 + MBMAIN_MAILLIST_MAX,

  MBMAIN_BGWF_MAX
};

// BMPWIN
enum {
  MBMAIN_NULL_CHARA_M = 0,
  MBMAIN_NULL_CHARA_S,
  MBMAIN_BMPWIN_ID_MAIL01,  // ���[�����X�g
  MBMAIN_BMPWIN_ID_MAIL02,
  MBMAIN_BMPWIN_ID_MAIL03,
  MBMAIN_BMPWIN_ID_MAIL04,
  MBMAIN_BMPWIN_ID_MAIL05,
  MBMAIN_BMPWIN_ID_MAIL06,
  MBMAIN_BMPWIN_ID_MAIL07,
  MBMAIN_BMPWIN_ID_MAIL08,
  MBMAIN_BMPWIN_ID_MAIL09,
  MBMAIN_BMPWIN_ID_MAIL10,
  MBMAIN_BMPWIN_ID_MAIL11,
  MBMAIN_BMPWIN_ID_MAIL12,
  MBMAIN_BMPWIN_ID_MAIL13,
  MBMAIN_BMPWIN_ID_MAIL14,
  MBMAIN_BMPWIN_ID_MAIL15,
  MBMAIN_BMPWIN_ID_MAIL16,
  MBMAIN_BMPWIN_ID_MAIL17,
  MBMAIN_BMPWIN_ID_MAIL18,
  MBMAIN_BMPWIN_ID_MAIL19,
  MBMAIN_BMPWIN_ID_MAIL20,

  MBMAIN_BMPWIN_ID_SELMAIL,   // �I�����[��
  MBMAIN_BMPWIN_ID_CLOSE,     //�u�Ƃ���v
  MBMAIN_BMPWIN_ID_PAGE,      // �y�[�W

  MBMAIN_BMPWIN_ID_TALK,      // ��b���b�Z�[�W

  MBMAIN_BMPWIN_ID_TITLE,      //�u���[���{�b�N�X�v
  MBMAIN_BMPWIN_ID_MAILMSG1,   // ���[���̓��e�P
  MBMAIN_BMPWIN_ID_MAILMSG2,   // ���[���̓��e�Q
  MBMAIN_BMPWIN_ID_MAILMSG3,   // ���[���̓��e�R
  MBMAIN_BMPWIN_ID_PMSWORD,    // �ȈՒP��
  MBMAIN_BMPWIN_ID_NAME,       // �쐬�Җ�

  MBMAIN_BMPWIN_ID_MAX,
};

// OBJ
enum {
  MBMAIN_OBJ_ARROW_L = 0,
  MBMAIN_OBJ_ARROW_R,
  MBMAIN_OBJ_CURSOR,
  MBMAIN_OBJ_RET_BTN,

  MBMAIN_OBJ_POKEICON1,
  MBMAIN_OBJ_POKEICON2,
  MBMAIN_OBJ_POKEICON3,

  MBMAIN_OBJ_MAX
};

// �{�^���A�j�����[�h
enum {
  MBMAIN_BTN_ANM_MODE_OBJ = 0,
  MBMAIN_BTN_ANM_MODE_BG
};

// �{�^���A�j�����[�N
typedef struct {
  u8  btn_mode:1;
  u8  btn_id:7;     // OBJ = ID, BG = FRM
  u8  btn_pal1:4;
  u8  btn_pal2:4;
  u8  btn_seq;
  u8  btn_cnt;

  u8  btn_px;     // BG�̂�
  u8  btn_py;
  u8  btn_sx;
  u8  btn_sy;
}BUTTON_ANM_WORK;

// ���[���{�b�N�X��ʃA�v�����[�N
typedef struct {
  BGWINFRM_WORK * wfrm;   // BGWINFRM
  u8  btn_back_cgx[0x20*2]; // �{�^���w�i�L����

  GFL_BMPWIN  *win[MBMAIN_BMPWIN_ID_MAX];
  PRINT_UTIL    printUtil[MBMAIN_BMPWIN_ID_MAX];
  PRINT_QUE     *printQue;
  PRINT_STREAM  *printStream;
  GFL_TCBLSYS   *pMsgTcblSys;

  GFL_MSGDATA *mman;    // ���b�Z�[�W�f�[�^�}�l�[�W��
  WORDSET     *wset;    // �P��Z�b�g
  STRBUF      *expbuf;  // ���b�Z�[�W�W�J�̈�
  u8          msgID;    // �\�������b�Z�[�W�h�c
  u8          msgTrg;   // ���b�Z�[�W�g���K�[�E�F�C�g

  // �͂��E�������֘A
//  TOUCH_SW_SYS * tsw;   // �^�b�`�E�B���h�E
  u32 ynID;             // �͂��E�������h�c

  // ���j���[���X�g
  BMPLIST_DATA * sblist;
  u32 sb_old_pos;

  // OBJ�֘A
//  CATS_SYS_PTR  csp;
//  CATS_RES_PTR  crp;
//  CATS_ACT_PTR  cap[MBMAIN_OBJ_MAX];
  // �Z���A�N�^�[
  GFL_CLUNIT   *clUnit;
  GFL_CLWK     *clwk[MBMAIN_OBJ_MAX];
  u32          clres[4][MBMAIN_OBJ_MAX];  // �Z���A�N�^�[���\�[�X�p�C���f�b�N�X



  // �|�P�����A�C�R��
  ARCHANDLE * pokeicon_ah;

  CURSORMOVE_WORK * cmwk; // �J�[�\���ړ����[�N

  MAIL_DATA * mail[MAIL_STOCK_MAX]; // ���[���f�[�^
  u8  mail_list[MAIL_STOCK_MAX];    // ���[���f�[�^�̃C���f�b�N�X

  u16 mail_max;       // �L�����[���f�[�^��
  u16 page_max;       // �y�[�W�ő吔

  BUTTON_ANM_WORK bawk;   // �{�^���A�j�����[�N
  int wipe_seq;       // ���C�v��̃V�[�P���X

  // ���j���[���ʃ��\�[�X
  APP_TASKMENU_RES  *menures;
  // �^�b�`���j���[
  APP_TASKMENU_WORK     *menuwork;
  APP_TASKMENU_ITEMWORK menuitem[4];
  APP_TASKMENU_ITEMWORK yn_menuitem[2];

  // �ȈՉ�b�`��p���[�N
  PMS_DRAW_WORK     *pms_draw_work;
  PMS_DATA          tmpPms;         // �ȈՒP��\���p
  GFL_CLUNIT        *pmsClunit;
  PRINT_QUE         *pmsPrintque;

}MAILBOX_APP_WORK;

// ���[���{�b�N�X��ʃV�X�e�����[�N
typedef struct {
  MAIL_BLOCK * sv_mail;   // ���[���f�[�^
  MAILBOX_PARAM * dat;

  MAILBOX_APP_WORK * app;

  GFL_PROC *subProcFunc;   // �T�u�v���Z�X
  void     *subProcWork;   // �T�u�v���Z�X�Ŏg�p���郏�[�N
  u32       subProcType;   // �T�u�v���Z�X�̎��
  GFL_FONT *font;

  u16 sel_page;     // ���݂̃y�[�W
  u8  sel_pos;      // �I���ʒu
  u8  lst_pos;      // �I���������[���f�[�^�̈ʒu

  u8  ret_mode;     // �|�P�������X�g����̖߂�l
  u8  ret_poke:7;     // �|�P�������X�g�I�����̈ʒu
  u8  check_item:1;   // �o�b�O�ɖ߂����� 0 = ����, 1 = ���s
  u16 ret_item;     // �߂��A�C�e��
  BOOL  mw_ret;     // ���[���쐬����

  int next_seq;     // ���̃V�[�P���X

  GFL_TCB      *vintr_tcb; // �o�^����VblankFunc�̃|�C���^
  GFL_PROCSYS  *mbProcSys; // ���[���{�b�N�X���Ŏ��s����PROC�̃T�u�V�X�e��

}MAILBOX_SYS_WORK;

typedef int (*pMailBoxFunc)(MAILBOX_SYS_WORK*);

// ���j���[�ԍ�
enum {
  MBMAIN_MENU_READ = 0,   // ���[����ǂ�
  MBMAIN_MENU_DELETE,     // ���[��������
  MBMAIN_MENU_POKESET,    // ��������
  MBMAIN_MENU_CANCEL,     // ��߂�
  MBMAIN_MENU_MAX
};



//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * VBLANK�֐�
 *
 * @param work  ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_VBlank( GFL_TCB *tcb, void *work );

//--------------------------------------------------------------------------------------------
/**
 * VRAM�ݒ�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_VramInit(void);

//--------------------------------------------------------------------------------------------
/**
 * �a�f������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_BgInit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �a�f���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_BgExit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �a�f�O���t�B�b�N�ǂݍ���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_BgGraphicLoad( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �E�B���h�E�ǂݍ���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_WindowLoad( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * BGWINFRM������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_BgWinFrmInit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * BGWINFRM���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_BgWinFrmExit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_MsgInit( MAILBOX_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_MsgExit( MAILBOX_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * �͂��E����������������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_YesNoWinInit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_YesNoWinExit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_YesNoWinSet( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �I���{�b�N�X�쐬
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_SelBoxInit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �I���{�b�N�X�폜
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_SelBoxExit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �I���{�b�N�X�\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_SelBoxPut( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �I���{�b�N�X��\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_SelBoxDel( MAILBOX_SYS_WORK * syswk );


//--------------------------------------------------------------------------------------------
/**
 * ���[���f�[�^�擾
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_MailDataInit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ���[���f�[�^���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_MailDataExit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ���[���I���{�^��������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_MailButtonInit( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �I���������[���̃^�u�\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_SelMailTabPut( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �{�^���A�j������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "TRUE = �A�j����"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL MBMAIN_ButtonAnmMain( MAILBOX_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ���[���O���t�B�b�N�ǂݍ���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void MBMAIN_MailGraphcLoad( MAILBOX_SYS_WORK * syswk );

extern void MBMAIN_PmsDrawInit( MAILBOX_SYS_WORK * syswk );
extern void MBMAIN_PmsDrawExit( MAILBOX_SYS_WORK * syswk );


#endif  // __MB_MAIN_H__
