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

#include "zukansearch.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �q�[�v�h�c����m�ۗp��`
#define	HEAPID_ZUKAN_SEARCH_L		( GFL_HEAP_LOWID(HEAPID_ZUKAN_SEARCH) )

#define	ZKNSEARCHMAIN_MBG_PAL_TOUCHBAR	( 14 )
#define	ZKNSEARCHMAIN_MBG_PAL_FONT			( 15 )

#define	ZKNSEARCHMAIN_SBG_PAL_FONT			( 15 )

// �^�b�`�o�[��`
#define	ZKNSEARCHMAIN_TOUCH_BAR_PX		( 0 )
#define	ZKNSEARCHMAIN_TOUCH_BAR_PY		( 21 )
#define	ZKNSEARCHMAIN_TOUCH_BAR_SX		( 32 )
#define	ZKNSEARCHMAIN_TOUCH_BAR_SY		( 3 )

// ���C�����[�N
typedef struct {
	ZUKANSEARCH_DATA * dat;		// �O���f�[�^

	GFL_TCB * vtask;		// TCB ( VBLANK )
	GFL_TCB * htask;		// TCB ( HBLANK )

	int	mainSeq;		// ���C���V�[�P���X
	int	nextSeq;		// ���̃V�[�P���X
	int	fadeSeq;		// �t�F�[�h��̃V�[�P���X�i���C�v���j


	u32	BaseScroll;

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
extern void ZKNSEARCHMAIN_InitBg(void);
extern void ZKNSEARCHMAIN_ExitBg(void);
extern void ZKNSEARCHMAIN_LoadBgGraphic(void);
