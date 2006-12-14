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
// �R�[���o�b�N�֐��̏���
typedef void (*PTRCommRecvFunc)(int netID, int size, void* pData, void* pWork);
// �T�C�Y���Œ�̏ꍇ�T�C�Y���֐��ŕԂ�
typedef int (*PTRCommRecvSizeFunc)(void);
// ��M�o�b�t�@�������Ă���ꍇ���̃|�C���^
typedef u8* (*PTRCommRecvBuffAddr)(int netID, void* pWork, int size);


typedef struct {
    PTRCommRecvFunc callbackFunc;    ///< �R�}���h���������ɌĂ΂��R�[���o�b�N�֐�
    PTRCommRecvSizeFunc getSizeFunc; ///< �R�}���h�̑��M�f�[�^�T�C�Y���Œ�Ȃ珑���Ă�������
    PTRCommRecvBuffAddr getAddrFunc;
} CommPacketTbl;

#define   _SET(callfunc, getSize, name)       {callfunc,  getSize},

/// �ėp�ʐM�R�}���h�̒�`
enum CommCommand_e {
  GFL_NET_CMD_NONE = 0,                ///< �Ȃɂ����Ȃ�
  GFL_NET_CMD_FREE = 0,                   ///< ��R�}���h
  GFL_NET_CMD_COMMAND_MIN = 1,             ///< �ŏ��l
  GFL_NET_CMD_EXIT = GFL_NET_CMD_COMMAND_MIN,            ///< �I��
  GFL_NET_CMD_AUTO_EXIT,            ///< �����I��
  GFL_NET_CMD_COMM_INFO,       ///< info���
  GFL_NET_CMD_COMM_INFO_ARRAY,  ///< info���𓊂��Ԃ�
  GFL_NET_CMD_COMM_INFO_END,   ///< info��񑗐M�I��
  GFL_NET_CMD_COMM_NEGOTIATION,  ///< ���������̃l�S�V�G�[�V����
  GFL_NET_CMD_COMM_NEGOTIATION_RETURN,
  GFL_NET_CMD_DSMP_CHANGE,            ///< DS���[�h�ʐM��MP���[�h�ʐM���ɐ؂�ւ��鋖�𓾂�
  GFL_NET_CMD_DSMP_CHANGE_REQ,        ///< DS���[�h�ʐM��MP���[�h�ʐM���ɐ؂�ւ���w�����o��
  GFL_NET_CMD_DSMP_CHANGE_END,        ///< DS���[�h�ʐM��MP���[�h�ʐM���ɐ؂�ւ������������Ƃ�ʒm
  GFL_NET_CMD_THROWOUT,       ///< �R�}���h��j�����鋖�𓾂�
  GFL_NET_CMD_THROWOUT_REQ,   ///< �R�}���h��j������w�����o��
  GFL_NET_CMD_THROWOUT_END,   ///< �R�}���h�j�������������Ƃ�ʒm-----
  GFL_NET_CMD_TIMING_SYNC,            ///< ���������R�}���h
  GFL_NET_CMD_TIMING_SYNC_END,        ///< ��������ꂽ���Ƃ�Ԃ��R�}���h
  GFL_NET_CMD_TIMING_SYNC_INFO,       ///< �����̏󋵂��q�@�ɕԂ��R�}���h
  //------------------------------------------------�����܂�----------
  GFL_NET_CMD_COMMAND_MAX   // �I�[--------------����͈ړ������Ȃ��ł�������     21
};

#define COMM_VARIABLE_SIZE (0xffff)   ///< �σf�[�^���M�ł��邱�Ƃ������Ă���


/// �R�}���h�e�[�u���̏�����������
extern void GFL_NET_CommandInitialize(const CommPacketTbl* pCommPacketLocal,int listNum,void* pWork);
/// �R�}���h�e�[�u���̊J������
extern void GFL_NET_CommandFinalize( void );
/// �f�[�^�̃T�C�Y�𓾂�
extern int GFL_NET_CommandGetPacketSize(int command);
///  �R�}���h�e�[�u����j�����鎞�̖���
extern BOOL GFL_NET_CommandThrowOut(void);
///  �R�}���h�e�[�u�����j�����I��������ǂ�������
extern BOOL GFL_NET_CommandIsThrowOuted(void);
///  �R�[���o�b�N���Ă�
extern void GFL_NET_CommandCallBack(int netID, int command, int size, void* pData);
/// �v�����g
extern void GFL_NET_CommandDebugPrint(int command);

// �T�C�Y�w��p�ȈՊ֐�  �e�ʍ팸�̈�extern�錾  06.03.29
extern int _getVariable(void);
extern int _getZero(void);
extern int _getOne(void);

extern void* GFL_NET_CommandCreateBuffStart(int command, int netID, int size);
extern BOOL GFL_NET_CommandCreateBuffCheck(int command);


#endif// __NET_COMMAND_H__

