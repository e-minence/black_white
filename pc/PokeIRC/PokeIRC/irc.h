#ifndef __IRC_H__
#define __IRC_H__

/***********************************************************************
***********************************************************************/
#define DHC_DEBUG	(1) // ��f�o�b�O���͒�`�������Ă����Ă�������
/***********************************************************************
***********************************************************************/
#define IRC_TRNS_MAX		(128)	// ��x�ɑ����]���o�C�g��

#define IRC_UNIT_DHC		(0x00)	// DHC
#define IRC_UNIT_IRC		(0x01)	// �ԊO���ʐM�J�[�h
#define IRC_UNIT_UNKNOWN	(0xFF)	// �@��s��

/***********************************************************************
�R�[���o�b�N�֐��̌^��`
***********************************************************************/
typedef void (*IRCRecvCallback)(unsigned char *data, unsigned char size, unsigned char command, unsigned char value);
/***********************************************************************
***********************************************************************/
extern void IRC_Init(void);		// �������֐�
extern void IRC_Move(void);		// ���t���[���Ăԃv���Z�X�֐�

extern void IRC_Send(unsigned char *buf, unsigned char size, unsigned char command, unsigned char value);	// �f�[�^���M
extern void IRC_SetRecvCallback(IRCRecvCallback cb); // �f�[�^��M�R�[���o�b�N�̐ݒ�

extern void IRC_Connect(void);	// �ڑ��v���B�������������(���ۂɂ͂��̗v�����󂯂����̋t)���Z���_�ɂȂ�܂��B
extern BOOL IRC_IsSender(void); // �Z���_���ǂ����̊m�F(�ڑ����̂ݗL��)

extern BOOL IRC_IsConnect(void); // �ڑ��̊m�F
extern void IRC_Shutdown(void); // �ؒf
/***********************************************************************
����ɂ͕K�{�łȂ�����
***********************************************************************/
extern void IRC_SetUnitNumber(unsigned char id); // ���g�̃��j�b�g�ԍ���ݒ肷��
extern unsigned char IRC_GetUnitNumber(void); // �ʐM����̃��j�b�g�ԍ��̊m�F(�ؒf����0xFF)

extern unsigned int IRC_GetRetryCount(void); // �g�[�^���̃��g���C�񐔂��擾����(�f�o�b�O�p)
/***********************************************************************
�ڑ��^�C���A�E�g���Ԃ̐ݒ�(�~���b�P��)
0���w�肷��ƃ^�C���A�E�g���Ȃ��Ȃ�܂�
IRC_Init ��100�ɐݒ肳��Ă��܂�
***********************************************************************/
extern void IRC_SetDisconnectTime(unsigned int time);
/***********************************************************************
USB �̃`�F�b�N(PC��p)
***********************************************************************/
#ifndef NITRO
extern BOOL IRC_IsConnectUsb(void);
extern TCHAR *IRC_GetPortName(void);
#endif

#endif
