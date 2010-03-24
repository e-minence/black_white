//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   pl_wifi_note.c
 *  @brief    �F�B�蒠  �v���`�i�o�[�W����
 *  @author   tomoya takahshi
 *  @data   2007.07.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"

#include "msg/msg_wifi_note.h"

#include "sound/pm_sndsys.h"
#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"
#include "system/touch_subwindow.h"
#include "system/selbox.h"
#include "system/wipe.h"
#include "print/wordset.h"
#include "print/global_msg.h"
#include "print/str_tool.h"
#include "gamesystem\msgspeed.h"

#include "savedata/wifilist.h"

#include "app/wifi_note.h"
#include "app/codein.h"
#include "app/name_input.h"
#include "net_app/wifi2dmap/wf2dmap_common.h"
#include "net_app/wifi2dmap/wifi_2dchar.h"
#include "net_app/union/union_beacon_tool.h"
#include "net_app/union/union_gra_tool.h"
#include "net/dwc_rapfriend.h"

#include "system/tr2dgra.h"
#include "system/blink_palanm.h"
#include "app/app_menu_common.h"
#include "app/app_taskmenu.h"
#include "ui/touchbar.h"
#include "ui/print_tool.h"

#include "wifi_note.naix"
#include "wifi_note_snd.h"


#define USE_SEL_BOX (1)
#define USE_FRONTIER_DATA (0)
#define NOTE_TEMP_COMMENT (0)
#define NOTE_DEBUG (1)

#pragma mark[>define
//-----------------------------------------------------------------------------
/**
 *          �萔�錾
*/
//-----------------------------------------------------------------------------


// WIFI2DMAP�V�X�e���I�[�o�[���C
FS_EXTERN_OVERLAY(wifi2dmap);

//-------------------------------------
/// ������o�b�t�@�T�C�Y
//=====================================
#define WFNOTE_STRBUF_SIZE  (128)

//-------------------------------------
/// CLACT
//=====================================
#define CLACT_RESOURCE_NUM    ( 4 ) // ���\�[�X�}�l�[�W����
#define CLACT_WKNUM       ( 64 )  // ���[�N��
#define CLACT_RESNUM      ( 64 )  // �ǂݍ��ރ��\�[�X��
#define CLACT_MAIN2_REND_Y_S32  ( 0 ) // �X�N���[�������Ȃ��T�[�t�F�[�X�ʂ�Y���W�ʒu
#define CLACT_MAIN2_REND_Y    ( 0 ) // �X�N���[�������Ȃ��T�[�t�F�[�X�ʂ�Y���W�ʒu
#define CLACT_MAIN_VRAM_SIZ   ( 0x14000 )
#define CLACT_SUB_VRAM_SIZ    ( 0 )

// �p���b�g��`
#define CLACT_PALNUM_WIFINOTE   ( 0 )
#define CLACT_PALSIZ_WIFINOTE   ( 3 )
#define CLACT_PALNUM_TB         ( CLACT_PALNUM_WIFINOTE + CLACT_PALSIZ_WIFINOTE )
#define CLACT_PALSIZ_TB         ( 3 )
#define CLACT_PALNUM_TRGRA      ( CLACT_PALNUM_TB + CLACT_PALSIZ_TB )
#define CLACT_PALSIZ_TRGRA      ( 1 )
#define CLACT_PALNUM_WF2DC      ( CLACT_PALNUM_TRGRA + CLACT_PALSIZ_TRGRA )
#define CLACT_PALSIZ_WF2DC      ( 8 )

//-------------------------------------
/// VRAMTRANFER_MAN
//=====================================
#define VRAMTR_MAN_NUM    (16)

//-------------------------------------
/// BG
//=====================================
#define DFRM_BACK     (GFL_BG_FRAME0_M)
#define DFRM_MSG      (GFL_BG_FRAME1_M)
#define DFRM_SCROLL   (GFL_BG_FRAME2_M)
#define DFRM_SCRMSG   (GFL_BG_FRAME3_M)
//#define UFRM_MSG      (GFL_BG_FRAME0_S)
//#define UFRM_BASE     (GFL_BG_FRAME1_S)
#define UFRM_BACK     (GFL_BG_FRAME0_S)
#define WFNOTE_BG_NUM (5)

enum{ // �p���b�g�e�[�u��
  // ���C��
  BGPLT_M_BACK_0 = 0,     // �x�[�X
  BGPLT_M_BACK_1,         // �{�^���i�ʏ펞�j
  BGPLT_M_BACK_2,         // �{�^���i�I�����̃O���f�p�P�j
  BGPLT_M_BACK_3,         // �{�^���i�I�����̃O���f�p�P�j
  BGPLT_M_BACK_4,         // �{�^���i���ڂȂ��p�j
  BGPLT_M_BACK_5,         // �^�b�`�o�[�p
////////////////////////
  BGPLT_M_BACK_6,
  BGPLT_M_BACK_7,
  BGPLT_M_BACK_8,
//  BGPLT_M_BACK_9,
//  BGPLT_M_BACK_A,
////////////////////////
  BGPLT_M_TRGRA = 0x09,
  BGPLT_M_MSGFONT = 0x0A,
  BGPLT_M_TALKWIN = 0x0B,
  BGPLT_M_YNWIN = 0x0C,
  BGPLT_M_YNWIN2 = 0x0D,
  BGPLT_M_SBOX = 0x0E,

  // �T�u
  BGPLT_S_BACK_0 = 0,
  BGPLT_S_BACK_1,
  BGPLT_S_BACK_2,
  BGPLT_S_BACK_3,
  BGPLT_S_BACK_4,
  BGPLT_S_MSGFONT = 0x0E,
  BGPLT_S_TRGRA = 0x0F,
};
#define BGPLT_M_BACK_NUM  ((BGPLT_M_BACK_5+1) - BGPLT_M_BACK_0)
#define BGPLT_S_BACK_NUM  (BGPLT_M_BACK_NUM)

//�A�N�^�[�A�j��
enum{
  ACTANM_HATENA,
  ACTANM_CLEAR,
  ACTANM_PAGE_NUM1,
  ACTANM_PAGE_NUM2,
  ACTANM_PAGE_NUM3,
  ACTANM_PAGE_NUM4,
  ACTANM_NULL = 0xFF,
};


// ��{CGX  MAIN_MSG�ʂɓǂݍ��܂�܂�
#define BMPL_WIN_CGX_MENU (1)
//�͂��E���������j���[�̈�
#define BMPL_YESNO_PX   ( 25 )
#define BMPL_YESNO_PY   ( 9 )
#define BMPL_YESNO_FRM    ( DFRM_MSG )
#define BMPL_YESNO_PAL    ( BGPLT_M_YNWIN )
#define BMPL_YESNO_CGX_SIZ  ( TOUCH_SW_USE_CHAR_NUM )
#define BMPL_YESNO_CGX    ( BMPL_WIN_CGX_MENU+MENU_WIN_CGX_SIZ )

#define BMPL_WIN_CGX_TALK   (BMPL_YESNO_CGX+BMPL_YESNO_CGX_SIZ)
#define BMPL_WIN_CGX_TALKEND  (BMPL_WIN_CGX_TALK+TALK_WIN_CGX_SIZ)

// ��{�^�C�g���r�b�g�}�b�v
#define BMPL_TITLE_X  ( 1 )
#define BMPL_TITLE_Y  ( 0 )
#define BMPL_TITLE_SIZX ( 24 )
#define BMPL_TITLE_SIZY ( 3 )
#define BMPL_TITLE_CGX  ( BMPL_WIN_CGX_TALKEND )
#define BMPL_TITLE_CGX_END  ( BMPL_TITLE_CGX + (BMPL_TITLE_SIZX*BMPL_TITLE_SIZY) )
#define BMPL_TITLE_PL_Y ( 4 )

//�I���{�b�N�X�E�B���h�E�̈�
#define SBOX_FLIST_SEL_CT (4)
#define SBOX_FLIST_WCGX   (BMPL_TITLE_CGX_END)
#define SBOX_FLIST_W    (17)
#define SBOX_FLIST_FCGX_SIZ (SBOX_FLIST_W*2*SBOX_FLIST_SEL_CT)
#define SBOX_FLIST_FCGX   (SBOX_FLIST_WCGX+SBOX_WINCGX_SIZ)
#define SBOX_FLIST_PX   (16)
#define SBOX_FLIST_PY   (5)


// ��{OAM�̃f�[�^
#define WFNOTE_OAM_COMM_CONTID  ( 100 ) // ��{�IOAM�̊Ǘ�ID

// FONT�J���[
#define WFNOTE_COL_BLACK  ( PRINTSYS_LSB_Make( 1, 2, 0 ) )    // �t�H���g�J���[�F��
#define WFNOTE_COL_WHITE  ( PRINTSYS_LSB_Make( 15, 2, 0 ) )   // �t�H���g�J���[�F��
#define WFNOTE_COL_RED    ( PRINTSYS_LSB_Make( 3, 4, 0 ) )    // �t�H���g�J���[�F��
#define WFNOTE_COL_BLUE   ( PRINTSYS_LSB_Make( 5, 6, 0 ) )    // �t�H���g�J���[�F��
#define WFNOTE_TCOL_BLACK ( PRINTSYS_LSB_Make( 1, 15, 0 ) )   // �t�H���g�J���[�F�����̍�

#define WFNOTE_FONT_COL_WHITE (0xF)
// �`��G���A��
enum{
  WFNOTE_DRAWAREA_MAIN,
  WFNOTE_DRAWAREA_RIGHT,
  WFNOTE_DRAWAREA_LEFT,
  WFNOTE_DRAWAREA_NUM,
};

//  ��{�X�N���[���f�[�^
enum{
  WFNOTE_SCRNDATA_BACK, // �w�i�p
  WFNOTE_SCRNDATA_KIT,    // �������ݗp
  WFNOTE_SCRNDATA_NUM,    //
};

//-------------------------------------
/// STATUS
//=====================================
enum {
  STATUS_MODE_SELECT, // �ŏ��̑I�����
  STATUS_FRIENDLIST,  // �F�B�f�[�^�\��
  STATUS_CODEIN,    // �F�B�R�[�h����
  STATUS_MYCODE,    // �����̗F�B�R�[�h�m�F
  STATUS_FRIENDINFO,  // �F�B�ڍ׃f�[�^
  STATUS_END,     // �I���i�L�����Z���j
  STATUS_END_X,     // �I���i�w�{�^���j
  STATUS_NUM
} ;
typedef enum{
  WFNOTE_STRET_CONTINUE,  // �Â�
  WFNOTE_STRET_FINISH,    // ��Ƃ͏I�����
} WFNOTE_STRET;
#define STATUS_CHANGE_WAIT  (4) // �ύX���E�G�C�g

//-------------------------------------
/// PROC SEQ
//=====================================
enum {
  WFNOTE_PROCSEQ_MAIN,
  WFNOTE_PROCSEQ_WAIT,
} ;

//-------------------------------------
/// �F�B�f�[�^�V�K�ݒ�֐��@�߂�l
//=====================================
enum{
  RCODE_NEWFRIEND_SET_OK, // �ݒ�ł���
  RCODE_NEWFRIEND_SET_FRIENDKEYNG,  // �F�B�R�[�h���s��
  RCODE_NEWFRIEND_SET_SETING, // �����ݒ肵�Ă�����
  RCODE_NEWFRIEND_SET_NUM

};

// ���f�[�^
#define WFNOTE_YAZIRUSHINUM   (2) // ���̐�
#define WFNOTE_YAZIRUSHI_LX   (8)
#define WFNOTE_YAZIRUSHI_RX   (248)
#define WFNOTE_YAZIRUSHI_LRY  (98)
#define WFNOTE_YAZIRUSHI_PRI  ( 8 )

#define WFNOTE_YAZIRUSHI_TBX  (256-12)
#define WFNOTE_YAZIRUSHI_TY   (3*8)
#define WFNOTE_YAZIRUSHI_BY   (17*8)

enum{
/*
  ACT_YAZI_L,
  ACT_YAZI_R,
*/
  ACT_YAZI_T,
  ACT_YAZI_B,
};
enum{
  ACT_YAZIANM_NORMAL,
  ACT_YAZIANM_ACTIVE,
};


//�J�[�\���f�[�^
#define WFNOTE_CURSOR_PRI   (9)
#define WFNOTE_CURSOR_BGPRI   (1)

// �^�C�g�����b�Z�[�W�X�s�[�h
#define WFNOTE_TITLE_MSG_SPEED  ( 2 )



//-------------------------------------
/// MODESELECT
//=====================================
// BMP
#define BMPL_MODESEL_MSG_X    ( 4 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MODESEL_MSG_Y    ( 5 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MODESEL_MSG_SIZX ( 24 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_MODESEL_MSG_SIZY ( 20 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_MODESEL_MSGCGX   ( 1 ) // �L�����N�^�I�t�Z�b�g

#define BMPL_MODESEL_TALK_X   ( 2 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MODESEL_TALK_Y   ( 19 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MODESEL_TALK_SIZX    ( 27 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_MODESEL_TALK_SIZY    ( 4 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_MODESEL_TALKCGX    ( BMPL_TITLE_CGX_END )  // �L�����N�^�I�t�Z�b�g

#define BMPL_MODESEL_YESNO_FRM    ( DFRM_MSG )
#define BMPL_MODESEL_YESNO_X    ( 25 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MODESEL_YESNO_Y    ( 13 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MODESEL_YESNO_SIZX   ( 6 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_MODESEL_YESNO_SIZY   ( 4 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_MODESEL_YESNOCGX   ( BMPL_MODESEL_TALKCGX+(BMPL_MODESEL_TALK_SIZX*BMPL_MODESEL_TALK_SIZY) )  // �L�����N�^�I�t�Z�b�g

// ����V�[�P���X
enum{
  SEQ_MODESEL_INIT,
  SEQ_MODESEL_FADEINWAIT,
  SEQ_MODESEL_INIT_NOFADE,
  SEQ_MODESEL_MAIN,
  SEQ_MODESEL_MAIN_ENDWAIT,
  SEQ_MODESEL_FADEOUT,
  SEQ_MODESEL_FADEOUTWAIT,

  // �R�[�h���͂���߂��Ă����Ƃ��p
  SEQ_MODESEL_CODECHECK_INIT,
  SEQ_MODESEL_CODECHECK_FADEINWAIT,
  SEQ_MODESEL_CODECHECK_MSGWAIT,    //�o�^�m�F���b�Z�[�WON
  SEQ_MODESEL_CODECHECK_MSGYESNOWAIT, //�͂��������I��҂�
  SEQ_MODESEL_CODECHECK_RESLUTMSGWAIT,  //�I�����ʕ\��
};
// �J�[�\��
enum{
  MODESEL_CUR_FLIST,
  MODESEL_CUR_CODEIN,
  MODESEL_CUR_MYCODE,
  MODESEL_CUR_END,
  MODESEL_CUR_END_X,
  MODESEL_CUR_NUM,
};
#define MODESEL_CUR_CSIZX ( 28 )  // �J�[�\�������̉��T�C�Y�i�L�����j
#define MODESEL_CUR_CSIZY ( 4 ) // �J�[�\�������̏c�T�C�Y�i�L�����j
#define MODESEL_CUR_PAL_ON  ( BGPLT_M_BACK_8 )  // �I����
#define MODESEL_CUR_PAL_ANM ( BGPLT_M_BACK_8 )  // �I����
#define MODESEL_CUR_PAL_OFF ( BGPLT_M_BACK_2 )  // ��I����


//-------------------------------------
/// FRIENDLIST
//=====================================
#define FLIST_PAGE_FRIEND_NUM (8) // 1�y�[�W�ɂ���l�̐�
#define FLIST_PAGE_FRIEND_HALF  (FLIST_PAGE_FRIEND_NUM/2) // 1�y�[�W�ɂ���l�̐�
#define FLIST_PAGE_MAX      (4) // �y�[�W����
#define FLIST_FRIEND_MAX    ( FLIST_PAGE_FRIEND_NUM*FLIST_PAGE_MAX )  // �\���ł���F�B�̍ő�l
#define FLIST_2DCHAR_NUM    (16)// 2dchar���[�N��

enum{
  FLIST_MAIN_EXE_CANCEL,
  FLIST_MAIN_EXE_LEFT,
  FLIST_MAIN_EXE_RIGHT,
  FLIST_MAIN_EXE_UP,
  FLIST_MAIN_EXE_DOWN,
  FLIST_MAIN_EXE_DECIDE,
  FLIST_MAIN_EXE_EXIT,
};

enum{ // �J�[�\���f�[�^����
  FLIST_CURSORDATA_0,
  FLIST_CURSORDATA_1,
  FLIST_CURSORDATA_2,
  FLIST_CURSORDATA_3,
  FLIST_CURSORDATA_4,
  FLIST_CURSORDATA_5,
  FLIST_CURSORDATA_6,
  FLIST_CURSORDATA_7,
  FLIST_CURSORDATA_BACK,
  FLIST_CURSORDATA_EXIT,
  FLIST_CURSORDATA_NUM,
  FLIST_CURSORDATA_OFF, // �J�[�\���������Ƃ���POS
};

//////////////////////////////////////////
// �r�b�g�}�b�v
//////////////////////////////////////////
#define BMPL_FLIST_TEXT_OFSX  ( 4 ) // �J�n�ʒu��
#define BMPL_FLIST_TEXT_OFSY  ( 4 ) // �J�n�ʒu��
#define BMPL_FLIST_TEXT_SIZX  ( 26 )  // �T�C�Y��
#define BMPL_FLIST_TEXT_SIZY  ( 17 )  // �T�C�Y��
#define BMPL_FLIST_TEXT_CGX0  ( 1 ) // cgx�̈�1
#define BMPL_FLIST_TEXT_CGX1  ( BMPL_FLIST_TEXT_CGX0+(BMPL_FLIST_TEXT_SIZX*BMPL_FLIST_TEXT_SIZY) )  // cgx�̈�2

#define BMPL_FLIST_BKMSG_X  ( 21 )  // �J�n�ʒu��
#define BMPL_FLIST_BKMSG_Y  ( 21 )  // �J�n�ʒu��
#define BMPL_FLIST_BKMSG_SIZX ( 9 ) // �T�C�Y��
#define BMPL_FLIST_BKMSG_SIZY ( 2 ) // �T�C�Y��
#define BMPL_FLIST_BKMSG_CGX  (SBOX_FLIST_FCGX+SBOX_FLIST_FCGX_SIZ )

#define BMPL_FLIST_TALK_X   ( 2 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FLIST_TALK_Y   ( 1 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FLIST_TALK_SIZX  ( 27 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_FLIST_TALK_SIZY  ( 4 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FLIST_TALK_CGX   (BMPL_FLIST_BKMSG_CGX+(BMPL_FLIST_BKMSG_SIZX*BMPL_FLIST_BKMSG_SIZY))  // �L�����N�^�I�t�Z�b�g

#define BMPL_FLIST_YESNO_FRM    ( GFL_BG_FRAME1_M )
#define BMPL_FLIST_YESNO_X    ( 25 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FLIST_YESNO_Y    ( 13 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FLIST_YESNO_SIZX   ( 6 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FLIST_YESNO_SIZY   ( 4 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FLIST_YESNO_CGX    ( BMPL_FLIST_TALK_CGX+(BMPL_FLIST_TALK_SIZX*BMPL_FLIST_TALK_SIZY) ) // �L�����N�^�I�t�Z�b�g


// ����V�[�P���X
enum{
  SEQ_FLIST_INIT_PAGEINIT,
  SEQ_FLIST_INIT,
  SEQ_FLIST_MAIN,
  SEQ_FLIST_SCRLLINIT,
  SEQ_FLIST_SCRLL,

  SEQ_FLIST_MENUINIT, //�@���j���[�I��
  SEQ_FLIST_MENUWAIT, // ���j���[�I��҂�
  SEQ_FLIST_INFOINIT,   // �ڂ��������
  SEQ_FLIST_INFO,   // �ڂ��������
  SEQ_FLIST_INFORET,  // �ڂ��������
  SEQ_FLIST_NAMECHG_INIT, // ���O�ύX
  SEQ_FLIST_NAMECHG_WIPE, // ���O�ύX
  SEQ_FLIST_NAMECHG_WAIT, // ���O�ύX
  SEQ_FLIST_NAMECHG_WAITWIPE, // ���O�ύX
  SEQ_FLIST_NAMECHG_WAITWIPEWAIT, // ���O�ύX
  SEQ_FLIST_DELETE_INIT,  // �j��
  SEQ_FLIST_DELETE_YESNODRAW, // �j��
  SEQ_FLIST_DELETE_WAIT,  // �j��YESNO���܂��Ă��珈��
  SEQ_FLIST_DELETE_ANMSTART,  // �j��YESNO���܂��Ă��珈��
  SEQ_FLIST_DELETE_ANMWAIT, // �j��YESNO���܂��Ă��珈��
  SEQ_FLIST_DELETE_END,   //  �j��YESNO���܂��Ă��珈��
  SEQ_FLIST_CODE_INIT,  // �R�[�h�؂�ւ�
  SEQ_FLIST_CODE_WAIT,  // �����L�[�������Ă��炤
  SEQ_FLIST_END,
  SEQ_FLIST_END_X,
  SEQ_FLIST_END_FADEOUT,
  SEQ_FLIST_END_FADEOUT_WAIT,
};

// ������
#define FLIST_SHIORI_X    ( 16 )    // �ʒu�i�L�����T�C�Y�j
#define FLIST_SHIORI_Y    ( 0 )   // �ʒu(�L�����T�C�Y)
#define FLIST_SHIORI_SIZX ( 16 )    // �T�C�Y�i�L�����N�^�T�C�Y�j
#define FLIST_SHIORI_SIZY ( 4 )   // �T�C�Y�i�L�����N�^�T�C�Y�j
#define FLIST_SHIORI_ONEX ( 4 )   // �P�y�[�W���̃T�C�Y
#define FLIST_SHIORI_OFSX ( 0 )   // ONEX�̒��ŕ\������L�����N�^�܂ł̐�
#define FLIST_SHIORI_OFSY ( 0 )   // ONEX�̒��ŕ\������L�����N�^�܂ł̐�
#define FLIST_SHIORISCRN_X  ( 0 )   // �X�N���[���ǂݍ��݂�
#define FLIST_SHIORISCRN_Y  ( 4 )   // �X�N���[���ǂݍ��݂�
#define FLIST_CL_SHIORISCRN_X ( 16 )    // �X�N���[���ǂݍ��݂�
#define FLIST_CL_SHIORISCRN_Y ( 4 )   // �X�N���[���ǂݍ��݂�
#define FLIST_SHIORISCRN_POS_X  ( 0 ) // �|�W�V�����X�N���[���ǂݍ��݂�
#define FLIST_SHIORISCRN_POS_Y  ( 8 ) // �|�W�V�����X�N���[���ǂݍ��݂�
#define FLIST_EFF_SHIORISCRN_POS_X  ( 16 )  // �G�t�F�N�g�p�|�W�V�����X�N���[���ǂݍ��݂�
#define FLIST_EFF_SHIORISCRN_POS_Y  ( 8 ) // �G�t�F�N�g�p�|�W�V�����X�N���[���ǂݍ��݂�

#define FLIST_SHIORIPOS_SIZ (4)     // �|�W�V�����X�N���[���̑傫��

// ���ǂ�\���ʒu
#define FLIST_BACK_X    ( 17 )  // �߂�\���ʒu
#define FLIST_BACK_Y    ( 20 )  // �߂�\���ʒu
#define FLIST_BACK_SIZX   ( 14 )  // �߂�\���T�C�Y
#define FLIST_BACK_SIZY   ( 4 ) // �߂�\���T�C�Y
#define FLIST_SCRBACK_X   ( 3 ) // �߂�ǂݍ��ݐ�ʒu
#define FLIST_SCRBACK_Y   ( 0 ) // �߂�ǂݍ��ݐ�ʒu
#define FLIST_CL_SCRBACK_X    ( 17 )  // �߂�ǂݍ��ݐ�ʒu
#define FLIST_CL_SCRBACK_Y    ( 0 ) // �߂�ǂݍ��ݐ�ʒu

// �v���C���[�\���ʒu�iDATA_FListCursorData�Ȃ��̈ʒu�f�[�^����̃I�t�Z�b�g�j
#define FLIST_PLAYER_X    ( 8 )
#define FLIST_PLAYER_Y    ( 6+CLACT_MAIN2_REND_Y_S32 )
#define FLIST_PLAYER_PRI      ( 8 )
#define FLIST_PLAYER_BGPRI      ( 1 )
// �H�\���ʒu�iDATA_FListCursorData�Ȃ��̈ʒu�f�[�^����̃I�t�Z�b�g�j
#define FLIST_HATENA_X    ( 16 )
#define FLIST_HATENA_Y    ( 16+CLACT_MAIN2_REND_Y_S32 )
//�@�J�[�\���\���v���C�I���e�B
#define FLIST_CURSOR_PRI    (FLIST_PLAYER_PRI+8)
#define FLIST_CURSOR_BGPRI    (FLIST_PLAYER_BGPRI)

// �͂ĂȂ̏ꍇ
#define FLIST_NONPLAYER_SIZX  (3)
#define FLIST_NONPLAYER_SIZY  (4)
#define FLIST_NONPLAYER_SCRN_X  (0)
#define FLIST_NONPLAYER_SCRN_Y  (0)

// �v���C���[���\���ʒu�iDATA_FListCursorData�Ȃ��̈ʒu�f�[�^����̃I�t�Z�b�g�j
#define FLIST_PLAYERNAME_X  ( 0 )
#define FLIST_PLAYERNAME_Y  ( -24 )

// SEQ_MAIN�֐��߂�l
enum{
  RCODE_FLIST_SEQMAIN_NONE,
  RCODE_FLIST_SEQMAIN_PCHG_LEFT,// �y�[�W�ύX
  RCODE_FLIST_SEQMAIN_PCHG_RIGHT,// �y�[�W�ύX
  RCODE_FLIST_SEQMAIN_CANCEL, // �߂�
  RCODE_FLIST_SEQMAIN_EXIT, // �I��
  RCODE_FLIST_SEQMAIN_SELECTPL, // �l��I������
};


// �X�N���[��
#define FLIST_SCROLL_COUNT  (24)  // �X�N���[���J�E���g�i4�̔{���j
#define FLIST_SCROLL_SIZX (256) // �X�N���[���T�C�Y


// ���j���[�f�[�^
enum{
  BMPL_FLIST_MENU_NML,    // �ʏ탁�j���[
  BMPL_FLIST_MENU_CODE, // �F�B�ԍ��̂ݓo�^���̃��j���[
  BMPL_FLIST_MENU_NUM,
};
#define BMPL_FLIST_MENU_LISTNUM (SBOX_FLIST_SEL_CT) // ���j���[���X�g�̍��ڐ�

// �ڍ׉�ʂփt�F�[�h�̃E�G�C�g
#define FLIST_INFO_WAIT (8)

//  �����S���A�N�^�[
#define FLIST_CLEARACT_Y  ( -4 + CLACT_MAIN2_REND_Y_S32 )

#define TP_FLIST_BTN_W    (14*8)
#define TP_FLIST_BTN_H    (4*8)
#define TP_FLIST_BTN_X01  (2*8)
#define TP_FLIST_BTN_X02  (16*8)
#define TP_FLIST_BTN_OY   (4*8)
#define TP_FLIST_BTN_Y01  (4*8)
#define TP_FLIST_BTN_Y02  (TP_FLIST_BTN_Y01+TP_FLIST_BTN_OY)
#define TP_FLIST_BTN_Y03  (TP_FLIST_BTN_Y02+TP_FLIST_BTN_OY)
#define TP_FLIST_BTN_Y04  (TP_FLIST_BTN_Y03+TP_FLIST_BTN_OY)

#define TP_FLIST_BACK_X   (17*8)
#define TP_FLIST_BACK_Y   (TP_FLIST_BTN_Y04+TP_FLIST_BTN_OY)

#define TP_FLIST_YAZI_W   (16)
#define TP_FLIST_YAZI_H   (16)
#define TP_FLIST_YAZI_Y   (11*8)
#define TP_FLIST_YAZIL_X  (0)
#define TP_FLIST_YAZIR_X  (30*8)

#define TP_FLIST_BM_W     (24)
#define TP_FLIST_BM_H     (24)
#define TP_FLIST_BM_Y     (168)
#define TP_FLIST_BM01_X   (8)
#define TP_FLIST_BM02_X   (TP_FLIST_BM01_X+TP_FLIST_BM_W)
#define TP_FLIST_BM03_X   (TP_FLIST_BM02_X+TP_FLIST_BM_W)
#define TP_FLIST_BM04_X   (TP_FLIST_BM03_X+TP_FLIST_BM_W)

//-------------------------------------
/// CODEIN
//=====================================
enum{ // �V�[�P���X
  SEQ_CODEIN_NAMEIN,
  SEQ_CODEIN_NAMEIN_WAIT,
  SEQ_CODEIN_CODEIN_WAIT,
  SEQ_CODEIN_END,

  SEQ_CODEIN_NAMEINONLY,  // ���O���͂̂�
  SEQ_CODEIN_NAMEINONLY_WAIT, // ���O���͂̂�
  SEQ_CODEIN_NAMEINONLY_END,  // ���O���͂̂�

};
#define FRIENDCODE_MAXLEN (12)  // �F�B�R�[�h������

//-------------------------------------
/// MYCODE
//=====================================
// BMP  ���b�Z�[�W
#define BMPL_MYCODE_MSG_X   ( 2 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MYCODE_MSG_Y   ( 10 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MYCODE_MSG_SIZX  ( 30 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_MYCODE_MSG_SIZY  ( 8 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_MYCODE_MSG_CGX   ( 1 ) // �L�����N�^�I�t�Z�b�g
// BMP  �R�[�h
#define BMPL_MYCODE_CODE_X    ( 9 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MYCODE_CODE_Y    ( 6 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MYCODE_CODE_SIZX ( 14 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_MYCODE_CODE_SIZY ( 2 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_MYCODE_CODE_CGX  ( BMPL_MYCODE_MSG_CGX+(BMPL_MYCODE_MSG_SIZX*BMPL_MYCODE_MSG_SIZY) ) // �L�����N�^�I�t�Z�b�g
// BMP  �߂�{�^��
#define BMPL_MYCODE_BACK_X    ( 21 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MYCODE_BACK_Y    ( 21 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_MYCODE_BACK_SIZX ( 9 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_MYCODE_BACK_SIZY ( 2 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_MYCODE_BACK_CGX  ( BMPL_MYCODE_CODE_CGX+(BMPL_MYCODE_CODE_SIZX*BMPL_MYCODE_CODE_SIZY) )  // �L�����N�^�I�t�Z�b�g

// �V�[�P���X
enum{
  SEQ_MYCODE_INIT,
  SEQ_MYCODE_MAIN,
  SEQ_MYCODE_BTNANM,
  SEQ_MYCODE_BTNANM_X,
  SEQ_MYCODE_FADEOUT,
  SEQ_MYCODE_FADEOUT_WAIT,
};

//-------------------------------------
/// FRIENDINFO
//=====================================
#define FINFO_PAGE_NUM          ( 8 ) // �y�[�W��
#define FINFO_BFOFF_PAGE_NUM    ( 3 ) // �t�����e�B�A�����y�[�W��

//���s�R�[�h
enum{
//  FINFO_EXE_DECIDE,
  FINFO_EXE_CANCEL,
  FINFO_EXE_EXIT,
//  FINFO_EXE_LEFT,
//  FINFO_EXE_RIGHT,
  FINFO_EXE_TOP,
  FINFO_EXE_BOTTOM,
};

// �V�[�P���X
enum{
  SEQ_FINFO_INIT,
  SEQ_FINFO_INITRP,
  SEQ_FINFO_MAIN,
  SEQ_FINFO_SCRINIT,
  SEQ_FINFO_SCR,
  SEQ_FINFO_SCR_WAIT_MSG,
  SEQ_FINFO_FRIENDCHG,
  SEQ_FINFO_END,
  SEQ_FINFO_END_X,
  SEQ_FINFO_END_FADEOUT,
  SEQ_FINFO_END_FADEOUT_WAIT,
};

//�X�N���[��
enum{
 FINFO_SCRNDATA_MB,
 FINFO_SCRNDATA_M,
 FINFO_SCRNDATA_MO,
 FINFO_SCRNDATA_S,
 FINFO_SCRNDATA_NUM
};

/////////////////////////////////
// �r�b�g�}�b�v�����
/////////////////////////////////
#define BMPL_FINFO_TEXT_CGX0  ( 1 ) // �T�C�Y��
#define BMPL_FINFO_TEXT_CGX1  ( BMPL_FINFO_TEXT_CGX0+(500) )  // �T�C�Y��

/////////////////////////////////
//�r�b�g�}�b�v����
/////////////////////////////////
enum{
 FINFO_WIN_PAGE,          // �y�[�W
 FINFO_WIN_RECORD,        //�u�������񂹂������v
 FINFO_WIN_WIN,           //�u�����v
 FINFO_WIN_LOSE,          //�u�܂��v
 FINFO_WIN_WIN_NUM,       // ������
 FINFO_WIN_LOSE_NUM,      // ������
 FINFO_WIN_CHANGE,        //�u�|�P������������v
 FINFO_WIN_CHANGE_NUM,    // ������
 FINFO_WIN_LAST_DAY,      //�u�������ɂ����񂾂ЂÂ��v
 FINFO_WIN_LAST_DAY_NUM,  // �Ō�ɗV�񂾓��t
/*
 FINFO_WIN_NAME,    
 FINFO_WIN_GRP02,
 FINFO_WIN_DAY02,
 FINFO_WIN_TRGRA,
 FINFO_WIN_GRP01,
 FINFO_WIN_DAY01,
*/
 FINFO_WIN_MAX,
};

#define BMPL_FINFO_FRM      ( DFRM_MSG )
#define BMPL_FINFO_PAL      ( BGPLT_M_MSGFONT )
// �y�[�W
#define BMPL_FINFO_WIN_PAGE_PX  ( 26 )
#define BMPL_FINFO_WIN_PAGE_PY  ( 1 )
#define BMPL_FINFO_WIN_PAGE_SX  ( 5 )
#define BMPL_FINFO_WIN_PAGE_SY  ( 2 )
//�u�������񂹂������v
#define BMPL_FINFO_WIN_RECORD_PX  ( 14 )
#define BMPL_FINFO_WIN_RECORD_PY  ( 5 )
#define BMPL_FINFO_WIN_RECORD_SX  ( 12 )
#define BMPL_FINFO_WIN_RECORD_SY  ( 2 )
//�u�����v
#define BMPL_FINFO_WIN_WIN_PX ( 23 )
#define BMPL_FINFO_WIN_WIN_PY ( 8 )
#define BMPL_FINFO_WIN_WIN_SX ( 3 )
#define BMPL_FINFO_WIN_WIN_SY ( 2 )
//�u�܂��v
#define BMPL_FINFO_WIN_LOSE_PX  ( 23 )
#define BMPL_FINFO_WIN_LOSE_PY  ( 10 )
#define BMPL_FINFO_WIN_LOSE_SX  ( 3 )
#define BMPL_FINFO_WIN_LOSE_SY  ( 2 )
// ������
#define BMPL_FINFO_WIN_WIN_NUM_PX ( 27 )
#define BMPL_FINFO_WIN_WIN_NUM_PY ( 8 )
#define BMPL_FINFO_WIN_WIN_NUM_SX ( 4 )
#define BMPL_FINFO_WIN_WIN_NUM_SY ( 2 )
// ������
#define BMPL_FINFO_WIN_LOSE_NUM_PX  ( 27 )
#define BMPL_FINFO_WIN_LOSE_NUM_PY  ( 10 )
#define BMPL_FINFO_WIN_LOSE_NUM_SX  ( 4 )
#define BMPL_FINFO_WIN_LOSE_NUM_SY  ( 2 )
//�u�|�P������������v
#define BMPL_FINFO_WIN_CHANGE_PX  ( 14 )
#define BMPL_FINFO_WIN_CHANGE_PY  ( 13 )
#define BMPL_FINFO_WIN_CHANGE_SX  ( 12 )
#define BMPL_FINFO_WIN_CHANGE_SY  ( 2 )
// ������
#define BMPL_FINFO_WIN_CHANGE_NUM_PX  ( 27 )
#define BMPL_FINFO_WIN_CHANGE_NUM_PY  ( 13 )
#define BMPL_FINFO_WIN_CHANGE_NUM_SX  ( 4 )
#define BMPL_FINFO_WIN_CHANGE_NUM_SY  ( 2 )
//�u�������ɂ����񂾂ЂÂ��v
#define BMPL_FINFO_WIN_LAST_DAY_PX  ( 2 )
#define BMPL_FINFO_WIN_LAST_DAY_PY  ( 17 )
#define BMPL_FINFO_WIN_LAST_DAY_SX  ( 17 )
#define BMPL_FINFO_WIN_LAST_DAY_SY  ( 2 )
// �Ō�ɗV�񂾓��t
#define BMPL_FINFO_WIN_LAST_DAY_NUM_PX  ( 20 )
#define BMPL_FINFO_WIN_LAST_DAY_NUM_PY  ( 17 )
#define BMPL_FINFO_WIN_LAST_DAY_NUM_SX  ( 10 )
#define BMPL_FINFO_WIN_LAST_DAY_NUM_SY  ( 2 )

/*
//�y�[�W
#define BMPL_FINFO_PAGE_X   ( 26 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_PAGE_Y   ( 1 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_PAGE_SIZX  ( 5)  // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_PAGE_SIZY  ( 2 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_PAGE_SIZ   ( BMPL_FINFO_PAGE_SIZX * BMPL_FINFO_PAGE_SIZY ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_PAGE_CGX   ( 1 ) // �L�����N�^�I�t�Z�b�g
// BMP �^�C�g��
#define BMPL_FINFO_TITLE_X    ( BMPL_TITLE_X )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_TITLE_Y    ( BMPL_TITLE_Y )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_TITLE_SIZX ( BMPL_TITLE_SIZX ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_TITLE_SIZY ( BMPL_TITLE_SIZY ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_TITLE_SIZ  ( BMPL_TITLE_SIZX * BMPL_TITLE_SIZY ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_TITLE_CGX  ( BMPL_FINFO_PAGE_CGX+BMPL_FINFO_PAGE_SIZ ) // �L�����N�^�I�t�Z�b�g
//���O
#define BMPL_FINFO_NAME_X   ( 22 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_NAME_Y   ( 6 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_NAME_SIZX  ( 9 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_NAME_SIZY  ( 2 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_NAME_SIZ   ( BMPL_FINFO_NAME_SIZX * BMPL_FINFO_NAME_SIZY ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_NAME_CGX   ( BMPL_FINFO_TITLE_CGX+BMPL_FINFO_TITLE_SIZ ) // �L�����N�^�I�t�Z�b�g
//�O���[�v
#define BMPL_FINFO_GRP01_X    ( 1 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_GRP01_Y    ( 17 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_GRP01_SIZX ( 6 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_GRP01_SIZY ( 2 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_GRP01_SIZ  ( BMPL_FINFO_GRP01_SIZX * BMPL_FINFO_GRP01_SIZY ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_GRP01_CGX  ( BMPL_FINFO_NAME_CGX+BMPL_FINFO_NAME_SIZ ) // �L�����N�^�I�t�Z�b�g

#define BMPL_FINFO_GRP02_X    ( 9 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_GRP02_Y    ( 17 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_GRP02_SIZX ( 8 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_GRP02_SIZY ( 2 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_GRP02_SIZ  ( BMPL_FINFO_GRP02_SIZX * BMPL_FINFO_GRP02_SIZY ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_GRP02_CGX  ( BMPL_FINFO_GRP01_CGX+BMPL_FINFO_GRP01_SIZ ) // �L�����N�^�I�t�Z�b�g

//���t
#define BMPL_FINFO_DAY01_X    ( 1 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_DAY01_Y    ( 21 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_DAY01_SIZX ( 17 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_DAY01_SIZY ( 2 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_DAY01_SIZ  ( BMPL_FINFO_DAY01_SIZX * BMPL_FINFO_DAY01_SIZY ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_DAY01_CGX  ( BMPL_FINFO_GRP02_CGX+BMPL_FINFO_GRP02_SIZ ) // �L�����N�^�I�t�Z�b�g

#define BMPL_FINFO_DAY02_X    ( 19 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_DAY02_Y    ( 21 )  // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_DAY02_SIZX ( 10 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_DAY02_SIZY ( 2 ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_DAY02_SIZ  ( BMPL_FINFO_DAY02_SIZX * BMPL_FINFO_DAY02_SIZY ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_DAY02_CGX  ( BMPL_FINFO_DAY01_CGX+BMPL_FINFO_DAY01_SIZ ) // �L�����N�^�I�t�Z�b�g
*/

//�g���[�i�[�O���t�B�b�N
#define BMPL_FINFO_TRGRA_X    ( 2 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_TRGRA_Y    ( 5 ) // �J�n�ʒu�i�L�����P�ʁj
#define BMPL_FINFO_TRGRA_SIZX ( 10 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_TRGRA_SIZY ( 10 )  // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_TRGRA_SIZ  ( BMPL_FINFO_TRGRA_SIZX * BMPL_FINFO_TRGRA_SIZY ) // �T�C�Y�i�L�����P�ʁj
#define BMPL_FINFO_TRGRA_CGX  ( BMPL_FINFO_DAY02_CGX+BMPL_FINFO_DAY02_SIZ ) // �L�����N�^�I�t�Z�b�g
#define BMPL_FINFO_TRGRA_PAL  ( BGPLT_M_TRGRA )
#define BMPL_FINFO_TRGRA_CUTX ( 0 )
#define BMPL_FINFO_TRGRA_CUTY ( 0 )

// ������
#define FINFO_SHIORI_X    ( 0 ) // �ʒu�i�L�����T�C�Y�j
#define FINFO_SHIORI_Y    ( 20 )    // �ʒu(�L�����T�C�Y)
#define FINFO_SHIORI_SIZX ( 32 )    // �T�C�Y�i�L�����N�^�T�C�Y�j
#define FINFO_SHIORI_SIZY ( 4 )   // �T�C�Y�i�L�����N�^�T�C�Y�j
#define FINFO_SHIORI_ONEX ( 1 )   // �P�y�[�W���̃T�C�Y
#define FINFO_SHIORI_OFSX ( 1 )   // ONEX�̒��ŕ\������L�����N�^�܂ł̐�
#define FINFO_SHIORI_OFSY ( 1 )   // ONEX�̒��ŕ\������L�����N�^�܂ł̐�
#define FINFO_SHIORISCRN_X  ( 0 )   // �X�N���[���ǂݍ��݂�
#define FINFO_SHIORISCRN_Y  ( 12 )    // �X�N���[���ǂݍ��݂�
#define FINFO_CL_SHIORISCRN_X ( 0 )   // �X�N���[���ǂݍ��݂�
#define FINFO_CL_SHIORISCRN_Y ( 28 )    // �X�N���[���ǂݍ��݂�
#define FINFO_SHIORISCRN_POS_X  ( 0 ) // �|�W�V�����X�N���[���ǂݍ��݂�
#define FINFO_SHIORISCRN_POS_Y  ( 16 )  // �|�W�V�����X�N���[���ǂݍ��݂�
#define FINFO_EFF_SHIORISCRN_POS_X  ( 5 ) // �G�t�F�N�g�p�|�W�V�����X�N���[���ǂݍ��݂�
#define FINFO_EFF_SHIORISCRN_POS_Y  ( 1 ) // �G�t�F�N�g�p�|�W�V�����X�N���[���ǂݍ��݂�
#define FINFO_EFF_SHIORI_POS_SIZ  (4)     // �|�W�V�����X�N���[���̑傫��

// �X�N���[��
#define FINFO_SCROLL_COUNT  (8) // �X�N���[���J�E���g
#define FINFO_SCROLL_SIZX (256) // �X�N���[���T�C�Y


// �ő�l
#define FINFO_PAGE_X  ( 2 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE_Y  ( 1 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE_SIZX ( 26 )  // �r�b�g�}�b�v�G���A
#define FINFO_PAGE_SIZY ( 18 )  // �r�b�g�}�b�v�G���A

// �y�[�W�O�O
enum{
  FINFO_PAGE00_BA_BTT,
  FINFO_PAGE00_BA_TRT = 0,
  FINFO_PAGE00_BA_BT = 0,
  FINFO_PAGE00_BA_TR = 0,
  FINFO_PAGE00_BA_NUM = 1,
};
#define FINFO_PAGE00_BA_CAP_X   ( 2 ) // �r�b�g�}�b�v�G���A �^�C�g��
#define FINFO_PAGE00_BA_CAP_SIZX  ( 12 )  // �r�b�g�}�b�v�G���A �^�C�g��
#define FINFO_PAGE00_BA_CAP_SIZY  ( 2 ) // �r�b�g�}�b�v�G���A �^�C�g��
#define FINFO_PAGE00_BA_CAP_BTT_Y ( 5 ) // �r�b�g�}�b�v�G���A �ΐ�^�C�g��
#define FINFO_PAGE00_BA_CAP_TRT_Y ( 12 )  // �r�b�g�}�b�v�G���A �����^�C�g��

#define FINFO_PAGE00_BA_BT_X  ( 19 )  // �r�b�g�}�b�v�G���A �ΐ퐬��
#define FINFO_PAGE00_BA_BT_Y  ( FINFO_PAGE00_BA_CAP_BTT_Y ) // �r�b�g�}�b�v�G���A �ΐ퐬��
#define FINFO_PAGE00_BA_BT_SIZX ( 8 ) // �r�b�g�}�b�v�G���A �ΐ퐬��
#define FINFO_PAGE00_BA_BT_SIZY ( 5 ) // �r�b�g�}�b�v�G���A �ΐ퐬��

#define FINFO_PAGE00_BA_TR_X  ( 23 )  // �r�b�g�}�b�v�G���A ����
#define FINFO_PAGE00_BA_TR_Y  ( FINFO_PAGE00_BA_CAP_TRT_Y ) // �r�b�g�}�b�v�G���A ����
#define FINFO_PAGE00_BA_TR_SIZX ( 4 ) // �r�b�g�}�b�v�G���A ����
#define FINFO_PAGE00_BA_TR_SIZY ( 2 ) // �r�b�g�}�b�v�G���A ����

#define FINFO_PAGE00_BTT_X    ( 0 ) // �ΐ퐬�у^�C�g��
#define FINFO_PAGE00_BTT_Y    ( 0 ) // �ΐ퐬�у^�C�g��
#define FINFO_PAGE00_BT_X   ( 0 ) // �ΐ퐬�т���&�܂�
#define FINFO_PAGE00_BT_Y   ( 0 ) // �ΐ퐬�т���&�܂�
#define FINFO_PAGE00_BT_OFSY  ( 24 )  // �ΐ퐬��Y�I�t�Z�b�g

#define FINFO_PAGE00_TRT_X  ( 0 ) // �|�P���������^�C�g��
#define FINFO_PAGE00_TRT_Y  ( 0 ) // �|�P���������^�C�g��
#define FINFO_PAGE00_TRN_X  ( 0 ) // �|�P���������l
#define FINFO_PAGE00_TRN_Y  ( 0 ) // �|�P���������l

// �y�[�W�O�P
enum{
  FINFO_PAGE01_BA_CAP = 0,
  FINFO_PAGE01_BA_DAT = 0,
  FINFO_PAGE01_BA_NUM = 1,
};
#define FINFO_PAGE01_BA_CAP_X ( 5 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE01_BA_CAP_Y ( 5 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE01_BA_CAP_SIZX ( 9 )  // �r�b�g�}�b�v�G���A
#define FINFO_PAGE01_BA_CAP_SIZY ( 2 )  // �r�b�g�}�b�v�G���A
#define FINFO_PAGE01_BA_DAT_X ( FINFO_PAGE01_BA_CAP_X ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE01_BA_DAT_Y ( 9 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE01_BA_DAT_SIZX ( 19 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE01_BA_DAT_SIZY ( 5 )  // �r�b�g�}�b�v�G���A

#define FINFO_PAGE01_BTT_X    ( 0 ) // �o�g���^���[�^�C�g��
#define FINFO_PAGE01_BTT_Y    ( 0 ) // �o�g���^���[�^�C�g��
#define FINFO_PAGE01_DAT_X    ( 0 ) // �o�g���^���[�^�C�g��
#define FINFO_PAGE01_DATN_X   ( 7*8 ) // �o�g���^���[�^�C�g��
#define FINFO_PAGE01_DAT_Y01  ( 0 ) // �o�g���^���[�^�C�g��
#define FINFO_PAGE01_DAT_Y02  ( 24 )  // �o�g���^���[�^�C�g��

// �y�[�W02
enum{
  FINFO_PAGE02_BA_TT = 0,   // �^�C�g��
  FINFO_PAGE02_LV50TT = 0,  // LV50�^�C�g��
  FINFO_PAGE02_LV50NO = 0,  // LV50�l
  FINFO_PAGE02_LV100TT = 0, // LV100�^�C�g��
  FINFO_PAGE02_LV100NO = 0, // LV100�l
  FINFO_PAGE02_BA_NUM = 1,
};

#define FINFO_PAGE02_BA_TT_X  ( 2 ) // �r�b�g�}�b�v�G���A �^�C�g��
#define FINFO_PAGE02_BA_TT_Y  ( 1 ) // �r�b�g�}�b�v�G���A �^�C�g��
#define FINFO_PAGE02_BA_TT_SIZX ( 14 )  // �r�b�g�}�b�v�G���A �^�C�g��
#define FINFO_PAGE02_BA_TT_SIZY ( 2 ) // �r�b�g�}�b�v�G���A �^�C�g��
#define FINFO_PAGE02_BA_LV50TT_X  ( 2 ) // �r�b�g�}�b�v�G���A LV50�^�C�g��
#define FINFO_PAGE02_BA_LV50TT_Y  ( 4 ) // �r�b�g�}�b�v�G���A LV50�^�C�g��
#define FINFO_PAGE02_BA_LV50TT_SIZX ( 26 )  // �r�b�g�}�b�v�G���A LV50�^�C�g��
#define FINFO_PAGE02_BA_LV50TT_SIZY ( 2 ) // �r�b�g�}�b�v�G���A LV50�^�C�g��
#define FINFO_PAGE02_BA_LV50NO_X  ( 2 ) // �r�b�g�}�b�v�G���A LV50�l
#define FINFO_PAGE02_BA_LV50NO_Y  ( 7 ) // �r�b�g�}�b�v�G���A LV50�l
#define FINFO_PAGE02_BA_LV50NO_SIZX ( 26 )  // �r�b�g�}�b�v�G���A LV50�l
#define FINFO_PAGE02_BA_LV50NO_SIZY ( 4 ) // �r�b�g�}�b�v�G���A LV50�l
#define FINFO_PAGE02_BA_LV100TT_X ( 2 ) // �r�b�g�}�b�v�G���A LV100�^�C�g��
#define FINFO_PAGE02_BA_LV100TT_Y ( 12 )  // �r�b�g�}�b�v�G���A LV100�^�C�g��
#define FINFO_PAGE02_BA_LV100TT_SIZX ( 26 ) // �r�b�g�}�b�v�G���A LV100�^�C�g��
#define FINFO_PAGE02_BA_LV100TT_SIZY ( 2 )  // �r�b�g�}�b�v�G���A LV100�^�C�g��
#define FINFO_PAGE02_BA_LV100NO_X ( 2 ) // �r�b�g�}�b�v�G���A LV100�l
#define FINFO_PAGE02_BA_LV100NO_Y ( 15 )  // �r�b�g�}�b�v�G���A LV100�l
#define FINFO_PAGE02_BA_LV100NO_SIZX ( 26 ) // �r�b�g�}�b�v�G���A LV100�l
#define FINFO_PAGE02_BA_LV100NO_SIZY ( 4 )  // �r�b�g�}�b�v�G���A LV100�l
#define FINFO_PAGE02_TT_X   ( 0 ) // �^�C�g��
#define FINFO_PAGE02_TT_Y   ( 0 ) // �^�C�g��
#define FINFO_PAGE02_LV50TT_X ( 0 ) // LV50�^�C�g��
#define FINFO_PAGE02_LV50TT2_X  ( 120 ) // LV50�^�C�g��
#define FINFO_PAGE02_LV50TT_Y ( 0 ) // LV50�^�C�g��
#define FINFO_PAGE02_LV50LT_X ( 0 ) // LV50�O��
#define FINFO_PAGE02_LV50LN0_X  ( 56 )  // LV50�O��
#define FINFO_PAGE02_LV50LN1_X  ( 160 ) // LV50�O��
#define FINFO_PAGE02_LV50L_Y  ( 0 ) // LV50�O��
#define FINFO_PAGE02_LV50MT_X ( 0 ) // LV50�ō�
#define FINFO_PAGE02_LV50MN0_X  ( 56 )  // LV50�ō�
#define FINFO_PAGE02_LV50MN1_X  ( 160 ) // LV50�ō�
#define FINFO_PAGE02_LV50M_Y  ( 16 )  // LV50�ō�
#define FINFO_PAGE02_LV100TT_X  ( 0 ) // LV100�^�C�g��
#define FINFO_PAGE02_LV100TT2_X ( 120 ) // LV100�^�C�g��
#define FINFO_PAGE02_LV100TT_Y  ( 0 ) // LV100�^�C�g��
#define FINFO_PAGE02_LV100LT_X  ( 0 ) // LV100�O��
#define FINFO_PAGE02_LV100LN0_X ( 56 )  // LV100�O��
#define FINFO_PAGE02_LV100LN1_X ( 160 ) // LV100�O��
#define FINFO_PAGE02_LV100L_Y ( 0 ) // LV100�O��
#define FINFO_PAGE02_LV100MT_X  ( 0 ) // LV100�ō�
#define FINFO_PAGE02_LV100MN0_X ( 56 )  // LV100�ō�
#define FINFO_PAGE02_LV100MN1_X ( 160 ) // LV100�ō�
#define FINFO_PAGE02_LV100M_Y ( 16 )  // LV100�ō�


// �y�[�W03
enum{
  FINFO_PAGE03_BA,
  FINFO_PAGE03_BA_NUM,
};
#define FINFO_PAGE03_BA_X ( 2 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE03_BA_Y ( 5 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE03_BA_SIZX ( 26 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE03_BA_SIZY ( 11 ) // �r�b�g�}�b�v�G���A

#define FINFO_PAGE03_TT_X ( 0 ) // �^�C�g��
#define FINFO_PAGE03_TT_Y ( 0 ) // �^�C�g��
#define FINFO_PAGE03_CPT_X  ( 104 ) // �L���b�X���|�C���g�^�C�g��
#define FINFO_PAGE03_CPT_Y  ( 20 )  // �L���b�X���|�C���g�^�C�g��
#define FINFO_PAGE03_CPLT_X ( 0 ) // �L���b�X���|�C���g�O��
#define FINFO_PAGE03_CPLN0_X  ( 56 )  // �L���b�X���|�C���g�O��
#define FINFO_PAGE03_CPLN1_X  ( 168 ) // �L���b�X���|�C���g�O��
#define FINFO_PAGE03_CPL_Y  ( 48 )  // �L���b�X���|�C���g�O��
#define FINFO_PAGE03_CPMT_X ( 0 ) // �L���b�X���|�C���g�ő�
#define FINFO_PAGE03_CPMN0_X  ( 56 )  // �L���b�X���|�C���g�ő�
#define FINFO_PAGE03_CPMN1_X  ( 168 ) // �L���b�X���|�C���g�ő�
#define FINFO_PAGE03_CPM_Y  ( 72 )  // �L���b�X���|�C���g�ő�


// �y�[�W04
enum{
  FINFO_PAGE04_BA,
  FINFO_PAGE04_BA_NUM,
};
#define FINFO_PAGE04_BA_X ( 2 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE04_BA_Y ( 5 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE04_BA_SIZX ( 20 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE04_BA_SIZY ( 11 ) // �r�b�g�}�b�v�G���A

#define FINFO_PAGE04_TT_X ( 0 ) // �^�C�g��
#define FINFO_PAGE04_TT_Y ( 0 ) // �^�C�g��
#define FINFO_PAGE04_TT2_Y  ( 24 )  // �^�C�g��
#define FINFO_PAGE04_LT_X ( 0 ) // �O��
#define FINFO_PAGE04_LN_X ( 56 )  // �O��
#define FINFO_PAGE04_L_Y  ( 48 )  // �O��
#define FINFO_PAGE04_MT_X ( 0 ) // �ő�
#define FINFO_PAGE04_MN_X ( 56 )  // �ő�
#define FINFO_PAGE04_M_Y  ( 72 )  // �ő�


// �y�[�W05
enum{
  FINFO_PAGE05_BA,
  FINFO_PAGE05_BA_NUM,
};
#define FINFO_PAGE05_BA_X ( 2 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE05_BA_Y ( 5 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE05_BA_SIZX ( 20 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE05_BA_SIZY ( 11 ) // �r�b�g�}�b�v�G���A

#define FINFO_PAGE05_TT_X ( 0 ) // �^�C�g��
#define FINFO_PAGE05_TT_Y ( 0 ) // �^�C�g��
#define FINFO_PAGE05_LT_X ( 0 ) // �O��
#define FINFO_PAGE05_LN_X ( 56 )  // �O��
#define FINFO_PAGE05_L_Y  ( 24 )  // �O��
#define FINFO_PAGE05_MT_X ( 0 ) // �ő�
#define FINFO_PAGE05_MN_X ( 56 )  // �ő�
#define FINFO_PAGE05_M_Y  ( 48 )  // �ő�

// �y�[�W06
enum{
  FINFO_PAGE06_BA,
  FINFO_PAGE06_BA_NUM,
};
#define FINFO_PAGE06_BA_X ( 2 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE06_BA_Y ( 5 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE06_BA_SIZX ( 22 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE06_BA_SIZY ( 11 ) // �r�b�g�}�b�v�G���A

#define FINFO_PAGE06_TT_X ( 0 ) // �^�C�g��
#define FINFO_PAGE06_TT_Y ( 0 ) // �^�C�g��
#define FINFO_PAGE06_BC_X ( 0 ) // ���܂Ȃ�
#define FINFO_PAGE06_BC_N_X ( 120 ) // �l
#define FINFO_PAGE06_BC_Y ( 24 )  // Y
#define FINFO_PAGE06_BB_X ( 0 ) // ���܂̂�
#define FINFO_PAGE06_BB_N_X ( 120 ) // �l
#define FINFO_PAGE06_BB_Y ( 48 )  // Y
#define FINFO_PAGE06_BL_X ( 0 ) // ���܂̂�
#define FINFO_PAGE06_BL_N_X ( 120 ) // �l
#define FINFO_PAGE06_BL_Y ( 72 )  // Y

// �y�[�W07
enum{
  FINFO_PAGE07_BA,
  FINFO_PAGE07_BA_NUM,
};
#define FINFO_PAGE07_BA_X ( 1 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE07_BA_Y ( 5 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE07_BA_SIZX ( 27 ) // �r�b�g�}�b�v�G���A
#define FINFO_PAGE07_BA_SIZY ( 2 )  // �r�b�g�}�b�v�G���A

// �y�[�W�ύX�E�G�C�g
#define FINFO_PAGECHG_WAIT  ( 4 )
// �{�^���A�j���E�F�C�g
#define WFNOTE_BTNANM_WAIT  (4)


#define TP_FINFO_BTN_H  (4*8)
#define TP_FINFO_BTN_Y  (20*8)

#define TP_FINFO_BTN01_W  (4*8)
#define TP_FINFO_BTN01_OX (4*8)
#define TP_FINFO_BTN01_X1 (0*0)
#define TP_FINFO_BTN01_X2 (TP_FINFO_BTN01_X1+TP_FINFO_BTN01_OX)
#define TP_FINFO_BTN01_X3 (TP_FINFO_BTN01_X2+TP_FINFO_BTN01_OX)
#define TP_FINFO_BTN01_X4 (TP_FINFO_BTN01_X3+TP_FINFO_BTN01_OX)
#define TP_FINFO_BTN01_X5 (TP_FINFO_BTN01_X4+TP_FINFO_BTN01_OX)
#define TP_FINFO_BTN01_X6 (TP_FINFO_BTN01_X5+TP_FINFO_BTN01_OX)
#define TP_FINFO_BTN01_X7 (TP_FINFO_BTN01_X6+TP_FINFO_BTN01_OX)
#define TP_FINFO_BTN01_X8 (TP_FINFO_BTN01_X7+TP_FINFO_BTN01_OX)

#define TP_FINFO_BTN02_W  (14*8)
#define TP_FINFO_BTN02_OX (TP_FINFO_BTN02_W)
#define TP_FINFO_BTN02_X1 (0*0)
#define TP_FINFO_BTN02_X2 (TP_FINFO_BTN02_X1+TP_FINFO_BTN02_OX)

#define TP_FINFO_YAZI_W   (32)
#define TP_FINFO_YAZI_H   (32)
#define TP_FINFO_YAZI_X   (29*8+4)
#define TP_FINFO_YAZI_Y1  (WFNOTE_YAZIRUSHI_TY-16)
#define TP_FINFO_YAZI_Y2  (WFNOTE_YAZIRUSHI_BY-16)

#define NOTE_PRINTUTIL_MAX  ( 64 )

//////////////////////////////////////////////////////////////////////////////
//�^�b�`�p�l�����W
//////////////////////////////////////////////////////////////////////////////
#define TP_MODESEL_BTN_X    (2*8+3)
#define TP_MODESEL_BTN_W    (28*8-6-1)
#define TP_MODESEL_BTN_H    (4*8-4-1)
#define TP_MODESEL_BTN_OY   (5*8)
#define TP_MODESEL_BTN_Y01  (5*8+2)
#define TP_MODESEL_BTN_Y02  (TP_MODESEL_BTN_Y01+TP_MODESEL_BTN_OY)
#define TP_MODESEL_BTN_Y03  (TP_MODESEL_BTN_Y02+TP_MODESEL_BTN_OY)
#define TP_MODESEL_BTN_Y04  (TP_MODESEL_BTN_Y03+TP_MODESEL_BTN_OY)

#define TP_MYCODE_BTN_X   (17*8)
#define TP_MYCODE_BTN_Y   (20*8)
#define TP_MYCODE_BTN_W   (14*8)
#define TP_MYCODE_BTN_H   (4*8)

//OBJ�p���\�[�XIDX
enum
{
/*
  WOR_OBJ_PLT,
  WOR_OBJ_NCG,
  WOR_OBJ_ANM,
  WNOTE_OBJ_RES_MAX,
*/
  // �L����
  CHR_RES_WOR = 0,
  CHR_RES_TB,
  CHR_RES_MAX,
  // �p���b�g
  PAL_RES_WOR = 0,
  PAL_RES_TB,
  PAL_RES_MAX,
  // �Z��
  CEL_RES_WOR = 0,
  CEL_RES_TB,
  CEL_RES_MAX,

}WNOTE_OBJ_RES;

#pragma mark[>struct
//-----------------------------------------------------------------------------
/**
 *          �\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
/// ���j���[�f�[�^�\����
//=====================================
typedef struct {
  u32 strid;
  u32 param;
} WFNOTE_MENU_DATA;


//-------------------------------------
//  �X�N���[���G���A�f�[�^
//=====================================
typedef struct {
  s16 scrn_x;   // �`��G���A�J�n�X�N���[����
  s16 scrn_y;   // �`��G���A�J�n�X�N���[��y
  u8 scrn_sizx; // �`��G���A�T�C�Y�X�N���[����
  u8 scrn_sizy; // �`��G���A�T�C�Y�X�N���[����
} WFNOTE_SCRNAREA;


//-------------------------------------
//  �X�N���[���f�[�^
//=====================================
typedef struct {
  void* p_scrnbuff[WFNOTE_SCRNDATA_NUM];  // �X�N���[���f�[�^
  NNSG2dScreenData* p_scrn[WFNOTE_SCRNDATA_NUM];  // �X�N���[���f�[�^
} WFNOTE_SCRNDATA;


//-------------------------------------
/// �R�[�h���͌��ʃf�[�^
//=====================================
typedef struct {
  STRBUF* p_name;
  STRBUF* p_code;
} WFNOTE_CODEIN_DATA;

//-------------------------------------
/// ���O���͏��������
//=====================================
typedef struct {
  const STRCODE* cp_str;
  BOOL init;
} WFNOTE_NAMEIN_INITDATA;


//-------------------------------------
/// �F�B�C���f�b�N�X�f�[�^
//=====================================
typedef struct {
  u32 friendnum;  // �F�B�̑���
  u32 infonum;  // �ڍ׏��̑���
  u8 fridx[FLIST_FRIEND_MAX]; // �F�B�C���f�b�N�X
  u8 infoidx[FLIST_FRIEND_MAX]; // �ڍ׏��C���f�b�N�X
} WFNOTE_IDXDATA;



//-------------------------------------
/// �O���[�o���f�[�^���[�N
//=====================================
typedef struct {
  int key_mode;
  //SAVE_CONTROL_WORK* p_save;  // �Z�[�u�f�[�^
  GAMEDATA* pGameData;
  u8 subseq;      // �T�u�V�[�P���X
  u8 reqstatus;   // ���ɐi�݂����X�e�[�^�X
  u8 reqsubseq;   // ���̃X�e�[�^�X�̊J�n�T�u�V�[�P���X
  s8 select_listidx;  // �I���������X�g�C���f�b�N�X

  u8  msg_spd;
  u8  win_type;
  u8  lr_key_enable;

  WFNOTE_IDXDATA idx; // �F�B�f�[�^�C���f�b�N�X

  WFNOTE_CODEIN_DATA codein;  // �R�[�h���͉�ʂœ��͂����f�[�^
  WFNOTE_NAMEIN_INITDATA namein_init; // ���O���͉�ʏ������f�[�^
} WFNOTE_DATA;

//-------------------------------------
/// �\�����[�N
//=====================================
typedef struct {
//  int key_mode;

//  GF_BGL_INI* p_bgl;    // bgl

//  GFL_CLUNIT *p_clact;  // �A�N�^�[
//  NNSG2dRenderSurface scrll_surface;  // �X�N���[���p�T�[�t�F�[�X

  WORDSET* p_wordset;     // ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  GFL_MSGDATA* p_msgman;  // ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[

  ARCHANDLE* p_handle;    // �O���t�B�b�N�A�[�J�C�u�n���h��

/*
#if USE_SEL_BOX
  SELBOX_SYS      *sbox;  // �I���{�b�N�X�V�X�e�����[�N
#endif
  TOUCH_SW_SYS    *ynbtn_wk;  ///<YesNo�{�^�����[�N
*/

  PRINT_UTIL  title;    // �^�C�g���p�r�b�g�}�b�v
  STRBUF* p_titlestr;     // �^�C�g��������
  STRBUF* p_titletmp;     // �^�C�g��������
  STRBUF* p_str;        // ������ėp�o�b�t�@
  STRBUF* p_tmp;        // ������ėp�o�b�t�@
  u32 title_stridx;     // �^�C�g���`��C���f�b�N�X

  GFL_TCBLSYS   *msgTcblSys;
  PRINT_STREAM  *printHandle;   //�^�C�g���p
  PRINT_STREAM  *printHandleMsg;  //���b�Z�[�W�p

  PRINT_QUE   *printQue;
  PRINT_UTIL * printUtil[NOTE_PRINTUTIL_MAX];

  //Obj�n
  GFL_CLUNIT  *cellUnit;
  GFL_CLUNIT  *cellUnitScroll;
  GFL_CLSYS_REND  *scrollRender;
//  u32     objResIdx[WNOTE_OBJ_RES_MAX];
  u32 chrRes[CHR_RES_MAX];
  u32 palRes[PAL_RES_MAX];
  u32 celRes[CEL_RES_MAX];

  GFL_CLWK *p_yazirushi[WFNOTE_YAZIRUSHINUM]; // ���A�N�^�[
//  GFL_CLWK *p_cur;  // �J�[�\���A�N�^�[
  GFL_CLWK *p_tb;   // �^�b�`�o�[�߂�{�^���A�N�^�[
  GFL_CLWK *p_tbx;  // �^�b�`�o�[�~�{�^���A�N�^�[

  WFNOTE_SCRNDATA scrn; // ��{�X�N���[���f�[�^

  GFL_FONT  *fontHandle;
  HEAPID    heapID;

  BLINKPALANM_WORK * bpawk;

} WFNOTE_DRAW;

//-------------------------------------
/// ���[�h�I����ʃ��[�N
//=====================================
typedef struct {
  // �f�[�^
  u32 cursor; // �J�[�\��
  u32 wait;

  // �{�^���A�j��
  u16 btn_anmSeq;   // �V�[�P���X
  u16 btn_anmCnt;   // �J�E���^

  // �O���t�B�b�N
  PRINT_UTIL  msg;  // ���b�Z�[�W��
  void* p_scrnbuff; // �X�N���[���f�[�^
  NNSG2dScreenData* p_scrn; // �X�N���[���f�[�^
  PRINT_UTIL  talk; // ��b�E�B���h�E
  BMPMENU_WORK* p_yesno;// YESNO�E�B���h�E
  u32 talkmsg_idx;  // ��b���b�Z�[�Widx
  STRBUF* p_str;

  // �͂��E������
  APP_TASKMENU_ITEMWORK ynlist[2];
  APP_TASKMENU_RES * listRes;
  APP_TASKMENU_WORK * listWork;

} WFNOTE_MODESELECT;

//-------------------------------------
/// �F�B�f�[�^�\����ʃ��[�N
//  �`��G���A�f�[�^
//=====================================
typedef struct {
  WFNOTE_SCRNAREA scrn_area;  // �`��X�N���[���G���A
  PRINT_UTIL * text;
//  GFL_BMPWIN **text;  // ���b�Z�[�W��
  WF_2DCWK* p_clwk[FLIST_PAGE_FRIEND_NUM];

  GFL_CLWK * p_hate[FLIST_PAGE_FRIEND_NUM];

} WFNOTE_FLIST_DRAWAREA;

//-------------------------------------
/// �F�B�f�[�^�J�[�\���f�[�^
//=====================================
typedef struct {
  WFNOTE_SCRNAREA scrn_data;
  u8  bg_frame;
  u8  off_pal,on_pal;
} WFNOTE_FLIST_CURSORDATA;


//-------------------------------------
/// �F�B�f�[�^�\����ʃ��[�N
//=====================================
typedef struct {
  s16 page; // �y�[�W
  s16 lastpage;
  s16 pos;  // �y�[�W���̈ʒu
  s16 lastpos;// 1�O�̈ʒu

  // �{�^���A�j��
  u16 btn_anmSeq;   // �V�[�P���X
  u16 btn_anmCnt;   // �J�E���^

  WFNOTE_FLIST_DRAWAREA drawdata[WFNOTE_DRAWAREA_NUM];
//  GFL_BMPWIN  *drawBmpWin[2];
  PRINT_UTIL  drawBmpWin[2];
  s16 count;  // �X�N���[���J�E���^
  u16 way;  // �X�N���[������
  WF_2DCSYS* p_charsys; // �L�����N�^�\���V�X�e��

//  GFL_BMPWIN *backmsg;  // �߂郁�b�Z�[�W�p

  PRINT_UTIL  talk; // �g�[�N���b�Z�[�W�p
  STRBUF* p_talkstr;
  u32 msgidx;
//  GF_BGL_BMPWIN menu; // ���j���[���X�g�p
//  BMPLIST_DATA* p_menulist[BMPL_FLIST_MENU_NUM];  // ���j���[���X�g
//    BMPLIST_WORK* p_listwk; // BMP���j���[���[�N
//#if USE_SEL_BOX
//  SELBOX_WORK*  p_listwk; ///<�I���{�b�N�X���[�N
//#endif
  APP_TASKMENU_ITEMWORK list1[4];
  APP_TASKMENU_ITEMWORK list2[4];
  APP_TASKMENU_ITEMWORK ynlist[2];
  APP_TASKMENU_RES * listRes;
  APP_TASKMENU_WORK * listWork;
  u32 listType;

//  BMPMENU_WORK* p_yesno;  // YESNO�E�B���h�E

  s32 wait;

  GFL_CLWK *p_clearact;   // �����S���A�N�^�[
  GFL_CLWK * p_pageact;   // �y�[�W�ԍ��A�N�^�[
  u32 last_frame;

} WFNOTE_FRIENDLIST;

//-------------------------------------
/// �F�B�R�[�h���͉�ʃ��[�N
//=====================================
typedef struct {
  GFL_PROCSYS *p_subproc; // �T�u�v���Z�X
  NAMEIN_PARAM* p_namein; // ���O���̓p�����[�^
  CODEIN_PARAM* p_codein; // �R�[�h���̓p�����[�^
} WFNOTE_CODEIN;

//-------------------------------------
/// �����̗F�B�R�[�h�m�F��ʃ��[�N
//=====================================
typedef struct {
  // �O���t�B�b�N�f�[�^
  PRINT_UTIL  msg;  // ���b�Z�[�W��
  PRINT_UTIL  code; // �R�[�h��
//  GFL_BMPWIN *back; // ���ǂ��
  void* p_scrnbuff; // �X�N���[���f�[�^
  NNSG2dScreenData* p_scrn; // �X�N���[���f�[�^

  int ct; //�J�E���^
} WFNOTE_MYCODE;


//-------------------------------------
/// �F�B�ڍ׏��`����
/// ��ʍ\���X�N���[���f�[�^
//=====================================
typedef struct {
  void* p_scrnbuff; // �X�N���[���f�[�^
  NNSG2dScreenData* p_scrn; // �X�N���[���f�[�^
} WFNOTE_FINFO_SCRNDATA;

//-------------------------------------
/// �F�B�ڍ׏��`����
//  �\���G���A���
//=====================================
typedef struct {
  WFNOTE_SCRNAREA scrnarea;
  u32 cgx;
//  GFL_BMPWIN**  p_msg[ FINFO_PAGE_NUM ];  //�\����
  PRINT_UTIL * p_msg[ FINFO_PAGE_NUM ];  //�\����
  u32 msgnum[ FINFO_PAGE_NUM ];   // ����msg�r�b�g�}�b�v��
} WFNOTE_FINFO_DRAWAREA;

//-------------------------------------
/// �F�B�ڍ׏��\����ʃ��[�N
//=====================================
typedef struct {
  s16 page; // ���݂̃y�[�W
  s16 lastpage; // �P�O�̃y�[�W
  s32 count;  // �X�N���[���J�E���g
  WF_COMMON_WAY way;  // ����
  u16 wait; // �E�G�C�g
  u16 rep;  // �㉺���s�[�g����
  BOOL frontier_draw; // �t�����e�B�A�`��L��


  WFNOTE_FINFO_DRAWAREA drawarea[WFNOTE_DRAWAREA_NUM];// �\���G���A
  WFNOTE_FINFO_SCRNDATA scrn[FINFO_SCRNDATA_NUM]; // ��ʍ\���X�N���[���f�[�^
  PRINT_UTIL  drawBmpWin[2];
  PRINT_UTIL  win[ FINFO_WIN_MAX ];  //�Œ�E�B���h�E

  GFL_CLWK * p_trgra;
  u32 trRes[3];

} WFNOTE_FRIENDINFO;


//-------------------------------------
/// �r�b�g�}�b�v�o�^�f�[�^
//=====================================
typedef struct {
  u8 pos_x;
  u8 pos_y;
  u8 siz_x;
  u8 siz_y;
} WFNOTE_FINFO_BMPDATA;



//-------------------------------------
/// �F�B�蒠���[�N
//=====================================
typedef struct {
  u32 status;     // ���̏��
  WFNOTE_DRAW draw; // �\���V�X�e�����[�N
  WFNOTE_DATA data; // �f�[�^���[�N
  WIFINOTE_PROC_PARAM*  pp;

  // status���[�N
  WFNOTE_MODESELECT mode_select;  // ���[�h�Z���N�g
  WFNOTE_FRIENDLIST friendlist;   // �F�B�f�[�^�\��
  WFNOTE_CODEIN   codein;     // �F�B�R�[�h����
  WFNOTE_MYCODE   mycode;     // �����̗F�B�R�[�h�m�F
  WFNOTE_FRIENDINFO friendinfo;   // �F�B�ڍו\�����

  GFL_TCB       *vBlankTcb;

  s32 wait;
} WFNOTE_WK;

#pragma mark[>prototype
//-----------------------------------------------------------------------------
/**
 *          �v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
/// �S�̊֐�
//=====================================
GFL_PROC_RESULT WifiNoteProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
GFL_PROC_RESULT WifiNoteProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
GFL_PROC_RESULT WifiNoteProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

static void WFNOTE_VBlank( GFL_TCB *tcp, void *work );
static void WFNOTE_DrawInit( WFNOTE_WK* p_wk, HEAPID heapID );
static void WFNOTE_DrawExit( WFNOTE_WK* p_wk );


//-------------------------------------
/// �f�[�^���[�N�֐�
//=====================================
static void Data_Init( WFNOTE_DATA* p_data, WIFINOTE_PROC_PARAM* wp, HEAPID heapID );
static void Data_Exit( WFNOTE_DATA* p_data, WIFINOTE_PROC_PARAM* wp );
static void Data_SetReqStatus( WFNOTE_DATA* p_data, u8 reqstatus, u8 reqsubseq );
//static void Data_CheckKeyModeChange( WFNOTE_DATA* p_data );
static void Data_CODEIN_Set( WFNOTE_DATA* p_data, const STRBUF* cp_name, const STRBUF* cp_code );
static u32 Data_NewFriendDataSet( WFNOTE_DATA* p_data, STRBUF* p_code, STRBUF* p_name );
static void Data_NAMEIN_INIT_Set( WFNOTE_DATA* p_data, const STRCODE* cp_str );
static void Data_SELECT_ListIdxSet( WFNOTE_DATA* p_data, u32 idx );
static void Data_FrIdxMake( WFNOTE_DATA* p_data );
static u32 Data_FrIdxGetIdx( const WFNOTE_DATA* cp_data, u32 idx );

//-------------------------------------
/// �`�惏�[�N�֐�
//=====================================
static void Draw_Init( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, HEAPID heapID );
static void Draw_Main( WFNOTE_DRAW* p_draw );
static void Draw_VBlank( WFNOTE_DRAW* p_draw );
static void Draw_Exit( WFNOTE_DRAW* p_draw );
static void Draw_BankInit( void );
static void Draw_BgInit( WFNOTE_DRAW* p_draw, HEAPID heapID );
static void Draw_BgExit( WFNOTE_DRAW* p_draw );
static void Draw_ClactInit( WFNOTE_DRAW* p_draw, HEAPID heapID );
static void Draw_ClactExit( WFNOTE_DRAW* p_draw );
static void Draw_MsgInit( WFNOTE_DRAW* p_draw, HEAPID heapID );
static void Draw_MsgExit( WFNOTE_DRAW* p_draw );
static void Draw_BmpInit( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, HEAPID heapID );
static void Draw_BmpExit( WFNOTE_DRAW* p_draw );
static void Draw_SCRNDATA_Init( WFNOTE_DRAW* p_draw, HEAPID heapID );
static void Draw_SCRNDATA_Exit( WFNOTE_DRAW* p_draw );
static void Draw_BmpTitleWrite( WFNOTE_DRAW* p_draw, PRINT_UTIL * wk, u32 msg_idx );
static void Draw_BmpTitleOff( WFNOTE_DRAW* p_draw );
static void Draw_FriendCodeSetWordset( WFNOTE_DRAW* p_draw, u64 code );
static void Draw_FriendNameSetWordset( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, u32 idx, HEAPID heapID );
static void Draw_FriendGroupSetWordset( WFNOTE_DRAW* p_draw,const WFNOTE_DATA* cp_data, u32 idx, HEAPID heapID );
static BOOL Draw_FriendDaySetWordset( WFNOTE_DRAW* p_draw,const WFNOTE_DATA* cp_data, u32 idx );
static void Draw_NumberSetWordset( WFNOTE_DRAW* p_draw, u32 num );
static void Draw_WflbyGameSetWordSet( WFNOTE_DRAW* p_draw, u32 num );
static void Draw_CommonActInit( WFNOTE_DRAW* p_draw, HEAPID heapID );
static void Draw_CommonActExit( WFNOTE_DRAW* p_draw );
static void Draw_YazirushiSetDrawFlag( WFNOTE_DRAW* p_draw, BOOL flag );
static void Draw_YazirushiSetAnmFlag( WFNOTE_DRAW* p_draw, BOOL flag );
static void Draw_YazirushiAnmChg( WFNOTE_DRAW* p_draw, u8 idx, u8 anm_no);
//static void Draw_CursorActorSet( WFNOTE_DRAW* p_draw, BOOL draw_f, u8 anm_no );
//static void Draw_CursorActorPriSet( WFNOTE_DRAW* p_draw, u8 bg_pri, u32 s_pri );
static APP_TASKMENU_WORK * Draw_YesNoWinSet( APP_TASKMENU_ITEMWORK * list, APP_TASKMENU_RES ** res, BOOL putType, HEAPID heapID );
static int Draw_YesNoWinMain( APP_TASKMENU_WORK * wk );

//-------------------------------------
/// �I�����[�h�֐�
//=====================================
static void ModeSelect_Init( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static WFNOTE_STRET ModeSelect_Main( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void ModeSelect_Exit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void ModeSelect_DrawInit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void ModeSelect_DrawExit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void ModeSelectSeq_Init( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static BOOL ModeSelectSeq_Main( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void ModeSelect_CursorDraw( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw );
static void ModeSelect_CursorScrnPalChange( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw );
static void ModeSelect_EndDataSet( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data );
static void ModeSelect_DrawOff( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw );
static void ModeSelect_TalkMsgPrint( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw, u32 msgidx, HEAPID heapID );
static BOOL ModeSelect_TalkMsgEndCheck( const WFNOTE_MODESELECT* cp_wk, WFNOTE_DATA* p_data , WFNOTE_DRAW *p_draw );
static void ModeSelect_TalkMsgOff( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw );
static BOOL ModeSelect_StatusChengeCheck( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data );

//-------------------------------------
/// �F�B�f�[�^�\���֐�
//=====================================
static void FList_Init( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static WFNOTE_STRET FList_Main( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FList_Exit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void FList_DrawInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FList_DrawExit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void FList_DrawOn( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FList_DrawOff( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static u32 FListSeq_Main( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void FListSeq_ScrollInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static BOOL FListSeq_ScrollMain( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FListSeq_MenuInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static u32 FListSeq_MenuWait( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static BOOL FListSeq_CodeInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FListSeq_DeleteInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static BOOL FListSeq_DeleteYesNoDraw( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static u32 FListSeq_DeleteWait( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FList_DrawPage( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 page, u32 pos, u32 draw_area, HEAPID heapID );
static void FList_DrawShiori( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page );
static void FList_DrawShioriEff00( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 );
static void FList_DrawShioriEff01( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 );
static void FList_ScrSeqChange( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WF_COMMON_WAY way );
static void FList_TalkMsgWrite( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 msgid, HEAPID heapID );
static BOOL FList_TalkMsgEndCheck( const WFNOTE_FRIENDLIST* cp_wk, WFNOTE_DATA* p_data ,WFNOTE_DRAW* p_draw );
static void FList_TalkMsgOff( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW * p_draw );
static void FList_TalkMsgClear( WFNOTE_FRIENDLIST* p_wk );
static u32 FList_FRIdxGet( const WFNOTE_FRIENDLIST* cp_wk );
static void FList_DeleteAnmStart( WFNOTE_FRIENDLIST* p_wk ,WFNOTE_DRAW* p_draw );
static BOOL FList_DeleteAnmMain( WFNOTE_FRIENDLIST* p_wk ,WFNOTE_DRAW* p_draw );
static void FList_DeleteAnmOff( WFNOTE_FRIENDLIST* p_wk ,WFNOTE_DRAW* p_draw);
static void FListDrawArea_Init( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_area, u32 text_cgx, HEAPID heapID );
static void FListDrawArea_Exit( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw );
static void FListDrawArea_BackWrite( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn );
//static void FListDrawArea_CursorWrite( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 pos );
//static void FListDrawArea_ActiveListWrite( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 pos ,BOOL active_f);
static void FListDrawArea_WritePlayer( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, WF_2DCSYS* p_charsys, const WFNOTE_SCRNDATA* cp_scrn, const WFNOTE_IDXDATA* cp_idx, u32 page, HEAPID heapID );
static void FListDrawArea_DrawOff( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn );
static void FListDrawArea_CharWkDel( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw );
static void FListDrawArea_SetPlayer( WFNOTE_FLIST_DRAWAREA* p_wk, WF_2DCSYS* p_charsys, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn, u32 pos, WIFI_LIST* p_list, u32 idx, HEAPID heapID );
static void FListDrawArea_SetNoPlayer( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn, u32 pos, HEAPID heapID );


//-------------------------------------
/// �F�B�R�[�h���͉�ʃ��[�N
//=====================================
static void CodeIn_Init( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static WFNOTE_STRET CodeIn_Main( WFNOTE_CODEIN* p_wk, WFNOTE_WK* p_sys, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void CodeIn_Exit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void CodeIn_DrawInit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void CodeIn_DrawExit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static NAMEIN_PARAM* CodeIn_NameInParamMake( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, HEAPID heapID );

//-------------------------------------
/// �����̗F�B�R�[�h�m�F��ʃ��[�N
//=====================================
static void MyCode_Init( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static WFNOTE_STRET MyCode_Main( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void MyCode_Exit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static int MyCode_Input( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void MyCode_DrawInit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void MyCode_DrawExit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void MyCodeSeq_Init( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void MyCode_DrawOff( WFNOTE_MYCODE* p_wk, WFNOTE_DRAW* p_draw );
static void MyCode_BackButtonDraw( WFNOTE_MYCODE* p_wk, WFNOTE_DRAW* p_draw ,BOOL on_f);

//-------------------------------------
/// �F�B�ڍ׏��
//=====================================
static void FInfo_Init( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static WFNOTE_STRET FInfo_Main( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FInfo_Exit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static u32 FInfoInput_Exe( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw ,u32 code );
static u32 FInfoInput_Key( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static u32 FInfoInput_Touch( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, BOOL* touch_f );
static u32 FInfo_Input( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void FInfo_ScrollInit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static BOOL FInfo_ScrollMain( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FInfo_DrawInit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FInfo_DrawExit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void FInfo_DrawOn( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FInfo_DrawOff( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw );
static void FInfo_DrawOffFriendChange( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw );
static void FInfo_DrawPage( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 drawarea, u32 page, HEAPID heapID );
static void FInfo_DrawBaseInfo( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FInfo_DrawOffBaseInfo( WFNOTE_FRIENDINFO* p_wk );
static void FInfo_CursorDraw( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page );
static void FInfo_CursorOff( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw );
static void FInfo_ShioriDraw( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page );
static void FInfo_ShioriOff( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw );
static void FInfo_ShioriAnm00( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page00, u32 page01 );
static void FInfo_ShioriAnm01( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page00, u32 page01 );
static BOOL FInfo_SelectListIdxAdd( WFNOTE_DATA* p_data, WF_COMMON_WAY way );
static BOOL FInfo_SelectListIdxCheck( WFNOTE_DATA* p_data, WF_COMMON_WAY way );
static void FInfoScrnData_Init( WFNOTE_FINFO_SCRNDATA* p_scrn, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FInfoScrnData_Exit( WFNOTE_FINFO_SCRNDATA* p_scrn );
static void FInfoBmpWin_Init( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw );
static void FInfoBmpWin_Exit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw );
static void FInfoDrawArea_Init( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_scrnarea, u32 cgx, HEAPID heapID );
static void FInfoDrawArea_Exit( WFNOTE_FINFO_DRAWAREA* p_wk );
static void FInfoDrawArea_MsgBmpInit( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 idx, u32 num, const WFNOTE_FINFO_BMPDATA* cp_data, HEAPID heapID );
static void FInfoDrawArea_MsgBmpExit( WFNOTE_FINFO_DRAWAREA* p_wk, u32 idx );
static void FInfoDrawArea_MsgBmpOnVReq( WFNOTE_FINFO_DRAWAREA* p_wk, u32 idx );
static void FInfoDrawArea_MsgBmpOffVReq( WFNOTE_FINFO_DRAWAREA* p_wk, u32 idx );
//static void FInfoDrawArea_Page( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 page, const WFNOTE_FINFO_SCRNDATA* cp_scrn, BOOL bf_on,HEAPID heapID );
static void FInfoDrawArea_PageOff( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw );
//static void FInfoDraw_Page00( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
//static void FInfoDraw_Page01( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
//static void FInfoDraw_Page02( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
//static void FInfoDraw_Page03( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
//static void FInfoDraw_Page04( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
//static void FInfoDraw_Page05( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
//static void FInfoDraw_Page06( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
//static void FInfoDraw_Page07( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID );
static void FInfoDraw_Clean( WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_scrnarea );
static void FInfoDraw_Bmp( WFNOTE_FINFO_DRAWAREA* p_wk, u32 page, u32 bmp, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 msg_idx, u32 x, u32 y, u32 col, STRBUF* p_str, STRBUF* p_tmp );
static void FInfoDraw_BaseBmp( PRINT_UTIL * util, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw,u32 msg_idx, u32 x, u32 y, u32 col );
static void FInfo_PageChange( WFNOTE_FRIENDINFO* p_wk, s32 add );

#if NOTE_TEMP_COMMENT //���g�p�BDP�EPt�p�H
static BOOL FInfo_PofinnCaseCheck( WFNOTE_DATA* p_data, HEAPID heapID );
#endif

//-------------------------------------
/// �ڐA�p
//=====================================
static u64 WFNOTE_STRBUF_GetNumber( STRBUF *str , BOOL *pbFlag );

static BOOL CheckUITrg( WFNOTE_DATA * p_data );
static void Draw_BlinkAnmInit( WFNOTE_DRAW * p_draw, HEAPID heapID );
static void Draw_BlinkAnmExit( WFNOTE_DRAW * p_draw );

static void SetTopPageButtonPassive( WFNOTE_DRAW * p_draw, u32 pos );
static void SetTopPageButtonActive( WFNOTE_DRAW * p_draw, u32 pos );
static void ChangeTopPageButton( WFNOTE_DRAW * p_draw, u32 now, u32 old );
static void ResetTopPageButton( WFNOTE_MODESELECT * p_wk );
static BOOL MainTopPageButtonDecedeAnime( WFNOTE_MODESELECT * p_wk );

static void SetListPageButton( u32 pos, u32 pal );
static void SetListPageButtonPassive( WFNOTE_DRAW * p_draw, u32 pos );
static void SetListPageButtonActive( WFNOTE_DRAW * p_draw, u32 pos );
static void ChangeListPageButton( WFNOTE_DRAW * p_draw, u32 now, u32 old );
static void ResetListPageButton( WFNOTE_FRIENDLIST * p_wk );
static BOOL MainListPageButtonDecedeAnime( WFNOTE_FRIENDLIST * p_wk );

static void SetCancelButtonAnime( WFNOTE_DRAW * p_draw );
static void SetExitButtonAnime( WFNOTE_DRAW * p_draw );
static void SetDispActive( WFNOTE_DRAW * p_draw, BOOL flg );
static void ChangeListPageNumAnime( WFNOTE_FRIENDLIST * p_wk, u32 page );
static void PutListTitle( WFNOTE_DRAW * p_draw );
static void SetListDispActive( WFNOTE_FRIENDLIST * p_wk, WFNOTE_DRAW * p_draw, BOOL flg );

static void InitPrintUtil( WFNOTE_DRAW * p_draw );
static void SetPrintUtil( WFNOTE_DRAW * p_draw, PRINT_UTIL * util );
static void TransPrintUtil( WFNOTE_DRAW * p_draw );
static void PutBmpWin( GFL_BMPWIN * win );

static void FInfo_TrGraDelete( WFNOTE_FRIENDINFO * p_wk );

FS_EXTERN_OVERLAY(ui_common);


#pragma mark[>Data
//-----------------------------------------------------------------------------
/**
 *    �f�[�^
 */
//-----------------------------------------------------------------------------

const GFL_PROC_DATA WifiNoteProcData =
{
  WifiNoteProc_Init,
  WifiNoteProc_Main,
  WifiNoteProc_End,
};

//�@�X�N���[���G���A
static const WFNOTE_SCRNAREA DATA_ScrnArea[ WFNOTE_DRAWAREA_NUM ] = {
  { 0, 0, 32, 24 },
  { 32, 0, 32, 24 },
  { -32, 0, 32, 24 }
};

// BG�R���g���[���f�[�^
static const GFL_BG_BGCNT_HEADER DATA_BgCntTbl[ WFNOTE_BG_NUM ] = {
  { // DFRM_BACK
    0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000,0x4000, GX_BG_EXTPLTT_01,
    3, 0, 0, FALSE
  },
  { // DFRM_MSG
    0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0xc800, GX_BG_CHARBASE_0x04000,0x8000, GX_BG_EXTPLTT_01,
    0, 0, 0, FALSE
  },
  { // DFRM_SCROLL
    0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000,0x8000, GX_BG_EXTPLTT_01,
    2, 0, 0, FALSE
  },
  { // DFRM_SCRMSG
    0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x18000,0x8000, GX_BG_EXTPLTT_01,
    1, 0, 0, FALSE
  },
/*
  { // UFRM_MSG
    0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000,0x8000, GX_BG_EXTPLTT_01,
    0, 0, 0, FALSE
  },
  { // UFRM_BASE
    0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000,0x0000, GX_BG_EXTPLTT_01,
    1, 0, 0, FALSE
  },
*/
  { // UFRM_BACK
    0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000,0x8000, GX_BG_EXTPLTT_01,
    0, 0, 0, FALSE
  },
};

// �X�N���[���p�T�[�t�F�[�X
static GFL_REND_SURFACE_INIT DATA_ScrollSurfaceRect = {
  0, 0,
  256, 192,
  CLSYS_DRAW_MAIN
};

static const GFL_DISP_VRAM vramSetTable = {
  GX_VRAM_BG_128_A,             // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,       // ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,         // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g

  GX_VRAM_OBJ_128_B,            // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,      // ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_16_I,         // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_NONE,             // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,         // �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_128K,  // ���C��OBJ�}�b�s���O���[�h
  GX_OBJVRAMMODE_CHAR_1D_128K,  // �T�uOBJ�}�b�s���O���[�h
};

//-------------------------------------
/// FRIENDLIST
//=====================================
static const WFNOTE_FLIST_CURSORDATA DATA_FListCursorData[] = {
  // �v���C���[0�`8
  {
    { 2, 4, 15, 4 },
    GFL_BG_FRAME2_M, BGPLT_M_BACK_2, BGPLT_M_BACK_8,
  },
  {
    { 2, 8, 15, 4 },
    GFL_BG_FRAME2_M, BGPLT_M_BACK_2, BGPLT_M_BACK_8,
  },
  {
    { 2, 12, 15, 4 },
    GFL_BG_FRAME2_M, BGPLT_M_BACK_2, BGPLT_M_BACK_8,
  },
  {
    { 2, 16, 15, 4 },
    GFL_BG_FRAME2_M, BGPLT_M_BACK_2, BGPLT_M_BACK_8,
  },
  {
    { 16, 4, 15, 4 },
    GFL_BG_FRAME2_M, BGPLT_M_BACK_2, BGPLT_M_BACK_8,
  },
  {
    { 16, 8, 15, 4 },
    GFL_BG_FRAME2_M, BGPLT_M_BACK_2, BGPLT_M_BACK_8,
  },
  {
    { 16, 12, 15, 4 },
    GFL_BG_FRAME2_M, BGPLT_M_BACK_2, BGPLT_M_BACK_8,
  },
  {
    { 16, 16, 15, 4 },
    GFL_BG_FRAME2_M, BGPLT_M_BACK_2, BGPLT_M_BACK_8,
  },
/*
  // �߂�
  {
    { FLIST_BACK_X, FLIST_BACK_Y, FLIST_BACK_SIZX, FLIST_BACK_SIZY },
    GFL_BG_FRAME0_M, BGPLT_M_BACK_1, BGPLT_M_BACK_8,
  },
*/
};

// ���j���[�f�[�^
static const WFNOTE_MENU_DATA DATA_FListMenuTbl[BMPL_FLIST_MENU_NUM][BMPL_FLIST_MENU_LISTNUM] = {
  {
    { msg_wifi_note_09, (u32)SEQ_FLIST_INFOINIT },
    { msg_wifi_note_10, (u32)SEQ_FLIST_NAMECHG_INIT },
    { msg_wifi_note_11, (u32)SEQ_FLIST_DELETE_INIT },
    { msg_wifi_note_12, (u32)SEQ_FLIST_MAIN },
  },
  {
    { msg_wifi_note_33, (u32)SEQ_FLIST_CODE_INIT },
    { msg_wifi_note_10, (u32)SEQ_FLIST_NAMECHG_INIT },
    { msg_wifi_note_11, (u32)SEQ_FLIST_DELETE_INIT },
    { msg_wifi_note_12, (u32)SEQ_FLIST_MAIN },
  }
};

#if USE_SEL_BOX
///�Z���N�g�{�b�N�X�@�w�b�_�f�[�^�\����
static const SELBOX_HEAD_PRM DATA_SBoxHeadParamFList = {
  TRUE,SBOX_OFSTYPE_CENTER, ///<���[�v�t���O�A����or�Z���^�����O

  4,  ///<���ڕ\���I�t�Z�b�gX���W(dot)

  DFRM_MSG,       ///<�t���[��No
  BGPLT_M_SBOX,     ///<�t���[���p���b�gID
  0,0,          ///oam BG Pri,Software Pri
  SBOX_FLIST_FCGX,SBOX_FLIST_WCGX,  ///<�E�B���h�Ecgx,�t���[��cgx
  SBOX_FLIST_FCGX_SIZ,    ///<BMP�E�B���h�E��L�L�����N�^�̈�T�C�Y(char)
};
#endif
//-------------------------------------
/// FRIENDINFO
//=====================================
static const WFNOTE_FINFO_BMPDATA DATA_Page00Msg[ FINFO_PAGE00_BA_NUM ] = {
  {
    FINFO_PAGE00_BA_CAP_X, FINFO_PAGE00_BA_CAP_BTT_Y,
    FINFO_PAGE00_BA_CAP_SIZX, FINFO_PAGE00_BA_CAP_SIZY,
  },
  /*
  {
    FINFO_PAGE00_BA_CAP_X, FINFO_PAGE00_BA_CAP_TRT_Y,
    FINFO_PAGE00_BA_CAP_SIZX, FINFO_PAGE00_BA_CAP_SIZY,
  },
  {
    FINFO_PAGE00_BA_BT_X, FINFO_PAGE00_BA_BT_Y,
    FINFO_PAGE00_BA_BT_SIZX, FINFO_PAGE00_BA_BT_SIZY,
  },
  {
    FINFO_PAGE00_BA_TR_X, FINFO_PAGE00_BA_TR_Y,
    FINFO_PAGE00_BA_TR_SIZX, FINFO_PAGE00_BA_TR_SIZY,
  },
  */
};
static const WFNOTE_FINFO_BMPDATA DATA_Page01Msg[ FINFO_PAGE01_BA_NUM ] = {
  {
    FINFO_PAGE01_BA_CAP_X, FINFO_PAGE01_BA_CAP_Y,
    FINFO_PAGE01_BA_CAP_SIZX, FINFO_PAGE01_BA_CAP_SIZY,
  },
  /*
  {
    FINFO_PAGE01_BA_DAT_X, FINFO_PAGE01_BA_DAT_Y,
    FINFO_PAGE01_BA_DAT_SIZX, FINFO_PAGE01_BA_DAT_SIZY,
  },
  */
};
static const WFNOTE_FINFO_BMPDATA DATA_Page02Msg[ FINFO_PAGE02_BA_NUM ] = {
  {
    FINFO_PAGE02_BA_TT_X, FINFO_PAGE02_BA_TT_Y,
    FINFO_PAGE02_BA_TT_SIZX, FINFO_PAGE02_BA_TT_SIZY,
  },
  /*
  {
    FINFO_PAGE02_BA_LV50TT_X, FINFO_PAGE02_BA_LV50TT_Y,
    FINFO_PAGE02_BA_LV50TT_SIZX, FINFO_PAGE02_BA_LV50TT_SIZY,
  },
  {
    FINFO_PAGE02_BA_LV50NO_X, FINFO_PAGE02_BA_LV50NO_Y,
    FINFO_PAGE02_BA_LV50NO_SIZX, FINFO_PAGE02_BA_LV50NO_SIZY,
  },
  {
    FINFO_PAGE02_BA_LV100TT_X, FINFO_PAGE02_BA_LV100TT_Y,
    FINFO_PAGE02_BA_LV100TT_SIZX, FINFO_PAGE02_BA_LV100TT_SIZY,
  },
  {
    FINFO_PAGE02_BA_LV100NO_X, FINFO_PAGE02_BA_LV100NO_Y,
    FINFO_PAGE02_BA_LV100NO_SIZX, FINFO_PAGE02_BA_LV100NO_SIZY,
  },
  */
};
static const WFNOTE_FINFO_BMPDATA DATA_Page03Msg[ FINFO_PAGE03_BA_NUM ] = {
  {
    FINFO_PAGE03_BA_X, FINFO_PAGE03_BA_Y,
    FINFO_PAGE03_BA_SIZX, FINFO_PAGE03_BA_SIZY,
  },
};
static const WFNOTE_FINFO_BMPDATA DATA_Page04Msg[ FINFO_PAGE04_BA_NUM ] = {
  {
    FINFO_PAGE04_BA_X, FINFO_PAGE04_BA_Y,
    FINFO_PAGE04_BA_SIZX, FINFO_PAGE04_BA_SIZY,
  },
};
static const WFNOTE_FINFO_BMPDATA DATA_Page05Msg[ FINFO_PAGE05_BA_NUM ] = {
  {
    FINFO_PAGE05_BA_X, FINFO_PAGE05_BA_Y,
    FINFO_PAGE05_BA_SIZX, FINFO_PAGE05_BA_SIZY,
  },
};
static const WFNOTE_FINFO_BMPDATA DATA_Page06Msg[ FINFO_PAGE06_BA_NUM ] = {
  {
    FINFO_PAGE06_BA_X, FINFO_PAGE06_BA_Y,
    FINFO_PAGE06_BA_SIZX, FINFO_PAGE06_BA_SIZY,
  },
};
static const WFNOTE_FINFO_BMPDATA DATA_Page07Msg[ FINFO_PAGE07_BA_NUM ] = {
  {
    FINFO_PAGE07_BA_X, FINFO_PAGE07_BA_Y,
    FINFO_PAGE07_BA_SIZX, FINFO_PAGE07_BA_SIZY,
  },
};

// ���ׂĂ܂Ƃ߂��e�[�u��
static const WFNOTE_FINFO_BMPDATA* DATA_FInfo_PageBmpP[ FINFO_PAGE_NUM ] = {
  DATA_Page00Msg, DATA_Page06Msg, DATA_Page01Msg, DATA_Page02Msg,
  DATA_Page03Msg, DATA_Page04Msg, DATA_Page05Msg, DATA_Page07Msg,

};
static const u8 DATA_FInfo_PageBmpNum[ FINFO_PAGE_NUM ] = {
  FINFO_PAGE00_BA_NUM,
  FINFO_PAGE06_BA_NUM,
  FINFO_PAGE01_BA_NUM,
  FINFO_PAGE02_BA_NUM,
  FINFO_PAGE03_BA_NUM,
  FINFO_PAGE04_BA_NUM,
  FINFO_PAGE05_BA_NUM,
  FINFO_PAGE07_BA_NUM,
};







//----------------------------------------------------------------------------
/**
 *  @brief  �v���Z�X�@������
 *
 *  @param  proc    �v���Z�X���[�N
 *  @param  seq     �V�[�P���X
 *
 *  @retval PROC_RES_CONTINUE = 0,    ///<����p����
 *  @retval PROC_RES_FINISH,      ///<����I��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WifiNoteProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  WFNOTE_WK* p_wk;

  // wifi_2dchar���g�p���邽�߂�OVERLAY��LOAD
  GFL_OVERLAY_Load( FS_OVERLAY_ID(wifi2dmap) );
  // ui
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

  // ���[�N�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFINOTE, 0xC0000 );
  p_wk = GFL_PROC_AllocWork( proc, sizeof(WFNOTE_WK), HEAPID_WIFINOTE );
  GFL_STD_MemFill( p_wk, 0, sizeof(WFNOTE_WK) );
/*
  if( pwk == NULL )
  {
    p_wk->pp = GFL_HEAP_AllocMemory( HEAPID_WIFINOTE, sizeof( WIFINOTE_PROC_PARAM));
    p_wk->pp->pGameData = GAMEDATA_Create( HEAPID_WIFINOTE );
  }
  else
  {
    p_wk->pp = pwk;
  }
*/
  p_wk->pp = pwk;

#ifdef  PM_DEBUG
//  �f�o�b�O�N���Ŗ��O���������p
  {
    MYSTATUS  *myStatus = GAMEDATA_GetMyStatus(p_wk->pp->pGameData);//SaveData_GetMyStatus( p_wk->pp->saveControlWork );
    if( MyStatus_CheckNameClear( myStatus ) == TRUE )
    {
      STRBUF  *name = GFL_STR_CreateBuffer( 6 , HEAPID_WIFINOTE );
      u16   nameCode[5] = {L'��',L'��',L'��',L'��',0xFFFF};
      //���O�̃Z�b�g
      GFL_STR_SetStringCode( name , nameCode );
      MyStatus_SetMyNameFromString( myStatus , name );
      GFL_STR_DeleteBuffer( name );
    }
  }
#endif  // PM_DEBUG

  // �f�[�^���[�N����
  p_wk->vBlankTcb = NULL;
  Data_Init( &p_wk->data, p_wk->pp, HEAPID_WIFINOTE );

  // �`��V�X�e�����[�N������
  Draw_Init( &p_wk->draw, &p_wk->data, HEAPID_WIFINOTE );

  // �ʐM�A�C�R���ݒ�
  GFL_NET_WirelessIconEasy_HoldLCD( TRUE, HEAPID_WIFINOTE );
  GFL_NET_ReloadIcon();

  // �e��ʂ̃��[�N������
//  ModeSelect_Init( &p_wk->mode_select, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
//  FList_Init( &p_wk->friendlist, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
//  CodeIn_Init( &p_wk->codein, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
//  MyCode_Init( &p_wk->mycode, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
//  FInfo_Init( &p_wk->friendinfo, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );

  // ���荞��
  if( p_wk->vBlankTcb == NULL )
  {
    p_wk->vBlankTcb = GFUser_VIntr_CreateTCB( WFNOTE_VBlank , p_wk , 16 );
  }
//  sys_VBlankFuncChange( WFNOTE_VBlank, p_wk );
//  sys_HBlankIntrStop();

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���Z�X�@���C������
 *
 *  @param  proc    �v���Z�X���[�N
 *  @param  seq     �V�[�P���X
 *
 *  @retval PROC_RES_CONTINUE = 0,    ///<����p����
 *  @retval PROC_RES_FINISH,      ///<����I��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WifiNoteProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  WFNOTE_WK* p_wk  = mywk;
  WFNOTE_STRET stret;

  switch( *seq ){
  case WFNOTE_PROCSEQ_MAIN:

    switch( p_wk->status ){
    case STATUS_MODE_SELECT:  // �ŏ��̑I�����
      stret = ModeSelect_Main( &p_wk->mode_select, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
      break;

    case STATUS_FRIENDLIST: // �F�B�f�[�^�\��
      stret = FList_Main( &p_wk->friendlist, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
      break;

    case STATUS_CODEIN:   // �F�B�R�[�h����
      stret = CodeIn_Main( &p_wk->codein, p_wk, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
      break;

    case STATUS_MYCODE:   // �����̗F�B�R�[�h�m�F
      stret = MyCode_Main( &p_wk->mycode, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
      break;

    case STATUS_FRIENDINFO: // �F�B�ڍ׃f�[�^
      stret = FInfo_Main( &p_wk->friendinfo, &p_wk->data, &p_wk->draw,  HEAPID_WIFINOTE );
      break;

    case STATUS_END:      // �I���i�L�����Z���j
      p_wk->pp->retMode = WIFINOTE_RET_MODE_CANCEL;
      return GFL_PROC_RES_FINISH;

    case STATUS_END_X:      // �I���i�w�{�^���j
      p_wk->pp->retMode = WIFINOTE_RET_MODE_EXIT;
      return GFL_PROC_RES_FINISH;
    }

    // ��Ԑ؂�ւ�����
    if( stret == WFNOTE_STRET_FINISH ){
      p_wk->status = p_wk->data.reqstatus;
      p_wk->data.subseq = p_wk->data.reqsubseq;
      p_wk->wait = STATUS_CHANGE_WAIT;
      (*seq) = WFNOTE_PROCSEQ_WAIT;
    }
    break;

  case WFNOTE_PROCSEQ_WAIT:
    p_wk->wait --;
    if( p_wk->wait <= 0 ){
      (*seq) = WFNOTE_PROCSEQ_MAIN;
    }

    break;
  }


  // �`�惁�C��
  Draw_Main( &p_wk->draw );

  return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���Z�X�@�j��
 *
 *  @param  proc    �v���Z�X���[�N
 *  @param  seq     �V�[�P���X
 *
 *  @retval PROC_RES_CONTINUE = 0,    ///<����p����
 *  @retval PROC_RES_FINISH,  GF_BGL_BmpWinInit   ///<����I��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WifiNoteProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  WFNOTE_WK* p_wk  = mywk;

  // ���荞��
//  sys_VBlankFuncChange( NULL, NULL );
//  sys_HBlankIntrStop();
  GFL_TCB_DeleteTask( p_wk->vBlankTcb );
  p_wk->vBlankTcb = NULL;

  // �e��ʂ̃��[�N�j��
//  ModeSelect_Exit( &p_wk->mode_select, &p_wk->data, &p_wk->draw );
//  FList_Exit( &p_wk->friendlist, &p_wk->data, &p_wk->draw );
//  CodeIn_Exit( &p_wk->codein, &p_wk->data, &p_wk->draw );
//  MyCode_Exit( &p_wk->mycode, &p_wk->data, &p_wk->draw );
//  FInfo_Exit( &p_wk->friendinfo, &p_wk->data, &p_wk->draw );

  // �`��V�X�e�����[�N�j��
  Draw_Exit( &p_wk->draw );

  // �f�[�^���[�N�j��
  Data_Exit( &p_wk->data, p_wk->pp );

  //GFL_HEAP_FreeMemory( p_wk->pp );  //���̏����͊O���ōs����

/*
  if( pwk == NULL )
  {
    GFL_HEAP_FreeMemory( p_wk->pp->pGameData );
  }
*/

  GFL_PROC_FreeWork( proc );        // PROC���[�N�J��
  GFL_HEAP_DeleteHeap( HEAPID_WIFINOTE );

  // �I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(wifi2dmap) );

  return GFL_PROC_RES_FINISH;
}




//-----------------------------------------------------------------------------
/**
 *    �v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//-------------------------------------
/// �S�̊֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  VBlank�֐�
 *
 *  @param  p_work  �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void WFNOTE_VBlank( GFL_TCB *tcp, void *work )
{
  WFNOTE_WK* p_wk = work;

  // �`��VBLANK
  Draw_VBlank( &p_wk->draw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  WFNOTE�@������
 *
 *  @param  p_wk    ���[�N
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DrawInit( WFNOTE_WK* p_wk, HEAPID heapID )
{
  // �`��V�X�e�����[�N������
  Draw_Init( &p_wk->draw, &p_wk->data, heapID );

  // �e��ʂ̃��[�N������
//  ModeSelect_DrawInit( &p_wk->mode_select, &p_wk->data, &p_wk->draw, heapID );
//  FList_DrawInit( &p_wk->friendlist, &p_wk->data, &p_wk->draw, heapID );
//  CodeIn_DrawInit( &p_wk->codein, &p_wk->data, &p_wk->draw, heapID );
//  MyCode_DrawInit( &p_wk->mycode, &p_wk->data, &p_wk->draw, heapID );
//  FInfo_DrawInit( &p_wk->friendinfo, &p_wk->data, &p_wk->draw, heapID );

  // ���荞��
  if( p_wk->vBlankTcb == NULL )
  {
    p_wk->vBlankTcb = GFUser_VIntr_CreateTCB( WFNOTE_VBlank , p_wk , 16 );
  }
//  sys_VBlankFuncChange( WFNOTE_VBlank, p_wk );
//  sys_HBlankIntrStop();

}

//----------------------------------------------------------------------------
/**
 *  @brief  WFNOTE  �j��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DrawExit( WFNOTE_WK* p_wk )
{
  // ���荞��
//  sys_VBlankFuncChange( NULL, NULL );
//  sys_HBlankIntrStop();
  GFL_TCB_DeleteTask( p_wk->vBlankTcb );
  p_wk->vBlankTcb = NULL;

  // �e��ʂ̃��[�N�j��
//  ModeSelect_DrawExit( &p_wk->mode_select, &p_wk->data, &p_wk->draw );
//  FList_DrawExit( &p_wk->friendlist, &p_wk->data, &p_wk->draw );
//  CodeIn_DrawExit( &p_wk->codein, &p_wk->data, &p_wk->draw );
//  MyCode_DrawExit( &p_wk->mycode, &p_wk->data, &p_wk->draw );
//  FInfo_DrawExit( &p_wk->friendinfo, &p_wk->data, &p_wk->draw );

  // �`��V�X�e�����[�N�j��
  Draw_Exit( &p_wk->draw );
}

//-------------------------------------
/// �ėp�T�u�֐�
//=====================================

//----------------------------------------------------------------------------
/*
 *  @brief  BmpWindow�̒ǉ��Ə�����
 */
//-----------------------------------------------------------------------------
static void Sub_BmpWinAdd(
              WFNOTE_DRAW * p_draw, PRINT_UTIL * util, u8 frmnum,
              u8 posx, u8 posy, u8 sizx, u8 sizy, u8 palnum, u16 chrofs ,u8 clear_col)
{
  util->win = GFL_BMPWIN_Create( frmnum , posx , posy , sizx , sizy , palnum , GFL_BMP_CHRAREA_GET_B );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(util->win) , clear_col );
  SetPrintUtil( p_draw, util );
}

//-------------------------------------
/// �f�[�^���[�N�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  �f�[�^���[�N������
 *
 *  @param  p_data    �f�[�^���[�N
 *  @param  p_save    �Z�[�u�f�[�^
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Data_Init( WFNOTE_DATA* p_data, WIFINOTE_PROC_PARAM* wp, HEAPID heapID )
{
//  CONFIG* config;

  p_data->pGameData = wp->pGameData;
  p_data->codein.p_name = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_data->codein.p_code = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  //�L�[���[�h������
  p_data->key_mode = GFL_UI_CheckTouchOrKey();

  //�R���t�B�O
//  config = SaveData_GetConfig(p_data->p_save);
  p_data->msg_spd = MSGSPEED_GetWait();
  p_data->win_type = 0;//WINTYPE_01;
//  if(CONFIG_GetInputMode(config) != INPUTMODE_L_A){
    p_data->lr_key_enable = TRUE;
//  }else{
//    p_data->lr_key_enable = FALSE;
//  }
#if NOTE_TEMP_COMMENT
  MsgPrintSkipFlagSet( TRUE );
  MsgPrintTouchPanelFlagSet( TRUE );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�[�^���[�N�j��
 *
 *  @param  p_data  �f�[�^���[�N
 */
//-----------------------------------------------------------------------------
static void Data_Exit( WFNOTE_DATA* p_data, WIFINOTE_PROC_PARAM* wp )
{
#if NOTE_TEMP_COMMENT
  MsgPrintSkipFlagSet( FALSE );
  MsgPrintTouchPanelFlagSet( FALSE );
#endif

  //�L�[���[�h�����߂�
  GFL_UI_SetTouchOrKey(p_data->key_mode);

  GFL_STR_DeleteBuffer( p_data->codein.p_name );
  GFL_STR_DeleteBuffer( p_data->codein.p_code );
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���N�G�X�g�X�e�[�^�X�ݒ�
 *
 *  @param  p_data    ���[�N
 *  @param  reqstatus ���N�G�X�g�X�e�[�^�X
 *  @param  reqsubseq ���N�G�X�g�T�u�V�[�P���X
 */
//-----------------------------------------------------------------------------
static void Data_SetReqStatus( WFNOTE_DATA* p_data, u8 reqstatus, u8 reqsubseq )
{
  p_data->reqstatus = reqstatus;
  p_data->reqsubseq = reqsubseq;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �L�[���[�h�J�ڃ`�F�b�N
 *
 *  @param  p_data    ���[�N
 *  @param  reqstatus ���N�G�X�g�X�e�[�^�X
 *  @param  reqsubseq ���N�G�X�g�T�u�V�[�P���X
 */
//-----------------------------------------------------------------------------
/*
static void Data_CheckKeyModeChange( WFNOTE_DATA* p_data )
{
  if(GFL_UI_KEY_GetTrg() & GFL_PAD_BUTTON_KTST_CHG){
    p_data->key_mode = GFL_APP_END_KEY;
  }
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �f�[�^�ɃR�[�h���͉�ʂ̌��ʂ�ݒ肷��
 *
 *  @param  p_data    ���[�N
 *  @param  cp_name   ���O
 *  @param  cp_code   �R�[�h
 */
//-----------------------------------------------------------------------------
static void Data_CODEIN_Set( WFNOTE_DATA* p_data, const STRBUF* cp_name, const STRBUF* cp_code )
{
  if( cp_name ){
    GFL_STR_CopyBuffer( p_data->codein.p_name, cp_name );
  }
  if( cp_code ){
    GFL_STR_CopyBuffer( p_data->codein.p_code, cp_code );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ŐV�̗F�B�f�[�^�ݒ�
 *
 *  @param  p_data    ���[�N
 *  @param  p_code    �R�[�h������
 *  @param  p_name    ���O������
 */
//-----------------------------------------------------------------------------
static u32 Data_NewFriendDataSet( WFNOTE_DATA* p_data, STRBUF* p_code, STRBUF* p_name )
{
  int i;
  WIFI_LIST* p_list;
  BOOL result;
  BOOL set_check;
  u64 friendcode;
  u64 mycode;
  DWCFriendData *p_dwc_list = NULL;
  DWCUserData *p_dwc_userdata = NULL;
  int ret;
  int pos;

  p_list = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );

  mycode = DWC_CreateFriendKey( WifiList_GetMyUserInfo(p_list) );

  for( i=0; i<WIFILIST_FRIEND_MAX; i++ ){
    result = WifiList_IsFriendData( p_list, i );
    if( result == FALSE ){
      friendcode = WFNOTE_STRBUF_GetNumber(p_code, &set_check);

      if( (set_check == TRUE) && (friendcode !=mycode) ){

        p_dwc_userdata = WifiList_GetMyUserInfo( p_list );
        if(!DWC_CheckFriendKey(p_dwc_userdata, friendcode))
        {
          // �F�B�R�[�h���Ⴄ
          return RCODE_NEWFRIEND_SET_FRIENDKEYNG;
        }
        ret = GFL_NET_DWC_CheckFriendCodeByToken(p_list, friendcode, &pos);
        if(ret == DWCFRIEND_INLIST){
          // �����ݒ肸��
          return RCODE_NEWFRIEND_SET_SETING;
        }
        // ����
        p_dwc_list = WifiList_GetDwcDataPtr(p_list, i);
        DWC_CreateFriendKeyToken(p_dwc_list, friendcode);
        // ���O�Ɛ��ʐݒ� �����Őݒ�
        WifiList_SetFriendName(p_list, i, p_name);
        WifiList_SetFriendInfo(p_list, i, WIFILIST_FRIEND_SEX, PM_NEUTRAL);
        return RCODE_NEWFRIEND_SET_OK;
      }else{
        // �F�B�R�[�h���Ⴄ
        return RCODE_NEWFRIEND_SET_FRIENDKEYNG;
      }
    }
  }

  GF_ASSERT(0);
  // �����͂Ƃ���Ȃ��͂�
  return RCODE_NEWFRIEND_SET_OK;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���O����  �������f�[�^�ݒ�
 *
 *  @param  p_data    �f�[�^
 *  @param  cp_str    ������
 */
//-----------------------------------------------------------------------------
static void Data_NAMEIN_INIT_Set( WFNOTE_DATA* p_data, const STRCODE* cp_str )
{
  p_data->namein_init.cp_str = cp_str;
  p_data->namein_init.init = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �I���������X�g�C���f�b�N�X��ݒ肷��
 *
 *  @param  p_data    �f�[�^
 *  @param  idx     �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void Data_SELECT_ListIdxSet( WFNOTE_DATA* p_data, u32 idx )
{
  p_data->select_listidx = idx;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B���X�g�쐬
 *
 *  @param  p_data    �f�[�^
 */
//-----------------------------------------------------------------------------
static void Data_FrIdxMake( WFNOTE_DATA* p_data )
{
  int i;
  WIFI_LIST* p_list;

  GFL_STD_MemFill( &p_data->idx, 0, sizeof(WFNOTE_IDXDATA) );

  p_list = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );

  p_data->idx.friendnum = 0;
  for( i=0; i<FLIST_FRIEND_MAX; i++ ){
    if( WifiList_IsFriendData( p_list, i ) == TRUE ){
      p_data->idx.fridx[ p_data->idx.friendnum ] = i;

      if(WifiList_GetFriendInfo( p_list, i , WIFILIST_FRIEND_SEX ) == PM_NEUTRAL){
        p_data->idx.infoidx[p_data->idx.friendnum] = 0xFF;
      }else{
        p_data->idx.infoidx[p_data->idx.friendnum] = p_data->idx.infonum++;
      }

      p_data->idx.friendnum ++;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  WIFI���X�g�e�[�u������F�B�ԍ����擾����
 *
 *  @param  cp_data   �f�[�^
 *  @param  idx     ���X�g�C���f�b�N�X
 *
 *  @return �F�B�ԍ�
 */
//-----------------------------------------------------------------------------
static u32 Data_FrIdxGetIdx( const WFNOTE_DATA* cp_data, u32 idx )
{
  return cp_data->idx.fridx[ idx ];
}



//-------------------------------------
/// �`�惏�[�N�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  �`�惏�[�N������
 *
 *  @param  p_draw    ���[�N
 *  @param  cp_data   �f�[�^
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Draw_Init( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, HEAPID heapID )
{
  // �܂��s������
  GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
  p_draw->p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFI_NOTE, heapID );

  p_draw->heapID = heapID;
  Draw_BankInit();
  Draw_BgInit( p_draw, heapID );
  Draw_ClactInit( p_draw, heapID );
  Draw_MsgInit( p_draw, heapID );
  Draw_BmpInit( p_draw, cp_data, heapID );
  Draw_CommonActInit( p_draw, heapID );
  Draw_SCRNDATA_Init( p_draw, heapID );

  Draw_BlinkAnmInit( p_draw, heapID );

  // OAM�ʕ\��
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`�惏�[�N���C��
 *
 *  @param  p_draw    ���[�N
 */
//-----------------------------------------------------------------------------
static void Draw_Main( WFNOTE_DRAW* p_draw )
{
  PRINTSYS_QUE_Main( p_draw->printQue );
  TransPrintUtil( p_draw );

  GFL_TCBL_Main( p_draw->msgTcblSys );

  GFL_CLACT_SYS_Main();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`�惏�[�NV�u�����N
 *
 *  @param  p_draw    ���[�N
 */
//-----------------------------------------------------------------------------
static void Draw_VBlank( WFNOTE_DRAW* p_draw )
{
  /*
  GF_BGL_VBlankFunc( p_draw->p_bgl );
  DoVramTransferManager();
  REND_OAMTrans();
  */
  GFL_CLACT_SYS_VBlankFunc();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`�惏�[�N�@�j��
 *
 *  @param  p_draw  ���[�N
 */
//-----------------------------------------------------------------------------
static void Draw_Exit( WFNOTE_DRAW* p_draw )
{
  Draw_BlinkAnmExit( p_draw );

  Draw_CommonActExit( p_draw );
  Draw_SCRNDATA_Exit( p_draw );
  Draw_BmpExit( p_draw );
  Draw_MsgExit( p_draw );
  Draw_ClactExit( p_draw );
  Draw_BgExit( p_draw );

  GFL_ARC_CloseDataHandle( p_draw->p_handle );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o���N�ݒ�
 */
//-----------------------------------------------------------------------------
static void Draw_BankInit( void )
{
  GFL_DISP_SetBank( &vramSetTable );
}

//----------------------------------------------------------------------------
/**
 *  @brief  BG������
 *
 *  @param  p_draw    ���[�N
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Draw_BgInit( WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  int i;

  GFL_BG_Init( heapID );

  // BG SYSTEM
  {
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  // BG�R���g���[���ݒ�
  for( i=0; i<WFNOTE_BG_NUM; i++ ){
    GFL_BG_SetBGControl( DFRM_BACK+i, &DATA_BgCntTbl[i], GFL_BG_MODE_TEXT );
    GFL_BG_SetClearCharacter( DFRM_BACK+i, 32, 0, heapID );
    GFL_BG_ClearScreenCode( DFRM_BACK+i , 0 );
    GFL_BG_SetVisible( DFRM_BACK+i , TRUE );
  }

  // ��{�I�ȃo�b�N�O���E���h�Ǎ��݁��]��
  // �L�����N�^�f�[�^�́A�X�N���[���ʂɂ��]��
  GFL_ARCHDL_UTIL_TransVramPalette( p_draw->p_handle, NARC_wifi_note_friend_bg_d_NCLR, PALTYPE_MAIN_BG,
      BGPLT_M_BACK_0, BGPLT_M_BACK_NUM*32, heapID );
  GFL_ARCHDL_UTIL_TransVramPalette( p_draw->p_handle, NARC_wifi_note_friend_bg_d_NCLR, PALTYPE_SUB_BG,
      BGPLT_S_BACK_0, BGPLT_S_BACK_NUM*32, heapID );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_draw->p_handle, NARC_wifi_note_friend_bg_d_NCGR,
      DFRM_BACK, 0, 0, FALSE, heapID );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_draw->p_handle, NARC_wifi_note_friend_bg_d_NCGR,
      DFRM_SCROLL, 0, 0, FALSE, heapID );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_draw->p_handle, NARC_wifi_note_friend_bg_d_NCGR,
      UFRM_BACK, 0, 0, FALSE, heapID );

  GFL_ARCHDL_UTIL_TransVramScreen( p_draw->p_handle, NARC_wifi_note_friend_back_d_NSCR,
      DFRM_BACK, 0, 0, FALSE, heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( p_draw->p_handle, NARC_wifi_note_friend_top_u_NSCR,
      UFRM_BACK, 0, 0, FALSE, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  BG�j��
 *
 *  @param  p_draw    ���[�N
 */
//-----------------------------------------------------------------------------
static void Draw_BgExit( WFNOTE_DRAW* p_draw )
{
  int i;

  // BG�R���g���[���j��
  for( i=0; i<WFNOTE_BG_NUM; i++ ){
    GFL_BG_FreeBGControl( DFRM_BACK+i );
  }

  GFL_BG_Exit();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �Z���A�N�^�[������
 *
 *  @param  p_draw    ���[�N
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Draw_ClactInit( WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  //OBJ�̏�����
  GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
  cellSysInitData.oamst_main = 0x10;  //�f�o�b�O���[�^�̕�
  cellSysInitData.oamnum_main = 128-0x10;
  GFL_CLACT_SYS_Create( &cellSysInitData , &vramSetTable ,heapID );

  p_draw->cellUnit  = GFL_CLACT_UNIT_Create( 32 , 0, heapID );
  GFL_CLACT_UNIT_SetDefaultRend( p_draw->cellUnit );

  //�X�N���[���p�����_���[�쐬
  p_draw->scrollRender = GFL_CLACT_USERREND_Create(&DATA_ScrollSurfaceRect,1,heapID );
  p_draw->cellUnitScroll = GFL_CLACT_UNIT_Create( 18 , 1 , heapID );
  GFL_CLACT_UNIT_SetUserRend( p_draw->cellUnitScroll , p_draw->scrollRender );

  //���\�[�X�̓ǂݍ���
  // �F�B�蒠��p
  p_draw->chrRes[CHR_RES_WOR] = GFL_CLGRP_CGR_Register(
                                  p_draw->p_handle,
                                  NARC_wifi_note_friend_obj_d_NCGR,
                                  FALSE , CLSYS_DRAW_MAIN, heapID );
  p_draw->palRes[PAL_RES_WOR] = GFL_CLGRP_PLTT_Register(
                                  p_draw->p_handle,
                                  NARC_wifi_note_friend_obj_d_NCLR,
                                  CLSYS_DRAW_MAIN, CLACT_PALNUM_WIFINOTE*0x20, heapID );
  p_draw->celRes[CEL_RES_WOR] = GFL_CLGRP_CELLANIM_Register(
                                  p_draw->p_handle,
                                  NARC_wifi_note_friend_obj_d_NCER,
                                  NARC_wifi_note_friend_obj_d_NANR, heapID );
  // �^�b�`�o�[
  {
    ARCHANDLE * ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );
    p_draw->chrRes[CHR_RES_TB] = GFL_CLGRP_CGR_Register(
                                  ah, APP_COMMON_GetBarIconCharArcIdx(),
                                  FALSE, CLSYS_DRAW_MAIN, heapID );
    p_draw->palRes[PAL_RES_TB] = GFL_CLGRP_PLTT_Register(
                                  ah, APP_COMMON_GetBarIconPltArcIdx(),
                                  CLSYS_DRAW_MAIN, CLACT_PALNUM_TB*0x20, heapID );
    p_draw->celRes[CEL_RES_TB] = GFL_CLGRP_CELLANIM_Register(
                                  ah,
                                  APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K),
                                  APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K),
                                  heapID );
    GFL_ARC_CloseDataHandle( ah );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  �Z���A�N�^�[�j��
 *
 *  @param  p_draw  ���[�N
 */
//-----------------------------------------------------------------------------
static void Draw_ClactExit( WFNOTE_DRAW* p_draw )
{
  u32 i;

  for( i=0; i<CHR_RES_MAX; i++ ){
    GFL_CLGRP_CGR_Release( p_draw->chrRes[i] );
  }
  for( i=0; i<PAL_RES_MAX; i++ ){
    GFL_CLGRP_PLTT_Release( p_draw->palRes[i] );
  }
  for( i=0; i<CEL_RES_MAX; i++ ){
    GFL_CLGRP_CELLANIM_Release( p_draw->celRes[i] );
  }
  GFL_CLACT_UNIT_Delete( p_draw->cellUnitScroll );
  GFL_CLACT_USERREND_Delete( p_draw->scrollRender );
  GFL_CLACT_UNIT_Delete( p_draw->cellUnit );
  GFL_CLACT_SYS_Delete();
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�@������
 *
 *  @param  p_draw    ���[�N
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Draw_MsgInit( WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  GFL_FONTSYS_SetDefaultColor();    // �f�t�H���g�����F�Z�b�g�i���ꂪ�Ȃ��ƍŏ��̃��b�Z�[�W�\���ŕ������o�Ȃ��j

  p_draw->p_wordset = WORDSET_Create( heapID );
  p_draw->p_msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_note_dat, heapID );

  //�^�b�`�t�H���g���[�h
  p_draw->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , heapID );

  p_draw->msgTcblSys = GFL_TCBL_Init( heapID , heapID , 3 , 0x100 );
  p_draw->printHandle = NULL;
  p_draw->printHandleMsg = NULL;

  p_draw->printQue = PRINTSYS_QUE_Create( heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�@�j��
 *
 *  @param  p_draw    ���[�N
 */
//-----------------------------------------------------------------------------
static void Draw_MsgExit( WFNOTE_DRAW* p_draw )
{
  PRINTSYS_QUE_Clear( p_draw->printQue );
  PRINTSYS_QUE_Delete( p_draw->printQue );

  if( p_draw->printHandle != NULL ){
    PRINTSYS_PrintStreamDelete( p_draw->printHandle );
    p_draw->printHandle = NULL;
  }
  if( p_draw->printHandleMsg != NULL ){
    PRINTSYS_PrintStreamDelete( p_draw->printHandleMsg );
    p_draw->printHandleMsg = NULL;
  }

  GFL_TCBL_Exit( p_draw->msgTcblSys );
  //�^�b�`�t�H���g�A�����[�h
  GFL_FONT_Delete( p_draw->fontHandle );

  GFL_MSG_Delete( p_draw->p_msgman );
  WORDSET_Delete( p_draw->p_wordset );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �r�b�g�}�b�v�֘A������
 *
 *  @param  p_draw
 *  @param  cp_data
 *  @param  heapID
 */
//-----------------------------------------------------------------------------
static void Draw_BmpInit( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, HEAPID heapID )
{
  // ��{�E�B���h�E�f�[�^�Ǎ���
//  TalkFontPaletteLoad( PALTYPE_MAIN_BG, BGPLT_M_MSGFONT*0x20, heapID );
//  TalkFontPaletteLoad( PALTYPE_SUB_BG, BGPLT_S_MSGFONT*0x20, heapID );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , BGPLT_M_MSGFONT*0x20, 16*2, heapID );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , BGPLT_S_MSGFONT*0x20, 16*2, heapID );

  BmpWinFrame_GraphicSet( DFRM_MSG, BMPL_WIN_CGX_TALK ,
    BGPLT_M_TALKWIN, cp_data->win_type, heapID );

  // ��{�r�b�g�}�b�v
  GFL_BMPWIN_Init( heapID );
  Sub_BmpWinAdd(
      p_draw,
      &p_draw->title,
      DFRM_MSG,
      BMPL_TITLE_X, BMPL_TITLE_Y,
      BMPL_TITLE_SIZX, BMPL_TITLE_SIZY,
      BGPLT_M_MSGFONT, BMPL_TITLE_CGX , 0 );

  // �^�C�g���p������o�b�t�@�쐬
  p_draw->p_titlestr = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_draw->p_titletmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_draw->p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_draw->p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

/*
#if USE_SEL_BOX
  p_draw->sbox = SelectBoxSys_AllocWork( heapID, NULL );
#endif

  //YesNo�{�^���V�X�e�����[�N�m��
  p_draw->ynbtn_wk = TOUCH_SW_AllocWork(heapID);
*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  �r�b�g�}�b�v�j��
 *
 *  @param  p_draw  ���[�N
 */
//-----------------------------------------------------------------------------
static void Draw_BmpExit( WFNOTE_DRAW* p_draw )
{
/*
  //YesNo�{�^���V�X�e�����[�N���
  TOUCH_SW_FreeWork( p_draw->ynbtn_wk );

#if USE_SEL_BOX
  SelectBoxSys_Free(p_draw->sbox);
#endif
*/

  GFL_STR_DeleteBuffer( p_draw->p_str );
  GFL_STR_DeleteBuffer( p_draw->p_tmp );
  GFL_STR_DeleteBuffer( p_draw->p_titlestr );
  GFL_STR_DeleteBuffer( p_draw->p_titletmp );
  GFL_BMPWIN_Delete( p_draw->title.win );
  GFL_BMPWIN_Exit();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ��{�X�N���[���f�[�^  ������
 *
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Draw_SCRNDATA_Init( WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  static const u8 sc_SCRNDATA[ WFNOTE_SCRNDATA_NUM ] = {
    NARC_wifi_note_friend_listwin_d_NSCR,
    NARC_wifi_note_friend_parts_d_NSCR,
  };
  int i;
  // �X�N���[���f�[�^������
  for( i=0; i<WFNOTE_SCRNDATA_NUM; i++ ){
    p_draw->scrn.p_scrnbuff[i] = GFL_ARCHDL_UTIL_LoadScreen( p_draw->p_handle,
        sc_SCRNDATA[i], FALSE, &p_draw->scrn.p_scrn[i], heapID );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���f�[�^�j��
 *
 *  @param  p_draw  �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void Draw_SCRNDATA_Exit( WFNOTE_DRAW* p_draw )
{
  int i;

  // �X�N���[���f�[�^������
  for( i=0; i<WFNOTE_SCRNDATA_NUM; i++ ){
    GFL_HEAP_FreeMemory( p_draw->scrn.p_scrnbuff[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �^�C�g���̕`��
 *
 *  @param  p_draw  �`�惏�[�N
 *  @param  msg_idx ���b�Z�[�WID
 */
//-----------------------------------------------------------------------------
static void Draw_BmpTitleWrite( WFNOTE_DRAW* p_draw, PRINT_UTIL * util, u32 msg_idx )
{
/*
  // �^�X�N�����~
  if( p_draw->printHandle != NULL ){
    PRINTSYS_PrintStreamDelete( p_draw->printHandle );
    p_draw->printHandle = NULL;
  }
*/

  // �N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(util->win), 0 );

  GFL_MSG_GetString( p_draw->p_msgman, msg_idx, p_draw->p_titletmp );
  WORDSET_ExpandStr( p_draw->p_wordset, p_draw->p_titlestr, p_draw->p_titletmp );
/*
  PRINTSYS_PrintQueColor(
    p_draw->printQue, GFL_BMPWIN_GetBmp(*win),
    0, BMPL_TITLE_PL_Y,
    p_draw->p_titlestr, p_draw->fontHandle, WFNOTE_COL_WHITE );
*/
  PRINTTOOL_PrintColor(
    util, p_draw->printQue,
    0, BMPL_TITLE_PL_Y,
    p_draw->p_titlestr, p_draw->fontHandle, WFNOTE_COL_WHITE, PRINTTOOL_MODE_LEFT );

  PutBmpWin( util->win );
/*
//    GF_BGL_BmpWinOnVReq(win);
  GFL_BMPWIN_TransVramCharacter( *win );
  GFL_BMPWIN_MakeScreen( *win );
  GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(*win) );
*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  �^�C�g���\��Off
 *
 *  @param  p_draw    ���[�N
 */
//-----------------------------------------------------------------------------
static void Draw_BmpTitleOff( WFNOTE_DRAW* p_draw )
{
  // �^�X�N�����~
  if( p_draw->printHandle != NULL ){
    PRINTSYS_PrintStreamDelete( p_draw->printHandle );
    p_draw->printHandle = NULL;
  }

//  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->title), 0 );
//  GFL_BMPWIN_TransVramCharacter( p_draw->title );
  GFL_BMPWIN_ClearTransWindow_VBlank( p_draw->title.win );
//  GF_BGL_BmpWinOffVReq(&p_draw->title);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�R�[�h�����[�h�Z�b�g�ɐݒ�
 *
 *  @param  p_draw  �`��V�X�e��
 *  @param  code  �R�[�h
 */
//-----------------------------------------------------------------------------
static void Draw_FriendCodeSetWordset( WFNOTE_DRAW* p_draw, u64 code )
{
  u64 num;

  num = code / 100000000;
  WORDSET_RegisterNumber(p_draw->p_wordset, 1, num, 4,STR_NUM_DISP_ZERO,STR_NUM_CODE_DEFAULT);
  num = (code/10000) % 10000;
  WORDSET_RegisterNumber(p_draw->p_wordset, 2, num, 4,STR_NUM_DISP_ZERO,STR_NUM_CODE_DEFAULT);
  num = code % 10000;
  WORDSET_RegisterNumber(p_draw->p_wordset, 3, num, 4,STR_NUM_DISP_ZERO,STR_NUM_CODE_DEFAULT);

}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�̖��O��ݒ肷��
 *
 *  @param  p_draw    �`��V�X�e��
 *  @param  p_save    �Z�[�u�f�[�^
 *  @param  idx     �F�B�̃C���f�b�N�X
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Draw_FriendNameSetWordset( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, u32 idx, HEAPID heapID )
{
  WIFI_LIST* p_wifilist;
  MYSTATUS* p_mystatus;

  p_wifilist = GAMEDATA_GetWiFiList(cp_data->pGameData); //SaveData_GetWifiListData( p_save );
  p_mystatus = MyStatus_AllocWork( heapID );
#if NOTE_DEBUG
  {
    STRBUF  *nameStr = GFL_STR_CreateBuffer( 10 , heapID );
    GFL_STR_SetStringCode( nameStr, WifiList_GetFriendNamePtr(p_wifilist, idx) );
    MyStatus_SetMyNameFromString( p_mystatus, nameStr );
    GFL_STR_DeleteBuffer( nameStr );
  }
#else
  MyStatus_SetMyName( p_mystatus, WifiList_GetFriendNamePtr(p_wifilist, idx) );
#endif
  WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, p_mystatus );
  GFL_HEAP_FreeMemory( p_mystatus );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�̃O���[�v�����[�h�Z�b�g�ɐݒ肷��
 *
 *  @param  p_draw    �`��V�X�e��
 *  @param  p_save    �f�[�^
 *  @param  idx     �C���f�b�N�X
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Draw_FriendGroupSetWordset( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, u32 idx, HEAPID heapID )
{
#if 0
  MYSTATUS* p_mystatus = MyStatus_AllocWork( heapID );
  WIFI_LIST* p_wifilist = GAMEDATA_GetWiFiList(cp_data->pGameData); //SaveData_GetWifiListData( p_save );
#if NOTE_DEBUG
  {
    STRBUF  *nameStr = GFL_STR_CreateBuffer( 10 , heapID );
    GFL_STR_SetStringCode( nameStr, WifiList_GetFriendGroupNamePtr( p_wifilist, idx ) );
    MyStatus_SetMyNameFromString( p_mystatus, nameStr );
    GFL_STR_DeleteBuffer( nameStr );
  }
#else
  MyStatus_SetMyName(p_mystatus, WifiList_GetFriendGroupNamePtr( p_wifilist, idx ));
#endif
  WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, p_mystatus );
  GFL_HEAP_FreeMemory( p_mystatus );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�ƍŌ�ɗV�񂾓��ɂ���ݒ�
 *
 *  @param  p_draw    �`��V�X�e��
 *  @param  p_save    �Z�[�u�f�[�^
 *  @param  idx     �F�B���X�g�C���f�b�N�X
 *
 *  @retval TRUE  ���ɂ���������
 *  @retval FALSE ���ɂ����Ȃ�����
 */
//-----------------------------------------------------------------------------
static BOOL Draw_FriendDaySetWordset( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, u32 idx )
{
  WIFI_LIST* p_wifilist = GAMEDATA_GetWiFiList(cp_data->pGameData); //SaveData_GetWifiListData( p_save );
  u32 num;
  BOOL ret = TRUE;

  num = WifiList_GetFriendInfo( p_wifilist, idx, WIFILIST_FRIEND_LASTBT_DAY);
  if( num == 0 ){
    ret = FALSE;  // �O�Ƃ������Ƃ͂P����V��ł��Ȃ�
  }
  WORDSET_RegisterNumber( p_draw->p_wordset, 2, num, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
  num = WifiList_GetFriendInfo( p_wifilist, idx, WIFILIST_FRIEND_LASTBT_YEAR);
  WORDSET_RegisterNumber( p_draw->p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
  num = WifiList_GetFriendInfo( p_wifilist, idx, WIFILIST_FRIEND_LASTBT_MONTH);
  WORDSET_RegisterNumber( p_draw->p_wordset, 1, num, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);

  return ret;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�Z�b�g�ɐ�����ݒ肷��
 *
 *  @param  p_draw    �`��V�X�e��
 *  @param  num     ����
 */
//-----------------------------------------------------------------------------
static void Draw_NumberSetWordset( WFNOTE_DRAW* p_draw, u32 num )
{
  WORDSET_RegisterNumber( p_draw->p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�Z�b�g�Ƀ��r�[�~�j�Q�[������ݒ�
 *
 *  @param  p_draw
 *  @param  num
 */
//-----------------------------------------------------------------------------
static void Draw_WflbyGameSetWordSet( WFNOTE_DRAW* p_draw, u32 num )
{
#if 0 //WiFi�L��֘A�͍폜
  WORDSET_RegisterWiFiLobbyGameName( p_draw->p_wordset, 0, num );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  ��󑼁@�R�����A�N�^�[������
 *
 *  @param  p_draw  �`�惏�[�N
 *  @param  heapID  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Draw_CommonActInit( WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  static GFL_CLWK_DATA act_add[4] = {
    { // ��
      TOUCHBAR_ICON_X_00,
      TOUCHBAR_ICON_Y,
      APP_COMMON_BARICON_CURSOR_DOWN,
      WFNOTE_YAZIRUSHI_PRI,
      2
    },
    { // ��
      TOUCHBAR_ICON_X_01,
      TOUCHBAR_ICON_Y,
      APP_COMMON_BARICON_CURSOR_UP,
      WFNOTE_YAZIRUSHI_PRI,
      2
    },
/*
    { // �J�[�\��
      0 ,
      0 ,
      ACTANM_CUR_LIST ,
      WFNOTE_YAZIRUSHI_PRI,
      0
    },
*/
    { // �^�b�`�o�[�L�����Z���{�^��
      TOUCHBAR_ICON_X_07,
      TOUCHBAR_ICON_Y,
      APP_COMMON_BARICON_RETURN,
      0,
      2
    },
    { // �^�b�`�o�[�w�{�^��
      TOUCHBAR_ICON_X_06,
      TOUCHBAR_ICON_Y,
      APP_COMMON_BARICON_EXIT,
      0,
      2
    },
  };
  int i;

  // ���A�N�^�[�쐬
  for( i=0; i<WFNOTE_YAZIRUSHINUM; i++ ){
    p_draw->p_yazirushi[i] = GFL_CLACT_WK_Create(
          p_draw->cellUnit,
          p_draw->chrRes[CHR_RES_TB],
          p_draw->palRes[PAL_RES_TB],
          p_draw->celRes[CEL_RES_TB],
          &act_add[i], CLSYS_DEFREND_MAIN,
          heapID );

    GFL_CLACT_WK_SetAutoAnmFlag( p_draw->p_yazirushi[i], TRUE );
    GFL_CLACT_WK_SetDrawEnable( p_draw->p_yazirushi[i], FALSE );
  }
  //�J�[�\���A�N�^�[�쐬
  //���X�g��ʂ̃L�����Ƃ̑O��֌W�ł�������unit�ɓo�^
/*
  p_draw->p_cur = GFL_CLACT_WK_Create(
        p_draw->cellUnitScroll,
        p_draw->chrRes[CHR_RES_WOR],
        p_draw->palRes[PAL_RES_WOR],
        p_draw->celRes[CEL_RES_WOR],
        &act_add[2] , CLSYS_DEFREND_MAIN,
        heapID );
  GFL_CLACT_WK_SetDrawEnable( p_draw->p_cur, FALSE );
*/
  // �^�b�`�o�[�L�����Z���{�^���A�N�^�[�쐬
  p_draw->p_tb = GFL_CLACT_WK_Create(
        p_draw->cellUnit,
        p_draw->chrRes[CHR_RES_TB],
        p_draw->palRes[PAL_RES_TB],
        p_draw->celRes[CEL_RES_TB],
        &act_add[2] , CLSYS_DEFREND_MAIN,
        heapID );
  GFL_CLACT_WK_SetDrawEnable( p_draw->p_tb, TRUE );
  // �^�b�`�o�[�w�{�^���A�N�^�[�쐬
  p_draw->p_tbx = GFL_CLACT_WK_Create(
        p_draw->cellUnit,
        p_draw->chrRes[CHR_RES_TB],
        p_draw->palRes[PAL_RES_TB],
        p_draw->celRes[CEL_RES_TB],
        &act_add[3] , CLSYS_DEFREND_MAIN,
        heapID );
  GFL_CLACT_WK_SetDrawEnable( p_draw->p_tbx, TRUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ��󑼁@�R�����A�N�^�[�j��
 *
 *  @param  p_draw  �`�惏�[�N
 */
//-----------------------------------------------------------------------------
static void Draw_CommonActExit( WFNOTE_DRAW* p_draw )
{
  int i;

  //�^�b�`�o�[�w�A�N�^�[�j��
  GFL_CLACT_WK_Remove( p_draw->p_tbx );
  //�^�b�`�o�[�L�����Z���A�N�^�[�j��
  GFL_CLACT_WK_Remove( p_draw->p_tb );
  //�J�[�\���A�N�^�[�j��
//  GFL_CLACT_WK_Remove( p_draw->p_cur );
  // ���A�N�^�[�j��
  for( i=0; i<WFNOTE_YAZIRUSHINUM; i++ ){
    GFL_CLACT_WK_Remove( p_draw->p_yazirushi[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���\���ݒ�
 *
 *  @param  p_draw  �`�惏�[�N
 *  @param  flag  �t���O
 */
//-----------------------------------------------------------------------------
static void Draw_YazirushiSetDrawFlag( WFNOTE_DRAW* p_draw, BOOL flag )
{
  int i;
  for( i=0; i<WFNOTE_YAZIRUSHINUM; i++ ){
    GFL_CLACT_WK_SetDrawEnable( p_draw->p_yazirushi[i], flag );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���A�j���ݒ�
 *
 *  @param  p_draw  �`�惏�[�N
 *  @param  flag  �t���O
 */
//-----------------------------------------------------------------------------
static void Draw_YazirushiSetAnmFlag( WFNOTE_DRAW* p_draw, BOOL flag )
{
  int i;
  for( i=0; i<WFNOTE_YAZIRUSHINUM; i++ ){
    GFL_CLACT_WK_SetAutoAnmFlag( p_draw->p_yazirushi[i], flag );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���A�j���Đ�
 *
 *  @param  p_draw  �`�惏�[�N
 *  @param  flag  �t���O
 */
//-----------------------------------------------------------------------------
static void Draw_YazirushiAnmChg( WFNOTE_DRAW* p_draw, u8 idx, u8 anm_no)
{
/*
  u8 anm_ofs = idx*2;
  if(idx >= ACT_YAZI_T){
    idx -= ACT_YAZI_T;
  }
*/
  GFL_CLACT_WK_SetAutoAnmFlag( p_draw->p_yazirushi[idx], TRUE );
//  GFL_CLACT_WK_SetAnmSeq( p_draw->p_yazirushi[idx], ACTANM_YAZI_LN+anm_ofs+anm_no);
  GFL_CLACT_WK_SetAnmSeq( p_draw->p_yazirushi[idx], anm_no );
  GFL_CLACT_WK_ResetAnm( p_draw->p_yazirushi[idx] );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�[�\���A�N�^�[�\���ݒ�
 *
 *  @param  p_draw  �`�惏�[�N
 *  @param  flag  �t���O
 */
//-----------------------------------------------------------------------------
/*
static void Draw_CursorActorSet( WFNOTE_DRAW* p_draw, BOOL draw_f, u8 anm_no )
{
  if(anm_no != ACTANM_NULL){
    GFL_CLACT_WK_SetAnmSeq( p_draw->p_cur, anm_no );
  }
  GFL_CLACT_WK_SetDrawEnable( p_draw->p_cur, draw_f );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �J�[�\���A�N�^�[�\���v���C�I���e�B�ݒ�
 *
 *  @param  p_draw  �`�惏�[�N
 *  @param  flag  �t���O
 */
//-----------------------------------------------------------------------------
/*
static void Draw_CursorActorPriSet( WFNOTE_DRAW* p_draw, u8 bg_pri, u32 s_pri )
{
  GFL_CLACT_WK_SetBgPri( p_draw->p_cur, bg_pri );
  GFL_CLACT_WK_SetSoftPri( p_draw->p_cur, s_pri );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  YesNo�E�B���h�E�J�n
 *
 *  @param  p_data  �f�[�^
 *  @param  p_draw  �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * Draw_YesNoWinSet( APP_TASKMENU_ITEMWORK * list, APP_TASKMENU_RES ** res, BOOL putType, HEAPID heapID )
{
  APP_TASKMENU_INITWORK wk;

  wk.heapId   = heapID;
  wk.itemNum  = 2;
  wk.itemWork = list;
  wk.posType  = ATPT_RIGHT_DOWN;
  wk.charPosX = 32;
  if( putType == TRUE ){
     wk.charPosY = 18;
  }else{
     wk.charPosY = 12;
  }
  wk.w        = APP_TASKMENU_PLATE_WIDTH_YN_WIN;
  wk.h        = APP_TASKMENU_PLATE_HEIGHT_YN_WIN;

  return APP_TASKMENU_OpenMenu( &wk, *res );
}

//----------------------------------------------------------------------------
/**
 *  @brief  YesNo�E�B���h�E�I���҂�
 *
 *  @param  p_data  �f�[�^
 *  @param  p_draw  �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static int Draw_YesNoWinMain( APP_TASKMENU_WORK * wk )
{
  APP_TASKMENU_UpdateMenu( wk );
  if( APP_TASKMENU_IsFinish( wk ) == TRUE ){
    int pos = APP_TASKMENU_GetCursorPos( wk );
    APP_TASKMENU_CloseMenu( wk );
    return pos;
  }
  return -1;
}

//-------------------------------------
/// �I�����[�h�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�I����ʁ@���[�N������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`�惏�[�N
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void ModeSelect_Init( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  p_data->key_mode = GFL_UI_CheckTouchOrKey();  // �L�[�^�b�`�ݒ�
  ModeSelect_DrawInit( p_wk, p_data, p_draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�I�����  ���C��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 *
 *  @retval WFNOTE_STRET_CONTINUE,  // �Â�
 *  @retval WFNOTE_STRET_FINISH,  // ��Ƃ͏I�����
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET ModeSelect_Main( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  BOOL result;

  switch( p_data->subseq ){
  case SEQ_MODESEL_INIT:  // ������
    ModeSelect_Init( p_wk, p_data, p_draw, HEAPID_WIFINOTE );

    ModeSelectSeq_Init( p_wk, p_data, p_draw );
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
        WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);
    p_data->subseq = SEQ_MODESEL_FADEINWAIT;
    break;

  case SEQ_MODESEL_FADEINWAIT:
    if( WIPE_SYS_EndCheck() ){
      p_data->subseq = SEQ_MODESEL_MAIN;
    }
    break;

  case SEQ_MODESEL_INIT_NOFADE:
    ModeSelect_Init( p_wk, p_data, p_draw, HEAPID_WIFINOTE );
    ModeSelectSeq_Init( p_wk, p_data, p_draw );
    p_data->subseq = SEQ_MODESEL_MAIN;
    break;

  case SEQ_MODESEL_MAIN:
    BLINKPALANM_Main( p_draw->bpawk );
    result = ModeSelectSeq_Main( p_wk, p_data, p_draw );
    if( result == FALSE ){
      break;
    }
    if( p_wk->cursor == MODESEL_CUR_END ){
      SetCancelButtonAnime( p_draw );
    }else if( p_wk->cursor == MODESEL_CUR_END_X ){
      SetExitButtonAnime( p_draw );
    }else{
      p_wk->btn_anmSeq = 0;
    }
    p_data->subseq = SEQ_MODESEL_MAIN_ENDWAIT;
    break;

  case SEQ_MODESEL_MAIN_ENDWAIT:
    // �{�^���A�j��
    if( p_wk->cursor == MODESEL_CUR_END ){
      if( GFL_CLACT_WK_CheckAnmActive( p_draw->p_tb ) == TRUE ){
        break;
      }
    }else if( p_wk->cursor == MODESEL_CUR_END_X ){
      if( GFL_CLACT_WK_CheckAnmActive( p_draw->p_tbx ) == TRUE ){
        break;
      }
    }else{
      if( MainTopPageButtonDecedeAnime( p_wk ) == TRUE ){
        break;
      }
    }

    // ���̏�ԂɑJ�ڂ��đ��v���`�F�b�N
//    if( ModeSelect_StatusChengeCheck( p_wk, p_data ) == TRUE && !(GFL_UI_KEY_GetCont() & PAD_BUTTON_R) ){
    if( ModeSelect_StatusChengeCheck( p_wk, p_data ) == TRUE ){
      // �J�[�\���ʒu�ɑΉ������I���ݒ���s��
      ModeSelect_EndDataSet( p_wk, p_data );

      // �I���`�F�b�N
      if( p_wk->cursor == MODESEL_CUR_END ){
        p_data->subseq = SEQ_MODESEL_FADEOUT;
      }else if( p_wk->cursor == MODESEL_CUR_END_X ){
        p_data->subseq = SEQ_MODESEL_FADEOUT;
      }else if( p_wk->cursor == MODESEL_CUR_CODEIN ){
        p_data->subseq = SEQ_MODESEL_FADEOUT;
      }else{
        ModeSelect_DrawOff( p_wk, p_draw );
        ModeSelect_Exit( p_wk, p_data, p_draw );
        return WFNOTE_STRET_FINISH;
      }
    }else{
//      if( p_wk->cursor == MODESEL_CUR_CODEIN ){
        // ���b�Z�[�W��������
        // ���b�Z�[�W�I���҂�����
        ModeSelect_TalkMsgPrint( p_wk, p_draw, msg_wifi_note_30, heapID );
        p_data->subseq = SEQ_MODESEL_CODECHECK_RESLUTMSGWAIT;
//      }
    }
    break;
    // ���̂܂܏I������Ƃ�
  case SEQ_MODESEL_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
            WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);

    p_data->subseq ++;
    break;

  case SEQ_MODESEL_FADEOUTWAIT:
    if( WIPE_SYS_EndCheck() == TRUE ){
      ModeSelect_Exit( p_wk, p_data, p_draw );
      return WFNOTE_STRET_FINISH;
    }
    break;

    // �R�[�h���͉�ʂ���̕��A
  case SEQ_MODESEL_CODECHECK_INIT:
    ModeSelect_Init( p_wk, p_data, p_draw, HEAPID_WIFINOTE );

    ModeSelectSeq_Init( p_wk, p_data, p_draw );
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
        WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);
    p_data->subseq = SEQ_MODESEL_CODECHECK_FADEINWAIT;
    break;

  case SEQ_MODESEL_CODECHECK_FADEINWAIT:
    if( WIPE_SYS_EndCheck() ){
      u64 code;
      MYSTATUS* p_friend;
      // ���b�Z�[�W�̕\��
      // ���O��ݒ�
      p_friend = MyStatus_AllocWork( heapID );
      MyStatus_SetMyNameFromString(p_friend, p_data->codein.p_name );
      WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, p_friend );
      GFL_HEAP_FreeMemory( p_friend );

      // �F�B�R�[�h�ݒ�
      code = WFNOTE_STRBUF_GetNumber(p_data->codein.p_code, &result);
      if( result ){ // �����ɕϊ��ł�����
        Draw_FriendCodeSetWordset( p_draw, code );

        ModeSelect_TalkMsgPrint( p_wk, p_draw, msg_wifi_note_26, heapID );
        p_data->subseq = SEQ_MODESEL_CODECHECK_MSGWAIT;
      }else{

        // �R�[�h���Ԉ���Ă���ƕ\��
        ModeSelect_TalkMsgPrint( p_wk, p_draw, msg_wifi_note_27, heapID );
        p_data->subseq = SEQ_MODESEL_CODECHECK_RESLUTMSGWAIT;  // �Ƃ������R�[�h�Ⴄ�\��
      }
    }
    break;

  case SEQ_MODESEL_CODECHECK_MSGWAIT:   //�o�^�m�F���b�Z�[�WON
    if( ModeSelect_TalkMsgEndCheck( p_wk, p_data , p_draw ) == FALSE ){
      break;
    }

    // �͂��������E�B���h�E�\��
    p_wk->listWork = Draw_YesNoWinSet( p_wk->ynlist, &p_wk->listRes, TRUE, heapID );
    p_data->subseq = SEQ_MODESEL_CODECHECK_MSGYESNOWAIT;
    break;

  case SEQ_MODESEL_CODECHECK_MSGYESNOWAIT:  //�͂��������I��҂�
    {
      int ret;
      u32 set_ret;
      static const u8 msgdata[ RCODE_NEWFRIEND_SET_NUM ] = {
        0,
        msg_wifi_note_27,
        msg_wifi_note_31,
      };

      ret = Draw_YesNoWinMain( p_wk->listWork );

      switch(ret){
      case 0:
        p_data->key_mode = GFL_UI_CheckTouchOrKey();
        // �o�^��Ƃ����Č��ʂ�\��
        set_ret = Data_NewFriendDataSet( p_data, p_data->codein.p_code, p_data->codein.p_name );
        if( set_ret == RCODE_NEWFRIEND_SET_OK ){
          // ���������̂ł��̂܂܏I���
          ModeSelect_TalkMsgOff( p_wk, p_draw );
          ResetTopPageButton( p_wk );
          p_data->subseq = SEQ_MODESEL_MAIN;
        }else{
          ModeSelect_TalkMsgPrint( p_wk, p_draw, msgdata[set_ret], heapID );
          p_data->subseq = SEQ_MODESEL_CODECHECK_RESLUTMSGWAIT;  // �Ƃ������R�[�h�Ⴄ�\��
        }
        break;
      case 1:
        p_data->key_mode = GFL_UI_CheckTouchOrKey();
        // ���b�Z�[�W�������ďI���
        ModeSelect_TalkMsgOff( p_wk, p_draw );
        ResetTopPageButton( p_wk );
        p_data->subseq = SEQ_MODESEL_MAIN;
        break;
      default:
        break;
      }
    }
    break;

  case SEQ_MODESEL_CODECHECK_RESLUTMSGWAIT: //�I�����ʕ\��
    if( ModeSelect_TalkMsgEndCheck( p_wk,p_data ,p_draw) == FALSE ){
      break;
    }
    if( CheckUITrg( p_data ) == TRUE ){
      ModeSelect_TalkMsgOff( p_wk, p_draw );
      ResetTopPageButton( p_wk );
      p_data->subseq = SEQ_MODESEL_MAIN;
    }
    break;
  }

  return WFNOTE_STRET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�I�����  ���[�N�j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void ModeSelect_Exit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  ModeSelect_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��֌W�̏�����
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void ModeSelect_DrawInit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  STRBUF* p_str;
  STRBUF* p_tmp;
  int i;

  // �r�b�g�}�b�v�E�B���h�E�쐬
  Sub_BmpWinAdd(
      p_draw,
      &p_wk->msg, DFRM_SCRMSG,
      BMPL_MODESEL_MSG_X, BMPL_MODESEL_MSG_Y,
      BMPL_MODESEL_MSG_SIZX, BMPL_MODESEL_MSG_SIZY,
      BGPLT_M_MSGFONT, BMPL_MODESEL_MSGCGX , 0 );

  Sub_BmpWinAdd(
      p_draw,
      &p_wk->talk, DFRM_MSG,
      BMPL_MODESEL_TALK_X, BMPL_MODESEL_TALK_Y,
      BMPL_MODESEL_TALK_SIZX, BMPL_MODESEL_TALK_SIZY,
      BGPLT_M_MSGFONT, BMPL_MODESEL_TALKCGX , 0 );

  // ��b�E�B���h�E�p���b�Z�[�W�o�b�t�@�m��
  p_wk->p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  // ���b�Z�[�W��������
  p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  // �����̖��O�����[�h�Z�b�g�ɐݒ�
  WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, GAMEDATA_GetMyStatus(p_data->pGameData) );

  for( i=0; i<3; i++ ){
    u32 siz;
    GFL_MSG_GetString( p_draw->p_msgman, msg_top_menu01+i, p_tmp );
    WORDSET_ExpandStr( p_draw->p_wordset, p_str, p_tmp );

    siz = PRINTSYS_GetStrWidth( p_str, p_draw->fontHandle , 0 );

    PRINTTOOL_PrintColor(
      &p_wk->msg, p_draw->printQue, 
      ((BMPL_MODESEL_MSG_SIZX*8)-siz)/2,
      8+(40*i),
      p_str, p_draw->fontHandle, WFNOTE_COL_BLACK, PRINTTOOL_MODE_LEFT );
  }

  PutBmpWin( p_wk->msg.win );
/*
  GFL_BMPWIN_TransVramCharacter( p_wk->msg );
  GFL_BG_LoadScreenV_Req( DFRM_SCRMSG );
*/

  GFL_STR_DeleteBuffer( p_str );
  GFL_STR_DeleteBuffer( p_tmp );

  // �J�[�\���X�N���[���ǂݍ���
  p_wk->p_scrnbuff = GFL_ARCHDL_UTIL_LoadScreen( p_draw->p_handle,
      NARC_wifi_note_friend_top_d_NSCR, FALSE, &p_wk->p_scrn, heapID );

  // �͂��E������
  p_wk->ynlist[0].str      = GFL_MSG_CreateString( p_draw->p_msgman, msg_wifi_note_yes );
  p_wk->ynlist[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  p_wk->ynlist[0].type     = APP_TASKMENU_WIN_TYPE_NORMAL;
  p_wk->ynlist[1].str      = GFL_MSG_CreateString( p_draw->p_msgman, msg_wifi_note_no );
  p_wk->ynlist[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  p_wk->ynlist[1].type     = APP_TASKMENU_WIN_TYPE_NORMAL;

  p_wk->listRes = APP_TASKMENU_RES_Create(
                    DFRM_MSG, BGPLT_M_SBOX, p_draw->fontHandle, p_draw->printQue, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��֌W�̔j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void ModeSelect_DrawExit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  // PrintUtil�̓]���f�[�^��������
  InitPrintUtil( p_draw );
  SetPrintUtil( p_draw, &p_draw->title );

  // �͂��E�������j��
  APP_TASKMENU_RES_Delete( p_wk->listRes );
  GFL_STR_DeleteBuffer( p_wk->ynlist[0].str );
  GFL_STR_DeleteBuffer( p_wk->ynlist[1].str );

  // ��b������o�b�t�@�j��
  GFL_STR_DeleteBuffer( p_wk->p_str );

  // �r�b�g�}�b�v�j��
  GFL_BMPWIN_Delete( p_wk->talk.win );
  GFL_BMPWIN_Delete( p_wk->msg.win );

  // �X�N���[���f�[�^�j��
  GFL_HEAP_FreeMemory( p_wk->p_scrnbuff );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�P���X������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^���[�N
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void ModeSelectSeq_Init( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  // �^�C�g���\��
  Draw_BmpTitleWrite( p_draw, &p_draw->title, msg_wifi_note_01 );
//  GFL_BG_LoadScreenV_Req( DFRM_MSG );

  // ������`��
//  PutBmpWin( p_wk->msg.win );
  // �{�^���\��
  GFL_BG_WriteScreen( DFRM_SCROLL,
      p_wk->p_scrn->rawData, 0, 0,
      p_wk->p_scrn->screenWidth/8, p_wk->p_scrn->screenHeight/8 );
  // �X�N���[�����W�����ɖ߂�
  GFL_BG_SetScroll( DFRM_SCROLL, GFL_BG_SCROLL_X_SET, 0 );
  GFL_BG_SetScroll( DFRM_SCROLL, GFL_BG_SCROLL_Y_SET, 0 );

  // �J�[�\���A�N�^�[�\���ݒ�
//  Draw_CursorActorSet( p_draw, FALSE , ACTANM_CUR_MSEL );
//  Draw_CursorActorPriSet( p_draw, 1, 0 );

  // �J�[�\���ʒu�̃p���b�g��ݒ�
//  ModeSelect_CursorScrnPalChange( p_wk, p_draw );
//  ModeSelect_CursorDraw( p_wk, p_draw );
  if( p_data->key_mode == GFL_APP_END_KEY ){
    SetTopPageButtonActive( p_draw, p_wk->cursor );
  }else{
    GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
  }

  // BG�ʓ]��
//  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�I����� �L�[����
 *
 *  @param  p_wk    ���[�h�I����ʃ��[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`�惏�[�N
 *
 *  @retval TRUE  ������I��
 *  @retval FALSE ���������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
/*
static BOOL ModeSelectInput_Key( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  BOOL move = FALSE;

  if( GFL_UI_KEY_GetTrg() & GFL_PAD_BUTTON_KTST_CHG){
    p_data->key_mode = GFL_APP_END_KEY;
  }

  // �I���`�F�b�N
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
    ModeSelect_CursorScrnPalChange( p_wk, p_draw );
    PMSND_PlaySE(WIFINOTE_DECIDE_SE);
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    return TRUE;
  }

  // �L�����Z���`�F�b�N
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
    PMSND_PlaySE(WIFINOTE_BS_SE);
    p_wk->cursor = MODESEL_CUR_END; // �I���ɂ���
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    return TRUE;
  }

  // �ړ��`�F�b�N
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
    if( p_wk->cursor > 0 ){
      p_wk->cursor--;
      move = TRUE;
    }
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
    if( p_wk->cursor < MODESEL_CUR_MYCODE ){
      p_wk->cursor++;
      move = TRUE;
    }
  }

  // �ړ������特�ƕ`��X�V
  if( move == TRUE ){
    PMSND_PlaySE( WIFINOTE_MOVE_SE );
//    ModeSelect_CursorScrnPalChange( p_wk, p_draw );
    ModeSelect_CursorDraw( p_wk, p_draw );
    GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
  }
  return FALSE;
}
*/
static BOOL ModeSelectInput_Key( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  // �L�����Z���`�F�b�N
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
    PMSND_PlaySE(WIFINOTE_BS_SE);
    SetTopPageButtonPassive( p_draw, p_wk->cursor );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    p_data->key_mode = GFL_APP_END_KEY;
    p_wk->cursor = MODESEL_CUR_END; // �I���ɂ���
    return TRUE;
  }

  // �I���`�F�b�N
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
    PMSND_PlaySE(WIFINOTE_EXIT);
    SetTopPageButtonPassive( p_draw, p_wk->cursor );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    p_data->key_mode = GFL_APP_END_KEY;
    p_wk->cursor = MODESEL_CUR_END_X; // �I���ɂ���
    return TRUE;
  }

  // �L�[���̓��[�h�֕ύX
  if( p_data->key_mode == GFL_APP_END_TOUCH ){
    if( GFL_UI_KEY_GetTrg() & GFL_PAD_BUTTON_KTST_CHG){
      PMSND_PlaySE( WIFINOTE_MOVE_SE );
      ChangeTopPageButton( p_draw, p_wk->cursor, p_wk->cursor );
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      p_data->key_mode = GFL_APP_END_KEY;
    }
    return FALSE;
  }

  // �I���`�F�b�N
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
    PMSND_PlaySE(WIFINOTE_DECIDE_SE);
    BLINKPALANM_InitAnimeCount( p_draw->bpawk );
    return TRUE;
  }

  // �ړ��`�F�b�N
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
    if( p_wk->cursor > 0 ){
      PMSND_PlaySE( WIFINOTE_MOVE_SE );
      ChangeTopPageButton( p_draw, p_wk->cursor-1, p_wk->cursor );
      p_wk->cursor--;
      return FALSE;
    }
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
    if( p_wk->cursor < MODESEL_CUR_MYCODE ){
      PMSND_PlaySE( WIFINOTE_MOVE_SE );
      ChangeTopPageButton( p_draw, p_wk->cursor+1, p_wk->cursor );
      p_wk->cursor++;
      return FALSE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�I����� �^�b�`����
 *
 *  @param  p_wk    ���[�h�I����ʃ��[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`�惏�[�N
 *
 *  @retval TRUE  ������I��
 *  @retval FALSE ���������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
/*
static BOOL ModeSelectInput_Touch( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw ,BOOL* touch_f)
{
  int ret;

  static const GFL_UI_TP_HITTBL TpRect[] = {
  //  {0,191,0,255}, ty,by,lx,rx
    {TP_MODESEL_BTN_Y01,TP_MODESEL_BTN_Y01+TP_MODESEL_BTN_H,TP_MODESEL_BTN_X,TP_MODESEL_BTN_X+TP_MODESEL_BTN_W},
    {TP_MODESEL_BTN_Y02,TP_MODESEL_BTN_Y02+TP_MODESEL_BTN_H,TP_MODESEL_BTN_X,TP_MODESEL_BTN_X+TP_MODESEL_BTN_W},
    {TP_MODESEL_BTN_Y03,TP_MODESEL_BTN_Y03+TP_MODESEL_BTN_H,TP_MODESEL_BTN_X,TP_MODESEL_BTN_X+TP_MODESEL_BTN_W},
    {GFL_UI_TP_HIT_END,0,0,0},
  };
  ret = GFL_UI_TP_HitTrg(TpRect);

  if(ret == GFL_UI_TP_HIT_NONE){
    return FALSE;
  }
  *touch_f = TRUE;
  p_data->key_mode = GFL_APP_END_TOUCH;
  GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

  // �I���`�F�b�N
  p_wk->cursor = MODESEL_CUR_FLIST+ret;
  PMSND_PlaySE(WIFINOTE_DECIDE_SE);
  ModeSelect_CursorScrnPalChange( p_wk, p_draw );
  ModeSelect_CursorDraw( p_wk, p_draw );
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
  return TRUE;
}
*/
static int ModeSelectInput_Touch( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  int ret;

  static const GFL_UI_TP_HITTBL TpRect[] = {
  //  {0,191,0,255}, ty,by,lx,rx
    {TP_MODESEL_BTN_Y01,TP_MODESEL_BTN_Y01+TP_MODESEL_BTN_H,TP_MODESEL_BTN_X,TP_MODESEL_BTN_X+TP_MODESEL_BTN_W},
    {TP_MODESEL_BTN_Y02,TP_MODESEL_BTN_Y02+TP_MODESEL_BTN_H,TP_MODESEL_BTN_X,TP_MODESEL_BTN_X+TP_MODESEL_BTN_W},
    {TP_MODESEL_BTN_Y03,TP_MODESEL_BTN_Y03+TP_MODESEL_BTN_H,TP_MODESEL_BTN_X,TP_MODESEL_BTN_X+TP_MODESEL_BTN_W},
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_07,TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1},
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_06,TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1},
    {GFL_UI_TP_HIT_END,0,0,0},
  };
  ret = GFL_UI_TP_HitTrg(TpRect);

  if( ret != GFL_UI_TP_HIT_NONE ){
    if( ret == MODESEL_CUR_END ){
      PMSND_PlaySE( WIFINOTE_BS_SE );
      SetTopPageButtonPassive( p_draw, p_wk->cursor );
    }else if( ret == MODESEL_CUR_END_X ){
      PMSND_PlaySE( WIFINOTE_EXIT );
      SetTopPageButtonPassive( p_draw, p_wk->cursor );
    }else{
      PMSND_PlaySE( WIFINOTE_DECIDE_SE );
      ChangeTopPageButton( p_draw, ret, p_wk->cursor );
    }
    p_wk->cursor = MODESEL_CUR_FLIST+ret;
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    p_data->key_mode = GFL_APP_END_TOUCH;
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�h�I����ʃ��C��
 *
 *  @param  p_wk    ���[�h�I����ʃ��[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`�惏�[�N
 *
 *  @retval TRUE  ������I��
 *  @retval FALSE ���������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL ModeSelectSeq_Main( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
/*
  BOOL ret = FALSE,touch_f = FALSE;

  ret = ModeSelectInput_Touch( p_wk, p_data, p_draw ,&touch_f);

  if(touch_f){
    p_data->key_mode = GFL_APP_END_TOUCH;
    return ret;
  }
  return ModeSelectInput_Key( p_wk, p_data, p_draw );
*/
  int ret;

  ret = ModeSelectInput_Touch( p_wk, p_data, p_draw );
  if( ret != GFL_UI_TP_HIT_NONE ){
    return TRUE;
  }
  return ModeSelectInput_Key( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�[�\���\��
 *
 *  @param  p_wk  ���[�N
 *  @param  p_draw  �`��V�X�e��
 */
//-----------------------------------------------------------------------------
/*
static void ModeSelect_CursorDraw( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw )
{
  GFL_CLACTPOS pos;
  pos.x = 128;
  pos.y = 40*p_wk->cursor+48;
  GFL_CLACT_WK_SetPos( p_draw->p_cur, &pos , CLSYS_DEFREND_MAIN );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �J�[�\���̈ʒu�ɍ��킹�ăX�N���[���̃p���b�g
 *
 *  @param  p_wk  ���[�N
 *  @param  p_draw  �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void ModeSelect_CursorScrnPalChange( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw )
{
  int i;
  u8 pal;

  for( i=0; i<MODESEL_CUR_NUM; i++ ){
    if( p_wk->cursor == i ){
      pal = MODESEL_CUR_PAL_ON;
    }else{
      pal = MODESEL_CUR_PAL_OFF;
    }
    GFL_BG_ChangeScreenPalette( DFRM_SCROLL,
      2, 4+5*i,
      MODESEL_CUR_CSIZX, MODESEL_CUR_CSIZY, pal );
  }
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�[�\���ʒu�ɑΉ������I���f�[�^��ݒ肷��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 */
//-----------------------------------------------------------------------------
static void ModeSelect_EndDataSet( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data )
{
  static const u8 reqstatus[ MODESEL_CUR_NUM ] = {
    STATUS_FRIENDLIST,
    STATUS_CODEIN,
    STATUS_MYCODE,
    STATUS_END,
    STATUS_END_X,
  };
  Data_SetReqStatus( p_data, reqstatus[p_wk->cursor], 0 );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\������S������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`�惏�[�N
 */
//-----------------------------------------------------------------------------
static void ModeSelect_DrawOff( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw )
{
  // �^�C�g��
  Draw_BmpTitleOff( p_draw );

  // SCROLL��
  GFL_BG_FillScreen( DFRM_SCROLL, 0, 0, 0, 32, 24 , GFL_BG_SCRWRT_PALNL );
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );

  // SCRMSG��
  GFL_BG_FillScreen( DFRM_SCRMSG, 0, 0, 0, 32, 24 , GFL_BG_SCRWRT_PALNL );
  GFL_BG_LoadScreenV_Req( DFRM_SCRMSG );

  // msg
  GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->msg.win );
//  GF_BGL_BmpWinOffVReq(&p_wk->msg);

  // �J�[�\���A�N�^�[�\���ݒ�
//  Draw_CursorActorSet( p_draw, FALSE , ACTANM_NULL );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�v�����g
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`�惏�[�N
 *  @param  msgidx    ���b�Z�[�WIDX
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void ModeSelect_TalkMsgPrint( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw, u32 msgidx, HEAPID heapID )
{
  STRBUF* p_tmp;

  // �^�X�N�����~
  if( p_draw->printHandleMsg != NULL ){
    PRINTSYS_PrintStreamDelete( p_draw->printHandleMsg );
    p_draw->printHandleMsg = NULL;
  }

  p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  // �N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->talk.win), 15 );

  GFL_MSG_GetString( p_draw->p_msgman, msgidx, p_tmp );
  WORDSET_ExpandStr( p_draw->p_wordset, p_wk->p_str, p_tmp );
  p_draw->printHandleMsg = PRINTSYS_PrintStream(p_wk->talk.win, 0, 0,
      p_wk->p_str, p_draw->fontHandle , MSGSPEED_GetWait(),
      p_draw->msgTcblSys, 10 , p_draw->heapID , PRINTSYS_LSB_GetB(WFNOTE_COL_BLACK) );

  BmpWinFrame_Write( p_wk->talk.win, WINDOW_TRANS_OFF,
      BMPL_WIN_CGX_TALK, BGPLT_M_TALKWIN );

  GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->talk.win );

  GFL_STR_DeleteBuffer( p_tmp );

  // ��ʂ��p�b�V�u��
  SetDispActive( p_draw, TRUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�̏I���҂�
 *
 *  @param  cp_wk ���[�N
 *
 *  @retval TRUE  �I��
 *  @retval FALSE �܂�
 */
//-----------------------------------------------------------------------------
static BOOL ModeSelect_TalkMsgEndCheck( const WFNOTE_MODESELECT* cp_wk, WFNOTE_DATA* p_data , WFNOTE_DRAW *p_draw )
{
  if( PRINTSYS_PrintStreamGetState( p_draw->printHandleMsg ) == PRINTSTREAM_STATE_DONE ){
//    p_data->key_mode = MsgPrintGetKeyTouchStatus();
    p_data->key_mode = GFL_APP_END_KEY;
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ��b�E�C���h�E������
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void ModeSelect_TalkMsgOff( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw )
{
  SetDispActive( p_draw, FALSE );
  BmpWinFrame_Clear( p_wk->talk.win, WINDOW_TRANS_ON );
  GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->talk.win );
//  GF_BGL_BmpWinOffVReq(&p_wk->talk);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�[�\���ʒu�̃X�e�[�^�X�ɑJ�ڂ��Ă����v���`�F�b�N����
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *
 *  @retval TRUE  ���v
 *  @retval FALSE ����
 */
//-----------------------------------------------------------------------------
static BOOL ModeSelect_StatusChengeCheck( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data )
{
  int  i;
  WIFI_LIST* p_list;

  if( p_wk->cursor == MODESEL_CUR_CODEIN ){
    // ���������邩�`�F�b�N
    p_list = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
    for(i=0;i<WIFILIST_FRIEND_MAX;i++){
      if( !WifiList_IsFriendData( p_list, i ) ){  //
        return TRUE;
      }
    }
  }else{

    // �F�B�o�^�ȊO�͖�������OK
    return TRUE;
  }

  return FALSE;
}



//-------------------------------------
/// �F�B�f�[�^�\���֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�f�[�^�\�����[�N  ������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FList_Init( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  FList_DrawInit( p_wk, p_data, p_draw,  heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�f�[�^�\�����[�N  ���C��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 *
 *  @retval WFNOTE_STRET_CONTINUE,  // �Â�
 *  @retval WFNOTE_STRET_FINISH,  // ��Ƃ͏I�����
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET FList_Main( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  u32 result;
  WIFI_LIST* p_wifilist;
  u32 listidx;

  switch( p_data->subseq ){
  case SEQ_FLIST_INIT_PAGEINIT: // �\������y�[�W������������Ƃ�
    p_wk->page = 0;
    p_wk->lastpage = 0;
    p_wk->pos = 0;
    p_wk->lastpos = 0;
  case SEQ_FLIST_INIT:  // �ȑO�̏�Ԃŕ\��
    FList_Init( p_wk, p_data, p_draw, HEAPID_WIFINOTE );
    // ���X�g�f�[�^�쐬
    Data_FrIdxMake( p_data );

    // �`�揉����
    FList_DrawOn( p_wk, p_data, p_draw, heapID );

    p_data->subseq  = SEQ_FLIST_MAIN;
    break;

    // ���X�g����
  case SEQ_FLIST_MAIN:
    BLINKPALANM_Main( p_draw->bpawk );
    result = FListSeq_Main( p_wk, p_data,  p_draw );
    switch( result ){
    case RCODE_FLIST_SEQMAIN_NONE:
      break;

    case RCODE_FLIST_SEQMAIN_PCHG_LEFT:// �y�[�W�ύX
      FList_ScrSeqChange( p_wk, p_data, WF_COMMON_LEFT );
      break;

    case RCODE_FLIST_SEQMAIN_PCHG_RIGHT:// �y�[�W�ύX
      FList_ScrSeqChange( p_wk, p_data, WF_COMMON_RIGHT );
      break;

    case RCODE_FLIST_SEQMAIN_CANCEL:  // �߂�
      p_data->subseq = SEQ_FLIST_END;
      break;

    case RCODE_FLIST_SEQMAIN_EXIT:  // �I��
      p_data->subseq = SEQ_FLIST_END_X;
      break;

    case RCODE_FLIST_SEQMAIN_SELECTPL:  // �l��I������
//      p_wk->wait = 8;
      p_data->subseq = SEQ_FLIST_MENUINIT;
      break;
    }
    break;

    // �X�N���[������
  case SEQ_FLIST_SCRLLINIT:
    FListSeq_ScrollInit( p_wk, p_data, p_draw, heapID );
    p_data->subseq = SEQ_FLIST_SCRLL;
    break;

  case SEQ_FLIST_SCRLL:

    result = FListSeq_ScrollMain( p_wk, p_data, p_draw, heapID );
    if( result == TRUE ){
      p_data->subseq = SEQ_FLIST_MAIN;
    }
    break;

  case SEQ_FLIST_MENUINIT:  //�@���j���[�I��
    if( MainListPageButtonDecedeAnime( p_wk ) == TRUE ){
      break;
    }
/*
    if(p_wk->wait-- > 0){ //������ƃE�F�C�g
      break;
    }
    p_wk->wait = 0;
*/
    // �I���������X�g�C���f�b�N�X���f�[�^�\���̂ɐݒ�
    Data_SELECT_ListIdxSet( p_data,
        FList_FRIdxGet( p_wk ) );

    FListSeq_MenuInit( p_wk, p_data, p_draw, heapID );
    p_data->subseq = SEQ_FLIST_MENUWAIT;
    break;

  case SEQ_FLIST_MENUWAIT:  // ���j���[�I��҂�
    p_data->subseq = FListSeq_MenuWait( p_wk, p_data, p_draw, heapID );
    break;

  case SEQ_FLIST_INFOINIT:    // �ڂ��������
    p_wk->wait = FLIST_INFO_WAIT;
    p_data->subseq = SEQ_FLIST_INFO;
    break;

  case SEQ_FLIST_INFO:    // �ڂ��������
    // ��ʐ؂�ւ������ꂢ�Ɍ�����悤�ɂ������wait
    p_wk->wait --;
    if( p_wk->wait <= 0 ){
      FList_DrawOff( p_wk, p_data, p_draw );
      Data_SetReqStatus( p_data, STATUS_FRIENDINFO, 0 );

      FList_Exit( p_wk, p_data, p_draw );
      return WFNOTE_STRET_FINISH;
    }
    break;

  case SEQ_FLIST_INFORET: // �ڂ������邩��߂��Ă����Ƃ�
    // �y�[�W��ݒ�
    p_wk->page = p_data->select_listidx / FLIST_PAGE_FRIEND_NUM;
    p_wk->pos = p_data->select_listidx % FLIST_PAGE_FRIEND_NUM;
    p_data->subseq = SEQ_FLIST_INIT;  // ��������
    break;

  case SEQ_FLIST_NAMECHG_INIT:  // ���O�ύX
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
        WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);

    // �����񏉊����f�[�^��ݒ�
    listidx = FList_FRIdxGet( p_wk );
    p_wifilist = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
    Data_NAMEIN_INIT_Set( p_data, WifiList_GetFriendNamePtr( p_wifilist, p_data->idx.fridx[listidx] ) );
    p_data->subseq = SEQ_FLIST_NAMECHG_WIPE;
    break;

  case SEQ_FLIST_NAMECHG_WIPE:  // ���O�ύX
    if( WIPE_SYS_EndCheck() ){
      // ���O���͉�ʂ�
      FList_DrawOff( p_wk, p_data, p_draw );
      Data_SetReqStatus( p_data, STATUS_CODEIN, SEQ_CODEIN_NAMEINONLY );

      FList_Exit( p_wk, p_data, p_draw );
      return WFNOTE_STRET_FINISH;
    }
    break;

  case SEQ_FLIST_NAMECHG_WAIT:  // ���O�ύX
    // ���O��ύX
    listidx = FList_FRIdxGet( p_wk );
    p_wifilist = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
    WifiList_SetFriendName( p_wifilist, p_data->idx.fridx[listidx], p_data->codein.p_name );
  case SEQ_FLIST_NAMECHG_WAITWIPE:  // ���O�ύX �������Ȃ��ŕ��A
    // �`�揉����
    FList_Init( p_wk, p_data, p_draw, HEAPID_WIFINOTE );
    FList_DrawOn( p_wk, p_data, p_draw, heapID );
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
        WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);
    p_data->subseq = SEQ_FLIST_NAMECHG_WAITWIPEWAIT;
    break;

  case SEQ_FLIST_NAMECHG_WAITWIPEWAIT:  // ���O�ύX
    if( WIPE_SYS_EndCheck() ){
      p_data->subseq = SEQ_FLIST_MAIN;
    }
    break;

  case SEQ_FLIST_DELETE_INIT: // �j��
    FListSeq_DeleteInit( p_wk, p_data, p_draw, heapID );
    p_data->subseq = SEQ_FLIST_DELETE_YESNODRAW;
    break;

  case SEQ_FLIST_DELETE_YESNODRAW:  // �j�� YESNO�E�B���h�E�\��
    result = FListSeq_DeleteYesNoDraw( p_wk, p_data, p_draw, heapID );
    if( result == TRUE ){
      p_data->subseq = SEQ_FLIST_DELETE_WAIT;
    }
    break;

  case SEQ_FLIST_DELETE_WAIT: // �j��YESNO���܂��Ă��珈��
    result = Draw_YesNoWinMain( p_wk->listWork );
    switch(result){
    case 0:
      p_data->key_mode = GFL_UI_CheckTouchOrKey();
      FList_TalkMsgOff( p_wk, p_draw );
      p_data->subseq =  SEQ_FLIST_DELETE_ANMSTART;
      break;
    case 1:
      p_data->key_mode = GFL_UI_CheckTouchOrKey();
      FList_TalkMsgOff( p_wk, p_draw );
      p_data->subseq =  SEQ_FLIST_MAIN;
      break;
    default:
      break;
    }
    break;

  case SEQ_FLIST_DELETE_ANMSTART: // �j��YESNO���܂��Ă��珈��
    FList_DeleteAnmStart( p_wk , p_draw );
    p_data->subseq = SEQ_FLIST_DELETE_ANMWAIT;
    break;

  case SEQ_FLIST_DELETE_ANMWAIT:  // �j��YESNO���܂��Ă��珈��
    if( FList_DeleteAnmMain( p_wk, p_draw ) == TRUE ){
      // �j������
      p_wifilist = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
      listidx = FList_FRIdxGet( p_wk );
      WifiList_ResetData( p_wifilist, p_data->idx.fridx[ listidx ] );
      // �F�B�C���f�b�N�X����蒼��
      // �F�B�̕\������������
      // ���X�g�f�[�^�쐬
      Data_FrIdxMake( p_data );

      // �`�揉����
      FList_DrawPage( p_wk, p_data, p_draw, p_wk->page, p_wk->pos, WFNOTE_DRAWAREA_MAIN, heapID );

      // ���b�Z�[�W�������Ė����o��
//      Draw_YazirushiSetDrawFlag( p_draw, TRUE );
      p_data->subseq =  SEQ_FLIST_DELETE_END;
    }
    break;

  case SEQ_FLIST_DELETE_END:
    FList_DeleteAnmOff( p_wk, p_draw );
    p_data->subseq =  SEQ_FLIST_MAIN;
    break;

  case SEQ_FLIST_CODE_INIT: // �R�[�h�؂�ւ�
    result = FListSeq_CodeInit( p_wk, p_data, p_draw, heapID );
    if( result == TRUE ){
      p_data->subseq = SEQ_FLIST_CODE_WAIT;
    }else{
//      Draw_YazirushiSetDrawFlag( p_draw, TRUE );  // ���\��
      p_data->subseq = SEQ_FLIST_MAIN;
    }
    break;

  case SEQ_FLIST_CODE_WAIT: // �����L�[�������Ă��炤
    if( FList_TalkMsgEndCheck( p_wk,p_data,p_draw ) == FALSE ){
      break;
    }

    if( CheckUITrg( p_data ) == TRUE ){
//      Draw_YazirushiSetDrawFlag( p_draw, TRUE );  // ���\��
      p_data->subseq = SEQ_FLIST_MAIN;
      FList_TalkMsgOff( p_wk, p_draw );
    }
    break;

  case SEQ_FLIST_END:
    if( GFL_CLACT_WK_CheckAnmActive( p_draw->p_tb ) == TRUE ){
      break;
    }
    FList_DrawOff( p_wk, p_data, p_draw );
    Data_SetReqStatus( p_data, STATUS_MODE_SELECT, SEQ_MODESEL_INIT_NOFADE );

    FList_Exit( p_wk, p_data, p_draw );
    return WFNOTE_STRET_FINISH;

  case SEQ_FLIST_END_X:
    if( GFL_CLACT_WK_CheckAnmActive( p_draw->p_tbx ) == TRUE ){
      break;
    }
    p_data->subseq = SEQ_FLIST_END_FADEOUT;
    break;

  case SEQ_FLIST_END_FADEOUT:
    WIPE_SYS_Start(
      WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
      WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID );
    p_data->subseq = SEQ_FLIST_END_FADEOUT_WAIT;
    break;

  case SEQ_FLIST_END_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() ){
      Data_SetReqStatus( p_data, STATUS_END_X, 0 );
      FList_Exit( p_wk, p_data, p_draw );
      return WFNOTE_STRET_FINISH;
    }
    break;
  }
  return WFNOTE_STRET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�f�[�^�\�����[�N  �j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void FList_Exit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  FList_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`�揉����
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FList_DrawInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  int i, j;
  STRBUF* p_str;
  BMPMENULIST_HEADER list_h;

  static const u16 sc_TEXTCGX[ 2 ] = {
    BMPL_FLIST_TEXT_CGX0,
    BMPL_FLIST_TEXT_CGX1,
  };

  // �r�b�g�}�b�v
  for( i=0; i < 2; i++ )
  {
    Sub_BmpWinAdd(
        p_draw,
        &p_wk->drawBmpWin[i],
        DFRM_SCRMSG,
        BMPL_FLIST_TEXT_OFSX+MATH_ABS(DATA_ScrnArea[i].scrn_x),
        BMPL_FLIST_TEXT_OFSY+MATH_ABS(DATA_ScrnArea[i].scrn_y),
        BMPL_FLIST_TEXT_SIZX, BMPL_FLIST_TEXT_SIZY,
        BGPLT_M_MSGFONT, sc_TEXTCGX[i] , 0 );
  }

  //  �`��G���A�쐬
  for( i=0; i < WFNOTE_DRAWAREA_NUM; i++ )
  {
    FListDrawArea_Init( &p_wk->drawdata[ i ], p_draw,
        &DATA_ScrnArea[i], sc_TEXTCGX[i], heapID );
  }
  p_wk->drawdata[ WFNOTE_DRAWAREA_MAIN ].text = &p_wk->drawBmpWin[0];
  p_wk->drawdata[ WFNOTE_DRAWAREA_RIGHT ].text = &p_wk->drawBmpWin[1];
  p_wk->drawdata[ WFNOTE_DRAWAREA_LEFT ].text = &p_wk->drawBmpWin[1];

  // �L�����N�^�f�[�^������
  p_wk->p_charsys = WF_2DC_SysInit( p_draw->cellUnitScroll, NULL, FLIST_2DCHAR_NUM, heapID );

  // ���j�I���L�����N�^��ǂݍ���
  WF_2DC_UnionResSet( p_wk->p_charsys, CLSYS_DRAW_MAIN,
      WF_2DC_MOVETURN, CLACT_PALNUM_WF2DC*0x20, heapID );

/*
  // �߂郁�b�Z�[�W�쐬
  Sub_BmpWinAdd( &p_wk->backmsg, DFRM_MSG,
      BMPL_FLIST_BKMSG_X, BMPL_FLIST_BKMSG_Y,
      BMPL_FLIST_BKMSG_SIZX, BMPL_FLIST_BKMSG_SIZY,
      BGPLT_M_MSGFONT, BMPL_FLIST_BKMSG_CGX, 0 );

  p_str = GFL_MSG_CreateString( p_draw->p_msgman, msg_flist_back );
  PRINTSYS_PrintQueColor( p_draw->printQue , GFL_BMPWIN_GetBmp(p_wk->backmsg), 0, 0, p_str, p_draw->fontHandle , WFNOTE_TCOL_BLACK );
  GFL_STR_DeleteBuffer( p_str );
*/

  // ���j���[�f�[�^�쐬
/*
  for( i=0; i<BMPL_FLIST_MENU_NUM; i++ ){
    p_wk->p_menulist[i] = BmpMenuWork_ListCreate( BMPL_FLIST_MENU_LISTNUM, heapID );
    for( j=0; j<BMPL_FLIST_MENU_LISTNUM; j++ ){
      BmpMenuWork_ListAddArchiveString( p_wk->p_menulist[i], p_draw->p_msgman,
          DATA_FListMenuTbl[i][j].strid, DATA_FListMenuTbl[i][j].param , heapID );
    }
  }
*/
  for( i=0; i<4; i++ ){
    p_wk->list1[i].str      = GFL_MSG_CreateString( p_draw->p_msgman, DATA_FListMenuTbl[0][i].strid );
    p_wk->list1[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
    p_wk->list1[i].type     = APP_TASKMENU_WIN_TYPE_NORMAL;

    p_wk->list2[i].str      = GFL_MSG_CreateString( p_draw->p_msgman, DATA_FListMenuTbl[1][i].strid );
    p_wk->list2[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
    p_wk->list2[i].type     = APP_TASKMENU_WIN_TYPE_NORMAL;
  }
  p_wk->list1[3].type = APP_TASKMENU_WIN_TYPE_RETURN;
  p_wk->list2[3].type = APP_TASKMENU_WIN_TYPE_RETURN;
  // �͂��E������
  p_wk->ynlist[0].str      = GFL_MSG_CreateString( p_draw->p_msgman, msg_wifi_note_yes );
  p_wk->ynlist[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  p_wk->ynlist[0].type     = APP_TASKMENU_WIN_TYPE_NORMAL;
  p_wk->ynlist[1].str      = GFL_MSG_CreateString( p_draw->p_msgman, msg_wifi_note_no );
  p_wk->ynlist[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  p_wk->ynlist[1].type     = APP_TASKMENU_WIN_TYPE_NORMAL;

  p_wk->listRes = APP_TASKMENU_RES_Create(
                    DFRM_MSG, BGPLT_M_SBOX, p_draw->fontHandle, p_draw->printQue, heapID );

  // ��b�E�B���h�E�쐬
  Sub_BmpWinAdd(
      p_draw,
      &p_wk->talk, DFRM_MSG,
      BMPL_FLIST_TALK_X, BMPL_FLIST_TALK_Y,
      BMPL_FLIST_TALK_SIZX, BMPL_FLIST_TALK_SIZY,
      BGPLT_M_MSGFONT, BMPL_FLIST_TALK_CGX ,15);

  p_wk->p_talkstr = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  // �����S���A�N�^�[�̍쐬
  {
    GFL_CLWK_DATA clearadd = {
      0,0,
      ACTANM_CLEAR,
      0,0,
    };
    p_wk->p_clearact = GFL_CLACT_WK_Create(
          p_draw->cellUnit,
          p_draw->chrRes[CHR_RES_WOR],
          p_draw->palRes[PAL_RES_WOR],
          p_draw->celRes[CEL_RES_WOR],
          &clearadd, CLSYS_DEFREND_MAIN,
          heapID );
    // �Ƃ肠������\��
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clearact, FALSE );
  }
  // �y�[�W�ԍ��A�N�^�[�̍쐬
  {
    GFL_CLWK_DATA clearadd = {
      56, 180,
      ACTANM_PAGE_NUM1,
      0,0,
    };
    p_wk->p_pageact = GFL_CLACT_WK_Create(
          p_draw->cellUnit,
          p_draw->chrRes[CHR_RES_WOR],
          p_draw->palRes[PAL_RES_WOR],
          p_draw->celRes[CEL_RES_WOR],
          &clearadd, CLSYS_DEFREND_MAIN,
          heapID );
    // �Ƃ肠������\��
//    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clearact, FALSE );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void FList_DrawExit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  int i;

  // PrintUtil�̓]���f�[�^��������
  InitPrintUtil( p_draw );
  SetPrintUtil( p_draw, &p_draw->title );

  // �y�[�W�ԍ��A�N�^�[
  GFL_CLACT_WK_Remove( p_wk->p_pageact );
  // �����S���A�N�^�[
  GFL_CLACT_WK_Remove( p_wk->p_clearact );

  // ��b�E�B���h�E
  GFL_BMPWIN_Delete( p_wk->talk.win );
  GFL_STR_DeleteBuffer( p_wk->p_talkstr );

  // ���j���[�f�[�^�j��
  APP_TASKMENU_RES_Delete( p_wk->listRes );
  GFL_STR_DeleteBuffer( p_wk->ynlist[0].str );
  GFL_STR_DeleteBuffer( p_wk->ynlist[1].str );
  for( i=0; i<4; i++ ){
//    BmpMenuWork_ListDelete( p_wk->p_menulist[i] );
    GFL_STR_DeleteBuffer( p_wk->list1[i].str );
    GFL_STR_DeleteBuffer( p_wk->list2[i].str );
  }

  // ���b�Z�[�W�ʔj��
//  GFL_BMPWIN_Delete( p_wk->backmsg );

  // ���j�I���L�����N�^��ǂݍ���
  WF_2DC_UnionResDel( p_wk->p_charsys );

  // �L�����N�^�f�[�^������
  WF_2DC_SysExit( p_wk->p_charsys );


  //  �`��G���A�쐬
  for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){
    FListDrawArea_Exit( &p_wk->drawdata[ i ], p_draw );
  }

  // �r�b�g�}�b�v
  for( i=0; i < 2; i++ )
  {
    GFL_BMPWIN_Delete( p_wk->drawBmpWin[i].win );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��J�n�֐�
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FList_DrawOn( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  int i;

  // �^�C�g���`��
  Draw_BmpTitleWrite( p_draw, &p_draw->title, msg_flist_title );

/*
  // �߂�`��
  GFL_BG_WriteScreenExpand( DFRM_BACK,
      FLIST_BACK_X, FLIST_BACK_Y, FLIST_BACK_SIZX, FLIST_BACK_SIZY,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      FLIST_SCRBACK_X, FLIST_SCRBACK_Y,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
*/
/*
  // �߂�̃��b�Z�[�W�`��
  GFL_BMPWIN_TransVramCharacter( p_wk->backmsg );
  GFL_BMPWIN_MakeScreen( p_wk->backmsg );
*/

  // BG�ʓ]��
  GFL_BG_LoadScreenV_Req( DFRM_BACK );
  // ���`��
//  Draw_YazirushiSetDrawFlag( p_draw, TRUE );

  //  �ŏ��̃y�[�W�`��
  FList_DrawPage( p_wk, p_data, p_draw, p_wk->page, p_wk->pos, WFNOTE_DRAWAREA_MAIN, heapID );

  // �J�[�\���A�N�^�[�\���ݒ�
//  Draw_CursorActorSet( p_draw, TRUE , ACTANM_CUR_LIST );
//  Draw_CursorActorPriSet( p_draw, FLIST_CURSOR_BGPRI, FLIST_CURSOR_PRI );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\��OFF
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void FList_DrawOff( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  int i;

  // �^�C�g��OFF
  Draw_BmpTitleOff( p_draw );

  // �߂��\��
//  GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->backmsg );
//  GF_BGL_BmpWinOffVReq( &p_wk->backmsg );

  // �`��G���A�̕\��OFF
  for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){
    FListDrawArea_DrawOff( &p_wk->drawdata[i],
        p_draw, &p_draw->scrn );
  }
  // �J�[�\���A�N�^�[�\���ݒ�
//  Draw_CursorActorSet( p_draw, FALSE , ACTANM_NULL );
//  Draw_CursorActorPriSet( p_draw, WFNOTE_CURSOR_BGPRI, WFNOTE_CURSOR_PRI );

  // ���`��OFF
//  Draw_YazirushiSetDrawFlag( p_draw, FALSE );

/*
  // �w�i�����ɖ߂�
  // ���ǂ�
  GFL_BG_WriteScreenExpand( DFRM_BACK,
      FLIST_BACK_X,
      FLIST_BACK_Y,
      FLIST_BACK_SIZX, FLIST_BACK_SIZY,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      FLIST_CL_SCRBACK_X, FLIST_CL_SCRBACK_Y,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
  // ������ԂԂ�
  GFL_BG_WriteScreenExpand( DFRM_BACK,
      FLIST_SHIORI_X, FLIST_SHIORI_Y,
      FLIST_SHIORI_SIZX, FLIST_SHIORI_SIZY,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      FLIST_CL_SHIORISCRN_X, FLIST_CL_SHIORISCRN_Y,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
*/
  GFL_BG_LoadScreenV_Req( DFRM_BACK );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B���X�g�\��  ���C���L�[���͎擾
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *
 *  @retval RCODE_FLIST_SEQMAIN_NONE,
 *  @retval RCODE_FLIST_SEQMAIN_PCHG_,// �y�[�W�ύX
 *  @retval RCODE_FLIST_SEQMAIN_CANCEL, // �߂�
 *  @retval RCODE_FLIST_SEQMAIN_SELECTPL, // �l��I������
 *
 */
//-----------------------------------------------------------------------------
static u32 FListInputMain_Exe( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw,u8 code )
{
  u16 lastpos;
  BOOL move;
  BOOL pagechang;
  WF_COMMON_WAY way;

  if(code == FLIST_MAIN_EXE_CANCEL){
    PMSND_PlaySE( WIFINOTE_BS_SE );
    SetCancelButtonAnime( p_draw );
    return RCODE_FLIST_SEQMAIN_CANCEL;
  }else if(code == FLIST_MAIN_EXE_EXIT){
    PMSND_PlaySE( WIFINOTE_EXIT );
    SetExitButtonAnime( p_draw );
    return RCODE_FLIST_SEQMAIN_EXIT;
  }else if( code == FLIST_MAIN_EXE_DECIDE){
    // �L�����Z���`�F�b�N
    if( p_wk->pos == FLIST_CURSORDATA_BACK ){
      PMSND_PlaySE( WIFINOTE_BS_SE );
      SetCancelButtonAnime( p_draw );
      return RCODE_FLIST_SEQMAIN_CANCEL;
    }
    // �I���`�F�b�N
    if( p_wk->pos == FLIST_CURSORDATA_EXIT ){
      PMSND_PlaySE( WIFINOTE_EXIT );
      SetExitButtonAnime( p_draw );
      return RCODE_FLIST_SEQMAIN_EXIT;
    }

    // ���̈ʒu�ɗF�B�����邩�`�F�b�N
    if( p_data->idx.friendnum <= p_wk->pos + (p_wk->page*FLIST_PAGE_FRIEND_NUM) ){
      return RCODE_FLIST_SEQMAIN_NONE;  // ���Ȃ�
    }

    // �F�B�̏���\������
    PMSND_PlaySE(WIFINOTE_DECIDE_SE);
//    FListDrawArea_ActiveListWrite( &p_wk->drawdata[WFNOTE_DRAWAREA_MAIN], p_draw, p_wk->pos ,TRUE );
    p_wk->btn_anmSeq = 0;
    return RCODE_FLIST_SEQMAIN_SELECTPL;
  }else if( code == FLIST_MAIN_EXE_LEFT ){
//    Draw_YazirushiAnmChg( p_draw, ACT_YAZI_L, ACT_YAZIANM_ACTIVE );
    return RCODE_FLIST_SEQMAIN_PCHG_LEFT;
  }else if( code == FLIST_MAIN_EXE_RIGHT ){
//    Draw_YazirushiAnmChg( p_draw, ACT_YAZI_R, ACT_YAZIANM_ACTIVE );
    return RCODE_FLIST_SEQMAIN_PCHG_RIGHT;
  }
  return RCODE_FLIST_SEQMAIN_NONE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B���X�g�\��  ���C���L�[����
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *
 *  @retval RCODE_FLIST_SEQMAIN_NONE,
 *  @retval RCODE_FLIST_SEQMAIN_PCHG_,// �y�[�W�ύX
 *  @retval RCODE_FLIST_SEQMAIN_CANCEL, // �߂�
 *  @retval RCODE_FLIST_SEQMAIN_SELECTPL, // �l��I������
 *
 */
//-----------------------------------------------------------------------------
static u32 FListInputMain_Key( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  u16 lastpos;
  BOOL move,pagechg,way;

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    p_data->key_mode = GFL_APP_END_KEY;
    return FListInputMain_Exe( p_wk, p_data, p_draw, FLIST_MAIN_EXE_CANCEL );
  }

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    p_data->key_mode = GFL_APP_END_KEY;
    return FListInputMain_Exe( p_wk, p_data, p_draw, FLIST_MAIN_EXE_EXIT );
  }

  // �L�[���̓��[�h�֕ύX
  if( p_data->key_mode == GFL_APP_END_TOUCH ){
    if( GFL_UI_KEY_GetTrg() & GFL_PAD_BUTTON_KTST_CHG){
      PMSND_PlaySE( WIFINOTE_MOVE_SE );
      ChangeListPageButton( p_draw, p_wk->pos, p_wk->pos );
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      p_data->key_mode = GFL_APP_END_KEY;
    }
    return RCODE_FLIST_SEQMAIN_NONE;
  }

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
//    PMSND_PlaySE( WIFINOTE_MOVE_SE );
    return FListInputMain_Exe( p_wk, p_data, p_draw, FLIST_MAIN_EXE_DECIDE );
  }

  lastpos = p_wk->pos;  // �O�̍��W��ۑ����Ă��� ���ǂ�@���烊�X�g�ɃJ�[�\�����ړ�����Ƃ��Ɏg�p����
  move = FALSE;
  pagechg = FALSE;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
    if( p_wk->pos == 0 ){
      p_wk->pos = 3;
    }else if( p_wk->pos == 4 ){
      p_wk->pos = 7;
    }else{
      p_wk->pos--;
    }
    move = TRUE;
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
    if( p_wk->pos == 3 ){
      p_wk->pos = 0;
    }else if( p_wk->pos == 7 ){
      p_wk->pos = 4;
    }else{
      p_wk->pos++;
    }
    move = TRUE;
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
    if( p_wk->pos < FLIST_PAGE_FRIEND_HALF ){
      p_wk->pos += FLIST_PAGE_FRIEND_HALF;
      move = TRUE;
      pagechg = TRUE;
      way = 0;
    }else{
      p_wk->pos -= FLIST_PAGE_FRIEND_HALF;
    }
    move = TRUE;
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
    if( p_wk->pos < FLIST_PAGE_FRIEND_HALF ){
      p_wk->pos += FLIST_PAGE_FRIEND_HALF;
    }else{
      p_wk->pos -= FLIST_PAGE_FRIEND_HALF;
      pagechg = TRUE;
      way = 1;
    }
    move = TRUE;
  }else if( p_data->lr_key_enable ){
    if( p_data->lr_key_enable && (GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L) ) {
      pagechg = TRUE;
      way = 0;
    }else if( p_data->lr_key_enable && (GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R) ) {
      pagechg = TRUE;
      way = 1;
    }
  }

  if( pagechg == TRUE ){
    // �X�N���[�������ֈڍs����
    if(way){  //�E
      p_wk->lastpage = p_wk->page;
      p_wk->page = (p_wk->page + 1) % FLIST_PAGE_MAX;
      return FListInputMain_Exe( p_wk, p_data, p_draw,FLIST_MAIN_EXE_RIGHT);
    }
    p_wk->lastpage = p_wk->page;
    p_wk->page -= 1;
    if( p_wk->page < 0 ){
      p_wk->page += FLIST_PAGE_MAX;
    }
    return FListInputMain_Exe( p_wk, p_data, p_draw,FLIST_MAIN_EXE_LEFT);
  }else if( move == TRUE ){
    // �J�[�\����������
    PMSND_PlaySE( WIFINOTE_MOVE_SE );
    ChangeListPageButton( p_draw, p_wk->pos, lastpos );
//    FListDrawArea_CursorWrite( &p_wk->drawdata[WFNOTE_DRAWAREA_MAIN], p_draw, p_wk->pos );
    p_wk->lastpos = lastpos;
  }
  return RCODE_FLIST_SEQMAIN_NONE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B���X�g�\��  ���C���^�b�`����
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *
 *  @retval RCODE_FLIST_SEQMAIN_NONE,
 *  @retval RCODE_FLIST_SEQMAIN_PCHG_,// �y�[�W�ύX
 *  @retval RCODE_FLIST_SEQMAIN_CANCEL, // �߂�
 *  @retval RCODE_FLIST_SEQMAIN_SELECTPL, // �l��I������
 *
 */
//-----------------------------------------------------------------------------
static u32 FListInputMain_Touch( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw ,BOOL* touch_f )
{
  u32 ret;
  s8  dif;

  static const GFL_UI_TP_HITTBL TpRect[] = {
  //  {0,191,0,255}, ty,by,lx,rx
    {TP_FLIST_BTN_Y01,TP_FLIST_BTN_Y01+TP_FLIST_BTN_H,TP_FLIST_BTN_X01,TP_FLIST_BTN_X01+TP_FLIST_BTN_W},
    {TP_FLIST_BTN_Y02,TP_FLIST_BTN_Y02+TP_FLIST_BTN_H,TP_FLIST_BTN_X01,TP_FLIST_BTN_X01+TP_FLIST_BTN_W},
    {TP_FLIST_BTN_Y03,TP_FLIST_BTN_Y03+TP_FLIST_BTN_H,TP_FLIST_BTN_X01,TP_FLIST_BTN_X01+TP_FLIST_BTN_W},
    {TP_FLIST_BTN_Y04,TP_FLIST_BTN_Y04+TP_FLIST_BTN_H,TP_FLIST_BTN_X01,TP_FLIST_BTN_X01+TP_FLIST_BTN_W},
    {TP_FLIST_BTN_Y01,TP_FLIST_BTN_Y01+TP_FLIST_BTN_H,TP_FLIST_BTN_X02,TP_FLIST_BTN_X02+TP_FLIST_BTN_W},
    {TP_FLIST_BTN_Y02,TP_FLIST_BTN_Y02+TP_FLIST_BTN_H,TP_FLIST_BTN_X02,TP_FLIST_BTN_X02+TP_FLIST_BTN_W},
    {TP_FLIST_BTN_Y03,TP_FLIST_BTN_Y03+TP_FLIST_BTN_H,TP_FLIST_BTN_X02,TP_FLIST_BTN_X02+TP_FLIST_BTN_W},
    {TP_FLIST_BTN_Y04,TP_FLIST_BTN_Y04+TP_FLIST_BTN_H,TP_FLIST_BTN_X02,TP_FLIST_BTN_X02+TP_FLIST_BTN_W},
    // 8
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_07,TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1},
    // 9
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_06,TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1},
    // 10
    {TP_FLIST_BM_Y,TP_FLIST_BM_Y+TP_FLIST_BM_H-1,TP_FLIST_BM01_X,TP_FLIST_BM01_X+TP_FLIST_BM_W-1},
    {TP_FLIST_BM_Y,TP_FLIST_BM_Y+TP_FLIST_BM_H-1,TP_FLIST_BM02_X,TP_FLIST_BM02_X+TP_FLIST_BM_W-1},
    {TP_FLIST_BM_Y,TP_FLIST_BM_Y+TP_FLIST_BM_H-1,TP_FLIST_BM03_X,TP_FLIST_BM03_X+TP_FLIST_BM_W-1},
    {TP_FLIST_BM_Y,TP_FLIST_BM_Y+TP_FLIST_BM_H-1,TP_FLIST_BM04_X,TP_FLIST_BM04_X+TP_FLIST_BM_W-1},

    {GFL_UI_TP_HIT_END,0,0,0},
  };
  ret = GFL_UI_TP_HitTrg(TpRect);

  if(ret == GFL_UI_TP_HIT_NONE){
    return RCODE_FLIST_SEQMAIN_NONE;
  }
  dif = 0;
  if(ret <= 7){
    *touch_f = TRUE;

    PMSND_PlaySE( WIFINOTE_MOVE_SE );
    SetListPageButtonPassive( p_draw, p_wk->pos );
    p_wk->pos = ret;
//    FListDrawArea_CursorWrite( &p_wk->drawdata[WFNOTE_DRAWAREA_MAIN], p_draw, p_wk->pos );
    return FListInputMain_Exe( p_wk, p_data, p_draw, FLIST_MAIN_EXE_DECIDE );
  }else if(ret == 8){
    *touch_f = TRUE;
    p_wk->pos = ret;
    return FListInputMain_Exe( p_wk, p_data, p_draw, FLIST_MAIN_EXE_DECIDE );
  }else if(ret == 9){
    *touch_f = TRUE;
    p_wk->pos = ret;
    return FListInputMain_Exe( p_wk, p_data, p_draw, FLIST_MAIN_EXE_DECIDE );
  }else{
    dif = (ret - 10) - p_wk->page;
  }
  if(dif == 0){
    return RCODE_FLIST_SEQMAIN_NONE;
  }

  *touch_f = TRUE;
  p_wk->lastpage = p_wk->page;
  p_wk->page += dif;
  if(dif >= 0){
    p_wk->page = p_wk->page % FLIST_PAGE_MAX;
    return FListInputMain_Exe( p_wk, p_data, p_draw,FLIST_MAIN_EXE_RIGHT);
  }else{
    if( p_wk->page < 0 ){
      p_wk->page += FLIST_PAGE_MAX;
    }
    return FListInputMain_Exe( p_wk, p_data, p_draw,FLIST_MAIN_EXE_LEFT);
  }
  return RCODE_FLIST_SEQMAIN_NONE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B���X�g�\��  ���C��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *
 *  @retval RCODE_FLIST_SEQMAIN_NONE,
 *  @retval RCODE_FLIST_SEQMAIN_PCHG_,// �y�[�W�ύX
 *  @retval RCODE_FLIST_SEQMAIN_CANCEL, // �߂�
 *  @retval RCODE_FLIST_SEQMAIN_SELECTPL, // �l��I������
 *
 */
//-----------------------------------------------------------------------------
static u32 FListSeq_Main( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  u32 ret;
  BOOL touch_f = FALSE;

  ret = FListInputMain_Touch( p_wk, p_data, p_draw, &touch_f);
  if(touch_f){
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    p_data->key_mode = GFL_APP_END_TOUCH;
    return ret;
  }
  return FListInputMain_Key( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���V�[�P���X������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FListSeq_ScrollInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  u16 draw_area;

  if( p_wk->way == WF_COMMON_LEFT ){
    draw_area = WFNOTE_DRAWAREA_LEFT;
  }else{
    draw_area = WFNOTE_DRAWAREA_RIGHT;
  }

  // �X�N���[�������ɏ����L��
  FList_DrawPage( p_wk, p_data, p_draw, p_wk->page, p_wk->pos, draw_area, heapID );

  // �J�[�\��������
  SetListPageButtonPassive( p_draw, p_wk->pos );
//  FListDrawArea_CursorWrite( &p_wk->drawdata[ WFNOTE_DRAWAREA_MAIN ], p_draw, FLIST_CURSORDATA_OFF );

  // �����Ƃ߂�
//  Draw_YazirushiSetAnmFlag( p_draw, FALSE );

  // �X�N���[����
  PMSND_PlaySE( WIFINOTE_SCRLL_SE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���V�[�P���X  ���C��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 *
 *  @retval TRUE  �I��
 *  @retval FALSE �r��
 */
//-----------------------------------------------------------------------------
static BOOL FListSeq_ScrollMain( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  u32 draw_area;
  s32 scrll_x;
  GFL_CLACTPOS surfacePos;
  u32 idx;
  static void (*pSHIORIFUNC[ 4 ])( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 ) = {
    FList_DrawShioriEff00,
    NULL,
    FList_DrawShioriEff01,
    NULL
  };

  // �J�E���^���I������炷�ׂĂ�`�悵�ďI���
  if( p_wk->count >= FLIST_SCROLL_COUNT ){

    // �X�N���[�����W�����ɖ߂�
    GFL_BG_SetScrollReq( DFRM_SCROLL, GFL_BG_SCROLL_X_SET, 0 );
    GFL_BG_SetScrollReq( DFRM_SCRMSG, GFL_BG_SCROLL_X_SET, 0 );
    // OAM�ɂ����f
    surfacePos.x = DATA_ScrollSurfaceRect.lefttop_x;
    surfacePos.y = DATA_ScrollSurfaceRect.lefttop_y;
    GFL_CLACT_USERREND_SetSurfacePos( p_draw->scrollRender,0, &surfacePos );

    // ��Ɨp�O���t�B�b�N������
    if( p_wk->way == WF_COMMON_LEFT ){
      draw_area = WFNOTE_DRAWAREA_LEFT;
    }else{
      draw_area = WFNOTE_DRAWAREA_RIGHT;
    }
    FListDrawArea_DrawOff( &p_wk->drawdata[ draw_area ], p_draw, &p_draw->scrn );

    // ���C���ʂɕ\��
    FList_DrawPage( p_wk, p_data, p_draw, p_wk->page, p_wk->pos, WFNOTE_DRAWAREA_MAIN, heapID );

    // ��󓮍�J�n
//    Draw_YazirushiSetAnmFlag( p_draw, TRUE );
    return TRUE;
  }

  scrll_x = (p_wk->count * FLIST_SCROLL_SIZX) / FLIST_SCROLL_COUNT;

  if( p_wk->way == WF_COMMON_LEFT ){
    scrll_x = -scrll_x;
  }

  //  �X�N���[�����W�ݒ�
  GFL_BG_SetScrollReq( DFRM_SCROLL, GFL_BG_SCROLL_X_SET, scrll_x );
  GFL_BG_SetScrollReq( DFRM_SCRMSG, GFL_BG_SCROLL_X_SET, scrll_x );

  // OAM�ɂ����f
  surfacePos.x = DATA_ScrollSurfaceRect.lefttop_x;
  surfacePos.y = DATA_ScrollSurfaceRect.lefttop_y;
  surfacePos.x += scrll_x;
  GFL_CLACT_USERREND_SetSurfacePos( p_draw->scrollRender,0, &surfacePos );

  // �A�j���[�V����
  if( p_wk->count % (FLIST_SCROLL_COUNT/4) ){
    idx = p_wk->count / (FLIST_SCROLL_COUNT/4);
    if( pSHIORIFUNC[idx] ){
      pSHIORIFUNC[idx]( p_wk, p_draw, p_wk->page, p_wk->lastpage );
    }
  }

  // �J�E���g
  p_wk->count ++;
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[�\��  ������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FListSeq_MenuInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  BMPMENULIST_HEADER list_h;
  WIFI_LIST* p_wifilist;
  u32 sex;
  u32 listidx;

  // wifi���X�g���琫�ʂ��擾
  p_wifilist = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
  listidx = FList_FRIdxGet( p_wk );
  sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[ listidx ], WIFILIST_FRIEND_SEX );

//  FListDrawArea_ActiveListWrite( &p_wk->drawdata[WFNOTE_DRAWAREA_MAIN], p_draw, p_wk->pos ,FALSE );

/*
  {
#if USE_SEL_BOX
    SELBOX_HEADER head;

    MI_CpuClear8(&head,sizeof(SELBOX_HEADER));

    head.prm = DATA_SBoxHeadParamFList;
    if( sex == PM_NEUTRAL ){
      head.list = (const BMPLIST_DATA*)p_wk->p_menulist[ BMPL_FLIST_MENU_CODE ];
    }else{
      head.list = (const BMPLIST_DATA*)p_wk->p_menulist[ BMPL_FLIST_MENU_NML ];
    }
    head.count = SBOX_FLIST_SEL_CT;
    head.fontHandle = p_draw->fontHandle;

    p_wk->p_listwk = SelectBoxSet(p_draw->sbox,&(head),p_data->key_mode,
        SBOX_FLIST_PX,SBOX_FLIST_PY,SBOX_FLIST_W,0);
#endif
  }
*/
  {
    APP_TASKMENU_INITWORK wk;

    wk.heapId   = heapID;
    wk.itemNum  = 4;
    if( sex == PM_NEUTRAL ){
      wk.itemWork = p_wk->list2;
      p_wk->listType = 1;
    }else{
      wk.itemWork = p_wk->list1;
      p_wk->listType = 0;
    }
    wk.posType  = ATPT_RIGHT_DOWN;
    wk.charPosX = 32;
    wk.charPosY = 24;
    wk.w        = SBOX_FLIST_W;
    wk.h        = APP_TASKMENU_PLATE_HEIGHT;

    p_wk->listWork = APP_TASKMENU_OpenMenu( &wk, p_wk->listRes );
  }

  // ���b�Z�[�W�\��
  Draw_FriendNameSetWordset( p_draw, p_data, p_data->idx.fridx[ listidx ], heapID );
  FList_TalkMsgWrite( p_wk, p_draw, msg_wifi_note_13, heapID );
  // �₶�邵��\��
//  Draw_YazirushiSetDrawFlag( p_draw, FALSE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[�\��  �I��҂�
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 *
 *  @return ���ɐi��łق����V�[�P���X
 */
//-----------------------------------------------------------------------------
static u32 FListSeq_MenuWait( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  u32 ret;
  u32 ret_seq = SEQ_FLIST_MENUWAIT;


  // ���b�Z�[�W�I���҂�
  if( FList_TalkMsgEndCheck( p_wk,p_data,p_draw ) == FALSE ){
    return ret_seq;
  }
/*
#if USE_SEL_BOX
  ret = SelectBoxMain(p_wk->p_listwk);
#else
  ret = SBOX_SELECT_CANCEL;
#endif
*/
  APP_TASKMENU_UpdateMenu( p_wk->listWork );
  if( APP_TASKMENU_IsFinish( p_wk->listWork ) == TRUE ){
    ret = APP_TASKMENU_GetCursorPos( p_wk->listWork );
    APP_TASKMENU_CloseMenu( p_wk->listWork );
  }else{
   ret = SBOX_SELECT_NULL;
  }

  switch(ret){
  case SBOX_SELECT_NULL:
    return ret_seq;
  case 3:
  case SBOX_SELECT_CANCEL:
   //   PMSND_PlaySE(WIFINOTE_MENU_BS_SE);
    p_data->key_mode = GFL_UI_CheckTouchOrKey();
    ret_seq = SEQ_FLIST_MAIN;
    break;
  default:
  //    PMSND_PlaySE(WIFINOTE_MENU_DECIDE_SE);
    p_data->key_mode = GFL_UI_CheckTouchOrKey();
    ret_seq = DATA_FListMenuTbl[p_wk->listType][ret].param;
    break;
  }
/*
#if USE_SEL_BOX
  p_data->key_mode = SelectBoxGetKTStatus(p_wk->p_listwk);
  SelectBoxExit(p_wk->p_listwk);
#endif
*/

  // MAIN�ɂ��ǂ�̂Ŗ����o��
  if( ret_seq == SEQ_FLIST_MAIN ){
//    Draw_YazirushiSetDrawFlag( p_draw, TRUE );
    FList_TalkMsgOff( p_wk, p_draw );
  }
  else if( ret_seq == SEQ_FLIST_INFOINIT ||
       ret_seq == SEQ_FLIST_NAMECHG_INIT )
  {
    //��ʑJ�ڂȂ̂Ŋ��S����
    FList_TalkMsgOff( p_wk, p_draw );
  }
  else
  {
    FList_TalkMsgClear( p_wk );
  }
  return ret_seq;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �R�[�h�\��  ������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 *
 *  @retval TRUE  �\���ł���
 *  @retval FALSE �\���ł��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FListSeq_CodeInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  WIFI_LIST* p_wifilist;
  DWCFriendData* p_frienddata;
  u64 code;
  u32 listidx;

  p_wifilist = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
  listidx = FList_FRIdxGet( p_wk );
  p_frienddata = WifiList_GetDwcDataPtr( p_wifilist,
      p_data->idx.fridx[ listidx ] );
  code = DWC_GetFriendKey( p_frienddata );
  if(code!=0){
    // �R�[�h�\��
//    PMSND_PlaySE(WIFINOTE_DECIDE_SE);
    Draw_FriendCodeSetWordset( p_draw, code );
    Draw_FriendNameSetWordset( p_draw, p_data, p_data->idx.fridx[ listidx ], heapID );
    FList_TalkMsgWrite( p_wk, p_draw, msg_wifi_note_32, heapID );
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �j������  ������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FListSeq_DeleteInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  u32 listidx;

  listidx = FList_FRIdxGet( p_wk );
  Draw_FriendNameSetWordset( p_draw, p_data, p_data->idx.fridx[ listidx ], heapID );
  FList_TalkMsgWrite( p_wk, p_draw, msg_wifi_note_29, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  YesNo�E�B���h�E�̕\��
 *
 *  @param  p_wk  ���[�N
 *  @param  p_data  �f�[�^
 *  @param  p_draw  �`��V�X�e��
 *  @param  heapID  �q�[�vID
 *
 *  @retval TRUE  �I��
 *  @retval TRUE  �r��
 */
//-----------------------------------------------------------------------------
static BOOL FListSeq_DeleteYesNoDraw( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  if( FList_TalkMsgEndCheck( p_wk, p_data , p_draw ) == FALSE ){
    return FALSE;
  }

  p_wk->listWork = Draw_YesNoWinSet( p_wk->ynlist, &p_wk->listRes, FALSE, heapID );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �j�������@YesNo�I����j������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 *
 *  @retval YesNo�I���̖߂�l
 */
//-----------------------------------------------------------------------------
static u32 FListSeq_DeleteWait( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  return  TRUE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �y�[�W�`��֐�
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  page    �y�[�W
 *  @param  pos     �J�[�\���ʒu
 *  @param  draw_area �`��G���A
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FList_DrawPage( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 page, u32 pos, u32 draw_area, HEAPID heapID )
{
  // ���C���ʂւ̕`�悩�`�F�b�N
  if( WFNOTE_DRAWAREA_MAIN == draw_area ){
    // �y�[�W�ƃJ�[�\���ύX
    p_wk->page = page;
    p_wk->pos = pos;

    // �y�[�W�ɑΉ������������\��
//    FList_DrawShiori( p_wk, p_draw, page );
    ChangeListPageNumAnime( p_wk, page );
  }

  // �w�i��`�悵�Ȃ���
  FListDrawArea_BackWrite( &p_wk->drawdata[draw_area], p_draw, &p_draw->scrn );

  // �y�[�W�f�[�^�`��
  FListDrawArea_WritePlayer( &p_wk->drawdata[draw_area], p_data,
      p_draw, p_wk->p_charsys, &p_draw->scrn, &p_data->idx, page, heapID );

  // �J�[�\���`��
  ResetListPageButton( p_wk );
//  FListDrawArea_CursorWrite( &p_wk->drawdata[draw_area], p_draw, pos );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������̕\��
 *
 *  @param  p_wk  ���[�N
 *  @param  p_draw  �`��V�X�e��
 *  @param  page  �y�[�W��
 */
//-----------------------------------------------------------------------------
/*
static void FList_DrawShiori( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page )
{
  u32 x;

  // ��{���܂��`��
  GFL_BG_WriteScreenExpand( DFRM_BACK,
      FLIST_SHIORI_X, FLIST_SHIORI_Y,
      FLIST_SHIORI_SIZX, FLIST_SHIORI_SIZY,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      FLIST_SHIORISCRN_X, FLIST_SHIORISCRN_Y,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

  // ���̃y�[�W�̂������`��
  x = (FLIST_SHIORI_ONEX*page) + FLIST_SHIORI_OFSX;
  GFL_BG_WriteScreenExpand( DFRM_BACK,
      x+FLIST_SHIORI_X, FLIST_SHIORI_Y + FLIST_SHIORI_OFSY,
      FLIST_SHIORIPOS_SIZ, FLIST_SHIORIPOS_SIZ,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      x+FLIST_SHIORISCRN_POS_X, FLIST_SHIORISCRN_POS_Y,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

  // BG�ʓ]��
  GFL_BG_LoadScreenV_Req( DFRM_BACK );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  ������A�j���@�V�[���O
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �f�[�^
 *  @param  page0   �y�[�W�P
 *  @param  page1   �y�[�W�Q
 */
//-----------------------------------------------------------------------------
static void FList_DrawShioriEff00( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 )
{
/*
  u32 x;

  x = (FLIST_SHIORI_ONEX*page1) + FLIST_SHIORI_OFSX;
  GFL_BG_WriteScreenExpand( DFRM_BACK,
      x+FLIST_SHIORI_X, FLIST_SHIORI_Y + FLIST_SHIORI_OFSY,
      FLIST_SHIORIPOS_SIZ, FLIST_SHIORIPOS_SIZ,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      x+FLIST_EFF_SHIORISCRN_POS_X, FLIST_EFF_SHIORISCRN_POS_Y,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

  // BG�ʓ]��
  GFL_BG_LoadScreenV_Req( DFRM_BACK );
*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������A�j���@�V�[���P
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �f�[�^
 *  @param  page0   �y�[�W�P
 *  @param  page1   �y�[�W�Q
 */
//-----------------------------------------------------------------------------
static void FList_DrawShioriEff01( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 )
{
/*
  u32 x;

  // ��{���܂��`��
  GFL_BG_WriteScreenExpand( DFRM_BACK,
      FLIST_SHIORI_X, FLIST_SHIORI_Y,
      FLIST_SHIORI_SIZX, FLIST_SHIORI_SIZY,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      FLIST_SHIORISCRN_X, FLIST_SHIORISCRN_Y,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

  // ���̃y�[�W�̂������`��
  x = (FLIST_SHIORI_ONEX*page0) + FLIST_SHIORI_OFSX;
  GFL_BG_WriteScreenExpand( DFRM_BACK,
      x+FLIST_SHIORI_X, FLIST_SHIORI_Y + FLIST_SHIORI_OFSY,
      FLIST_SHIORIPOS_SIZ, FLIST_SHIORIPOS_SIZ,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      x, FLIST_SHIORISCRN_POS_Y,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

  // BG�ʓ]��
  GFL_BG_LoadScreenV_Req( DFRM_BACK );
*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���V�[�P���X�ɕύX����
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  way     ����
 */
//-----------------------------------------------------------------------------
static void FList_ScrSeqChange( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WF_COMMON_WAY way )
{
  p_data->subseq = SEQ_FLIST_SCRLLINIT;
  p_wk->way = way;
  p_wk->count = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�\��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  msgid   ���b�Z�[�WID
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FList_TalkMsgWrite( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 msgid, HEAPID heapID )
{
  STRBUF* p_tmp;

  if( p_draw->printHandleMsg != NULL ){
    PRINTSYS_PrintStreamDelete( p_draw->printHandleMsg );
    p_draw->printHandleMsg = NULL;
  }
  // �N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->talk.win), 15 );

  p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  GFL_MSG_GetString( p_draw->p_msgman, msgid, p_tmp );
  WORDSET_ExpandStr( p_draw->p_wordset, p_wk->p_talkstr, p_tmp );

  p_draw->printHandleMsg = PRINTSYS_PrintStream(p_wk->talk.win, 0, 0,
      p_wk->p_talkstr, p_draw->fontHandle , MSGSPEED_GetWait(),
      p_draw->msgTcblSys , 10 , p_draw->heapID , PRINTSYS_LSB_GetB(WFNOTE_COL_BLACK) );

  BmpWinFrame_Write( p_wk->talk.win, WINDOW_TRANS_OFF,
      BMPL_WIN_CGX_TALK, BGPLT_M_TALKWIN );

  GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->talk.win );

  GFL_STR_DeleteBuffer( p_tmp );

  SetListDispActive( p_wk, p_draw, TRUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�\���I���҂�
 *
 *  @param  cp_wk ���[�N
 *
 *  @retval TRUE  �I��
 *  @retval FALSE �r��
 */
//-----------------------------------------------------------------------------
static BOOL FList_TalkMsgEndCheck( const WFNOTE_FRIENDLIST* cp_wk, WFNOTE_DATA* p_data ,WFNOTE_DRAW* p_draw)
{
  if( PRINTSYS_PrintStreamGetState( p_draw->printHandleMsg ) == PRINTSTREAM_STATE_DONE ){
//    p_data->key_mode = MsgPrintGetKeyTouchStatus();
    p_data->key_mode = GFL_APP_END_KEY;
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W���\���ɂ���
 *
 *  @param  p_wk    ���[�N
 */
//-----------------------------------------------------------------------------
static void FList_TalkMsgOff( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW * p_draw )
{
  BmpWinFrame_Clear( p_wk->talk.win, WINDOW_TRANS_ON_V );
/*
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_wk->talk ), 0 );
  GFL_BMPWIN_TransVramCharacter( p_wk->talk );
*/

  PutListTitle( p_draw );
  ResetListPageButton( p_wk );

  SetListDispActive( p_wk, p_draw, FALSE );

  // �߂�̕�����������̂ŕ\��
//  GFL_BMPWIN_MakeScreen( p_wk->backmsg );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W������
 *
 *  @param  p_wk    ���[�N
 */
//-----------------------------------------------------------------------------
static void FList_TalkMsgClear( WFNOTE_FRIENDLIST* p_wk )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_wk->talk.win ), WFNOTE_FONT_COL_WHITE );
  GFL_BMPWIN_TransVramCharacter( p_wk->talk.win );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���ݑI�𒆂̗F�B���X�g�C���f�b�N�X���擾
 *
 *  @param  cp_wk   ���[�N
 *
 *  @return �F�B���X�g�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u32 FList_FRIdxGet( const WFNOTE_FRIENDLIST* cp_wk )
{
  u32 num;
  num = cp_wk->page * FLIST_PAGE_FRIEND_NUM;
  num += cp_wk->pos;
  return num;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���j���[�̂P�s�ړ����Ƃ̃R�[���o�b�N
 *
 *  @param  p_wk    ���[�N
 *  @param  param   �p�����[�^
 *  @param  mode    ���[�h
 */
//-----------------------------------------------------------------------------
static void FList_CB_MenuList( BMPMENULIST_WORK * p_wk, u32 param, u8 mode )
{
  if( mode == 0 ){
    PMSND_PlaySE(WIFINOTE_MENU_MOVE_SE);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���X�g�j���A�j��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FList_DeleteAnmStart( WFNOTE_FRIENDLIST* p_wk ,WFNOTE_DRAW* p_draw )
{
  GFL_CLACTPOS cellPos;
  u32 pos;


  // �\��ON
  GFL_CLACT_WK_SetDrawEnable( p_wk->p_clearact, TRUE );
//  Draw_CursorActorSet( p_draw, FALSE, ACTANM_NULL );

  // �A�j������
  GFL_CLACT_WK_SetAnmSeq( p_wk->p_clearact, ACTANM_CLEAR );
  GFL_CLACT_WK_ResetAnm( p_wk->p_clearact );
  p_wk->last_frame = 0;

  // �I�[�g�A�j���ݒ�
  GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clearact, TRUE );

  pos = p_wk->pos;

  // �|�W�V�����ɂ��킹�č��W��ݒ�
  cellPos.x = ((MATH_ABS(DATA_ScrnArea[ WFNOTE_DRAWAREA_MAIN ].scrn_x)+DATA_FListCursorData[pos].scrn_data.scrn_x) * 8);
  cellPos.y = ((MATH_ABS(DATA_ScrnArea[ WFNOTE_DRAWAREA_MAIN ].scrn_y)+DATA_FListCursorData[pos].scrn_data.scrn_y) * 8) + FLIST_CLEARACT_Y;
  GFL_CLACT_WK_SetPos( p_wk->p_clearact, &cellPos , CLSYS_DEFREND_MAIN );

  // �E�B���h�E�}�X�N�ݒ�ON
  G2_SetWndOBJInsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2, FALSE );
  G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|GX_WND_PLANEMASK_OBJ, TRUE );
  GX_SetVisibleWnd( GX_WNDMASK_OW );//*/

  // �����S����
  PMSND_PlaySE( WIFINOTE_CLEAN_SE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���X�g�j���A�j��  ���C��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static BOOL FList_DeleteAnmMain( WFNOTE_FRIENDLIST* p_wk ,WFNOTE_DRAW* p_draw)
{
  u32 frame;

  if( GFL_CLACT_WK_CheckAnmActive( p_wk->p_clearact ) == FALSE ){
    return TRUE;
  }

  // �A�j�����؂�ւ����������S����
  frame = GFL_CLACT_WK_GetAnmFrame( p_wk->p_clearact );
  if( (frame - p_wk->last_frame) >= 2 ){
    p_wk->last_frame = frame;
//    PMSND_PlaySE( WIFINOTE_CLEAN_SE );
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �A�j���\����Ԃ�����
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FList_DeleteAnmOff( WFNOTE_FRIENDLIST* p_wk ,WFNOTE_DRAW* p_draw )
{
  GX_SetVisibleWnd( GX_WNDMASK_NONE );
  GFL_CLACT_WK_SetDrawEnable( p_wk->p_clearact, FALSE );
  GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clearact, FALSE );
//  Draw_CursorActorSet( p_draw, TRUE, ACTANM_NULL );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��G���A���[�N  ������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  cp_area   �X�N���[���G���A���
 *  @param  text_cgx  �e�L�X�g�ʂ̂�����
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FListDrawArea_Init( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_area, u32 text_cgx, HEAPID heapID )
{
  // �X�N���[���f�[�^�ۑ�
  p_wk->scrn_area = *cp_area;
/*
  // �r�b�g�}�b�v
  Sub_BmpWinAdd( &p_wk->text,
      DFRM_SCRMSG,
      BMPL_FLIST_TEXT_OFSX+MATH_ABS(cp_area->scrn_x),
      BMPL_FLIST_TEXT_OFSY+MATH_ABS(cp_area->scrn_y),
      BMPL_FLIST_TEXT_SIZX, BMPL_FLIST_TEXT_SIZY,
      BGPLT_M_MSGFONT, text_cgx , 0 );
*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��G���A���[�N  �j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void FListDrawArea_Exit( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw )
{
//  GFL_BMPWIN_Delete( p_wk->text );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��G���A�ɔw�i�`��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  cp_scrn   �X�N���[���f�[�^
 */
//-----------------------------------------------------------------------------
static void FListDrawArea_BackWrite( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn )
{
  // �w�i�`��
  GFL_BG_WriteScreen( DFRM_SCROLL,
      cp_scrn->p_scrn[WFNOTE_SCRNDATA_BACK]->rawData,
      MATH_ABS(p_wk->scrn_area.scrn_x), MATH_ABS(p_wk->scrn_area.scrn_y),
      cp_scrn->p_scrn[WFNOTE_SCRNDATA_BACK]->screenWidth/8,
      cp_scrn->p_scrn[WFNOTE_SCRNDATA_BACK]->screenHeight/8 );

  // BG�ʓ]��
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );

}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�[�\���`��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  pos     �J�[�\���ʒu
 */
//-----------------------------------------------------------------------------
/*
static void FListDrawArea_CursorWrite( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 pos )
{
  int i;
  int pal;
  const WFNOTE_FLIST_CURSORDATA* dat;
  GFL_CLACTPOS cellPos;

  if(pos == FLIST_CURSORDATA_OFF){
    Draw_CursorActorSet( p_draw,FALSE, ACTANM_NULL );
    return;
  }
  dat = &DATA_FListCursorData[pos];
  cellPos.x = (dat->scrn_data.scrn_x+7)*8;
  cellPos.y = (dat->scrn_data.scrn_y+2)*8;
  GFL_CLACT_WK_SetPos(p_draw->p_cur,&cellPos,CLSYS_DEFREND_MAIN);
  Draw_CursorActorSet( p_draw,TRUE, ACTANM_NULL );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  ���X�g�A�N�e�B�u��ԕ`��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  pos     �J�[�\���ʒu
 */
//-----------------------------------------------------------------------------
/*
static void FListDrawArea_ActiveListWrite( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 pos ,BOOL active_f)
{
  int i;
  int pal;
  const WFNOTE_FLIST_CURSORDATA* dat;

  for( i=0; i<FLIST_CURSORDATA_NUM; i++ ){
    dat = &DATA_FListCursorData[i];

    if( active_f == TRUE && pos == i ){
      pal = dat->on_pal;
    }else{
      pal = dat->off_pal;
    }

    // �p���b�g�J���[�����킹��
    GFL_BG_ChangeScreenPalette(
        dat->bg_frame,
        MATH_ABS(p_wk->scrn_area.scrn_x) + dat->scrn_data.scrn_x,
        MATH_ABS(p_wk->scrn_area.scrn_y) + dat->scrn_data.scrn_y,
        dat->scrn_data.scrn_sizx,
        dat->scrn_data.scrn_sizy, pal );
  }

  // BG�ʓ]��
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
  GFL_BG_LoadScreenV_Req( DFRM_BACK );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[���\��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  p_charsys �L�����N�^�V�X�e��
 *  @param  cp_scrn   �X�N���[���V�X�e��
 *  @param  cp_idx    �C���f�b�N�X�f�[�^
 *  @param  page    �y�[�W
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FListDrawArea_WritePlayer( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, WF_2DCSYS* p_charsys, const WFNOTE_SCRNDATA* cp_scrn, const WFNOTE_IDXDATA* cp_idx, u32 page, HEAPID heapID )
{
  int i;
  int idx;
  WIFI_LIST* p_list;
  BOOL  set;

  p_list = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );

  // �N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->text->win), 0 );

  // �A�N�^�[���c���Ă�����͂�
  FListDrawArea_CharWkDel( p_wk, p_draw );

  // ���O�ƃA�N�^�[�̐���
  set = FALSE;
  idx = page * FLIST_PAGE_FRIEND_NUM;
  for( i=0; i<FLIST_PAGE_FRIEND_NUM; i++ ){
    if( cp_idx->friendnum > (idx+i) ){
      // �\������
      FListDrawArea_SetPlayer( p_wk, p_charsys, p_draw, cp_scrn, i, p_list, cp_idx->fridx[idx+i],  heapID );
      set = TRUE;
    }else{
      // �v���C���[�͕\�����Ȃ�
      FListDrawArea_SetNoPlayer( p_wk, p_draw, cp_scrn, i, heapID );
    }
  }
  if( set == FALSE ){
    GFL_BMPWIN_TransVramCharacter( p_wk->text->win );
  }
  PutBmpWin( p_wk->text->win );

  // ��������BG���X�V
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );

  // �e�L�X�g�ʕ\��
//  GFL_BMPWIN_MakeTransWindow_VBlank( *p_wk->text );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\��OFF
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  cp_scrn   �X�N���[���f�[�^
 */
//-----------------------------------------------------------------------------
static void FListDrawArea_DrawOff( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn )
{
  // �A�N�^�[�j��
  FListDrawArea_CharWkDel( p_wk, p_draw );

  // �\��OFF
  GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->text->win );
//  GF_BGL_BmpWinOffVReq( &p_wk->text );

  // �w�i�N���A
  GFL_BG_FillScreen( DFRM_SCROLL,
      0,
      MATH_ABS(p_wk->scrn_area.scrn_x), MATH_ABS(p_wk->scrn_area.scrn_y),
      p_wk->scrn_area.scrn_sizx,
      p_wk->scrn_area.scrn_sizy, 0 );

  // BG�ʓ]��
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );

  // �X�N���[���ʂ��N���A
  GFL_BG_FillScreen( DFRM_SCRMSG,
      0,
      MATH_ABS(p_wk->scrn_area.scrn_x), MATH_ABS(p_wk->scrn_area.scrn_y),
      p_wk->scrn_area.scrn_sizx,
      p_wk->scrn_area.scrn_sizy, 0 );
  GFL_BG_LoadScreenV_Req( DFRM_SCRMSG );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ��C�ɃL�����N�^���[�N��j������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void FListDrawArea_CharWkDel( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw )
{
  int i;

  for( i=0; i<FLIST_PAGE_FRIEND_NUM; i++ ){
    if( p_wk->p_clwk[ i ] ){
      WF_2DC_WkDel( p_wk->p_clwk[ i ] );
      p_wk->p_clwk[ i ] = NULL;
    }
    if( p_wk->p_hate[i] != NULL ){
      GFL_CLACT_WK_Remove( p_wk->p_hate[i] );
      p_wk->p_hate[i] = NULL;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[�f�[�^�\��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_charsys �L�����N�^�V�X�e��
 *  @param  p_draw    �`��V�X�e��
 *  @param  cp_scrn   �X�N���[���f�[�^
 *  @param  pos     �ʒu
 *  @param  p_list    WiFi���X�g
 *  @param  idx     WiFi���X�g���̃C���f�b�N�X
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FListDrawArea_SetPlayer( WFNOTE_FLIST_DRAWAREA* p_wk, WF_2DCSYS* p_charsys, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn, u32 pos, WIFI_LIST* p_list, u32 idx, HEAPID heapID )
{
  int sex;
  int view;
  WF_2DC_WKDATA adddata;
  STRBUF* p_str;
  int x, y;
  PRINTSYS_LSB sc_SEXCOL[3];
  sc_SEXCOL[0] = WFNOTE_COL_BLUE;
  sc_SEXCOL[1] = WFNOTE_COL_RED;
  sc_SEXCOL[2] = WFNOTE_COL_BLACK;

  view = WifiList_GetFriendInfo( p_list, idx, WIFILIST_FRIEND_UNION_GRA );
  sex = WifiList_GetFriendInfo( p_list, idx, WIFILIST_FRIEND_SEX );

  if( sex == PM_NEUTRAL ){
    // ���Ȃ��G��\��
    FListDrawArea_SetNoPlayer( p_wk, p_draw, cp_scrn, pos, heapID );
  }else{
    // �L�����N�^�\��
    adddata.x = ((p_wk->scrn_area.scrn_x+DATA_FListCursorData[pos].scrn_data.scrn_x) * 8) + FLIST_PLAYER_X;
    adddata.y = ((p_wk->scrn_area.scrn_y+DATA_FListCursorData[pos].scrn_data.scrn_y) * 8) + FLIST_PLAYER_Y;
    adddata.pri = FLIST_PLAYER_PRI;
    adddata.bgpri = FLIST_PLAYER_BGPRI;
    p_wk->p_clwk[ pos ] = WF_2DC_WkAdd( p_charsys, &adddata, view, heapID );
//    p_wk->p_clwk[ pos ] = WF_2DC_WkAdd( p_charsys, &adddata, 5, heapID );
    WF_2DC_WkPatAnmStart( p_wk->p_clwk[ pos ] , WF_2DC_ANMWAY , WF2DMAP_WAY_DOWN );
  }

  //  ���O�\��
  x = ((DATA_FListCursorData[pos].scrn_data.scrn_x) * 8) + FLIST_PLAYERNAME_X;
  y = ((DATA_FListCursorData[pos].scrn_data.scrn_y) * 8) + FLIST_PLAYERNAME_Y;
  p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  GFL_STR_SetStringCode( p_str, WifiList_GetFriendNamePtr(p_list,idx) );

//  PRINTSYS_PrintQueColor( p_draw->printQue , GFL_BMPWIN_GetBmp(*p_wk->text), x, y, p_str, p_draw->fontHandle , sc_SEXCOL[sex] );
  PRINTTOOL_PrintColor(
    p_wk->text, p_draw->printQue, x, y, p_str, p_draw->fontHandle , sc_SEXCOL[sex], PRINTTOOL_MODE_LEFT );

  GFL_STR_DeleteBuffer( p_str );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[�̂��Ȃ���Ԃ�\��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  cp_scrn   �X�N���[���f�[�^
 *  @param  pos     �ʒu
 */
//-----------------------------------------------------------------------------
static void FListDrawArea_SetNoPlayer( WFNOTE_FLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn, u32 pos, HEAPID heapID )
{
  GFL_CLWK_DATA dat;

  dat.pos_x = ((p_wk->scrn_area.scrn_x+DATA_FListCursorData[pos].scrn_data.scrn_x) * 8) + FLIST_HATENA_X;
  dat.pos_y = ((p_wk->scrn_area.scrn_y+DATA_FListCursorData[pos].scrn_data.scrn_y) * 8) + FLIST_HATENA_Y;
  dat.anmseq = ACTANM_HATENA;
  dat.softpri = FLIST_PLAYER_PRI;
  dat.bgpri = FLIST_PLAYER_BGPRI;

  p_wk->p_hate[pos] = GFL_CLACT_WK_Create(
                        p_draw->cellUnitScroll,
                        p_draw->chrRes[CHR_RES_WOR],
                        p_draw->palRes[PAL_RES_WOR],
                        p_draw->celRes[CEL_RES_WOR],
                        &dat, CLSYS_DEFREND_MAIN,
                        heapID );
}

//-------------------------------------
/// �F�B�R�[�h���͉�ʃ��[�N
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�R�[�h���͉��  ������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void CodeIn_Init( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  CodeIn_DrawInit( p_wk, p_data, p_draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�R�[�h���͉��  ���C���֐�
 *
 *  @param  p_wk    ���[�N
 *  @param  p_sys   �V�X�e�����[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET CodeIn_Main( WFNOTE_CODEIN* p_wk, WFNOTE_WK* p_sys, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  int block[ CODE_BLOCK_MAX ];


  switch( p_data->subseq ){
  case SEQ_CODEIN_NAMEIN:
    // ui�폜
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );
    // ���O���́A�R�[�h���̓p�����[�^�쐬
    GFL_OVERLAY_Load( FS_OVERLAY_ID(namein) );
    p_wk->p_namein = CodeIn_NameInParamMake( p_wk, p_data, heapID );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(namein) );

    CodeIn_BlockDataMake_4_4_4( block );
    p_wk->p_codein = CodeInput_ParamCreate( heapID, CODEIN_MODE_FRIEND, FRIENDCODE_MAXLEN, block );

    WFNOTE_DrawExit( p_sys ); // ��ʃ��[�N�S�j��
    p_wk->p_subproc = GFL_PROC_LOCAL_boot( heapID );
    GFL_PROC_LOCAL_CallProc( p_wk->p_subproc , FS_OVERLAY_ID(namein) ,
        &NameInputProcData, p_wk->p_namein );
    p_data->subseq = SEQ_CODEIN_NAMEIN_WAIT;
    break;

  case SEQ_CODEIN_NAMEIN_WAIT:
    // �I���҂�
    if( GFL_PROC_LOCAL_Main( p_wk->p_subproc ) != GFL_PROC_MAIN_NULL ){
      break;
    }
//    GFL_PROC_LOCAL_Exit( p_wk->p_subproc );

    // �L�����Z���`�F�b�N
    if( p_wk->p_namein->cancel == 0 ){
      // ����
      GFL_PROC_LOCAL_CallProc( p_wk->p_subproc , FS_OVERLAY_ID(codein),
        &CodeInput_ProcData, p_wk->p_codein );
      p_data->subseq = SEQ_CODEIN_CODEIN_WAIT;
    }else{
      // �I����
      GFL_PROC_LOCAL_Exit( p_wk->p_subproc );
      Data_SetReqStatus( p_data, STATUS_MODE_SELECT, SEQ_MODESEL_INIT );
      p_data->subseq = SEQ_CODEIN_END;
    }
    break;

  case SEQ_CODEIN_CODEIN_WAIT:
    // �I���҂�
    if( GFL_PROC_LOCAL_Main( p_wk->p_subproc ) != GFL_PROC_MAIN_NULL ){
      break;
    }
    GFL_PROC_LOCAL_Exit( p_wk->p_subproc );

    // �f�[�^��ݒ�
    Data_CODEIN_Set( p_data, p_wk->p_namein->strbuf, p_wk->p_codein->strbuf );
    Data_SetReqStatus( p_data, STATUS_MODE_SELECT, SEQ_MODESEL_CODECHECK_INIT );
    p_data->subseq = SEQ_CODEIN_END;
    break;

  case SEQ_CODEIN_END:
    // ���O���́A�R�[�h���͉�ʂ̔j��
    GFL_OVERLAY_Load( FS_OVERLAY_ID(namein) );
    NAMEIN_FreeParam( p_wk->p_namein );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(namein) );
    CodeInput_ParamDelete( p_wk->p_codein );
    // ui�ǂݍ���
    GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
    WFNOTE_DrawInit( p_sys, heapID ); // ��ʕ��A
    return WFNOTE_STRET_FINISH;



  case SEQ_CODEIN_NAMEINONLY: // ���O���͂̂�
    // ui�폜
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );
    // ���O���́A�R�[�h���̓p�����[�^�쐬
    GFL_OVERLAY_Load( FS_OVERLAY_ID(namein) );
    p_wk->p_namein = CodeIn_NameInParamMake( p_wk, p_data, heapID );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(namein) );

    WFNOTE_DrawExit( p_sys ); // ��ʃ��[�N�S�j��
    p_wk->p_subproc = GFL_PROC_LOCAL_boot( heapID );
    GFL_PROC_LOCAL_CallProc( p_wk->p_subproc , FS_OVERLAY_ID(namein),
        &NameInputProcData, p_wk->p_namein );
    p_data->subseq = SEQ_CODEIN_NAMEINONLY_WAIT;
    break;

  case SEQ_CODEIN_NAMEINONLY_WAIT:  // ���O���͂̂�
    // �I���҂�
    if( GFL_PROC_LOCAL_Main( p_wk->p_subproc ) != GFL_PROC_MAIN_NULL ){
      break;
    }
    GFL_PROC_LOCAL_Exit( p_wk->p_subproc );

    p_data->key_mode = GFL_UI_CheckTouchOrKey();

    // �L�����Z���`�F�b�N
    if( p_wk->p_namein->cancel==0 ){
      // �����Ɠ��͂��ꂽ�̂ŁA���̏ꍇ��FRIENDLIST�ɖ߂�
      Data_SetReqStatus( p_data, STATUS_FRIENDLIST, SEQ_FLIST_NAMECHG_WAIT );
      // �f�[�^��ݒ�
      Data_CODEIN_Set( p_data, p_wk->p_namein->strbuf, NULL );
      p_data->subseq = SEQ_CODEIN_NAMEINONLY_END;
    }else{
      // �I����
      Data_SetReqStatus( p_data, STATUS_FRIENDLIST, SEQ_FLIST_NAMECHG_WAITWIPE );
      p_data->subseq = SEQ_CODEIN_NAMEINONLY_END;
    }
    break;

  case SEQ_CODEIN_NAMEINONLY_END: // ���O���͂̂�
    // ���O���́A�R�[�h���͉�ʂ̔j��
    GFL_OVERLAY_Load( FS_OVERLAY_ID(namein) );
    NAMEIN_FreeParam( p_wk->p_namein );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(namein) );
    // ui�ǂݍ���
    GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
    WFNOTE_DrawInit( p_sys, heapID ); // ��ʕ��A
    return WFNOTE_STRET_FINISH;
  }
  return WFNOTE_STRET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�R�[�h���͉��  �j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void CodeIn_Exit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{

  CodeIn_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��֌W�̏�����
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�v
 */
//-----------------------------------------------------------------------------
static void CodeIn_DrawInit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��֌W�̔j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void CodeIn_DrawExit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���O���́@���[�N�쐬
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static NAMEIN_PARAM* CodeIn_NameInParamMake( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, HEAPID heapID )
{
  MISC        *misc = SaveData_GetMisc( GAMEDATA_GetSaveControlWork(p_data->pGameData) );
  NAMEIN_PARAM* p_param;

  p_param = NAMEIN_AllocParam( heapID, NAMEIN_FRIENDNAME, 0, 0,
              NAMEIN_PERSON_LENGTH,
              NULL,  misc );

  // ���O���͉�ʏ�����data�`�F�b�N
  if( p_data->namein_init.init == TRUE ){
//@todo ����Ή�
    #if( PM_LANG == LANG_JAPAN )
    {
      //if( IsJapaneseCode( *p_data->namein_init.cp_str ) ){
      if( TRUE ){
        GFL_STR_SetStringCode( p_param->strbuf, p_data->namein_init.cp_str );
      }else{
        GFL_STR_ClearBuffer( p_param->strbuf );
      }
    }
    #else
    {
      //if( IsJapaneseCode( *p_data->namein_init.cp_str ) == FALSE ){
      if( FALSE ){
        GFL_STR_SetStringCode( p_param->strbuf, p_data->namein_init.cp_str );
      }else{
        GFL_STR_ClearBuffer( p_param->strbuf );
      }
    }
    #endif
    p_data->namein_init.init = FALSE;
  }
  return p_param;
}

//-------------------------------------
/// �����̗F�B�R�[�h�m�F��ʃ��[�N
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  �����̗F�B�R�[�h�m�F��ʃ��[�N������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�v
 */
//-----------------------------------------------------------------------------
static void MyCode_Init( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  MyCode_DrawInit( p_wk, p_data, p_draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����̗F�B�R�[�h�m�F��� ���C������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *
 *  @retval WFNOTE_STRET_CONTINUE,  // �Â�
 *  @retval WFNOTE_STRET_FINISH,  // ��Ƃ͏I�����
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET MyCode_Main( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  switch( p_data->subseq ){
  case SEQ_MYCODE_INIT:
    MyCode_Init( p_wk, p_data, p_draw, HEAPID_WIFINOTE );
    MyCodeSeq_Init( p_wk, p_data, p_draw );
    p_data->subseq = SEQ_MYCODE_MAIN;
    break;

  case SEQ_MYCODE_MAIN:
    {
      int ret = MyCode_Input( p_wk, p_data, p_draw );
      if( ret == 0 ){
        PMSND_PlaySE( WIFINOTE_BS_SE );
        SetCancelButtonAnime( p_draw );
        p_data->subseq = SEQ_MYCODE_BTNANM;
      }else if( ret == 1 ){
        PMSND_PlaySE( WIFINOTE_EXIT );
        SetExitButtonAnime( p_draw );
        p_data->subseq = SEQ_MYCODE_BTNANM_X;
      }
    }
    break;

  case SEQ_MYCODE_BTNANM:
    if( GFL_CLACT_WK_CheckAnmActive( p_draw->p_tb ) == TRUE ){
      break;
    }
    Data_SetReqStatus( p_data, STATUS_MODE_SELECT, SEQ_MODESEL_INIT_NOFADE );
    MyCode_DrawOff( p_wk, p_draw ); // �\��OFF
    MyCode_Exit( p_wk, p_data, p_draw );
    return WFNOTE_STRET_FINISH;

  case SEQ_MYCODE_BTNANM_X:
    if( GFL_CLACT_WK_CheckAnmActive( p_draw->p_tbx ) == TRUE ){
      break;
    }
    p_data->subseq = SEQ_MYCODE_FADEOUT;
    break;

  case SEQ_MYCODE_FADEOUT:
    WIPE_SYS_Start(
      WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
      WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);
    p_data->subseq = SEQ_MYCODE_FADEOUT_WAIT;
    break;

  case SEQ_MYCODE_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE ){
      Data_SetReqStatus( p_data, STATUS_END_X, 0 );
//      MyCode_DrawOff( p_wk, p_draw ); // �\��OFF
      MyCode_Exit( p_wk, p_data, p_draw );
      return WFNOTE_STRET_FINISH;
    }
    break;
  }
  return WFNOTE_STRET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����̗F�B�R�[�h�m�F��ʃ��[�N�j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void MyCode_Exit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  MyCode_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����̗F�����R�[�h�m�F��ʁ@���͐���
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *
 *  @retval "0 = �L�����Z��"
 *  @retval "1 = �I��"
 *  @retval "-1 = ������"
 */
//-----------------------------------------------------------------------------
static int MyCode_Input( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  static const GFL_UI_TP_HITTBL TpRect[] = {
  //  {0,191,0,255}, ty,by,lx,rx
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_07,TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1},
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_06,TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1},
    {GFL_UI_TP_HIT_END,0,0,0},
  };

  int ret;

  ret = GFL_UI_TP_HitTrg( TpRect );

  if( ret != GFL_UI_TP_HIT_NONE ){
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    p_data->key_mode = GFL_APP_END_TOUCH;
    return ret;
  }

//  Data_CheckKeyModeChange(p_data);
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    p_data->key_mode = GFL_APP_END_KEY;
    return 0;
  }

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    p_data->key_mode = GFL_APP_END_KEY;
    return 1;
  }

  return -1;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��֌W�̏�����
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void MyCode_DrawInit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  u64 code;
  DWCUserData* p_userdata;
  WIFI_LIST* p_list;
  STRBUF* p_str;
  STRBUF* p_tmp;

  // �r�b�g�}�b�v�쐬
  Sub_BmpWinAdd(
      p_draw,
      &p_wk->msg, DFRM_SCRMSG,
      BMPL_MYCODE_MSG_X, BMPL_MYCODE_MSG_Y,
      BMPL_MYCODE_MSG_SIZX, BMPL_MYCODE_MSG_SIZY,
      BGPLT_M_MSGFONT, BMPL_MYCODE_MSG_CGX , 0);
  Sub_BmpWinAdd(
      p_draw,
      &p_wk->code, DFRM_SCRMSG,
      BMPL_MYCODE_CODE_X, BMPL_MYCODE_CODE_Y,
      BMPL_MYCODE_CODE_SIZX, BMPL_MYCODE_CODE_SIZY,
      BGPLT_M_MSGFONT, BMPL_MYCODE_CODE_CGX , 0);
/*
  Sub_BmpWinAdd( &p_wk->back, DFRM_SCRMSG,
      BMPL_MYCODE_BACK_X, BMPL_MYCODE_BACK_Y,
      BMPL_MYCODE_BACK_SIZX, BMPL_MYCODE_BACK_SIZY,
      BGPLT_M_MSGFONT, BMPL_MYCODE_BACK_CGX , 0);
*/

  p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  // �����̃R�[�h�擾
  p_list = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData(p_data->p_save);
  p_userdata = WifiList_GetMyUserInfo( p_list );
  code = DWC_CreateFriendKey( p_userdata );

  if( code != 0 ){
    // �R�[�h����
    // ���b�Z�[�W
    GFL_MSG_GetString( p_draw->p_msgman, msg_mycode_codeon, p_str );
    PRINTTOOL_PrintColor(
      &p_wk->msg, p_draw->printQue,
      4, 0, p_str, p_draw->fontHandle, WFNOTE_COL_BLACK, PRINTTOOL_MODE_LEFT );
    PutBmpWin( p_wk->msg.win );

    // ����
    Draw_FriendCodeSetWordset( p_draw, code );
    GFL_MSG_GetString( p_draw->p_msgman, msg_mycode_code, p_tmp );
    WORDSET_ExpandStr( p_draw->p_wordset, p_str, p_tmp );
    PRINTTOOL_PrintColor(
      &p_wk->code, p_draw->printQue,
      0, 0, p_str, p_draw->fontHandle , WFNOTE_COL_WHITE, PRINTTOOL_MODE_LEFT );
    PutBmpWin( p_wk->code.win );
  }else{
    // �R�[�h�Ȃ�
    // ���b�Z�[�W
    GFL_MSG_GetString( p_draw->p_msgman, msg_mycode_codeoff, p_str );
    PRINTTOOL_PrintColor(
      &p_wk->msg, p_draw->printQue,
      4, 0, p_str, p_draw->fontHandle, WFNOTE_COL_BLACK, PRINTTOOL_MODE_LEFT );
    PutBmpWin( p_wk->msg.win );
  }
  //�߂�{�^��
/*
  GFL_MSG_GetString( p_draw->p_msgman, msg_mycode_back, p_str );
  PRINTSYS_PrintQueColor( p_draw->printQue , GFL_BMPWIN_GetBmp(p_wk->back), 0, 0, p_str, p_draw->fontHandle , WFNOTE_TCOL_BLACK);
*/
  GFL_STR_DeleteBuffer( p_str );
  GFL_STR_DeleteBuffer( p_tmp );

  // �X�N���[���f�[�^�ǂݍ���
  p_wk->p_scrnbuff = GFL_ARCHDL_UTIL_LoadScreen( p_draw->p_handle,
      NARC_wifi_note_friend_code_d_NSCR, FALSE, &p_wk->p_scrn, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��֌W�̔j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void MyCode_DrawExit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  // PrintUtil�̓]���f�[�^��������
  InitPrintUtil( p_draw );
  SetPrintUtil( p_draw, &p_draw->title );

  GFL_BMPWIN_Delete( p_wk->msg.win );
  GFL_BMPWIN_Delete( p_wk->code.win );
//  GFL_BMPWIN_Delete( p_wk->back );
  GFL_HEAP_FreeMemory( p_wk->p_scrnbuff );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �������@�V�[�P���X
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void MyCodeSeq_Init( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  // �^�C�g���ݒ�
  WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, GAMEDATA_GetMyStatus(p_data->pGameData) );
  Draw_BmpTitleWrite( p_draw, &p_draw->title, msg_mycode_title );

  // �w�i�ݒ�
  GFL_BG_WriteScreen( DFRM_SCROLL,
      p_wk->p_scrn->rawData, 0, 0,
      p_wk->p_scrn->screenWidth/8, p_wk->p_scrn->screenHeight/8 );
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );

  // ������ݒ�
//  GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->msg.win );
//  GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->code.win );
//  GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->back );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\��OFF
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void MyCode_DrawOff( WFNOTE_MYCODE* p_wk, WFNOTE_DRAW* p_draw )
{
  // �^�C�g��
  Draw_BmpTitleOff( p_draw );

  // SCROLL��
  GFL_BG_FillScreen( DFRM_SCROLL, 0, 0, 0, 32, 24, 0 );
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );

  // msg
  GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->msg.win );
  GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->code.win );
//  GF_BGL_BmpWinOffVReq(&p_wk->code);
//  GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->back );
//  GF_BGL_BmpWinOffVReq(&p_wk->back);

  // SCRMSG��
  GFL_BG_FillScreen( DFRM_SCRMSG, 0, 0, 0, 32, 24, 0 );
  GFL_BG_LoadScreenV_Req( DFRM_SCRMSG );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �߂�{�^����ԕ\��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void MyCode_BackButtonDraw( WFNOTE_MYCODE* p_wk, WFNOTE_DRAW* p_draw ,BOOL on_f)
{
  u8 pal[] = {MODESEL_CUR_PAL_OFF,MODESEL_CUR_PAL_ON};

  GFL_BG_ChangeScreenPalette( DFRM_SCROLL, 17, 20, 14, 4, pal[on_f]);
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
}

//-------------------------------------
/// �F�B�ڍ׏��
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�ڍו\�����  ������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfo_Init( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  // SaveData����t�����e�B�A��\�����邩�`�F�b�N����
  // ���͏�ɕ\��
//FIXME �������`�F�b�N�� ����GS�ڐA���̘b
  //p_wk->frontier_draw = SysFlag_ArriveGet(SaveData_GetEventWork(p_data->p_save),FLAG_ARRIVE_D32R0101);
  p_wk->frontier_draw = TRUE;

  FInfo_DrawInit( p_wk, p_data, p_draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�ڍו\�����  ���C��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET FInfo_Main( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  BOOL result;

  switch( p_data->subseq ){
  case SEQ_FINFO_INIT:
    p_wk->rep = FALSE;
    p_wk->page = 0;
  case SEQ_FINFO_INITRP:
    // �`��J�n
    FInfo_Init( p_wk, p_data, p_draw, heapID );
    FInfo_DrawOn( p_wk, p_data, p_draw, heapID );
    p_data->subseq = SEQ_FINFO_MAIN;
    break;

  case SEQ_FINFO_MAIN:
    p_data->subseq = FInfo_Input( p_wk, p_data, p_draw );
    break;

  case SEQ_FINFO_SCRINIT:
    FInfo_ScrollInit( p_wk, p_data, p_draw, heapID );
    FInfo_ScrollMain( p_wk, p_data, p_draw, heapID );
    p_data->subseq = SEQ_FINFO_SCR;
    break;

  case SEQ_FINFO_SCR:
    result = FInfo_ScrollMain( p_wk, p_data, p_draw, heapID );
    if( result == TRUE ){
      p_data->subseq = SEQ_FINFO_SCR_WAIT_MSG;
    }
    break;

  case SEQ_FINFO_SCR_WAIT_MSG:
    if( PRINTSYS_QUE_IsFinished(p_draw->printQue) == TRUE ){
      // �X�N���[�����W�����ɖ߂�
      GFL_BG_SetScrollReq( DFRM_SCROLL, GFL_BG_SCROLL_X_SET, 0 );
      GFL_BG_SetScrollReq( DFRM_SCRMSG, GFL_BG_SCROLL_X_SET, 0 );

      p_data->subseq = SEQ_FINFO_MAIN;
    }
    break;

    // �\������l��������
  case SEQ_FINFO_FRIENDCHG:
/*
    if( GFL_CLACT_WK_CheckAnmActive( p_draw->p_yazirushi[ACT_YAZI_T] ) == TRUE ||
        GFL_CLACT_WK_CheckAnmActive( p_draw->p_yazirushi[ACT_YAZI_B] ) == TRUE ){
      break;
    }
*/
    if( p_wk->rep ){  // ���s�[�g���Ȃ�E�G�C�g
      p_wk->wait --;
      if( p_wk->wait > 0 ){
        break;
      }
    }

    result = FInfo_SelectListIdxAdd( p_data, p_wk->way );
    if( result ){

      PMSND_PlaySE( WIFINOTE_MOVE_SE );

//      FInfo_DrawOff( p_wk, p_draw );
      FInfo_DrawOffFriendChange( p_wk, p_draw );
      Data_SetReqStatus( p_data, STATUS_FRIENDINFO, SEQ_FINFO_INITRP );
      // �������㉺��������Ă����烊�s�[�g
      p_wk->rep = TRUE;
      FInfo_DrawExit( p_wk, p_data, p_draw );
      return WFNOTE_STRET_FINISH;
    }

    // �ύX�ł��Ȃ������̂Ō��ɖ߂�
    p_data->subseq = SEQ_FINFO_MAIN;
    break;

  case SEQ_FINFO_END:
    if( GFL_CLACT_WK_CheckAnmActive( p_draw->p_tb ) == TRUE ){
      break;
    }
    FInfo_DrawOff( p_wk, p_draw );
    Data_SetReqStatus( p_data, STATUS_FRIENDLIST, SEQ_FLIST_INFORET );
    FInfo_DrawExit( p_wk, p_data, p_draw );
    return WFNOTE_STRET_FINISH;

  case SEQ_FINFO_END_X:
    if( GFL_CLACT_WK_CheckAnmActive( p_draw->p_tbx ) == TRUE ){
      break;
    }
    p_data->subseq = SEQ_FINFO_END_FADEOUT;
    break;

  case SEQ_FINFO_END_FADEOUT:
    WIPE_SYS_Start(
      WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
      WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID );
    p_data->subseq = SEQ_FINFO_END_FADEOUT_WAIT;
    break;

  case SEQ_FINFO_END_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() ){
      Data_SetReqStatus( p_data, STATUS_END_X, 0 );
      FInfo_DrawExit( p_wk, p_data, p_draw );
      return WFNOTE_STRET_FINISH;
    }
    break;
  }

  return WFNOTE_STRET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�ڍו\�����  �j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void FInfo_Exit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  FInfo_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ڍ׉�ʁ@���̓y�[�W���ŏI�y�[�W���ǂ����H
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static BOOL FInfoInput_IsEndPage( WFNOTE_FRIENDINFO* p_wk ,s16 page )
{
  s16 page_num;
  // �t�����e�B�A��`�悷�邩�`�F�b�N
  if( p_wk->frontier_draw == TRUE ){
    page_num = FINFO_PAGE_NUM;
  }else{
    page_num = FINFO_BFOFF_PAGE_NUM;
  }
  if(page == page_num-1){
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ڍ׉�ʁ@���̓R�[�h���s
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static u32 FInfoInput_Exe( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw ,u32 code )
{
  u8 page_num;

  switch( code ){
/*
  case FINFO_EXE_DECIDE:
    if( FInfoInput_IsEndPage( p_wk, p_wk->page )){
      PMSND_PlaySE( WIFINOTE_DECIDE_SE );
      return SEQ_FINFO_END;
    }
    break;
*/
  case FINFO_EXE_CANCEL:
    PMSND_PlaySE( WIFINOTE_BS_SE );
    SetCancelButtonAnime( p_draw );
    return SEQ_FINFO_END;

  case FINFO_EXE_EXIT:
    PMSND_PlaySE( WIFINOTE_EXIT );
    SetExitButtonAnime( p_draw );
    return SEQ_FINFO_END_X;
/*
  case FINFO_EXE_LEFT:
    FInfo_CursorDraw( p_wk, p_draw, p_wk->page );
    FInfo_ShioriDraw( p_wk, p_draw, p_wk->page );
    p_wk->way = WF_COMMON_LEFT;
    return SEQ_FINFO_SCRINIT;
  case FINFO_EXE_RIGHT:
    FInfo_CursorDraw( p_wk, p_draw, p_wk->page );
    FInfo_ShioriDraw( p_wk, p_draw, p_wk->page );
    p_wk->way = WF_COMMON_RIGHT;
    return SEQ_FINFO_SCRINIT;
*/
  case FINFO_EXE_TOP:
    if( FInfo_SelectListIdxCheck( p_data, WF_COMMON_TOP ) == TRUE ){
      p_wk->way = WF_COMMON_TOP;
      p_wk->wait = FINFO_PAGECHG_WAIT;
      Draw_YazirushiAnmChg( p_draw, ACT_YAZI_B, APP_COMMON_BARICON_CURSOR_UP_ON );
      return SEQ_FINFO_FRIENDCHG;
    }
    break;

  case FINFO_EXE_BOTTOM:
    if( FInfo_SelectListIdxCheck( p_data, WF_COMMON_BOTTOM ) == TRUE ){
      p_wk->way = WF_COMMON_BOTTOM;
      p_wk->wait = FINFO_PAGECHG_WAIT;
      Draw_YazirushiAnmChg( p_draw, ACT_YAZI_T, APP_COMMON_BARICON_CURSOR_DOWN_ON );
      return SEQ_FINFO_FRIENDCHG;
    }
    break;
  }
  return SEQ_FINFO_MAIN;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ڍ׉�ʁ@�L�[���͎擾
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static u32 FInfoInput_Key( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  if( p_data->key_mode == GFL_APP_END_TOUCH ){
    if( GFL_UI_KEY_GetTrg() & GFL_PAD_BUTTON_KTST_CHG ){
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      p_data->key_mode = GFL_APP_END_KEY;
    }
  }

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
    return FInfoInput_Exe( p_wk, p_data, p_draw, FINFO_EXE_CANCEL );
  }
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
    return FInfoInput_Exe( p_wk, p_data, p_draw, FINFO_EXE_EXIT );
  }
/*
    if( ( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ) ||
      ( p_data->lr_key_enable && (GFL_UI_KEY_GetRepeat()  & PAD_BUTTON_L) ) ){
      FInfo_PageChange( p_wk, -1 );
      return FInfoInput_Exe( p_wk, p_data, p_draw ,FINFO_EXE_LEFT);
    }else if( ( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ) ||
          ( p_data->lr_key_enable && (GFL_UI_KEY_GetRepeat()  & PAD_BUTTON_R) ) ){
      FInfo_PageChange( p_wk, 1 );
      return FInfoInput_Exe( p_wk, p_data, p_draw ,FINFO_EXE_RIGHT);
    }else if( GFL_UI_KEY_GetCont() & PAD_KEY_UP ){
      return FInfoInput_Exe( p_wk, p_data, p_draw ,FINFO_EXE_TOP);
    }else if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN ){
      return FInfoInput_Exe( p_wk, p_data, p_draw ,FINFO_EXE_BOTTOM);
    }
*/
  if( GFL_UI_KEY_GetCont() & PAD_KEY_UP ){
    return FInfoInput_Exe( p_wk, p_data, p_draw, FINFO_EXE_TOP );
  }
  if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN ){
    return FInfoInput_Exe( p_wk, p_data, p_draw, FINFO_EXE_BOTTOM );
  }

  return SEQ_FINFO_MAIN;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ڍ׉�ʁ@�^�b�`���͎擾
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static u32 FInfoInput_Touch( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, BOOL* touch_f )
{
  u32 ret;
  s16 dif;

/*
  static const GFL_UI_TP_HITTBL TpRectBfOn[] = {
  //  {0,191,0,255}, ty,by,lx,rx
    {TP_FINFO_YAZI_Y1,TP_FINFO_YAZI_Y1+TP_FINFO_YAZI_H,TP_FINFO_YAZI_X,TP_FINFO_YAZI_X+TP_FINFO_YAZI_W},
    {TP_FINFO_YAZI_Y2,TP_FINFO_YAZI_Y2+TP_FINFO_YAZI_H,TP_FINFO_YAZI_X,TP_FINFO_YAZI_X+TP_FINFO_YAZI_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN01_X1,TP_FINFO_BTN01_X1+TP_FINFO_BTN01_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN01_X2,TP_FINFO_BTN01_X2+TP_FINFO_BTN01_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN01_X3,TP_FINFO_BTN01_X3+TP_FINFO_BTN01_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN01_X4,TP_FINFO_BTN01_X4+TP_FINFO_BTN01_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN01_X5,TP_FINFO_BTN01_X5+TP_FINFO_BTN01_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN01_X6,TP_FINFO_BTN01_X6+TP_FINFO_BTN01_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN01_X7,TP_FINFO_BTN01_X7+TP_FINFO_BTN01_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN01_X8,TP_FINFO_BTN01_X8+TP_FINFO_BTN01_W},
    {GFL_UI_TP_HIT_END,0,0,0},
  };
  static const GFL_UI_TP_HITTBL TpRectBfOff[] = {
  //  {0,191,0,255}, ty,by,lx,rx
    {TP_FINFO_YAZI_Y1,TP_FINFO_YAZI_Y1+TP_FINFO_YAZI_H,TP_FINFO_YAZI_X,TP_FINFO_YAZI_X+TP_FINFO_YAZI_W},
    {TP_FINFO_YAZI_Y2,TP_FINFO_YAZI_Y2+TP_FINFO_YAZI_H,TP_FINFO_YAZI_X,TP_FINFO_YAZI_X+TP_FINFO_YAZI_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN02_X1,TP_FINFO_BTN02_X1+TP_FINFO_BTN01_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN02_X2,TP_FINFO_BTN02_X2+TP_FINFO_BTN01_W},
    {TP_FINFO_BTN_Y,TP_FINFO_BTN_Y+TP_FINFO_BTN_H,TP_FINFO_BTN01_X8,TP_FINFO_BTN01_X8+TP_FINFO_BTN01_W},
    {GFL_UI_TP_HIT_END,0,0,0},
  };
  if(p_wk->frontier_draw){
    ret = GFL_UI_TP_HitTrg(TpRectBfOn);
  }else{
    ret = GFL_UI_TP_HitTrg(TpRectBfOff);
  }
*/
  static const GFL_UI_TP_HITTBL TpRect[] = {
  //  {0,191,0,255}, ty,by,lx,rx
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_00,TOUCHBAR_ICON_X_00+TOUCHBAR_ICON_WIDTH-1},
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_01,TOUCHBAR_ICON_X_01+TOUCHBAR_ICON_WIDTH-1},
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_07,TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1},
    {TOUCHBAR_ICON_Y,TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,TOUCHBAR_ICON_X_06,TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1},
    {GFL_UI_TP_HIT_END,0,0,0},
  };
  ret = GFL_UI_TP_HitTrg(TpRect);

  if(ret == GFL_UI_TP_HIT_NONE){
    return SEQ_FINFO_MAIN;
  }

  *touch_f = TRUE;

  if( ret == 0 ){
    return FInfoInput_Exe( p_wk, p_data, p_draw, FINFO_EXE_BOTTOM );
  }else if( ret == 1 ){
    return FInfoInput_Exe( p_wk, p_data, p_draw, FINFO_EXE_TOP );
  }else if( ret == 2 ){
    return FInfoInput_Exe( p_wk, p_data, p_draw, FINFO_EXE_CANCEL );
  }
  return FInfoInput_Exe( p_wk, p_data, p_draw, FINFO_EXE_EXIT );

/*
  ret -= 2;
  if( FInfoInput_IsEndPage( p_wk, ret )){
    p_wk->page = ret;
    *touch_f = TRUE;
    return FInfoInput_Exe( p_wk, p_data, p_draw ,FINFO_EXE_CANCEL );
  }
  dif = ret - p_wk->page;
  if(dif == 0){
    return SEQ_FINFO_MAIN;
  }
  *touch_f = TRUE;

  FInfo_PageChange( p_wk, dif );
  if(dif >= 0){
    return FInfoInput_Exe( p_wk, p_data, p_draw ,FINFO_EXE_RIGHT);
  }else{
    return FInfoInput_Exe( p_wk, p_data, p_draw ,FINFO_EXE_LEFT);
  }
  return SEQ_FINFO_MAIN;
*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ڍ׉�ʁ@���͎擾
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static u32 FInfo_Input( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  u32 ret;
  BOOL touch_f = FALSE;

/*
  if( (GFL_UI_KEY_GetCont() & (PAD_KEY_UP|PAD_KEY_DOWN)) == 0 ){
    p_wk->rep = FALSE;
  }
*/

  ret = FInfoInput_Touch(p_wk, p_data, p_draw, &touch_f);
  if(touch_f){
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    p_data->key_mode = GFL_APP_END_TOUCH;
    return ret;
  }
  return FInfoInput_Key( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[��������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfo_ScrollInit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  u32 drawarea;

  p_wk->count = 0;

  // ���̃A�j�����Ƃ߂�
  Draw_YazirushiSetAnmFlag( p_draw, FALSE );

  // �X�N���[�������Ƀy�[�W����������
  if( p_wk->way == WF_COMMON_LEFT ){
    drawarea = WFNOTE_DRAWAREA_LEFT;
  }else{
    drawarea = WFNOTE_DRAWAREA_RIGHT;
  }
  FInfo_DrawPage( p_wk, p_data, p_draw, drawarea, p_wk->page, heapID );

  // �X�N���[����
  PMSND_PlaySE( WIFINOTE_SCRLL_SE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[�����C��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 *
 *  @retval TRUE  �I��
 *  @retval FALSE �r��
 */
//-----------------------------------------------------------------------------
static BOOL FInfo_ScrollMain( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  u32 draw_area;
  s32 scrll_x;
  NNSG2dViewRect rect;
  u32 idx;

  // �J�E���^���I������炷�ׂĂ�`�悵�ďI���
  if( p_wk->count > FINFO_SCROLL_COUNT ){

    // �X�N���[�����W�����ɖ߂�
    //���b�Z�[�W�\����҂��Ă���߂��悤��
    //GFL_BG_SetScrollReq( DFRM_SCROLL, GFL_BG_SCROLL_X_SET, 0 );
    //GFL_BG_SetScrollReq( DFRM_SCRMSG, GFL_BG_SCROLL_X_SET, 0 );


    // ��Ɨp�O���t�B�b�N������
    if( p_wk->way == WF_COMMON_LEFT ){
      draw_area = WFNOTE_DRAWAREA_LEFT;
    }else{
      draw_area = WFNOTE_DRAWAREA_RIGHT;
    }
    // ���C���ʂɕ\��
    FInfo_DrawPage( p_wk, p_data, p_draw, WFNOTE_DRAWAREA_MAIN,
        p_wk->page, heapID );

    // ��󓮍�J�n
    Draw_YazirushiSetAnmFlag( p_draw, TRUE );
    FInfo_ShioriDraw( p_wk, p_draw, 0xFF );
    return TRUE;
  }


  scrll_x = (p_wk->count * FINFO_SCROLL_SIZX) / FINFO_SCROLL_COUNT;

  if( p_wk->way == WF_COMMON_LEFT ){
    scrll_x = -scrll_x;
  }

  //  �X�N���[�����W�ݒ�
  GFL_BG_SetScroll( DFRM_SCRMSG, GFL_BG_SCROLL_X_SET, scrll_x );

  // �J�E���g
  p_wk->count ++;

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`�揉����
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�v
 */
//-----------------------------------------------------------------------------
static void FInfo_DrawInit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  int i, j;
  static const u32 tbl_DrawAreaCgx[ WFNOTE_DRAWAREA_NUM ] = {
    BMPL_FINFO_TEXT_CGX0,
    BMPL_FINFO_TEXT_CGX1,
    BMPL_FINFO_TEXT_CGX1,
  };

  // �e�y�[�W�̃X�N���[���f�[�^�ǂݍ���
  FInfoScrnData_Init( p_wk->scrn, p_draw, heapID );
  //�x�[�X�E�B���h�E�ǉ�
  FInfoBmpWin_Init( p_wk, p_draw );

  // �`��p�r�b�g�}�b�v
  for( i=0; i < 2; i++ )
  {
    Sub_BmpWinAdd(
        p_draw,
        &p_wk->drawBmpWin[i],
        DFRM_SCRMSG,
        FINFO_PAGE_X+MATH_ABS(DATA_ScrnArea[i].scrn_x),
        FINFO_PAGE_Y+MATH_ABS(DATA_ScrnArea[i].scrn_y),
        FINFO_PAGE_SIZX, FINFO_PAGE_SIZY,
        BGPLT_M_MSGFONT, 0 , 0 );
  }

  // �`��G���A������
  for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){
    FInfoDrawArea_Init( &p_wk->drawarea[i], p_draw, &DATA_ScrnArea[i],
      tbl_DrawAreaCgx[i], heapID );
    // �`��G���A���̃r�b�g�}�b�v�쐬
    for( j=0; j<FINFO_PAGE_NUM; j++ ){
      FInfoDrawArea_MsgBmpInit( &p_wk->drawarea[i], p_draw, j,
          DATA_FInfo_PageBmpNum[j],
          DATA_FInfo_PageBmpP[j], heapID );

      p_wk->drawarea[WFNOTE_DRAWAREA_MAIN ].p_msg[j] = &p_wk->drawBmpWin[0];
      p_wk->drawarea[WFNOTE_DRAWAREA_RIGHT].p_msg[j] = &p_wk->drawBmpWin[1];
      p_wk->drawarea[WFNOTE_DRAWAREA_LEFT ].p_msg[j] = &p_wk->drawBmpWin[1];
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��j��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void FInfo_DrawExit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
  int i, j;

  // PrintUtil�̓]���f�[�^��������
  InitPrintUtil( p_draw );
  SetPrintUtil( p_draw, &p_draw->title );

  // �`��G���A�j��
  for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){
    for( j=0; j<FINFO_PAGE_NUM; j++ ){
      FInfoDrawArea_MsgBmpExit( &p_wk->drawarea[i], j );
    }
    FInfoDrawArea_Exit( &p_wk->drawarea[i] );
  }
  // �`��p�r�b�g�}�b�v
  for( i=0; i < 2; i++ )
  {
    GFL_BMPWIN_Delete( p_wk->drawBmpWin[i].win );
  }
  //�E�B���h�E�j��
  FInfoBmpWin_Exit( p_wk, p_draw );
  // �e�y�[�W�̃X�N���[���f�[�^�j��
  FInfoScrnData_Exit( p_wk->scrn );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\���J�n
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfo_DrawOn( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  GFL_CLACTPOS pos;

  // �^�C�g���\��
  Draw_FriendNameSetWordset( p_draw, p_data,
      p_data->idx.fridx[p_data->select_listidx], heapID );
  Draw_BmpTitleWrite( p_draw, &p_draw->title, msg_wifi_note_14 );

  //���ʃx�[�XBG�\��
/*
  GFL_BG_WriteScreen( UFRM_BASE,
      p_wk->scrn[FINFO_SCRNDATA_S].p_scrn->rawData,
      0, 0,
      p_wk->scrn[FINFO_SCRNDATA_S].p_scrn->screenWidth/8,
      p_wk->scrn[FINFO_SCRNDATA_S].p_scrn->screenHeight/8 );
  GFL_BG_LoadScreenV_Req( UFRM_BASE );
*/
  //����ʃx�[�XBG�\��
  GFL_BG_WriteScreen( DFRM_BACK,
      p_wk->scrn[FINFO_SCRNDATA_MB].p_scrn->rawData,
      0, 0,
      p_wk->scrn[FINFO_SCRNDATA_MB].p_scrn->screenWidth/8,
      p_wk->scrn[FINFO_SCRNDATA_MB].p_scrn->screenHeight/8 );
  GFL_BG_LoadScreenV_Req( DFRM_BACK );
  GFL_BG_WriteScreen( DFRM_SCROLL,
      p_wk->scrn[FINFO_SCRNDATA_M].p_scrn->rawData,
      0, 0,
      p_wk->scrn[FINFO_SCRNDATA_M].p_scrn->screenWidth/8,
      p_wk->scrn[FINFO_SCRNDATA_M].p_scrn->screenHeight/8 );
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );

  FInfo_DrawBaseInfo( p_wk, p_data, p_draw, heapID );

//  Draw_CursorActorPriSet( p_draw, 0, 0 );

  // �y�[�W�̕\��
  FInfo_DrawPage( p_wk, p_data, p_draw, WFNOTE_DRAWAREA_MAIN,
      p_wk->page, heapID );

  // ���\��
  if( FInfo_SelectListIdxCheck( p_data, WF_COMMON_TOP ) == TRUE ){
    Draw_YazirushiAnmChg( p_draw, ACT_YAZI_B, APP_COMMON_BARICON_CURSOR_UP );
    Draw_YazirushiAnmChg( p_draw, ACT_YAZI_T, APP_COMMON_BARICON_CURSOR_DOWN );
  }else{
    Draw_YazirushiAnmChg( p_draw, ACT_YAZI_B, APP_COMMON_BARICON_CURSOR_UP_OFF );
    Draw_YazirushiAnmChg( p_draw, ACT_YAZI_T, APP_COMMON_BARICON_CURSOR_DOWN_OFF );
  }
  Draw_YazirushiSetDrawFlag( p_draw, TRUE );

/*
  // ���\��
  pos.x = WFNOTE_YAZIRUSHI_TBX;
  pos.y = WFNOTE_YAZIRUSHI_TY;
  GFL_CLACT_WK_SetPos( p_draw->p_yazirushi[ACT_YAZI_L], &pos , CLSYS_DEFREND_MAIN );
  Draw_YazirushiAnmChg( p_draw, ACT_YAZI_T, ACT_YAZIANM_NORMAL );

  pos.y = WFNOTE_YAZIRUSHI_BY;
  GFL_CLACT_WK_SetPos( p_draw->p_yazirushi[ACT_YAZI_R], &pos , CLSYS_DEFREND_MAIN );
  Draw_YazirushiAnmChg( p_draw, ACT_YAZI_B, ACT_YAZIANM_NORMAL );
  Draw_YazirushiSetDrawFlag( p_draw, TRUE );
*/

  //�L�����N�^�x�[�X�X�V
//  GFL_BG_SetPriority( DFRM_SCROLL, 0);
//  GF_BGL_BGControlReset( p_draw->p_bgl, DFRM_MSG, BGL_RESET_CHRBASE,GX_BG_CHARBASE_0x10000 );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\��OFF
 *
 *  @param  p_wk  ���[�N
 *  @param  p_draw  �`��V�X�e��
 *
 */
//-----------------------------------------------------------------------------
static void FInfo_DrawOff( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw )
{
  int i;
  GFL_CLACTPOS pos;

  Draw_YazirushiSetDrawFlag( p_draw, FALSE );

/*
  pos.x = WFNOTE_YAZIRUSHI_LX;
  pos.y = WFNOTE_YAZIRUSHI_LRY;
  GFL_CLACT_WK_SetPos( p_draw->p_yazirushi[ACT_YAZI_L], &pos , CLSYS_DEFREND_MAIN );
  Draw_YazirushiAnmChg( p_draw, ACT_YAZI_L, ACT_YAZIANM_NORMAL );

  pos.x = WFNOTE_YAZIRUSHI_RX;
  GFL_CLACT_WK_SetPos( p_draw->p_yazirushi[ACT_YAZI_R], &pos , CLSYS_DEFREND_MAIN );
  Draw_YazirushiAnmChg( p_draw, ACT_YAZI_R, ACT_YAZIANM_NORMAL );
*/
/*
  for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){
    FInfoDrawArea_PageOff( &p_wk->drawarea[ i ], p_draw );
  }
*/
  FInfo_ShioriOff( p_wk, p_draw );
//  FInfo_CursorOff( p_wk, p_draw );
  FInfo_DrawOffBaseInfo( p_wk );

//  GFL_BG_FillScreen( UFRM_BASE, 0, 0, 0, 32, 32, 0);
//  GFL_BG_FillScreen( UFRM_MSG, 0, 0, 0, 32, 32, 0);
  GFL_BG_FillScreen( DFRM_SCROLL, 0, 0, 0, 32, 32, 0);
//  GFL_BG_LoadScreenV_Req( UFRM_BASE );
//  GFL_BG_LoadScreenV_Req( UFRM_MSG );
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );

  GFL_BG_WriteScreen( DFRM_BACK,
      p_wk->scrn[FINFO_SCRNDATA_MO].p_scrn->rawData,
      0, 0,
      p_wk->scrn[FINFO_SCRNDATA_MO].p_scrn->screenWidth/8,
      p_wk->scrn[FINFO_SCRNDATA_MO].p_scrn->screenHeight/8 );
  GFL_BG_LoadScreenV_Req( DFRM_BACK );

  Draw_BmpTitleOff( p_draw );
  FInfo_TrGraDelete( p_wk );

//  GF_BGL_BGControlReset( p_draw->p_bgl, DFRM_MSG, BGL_RESET_CHRBASE,GX_BG_CHARBASE_0x00000 );
//  GFL_BG_SetPriority( DFRM_SCROLL, 2);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���X�g�ύX���@�\��OFF
 *
 *  @param  p_wk  ���[�N
 *  @param  p_draw  �`��V�X�e��
 *
 */
//-----------------------------------------------------------------------------
static void FInfo_DrawOffFriendChange( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw )
{
  int i;

//  Draw_YazirushiSetDrawFlag( p_draw, FALSE );

  for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){
    FInfoDrawArea_PageOff( &p_wk->drawarea[ i ], p_draw );
  }
/*
  //
  for(i = FINFO_WIN_PAGE;i <= FINFO_WIN_TRGRA;i++){
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->win[i]), 0 );
    GFL_BMPWIN_TransVramCharacter(p_wk->win[i]);
  }
*/
  Draw_BmpTitleOff( p_draw );
  FInfo_TrGraDelete( p_wk );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���ʃx�[�X���̕\��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfo_DrawBaseInfo( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  WIFI_LIST* p_wifilist;
  u32 sex,col,i;

  p_wifilist = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
  sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_SEX );

  for(i = 0; i < FINFO_WIN_MAX;i++){
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->win[i].win),0);
  }

  // �y�[�W
  PRINTTOOL_PrintFractionColor(
    &p_wk->win[FINFO_WIN_PAGE], p_draw->printQue, p_draw->fontHandle,
    GFL_BMPWIN_GetScreenSizeX(p_wk->win[FINFO_WIN_PAGE].win)*8/2, 0, WFNOTE_COL_WHITE,
    p_data->idx.infoidx[p_data->select_listidx]+1,
    p_data->idx.infonum,
    heapID );
  //�u�������񂹂������v
  FInfoDraw_BaseBmp( &p_wk->win[FINFO_WIN_RECORD], p_data, p_draw, msg_finfo_p00_btt, 0, 0, WFNOTE_COL_WHITE );
  //�u�����v
  FInfoDraw_BaseBmp( &p_wk->win[FINFO_WIN_WIN], p_data, p_draw, msg_finfo_p00_bt01, 0, 0, WFNOTE_COL_WHITE );
  //�u�܂��v
  FInfoDraw_BaseBmp( &p_wk->win[FINFO_WIN_LOSE], p_data, p_draw, msg_finfo_p00_bt02, 0, 0, WFNOTE_COL_WHITE );
  // ������
  Draw_NumberSetWordset(
    p_draw,
    WifiList_GetFriendInfo(p_wifilist,p_data->idx.fridx[p_data->select_listidx],WIFILIST_FRIEND_BATTLE_WIN));
  FInfoDraw_BaseBmp( &p_wk->win[FINFO_WIN_WIN_NUM], p_data, p_draw, msg_finfo_p00_btnum, 0, 0, WFNOTE_COL_WHITE );
  // ������
  Draw_NumberSetWordset(
    p_draw,
    WifiList_GetFriendInfo(p_wifilist,p_data->idx.fridx[p_data->select_listidx],WIFILIST_FRIEND_BATTLE_LOSE));
  FInfoDraw_BaseBmp( &p_wk->win[FINFO_WIN_LOSE_NUM], p_data, p_draw, msg_finfo_p00_btnum, 0, 0, WFNOTE_COL_WHITE );
  //�u�|�P������������v
  FInfoDraw_BaseBmp( &p_wk->win[FINFO_WIN_CHANGE], p_data, p_draw, msg_finfo_p00_trt, 0, 0, WFNOTE_COL_WHITE );
  // ������
  Draw_NumberSetWordset(
    p_draw,
    WifiList_GetFriendInfo(p_wifilist,p_data->idx.fridx[p_data->select_listidx],WIFILIST_FRIEND_TRADE_NUM));
  FInfoDraw_BaseBmp( &p_wk->win[FINFO_WIN_CHANGE_NUM], p_data, p_draw, msg_finfo_p00_trnum, 0, 0, WFNOTE_COL_WHITE );
  //�u�������ɂ����񂾂ЂÂ��v
  FInfoDraw_BaseBmp( &p_wk->win[FINFO_WIN_LAST_DAY], p_data, p_draw, msg_finfo_day01, 0, 0, WFNOTE_COL_WHITE );
  // �Ō�ɗV�񂾓��t
  {
    u32 result = Draw_FriendDaySetWordset( p_draw, p_data, p_data->idx.fridx[p_data->select_listidx] );
    if( result ){
      FInfoDraw_BaseBmp(
        &p_wk->win[FINFO_WIN_LAST_DAY_NUM], p_data, p_draw, msg_finfo_day02, 0, 0, WFNOTE_COL_WHITE );
    }else{
      GFL_BMPWIN_TransVramCharacter( p_wk->win[FINFO_WIN_LAST_DAY_NUM].win );
    }
  }

  for( i=0; i<FINFO_WIN_MAX;i++ ){
    PutBmpWin( p_wk->win[i].win );
  }

  //�g���[�i�[�O���t�B�b�N
  {
    ARCHANDLE * ah;
    u32 num;

    GFL_CLWK_DATA clearadd = {
      56,80,
      0,
      0,0,
    };

    num = WifiList_GetFriendInfo(
            p_wifilist,
            p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_UNION_GRA );
    //num = UnionView_GetTrainerInfo( sex, num, UNIONVIEW_TRTYPE );
//    num = UnionView_GetTrType( UnionView_Objcode_to_Index(num) );
    num = UnionView_GetTrType( num );

    ah = TR2DGRA_OpenHandle( heapID );
    p_wk->trRes[0] = TR2DGRA_OBJ_CGR_Register( ah, num, CLSYS_DRAW_MAIN, heapID );
    p_wk->trRes[1] = TR2DGRA_OBJ_PLTT_Register( ah, num, CLSYS_DRAW_MAIN, CLACT_PALNUM_TRGRA*0x20, heapID );
    p_wk->trRes[2] = TR2DGRA_OBJ_CELLANM_Register( num, APP_COMMON_MAPPING_128K, CLSYS_DRAW_MAIN, heapID );
    GFL_ARC_CloseDataHandle( ah );

    p_wk->p_trgra = GFL_CLACT_WK_Create(
                        p_draw->cellUnit,
                        p_wk->trRes[0],
                        p_wk->trRes[1],
                        p_wk->trRes[2],
                        &clearadd, CLSYS_DEFREND_MAIN, heapID );

/*
    TR_CLACT_GRA arcdata;
    void*   p_trgrabuff;
    GFL_BMP_DATA* tempBmp;
    u32 num,no;

    num = WifiList_GetFriendInfo( p_wifilist,
        p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_UNION_GRA );

    no = UnionView_GetTrainerInfo(sex, num, UNIONVIEW_TRTYPE);

//    TrCLACTGraDataGet( no, TR_PARA_FRONT, &arcdata);
    //p_trgrabuff = GFL_BMP_LoadCharacter( arcdata.arcID, arcdata.ncbrID, FALSE , heapID );

//    p_trgrabuff = GFL_HEAP_AllocMemory( heapID, (BMPL_FINFO_TRGRA_SIZX*BMPL_FINFO_TRGRA_SIZY)*32 );

  ChangesInto_1D_from_2D( arcdata.arcID, arcdata.ncbrID, heapID,
        BMPL_FINFO_TRGRA_CUTX,
        BMPL_FINFO_TRGRA_CUTY,
        BMPL_FINFO_TRGRA_SIZX,
        BMPL_FINFO_TRGRA_SIZY, p_trgrabuff);

    buf = GFL_ARC_UTIL_LoadBGCharacter( TRGRA_GetArcID(), TRGRA_GetCgrArcIndex(0), FALSE, &chr, heapID );

    tempBmp = GFL_BMP_CreateWithData( (u8 *)chr->pRawData, BMPL_FINFO_TRGRA_SIZX, BMPL_FINFO_TRGRA_SIZY, GFL_BMP_16_COLOR, heapID );

    GFL_HEAP_FreeMemory( buf );

    // �r�b�g�}�b�v�ɏ�������
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->win[FINFO_WIN_TRGRA].win),1);
    GFL_BMP_Print(
        tempBmp,
        GFL_BMPWIN_GetBmp(p_wk->win[FINFO_WIN_TRGRA].win),
        0, 0,
        0, 0,
        BMPL_FINFO_TRGRA_SIZX*8,
        BMPL_FINFO_TRGRA_SIZY*8 ,
        GF_BMPPRT_NOTNUKI);
    GFL_BMP_Delete( tempBmp );

    // �g���[�i�[�p���b�g�]��
    GFL_ARC_UTIL_TransVramPalette( arcdata.arcID, arcdata.nclrID, PALTYPE_SUB_BG,
        BMPL_FINFO_TRGRA_PAL*32, 32, heapID );

    GFL_BG_ChangeScreenPalette( BMPL_FINFO_FRM,
          BMPL_FINFO_TRGRA_X,BMPL_FINFO_TRGRA_Y,
          BMPL_FINFO_TRGRA_SIZX, BMPL_FINFO_TRGRA_SIZY, BGPLT_S_TRGRA );

    GFL_BMPWIN_TransVramCharacter( p_wk->win[FINFO_WIN_TRGRA].win );
*/
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���ʃx�[�X���̕\��Off
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfo_DrawOffBaseInfo( WFNOTE_FRIENDINFO* p_wk )
{
  int i;

  for( i=0; i<FINFO_WIN_MAX; i++ ){
    GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->win[i].win );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �y�[�W�̕\��
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  drawarea  �`��G���A
 *  @param  page    �y�[�W
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfo_DrawPage( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 drawarea, u32 page, HEAPID heapID )
{
  if( drawarea == WFNOTE_DRAWAREA_MAIN ){
    p_wk->page = page;  // �\���y�[�W�X�V
    // ������X�V
//    FInfo_ShioriDraw( p_wk, p_draw, page );
//    FInfo_CursorDraw( p_wk, p_draw, page );
  }
/*
  // �y�[�W�`��
  FInfoDrawArea_Page( &p_wk->drawarea[ drawarea ], p_data,
      p_draw, page, &p_wk->scrn[FINFO_SCRNDATA_M], p_wk->frontier_draw,heapID );
*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�[�\���`��  ON
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  page    �y�[�W
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
/*
static void FInfo_CursorDraw( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page )
{
  GFL_CLACTPOS pos;
  u8 anmno;
  if( p_wk->frontier_draw == TRUE ){
    anmno = ACTANM_CUR_REC;
    pos.x = page*32+16;
    pos.y = 22*8;
    GFL_CLACT_WK_SetPos(p_draw->p_cur,&pos,CLSYS_DEFREND_MAIN);
  }else{
    if(page == 2){
      anmno = ACTANM_CUR_REC;
      pos.x = 8*32+16;
      pos.y = 22*8;
      GFL_CLACT_WK_SetPos(p_draw->p_cur,&pos,CLSYS_DEFREND_MAIN);
    }else{
      anmno = ACTANM_CUR_LIST;
      pos.x = page*(14*8)+56;
      pos.y = 22*8;
      GFL_CLACT_WK_SetPos(p_draw->p_cur,&pos,CLSYS_DEFREND_MAIN);
    }
  }
  Draw_CursorActorSet( p_draw,TRUE, anmno );
}
static void FInfo_CursorOff( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw )
{
  Draw_CursorActorSet( p_draw,FALSE, ACTANM_NULL );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  ������`��  ON
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  page    �y�[�W
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfo_ShioriDraw( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page )
{
  u32 x;
  static const u8 siz_x[2][8] = {
    {14,14,4,0,0,0,0,0},
    {4,4,4,4,4,4,4,4},
  };
  static const u8 pos_x[2][8] = {
    {0,14,28,0,0,0,0,0},
    {0,4,8,12,16,20,24,28},
  };
  static const u8 pos_y[2] = {FINFO_SHIORISCRN_POS_Y+8,FINFO_SHIORISCRN_POS_Y};

  // ��{���܂��`��
  // �t�����e�B�AOFF�Ȃ��{�������ύX
  if( p_wk->frontier_draw == TRUE ){
    GFL_BG_WriteScreenExpand( DFRM_SCROLL,
        FINFO_SHIORI_X, FINFO_SHIORI_Y,
        FINFO_SHIORI_SIZX, FINFO_SHIORI_SIZY,
        p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
        FINFO_SHIORISCRN_X, FINFO_SHIORISCRN_Y,
        p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
        p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
  }else{
    GFL_BG_WriteScreenExpand( DFRM_SCROLL,
        FINFO_SHIORI_X, FINFO_SHIORI_Y,
        FINFO_SHIORI_SIZX, FINFO_SHIORI_SIZY,
        p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
        FINFO_SHIORISCRN_X, FINFO_SHIORISCRN_Y+8,
        p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
        p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
  }
  if(page == 0xFF){
    // BG�ʓ]��
    GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
    return;
  }
  // ���̃y�[�W�̂������`��
  x = FINFO_SHIORI_X + (FINFO_SHIORI_ONEX*page) + FINFO_SHIORI_OFSX;
  GFL_BG_WriteScreenExpand( DFRM_SCROLL,
      pos_x[p_wk->frontier_draw][page], FINFO_SHIORI_Y,
      siz_x[p_wk->frontier_draw][page], FINFO_EFF_SHIORI_POS_SIZ,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      pos_x[p_wk->frontier_draw][page], pos_y[p_wk->frontier_draw],
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );


  // BG�ʓ]��
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������OFF
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void FInfo_ShioriOff( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw )
{
  // ������ԂԂ�
  GFL_BG_WriteScreenExpand( DFRM_SCROLL,
      FINFO_SHIORI_X, FINFO_SHIORI_Y,
      FINFO_SHIORI_SIZX, FINFO_SHIORI_SIZY,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData,
      FINFO_CL_SHIORISCRN_X, FINFO_CL_SHIORISCRN_Y,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
      p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
}


//----------------------------------------------------------------------------
/**
 *  @brief  ������A�j���O�O
 *
 *  @param  p_wk  ���[�N
 *  @param  p_draw  �`��V�X�e��
 *  @param  page00  ���̃y�[�W
 *  @param  page01  �O�̃y�[�W
 */
//-----------------------------------------------------------------------------
static void FInfo_ShioriAnm00( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page00, u32 page01 )
{
  int x;

//  FInfo_ShioriDraw( p_wk, p_draw, page01 );
  FInfo_CursorDraw( p_wk, p_draw, page01 );
  // BG�ʓ]��
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������A�j���O�P
 *
 *  @param  p_wk  ���[�N
 *  @param  p_draw  �`��V�X�e��
 *  @param  page00  ���̃y�[�W
 *  @param  page01  �O�̃y�[�W
 */
//-----------------------------------------------------------------------------
static void FInfo_ShioriAnm01( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page00, u32 page01 )
{
  int x;

  FInfo_ShioriDraw( p_wk, p_draw, 0xFF );
//  FInfo_CursorDraw( p_wk, p_draw, page00 );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �I�����X�g�C���f�b�N�X�l�@�ύX
 *
 *  @param  p_data  �f�[�^
 *  @param  way   ����    �ォ����
 *
 *  @retval TRUE  �ύX����
 *  @retval FALSE �ύX�ł��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FInfo_SelectListIdxAdd( WFNOTE_DATA* p_data, WF_COMMON_WAY way )
{
  int i;
  u32 sex;
  s32 select_listidx;
  WIFI_LIST* p_wifilist;

  p_wifilist = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
  select_listidx = p_data->select_listidx;

  if( way == WF_COMMON_TOP ){
    for( i=0; i<p_data->idx.friendnum-1; i++ ){
      select_listidx --;
      if( select_listidx < 0 ){
        select_listidx += p_data->idx.friendnum;
      }

      // ���̐l�̐��ʂ��`�F�b�N
      sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[select_listidx], WIFILIST_FRIEND_SEX );
      if( sex != PM_NEUTRAL ){
        p_data->select_listidx = select_listidx;
        return TRUE;
      }
    }
  }else{
    for( i=0; i<p_data->idx.friendnum-1; i++ ){
      select_listidx = (select_listidx + 1) % p_data->idx.friendnum;

      // ���̐l�̐��ʂ��`�F�b�N
      sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[select_listidx], WIFILIST_FRIEND_SEX );
      if( sex != PM_NEUTRAL ){
        p_data->select_listidx = select_listidx;
        return TRUE;
      }
    }
  }
  return FALSE;
}

static BOOL FInfo_SelectListIdxCheck( WFNOTE_DATA* p_data, WF_COMMON_WAY way )
{
  int i;
  u32 sex;
  s32 select_listidx;
  WIFI_LIST* p_wifilist;

  p_wifilist = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
  select_listidx = p_data->select_listidx;

  if( way == WF_COMMON_TOP ){
    for( i=0; i<p_data->idx.friendnum-1; i++ ){
      select_listidx --;
      if( select_listidx < 0 ){
        select_listidx += p_data->idx.friendnum;
      }
      if( p_data->select_listidx == select_listidx ){
        break;
      }
      // ���̐l�̐��ʂ��`�F�b�N
      sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[select_listidx], WIFILIST_FRIEND_SEX );
      if( sex != PM_NEUTRAL ){
        return TRUE;
      }
    }
  }else{
    for( i=0; i<p_data->idx.friendnum-1; i++ ){
      select_listidx = (select_listidx + 1) % p_data->idx.friendnum;
      if( p_data->select_listidx == select_listidx ){
        break;
      }
      // ���̐l�̐��ʂ��`�F�b�N
      sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[select_listidx], WIFILIST_FRIEND_SEX );
      if( sex != PM_NEUTRAL ){
        return TRUE;
      }
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���f�[�^������
 *
 *  @param  p_scrn    �X�N���[���f�[�^�i�[��
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfoScrnData_Init( WFNOTE_FINFO_SCRNDATA* p_scrn, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  int i;
  u16 id_tbl[] = {
    NARC_wifi_note_friend_recbase_d_NSCR,
    NARC_wifi_note_friend_record_d_NSCR,
    NARC_wifi_note_friend_back_d_NSCR,
    NARC_wifi_note_friend_record_u_NSCR,
  };

  for(i = 0;i < FINFO_SCRNDATA_NUM;i++){
    p_scrn->p_scrnbuff = GFL_ARCHDL_UTIL_LoadScreen(
        p_draw->p_handle, id_tbl[i], FALSE,
        &p_scrn->p_scrn, heapID );
    p_scrn++;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�N���[���f�[�^�j��
 *
 *  @param  p_scrn    �X�N���[���f�[�^
 */
//-----------------------------------------------------------------------------
static void FInfoScrnData_Exit( WFNOTE_FINFO_SCRNDATA* p_scrn )
{
  int i;

  for(i = 0;i < FINFO_SCRNDATA_NUM;i++){
    GFL_HEAP_FreeMemory( p_scrn->p_scrnbuff );
    p_scrn++;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  Bmp�E�B���h�E������
 *
 *  @param  p_scrn    �X�N���[���f�[�^�i�[��
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
///�r�b�g�}�b�v���X�g�p�f�[�^�\����
typedef struct{
//  u8  frm_num;  ///<�E�C���h�E�g�p�t���[��
  u8  pos_x;    ///<�E�C���h�E�̈�̍����X���W�i�L�����P�ʂŎw��j
  u8  pos_y;    ///<�E�C���h�E�̈�̍����Y���W�i�L�����P�ʂŎw��j
  u8  siz_x;    ///<�E�C���h�E�̈��X�T�C�Y�i�L�����P�ʂŎw��j
  u8  siz_y;    ///<�E�C���h�E�̈��Y�T�C�Y�i�L�����P�ʂŎw��j
//  u8  palnum;   ///<�E�C���h�E�̈�̃p���b�g�i���o�[
//  u16 chrnum;   ///<�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
}WNOTE_BMPWIN_DAT;

static void FInfoBmpWin_Init( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw )
{
  int i;
  static const WNOTE_BMPWIN_DAT BmpWinData[] =
  {
    // �y�[�W
    { BMPL_FINFO_WIN_PAGE_PX, BMPL_FINFO_WIN_PAGE_PY, BMPL_FINFO_WIN_PAGE_SX, BMPL_FINFO_WIN_PAGE_SY },
    //�u�������񂹂������v
    { BMPL_FINFO_WIN_RECORD_PX, BMPL_FINFO_WIN_RECORD_PY, BMPL_FINFO_WIN_RECORD_SX, BMPL_FINFO_WIN_RECORD_SY },
    //�u�����v
    { BMPL_FINFO_WIN_WIN_PX, BMPL_FINFO_WIN_WIN_PY, BMPL_FINFO_WIN_WIN_SX, BMPL_FINFO_WIN_WIN_SY },
    //�u�܂��v
    { BMPL_FINFO_WIN_LOSE_PX, BMPL_FINFO_WIN_LOSE_PY, BMPL_FINFO_WIN_LOSE_SX, BMPL_FINFO_WIN_LOSE_SY },
    // ������
    { BMPL_FINFO_WIN_WIN_NUM_PX, BMPL_FINFO_WIN_WIN_NUM_PY, BMPL_FINFO_WIN_WIN_NUM_SX, BMPL_FINFO_WIN_WIN_NUM_SY },
    // ������
    { BMPL_FINFO_WIN_LOSE_NUM_PX, BMPL_FINFO_WIN_LOSE_NUM_PY, BMPL_FINFO_WIN_LOSE_NUM_SX, BMPL_FINFO_WIN_LOSE_NUM_SY },
    //�u�|�P������������v
    { BMPL_FINFO_WIN_CHANGE_PX, BMPL_FINFO_WIN_CHANGE_PY, BMPL_FINFO_WIN_CHANGE_SX, BMPL_FINFO_WIN_CHANGE_SY },
    // ������
    { BMPL_FINFO_WIN_CHANGE_NUM_PX, BMPL_FINFO_WIN_CHANGE_NUM_PY, BMPL_FINFO_WIN_CHANGE_NUM_SX, BMPL_FINFO_WIN_CHANGE_NUM_SY },
    //�u�������ɂ����񂾂ЂÂ��v
    { BMPL_FINFO_WIN_LAST_DAY_PX, BMPL_FINFO_WIN_LAST_DAY_PY, BMPL_FINFO_WIN_LAST_DAY_SX, BMPL_FINFO_WIN_LAST_DAY_SY },
    // �Ō�ɗV�񂾓��t
    { BMPL_FINFO_WIN_LAST_DAY_NUM_PX, BMPL_FINFO_WIN_LAST_DAY_NUM_PY, BMPL_FINFO_WIN_LAST_DAY_NUM_SX, BMPL_FINFO_WIN_LAST_DAY_NUM_SY },
  };

  for( i=0; i<FINFO_WIN_MAX; i++ ){
    Sub_BmpWinAdd(
      p_draw,
      &p_wk->win[i], BMPL_FINFO_FRM,
      BmpWinData[i].pos_x , BmpWinData[i].pos_y ,
      BmpWinData[i].siz_x , BmpWinData[i].siz_y ,
      BMPL_FINFO_PAL, 0,
      0 );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  Bmp�E�B���h�E���
 *
 *  @param  p_scrn    �X�N���[���f�[�^�i�[��
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfoBmpWin_Exit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw )
{
  int i;

  for( i=0; i<FINFO_WIN_MAX; i++ ){
//    GFL_BMPWIN_ClearTransWindow( p_wk->win[i] );
//    GF_BGL_BmpWinOff( &p_wk->win[i] );
    GFL_BMPWIN_Delete( p_wk->win[i].win );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\���G���A���[�N  ������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  cp_scrnarea �\���G���A�f�[�^
 *  @param  cgx     ���b�Z�[�W�ʂ�CGX
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfoDrawArea_Init( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_scrnarea, u32 cgx, HEAPID heapID )
{
  int i;
  p_wk->scrnarea = *cp_scrnarea;
  p_wk->cgx = cgx;

  for( i=0; i<FINFO_PAGE_NUM; i++ ){
//    p_wk->p_msg[i] = NULL;
    p_wk->msgnum[i] = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��G���A�@�j��
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FInfoDrawArea_Exit( WFNOTE_FINFO_DRAWAREA* p_wk )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�r�b�g�}�b�v  ������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_draw    �`��V�X�e��
 *  @param  idx     �C���f�b�N�X
 *  @param  num     ��
 *  @param  cp_data   �\���f�[�^
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FInfoDrawArea_MsgBmpInit( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 idx, u32 num, const WFNOTE_FINFO_BMPDATA* cp_data, HEAPID heapID )
{
  int i;
  u16 cgx;

//  p_wk->p_msg[idx] = GF_BGL_BmpWinAllocGet( heapID, num );
//  p_wk->p_msg[idx] = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMPWIN*)*num );
  p_wk->msgnum[idx] = num;

  // �쐬
/*
  cgx = p_wk->cgx;
  for( i=0; i<num; i++ ){
    Sub_BmpWinAdd( &p_wk->p_msg[idx][i], DFRM_SCRMSG,
        MATH_ABS(p_wk->scrnarea.scrn_x) + cp_data[i].pos_x,
        MATH_ABS(p_wk->scrnarea.scrn_y) + cp_data[i].pos_y,
        cp_data[i].siz_x, cp_data[i].siz_y,
        BGPLT_M_MSGFONT, cgx , 0 );
    cgx += ( cp_data[i].siz_x * cp_data[i].siz_y );
  }
*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�r�b�g�}�b�v�j��
 *
 *  @param  p_wk  ���[�N
 *  @param  idx   �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void FInfoDrawArea_MsgBmpExit( WFNOTE_FINFO_DRAWAREA* p_wk, u32 idx )
{
  u8 i;
  if( p_wk->p_msg[idx] )
  {
    for( i=0;i<p_wk->msgnum[idx];i++ )
    {
//      GFL_BMPWIN_Delete( p_wk->p_msg[idx][i] );
    }
//    GFL_HEAP_FreeMemory( *p_wk->p_msg[idx] );
    p_wk->p_msg[idx] = NULL;
    p_wk->msgnum[idx] = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�V
 *
 *  @param  p_wk  ���[�N
 *  @param  idx   �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void FInfoDrawArea_MsgBmpOnVReq( WFNOTE_FINFO_DRAWAREA* p_wk, u32 idx )
{
  int i;
  for( i=0; i<p_wk->msgnum[idx]; i++ ){
    GFL_BMPWIN_MakeTransWindow_VBlank( p_wk->p_msg[idx]->win );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\��OFF
 *
 *  @param  p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FInfoDrawArea_MsgBmpOffVReq( WFNOTE_FINFO_DRAWAREA* p_wk, u32 idx )
{
  int i;

  for( i=0; i<p_wk->msgnum[idx]; i++ ){
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_msg[idx]->win),0);
    GFL_BMPWIN_ClearTransWindow_VBlank( p_wk->p_msg[idx]->win );
//    GF_BGL_BmpWinOffVReq( &p_wk->p_msg[idx][i] );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �`��A���A�Ƀy�[�W�̏�������
 *
 *  @param  p_wk    ���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`�惏�[�N
 *  @param  page    �y�[�W
 *  @param  cp_scrn   �X�N���[���`��f�[�^
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
/*
static void FInfoDrawArea_Page( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data,
  WFNOTE_DRAW* p_draw, u32 page, const WFNOTE_FINFO_SCRNDATA* cp_scrn, BOOL bf_on,HEAPID heapID )
{
  static void (*pWriteBfOn[ FINFO_PAGE_NUM ])( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID ) = {
    FInfoDraw_Page00,
    FInfoDraw_Page06,
    FInfoDraw_Page01,
    FInfoDraw_Page02,
    FInfoDraw_Page03,
    FInfoDraw_Page04,
    FInfoDraw_Page05,
    FInfoDraw_Page07,
  };
  static void (*pWriteBfOff[ FINFO_BFOFF_PAGE_NUM ])( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID ) = {
    FInfoDraw_Page00,
    FInfoDraw_Page06,
    FInfoDraw_Page07,
  };

  // ���܂ł̕\���N���A
  FInfoDrawArea_PageOff( p_wk, p_draw );

  // �y�[�W��������
  if(bf_on){
    pWriteBfOn[page]( p_wk, p_data, p_draw, heapID );
  }else{
    pWriteBfOff[page]( p_wk, p_data, p_draw, heapID );
  }
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �؁[��OFF
 *
 *  @param  p_wk  ���[�N
 *  @param  p_draw  �`��V�X�e��
 */
//-----------------------------------------------------------------------------
static void FInfoDrawArea_PageOff( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw )
{
  int i;
  for( i=0; i<FINFO_PAGE_NUM; i++ ){
    FInfoDrawArea_MsgBmpOffVReq( p_wk, i );
  }
  FInfoDraw_Clean( p_draw, &p_wk->scrnarea );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �y�[�W�̏�������
 *
 *  @param  p_wk    �`��G���A���[�N
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
/*
static void FInfoDraw_Page00( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  WIFI_LIST* p_wifilist;
  u32 i;
  u8 ofsx = 0;
  u8 ofsy = 0;

  p_wifilist = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );

  // �ΐ퐬�у^�C�g��
  FInfoDraw_Bmp( p_wk, 0, FINFO_PAGE00_BA_BTT,
      p_data, p_draw, msg_finfo_p00_btt,
      FINFO_PAGE00_BTT_X, FINFO_PAGE00_BTT_Y,
      WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );

  // ���� & ����
  for(i = 0;i < 2;i++){
    ofsx = (FINFO_PAGE00_BA_BT_X-FINFO_PAGE00_BA_CAP_X) * 8;
    ofsy = FINFO_PAGE00_BT_Y+i*FINFO_PAGE00_BT_OFSY
        + (FINFO_PAGE00_BA_BT_Y-FINFO_PAGE00_BA_CAP_BTT_Y)*8;

    FInfoDraw_Bmp( p_wk, 0, FINFO_PAGE00_BA_BT,
        p_data, p_draw, msg_finfo_p00_bt01+i,
        FINFO_PAGE00_BT_X+ofsx, ofsy,
        WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );

    Draw_NumberSetWordset( p_draw,
      WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx],
                  WIFILIST_FRIEND_BATTLE_WIN+i));

    ofsx = (FINFO_PAGE00_BA_TR_X-FINFO_PAGE00_BA_CAP_X)*8;
    FInfoDraw_Bmp( p_wk, 0, FINFO_PAGE00_BA_BT,
        p_data, p_draw, msg_finfo_p00_btnum,
        ofsx,ofsy,
        WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );
  }

  // �|�P���������^�C�g��
  ofsx = (FINFO_PAGE00_BA_CAP_X-FINFO_PAGE00_BA_CAP_X)*8;
  ofsy = (FINFO_PAGE00_BA_CAP_TRT_Y-FINFO_PAGE00_BA_CAP_BTT_Y)*8;
  FInfoDraw_Bmp( p_wk, 0, FINFO_PAGE00_BA_TRT,
      p_data, p_draw, msg_finfo_p00_trt,
      FINFO_PAGE00_TRT_X+ofsx, FINFO_PAGE00_TRT_Y+ofsy,
      WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );

  Draw_NumberSetWordset( p_draw,
    WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx],
                WIFILIST_FRIEND_TRADE_NUM));

  ofsx = (FINFO_PAGE00_BA_TR_X-FINFO_PAGE00_BA_CAP_X)*8;
  ofsy = (FINFO_PAGE00_BA_TR_Y-FINFO_PAGE00_BA_CAP_BTT_Y)*8;
  FInfoDraw_Bmp( p_wk, 0, FINFO_PAGE00_BA_TR,
      p_data, p_draw, msg_finfo_p00_trnum,
      FINFO_PAGE00_TRN_X+ofsx, FINFO_PAGE00_TRN_Y+ofsy,
      WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );

  FInfoDrawArea_MsgBmpOnVReq( p_wk, 0 );
}

static void FInfoDraw_Page01( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  u32 num;
  u8 ofsx,ofsy;

  // �^�C�g��
  ofsx = (FINFO_PAGE01_BA_CAP_X-FINFO_PAGE_X)*8;
  ofsy = (FINFO_PAGE01_BA_CAP_Y-FINFO_PAGE_Y)*8;
  FInfoDraw_Bmp( p_wk, 2, FINFO_PAGE01_BA_CAP,
      p_data, p_draw, msg_wifi_note_36,
      FINFO_PAGE01_BTT_X+ofsx, FINFO_PAGE01_BTT_Y+ofsy,
      WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );

  // �O��(���񂴂�)
  ofsx = (FINFO_PAGE01_BA_DAT_X-FINFO_PAGE_X)*8;
  ofsy = (FINFO_PAGE01_BA_DAT_Y-FINFO_PAGE_Y)*8;
  {
    BOOL clear_flag = TRUE;


    FInfoDraw_Bmp( p_wk, 2, FINFO_PAGE01_BA_DAT,
        p_data, p_draw, msg_finfo_frprm_win01+clear_flag,
        FINFO_PAGE01_DAT_X+ofsx, FINFO_PAGE01_DAT_Y01+ofsy,
        WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );
    num = 123;
    Draw_NumberSetWordset( p_draw, num );
    FInfoDraw_Bmp( p_wk, 2, FINFO_PAGE01_BA_DAT,
        p_data, p_draw, msg_finfo_frprm_wnum,
        FINFO_PAGE01_DATN_X+ofsx, FINFO_PAGE01_DAT_Y01+ofsy,
        WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );
  }

  //  �ō�
  FInfoDraw_Bmp( p_wk, 2, FINFO_PAGE01_BA_DAT,
      p_data, p_draw, msg_finfo_frprm_win03,
      FINFO_PAGE01_DAT_X+ofsx, FINFO_PAGE01_DAT_Y02+ofsy,
      WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );
  num = 456;
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 2, FINFO_PAGE01_BA_DAT,
      p_data, p_draw, msg_finfo_frprm_wnum,
      FINFO_PAGE01_DATN_X+ofsx, FINFO_PAGE01_DAT_Y02+ofsy,
      WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );

  FInfoDrawArea_MsgBmpOnVReq( p_wk, 2 );
}

static void FInfoDraw_Page02( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  STRBUF* p_str;
  STRBUF* p_tmp;
  u32 num;
  BOOL clear_flag;
  u32 stridx;
  int ofsx,ofsy;
  p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  // �^�C�g��
  ofsx = (FINFO_PAGE02_BA_TT_X-FINFO_PAGE_X)*8;
  ofsy = (FINFO_PAGE02_BA_TT_Y-FINFO_PAGE_Y)*8;
  FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_BA_TT,
      p_data, p_draw, msg_wifi_note_37,
      FINFO_PAGE02_TT_X+ofsx, FINFO_PAGE02_TT_Y+ofsy,
      WFNOTE_COL_BLACK, p_str, p_tmp );

  // LV50
  {
    // �^�C�g��
    ofsx = (FINFO_PAGE02_BA_LV50TT_X-FINFO_PAGE_X)*8;
    ofsy = (FINFO_PAGE02_BA_LV50TT_Y-FINFO_PAGE_Y)*8;
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV50TT,
        p_data, p_draw, msg_wifi_note_43,
        FINFO_PAGE02_LV50TT_X+ofsx, FINFO_PAGE02_LV50TT_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV50TT,
        p_data, p_draw, msg_wifi_note_45,
        FINFO_PAGE02_LV50TT2_X+ofsx, FINFO_PAGE02_LV50TT_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );

    //�O��
    clear_flag = TRUE;
    if( clear_flag == FALSE ){
      stridx = msg_finfo_frprm_win01;
    }else{
      stridx = msg_finfo_frprm_win02;
    }
    ofsx = (FINFO_PAGE02_BA_LV50NO_X-FINFO_PAGE_X)*8;
    ofsy = (FINFO_PAGE02_BA_LV50NO_Y-FINFO_PAGE_Y)*8;
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV50NO,
        p_data, p_draw, stridx,
        FINFO_PAGE02_LV50LT_X+ofsx, FINFO_PAGE02_LV50L_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );

    // �A��
    num = 789;
    Draw_NumberSetWordset( p_draw, num );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV50NO,
        p_data, p_draw, msg_finfo_frprm_wnum,
        FINFO_PAGE02_LV50LN0_X+ofsx, FINFO_PAGE02_LV50L_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );
    // ����
    num = 147;
    Draw_NumberSetWordset( p_draw, num );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV50NO,
        p_data, p_draw, msg_wifi_note_49,
        FINFO_PAGE02_LV50LN1_X+ofsx, FINFO_PAGE02_LV50L_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );


    //�ō�
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV50NO,
        p_data, p_draw, msg_finfo_frprm_win03,
        FINFO_PAGE02_LV50MT_X+ofsx, FINFO_PAGE02_LV50M_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );

    // �A��
    num = 258;
    Draw_NumberSetWordset( p_draw, num );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV50NO,
        p_data, p_draw, msg_finfo_frprm_wnum,
        FINFO_PAGE02_LV50MN0_X+ofsx, FINFO_PAGE02_LV50M_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );
    // ����
    num = 369;
    Draw_NumberSetWordset( p_draw, num );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV50NO,
        p_data, p_draw, msg_wifi_note_49,
        FINFO_PAGE02_LV50MN1_X+ofsx, FINFO_PAGE02_LV50M_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );
  }

  // LV100
  {
    // �^�C�g��
    ofsx = (FINFO_PAGE02_BA_LV100TT_X-FINFO_PAGE_X)*8;
    ofsy = (FINFO_PAGE02_BA_LV100TT_Y-FINFO_PAGE_Y)*8;
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV100TT,
        p_data, p_draw, msg_wifi_note_44,
        FINFO_PAGE02_LV100TT_X+ofsx, FINFO_PAGE02_LV100TT_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV100TT,
        p_data, p_draw, msg_wifi_note_45,
        FINFO_PAGE02_LV100TT2_X+ofsx, FINFO_PAGE02_LV100TT_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );

    //�O��
    clear_flag = TRUE;
    if( clear_flag == FALSE ){
      stridx = msg_finfo_frprm_win01;
    }else{
      stridx = msg_finfo_frprm_win02;
    }
    ofsx = (FINFO_PAGE02_BA_LV100NO_X-FINFO_PAGE_X)*8;
    ofsy = (FINFO_PAGE02_BA_LV100NO_Y-FINFO_PAGE_Y)*8;
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV100NO,
        p_data, p_draw, stridx,
        FINFO_PAGE02_LV100LT_X+ofsx, FINFO_PAGE02_LV100L_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );

    // �A��
    num = 123;
    Draw_NumberSetWordset( p_draw, num );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV100NO,
        p_data, p_draw, msg_finfo_frprm_wnum,
        FINFO_PAGE02_LV100LN0_X+ofsx, FINFO_PAGE02_LV100L_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );
    // ����
    num = 456;
    Draw_NumberSetWordset( p_draw, num );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV100NO,
        p_data, p_draw, msg_wifi_note_49,
        FINFO_PAGE02_LV100LN1_X+ofsx, FINFO_PAGE02_LV100L_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );


    //�ō�
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV100NO,
        p_data, p_draw, msg_finfo_frprm_win03,
        FINFO_PAGE02_LV100MT_X+ofsx, FINFO_PAGE02_LV100M_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );

    // �A��
    num = 789;
    Draw_NumberSetWordset( p_draw, num );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV100NO,
        p_data, p_draw, msg_finfo_frprm_wnum,
        FINFO_PAGE02_LV100MN0_X+ofsx, FINFO_PAGE02_LV100M_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );
    // ����
    num = 255;
    Draw_NumberSetWordset( p_draw, num );
    FInfoDraw_Bmp( p_wk, 3, FINFO_PAGE02_LV100NO,
        p_data, p_draw, msg_wifi_note_49,
        FINFO_PAGE02_LV100MN1_X+ofsx, FINFO_PAGE02_LV100M_Y+ofsy,
        WFNOTE_COL_BLACK, p_str, p_tmp );
  }


  FInfoDrawArea_MsgBmpOnVReq( p_wk, 3 );

  GFL_STR_DeleteBuffer( p_str );
  GFL_STR_DeleteBuffer( p_tmp );
}
static void FInfoDraw_Page03( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  STRBUF* p_str;
  STRBUF* p_tmp;
  u32 num;
  BOOL clear_flag;
  u32 stridx;
#if USE_FRONTIER_DATA
  FRONTIER_SAVEWORK* p_fsave;

  p_fsave = SaveData_GetFrontier( p_data->p_save );
#endif
  p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );


  // �^�C�g��
  FInfoDraw_Bmp( p_wk, 4, FINFO_PAGE03_BA,
      p_data, p_draw, msg_wifi_note_38,
      FINFO_PAGE03_TT_X, FINFO_PAGE03_TT_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  FInfoDraw_Bmp( p_wk, 4, FINFO_PAGE03_BA,
      p_data, p_draw, msg_wifi_note_46,
      FINFO_PAGE03_CPT_X, FINFO_PAGE03_CPT_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );

  // �O��
#if USE_FRONTIER_DATA
  clear_flag = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_CLEAR_BIT, p_data->idx.fridx[p_data->select_listidx]);
#else
  clear_flag = TRUE;
#endif
  if( clear_flag == FALSE ){
    stridx = msg_finfo_frprm_win01;
  }else{
    stridx = msg_finfo_frprm_win02;
  }
  FInfoDraw_Bmp( p_wk, 4, FINFO_PAGE03_BA,
      p_data, p_draw, stridx,
      FINFO_PAGE03_CPLT_X, FINFO_PAGE03_CPL_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // ���肠
#if USE_FRONTIER_DATA
  num = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_RENSHOU_CNT, p_data->idx.fridx[p_data->select_listidx]);
#else
  num = 147;
#endif
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 4, FINFO_PAGE03_BA,
      p_data, p_draw, msg_wifi_note_51,
      FINFO_PAGE03_CPLN0_X, FINFO_PAGE03_CPL_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // CP
#if USE_FRONTIER_DATA
  num = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_CP, p_data->idx.fridx[p_data->select_listidx]);
#else
  num = 258;
#endif
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 4, FINFO_PAGE03_BA,
      p_data, p_draw, msg_wifi_note_50,
      FINFO_PAGE03_CPLN1_X, FINFO_PAGE03_CPL_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );


  // �ō�
  FInfoDraw_Bmp( p_wk, 4, FINFO_PAGE03_BA,
      p_data, p_draw, msg_finfo_frprm_win03,
      FINFO_PAGE03_CPMT_X, FINFO_PAGE03_CPM_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // ���肠
#if USE_FRONTIER_DATA
  num = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_RENSHOU, p_data->idx.fridx[p_data->select_listidx]);
#else
  num = 369;
#endif
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 4, FINFO_PAGE03_BA,
      p_data, p_draw, msg_wifi_note_51,
      FINFO_PAGE03_CPMN0_X, FINFO_PAGE03_CPM_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // CP
#if USE_FRONTIER_DATA
  num = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_REMAINDER_CP, p_data->idx.fridx[p_data->select_listidx]);
#else
  num = 123;
#endif
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 4, FINFO_PAGE03_BA,
      p_data, p_draw, msg_wifi_note_50,
      FINFO_PAGE03_CPMN1_X, FINFO_PAGE03_CPM_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );


  FInfoDrawArea_MsgBmpOnVReq( p_wk, 4 );


  GFL_STR_DeleteBuffer( p_str );
  GFL_STR_DeleteBuffer( p_tmp );
}
static void FInfoDraw_Page04( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  STRBUF* p_str;
  STRBUF* p_tmp;
  STRBUF* p_monsstr;
  u32 num;
  BOOL clear_flag;
  u32 stridx;
#if USE_FRONTIER_DATA
  FRONTIER_SAVEWORK* p_fsave;

  p_fsave = SaveData_GetFrontier( p_data->p_save );
#endif

  p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );


  // �^�C�g��
  FInfoDraw_Bmp( p_wk, 5, FINFO_PAGE04_BA,
      p_data, p_draw, msg_wifi_note_39,
      FINFO_PAGE04_TT_X, FINFO_PAGE04_TT_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );

#if USE_FRONTIER_DATA
  num = FrontierRecord_Get(p_fsave, FRID_STAGE_MULTI_WIFI_MONSNO, p_data->idx.fridx[p_data->select_listidx]);
#else
  num = 123;
#endif
  p_monsstr = GFL_MSG_CreateString( GlobalMsg_PokeName , num );

  PRINTSYS_PrintQueColor( p_draw->printQue , GFL_BMPWIN_GetBmp(p_wk->p_msg[5][FINFO_PAGE04_BA]),
      FINFO_PAGE04_TT_X, FINFO_PAGE04_TT2_Y,
      p_monsstr, p_draw->fontHandle , WFNOTE_COL_BLACK );

  GFL_STR_DeleteBuffer( p_monsstr );

  // �O��
#if USE_FRONTIER_DATA
  clear_flag = FrontierRecord_Get(p_fsave, FRID_STAGE_MULTI_WIFI_CLEAR_BIT, p_data->idx.fridx[p_data->select_listidx]);
#else
  clear_flag = TRUE;
#endif
  if( clear_flag == FALSE ){
    stridx = msg_finfo_frprm_win01;
  }else{
    stridx = msg_finfo_frprm_win02;
  }
  FInfoDraw_Bmp( p_wk, 5, FINFO_PAGE04_BA,
      p_data, p_draw, stridx,
      FINFO_PAGE04_LT_X, FINFO_PAGE04_L_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // �l
#if USE_FRONTIER_DATA
  num = FrontierRecord_Get(p_fsave, FRID_STAGE_MULTI_WIFI_RENSHOU_CNT, p_data->idx.fridx[p_data->select_listidx]);
#else
  num = 789;
#endif
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 5, FINFO_PAGE04_BA,
      p_data, p_draw, msg_finfo_frprm_wnum,
      FINFO_PAGE04_LN_X, FINFO_PAGE04_L_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );

  // �ō�
  FInfoDraw_Bmp( p_wk, 5, FINFO_PAGE04_BA,
      p_data, p_draw, msg_finfo_frprm_win03,
      FINFO_PAGE04_MT_X, FINFO_PAGE04_M_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // �l
#if USE_FRONTIER_DATA
  num = FrontierRecord_Get(p_fsave, FRID_STAGE_MULTI_WIFI_RENSHOU, p_data->idx.fridx[p_data->select_listidx]);
#else
  num = 147;
#endif
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 5, FINFO_PAGE04_BA,
      p_data, p_draw, msg_finfo_frprm_wnum,
      FINFO_PAGE04_MN_X, FINFO_PAGE04_M_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );

  FInfoDrawArea_MsgBmpOnVReq( p_wk, 5 );

  GFL_STR_DeleteBuffer( p_str );
  GFL_STR_DeleteBuffer( p_tmp );
}
static void FInfoDraw_Page05( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  STRBUF* p_str;
  STRBUF* p_tmp;
  u32 num;
  BOOL clear_flag;
  u32 stridx;
#if USE_FRONTIER_DATA
  FRONTIER_SAVEWORK* p_fsave;

  p_fsave = SaveData_GetFrontier( p_data->p_save );
#endif

  p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  // �^�C�g��
  FInfoDraw_Bmp( p_wk, 6, FINFO_PAGE05_BA,
      p_data, p_draw, msg_wifi_note_40,
      FINFO_PAGE05_TT_X, FINFO_PAGE05_TT_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );

  // �O��
#if USE_FRONTIER_DATA
  clear_flag = FrontierRecord_Get(p_fsave, FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT, p_data->idx.fridx[p_data->select_listidx]);
#else
  clear_flag = TRUE;
#endif
  if( clear_flag == FALSE ){
    stridx = msg_finfo_frprm_win01;
  }else{
    stridx = msg_finfo_frprm_win02;
  }
  FInfoDraw_Bmp( p_wk, 6, FINFO_PAGE05_BA,
      p_data, p_draw, stridx,
      FINFO_PAGE05_LT_X, FINFO_PAGE05_L_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // �l
#if USE_FRONTIER_DATA
  num = FrontierRecord_Get(p_fsave, FRID_ROULETTE_MULTI_WIFI_RENSHOU_CNT, p_data->idx.fridx[p_data->select_listidx]);
#else
  num = 258;
#endif
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 6, FINFO_PAGE05_BA,
      p_data, p_draw, msg_finfo_frprm_win03,
      FINFO_PAGE05_LN_X, FINFO_PAGE05_L_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );

  // �ō�
  FInfoDraw_Bmp( p_wk, 6, FINFO_PAGE05_BA,
      p_data, p_draw, msg_finfo_frprm_win03,
      FINFO_PAGE05_MT_X, FINFO_PAGE05_M_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // �l
#if USE_FRONTIER_DATA
  num = FrontierRecord_Get(p_fsave, FRID_ROULETTE_MULTI_WIFI_RENSHOU, p_data->idx.fridx[p_data->select_listidx]);
#else
  num = 369;
#endif
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 6, FINFO_PAGE05_BA,
      p_data, p_draw, msg_finfo_frprm_win03,
      FINFO_PAGE05_MN_X, FINFO_PAGE05_M_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );



  FInfoDrawArea_MsgBmpOnVReq( p_wk, 6 );

  GFL_STR_DeleteBuffer( p_str );
  GFL_STR_DeleteBuffer( p_tmp );
}
// �~�j�Q�[��
static void FInfoDraw_Page06( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  STRBUF* p_str;
  STRBUF* p_tmp;
  WIFI_LIST* p_wifilist;
  u32 num;
#if USE_FRONTIER_DATA
  FRONTIER_SAVEWORK* p_fsave;

  p_fsave = SaveData_GetFrontier( p_data->p_save );
#endif

  p_wifilist  = GAMEDATA_GetWiFiList(p_data->pGameData); //SaveData_GetWifiListData( p_data->p_save );
  p_str = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );
  p_tmp = GFL_STR_CreateBuffer( WFNOTE_STRBUF_SIZE, heapID );

  // �^�C�g��
  FInfoDraw_Bmp( p_wk, 1, FINFO_PAGE06_BA,
      p_data, p_draw, msg_wifi_note_52,
      FINFO_PAGE06_TT_X, FINFO_PAGE06_TT_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );

  // ���܂���
//  Draw_WflbyGameSetWordSet( p_draw, WFLBY_GAME_BALLSLOW );
  FInfoDraw_Bmp( p_wk, 1, FINFO_PAGE06_BA,
      p_data, p_draw, msg_wifi_note_53,
      FINFO_PAGE06_BC_X, FINFO_PAGE06_BC_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // �l
  num = 0;//WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_BALLSLOW_NUM );
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 1, FINFO_PAGE06_BA,
      p_data, p_draw, msg_wifi_note_56,
      FINFO_PAGE06_BC_N_X, FINFO_PAGE06_BC_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );

  // ���܂̂�
//  Draw_WflbyGameSetWordSet( p_draw, WFLBY_GAME_BALANCEBALL );
  FInfoDraw_Bmp( p_wk, 1, FINFO_PAGE06_BA,
      p_data, p_draw, msg_wifi_note_53,
      FINFO_PAGE06_BB_X, FINFO_PAGE06_BB_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // �l
  num = 0;//WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_BALANCEBALL_NUM );
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 1, FINFO_PAGE06_BA,
      p_data, p_draw, msg_wifi_note_56,
      FINFO_PAGE06_BB_N_X, FINFO_PAGE06_BB_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );

  // �ӂ�����
//  Draw_WflbyGameSetWordSet( p_draw, WFLBY_GAME_BALLOON );
  FInfoDraw_Bmp( p_wk, 1, FINFO_PAGE06_BA,
      p_data, p_draw, msg_wifi_note_53,
      FINFO_PAGE06_BL_X, FINFO_PAGE06_BL_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );
  // �l
  num =  0;//WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_BALLOON_NUM );
  Draw_NumberSetWordset( p_draw, num );
  FInfoDraw_Bmp( p_wk, 1, FINFO_PAGE06_BA,
      p_data, p_draw, msg_wifi_note_56,
      FINFO_PAGE06_BL_N_X, FINFO_PAGE06_BL_Y,
      WFNOTE_COL_BLACK, p_str, p_tmp );


  FInfoDrawArea_MsgBmpOnVReq( p_wk, 1 );

  GFL_STR_DeleteBuffer( p_str );
  GFL_STR_DeleteBuffer( p_tmp );
}

// �G���h�y�[�W
static void FInfoDraw_Page07( WFNOTE_FINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, HEAPID heapID )
{
  // �^�C�g��
  FInfoDraw_Bmp( p_wk, 7, FINFO_PAGE07_BA,
      p_data, p_draw, msg_finfo_backmsg,
      0, 0,
      WFNOTE_COL_BLACK, p_draw->p_str, p_draw->p_tmp );

  FInfoDrawArea_MsgBmpOnVReq( p_wk, 7 );
}
*/

//----------------------------------------------------------------------------
/**
 *  @brief  �\���ʂ̃N���A
 *
 *  @param  p_draw  �`��V�X�e��
 *  @param  cp_scrnarea �X�N���[���G���A
 */
//-----------------------------------------------------------------------------
static void FInfoDraw_Clean( WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_scrnarea )
{

  // �X�N���[���ʂ��N���A
  GFL_BG_FillScreen( DFRM_SCRMSG,
      0,
      MATH_ABS(cp_scrnarea->scrn_x), MATH_ABS(cp_scrnarea->scrn_y),
      32,
      24, 0 );
  GFL_BG_LoadScreenV_Req( DFRM_SCRMSG );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �r�b�g�}�b�v�ւ̕����̕`��
 *
 *  @param  p_wk    �`��G���A���[�N
 *  @param  page    �y�[�W
 *  @param  bmp     �r�b�g�}�b�v�C���f�b�N�X
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  msg_idx   ���b�Z�[�W�C���f�b�N�X
 *  @param  x     ��
 *  @param  y     ��
 *  @param  col     �J���[
 *  @param  p_str   �g�p����STRBUF
 *  @param  p_tmp   �g�p����STRBUF
 */
//-----------------------------------------------------------------------------
static void FInfoDraw_Bmp( WFNOTE_FINFO_DRAWAREA* p_wk, u32 page, u32 bmp, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 msg_idx, u32 x, u32 y, u32 col, STRBUF* p_str, STRBUF* p_tmp )
{
/*
  GFL_MSG_GetString( p_draw->p_msgman, msg_idx, p_tmp );
  WORDSET_ExpandStr( p_draw->p_wordset, p_str, p_tmp );
//  PRINTSYS_PrintQueColor( p_draw->printQue , GFL_BMPWIN_GetBmp(p_wk->p_msg[page][bmp]), x, y, p_str, p_draw->fontHandle , col );
  PRINTTOOL_PrintColor(
    p_wk->p_msg[page][bmp], p_draw->printQue,
    x, y, p_str, p_draw->fontHandle, col, PRINTTOOL_MODE_LEFT );
*/
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ڍ׏����ʁ@�r�b�g�}�b�v�ւ̕����̕`��
 *
 *  @param  p_wk    �`��G���A���[�N
 *  @param  page    �y�[�W
 *  @param  bmp     �r�b�g�}�b�v�C���f�b�N�X
 *  @param  p_data    �f�[�^
 *  @param  p_draw    �`��V�X�e��
 *  @param  msg_idx   ���b�Z�[�W�C���f�b�N�X
 *  @param  x     ��
 *  @param  y     ��
 *  @param  col     �J���[
 *  @param  p_str   �g�p����STRBUF
 *  @param  p_tmp   �g�p����STRBUF
 */
//-----------------------------------------------------------------------------
static void FInfoDraw_BaseBmp( PRINT_UTIL * util, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw,
        u32 msg_idx, u32 x, u32 y, u32 col )
{
  GFL_MSG_GetString( p_draw->p_msgman, msg_idx, p_draw->p_tmp );
  WORDSET_ExpandStr( p_draw->p_wordset, p_draw->p_str, p_draw->p_tmp );
  PRINTTOOL_PrintColor(
    util, p_draw->printQue, x, y, p_draw->p_str, p_draw->fontHandle, col, PRINTTOOL_MODE_LEFT );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �y�[�W�ύX
 *
 *  @param  p_wk    ���[�N
 *  @param  page    �y�[�W
 */
//-----------------------------------------------------------------------------
static void FInfo_PageChange( WFNOTE_FRIENDINFO* p_wk, s32 add )
{
  int page_num;

  // �t�����e�B�A��`�悷�邩�`�F�b�N
  if( p_wk->frontier_draw == TRUE ){
    page_num = FINFO_PAGE_NUM;
  }else{
    page_num = FINFO_BFOFF_PAGE_NUM;
  }

  if( add > 0 ){
    p_wk->lastpage = p_wk->page;
    p_wk->page = (p_wk->page + add) % page_num;
  }else if( add < 0 ){
    p_wk->lastpage = p_wk->page;
    p_wk->page = p_wk->page + add;
    if( p_wk->page < 0 ){
      p_wk->page += page_num;
    }
  }
}

#if NOTE_TEMP_COMMENT
//���g�p�BDP�EPt�p�H
//----------------------------------------------------------------------------
/**
 *  @brief  �|�t�B���P�[�X�����Ă��邩�`�F�b�N
 *
 *  @param  p_data  ���[�N
 */
//-----------------------------------------------------------------------------
static BOOL FInfo_PofinnCaseCheck( WFNOTE_DATA* p_data, HEAPID heapID )
{
  // �|���g�P�[�X�����邩�`�F�b�N
  if( !MyItem_GetItemNum( SaveData_GetMyItem(p_data->p_save),ITEM_POFINKEESU, heapID ) ){
    return FALSE;
  }
  return TRUE;
}
#endif  //NOTE_TEMP_COMMENT

//-------------------------------------
/// �ڐA�p
//=====================================

//------------------------------------------------------------------
/**
 * ���������𐔎��ɕϊ�����  18�����Ή�  ���̊֐��͕����R�[�h�ɍ��E�����
 *
 * @param src     �ϊ������������������Ă���o�b�t�@
 * @param pbFlag    �ϊ��ł������ǂ���
 * @return  ���������ꍇ�ϊ�����
 *
 *  GS�ł�WFNOTE���ł����g���Ă��Ȃ��̂ł����ɈڐA
 *  ���[�`���������R�[�h����UNICODE�ɕς�����̂ŕύX
 */
//------------------------------------------------------------------
static u64 WFNOTE_STRBUF_GetNumber( STRBUF *str , BOOL *pbFlag )
{
  const STRCODE eonCode = GFL_STR_GetEOMCode();
  const STRCODE *code = GFL_STR_GetStringCodePointer( str );

  u64 num = 0;
  u64 digit = 1;
  u16 arrDigit = 0;
  u16 tempNo;

  static const u8 MAX_DIGIT = 18;

  *pbFlag = FALSE;
  while( code[arrDigit] != eonCode )
  {
    if( arrDigit >= MAX_DIGIT )
    {
      //���I�[�o�[
      return num;
    }

    tempNo = code[arrDigit] - L'0';
    if( tempNo >= 10 )
    {
      tempNo = code[arrDigit] - L'�O';
      if( tempNo >= 10 )
      {
        //�z��O�̕����������Ă���
        return num;
      }
    }
    /*
    num += tempNo * digit;
    digit *= 10;
    */
    num *= 10;
    num += tempNo;
    arrDigit++;
  }
  *pbFlag = TRUE;
  return num;
}





//============================================================================================
//  ���ǉ������@2010/01/18 by nakahiro
//============================================================================================

static BOOL CheckUITrg( WFNOTE_DATA * p_data )
{
  if( GFL_UI_TP_GetTrg() == TRUE ){
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    p_data->key_mode = GFL_APP_END_TOUCH;
    return TRUE;
  }
  if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    p_data->key_mode = GFL_APP_END_KEY;
    return TRUE;
  }
  return FALSE;
}


#define SCRN_MODESEL_BTN_PX   ( 2 )
#define SCRN_MODESEL_BTN_PY   ( 5 )
#define SCRN_MODESEL_BTN_SX   ( 28 )
#define SCRN_MODESEL_BTN_SY   ( 4 )
#define SCRN_MODESEL_BTN_OY   ( 5 )

#define SCRN_MODELIST_BTN_PX    ( 2 )
#define SCRN_MODELIST_BTN_PY    ( 4 )
#define SCRN_MODELIST_BTN_SX    ( 14 )
#define SCRN_MODELIST_BTN_SY    ( 4 )
#define SCRN_MODELIST_BTN_OX    ( 14 )
#define SCRN_MODELIST_BTN_OY    ( 4 )

//--------------------------------------------------------------------------------------------
/**
 * @brief   �I�𒆃J�[�\���A�j���쐬
 *
 * @param   p_draw    ���[�N
 * @param   heapID    �q�[�v�h�c
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void Draw_BlinkAnmInit( WFNOTE_DRAW * p_draw, HEAPID heapID )
{
  p_draw->bpawk = BLINKPALANM_Create( BGPLT_M_BACK_3*16, 16, GFL_BG_FRAME0_M, heapID );
  BLINKPALANM_SetPalBufferArcHDL(
    p_draw->bpawk, p_draw->p_handle, NARC_wifi_note_friend_bg_d_NCLR, BGPLT_M_BACK_3*16, BGPLT_M_BACK_2*16 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �I�𒆃J�[�\���A�j���폜
 *
 * @param   p_draw    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void Draw_BlinkAnmExit( WFNOTE_DRAW * p_draw )
{
  BLINKPALANM_Exit( p_draw->bpawk );
}

//--------------------------------------------------------------------------------------------
//  �a�f�{�^��
//--------------------------------------------------------------------------------------------
/*
  BGPLT_M_BACK_1,         // �{�^���i�ʏ펞�j
  BGPLT_M_BACK_2,         // �{�^���i�I�����̃O���f�p�P�j
  BGPLT_M_BACK_3,         // �{�^���i�I�����̃O���f�p�P�j
  BGPLT_M_BACK_4,         // �{�^���i���ڂȂ��p�j
*/

static void SetTopPageButton( u32 pos, u32 pal )
{
  GFL_BG_ChangeScreenPalette(
    DFRM_SCROLL,
    SCRN_MODESEL_BTN_PX, SCRN_MODESEL_BTN_PY+SCRN_MODESEL_BTN_OY*pos,
    SCRN_MODESEL_BTN_SX, SCRN_MODESEL_BTN_SY, pal );
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
}

static void SetTopPageButtonPassive( WFNOTE_DRAW * p_draw, u32 pos )
{
  SetTopPageButton( pos, BGPLT_M_BACK_1 );
  BLINKPALANM_InitAnimeCount( p_draw->bpawk );
}

static void SetTopPageButtonActive( WFNOTE_DRAW * p_draw, u32 pos )
{
  SetTopPageButton( pos, BGPLT_M_BACK_3 );
  BLINKPALANM_InitAnimeCount( p_draw->bpawk );
}

static void ChangeTopPageButton( WFNOTE_DRAW * p_draw, u32 now, u32 old )
{
  SetTopPageButtonPassive( p_draw, old );
  SetTopPageButtonActive( p_draw, now );
}

static void ResetTopPageButton( WFNOTE_MODESELECT * p_wk )
{
  u32 i;

  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
    for( i=0; i<3; i++ ){
      SetTopPageButton( i, BGPLT_M_BACK_1 );
    }
  }else{
    for( i=0; i<3; i++ ){
      if( i == p_wk->cursor ){
        SetTopPageButton( i, BGPLT_M_BACK_3 );
      }else{
        SetTopPageButton( i, BGPLT_M_BACK_1 );
      }
    }
  }
}

static BOOL MainTopPageButtonDecedeAnime( WFNOTE_MODESELECT * p_wk )
{
  switch( p_wk->btn_anmSeq ){
  case 0:
    SetTopPageButton( p_wk->cursor, BGPLT_M_BACK_1 );
    p_wk->btn_anmCnt = 0;
    p_wk->btn_anmSeq++;
    break;
  case 1:
    p_wk->btn_anmCnt++;
    if( p_wk->btn_anmCnt == 4 ){
      SetTopPageButton( p_wk->cursor, BGPLT_M_BACK_2 );
      p_wk->btn_anmCnt = 0;
      p_wk->btn_anmSeq++;
    }
    break;
  case 2:
    p_wk->btn_anmCnt++;
    if( p_wk->btn_anmCnt == 4 ){
      SetTopPageButton( p_wk->cursor, BGPLT_M_BACK_1 );
      p_wk->btn_anmCnt = 0;
      p_wk->btn_anmSeq++;
    }
    break;
  case 3:
    p_wk->btn_anmCnt++;
    if( p_wk->btn_anmCnt == 4 ){
      SetTopPageButton( p_wk->cursor, BGPLT_M_BACK_2 );
      p_wk->btn_anmCnt = 0;
      p_wk->btn_anmSeq++;
    }
    break;
  case 4:
    p_wk->btn_anmCnt++;
    if( p_wk->btn_anmCnt == 4 ){
      p_wk->btn_anmSeq = 0;
      p_wk->btn_anmCnt = 0;
      return FALSE;
    }
    break;
  }

  return TRUE;
}



static void SetListPageButton( u32 pos, u32 pal )
{
  GFL_BG_ChangeScreenPalette(
    DFRM_SCROLL,
    SCRN_MODELIST_BTN_PX+SCRN_MODELIST_BTN_OX*(pos/4),
    SCRN_MODELIST_BTN_PY+SCRN_MODELIST_BTN_OY*(pos%4),
    SCRN_MODELIST_BTN_SX, SCRN_MODELIST_BTN_SY, pal );
  GFL_BG_LoadScreenV_Req( DFRM_SCROLL );
}

static void SetListPageButtonPassive( WFNOTE_DRAW * p_draw, u32 pos )
{
  SetListPageButton( pos, BGPLT_M_BACK_1 );
  BLINKPALANM_InitAnimeCount( p_draw->bpawk );
}

static void SetListPageButtonActive( WFNOTE_DRAW * p_draw, u32 pos )
{
  SetListPageButton( pos, BGPLT_M_BACK_3 );
  BLINKPALANM_InitAnimeCount( p_draw->bpawk );
}

static void ChangeListPageButton( WFNOTE_DRAW * p_draw, u32 now, u32 old )
{
  SetListPageButtonPassive( p_draw, old );
  SetListPageButtonActive( p_draw, now );
}

static void ResetListPageButton( WFNOTE_FRIENDLIST * p_wk )
{
  u32 i;

  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
    for( i=0; i<8; i++ ){
      SetListPageButton( i, BGPLT_M_BACK_1 );
    }
  }else{
    for( i=0; i<8; i++ ){
      if( i == p_wk->pos ){
        SetListPageButton( i, BGPLT_M_BACK_3 );
      }else{
        SetListPageButton( i, BGPLT_M_BACK_1 );
      }
    }
  }
}

static BOOL MainListPageButtonDecedeAnime( WFNOTE_FRIENDLIST * p_wk )
{
  switch( p_wk->btn_anmSeq ){
  case 0:
    SetListPageButton( p_wk->pos, BGPLT_M_BACK_1 );
    p_wk->btn_anmCnt = 0;
    p_wk->btn_anmSeq++;
    break;
  case 1:
    p_wk->btn_anmCnt++;
    if( p_wk->btn_anmCnt == 4 ){
      SetListPageButton( p_wk->pos, BGPLT_M_BACK_2 );
      p_wk->btn_anmCnt = 0;
      p_wk->btn_anmSeq++;
    }
    break;
  case 2:
    p_wk->btn_anmCnt++;
    if( p_wk->btn_anmCnt == 4 ){
      SetListPageButton( p_wk->pos, BGPLT_M_BACK_1 );
      p_wk->btn_anmCnt = 0;
      p_wk->btn_anmSeq++;
    }
    break;
  case 3:
    p_wk->btn_anmCnt++;
    if( p_wk->btn_anmCnt == 4 ){
      SetListPageButton( p_wk->pos, BGPLT_M_BACK_2 );
      p_wk->btn_anmCnt = 0;
      p_wk->btn_anmSeq++;
    }
    break;
  case 4:
    p_wk->btn_anmCnt++;
    if( p_wk->btn_anmCnt == 4 ){
      p_wk->btn_anmSeq = 0;
      p_wk->btn_anmCnt = 0;
      return FALSE;
    }
    break;
  }

  return TRUE;
}


//--------------------------------------------------------------------------------------------
//  �߂�{�^��
//--------------------------------------------------------------------------------------------

static void SetCancelButtonAnime( WFNOTE_DRAW * p_draw )
{
  GFL_CLACT_WK_SetAnmFrame( p_draw->p_tb, 0 );
  GFL_CLACT_WK_SetAnmSeq( p_draw->p_tb, APP_COMMON_BARICON_RETURN_ON );
  GFL_CLACT_WK_SetAutoAnmFlag( p_draw->p_tb, TRUE );
}

static void SetExitButtonAnime( WFNOTE_DRAW * p_draw )
{
  GFL_CLACT_WK_SetAnmFrame( p_draw->p_tbx, 0 );
  GFL_CLACT_WK_SetAnmSeq( p_draw->p_tbx, APP_COMMON_BARICON_EXIT_ON );
  GFL_CLACT_WK_SetAutoAnmFlag( p_draw->p_tbx, TRUE );
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#define DISP_PASSIVE_BLEND_EV1  ( 10 )
#define DISP_PASSIVE_BLEND_EV2  ( 6 )


static void SetDispActive( WFNOTE_DRAW * p_draw, BOOL flg )
{
  if( flg == TRUE ){
    GFL_CLACT_WK_SetObjMode( p_draw->p_tb, GX_OAM_MODE_XLU );
    GFL_CLACT_WK_SetObjMode( p_draw->p_tbx, GX_OAM_MODE_XLU );
    G2_SetBlendAlpha(
      GX_BLEND_PLANEMASK_BG2,
      GX_BLEND_PLANEMASK_BG0 |
      GX_BLEND_PLANEMASK_BG2,
      DISP_PASSIVE_BLEND_EV1,
      DISP_PASSIVE_BLEND_EV2 );
  }else{
    GFL_CLACT_WK_SetObjMode( p_draw->p_tb, GX_OAM_MODE_NORMAL );
    GFL_CLACT_WK_SetObjMode( p_draw->p_tbx, GX_OAM_MODE_NORMAL );
    G2_BlendNone();
  }
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
static void ChangeListPageNumAnime( WFNOTE_FRIENDLIST * p_wk, u32 page )
{
  GFL_CLACT_WK_SetAnmFrame( p_wk->p_pageact, 0 );
  GFL_CLACT_WK_SetAnmSeq( p_wk->p_pageact, ACTANM_PAGE_NUM1+page );
}

static void PutListTitle( WFNOTE_DRAW * p_draw )
{
  GFL_BMPWIN_MakeScreen( p_draw->title.win );
  GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(p_draw->title.win) );
}

static void SetListDispActive( WFNOTE_FRIENDLIST * p_wk, WFNOTE_DRAW * p_draw, BOOL flg )
{
  WFNOTE_FLIST_DRAWAREA * da;
  u32 i;

  da = &p_wk->drawdata[WFNOTE_DRAWAREA_MAIN];

  if( flg == TRUE ){
    GFL_CLACT_WK_SetObjMode( p_wk->p_pageact, GX_OAM_MODE_XLU );
    GFL_CLACT_WK_SetObjMode( p_draw->p_tb, GX_OAM_MODE_XLU );
    GFL_CLACT_WK_SetObjMode( p_draw->p_tbx, GX_OAM_MODE_XLU );
    for( i=0; i<FLIST_PAGE_FRIEND_NUM; i++ ){
      if( da->p_hate[i] == NULL ){
        GFL_CLACT_WK_SetObjMode( WF_2DC_WkClWkGet(da->p_clwk[i]), GX_OAM_MODE_XLU );
      }else{
        GFL_CLACT_WK_SetObjMode( da->p_hate[i], GX_OAM_MODE_XLU );
      }
    }
    G2_SetBlendAlpha(
      GX_BLEND_PLANEMASK_BG2,
      GX_BLEND_PLANEMASK_BG0 |
      GX_BLEND_PLANEMASK_BG2,
      DISP_PASSIVE_BLEND_EV1,
      DISP_PASSIVE_BLEND_EV2 );
  }else{
    GFL_CLACT_WK_SetObjMode( p_wk->p_pageact, GX_OAM_MODE_NORMAL );
    GFL_CLACT_WK_SetObjMode( p_draw->p_tb, GX_OAM_MODE_NORMAL );
    GFL_CLACT_WK_SetObjMode( p_draw->p_tbx, GX_OAM_MODE_NORMAL );
    for( i=0; i<FLIST_PAGE_FRIEND_NUM; i++ ){
      if( da->p_hate[i] == NULL ){
        GFL_CLACT_WK_SetObjMode( WF_2DC_WkClWkGet(da->p_clwk[i]), GX_OAM_MODE_NORMAL );
      }else{
        GFL_CLACT_WK_SetObjMode( da->p_hate[i], GX_OAM_MODE_NORMAL );
      }
    }
    G2_BlendNone();
  }
}

// �o�^������
static void InitPrintUtil( WFNOTE_DRAW * p_draw )
{
  u32 i;

  for( i=0; i<NOTE_PRINTUTIL_MAX; i++ ){
    p_draw->printUtil[i] = NULL;
  }
}

// �Z�b�g
static void SetPrintUtil( WFNOTE_DRAW * p_draw, PRINT_UTIL * util )
{
  u32 i;

  for( i=0; i<NOTE_PRINTUTIL_MAX; i++ ){
    if( p_draw->printUtil[i] == NULL ){
      p_draw->printUtil[i] = util;
      break;
    }
  }
}

// �]��
static void TransPrintUtil( WFNOTE_DRAW * p_draw )
{
  u32 i;

  for( i=0; i<NOTE_PRINTUTIL_MAX; i++ ){
    if( p_draw->printUtil[i] == NULL ){ break; }
    PRINT_UTIL_Trans( p_draw->printUtil[i], p_draw->printQue );
  }
}

// �a�l�o�v�h�m�X�N���[���]��
static void PutBmpWin( GFL_BMPWIN * win )
{
  GFL_BMPWIN_MakeScreen( win );
  GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(win) );
}

// �g���[�i�[�O���t�B�b�N�n�a�i�폜
static void FInfo_TrGraDelete( WFNOTE_FRIENDINFO * p_wk )
{
  GFL_CLGRP_CGR_Release( p_wk->trRes[0] );
  GFL_CLGRP_PLTT_Release( p_wk->trRes[1] );
  GFL_CLGRP_CELLANIM_Release( p_wk->trRes[2] );
  GFL_CLACT_WK_Remove( p_wk->p_trgra );
}
