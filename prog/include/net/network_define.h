






//=============================================================================
/**
 * @file	network_define.h
 * @brief	WB�̃l�b�g���[�N��`�������w�b�_�[�t�@�C��
 * @author	Katsumi Ohno
 * @date    2008.07.29
 */
//=============================================================================

#ifndef __NETWORK_DEFINE_H__
#define __NETWORK_DEFINE_H__

#define NET_WORK_ON (1)   // �T���v���v���O������ŒʐM�𓮂����ꍇ��`�@�������Ȃ��ꍇ����`�ɂ���


//==============================================================================
//	��`
//==============================================================================

extern void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo,void* pWork);
extern u32 GFLR_NET_GetGGID(void);

//==============================================================================
//	�^�錾
//==============================================================================



/// �ėp�ʐM�R�}���h�̒�`
enum NetworkServiceID_e {
  WB_NET_FIELDMOVE_SERVICEID = 1,  ///< �T���v���̃t�B�[���h�ړ�
  WB_NET_DEBUG_OHNO_SERVICEID,
  WB_NET_SERVICEID_DEBUG_TAYA,
  WB_NET_DEBUG_MATSUDA_SERVICEID,
  WB_NET_SERVICEID_MAX   // �I�[
};


#endif //__NETWORK_DEFINE_H__

