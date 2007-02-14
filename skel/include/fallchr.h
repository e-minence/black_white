
//============================================================================================
/**
 * @file	fallchr.h
 * @brief	DS�Ń��b�V�[�̂��܂������L�����w�b�_�[
 * @date	2007.02.06
 */
//============================================================================================

#ifndef	__FALL_CHR_H__
#define	__FALL_CHR_H__

#undef GLOBAL
#ifdef __FALL_CHR_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	�����L�����N�^�p�����[�^�\����
 */
//-----------------------------------------------------------------------------
struct fall_chr_param{
	GAME_PARAM	*gp;
	CLWK		*clwk;
	u8			seq_no;			//�V�[�P���X�i���o�[
	u8			push_seq_no;	//�V�[�P���X�i���o�[
	u8			player_no;		//�v���[���[�i���o�[
	u8			type;			//�L�����̎��

	u8			line_no;		//���݂��郉�C��
	u8			dir;			//�L�����N�^�̌���
	u8			clact_no;		//�Z���A�N�^�[�i���o�[
	u8			fall_wait;

	int			rotate_speed;

	u8			fall_wait_tmp;
	u8			rotate_flag		:2;		//��]�t���O
	u8			overturn_flag	:1;		//�Ђ�����Ԃ��t���O
	u8							:5;
};

//----------------------------------------------------------------------------
/**
 *	@brief	�����L����������
 *	
 *	@param	gp			�Q�[���p�����[�^�|�C���^
 *	@param	player_no	1P or 2P
 *	@param	type		�L�����^�C�v
 *	@param	line_no		�������C���i���o�[
 *
 *	@retval FALL_CHR_PARAM
 */
//-----------------------------------------------------------------------------
GLOBAL	FALL_CHR_PARAM	*YT_InitFallChr(GAME_PARAM *gp,u8 player_no,u8 type,u8 line_no);

#endif	__FALL_CHR_H__
