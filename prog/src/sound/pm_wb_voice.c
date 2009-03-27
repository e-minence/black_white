//============================================================================================
/**
 * @file	pm_wb_voice.c
 * @brief	�|�P�����v�a�p�����Đ��R�[���o�b�N�֐�
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "poke_tool/monsno_def.h"
#include "sound/wb_sound_data.sadl"		//�T�E���h���x���t�@�C��

#include "sound/pm_wb_voice.h"
//============================================================================================
/**
 *
 *
 * @brief	�g�`�h�c�w�擾
 *
 *
 */
//============================================================================================
void PMWB_GetVoiceWaveIdx(	u32 pokeNum, 		// [in]�|�P�����i���o�[
							u32 pokeFormNum,	// [in]�|�P�����t�H�[���i���o�[
							u32* waveIdx )		// [out]�g�`IDX
{
	if(( pokeNum < PMVOICE_START)&&( pokeNum > PMVOICE_END) ){
		//�w��͈͊O
		*waveIdx = PMVOICE_POKE001;
		return;
	}
	// �g�`�h�c�w�擾
	*waveIdx = (pokeNum-1) + PMVOICE_POKE001;	// 1origin, PMVOICE_POKE001�`
}

//============================================================================================
/**
 *
 *
 * @brief	�g�`�f�[�^�擾
 *
 *
 */
//============================================================================================
BOOL PMWB_CustomVoiceWave(	u32 pokeNum,		// [in]�|�P�����i���o�[
							u32 pokeFormNum,	// [in]�|�P�����t�H�[���i���o�[
							void** wave,		// [out]�g�`�f�[�^
							u32* size,			// [out]�g�`�T�C�Y
							int* rate,			// [out]�g�`�Đ����[�g
							int* speed )		// [out]�g�`�Đ��X�s�[�h
{
	return FALSE;
}

