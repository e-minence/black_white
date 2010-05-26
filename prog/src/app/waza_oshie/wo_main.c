//============================================================================================
/**
 * @file  wo_main.c
 * @brief �Z����/�v���o�������@���C��
 * @author  Hiroyuki Nakamura / Akito Mori(WB�Ɉ��z���j
 * @date  109.10.13
 */
//============================================================================================
#include <gflib.h>


#include "system/gfl_use.h"   //GFUser_VIntr_CreateTCB
#include "system/main.h"      //GFL_HEAPID_APP�Q��
#include "net/network_define.h"
#include "arc_def.h"

#include "item/item.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"
#include "gamesystem/msgspeed.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"

#include "message.naix"
#include "bag.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_waza_oboe.h"
#include "font/font.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/gfl_use.h"

#include "system/wipe.h"
#include "system/bmp_menu.h"

#include "system/cursor_move.h"
#include "sound/pm_sndsys.h"

#include "savedata/config.h"
#include "app/app_keycursor.h"      // APP_KEY_CURSOR
#include "app/app_menu_common.h"
#include "waza_tool/wazadata.h"
#include "app/p_status.h"
#include "app/app_taskmenu.h"
#include "system/poke2dgra.h"

#include "app/waza_oshie.h"
#include "wo_bmp_def.h"
#include "wo_snd_def.h"
#include "waza_oshie_gra.naix"


FS_EXTERN_OVERLAY( poke_status );

//============================================================================================
//  �萔��`
//============================================================================================

#define MFRM_POKE     (GFL_BG_FRAME0_M)
#define MFRM_PARAM    (GFL_BG_FRAME2_M)
#define MFRM_BACK     (GFL_BG_FRAME3_M)

#define SFRM_MSG      (GFL_BG_FRAME0_S)
#define SFRM_TOUCHBAR (GFL_BG_FRAME1_S)
#define SFRM_PARAM    (GFL_BG_FRAME2_S)
#define SFRM_BACK     (GFL_BG_FRAME3_S)

#define MFRM_POKE_PRI (1)
#define MFRM_PARAM_PRI  (2)
#define MFRM_BACK_PRI (3)
#define SFRM_MSG_PRI  (0)
#define SFRM_PARAM_PRI  (2)
#define SFRM_BACK_PRI (3)


enum {
  WIN_SPACE_M=0,
  WIN_SPACE_S,
  WIN_STR_ATTACK,     // �u����傭�v
  WIN_STR_HIT,      // �u�߂����イ�v
  WIN_PRM_ATTACK,     // �З͒l
  WIN_PRM_HIT,      // �����l
  WIN_BTL_INFO,     // �퓬����
  WIN_MSG,        // ���b�Z�[�W�E�B���h�E
  WIN_EXP,        // �����E�B���h�E
  WIN_ABTN,       // ����{�^���E�B���h�E
  WIN_BACK,       // �߂�{�^���E�B���h�E
  WIN_LIST,       // �Z���X�g
  WIN_YESNO,        // �͂�/������

  WIN_TITLE,        // �^�C�g��
  WIN_MWAZA,        // ��@�Z���X�g
  WIN_MPRM,         // ��@�|�P�������ƃ��x��

  WIN_MAX
};

enum {
  WO_CLA_ARROW_D = 0,     // ���X�g�J�[�\����
  WO_CLA_ARROW_U,     // ���X�g�J�[�\����
  WO_CLA_CURSOR,      // �I���J�[�\��
  WO_CLA_KIND,      // ���ރA�C�R��
  WO_CLA_TYPE1,     // �^�C�v�A�C�R���P
  WO_CLA_TYPE2,     // �^�C�v�A�C�R���Q
  WO_CLA_TYPE3,     // �^�C�v�A�C�R���R
  WO_CLA_TYPE4,     // �^�C�v�A�C�R���S

  WO_CLA_TYPE5,     // �^�C�v�A�C�R���P
  WO_CLA_TYPE6,     // �^�C�v�A�C�R���Q
  WO_CLA_TYPE7,     // �^�C�v�A�C�R���R
  WO_CLA_TYPE8,     // �^�C�v�A�C�R���S

  WO_CLA_POKEGRA,   // �|�P�������ʊG
  WO_CLA_EXIT,      // �߂�{�^��

  WO_CLA_MAX
};

enum{
  WO_ANIME_OBJ=0,   ///< OBJ�A�j����҂�
  WO_ANIME_APPTASK, ///< APPTASKUMENU�̃A�j����҂�
};

typedef struct _WO_3DWORK{
  GFL_G3D_CAMERA *camera;
  void*     ssm;

//  SOFT_SPRITE_ARC ssa;  ///<�\�t�g�E�F�A�X�v���C�g  // @@@softsprite
//  SOFT_SPRITE * ss; // @@@softsprite
}WO_3DWORK;

// �{�^���A�j��
typedef struct {
  u8  px;
  u8  py;
  u8  sx;
  u8  sy;
  u16 cnt;
  u16 index;
  u16 next;
  u16 mode;
}BUTTON_ANM_WORK;

typedef struct {
  WAZAOSHIE_DATA * dat;     // �O���ݒ�f�[�^

  GFL_BMPWIN  *win[WIN_MAX];  // BMP�E�B���h�E�f�[�^

  GFL_MSGDATA * mman;   // ���b�Z�[�W�f�[�^�}�l�[�W��

  WORDSET * wset;       // �P��Z�b�g
  STRBUF * mbuf;        // �ėp������W�J�̈�
  STRBUF * pp1_str;     // PP�p������W�J�̈�(PP/MPP)
  STRBUF * pp2_str;     // PP�p������W�J�̈�(PP/MPP)

  BMP_MENULIST_DATA * ld;   // BMP���X�g�f�[�^

  BMPMENU_WORK * mw;      // BMP���j���[���[�N

  GFL_FONT     *fontHandle;  // �t�H���g�n���h��
  GFL_TCBLSYS  *pMsgTcblSys;  // ���b�Z�[�W�\���p�^�X�NSYS
  GFL_TCB      *vBlankTcb;
  PRINT_STREAM *printStream;

  u32 clres[4][WO_CHR_ID_MAX];  // �Z���A�N�^�[���\�[�X�C���f�b�N�X�p���[�N

  // �Z���A�N�^�[
  GFL_CLUNIT    *clUnit;
//  CATS_RES_PTR  crp;
  GFL_CLWK    *cap[WO_CLA_MAX];

  WO_3DWORK   p3d;

  PSTATUS_DATA  psd;
  GFL_PROC * subProc;
/*��[GS_CONVERT_TAG]*/

  int seq;          // ���݂̃V�[�P���X
  int next_seq;     // ���̃V�[�P���X
  int key_mode;

  u8  sel_max;

  u8  midx;         // ���b�Z�[�W�C���f�b�N�X
  u8  ynidx;          // �͂��E������ID

  u8  cpos_x;       // �J�[�\���|�W�V����
  u8  cpos_y;       // �J�[�\���|�W�V����
  u8  cur_pos;        // �J�[�\���|�W�V����

  void* pSBufDParts;
  NNSG2dScreenData* pSDParts;

  CURSORMOVE_WORK * cmwk; // �J�[�\���ړ����[�N
  u16 cm_pos;         // �J�[�\���ړ��̌��݂̈ʒu
  u16 enter_flg;        //�u���ڂ���v�\���t���O

  BUTTON_ANM_WORK button_anm_work;

  // APPMENU�֘A
  APP_TASKMENU_RES   *app_res;
  APP_TASKMENU_WORK  *app_menuwork;
  APP_TASKMENU_ITEMWORK menuitem[2];
  APP_TASKMENU_ITEMWORK yn_menuitem[2];
  PRINT_UTIL            *printUtil;
  PRINT_QUE             *printQue;
  APP_KEYCURSOR_WORK *printCursor;
  APP_TASKMENU_WIN_WORK *oboe_menu_work[2];
}WO_WORK;


typedef int (*WO_SEQ_FUNC)(WO_WORK*);

// �͂��E����������
typedef struct {
  WO_SEQ_FUNC yes;  // �͂�
  WO_SEQ_FUNC no;   // ������
}YESNO_FUNC;

// �y�[�W
enum {
  WO_PAGE_BATTLE = 0, // �퓬
  WO_PAGE_CONTEST   // �R���e�X�g
};

// ���C���V�[�P���X
enum {
  SEQ_FADE_WAIT = 0,  // �t�F�[�h�I���҂�
  SEQ_SELECT,     // �Z�Z���N�g
  SEQ_MSG_WAIT,   // ���b�Z�[�W�I���҂�
  SEQ_YESNO_PUT,    // �͂��E�������Z�b�g
  SEQ_YESNO_WAIT,   // �͂��E�������I��҂�
  SEQ_WAZA_SET,   // �Z�o��
  SEQ_WAZADEL_SET,  // �Y��ċZ�o��
  SEQ_DEL_CHECK,    // �Y�ꂳ����Z���`�F�b�N
  SEQ_FADEOUT_SET,  // �t�F�[�h�A�E�g�Z�b�g
  SEQ_END,      // �I��

  SEQ_PST_INIT,   // �X�e�[�^�X��ʂ�
  SEQ_PST_CALL,   // �X�e�[�^�X��ʌĂяo��
  SEQ_PST_WAIT,   // �X�e�[�^�X��ʏI���҂�

  SEQ_BUTTON_ANM,   // �{�^���A�j��
  SEQ_RET_BUTTON,   //�u���ǂ�v
  SEQ_ENTER_BUTTON, //�u���ڂ���v
};

//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void WO_DispInit( WO_WORK * wk );
static void WO_DispExit( WO_WORK * wk );

static void WO_VBlank( GFL_TCB *tcb, void * work );
static void WO_VramBankSet(void);
static void WO_BgSet( void);
static void WO_BgExit( void );
static void WO_BgGraphicSet( WO_WORK * wk, ARCHANDLE* p_handle );
static void WO_BgGraphicRelease( WO_WORK * wk );
static void WO_BmpWinSet( WO_WORK * wk );
static void WO_BmpWinExit( WO_WORK * wk );
static void WO_MsgCreate( WO_WORK * wk );
static void WO_MsgRelease( WO_WORK * wk );
static int WO_SeqFadeWait( WO_WORK * wk );

static int WO_SeqSelectDecide(WO_WORK* wk);
static void WO_InputModeChange( WO_WORK * wk );
static int WO_SeqSelectInputCheck( WO_WORK * wk );

static int WO_SeqSelect( WO_WORK * wk );
static int WO_SeqFadeOutSet( WO_WORK * wk );
static int WO_SeqMsgWait( WO_WORK * wk );
static int WO_SeqYesNoPut( WO_WORK * wk );
static int WO_SeqYesNoWait( WO_WORK * wk );
static int WO_SeqWazaSet( WO_WORK * wk );
static int WO_SeqWazaDelSet( WO_WORK * wk );
static int WO_SeqDelCheck( WO_WORK * wk );
static int WO_SeqPstInit( WO_WORK * wk );
static int WO_SeqPstCall( WO_WORK * wk );
static int WO_SeqPstWait( WO_WORK * wk );

static void StrPut( WO_WORK * wk, u32 widx, GFL_FONT *font, PRINTSYS_LSB col, u32 mode, u8 py );
static void WO_PageChange( WO_WORK * wk );
static void WO_DefStrWrite( WO_WORK * wk );
static void WO_WazaListDraw( WO_WORK * wk );
static void WO_WazaListMake( WO_WORK * wk );
static void WO_WazaListExit( WO_WORK * wk );
static void WO_AButtonDraw( WO_WORK * wk );

static void BattleWazaParamPut( WO_WORK * wk, u32 prm );

static void WO_MsgMake( WO_WORK * wk, u32 id );
static void WO_TalkMsgSet( WO_WORK * wk, u32 id );
static void WO_TalkMsgOff( WO_WORK * wk);
static BOOL WO_TalkMsgCallBack( u32 value );
static u16 WO_SelWazaGet( WO_WORK * wk );
static u16 WO_DelWazaGet( WO_WORK * wk );
static u8 WO_WazaSetCheck( WO_WORK * wk );

static int WO_SetCheck_Yes( WO_WORK * wk );
static int WO_SetCheck_No( WO_WORK * wk );
static int WO_EndCheck_Yes( WO_WORK * wk );
static int WO_EndCheck_No( WO_WORK * wk );
static int WO_ChgCheck_Yes( WO_WORK * wk );
static int WO_ChgCheck_No( WO_WORK * wk );
static int WO_DelCheck_Yes( WO_WORK * wk );
static int WO_DelCheck_No( WO_WORK * wk );
static int WO_DelEnter_Yes( WO_WORK * wk );
static int WO_DelEnter_No( WO_WORK * wk );

static void WO_ObjInit( WO_WORK * wk, ARCHANDLE* p_handle );
static void WO_ObjFree( WO_WORK * wk );
static void WO_3DInit( WO_WORK * wk );
static void WO_3DMain(WO_3DWORK* wk);
static void WO_3DRelease( WO_3DWORK * wk );
static void WO_TypeIconChange( WO_WORK * wk, u16 waza, u16 res_offset );
static void WO_TypeIconInit( WO_WORK * wk );
static void WO_TypeIconScroll( WO_WORK * wk, u16 old_scr, u16 new_scr );
static void WO_KindIconChange( WO_WORK * wk, u16 waza );
static void WO_SelCursorChange( WO_WORK * wk, u8 pos, u8 pal );
static void WO_ScrollCursorPut( WO_WORK * wk ,u8 idx,BOOL anm_f);
//static void WO_ScrollCursorOff( WO_WORK * wk );

static void WO_SubBGPartsDraw( WO_WORK * wk ,u8 px,u8 py,u8 sx, u8 sy,u8 ox,u8 oy);

static void CursorMoveInit( WO_WORK * wk );
static void CursorMoveExit( WO_WORK * wk );

static void ScrollButtonOnOff( WO_WORK * wk );
static void EnterButtonOnOff( WO_WORK * wk, BOOL flg );

static int  RetButtonAnmInit( WO_WORK * wk, int next );
static int  EnterButtonAnmInit( WO_WORK * wk, int next );
static int  YameruButtonAnmInit( WO_WORK *wk, int next );
static int  ButtonAnmMaun( WO_WORK * wk );
static int  WO_SeqRetButton( WO_WORK * wk );
static int  WO_SeqEnterButton( WO_WORK * wk );
static void WazaSelBgChange( WO_WORK * wk, u32 pos );
static int  WazaSelectEnter( WO_WORK * wk );
static void PassiveSet( BOOL flg );
static void wazaoshie_3d_setup( void );
static void WO_AppTaskMenuMain( WO_WORK *wk );
static int  EnterButtonCheck( WO_WORK *wk );
static void ScrollButtonAnmChange( WO_WORK * wk, s32 mv );
static void ScrollButtonInit( WO_WORK *wk );



typedef struct{
  int frm, x, y, w, h, pal, offset;
}BMPWIN_DAT;

const GFL_PROC_DATA WazaOshieProcData = {
  WazaOshieProc_Init,
  WazaOshieProc_Main,
  WazaOshieProc_End,
};


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================
// BMP�E�B���h�E�f�[�^
static const BMPWIN_DAT BmpWinData[] =
{

  { // ���C���ʗp�P�L����������
    MFRM_PARAM, 0, 25,
    1, 1, 0, 0
  },
  { // �T�u�ʗp�P�L����������WIN_SPACE_M
    SFRM_PARAM, 0,25,
    1, 1, 0, 0
  },

  { // �u����傭�v
    SFRM_PARAM, WIN_STR_ATTACK_PX, WIN_STR_ATTACK_PY,
    WIN_STR_ATTACK_SX, WIN_STR_ATTACK_SY, WIN_STR_ATTACK_PAL, WIN_STR_ATTACK_CGX
  },
  { // �u�߂����イ�v
    SFRM_PARAM, WIN_STR_HIT_PX, WIN_STR_HIT_PY,
    WIN_STR_HIT_SX, WIN_STR_HIT_SY, WIN_STR_HIT_PAL, WIN_STR_HIT_CGX
  },
  { // �З͒l
    SFRM_PARAM, WIN_PRM_ATTACK_PX, WIN_PRM_ATTACK_PY,
    WIN_PRM_ATTACK_SX, WIN_PRM_ATTACK_SY, WIN_PRM_ATTACK_PAL, WIN_PRM_ATTACK_CGX
  },
  { // �����l
    SFRM_PARAM, WIN_PRM_HIT_PX, WIN_PRM_HIT_PY,
    WIN_PRM_HIT_SX, WIN_PRM_HIT_SY, WIN_PRM_HIT_PAL, WIN_PRM_HIT_CGX
  },
  { // �퓬����
    SFRM_PARAM, WIN_BTL_INFO_PX, WIN_BTL_INFO_PY,
    WIN_BTL_INFO_SX, WIN_BTL_INFO_SY, WIN_BTL_INFO_PAL, WIN_BTL_INFO_CGX
  },
  { // ���b�Z�[�W�E�B���h�E
    SFRM_MSG, WIN_MSG_PX, WIN_MSG_PY,
    WIN_MSG_SX, WIN_MSG_SY, WIN_MSG_PAL, WIN_MSG_CGX,
  },
  { // ��������E�B���h�E
    SFRM_PARAM, WIN_EXP_PX, WIN_EXP_PY,
    WIN_EXP_SX, WIN_EXP_SY, WIN_EXP_PAL, WIN_EXP_CGX,
  },
  { // A�{�^��
    SFRM_PARAM, WIN_ABTN_PX, WIN_ABTN_PY,
    WIN_ABTN_SX, WIN_ABTN_SY, WIN_ABTN_PAL, WIN_ABTN_CGX
  },
  { // �߂�{�^���E�B���h�E
    SFRM_PARAM, WIN_BACK_PX, WIN_BACK_PY,
    WIN_BACK_SX, WIN_BACK_SY, WIN_BACK_PAL, WIN_BACK_CGX,
  },
  { // �Z���X�g
    SFRM_PARAM, WIN_LIST_PX, WIN_LIST_PY,
    WIN_LIST_SX, WIN_LIST_SY, WIN_LIST_PAL, WIN_LIST_CGX
  },
  { // �͂�/������
    SFRM_MSG, WIN_YESNO_PX, WIN_YESNO_PY,
    WIN_YESNO_SX, WIN_YESNO_SY, WIN_YESNO_PAL, WIN_YESNO_CGX
  },

  { // �^�C�g��
    MFRM_PARAM, WIN_TITLE_PX, WIN_TITLE_PY,
    WIN_TITLE_SX, WIN_TITLE_SY, WIN_TITLE_PAL, WIN_TITLE_CGX
  },
  { // �Z���X�g
    MFRM_PARAM, WIN_MWAZA_PX, WIN_MWAZA_PY,
    WIN_MWAZA_SX, WIN_MWAZA_SY, WIN_MWAZA_PAL, WIN_MWAZA_CGX
  },
  { // �p�����[�^
    MFRM_PARAM, WIN_MPRM_PX, WIN_MPRM_PY,
    WIN_MPRM_SX, WIN_MPRM_SY, WIN_MPRM_PAL, WIN_MPRM_CGX
  },
};

// ���b�Z�[�W�e�[�u��
static const u32 MsgDataTbl[2][11] =
{
  { // �Z����
    msg_waza_oboe_01, // @0�Ɂ@�ǂ̂킴���@���ڂ�������H
    msg_waza_oboe_02, // @1�@��  ���ڂ������܂����H
    msg_waza_oboe_03, // @0�Ɂ@�킴��  ���ڂ�������̂��@������߂܂����H
    msg_waza_oboe_04, // @0�́@�����炵���@@1���@���ڂ����I��"
    msg_waza_oboe_05, // @0�́@�����炵���@@1���@���ڂ������c�c���@�������@�`
    msg_waza_oboe_06, // �P�@�Q�́c�c�@�|�J���I���@@0�́@@1�� �����������@�`
    msg_waza_oboe_07, // @0�́@�����炵�� @1���@���ڂ����I��
    msg_waza_oboe_08, // ����ł́c�c�@@1���@���ڂ���̂��@������߂܂����H"
    msg_waza_oboe_09, // @0�́@@1���@���ڂ����Ɂ@��������I��
    msg_waza_oboe_10, // @2�́@���ꂢ�Ɂ@�n�[�g�̃E���R���@�P�܂��@�������I��
    msg_waza_oboe_11, // @0���@�킷�ꂳ���ā@��낵���ł��ˁH"
  },
  { // �Z�v���o��
    msg_waza_omoi_01, // @0�� �ǂ̂킴���@��������������H
    msg_waza_omoi_02, // @1�@�� �����������܂����H
    msg_waza_omoi_03, // @0�Ɂ@�킴�� ��������������̂��@������߂܂����H
    msg_waza_omoi_04, // @0�́@�킷��Ă����@@1���@�������������I��
    msg_waza_omoi_05, // @0�́@@1���@���������������c�c���@�������@�`
    msg_waza_omoi_06, // �P�@�Q�́c�c�@�|�J���I���@@0�́@@1�� �����������@�`
    msg_waza_omoi_07, // @0�́@�킷��Ă����@@1���@�������������I��
    msg_waza_omoi_08, // ����ł́c�c�@@1���@�����������̂��@������߂܂����H
    msg_waza_omoi_09, // @0�́@@1���@�������������Ɂ@��������I��
    msg_waza_omoi_10, // @2�́@���ꂢ�� �n�[�g�̃E���R���@�P�܂��@�������I��
    msg_waza_omoi_11, // @0���@�킷�ꂳ���ā@��낵���ł��ˁH"
  }
};

// �͂��E�����������e�[�u��
static const YESNO_FUNC YesNoFunc[] =
{
  { WO_SetCheck_Yes, WO_SetCheck_No },  // @1�@��  ���ڂ������܂����H
  { WO_EndCheck_Yes, WO_EndCheck_No },  // @0�Ɂ@�킴��  ���ڂ�������̂��@������߂܂����H
  { WO_ChgCheck_Yes, WO_ChgCheck_No },  // @0�́@�����炵���@@1���@���ڂ������c�c���@�������@�`
  { WO_DelCheck_Yes, WO_DelCheck_No },  // ����ł́c�c�@@1���@���ڂ���̂��@������߂܂����H
  { WO_DelEnter_Yes, WO_DelEnter_No },  // @0���@�킷�ꂳ���ā@��낵���ł��ˁH"
};


#define CLACT_U_RES_MAX   ( 4 )

typedef struct{
  s16 x;              ///< [ X ] ���W
  s16 y;              ///< [ Y ] ���W
  s16 z;              ///< [ Z ] ���W

  u16 anm;            ///< �A�j���ԍ�
  int pri;            ///< �D�揇��
  int pal;            ///< �p���b�g�ԍ� �����̒l�� TCATS_ADD_S_PAL_AUTO �ɂ��邱�ƂŁA
                      ///< NCE�f�[�^�̃J���[No�w����󂯌p��
  int d_area;           ///< �`��G���A

  int id[ CLACT_U_RES_MAX ];    ///< �g�p���\�[�XID�e�[�u��

  int bg_pri;           ///< BG�ʂւ̗D��x
  int vram_trans;         ///< Vram�]���t���O
} CLACT_ENTRY_DATA;


// �Z���A�N�^�[�f�[�^
static const CLACT_ENTRY_DATA ClactParamTbl[] =
{
  { // ���X�g�J�[�\����
    LIST_CUR_D_PX, LIST_CUR_D_PY, 0,
    APP_COMMON_BARICON_CURSOR_DOWN, 1, 2, CLSYS_DEFREND_SUB,
    { WO_CHR_ID_APP_COMMON, WO_PAL_ID_APP_COMMON, WO_CEL_ID_APP_COMMON, WO_CEL_ID_APP_COMMON, },
    2, 0
  },
  { // ���X�g�J�[�\����
    LIST_CUR_U_PX, LIST_CUR_U_PY, 0,
    APP_COMMON_BARICON_CURSOR_UP, 1, 2, CLSYS_DEFREND_SUB,
    { WO_CHR_ID_APP_COMMON, WO_PAL_ID_APP_COMMON, WO_CEL_ID_APP_COMMON, WO_CEL_ID_APP_COMMON, },
    2, 0
  },
  { // �I���J�[�\��
    SEL_CURSOR_PX, SEL_CURSOR_PY, 0,
    ANMDW_CURSOR, 0, 3, CLSYS_DEFREND_SUB,
    { WO_CHR_ID_CURSOR, WO_PAL_ID_OBJ, WO_CEL_ID_CURSOR, WO_CEL_ID_CURSOR, },
    2, 0
  },
  { // ���ރA�C�R��
    KIND_ICON_PX, KIND_ICON_PY, 0,
    0, 0, TICON_ACTPAL_IDX, CLSYS_DEFREND_SUB,
    { WO_CHR_ID_KIND, WO_PAL_ID_TYPE, WO_CEL_ID_TYPE, WO_CEL_ID_TYPE, },
    2, 0
  },
  { // �^�C�v�A�C�R���P(����ʁj
    TYPE_ICON1_PX, TYPE_ICON1_PY, 0,
    0, 1, TICON_ACTPAL_IDX, CLSYS_DEFREND_SUB,
    { WO_CHR_ID_TYPE1, WO_PAL_ID_TYPE, WO_CEL_ID_TYPE, WO_CEL_ID_TYPE,  },
    2, 0
  },
  { // �^�C�v�A�C�R���Q(����ʁj
    TYPE_ICON2_PX, TYPE_ICON2_PY, 0,
    0, 1, TICON_ACTPAL_IDX, CLSYS_DEFREND_SUB,
    { WO_CHR_ID_TYPE2, WO_PAL_ID_TYPE, WO_CEL_ID_TYPE, WO_CEL_ID_TYPE,  },
    2, 0
  },
  { // �^�C�v�A�C�R���R(����ʁj
    TYPE_ICON3_PX, TYPE_ICON3_PY, 0,
    0, 1, TICON_ACTPAL_IDX, CLSYS_DEFREND_SUB,
    { WO_CHR_ID_TYPE3, WO_PAL_ID_TYPE, WO_CEL_ID_TYPE, WO_CEL_ID_TYPE,  },
    2, 0
  },
  { // �^�C�v�A�C�R���S(����ʁj
    TYPE_ICON4_PX, TYPE_ICON4_PY, 0,
    0, 1, TICON_ACTPAL_IDX, CLSYS_DEFREND_SUB,
    { WO_CHR_ID_TYPE4, WO_PAL_ID_TYPE, WO_CEL_ID_TYPE, WO_CEL_ID_TYPE,  },
    2, 0
  },

  { // �^�C�v�A�C�R���T(���ʁj
    TYPE_ICON5_PX, TYPE_ICON5_PY, 0,
    0, 1, TICON_ACTPAL_IDX_M, CLSYS_DEFREND_MAIN,
    { WO_CHR_ID_TYPE5, WO_PAL_ID_TYPE_M, WO_CEL_ID_TYPE, WO_CEL_ID_TYPE, },
    2, 0
  },
  { // �^�C�v�A�C�R���U(���ʁj
    TYPE_ICON6_PX, TYPE_ICON6_PY, 0,
    0, 1, TICON_ACTPAL_IDX_M, CLSYS_DEFREND_MAIN,
    { WO_CHR_ID_TYPE6, WO_PAL_ID_TYPE_M, WO_CEL_ID_TYPE, WO_CEL_ID_TYPE, },
    2, 0
  },
  { // �^�C�v�A�C�R���V(���ʁj
    TYPE_ICON7_PX, TYPE_ICON7_PY, 0,
    0, 1, TICON_ACTPAL_IDX_M, CLSYS_DEFREND_MAIN,
    { WO_CHR_ID_TYPE7, WO_PAL_ID_TYPE_M, WO_CEL_ID_TYPE, WO_CEL_ID_TYPE, },
    2, 0
  },
  { // �^�C�v�A�C�R���W(���ʁj
    TYPE_ICON8_PX, TYPE_ICON8_PY, 0,
    0, 1, TICON_ACTPAL_IDX_M, CLSYS_DEFREND_MAIN,
    { WO_CHR_ID_TYPE8, WO_PAL_ID_TYPE_M, WO_CEL_ID_TYPE, WO_CEL_ID_TYPE, },
    2, 0
  },

  { // �|�P�������ʊG(���ʁj
    POKE_PX, POKE_PY, 0,
    0, 0, 0, CLSYS_DEFREND_MAIN,
    { WO_CHR_ID_POKEGRA, WO_PAL_ID_POKEGRA, WO_CEL_ID_POKEGRA, WO_CEL_ID_POKEGRA, },
    2, 0
  },
  { // �߂�{�^��
    224, 168, 0,
    APP_COMMON_BARICON_RETURN, 0, 0, CLSYS_DEFREND_SUB,
    { WO_CHR_ID_APP_COMMON, WO_PAL_ID_APP_COMMON, WO_CEL_ID_APP_COMMON, WO_CEL_ID_APP_COMMON, },
    2, 0
  },
};


//============================================================================================
//  �v���Z�X�֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param proc  �v���Z�X�f�[�^
 * @param pwk   �p�����[�^���[�N
 * @param mywk  �A�v���P�[�V�������[�N
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT WazaOshieProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  WO_WORK * wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WAZAOSHIE, 0x50000 );   // �q�[�v�쐬

  wk = GFL_PROC_AllocWork( proc, sizeof(WO_WORK), HEAPID_WAZAOSHIE ); // ���[�N�m�ہ��N���A
  GFL_STD_MemFill( wk, 0, sizeof(WO_WORK) );

  wk->dat = pwk;  // PROC�쐬���p�����[�^���n��

  wk->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr ,
                                    GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_WAZAOSHIE );

  wk->pMsgTcblSys = GFL_TCBL_Init( HEAPID_WAZAOSHIE , HEAPID_WAZAOSHIE , 32 , 32 );

  WO_DispInit( wk );

  OS_Printf("first pos = %d\n", wk->dat->pos);

  WO_SelCursorChange( wk, wk->dat->pos, PALDW_CURSOR );
  wk->next_seq = SEQ_SELECT;


//  WO_ScrollCursorPut( wk ,0, FALSE);

  return GFL_PROC_RES_FINISH;
/*��[GS_CONVERT_TAG]*/
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F���C��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT WazaOshieProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
/*��[GS_CONVERT_TAG]*/
{
  WO_WORK * wk  = mywk;

  switch( *seq ){
  case SEQ_FADE_WAIT:   // �t�F�[�h�I���҂�
    *seq = WO_SeqFadeWait( wk );
    break;

  case SEQ_SELECT:    // �Z�Z���N�g
    *seq = WO_SeqSelect( wk );
    break;

  case SEQ_MSG_WAIT:    // ���b�Z�[�W�I���҂�
    *seq = WO_SeqMsgWait( wk );
    break;

  case SEQ_YESNO_PUT:   // �͂��E�������Z�b�g
    *seq = WO_SeqYesNoPut( wk );
    break;

  case SEQ_YESNO_WAIT:  // �͂��E�������I��҂�
    *seq = WO_SeqYesNoWait( wk );
    break;

  case SEQ_WAZA_SET:    // �Z�o��
    *seq = WO_SeqWazaSet( wk );
    break;

  case SEQ_WAZADEL_SET: // �Y��ċZ�o��
    *seq = WO_SeqWazaDelSet( wk );
    break;

  case SEQ_DEL_CHECK:   // �Y�ꂳ����Z���`�F�b�N
    *seq = WO_SeqDelCheck( wk );
    break;

  case SEQ_FADEOUT_SET: // �t�F�[�h�A�E�g�Z�b�g
    *seq = WO_SeqFadeOutSet( wk );
    break;

  case SEQ_END:     // �I��
    return GFL_PROC_RES_FINISH;

  case SEQ_PST_INIT:    // �X�e�[�^�X��ʂ�
    *seq = WO_SeqPstInit( wk );
    break;

  case SEQ_PST_CALL:    // �X�e�[�^�X��ʌĂяo��
    *seq = WO_SeqPstCall( wk );
    return GFL_PROC_RES_CONTINUE;

  case SEQ_PST_WAIT:    // �X�e�[�^�X��ʏI���҂�
    *seq = WO_SeqPstWait( wk );
    return GFL_PROC_RES_CONTINUE;

  case SEQ_BUTTON_ANM:
    *seq = ButtonAnmMaun( wk );
    break;

  case SEQ_RET_BUTTON:
    *seq = WO_SeqRetButton( wk );
    break;

  case SEQ_ENTER_BUTTON:
    *seq = WO_SeqEnterButton( wk );
    break;
  }
  
  WO_AppTaskMenuMain(wk);
  WO_3DMain(&wk->p3d);
  GFL_CLACT_SYS_Main( );
  GFL_TCBL_Main( wk->pMsgTcblSys );
  PRINTSYS_QUE_Main( wk->printQue );

  return GFL_PROC_RES_CONTINUE;
/*��[GS_CONVERT_TAG]*/
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT WazaOshieProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
/*��[GS_CONVERT_TAG]*/
{
  WO_WORK * wk  = mywk;
/*��[GS_CONVERT_TAG]*/



  WO_DispExit( wk );

//  GFL_TCB_DeleteTask( wk->vBlankTcb );

  GFL_TCBL_Exit( wk->pMsgTcblSys );

  GFL_FONT_Delete( wk->fontHandle );


  GFL_PROC_FreeWork( proc );          // ���[�N�J��
/*��[GS_CONVERT_TAG]*/

  GFL_HEAP_DeleteHeap( HEAPID_WAZAOSHIE );
/*��[GS_CONVERT_TAG]*/

  return GFL_PROC_RES_FINISH;
/*��[GS_CONVERT_TAG]*/
}


static const int menu_item[][2]={
  { msg_exp_decide, APP_TASKMENU_WIN_TYPE_NORMAL },
  { msg_exp_back,   APP_TASKMENU_WIN_TYPE_RETURN },
};

static const int yn_item[][2]={
  { msg_wazaoshie_yes,  APP_TASKMENU_WIN_TYPE_NORMAL },
  { msg_wazaoshie_no,   APP_TASKMENU_WIN_TYPE_NORMAL },
};

//----------------------------------------------------------------------------------
/**
 * @brief �^�X�N���j���[�p������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void InitTaskMenu( WO_WORK *wk )
{
  int i;
  wk->printQue = PRINTSYS_QUE_Create( HEAPID_WAZAOSHIE );
  
  // APPMENU���\�[�X�ǂݍ���
  wk->app_res = APP_TASKMENU_RES_Create( SFRM_MSG, 10, wk->fontHandle, wk->printQue, HEAPID_WAZAOSHIE );

  // �^�b�`�o�[���j���[����
  for(i=0;i<2;i++){
    wk->menuitem[i].str      = GFL_MSG_CreateString( wk->mman, menu_item[i][0] ); //���j���[�ɕ\�����镶����
    wk->menuitem[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;   //�����F�B�f�t�H���g�ł悢�Ȃ��APP_TASKMENU_ITEM_MSGCOLOR
    wk->menuitem[i].type     = menu_item[i][1];
  }
  // �͂��E������
  for(i=0;i<2;i++){
    wk->yn_menuitem[i].str      = GFL_MSG_CreateString( wk->mman, yn_item[i][0] ); //���j���[�ɕ\�����镶����
    wk->yn_menuitem[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;   //�����F�B�f�t�H���g�ł悢�Ȃ��APP_TASKMENU_ITEM_MSGCOLOR
    wk->yn_menuitem[i].type     = yn_item[i][1];
  }
  wk->oboe_menu_work[0] = NULL;
  wk->oboe_menu_work[1] = NULL;

}

//----------------------------------------------------------------------------------
/**
 * @brief �^�X�N���j���[�I��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void ExitTaskMenu( WO_WORK *wk )
{
  // ���j���[����Y��̏ꍇ�p��
  EnterButtonOnOff( wk, FALSE );
  
  // ������j��
  GFL_STR_DeleteBuffer( wk->menuitem[0].str);
  GFL_STR_DeleteBuffer( wk->menuitem[1].str);
  GFL_STR_DeleteBuffer( wk->yn_menuitem[0].str);
  GFL_STR_DeleteBuffer( wk->yn_menuitem[1].str);


  // APPMENU���\�[�X���
  APP_TASKMENU_RES_Delete( wk->app_res );

  PRINTSYS_QUE_Delete( wk->printQue );
}

//----------------------------------------------------------------------------------
/**
 * @brief �^�X�N���j���[���C���֐�
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void WO_AppTaskMenuMain( WO_WORK *wk )
{
  // �u���ڂ���v�u���ǂ�v���j���[�̕\�����C������
  if(wk->oboe_menu_work[0]!=NULL){
    APP_TASKMENU_WIN_Update( wk->oboe_menu_work[0] );
    APP_TASKMENU_WIN_Update( wk->oboe_menu_work[1] );
  }

}

//--------------------------------------------------------------------------------------------
/**
 * �����ݒ�
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_DispInit( WO_WORK * wk )
{
  ARCHANDLE* p_handle;

  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );

  p_handle = GFL_ARC_OpenDataHandle( ARCID_WAZAOSHIE_GRA, HEAPID_WAZAOSHIE );

  WO_VramBankSet();       // VRAM�ݒ�
  WO_BgSet( );      // BG�ݒ�
  WO_BgGraphicSet( wk, p_handle );      // BG�O���t�B�b�N�Z�b�g

  WO_ObjInit( wk, p_handle );
  WO_3DInit( wk );

  WO_MsgCreate( wk );
  WO_BmpWinSet( wk );
  WO_WazaListMake( wk );
  WO_PageChange( wk );

  WO_DefStrWrite( wk );

//  WO_InputModeChange(wk);

  CursorMoveInit( wk );
  ScrollButtonInit(wk);

  WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,   WIPE_TYPE_FADEIN,
                  WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WAZAOSHIE );

  // �㉺��ʕ\���ݒ�
  GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);

  // VblankFunc�Z�b�g
  wk->vBlankTcb = GFUser_VIntr_CreateTCB( WO_VBlank , wk , 16 );

  GFL_ARC_CloseDataHandle( p_handle );
/*��[GS_CONVERT_TAG]*/
  
  InitTaskMenu(wk); // �^�X�N���j���[������
}

//--------------------------------------------------------------------------------------------
/**
 * �������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_DispExit( WO_WORK * wk )
{

  ExitTaskMenu(wk);
  
  CursorMoveExit( wk );

  WO_WazaListExit( wk );
  WO_BmpWinExit( wk );
  WO_BgGraphicRelease( wk );
  WO_BgExit(  );    // BGL�폜

  WO_MsgRelease( wk );

  WO_3DRelease(&wk->p3d);
  WO_ObjFree( wk );

//  MsgPrintTouchPanelFlagSet( MSG_TP_OFF );


  // VBlankFunc�N���A
  GFL_TCB_DeleteTask( wk->vBlankTcb );
}


//--------------------------------------------------------------------------------------------
/**
 * VBlank�֐�
 *
 * @param work  ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_VBlank( GFL_TCB *tcb, void * work )
{
  WO_WORK * wk = work;

  GFL_BG_VBlankFunc( );

  // �Z���A�N�^�[
  GFL_CLACT_SYS_VBlankFunc();

  OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

static const GFL_DISP_VRAM waza_oshie_vram = {
    GX_VRAM_BG_128_B,       // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g

    GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g

    GX_VRAM_OBJ_16_G,       // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g

    GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g

    GX_VRAM_TEX_0_A,        // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_0_F,      // �e�N�X�`���p���b�g�X���b�g

    GX_OBJVRAMMODE_CHAR_1D_32K,
    GX_OBJVRAMMODE_CHAR_1D_32K

};


//--------------------------------------------------------------------------------------------
/**
 * VRAM�ݒ�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_VramBankSet(void)
{

  GFL_DISP_SetBank( &waza_oshie_vram );
}

//--------------------------------------------------------------------------------------------
/**
 * BG�ݒ�
 *
 * @param ini   BGL�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_BgSet( void )
{

  GFL_BG_Init( HEAPID_WAZAOSHIE );
  // BMPWIN�V�X�e���J�n
  GFL_BMPWIN_Init( HEAPID_WAZAOSHIE );

  // ���C��
  { // BG SYSTEM
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  { // PARAM FONT (BMP)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01, MFRM_PARAM_PRI, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( MFRM_PARAM, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  MFRM_PARAM );
  }
  { // PLATE (CHAR)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01, MFRM_BACK_PRI, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  MFRM_BACK, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(   MFRM_BACK );
  }


  // �T�u
  { // WINDOW (BMP)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01, SFRM_MSG_PRI, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( SFRM_MSG, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  SFRM_MSG );
  }

  { // PARAM FONT (BMP)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x18000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01, SFRM_PARAM_PRI, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( SFRM_PARAM, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  SFRM_PARAM );
  }

  { // TOUCHBAR_BG (CHAR)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, SFRM_PARAM_PRI, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( SFRM_TOUCHBAR, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  SFRM_TOUCHBAR );
  }

  { // PLATE (CHAR)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_128x128,
      GX_BG_EXTPLTT_01,   SFRM_BACK_PRI, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( SFRM_BACK, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  SFRM_BACK );
  }

  // BG�ʕ\��ON
  GFL_BG_SetVisible( MFRM_PARAM,    VISIBLE_ON );
  GFL_BG_SetVisible( MFRM_BACK,     VISIBLE_ON );
  GFL_BG_SetVisible( SFRM_MSG,      VISIBLE_ON );
  GFL_BG_SetVisible( SFRM_TOUCHBAR, VISIBLE_ON );
  GFL_BG_SetVisible( SFRM_PARAM,    VISIBLE_ON );
  GFL_BG_SetVisible( SFRM_BACK,     VISIBLE_ON );

  GFL_BG_SetClearCharacter( MFRM_PARAM,    32, 0, HEAPID_WAZAOSHIE );
  GFL_BG_SetClearCharacter( SFRM_MSG,      32, 0, HEAPID_WAZAOSHIE );
  GFL_BG_SetClearCharacter( SFRM_TOUCHBAR, 32, 0, HEAPID_WAZAOSHIE );

  GFL_BG_SetClearCharacter( MFRM_PARAM, 32, 0, HEAPID_WAZAOSHIE );
  GFL_BG_SetClearCharacter( SFRM_MSG,   32, 0, HEAPID_WAZAOSHIE );

  G2_BlendNone();

  //���ʂ��T�u,����ʂ����C���ɕύX
//  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
}

//--------------------------------------------------------------------------------------------
/**
 * BG���
 *
 * @param ini   BGL�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_BgExit( void )
{
  GFL_DISP_GXS_SetVisibleControl(
/*��[GS_CONVERT_TAG]*/
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG2 |
    GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ, VISIBLE_OFF );

  GFL_BG_FreeBGControl( SFRM_TOUCHBAR );
  GFL_BG_FreeBGControl( SFRM_BACK );
  GFL_BG_FreeBGControl( SFRM_PARAM );
  GFL_BG_FreeBGControl( SFRM_MSG );
  GFL_BG_FreeBGControl( MFRM_BACK );
  GFL_BG_FreeBGControl( MFRM_PARAM );

  //���ʂ����C��,����ʂ��T�u�ɖ߂�
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

  // BG�������
  GFL_BG_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_BgGraphicSet( WO_WORK * wk, ARCHANDLE* p_handle )
{
  // ���ʔw�i
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_waza_oshie_gra_bgd_NCGR,
                                        MFRM_BACK, 0,0,0, HEAPID_WAZAOSHIE );
  GFL_ARCHDL_UTIL_TransVramScreen( p_handle,NARC_waza_oshie_gra_bgu_back_NSCR,
                                        MFRM_BACK, 0, 0x800,0,HEAPID_WAZAOSHIE);
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_waza_oshie_gra_bgd_NCLR,
                                        PALTYPE_MAIN_BG, 0,0, HEAPID_WAZAOSHIE);

  // ����ʔw�i
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_waza_oshie_gra_bgd_NCGR,
                                        SFRM_BACK, 0,0,0, HEAPID_WAZAOSHIE);
  GFL_ARCHDL_UTIL_TransVramScreen( p_handle,NARC_waza_oshie_gra_bgd_back_NSCR,
                                        SFRM_BACK, 0,0x800,0, HEAPID_WAZAOSHIE);
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle,NARC_waza_oshie_gra_bgd_NCLR,
                                        PALTYPE_SUB_BG,0,0, HEAPID_WAZAOSHIE);

#if 0
  // �^�b�`�o�[�w�i
  {
    ARCHANDLE *c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_WAZAOSHIE );
    GFL_ARCHDL_UTIL_TransVramPalette( c_handle,APP_COMMON_GetBarPltArcIdx(),
                                        PALTYPE_SUB_BG, 32*10, 32, HEAPID_WAZAOSHIE);
    GFL_ARCHDL_UTIL_TransVramBgCharacter( c_handle, APP_COMMON_GetBarCharArcIdx(),
                                        SFRM_TOUCHBAR, 0,0,0, HEAPID_WAZAOSHIE);
    GFL_ARCHDL_UTIL_TransVramScreen( c_handle,APP_COMMON_GetBarScrnArcIdx(),
                                        SFRM_TOUCHBAR, 0,0x800,0, HEAPID_WAZAOSHIE);
    // �p���b�g�ύX
    GFL_BG_ChangeScreenPalette( SFRM_TOUCHBAR, 0, 0, 32, 24, 10 );
    GFL_BG_LoadScreenReq( SFRM_TOUCHBAR );
    GFL_ARC_CloseDataHandle( c_handle );
  }

#endif

  //�X�N���[�����\�[�X�擾
  wk->pSBufDParts = GFL_ARCHDL_UTIL_LoadScreen(
            p_handle,
            NARC_waza_oshie_gra_bgd_parts_NSCR,
            FALSE,
            &wk->pSDParts,
            HEAPID_WAZAOSHIE );

  // �t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG ,
                                  WO_PAL_TALK_FONT * 32, 16*2, HEAPID_WAZAOSHIE );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG ,
                                  WO_PAL_SYS_FONT*32, 16*2,HEAPID_WAZAOSHIE );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG ,
                                  WO_PAL_SYS_FONT*32, 16*2,HEAPID_WAZAOSHIE );

  // ��b�E�B���h�E�Z�b�g
  BmpWinFrame_GraphicSet( SFRM_MSG, WO_TALK_WIN_CGX,WO_PAL_TALK_WIN,
                          MENU_TYPE_SYSTEM, HEAPID_WAZAOSHIE );
}

static void WO_BgGraphicRelease( WO_WORK * wk )
{
  GFL_HEAP_FreeMemory(wk->pSBufDParts);
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�E�B���h�E�Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_BmpWinSet( WO_WORK * wk )
{
  u32 i;
  const BMPWIN_DAT *bmp;
  GFL_BMP_DATA *bmpdat;

  for( i=0; i<WIN_MAX; i++ ){
    bmp = &BmpWinData[i];
    OS_Printf("frm=%d, x=%d, y=%d,w=%d, h=%d, pal=%d\n", bmp->frm, bmp->x, bmp->y, bmp->w, bmp->h, bmp->pal);
    wk->win[i] = GFL_BMPWIN_Create( bmp->frm, bmp->x, bmp->y, bmp->w, bmp->h, bmp->pal, GFL_BMP_CHRAREA_GET_B );
    bmpdat = GFL_BMPWIN_GetBmp( wk->win[i] );
    GFL_BMP_Clear( bmpdat, 0 );
  }

  GFL_BMPWIN_ClearTransWindow_VBlank( wk->win[WIN_ABTN] );

  wk->printCursor = APP_KEYCURSOR_Create( 15, TRUE, FALSE, HEAPID_WAZAOSHIE );
  

}

//--------------------------------------------------------------------------------------------
/**
 * BMP�E�B���h�E�폜
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_BmpWinExit( WO_WORK * wk )
{
  u32 i;

  APP_KEYCURSOR_Delete(wk->printCursor);

  for( i=0; i<WIN_MAX; i++ ){
    GFL_BMPWIN_Delete( wk->win[i] );
  }
//  MsgPrintAutoFlagSet( MSG_TP_RECT_OFF );

  // BMPWIN�V�X�e���I��
  GFL_BMPWIN_Exit( );
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A�쐬
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_MsgCreate( WO_WORK * wk )
{
  wk->mman = GFL_MSG_Create(
          GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_waza_oboe_dat, HEAPID_WAZAOSHIE );

  wk->wset = WORDSET_Create( HEAPID_WAZAOSHIE );

  wk->mbuf = GFL_STR_CreateBuffer( TMP_MSGBUF_SIZ, HEAPID_WAZAOSHIE );
  wk->pp1_str = GFL_MSG_CreateString( wk->mman, msg_waza_oboe_01_04);
  wk->pp2_str = GFL_MSG_CreateString( wk->mman, msg_waza_oboe_01_10);
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A�폜
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_MsgRelease( WO_WORK * wk )
{
  GFL_MSG_Delete( wk->mman );
  WORDSET_Delete( wk->wset );
  GFL_STR_DeleteBuffer( wk->mbuf );
  GFL_STR_DeleteBuffer( wk->pp1_str );
  GFL_STR_DeleteBuffer( wk->pp2_str );
}



//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�t�F�[�h�C��
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqFadeWait( WO_WORK * wk )
{
  if( WIPE_SYS_EndCheck() == TRUE ){
    return wk->next_seq;
  }
  return SEQ_FADE_WAIT;
}


//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F���́@����ʒu������s
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqSelectDecide(WO_WORK* wk)
{
  //�߂�
  if(wk->cur_pos == CPOS_BACK){
    PMSND_PlaySE( WO_SE_CANCEL );
    WO_SelCursorChange( wk, wk->dat->pos, PALDW_CURSOR);
//    WO_ScrollCursorOff( wk );
    WO_TalkMsgSet( wk, MSG_END_CHECK );
    wk->ynidx = YESNO_END_CHECK;
    wk->next_seq = SEQ_YESNO_PUT;
    return SEQ_MSG_WAIT;
  }
  //�y�[�W�X�N���[��
  if(wk->cur_pos == CPOS_PAGE_DW){
    if(wk->dat->scr + LIST_NUM >= wk->sel_max){
      return SEQ_SELECT;
    }
    wk->dat->scr += LIST_NUM;
    WO_WazaListDraw( wk );
//    WO_ScrollCursorPut( wk ,0,TRUE);
    return SEQ_SELECT;
  }
  if(wk->cur_pos == CPOS_PAGE_UP){
    if(wk->dat->scr == 0){
      return SEQ_SELECT;
    }
    wk->dat->scr -= LIST_NUM;
    WO_WazaListDraw( wk );
//    WO_ScrollCursorPut( wk ,1,TRUE);
    return SEQ_SELECT;
  }
  //�o�������Z����
  if(wk->dat->scr + wk->dat->pos >= wk->sel_max){
    return SEQ_SELECT;
  }
  PMSND_PlaySE( WO_SE_DECIDE );
  WO_SelCursorChange( wk, wk->dat->pos, PALDW_CURSOR);
//  WO_ScrollCursorOff( wk );
  if( WO_WazaSetCheck( wk ) < 4 ){
    WO_TalkMsgSet( wk, MSG_SET_CHECK );
    wk->ynidx = YESNO_SET_CHECK;
    wk->next_seq = SEQ_YESNO_PUT;
  }else{
    WO_TalkMsgSet( wk, MSG_CHG_CHECK );
    wk->ynidx = YESNO_CHG_CHECK;
    wk->next_seq = SEQ_YESNO_PUT;
  }
  return SEQ_MSG_WAIT;
}


//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�Z�I���@�L�[or�^�b�`�\����ԃZ�b�g
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static void WO_InputModeChange( WO_WORK * wk )
{
  if(wk->key_mode == GFL_APP_KTST_TOUCH){
    GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_CURSOR], FALSE);
  }else{
    GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_CURSOR], TRUE);
    WO_SelCursorChange( wk, wk->cur_pos, PALDW_CURSOR);
  }
  WO_AButtonDraw(wk);
  WO_WazaListDraw(wk);
}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�Z�I���@�L�[or�^�b�`�ؑ�
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqSelectInputCheck( WO_WORK * wk )
{
  if(wk->key_mode == GFL_APP_KTST_TOUCH){
    if(GFL_UI_TP_GetCont() != 0){ //
/*��[GS_CONVERT_TAG]*/
      return FALSE;
    }
    if(GFL_UI_KEY_GetCont() != 0){
/*��[GS_CONVERT_TAG]*/
      wk->key_mode = GFL_APP_KTST_KEY;
/*��[GS_CONVERT_TAG]*/
      WO_InputModeChange(wk);
      return TRUE;
    }
  }else{
    if(GFL_UI_KEY_GetCont() != 0){
/*��[GS_CONVERT_TAG]*/
      return FALSE;
    }
    if(GFL_UI_TP_GetCont() != 0){
/*��[GS_CONVERT_TAG]*/
      wk->key_mode = GFL_APP_KTST_TOUCH;
/*��[GS_CONVERT_TAG]*/
      WO_InputModeChange(wk);
      return FALSE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�Z�I��
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqSelect( WO_WORK * wk )
{
  u32 ret;

  wk->cm_pos = CURSORMOVE_PosGet( wk->cmwk );   // �O��̈ʒu�ۑ�
  ret        = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case 0:   // �Z�P
  case 1:   // �Z�Q
  case 2:   // �Z�R
  case 3:   // �Z�S
    if( GFL_UI_TP_GetTrg() == FALSE ){
      if( wk->dat->scr + ret < wk->sel_max ){
        PMSND_PlaySE( WO_SE_DECIDE );
        WazaSelBgChange( wk, ret );
        return WazaSelectEnter( wk );
      }
    }else{
      if( wk->dat->scr + ret < wk->sel_max ){
        PMSND_PlaySE( WO_SE_LIST_MOVE );
        WazaSelBgChange( wk, ret );
      }else{
        WazaSelBgChange( wk, 5 );
      }
    }
    break;

  case 4:   // �����
//    PMSND_PlaySE( WO_SE_PAGE_MOVE );
//    ScrollButtonOnOff( wk );
    break;

  case 5:   // ����
//    PMSND_PlaySE( WO_SE_PAGE_MOVE );
    break;

  case 6:                   // ���ǂ�
    PMSND_PlaySE( WO_SE_CANCEL );
    if(EnterButtonCheck(wk)){
      return YameruButtonAnmInit( wk, SEQ_RET_BUTTON );
    }else{
      return RetButtonAnmInit( wk, SEQ_RET_BUTTON );
    }
    break;
  case CURSORMOVE_CANCEL:   // �L�����Z��
    PMSND_PlaySE( WO_SE_CANCEL );
    WazaSelBgChange( wk, 5 );
    EnterButtonOnOff( wk, FALSE );  // ���������
    return RetButtonAnmInit( wk, SEQ_RET_BUTTON );
  case 7:   // ���ڂ���
    PMSND_PlaySE( WO_SE_DECIDE );
    return EnterButtonAnmInit( wk, SEQ_ENTER_BUTTON );

  case CURSORMOVE_CURSOR_MOVE:  // �ړ�
//    PMSND_PlaySE( WO_SE_LIST_MOVE );
    break;

  case CURSORMOVE_NO_MOVE_UP:
    if(wk->dat->scr!=0){
      PMSND_PlaySE( WO_SE_PAGE_MOVE );
      wk->dat->scr--;
      BattleWazaParamPut( wk, WO_SelWazaGet( wk ) );
      WO_WazaListDraw( wk );
      WazaSelBgChange( wk, 5 );
      ScrollButtonOnOff( wk );
      ScrollButtonAnmChange( wk, -1 );
    }
    break;

  case CURSORMOVE_CURSOR_ON:    // �J�[�\���\��
  case CURSORMOVE_NONE:     // ����Ȃ�
    break;
  }

  return SEQ_SELECT;

}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F���b�Z�[�W�I���҂�
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqMsgWait( WO_WORK * wk )
{ 
  int status = PRINTSYS_PrintStreamGetState( wk->printStream );
  APP_KEYCURSOR_Main( wk->printCursor, wk->printStream, wk->win[WIN_MSG] );

  if(status == PRINTSTREAM_STATE_RUNNING){
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_DECIDE){
      PRINTSYS_PrintStreamShortWait( wk->printStream, 0 );
    }
  }else if( status == PRINTSTREAM_STATE_DONE ){
    PRINTSYS_PrintStreamDelete( wk->printStream );
    return wk->next_seq;
  }else if(status==PRINTSTREAM_STATE_PAUSE){
    if(GFL_UI_KEY_GetTrg()&(PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) || GFL_UI_TP_GetTrg()){
       PRINTSYS_PrintStreamReleasePause( wk->printStream );
       PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
    }
  }
  return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�͂��E�������\��
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqYesNoPut( WO_WORK * wk )
{
  APP_TASKMENU_INITWORK init;

  init.heapId   = HEAPID_WAZAOSHIE;
  init.itemNum  = 2;
  init.itemWork = wk->yn_menuitem;
  init.posType  = ATPT_LEFT_UP;
  init.charPosX = 21; //�E�B���h�E�J�n�ʒu(�L�����P��
  init.charPosY =  8;
  init.w = 10;  //�L�����P��
  init.h =  3;  //�L�����P��

  // �͂��E���������j���[�J�n
  wk->app_menuwork = APP_TASKMENU_OpenMenu( &init, wk->app_res );

  PassiveSet( TRUE );
  return SEQ_YESNO_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�͂��E����������
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqYesNoWait( WO_WORK * wk )
{

  u32 ret=SEQ_YESNO_WAIT;
  if(APP_TASKMENU_IsFinish( wk->app_menuwork )){
    if(APP_TASKMENU_GetCursorPos(wk->app_menuwork)==0){
      ret = YesNoFunc[wk->ynidx].yes( wk );
    }else{
      ret = YesNoFunc[wk->ynidx].no( wk );
    }
    PassiveSet( FALSE );
  }
  APP_TASKMENU_UpdateMenu( wk->app_menuwork );
  if(ret!=SEQ_YESNO_WAIT){
    APP_TASKMENU_CloseMenu( wk->app_menuwork );
  }

  return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�Z�Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqWazaSet( WO_WORK * wk )
{
  // �Z�Z�b�g
  PP_Put( wk->dat->pp, ID_PARA_waza1+wk->dat->del_pos, WO_SelWazaGet( wk ) );
  // �Z�b�g�����Z��PP�{����0�ɏ�����
  PP_Put( wk->dat->pp, ID_PARA_pp_count1+wk->dat->del_pos, 0 );
  // �Z�b�g�����Z��PPMAX���Z�b�g
  PP_Put( wk->dat->pp, ID_PARA_pp1+wk->dat->del_pos, WAZADATA_GetMaxPP( WO_SelWazaGet(wk), 0 ) );

  wk->dat->ret = WAZAOSHIE_RET_SET;

  return SEQ_FADEOUT_SET;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�Z�Z�b�g�������b�Z�[�W�Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqWazaDelSet( WO_WORK * wk )
{
  WO_TalkMsgSet( wk, MSG_CHG_WAZA );
  wk->next_seq = SEQ_WAZA_SET;
  return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�Z�I���`�F�b�N
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqDelCheck( WO_WORK * wk )
{
  if( wk->dat->del_pos < 4 ){
    WO_TalkMsgSet( wk, MSG_DEL_ENTER );
    wk->ynidx = YESNO_DEL_ENTER;
  }else{
    WO_TalkMsgSet( wk, MSG_DEL_CHECK );
    wk->ynidx = YESNO_DEL_CHECK;
  }
  wk->next_seq = SEQ_YESNO_PUT;

  return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�F�t�F�[�h�A�E�g�Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqFadeOutSet( WO_WORK * wk )
{
  WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,  WIPE_TYPE_FADEOUT,
                  WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WAZAOSHIE );
  wk->next_seq = SEQ_END;
  return SEQ_FADE_WAIT;
}


//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ�
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_PageChange( WO_WORK * wk )
{
  u16 waza;

  waza = WO_SelWazaGet( wk );

  if( waza != WAZAOSHIE_TBL_MAX ){
    BattleWazaParamPut( wk, waza );
  }else{
    BattleWazaParamPut( wk, BMPMENULIST_CANCEL );
  }
  WO_TypeIconInit( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �����\��
 *
 * @param wk    ���[�N
 * @param widx  �E�B���h�E�C���f�b�N�X
 * @param font    �\���t�H���g
 * @param col   �\���J���[
 * @param mode  �\�����[�h
 * @param py    y�����I�t�Z�b�g
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void StrPut( WO_WORK * wk, u32 widx, GFL_FONT *font, PRINTSYS_LSB  col, u32 mode, u8 py )
{
  u8  siz;
  u8  wsx;
  u8  px;

  switch( mode ){
  case STR_MODE_LEFT:   // ���l��
    px = 0;
    break;
  case STR_MODE_RIGHT:  // �E�l��
    siz = PRINTSYS_GetStrWidth( wk->mbuf, font, 0 );
    wsx = GFL_BMPWIN_GetSizeX( wk->win[widx] ) * 8;
    px  = wsx - siz;
    break;
  case STR_MODE_CENTER: // ����
    siz = PRINTSYS_GetStrWidth( wk->mbuf, font, 0 );
    wsx = GFL_BMPWIN_GetSizeX( wk->win[widx] ) * 8;
    px  = (wsx-siz)/2;
    break;
  }

  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->win[widx]), px, py, wk->mbuf, font, col );
//  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[widx] );


}

//--------------------------------------------------------------------------------------------
/**
 * �������蕶����W�J
 *
 * @param wk    ���[�N
 * @param msg_id  ���b�Z�[�WID
 * @param num   ����
 * @param keta  ��
 * @param type  �\���^�C�v
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void NumPrmSet( WO_WORK * wk, u32 msg_id, u32 num, u8 keta, u8 type )
{
  STRBUF * str;

  str = GFL_MSG_CreateString( wk->mman, msg_id );
  WORDSET_RegisterNumber( wk->wset, 0, num, keta, type, STR_NUM_CODE_ZENKAKU );
  WORDSET_ExpandStr( wk->wset, wk->mbuf, str );
  GFL_STR_DeleteBuffer( str );
}


//--------------------------------------------------------------------------------------------
/**
 * �Œ蕶����`��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_DefStrWrite( WO_WORK * wk )
{
  STRBUF  *str;
  GFL_MSGDATA * waza_man;
  const POKEMON_PASO_PARAM* ppp;
  u32 siz,i;

  //�@���߂�
//  GFL_MSG_GetString( wk->mman, msg_waza_oboe_exp_01, wk->mbuf );
//  StrPut( wk, WIN_EXP, wk->fontHandle, WOFCOL_N_BLACK, STR_MODE_LEFT ,4);
//  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_EXP] );

  //�@���ǂ�
  GFL_MSG_GetString( wk->mman, msg_exp_decide, wk->mbuf );
  StrPut( wk, WIN_ABTN, wk->fontHandle, WOFCOL_N_WHITE, STR_MODE_CENTER ,4);

/*
  //�@���ǂ�
  GFL_MSG_GetString( wk->mman, msg_exp_back, wk->mbuf );
  StrPut( wk, WIN_BACK, wk->fontHandle, WOFCOL_N_WHITE, STR_MODE_CENTER ,4);
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_BACK] );
*/

  // �u����傭�v
  GFL_MSG_GetString( wk->mman, msg_waza_oboe_01_02, wk->mbuf );
  StrPut( wk, WIN_STR_ATTACK, wk->fontHandle, WOFCOL_N_BLACK, STR_MODE_LEFT ,0);
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_STR_ATTACK] );

  // �u�߂����イ�v
  GFL_MSG_GetString( wk->mman, msg_waza_oboe_01_03, wk->mbuf );
  StrPut( wk, WIN_STR_HIT, wk->fontHandle, WOFCOL_N_BLACK, STR_MODE_LEFT ,0);
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_STR_HIT] );

  ///////////////////////////////////////////////////////////////////////////
  //����
  str = GFL_STR_CreateBuffer( TMP_MSGBUF_SIZ, HEAPID_WAZAOSHIE );

  // �^�C�g��
  GFL_MSG_GetString( wk->mman, msg_param_exp, str );
  WORDSET_RegisterPokeNickName( wk->wset, 0, wk->dat->pp );
  WORDSET_ExpandStr( wk->wset, wk->mbuf, str );
  StrPut( wk, WIN_TITLE, wk->fontHandle, WOFCOL_N_WHITE, STR_MODE_LEFT ,4);
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_TITLE] );

  waza_man = GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wazaname_dat, HEAPID_WAZAOSHIE );
  ppp = PP_GetPPPPointerConst(wk->dat->pp);

  //�Z���X�g
  for(i = 0;i < 4;i++){
    u16 waza = PPP_Get(ppp,ID_PARA_waza1+i,NULL);
    if(waza == 0){
      GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_TYPE5+i], 0 );
      continue;
    }else{
      GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_TYPE5+i], 1 );
      
      // �Z�^�C�v�A�C�R����������
      WO_TypeIconChange( wk, waza, i+LIST_NUM );
    }
    //�Z��
    GFL_MSG_GetString( waza_man, waza , wk->mbuf );
    StrPut( wk, WIN_MWAZA, wk->fontHandle, WOFCOL_N_WHITE, STR_MODE_LEFT ,32*i);

    //PP
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->win[WIN_MWAZA]), U_WLIST_PP1_X, 32*i+16, wk->pp1_str,
                         wk->fontHandle, WOFCOL_N_BLACK );
    GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_MWAZA] );


    //PP/MPP
    WORDSET_RegisterNumber( wk->wset, 0,
      PPP_Get(ppp,ID_PARA_pp1+i,NULL),2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( wk->wset, 1,
      PPP_Get(ppp,ID_PARA_pp_max1+i,NULL),2, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU );
    WORDSET_ExpandStr( wk->wset, wk->mbuf, wk->pp2_str );
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->win[WIN_MWAZA]), U_WLIST_PP2_X, 32*i+16,
                         wk->mbuf, wk->fontHandle, WOFCOL_N_BLACK );
    GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_MWAZA] );

  }
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_MWAZA] );

  //�|�P�������ƃ��x��
  GFL_MSG_GetString( wk->mman, msg_param_name, str );
  WORDSET_RegisterPokeMonsName(wk->wset,0,wk->dat->pp );
  WORDSET_ExpandStr( wk->wset, wk->mbuf, str );
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->win[WIN_MPRM]), F_WLIST_NAME_OX, 0,wk->mbuf,
                       wk->fontHandle,  WOFCOL_N_BLACK);
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_MPRM] );

  GFL_MSG_GetString( wk->mman, msg_param_level, str );
  WORDSET_RegisterNumber( wk->wset, 0,
      PPP_Get(ppp,ID_PARA_level,NULL),3, STR_NUM_DISP_SPACE, STR_NUM_CODE_ZENKAKU );

  WORDSET_ExpandStr( wk->wset, wk->mbuf, str );
  StrPut( wk, WIN_MPRM, wk->fontHandle, WOFCOL_N_BLACK, STR_MODE_RIGHT ,16);

  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_MPRM] );

  GFL_MSG_Delete( waza_man );

  GFL_STR_DeleteBuffer(str);
}


//--------------------------------------------------------------------------------------------
/**
 * �Z���擾
 *
 * @param wk    ���[�N
 *
 * @return  �Z��
 */
//--------------------------------------------------------------------------------------------
static u32 WO_WazaTableNumGet( WO_WORK * wk )
{
  u32 i;

  for( i=0; i<256; i++ ){
    if( wk->dat->waza_tbl[i] == WAZAOSHIE_TBL_MAX ){
      break;
    }
  }
  return i;
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʃp�[�c�`��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_SubBGPartsDraw( WO_WORK * wk ,u8 px,u8 py,u8 sx, u8 sy,u8 ox,u8 oy)
{
/*
  GFL_BG_WriteScreenExpand(SFRM_BACK, px,py,sx,sy, wk->pSDParts->rawData,
                           ox,oy,wk->pSDParts->screenWidth/8,wk->pSDParts->screenHeight/8);

  GFL_BG_LoadScreenV_Req( SFRM_BACK );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���X�g ���C���`��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_WazaListLineDraw( WO_WORK * wk ,u8 scr,u8 pos)
{
  u32 tmp,py;

  py = pos*LIST_SY;
  tmp = 0;

  if(scr+pos >= wk->sel_max){
    WO_SubBGPartsDraw(wk,BG_WLIST_PX,WIN_LIST_PY+(pos*BG_WLIST_SY),
      BG_WLIST_SX,BG_WLIST_SY,BG_WLIST_SPX,BG_WLIST_SPY+BG_WLIST_SY);
    return;
  }

  //�Z���`��
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->win[WIN_LIST]),0, py,wk->ld[scr+pos].str,
                      wk->fontHandle, WOFCOL_N_WHITE );
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_LIST] );

  //PP
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->win[WIN_LIST]), F_WLIST_PP1_OX, py,wk->pp1_str, wk->fontHandle, WOFCOL_N_BLACK);
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_LIST] );

  // PP/MPP
  tmp = WAZADATA_GetMaxPP( wk->ld[scr+pos].param, 0 );
  WORDSET_RegisterNumber( wk->wset, 0, tmp,2, STR_NUM_DISP_SPACE, STR_NUM_CODE_ZENKAKU );

  WORDSET_RegisterNumber( wk->wset, 1, tmp,2, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU );

  WORDSET_ExpandStr( wk->wset, wk->mbuf, wk->pp2_str );
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->win[WIN_LIST]), F_WLIST_PP2_OX, py, wk->mbuf,
                       wk->fontHandle, WOFCOL_N_BLACK );
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_LIST] );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���X�g �y�[�W�`��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_WazaListDraw( WO_WORK * wk )
{
  u32 i;

  //�̈�N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_LIST]),0);

  for(i = 0;i < LIST_NUM;i++){
    WO_WazaListLineDraw(wk ,wk->dat->scr,i);
  }
  //�E�B���h�E�v�b�g
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_LIST]);
  GFL_BG_LoadScreenV_Req( SFRM_BACK );

  WO_TypeIconInit( wk );
}


//--------------------------------------------------------------------------------------------
/**
 * �Z���X�g�쐬
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_WazaListMake( WO_WORK * wk )
{
  GFL_MSGDATA * mman;
  u32 i;

  wk->sel_max = (u8)WO_WazaTableNumGet( wk );

  wk->ld = BmpMenuWork_ListCreate( wk->sel_max, HEAPID_WAZAOSHIE );
/*��[GS_CONVERT_TAG]*/
  i = wk->ld[0].param;

  mman = GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wazaname_dat, HEAPID_WAZAOSHIE );
/*��[GS_CONVERT_TAG]*/

  for( i=0; i<wk->sel_max; i++ ){
    if( wk->dat->waza_tbl[i] != WAZAOSHIE_TBL_MAX ){
//      BMP_MENULIST_AddArchiveString( wk->ld, mman, wk->dat->waza_tbl[i], wk->dat->waza_tbl[i] );
      BmpMenuWork_ListAddArchiveString( wk->ld, mman, wk->dat->waza_tbl[i],
                                        wk->dat->waza_tbl[i], HEAPID_WAZAOSHIE );

    }else{
//      BMP_MENULIST_AddArchiveString(
//        wk->ld, wk->mman, msg_waza_oboe_01_11, BMPMENULIST_CANCEL );
      BmpMenuWork_ListAddArchiveString(
        wk->ld, wk->mman, msg_waza_oboe_01_11, BMPMENULIST_CANCEL, HEAPID_WAZAOSHIE );
      break;
    }
  }

  GFL_MSG_Delete( mman );
  wk->cpos_x = 0;
  wk->cpos_y = wk->dat->pos;
  WO_WazaListDraw( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���X�g�폜
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_WazaListExit( WO_WORK * wk )
{
//  BMP_MENULIST_Delete( wk->ld );
    BmpMenuWork_ListDelete( wk->ld );
}

//--------------------------------------------------------------------------------------------
/**
 * ����{�^���`��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_AButtonDraw( WO_WORK * wk )
{
/*
  u16 ox;
  if( wk->key_mode == GFL_APP_KTST_KEY ||
    (wk->dat->scr+wk->dat->pos >= wk->sel_max) ||
    wk->cur_pos >= LIST_NUM){
    GFL_BMPWIN_ClearTransWindow_VBlank( wk->win[WIN_ABTN] );
    ox = BG_ABTN_OX;
  }else{
    GFL_BMPWIN_MakeTransWindow_VBlank( &wk->win[WIN_ABTN] );
    ox = BG_ABTN_OX+BG_ABTN_SX;
  }
  WO_SubBGPartsDraw(wk,BG_ABTN_PX,BG_ABTN_PY,
    BG_ABTN_SX,BG_ABTN_SY,ox,BG_ABTN_OY);
*/
}


//--------------------------------------------------------------------------------------------
/**
 * �퓬�Z�p�����[�^�\��
 *
 * @param wk    ���[�N
 * @param prm   BMP���X�g�f�[�^�i��ɋZ�ԍ��j
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BattleWazaParamPut( WO_WORK * wk, u32 prm )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_BTL_INFO]), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_PRM_ATTACK]), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_PRM_HIT]), 0 );

  GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_KIND], 0 );

  if( prm != BMPMENULIST_CANCEL ){
    GFL_MSGDATA * mman;
/*��[GS_CONVERT_TAG]*/
    u32 tmp;

    // �З�
    tmp = WAZADATA_GetParam( prm, WAZAPARAM_POWER );
    if( tmp <= 1 ){
      GFL_MSG_GetString( wk->mman, msg_waza_oboe_01_12, wk->mbuf );
    }else{
      NumPrmSet( wk, msg_waza_oboe_01_08, tmp, 3, STR_NUM_DISP_LEFT );
    }
    StrPut( wk, WIN_PRM_ATTACK, wk->fontHandle, WOFCOL_N_BLACK, STR_MODE_RIGHT ,0);

    // ����
    tmp = WAZADATA_GetParam( prm, WAZAPARAM_HITPER );
    if( tmp == 0 || WAZADATA_IsAlwaysHit( prm )){
      GFL_MSG_GetString( wk->mman, msg_waza_oboe_01_12, wk->mbuf );
    }else{
      NumPrmSet( wk, msg_waza_oboe_01_09, tmp, 3, STR_NUM_DISP_LEFT );
    }
    StrPut( wk, WIN_PRM_HIT, wk->fontHandle, WOFCOL_N_BLACK, STR_MODE_RIGHT ,0);

    // ����
    mman = GFL_MSG_Create(
        GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wazainfo_dat, HEAPID_WAZAOSHIE );
    GFL_MSG_GetString( mman, prm, wk->mbuf );
    StrPut( wk, WIN_BTL_INFO, wk->fontHandle, WOFCOL_N_BLACK, STR_MODE_LEFT ,0);
    GFL_MSG_Delete( mman );

    WO_KindIconChange( wk, prm );
    GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_KIND], 1 );

    GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_STR_ATTACK] );
    GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_STR_HIT] );
  }else{
    GFL_BMPWIN_ClearTransWindow_VBlank( wk->win[WIN_STR_ATTACK] );
    GFL_BMPWIN_ClearTransWindow_VBlank( wk->win[WIN_STR_HIT] );
  }

  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_BTL_INFO] );
/*��[GS_CONVERT_TAG]*/
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_PRM_ATTACK] );
/*��[GS_CONVERT_TAG]*/
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_PRM_HIT] );
/*��[GS_CONVERT_TAG]*/
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�쐬
 *
 * @param wk    ���[�N
 * @param id    ���b�Z�[�WID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_MsgMake( WO_WORK * wk, u32 id )
{
  STRBUF * str;

  switch( id ){
  case MSG_SEL_WAZA:  // @0�Ɂ@�ǂ̂킴���@���ڂ�������H
    WORDSET_RegisterPokeNickName( wk->wset, 0, wk->dat->pp );
    break;
  case MSG_SET_CHECK: // @1�@��  ���ڂ������܂����H
    WORDSET_RegisterWazaName( wk->wset, 1, WO_SelWazaGet( wk ) );
    break;
  case MSG_END_CHECK: // @0�Ɂ@�킴��  ���ڂ�������̂��@������߂܂����H
    WORDSET_RegisterPokeNickName( wk->wset, 0, wk->dat->pp );
    break;
  case MSG_SET_WAZA:  // @0�́@�����炵���@@1���@���ڂ����I��"
    WORDSET_RegisterPokeNickName( wk->wset, 0, wk->dat->pp );
    WORDSET_RegisterWazaName( wk->wset, 1, WO_SelWazaGet( wk ) );
    break;
  case MSG_CHG_CHECK: // @0�́@�����炵���@@1���@���ڂ������c�c���@�������@�`
    WORDSET_RegisterPokeNickName( wk->wset, 0, wk->dat->pp );
    WORDSET_RegisterWazaName( wk->wset, 1, WO_SelWazaGet( wk ) );
    break;
  case MSG_DEL_WAZA:  // �P�@�Q�́c�c�@�|�J���I���@@0�́@@1�� �����������@�`
    WORDSET_RegisterPokeNickName( wk->wset, 0, wk->dat->pp );
    WORDSET_RegisterWazaName( wk->wset, 1, WO_DelWazaGet( wk ) );
    break;
  case MSG_CHG_WAZA:  // @0�́@�����炵�� @1���@���ڂ����I��
    WORDSET_RegisterPokeNickName( wk->wset, 0, wk->dat->pp );
    WORDSET_RegisterWazaName( wk->wset, 1, WO_SelWazaGet( wk ) );
    break;
  case MSG_DEL_CHECK: // ����ł́c�c�@@1���@���ڂ���̂��@������߂܂����H
    WORDSET_RegisterWazaName( wk->wset, 1, WO_SelWazaGet( wk ) );
    break;
  case MSG_CANCEL:  // @0�́@@1���@���ڂ����Ɂ@��������I��
    WORDSET_RegisterPokeNickName( wk->wset, 0, wk->dat->pp );
    WORDSET_RegisterWazaName( wk->wset, 1, WO_SelWazaGet( wk ) );
    break;
  case MSG_SUB_ITEM:  // @2�́@���ꂢ�Ɂ@�n�[�g�̃E���R���@�P�܂��@�������I��
    WORDSET_RegisterPlayerName( wk->wset, 2, wk->dat->myst );
    break;
  case MSG_DEL_ENTER: // @0���@�킷�ꂳ���ā@��낵���ł��ˁH
    WORDSET_RegisterWazaName( wk->wset, 0, WO_DelWazaGet( wk ) );
    break;
  }

  str = GFL_MSG_CreateString( wk->mman, MsgDataTbl[wk->dat->mode][id] );
  WORDSET_ExpandStr( wk->wset, wk->mbuf, str );
  GFL_STR_DeleteBuffer( str );
}

//--------------------------------------------------------------------------------------------
/**
 * �o����Z���擾
 *
 * @param wk    ���[�N
 *
 * @return  �o����Z
 */
//--------------------------------------------------------------------------------------------
static u16 WO_SelWazaGet( WO_WORK * wk )
{
  return wk->dat->waza_tbl[ wk->dat->scr + wk->dat->pos ];
}

//--------------------------------------------------------------------------------------------
/**
 * �Y���Z�擾
 *
 * @param wk    ���[�N
 *
 * @return  �Y���Z
 */
//--------------------------------------------------------------------------------------------
static u16 WO_DelWazaGet( WO_WORK * wk )
{
  return (u16)PP_Get( wk->dat->pp, ID_PARA_waza1+wk->dat->del_pos, NULL );
}

//--------------------------------------------------------------------------------------------
/**
 * ��b�i���j���b�Z�[�W�\��
 *
 * @param wk    ���[�N
 * @param id    ���b�Z�[�WID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_TalkMsgSet( WO_WORK * wk, u32 id )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_MSG]), 15 );
  BmpWinFrame_Write( wk->win[WIN_MSG], WINDOW_TRANS_ON, WO_TALK_WIN_CGX, WO_PAL_TALK_WIN );

  WO_MsgMake( wk, id );

  wk->printStream = PRINTSYS_PrintStreamCallBack(
    wk->win[WIN_MSG], 0, 0, wk->mbuf, wk->fontHandle,
    MSGSPEED_GetWait(), wk->pMsgTcblSys,
    0, HEAPID_WAZAOSHIE, 0xffff, WO_TalkMsgCallBack );

  GFL_BMPWIN_MakeTransWindow( wk->win[WIN_MSG] );

}

//--------------------------------------------------------------------------------------------
/**
 * ��b�i���j���b�Z�[�W�\��OFF
 *
 * @param wk    ���[�N
 * @param id    ���b�Z�[�WID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_TalkMsgOff( WO_WORK * wk)
{
  BmpWinFrame_Clear( wk->win[WIN_MSG],WINDOW_TRANS_OFF);
  GFL_BMPWIN_ClearTransWindow_VBlank( wk->win[WIN_MSG]);
}

//--------------------------------------------------------------------------------------------
/**
 * ��b�i���j���b�Z�[�W�R�[���o�b�N
 *
 * @param mph   ���b�Z�[�W�`��f�[�^
 * @param value �R�[���o�b�N�i���o
 *
 * @retval  "TRUE = �҂�"
 * @retval  "FALSE = ���̏�����"
 */
//--------------------------------------------------------------------------------------------
static BOOL WO_TalkMsgCallBack( u32 value )
{
  switch( value ){
  case 1:   // SE�I���҂�
    return PMSND_CheckPlaySE();

  case 2:   // SE�I���҂�
    return PMSND_CheckPlaySE();       //Snd_MePlayCheckBgmPlay(); ME�͂ǂ�����čĐ�����H
  case 3:   // "�|�J��"
    PMSND_PlaySE( SEQ_SE_KON );
    break;

  case 4:   // "���ڂ���"
    PMSND_PauseBGM( TRUE );
    PMSND_PushBGM();
    PMSND_PlayBGM( SEQ_ME_LVUP );
    break;

  case 5:   // "�|�J��"��SE�I���҂�
    if(PMSND_CheckPlayBGM()==FALSE){
      PMSND_PopBGM();
      PMSND_PauseBGM( FALSE );
    }else{
      return TRUE;
    }
    
  }

  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���S�o���Ă��邩�`�F�b�N
 *
 * @param wk    ���[�N
 *
 * @retval  "�o���Ă���ꍇ = 4"
 * @retval  "�o���Ă��Ȃ��ꍇ = 0 �` 3"
 */
//--------------------------------------------------------------------------------------------
static u8 WO_WazaSetCheck( WO_WORK * wk )
{
  u8  i;

  for( i=0; i<4; i++ ){
    if( PP_Get( wk->dat->pp, ID_PARA_waza1+i, NULL ) == 0 ){
      break;
    }
  }
  return i;
}


//--------------------------------------------------------------------------------------------
/**
 * �u@1�@��  ���ڂ������܂����H�v�Łu�͂��v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SetCheck_Yes( WO_WORK * wk )
{
  WO_TalkMsgSet( wk, MSG_SET_WAZA );
  wk->dat->del_pos = WO_WazaSetCheck( wk );
  wk->next_seq = SEQ_WAZA_SET;
  return SEQ_MSG_WAIT;
}
//--------------------------------------------------------------------------------------------
/**
 * �u@1�@��  ���ڂ������܂����H�v�Łu�������v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SetCheck_No( WO_WORK * wk )
{
  WO_TalkMsgOff(wk);
//  WO_SelCursorChange( wk, wk->dat->pos, PALDW_CURSOR);
  if( wk->enter_flg == 0 ){
    WazaSelBgChange( wk, 5 );
  }
  return SEQ_SELECT;
}

//--------------------------------------------------------------------------------------------
/**
 * �u@0�Ɂ@�킴��  ���ڂ�������̂��@������߂܂����H�v�Łu�͂��v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_EndCheck_Yes( WO_WORK * wk )
{
  wk->dat->ret = WAZAOSHIE_RET_CANCEL;
  return SEQ_FADEOUT_SET;
}
//--------------------------------------------------------------------------------------------
/**
 * �u@0�Ɂ@�킴��  ���ڂ�������̂��@������߂܂����H�v�Łu�������v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_EndCheck_No( WO_WORK * wk )
{
  WO_TalkMsgOff(wk);
//  WO_SelCursorChange( wk, wk->dat->pos, PALDW_CURSOR );
  return SEQ_SELECT;
}

//--------------------------------------------------------------------------------------------
/**
 * �u@0�́@�����炵���@@1���@���ڂ������c�c���@�������@�`�v�Łu�͂��v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_ChgCheck_Yes( WO_WORK * wk )
{
  wk->next_seq = SEQ_PST_INIT;
  return SEQ_FADE_WAIT;
}
//--------------------------------------------------------------------------------------------
/**
 * �u@0�́@�����炵���@@1���@���ڂ������c�c���@�������@�`�v�Łu�������v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_ChgCheck_No( WO_WORK * wk )
{
  WO_TalkMsgSet( wk, MSG_DEL_CHECK );
  wk->ynidx = YESNO_DEL_CHECK;
  wk->next_seq = SEQ_YESNO_PUT;
  return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �u����ł́c�c�@@1���@���ڂ���̂��@������߂܂����H�v�Łu�͂��v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_DelCheck_Yes( WO_WORK * wk )
{
  WO_TalkMsgSet( wk, MSG_CANCEL );
  wk->next_seq = SEQ_FADEOUT_SET;
  wk->dat->ret = WAZAOSHIE_RET_CANCEL;
  return SEQ_MSG_WAIT;
}
//--------------------------------------------------------------------------------------------
/**
 * �u����ł́c�c�@@1���@���ڂ���̂��@������߂܂����H�v�Łu�������v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_DelCheck_No( WO_WORK * wk )
{
  WO_TalkMsgSet( wk, MSG_CHG_CHECK );
  wk->ynidx = YESNO_CHG_CHECK;
  wk->next_seq = SEQ_YESNO_PUT;
  return SEQ_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �u@0���@�킷�ꂳ���ā@��낵���ł��ˁH�v�Łu�͂��v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_DelEnter_Yes( WO_WORK * wk )
{
  WO_TalkMsgSet( wk, MSG_DEL_WAZA );
  wk->next_seq = SEQ_WAZADEL_SET;
  return SEQ_MSG_WAIT;
}
//--------------------------------------------------------------------------------------------
/**
 * �u@0���@�킷�ꂳ���ā@��낵���ł��ˁH�v�Łu�������v���I�����ꂽ�ꍇ
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_DelEnter_No( WO_WORK * wk )
{
  wk->next_seq = SEQ_PST_INIT;
  return SEQ_FADE_WAIT;
}



//============================================================================================
//  �Z���A�N�^�[
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�}�l�[�W���[������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_ClactResManInit( WO_WORK * wk )
{

  static const GFL_CLSYS_INIT clsys_init  =
  {
    0, 0,   //���C���T�[�t�F�[�X�̍���X,Y���W
    0, 512, //�T�u�T�[�t�F�[�X�̍���X,Y���W
    4, 124, //���C��OAM�Ǘ��J�n�`�I�� �i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
    4, 124, //��bOAM�Ǘ��J�n�`�I��  �i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
    0,      //�Z��Vram�]���Ǘ���
    WO_CHR_ID_MAX, WO_PAL_ID_MAX, WO_CEL_ID_MAX, 32,
    16, 16,       //���C���A�T�u��CGRVRAM�Ǘ��̈�J�n�I�t�Z�b�g�i�ʐM�A�C�R���p��16�ȏ�ɂ��Ă��������j
  };


  //�V�X�e���쐬
  GFL_CLACT_SYS_Create( &clsys_init, &waza_oshie_vram, HEAPID_WAZAOSHIE );
  wk->clUnit = GFL_CLACT_UNIT_Create( WO_CLA_MAX, 0, HEAPID_WAZAOSHIE );
  GFL_CLACT_UNIT_SetDefaultRend( wk->clUnit );



  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}


//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[���
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_ObjFree( WO_WORK * wk )
{
  u32 i;

  for( i=0; i<WO_CLA_MAX; i++ ){
    GFL_CLACT_WK_Remove( wk->cap[i] );
  }
//  CATS_ResourceDestructor_S( wk->clUnit, wk->crp );
//  CATS_FreeMemory( wk->clUnit );

  //�V�X�e���j��
  GFL_CLACT_UNIT_Delete( wk->clUnit );
  GFL_CLACT_SYS_Delete();

}

//--------------------------------------------------------------------------------------------
/**
 * ��Ԉُ�A�C�R���̃��\�[�X�ݒ�
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_ResourceLoad( WO_WORK * wk, ARCHANDLE* p_handle )
{
  u32 i;
  ARCHANDLE *c_handle;
  POKEMON_PASO_PARAM * ppp;
  BOOL  fast;
  // �L����
  // ���j���[���ʃ��\�[�X�n���h���I�[�v��
  c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_WAZAOSHIE );

    // �^�b�`�o�[�L�����o�^
  wk->clres[0][WO_CHR_ID_APP_COMMON] = GFL_CLGRP_CGR_Register(
                            c_handle, APP_COMMON_GetBarIconCharArcIdx(), 0,
                            CLSYS_DRAW_SUB,   HEAPID_WAZAOSHIE );
  wk->clres[0][WO_CHR_ID_CURSOR] = GFL_CLGRP_CGR_Register(
                            p_handle, NARC_waza_oshie_gra_scroll_cur_NCGR, 0,
                            CLSYS_DRAW_SUB, HEAPID_WAZAOSHIE );

  // ��ރA�C�R��
  wk->clres[0][WO_CHR_ID_KIND] = GFL_CLGRP_CGR_Register( c_handle,
                                    APP_COMMON_GetWazaKindCharArcIdx(0), 0,
                                    CLSYS_DRAW_SUB, HEAPID_WAZAOSHIE );


  // �Z�^�C�v�A�C�R��
  for( i=WO_CHR_ID_TYPE1; i<=WO_CHR_ID_TYPE_MAX; i++ ){
    if(i < WO_CHR_ID_TYPE5){
      wk->clres[0][i] = GFL_CLGRP_CGR_Register( c_handle,
                                        APP_COMMON_GetPokeTypeCharArcIdx(0), 0,
                                        CLSYS_DRAW_SUB, HEAPID_WAZAOSHIE );
    }else{
      wk->clres[0][i] = GFL_CLGRP_CGR_Register( c_handle,
                                        APP_COMMON_GetPokeTypeCharArcIdx(0), 0,
                                        CLSYS_DRAW_MAIN, HEAPID_WAZAOSHIE );
    }
  }

  // �p���b�g
  wk->clres[1][WO_PAL_ID_APP_COMMON] = GFL_CLGRP_PLTT_RegisterEx(
                                        c_handle, APP_COMMON_GetBarIconPltArcIdx(),
                                        CLSYS_DRAW_SUB, 0, 0, 4, HEAPID_WAZAOSHIE );
  wk->clres[1][WO_PAL_ID_OBJ] = GFL_CLGRP_PLTT_RegisterEx(
                                p_handle, NARC_waza_oshie_gra_oam_dw_NCLR,
                                CLSYS_DRAW_SUB, 4*32,0,1, HEAPID_WAZAOSHIE );

  wk->clres[1][WO_PAL_ID_TYPE]   = GFL_CLGRP_PLTT_RegisterEx(
                                    c_handle, APP_COMMON_GetPokeTypePltArcIdx(),
                                    CLSYS_DRAW_SUB, DW_ACTPAL_NUM*32,
                                    0,3,HEAPID_WAZAOSHIE );

  wk->clres[1][WO_PAL_ID_TYPE_M] = GFL_CLGRP_PLTT_Register(
                                    c_handle, APP_COMMON_GetPokeTypePltArcIdx(),
                                    CLSYS_DRAW_MAIN, 0, HEAPID_WAZAOSHIE );

  OS_Printf("wk->clres[1][WO_PAL_ID_TYPE_M]=%d\n",wk->clres[1][WO_PAL_ID_TYPE_M]);

  // �Z��
  wk->clres[2][WO_CEL_ID_APP_COMMON] = GFL_CLGRP_CELLANIM_Register(
                                        c_handle, 
                                        APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K),
                                        APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K), 
                                        HEAPID_WAZAOSHIE );
  wk->clres[2][WO_CEL_ID_CURSOR] = GFL_CLGRP_CELLANIM_Register(
                                      p_handle, NARC_waza_oshie_gra_oam_dw_NCER,
                                      NARC_waza_oshie_gra_oam_dw_NANR, HEAPID_WAZAOSHIE );

  wk->clres[2][WO_CEL_ID_TYPE] = GFL_CLGRP_CELLANIM_Register(
                                      c_handle,
                                      APP_COMMON_GetPokeTypeCellArcIdx(APP_COMMON_MAPPING_32K),
                                      APP_COMMON_GetPokeTypeAnimeArcIdx(APP_COMMON_MAPPING_32K),
                                      HEAPID_WAZAOSHIE );


  GFL_ARC_CloseDataHandle( c_handle );

  // �|�P�������ʊG
  c_handle = POKE2DGRA_OpenHandle( HEAPID_WAZAOSHIE );

  ppp  = PP_GetPPPPointer( wk->dat->pp );
  fast = PPP_FastModeOn( ppp );
  wk->clres[0][WO_CHR_ID_POKEGRA] = POKE2DGRA_OBJ_CGR_RegisterPPP( c_handle, ppp, POKEGRA_DIR_FRONT, CLSYS_DRAW_MAIN, HEAPID_WAZAOSHIE );
  wk->clres[1][WO_PAL_ID_POKEGRA] = POKE2DGRA_OBJ_PLTT_RegisterPPP( c_handle, ppp, POKEGRA_DIR_FRONT, CLSYS_DRAW_MAIN, 4*32, HEAPID_WAZAOSHIE );
  wk->clres[2][WO_CEL_ID_POKEGRA] = POKE2DGRA_OBJ_CELLANM_RegisterPPP( ppp, POKEGRA_DIR_FRONT, APP_COMMON_MAPPING_32K, CLSYS_DRAW_MAIN, HEAPID_WAZAOSHIE );
  PPP_FastModeOff( ppp, fast );

  GFL_ARC_CloseDataHandle( c_handle );
}


static void CGR_Replace( ARCHANDLE *handle, u32 res_index, u32 ArcId, BOOL compressFlag, HEAPID heapId );
//----------------------------------------------------------------------------------
/**
 * @brief �풓�^OBJ�̃L�������\�[�X�������ւ���
 *
 * @param   handle        �ǂݏo���f�[�^�̃t�@�C���n���h��
 * @param   res_index     �����ւ���OBJ�L�������\�[�X�n���h��
 * @param   ArcId         �ǂݍ���OBJ�L�����f�[�^
 * @param   compressFlag  ���k�t���O
 * @param   heapId        �q�[�vID
 */
//----------------------------------------------------------------------------------
static void CGR_Replace( ARCHANDLE *handle, u32 res_index, u32 ArcId, BOOL compressFlag, HEAPID heapId )
{
  NNSG2dCharacterData* charData;
  void *buf = GFL_ARCHDL_UTIL_LoadOBJCharacter( handle, ArcId, compressFlag, &charData, heapId );

  GFL_CLGRP_CGR_Replace( res_index, charData );

  GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v�A�C�R���؂�ւ�
 *
 * @param wk    ���[�N
 * @param waza  �Z�ԍ�
 * @param num   �A�C�R���ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_TypeIconChange( WO_WORK * wk, u16 waza, u16 res_offset )
{
  u32 type;
  ARCHANDLE *handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_WAZAOSHIE );

  // �퓬�p
  if( wk->dat->page == WO_PAGE_BATTLE ){
    type = WAZADATA_GetParam( waza, WAZAPARAM_TYPE );
  }else{
    type = WAZADATA_GetParam( waza, WAZAPARAM_TYPE ) + ICONTYPE_STYLE;
  }

  // �L������������
  CGR_Replace( handle, wk->clres[0][res_offset+WO_CHR_ID_TYPE1],
               APP_COMMON_GetPokeTypeCharArcIdx(type), 0, HEAPID_WAZAOSHIE );

  // �p���b�g�I�t�Z�b�g�ύX
  if(res_offset < LIST_NUM){
      GFL_CLACT_WK_SetPlttOffs(wk->cap[WO_CLA_TYPE1+res_offset],
                               APP_COMMON_GetPokeTypePltOffset(type),CLWK_PLTTOFFS_MODE_PLTT_TOP);
  }else{
    GFL_CLACT_WK_SetPlttOffs( wk->cap[WO_CLA_TYPE1+res_offset],
                              APP_COMMON_GetPokeTypePltOffset(type), CLWK_PLTTOFFS_MODE_PLTT_TOP);
  }

  GFL_ARC_CloseDataHandle( handle );

}

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v�A�C�R��������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_TypeIconInit( WO_WORK * wk )
{
  u32 i;
  GFL_CLACTPOS clpos;

  for( i=0; i<LIST_NUM; i++ ){
    clpos.x = TYPE_ICON1_PX;
    clpos.y = TYPE_ICON1_PY+SEL_CURSOR_SY*i;
    GFL_CLACT_WK_SetPos( wk->cap[WO_CLA_TYPE1+i], &clpos, CLSYS_DEFREND_SUB );
    if( wk->dat->scr + i >= wk->sel_max ){
      GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_TYPE1+i], 0 );
    }else{
      GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_TYPE1+i], 1 );
      WO_TypeIconChange( wk, wk->dat->waza_tbl[wk->dat->scr+i], i );
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v�A�C�R���X�N���[��
 *
 * @param wk      ���[�N
 * @param old_scr   ���܂ł̃X�N���[���J�E���^
 * @param new_scr   �V�����X�N���[���J�E���^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_TypeIconScroll( WO_WORK * wk, u16 old_scr, u16 new_scr )
{
  u32 i;
  GFL_CLACTPOS clpos;

  if( old_scr < new_scr ){
    for( i=0; i<LIST_NUM; i++ ){
      GFL_CLACT_WK_GetPos( wk->cap[WO_CLA_TYPE1+i], &clpos, CLSYS_DEFREND_SUB);
      if( clpos.y == TYPE_ICON1_PY ){
        clpos.y = TYPE_ICON4_PY;
        if( wk->dat->waza_tbl[new_scr+LIST_NUM_OFS] != WAZAOSHIE_TBL_MAX ){
          WO_TypeIconChange( wk, wk->dat->waza_tbl[new_scr+LIST_NUM_OFS], i );
        }
      }else{
        clpos.y -= SEL_CURSOR_SY;
      }
      GFL_CLACT_WK_SetPos( wk->cap[WO_CLA_TYPE1+i], &clpos, CLSYS_DEFREND_SUB );
    }
  }else if( old_scr > new_scr ){
    for( i=0; i<LIST_NUM; i++ ){
      GFL_CLACT_WK_GetPos( wk->cap[WO_CLA_TYPE1+i], &clpos, CLSYS_DEFREND_SUB );
      if( clpos.y == TYPE_ICON4_PY ){
        clpos.y = TYPE_ICON1_PY;
        if( wk->dat->waza_tbl[new_scr] != WAZAOSHIE_TBL_MAX ){
          WO_TypeIconChange( wk, wk->dat->waza_tbl[new_scr], i );
        }
      }else{
        clpos.y += SEL_CURSOR_SY;
      }
      GFL_CLACT_WK_SetPos( wk->cap[WO_CLA_TYPE1+i], &clpos, CLSYS_DEFREND_SUB );
    }
  }else{
    return;
  }

  for( i=0; i<LIST_NUM; i++ ){
    GFL_CLACT_WK_GetPos( wk->cap[WO_CLA_TYPE1+i], &clpos, CLSYS_DEFREND_SUB );
    clpos.y = ( clpos.y - TYPE_ICON1_PY ) / SEL_CURSOR_SY;
    if( new_scr + clpos.y >= wk->sel_max ){
      GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_TYPE1+i], 0 );
    }else{
      GFL_CLACT_WK_SetDrawEnable( wk->cap[WO_CLA_TYPE1+i], 1 );
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * ���ރA�C�R���؂�ւ�
 *
 * @param wk    ���[�N
 * @param waza  �Z�ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_KindIconChange( WO_WORK * wk, u16 waza )
{
  u32 kind = WAZADATA_GetParam( waza, WAZAPARAM_DAMAGE_TYPE );
  ARCHANDLE *handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_WAZAOSHIE );

//  CATS_ChangeResourceCharArc(
//    wk->clUnit, wk->crp, WazaKindIcon_ArcIDGet(),
//    WazaKindIcon_CgrIDGet(kind), WAZAKINDICON_COMP_CHAR, WO_CHR_ID_KIND );

//  GFL_CLGRP_CGR_ReplaceSrc_VramTransfer( wk->clres[0][WO_CHR_ID_KIND],
//                                         handle, APP_COMMON_GetWazaKindCharArcIdx(kind),
//                                         0, HEAPID_WAZAOSHIE );

  CGR_Replace( handle, wk->clres[0][WO_CHR_ID_KIND],
               APP_COMMON_GetWazaKindCharArcIdx(kind), 0, HEAPID_WAZAOSHIE );


//  CATS_ObjectPaletteSetCap( wk->cap[WO_CLA_KIND], WazaKindIcon_PlttOffsetGet(kind)+TICON_ACTPAL_IDX );
  GFL_CLACT_WK_SetPlttOffs( wk->cap[WO_CLA_KIND],
                            APP_COMMON_GetWazaKindPltOffset(kind), CLWK_PLTTOFFS_MODE_PLTT_TOP );

  GFL_ARC_CloseDataHandle( handle );

}


//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_ObjInit( WO_WORK * wk, ARCHANDLE* p_handle )
{
  u32 i;

  WO_ClactResManInit( wk );
  WO_ResourceLoad( wk, p_handle );

  for( i=0; i<WO_CLA_MAX; i++ ){
    GFL_CLWK_DATA dat;
    dat.pos_x = ClactParamTbl[i].x;       // �����W
    dat.pos_y = ClactParamTbl[i].y;       // �����W
    dat.anmseq  = ClactParamTbl[i].anm;       // �A�j���[�V�����V�[�P���X
    dat.softpri = ClactParamTbl[i].pri;     // �\�t�g�D�揇�� 0>0xff
    dat.bgpri   = ClactParamTbl[i].bg_pri;        // BG�D�揇��

    OS_Printf("%d:pal=%d handle=%d\n", i, ClactParamTbl[i].id[1], wk->clres[1][ClactParamTbl[i].id[1]]);

    wk->cap[i] = GFL_CLACT_WK_Create( wk->clUnit, wk->clres[0][ClactParamTbl[i].id[0]],
                                               wk->clres[1][ClactParamTbl[i].id[1]],
                                               wk->clres[2][ClactParamTbl[i].id[2]], &dat,
                                               ClactParamTbl[i].d_area, HEAPID_WAZAOSHIE );
    GFL_CLACT_WK_SetAutoAnmFlag(wk->cap[i],FALSE);
  }
  GFL_CLACT_WK_SetAutoAnmFlag( wk->cap[WO_CLA_EXIT],TRUE );
  GFL_CLACT_WK_SetAutoAnmFlag( wk->cap[WO_CLA_CURSOR],TRUE );
  GFL_CLACT_WK_SetAutoAnmFlag( wk->cap[WO_CLA_ARROW_U], TRUE );
  GFL_CLACT_WK_SetAutoAnmFlag( wk->cap[WO_CLA_ARROW_D], TRUE );

}



/**
 *  @brief  �\�t�g�E�F�A�X�v���C�g������
 */
static void WO_SoftSpriteInit(WO_3DWORK* wk,POKEMON_PARAM* pp,int heapID)
{
//  SOFT_SPRITE_ARC ssa;  // @@@softsprite

//  wk->ssm = SoftSpriteInit(heapID);

//  PokeGraArcDataGetPP( &ssa,pp,PARA_FRONT); // @@@softsprite

//  wk->ss = SoftSpriteAdd( wk->ssm, &ssa,POKE_PX,POKE_PY, 0, 0, NULL, NULL );  // @@@softsprite
//  SoftSpriteParaSet( wk->ss, SS_PARA_H_FLIP,  // @@@softsprite
//    PokeFormNoPersonalParaGet(PokeParaGet(pp,ID_PARA_monsno,NULL),PokeParaGet(pp,ID_PARA_form_no,NULL),ID_PER_reverse )^1 );  // @@@softsprite
//    PokePersonalParaGet(PokeParaGet(pp,ID_PARA_monsno,NULL), ID_PER_reverse ) ^ 1);
}

#define GRAPHIC_G3D_TEXSIZE (GFL_G3D_TEX128K) //�o���N�̃e�N�X�`���C���[�W�X���b�g���ނƂ��킹�Ă�������
#define GRAPHIC_G3D_PLTSIZE (GFL_G3D_PLT32K)  //�o���N����گăC���[�W�X���b�g���ނƂ��킹�Ă�������

//-------------------------------------
/// �J�����ʒu
//=====================================
static const VecFx32 sc_CAMERA_PER_POS    = { 0,0,FX32_CONST( 70 ) }; //�ʒu
static const VecFx32 sc_CAMERA_PER_UP     = { 0,FX32_ONE,0 };         //�����
static const VecFx32 sc_CAMERA_PER_TARGET = { 0,0,FX32_CONST( 0 ) };  //�^�[�Q�b�g


//-------------------------------------
/// 3D�ݒ�R�[���o�b�N�֐�
//=====================================
static void wazaoshie_3d_setup( void )
{
  // �R�c�g�p�ʂ̐ݒ�(�\�����v���C�I���e�B�[)
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
  G2_SetBG0Priority(0);

  // �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
  //  �t�H�O�A�G�b�W�}�[�L���O�A�g�D�[���V�F�[�h�̏ڍאݒ��
  //  �e�[�u�����쐬���Đݒ肵�Ă�������
  G3X_SetShading( GX_SHADING_HIGHLIGHT ); //�V�F�[�f�B���O
  G3X_AntiAlias( FALSE );                 //  �A���`�G�C���A�X
  G3X_AlphaTest( FALSE, 0 );              //  �A���t�@�e�X�g�@�@�I�t
  G3X_AlphaBlend( TRUE );                 //  �A���t�@�u�����h�@�I��
  G3X_EdgeMarking( FALSE );               //  �G�b�W�}�[�L���O
  G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );  //�t�H�O

  // �N���A�J���[�̐ݒ�
  G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE); //color,alpha,depth,polygonID,fog
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


//--------------------------------------------------------------------------------------------
/**
 * 3D/�\�t�g�E�F�A�X�v���C�g������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_3DInit( WO_WORK * wk )
{
  GFL_G3D_Init( GFL_G3D_VMANLNK, GRAPHIC_G3D_TEXSIZE,
      GFL_G3D_VMANLNK, GRAPHIC_G3D_PLTSIZE, 0, HEAPID_WAZAOSHIE, wazaoshie_3d_setup );

  wk->p3d.camera = GFL_G3D_CAMERA_CreateOrtho(
                    FX32_CONST(24), -FX32_CONST(24), -FX32_CONST(32), FX32_CONST(32),
                    defaultCameraNear, defaultCameraFar, 0,
                    &sc_CAMERA_PER_POS, &sc_CAMERA_PER_UP, &sc_CAMERA_PER_TARGET, HEAPID_WAZAOSHIE );

  //�\�t�g�E�F�A�X�v���C�g������
  WO_SoftSpriteInit(&wk->p3d,wk->dat->pp,HEAPID_WAZAOSHIE);

  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
  G2_SetBG0Priority(MFRM_POKE_PRI);
}

/**
 *  @brief  �\�t�g�E�F�A�X�v���C�g�@3D���C��
 */
static void WO_3DMain(WO_3DWORK* wk)
{
//  GFC_CameraLookAt();   // @@@camera

  G3_MtxMode( GX_MTXMODE_PROJECTION );
  G3_Identity();
  G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
  G3_Identity();

  NNS_G3dGlbFlush();
  NNS_G2dSetupSoftwareSpriteCamera();
//  SoftSpriteMain( wk->ssm );

  G3_SwapBuffers( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
}


//--------------------------------------------------------------------------------------------
/**
 * 3D/�\�t�g�E�F�A�X�v���C�g���
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_3DRelease( WO_3DWORK * wk )
{
  // �J�����ݒ���
  GFL_G3D_CAMERA_Delete(wk->camera);    // @@@camera

  // G3D�V�X�e���I��
  GFL_G3D_Exit();
//  SoftSpriteEnd(wk->ssm);
//  MI_CpuClear8(wk,sizeof(WO_3DWORK));
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�I���J�[�\���؂�ւ�
 *
 * @param wk    ���[�N
 * @param pos   �\���ʒu
 * @param pal   �\���p���b�g
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WO_SelCursorChange( WO_WORK * wk, u8 pos, u8 pal )
{
  GFL_CLACTPOS clpos;
  if(pos < LIST_NUM){
    clpos.x = SEL_CURSOR_PX; clpos.y = SEL_CURSOR_PY+SEL_CURSOR_SY*pos;
    GFL_CLACT_WK_SetPos(wk->cap[WO_CLA_CURSOR], &clpos, CLSYS_DEFREND_SUB );
    GFL_CLACT_WK_SetAnmFrame(wk->cap[WO_CLA_CURSOR],0);
  }else if(pos < (LIST_NUM+2)){
    clpos.x = SEL_CURSOR_P_PX+(pos-LIST_NUM)*SEL_CURSOR_P_SX; clpos.y = SEL_CURSOR_P_PY;
    GFL_CLACT_WK_SetPos(wk->cap[WO_CLA_CURSOR], &clpos, CLSYS_DEFREND_SUB );

    GFL_CLACT_WK_SetAnmFrame(wk->cap[WO_CLA_CURSOR],1);
  }else{
    clpos.x = SEL_CURSOR_B_PX; clpos.y = SEL_CURSOR_B_PY;
    GFL_CLACT_WK_SetPos(wk->cap[WO_CLA_CURSOR], &clpos, CLSYS_DEFREND_SUB );
    GFL_CLACT_WK_SetAnmFrame(wk->cap[WO_CLA_CURSOR],2);
  }
//  GFL_CLACT_WK_SetPlttOffs( wk->cap[WO_CLA_CURSOR], pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
}



//----------------------------------------------------------------------------------
/**
 * @brief �X�N���[���J�[�\���\���ؑ�
 *
 * @param   wk      �Z�������[�N
 * @param   idx     0���Ɖ��A1���Ə�
 * @param   anm_f   
 */
//----------------------------------------------------------------------------------
static void WO_ScrollCursorPut( WO_WORK *wk, u8 idx, BOOL anm_f )
{
  u8 flag;

  if(idx == 0 && anm_f){
    flag = 1;
    GFL_CLACT_WK_ResetAnm(wk->cap[WO_CLA_ARROW_D]);
  }else{
    flag = 0;
  }
  if( wk->dat->scr + LIST_NUM < wk->sel_max ){
//    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_D],APP_COMMON_BARICON_CURSOR_DOWN+flag);
  }else{
//    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_D],ANMDW_ARROW_DF+flag);
  }

  if(idx == 1 && anm_f){
    flag = 1;
    GFL_CLACT_WK_ResetAnm(wk->cap[WO_CLA_ARROW_U]);
  }else{
    flag = 0;
  }
  if( wk->dat->scr != 0 ){
//    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_U],ANMDW_ARROW_UT+flag);
  }else{
//    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_U],ANMDW_ARROW_UF+flag);
  }
  
}



//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X��ʌĂяo���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqPstInit( WO_WORK * wk )
{
  WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT,  WIPE_TYPE_FADEOUT,
                  WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WAZAOSHIE );
  wk->next_seq = SEQ_PST_CALL;
  return SEQ_FADE_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X��ʌĂяo��
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqPstCall( WO_WORK * wk )
{
  /*
  u8 page[] = {
    PST_PAGE_PARAM_B_SKILL,   // �u���������킴�v
    PST_PAGE_MAX
  };
  */

  WO_DispExit( wk );

  wk->psd.ppd  = wk->dat->pp;
  wk->psd.cfg  = wk->dat->cfg;
  wk->psd.game_data  = GAMESYSTEM_GetGameData( wk->dat->gsys );
  wk->psd.ppt  = PST_PP_TYPE_POKEPARAM;
  wk->psd.pos  = 0;
  wk->psd.max  = 1;
  wk->psd.waza = WO_SelWazaGet( wk );
  wk->psd.mode = PST_MODE_WAZAADD;
  wk->psd.page = PPT_INFO;
//  wk->psd.ev_contest = 1;

  //wk->psd.player_name = MyStatus_GetMyName( wk->dat->myst );
  //wk->psd.player_id   = MyStatus_GetID( wk->dat->myst );
  //wk->psd.player_sex  =  MyStatus_GetMySex( wk->dat->myst );

  wk->psd.zukan_mode = 0;
//  wk->psd.pokethlon  = FALSE;


//  PokeStatus_PageSet( &wk->psd, page );


  // WB�ł�PROC�Ăяo���̓A�v���ƕ��s�̑��݂ɂȂ�̂ŁA
  // �Ăяo�������_���猻�݂̃A�v���ɂ͗��Ȃ��Ȃ�B
  GAMESYSTEM_CallProc( wk->dat->gsys,
                        FS_OVERLAY_ID(poke_status), &PokeStatus_ProcData, &wk->psd );

//  wk->subProc = GFL_PROC_Create( &PokeStatusProcData, &wk->psd, HEAPID_WAZAOSHIE );
/*��[GS_CONVERT_TAG]*/
  return SEQ_PST_WAIT;
}


// �Y���Z��I�΂Ȃ�����
#define PSTATUS_SELECT_WAZA_CANCEL    ( 4 )

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X��ʏI���҂�
 *
 * @param wk    ���[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WO_SeqPstWait( WO_WORK * wk )
{
  // �X�e�[�^�X��ʏI�����ɂ����ɗ���
    WO_DispInit( wk );
    WazaSelBgChange( wk, wk->dat->pos );
    WO_SelCursorChange( wk, wk->dat->pos, PALDW_CURSOR);
//    WO_ScrollCursorOff( wk );
    if(wk->psd.ret_mode==PST_RET_DECIDE){
      wk->dat->del_pos = wk->psd.ret_sel;
    }else{
      wk->dat->del_pos = PSTATUS_SELECT_WAZA_CANCEL;
    }
    wk->next_seq = SEQ_DEL_CHECK;
    return SEQ_FADE_WAIT;

  return SEQ_PST_WAIT;
}






//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
static void CursorMoveCallBack( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos );

static void ScrollButtonOnOff( WO_WORK * wk );
static void EnterButtonOnOff( WO_WORK * wk, BOOL flg );


#if 0
static const GFL_UI_TP_HITTBL ListHitTbl[] =
{
  {TP_WAZA1_PY,TP_WAZA1_PY+TP_WAZA_SY-1,TP_WAZA_PX,TP_WAZA_PX+TP_WAZA_SX-1},
  {TP_WAZA2_PY,TP_WAZA2_PY+TP_WAZA_SY-1,TP_WAZA_PX,TP_WAZA_PX+TP_WAZA_SX-1},
  {TP_WAZA3_PY,TP_WAZA3_PY+TP_WAZA_SY-1,TP_WAZA_PX,TP_WAZA_PX+TP_WAZA_SX-1},
  {TP_WAZA4_PY,TP_WAZA4_PY+TP_WAZA_SY-1,TP_WAZA_PX,TP_WAZA_PX+TP_WAZA_SX-1},
  {TP_SB_PY,TP_SB_PY+TP_SB_SY-1,TP_SBD_PX,TP_SBD_PX+TP_SB_SX-1},
  {TP_SB_PY,TP_SB_PY+TP_SB_SY-1,TP_SBU_PX,TP_SBU_PX+TP_SB_SX-1},
  {TP_BACK_PY,TP_BACK_PY+TP_BACK_SY-1,TP_BACK_PX,TP_BACK_PX+TP_BACK_SX-1},
  {TP_ABTN_PY,TP_ABTN_PY+TP_ABTN_SY-1,TP_ABTN_PX,TP_ABTN_PX+TP_ABTN_SX-1},
  {GFL_UI_TP_HIT_END,0,0,0}
};
static const POINTSEL_WORK ListKeyTbl[] =
{
  { 40,  52, 0, 0,  6, 1, 0, 0 }, // 00: �Z�P�i�_�~�[�Łu���ǂ�v�Ƀ��[�v�j
  { 80,  60, 0, 0,  0, 2, 1, 1 }, // 01: �Z�Q
  { 40,  84, 0, 0,  1, 3, 2, 2 }, // 02: �Z�R
  { 80,  92, 0, 0,  2, 6, 3, 3 }, // 03: �Z�S

  { 40, 116, 0, 0,  4, 4, 4, 4 }, // 04: �����
  { 80, 124, 0, 0,  5, 5, 5, 5 }, // 05: ����

  { 224, 168, 0, 0, 3, 6, 6, 6 }, // 06: ���ǂ�
  { 224, 168, 0, 0, 7, 7, 7, 7 }, // 07: ���ڂ���
};
#endif

static const CURSORMOVE_DATA ListKeyTbl[]={
  { 40,  52, 0, 0,  0, 1, 0, 0 ,  {TP_WAZA1_PY,TP_WAZA1_PY+TP_WAZA_SY-1,TP_WAZA_PX,TP_WAZA_PX+TP_WAZA_SX-1},},// 00: �Z�P�i�_�~�[�Łu���ǂ�v�Ƀ��[�v�j
  { 80,  60, 0, 0,  0, 2, 1, 1 ,  {TP_WAZA2_PY,TP_WAZA2_PY+TP_WAZA_SY-1,TP_WAZA_PX,TP_WAZA_PX+TP_WAZA_SX-1},},// 01: �Z�Q
  { 40,  84, 0, 0,  1, 3, 2, 2 ,  {TP_WAZA3_PY,TP_WAZA3_PY+TP_WAZA_SY-1,TP_WAZA_PX,TP_WAZA_PX+TP_WAZA_SX-1},},// 02: �Z�R
  { 80,  92, 0, 0,  2, 6, 3, 3 ,  {TP_WAZA4_PY,TP_WAZA4_PY+TP_WAZA_SY-1,TP_WAZA_PX,TP_WAZA_PX+TP_WAZA_SX-1},},// 03: �Z�S
  { 40, 116, 0, 0,  4, 4, 4, 4 ,  {TP_SB_PY,TP_SB_PY+TP_SB_SY-1,TP_SBD_PX,TP_SBD_PX+TP_SB_SX-1},},// 04: �����
  { 80, 124, 0, 0,  5, 5, 5, 5 ,  {TP_SB_PY,TP_SB_PY+TP_SB_SY-1,TP_SBU_PX,TP_SBU_PX+TP_SB_SX-1},},// 05: ����
  { 224, 168, 0, 0, 3, 6, 6, 6 ,  {TP_BACK_PY,TP_BACK_PY+TP_BACK_SY-1,TP_BACK_PX,TP_BACK_PX+TP_BACK_SX-1},},// 06: ���ǂ�
  { 224, 168, 0, 0, 7, 7, 7, 7 ,  {TP_ABTN_PY,TP_ABTN_PY+TP_ABTN_SY-1,TP_ABTN_PX,TP_ABTN_PX+TP_ABTN_SX-1},},// 07: ���ڂ���
  { 0, 0, 0, 0, 0, 0, 0, 0,       {GFL_UI_TP_HIT_END, 0, 0, 0 }},

};

static const CURSORMOVE_CALLBACK ListCallBack = {
  CursorMoveCallBack,
  CursorMoveCallBack,
  CursorMoveCallBack_Move,
  CursorMoveCallBack_Touch
};

//----------------------------------------------------------------------------------
/**
 * @brief �J�[�\���E�^�b�`���ʏ���������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void CursorMoveInit( WO_WORK * wk )
{
  wk->cmwk = CURSORMOVE_Create( ListKeyTbl, &ListCallBack, wk, TRUE, 0, HEAPID_WAZAOSHIE );

  ScrollButtonOnOff( wk );    // �X�N���[���ݒ�
}


//----------------------------------------------------------------------------------
/**
 * @brief �J�[�\���E�^�b�`���ʏ������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void CursorMoveExit( WO_WORK * wk )
{
  CURSORMOVE_Exit( wk->cmwk );
}

/*
static void CM_CursorPut( WO_WORK * wk, int pos )
{
}
*/

//----------------------------------------------------------------------------------
/**
 * @brief �J�[�\���^�b�`���ʏ����R�[���o�b�N
 *
 * @param   work      
 * @param   now_pos   ���݈ʒu
 * @param   old_pos   �ߋ��ʒu
 */
//----------------------------------------------------------------------------------
static void CursorMoveCallBack( void * work, int now_pos, int old_pos )
{
  // �_�~�[�֐�
}

//----------------------------------------------------------------------------------
/**
 * @brief �yCURSORMOVE�R�[���o�b�N�z�L�[���쎞
 *
 * @param   work      
 * @param   now_pos   ���݈ʒu
 * @param   old_pos   �ߋ��ʒu
 */
//----------------------------------------------------------------------------------
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
  WO_WORK * wk = work;

  // ���������
  EnterButtonOnOff( wk, FALSE );
  WazaSelBgChange( wk, 5 );

  // �Z
  if( now_pos <= 3 ){
    PMSND_PlaySE( WO_SE_LIST_MOVE );
    wk->dat->pos = now_pos;
    if( wk->dat->scr + wk->dat->pos < wk->sel_max ){
      BattleWazaParamPut( wk, WO_SelWazaGet( wk ) );
    }else{
      BattleWazaParamPut( wk, BMPMENULIST_CANCEL );
    }
  //�u���ǂ�v
  }else if( now_pos == 6 ){
    // ���̋Z����
    if( old_pos == 3 && wk->dat->scr + 4 < wk->sel_max ){
      PMSND_PlaySE( WO_SE_PAGE_MOVE );
      now_pos = 3;
      CURSORMOVE_PosSet( wk->cmwk, now_pos );
      wk->dat->scr++;
      WO_WazaListDraw( wk );
      BattleWazaParamPut( wk, WO_SelWazaGet( wk ) );
      ScrollButtonOnOff( wk );
      ScrollButtonAnmChange( wk, 1 );
      
    // ��̋Z����
    }else if( old_pos == 0 ){
      if( wk->dat->scr != 0 ){
        PMSND_PlaySE( WO_SE_PAGE_MOVE );
        wk->dat->scr--;
        WO_WazaListDraw( wk );
        BattleWazaParamPut( wk, WO_SelWazaGet( wk ) );
        ScrollButtonOnOff( wk );
        ScrollButtonAnmChange( wk, -1 );
      }
      now_pos = 0;
      CURSORMOVE_PosSet( wk->cmwk, now_pos );

    // �Z�̍Ō�Ȃ̂Łu���ǂ�Ɉړ��v�A�A�A�����Ȃ�
    }else{
      now_pos = 3;
      CURSORMOVE_PosSet( wk->cmwk, 3 );
      //PMSND_PlaySE( WO_SE_LIST_MOVE );
      //BattleWazaParamPut( wk, BMPMENULIST_CANCEL );
    }
  // ���̑�
  }else{
    PMSND_PlaySE( WO_SE_LIST_MOVE );
  }

  WO_SelCursorChange( wk, now_pos, PALDW_CURSOR );
}

//----------------------------------------------------------------------------------
/**
 * @brief �yCURSORMOVE�R�[���o�b�N�z�^�b�`��
 *
 * @param   work    
 * @param   now_pos   ���݈ʒu
 * @param   old_pos   �ߋ��ʒu
 */
//----------------------------------------------------------------------------------
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos )
{
  WO_WORK * wk = work;

  // �Z
  if( now_pos <= 3 ){
    wk->dat->pos = now_pos;
    if( wk->dat->scr + wk->dat->pos < wk->sel_max ){
      WazaSelBgChange( wk, wk->dat->scr );
      EnterButtonOnOff( wk, TRUE ); // �����\��
      BattleWazaParamPut( wk, WO_SelWazaGet( wk ) );
    }else{
      WazaSelBgChange( wk, 5 );
      EnterButtonOnOff( wk, FALSE );  // ���������
      BattleWazaParamPut( wk, BMPMENULIST_CANCEL );
    }
  // ��
  }else if( now_pos == 4 ){
    PMSND_PlaySE( WO_SE_PAGE_MOVE );
    now_pos = wk->dat->pos;
    wk->dat->scr++;
    BattleWazaParamPut( wk, WO_SelWazaGet( wk ) );
    WO_WazaListDraw( wk );
    WazaSelBgChange( wk, 5 );
    EnterButtonOnOff( wk, FALSE );  // ���������
    ScrollButtonOnOff( wk );
    ScrollButtonAnmChange( wk, 1 );
    CURSORMOVE_PosSet( wk->cmwk, now_pos );
  // ��
  }else if( now_pos == 5 ){
    PMSND_PlaySE( WO_SE_PAGE_MOVE );
    now_pos = wk->dat->pos;
    wk->dat->scr--;
    BattleWazaParamPut( wk, WO_SelWazaGet( wk ) );
    WO_WazaListDraw( wk );
    WazaSelBgChange( wk, 5 );
    EnterButtonOnOff( wk, FALSE );  // ���������
    ScrollButtonOnOff( wk );
    ScrollButtonAnmChange( wk, -1 );
    CURSORMOVE_PosSet( wk->cmwk, now_pos );
  // ���ǂ�
  }else if( now_pos == 6 ){
    WazaSelBgChange( wk, 5 );
    if( wk->cm_pos != 6 ){
      now_pos = wk->dat->pos;
      CURSORMOVE_PosSet( wk->cmwk, now_pos );
    }
  // �����Ă�
  }else if( now_pos == 7 ){
    now_pos = wk->dat->pos;
    CURSORMOVE_PosSet( wk->cmwk, now_pos );
  }

  OS_Printf("now_pos=%d\n", now_pos);
  WO_SelCursorChange( wk, now_pos, PALDW_CURSOR );
}

//----------------------------------------------------------------------------------
/**
 * @brief �{�^��ON/OFF
 *
 * @param   wk    
 * @param   flg   
 */
//----------------------------------------------------------------------------------
static void EnterButtonOnOff( WO_WORK * wk, BOOL flg )
{
  if(flg==TRUE)
  {
    if(wk->oboe_menu_work[0]==NULL){
      wk->oboe_menu_work[0] = APP_TASKMENU_WIN_Create( wk->app_res, &wk->menuitem[0], 13, 21, 9, HEAPID_WAZAOSHIE );
      wk->oboe_menu_work[1] = APP_TASKMENU_WIN_Create( wk->app_res, &wk->menuitem[1], 22, 21, 9, HEAPID_WAZAOSHIE );
      wk->enter_flg = 1;
    }
  }else {
    if(wk->oboe_menu_work[0]!=NULL){
      APP_TASKMENU_WIN_Delete( wk->oboe_menu_work[0] );
      APP_TASKMENU_WIN_Delete( wk->oboe_menu_work[1] );
      wk->oboe_menu_work[0] = NULL;
      wk->oboe_menu_work[1] = NULL;
    }
    wk->enter_flg = 0;
  }

}

//----------------------------------------------------------------------------------
/**
 * @brief �u���ڂ���v�u��߂�v�͕\������Ă��邩
 *
 * @param   wk    
 *
 * @retval  int   1:����  0:���Ȃ�
 */
//----------------------------------------------------------------------------------
static int EnterButtonCheck( WO_WORK *wk )
{
  if( wk->oboe_menu_work[0]!=NULL ){
    return  1;
  }
  
  return 0;
}

//----------------------------------------------------------------------------------
/**
 * @brief 
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void ScrollButtonOnOff( WO_WORK * wk )
{
  if( wk->sel_max < 4 ){
    CURSORMOVE_MoveTableBitOff( wk->cmwk, 4 );
    CURSORMOVE_MoveTableBitOff( wk->cmwk, 5 );
    return;
  }

  if( wk->dat->scr == 0 ){
    CURSORMOVE_MoveTableBitOff( wk->cmwk, 5 );
  }else{
    CURSORMOVE_MoveTableBitOn( wk->cmwk, 5 );
  }

  if( wk->dat->scr + 4 >= wk->sel_max ){
    CURSORMOVE_MoveTableBitOff( wk->cmwk, 4 );
  }else{
    CURSORMOVE_MoveTableBitOn( wk->cmwk, 4 );
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief �㉺�{�^���\���A�j���ύX
 *
 * @param   wk    
 * @param   mv    
 */
//----------------------------------------------------------------------------------
static void ScrollButtonAnmChange( WO_WORK * wk, s32 mv )
{
  GFL_CLACT_WK_SetAnmFrame( wk->cap[WO_CLA_ARROW_U], 0 );
  GFL_CLACT_WK_SetAnmFrame( wk->cap[WO_CLA_ARROW_D], 0 );

  // ���Ɉړ�
  if( mv > 0 ){
    if( wk->dat->scr + 4 < wk->sel_max ){
      GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_D], ANMDW_ARROW_DTA);
    }else{
      GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_D], ANMDW_ARROW_DFA );
    }
    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_U], ANMDW_ARROW_UT );
  }else{
  // ��Ɉړ�
    if( wk->dat->scr == 0 ){
      GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_U], ANMDW_ARROW_UFA);
    }else{
      GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_U], ANMDW_ARROW_UTA );
    }
    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_D], ANMDW_ARROW_DT );
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief ��ʏ��������̃X�N���[�����A�C�R���̌����ڐݒ�
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void ScrollButtonInit( WO_WORK *wk )
{
  if(wk->dat->scr+4 < wk->sel_max){
    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_U], ANMDW_ARROW_UF );
    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_D], ANMDW_ARROW_DT );
  }else if(wk->dat->scr==0){
    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_U], ANMDW_ARROW_UT );
    GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_ARROW_D], ANMDW_ARROW_DF );
    
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief �T�u��ʃp�[�c�`��
 *
 * @param   wk    
 * @param   pos   
 */
//----------------------------------------------------------------------------------
static void WazaSelBgChange( WO_WORK * wk, u32 pos )
{
  u32 i;


  return ;
  for( i=0; i<4; i++ ){
    if( i == pos ){
      WO_SubBGPartsDraw(
        wk, BG_WLIST_PX, WIN_LIST_PY+(i*BG_WLIST_SY),
        BG_WLIST_SX, BG_WLIST_SY, BG_WLIST_SPX, BG_WLIST_SPY+BG_WLIST_SY*2 );
    }else{
      if( i >= wk->sel_max ){
        WO_SubBGPartsDraw(
          wk, BG_WLIST_PX, WIN_LIST_PY+(i*BG_WLIST_SY),
          BG_WLIST_SX, BG_WLIST_SY, BG_WLIST_SPX, BG_WLIST_SPY+BG_WLIST_SY );
      }else{
        WO_SubBGPartsDraw(
          wk, BG_WLIST_PX, WIN_LIST_PY+(i*BG_WLIST_SY),
          BG_WLIST_SX, BG_WLIST_SY, BG_WLIST_SPX, BG_WLIST_SPY );
      }
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief �I���{�^���A�j���J�n
 *
 * @param   wk    
 * @param   next    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int RetButtonAnmInit( WO_WORK * wk, int next )
{

  BUTTON_ANM_WORK * bawk = &wk->button_anm_work;
/*
  bawk->px = 24;
  bawk->py = 20;
  bawk->sx = 8;
  bawk->sy = 4;

  bawk->cnt = 0;
  bawk->seq = 0;
*/

  bawk->mode  = WO_ANIME_OBJ;
  bawk->index = WO_CLA_EXIT;
  bawk->next  = next;
  // ���ǂ�}�[�N�A�j��
  GFL_CLACT_WK_SetAnmSeq( wk->cap[WO_CLA_EXIT], 9 );
  return SEQ_BUTTON_ANM;
}

//----------------------------------------------------------------------------------
/**
 * @brief �u���ڂ���v�{�^���A�j���J�n
 *
 * @param   wk    
 * @param   next    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int EnterButtonAnmInit( WO_WORK * wk, int next )
{
  BUTTON_ANM_WORK * bawk = &wk->button_anm_work;
/*
  bawk->px = 16;
  bawk->py = 20;
  bawk->sx = 8;
  bawk->sy = 4;

  bawk->cnt = 0;
  bawk->seq = 0;
*/
  bawk->mode  = WO_ANIME_APPTASK;
  bawk->index = 0;
  bawk->next  = next;
  OS_Printf("index=%d, mode=%d,next=%d, adr=%08x", bawk->index, bawk->mode, bawk->next, (u32)wk->oboe_menu_work[bawk->index]);
  APP_TASKMENU_WIN_SetDecide( wk->oboe_menu_work[bawk->index], TRUE );
  return SEQ_BUTTON_ANM;
}

//----------------------------------------------------------------------------------
/**
 * @brief �u��߂�v�{�^���A�j���J�n
 *
 * @param   wk    
 * @param   next    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int YameruButtonAnmInit( WO_WORK *wk, int next )
{

  BUTTON_ANM_WORK * bawk = &wk->button_anm_work;
  bawk->mode  = WO_ANIME_APPTASK;
  bawk->index = 1;
  bawk->next  = next;
  OS_Printf("index=%d, mode=%d,next=%d, adr=%08x", bawk->index, bawk->mode, bawk->next, (u32)wk->oboe_menu_work[bawk->index]);
  APP_TASKMENU_WIN_SetDecide( wk->oboe_menu_work[bawk->index], TRUE );
  return SEQ_BUTTON_ANM;

}

//----------------------------------------------------------------------------------
/**
 * @brief �{�^���A�j���҂�
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int ButtonAnmMaun( WO_WORK * wk )
{
  BUTTON_ANM_WORK * bawk = &wk->button_anm_work;

  // �w���OBJ��APPTASKMENU���ŏI���҂����s��
  if(bawk->mode==WO_ANIME_OBJ){
    if(GFL_CLACT_WK_CheckAnmActive( wk->cap[bawk->index])==FALSE){
      return bawk->next;
    }
  }else if(bawk->mode==WO_ANIME_APPTASK){
    if(APP_TASKMENU_WIN_IsFinish(wk->oboe_menu_work[bawk->index])){
//      APP_TASKMENU_WIN_ResetDecide(wk->oboe_menu_work[bawk->index]);
        EnterButtonOnOff( wk, FALSE );
      return bawk->next;
    }
  }

  return SEQ_BUTTON_ANM;
}


static int WO_SeqRetButton( WO_WORK * wk )
{
//  WO_SelCursorChange( wk, wk->dat->pos, PALDW_CURSOR);
//  WO_ScrollCursorOff( wk );
  WO_TalkMsgSet( wk, MSG_END_CHECK );
  wk->ynidx = YESNO_END_CHECK;
  wk->next_seq = SEQ_YESNO_PUT;
  return SEQ_MSG_WAIT;
}

static int WO_SeqEnterButton( WO_WORK * wk )
{
  return WazaSelectEnter( wk );
}

static int WazaSelectEnter( WO_WORK * wk )
{
  if( WO_WazaSetCheck( wk ) < 4 ){
    WO_TalkMsgSet( wk, MSG_SET_CHECK );
    wk->ynidx = YESNO_SET_CHECK;
    wk->next_seq = SEQ_YESNO_PUT;
  }else{
    WO_TalkMsgSet( wk, MSG_CHG_CHECK );
    wk->ynidx = YESNO_CHG_CHECK;
    wk->next_seq = SEQ_YESNO_PUT;
  }
  return SEQ_MSG_WAIT;
}

static void PassiveSet( BOOL flg )
{
  if( flg == TRUE ){
    G2S_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 |
      GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ, -8 );
  }else{
    G2S_BlendNone();
  }
}

