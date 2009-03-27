//============================================================================================
/**
 * @file	pm_wb_voice.c
 * @brief	�|�P�����v�a�p�����Đ��R�[���o�b�N�֐�
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "system/gfl_use.h"

#include "savedata/save_control.h"
#include "savedata/perapvoice.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "sound/wb_sound_data.sadl"		//�T�E���h���x���t�@�C��

#include "sound/pm_wb_voice.h"

//==============================================================================================
//
//	�؃��b�v�p��`
//
//==============================================================================================
#define PERAP_SAMPLING_RATE		(2000)									//�T���v�����O���[�g
#define PERAP_SAMPLING_TIME		(1)										//�T���v�����O����
#define PERAP_SAMPLING_SIZE		(PERAP_SAMPLING_RATE * PERAP_SAMPLING_TIME)	//�f�[�^��

#define PERAP_WAVE_SPD			(32768)			//�Đ��X�s�[�h(�P�{��)
#define PERAP_WAVE_SPDRAND		(8192)			//�Đ��X�s�[�h�̃����_��
extern PERAPVOICE * SaveData_GetPerapVoice(SAVE_CONTROL_WORK * sv);
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
 * @brief	�g�`�f�[�^�J�X�^�}�C�Y
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
	if(( pokeNo == MONSNO_PERAPPU )&&( pokeFormNo == 0 )){
		PERAPVOICE* perapVoice = SaveData_GetPerapVoice(SaveControl_GetPointer());
		//�^���f�[�^���ݔ���
		if( PERAPVOICE_GetExistFlag(perapVoice) == TRUE ){
			//�^���f�[�^�W�J
			PERAPVOICE_ExpandVoiceData( *wave, PERAPVOICE_GetVoiceData(perapVoice) );
			*size = PERAP_SAMPLING_SIZE;
			*rate = PERAP_SAMPLING_RATE;
			//�����_���ɉ�����ς���d�l�炵��
			*speed = PERAP_WAVE_SPD + GFUser_GetPublicRand( PERAP_WAVE_SPDRAND );
			return TRUE;
		}
	}
	return FALSE;
}

