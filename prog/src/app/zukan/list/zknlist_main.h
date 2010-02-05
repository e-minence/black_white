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

//	int	key_repeat_speed;
//	int	key_repeat_wait;

	int	mainSeq;		// ���C���V�[�P���X
	int	nextSeq;		// ���̃V�[�P���X
	int	wipeSeq;		// ���C�v��̃V�[�P���X

//	BGWINFRM_WORK * wfrm;			// �E�B���h�E�t���[��
//	u16 * nameBG[2];

	PRINT_UTIL	win[ZKNLISTBMP_WINIDX_MAX];		// BMPWIN
	u8 * nameBmp;

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

	u16	seeNum;			// ��������
	u16	getNum;			// �߂܂�����

	// ���X�g�֘A
	FRAMELIST_WORK * lwk;
/*
	ZUKAN_LIST_WORK * list;
	s32	listPutIndex;
	u8	listScroll;
	u8	listSpeed;
	u8	listRepeat;
	u8	listConut;
	u8	listBgScroll;


	BOOL	initTouchFlag;
	u32	initTouchPY;
	u32	frameTouchPY;

//	u32	autoSpeed;
	u32	autoScroll;
	u32	autoSpeed;
	u32	autoCount;
	u32	autoWait;
*/
/*
	u32	tsCount;
	u32	targetPos;
*/

	u32	BaseScroll;


/*
	PALETTE_FADE_PTR	pfd;		// �p���b�g�t�F�[�h�f�[�^
	BUTTON_ANM_WORK	bawk;				// �{�^���A�j�����[�N
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

extern void ZKNLISTMAIN_InitHBlank( ZKNLISTMAIN_WORK * wk );
extern void ZKNLISTMAIN_ExitHBlank( ZKNLISTMAIN_WORK * wk );


extern void ZKNLISTMAIN_InitVram(void);
extern const GFL_DISP_VRAM * ZKNLISTMAIN_GetVramBankData(void);
extern void ZKNLISTMAIN_InitBg(void);
extern void ZKNLISTMAIN_ExitBg(void);
extern void ZKNLISTMAIN_LoadBgGraphic(void);

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

/*
extern u32 ZKNLISTMAIN_GetListMons( ZUKAN_LIST_WORK * wk, u32 pos );

extern u32 ZKNLISTMAIN_GetListInfo( ZUKAN_LIST_WORK * wk, u32 pos );

extern void ZKNLISTMAIN_PutListCursor( ZKNLISTMAIN_WORK * wk, u8 pal, s16 pos );

extern void ZKNLISTMAIN_InitListPut( ZKNLISTMAIN_WORK * wk );





#define	ZKNLISTMAIN_LIST_MOVE_UP							( 0 )
#define	ZKNLISTMAIN_LIST_MOVE_DOWN						( 1 )
#define	ZKNLISTMAIN_LIST_MOVE_LEFT						( 2 )
#define	ZKNLISTMAIN_LIST_MOVE_RIGHT						( 3 )
#define	ZKNLISTMAIN_LIST_MOVE_SCROLL_UP				( 4 )
#define	ZKNLISTMAIN_LIST_MOVE_SCROLL_DOWN			( 5 )
#define	ZKNLISTMAIN_LIST_MOVE_POS_DIRECT			( 6 )
#define	ZKNLISTMAIN_LIST_MOVE_SCROLL_DIRECT		( 7 )
#define	ZKNLISTMAIN_LIST_MOVE_NONE						( 0xffffffff )

extern ZUKAN_LIST_WORK * ZKNLISTMAIN_CreateList( u32 siz, HEAPID heapID );
extern void ZKNLISTMAIN_ExitList( ZUKAN_LIST_WORK * wk );
extern void ZKNLISTMAIN_AddListData( ZUKAN_LIST_WORK * wk, STRBUF * str, u32 prm );
extern void ZKNLISTMAIN_InitList( ZUKAN_LIST_WORK * wk, s16 pos, s16 posMax, s16 scroll, void * work, pZUKAN_LIST_CALLBACK func );
extern s16 ZKNLISTMAIN_GetListPos( ZUKAN_LIST_WORK * wk );
extern s16 ZKNLISTMAIN_GetListScroll( ZUKAN_LIST_WORK * wk );
extern s16 ZKNLISTMAIN_GetListPosMax( ZUKAN_LIST_WORK * wk );
extern s16 ZKNLISTMAIN_GetListMax( ZUKAN_LIST_WORK * wk );
extern s16 ZKNLISTMAIN_GetListCursorPos( ZUKAN_LIST_WORK * wk );
extern s16 ZKNLISTMAIN_GetListScrollMax( ZUKAN_LIST_WORK * wk );
extern STRBUF * ZKNLISTMAIN_GetListStr( ZUKAN_LIST_WORK * wk, u32 pos );
extern u32 ZKNLISTMAIN_GetListParam( ZUKAN_LIST_WORK * wk, u32 pos );
extern void ZKNLISTMAIN_SetListPos( ZUKAN_LIST_WORK * wk, s16 pos );
extern void ZKNLISTMAIN_SetListScroll( ZUKAN_LIST_WORK * wk, s16 scroll );
extern u32 ZKNLISTMAIN_Main( ZUKAN_LIST_WORK * wk );
extern u32 ZKNLISTMAIN_MoveLeft( ZUKAN_LIST_WORK * wk );
extern u32 ZKNLISTMAIN_MoveRight( ZUKAN_LIST_WORK * wk );
extern void ZKNLISTMAIN_SetPosDirect( ZUKAN_LIST_WORK * wk, s16 pos );
extern void ZKNLISTMAIN_SetScrollDirect( ZUKAN_LIST_WORK * wk, s16 scroll );
extern u32 ZKNLISTMAIN_SetListDirect( ZUKAN_LIST_WORK * wk, s16 pos, s16 mv, BOOL autoFlag );
*/
