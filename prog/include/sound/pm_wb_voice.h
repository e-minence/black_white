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
//------------------------------------------------------------------
/**
 * @brief	�f�[�^�h�m�c�d�w�L���͈�
 */
//------------------------------------------------------------------
#define PMVOICE_START	(BANK_PV001)
#define PMVOICE_END		(BANK_PV516_SKY)
#define PMVOICE_POKE001	(PMVOICE_START)

//============================================================================================
/**
 * @brief	�g�`�h�c�w�擾�R�[���o�b�N
 */
//============================================================================================
extern void	PMWB_GetVoiceWaveIdx( u32, u32, u32* );
//============================================================================================
/**
 * @brief	�g�`�f�[�^�����R�[���o�b�N
 */
//============================================================================================
extern BOOL	PMWB_CustomVoiceWave( u32, u32, void**, u32*, int*, int* );

//============================================================================================
/**
 * @brief	�����}�N��
 */
//============================================================================================
#define PMWB_PlayVoice( pokeNum, formNum ) \
		PMVOICE_Play(pokeNum, formNum, 64, FALSE, 0, 0, FALSE, FALSE)

#define PMWB_PlayVoiceChorus( pokeNum, formNum ) \
		PMVOICE_Play(pokeNum, formNum, 64, TRUE, -10, 20, FALSE, FALSE)

#define PMWB_PlayVoiceRev( pokeNum, formNum ) \
		PMVOICE_Play(pokeNum, formNum, 64, FALSE, 0, 0, FALSE, TRUE)

#endif


