//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		comm_command_wfp2pmf_func.h
 *	@brief		�Q�`�S�l��p�ҍ���	�ʐM�R�}���h
 *	@author		tomoya takahashi
 *	@data		2007.05.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once


#include "net_app/wificlub/wifi_p2pmatch.h"
#include "wifi_p2pmatch_local.h"

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
//extern void CommCommandWFP2PMFInitialize( WFP2P_WK* p_wk );
extern void CommCommandWFP2PMF_MatchStartInitialize( WIFIP2PMATCH_WORK *wk );
//extern void CommCommandWFP2PMF_MatchStartInitialize( void );	// wifi_p2pmatch.c���ŃR�}���h��ݒ肷��Ƃ��̊֐�

// ���ʃT�C�Y�擾�֐�
extern int CommWFP2PMFGetZeroSize( void );
extern int CommWFP2PMFGetWFP2PMF_COMM_RESULTSize( void );
extern int CommWFP2PMFGetWFP2PMF_COMM_VCHATSize( void );

// �e����̃��N�G�X�g
extern void CommWFP2PMFGameResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void CommWFP2PMFGameStart(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void CommWFP2PMFGameVchat(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);



