//============================================================================================
/**
 * @file	b_bag_main.h
 * @brief	�퓬�p�o�b�O���
 * @author	Hiroyuki Nakamura
 * @date	05.02.10
 */
//============================================================================================
#ifndef B_BAG_MAIN_H
#define B_BAG_MAIN_H

#include "print/wordset.h"
#include "system/palanm.h"
#include "savedata/myitem_savedata.h"
#include "item/item.h"
//#include "itemtool/myitem.h"
//#include "finger_cursor.h"

//============================================================================================
//	�萔��`
//============================================================================================
// �p���b�g��`
//#define	BBAG_PAL_BTN_FONT	( 0 )			///< �p���b�g00�F�{�^���t�H���g�Ŏg�p���Ă܂�
//#define	BPL_PAL_SYS_WIN	( 12 )			///< �p���b�g12�F�V�X�e���E�B���h�E
//#define	BBAG_PAL_SYS_FONT	( 4 )			///< �p���b�g13�F�V�X�e���t�H���g
#define	BBAG_PAL_TALK_WIN		( 11 )		///< �p���b�g11�F��b�E�B���h�E
#define	BBAG_PAL_TALK_FONT	( 12 )		///< �p���b�g12�F��b�t�H���g

// BMP�E�B���h�E�C���f�b�N�X
enum {
	// �y�[�W�P
	WIN_P1_HP = 0,		// �uHP/PP�����ӂ��v
	WIN_P1_ZYOUTAI,		// �u���傤���������ӂ��v
	WIN_P1_BALL,		// �u�{�[���v
	WIN_P1_BATTLE,		// �u����Ƃ��悤�v
	WIN_P1_LASTITEM,	// �u�������ɂ������ǂ����v

	// �y�[�W�Q
	WIN_P2_NAME1,		// ����P
	WIN_P2_NUM1,		// ����P
	WIN_P2_NAME2,		// ����Q
	WIN_P2_NUM2,		// ����Q
	WIN_P2_NAME3,		// ����R
	WIN_P2_NUM3,		// ����R
	WIN_P2_NAME4,		// ����S
	WIN_P2_NUM4,		// ����S
	WIN_P2_NAME5,		// ����T
	WIN_P2_NUM5,		// ����T
	WIN_P2_NAME6,		// ����U
	WIN_P2_NUM6,		// ����U

	WIN_P2_NAME1_S,		// ����P�i�X���b�v�p�j
	WIN_P2_NUM1_S,		// ����P�i�X���b�v�p�j
	WIN_P2_NAME2_S,		// ����Q�i�X���b�v�p�j
	WIN_P2_NUM2_S,		// ����Q�i�X���b�v�p�j
	WIN_P2_NAME3_S,		// ����R�i�X���b�v�p�j
	WIN_P2_NUM3_S,		// ����R�i�X���b�v�p�j
	WIN_P2_NAME4_S,		// ����S�i�X���b�v�p�j
	WIN_P2_NUM4_S,		// ����S�i�X���b�v�p�j
	WIN_P2_NAME5_S,		// ����T�i�X���b�v�p�j
	WIN_P2_NUM5_S,		// ����T�i�X���b�v�p�j
	WIN_P2_NAME6_S,		// ����U�i�X���b�v�p�j
	WIN_P2_NUM6_S,		// ����U�i�X���b�v�p�j

	WIN_P2_POCKET,		// �|�P�b�g��
	WIN_P2_PAGENUM,		// �y�[�W���i�A�C�e�����j

	// �y�[�W�R
	WIN_P3_NAME,		// ���
	WIN_P3_NUM,			// ��
	WIN_P3_INFO,		// ����
	WIN_P3_USE,			// �u�����v

	WIN_MAX
};

// �y�[�WID
enum {
	BBAG_PAGE_POCKET = 0,	// �|�P�b�g�I���y�[�W
	BBAG_PAGE_MAIN,			// �A�C�e���I���y�[�W
	BBAG_PAGE_ITEM			// �A�C�e���g�p�y�[�W
};

// �y�[�W�P�̑I��ID
enum {
	BBAG_P1_LASTITEM = BBAG_POKE_MAX,	// �Ō�Ɏg�p��������
	BBAG_P1_RETURN						// �߂�
};

// �Z���A�N�^�[��ID
enum {
	BBAG_CA_ITEM1 = 0,
	BBAG_CA_ITEM2,
	BBAG_CA_ITEM3,
	BBAG_CA_ITEM4,
	BBAG_CA_ITEM5,
	BBAG_CA_ITEM6,
	BBAG_CA_ITEM7,

	BBAG_CA_COST1,
	BBAG_CA_COST2,
	BBAG_CA_COST3,
	BBAG_CA_COST4,
	BBAG_CA_COST5,
	BBAG_CA_COST6,
	BBAG_CA_ENERGIE,

//	BBAG_CA_GETDEMO,

	BBAG_CA_MAX
};

#define	BBAG_POCKET_IN_MAX		( 36 )		// �e�|�P�b�g�ɓ��鐔


#define	BBAG_BANM_MAX_N	( 3 )		// �{�^���A�j���p�^�[�����i�ʏ�j
#define	BBAG_BANM_MAX_E	( 4 )		// �{�^���A�j���p�^�[�����i�����Ȃ��{�^������j
// �|�P�b�g�{�^���T�C�Y
#define	BBAG_BSX_POCKET	( 16 )
#define	BBAG_BSY_POCKET	( 9 )
// �g�p�{�^���T�C�Y
#define	BBAG_BSX_USE	( 26 )
#define	BBAG_BSY_USE	( 5 )
// �߂�{�^���T�C�Y
#define	BBAG_BSX_RET	( 5 )
#define	BBAG_BSY_RET	( 5 )
// �A�C�e���{�^���T�C�Y
#define	BBAG_BSX_ITEM	( 16 )
#define	BBAG_BSY_ITEM	( 6 )
// �O�̃y�[�W�փ{�^���T�C�Y
#define	BBAG_BSX_UP		( 5 )
#define	BBAG_BSY_UP		( 5 )
// ���̃y�[�W�փ{�^���T�C�Y
#define	BBAG_BSX_DOWN	( 5 )
#define	BBAG_BSY_DOWN	( 5 )
// �|�P�b�g�{�^���̃A�C�R���T�C�Y
#define	BBAG_BSX_ICON	( 4 )
#define	BBAG_BSY_ICON	( 4 )

// OBJ���\�[�X
enum {
	// �L����
	BBAG_CHRRES_ITEM1 = 0,
	BBAG_CHRRES_ITEM2,
	BBAG_CHRRES_ITEM3,
	BBAG_CHRRES_ITEM4,
	BBAG_CHRRES_ITEM5,
	BBAG_CHRRES_ITEM6,
	BBAG_CHRRES_ITEM7,
	BBAG_CHRRES_COST,
	BBAG_CHRRES_MAX,

	// �p���b�g
	BBAG_PALRES_ITEM1 = 0,
	BBAG_PALRES_ITEM2,
	BBAG_PALRES_ITEM3,
	BBAG_PALRES_ITEM4,
	BBAG_PALRES_ITEM5,
	BBAG_PALRES_ITEM6,
	BBAG_PALRES_ITEM7,
	BBAG_PALRES_COST,
	BBAG_PALRES_MAX,

	// �Z���E�A�j��
	BBAG_CELRES_ITEM = 0,
	BBAG_CELRES_COST,
	BBAG_CELRES_MAX,
};

typedef struct {
	BBAG_DATA * dat;		// �O���Q�ƃf�[�^

	GFL_TCBLSYS * tcbl;		// TCBL

	PALETTE_FADE_PTR pfd;	// �p���b�g�t�F�[�h�f�[�^

//	NUMFONT * nfnt;				// 8x8�t�H���g
	GFL_MSGDATA * mman;			// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;					// �P��Z�b�g
	STRBUF * msg_buf;				// ���b�Z�[�W�p�o�b�t�@
	PRINT_QUE * que;				// �v�����g�L���[
	PRINT_STREAM * stream;	// �v�����g�X�g���[��

//	GF_BGL_BMPWIN	talk_win;	// BMP�E�B���h�E�f�[�^�i�ʏ�j
//	GF_BGL_BMPWIN * add_win;	// BMP�E�B���h�E�f�[�^�i�ǉ��j
	GFL_BMPWIN * talk_win;
//	GFL_BMPWIN * add_win[WIN_MAX];
	BAPP_BMPWIN_QUE	add_win[WIN_MAX];

	u8	bmp_add_max;			// �ǉ�����BMP�̐�
	u8	p2_swap;				// �y�[�W�Q�̃X���b�v�`��t���O
//	u8	midx;					// ���b�Z�[�W�C���f�b�N�X

	BAPP_CURSOR_MVWK * cmv_wk;	// �J�[�\�����䃏�[�N

//	FINGER_PTR	finger;			// �ߊl�f���p�w�J�[�\��

	ITEM_ST	pocket[5][BBAG_POCKET_IN_MAX];	// �|�P�b�g�f�[�^

//	CATS_RES_PTR	crp;				// ���\�[�X�Ǘ�
//	CATS_ACT_PTR	cap[BBAG_CA_MAX];	// OAM��CAP
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[BBAG_CA_MAX];

	// �{�^���A�j���X�N���[���f�[�^
	u16	btn_pocket[BBAG_BANM_MAX_N][BBAG_BSX_POCKET*BBAG_BSY_POCKET];	// �|�P�b�g�{�^��
	u16	btn_use[BBAG_BANM_MAX_E][BBAG_BSX_USE*BBAG_BSY_USE];			// �g�p�{�^��
	u16	btn_ret[BBAG_BANM_MAX_N][BBAG_BSX_RET*BBAG_BSY_RET];			// �߂�{�^��
	u16	btn_item[BBAG_BANM_MAX_E][BBAG_BSX_ITEM*BBAG_BSY_ITEM];			// �A�C�e��
	u16	btn_up[BBAG_BANM_MAX_E][BBAG_BSX_UP*BBAG_BSY_UP];				// �O�̃y�[�W��
	u16	btn_down[BBAG_BANM_MAX_E][BBAG_BSX_DOWN*BBAG_BSY_DOWN];			// ���̃y�[�W��

	u16	btn_icon_hp[BBAG_BANM_MAX_N][BBAG_BSX_ICON*BBAG_BSY_ICON];
	u16	btn_icon_st[BBAG_BANM_MAX_N][BBAG_BSX_ICON*BBAG_BSY_ICON];
	u16	btn_icon_ball[BBAG_BANM_MAX_N][BBAG_BSX_ICON*BBAG_BSY_ICON];
	u16	btn_icon_batl[BBAG_BANM_MAX_N][BBAG_BSX_ICON*BBAG_BSY_ICON];

	u8	btn_seq;
	u8	btn_cnt;
	u8	btn_id;
	u8	btn_flg:4;
	u8	btn_mode:4;
	u8	btn_win[8];

	u8	seq;			// ���C���V�[�P���X
	u8	ret_seq;		// ���A�V�[�P���X
	u8	page;			// �y�[�WID
	u8	poke_id;		// �|�P�b�gID

	s8	page_mv;		// �A�C�e���I���y�[�W�̈ړ�����

	u8	item_max[5];	// �A�C�e����
	u8	scr_max[5];		// �A�C�e���X�N���[���J�E���^�ő�l

	u8	get_seq;		// �ߊl�f���V�[�P���X
	u8	get_cnt;		// �ߊl�f���J�E���^
//	u8	get_anm;		// �ߊl�f���J�[�\���A�j���t���O


//	WB�ǉ�
	BAG_CURSOR * cur;		// ��

	u32	chrRes[BBAG_CHRRES_MAX];
	u32	palRes[BBAG_PALRES_MAX];
	u32	celRes[BBAG_CELRES_MAX];
//	u32	anmRes[16];

}BBAG_WORK;

typedef int (*pBBagFunc)(BBAG_WORK*);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �g�p����|�P�������擾
 *
 * @param	wk		���[�N
 *
 * @return	�|�P�����ʒu
 */
//--------------------------------------------------------------------------------------------
int BattleBag_SelMonsNoGet( BBAG_WORK * wk );


#endif	// B_BAG_MAIN_H
