//=============================================================================
/**
 * @file	net_local.h
 * @brief	�ʐM���C�u�����̓������J�֐�
 * @author	k.ohno
 * @date    2008.3.4
 */
//=============================================================================
#ifndef __NET_LOCAL_H__
#define __NET_LOCAL_H__

#include "net_def.h"
#include "net_handle.h"

///< @brief �ʐM�Ǘ��\����
typedef struct _GFL_NETSYS GFL_NETSYS;

///< @brief  �ʐM�V�X�e���Ǘ��\����
struct _GFL_NETSYS{
    GFLNetInitializeStruct aNetInit;              ///< �������\���̂̃R�s�[
    GFL_NETHANDLE aNetHandle[GFL_NET_HANDLE_MAX]; ///< �ʐM�n���h��
    UI_KEYSYS* pKey[GFL_NET_HANDLE_MAX];  ///< �L�[�o�b�t�@
    GFL_NETWL* pNetWL;                    ///<  ���C�����X�}�l�[�W���[�|�C���^
    NetModeChangeFunc pNetModeChangeFunc;
    NetEndCallback pNetEndCallback;     ///< �l�b�g���[�N�I���R�[���o�b�N
};


//==============================================================================
/**
 * @brief       GFL_NETSYS�\���̂̃|�C���^�𓾂�
 * @param       none
 * @return      GFL_NETSYS
 */
//==============================================================================
extern GFL_NETSYS* _GFL_NET_GetNETSYS(void);

//==============================================================================
/**
 * @brief       netHandle��S�ď���
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @return      none
 */
//==============================================================================
extern void GFL_NET_HANDLE_DeleteAll(GFL_NETSYS* pNet);

#endif //__NET_LOCAL_H__

