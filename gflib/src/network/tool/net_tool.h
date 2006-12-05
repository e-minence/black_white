//=============================================================================
/**
 * @file	net_tool.h
 * @brief	�ʐM�c�[���̊֐�
 * @author  GAME FREAK Inc.
 * @date    2006.12.5
 */
//=============================================================================
#ifndef __NET_TOOL_H__
#define __NET_TOOL_H__

/// @brief  �ʐM�c�[�����[�N
typedef struct _NET_TOOLSYS_t NET_TOOLSYS;


//==============================================================================
/**
 * @brief   �l�b�g���[�N�c�[��������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @param   num       �������l��
 * @return  NET_TOOLSYS  �l�b�g���[�N�c�[���V�X�e�����[�N
 */
//==============================================================================
extern NET_TOOLSYS* GFL_NET_TOOL_sysInit(const int heapID, const int num);
//==============================================================================
/**
 * @brief   �l�b�g���[�N�c�[���I��
 * @param   pCT    �l�b�g���[�N�c�[���V�X�e�����[�N
 * @return  none
 */
//==============================================================================
extern void GFL_NET_TOOL_sysEnd(NET_TOOLSYS* pCT);
//==============================================================================
/**
 * @brief  �^�C�~���O�R�}���h����M����   CS_TIMING_SYNC
 * @param[in]   netID  �ʐM���M��ID
 * @param[in]   size   ��M�f�[�^�T�C�Y
 * @param[in]   pData  ��M�f�[�^�|�C���^
 * @param[in,out]   pWork  �g�p�҂��K�v��work�̃|�C���^
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @retval      none
 */
//==============================================================================
extern void GFL_NET_TOOL_RecvTimingSync(const int netID, const int size, const void* pData,
                                        void* pWork, GFL_NETHANDLE* pNet);
//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���hEND����M����   CS_TIMING_SYNC_END
 * @param[in]   netID  �ʐM���M��ID
 * @param[in]   size   ��M�f�[�^�T�C�Y
 * @param[in]   pData  ��M�f�[�^�|�C���^
 * @param[in,out]   pWork  �g�p�҂��K�v��work�̃|�C���^
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @retval      none
 */
//==============================================================================
extern void CommRecvTimingSyncEnd(const int netID, const int size, const void* pData,
                                  void* pWork, GFL_NETHANDLE* pNet);
//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @param   no   �^�C�~���O��肽���ԍ�
 * @retval  none
 */
//==============================================================================
extern void CommTimingSyncStart(GFL_NETHANDLE* pNet, u8 no);
//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h�𑗐M
 * @param   no   �^�C�~���O��肽���ԍ�
 * @retval  none
 */
//==============================================================================
extern void CommTimingSyncSend(GFL_NETHANDLE* pNet);
//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param   no   �͂��ԍ�
 * @retval  �͂��Ă�����TRUE
 */
//==============================================================================
extern BOOL CommIsTimingSync(GFL_NETHANDLE* pNet, u8 no);
//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param   no   �͂��ԍ�
 * @retval  �͂��Ă�����TRUE
 */
//==============================================================================
extern int CommGetTimingSyncNo(GFL_NETHANDLE* pNet, int netID);


#endif //__NET_TOOL_H__



