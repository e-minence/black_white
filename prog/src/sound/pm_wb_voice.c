//============================================================================================
/**
 * @file	pm_wb_voice.c
 * @brief	�|�P�����v�a�p�����Đ��R�[���o�b�N�֐�
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "poke_tool/poke_tool.h"
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
void PMWB_GetVoiceWaveIdx(	u32 pokeNo, 		// [in]�|�P�����i���o�[
							u32 pokeFormNo,		// [in]�|�P�����t�H�[���i���o�[
							u32* waveIdx )		// [out]�g�`IDX
{
	if(( pokeNo < PMVOICE_START)&&( pokeNo > PMVOICE_END) ){
		//�w��͈͊O
		*waveIdx = PMVOICE_POKE001;
		return;
	}
	if( pokeNo == MONSNO_EURISU ){					//�V�F�C�~�̎��̂݁A�t�H�����`�F�b�N
		if( pokeFormNo == FORMNO_SHEIMI_FLOWER ){	//�X�J�C�t�H����
			*waveIdx = WAVE_ARC_PV516_SKY;
			return;
		}
	}
	// �g�`�h�c�w�擾
	*waveIdx = (pokeNo-1) + PMVOICE_POKE001;	// 1origin, PMVOICE_POKE001�`
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
BOOL PMWB_CustomVoiceWave(	u32 pokeNo,			// [in]�|�P�����i���o�[
							u32 pokeFormNo,		// [in]�|�P�����t�H�[���i���o�[
							void** wave,		// [out]�g�`�f�[�^
							u32* size,			// [out]�g�`�T�C�Y
							int* rate,			// [out]�g�`�Đ����[�g
							int* speed )		// [out]�g�`�Đ��X�s�[�h
{
	return FALSE;
}

