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

//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE //�ʏ펞����
  #include "sound/wb_sound_data.sadl"		//�T�E���h���x���t�@�C��
#else

//�}���`�u�[�g����2��ނ���̂ł��̃w�b�_�̑O�ɌĂ�ł������ƁB

#endif //MULTI_BOOT_MAKE
//------------------------------------------------------------------
/**
 * @brief	�f�[�^�h�m�c�d�w�L���͈�
 */
//------------------------------------------------------------------
#define PMSND_BGM_START		(BGM_START)
#define PMSND_BGM_END			(SEQ_BGM_END)
#define PMSND_SE_START		(SE_START)
#define PMSND_SE_END			(SEQ_SE_END)
#define PMSND_ME_START		(ME_START)
#define PMSND_ME_END			(SEQ_ME_END)

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�ݒ�g����`
 */
//------------------------------------------------------------------
#define PMSND_PLAYER_MAX (SEPLAYER_SE3 + 1)

#define PMSND_MASKPL_BGM	(1<<PLAYER_BGM)
#define PMSND_MASKPL_SYS	(1<<PLAYER_SE_SYS)
#define PMSND_MASKPL_SE1	(1<<PLAYER_SE_1)
#define PMSND_MASKPL_SE2	(1<<PLAYER_SE_2)
#define PMSND_MASKPL_PSG	(1<<PLAYER_SE_PSG)
#define PMSND_MASKPL_SE3	(1<<PLAYER_SE_3)

#define PMSND_MASKPL_EFFSE	(PMSND_MASKPL_SE1|PMSND_MASKPL_SE2|PMSND_MASKPL_PSG|PMSND_MASKPL_SE3)
#define PMSND_MASKPL_ALLSE	(PMSND_MASKPL_SYS|PMSND_MASKPL_EFFSE)
#define PMSND_MASKPL_ALL	(PMSND_MASKPL_BGM|PMSND_MASKPL_ALLSE)

//------------------------------------------------------------------
/**
 * @brief	BGM �t�F�[�h�t���[����
 */
//------------------------------------------------------------------
#define PMSND_FADE_FAST    (6)  // �Z 
#define PMSND_FADE_SHORT  (30)  // ���Z
#define PMSND_FADE_NORMAL (60)  // ��
#define PMSND_FADE_LONG   (90)  // ��

//------------------------------------------------------------------
/**
 * @brief	�r�d�v���[���[�ݒ��`
 */
//------------------------------------------------------------------
typedef enum {
	SEPLAYER_SYS = 0,
	SEPLAYER_SE1,
	SEPLAYER_SE2,
	SEPLAYER_SE_PSG,
	SEPLAYER_SE3,

}SEPLAYER_ID;

#define SEPLAYER_MAX (5)

//------------------------------------------------------------------
/**
 * @brief	�֐������p��`
 */
//------------------------------------------------------------------
#define PMSND_NOEFFECT (-1)

//------------------------------------------------------------------
/**
 * @brief	�Đ��۔���R�[���o�b�N�ݒ�
 */
//------------------------------------------------------------------
typedef BOOL (*PMSND_PLAYABLE_CALLBACK)( u32 soundIdx );

//�Đ��۔���R�[���o�b�N�֐��̓o�^
extern void PMSND_SetPlayableCallBack( PMSND_PLAYABLE_CALLBACK func );
extern void PMSND_ResetPlayableCallBack( void );

//============================================================================================
/**
 *
 * @brief	�T�E���h�V�X�e���imain.c������Ăяo�����j
 *
 */
//============================================================================================
extern void	PMSND_Init( void );
extern void	PMSND_InitMultiBoot( void* sndData );
extern void	PMSND_Main( void );
extern void	PMSND_Exit( void );


//	�X�e���I/���m�����ݒ�
extern void	PMSND_SetStereo( BOOL flag );

//	���擾
#ifdef PM_DEBUG
extern u32 PMSND_GetSndHeapSize( void );
extern u32 PMSND_GetSndHeapRemainsAfterSys( void );
extern u32 PMSND_GetSndHeapRemainsAfterPlayer( void );
extern u32 PMSND_GetSndHeapRemainsAfterPresetSE( void );
extern u32 PMSND_GetSEPlayerNum( void );
#endif
extern u32						PMSND_GetSndHeapSize( void );
extern u32						PMSND_GetSndHeapFreeSize( void );
extern NNSSndHandle*	PMSND_GetNowSndHandlePointer( void );
extern NNSSndHandle*	PMSND_GetBGMhandlePointer( void );
extern u32						PMSND_GetBGMsoundNo( void );
extern u32						PMSND_GetNextBGMsoundNo( void );	//�Đ��\��or�Đ�����IDX�擾
extern u32						PMSND_GetBGMplayerNoIdx( void );
extern BOOL						PMSND_CheckOnReverb( void );
extern u8							PMSND_GetBGMTrackVolume( int trackNo );
extern BOOL           PMSND_IsLoading( void );  // ���[�h�����ǂ���

//	���o�[�u�ݒ�
extern void PMSND_EnableCaptureReverb( u32 depth, u32 samplingRate, int volume, int stopFrames );
extern void PMSND_DisableCaptureReverb( void );
extern void PMSND_ChangeCaptureReverb( u32 depth, u32 samplingRate, int volume, int stopFrames );

//============================================================================================
/**
 *
 * @brief	�v���[���[�̉��ʃR���g���[��(ON/OFF)
 *					������32bit = 32�v���[���[�܂őΉ�
 *					in: bitmask : �ŉ���bit����0�`�̃v���[���[�w��
 *												��L�v���[���[�ݒ�g����`���Q��
 */
//============================================================================================
extern void PMSND_AllPlayerVolumeEnable( BOOL playerON, u32 bitmask );

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
#define PMSND_PlayNextBGM( soundIdx, fadeOutFrame, fadeInFrame ) \
					PMSND_PlayNextBGM_EX( soundIdx, 0xffff, fadeOutFrame, fadeInFrame );	//��L�ȈՔ�
//�a�f�l�I�����o(TRUE���s��)
extern BOOL	PMSND_CheckPlayBGM( void );
//�a�f�l�̍Đ��g���b�N�ύX
extern void	PMSND_ChangeBGMtrack( u16 trackBit );
//�a�f�l�X�e�[�^�X�ύX
extern void	PMSND_SetStatusBGM_EX( u16 trackBit, int tempoRatio, int pitch, int pan );
#define PMSND_SetStatusBGM( tempoRatio, pitch, pan ) \
          PMSND_SetStatusBGM_EX( 0xffff, tempoRatio, pitch, pan )   // ��L�ȈՔ�
//�a�f�l�{�����[���ύX
extern void PMSND_ChangeBGMVolume( u16 trackBit, int volume );

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

// �������[�h�E�Đ�
extern BOOL PMSND_PlayBGMdiv(u32 no, u32* seq, BOOL start);

// �g���`�����l���Đ�
extern void	PMSND_PlayBGM_WideChannel( u32 soundIdx );
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
//�r�d���{�����[���w��t���ōĐ�(�L���l 0-127)
extern void	PMSND_PlaySEVolume( u32 soundIdx, u32 volume );
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
//�Đ����̂r�d�����͂��ꂽ�h�c�w�ƈ�v���邩�`�F�b�N
extern BOOL	PMSND_CheckPlayingSEIdx( u32 soundIdx );

/***
 * �{�����[������ 
 *  SE�̃{�����[��
 *    SE�̃{�����[���́A�V�[�P���X�{�����[���{�C�j�V�����{�����[���@
 *    2�̃{�����[���ő��삷�邱�Ƃ��o���܂��B
 *
 * �V�[�P���X�{�����[������
 *    PMSND_PlayerSetVolume
 *    �V�[�P���X�̃{�����[����ݒ肵�܂��B
 * 
 * �C�j�V�����{�����[������
 *    PMSND_PlayerSetInitialVolume
 *    �T�E���h����҂��ݒ肵���{�����[�����v���O�������ŏ㏑�����邱�Ƃ��o���܂��B
 * 
 * 
 * "�C�j�V�����{�����[���𑀍삷��ƁA�T�E���h����҂̈Ӑ}�����{�����[���o�����X���ύX�����
 * ���܂��܂��B�C�j�V�����{�����[���g�p���ɂ́A�T�E���h����҂̊m�F�����悤�ɂ��Ă��������B"
 *  
 ****/
//�r�d�v���[���[�{�����[���Z�b�g
extern void PMSND_PlayerSetVolume( SEPLAYER_ID sePlayerID, u32 vol );
//SE�C�j�V�����{�����[���Z�b�g
extern void PMSND_PlayerSetInitialVolume( SEPLAYER_ID sePlayerID, u32 vol );

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

//============================================================================================
/**
 *
 *
 * @brief	�O�����y�f�[�^�Đ�
 *
 *
 */
//============================================================================================
extern BOOL PMDSND_PresetExtraMusic( void* seqAdrs, void* bnkAdrs, u32 dummyNo );
extern BOOL  PMDSND_ChangeWaveData
							( u32 waveArcDstID, u32 waveDstIdx, void* waveArcSrcAdrs, u32 waveSrcIdx);
extern BOOL PMDSND_PlayExtraMusic( u32 dummyNo );
extern void PMDSND_StopExtraMusic( void );
extern void PMDSND_ReleaseExtraMusic( void );

#endif
