//============================================================================================
/**
 * @file		zknsearch_main.h
 * @brief		�}�ӌ������ ���C������
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	���W���[�����FZKNSEARCHMAIN
 */
//============================================================================================
#pragma	once

#include "system/cursor_move.h"
#include "system/blink_palanm.h"
#include "system/bgwinfrm.h"
#include "system/palanm.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "ui/frame_list.h"

#include "../zukan_common.h"
#include "zukansearch.h"
#include "zknsearch_bmp_def.h"
#include "zknsearch_obj_def.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �q�[�v�h�c����m�ۗp��`
#define	HEAPID_ZUKAN_SEARCH_L		( GFL_HEAP_LOWID(HEAPID_ZUKAN_SEARCH) )

#define	ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF	( 1 )
#define	ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR	( 2 )
#define	ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2	( 3 )
#define	ZKNSEARCHMAIN_MBG_PAL_TOUCHBAR		( 14 )
#define	ZKNSEARCHMAIN_MBG_PAL_FONT				( 15 )

#define	ZKNSEARCHMAIN_SBG_PAL_FONT			( 15 )

#define	ZKNSEARCHMAIN_ITEM_MAX				( 50 )		// ���X�g���ڐ�

// �^�b�`�o�[��`
#define	ZKNSEARCHMAIN_TOUCH_BAR_PX		( 0 )
#define	ZKNSEARCHMAIN_TOUCH_BAR_PY		( 21 )
#define	ZKNSEARCHMAIN_TOUCH_BAR_SX		( 32 )
#define	ZKNSEARCHMAIN_TOUCH_BAR_SY		( 3 )

// �y�[�W�ԍ�
enum {
	ZKNSEARCHMAIN_PAGE_ROW = 0,
	ZKNSEARCHMAIN_PAGE_NAME,
	ZKNSEARCHMAIN_PAGE_TYPE,
	ZKNSEARCHMAIN_PAGE_COLOR,
	ZKNSEARCHMAIN_PAGE_FORM,
};

// ���C�����[�N
typedef struct {
	ZUKANSEARCH_DATA * dat;		// �O���f�[�^

	GFL_TCB * vtask;		// TCB ( VBLANK )
	GFL_TCB * htask;		// TCB ( HBLANK )

	PALETTE_FADE_PTR	pfd;		// �p���b�g�t�F�[�h�f�[�^

	int	mainSeq;		// ���C���V�[�P���X
	int	nextSeq;		// ���̃V�[�P���X
	int	funcSeq;		// �t�F�[�h�ȂǓ���̏����̌�̃V�[�P���X

	BGWINFRM_WORK * wfrm;

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[ZKNSEARCHOBJ_IDX_MAX];
	u32	chrRes[ZKNSEARCHOBJ_CHRRES_MAX];
	u32	palRes[ZKNSEARCHOBJ_PALRES_MAX];
	u32	celRes[ZKNSEARCHOBJ_CELRES_MAX];

	PRINT_UTIL	win[ZKNSEARCHBMP_WINIDX_MAX];		// BMPWIN

	GFL_FONT * font;						// �ʏ�t�H���g
	GFL_MSGDATA * mman;					// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;							// �P��Z�b�g
	STRBUF * exp;								// ���b�Z�[�W�W�J�̈�
	PRINT_QUE * que;						// �v�����g�L���[

	FRAMELIST_WORK * lwk;										// ���X�g���[�N
	STRBUF * item[ZKNSEARCHMAIN_ITEM_MAX];	// ���X�g���ڕ�����

	CURSORMOVE_WORK * cmwk;			// �J�[�\���ړ����[�N
	BLINKPALANM_WORK * blink;		// �J�[�\���A�j�����[�N

	u8	bgVanish;

	u8	page;
	u8	pageSeq;

	u8	loadingCnt;

	u8	btnMode;
	u8	btnID;
	u8	btnSeq;
	u8	btnCnt;

	u32	BaseScroll;

	u16	frameScrollCnt;			// �J�n�E�I�����̃X�N���[���J�E���^
	s16	frameScrollVal;			// �J�n�E�I�����̃X�N���[���l

}ZKNSEARCHMAIN_WORK;

typedef int (*pZKNSEARCH_FUNC)(ZKNSEARCHMAIN_WORK*);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_InitVBlank( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��폜
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_ExitVBlank( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK�֐��ݒ�
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_InitHBlank( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK�֐��폜
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_ExitHBlank( ZKNSEARCHMAIN_WORK * wk );



extern void ZKNSEARCHMAIN_InitVram(void);
extern const GFL_DISP_VRAM * ZKNSEARCHMAIN_GetVramBankData(void);
extern void ZKNSEARCHMAIN_InitBg(void);
extern void ZKNSEARCHMAIN_ExitBg(void);

extern void ZKNSEARCHMAIN_ChangeBgPriorityMenu(void);
extern void ZKNSEARCHMAIN_ChangeBgPriorityList(void);

extern void ZKNSEARCHMAIN_LoadBgGraphic(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h������
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_InitPaletteFade( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h���
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_ExitPaletteFade( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h���N�G�X�g
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		start		�J�n�Z�x
 * @param		end			�I���Z�x
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_SetPalFadeSeq( ZKNSEARCHMAIN_WORK * wk, u8 start, u8 end );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A���t�@�u�����h�ݒ�
 *
 * @param		flg		ON/OFF
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_SetBlendAlpha( BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A������
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_InitMsg( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A���
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHMAIN_ExitMsg( ZKNSEARCHMAIN_WORK * wk );



extern void ZKNSEARCHMAIN_InitBlinkAnm( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_ExitBlinkAnm( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHMAIN_InitBgWinFrame( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_ExitBgWinFrame( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHMAIN_LoadMenuPageScreen( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_LoadRowListPageScreen( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_LoadNameListPageScreen( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_LoadTypeListPageScreen( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_LoadColorListPageScreen( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_LoadFormListPageScreen( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHMAIN_LoadingWindowOn( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_LoadingWindowOff( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHMAIN_ListBGOn( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_ListBGOff( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHMAIN_InitFrameScroll( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHMAIN_SetFrameScrollParam( ZKNSEARCHMAIN_WORK * wk, s16 val );
extern BOOL ZKNSEARCHMAIN_MainSrameScroll( ZKNSEARCHMAIN_WORK * wk );
