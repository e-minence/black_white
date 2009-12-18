//============================================================================================
/**
 * @file	pm_voice.h
 * @brief	�|�P���������g�`�Đ���{�V�X�e��	
 * @author	
 * @date	
 */
//============================================================================================
#ifndef __PMVOICE_H__
#define __PMVOICE_H__

//------------------------------------------------------------------
/**
 * @brief	�����I�v�V����
 */
//------------------------------------------------------------------
enum {
	PMVOICE_MODE_NORMAL = 0,
	PMVOICE_MODE_CHORUS = 1,
	PMVOICE_MODE_REVERSE = 2,
};

//------------------------------------------------------------------
/**
 * @brief	�J�X�^�}�C�Y�R�[���o�b�N�p��`
 */
//------------------------------------------------------------------
// �g�`IDX�擾�p�R�[���o�b�N
typedef void (PMVOICE_CB_GET_WVIDX)(	u32 pokeNo, 		// [in]�|�P�����i���o�[
										u32 pokeFormNo,		// [in]�|�P�����t�H�[���i���o�[
										u32* waveIdx );		// [out]�g�`IDX
// �g�`�J�X�^�}�C�Y�R�[���o�b�N(TRUE: �R�[���o�b�N���Ő���)�@
typedef BOOL (PMVOICE_CB_GET_WVDAT)(	u32 pokeNo,			// [in]�|�P�����i���o�[
										u32 pokeFormNo,		// [in]�|�P�����t�H�[���i���o�[
										u32 userParam,		// [in]���[�U�[�p�����[�^�[
										void** wave,		// [out]�g�`�f�[�^
										u32* size,			// [out]�g�`�T�C�Y
										int* rate,			// [out]�g�`�Đ����[�g
										int* speed );		// [out]�g�`�Đ��X�s�[�h

//============================================================================================
/**
 *
 * @brief	�V�X�e���imain.c������Ăяo�����j
 *
 */
//============================================================================================
extern void	PMVOICE_Init
			( HEAPID heapID,							// �g�pheapID 
			  PMVOICE_CB_GET_WVIDX* CallBackGetWaveIdx,	// �g�`IDX�擾�p�R�[���o�b�N
			  PMVOICE_CB_GET_WVDAT* CallBackCustomWave 	// �g�`�J�X�^�}�C�Y�R�[���o�b�N
			);
extern void	PMVOICE_Reset( void );
extern void	PMVOICE_Main( void );
extern void	PMVOICE_Exit( void );
extern BOOL	PMVOICE_CheckBusy( void );

//============================================================================================
/**
 *
 * @brief	�V�X�e���ݒ�
 *
 */
//============================================================================================
extern void	PMVOICE_PlayerHeapReserve( int num, HEAPID heapID );// �v���[���[�pwave�o�b�t�@���O�m��
extern void	PMVOICE_PlayerHeapRelease( void );		// �v���[���[�pwave�o�b�t�@�J��

//============================================================================================
/**
 *
 * @brief	�����T�E���h�֐�
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�����Đ��֐�(return: voicePlayerIdx)
 */
//------------------------------------------------------------------
extern u32	PMVOICE_Play
			(	u32		pokeNo,			// �|�P�����i���o�[
				u32		pokeFormNo,		// �|�P�����t�H�[���i���o�[
				u8		pan,			// ���(L:0 - 64 - 127:R)
				BOOL	chorus,			// �R�[���X�g�p�t���O
				int		chorusVolOfs,	// �R�[���X�{�����[����
				int		chorusSpOfs,	// �Đ����x��
				BOOL	reverse,		// �t�Đ��t���O
				u32		userParam		// ���[�U�[�p�����[�^�[	
			);		
//------------------------------------------------------------------
/**
 * @brief	�����X�e�[�^�X�ύX�֐�
 */
//------------------------------------------------------------------
extern void	PMVOICE_SetPan( u32 voicePlayerIdx, u8 pan);
extern void	PMVOICE_SetVolume( u32 voicePlayerIdx, int volOfs);
extern void	PMVOICE_SetSpeed( u32 voicePlayerIdx, int spdOfs);
extern void	PMVOICE_SetStatus( u32 voicePlayerIdx, u8 pan, int volOfs, int spdOfs );
//------------------------------------------------------------------
/**
 * @brief	����������~�֐�
 */
//------------------------------------------------------------------
extern void	PMVOICE_Stop( u32 voicePlayerIdx );			//�������~
//------------------------------------------------------------------
/**
 * @brief	�����I�����o�֐�(TRUE: �Đ���)
 */
//------------------------------------------------------------------
extern BOOL	PMVOICE_CheckPlay( u32 voicePlayerIdx );	//�����I�����o(TRUE:�Đ���)





#endif

