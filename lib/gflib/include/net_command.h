#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
/**
 * @file	net_command.h
 * @brief	�f�[�^���L���s���ꍇ�̒ʐM�V�X�e��
 *          �g�p����ꍇ�ɏ������ޕK�v��������̂��܂Ƃ߂��w�b�_�[
 * @author	Katsumi Ohno
 * @date    2005.07.26
 */
//=============================================================================

#pragma once


//==============================================================================
//	��`
//==============================================================================

//==============================================================================
//	�^�錾
//==============================================================================

//#define   _SET(callfunc, getSize, name)       {callfunc,  getSize},

///-------------------------------------------------------------------------
///-------------------------------------------------------------------------

/// �ėp�ʐM�R�}���h�̒�`
enum GFL_NET_CommCommand_e {
  GFL_NET_CMD_NONE = 0,                ///< �Ȃɂ����Ȃ�
  GFL_NET_CMD_FREE = GFL_NET_CMD_NONE,                   ///< ��R�}���h
  GFL_NET_CMD_COMMAND_MIN = 1,             ///< �ŏ��l
  GFL_NET_CMD_EXIT_REQ = GFL_NET_CMD_COMMAND_MIN,            ///< �I��
  GFL_NET_CMD_EXIT,            ///< �����I��
  GFL_NET_CMD_NEGOTIATION,  ///< ���������̃l�S�V�G�[�V����
  GFL_NET_CMD_NEGOTIATION_RETURN,  ///< �l�S�V�G�[�V�����̕Ԏ�
  GFL_NET_CMD_NEGOTIATION_RESET,  ///< �l�S�V�G�[�V����������
  GFL_NET_CMD_DSMP_CHANGE,            ///< DS���[�h�ʐM��MP���[�h�ʐM���ɐ؂�ւ��鋖�𓾂�
  GFL_NET_CMD_TIMING_SYNC,            ///< ���������R�}���h  8
  GFL_NET_CMD_TIMING_SYNC_END,        ///< ��������ꂽ���Ƃ�Ԃ��R�}���h
  GFL_NET_CMD_SEND_MACADDRESS,      ///< �����Ă�}�b�N�A�h���X�𑗂�
  GFL_NET_CMD_INFOMATION,           ///< �C���t�H���[�V�����R�}���h�𑗂�
  GFL_NET_CMD_WIFI_EXIT,              ///< �q�@���e�@�ɏI������悤�ɑ��M
  GFL_NET_CMD_HUGEDATA,            ///< �傫�ȃf�[�^�̕����]�����[�`��
  GFL_NET_CMD_HUGEPREDATA,         ///< �傫�ȃf�[�^�𑗂�\��R�}���h
  GFL_NET_CMD_LOWER_DATA,          ///< ��D��x�f�[�^���M
  GFL_NET_CMD_LOWER_DATA_FLG,      ///< ��D��x�f�[�^���M�̃t���O
  //------------------------------------------------�����܂�----------
  GFL_NET_CMD_COMMAND_MAX   // �I�[--------------����͈ړ������Ȃ��ł�������
};


//

//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̏�����  �R�}���h�e�[�u���̕ύX
 * @param   cokind               �R�}���h�̌n�ԍ� NetStartCommand_e
 * @param   pCommPacketLocal �Ăяo�����W���[����p�̃R�}���h�̌n�e�[�u��
 * @param   listNum          �R�}���h��
 * @param   pWork            �Ăяo�����W���[����p�̃��[�N�G���A
 * @retval  none
 */
//--------------------------------------------------------------
extern void GFL_NET_COMMAND_Init(const int cokind, const NetRecvFuncTable* pCommPacketLocal,int listNum,void* pWork, HEAPID heapID);

//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̒ǉ�
 * @param   cokind          �R�}���h�̌n�ԍ� NetStartCommand_e
 * @param   pCommPacketLocal �Ăяo�����W���[����p�̃R�}���h�̌n�e�[�u��
 * @param   listNum          �R�}���h��
 * @param   pWork            �Ăяo�����W���[����p�̃��[�N�G���A
 * @retval  none
 */
//--------------------------------------------------------------
extern void GFL_NET_AddCommandTable(const int cokind,const NetRecvFuncTable* pCommPacketLocal,int listNum,void* pWork);

//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̍폜
 * @param   cokind          �R�}���h�̌n�ԍ� NetStartCommand_e
 * @retval  none
 */
//--------------------------------------------------------------
extern void GFL_NET_DelCommandTable(const int cokind);

//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̊J������
 *          �Q�[�����ɊJ������ꍇ�́A�K�����������킹�Ă���J�����Ă�������
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
extern void GFL_NET_COMMAND_Exit( void );

//------------------------------------------------------------------------------
/**
 * @brief ���M�J�n
 * @param[in,out]  pNet  �ʐM�n���h��
 * @param[in]   sendBit                     ���M���� �S���֑��M����ꍇ NET_SENDID_ALLUSER
 * @param[in]   sendCommand                ���M����R�}���h
 * @param[in]   size                       ���M�f�[�^�T�C�Y
 * @param[in]   data                       ���M�f�[�^�|�C���^
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//------------------------------------------------------------------------------
extern BOOL GFL_NET_COMMAND_SendHugeData(GFL_NETHANDLE* pNet,const NetID sendBit,const u16 sendCommand, const u32 size,const void* data);


// �������牺�̓��C�u������p GFI�֐�

//--------------------------------------------------------------
/**
 * @brief   �e�[�u���ɏ]�� ��M�R�[���o�b�N���Ăяo���܂�
 * @param   netID           �l�b�g���[�NID
 * @param   sendID          ���MID
 * @param   command         ��M�R�}���h
 * @param   size            ��M�f�[�^�T�C�Y
 * @param   pData           ��M�f�[�^
 * @retval  none
 */
//--------------------------------------------------------------
extern void GFI_NET_COMMAND_CallBack(int netID, int sendID, int command, int size, void* pData,GFL_NETHANDLE* pNetHandle);

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@�������Ă��邩�ǂ����̌���
 * @param   command         �R�}���h
 * @retval  �����Ă�Ȃ�TRUE
 */
//--------------------------------------------------------------
extern BOOL GFI_NET_COMMAND_CreateBuffCheck(int command);


//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@�𓾂�
 * @param   command         �R�}���h
 * @param   netID           ���M��
 * @param   size         �T�C�Y
 * @retval  ��M�o�b�t�@ �����Ă��Ȃ��Ȃ��NULL
 */
//--------------------------------------------------------------

extern void* GFI_NET_COMMAND_CreateBuffStart(int command, int netID, int size);

//--------------------------------------------------------------
/**
 * @brief   �R�}���h���󂯎��l���ǂ���
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

extern BOOL GFI_NET_COMMAND_RecvCheck(int recvID);



#ifdef __cplusplus
} /* extern "C" */
#endif
