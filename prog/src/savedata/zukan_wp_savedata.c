//============================================================================================
/**
 * @file		zukan_wp_savedata.c
 * @brief		�}�ӊO���Z�[�u�f�[�^����
 * @author	Hiroyuki Nakamura
 * @date		10.02.08
 *
 *	���W���[�����FZUKAN_WP_SAVEDATA
 */
//============================================================================================
#include <gflib.h>

#include "savedata/zukan_wp_savedata.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �Z�[�u�f�[�^
struct _ZUKANWP_SAVEDATA {
	// �J�X�^���O���t�B�b�N�L����
	u8	customChar[ZUKANWP_SAVEDATA_CHAR_SIZE_X*ZUKANWP_SAVEDATA_CHAR_SIZE_Y*0x20];
	// �J�X�^���O���t�B�b�N�p���b�g
	u16	customPalette[16];
	// �t���[���p���b�g
	u16	framePalette[16];
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�f�[�^�̃T�C�Y�擾
 *
 * @param		none
 *
 * @return	�Z�[�u�f�[�^�̃T�C�Y
 */
//--------------------------------------------------------------------------------------------
int ZUKANWP_SAVEDATA_GetWorkSize(void)
{
	return sizeof(ZUKANWP_SAVEDATA);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�f�[�^������
 *
 * @param		sv		�Z�[�u�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZUKANWP_SAVEDATA_InitWork( ZUKANWP_SAVEDATA * sv )
{
	GFL_STD_MemClear( sv, ZUKANWP_SAVEDATA_GetWorkSize() );
}
