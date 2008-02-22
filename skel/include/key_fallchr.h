
//============================================================================================
/**
 * @file	fallchr.h
 * @brief	DS�Ń��b�V�[�̂��܂������L�����w�b�_�[
 * @date	2007.02.06
 */
//============================================================================================

#ifndef	__FALL_CHR_H__
#define	__FALL_CHR_H__

#define GLOBAL extern

//----------------------------------------------------------------------------
/**
 *	@brief	�����L�����N�^�p�����[�^�\����
 */
//-----------------------------------------------------------------------------
struct fall_chr_param{
	GAME_PARAM		*gp;
	GFL_CLWK		*clwk;
	GFL_CLACTPOS	now_pos;
	GFL_CLACTPOS	offset_pos;

	u8			seq_no;			//�V�[�P���X�i���o�[
	u8			push_seq_no;	//�V�[�P���X�i���o�[
	u8			player_no;		//�v���[���[�i���o�[
	u8			type;			//�L�����̎��

	u8			line_no;		//���݂��郉�C��
	u8			dir;			//�L�����N�^�̌���
	u8			clact_no;		//�Z���A�N�^�[�i���o�[
	u8			fall_wait;

	int			speed_value;	//�ړ��X�s�[�h�i�[���[�N�iROTATE�AEGG_MAKE�Ȃǁj

	u8			wait_value;		//�E�G�C�g�n�̊i�[���[�N
	u8			fall_wait_tmp;
	u8			rotate_flag		:2;		//��]�t���O
	u8			overturn_flag	:1;		//�Ђ�����Ԃ��t���O
	u8			egg_make_flag	:1;
	u8							:4;		//�A���������t���O
	u8			chr_count;		//�^�}�S�L�����̎��̂͂���ł���^�}�S�̐�
	int			birth_wait;		//���܂��܂ł̃E�G�C�g
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
GLOBAL	FALL_CHR_PARAM	*KEY_YT_InitFallChr(GAME_PARAM *gp,u8 player_no,u8 type,u8 line_no);

//----------------------------------------------------------------------------
/**
 *	@brief	Network�p�����L��������
 *	
 *	@param	gp			�Q�[���p�����[�^�|�C���^
 *	@param	player_no	1P or 2P
 *	@param	type		�L�����^�C�v
 *	@param	line_no		�������C���i���o�[
 *
 *	@retval GFL_CLWK*
 */
//-----------------------------------------------------------------------------
GLOBAL GFL_CLWK* KEY_YT_InitNetworkFallChr(GAME_PARAM *gp,u8 player_no,u8 type,u8 line_no);

//----------------------------------------------------------------------------
/**
 *	@brief	�^�}�S�����`�F�b�N
 *	
 *	@param	ps		�v���[���[�X�e�[�^�X
 */
//-----------------------------------------------------------------------------
GLOBAL	void	KEY_YT_EggMakeCheck(KEY_YT_PLAYER_STATUS *ps);


//----------------------------------------------------------------------------
/**
 *	@brief	���b�V�[�̐��܂��A�j���[�V�����^�X�N���Z�b�g
 *	
 *	@param	gs		�Q�[���p�����[�^�|�C���^
 *	@param	ps		�v���[���[�X�e�[�^�X
 *	@param	pos_x		
 *	@param	pos_y		
 *	@param	count		
 */
//-----------------------------------------------------------------------------
GLOBAL void KEY_YT_YossyBirthAnimeTaskSet(GAME_PARAM *gp,KEY_YT_PLAYER_STATUS *ps,u8 pos_x,u8 pos_y,u8 count);


#endif	__FALL_CHR_H__
