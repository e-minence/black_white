
//=============================================================================================
/**
 * @file	fade.h
 * @brief	��ʃt�F�[�h�V�X�e��
 * @author	Hisashi Sogabe
 * @date	2007/01/18
 */
//=============================================================================================

#ifndef _FADE_H_
#define _FADE_H_

#ifdef __cplusplus
extern "C" {
#endif

//=========================================================================
//	�萔��`
//=========================================================================
//�t�F�[�h���[�h�w��
#define	GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN	(0x0001)		//�}�X�^�[�P�x�u���b�N�A�E�g�i���C����ʁj
#define	GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB		(0x0002)		//�}�X�^�[�P�x�u���b�N�A�E�g�i�T�u��ʁj
#define	GFL_FADE_MASTER_BRIGHT_BLACKOUT		    (0x0003)		//�}�X�^�[�P�x�u���b�N�A�E�g�i����ʁj
#define	GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN	(0x0004)		//�}�X�^�[�P�x�z���C�g�A�E�g�i���C����ʁj
#define	GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB		(0x0008)		//�}�X�^�[�P�x�z���C�g�A�E�g�i�T�u��ʁj
#define	GFL_FADE_MASTER_BRIGHT_WHITEOUT		    (0x000C)		//�}�X�^�[�P�x�z���C�g�A�E�g�i����ʁj
#define	GFL_FADE_SOFT_CALC_FADE					(0x0010)		//�v���O�����v�Z�ɂ��J���[�����Z�i�������j

#define	GFL_FADE_MASTER_BRIGHT	(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | \
								 GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB | \
								 GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN | \
								 GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB)

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�V�X�e��������
 *
 * @param	heapID	�q�[�v�h�c
 *
 * @return	�擾�����������̃A�h���X
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_FADE_Init( u32 heapID );

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�V�X�e�����C��
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_FADE_Main( void );

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�V�X�e���I��
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_FADE_Exit( void );

//--------------------------------------------------------------------------------------------
/**
 * �}�X�^�[�P�x���N�G�X�g
 *
 * @param	mode		�t�F�[�h���[�h�iGFL_FADE_MASTER_BRIGHT_BLACKOUT or GFL_FADE_MASTER_BRIGHT_WHITEOUT�j
 * @param	start_evy	�X�^�[�g�P�x�i0�`16�j
 * @param	end_evy		�G���h�P�x�i0�`16�j
 * @param	wait		�t�F�[�h�X�s�[�h
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_FADE_SetMasterBrightReq( int mode, int start_evy, int end_evy, int wait );

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h���s���̃`�F�b�N
 *
 * @reval	TRUE:���s�� FALSE:�����s
 */
//--------------------------------------------------------------------------------------------
extern	BOOL	GFL_FADE_CheckFade( void );

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif //_FADE_H_
