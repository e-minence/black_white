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
#define PMVOICE_END		(WAVE_ARC_PV492_SKY)
#define PMVOICE_POKE001	(PMVOICE_START)

//------------------------------------------------------------------
/**
 * @brief	�������[�U�[�p�����[�^�\����
 */
//------------------------------------------------------------------
typedef struct {
	PERAPVOICE*	perapVoice;
}PMV_REF;

//------------------------------------------------------------------
/**
 * @brief	pan�i��ʁj�ݒ��`(��:0 - 64 - 127:�E)
 */
//------------------------------------------------------------------
#define PMV_PAN_L (0)
#define PMV_PAN_C (64)
#define PMV_PAN_R (127)

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
extern void PMV_MakeRefDataMine( PMV_REF* pmvRef );





//============================================================================================
/**
 *
 * @brief	��������C�����C���֐�
 *
 */
//============================================================================================
// ��~�i�Đ����̑S�������Ώہj
inline void PMV_StopVoice( void ){ PMVOICE_Reset(); }

// �I�����o�i�Đ����̑S�������Ώہj
inline void PMV_CheckPlay( void ){ PMVOICE_CheckBusy(); }


//============================================================================================
/**
 * @brief	�����Đ��C�����C���֐�
 *
 *	���C�����C���֐��Ƃ��ĂR��
 *	�@�E����		�F���ׂĔg�`�f�[�^���Đ�
 *	�@�E_forMine	�F�Z�[�u�f�[�^�̓��e���Q�Ƃ��čĐ��i�莝���|�P������z��j
 *	�@�E_forMulti	�F�O������n���ꂽ���e���Q�Ƃ��čĐ��i�}���`�v���C��z��j
 *	������B
 *
 *	���ꂼ��
 *	�@�E����		�F�ʏ�Đ�
 *	�@�E_Pan		�F��ʐݒ�����čĐ�
 *	�@�E_Chorus		�F�R�[���X�ݒ�����čĐ�
 *	�@�E_Reverse	�F�t�Đ�
 *	�@�E_Custom		�F�t���J�X�^�}�C�Y
 *	�ƂȂ��Ă���B
 *			
 *	��������
 *	�@�EpokeNo		�F�|�P�����i���o�[
 *	�@�EformNo		�F�|�P�����t�H�[���i���o�[
 *	�@�Epan			�F���(��:0 - 64 - 127:�E)
 *	�@�Echorus		�F�R�[���X�g�p�t���O
 *	�@�Ech_vol		�F�R�[���X�{�����[����(0 = ���̔g�`�Ƃ̍����Ȃ�)
 *	�@�Ech_spd		�F�Đ����x��(0 = ���̔g�`�Ƃ̍����Ȃ�)
 *	�@�Ereverse		�F�t�Đ��t���O
 *	�@�EpmvRef		�F�Q�ƃf�[�^
 */
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * @brief	����
 */
//--------------------------------------------------------------------------------------------
inline u32 PMV_PlayVoice( u32 pokeNo, u32 formNo )
{
	return PMVOICE_Play(pokeNo, formNo, PMV_PAN_C, FALSE, 0, 0, FALSE, 0);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_Pan( u32 pokeNo, u32 formNo, u8 pan )
{
	return PMVOICE_Play(pokeNo, formNo, pan, FALSE, 0, 0, FALSE, 0);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_Chorus( u32 pokeNo, u32 formNo, int ch_vol, int ch_spd)
{
	return PMVOICE_Play(pokeNo, formNo, PMV_PAN_C, TRUE, ch_vol, ch_spd, FALSE, 0);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_Reverse( u32 pokeNo, u32 formNo)
{
	return PMVOICE_Play(pokeNo, formNo, PMV_PAN_C, FALSE, 0, 0, TRUE, 0);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_Custom
	( u32 pokeNo, u32 formNo, u8 pan, BOOL chorus, int ch_vol, int ch_spd, BOOL reverse )
{
	return PMVOICE_Play(pokeNo, formNo, pan, chorus, ch_vol, ch_spd, reverse, 0);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	_forMine
 */
//--------------------------------------------------------------------------------------------
inline u32 PMV_PlayVoice_forMine( u32 pokeNo, u32 formNo )
{
	PMV_REF pmvRef;
	PMV_MakeRefDataMine(&pmvRef);
	return PMVOICE_Play(pokeNo, formNo, PMV_PAN_C, FALSE, 0, 0, FALSE, (u32)&pmvRef);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_forMine_Pan( u32 pokeNo, u32 formNo, u8 pan )
{
	PMV_REF pmvRef;
	PMV_MakeRefDataMine(&pmvRef);
	return PMVOICE_Play(pokeNo, formNo, pan, FALSE, 0, 0, FALSE, (u32)&pmvRef);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_forMine_Chorus( u32 pokeNo, u32 formNo, int ch_vol, int ch_spd)
{
	PMV_REF pmvRef;
	PMV_MakeRefDataMine(&pmvRef);
	return PMVOICE_Play(pokeNo, formNo, PMV_PAN_C, TRUE, ch_vol, ch_spd, FALSE, (u32)&pmvRef);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_forMine_Reverse( u32 pokeNo, u32 formNo)
{
	PMV_REF pmvRef;
	PMV_MakeRefDataMine(&pmvRef);
	return PMVOICE_Play(pokeNo, formNo, PMV_PAN_C, FALSE, 0, 0, TRUE, (u32)&pmvRef);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_forMine_Custom
	( u32 pokeNo, u32 formNo, u8 pan, BOOL chorus, int ch_vol, int ch_spd, BOOL reverse )
{
	PMV_REF pmvRef;
	PMV_MakeRefDataMine(&pmvRef);
	return PMVOICE_Play(pokeNo, formNo, pan, chorus, ch_vol, ch_spd, reverse, (u32)&pmvRef);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	_forMulti
 */
//--------------------------------------------------------------------------------------------
inline u32 PMV_PlayVoice_forMulti( u32 pokeNo, u32 formNo, PMV_REF* pmvRef )
{
	return PMVOICE_Play(pokeNo, formNo, PMV_PAN_C, FALSE, 0, 0, FALSE, (u32)&pmvRef);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_forMulti_Pan( u32 pokeNo, u32 formNo, u8 pan, PMV_REF* pmvRef )
{
	return PMVOICE_Play(pokeNo, formNo, pan, FALSE, 0, 0, FALSE, (u32)&pmvRef);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_forMulti_Chorus
		( u32 pokeNo, u32 formNo, int ch_vol, int ch_spd, PMV_REF* pmvRef)
{
	return PMVOICE_Play(pokeNo, formNo, PMV_PAN_C, TRUE, ch_vol, ch_spd, FALSE, (u32)&pmvRef);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_forMulti_Reverse( u32 pokeNo, u32 formNo, PMV_REF* pmvRef)
{
	return PMVOICE_Play(pokeNo, formNo, PMV_PAN_C, FALSE, 0, 0, TRUE, (u32)&pmvRef);
}
//--------------------------------------------------------------
inline u32 PMV_PlayVoice_forMulti_Custom( u32 pokeNo, u32 formNo, u8 pan, 
						BOOL chorus, int ch_vol, int ch_spd, BOOL reverse, PMV_REF* pmvRef )
{
	return PMVOICE_Play(pokeNo, formNo, pan, chorus, ch_vol, ch_spd, reverse, (u32)&pmvRef);
}


#endif


