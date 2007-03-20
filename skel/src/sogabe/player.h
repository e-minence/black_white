
//============================================================================================
/**
 * @file	player.h
 * @brief	DS�Ń��b�V�[�̂��܂� �v���[���[�w�b�_�[
 * @date	2007.02.06
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
 */
//-----------------------------------------------------------------------------
GLOBAL PLAYER_PARAM*	YT_InitPlayer(GAME_PARAM *gp,u8 player_no,u8 type);

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
GLOBAL void	YT_PlayerAnimeNetSet(GAME_PARAM *gp,PLAYER_PARAM *pp,int player_no,int anm_no,int line_no,int rot,int actno);

#endif	//__PLAYER_H__

