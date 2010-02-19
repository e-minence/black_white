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
#include "system/bgwinfrm.h"
#include "system/cursor_move.h"
#include "system/palanm.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "ui/frame_list.h"

#include "../zukan_common.h"
#include "zukanlist.h"
#include "zknlist_bmp_def.h"
#include "zknlist_obj_def.h"
#include "zknlist_bgwfrm_def.h"



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

// ���X�g���ڃT�C�Y
#define	ZKNLISTMAIN_LIST_PX					( 16 )
#define	ZKNLISTMAIN_LIST_PY					( 0 )
#define	ZKNLISTMAIN_LIST_SX					( 16 )
#define	ZKNLISTMAIN_LIST_SY					( 3 )

// ���X�g�p�����[�^�֘A
#define	LIST_MONS_MASK		( 0x0fffffff )
#define	LIST_INFO_MASK		( 0xf0000000 )
#define	LIST_INFO_SHIFT		( 28 )

#define	SET_LIST_PARAM(p,m)	( ( p << LIST_INFO_SHIFT ) | m )
#define	GET_LIST_MONS(a)		( a & LIST_MONS_MASK )
#define	GET_LIST_INFO(a)		( ( a & LIST_INFO_MASK ) >> LIST_INFO_SHIFT )


//typedef struct _ZUKAN_LIST_WORK	ZUKAN_LIST_WORK;
//typedef void (*pZUKAN_LIST_CALLBACK)(void*,s16,s16,s16,s16,u32);


// ���X�g��ʃ��[�N
typedef struct {
	ZUKANLIST_DATA * dat;

	GFL_TCB * vtask;		// TCB ( VBLANK )
	GFL_TCB * htask;		// TCB ( HBLANK )

	PALETTE_FADE_PTR	pfd;		// �p���b�g�t�F�[�h�f�[�^

//	int	key_repeat_speed;
//	int	key_repeat_wait;

	int	mainSeq;		// ���C���V�[�P���X
	int	nextSeq;		// ���̃V�[�P���X
	int	wipeSeq;		// ���C�v��̃V�[�P���X

//	BGWINFRM_WORK * wfrm;			// �E�B���h�E�t���[��
//	u16 * nameBG[2];

	PRINT_UTIL	win[ZKNLISTBMP_WINIDX_MAX];		// BMPWIN
//	u8 * nameBmp;

	GFL_FONT * font;						// �ʏ�t�H���g
	GFL_MSGDATA * mman;					// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;							// �P��Z�b�g
	STRBUF * exp;								// ���b�Z�[�W�W�J�̈�
	PRINT_QUE * que;						// �v�����g�L���[
	STRBUF * name[MONSNO_END];	// �|�P������������

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[ZKNLISTOBJ_IDX_MAX];
	u32	chrRes[ZKNLISTOBJ_CHRRES_MAX];
	u32	palRes[ZKNLISTOBJ_PALRES_MAX];
	u32	celRes[ZKNLISTOBJ_CELRES_MAX];

	u16	pokeGraFlag:1;			// �|�P�������ʊG�\������
	u16	buttonID:15;				// �{�^���A�j���p�h�c
	u8	buttonSeq;					// �{�^���A�j���p�V�[�P���X
	u8	buttonCnt;					// �{�^���A�j���p�J�E���^

	u16	iconPutMain;				// �|�P�A�C�R���\���󋵁i���C���j
	u16	iconPutSub;					// �|�P�A�C�R���\���󋵁i�T�u�j

	FRAMELIST_WORK * lwk;		// ���X�g���[�N
	u32	listInit;					// ���X�g�������V�[�P���X

	u16 * localNo;					// �n���}�Ӕԍ����X�g

	u32	BaseScroll;					// �w�i�X�N���[���J�E���^

	u16	frameScrollCnt;			// �J�n�E�I�����̃X�N���[���J�E���^
	s16	frameScrollVal;			// �J�n�E�I�����̃X�N���[���l

}ZKNLISTMAIN_WORK;

typedef int (*pZKNLIST_FUNC)(ZKNLISTMAIN_WORK*);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�
 *
 * @param		wk		�}�Ӄ��X�g��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_InitVBlank( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��폜
 *
 * @param		wk		�}�Ӄ��X�g��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_ExitVBlank( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTMAIN_InitHBlank( ZKNLISTMAIN_WORK * wk );
extern void ZKNLISTMAIN_ExitHBlank( ZKNLISTMAIN_WORK * wk );


extern void ZKNLISTMAIN_InitVram(void);
extern const GFL_DISP_VRAM * ZKNLISTMAIN_GetVramBankData(void);
extern void ZKNLISTMAIN_InitBg(void);
extern void ZKNLISTMAIN_ExitBg(void);
extern void ZKNLISTMAIN_LoadBgGraphic(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h������
 *
 * @param		wk		�}�Ӄ��X�g��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_InitPaletteFade( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h���
 *
 * @param		wk		�}�Ӄ��X�g��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_ExitPaletteFade( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h���N�G�X�g
 *
 * @param		wk			�}�Ӄ��X�g��ʃ��[�N
 * @param		start		�J�n�Z�x
 * @param		end			�I���Z�x
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_SetPalFadeSeq( ZKNLISTMAIN_WORK * wk, u8 start, u8 end );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A���t�@�u�����h�ݒ�
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_SetBlendAlpha(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A������
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_InitMsg( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A���
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_ExitMsg( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�쐬
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_MakeList( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�폜
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_FreeList( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTMAIN_LoadLocalNoList( ZKNLISTMAIN_WORK * wk );
extern void ZKNLISTMAIN_FreeLocalNoList( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTMAIN_InitFrameScroll( ZKNLISTMAIN_WORK * wk );
extern void ZKNLISTMAIN_SetFrameScrollParam( ZKNLISTMAIN_WORK * wk, s16 val );
extern BOOL ZKNLISTMAIN_MainSrameScroll( ZKNLISTMAIN_WORK * wk );
