//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		comm_command_wfp2pmf_func.c
 *	@brief		�Q�`�S�l��p�ҍ���	�ʐM�R�}���h
 *	@author		tomoya takahashi
 *	@data		2007.05.24
 *
 *	���̃\�[�X��
 *	wifi_p2pmatch.c	wifi_p2pmatchfour.c	�ǂ��炩����Ă΂��̂ŏ풓�ɂ����܂��B
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "wifi_p2pmatch_local.h"
#include "comm_command_wfp2pmf.h"
#include "comm_command_wfp2pmf_func.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�R�}���h�ݒ�	wifi_p2pmatch.c���Ńe�[�u���ݒ�p
 */
//-----------------------------------------------------------------------------
void CommCommandWFP2PMF_MatchStartInitialize( WIFIP2PMATCH_WORK *wk )
{
	GFL_NET_AddCommandTable(GFL_NET_CMD_WIFICLUB, WFP2PMF_CommCommandTclGet(), 
			WFP2PMF_CommCommandTblNumGet(), wk );
}

int CommWFP2PMFGetZeroSize( void )
{
	return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ʃf�[�^�T�C�Y
 *	@return	�f�[�^�T�C�Y
 */
//-----------------------------------------------------------------------------
int CommWFP2PMFGetWFP2PMF_COMM_RESULTSize( void )
{
	return sizeof(WFP2PMF_COMM_RESULT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	VCHAT�f�[�^�T�C�Y
 *	@return	�f�[�^�T�C�Y
 */
//-----------------------------------------------------------------------------
int CommWFP2PMFGetWFP2PMF_COMM_VCHATSize( void )
{
	return sizeof(WFP2PMF_COMM_VCHAT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e����̒ʐMOK�ʒm
 *
 *	@param	netID		�l�b�gID
 *	@param	size		�T�C�Y
 *	@param	pBuff		�o�b�t�@
 *	@param	pWork		���[�N
 */
//-----------------------------------------------------------------------------
void CommWFP2PMFGameResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	WFP2P_WK* p_wk = pWork;
	const WFP2PMF_COMM_RESULT* p_data = pData;

	if( p_wk == NULL ){
		return ;
	}
	
	WFP2PMF_CommResultRecv( p_wk, p_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e����̃Q�[���J�n��ʒm
 *
 *	@param	netID		�l�b�gID
 *	@param	size		�T�C�Y
 *	@param	pBuff		�o�b�t�@
 *	@param	pWork		���[�N
 */
//-----------------------------------------------------------------------------
void CommWFP2PMFGameStart(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	WFP2P_WK* p_wk = pWork;

	if( p_wk == NULL ){
		return ;
	}
	
	// �Q�[���J�n
	WFP2PMF_CommStartRecv( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e����VCHAT�t���O�擾
 *
 *	@param	netID		�l�b�gID
 *	@param	size		�T�C�Y
 *	@param	pBuff		�o�b�t�@
 *	@param	pWork		���[�N
 */
//-----------------------------------------------------------------------------
void CommWFP2PMFGameVchat(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	WFP2P_WK* p_wk = pWork;

	if( p_wk == NULL ){
		return ;
	}
	
	// VCHAT�t���O�擾
	WFP2PMF_CommVchatRecv( p_wk, pData );
}
