//============================================================================================
/**
 * @file	pm_wb_voice.h
 * @brief	�|�P�����v�a�p�����Đ��R�[���o�b�N�֐�
 * @author	
 * @date	
 */
//============================================================================================
#ifndef __PMWB_VOICE_H__
#define __PMWB_VOICE_H__

#include "sound/pm_voice.h"
#include "savedata/perapvoice.h"
//------------------------------------------------------------------
/**
 * @brief	�f�[�^�h�m�c�d�w�L���͈�
 */
//------------------------------------------------------------------
#define PMVOICE_START	(BANK_PV001)
#define PMVOICE_END		(BANK_PV516_SKY)
#define PMVOICE_POKE001	(PMVOICE_START)

//------------------------------------------------------------------
/**
 * @brief	�������[�U�[�p�����[�^�\����
 */
//------------------------------------------------------------------
typedef struct {
	PERAPVOICE*	perapVoice;
}PMV_REF;

//============================================================================================
/**
 * @brief	�g�`�h�c�w�擾�R�[���o�b�N
 */
//============================================================================================
extern void	PMV_GetVoiceWaveIdx( u32, u32, u32* );
//============================================================================================
/**
 * @brief	�g�`�f�[�^�J�X�^�}�C�Y�R�[���o�b�N
 */
//============================================================================================
extern BOOL	PMV_CustomVoiceWave( u32, u32, u32, void**, u32*, int*, int* );

//============================================================================================
/**
 * @brief	�Q�ƃf�[�^�쐬�i�����̂؃��b�v�p�f�[�^���j
 */
//============================================================================================
extern void PMV_MakeRefData( PMV_REF* pmvRef );

//============================================================================================
/**
 * @brief	�����C�����C���֐�
 */
//============================================================================================
// �ʏ�i����wave�̂ݎg�p�j
inline void PMV_PlayVoice( u32 pokeNum, u32 formNum ){
		PMVOICE_Play(pokeNum, formNum, 64, FALSE, 0, 0, FALSE, 0);
}

// �莝���|�P�����p�Đ��i�Q�ƃf�[�^�͎����̃Z�[�u�f�[�^�j
inline void PMV_PlayVoiceMine( u32 pokeNum, u32 formNum ){
		PMV_REF pmvRef;

		PMV_MakeRefData(&pmvRef);
		PMVOICE_Play(pokeNum, formNum, 64, FALSE, 0, 0, FALSE, (u32)&pmvRef);
}

// �}���`�v���C�p�Đ��i�Q�ƃf�[�^���O�����󂯎��j
inline void PMV_PlayVoiceMulti( u32 pokeNum, u32 formNum, PMV_REF* pmvRef ){
		PMVOICE_Play(pokeNum, formNum, 64, FALSE, 0, 0, FALSE, (u32)&pmvRef);
}

//--------------------------------------------------------------------------------------------
inline void PMV_PlayVoiceChorus( u32 pokeNum, u32 formNum ){
		PMV_REF pmvRef;

		PMV_MakeRefData(&pmvRef);
		PMVOICE_Play(pokeNum, formNum, 64, TRUE, -10, 20, FALSE, 0);
}

inline void PMV_PlayVoiceRev( u32 pokeNum, u32 formNum ){
		PMV_REF pmvRef;

		PMV_MakeRefData(&pmvRef);
		PMVOICE_Play(pokeNum, formNum, 64, FALSE, 0, 0, TRUE, 0);
}

#endif


