
//============================================================================================
/**
 * @file	game.h
 * @brief	DS�Ń��b�V�[�̂��܂��Q�[���w�b�_�[
 * @date	2007.02.06
 */
//============================================================================================

#ifndef	__GAME_H__
#define	__GAME_H__

#undef GLOBAL
#ifdef __GAME_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

GLOBAL	void	YT_InitGame(GAME_PARAM *gp);
GLOBAL	void	YT_MainGame(GAME_PARAM *gp);

#endif	__GAME_H__
