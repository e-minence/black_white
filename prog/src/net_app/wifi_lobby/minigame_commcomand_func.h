//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		minigame_commcomand_func.h
 *	@brief		�~�j�Q�[���c�[���ʐM�R�}���h
 *	@author		tomoya takahashi
 *	@data		2007.11.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __MINIGAME_COMMCOMAND_FUNC_H__
#define __MINIGAME_COMMCOMAND_FUNC_H__

#include "minigame_tool_local.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// �ʐM�������֐�
extern void CommCommandMNGMInitialize( MNGM_RESULTWK* p_wk );
extern void CommCommandMNGMEntryInitialize( MNGM_ENTRYWK* p_wk );

// ���ʃT�C�Y�擾�֐�
extern int CommMNGMGetZeroSize( void );
extern int CommMNGMGetu32Size( void );

// �e�[���q�@Rare�Q�[���^�C�v
extern void CommMNGMRareGame( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle );


// �q�[>�e	����������
extern void CommMNGMRetryYes( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle );
// �q�[>�e	�������Ȃ�
extern void CommMNGMRetryNo( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle );

// �e�[>�q	����������
extern void CommMNGMRetryOk( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle );
// �e�[>�q	�������Ȃ�
extern void CommMNGMRetryNg( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle );

#endif		// __MINIGAME_COMMCOMAND_FUNC_H__

