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

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "sound/wb_sound_data.sadl"		//�T�E���h���x���t�@�C��

#include "sound/pm_wb_voice.h"

//==============================================================================================
//
//	�T���v�����O�f�[�^�p��`
//
//==============================================================================================
// �؃��b�v
#define PERAP_SAMPLING_RATE		(2000)			//�T���v�����O���[�g
#define PERAP_SAMPLING_TIME		(1)				//�T���v�����O����
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
void PMV_GetVoiceWaveIdx(	u32 pokeNo, 		// [in]�|�P�����i���o�[
							u32 pokeFormNo,		// [in]�|�P�����t�H�[���i���o�[
							u32* waveIdx )		// [out]�g�`IDX
{
	if(( pokeNo < MONSNO_HUSIGIDANE)&&( pokeNo > MONSNO_END) ){
		//�w��͈͊O
		*waveIdx = PMVOICE_POKE001;
		return;
	}
	if( pokeNo == MONSNO_SHEIMI ){					//�V�F�C�~�̎��̂݁A�t�H�����`�F�b�N
		if( pokeFormNo == FORMNO_SHEIMI_SKY ){	//�X�J�C�t�H����
			*waveIdx = WAVE_ARC_PV516_SKY;
			return;
		}
	}
	// �g�`�h�c�w�擾
	*waveIdx = (pokeNo-1) + PMVOICE_POKE001;	// pokeNo(1origin)��ϊ��BPMVOICE_POKE001�`
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
BOOL PMV_CustomVoiceWave(	u32 pokeNo,			// [in]�|�P�����i���o�[
							u32 pokeFormNo,		// [in]�|�P�����t�H�[���i���o�[
							u32 userParam,		// [in]���[�U�[�p�����[�^�[
							void** wave,		// [out]�g�`�f�[�^
							u32* size,			// [out]�g�`�T�C�Y(MAX 26000)
							int* rate,			// [out]�g�`�Đ����[�g
							int* speed )		// [out]�g�`�Đ��X�s�[�h
{
	PMV_REF* pmvRef;

	if( userParam == NULL ){ return FALSE; }	// ����wave�̂ݎg�p

	//�Q�ƃ��[�U�[�p�����[�^�ݒ�
	pmvRef = (PMV_REF*)userParam;

	if(( pokeNo == MONSNO_PERAPPU )&&( pokeFormNo == 0 )){	//�؃��b�v����
		//�^���f�[�^���ݔ���
		if( PERAPVOICE_GetExistFlag(pmvRef->perapVoice) == TRUE ){
			//�^���f�[�^�W�J
			PERAPVOICE_ExpandVoiceData( *wave, PERAPVOICE_GetVoiceData(pmvRef->perapVoice) );
			*size = PERAP_SAMPLING_SIZE;
			*rate = PERAP_SAMPLING_RATE;
			//�����_���ɉ�����ς���d�l�炵��
			*speed = PERAP_WAVE_SPD + GFUser_GetPublicRand( PERAP_WAVE_SPDRAND );
			return TRUE;
		}
	}
	return FALSE;
}

//============================================================================================
/**
 *
 *
 * @brief	�Q�ƃf�[�^�쐬�i�����̂؃��b�v�p�f�[�^���j
 *
 *
 */
//============================================================================================
void PMV_MakeRefDataMine( PMV_REF* pmvRef )
{
	//�؃��b�v�̘^���f�[�^�X�e�[�^�X�|�C���^���Z�[�u�f�[�^���擾
	pmvRef->perapVoice = SaveData_GetPerapVoice(SaveControl_GetPointer());
}







