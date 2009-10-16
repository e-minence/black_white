//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_p2pmatchfour.c
 *	@brief		wifi�S�l�}�b�`���O��p���
 *	@author		tomoya takahashi
 *	@data		2007.05.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
//#include <calctool.h>
#include "arc_def.h"

#include "message.naix"
#include "msg/msg_wifi_lobby.h"


#include "comm_command_wfp2pmf.h"
#include "comm_command_wfp2pmf_func.h"

#include "net_app/wifi2dmap/wf2dmap_obj.h"
#include "net_app/wifi2dmap/wf2dmap_objdraw.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "system/main.h"

//#include "net/dwc_rapfriend.h"
//#include "system/snd_tool.h"
//#include "wifi_p2pmatch_se.h"
#include "font/font.naix"

#include "net/dwc_rap.h"
#include "wifi_p2pmatchfour.h"
#include "wifi_p2pmatch_local.h"
#include "wifi_p2pmatchfour_local.h"

#include "wifip2pmatch.naix"			// �O���t�B�b�N�A�[�J�C�u��`

#include "print/str_tool.h"


//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
 */
//-----------------------------------------------------------------------------

// WIFI2DMAP�V�X�e���I�[�o�[���C
FS_EXTERN_OVERLAY(wifi2dmap);

// �Z���A�N�^�[
#define WIFI_P2PMATCH_RESNUM	(4)	// ���\�[�X��
#define WIFI_P2PMATCH_CLACTWK_NUM	(4)		// �A�N�^�[���[�N��
#define WIFI_P2PMATCH_LOADRESNUM	(4)		// �ǂݍ��ރ��\�[�X��
#define WIFI_P2PMATCH_VRAMMAINSIZ	(128*1024)
#define WIFI_P2PMATCH_VRAMSUBSIZ	(16*1024)
#define WIFI_P2PMATCH_VRAMTRMAN_NUM	( 16 )

// ���b�Z�[�W�\������
#define WIFI_P2PMATCH_MSG_WAIT	( 30 )


#define _TIMING_GAME_CHECK  (13)// �Ȃ���������
#define _TIMING_GAME_CHECK2  (14)// �Ȃ���������
#define _TIMING_GAME_START  (15)// �^�C�~���O�����낦��
#define _TIMING_GAME_START2  (18)// �^�C�~���O�����낦��
#define _TIMING_BATTLE_END  (16)// �^�C�~���O�����낦��


#define RECONNECT_WAIT	(90)

#define KO_ENTRYNUM_CHECK_WAIT	(10*30)

#define CONNECT_TIMEOUT		(60*30)

//@SE�̊֐��������Ɍ��܂�����Ή�
#define SOUND_SEPLAY(seno)       ((void) 0)


//-------------------------------------
///	���C������
//=====================================
enum {
	WFP2PMF_MAIN_WIPE_S,
	WFP2PMF_MAIN_WIPE_E,
	WFP2PMF_MAIN_MATCHWAIT,
	WFP2PMF_MAIN_EWIPE_S,
	WFP2PMF_MAIN_EWIPE_E,
} ;


//-------------------------------------
///	�e����STATUS
//=====================================
enum {
	WFP2PMF_OYA_STATUS_INIT,				// �e�̏�����
	WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT,	// CONNECT�҂�������
	WFP2PMF_OYA_STATUS_CONNECT_WAIT,		// CONNECT�҂�
	WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT00,// CONNECT�҂�
	WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT01,// CONNECT�����l�����邩�I��������
	WFP2PMF_OYA_STATUS_CONNECT_SELECT,		// CONNECT�����l�����邩�I��
	WFP2PMF_OYA_STATUS_CONNECT_MIN_INIT,	// �Q�[���J�n���������A�l��������Ȃ�
	WFP2PMF_OYA_STATUS_CONNECT_MIN,			// �Q�[���J�n���������A�l��������Ȃ�
	WFP2PMF_OYA_STATUS_START_INIT00,			// �Q�[���J�n�����@�J�n
	WFP2PMF_OYA_STATUS_START_INIT01,			// �Q�[���J�n�����@�J�n
	WFP2PMF_OYA_STATUS_START_SELECT,		// �Q�[���J�n�����ɍs�����I��
	WFP2PMF_OYA_STATUS_START,				// �Q�[���J�n��
	WFP2PMF_OYA_STATUS_STARTNUMCHECK,		// �Q�[���J�n�@�l���`�F�b�N
	WFP2PMF_OYA_STATUS_STARTSYNC_INIT,		// �Q�[���J�n����
	WFP2PMF_OYA_STATUS_STARTSYNC,			// �Q�[���J�n����
	WFP2PMF_OYA_STATUS_STATE_SEND,			// �Q�[���J�n	�X�e�[�^�X���M
	WFP2PMF_OYA_STATUS_STATE_CHECK,			// �Q�[���J�n	�X�e�[�^�X�`�F�b�N
	WFP2PMF_OYA_STATUS_MYSTATUS_WAIT,		// �Q�[���J�n	�f�[�^�҂�
	WFP2PMF_OYA_STATUS_GAME_START,			// �J�n
	WFP2PMF_OYA_STATUS_END_INIT00,			// �ҋ@��Ԃ���߂邩�@�J�n
	WFP2PMF_OYA_STATUS_END_INIT01,			// �ҋ@��Ԃ���߂邩�@�J�n
	WFP2PMF_OYA_STATUS_END_SELECT,			// �ҋ@��Ԃ���߂邩�@�I��
	WFP2PMF_OYA_STATUS_END2_INIT00,			// �ҋ@��Ԃ���߂�Ɓ@���U�@�J�n
	WFP2PMF_OYA_STATUS_END2_INIT01,			// �ҋ@��Ԃ���߂�Ɓ@���U�@�J�n
	WFP2PMF_OYA_STATUS_END2_SELECT,			// �ҋ@��Ԃ���߂�Ɓ@���U�@�I��
	WFP2PMF_OYA_STATUS_END3_INIT,			// �ҋ@��Ԃ���߂�Ɓ@���U�@�J�n
	WFP2PMF_OYA_STATUS_END,					// �ҋ@��ԏI��
	WFP2PMF_OYA_STATUS_ERR_INIT,			// �ʐM�G���[�ɂ��I�� �J�n
	WFP2PMF_OYA_STATUS_ERR_INIT01,			// �ʐM�G���[�ɂ��I�� �J�n
	WFP2PMF_OYA_STATUS_ERR,					// �ʐM�G���[�ɂ��I��
	WFP2PMF_OYA_STATUS_VCHAT_INIT00,		// �{�C�X�`���b�g�ύX	�J�n
	WFP2PMF_OYA_STATUS_VCHAT_INIT01,		// �{�C�X�`���b�g�ύX	�J�n
	WFP2PMF_OYA_STATUS_VCHAT_SELECT,		// �{�C�X�`���b�g�ύX	�I��
	WFP2PMF_OYA_STATUS_NUM
} ;

//-------------------------------------
///	�q������
//=====================================
enum {
	WFP2PMF_KO_STATUS_ENTRY_OYAWAITINIT,// �e�̏��擾�҂�
	WFP2PMF_KO_STATUS_ENTRY_OYAWAIT,	// �e�̏��擾�҂�
	WFP2PMF_KO_STATUS_ENTRY_INIT,		// ENTRY�����҂��J�n
	WFP2PMF_KO_STATUS_ENTRY_WAIT,		// ENTRY�����҂�
	WFP2PMF_KO_STATUS_ENTRY_OKINIT,		// ENTRYOK������
	WFP2PMF_KO_STATUS_ENTRY_OK,			// ENTRYOK
	WFP2PMF_KO_STATUS_ENTRY_NGINIT,		// ENTRYNG������
	WFP2PMF_KO_STATUS_ENTRY_NG,			// ENTRYNG
	WFP2PMF_KO_STATUS_CONNECT_INIT,		// CONNECT�����҂��J�n
	WFP2PMF_KO_STATUS_CONNECT_WAIT,		// CONNECT�����҂�
	WFP2PMF_KO_STATUS_START,			// �J�n
	WFP2PMF_KO_STATUS_STATE_SEND,			// �Q�[���J�n	�X�e�[�^�X���M
	WFP2PMF_KO_STATUS_STATE_CHECK,			// �Q�[���J�n	�X�e�[�^�X�`�F�b�N
	WFP2PMF_KO_STATUS_MYSTATUS_WAIT,		// �Q�[���J�n	�f�[�^�҂�
	WFP2PMF_KO_STATUS_GAME_START,			// �J�n
	WFP2PMF_KO_STATUS_ERR_INIT,			// �G���[�I��	�J�n
	WFP2PMF_KO_STATUS_ERR_INIT01,		// �G���[�I��	�J�n
	WFP2PMF_KO_STATUS_ERR,				// �G���[�I��
	WFP2PMF_KO_STATUS_VCHAT_INIT00,		// �{�C�X�`���b�g�ύX	�J�n
	WFP2PMF_KO_STATUS_VCHAT_INIT01,		// �{�C�X�`���b�g�ύX	�J�n
	WFP2PMF_KO_STATUS_VCHAT_SELECT,		// �{�C�X�`���b�g�ύX	�I��
	WFP2PMF_KO_STATUS_CONLOCK_INIT00,		// �ʐMLOCK���Ȃ̂ōĐڑ�
	WFP2PMF_KO_STATUS_CONLOCK_INIT01,		// �ʐMLOCK���Ȃ̂ōĐڑ�
	WFP2PMF_KO_STATUS_CONLOCK_SELECT,		// �ʐMLOCK���Ȃ̂ōĐڑ�
	WFP2PMF_KO_STATUS_CONLOCK_RECON,		// �ʐMLOCK���@�Đڑ�
	WFP2PMF_KO_STATUS_CONLOCK_RECON01,		// �ʐMLOCK���@�Đڑ��O�P
	WFP2PMF_KO_STATUS_CONLOCK_RECONWAIT,	// �ʐMLOCK���@�Đڑ��҂�
	WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET00,	// �ʐMLOCK���@��ʍ\�z
	WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET01,	// �ʐMLOCK���@��ʍ\�z
	WFP2PMF_KO_STATUS_CONLOCK_DECON,		// �ʐMLOCK���@�ؒf
	WFP2PMF_KO_STATUS_NUM
} ;

//-------------------------------------
///	�`��I�u�W�F�N�g����V�[�P���X
//=====================================
enum {
	WFP2PMF_DRAWOBJ_MOVENONE,
	WFP2PMF_DRAWOBJ_MOVEINIT,
	WFP2PMF_DRAWOBJ_MOVEIN,
	WFP2PMF_DRAWOBJ_MOVENORMAL,
	WFP2PMF_DRAWOBJ_MOVENUM,
} ;
#define WFP2PMF_DRAWOBJ_INTIME	( 16 )	// ���o�G�t�F�N�g�^�C��
// �e������INTIME�̍�
static const u8 WFP2PMF_DRAWOBJ_INTIME_DIV[ WF2DMAP_WAY_NUM ] = {
	8,
	0,
	4,
	12,
};



//-------------------------------------
///	�G���g���[�t���O
//=====================================
enum {
	WFP2PMF_ENTRY_WAIT,
	WFP2PMF_ENTRY_KO,
	WFP2PMF_ENTRY_NG,
	WFP2PMF_ENTRY_LOCK,
} ;


//-------------------------------------
///	OAM�\���V�[�P���X
//=====================================
enum {
	WFP2PMF_OAM_SEQ_NONE,
	WFP2PMF_OAM_SEQ_INIT,
	WFP2PMF_OAM_SEQ_WAIT,
} ;



//-------------------------------------
///		BGL
//=====================================
#define WFP2PMF_BACK_PAL		(0)	// �w�i�p���b�g
#define WFP2PMF_BACK_PALNUM		(18)// �A�j���̐�
#define WFP2PMF_BACK_PALANM	(3)// �A�j���t���[��
#define WFP2PMF_TALK_PAL	(1)	// �����p���b�g

#define FBMP_COL_WHITE  (15)  //�E�C���h�E�J���[��

// �p���b�g�A�j���f�[�^
static const u8 WFP2PMF_BACK_PAL_IDX[ WFP2PMF_BACK_PALNUM ] = {
	5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 5, 5, 5,
};

// VCHATICON
#define WFP2PMF_VCHAT_ICON_FRM_NUM	( GFL_BG_FRAME2_M )
#define WFP2PMF_VCHAT_ICON_PAL		( 5 )
#define WFP2PMF_VCHAT_ICON_PALNUM	( 1 )
#define WFP2PMF_VCHAT_ICON_CG		( 0 )
#define WFP2PMF_VCHAT_ICON_CGSIZ	( 24*2 )
#define WFP2PMF_VCHAT_ICON_SIZ		(2)
#define WFP2PMF_VCHAT_ICON_WX		(18)
#define WFP2PMF_VCHAT_ICON_WY		(5)
enum{
	WFP2PMF_VCHATICON_NONE,
	WFP2PMF_VCHATICON_OFF,
	WFP2PMF_VCHATICON_ON,
};


// BG1
#define WFP2PMF_BG1_TALKWIN_CGX	( 1 )
#define WFP2PMF_BG1_TALKWIN_PAL	( 2 )
#define WFP2PMF_BG1_MENUWIN_CGX	( WFP2PMF_BG1_TALKWIN_CGX+TALK_WIN_CGX_SIZ )
#define WFP2PMF_BG1_MENUWIN_PAL	( 3 )

// BG2
#define WFP2PMF_BG2_MENUWIN_CGX	( WFP2PMF_VCHAT_ICON_CG+WFP2PMF_VCHAT_ICON_CGSIZ )
#define WFP2PMF_BG2_MENUWIN_PAL	( 4 )

// ���b�Z�[�W�E�B���h�E
#define WFP2PMF_MSGWIN_X		( 2 )
#define WFP2PMF_MSGWIN_Y		( 19 )
#define WFP2PMF_MSGWIN_SIZX		( 27 )
#define WFP2PMF_MSGWIN_SIZY		( 4 )
#define WFP2PMF_MSGWIN_PAL		( WFP2PMF_TALK_PAL )
#define WFP2PMF_MSGWIN_CGX		( WFP2PMF_BG1_MENUWIN_CGX + MENU_WIN_CGX_SIZ )
#define WFP2PMF_MSGWIN_SIZ		( WFP2PMF_MSGWIN_SIZX*WFP2PMF_MSGWIN_SIZY )

// �^�C�g���E�B���h�E
#define WFP2PMF_TITLEWIN_X		( 1 )
#define WFP2PMF_TITLEWIN_Y		( 1 )
#define WFP2PMF_TITLEWIN_SIZX	( 27 )
#define WFP2PMF_TITLEWIN_SIZY	( 2 )
#define WFP2PMF_TITLEWIN_PAL	( WFP2PMF_TALK_PAL )
#define WFP2PMF_TITLEWIN_CGX	( WFP2PMF_MSGWIN_CGX + WFP2PMF_MSGWIN_SIZ )
#define WFP2PMF_TITLEWIN_SIZ	( WFP2PMF_TITLEWIN_SIZX*WFP2PMF_TITLEWIN_SIZY )

// �{�C�X�`���b�g�E�B���h�E
#define WFP2PMF_VCHATWIN_X		( 2 )
#define WFP2PMF_VCHATWIN_Y		( 16 )
#define WFP2PMF_VCHATWIN_SIZX	( 28 )
#define WFP2PMF_VCHATWIN_SIZY	( 2 )
#define WFP2PMF_VCHATWIN_PAL	( WFP2PMF_TALK_PAL )
#define WFP2PMF_VCHATWIN_CGX	( WFP2PMF_TITLEWIN_CGX + WFP2PMF_TITLEWIN_SIZ )
#define WFP2PMF_VCHATWIN_SIZ	( WFP2PMF_VCHATWIN_SIZX*WFP2PMF_VCHATWIN_SIZY )

// CONNECT���X�g�E�B���h�E
#define WFP2PMF_CONLISTWIN_X		( 1 )
#define WFP2PMF_CONLISTWIN_Y		( 5 )
#define WFP2PMF_CONLISTWIN_SIZX	( 20 )
#define WFP2PMF_CONLISTWIN_SIZY	( 8 )
#define WFP2PMF_CONLISTWIN_PAL	( WFP2PMF_TALK_PAL )
#define WFP2PMF_CONLISTWIN_CGX	( WFP2PMF_VCHATWIN_CGX + WFP2PMF_VCHATWIN_SIZ )
#define WFP2PMF_CONLISTWIN_SIZ	( WFP2PMF_CONLISTWIN_SIZX*WFP2PMF_CONLISTWIN_SIZY )
#define WFP2PMF_CONLISTWIN_ONELIST_Y	( 2 )
#define WFP2PMF_CONLISTWIN_ID_X	( 72 )

// NEWCONNECT���X�g�E�B���h�E
#define WFP2PMF_NEWCONWIN_X		( 23 )
#define WFP2PMF_NEWCONWIN_Y		( 5 )
#define WFP2PMF_NEWCONWIN_SIZX	( 7 )
#define WFP2PMF_NEWCONWIN_SIZY	( 5 )
#define WFP2PMF_NEWCONWIN_PAL	( WFP2PMF_TALK_PAL )
#define WFP2PMF_NEWCONWIN_CGX	( WFP2PMF_CONLISTWIN_CGX + WFP2PMF_CONLISTWIN_SIZ )
#define WFP2PMF_NEWCONWIN_SIZ	( WFP2PMF_NEWCONWIN_SIZX*WFP2PMF_NEWCONWIN_SIZY )

// yes no win
#define WFP2PMF_YESNOWIN_FRM_NUM	( GFL_BG_FRAME2_M )
#define WFP2PMF_YESNOWIN_X			( 25 )
#define WFP2PMF_YESNOWIN_Y			( 13 )
#define WFP2PMF_YESNOWIN_SIZX		( 5 )
#define WFP2PMF_YESNOWIN_SIZY		( 4 )
#define WFP2PMF_YESNOWIN_PAL		( WFP2PMF_TALK_PAL )
#define WFP2PMF_YESNOWIN_CGX		( WFP2PMF_BG2_MENUWIN_CGX + MENU_WIN_CGX_SIZ )
#define WFP2PMF_YESNOWIN_SIZ		( WFP2PMF_YESNOWIN_SIZX * WFP2PMF_YESNOWIN_SIZY )

static const BMPWIN_YESNO_DAT WFP2PMF_YESNOBMPDAT = {
	WFP2PMF_YESNOWIN_FRM_NUM, WFP2PMF_YESNOWIN_X, WFP2PMF_YESNOWIN_Y,
	WFP2PMF_YESNOWIN_PAL, WFP2PMF_YESNOWIN_CGX
	};

#define	_COL_N_BLACK	( GF_PRINTCOLOR_MAKE( 1, 2, 15 ) )		// �t�H���g�J���[�F��
#define	_COL_N_BLACK_C	( GF_PRINTCOLOR_MAKE( 1, 2, 0 ) )		// �t�H���g�J���[�F��


//-------------------------------------
///	���b�Z�[�W�f�[�^ msgdata
//=====================================
#define WFP2PMF_MSGDATA_STRNUM	 ( 256 )
#define WFP2PMF_MSGNO_NONE		(0xff)
#define WFP2PMF_MSGENDWAIT		( 30 )		//  ���b�Z�[�W�\����҂E�G�C�g



//-------------------------------------
///	�^�C�g���f�[�^
//=====================================
static u8 WFP2PMF_TITLE[ WFP2PMF_TYPE_NUM ] = {
	msg_wifilobby_131,
	msg_wifilobby_150,
	msg_wifilobby_150,
	msg_wifilobby_150,
};


//-------------------------------------
///	�I�u�W�F�N�g�o�^�f�[�^
//=====================================
static const WF2DMAP_OBJDATA WFP2PMF_OBJData[ WIFI_P2PMATCH_NUM_MAX ] = {
	{// �e
		208, 48, 0, WF2DMAP_OBJST_NONE,
		WF2DMAP_WAY_DOWN, 0
		},
	{
		232, 72, 1, WF2DMAP_OBJST_NONE,
		WF2DMAP_WAY_LEFT, 0
		},
	{
		208, 96, 2, WF2DMAP_OBJST_NONE,
		WF2DMAP_WAY_UP, 0
		},
	{
		184, 72, 3, WF2DMAP_OBJST_NONE,
		WF2DMAP_WAY_RIGHT, 0
		},
};



//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	�f�[�^
//=====================================
typedef struct {
	TEST_MATCH_WORK*	p_match;		// �T�[�o�[�Ƒ���M���鎩���ƗF�B�̏��
	u8					oya;			// �e���ǂ���
	u8					status;			// ���
	u8					new_con;		// �V�����R�l�N�g
	u8					match_result;	// ��ɕԂ�����
	s32					timer;			// �^�C�}�[
	s32					timeout;		// �^�C���A�E�g
	u8					timeout_flag;	// �^�C���A�E�g�̗L��
	u8					vchat_flag;		// Vchat�t���O
	u8					ko_newcon_flag;	// �q�p�V�K�ڑ������������`�F�b�N
	u8					entry;			// �G���g���[�t���O
	u8					logout_in[WIFI_P2PMATCH_NUM_MAX];// LOG�A�E�g���m
	u8					logout_id[WIFI_P2PMATCH_NUM_MAX];// LOG�A�E�g�����l��ID
	u8					vchat_tmp[WIFI_P2PMATCH_NUM_MAX];	// �P�O��VCHAT
	u8					vchat_now[WIFI_P2PMATCH_NUM_MAX];	// �P�O��VCHAT
	u8					conlist_rewrite;// conlist���ĕ`��
	u8					last_netid;		// �V�K�R�l�N�g���b�N�t���O
	u8					oya_friendid;	// �e�̗F�B�R�[�h
	u8					err_check;		// �G���[�`�F�b�N���K�v��
	u8					entry_on[WIFI_P2PMATCH_NUM_MAX];// �G���g���[�t���O
	u8					oya_start;		// �e����Q�[���J�n����M
	u8					ng_msg_idx;		// NG���b�Z�[�W


} WFP2PMF_DATA;

//-------------------------------------
///	�@�`��I�u�W�F�N�g���[�N
//=====================================
typedef struct {
	WF2DMAP_OBJWK*		p_objwk;		// �I�u�W�F�N�g�V�X�e��
	WF2DMAP_OBJDRAWWK*	p_objdrawwk;	// �I�u�W�F�N�g�`�惏�[�N
	u16 seq;
	s16 count;
} WFP2PMF_DRAWOBJ;


//-------------------------------------
///	�`��f�[�^
//=====================================
typedef struct {
	//	GF_BGL_INI*			p_bgl;				// GF_BGL_INI
	WORDSET*			p_wordset;			// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
	PRINT_QUE			*printQue;
	GFL_FONT 			*fontHandle;
	GFL_MSGDATA*	p_msgman;			// ���b�Z�[�W�f�[�^�}�l�[�W���[
	STRBUF*				p_msgstr;			// ���b�Z�[�W�p������o�b�t�@
	STRBUF*				p_msgtmp;			// ���b�Z�[�W�p������o�b�t�@
	u8					msg_no;				// ���b�Z�[�W���������i���o�[
	u8					msg_wait;			// ���b�Z�[�W�E�G�C�g
	u16					msg_speed;			// ���b�Z�[�W�X�s�[�h

	GFL_CLUNIT* 			clactSet;						// �Z���A�N�^�[�Z�b�g
	GFL_CLSYS_REND*          renddata;						// �ȈՃ����_�[�f�[�^
	//	CLACT_U_EASYRENDER_DATA	renddata;						// �ȈՃ����_�[�f�[�^
	//	CLACT_U_RES_MANAGER_PTR	resMan[WIFI_P2PMATCH_RESNUM];	// �L�����E�p���b�g���\�[�X�}�l�[�W��

	WF2DMAP_OBJSYS*		p_objsys;			// �I�u�W�F�N�g�V�X�e��
	WF2DMAP_OBJDRAWSYS* p_objdraw;			// �I�u�W�F�N�g�`��V�X�e��
	WFP2PMF_DRAWOBJ		objdata[WIFI_P2PMATCH_CLACTWK_NUM];	// �I�u�W�F�N�g�f�[�^


	GFL_BMPWIN*		msgwin;				// ��b�E�C���h�E
	GFL_BMPWIN*		titlewin;			// �����̏�ԕ\��
	GFL_BMPWIN*		vchatwin;			// �{�C�X�`���b�g�E�B���h�E
	GFL_BMPWIN*		conlistwin;			// �R�l�N�g���X�g�E�B���h�E
	GFL_BMPWIN*		newconwin;			// �V�K�R�l�N�g�E�B���h�E

	PRINT_UTIL		msgwinPrintUtil;
	PRINT_UTIL		titlewinPrintUtil;
	PRINT_UTIL	    vchatwinPrintUtil;
	PRINT_UTIL	    conlistwinPrintUtil;
	PRINT_UTIL	    newconwinPrintUtil;

	BMPMENU_WORK*		p_yesnowin;			// yes no�E�B���h�E

	void* p_vchatscrnbuf;
	NNSG2dScreenData*  p_vchatscrn;			// �{�C�X�`���b�g�A�C�R��

	void* p_timewaiticon;

	// �w�i�A�j��
	void*				p_plttbuff;
	NNSG2dPaletteData*	p_pltt;
	s16					pltt_count;
	s16					pltt_idx;
} WFP2PMF_DRAW;


//-------------------------------------
///	�S�l�}�b�`���O��ʃ��[�N
//=====================================
typedef struct _WFP2P_WK{
	WFP2PMF_DATA	data;
	WFP2PMF_DRAW	draw;
} WFP2PMF_WK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WifiP2PMatchFourProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT WifiP2PMatchFourProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT WifiP2PMatchFourProc_End(GFL_PROC * proc, int * seq, void * pwk, void * mywk);

static void VBlankFunc( void * work );

static BOOL WFP2PMF_MyDataOyaCheck( const WFP2PMF_DATA* cp_data );
static void WFP2PMF_GraphicInit( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicDelete( WFP2PMF_WK* p_wk, u32 heapID );
static void WFP2PMF_GraphicBGLInit( WFP2PMF_DRAW* p_draw, u32 heapID );
static void WFP2PMF_GraphicBGLDelete( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicBmpInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicBmpDelete( WFP2PMF_DRAW* p_draw, u32 heapID );
static void WFP2PMF_GraphicMsgInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicMsgDelete( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicBmpMsgInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicClactInit( WFP2PMF_DRAW* p_draw, u32 heapID );
static void WFP2PMF_GraphicClactDelete( WFP2PMF_DRAW* p_draw );

static void WFP2PMF_GraphicMsgBmpStrPut( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u32 heapID );
static void WFP2PMF_GraphicMsgBmpStrPutEx( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u8 wait, u32 heapID );
static void WFP2PMF_GraphicMsgBmpStrPutAll( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u32 heapID );
static BOOL WFP2PMF_GraphicMsgBmpStrMain( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicMsgBmpStrOff( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicNewConStrPut( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID );
static void WFP2PMF_GraphicNewConStrOff( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicConlistStrPutAll( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicConlistStrPut( WFP2PMF_WK* p_wk, WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID );
static void WFP2PMF_GraphicConlistStrOff( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID );

static void WFP2PMF_WordSetPlayerNameSet( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, int set_no, u32 heapID );
static void WFP2PMF_WordSetPlayerIDSet( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, int set_no, u32 heapID );

static void WFP2PMF_PlttAnm( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_PlttAnm_Trans( WFP2PMF_DRAW* p_draw, u32 idx );

static BOOL WFP2PMF_VchatSwitch( WFP2PMF_WK* p_wk, u32 heapID );


static void WFP2PMF_ErrCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_param, u32 heapID );
static void WFP2PMF_ReConErrCheck( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_MatchCheck( const WFP2PMF_WK* cp_wk );
static BOOL WFP2PMF_CommWiFiMatchStart( u32 friendno, u32 type );
static BOOL WFP2PMF_CheckP2PMatchFriendStatus( const WFP2PMF_WK* cp_wk, const WFP2PMF_INIT* cp_param, int friend );

static void WFP2PMF_StatusChange( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init );

static BOOL WFP2PMF_StatusVChatOn( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_StatusVChatOff( WFP2PMF_WK* p_wk );
static void WFP2PMF_StatusVChatOnBmpFade( WFP2PMF_WK* p_wk );
static void WFP2PMF_StatusVChatOffBmpFade( WFP2PMF_WK* p_wk );

static int WFP2PMF_GetEntryNum( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_SetEntryOya( WFP2PMF_WK* p_wk, u32 heapID );

static void WFP2PMF_CommSendNewEntryNg( WFP2PMF_WK* p_wk );
static void WFP2PMF_CommSendAllEntryNg( WFP2PMF_WK* p_wk );

static void WFP2PMF_LogOutCheckStart( WFP2PMF_WK* p_wk );
static void WFP2PMF_LogOutCheckEnd( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_LogOutCheck( WFP2PMF_WK* p_wk );
static void WFP2PMF_LogOutCallBack( u16 aid, void* pWork );

static BOOL WFP2PMF_ConListWriteCheck( WFP2PMF_WK* p_wk );
static void WFP2PMF_ConListWriteReq( WFP2PMF_WK* p_wk );

static void WFP2PMF_CommInfoSetEntry( WFP2PMF_WK* p_wk, u32 aid, u32 heapID );

static void WFP2PMF_DrawOamInit( WFP2PMF_WK* p_wk, u32 aid, u32 heapID );
static void WFP2PMF_DrawOamMain( WFP2PMF_WK* p_wk );
static void WFP2PMF_DrawOamDelete( WFP2PMF_WK* p_wk );

static BOOL WFP2PMF_NewConLockCheck( const WFP2PMF_WK* cp_wk );

static void WFP2PMF_NewConLockSend( WFP2PMF_WK* p_wk, u32 aid );
static void WFP2PMF_NewConLockNewCon( WFP2PMF_WK* p_wk );

static void WFP2PMF_KoRecvEntry( WFP2PMF_WK* p_wk, u32 heapID );

static void WFP2PMF_TimeWaitIconOn( WFP2PMF_WK* p_wk );
static void WFP2PMF_TimeWaitIconOff( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID );

static void WFP2PMF_DrawObjMoveNone( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj );
static void WFP2PMF_DrawObjMoveInit( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj );
static void WFP2PMF_DrawObjMoveIn( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj );
static void WFP2PMF_DrawObjMoveNormal( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj );

static void WFP2PMF_VchatCommSend( WFP2PMF_WK* p_wk );

static BOOL WFP2PMF_MatchOkCheck( WFP2PMF_WK* p_wk );

static BOOL WFP2PMF_Oya_CheckConnectPlayer( const WFP2PMF_WK* cp_wk, const WFP2PMF_INIT* cp_param );


static void WFP2PMF_TimeOut_Start( WFP2PMF_WK* p_wk );
static void WFP2PMF_TimeOut_Stop( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_TimeOut_Main( WFP2PMF_WK* p_wk );

//-------------------------------------
/// �e����f�[�^
//=====================================
static BOOL WFP2PMF_OyaInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectWaitInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectSelectInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectSelectInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectMinInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectMin( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStartInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStartInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStartSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStartNumCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStartSyncInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStartSync( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStateSend( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStateCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaMyStatusWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaGameStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaEndInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaEndInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaEndSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaEnd2Init00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaEnd2Init01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaEnd2Select( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaEnd3Init( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaEnd( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaErrInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaErrInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaErr( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaVchatInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaVchatInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaVchatSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );




//-------------------------------------
/// �q����f�[�^
//=====================================
static BOOL WFP2PMF_KoEntryOyaWaitInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryOyaWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryOkInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryOk( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryNgInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryNg( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConnectInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConnectWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoStateSend( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoStateCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoMyStatusWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoGameStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoErrInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoErrInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoErr( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoVchatInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoVchatInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoVchatSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReCon( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReCon01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReConWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReConGraphicReset00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReConGraphicReset01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockDeCon( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );


//-------------------------------------
///	�e�֐��e�[�u��
//=====================================
static BOOL (* const pOyaFunc[ WFP2PMF_OYA_STATUS_NUM ])( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID ) = {
	WFP2PMF_OyaInit,								// WFP2PMF_OYA_STATUS_INIT,				// �e�̏�����
	WFP2PMF_OyaConnectWaitInit,                     // WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT,	// CONNECT�҂�������
	WFP2PMF_OyaConnectWait,                         // WFP2PMF_OYA_STATUS_CONNECT_WAIT,		// CONNECT�҂�
	WFP2PMF_OyaConnectSelectInit00,                 // WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT00,// CONNECT�҂�
	WFP2PMF_OyaConnectSelectInit01,                 // WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT01,// CONNECT�����l�����邩�I��������
	WFP2PMF_OyaConnectSelect,                       // WFP2PMF_OYA_STATUS_CONNECT_SELECT,		// CONNECT�����l�����邩�I��
	WFP2PMF_OyaConnectMinInit,                      // WFP2PMF_OYA_STATUS_CONNECT_MIN_INIT,	// �Q�[���J�n���������A�l��������Ȃ�
	WFP2PMF_OyaConnectMin,                          // WFP2PMF_OYA_STATUS_CONNECT_MIN,			// �Q�[���J�n���������A�l��������Ȃ�
	WFP2PMF_OyaStartInit00,                         // WFP2PMF_OYA_STATUS_START_INIT00,			// �Q�[���J�n�����@�J�n
	WFP2PMF_OyaStartInit01,                         // WFP2PMF_OYA_STATUS_START_INIT01,			// �Q�[���J�n�����@�J�n
	WFP2PMF_OyaStartSelect,                         // WFP2PMF_OYA_STATUS_START_SELECT,		// �Q�[���J�n�����ɍs�����I��
	WFP2PMF_OyaStart,                               // WFP2PMF_OYA_STATUS_START,				// �Q�[���J�n��
	WFP2PMF_OyaStartNumCheck,                       // WFP2PMF_OYA_STATUS_STARTNUMCHECK,		// �Q�[���J�n�@�l���`�F�b�N
	WFP2PMF_OyaStartSyncInit,                       // WFP2PMF_OYA_STATUS_STARTSYNC_INIT,		// �Q�[���J�n����
	WFP2PMF_OyaStartSync,                           // WFP2PMF_OYA_STATUS_STARTSYNC,			// �Q�[���J�n����
	WFP2PMF_OyaStateSend,                           // WFP2PMF_OYA_STATUS_STATE_SEND,			// �Q�[���J�n	�X�e�[�^�X���M
	WFP2PMF_OyaStateCheck,                          // WFP2PMF_OYA_STATUS_STATE_CHECK,			// �Q�[���J�n	�X�e�[�^�X�`�F�b�N
	WFP2PMF_OyaMyStatusWait,                        // WFP2PMF_OYA_STATUS_MYSTATUS_WAIT,		// �Q�[���J�n	�f�[�^�҂�
	WFP2PMF_OyaGameStart,	                        // WFP2PMF_OYA_STATUS_GAME_START,			// �J�n
	WFP2PMF_OyaEndInit00,                           // WFP2PMF_OYA_STATUS_END_INIT00,			// �ҋ@��Ԃ���߂邩�@�J�n
	WFP2PMF_OyaEndInit01,                           // WFP2PMF_OYA_STATUS_END_INIT01,			// �ҋ@��Ԃ���߂邩�@�J�n
	WFP2PMF_OyaEndSelect,                           // WFP2PMF_OYA_STATUS_END_SELECT,			// �ҋ@��Ԃ���߂邩�@�I��
	WFP2PMF_OyaEnd2Init00,                          // WFP2PMF_OYA_STATUS_END2_INIT00,			// �ҋ@��Ԃ���߂�Ɓ@���U�@�J�n
	WFP2PMF_OyaEnd2Init01,                          // WFP2PMF_OYA_STATUS_END2_INIT01,			// �ҋ@��Ԃ���߂�Ɓ@���U�@�J�n
	WFP2PMF_OyaEnd2Select,                          // WFP2PMF_OYA_STATUS_END2_SELECT,			// �ҋ@��Ԃ���߂�Ɓ@���U�@�I��
	WFP2PMF_OyaEnd3Init,                            // WFP2PMF_OYA_STATUS_END3_INIT,			// �ҋ@��Ԃ���߂�Ɓ@���U�@�J�n
	WFP2PMF_OyaEnd,                                 // WFP2PMF_OYA_STATUS_END,					// �ҋ@��ԏI��
	WFP2PMF_OyaErrInit,                             // WFP2PMF_OYA_STATUS_ERR_INIT,			// �ʐM�G���[�ɂ��I�� �J�n
	WFP2PMF_OyaErrInit01,                           // WFP2PMF_OYA_STATUS_ERR_INIT01,			// �ʐM�G���[�ɂ��I�� �J�n
	WFP2PMF_OyaErr,                                 // WFP2PMF_OYA_STATUS_ERR,					// �ʐM�G���[�ɂ��I��
	WFP2PMF_OyaVchatInit00,                         // WFP2PMF_OYA_STATUS_VCHAT_INIT00,		// �{�C�X�`���b�g�ύX	�J�n
	WFP2PMF_OyaVchatInit01,                         // WFP2PMF_OYA_STATUS_VCHAT_INIT01,		// �{�C�X�`���b�g�ύX	�J�n
	WFP2PMF_OyaVchatSelect,                         // WFP2PMF_OYA_STATUS_VCHAT_SELECT,		// �{�C�X�`���b�g�ύX	�I��
};

//-------------------------------------
///	�q�֐��e�[�u��
//=====================================
static BOOL (* const pKoFunc[ WFP2PMF_KO_STATUS_NUM ])( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID ) = {
	WFP2PMF_KoEntryOyaWaitInit,					// WFP2PMF_KO_STATUS_ENTRY_OYAWAITINIT,// �e�̏��擾�҂�
	WFP2PMF_KoEntryOyaWait,                     // WFP2PMF_KO_STATUS_ENTRY_OYAWAIT,	// �e�̏��擾�҂�
	WFP2PMF_KoEntryInit,                        // WFP2PMF_KO_STATUS_ENTRY_INIT,		// ENTRY�����҂��J�n
	WFP2PMF_KoEntryWait,                        // WFP2PMF_KO_STATUS_ENTRY_WAIT,		// ENTRY�����҂�
	WFP2PMF_KoEntryOkInit,                      // WFP2PMF_KO_STATUS_ENTRY_OKINIT,		// ENTRYOK������
	WFP2PMF_KoEntryOk,                          // WFP2PMF_KO_STATUS_ENTRY_OK,			// ENTRYOK
	WFP2PMF_KoEntryNgInit,                      // WFP2PMF_KO_STATUS_ENTRY_NGINIT,		// ENTRYNG������
	WFP2PMF_KoEntryNg,                          // WFP2PMF_KO_STATUS_ENTRY_NG,			// ENTRYNG
	WFP2PMF_KoConnectInit,                      // WFP2PMF_KO_STATUS_CONNECT_INIT,		// CONNECT�����҂��J�n
	WFP2PMF_KoConnectWait,                      // WFP2PMF_KO_STATUS_CONNECT_WAIT,		// CONNECT�����҂�
	WFP2PMF_KoStart,                            // WFP2PMF_KO_STATUS_START,			// �J�n
	WFP2PMF_KoStateSend,                        // WFP2PMF_KO_STATUS_STATE_SEND,			// �Q�[���J�n	�X�e�[�^�X���M
	WFP2PMF_KoStateCheck,                       // WFP2PMF_KO_STATUS_STATE_CHECK,			// �Q�[���J�n	�X�e�[�^�X�`�F�b�N
	WFP2PMF_KoMyStatusWait,                     // WFP2PMF_KO_STATUS_MYSTATUS_WAIT,		// �Q�[���J�n	�f�[�^�҂�
	WFP2PMF_KoGameStart,	                    // WFP2PMF_KO_STATUS_GAME_START,			// �J�n
	WFP2PMF_KoErrInit,                          // WFP2PMF_KO_STATUS_ERR_INIT,			// �G���[�I��	�J�n
	WFP2PMF_KoErrInit01,                        // WFP2PMF_KO_STATUS_ERR_INIT01,		// �G���[�I��	�J�n
	WFP2PMF_KoErr,                              // WFP2PMF_KO_STATUS_ERR,				// �G���[�I��
	WFP2PMF_KoVchatInit00,                      // WFP2PMF_KO_STATUS_VCHAT_INIT00,		// �{�C�X�`���b�g�ύX	�J�n
	WFP2PMF_KoVchatInit01,                      // WFP2PMF_KO_STATUS_VCHAT_INIT01,		// �{�C�X�`���b�g�ύX	�J�n
	WFP2PMF_KoVchatSelect,                      // WFP2PMF_KO_STATUS_VCHAT_SELECT,		// �{�C�X�`���b�g�ύX	�I��
	WFP2PMF_KoConLockInit00,                    // WFP2PMF_KO_STATUS_CONLOCK_INIT00,		// �ʐMLOCK���Ȃ̂ōĐڑ�
	WFP2PMF_KoConLockInit01,                    // WFP2PMF_KO_STATUS_CONLOCK_INIT01,		// �ʐMLOCK���Ȃ̂ōĐڑ�
	WFP2PMF_KoConLockSelect,                    // WFP2PMF_KO_STATUS_CONLOCK_SELECT,		// �ʐMLOCK���Ȃ̂ōĐڑ�
	WFP2PMF_KoConLockReCon,                     // WFP2PMF_KO_STATUS_CONLOCK_RECON,		// �ʐMLOCK���@�Đڑ�
	WFP2PMF_KoConLockReCon01,                   // WFP2PMF_KO_STATUS_CONLOCK_RECON01,		// �ʐMLOCK���@�Đڑ��O�P
	WFP2PMF_KoConLockReConWait,                 // WFP2PMF_KO_STATUS_CONLOCK_RECONWAIT,	// �ʐMLOCK���@�Đڑ��҂�
	WFP2PMF_KoConLockReConGraphicReset00,       // WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET00,	// �ʐMLOCK���@��ʍ\�z
	WFP2PMF_KoConLockReConGraphicReset01,       // WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET01,	// �ʐMLOCK���@��ʍ\�z
	WFP2PMF_KoConLockDeCon,                     // WFP2PMF_KO_STATUS_CONLOCK_DECON,		// �ʐMLOCK���@�ؒf
};


static GFL_DISP_VRAM p2pmatchfourVramTbl = {
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



// �`��I�u�W�F�N�g����
static void (* const pDrawObjMoveFunc[ WFP2PMF_DRAWOBJ_MOVENUM ])( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj ) = {
	WFP2PMF_DrawObjMoveNone,
	WFP2PMF_DrawObjMoveInit,
	WFP2PMF_DrawObjMoveIn,
	WFP2PMF_DrawObjMoveNormal,
};


//----------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param	proc	�v���Z�X�f�[�^
 * @param	seq		�V�[�P���X
 *
 * @return	������
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WifiP2PMatchFourProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	WFP2PMF_WK* p_wk;
	WFP2PMF_INIT* p_init = pwk;
	BOOL result;

	// wifi_2dmap�I�[�o�[���C�Ǎ���
	//	GFL_OVERLAY_Load( FS_OVERLAY_ID(wifi2dmap));

	// �ʐM�����`�F�b�N
	//result = CommStateIsWifiConnect();
	//GF_ASSERT( result == TRUE );

	// �q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFI_FOURMATCH, 0x18000 );
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFI_FOURMATCHVCHAT, 0xa000 );

	// �v���Z�X���[�N�쐬
	p_wk = GFL_PROC_AllocWork( proc, sizeof(WFP2PMF_WK), HEAPID_WIFI_FOURMATCH );
	GFL_STD_MemClear( p_wk, sizeof(WFP2PMF_WK) );


	// VCHAT���[�N�̏�����
	GFL_STD_MemFill( p_wk->data.vchat_tmp, TRUE, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// �P�O��VCHAT
	GFL_STD_MemFill( p_wk->data.vchat_now, TRUE, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// �P�O��VCHAT



	// VRAM�]���}�l�[�W��������
	//initVramTransferManagerHeap( WIFI_P2PMATCH_VRAMTRMAN_NUM, HEAPID_WIFI_FOURMATCH );


	// �ʐM�f�[�^�擾
	p_wk->data.p_match = GFL_NET_GetWork();
	p_wk->data.p_match->myMatchStatus.vchat = p_wk->data.p_match->myMatchStatus.vchat_org;

	// �������e���`�F�b�N
	p_wk->data.oya = WFP2PMF_MyDataOyaCheck( &p_wk->data );

	// ���[�N������
	p_wk->data.new_con = GFL_NET_NETID_INVALID;

	// �ʐM�R�}���h�ݒ�
	CommCommandWFP2PMFInitialize( p_wk );

	// �ؒf�`�F�b�N�J�n
	WFP2PMF_LogOutCheckStart( p_wk );


	// �`��f�[�^������
	WFP2PMF_GraphicInit( p_wk, p_init, HEAPID_WIFI_FOURMATCH );

	// VBlank�֐��Z�b�g
	//  sys_VBlankFuncChange( VBlankFunc, p_wk );

	GFL_NET_ReloadIcon();  // �ڑ����Ȃ̂ŃA�C�R���\��

	// �G���[�`�F�b�N�@�J�n
	p_wk->data.err_check = TRUE;


	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F���C�����[�v
 *
 * @param	proc	�v���Z�X�f�[�^
 * @param	seq		�V�[�P���X
 *
 * @return	������
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WifiP2PMatchFourProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	WFP2PMF_WK* p_wk = mywk;
	WFP2PMF_INIT* p_init = pwk;
	BOOL result;

	// ���C������
	switch( *seq ){
	case WFP2PMF_MAIN_WIPE_S:
		// ���C�v�t�F�[�h�J�n
		//        WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
		//                        COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFI_FOURMATCH);
		GFL_FADE_SetMasterBrightReq(
			GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
			16, 0, 0);


		(*seq)++;
		break;

	case WFP2PMF_MAIN_WIPE_E:
		if( GFL_FADE_CheckFade() ){
			(*seq)++;
		}
		break;

	case WFP2PMF_MAIN_MATCHWAIT:

		// �G���[�`�F�b�N
		if( p_wk->data.err_check ){

			WFP2PMF_ErrCheck( p_wk, p_init, HEAPID_WIFI_FOURMATCH );
		}

		if( p_wk->data.oya ){

#if 0 ///COMMINFO���l������@@OO
			// CommInfo�����邩���`�F�b�N
			if( CommInfoIsInitialize() == TRUE ){

				// �݂�Ȃ�VCHAT�t���O���`�F�b�N���đ��M
				WFP2PMF_VchatCommSend( p_wk );

				// �V�KPOKEDATA����M������݂�Ȃɑ��M
				CommInfoSendArray_ServerSide();	// �݂�Ȃ���������POKEDATA�𑗐M

				// last_netID��ENTRY�ɂȂ�O�ɂ�����傫��NETID�̐l��NEWENTRY��
				// �Ȃ�����Đڑ����Ă��炤
				WFP2PMF_NewConLockNewCon( p_wk );
			}
#endif

			// �e�̓���
			result = pOyaFunc[ p_wk->data.status ]( p_wk, p_init, HEAPID_WIFI_FOURMATCH );
		}else{


			result = pKoFunc[ p_wk->data.status ]( p_wk, p_init, HEAPID_WIFI_FOURMATCH );

#if 0 ///COMMINFO���l������@@OO
			// CommInfo�����邩���`�F�b�N
			if( CommInfoIsInitialize() == TRUE ){
				//  �����ȊO�̐l�̃G���g���[����
				WFP2PMF_KoRecvEntry( p_wk, HEAPID_WIFI_FOURMATCH );
			}
#endif
		}

		// ���X�g�̕`��`�F�b�N
		if( WFP2PMF_ConListWriteCheck( p_wk ) ){
			// ���X�g�̕`��
			WFP2PMF_GraphicConlistStrPutAll( p_wk, p_init, HEAPID_WIFI_FOURMATCH );
		}

		// OAM���C������
		WFP2PMF_DrawOamMain( p_wk );

		// �I��
		if( result == TRUE ){

			// �}�b�`���O���ʊi�[
			p_init->result = p_wk->data.match_result;
			p_init->vchat = p_wk->data.vchat_flag;

			(*seq)++;
		}
		break;

	case WFP2PMF_MAIN_EWIPE_S:
		// ���C�v�t�F�[�h�J�n
		//		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
		//						COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFI_FOURMATCH);
		GFL_FADE_SetMasterBrightReq(
			GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
			0, 16, 0);

		(*seq)++;
		break;
	case WFP2PMF_MAIN_EWIPE_E:
		if( GFL_FADE_CheckFade() ){
			return GFL_PROC_RES_FINISH;
		}
		break;
	}

	// �p���b�g�A�j��
	WFP2PMF_PlttAnm( &p_wk->draw );


	// OAM�`�揈��
	//	CLACT_Draw( p_wk->draw.clactSet );

	return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I������
 *
 * @param	proc	�v���Z�X�f�[�^
 * @param	seq		�V�[�P���X
 *
 * @return	������
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WifiP2PMatchFourProc_End(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	WFP2PMF_WK* p_wk = mywk;
	WFP2PMF_INIT* p_init = pwk;


	//    sys_VBlankFuncChange( NULL, NULL );	// VBlankReset

	// LOGOUT�`�F�b�N�I��
	WFP2PMF_LogOutCheckEnd( p_wk );

	// OAM�j������
	WFP2PMF_DrawOamDelete( p_wk );

	// VCHAT���Ƃ߂�
	WFP2PMF_StatusVChatOff( p_wk );

	// �`��f�[�^�j��
	WFP2PMF_GraphicDelete( p_wk, HEAPID_WIFI_FOURMATCH );

#if WB_TEMP_FIX
	// VRAMTRMAN
	DellVramTransferManager();
#endif

	// �v���Z�X���[�N�j��
	GFL_PROC_FreeWork( proc );				// PROC���[�N�J��

	GFL_HEAP_DeleteHeap( HEAPID_WIFI_FOURMATCHVCHAT );
	GFL_HEAP_DeleteHeap( HEAPID_WIFI_FOURMATCH );

	// �I�[�o�[���C�j��
	//GFL_OVERLAY_Unload( FS_OVERLAY_ID(wifi2dmap) );  �O�ōs���K�v������

	return GFL_PROC_RES_FINISH;
}



// �ʐM�f�[�^��M
//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�f�[�^��M
 *
 *	@param	p_wk		���[�N
 *	@param	cp_data		�f�[�^
 */
//-----------------------------------------------------------------------------
void WFP2PMF_CommResultRecv( WFP2P_WK* p_wk, const WFP2PMF_COMM_RESULT* cp_data )
{
	NET_PRINT( "recv result [%d]\n", cp_data->flag );

	if( p_wk->data.oya == FALSE ){
		if( cp_data->netID == GFL_NET_SystemGetCurrentID() ){


			switch( cp_data->flag ){
			case WFP2PMF_CON_OK:
				p_wk->data.entry = WFP2PMF_ENTRY_KO;
				break;

			case WFP2PMF_CON_NG:
				p_wk->data.entry = WFP2PMF_ENTRY_NG;
				break;

			case WFP2PMF_CON_LOCK:
				p_wk->data.entry = WFP2PMF_ENTRY_LOCK;
				break;
			}

		}else{

			switch( cp_data->flag ){
			case WFP2PMF_CON_OK:
				p_wk->data.entry_on[ cp_data->netID ] = TRUE;
				break;

			case WFP2PMF_CON_NG:
			case WFP2PMF_CON_LOCK:
#if 0 ///COMMINFO���l������@@OO
				if( CommInfoIsInitialize() == TRUE ){
					CommInfoDeletePlayer( cp_data->netID );
				}
#endif
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���J�n�@�����J�n
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WFP2PMF_CommStartRecv( WFP2P_WK* p_wk )
{
	// 1�񂵂�������Ȃ��悤��
	if( p_wk->data.oya_start == FALSE ){
		p_wk->data.oya_start = TRUE;
		// �����J�n
		//		CommTimingSyncStart(_TIMING_GAME_CHECK);
		GFL_NET_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_GAME_CHECK);
	}
	OS_TPrintf( "sync start \n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[��VCHAT�f�[�^�@�擾
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WFP2PMF_CommVchatRecv( WFP2P_WK* p_wk, const WFP2PMF_COMM_VCHAT* cp_data )
{
	if( p_wk->data.oya != TRUE ){	// �e����Ȃ��Ƃ�����
		//		memcpy( p_wk->data.vchat_now, cp_data->vchat_now, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );
		GFL_STD_MemCopy( cp_data->vchat_now, p_wk->data.vchat_now, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );
	}
}



//-----------------------------------------------------------------------------
/**
 *		�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
// �@VBLANK
static void VBlankFunc( void * work )
{
	WFP2PMF_WK* p_wk = work;

	// BG��������
	//	GF_BGL_VBlankFunc( p_wk->draw.p_bgl );

	// Vram�]���}�l�[�W���[���s
	//    DoVramTransferManager();

	// �����_�����LOAM�}�l�[�W��Vram�]��
	//    REND_OAMTrans();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����̃f�[�^���e���q���`�F�b�N
 *
 *	@param	cp_data			���[�N
 *
 *	@retval	TRUE	�e
 *	@retval	FALSE	�q
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_MyDataOyaCheck( const WFP2PMF_DATA* cp_data )
{
	if( GFL_NET_IsParentMachine() ){
		// �ҋ@�Ȃ�e
		return TRUE;
	}
	// ����ȊO�͎q
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�f�[�^������
 *
 *	@param	p_wk		���[�N
 *	@param	cp_init		�������f�[�^
 *	@param	heapID		�q�[�v
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicInit( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID )
{
	// �o���N�ݒ�
	GFL_DISP_SetBank( &p2pmatchfourVramTbl );

	// BGL�쐬
	WFP2PMF_GraphicBGLInit( &p_wk->draw, heapID );

	// �r�b�g�}�b�v�f�[�^�쐬
	WFP2PMF_GraphicBmpInit( &p_wk->draw, cp_init, heapID );

	// ���b�Z�[�W�f�[�^������
	WFP2PMF_GraphicMsgInit( &p_wk->draw, cp_init, heapID );

	// �r�b�g�}�b�v�Ƀ��b�Z�[�W����������
	WFP2PMF_GraphicBmpMsgInit( &p_wk->draw, cp_init, heapID );

	// ���X�g
	// �����̖��O��ID��ݒ肷��
	WFP2PMF_GraphicConlistStrPutAll( p_wk, cp_init, heapID );

	// �Z���A�N�^�[
	WFP2PMF_GraphicClactInit( &p_wk->draw, heapID );

}

//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�f�[�^�j��
 *
 *	@param	p_wk	���[�N
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicDelete( WFP2PMF_WK* p_wk, u32 heapID )
{

	// �r�b�g�}�b�v�E�C���h�E
	GFL_BMPWIN_Exit();
	// �Z���A�N�^�[
	WFP2PMF_GraphicClactDelete( &p_wk->draw );

	// ���b�Z�[�W�f�[�^�j��
	WFP2PMF_GraphicMsgDelete( &p_wk->draw );

	// �r�b�g�}�b�v�f�[�^�j��
	WFP2PMF_GraphicBmpDelete( &p_wk->draw, heapID );

	// BGL�j��
	WFP2PMF_GraphicBGLDelete( &p_wk->draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGL������
 *
 *	@param	p_draw		���[�N
 *	@param	heapID		�q�[�v
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBGLInit( WFP2PMF_DRAW* p_draw, u32 heapID )
{
	ARCHANDLE* p_handle;

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
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000,
			0x10000,
			GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
			};
		GFL_BG_SetBGControl(GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
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
		GFL_BG_SetBGControl(GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearFrame( GFL_BG_FRAME1_M );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );
	}

	// YesNo���X�g
	{
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000,
			0x8000,
			GX_BG_EXTPLTT_23,
			0, 0, 0, FALSE
			};
		GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearFrame( GFL_BG_FRAME2_M );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
	}


	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG3, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 |
		GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ, VISIBLE_OFF );
	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 |
		GX_PLANEMASK_OBJ, VISIBLE_ON );


	p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, heapID );

	// VCHAT�A�C�R���p�L�����N�^�̓Ǎ���
	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCLR,
																		PALTYPE_MAIN_BG,  WFP2PMF_VCHAT_ICON_PAL*32,
																		WFP2PMF_VCHAT_ICON_PALNUM*32, heapID );
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCGR,
																				WFP2PMF_VCHAT_ICON_FRM_NUM, WFP2PMF_VCHAT_ICON_CG*32, WFP2PMF_VCHAT_ICON_CGSIZ*32, FALSE, heapID );
	p_draw->p_vchatscrnbuf = GFL_ARCHDL_UTIL_LoadScreen( p_handle,
																											 NARC_wifip2pmatch_wf_match_all_icon_NSCR, FALSE,
																											 &p_draw->p_vchatscrn, heapID );

	// �w�i�ǂݍ���
	{
		p_draw->p_plttbuff = GFL_ARCHDL_UTIL_LoadPalette( p_handle, NARC_wifip2pmatch_wifi_check_bg_NCLR,
																											&p_draw->p_pltt, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_wifi_check_bg_NCGR,
																					GFL_BG_FRAME0_M, 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_wifip2pmatch_wifi_check_bg_NSCR,
																		 GFL_BG_FRAME0_M, 0, 0, FALSE, heapID );

		// �p���b�g�A�j��������
		// �Ƃ肠�����A�O�t���[���ڂ𑗐M
		WFP2PMF_PlttAnm_Trans( p_draw, 0 );
		p_draw->pltt_count	= WFP2PMF_BACK_PALANM;
		p_draw->pltt_idx	= 1;
	}

	GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGL�j��
 *
 *	@param	p_draw	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBGLDelete( WFP2PMF_DRAW* p_draw )
{
	GFL_HEAP_FreeMemory( p_draw->p_vchatscrnbuf );
	GFL_HEAP_FreeMemory( p_draw->p_plttbuff );

	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

}

//----------------------------------------------------------------------------
/**
 *	@brief	�r�b�g�}�b�v������
 *
 *	@param	p_draw		���[�N
 *	@param	cp_init		�������f�[�^
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBmpInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID )
{
	int wintype;

	GFL_BMPWIN_Init(heapID);
	// �p���b�g�A�L�����N�^�f�[�^�]��
	//    TalkFontPaletteLoad( PALTYPE_MAIN_BG, WFP2PMF_TALK_PAL*0x20, heapID );
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , 13*0x20, 16*2, heapID );

	wintype = cp_init->wintype;

	//    TalkWinGraphicSet(
	BmpWinFrame_GraphicSet(
		GFL_BG_FRAME1_M, WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL,  wintype, heapID );
	//	MenuWinGraphicSet(
	BmpWinFrame_GraphicSet(
		GFL_BG_FRAME1_M, WFP2PMF_BG1_MENUWIN_CGX, WFP2PMF_BG1_MENUWIN_PAL, 0, heapID );
	//	MenuWinGraphicSet(
	BmpWinFrame_GraphicSet(
		GFL_BG_FRAME2_M, WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL, 0, heapID );

	// �r�b�g�}�b�v�쐬
	// ���b�Z�[�W�E�B���h�E
	p_draw->msgwin = GFL_BMPWIN_Create(GFL_BG_FRAME1_M,
																		 WFP2PMF_MSGWIN_X, WFP2PMF_MSGWIN_Y,
																		 WFP2PMF_MSGWIN_SIZX, WFP2PMF_MSGWIN_SIZY,
																		 WFP2PMF_MSGWIN_PAL, GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->msgwin), 15 );
	GFL_BMPWIN_MakeFrameScreen( p_draw->msgwin,
															WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL );


	// �^�C�g���E�B���h�E
	p_draw->titlewin = GFL_BMPWIN_Create(GFL_BG_FRAME1_M,
																			 WFP2PMF_TITLEWIN_X, WFP2PMF_TITLEWIN_Y,
																			 WFP2PMF_TITLEWIN_SIZX, WFP2PMF_TITLEWIN_SIZY,
																			 WFP2PMF_TITLEWIN_PAL, GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->titlewin), 15 );
	GFL_BMPWIN_MakeFrameScreen( p_draw->titlewin,
															WFP2PMF_BG1_MENUWIN_CGX, WFP2PMF_BG1_MENUWIN_PAL );


	// �{�C�X�`���b�g�E�B���h�E
	p_draw->vchatwin = GFL_BMPWIN_Create(GFL_BG_FRAME1_M,
																			 WFP2PMF_VCHATWIN_X, WFP2PMF_VCHATWIN_Y,
																			 WFP2PMF_VCHATWIN_SIZX, WFP2PMF_VCHATWIN_SIZY,
																			 WFP2PMF_VCHATWIN_PAL, GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->vchatwin), 0 );

	// CONNECT���X�g
	p_draw->conlistwin = GFL_BMPWIN_Create(GFL_BG_FRAME1_M,
																				 WFP2PMF_CONLISTWIN_X, WFP2PMF_CONLISTWIN_Y,
																				 WFP2PMF_CONLISTWIN_SIZX, WFP2PMF_CONLISTWIN_SIZY,
																				 WFP2PMF_CONLISTWIN_PAL, GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->conlistwin), 15 );
	GFL_BMPWIN_MakeFrameScreen( p_draw->conlistwin,
															WFP2PMF_BG1_MENUWIN_CGX, WFP2PMF_BG1_MENUWIN_PAL );

	// NEWCON�E�B���h�E
	p_draw->newconwin = GFL_BMPWIN_Create(GFL_BG_FRAME1_M,
																				WFP2PMF_NEWCONWIN_X, WFP2PMF_NEWCONWIN_Y,
																				WFP2PMF_NEWCONWIN_SIZX, WFP2PMF_NEWCONWIN_SIZY,
																				WFP2PMF_NEWCONWIN_PAL, GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->newconwin), 15 );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�r�b�g�}�b�v�j��
 *
 *	@param	p_draw	���[�N
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBmpDelete( WFP2PMF_DRAW* p_draw, u32 heapID )
{
	// ���b�Z�[�W������
	WFP2PMF_GraphicMsgBmpStrOff( p_draw );

	// �r�b�g�}�b�v�̔j��
	// ���b�Z�[�W
	BmpWinFrame_Clear( p_draw->msgwin, WINDOW_TRANS_ON );
	//BmpTalkWinClear( &p_draw->msgwin, WINDOW_TRANS_ON );
	//	GF_BGL_BmpWinDel( &p_draw->msgwin );
	GFL_BMPWIN_Delete( p_draw->msgwin );

	// �^�C�g��
	//	BmpMenuWinClear( &p_draw->titlewin, WINDOW_TRANS_ON );
	BmpWinFrame_Clear( p_draw->titlewin, WINDOW_TRANS_ON );
	//	GF_BGL_BmpWinDel( &p_draw->titlewin );
	GFL_BMPWIN_Delete( p_draw->titlewin );

	// �{�C�X�`���b�g
	//	GF_BGL_BmpWinDel( &p_draw->vchatwin );
	GFL_BMPWIN_Delete( p_draw->vchatwin );

	// CONNECT���X�g
	//	BmpMenuWinClear( &p_draw->conlistwin, WINDOW_TRANS_ON );
	//	GF_BGL_BmpWinDel( &p_draw->conlistwin );
	BmpWinFrame_Clear( p_draw->conlistwin, WINDOW_TRANS_ON );
	GFL_BMPWIN_Delete( p_draw->conlistwin );

	// �V�R�l�N�g���X�g
	//	BmpMenuWinClear( &p_draw->newconwin, WINDOW_TRANS_ON );
	//	GF_BGL_BmpWinDel( &p_draw->newconwin );
	BmpWinFrame_Clear( p_draw->newconwin, WINDOW_TRANS_ON );
	GFL_BMPWIN_Delete( p_draw->newconwin );

	// YesNo�̃��[�N����������j��
	if( p_draw->p_yesnowin != NULL ){
		BmpMenu_YesNoMenuExit( p_draw->p_yesnowin );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�f�[�^������
 *
 *	@param	p_draw		���[�N
 *	@param	cp_init		�������f�[�^
 *	@param	heapID		�q�[�v
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID )
{
	p_draw->p_wordset  = WORDSET_Create( heapID );
	p_draw->p_msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_lobby_dat, heapID );
	p_draw->p_msgstr = GFL_STR_CreateBuffer( WFP2PMF_MSGDATA_STRNUM, heapID );
	p_draw->p_msgtmp = GFL_STR_CreateBuffer( WFP2PMF_MSGDATA_STRNUM, heapID );
	//	p_draw->msg_no = WFP2PMF_MSGNO_NONE;
	//	p_draw->msg_speed = CONFIG_GetMsgPrintSpeed(SaveData_GetConfig(cp_init->p_savedata));



	p_draw->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , heapID );
	p_draw->printQue = PRINTSYS_QUE_Create( heapID );
	PRINT_UTIL_Setup( &p_draw->msgwinPrintUtil , p_draw->msgwin );
	PRINT_UTIL_Setup( &p_draw->titlewinPrintUtil , p_draw->titlewin );
	PRINT_UTIL_Setup( &p_draw->vchatwinPrintUtil , p_draw->vchatwin );
	PRINT_UTIL_Setup( &p_draw->conlistwinPrintUtil , p_draw->conlistwin );
	PRINT_UTIL_Setup( &p_draw->newconwinPrintUtil , p_draw->newconwin );

	p_draw->p_msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
																		 NARC_message_wifi_lobby_dat, heapID );

}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�f�[�^�j��
 *
 *	@param	p_draw		���[�N
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgDelete( WFP2PMF_DRAW* p_draw )
{
	//	STRBUF_Delete( p_draw->p_msgtmp );
	//	STRBUF_Delete( p_draw->p_msgstr );
	//    MSGMAN_Delete( p_draw->p_msgman );
	WORDSET_Delete( p_draw->p_wordset );

	GFL_MSG_Delete( p_draw->p_msgman );


	GFL_STR_DeleteBuffer( p_draw->p_msgtmp );
	GFL_STR_DeleteBuffer( p_draw->p_msgstr );

	PRINTSYS_QUE_Clear(p_draw->printQue);
	PRINTSYS_QUE_Delete(p_draw->printQue);
	GFL_FONT_Delete(p_draw->fontHandle);

}

//----------------------------------------------------------------------------
/**
 *	@brief	�r�b�g�}�b�v���b�Z�[�W������
 *
 *	@param	p_draw		���[�N
 *	@param	cp_ini		�������f�[�^t
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBmpMsgInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID )
{
#if 0 //WiFi�L��֘A�͍폜
	// �^�C�g��
	switch( cp_init->type ){
	case WFP2PMF_TYPE_BUCKET:			// ���܂���
		WORDSET_RegisterWiFiLobbyGameName( p_draw->p_wordset, 0, WFLBY_GAME_BALLSLOW );
		break;

	case WFP2PMF_TYPE_BALANCE_BALL:		// ���܂̂�
		WORDSET_RegisterWiFiLobbyGameName( p_draw->p_wordset, 0, WFLBY_GAME_BALANCEBALL );
		break;

	case WFP2PMF_TYPE_BALLOON:			// �ӂ�������
		WORDSET_RegisterWiFiLobbyGameName( p_draw->p_wordset, 0, WFLBY_GAME_BALLOON );
		break;
	}
#endif
	//MSGMAN_GetString(  p_draw->p_msgman, WFP2PMF_TITLE[ cp_init->type ], p_draw->p_msgtmp );
	GFL_MSG_GetString(  p_draw->p_msgman, WFP2PMF_TITLE[ cp_init->type ], p_draw->p_msgtmp );

	WORDSET_ExpandStr(  p_draw->p_wordset, p_draw->p_msgstr, p_draw->p_msgtmp );
	//GF_STR_PrintColor( &p_draw->titlewin, FONT_SYSTEM, p_draw->p_msgstr, 0, 0, MSG_NO_PUT, _COL_N_BLACK, NULL);
	PRINT_UTIL_Print(&p_draw->titlewinPrintUtil , p_draw->printQue , 0, 0, p_draw->p_msgstr, p_draw->fontHandle);

	//	GF_BGL_BmpWinOnVReq( &p_draw->titlewin );
	GFL_BMPWIN_TransVramCharacter( p_draw->titlewin );

	// ���X�g
	//	GF_BGL_BmpWinOnVReq( &p_draw->conlistwin );
	GFL_BMPWIN_TransVramCharacter( p_draw->conlistwin );


	// VCHT
	//    MSGMAN_GetString(  p_draw->p_msgman, msg_wifilobby_132, p_draw->p_msgstr );
	GFL_MSG_GetString(  p_draw->p_msgman, msg_wifilobby_132, p_draw->p_msgstr );
	//    GF_STR_PrintColor( &p_draw->vchatwin, FONT_SYSTEM, p_draw->p_msgstr, 0, 0, MSG_NO_PUT, _COL_N_BLACK_C, NULL);
	PRINT_UTIL_Print(&p_draw->vchatwinPrintUtil , p_draw->printQue , 0, 0, p_draw->p_msgstr, p_draw->fontHandle);


	//GF_BGL_BmpWinOnVReq( &p_draw->vchatwin );
	GFL_BMPWIN_TransVramCharacter( p_draw->vchatwin );

	// ���b�Z�[�W
	//GF_BGL_BmpWinOnVReq( &p_draw->msgwin );
	GFL_BMPWIN_TransVramCharacter( p_draw->msgwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[������
 *
 *	@param	p_draw		���[�N
 *	@param	heapID		�q�[�v
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicClactInit( WFP2PMF_DRAW* p_draw, u32 heapID )
{
	int i;

	// OAM�}�l�[�W���[�̏�����
	NNS_G2dInitOamManagerModule();
	GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN , &p2pmatchfourVramTbl, heapID );


	// �L�����N�^�}�l�[�W���[������
	{
		//        CHAR_MANAGER_MAKE cm = {
		//  WIFI_P2PMATCH_LOADRESNUM,
		//    WIFI_P2PMATCH_VRAMMAINSIZ,
		//      WIFI_P2PMATCH_VRAMSUBSIZ,
		//        0
		//      };
		//		cm.heap = heapID;
		//        InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_32K );
	}
	// �p���b�g�}�l�[�W���[������
	//  InitPlttManager(WIFI_P2PMATCH_LOADRESNUM, heapID);

	// �ǂݍ��݊J�n�ʒu��������
	//CharLoadStartAll();
	//PlttLoadStartAll();

	//�ʐM�A�C�R���p�ɃL�������p���b�g����
	//	CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
	//	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);


	// �Z���A�N�^�[�Z�b�g�쐬
	p_draw->clactSet = GFL_CLACT_UNIT_Create( WIFI_P2PMATCH_CLACTWK_NUM, 0, heapID );
	//p_draw->clactSet = CLACT_U_SetEasyInit( WIFI_P2PMATCH_CLACTWK_NUM, &p_draw->renddata, heapID );
	// �L�����ƃp���b�g�̃��\�[�X�}�l�[�W���쐬
	//	for( i=0; i<WIFI_P2PMATCH_RESNUM; i++ ){
	//		p_draw->resMan[i] = CLACT_U_ResManagerInit(WIFI_P2PMATCH_LOADRESNUM, i, heapID);
	//	}


	// �I�u�W�F�N�g�V�X�e��������
	p_draw->p_objsys = WF2DMAP_OBJSysInit( WIFI_P2PMATCH_CLACTWK_NUM, heapID );
	p_draw->p_objdraw = WF2DMAP_OBJDrawSysInit( p_draw->clactSet, NULL, WIFI_P2PMATCH_CLACTWK_NUM, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );

}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�j��
 *
 *	@param	p_draw		���[�N
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicClactDelete( WFP2PMF_DRAW* p_draw )
{
	int i;

	// �I�u�W�F�N�g���[�N�j��
	WF2DMAP_OBJDrawSysExit( p_draw->p_objdraw );
	WF2DMAP_OBJSysExit( p_draw->p_objsys );

	// �A�N�^�[�̔j��
	//	CLACT_DestSet( p_draw->clactSet );
	GFL_CLACT_UNIT_Delete( p_draw->clactSet );

	//	for( i=0; i<WIFI_P2PMATCH_RESNUM; i++ ){
	//		CLACT_U_ResManagerDelete( p_draw->resMan[i] );
	//	}

	// ���\�[�X���
	//DeleteCharManager();
	//DeletePlttManager();

	//OAM�����_���[�j��
	GFL_CLACT_SYS_Delete();
}


//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�f�[�^�\��
 *
 *	@param	p_draw		���[�N
 *	@param	msg_idx		���b�Z�[�WIDX
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgBmpStrPut( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u32 heapID )
{
	WFP2PMF_GraphicMsgBmpStrPutEx( p_draw, cp_init, msg_idx, 0, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\��	���b�Z�[�W�I�����WAIT�ݒ�o�[�W����
 *
 *	@param	p_draw		���[�N
 *	@param	msg_idx		���b�Z�[�WIDX
 *	@param	wait		�҂�����
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgBmpStrPutEx( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u8 wait, u32 heapID )
{
	STRBUF* p_expand;
	int wintype = cp_init->wintype;;

	// �P�O�̕����I����Ă��Ȃ�������Ƃ߂�
	//    if( p_draw->msg_no != WFP2PMF_MSGNO_NONE){
	//        if(GF_MSG_PrintEndCheck( p_draw->msg_no )!=0){
	//			NET_PRINT( "msg stop\n" );
	//            GF_STR_PrintForceStop( p_draw->msg_no );
	//			p_draw->msg_no = WFP2PMF_MSGNO_NONE;
	//        }
	//    }
	if(PRINTSYS_QUE_IsFinished(p_draw->printQue)){
		PRINTSYS_QUE_Clear( p_draw->printQue );
	}

	// ��������
	//    GF_BGL_BmpWinDataFill( &p_draw->msgwin, 15 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->msgwin), 15 );
	p_expand = GFL_STR_CreateBuffer( WFP2PMF_MSGDATA_STRNUM, heapID );
	//	p_expand = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	//	MSGMAN_GetString(  p_draw->p_msgman, msg_idx, p_expand );
	GFL_MSG_GetString(  p_draw->p_msgman, msg_idx, p_expand );
	WORDSET_ExpandStr( p_draw->p_wordset, p_draw->p_msgstr, p_expand );
	//    p_draw->msg_no = GF_STR_PrintColor( &p_draw->msgwin, FONT_TALK, p_draw->p_msgstr, 0, 0, p_draw->msg_speed, _COL_N_BLACK, NULL);
	PRINT_UTIL_Print(&p_draw->msgwinPrintUtil , p_draw->printQue , 0, 0, p_draw->p_msgstr, p_draw->fontHandle);

	//    GF_BGL_BmpWinOnVReq( &p_draw->msgwin );
	GFL_BMPWIN_TransVramCharacter( p_draw->msgwin );
	GFL_STR_DeleteBuffer( p_expand );


	// �E�B���h�E�f�[�^�ē]��
	//	TalkWinGraphicSet(
	BmpWinFrame_GraphicSet(
		GFL_BG_FRAME1_M, WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL,  wintype, heapID );

	p_draw->msg_wait = wait;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W����C�ɕ\��
 *
 *	@param	p_draw		���[�N
 *	@param	msg_idx		���b�Z�[�WIDX
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgBmpStrPutAll( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u32 heapID )
{
	int wintype;

	// �P�O�̕����I����Ă��Ȃ�������Ƃ߂�
	if(PRINTSYS_QUE_IsFinished(p_draw->printQue)){
		PRINTSYS_QUE_Clear( p_draw->printQue );
	}

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->msgwin), 15 );
	GFL_MSG_GetString(  p_draw->p_msgman, msg_idx, p_draw->p_msgstr );
	PRINT_UTIL_Print(&p_draw->msgwinPrintUtil , p_draw->printQue , 0, 0, p_draw->p_msgstr, p_draw->fontHandle);

	GFL_BMPWIN_TransVramCharacter( p_draw->msgwin );

	// �E�B���h�E�f�[�^�ē]��
	wintype = cp_init->wintype;
	//	TalkWinGraphicSet(
	BmpWinFrame_GraphicSet(
		GFL_BG_FRAME1_M, WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL,  wintype, heapID );

	p_draw->msg_wait = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\�������`�F�b�N
 *
 *	@param	cp_draw		���[�N
 *
 *	@retval	TRUE	�I��
 *	@retval	FALSE	�܂�������ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_GraphicMsgBmpStrMain( WFP2PMF_DRAW* p_draw )
{
	if(PRINTSYS_QUE_IsFinished(p_draw->printQue)){
		if( p_draw->msg_wait > 0 ){
			p_draw->msg_wait --;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\��OFF����
 *
 *	@param	p_draw	���[�N
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgBmpStrOff( WFP2PMF_DRAW* p_draw )
{
	// �P�O�̕����I����Ă��Ȃ�������Ƃ߂�
	if(PRINTSYS_QUE_IsFinished(p_draw->printQue)){
		PRINTSYS_QUE_Clear( p_draw->printQue );
	}


	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->msgwin), 15 );
	GFL_BMPWIN_TransVramCharacter( p_draw->msgwin );

	p_draw->msg_wait = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�K�R�l�N�g�v���C���[�̕\��
 *
 *	@param	p_draw		���[�N
 *	@param	cp_init		�������f�[�^
 *	@param	netid	�F�B�i���o�[
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicNewConStrPut( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID )
{
	STRBUF* p_expand;
	STRBUF* p_setstr;

	p_expand = GFL_STR_CreateBuffer( WFP2PMF_MSGDATA_STRNUM, heapID );
	p_setstr = GFL_STR_CreateBuffer( WFP2PMF_MSGDATA_STRNUM, heapID );

	WFP2PMF_WordSetPlayerNameSet( p_draw, cp_init, netid, 0, heapID );
	WFP2PMF_WordSetPlayerIDSet( p_draw, cp_init, netid, 1, heapID );

	GFL_MSG_GetString(  p_draw->p_msgman, msg_wifilobby_135, p_expand );
	WORDSET_ExpandStr( p_draw->p_wordset, p_setstr, p_expand );

	// ��������
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_draw->newconwin), 15 );

	PRINT_UTIL_Print(&p_draw->newconwinPrintUtil , p_draw->printQue , 0, 0, p_setstr, p_draw->fontHandle);

	GFL_BMPWIN_MakeFrameScreen( p_draw->newconwin,
															WFP2PMF_BG1_MENUWIN_CGX, WFP2PMF_BG1_MENUWIN_PAL );
	GFL_BMPWIN_TransVramCharacter( p_draw->newconwin );

	GFL_STR_DeleteBuffer( p_expand );
	GFL_STR_DeleteBuffer( p_setstr );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�K�R�l�N�g�\��OFF
 *
 *	@param	p_draw	�`�惏�[�N
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicNewConStrOff( WFP2PMF_DRAW* p_draw )
{
	// �V�R�l�N�g���X�g
	//BmpMenuWinClear( &p_draw->newconwin, WINDOW_TRANS_OFF );
	//GF_BGL_BmpWinOffVReq( &p_draw->newconwin );
	BmpWinFrame_Clear( p_draw->newconwin, WINDOW_TRANS_OFF );
	GFL_BMPWIN_TransVramCharacter( p_draw->newconwin );

}

//----------------------------------------------------------------------------
/**
 *	@brief	���X�g�̑S�`��
 *
 *	@param	p_wk		���[�N
 *	@param	cp_init		�f�[�^
 *	@param	heapID		�q�[�v
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicConlistStrPutAll( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID )
{
	int i;
	int connum = WIFI_P2PMATCH_NUM_MAX;


	// COMMINFO�@�������Â݃`�F�b�N
#if 0 ///COMMINFO���l������@@OO
	if( CommInfoIsInitialize() == FALSE ){
		return ;
	}

	// �G���g���[�l�����\��
	for( i=0; i<connum; i++ ){

		if( (i == GFL_NET_NO_PARENTMACHINE) && (CommInfoIsNewName(i) == TRUE) ){	// �e�͊m����
			WFP2PMF_GraphicConlistStrPut( p_wk, &p_wk->draw, cp_init, i, heapID );
		}else{
			if( (CommInfoGetEntry(i) == TRUE) ){	// ���̑���ENTRY��ԂȂ�\��
				WFP2PMF_GraphicConlistStrPut( p_wk, &p_wk->draw, cp_init, i, heapID );
			}else{
				// ����
				WFP2PMF_GraphicConlistStrOff( &p_wk->draw, cp_init, i, heapID );
			}
		}
	}
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�R�l�N�g���X�g�@���b�Z�[�W�\��
 *
 *	@param	p_draw		���[�N
 *	@param	cp_init		�f�[�^
 *	@param	netid	�F�B�i���o�[
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicConlistStrPut( WFP2PMF_WK* p_wk, WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID )
{
	STRBUF* p_expand;
	STRBUF* p_setstr;
	u8 vchat;
	u8 friendno;

	//	p_expand = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	//	p_setstr = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	p_expand = GFL_STR_CreateBuffer( WFP2PMF_MSGDATA_STRNUM, heapID );
	p_setstr = GFL_STR_CreateBuffer( WFP2PMF_MSGDATA_STRNUM, heapID );

	WFP2PMF_WordSetPlayerNameSet( p_draw, cp_init, netid, 0, heapID );
	//    MSGMAN_GetString(  p_draw->p_msgman, msg_wifilobby_133, p_expand );
	GFL_MSG_GetString(p_draw->p_msgman, msg_wifilobby_133, p_expand);
	WORDSET_ExpandStr( p_draw->p_wordset, p_setstr, p_expand );

	// ��������
	//    GF_BGL_BmpWinFill( &p_draw->conlistwin, 15,
	//			0, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
	//			(WFP2PMF_CONLISTWIN_SIZX*8), (WFP2PMF_CONLISTWIN_ONELIST_Y*8) );
	GFL_BMP_Fill(GFL_BMPWIN_GetBmp(p_draw->conlistwin),
							 0, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
							 (WFP2PMF_CONLISTWIN_SIZX*8), (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
							 FBMP_COL_WHITE);


	//    GF_STR_PrintColor( &p_draw->conlistwin, FONT_SYSTEM, p_setstr,
	//			0, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
	//			MSG_NO_PUT, _COL_N_BLACK, NULL);
	PRINT_UTIL_Print(&p_draw->conlistwinPrintUtil , p_draw->printQue , 0,
									 netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8), p_setstr, p_draw->fontHandle);

	WFP2PMF_WordSetPlayerIDSet( p_draw, cp_init, netid, 0, heapID );
	//    MSGMAN_GetString(  p_draw->p_msgman, msg_wifilobby_134, p_expand );
	GFL_MSG_GetString(  p_draw->p_msgman, msg_wifilobby_134, p_expand );
	WORDSET_ExpandStr( p_draw->p_wordset, p_setstr, p_expand );

	//    GF_STR_PrintColor( &p_draw->conlistwin, FONT_SYSTEM, p_setstr,
	//			WFP2PMF_CONLISTWIN_ID_X, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
	//			MSG_NO_PUT, _COL_N_BLACK, NULL);

	PRINT_UTIL_Print(&p_draw->conlistwinPrintUtil , p_draw->printQue ,
									 WFP2PMF_CONLISTWIN_ID_X, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8), p_setstr,
									 p_draw->fontHandle);


	//    GF_BGL_BmpWinOnVReq( &p_draw->conlistwin );
	GFL_BMPWIN_TransVramCharacter( p_draw->conlistwin );

	// VCHAT�f�[�^�`��
	if( netid == GFL_NET_SystemGetCurrentID() ){
		if( p_wk->data.p_match->myMatchStatus.vchat ){
			vchat = WFP2PMF_VCHATICON_ON;
		}else{
			vchat = WFP2PMF_VCHATICON_OFF;
		}
		friendno = 0;
	}else{
		if( p_wk->data.vchat_now[netid] ){
			vchat = WFP2PMF_VCHATICON_ON;
		}else{
			vchat = WFP2PMF_VCHATICON_OFF;
		}
	}
	// �F�B�i���o���A���Ă��Ă��Ȃ��Ȃ�`�悵�Ȃ�
	//	GF_BGL_ScrWriteExpand( p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM,
	//			WFP2PMF_VCHAT_ICON_WX,
	//			WFP2PMF_VCHAT_ICON_WY+(netid*WFP2PMF_CONLISTWIN_ONELIST_Y),
	//			WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_SIZ,
	//			p_draw->p_vchatscrn->rawData,
	//			vchat*WFP2PMF_VCHAT_ICON_SIZ, 0,
	//			p_draw->p_vchatscrn->screenWidth/8,
	//			p_draw->p_vchatscrn->screenHeight/8 );
	GFL_BG_WriteScreenExpand( WFP2PMF_VCHAT_ICON_FRM_NUM,
														WFP2PMF_VCHAT_ICON_WX,
														WFP2PMF_VCHAT_ICON_WY+(netid*WFP2PMF_CONLISTWIN_ONELIST_Y),
														WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_SIZ,
														p_draw->p_vchatscrn->rawData,
														vchat*WFP2PMF_VCHAT_ICON_SIZ, 0,
														p_draw->p_vchatscrn->screenWidth/8,
														p_draw->p_vchatscrn->screenHeight/8 );

	GFL_BG_ChangeScreenPalette( WFP2PMF_VCHAT_ICON_FRM_NUM,
															WFP2PMF_VCHAT_ICON_WX,
															WFP2PMF_VCHAT_ICON_WY+(netid*WFP2PMF_CONLISTWIN_ONELIST_Y),
															WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_PAL );
	//GF_BGL_LoadScreenV_Req( p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM );
	GFL_BG_LoadScreenReq( WFP2PMF_VCHAT_ICON_FRM_NUM );

	GFL_STR_DeleteBuffer( p_expand );
	GFL_STR_DeleteBuffer( p_setstr );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�R�l�N�g���X�g�\����\��
 *
 *	@param	p_draw		���[�N
 *	@param	cp_init		�f�[�^
 *	@param	netid	�F�B�i���o�[
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicConlistStrOff( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID )
{
	// ��������
	//    GF_BGL_BmpWinFill( &p_draw->conlistwin, 15,
	//			0, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
	//			(WFP2PMF_CONLISTWIN_SIZX*8), (WFP2PMF_CONLISTWIN_ONELIST_Y*8) );
	GFL_BMP_Fill(GFL_BMPWIN_GetBmp(p_draw->conlistwin),
							 0, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
							 (WFP2PMF_CONLISTWIN_SIZX*8), (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
							 FBMP_COL_WHITE);

	//	GF_BGL_ScrWriteExpand( p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM,
	//			WFP2PMF_VCHAT_ICON_WX,
	//			WFP2PMF_VCHAT_ICON_WY+netid*WFP2PMF_CONLISTWIN_ONELIST_Y,
	//			WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_SIZ,
	//			p_draw->p_vchatscrn->rawData,
	//			WFP2PMF_VCHATICON_NONE*WFP2PMF_VCHAT_ICON_SIZ, 0,
	//			p_draw->p_vchatscrn->screenWidth/8,
	//			p_draw->p_vchatscrn->screenHeight/8 );

	GFL_BG_WriteScreenExpand(  WFP2PMF_VCHAT_ICON_FRM_NUM,
														 WFP2PMF_VCHAT_ICON_WX,
														 WFP2PMF_VCHAT_ICON_WY+netid*WFP2PMF_CONLISTWIN_ONELIST_Y,
														 WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_SIZ,
														 p_draw->p_vchatscrn->rawData,
														 WFP2PMF_VCHATICON_NONE*WFP2PMF_VCHAT_ICON_SIZ, 0,
														 p_draw->p_vchatscrn->screenWidth/8,
														 p_draw->p_vchatscrn->screenHeight/8 );

	//	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM );
	GFL_BG_LoadScreenReq(WFP2PMF_VCHAT_ICON_FRM_NUM);

	//    GF_BGL_BmpWinOnVReq( &p_draw->conlistwin );
	GFL_BMPWIN_TransVramCharacter( p_draw->conlistwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�h�Z�b�g�Ƀv���C���[����ݒ肷��
 *
 *	@param	p_draw		���[�N
 *	@param	cp_init		�f�[�^
 *	@param	netid	�F�B�ԍ�
 *	@param	set_no		���[�h�Z�b�g�ւ̐ݒ�ꏊ
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_WordSetPlayerNameSet( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, int set_no, u32 heapID )
{
#if 0 ///COMMINFO���l������@@OO
	MYSTATUS* p_target;
	p_target = CommInfoGetMyStatus( netid );
	NET_PRINT( "player netid %d\n", netid );
	WORDSET_RegisterPlayerName( p_draw->p_wordset, set_no, p_target );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�h�Z�b�g�Ƀv���C���[ID��ݒ肷��
 *
 *	@param	p_draw		���[�N
 *	@param	cp_init		�f�[�^
 *	@param	netid		NETID
 *	@param	set_no		���[�h�Z�b�g�ւ̐ݒ�ꏊ
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_WordSetPlayerIDSet( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, int set_no, u32 heapID )
{
#if 0 ///COMMINFO���l������@@OO
	MYSTATUS* p_target;
	u16 id;
	p_target = CommInfoGetMyStatus( netid );
	id = MyStatus_GetID_Low( p_target );
	WORDSET_RegisterNumber( p_draw->p_wordset, set_no, id, 5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_HANKAKU );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�A�j�����C��
 *
 *	@param	p_draw		���[�N
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_PlttAnm( WFP2PMF_DRAW* p_draw )
{
	if( p_draw->pltt_count <= 0 ){
		//  �]��
		WFP2PMF_PlttAnm_Trans( p_draw, p_draw->pltt_idx );

		p_draw->pltt_count	= WFP2PMF_BACK_PALANM;
		p_draw->pltt_idx	= (p_draw->pltt_idx+1) % WFP2PMF_BACK_PALNUM;
	}else{
		p_draw->pltt_count --;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�]��
 *
 *	@param	p_draw		�`�惏�[�N
 *	@param	idx			�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_PlttAnm_Trans( WFP2PMF_DRAW* p_draw, u32 idx )
{
	u16* p_data;
	u32 num;

	num = WFP2PMF_BACK_PAL_IDX[ idx ];

	p_data = p_draw->p_pltt->pRawData;
	//result = AddVramTransferManager( NNS_GFD_DST_2D_BG_PLTT_MAIN, WFP2PMF_BACK_PAL*32,
	//		&p_data[ num*16 ], 32 );
	NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN, WFP2PMF_BACK_PAL*32,
																			&p_data[ num*16 ], 32 );
	//	GF_ASSERT( result );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�C�X�`���b�g�@�X�C�b�`��؂�ւ���
 *
 *	@param	p_wk	���[�N
 *
 *	@retval	TRUE	�X�C�b�`�@ON
 *	@retval	FALSE	�X�C�b�`�@OFF
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_VchatSwitch( WFP2PMF_WK* p_wk, u32 heapID )
{
	p_wk->data.p_match->myMatchStatus.vchat_org = 1 - p_wk->data.p_match->myMatchStatus.vchat_org;
	p_wk->data.p_match->myMatchStatus.vchat = p_wk->data.p_match->myMatchStatus.vchat_org;
	GFL_NET_DWC_SetVchat( p_wk->data.p_match->myMatchStatus.vchat );
	GFL_NET_DWC_SetMyInfo( &(p_wk->data.p_match->myMatchStatus), sizeof(_WIFI_MACH_STATUS) );

	if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){

		WFP2PMF_StatusVChatOnBmpFade( p_wk );

	}else{
		WFP2PMF_StatusVChatOffBmpFade( p_wk );
	}

	return p_wk->data.p_match->myMatchStatus.vchat_org;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�@�G���[�`�F�b�N����
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_ErrCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_param, u32 heapID )
{
	BOOL result;
	BOOL connect_num_errror;
	BOOL timeout;
	//	BOOL parent_status_check;

	// �G���[�ɂ����Ă����甲����
	if( p_wk->data.oya ){
		if( (p_wk->data.status == WFP2PMF_OYA_STATUS_ERR_INIT) ||
				(p_wk->data.status == WFP2PMF_OYA_STATUS_ERR_INIT01) ||
				(p_wk->data.status == WFP2PMF_OYA_STATUS_ERR) ){
			return ;
		}

		// �G���g���[�����P�ȉ��Ń}�b�`���Ă��Ȃ��Ƃ��͉������Ȃ��Ă悢
		// �������A�G���g���[�����Q�ȏ�ɂȂ����牽�炩�̃`�F�b�N���s���K�v������
		// �G���g���[���P�Ƃ����͎̂������G���g���[���Ă����ԂƂ������Ƃł�
		if( GFL_NET_StateGetWifiStatus() <= GFL_NET_STATE_NOTMATCH ){	// �}�b�`�X�e�[�g�I�ɂ��������Ȃ��Ă�����A�G���[�`�F�b�N����
			if( (WFP2PMF_GetEntryNum( p_wk ) <= 1) && (WFP2PMF_MatchCheck( p_wk ) == FALSE) ){
				return ;
			}
		}


		/*WIFIP2PModeCheck�������̂ł���Ȃ�	tomoya
		// status�`�F�b�N�ؒf����
//		result = WFP2PMF_Oya_CheckConnectPlayer( p_wk, p_param );
		// status���s���ȂЂƂ�������I����

//		parent_status_check = TRUE;
//		//*/

	}else{
		if( (p_wk->data.status == WFP2PMF_KO_STATUS_ERR_INIT) ||
				(p_wk->data.status == WFP2PMF_KO_STATUS_ERR_INIT01) ||
				(p_wk->data.status == WFP2PMF_KO_STATUS_ERR) ||
				(p_wk->data.status == WFP2PMF_KO_STATUS_ENTRY_NG) ||
				(p_wk->data.status == WFP2PMF_KO_STATUS_ENTRY_NGINIT) ){
			return ;
		}

	}

	// �^�C���A�E�g�`�F�b�N
	timeout = WFP2PMF_TimeOut_Main( p_wk );

	// �ؒf�`�F�b�N
	result = WFP2PMF_LogOutCheck( p_wk );

	//  �G���g���[�����ʐM�l������������G���[��
	if( WFP2PMF_GetEntryNum( p_wk ) > GFL_NET_GetConnectNum() ){
		connect_num_errror = TRUE;
	}else{
		connect_num_errror = FALSE;
	}

	// �G���[�`�F�b�N
/*
    if( (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT) ||
		(GFL_NET_StateGetWifiStatus() > GFL_NET_STATE_DISCONNECTING) ||		// �ؒf�`�F�b�N�͑��ł��
 */
	// 080704	tomoya takahashi
    if( (GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_TIMEOUT) ||	// Disconnect���݂�
				(GFL_NET_StateIsWifiError()) ||
				GFL_NET_StateIsWifiDisconnect() ||
				(GFL_NET_IsConnectMember(GFL_NET_NETID_SERVER) == FALSE) ||	// �e�Ɛؒf����
				(WFP2PMF_MatchCheck( p_wk ) == FALSE) ||	// �N���ƂȂ����Ă���̂ɁA�ʐM���Ă��Ȃ��Ƃ��͂���
				(connect_num_errror == TRUE) ||		// �G���g���[�����ʐM�l�����ւ���
				(timeout == TRUE) ||		// �ڑ��^�C���A�E�g
				//		(parent_status_check == FALSE) ||	// �e�ƒʐM�X�e�[�^�X���ꏏ���`�F�b�N�i_WIFI_MACH_STATUS�j
				(result == TRUE) ){			// �ؒf

#ifdef WFP2P_DEBUG
			if( (GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_TIMEOUT) ){
				NET_PRINT( "(GFL_NET_StateGetWifiStatus() == %d)\n", GFL_NET_StateGetWifiStatus() );
			}
			if( GFL_NET_StateIsWifiError() ){
				NET_PRINT( "GFL_NET_StateIsWifiError()\n" );
			}
			if( GFL_NET_StateIsWifiDisconnect() ){
				NET_PRINT( "CommStateIsWifiDisconnect()\n" );
			}
			if( GFL_NET_IsConnectMember(GFL_NET_NETID_SERVER) == FALSE ){
				NET_PRINT( "!CommIsConnect(COMM_PARENT_ID)\n" );
			}
			if( (WFP2PMF_MatchCheck( p_wk ) == FALSE) ){
				NET_PRINT( "(WFP2PMF_MatchCheck( p_wk ) == FALSE)\n" );
			}
			if( result ){
				NET_PRINT( "errcheck  logout\n" );
			}
			if( timeout ){
				NET_PRINT( "errcheck  timeout\n" );
			}
#endif

			// GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_DISCONNECTING�̐ؒf�����͖�������
			if( p_wk->data.oya ){
				p_wk->data.status = WFP2PMF_OYA_STATUS_ERR_INIT;
			}else{
				p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
			}
		}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Đڑ����Ǝ��̃G���[�`�F�b�N
 *
 *	@param	p_wk
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_ReConErrCheck( WFP2PMF_WK* p_wk )
{
	// �G���[�`�F�b�N
#if 0
	if( (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT) ||
			(GFL_NET_StateGetWifiStatus() > GFL_NET_STATE_DISCONNECTING) ||		// �ؒf�`�F�b�N�͑��ł��
			(GFL_NET_StateIsWifiError())  ){	// �ؒf
#endif
		if( (GFL_NET_StateGetWifiStatus() > GFL_NET_STATE_TIMEOUT) ||
				GFL_NET_StateIsWifiDisconnect() ||
				(GFL_NET_StateIsWifiError())  ){	// �ؒf

#ifdef WFP2P_DEBUG
			if( (GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_TIMEOUT) ){
				NET_PRINT( "(GFL_NET_StateGetWifiStatus() == %d)\n", GFL_NET_StateGetWifiStatus() );
			}
			if( GFL_NET_StateIsWifiError() ){
				NET_PRINT( "GFL_NET_StateIsWifiError()\n" );
			}
#endif

			p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
		}
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�N���ƂȂ����Ă��邩�`�F�b�N
	 *
	 *	@param	cp_wk	���[�N
	 *
	 *	@retval	TRUE	�Ȃ����Ă���
	 *	@retval	FALSE	�Ȃ����Ă��Ȃ�
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_MatchCheck( const WFP2PMF_WK* cp_wk )
	{
		if( GFL_NET_GetConnectNum() > 0 ){
			return TRUE;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�}�b�`���O�J�n
	 *
	 *	@param	friendno		�F�B�ԍ�
	 *	@param	type			�}�b�`���O�^�C�v
	 *
	 *	@retval	TRUE	����
	 *	@retval	FALSE	���s
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_CommWiFiMatchStart( u32 friendno, u32 type )
	{
		BOOL ret;

		ret = GFL_NET_StateStartWifiPeerMatch( friendno );
		if( ret == TRUE ){
			//		switch( type ){
			//		case WFP2PMF_TYPE_BUCKET:			// ���܂���
			//		case WFP2PMF_TYPE_BALANCE_BALL:		// ���܂̂�
			//		case WFP2PMF_TYPE_BALLOON:			// �ӂ�������
			//			CommStateChangeWiFiClub();
			//			break;
			//		}
		}
		return ret;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�̃X�e�[�^�X�Ǝ����̃X�e�[�^�X����v���邩�`�F�b�N����
	 *
	 *	@param	cp_wk	���[�N
	 *
	 *	@retval	TRUE	��v
	 *	@retval	FALSE	��v���Ȃ�
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_CheckP2PMatchFriendStatus( const WFP2PMF_WK* cp_wk, const WFP2PMF_INIT* cp_param, int friend )
	{
		_WIFI_MACH_STATUS* pFriendSt;

		if( friend == WIFILIST_FRIEND_MAX ){
			return TRUE;
		}
		pFriendSt = (_WIFI_MACH_STATUS*)GFL_NET_DWC_GetFriendInfo( friend );

		switch( cp_param->type ){

		case WFP2PMF_TYPE_BUCKET:
			break;

		case WFP2PMF_TYPE_BALANCE_BALL:
			break;

		case WFP2PMF_TYPE_BALLOON:
			break;
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	��Ԃ��A��W��Ԃ����s��ԂɕύX����
	 *
	 *	@param	p_wk		���[�N
	 *	@param	cp_init		�f�[�^
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_StatusChange( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init )
	{
		switch( cp_init->type ){

		case WFP2PMF_TYPE_BUCKET:
			break;

		case WFP2PMF_TYPE_BALANCE_BALL:
			break;

		case WFP2PMF_TYPE_BALLOON:
			break;
		}
		GFL_NET_DWC_SetMyInfo( &(p_wk->data.p_match->myMatchStatus), sizeof(_WIFI_MACH_STATUS) );
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�{�C�X�`���b�g	ON
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_StatusVChatOn( WFP2PMF_WK* p_wk )
	{
		if( p_wk->data.vchat_flag == TRUE ){
			// ���������Ă���
			return FALSE;
		}

		// �}�b�`���O���Ă�����{�C�X�`���b�g�𓮂���
		if( WFP2PMF_MatchCheck( p_wk ) == TRUE ){
			GFL_NET_DWC_StartVChat( HEAPID_WIFI_FOURMATCHVCHAT );
			p_wk->data.vchat_flag = TRUE;

			NET_PRINT( "vcht on\n" );
			return TRUE;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�{�C�X�`���b�gOFF
	 *
	 *	@param	p_wk
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_StatusVChatOff( WFP2PMF_WK* p_wk )
	{
		if( p_wk->data.vchat_flag == FALSE ){
			// �����Ƃ܂��Ă�
			return FALSE;
		}

		GFL_NET_DWC_StopVChat();
		p_wk->data.vchat_flag = FALSE;

		NET_PRINT( "vcht off\n" );

		return TRUE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	VCHT�̐ݒ��BGM�̃t�F�[�h
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_StatusVChatOnBmpFade( WFP2PMF_WK* p_wk )
	{
		if( WFP2PMF_StatusVChatOn( p_wk ) ){
			// BGM�̉��ʂ𗎂Ƃ�
			//		Snd_VChatVolSetBySeqNo( Snd_NowBgmNoGet() );  //@@OO�T�E���h�ł܂莟��
		}
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	VCHT�̐ݒ��BGM�̃t�F�[�h
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_StatusVChatOffBmpFade( WFP2PMF_WK* p_wk )
	{
		if( WFP2PMF_StatusVChatOff( p_wk ) ){
			//		Snd_PlayerSetInitialVolume( SND_HANDLE_FIELD, BGM_WIFILOBBY_VOL );//@@OO�T�E���h�ł܂莟��
		}
	}


	//----------------------------------------------------------------------------
	/**
	 *	@brief	�G���g���[�����擾
	 */
	//-----------------------------------------------------------------------------
	static int WFP2PMF_GetEntryNum( WFP2PMF_WK* p_wk )
	{
#if 0 ///COMMINFO���l������@@OO
		if( p_wk->data.oya ){
			return CommInfoGetEntryNum();
		}else{
			return CommInfoGetEntryNum();
		}
#else
		return GFL_NET_GetConnectNum();
#endif
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�Ǝ�����菬����NETID�̐l��ENTRY��Ԃɂ���
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_SetEntryOya( WFP2PMF_WK* p_wk, u32 heapID )
	{
		int i;
		int currentid;
		BOOL result = TRUE;

#if 0 ///COMMINFO���l������@@OO

		// �e�Ǝ�����AID��菬����AID�̎q�̓G���g���[��Ԃɂ���
		currentid = GFL_NET_SystemGetCurrentID();
		currentid --;

		//	NET_PRINT( "my id = %d\n", currentid + 1 );

		for( i=currentid; i>=GFL_NET_NO_PARENTMACHINE; i-- ){
			if( CommInfoIsBattleNewName( i ) == TRUE ){
				WFP2PMF_CommInfoSetEntry( p_wk, i, heapID );

				NET_PRINT( "entry id = %d\n", i );

			}else if( CommInfoGetEntry( i ) == FALSE ) {
				// NEWENTRY�ł��Ȃ�ENTRY�ł��Ȃ��Ƃ��́A
				// �܂��󂯎���Ă��Ȃ��l�̃f�[�^������
				result = FALSE;
			}
		}

#endif
		return result;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	NEW�G���g���[�̐l�ɒʐMNG�𑗐M
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_CommSendNewEntryNg( WFP2PMF_WK* p_wk )
	{
#if 0 ///COMMINFO���l������@@OO
		int i;
		WFP2PMF_COMM_RESULT result;
		BOOL send_result;

		result.flag = WFP2PMF_CON_NG;

		for( i= 1; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
			if( CommInfoIsBattleNewName( i ) == TRUE ){
				result.netID  = i;
				send_result = CommSendData( CNM_WFP2PMF_RESULT, &result, sizeof(WFP2PMF_COMM_RESULT) );
				GF_ASSERT( send_result == TRUE );
				CommInfoDeletePlayer( i );	// ���Ȃ����Ƃɂ���
			}
		}
#endif
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�Ȃ����Ă��邷�ׂẴv���C���[�ɐؒf��ʒm
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_CommSendAllEntryNg( WFP2PMF_WK* p_wk )
	{
#if 0 ///COMMINFO���l������@@OO
		int i;
		WFP2PMF_COMM_RESULT result;
		BOOL send_result;

		result.flag = WFP2PMF_CON_NG;

		for( i= 1; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
			if( CommInfoIsNewName( i ) == TRUE ){
				result.netID  = i;
				send_result = CommSendData( CNM_WFP2PMF_RESULT, &result, sizeof(WFP2PMF_COMM_RESULT) );
				GF_ASSERT( send_result == TRUE );
				CommInfoDeletePlayer( i );	// ���Ȃ����Ƃɂ���
			}
		}
#endif
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	LOG Out�`�F�b�N�J�n
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_LogOutCheckStart( WFP2PMF_WK* p_wk )
	{
		GFL_NET_DWC_SetDisconnectCallback( WFP2PMF_LogOutCallBack, p_wk );
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	LOG out�@�`�F�b�N�I��
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_LogOutCheckEnd( WFP2PMF_WK* p_wk )
	{
		GFL_NET_DWC_SetDisconnectCallback( NULL, NULL );
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	LOGOUT�`�F�b�N
	 *
	 *	@param	p_wk	���[�N
	 *
	 *	@retval	TRUE	LOG�A�E�g���m		ERR������
	 *	@retval	FALSE	LOG�A�E�g�Ȃ��@�܂��͊֌W�Ȃ��l��LOG�A�E�g����
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_LogOutCheck( WFP2PMF_WK* p_wk )
	{
		int i;
		BOOL result = FALSE;

#if 0 ///COMMINFO���l������@@OO
		for( i=0;i<WIFI_P2PMATCH_NUM_MAX; i++ ){

			if( p_wk->data.logout_in[i] == TRUE ){

				// ���̐l��ENTER���傤�������`�F�b�N
				if( CommInfoGetEntry( p_wk->data.logout_id[i] ) == TRUE ){
					result = TRUE;
				}

				// ���[�N������
				p_wk->data.logout_in[i] = FALSE;
				p_wk->data.logout_id[i] = 0;
			}
		}
#endif
		return result;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	LOGOUT�R�[���o�b�N	�ؒf���ɌĂ΂��
	 *
	 *	@param	aid			�ؒf����AID
	 *	@param	pWork		���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_LogOutCallBack( u16 aid, void* pWork )
	{
		WFP2PMF_WK* p_wk = pWork;

		p_wk->data.logout_in[aid] = TRUE;
		p_wk->data.logout_id[aid] = aid;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�R�l�N�g���X�g���ĕ`�悷��K�v�����邩�`�F�b�N
	 *
	 *	@param	p_wk	���[�N
	 *
	 *	@retval	TRUE	�ĕ`��
	 *	@retval FALSE	�ĕ`��̕K�v�Ȃ�
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_ConListWriteCheck( WFP2PMF_WK* p_wk )
	{
		int i;
		BOOL result = FALSE;

		// �N����VCHAT�t���O���ς���Ă���
		for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
			if( p_wk->data.vchat_tmp[i] != p_wk->data.vchat_now[i] ){
				result = TRUE;
			}
			p_wk->data.vchat_tmp[i] = p_wk->data.vchat_now[i];
		}

		// �ĕ`��t���O�������Ă���
		if( p_wk->data.conlist_rewrite ){
			p_wk->data.conlist_rewrite = FALSE;
			result = TRUE;
		}

		return result;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�R�l�N�g���X�g�ĕ`��t���O�̐ݒ�
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_ConListWriteReq( WFP2PMF_WK* p_wk )
	{
		p_wk->data.conlist_rewrite = TRUE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	COMMINFO�̃��[�N�ɃG���g���[��ݒ�
	 *
	 *	@param	p_wk	���[�N
	 *	@param	aid		AID
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_CommInfoSetEntry( WFP2PMF_WK* p_wk, u32 aid, u32 heapID )
	{
#if 0 ///COMMINFO���l������@@OO
		// ENTRY����Ȃ�������ENTRY
		if( CommInfoGetEntry( aid ) == FALSE ){
			CommInfoSetEntry( aid );

			// �O��NETID���i�[
			p_wk->data.last_netid = aid;

			// OAM�`��J�n
			WFP2PMF_DrawOamInit( p_wk, aid, heapID );

			// �R�l�N�g���X�g�ĕ`��
			WFP2PMF_ConListWriteReq( p_wk );
		}
#endif
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	OAM�`��J�n
	 *
	 *	@param	p_wk		���[�N
	 *	@param	aid			AID
	 *	@param	heapID		�q�[�vID
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_DrawOamInit( WFP2PMF_WK* p_wk, u32 aid, u32 heapID )
	{
		WF2DMAP_OBJDATA objdata;
		MYSTATUS* p_status;

		// ��������Ă��邩�牽�����Ȃ�
		if( p_wk->draw.objdata[aid].p_objwk != NULL ){
			return ;
		}

		// �������f�[�^�R�s�[
		objdata = WFP2PMF_OBJData[ aid ];
		if( aid != GFL_NET_SystemGetCurrentID() ){
#if 0 ///COMMINFO���l������@@OO
			p_status = CommInfoGetMyStatus( aid );
			GF_ASSERT( p_status != NULL );
			objdata.charaid = MyStatus_GetTrainerView( p_status );
#endif
		}else{
			if( p_wk->data.p_match->myMatchStatus.sex == PM_MALE ){
				//			objdata.charaid = HERO;    //@@OO
			}else{
				//			objdata.charaid = HEROINE;  //@@OO
			}
		}

		// �I�u�W�F�N�g������
		p_wk->draw.objdata[aid].p_objwk = WF2DMAP_OBJWkNew( p_wk->draw.p_objsys, &objdata );


		// �`�惊�\�[�X�̓Ǎ���
		if( WF2DMAP_OBJDrawSysResCheck( p_wk->draw.p_objdraw, objdata.charaid ) == FALSE ){
			WF2DMAP_OBJDrawSysResSet( p_wk->draw.p_objdraw, objdata.charaid,
																WF_2DC_MOVETURN, heapID );
		}


		// �`�惏�[�N�쐬
		p_wk->draw.objdata[aid].p_objdrawwk = WF2DMAP_OBJDrawWkNew(
			p_wk->draw.p_objdraw, p_wk->draw.objdata[aid].p_objwk,
			FALSE, heapID );

		// �ŏ��͔�\��
		WF2DMAP_OBJDrawWkDrawFlagSet( p_wk->draw.objdata[aid].p_objdrawwk, FALSE );

		// ����J�n
		p_wk->draw.objdata[aid].seq++;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	OAM�`�惁�C������
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_DrawOamMain( WFP2PMF_WK* p_wk )
	{
		int i;

		// �`��I�u�W�F�N�g����
		for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
			pDrawObjMoveFunc[p_wk->draw.objdata[i].seq]( p_wk, &p_wk->draw.objdata[i] );
		}

		// �R�}���h�I�u�W�F�N�g����
		WF2DMAP_OBJSysMain( p_wk->draw.p_objsys );

		// �`��X�V
		WF2DMAP_OBJDrawSysUpdata( p_wk->draw.p_objdraw );
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	OAM�`��j������
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_DrawOamDelete( WFP2PMF_WK* p_wk )
	{
		// �S���\�[�X�j��
		WF2DMAP_OBJDrawSysAllResDel( p_wk->draw.p_objdraw );

		// �`��I�u�W�F�f�[�^�j��
		GFL_STD_MemClear( p_wk->draw.objdata, sizeof( WFP2PMF_DRAWOBJ )*WIFI_P2PMATCH_CLACTWK_NUM );
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�ɒʐMLOCK�𑗐M
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_NewConLockSend( WFP2PMF_WK* p_wk, u32 aid )
	{
		WFP2PMF_COMM_RESULT result;
		BOOL send_result;

		result.netID = aid;
		result.flag = WFP2PMF_CON_LOCK;
		send_result = GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_RESULT, sizeof(WFP2PMF_COMM_RESULT),&result );
		GF_ASSERT( send_result == TRUE );
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	last_netID��ENTRY�ɂȂ�O�ɂ�����傫��NETID�̐l��NEWENTRY��
	 *          �Ȃ�����Đڑ����Ă��炤
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_NewConLockNewCon( WFP2PMF_WK* p_wk )
	{
		int i;
#if 0 ///COMMINFO���l������@@OO

		for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
			if( p_wk->data.last_netid+1 < i ){	// �Ō��ENTRY����NETID���傫��

				if( CommInfoIsBattleNewName( i ) == TRUE ){
					// �ؒf�肢�𑗐M
					WFP2PMF_NewConLockSend( p_wk, i );
					// ���[�U�[�f�[�^�j��
					CommInfoDeletePlayer( i );
				}
			}
		}
#endif
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�̎�M�����l��ENTRY����
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_KoRecvEntry( WFP2PMF_WK* p_wk, u32 heapID )
	{
		int i;
#if 0 ///COMMINFO���l������@@OO

		for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
			if( p_wk->data.entry_on[ i ] == TRUE ){

				if( CommInfoIsNewName( i ) == TRUE ){
					WFP2PMF_CommInfoSetEntry( p_wk, i, heapID );

					p_wk->data.entry_on[ i ] = FALSE;

					// �V�K�R�l�N�g��������
					p_wk->data.ko_newcon_flag = TRUE;
				}
			}
		}
#endif
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	TimeWaitIcon�\��
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_TimeWaitIconOn( WFP2PMF_WK* p_wk )
	{
		GF_ASSERT( p_wk->draw.p_timewaiticon == NULL );
		//	p_wk->draw.p_timewaiticon = TimeWaitIconAdd( &p_wk->draw.msgwin, WFP2PMF_BG1_TALKWIN_CGX ); //@@OO
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	TimeWaitIcon�@��\��
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_TimeWaitIconOff( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID )
	{
#if 0
		int wintype;

		if( p_wk->draw.p_timewaiticon == NULL ){
			return ;
		}

		TimeWaitIconDel(p_wk->draw.p_timewaiticon);  // �^�C�}�[�~��
		p_wk->draw.p_timewaiticon = NULL;

		// �E�B���h�E�f�[�^�ē]��
		wintype = cp_init->wintype;
		//	TalkWinGraphicSet(
		BmpWinFrame_GraphicSet(
			GFL_BG_FRAME1_M, WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL,  wintype, heapID );
#endif
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�`��I�u�W�F	����Ȃ�
	 *
	 *	@param	p_wk		�V�X�e�����[�N
	 *	@param	p_obj		�I�u�W�F�N�g�f�[�^
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_DrawObjMoveNone( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj )
	{
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�`��I�u�W�F�N�g�@���쏉����
	 *
	 *	@param	p_wk		�V�X�e�����[�N
	 *	@param	p_obj		�I�u�W�F�N�g�f�[�^
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_DrawObjMoveInit( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj )
	{
		u32 way;

		// �`��X�V��~
		WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_objdrawwk, FALSE );

		// ���邭��A�j���ݒ�
		WF2DMAP_OBJDrawWkKuruAnimeStart( p_obj->p_objdrawwk );

		// �`��J�n
		WF2DMAP_OBJDrawWkDrawFlagSet( p_obj->p_objdrawwk, TRUE );

		// ���o�����
		p_obj->seq = WFP2PMF_DRAWOBJ_MOVEIN;

		// ���o����G�t�F�N�g�^�C��
		way = WF2DMAP_OBJWkDataGet( p_obj->p_objwk, WF2DMAP_OBJPM_WAY );
		p_obj->count = WFP2PMF_DRAWOBJ_INTIME + WFP2PMF_DRAWOBJ_INTIME_DIV[way];
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�`��I�u�W�F�N�g	����
	 *
	 *	@param	p_wk		�V�X�e�����[�N
	 *	@param	p_obj		�I�u�W�F�N�g�f�[�^
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_DrawObjMoveIn( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj )
	{
		// ���邭��A�j�����C��
		WF2DMAP_OBJDrawWkKuruAnimeMain( p_obj->p_objdrawwk );

		// �^�C���v�Z
		p_obj->count --;
		if( p_obj->count <= 0 ){
			// ���o�G�t�F�N�g�I��
			WF2DMAP_OBJDrawWkKuruAnimeEnd( p_obj->p_objdrawwk );

			// �`��A�b�v�f�[�g�J�n
			WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_objdrawwk, TRUE );

			// �ʏ�`���
			p_obj->seq = WFP2PMF_DRAWOBJ_MOVENORMAL;
		}
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�`��I�u�W�F�N�g	�ʏ펞
	 *
	 *	@param	p_wk		�V�X�e�����[�N
	 *	@param	p_obj		�I�u�W�F�N�g�f�[�^
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_DrawObjMoveNormal( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj )
	{
		// ���ɉ������Ȃ�
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	VChat�t���O�𑗐M
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_VchatCommSend( WFP2PMF_WK* p_wk )
	{
		int i;
		int roop_num;
		BOOL result = FALSE;
		int friendno;
		int vchat;
		WFP2PMF_COMM_VCHAT comm_vchat;
#if 0 ///COMMINFO���l������@@OO

		// Info���Ȃ��Ȃ�����Ă΂Ȃ�
		if( CommInfoIsInitialize() == FALSE ){

			return ;
		}

		// �G���g���[�ς݂̐l�̐����擾
		roop_num = CommInfoGetEntryNum();

		// �V�����l�������Ă�����A���̃{�C�X�`���b�g��Ԃ͑��M����
		if(CommInfoIsNewInfoData()){
			if(!CommIsSendCommand_ServerSize(CS_COMM_INFO_END)){
				result = TRUE;
			}
		}

		// �N����VCHAT�t���O���ς���Ă���
		for( i=0; i<roop_num; i++ ){
			if( i==GFL_NET_NO_PARENTMACHINE ){
				vchat = p_wk->data.p_match->myMatchStatus.vchat;
			}else{
				friendno = CommInfoSearchWifiListIndex( i );
				GF_ASSERT( friendno != WIFILIST_FRIEND_MAX );
				vchat = p_wk->data.p_match->friendMatchStatus[ friendno ].vchat;
			}
			if( p_wk->data.vchat_now[i] != vchat ){
				result = TRUE;
			}
			p_wk->data.vchat_now[i] =vchat;
		}

		// �t���O���ς���Ă����瑗�M����
		if( result == TRUE ){
			BOOL send_result;
			//		memcpy( comm_vchat.vchat_now, p_wk->data.vchat_now, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );
			GFL_STD_MemCopy(  p_wk->data.vchat_now,comm_vchat.vchat_now, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );
			CommSendData( CNM_WFP2PMF_VCHAT, &comm_vchat, sizeof(WFP2PMF_COMM_VCHAT) );
			NET_PRINT( "VCHATFLAG send\n" );
		}
#endif
	}


	//----------------------------------------------------------------------------
	/**
	 *	@brief	�}�b�`���O���������ă~�j�Q�[���̊J�n�֐i�ނ��`�F�b�N
	 *
	 *	@param	p_wk	���[�N
	 *
	 *	@retval	TRUE	�}�b�`���O�����[���~�j�Q�[���J�n��
	 *	@retval	FALSE	�܂���W��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_MatchOkCheck( WFP2PMF_WK* p_wk )
	{
		if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_CHECK)){
			return TRUE;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ڑ����Ă����v���C���[�̏�Ԃ��`�F�b�N����
	 *
	 *	@param	p_wk	���[�N
	 *
	 *	@retval	TRUE	����
	 *	@retval	FALSE	�s��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_Oya_CheckConnectPlayer( const WFP2PMF_WK* cp_wk, const WFP2PMF_INIT* cp_param )
	{
		int i;
		int friend;
		BOOL ret;
		BOOL result;

#if 0 ///COMMINFO���l������@@OO
		ret = TRUE;

		// �ʐM�ڑ������l�S����STATUS���`�F�b�N���A
		// �s���Ȑl��������A�ؒf����B
		for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
			if( i != GFL_NET_SystemGetCurrentID() ){
				if( GFL_NET_IsConnectMember(i) == TRUE ){

					// STATUS�`�F�b�N
					friend = CommInfoSearchWifiListIndex( i );
					if( friend != WIFILIST_FRIEND_MAX ){
						result = WFP2PMF_CheckP2PMatchFriendStatus( cp_wk, cp_param, friend );
					}else{

						// �������������ɂ͂��Ȃ��͂������A
						// ���̐l��ؒf
						result = FALSE;
					}

					// �ؒf����
					if( result == FALSE ){
						ret = FALSE;

						// ���̐l��ؒf
						DWC_CloseConnectionHard(i);
					}

				}
			}
		}
#endif
		return ret;
	}


	//----------------------------------------------------------------------------
	/**
	 *	@brief	�^�C���A�E�g	�X�^�[�g
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_TimeOut_Start( WFP2PMF_WK* p_wk )
	{
		p_wk->data.timeout_flag = TRUE;
		p_wk->data.timeout		= CONNECT_TIMEOUT;
	}


	//----------------------------------------------------------------------------
	/**
	 *	@brief	�^�C���A�E�g	�X�g�b�v
	 *
	 *	@param	p_wk	���[�N
	 */
	//-----------------------------------------------------------------------------
	static void WFP2PMF_TimeOut_Stop( WFP2PMF_WK* p_wk )
	{
		p_wk->data.timeout_flag = FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�^�C���A�E�g	���C��
	 *
	 *	@param	p_wk	���[�N
	 *
	 *	@retval	TRUE	�G���[���
	 *	@retval	FALSE	�ʏ�
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_TimeOut_Main( WFP2PMF_WK* p_wk )
	{
		// �^�C���E�G�C�g�`�F�b�N�����H
		if( p_wk->data.timeout_flag == FALSE ){
			return FALSE;
		}

		if( p_wk->data.timeout > 0 ){
			p_wk->data.timeout--;
			return FALSE;
		}
		return TRUE;
	}




	//-----------------------------------------------------------------------------
	/**
	 *		�e�@��ԁ@�֐�
	 */
	//-----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e		������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// VCHATONOFF
		if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){

			WFP2PMF_StatusVChatOnBmpFade( p_wk );
		}

		// ��W���́AClosedCallback�����Ă��A�ؒf�����ɑJ�ڂ��Ȃ��悤�Ƀ��b�N����
		GFL_NET_DWC_SetClosedDisconnectFlag( FALSE );

		GFL_NET_SetAutoErrorCheck(TRUE);
		GFL_NET_SetNoChildErrorCheck(FALSE);

		// OAM�`��J�n
		WFP2PMF_DrawOamInit( p_wk, GFL_NET_SystemGetCurrentID(), heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e		�R�l�N�g�҂���ԏ�����
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaConnectWaitInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// ���b�Z�[�W
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_107, heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e		�R�l�N�g�҂����
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaConnectWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

#if 0 ///COMMINFO���l������@@OO

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// �V�K�R�l�N�g�҂�
		// �ʐM�����`�F�b�N
		if( WFP2PMF_MatchCheck( p_wk ) == TRUE ){
			int new_con_netid;
			WFP2PMF_COMM_RESULT result;
			BOOL send_result;


			new_con_netid = CommInfoGetNewNameID();

			if( new_con_netid != GFL_NET_NETID_INVALID ){

				// ������������Ȃɂ����Ȃ�
				if( p_wk->data.new_con != GFL_NET_NO_PARENTMACHINE ){
					// ���̐l��m���Ă��邩�`�F�b�N
					if( CommInfoSearchWifiListIndex( new_con_netid ) == WIFILIST_FRIEND_MAX ){
						// �m��Ȃ��̂ŁA���ۂ���
						result.netID	= new_con_netid;
						result.flag		= WFP2PMF_CON_NG;

						// �ؒf�𖽗߂���
						CommInfoDeletePlayer( new_con_netid );

						// �q�@�ɒʐM�ؒf��ʒm
						send_result = CommSendData( CNM_WFP2PMF_RESULT, &result, sizeof(WFP2PMF_COMM_RESULT) );
						GF_ASSERT( send_result == TRUE );

					}else{
						p_wk->data.new_con = new_con_netid;
					}
				}
			}
		}

		do{
			// �i�ރ{�^����������
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
				SOUND_SEPLAY( _SE_DESIDE );

				// �ʐM�����`�F�b�N
				if( WFP2PMF_MatchCheck( p_wk ) == FALSE ){
					// �N�Ƃ��}�b�`���O���Ă��Ȃ�
					p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_MIN_INIT;
					break;
				}else{

					// �G���g���[��������Ă��邩�`�F�b�N
					if( p_init->comm_min <= WFP2PMF_GetEntryNum( p_wk ) ){
						p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT00;
						break;
					}else{
						// ����Ă��Ȃ��̂ŁA�V�Ԃ̂���߂�̂������ɂ���
						p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_MIN_INIT;
						break;
					}
				}
			}
			// �L�����Z�������`�F�b�N
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
				SOUND_SEPLAY( _SE_DESIDE );
				//			p_wk->data.status = WFP2PMF_OYA_STATUS_END_INIT00;	// �I���`�F�b�N���Q����s��
				p_wk->data.status = WFP2PMF_OYA_STATUS_END2_INIT00;	// �I���`�F�b�N���Q����s��
				break;
			}

			// �V�R�l�N�g�������I
			if( p_wk->data.new_con != GFL_NET_NETID_INVALID ){

				SOUND_SEPLAY( _SE_DESIDE );

				p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT00;
				break;
			}

			// �{�C�X�`���b�g�`�F�b�N
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
				p_wk->data.status = WFP2PMF_OYA_STATUS_VCHAT_INIT00;
				SOUND_SEPLAY( _SE_DESIDE );
				break;
			}

		}while(0);
#endif
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e			�V�R�l�N�g�I���@������00
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaConnectSelectInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{

		// ���̐l�̏��\��
		WFP2PMF_GraphicNewConStrPut( &p_wk->draw, p_init, p_wk->data.new_con, heapID );

		// ���b�Z�[�W�̕\��
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_108, heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT01;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e			�V�R�l�N�g�I���@������01
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaConnectSelectInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;
		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}


		// YESNO�E�B���h�E���o��
		//	p_wk->draw.p_yesnowin = BmpYesNoSelectInit( p_wk->draw.p_bgl,
		//							&WFP2PMF_YESNOBMPDAT,
		//							WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
		//							heapID );
		p_wk->draw.p_yesnowin = BmpMenu_YesNoSelectInit(&WFP2PMF_YESNOBMPDAT,
																										WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,0,
																										heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_SELECT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�V�R�l�N�g�I��
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaConnectSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int yesno_ret;
		WFP2PMF_COMM_RESULT result;
		BOOL send_result;

		// YES�Ȃ�R�l�N�g�@No�Ȃ�ؒf
		yesno_ret = BmpMenu_YesNoSelectMain( p_wk->draw.p_yesnowin );


		if( yesno_ret != BMPMENU_NULL ){
			result.netID = p_wk->data.new_con;

			if( yesno_ret == 0 ){	// �͂���I��
				result.flag = WFP2PMF_CON_OK;

				// �G���g���[
				WFP2PMF_CommInfoSetEntry( p_wk, p_wk->data.new_con, heapID );


				// Vchat���Ȃ�
				if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){
					// �{�C�X�`���b�gOFF
					WFP2PMF_StatusVChatOff( p_wk );

					// �{�C�X�`���b�gON
					WFP2PMF_StatusVChatOnBmpFade( p_wk );
				}


				// �l����MAX�ɂȂ�����Q�[���J�n��
				if( p_init->comm_max <= WFP2PMF_GetEntryNum( p_wk ) ){
					p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT00;
				}else{
					// MAX�ɂȂ��Ă��Ȃ��Ȃ�҂���Ԃɖ߂�
					p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
				}

			}else{

				result.flag = WFP2PMF_CON_NG;
#if 0 ///COMMINFO���l������@@OO
				// �L�����Z���Ȃ�ؒf
				// �ؒf�𖽗߂���
				CommInfoDeletePlayer( p_wk->data.new_con );
#endif

				// �҂���Ԃɖ߂�
				p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
			}

			// �q�@�ɒʐMOK��ʒm
			send_result = GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_RESULT, sizeof(WFP2PMF_COMM_RESULT),&result  );
			GF_ASSERT( send_result == TRUE );

			// �V�K�R�l�N�g�I��
			p_wk->data.new_con = GFL_NET_NETID_INVALID;
			WFP2PMF_GraphicNewConStrOff( &p_wk->draw );
			p_wk->draw.p_yesnowin = NULL;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�J�n���������ǁA�l��������Ȃ������Ƃ�	������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaConnectMinInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// ���b�Z�[�W�\��
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_122, heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_MIN;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�J�n���������ǁA�l��������Ȃ������Ƃ�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaConnectMin( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// ��߂邩�����V�[�P���X��
		p_wk->data.status = WFP2PMF_OYA_STATUS_END_INIT00;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�J�n	������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaStartInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// ���b�Z�[�W�\��	���̃����o�[�ł�낵���ł����H
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_111, heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT01;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�J�n	������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaStartInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// YESNO�E�B���h�E���o��
		p_wk->draw.p_yesnowin = BmpMenu_YesNoSelectInit(
			&WFP2PMF_YESNOBMPDAT,
			WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,0,
			heapID );
		p_wk->data.status = WFP2PMF_OYA_STATUS_START_SELECT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�J�n	�I��
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaStartSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int yesno_ret;

		// YES�Ȃ�R�l�N�g�@No�Ȃ�ؒf
		yesno_ret = BmpMenu_YesNoSelectMain( p_wk->draw.p_yesnowin );
		if( yesno_ret != BMPMENU_NULL ){
			if( yesno_ret == 0 ){	// �͂���I��

				p_wk->data.status = WFP2PMF_OYA_STATUS_START;
			}else{

				// �l����MAX�ɂȂ�����Q�[���I����
				if( p_init->comm_max <= WFP2PMF_GetEntryNum( p_wk ) ){
					p_wk->data.status = WFP2PMF_OYA_STATUS_END_INIT00;
				}else{
					p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
				}
			}

			// �I��
			p_wk->draw.p_yesnowin = NULL;
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@���ۂɊJ�n
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// �����J�n�^�C�}�[�Z�b�g
		p_wk->data.timer = 30;

		// ENTRY�ȊO�̐l��ؒf����
		WFP2PMF_CommSendNewEntryNg( p_wk );

		// �ʐM�ҋ@�����b�Z�[�W�\��
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_066, heapID );

		// �҂��A�C�R���\��
		WFP2PMF_TimeWaitIconOn( p_wk );

		// �^�C���E�G�C�g�J�n
		WFP2PMF_TimeOut_Start( p_wk );

		// �l���`�F�b�N��
		p_wk->data.status = WFP2PMF_OYA_STATUS_STARTNUMCHECK;

		// �N���C�A���g���b�N���ꉞ����
		GFL_NET_DWC_ResetClientBlock();

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�J�n�O�Ɂ@�ʐM�l���`�F�b�N
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaStartNumCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int result;

#if 0 ///COMMINFO���l������@@OO

		// ENTRY�l���ƒʐM���̐l�����ꏏ�ɂȂ�����I���
		if( GFL_NET_GetConnectNum() != CommInfoGetEntryNum() ){
			NET_PRINT( "connectnum notmatch\n" );
			// ENTRY�ȊO�̐l��ؒf����
			WFP2PMF_CommSendNewEntryNg( p_wk );
			return FALSE;
		}
#endif
		// �V�K�R�l�N�g���b�N
		result = GFL_NET_DWC_SetClientBlock();
		if( result == FALSE ){
			NET_PRINT( "GFL_NET_DWC_SetClientBlock false  %d  \n", result );
			return FALSE;
		}

		// ClosedCallback��������ؒf�����֑J�ڂ���
		GFL_NET_DWC_SetClosedDisconnectFlag( TRUE );
		// �����J�n��
		p_wk->data.status = WFP2PMF_OYA_STATUS_STARTSYNC_INIT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�����J�n
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaStartSyncInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		if(p_wk->data.timer == 0){
			result = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_START, 0, NULL );	// �Q�[���J�n
			if( result ){
				p_wk->data.status = WFP2PMF_OYA_STATUS_STARTSYNC;
			}
		}else{
			p_wk->data.timer--;
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�����҂�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaStartSync( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_CHECK)){

			// �X�e�[�^�X��������
			WFP2PMF_StatusChange( p_wk, p_init );

			//		CommToolTempDataReset();         //@@OO�R�}���h������ĈڐA���Ȃ���΂����Ȃ� 2.2
			//		CommTimingSyncStart(_TIMING_GAME_CHECK2);
			GFL_NET_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_GAME_CHECK2);
			p_wk->data.status = WFP2PMF_OYA_STATUS_STATE_SEND;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�X�e�[�^�X���M
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaStateSend( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_CHECK2)){
			u16 status = p_wk->data.p_match->myMatchStatus.status;
			BOOL result=TRUE;
			//        result = CommToolSetTempData(GFL_NET_SystemGetCurrentID() ,&status);  @@OO
			if( result == TRUE ){
				p_wk->data.status = WFP2PMF_OYA_STATUS_STATE_CHECK;
			}
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�X�e�[�^�X�`�F�b�N
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaStateCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int i;
		int con_num;
		int current_id;
		const u16* cp_data=NULL;
		int ok_num;

		// �G���g���[�����擾
		con_num = WFP2PMF_GetEntryNum( p_wk );

		current_id = GFL_NET_SystemGetCurrentID();

		ok_num = 0;

		for( i=0; i<con_num; i++ ){

			if( current_id != i ){

				// �X�e�[�^�X���ꏏ���`�F�b�N
				//			cp_data = CommToolGetTempData( i );	  @@OO

				// �X�e�[�^�X�̎�M���������Ă��邩�`�F�b�N
				if( cp_data != NULL ){
					if( cp_data[ 0 ] == p_wk->data.p_match->myMatchStatus.status ){
						ok_num ++;
					}else{
						p_wk->data.status = WFP2PMF_OYA_STATUS_ERR_INIT;
					}
				}
			}else{
				// ������OK
				ok_num ++;
			}
		}

		// �S���̃X�e�[�^�X����M���ă`�F�b�N���ʂ�OK�Ȃ玟�̏�����
		if( ok_num == con_num ){
			p_wk->data.status = WFP2PMF_OYA_STATUS_MYSTATUS_WAIT;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�X�e�[�^�X�`�F�b�N
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaMyStatusWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		//	CommInfoSendPokeData();
		GFL_NET_SetAutoErrorCheck(TRUE);
		GFL_NET_SetNoChildErrorCheck(TRUE);
		GFL_NET_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_GAME_START2);
		p_wk->data.status = WFP2PMF_OYA_STATUS_GAME_START;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@	�Q�[���J�n
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaGameStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_START2)){
			// �����J�n�Ȃ�
			p_wk->data.match_result = TRUE;

			// WaitIcon������
			WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

			return TRUE;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�I���`�F�b�N�@������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaEndInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// ���b�Z�[�W�̕\��
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_123, heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_END_INIT01;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�I���`�F�b�N�@������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaEndInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;
		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// YESNO�E�B���h�E���o��
		p_wk->draw.p_yesnowin = BmpMenu_YesNoSelectInit(
			&WFP2PMF_YESNOBMPDAT,
			WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
			1, heapID );
		p_wk->data.status = WFP2PMF_OYA_STATUS_END_SELECT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�I���`�F�b�N
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaEndSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int yesno_ret;

		// YES�Ȃ�R�l�N�g�@No�Ȃ�ؒf
		yesno_ret = BmpMenu_YesNoSelectMain( p_wk->draw.p_yesnowin );
		if( yesno_ret != BMPMENU_NULL ){
			if( yesno_ret == 0 ){	// �͂���I��
				p_wk->data.status = WFP2PMF_OYA_STATUS_END2_INIT00;
			}else{

				// �ʐM���Ă邩�`�F�b�N
				if( WFP2PMF_MatchCheck( p_wk ) ){
					// �l����MAX�ɂȂ�����Q�[���I����
					if( p_init->comm_max <= WFP2PMF_GetEntryNum( p_wk ) ){
						p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT00;
					}else{
						p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
					}
				}else{
					p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
				}
			}

			p_wk->draw.p_yesnowin = NULL;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�I���`�F�b�N2
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaEnd2Init00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// ���b�Z�[�W�\��	���̃����o�[�ł�낵���ł����H
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_124, heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_END2_INIT01;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�I���`�F�b�N2
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaEnd2Init01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// YESNO�E�B���h�E���o��
		p_wk->draw.p_yesnowin = BmpMenu_YesNoSelectInit(
			&WFP2PMF_YESNOBMPDAT,
			WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
			1,heapID );
		p_wk->data.status = WFP2PMF_OYA_STATUS_END2_SELECT;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�I���`�F�b�N2
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaEnd2Select( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int yesno_ret;

		// YES�Ȃ�R�l�N�g�@No�Ȃ�ؒf
		yesno_ret = BmpMenu_YesNoSelectMain( p_wk->draw.p_yesnowin );
		if( yesno_ret != BMPMENU_NULL ){
			if( yesno_ret == 0 ){	// �͂���I��

				p_wk->data.status = WFP2PMF_OYA_STATUS_END3_INIT;
			}else{

				// �ʐM���Ă邩�`�F�b�N
				if( WFP2PMF_MatchCheck( p_wk ) ){
					// �l����MAX�ɂȂ�����Q�[���I����
					if( p_init->comm_max <= WFP2PMF_GetEntryNum( p_wk ) ){
						p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT00;
					}else{
						p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
					}
				}else{
					p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
				}
			}

			// �I��
			p_wk->draw.p_yesnowin = NULL;
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�I���`�F�b�N3
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaEnd3Init( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_125, heapID );

		// ���b�Z�[�W�\���Ԋu
		p_wk->data.timer = WIFI_P2PMATCH_MSG_WAIT;

		p_wk->data.status = WFP2PMF_OYA_STATUS_END;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�I������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaEnd( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// ���b�Z�[�W�\�����Ԃ��I���܂ł܂�
		p_wk->data.timer --;
		if( p_wk->data.timer > 0 ){
			return FALSE;
		}


		p_wk->data.match_result = FALSE;

		// �}�b�`���O���Ȃ�}�b�`���O������
		//	if( WFP2PMF_MatchCheck(p_wk) ){  //�ēx�Ȃ����������ֈڍs���� 080525/k.o/0193
		GFL_NET_StateWifiMatchEnd(TRUE);
		//	}

		// �݂�Ȑؒf����
		WFP2PMF_CommSendAllEntryNg( p_wk );

		// �ʐM��Ԃ����ɖ߂�
		//CommStateChangeWiFiLogin();  //@@OO


		GFL_NET_SetNoChildErrorCheck(FALSE);
		GFL_NET_SetAutoErrorCheck(TRUE);

		return TRUE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�G���[����	������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaErrInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// YesNo�̃��[�N����������j��
		if( p_wk->draw.p_yesnowin != NULL ){
			BmpMenu_YesNoMenuExit( p_wk->draw.p_yesnowin );
			p_wk->draw.p_yesnowin = NULL;
		}
		WFP2PMF_GraphicNewConStrOff( &p_wk->draw );

		// �҂��A�C�R����\��
		WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );


		p_wk->data.status = WFP2PMF_OYA_STATUS_ERR_INIT01;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�G���[����	������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaErrInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// ���b�Z�[�W�\��	���̃����o�[�ł�낵���ł����H
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_120, heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_ERR;

		// �݂�Ȑؒf����
		WFP2PMF_CommSendAllEntryNg( p_wk );

#if 1//PL_G0255_081217_FIX
		{
			int ret;
			WFP2PMF_StatusChange( p_wk, p_init );
			ret = GFL_NET_StateGetWifiStatus();
			if( (ret == GFL_NET_STATE_DISCONNECTING) ||	// �ؒf��Ԃ��y�x�ȃG���[�Ȃ�ADWC�̍X�V�̂ݍs��
					(ret == GFL_NET_STATE_FAIL) ){
				DWC_ProcessFriendsMatch();  // DWC�ʐM�����X�V
			}
		}
#endif

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@�G���[����
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaErr( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

#if 1//PL_G0255_081217_FIX
		{
			int ret;
			ret = GFL_NET_StateGetWifiStatus();
			if( (ret == GFL_NET_STATE_DISCONNECTING) ||	// �ؒf��Ԃ��y�x�ȃG���[�Ȃ�ADWC�̍X�V�̂ݍs��
					(ret == GFL_NET_STATE_FAIL) ){
				DWC_ProcessFriendsMatch();  // DWC�ʐM�����X�V
			}
		}
#endif

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// �}�b�`���O������
		GFL_NET_StateWifiMatchEnd(TRUE);


		// �ʐM��Ԃ����ɖ߂�
		//	CommStateChangeWiFiLogin();  //@@OO

		// �݂�Ȑؒf����
		WFP2PMF_CommSendAllEntryNg( p_wk );


		// �I������
		p_wk->data.match_result = FALSE;

		GFL_NET_SetNoChildErrorCheck(FALSE);
		GFL_NET_SetAutoErrorCheck(TRUE);

		return TRUE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@vchat����	������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaVchatInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		u32 idx;

		if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){
			idx = msg_wifilobby_128;
		}else{
			idx = msg_wifilobby_127;
		}

		// ���b�Z�[�W�\��	���̃����o�[�ł�낵���ł����H
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, idx, heapID );

		p_wk->data.status = WFP2PMF_OYA_STATUS_VCHAT_INIT01;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@vchat����	������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaVchatInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// YESNO�E�B���h�E���o��
		p_wk->draw.p_yesnowin = BmpMenu_YesNoSelectInit(
			&WFP2PMF_YESNOBMPDAT,
			WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,0,
			heapID );
		p_wk->data.status = WFP2PMF_OYA_STATUS_VCHAT_SELECT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�e�@vchat����
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_OyaVchatSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int yesno_ret;

		// YES�Ȃ�R�l�N�g�@No�Ȃ�ؒf
		yesno_ret = BmpMenu_YesNoSelectMain( p_wk->draw.p_yesnowin );
		if( yesno_ret != BMPMENU_NULL ){
			if( yesno_ret == 0 ){	// �͂���I��

				WFP2PMF_VchatSwitch( p_wk, heapID );

				// �R�l�N�g���X�g�ĕ`��
				WFP2PMF_ConListWriteReq( p_wk );

			}
			p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;

			// �I��
			p_wk->draw.p_yesnowin = NULL;
		}

		return FALSE;
	}



	//-----------------------------------------------------------------------------
	/**
	 *		�q�@��ԁ@�֐�
	 */
	//-----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�e�̏�񂪓͂��܂ő҂�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoEntryOyaWaitInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
#if 0 ///COMMINFO���l������@@OO
		CommInfoSendPokeData();	// Info�f�[�^���M
#endif

		NET_PRINT( "�q�@�ʐM�f�[�^���M\n" );

		GFL_NET_SetNoChildErrorCheck(FALSE);
		GFL_NET_SetAutoErrorCheck(TRUE);

#if 1//PL_G0254_081217_FIX
		// �e�܂��^�C���A�E�g�J�n
		WFP2PMF_TimeOut_Start( p_wk );
#endif

		p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_OYAWAIT;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�e�̏�񂪓͂��܂ő҂�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoEntryOyaWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{


		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );

		// �e����ؒf���ꂽ�炦��[������
		if( GFL_NET_IsConnectMember(GFL_NET_NETID_SERVER) == FALSE ){
#if 1//PL_G0254_081217_FIX
			// �e�܂��^�C���A�E�g��~
			WFP2PMF_TimeOut_Stop( p_wk );
#endif
			p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
		}else{


#if 0 ///COMMINFO���l������@@OO

			// �e�̃f�[�^�����ł���̂͑҂�
			if( CommInfoGetEntry( COMM_PARENT_ID ) ){
#if 1//PL_G0254_081217_FIX
				// �e�܂��^�C���A�E�g��~
				WFP2PMF_TimeOut_Stop( p_wk );
#endif
				p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_INIT;
			}
#endif
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�G���g���[�҂��@������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoEntryInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{

		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );

		// ���b�Z�[�W
		WFP2PMF_WordSetPlayerNameSet( &p_wk->draw, p_init, GFL_NET_NO_PARENTMACHINE, 0, heapID );
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_118, heapID );

		// �҂��A�C�R���\��
		WFP2PMF_TimeWaitIconOn( p_wk );

		p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_WAIT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�G���g���[�҂�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoEntryWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		if( p_wk->data.entry != WFP2PMF_ENTRY_WAIT ){

			// WaitIcon������
			WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

			switch( p_wk->data.entry ){
			case WFP2PMF_ENTRY_KO:
				p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_OKINIT;
				break;
			case WFP2PMF_ENTRY_NG:
				p_wk->data.status		= WFP2PMF_KO_STATUS_ENTRY_NGINIT;
				p_wk->data.ng_msg_idx	= msg_wifilobby_130;
				break;
			case WFP2PMF_ENTRY_LOCK:
				p_wk->data.status		= WFP2PMF_KO_STATUS_ENTRY_NGINIT;
				p_wk->data.ng_msg_idx	= msg_wifilobby_136;

				// 080707	tomoya
				// �Đڑ��́A��������N���u��ʂɂ��ǂ��Ă���ƂȂ�܂����B
				//			p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_INIT00;
				break;
			}
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�G���g���[�����肪�Ƃ�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoEntryOkInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// ���b�Z�[�W
		WFP2PMF_WordSetPlayerNameSet( &p_wk->draw, p_init, GFL_NET_NO_PARENTMACHINE, 0, heapID );
		WFP2PMF_GraphicMsgBmpStrPutEx( &p_wk->draw, p_init, msg_wifilobby_129, WFP2PMF_MSGENDWAIT, heapID );

		p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_OK;

		// �������G���g���[�ɂ���
		WFP2PMF_CommInfoSetEntry( p_wk, GFL_NET_HANDLE_GetNetHandleID(GFL_NET_HANDLE_GetCurrentHandle()), HEAPID_WIFI_FOURMATCH );

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�G���g���[�����肪�Ƃ�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoEntryOk( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		p_wk->data.status = WFP2PMF_KO_STATUS_CONNECT_INIT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�G���g���[���~��Ȃ������Ƃ�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoEntryNgInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// ���b�Z�[�W
		WFP2PMF_WordSetPlayerNameSet( &p_wk->draw, p_init, GFL_NET_NO_PARENTMACHINE, 0, heapID );
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, p_wk->data.ng_msg_idx, heapID );

		p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_NG;

		// 080703	tomoya	�ʐM�I���́A���b�Z�[�W�\�����ɍs��
		// �}�b�`���O������
		GFL_NET_StateWifiMatchEnd(TRUE);

		// �ʐM��Ԃ����ɖ߂�
		//	CommStateChangeWiFiLogin();  @@OO

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�G���g���[���~��Ȃ������Ƃ�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoEntryNg( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}


		// �I������
		p_wk->data.match_result = FALSE;

		GFL_NET_SetNoChildErrorCheck(FALSE);
		GFL_NET_SetAutoErrorCheck(TRUE);

		return TRUE;

	}


	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�R�l�N�g�J�n
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConnectInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{

		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );

		// ���b�Z�[�W
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_126, heapID );

		// �҂��A�C�R���\��
		WFP2PMF_TimeWaitIconOn( p_wk );

		p_wk->data.status = WFP2PMF_KO_STATUS_CONNECT_WAIT;

		// VCHATONOFF
		if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){

			WFP2PMF_StatusVChatOnBmpFade( p_wk );
		}


		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@���b�Z�[�W�I���҂�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConnectWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}


		p_wk->data.status = WFP2PMF_KO_STATUS_START;

		return FALSE;

	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�����҂�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{

		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );

		OS_TPrintf( "Ko start \n" );

		// �V�����l���Ȃ�������VCHAT���Z�b�g
		if( p_wk->data.ko_newcon_flag ){
			p_wk->data.ko_newcon_flag = FALSE;

			// Vchat���Ȃ�
			if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){
				// �{�C�X�`���b�gOFF
				WFP2PMF_StatusVChatOff( p_wk );

				// �{�C�X�`���b�gOPOFIN_WAITN
				WFP2PMF_StatusVChatOnBmpFade( p_wk );
			}

			// �R�l�N�g���X�g�ĕ`��
			WFP2PMF_ConListWriteReq( p_wk );
		}

		if( WFP2PMF_MatchOkCheck( p_wk ) == TRUE ){
			//		CommToolTempDataReset();  @@OO
			GFL_NET_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_GAME_CHECK2);

			WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

			// �ʐM�ҋ@���b�Z�[�W�\��
			WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_066, heapID );

			// �^�C���E�G�C�g�J�n
			WFP2PMF_TimeOut_Start( p_wk );

			// �q�@���̐l���`�F�b�N�E�G�C�g
			p_wk->data.timer = KO_ENTRYNUM_CHECK_WAIT;

			// �҂��A�C�R���\��
			WFP2PMF_TimeWaitIconOn( p_wk );

			p_wk->data.status = WFP2PMF_KO_STATUS_STATE_SEND;

		}else{

			// VChatOnOff
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){

				// �҂��A�C�R����\��
				WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );
				p_wk->data.status = WFP2PMF_KO_STATUS_VCHAT_INIT00;
			}
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�X�e�[�^�X���M
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoStateSend( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int con_num;

		if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_GAME_CHECK2)){
			u16 status = p_wk->data.p_match->myMatchStatus.status;
			BOOL result=TRUE;

			// �G���g���[�����擾
			con_num = WFP2PMF_GetEntryNum( p_wk );

			if(GFL_NET_GetConnectNum()!=con_num){
				p_wk->data.timer--;
				if( p_wk->data.timer <= 0 ){
					// �������ؒf������
					p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
				}
				return FALSE;
			}


			//        result = CommToolSetTempData(GFL_NET_SystemGetCurrentID() ,&status);  @@OO
			if( result == TRUE ){
				p_wk->data.status = WFP2PMF_KO_STATUS_STATE_CHECK;
			}

		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�X�e�[�^�X�`�F�b�N
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoStateCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int i;
		int con_num;
		int current_id;
		const u16* cp_data=NULL;
		int ok_num;

		// �G���g���[�����擾
		con_num = WFP2PMF_GetEntryNum( p_wk );

		current_id = GFL_NET_SystemGetCurrentID();

		ok_num = 0;

		for( i=0; i<con_num; i++ ){

			if( current_id != i ){

				// �X�e�[�^�X���ꏏ���`�F�b�N
				//			cp_data = CommToolGetTempData( i );	  //@@OO

				// �X�e�[�^�X�̎�M���������Ă��邩�`�F�b�N
				if( cp_data != NULL ){
					if( cp_data[ 0 ] == p_wk->data.p_match->myMatchStatus.status ){
						ok_num ++;
					}else{
						// �������ؒf������
						p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
					}
				}
			}else{
				// ������OK
				ok_num ++;
			}
		}

		// �S���̃X�e�[�^�X����M���ă`�F�b�N���ʂ�OK�Ȃ玟�̏�����
		if( ok_num == con_num ){
			p_wk->data.status = WFP2PMF_KO_STATUS_MYSTATUS_WAIT;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�X�e�[�^�X�n���M
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoMyStatusWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		//	CommInfoSendPokeData();
		GFL_NET_SetNoChildErrorCheck(TRUE);
		GFL_NET_SetAutoErrorCheck(TRUE);
		//	CommTimingSyncStart(_TIMING_GAME_START2);
		GFL_NET_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_GAME_START2);
		p_wk->data.status = WFP2PMF_KO_STATUS_GAME_START;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@	�Q�[���J�n
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoGameStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_GAME_CHECK2)){

			// �҂��A�C�R����\��
			WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

			// �����J�n�Ȃ�
			p_wk->data.match_result = TRUE;

			return TRUE;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�G���[����	������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoErrInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{

		// YesNo�̃��[�N����������j��
		if( p_wk->draw.p_yesnowin != NULL ){
			BmpMenu_YesNoMenuExit( p_wk->draw.p_yesnowin );
			p_wk->draw.p_yesnowin = NULL;
		}
		WFP2PMF_GraphicNewConStrOff( &p_wk->draw );

		// �҂��A�C�R���\��
		WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );


		p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT01;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�G���[����	������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoErrInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{

		// ���b�Z�[�W�\��	���̃����o�[�ł�낵���ł����H
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_120, heapID );

		p_wk->data.status = WFP2PMF_KO_STATUS_ERR;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�G���[����
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoErr( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// �}�b�`���O������
		GFL_NET_StateWifiMatchEnd(TRUE);


		// �ʐM��Ԃ����ɖ߂�
		//	CommStateChangeWiFiLogin(); @@OO

		// �I������
		p_wk->data.match_result = FALSE;

		GFL_NET_SetNoChildErrorCheck(FALSE);
		GFL_NET_SetAutoErrorCheck(TRUE);

		return TRUE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�{�C�X�`���b�gONOFF
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoVchatInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		u32 idx;

		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );


		if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){
			idx = msg_wifilobby_128;
		}else{
			idx = msg_wifilobby_127;
		}

		// ���b�Z�[�W�\��	���̃����o�[�ł�낵���ł����H
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, idx, heapID );

		p_wk->data.status = WFP2PMF_KO_STATUS_VCHAT_INIT01;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�{�C�X�`���b�gONOFF
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoVchatInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;


		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );


		// ��W���I�������VCHAT�ύX������CANCEL
		if( WFP2PMF_MatchOkCheck( p_wk ) == TRUE ){
			p_wk->data.status = WFP2PMF_KO_STATUS_START;
			// ���b�Z�[�W�\��OFF
			WFP2PMF_GraphicMsgBmpStrOff( &p_wk->draw );
			return FALSE;
		}

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// YESNO�E�B���h�E���o��
		p_wk->draw.p_yesnowin = BmpMenu_YesNoSelectInit(
			&WFP2PMF_YESNOBMPDAT,
			WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,0,
			heapID );
		p_wk->data.status = WFP2PMF_KO_STATUS_VCHAT_SELECT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�q�@�{�C�X�`���b�gONOFF
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoVchatSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int yesno_ret;

		// �������ႢNETID�̐l�̃f�[�^����M������
		// �G���g���[��Ԃɂ���
		WFP2PMF_SetEntryOya( p_wk, heapID );

		// ��W���I�������VCHAT�ύX������CANCEL
		if( WFP2PMF_MatchOkCheck( p_wk ) == TRUE ){
			p_wk->data.status = WFP2PMF_KO_STATUS_START;
			// ���b�Z�[�W�\��OFF
			WFP2PMF_GraphicMsgBmpStrOff( &p_wk->draw );
			BmpMenu_YesNoMenuExit( p_wk->draw.p_yesnowin );
			p_wk->draw.p_yesnowin = NULL;
			return FALSE;
		}

		// YES�Ȃ�R�l�N�g�@No�Ȃ�ؒf
		yesno_ret = BmpMenu_YesNoSelectMain( p_wk->draw.p_yesnowin );
		if( yesno_ret != BMPMENU_NULL ){
			if( yesno_ret == 0 ){	// �͂���I��

				WFP2PMF_VchatSwitch( p_wk, heapID );

				// �R�l�N�g���X�g�ĕ`��
				WFP2PMF_ConListWriteReq( p_wk );

			}
			p_wk->data.status = WFP2PMF_KO_STATUS_CONNECT_INIT;

			// �I��
			p_wk->draw.p_yesnowin = NULL;
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ʐM���b�N���̂��߁@�Đڑ��@�������O�O
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConLockInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{

#if 0 ///COMMINFO���l������@@OO
		// �e�̗F�Bnumber���擾
		p_wk->data.oya_friendid = CommInfoSearchWifiListIndex( GFL_NET_NO_PARENTMACHINE );
		GF_ASSERT( p_wk->data.oya_friendid != WIFILIST_FRIEND_MAX );

		// ���[�h�Z�b�g�ɖ��O�������Ă����Ƃ�
		WFP2PMF_WordSetPlayerNameSet( &p_wk->draw, p_init, GFL_NET_NO_PARENTMACHINE, 0, heapID );
#endif

		GFL_NET_StateWifiMatchEnd(TRUE);

		// �G���[�`�F�b�N�@OFF
		p_wk->data.err_check = FALSE;

		// ���b�Z�[�W�\��	���̃����o�[�ł�낵���ł����H
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_136, heapID );

		p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_INIT01;
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ʐM���b�N���̂��߁@�Đڑ��@�������O�P
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConLockInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;

		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		// YESNO�E�B���h�E���o��
		p_wk->draw.p_yesnowin = BmpMenu_YesNoSelectInit(
			&WFP2PMF_YESNOBMPDAT,
			WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,0,
			heapID );
		p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_SELECT;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ʐM���b�N���̂��߁@�Đڑ��@�I��
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConLockSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		int yesno_ret;


		// YES�Ȃ�R�l�N�g�@No�Ȃ�ؒf
		yesno_ret = BmpMenu_YesNoSelectMain( p_wk->draw.p_yesnowin );
		if( yesno_ret != BMPMENU_NULL ){
			if( yesno_ret == 0 ){	// �͂���I��
				p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_RECON;
			}else{
				p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_DECON;
			}
			// �I��
			p_wk->draw.p_yesnowin = NULL;
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ʐM���b�N���̂��߁@�Đڑ��@�Đڑ�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConLockReCon( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{

		// ���b�Z�[�W�\��	�`����ɂ��������݂��イ�ł�
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_014, heapID );

		// �҂��A�C�R���\��
		WFP2PMF_TimeWaitIconOn( p_wk );

		// �E�G�C�g
		p_wk->data.timer = RECONNECT_WAIT;

		p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_RECON01;

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ʐM���b�N���̂��߁@�Đڑ��@�Đڑ�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConLockReCon01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		BOOL result;


		// �E�G�C�g
		if( p_wk->data.timer > 0 ){
			p_wk->data.timer--;
			return FALSE;
		}
		result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
		// ���b�Z�[�W�\�����I���܂ł܂�
		if( result == FALSE ){
			return FALSE;
		}

		NET_PRINT( "wifi�ڑ��� %d\n", p_wk->data.oya_friendid );

		// �Đڑ�
		if( WFP2PMF_CommWiFiMatchStart( p_wk->data.oya_friendid, p_init->type ) ){

			p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_RECONWAIT;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ʐM���b�N���̂��߁@�Đڑ��@�Đڑ��҂�
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConLockReConWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// �����ł͓Ǝ���ERR�`�F�b�N���s��
		WFP2PMF_ReConErrCheck( p_wk );

		// �ҋ@
		if( (GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED) && (
			GFL_NET_IsConnectMember(GFL_NET_NETID_SERVER) == TRUE) ){

			//
			GFL_NET_SetWifiBothNet(FALSE);

			// �҂��A�C�R���\��
			WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

			// ���C�v�t�F�[�h�J�n
			//		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
			//						COMM_BRIGHTNESS_SYNC, 1, heapID);
			GFL_FADE_SetMasterBrightReq(
				GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
				0, 16, 0);


			p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET00;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ʐM���b�N���̂��߁@�Đڑ��@��ʍč\�z
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConLockReConGraphicReset00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		if( GFL_FADE_CheckFade() ){

			// �܂��͒ʐM�̊J��
			//@@CommInfoInitialize(p_init->p_savedata,NULL);   //Info������

			// �G���[�`�F�b�N�@ON
			p_wk->data.err_check = TRUE;

			// ��ʍč\�z
			{
				// OAM�j������
				WFP2PMF_DrawOamDelete( p_wk );
				// �`��f�[�^�j��
				WFP2PMF_GraphicDelete( p_wk, heapID );

				// �`�惏�[�N���N���[��
				GFL_STD_MemClear( &p_wk->draw, sizeof(WFP2PMF_DRAW) );

				// �ؒf�`�F�b�N�J�n
				WFP2PMF_LogOutCheckStart( p_wk );

				// �`��f�[�^������
				WFP2PMF_GraphicInit( p_wk, p_init, heapID );

				GFL_NET_ReloadIcon();  // �ڑ����Ȃ̂ŃA�C�R���\��
			}

			// ���[�N�̃N���A
			{
				p_wk->data.ko_newcon_flag = 0;
				p_wk->data.entry = 0;
				GFL_STD_MemClear( p_wk->data.logout_in,  sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// LOG�A�E�g���m
				GFL_STD_MemClear( p_wk->data.logout_id,  sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// LOG�A�E�g���m
				GFL_STD_MemFill( p_wk->data.vchat_tmp, TRUE, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// �P�O��VCHAT
				GFL_STD_MemFill( p_wk->data.vchat_now, TRUE, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// �P�O��VCHAT
				p_wk->data.conlist_rewrite = 0;	// conlist���ĕ`��
				p_wk->data.oya_friendid = 0;	// �e�̗F�B�R�[�h
				GFL_STD_MemClear( p_wk->data.entry_on, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// �G���g���[�t���O
			}

			// ���C�v�t�F�[�h�J�n
			//        WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
			//                        COMM_BRIGHTNESS_SYNC, 1, heapID);
			GFL_FADE_SetMasterBrightReq(
				GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
				16, 0, 0);

			p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET01;
		}

		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ʐM���b�N���̂��߁@�Đڑ��@��ʍč\�z
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConLockReConGraphicReset01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		if( GFL_FADE_CheckFade() ){
			// �ڑ��J�n
			p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_OYAWAITINIT;
		}
		return FALSE;
	}

	//----------------------------------------------------------------------------
	/**
	 *	@brief	�ʐM���b�N���̂��߁@�Đڑ��@�I������
	 *
	 *	@param	p_wk		���[�N
	 *	@param	p_init		��̃A�v������̃f�[�^
	 *	@param	heapID		�q�[�vID
	 *
	 *	@retval	TRUE	�}�b�`���O�I��
	 *	@retval	FALSE	�}�b�`���O��
	 */
	//-----------------------------------------------------------------------------
	static BOOL WFP2PMF_KoConLockDeCon( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
	{
		// �}�b�`���O������
		GFL_NET_StateWifiMatchEnd(TRUE);


		// �ʐM��Ԃ����ɖ߂�
		//	CommStateChangeWiFiLogin();   @@OO

		// �I������
		p_wk->data.match_result = FALSE;

		GFL_NET_SetNoChildErrorCheck(FALSE);
		GFL_NET_SetAutoErrorCheck(TRUE);

		return TRUE;
	}


	const GFL_PROC_DATA WifiP2PMatchFourProcData = {
		WifiP2PMatchFourProc_Init,
		WifiP2PMatchFourProc_Main,
		WifiP2PMatchFourProc_End,
	};
