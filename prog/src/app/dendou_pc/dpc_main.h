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
#include "system/palanm.h"
#include "system/bmp_oam.h"
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

	DPC_PARTY_DATA	party[DENDOU_RECORD_MAX];
	s16	nowRad[6];
	s16	posRad[6];

	GFL_TCB * vtask;		// TCB ( VBLANK )
	GFL_TCB * htask;		// TCB ( HBLANK )

	PALETTE_FADE_PTR	pfd;		// �p���b�g�t�F�[�h�f�[�^
	u8	posEvy[6];
	u8	nowEvy[6];
	u8	tmpEvy[6];

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

	// �n�`�l�t�H���g
	BMPOAM_SYS_PTR	fntoam;
	BMPOAM_ACT_PTR	foact;
	GFL_BMP_DATA * fobmp;

	s8	pokePos;
	s8	pokeChg;
	s8	page;
	u8	pageMax;
	u8	pokeSwap;

	s8	pokeMove;
	u8	pokeMoveCnt;

	u32	buttonID;		// �{�^���n�a�i�h�c
	int	buttonSeq;	// �{�^���A�j���p�V�[�P���X

	int	mainSeq;		// ���C���V�[�P���X
	int	subSeq;			// �T�u�V�[�P���X
	int	nextSeq;		// ���̃V�[�P���X

}DPCMAIN_WORK;

typedef int (*pDENDOUPC_FUNC)(DPCMAIN_WORK*);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_InitVBlank( DPCMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��폜
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_ExitVBlank( DPCMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM�ݒ�
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_InitVram(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM�ݒ�擾
 *
 * @param		none
 *
 * @return	VRAM�ݒ�f�[�^
 */
//--------------------------------------------------------------------------------------------
extern const GFL_DISP_VRAM * DPCMAIN_GetVramBankData(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_InitBg(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG���
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_ExitBg(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�O���t�B�b�N�ǂݍ���
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_LoadBgGraphic(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h������
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_InitPaletteFade( DPCMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h���
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_ExitPaletteFade( DPCMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h���N�G�X�g
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_RequestPaletteFade( DPCMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h�`�F�b�N
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @retval	"TRUE = ������"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL DPCMAIN_CheckPaletteFade( DPCMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A���t�@�u�����h�ݒ�
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_SetBlendAlpha(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A������
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_InitMsg( DPCMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A���
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_ExitMsg( DPCMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �|�P�����f�[�^�쐬
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_CreatePokeData( DPCMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �|�P�����f�[�^���
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void DPCMAIN_ExitPokeData( DPCMAIN_WORK * wk );
