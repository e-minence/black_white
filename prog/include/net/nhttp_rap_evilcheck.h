//=============================================================================
/**
 * @file	  nhttp_rap_evilcheck.h
 * @brief	  nhttp�ŕs���`�F�b�N����Ƃ��̃��X�|���X�R�[�h�Ȃ�
 * @author	Toru=Nagihashi
 * @date    2010.02.18
 *
 */
//=============================================================================

#pragma once

//=============================================================================
/**
 *    �萔
 */
//=============================================================================
//-------------------------------------
///	���X�|���X�R�[�h
//    ���L�ȊO�̒l���ƃT�[�o�G���[
//=====================================
#define NHTTP_RAP_EVILCHECK_RESPONSE_CODE_SUCCES  200         //�s���`�F�b�N����
#define NHTTP_RAP_EVILCHECK_RESPONSE_CODE_DIRTY_GAMEMODE  400 //�Q�[�����[�h���s��
#define NHTTP_RAP_EVILCHECK_RESPONSE_CODE_DIRTY_TOKEN  401    //�F�؃g�[�N�����s��  

//-------------------------------------
///	�����̒���
//=====================================
#define NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN 128

//-------------------------------------
///	�s���`�F�b�N�̌���
//=====================================
typedef enum
{ 
	NHTTP_RAP_EVILCHECK_RESULT_OK	= 0,	//OK	�o�g���r�f�I�̏ꍇ�́A�^�}�S�t���O���I���̏ꍇ�͖������ŁAOK
	NHTTP_RAP_EVILCHECK_RESULT_ON_EVIL_FLAG	= 10,	//�s���t���O�I��	
	NHTTP_RAP_EVILCHECK_RESULT_OUTRANGE_MONSNO	= 20,	//monsno���͈͊O	
	NHTTP_RAP_EVILCHECK_RESULT_LOCK_POKE	= 21,	//�����փ|�P����	
	NHTTP_RAP_EVILCHECK_RESULT_RARE	= 22,	//�F�Ⴂ�|�P�����`�F�b�N	�F�Ⴂ������͂����Ȃ��̂ɁA�F�Ⴂ
	NHTTP_RAP_EVILCHECK_RESULT_UNAPPER_SUBWAY	= 23,	//�T�u�E�F�C�o��֎~�|�P����	�T�u�E�F�C����
	NHTTP_RAP_EVILCHECK_RESULT_RIBBON	= 24,	//�O�����{������|�P����	GTS����B�O�����{���̃|�P�����������݂��Ȃ��̂ɁAGTS�փA�b�v����Ă���
	NHTTP_RAP_EVILCHECK_RESULT_OUTRANGE_LV	= 30,	//���x���͈͊O	1�`100�͈̔͂𒴂��Ă���
	NHTTP_RAP_EVILCHECK_RESULT_LOW_LV	= 31,	//�Œ჌�x���`�F�b�N	
	NHTTP_RAP_EVILCHECK_RESULT_OUTRANGE_FORMNO	= 32,	//�|�P�����̃t�H�����ԍ����͈͂𒴂��Ă���	
	NHTTP_RAP_EVILCHECK_RESULT_NOT_PRESENT_FLAG	= 33,	//�z�z����|�P�����Ȃ̂ɁA�z�z�t���O���Ȃ�	
	NHTTP_RAP_EVILCHECK_RESULT_EMPTY_NICKNAME	= 40,	//�j�b�N�l�[������	
	NHTTP_RAP_EVILCHECK_RESULT_UNDEF_NICKNAME	= 41,	//�j�b�N�l�[���ɖ���`�̕������g���Ă���	
	NHTTP_RAP_EVILCHECK_RESULT_NOEOM_NICKNAME	= 42,	//�j�b�N�l�[����EOM���Ȃ�	
	NHTTP_RAP_EVILCHECK_RESULT_EMPTY_OYANAME	= 45,	//�e������	
	NHTTP_RAP_EVILCHECK_RESULT_UNDEF_OYANAME	= 46,	//�e���ɖ���`�̕������g���Ă���	
	NHTTP_RAP_EVILCHECK_RESULT_NOEOM_OYANAME	= 47,	//�e����EOM���Ȃ�	
	NHTTP_RAP_EVILCHECK_RESULT_ERR_CATCH_BALL	= 50,	//�߂܂����{�[���G���[	������́A�ォ��g����{�[�����ύX�ɂȂ�\�������邩�H
	NHTTP_RAP_EVILCHECK_RESULT_ERR_INIT_BALL	= 51,	//�����|�P�����̃����X�^�[�{�[���`�F�b�N	�����R�́i�`�R�E�q�m�E���j�������j�̃{�[���������X�^�[�{�[���ȊO�Ȃ�NG
	NHTTP_RAP_EVILCHECK_RESULT_ERR_EGG_BALL	= 52,	//�^�}�S�|�P�̃����X�^�[�{�[���`�F�b�N	�^�}�S����z�����|�P�������u�����X�^�[�{�[���v�ȊO�̃{�[���ɓ����Ă���
	NHTTP_RAP_EVILCHECK_RESULT_ERR_TOKUSEI	= 60,	//�������s��	
	NHTTP_RAP_EVILCHECK_RESULT_ERR_ITEM	= 61,	//�����A�C�e�����s��	
	NHTTP_RAP_EVILCHECK_RESULT_ZERO_COUNTRY	= 70,	//�|�P�����̍��R�[�h�Ƃ�0���w��Ȃ�NG	
	NHTTP_RAP_EVILCHECK_RESULT_OUTRANGE_DORYOKUTI	= 80,	//�w�͒l���p�����[�^�O	+n:0��HP�@1���������� 2���ڂ�����@3�����΂₳�@4���Ƃ������@5���Ƃ��ڂ�
	NHTTP_RAP_EVILCHECK_RESULT_OVER_DORYOKUTI	= 90,	//�w�͒l�̍��v��510�I�[�o�[	
	NHTTP_RAP_EVILCHECK_RESULT_OUTRANGE_POWRND_S	= 100,	//�p���[�������͈͊O	+n:0��HP�@1���������� 2���ڂ�����@3�����΂₳�@4���Ƃ������@5���Ƃ��ڂ�
	NHTTP_RAP_EVILCHECK_RESULT_OUTRANGE_POWRND_E	= 105,    //���`���̒l�܂ł��G���[�l
	NHTTP_RAP_EVILCHECK_RESULT_ERR_CALC_POWRND_S	= 110,	//�\�͒l�̌v�Z�l�͂���Ȃ�	+n:0��HP�@1���������� 2���ڂ�����@3�����΂₳�@4���Ƃ������@5���Ƃ��ڂ�
	NHTTP_RAP_EVILCHECK_RESULT_ERR_CALC_POWRND_E	= 115,	//���`���̒l�܂ł��G���[�l
	NHTTP_RAP_EVILCHECK_RESULT_OUTRANGE_WAZA_S	= 120,	//�Z�̔ԍ����͈͂𒴂��Ă���	+n�͉��ڂ̋Z���Ђ�����������
	NHTTP_RAP_EVILCHECK_RESULT_OUTRANGE_WAZA_E	= 123,	//���`���̒l�܂ł��G���[�l
	NHTTP_RAP_EVILCHECK_RESULT_OVERLAP_WAZA_S	= 130,	//�����Z�������Ă���	      +n�͉��ڂ̋Z���Ђ�����������
	NHTTP_RAP_EVILCHECK_RESULT_OVERLAP_WAZA_E	= 133,	//���`���̒l�܂ł��G���[�l
	NHTTP_RAP_EVILCHECK_RESULT_EMPTY_SLOT1_WAZA	= 140,	//�P�X���b�g�ڂɋZ���Z�b�g����Ă��Ȃ�	
	NHTTP_RAP_EVILCHECK_RESULT_BETWEEN_WAZA = 141,	//�o���Ă��Ȃ��킴���r���ɂ���	
	NHTTP_RAP_EVILCHECK_RESULT_HAVE_CANT_WAZA_S	= 150,	//�o�����Ȃ��Z���o���Ă���	                +n�͉��ڂ̋Z���Ђ�����������
	NHTTP_RAP_EVILCHECK_RESULT_HAVE_CANT_WAZA_E	= 153,	//���`���̒l�܂ł��G���[�l
	NHTTP_RAP_EVILCHECK_RESULT_NOT_PALPOKE_HAVE_AGB_WAZA_S	= 160,	//AGB�Z���p���p�[�N�ȊO���o���Ă���	          +n�͉��ڂ̋Z���Ђ�����������
	NHTTP_RAP_EVILCHECK_RESULT_NOT_PALPOKE_HAVE_AGB_WAZA_E	= 163,	//���`���̒l�܂ł��G���[�l
	NHTTP_RAP_EVILCHECK_RESULT_NOT_PALPOKE_HAVE_XD_WAZA_S	= 170,	//XD�Z���p���p�[�N�ȊO���o���Ă���	          +n�͉��ڂ̋Z���Ђ�����������
	NHTTP_RAP_EVILCHECK_RESULT_NOT_PALPOKE_HAVE_XD_WAZA_E	= 173,	//���`���̒l�܂ł��G���[�l
	NHTTP_RAP_EVILCHECK_RESULT_NOT_EGGPOKE_HAVE_EGG_WAZA_S	= 180,	//�^�}�S�Z���^�}�S���琶�܂ꂽ�|�P�����ȊO��  +n�͉��ڂ̋Z���Ђ�����������
	NHTTP_RAP_EVILCHECK_RESULT_NOT_EGGPOKE_HAVE_EGG_WAZA_E	= 183,	//���`���̒l�܂ł��G���[�l
	NHTTP_RAP_EVILCHECK_RESULT_OVERLAP_AGB_XD_WAZA	= 190,	//XD�Z�ƃ^�}�S�Z�̏d��	
	NHTTP_RAP_EVILCHECK_RESULT_OVERLAP_PRESENT_EGG_WAZA	= 191,	//�z�z�Z�ƃ^�}�S�Z�̏d��	
	NHTTP_RAP_EVILCHECK_RESULT_OUTRANGE_SEIKAKU	= 200,	//�u���������v���͈͊O	
	NHTTP_RAP_EVILCHECK_RESULT_ERR_SEX	= 201,	//���ʂ��A���肦�Ȃ��l	
	NHTTP_RAP_EVILCHECK_RESULT_ERR_PLACE	= 210,	//�߂܂����ꏊ�^���܂ꂽ�ꏊ���s��	
	NHTTP_RAP_EVILCHECK_RESULT_ERR_PALPOKE	= 211,	//�p���p�[�N����|�P�����̕߂܂����ꏊ���p���p�[�N�łȂ�	
	NHTTP_RAP_EVILCHECK_RESULT_ERR_NICKNAME	= 250,	//�j�b�N�l�[����NG	�G���[�ɂ��邩�́A���[�h�ɂ��
	NHTTP_RAP_EVILCHECK_RESULT_ERR_OYANAME	= 251,	//�e����NG	�G���[�ɂ��邩�́A���[�h�ɂ��
	NHTTP_RAP_EVILCHECK_RESULT_ERR_NICK_OYA_NAME	= 252,	//�j�b�N�l�[���E�e������NG	�G���[�ɂ��邩�́A���[�h�ɂ��
} NHTTP_RAP_EVILCHECK_RESULT;


//=============================================================================
/**
 *  �O�����J�֐�
 */
//=============================================================================
//=============================================================================
/**
 *  �s�������ŕԂ��Ă����f�[�^�̒��g���擾����
 */
//=============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����s������ �X�e�[�^�X�R�[�h���擾
 * @param   cp_data  NHTTP_RAP_GetRecvBuffer�̒��g
 *
 * @retval  �X�e�[�^�X�R�[�h  �O�Ȃ�ΐ���  �P�Ȃ�Ύ��s
 */
//------------------------------------------------------------------------------
extern u8 NHTTP_RAP_EVILCHECK_GetStatusCode( const void *cp_data );

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����s������ �|�P�����s�������̌���
 * @param   cp_data     NHTTP_RAP_GetRecvBuffer�̒��g
 * @param   poke_index  �s���`�F�b�N�̂Ƃ��ɑ������|�P�����̈ʒu
 *
 * @retval  �s�������̌���  �iNHTTP_RAP_EVILCHECK_RESULT�̍��ڎQ�Ɓj
 */
//------------------------------------------------------------------------------
extern NHTTP_RAP_EVILCHECK_RESULT NHTTP_RAP_EVILCHECK_GetPokeResult( const void *cp_data, int poke_index );

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����s������  �������擾
 *
 *  @param    cp_data     NHTTP_RAP_GetRecvBuffer�̒��g
 *  @param    poke_max    �s���`�F�b�N�̂Ƃ��ɑ������|�P�����̍ő吔
 *
 *	@return �����ւ̃A�h���X  �R�s�[����ꍇ�A��������NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN���Amemcpy���Ă�������
 */
//-----------------------------------------------------------------------------
extern const s8 * NHTTP_RAP_EVILCHECK_GetSign( const void *cp_data, int poke_max );

//=============================================================================
/**
 *  �����ƃ|�P�����̏ؖ�������֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����s������  �����ƃ|�P��������v���Ă��邱�Ƃ��ؖ�����
 *
 *	@param	const void *p_poke_buff �|�P�����f�[�^���i�[����Ă���o�b�t�@  (�ȉ��̂R�̊֐����g���Ă�������)
 *	@param	poke_size               �s���`�F�b�N�̎��ɑ������P�̂�����̃T�C�Y
 *	@param	poke_max                �s���`�F�b�N�̎��ɑ������|�P�����̍ő吔
 *	@param	s8 *cp_sign             �s���`�F�b�N�Ŏ󂯎�����T�C��
 *
 *	@return TRUE��v���Ă���  FALSE��v���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
extern BOOL NHTTP_RAP_EVILCHECK_VerifySign( const void *p_poke_buff, int poke_size, int poke_max, const s8 *cp_sign );

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����ؖ��̂��߂̃o�b�t�@���쐬����
 *	        NHTTP_RAP_PokemonEvilCheckCreate�͑��M�o�b�t�@�̍쐬�ł����A
 *	        ������́A�ؖ��̂��߂�POKEPARTY����NHTTP_RAP_EVILCHECK_VerifySign�Ŏg�p����o�b�t�@��
 *	        �쐬���邽�߂Ɏg���܂�
 *
 *	@param	poke_size               �s���`�F�b�N�̎��ɑ������P�̂�����̃T�C�Y
 *	@param	poke_max                �s���`�F�b�N�̎��ɑ������|�P�����̍ő吔
 *	@param	HEAPID heapID �o�b�t�@�쐬�̂��߂̃q�[�vID
 *
 *	@return �o�b�t�@
 */
//-----------------------------------------------------------------------------
extern void * NHTTP_RAP_EVILCHECK_CreateVerifyPokeBuffer( int poke_size, int poke_max, HEAPID heapID );

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����ؖ��̂��߂̃o�b�t�@��j������
 *
 *	@param	void *p_buff �o�b�t�@
 */
//-----------------------------------------------------------------------------
extern void NHTTP_RAP_EVILCHECK_DeleteVerifyPokeBuffer( void *p_buff );

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����ؖ��o�b�t�@�փ|�P�����f�[�^��ǉ�����
 *
 *	@param	void *p_buff            �o�b�t�@
 *	@param	void *cp_data           �|�P�����f�[�^
 *	@param	poke_size               �s���`�F�b�N�̎��ɑ������P�̂�����̃T�C�Y
 *	@param	poke_index              �i�[���鏇�ԃC���f�b�N�X
 */
//-----------------------------------------------------------------------------
extern void NHTTP_RAP_EVILCHECK_AddPokeVerifyPokeBuffer( void *p_buff, const void *cp_data, int poke_size, int poke_index );
