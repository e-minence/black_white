//============================================================================================
/**
 * @file  wifi_p2pmatch.c
 * @bfief WIFIP2P�}�b�`���O���
 * @author  k.ohno
 * @date  06.04.07
 */
//============================================================================================

#include <gflib.h>
#include <dwc.h>
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_wifi_lobby.h"

#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/dwc_rapfriend.h"
#include "net_app/wificlub/wifi_p2pmatch.h"
#include "wifi_p2pmatch_local.h"
#include "wifi_p2pmatch_se.h"
#include "wifi_p2pmatchroom.h"


#include "poke_tool/regulation_def.h"

#include "savedata/wifihistory.h"
#include "savedata/config.h"
#include "savedata/battle_box_save.h"
#include "savedata/etc_save.h"

#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"
#include "system/main.h"
#include "system/wipe.h"
#include "system/rtc_tool.h"
#include "system/ds_system.h"

#include "gamesystem/msgspeed.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/pokeparty.h"
#include "item/itemsym.h"
#include "system/net_err.h"
#include "net/dwc_error.h"



#include "font/font.naix"
#include "print/str_tool.h"

#include "wifip2pmatch.naix"      // �O���t�B�b�N�A�[�J�C�u��`
#include "comm_command_wfp2pmf.h"  //�Q�c�t�B�[���h
#include "comm_command_wfp2pmf_func.h"  //�Q�c�t�B�[���h
#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_system.h"

#include "sound/pm_sndsys.h"  //SOUND�֘A
#include "sound/pm_wb_voice.h"




// �u��������K�v�����邪�܂��Ȃ��֐�  @@OO
#define SND_HANDLE_FIELD (0)
#define BGM_POKECEN_VOL (0)
#define SND_SCENE_P2P (0)
#define SEQ_WIFILOBBY (0)
#define SND_SCENE_DUMMY (0)
#define BGM_WIFILOBBY_VOL (0)
#define FLAG_ARRIVE_D32R0101 (0)
#define GMDATA_ID_EMAIL (0)
#define ID_PARA_monsno (0)
#define ID_PARA_item (0)
#define NUMBER_DISPTYPE_ZERO (STR_NUM_DISP_ZERO)
#define NUMBER_CODETYPE_DEFAULT (0)
#define FBMP_COL_NULL   (0)
#define FBMP_COL_BLACK    (1)
#define FBMP_COL_BLK_SDW  (2)
#define FBMP_COL_RED    (3)
#define FBMP_COL_RED_SDW  (4)
#define FBMP_COL_GREEN    (5)
#define FBMP_COL_GRN_SDW  (6)
#define FBMP_COL_BLUE   (7)
#define FBMP_COL_BLU_SDW  (8)
#define FBMP_COL_PINK   (9)
#define FBMP_COL_PNK_SDW  (10)
#define FBMP_COL_WHITE    (15)
#define BGM_FADE_VCHAT_TIME (0)
#define BGM_WIFILOBBY_VOL (0)
#define BGM_FADEIN_START_VOL_NOW (0)
#define _BATTLE_TOWER_REC (0)   // �o�g���^���[�̋L�^�\���𕕈�

#define _OAMBUTTON (0)  //����ʂ̃{�^���͗v��Ȃ�
#define MYSTATUS_GETTRAINERVIEW_COMPLETION (0)   //���j�I���̎p�������œ���Ȃ��Ƃ��܂��o�Ȃ�

typedef struct{
  int a;
} ZUKAN_WORK;


//static void Snd_PlayerSetInitialVolume(int a,int b){}
//static void TimeWaitIconTaskDel(void* c){}
//static void Snd_DataSetByScene( int a, int b, int c ){}
//static void Snd_SceneSet( int a ){}
//static int Snd_NowBgmNoGet(void){ return 0;}
//static void Snd_PlayerSetInitialVolumeBySeqNo( int a,  int b){}
//static void* SaveData_GetEventWork(void* a){ return NULL; }
//static BOOL SysFlag_ArriveGet(void* a,int b){ return TRUE;}
//static void* SaveData_Get(void* a, int b){ return NULL; }
//static void* SaveData_GetFrontier(void* a){ return NULL; }
//static void EMAILSAVE_Init(void* a){}
//static ZUKAN_WORK* SaveData_GetZukanWork(SAVE_CONTROL_WORK* a){ return NULL; }
//static int PokeParaGet( POKEMON_PARAM* poke, int no, void* c ){return 0;}
//static BOOL ZukanWork_GetZenkokuZukanFlag(ZUKAN_WORK* pZukan){ return TRUE; }
//static void CommInfoFinalize(void){}
//static void Snd_SePlay(int a){}
//static void Snd_BgmFadeOut( int a, int b){}
//static void Snd_BgmFadeIn( int a, int b, int c){}
//static void FONTOAM_OAMDATA_Delete( void* x){}
static BOOL _playerDirectConnectStart( WIFIP2PMATCH_WORK *wk );



#define COMM_BRIGHTNESS_SYNC  ( 1 ) // ��{�̋P�x�ύXSync��
#define _BMPMENULIST_FONTSIZE   (16)

// WIFI2DMAP�V�X�e���I�[�o�[���C
//FS_EXTERN_OVERLAY(wifi2dmap);

// WIFI�@�ΐ�AUTOӰ�ރf�o�b�N
#ifdef _WIFI_DEBUG_TUUSHIN
extern WIFI_DEBUG_BATTLE_WK WIFI_DEBUG_BATTLE_Work;
#endif  //_WIFI_DEBUG_TUUSHIN


//-------------------------------------
/// �}�b�`���O�p�g���q�[�v�T�C�Y
//=====================================
#define WIFI_P2PMATCH_EXHEAP_SIZE ( 0x10000 )


#define SE_CALL_SIGN  (SEQ_SE_SYS_101)  //SEQ_SE_SYS_101  ,SEQ_SE_W055_01
#define SE_CALL_SIGN_PLAYER  (SEPLAYER_SYS)  //SEQ_SE_SYS_101


/*********************************************************************************************
  ���C����ʂ�CGX����U��   2006/01/12

    �E�B���h�E�g  �F  409 - 511
      ��b�A���j���[�A�n���A�Ŕ�

    BMP�E�B���h�E�P �F  297 - 408
      ��b�i�ő�j�A�ŔA�c��{�[����

    BMP�E�B���h�E�Q �F  55 - 296
      ���j���[�i�ő�j�A�͂�/�������A�n��

 *********************************************************************************************/

/*********************************************************************************************
  �E�B���h�E�g
 *********************************************************************************************/
// ��b�E�B���h�E�L����
#define TALK_WIN_CGX_SIZE ( 18+12 )
#define TALK_WIN_CGX_NUM  ( 512 - TALK_WIN_CGX_SIZE )

// ���j���[�E�B���h�E�L����
#define MENU_WIN_CGX_SIZE ( 9 )
#define MENU_WIN_CGX_NUM  ( TALK_WIN_CGX_NUM - MENU_WIN_CGX_SIZE )
//#define MENU_WIN_PAL    ( 11 )

// �n���E�B���h�E�L����
#define PLACE_WIN_CGX_SIZE  ( 10 )
#define PLACE_WIN_CGX_NUM ( MENU_WIN_CGX_NUM - PLACE_WIN_CGX_SIZE )
//#define PLACE_WIN_PAL   ( 7 )

// �ŔE�B���h�E�L����
#define BOARD_WIN_CGX_SIZE  ( 18+12 + 24 )
#define BOARD_WIN_CGX_NUM ( PLACE_WIN_CGX_NUM - BOARD_WIN_CGX_SIZE )
//#define BOARD_WIN_PAL   ( FLD_BOARD2FRAME_PAL )

/*********************************************************************************************
  BMP�E�B���h�E
 *********************************************************************************************/
// ��b�E�B���h�E�i���C���j
#define FLD_MSG_WIN_PX    ( 2 )
#define FLD_MSG_WIN_PY    ( 19 )
#define FLD_MSG_WIN_SX    ( 27 )
#define FLD_MSG_WIN_SY    ( 4 )
#define FLD_MSG_WIN_CGX   ( BOARD_WIN_CGX_NUM - ( FLD_MSG_WIN_SX * FLD_MSG_WIN_SY ) )
// �͂�/�������E�B���h�E�i���C���j�i���j���[�Ɠ����ʒu�i���j���[��菬�����j�j
#define FLD_YESNO_WIN_PX  ( 24 )
#define FLD_YESNO_WIN_PY  ( 13 )
#define FLD_YESNO_WIN_SX  ( 6 )
#define FLD_YESNO_WIN_SY  ( 4 )
#define FLD_YESNO_WIN_CGX ( FLD_MSG_WIN_CGX - ( FLD_YESNO_WIN_SX * FLD_YESNO_WIN_SY ) )



/// �r�b�g�}�b�v�]���֐��p��`
//------------------------------------------------------------------
#define FBMP_TRANS_OFF  (0)
#define FBMP_TRANS_ON (1)
/// �����\���]���֐��p��`
//------------------------------------------------------------------
#define FBMPMSG_WAITON_SKIPOFF  (0)
#define FBMPMSG_WAITON_SKIPON (1)
#define FBMPMSG_WAITOFF     (2)

//-------------------------------------------------------------------------
/// �����\���F��`(default) -> gflib/fntsys.h�ֈړ�
//------------------------------------------------------------------

#define WINCLR_COL(col) (((col)<<4)|(col))

//-------------------------------------------------------------------------
/// �����\���X�s�[�h��`(default)
//------------------------------------------------------------------
#define FBMP_MSG_SPEED_SLOW   (8)
#define FBMP_MSG_SPEED_NORMAL (4)
#define FBMP_MSG_SPEED_FAST   (1)



//============================================================================================
//  �萔��`
//============================================================================================
enum {
  SEQ_IN = 0,
  SEQ_MAIN,
  SEQ_OUT,
};

enum {
  _MENU_LIST,
  _MENU_INPUT,
  _MENU_MYCODE,
  _MENU_EXIT,
};

typedef enum {
  _CLACT_UP_CUR,
  _CLACT_DOWN_CUR,
  _CLACT_LINE_CUR,
  _CLACT_VCT_STOP,
  _CLACT_VCT_MOVE,
} _OAM_ANIM_E;

typedef struct{  // �X�N���[���pRECT�\����
  u8 lt_x;
  u8 lt_y;
  u8 rb_x;
  u8 rb_y;
} _SCR_RECT;

#define WF_CLACT_WKNUM  ( 8 )
#define WF_CLACT_RESNUM ( 1 )
#define WF_FONTOAM_NUM  ( 1 )

#define _PRINTTASK_MAX (8)
#define _TIMING_GAME_CHECK  (13)// �Ȃ���������
#define _TIMING_GAME_CHECK2  (14)// �Ȃ���������
#define _TIMING_GAME_START  (15)// �^�C�~���O�����낦��
#define _TIMING_GAME_START2  (18)// �^�C�~���O�����낦��
#define _TIMING_BATTLE_END  (16)// �^�C�~���O�����낦��
#define _TIMING_POKEPARTY_END  (17)// �^�C�~���O�����낦��
#define _TIMING_VCTEND  (18)// �^�C�~���O�����낦��
#define _TIMING_VCTST  (19)// �^�C�~���O�����낦��
#define _TIMING_DISCONNECT_END  (21)// �^�C�~���O�����낦��
#define _TIMING_SECOND_MATCH  (24)// �^�C�~���O�����낦��
#define _TIMING_CEND (25)// �^�C�~���O�����낦��

#define _RECONECTING_WAIT_TIME (20)  //�Đڑ�����


// ���[�U�[�\���ʂ̐ݒ�
#define WIFIP2PMATCH_PLAYER_DISP_X  ( 1 )
#define WIFIP2PMATCH_PLAYER_DISP_Y  ( 1 )
#define WIFIP2PMATCH_PLAYER_DISP_SIZX ( 28 )
#define WIFIP2PMATCH_PLAYER_DISP_SIZY ( 2 )
#define WIFIP2PMATCH_PLAYER_DISP_NAME_X ( 32 )
#define WIFIP2PMATCH_PLAYER_DISP_ST_X ( 102 )
#define WIFIP2PMATCH_PLAYER_DISP_WINSIZ (WIFIP2PMATCH_PLAYER_DISP_SIZX*WIFIP2PMATCH_PLAYER_DISP_SIZY)


//FRAME3
#define _CGX_BOTTOM ( 512 )             // ��b�E�B���h�E�L����
#define _CGX_TITLE_BOTTOM (_CGX_BOTTOM - 18*3)  // �^�C�g���E�B���h�E
#define _CGX_USER_BOTTOM  (_CGX_TITLE_BOTTOM - WIFIP2PMATCH_PLAYER_DISP_WINSIZ) // ���[�U�[�E�B���h�E

// FRAME1�ɓ]�����郆�[�U�[�f�[�^�w�i
#define _CGX_USET_BACK_BOTTOM ( MENU_WIN_CGX_NUM - WIFIP2PMATCH_PLAYER_DISP_WINSIZ )


// FRAME1�ɓ]������ICON�f�[�^
#define PLAYER_DISP_ICON_PLTTOFS  (8)
#define PLAYER_DISP_ICON_PLTTOFS_SUB (8)  //9-10-11-12

#define COMM_MESFONT_PAL      ( 14 )     //  �E�C���h�E�̃p���b�g
#define MCV_SYSFONT_PAL       ( 15 )     // �V�X�e���t�H���g�̃p���b�g
#define COMM_MESFRAME_PAL     ( 14 )         //  �E�C���h�E
#define FLD_YESNO_WIN_PAL     ( 15 )  //�t�H���g�̃p���b�g
#define MENU_WIN_PAL          ( 14 )   //�E�C���h�E


//MCV_SYSFONT_PAL



#define PLAYER_DISP_ICON_PLTTNUM  (4)


#define PLAYER_DISP_ICON_CG_SIZX  (12)
#define PLAYER_DISP_ICON_CG_SIZY  (4)
#define PLAYER_DISP_ICON_CGX    ( 0 )
#define PLAYER_DISP_ICON_SCRN_X ( 2 )
#define PLAYER_DISP_ICON_SCRN_Y ( 2 )
#define PLAYER_DISP_ICON_POS_X  ( 2 )
#define PLAYER_DISP_ICON_POS_Y  ( 1 )
#define PLAYER_DISP_VCTICON_POS_X ( 26 )
#define PLAYER_DISP_VCTICON_POS_Y ( 1 )
enum{
  PLAYER_DISP_ICON_IDX_NONE_NUKI = 0,  //�����F
  PLAYER_DISP_ICON_IDX_NORMAL_ACT,
  PLAYER_DISP_ICON_IDX_NORMAL,
  PLAYER_DISP_ICON_IDX_VCT_ACT,
  PLAYER_DISP_ICON_IDX_VCT,
  PLAYER_DISP_ICON_IDX_TVT_ACT,
  PLAYER_DISP_ICON_IDX_TVT,
  PLAYER_DISP_ICON_IDX_FIGHT_ACT,
  PLAYER_DISP_ICON_IDX_FIGHT,
  PLAYER_DISP_ICON_IDX_CHANGE_ACT,
  PLAYER_DISP_ICON_IDX_CHANGE,
  PLAYER_DISP_ICON_IDX_VCTNOT,
  PLAYER_DISP_ICON_IDX_VCTBIG,
  PLAYER_DISP_ICON_IDX_UNK_ACT,      //���̑��s��
  PLAYER_DISP_ICON_IDX_UNK,      //���̑��s��
  PLAYER_DISP_ICON_IDX_NONE,  //��
  PLAYER_DISP_ICON_IDX_NUM,    //��
};


#define PLAYER_DISP_ICON_MYMODE (PLAYER_DISP_ICON_IDX_NUM*3)
#define PLAYER_DISP_ICON_TOUCHMODE (PLAYER_DISP_ICON_IDX_NUM*1)
#define PLAYER_DISP_ICON_NAMEMODE (0)
#define PLAYER_DISP_ICON_CARDMODE (PLAYER_DISP_ICON_IDX_NUM*2)


// ��b�E�B���h�E FRAME2
//#define COMM_MENUFRAME_PAL    ( 11 )         //  ���j���[�E�C���h�E
//#define COMM_SYSFONT_PAL    ( 13 )         //  �V�X�e���t�H���g

#define COMM_TALK_WIN_CGX_SIZE  ( 18+12 )
//#define COMM_TALK_WIN_CGX_NUM ( 512 - COMM_TALK_WIN_CGX_SIZE)
#define COMM_TALK_WIN_CGX_NUM ( 48 )

#define COMM_MSG_WIN_PX   ( 2 )
#define COMM_MSG_WIN_PY   ( 19 )
#define COMM_MSG_WIN_SX   ( 27 )
#define COMM_MSG_WIN_SY   ( 4 )
#define COMM_MSG_WIN_PAL    ( COMM_MESFONT_PAL )
//#define COMM_MSG_WIN_CGX    ( (COMM_TALK_WIN_CGX_NUM - 73) - ( COMM_MSG_WIN_SX * COMM_MSG_WIN_SY ) )

#define COMM_SYS_WIN_PX   ( 4 )
#define COMM_SYS_WIN_PY   ( 4 )
#define COMM_SYS_WIN_SX   ( 23 )
#define COMM_SYS_WIN_SY   ( 16 )
#define COMM_SYS_WIN_PAL    ( COMM_MESFONT_PAL )
#define COMM_SYS_WIN_CGX    ( MENU_WIN_CGX_NUM - ( COMM_SYS_WIN_SX * COMM_SYS_WIN_SY ) )  // �ʐM�V�X�e���E�B���h�E�]����


#define REGWINDOW_WIN_PX   ( 2 )
#define REGWINDOW_WIN_PY   ( 5 )
#define REGWINDOW_WIN_SX   ( 26 )
#define REGWINDOW_WIN_SY   ( 16 )


// YesNoWin
#define YESNO_WIN_CGX ((COMM_SYS_WIN_CGX - (FLD_YESNO_WIN_SX*FLD_YESNO_WIN_SY)))
#define FRAME1_YESNO_WIN_CGX  ((_CGX_USET_BACK_BOTTOM - (FLD_YESNO_WIN_SX*FLD_YESNO_WIN_SY)))


// ���C�����b�Z�[�W�L����
#define _NUKI_FONT_PALNO  (13)
#define _COL_N_BLACK  ( GFL_FONTSYS_SetColor( 1, 2, 0 ) )   // �t�H���g�J���[�F��
#define _COL_N_WHITE  ( GFL_FONTSYS_SetColor( 15, 2, 0 ) )   // �t�H���g�J���[�F��
#define _COL_N_RED      ( GFL_FONTSYS_SetColor( 3, 4, 0 ) )   // �t�H���g�J���[�F��
#define _COL_N_BLUE     ( GFL_FONTSYS_SetColor( 5, 6, 0 ) )   // �t�H���g�J���[�F��
#define _COL_N_GRAY   ( GFL_FONTSYS_SetColor( 1, 2, 0 ) )    // �t�H���g�J���[�F�D

// ���O�\��BMP�i���ʁj
#define WIFIP2PMATCH_NAME_BMP_W  ( 16 )
#define WIFIP2PMATCH_NAME_BMP_H  (  2 )
#define WIFIP2PMATCH_NAME_BMP_SIZE (WIFIP2PMATCH_NAME_BMP_W * WIFIP2PMATCH_NAME_BMP_H)

// ��b�E�C���h�E�\���ʒu��`
#define WIFIP2PMATCH_TALK_X   (  2 )
#define WIFIP2PMATCH_TALK_Y   (  19 )

#define WIFIP2PMATCH_TITLE_X    (   3  )
#define WIFIP2PMATCH_TITLE_Y    (   1  )
#define WIFIP2PMATCH_TITLE_W    (  26  )
#define WIFIP2PMATCH_TITLE_H    (   2  )


#define WIFIP2PMATCH_MSG_WIN_OFFSET     (1 + TALK_WIN_CGX_SIZ     + MENU_WIN_CGX_SIZ)
#define WIFIP2PMATCH_TITLE_WIN_OFFSET   ( WIFIP2PMATCH_MSG_WIN_OFFSET   + FLD_MSG_WIN_SX*FLD_MSG_WIN_SY )
#define WIFIP2PMATCH_NAME_WIN_OFFSET    ( WIFIP2PMATCH_TITLE_WIN_OFFSET + WIFIP2PMATCH_TITLE_W*WIFIP2PMATCH_TITLE_H )
#define WIFIP2PMATCH_YESNO_WIN_OFFSET   ( WIFIP2PMATCH_NAME_WIN_OFFSET  + WIFIP2PMATCH_NAME_BMP_SIZE*5 )


// VIEW��
enum{
  MCV_BTTN_FRIEND_TYPE_NONE,  // �ݒ肵�Ă��Ȃ�
  MCV_BTTN_FRIEND_TYPE_RES, // �\��
  MCV_BTTN_FRIEND_TYPE_IN,    // �o�^�ς�
  MCV_BTTN_FRIEND_TYPE_MAX, // �^�C�v�ő�l
};
#define MCV_PAL_BACK    ( 0 ) // �w�i�p���b�g�̊J�n�ʒu
#define MCV_PAL_FRMNO   ( 0 ) // �w�i�̃o�b�g�p���b�g�J�n�ʒu
#define MCV_PAL_BTTN    ( 0 ) // �{�^���p���b�g�̊J�n�ʒu
enum{
  //  MCV_PAL_BACK_0 = 0,
  //MCV_PAL_BACK_NUM,

  MCV_PAL_BTTN_GIRL = 0,
  MCV_PAL_BTTN_MAN,
  MCV_PAL_BTTN_NONE,
  MCV_PAL_BTTNST_GIRL,
  MCV_PAL_BTTNST_MAN,
  MCV_PAL_BTTNST_DUMMY1,
  MCV_PAL_BTTNST_DUMMY2,
  MCV_PAL_BTTNST_DUMMY3,
  MCV_PAL_BTTN_NUM  // ���̂Ƃ���]���
    // MCV_PAL_BTTN+MCV_PAL_BTTN_NUM�`(PLAYER_DISP_ICON_PLTTOFS_SUB-1�܂�
};
// �A�C�R���̓]���ʒu
#define MCV_ICON_CGX  (0)
#define MCV_ICON_CGSIZ  (32*4)
#define MCV_ICON_PAL    (PLAYER_DISP_ICON_PLTTOFS_SUB)

#define MCV_CGX_BTTN2 (MCV_ICON_CGX+MCV_ICON_CGSIZ) // FRAME2���[�U�[�f�[�^
#define MCV_CGX_BACK  (0)// FRAME0�w�i
// WIN�ݒ�
#define MCV_NAMEWIN_CGX   ( 1 ) // ���O�E�B���h�E�J�n�ʒu
#define MCV_NAMEWIN_DEFX  ( 4 ) // ��{�ʒu
#define MCV_NAMEWIN_DEFY  ( 1 )
#define MCV_NAMEWIN_OFSX  ( 16 )  // �ʒu�̈ړ��l
#define MCV_NAMEWIN_OFSY  ( 6 )
#define MCV_NAMEWIN_SIZX  ( 9 ) // �T�C�Y
#define MCV_NAMEWIN_SIZY  ( 3 )
#define MCV_NAMEWIN_CGSIZ ( MCV_NAMEWIN_SIZX*MCV_NAMEWIN_SIZY ) // CG�T�C�Y
#define MCV_NAMEWIN_CGALLSIZ  ((MCV_NAMEWIN_CGSIZ*WCR_MAPDATA_1BLOCKOBJNUM)+MCV_NAMEWIN_CGX)// CG�����T�C�Y

#define MCV_STATUSWIN_CGX ( MCV_NAMEWIN_CGALLSIZ+1 )
#define MCV_STATUSWIN_DEFX  ( 1 ) // ��{�ʒu
#define MCV_STATUSWIN_DEFY  ( 1 )
#define MCV_STATUSWIN_VCHATX ( 12 ) // VCHAT�ʒu
#define MCV_STATUSWIN_OFSX  ( 16 )  // �ʒu�̈ړ��l
#define MCV_STATUSWIN_OFSY  ( 6 )
#define MCV_STATUSWIN_SIZX  ( 2 ) // �T�C�Y
#define MCV_STATUSWIN_SIZY  ( 3 )
#define MCV_STATUSWIN_CGSIZ ( MCV_STATUSWIN_SIZX*MCV_STATUSWIN_SIZY ) // CG�T�C�Y

#define MCV_USERWIN_CGX   ( 1 )
#define MCV_USERWIN_X   ( 1 )
#define MCV_USERWIN_Y   ( 1 )
#define MCV_USERWIN_SIZX  ( 30 )
#define MCV_USERWIN_SIZY  ( 21 )


#define MCV_BUTTON_SIZX ( 16 )
#define MCV_BUTTON_SIZY ( 6 )
#define MCV_BUTTON_DEFX (0)
#define MCV_BUTTON_DEFY (0)
#define MCV_BUTTON_OFSX (16)
#define MCV_BUTTON_OFSY (6)

#define MCV_BUTTON_ICON_OFS_X ( 1 )
#define MCV_BUTTON_VCTICON_OFS_X  ( 13 )
#define MCV_BUTTON_ICON_OFS_Y ( 1 )

#define MCV_BUTTON_FRAME_NUM  (16) // �{�^���A�j���t���[����

enum{
  MCV_BUTTON_TYPE_MAN,
  MCV_BUTTON_TYPE_GIRL,
  MCV_BUTTON_TYPE_NONE,
};

enum{
  MCV_USERDISP_OFF, // ���[�U�[�\��OFF
  MCV_USERDISP_INIT,  // ���[�U�[�\��������
  MCV_USERDISP_ON,  // ���[�U�[�\��ON
  MCV_USERDISP_ONEX,  // ���[�U�[�\������ON
};

#define VRANTRANSFERMAN_NUM (32)  // VramTransferManager�^�X�N��


enum{
  MCV_USERD_BTTN_ANM_LEFT_PUSH = 0,
  MCV_USERD_BTTN_ANM_LEFT_RELEASE,
  MCV_USERD_BTTN_ANM_BACK_PUSH,
  MCV_USERD_BTTN_ANM_BACK_RELEASE,
  MCV_USERD_BTTN_ANM_RIGHT_PUSH,
  MCV_USERD_BTTN_ANM_RIGHT_RELEASE,
};

enum{
  MCV_USERD_BTTN_MODE_WAIT,
  MCV_USERD_BTTN_MODE_NML,
  MCV_USERD_BTTN_MODE_NOBACK,
};

#define MCV_USERD_BTTN_RESCONTID  ( 30 )  // ���\�[�XID
#define MCV_USERD_BTTN_BGPRI    (0)   // BG�D�揇��
#define MCV_USERD_BTTN_PRI      (128)   // BG�D�揇��
#define MCV_USERD_BTTN_Y      (172) // �{�^��Y�ʒu
#define MCV_USERD_BTTN_LEFT_X   (40)  // ��
#define MCV_USERD_BTTN_BACK_X   (128) // ���ǂ�
#define MCV_USERD_BTTN_RIGHT_X    (224) // ��
#define MCV_USERD_BTTN_FONT_X   ( -18 ) // ���ǂ�@�����ʒu
#define MCV_USERD_BTTN_FONT_Y   ( -8 )  // ���ǂ�@�����ʒu
#define MCV_USERD_BTTN_FONT_SIZX  ( 8 ) // ���ǂ�@�����`��͈�
#define MCV_USERD_BTTN_FONT_SIZY  ( 2 ) // ���ǂ�@�����`��͈�
#define MCV_USERD_BTTN_FONT_CGXOFS  ( 0 ) // CGX�I�t�Z�b�g
#define MCV_USERD_BTTN_FONT_COL   ( 0 ) // �J���[�p���b�g
#define MCV_USERD_BTTN_FONT_OAMPRI  ( 0 )
#define MCV_USERD_BTTN_LEFT_SIZX  ( 56 )  // ���T�C�Y
#define MCV_USERD_BTTN_BACK_SIZX  ( 96 )  // ���ǂ�T�C�Y
#define MCV_USERD_BTTN_RIGHT_SIZX ( 56 )  // ��������
#define MCV_USERD_BTTN_SIZY     ( 32 )  // �c������
#define MCV_USERD_BTTN_LEFT_HIT_X ( 8 ) // �������蔻��pX
#define MCV_USERD_BTTN_BACK_HIT_X ( 80 )  // �������蔻��pX
#define MCV_USERD_BTTN_RIGHT_HIT_X  ( 192 ) // �������蔻��pX
#define MCV_USERD_BTTN_RIGHT_HIT_Y  ( 160 ) // �������蔻��pY
#define MCV_USERD_BTTN_ANMPUSHOK  ( 3 ) // �������Ɣ��f����{�^���̃A�j��
#define MCV_USERD_BTTN_ANMMAX   ( 5 ) // �A�j�����t���[����
#define MCV_USERD_BTTN_ANMMAX_0ORG  ( MCV_USERD_BTTN_ANMMAX-1 ) // �A�j�����t���[����

// FONTOAMY���W�A�j���f�[�^
static const s8 c_MCV_USER_BTTN_FONT_YANM[ MCV_USERD_BTTN_ANMMAX ] = {
  MCV_USERD_BTTN_FONT_Y,
  MCV_USERD_BTTN_FONT_Y - 1,
  MCV_USERD_BTTN_FONT_Y - 2,
  MCV_USERD_BTTN_FONT_Y - 2,
  MCV_USERD_BTTN_FONT_Y - 1,
};

// �{�^���V�[�P���X�@PUSH�A�j��
static const u8 c_MCV_USER_BTTN_ANM_PUSH[ MCV_USERD_BTTN_NUM ] = {
  MCV_USERD_BTTN_ANM_LEFT_PUSH,
  MCV_USERD_BTTN_ANM_BACK_PUSH,
  MCV_USERD_BTTN_ANM_RIGHT_PUSH,
};

// �{�^���V�[�P���X�@RELEASE�A�j��
static const u8 c_MCV_USER_BTTN_ANM_RELEASE[ MCV_USERD_BTTN_NUM ] = {
  MCV_USERD_BTTN_ANM_LEFT_RELEASE,
  MCV_USERD_BTTN_ANM_BACK_RELEASE,
  MCV_USERD_BTTN_ANM_RIGHT_RELEASE,
};

static const GFL_UI_TP_HITTBL c_MCV_USER_BTTN_OAM_Hit[ ] = {
  { // ��
    MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
    MCV_USERD_BTTN_LEFT_HIT_X, MCV_USERD_BTTN_LEFT_HIT_X+MCV_USERD_BTTN_LEFT_SIZX,
  },
  { // ���ǂ�
    MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
    MCV_USERD_BTTN_BACK_HIT_X, MCV_USERD_BTTN_BACK_HIT_X+MCV_USERD_BTTN_BACK_SIZX,
  },
  { // ��
    MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
    MCV_USERD_BTTN_RIGHT_HIT_X, MCV_USERD_BTTN_RIGHT_HIT_X+MCV_USERD_BTTN_RIGHT_SIZX,
  },
  {GFL_UI_TP_HIT_END,0,0,0},     //�I���f�[�^
};
enum{
  MCV_USERD_BTTN_RET_NONE,  // ���̔������Ȃ�
  MCV_USERD_BTTN_RET_LEFT,  // ���������ꂽ
  MCV_USERD_BTTN_RET_BACK,  // ���ǂ邪�����ꂽ
  MCV_USERD_BTTN_RET_RIGHT, // �E�������ꂽ
};


static GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g

  GX_VRAM_OBJ_128_B,        // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_128_D,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_NONE,       // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,     // �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K,

};

#ifdef WFP2P_DEBUG  // �l�������ς��o��
//#define WFP2PM_MANY_OBJ
#endif


static void _debugChangeState(WIFIP2PMATCH_WORK* wk, int state, int line)
{
  OS_TPrintf("p2p: %d\n",line);
  wk->seq = state;
}

#if PM_DEBUG
#define   _CHANGESTATE(wk,state) _debugChangeState(wk,state,__LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGESTATE(wk,state)  wk->seq = state
#endif //GFL_NET_DEBUG

//static int _seqBackup;

//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
//volume�ύX
static void _changeBGMVol( WIFIP2PMATCH_WORK *wk, u8 endVol);
static void _initBGMVol( WIFIP2PMATCH_WORK* wk, int status);

static WIFI_STATUS* WifiFriendMatchStatusGet( u32 idx );
static u8 WifiDwc_getFriendStatus( int idx );

static BOOL WifiP2PMatch_CommWifiBattleStart( WIFIP2PMATCH_WORK* wk, int friendno );

/*** �֐��v���g�^�C�v ***/
static void VBlankFunc( GFL_TCB *tcb , void * work );
static void VramBankSet(void);
static void BgInit( HEAPID heapID );
static void InitMessageWork( WIFIP2PMATCH_WORK *wk );
static void FreeMessageWork( WIFIP2PMATCH_WORK *wk );
static void BgExit( WIFIP2PMATCH_WORK *wk );
static void BgGraphicSet( WIFIP2PMATCH_WORK * wk, ARCHANDLE* p_handle );
static void char_pltt_manager_init(void);
static void InitCellActor(WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle);
static void BmpWinInit(WIFIP2PMATCH_WORK *wk, GFL_PROC* proc);
static void MainMenuMsgInit(WIFIP2PMATCH_WORK *wk);
static void SetCursor_Pos( GFL_CLWK* act, int x, int y );
static void WindowSet(void);

// �}�b�`���O���[���Ǘ��p�֐�
static void MCRSYS_SetMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj );
static void MCRSYS_DelMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj );
static void MCRSYS_SetFriendObj( WIFIP2PMATCH_WORK* wk, u32 heapID );
static int MCRSYS_ContFiendInOut( WIFIP2PMATCH_WORK* wk );
static void MCRSYS_ContFriendStatus( WIFIP2PMATCH_WORK* wk, u32 heapID );
static MCR_MOVEOBJ* MCRSYS_GetMoveObjWork( WIFIP2PMATCH_WORK* wk, u32 friendNo );

static void WifiP2PMatchFriendListIconLoad( WIFIP2PMATCH_ICON* p_data, ARCHANDLE* p_handle, u32 heapID );
static void WifiP2PMatchFriendListIconRelease( WIFIP2PMATCH_ICON* p_data );
static void WifiP2PMatchFriendListIconWrite(  WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 icon_type, u32 col );
static void WifiP2PMatchFriendListStIconWrite( WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 status,u32 gamemode, u32 iconmode  );
static void WifiP2PMatchFriendListBmpIconWrite(  GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 icon_type, u32 col,int pal );
static void WifiP2PMatchFriendListBmpStIconWrite( GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 status,u32 gamemode,int pal );
static int WifiP2PMatchBglFrmIconPalGet( u32 frm );

static int WifiP2PMatchFriendListStart( WIFIP2PMATCH_WORK* wk,BOOL bWipe );

// �F�B�f�[�^�̋����\������
static void WifiP2PMatch_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID );
static void WifiP2PMatch_UserDispOff( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void WifiP2PMatch_UserDispOff_Target( WIFIP2PMATCH_WORK *wk, u32 target_friend, u32 heapID );

static void WifiP2PMatch_UserDispOn_MyAcces( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID );
static void WifiP2PMatch_UserDispOff_MyAcces( WIFIP2PMATCH_WORK *wk, u32 heapID );

// �F�B�f�[�^�r���[�A�[
static BOOL MCVSys_MoveCheck( const WIFIP2PMATCH_WORK *wk );
static void MCVSys_Init( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID );
static void MCVSys_Exit( WIFIP2PMATCH_WORK *wk );
static u32 MCVSys_Updata( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UpdataBttn( WIFIP2PMATCH_WORK *wk );
static BOOL MCVSys_UserDispEndCheck( WIFIP2PMATCH_WORK *wk );
static u32  MCVSys_UserDispGetFriend( const WIFIP2PMATCH_WORK* cp_wk );
static void MCVSys_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID );
static void MCVSys_UserDispOff( WIFIP2PMATCH_WORK *wk );
static void MCVSys_UserDispPageChange( WIFIP2PMATCH_WORK *wk, u32 oambttn_ret );
static void MCVSys_BttnSet( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 type );
static void MCVSys_BttnDel( WIFIP2PMATCH_WORK *wk, u32 friendNo );
static u32 MCVSys_BttnTypeGet( const WIFIP2PMATCH_WORK *wk, u32 friendNo );
static void MCVSys_ReWrite( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_BttnAllWriteReq( WIFIP2PMATCH_WORK *wk );

static void MCVSys_BttnCallBack( u32 bttnid, u32 event, void* p_work );
static void MCVSys_BttnAnmMan( WIFIP2PMATCH_WORK *wk );
static void MCVSys_GraphicSet( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID );
static void MCVSys_GraphicDel( WIFIP2PMATCH_WORK *wk );
static void MCVSys_GraphicScrnCGOfsChange( NNSG2dScreenData* p_scrn, u8 cgofs );
static void MCVSys_BackDraw( WIFIP2PMATCH_WORK *wk );
static void MCVSys_BttnDraw( WIFIP2PMATCH_WORK *wk );
static void MCVSys_UserDispDraw( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType00( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispFrontiorNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 factoryid, u32 friendno, u32 x, u32 y );
static void MCVSys_UserDispFrontiorTitleStrGet( WIFIP2PMATCH_WORK *wk, STRBUF* p_str, u32 factoryid, u32 friendno );
static void MCVSys_UserDispNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 num, u32 x, u32 y );
static void MCVSys_BttnWrite( WIFIP2PMATCH_WORK *wk, u8 cx, u8 cy, u8 type, u8 frame );
static u32 MCVSys_StatusMsgIdGet( u32 status,u32 gamemode, int* col );
static void MCVSys_BttnWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN* p_bmp, u32 friendNo, u32 frame, u32 area_id );
static void MCVSys_BttnStatusWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN** p_stbmp, u32 friendNo, u32 frame, u32 x, u32 y );
static void MCVSys_OamBttnOn( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOnNoBack( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOff( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnObjAnmStart( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no );
static BOOL MCVSys_OamBttnObjAnmMain( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no, u32 push_bttn, u32 event );
static void MCVSys_FriendNameSet( WIFIP2PMATCH_WORK* p_wk, int friendno );

static void BmpWinDelete( WIFIP2PMATCH_WORK *wk );
static void _systemMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno );
//static void WifiP2PMatchMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno , BOOL bSystem);
static void EndMessageWindowOff( WIFIP2PMATCH_WORK *wk );
static void WifiP2PMatchDataSendRecv( WIFIP2PMATCH_WORK *wk );
static void SequenceChange_MesWait( WIFIP2PMATCH_WORK *wk, int next );
static void _myStatusChange(WIFIP2PMATCH_WORK *wk, int status,int gamemode);
static void _myStatusChange_not_send(WIFIP2PMATCH_WORK *wk, int status,int gamemode);
static u32 _WifiMyStatusGet( WIFIP2PMATCH_WORK * p_wk, WIFI_STATUS* p_status );
static u32 _WifiMyGameModeGet( WIFIP2PMATCH_WORK * p_wk, WIFI_STATUS* p_status );
static BOOL _regulationCheck(WIFIP2PMATCH_WORK * wk);
static void MCRSYS_AllDelMoveObjWork( WIFIP2PMATCH_WORK* wk );


static void WifiP2P_SetLobbyBgm( void );



static void _errorDisp(WIFIP2PMATCH_WORK* wk);
static void _windelandSEcall(WIFIP2PMATCH_WORK *wk);

static void FriendRequestWaitOn( WIFIP2PMATCH_WORK* wk, BOOL msg_on );
static void FriendRequestWaitOff( WIFIP2PMATCH_WORK* wk );
static BOOL FriendRequestWaitFlagGet( const WIFIP2PMATCH_WORK* cp_wk );


// FuncTable����V�[�P���X�J�ڂŌĂ΂��֐�
static int WifiP2PMatch_MainInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _normalConnectYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _normalConnectWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _differMachineInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _differMachineNext( WIFIP2PMATCH_WORK *wk, int seq );
static int _differMachineOneMore( WIFIP2PMATCH_WORK *wk, int seq );
static int _firstYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _retryInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _retryYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _retryWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _retry( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_ReConnectingWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_ConnectingInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_Connecting( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FirstSaving( WIFIP2PMATCH_WORK *wk, int seq );
static int _firstConnectEndMsg( WIFIP2PMATCH_WORK *wk, int seq );
static int _firstConnectEndMsgWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FriendListInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FriendListInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FriendListMain( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnect( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectEndYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectEndWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTDisconnect(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_BattleDisconnect(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_Disconnect(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_BattleConnectInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BattleConnectWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BattleConnect( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_MainReturn( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectMenuInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentRestart( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSubSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectRelInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectRelYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectRelWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuLoop( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelYesNoVCT( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelWaitVCT( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuSend( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuCheck( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMyStatusWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _personalDataInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _personalDataWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _personalDataEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitExiting( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int _nextBattleYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _nextBattleWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _vchatNegoCheck( WIFIP2PMATCH_WORK *wk, int seq );
static int _vchatNegoWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _playerDirectInit( WIFIP2PMATCH_WORK *wk, int seq );
static int MessageEndReturnList( WIFIP2PMATCH_WORK *wk, int seq );
static int ReturnList( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectMenuInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuSendD( WIFIP2PMATCH_WORK *wk, int seq );
static int _DirectConnectWait( WIFIP2PMATCH_WORK *wk, int seq  );
static int _DirectConnectWait2( WIFIP2PMATCH_WORK *wk, int seq  );
static int WifiP2PMatch_Disconnect2(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_VCTDisconnectSend1(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_VCTDisconnectSend2(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_VCTDisconnectSend3(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_FriendListMain_MW( WIFIP2PMATCH_WORK *wk, int seq );

static int WifiP2PMatch_ReconectingWaitPre(WIFIP2PMATCH_WORK *wk, int seq);


static BOOL _myVChatStatusToggle(WIFIP2PMATCH_WORK *wk);
static BOOL _myVChatStatusToggleOrg(WIFIP2PMATCH_WORK *wk);
static BOOL _myVChatStatusOrgSet(WIFIP2PMATCH_WORK *wk);
static int _vchatToggleWait( WIFIP2PMATCH_WORK *wk, int seq );
static BOOL _tradeNumCheck(WIFIP2PMATCH_WORK * wk);
static int _childModeConnect( WIFIP2PMATCH_WORK *wk, int seq );


static int WifiP2PMatch_EndWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CheckAndEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CancelEnableWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CancelEnableWaitDP( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FirstSaving2( WIFIP2PMATCH_WORK *wk, int seq );
static int _callGameInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectMain( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTDisconnectSendEnd(WIFIP2PMATCH_WORK *wk, int seq);
static int _modeTVT1YesNo( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT1Wait( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT2YesNo( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT2Wait( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT3YesNo( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT3Wait( WIFIP2PMATCH_WORK* wk, int seq );
static int _playerDirectInit0Next( WIFIP2PMATCH_WORK *wk, int seq );


#include "wifi_p2pmatch_message.c"
#include "wifi_p2pmatch_direct.c"
#include "wifi_p2pmatch_machine.c"



static void* _getMyUserData(void* pWork)  //DWCUserData
{
  WIFIP2PMATCH_WORK *wk = (WIFIP2PMATCH_WORK*)pWork;
  return WifiList_GetMyUserInfo(GAMEDATA_GetWiFiList(wk->pGameData));
}

static void* _getFriendData(void* pWork)  //DWCFriendData
{
  WIFIP2PMATCH_WORK *wk = (WIFIP2PMATCH_WORK*)pWork;
  return WifiList_GetDwcDataPtr(GAMEDATA_GetWiFiList(wk->pGameData),0);
}

static void _deleteFriendList(int deletedIndex,int srcIndex, void* pWork)
{
  WIFIP2PMATCH_WORK *wk = (WIFIP2PMATCH_WORK*)pWork;

  WifiList_DataMarge(GAMEDATA_GetWiFiList(wk->pGameData), deletedIndex, srcIndex);
}


// ���R�[�h�R�[�i�[���C���V�[�P���X�p�֐��z���`
static int (*FuncTable[])(WIFIP2PMATCH_WORK *wk, int seq)={
  WifiP2PMatch_MainInit,  // WIFIP2PMATCH_MODE_INIT  = 0,
  _retryInit,        //WIFIP2PMATCH_RETRY_INIT
  _retryYesNo,        //WIFIP2PMATCH_RETRY_YESNO
  _retryWait,         //WIFIP2PMATCH_RETRY_WAIT
  _retry,             //WIFIP2PMATCH_RETRY
  WifiP2PMatch_ConnectingInit,        // WIFIP2PMATCH_CONNECTING_INIT,
  WifiP2PMatch_Connecting,            // WIFIP2PMATCH_CONNECTING,
  _firstConnectEndMsg,                 //WIFIP2PMATCH_FIRST_ENDMSG
  _firstConnectEndMsgWait,    //WIFIP2PMATCH_FIRST_ENDMSG_WAIT
  WifiP2PMatch_FriendListInit,        // WIFIP2PMATCH_FRIENDLIST_INIT,
  WifiP2PMatch_FriendListInit2, //WIFIP2PMATCH_FRIENDLIST_INIT2
  WifiP2PMatch_FriendListMain,            // WIFIP2PMATCH_MODE_FRIENDLIST
  _vchatToggleWait,                   // WIFIP2PMATCH_VCHATWIN_WAIT
  WifiP2PMatch_VCTConnectInit2,        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2
  WifiP2PMatch_VCTConnectInit,        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT
  WifiP2PMatch_VCTConnectWait,        // WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT
  WifiP2PMatch_VCTConnect,        // WIFIP2PMATCH_MODE_VCT_CONNECT
  WifiP2PMatch_VCTConnectEndYesNo,   // WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO
  WifiP2PMatch_VCTConnectEndWait,   // WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT
  WifiP2PMatch_VCTDisconnect,   // WIFIP2PMATCH_MODE_VCT_DISCONNECT
  WifiP2PMatch_BattleDisconnect,   //WIFIP2PMATCH_MODE_BATTLE_DISCONNECT
  WifiP2PMatch_Disconnect,  //WIFIP2PMATCH_MODE_DISCONNECT
  WifiP2PMatch_BattleConnectInit,     // WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT
  WifiP2PMatch_BattleConnectWait,     // WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT
  WifiP2PMatch_BattleConnect,        // WIFIP2PMATCH_MODE_BATTLE_CONNECT
  WifiP2PMatch_MainReturn,            // WIFIP2PMATCH_MODE_MAIN_MENU
  WifiP2PMatch_EndWait,         // WIFIP2PMATCH_MODE_END_WAIT,
  WifiP2PMatch_CheckAndEnd,          //  WIFIP2PMATCH_MODE_CHECK_AND_END
  _parentModeSelectMenuInit,  // WIFIP2PMATCH_MODE_SELECT_INIT
  _parentModeSelectMenuWait,  // WIFIP2PMATCH_MODE_SELECT_WAIT
  _parentModeSubSelectMenuWait, // WIFIP2PMATCH_MODE_SUBBATTLE_WAIT
  _parentModeSelectRelInit,  //WIFIP2PMATCH_MODE_SELECT_REL_INIT
  _parentModeSelectRelYesNo, //WIFIP2PMATCH_MODE_SELECT_REL_YESNO
  _parentModeSelectRelWait,  //WIFIP2PMATCH_MODE_SELECT_REL_WAIT
  _childModeMatchMenuInit,   //WIFIP2PMATCH_MODE_MATCH_INIT
  _childModeMatchMenuInit2,   //WIFIP2PMATCH_MODE_MATCH_INIT2
  _childModeMatchMenuWait,   //WIFIP2PMATCH_MODE_MATCH_WAIT
  _childModeMatchMenuLoop, //WIFIP2PMATCH_MODE_MATCH_LOOP
  WifiP2PMatch_BCancelYesNo, //WIFIP2PMATCH_MODE_BCANCEL_YESNO
  WifiP2PMatch_BCancelWait,    //WIFIP2PMATCH_MODE_BCANCEL_WAIT
  _parentModeCallMenuInit,//WIFIP2PMATCH_MODE_CALL_INIT
  _parentModeCallMenuYesNo,//WIFIP2PMATCH_MODE_CALL_YESNO
  _parentModeCallMenuSend, //WIFIP2PMATCH_MODE_CALL_SEND
  _parentModeCallMenuCheck, //WIFIP2PMATCH_MODE_CALL_CHECK
  _parentModeCallMyStatusWait, //WIFIP2PMATCH_MODE_MYSTATUS_WAIT   //51
  _parentModeCallMenuWait,//WIFIP2PMATCH_MODE_CALL_WAIT
  _personalDataInit,    //WIFIP2PMATCH_MODE_PERSONAL_INIT
  _personalDataWait,    //WIFIP2PMATCH_MODE_PERSONAL_WAIT
  _personalDataEnd,    //WIFIP2PMATCH_MODE_PERSONAL_END
  _exitYesNo,       //WIFIP2PMATCH_MODE_EXIT_YESNO
  _exitWait,        //WIFIP2PMATCH_MODE_EXIT_WAIT
  _exitExiting,        //WIFIP2PMATCH_MODE_EXITING
  _exitEnd,        //WIFIP2PMATCH_MODE_EXIT_END
  _nextBattleYesNo,  //WIFIP2PMATCH_NEXTBATTLE_YESNO
  _nextBattleWait,  //WIFIP2PMATCH_NEXTBATTLE_WAIT
  _vchatNegoCheck, //WIFIP2PMATCH_MODE_VCHAT_NEGO
  _vchatNegoWait, //WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT
  WifiP2PMatch_ReConnectingWait, //WIFIP2PMATCH_RECONECTING_WAIT
  WifiP2PMatch_BCancelYesNoVCT, //WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT
  WifiP2PMatch_BCancelWaitVCT,    //WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT
  _parentRestart,  //WIFIP2PMATCH_PARENT_RESTART
  WifiP2PMatch_FirstSaving, //WIFIP2PMATCH_FIRST_SAVING
  WifiP2PMatch_CancelEnableWait, //WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT
  WifiP2PMatch_FirstSaving2, //WIFIP2PMATCH_FIRST_SAVING2
  _callGameInit,  //WIFIP2PMATCH_MODE_CALLGAME_INIT
  _playerDirectInit, //WIFIP2PMATCH_MODE_PLAYERDIRECT_INIT
  _playerDirectInit1, //  WIFIP2PMATCH_PLAYERDIRECT_INIT1,
  _playerDirectInit2, //  WIFIP2PMATCH_PLAYERDIRECT_INIT2,
  _playerDirectInit3, //  WIFIP2PMATCH_PLAYERDIRECT_INIT3,
  _playerDirectInit5, //  WIFIP2PMATCH_PLAYERDIRECT_INIT5,
  _playerDirectInit6, //  WIFIP2PMATCH_PLAYERDIRECT_INIT6,
  _playerDirectInit7, //  WIFIP2PMATCH_PLAYERDIRECT_INIT7,
  _playerDirectReturn, //WIFIP2PMATCH_PLAYERDIRECT_RETURN
  _playerDirectWait, //  WIFIP2PMATCH_PLAYERDIRECT_WAIT,
  _playerDirectEnd, //  WIFIP2PMATCH_PLAYERDIRECT_END,    //80
  _playerDirectVCT, //  WIFIP2PMATCH_PLAYERDIRECT_VCT,
  _playerDirectTVT, //  WIFIP2PMATCH_PLAYERDIRECT_TVT,
  _playerDirectTrade, //  WIFIP2PMATCH_PLAYERDIRECT_TRADE,
  _playerDirectBattle1, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE1,
  _playerDirectSub1, // WIFIP2PMATCH_PLAYERDIRECT_SUB1
  _playerDirectSub2, // WIFIP2PMATCH_PLAYERDIRECT_SUB2
  _playerDirectSubStart, //WIFIP2PMATCH_PLAYERDIRECT_SUBSTART
  _playerDirectSubstartCall, //WIFIP2PMATCH_PLAYERDIRECT_SUBSTARTCALL
  _playerDirectEndCall, // WIFIP2PMATCH_PLAYERDIRECT_ENDCALL
  _playerDirectBattle2,//WIFIP2PMATCH_PLAYERDIRECT_BATTLE2
  _playerDirectBattleMode,  //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE,
  _playerDirectBattleMode2,  //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE2,
  _playerDirectBattleRule,  // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE,   //���[��
  _playerDirectBattleRule2,  // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE2,   //���[��
  _playerDirectBattleShooter,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER,  //�V���[�^�[
  _playerDirectBattleShooter2,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER2,  //�V���[�^�[
  _playerDirectBattleDecide,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE,  //����
  _playerDirectBattleWatch,  // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH,  //���[��������
  _playerDirectBattleWatch2,  // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH2,  //���[��������
  _playerDirectBattleGO,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO
  _playerDirectBattleGO1,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO1
  _playerDirectBattleGO2,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO2
  _playerDirectBattleGO3,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO3
  _playerDirectBattleGO4,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO4
  _playerDirectNoregParent, //WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT  /���M�����[�V�������Ⴄ
  _playerDirectBattleStart, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START
  _playerDirectBattleStart2, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START2
  _playerDirectBattleStart3, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START3
  _playerDirectBattleStart4, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START4
  _playerDirectBattleStart5, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START5
  _playerDirectBattleStart6, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START6
  _playerDirectBattleStart7, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START7

  _playerDirectBattleNoregSelectTemoti, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_TEMOTI
  _playerDirectBattleNoregSelectBBox,   //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_BBOX
  _playerDirectBattleNoregSelectWait,   //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_WAIT

  _playerDirectFailed, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED
  _playerDirectFailed2, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED2
  _playerDirectFailed3, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED3
  MessageEndReturnList, //WIFIP2PMATCH_MESSAGEEND_RETURNLIST
  ReturnList,         //WIFIP2PMATCH_RETURNLIST
  _playerMachineInit1,//WIFIP2PMATCH_PLAYERMACHINE_INIT1
  _playerMachineNoregParent, //WIFIP2PMATCH_PLAYERMACHINE_NOREG_PARENT
  _playerMachineBattleDecide, //WIFIP2PMATCH_PLAYERMACHINE_BATTLE_DECIDE
  _childModeConnect, //WIFIP2PMATCH_MODE_CHILD_CONNECT
  _playerDirectNoregParent1, //WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT1
  _playerDirectNoregParent2, //WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT2
  _playerDirectInit1Next, //WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT1
  _playerDirectBattleGO_12, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_12
  _playerDirectBattleGO_13, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_13
  _playerDirectBattleGO_14, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_14
  _playerDirectVCTChange1,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE1
  _playerDirectVCTChange2,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE2
  _playerDirectVCTChange3,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE3
  _playerDirectVCTChange4,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE4
  _playerDirectVCTChange5,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE5
  _playerDirectVCTChange6,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE6
  _parentModeSelectMenuInit2, //WIFIP2PMATCH_MODE_SELECT_INIT2
  _playerDirectEnd2, //WIFIP2PMATCH_PLAYERDIRECT_END2,
  _playerDirectEndCall2, //WIFIP2PMATCH_PLAYERDIRECT_ENDCALL2,
  _playerDirectVCTChangeE, //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE_E
  _parentModeCallMenuSendD, //WIFIP2PMATCH_MODE_CALL_CHECK_D
  _DirectConnectWait,//WIFIP2PMATCH_MODE_CONNECTWAIT,
  _DirectConnectWait2,//  WIFIP2PMATCH_MODE_CONNECTWAIT2,
  _playerDirectWaitSendCommand, // WIFIP2PMATCH_PLAYERDIRECT_WAIT_COMMAND
  _playerDirectBattleGo3KeyWait, // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO3_KEYWAIT
  _playerDirectEndKeyWait,  //WIFIP2PMATCH_PLAYERDIRECT_END_KEYWAIT
  _playerDirectEnd3, //WIFIP2PMATCH_PLAYERDIRECT_END3
  _playerDirectSub3, //WIFIP2PMATCH_PLAYERDIRECT_SUB3
  _playerDirectSubFailed, //WIFIP2PMATCH_PLAYERDIRECT_SUB_FAILED
  WifiP2PMatch_Disconnect2, //WIFIP2PMATCH_MODE_DISCONNECT2
  WifiP2PMatch_VCTDisconnectSend1,  //WIFIP2PMATCH_VCTEND_COMMSEND1
  WifiP2PMatch_VCTDisconnectSend2,  //WIFIP2PMATCH_VCTEND_COMMSEND2
  WifiP2PMatch_VCTDisconnectSend3, //WIFIP2PMATCH_VCTEND_COMMSEND3
  WifiP2PMatch_VCTConnectMain, //WIFIP2PMATCH_MODE_VCT_CONNECT_MAIN
  _playerDirectSub23, //  WIFIP2PMATCH_PLAYERDIRECT_SUB23
  _playerMachineTalkEnd, //  WIFIP2PMATCH_PLAYERMACHINE_TALKEND,
  _playerDirectBattleStart42, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START42
  _playerDirectEndNext,  ///WIFIP2PMATCH_PLAYERDIRECT_END_NEXT
  WifiP2PMatch_VCTDisconnectSendEnd,//WIFIP2PMATCH_VCTEND_COMMSEND2_ENDCHK
  _playerDirectCancelEnd, //WIFIP2PMATCH_PLAYERDIRECT_CANCELEND
  _playerDirectCancelEndNext, //WIFIP2PMATCH_PLAYERDIRECT_CANCELEND_NEXT
  _playerDirectSub45, //WIFIP2PMATCH_PLAYERDIRECT_SUB45
  WifiP2PMatch_FriendListMain_MW, //WIFIP2PMATCH_MODE_FRIENDLIST_MW
  WifiP2PMatch_ReconectingWaitPre, //WIFIP2PMATCH_RECONECTING_WAIT_PRE
  _modeTVT1YesNo,    //WIFIP2PMATCH_MODE_TVTMESSAGE1_YESNO
  _modeTVT1Wait,  //WIFIP2PMATCH_MODE_TVTMESSAGE1_WAIT
  _modeTVT2YesNo,    //  WIFIP2PMATCH_MODE_TVTMESSAGE2_YESNO
  _modeTVT2Wait,  // WIFIP2PMATCH_MODE_TVTMESSAGE2_WAIT
  _modeTVT3YesNo,    //  WIFIP2PMATCH_MODE_TVTMESSAGE3_YESNO
  _modeTVT3Wait,  // WIFIP2PMATCH_MODE_TVTMESSAGE3_WAIT
  _modeTVT4YesNo,    //  WIFIP2PMATCH_MODE_TVTMESSAGE4_YESNO
  _modeTVT4Wait,  // WIFIP2PMATCH_MODE_TVTMESSAGE4_WAIT
  _playerDirectEndChild, //WIFIP2PMATCH_PLAYERDIRECT_END_CHILD
  _playerDirectEndChildNext,//WIFIP2PMATCH_PLAYERDIRECT_END_CHILD_NEXT
  _playerDirectInit2Next,//WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT2
  _playerDirectInit3Next,//WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT3
  _playerDirectInit0Next, //WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT0

};



#define _MAXNUM   (2)         // �ő�ڑ��l��
#define _MAXSIZE  (80)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��


static GFLNetInitializeStruct aGFLNetInit = {
  NULL,  // ��M�֐��e�[�u��
  0, // ��M�e�[�u���v�f��
  NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  NULL,  // �r�[�R���f�[�^�擾�֐�
  NULL,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
  NULL,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
  NULL,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  _deleteFriendList,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  _getFriendData,   ///< DWC�`���̗F�B���X�g
  _getMyUserData,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  GFL_NET_DWC_HEAPSIZE,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
  SYACHI_NETWORK_GGID,  //ggid
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_NETWORK,  //IRC�p��create�����HEAPID
  GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
  _MAXNUM,     // �ő�ڑ��l��
  _MAXSIZE,  //�ő呗�M�o�C�g��
  _BCON_GET_NUM,    // �ő�r�[�R�����W��
  TRUE,     // CRC�v�Z
  FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIFI,  //�ʐM���
  TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_WIFICLUB,  //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
  0,//MP�e�ő�T�C�Y 512�܂�
  0,//dummy
};



static void _GFL_NET_InitAndStruct(WIFIP2PMATCH_WORK *wk,BOOL bInit)
{

  if( OS_IsRunOnTwl() ){//DSI�Ȃ�VCT�p�̃��������傫���Ƃ���ׁA�̈�𑽂����Ȃ��Ƃ����Ȃ�
    aGFLNetInit.heapSize = GFL_NET_DWCLOBBY_HEAPSIZE;
  }
  else{
    aGFLNetInit.heapSize = GFL_NET_DWC_HEAPSIZE;
  }
  if(bInit){
    GFL_NET_Init(&aGFLNetInit, NULL, wk);
  }
  else{
    GFL_NET_ChangeInitStruct(&aGFLNetInit);
  }
}

static const u8 ViewButtonFrame_y[ 4 ] = {
  8, 7, 5, 7
  };



// �����X�e�[�^�X���擾����
static u32 _WifiMyStatusGet( WIFIP2PMATCH_WORK * p_wk, WIFI_STATUS* p_status )
{
  return WIFI_STATUS_GetWifiStatus(p_status);
}
// �����Q�[�����[�h���擾����
static u32 _WifiMyGameModeGet( WIFIP2PMATCH_WORK * p_wk, WIFI_STATUS* p_status )
{
  return WIFI_STATUS_GetGameMode(p_status);
}

static BOOL _modeIsBattleStatus(int gamemode)
{
  switch(gamemode){
  case WIFI_GAME_BATTLE_SINGLE_ALL:   // �V���O��Lv50�ΐ��W��
  case WIFI_GAME_BATTLE_SINGLE_FLAT:      // �V���O���o�g��
  case WIFI_GAME_BATTLE_DOUBLE_ALL:      // �_�u��
  case WIFI_GAME_BATTLE_DOUBLE_FLAT:      // �_�u��
  case WIFI_GAME_BATTLE_TRIPLE_ALL:      // �g���v��
  case WIFI_GAME_BATTLE_TRIPLE_FLAT:      // �g���v��
  case WIFI_GAME_BATTLE_ROTATION_ALL:      // ���[�e�[�V����
  case WIFI_GAME_BATTLE_ROTATION_FLAT:      // ���[�e�[�V����
    return TRUE;
  }
  return FALSE;
}

//�ڑ��\���[�h���ǂ���
static BOOL _modeIsConnectStatus(int gamemode)
{
  if((gamemode < WIFI_GAME_UNKNOWN)  && (gamemode > WIFI_GAME_UNIONMATCH)){
    return TRUE;
  }
  return FALSE;
}



// �X�e�[�g��BATTLEWAIT���ǂ���
static BOOL _modeBattleWait(int status,int gamemode)
{
  if(_modeIsBattleStatus(gamemode)){
    if(status == WIFI_STATUS_RECRUIT){
      return TRUE;
    }
  }
  return FALSE;
}

// �X�e�[�g��WAIT���ǂ���
static BOOL
_modeWait(int status)
{
  if(status==WIFI_STATUS_RECRUIT){
    return TRUE;
  }
  return FALSE;
}

// �X�e�[�g��BATTLE���ǂ���
static BOOL _modeBattle(int status,int gamemode)
{
  if(_modeIsBattleStatus(gamemode)){
    if(status == WIFI_STATUS_PLAYING){
      return TRUE;
    }
  }
  return FALSE;
}

static BOOL _is2pokeMode(int status,int gamemode)
{
  if(gamemode > WIFI_GAME_LOGIN_WAIT){
    if(gamemode < WIFI_GAME_UNKNOWN){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����̐��������ɓK���Ă��邩�ǂ���
 * @param status
 * @return  none
 */
//--------------------------------------------------------------------------------------------

static BOOL _tradeNumCheck(WIFIP2PMATCH_WORK * wk)
{
  BOX_MANAGER * pManager = GAMEDATA_GetBoxManager(wk->pGameData);

  /*
  if(GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT){
    return FALSE;
  }
   */
  if(PokeParty_GetPokeCountNotEgg(GAMEDATA_GetMyPokemon(wk->pGameData)) < 2 ){
//  if((PokeParty_GetPokeCount(GAMEDATA_GetMyPokemon(wk->pGameData))==1) && ( BOXDAT_GetPokeExistCountTotal(pManager)==0 )){
    return FALSE;
  }
  return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * ���M�����[�V�����Ńo�g���ł��邩�ǂ���
 * @param status
 * @return  none
 */
//--------------------------------------------------------------------------------------------

static BOOL _regulationCheck(WIFIP2PMATCH_WORK * wk)
{
  u32 fail_bit;

  if(POKE_REG_OK!=_CheckRegulation_Temoti(wk->pRegulation, wk->pGameData, &fail_bit )){
    if(POKE_REG_OK!=_CheckRegulation_BBox(wk->pRegulation, wk->bb_party, &fail_bit )){
      // �I�Ԏ����ł��Ȃ�
      return FALSE;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�ɂ��킹�ĒʐM�X�e�[�g���ύX����
 * @param status
 * @return  none
 */
//--------------------------------------------------------------------------------------------

static void _commStateChange(WIFIP2PMATCH_WORK * wk,int status,int gamemode)
{
  if(status == WIFI_STATUS_WAIT){
    GFL_NET_SetWifiBothNet(FALSE);
    _GFL_NET_InitAndStruct(wk,FALSE);
  }
  else if(_modeBattle(status, gamemode)){
    GFL_NET_SetWifiBothNet(TRUE); // wifi�̒ʐM��񓯊����瓯����
  }

  switch(status){
  case WIFI_STATUS_RECRUIT:    // ��W��  = �N�ł��q�������\
  case WIFI_STATUS_PLAY_AND_RECRUIT:      // ���̐l�Ɛڑ����ł���ɕ�W��
    GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),TRUE);  //�ڑ�����
    break;
  case WIFI_STATUS_CALL:       // ���̐l�ɐڑ����悤�Ƃ��Ă���
  case WIFI_STATUS_WAIT:   // �ҋ@��
  case WIFI_STATUS_NONE:   // ��������	NONE�̂Ƃ��͏o�������܂���
  case WIFI_STATUS_PLAYING:      // ���̐l�Ɛڑ���
  case WIFI_STATUS_UNKNOWN:   // �V���ɍ�����炱�̔ԍ��ȏ�ɂȂ�
  default:
    GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),TRUE);  //�ڑ��s��
    break;
  }
  OS_TPrintf("status %d gamemode %d\n",status,gamemode);


}

//============================================================================================
//  �v���Z�X�֐�
//============================================================================================

static void _graphicInit(WIFIP2PMATCH_WORK * wk)
{
  ARCHANDLE* p_handle;

  // sys_VBlankFuncChange( NULL, NULL );  // VBlank�Z�b�g
  //    sys_HBlankIntrStop(); //HBlank���荞�ݒ�~
  // Vblank���Ԓ���BG�]���I��
  if(wk->vblankFunc){
    GFL_TCB_DeleteTask( wk->vblankFunc );
  }

  GFL_DISP_GX_SetVisibleControlDirect(0);   //�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
#if WB_FIX
  sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );
#endif

  p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, HEAPID_WIFIP2PMATCH );


  // ������}�l�[�W���[����
  wk->WordSet    = WORDSET_Create( HEAPID_WIFIP2PMATCH );
  wk->MsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_lobby_dat, HEAPID_WIFIP2PMATCH );
  wk->SysMsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_system_dat, HEAPID_WIFIP2PMATCH );
  wk->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_WIFIP2PMATCH );
  wk->font_handle_num = GFL_FONT_Create( ARCID_FONT, NARC_font_num_gftr,  GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIP2PMATCH );


  GFL_DISP_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

  // VRAM �o���N�ݒ�
  VramBankSet();

  // BGL���W�X�^�ݒ�
  BgInit(HEAPID_WIFIP2PMATCH );
  GFL_BMPWIN_Init( HEAPID_WIFIP2PMATCH );



  //BG�O���t�B�b�N�Z�b�g
  BgGraphicSet( wk, p_handle );

  // VBlank�֐��Z�b�g
  //    sys_VBlankFuncChange( VBlankFunc, wk );


  wk->vblankFunc = GFUser_VIntr_CreateTCB( VBlankFunc , wk , 1 );


  // OBJ�L�����A�p���b�g�}�l�[�W���[������
  char_pltt_manager_init();

  // CellActor�V�X�e��������
  InitCellActor(wk, p_handle);

  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON ); //���C�����OBJ�ʂn�m

  GFL_ARC_CloseDataHandle( p_handle );
}

//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�ɂ�����镔���̏I������
 * @param wk  WIFIP2PMATCH_WORK
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void _graphicEnd(WIFIP2PMATCH_WORK* wk)
{
  int i;

  if(wk->vblankFunc){
    GFL_TCB_DeleteTask( wk->vblankFunc );
  }
  _TrainerOAMFree( wk );
  _TouchResExit(wk);
  _UnderScreenReload( wk );


  // �}�b�`���O���[���j��
  if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
    MCRSYS_AllDelMoveObjWork(wk);
  }
  // �r���[�A�[�j��
  if( MCVSys_MoveCheck( wk ) == TRUE ){
    MCVSys_Exit( wk );
  }

  // �A�C�R���j��
  WifiP2PMatchFriendListIconRelease( &wk->icon );

  // �Z���A�N�^�[�Z�b�g�j��
  GFL_CLACT_USERREND_Delete(wk->renddata);
  GFL_CLACT_UNIT_Delete(wk->clactSet);

  //OAM�����_���[�j��
  GFL_CLACT_SYS_Delete();//

  // BMP�E�B���h�E�J��
  BmpWinDelete( wk );
  BgExit(wk);

  GFL_BMPWIN_Exit();
  // BGL�폜
  GFL_BG_Exit( );


  // ���b�Z�[�W�}�l�[�W���[�E���[�h�Z�b�g�}�l�[�W���[���
  GFL_MSG_Delete( wk->SysMsgManager );
  GFL_MSG_Delete( wk->MsgManager );
  WORDSET_Delete( wk->WordSet );

  GFL_FONT_Delete(wk->font_handle_num);
  GFL_FONT_Delete(wk->fontHandle);
}


//--------------------------------------------------------------------------------------------
/**
 * VBlank�֐�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB *tcb , void * work )
{
  WIFIP2PMATCH_WORK* wk = work;
  GFL_CLACT_SYS_VBlankFunc();

}

//--------------------------------------------------------------------------------------------
/**
 * VRAM�ݒ�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VramBankSet(void)
{
  GFL_DISP_SetBank( &_defVBTbl );
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
static void BgInit(HEAPID heapID )
{
  int i;

  GFL_BG_Init(heapID);
  // BG SYSTEM
  {
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  // �w�i��
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000,
      0x10000,
      GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME0_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );


  }

  // ���C�����1
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000,
      0x8000,
      GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    //    GFL_BG_FillCharacter( GFL_BG_FRAME1_M, 0x00, 1, 0 );
    GFL_BG_ClearFrame( GFL_BG_FRAME1_M);
    GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );
  }

  // ���X�g�Ȃ�
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000,
      0x8000,
      GX_BG_EXTPLTT_23,
      2, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_FillCharacter( GFL_BG_FRAME2_M, 0x00, 1, 0 );
    GFL_BG_ClearFrame( GFL_BG_FRAME2_M);
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
  }

#if 1
  // ���낢��Ɏg��
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x18000,
      0x8000,
      GX_BG_EXTPLTT_23,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME3_M);
    GFL_BG_LoadScreenReq( GFL_BG_FRAME3_M );
  }
#endif

  // �T�u��ʃe�L�X�g��
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000,
      0x8000,
      GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME0_S );
  }
  // ���[�U�[��ԕ\����
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000,
      0x8000,
      GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME1_S );
  }
  // �{�^��or���[�U�[�f�[�^���̔w�i
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000,
      0x8000,
      GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME2_S );
  }
  // ���[�U�[�f�[�^�e�L�X�g
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000,
      0x8000,
      GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME3_S );
  }

  for(i=0;i <GFL_BG_FRAME3_S;i++){
    GFL_BG_ClearCharacter( i );
  }

  GFL_BG_SetPriority(GFL_BG_FRAME0_M , 3);  //�͂�����
  GFL_BG_SetPriority(GFL_BG_FRAME1_M , 1);   // yesno win�@���[�U�[�f�[�^�̔w�i
  GFL_BG_SetPriority(GFL_BG_FRAME3_M , 1);  // ���b�Z�[�W
  GFL_BG_SetPriority(GFL_BG_FRAME2_M , 0);   // menu���X�g

  G2_BlendNone();
  G2S_BlendNone();
}

static void WindowSet(void)
{
  GX_SetVisibleWnd( GX_WNDMASK_NONE );
  GXS_SetVisibleWnd( GX_WNDMASK_NONE );
  /*
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
  G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_NONE, 1 );
  G2_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, 1 );
  G2_SetWnd0Position( 0, 255, 0, 192 );

  GXS_SetVisibleWnd( GX_WNDMASK_W0 );
  G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_NONE, 1 );
  G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, 1 );
  G2S_SetWnd0Position( 0, 255, 0, 192 );
   */
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������������I�u�W�F�N�g���[�N��ݒ肷��
 *
 *  @param  wk    ���[�N
 *  @param  p_obj �I�u�W�F�N�g���[�N
 */
//-----------------------------------------------------------------------------
static void MCRSYS_SetMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj )
{
  int i;

  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
    if( wk->p_matchMoveObj[i] == NULL ){
      wk->p_matchMoveObj[i] = p_obj;
      return ;
    }
  }
  GF_ASSERT( 0 );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ������OBJ�������Ă��郏�[�N��NULL�ɂ���
 *
 *  @param  wk    ���[�N
 *  @param  p_obj NULL�����Ăق������[�N�ɓ����Ă���|�C���^
 */
//-----------------------------------------------------------------------------
static void MCRSYS_DelMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj )
{
  int i;

  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
    if( wk->p_matchMoveObj[i] == p_obj ){
      wk->p_matchMoveObj[i] = NULL;
      return ;
    }
  }
  GF_ASSERT( 0 );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �����S���������{���[�N����
 *
 *  @param  wk    ���[�N
 *  @param  p_obj NULL�����Ăق������[�N�ɓ����Ă���|�C���^
 */
//-----------------------------------------------------------------------------
static void MCRSYS_AllDelMoveObjWork( WIFIP2PMATCH_WORK* wk )
{
  int i;

  WIFI_MCR_Dest( &wk->matchroom );
  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ )
  {
    wk->p_matchMoveObj[i] = NULL;
  }
  MI_CpuClear8(wk->matchStatusBackup, sizeof(wk->matchStatusBackup));
}

//----------------------------------------------------------------------------
/**
 *  @brief  friendNO�̃I�u�W�F�N�g���[�N���擾����
 *
 *  @param  wk      ���[�N
 *  @param  friendNo  friendNO
 *
 *  @return �I�u�W�F�N�g���[�N
 */
//-----------------------------------------------------------------------------
static MCR_MOVEOBJ* MCRSYS_GetMoveObjWork( WIFIP2PMATCH_WORK* wk, u32 friendNo )
{
  int i;

  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
    if( wk->p_matchMoveObj[i] ){
      if( WIFI_MCR_GetFriendNo( wk->p_matchMoveObj[i] ) == friendNo ){
        return wk->p_matchMoveObj[i];
      }
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �A�C�R���f�[�^�Ǎ���
 *
 *  @param  p_data    �f�[�^�i�[��
 *  @param  p_handle  �n���h��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListIconLoad( WIFIP2PMATCH_ICON* p_data, ARCHANDLE* p_handle, u32 heapID )
{
  // �p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCLR,
                                    PALTYPE_MAIN_BG, PLAYER_DISP_ICON_PLTTOFS*32,
                                    PLAYER_DISP_ICON_PLTTNUM*32, heapID );

  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCLR,
                                    PALTYPE_SUB_BG, PLAYER_DISP_ICON_PLTTOFS_SUB*32,
                                    PLAYER_DISP_ICON_PLTTNUM*32, heapID );

  // �L�����N�^�f�[�^�]�� �擪����Ƃ�Ȃ��Ƃ����Ȃ�

  p_data->wf_match_all_iconcgx1m =
    GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle,
                                                 NARC_wifip2pmatch_wf_match_all_icon_NCGR,
                                                 GFL_BG_FRAME1_M, 0, 0, heapID);

  p_data->wf_match_all_iconcgx2s =
    GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle,
                                                 NARC_wifip2pmatch_wf_match_all_icon_NCGR,
                                                 GFL_BG_FRAME2_S, 0, 0, heapID);

  // ����ɃL�����N�^�f�[�^��ۑ����Ă���
  if( p_data->p_charbuff == NULL ){
    p_data->p_charbuff = GFL_ARCHDL_UTIL_LoadOBJCharacter( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCGR,
                                                           FALSE, &p_data->p_char, heapID);
  }

  // �X�N���[���f�[�^�Ǎ���
  // ���g���C���ɃX�N���[���f�[�^������ꍇ�͓ǂݍ��܂Ȃ�
  if( p_data->p_buff == NULL ){
    p_data->p_buff = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NSCR,
                                                 FALSE, &p_data->p_scrn, heapID);
  }

}

//----------------------------------------------------------------------------
/**
 *  @brief  Icon�f�[�^�͂�
 *
 *  @param  p_data  ���[�N
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListIconRelease( WIFIP2PMATCH_ICON* p_data )
{

  if( p_data->p_buff != NULL ){
    GFL_HEAP_FreeMemory( p_data->p_buff );
    p_data->p_buff = NULL;
  }
  if( p_data->p_charbuff != NULL ){
    GFL_HEAP_FreeMemory( p_data->p_charbuff );
    p_data->p_charbuff = NULL;
  }

  //GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_M, PLAYER_DISP_ICON_CGX, p_data->wf_match_all_iconcgx1m);
  //GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S, PLAYER_DISP_ICON_CGX,  p_data->wf_match_all_iconcgx2s);

  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_M,
                           GFL_ARCUTIL_TRANSINFO_GetPos(p_data->wf_match_all_iconcgx1m),
                           GFL_ARCUTIL_TRANSINFO_GetSize(p_data->wf_match_all_iconcgx1m));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,
                           GFL_ARCUTIL_TRANSINFO_GetPos(p_data->wf_match_all_iconcgx2s),
                           GFL_ARCUTIL_TRANSINFO_GetSize(p_data->wf_match_all_iconcgx2s));



}

//----------------------------------------------------------------------------
/**
 *  @brief  �A�C�R������������
 *
 *  @param  p_data    �f�[�^
 *  @param  frm     �t���[��
 *  @param  cx      �L�����N�^�����W
 *  @param  cy      �L�����N�^�����W
 *  @param  icon_type �A�C�R���^�C�v
 *  @param  col     0=�D 1=��
 */
//-----------------------------------------------------------------------------


static void WifiP2PMatchFriendListIconWrite(WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 icon_type, u32 col )
{
  int pal;
  // ��������
  int i,j;
  int datax,datay;
  int width;
  u16* p_scrndata = (u16*)p_data->p_scrn->rawData;

  pal = WifiP2PMatchBglFrmIconPalGet( frm );
  datax = PLAYER_DISP_ICON_SCRN_X*(icon_type % PLAYER_DISP_ICON_IDX_NUM);
  datay = 2 * (icon_type / PLAYER_DISP_ICON_IDX_NUM);
  width = p_data->p_scrn->screenWidth/8;

  for(i = 0;i < PLAYER_DISP_ICON_SCRN_Y;i++){
    for(j = 0;j < PLAYER_DISP_ICON_SCRN_X;j++){
      GFL_BG_ScrSetDirect( frm, j+cx,i+cy,  ((p_scrndata[ width*(datay+i) + datax+j ] + ( pal << 12 ))));
    }
  }
  // �]��
  GFL_BG_LoadScreenV_Req( frm );
}



static u8 _gamemode2icon(  u32 gamemode,u32 status )
{
  u8 scrn_idx = PLAYER_DISP_ICON_IDX_NONE;

  switch(gamemode){
  case WIFI_GAME_VCT:
    if(status == WIFI_STATUS_RECRUIT){
      scrn_idx = PLAYER_DISP_ICON_IDX_VCT;
    }
    else{
      scrn_idx = PLAYER_DISP_ICON_IDX_VCT_ACT;
    }
    break;
  case WIFI_GAME_TVT:      // TVT��
    if(status == WIFI_STATUS_RECRUIT){
      scrn_idx = PLAYER_DISP_ICON_IDX_TVT;
    }
    else{
      scrn_idx = PLAYER_DISP_ICON_IDX_TVT_ACT;
    }
    break;
  case WIFI_GAME_TRADE:
    if(status == WIFI_STATUS_RECRUIT){
      scrn_idx = PLAYER_DISP_ICON_IDX_CHANGE;
    }
    else{
      scrn_idx = PLAYER_DISP_ICON_IDX_CHANGE_ACT;
    }
    break;
  case WIFI_GAME_LOGIN_WAIT:
    scrn_idx = PLAYER_DISP_ICON_IDX_NORMAL;
    break;
  case WIFI_GAME_NONE:
    scrn_idx = PLAYER_DISP_ICON_IDX_NONE;
    break;
  case WIFI_GAME_UNIONMATCH:
    scrn_idx = PLAYER_DISP_ICON_IDX_NORMAL_ACT;
    break;
  default:
    if(_modeIsBattleStatus(gamemode)){
      if(status == WIFI_STATUS_RECRUIT){
        scrn_idx = PLAYER_DISP_ICON_IDX_FIGHT;
      }
      else{
        scrn_idx = PLAYER_DISP_ICON_IDX_FIGHT_ACT;
      }
    }
    else if(gamemode >= WIFI_GAME_UNKNOWN){    // �V���ɍ�����炱�̔ԍ��ȏ�ɂȂ�
      scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
    }
    break;
  }
  return scrn_idx;
}


//----------------------------------------------------------------------------
/**
 *  @brief  Icon��������
 *
 *  @param  p_data    �f�[�^
 *  @param  frm     �t���[��
 *  @param  cx      �L�����N�^�����W
 *  @param  cy      �L�����N�^�����W
 *  @param  status    ���
 *  @param  vctIcon   �{�C�X�`���b�gONOFF�t���O
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListStIconWrite(  WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 status,u32 gamemode, u32 iconmode )
{
  u8 col=0;
  u8 scrn_idx=0;

  scrn_idx = _gamemode2icon(gamemode, status);
  //  if(status == WIFI_STATUS_RECRUIT){
  //   col = 1;
  //  }

  // ��������
  WifiP2PMatchFriendListIconWrite( p_data, frm, cx, cy, scrn_idx + iconmode, col );
}



//----------------------------------------------------------------------------
/**
 *  @brief  �r�b�g�}�b�v�ɃA�C�R���f�[�^����������
 *
 *  @param  p_bmp   �r�b�g�}�b�v
 *  @param  p_data    �A�C�R���f�[�^
 *  @param  x     �������݂����W
 *  @param  y     �������݂����W
 *  @param  status    ���
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListBmpStIconWrite( GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 status,u32 gamemode,int pal )
{
  u8 col=0;
  u8 scrn_idx=0;

  //  if(status == WIFI_STATUS_RECRUIT){
  //  col = 1;
  //  }
  scrn_idx = _gamemode2icon(gamemode, status);

  WifiP2PMatchFriendListBmpIconWrite( p_bmp, p_data, x, y, scrn_idx, col,pal );
}

//----------------------------------------------------------------------------
/**
 *  @brief  frm�̃A�C�R���p���b�g�i���o�[��Ԃ�
 *
 *  @param  frm   BGL�@FRAME
 *
 *  @return �p���b�gNO
 */
//-----------------------------------------------------------------------------
static int WifiP2PMatchBglFrmIconPalGet( u32 frm )
{
  if( frm < GFL_BG_FRAME0_S ){
    return PLAYER_DISP_ICON_PLTTOFS;
  }
  return PLAYER_DISP_ICON_PLTTOFS_SUB;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�����h���X�g�J�n�錾
 */
//-----------------------------------------------------------------------------
static int WifiP2PMatchFriendListStart( WIFIP2PMATCH_WORK *wk,BOOL bWipe )
{
  if(bWipe){
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );
  }

  // ������BGM�ݒ�
  WIFI_STATUS_SetVChatStatus(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetUseVChat(wk->pMatch, wk->pParentWork->vchatMain);
  WifiP2P_SetLobbyBgm();

  return WIFIP2PMATCH_FRIENDLIST_INIT;
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
static void BgExit( WIFIP2PMATCH_WORK *wk )
{
  int i;

  // for(i=0;i<GFL_BG_FRAME3_S;i++){
  //   GFL_BG_FreeBGControl( i );
  // }
  return;
  if(wk->talkwin_m2){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(wk->talkwin_m2),
                             GFL_ARCUTIL_TRANSINFO_GetSize(wk->talkwin_m2));
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2),
                             GFL_ARCUTIL_TRANSINFO_GetSize(wk->menuwin_m2));
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m1),
                             GFL_ARCUTIL_TRANSINFO_GetSize(wk->menuwin_m1));
    wk->talkwin_m2=0;
    wk->menuwin_m2=0;
    wk->menuwin_m1=0;
  }

}


//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param wk    �|�P�������X�g��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------


static void BgGraphicSet( WIFIP2PMATCH_WORK * wk, ARCHANDLE* p_handle )
{
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 |
                                 GX_PLANEMASK_BG3, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_ON );

  // �A�C�R���f�[�^�Ǎ���
  WifiP2PMatchFriendListIconLoad(  &wk->icon, p_handle, HEAPID_WIFIP2PMATCH );


  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , MCV_SYSFONT_PAL*0x20, 16*2, HEAPID_WIFIP2PMATCH );

  {
    wk->talkwin_m2 = BmpWinFrame_GraphicSetAreaMan(
      GFL_BG_FRAME2_M,  COMM_MESFRAME_PAL,  MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH );
    wk->menuwin_m2 = BmpWinFrame_GraphicSetAreaMan(
      GFL_BG_FRAME2_M,  MENU_WIN_PAL, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH );
    wk->menuwin_m1 = BmpWinFrame_GraphicSetAreaMan(
      GFL_BG_FRAME1_M,  MENU_WIN_PAL, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH );
  }

}


//** CharManager PlttManager�p **//
#define WIFIP2PMATCH_CHAR_CONT_NUM        (20)
#define WIFIP2PMATCH_CHAR_VRAMTRANS_MAIN_SIZE   (128*1024)
#define WIFIP2PMATCH_CHAR_VRAMTRANS_SUB_SIZE    (16*1024)
#define WIFIP2PMATCH_PLTT_CONT_NUM        (20)

//-------------------------------------
//
//  �L�����N�^�}�l�[�W���[
//  �p���b�g�}�l�[�W���[�̏�����
//
//=====================================
static void char_pltt_manager_init(void)
{
#if 0
  // �L�����N�^�}�l�[�W���[������
  {
    CHAR_MANAGER_MAKE cm = {
      WIFIP2PMATCH_CHAR_CONT_NUM,
      WIFIP2PMATCH_CHAR_VRAMTRANS_MAIN_SIZE,
      WIFIP2PMATCH_CHAR_VRAMTRANS_SUB_SIZE,
      HEAPID_WIFIP2PMATCH
      };
    InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_32K );
  }
  // �p���b�g�}�l�[�W���[������
  InitPlttManager(WIFIP2PMATCH_PLTT_CONT_NUM, HEAPID_WIFIP2PMATCH);

  // �ǂݍ��݊J�n�ʒu��������
  CharLoadStartAll();
  PlttLoadStartAll();
  //�ʐM�A�C�R���p�ɃL�������p���b�g����
  CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
  CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
#endif
}


#define _CLSYS_RESOUCE_MAX		(100)

//--------------------------------------------------------------
///	�Z���A�N�^�[�@�������f�[�^
//--------------------------------------------------------------
static const GFL_CLSYS_INIT fldmapdata_CLSYS_Init =
{
  0, 0,
  0, 512,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL, //�ʐM�A�C�R������
  0,
  _CLSYS_RESOUCE_MAX,
  _CLSYS_RESOUCE_MAX,
  _CLSYS_RESOUCE_MAX,
  _CLSYS_RESOUCE_MAX,
  16, 16,
};

//------------------------------------------------------------------
/**
 * �Z���A�N�^�[������
 *
 * @param   wk    ���[�_�[�\���̂̃|�C���^
 *
 * @retval  none
 */
//------------------------------------------------------------------

//-------------------------------------
///	�Ǝ������_���[�쐬�p
/// �T�[�t�F�[�X�f�[�^�\����
//=====================================
const static GFL_REND_SURFACE_INIT sini[] ={
  {
    0,0,			// �T�[�t�F�[�X���゘���W			// �T�[�t�F�[�X���゙���W
    256,				// �T�[�t�F�[�X��
    192,				// �T�[�t�F�[�X����
    CLSYS_DRAW_MAIN,	// �T�[�t�F�[�X�^�C�v(CLSYS_DRAW_TYPE)
    CLSYS_REND_CULLING_TYPE_NORMAL,       // �J�����O�^�C�v
  },
  {
    0,256,			// �T�[�t�F�[�X���゘���W			// �T�[�t�F�[�X���゙���W
    256,				// �T�[�t�F�[�X��
    192,				// �T�[�t�F�[�X����
    CLSYS_DRAW_SUB,	// �T�[�t�F�[�X�^�C�v(CLSYS_DRAW_TYPE)
    CLSYS_REND_CULLING_TYPE_NORMAL,       // �J�����O�^�C�v
  }};

static void InitCellActor(WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle)
{
  int i;

  const u8 CELL_MAX = 17*4;

  GFL_CLACT_SYS_Create( &fldmapdata_CLSYS_Init , &_defVBTbl, HEAPID_WIFIP2PMATCH );
  wk->clactSet  = GFL_CLACT_UNIT_Create( CELL_MAX , 0, HEAPID_WIFIP2PMATCH );
  //GFL_CLACT_UNIT_SetDefaultRend( wk->clactSet );

  {
    wk->renddata =  GFL_CLACT_USERREND_Create( sini, NELEMS(sini), HEAPID_WIFIP2PMATCH );
    GFL_CLACT_UNIT_SetUserRend(wk->clactSet, wk->renddata);
  }

}

#define TRAINER_NAME_POS_X    ( 24 )
#define TRAINER_NAME_POS_Y    ( 32 )
#define TRAINER_NAME_POS_SPAN ( 32 )

#define TRAINER_NAME_WIN_X    (  3 )
#define TRAINER1_NAME_WIN_Y   (  6 )
#define TRAINER2_NAME_WIN_Y   (  7 )


// �͂��E������BMP�i����ʁj
#define YESNO_WIN_FRAME_CHAR  ( 1 + TALK_WIN_CGX_SIZ )
#define YESNO_CHARA_OFFSET    ( 1 + TALK_WIN_CGX_SIZ + MENU_WIN_CGX_SIZ + FLD_MSG_WIN_SX*FLD_MSG_WIN_SY )
#define YESNO_WINDOW_X      ( 22 )
#define YESNO_WINDOW_Y1     (  7 )
#define YESNO_WINDOW_Y2     ( 13 )
#define YESNO_CHARA_W     (  8 )
#define YESNO_CHARA_H     (  4 )

/* static const NET_BMPWIN_DAT TouchYesNoBmpDat[2]={
 *     {
 *         GFL_BG_FRAME0_M, YESNO_WINDOW_X, YESNO_WINDOW_Y1,
 *         YESNO_CHARA_W, YESNO_CHARA_H, 13, YESNO_CHARA_OFFSET
 *         },
 *     {
 *         GFL_BG_FRAME0_M, YESNO_WINDOW_X, YESNO_WINDOW_Y2,
 *         YESNO_CHARA_W, YESNO_CHARA_H, 13, YESNO_CHARA_OFFSET+YESNO_CHARA_W*YESNO_CHARA_H
 *
 *         }
 * };
 */



//------------------------------------------------------------------
/**
 * $brief   �J�n���̃��b�Z�[�W   WIFIP2PMATCH_MODE_INIT
 * @param   wk
 * @param   seq
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_MainInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 status,gamemode;

  if(wk->pParentWork->seq != WIFI_GAME_NONE){
    _initBGMVol( wk, WIFI_STATUS_PLAYING);
    if(((wk->state == WIFIP2PMATCH_STATE_TALK) || (wk->state == WIFIP2PMATCH_STATE_RECV))
       && (!GFL_NET_DWC_IsDisconnect())){  //�b�������ڑ���
      //������VCT�ؒf
      DWCRAP_StopVChat();
      WifiP2PMatch_FriendListInit2( wk, seq );
      wk->friendNo = wk->pParentWork->friendNo;
      _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_UNIONMATCH);
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_SECOND_MATCH, WB_NET_WIFICLUB);
      _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT0);
    }
    else{  //����Ɛؒf
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_VCTEND, WB_NET_WIFICLUB);
      _CHANGESTATE(wk, WIFIP2PMATCH_VCTEND_COMMSEND2);
    }
  }
  else
  {        // �ڑ����܂�
    _CHANGESTATE(wk, WIFIP2PMATCH_FRIENDLIST_INIT);
    _initBGMVol( wk, WIFI_STATUS_WAIT);
  }

  //BTS2447 �Ώ� nagihashi
  //���C�u�L���X�^�[���ł����ɖ߂��Ă���ƍēxPlayBGM�����Ă��܂��̂ŁA
  //�����Ȃ̎��͖炳�Ȃ��悤�ɂ���B
  if( PMSND_GetNextBGMsoundNo() != SEQ_BGM_POKECEN )
  {
    PMSND_PlayBGM(SEQ_BGM_POKECEN);
  }
  return seq;
}


#if 0

//WIFIP2PMATCH_VCTEND_COMMSEND2
static int WifiP2PMatch_VCTDisconnectSend2(WIFIP2PMATCH_WORK *wk, int seq)
{
  wk->VChatModeOff = FALSE;
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_VCTEND, WB_NET_WIFICLUB)){

WIFIP2PMATCH_RECONECTING_WAIT_PRE


if(GFL_NET_HANDLE_IsTime(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_DISCONNECT_END, WB_NET_WIFICLUB);


      GFL_NET_SetAutoErrorCheck(FALSE);
      GFL_NET_SetNoChildErrorCheck(FALSE);


      GFL_NET_StateWifiMatchEnd(TRUE);  // �}�b�`���O��؂�
      _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
      wk->timer = _RECONECTING_WAIT_TIME;
      _CHANGESTATE(wk, WIFIP2PMATCH_RECONECTING_WAIT);
#endif


//------------------------------------------------------------------
/**
 * $brief   ���O�C���ڑ���  WIFIP2PMATCH_CONNECTING_INIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_ConnectingInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  GFL_BG_SetPriority(GFL_BG_FRAME0_M , 3);  //�͂�����
  GFL_BG_SetPriority(GFL_BG_FRAME1_M , 1);   // yesno win
  GFL_BG_SetPriority(GFL_BG_FRAME3_M , 1);  // ���b�Z�[�W
  GFL_BG_SetPriority(GFL_BG_FRAME2_M , 0);   //
  _CHANGESTATE(wk,WIFIP2PMATCH_CONNECTING);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   _sendMatchStatus �X�e�[�^�X�𑗂�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _sendMatchStatus(WIFIP2PMATCH_WORK *wk)
{
  GFL_NET_DWC_SetMyInfo((const char *)wk->pMatch, sizeof(WIFI_STATUS));
}

//------------------------------------------------------------------
/**
 * $brief   MyMatchStatus�쐬
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static void _makeMyMatchStatus(WIFIP2PMATCH_WORK* wk, u32 status, u32 gamemode)
{
  MYSTATUS* pMyStatus = GAMEDATA_GetMyStatus(wk->pGameData);

  WIFI_STATUS_SetPMVersion(wk->pMatch, PM_VERSION);
  WIFI_STATUS_SetPMLang(wk->pMatch, PM_LANG);
  WIFI_STATUS_SetSex(wk->pMatch, MyStatus_GetMySex(pMyStatus));
  WIFI_STATUS_SetTrainerView(wk->pMatch,MyStatus_GetTrainerView(pMyStatus));
  _myStatusChange_not_send(wk, status, gamemode); // BGM��ԂȂǂ𒲐�
  WIFI_STATUS_SetMyNation(wk->pMatch, MyStatus_GetMyNation(pMyStatus));
  WIFI_STATUS_SetMyArea(wk->pMatch, MyStatus_GetMyArea(pMyStatus));
  WIFI_STATUS_SetVChatStatus(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetUseVChat(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetPlayerID(wk->pMatch, MyStatus_GetID(pMyStatus));
  WIFI_STATUS_SetGameSyncID(wk->pMatch, MyStatus_GetProfileID(pMyStatus));
  
  _sendMatchStatus(wk);

}

//------------------------------------------------------------------
/**
 * $brief   �t�����h��status�ǂݍ���
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _readFriendMatchStatus(WIFIP2PMATCH_WORK* wk)
{
  int i,num = 0;
  WIFI_STATUS* p_status;
  u32 status;

  //    MI_CpuFill8(wk->index2No, 0, sizeof(WIFIP2PMATCH_MEMBER_MAX));
  for(i = 0;i < WIFIP2PMATCH_MEMBER_MAX;i++){
    wk->index2No[i] = 0;

    // ���̐l�͎����̗F�B���H
    if( wk->friend_num > i ){
      // ���̃`�F�b�N�������i��������
      // �ŐV������M���ď��߂ēo�^�����
      if(DWC_STATUS_OFFLINE != WifiDwc_getFriendStatus(i)){
        p_status = WifiFriendMatchStatusGet( i );
        status = _WifiMyStatusGet( wk, p_status );

        if( (status != WIFI_STATUS_NONE) &&
            (status < WIFI_STATUS_UNKNOWN) ){
          //          GF_ASSERT(p_status->trainer_view != PLHERO);
          wk->index2No[num] = i+1;
          num++;
        }else{
          if( MCVSys_BttnTypeGet( wk, i+1 ) != MCV_BTTN_FRIEND_TYPE_NONE ){
            MCVSys_BttnDel( wk, i+1 );
          }
        }
      }
    }
  }
  return num;
}

//------------------------------------------------------------------
/**
 * $brief   �t�����h��status�����@������̏�Ԃƕς������ς��������Ԃ�
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _checkUserDataMatchStatus(WIFIP2PMATCH_WORK* wk)
{
  int i,num = 0;
  BOOL back_up_wait;
  BOOL now_wait;
  MCR_MOVEOBJ* p_obj;
  WIFI_STATUS* p_status;
  u8 status;
  u8 gamemode;

  for(i = 0;i < WIFIP2PMATCH_MEMBER_MAX;i++){

    p_status = WifiFriendMatchStatusGet( i );
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = _WifiMyGameModeGet(wk, p_status);
    if(DWC_STATUS_OFFLINE == WifiDwc_getFriendStatus( i )){
      status = WIFI_GAME_NONE;
    }
    if((wk->matchStatusBackup[i]  != status) ||
       (wk->matchGamemodeBackup[i]  != gamemode) ||
       (wk->matchVchatBackup[i]  != WIFI_STATUS_GetVChatStatus(p_status)) ){

      // �I�u�W�F�N�g���[�N
      p_obj = MCRSYS_GetMoveObjWork( wk, i+1 );

      // ���̃I�u�W�F���o�^����A�V������Ԃ��`��I�u�W�F�ɔ��f�����܂ł�
      // �o�b�N�A�b�v�X�e�[�^�X���X�V���Ȃ�
      if( p_obj ){

        // �X�e�[�^�X���҂���ԂɂȂ�����W�����v������
        // �҂���Ԃ��畁�ʂɖ߂�����ʏ�ɖ߂�
        back_up_wait = _modeWait( wk->matchStatusBackup[i] );
        now_wait = _modeWait( status );
        if( (back_up_wait == TRUE) && (now_wait == FALSE) ){
          //NPC�̒ʏ퓮��ɖ߂�
          WIFI_MCR_NpcMoveSet( &wk->matchroom, p_obj,  MCR_NPC_MOVE_NORMAL );
        }else if( (back_up_wait == FALSE) && (now_wait == TRUE) ){

          //NPC���W�����v����ɂ���
          WIFI_MCR_NpcMoveSet( &wk->matchroom, p_obj,  MCR_NPC_MOVE_JUMP );
        }
        {   //�厖�ȏ��Z�b�g
          WifiList_SetFriendInfo(wk->pList, i, WIFILIST_FRIEND_UNION_GRA, WIFI_STATUS_GetTrainerView(p_status));
          WifiList_SetFriendInfo(wk->pList, i, WIFILIST_FRIEND_SEX, WIFI_STATUS_GetSex(p_status));
          WifiList_SetFriendInfo(wk->pList, i, WIFILIST_GAMESYNC_ID, WIFI_STATUS_GetGameSyncID(p_status));
          WifiList_SetFriendInfo(wk->pList, i, WIFILIST_FRIEND_ID, WIFI_STATUS_GetPlayerID(p_status));
        }
      }
      wk->matchStatusBackup[i] = status;
      wk->matchGamemodeBackup[i] = gamemode;
      wk->matchVchatBackup[i] = WIFI_STATUS_GetVChatStatus(p_status);
      num++;
    }
  }
  return num;
}

//------------------------------------------------------------------
/**
 * $brief   �G���[�\��
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static void _errorDisp(WIFIP2PMATCH_WORK* wk)
{
  GFL_NETSTATE_DWCERROR* pErr = GFL_NET_StateGetWifiError();
  int msgno,no = pErr->errorCode;
  int type =  GFL_NET_DWC_ErrorType(pErr->errorCode, pErr->errorType);

  if((type == 11) || (no == ERRORCODE_0)){
    msgno = dwc_error_0015;
    type = 11;
  }
  else if(no == ERRORCODE_HEAP){
    msgno = dwc_error_0014;
    type = 12;
  }
  else{
    if( type >= 0 ){
      msgno = dwc_error_0001 + type;
    }else{
      msgno = dwc_error_0012;
    }
  }
  OS_TPrintf("�G���[���b�Z�[�W %d \n",msgno);
  EndMessageWindowOff(wk);
  WORDSET_RegisterNumber(wk->WordSet, 0, no,
                         5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);

  // �G���[���b�Z�[�W��\�����Ă����V���N��
  wk->timer = 30;

  // ���ʉ��̎�����񕶎�����N���A����
  _systemMessagePrint(wk, msgno);

  switch(type){
  case 1:
  case 4:
  case 5:
  case 11:
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_INIT);
    break;
  case 6:
  case 7:
  case 8:
  case 9:
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_INIT);
    break;
  case 10:
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_INIT);
    break;
  case 0:
  case 2:
  case 3:
  default:
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CHECK_AND_END);
    break;
  }
}

//------------------------------------------------------------------
/**
 * $brief   �ڑ������g���C����  WIFIP2PMATCH_RETRY_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _retryInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( wk->timer > 0 ){
    wk->timer --;
    return seq;
  }

  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_052, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_YESNO);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �ڑ������g���C����  WIFIP2PMATCH_RETRY_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _retryYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( WifiP2PMatchMessageEndCheck(wk) ){

    _yenowinCreateM2( wk );

    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �ڑ������g���C����  WIFIP2PMATCH_RETRY_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _retryWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret;

  ret = _bmpMenu_YesNoSelectMain(wk);

  if(ret == BMPMENU_NULL)
  {  // �܂��I��
    return seq;
  }
  else if(ret == 0)
  { // �͂���I�������ꍇ
    //GFL_NET_StateWifiLogout();
    GFL_NET_Exit(NULL);
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY);
  }
  else
  {  // ��������I�������ꍇ
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT );  // VCHAT�̏�Ԃ����ɖ߂�����
    //GFL_NET_StateWifiLogout();
    GFL_NET_Exit(NULL);
    //@@OO     CommInfoFinalize();   //Info������
    wk->endSeq = WIFI_GAME_NONE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
  }
  EndMessageWindowOff(wk);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �ڑ������g���C  WIFIP2PMATCH_RETRAY
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _retry( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  ARCHANDLE* p_handle;

  if(!GFL_NET_IsResetEnable())
  {

    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE )
    {

      // �u���b�N�A�E�g
      //      WIPE_SetBrightness( WIPE_DISP_MAIN, WIPE_FADE_BLACK );
      //      WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
      //      GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,16,16,0);

      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
                      WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );

      MCRSYS_AllDelMoveObjWork(wk);

      // OAM�o�b�t�@�N���[��
      GFL_CLACT_SYS_ClearAllOamBuffer();
    }
    // �r���[�A�[�j��
    if( MCVSys_MoveCheck( wk ) == TRUE ){
      MCVSys_Exit( wk );

    }

    // �r�b�g�}�b�vOFF
    // ���[�U�[�f�[�^
    wk->MyInfoWinBack = _BmpWinDel(wk->MyInfoWinBack);

    //�ڑ��J�n
    GFL_BG_ClearFrame( GFL_BG_FRAME3_M);

    // �O���t�B�b�N���Z�b�g
    p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, HEAPID_WIFIP2PMATCH );
    BgGraphicSet( wk, p_handle );
    GFL_ARC_CloseDataHandle( p_handle );



    // �u���C�g�l�X��ԃ��Z���g
    // MCR��ʔj�����ɈÂ�������������
    wk->brightness_reset = TRUE;
    //    WIPE_ResetBrightness( WIPE_DISP_MAIN );
    //    WIPE_ResetBrightness( WIPE_DISP_SUB );

    if( wk->menulist){
      GFL_BMPWIN_Delete(wk->ListWin);
      BmpMenuList_Exit(wk->lw, NULL, NULL);
      wk->lw = NULL;
      BmpMenuWork_ListDelete( wk->menulist );
      wk->menulist=NULL;
    }

    //        wk->friendMatchReadCount;
    MI_CpuClear8(wk->index2No, sizeof(wk->index2No));
    MI_CpuClear8(wk->index2NoBackUp, sizeof(wk->index2NoBackUp));
    MI_CpuClear8(wk->matchStatusBackup, sizeof(wk->matchStatusBackup));
    MI_CpuClear8(wk->matchGamemodeBackup, sizeof(wk->matchGamemodeBackup));
    MI_CpuClear8(wk->matchVchatBackup, sizeof(wk->matchVchatBackup));

#if 0
    for(i=0;i < _OAM_NUM;i++){
      CLACT_SetDrawFlag( wk->MainActWork[i], 0 );
    }
#endif
    MainMenuMsgInit(wk);
    _GFL_NET_InitAndStruct(wk,FALSE);
    //    GFL_NET_Init(&aGFLNetInit, NULL, wk);
    GFL_NET_StateWifiEnterLogin();
    //        wk->pMatch = GFL_NET_StateWifiEnterLogin(wk->pSaveData,sizeof(TEST_MATCH_WORK));
    WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_CONNECTING_INIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ���O�C���ڑ���  WIFIP2PMATCH_CONNECTING
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_Connecting( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if( GFL_NET_DWC_GetSaving()) {
    _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_SAVING);
    WifiP2PMatchMessagePrint(wk, dwc_message_0015, TRUE);
  }

  if(GFL_NET_StateIsWifiLoginState()){
    //OS_TPrintf("WIFI�ڑ�����\n");
    if( wk->bInitMessage ){  // ����ڑ����ɂ̓Z�[�u�V�[�P���X��
      //            SaveData_SaveParts(wk->pSaveData, SVBLK_ID_NORMAL);  //�Z�[�u��
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG);
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
    }
  }
  else if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(wk);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ����Z�[�u����
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FirstSaving( WIFIP2PMATCH_WORK *wk, int seq )

{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if( GFL_NET_DWC_GetSaving()) {
    GAMEDATA_SaveAsyncStart(wk->pGameData);
    _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_SAVING2);
    return seq;
  }
  if(GFL_NET_StateIsWifiLoginState()){
    //OS_TPrintf("WIFI�ڑ�����\n");
    if( wk->bInitMessage ){
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG);
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
    }
  }
  else if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(wk);
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   ����Z�[�u����  WIFIP2PMATCH_FIRST_SAVING2
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FirstSaving2( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_NET_DWC_GetSaving()){
    SAVE_RESULT result = GAMEDATA_SaveAsyncMain(wk->pGameData);
    if (result != SAVE_RESULT_CONTINUE && result != SAVE_RESULT_LAST) {
      GFL_NET_DWC_ResetSaving();
    }
    else{
      return seq;
    }
  }
  if(GFL_NET_StateIsWifiLoginState()){
    //OS_TPrintf("WIFI�ڑ�����\n");
    if( wk->bInitMessage ){
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG);
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
    }
  }
  else if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(wk);
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   �ؒf�{VCT�҂��Ɉڍs����܂ő҂�  WIFIP2PMATCH_RECONECTING_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_ReConnectingWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;

  if(wk->timer > 0){
    wk->timer--;
    return seq;
  }

  if(wk->rpm){
    PokeRegulation_DeletePrintMsg(wk->rpm);
    wk->rpm = NULL;
  }

  
  if(GFL_NET_StateIsWifiLoginMatchState()){
    //OS_TPrintf("WIFI�ڑ�����\n");
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
      MCRSYS_AllDelMoveObjWork(wk);
    }
    _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
  }
  else if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(wk);
  }
  else if((GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_DISCONNECTING) || GFL_NET_StateIsWifiDisconnect() || !GFL_NET_IsConnectMember(GFL_NET_NETID_SERVER)){
    GFL_NET_StateWifiMatchEnd(TRUE);
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
      MCRSYS_AllDelMoveObjWork(wk);
    }
    _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
  }
  return seq;
}




//------------------------------------------------------------------
/**
 * $brief   �ŏ��ڑ�����  WIFIP2PMATCH_FIRST_ENDMSG
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _firstConnectEndMsg( WIFIP2PMATCH_WORK *wk, int seq )
{
  EndMessageWindowOff(wk);
  _systemMessagePrint(wk, dwc_message_0004);
  _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG_WAIT);
  //#if AFTER_MASTER_070410_WIFIAPP_N22_EUR_FIX
  wk->bInitMessage = FALSE;
  //#endif //AFTER_MASTER_070410_WIFIAPP_N22_EUR_FIX
  return seq;

}

//WIFIP2PMATCH_FIRST_ENDMSG_WAIT
static int _firstConnectEndMsgWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    if( 0==WifiList_GetFriendDataNum(wk->pList) ){  //�t�����h�����Ȃ�
      WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXITING);
      wk->timer = 1;
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      {
        _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
      }
      wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
    }
  }
  return seq;

}

//------------------------------------------------------------------
/**
 * $brief   �t�����h���X�g�\��������
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------

static void _userDataDisp(WIFIP2PMATCH_WORK* wk)
{
  if(wk->MyInfoWin){
    GFL_BMPWIN_Delete(wk->MyInfoWin);
  }

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*MCV_SYSFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  wk->MyInfoWinBack = _BmpWinDel(wk->MyInfoWinBack);

  wk->MyInfoWin=GFL_BMPWIN_Create(
    GFL_BG_FRAME3_M,
    WIFIP2PMATCH_PLAYER_DISP_X, WIFIP2PMATCH_PLAYER_DISP_Y,
    WIFIP2PMATCH_PLAYER_DISP_SIZX, WIFIP2PMATCH_PLAYER_DISP_SIZY,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );


  wk->MyInfoWinBack=GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    WIFIP2PMATCH_PLAYER_DISP_X, WIFIP2PMATCH_PLAYER_DISP_Y,
    WIFIP2PMATCH_PLAYER_DISP_SIZX, WIFIP2PMATCH_PLAYER_DISP_SIZY,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );

  // ������
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MyInfoWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWin);
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MyInfoWinBack), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWinBack);


  // �V�X�e���E�B���h�E�ݒ�
  //  BmpMenuWinWrite( wk->MyInfoWinBack, WINDOW_TRANS_ON, MENU_WIN_CGX_NUM, MENU_WIN_PAL );
  //    GFL_BMPWIN_MakeFrameScreen(wk->MyInfoWinBack, MENU_WIN_CGX_NUM, MENU_WIN_PAL);
  //BmpWinFrame_CgxSet(GFL_BG_FRAME1_M,COMM_TALK_WIN_CGX_NUM, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);
  BmpWinFrame_Write( wk->MyInfoWinBack, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFRAME_PAL );
}

// �_�~�[�I�u�W�F�N�g��o�^���܂���
static void DEBUG_DummyObjIn( WIFIP2PMATCH_WORK *wk, int num )
{
  int i;
  MCR_MOVEOBJ* p_moveobj;

  for( i=0; i<num; i++ ){
    // �������o��
    p_moveobj = WIFI_MCR_SetNpc( &wk->matchroom, i%16, i+1 );

    // �݂�Ȕ�ђ��˂�
    WIFI_MCR_NpcMoveSet( &wk->matchroom, p_moveobj, MCR_NPC_MOVE_JUMP );

    MCRSYS_SetMoveObjWork( wk, p_moveobj ); // �o�^�������[�N��ۑ����Ă���

  }
}

//------------------------------------------------------------------
/**
 * $brief   �}�b�`���O���Ă悢���ǂ����̔���
 * @param   friendIndex���肷��F�l��index
 * @retval  �}�b�`���Ook�Ȃ�TRUE
 */
//------------------------------------------------------------------

static BOOL WIFIP2PModeCheck( int friendIndex ,void* pWork)
{
  u32 mySt,myGamemode;
  u32 targetSt,targetGamemode;
  WIFIP2PMATCH_WORK *wk = pWork;
  WIFI_STATUS* pMatch = wk->pMatch;
  WIFI_STATUS* p_status = WifiFriendMatchStatusGet( friendIndex);

  mySt = WIFI_STATUS_GetWifiStatus(pMatch);
  targetSt = WIFI_STATUS_GetWifiStatus(p_status);

  myGamemode = WIFI_STATUS_GetGameMode(pMatch);
  targetGamemode = WIFI_STATUS_GetGameMode(p_status);

  if(myGamemode == targetGamemode){
    return TRUE;
  }
  return FALSE;
}



//------------------------------------------------------------------
/**
 * $brief   �t�����h���X�g�\��������   WIFIP2PMATCH_FRIENDLIST_INIT
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FriendListInit( WIFIP2PMATCH_WORK *wk, int seq )
{

  // �G���[�`�F�b�N
  if( GFL_NET_StateIsWifiError() == FALSE ){

    // �ؒf����
    if(!GFL_NET_StateIsWifiLoginMatchState()){
      return seq;
    }
  }
  _CHANGESTATE(wk,WIFIP2PMATCH_FRIENDLIST_INIT2);
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   �t�����h���X�g�\��������   WIFIP2PMATCH_FRIENDLIST_INIT2
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FriendListInit2( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i,x;
  int num = 0;
  ARCHANDLE* p_handle;
  MCR_MOVEOBJ* p_moveobj;
  int obj_code;

  // �R�[���o�b�N��ݒ�
  GFL_NET_DWC_SetConnectModeCheckCallback( WIFIP2PModeCheck );

  // ���������͒ʐM�G���[���V�X�e���E�B���h�E�ŏo��


  p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, HEAPID_WIFIP2PMATCH );



  _myVChatStatusOrgSet(wk);

  _windelandSEcall(wk);

  MI_CpuClear8(wk->index2NoBackUp, sizeof(wk->index2NoBackUp));


  if(wk->menulist){
    GFL_BMPWIN_Delete(wk->ListWin);
    BmpMenuList_Exit(wk->lw, NULL, NULL);
    wk->lw = NULL;
    BmpMenuWork_ListDelete( wk->menulist );
    wk->menulist = NULL;
  }
  if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){

    if( MyStatus_GetMySex( GAMEDATA_GetMyStatus(wk->pGameData) ) == PM_MALE ){
      obj_code = WB_HERO;
    }else{
      obj_code = WB_HEROINE;
    }

    // �}�b�`���O���[��������
    wk->friend_num = WifiList_GetFriendDataLastIdx( wk->pList );
#ifdef WFP2PM_MANY_OBJ
    wk->friend_num = 32;
#endif
    WIFI_MCR_Init( &wk->matchroom, HEAPID_WIFIP2PMATCH, p_handle,
                   obj_code, wk->friend_num, ARCID_WIFIP2PMATCH,wk->clactSet, wk->renddata );

    p_moveobj = WIFI_MCR_SetPlayer( &wk->matchroom, obj_code );

    MCRSYS_SetMoveObjWork( wk, p_moveobj ); // �o�^�������[�N��ۑ����Ă���

#ifdef WFP2PM_MANY_OBJ
    DEBUG_DummyObjIn( wk, wk->friend_num );
#endif
  }
  // �r���[�A�[������
  if( MCVSys_MoveCheck( wk ) == FALSE ){
    MCVSys_Init( wk, p_handle, HEAPID_WIFIP2PMATCH );
  }

  //MAC�A�h���X�����Z�b�g���Ă���
  WIFI_STATUS_ResetVChatMac(wk->pMatch);
  wk->DirectMacSet = 0;

  // ���̏�Ԃ���������
  MCVSys_ReWrite( wk, HEAPID_WIFIP2PMATCH );


  // ���[�U�[�f�[�^�\��
  EndMessageWindowOff(wk);
  _readFriendMatchStatus(wk);

  // ���[�U�[�f�[�^��BMP�쐬
  _userDataDisp(wk);

  _myStatusChange(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);

  _commStateChange(wk,WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
  _GFL_NET_InitAndStruct(wk,FALSE);

  wk->preConnect = -1;

  // 080605 tomoya BTS:249�̑Ώ��̂���
  // �e�̒ʐM���Z�b�g�����㒼���ɐڑ����Ă��܂��̂����
  // �i�ڑ�����Ă�VCHAT�̏�Ԃ̂܂܁AFriendList�V�[�P���X�Ɉڍs������j
  //  mydwc_ResetNewPlayer(); // ���Ȃ݂�mydwc_returnLobby���ł����������Ă�
#if 0
  CLACT_SetDrawFlag( wk->MainActWork[_CLACT_LINE_CUR], 1 );
#endif

  GFL_ARC_CloseDataHandle( p_handle );

  // ���C�v�C��
  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );

  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  FriendRequestWaitOff(wk);
  GFL_NET_SetAutoErrorCheck(FALSE);
  GFL_NET_SetNoChildErrorCheck(FALSE);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �N�����ڑ����Ă����ꍇ
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _checkParentConnect(WIFIP2PMATCH_WORK *wk)
{
  if( GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED ){
    return TRUE;
  }
  return FALSE;
}

static BOOL _checkParentNewPlayer( WIFIP2PMATCH_WORK *wk )
{
  if( (GFL_NET_DWC_IsNewPlayer() != -1) ){
    return TRUE;
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�̏o������Ǘ�����
 *
 *  @param  wk  ���[�N
 *
 *  @retval ���̗F�B�̐�
 */
//-----------------------------------------------------------------------------
static int MCRSYS_ContFiendInOut( WIFIP2PMATCH_WORK* wk )
{
  int friend_num;
  int i, j;
  MCR_MOVEOBJ* p_obj;
  BOOL match;
  WIFI_STATUS* p_status;
  BOOL in_flag;
  BOOL out_flag;

  // �ŐV�ł̗F�B���ƁA�F�B�i���o�[�z����쐬
  friend_num = _readFriendMatchStatus( wk );
  //  wk->friendMatchReadCount = friend_num;


  in_flag = FALSE;
  out_flag = FALSE;

  // �o�b�N�A�b�v�Ɣ�r����
  if( GFL_STD_MemComp( wk->index2No, wk->index2NoBackUp, sizeof(wk->index2NoBackUp) ) != 0 ){

    // �l���ς����
    // �o�b�N�A�b�v���ɂ��āA�ŐV�łɂ��Ȃ����OUT
    // �ŐV�łɂ��āA�o�b�N�A�b�v�łɂ��Ȃ����IN
    for( i=0; i<WIFIP2PMATCH_MEMBER_MAX; i++ ){

      // OUT�`�F�b�N
      match = FALSE;
      for( j=0; j<WIFIP2PMATCH_MEMBER_MAX; j++ ){
        if( wk->index2No[j] == wk->index2NoBackUp[i] ){
          match = TRUE;
          break;
        }
      }
      // ���Ȃ������̂ō폜
      // ��C��32�l�����Ă����Ƃ�0�����Ȃ��Ƃ������ɂȂ��Ă��܂��̂�
      // �O����Ȃ������`�F�b�N
      if( (match == FALSE) && (wk->index2NoBackUp[i] != 0) ){
        p_obj = MCRSYS_GetMoveObjWork( wk, wk->index2NoBackUp[i] );
        if( p_obj != NULL ){
          WIFI_MCR_DelPeopleReq( &wk->matchroom, p_obj );
          MCRSYS_DelMoveObjWork( wk, p_obj );
          MCVSys_BttnDel( wk, wk->index2NoBackUp[i] );
          WifiP2PMatch_UserDispOff_Target( wk, wk->index2NoBackUp[i], HEAPID_WIFIP2PMATCH );  // ���̐l��������ʂɏڍו\������Ă���Ȃ����
          out_flag = TRUE;
        }
        else{
          MCVSys_BttnDel( wk, wk->index2NoBackUp[i] );
        }
      }

      // IN�`�F�b�N
      match = FALSE;
      for( j=0; j<WIFIP2PMATCH_MEMBER_MAX; j++ ){
        if( wk->index2No[i] == wk->index2NoBackUp[j] ){
          match = TRUE;
          break;
        }
      }
      // �O�͂��Ȃ������̂Œǉ�
      if( (match == FALSE) && (wk->index2No[i] != 0) ){
        p_status = WifiFriendMatchStatusGet( wk->index2No[i]-1 );

        p_obj = WIFI_MCR_SetNpc( &wk->matchroom,
                                 WIFI_STATUS_GetTrainerView(p_status), wk->index2No[i] );


        // �o�^�ł������`�F�b�N
        if( p_obj ){

          MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_IN );

          MCRSYS_SetMoveObjWork( wk, p_obj );

          in_flag = TRUE;
        }else{

          // �I�u�W�F���o�^�ł��Ȃ������̂ŗ\��o�^
          // �i���ꂾ�ƃ{�^�����������Ƃ��o���Ȃ��j
          MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_RES );
   //       wk->index2No[i] = 0;  // �����Ă����̂͂Ȃ��������Ƃɂ���

        }
      }
    }

    // �ŐV�łɕύX
    // memcpy( wk->index2NoBackUp, wk->index2No, sizeof(u8)*WIFIP2PMATCH_MEMBER_MAX );
    GFL_STD_MemCopy(  wk->index2No, wk->index2NoBackUp,sizeof(wk->index2NoBackUp) );


    if( in_flag || out_flag){
      // �{�^���S�`�惊�N�G�X�g
      MCVSys_BttnAllWriteReq( wk );
    }
    //�o����SE
    if( in_flag == TRUE ){
      PMSND_PlaySE( SEQ_SE_FLD_05 );
    }else if( out_flag == TRUE ){
      PMSND_PlaySE( SEQ_SE_FLD_05 );
    }

  }


  return friend_num;
}




//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�̏o������Ǘ�����   OBJ�o�^���Ă��Ȃ����F�B����
 *
 *  @param  wk  ���[�N
 *
 *  @retval ���̗F�B�̐�
 */
//-----------------------------------------------------------------------------
static void MCRSYS_ContFiendInOut2( WIFIP2PMATCH_WORK* wk )
{
  int friend_num;
  int i, j;
  MCR_MOVEOBJ* p_obj;
  BOOL match;
  WIFI_STATUS* p_status;
  BOOL in_flag;

  in_flag = FALSE;
  for( i=0; i<WIFIP2PMATCH_MEMBER_MAX; i++ ){
    if(wk->index2No[i] != 0){ 
      p_obj = MCRSYS_GetMoveObjWork( wk, wk->index2No[i] );
      if(p_obj){
        continue;
      }
      p_status = WifiFriendMatchStatusGet( wk->index2No[i]-1 );
      
      p_obj = WIFI_MCR_SetNpc( &wk->matchroom,
                                 WIFI_STATUS_GetTrainerView(p_status), wk->index2No[i] );
      // �o�^�ł������`�F�b�N
      if( p_obj ){
        MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_IN );
        MCRSYS_SetMoveObjWork( wk, p_obj );

        wk->matchStatusBackup[i] = 0;
        wk->matchGamemodeBackup[i] = 0;
        wk->matchVchatBackup[i] = 0;

        in_flag = TRUE;
      }
    }
  }
  //�o����SE
  if( in_flag == TRUE ){
    PMSND_PlaySE( SEQ_SE_FLD_05 );
  }
}




//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�̏�ԕω���\��
 *
 *  @param  wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void MCRSYS_ContFriendStatus( WIFIP2PMATCH_WORK* wk, u32 heapID )
{
  int i;
  u32 friendID;
  u32 type;
  BOOL vchat;
  int change_num;

  // ��ԕύX�����擾�i���܂łƓ������ꏏ�ɂ��邽�߂ɂ������Ă����j
  change_num = _checkUserDataMatchStatus( wk );

  // �F�̏�ԕω���o�^����
  if( change_num > 0 ){
    MCVSys_ReWrite( wk, heapID );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�I�u�W�F�N�g�̓o�^�j���Ǘ�
 *
 *  @param  wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void MCRSYS_SetFriendObj( WIFIP2PMATCH_WORK* wk, u32 heapID )
{
  // �F�B�i���o�[���ς���Ă����炢�Ȃ��Ȃ����F�B�������A�������F�B��o�^
  MCRSYS_ContFiendInOut( wk );

  // ��Ԃ��ς�����牽����ς���
  MCRSYS_ContFriendStatus( wk, heapID );

  //�ז����ꂽ���F�B����
  MCRSYS_ContFiendInOut2( wk );
}

//------------------------------------------------------------------
/**
 * $brief   �e�̕\�������ɖ߂�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void MCRSYS_ReloadShadow( WIFIP2PMATCH_WORK* wk )
{
  int i;

  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
    if( wk->p_matchMoveObj[i] ){
      WIFI_MCR_ShadowOn(&wk->matchroom, wk->p_matchMoveObj[i]);  //�e���o��
    }
  }
}

//------------------------------------------------------------------
/**
 * $brief   �t�����h���X�g�\�������� WIFIP2PMATCH_MODE_FRIENDLIST
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_FriendListMain( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret = MCR_RET_NONE;
  int checkMatch;
  u32 check_friend;
  MCR_MOVEOBJ* p_obj;
  u32 status,gamemode;

  wk->vchatrev = 0;  //�}�V���ł̎b��VCT�t���O���Z�b�g

  // ���C�v�I���҂�
  if( !WIPE_SYS_EndCheck() ){
    return seq;
  }

  // �G���[�`�F�b�N
  if( GFL_NET_StateIsWifiError()){
    //        wk->localTime=0;
    _errorDisp(wk);
    return seq;
  }

  // �{�C�X�`���b�gONOFF�J��Ԃ��ł����̉�ʂł͂܂��
  // �����ԂɂȂ�����DISCONNECT�����ɑJ��
  if( GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_DISCONNECTING ){
    // ���̏����́A_parentModeCallMenuInit��������Ă��܂���
    // �K�����Ȃ�=>�ؒf������
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    _myStatusChange(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
    GFL_NET_StateWifiMatchEnd(TRUE);
    return seq;
  }

  // �F�B�����ꂽ��o��
  MCRSYS_SetFriendObj( wk, HEAPID_WIFIP2PMATCH );
  MCVSys_UpdataBttn( wk );


  // �p�\�R���A�j���������Ă�����I��
  WIFI_MCR_PCAnmOff( &wk->matchroom );

  // �{�C�X�`���b�g��ONOFF�̓v���C���[��������Ƃ��ɂ����ݒ�ł���
  if( WIFI_MCR_PlayerMovePauseGet( &wk->matchroom ) == FALSE ){
    // �{�C�X�`���b�g
    if(GFL_NET_DWC_IsNewPlayer() == -1){
      if(PAD_BUTTON_X & GFL_UI_KEY_GetTrg()){
        if(_myVChatStatusToggleOrg(wk)){
          WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_054, FALSE);
        }
        else{
          WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_055, FALSE);
        }
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        _userDataInfoDisp(wk);
        _CHANGESTATE(wk,WIFIP2PMATCH_VCHATWIN_WAIT);
        WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
        return seq;
      }
#if PM_DEBUG
      if(PAD_BUTTON_Y & GFL_UI_KEY_GetTrg()){
        if(WIFI_STATUS_GetGameMode(wk->pMatch) != WIFI_GAME_UNKNOWN){
          _myStatusChange(wk, WIFI_STATUS_RECRUIT,WIFI_GAME_UNKNOWN);
          WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_074, FALSE);
        }
        else{
          _myStatusChange(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
          WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_075, FALSE);
        }
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        _userDataInfoDisp(wk);
        _CHANGESTATE(wk,WIFIP2PMATCH_VCHATWIN_WAIT);
        WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
        return seq;
      }
#endif
    }

    // ������ʂ������Ȃ�
    check_friend = MCVSys_Updata( wk, HEAPID_WIFIP2PMATCH );  // ������ʂ�������
    p_obj = MCRSYS_GetMoveObjWork( wk, check_friend );
    if( check_friend != 0 ){
      if( p_obj ){
        WIFI_MCR_CursorOn( &wk->matchroom, p_obj );
      }else{
        MCRSYS_ReloadShadow(wk);
        WIFI_MCR_CursorOff( &wk->matchroom );
      }
    }else{
      MCRSYS_ReloadShadow(wk);
      WIFI_MCR_CursorOff( &wk->matchroom );
    }

  }


  //  if( wk->DirectModeNo!=0){
  //    if(!WIFI_STATUS_IsVChatMac(wk->pMatch, WifiFriendMatchStatusGet( wk->DirectModeNo-1 ))){
  //      wk->DirectModeNo=0;
  //}
  //  }
  {
    int j;
    for(j = 0; j < WIFIP2PMATCH_MEMBER_MAX ; j++){
      int n = GFUser_GetPublicRand0(WIFIP2PMATCH_MEMBER_MAX);  //�����Ⴂ���ɂȂ疳���悤�ɗ�������
      if(WIFI_STATUS_IsVChatMac(wk->pMatch, WifiFriendMatchStatusGet( n ))){
        u8 callcount = WIFI_STATUS_GetCallCounter(WifiFriendMatchStatusGet( n ));
        OS_TPrintf("�Ăяo�� %d %d\n",callcount,wk->pParentWork->matchno[n] );
        if(callcount != wk->pParentWork->matchno[n] ){
          //�������Ăяo����Ă���̂ŁA�ڑ��J�n ��Ԃ��擾
          status = _WifiMyStatusGet( wk, WifiFriendMatchStatusGet( n ) );
          gamemode = _WifiMyGameModeGet( wk, WifiFriendMatchStatusGet( n ) );
          wk->friendNo = n + 1;
          //OS_TPrintf("�_�C���N�g�ԍ��Z�b�g%d\n", wk->friendNo);
          PMSND_PlaySE_byPlayerID( SE_CALL_SIGN,SE_CALL_SIGN_PLAYER );
          _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALLGAME_INIT);
          return seq;
        }
      }
    }
  }


  // �F�B���炱����ɐڑ����Ă����Ƃ��̏���
  checkMatch = _checkParentConnect(wk);
  if( (0 !=  checkMatch) && (wk->preConnect != -1) ){ // �ڑ����Ă���
    OS_TPrintf("�ڑ� %d\n",wk->DirectModeNo);
    PMSND_PlaySE_byPlayerID( SE_CALL_SIGN,SE_CALL_SIGN_PLAYER );
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_INIT);
    return seq;
  }
  if((wk->preConnect == -1) && (GFL_NET_DWC_IsNewPlayer() != -1)){  // �ʏ�̃R�l�N�g�J�n
    wk->preConnect = GFL_NET_DWC_IsNewPlayer();
    _friendNameExpand(wk, wk->preConnect);
    if(wk->DirectMacSet==0){
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_043, FALSE);
    }

    // �Ȃ������l�̃f�[�^�\��
    //    WifiP2PMatch_UserDispOn( wk, wk->preConnect+1, HEAPID_WIFIP2PMATCH );

    wk->localTime = 0;
  }
  else if((wk->preConnect != -1) && (GFL_NET_DWC_IsNewPlayer() == -1)){ // �����I�ɂ͐V�����R�l�N�g���Ă����̂ɁA���ۂ̓R�l�N�g���Ă��Ă��Ȃ������Ƃ��H

    // �{�C�X�`���b�g�ݒ�
    GFL_NET_DWC_SetVChat(FALSE);// �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi
    _friendNameExpand(wk, wk->preConnect);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    GFL_NET_StateWifiMatchEnd(TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }

  // VCHAT�@ON�@��ԂȂ̂ɁA�V�����R�l�N�g��-1�Ȃ�VCHAT��OFF�ɂ���
  // 080615 tomoya
  if( (GFL_NET_DWC_IsVChat() == TRUE) && (GFL_NET_DWC_IsNewPlayer() == -1) ){
    GFL_NET_DWC_SetVChat(FALSE);// �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi
  }


  // ��Ԃ��擾
  status = _WifiMyStatusGet( wk, wk->pMatch );

  // �N�������ɐڑ����Ă��Ă��Ȃ��Ƃ��������X�g�𓮂�����
  if(wk->preConnect == -1){

    // CANCEL�{�^���ł��ҋ@��Ԃ��N���A
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
      if(_modeWait(status)){  // �҂���Ԃ̂Ƃ�
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
        WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
        return seq;
      }
    }
    ret = WIFI_MCR_Main( &wk->matchroom );
    WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��
  }
  switch(ret){
  case MCR_RET_NONE:
    return seq;
  case MCR_RET_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    if(_modeWait(status)){  // �҂���Ԃ̂Ƃ�
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
    }
    else{ // ���ꂩ�I���`�F�b�N��
      wk->endSeq = WIFI_GAME_NONE;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_YESNO);
      WifiP2PMatchMessagePrint(wk, dwc_message_0010, TRUE);
      //        wk->localTime=0;
      ret = BMPMENULIST_CANCEL;
    }
    WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
    return seq;

  case MCR_RET_MYSELECT:   //�p�\�R���ɘb��������
    {  // ��W�̍s�őI�������Ƃ�
      if(status == WIFI_STATUS_WAIT){
        wk->state = WIFIP2PMATCH_STATE_MACHINE;  //NOT�_�C���N�g
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        WIFI_MCR_PCAnmStart( &wk->matchroom );  // pc�A�j���J�n
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT);
        WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
        wk->DirectMacSet=0;
        return seq;
      }
    }
    break;
  case MCR_RET_SELECT:   //����ɘb��������
    if(WIFI_STATUS_WAIT==status){  // �l�̖��O�[���}�b�`���O��
      //����̏�Ԃ��m�� �Ȍケ�̏�Ԃ��݂�悤�ɕύX ����̓�������A���^�C���ɒǂ�Ȃ�
      int friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
      PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
      GFL_STD_MemCopy(WifiFriendMatchStatusGet( friendNo - 1 ), &wk->targetStatus, sizeof(WIFI_STATUS));
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT);
      WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
      return seq;
    }
    break;
  default:
    GF_ASSERT(0);
    break;
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   �t�����h���X�g�\�������� WIFIP2PMATCH_MODE_FRIENDLIST_MW
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_FriendListMain_MW( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret = MCR_RET_NONE;
  int checkMatch;
  u32 check_friend;
  MCR_MOVEOBJ* p_obj;
  u32 status,gamemode;

  //BTS3567�ABTS3795-b�̑Ώ� Saito
  //���b�Z�[�W������ꍇ�A���b�Z�[�W�𑁑��肷�邽�߂ɓǂ�ł��܂�
  WifiP2PMatchMessageEndCheck(wk);

  wk->vchatrev = 0;  //�}�V���ł̎b��VCT�t���O���Z�b�g

  // �F�B�����ꂽ��o��
  MCRSYS_SetFriendObj( wk, HEAPID_WIFIP2PMATCH );
  MCVSys_UpdataBttn( wk );

  // �p�\�R���A�j���������Ă�����I��
  WIFI_MCR_PCAnmOff( &wk->matchroom );

  // �F�B���炱����ɐڑ����Ă����Ƃ��̏���
  checkMatch = _checkParentConnect(wk);
  if( (0 !=  checkMatch) && (wk->preConnect != -1) ){ // �ڑ����Ă���
    OS_TPrintf("�ڑ� %d\n",wk->DirectModeNo);
    PMSND_PlaySE_byPlayerID( SE_CALL_SIGN,SE_CALL_SIGN_PLAYER );
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_INIT);
    return seq;
  }
  if((wk->preConnect == -1) && (GFL_NET_DWC_IsNewPlayer() != -1)){  // �ʏ�̃R�l�N�g�J�n
    wk->preConnect = GFL_NET_DWC_IsNewPlayer();
    _friendNameExpand(wk, wk->preConnect);
    if(wk->DirectMacSet==0){
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_043, FALSE);
    }
    wk->localTime = 0;
  }
  else if(GFL_NET_DWC_GetDisconnectNewPlayer() != -1){
    _friendNameExpand(wk, GFL_NET_DWC_GetDisconnectNewPlayer());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }


  // ��Ԃ��擾
  status = _WifiMyStatusGet( wk, wk->pMatch );

  // �N�������ɐڑ����Ă��Ă��Ȃ��Ƃ��������X�g�𓮂�����
  if(wk->preConnect == -1){

    // CANCEL�{�^���ł��ҋ@��Ԃ��N���A
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
      if(_modeWait(status)){  // �҂���Ԃ̂Ƃ�
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
        WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
        return seq;
      }
    }
    ret = WIFI_MCR_Main( &wk->matchroom );
    WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��
  }
  switch(ret){
  case MCR_RET_NONE:
    return seq;
  case MCR_RET_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    if(_modeWait(status)){  // �҂���Ԃ̂Ƃ�
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
    }
    else{ // ���ꂩ�I���`�F�b�N��
      wk->endSeq = WIFI_GAME_NONE;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_YESNO);
      WifiP2PMatchMessagePrint(wk, dwc_message_0010, TRUE);
      //        wk->localTime=0;
      ret = BMPMENULIST_CANCEL;
    }
    WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
    return seq;

  case MCR_RET_MYSELECT:   //�p�\�R���ɘb��������
    {  // ��W�̍s�őI�������Ƃ�
      if(status == WIFI_STATUS_WAIT){
        wk->state = WIFIP2PMATCH_STATE_MACHINE;  //NOT�_�C���N�g
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        WIFI_MCR_PCAnmStart( &wk->matchroom );  // pc�A�j���J�n
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT);
        WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
        return seq;
      }
    }
    break;
  case MCR_RET_SELECT:   //����ɘb��������
    if(WIFI_STATUS_WAIT==status){  // �l�̖��O�[���}�b�`���O��
      //����̏�Ԃ��m�� �Ȍケ�̏�Ԃ��݂�悤�ɕύX ����̓�������A���^�C���ɒǂ�Ȃ�
      int friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
      PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
      GFL_STD_MemCopy(WifiFriendMatchStatusGet( friendNo - 1 ), &wk->targetStatus, sizeof(WIFI_STATUS));
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT);
      WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����
      return seq;
    }
    break;
  default:
    GF_ASSERT(0);
    break;
  }
  return seq;
}




//------------------------------------------------------------------
/**
 * $brief   �b��������ꂽ�ꍇ�̊m�F��ʂ�   WIFIP2PMATCH_MODE_CALLGAME_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _callGameInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  // ���ꂩ���b�������Ă���
  int n = wk->friendNo -1;

  if(PMSND_CheckPlaySE_byPlayerID( SE_CALL_SIGN_PLAYER )){
    return seq;
  }
  {
    u8 callcount = WIFI_STATUS_GetCallCounter(WifiFriendMatchStatusGet( n ));
    wk->pParentWork->matchno[n] = callcount;   //�O��}�b�`���O��������no
  }
  
  if(n>=0 && n < WIFIP2PMATCH_MEMBER_MAX){
    if( WifiP2PMatch_CommWifiBattleStart( wk, n ) ){
      wk->cancelEnableTimer = _CANCELENABLE_TIMER;

      _commStateChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_UNIONMATCH);
      _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_UNIONMATCH);  // �ڑ����ɂȂ�

      WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // ������ʏ�����

      _friendNameExpand(wk, n);

      wk->cancelEnableTimer = _CANCELENABLE_TIMER;
      WifiP2PMatchMessagePrintDirect(wk,msg_wifilobby_082, FALSE);
      WifiP2PMatchMessage_TimeIconStart(wk);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_LOOP);

//      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_MATCH_LOOP_MSG);
//      {
//        u8 callcount = WIFI_STATUS_GetCallCounter(WifiFriendMatchStatusGet( n ));
//        wk->pParentWork->matchno[n] = callcount;   //�O��}�b�`���O��������no
//      }
      wk->DirectModeNo = wk->friendNo;
      return seq;
    }
  }
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST); //���Ƃɖ߂�

  return seq;

}

//------------------------------------------------------------------
/**
 * $brief   VCTONOFF�E�C���h�E�����  WIFIP2PMATCH_VCHATWIN_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatToggleWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_UI_KEY_GetTrg()){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    // ��l���̓��������
    FriendRequestWaitOff( wk );
    EndMessageWindowOff(wk);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT�ڑ��J�n
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectInit2( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2
{
  WIFI_STATUS* p_status;
  int vchat;
  int status;

  // VChat�t���O���擾
  p_status = WifiFriendMatchStatusGet( wk->friendNo - 1 );
  vchat = WIFI_STATUS_GetVChatStatus(p_status);
  status  = _WifiMyStatusGet( wk, p_status );

  wk->cancelEnableTimer--;
  if(wk->cancelEnableTimer < 0  ){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT);
  }
#if PLFIX_T869
  // VCHAT�t���O��OFF�ɂȂ��Ă�����ʐM�ؒf
  else if(vchat == FALSE){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
#endif
  // ����̏�Ԃ��ʏ��ԂłȂ��ꍇ�ؒf
  else if( (status != WIFI_STATUS_WAIT) && (status != WIFI_STATUS_RECRUIT) ){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED){  // �Ȃ�����
    //    _timeWaitIconDel(wk);   timeWait����MsgWin��j�����Ă���Ƃ������Ƃ̓��b�Z�[�W�I���ł�OK��
    EndMessageWindowOff(wk);
    // VCT�ڑ��J�n + �ڑ�MAC�͏���
    WIFI_STATUS_ResetVChatMac(wk->pMatch);
    wk->DirectMacSet = 0;
    _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_VCT);  // VCT���ɂȂ�
    wk->VChatModeOff = FALSE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
    //WifiList_SetLastPlayDate( wk->pList, wk->friendNo - 1); // �Ō�ɗV�񂾓��t�́AVCT���Ȃ������Ƃ��ɐݒ肷��
    _changeBGMVol( wk, 0 );

    WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_1015, FALSE);
  }

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT�ڑ��J�n
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectInit( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT
{

  // 080703 tomoya takahashi
  // ��͂�͂܂�Ƃ��͂���悤�Ȃ̂ŁA
  //�@�����Ɏ��ɂ����悤�ɏC������
  _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
  WifiP2PMatchMessagePrint(wk, msg_wifilobby_011, FALSE);
  _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_VCT);  // VCT���ɂȂ�
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT);

  //WifiList_SetLastPlayDate( wk->pList, GFL_NET_DWC_GetFriendIndex()); // �Ō�ɗV�񂾓��t�́AVCT���Ȃ������Ƃ��ɐݒ肷��

#if 0
  if(GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED){  // �Ȃ�����
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_011, FALSE);
    _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_VCT);  // VCT���ɂȂ�
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT);
  }
#endif

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT�ڑ��܂� WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectWait( WIFIP2PMATCH_WORK *wk, int seq )        //
{

  WIFI_STATUS* p_status;
  int vchat;
  int status;

  if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
    return seq;
  }

  // 080624�@�ǉ�
  // �ʐM���Z�b�g������index��0��菬�����Ȃ�
  // ��������ؒf
  if( GFL_NET_DWC_GetFriendIndex() < 0 ){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }

  if( WifiP2PMatchMessageEndCheck(wk) ){
    if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
      EndMessageWindowOff(wk);
      WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_1015, FALSE);

      _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_VCT);  // VCT���ɂȂ�

      _changeBGMVol( wk, 0 );
      wk->VChatModeOff = FALSE;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
    }
    else{
      WifiP2PMatch_VCTConnect(wk,seq);
    }
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   VCT�ڑ���  WIFIP2PMATCH_MODE_VCT_CONNECT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnect( WIFIP2PMATCH_WORK *wk, int seq )
{
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_VCTST, WB_NET_WIFICLUB);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_MAIN);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT�ڑ���  WIFIP2PMATCH_MODE_VCT_CONNECT_MAIN
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectMain( WIFIP2PMATCH_WORK *wk, int seq )
{
  WIFI_STATUS* p_status;
  int status,gamemode;

  // VChat�t���O���擾
  p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
  status  = _WifiMyStatusGet( wk, p_status );
  gamemode  = _WifiMyGameModeGet( wk, p_status );

  // �F�B�����ꂽ��o��
  MCRSYS_SetFriendObj( wk, HEAPID_WIFIP2PMATCH );
  MCVSys_UpdataBttn( wk );

  if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_017, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO);
  }
  else if(wk->VChatModeOff){
    EndMessageWindowOff(wk);
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
    wk->timer = 60;
    _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND3);
  }
  else{
    // �A�C�R����������
    if(GFL_NET_DWC_IsSendVoiceAndInc()){
      WifiP2PMatchFriendListIconWrite(   &wk->icon, GFL_BG_FRAME1_M,
                                         PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y, PLAYER_DISP_ICON_IDX_VCTBIG +PLAYER_DISP_ICON_MYMODE, 0 );
    }
    else{
      WifiP2PMatchFriendListIconWrite(   &wk->icon, GFL_BG_FRAME1_M,
                                         PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y, PLAYER_DISP_ICON_IDX_VCT_ACT + PLAYER_DISP_ICON_MYMODE, 0 );
    }
  }

  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   VCT�ڑ��I���҂�  WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO
 * @param   wk
 * @retval  seq
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectEndYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{


  if( WifiP2PMatchMessageEndCheck(wk)){
    // �Ō�ɗV�񂾓��t�́AVCT���Ȃ������Ƃ��ɐݒ肷��
    //WifiList_SetLastPlayDate( wk->pList, GFL_NET_DWC_GetFriendIndex());

    if(wk->VChatModeOff){
      EndMessageWindowOff(wk);
      _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
      wk->timer = 60;
      _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND3);
    }
    else{
      // �͂��������E�C���h�E���o��
      _yenowinCreateM2(wk);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT);
    }
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT�ڑ��I��YESNO�҂�  WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT
 * @param   wk
 * @retval  seq
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectEndWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret;
  ret = _bmpMenu_YesNoSelectMain(wk);

  if(ret == BMPMENU_NULL){  // �܂��I��
    if(wk->VChatModeOff){
      EndMessageWindowOff(wk);
      BmpMenu_YesNoMenuExit( wk->pYesNoWork );
      wk->pYesNoWork = NULL;
      _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
      wk->timer = 60;
      _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND3);
    }
  }
  else if(ret == 0)
  { // �͂���I�������ꍇ
    _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND1);
  }
  else
  {  // ��������I�������ꍇ
    WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_1015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
  }
  return seq;
}


//WIFIP2PMATCH_VCTEND_COMMSEND1
static int WifiP2PMatch_VCTDisconnectSend1(WIFIP2PMATCH_WORK *wk, int seq)
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_VCHATOFF, 0, NULL)){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_VCTEND, WB_NET_WIFICLUB);
    _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND2);
  }
  return seq;
}


//WIFIP2PMATCH_VCTEND_COMMSEND2_ENDCHK
//VCT���C�u�����̏I���҂�
static int WifiP2PMatch_VCTDisconnectSendEnd(WIFIP2PMATCH_WORK *wk, int seq)
{
  if(DWCRAP_IsVChat()==FALSE){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_SECOND_MATCH, WB_NET_WIFICLUB);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT0);
  }
  return seq;
}


//WIFIP2PMATCH_VCTEND_COMMSEND2
static int WifiP2PMatch_VCTDisconnectSend2(WIFIP2PMATCH_WORK *wk, int seq)
{
  wk->VChatModeOff = FALSE;

  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_VCTEND, WB_NET_WIFICLUB) || (GFL_NET_HANDLE_GetNumNegotiation()!=2 )){
    if((wk->state == WIFIP2PMATCH_STATE_TALK ) || (wk->state == WIFIP2PMATCH_STATE_RECV )){
      //������VCT�ؒf
      DWCRAP_StopVChat();
      _changeBGMVol( wk, _VOL_DEFAULT );
      EndMessageWindowOff(wk);
      _myStatusChange(wk, WIFI_STATUS_CALL,WIFI_GAME_UNIONMATCH);
      _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND2_ENDCHK);
    }
    else{
      _myStatusChange(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
      GFL_NET_SetAutoErrorCheck(FALSE);
      GFL_NET_SetNoChildErrorCheck(FALSE);
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_DISCONNECT_END, WB_NET_WIFICLUB);
      _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT_PRE);
    }
  }
  return seq;
}



//WIFIP2PMATCH_RECONECTING_WAIT_PRE
static int WifiP2PMatch_ReconectingWaitPre(WIFIP2PMATCH_WORK *wk, int seq)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_DISCONNECT_END, WB_NET_WIFICLUB)
     || (GFL_NET_HANDLE_GetNumNegotiation()!=2 )){
    GFL_NET_StateWifiMatchEnd(TRUE);
    wk->preConnect = -1;
    wk->timer = _RECONECTING_WAIT_TIME;
    EndMessageWindowOff(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
  }
  return seq;
}



//WIFIP2PMATCH_VCTEND_COMMSEND3
static int WifiP2PMatch_VCTDisconnectSend3(WIFIP2PMATCH_WORK *wk, int seq)
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  wk->timer--;
  if(wk->timer < 0){
    //   if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_VCHATOFF, 0, NULL)){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_VCTEND, WB_NET_WIFICLUB);
    _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND2);
    // }
  }
  return seq;
}

//WIFIP2PMATCH_MODE_VCT_DISCONNECT
static int WifiP2PMatch_VCTDisconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
  u32 status;

  if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
    return seq;
  }


  // ���[�U�[�f�[�^OFF
  //  WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );

  status = _WifiMyStatusGet( wk, wk->pMatch );

  if(status != WIFI_STATUS_WAIT){
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
  }
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    wk->timer = 30;
    return seq;
  }
  wk->timer--;
  if((GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)) || (wk->timer==0)){
    EndMessageWindowOff(wk);
    GFL_NET_StateWifiMatchEnd(TRUE);

    // �ʐM��Ԃ����ɖ߂�
    //GFL_NET_StateChangeWiFiLogin();
    //    GFL_NET_ChangeInitStruct(&aGFLNetInit);
    _GFL_NET_InitAndStruct(wk,FALSE);

    //        if(wk->menulist==NULL){
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){
      wk->timer = _RECONECTING_WAIT_TIME;
      _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
    }
    else{

      // VCHAT�����ɖ߂�
      _myVChatStatusOrgSet( wk );
      _userDataInfoDisp(wk);

      wk->preConnect = -1;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

      // ��l���̓��������
      FriendRequestWaitOff( wk );
    }
  }
  return seq;
}

//WIFIP2PMATCH_MODE_BATTLE_DISCONNECT
static int WifiP2PMatch_BattleDisconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    EndMessageWindowOff(wk);
    _myStatusChange(wk,  WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    GFL_NET_StateWifiMatchEnd(TRUE);
    wk->timer = _RECONECTING_WAIT_TIME;
    _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
  }
  return seq;
}




//WIFIP2PMATCH_MODE_DISCONNECT2
static int WifiP2PMatch_Disconnect2(WIFIP2PMATCH_WORK *wk, int seq)
{
  wk->timer++;
  if(wk->timer <= (60*4)){
    return seq;
  }

  // �b�������Ă���F�B�i���o�[�擾
  WIFI_MCR_NpcPauseOff( &wk->matchroom, MCRSYS_GetMoveObjWork( wk, WIFI_MCR_PlayerSelect( &wk->matchroom ) ) );

  EndMessageWindowOff(wk);
  _myStatusChange(wk,  WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);

  GFL_NET_StateWifiMatchEnd(TRUE);
  // �ʐM��Ԃ����ɖ߂�
  _GFL_NET_InitAndStruct(wk,FALSE);

  if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){
    _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
  }
  else{
    // VCHAT���I���W�i���ɖ߂�
    _myVChatStatusOrgSet( wk );
    _userDataInfoDisp(wk);

    wk->preConnect = -1;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    // ��l���̓��������
    FriendRequestWaitOff( wk );
  }
  return seq;
}




//WIFIP2PMATCH_MODE_DISCONNECT
static int WifiP2PMatch_Disconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
  // ���[�U�[�f�[�^OFF
  //  WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );
  //




  // �G���[�`�F�b�N
  if( GFL_NET_StateIsWifiError() ){
    _errorDisp(wk);
    return seq;
  }


  if( !WifiP2PMatchMessageEndCheck(wk) ){
    wk->timer = 30;
    return seq;
  }
  wk->timer--;
  if((GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)) || (wk->timer==0)){


    // �b�������Ă���F�B�i���o�[�擾
    WIFI_MCR_NpcPauseOff( &wk->matchroom, MCRSYS_GetMoveObjWork( wk, WIFI_MCR_PlayerSelect( &wk->matchroom ) ) );

    EndMessageWindowOff(wk);
    _myStatusChange(wk,  WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);

    GFL_NET_StateWifiMatchEnd(TRUE);
    // �ʐM��Ԃ����ɖ߂�
    //GFL_NET_StateChangeWiFiLogin();
    //    GFL_NET_ChangeInitStruct(&aGFLNetInit);
    _GFL_NET_InitAndStruct(wk,FALSE);

    //        if(wk->menulist==NULL){
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
    }
    else{

      // VCHAT���I���W�i���ɖ߂�
      _myVChatStatusOrgSet( wk );
      _userDataInfoDisp(wk);

      wk->preConnect = -1;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
      // ��l���̓��������
      FriendRequestWaitOff( wk );
    }
  }
  return seq;
}





static int WifiP2PMatch_BattleConnectInit( WIFIP2PMATCH_WORK *wk, int seq )    // WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT
{
  return seq;
}

static int WifiP2PMatch_BattleConnectWait( WIFIP2PMATCH_WORK *wk, int seq )     // WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT
{
  return seq;
}

static int WifiP2PMatch_BattleConnect( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_BATTLE_CONNECT
{
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   �J�[�\���ʒu��ύX����
 *
 * @param   act   �A�N�^�[�̃|�C���^
 * @param   x
 * @param   y
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void SetCursor_Pos( GFL_CLWK* act, int x, int y )
{
  GFL_CLACTPOS clp;

  clp.x = x;
  clp.y = y;
  GFL_CLACT_WK_SetWldPos(act ,&clp);

}

//------------------------------------------------------------------
/**
 * $brief   �{�^���������ƏI��  WIFIP2PMATCH_MODE_CHECK_AND_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_CheckAndEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( wk->timer > 0 ){
    wk->timer --;
    return seq;
  }

  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){


    WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXITING);
    wk->timer = 1;

    /*
        wk->endSeq = WIFI_P2PMATCH_END;
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
        GFL_NET_StateWifiLogout();
//*/
    }
    return seq;
}



//------------------------------------------------------------------
/**
 * $brief   ��W�̎����� WIFIP2PMATCH_MODE_SELECT_REL_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeSelectRelInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��
  WifiP2PMatchMessagePrint(wk, msg_wifilobby_007, FALSE);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_YESNO);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ��W�̎�����  WIFIP2PMATCH_MODE_SELECT_REL_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _parentModeSelectRelYesNo( WIFIP2PMATCH_WORK* wk, int seq )
{
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��
  if( WifiP2PMatchMessageEndCheck(wk) ){
    // �͂��������E�C���h�E���o��
    _yenowinCreateM2(wk);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ��W�̎�����  WIFIP2PMATCH_MODE_SELECT_REL_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _parentModeSelectRelWait( WIFIP2PMATCH_WORK* wk, int seq )
{
  int i;
  int ret;
  ret = _bmpMenu_YesNoSelectMain(wk);

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��

  if((wk->preConnect == -1) && (GFL_NET_DWC_IsNewPlayer() != -1)){  // �ڑ���������
    // ���ł�YesNoSelectMain�ŉ������ĂȂ����
    if( ret == BMPMENU_NULL ){
      BmpMenu_YesNoMenuExit( wk->pYesNoWork );
      wk->pYesNoWork = NULL;
    }
    ret = 1;  // �������ɕύX
  }
  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }else if(ret == 0){ // �͂���I�������ꍇ
    //��������
    _myStatusChange(wk,  WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    GFL_NET_StateWifiMatchEnd(TRUE);
    // �ʐM��Ԃ����ɖ߂�
    _GFL_NET_InitAndStruct(wk,FALSE);

    // ��l���̓��������
    FriendRequestWaitOff( wk );
  }
  else{  // ��������I�������ꍇ

    // ��l�������~���ēx�\��
    FriendRequestWaitOff( wk );
    FriendRequestWaitOn( wk, TRUE );
  }
  //_CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);

  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   �ҋ@��ԂɂȂ�ׂ̑I�����j���[ WIFIP2PMATCH_MODE_SELECT_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeSelectMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  _ParentModeSelectMenu(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT2);
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��
  return seq;
}

//WIFIP2PMATCH_MODE_SELECT_INIT2
static int _parentModeSelectMenuInit2( WIFIP2PMATCH_WORK *wk, int seq )
{
  WifiP2PMatchMessagePrint(wk, msg_wifilobby_006, FALSE);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_WAIT);
  return seq;
}



static void _windelandSEcall(WIFIP2PMATCH_WORK *wk)
{
  if(wk->SubListWin){
    wk->SubListWin = _BmpWinDel(wk->SubListWin);
    BmpMenuList_Exit(wk->sublw, NULL,  &wk->singleCur[_MENUTYPE_GAME]);
    BmpMenuWork_ListDelete( wk->submenulist );
    EndMessageWindowOff(wk);
  }
}


//------------------------------------------------------------------
/**
 * $brief   �ҋ@��ԂɂȂ�ׂ̑I�����j���[ WIFIP2PMATCH_MODE_SELECT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret;
  int num = PokeParty_GetBattlePokeNum(wk->pMyPoke);

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  ret = BmpMenuList_Main(wk->sublw);

  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    _windelandSEcall(wk);
    FriendRequestWaitOff(wk);
    wk->state = WIFIP2PMATCH_STATE_NONE;
    return seq;
    break;
  case WIFI_GAME_BATTLE_SINGLE_ALL:
    _windelandSEcall(wk);
    _battleCustomSelectMenu(wk,TRUE);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE2);
    return seq;
  case WIFI_GAME_VCT:
    if( !wk->pParentWork->vchatMain ){
      return seq;
    }
    else{
      _windelandSEcall(wk);
      WifiP2PMatch_CommWifiBattleStart( wk, -1 );
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
//      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW0);
    }
    break;
  case WIFI_GAME_TRADE:
    if(!_tradeNumCheck(wk)){
      wk->SubListWin = _BmpWinDel(wk->SubListWin);
      BmpMenuList_Exit(wk->sublw, NULL,  &wk->singleCur[_MENUTYPE_GAME]);
      BmpMenuWork_ListDelete( wk->submenulist );
      EndMessageWindowOff(wk);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_1013, FALSE);
      _CHANGESTATE(wk, WIFIP2PMATCH_MESSAGEEND_RETURNLIST);
      return seq;
    }
    else{
      _windelandSEcall(wk);
      WifiP2PMatch_CommWifiBattleStart( wk, -1 );
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
//      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW0);
    }
    break;
  case WIFI_GAME_TVT:
    if( !wk->pParentWork->vchatMain ){   //TVT�I��s��
      return seq;
    }
    else{
      if((FALSE == DS_SYSTEM_IsRestrictPhotoExchange()) && DS_SYSTEM_IsRunOnTwl()){  //����
        _windelandSEcall(wk);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_1046, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_TVTMESSAGE1_YESNO);
        return seq;
      }
      else{
        _windelandSEcall(wk);
        WifiP2PMatch_CommWifiBattleStart( wk, -1 );
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
//        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW0);
      }
    }
    break;
  }
  FriendRequestWaitOn( wk, TRUE );       // �����~������
  _myStatusChange(wk, WIFI_STATUS_RECRUIT, ret );  //��Ԃ�ύX
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   TVT�m�F���b�Z�[�W  WIFIP2PMATCH_MODE_TVTMESSAGE1_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _modeTVT1YesNo( WIFIP2PMATCH_WORK* wk, int seq )
{
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��
  if( WifiP2PMatchMessageEndCheck(wk) ){
    // �͂��������E�C���h�E���o��
    _yenowinCreateM2(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_TVTMESSAGE1_WAIT);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   TVT�m�F���b�Z�[�W  WIFIP2PMATCH_MODE_TVTMESSAGE1_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _modeTVT1Wait( WIFIP2PMATCH_WORK* wk, int seq )
{
  int i;
  int ret;
  ret = _bmpMenu_YesNoSelectMain(wk);

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��

  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }else if(ret == 0){ // �͂���I�������ꍇ
    WifiP2PMatch_CommWifiBattleStart( wk, -1 );
    _myStatusChange(wk, WIFI_STATUS_RECRUIT, WIFI_GAME_TVT );
    FriendRequestWaitOn( wk, TRUE );       // �����~������
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
  }
  else{  // ��������I�������ꍇ
    FriendRequestWaitOff(wk);
    wk->state = WIFIP2PMATCH_STATE_NONE;
    EndMessageWindowOff(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   TVT�m�F���b�Z�[�W  WIFIP2PMATCH_MODE_TVTMESSAGE2_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _modeTVT2YesNo( WIFIP2PMATCH_WORK* wk, int seq )
{
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��
  if( WifiP2PMatchMessageEndCheck(wk) ){
    // �͂��������E�C���h�E���o��
    _yenowinCreateM2(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_TVTMESSAGE2_WAIT);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   TVT�m�F���b�Z�[�W  WIFIP2PMATCH_MODE_TVTMESSAGE2_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _modeTVT2Wait( WIFIP2PMATCH_WORK* wk, int seq )
{
  int i;
  int ret;
  WIFI_STATUS* p_status=&wk->targetStatus;
  int friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  MCR_MOVEOBJ* p_npc;

  ret = _bmpMenu_YesNoSelectMain(wk);
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��

  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }else if(ret == 0){ // �͂���I�������ꍇ
    wk->state = WIFIP2PMATCH_STATE_MACHINE_RECV;
    if(WIFI_STATUS_GetVChatStatus(p_status) == WIFI_STATUS_GetVChatStatus(wk->pMatch)){
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_CHILD_CONNECT);
    }
    else{
      if(WIFI_STATUS_GetVChatStatus(wk->pMatch)){//������ON
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_070, FALSE);
      }
      else{
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_069, FALSE);
      }
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_VCHAT_NEGO);
    }
  }
  else{  // ��������I�������ꍇ
    wk->state = WIFIP2PMATCH_STATE_NONE;
    EndMessageWindowOff(wk);
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    wk->preConnect = -1;
    // ��l���̓��������
    FriendRequestWaitOff( wk );
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

    // NPC�����ɖ߂�
    if( p_npc != NULL ){
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    }


  }
  return seq;
}




//------------------------------------------------------------------
/**
 * $brief   ���X�g�ɖ߂�  WIFIP2PMATCH_MESSAGEEND_RETURNLIST
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int MessageEndReturnList( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if(GFL_UI_KEY_GetTrg()){
    EndMessageWindowOff(wk);
    GFL_NET_StateWifiMatchEnd(TRUE);
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    wk->preConnect = -1;
    // ��l���̓��������
    FriendRequestWaitOff( wk );
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ���X�g�ɖ߂�  WIFIP2PMATCH_RETURNLIST
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int ReturnList( WIFIP2PMATCH_WORK *wk, int seq )
{
  EndMessageWindowOff(wk);
  GFL_NET_StateWifiMatchEnd(TRUE);
  _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
  wk->preConnect = -1;
  // ��l���̓��������
  FriendRequestWaitOff( wk );
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �Q�[���̓��e�����܂����̂ŁA�e�Ƃ��čď�����  WIFIP2PMATCH_PARENT_RESTART
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentRestart( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret;

  // �G���[�`�F�b�N
  if( GFL_NET_StateIsWifiError() ){
    _errorDisp(wk);
    return seq;
  }

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��
  if( 0 !=  _checkParentNewPlayer(wk)){ // �ڑ����Ă���

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  }
  if(GFL_NET_StateIsWifiLoginMatchState()){
    GFL_NET_SetAutoErrorCheck(FALSE);
    GFL_NET_SetNoChildErrorCheck(TRUE);

    GFL_NET_SetWifiBothNet(FALSE);
    //        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
    //                      COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );
    //    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,
    //                              0,16,COMM_BRIGHTNESS_SYNC);

    wk->bRetryBattle = FALSE;

    //        CommInfoInitialize( wk->pSaveData, NULL );   //@@OO
    CommCommandWFP2PMF_MatchStartInitialize(wk);

    // ������������
    //        CommInfoSendPokeData();  //@@OO

    // �����̓G���g���[
    //        CommInfoSetEntry( GFL_NET_SystemGetCurrentID() );    //@@OO
    seq = SEQ_OUT;            //�I���V�[�P���X��
  }
  return seq;

}





//------------------------------------------------------------------
/**
 * $brief   �ҋ@��ԂɂȂ�ׂ̑I�����j���[ WIFIP2PMATCH_MODE_SUBBATTLE_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeSubSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret;

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // �p�\�R���A�j�����C��

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  ret = BmpMenuList_Main(wk->sublw);
  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT);
    break;
  default:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    _myStatusChange(wk, WIFI_STATUS_RECRUIT ,ret);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    EndMessageWindowOff(wk);
    break;
  }


  //  {
  if( wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST ){  // �����I�������Ƃ��̂݁ACANCEL�͂���
    BOOL msg_on = TRUE;
    WifiP2PMatch_CommWifiBattleStart( wk, -1 );
    // �����~������
    FriendRequestWaitOn( wk, msg_on );
  }



  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, &wk->singleCur[wk->bSingle]);
  BmpMenuWork_ListDelete( wk->submenulist );
  return seq;

}

//------------------------------------------------------------------
/**
 * $brief   �}�b�`���O�\������ WIFIP2PMATCH_MODE_MATCH_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _childModeMatchMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  int gmmno,gmmidx=0;
  u16 friendNo,status,gamemode;
  WIFI_STATUS* p_status = &wk->targetStatus;
  MCR_MOVEOBJ* p_player;
  MCR_MOVEOBJ* p_npc;
  u32 way;

  GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),FALSE);  //�ڑ��֎~

  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

  //NPC�������̕����Ɍ�����
  p_player = MCRSYS_GetMoveObjWork( wk, 0 );
  GF_ASSERT( p_player );

  way = WIFI_MCR_GetRetWay( p_player );
  WIFI_MCR_NpcPauseOn( &wk->matchroom, p_npc, way );

  status = _WifiMyStatusGet( wk, p_status );
  gamemode = _WifiMyGameModeGet( wk, p_status );

  _friendNameExpand(wk, friendNo - 1);

  if(WIFI_GAME_TRADE == gamemode){
    gmmidx = 1;
  }
  else if(WIFI_GAME_TVT == gamemode){
    gmmidx = 2;
  }
  else if(WIFI_GAME_VCT == gamemode){
    gmmidx = 3;
  }
  else if(_modeIsBattleStatus(gamemode)){
    gmmidx = 4;
  }

  switch(status){
  case WIFI_STATUS_NONE:
    gmmno = msg_wifilobby_077;
    break;
  case WIFI_STATUS_WAIT:   // �ҋ@��
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    _playerDirectConnectStart(wk);
    //    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT2);
    return seq;
  case WIFI_STATUS_RECRUIT:    // ��W��
    {
      u32 msgbuff[]={msg_wifilobby_077, msg_wifilobby_004,msg_wifilobby_080,msg_wifilobby_078,msg_wifilobby_003};
      gmmno = msgbuff[gmmidx];
    }
    break;
  case WIFI_STATUS_PLAYING:      // ���̐l�Ɛڑ���
    {
      u32 msgbuff[]={msg_wifilobby_077, msg_wifilobby_049,msg_wifilobby_079,msg_wifilobby_050,msg_wifilobby_048};
      gmmno = msgbuff[gmmidx];
    }
    break;
  case WIFI_STATUS_PLAY_AND_RECRUIT:      // ���̐l�Ɛڑ����ł���ɕ�W��
    gmmno = msg_wifilobby_077;
    break;
  default://WIFI_STATUS_UNKNOWN
    gmmno = msg_wifilobby_077;
    break;
  }

  WifiP2PMatchMessagePrint(wk, gmmno, FALSE);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT2);
  return seq;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�̏I�������āA���X�g���o���܂� WIFIP2PMATCH_MODE_MATCH_INIT2
 */
//-----------------------------------------------------------------------------
static int _childModeMatchMenuInit2( WIFIP2PMATCH_WORK *wk, int seq )
{
  MCR_MOVEOBJ* p_npc;
  u32 checkMatch;
  int friendNo;
  u32 status,gamemode;
  WIFI_STATUS* p_status = &wk->targetStatus;


  // �G���[�`�F�b�N
  if( GFL_NET_StateIsWifiError() ){
    _errorDisp(wk);
    return seq;
  }

  // �b�������Ă���F�B�i���o�[�擾
  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );


  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  {
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = _WifiMyGameModeGet( wk, p_status );

    switch(status){
    case WIFI_STATUS_WAIT:   //����w��ŌĂт����郂�[�h�ɂ���
      _ChildModeMatchMenuDisp(wk);
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_PLAYERDIRECT_INIT);
      break;
    case WIFI_STATUS_RECRUIT:
    case WIFI_STATUS_PLAY_AND_RECRUIT:
      if(_modeIsConnectStatus(gamemode)){
        _ChildModeMatchMenuDisp(wk);
        _CHANGESTATE(wk, WIFIP2PMATCH_MODE_MATCH_WAIT);
        break;
      }
      //break throw
    case WIFI_STATUS_PLAYING:
    default:
      //���b�Z�[�W�o���ďI��
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
      _CHANGESTATE(wk,WIFIP2PMATCH_MESSAGEEND_RETURNLIST);
      break;
    }
  }
  return seq;
}

static BOOL _playerDirectConnectStart( WIFIP2PMATCH_WORK *wk )
{
  int friendNo,message = 0,vchat,fst;
  u32 ret;
  int status,gamemode;
  u16 mainCursor;
  int checkMatch;
  MCR_MOVEOBJ* p_npc;
  WIFI_STATUS* p_status;

  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );

  wk->friendNo = friendNo;
  //OS_TPrintf("�_�C���N�g�ԍ��Z�b�g%d\n", friendNo);

  if(  friendNo != 0 ){
    int num = PokeParty_GetBattlePokeNum(wk->pMyPoke);

    p_status = WifiFriendMatchStatusGet( friendNo - 1 );
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = WIFI_GAME_UNIONMATCH;
    vchat = WIFI_STATUS_GetVChatStatus(p_status);

    if(WIFI_STATUS_UNKNOWN == status){
      return FALSE;
    }


    {
      //OS_TPrintf( "wifi�ڑ��� %d %d\n", friendNo - 1,status );
      if( WifiP2PMatch_CommWifiBattleStart( wk, - 1 ) ){
        WIFI_STATUS_SetVChatMac(wk->pMatch, WifiFriendMatchStatusGet( friendNo - 1 ));

        wk->cancelEnableTimer = _CANCELENABLE_TIMER;
        _commStateChange(wk,WIFI_STATUS_CALL, gamemode);
        _myStatusChange(wk, WIFI_STATUS_CALL, gamemode);  // �Ăт����ҋ@���ɂȂ�
        _friendNameExpand(wk, friendNo - 1);
        wk->DirectMacSet = friendNo;
        WifiP2PMatchMessagePrintDirect(wk,msg_wifilobby_014, FALSE);
        WifiP2PMatchMessage_TimeIconStart(wk);
        _CHANGESTATE(wk,  WIFIP2PMATCH_MODE_CONNECTWAIT);
        message = 1;
      }else{
        _friendNameExpand(wk, friendNo - 1);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
        message = 1;
      }
    }
  }
  return TRUE;
}


//WIFIP2PMATCH_MODE_CONNECTWAIT
static int _DirectConnectWait( WIFIP2PMATCH_WORK *wk, int seq  )
{
  //BTS3795-a�̑Ώ� Saito
  //���b�Z�[�W�𑁑��肷�邽�߂ɓǂ�ł��܂�
  WifiP2PMatchMessageEndCheck(wk);

  if(GFL_NET_DWC_IsNewPlayer()!=-1){
    _CHANGESTATE(wk,  WIFIP2PMATCH_MODE_FRIENDLIST);
  }

  wk->timer++;

  if( (wk->timer % (15*60))==0 ){  //�J�E���g�A�b�v
    WIFI_STATUS_SetVChatMac(wk->pMatch, WifiFriendMatchStatusGet( wk->friendNo-1 ));
    _myStatusChange(wk, WIFI_STATUS_CALL, WIFI_GAME_UNIONMATCH);  // �Ăт����ҋ@���ɂȂ�
  }

  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_068, FALSE);
    _yenowinCreateM2(wk);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CONNECTWAIT2);
  }
  return seq;
}


static int _DirectConnectWait2( WIFIP2PMATCH_WORK *wk, int seq  )
{
  int ret;
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  ret = _bmpMenu_YesNoSelectMain(wk);
  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }else if(ret == 0){ // �͂���I�������ꍇ
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_071, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
  }
  else{  // ��������I�������ꍇ
    EndMessageWindowOff(wk);
    WifiP2PMatchMessagePrint(wk,msg_wifilobby_014, FALSE);
    WifiP2PMatchMessage_TimeIconStart(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CONNECTWAIT);
  }
  return seq;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ������w�肵�Đڑ� WIFIP2PMATCH_MODE_PLAYERDIRECT_INIT
 */
//-----------------------------------------------------------------------------
static int _playerDirectInit( WIFIP2PMATCH_WORK *wk, int seq )
{


  u32 ret;
  int status,gamemode;
  int friendNo,message = 0,vchat,fst;
  u16 mainCursor;
  int checkMatch;
  MCR_MOVEOBJ* p_npc;
  WIFI_STATUS* p_status;


  ret = BmpMenuList_Main(wk->sublw);

  // �b�������Ă���F�B�i���o�[�擾
  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

  // �G�[���[�`�F�b�N
  if(GFL_NET_StateIsWifiError()){
    wk->SubListWin = _BmpWinDel(wk->SubListWin);
    BmpMenuList_Exit(wk->sublw, NULL, NULL);
    BmpMenuWork_ListDelete( wk->submenulist );

    if( p_npc != NULL ){
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    }

    _errorDisp(wk);
    return seq;
  }


  switch(ret){
  case BMPMENULIST_NULL:

    // ���肪���Ȃ��Ȃ�����
    // ����̃X�e�[�^�X���Ⴄ���̂ɕς������A
    // �\���������Č��ɖ߂�
    if( p_npc == NULL ){
      BmpMenuList_Exit( wk->sublw, NULL, NULL );
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
      message = 1;
      break;
    }else{
      p_status = WifiFriendMatchStatusGet( friendNo - 1 );
      status = _WifiMyStatusGet( wk, p_status );
      vchat = WIFI_STATUS_GetVChatStatus(p_status);
      gamemode = _WifiMyGameModeGet( wk, p_status );

      // ��Ԃ����������
      // 4�l��W�̃Q�[������Ȃ��̂ɁAVCHAT�t���O���ς������ؒf
      if((_WifiMyGameModeGet( wk, &wk->targetStatus ) != gamemode) ||
         ( (WIFI_STATUS_GetVChatStatus( &wk->targetStatus ) != vchat)) ){

        _friendNameExpand(wk, friendNo - 1);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
        message = 1;
        break;
      }
    }
    return seq;

  case BMPMENULIST_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_CANCEL1);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    break;
  default:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    if(ret == _CONNECTING){  // MAC���r�[�R���ŗ����A�e�ɂȂ�
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
      _playerDirectConnectStart(wk);
    }
    else if(ret == _INFOVIEW){
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_PERSONAL_INIT);
    }
    break;
  }


  // ��n��

  if(message==0){
    EndMessageWindowOff(wk);
  }
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, NULL);
  BmpMenuWork_ListDelete( wk->submenulist );

  // NPC�����ɖ߂�
  if( p_npc != NULL ){
    WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
  }

  // �߂邾���Ȃ�l�̏�������
  if( wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST ){

    // ���̐l�̏�������
    //    WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );

  }else{

    // �q����ɂ����Ƃ�

  }

  return seq;

}




//----------------------------------------------------------------------------
/**
 *  @brief  ���r�[�p��BGM��ݒ肷��
 */
//-----------------------------------------------------------------------------
static void WifiP2P_SetLobbyBgm( void )
{
  u32 bgm;

  // �{�C�X�`���b�g�Ȃ��ɂ���
  GFL_NET_DWC_SetVChat(FALSE);    // �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi

}

//------------------------------------------------------------------
/**
 * $brief   �Ȃ��ɍs���I�����j���[ WIFIP2PMATCH_MODE_MATCH_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _childModeMatchMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret;
  int status,gamemode;
  int friendNo,message = 0,vchat,fst;
  u16 mainCursor;
  int checkMatch;
  MCR_MOVEOBJ* p_npc;
  WIFI_STATUS* p_status=&wk->targetStatus;

  ret = BmpMenuList_Main(wk->sublw);

  checkMatch = _checkParentNewPlayer(wk);
  if( 0 !=  checkMatch ){ // �ڑ����Ă���
    ret = BMPMENULIST_CANCEL;
  }

  // �b�������Ă���F�B�i���o�[�擾
  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  wk->friendNo = friendNo;
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

  // �G���[�`�F�b�N
  if(GFL_NET_StateIsWifiError()){
    wk->SubListWin = _BmpWinDel(wk->SubListWin);
    BmpMenuList_Exit(wk->sublw, NULL, NULL);
    BmpMenuWork_ListDelete( wk->submenulist );

    if( p_npc != NULL ){
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    }

    _errorDisp(wk);
    return seq;
  }
  if(ret != BMPMENULIST_NULL){
    EndMessageWindowOff(wk);
    wk->SubListWin = _BmpWinDel(wk->SubListWin);
    BmpMenuList_Exit(wk->sublw, NULL, NULL);
    BmpMenuWork_ListDelete( wk->submenulist );
  }
  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_CANCEL1);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    break;
  default:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    gamemode = _WifiMyGameModeGet( wk, p_status );
    if(gamemode == WIFI_GAME_TVT){
      if((FALSE == DS_SYSTEM_IsRestrictPhotoExchange()) && DS_SYSTEM_IsRunOnTwl()){  //����
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_1046, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_TVTMESSAGE2_YESNO);
        return seq;
      }
    }
    if(gamemode == WIFI_GAME_TRADE){
      if(!_tradeNumCheck(wk)){
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_1013, FALSE);
        _CHANGESTATE(wk, WIFIP2PMATCH_MESSAGEEND_RETURNLIST);
        break;
      }
    }
    if(_modeIsBattleStatus(gamemode)){
      wk->battleMode = (gamemode - WIFI_GAME_BATTLE_SINGLE_ALL) / 2;
      wk->battleRule = (gamemode - WIFI_GAME_BATTLE_SINGLE_ALL) % 2;
      {
        u32 regulation = _createRegulation(wk);
        if(!_regulationCheck(wk)){
          WifiP2PMatchMessagePrint(wk, msg_wifilobby_100, FALSE);
          _CHANGESTATE(wk, WIFIP2PMATCH_MESSAGEEND_RETURNLIST);
        }
      }
    }
    wk->state = WIFIP2PMATCH_STATE_MACHINE_RECV;
    if(WIFI_STATUS_GetVChatStatus(p_status) == WIFI_STATUS_GetVChatStatus(wk->pMatch)){
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_CHILD_CONNECT);
    }
    else{
      if(WIFI_STATUS_GetVChatStatus(wk->pMatch)){//������ON
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_070, FALSE);
      }
      else{
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_069, FALSE);
      }
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_VCHAT_NEGO);
    }
    break;
  }
  // NPC�����ɖ߂�
  if( p_npc != NULL ){
    WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
  }
  return seq;
}






//------------------------------------------------------------------
/**
 * $brief   �q�@���Ȃ��ɍs�� WIFIP2PMATCH_MODE_CHILD_CONNECT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _childModeConnect( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret;
  int status,gamemode;
  int message = 0,vchat,fst;
  u16 mainCursor;
  int checkMatch;
  MCR_MOVEOBJ* p_npc;
  WIFI_STATUS* p_status=&wk->targetStatus;
  int friendNo = wk->friendNo;
  int num = PokeParty_GetBattlePokeNum(wk->pMyPoke);


  if(  wk->friendNo == 0 ){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_071, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
  }

  {
    p_status = WifiFriendMatchStatusGet( friendNo - 1 );
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = _WifiMyGameModeGet( wk, p_status );
    vchat = WIFI_STATUS_GetVChatStatus(p_status);
    if(
      (_WifiMyStatusGet( wk, &wk->targetStatus) != status) ||
      (_WifiMyGameModeGet( wk, &wk->targetStatus) != gamemode) ||
      (WIFI_STATUS_GetVChatStatus( &wk->targetStatus) != vchat) ||
      (WIFI_STATUS_UNKNOWN == status)
      ){
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
      return seq;
    }
  }

  //�������q�@�ɂȂ��đ���ɐڑ�
  fst = WifiDwc_getFriendStatus(friendNo - 1);

  {
    //OS_TPrintf( "wifi�ڑ��� %d %d\n", friendNo - 1,status );
    if( WifiP2PMatch_CommWifiBattleStart( wk, friendNo - 1 ) ){

      wk->cancelEnableTimer = _CANCELENABLE_TIMER;
      _commStateChange(wk,WIFI_STATUS_CALL, gamemode);
      _myStatusChange(wk, WIFI_STATUS_CALL, gamemode);  // �ڑ��ҋ@���ɂȂ�
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk,msg_wifilobby_014, FALSE);
      WifiP2PMatchMessage_TimeIconStart(wk);
      wk->friendNo = friendNo;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_LOOP);
    }
    else{
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    }
  }
  return seq;
}


//WIFIP2PMATCH_MODE_MATCH_LOOP
static int _childModeMatchMenuLoop( WIFIP2PMATCH_WORK *wk, int seq )
{
  int status;

  //BTS2302�̑Ώ� naigihashi
  //���b�Z�[�W�𑁑��肷�邽�߂ɓǂ�ł��܂�
  //���b�Z�[�W��`�悵�Ă��Ȃ��Ă������ɗ���ꍇ������܂�
  WifiP2PMatchMessageEndCheck(wk);

  wk->cancelEnableTimer--;
  if(wk->cancelEnableTimer < 0  ){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT);
  }
  else if(GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_FAIL ){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_DWC_GetStepMatchResult() == GFL_NET_STATE_FAIL){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_012, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if((GFL_NET_DWC_GetStepMatchResult() == GFL_NET_STATE_DISCONNECTING) || (GFL_NET_StateIsWifiDisconnect())){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED){  // ����ɐڑ�����

    WIFI_STATUS_ResetVChatMac(wk->pMatch);
    _sendMatchStatus(wk);
    // �Q�`�S�l��W�łȂ��Ƃ�
    status = _WifiMyStatusGet( wk, wk->pMatch );
    {

      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
      //    _timeWaitIconDel(wk);   timeWait����MsgWin��j�����Ă���Ƃ������Ƃ̓��b�Z�[�W�I���ł�OK��
      EndMessageWindowOff(wk);
      //    CommInfoInitialize(wk->pSaveData,NULL);   //Info������   //@@OO
      CommCommandWFP2PMF_MatchStartInitialize(wk);
      wk->timer = 30;

      GFL_NET_SetAutoErrorCheck(TRUE);
      GFL_NET_SetNoChildErrorCheck(TRUE);
    }
  }
  return seq;
}

//-------------------------------------VCT�̃L�����Z���@�\���}篎���
//-------------------------------------VCT�̃L�����Z���@�\���}篎��O��

//------------------------------------------------------------------
/**
 * $brief   B�L�����Z��  WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelYesNoVCT( WIFIP2PMATCH_WORK *wk, int seq )
{
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   B�L�����Z��  WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelWaitVCT( WIFIP2PMATCH_WORK *wk, int seq )
{
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   DWC�ؒf WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_CancelEnableWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  _myVChatStatusOrgSet(wk);
  _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
  GFL_NET_StateWifiMatchEnd(TRUE);
  wk->preConnect = -1;
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  // ��l���̓��������
  FriendRequestWaitOff( wk );
  EndMessageWindowOff(wk);
  return seq;
}


//------------------------------------------

//------------------------------------------------------------------
/**
 * $brief   B�L�����Z��  WIFIP2PMATCH_MODE_BCANCEL_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelYesNo( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT
{
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   B�L�����Z��  WIFIP2PMATCH_MODE_BCANCEL_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelWait( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT
{
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   �e�@��t�܂��B�q�@���牞�傪���������Ƃ�m�点�� WIFIP2PMATCH_MODE_CALL_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeCallMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  int mySt;
  int targetSt;
  WIFI_STATUS* p_status;
  int myvchat;


  if(PMSND_CheckPlaySE_byPlayerID( SE_CALL_SIGN_PLAYER )){
    return seq;
  }

  GFL_NET_SetAutoErrorCheck(TRUE);
  GFL_NET_SetNoChildErrorCheck(TRUE);
  
  p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
  mySt = _WifiMyStatusGet( wk, wk->pMatch );
  targetSt = _WifiMyStatusGet( wk,p_status );
  myvchat = WIFI_STATUS_GetVChatStatus( wk->pMatch );
  wk->friendNo = GFL_NET_DWC_GetFriendIndex()+1;
  //OS_TPrintf("�_�C���N�g%d\n", wk->friendNo);
  if(wk->DirectMacSet==0){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_008, FALSE);
  }
  CommCommandWFP2PMF_MatchStartInitialize(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  wk->timer = 30;
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �R�l�N�V�����͂낤�Ƃ��Ă�����Ԓ��̃G���[�\��
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static BOOL _connectingErrFunc(WIFIP2PMATCH_WORK *wk)
{
  if(GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_TIMEOUT){
    _friendNameExpand(wk, wk->preConnect);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
  }
  else{
    return FALSE;
  }
  wk->bRetryBattle = FALSE;
  return TRUE;
}

//------------------------------------------------------------------
/**
 * $brief   �Ȃ���ׂ��X�e�[�g�m�F  WIFIP2PMATCH_MODE_CALL_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMenuYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  //BTS2302�̑Ώ� naigihashi
  //���b�Z�[�W�𑁑��肷�邽�߂ɓǂ�ł��܂�
  ////���b�Z�[�W��`�悵�Ă��Ȃ��Ă������ɗ���ꍇ������܂�
  WifiP2PMatchMessageEndCheck(wk);

  if((GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_DISCONNECTING) || GFL_NET_StateIsWifiDisconnect() ){

    if(wk->bRetryBattle){
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_065, FALSE);
    }
    else{
      _friendNameExpand(wk, wk->preConnect);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    }
    wk->bRetryBattle = FALSE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(_connectingErrFunc(wk)){
  }
  else{

    if(wk->timer > 1){
      wk->timer--;
    }

    if(wk->timer==0){
      if(GFL_NET_HANDLE_IsNegotiation(GFL_NET_HANDLE_GetCurrentHandle())){
        GFL_STD_MemClear(&wk->matchGameMode,sizeof(wk->matchGameMode));
        GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_CHECK2,WB_NET_WIFICLUB);
        wk->bRetryBattle = FALSE;
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_066, FALSE);
        WifiP2PMatchMessage_TimeIconStart(wk);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_SEND);
      }
    }
    else{
      if(wk->timer == 1){
        if(GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())!=0){
          if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
            wk->timer = 0;
          }
        }
        else if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
          if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
            wk->timer = 0;
          }
        }
      }
    }
  }
  return seq;
}




//------------------------------------------------------------------
/**
 * $brief   MySTATUS����M CNM_WFP2PMF_MYSTATUS
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvMyStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;
  MYSTATUS* pTargetSt;
  const u16 *pRecvData = pData;


  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  //�O�C�P�C�Q�C�R���ʐM�p�ɓ���Ă��ǂ�
  pTargetSt = GAMEDATA_GetMyStatusPlayer(wk->pGameData,netID);  //netID�ő��
  MyStatus_Copy(pData,pTargetSt);

  {
    ETC_SAVE_WORK * pETC = SaveData_GetEtc( wk->pSaveData );
    EtcSave_SetAcquaintance(pETC, MyStatus_GetID(pTargetSt));
  }
}



//------------------------------------------------------------------
/**
 * $brief   PokeParty����M CNM_WFP2PMF_POPEPARTY
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvPokeParty(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{

  //���łɎ�M�͊���

}

//------------------------------------------------------------------
/**
 * $brief   PokeParty����M CNM_WFP2PMF_POPEPARTY
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
void WifiP2PMatchRecvVctOff(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  wk->VChatModeOff=TRUE;
}


//------------------------------------------------------------------
/**
 * $brief   PokeParty�̎�M���[�N��Ԃ� CNM_WFP2PMF_POPEPARTY
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

u8* WifiP2PMatchGetPokePartyWork(int netID, void* pWk, int size)
{
  WIFIP2PMATCH_WORK *wk = pWk;
  return (void*)wk->pParentWork->pPokeParty[netID];
}

//------------------------------------------------------------------
/**
 * $brief   ���ڐڑ����̃t���[�R�}���h����M CNM_WFP2PMF_DIRECT_COMMAND
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvDirectMode(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;
  MYSTATUS* pTargetSt;
  const u8 *pChangeStateNo = pData;


  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }

  if(wk->seq == WIFIP2PMATCH_PLAYERDIRECT_WAIT){
    NET_PRINT("_CHANGESTATE %d\n",pChangeStateNo[0]);
    _CHANGESTATE(wk, pChangeStateNo[0]);
  }
  else{
    GF_ASSERT(0);  // �s���R�}���h��M�͐ؒf
    _CHANGESTATE(wk, WIFIP2PMATCH_MODE_DISCONNECT);
  }
  //  wk->matchGameMode[netID] = pRecvData[0];
}

//------------------------------------------------------------------
/**
 * $brief   ���M�����[�V��������M CNM_WFP2PMF_DIRECT_COMMAND
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvBattleRegulation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;
  MYSTATUS* pTargetSt;
  const u32 *pRegNo = pData;


  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  //  wk->matchRegulation = pRegNo[0];
  _convertRegulation(wk,pRegNo[0]);


}


//------------------------------------------------------------------
/**
 * $brief   �Ȃ���ׂ��X�e�[�g����M CNM_WFP2PMF_STATUS
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvGameStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;
  const u16 *pRecvData = pData;

  wk->matchGameMode[netID] = pRecvData[0];
  wk->pParentWork->VCTOn[netID] = pRecvData[1];
}

//
//------------------------------------------------------------------
/**
 * $brief   �Ȃ���ׂ��X�e�[�g�𑗐M  WIFIP2PMATCH_MODE_CALL_SEND
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeCallMenuSend( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(_connectingErrFunc(wk)){
  }
  else if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_GAME_CHECK2, WB_NET_WIFICLUB)){
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_MYSTATUS, MyStatus_GetWorkSize(), GAMEDATA_GetMyStatus(wk->pGameData))){
      EndMessageWindowOff(wk);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_CHECK_D);
    }
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �Ȃ���ׂ��X�e�[�g�𑗐M  WIFIP2PMATCH_MODE_CALL_CHECK_D
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeCallMenuSendD( WIFIP2PMATCH_WORK *wk, int seq )
{
  u16 gamemode[2];
  u16 status = _WifiMyStatusGet( wk, wk->pMatch );
  gamemode[0] = _WifiMyGameModeGet( wk, wk->pMatch );

  if(wk->vchatrev){  //�}�V���ł̎b��VCT�t���O
    gamemode[1] = wk->vchatrev-1;
    wk->vchatrev=0;
  }
  else{
    gamemode[1] = wk->pParentWork->vchatMain;
  }
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_STATUS, sizeof(u16)*2, gamemode)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_CHECK);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   �Ȃ���ׂ��X�e�[�g���  WIFIP2PMATCH_MODE_CALL_CHECK
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMenuCheck( WIFIP2PMATCH_WORK *wk, int seq )
{
  int id=0;

  if(GFL_NET_IsParentMachine()){  //���葤��ID���݂�
    id = 1;
  }
  if(_connectingErrFunc(wk)){
  }
  else if(WIFI_GAME_NONE != wk->matchGameMode[id]){  // �����Ă���
    u16 gamemode = _WifiMyGameModeGet( wk, wk->pMatch );
    if(wk->matchGameMode[id] == gamemode){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_START,WB_NET_WIFICLUB);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MYSTATUS_WAIT);
    }
    else{  // �قȂ�X�e�[�g��I�������ꍇ
      _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    }
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   �^�C�~���O�R�}���h�҂� WIFIP2PMATCH_MODE_MYSTATUS_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMyStatusWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int status;

  if(_connectingErrFunc(wk)){
  }
  else if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_GAME_START, WB_NET_WIFICLUB)){
    GFL_NET_SetAutoErrorCheck(TRUE);
    GFL_NET_SetNoChildErrorCheck(TRUE);

    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_START2,WB_NET_WIFICLUB);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_WAIT);
  }
  return seq;
}


static BOOL _parent_MsgEndCheck( WIFIP2PMATCH_WORK *wk )
{
  if(WifiP2PMatchMessageEndCheck(wk)){
    return TRUE;
  }

  if( !wk->MsgWin ){
    return TRUE;
  }

  return FALSE;
}

//------------------------------------------------------------------
/**
 * $brief   �^�C�~���O�R�}���h�҂� WIFIP2PMATCH_MODE_CALL_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 status,gamemode;
  int checkFriend[GFL_NET_MACHINE_MAX];
  WIFI_STATUS* p_status;

  if(_connectingErrFunc(wk)){
  }
  else if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_GAME_START2,WB_NET_WIFICLUB)
          && (_parent_MsgEndCheck( wk ) == TRUE) ){   // ���b�Z�[�W�̏I�����҂悤�ɕύX 08.06.01  tomoya
    GFL_NET_DWC_FriendAutoInputCheck( WifiList_GetDwcDataPtr(GAMEDATA_GetWiFiList(wk->pGameData), 0));

    EndMessageWindowOff(wk);

    p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = _WifiMyGameModeGet( wk, p_status );

    if(gamemode == WIFI_GAME_UNIONMATCH){ //�q�������B�q������Ɋe�Q�[���I��
      _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_INIT1);
    }
    else{
      /// ������ɃQ�[���ɔ��
      status = WIFI_STATUS_PLAYING;
      _myStatusChange(wk, status,gamemode);  // �ڑ����ɂȂ�
      _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERMACHINE_INIT1);
    }
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   ���C�����j���[�̖߂�ۂ̏�����
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_MainReturn( WIFIP2PMATCH_WORK *wk, int seq )
{
  GFL_BG_ClearFrame(  GFL_BG_FRAME3_M);
  wk->mainCur = 0;
  _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �l�f�[�^�\�������� WIFIP2PMATCH_MODE_PERSONAL_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _personalDataInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_PERSONAL_WAIT);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �l�f�[�^�\�� WIFIP2PMATCH_MODE_PERSONAL_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _personalDataWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if( 0 !=  _checkParentConnect(wk)){ // �ڑ����Ă���
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_PERSONAL_END);
  }
  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_PERSONAL_END);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �l�f�[�^�\������� WIFIP2PMATCH_MODE_PERSONAL_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _personalDataEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i,x;
  int num = 0;

  EndMessageWindowOff(wk);


  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �I���m�F���b�Z�[�W  WIFIP2PMATCH_MODE_EXIT_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _exitYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( WifiP2PMatchMessageEndCheck(wk) ){
    // �͂��������E�C���h�E���o��
    _yenowinCreateM2(wk);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_WAIT);
  }


  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �I���m�F���b�Z�[�W  WIFIP2PMATCH_MODE_EXIT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _exitWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret;

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  
  ret = _bmpMenu_YesNoSelectMain(wk);

  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }else if(ret == 0){ // �͂���I�������ꍇ
    wk->pYesNoWork=NULL;
    WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXITING);
    wk->timer = 1;
  }
  else{  // ��������I�������ꍇ
    wk->pYesNoWork=NULL;
    EndMessageWindowOff(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    // ��l���̓��������
    FriendRequestWaitOff( wk );
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �I���m�F���b�Z�[�W  WIFIP2PMATCH_MODE_EXITING
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _exitExiting( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if(wk->timer == 1){
    wk->timer = 0;
    GFL_NET_Exit(NULL);
  }
  if(!GFL_NET_IsInit()){
    //OS_TPrintf("�ؒf�������Ƀt�����h�R�[�h���l�߂�\n");
    WifiList_FormUpData(wk->pList);  // �ؒf�������Ƀt�����h�R�[�h���l�߂�
    WifiP2PMatchMessagePrint(wk, dwc_message_0012, TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_END);
    wk->timer = 30;
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �I������  WIFIP2PMATCH_MODE_EXIT_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------


static int _exitEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  wk->timer--;
  if(wk->timer==0){
    wk->endSeq = WIFI_GAME_NONE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
    EndMessageWindowOff(wk);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   ������x�ΐ킷�邩���� WIFIP2PMATCH_NEXTBATTLE_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _nextBattleYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_BATTLE_END,WB_NET_WIFICLUB)==FALSE){
    return seq;
  }
  // �ʐM�������ɓd����؂�ꂽ��A�����Ɠ����҂����Ă��܂��̂ŁA�ʐM������ɃI�[�g�G���[�`�F�b�N��
  // �͂���
  GFL_NET_SetAutoErrorCheck(FALSE);
  GFL_NET_SetNoChildErrorCheck(TRUE);

  if( WifiP2PMatchMessageEndCheck(wk) ){
    _yenowinCreateM2(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_NEXTBATTLE_WAIT);
  }

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ������x�ΐ킷�邩���� WIFIP2PMATCH_NEXTBATTLE_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _nextBattleWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret;

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }

  ret = _bmpMenu_YesNoSelectMain(wk);
  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }else if(ret == 0){ // �͂���I�������ꍇ
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_073, FALSE);
    WifiP2PMatchMessage_TimeIconStart(wk);
    wk->bRetryBattle = TRUE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
    wk->timer = 30;
  }
  else{  // ��������I�������ꍇ
    EndMessageWindowOff(wk);
    GFL_NET_StateWifiMatchEnd(TRUE);
    wk->timer = _RECONECTING_WAIT_TIME;
    _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
  }
  //        wk->pMatch = GFL_NET_StateGetMatchWork();  //@@
  //      wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   VCAHT��ύX���Ă��������ǂ������� WIFIP2PMATCH_MODE_VCHAT_NEGO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatNegoCheck( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( WifiP2PMatchMessageEndCheck(wk) ){
    _yenowinCreateM2(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCAHT��ύX���Ă��������ǂ������� WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatNegoWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret,status;
  int friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  MCR_MOVEOBJ* p_npc;

  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );
  
  GFL_FONTSYS_SetDefaultColor();
  ret = _bmpMenu_YesNoSelectMain(wk);
  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }else if(ret == 0){ // �͂���I�������ꍇ
    // �ڑ��J�n
    //VCT���]
    WIFI_STATUS_SetVChatStatus(wk->pMatch, 1-WIFI_STATUS_GetVChatStatus(wk->pMatch));
    WIFI_STATUS_SetUseVChat(wk->pMatch, WIFI_STATUS_GetVChatStatus(wk->pMatch));
    wk->vchatrev = (1 - wk->pParentWork->vchatMain) + 1;  //���]�t���O���i�[
    _CHANGESTATE(wk, WIFIP2PMATCH_MODE_CHILD_CONNECT);
  }
  else{
    wk->state = WIFIP2PMATCH_STATE_NONE;
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    wk->preConnect = -1;
    // NPC�����ɖ߂�
    if( p_npc != NULL ){
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    }
    // VCHAT���ɖ߂�
    _myVChatStatusOrgSet( wk );
    _userDataInfoDisp(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  }
  EndMessageWindowOff(wk);
  return seq;
}


// WIFIP2PMATCH_MODE_END_WAIT
//------------------------------------------------------------------
/**
 * $brief   �I��
 *
 * @param   wk
 * @param   seq
 *
 * @retval  int
 */
//------------------------------------------------------------------
static int  WifiP2PMatch_EndWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(!GFL_NET_IsInit()){
    //        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
    //                      COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
    //  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
    //                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );
    //    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,
    //                              0,16,COMM_BRIGHTNESS_SYNC);
    seq = SEQ_OUT;            //�I���V�[�P���X��
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _myStatusChange(WIFIP2PMATCH_WORK *wk, int status,int gamemode)
{
  if(wk->pMatch==NULL){  //0707
    return;
  }
  if((status == WIFI_STATUS_WAIT) && (gamemode==WIFI_GAME_LOGIN_WAIT)){
    WIFI_STATUS_ResetVChatMac(wk->pMatch);
  }
  if((status == WIFI_STATUS_PLAYING) && (gamemode<WIFI_GAME_UNIONMATCH)){
    //�o�g����ΐ�ȊO�ł��X�V����ׂɂ����ɂ���
    WifiList_SetLastPlayDate( wk->pList, GFL_NET_DWC_GetFriendIndex());
  }
  _myStatusChange_not_send( wk, status, gamemode );
  _sendMatchStatus(wk);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����̃X�e�[�^�X�ݒ�    �������M�͂��Ȃ�
 *
 *  @param  wk
 *  @param  status
 */
//-----------------------------------------------------------------------------
static void _myStatusChange_not_send(WIFIP2PMATCH_WORK *wk, int status,int gamemode)
{
  int org_status,org_mode;

  if(wk->pMatch==NULL){  //0707
    return;
  }

  org_status = _WifiMyStatusGet( wk, wk->pMatch );
  org_mode = _WifiMyGameModeGet(wk, wk->pMatch);

  if((org_status != status) || (org_mode != gamemode)){
    _commStateChange(wk,status,gamemode);

    WIFI_STATUS_SetWifiStatus(wk->pMatch,status);
    WIFI_STATUS_SetGameMode(wk->pMatch,gamemode);
    if(gamemode == WIFI_GAME_TVT && status == WIFI_STATUS_PLAYING){
    }
    else if(status == WIFI_STATUS_PLAYING ){
      // �{�����[���𗎂�
      if(gamemode != WIFI_GAME_UNIONMATCH){
     //   _changeBGMVol( wk, _VOL_DEFAULT/3 );
      }
    }
    else{
      //  �{�����[����߂�
      _changeBGMVol( wk, _VOL_DEFAULT );
    }
  }
  _userDataInfoDisp(wk);
}

//------------------------------------------------------------------
/**
 * $brief   VCHAT�t���O�̐؂�ւ�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
#if 0
static BOOL _myVChatStatusToggle(WIFIP2PMATCH_WORK *wk)
{
  WIFI_STATUS_SetVChatStatus(wk->pMatch, 1 - WIFI_STATUS_GetVChatStatus( wk->pMatch ));
  WIFI_STATUS_SetUseVChat(wk->pMatch, WIFI_STATUS_GetVChatStatus(wk->pMatch));
  _sendMatchStatus(wk);
  return WIFI_STATUS_GetVChatStatus( wk->pMatch );
}
#endif
//------------------------------------------------------------------
/**
 * $brief   VCHAT�t���O�̐؂�ւ�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _myVChatStatusToggleOrg(WIFIP2PMATCH_WORK *wk)
{
  //�{�����Ђ�����Ԃ�
  wk->pParentWork->vchatMain = 1 - wk->pParentWork->vchatMain;
  WIFI_STATUS_SetVChatStatus(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetUseVChat(wk->pMatch, wk->pParentWork->vchatMain);
  _sendMatchStatus(wk);
  return WIFI_STATUS_GetVChatStatus( wk->pMatch );
}

//------------------------------------------------------------------
/**
 * $brief   VCHAT�t���O���I���W�i���ɂ��ǂ�  ==.> VCHAT�X�e�[�^�X���I���W�i���ɖ߂�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _myVChatStatusOrgSet(WIFIP2PMATCH_WORK *wk)
{
  _changeBGMVol( wk, _VOL_DEFAULT );

  WIFI_STATUS_SetVChatStatus(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetUseVChat(wk->pMatch, wk->pParentWork->vchatMain);
  _sendMatchStatus(wk);

  return WIFI_STATUS_GetVChatStatus( wk->pMatch );
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���[�U�[�f�[�^�����\������
 *
 *  @param  wk      �V�X�e�����[�N
 *  @param  friendNo  �F�B�ԍ�
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
  MCR_MOVEOBJ* p_obj;

  MCVSys_UserDispOn( wk, friendNo, heapID );
  p_obj = MCRSYS_GetMoveObjWork( wk, friendNo );
  if( p_obj ){
    WIFI_MCR_CursorOn( &wk->matchroom, p_obj );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�U�[�f�[�^�����\�������I��
 *
 *  @param  wk      �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff( WIFIP2PMATCH_WORK *wk, u32 heapID )
{

  _TrainerOAMFree( wk );
  _TouchResExit(wk);
  _UnderScreenReload( wk );

  MCVSys_UserDispOff( wk );
  MCRSYS_ReloadShadow(wk);
  WIFI_MCR_CursorOff( &wk->matchroom );

  // �ĕ`��
  MCVSys_ReWrite( wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\�����̐l�Ȃ狭���I�ɕ\���I��
 *
 *  @param  wk        �V�X�e�����[�N
 *  @param  target_friend �^�[�Q�b�g�̐l���̃C���f�b�N�X
 *  @param  heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff_Target( WIFIP2PMATCH_WORK *wk, u32 target_friend, u32 heapID )
{
  if( MCVSys_UserDispGetFriend( wk ) == target_friend ){
    WifiP2PMatch_UserDispOff( wk, heapID );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����ŃA�N�Z�X�����Ƃ��̃��[�U�[�f�[�^�����\������
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOn_MyAcces( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
  MCVSys_UserDispOn( wk, friendNo, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����ŃA�N�Z�X�����Ƃ��̃��[�U�[�f�[�^�����\�������I��
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff_MyAcces( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  MCVSys_UserDispOff( wk );

  // �ĕ`��
  MCVSys_ReWrite( wk, HEAPID_WIFIP2PMATCH );
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���삵�Ă��邩�`�F�b�N
 *
 *  @param  wk  �V�X�e�����[�N
 *
 *  @retval TRUE  ���쒆
 *  @retval FALSE �񓮍쒆
 */
//-----------------------------------------------------------------------------
static BOOL MCVSys_MoveCheck( const WIFIP2PMATCH_WORK *wk )
{
  if( wk->view.p_bttnman == NULL ){
    return FALSE;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�f�[�^�r���[�A�[������
 *
 *  @param  wk      �V�X�e�����[�N
 *  @param  p_handle  �n���h��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void MCVSys_Init( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
  static const GFL_UI_TP_HITTBL bttndata[ WCR_MAPDATA_1BLOCKOBJNUM+1 ] = {
    { 0,    47,   0,    119 },
    { 48,   95,   0,    119 },
    { 96,   143,  0,    119 },
    { 144,  191,  0,    119 },

    { 0,    47,   128,  255 },
    { 48,   95,   128,  255 },
    { 96,   143,  128,  255 },
    { 144,  191,  128,  255 },
    {GFL_UI_TP_HIT_END,0,0,0},     //�I���f�[�^
  };

  GFL_STD_MemFill( &wk->view, 0, sizeof(WIFIP2PMATCH_VIEW) );

  // �\���ݒ�
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );

  // �{�^���f�[�^������
  wk->view.p_bttnman = GFL_BMN_Create( bttndata, MCVSys_BttnCallBack, wk, heapID );

  // ���[�h�Z�b�g������
  wk->view.p_wordset = WORDSET_Create( heapID );

  // �O���t�B�b�N�f�[�^�Ǎ���
  MCVSys_GraphicSet( wk, p_handle, heapID );


  // �Ƃ肠�����X�V
  wk->view.bttn_allchg = TRUE;
  MCVSys_BackDraw( wk );
  MCVSys_BttnDraw( wk );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�f�[�^�r���[�A�[�j��
 *
 *  @param  wk      �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_Exit( WIFIP2PMATCH_WORK *wk )
{
  if( MCVSys_MoveCheck( wk ) == FALSE ){
    return;
  }

  // �O���t�B�b�N�͂�
  MCVSys_GraphicDel( wk );

  // �{�^���}�l�[�W���j��
  GFL_BMN_Delete( wk->view.p_bttnman );
  wk->view.p_bttnman = NULL;

  // ���[�h�Z�b�g�j��
  WORDSET_Delete( wk->view.p_wordset );
  wk->view.p_wordset = NULL;

  // BG�Q�A�R�͔�\���ɂ��Ă���
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�f�[�^�r���[�A�[�A�b�v�f�[�g
 *
 *  @param  wk      �V�X�e�����[�N
 *
 *  @retval ������Ă���F�B�ԍ�  + 1
 *  @retval 0 ������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_Updata( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  u32 map_param, map_param2;
  u32 oambttn_ret;
  BOOL userd_end;

  // ���݃t���[���ݒ�
  map_param = WIFI_MCR_GetPlayerOnMapParam( &wk->matchroom );

  // �I�u�W�F�N�g�̈ʒu���Ɖ������Ȃ��Ƃ����Ƃ����t���[���̈ʒu���擾�ł��Ȃ�
  map_param2 = WIFI_MCR_GetPlayerOnUnderMapParam( &wk->matchroom );
  if( (map_param2 >= MCR_MAPPM_MAP00) &&
      (map_param2 <= MCR_MAPPM_MAP03) ){
    map_param = map_param2;
  }

  if( (map_param >= MCR_MAPPM_MAP00) &&
      (map_param <= MCR_MAPPM_MAP03) ){
    if( wk->view.frame_no != (map_param - MCR_MAPPM_MAP00) ){
      wk->view.frame_no = (map_param - MCR_MAPPM_MAP00);

      // �w�i�J���[�ύX
      MCVSys_BackDraw( wk );

      // �{�^��������������
      wk->view.button_on = TRUE;
      wk->view.bttn_allchg = TRUE;  // �{�^�������ύX
    }
  }


  // ���[�U�[�f�[�^�\������
  if( wk->view.user_disp == MCV_USERDISP_INIT ){
    wk->view.user_disp = MCV_USERDISP_ON;
    MCVSys_UserDispDraw( wk, heapID );
  }



  if( wk->view.user_disp == MCV_USERDISP_OFF ){

    _TrainerOAMFree( wk );
    _TouchResExit(wk);
    _UnderScreenReload( wk );


    // �{�^�����C��
    GFL_BMN_Main( wk->view.p_bttnman );

    // �{�^���A�j������
    MCVSys_BttnAnmMan( wk );

    // �\�����C��
    if( wk->view.button_on == TRUE ){
      MCVSys_BttnDraw( wk );
      wk->view.button_on = FALSE;
    }
  }

  // ��������A�^�b�`�p�l���ɐG��Ă����烆�[�U�[�\����OFF����
  if( (wk->view.user_disp == MCV_USERDISP_ON) ||
      (wk->view.user_disp == MCV_USERDISP_ONEX) ){

    // �������OAM�{�^�����C��
    //oambttn_ret = MCVSys_OamBttnMain( wk );

    //  USERD�I���`�F�b�N
    userd_end = MCVSys_UserDispEndCheck( wk );

    // �Ȃɂ��L�[���������A�u���ǂ�v����������I������
    if( userd_end == TRUE ){
      wk->view.bttn_chg_friendNo = wk->view.touch_friendNo; // ���̗F�B�̃{�^�����X�V���Ă��炤
      wk->view.touch_friendNo = 0;
      wk->view.touch_frame = 0;
      wk->view.button_on = TRUE;
      wk->view.bttn_allchg = TRUE;
      wk->view.user_disp = MCV_USERDISP_OFF;
    }
  }

  return wk->view.touch_friendNo;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�f�[�^�r���[�A�[�A�b�v�f�[�g  �{�^���̕\���̂�
 *
 *  @param  wk  �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_UpdataBttn( WIFIP2PMATCH_WORK *wk )
{
  if( wk->view.user_disp == MCV_USERDISP_OFF ){

    // �\�����C��
    if( wk->view.button_on == TRUE ){
      MCVSys_BttnDraw( wk );
      wk->view.button_on = FALSE;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�U�[�f�B�X�v���C���I�����邩�`�F�b�N
 *
 *  @param  wk      ���[�N
 *  @param  oambttn_ret OAM�{�^�����C���߂�l
 *
 *  @retval TRUE  �I��
 *  @retval FALSE ���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL MCVSys_UserDispEndCheck( WIFIP2PMATCH_WORK *wk  )
{
  // �����\�����Ă���̂ŏI�����Ȃ�
  if( (wk->view.user_disp == MCV_USERDISP_ONEX) ){
    return FALSE;
  }

  if(wk->pTouchWork){
    TOUCHBAR_Main(wk->pTouchWork);
    switch( TOUCHBAR_GetTrg(wk->pTouchWork )){
    case TOUCHBAR_ICON_RETURN:
      return TRUE;
    }
  }
  //  �ړ���cont�{�^���̓g���K�[
  if( (GFL_UI_KEY_GetCont() & (PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_KEY_UP|PAD_KEY_DOWN)) ||
      (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_X))
      ){

    //  if( (GFL_UI_KEY_GetCont() & (PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_KEY_UP|PAD_KEY_DOWN)) ){

    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���ڍו\�����Ă���l�̗F�BIDX���擾
 *
 *  @param  cp_wk ���[�N
 *
 *  @return �F�B�C���f�b�N�X  (0�Ȃ�\�����ĂȂ�)
 */
//-----------------------------------------------------------------------------
static u32  MCVSys_UserDispGetFriend( const WIFIP2PMATCH_WORK* cp_wk )
{
  return cp_wk->view.touch_friendNo;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�U�[�f�[�^�\��
 *
 *  @param  wk      ���[�N
 *  @param  friendNo  �F�B�ԍ�
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
  if( MCVSys_MoveCheck(wk) == TRUE ){
    wk->view.touch_friendNo = friendNo;
    wk->view.touch_frame  = 2;
    wk->view.user_disp    = MCV_USERDISP_ONEX;
    //    wk->view.user_dispno = WF_USERDISPTYPE_NRML;
    MCVSys_UserDispDraw( wk, heapID );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�U�[�f�[�^OFF
 *
 *  @param  wk      ���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispOff( WIFIP2PMATCH_WORK *wk )
{
  if( MCVSys_MoveCheck(wk) == TRUE ){
    wk->view.touch_friendNo = 0;
    wk->view.touch_frame = 0;
    wk->view.user_disp    = MCV_USERDISP_OFF;
    wk->view.button_on    = TRUE;
    wk->view.bttn_allchg = TRUE;
    //    MCVSys_OamBttnOff( wk );  // �{�^��OFF
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �{�^���C�x���g�R�[���o�b�N
 *
 *  @param  bttnid    �{�^��ID
 *  @param  event   �C�x���g���
 *  @param  p_work    ���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  WIFIP2PMATCH_WORK *wk = p_work;
  u32 friendNo;

  // �F�B�ԍ��擾
  friendNo = (wk->view.frame_no * WCR_MAPDATA_1BLOCKOBJNUM) + bttnid;
  friendNo ++;  // �������O������

  // ���쒆���`�F�b�N
  if( wk->view.bttnfriendNo[ friendNo-1 ] != MCV_BTTN_FRIEND_TYPE_IN ){
    // ���삵�Ă��Ȃ��̂ŉ������Ȃ�
    return ;
  }
  // ���łɃ{�^�����쒆�Ȃ̂Ŕ������Ȃ�
  if( wk->view.touch_friendNo != 0 ){
    return ;
  }

  switch( event ){
  case GFL_BMN_EVENT_TOUCH:   ///< �G�ꂽ�u��
    wk->view.touch_friendNo = friendNo;
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    break;
  default:
    break;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���A�j�����C��
 *
 *	@param	wk		�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnAnmMan( WIFIP2PMATCH_WORK *wk )
{
  static const u8 BttnAnmFrame[ MCV_BUTTON_FRAME_NUM ] = {
    1, 1,1,1, 2,2,2,2,
    1, 1,1,1, 2,2,2,2,
  };

  // ���삵�Ă��邩�`�F�b�N
  if( wk->view.touch_friendNo == 0 ){
    return ;
  }

  // ����
  wk->view.touch_frame = BttnAnmFrame[wk->view.button_count];
  wk->view.button_count ++;
  wk->view.button_on = TRUE;
  // �I���`�F�b�N
  if( wk->view.button_count >= MCV_BUTTON_FRAME_NUM ){
    wk->view.button_count = 0;
    wk->view.touch_frame = 0;
    wk->view.user_disp = MCV_USERDISP_INIT;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�N�f�[�^�ݒ�
 *
 *  @param  wk      �V�X�e�����[�N
 *  @param  p_handle  �A�[�J�C�u�n���h��
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicSet( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
  int i, j;
  int x, y;

  // BG�ݒ�
  // �p���b�g�]��
  //  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_btm_NCLR,  // �w�i�p
  //                                  PALTYPE_SUB_BG, MCV_PAL_BACK*32, MCV_PAL_BACK_NUM*32, heapID );
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NCLR, // �{�^���p
                                    PALTYPE_SUB_BG, MCV_PAL_BTTN*32, MCV_PAL_BTTN_NUM*32, heapID );

  // �L�����N�^�]��
  wk->bgchrSubBack = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_wifip2pmatch_wf_match_btm_NCGR,
                                                                  GFL_BG_FRAME0_S, 0, FALSE, heapID );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NCGR,
                                        GFL_BG_FRAME2_S, MCV_CGX_BTTN2, 0, FALSE, heapID );

  // �X�N���[���Ǎ���or�]��
  // �w�i�̓L�����N�^�ʒu������Ă���̂ŃX�N���[���f�[�^������������
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, NARC_wifip2pmatch_wf_match_btm_NSCR,
                                          GFL_BG_FRAME0_S, 0,  GFL_ARCUTIL_TRANSINFO_GetPos(wk->bgchrSubBack),
                                          0, FALSE, heapID );

  // �{�^���X�N���[���Ǎ���
  wk->view.p_bttnbuff = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NSCR, FALSE, &wk->view.p_bttnscrn, heapID );
  MCVSys_GraphicScrnCGOfsChange( wk->view.p_bttnscrn, MCV_CGX_BTTN2 );

  // ���[�U�[�X�N���[���Ǎ���
  for( i=0; i < 1; i++ ){
    wk->view.p_userbuff[i] = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_result00_NSCR+i, FALSE, &wk->view.p_userscrn[i], heapID );
    MCVSys_GraphicScrnCGOfsChange( wk->view.p_userscrn[i], MCV_CGX_BTTN2 );
  }

  // �_�~�[�X�N���[���ǂݍ���
  //  wk->view.p_useretcbuff = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_etc_NSCR, FALSE, &wk->view.p_useretcscrn, heapID );
  //  MCVSys_GraphicScrnCGOfsChange( wk->view.p_useretcscrn, MCV_CGX_BTTN2 );



  // �t�H���g�p���b�g�Ǎ���
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                MCV_SYSFONT_PAL*32, 0x20, heapID);
  //  TalkFontPaletteLoad( PALTYPE_SUB_BG, MCV_SYSFONT_PAL*32, heapID );
  //GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , 13*0x20, 16*2, heapID );

  // �r�b�g�}�b�v�쐬
  for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){
    x = i/4;  // �z�u��2*4
    y = i%4;
    //    GFL_BG_BmpWinInit( wk->view.nameWin[i] );
    wk->view.nameWin[i] = GFL_BMPWIN_Create(
      GFL_BG_FRAME3_S,
      MCV_NAMEWIN_DEFX+(MCV_NAMEWIN_OFSX*x),
      MCV_NAMEWIN_DEFY+(MCV_NAMEWIN_OFSY*y),
      MCV_NAMEWIN_SIZX, MCV_NAMEWIN_SIZY,
      MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );
    // �����ɂ��ēW�J
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.nameWin[i]), 0 );
    GFL_BMPWIN_TransVramCharacter( wk->view.nameWin[i] );

    // ��Ԗʏ������ݐ�
    for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
      //  GFL_BG_BmpWinInit( wk->view.statusWin[i][j] );
      wk->view.statusWin[i][j] = GFL_BMPWIN_Create(
        GFL_BG_FRAME1_S,
        MCV_STATUSWIN_DEFX+(MCV_STATUSWIN_OFSX*x)+(j*MCV_STATUSWIN_VCHATX),
        MCV_STATUSWIN_DEFY+(MCV_STATUSWIN_OFSY*y),
        MCV_STATUSWIN_SIZX, MCV_STATUSWIN_SIZY,
        MCV_SYSFONT_PAL,
        GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.statusWin[i][j]), 0 );
      GFL_BMPWIN_TransVramCharacter( wk->view.statusWin[i][j] );
    }

  }
  wk->view.userWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME3_S,
    MCV_USERWIN_X, MCV_USERWIN_Y,
    MCV_USERWIN_SIZX, MCV_USERWIN_SIZY,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );
  // �����ɂ��ēW�J
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.userWin), 0 );
  GFL_BMPWIN_MakeScreen(wk->view.userWin);
  GFL_BMPWIN_TransVramCharacter( wk->view.userWin );

  GFL_BG_LoadScreenReq(GFL_BG_FRAME0_S);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME3_S);

}

//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�N�f�[�^�j��
 *
 *  @param  wk  �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicDel( WIFIP2PMATCH_WORK *wk )
{
  int i, j;


  // �r�b�g�}�b�v�j��
  for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){
    GFL_BMPWIN_Delete( wk->view.nameWin[i] );
    for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
      GFL_BMPWIN_Delete( wk->view.statusWin[i][j] );
    }
  }
  GFL_BMPWIN_Delete( wk->view.userWin );

  // �{�^���X�N���[���j��
  GFL_HEAP_FreeMemory( wk->view.p_bttnbuff );

  // ���[�U�[�E�C���h�E
  for( i=0; i<1; i++ ){
    GFL_HEAP_FreeMemory( wk->view.p_userbuff[i] );
  }

  // �_�~�[�X�N���[���j��
  //  GFL_HEAP_FreeMemory( wk->view.p_useretcbuff );
}

//----------------------------------------------------------------------------
/**
 *  @brief  SCRN�̃L�����N�^No��]����̃A�h���X������
 *
 *  @param  p_scrn  �X�N���[���f�[�^
 *  @param  cgofs �L�����N�^�I�t�Z�b�g�i�L�����N�^�P�ʁj
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicScrnCGOfsChange( NNSG2dScreenData* p_scrn, u8 cgofs )
{
  u16* p_scrndata;
  int i, j;
  int siz_x, siz_y;

  p_scrndata = (u16*)p_scrn->rawData;
  siz_x = p_scrn->screenWidth/8;
  siz_y = p_scrn->screenHeight/8;

  for( i=0; i<siz_y; i++ ){
    for( j=0; j<siz_x; j++ ){
      p_scrndata[ (i*siz_x)+j ] += cgofs;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �{�^���ݒ�
 *
 *  @param  wk      �V�X�e�����[�N
 *  @param  friendNo  �F�B�ԍ�
 *  @param  type    �ݒ�^�C�v
 *
 *  type
 *    MCV_BTTN_FRIEND_TYPE_RES, // �\��
 *    MCV_BTTN_FRIEND_TYPE_IN,  // �o�^�ς�
 *
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnSet( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 type )
{
  GF_ASSERT( friendNo > 0 );
  GF_ASSERT( type > MCV_BTTN_FRIEND_TYPE_NONE );
  GF_ASSERT( type < MCV_BTTN_FRIEND_TYPE_MAX );
  wk->view.bttnfriendNo[ friendNo-1 ] = type;
  wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �{�^������͂���
 *
 *  @param  wk      �V�X�e�����[�N
 *  @param  friendNo  �F�B�ԍ�
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnDel( WIFIP2PMATCH_WORK *wk, u32 friendNo )
{
  GF_ASSERT( friendNo > 0 );
  wk->view.bttnfriendNo[ friendNo-1 ] = MCV_BTTN_FRIEND_TYPE_NONE;
  wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �{�^���̕\���^�C�v���擾����
 *
 *  @param  wk      ���[�N
 *  @param  friendNo  �F�Bnumber
 *
 *  @return �\���^�C�v
 *  type
 *    MCV_BTTN_FRIEND_TYPE_RES, // �\��
 *    MCV_BTTN_FRIEND_TYPE_IN,  // �o�^�ς�
 *    MCV_BTTN_FRIEND_TYPE_NONE,  // �Ȃ�
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_BttnTypeGet( const WIFIP2PMATCH_WORK *wk, u32 friendNo )
{
  GF_ASSERT( friendNo > 0 );
  return wk->view.bttnfriendNo[ friendNo-1 ];
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������������
 *
 *  @param  wk      �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_ReWrite( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  if( wk->view.user_disp != MCV_USERDISP_OFF ){
    MCVSys_UserDispDraw( wk, heapID );
  }else{
    wk->view.bttn_allchg = TRUE;
    MCVSys_BttnDraw( wk );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �{�^�������ׂď������N�G�X�g���o��
 *
 *  @param  wk
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnAllWriteReq( WIFIP2PMATCH_WORK *wk )
{
  wk->view.bttn_allchg = TRUE;
  wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �w�i�`��
 *
 *  @param  wk  �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_BackDraw( WIFIP2PMATCH_WORK *wk )
{
  // �w�i�̃J���[��ς���
  //  GFL_BG_ChangeScreenPalette(   GFL_BG_FRAME0_S, 0, 0,
  //                              32, 24, wk->view.frame_no+MCV_PAL_FRMNO );

  //  GFL_BG_LoadScreenV_Req(   GFL_BG_FRAME0_S );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �{�^���\��
 *
 *  @param  wk  �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnDraw( WIFIP2PMATCH_WORK *wk )
{
  int i, j;
  int sex;
  int bttn_type;
  int x, y;
  int friend_no;
  int frame;
  BOOL write_change_masterflag; // �����I�ɂ��ׂẴ{�^��������������
  BOOL write_change_localflag;  // �����̃{�^�����Ƃ̏��������`�F�b�N

  // �S���������`�F�b�N
  if( wk->view.bttn_allchg == TRUE ){
    wk->view.bttn_allchg = FALSE;
    write_change_masterflag = TRUE;

    // �X�N���[���N���A
    //    GFL_BG_ScrFill( GFL_BG_FRAME2_S, 0, 0, 0, 32, 24, 0 );
    GFL_BG_ClearFrame( GFL_BG_FRAME3_S);
   // GFL_BG_ClearScreenCodeVReq(GFL_BG_FRAME2_S);
/*    {
      ARCHANDLE* p_handle;
      p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, HEAPID_WIFIP2PMATCH );
      GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, NARC_wifip2pmatch_wf_match_btm_NSCR,
                                              GFL_BG_FRAME0_S, 0,  GFL_ARCUTIL_TRANSINFO_GetPos(wk->bgchrSubBack),
                                              0, FALSE, HEAPID_WIFIP2PMATCH );
      GFL_ARC_CloseDataHandle(p_handle);
    }*/

  }else{
    write_change_masterflag = FALSE;
  }

  for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){

    friend_no = (wk->view.frame_no*WCR_MAPDATA_1BLOCKOBJNUM) + i + 1;
    // �\���^�C�v�ݒ�
    if( wk->view.bttnfriendNo[friend_no - 1] != MCV_BTTN_FRIEND_TYPE_NONE ){
      sex = WifiList_GetFriendInfo( wk->pList, friend_no - 1, WIFILIST_FRIEND_SEX );
      if(sex == PM_MALE){
        bttn_type = MCV_BUTTON_TYPE_MAN;
      }else{
        bttn_type = MCV_BUTTON_TYPE_GIRL;
      }
    }else{
      bttn_type = MCV_BUTTON_TYPE_NONE;
    }

    // �{�^���\�����W
    x = i/4;
    y = i%4;

    // ������Ă���{�^�����`�F�b�N
    if( friend_no == wk->view.touch_friendNo ){
      frame = wk->view.touch_frame;
    }else{
      frame = 0;
    }


    // �{�^���̌`���ς���Ă��邩�`�F�b�N
    if( (friend_no == wk->view.touch_friendNo) ||
        (friend_no == wk->view.bttn_chg_friendNo) ){
      write_change_localflag = TRUE;
    }else{
      write_change_localflag = FALSE;
    }

    // ���������邩�`�F�b�N
    if( (write_change_masterflag == TRUE) ||
        (write_change_localflag == TRUE) ){

      // �{�^��
      MCVSys_BttnWrite( wk,
                        MCV_BUTTON_DEFX+(MCV_BUTTON_OFSX*x), MCV_BUTTON_DEFY+(MCV_BUTTON_OFSY*y),
                        bttn_type, frame );

      // ���O�\��
      if( bttn_type != MCV_BUTTON_TYPE_NONE ){

        // ���O�̕\��
        MCVSys_BttnWinDraw( wk, wk->view.nameWin[i], friend_no, frame, i );
        GFL_BMPWIN_MakeScreen(wk->view.nameWin[i]);
        GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);

        MCVSys_BttnStatusWinDraw( wk, wk->view.statusWin[i], friend_no, frame,
                                  MCV_BUTTON_DEFX+(MCV_BUTTON_OFSX*x), MCV_BUTTON_DEFY+(MCV_BUTTON_OFSY*y) );
      }else{

        // �����ɂ���
        GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.nameWin[i]), 0 );
        GFL_BMPWIN_TransVramCharacter( wk->view.nameWin[i] );
        for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
          GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.statusWin[i][j]), 0 );
          GFL_BMPWIN_TransVramCharacter( wk->view.statusWin[i][j] );
        }
      }
    }
  }

  GFL_BG_LoadScreenV_Req(  GFL_BG_FRAME2_S );

  // ���b�Z�[�W�ʂ̕\���ݒ�
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �t�@�N�g���[�f�[�^�\��
 *
 *  @param  wk      ���[�N
 *  @param  strid   ����ID
 *  @param  factoryid �t�@�N�g���[�f�[�^ID
 *  @param  friendno  �F�B�ԍ�
 *  @param  x     �\��X
 *  @param  y     �\��Y
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispFrontiorNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 factoryid, u32 friendno, u32 x, u32 y )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�����e�B�A�@�^�C�g�����b�Z�[�W�擾
 *
 *  @param  wk      ���[�N
 *  @param  p_str   ������i�[��
 *  @param  factoryid �t�@�N�g���[�@�Z�[�u�f�[�^ID
 *  @param  friendno  �F�B�i���o�[
 *
 *  factoryid
 *    MCV_FRONTIOR_TOWOR,
 *    MCV_FRONTIOR_FACTORY,
 *    MCV_FRONTIOR_FACTORY100,
 *    MCV_FRONTIOR_CASTLE,
 *    MCV_FRONTIOR_STAGE,
 *    MCV_FRONTIOR_ROULETTE,
 *
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispFrontiorTitleStrGet( WIFIP2PMATCH_WORK *wk, STRBUF* p_str, u32 factoryid, u32 friendno )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������\�����鏈��
 *
 *  @param  wk    ���[�N
 *  @param  strid ���b�Z�[�WID
 *  @param  num   ����
 *  @param  x   ���h�b�g���W
 *  @param  y   ���h�b�g���W
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 num, u32 x, u32 y )
{
  WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
  GFL_MSG_GetString(  wk->MsgManager, strid, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                  x,  y,
                  wk->TitleString, wk->fontHandle);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �{�^���`��
 *
 *  @param  wk    �V�X�e�����[�N
 *  @param  cx    ���L�������W
 *  @param  cy    ���L�������W
 *  @param  type  �{�^���^�C�v
 *  @param  frame �t���[��
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnWrite( WIFIP2PMATCH_WORK *wk, u8 cx, u8 cy, u8 type, u8 frame )
{
  u16 reed_x, reed_y;
  u16* p_buff;


#if 1

  if(type == MCV_BUTTON_TYPE_NONE){
    reed_x = MCV_BUTTON_SIZX * 0;
    reed_y = MCV_BUTTON_SIZY * 3;
  }
  else{
    reed_x = MCV_BUTTON_SIZX * type;
    reed_y = MCV_BUTTON_SIZY * frame;
  }

  GFL_BG_WriteScreenExpand( GFL_BG_FRAME2_S,
                            cx, cy, MCV_BUTTON_SIZX, MCV_BUTTON_SIZY,
                            wk->view.p_bttnscrn->rawData,
                            reed_x, reed_y,
                            32, 24 );


#else
  // frame 3�̂Ƃ���1��\������
  if( frame == 3 ){
    frame = 1;
  }

  if( (frame < 2) ){
    reed_x = MCV_BUTTON_SIZX * type;
    reed_y = MCV_BUTTON_SIZY * frame;

    GFL_BG_WriteScreenExpand( GFL_BG_FRAME2_S,
                              cx, cy, MCV_BUTTON_SIZX, MCV_BUTTON_SIZY,
                              wk->view.p_bttnscrn->rawData,
                              reed_x, reed_y,
                              32, 18 );
  }else{
    reed_x = 0;
    reed_y = MCV_BUTTON_SIZY * type;
    p_buff = (u16*)wk->view.p_bttnscrn->rawData;

    GFL_BG_WriteScreenExpand( GFL_BG_FRAME2_S,
                              cx, cy, MCV_BUTTON_SIZX, MCV_BUTTON_SIZY,
                              &p_buff[ 32*18 ],
                              reed_x, reed_y,
                              16, 18 );
  }

  // �p���b�g�J���[�����킹��
  //  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S, cx, cy,
  //                            MCV_BUTTON_SIZX, MCV_BUTTON_SIZY, type+MCV_PAL_BTTN );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  �{�^����̃g���[�i�[���̕\��
 *
 *  @param  wk      �V�X�e�����[�N
 *  @param  p_bmp   �r�b�g�}�b�v
 *  @param  friendNo  �F�B�i���o�[
 *  @param  frame   �{�^���t���[��
 *  @param  area_id   �z�uID
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN* p_bmp, u32 friendNo, u32 frame, u32 area_id )
{
  int sex;
  int col;
  int y;

  // ��ʃN���[��
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_bmp), 0 );

  //y���W���擾
  y = 8;//ViewButtonFrame_y[ frame ];

  _COL_N_WHITE;
  MCVSys_FriendNameSet(wk, friendNo-1);
  GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_033, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_bmp),
                  2,  y,
                  wk->TitleString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter( p_bmp );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ��ԃE�B���h�E�̕`��
 *
 *  @param  wk      �����Ăރ��[�N
 *  @param  p_stbmp   ��ԃr�b�g�}�b�v
 *  @param  friendNo  �F�B�ԍ�
 *  @param  frame   �{�^���t���[��
 *  @param  area_id   �z�uID
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnStatusWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN** p_stbmp, u32 friendNo, u32 frame, u32 x, u32 y )
{
  int i;
  int vct_icon;
  WIFI_STATUS* p_status;
  u32 status,gamemode,mode;

  if(frame==0){
    mode = PLAYER_DISP_ICON_NAMEMODE;
  }
  else{
    mode = PLAYER_DISP_ICON_TOUCHMODE;
  }
  p_status = WifiFriendMatchStatusGet( friendNo - 1 );
  status = _WifiMyStatusGet( wk, p_status );
  gamemode = _WifiMyGameModeGet( wk, p_status );

  WifiP2PMatchFriendListStIconWrite( &wk->icon, GFL_BG_FRAME2_S,
                                     x+2, y+2,
                                     status,gamemode, mode);

  if( WIFI_STATUS_GetUseVChat(p_status) ){
    vct_icon = PLAYER_DISP_ICON_IDX_NONE;
  }else{
    vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
  }
  WifiP2PMatchFriendListIconWrite(  &wk->icon, GFL_BG_FRAME2_S,
                                    x+12, y+2,
                                    vct_icon+mode, 0 );

  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);

}


//----------------------------------------------------------------------------
/**
 *  @brief  Vier�̂ق��̃��[�h�Z�b�g�ɐl�̖��O��ݒ�
 *
 *  @param  p_wk      ���[�N
 *  @param  friendno    �F�B�i���o�[
 */
//-----------------------------------------------------------------------------
static void MCVSys_FriendNameSet( WIFIP2PMATCH_WORK* p_wk, int friendno )
{
  if(friendno != -1){
    int sex = WifiList_GetFriendInfo(p_wk->pList, friendno, WIFILIST_FRIEND_SEX);
    WifiList_GetFriendName(p_wk->pList, friendno, p_wk->pExpStrBuf);
    WORDSET_RegisterWord( p_wk->view.p_wordset, 0, p_wk->pExpStrBuf, sex, TRUE, PM_LANG);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�}�b�`�f�[�^�̃X�e�[�^�X�擾
 *
 *  @param  wk      �V�X�e�����[�N
 *  @param  idx     �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static WIFI_STATUS* WifiFriendMatchStatusGet( u32 idx )
{
  GF_ASSERT( idx < WIFIP2PMATCH_MEMBER_MAX );

  return (WIFI_STATUS*)GFL_NET_DWC_GetFriendInfo( idx );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B�̏�Ԃ��擾����
 *
 *  @param  idx   �C���f�b�N�X
 */
//-----------------------------------------------------------------------------

static u8 WifiDwc_getFriendStatus( int idx )
{
  return GFL_NET_DWC_getFriendStatus(idx);
}

//----------------------------------------------------------------------------
/**
 *  @brief  P2P�ʐM�Q�[�����J�n����B
 *
 *  @param  friendno  �^�[�Q�b�g�ԍ�  �i-1�Ȃ�e�j
 *  @param  status    �X�e�[�^�X
 *
 *  @return
 */
//-----------------------------------------------------------------------------
static BOOL WifiP2PMatch_CommWifiBattleStart( WIFIP2PMATCH_WORK* wk, int friendno )
{
  // �ڑ�����O�ɂS�l��W�ő����Ă���\���̂���
  // �R�}���h��ݒ肷��
  CommCommandWFP2PMF_MatchStartInitialize(wk);

  return GFL_NET_StateStartWifiPeerMatch( friendno );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B��W���@�ݒ�ON
 *
 *  @param  wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FriendRequestWaitOn( WIFIP2PMATCH_WORK* wk, BOOL msg_on )
{
  if( wk->friend_request_wait == FALSE ){
    // ���b�Z�[�W�������āA�v���C���[������~����
    wk->friend_request_wait = TRUE;
    WIFI_MCR_PlayerMovePause( &wk->matchroom, TRUE );

    if( msg_on == TRUE ){
      WifiP2PMatchMessagePrintDirect( wk, msg_wifilobby_142, FALSE );
  //    WifiP2PMatchMessagePrint( wk, msg_wifilobby_142, FALSE );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B��W���@�ݒ�OFF
 *
 *  @param  wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void FriendRequestWaitOff( WIFIP2PMATCH_WORK* wk )
{
  if( wk->friend_request_wait == TRUE ){
    // ���b�Z�[�W�������āA�v���C���[������J�n����
    wk->friend_request_wait = FALSE;
    EndMessageWindowOff( wk );
    WIFI_MCR_PlayerMovePause( &wk->matchroom, FALSE );

    wk->state = WIFIP2PMATCH_STATE_NONE;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �F�B��W���t���O�擾
 *
 *  @param  cp_wk ���[�N
 *
 *  @retval TRUE  ��W��
 *  @retval FALSE �ڂ��イ���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FriendRequestWaitFlagGet( const WIFIP2PMATCH_WORK* cp_wk )
{
  return cp_wk->friend_request_wait;
}



static BOOL _funcBGMVol(WIFIP2PMATCH_WORK* wk )
{

  //  wk->aVol.bgmVolStart = wk->aVol.bgmVol;
  //  wk->aVol.bgmVolCount = _VOL_TRACK_FRAME;
  //  wk->aVol.bgmVolEnd = endVol;
  int volnum;

  if(wk->aVol.bgmVolCount!=0){
    wk->aVol.bgmVolCount--;
    if(wk->aVol.bgmVolCount!=0){
      int sub = wk->aVol.bgmVolEnd;
      sub = sub - wk->aVol.bgmVolStart;
      if(sub < 0){
        sub = - sub;
        sub = (sub / _VOL_TRACK_FRAME) * wk->aVol.bgmVolCount;
        wk->aVol.bgmVol = wk->aVol.bgmVolEnd + sub;
      }
      else{
        sub = (sub / _VOL_TRACK_FRAME) * wk->aVol.bgmVolCount;
        wk->aVol.bgmVol = wk->aVol.bgmVolEnd - sub;
      }
      //    OS_TPrintf("VOL�ύX %d %d \n",wk->aVol.bgmVol, sub);
      PMSND_ChangeBGMVolume( _VOL_TRACK_ALL, wk->aVol.bgmVol );
      return FALSE;
    }
    else{
      //  OS_TPrintf("VOL����� %d\n",wk->aVol.bgmVolEnd);
      wk->aVol.bgmVol = wk->aVol.bgmVolEnd;
      PMSND_ChangeBGMVolume( _VOL_TRACK_ALL, wk->aVol.bgmVolEnd );
    }
  }
  return TRUE;
}


static void _changeBGMVol( WIFIP2PMATCH_WORK* wk, u8 endVol)
{

  if(wk->aVol.bgmVolEnd == endVol){
    //OS_TPrintf("�{�����[�����̂܂� %d %d\n",endVol);
    return;
  }
  OS_TPrintf("�{�����[���ύX %d %d \n",wk->aVol.bgmVolEnd, endVol);
  wk->aVol.bgmVolStart = wk->aVol.bgmVol;
  wk->aVol.bgmVolCount = _VOL_TRACK_FRAME;
  wk->aVol.bgmVolEnd = endVol;
  //  PMSND_SetMaxVolume( endVol );

  PMSND_AllPlayerVolumeSet( endVol, PMSND_MASKPL_ALL);
  PMV_SetMasterVolume(endVol);

}


static void _initBGMVol( WIFIP2PMATCH_WORK* wk, int status)
{

  if(status == WIFI_STATUS_PLAYING){
    wk->aVol.bgmVolEnd = _VOL_DEFAULT/3;
  }
  else{
    wk->aVol.bgmVolEnd = _VOL_DEFAULT;
  }
  wk->aVol.bgmVolStart = wk->aVol.bgmVolEnd;
  // OS_TPrintf("�{�����[�������� %d\n",wk->aVol.bgmVolEnd);
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT WifiP2PMatchProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFIP2PMATCH_WORK * wk = mywk;
  WIFIP2PMATCH_PROC_PARAM* pParentWork = pwk;
  u32 result;


  GFL_HEAP_CreateHeapLo( GFL_HEAPID_APP, HEAPID_WIFIP2PMATCH, 0xa0000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(WIFIP2PMATCH_WORK), HEAPID_WIFIP2PMATCH );
  MI_CpuFill8( wk, 0, sizeof(WIFIP2PMATCH_WORK) );
  GFL_NET_ChangeWork(wk);

  // Vram�]���}�l�[�W���쐬
#if WB_FIX
  initVramTransferManagerHeap( VRANTRANSFERMAN_NUM, HEAPID_WIFIP2PMATCH );
#endif

  //        wk->MsgIndex = _PRINTTASK_MAX;
  wk->pMatch = pParentWork->pMatch;

  wk->pSaveData = pParentWork->pSaveData;
  wk->pGameData = pParentWork->pGameData;

  wk->bb_party = _BBox_PokePartyAlloc(wk->pGameData);
  
  wk->pMyPoke = GAMEDATA_GetMyPokemon(wk->pGameData);
  wk->pList = GAMEDATA_GetWiFiList(wk->pGameData);
  wk->pConfig = SaveData_GetConfig(pParentWork->pSaveData);
  wk->initSeq = pParentWork->seq;    // P2P��DPW��

  wk->state = pParentWork->bTalk;
  
  if(pParentWork->seq != WIFI_GAME_NONE){
    _initBGMVol( wk, WIFI_STATUS_PLAYING);
  }
  else
  {        // �ڑ����܂�
    _initBGMVol( wk, WIFI_STATUS_WAIT);
  }

  wk->endSeq = WIFI_GAME_NONE;
  wk->preConnect = -1;
  wk->pParentWork = pParentWork;

  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );

  // ���[�N������
  InitMessageWork( wk );
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_INIT);

  // �O���t�B�b�N������
  _graphicInit(wk);

  if(pParentWork->seq == WIFI_GAME_NONE){
    _makeMyMatchStatus(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
  }

  if(GFL_NET_IsInit()){
    GFL_NET_ReloadIconTopOrBottom(TRUE, HEAPID_WIFIP2PMATCH);
  }

  return GFL_PROC_RES_FINISH;
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

static GFL_PROC_RESULT WifiP2PMatchProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFIP2PMATCH_WORK * wk  = mywk;


  if( NetErr_App_CheckError() == NET_ERR_CHECK_NONE ){
    if(FuncTable[wk->seq]!=NULL){
      *seq = (*FuncTable[wk->seq])( wk, *seq );
      if(*seq == SEQ_OUT){
        WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
                        WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );
        return GFL_PROC_RES_FINISH;
      }
    }
  }
  if(wk->clactSet){
    GFL_CLACT_SYS_Main();
  }

#ifdef DEBUG_ONLY_FOR_toru_nagihashi
  {
    static const char *sc_print_tbl[] =
    {
      "�Ȃ�",
      "�}�V�[����W",
      "��W�ɎQ��",
      "�͂Ȃ�������",
      "��ꂽ",
    };
    NAGI_Printf( "���:%s\n", sc_print_tbl[ wk->state ] );
  }
#endif

  if(wk->SysMsgQue){
    u8 defL, defS, defB;
    GFL_FONTSYS_GetColor( &defL, &defS, &defB );
    GFL_FONTSYS_SetColor(FBMP_COL_BLACK, FBMP_COL_BLK_SDW, 15);
    GFL_TCBL_Main( wk->pMsgTcblSys );
    GFL_FONTSYS_SetColor( defL, defS, defB );
    PRINTSYS_QUE_Main(wk->SysMsgQue);
  }
  if(wk->SysMenuQue){
    PRINTSYS_QUE_Main(wk->SysMenuQue);
  }
  _funcBGMVol(wk);

  if(GFL_NET_IsInit()){
    switch(GFL_NET_DWC_ERROR_ReqErrorDisp(TRUE,TRUE)){
    case GFL_NET_DWC_ERROR_RESULT_TIMEOUT:
      WIFIP2PMatch_pokePartyMenuDelete(wk);
      BmpWinDelete(wk);
      GFL_NET_SetAutoErrorCheck(FALSE);
      GFL_NET_SetNoChildErrorCheck(FALSE);
      GFL_NET_StateWifiMatchEnd(TRUE);  // �}�b�`���O��؂�
      _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
      wk->timer = _RECONECTING_WAIT_TIME;
    _CHANGESTATE(wk, WIFIP2PMATCH_RECONECTING_WAIT);
      break;
    case GFL_NET_DWC_ERROR_RESULT_RETURN_PROC:
      WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
      WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
      wk->endSeq = WIFI_GAME_ERROR;
      return GFL_PROC_RES_FINISH;
    }
  }
  return GFL_PROC_RES_CONTINUE;
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
static GFL_PROC_RESULT WifiP2PMatchProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFIP2PMATCH_WORK  *wk    = mywk;
  WIFIP2PMATCH_PROC_PARAM* pParentWork = pwk;


  if(!_funcBGMVol(wk)){
    return GFL_PROC_RES_CONTINUE;
  }
  if( !WIPE_SYS_EndCheck() ){
    return GFL_PROC_RES_CONTINUE;
  }

  pParentWork->bTalk =  wk->state;

  if(wk->pRegulation){
    Regulation_Copy(wk->pRegulation, pParentWork->pRegulation);
    GFL_HEAP_FreeMemory(wk->pRegulation);
    wk->pRegulation=NULL;
  }

  WIFIP2PMatch_pokePartyMenuDelete(wk);
  _graphicEnd(wk);

  pParentWork->friendNo = wk->friendNo;
  OS_TPrintf("�Ƃ������̂΂񂲂���%d\n", wk->friendNo);
  pParentWork->seq = wk->endSeq;

  if(GFL_NET_IsInit()){
    pParentWork->targetID = GFL_NET_DWC_GetFriendIndex();
  }

  _BBox_PokePartyFree(wk->bb_party);

  
  // ���[�N���
  FreeMessageWork( wk );

  GFL_PROC_FreeWork( proc );        // GFL_PROC���[�N�J��


#if WB_TEMP_FIX
  // VramTransfer�}�l�[�W���j��
  DellVramTransferManager();
#endif

  GFL_HEAP_DeleteHeap( HEAPID_WIFIP2PMATCH );


  return GFL_PROC_RES_FINISH;
}



// �v���Z�X��`�f�[�^
const GFL_PROC_DATA WifiP2PMatchProcData = {
  WifiP2PMatchProc_Init,
  WifiP2PMatchProc_Main,
  WifiP2PMatchProc_End,
};

