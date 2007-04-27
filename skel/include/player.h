
//============================================================================================
/**
 * @file	player.h
 * @brief	DS�Ń��b�V�[�̂��܂��^�C�g���w�b�_�[
 * @date	2007.02.08
 */
//============================================================================================

#ifndef	__PLAYER_H__
#define	__PLAYER_H__

#undef GLOBAL
#ifdef __PLAYER_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

/// �v���[���[�\����
typedef struct _PLAYER_PARAM PLAYER_PARAM;


//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[������
 *	
 *	@param	gp			�Q�[���p�����[�^�|�C���^
 *	@param	player_no	1P or 2P
 *	@param	type		�v���[���[�^�C�v
 *	@param	bNetSend	���M�f�[�^�𑗂�K�v������Ƃ�TRUE
 */
//-----------------------------------------------------------------------------
GLOBAL PLAYER_PARAM*	YT_InitPlayer(GAME_PARAM *gp,u8 player_no,u8 type,u8 bNetSend);

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[���^�X�N�ɓo�^
 *	
 *	@param	gp			�Q�[���p�����[�^�|�C���^
 *	@param	type		�v���[���[�^�C�v
 *	@param	player_no	1P or 2P
 *  @retval void
 */
//-----------------------------------------------------------------------------
GLOBAL void YT_InitPlayerAddTask(GAME_PARAM *gp, PLAYER_PARAM* pp,u8 player_no);

//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�ł����v���[���[�A�j�����Z�b�g
 *	
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	anm_no	�Z�b�g����A�j���i���o�[
 */
//-----------------------------------------------------------------------------
GLOBAL void	YT_NetPlayerChrTrans(GAME_PARAM *gp,PLAYER_PARAM *pp,int player_no,int anm_no,int line_no,int rot,int pat_no);

//----------------------------------------------------------------------------
/**
 *	@brief	�v���[���[�\���p�X�N���[������
 *	
 *	@param	pp			�v���[���[�p�����[�^�|�C���^
 *	@param	old_line_no	���݂̃v���[���[�̃��C���ʒu
 *	@param	new_line_no	�V���ȃv���[���[�̃��C���ʒu
 */
//-----------------------------------------------------------------------------
GLOBAL void YT_PlayerScreenMakeNetFunc(PLAYER_PARAM *pp, u8 player_no, u8 old_line_no,u8 new_line_no);

//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�p �v���[���[�\���p�X�N���[�������i�^�[������Ƃ��j
 *	
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	flag	0:�^�[���J�n���@1:�^�[���I����
 */
//-----------------------------------------------------------------------------

GLOBAL void	YT_PlayerRotateScreenMakeNetFunc(PLAYER_PARAM *pp, u8 player_no, u8 line_no, u8 flag);

//----------------------------------------------------------------------------
/**
 *	@brief	���������t���O�𗧂Ă�
 *	@param	pp		�v���[���[�p�����[�^�|�C���^
 *	@param	bWin	�������ꍇ��TRUE
 */
//-----------------------------------------------------------------------------
GLOBAL void YT_PlayerSetWinLoseFlag(YT_PLAYER_STATUS *ps, int bWin);

#endif	__PLAYER_H__
