//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   minigame_commcomand_func.c
 *  @brief    �~�j�Q�[���c�[���ʐM�R�}���h
 *  @author   tomoya takahashi
 *  @data   2007.11.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "minigame_commcomand.h"
#include "minigame_commcomand_func.h"

#include "net_app/net_bugfix.h"

//-----------------------------------------------------------------------------
/**
 *          �R�[�f�B���O�K��
 *    ���֐���
 *        �P�����ڂ͑啶������ȍ~�͏������ɂ���
 *    ���ϐ���
 *        �E�ϐ�����
 *            const�ɂ� c_ ��t����
 *            static�ɂ� s_ ��t����
 *            �|�C���^�ɂ� p_ ��t����
 *            �S�č��킳��� csp_ �ƂȂ�
 *        �E�O���[�o���ϐ�
 *            �P�����ڂ͑啶��
 *        �E�֐����ϐ�
 *            �������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *          �萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          �\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          �v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


// �ʐM�������֐�
void CommCommandMNGMInitialize( MNGM_RESULTWK* p_wk )
{
#if WB_FIX
  CommCommandInitialize( MNGM_CommCommandTclGet(), 
      MNGM_CommCommandTblNumGet(), p_wk );
#else
	GFL_NET_AddCommandTable(GFL_NET_CMD_MINIGAME_TOOL, 
	  MNGM_CommCommandTclGet(), MNGM_CommCommandTblNumGet(), p_wk);
#endif
}
void CommCommandMNGMEntryInitialize( MNGM_ENTRYWK* p_wk )
{
#if WB_FIX
  CommCommandInitialize( MNGM_CommCommandTclGet(), 
      MNGM_CommCommandTblNumGet(), p_wk );
#else
	GFL_NET_AddCommandTable(GFL_NET_CMD_MINIGAME_TOOL, 
	  MNGM_CommCommandTclGet(), MNGM_CommCommandTblNumGet(), p_wk);
#endif
}

// �e�[���q�@Rare�Q�[���^�C�v
void CommMNGMRareGame( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle )
{
  MNGM_ENTRY_SetRareGame( pWork, *((u32*)pBuff) );
}


// �q�[>�e  ����������
void CommMNGMRetryYes( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle )
{
  MNGM_RESULT_SetKoRetry( pWork, netID, TRUE );
}

// �q�[>�e  �������Ȃ�
void CommMNGMRetryNo( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle )
{
  MNGM_RESULT_SetKoRetry( pWork, netID, FALSE );
}


// �e�[>�q  ����������
void CommMNGMRetryOk( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle )
{
  MNGM_RESULT_SetOyaRetry( pWork, TRUE );
}

// �e�[>�q  �������Ȃ�
void CommMNGMRetryNg( const int netID, const int size, const void* pBuff, void* pWork, GFL_NETHANDLE *pNetHandle )
{
  MNGM_RESULT_SetOyaRetry( pWork, FALSE );
}

