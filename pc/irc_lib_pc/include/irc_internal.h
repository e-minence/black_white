#ifndef __IRC_INTERNAL_H__
#define __IRC_INTERNAL_H__

#define USE_DEBUGPRINT	(0)

#if USE_DEBUGPRINT==1
#define PRINTF printf
#else
#define PRINTF TRUE ? (void)0 : printf
#endif
/******************************************************************************
******************************************************************************/
typedef signed   char	s8;
typedef unsigned char	u8;
typedef signed   short	s16;
typedef unsigned short	u16;
typedef signed   int	s32;
typedef unsigned int	u32;
/******************************************************************************
******************************************************************************/
typedef struct{
  u8	command;	// �R�}���h
  u8	value;		// ���[�U�t���[
  u8	sumL;		// �`�F�b�N�T�����
  u8	sumH;		// �`�F�b�N�T������
}Header;
/***********************************************************************
NitroSDK ���Œ�`���Ă���֐���extern �錾
***********************************************************************/
extern u16 _calcsum(void *input, u32 length);
extern void dhc_init(void); // NitroSDK ���Ɏ���
extern u8 dhc_read(u8 *buf); // ���[�U�͌Ă΂Ȃ��Ă悢�͂�
extern void dhci_write(u8 *buf, u8 size, u8 command, u8 value); // ���[�U�͌Ă΂Ȃ��Ă悢�͂�
extern void dhc_write(u8 *buf, u8 size, u8 command, u8 value);
extern void dhc_savedata(void); // ���[�U�͌Ă΂Ȃ��Ă悢�͂�
extern u8* dhc_getretrydata(void); // ���[�U�͌Ă΂Ȃ��Ă悢�͂�
extern void dhc_retry(void); // ���[�U�͌Ă΂Ȃ��Ă悢�͂�
extern void dhc_setreadwait(u8 time); // ���[�U�͌Ă΂Ȃ��Ă悢�͂�
extern void dhc_setwritewait(u8 time); // ���[�U�͌Ă΂Ȃ��Ă悢�͂�
extern void dhci_command(u8 command); // 1�o�C�g���M�֐�

#endif
