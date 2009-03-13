//============================================================================================
/**
 * @file	pm_sndsys.h
 * @brief	�|�P�����T�E���h�V�X�e��
 * @author	
 * @date	
 */
//============================================================================================
//============================================================================================
/**
 *
 * @brief	�T�E���h�V�X�e���imain.c������Ăяo�����j
 *
 */
//============================================================================================
extern void	PMSNDSYS_Init( void );
extern void	PMSNDSYS_Main( void );
extern void	PMSNDSYS_Exit( void );

//============================================================================================
/**
 *
 * @brief	�T�E���h�֐��i�eapp����Ăяo�����j
 *
 */
//============================================================================================
extern NNSSndHandle* PMSNDSYS_GetBGMhandlePointer( void );

extern BOOL	PMSNDSYS_PlayBGM( u32 soundIdx );
extern BOOL	PMSNDSYS_PlayBGM_EX( u32 soundIdx, u16 trackBit );
extern void	PMSNDSYS_ChangeBGMtrack( u16 trackBit );

extern void	PMSNDSYS_StopBGM( void );
extern void	PMSNDSYS_PauseBGM( BOOL pauseFlag );

extern void	PMSNDSYS_PushBGM( void );
extern void	PMSNDSYS_PopBGM( void );

extern BOOL	PMSNDSYS_PlaySE( u32 soundNum );
extern BOOL	PMSNDSYS_PlayVoice( u32 pokeNum );

//�T�E���h�e�X�g�p
//�����݃}�b�v�؂�ւ�莞�̃f�[�^�擾�Ɛ퓬�Ȃ�SubProc����A���Ă���
//�@�ꏊ���؂蕪�����Ă��Ȃ��̂ŉ��Ńt���O�Ǘ�����
extern BOOL debugBGMsetFlag;

