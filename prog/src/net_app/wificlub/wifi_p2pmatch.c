//============================================================================================
/**
 * @file	wifi_p2pmatch.c
 * @bfief	WIFIP2P�}�b�`���O���
 * @author	k.ohno
 * @date	06.04.07
 */
//============================================================================================

#include <gflib.h>
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


#include "savedata/wifihistory.h"
#include "savedata/config.h"
#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"
#include "system/main.h"
#include "system/rtc_tool.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/pokeparty.h"
#include "item/itemsym.h"

#include "font/font.naix"
#include "print/str_tool.h"

#include "wifip2pmatch.naix"			// �O���t�B�b�N�A�[�J�C�u��`
#include "comm_command_wfp2pmf.h"  //�Q�c�t�B�[���h
#include "comm_command_wfp2pmf_func.h"  //�Q�c�t�B�[���h
#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_system.h"

#include "field/fldmmdl_pl_code.h" //�N���[�`���[
#include "sound/pm_sndsys.h"  //SOUND�֘A



// �u��������K�v�����邪�܂��Ȃ��֐�  @@OO
#define SND_HANDLE_FIELD (0)
#define BGM_POKECEN_VOL (0)
#define SND_SCENE_P2P (0)
#define SEQ_WIFILOBBY (0)
#define SND_SCENE_DUMMY (0)
#define BGM_WIFILOBBY_VOL (0)
#define SEQ_PC_01 (0)
#define SEQ_PC_02 (0)
#define FLAG_ARRIVE_D32R0101 (0)
#define GMDATA_ID_EMAIL (0)
#define ID_PARA_monsno (0)
#define ID_PARA_item (0)
#define NUMBER_DISPTYPE_ZERO (STR_NUM_DISP_ZERO)
#define NUMBER_CODETYPE_DEFAULT (0)
#define _SE_CURSOR (0)
#define _SE_INOUT  (0)
#define _SE_DESIDE (0)
#define _SE_OFFER (0)
#define _SE_TBLCHANGE (0)
#define	FBMP_COL_NULL		(0)
#define	FBMP_COL_BLACK		(1)
#define	FBMP_COL_BLK_SDW	(2)
#define	FBMP_COL_RED		(3)
#define	FBMP_COL_RED_SDW	(4)
#define	FBMP_COL_GREEN		(5)
#define	FBMP_COL_GRN_SDW	(6)
#define	FBMP_COL_BLUE		(7)
#define	FBMP_COL_BLU_SDW	(8)
#define	FBMP_COL_PINK		(9)
#define	FBMP_COL_PNK_SDW	(10)
#define	FBMP_COL_WHITE		(15)
#define BGM_FADE_VCHAT_TIME (0)
#define BGM_WIFILOBBY_VOL (0)
#define BGM_FADEIN_START_VOL_NOW (0)
#define _BATTLE_TOWER_REC (0)   // �o�g���^���[�̋L�^�\���𕕈�
#define _OAMBUTTON (1)
#define MYSTATUS_GETTRAINERVIEW_COMPLETION (0)   //���j�I���̎p�������œ���Ȃ��Ƃ��܂��o�Ȃ�

typedef struct{
  int a;
} ZUKAN_WORK;


static void Snd_PlayerSetInitialVolume(int a,int b){}
static void* TimeWaitIconAdd(GFL_BMPWIN* a,int cgx){ return NULL; }
static void TimeWaitIconTaskDel(void* c){}
static void Snd_DataSetByScene( int a, int b, int c ){}
static void Snd_SceneSet( int a ){}
static int Snd_NowBgmNoGet(void){ return 0;}
static void Snd_PlayerSetInitialVolumeBySeqNo( int a,  int b){}
static void* SaveData_GetEventWork(void* a){ return NULL; }
static BOOL SysFlag_ArriveGet(void* a,int b){ return TRUE;}
static void* SaveData_Get(void* a, int b){ return NULL; }
static void* SaveData_GetFrontier(void* a){ return NULL; }
static void EMAILSAVE_Init(void* a){}
static ZUKAN_WORK* SaveData_GetZukanWork(SAVE_CONTROL_WORK* a){ return NULL; }
static int PokeParaGet( POKEMON_PARAM* poke, int no, void* c ){return 0;}
static BOOL ZukanWork_GetZenkokuZukanFlag(ZUKAN_WORK* pZukan){ return TRUE; }
static void CommInfoFinalize(void){}
static void Snd_SePlay(int a){}
static void Snd_BgmFadeOut( int a, int b){}
static void Snd_BgmFadeIn( int a, int b, int c){}
static void	FONTOAM_OAMDATA_Delete( void* x){}



#define	COMM_BRIGHTNESS_SYNC	( 1 )	// ��{�̋P�x�ύXSync��
#define _BMPMENULIST_FONTSIZE   (12)

// WIFI2DMAP�V�X�e���I�[�o�[���C
//FS_EXTERN_OVERLAY(wifi2dmap);

// WIFI�@�ΐ�AUTOӰ�ރf�o�b�N
#ifdef _WIFI_DEBUG_TUUSHIN
extern WIFI_DEBUG_BATTLE_WK WIFI_DEBUG_BATTLE_Work;
#endif	//_WIFI_DEBUG_TUUSHIN


//-------------------------------------
///	�}�b�`���O�p�g���q�[�v�T�C�Y
//=====================================
#define WIFI_P2PMATCH_EXHEAP_SIZE	( 0x10000 )




///	�a�f�p���b�g��` 2005/09/15
//
//	0 �` 5  : �t�B�[���h�}�b�v�a�f�p
//  6       : �V��
//  7       : �n���E�C���h�E
//  8       : �ŔE�C���h�E�i�}�b�v�A�W���Ȃǁj
//  9       : �ŔE�C���h�E�i�g�A�t�H���g�j
//  10      : ���b�Z�[�W�E�C���h�E
//  11      : ���j���[�E�C���h�E
//  12      : ���b�Z�[�W�t�H���g
//  13      : �V�X�e���t�H���g
//	14		: ���g�p�i���[�J���C�Y�p�j
//	15		: �f�o�b�O�p�i���i�łł͖��g�p�j
#define FLD_WEATHER_PAL      (  6 )			//  �V��
#define FLD_PLACENAME_PAL    (  7 )         //  �n���E�C���h�E
#define FLD_BOARD1FRAME_PAL  (  8 )         //  �ŔE�C���h�E�i�}�b�v�A�W���Ȃǁj
#define FLD_BOARD2FRAME_PAL  (  9 )         //  �ŔE�C���h�E�i�g�A�t�H���g�j
#define FLD_MESFRAME_PAL     ( 10 )         //  ���b�Z�[�W�E�C���h�E
#define FLD_MENUFRAME_PAL    ( 11 )         //  ���j���[�E�C���h�E
#define FLD_MESFONT_PAL      ( 12 )         //  ���b�Z�[�W�t�H���g
#define FLD_SYSFONT_PAL	     ( 13 )         //  �V�X�e���t�H���g
#define FLD_LOCALIZE_PAL     ( 14 )         //	���g�p�i���[�J���C�Y�p�j
#define FLD_DEBUG_PAL        ( 15 )         //	�f�o�b�O�p�i���i�łł͖��g�p�j

/*********************************************************************************************
	���C����ʂ�CGX����U��		2006/01/12

		�E�B���h�E�g	�F	409 - 511
			��b�A���j���[�A�n���A�Ŕ�

		BMP�E�B���h�E�P	�F	297 - 408
			��b�i�ő�j�A�ŔA�c��{�[����

		BMP�E�B���h�E�Q	�F	55 - 296
			���j���[�i�ő�j�A�͂�/�������A�n��

 *********************************************************************************************/

/*********************************************************************************************
	�E�B���h�E�g
 *********************************************************************************************/
// ��b�E�B���h�E�L����
#define	TALK_WIN_CGX_SIZE	( 18+12 )
#define	TALK_WIN_CGX_NUM	( 512 - TALK_WIN_CGX_SIZE )
#define	TALK_WIN_PAL		( 10 )

// ���j���[�E�B���h�E�L����
#define	MENU_WIN_CGX_SIZE	( 9 )
#define	MENU_WIN_CGX_NUM	( TALK_WIN_CGX_NUM - MENU_WIN_CGX_SIZE )
#define	MENU_WIN_PAL		( 11 )

// �n���E�B���h�E�L����
#define	PLACE_WIN_CGX_SIZE	( 10 )
#define	PLACE_WIN_CGX_NUM	( MENU_WIN_CGX_NUM - PLACE_WIN_CGX_SIZE )
#define	PLACE_WIN_PAL		( 7 )

// �ŔE�B���h�E�L����
#define	BOARD_WIN_CGX_SIZE	( 18+12 + 24 )
#define	BOARD_WIN_CGX_NUM	( PLACE_WIN_CGX_NUM - BOARD_WIN_CGX_SIZE )
#define	BOARD_WIN_PAL		( FLD_BOARD2FRAME_PAL )

/*********************************************************************************************
	BMP�E�B���h�E
 *********************************************************************************************/
// ��b�E�B���h�E�i���C���j
#define	FLD_MSG_WIN_PX		( 2 )
#define	FLD_MSG_WIN_PY		( 19 )
#define	FLD_MSG_WIN_SX		( 27 )
#define	FLD_MSG_WIN_SY		( 4 )
#define	FLD_MSG_WIN_PAL		( FLD_MESFONT_PAL )
#define	FLD_MSG_WIN_CGX		( BOARD_WIN_CGX_NUM - ( FLD_MSG_WIN_SX * FLD_MSG_WIN_SY ) )

// �ŔE�B���h�E�i���C���j�i��b�Ɠ����ʒu�i��b��菬�����j�j
#define	FLD_BOARD_WIN_PX	( 9 )
#define	FLD_BOARD_WIN_PY	( 19 )
#define	FLD_BOARD_WIN_SX	( 20 )
#define	FLD_BOARD_WIN_SY	( 4 )
#define	FLD_BOARD_WIN_PAL	( FLD_BOARD2FRAME_PAL )
#define	FLD_BOARD_WIN_CGX	( FLD_MSG_WIN_CGX )

// �c��{�[�����i��b�Ɠ����ʒu�i��b��菬�����j�j
#define	FLD_BALL_WIN_PX		( 1 )
#define	FLD_BALL_WIN_PY		( 1 )
#define	FLD_BALL_WIN_SX		( 12 )
#define	FLD_BALL_WIN_SY		( 4 )
#define	FLD_BALL_WIN_PAL	( FLD_SYSFONT_PAL )
#define	FLD_BALL_WIN_CGX	( FLD_MSG_WIN_CGX )

// ���j���[�E�B���h�E�i���C���j
#define	FLD_MENU_WIN_PX		( 20 )
#define	FLD_MENU_WIN_PY		( 1 )
#define	FLD_MENU_WIN_SX		( 11 )
#define	FLD_MENU_WIN_SY		( 22 )
#define	FLD_MENU_WIN_PAL	( FLD_SYSFONT_PAL )
#define	FLD_MENU_WIN_CGX	( FLD_MSG_WIN_CGX - ( FLD_MENU_WIN_SX * FLD_MENU_WIN_SY ) )

// �͂�/�������E�B���h�E�i���C���j�i���j���[�Ɠ����ʒu�i���j���[��菬�����j�j
#define	FLD_YESNO_WIN_PX	( 25 )
#define	FLD_YESNO_WIN_PY	( 13 )
#define	FLD_YESNO_WIN_SX	( 6 )
#define	FLD_YESNO_WIN_SY	( 4 )
#define	FLD_YESNO_WIN_PAL	( FLD_SYSFONT_PAL )
#define	FLD_YESNO_WIN_CGX	( FLD_MSG_WIN_CGX - ( FLD_YESNO_WIN_SX * FLD_YESNO_WIN_SY ) )

// �n���E�B���h�E�i���j���[�Ɠ����ʒu�i���j���[��菬�����j�j
#define FLD_PLACE_WIN_PX	( 0 )
#define FLD_PLACE_WIN_PY	( 0 )
#define	FLD_PLACE_WIN_SX	( 32 )
#define	FLD_PLACE_WIN_SY	( 3 )
#define	FLD_PLACE_WIN_CGX	( FLD_MSG_WIN_CGX - ( FLD_PLACE_WIN_SX * FLD_PLACE_WIN_SY ) )



// ���b�Z�[�W�E�B���h�E�i�T�u�j
#define	FLD_MSG_WIN_S_PX	( 2  )
#define	FLD_MSG_WIN_S_PY	( 19 )
#define	FLD_MSG_WIN_S_PAL	( FLD_MESFONT_PAL )
#define	FLD_MSG_WIN_S_CGX	( MENU_WIN_CGX_NUM - ( FLD_MSG_WIN_SX * FLD_MSG_WIN_SY ) )

// �ŔE�B���h�E�i�T�u�j
#define	FLD_BOARD_WIN_S_PX	( 9 )
#define	FLD_BOARD_WIN_S_PY	( 19 )
#define	FLD_BOARD_WIN_S_SX	( 21 )
#define	FLD_BOARD_WIN_S_SY	( 4 )
#define	FLD_BOARD_WIN_S_PAL	( FLD_BOARD2FRAME_PAL )
#define	FLD_BOARD_WIN_S_CGX	( FLD_MSG_WIN_S_CGX )

// ���j���[�E�B���h�E�i�T�u�j
#define	FLD_MENU_WIN_S_PX	( 25 )
#define	FLD_MENU_WIN_S_PY	( 1 )
#define	FLD_MENU_WIN_S_SX	( 6 )
#define	FLD_MENU_WIN_S_SY	( 16 )
#define	FLD_MENU_WIN_S_PAL	( FLD_SYSFONT_PAL )
#define	FLD_MENU_WIN_S_CGX	( MENU_WIN_CGX_NUM - ( FLD_MENU_WIN_SX * FLD_MENU_WIN_SY ) )



///	�r�b�g�}�b�v�]���֐��p��`
//------------------------------------------------------------------
#define	FBMP_TRANS_OFF	(0)
#define	FBMP_TRANS_ON	(1)
///	�����\���]���֐��p��`
//------------------------------------------------------------------
#define	FBMPMSG_WAITON_SKIPOFF	(0)
#define	FBMPMSG_WAITON_SKIPON	(1)
#define	FBMPMSG_WAITOFF			(2)

//-------------------------------------------------------------------------
///	�����\���F��`(default)	-> gflib/fntsys.h�ֈړ�
//------------------------------------------------------------------

#define WINCLR_COL(col)	(((col)<<4)|(col))

//-------------------------------------------------------------------------
///	�����\���X�s�[�h��`(default)
//------------------------------------------------------------------
#define	FBMP_MSG_SPEED_SLOW		(8)
#define	FBMP_MSG_SPEED_NORMAL	(4)
#define	FBMP_MSG_SPEED_FAST		(1)



//============================================================================================
//	�萔��`
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

#define WF_CLACT_WKNUM	( 8 )
#define WF_CLACT_RESNUM	( 1 )
#define WF_FONTOAM_NUM	( 1 )

#define _PRINTTASK_MAX (8)
#define _TIMING_GAME_CHECK  (13)// �Ȃ���������
#define _TIMING_GAME_CHECK2  (14)// �Ȃ���������
#define _TIMING_GAME_START  (15)// �^�C�~���O�����낦��
#define _TIMING_GAME_START2  (18)// �^�C�~���O�����낦��
#define _TIMING_BATTLE_END  (16)// �^�C�~���O�����낦��

#define _RECONECTING_WAIT_TIME (20)  //�Đڑ�����


// ���[�U�[�\���ʂ̐ݒ�
#define WIFIP2PMATCH_PLAYER_DISP_X	( 1 )
#define WIFIP2PMATCH_PLAYER_DISP_Y	( 1 )
#define WIFIP2PMATCH_PLAYER_DISP_SIZX	( 28 )
#define WIFIP2PMATCH_PLAYER_DISP_SIZY	( 2 )
#define WIFIP2PMATCH_PLAYER_DISP_NAME_X	( 32 )
#define WIFIP2PMATCH_PLAYER_DISP_ST_X	( 102 )
#define WIFIP2PMATCH_PLAYER_DISP_WINSIZ	(WIFIP2PMATCH_PLAYER_DISP_SIZX*WIFIP2PMATCH_PLAYER_DISP_SIZY)


//FRAME3
#define	_CGX_BOTTOM	( 512 )							// ��b�E�B���h�E�L����
#define _CGX_TITLE_BOTTOM	(_CGX_BOTTOM - 18*3)	// �^�C�g���E�B���h�E
#define _CGX_USER_BOTTOM	(_CGX_TITLE_BOTTOM - WIFIP2PMATCH_PLAYER_DISP_WINSIZ)	// ���[�U�[�E�B���h�E

// FRAME1�ɓ]�����郆�[�U�[�f�[�^�w�i
#define _CGX_USET_BACK_BOTTOM	( MENU_WIN_CGX_NUM - WIFIP2PMATCH_PLAYER_DISP_WINSIZ )

// FRAME1�ɓ]������ICON�f�[�^
#define PLAYER_DISP_ICON_PLTTOFS	(8)
#define PLAYER_DISP_ICON_PLTTOFS_SUB (13)
#define PLAYER_DISP_ICON_PLTTNUM	(2)
#define PLAYER_DISP_ICON_CG_SIZX	(12)
#define PLAYER_DISP_ICON_CG_SIZY	(4)
#define PLAYER_DISP_ICON_CGX		( 0 )
#define PLAYER_DISP_ICON_SCRN_X	( 2 )
#define PLAYER_DISP_ICON_SCRN_Y	( 2 )
#define PLAYER_DISP_ICON_POS_X	( 2 )
#define PLAYER_DISP_ICON_POS_Y	( 1 )
#define PLAYER_DISP_VCTICON_POS_X	( 26 )
#define PLAYER_DISP_VCTICON_POS_Y	( 1 )
enum{
  PLAYER_DISP_ICON_IDX_NONE00,
  PLAYER_DISP_ICON_IDX_VCTNOT,
  PLAYER_DISP_ICON_IDX_VCTBIG,
  PLAYER_DISP_ICON_IDX_VCT,
  PLAYER_DISP_ICON_IDX_NORMAL,
  PLAYER_DISP_ICON_IDX_UNK,
  PLAYER_DISP_ICON_IDX_FIGHT,
  PLAYER_DISP_ICON_IDX_CHANGE,
  PLAYER_DISP_ICON_IDX_NONE,
  PLAYER_DISP_ICON_IDX_MINIGAME,
  PLAYER_DISP_ICON_IDX_FRONTIER,
  PLAYER_DISP_ICON_IDX_NUM,
};


// ��b�E�B���h�E	FRAME2
#define COMM_MESFRAME_PAL     ( 10 )         //  ���b�Z�[�W�E�C���h�E
#define COMM_MENUFRAME_PAL    ( 11 )         //  ���j���[�E�C���h�E
#define COMM_MESFONT_PAL      ( 12 )         //  ���b�Z�[�W�t�H���g
#define COMM_SYSFONT_PAL	  ( 13 )         //  �V�X�e���t�H���g
#define	COMM_TALK_WIN_CGX_SIZE	( 18+12 )
//#define	COMM_TALK_WIN_CGX_NUM	( 512 - COMM_TALK_WIN_CGX_SIZE)
#define	COMM_TALK_WIN_CGX_NUM	( 48 )

#define	COMM_MSG_WIN_PX		( 2 )
#define	COMM_MSG_WIN_PY		( 19 )
#define	COMM_MSG_WIN_SX		( 27 )
#define	COMM_MSG_WIN_SY		( 4 )
#define	COMM_MSG_WIN_PAL		( COMM_MESFONT_PAL )
#define	COMM_MSG_WIN_CGX		( (COMM_TALK_WIN_CGX_NUM - 73) - ( COMM_MSG_WIN_SX * COMM_MSG_WIN_SY ) )

#define	COMM_SYS_WIN_PX		( 4 )
#define	COMM_SYS_WIN_PY		( 4 )
#define	COMM_SYS_WIN_SX		( 23 )
#define	COMM_SYS_WIN_SY		( 16 )
#define	COMM_SYS_WIN_PAL		( COMM_MESFONT_PAL )
#define	COMM_SYS_WIN_CGX		( MENU_WIN_CGX_NUM - ( COMM_SYS_WIN_SX * COMM_SYS_WIN_SY ) )	// �ʐM�V�X�e���E�B���h�E�]����


// YesNoWin
#define YESNO_WIN_CGX	((COMM_SYS_WIN_CGX - (FLD_YESNO_WIN_SX*FLD_YESNO_WIN_SY)))
#define FRAME1_YESNO_WIN_CGX	((_CGX_USET_BACK_BOTTOM - (FLD_YESNO_WIN_SX*FLD_YESNO_WIN_SY)))


// ���C�����b�Z�[�W�L����
#define _NUKI_FONT_PALNO  (13)
#define	_COL_N_BLACK	( GFL_FONTSYS_SetColor( 1, 2, 0 ) )		// �t�H���g�J���[�F��
#define	_COL_N_WHITE	( GFL_FONTSYS_SetColor( 15, 14, 0 ) )		// �t�H���g�J���[�F��
#define	_COL_N_RED	    ( GFL_FONTSYS_SetColor( 3, 4, 0 ) )		// �t�H���g�J���[�F��
#define	_COL_N_BLUE	    ( GFL_FONTSYS_SetColor( 5, 6, 0 ) )		// �t�H���g�J���[�F��
#define	_COL_N_GRAY  	( GFL_FONTSYS_SetColor( 2, 14, 0 ) )		// �t�H���g�J���[�F�D

// ���O�\��BMP�i���ʁj
#define WIFIP2PMATCH_NAME_BMP_W	 ( 16 )
#define WIFIP2PMATCH_NAME_BMP_H	 (  2 )
#define WIFIP2PMATCH_NAME_BMP_SIZE (WIFIP2PMATCH_NAME_BMP_W * WIFIP2PMATCH_NAME_BMP_H)

// ��b�E�C���h�E�\���ʒu��`
#define WIFIP2PMATCH_TALK_X		(  2 )
#define WIFIP2PMATCH_TALK_Y		(  19 )

#define WIFIP2PMATCH_TITLE_X		(   3  )
#define WIFIP2PMATCH_TITLE_Y		(   1  )
#define WIFIP2PMATCH_TITLE_W		(  26  )
#define WIFIP2PMATCH_TITLE_H		(   2  )


#define WIFIP2PMATCH_MSG_WIN_OFFSET 		(1 + TALK_WIN_CGX_SIZ     + MENU_WIN_CGX_SIZ)
#define WIFIP2PMATCH_TITLE_WIN_OFFSET		( WIFIP2PMATCH_MSG_WIN_OFFSET   + FLD_MSG_WIN_SX*FLD_MSG_WIN_SY )
#define WIFIP2PMATCH_NAME_WIN_OFFSET		( WIFIP2PMATCH_TITLE_WIN_OFFSET + WIFIP2PMATCH_TITLE_W*WIFIP2PMATCH_TITLE_H )
#define WIFIP2PMATCH_YESNO_WIN_OFFSET		( WIFIP2PMATCH_NAME_WIN_OFFSET  + WIFIP2PMATCH_NAME_BMP_SIZE*5 )


// VIEW��
enum{
  MCV_BTTN_FRIEND_TYPE_NONE,	// �ݒ肵�Ă��Ȃ�
  MCV_BTTN_FRIEND_TYPE_RES,	// �\��
  MCV_BTTN_FRIEND_TYPE_IN,		// �o�^�ς�
  MCV_BTTN_FRIEND_TYPE_MAX,	// �^�C�v�ő�l
};
#define MCV_PAL_BACK		( 0 )	// �w�i�p���b�g�̊J�n�ʒu
#define MCV_PAL_FRMNO		( 0 )	// �w�i�̃o�b�g�p���b�g�J�n�ʒu
#define MCV_PAL_BTTN		( 4 )	// �{�^���p���b�g�̊J�n�ʒu
enum{
  MCV_PAL_BACK_0 = 0,
  MCV_PAL_BACK_1,
  MCV_PAL_BACK_2,
  MCV_PAL_BACK_3,
  MCV_PAL_BACK_NUM,

  MCV_PAL_BTTN_GIRL = 0,
  MCV_PAL_BTTN_MAN,
  MCV_PAL_BTTN_NONE,
  MCV_PAL_BTTNST_GIRL,
  MCV_PAL_BTTNST_MAN,
  MCV_PAL_BTTN_NUM	// ���̂Ƃ���]���
    // MCV_PAL_BTTN+MCV_PAL_BTTN_NUM�`(PLAYER_DISP_ICON_PLTTOFS_SUB-1�܂�
};
// �A�C�R���̓]���ʒu
#define MCV_ICON_CGX	(0)
#define MCV_ICON_CGSIZ	(48)
#define MCV_ICON_PAL		(PLAYER_DISP_ICON_PLTTOFS_SUB)

#define MCV_CGX_BTTN2	(MCV_ICON_CGX+MCV_ICON_CGSIZ)	// FRAME2���[�U�[�f�[�^
#define MCV_CGX_BACK	(0)// FRAME0�w�i
#define MCV_SYSFONT_PAL	( 15 )	// �V�X�e���t�H���g
// WIN�ݒ�
#define MCV_NAMEWIN_CGX		( 1 )	// ���O�E�B���h�E�J�n�ʒu
#define MCV_NAMEWIN_DEFX	( 4 )	// ��{�ʒu
#define MCV_NAMEWIN_DEFY	( 1 )
#define MCV_NAMEWIN_OFSX	( 16 )	// �ʒu�̈ړ��l
#define MCV_NAMEWIN_OFSY	( 6 )
#define MCV_NAMEWIN_SIZX	( 9 )	// �T�C�Y
#define MCV_NAMEWIN_SIZY	( 3 )
#define MCV_NAMEWIN_CGSIZ	( MCV_NAMEWIN_SIZX*MCV_NAMEWIN_SIZY )	// CG�T�C�Y
#define MCV_NAMEWIN_CGALLSIZ	((MCV_NAMEWIN_CGSIZ*WCR_MAPDATA_1BLOCKOBJNUM)+MCV_NAMEWIN_CGX)// CG�����T�C�Y

#define MCV_STATUSWIN_CGX	( MCV_NAMEWIN_CGALLSIZ+1 )
#define MCV_STATUSWIN_DEFX	( 1 )	// ��{�ʒu
#define MCV_STATUSWIN_DEFY	( 1 )
#define MCV_STATUSWIN_VCHATX ( 12 )	// VCHAT�ʒu
#define MCV_STATUSWIN_OFSX	( 16 )	// �ʒu�̈ړ��l
#define MCV_STATUSWIN_OFSY	( 6 )
#define MCV_STATUSWIN_SIZX	( 2 )	// �T�C�Y
#define MCV_STATUSWIN_SIZY	( 3 )
#define MCV_STATUSWIN_CGSIZ	( MCV_STATUSWIN_SIZX*MCV_STATUSWIN_SIZY )	// CG�T�C�Y

#define MCV_USERWIN_CGX		( 1 )
#define MCV_USERWIN_X		( 1 )
#define MCV_USERWIN_Y		( 1 )
#define MCV_USERWIN_SIZX	( 30 )
#define MCV_USERWIN_SIZY	( 21 )


#define MCV_BUTTON_SIZX	( 16 )
#define MCV_BUTTON_SIZY	( 6 )
#define MCV_BUTTON_DEFX	(0)
#define MCV_BUTTON_DEFY	(0)
#define MCV_BUTTON_OFSX	(16)
#define MCV_BUTTON_OFSY	(6)

#define MCV_BUTTON_ICON_OFS_X	( 1 )
#define MCV_BUTTON_VCTICON_OFS_X	( 13 )
#define MCV_BUTTON_ICON_OFS_Y	( 1 )

#define MCV_BUTTON_FRAME_NUM	(4)	// �{�^���A�j���t���[����

enum{
  MCV_BUTTON_TYPE_GIRL,
  MCV_BUTTON_TYPE_MAN,
  MCV_BUTTON_TYPE_NONE,
};

enum{
  MCV_USERDISP_OFF,	// ���[�U�[�\��OFF
  MCV_USERDISP_INIT,	// ���[�U�[�\��������
  MCV_USERDISP_ON,	// ���[�U�[�\��ON
  MCV_USERDISP_ONEX,	// ���[�U�[�\������ON
};

// PAGE 1
#define MCV_USERD_NAME_X	( 32 )
#define MCV_USERD_NAME_Y	( 8 )
#define MCV_USERD_ST_X	( 104 )
#define MCV_USERD_ST_Y	( 8 )
#define MCV_USERD_GR_X	( 8 )
#define MCV_USERD_GR_Y	( 32 )
#define MCV_USERD_VS_X	( 8 )
#define MCV_USERD_VS_Y	( 56 )
#define MCV_USERD_VS_WIN_X	( 120 )
#define MCV_USERD_VS_WIN_Y	( 56 )
#define MCV_USERD_VS_LOS_X	( 184 )
#define MCV_USERD_TR_X		( 8 )
#define MCV_USERD_TR_Y		( 80 )
#define MCV_USERD_TRNUM_X	( 152 )
#define MCV_USERD_DAY_X		( 8 )
#define MCV_USERD_DAY_Y		( 128 )
#define MCV_USERD_DAYNUM_X	( 152 )
#define MCV_USERD_ICON_X	( 2 )
#define MCV_USERD_VCTICON_X	( 28 )
#define MCV_USERD_ICON_Y	( 2 )

// PAGE2
#define MCV_USERD_BTTW_TITLE_X		(8)
#define MCV_USERD_BTTW_TITLE_Y		(0)
#define MCV_USERD_BTTW_LAST_X		(8)
#define MCV_USERD_BTTW_LAST_Y		(24)
#define MCV_USERD_BTTW_LASTNUM_X	(64)
#define MCV_USERD_BTTW_LASTNUM_Y	(24)
#define MCV_USERD_BTTW_MAX_X		(8)
#define MCV_USERD_BTTW_MAX_Y		(48)
#define MCV_USERD_BTTW_MAXNUM_X		(64)
#define MCV_USERD_BTTW_MAXNUM_Y		(48)

// PAGE3
#define MCV_USERD_BTFC_TITLE_X			(8)
#define MCV_USERD_BTFC_TITLE_Y			(0)
#define MCV_USERD_BTFC_LV50_X			(8)
#define MCV_USERD_BTFC_LV50_Y			(24)
#define MCV_USERD_BTFC_LV50K_X			(136)
#define MCV_USERD_BTFC_LV50K_Y			(24)
#define MCV_USERD_BTFC_LV50LAST_X		(8)
#define MCV_USERD_BTFC_LV50LAST_Y		(48)
#define MCV_USERD_BTFC_LV50LASTNUM_X	(64)
#define MCV_USERD_BTFC_LV50LASTNUM_Y	(48)
#define MCV_USERD_BTFC_LV50LASTTRNUM_X	(184)
#define MCV_USERD_BTFC_LV50LASTTRNUM_Y	(48)
#define MCV_USERD_BTFC_LV50MAX_X		(8)
#define MCV_USERD_BTFC_LV50MAX_Y		(64)
#define MCV_USERD_BTFC_LV50MAXNUM_X		(64)
#define MCV_USERD_BTFC_LV50MAXNUM_Y		(64)
#define MCV_USERD_BTFC_LV50MAXTRNUM_X	(184)
#define MCV_USERD_BTFC_LV50MAXTRNUM_Y	(64)
#define MCV_USERD_BTFC_OPN_X			(8)
#define MCV_USERD_BTFC_OPN_Y			(88)
#define MCV_USERD_BTFC_OPNK_X			(136)
#define MCV_USERD_BTFC_OPNK_Y			(88)
#define MCV_USERD_BTFC_OPNLAST_X		(8)
#define MCV_USERD_BTFC_OPNLAST_Y		(112)
#define MCV_USERD_BTFC_OPNLASTNUM_X	(64)
#define MCV_USERD_BTFC_OPNLASTNUM_Y	(112)
#define MCV_USERD_BTFC_OPNLASTTRNUM_X	(184)
#define MCV_USERD_BTFC_OPNLASTTRNUM_Y	(112)
#define MCV_USERD_BTFC_OPNMAX_X		(8)
#define MCV_USERD_BTFC_OPNMAX_Y		(128)
#define MCV_USERD_BTFC_OPNMAXNUM_X		(64)
#define MCV_USERD_BTFC_OPNMAXNUM_Y		(128)
#define MCV_USERD_BTFC_OPNMAXTRNUM_X	(184)
#define MCV_USERD_BTFC_OPNMAXTRNUM_Y	(128)


// PAGE4
#define MCV_USERD_BTKS_TITLE_X		(8)
#define MCV_USERD_BTKS_TITLE_Y		(0)
#define MCV_USERD_BTKS_K_X			(136)
#define MCV_USERD_BTKS_K_Y			(24)
#define MCV_USERD_BTKS_LAST_X		(8)
#define MCV_USERD_BTKS_LAST_Y		(48)
#define MCV_USERD_BTKS_LASTNUM_X	(64)
#define MCV_USERD_BTKS_LASTNUM_Y	(48)
#define MCV_USERD_BTKS_LASTCPNUM_X	(184)
#define MCV_USERD_BTKS_LASTCPNUM_Y	(48)
#define MCV_USERD_BTKS_MAX_X		(8)
#define MCV_USERD_BTKS_MAX_Y		(80)
#define MCV_USERD_BTKS_MAXNUM_X		(64)
#define MCV_USERD_BTKS_MAXNUM_Y		(80)
#define MCV_USERD_BTKS_MAXCPNUM_X	(184)
#define MCV_USERD_BTKS_MAXCPNUM_Y	(80)

// PAGE5
#define MCV_USERD_BTST_TITLE_X		(8)
#define MCV_USERD_BTST_TITLE_Y		(0)
#define MCV_USERD_BTST_K_X			(8)
#define MCV_USERD_BTST_K_Y			(24)
#define MCV_USERD_BTST_LAST_X		(8)
#define MCV_USERD_BTST_LAST_Y		(48)
#define MCV_USERD_BTST_LASTNUM_X	(64)
#define MCV_USERD_BTST_LASTNUM_Y	(48)
#define MCV_USERD_BTST_MAX_X		(8)
#define MCV_USERD_BTST_MAX_Y		(80)
#define MCV_USERD_BTST_MAXNUM_X		(64)
#define MCV_USERD_BTST_MAXNUM_Y		(80)

// PAGE6
#define MCV_USERD_BTRT_TITLE_X		(8)
#define MCV_USERD_BTRT_TITLE_Y		(0)
#define MCV_USERD_BTRT_LAST_X		(8)
#define MCV_USERD_BTRT_LAST_Y		(24)
#define MCV_USERD_BTRT_LASTNUM_X	(64)
#define MCV_USERD_BTRT_LASTNUM_Y	(24)
#define MCV_USERD_BTRT_MAX_X		(8)
#define MCV_USERD_BTRT_MAX_Y		(48)
#define MCV_USERD_BTRT_MAXNUM_X		(64)
#define MCV_USERD_BTRT_MAXNUM_Y		(48)

// PAGE7
#define MCV_USERD_MINI_TITLE_X		(8)
#define MCV_USERD_MINI_TITLE_Y		(0)
#define MCV_USERD_MINI_BC_X			(8)		// ���܂���
#define MCV_USERD_MINI_BC_Y			(24)
#define MCV_USERD_MINI_BCNUM_X		(136)	// ���܂���@����
#define MCV_USERD_MINI_BCNUM_Y		(24)
#define MCV_USERD_MINI_BB_X			(8)		// ���܂̂�
#define MCV_USERD_MINI_BB_Y			(48)
#define MCV_USERD_MINI_BBNUM_X		(136)	// ���܂̂�@����
#define MCV_USERD_MINI_BBNUM_Y		(48)
#define MCV_USERD_MINI_BL_X			(8)		// �ӂ�����
#define MCV_USERD_MINI_BL_Y			(72)
#define MCV_USERD_MINI_BLNUM_X		(136)	// �ӂ�����@����
#define MCV_USERD_MINI_BLNUM_Y		(72)

// �t�����e�B�A��\���X�N���[���`��
#define MCV_USERD_NOFR_SCRN_X		( 0x1a )
#define MCV_USERD_NOFR_SCRN_Y		( 0 )
#define MCV_USERD_NOFR_SCRN_SIZX	( 0x1 )
#define MCV_USERD_NOFR_SCRN_SIZY	( 0x1 )

//  �t�����e�B�A�^�C�v
enum{
  MCV_FRONTIOR_TOWOR,
  MCV_FRONTIOR_FACTORY,
  MCV_FRONTIOR_FACTORY100,
  MCV_FRONTIOR_CASTLE,
  MCV_FRONTIOR_STAGE,
  MCV_FRONTIOR_ROULETTE,
  MCV_FRONTIOR_NUM,
} ;

#define VRANTRANSFERMAN_NUM	(32)	// VramTransferManager�^�X�N��


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

#define MCV_USERD_BTTN_RESCONTID	( 30 )	// ���\�[�XID
#define MCV_USERD_BTTN_BGPRI		(0)		// BG�D�揇��
#define MCV_USERD_BTTN_PRI			(128)		// BG�D�揇��
#define MCV_USERD_BTTN_Y			(172)	// �{�^��Y�ʒu
#define MCV_USERD_BTTN_LEFT_X		(40)	// ��
#define MCV_USERD_BTTN_BACK_X		(128)	// ���ǂ�
#define MCV_USERD_BTTN_RIGHT_X		(224)	// ��
#define MCV_USERD_BTTN_FONT_X		( -18 )	// ���ǂ�@�����ʒu
#define MCV_USERD_BTTN_FONT_Y		( -8 )	// ���ǂ�@�����ʒu
#define MCV_USERD_BTTN_FONT_SIZX	( 8 )	// ���ǂ�@�����`��͈�
#define MCV_USERD_BTTN_FONT_SIZY	( 2 )	// ���ǂ�@�����`��͈�
#define MCV_USERD_BTTN_FONT_CGXOFS	( 0 )	// CGX�I�t�Z�b�g
#define MCV_USERD_BTTN_FONT_COL		( 0 )	// �J���[�p���b�g
#define MCV_USERD_BTTN_FONT_OAMPRI	( 0 )
#define MCV_USERD_BTTN_LEFT_SIZX	( 56 )	// ���T�C�Y
#define MCV_USERD_BTTN_BACK_SIZX	( 96 )	// ���ǂ�T�C�Y
#define MCV_USERD_BTTN_RIGHT_SIZX	( 56 )	// ��������
#define MCV_USERD_BTTN_SIZY			( 32 )	// �c������
#define MCV_USERD_BTTN_LEFT_HIT_X	( 8 )	// �������蔻��pX
#define MCV_USERD_BTTN_BACK_HIT_X	( 80 )	// �������蔻��pX
#define MCV_USERD_BTTN_RIGHT_HIT_X	( 192 )	// �������蔻��pX
#define MCV_USERD_BTTN_RIGHT_HIT_Y	( 160 )	// �������蔻��pY
#define MCV_USERD_BTTN_ANMPUSHOK	( 3 )	// �������Ɣ��f����{�^���̃A�j��
#define MCV_USERD_BTTN_ANMMAX		( 5 )	// �A�j�����t���[����
#define MCV_USERD_BTTN_ANMMAX_0ORG	( MCV_USERD_BTTN_ANMMAX-1 )	// �A�j�����t���[����

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

static const GFL_UI_TP_HITTBL	c_MCV_USER_BTTN_OAM_Hit[ ] = {
  {	// ��
    MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
    MCV_USERD_BTTN_LEFT_HIT_X, MCV_USERD_BTTN_LEFT_HIT_X+MCV_USERD_BTTN_LEFT_SIZX,
  },
  {	// ���ǂ�
    MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
    MCV_USERD_BTTN_BACK_HIT_X, MCV_USERD_BTTN_BACK_HIT_X+MCV_USERD_BTTN_BACK_SIZX,
  },
  {	// ��
    MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
    MCV_USERD_BTTN_RIGHT_HIT_X, MCV_USERD_BTTN_RIGHT_HIT_X+MCV_USERD_BTTN_RIGHT_SIZX,
  },
  {GFL_UI_TP_HIT_END,0,0,0},		 //�I���f�[�^
};
enum{
  MCV_USERD_BTTN_RET_NONE,	// ���̔������Ȃ�
  MCV_USERD_BTTN_RET_LEFT,	// ���������ꂽ
  MCV_USERD_BTTN_RET_BACK,	// ���ǂ邪�����ꂽ
  MCV_USERD_BTTN_RET_RIGHT,	// �E�������ꂽ
};


static GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

  //        GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
  GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_32K,

};

#ifdef WFP2P_DEBUG	// �l�������ς��o��
//#define WFP2PM_MANY_OBJ
#endif


static void _debugChangeState(WIFIP2PMATCH_WORK* wk, int state, int line)
{
  NET_PRINT("p2p: %d\n",line);
  wk->seq = state;
}

#if 1
#define   _CHANGESTATE(wk,state) _debugChangeState(wk,state,__LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGESTATE(wk,state)  wk->seq = state
#endif //GFL_NET_DEBUG

static int _seqBackup;

//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static WIFI_STATUS* WifiFriendMatchStatusGet( u32 idx );
static u8 WifiDwc_getFriendStatus( int idx );

static BOOL WifiP2PMatch_CommWifiBattleStart( WIFIP2PMATCH_WORK* wk, int friendno, int status );

/*** �֐��v���g�^�C�v ***/
static void VBlankFunc( GFL_TCB *tcb , void * work );
static void VramBankSet(void);
static void BgInit( HEAPID heapID );
static void InitWork( WIFIP2PMATCH_WORK *wk );
static void FreeWork( WIFIP2PMATCH_WORK *wk );
static void BgExit( void );
static void BgGraphicSet( WIFIP2PMATCH_WORK * wk, ARCHANDLE* p_handle );
static void char_pltt_manager_init(void);
static void InitCellActor(WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle);
static void SetCellActor(WIFIP2PMATCH_WORK *wk);
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
static void WifiP2PMatchFriendListStIconWrite( WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 status );
static void WifiP2PMatchFriendListBmpIconWrite(  GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 icon_type, u32 col,int pal );
static void WifiP2PMatchFriendListBmpStIconWrite( GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 status,int pal );
static int WifiP2PMatchBglFrmIconPalGet( u32 frm );

static int WifiP2PMatchFriendListStart( void );

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
static BOOL MCVSys_UserDispEndCheck( WIFIP2PMATCH_WORK *wk, u32 oambttn_ret );
static u32	MCVSys_UserDispGetFriend( const WIFIP2PMATCH_WORK* cp_wk );
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
static void MCVSys_UserDispDrawType01( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType02( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType03( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType04( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType05( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType06( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawFrontierOffScrn( WIFIP2PMATCH_WORK *wk );
static void MCVSys_UserDispFrontiorNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 factoryid, u32 friendno, u32 x, u32 y );
static void MCVSys_UserDispFrontiorTitleStrGet( WIFIP2PMATCH_WORK *wk, STRBUF* p_str, u32 factoryid, u32 friendno );
static void MCVSys_UserDispNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 num, u32 x, u32 y );
static void MCVSys_BttnWrite( WIFIP2PMATCH_WORK *wk, u8 cx, u8 cy, u8 type, u8 frame );
static u32 MCVSys_StatusMsgIdGet( u32 status, int* col );
static void MCVSys_BttnWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN* p_bmp, u32 friendNo, u32 frame, u32 area_id );
static void MCVSys_BttnStatusWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN** p_stbmp, u32 friendNo, u32 frame, u32 area_id );
static void MCVSys_OamBttnInit( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID );
static void MCVSys_OamBttnDelete( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOn( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOnNoBack( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOff( WIFIP2PMATCH_WORK *wk );
static u32 MCVSys_OamBttnMain( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnCallBack( u32 bttn_no, u32 event, void* p_work );
static void MCVSys_OamBttnObjAnmStart( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no );
static BOOL MCVSys_OamBttnObjAnmMain( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no, u32 push_bttn, u32 event );
static void MCVSys_FriendNameSet( WIFIP2PMATCH_WORK* p_wk, int friendno );

static void BmpWinDelete( WIFIP2PMATCH_WORK *wk );
static void _systemMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno );
static void WifiP2PMatchMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno , BOOL bSystem);
static void EndMessageWindowOff( WIFIP2PMATCH_WORK *wk );
static void WifiP2PMatchDataSendRecv( WIFIP2PMATCH_WORK *wk );
static void SequenceChange_MesWait( WIFIP2PMATCH_WORK *wk, int next );
static void _myStatusChange(WIFIP2PMATCH_WORK *wk, int status);
static void _myStatusChange_not_send(WIFIP2PMATCH_WORK *wk, int status);
static void _timeWaitIconDel(WIFIP2PMATCH_WORK *wk);
static void _friendNameExpand( WIFIP2PMATCH_WORK *wk, int friendNo);


static void WifiP2P_SetLobbyBgm( void );
static BOOL WifiP2P_CheckLobbyBgm( void );


//static void WifiP2P_Fnote_Set( WIFIP2PMATCH_WORK* wk, u32 idx );

static void _errorDisp(WIFIP2PMATCH_WORK* wk);

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
static int _poweroffInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _poweroffYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _poweroffWait( WIFIP2PMATCH_WORK *wk, int seq );
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
static int WifiP2PMatch_FriendList( WIFIP2PMATCH_WORK *wk, int seq );
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

static BOOL _myVChatStatusToggle(WIFIP2PMATCH_WORK *wk);
static BOOL _myVChatStatusToggleOrg(WIFIP2PMATCH_WORK *wk);
static BOOL _myVChatStatusOrgSet(WIFIP2PMATCH_WORK *wk);
static int _vchatToggleWait( WIFIP2PMATCH_WORK *wk, int seq );


static int WifiP2PMatch_EndWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CheckAndEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CancelEnableWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CancelEnableWaitDP( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FirstSaving2( WIFIP2PMATCH_WORK *wk, int seq );


static DWCUserData* _getMyUserData(void* pWork)
{
  WIFIP2PMATCH_WORK *wk = (WIFIP2PMATCH_WORK*)pWork;
  return WifiList_GetMyUserInfo(SaveData_GetWifiListData(wk->pSaveData));
}

static DWCFriendData* _getFriendData(void* pWork)
{
  WIFIP2PMATCH_WORK *wk = (WIFIP2PMATCH_WORK*)pWork;
  //    NET_PRINT("Friend %d\n",WifiList_GetFriendDataNum(SaveData_GetWifiListData(wk->pSaveData)));
  return WifiList_GetDwcDataPtr(SaveData_GetWifiListData(wk->pSaveData),0);
}

static void _deleteFriendList(int deletedIndex,int srcIndex, void* pWork)
{
  WIFIP2PMATCH_WORK *wk = (WIFIP2PMATCH_WORK*)pWork;

  WifiList_DataMarge(SaveData_GetWifiListData(wk->pSaveData), deletedIndex, srcIndex);
}


// ���R�[�h�R�[�i�[���C���V�[�P���X�p�֐��z���`
static int (*FuncTable[])(WIFIP2PMATCH_WORK *wk, int seq)={
  WifiP2PMatch_MainInit,	// WIFIP2PMATCH_MODE_INIT  = 0,
  _normalConnectYesNo,    // WIFIP2PMATCH_NORMALCONNECT_YESNO
  _normalConnectWait,     // WIFIP2PMATCH_NORMALCONNECT_WAIT
  _differMachineInit,     // WIFIP2PMATCH_DIFFER_MACHINE_INIT
  _differMachineNext,     // WIFIP2PMATCH_DIFFER_MACHINE_NEXT
  _differMachineOneMore,  // WIFIP2PMATCH_DIFFER_MACHINE_ONEMORE
  _firstYesNo,            //WIFIP2PMATCH_FIRST_YESNO
  _poweroffInit,      // WIFIP2PMATCH_POWEROFF_INIT
  _poweroffYesNo,     // WIFIP2PMATCH_POWEROFF_YESNO
  _poweroffWait,      // WIFIP2PMATCH_POWEROFF_WAIT
  _retryInit,        //WIFIP2PMATCH_RETRY_INIT
  _retryYesNo,        //WIFIP2PMATCH_RETRY_YESNO
  _retryWait,         //WIFIP2PMATCH_RETRY_WAIT
  _retry,             //WIFIP2PMATCH_RETRY
  WifiP2PMatch_ConnectingInit,        // WIFIP2PMATCH_CONNECTING_INIT,
  WifiP2PMatch_Connecting,            // WIFIP2PMATCH_CONNECTING,
  _firstConnectEndMsg,                 //WIFIP2PMATCH_FIRST_ENDMSG
  _firstConnectEndMsgWait,    //WIFIP2PMATCH_FIRST_ENDMSG_WAIT
  WifiP2PMatch_FriendListInit,        // WIFIP2PMATCH_FRIENDLIST_INIT,
  WifiP2PMatch_FriendList,            // WIFIP2PMATCH_MODE_FRIENDLIST
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
  WifiP2PMatch_EndWait,			    // WIFIP2PMATCH_MODE_END_WAIT,
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
  0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
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
  IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
    0,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
};




static const u8 ViewButtonFrame_y[ 4 ] = {
  8, 7, 5, 7
  };


static const BMPWIN_YESNO_DAT _yesNoBmpDat = {
  GFL_BG_FRAME2_M, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY,
  FLD_YESNO_WIN_PAL, FLD_YESNO_WIN_CGX
  };

// �t�����e�B�A�t���O�������Ă��邩�`�F�b�N
static BOOL _frontierInCheck( WIFIP2PMATCH_WORK * wk )
{
  return SysFlag_ArriveGet(SaveData_GetEventWork(wk->pSaveData),FLAG_ARRIVE_D32R0101);
}

// �~�j�Q�[���t���O�������Ă��邩�`�F�b�N
static BOOL _miniGameInCheck( WIFIP2PMATCH_WORK * wk )
{
  return TRUE;	// �~�j�Q�[���͏�Ɍ������Ԃɂ���
}

// �����X�e�[�^�X���擾����
static u32 _WifiMyStatusGet( WIFIP2PMATCH_WORK * p_wk, WIFI_STATUS* p_status )
{
  return WIFI_STATUS_GetWifiMode(p_status);
}



// �X�e�[�g��BATTLEWAIT���ǂ���
static BOOL _modeBattleWait(int status)
{
  switch(status){
  case WIFI_STATUS_SBATTLE50_WAIT:   // �V���O��Lv50�ΐ��W��
  case WIFI_STATUS_SBATTLE100_WAIT:   // �V���O��Lv100�ΐ��W��
  case WIFI_STATUS_SBATTLE_FREE_WAIT:   // �V���O��Lv100�ΐ��W��
  case WIFI_STATUS_DBATTLE50_WAIT:   // �_�u��Lv50�ΐ��W��
  case WIFI_STATUS_DBATTLE100_WAIT:   // �_�u��Lv100�ΐ��W��
  case WIFI_STATUS_DBATTLE_FREE_WAIT:   // �_�u��Lv100�ΐ��W��
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE_WAIT:   // �}���`�o�g���@�t���[�ΐ��W��
#endif
    return TRUE;
  default:
    return FALSE;
  }
}

// �X�e�[�g��WAIT���ǂ���
static BOOL _modeWait(int status)
{
  if(_modeBattleWait(status)){
    return TRUE;
  }
  switch(status){
  case WIFI_STATUS_TRADE_WAIT:    // ������W��
  case WIFI_STATUS_FRONTIER_WAIT:    // �t�����e�B�A��W��
    return TRUE;
  }
  return FALSE;
}

// �X�e�[�g��BATTLE���ǂ���
static BOOL _modeBattle(int status)
{
  switch(status){
  case WIFI_STATUS_SBATTLE50:      // �V���O���ΐ풆
  case WIFI_STATUS_SBATTLE100:      // �V���O���ΐ풆
  case WIFI_STATUS_SBATTLE_FREE:      // �V���O���ΐ풆
  case WIFI_STATUS_DBATTLE50:      // �_�u���ΐ풆
  case WIFI_STATUS_DBATTLE100:      // �_�u���ΐ풆
  case WIFI_STATUS_DBATTLE_FREE:      // �_�u���ΐ풆

    return TRUE;
  default:
    return FALSE;
  }
}

// �X�e�[�g��ACTIVE���ǂ���
static BOOL _modeActive(int status)
{
  if(_modeBattle(status)){
    return TRUE;
  }
  switch(status){
  case WIFI_STATUS_TRADE:    // ������
  case WIFI_STATUS_FRONTIER:    // �t�����e�B�A��
  case WIFI_STATUS_VCT:    // ������
    return TRUE;
  }
  return FALSE;
}




static BOOL _is2pokeMode(int status)
{
  switch(status){
  case WIFI_STATUS_TRADE_WAIT:
  case WIFI_STATUS_DBATTLE50_WAIT:
  case WIFI_STATUS_DBATTLE100_WAIT:
  case WIFI_STATUS_DBATTLE_FREE_WAIT:
    return TRUE;
  }
  return FALSE;
}

static int _convertState(int state)
{
  int ret = WIFI_STATUS_UNKNOWN;

  switch(state){
  case WIFI_STATUS_FRONTIER_WAIT:
    ret = WIFI_STATUS_FRONTIER;
    break;
  case WIFI_STATUS_TRADE_WAIT:
    ret = WIFI_STATUS_TRADE;
    break;
  case WIFI_STATUS_SBATTLE50_WAIT:
    ret = WIFI_STATUS_SBATTLE50;
    break;
  case WIFI_STATUS_SBATTLE100_WAIT:
    ret = WIFI_STATUS_SBATTLE100;
    break;
  case WIFI_STATUS_SBATTLE_FREE_WAIT:
    ret = WIFI_STATUS_SBATTLE_FREE;
    break;
  case WIFI_STATUS_DBATTLE50_WAIT:
    ret = WIFI_STATUS_DBATTLE50;
    break;
  case WIFI_STATUS_DBATTLE100_WAIT:
    ret = WIFI_STATUS_DBATTLE100;
    break;
  case WIFI_STATUS_DBATTLE_FREE_WAIT:
    ret = WIFI_STATUS_DBATTLE_FREE;
    break;
  case WIFI_STATUS_LOGIN_WAIT:
    ret = WIFI_STATUS_VCT;
    break;
  case WIFI_STATUS_FRONTIER:
    ret = WIFI_STATUS_FRONTIER_WAIT;
    break;
  case WIFI_STATUS_TRADE:
    ret = WIFI_STATUS_TRADE_WAIT;
    break;
  case WIFI_STATUS_SBATTLE50:
    ret = WIFI_STATUS_SBATTLE50_WAIT;
    break;
  case WIFI_STATUS_SBATTLE100:
    ret = WIFI_STATUS_SBATTLE100_WAIT;
    break;
  case WIFI_STATUS_SBATTLE_FREE:
    ret = WIFI_STATUS_SBATTLE_FREE_WAIT;
    break;
  case WIFI_STATUS_DBATTLE50:
    ret = WIFI_STATUS_DBATTLE50_WAIT;
    break;
  case WIFI_STATUS_DBATTLE100:
    ret = WIFI_STATUS_DBATTLE100_WAIT;
    break;
  case WIFI_STATUS_DBATTLE_FREE:
    ret = WIFI_STATUS_DBATTLE_FREE_WAIT;
    break;
  case WIFI_STATUS_VCT:
    ret = WIFI_STATUS_LOGIN_WAIT;
    break;
  }
  return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�ɂ��킹�ĒʐM�X�e�[�g���ύX����
 * @param	status
 * @return	none
 */
//--------------------------------------------------------------------------------------------

static void _commStateChange(int status)
{
  if(status == WIFI_STATUS_LOGIN_WAIT){
    //        GFL_NET_StateChangeWiFiLogin();   // @@OO ���O�C���p
    GFL_NET_SetWifiBothNet(FALSE);
    GFL_NET_ChangeInitStruct(&aGFLNetInit);
  }
  else if( (status == WIFI_STATUS_FRONTIER_WAIT) || (status == WIFI_STATUS_FRONTIER) || (status == WIFI_STATUS_VCT) ){
    GFL_NET_SetWifiBothNet(FALSE);
    //		GFL_NET_StateChangeWiFiFactory();
  }
  else{
    GFL_NET_SetWifiBothNet(TRUE); // wifi�̒ʐM��񓯊����瓯����
    //        GFL_NET_StateChangeWiFiBattle();
  }


}

//============================================================================================
//	�v���Z�X�֐�
//============================================================================================

static void _graphicInit(WIFIP2PMATCH_WORK * wk)
{
  ARCHANDLE* p_handle;

  // sys_VBlankFuncChange( NULL, NULL );	// VBlank�Z�b�g
  //    sys_HBlankIntrStop();	//HBlank���荞�ݒ�~
  // Vblank���Ԓ���BG�]���I��
  if(wk->vblankFunc){
    GFL_TCB_DeleteTask( wk->vblankFunc );
  }

  GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
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
  wk->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_WIFIP2PMATCH );

  GFL_DISP_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

  // VRAM �o���N�ݒ�
  VramBankSet();

  // BGL���W�X�^�ݒ�
  BgInit(HEAPID_WIFIP2PMATCH );
  GFL_BMPWIN_Init( HEAPID_WIFIP2PMATCH );

  //BG�O���t�B�b�N�Z�b�g
  BgGraphicSet( wk, p_handle );

  //�p���b�g�A�j���V�X�e���쐬
  ConnectBGPalAnm_Init(&wk->cbp, p_handle, NARC_wifip2pmatch_conect_anm_NCLR, HEAPID_WIFIP2PMATCH);

  // VBlank�֐��Z�b�g
  //    sys_VBlankFuncChange( VBlankFunc, wk );


  wk->vblankFunc = GFUser_VIntr_CreateTCB( VBlankFunc , wk , 1 );


  // OBJ�L�����A�p���b�g�}�l�[�W���[������
  char_pltt_manager_init();

  // CellActor�V�X�e��������
  InitCellActor(wk, p_handle);

  // CellActro�\���o�^
  SetCellActor(wk);

  GFL_ARC_CloseDataHandle( p_handle );
}


//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param	proc	�v���Z�X�f�[�^
 * @param	seq		�V�[�P���X
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT WifiP2PMatchProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFIP2PMATCH_WORK * wk = mywk;
  WIFIP2PMATCH_PROC_PARAM* pParentWork = pwk;
  u32 result;

  switch(*seq){
  case 0:

    // �ʐM�g���q�[�v�̃������m��
    // 080611	tomoya �ǉ�
    if(GFL_NET_IsInit()){       // �ڑ���
      switch( pParentWork->seq ){	// 1�O�̗V�тŒʐM�g���q�[�v��j�����Ă�����Ċm��
      case WIFI_P2PMATCH_END:
      case WIFI_P2PMATCH_UTIL:
      case WIFI_P2PMATCH_DPW_END:
      case WIFI_P2PMATCH_SBATTLE50:
      case WIFI_P2PMATCH_SBATTLE100:
      case WIFI_P2PMATCH_SBATTLE_FREE:
      case WIFI_P2PMATCH_DBATTLE50:
      case WIFI_P2PMATCH_DBATTLE100:
      case WIFI_P2PMATCH_DBATTLE_FREE:
      case WIFI_P2PMATCH_FRONTIER:
      case WIFI_P2PMATCH_TRADE:
        // �o�g������߂��Ă����Ȃ�g���q�[�v�쐬
        GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIP2PMATCHEX, WIFI_P2PMATCH_EXHEAP_SIZE );
        break;

      default:
        break;
      }
    }else{
      // �ʐM�O�Ȃ��΂Ɋg���q�[�v���m��
      GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIP2PMATCHEX, WIFI_P2PMATCH_EXHEAP_SIZE );
    }

    // wifi_2dmap�I�[�o�[���C�Ǎ���
    //		GFL_OVERLAY_Load( FS_OVERLAY_ID(wifi_2dmapsys));

    if(pParentWork->seq == WIFI_P2PMATCH_DPW){ // ���E�ΐ�Ȃ�
      GFL_HEAP_CreateHeapLo( GFL_HEAPID_APP, HEAPID_WIFIP2PMATCH, 0x70000 );
    }
    else{
      //            GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIP2PMATCH, 0xa0000 );
      GFL_HEAP_CreateHeapLo( GFL_HEAPID_APP, HEAPID_WIFIP2PMATCH, 0xa0000 );
    }
    wk = GFL_PROC_AllocWork( proc, sizeof(WIFIP2PMATCH_WORK), HEAPID_WIFIP2PMATCH );
    MI_CpuFill8( wk, 0, sizeof(WIFIP2PMATCH_WORK) );

    // Vram�]���}�l�[�W���쐬
#if WB_FIX
    initVramTransferManagerHeap( VRANTRANSFERMAN_NUM, HEAPID_WIFIP2PMATCH );
#endif

    //        wk->MsgIndex = _PRINTTASK_MAX;
    wk->pMatch = pParentWork->pMatch;
    wk->pSaveData = pParentWork->pSaveData;

    wk->pMyPoke = SaveData_GetTemotiPokemon(pParentWork->pSaveData);
    wk->pList = SaveData_GetWifiListData(pParentWork->pSaveData);
    wk->pConfig = SaveData_GetConfig(pParentWork->pSaveData);
    wk->initSeq = pParentWork->seq;    // P2P��DPW��
    wk->endSeq = WIFI_P2PMATCH_END;
    wk->preConnect = -1;

    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );

    // ���[�N������
    InitWork( wk );

    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16,16,0);

    // �O���t�B�b�N������
    _graphicInit(wk);
    // ���C�v�t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,
                                16,0,COMM_BRIGHTNESS_SYNC);

    if(GFL_NET_IsInit()){
      GFL_NET_ReloadIcon();  // �ڑ����Ȃ̂ŃA�C�R���\��
    }
    // �^�b�`�p�l���J�n
    //		result = InitTPNoBuff( 4 );
    //		GF_ASSERT( result == TP_OK );

    (*seq)++;
    break;
  case 1:
    //wk = GFL_PROC_GetWork( proc );
    (*seq) = SEQ_IN;
    return GFL_PROC_RES_FINISH;
    break;
  }
  return GFL_PROC_RES_CONTINUE;
}




//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F���C��
 *
 * @param	proc	�v���Z�X�f�[�^
 * @param	seq		�V�[�P���X
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------

static GFL_PROC_RESULT WifiP2PMatchProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFIP2PMATCH_WORK * wk  = mywk;

  switch( *seq ){
  case SEQ_IN:
    if( !GFL_FADE_CheckFade() ){
      // ���C�v�����҂�
      *seq = SEQ_MAIN;


    }
    break;

  case SEQ_MAIN:

#ifdef _WIFI_DEBUG_TUUSHIN
    if( WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_MODE != _WIFI_DEBUG_NONE ){
      GFL_UI_KEY_GetTrg() |= PAD_KEY_DOWN;	//  ������������
      sys.cont |= PAD_KEY_DOWN;
      GFL_UI_KEY_GetTrg() |= PAD_BUTTON_A;
      sys.cont |= PAD_BUTTON_A;
    }
#endif


    // �J�[�\���ړ�

    // �V�[�P���X���̓���
    if(FuncTable[wk->seq]!=NULL){
      //   static int seqBk=0;

      //          if(seqBk != wk->seq){
      //                OHNO_PRINT("wp2p %d \n",wk->seq);
      //                NET_PRINT("wp2p %d \n",wk->seq);
      //            }
      //          seqBk = wk->seq;

      *seq = (*FuncTable[wk->seq])( wk, *seq );
    }

    break;

  case SEQ_OUT:
    if( !GFL_FADE_CheckFade() ){
      return GFL_PROC_RES_FINISH;
    }
    break;
  }
#if 1
  if(wk->clactSet){
    GFL_CLACT_SYS_Main();			// �Z���A�N�^�[�풓�֐�
    //      CLACT_Draw( wk->clactSet );									// �Z���A�N�^�[�풓�֐�
  }
#endif
  if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
    WIFI_MCR_Draw( &wk->matchroom );
  }

  ConnectBGPalAnm_Main(&wk->cbp);

  return GFL_PROC_RES_CONTINUE;
}

#define DEFAULT_NAME_MAX		18

//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�ɂ�����镔���̏I������
 * @param	wk	WIFIP2PMATCH_WORK
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void _graphicEnd(WIFIP2PMATCH_WORK* wk)
{
  int i;

  //    sys_VBlankFuncChange( NULL, NULL );	// VBlankReset
  // Vblank���Ԓ���BG�]���I��
  if(wk->vblankFunc){
    GFL_TCB_DeleteTask( wk->vblankFunc );
  }
  ConnectBGPalAnm_End(&wk->cbp);

  // �}�b�`���O���[���j��
  if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
    WIFI_MCR_Dest( &wk->matchroom );
  }
  // �r���[�A�[�j��
  if( MCVSys_MoveCheck( wk ) == TRUE ){
    MCVSys_Exit( wk );
  }

  // �A�C�R���j��
  WifiP2PMatchFriendListIconRelease( &wk->icon );

#if 0
  // �L�����]���}�l�[�W���[�j��
  CLACT_U_CharManagerDelete(wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES]);

  // �p���b�g�]���}�l�[�W���[�j��
  CLACT_U_PlttManagerDelete(wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES]);
#endif

  // FontOAM�j��
  //FONTOAM_SysDelete( wk->fontoam );  @@OO

  // �L�����E�p���b�g�E�Z���E�Z���A�j���̃��\�[�X�}�l�[�W���[�j��
  //    for(i=0;i<CLACT_RESOURCE_NUM;i++){
  //        CLACT_U_ResManagerDelete(wk->resMan[i]);
  //    }
  // �Z���A�N�^�[�Z�b�g�j��
  GFL_CLACT_UNIT_Delete(wk->clactSet);

  //OAM�����_���[�j��
  //    REND_OAM_Delete();
  GFL_CLACT_SYS_Delete();//

  // ���\�[�X���
  //    DeleteCharManager();
  //    DeletePlttManager();

  // BMP�E�B���h�E�J��
  BmpWinDelete( wk );

  GFL_BMPWIN_Exit();
  // BGL�폜
  GFL_BG_Exit( );

  // �e�̃v���b�N�̏���
  //    UnionRoomView_ObjInit( (COMM_UNIONROOM_VIEW*)GFL_PROC_GetParentWork( proc ) );

  // �^�b�`�p�l���V�X�e���I��
  //	StopTP();


  // ���b�Z�[�W�}�l�[�W���[�E���[�h�Z�b�g�}�l�[�W���[���
  GFL_MSG_Delete( wk->SysMsgManager );
  GFL_MSG_Delete( wk->MsgManager );
  WORDSET_Delete( wk->WordSet );

  GFL_FONT_Delete(wk->fontHandle);

  // �I�[�o�[���C�j��
  //	GFL_OVERLAY_Unload( FS_OVERLAY_ID(wifi_2dmapsys) );
}


//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I��
 *
 * @param	proc	�v���Z�X�f�[�^
 * @param	seq		�V�[�P���X
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT WifiP2PMatchProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFIP2PMATCH_WORK  *wk    = mywk;
  WIFIP2PMATCH_PROC_PARAM* pParentWork = pwk;


  _graphicEnd(wk);

  pParentWork->seq = wk->endSeq;
  //    if( (wk->endSeq == WIFI_P2PMATCH_END) || (wk->endSeq == WIFI_P2PMATCH_DPW_END) ){  // �I�[�o�[���C����
  //      DwcOverlayEnd();
  //    }
  //  else{

  //    pParentWork->targetID = GFL_NET_DWC_GetFriendIndex();
  //    }

  if(GFL_NET_IsInit()){
    pParentWork->targetID = GFL_NET_DWC_GetFriendIndex();
  }

  // ���[�N���
  FreeWork( wk );

  GFL_PROC_FreeWork( proc );				// GFL_PROC���[�N�J��

  // �^�b�`�p�l����~
  //	StopTP();

#if WB_TEMP_FIX
  // VramTransfer�}�l�[�W���j��
  DellVramTransferManager();
#endif

  GFL_HEAP_DeleteHeap( HEAPID_WIFIP2PMATCH );
  //    WIPE_ResetBrightness( WIPE_DISP_MAIN );
  //  WIPE_ResetBrightness( WIPE_DISP_SUB );


  // �g���q�[�v�̔j��
  // �ʐM�̏I���ƁA�o�g�����͊g���q�[�v��j������
  // ����ȊO�͔j�����Ȃ�
  switch( wk->endSeq ){
  case WIFI_P2PMATCH_END:
  case WIFI_P2PMATCH_UTIL:
  case WIFI_P2PMATCH_DPW_END:
  case WIFI_P2PMATCH_SBATTLE50:
  case WIFI_P2PMATCH_SBATTLE100:
  case WIFI_P2PMATCH_SBATTLE_FREE:
  case WIFI_P2PMATCH_DBATTLE50:
  case WIFI_P2PMATCH_DBATTLE100:
  case WIFI_P2PMATCH_DBATTLE_FREE:
  case WIFI_P2PMATCH_FRONTIER:
  case WIFI_P2PMATCH_TRADE:

    GFL_HEAP_DeleteHeap( HEAPID_WIFIP2PMATCHEX );
    break;

  default:
    break;
  }


  // BGM���|�P�Z���̂܂܂Ȃ�BGM���ʂ�ύX����
  if( WifiP2P_CheckLobbyBgm() == TRUE ){
    Snd_PlayerSetInitialVolume( SND_HANDLE_FIELD, BGM_POKECEN_VOL );
  }

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * VBlank�֐�
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//#if 0
static void VBlankFunc( GFL_TCB *tcb , void * work )
{
  WIFIP2PMATCH_WORK* wk = work;

  // �Z���A�N�^�[
  // Vram�]���}�l�[�W���[���s
  //  DoVramTransferManager();

  // �����_�����LOAM�}�l�[�W��Vram�]��
  //REND_OAMTrans();

  //ConnectBGPalAnm_VBlank(&wk->cbp);

  // _retry�֐����Ń}�X�^�[�P�x��ݒ肵�āA
  // ���ꂢ�ɕ��A�ł���悤�ɂ��邽�߂����ŕ��A
  //	if( wk->brightness_reset == TRUE ){
  //		WIPE_ResetBrightness( WIPE_DISP_MAIN );
  //		WIPE_ResetBrightness( WIPE_DISP_SUB );
  //		wk->brightness_reset = FALSE;
  //	}

  // OS_SetIrqCheckFlag( OS_IE_V_BLANK );

  GFL_CLACT_SYS_VBlankFunc();

}
//#endif

//--------------------------------------------------------------------------------------------
/**
 * VRAM�ݒ�
 *
 * @param	none
 *
 * @return	none
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
 * @param	ini		BGL�f�[�^
 *
 * @return	none
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
      0, 0, 0, FALSE
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
 *	@brief	������������I�u�W�F�N�g���[�N��ݒ肷��
 *
 *	@param	wk		���[�N
 *	@param	p_obj	�I�u�W�F�N�g���[�N
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
 *	@brief	������OBJ�������Ă��郏�[�N��NULL�ɂ���
 *
 *	@param	wk		���[�N
 *	@param	p_obj	NULL�����Ăق������[�N�ɓ����Ă���|�C���^
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
 *	@brief	friendNO�̃I�u�W�F�N�g���[�N���擾����
 *
 *	@param	wk			���[�N
 *	@param	friendNo	friendNO
 *
 *	@return	�I�u�W�F�N�g���[�N
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
 *	@brief	�A�C�R���f�[�^�Ǎ���
 *
 *	@param	p_data		�f�[�^�i�[��
 *	@param	p_handle	�n���h��
 *	@param	heapID		�q�[�vID
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

  // �L�����N�^�f�[�^�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCGR,
                                        GFL_BG_FRAME1_M, PLAYER_DISP_ICON_CGX, 0, FALSE, heapID );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCGR,
                                        GFL_BG_FRAME2_S, PLAYER_DISP_ICON_CGX, 0, FALSE, heapID );

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
 *	@brief	Icon�f�[�^�͂�
 *
 *	@param	p_data	���[�N
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
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�C�R������������
 *
 *	@param	p_data		�f�[�^
 *	@param	frm			�t���[��
 *	@param	cx			�L�����N�^�����W
 *	@param	cy			�L�����N�^�����W
 *	@param	icon_type	�A�C�R���^�C�v
 *	@param	col			0=�D 1=��
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListIconWrite(WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 icon_type, u32 col )
{
  int pal;
  // ��������
  GFL_BG_WriteScreenExpand(frm, cx, cy,
                           PLAYER_DISP_ICON_SCRN_X, PLAYER_DISP_ICON_SCRN_Y,
                           p_data->p_scrn->rawData,
                           PLAYER_DISP_ICON_SCRN_X*icon_type, 0,
                           p_data->p_scrn->screenWidth/8, p_data->p_scrn->screenHeight/8 );


  // �p���b�g
  pal = WifiP2PMatchBglFrmIconPalGet( frm );

  // �p���b�g�i���o�[�����킹��
  GFL_BG_ChangeScreenPalette( frm, cx, cy,
                              PLAYER_DISP_ICON_SCRN_X, PLAYER_DISP_ICON_SCRN_Y, pal+col );

  // �]��
  GFL_BG_LoadScreenV_Req( frm );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Icon��������
 *
 *	@param	p_data		�f�[�^
 *	@param	frm			�t���[��
 *	@param	cx			�L�����N�^�����W
 *	@param	cy			�L�����N�^�����W
 *	@param	status		���
 *	@param	vctIcon		�{�C�X�`���b�gONOFF�t���O
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListStIconWrite(  WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 status )
{
  u8 col=0;
  u8 scrn_idx=0;

  switch( status ){
  case WIFI_STATUS_VCT:      // VCT��
    scrn_idx = PLAYER_DISP_ICON_IDX_VCT;
    break;

  case WIFI_STATUS_SBATTLE50_WAIT:   // �V���O��Lv50�ΐ��W��
  case WIFI_STATUS_SBATTLE100_WAIT:   // �V���O��Lv100�ΐ��W��
  case WIFI_STATUS_SBATTLE_FREE_WAIT:   // �V���O��Lv100�ΐ��W��
  case WIFI_STATUS_DBATTLE50_WAIT:   // �_�u��Lv50�ΐ��W��
  case WIFI_STATUS_DBATTLE100_WAIT:   // �_�u��Lv100�ΐ��W��
  case WIFI_STATUS_DBATTLE_FREE_WAIT:   // �_�u��Lv100�ΐ��W��
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE_WAIT:// �}���`�o�g����W��
#endif
    col = 1;
  case WIFI_STATUS_SBATTLE50:      // �V���O���ΐ풆
  case WIFI_STATUS_SBATTLE100:      // �V���O���ΐ풆
  case WIFI_STATUS_SBATTLE_FREE:      // �V���O���ΐ풆
  case WIFI_STATUS_DBATTLE50:      // �_�u���ΐ풆
  case WIFI_STATUS_DBATTLE100:      // �_�u���ΐ풆
  case WIFI_STATUS_DBATTLE_FREE:      // �_�u���ΐ풆
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE:     // �}���`�o�g����
#endif
    scrn_idx = PLAYER_DISP_ICON_IDX_FIGHT;
    break;


  case WIFI_STATUS_TRADE_WAIT:    // ������W��
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_BATTLEROOM_WAIT:// �o�g�����[����W��
#endif
    col = 1;
  case WIFI_STATUS_TRADE:          // ������
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_BATTLEROOM:     // �o�g�����[����
#endif
    scrn_idx = PLAYER_DISP_ICON_IDX_CHANGE;
    break;


  case WIFI_STATUS_FRONTIER_WAIT:    // �t�����e�B�A��W��
    col = 1;
  case WIFI_STATUS_FRONTIER:          // �t�����e�B�A��
    scrn_idx = PLAYER_DISP_ICON_IDX_FRONTIER;
    break;

  case WIFI_STATUS_LOGIN_WAIT:    // �ҋ@���@���O�C������͂���
    scrn_idx = PLAYER_DISP_ICON_IDX_NORMAL;
    break;

  case WIFI_STATUS_NONE:		// ��������
    scrn_idx = PLAYER_DISP_ICON_IDX_NONE;
    break;

  case WIFI_STATUS_UNKNOWN:   // �V���ɍ�����炱�̔ԍ��ȏ�ɂȂ�
  case WIFI_STATUS_PLAY_OTHER:
    scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
    break;
  default:
    scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
    break;
  }

  // ��������
  WifiP2PMatchFriendListIconWrite( p_data, frm, cx, cy, scrn_idx, col );
}


//==============================================================================
/**
 * @brief   �`��J�n�ʒu���}�C�i�X�����ɂ����Ă��`��ł���BmpWinPrint���b�p�[
 * @retval  none
 */
//==============================================================================
static void _BmpWinPrint_Rap(
  GFL_BMPWIN * win, void * src,
  int src_x, int src_y, int src_dx, int src_dy,
  int win_x, int win_y, int win_dx, int win_dy,HEAPID heapID )
{
  GFL_BMP_DATA *srcBmp = GFL_BMP_CreateWithData( (u8*)src , 1,1,GFL_BMP_16_COLOR,heapID );
  if(win_x < 0){
    int diff;
    diff = win_x*-1;
    if(diff>win_dx){
      diff = win_dx;
    }
    win_x   = 0;
    src_x  += diff;
    src_dx -= diff;
    win_dx -= diff;
  }

  if(win_y < 0){
    int diff;
    diff = win_y*-1;
    if(diff>win_dy){
      diff = win_dy;
    }
    win_y   = 0;
    src_y  += diff;
    src_dy -= diff;
    win_dy -= diff;
  }

  GFL_BMP_Print( srcBmp, GFL_BMPWIN_GetBmp(win), src_x, src_y, win_x, win_y, win_dx, win_dy, 0);

  GFL_BMP_Delete( srcBmp );
}

static GFL_BMPWIN* _BmpWinDel(GFL_BMPWIN* pBmpWin)
{
  if(pBmpWin){
    BmpWinFrame_Clear( pBmpWin, WINDOW_TRANS_ON );
    //        GFL_BMPWIN_ClearScreen(pBmpWin);
    GFL_BMPWIN_Delete(pBmpWin);
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�r�b�g�}�b�v�ɃA�C�R���f�[�^����������
 *
 *	@param	p_bmp		�r�b�g�}�b�v
 *	@param	p_data		�A�C�R���f�[�^
 *	@param	x			�������݂����W
 *	@param	y			�������݂����W
 *	@param	icon_type	�A�C�R���^�C�v
 *	@param	col			�J���[
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListBmpIconWrite(  GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 icon_type, u32 col,int pal )
{
  //	int pal;

  // ��������
  _BmpWinPrint_Rap( p_bmp,
                    p_data->p_char->pRawData,
                    (PLAYER_DISP_ICON_SCRN_X*8)*icon_type, 0,
                    p_data->p_char->W*8, p_data->p_char->H*8,
                    x, y,
                    PLAYER_DISP_ICON_SCRN_X*8, PLAYER_DISP_ICON_SCRN_Y*8,
                    HEAPID_WIFIP2PMATCH);

  //	// �p���b�gNo�擾
  //	pal = WifiP2PMatchBglFrmIconPalGet( p_bmp->frmnum );

  // �p���b�g�i���o�[�����킹��
  GFL_BMPWIN_SetPalette( p_bmp, pal+col );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�r�b�g�}�b�v�ɃA�C�R���f�[�^����������
 *
 *	@param	p_bmp		�r�b�g�}�b�v
 *	@param	p_data		�A�C�R���f�[�^
 *	@param	x			�������݂����W
 *	@param	y			�������݂����W
 *	@param	status		���
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListBmpStIconWrite( GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 status,int pal )
{
  u8 col=0;
  u8 scrn_idx=0;

  switch( status ){
  case WIFI_STATUS_VCT:      // VCT��
    scrn_idx = PLAYER_DISP_ICON_IDX_VCT;
    break;

  case WIFI_STATUS_SBATTLE50_WAIT:   // �V���O��Lv50�ΐ��W��
  case WIFI_STATUS_SBATTLE100_WAIT:   // �V���O��Lv100�ΐ��W��
  case WIFI_STATUS_SBATTLE_FREE_WAIT:   // �V���O��Lv100�ΐ��W��
  case WIFI_STATUS_DBATTLE50_WAIT:   // �_�u��Lv50�ΐ��W��
  case WIFI_STATUS_DBATTLE100_WAIT:   // �_�u��Lv100�ΐ��W��
  case WIFI_STATUS_DBATTLE_FREE_WAIT:   // �_�u��Lv100�ΐ��W��
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE_WAIT:// �}���`�o�g����W��
#endif
    col = 1;
  case WIFI_STATUS_SBATTLE50:      // �V���O���ΐ풆
  case WIFI_STATUS_SBATTLE100:      // �V���O���ΐ풆
  case WIFI_STATUS_SBATTLE_FREE:      // �V���O���ΐ풆
  case WIFI_STATUS_DBATTLE50:      // �_�u���ΐ풆
  case WIFI_STATUS_DBATTLE100:      // �_�u���ΐ풆
  case WIFI_STATUS_DBATTLE_FREE:      // �_�u���ΐ풆
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE:     // �}���`�o�g����
#endif
    scrn_idx = PLAYER_DISP_ICON_IDX_FIGHT;
    break;


  case WIFI_STATUS_TRADE_WAIT:    // ������W��
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_BATTLEROOM_WAIT:// �o�g�����[����W��
#endif
    col = 1;
  case WIFI_STATUS_TRADE:          // ������
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_BATTLEROOM:     // �o�g�����[����
#endif
    scrn_idx = PLAYER_DISP_ICON_IDX_CHANGE;
    break;

  case WIFI_STATUS_FRONTIER_WAIT:    // �t�����e�B�A��W��
    col = 1;
  case WIFI_STATUS_FRONTIER:          // �t�����e�B�A��
    scrn_idx = PLAYER_DISP_ICON_IDX_FRONTIER;
    break;

  case WIFI_STATUS_LOGIN_WAIT:    // �ҋ@���@���O�C������͂���
    scrn_idx = PLAYER_DISP_ICON_IDX_NORMAL;
    break;

  case WIFI_STATUS_NONE:   // ��������
    scrn_idx = PLAYER_DISP_ICON_IDX_NONE;
    break;

  case WIFI_STATUS_UNKNOWN:   // �V���ɍ�����炱�̔ԍ��ȏ�ɂȂ�
  case WIFI_STATUS_PLAY_OTHER:   // �V���ɍ�����炱�̔ԍ��ȏ�ɂȂ�
    scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
    break;
  default:
    scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
    break;
  }

  WifiP2PMatchFriendListBmpIconWrite( p_bmp, p_data, x, y, scrn_idx, col,pal );
}

//----------------------------------------------------------------------------
/**
 *	@brief	frm�̃A�C�R���p���b�g�i���o�[��Ԃ�
 *
 *	@param	frm		BGL�@FRAME
 *
 *	@return �p���b�gNO
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
 *	@brief	�t�����h���X�g�J�n�錾
 */
//-----------------------------------------------------------------------------
static int WifiP2PMatchFriendListStart( void )
{
  //	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
  //					COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,
                              0,16,COMM_BRIGHTNESS_SYNC);

  // ������BGM�ݒ�
  WifiP2P_SetLobbyBgm();

  return WIFIP2PMATCH_FRIENDLIST_INIT;
}

#define TALK_MESSAGE_BUF_NUM	( 190*2 )
#define TITLE_MESSAGE_BUF_NUM	( 90*2 )

//------------------------------------------------------------------
/**
 * ���[�N������
 *
 * @param   wk		WIFIP2PMATCH_WORK*
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitWork( WIFIP2PMATCH_WORK *wk )
{
  int i;
  int flag;

  wk->TalkString =  GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
  wk->pTemp = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );        // ���͓o�^���̈ꎞ�o�b�t�@
  wk->pExpStrBuf = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
  wk->TitleString = GFL_STR_CreateBuffer( TITLE_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
  wk->SysMsgQue = PRINTSYS_QUE_Create( HEAPID_WIFIP2PMATCH );

  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_INIT);

}

//------------------------------------------------------------------
/**
 * $brief   ���[�N���
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void FreeWork( WIFIP2PMATCH_WORK *wk )
{
  int i;


  GFL_STR_DeleteBuffer( wk->TitleString );
  GFL_STR_DeleteBuffer( wk->TalkString );
  GFL_STR_DeleteBuffer( wk->pTemp );

  GFL_STR_DeleteBuffer(wk->pExpStrBuf);
  PRINTSYS_QUE_Clear(wk->SysMsgQue);
  PRINTSYS_QUE_Delete(wk->SysMsgQue);

}

//--------------------------------------------------------------------------------------------
/**
 * BG���
 *
 * @param	ini		BGL�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( void )
{
  int i;

  for(i=0;i<GFL_BG_FRAME3_S;i++){
    GFL_BG_FreeBGControl( i );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param	wk		�|�P�������X�g��ʂ̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------


static void BgGraphicSet( WIFIP2PMATCH_WORK * wk, ARCHANDLE* p_handle )
{
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 |
                                 GX_PLANEMASK_BG3, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG2, VISIBLE_ON );

  // �㉺��ʂa�f�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_wifip2pmatch_conect_NCLR, PALTYPE_MAIN_BG, 0, 0,  HEAPID_WIFIP2PMATCH);
  GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_wifip2pmatch_conect_NCLR, PALTYPE_SUB_BG,  0, 0,  HEAPID_WIFIP2PMATCH);

  // ��b�t�H���g�p���b�g�]��
  //    TalkFontPaletteLoad( PALTYPE_MAIN_BG, _NUKI_FONT_PALNO*0x20, HEAPID_WIFIP2PMATCH );
  //    TalkFontPaletteLoad( PALTYPE_MAIN_BG, COMM_MESFONT_PAL*0x20, HEAPID_WIFIP2PMATCH );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , 13*0x20, 16*2, HEAPID_WIFIP2PMATCH );


  // ���C�����BG2�L�����]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_conect_NCGR,
                                        GFL_BG_FRAME0_M, 0, 0, 0, HEAPID_WIFIP2PMATCH);

  // ���C�����BG2�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_wifip2pmatch_conect_01_NSCR,
                                     GFL_BG_FRAME0_M, 0, 0, 0, HEAPID_WIFIP2PMATCH);


  // �T�u���BG0�L�����]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_conect_sub_NCGR,  GFL_BG_FRAME0_S, 0, 0, 0, HEAPID_WIFIP2PMATCH);

  // �T�u���BG0�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_wifip2pmatch_conect_sub_NSCR, GFL_BG_FRAME0_S, 0, 0, 0, HEAPID_WIFIP2PMATCH);

  {
    int type = CONFIG_GetWindowType(wk->pConfig);
    BmpWinFrame_GraphicSet(
      GFL_BG_FRAME2_M, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL,  type, HEAPID_WIFIP2PMATCH );
    BmpWinFrame_GraphicSet(
      GFL_BG_FRAME2_M, MENU_WIN_CGX_NUM, MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
    BmpWinFrame_GraphicSet(
      GFL_BG_FRAME1_M, MENU_WIN_CGX_NUM, MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
  }

}


//** CharManager PlttManager�p **//
#define WIFIP2PMATCH_CHAR_CONT_NUM				(20)
#define WIFIP2PMATCH_CHAR_VRAMTRANS_MAIN_SIZE		(128*1024)
#define WIFIP2PMATCH_CHAR_VRAMTRANS_SUB_SIZE		(16*1024)
#define WIFIP2PMATCH_PLTT_CONT_NUM				(20)

//-------------------------------------
//
//	�L�����N�^�}�l�[�W���[
//	�p���b�g�}�l�[�W���[�̏�����
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


//------------------------------------------------------------------
/**
 * �Z���A�N�^�[������
 *
 * @param   wk		���[�_�[�\���̂̃|�C���^
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitCellActor(WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle)
{
  int i;

#if 0
  // OAM�}�l�[�W���[�̏�����
  NNS_G2dInitOamManagerModule();

  // ���LOAM�}�l�[�W���쐬
  // �����_���pOAM�}�l�[�W���쐬
  // �����ō쐬����OAM�}�l�[�W�����݂�Ȃŋ��L����
  REND_OAMInit(
    0, 126,		// ���C�����OAM�Ǘ��̈�
    0, 32,		// ���C����ʃA�t�B���Ǘ��̈�
    0, 126,		// �T�u���OAM�Ǘ��̈�
    0, 32,		// �T�u��ʃA�t�B���Ǘ��̈�
    HEAPID_WIFIP2PMATCH);


  // �Z���A�N�^�[������
  wk->clactSet = CLACT_U_SetEasyInit( WF_CLACT_WKNUM, &wk->renddata, HEAPID_WIFIP2PMATCH );

  CLACT_U_SetSubSurfaceMatrix( &wk->renddata, 0, NAMEIN_SUB_ACTOR_DISTANCE );


  //���\�[�X�}�l�[�W���[������
  for(i=0;i<CLACT_RESOURCE_NUM;i++){		//���\�[�X�}�l�[�W���[�쐬
    wk->resMan[i] = CLACT_U_ResManagerInit(WF_CLACT_RESNUM, i, HEAPID_WIFIP2PMATCH);
  }
#endif
  const u8 CELL_MAX = 16;
  //  GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
  //  cellSysInitData.oamst_main = GFL_CLSYS_OAMMAN_INTERVAL; //�ʐM�A�C�R���̕�
  //  cellSysInitData.oamnum_main = 64-GFL_CLSYS_OAMMAN_INTERVAL;
  //  cellSysInitData.oamst_sub = 16;
  //  cellSysInitData.oamnum_sub = 128-16;

  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN , &_defVBTbl, HEAPID_WIFIP2PMATCH );
  wk->clactSet  = GFL_CLACT_UNIT_Create( CELL_MAX , 0, HEAPID_WIFIP2PMATCH );
  GFL_CLACT_UNIT_SetDefaultRend( wk->clactSet );


  // FontOAM�V�X�e���쐬
  //	wk->fontoam = FONTOAM_SysInit( WF_FONTOAM_NUM, HEAPID_WIFIP2PMATCH );  //@@OO


#if 0
  //---------���ʗp-------------------

  //chara�ǂݍ���
  wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] =
    CLACT_U_ResManagerResAddArcChar_ArcHandle(wk->resMan[CLACT_U_CHAR_RES],
                                              p_handle,
                                              NARC_wifip2pmatch_conect_obj_NCGR, FALSE, 0, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_WIFIP2PMATCH);

  //pal�ǂݍ���
  wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] =
    CLACT_U_ResManagerResAddArcPltt_ArcHandle(wk->resMan[CLACT_U_PLTT_RES],
                                              p_handle,
                                              NARC_wifip2pmatch_conect_obj_NCLR, 0, 0, NNS_G2D_VRAM_TYPE_2DMAIN, 7, HEAPID_WIFIP2PMATCH);

  //cell�ǂݍ���
  wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES] =
    CLACT_U_ResManagerResAddArcKindCell_ArcHandle(wk->resMan[CLACT_U_CELL_RES],
                                                  p_handle,
                                                  NARC_wifip2pmatch_conect_NCER, FALSE, 0, CLACT_U_CELL_RES,HEAPID_WIFIP2PMATCH);

  //�����֐���anim�ǂݍ���
  wk->resObjTbl[MAIN_LCD][CLACT_U_CELLANM_RES] =
    CLACT_U_ResManagerResAddArcKindCell_ArcHandle(wk->resMan[CLACT_U_CELLANM_RES],
                                                  p_handle,
                                                  NARC_wifip2pmatch_conect_NANR, FALSE, 0, CLACT_U_CELLANM_RES,HEAPID_WIFIP2PMATCH);


  // ���\�[�X�}�l�[�W���[����]��

  // Chara�]��
  CLACT_U_CharManagerSetAreaCont( wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] );
  //	CLACT_U_CharManagerSet( wk->resObjTbl[SUB_LCD][CLACT_U_CHAR_RES] );

  // �p���b�g�]��
  CLACT_U_PlttManagerSetCleanArea( wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] );
  //	CLACT_U_PlttManagerSet( wk->resObjTbl[SUB_LCD][CLACT_U_PLTT_RES] );

#endif

}

#define TRAINER_NAME_POS_X		( 24 )
#define TRAINER_NAME_POS_Y		( 32 )
#define TRAINER_NAME_POS_SPAN	( 32 )

#define TRAINER_NAME_WIN_X		(  3 )
#define TRAINER1_NAME_WIN_Y		(  6 )
#define TRAINER2_NAME_WIN_Y		(  7 )

//------------------------------------------------------------------
/**
 * �Z���A�N�^�[�o�^
 *
 * @param   wk			WIFIP2PMATCH_WORK*
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void SetCellActor(WIFIP2PMATCH_WORK *wk)
{
#if 0
  int i;
  // �Z���A�N�^�[�w�b�_�쐬
  CLACT_U_MakeHeader(&wk->clActHeader_m, 0, 0, 0, 0, CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
                     0, 0,
                     wk->resMan[CLACT_U_CHAR_RES],
                     wk->resMan[CLACT_U_PLTT_RES],
                     wk->resMan[CLACT_U_CELL_RES],
                     wk->resMan[CLACT_U_CELLANM_RES],
                     NULL,NULL);

  /*
	CLACT_U_MakeHeader(&wk->clActHeader_s, 1, 1, 1, 1, CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
	0, 0,
	wk->resMan[CLACT_U_CHAR_RES],
	wk->resMan[CLACT_U_PLTT_RES],
	wk->resMan[CLACT_U_CELL_RES],
	wk->resMan[CLACT_U_CELLANM_RES],
	NULL,NULL);
   */
  {
    //�o�^���i�[
    CLACT_ADD add;

    add.ClActSet	= wk->clactSet;
    add.ClActHeader	= &wk->clActHeader_m;

    add.mat.z		= 0;
    add.sca.x		= FX32_ONE;
    add.sca.y		= FX32_ONE;
    add.sca.z		= FX32_ONE;
    add.rot			= 0;
    add.pri			= 1;
    add.DrawArea	= NNS_G2D_VRAM_TYPE_2DMAIN;
    add.heap		= HEAPID_WIFIP2PMATCH;

    //�Z���A�N�^�[�\���J�n
    // �T�u��ʗp(���̓o�^�j
    for(i=0;i < _OAM_NUM;i++){
      add.mat.x = FX32_ONE *   TRAINER_NAME_POS_X;
      add.mat.y = FX32_ONE * ( TRAINER_NAME_POS_Y + TRAINER_NAME_POS_SPAN*i ) + NAMEIN_SUB_ACTOR_DISTANCE;
      wk->MainActWork[i] = CLACT_Add(&add);
      CLACT_SetAnmFlag(wk->MainActWork[i],1);
      CLACT_AnmChg( wk->MainActWork[i], i );
      CLACT_BGPriorityChg(wk->MainActWork[i], 2);
      CLACT_SetDrawFlag( wk->MainActWork[i], 0 );
    }

  }
#endif
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );	//���C�����OBJ�ʂn�m
  //	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );	//�T�u���OBJ�ʂn�m
}


//------------------------------------------------------------------
/**
 * BMPWIN�����i�����p�l���Ƀt�H���g�`��j
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void MainMenuMsgInit(WIFIP2PMATCH_WORK *wk)
{
  int i,col;

  _BmpWinDel(wk->MyInfoWinBack);
  wk->MyInfoWinBack = GFL_BMPWIN_Create(GFL_BG_FRAME3_M, 5, 1, 22, 2,
                                        _NUKI_FONT_PALNO,  GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeScreen( wk->MyInfoWinBack );
  GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_018, wk->TitleString );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MyInfoWinBack), 0, 0, wk->TitleString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWinBack);
}


// �͂��E������BMP�i����ʁj
#define YESNO_WIN_FRAME_CHAR	( 1 + TALK_WIN_CGX_SIZ )
#define YESNO_CHARA_OFFSET		( 1 + TALK_WIN_CGX_SIZ + MENU_WIN_CGX_SIZ + FLD_MSG_WIN_SX*FLD_MSG_WIN_SY )
#define YESNO_WINDOW_X			( 22 )
#define YESNO_WINDOW_Y1			(  7 )
#define YESNO_WINDOW_Y2			( 13 )
#define YESNO_CHARA_W			(  8 )
#define YESNO_CHARA_H			(  4 )

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
 * $brief   �m�ۂ���BMPWIN�����
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void BmpWinDelete( WIFIP2PMATCH_WORK *wk )
{
  int i;

  // ���b�Z�[�W��~
  EndMessageWindowOff( wk );

  if(wk->MyInfoWinBack){
    GFL_BMPWIN_Delete(wk->MyInfoWinBack);
    wk->MyInfoWinBack = NULL;
  }
  if(wk->MsgWin){
    GFL_BMPWIN_Delete(wk->MsgWin);
    wk->MsgWin=NULL;
  }
  if(wk->MyWin){
    GFL_BMPWIN_Delete(wk->MyWin);
    wk->MyWin=NULL;
  }
  if(wk->MyInfoWin){
    GFL_BMPWIN_Delete(wk->MyInfoWin);
    wk->MyInfoWin=NULL;
  }
  if(wk->ListWin){
    GFL_BMPWIN_Delete(wk->ListWin);
    wk->ListWin=NULL;
  }
  if(wk->SysMsgWin){
    GFL_BMPWIN_Delete(wk->SysMsgWin);
    wk->SysMsgWin = NULL;
  }
  if(wk->lw){
    BmpMenuList_Exit(wk->lw, NULL, NULL);
    wk->lw = NULL;
  }
  if(wk->menulist){
    BmpMenuWork_ListDelete( wk->menulist );
    wk->menulist = NULL;
  }
}


//------------------------------------------------------------------
/**
 * $brief   �J�n���̃��b�Z�[�W   WIFIP2PMATCH_MODE_INIT
 * @param   wk
 * @param   seq
 * @retval  none
 */
//------------------------------------------------------------------

static const BMPWIN_YESNO_DAT _yesNoBmpDatSys = {
  GFL_BG_FRAME2_M, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY,
  FLD_YESNO_WIN_PAL, YESNO_WIN_CGX
  };
static const BMPWIN_YESNO_DAT _yesNoBmpDatSys2 = {
  GFL_BG_FRAME1_M, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY+6,
  FLD_YESNO_WIN_PAL, FRAME1_YESNO_WIN_CGX
  };

static int WifiP2PMatch_MainInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 status;

  if(GFL_NET_IsInit()){       // �ڑ���

    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    //		WIFI_STATUS_SetVChatStatus(pMatch, wk->pMatch->myMatchStatus.vchat_org);  @@OO



    // status�擾
    status = _WifiMyStatusGet( wk, wk->pMatch );

    if( (status == WIFI_STATUS_TRADE) ||
        (status == WIFI_STATUS_FRONTIER)
        )
    {	// ������or�|�t�B���Ȃ�

      // 080605 tomoya BTS:249�̑Ώ��̂���
      // �e�̒ʐM���Z�b�g�����㒼���ɐڑ����Ă��܂��̂����
      // �X�e�[�^�X��LOGIN_WAIT�ɕύX
      _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);

      //            CommInfoFinalize();    @@OO
      GFL_NET_StateWifiMatchEnd(TRUE);	// �}�b�`���O��؂�
      //            wk->pMatch = GFL_NET_StateGetMatchWork();   // �����Ə�̏ꏊ�Ɉړ�����@@OO
      //            wk->pMatch =
      //            wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
      wk->timer = _RECONECTING_WAIT_TIME;
      _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
    }
    else
    {
      MainMenuMsgInit(wk);// BMPWIN�o�^�E�`��

      // �o�g������߂��Ă����Ƃ�
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_044, FALSE);

      GFL_NET_SetAutoErrorCheck(TRUE);
      GFL_NET_SetNoChildErrorCheck(TRUE);
      GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_BATTLE_END);
      _CHANGESTATE(wk,WIFIP2PMATCH_NEXTBATTLE_YESNO);
    }
  }
  else
  {        // �ڑ����܂�

    Snd_DataSetByScene( SND_SCENE_P2P, SEQ_WIFILOBBY, 1 );
    PMSND_PlayNextBGM(SEQ_WIFI_ACCESS, 60, 0);
    MainMenuMsgInit(wk);// BMPWIN�o�^�E�`��
    if( !DWC_CheckHasProfile( WifiList_GetMyUserInfo(wk->pList) ) )
    {
      GFL_BG_SetPriority(GFL_BG_FRAME0_M , 3);  //�͂�����
      GFL_BG_SetPriority(GFL_BG_FRAME1_M , 0);   // yesno win
      GFL_BG_SetPriority(GFL_BG_FRAME3_M , 1);  // ���b�Z�[�W
      GFL_BG_SetPriority(GFL_BG_FRAME2_M , 1);   // menulist
      _systemMessagePrint(wk, dwc_message_0003);
      wk->pYesNoWork =
        BmpMenu_YesNoSelectInit(
          &_yesNoBmpDatSys2,
          MENU_WIN_CGX_NUM, MENU_WIN_PAL,0,
          HEAPID_WIFIP2PMATCH );
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_YESNO);
    }
    else if( !DWC_CheckValidConsole(WifiList_GetMyUserInfo(wk->pList)) )
    {
      _systemMessagePrint(wk, dwc_message_0005);
      _CHANGESTATE(wk,WIFIP2PMATCH_DIFFER_MACHINE_INIT);
    }
    else
    {
      WifiP2PMatchMessagePrint(wk, dwc_message_0002, TRUE);
      _CHANGESTATE(wk,WIFIP2PMATCH_NORMALCONNECT_YESNO);
    }
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �ʏ�̐ڑ�  WIFIP2PMATCH_NORMALCONNECT_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int _normalConnectYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) )
  {
    wk->pYesNoWork =
      BmpMenu_YesNoSelectInit(
        &_yesNoBmpDatSys,
        MENU_WIN_CGX_NUM, MENU_WIN_PAL,0,
        HEAPID_WIFIP2PMATCH );
    _CHANGESTATE(wk,WIFIP2PMATCH_NORMALCONNECT_WAIT);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   �ʏ�̐ڑ�  WIFIP2PMATCH_NORMALCONNECT_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int _normalConnectWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);
  if(ret == BMPMENU_NULL)
  {  // �܂��I��
    return seq;
  }
  else
  {
    if(ret == 0)
    { // �͂���I�������ꍇ
      //�ڑ��J�n
      GFL_NET_Init(&aGFLNetInit, NULL, wk);
      GFL_NET_StateWifiEnterLogin();

      WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
      GF_ASSERT( wk->timeWaitWork == NULL );
      wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
      _CHANGESTATE(wk,WIFIP2PMATCH_CONNECTING_INIT);
    }
    else
    {  // ��������I�������ꍇ
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
    }
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �ʂ�DS�������ꍇ  WIFIP2PMATCH_DIFFER_MACHINE_INIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _differMachineInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE))
  {
    _systemMessagePrint(wk,dwc_message_0006);
    GFL_BG_SetPriority(GFL_BG_FRAME3_M , 1);  // ���b�Z�[�W
    GFL_BG_SetPriority(GFL_BG_FRAME2_M , 1);   //
    GFL_BG_SetPriority(GFL_BG_FRAME1_M , 0);   // yesno win
    GFL_BG_SetPriority(GFL_BG_FRAME0_M , 3);  //�͂�����
    wk->pYesNoWork =
      BmpMenu_YesNoSelectInit(
        &_yesNoBmpDatSys2,
        MENU_WIN_CGX_NUM, MENU_WIN_PAL,
        1, HEAPID_WIFIP2PMATCH );	// �������@����n�܂�
    _CHANGESTATE(wk,WIFIP2PMATCH_DIFFER_MACHINE_NEXT);
  }

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �ʂ�DS�������ꍇ  WIFIP2PMATCH_DIFFER_MACHINE_NEXT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _differMachineNext( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);
  if(ret == BMPMENU_NULL)
  {  // �܂��I��
    return seq;
  }
  else{
    wk->SysMsgWin = _BmpWinDel( wk->SysMsgWin );
    if(ret == 0){ // �͂���I�������ꍇ
      _systemMessagePrint(wk,dwc_message_0007);
      wk->pYesNoWork =
        BmpMenu_YesNoSelectInit(
          &_yesNoBmpDatSys2,
          MENU_WIN_CGX_NUM, MENU_WIN_PAL,
          1, HEAPID_WIFIP2PMATCH );	// �������@����n�߂�
      _CHANGESTATE(wk,WIFIP2PMATCH_DIFFER_MACHINE_ONEMORE);
    }
    else{  // ��������I�������ꍇ
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
    }
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �ʂ�DS�������ꍇ  WIFIP2PMATCH_DIFFER_MACHINE_ONEMORE
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _differMachineOneMore( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);
  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }
  else{

    // �D�揇�ʂ�߂�
    GFL_BG_SetPriority(GFL_BG_FRAME3_M , 1);  // ���b�Z�[�W
    GFL_BG_SetPriority(GFL_BG_FRAME2_M , 0);   //
    GFL_BG_SetPriority(GFL_BG_FRAME1_M , 1);   // yesno win
    GFL_BG_SetPriority(GFL_BG_FRAME0_M , 3);  //�͂�����

    wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
    if(ret == 0){ // �͂���I�������ꍇ

      GFL_NET_Init(&aGFLNetInit, NULL, wk);
      //�ڑ��J�n
      WifiList_Init(wk->pList);
      EMAILSAVE_Init(SaveData_Get(wk->pSaveData, GMDATA_ID_EMAIL));
      //  FrontierRecord_DataCrear(SaveData_GetFrontier(wk->pSaveData));

      _CHANGESTATE(wk,WIFIP2PMATCH_CONNECTING_INIT);
      GFL_NET_StateWifiEnterLogin();
      WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
      GF_ASSERT( wk->timeWaitWork == NULL );
      wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
      wk->bInitMessage = TRUE;
    }
    else{  // ��������I�������ꍇ
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
    }
  }
  return seq;
}

//WIFIP2PMATCH_FIRST_YESNO
//------------------------------------------------------------------
/**
 * $brief   ����ڑ����̊m�F  WIFIP2PMATCH_FIRST_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _firstYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret = BMPMENU_NULL;

  ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);
  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }
  else{

    // �\���D�揇�ʂ�߂�
    GFL_BG_SetPriority(GFL_BG_FRAME0_M , 3);  //�͂�����
    GFL_BG_SetPriority(GFL_BG_FRAME1_M , 1);   // yesno win
    GFL_BG_SetPriority(GFL_BG_FRAME3_M , 1);  // ���b�Z�[�W
    GFL_BG_SetPriority(GFL_BG_FRAME2_M , 0);   // menulist

    wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
    if(ret == 0){ // �͂���I�������ꍇ
      GFL_NET_Init(&aGFLNetInit, NULL, wk);

      //�ڑ��J�n
      _CHANGESTATE(wk,WIFIP2PMATCH_CONNECTING_INIT);
      GFL_NET_StateWifiEnterLogin();
      WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
      GF_ASSERT( wk->timeWaitWork == NULL );
      wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
      wk->bInitMessage = TRUE;
    }
    else{  // ��������I�������ꍇ
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
    }
  }

  return seq;
}

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
  GFL_NET_DWC_SetMyInfo((const char *)wk->pMatch, WIFI_STATUS_GetSize());
}

//------------------------------------------------------------------
/**
 * $brief   MyMatchStatus�쐬
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static void _makeMyMatchStatus(WIFIP2PMATCH_WORK* wk, int status)
{
  MYSTATUS* pMyStatus = SaveData_GetMyStatus(wk->pSaveData);
  ZUKAN_WORK* pZukan = SaveData_GetZukanWork(wk->pSaveData);
  POKEPARTY* pPokeParty = SaveData_GetTemotiPokemon(wk->pSaveData);
  WIFI_HISTORY* pHistry = SaveData_GetWifiHistory(wk->pSaveData);
  int max,i;

  max = PokeParty_GetPokeCount(pPokeParty);
  for(i = 0;i < max;i++){
    POKEMON_PARAM* poke = PokeParty_GetMemberPointer( pPokeParty, i);
    WIFI_STATUS_SetMonsNo(wk->pMatch, i , PokeParaGet( poke, ID_PARA_monsno, NULL ));
    WIFI_STATUS_SetItemNo(wk->pMatch, i , PokeParaGet( poke, ID_PARA_item, NULL ));
  }
  WIFI_STATUS_SetMyStatus(wk->pMatch , pMyStatus);

#if PM_DEBUG
  if(MyStatus_GetMySex(pMyStatus) == PM_MALE){
    MyStatus_SetTrainerView(WIFI_STATUS_GetMyStatus(wk->pMatch), PLBOY1);
  }
  else{
    MyStatus_SetTrainerView(WIFI_STATUS_GetMyStatus(wk->pMatch), PLGIRL1);
  }
#endif

  //	wk->pMatch->myMatchStatus.version = MyStatus_GetRomCode(pMyStatus);
  //  wk->pMatch->myMatchStatus.regionCode = MyStatus_GetRegionCode(pMyStatus);

  //	wk->pMatch->myMatchStatus.pokemonZukan = ZukanWork_GetZenkokuZukanFlag(pZukan);
  //    wk->pMatch->myMatchStatus.status = status;
  //    wk->pMatch->myMatchStatus.status = WIFI_STATUS_UNKNOWN;
  _myStatusChange_not_send(wk, status);	// BGM��ԂȂǂ𒲐�
  //  wk->pMatch->myMatchStatus.regulation = _REGULATION_BATTLE_TOWER;
  //#if MYSTATUS_GETTRAINERVIEW_COMPLETION
  //  wk->pMatch->myMatchStatus.trainer_view = MyStatus_GetTrainerView(pMyStatus);
  //#else
  //  wk->pMatch->myMatchStatus.trainer_view = PLBOY1;
  //#endif
  //  wk->pMatch->myMatchStatus.sex = MyStatus_GetMySex(pMyStatus);
  WIFI_STATUS_SetMyNation(wk->pMatch, WIFIHISTORY_GetMyNation(pHistry));
  WIFI_STATUS_SetMyArea(wk->pMatch, WIFIHISTORY_GetMyArea(pHistry));
  //  wk->pMatch->myMatchStatus.vchat = TRUE;
  //  wk->pMatch->myMatchStatus.vchat_org = TRUE;
  WIFI_STATUS_SetVChatStatus(wk->pMatch, TRUE);

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
            (status != WIFI_STATUS_PLAY_OTHER) ){
          //          GF_ASSERT(p_status->trainer_view != PLHERO);
          wk->index2No[num] = i+1;
          num++;
        }else{

          if( (status == WIFI_STATUS_PLAY_OTHER) ){
            // �������̐l�̃{�^�������ɏo�Ă��������
            // BTS:�ʐM393�̑Ώ�
            if( MCVSys_BttnTypeGet( wk, i+1 ) != MCV_BTTN_FRIEND_TYPE_NONE ){
              MCVSys_BttnDel( wk, i+1 );
            }
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
  u32 status;

  for(i = 0;i < WIFIP2PMATCH_MEMBER_MAX;i++){

    p_status = WifiFriendMatchStatusGet( i );
    status = _WifiMyStatusGet( wk, p_status );
    if((wk->matchStatusBackup[i]  != status) ||
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

        wk->matchStatusBackup[i] = status;
        wk->matchVchatBackup[i] = WIFI_STATUS_GetVChatStatus(p_status);
        {
          const MYSTATUS* pMy = WIFI_STATUS_GetMyStatus(p_status);
          // �ʐM�擾��������
          NET_PRINT("�ʐM�擾��������  %d %d\n", i, MyStatus_GetTrainerView(pMy));
          WifiList_SetFriendInfo(wk->pList, i,
                                 WIFILIST_FRIEND_UNION_GRA,
                                 MyStatus_GetTrainerView(pMy));
          WifiList_SetFriendInfo(wk->pList, i,
                                 WIFILIST_FRIEND_SEX,
                                 MyStatus_GetMySex(pMy));
        }
        num++;
      }
      /*            { �o�g�����Ɉꊇ�ŏ������Ă���̂ŁA��������͍폜
                WIFI_HISTORY* pHistry = SaveData_GetWifiHistory(wk->pSaveData);
                int nation = wk->pMatch->friendMatchStatus[i].nation;
                int area = wk->pMatch->friendMatchStatus[i].area;
                WIFIHIST_STAT stat = WIFIHISTORY_GetStat(pHistry, nation, area);
                if((WIFIHIST_STAT_NODATA == stat) || (WIFIHIST_STAT_NEW == stat)){
                    WIFIHISTORY_SetStat(pHistry, nation, area, stat);
                }
            } */
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
  NET_PRINT("�G���[���b�Z�[�W %d \n",msgno);
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
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_INIT);//WIFIP2PMATCH_POWEROFF_INIT);
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
 * $brief   �d����؂���WIFI�ݒ��  WIFIP2PMATCH_POWEROFF_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _poweroffInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  // WIFI�ݒ�ɍs���Ȃ��悤��

  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
    _CHANGESTATE(wk,WIFIP2PMATCH_POWEROFF_YESNO);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �d����؂���WIFI�ݒ��  WIFIP2PMATCH_POWEROFF_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _poweroffYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  _CHANGESTATE(wk,WIFIP2PMATCH_POWEROFF_WAIT);
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   �d����؂���WIFI�ݒ��  WIFIP2PMATCH_POWEROFF_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _poweroffWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  // WIFI�ݒ�ɍs���Ȃ��悤��
  /*
    int i;
    int ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);
    if(ret == BMPMENU_NULL){  // �܂��I��
        return seq;
    }else if(ret == 0){ // �͂���I�������ꍇ
        GFL_NET_StateWifiLogout();
        wk->endSeq = WIFI_P2PMATCH_UTIL;   //�\�t�g�E�G�A���Z�b�g
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
        EndMessageWindowOff(wk);
    }
    else{  // ��������I�������ꍇ
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_052, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_YESNO);
    }
   */
  _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_YESNO);
  return seq;
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
  if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    // �͂��������E�C���h�E���o��
    wk->pYesNoWork =
      BmpMenu_YesNoSelectInit(
        &_yesNoBmpDat,
        MENU_WIN_CGX_NUM, MENU_WIN_PAL,0,
        HEAPID_WIFIP2PMATCH );
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
  int ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);

  if(ret == BMPMENU_NULL)
  {  // �܂��I��
    return seq;
  }
  else if(ret == 0)
  { // �͂���I�������ꍇ
    //GFL_NET_StateWifiLogout();
    GFL_NET_Exit(NULL);
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY);
  }
  else
  {  // ��������I�������ꍇ
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);	// VCHAT�̏�Ԃ����ɖ߂�����
    //GFL_NET_StateWifiLogout();
    GFL_NET_Exit(NULL);
    //@@OO     CommInfoFinalize();   //Info������
    wk->endSeq = WIFI_P2PMATCH_END;
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
      //			WIPE_SetBrightness( WIPE_DISP_MAIN, WIPE_FADE_BLACK );
      //			WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
      GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,16,16,0);

      WIFI_MCR_Dest( &wk->matchroom );
      for( i=0; i<MATCHROOM_IN_OBJNUM; i++ )
      {
        wk->p_matchMoveObj[i] = NULL;
      }

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


    // �p���b�g�t�F�[�h�ĊJ
    ConnectBGPalAnm_OccSet(&wk->cbp, TRUE);

    // �u���C�g�l�X��ԃ��Z���g
    // MCR��ʔj�����ɈÂ�������������
    wk->brightness_reset = TRUE;
    //		WIPE_ResetBrightness( WIPE_DISP_MAIN );
    //		WIPE_ResetBrightness( WIPE_DISP_SUB );

    if( wk->menulist){
      GFL_BMPWIN_Delete(wk->ListWin);
      BmpMenuList_Exit(wk->lw, NULL, NULL);
      wk->lw = NULL;
      BmpMenuWork_ListDelete( wk->menulist );
      wk->menulist=NULL;
    }

    //        wk->friendMatchReadCount;
    MI_CpuClear8(wk->index2No, WIFIP2PMATCH_MEMBER_MAX);
    MI_CpuClear8(wk->index2NoBackUp, WIFIP2PMATCH_MEMBER_MAX);
    MI_CpuClear8(wk->matchStatusBackup, WIFIP2PMATCH_MEMBER_MAX*sizeof(int));
    MI_CpuClear8(wk->matchVchatBackup, WIFIP2PMATCH_MEMBER_MAX*sizeof(int));

#if 0
    for(i=0;i < _OAM_NUM;i++){
      CLACT_SetDrawFlag( wk->MainActWork[i], 0 );
    }
#endif
    MainMenuMsgInit(wk);
    GFL_NET_Init(&aGFLNetInit, NULL, wk);
    GFL_NET_StateWifiEnterLogin();
    //        wk->pMatch = GFL_NET_StateWifiEnterLogin(wk->pSaveData,sizeof(TEST_MATCH_WORK));
    WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
    GF_ASSERT( wk->timeWaitWork == NULL );
    wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
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

  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    return seq;
  }
  if( GFL_NET_DWC_GetSaving()) {
    _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_SAVING);
    WifiP2PMatchMessagePrint(wk, dwc_message_0015, TRUE);
    GF_ASSERT( wk->timeWaitWork == NULL );
    wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
  }

  if(GFL_NET_StateIsWifiLoginState()){
    NET_PRINT("WIFI�ڑ�����\n");
    if( wk->bInitMessage ){  // ����ڑ����ɂ̓Z�[�u�V�[�P���X��
      //            SaveData_SaveParts(wk->pSaveData, SVBLK_ID_NORMAL);  //�Z�[�u��
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG);
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart());
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
  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    return seq;
  }
  if( GFL_NET_DWC_GetSaving()) {
    //    SaveControl_SaveAsyncInit(wk->pSaveData); ///�Z�[�u�J�n
    GFL_NET_DWC_SaveAsyncInit(wk->pSaveData);
    _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_SAVING2);
    return seq;
  }
  if(GFL_NET_StateIsWifiLoginState()){
    NET_PRINT("WIFI�ڑ�����\n");
    if( wk->bInitMessage ){
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG);
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart());
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
    //    SAVE_RESULT result = SaveControl_SaveAsyncMain(wk->pSaveData);
    SAVE_RESULT result = GFL_NET_DWC_SaveAsyncMain(wk->pSaveData);
    if (result != SAVE_RESULT_CONTINUE && result != SAVE_RESULT_LAST) {
      GFL_NET_DWC_ResetSaving();
    }
    else{
      return seq;
    }
  }
  if(GFL_NET_StateIsWifiLoginState()){
    NET_PRINT("WIFI�ڑ�����\n");
    if( wk->bInitMessage ){
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG);
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart());
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

  NET_PRINT("GFL_NET_StateIsWifiLoginState %d  \n",GFL_NET_StateIsWifiLoginState());
  NET_PRINT("GFL_NET_StateGetWifiStatus %d  \n",GFL_NET_StateGetWifiStatus());
  NET_PRINT("GFL_NET_StateIsWifiDisconnect %d  \n",GFL_NET_StateIsWifiDisconnect());
  NET_PRINT("GFL_NET_IsConnectMember %d  \n",GFL_NET_IsConnectMember(GFL_NET_NETID_SERVER));

  if(GFL_NET_StateIsWifiLoginMatchState()){
    NET_PRINT("WIFI�ڑ�����\n");
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
      WIFI_MCR_Dest( &wk->matchroom );
    }
    _CHANGESTATE(wk,WifiP2PMatchFriendListStart());
  }
  else if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(wk);
  }
  else if((GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_DISCONNECTING) || GFL_NET_StateIsWifiDisconnect() || !GFL_NET_IsConnectMember(GFL_NET_NETID_SERVER)){
    GFL_NET_StateWifiMatchEnd(TRUE);
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
      WIFI_MCR_Dest( &wk->matchroom );
    }
    _CHANGESTATE(wk,WifiP2PMatchFriendListStart());
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
    if(wk->initSeq == WIFI_P2PMATCH_DPW){ // ���E�ΐ�Ȃ�
      wk->endSeq = WIFI_P2PMATCH_DPW_END;   // �I��������
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
      //GFL_NET_StateWifiLogout();
      GFL_NET_Exit(NULL);
    }
    else if( 0==WifiList_GetFriendDataNum(wk->pList) ){  //�t�����h�����Ȃ�
      WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXITING);
      wk->timer = 1;
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      {
        int i = WifiP2PMatchFriendListStart();
        _CHANGESTATE(wk,i);
      }
      wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
    }
  }
  return seq;

}

//------------------------------------------------------------------
/**
 * $brief   �����̏�Ԃ�\��
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void	_userDataInfoDisp(WIFIP2PMATCH_WORK * wk)
{
  int col,sex, vct_icon;
  int msg_id;
  MYSTATUS* pMy = SaveData_GetMyStatus(wk->pSaveData);
  u32 status;

  if(!wk->MyInfoWin){
    return;
  }
  GFL_FONTSYS_SetDefaultColor();
  {
    STRBUF* pSTRBUF = MyStatus_CreateNameString(pMy, HEAPID_WIFIP2PMATCH);
    // ������
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MyInfoWin), WINCLR_COL(FBMP_COL_WHITE) );
    sex = MyStatus_GetMySex(pMy);
    if( sex == PM_MALE ){
      _COL_N_BLUE;
    }
    else{
      _COL_N_RED;
    }
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MyInfoWin), WIFIP2PMATCH_PLAYER_DISP_NAME_X, 0,
                    pSTRBUF, wk->fontHandle);
    GFL_STR_DeleteBuffer(pSTRBUF);
  }
  status = _WifiMyStatusGet( wk, wk->pMatch );
  // msg_id�ƃJ���[���擾
  msg_id = MCVSys_StatusMsgIdGet( status, &col );
  if( status == WIFI_STATUS_VCT ){      // VCT��
    if(GFL_NET_DWC_GetFriendIndex() == -1){
      GFL_MSG_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
    }
    else{
      sex = WifiList_GetFriendInfo(wk->pList, GFL_NET_DWC_GetFriendIndex(), WIFILIST_FRIEND_SEX);
      if( PM_MALE == sex){
        _COL_N_BLUE;
      }
      else if( PM_FEMALE == sex){
        _COL_N_RED;
      }
      //            STRBUF_SetStringCode(wk->pExpStrBuf ,WifiList_GetFriendNamePtr(wk->pList,GFL_NET_DWC_GetFriendIndex()));
      WifiList_GetFriendName(wk->pList,GFL_NET_DWC_GetFriendIndex(),wk->pExpStrBuf);
    }
  }else{
    GFL_MSG_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
  }
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MyInfoWin), WIFIP2PMATCH_PLAYER_DISP_ST_X, 0,
                  wk->pExpStrBuf, wk->fontHandle);

  //��ԃA�C�R���ݒ�
  // Frame1�̑Ή�����ʒu�ɃA�C�R����]��
  WifiP2PMatchFriendListStIconWrite( &wk->icon, GFL_BG_FRAME1_M,
                                     PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y,
                                     status );
  if( WIFI_STATUS_GetVChatStatus(wk->pMatch) ){
    vct_icon = PLAYER_DISP_ICON_IDX_NONE;
  }else{
    vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
  }

  WifiP2PMatchFriendListIconWrite(  &wk->icon, GFL_BG_FRAME1_M,
                                    PLAYER_DISP_VCTICON_POS_X, PLAYER_DISP_VCTICON_POS_Y,
                                    vct_icon, 0 );

  GFL_BMPWIN_ClearScreen(wk->MyInfoWin);
  GFL_BMPWIN_MakeScreen(wk->MyInfoWin);
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWin);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME3_M);
}

//------------------------------------------------------------------
/**
 * $brief   �ꃉ�C���R�[���o�b�N
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void	_lineCallBack(BMPMENULIST_WORK * lw, u32 param, u8 y)
{
  WIFIP2PMATCH_WORK* wk = (WIFIP2PMATCH_WORK*)BmpListParamGet(lw, BMPMENULIST_ID_WORK);
  //    int gmmNo = 0,col = _COL_N_BLACK;
  int gmmNo = 0;
  WIFI_STATUS* p_status;
  u32 status;

  if((param == WIFIP2PMATCH_MEMBER_MAX) || (param == BMPMENULIST_CANCEL)){
    return;
  }
  _COL_N_BLACK;

  p_status = WifiFriendMatchStatusGet( param );
  status = _WifiMyStatusGet( wk,  p_status );
  switch(status){
  case WIFI_STATUS_VCT:      // VCT��
    gmmNo = msg_wifilobby_027;
    //        col = _COL_N_GRAY;
    _COL_N_GRAY;
    break;
  case WIFI_STATUS_SBATTLE50:      // �ΐ풆
  case WIFI_STATUS_SBATTLE100:      // �ΐ풆
  case WIFI_STATUS_SBATTLE_FREE:      // �ΐ풆
  case WIFI_STATUS_DBATTLE50:      // �ΐ풆
  case WIFI_STATUS_DBATTLE100:      // �ΐ풆
  case WIFI_STATUS_DBATTLE_FREE:      // �ΐ풆
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE:      // �ΐ풆
#endif
    gmmNo = msg_wifilobby_024;
    //        col = _COL_N_GRAY;
    _COL_N_GRAY;
    break;
  case WIFI_STATUS_SBATTLE50_WAIT:   // �ΐ��W��
    gmmNo = msg_wifilobby_060;
    break;
  case WIFI_STATUS_SBATTLE100_WAIT:   // �ΐ��W��
    gmmNo = msg_wifilobby_061;
    break;
  case WIFI_STATUS_SBATTLE_FREE_WAIT:   // �ΐ��W��
    gmmNo = msg_wifilobby_059;
    break;
  case WIFI_STATUS_DBATTLE50_WAIT:   // �ΐ��W��
    gmmNo = msg_wifilobby_063;
    break;
  case WIFI_STATUS_DBATTLE100_WAIT:   // �ΐ��W��
    gmmNo = msg_wifilobby_064;
    break;
  case WIFI_STATUS_DBATTLE_FREE_WAIT:   // �ΐ��W��
    gmmNo = msg_wifilobby_062;
    break;
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE_WAIT:   // �ΐ��W��
    gmmNo = msg_wifilobby_062;
    break;
#endif
  case WIFI_STATUS_TRADE:          // ������
    gmmNo = msg_wifilobby_026;
    //        col = _COL_N_GRAY;
    _COL_N_GRAY;
    break;
  case WIFI_STATUS_TRADE_WAIT:    // ������W��
    gmmNo = msg_wifilobby_025;
    break;
  case WIFI_STATUS_FRONTIER_WAIT:    // �t�����e�B�A��W��
    gmmNo = msg_wifilobby_138;
    break;
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_BATTLEROOM_WAIT:    // �o�g�����[����W��
    gmmNo = msg_wifilobby_debug_00;
    break;
#endif
  case WIFI_STATUS_LOGIN_WAIT:    // �ҋ@���@���O�C������͂���
    gmmNo = msg_wifilobby_046;
    break;
  case WIFI_STATUS_UNKNOWN:
  default:
    gmmNo = msg_wifilobby_056;
    break;
  }
  NET_PRINT("�����R�[���o�b�N�Ăт��� %d %d %d\n",param,y,gmmNo);

  if(gmmNo != 0){
    GFL_MSG_GetString(wk->MsgManager, gmmNo, wk->pExpStrBuf);

    {
      int length = PRINTSYS_GetStrWidth( wk->pExpStrBuf,wk->fontHandle, 0 );
      int x      = 164 - length;

      PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->ListWin), x, y,
                      wk->pExpStrBuf, wk->fontHandle);
      GFL_BMPWIN_TransVramCharacter(wk->ListWin);
    }
  }

  {
    int sex = WifiList_GetFriendInfo(wk->pList, param, WIFILIST_FRIEND_SEX);
    _COL_N_BLACK;
    if( PM_MALE == sex){
      _COL_N_BLUE;
    }
    else if( PM_FEMALE == sex){
      _COL_N_RED;
    }

    GFL_BMPWIN_TransVramCharacter(wk->ListWin);
  }
}

//------------------------------------------------------------------
/**
 * $brief   �A�C�R���\��
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void	_iconDisp(WIFIP2PMATCH_WORK * wk, int status, int vctIcon, int x,int y,int x2)
{
  u16 btl[]={ 0x17,0x18,0xc18,0x20 };  //�΂Ƃ�
  u16 chg[]={ 0x25,0x26,0x2a,0xc25 };  //��������
  u16 wai[]={ 0x2f,0x42f,0x82f,0xc2f };  //������
  u16 vct[]={ 0x4a,0x4b,0x084a,0x084b };  //VCT
  u16 vctoff[]={ 0x48,0x49,0x0848,0x0849 };  //VCToff
  u16 hate[]={ 0x44,0x45,0x46,0x47 };  //?
  u16 none[]={ 0x6,0x6,0x6,0x6 };  //none
  u16* pData;
  // 0x06-----------�Ȃ�
  u16* pScrAddr = GFL_BG_GetScreenBufferAdrs( GFL_BG_FRAME0_M );
  int pos,col = 0;

  switch(status){
  case WIFI_STATUS_VCT:      // VCT��
    pData = vct;
    break;
  case WIFI_STATUS_SBATTLE50_WAIT:   // �ΐ��W��
  case WIFI_STATUS_SBATTLE100_WAIT:   // �ΐ��W��
  case WIFI_STATUS_SBATTLE_FREE_WAIT:   // �ΐ��W��
  case WIFI_STATUS_DBATTLE50_WAIT:   // �ΐ��W��
  case WIFI_STATUS_DBATTLE100_WAIT:   // �ΐ��W��
  case WIFI_STATUS_DBATTLE_FREE_WAIT:   // �ΐ��W��
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE_WAIT:   // �ΐ��W��
#endif
    col = 0x1000;
  case WIFI_STATUS_SBATTLE50:      // �ΐ풆
  case WIFI_STATUS_SBATTLE100:      // �ΐ풆
  case WIFI_STATUS_SBATTLE_FREE:      // �ΐ풆
  case WIFI_STATUS_DBATTLE50:      // �ΐ풆
  case WIFI_STATUS_DBATTLE100:      // �ΐ풆
  case WIFI_STATUS_DBATTLE_FREE:      // �ΐ풆
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE:      // �ΐ풆
#endif
    pData = btl;
    break;
  case WIFI_STATUS_TRADE_WAIT:    // ������W��
  case WIFI_STATUS_FRONTIER_WAIT:    // �t�����e�B�A��W��
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_BATTLEROOM_WAIT:// ������W��
#endif
    col = 0x1000;
  case WIFI_STATUS_TRADE:          // ������
  case WIFI_STATUS_FRONTIER:          // �t�����e�B�A��
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_BATTLEROOM:     // ������
#endif
    pData = chg;
    break;
  case WIFI_STATUS_LOGIN_WAIT:    // �ҋ@���@���O�C������͂���
    pData = wai;
    break;
  case WIFI_STATUS_UNKNOWN:
    pData = hate;
    break;
  default:
    pData = none;
    break;
  }

  pos = x + y * 32;
  pScrAddr[pos] = pData[0]+col;
  pScrAddr[pos+1] = pData[1]+col;
  pScrAddr[pos+32] = pData[2]+col;
  pScrAddr[pos+33] = pData[3]+col;

  if(vctIcon){
    pData = none;
  }
  else{
    pData = vctoff;
  }
  pos = x2 + y * 32;
  pScrAddr[pos] = pData[0];
  pScrAddr[pos+1] = pData[1];
  pScrAddr[pos+32] = pData[2];
  pScrAddr[pos+33] = pData[3];
}

//------------------------------------------------------------------
/**
 * $brief   ���\�����Ƃ̃R�[���o�b�N�֐�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void	_iconMenuDisp(WIFIP2PMATCH_WORK * wk, int toppos)
{
  u16* pData;
  // 0x06-----------�Ȃ�
  u16* pScrAddr = GFL_BG_GetScreenBufferAdrs(  GFL_BG_FRAME0_M );
  int pos,status,i,vchat;
  WIFI_STATUS* p_status;


  for(i = 0; i < WIFIP2PMATCH_DISP_MEMBER_MAX;i++){
    pos = toppos + i;
    if(pos >= WIFIP2PMATCH_MEMBER_MAX){
      status = WIFI_STATUS_NONE;
      vchat = 1;
    }
    else if(wk->index2No[pos]==0){
      status = WIFI_STATUS_NONE;
      vchat = 1;
    }
    else{
      p_status = WifiFriendMatchStatusGet( wk->index2No[pos] - 1 );
      status = _WifiMyStatusGet( wk, p_status );
      vchat = WIFI_STATUS_GetVChatStatus(p_status);
    }
    pos = 3 + ((5+(i*3)) * 32);
    _iconDisp(wk, status, vchat, 3, 5+(i*3), 27);
  }
  status = _WifiMyStatusGet( wk, wk->pMatch );
  vchat = WIFI_STATUS_GetVChatStatus(wk->pMatch);
  if(status == WIFI_STATUS_VCT){
    status = WIFI_STATUS_NONE;
  }
  _iconDisp(wk, status, vchat, 3, 21, 27);
  GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M);  // �X�N���[���ɍX�V
}

//------------------------------------------------------------------
/**
 * $brief   �J�[�\���R�[���o�b�N
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _curCallBack(BMPMENULIST_WORK * wk,u32 param,u8 mode)
{
  if(mode == 0){
    Snd_SePlay(_SE_CURSOR);
  }
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
                                0x20*COMM_MESFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  wk->MyInfoWinBack = _BmpWinDel(wk->MyInfoWinBack);

  wk->MyInfoWin=GFL_BMPWIN_Create(
    GFL_BG_FRAME3_M,
    WIFIP2PMATCH_PLAYER_DISP_X, WIFIP2PMATCH_PLAYER_DISP_Y,
    WIFIP2PMATCH_PLAYER_DISP_SIZX, WIFIP2PMATCH_PLAYER_DISP_SIZY,
    COMM_MESFONT_PAL, GFL_BMP_CHRAREA_GET_B );


  wk->MyInfoWinBack=GFL_BMPWIN_Create(
    GFL_BG_FRAME1_M,
    WIFIP2PMATCH_PLAYER_DISP_X, WIFIP2PMATCH_PLAYER_DISP_Y,
    WIFIP2PMATCH_PLAYER_DISP_SIZX, WIFIP2PMATCH_PLAYER_DISP_SIZY,
    COMM_MESFONT_PAL, GFL_BMP_CHRAREA_GET_B );

  // ������
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MyInfoWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWin);
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MyInfoWinBack), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWinBack);


  // �V�X�e���E�B���h�E�ݒ�
  //	BmpMenuWinWrite( wk->MyInfoWinBack, WINDOW_TRANS_ON, MENU_WIN_CGX_NUM, MENU_WIN_PAL );
  //    GFL_BMPWIN_MakeFrameScreen(wk->MyInfoWinBack, MENU_WIN_CGX_NUM, MENU_WIN_PAL);
  BmpWinFrame_CgxSet(GFL_BG_FRAME1_M,COMM_TALK_WIN_CGX_NUM, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);
  BmpWinFrame_Write( wk->MyInfoWinBack, WINDOW_TRANS_ON, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
}

// �_�~�[�I�u�W�F�N�g��o�^���܂���
static void DEBUG_DummyObjIn( WIFIP2PMATCH_WORK *wk, int num )
{
  int i;
  MCR_MOVEOBJ* p_moveobj;
  static const u8 view_list[ 32 ][2] = {
    { PLBOY1, PM_MALE },
    { PLBOY3, PM_MALE },
    { PLMAN3, PM_MALE },
    { PLBADMAN, PM_MALE },
    { PLEXPLORE, PM_MALE },
    { PLFIGHTER, PM_MALE },
    { PLGORGGEOUSM, PM_MALE },
    { PLMYSTERY, PM_MALE },
    { PLGIRL1, PM_FEMALE },
    { PLGIRL2, PM_FEMALE },
    { PLWOMAN2, PM_FEMALE },
    { PLWOMAN3, PM_FEMALE },
    { PLIDOL, PM_FEMALE },
    { PLLADY, PM_FEMALE },
    { PLCOWGIRL, PM_FEMALE },
    { PLGORGGEOUSW, PM_FEMALE },
    { PLBOY1, PM_MALE },
    { PLBOY3, PM_MALE },
    { PLMAN3, PM_MALE },
    { PLBADMAN, PM_MALE },
    { PLEXPLORE, PM_MALE },
    { PLFIGHTER, PM_MALE },
    { PLGORGGEOUSM, PM_MALE },
    { PLMYSTERY, PM_MALE },
    { PLGIRL1, PM_FEMALE },
    { PLGIRL2, PM_FEMALE },
    { PLWOMAN2, PM_FEMALE },
    { PLWOMAN3, PM_FEMALE },
    { PLIDOL, PM_FEMALE },
    { PLLADY, PM_FEMALE },
    { PLCOWGIRL, PM_FEMALE },
    { PLGORGGEOUSW, PM_FEMALE },
  };

  for( i=0; i<num; i++ ){
    // �������o��
    p_moveobj = WIFI_MCR_SetNpc( &wk->matchroom, view_list[i][0], i+1 );

    // �݂�Ȕ�ђ��˂�
    WIFI_MCR_NpcMoveSet( &wk->matchroom, p_moveobj, MCR_NPC_MOVE_JUMP );
    MCRSYS_SetMoveObjWork( wk, p_moveobj );	// �o�^�������[�N��ۑ����Ă���

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
  int mySt;
  int targetSt;
  WIFIP2PMATCH_WORK *wk = pWork;
  WIFI_STATUS* pMatch = wk->pMatch;
  WIFI_STATUS* p_status = WifiFriendMatchStatusGet( friendIndex);

  mySt = WIFI_STATUS_GetWifiMode(pMatch);

  targetSt = WIFI_STATUS_GetWifiMode(p_status);

  OS_TPrintf("%d %d\n",mySt, targetSt);
  
  if((mySt == WIFI_STATUS_DBATTLE50_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE50)){
    return TRUE;
  }
  else if((mySt == WIFI_STATUS_DBATTLE100_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE100)){
    return TRUE;
  }
  else if((mySt == WIFI_STATUS_DBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE_FREE)){
    return TRUE;
  }
#ifdef WFP2P_DEBUG_EXON
  else if((mySt == WIFI_STATUS_MBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_MBATTLE_FREE)){
    return TRUE;
  }
#endif
  else if((mySt == WIFI_STATUS_SBATTLE50_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE50)){
    return TRUE;
  }
  else if((mySt == WIFI_STATUS_SBATTLE100_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE100)){
    return TRUE;
  }
  else if((mySt == WIFI_STATUS_SBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE_FREE)){
    return TRUE;
  }
  else if((mySt == WIFI_STATUS_TRADE_WAIT)&&(targetSt == WIFI_STATUS_TRADE)){
    return TRUE;
  }
  else if((mySt == WIFI_STATUS_FRONTIER_WAIT)&&(targetSt == WIFI_STATUS_FRONTIER)){
    return TRUE;
  }
#ifdef WFP2P_DEBUG_EXON
  else if((mySt == WIFI_STATUS_BATTLEROOM_WAIT)&&(targetSt == WIFI_STATUS_BATTLEROOM)){
    return TRUE;
  }
#endif
  else if((mySt == WIFI_STATUS_LOGIN_WAIT)&&(targetSt == WIFI_STATUS_VCT)){
    return TRUE;
  }
  else if((mySt == WIFI_STATUS_VCT)&&(targetSt == WIFI_STATUS_LOGIN_WAIT)){
    return TRUE;
  }
  else if((mySt == WIFI_STATUS_VCT)&&(targetSt == WIFI_STATUS_VCT)){
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
  int i,x;
  int num = 0;
  ARCHANDLE* p_handle;
  MCR_MOVEOBJ* p_moveobj;
  int obj_code;

  // ���C�v�I���҂�
  if( GFL_FADE_CheckFade() ){
    return seq;
  }

  // �G���[�`�F�b�N
  if( GFL_NET_StateIsWifiError() == FALSE ){

    // �ؒf�����VCHAT��W��Ԃɕς�����̂����`�F�b�N
    if(!GFL_NET_StateIsWifiLoginMatchState()){
      return seq;
    }
  }

  // �R�[���o�b�N��ݒ�
  GFL_NET_DWC_SetConnectModeCheckCallback( WIFIP2PModeCheck );

  // ���������͒ʐM�G���[���V�X�e���E�B���h�E�ŏo��

  ConnectBGPalAnm_OccSet(&wk->cbp, FALSE);

  p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, HEAPID_WIFIP2PMATCH );


  // �A�C�R���f�[�^�Ǎ���
  WifiP2PMatchFriendListIconLoad(  &wk->icon, p_handle, HEAPID_WIFIP2PMATCH );

  _myVChatStatusOrgSet(wk);

  //    _timeWaitIconDel(wk);		timeWait����MsgWin��j�����Ă���Ƃ������Ƃ̓��b�Z�[�W�I���ł�OK��
  EndMessageWindowOff(wk);

  GFL_CLACT_SYS_Delete();//


  GFL_BG_ClearFrame(  GFL_BG_FRAME3_M);

  if(wk->menulist){
    GFL_BMPWIN_Delete(wk->ListWin);
    BmpMenuList_Exit(wk->lw, NULL, NULL);
    wk->lw = NULL;
    BmpMenuWork_ListDelete( wk->menulist );
    wk->menulist = NULL;
  }
  if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){

    if( MyStatus_GetMySex(WIFI_STATUS_GetMyStatus(wk->pMatch)) == PM_MALE ){
      obj_code = PLHERO;
    }else{
      obj_code = PLHEROINE;
    }

    // �}�b�`���O���[��������
    wk->friend_num = WifiList_GetFriendDataLastIdx( wk->pList );
#ifdef WFP2PM_MANY_OBJ
    wk->friend_num = 32;
#endif
    WIFI_MCR_Init( &wk->matchroom, HEAPID_WIFIP2PMATCH, p_handle,
                   obj_code, wk->friend_num, ARCID_WIFIP2PMATCH );

    OS_TPrintf("�������o��--\n");
    p_moveobj = WIFI_MCR_SetPlayer( &wk->matchroom, obj_code );
    MCRSYS_SetMoveObjWork( wk, p_moveobj );	// �o�^�������[�N��ۑ����Ă���
#ifdef WFP2PM_MANY_OBJ
    DEBUG_DummyObjIn( wk, wk->friend_num );
#endif
  }
  // �r���[�A�[������
  if( MCVSys_MoveCheck( wk ) == FALSE ){
    MCVSys_Init( wk, p_handle, HEAPID_WIFIP2PMATCH );
  }

  // ���̏�Ԃ���������
  MCVSys_ReWrite( wk, HEAPID_WIFIP2PMATCH );


  // ���[�U�[�f�[�^�\��
  EndMessageWindowOff(wk);
  _readFriendMatchStatus(wk);

  // ���[�U�[�f�[�^��BMP�쐬
  _userDataDisp(wk);

  // WIFI�@�ΐ�AUTOӰ�ރf�o�b�N
#ifndef _WIFI_DEBUG_TUUSHIN
  _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
#else	//_WIFI_DEBUG_TUUSHIN

  if( WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_BATTLE == FALSE ){

    if( WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_MODE == _WIFI_DEBUG_MODE_X ){
      _myStatusChange(wk, WIFI_STATUS_SBATTLE50_WAIT);
      WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = FALSE;
      WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = FALSE;
    }else{
      _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
      WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = TRUE;
      WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = FALSE;
    }

    WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_BATTLE = TRUE;
  }else{
    WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_BATTLE = FALSE;
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
  }
#endif	//_WIFI_DEBUG_TUUSHIN

  // �ʐM��Ԃ����ɖ߂�
  //	GFL_NET_StateChangeWiFiLogin(); //@@OO
  _commStateChange(WIFI_STATUS_LOGIN_WAIT);
  GFL_NET_ChangeInitStruct(&aGFLNetInit);

  wk->preConnect = -1;

  // 080605 tomoya BTS:249�̑Ώ��̂���
  // �e�̒ʐM���Z�b�g�����㒼���ɐڑ����Ă��܂��̂����
  // �i�ڑ�����Ă�VCHAT�̏�Ԃ̂܂܁AFriendList�V�[�P���X�Ɉڍs������j
  //	mydwc_ResetNewPlayer();	// ���Ȃ݂�mydwc_returnLobby���ł����������Ă�
#if 0
  CLACT_SetDrawFlag( wk->MainActWork[_CLACT_LINE_CUR], 1 );
#endif

  GFL_ARC_CloseDataHandle( p_handle );

  // ���C�v�C��
  //	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
  //					COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,
                              16,0,COMM_BRIGHTNESS_SYNC);

  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
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
    // �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi
    // ������VCHAT��ON�ɂ��Ȃ���
    // DWC_RAP����VCHAT���J�n���Ȃ�
    GFL_NET_DWC_SetVChat( WIFI_STATUS_GetVChatStatus( wk->pMatch ));
    NET_PRINT( "Connect VCHAT set\n" );
    return TRUE;
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�F�B�̏o������Ǘ�����
 *
 *	@param	wk	���[�N
 *
 *	@retval	���̗F�B�̐�
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
  //	wk->friendMatchReadCount = friend_num;

  in_flag = FALSE;
  out_flag = FALSE;

  // �o�b�N�A�b�v�Ɣ�r����
  if( memcmp( wk->index2No, wk->index2NoBackUp, sizeof(u8)*WIFIP2PMATCH_MEMBER_MAX ) != 0 ){

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
          WifiP2PMatch_UserDispOff_Target( wk, wk->index2NoBackUp[i], HEAPID_WIFIP2PMATCH );	// ���̐l��������ʂɏڍו\������Ă���Ȃ����
          out_flag = TRUE;
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
                                 MyStatus_GetTrainerView(WIFI_STATUS_GetMyStatus(p_status)), wk->index2No[i] );


        // �o�^�ł������`�F�b�N
        if( p_obj ){

          MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_IN );
          MCRSYS_SetMoveObjWork( wk, p_obj );

          in_flag = TRUE;
        }else{

          // �I�u�W�F���o�^�ł��Ȃ������̂ŗ\��o�^
          // �i���ꂾ�ƃ{�^�����������Ƃ��o���Ȃ��j
          MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_RES );
          wk->index2No[i] = 0;	// �����Ă����̂͂Ȃ��������Ƃɂ���
        }
      }
    }

    // �ŐV�łɕύX
    memcpy( wk->index2NoBackUp, wk->index2No, sizeof(u8)*WIFIP2PMATCH_MEMBER_MAX );

    // �{�^���S�`�惊�N�G�X�g
    MCVSys_BttnAllWriteReq( wk );

    if( in_flag == TRUE ){
      Snd_SePlay( _SE_INOUT );
    }else if( out_flag == TRUE ){
      Snd_SePlay( _SE_INOUT );
    }
  }


  return friend_num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�F�B�̏�ԕω���\��
 *
 *	@param	wk	���[�N
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
 *	@brief	�F�B�I�u�W�F�N�g�̓o�^�j���Ǘ�
 *
 *	@param	wk	���[�N
 */
//-----------------------------------------------------------------------------
static void MCRSYS_SetFriendObj( WIFIP2PMATCH_WORK* wk, u32 heapID )
{
  // �F�B�i���o�[���ς���Ă����炢�Ȃ��Ȃ����F�B�������A�������F�B��o�^
  MCRSYS_ContFiendInOut( wk );

  // ��Ԃ��ς�����牽����ς���
  MCRSYS_ContFriendStatus( wk, heapID );
}


//------------------------------------------------------------------
/**
 * $brief   �t�����h���X�g�\�������� WIFIP2PMATCH_MODE_FRIENDLIST
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_FriendList( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32	ret = MCR_RET_NONE;
  int checkMatch;
  u32 check_friend;
  MCR_MOVEOBJ* p_obj;
  u32 status;


  // ���C�v�I���҂�
  if( GFL_FADE_CheckFade() ){
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
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
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
          WifiP2PMatchMessagePrint(wk, msg_wifilobby_054, FALSE);
        }
        else{
          WifiP2PMatchMessagePrint(wk, msg_wifilobby_055, FALSE);
        }
        Snd_SePlay(_SE_DESIDE);
        _userDataInfoDisp(wk);
        _CHANGESTATE(wk,WIFIP2PMATCH_VCHATWIN_WAIT);
        WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// ������ʏ�����
        return seq;
      }
    }


    // ������ʂ������Ȃ�
    check_friend = MCVSys_Updata( wk, HEAPID_WIFIP2PMATCH );	// ������ʂ�������
    p_obj = MCRSYS_GetMoveObjWork( wk, check_friend );
    if( check_friend != 0 ){
      if( p_obj ){
        WIFI_MCR_CursorOn( &wk->matchroom, p_obj );
      }else{
        WIFI_MCR_CursorOff( &wk->matchroom );
      }
    }else{
      WIFI_MCR_CursorOff( &wk->matchroom );
    }

  }

  {
    int j;
    for(j = 0; j < WIFIP2PMATCH_MEMBER_MAX;j++){
      if(WIFI_STATUS_IsVChatMac(wk->pMatch, WifiFriendMatchStatusGet( j ))){
        //�������Ăяo����Ă���̂ŁA�ڑ��J�n
        // ��Ԃ��擾
        status = _WifiMyStatusGet( wk, WifiFriendMatchStatusGet( j ) );
        wk->friendNo = j + 1;
        if( WifiP2PMatch_CommWifiBattleStart( wk, j, status ) ){
          WIFI_STATUS_SetVChatMac(wk->pMatch, WifiFriendMatchStatusGet( j ));
          wk->cancelEnableTimer = _CANCELENABLE_TIMER;
          status = WIFI_STATUS_VCT;
          _commStateChange(status);
          _myStatusChange(wk, status);  // �ڑ����ɂȂ�
          _friendNameExpand(wk, j);
          WifiP2PMatchMessagePrint(wk,msg_wifilobby_014, FALSE);
          GF_ASSERT( wk->timeWaitWork == NULL );
          wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
          wk->cancelEnableTimer = _CANCELENABLE_TIMER;
          _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2);
          return seq;
        }
      }
    }
  }


  // �F�B���炱����ɐڑ����Ă����Ƃ��̏���
  checkMatch = _checkParentConnect(wk);
  if( (0 !=  checkMatch) && (wk->preConnect != -1) ){ // �ڑ����Ă���
    NET_PRINT("�ڑ����E���Ă��܂��Ă���-------------\n");
    Snd_SePlay(_SE_OFFER);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_INIT);
    return seq;
  }
  if((wk->preConnect == -1) && (GFL_NET_DWC_IsNewPlayer() != -1)){	// �ʏ�̃R�l�N�g�J�n
    wk->preConnect = GFL_NET_DWC_IsNewPlayer();
    _friendNameExpand(wk, wk->preConnect);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_043, FALSE);

    // �{�C�X�`���b�g�ݒ�
    GFL_NET_DWC_SetVChat(WIFI_STATUS_GetVChatStatus( wk->pMatch ));// �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi
    NET_PRINT( "Connect VCHAT set\n" );

    // �Ȃ������l�̃f�[�^�\��
    //		WifiP2PMatch_UserDispOn( wk, wk->preConnect+1, HEAPID_WIFIP2PMATCH );

    wk->localTime = 0;
  }
  else if((wk->preConnect != -1) && (GFL_NET_DWC_IsNewPlayer() == -1)){	// �����I�ɂ͐V�����R�l�N�g���Ă����̂ɁA���ۂ̓R�l�N�g���Ă��Ă��Ȃ������Ƃ��H

    // �{�C�X�`���b�g�ݒ�
    GFL_NET_DWC_SetVChat(FALSE);// �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi
    _friendNameExpand(wk, wk->preConnect);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    GFL_NET_StateWifiMatchEnd(TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }

  // VCHAT�@ON�@��ԂȂ̂ɁA�V�����R�l�N�g��-1�Ȃ�VCHAT��OFF�ɂ���
  // 080615	tomoya
  if( (GFL_NET_DWC_IsVChat() == TRUE) && (GFL_NET_DWC_IsNewPlayer() == -1) ){
    GFL_NET_DWC_SetVChat(FALSE);// �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi
  }


  // ��Ԃ��擾
  status = _WifiMyStatusGet( wk, wk->pMatch );

  // �N�������ɐڑ����Ă��Ă��Ȃ��Ƃ��������X�g�𓮂�����
  if(wk->preConnect == -1){

    // CANCEL�{�^���ł��ҋ@��Ԃ��N���A
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
      if(_modeWait(status)){	// �҂���Ԃ̂Ƃ�
        Snd_SePlay(_SE_DESIDE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
        WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// ������ʏ�����
        return seq;
      }
    }

    ret = WIFI_MCR_Main( &wk->matchroom );
    WIFI_MCR_PCAnmMain( &wk->matchroom );	// �p�\�R���A�j�����C��
    //        ret = BmpListMain(wk->lw);
  }
  switch(ret){
  case MCR_RET_NONE:
    return seq;
  case MCR_RET_CANCEL:
    Snd_SePlay(_SE_DESIDE);
    if(_modeWait(status)){	// �҂���Ԃ̂Ƃ�
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
    }
    else{	// ���ꂩ�I���`�F�b�N��
      wk->endSeq = WIFI_P2PMATCH_END;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_YESNO);
      WifiP2PMatchMessagePrint(wk, dwc_message_0010, TRUE);
      //        wk->localTime=0;
      ret = BMPMENULIST_CANCEL;
    }
    WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// ������ʏ�����
    return seq;

  case MCR_RET_MYSELECT:
    //        wk->localTime=0;
    Snd_SePlay(_SE_DESIDE);
    if(_modeWait(status)){
      WIFI_MCR_PCAnmStart( &wk->matchroom );	// pc�A�j���J�n
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
    }
    else{  // ��W�̍s�őI�������Ƃ�
      if(status == WIFI_STATUS_LOGIN_WAIT){

        // �����̏�Ԃ��A�}�b�`���O�܂ł����Ă��邩�`�F�b�N����
        // 080628	tomoya
        if( GFL_NET_StateIsWifiLoginMatchState() == TRUE ){
          WIFI_MCR_PCAnmStart( &wk->matchroom );	// pc�A�j���J�n
          _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT);
        }
      }
    }
    WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// ������ʏ�����
    return seq;
    break;

  case MCR_RET_SELECT:
    Snd_SePlay(_SE_DESIDE);
    if(_modeWait(status)){
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
    }
    else{  // �l�̖��O�[���}�b�`���O��
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT);
    }
    WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// ������ʏ�����
    return seq;
    break;

  default:
    GF_ASSERT(0);
    break;
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCTONOFF�E�C���h�E�����
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatToggleWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( 0 !=  _checkParentNewPlayer(wk)){ // �ڑ����Ă���
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    // ��l���̓��������
    FriendRequestWaitOff( wk );
  }
  else if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
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
  vchat	= WIFI_STATUS_GetVChatStatus(p_status);
  status	= _WifiMyStatusGet( wk, p_status );

  wk->cancelEnableTimer--;
  if(wk->cancelEnableTimer < 0  ){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT);
  }
  else if(GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_FAIL){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_012, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT ){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if((GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_DISCONNECTING) || (GFL_NET_StateIsWifiDisconnect())){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
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
  else if( (status != WIFI_STATUS_LOGIN_WAIT) && (status != WIFI_STATUS_VCT) ){
    OS_TPrintf("status not loginwait %d\n",status);
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED){  // �Ȃ�����
    //    _timeWaitIconDel(wk);		timeWait����MsgWin��j�����Ă���Ƃ������Ƃ̓��b�Z�[�W�I���ł�OK��
    EndMessageWindowOff(wk);
    // VCT�ڑ��J�n + �ڑ�MAC�͏���
    WIFI_STATUS_ResetVChatMac(wk->pMatch);
    _myStatusChange(wk, WIFI_STATUS_VCT);  // VCT���ɂȂ�
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
    WifiList_SetLastPlayDate( wk->pList, wk->friendNo - 1);	// �Ō�ɗV�񂾓��t�́AVCT���Ȃ������Ƃ��ɐݒ肷��
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

  // 080703	tomoya takahashi
  // ��͂�͂܂�Ƃ��͂���悤�Ȃ̂ŁA
  //�@�����Ɏ��ɂ����悤�ɏC������
  _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
  WifiP2PMatchMessagePrint(wk, msg_wifilobby_011, FALSE);
  _myStatusChange(wk, WIFI_STATUS_VCT);  // VCT���ɂȂ�
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT);

  WifiList_SetLastPlayDate( wk->pList, GFL_NET_DWC_GetFriendIndex());	// �Ō�ɗV�񂾓��t�́AVCT���Ȃ������Ƃ��ɐݒ肷��

#if 0
  if(GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED){  // �Ȃ�����
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_011, FALSE);
    _myStatusChange(wk, WIFI_STATUS_VCT);  // VCT���ɂȂ�
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT);
  }
#endif

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT�ڑ��܂�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectWait( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT
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

  // 080624�@�ǉ�
  // status���`�F�b�N
  p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
  status	= _WifiMyStatusGet( wk, p_status );
  if( (status != WIFI_STATUS_LOGIN_WAIT) && (status != WIFI_STATUS_VCT) ){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }

  if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
      EndMessageWindowOff(wk);
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
  WIFI_STATUS* p_status;
  int status;


  // 080624�@�ύX
  //if( GFL_NET_DWC_GetFriendIndex() < 0 ){
  //    wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
  //}
  // �ʐM���Z�b�g������index��0��菬�����Ȃ�
  // ��������ؒf
  if( GFL_NET_DWC_GetFriendIndex() < 0 ){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }


  // VChat�t���O���擾
  p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
  status	= _WifiMyStatusGet( wk, p_status );

  // �F�B�����ꂽ��o��
  MCRSYS_SetFriendObj( wk, HEAPID_WIFIP2PMATCH );
  MCVSys_UpdataBttn( wk );

#if 0
  CLACT_SetDrawFlag( wk->MainActWork[_CLACT_VCT_MOVE], 0 );
  CLACT_SetDrawFlag( wk->MainActWork[_CLACT_VCT_STOP], 0 );
#endif
  if(GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_FAIL){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_012, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT ){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if((GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_DISCONNECTING) || (GFL_NET_StateIsWifiDisconnect())){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
  }
  // ����̏�Ԃ��ʏ���orVCT��ԂłȂ��ꍇ�ؒf
  else if( (status != WIFI_STATUS_LOGIN_WAIT) && (status != WIFI_STATUS_VCT) ){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_017, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO);
  }
  else{
    // �A�C�R����������
    if(GFL_NET_DWC_IsSendVoiceAndInc()){
      WifiP2PMatchFriendListIconWrite(   &wk->icon, GFL_BG_FRAME1_M,
                                         PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y, PLAYER_DISP_ICON_IDX_VCTBIG, 0 );
    }
    else{
      WifiP2PMatchFriendListIconWrite(   &wk->icon, GFL_BG_FRAME1_M,
                                         PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y, PLAYER_DISP_ICON_IDX_VCT, 0 );
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
  if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
    return seq;
  }

  //Snd_PlayerSetInitialVolume( SND_HANDLE_BGM, PV_VOL_DEFAULT/3 );
  if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    // �Ō�ɗV�񂾓��t�́AVCT���Ȃ������Ƃ��ɐݒ肷��
    WifiList_SetLastPlayDate( wk->pList, GFL_NET_DWC_GetFriendIndex());
    // �͂��������E�C���h�E���o��
    wk->pYesNoWork =
      BmpMenu_YesNoSelectInit(
        &_yesNoBmpDat,
        MENU_WIN_CGX_NUM, MENU_WIN_PAL,0,
        HEAPID_WIFIP2PMATCH );
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT);
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
  int ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);

  if(ret == BMPMENU_NULL){  // �܂��I��
    if((GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_DISCONNECTING)
       || GFL_NET_StateIsWifiDisconnect()
       || !GFL_NET_IsConnectMember(GFL_NET_NETID_SERVER)){

      // ���[�U�[�f�[�^OFF
      //			WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );
      //
      EndMessageWindowOff(wk);

      _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
      BmpMenu_YesNoMenuExit( wk->pYesNoWork );
      wk->preConnect = -1;
      wk->timer = _RECONECTING_WAIT_TIME;
      _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
    }
    else if( GFL_NET_StateIsWifiError() ){
      _errorDisp(wk);
    }
    return seq;
  }
  else if(ret == 0)
  { // �͂���I�������ꍇ

    if(!GFL_NET_StateIsWifiError()){
      _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
      //GFL_NET_StateWifiBattleMatchEnd();
      GFL_NET_StateWifiMatchEnd(TRUE);
      wk->preConnect = -1;
      wk->timer = _RECONECTING_WAIT_TIME;
      _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
    }
    else
    {
      _errorDisp(wk);
    }
  }
  else
  {  // ��������I�������ꍇ
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
  }
  EndMessageWindowOff(wk);
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
  //	WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );

  status = _WifiMyStatusGet( wk, wk->pMatch );

  if(status != WIFI_STATUS_LOGIN_WAIT){
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
  }
  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    wk->timer = 30;
    return seq;
  }
  wk->timer--;
  if((GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)) || (wk->timer==0)){
    EndMessageWindowOff(wk);
    GFL_NET_StateWifiMatchEnd(TRUE);

    // �ʐM��Ԃ����ɖ߂�
    //GFL_NET_StateChangeWiFiLogin();
    GFL_NET_ChangeInitStruct(&aGFLNetInit);

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
  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    return seq;
  }
  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    EndMessageWindowOff(wk);
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
    GFL_NET_StateWifiMatchEnd(TRUE);
    wk->timer = _RECONECTING_WAIT_TIME;
    _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
  }
  return seq;
}

//WIFIP2PMATCH_MODE_DISCONNECT
static int WifiP2PMatch_Disconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
  // ���[�U�[�f�[�^OFF
  //	WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );
  //
  // �G���[�`�F�b�N
  if( GFL_NET_StateIsWifiError() ){
    _errorDisp(wk);
    return seq;
  }


  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    wk->timer = 30;
    return seq;
  }
  wk->timer--;
  if((GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)) || (wk->timer==0)){
    EndMessageWindowOff(wk);
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);

    // �ʐM��Ԃ����ɖ߂�
    //GFL_NET_StateChangeWiFiLogin();
    GFL_NET_ChangeInitStruct(&aGFLNetInit);

    //        if(wk->menulist==NULL){
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart());
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
 * @param   act		�A�N�^�[�̃|�C���^
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
  WIFI_MCR_PCAnmMain( &wk->matchroom );	// �p�\�R���A�j�����C��
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
  WIFI_MCR_PCAnmMain( &wk->matchroom );	// �p�\�R���A�j�����C��
  if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    // �͂��������E�C���h�E���o��
    wk->pYesNoWork =
      BmpMenu_YesNoSelectInit(
        &_yesNoBmpDat,
        MENU_WIN_CGX_NUM, MENU_WIN_PAL,0,
        HEAPID_WIFIP2PMATCH );
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
  int ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);

  WIFI_MCR_PCAnmMain( &wk->matchroom );	// �p�\�R���A�j�����C��

  if((wk->preConnect == -1) && (GFL_NET_DWC_IsNewPlayer() != -1)){  // �ڑ���������

    // �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi
    GFL_NET_DWC_SetVChat(WIFI_STATUS_GetVChatStatus( wk->pMatch ));
    NET_PRINT( "Connect VCHAT set\n" );

    // ���ł�YesNoSelectMain�ŉ������ĂȂ����
    if( ret == BMPMENU_NULL ){
      BmpMenu_YesNoMenuExit( wk->pYesNoWork );
    }
    ret = 1;  // �������ɕύX
  }
  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }else if(ret == 0){ // �͂���I�������ꍇ
    //��������
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);

    // �ʐM��Ԃ����ɖ߂�
    //		GFL_NET_StateChangeWiFiLogin();
    GFL_NET_ChangeInitStruct(&aGFLNetInit);

    // ��l���̓��������
    FriendRequestWaitOff( wk );
  }
  else{  // ��������I�������ꍇ

    // ��l�������~���ēx�\��
    FriendRequestWaitOff( wk );
    FriendRequestWaitOn( wk, TRUE );
  }
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   �ҋ@��ԂɂȂ�ׂ̑I�����j���[ WIFIP2PMATCH_MODE_SELECT_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
#define WIFI_STSET_SINGLEBATTLE	(WIFI_STATUS_NONE)		// �I�����̖������̂����蓖�Ă�
#define WIFI_STSET_DOUBLEBATTLE	(WIFI_STATUS_VCT)
#define WIFI_STSET_MINIGAME		(WIFI_STATUS_UNKNOWN)

typedef struct {
  u32  str_id;
  u32  param;
} _infoMenu;

//  ���j���[���X�g
#define WIFI_PARENTINFO_MENULIST_MAX	  ( 10 )
#ifdef WFP2P_DEBUG_EXON
#define WIFI_PARENTINFO_MENULIST_COMMON	  ( 5 )	// �ϓ����Ȃ������̐�
#else
#define WIFI_PARENTINFO_MENULIST_COMMON	  ( 3 )	// �ϓ����Ȃ������̐�
#endif
_infoMenu _parentInfoMenuList[ WIFI_PARENTINFO_MENULIST_MAX ] = {
  // �ϓ����Ȃ�����
  { msg_wifilobby_057, (u32)WIFI_STSET_SINGLEBATTLE },
  { msg_wifilobby_058, (u32)WIFI_STSET_DOUBLEBATTLE },
  { msg_wifilobby_025, (u32)WIFI_STATUS_TRADE_WAIT },
#ifdef WFP2P_DEBUG_EXON
  { msg_wifilobby_debug_00, (u32)WIFI_STATUS_BATTLEROOM_WAIT },
  { msg_wifilobby_debug_01, (u32)WIFI_STATUS_MBATTLE_FREE_WAIT },
#endif
};

// �ǉ����X�g�f�[�^
static _infoMenu _minigameMenu = {
  msg_wifilobby_147, (u32)WIFI_STSET_MINIGAME
  };
static _infoMenu _frontierMenu = {
  msg_wifilobby_138, (u32)WIFI_STATUS_FRONTIER_WAIT
  };
static _infoMenu _endMenu = {
  msg_wifilobby_032, (u32)BMPMENULIST_CANCEL
  };


_infoMenu _parentSingleInfoMenuList[] = {
  { msg_wifilobby_059, (u32)WIFI_STATUS_SBATTLE_FREE_WAIT },
  { msg_wifilobby_060, (u32)WIFI_STATUS_SBATTLE50_WAIT },
  { msg_wifilobby_061, (u32)WIFI_STATUS_SBATTLE100_WAIT },
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};

_infoMenu _parentDoubleInfoMenuList[] = {
  { msg_wifilobby_062, (u32)WIFI_STATUS_DBATTLE_FREE_WAIT },
  { msg_wifilobby_063, (u32)WIFI_STATUS_DBATTLE50_WAIT },
  { msg_wifilobby_064, (u32)WIFI_STATUS_DBATTLE100_WAIT },
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};


///�I�����j���[�̃��X�g
static BMPMENULIST_HEADER _parentInfoMenuListHeader = {
  NULL,			// �\�������f�[�^�|�C���^
  _curCallBack,					// �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  NULL,					// ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                   //
  WIFI_PARENTINFO_MENULIST_COMMON,// ���X�g���ڐ�
  WIFI_PARENTINFO_MENULIST_COMMON,// �\���ő區�ڐ�
  0,						// ���x���\���w���W
  8,						// ���ڕ\���w���W
  0,						// �J�[�\���\���w���W
  0,						// �\���x���W
  FBMP_COL_BLACK,			// �����F
  FBMP_COL_WHITE,			// �w�i�F
  FBMP_COL_BLK_SDW,		// �����e�F
  0,						// �����Ԋu�w
  16,						// �����Ԋu�x
  BMPMENULIST_NO_SKIP,		// �y�[�W�X�L�b�v�^�C�v
  0,//FONT_SYSTEM,				// �����w��
  0,						// �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  NULL,                   // work
  _BMPMENULIST_FONTSIZE,			//�����T�C�YX(�h�b�g
  _BMPMENULIST_FONTSIZE,			//�����T�C�YY(�h�b�g
  NULL,		//�\���Ɏg�p���郁�b�Z�[�W�o�b�t
  NULL,		//�\���Ɏg�p����v�����g���[�e�B
  NULL,		//�\���Ɏg�p����v�����g�L���[
  NULL,		//�\���Ɏg�p����t�H���g�n���h��
};

///�I�����j���[�̃��X�g
static const BMPMENULIST_HEADER _parentInfoBattleMenuListHeader = {
  NULL,			// �\�������f�[�^�|�C���^
  _curCallBack,					// �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  NULL,					// ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                   //
  NELEMS(_parentSingleInfoMenuList),	// ���X�g���ڐ�
  NELEMS(_parentSingleInfoMenuList),	// �\���ő區�ڐ�
  0,						// ���x���\���w���W
  8,						// ���ڕ\���w���W
  0,						// �J�[�\���\���w���W
  0,						// �\���x���W
  FBMP_COL_BLACK,			// �����F
  FBMP_COL_WHITE,			// �w�i�F
  FBMP_COL_BLK_SDW,		// �����e�F
  0,						// �����Ԋu�w
  16,						// �����Ԋu�x
  BMPMENULIST_NO_SKIP,		// �y�[�W�X�L�b�v�^�C�v
  0,//FONT_SYSTEM,				// �����w��
  0,						// �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  NULL,                    // work
  _BMPMENULIST_FONTSIZE,			//�����T�C�YX(�h�b�g
  _BMPMENULIST_FONTSIZE,			//�����T�C�YY(�h�b�g
  NULL,		//�\���Ɏg�p���郁�b�Z�[�W�o�b�t
  NULL,		//�\���Ɏg�p����v�����g���[�e�B
  NULL,		//�\���Ɏg�p����v�����g�L���[
  NULL,		//�\���Ɏg�p����t�H���g�n���h��
};


#define PARENTMENU_Y	( 3 )
static int _parentModeSelectMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i,length;
  BMPMENULIST_HEADER list_h;
  _infoMenu* p_menu;

  // ���X�g��{���ݒ�
  length = WIFI_PARENTINFO_MENULIST_COMMON;
  list_h = _parentInfoMenuListHeader;
  p_menu = _parentInfoMenuList;


  // �t�����e�B�A�̗L��
  if( _frontierInCheck( wk ) == TRUE ){
    length ++;			//  ���ڐ��ǉ�
    list_h.count ++;	// ���X�g�ő區�ڐ��ǉ�
    list_h.line ++;		// ���X�g�\���ő吔�ǉ�

    p_menu[ length-1 ] = _frontierMenu;
  }

  // �~�j�Q�[���̗L��
  if( _miniGameInCheck( wk ) == TRUE ){
    length ++;			//  ���ڐ��ǉ�
    list_h.count ++;	// ���X�g�ő區�ڐ��ǉ�
    list_h.line ++;		// ���X�g�\���ő吔�ǉ�

    p_menu[ length-1 ] =_minigameMenu;
  }

  // �I���̒ǉ�
  {
    length ++;			//  ���ڐ��ǉ�
    list_h.count ++;	// ���X�g�ő區�ڐ��ǉ�
    list_h.line ++;		// ���X�g�\���ő吔�ǉ�

    p_menu[ length-1 ] =_endMenu;
  }

  wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
  for(i=0; i< length ; i++){
    BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager, p_menu[i].str_id, p_menu[i].param, HEAPID_WIFIP2PMATCH );
  }

  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  //BMP�E�B���h�E����
  wk->SubListWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M, 16, PARENTMENU_Y, 15, length * 2, FLD_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  //    GFL_BMPWIN_MakeFrameScreen(wk->SubListWin,  COMM_TALK_WIN_CGX_NUM, MENU_WIN_PAL );
  BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_ON, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
  list_h.list = wk->submenulist;
  list_h.win = wk->SubListWin;

  list_h.print_que = wk->SysMsgQue;
  PRINT_UTIL_Setup( &wk->SysMsgPrintUtil , wk->SubListWin );
  list_h.print_util = &wk->SysMsgPrintUtil;
  list_h.font_handle = wk->fontHandle;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->SubListWin), WINCLR_COL(FBMP_COL_WHITE) );

  wk->sublw = BmpMenuList_Set(&list_h, 0, wk->battleCur, HEAPID_WIFIP2PMATCH);

  GFL_BMPWIN_TransVramCharacter(wk->SubListWin);
  GFL_BMPWIN_MakeScreen(wk->SubListWin);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME2_M);

  WifiP2PMatchMessagePrint(wk, msg_wifilobby_006, FALSE);


  WIFI_MCR_PCAnmMain( &wk->matchroom );	// �p�\�R���A�j�����C��

  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_WAIT);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   �ҋ@��ԂɂȂ�ׂ̑I�����j���[ WIFIP2PMATCH_MODE_SELECT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _battleSubMenuInit( WIFIP2PMATCH_WORK *wk, int ret );
static int _parentModeSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32	ret;
  int num = PokeParty_GetBattlePokeNum(wk->pMyPoke);

  WIFI_MCR_PCAnmMain( &wk->matchroom );	// �p�\�R���A�j�����C��


  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){

    // �����Őڑ����Ă���\��������	080617	tomoya
    if( 0 !=  _checkParentNewPlayer(wk)){ // �ڑ����Ă���
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
      wk->SubListWin = _BmpWinDel(wk->SubListWin);
      BmpMenuList_Exit(wk->sublw, NULL, &wk->battleCur);
      BmpMenuWork_ListDelete( wk->submenulist );
      EndMessageWindowOff(wk);
    }
    return seq;
  }
  ret = BmpMenuList_Main(wk->sublw);

  if( 0 !=  _checkParentNewPlayer(wk)){ // �ڑ����Ă���
    ret = BMPMENULIST_CANCEL;
  }
  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    EndMessageWindowOff(wk);
    Snd_SePlay(_SE_DESIDE);
    break;
  default:
    Snd_SePlay(_SE_DESIDE);
    // �����`�F�b�N
    if((ret == WIFI_STATUS_TRADE_WAIT) && (2 > num )){
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_053, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    }
    // �_�u���o�g���`�F�b�N
    else if((ret == WIFI_STSET_DOUBLEBATTLE) && (2 > num)){
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_067, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    }
    // �T�u���X�g������
    else if((ret == WIFI_STSET_SINGLEBATTLE) ||
            (ret == WIFI_STSET_DOUBLEBATTLE) ||
            (ret == WIFI_STSET_MINIGAME) ){
      //            _myStatusChange(wk, ret);
      wk->SubListWin = _BmpWinDel(wk->SubListWin);
      BmpMenuList_Exit(wk->sublw, NULL,  &wk->battleCur);
      BmpMenuWork_ListDelete( wk->submenulist );
      _battleSubMenuInit(wk, ret);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SUBBATTLE_WAIT);
      return seq;
      //            EndMessageWindowOff(wk);
    }
    else{
      _myStatusChange(wk, ret);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
      EndMessageWindowOff(wk);
    }
    {
      BOOL  msg_on = TRUE;

      {
        WifiP2PMatch_CommWifiBattleStart( wk, -1, ret );	// �����ł��ʐM���@��ύX���邽�߂ɂ����Ɉړ�
        _commStateChange( ret );
        /*				��commStateChange�ł悢
				if(ret == WIFI_STATUS_FRONTIER_WAIT){
//					WifiP2PMatch_CommWifiBattleStart( wk, -1, ret );
//
					GFL_NET_SetWifiBothNet(FALSE);
					GFL_NET_StateChangeWiFiFactory();
				}
         */
      }

      if( wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST ){
        // �����~������
        FriendRequestWaitOn( wk, msg_on );
      }
    }
    break;
  }
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, &wk->battleCur);
  BmpMenuWork_ListDelete( wk->submenulist );

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
  u32	ret;

  // �G���[�`�F�b�N
  if( GFL_NET_StateIsWifiError() ){
    _errorDisp(wk);
    return seq;
  }

  WIFI_MCR_PCAnmMain( &wk->matchroom );	// �p�\�R���A�j�����C��
  if( 0 !=  _checkParentNewPlayer(wk)){ // �ڑ����Ă���

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  }
  if(GFL_NET_StateIsWifiLoginMatchState()){
    GFL_NET_SetAutoErrorCheck(FALSE);
    GFL_NET_SetNoChildErrorCheck(TRUE);

    GFL_NET_SetWifiBothNet(FALSE);
    //        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
    //                      COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,
                                0,16,COMM_BRIGHTNESS_SYNC);

    wk->bRetryBattle = FALSE;

    //        CommInfoInitialize( wk->pSaveData, NULL );   //@@OO
    CommCommandWFP2PMF_MatchStartInitialize(wk);

    // ������������
    //        CommInfoSendPokeData();  //@@OO

    // �����̓G���g���[
    //        CommInfoSetEntry( GFL_NET_SystemGetCurrentID() );    //@@OO
    seq = SEQ_OUT;						//�I���V�[�P���X��
  }
  return seq;

}



//------------------------------------------------------------------
/**
 * $brief   �o�g���̏ڍו��������߂� WIFIP2PMATCH_MODE_SUBBATTLE_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _battleSubMenuInit( WIFIP2PMATCH_WORK *wk, int ret )
{
  int i,length;
  BMPMENULIST_HEADER list_h;
  _infoMenu* pMenu;

  switch(ret){
  case WIFI_STSET_SINGLEBATTLE:
    pMenu = _parentSingleInfoMenuList;
    length = NELEMS(_parentSingleInfoMenuList);
    wk->bSingle = 1;
    list_h = _parentInfoBattleMenuListHeader;
    break;

  case WIFI_STSET_DOUBLEBATTLE:
    pMenu = _parentDoubleInfoMenuList;
    length = NELEMS(_parentDoubleInfoMenuList);
    wk->bSingle = 0;
    list_h = _parentInfoBattleMenuListHeader;
    break;

  }

  wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
  for(i=0; i< length ; i++){
    if( pMenu[i].str_id != msg_wifilobby_mg02 ){
      BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager, pMenu[i].str_id, pMenu[i].param,HEAPID_WIFIP2PMATCH );
    }else{
      // �~�j�Q�[���̓^�O�ŕ\������i������Ƃނ��肷���E�E�E�j
      WORDSET_RegisterWiFiLobbyGameName( wk->WordSet, 0, i );
      GFL_MSG_GetString(  wk->MsgManager, pMenu[i].str_id, wk->pExpStrBuf );
      WORDSET_ExpandStr( wk->WordSet, wk->TitleString, wk->pExpStrBuf );
      BmpMenuWork_ListAddString( wk->submenulist, wk->TitleString, pMenu[i].param,HEAPID_WIFIP2PMATCH );
    }
  }
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  //BMP�E�B���h�E����
  wk->SubListWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M, 16, 9, 15, length * 2, FLD_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  //GFL_BMPWIN_MakeFrameScreen(wk->SubListWin,  MENU_WIN_CGX_NUM, MENU_WIN_PAL );
  BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_ON, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
  list_h.list = wk->submenulist;
  list_h.win = wk->SubListWin;
  //    wk->sublw = BmpMenuWork_ListCreate(length,HEAPID_WIFIP2PMATCH);

  list_h.print_que = wk->SysMsgQue;
  PRINT_UTIL_Setup( &wk->SysMsgPrintUtil , wk->SubListWin );
  list_h.print_util = &wk->SysMsgPrintUtil;
  list_h.font_handle = wk->fontHandle;

  wk->sublw = BmpMenuList_Set(&list_h, 0, wk->singleCur[wk->bSingle], HEAPID_WIFIP2PMATCH);
  GFL_BMPWIN_TransVramCharacter(wk->SubListWin);

  GFL_BMPWIN_MakeScreen(wk->SubListWin);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME2_M);

  //    WifiP2PMatchMessagePrint(wk, msg_wifilobby_006, FALSE);

  //    wk->seq = WIFIP2PMATCH_MODE_SELECT_WAIT;
  return TRUE;
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
  u32	ret;

  WIFI_MCR_PCAnmMain( &wk->matchroom );	// �p�\�R���A�j�����C��

  // �����Őڑ����Ă���\��������	080617	tomoya
  if( 0 !=  _checkParentNewPlayer(wk)){ // �ڑ����Ă���
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    wk->SubListWin = _BmpWinDel(wk->SubListWin);
    BmpMenuList_Exit(wk->sublw, NULL, &wk->singleCur[wk->bSingle]);
    BmpMenuWork_ListDelete( wk->submenulist );
    EndMessageWindowOff(wk);
    return seq;
  }

  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    return seq;
  }
  ret = BmpMenuList_Main(wk->sublw);
  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    Snd_SePlay(_SE_DESIDE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT);
    break;
  default:
    Snd_SePlay(_SE_DESIDE);
    _myStatusChange(wk, ret);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    EndMessageWindowOff(wk);
    break;
  }


  //	{
  if( wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST ){	// �����I�������Ƃ��̂݁ACANCEL�͂���
    BOOL msg_on = TRUE;
    WifiP2PMatch_CommWifiBattleStart( wk, -1, ret );
    //@@OO            GFL_NET_StateChangeWiFiBattle();
    // �����~������
    FriendRequestWaitOn( wk, msg_on );
    //    _commStateChange(ret);
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

enum{
  _CONNECTING = 1,
  _INFOVIEW,
};


static const struct {
  u32  str_id;
  u32  param;
}_childMatchMenuList[] = {
  { msg_wifilobby_030, (u32)_CONNECTING },
  //    { msg_wifilobby_031, (u32)_INFOVIEW },
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};

///�I�����j���[�̃��X�g
static const BMPMENULIST_HEADER _childMatchMenuListHeader = {
  NULL,			// �\�������f�[�^�|�C���^
  _curCallBack,					// �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  NULL,					// ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                   //
  NELEMS(_childMatchMenuList),	// ���X�g���ڐ�
  NELEMS(_childMatchMenuList),	// �\���ő區�ڐ�
  0,						// ���x���\���w���W
  8,						// ���ڕ\���w���W
  0,						// �J�[�\���\���w���W
  0,						// �\���x���W
  FBMP_COL_BLACK,			// �����F
  FBMP_COL_WHITE,			// �w�i�F
  FBMP_COL_BLK_SDW,		// �����e�F
  0,						// �����Ԋu�w
  16,						// �����Ԋu�x
  BMPMENULIST_NO_SKIP,		// �y�[�W�X�L�b�v�^�C�v
  0,//FONT_SYSTEM,				// �����w��
  0,						// �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  NULL,                    // work
  _BMPMENULIST_FONTSIZE,			//�����T�C�YX(�h�b�g
  _BMPMENULIST_FONTSIZE,			//�����T�C�YY(�h�b�g
  NULL,		//�\���Ɏg�p���郁�b�Z�[�W�o�b�t
  NULL,		//�\���Ɏg�p����v�����g���[�e�B
  NULL,		//�\���Ɏg�p����v�����g�L���[
  NULL,		//�\���Ɏg�p����t�H���g�n���h��
};


static int _childModeMatchMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  int gmmno;
  u16 friendNo,status;
  WIFI_STATUS* p_status;
  MCR_MOVEOBJ* p_player;
  MCR_MOVEOBJ* p_npc;
  u32 way;

  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  NET_PRINT("_childModeMatchMenuInit %d\n",friendNo);

  //NPC�������̕����Ɍ�����
  p_player = MCRSYS_GetMoveObjWork( wk, 0 );
  GF_ASSERT( p_player );

  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );
  // ���肪���Ȃ��Ȃ�����
  // ����̃X�e�[�^�X���Ⴄ���̂ɕς������A
  // �\���������Č��ɖ߂�
  if( p_npc == NULL ){
    _friendNameExpand(wk, friendNo - 1);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    return seq;
  }

  way = WIFI_MCR_GetRetWay( p_player );
  WIFI_MCR_NpcPauseOn( &wk->matchroom, p_npc, way );


  // ��ԕۑ�
  p_status = WifiFriendMatchStatusGet( friendNo - 1 );
  status = _WifiMyStatusGet( wk, p_status );
  wk->keepStatus = status;


  _friendNameExpand(wk, friendNo - 1);
  if(status == WIFI_STATUS_TRADE_WAIT){
    gmmno = msg_wifilobby_004;
  }
  else if(status == WIFI_STATUS_FRONTIER_WAIT){
    gmmno = msg_wifilobby_137;
  }
  else if(status == WIFI_STATUS_FRONTIER){
    gmmno = msg_wifilobby_140;
  }
#ifdef WFP2P_DEBUG_EXON
  else if(status == WIFI_STATUS_BATTLEROOM_WAIT){
    gmmno = msg_wifilobby_debug_00;
  }
  else if(status == WIFI_STATUS_BATTLEROOM){
    gmmno = msg_wifilobby_debug_00;
  }
#endif
  else if(_modeBattleWait(status)){
    gmmno = msg_wifilobby_003;
  }
  else if(status == WIFI_STATUS_TRADE){
    gmmno = msg_wifilobby_049;
  }
  else if(_modeBattle(status)){
    gmmno = msg_wifilobby_048;
  }
  else if(status == WIFI_STATUS_VCT){
    gmmno = msg_wifilobby_050;
  }
  else{
    gmmno = msg_wifilobby_005;
  }
  WifiP2PMatchMessagePrint(wk, gmmno, FALSE);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT2);
  return seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�̏I�������āA���X�g���o���܂� WIFIP2PMATCH_MODE_MATCH_INIT2
 */
//-----------------------------------------------------------------------------
static int _childModeMatchMenuInit2( WIFIP2PMATCH_WORK *wk, int seq )
{
  MCR_MOVEOBJ* p_npc;
  u32 checkMatch;
  int friendNo, status;
  WIFI_STATUS* p_status;


  // �G���[�`�F�b�N
  if( GFL_NET_StateIsWifiError() ){
    //        wk->localTime=0;
    _errorDisp(wk);
    return seq;
  }

  // �b�������Ă���F�B�i���o�[�擾
  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

  checkMatch = _checkParentNewPlayer(wk);
  if( 0 !=  checkMatch ){ // �ڑ����Ă���
    // NPC�����ɖ߂�
    WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    EndMessageWindowOff(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    return seq;
  }

  // ���肪���Ȃ��Ȃ�����
  // ����̃X�e�[�^�X���Ⴄ���̂ɕς������A
  // �\���������Č��ɖ߂�
  if( p_npc == NULL ){

    // NPC�����ɖ߂�
    WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );

    BmpMenuList_Exit( wk->sublw, NULL, NULL );
    _friendNameExpand(wk, friendNo - 1);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    return seq;
  }else{
    p_status = WifiFriendMatchStatusGet( friendNo - 1 );
    status = _WifiMyStatusGet( wk, p_status );

    // ��Ԃ����������
    if((wk->keepStatus != status)){

      // NPC�����ɖ߂�
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );

      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
      return seq;
    }
  }


  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    return seq;
  }

  {
    int i,length;
    BMPMENULIST_HEADER list_h;
    u16 friendNo,status,vchat;
    WIFI_STATUS* p_status;

    length = NELEMS(_childMatchMenuList);
    list_h = _childMatchMenuListHeader;
    friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
    p_status = WifiFriendMatchStatusGet( friendNo - 1 );
    status = _WifiMyStatusGet( wk, p_status );
    vchat = WIFI_STATUS_GetVChatStatus(p_status);
    wk->keepVChat = vchat;
    vchat += WIFI_STATUS_GetVChatStatus( wk->pMatch );

    // ���̐l�̏���\��
    //	WifiP2PMatch_UserDispOn_MyAcces( wk, friendNo, HEAPID_WIFIP2PMATCH );


    wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
    i = 0;

    for(i = 0; i < NELEMS(_childMatchMenuList);i++){
      if(i == 0){
        if(_modeActive(status) || (status == WIFI_STATUS_NONE) ||
           (status == WIFI_STATUS_PLAY_OTHER) ||
           (status >= WIFI_STATUS_UNKNOWN)){  // ���������ނ��o���Ȃ�����
          list_h.line -= 1;
          list_h.count -= 1;
          length -= 1;
        }
        else if(status == WIFI_STATUS_LOGIN_WAIT){
          if(vchat == 2){
            BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager,
                                              msg_wifilobby_027,
                                              _childMatchMenuList[i].param,HEAPID_WIFIP2PMATCH );
          }
          else{
            list_h.line -= 1;
            list_h.count -= 1;
            length -= 1;
          }
        }
        else{
          BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager,
                                            _childMatchMenuList[i].str_id,
                                            _childMatchMenuList[i].param,HEAPID_WIFIP2PMATCH );
        }
      }
      else{
        BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager,
                                          _childMatchMenuList[i].str_id,
                                          _childMatchMenuList[i].param,HEAPID_WIFIP2PMATCH );
      }
    }
    //BMP�E�B���h�E����
    wk->SubListWin = GFL_BMPWIN_Create(
      GFL_BG_FRAME2_M, 16, 11+ ((3-length)*2), 15  , length * 2,
      FLD_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
    //    GFL_BMPWIN_MakeFrameScreen(wk->SubListWin,  MENU_WIN_CGX_NUM, MENU_WIN_PAL );
    BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_ON, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
    list_h.list = wk->submenulist;
    list_h.win = wk->SubListWin;

    list_h.print_que = wk->SysMsgQue;
    PRINT_UTIL_Setup( &wk->SysMsgPrintUtil , wk->SubListWin );
    list_h.print_util = &wk->SysMsgPrintUtil;
    list_h.font_handle = wk->fontHandle;

    //wk->sublw = BmpMenuWork_ListCreate(NELEMS(_childMatchMenuList),HEAPID_WIFIP2PMATCH);
    wk->sublw = BmpMenuList_Set(&list_h, 0, 0, HEAPID_WIFIP2PMATCH);
    GFL_BMPWIN_TransVramCharacter(wk->SubListWin);
    GFL_BMPWIN_MakeScreen(wk->SubListWin);
    GFL_BG_LoadScreenReq(GFL_BG_FRAME2_M);
  }

  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_WAIT);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ���m�̃A�C�e������
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _isItemCheck(WIFI_STATUS* pTargetStatus)
{
  int i;

  for(i = 0; i < _POKEMON_NUM; i++){
    if(MONSNO_DAMETAMAGO == WIFI_STATUS_GetMonsNo(pTargetStatus,i)){
      return FALSE;
    }
    if(MONSNO_MAX < WIFI_STATUS_GetMonsNo(pTargetStatus,i)){
      return FALSE;
    }
    if(ITEM_DATA_MAX < WIFI_STATUS_GetItemNo(pTargetStatus,i)){
      return FALSE;
    }
  }
  return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	���r�[�p��BGM��ݒ肷��
 */
//-----------------------------------------------------------------------------
static void WifiP2P_SetLobbyBgm( void )
{
  u32 bgm;

  // �{�C�X�`���b�g�Ȃ��ɂ���
  GFL_NET_DWC_SetVChat(FALSE);		// �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi

  if( WifiP2P_CheckLobbyBgm() == FALSE ){

    if( GFL_RTC_IsNightTime() == FALSE ){			//FALSE=���ATRUE=��
      bgm = SEQ_PC_01;
    }else{
      bgm = SEQ_PC_02;
    }
    Snd_SceneSet( SND_SCENE_DUMMY );
    Snd_DataSetByScene( SND_SCENE_P2P, bgm, 1 );	//wifi���r�[�Đ�
  }else{
    // �����a�f�l����Ă��Ă��A���ʂ������ɖ߂�
    Snd_PlayerSetInitialVolumeBySeqNo( Snd_NowBgmNoGet(), BGM_WIFILOBBY_VOL );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���r�[�p��BGM���Đ������擾����
 *
 *	@retval	TRUE	�Đ���
 *	@retval	FALSE	����BGM���Đ���
 */
//-----------------------------------------------------------------------------
static BOOL WifiP2P_CheckLobbyBgm( void )
{
  u32 now_bgm;

  now_bgm = Snd_NowBgmNoGet();

  if( (now_bgm != SEQ_PC_01) &&
      (now_bgm != SEQ_PC_02) ){
    return FALSE;
  }
  return TRUE;
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
  u32	ret;
  int status,friendNo,message = 0,vchat;
  u16 mainCursor;
  int checkMatch;
  MCR_MOVEOBJ* p_npc;
  WIFI_STATUS* p_status;

  ret = BmpMenuList_Main(wk->sublw);

  checkMatch = _checkParentNewPlayer(wk);
  if( 0 !=  checkMatch ){ // �ڑ����Ă���
    ret = BMPMENULIST_CANCEL;
  }

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

      // ��Ԃ����������
      // 4�l��W�̃Q�[������Ȃ��̂ɁAVCHAT�t���O���ς������ؒf
      if((wk->keepStatus != status) ||
         ( (wk->keepVChat != vchat)) ){

        // NPC�����ɖ߂�
        // �����ł���Ă��܂���PauseOff���Q�d�ł��ł��܂�
        //				WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );

        _friendNameExpand(wk, friendNo - 1);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
        message = 1;
        break;
      }
    }
    return seq;

  case BMPMENULIST_CANCEL:
    Snd_SePlay(_SE_DESIDE);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    break;
  default:
    Snd_SePlay(_SE_DESIDE);
    if(ret == _CONNECTING){  // �q�@���e�@�ɐڑ�
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
      //            BmpListDirectPosGet(wk->lw,&mainCursor);
      //		    friendNo = wk->index2No[mainCursor];
      wk->friendNo = friendNo;
      if(  friendNo != 0 ){
        int num = PokeParty_GetBattlePokeNum(wk->pMyPoke);

        p_status = WifiFriendMatchStatusGet( friendNo - 1 );
        status = _WifiMyStatusGet( wk, p_status );
        vchat = WIFI_STATUS_GetVChatStatus(p_status);


        // ��Ԃ����������
        // 4�l��W�̃Q�[������Ȃ��̂ɁAVCHAT�t���O���ς������ؒf
        if((wk->keepStatus != status) ||
           ((wk->keepVChat != vchat)) ){
          _friendNameExpand(wk, friendNo - 1);
          WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
          _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
          message = 1;
          break;
        }
        // �s���A�C�e���`�F�b�N
        if(!_isItemCheck(p_status)){
          _friendNameExpand(wk, friendNo - 1);
          WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
          _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
          message = 1;
          break;
        }
        // �����X�^�[�Q�΃`�F�b�N
        if(_is2pokeMode(status) && (2 > num)){
          if(WIFI_STATUS_TRADE_WAIT==status){
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_053, FALSE);
          }
          else{
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_067, FALSE);
          }
          _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
          message = 1;
          break;
        }
        // VCT���[�h�`�F�b�N
        // 2�`4�l�}�b�`���O�̏ꍇVCT���[�h�����킹��K�v�͂Ȃ�
        {

          if(vchat != WIFI_STATUS_GetVChatStatus( wk->pMatch )){  // ������VCHAT�Ə�Ԃ��Ⴄ�ꍇ���b�Z�[�W
            if(vchat){
              WifiP2PMatchMessagePrint(wk, msg_wifilobby_069, FALSE);
            }
            else{
              WifiP2PMatchMessagePrint(wk, msg_wifilobby_070, FALSE);
            }

            // 080703	tomoya takahashi
            // VCHAT��ON�EOFF�𕷂��ӏ��ɂ�������
            // �������咆�Ƃ������Ƃɂ��āA
            // �R�҂̉��傪�ł��Ȃ��悤�ɂ���
            _myStatusChange(wk, _convertState(status));  //

            _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCHAT_NEGO);
            message = 1;
            break;
          }
        }
        status = _convertState(status);
        if(WIFI_STATUS_UNKNOWN == status){
          break;
        }

#if 1  //�ŏ��̓T�[�o�łȂ� �������T�[�o�ɂȂ��đ���Ɍq���ł��炤
        // �������̓����_���}�b�`�Ȃ̂ő��݂ɐe�q���������
        if( WifiDwc_getFriendStatus(friendNo - 1) == DWC_STATUS_ONLINE )
        {
          NET_PRINT( "wifi�ڑ��� %d %d\n", friendNo - 1,status );
          
          status = WIFI_STATUS_LOGIN_WAIT;

          if( WifiP2PMatch_CommWifiBattleStart( wk, -1, status ) ){

            WIFI_STATUS_SetVChatMac(wk->pMatch, WifiFriendMatchStatusGet( friendNo - 1 ));

            wk->cancelEnableTimer = _CANCELENABLE_TIMER;
            _commStateChange(status);
            _myStatusChange(wk, status);  // �ڑ����ɂȂ�
            _friendNameExpand(wk, friendNo - 1);
            WifiP2PMatchMessagePrint(wk,msg_wifilobby_014, FALSE);
            GF_ASSERT( wk->timeWaitWork == NULL );
            wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
//            if(status != WIFI_STATUS_VCT){
//              _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_LOOP);
//            }
//            else
            {
              wk->cancelEnableTimer = _CANCELENABLE_TIMER;
              _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2);
            }
            message = 1;
          }else{
            _friendNameExpand(wk, friendNo - 1);
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
            _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
            message = 1;
          }

        }
        else
        {
          _friendNameExpand(wk, friendNo - 1);
          WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
          _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
          message = 1;
          break;
        }


#else
        if( WifiDwc_getFriendStatus(friendNo - 1) == DWC_STATUS_MATCH_SC_SV ){
          NET_PRINT( "wifi�ڑ��� %d\n", friendNo - 1 );

          if( WifiP2PMatch_CommWifiBattleStart( wk, friendNo - 1, status ) ){
            wk->cancelEnableTimer = _CANCELENABLE_TIMER;
            _commStateChange(status);
            _myStatusChange(wk, status);  // �ڑ����ɂȂ�
            _friendNameExpand(wk, friendNo - 1);
            WifiP2PMatchMessagePrint(wk,msg_wifilobby_014, FALSE);
            GF_ASSERT( wk->timeWaitWork == NULL );
            wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
            if(status != WIFI_STATUS_VCT){
              _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_LOOP);
            }
            else{
              wk->cancelEnableTimer = _CANCELENABLE_TIMER;
              _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2);
            }
            message = 1;
          }else{
            _friendNameExpand(wk, friendNo - 1);
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
            _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
            message = 1;
          }
        }else{

          _friendNameExpand(wk, friendNo - 1);
          WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
          _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
          message = 1;
          break;
        }
#endif //VCTON
      }
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
    //		WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );

  }else{

    // �q����ɂ����Ƃ�

  }

  return seq;
}


//WIFIP2PMATCH_MODE_MATCH_LOOP
static int _childModeMatchMenuLoop( WIFIP2PMATCH_WORK *wk, int seq )
{
  int status;


  wk->cancelEnableTimer--;
  if(wk->cancelEnableTimer < 0  ){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT);
  }
  else if(GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT ){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_FAIL){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_012, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if((GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_DISCONNECTING) || (GFL_NET_StateIsWifiDisconnect())){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
  }
  else if(GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED){  // ����ɐڑ�����

    // �Q�`�S�l��W�łȂ��Ƃ�
    status = _WifiMyStatusGet( wk, wk->pMatch );
    {

      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
      //    _timeWaitIconDel(wk);		timeWait����MsgWin��j�����Ă���Ƃ������Ƃ̓��b�Z�[�W�I���ł�OK��
      EndMessageWindowOff(wk);
      //		CommInfoInitialize(wk->pSaveData,NULL);   //Info������   //@@OO
      CommCommandWFP2PMF_MatchStartInitialize(wk);
      wk->timer = 30;

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
  _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
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
 * $brief   �e�@��t�܂��B�q�@���牞�傪���������Ƃ�m�点��
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

  p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
  mySt = _WifiMyStatusGet( wk, wk->pMatch );
  targetSt = _WifiMyStatusGet( wk,p_status );
  myvchat	= WIFI_STATUS_GetVChatStatus( wk->pMatch );

  if((mySt == WIFI_STATUS_DBATTLE50_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE50)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
  else if((mySt == WIFI_STATUS_DBATTLE100_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE100)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
  else if((mySt == WIFI_STATUS_DBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE_FREE)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
#ifdef WFP2P_DEBUG_EXON
  else if((mySt == WIFI_STATUS_MBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_MBATTLE_FREE)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
#endif
  else if((mySt == WIFI_STATUS_SBATTLE50_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE50)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
  else if((mySt == WIFI_STATUS_SBATTLE100_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE100)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
  else if((mySt == WIFI_STATUS_SBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE_FREE)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
  else if((mySt == WIFI_STATUS_TRADE_WAIT)&&(targetSt == WIFI_STATUS_TRADE)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
  else if((mySt == WIFI_STATUS_FRONTIER_WAIT)&&(targetSt == WIFI_STATUS_FRONTIER)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
#ifdef WFP2P_DEBUG_EXON
  else if((mySt == WIFI_STATUS_BATTLEROOM_WAIT)&&(targetSt == WIFI_STATUS_BATTLEROOM)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  }
#endif
  else if((mySt == WIFI_STATUS_LOGIN_WAIT)&&(targetSt == WIFI_STATUS_VCT)&&(myvchat > 0)){	// 080703 tomoya VCHAT��ON�ł��邱�Ƃ������ɒǉ�
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_INIT);
    return seq;
  }
  else{
    // �K�����Ȃ�=>�ؒf������
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
    GFL_NET_StateWifiMatchEnd(TRUE);
    return seq;
  }
  _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
  WifiP2PMatchMessagePrint(wk, msg_wifilobby_008, FALSE);
  //   CommInfoInitialize(wk->pSaveData,NULL);   //Info������  //@@OO
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
    NET_PRINT("_connectingErrFunc%d \n",GFL_NET_StateGetWifiStatus());
    _friendNameExpand(wk, wk->preConnect);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
  }
  else{

#if 1//PL_T0857_080711_FIX
    // �F�B��STATUS��VCHAT���`�F�b�N	����Ă�����ؒf
    {
      int mySt;
      int targetSt, targetSt_org;
      WIFI_STATUS* p_status;
      int myvchat, targetvchat;

      p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
      mySt		= _WifiMyStatusGet( wk, wk->pMatch );
      targetSt_org= _WifiMyStatusGet( wk,p_status );
      targetSt	= _convertState(targetSt_org);
      myvchat		= WIFI_STATUS_GetVChatStatus( wk->pMatch );
      targetvchat	= WIFI_STATUS_GetVChatStatus(p_status);

      //			NET_PRINT( "check mystart=%d tastatus=%d tastatus_org=%d myvchat=%d tavchat=%d \n",
      //					mySt, targetSt, targetSt_org, myvchat, targetvchat );

      if( ((mySt != targetSt) && (mySt != targetSt_org)) ||
          (myvchat != targetvchat) ){
        _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);

      }else{

        return FALSE;
      }
    }
#else
    return FALSE;
#endif
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
  if((GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_DISCONNECTING) || GFL_NET_StateIsWifiDisconnect() ){
    NET_PRINT("DISCONNECT %d %d\n",GFL_NET_StateGetWifiStatus(),GFL_NET_StateIsWifiDisconnect());

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
        //@@OO                CommToolTempDataReset();


        GFL_STD_MemClear(&wk->matchState,sizeof(wk->matchState));
        GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_CHECK2);
        wk->bRetryBattle = FALSE;
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_SEND);
      }
    }
    else{
      if(wk->timer == 1){
        if(GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())!=0){
          NET_PRINT("������ %d\n",GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()));
          if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
            wk->timer = 0;
          }
        }
        else if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
          NET_PRINT("������ %d\n",GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()));
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
 * $brief   �Ȃ���ׂ��X�e�[�g����M
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvGameStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;
  const u16 *pRecvData = pData;


  wk->matchState[netID] = pRecvData[0];
  NET_PRINT("WifiP2PMatchRecvGameStatus %d %d\n",wk->matchState[netID], netID);

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
  else if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_GAME_CHECK2)){
    //    else if(CommIsTimingSync(_TIMING_GAME_CHECK2)){
    u16 status = _WifiMyStatusGet( wk, wk->pMatch );
    BOOL result = TRUE;

    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_STATUS, sizeof(u16), &status);
    //@@OO        result = CommToolSetTempData(GFL_NET_SystemGetCurrentID() ,&status);
    if( result ){
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_CHECK);
    }
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

  if(GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) == 0){
    id = 1;
  }
  if(_connectingErrFunc(wk)){
  }
  else if(WIFI_STATUS_NONE != wk->matchState[id]){
    u16 org_status = _WifiMyStatusGet( wk, wk->pMatch );
    u16 status = _convertState(org_status);
    if((wk->matchState[id] == status) || (wk->matchState[id] == org_status)){
      GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_START);
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
  else if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_GAME_START)){
    //   CommInfoSendPokeData();    //@@OO
    GFL_NET_SetAutoErrorCheck(TRUE);
    GFL_NET_SetNoChildErrorCheck(TRUE);

    GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_START2);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_WAIT);
  }
  return seq;
}


static BOOL _parent_MsgEndCheck( WIFIP2PMATCH_WORK *wk )
{
  if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
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
  int status;
  int checkFriend[GFL_NET_MACHINE_MAX];
  WIFI_STATUS* p_status;

  if(_connectingErrFunc(wk)){
  }
  else if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_GAME_START2)
          && (_parent_MsgEndCheck( wk ) == TRUE) ){		// ���b�Z�[�W�̏I�����҂悤�ɕύX 08.06.01	tomoya
    GFL_NET_DWC_FriendAutoInputCheck( WifiList_GetDwcDataPtr(SaveData_GetWifiListData(wk->pSaveData), 0));

    EndMessageWindowOff(wk);

    p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
    status = _WifiMyStatusGet( wk, p_status );
    if((status == WIFI_STATUS_TRADE_WAIT) || (status == WIFI_STATUS_TRADE)){
      status = WIFI_STATUS_TRADE;
      wk->endSeq = WIFI_P2PMATCH_TRADE;
    }
    else if((status == WIFI_STATUS_FRONTIER_WAIT) || (status == WIFI_STATUS_FRONTIER)){
      status = WIFI_STATUS_FRONTIER;
      wk->endSeq = WIFI_P2PMATCH_FRONTIER;
    }
#ifdef WFP2P_DEBUG_EXON
    else if((status == WIFI_STATUS_BATTLEROOM_WAIT) || (status == WIFI_STATUS_BATTLEROOM)){
      status = WIFI_STATUS_BATTLEROOM;
      wk->endSeq = WIFI_P2PMATCH_BATTLEROOM;
    }
#endif
    else if((status == WIFI_STATUS_SBATTLE50_WAIT) || (status == WIFI_STATUS_SBATTLE50)){
      status = WIFI_STATUS_SBATTLE50;
      wk->endSeq = WIFI_P2PMATCH_SBATTLE50;
    }
    else if((status == WIFI_STATUS_SBATTLE100_WAIT) || (status == WIFI_STATUS_SBATTLE100)){
      status = WIFI_STATUS_SBATTLE100;
      wk->endSeq = WIFI_P2PMATCH_SBATTLE100;
    }
    else if((status == WIFI_STATUS_SBATTLE_FREE_WAIT) || (status == WIFI_STATUS_SBATTLE_FREE)){
      status = WIFI_STATUS_SBATTLE_FREE;
      wk->endSeq = WIFI_P2PMATCH_SBATTLE_FREE;
    }
    else if((status == WIFI_STATUS_DBATTLE50_WAIT) || (status == WIFI_STATUS_DBATTLE50)){
      status = WIFI_STATUS_DBATTLE50;
      wk->endSeq = WIFI_P2PMATCH_DBATTLE50;
    }
    else if((status == WIFI_STATUS_DBATTLE100_WAIT) || (status == WIFI_STATUS_DBATTLE100)){
      status = WIFI_STATUS_DBATTLE100;
      wk->endSeq = WIFI_P2PMATCH_DBATTLE100;
    }
    else if((status == WIFI_STATUS_DBATTLE_FREE_WAIT) || (status == WIFI_STATUS_DBATTLE_FREE)){
      status = WIFI_STATUS_DBATTLE_FREE;
      wk->endSeq = WIFI_P2PMATCH_DBATTLE_FREE;
    }
#ifdef WFP2P_DEBUG_EXON
    else if((status == WIFI_STATUS_MBATTLE_FREE_WAIT) || (status == WIFI_STATUS_MBATTLE_FREE)){
      status = WIFI_STATUS_MBATTLE_FREE;
      wk->endSeq = WIFI_P2PMATCH_MBATTLE_FREE;
    }
#endif

    // WIFI�@�ΐ�AUTOӰ�ރf�o�b�N
#ifdef _WIFI_DEBUG_TUUSHIN
    WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = FALSE;
    WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_TOUCH_REQ = FALSE;
#endif	//_WIFI_DEBUG_TUUSHIN

    // �{�E�P���m�[�g
    //WifiP2P_Fnote_Set( wk, GFL_NET_DWC_GetFriendIndex() );

    _myStatusChange(wk, status);  // �ڑ����ɂȂ�
    //        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
    //                        COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,
                                0,16,COMM_BRIGHTNESS_SYNC);


    seq = SEQ_OUT;						//�I���V�[�P���X��
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
  _CHANGESTATE(wk,WifiP2PMatchFriendListStart());
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
  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
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
  if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    // �͂��������E�C���h�E���o��

    BmpWinFrame_GraphicSet(
      GFL_BG_FRAME2_M, MENU_WIN_CGX_NUM, MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );

    wk->pYesNoWork =
      BmpMenu_YesNoSelectInit(
        &_yesNoBmpDat,
        MENU_WIN_CGX_NUM, MENU_WIN_PAL,0,
        HEAPID_WIFIP2PMATCH );
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_WAIT);
  }

  // WIFI�@�ΐ�AUTOӰ�ރf�o�b�N
#ifdef _WIFI_DEBUG_TUUSHIN
  WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = TRUE;
  WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = FALSE;
#endif	//_WIFI_DEBUG_TUUSHIN

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
  int ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);

  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    return seq;
  }

  //  �ڑ��`�F�b�N
  if( 0 !=  _checkParentNewPlayer(wk)){ // �ڑ����Ă���
    if(ret == BMPMENU_NULL){
      BmpMenu_YesNoMenuExit( wk->pYesNoWork );
    }
    ret = BMPMENU_CANCEL;	// CANCEL
  }

  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }else if(ret == 0){ // �͂���I�������ꍇ
    WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXITING);
    wk->timer = 1;
  }
  else{  // ��������I�������ꍇ
    EndMessageWindowOff(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

    // ��l���̓��������
    FriendRequestWaitOff( wk );
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

static int _exitExiting( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    return seq;
  }
  if(wk->timer == 1){
    wk->timer = 0;
    //GFL_NET_StateWifiLogout();
    GFL_NET_Exit(NULL);
  }
  if(!GFL_NET_IsInit()){
    NET_PRINT("�ؒf�������Ƀt�����h�R�[�h���l�߂�\n");
    WifiList_FormUpData(wk->pList);  // �ؒf�������Ƀt�����h�R�[�h���l�߂�
    //SaveData_SaveParts(wk->pSaveData, SVBLK_ID_NORMAL);  //�Z�[�u��
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
  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    return seq;
  }
  wk->timer--;
  if(wk->timer==0){
    wk->endSeq = WIFI_P2PMATCH_END;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
    EndMessageWindowOff(wk);
  }

  // WIFI�@�ΐ�AUTOӰ�ރf�o�b�N
#ifdef _WIFI_DEBUG_TUUSHIN
  WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = FALSE;
  WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = FALSE;
  WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_TOUCH_REQ = FALSE;
#endif	//_WIFI_DEBUG_TUUSHIN
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
  if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_BATTLE_END)==FALSE){
    return seq;
  }
  // �ʐM�������ɓd����؂�ꂽ��A�����Ɠ����҂����Ă��܂��̂ŁA�ʐM������ɃI�[�g�G���[�`�F�b�N��
  // �͂���
  GFL_NET_SetAutoErrorCheck(FALSE);
  GFL_NET_SetNoChildErrorCheck(TRUE);

  if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    // �͂��������E�C���h�E���o��
    wk->pYesNoWork =
      BmpMenu_YesNoSelectInit(
        &_yesNoBmpDat,
        MENU_WIN_CGX_NUM, MENU_WIN_PAL,0,
        HEAPID_WIFIP2PMATCH );
    _CHANGESTATE(wk,WIFIP2PMATCH_NEXTBATTLE_WAIT);
  }

  // WIFI�@�ΐ�AUTOӰ�ރf�o�b�N
#ifdef _WIFI_DEBUG_TUUSHIN
  WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = FALSE;
  WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_TOUCH_REQ = FALSE;
  WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = TRUE;
#endif	//_WIFI_DEBUG_TUUSHIN

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

  if( !PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    return seq;
  }

  if(GFL_NET_StateIsWifiLoginState() || GFL_NET_StateIsWifiDisconnect() || (GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_TIMEOUT)){  // �ؒf���������Ƃ�
    BmpMenu_YesNoMenuExit(wk->pYesNoWork);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_065, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_BATTLE_DISCONNECT);
  }
  else if(GFL_NET_StateIsWifiError()){
    BmpMenu_YesNoMenuExit(wk->pYesNoWork);
    _errorDisp(wk);
  }
  else{
    ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);
    if(ret == BMPMENU_NULL){  // �܂��I��
      return seq;
    }else if(ret == 0){ // �͂���I�������ꍇ
      //EndMessageWindowOff(wk);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_073, FALSE);
      GF_ASSERT( wk->timeWaitWork == NULL );
      wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
      wk->bRetryBattle = TRUE;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
      wk->timer = 30;
    }
    else{  // ��������I�������ꍇ
      EndMessageWindowOff(wk);
      //@@OO            CommInfoFinalize();
      //GFL_NET_StateWifiBattleMatchEnd();
      GFL_NET_StateWifiMatchEnd(TRUE);
      wk->timer = _RECONECTING_WAIT_TIME;
      _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
    }
    //        wk->pMatch = GFL_NET_StateGetMatchWork();  //@@
    //      wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
  }
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
  // ����̏�Ԃ���������肵����I���
  {
    int status,friendNo,vchat;
    MCR_MOVEOBJ* p_npc;
    WIFI_STATUS* p_status;

    // �b�������Ă���F�B�i���o�[�擾
    friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
    p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

    // ���肪���Ȃ��Ȃ�����
    // ����̃X�e�[�^�X���Ⴄ���̂ɕς������A
    // �\���������Č��ɖ߂�
    if( p_npc == NULL ){
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
      return seq;
    }else{
      p_status = WifiFriendMatchStatusGet( friendNo - 1 );
      status = _WifiMyStatusGet( wk, p_status );
      vchat = WIFI_STATUS_GetVChatStatus(p_status);

      // ��Ԃ����������
      if((wk->keepStatus != status) || (wk->keepVChat != vchat)){
        _friendNameExpand(wk, friendNo - 1);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
        return seq;
      }
    }
  }

  if( PRINTSYS_QUE_IsFinished(wk->SysMsgQue) ){
    // �͂��������E�C���h�E���o��
    wk->pYesNoWork =
      BmpMenu_YesNoSelectInit(
        &_yesNoBmpDat,
        MENU_WIN_CGX_NUM, MENU_WIN_PAL,0,
        HEAPID_WIFIP2PMATCH );
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCAHT��ύX���Ă��������ǂ�������
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatNegoWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret,status;

  // ����̏�Ԃ���������肵����I���
  {
    int friendNo,vchat;
    MCR_MOVEOBJ* p_npc;
    WIFI_STATUS* p_status;

    // �b�������Ă���F�B�i���o�[�擾
    friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
    p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

    // ���肪���Ȃ��Ȃ�����
    // ����̃X�e�[�^�X���Ⴄ���̂ɕς������A
    // �\���������Č��ɖ߂�
    if( p_npc == NULL ){
      BmpMenu_YesNoMenuExit(wk->pYesNoWork);
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
      return seq;
    }else{
      p_status = WifiFriendMatchStatusGet( friendNo - 1 );
      status = _WifiMyStatusGet( wk, p_status );
      vchat = WIFI_STATUS_GetVChatStatus(p_status);

      // ��Ԃ����������
      if((wk->keepStatus != status) || (wk->keepVChat != vchat)){
        BmpMenu_YesNoMenuExit(wk->pYesNoWork);
        _friendNameExpand(wk, friendNo - 1);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
        return seq;
      }
    }
  }


  if(GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_TIMEOUT){
    BmpMenu_YesNoMenuExit(wk->pYesNoWork);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);	// �������Ȃ��E�E�E
    GFL_NET_StateWifiMatchEnd(TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    return seq;
  }
  else if(GFL_NET_StateIsWifiError()){
    BmpMenu_YesNoMenuExit(wk->pYesNoWork);
    _errorDisp(wk);
  }
  else{
    ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);
    if(ret == BMPMENU_NULL){  // �܂��I��
      return seq;
    }else if(ret == 0){ // �͂���I�������ꍇ
      // �ڑ��J�n
      status = _convertState(wk->keepStatus);
      if(WIFI_STATUS_UNKNOWN == status){   // ���m�̃X�e�[�g�̏ꍇ���������悤�ɖ߂�
      }
      else if( WifiDwc_getFriendStatus(wk->friendNo - 1) == DWC_STATUS_MATCH_SC_SV ){
        _myVChatStatusToggle(wk); // ������VCHAT�𔽓]
        if( WifiP2PMatch_CommWifiBattleStart( wk, wk->friendNo - 1, status ) ){
          wk->cancelEnableTimer = _CANCELENABLE_TIMER;
          _commStateChange(status);
          GFL_NET_SetWifiBothNet(FALSE);  // VCT���͓������M�̕K�v�Ȃ�
          _myStatusChange(wk, status);  // �ڑ����ɂȂ�
          _friendNameExpand(wk, wk->friendNo - 1);
          WifiP2PMatchMessagePrint(wk,msg_wifilobby_014, FALSE);
          GF_ASSERT( wk->timeWaitWork == NULL );
          wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
          if(status != WIFI_STATUS_VCT){
            _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_LOOP);
            return seq;
          }
          else{
            wk->cancelEnableTimer = _CANCELENABLE_TIMER;
            _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2);
            return seq;
          }
        }
      }
    }
    // ��������I�������ꍇ  �������͓K�����Ȃ��ꍇ
    EndMessageWindowOff(wk);


    // ���̐l�̏�������
    //		WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );

    // �X�e�[�^�X�����ǂ�
    // 080703	tomoya takahashi
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);

    // VCHAT���ɖ߂�
    _myVChatStatusOrgSet( wk );
    _userDataInfoDisp(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    return seq;

  }
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
static int 	WifiP2PMatch_EndWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(!GFL_NET_IsInit()){
    //        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
    //                      COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,
                                0,16,COMM_BRIGHTNESS_SYNC);
    seq = SEQ_OUT;						//�I���V�[�P���X��
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ��b�E�C���h�E�\��
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------


static void WifiP2PMatchMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno, BOOL bSystem )
{
  // ������擾
  u8 speed = CONFIG_GetMsgPrintSpeed(wk->pConfig);

  // TimeWaitIcon�j��
  _timeWaitIconDel( wk );

  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
  wk->MsgWin = _BmpWinDel(wk->MsgWin);

  if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
    PRINTSYS_QUE_Clear(wk->SysMsgQue);
  }

  wk->MsgWin=GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    COMM_MSG_WIN_PX, COMM_MSG_WIN_PY,
    COMM_MSG_WIN_SX, COMM_MSG_WIN_SY,
    COMM_MESFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  if(bSystem){
    GFL_MSG_GetString(  wk->SysMsgManager, msgno, wk->pExpStrBuf );
  }
  else{
    GFL_MSG_GetString(  wk->MsgManager, msgno, wk->pExpStrBuf );
  }
  WORDSET_ExpandStr( wk->WordSet, wk->TalkString, wk->pExpStrBuf );


  // ��b�E�C���h�E�g�`��
  //GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 15 );
  //    BmpTalkWinWrite(&wk->MsgWin, WINDOW_TRANS_OFF,COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
  //GFL_BMPWIN_MakeFrameScreen(wk->MsgWin, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL);


  //------

  BmpWinFrame_CgxSet(GFL_BG_FRAME2_M,COMM_TALK_WIN_CGX_NUM, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*COMM_MESFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  // �V�X�e���E�C���h�E�g�`��
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->MsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(wk->MsgWin);
  GFL_BMPWIN_TransVramCharacter(wk->MsgWin);

  BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );

  //------

  // ������`��J�n
  //	MsgPrintSkipFlagSet( MSG_SKIP_ON );		// ���b�Z�[�W�X�L�b�vON
  //	MsgPrintAutoFlagSet( MSG_AUTO_OFF );	// ���b�Z�[�W��������OFF
  //    wk->MsgIndex = GF_STR_PrintSimple( &wk->MsgWin, FONT_TALK, wk->TalkString,
  //                                     0, 0, speed, NULL);

#if 1
  GFL_FONTSYS_SetDefaultColor();
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MsgWin), 2, 2, wk->TalkString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter(wk->MsgWin);
#endif


  //    PRINT_UTIL_Print(&wk->SysMsgPrintUtil, wk->SysMsgQue,
  //                   0, 0,wk->TalkString,wk->fontHandle);

  //GFL_BMPWIN_TransVramCharacter(wk->MsgWin);
}

//------------------------------------------------------------------
/**
 * $brief   �V�X�e�����b�Z�[�W�E�C���h�E�\��
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------

static void _systemMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno )
{
  _timeWaitIconDel(wk);
  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
  wk->MsgWin = _BmpWinDel(wk->MsgWin);

  //   if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
  //     PRINTSYS_QUE_Clear(wk->SysMsgQue);
  //    }

  wk->SysMsgWin= GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    COMM_SYS_WIN_PX, COMM_SYS_WIN_PY,
    COMM_SYS_WIN_SX, COMM_SYS_WIN_SY,
    COMM_MESFONT_PAL, GFL_BMP_CHRAREA_GET_B);

  BmpWinFrame_CgxSet(GFL_BG_FRAME2_M,COMM_SYS_WIN_CGX, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*COMM_MESFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  // �V�X�e���E�C���h�E�g�`��
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->SysMsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_ClearScreen(wk->SysMsgWin);
  GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
  BmpWinFrame_Write( wk->SysMsgWin, WINDOW_TRANS_ON, COMM_SYS_WIN_CGX, COMM_MESFONT_PAL );

  //    GFL_BMPWIN_ClearScreen(wk->SysMsgWin);
  //   GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
  //    GFL_BMPWIN_MakeFrameScreen(wk->SysMsgWin,  MENU_WIN_CGX_NUM, MENU_WIN_PAL );
  // ������`��J�n
  //    wk->MsgIndex = GF_STR_PrintSimple( &wk->SysMsgWin, FONT_TALK,
  //                                     wk->TalkString, 0, 0, MSG_NO_PUT, NULL);

  //    PRINT_UTIL_Print(&wk->SysMsgPrintUtil, wk->SysMsgQue, 0, 0,wk->TalkString,wk->fontHandle);
#if 1
  GFL_MSG_GetString(  wk->SysMsgManager, msgno, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->WordSet, wk->TalkString, wk->pExpStrBuf );
  GFL_FONTSYS_SetDefaultColor();
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->SysMsgWin), 4, 4, wk->TalkString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter(wk->SysMsgWin);
#endif
  //  GFL_BMPWIN_MakeScreen(wk->SysMsgWin);


}

//------------------------------------------------------------------
/**
 * $brief   �F�l�ԍ��̖��O��expand����
 * @param   msg_index
 * @retval  int		    int friend = GFL_NET_DWC_GetFriendIndex();

 */
//------------------------------------------------------------------
static void _friendNameExpand( WIFIP2PMATCH_WORK *wk, int friendNo)
{
  if(friendNo != -1){
    int sex = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_SEX);
    WifiList_GetFriendName(wk->pList, friendNo, wk->pExpStrBuf);
    WORDSET_RegisterWord( wk->WordSet, 0, wk->pExpStrBuf, sex, TRUE, PM_LANG);
  }
}


//------------------------------------------------------------------
/**
 * $brief
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void EndMessageWindowOff( WIFIP2PMATCH_WORK *wk )
{
  _timeWaitIconDel(wk);
  if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
    PRINTSYS_QUE_Clear(wk->SysMsgQue);
  }
  wk->MsgWin = _BmpWinDel(wk->MsgWin);


}

//------------------------------------------------------------------
/**
 * $brief
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _myStatusChange(WIFIP2PMATCH_WORK *wk, int status)
{
  if(wk->pMatch==NULL){  //0707
    return;
  }
  _myStatusChange_not_send( wk, status );
  _sendMatchStatus(wk);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����̃X�e�[�^�X�ݒ�		�������M�͂��Ȃ�
 *
 *	@param	wk
 *	@param	status
 */
//-----------------------------------------------------------------------------
static void _myStatusChange_not_send(WIFIP2PMATCH_WORK *wk, int status)
{
  int org_status;

  if(wk->pMatch==NULL){  //0707
    return;
  }

  org_status = _WifiMyStatusGet( wk, wk->pMatch );

  if(org_status != status){
    _commStateChange(status);

    WIFI_STATUS_SetWifiMode(wk->pMatch,status);
    //		wk->pMatch->myMatchStatus.status = status;
    if(_modeBattle(status) || (status == WIFI_STATUS_TRADE)|| (status == WIFI_STATUS_FRONTIER)
#ifdef WFP2P_DEBUG_EXON
       ||(status == WIFI_STATUS_BATTLEROOM)
#endif
       ){
      //     Snd_BgmFadeOut( 24, BGM_FADE_VCHAT_TIME);
    }
    else if(status == WIFI_STATUS_VCT){
      Snd_BgmFadeOut( 0, BGM_FADE_VCHAT_TIME); // VCT��Ԃ�

    }
    else if(status == WIFI_STATUS_LOGIN_WAIT){    // �ҋ@���@���O�C������͂���

      // �{�C�X�`���b�g�Ȃ��ɂ���
      GFL_NET_DWC_SetVChat(FALSE);		// �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi

      if( WifiP2P_CheckLobbyBgm() == TRUE ){
        // WiFi�L��ɗ����Ƃ��͂ǂ�ȃ{�����[���ł��ʏ�{�����[���ɂ���
        Snd_BgmFadeIn( BGM_WIFILOBBY_VOL, BGM_FADE_VCHAT_TIME, BGM_FADEIN_START_VOL_NOW);
      }
#if 0
      //�{�C�X�`���b�g���I�����Ă���̂ŁA�ݒ艹�ʂ������l�ɖ߂��I
      //�{���͐ݒ肵�Ă��鏉�����ʂɖ߂��Ȃ��Ƃ����Ȃ����B�B�Ƃ肠��������őΏ��I
      Snd_PlayerSetInitialVolumeBySeqNo( _BGM_MAIN, BGM_WIFILOBBY_VOL );
      Snd_PlayerMoveVolume( SND_HANDLE_BGM, BGM_WIFILOBBY_START_VOL, 0 );
      Snd_BgmFadeIn( BGM_VOL_MAX, BGM_FADE_VCHAT_TIME, BGM_FADEIN_START_VOL_NOW);
      NET_PRINT("Snd_BgmFadeIn\n");
#endif
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

static BOOL _myVChatStatusToggle(WIFIP2PMATCH_WORK *wk)
{
  WIFI_STATUS_SetVChatStatus(wk->pMatch, 1 - WIFI_STATUS_GetVChatStatus( wk->pMatch ));
  _sendMatchStatus(wk);
  return WIFI_STATUS_GetVChatStatus( wk->pMatch );
}

//------------------------------------------------------------------
/**
 * $brief   VCHAT�t���O�̐؂�ւ�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _myVChatStatusToggleOrg(WIFIP2PMATCH_WORK *wk)
{
  //@@oo
  //  wk->pMatch->myMatchStatus.vchat_org = 1 - wk->pMatch->myMatchStatus.vchat_org;
  //wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
  _sendMatchStatus(wk);
  return WIFI_STATUS_GetVChatStatus( wk->pMatch );//wk->pMatch->myMatchStatus.vchat_org;
}

//------------------------------------------------------------------
/**
 * $brief   VCHAT�t���O���I���W�i���ɂ��ǂ�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _myVChatStatusOrgSet(WIFIP2PMATCH_WORK *wk)
{
  //	@@oo
  //  NET_PRINT( "change org %d\n", wk->pMatch->myMatchStatus.vchat_org );
  //  wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
  //  _sendMatchStatus(wk);
  return WIFI_STATUS_GetVChatStatus( wk->pMatch );//wk->pMatch->myMatchStatus.vchat_org;
}


//------------------------------------------------------------------
/**
 * $brief
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _timeWaitIconDel(WIFIP2PMATCH_WORK *wk)
{
  if(wk->timeWaitWork){
    TimeWaitIconTaskDel(wk->timeWaitWork);  // �^�C�}�[�~��
    wk->timeWaitWork = NULL;
    wk->MsgWin = _BmpWinDel(wk->MsgWin);

  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[�f�[�^�����\������
 *
 *	@param	wk			�V�X�e�����[�N
 *	@param	friendNo	�F�B�ԍ�
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
 *	@brief	���[�U�[�f�[�^�����\�������I��
 *
 *	@param	wk			�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  MCVSys_UserDispOff( wk );
  WIFI_MCR_CursorOff( &wk->matchroom );

  // �ĕ`��
  MCVSys_ReWrite( wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\�����̐l�Ȃ狭���I�ɕ\���I��
 *
 *	@param	wk				�V�X�e�����[�N
 *	@param	target_friend	�^�[�Q�b�g�̐l���̃C���f�b�N�X
 *	@param	heapID			�q�[�vID
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
 *	@brief	�����ŃA�N�Z�X�����Ƃ��̃��[�U�[�f�[�^�����\������
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOn_MyAcces( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
  MCVSys_UserDispOn( wk, friendNo, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����ŃA�N�Z�X�����Ƃ��̃��[�U�[�f�[�^�����\�������I��
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
 *	@brief	���삵�Ă��邩�`�F�b�N
 *
 *	@param	wk	�V�X�e�����[�N
 *
 *	@retval	TRUE	���쒆
 *	@retval	FALSE	�񓮍쒆
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
 *	@brief	�F�B�f�[�^�r���[�A�[������
 *
 *	@param	wk			�V�X�e�����[�N
 *	@param	p_handle	�n���h��
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void MCVSys_Init( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
  static const GFL_UI_TP_HITTBL bttndata[ WCR_MAPDATA_1BLOCKOBJNUM+1 ] = {
    {	0,		47,		0,		119 },
    {	48,		95,		0,		119 },
    {	96,		143,	0,		119 },
    {	144,	191,	0,		119 },

    {	0,		47,		128,	255 },
    {	48,		95,		128,	255 },
    {	96,		143,	128,	255 },
    {	144,	191,	128,	255 },
    {GFL_UI_TP_HIT_END,0,0,0},		 //�I���f�[�^
  };

  memset( &wk->view, 0, sizeof(WIFIP2PMATCH_VIEW) );

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
 *	@brief	�F�B�f�[�^�r���[�A�[�j��
 *
 *	@param	wk			�V�X�e�����[�N
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
 *	@brief	�F�B�f�[�^�r���[�A�[�A�b�v�f�[�g
 *
 *	@param	wk			�V�X�e�����[�N
 *
 *	@retval	������Ă���F�B�ԍ�	+ 1
 *	@retval	0	������Ă��Ȃ�
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


      Snd_SePlay( _SE_TBLCHANGE );

      // �w�i�J���[�ύX
      MCVSys_BackDraw( wk );

      // �{�^��������������
      wk->view.button_on = TRUE;
      wk->view.bttn_allchg = TRUE;	// �{�^�������ύX
    }
  }


  // ���[�U�[�f�[�^�\������
  if( wk->view.user_disp == MCV_USERDISP_INIT ){
    wk->view.user_disp = MCV_USERDISP_ON;
    wk->view.user_dispno = WF_USERDISPTYPE_NRML;
    MCVSys_UserDispDraw( wk, heapID );
    MCVSys_OamBttnOn( wk );	// OAM�{�^���\��
  }


  if( wk->view.user_disp == MCV_USERDISP_OFF ){

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
    oambttn_ret = MCVSys_OamBttnMain( wk );

    //  USERD�I���`�F�b�N
    userd_end = MCVSys_UserDispEndCheck( wk, oambttn_ret );

    // �Ȃɂ��L�[���������A�u���ǂ�v����������I������
    if( userd_end == TRUE ){
      wk->view.bttn_chg_friendNo = wk->view.touch_friendNo;	// ���̗F�B�̃{�^�����X�V���Ă��炤
      wk->view.touch_friendNo = 0;
      wk->view.touch_frame = 0;
      wk->view.button_on = TRUE;
      wk->view.bttn_allchg = TRUE;
      wk->view.user_disp = MCV_USERDISP_OFF;
      MCVSys_OamBttnOff( wk );	// �{�^��OFF
      Snd_SePlay( _SE_DESIDE );	// �L�����Z����
    }else{
      // ���E�̃{�^����������Ă�����y�[�W�؂�ւ��P
      if( oambttn_ret != MCV_USERD_BTTN_RET_NONE ){
        Snd_SePlay( _SE_DESIDE );						// �y�[�W�ύX��
        MCVSys_UserDispPageChange( wk, oambttn_ret );	// �ύX����
        MCVSys_UserDispDraw( wk, heapID );
      }
    }
  }

  return wk->view.touch_friendNo;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�F�B�f�[�^�r���[�A�[�A�b�v�f�[�g	�{�^���̕\���̂�
 *
 *	@param	wk	�V�X�e�����[�N
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
 *	@brief	���[�U�[�f�B�X�v���C���I�����邩�`�F�b�N
 *
 *	@param	wk			���[�N
 *	@param	oambttn_ret	OAM�{�^�����C���߂�l
 *
 *	@retval	TRUE	�I��
 *	@retval	FALSE	���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL MCVSys_UserDispEndCheck( WIFIP2PMATCH_WORK *wk, u32 oambttn_ret )
{
  // �����\�����Ă���̂ŏI�����Ȃ�
  if( (wk->view.user_disp == MCV_USERDISP_ONEX) ){
    return FALSE;
  }

  //  �ړ���cont�{�^���̓g���K�[
  if( (GFL_UI_KEY_GetCont() & (PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_KEY_UP|PAD_KEY_DOWN)) ||
      (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_X)) ||
      (oambttn_ret == MCV_USERD_BTTN_RET_BACK) ){
    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ڍו\�����Ă���l�̗F�BIDX���擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�F�B�C���f�b�N�X	(0�Ȃ�\�����ĂȂ�)
 */
//-----------------------------------------------------------------------------
static u32	MCVSys_UserDispGetFriend( const WIFIP2PMATCH_WORK* cp_wk )
{
  return cp_wk->view.touch_friendNo;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[�f�[�^�\��
 *
 *	@param	wk			���[�N
 *	@param	friendNo	�F�B�ԍ�
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
  if( MCVSys_MoveCheck(wk) == TRUE ){
    wk->view.touch_friendNo = friendNo;
    wk->view.touch_frame	= 2;
    wk->view.user_disp		= MCV_USERDISP_ONEX;
    wk->view.user_dispno = WF_USERDISPTYPE_NRML;
    MCVSys_UserDispDraw( wk, heapID );
    MCVSys_OamBttnOnNoBack( wk );	//  �����\���p�{�^���\��
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[�f�[�^OFF
 *
 *	@param	wk			���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispOff( WIFIP2PMATCH_WORK *wk )
{
  if( MCVSys_MoveCheck(wk) == TRUE ){
    wk->view.touch_friendNo = 0;
    wk->view.touch_frame = 0;
    wk->view.user_disp		= MCV_USERDISP_OFF;
    wk->view.button_on		= TRUE;
    wk->view.bttn_allchg = TRUE;
    MCVSys_OamBttnOff( wk );	// �{�^��OFF
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�y�[�W�ύX����
 *
 *	@param	wk				���[�N
 *	@param	oambttn_ret		�{�^���߂�l
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispPageChange( WIFIP2PMATCH_WORK *wk, u32 oambttn_ret )
{
  u32 typenum;

  // �t�����e�B�A�L���ŁA�\������ς���
  if( _frontierInCheck(wk) == TRUE ){
    typenum = WF_USERDISPTYPE_NUM;
  }else{
    typenum = WF_USERDISPTYPE_MINI+1;	// �~�j�Q�[���܂ŕ\��
  }

  if( oambttn_ret == MCV_USERD_BTTN_RET_RIGHT ){
    wk->view.user_dispno = (wk->view.user_dispno+1) % typenum;
  }else{
    wk->view.user_dispno --;
    if( wk->view.user_dispno < 0 ){
      wk->view.user_dispno += typenum;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���C�x���g�R�[���o�b�N
 *
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  WIFIP2PMATCH_WORK *wk = p_work;
  u32 friendNo;

  // �F�B�ԍ��擾
  friendNo = (wk->view.frame_no * WCR_MAPDATA_1BLOCKOBJNUM) + bttnid;
  friendNo ++;	// �������O������

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
  case GFL_BMN_EVENT_TOUCH:		///< �G�ꂽ�u��
    wk->view.touch_friendNo = friendNo;
    Snd_SePlay( _SE_DESIDE );
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�f�[�^�ݒ�
 *
 *	@param	wk			�V�X�e�����[�N
 *	@param	p_handle	�A�[�J�C�u�n���h��
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicSet( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
  int i, j;
  int x, y;

  // BG�ݒ�
  // FRAME0_S�X�N���[���f�[�^�N���[��
  GFL_BG_ClearFrame(   GFL_BG_FRAME0_S );
  // �p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_btm_NCLR,	// �w�i�p
                                    PALTYPE_SUB_BG, MCV_PAL_BACK*32, MCV_PAL_BACK_NUM*32, heapID );
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NCLR,	// �{�^���p
                                    PALTYPE_SUB_BG, MCV_PAL_BTTN*32, MCV_PAL_BTTN_NUM*32, heapID );

  // �L�����N�^�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_wf_match_btm_NCGR,
                                        GFL_BG_FRAME0_S, MCV_CGX_BACK, 0, FALSE, heapID );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NCGR,
                                        GFL_BG_FRAME2_S, MCV_CGX_BTTN2, 0, FALSE, heapID );

  // �X�N���[���Ǎ���or�]��
  // �w�i�̓L�����N�^�ʒu������Ă���̂ŃX�N���[���f�[�^������������
  GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_NSCR,   GFL_BG_FRAME0_S, 0, 0, FALSE, heapID );

  // �{�^���X�N���[���Ǎ���
  wk->view.p_bttnbuff = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NSCR, FALSE, &wk->view.p_bttnscrn, heapID );
  MCVSys_GraphicScrnCGOfsChange( wk->view.p_bttnscrn, MCV_CGX_BTTN2 );

  // ���[�U�[�X�N���[���Ǎ���
  for( i=0; i<WF_USERDISPTYPE_NUM; i++ ){
    wk->view.p_userbuff[i] = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_result00_NSCR+i, FALSE, &wk->view.p_userscrn[i], heapID );
    MCVSys_GraphicScrnCGOfsChange( wk->view.p_userscrn[i], MCV_CGX_BTTN2 );
  }

  // �_�~�[�X�N���[���ǂݍ���
  wk->view.p_useretcbuff = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_etc_NSCR, FALSE, &wk->view.p_useretcscrn, heapID );
  MCVSys_GraphicScrnCGOfsChange( wk->view.p_useretcscrn, MCV_CGX_BTTN2 );



  // �t�H���g�p���b�g�Ǎ���
  //	TalkFontPaletteLoad( PALTYPE_SUB_BG, MCV_SYSFONT_PAL*32, heapID );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , 13*0x20, 16*2, heapID );

  // �r�b�g�}�b�v�쐬
  for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){
    x = i/4;	// �z�u��2*4
    y = i%4;
    //		GFL_BG_BmpWinInit( wk->view.nameWin[i] );
    wk->view.nameWin[i] = GFL_BMPWIN_Create(
      GFL_BG_FRAME1_S,
      MCV_NAMEWIN_DEFX+(MCV_NAMEWIN_OFSX*x),
      MCV_NAMEWIN_DEFY+(MCV_NAMEWIN_OFSY*y),
      MCV_NAMEWIN_SIZX, MCV_NAMEWIN_SIZY,
      MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );
    // �����ɂ��ēW�J
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.nameWin[i]), 0 );
    GFL_BMPWIN_TransVramCharacter( wk->view.nameWin[i] );

    // ��Ԗʏ������ݐ�
    for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
      //	GFL_BG_BmpWinInit( wk->view.statusWin[i][j] );
      wk->view.statusWin[i][j] = GFL_BMPWIN_Create(
        GFL_BG_FRAME1_S,
        MCV_STATUSWIN_DEFX+(MCV_STATUSWIN_OFSX*x)+(j*MCV_STATUSWIN_VCHATX),
        MCV_STATUSWIN_DEFY+(MCV_STATUSWIN_OFSY*y),
        MCV_STATUSWIN_SIZX, MCV_STATUSWIN_SIZY,
        PLAYER_DISP_ICON_PLTTOFS_SUB,
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
  GFL_BMPWIN_TransVramCharacter( wk->view.userWin );


  // �T�u���OAM������
  MCVSys_OamBttnInit( wk, p_handle, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�f�[�^�j��
 *
 *	@param	wk	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicDel( WIFIP2PMATCH_WORK *wk )
{
  int i, j;

  // �T�u���OAM�j��
  MCVSys_OamBttnDelete( wk );

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
  for( i=0; i<WF_USERDISPTYPE_NUM; i++ ){
    GFL_HEAP_FreeMemory( wk->view.p_userbuff[i] );
  }

  // �_�~�[�X�N���[���j��
  GFL_HEAP_FreeMemory( wk->view.p_useretcbuff );
}

//----------------------------------------------------------------------------
/**
 *	@brief	SCRN�̃L�����N�^No��]����̃A�h���X������
 *
 *	@param	p_scrn	�X�N���[���f�[�^
 *	@param	cgofs	�L�����N�^�I�t�Z�b�g�i�L�����N�^�P�ʁj
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
 *	@brief	�{�^���ݒ�
 *
 *	@param	wk			�V�X�e�����[�N
 *	@param	friendNo	�F�B�ԍ�
 *	@param	type		�ݒ�^�C�v
 *
 *	type
 *		MCV_BTTN_FRIEND_TYPE_RES,	// �\��
 *		MCV_BTTN_FRIEND_TYPE_IN,	// �o�^�ς�
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
 *	@brief	�{�^������͂���
 *
 *	@param	wk			�V�X�e�����[�N
 *	@param	friendNo	�F�B�ԍ�
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
 *	@brief	�{�^���̕\���^�C�v���擾����
 *
 *	@param	wk			���[�N
 *	@param	friendNo	�F�Bnumber
 *
 *	@return	�\���^�C�v
 *	type
 *		MCV_BTTN_FRIEND_TYPE_RES,	// �\��
 *		MCV_BTTN_FRIEND_TYPE_IN,	// �o�^�ς�
 *		MCV_BTTN_FRIEND_TYPE_NONE,	// �Ȃ�
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_BttnTypeGet( const WIFIP2PMATCH_WORK *wk, u32 friendNo )
{
  GF_ASSERT( friendNo > 0 );
  return wk->view.bttnfriendNo[ friendNo-1 ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	������������
 *
 *	@param	wk			�V�X�e�����[�N
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
 *	@brief	�{�^�������ׂď������N�G�X�g���o��
 *
 *	@param	wk
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnAllWriteReq( WIFIP2PMATCH_WORK *wk )
{
  wk->view.bttn_allchg = TRUE;
  wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�w�i�`��
 *
 *	@param	wk	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_BackDraw( WIFIP2PMATCH_WORK *wk )
{
  // �w�i�̃J���[��ς���
  GFL_BG_ChangeScreenPalette(   GFL_BG_FRAME0_S, 0, 0,
                                32, 24, wk->view.frame_no+MCV_PAL_FRMNO );

  GFL_BG_LoadScreenV_Req(   GFL_BG_FRAME0_S );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���\��
 *
 *	@param	wk	�V�X�e�����[�N
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
  BOOL write_change_masterflag;	// �����I�ɂ��ׂẴ{�^��������������
  BOOL write_change_localflag;	// �����̃{�^�����Ƃ̏��������`�F�b�N

  // �S���������`�F�b�N
  if( wk->view.bttn_allchg == TRUE ){
    wk->view.bttn_allchg = FALSE;
    write_change_masterflag = TRUE;

    // �X�N���[���N���A
    //		GFL_BG_ScrFill( GFL_BG_FRAME2_S, 0, 0, 0, 32, 24, 0 );
    GFL_BG_ClearScreen(GFL_BG_FRAME2_S);
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
        NET_PRINT("friendno %d\n",friend_no);
        MCVSys_BttnWinDraw( wk, wk->view.nameWin[i], friend_no, frame, i );
        //@@OO				MCVSys_BttnStatusWinDraw( wk, wk->view.statusWin[i], friend_no, frame, i );
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
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[�f�[�^�\��
 *
 *	@param	wk	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispDraw( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  static void (*writeFunc[ WF_USERDISPTYPE_NUM ])( WIFIP2PMATCH_WORK *wk, u32 heapID ) =
  {
    MCVSys_UserDispDrawType00,
    MCVSys_UserDispDrawType06,
    MCVSys_UserDispDrawType01,
    MCVSys_UserDispDrawType02,
    MCVSys_UserDispDrawType03,
    MCVSys_UserDispDrawType04,
    MCVSys_UserDispDrawType05
    };
  int sex;
  int pal;

  sex = WifiList_GetFriendInfo( wk->pList,
                                wk->view.touch_friendNo - 1, WIFILIST_FRIEND_SEX );

  if(sex == PM_FEMALE){
    pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_GIRL;
  }else{
    pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_MAN;
  }
  GFL_BG_LoadScreen( GFL_BG_FRAME2_S, wk->view.p_userscrn[wk->view.user_dispno]->rawData,
                     wk->view.p_userscrn[wk->view.user_dispno]->szByte,0 );
  //	GFL_BG_ScreenBufSet( GFL_BG_FRAME2_S, wk->view.p_userscrn[wk->view.user_dispno]->rawData,
  //			wk->view.p_userscrn[wk->view.user_dispno]->szByte );

  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S, 0, 0,
                              32, 24, pal );

  // BG�R�ʂ̃X�N���[�����N���A
  GFL_BG_ClearFrame( GFL_BG_FRAME3_S);

  // �t�����e�B�A��\�����[�h�`�F�b�N
  if( _frontierInCheck( wk ) == FALSE ){
    MCVSys_UserDispDrawFrontierOffScrn( wk );	// ��\���X�N���[���ݒ�
  }


  // ���̐l�̂��Ƃ�`��
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.userWin), 0x0 );

  // �`��
  writeFunc[ wk->view.user_dispno ]( wk, heapID );

  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
  GFL_BMPWIN_TransVramCharacter(wk->view.userWin);

  // ���b�Z�[�W�ʂ̕\���ݒ�
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
}

// �ʏ�
static void MCVSys_UserDispDrawType00( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int sex;
  int col;
  int msg_id;
  int friendNo;
  int num;
  int vct_icon;
  WIFI_STATUS* p_status;
  u32 status;

  friendNo = wk->view.touch_friendNo - 1;

  sex = WifiList_GetFriendInfo( wk->pList, friendNo, WIFILIST_FRIEND_SEX );

  // �g���[�i�[��
  if( sex == PM_MALE ){
    _COL_N_BLUE;
  }else{
    _COL_N_RED;
  }
  MCVSys_FriendNameSet(wk, friendNo);
  GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_033, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_NAME_X, MCV_USERD_NAME_Y,
                  wk->TitleString, wk->fontHandle);

  p_status = WifiFriendMatchStatusGet( friendNo );
  status = _WifiMyStatusGet( wk, p_status );

  // ���
  msg_id = MCVSys_StatusMsgIdGet( status, &col );
  GFL_MSG_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_ST_X, MCV_USERD_ST_Y,
                  wk->pExpStrBuf, wk->fontHandle);

  // �O���[�v
  {
    //MYSTATUS* pTarget = MyStatus_AllocWork(HEAPID_WIFIP2PMATCH);
    //MyStatus_SetMyName(pTarget, WifiList_GetFriendGroupNamePtr(wk->pList,friendNo));
    //WORDSET_RegisterPlayerName( wk->view.p_wordset, 0, pTarget);
    int sex = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_SEX);
    WifiList_GetFriendName(wk->pList, friendNo, wk->pExpStrBuf);
    WORDSET_RegisterWord( wk->WordSet, 0, wk->pExpStrBuf, sex, TRUE, PM_LANG);
    //        GFL_HEAP_FreeMemory(pTarget);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_034, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_GR_X, MCV_USERD_GR_Y,
                    wk->TitleString, wk->fontHandle);
  }

  // �ΐ퐬��
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_035, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_X, MCV_USERD_VS_Y,
                    wk->TitleString, wk->fontHandle);
    // ����
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_WIN);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_036, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_WIN_X, MCV_USERD_VS_WIN_Y,
                    wk->TitleString, wk->fontHandle);
    // �܂�
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_LOSE);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_037, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_LOS_X, MCV_USERD_VS_WIN_Y,
                    wk->TitleString, wk->fontHandle);
  }
  // �|�P��������
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_038, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_TR_X,  MCV_USERD_TR_Y,
                    wk->TitleString, wk->fontHandle);

    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_TRADE_NUM);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_039, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_TRNUM_X, MCV_USERD_TR_Y,
                    wk->TitleString, wk->fontHandle);
  }


  // �Ō�ɗV�񂾓��t
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_040, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_DAY_X, MCV_USERD_DAY_Y,
                    wk->TitleString, wk->fontHandle);

    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_DAY);
    if(num!=0){
      WORDSET_RegisterNumber(wk->view.p_wordset, 2, num, 2, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
      num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_YEAR);
      WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
      num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_MONTH);
      WORDSET_RegisterNumber(wk->view.p_wordset, 1, num, 2, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
      GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_041, wk->pExpStrBuf );
      WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_DAYNUM_X, MCV_USERD_DAY_Y,
                      wk->TitleString, wk->fontHandle);
    }
  }

  // �A�C�R��
  WifiP2PMatchFriendListStIconWrite(  &wk->icon, GFL_BG_FRAME2_S,
                                      MCV_USERD_ICON_X, MCV_USERD_ICON_Y,
                                      status );
  if( WIFI_STATUS_GetVChatStatus(p_status) ){
    vct_icon = PLAYER_DISP_ICON_IDX_NONE;
  }else{
    vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
  }
  WifiP2PMatchFriendListIconWrite( &wk->icon, GFL_BG_FRAME2_S,
                                   MCV_USERD_VCTICON_X, MCV_USERD_ICON_Y,
                                   vct_icon, 0 );

}

// �o�g���^���[
static void MCVSys_UserDispDrawType01( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int friendNo;

  friendNo = wk->view.touch_friendNo - 1;

  // �^�C�g��
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf01, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTTW_TITLE_X,  MCV_USERD_BTTW_TITLE_Y,
                    wk->TitleString, wk->fontHandle);
  }
#if _BATTLE_TOWER_REC
  // �O��L�^
  {

    MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_TOWOR, friendNo );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTTW_LAST_X,  MCV_USERD_BTTW_LAST_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13,
                                    FRID_TOWER_MULTI_WIFI_RENSHOU_CNT, friendNo,
                                    MCV_USERD_BTTW_LASTNUM_X, MCV_USERD_BTTW_LASTNUM_Y );
  }

  // �ō��L�^
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTTW_MAX_X,  MCV_USERD_BTTW_MAX_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13,
                                    FRID_TOWER_MULTI_WIFI_RENSHOU, friendNo,
                                    MCV_USERD_BTTW_MAXNUM_X, MCV_USERD_BTTW_MAXNUM_Y );
  }
#endif
}

// �o�g���t�@�N�g���[
static void MCVSys_UserDispDrawType02( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int friendNo;

  friendNo = wk->view.touch_friendNo - 1;

  // �^�C�g��
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf02, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTTW_TITLE_X,  MCV_USERD_BTTW_TITLE_Y,
                    wk->TitleString, wk->fontHandle);
  }

#if _BATTLE_TOWER_REC
  // LV50
  {
    // �^�C�g��
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf08, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTFC_LV50_X,  MCV_USERD_BTFC_LV50_Y,
                    wk->TitleString, wk->fontHandle);

    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf10, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTFC_LV50K_X,  MCV_USERD_BTFC_LV50K_Y,
                    wk->TitleString, wk->fontHandle);

    // �O��
    MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_FACTORY, friendNo );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTFC_LV50LAST_X,  MCV_USERD_BTFC_LV50LAST_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13,
                                    FRID_FACTORY_MULTI_WIFI_RENSHOU_CNT, friendNo,
                                    MCV_USERD_BTFC_LV50LASTNUM_X, MCV_USERD_BTFC_LV50LASTNUM_Y );

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf14,
                                    FRID_FACTORY_MULTI_WIFI_TRADE_CNT, friendNo,
                                    MCV_USERD_BTFC_LV50LASTTRNUM_X, MCV_USERD_BTFC_LV50LASTTRNUM_Y );

    // �ō�
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTFC_LV50MAX_X,  MCV_USERD_BTFC_LV50MAX_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13,
                                    FRID_FACTORY_MULTI_WIFI_RENSHOU, friendNo,
                                    MCV_USERD_BTFC_LV50MAXNUM_X, MCV_USERD_BTFC_LV50MAXNUM_Y );

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf14,
                                    FRID_FACTORY_MULTI_WIFI_TRADE, friendNo,
                                    MCV_USERD_BTFC_LV50MAXTRNUM_X, MCV_USERD_BTFC_LV50MAXTRNUM_Y );
  }

  // OPEN
  {
    // �^�C�g��
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf09, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTFC_OPN_X,  MCV_USERD_BTFC_OPN_Y,
                    wk->TitleString, wk->fontHandle);

    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf10, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTFC_OPNK_X,  MCV_USERD_BTFC_OPNK_Y,
                    wk->TitleString, wk->fontHandle);

    // �O��
    MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_FACTORY100, friendNo );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTFC_OPNLAST_X,  MCV_USERD_BTFC_OPNLAST_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13,
                                    FRID_FACTORY_MULTI_WIFI_RENSHOU100_CNT, friendNo,
                                    MCV_USERD_BTFC_OPNLASTNUM_X, MCV_USERD_BTFC_OPNLASTNUM_Y );

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf14,
                                    FRID_FACTORY_MULTI_WIFI_TRADE100_CNT, friendNo,
                                    MCV_USERD_BTFC_OPNLASTTRNUM_X, MCV_USERD_BTFC_OPNLASTTRNUM_Y );

    // �ō�
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_BTFC_OPNMAX_X,  MCV_USERD_BTFC_OPNMAX_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13,
                                    FRID_FACTORY_MULTI_WIFI_RENSHOU100, friendNo,
                                    MCV_USERD_BTFC_OPNMAXNUM_X, MCV_USERD_BTFC_OPNMAXNUM_Y );

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf14,
                                    FRID_FACTORY_MULTI_WIFI_TRADE100, friendNo,
                                    MCV_USERD_BTFC_OPNMAXTRNUM_X, MCV_USERD_BTFC_OPNMAXTRNUM_Y );
  }
#endif
}

// �o�g���L���b�X��
static void MCVSys_UserDispDrawType03( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int friendNo;

  friendNo = wk->view.touch_friendNo - 1;

  // �^�C�g��
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf03, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTKS_TITLE_X,  MCV_USERD_BTKS_TITLE_X,
                    wk->TitleString, wk->fontHandle);

    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf11, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTKS_K_X,  MCV_USERD_BTKS_K_Y,
                    wk->TitleString, wk->fontHandle);
  }

#if _BATTLE_TOWER_REC
  // �O��L�^
  {
    MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_CASTLE, friendNo );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTKS_LAST_X,  MCV_USERD_BTKS_LAST_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf16,
                                    FRID_CASTLE_MULTI_WIFI_RENSHOU_CNT, friendNo,
                                    MCV_USERD_BTKS_LASTNUM_X, MCV_USERD_BTKS_LASTNUM_Y );

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf15,
                                    FRID_CASTLE_MULTI_WIFI_CP, friendNo,
                                    MCV_USERD_BTKS_LASTCPNUM_X, MCV_USERD_BTKS_LASTCPNUM_Y );
  }

  // �ō��L�^
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTKS_MAX_X,  MCV_USERD_BTKS_MAX_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf16,
                                    FRID_CASTLE_MULTI_WIFI_RENSHOU, friendNo,
                                    MCV_USERD_BTKS_MAXNUM_X, MCV_USERD_BTKS_MAXNUM_Y );

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf15,
                                    FRID_CASTLE_MULTI_WIFI_REMAINDER_CP, friendNo,
                                    MCV_USERD_BTKS_MAXCPNUM_X, MCV_USERD_BTKS_MAXCPNUM_Y );
  }
#endif //_BATTLE_TOWER_REC
}

// �o�g���X�e�[�W
static void MCVSys_UserDispDrawType04( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int friendNo;
  int num;
  STRBUF* p_str;

  friendNo = wk->view.touch_friendNo - 1;

  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf04, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTST_TITLE_X,  MCV_USERD_BTST_TITLE_Y,
                    wk->TitleString, wk->fontHandle);
  }

#if _BATTLE_TOWER_REC
  {
    FRONTIER_SAVEWORK* p_fsave;
    p_fsave = SaveData_GetFrontier( wk->pSaveData );

    num = FrontierRecord_Get( p_fsave, FRID_STAGE_MULTI_WIFI_MONSNO, friendNo );
    p_str = MSGDAT_UTIL_GetMonsName( num, heapID );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTST_K_X,  MCV_USERD_BTST_K_Y,
                    p_str, wk->fontHandle);
    GFL_STR_DeleteBuffer( p_str );
  }


  // �O��L�^
  {
    MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_STAGE, friendNo );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTST_LAST_X,  MCV_USERD_BTST_LAST_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13,
                                    FRID_STAGE_MULTI_WIFI_RENSHOU_CNT, friendNo,
                                    MCV_USERD_BTST_LASTNUM_X, MCV_USERD_BTST_LASTNUM_Y );
  }

  // �ō��L�^
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTST_MAX_X,  MCV_USERD_BTST_MAX_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13,
                                    FRID_STAGE_MULTI_WIFI_RENSHOU, friendNo,
                                    MCV_USERD_BTST_MAXNUM_X, MCV_USERD_BTST_MAXNUM_Y );
  }
#endif //_BATTLE_TOWER_REC
}

// �o�g�����[���b�g
static void MCVSys_UserDispDrawType05( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int friendNo;

  friendNo = wk->view.touch_friendNo - 1;

  // �^�C�g��
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf05, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTRT_TITLE_X,  MCV_USERD_BTRT_TITLE_Y,
                    wk->TitleString, wk->fontHandle);
  }

#if _BATTLE_TOWER_REC
  // �O��L�^
  {
    MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_ROULETTE, friendNo );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTRT_LAST_X,  MCV_USERD_BTRT_LAST_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf17,
                                    FRID_ROULETTE_MULTI_WIFI_RENSHOU_CNT, friendNo,
                                    MCV_USERD_BTRT_LASTNUM_X, MCV_USERD_BTRT_LASTNUM_Y );
  }

  // �ō��L�^
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_BTRT_MAX_X,  MCV_USERD_BTRT_MAX_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf17,
                                    FRID_ROULETTE_MULTI_WIFI_RENSHOU, friendNo,
                                    MCV_USERD_BTRT_MAXNUM_X, MCV_USERD_BTRT_MAXNUM_Y );
  }
#endif //_BATTLE_TOWER_REC

}

// �~�j�Q�[��
static void MCVSys_UserDispDrawType06( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int friendNo;
  WIFI_LIST* p_list;

  p_list	= SaveData_GetWifiListData( wk->pSaveData );

  friendNo = wk->view.touch_friendNo - 1;

  // �^�C�g��
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_mg01, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_MINI_TITLE_X,  MCV_USERD_MINI_TITLE_Y,
                    wk->TitleString, wk->fontHandle);
  }

  // ���܂���
  {
    WORDSET_RegisterWiFiLobbyGameName( wk->view.p_wordset, 0, WFLBY_GAME_BALLSLOW );
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_mg02, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_MINI_BC_X,  MCV_USERD_MINI_BC_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispNumDraw( wk, msg_wifilobby_mg05,
                            WifiList_GetFriendInfo( p_list, friendNo, WIFILIST_FRIEND_BALLSLOW_NUM ),
                            MCV_USERD_MINI_BCNUM_X, MCV_USERD_MINI_BCNUM_Y );
  }

  // ���܂̂�
  {
    WORDSET_RegisterWiFiLobbyGameName( wk->view.p_wordset, 0, WFLBY_GAME_BALANCEBALL );
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_mg02, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_MINI_BB_X,  MCV_USERD_MINI_BB_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispNumDraw( wk, msg_wifilobby_mg05,
                            WifiList_GetFriendInfo( p_list, friendNo, WIFILIST_FRIEND_BALANCEBALL_NUM ),
                            MCV_USERD_MINI_BBNUM_X, MCV_USERD_MINI_BBNUM_Y );
  }

  // �ӂ�����
  {
    WORDSET_RegisterWiFiLobbyGameName( wk->view.p_wordset, 0, WFLBY_GAME_BALLOON );
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_mg02, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                    MCV_USERD_MINI_BL_X,  MCV_USERD_MINI_BL_Y,
                    wk->TitleString, wk->fontHandle);

    MCVSys_UserDispNumDraw( wk, msg_wifilobby_mg05,
                            WifiList_GetFriendInfo( p_list, friendNo, WIFILIST_FRIEND_BALLOON_NUM ),
                            MCV_USERD_MINI_BLNUM_X, MCV_USERD_MINI_BLNUM_Y );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�����e�B�A��\�����[�h�̃X�N���[���ɕύX
 *
 *	@param	wk		���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispDrawFrontierOffScrn( WIFIP2PMATCH_WORK *wk )
{
  int i;
  int roop;

  // �t�����e�B�A�{�ݐ��擾
  roop = WF_USERDISPTYPE_NUM - WF_USERDISPTYPE_BLTW;

  for( i=0; i<roop; i++ ){
    GFL_BG_WriteScreenExpand( GFL_BG_FRAME2_S,
                              MCV_USERD_NOFR_SCRN_X+(MCV_USERD_NOFR_SCRN_SIZX*i), MCV_USERD_NOFR_SCRN_Y,
                              MCV_USERD_NOFR_SCRN_SIZX, MCV_USERD_NOFR_SCRN_SIZY,
                              wk->view.p_useretcscrn->rawData,
                              0, 0,
                              wk->view.p_useretcscrn->screenWidth/8, wk->view.p_useretcscrn->screenHeight/8 );
  }

  // �p���b�g�ύX
  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S,
                              MCV_USERD_NOFR_SCRN_X, MCV_USERD_NOFR_SCRN_Y,
                              (MCV_USERD_NOFR_SCRN_SIZX*roop), MCV_USERD_NOFR_SCRN_SIZY,
                              MCV_PAL_BTTN+MCV_PAL_BTTN_NONE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�@�N�g���[�f�[�^�\��
 *
 *	@param	wk			���[�N
 *	@param	strid		����ID
 *	@param	factoryid	�t�@�N�g���[�f�[�^ID
 *	@param	friendno	�F�B�ԍ�
 *	@param	x			�\��X
 *	@param	y			�\��Y
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispFrontiorNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 factoryid, u32 friendno, u32 x, u32 y )
{
#if _BATTLE_TOWER_REC
  int num;
  FRONTIER_SAVEWORK* p_fsave;

  p_fsave = SaveData_GetFrontier( wk->pSaveData );

  num = FrontierRecord_Get(p_fsave, factoryid, friendno);

  MCVSys_UserDispNumDraw( wk, strid, num, x, y );
#endif //_BATTLE_TOWER_REC
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�����e�B�A�@�^�C�g�����b�Z�[�W�擾
 *
 *	@param	wk			���[�N
 *	@param	p_str		������i�[��
 *	@param	factoryid	�t�@�N�g���[�@�Z�[�u�f�[�^ID
 *	@param	friendno	�F�B�i���o�[
 *
 *	factoryid
 *		MCV_FRONTIOR_TOWOR,
 *		MCV_FRONTIOR_FACTORY,
 *		MCV_FRONTIOR_FACTORY100,
 *		MCV_FRONTIOR_CASTLE,
 *		MCV_FRONTIOR_STAGE,
 *		MCV_FRONTIOR_ROULETTE,
 *
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispFrontiorTitleStrGet( WIFIP2PMATCH_WORK *wk, STRBUF* p_str, u32 factoryid, u32 friendno )
{
#if _BATTLE_TOWER_REC
  FRONTIER_SAVEWORK* p_fsave;
  BOOL clear_flag;
  u32 stridx;
  static const u32 sc_MCVSYS_FRONTIOR_CLEAR_BIT[ MCV_FRONTIOR_NUM ] = {
    FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
    FRID_FACTORY_MULTI_WIFI_CLEAR_BIT,
    FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT,
    FRID_CASTLE_MULTI_WIFI_CLEAR_BIT,
    FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
    FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT,
  };

  GF_ASSERT( factoryid < MCV_FRONTIOR_NUM );

  p_fsave		= SaveData_GetFrontier( wk->pSaveData );

  clear_flag = FrontierRecord_Get(p_fsave, sc_MCVSYS_FRONTIOR_CLEAR_BIT[factoryid], friendno);
  if( clear_flag == FALSE ){
    stridx = msg_wifilobby_bf06;
  }else{
    stridx = msg_wifilobby_bf18;
  }
  GFL_MSG_GetString(  wk->MsgManager, stridx, p_str );
#endif //_BATTLE_TOWER_REC
}

//----------------------------------------------------------------------------
/**
 *	@brief	������\�����鏈��
 *
 *	@param	wk		���[�N
 *	@param	strid	���b�Z�[�WID
 *	@param	num		����
 *	@param	x		���h�b�g���W
 *	@param	y		���h�b�g���W
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
 *	@brief	�{�^���`��
 *
 *	@param	wk		�V�X�e�����[�N
 *	@param	cx		���L�������W
 *	@param	cy		���L�������W
 *	@param	type	�{�^���^�C�v
 *	@param	frame	�t���[��
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnWrite( WIFIP2PMATCH_WORK *wk, u8 cx, u8 cy, u8 type, u8 frame )
{
  u16 reed_x, reed_y;
  u16* p_buff;

  GF_ASSERT( frame < 4 );

  // frame 3�̂Ƃ���1��\������
  if( frame == 3 ){
    frame = 1;
  }

  if( (frame < 2) ){
    reed_x = MCV_BUTTON_SIZX * frame;
    reed_y = MCV_BUTTON_SIZY * type;

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
  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S, cx, cy,
                              MCV_BUTTON_SIZX, MCV_BUTTON_SIZY, type+MCV_PAL_BTTN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��ԂɑΉ��������b�Z�[�W�f�[�^�\��
 *
 *	@param	status	���
 *
 *	@return	���b�Z�[�W�f�[�^
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_StatusMsgIdGet( u32 status, int* col )
{
  u32 msg_id;
  //	*col = _COL_N_BLACK;
  _COL_N_BLACK;
  switch(status){
  case WIFI_STATUS_VCT:      // VCT��
    msg_id = msg_wifilobby_027;
    //		*col = _COL_N_GRAY;
    _COL_N_GRAY;
    break;
  case WIFI_STATUS_SBATTLE50:      // �ΐ풆
  case WIFI_STATUS_SBATTLE100:      // �ΐ풆
  case WIFI_STATUS_SBATTLE_FREE:      // �ΐ풆
  case WIFI_STATUS_DBATTLE50:      // �ΐ풆
  case WIFI_STATUS_DBATTLE100:      // �ΐ풆
  case WIFI_STATUS_DBATTLE_FREE:      // �ΐ풆
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE:      // �ΐ풆
#endif
    msg_id = msg_wifilobby_024;
    //		*col = _COL_N_GRAY;
    _COL_N_GRAY;
    break;
  case WIFI_STATUS_SBATTLE50_WAIT:   // �ΐ��W��
    msg_id = msg_wifilobby_060;
    break;
  case WIFI_STATUS_SBATTLE100_WAIT:   // �ΐ��W��
    msg_id = msg_wifilobby_061;
    break;
  case WIFI_STATUS_SBATTLE_FREE_WAIT:   // �ΐ��W��
    msg_id = msg_wifilobby_059;
    break;
  case WIFI_STATUS_DBATTLE50_WAIT:   // �ΐ��W��
    msg_id = msg_wifilobby_063;
    break;
  case WIFI_STATUS_DBATTLE100_WAIT:   // �ΐ��W��
    msg_id = msg_wifilobby_064;
    break;
  case WIFI_STATUS_DBATTLE_FREE_WAIT:   // �ΐ��W��
    msg_id = msg_wifilobby_062;
    break;
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_MBATTLE_FREE_WAIT:   // �ΐ��W��
    msg_id = msg_wifilobby_062;
    break;
#endif
  case WIFI_STATUS_TRADE:          // ������
    msg_id = msg_wifilobby_026;
    //		*col = _COL_N_GRAY;
    _COL_N_GRAY;
    break;
  case WIFI_STATUS_TRADE_WAIT:    // ������W��
    msg_id = msg_wifilobby_025;
    break;
  case WIFI_STATUS_FRONTIER:          // �t�����e�B�A��
    msg_id = msg_wifilobby_139;
    //		*col = _COL_N_GRAY;
    _COL_N_GRAY;
    break;
  case WIFI_STATUS_FRONTIER_WAIT:    // �t�����e�B�A��W��
    msg_id = msg_wifilobby_138;
    break;
#ifdef WFP2P_DEBUG_EXON
  case WIFI_STATUS_BATTLEROOM:          // �o�g�����[����
    msg_id = msg_wifilobby_debug_00;
    //		*col = _COL_N_GRAY;
    _COL_N_GRAY;
    break;
  case WIFI_STATUS_BATTLEROOM_WAIT:    // �o�g�����[����W��
    msg_id = msg_wifilobby_debug_00;
    break;
#endif
  case WIFI_STATUS_LOGIN_WAIT:    // �ҋ@���@���O�C������͂���
    msg_id = msg_wifilobby_046;
    break;
  default:
    msg_id = msg_wifilobby_056;
    break;
  }
  return msg_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^����̃g���[�i�[���̕\��
 *
 *	@param	wk			�V�X�e�����[�N
 *	@param	p_bmp		�r�b�g�}�b�v
 *	@param	friendNo	�F�B�i���o�[
 *	@param	frame		�{�^���t���[��
 *	@param	area_id		�z�uID
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
  y = ViewButtonFrame_y[ frame ];

  sex = WifiList_GetFriendInfo( wk->pList, friendNo-1, WIFILIST_FRIEND_SEX );
  // �g���[�i�[��
  if( sex == PM_MALE ){
    _COL_N_BLUE;
  }else{
    _COL_N_RED;
  }
  MCVSys_FriendNameSet(wk, friendNo-1);
  GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_033, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_bmp),
                  0,  y,
                  wk->TitleString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter( p_bmp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��ԃE�B���h�E�̕`��
 *
 *	@param	wk			�����Ăރ��[�N
 *	@param	p_stbmp		��ԃr�b�g�}�b�v
 *	@param	friendNo	�F�B�ԍ�
 *	@param	frame		�{�^���t���[��
 *	@param	area_id		�z�uID
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnStatusWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN** p_stbmp, u32 friendNo, u32 frame, u32 area_id )
{
  int y;
  int i;
  int vct_icon;
  WIFI_STATUS* p_status;
  u32 status;

  p_status = WifiFriendMatchStatusGet( friendNo - 1 );
  status = _WifiMyStatusGet( wk, p_status );

  // �A�C�R���\��
  y = ViewButtonFrame_y[ frame ];

  for( i=0; i<WF_VIEW_STATUS_NUM; i++ ){
    // ��ʃN���[��
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_stbmp[i]), WINCLR_COL(FBMP_COL_WHITE) );
    if( i==0 ){
      int	pal = WifiP2PMatchBglFrmIconPalGet( GFL_BMPWIN_GetFrame(p_stbmp[i]) );
      WifiP2PMatchFriendListBmpStIconWrite( p_stbmp[i], &wk->icon,
                                            0, y,
                                            status,pal );
    }else{
      // �p���b�gNo�擾
      int pal = WifiP2PMatchBglFrmIconPalGet( frame );

      if( WIFI_STATUS_GetVChatStatus(p_status) ){
        vct_icon = PLAYER_DISP_ICON_IDX_NONE;
      }else{
        vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
      }
      WifiP2PMatchFriendListBmpIconWrite( p_stbmp[i], &wk->icon,
                                          0, y,
                                          vct_icon, 0, pal );
    }
    GFL_BMPWIN_TransVramCharacter( p_stbmp[i] );
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
    0, 1, 2, 1
    };

  // ���삵�Ă��邩�`�F�b�N
  if( wk->view.touch_friendNo == 0 ){
    return ;
  }

  // ����
  wk->view.button_count ++;
  if( wk->view.button_count >= BttnAnmFrame[ wk->view.touch_frame ] ){
    wk->view.button_count = 0;
    wk->view.touch_frame ++;

    wk->view.button_on = TRUE;

    // �I���`�F�b�N
    if( wk->view.touch_frame >= MCV_BUTTON_FRAME_NUM ){
      wk->view.button_count = 0;
      wk->view.touch_frame = 0;
      wk->view.user_disp = MCV_USERDISP_INIT;
    }
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�u��ʁ@OAM�{�^���@������
 *
 *	@param	wk			���[�N
 *	@param	p_handle	�n���h��
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnInit( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
#if _OAMBUTTON //@@OO
  BOOL result;
  int i;
  /* 	CLACT_HEADER head; */
  //	GFL_BMPWIN* bttn_font;
  GFL_BMP_DATA *bmp;
  STRBUF* p_str;
  //	FONTOAM_OAM_DATA_PTR p_fontoam_data;
  int fontoam_cgsize;
  //	FONTOAM_INIT fontoam_add;


  GFL_CLWK_DATA add[ MCV_USERD_BTTN_NUM ] = {
    {	// LEFT
      MCV_USERD_BTTN_LEFT_X, MCV_USERD_BTTN_Y+NAMEIN_SUB_ACTOR_DISTANCE, 0,
      MCV_USERD_BTTN_PRI, 0
      },
    {	// BACK
      MCV_USERD_BTTN_BACK_X, MCV_USERD_BTTN_Y+NAMEIN_SUB_ACTOR_DISTANCE, 0 ,
      MCV_USERD_BTTN_PRI, 0
      },
    {	// RIGHT
      MCV_USERD_BTTN_RIGHT_X, MCV_USERD_BTTN_Y+NAMEIN_SUB_ACTOR_DISTANCE, 0,
      MCV_USERD_BTTN_PRI, 0
      },
  };


  // �ꉞ�\����OFF
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
  //	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  // ���\�[�X�Ǎ���
  //	wk->view.button_res[0] = CLACT_U_ResManagerResAddArcChar_ArcHandle(
  //			wk->resMan[0], p_handle, NARC_wifip2pmatch_wf_match_btm_oam_NCGR,
  //			FALSE, MCV_USERD_BTTN_RESCONTID,
  //			NNS_G2D_VRAM_TYPE_2DSUB, heapID );
  wk->view.button_res[0] = GFL_CLGRP_CGR_Register(p_handle,
                                                  NARC_wifip2pmatch_wf_match_btm_oam_NCGR,
                                                  FALSE , CLSYS_DRAW_SUB, heapID);


  //	wk->view.button_res[1] = CLACT_U_ResManagerResAddArcPltt_ArcHandle(
  //			wk->resMan[1], p_handle, NARC_wifip2pmatch_wf_match_btm_oam_NCLR,
  //			FALSE, MCV_USERD_BTTN_RESCONTID,
  //			NNS_G2D_VRAM_TYPE_2DSUB, 8, heapID );

  wk->view.button_res[1] = GFL_CLGRP_PLTT_Register(p_handle,
                                                   NARC_wifip2pmatch_wf_match_btm_oam_NCLR,
                                                   CLSYS_DRAW_SUB, 8, heapID);


  /*     wk->view.button_res[2] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
   * 			wk->resMan[2], p_handle, NARC_wifip2pmatch_wf_match_btm_oam_NCER,
   * 			FALSE, MCV_USERD_BTTN_RESCONTID, CLACT_U_CELL_RES,
   * 			heapID );
   * 	wk->view.button_res[3] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
   * 			wk->resMan[3], p_handle, NARC_wifip2pmatch_wf_match_btm_oam_NANR,
   * 			FALSE, MCV_USERD_BTTN_RESCONTID, CLACT_U_CELLANM_RES,
   * 			heapID );
   */
  wk->view.button_res[2] = GFL_CLGRP_CELLANIM_Register(p_handle,
                                                       NARC_wifip2pmatch_wf_match_btm_oam_NCER,
                                                       NARC_wifip2pmatch_wf_match_btm_oam_NANR,
                                                       heapID);


  // VRAM�W�J
  /* 	result = CLACT_U_CharManagerSetCharModeAdjustAreaCont( wk->view.button_res[0] );
   * 	GF_ASSERT( result );
   * 	result = CLACT_U_PlttManagerSetCleanArea( wk->view.button_res[1] );
   * 	GF_ASSERT( result );
   * 	CLACT_U_ResManagerResOnlyDelete( wk->view.button_res[0] );
   * 	CLACT_U_ResManagerResOnlyDelete( wk->view.button_res[1] );
   */

  // �w�b�_�[�쐬
  /*
    CLACT_U_MakeHeader( &head,
			MCV_USERD_BTTN_RESCONTID, MCV_USERD_BTTN_RESCONTID,
			MCV_USERD_BTTN_RESCONTID, MCV_USERD_BTTN_RESCONTID,
			CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
			0, MCV_USERD_BTTN_BGPRI,
			wk->resMan[0], wk->resMan[1], wk->resMan[2], wk->resMan[3],
			NULL, NULL );
   */

  // �A�N�^�[�쐬
  for( i=0; i<MCV_USERD_BTTN_NUM; i++ ){
    /* 		add[i].ClActSet = wk->clactSet;
     * 		add[i].ClActHeader = &head;
     * 		add[i].heap = heapID;
     */
    /* 		wk->view.button_act[i] = CLACT_Add( &add[i] );
     * 		CLACT_AnmChg( wk->view.button_act[i], c_MCV_USER_BTTN_ANM_RELEASE[i] );
     */
    wk->view.button_act[i] = GFL_CLACT_WK_Create(wk->clactSet,
                                                 wk->view.button_res[0],
                                                 wk->view.button_res[1],
                                                 wk->view.button_res[2],
                                                 &add[i], CLSYS_DEFREND_SUB, heapID );


  }

  // �{�^���t�H���g�Ǎ���
  //FontProc_LoadFont( FONT_BUTTON, heapID );

  // ���ǂ�@�`��p�̈�쐬
  /* 	GFL_BG_BmpWinInit( &bttn_font );
   *     GFL_BG_BmpWinObjAdd(  &bttn_font,
   * 			MCV_USERD_BTTN_FONT_SIZX, MCV_USERD_BTTN_FONT_SIZY,
   * 			MCV_USERD_BTTN_FONT_CGXOFS, MCV_USERD_BTTN_FONT_COL );
   */
  // ������擾
  p_str = GFL_STR_CreateBuffer( 64, heapID );
  GFL_MSG_GetString( wk->MsgManager, msg_wifilobby_143, p_str );

  {
    int char_len , font_len;
    //������̃h�b�g������A�g�p����L���������Z�o����
    font_len = PRINTSYS_GetStrWidth(p_str, wk->fontHandle, 0);
    char_len = font_len / 8;
    char_len++;

    //BMP�쐬
    bmp = GFL_BMP_Create(char_len, MCV_USERD_BTTN_FONT_SIZY, GFL_BMP_16_COLOR, heapID);
    PRINTSYS_Print(bmp, 0, 0, p_str, wk->fontHandle);
  }

  /* 	bmp = GFL_BMP_Create(char_len, y_char_len, GFL_BMP_16_COLOR, HEAPID_BALLOON);
   *     bttn_font = GFL_BMPWIN_Create(
   *         GFL_BG_FRAME1_S,
   *         0,
   *         0,
   *         MCV_USERD_BTTN_FONT_SIZX, MCV_USERD_BTTN_FONT_SIZY,
   *         MCV_USERD_BTTN_FONT_COL, GFL_BMP_CHRAREA_GET_B );
   */


  // ���������������
  //	PRINT_UTIL_Print( &bttn_font, FONT_BUTTON, p_str, 0, 0, MSG_NO_PUT, GF_PRINTCOLOR_MAKE(1,2,0), NULL );
  /*     PRINTSYS_Print( GFL_BMPWIN_GetBmp(bttn_font),
   *                     0,  0,
   *                     p_str, wk->fontHandle);
   */



  GFL_STR_DeleteBuffer( p_str );

  // ������OAM�f�[�^�쐬
  //	p_fontoam_data = FONTOAM_OAMDATA_Make( &bttn_font, heapID );

  //�t�H���gOAM�V�X�e���쐬
  wk->view.back_fontoam = BmpOam_Init(heapID, wk->clactSet);


  // �T�C�Y�擾
  //	fontoam_cgsize = FONTOAM_OAMDATA_NeedCharSize( p_fontoam_data, NNS_G2D_VRAM_TYPE_2DSUB );

  // CharAreaAlloc
  //	result = CharVramAreaAlloc( fontoam_cgsize, CHARM_CONT_AREACONT,
  //			NNS_G2D_VRAM_TYPE_2DSUB, &wk->view.back_fontoam_cg );
  //	GF_ASSERT(result);

  /* 	// fontoam�쐬
   * 	fontoam_add.fontoam_sys = wk->fontoam;
   * 	fontoam_add.bmp = &bttn_font;
   * 	fontoam_add.clact_set = wk->clactSet;
   * 	fontoam_add.pltt = CLACT_U_PlttManagerGetProxy( wk->view.button_res[1], NULL );
   * 	fontoam_add.parent = wk->view.button_act[MCV_USERD_BTTN_BACK];
   * 	fontoam_add.char_ofs = wk->view.back_fontoam_cg.alloc_ofs;
   * 	fontoam_add.x = MCV_USERD_BTTN_FONT_X;
   * 	fontoam_add.y = MCV_USERD_BTTN_FONT_Y;
   * 	fontoam_add.bg_pri = MCV_USERD_BTTN_BGPRI;
   * 	fontoam_add.soft_pri = MCV_USERD_BTTN_FONT_OAMPRI;
   * 	fontoam_add.draw_area = NNS_G2D_VRAM_TYPE_2DSUB;
   * 	fontoam_add.heap = heapID;
   * 	wk->view.back_fontoam = FONTOAM_OAMDATA_Init( &fontoam_add, p_fontoam_data );
   */
  {
    BMPOAM_ACT_DATA head;

    //�t�H���g�A�N�^�[�쐬
    head.bmp = bmp;
    head.x = MCV_USERD_BTTN_FONT_X;
    head.y = MCV_USERD_BTTN_FONT_Y;
    head.pltt_index = wk->view.button_res[1];
    head.pal_offset = 0;
    head.soft_pri = MCV_USERD_BTTN_FONT_OAMPRI;
    head.bg_pri = MCV_USERD_BTTN_BGPRI;
    head.setSerface = CLWK_SETSF_NONE;
    head.draw_type = CLSYS_DRAW_SUB;
    wk->view.BmpOamAct = BmpOam_ActorAdd(wk->view.back_fontoam, &head);
    wk->view.BmpOamBmp = bmp;

    // OAM�\���f�[�^�j��
    //        FONTOAM_OAMDATA_Free( p_fontoam_data );
  }
  // BMP�j��
  //	GFL_BMPWIN_Delete( bttn_font );

  // ������t�H���g�j��
  //	FontProc_UnloadFont( FONT_BUTTON );

  // �{�^�������蔻��f�[�^�쐬
  wk->view.p_oambttnman = GFL_BMN_Create( c_MCV_USER_BTTN_OAM_Hit,
                                          MCVSys_OamBttnCallBack, wk, heapID );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���f�[�^�̔j��
 *
 *	@param	wk	���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnDelete( WIFIP2PMATCH_WORK *wk )
{
#if _OAMBUTTON //@@OO
  int i;

  // �{�^���}�l�[�W���j��
  GFL_BMN_Delete( wk->view.p_oambttnman );

  // �t�H���gOAM�j��
  FONTOAM_OAMDATA_Delete( wk->view.back_fontoam );

  // �L�����̈�j��
  //	CharVramAreaFree( &wk->view.back_fontoam_cg );

  // ���[�N�̔j��
  for( i=0; i<MCV_USERD_BTTN_NUM; i++ ){
    GFL_CLACT_WK_Remove( wk->view.button_act[i] );
  }

  // ���\�[�X�j��
  GFL_CLGRP_CGR_Release( wk->view.button_res[0] );
  GFL_CLGRP_PLTT_Release( wk->view.button_res[1] );
  GFL_CLGRP_CELLANIM_Release( wk->view.button_res[2] );

  BmpOam_ActorDel(wk->view.BmpOamAct);
  GFL_BMP_Delete( wk->view.BmpOamBmp );
  BmpOam_Exit(wk->view.back_fontoam);

#endif //_OAMBUTTON //@@OO
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM�{�^��ON
 *
 *	@param	wk	���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnOn( WIFIP2PMATCH_WORK *wk )
{
#if _OAMBUTTON //@@OO
  int i;

  // �\����Ԃɂ���
  wk->view.buttonact_on = MCV_USERD_BTTN_MODE_NML;

  // �\��ON
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  // �p�����[�^������
  wk->view.touch_button = 0;

  // �A�j��������
  for( i=0; i<MCV_USERD_BTTN_NUM; i++ ){
    MCVSys_OamBttnObjAnmStart( &wk->view, i );
  }
#endif //_OAMBUTTON //@@OO
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���\���@BACK��\�����[�h
 *
 *	@param	wk	���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnOnNoBack( WIFIP2PMATCH_WORK *wk )
{
#if _OAMBUTTON //@@OO
  MCVSys_OamBttnOn( wk );

  // ���ǂ邾����\��
  GFL_CLACT_WK_SetDrawEnable( wk->view.button_act[MCV_USERD_BTTN_BACK], FALSE );

  //@@OO
  //	FONTOAM_SetDrawFlag( wk->view.back_fontoam, FALSE );
  BmpOam_ActorSetDrawEnable(wk->view.BmpOamAct, FALSE);

  // �\����Ԃɂ���
  wk->view.buttonact_on = MCV_USERD_BTTN_MODE_NOBACK;
#endif //_OAMBUTTON //@@OO
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^��OFF
 *
 *	@param	wk
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnOff( WIFIP2PMATCH_WORK *wk )
{
#if _OAMBUTTON //@@OO
  // �߂邪�\��OFF�ɂȂ��Ă邩������Ȃ��̂�ON�ɂ��Ă���
  if( wk->view.buttonact_on == MCV_USERD_BTTN_MODE_NOBACK ){
    GFL_CLACT_WK_SetDrawEnable( wk->view.button_act[MCV_USERD_BTTN_BACK], TRUE );
    //@@OO
    //	FONTOAM_SetDrawFlag( wk->view.back_fontoam, TRUE );
    BmpOam_ActorSetDrawEnable(wk->view.BmpOamAct, TRUE);
  }

  // ��\����Ԃɂ���
  wk->view.buttonact_on = MCV_USERD_BTTN_MODE_WAIT;

  // �\����OFF
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
#endif //_OAMBUTTON //@@OO

}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^����\������
 *
 *	@param	wk
 *
 *	@retval	MCV_USERD_BTTN_RET_NONE,	// ���̔������Ȃ�
 *	@retval	MCV_USERD_BTTN_RET_LEFT,	// ���������ꂽ
 *	@retval	MCV_USERD_BTTN_RET_BACK,	// ���ǂ邪�����ꂽ
 *	@retval	MCV_USERD_BTTN_RET_RIGHT,	// �E�������ꂽ
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_OamBttnMain( WIFIP2PMATCH_WORK *wk )
{
#if _OAMBUTTON //@@OO
  int i;
  BOOL result;

  // �\��ON���傤�������`�F�b�N
  if( wk->view.buttonact_on == MCV_USERD_BTTN_MODE_WAIT ){
    return MCV_USERD_BTTN_RET_NONE;
  }


  // �{�^���C�x���g�f�[�^����
  wk->view.touch_button = MCV_USERD_BTTN_NUM;	// ���ɂ��������Ȃ��悤�ɂ��肦�Ȃ��{�^���ԍ�
  wk->view.touch_button_event = 0xff;	// ���ɂ��������Ȃ��悤�ɂ��肦�Ȃ�����������Ă���

  // �{�^���}�l�[�W�����s
  GFL_BMN_Main( wk->view.p_oambttnman );

  // �{�^���A�j���R���g���[��
  for( i=0; i<MCV_USERD_BTTN_NUM; i++ ){

    // �o�b�N��\�����[�h�Ȃ�o�b�N�̏������s��Ȃ�
    if( (wk->view.buttonact_on == MCV_USERD_BTTN_MODE_NOBACK) &&
        (i == MCV_USERD_BTTN_BACK) ){
      continue;
    }

    result = MCVSys_OamBttnObjAnmMain( &wk->view, i, wk->view.touch_button, wk->view.touch_button_event );
    if( result == TRUE ){
      // �{�^����������
      return MCV_USERD_BTTN_RET_LEFT + i;
    }
  }
#endif //_OAMBUTTON //@@OO

  return MCV_USERD_BTTN_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���R�[���o�b�N
 *
 *	@param	bttn_no		�{�^��No
 *	@param	event		�C�x���gNo
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnCallBack( u32 bttn_no, u32 event, void* p_work )
{
  WIFIP2PMATCH_WORK* p_wk = p_work;
  p_wk->view.touch_button = bttn_no;
  p_wk->view.touch_button_event = event;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���A�j���J�n
 *
 *	@param	p_view		���[�N
 *	@param	bttn_no		�{�^���i���o�[
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnObjAnmStart( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no )
{
#if _OAMBUTTON //@@OO
  // �A�j��������	RELEASE�A�j���ŏI�t���[���ɐݒ�
  GFL_CLACT_WK_SetAnmSeq( p_view->button_act[ bttn_no ], c_MCV_USER_BTTN_ANM_RELEASE[bttn_no] );
  GFL_CLACT_WK_SetAnmIndex( p_view->button_act[ bttn_no ], MCV_USERD_BTTN_ANMMAX_0ORG );

  // ������̍��W������ @@OO
  //	if( bttn_no == MCV_USERD_BTTN_BACK ){
  //		FONTOAM_SetMat( p_view->back_fontoam, MCV_USERD_BTTN_FONT_X, c_MCV_USER_BTTN_FONT_YANM[0] );
  //	}
#endif //_OAMBUTTON //@@OO

}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j�����C��
 *
 *	@param	p_view		���[�N
 *	@param	bttn_no		�{�^���i���o�[
 *	@param	push_bttn	������Ă���{�^��
 *	@param	event		�C�x���g
 *
 *	@retval	TRUE	�����I�����
 *	@retval	FALSE	������Ă��Ȃ����͉����I����ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL MCVSys_OamBttnObjAnmMain( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no, u32 push_bttn, u32 event )
{
  BOOL ret = FALSE;
#if _OAMBUTTON //@@OO
  u32 anm_frame;
  u32 anm_seq;

  // �������ԉ����Ă���Ƃ���MCV_USERD_BTTN_ANMPUSHOK�܂ł�������TRUE��Ԃ�
  // �Z�����ԉ����Ă���Ƃ��������ꂽ�Ƃ���TRUE��Ԃ�


  // ������������Ă��邩�`�F�b�N
  if( (push_bttn == bttn_no)  ){

    anm_frame = GFL_CLACT_WK_GetAnmIndex( p_view->button_act[bttn_no] );

    // ������Ă���Ƃ�
    if( (event == GFL_BMN_EVENT_TOUCH)  || (event == GFL_BMN_EVENT_HOLD) ){

      // �C�x���g���ŏ��̃^�b�`��������A�j���ݒ�
      if( event == GFL_BMN_EVENT_TOUCH ){
        GFL_CLACT_WK_SetAnmSeq( p_view->button_act[ bttn_no ], c_MCV_USER_BTTN_ANM_PUSH[bttn_no] );
      }

      // �A�j����PUSHOK�ȉ��̎��̂ݓ�����
      if( anm_frame < MCV_USERD_BTTN_ANMPUSHOK ){

        GFL_CLACT_WK_AddAnmFrame( p_view->button_act[bttn_no], FX32_CONST(2) );
        anm_frame = GFL_CLACT_WK_GetAnmIndex( p_view->button_act[bttn_no] );

        // �u���ǂ�v�Ȃ�t�H���g��������
        if( bttn_no == MCV_USERD_BTTN_BACK ){
          //@@OO
          //					FONTOAM_SetMat( p_view->back_fontoam, MCV_USERD_BTTN_FONT_X,
          //							c_MCV_USER_BTTN_FONT_YANM[anm_frame] );
        }

        // �{�^�������܂ŃA�j�������牟�������Ƃɂ���
        // ���̎��̃t���[������͏��anm_frame�`�F�b�N��
        // �Ђ�������̂ł����͏��߂�OK�ɂ����Ƃ��ɂ̂�
        // ���s�����
        if( anm_frame >= MCV_USERD_BTTN_ANMPUSHOK ){
          ret = TRUE;
        }
      }

    }
    // �������Ƃ�
    else if( event == GFL_BMN_EVENT_RELEASE ){
      if( anm_frame < MCV_USERD_BTTN_ANMPUSHOK ){
        // �{�^�������������ƂɂȂ�t���[���܂ł����Ă��Ȃ����
        // �{�^�������������Ƃɂ���
        ret = TRUE;
      }
    }
  }else{

    anm_seq = GFL_CLACT_WK_GetAnmSeq( p_view->button_act[bttn_no] );//GFL_CLACT_WK_GetAnmSeq
    anm_frame = GFL_CLACT_WK_GetAnmIndex( p_view->button_act[bttn_no] );

    // �A�j����PUSH�A�j����������RELEASE�A�j���ɐ؂�ւ���
    if( anm_seq == c_MCV_USER_BTTN_ANM_PUSH[bttn_no] ){
      GFL_CLACT_WK_SetAnmSeq( p_view->button_act[bttn_no], c_MCV_USER_BTTN_ANM_RELEASE[bttn_no] );
      GFL_CLACT_WK_SetAnmIndex( p_view->button_act[bttn_no], MCV_USERD_BTTN_ANMMAX_0ORG - anm_frame );
    }

    // �A�j�����I���܂ŃA�j��������
    GFL_CLACT_WK_AddAnmFrame( p_view->button_act[bttn_no], FX32_CONST(2) );
  }

#endif //_OAMBUTTON //@@OO
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vier�̂ق��̃��[�h�Z�b�g�ɐl�̖��O��ݒ�
 *
 *	@param	p_wk			���[�N
 *	@param	friendno		�F�B�i���o�[
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
 *	@brief	�F�B�}�b�`�f�[�^�̃X�e�[�^�X�擾
 *
 *	@param	wk			�V�X�e�����[�N
 *	@param	idx			�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static WIFI_STATUS* WifiFriendMatchStatusGet( u32 idx )
{
  GF_ASSERT( idx < WIFIP2PMATCH_MEMBER_MAX );

#ifdef WFP2PM_MANY_OBJ
  idx = 0;
#endif
  return (WIFI_STATUS*)GFL_NET_DWC_GetFriendInfo( idx );
  //	return &wk->pMatch->friendMatchStatus[ idx ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�F�B�̏�Ԃ��擾����
 *
 *	@param	idx		�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u8 WifiDwc_getFriendStatus( int idx )
{
#ifdef WFP2PM_MANY_OBJ
  idx = 0;
#endif
  return GFL_NET_DWC_getFriendStatus( idx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	P2P�ʐM�Q�[�����J�n����B
 *
 *	@param	friendno	�^�[�Q�b�g�ԍ�	�i-1�Ȃ�e�j
 *	@param	status		�X�e�[�^�X
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL WifiP2PMatch_CommWifiBattleStart( WIFIP2PMATCH_WORK* wk, int friendno, int status )
{

  // �{�C�X�`���b�g�ݒ�
  // �{�C�X�`���b�g��BGM���ʂ̊֌W�𐮗� tomoya takahashi
  GFL_NET_DWC_SetVChat(WIFI_STATUS_GetVChatStatus( wk->pMatch ));

  // �ڑ�����O�ɂS�l��W�ő����Ă���\���̂���
  // �R�}���h��ݒ肷��
  CommCommandWFP2PMF_MatchStartInitialize(wk);

  return GFL_NET_StateStartWifiPeerMatch( friendno );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�F�B��W���@�ݒ�ON
 *
 *	@param	wk	���[�N
 */
//-----------------------------------------------------------------------------
static void FriendRequestWaitOn( WIFIP2PMATCH_WORK* wk, BOOL msg_on )
{
  if( wk->friend_request_wait == FALSE ){
    // ���b�Z�[�W�������āA�v���C���[������~����
    wk->friend_request_wait = TRUE;
    WIFI_MCR_PlayerMovePause( &wk->matchroom, TRUE );

    if( msg_on == TRUE ){
      WifiP2PMatchMessagePrint( wk, msg_wifilobby_142, FALSE );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�F�B��W���@�ݒ�OFF
 *
 *	@param	wk	���[�N
 */
//-----------------------------------------------------------------------------
static void FriendRequestWaitOff( WIFIP2PMATCH_WORK* wk )
{
  if( wk->friend_request_wait == TRUE ){
    // ���b�Z�[�W�������āA�v���C���[������J�n����
    wk->friend_request_wait = FALSE;
    EndMessageWindowOff( wk );
    WIFI_MCR_PlayerMovePause( &wk->matchroom, FALSE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�F�B��W���t���O�擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	��W��
 *	@retval	FALSE	�ڂ��イ���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FriendRequestWaitFlagGet( const WIFIP2PMATCH_WORK* cp_wk )
{
  return cp_wk->friend_request_wait;
}



// �v���Z�X��`�f�[�^
const GFL_PROC_DATA WifiP2PMatchProcData = {
  WifiP2PMatchProc_Init,
  WifiP2PMatchProc_Main,
  WifiP2PMatchProc_End,
};

