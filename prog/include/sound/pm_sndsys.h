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
#define PMSND_BGM_END			(SEQ_MUS_END)
#define PMSND_SE_START		(SE_START)
#define PMSND_SE_END			(SEQ_SE_END)

//------------------------------------------------------------------
/**
 * @brief	�r�d�v���[���[�ݒ��`
 */
//------------------------------------------------------------------
typedef enum {
	SEPLAYER_SYS = 0,
	SEPLAYER_SE1,
	SEPLAYER_SE2,

}SEPLAYER_ID;

#define SEPLAYER_MAX (3)
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
extern u32						PMSND_GetSndHeapFreeSize( void );
extern NNSSndHandle*	PMSND_GetBGMhandlePointer( void );
extern u32						PMSND_GetBGMsoundNo( void );
extern u32						PMSND_GetNextBGMsoundNo( void );	//�Đ��\��or�Đ�����IDX�擾
extern u32						PMSND_GetBGMplayerNoIdx( void );
extern BOOL						PMSND_CheckOnReverb( void );

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
//�a�f�l���Đ�
extern void	PMSND_PlayBGM_EX( u32 soundIdx, u16 trackBit );
#define PMSND_PlayBGM( soundIdx ) PMSND_PlayBGM_EX( soundIdx, 0xffff )	//��L�ȈՔ�
//�a�f�l�����t�F�[�h�Đ�
extern void	PMSND_PlayNextBGM_EX
		( u32 soundIdx, u16 trackBit, u8 fadeOutFrame, u8 fadeInFrame );	
#define PMSND_PlayNextBGM( soundIdx, fadeInFrame, fadeOutFrame ) \
					PMSND_PlayNextBGM_EX( soundIdx, 0xffff, fadeInFrame, fadeOutFrame );	//��L�ȈՔ�
//�a�f�l�I�����o(TRUE���s��)
extern BOOL	PMSND_CheckPlayBGM( void );
//�a�f�l�̍Đ��g���b�N�ύX
extern void	PMSND_ChangeBGMtrack( u16 trackBit );
//�a�f�l�X�e�[�^�X�ύX
extern void	PMSND_SetStatusBGM( int tempoRatio, int pitch, int pan );

//���݂̂a�f�l���~
extern void	PMSND_StopBGM( void );
//���݂̂a�f�l���ꎞ��~(TRUE��~,FALSE�ĊJ)
extern void	PMSND_PauseBGM( BOOL pauseFlag );
//���݂̂a�f�l���t�F�[�h�C��
extern void	PMSND_FadeInBGM( u16 frames );
//���݂̂a�f�l���t�F�[�h�A�E�g
extern void	PMSND_FadeOutBGM( u16 frames );
//�t�F�[�h���s�`�F�b�N(TRUE���s��)
extern BOOL	PMSND_CheckFadeOnBGM( void );

//���݂̂a�f�l��ޔ�
extern void	PMSND_PushBGM( void );
//���݂̂a�f�l�𕜌�
extern void	PMSND_PopBGM( void );

//�V�X�e���t�F�[�h�i�����j�t���[���ݒ�
extern void PMSND_SetSystemFadeFrames( int fadeOutFrame, int fadeInFrame );

//============================================================================================
/**
 *
 * @brief	�r�d�T�E���h�֐��i�eapp����Ăяo�����j
 *
 */
//============================================================================================
//�r�d�����ݒ�SEPLAYER_ID�擾
extern SEPLAYER_ID	PMSND_GetSE_DefaultPlayerID( u32 soundIdx );
//�r�d�v���[���[�n���h���擾
extern NNSSndHandle* PMSND_GetSE_SndHandle( SEPLAYER_ID sePlayerID );

extern void	PMSND_PlaySE_byPlayerID( u32 soundIdx, SEPLAYER_ID sePlayerID );
#define PMSND_PlaySystemSE( soundNum ) PMSND_PlaySE( soundNum )
//�r�d���Đ�
extern void	PMSND_PlaySE( u32 soundIdx );
//�r�d���v���[���[���w�肵�čĐ�
extern void	PMSND_PlaySE_byPlayerID( u32 soundIdx, SEPLAYER_ID sePlayerID );
//�S�r�d���~
extern void	PMSND_StopSE( void );
//�r�d���v���[���[���w�肵�Ē�~
extern void	PMSND_StopSE_byPlayerID( SEPLAYER_ID sePlayerID );
//�S�r�d�̏I�������o(TRUE���s��)
extern BOOL	PMSND_CheckPlaySE( void );
//�r�d�̏I�����v���[���[���w�肵�ďI�����o(TRUE���s��)
extern BOOL	PMSND_CheckPlaySE_byPlayerID( SEPLAYER_ID sePlayerID );
//�r�d�̃X�e�[�^�X��ύX
extern void	PMSND_SetStatusSE( int tempoRatio, int pitch, int pan );
//�r�d�̃X�e�[�^�X���v���[���[���w�肵�ĕύX
extern void	PMSND_SetStatusSE_byPlayerID(SEPLAYER_ID sePlayerID,int tempoRatio,int pitch,int pan);

//============================================================================================
/**
 *
 *
 * @brief	�v���Z�b�g�֐����b�p�[
 *					�Ƃ肠�����V�X�e���Ńn���h�����P�����p�ӂ��ȈՓo�^�\�ɂ���
 *
 *
 */
//============================================================================================
extern void	PMDSND_PresetSoundTbl( const u32* soundIdxTbl, u32 tblNum );
extern void	PMSND_PresetGroup( u32 groupIdx );
extern void	PMSND_ReleasePreset( void );




//�T�E���h�e�X�g�p
//�����݃}�b�v�؂�ւ�莞�̃f�[�^�擾�Ɛ퓬�Ȃ�SubProc����A���Ă���
//�@�ꏊ���؂蕪�����Ă��Ȃ��̂ŉ��Ńt���O�Ǘ�����
extern BOOL debugBGMsetFlag;
#endif
