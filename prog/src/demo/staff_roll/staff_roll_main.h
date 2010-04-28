//============================================================================================
/**
 * @file		staff_roll_main.h
 * @brief		�G���f�B���O�E�X�^�b�t���[�����
 * @author	Hiroyuki Nakamura
 * @date		10.04.19
 *
 *	���W���[�����FSRMAIN
 */
//============================================================================================
#pragma	once

#include "print/printsys.h"
#include "print/wordset.h"
#include "demo/staff_roll.h"


//============================================================================================
//============================================================================================

// �t�H���g�^�C�v
enum {
	SRMAIN_FONT_NORMAL = 0,			// �ʏ�
	SRMAIN_FONT_SMALL,					// �������t�H���g
	SRMAIN_FONT_MAX							// �t�H���g�ő吔
};

// ���X�g�f�[�^
typedef struct {
	u32	msgIdx;

	u16	wait;
	u8	label;
	u8	labelType;

	u8	color;
	u8	font;
	u16	putMode;

	s16	px;
	s16	offs_x;
}ITEMLIST_DATA;

typedef struct {
	STAFFROLL_DATA * dat;			// �O���ݒ�f�[�^

	GFL_TCB * vtask;		// TCB ( VBLANK )

/*
	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk;
	u32	chrRes;
	u32	palRes;
	u32	celRes;
*/

	GFL_FONT * font[SRMAIN_FONT_MAX];		// �t�H���g
	GFL_MSGDATA * mman;									// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;											// �P��Z�b�g
	STRBUF * exp;												// ���b�Z�[�W�W�J�̈�
//	PRINT_QUE * que;										// �v�����g�L���[

	PRINT_UTIL	util[2];			// BMPWIN
//	u8	vBmp[0x20*32*32*2];		// ���z�a�l�o
	u8	bmpTransFlag;
//	u8	bmpShitfFlag;

	u8	labelType;
	u8	labelSeq;

	u8	putFrame;

	s8	britness;

	u8	skipCount;
	u8	skipFlag;

	ITEMLIST_DATA * list;
	u16	listPos;
	s16	listWait;
	fx32	listScrollCnt;
	BOOL	listScrollFlg;


	int	mainSeq;
	int	subSeq;
	int	nextSeq;

#ifdef	PM_DEBUG
	BOOL	debugStopFlg;
#endif

}SRMAIN_WORK;

typedef int (*pSRMAIN_FUNC)(SRMAIN_WORK*);


//============================================================================================
//============================================================================================

extern BOOL SRMAIN_Main( SRMAIN_WORK * wk );
