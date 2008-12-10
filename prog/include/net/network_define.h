
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

/// �Q�[���̎�ނ���ʂ���ׂ̒�`   �������\���̂�gsid�Ƃ��ēn���Ă�������
enum NetworkServiceID_e {
  WB_NET_NOP_SERVICEID = 0,
  WB_NET_BATTLE_SERVICEID,       ///< �o�g���pID
  WB_NET_FIELDMOVE_SERVICEID,  ///< �T���v���̃t�B�[���h�ړ�
  WB_NET_BOX_DOWNLOAD_SERVICEID,  ///< �_�E�����[�h�ʐM�̃{�b�N�X�擾
  WB_NET_DEBUG_OHNO_SERVICEID,
  WB_NET_SERVICEID_DEBUG_TAYA,
  WB_NET_DEBUG_MATSUDA_SERVICEID,
  WB_NET_TRADE_SERVICEID,			///<�|�P��������
  WB_NET_SERVICEID_MAX   // �I�[
};

///�ʐM�R�}���h����ʂ���ׂ̒�`  �R�[���o�b�N�e�[�u����n���Ƃ���ID�Ƃ��Ē�`���Ă�������
/// ��̒�`�Ɠ������тŏ����Ă�������
enum NetworkCommandHeaderNo_e {
  GFL_NET_CMD_BASE = (WB_NET_NOP_SERVICEID<<8),  ///< �x�[�X�R�}���h�J�n�ԍ�
  GFL_NET_CMD_BATTLE = (WB_NET_BATTLE_SERVICEID<<8),   ///< �o�g���J�n�ԍ�
  GFL_NET_CMD_FIELD = (WB_NET_FIELDMOVE_SERVICEID<<8),   ///< �t�B�[���h�̃x�[�X�J�n�ԍ�
  GFL_NET_CMD_BOX_DOWNLOAD = (WB_NET_BOX_DOWNLOAD_SERVICEID<<8),   ///< �t�B�[���h�̃x�[�X�J�n�ԍ�
  GFL_NET_CMD_DEBUG_OHNO = (WB_NET_DEBUG_OHNO_SERVICEID<<8),
  GFL_NET_CMD_DEBUG_TAYA = (WB_NET_SERVICEID_DEBUG_TAYA<<8),
  GFL_NET_CMD_DEBUG_MATSUDA = (WB_NET_DEBUG_MATSUDA_SERVICEID<<8),
  GFL_NET_CMD_TRADE = (WB_NET_TRADE_SERVICEID<<8),		///<�|�P���������̊J�n�ԍ�
};



#endif //__NETWORK_DEFINE_H__

