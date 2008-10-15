//======================================================================
/**
 *
 * @file	ari_comm_def.c	
 * @brief	�ʐM�p��`�S
 * @author	ariizumi
 * @data	08.10.14
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "net/network_define.h"
#include "textprint.h"
#include "arc_def.h"

#include "ari_comm_func.h"

#ifndef ARI_COMM_DEF_H__
#define ARI_COMM_DEF_H__

//======================================================================
//	define
//======================================================================
#define FIELD_COMM_NAME_LENGTH (6)	    //�I�[��������
#define FC_ID			u16
#define FIELD_COMM_ID_MAX	( 0xFFFE )  //ID�̍ő�l
#define FIELD_COMM_ID_INVALID	( 0xFFFF )  //ID�̖����l
#define FIELD_COMM_SEARCH_PARENT_NUM (4)    //�q�@���e�@��T���ĕ\�����鐔

#define IS_PARENT		(GFL_NET_IsParentMachine())
#define IS_CHILD		(!GFL_NET_IsParentMachine())	

//����M�֐��p
enum FIELD_COMM_COMMAND_TYPE
{
    FC_CMD_FIRST_BEACON,    //�ڑ�����Ɏq���e�֑���r�[�R��
    FC_CMD_START_MODE,	    //�t���[�ړ��J�n
    FC_CMD_PLAYER_DATA,	    //�v���C���[���
};

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	FIELD_COMM_BEACON
//	�r�[�R���f�[�^
//======================================================================
typedef struct
{
    STRCODE name[FIELD_COMM_NAME_LENGTH];
    u16	    id;
}FIELD_COMM_BEACON;


#endif //ARI_COMM_DEF_H__
