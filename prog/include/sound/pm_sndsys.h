//============================================================================================
/**
 * @file	pm_sndsys.h
 * @brief	�|�P�����T�E���h�V�X�e��
 * @author	
 * @date	
 */
//============================================================================================
#ifndef __PMSNDSYS_H__
#define __PMSNDSYS_H__

#include "sound/wb_sound_data.sadl"		//�T�E���h���x���t�@�C��
//------------------------------------------------------------------
/**
 * @brief	�f�[�^�h�m�c�d�w�L���͈�
 */
//------------------------------------------------------------------
#define PMSND_BGM_START		(SEQ_MUS_GS_BICYCLE)
#define PMSND_BGM_END		(SEQ_MUS_WB_WIN4)
#define PMSND_SE_START		(SEQ_SE_DP_000)
#define PMSND_SE_END		(SEQ_TEST_TITLE)
#define PMSND_VOICE_START	(BANK_PV001)
#define PMSND_VOICE_END		(BANK_PV516_SKY)
#define PMSND_POKEVOICE_001	(BANK_PV001)

//------------------------------------------------------------------
/**
 * @brief	�֐������p��`
 */
//------------------------------------------------------------------
#define PMSND_NOEFFECT (-1)

//============================================================================================
/**
 *
 * @brief	�T�E���h�V�X�e���imain.c������Ăяo�����j
 *
 */
//============================================================================================
extern void	PMSND_Init( void );
extern void	PMSND_Main( void );
extern void	PMSND_Exit( void );

//============================================================================================
/**
 *
 * @brief	�T�E���h�֐��i�eapp����Ăяo�����j
 *
 */
//============================================================================================
extern NNSSndHandle* PMSND_GetBGMhandlePointer( void );

extern BOOL	PMSND_PlayBGM( u32 soundIdx );					//�a�f�l���Đ�
extern BOOL	PMSND_PlayBGM_EX( u32 soundIdx, u16 trackBit );	//��L�g��
extern BOOL	PMSND_PlayNextBGM( u32 soundIdx );				//�a�f�l�t�F�[�h�C���A�E�g�L��ōĐ�
extern BOOL	PMSND_PlayNextBGM_EX( u32 soundIdx, u16 trackBit );//��L�g��
extern void	PMSND_ChangeBGMtrack( u16 trackBit );			//�a�f�l�̍Đ��g���b�N�ύX
extern BOOL	PMSND_CheckPlayBGM( void );						//�a�f�l�I�����o
extern void	PMSND_SetStatusBGM
				( int tempoRatio, int pitch, int pan );//�a�f�l�X�e�[�^�X�ύX

extern void	PMSND_StopBGM( void );				//���݂̂a�f�l���~
extern void	PMSND_PauseBGM( BOOL pauseFlag );	//���݂̂a�f�l���ꎞ��~(TRUE��~,FALSE�ĊJ)
extern void	PMSND_FadeInBGM( u16 frames );		//���݂̂a�f�l���t�F�[�h�C��
extern void	PMSND_FadeOutBGM( u16 frames );		//���݂̂a�f�l���t�F�[�h�A�E�g
extern BOOL	PMSND_CheckFadeOnBGM( void );		//�t�F�[�h���s�`�F�b�N(TRUE���s��)

extern void	PMSND_PushBGM( void );				//���݂̂a�f�l��ޔ�
extern void	PMSND_PopBGM( void );				//���݂̂a�f�l�𕜌�

extern BOOL	PMSND_PlaySystemSE( u32 soundNum );	//�V�X�e���r�d���Đ�
extern BOOL	PMSND_PlaySE( u32 soundNum );		//�r�d���Đ�
extern BOOL	PMSND_PlayVoice( u32 pokeNum );		//�������Đ�
extern BOOL	PMSND_CheckPlaySEVoice( void );		//�r�d�I�����o
extern void	PMSND_SetStatusSEVoice
				( int tempoRatio, int pitch, int pan );//�r�d�������X�e�[�^�X�ύX
extern BOOL	PMSND_SetEchoChorus( u32 wait, int pitch );

//============================================================================================
/**
 *
 * @brief	�V�X�e���t�F�[�h�t���[���ݒ�i�eapp����Ăяo�����j
 *
 */
//============================================================================================
extern void PMSND_SetSystemFadeFrames( int frames );

//============================================================================================
/**
 *
 * @brief	�V�X�e���G�R�[���R�[���X�ݒ�i�eapp����Ăяo�����j
 *
 */
//============================================================================================
extern void PMSND_EnableSystemEchoChorus( int volume, int pitch, int waitFrames );
extern void PMSND_DisableSystemEchoChorus( void );

//�T�E���h�e�X�g�p
//�����݃}�b�v�؂�ւ�莞�̃f�[�^�擾�Ɛ퓬�Ȃ�SubProc����A���Ă���
//�@�ꏊ���؂蕪�����Ă��Ȃ��̂ŉ��Ńt���O�Ǘ�����
extern BOOL debugBGMsetFlag;

#endif
