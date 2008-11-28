//=============================================================================================
/**
 * @file	btl_net.c
 * @brief	�|�P����WB �o�g��  �ʐM����
 * @author	taya
 *
 * @date	2008.10.10	�쐬
 */
//=============================================================================================

#include "btl_common.h"

#include "btl_net.h"


typedef struct {

	GFL_NETHANDLE* netHandle;
	HEAPID	heapID;

}SYSWORK;

static SYSWORK Sys;


void BTL_NET_InitSystem( GFL_NETHANDLE* netHandle, HEAPID heapID )
{
	Sys.netHandle = netHandle;
	Sys.heapID = heapID;
}

void BTL_NET_QuitSystem( void )
{
	
}


void BTL_NET_StartCommand( BtlNetCommand cmd )
{
	// @@@�͂�ڂ�
}
BOOL BTL_NET_WaitCommand( void )
{
	// @@@�͂�ڂ�
	return TRUE;
}


