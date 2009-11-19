//============================================================================================
/**
 * @file	global_msg.c
 * @brief	�O���[�o�����b�Z�[�W�f�[�^
 * @author	taya
 * @date	2008.11.19
 */
//============================================================================================
#include <assert.h>
#include <heapsys.h>

#include "print/global_msg.h"

#include "arc_def.h"
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
#include "message.naix"
#else                    //DL�q�@������
#include "message_dl.naix"
#endif //MULTI_BOOT_MAKE

//--------------------------------------------------------------
/**
 *	�O���[�o�����b�Z�[�W�f�[�^
 */
//--------------------------------------------------------------
const GFL_MSGDATA* GlobalMsg_PokeName = NULL;


//=============================================================================================
/**
 * �O���[�o�����b�Z�[�W�f�[�^�n���h���𐶐��i�v���O�����N����ɂP�x�����Ăԁj
 *
 * @param   heapID		�����p�q�[�vID
 */
//=============================================================================================
void GLOBALMSG_Init( HEAPID heapID )
{
	GF_ASSERT(GlobalMsg_PokeName == NULL);

//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	GlobalMsg_PokeName = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_monsname_dat, heapID );
#else                    //DL�q�@������
	GlobalMsg_PokeName = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_dl_monsname_dat, heapID );
#endif //MULTI_BOOT_MAKE
}


