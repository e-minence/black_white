//=============================================================================
/**
 * @file	net_command.h
 * @brief	�f�[�^���L���s���ꍇ�̒ʐM�V�X�e��
 *          �g�p����ꍇ�ɏ������ޕK�v��������̂��܂Ƃ߂��w�b�_�[
 * @author	Katsumi Ohno
 * @date    2005.07.26
 */
//=============================================================================

#ifndef __NET_COMMAND_H__
#define __NET_COMMAND_H__


//==============================================================================
//	��`
//==============================================================================

//==============================================================================
//	�^�錾
//==============================================================================

//#define   _SET(callfunc, getSize, name)       {callfunc,  getSize},


/// �ėp�ʐM�R�}���h�̒�`
enum CommCommand_e {
  GFL_NET_CMD_NONE = 0,                ///< �Ȃɂ����Ȃ�
  GFL_NET_CMD_FREE = 0,                   ///< ��R�}���h
  GFL_NET_CMD_COMMAND_MIN = 1,             ///< �ŏ��l
  GFL_NET_CMD_EXIT_REQ = GFL_NET_CMD_COMMAND_MIN,            ///< �I��
  GFL_NET_CMD_EXIT,            ///< �����I��
  GFL_NET_CMD_NEGOTIATION,  ///< ���������̃l�S�V�G�[�V����
  GFL_NET_CMD_NEGOTIATION_RETURN,  ///< �l�S�V�G�[�V�����̕Ԏ�
  GFL_NET_CMD_DSMP_CHANGE,            ///< DS���[�h�ʐM��MP���[�h�ʐM���ɐ؂�ւ��鋖�𓾂�
  GFL_NET_CMD_DSMP_CHANGE_REQ,        ///< DS���[�h�ʐM��MP���[�h�ʐM���ɐ؂�ւ���w�����o��
  GFL_NET_CMD_DSMP_CHANGE_END,        ///< DS���[�h�ʐM��MP���[�h�ʐM���ɐ؂�ւ������������Ƃ�ʒm
  GFL_NET_CMD_TIMING_SYNC,            ///< ���������R�}���h
  GFL_NET_CMD_TIMING_SYNC_END,        ///< ��������ꂽ���Ƃ�Ԃ��R�}���h
  //------------------------------------------------�����܂�----------
  GFL_NET_CMD_COMMAND_MAX   // �I�[--------------����͈ړ������Ȃ��ł�������
};



/// �R�}���h�e�[�u���̏�����������
extern void GFL_NET_CommandInitialize(const NetRecvFuncTable* pCommPacketLocal,int listNum,void* pWork);
/// �R�}���h�e�[�u���̊J������
extern void GFL_NET_CommandFinalize( void );
/// �f�[�^�̃T�C�Y�𓾂�
extern int GFL_NET_CommandGetPacketSize(int command);
///  �R�}���h�e�[�u����j�����鎞�̖���
extern BOOL GFL_NET_CommandThrowOut(void);
///  �R�}���h�e�[�u�����j�����I��������ǂ�������
extern BOOL GFL_NET_CommandIsThrowOuted(void);
///  �R�[���o�b�N���Ă�
extern void GFL_NET_CommandCallBack(int netID, int sendID, int command, int size, void* pData);
/// �v�����g
extern void GFL_NET_CommandDebugPrint(int command);

extern void* GFL_NET_CommandCreateBuffStart(int command, int netID, int size);
extern BOOL GFL_NET_CommandCreateBuffCheck(int command);


#endif// __NET_COMMAND_H__

