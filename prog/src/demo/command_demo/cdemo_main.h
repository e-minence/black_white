//============================================================================================
/**
 * @file		cdemo_main.h
 * @brief		�R�}���h�f����� ���C������
 * @author	Hiroyuki Nakamura
 * @date		09.01.21
 */
//============================================================================================
#pragma	once

#include "demo/command_demo.h"

//============================================================================================
//	�萔��`
//============================================================================================

// [ HEAPID_COMMAND_DEMO ] ����m�ۗp��`
#define	HEAPID_COMMAND_DEMO_L		( GFL_HEAP_LOWID(HEAPID_COMMAND_DEMO) )

// �p���b�g�f�[�^
typedef struct {
	u16	buff[256*16];
	u32	size;
}CDEMO_PALETTE;

// ���[�N
typedef struct {
	COMMANDDEMO_DATA * dat;

	ARCHANDLE * gra_ah;

	CDEMO_PALETTE	pltt[3];

	GFL_TCB * vtask;					// TCB ( VBLANK )

	int * commSrc;
	int * commPos;

	u32	cnt;		// �J�E���^

	// �A���t�@�u�����h
	int	alpha_plane1;		// �ΏۖʂP
	int	alpha_ev1;			// �ΏۖʂP�̃u�����h�W��
	int	alpha_plane2;		// �ΏۖʂQ
	int	alpha_ev2;			// �ΏۖʂQ�̃u�����h�W��
	int	alpha_end_ev;		// �I���u�����h�W��
	int	alpha_mv_frm;		// �I���܂ł̃t���[����

	// �a�f�X�N���[���؂�ւ��A�j��
//	u16	frmMax;
	u16	bgsa_num;
	u8	bgsa_load;
	u8	bgsa_seq;
	OSTick	bfTick;
	OSTick	stTick;
//	u8	bgsa_wait;
//	u8	bgsa_cnt;
//	int	bgsa_chr;
//	int	bgsa_pal;
//	int	bgsa_scr;
//	OSTick	dfTick;
//	OSTick	afTick;
//	s64	stTick;

	int	main_seq;
	int	next_seq;

	BOOL	init_flg;			// �������ς݃t���O

#ifdef PM_DEBUG
	u32	debug_count_frm;
	u32	debug_count;

	OSTick	stick;
	OSTick	etick;

#endif	// PM_DEBUG

}CDEMO_WORK;

typedef int (*pCommDemoFunc)(CDEMO_WORK*);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM�ݒ�
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_VramBankSet(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�ݒ�
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_BgInit( CDEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�폜
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_BgExit(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�}���h�f�[�^�ǂݍ���
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_CommDataLoad( CDEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�}���h�f�[�^�폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_CommDataDelete( CDEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�ݒ�
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_InitVBlank( CDEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_ExitVBlank( CDEMO_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�C���Z�b�g
 *
 * @param		wk		���[�N
 * @param		div		������
 * @param		sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param		next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_FadeInSet( CDEMO_WORK * wk, int div, int sync, int next );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�A�E�g�Z�b�g
 *
 * @param		wk		���[�N
 * @param		div		������
 * @param		sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param		next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_FadeOutSet( CDEMO_WORK * wk, int div, int sync, int next );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�z���C�g�C���Z�b�g
 *
 * @param		wk		���[�N
 * @param		div		������
 * @param		sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param		next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_WhiteInSet( CDEMO_WORK * wk, int div, int sync, int next );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�z���C�g�A�E�g�Z�b�g
 *
 * @param		wk		���[�N
 * @param		div		������
 * @param		sync	�������ꂽ�t���[���̃E�F�C�g��
 * @param		next	�t�F�[�h��̃V�[�P���X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_WhiteOutSet( CDEMO_WORK * wk, int div, int sync, int next );

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�p���b�g�]�����N�G�X�g
 *
 * @param		wk		���[�N
 * @param		frm		BG�t���[��
 * @param		id		�A�[�N�C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_LoadPaletteRequest( CDEMO_WORK * wk, u32 frm, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�O���t�B�b�N�ǂݍ���
 *
 * @param		wk		���[�N
 * @param		chr		�L�����A�[�N�C���f�b�N�X
 * @param		pal		�p���b�g�A�[�N�C���f�b�N�X
 * @param		scrn	�X�N���[���A�[�N�C���f�b�N�X
 * @param		frm		BG�t���[��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CDEMOMAIN_LoadBgGraphic( CDEMO_WORK * wk, u32 chr, u32 pal, u32 scrn, u32 frm );
