//============================================================================================
/**
 * @file		dpc_main.h
 * @brief		�a������m�F��� ���C������
 * @author	Hiroyuki Nakamura
 * @date		10.03.29
 *
 *	���W���[�����FDPCMAIN
 */
//============================================================================================
#pragma	once

#include "system/main.h"
#include "system/cursor_move.h"
#include "savedata/dendou_save.h"
#include "print/printsys.h"
#include "print/wordset.h"

#include "app/dendou_pc.h"
#include "dpc_bmp_def.h"
#include "dpc_obj_def.h"


//============================================================================================
//	�萔��`
//============================================================================================

// ����m�ۗp�q�[�v�h�c
#define	HEAPID_DENDOU_PC_L		( GFL_HEAP_LOWID(HEAPID_DENDOU_PC) )

// �a�f�p���b�g
#define	DPCMAIN_MBG_PAL_FONT	( 15 )
#define	DPCMAIN_SBG_PAL_FONT	( 15 )


typedef struct {
	DENDOU_POKEMON_DATA	dat[6];
	RTCDate	date;
	u16	pokeMax;
	u16	recNo;
}DPC_PARTY_DATA;

// ���[�N
typedef struct {
	DENDOUPC_PARAM * dat;

	DENDOU_RECORD * rec;
	DENDOU_SAVEDATA * ex_rec;

	DPC_PARTY_DATA	party[DENDOU_RECORD_MAX+1];

	GFL_TCB * vtask;		// TCB ( VBLANK )

	PRINT_UTIL	win[DPCBMP_WINID_MAX];		// BMPWIN

	GFL_FONT * font;						// �ʏ�t�H���g
	GFL_FONT * nfnt;						// �����t�H���g
	GFL_MSGDATA * mman;					// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;							// �P��Z�b�g
	STRBUF * exp;								// ���b�Z�[�W�W�J�̈�
	PRINT_QUE * que;						// �v�����g�L���[

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[DPCOBJ_ID_MAX];
	u32	chrRes[DPCOBJ_CHRRES_MAX];
	u32	palRes[DPCOBJ_PALRES_MAX];
	u32	celRes[DPCOBJ_CELRES_MAX];

	CURSORMOVE_WORK * cmwk;			// �J�[�\���ړ����[�N

	u8	pokePos;
	u8	page;
	u8	pageMax;
	u8	pokeSwap;

	int	mainSeq;		// ���C���V�[�P���X
	int	nextSeq;		// ���̃V�[�P���X
//	int	wipeSeq;		// ���C�v��̃V�[�P���X








/*
	u16	pokeGraFlag:1;			// �|�P�������ʊG�\������
	u16	buttonID:15;				// �{�^���A�j���p�h�c
	u8	buttonSeq;					// �{�^���A�j���p�V�[�P���X
	u8	buttonCnt;					// �{�^���A�j���p�J�E���^
*/


}DPCMAIN_WORK;

typedef int (*pDENDOUPC_FUNC)(DPCMAIN_WORK*);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

extern void DPCMAIN_InitVBlank( DPCMAIN_WORK * wk );
extern void DPCMAIN_ExitVBlank( DPCMAIN_WORK * wk );

extern void DPCMAIN_InitVram(void);
extern const GFL_DISP_VRAM * DPCMAIN_GetVramBankData(void);


extern void DPCMAIN_InitBg(void);
extern void DPCMAIN_ExitBg(void);

extern void DPCMAIN_LoadBgGraphic(void);

extern void DPCMAIN_SetBlendAlpha(void);

extern void DPCMAIN_InitMsg( DPCMAIN_WORK * wk );
extern void DPCMAIN_ExitMsg( DPCMAIN_WORK * wk );


extern void DPCMAIN_CreatePokeData( DPCMAIN_WORK * wk );
extern void DPCMAIN_ExitPokeData( DPCMAIN_WORK * wk );
