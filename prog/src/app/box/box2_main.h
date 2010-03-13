//============================================================================================
/**
 * @file		box2_main.h
 * @brief		�{�b�N�X��� ���C������
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	���W���[�����FBOX2MAIN
 */
//============================================================================================
#pragma	once

#include "pm_define.h"
#include "savedata/box_savedata.h"
#include "system/palanm.h"
#include "system/bgwinfrm.h"
#include "system/cursor_move.h"
#include "system/bmp_oam.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "app/app_taskmenu.h"
#include "app/box2.h"


//============================================================================================
//	�萔��`
//============================================================================================

// [ HEAPID_BOX_SYS ] ����m�ۗp��`
#define	HEAPID_BOX_SYS_L		( GFL_HEAP_LOWID(HEAPID_BOX_SYS) )
// [ HEAPID_BOX_APP ] ����m�ۗp��`
#define	HEAPID_BOX_APP_L		( GFL_HEAP_LOWID(HEAPID_BOX_APP) )

// BG�p���b�g
#define	BOX2MAIN_BG_PAL_TOUCH_BAR	( 7 )			// �^�b�`�o�[
#define	BOX2MAIN_BG_PAL_YNWIN			( 8 )			// �͂��E�������E�B���h
#define	BOX2MAIN_BG_PAL_SYSWIN		( 10 )		// ���b�Z�[�W�E�B���h�E
#define	BOX2MAIN_BG_PAL_SYSFNT		( 11 )		// ���b�Z�[�W�t�H���g
#define	BOX2MAIN_BG_PAL_SELWIN		( 12 )		// �I���{�^���i�Q�{�j
#define	BOX2MAIN_BG_PAL_WALL1			( 14 )		// �ǎ��P
#define	BOX2MAIN_BG_PAL_WALL2			( 15 )		// �ǎ��Q

#define	BOX2MAIN_BG_PAL_SYSFNT_S	( 15 )		// �T�u��ʃt�H���g�p���b�g

// �I���E�B���h�E��`
#define	BOX2MAIN_SELWIN_CGX_SIZ	( 3 * 4 )
#define	BOX2MAIN_SELWIN_CGX_POS	( 1024 - BOX2MAIN_SELWIN_CGX_SIZ * 2 )
#define	BOX2MAIN_SELWIN_CGX_OFF	( BOX2MAIN_SELWIN_CGX_POS )
// �͂��E�������E�B���h�E��`
#define	BOX2MAIN_YNWIN_CGX_SIZ	( 9 )
#define	BOX2MAIN_YNWIN_CGX_POS	( BOX2MAIN_SELWIN_CGX_POS - BOX2MAIN_YNWIN_CGX_SIZ )

// �^�b�`�o�[��`
#define	BOX2MAIN_TOUCH_BAR_PX		( 0 )
#define	BOX2MAIN_TOUCH_BAR_PY		( 21 )
#define	BOX2MAIN_TOUCH_BAR_SX		( 32 )
#define	BOX2MAIN_TOUCH_BAR_SY		( 3 )

// �|�P�����A�C�R����
#define	BOX2OBJ_POKEICON_H_MAX		( BOX_MAX_COLUMN )
#define	BOX2OBJ_POKEICON_V_MAX		( BOX_MAX_RAW )
#define	BOX2OBJ_POKEICON_TRAY_MAX	( BOX_MAX_POS )
#define	BOX2OBJ_POKEICON_MINE_MAX	( TEMOTI_POKEMAX )
// �g���C�{�莝��
#define	BOX2OBJ_POKEICON_PUT_MAX	( BOX2OBJ_POKEICON_TRAY_MAX + BOX2OBJ_POKEICON_MINE_MAX )
// �g���C�{�莝���{�擾��
#define	BOX2OBJ_POKEICON_MAX		( BOX2OBJ_POKEICON_PUT_MAX + 1 )
// �擾���A�C�R���ʒu
#define	BOX2OBJ_POKEICON_GET_POS	( BOX2OBJ_POKEICON_MAX - 1 )

#define	BOX2OBJ_PI_OUTLINE_MAX		( 8 )

#define	BOX2OBJ_TRAYICON_MAX		( 6 )

// �|�P�����A�C�R���̃L�����T�C�Y
#define	BOX2OBJ_POKEICON_CGX_SIZE	( 0x20 * 4 * 4 )

#define	BOX2MAIN_GETPOS_NONE		( 0xff )	// �|�P�������擾

#define	BOX2MAIN_PPP_GET_MINE		( 0xff )	// BOX2MAIN_PPPGet()�Ŏ莝�����擾����Ƃ��̃g���C��`

//#define	BOX2MAIN_BOXMOVE_FLG		( 0x80 )	// ���̃g���C�ɓ�����Ƃ��̃t���O

#define	BOX2MAIN_TRAY_SCROLL_SPD		( 8 )		// �g���C�X�N���[�����x
#define	BOX2MAIN_TRAY_SCROLL_CNT		( 23 )	// �g���C�X�N���[���J�E���g


// �L�������\�[�XID
enum {
	BOX2MAIN_CHRRES_POKEICON = 0,	// �|�P�����A�C�R��
	BOX2MAIN_CHRRES_POKEGRA = BOX2MAIN_CHRRES_POKEICON + BOX2OBJ_POKEICON_MAX,
	BOX2MAIN_CHRRES_POKEGRA2,
	BOX2MAIN_CHRRES_ITEMICON,
	BOX2MAIN_CHRRES_ITEMICON_SUB,
	BOX2MAIN_CHRRES_TYPEICON,
	BOX2MAIN_CHRRES_TRAYICON = BOX2MAIN_CHRRES_TYPEICON + POKETYPE_MAX,
	BOX2MAIN_CHRRES_BOXOBJ = BOX2MAIN_CHRRES_TRAYICON + BOX2OBJ_TRAYICON_MAX,
	BOX2MAIN_CHRRES_BOX_BAR,
	BOX2MAIN_CHRRES_TOUCH_BAR,
	BOX2MAIN_CHRRES_POKEMARK,
	BOX2MAIN_CHRRES_POKEMARK_SUB,
	BOX2MAIN_CHRRES_POKERUSICON,
	BOX2MAIN_CHRRES_MAX
};
// �p���b�g���\�[�XID
enum {
	BOX2MAIN_PALRES_POKEICON = 0,
	BOX2MAIN_PALRES_POKEGRA,
	BOX2MAIN_PALRES_POKEGRA2,
	BOX2MAIN_PALRES_ITEMICON,
	BOX2MAIN_PALRES_ITEMICON_SUB,
	BOX2MAIN_PALRES_TYPEICON,
	BOX2MAIN_PALRES_TRAYICON,
	BOX2MAIN_PALRES_BOXOBJ,
	BOX2MAIN_PALRES_TOUCH_BAR,
	BOX2MAIN_PALRES_POKEMARK,
	BOX2MAIN_PALRES_POKEMARK_SUB,
	BOX2MAIN_PALRES_POKERUSICON,
	BOX2MAIN_PALRES_MAX
};
// �Z�����\�[�XID
enum {
	BOX2MAIN_CELRES_POKEICON = 0,
	BOX2MAIN_CELRES_POKEGRA,
	BOX2MAIN_CELRES_POKEGRA2,
	BOX2MAIN_CELRES_ITEMICON,
	BOX2MAIN_CELRES_TYPEICON,
	BOX2MAIN_CELRES_TRAYICON,
	BOX2MAIN_CELRES_BOXOBJ,
	BOX2MAIN_CELRES_BOX_BAR,
	BOX2MAIN_CELRES_TOUCH_BAR,
	BOX2MAIN_CELRES_POKEMARK,
	BOX2MAIN_CELRES_POKEMARK_SUB,
	BOX2MAIN_CELRES_POKERUSICON,
	BOX2MAIN_CELRES_MAX
};

// OBJ ID
enum {
	BOX2OBJ_ID_L_ARROW = 0,
	BOX2OBJ_ID_R_ARROW,

	BOX2OBJ_ID_TRAY_CUR,	// �g���C�J�[�\��
	BOX2OBJ_ID_TRAY_NAME,	// �g���C���w�i

	BOX2OBJ_ID_HAND_CURSOR,	// ��J�[�\��
	BOX2OBJ_ID_HAND_SHADOW,	// ��J�[�\���e

	BOX2OBJ_ID_TB_CANCEL,		// �^�b�`�o�[�߂�{�^���i�{�b�N�X���j���[�ցj
	BOX2OBJ_ID_TB_END,			// �^�b�`�o�[�I���{�^���iC-gear�ցj
	BOX2OBJ_ID_TB_STATUS,		// �^�b�`�o�[�X�e�[�^�X�{�^��

	BOX2OBJ_ID_ITEMICON,
	BOX2OBJ_ID_ITEMICON_SUB,

	BOX2OBJ_ID_POKEGRA,
	BOX2OBJ_ID_POKEGRA2,

	BOX2OBJ_ID_MARK1,
	BOX2OBJ_ID_MARK2,
	BOX2OBJ_ID_MARK3,
	BOX2OBJ_ID_MARK4,
	BOX2OBJ_ID_MARK5,
	BOX2OBJ_ID_MARK6,

	BOX2OBJ_ID_MARK1_S,
	BOX2OBJ_ID_MARK2_S,
	BOX2OBJ_ID_MARK3_S,
	BOX2OBJ_ID_MARK4_S,
	BOX2OBJ_ID_MARK5_S,
	BOX2OBJ_ID_MARK6_S,

	BOX2OBJ_ID_RARE,
	BOX2OBJ_ID_POKERUS,
	BOX2OBJ_ID_POKERUS_ICON,

	BOX2OBJ_ID_TYPEICON,

	BOX2OBJ_ID_TRAYICON = BOX2OBJ_ID_TYPEICON + POKETYPE_MAX,

	BOX2OBJ_ID_POKEICON = BOX2OBJ_ID_TRAYICON + BOX2OBJ_TRAYICON_MAX,

	BOX2OBJ_ID_OUTLINE = BOX2OBJ_ID_POKEICON + BOX2OBJ_POKEICON_MAX,

	BOX2OBJ_ID_MAX = BOX2OBJ_ID_OUTLINE + BOX2OBJ_PI_OUTLINE_MAX
};

// BMPWIN ID
enum {
	// ����
	BOX2BMPWIN_ID_NAME = 0,		// �|�P������
	BOX2BMPWIN_ID_NICKNAME,		// �j�b�N�l�[��
	BOX2BMPWIN_ID_LV,					// ���x��
	BOX2BMPWIN_ID_TITLE,			// �^�C�g��
	BOX2BMPWIN_ID_SEX,				// ����
	BOX2BMPWIN_ID_ZKNNUM,			// �}�Ӕԍ�
	BOX2BMPWIN_ID_SEIKAKU,		// ���i
	BOX2BMPWIN_ID_TOKUSEI,		// ����
	BOX2BMPWIN_ID_ITEM,				// ������
	BOX2BMPWIN_ID_WAZA,				// �Z

	BOX2BMPWIN_ID_WAZA_STR,			//�u�����Ă���킴�v
	BOX2BMPWIN_ID_SEIKAKU_STR,	//�u���������v
	BOX2BMPWIN_ID_TOKUSEI_STR,	//�u�Ƃ������v
	BOX2BMPWIN_ID_ITEM_STR,			//�u�������́v

//	BOX2BMPWIN_ID_ITEMLABEL,	// �A�C�e�����i�����E�B���h�E�̃^�u�����j
//	BOX2BMPWIN_ID_ITEMINFO,		// �A�C�e������
//	BOX2BMPWIN_ID_ITEMLABEL2,	// �A�C�e�����i�����E�B���h�E�̃^�u�����j�i�X���b�v�p�j
//	BOX2BMPWIN_ID_ITEMINFO2,	// �A�C�e�������i�X���b�v�p�j

	// �����
	BOX2BMPWIN_ID_MENU1,		// ���j���[�P
	BOX2BMPWIN_ID_MENU2,		// ���j���[�Q
	BOX2BMPWIN_ID_MENU3,		// ���j���[�R
	BOX2BMPWIN_ID_MENU4,		// ���j���[�S
	BOX2BMPWIN_ID_MENU5,		// ���j���[�T
	BOX2BMPWIN_ID_MENU6,		// ���j���[�U

	BMPWIN_MARK_ENTER,			// �}�[�L���O����
	BMPWIN_MARK_CANCEL,			// �}�[�L���O�L�����Z��

	BOX2BMPWIN_ID_TEMOCHI,	//�u�Ă����|�P�����v
	BOX2BMPWIN_ID_BOXLIST,	//�u�{�b�N�X���X�g�v

	BOX2BMPWIN_ID_MSG1,			// ���b�Z�[�W�P
	BOX2BMPWIN_ID_MSG2,			// ���b�Z�[�W�Q
	BOX2BMPWIN_ID_MSG3,			// ���b�Z�[�W�R
	BOX2BMPWIN_ID_MSG4,			// ���b�Z�[�W�S

	BOX2BMPWIN_ID_MAX
};

// BGWINFRAME
enum {
	BOX2MAIN_WINFRM_MENU1 = 0,	// ���j���[�t���[��
	BOX2MAIN_WINFRM_MENU2,			// ���j���[�t���[��
	BOX2MAIN_WINFRM_MENU3,			// ���j���[�t���[��
	BOX2MAIN_WINFRM_MENU4,			// ���j���[�t���[��
	BOX2MAIN_WINFRM_MENU5,			// ���j���[�t���[��
	BOX2MAIN_WINFRM_MENU6,			// ���j���[�t���[��

	BOX2MAIN_WINFRM_TOUCH_BAR,	// �^�b�`�o�[

	BOX2MAIN_WINFRM_MARK,				// �}�[�L���O�t���[��
	BOX2MAIN_WINFRM_PARTY,			// �莝���|�P�����t���[��
	BOX2MAIN_WINFRM_MOVE,				// �{�b�N�X�ړ��t���[��

	BOX2MAIN_WINFRM_POKE_BTN,			//�u�Ă����|�P�����v�{�^��
	BOX2MAIN_WINFRM_BOXLIST_BTN,	//�u�{�b�N�X���X�g�v�{�^��

//	BOX2MAIN_WINFRM_RET_BTN,	//�u���ǂ�v�{�^��
//	BOX2MAIN_WINFRM_CLOSE_BTN,	//�u�Ƃ���v�{�^��
//	BOX2MAIN_WINFRM_BOXMV_MENU,	// �{�b�N�X�ړ����̃��j���[
//	BOX2MAIN_WINFRM_BOXMV_BTN,	// �{�b�N�X�ړ����̃{�^��
//	BOX2MAIN_WINFRM_Y_ST_BTN,	// �x�{�^���X�e�[�^�X

//	BOX2MAIN_WINFRM_SUBDISP,	// ���ʗp�i�Z�E�������j

	BOX2MAIN_WINFRM_MAX,
};

// �n�`�l�t�H���g
enum {
	BOX2MAIN_FNTOAM_TRAY_NAME = 0,
	BOX2MAIN_FNTOAM_TRAY_NUM1,
	BOX2MAIN_FNTOAM_TRAY_NUM2,
	BOX2MAIN_FNTOAM_TRAY_NUM3,
	BOX2MAIN_FNTOAM_TRAY_NUM4,
	BOX2MAIN_FNTOAM_TRAY_NUM5,
	BOX2MAIN_FNTOAM_TRAY_NUM6,
	BOX2MAIN_FNTOAM_BOX_NAME1,
	BOX2MAIN_FNTOAM_BOX_NAME2,
	BOX2MAIN_FNTOAM_MAX
};

// VBLANK�֐����[�N
typedef struct {
	void * func;
	void * freq;
	void * work;
	u16	seq;
	u16	cnt;
//	BOOL flg;		// �ғ��t���O
}BOX2_IRQWK;

// �n�`�l�t�H���g���[�N
typedef struct {
	BMPOAM_ACT_PTR oam;
	GFL_BMP_DATA * bmp;
//	CHAR_MANAGER_ALLOCDATA	cma;
}BOX2_FONTOAM;

// �{�^���A�j�����[�h
enum {
	BOX2MAIN_BTN_ANM_MODE_OBJ = 0,
	BOX2MAIN_BTN_ANM_MODE_BG
};

// �{�^���A�j�����[�N
typedef struct {
	u8	btn_mode:1;
	u8	btn_id:7;			// OBJ = ID, BG = FRM
	u8	btn_pal1:4;
	u8	btn_pal2:4;
	u8	btn_seq;
	u8	btn_cnt;

	u8	btn_px;			// BG�̂�
	u8	btn_py;
	u8	btn_sx;
	u8	btn_sy;
}BUTTON_ANM_WORK;

// �{�b�N�X��ʃA�v�����[�N
typedef struct {
	// �n�`�l�t�H���g
	BMPOAM_SYS_PTR	fntoam;
	BOX2_FONTOAM	fobj[BOX2MAIN_FNTOAM_MAX];

//	u16	obj_trans_stop;		// VBLANK�ł�OBJ�`����~����t���O ( 1 = stop )

	int	key_repeat_speed;
	int	key_repeat_wait;

	GFL_TCB * vtask;					// TCB ( VBLANK )

	BOX2_IRQWK	vfunk;				// VBLANK�֐����[�N
	int	vnext_seq;

	PALETTE_FADE_PTR	pfd;		// �p���b�g�t�F�[�h�f�[�^

	CURSORMOVE_WORK * cmwk;		// �J�[�\���ړ����[�N
	BGWINFRM_WORK * wfrm;			// �E�B���h�E�t���[��

	GFL_FONT * font;					// �ʏ�t�H���g
	GFL_FONT * nfnt;					// 8x8�t�H���g
	GFL_MSGDATA * mman;				// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;						// �P��Z�b�g
	STRBUF * exp;							// ���b�Z�[�W�W�J�̈�
	PRINT_QUE * que;					// �v�����g�L���[
	PRINT_STREAM * stream;		// �v�����g�X�g���[��

	PRINT_UTIL	win[BOX2BMPWIN_ID_MAX];		// BMPWIN

	GFL_ARCUTIL_TRANSINFO	syswinInfo;

	BOOL	zenkokuFlag;
	u16 * localNo;

	// �͂��E�������֘A
//	TOUCH_SW_SYS * tsw;		// �^�b�`�E�B���h�E
	APP_TASKMENU_ITEMWORK	ynList[2];
	APP_TASKMENU_RES * ynRes;
	APP_TASKMENU_WORK * ynWork;
	u16	ynID;				// �͂��E�������h�c

	BUTTON_ANM_WORK	bawk;				// �{�^���A�j�����[�N

//	ARCHANDLE * ppd_ah;				// �p�[�\�i���f�[�^�̃A�[�N�n���h��

	// �|�P�����A�C�R��
	ARCHANDLE * pokeicon_ah;

	u8	pokeicon_cgx[BOX2OBJ_POKEICON_TRAY_MAX][BOX2OBJ_POKEICON_CGX_SIZE];
	u8	pokeicon_pal[BOX2OBJ_POKEICON_TRAY_MAX];
	u8	pokeicon_id[BOX2OBJ_POKEICON_MAX];
	BOOL	pokeicon_exist[BOX2OBJ_POKEICON_TRAY_MAX];

	// �g���C�A�C�R���̃L�����f�[�^
	u8	trayicon_cgx[BOX_MAX_TRAY][0x40*4*4];

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[BOX2OBJ_ID_MAX];
	u32	chrRes[BOX2MAIN_CHRRES_MAX];
	u32	palRes[BOX2MAIN_PALRES_MAX];
	u32	celRes[BOX2MAIN_CELRES_MAX];

	u16	pokegra_swap;		// ���ʃ|�P�����O���t�B�b�N�ؑփt���O

	u32	old_cur_pos;		// �O��̃J�[�\���ʒu�i�O���ŕK�v�Ȃ��߁j

	u16	get_item;						// �擾�A�C�e��
	u16	get_item_init_pos;	// �A�C�e���擾�ʒu

//	u8	subdisp_win_swap:4;		// �T�u���BMPWIN�̃X���b�v�t���O
//	u8	subdisp_waza_put:3;		// �T�u��ʋZBMPWIN�̃X���b�v�t���O
//	u8	poke_free_err:1;			// �����������s�ł��Ȃ��Ƃ��P
	u8	poke_free_err;			// �����������s�ł��Ȃ��Ƃ�

//	u8	party_frm;			// �莝���|�P�����t���[������

	u8	mode_chg_flg;			// �h���b�O���̃��[�h�ؑ֗L�������t���O 0 = �L��, 1 = ����

	u8	msg_put;				// ���b�Z�[�W��\�����邩
	u8	mv_err_code;		// �|�P�����ړ��G���[�R�[�h

//	BOOL poke_mv_cur_flg;	// �|�P�����A�C�R���ړ���ɃA�E�g���C����\�����邩

	u32	pokeMark;			// �}�[�L���O�ύX���[�N

	// �{�b�N�X�؂�ւ��֘A
	s8	wall_px;
	u8	wall_area;

	u8	wallpaper_pos;	// �ǎ��I���ʒu
	u8	wp_menu;				// �ǎ����j���[�ԍ�
	u8	wp_special;			// �ǉ��ǎ��t���O

	u8	poke_put_key;		// �L�[�ł̔z�u�ʒu
	u8	chg_tray_pos;		// �؂�ւ���g���C

	u32	tpx;				// �^�b�`�w���W
	u32	tpy;				// �^�b�`�x���W

	int	wipe_seq;							// ���C�v��̃V�[�P���X
	int	wait;									// �E�F�C�g

	void * seqwk;			// �V�[�P���X���[�N
	int	sub_seq;			// �V�[�P���X���V�[�P���X

}BOX2_APP_WORK;

// �{�b�N�X��ʃV�X�e�����[�N
typedef struct {

/*
	BOX_DATA * box;			// �{�b�N�X�f�[�^
	POKEPARTY * party;		// �莝���|�P����
	MYITEM * item;			// �A�C�e���f�[�^
	CONFIG * config;		// �R���t�B�O�f�[�^


	u8	pos;				// ���݂̃J�[�\���ʒu
*/


	BOX2_GFL_PROC_PARAM * dat;	// �O���f�[�^

	GFL_PROCSYS * localProc;						// ���[�J���o�q�n�b
	GFL_PROC_MAIN_STATUS	procStatus;		// ���[�J���o�q�n�b�̖߂�l

	void * subProcWork;				// �T�u�v���Z�X�Ŏg�p���郏�[�N
	u16	subRet;								// �T�u�v���Z�X�̖߂�l�Ȃǂ�ۑ�����ꏊ
	u8	subProcType;					// �T�u�v���Z�X�̎��
	u8	subProcMode;					// �T�u�v���Z�X�̌Ăяo�����[�h

	u8	tray;										// ���݂̃g���C
	u8	trayMax;								// �J������Ă���g���C��

//	POKEMON_PARAM * getPP;			// �擾�����|�P�������R�s�[���Ă����̈�

	u8	get_pos;								// �擾�ʒu
	u8	get_tray;								// �擾�g���C
	u8	poke_get_key;						// �͂�ł��邩

	u8	box_mv_pos;							// �{�b�N�X�ړ��I���ʒu
//	u8	box_mv_scroll;

	u8	move_mode;				// �|�P�����ړ����[�h
	u8	get_start_mode;		// �A�C�R����͂񂾂Ƃ��̃��[�h 0 = �g���C����, 1 = �莝������

	u8	mv_cnv_mode:4;		// �֗����[�h�t���O 1 = yes, 0 = no
	u8	mv_cnv_end:4;			// �͂ޏI���t���O

//	u8	quick_mode;			//�u�|�P�������ǂ��v�̃��[�h ( 0 = �ʏ�, 1 = �g���C�|�P����, 2 = �莝���|�P���� )
//	u8	quick_get;			//�u�|�P�������ǂ��v���j���[�ł̎擾�ʒu

//	u8	y_status_flg;		// �x�{�^���ŃX�e�[�^�X���J�������ǂ���
//	u8	y_status_hand;		// �x�X�e�[�^�X���ɒ͂�ł��邩
	u8	tb_status_pos;		// �͂�ł���Ƃ��̃X�e�[�^�X��ʂ���̖߂�ʒu

	u8	pokechg_mode;		// ����ւ����[�h

//	u8	party_sel_flg;		// �莝���I���t���O
//	u8	box_mv_flg;			// �{�b�N�X�ړ��t���O

	u8	tb_ret_btn;			// �^�b�`�o�[�ɕ\�����Ă���߂�{�^���̕\����
	u8	tb_exit_btn;		// �^�b�`�o�[�ɕ\�����Ă���I���{�^���̕\����
	u8	tb_status_btn;	// �^�b�`�o�[�ɕ\�����Ă���X�e�[�^�X�{�^���̕\����

	u32	cur_rcv_pos;		// �J�[�\���\�����A�ʒu

	int	next_seq;								// ���̃V�[�P���X

	BOX2_APP_WORK * app;				// �A�v�����[�N

}BOX2_SYS_WORK;

typedef int (*pBOX2_FUNC)(BOX2_SYS_WORK*);


/*
// �L�[����f�[�^�擾��`
enum {
	BOX2MAIN_KEY_JUMP_UP = 0,
	BOX2MAIN_KEY_JUMP_DOWN,
	BOX2MAIN_KEY_JUMP_LEFT,
	BOX2MAIN_KEY_JUMP_RIGHT,
};

// �ꏊ�^�C�v�擾��`
enum {
	BOX2MAIN_POS_TYPE_BOXNAME = 0,
	BOX2MAIN_POS_TYPE_SCROLL_L,
	BOX2MAIN_POS_TYPE_SCROLL_R,
	BOX2MAIN_POS_TYPE_BOXPOKE,
	BOX2MAIN_POS_TYPE_PARTYPOKE,
};
*/
// �{�b�N�X�؂�ւ�����
enum {
	BOX2MAIN_TRAY_SCROLL_L = 0,
	BOX2MAIN_TRAY_SCROLL_R,
	BOX2MAIN_TRAY_SCROLL_NONE,
};

// �|�P�����𓦂��������̃��[�N
typedef struct {
	GFL_CLWK * clwk;		// �A�C�R���Z���A�N�^�[
	u16	check_cnt;			// �Z�`�F�b�N�J�E���g
	u8	check_flg;			// �Z�`�F�b�N�t���O ( 0xff = def, 0 = none, 1 = hit )
	u8	scale_cnt;			// �g�k�J�E���g
	f32	scale;				// �g�k�l
}BOX2MAIN_POKEFREE_WORK;

// �莝���t���[�������`
#define	BOX2MAIN_PARTYPOKE_FRM_SPD	( 8 )		// �ړ���
#define	BOX2MAIN_PARTYPOKE_FRM_CNT	( 18 )		// �J�E���g

#define	BOX2MAIN_PARTYPOKE_FRM_H_SPD	( 8 )	// �莝���|�P�����t���[�����X�N���[�����x
#define	BOX2MAIN_PARTYPOKE_FRM_H_CNT	( 19 )	// �莝���|�P�����t���[�����X�N���[���J�E���g

// �|�P�����ړ����[�h
enum {
	BOX2MAIN_POKEMOVE_MODE_TRAY = 0,
	BOX2MAIN_POKEMOVE_MODE_PARTY,
	BOX2MAIN_POKEMOVE_MODE_ALL,
};

// �擾�|�P�����ړ�
typedef struct {
	u16	df_pos;		// �ړ�������A�C�R���̃f�[�^�ʒu
	u16	mv_pos;		// �ړ���̉�ʈʒu
	u16	gt_pos;		// �ړ����̉�ʈʒu
	u16	flg;
	u32	mx;
	u32	my;
	s16	vx;
	s16	vy;
	s16	dx;
	s16	dy;
}BOX2MAIN_POKEMOVE_DATA;

typedef struct {
	BOX2MAIN_POKEMOVE_DATA	dat[BOX2OBJ_POKEICON_MINE_MAX+1];	// �莝���{�g���C�I��
	u32	cnt;
	u32	get_pos;
	u32	put_pos;
	u32	set_pos;
}BOX2MAIN_POKEMOVE_WORK;

// �擾�A�C�e���ړ�
typedef struct {
	u16	put_pos;
	u16	set_pos;
	u32	mv_mode;

	u32	now_x;
	u32	now_y;
	u32	mx;
	u32	my;
	u32	mv_x:1;
	u32	mv_y:1;
	u32	cnt:30;
}BOX2MAIN_ITEMMOVE_WORK;

// �\������Ă���g���C�A�C�R���̃g���C�ԍ��̊�l���擾
#define	BOX2MAIN_MV_TRAY_DEF(a)	( a / BOX2OBJ_TRAYICON_MAX * BOX2OBJ_TRAYICON_MAX )

// �J�[�\���ړ�
typedef struct {
	u8	px;			// �ړ���w���W
	u8	py;			// �ړ���x���W
	u8	vx;			// �w�ړ���
	u8	vy;			// �x�ړ���
	u32	mx:1;		// �w�ړ�����
	u32	my:1;		// �x�ړ�����
	u32	cnt:30;		// �J�E���^
}BOX2MAIN_CURSORMOVE_WORK;

// ���ʕ\���p�f�[�^
typedef struct {
	POKEMON_PASO_PARAM * ppp;

	u16	mons;					// �|�P�����ԍ�
	u16	item;					// �A�C�e���ԍ�

	u32	rand;					// ������

	u8	type1;				// �^�C�v�P
	u8	type2;				// �^�C�v�Q
	u8	tokusei;			// ����
	u8	seikaku;			// ���i

	u16	mark;					// �{�b�N�X�}�[�N
	u8	lv:7;					// ���x��
	u8	tamago:1;			// �^�}�S�t���O
	u8	sex:4;				// ����
	u8	sex_put:1;		// ���ʕ\���i�j�h�����p�j1=ON, 0=OFF
	u8	rare:1;				// ���A
	u8	pokerus:2;		// �|�P���X

	u16	waza[4];			// �Z

}BOX2_POKEINFO_DATA;


//#define	BOX2MAIN_TRAYICON_SCROLL_CNT		( 9 )
#define	BOX2MAIN_TRAYICON_SCROLL_CNT		( 5 )


// �|�P�����ړ��G���[�R�[�h
enum {
	BOX2MAIN_ERR_CODE_NONE = 0,		// �G���[�Ȃ�
	BOX2MAIN_ERR_CODE_BOXMAX,			// �{�b�N�X�������ς�
	BOX2MAIN_ERR_CODE_MAIL,				// ���[���������Ă���
	BOX2MAIN_ERR_CODE_BATTLE,			// �킦��|�P���������Ȃ��Ȃ�
	BOX2MAIN_ERR_CODE_EGG,				// �^�}�S�͂���ׂȂ��i�o�g���{�b�N�X��p�j
	BOX2MAIN_ERR_CODE_ROCK,				// ���b�N����Ă���i�o�g���{�b�N�X��p�j
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_InitVBlank( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��폜
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_ExitVBlank( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM�ݒ�
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VramBankSet(void);

extern const GFL_DISP_VRAM * BOX2MAIN_GetVramBankData(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�O���t�B�b�N�ǂݍ���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgGraphicLoad( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PaletteFadeInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PaletteFadeExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	 �A���t�@�u�����h�ݒ�
 *
 * @param		flg		TRUE = �L��, FALSE = ����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_SetBlendAlpha( BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_MsgInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_MsgExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����f�[�^�擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�ʒu
 * @param		prm			�擾�p�����[�^
 * @param		buf			�擾�o�b�t�@
 *
 * @return	�擾�f�[�^
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2MAIN_PokeParaGet( BOX2_SYS_WORK * syswk, u16 pos, u16 tray, int prm, void * buf );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����f�[�^�ݒ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�ʒu
 * @param		prm			�ݒ�p�����[�^
 * @param		buf			�ݒ�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeParaPut( BOX2_SYS_WORK * syswk, u32 pos, u32 tray, int prm, u32 buf );

//--------------------------------------------------------------------------------------------
/**
 * @brief		POKEMON_PASO_PARAM�擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		tray		�g���C�ԍ�
 * @param		pos			�ʒu
 *
 * @return	POKEMON_PASO_PARAM
 */
//--------------------------------------------------------------------------------------------
extern POKEMON_PASO_PARAM * BOX2MAIN_PPPGet( BOX2_SYS_WORK * syswk, u32 tray, u32 pos );

// �|�P�����f�[�^�擾
//extern void BOX2MAIN_GetPokeData( BOX2_SYS_WORK * syswk, u32 tray, u32 pos );

// �擾�����|�P�����f�[�^��z�u
//extern void BOX2MAIN_PutPokeData( BOX2_SYS_WORK * syswk, u32 tray, u32 pos );


//--------------------------------------------------------------------------------------------
/**
 * @brief		�ǎ��Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		id			�ǎ��h�c
 * @param		mv			�X�N���[������
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_WallPaperSet( BOX2_SYS_WORK * syswk, u32 id, u32 mv );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ǎ��ԍ��擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		num			�g���C�ԍ�
 *
 * @return	�ǎ��ԍ�
 *
 *	�f�t�H���g�ǎ�����̒ʂ��ԍ��ɂ���
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2MAIN_GetWallPaperNumber( BOX2_SYS_WORK * syswk, u32 num );



extern u8 BOX2MAIN_GetBoxMoveTrayNum( BOX2_SYS_WORK * syswk, s8 mv );











//============================================================================================
//	�V�X�e���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * VBLANK����
 *
 * @param	tcb
 * @param	work
 *
 * @return	none
 *
 *	1/60�̂��߂̏���
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VBlank( GFL_TCB* tcb, void * work );
//��[GS_CONVERT_TAG]

//--------------------------------------------------------------------------------------------
/**
 * VBLANK���ŌĂ΂��֐���ݒ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	func	�֐��|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VFuncSet( BOX2_APP_WORK * appwk, void * func );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK���ŌĂ΂��֐���\��
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	func	�֐��|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VFuncReq( BOX2_APP_WORK * appwk, void * func );

//--------------------------------------------------------------------------------------------
/**
 * �\�񂵂�VBLANK�֐����Z�b�g
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VFuncReqSet( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * VRAM�ݒ�
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VramBankSet(void);

//--------------------------------------------------------------------------------------------
/**
 * �a�f������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �a�f���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �a�f�O���t�B�b�N�ǂݍ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgGraphicLoad( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_MsgInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_MsgExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �E�B���h�E�ǂݍ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_WindowLoad( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�t�F�[�h������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PaletteFadeInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�t�F�[�h���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PaletteFadeExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �A���t�@�u�����h�ݒ�
 *
 * @param	flg		TRUE = �L��, FALSE = ����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_SetBlendAlpha( BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * �͂��E����������������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_YesNoWinInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_YesNoWinExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_YesNoWinSet( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * �{�^���A�j������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"TRUE = �A�j����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_ButtonAnmMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�{�^���A�j���Z�b�g
 *
 * @param		syswk			�{�b�N�X��ʃV�X�e�����[�N
 * @param		wfrmID		�a�f�E�B���h�E�t���[���h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_InitBgButtonAnm( BOX2_SYS_WORK * syswk, u32 wfrmID );

//--------------------------------------------------------------------------------------------
/**
 * �L�[�^�b�`�X�e�[�^�X�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none 
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_KeyTouchStatusSet( BOX2_SYS_WORK * syswk );


extern void BOX2MAIN_ResetTouchBar( BOX2_SYS_WORK * syswk );

extern void BOX2MAIN_InitTrayScroll( BOX2_SYS_WORK * syswk, u32 mvID );

extern void BOX2MAIN_InitTrayIconScroll( BOX2_SYS_WORK * syswk, s32 mv );

extern void BOX2MAIN_InitBoxMoveFrameScroll( BOX2_SYS_WORK * syswk );

extern u32 BOX2MAIN_GetTrayScrollDir( BOX2_SYS_WORK * syswk, u32 now, u32 chg );

//--------------------------------------------------------------------------------------------
/**
 * @brief		��p�{�^�����������Ƃ��̃J�[�\���\���؂�ւ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_ChgCursorButton( BOX2_SYS_WORK * syswk );


//============================================================================================
//	�|�P�����f�[�^
//============================================================================================


//--------------------------------------------------------------------------------------------
/**
 * �|�P�����f�[�^�N���A
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	tray	�g���C�ԍ�
 * @param	pos		�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeDataClear( BOX2_SYS_WORK * syswk, u32 tray, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����f�[�^�ړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeDataMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�ȊO�ɐ킦��|�P���������邩
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�ΏۊO�̃|�P�����ʒu
 *
 * @retval	"TRUE = ����"
 * @retval	"FALSE = ���Ȃ�"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_BattlePokeCheck( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * �������ύX�ɂ��t�H�����`�F���W
 *
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @retval	"TRUE = �t�H�����`�F���W"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_PokeItemFormChange( POKEMON_PASO_PARAM * ppp );

//--------------------------------------------------------------------------------------------
/**
 * �莝������{�b�N�X�Ɉړ��������Ƃ��̃V�F�C�~�̃t�H�����`�F���W
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	b_pos	�ړ��O�̈ʒu
 * @param	a_pos	�ړ���̈ʒu
 *
 * @return	none
 *
 *	�f�[�^�͓���ւ������ƂɌĂ�
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_FormChangeSheimi( BOX2_SYS_WORK * syswk, u32 b_pos, u32 a_pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n���}�Ӕԍ��e�[�u���ǂݍ���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_LoadLocalNoList( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n���}�Ӕԍ��e�[�u�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_FreeLocalNoList( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �擾�A�C�e�����w��ꏊ�̃|�P�����ɃZ�b�g�ł��邩�`�F�b�N
 *
 * @param	syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param	get_pos		�擾�ꏊ
 * @param	put_pos		�z�u�ʒu
 *
 * @retval	"TRUE = ��"
 * @retval	"FALSE = �s��"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_PokeItemMoveCheck( BOX2_SYS_WORK * syswk, u32 get_pos, u32 put_pos );


//============================================================================================
//	�|�P�����𓦂���
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�����𓦂����v���[�N�쐬
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeFreeCreate( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�����𓦂����v���[�N���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeFreeExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �������邩�ǂ����Z�`�F�b�N
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	"TRUE = �`�F�b�N��"
 * @return	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_PokeFreeWazaCheck( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �I�����Ɏ莝���Ƀy���b�v�����Ȃ��ꍇ�͂؃^�b�v�{�C�X�̑��݃t���O�𗎂Ƃ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_CheckPartyPerapu( BOX2_SYS_WORK * syswk );


//============================================================================================
//	�f�[�^�\���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �I�𒆂̃|�P�����̏��\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_PokeInfoPut( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * ���ʂ̈ꕔ���ĕ`��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeInfoRewrite( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * ���ʕ\���I�t
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeInfoOff( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ��I���ɐ؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeSelectOff( BOX2_SYS_WORK * syswk );

//============================================================================================
//	�ǎ�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	id		�ǎ��h�c
 * @param	mv		�X�N���[������
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_WallPaperSet( BOX2_SYS_WORK * syswk, u32 id, u32 mv );

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	id		�ǎ��ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_WallPaperChange( BOX2_SYS_WORK * syswk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ԍ��擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	num		�g���C�ԍ�
 *
 * @return	�ǎ��ԍ�
 *
 *	�f�t�H���g�ǎ�����̒ʂ��ԍ��ɂ���
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2MAIN_GetWallPaperNumber( BOX2_SYS_WORK * syswk, u32 num );


//============================================================================================
//	�a�f�t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �a�f�t���[�����[�N�쐬�i�S�́j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgFrameWorkMake( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �a�f�t���[�����[�N���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgFrameWorkExit( BOX2_APP_WORK * appwk );



//============================================================================================
//	�}�[�L���O�t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �}�[�N�L���O�t���[���\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_MarkingFramePut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �}�[�N�\���؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�}�[�N�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_MarkingSwitch( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * �}�[�L���O�t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_MarkingFrameInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �}�[�L���O�t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_MarkingFrameOutSet( BGWINFRM_WORK * wk );


//============================================================================================
//	�莝���|�P�����t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���\��
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFramePut( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���\���i�E�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFramePutRight( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[����\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameOff( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�������ʒu�ݒ�i���j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameInitPutLeft( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�������ʒu�ݒ�i�E�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameInitPutRight( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���E�ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���E�ړ��Z�b�g�i�u�|�P��������������v�p�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_AzukeruPartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�����ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�����ړ��Z�b�g�i�u�|�P��������������v�p�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_AzukeruPartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �a�f�E�B���h�E�t���[���Ɏ莝���|�P�����t���[���O���t�B�b�N���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 * @param	index	�a�f�E�B���h�E�t���[���C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameLoadArrange2( BGWINFRM_WORK * wk, u32 index );

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����{�^���v�{�^���z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_ButtonPutTemochi( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����{�^���v�{�^������ʊO�ɔz�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_ButtonOutPutTemochi( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����{�^���v�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_TemochiButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����{�^���v�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_TemochiButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�������ǂ��v�{�^���z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_ButtonPutIdou( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�������ǂ��v�{�^������ʊO�ɔz�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_ButtonOutPutIdou( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�������ǂ��v�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_IdouButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�������ǂ��v�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_IdouButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �u���ǂ�v�{�^���z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_ButtonPutModoru( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u��߂�v�{�^���z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_ButtonPutYameru( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �L�����Z���{�^������ʊO�ɔz�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_RetButtonOutPut( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �L�����Z���{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_RetButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �L�����Z���{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_RetButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �u�Ƃ���v�{�^���z�u�`�F�b�N
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �z�u�ς�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern BOOL BOX2MAIN_CloseButtonPutCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����v�u�|�P�������ǂ��v�{�^����\��
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxPartyButtonVanish( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^������ʓ��ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PokeMenuOpenPosSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^����\��
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PokeMenuOff( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PokeMenuInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PokeMenuOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�z�u�`�F�b�N
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �z�u�ς�"
 * @retval	"FLASE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern BOOL BOX2MAIN_PokeMenuPutCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX�t���[����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxThemaMenuInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX�t���[����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxThemaMenuOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^���z�u�`�F�b�N
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �z�u�ς�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern BOOL BOX2MAIN_BoxMoveButtonCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_YStatusButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^������ʓ��֔z�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_YStatusButtonPut( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_YStatusButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^���z�u�`�F�b�N
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �z�u�ς�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern BOOL BOX2MAIN_YStatusButtonCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �g���C�I���t���[������ʓ��֔z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 *
 *	����ʂ��畜�A���Ƀ{�b�N�X�ړ��t���[����\��
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveFrmPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �g���C�I���t���[����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveFrmInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �g���C�I���t���[����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveFrmOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_WallPaperFrameInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_WallPaperFrameOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�؂�ւ����j���[�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 *
 * @li	�ǎ��ύX�A�a����{�b�N�X�I���Ŏg�p���Ă܂�
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveMenuInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�؂�ւ����j���[�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 *
 * @li	�ǎ��ύX�A�a����{�b�N�X�I���Ŏg�p���Ă܂�
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveMenuOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX�t���[������ʓ��֔z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 *
 *	����ʂ��畜�A���Ƀ{�b�N�X�e�[�}�ύX�֘A�t���[����\��
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxThemaFrmPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����v�u�|�P�������ǂ��v���\���ɂ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_ArrangeUnderButtonDel( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[���C���Z�b�g
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispWazaFrmInSet( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[���A�E�g�Z�b�g
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispWazaFrmOutSet( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[����ʓ��ɔz�u
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispWazaFrmInPosSet( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[����ʊO�ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_SubDispWazaFrmOutPosSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispItemFrmInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispItemFrmOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[������ʓ��ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispItemFrmInPosSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[������ʊO�ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispItemFrmOutPosSet( BGWINFRM_WORK * wk );


extern void BOX2MAIN_MainDispMarkingChange( BOX2_SYS_WORK * syswk, u32 mark );

//--------------------------------------------------------------------------------------------
/**
 * ���ʂ̃}�[�L���O��ύX
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	maek	�}�[�N�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_SubDispMarkingChange( BOX2_SYS_WORK * syswk, u32 mark );


//============================================================================================
//	�T�u�v���Z�X
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X��ʌĂяo��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_PokeStatusCall( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X��ʏI��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_PokeStatusExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �o�b�O��ʌĂяo��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_BagCall( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �o�b�O��ʏI��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_BagExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ���O���͉�ʌĂяo��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_NameInCall( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ���O���͉�ʏI��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_NameInExit( BOX2_SYS_WORK * syswk );


//============================================================================================
//	VBLANK FUNC
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �擾�|�P�����ړ��i�p�[�e�B�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncGetPokeMoveParty( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �擾�|�P�����ړ��i�p�[�e�B���g���C�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncGetPokeMoveBoxParty( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �莝���𓦂��������OBJ����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyPokeFreeSort( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �|�P�������莝���ɉ����铮��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyInPokeMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �g���C���X�N���[���i���{�^���������ꂽ�Ƃ��j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncTrayScrollLeft( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �g���C���X�N���[���i�E�{�^���������ꂽ�Ƃ��j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncTrayScrollRight( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �|�P�������j���[����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPokeMenuMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ���j���[�E�u�Ă����|�P�����v�E�u�|�P�������ǂ��v�E�u���ǂ�v����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern int BOX2MAIN_VFuncPokeFreeMenuMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �}�[�L���O�t���[������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncMarkingFrameMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �u�Ă����|�P�����v�u�|�P�������ǂ��v�u�Ƃ���v�u���ǂ�v�Ȃǂ̃{�^������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern int BOX2MAIN_VFuncUnderButtonMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �莝���|�P�����t���[������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyFrameMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �莝���|�P�����t���[�����E��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern int BOX2MAIN_VFuncPartyFrmRight( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �{�b�N�X�I���E�B���h�E�C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncBoxMoveFrmIn( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �{�b�N�X�I���E�B���h�E�A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncBoxMoveFrmOut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �擾�|�P�����ړ��i�|�P��������������j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_GetPartyPokeMoveDrop( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �擾�|�P�����ړ��i�|�P��������Ă����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_GetTrayPokeMoveDrop( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ��J�[�\���ړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncCursorMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �߂�������ă��j���[���o��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncMenuMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ���j���[�������Ė߂���o��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern int BOX2MAIN_VFuncMenuCloseKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ���j���[�������Ė߂���o���i�{�b�N�X�����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern int BOX2MAIN_VFuncMenuCloseKeyArrange( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ��J�[�\���Ń|�P�����擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPokeMoveGetKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �|�P���������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPokeMovePutKey( BOX2_SYS_WORK * syswk );
extern int BOX2MAIN_VFuncPokeMovePutKey2( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e���擾�i�����������E�L�[����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeGetKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e���擾�i�����������E�^�b�`����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemGetTouch( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ���j���[�������Ė߂���o���i�����������j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeMenuClose( BOX2_SYS_WORK * syswk );

extern int BOX2MAIN_VFuncItemGetMenuClose( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e���A�C�R������ւ�����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
//extern int BOX2MAIN_VFuncItemIconChange( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �����������E�g���C/�莝���擾�i�^�b�`�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemMoveTouch( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �����������E�g���C/�莝������ւ��i�^�b�`�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemIconChgTouch( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ��J�[�\���ŃA�C�e���擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeBoxPartyIconGetKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e�������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemIconPutKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e��������E�L�����Z��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemIconPutKeyCancel( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �莝���|�P�����̃A�C�e���擾�i�����������E�^�b�`����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemPartyGetTouch( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : �A�C�e���A�C�R������ւ�����i�莝���|�P�����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemIconPartyChange( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 *�|�P�����A�C�R������Ŏ擾������Ԃɂ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"1 = ������"
 * @retval	"0 = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_HandGetPokeSet( BOX2_SYS_WORK * syswk );



extern int BOX2MAIN_VFuncTrayIconScrollUp( BOX2_SYS_WORK * syswk );
extern int BOX2MAIN_VFuncTrayIconScrollDown( BOX2_SYS_WORK * syswk );

extern int BOX2MAIN_VFuncTrayIconScrollUpJump( BOX2_SYS_WORK * syswk );
extern int BOX2MAIN_VFuncTrayIconScrollDownJump( BOX2_SYS_WORK * syswk );

extern int BOX2MAIN_VFuncGetPokeMoveBattleBoxMain( BOX2_SYS_WORK * syswk );
