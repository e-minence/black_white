//=============================================================================
/**
 * @file  comm_command_oekaki.h
 * @brief ���G�����{�[�h�ʐM�R�}���h��`�w�b�_
 *          
 * @author  Akito Mori
 * @date    2010.01.20
 */
//=============================================================================
#ifndef __COMM_COMMAND_OEKAKI_H__
#define __COMM_COMMAND_OEKAKI_H__

#include <gflib.h>

//#include "communication/comm_command.h"
//#include "../../field/comm_command_field.h"
#include "net/network_define.h"

// �T�l�ڑ��ʐM�̎��Ƀ��[�U�[���q�@�Ƃ��đ��M�ł���ő��M�o�C�g��
#define COMM_SEND_5TH_PACKET_MAX  (6)

// ---------------------------------------------------
// �ʐM�R�}���h��`
// ---------------------------------------------------
enum{
  CO_OEKAKI_GRAPHICDATA=GFL_NET_CMD_PICTURE,
  CO_OEKAKI_LINEPOS,   
  CO_OEKAKI_LINEPOS_SERVER,   
  CO_OEKAKI_STOP,      
  CO_OEKAKI_RESTART,   
  CO_OEKAKI_END_CHILD, 
  CO_OEKAKI_END,     
};
extern void OEKAKIBOARD_CommandInitialize( void* pWk );



#endif
