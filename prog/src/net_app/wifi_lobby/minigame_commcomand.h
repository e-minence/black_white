//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		minigame_commcomand.h
 *	@brief		�~�j�Q�[���c�[���ʐM�R�}���h
 *	@author		tomoya takahashi
 *	@data		2007.11.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __MINIGAME_COMMCOMAND_H__
#define __MINIGAME_COMMCOMAND_H__

#include "net\network_define.h"

enum CommCommandMNGM {

	//-------------------------------------
	//�@�Q�[���p
	//=====================================
	CNM_MNGM_RETRY_YES = GFL_NET_CMD_MINIGAME_TOOL,	//< �q�[���e	RETRY YES
	CNM_MNGM_RETRY_NO,						//<	�q�[���e	RETRY�@NO
	CNM_MNGM_RETRY_OK,						//< �e�[���q	RETRY
	CNM_MNGM_RETRY_NG,						//< 
	CNM_MNGM_ENTRY_RAREGAME,				//<	�e�[���q	Rare�Q�[���^�C�v

};

extern const NetRecvFuncTable* MNGM_CommCommandTclGet( void );
extern int MNGM_CommCommandTblNumGet( void );

#endif		// __MINIGAME_COMMCOMAND_H__

