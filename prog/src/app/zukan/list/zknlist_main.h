//============================================================================================
/**
 * @file		zknlist_main.h
 * @brief		�}�Ӄ��X�g��� ���C������
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	���W���[�����FZKNLISTMAIN
 */
//============================================================================================
#pragma once

#include "system/main.h"
#include "print/printsys.h"

#include "../zukan_common.h"
#include "zukanlist.h"
#include "zknlist_bmp_def.h"
#include "zknlist_obj_def.h"


//============================================================================================
//	�萔��`
//============================================================================================

// ����m�ۗp�q�[�v�h�c
#define	HEAPID_ZUKAN_LIST_L		( GFL_HEAP_LOWID(HEAPID_ZUKAN_LIST) )

#define	ZKNLISTMAIN_MBG_PAL_TOUCHBAR	( 14 )
#define	ZKNLISTMAIN_MBG_PAL_FONT			( 15 )

#define	ZKNLISTMAIN_SBG_PAL_FONT			( 15 )

// �^�b�`�o�[��`
#define	ZKNLISTMAIN_TOUCH_BAR_PX		( 0 )
#define	ZKNLISTMAIN_TOUCH_BAR_PY		( 21 )
#define	ZKNLISTMAIN_TOUCH_BAR_SX		( 32 )
#define	ZKNLISTMAIN_TOUCH_BAR_SY		( 3 )



// ���X�g��ʃ��[�N
typedef struct {
	ZUKANLIST_DATA * dat;

	GFL_TCB * vtask;		// TCB ( VBLANK )

//	int	key_repeat_speed;
//	int	key_repeat_wait;

	int	mainSeq;		// ���C���V�[�P���X
	int	nextSeq;		// ���̃V�[�P���X
	int	wipeSeq;		// ���C�v��̃V�[�P���X

	PRINT_UTIL	win[ZKNLISTBMP_WINIDX_MAX];		// BMPWIN

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[ZKNLISTOBJ_IDX_MAX];
	u32	chrRes[ZKNLISTOBJ_CHRRES_MAX];
	u32	palRes[ZKNLISTOBJ_PALRES_MAX];
	u32	celRes[ZKNLISTOBJ_CELRES_MAX];



/*
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

	GFL_ARCUTIL_TRANSINFO	syswinInfo;

	// �͂��E�������֘A
//	TOUCH_SW_SYS * tsw;		// �^�b�`�E�B���h�E
	APP_TASKMENU_ITEMWORK	ynList[2];
	APP_TASKMENU_RES * ynRes;
	APP_TASKMENU_WORK * ynWork;
	u16	ynID;				// �͂��E�������h�c

	BUTTON_ANM_WORK	bawk;				// �{�^���A�j�����[�N

	// �|�P�����A�C�R��
	ARCHANDLE * pokeicon_ah;

	u8	pokeicon_cgx[BOX2OBJ_POKEICON_TRAY_MAX][BOX2OBJ_POKEICON_CGX_SIZE];
	u8	pokeicon_pal[BOX2OBJ_POKEICON_TRAY_MAX];
	u8	pokeicon_id[BOX2OBJ_POKEICON_MAX];
	BOOL	pokeicon_exist[BOX2OBJ_POKEICON_TRAY_MAX];
*/



}ZKNLISTMAIN_WORK;

typedef int (*pZKNLIST_FUNC)(ZKNLISTMAIN_WORK*);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_InitVBlank( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��폜
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_ExitVBlank( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTMAIN_InitVram(void);
extern const GFL_DISP_VRAM * ZKNLISTMAIN_GetVramBankData(void);
extern void ZKNLISTMAIN_InitBg(void);
extern void ZKNLISTMAIN_ExitBg(void);
extern void ZKNLISTMAIN_LoadBgGraphic(void);
