//==============================================================================
/**
 * @file	compati_irc.c
 * @brief	�ԊO���Ōq�����Ă��邩
 * @author	matsuda
 * @date	2009.02.13(��)
 */
//==============================================================================
#include <gflib.h>
#include "system\main.h"
#include "net\network_define.h"
#include "system/gfl_use.h"

#include "compati_types.h"
#include "compati_tool.h"
#include "compati_comm.h"
#include "compati_irc.h"



//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   ircsys		
 */
//--------------------------------------------------------------
void CompatiIrc_Init(COMPATI_IRC_SYS *ircsys, COMPATI_CONNECT_SYS *commsys)
{
	;
}

//--------------------------------------------------------------
/**
 * @brief   �����`�F�b�N�Q�[�����̐ԊO���ڑ��m�F
 *
 * @param   ircsys		
 *
 * @retval  COMPATIIRC_RESULT_???
 */
//--------------------------------------------------------------
int CompatiIrc_Main(COMPATI_IRC_SYS *ircsys, COMPATI_CONNECT_SYS *commsys)
{
	if(ircsys->shutdown_req == COMPATIIRC_RESULT_CONNECT && ircsys->seq < 100){
		if(GFL_NET_GetConnectNum() > 1){
			ircsys->seq = 100;
		}
		else{
			ircsys->seq = 101;
		}
	}
	
	switch(ircsys->seq){
	case 0:
		GFL_STD_MemClear(commsys, sizeof(COMPATI_CONNECT_SYS));
		ircsys->seq++;
		//break;
	case 1:
		if(CompatiComm_Init(commsys, 30) == TRUE){
			ircsys->seq++;
		}
		break;
	case 2:
		if(CompatiComm_Connect(commsys) == TRUE){
			OS_TPrintf("�ԊO���F�q������\n");
			ircsys->seq++;
		}
		break;
	case 3:
		if(GFL_NET_GetConnectNum() > 1){
			return COMPATIIRC_RESULT_CONNECT;
		}
		else{
			ircsys->seq++;
			OS_TPrintf("�ԊO���ؒf\n");
		}
		break;
	case 4:
		if(CompatiComm_Exit(commsys) == TRUE){
			ircsys->seq = 0;
		}
		break;
	
	case 100:
		if(CompatiComm_Shoutdown(commsys) == TRUE){
			ircsys->seq++;
			return COMPATIIRC_RESULT_FALSE;
		}
		return COMPATIIRC_RESULT_CONNECT;
	case 101:
		if(CompatiComm_Exit(commsys) == TRUE){
			ircsys->seq = 0;
			return COMPATIIRC_RESULT_EXIT;
		}
		return COMPATIIRC_RESULT_FALSE;
	}
	
	return COMPATIIRC_RESULT_FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO���ؒf���N�G�X�g
 *
 * @param   ircsys		
 */
//--------------------------------------------------------------
void CompatiIrc_Shoutdown(COMPATI_IRC_SYS *ircsys, COMPATI_CONNECT_SYS *commsys)
{
	ircsys->shutdown_req = TRUE;
}

