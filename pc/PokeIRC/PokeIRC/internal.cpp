#include "stdafx.h"

#include "system.h"
#include "irc_internal.h"
#include "com.h"
/***********************************************************************
***********************************************************************/
#define COMMAND_READ			(0x0001)	// DHC ���ʎq�����˂�
#define COMMAND_WRITE			(0x0002)	// DHC ���ʎq�����˂�
/***********************************************************************
***********************************************************************/
#define COM_DATA_SIZE	(128)	// 2007/11/02 ���[�U����M�o�b�t�@�T�C�Y
#define COM_TRNS_SIZE	(1+sizeof(Header)+COM_DATA_SIZE)	// �V�X�e������M�o�b�t�@�T�C�Y
/***********************************************************************
***********************************************************************/
static u8 last_size, backup_size;				// ���g���C�p
static u8 sSendBuf[COM_TRNS_SIZE]; // ���g���C�p�Ɏ�M�ƕ�����
static u8 sRecvBuf[COM_TRNS_SIZE];
static u8 sBackBuf[COM_TRNS_SIZE+1]; // ���g���C�p TODO: 1byte �̓f�o�O�p
/***********************************************************************
***********************************************************************/
extern u16 _calcsum(void *input, u32 length)
{
  u32 sum, i;
  u8 t;
  u8 *p;

  sum = 0;
  p = (u8*)input;
  for(i = 0; i<length; i++){
	t = *p++;
	if(i&1){ sum += t; }else{ sum += t<<8; }
  }

  sum = (sum >> 16) + (sum & 0xFFFF); sum += (sum >> 16);

  return ((u16)sum);
}
/***********************************************************************
***********************************************************************/
extern void dhc_init(void)
{
  if(ComInit(COM_AUTO) == FALSE){ 
	ComInit(COM_PORT4); // �Ō�͂���Ȃ�
  }
}
/***********************************************************************
***********************************************************************/
static void dhc_savedata(void)
{
  int i;

  for(i=0;i<last_size;i++){ sBackBuf[i]= sSendBuf[i]; }
  backup_size = last_size;
}
/***********************************************************************
buf �Ƀf�[�^������: �߂�l�͎�M�T�C�Y
***********************************************************************/
extern u8 dhc_read(u8 *buf)
{
  int i;
  u8 size = 0;
#if 0
  static int noDataCount = 0;
  static int noDataMax = 0;
#endif
  {
	u8 v = COMMAND_READ;
	//printf("�������݊J�n\n");
	BOOL ret = WriteData(&v, 1);
	if(ret == FALSE){ ComEnd(); dhc_init(); }
  }
  size = ReadData(buf, COM_TRNS_SIZE);
  
  if(size > COM_TRNS_SIZE){ size = 0; }
#if 0
  if(size == 0){ noDataCount++; }else{ noDataMax = noDataCount; noDataCount = 0; }
  PrintString(16*14, SCR_HT+16*0, L"�J�E���g %d", noDataMax);
#endif
  //if(size == 0){ printf("��M�T�C�Y 0\n"); }
  //for(int i=0;i<size;i++){ �ǂݍ��� }
  
#if 1 
	for(i=0;i<size;i++){ buf[i] = (u8)(buf[i] ^ 0xAA); } // �Í�
#endif  

  return (size);
}
/***********************************************************************
***********************************************************************/
extern void dhci_write(u8 *buf, u8 size, u8 command, u8 value)
{
  u8 *p;
  u16 sum;
  int i;
  Header *pHeader;
  /*********************************************************************
  �f�[�^�̏���
  *********************************************************************/
  pHeader = (Header*)&sSendBuf[1];
  pHeader->command = command;
  pHeader->value = value;
  pHeader->sumL = 0; pHeader->sumH = 0; 
  p = &sSendBuf[1+sizeof(Header)];

  for(i=0;i<size;i++){ *p++ = *buf++; }
  sum = _calcsum(&sSendBuf[1], sizeof(Header)+size);

#if 0 // 0828
  OS_Printf("command 0x%X value 0x%X checksum 0x%X\n", command, value, sum);
#endif
  pHeader->sumL = (u8)(sum&0xFF); pHeader->sumH = (u8)((sum>>8)&0xFF);
  /*********************************************************************
  *********************************************************************/
  size += 1+sizeof(Header);
  //card_param.src = (u32)sSendBuf; card_param.dst = (u32)NULL;
  sSendBuf[0] = COMMAND_WRITE;
  /*********************************************************************
  *********************************************************************/
#if 1 // �Í� �f�o�C�X�I���R�}���h�͈Í������Ȃ�
  for(i=1;i<size;i++){ sSendBuf[i]= (u8)(sSendBuf[i] ^ 0xAA); }
#endif
  last_size = size;
  /*********************************************************************
  *********************************************************************/
  WriteData(sSendBuf, size);
}
/***********************************************************************
***********************************************************************/
extern void dhc_write(u8 *data, u8 size, u8 command, u8 value)
{
  dhci_write(data, size, command, value);
  dhc_savedata();
}
/***********************************************************************
***********************************************************************/
extern void dhc_retry(void)
{
  //int i;
  
  sBackBuf[COM_TRNS_SIZE] = 0;
  /*********************************************************************
  *********************************************************************/
#if 1
  WriteData(sBackBuf, backup_size);
#else
  for(i=0;i<backup_size;i++){
	//CommWrite(&card_param);
  }
#endif
}
/***********************************************************************
��֐���OK
***********************************************************************/
extern void dhc_setreadwait(u8 time)
{

}
/***********************************************************************
��֐���OK
***********************************************************************/
extern void dhc_setwritewait(u8 time)
{

}
/***********************************************************************
***********************************************************************/
extern void dhci_command(u8 command) // 
{
  u8 size;
  int i;
  Header *pHeader;
  /*********************************************************************
  �f�[�^�̏���
  *********************************************************************/
  pHeader = (Header*)&sSendBuf[1];
  pHeader->command = command;
  pHeader->value = 0;
  pHeader->sumL = 0; pHeader->sumH = 0; 
  /*********************************************************************
  *********************************************************************/
  size = 1+1;
  sSendBuf[0] = COMMAND_WRITE; 
  /*********************************************************************
  *********************************************************************/
#if 1 // �Í� �f�o�C�X�I���R�}���h�͈Í������Ȃ�
  for(i=1;i<size;i++){ sSendBuf[i]= (u8)(sSendBuf[i] ^ 0xAA); }
#endif  
  WriteData(sSendBuf, size);
}

