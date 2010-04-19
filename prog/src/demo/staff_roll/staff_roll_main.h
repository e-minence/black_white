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

#include "demo/staff_roll.h"

/*
typedef struct {
	PRINTSYS_LSB	fontColor;
	u16	bgColor;
}SRMAIN_VER_DATA;
*/

typedef struct {
//	SRMAIN_VER_DATA	ver;		// �o�[�W�����ʃf�[�^

	GFL_TCB * vtask;		// TCB ( VBLANK )

	int	mainSeq;
	int	nextSeq;

}SRMAIN_WORK;

typedef int (*pSRMAIN_FUNC)(SRMAIN_WORK*);


extern BOOL SRMAIN_Main( SRMAIN_WORK * wk );
