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

#include "sound/sound_data.h"
#include "sound/wb_sound_data.sadl"		//�T�E���h���x���t�@�C��
//------------------------------------------------------------------
/**
 * @brief	�f�[�^�h�m�c�d�w�L���͈�
 */
//------------------------------------------------------------------
#define PMSND_BGM_START		(BGM_START)
#define PMSND_BGM_END		(SEQ_WB_GTS)
#define PMSND_SE_START		(SE_START)
#define PMSND_SE_END		(SEQ_SE_GS_SLOT05)

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�ݒ��`
 */
//------------------------------------------------------------------
enum {
	PLAYER_SYSSE = 0,
	PLAYER_SEVOICE,

	PLAYER_DEFAULT_MAX,
};

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

//	���擾
extern u32				PMSND_GetSndHeapFreeSize( void );
extern NNSSndHandle*	PMSND_GetBGMhandlePointer( void );
extern u32				PMSND_GetBGMplayerNoIdx( void );
extern BOOL				PMSND_CheckOnReverb( void );

//	���o�[�u�ݒ�
extern void PMSND_EnableCaptureReverb( u32 depth, u32 samplingRate, int volume, int stopFrames );
extern void PMSND_DisableCaptureReverb( void );
extern void PMSND_ChangeCaptureReverb( u32 depth, u32 samplingRate, int volume, int stopFrames );

//============================================================================================
/**
 *
 * @brief	�a�f�l�T�E���h�֐��i�eapp����Ăяo�����j
 *
 */
//============================================================================================
extern void	PMSND_PlayBGM_EX( u32 soundIdx, u16 trackBit );				//�a�f�l���Đ�
#define PMSND_PlayBGM( soundIdx ) PMSND_PlayBGM_EX( soundIdx, 0xffff )	//��L�ȈՔ�
extern void	PMSND_PlayNextBGM_EX( u32 soundIdx, u16 trackBit );			//�a�f�l�����t�F�[�h�Đ�
#define PMSND_PlayNextBGM( soundIdx ) PMSND_PlayNextBGM_EX( soundIdx, 0xffff );	//��L�ȈՔ�
extern BOOL	PMSND_CheckPlayBGM( void );						//�a�f�l�I�����o(TRUE���s��)
extern void	PMSND_ChangeBGMtrack( u16 trackBit );			//�a�f�l�̍Đ��g���b�N�ύX
extern void	PMSND_SetStatusBGM( int tempoRatio, int pitch, int pan );//�a�f�l�X�e�[�^�X�ύX

extern void	PMSND_StopBGM( void );				//���݂̂a�f�l���~
extern void	PMSND_PauseBGM( BOOL pauseFlag );	//���݂̂a�f�l���ꎞ��~(TRUE��~,FALSE�ĊJ)
extern void	PMSND_FadeInBGM( u16 frames );		//���݂̂a�f�l���t�F�[�h�C��
extern void	PMSND_FadeOutBGM( u16 frames );		//���݂̂a�f�l���t�F�[�h�A�E�g
extern BOOL	PMSND_CheckFadeOnBGM( void );		//�t�F�[�h���s�`�F�b�N(TRUE���s��)

extern void	PMSND_PushBGM( void );				//���݂̂a�f�l��ޔ�
extern void	PMSND_PopBGM( void );				//���݂̂a�f�l�𕜌�

extern void PMSND_SetSystemFadeFrames( int frames );	//�V�X�e���t�F�[�h�i�����j�t���[���ݒ�

//============================================================================================
/**
 *
 * @brief	�r�d�T�E���h�֐��i�eapp����Ăяo�����j
 *
 */
//============================================================================================
extern void	PMSND_PlaySystemSE( u32 soundNum );	//�V�X�e���r�d���Đ�
extern void	PMSND_PlaySE( u32 soundNum );		//�r�d���Đ�
extern BOOL	PMSND_CheckPlaySE( void );			//�r�d�I�����o(TRUE���s��)
extern void	PMSND_SetStatusSE( int tempoRatio, int pitch, int pan );//�r�d�X�e�[�^�X�ύX







//�T�E���h�e�X�g�p
//�����݃}�b�v�؂�ւ�莞�̃f�[�^�擾�Ɛ퓬�Ȃ�SubProc����A���Ă���
//�@�ꏊ���؂蕪�����Ă��Ȃ��̂ŉ��Ńt���O�Ǘ�����
extern BOOL debugBGMsetFlag;
#endif
