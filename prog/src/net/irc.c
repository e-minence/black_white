#include <nitro.h>

#include "net/irc.h"
#ifdef DHC_DEBUG  
//#include "draw.h"
#endif

/***********************************************************************
***********************************************************************/
#define NUM_DS_RETRY	(5) // DS �̃��g���C���s��
/***********************************************************************
NitroSDK ���Œ�`���Ă���֐���extern �錾
***********************************************************************/
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
/***********************************************************************
���C�u�����Ƃ��Ԃ��Ă�̂łȂ�Ƃ��������H
***********************************************************************/
typedef struct{
  u8	command;	// �R�}���h
  u8	value;		// ���[�U�t���[
  u8	sumL;		// �`�F�b�N�T�����
  u8	sumH;		// �`�F�b�N�T������
}Header;
/***********************************************************************
***********************************************************************/
static u32 cnt_retry;
static u8 myUnitNumber = 0xFF;
static u8 unit_number = 0xFF;
static u8 retry_count = 0;
static BOOL flg_connect = FALSE;
static BOOL flg_restore = FALSE; // �G���[���A�����ǂ���
static BOOL isTry = FALSE;		// �e�q�����܂��Ă��邩�ǂ���
static BOOL isSender = FALSE;	// �Z���_���ǂ���
static OSTick timeout;
static u8 buf[sizeof(Header)+IRC_TRNS_MAX];
static IRCRecvCallback sRecvCallback;
/***********************************************************************
***********************************************************************/
static u16 _calcsum(void *input, u32 length)
{
  u32 sum, i;
  u8 t;
  
  sum = 0;

  for(i = 0; i<length; i++){ 
	t = *((u8*)input)++;
	if(i&1){ sum += t; }else{ sum += t<<8; }
  }

  sum = (sum >> 16) + (sum & 0xFFFF); sum += (sum >> 16);

  return ((u16)sum);
}
/***********************************************************************
***********************************************************************/
static void _restore(void)
{
  flg_restore = TRUE;
  cnt_retry++; // IRC �̃J�E���^�A�b�v
}

/***********************************************************************
���ʏ���������(�V���b�g�_�E�����ɂ��Ă����Ȃ����̂̂�)
***********************************************************************/
static void _init(void)
{
  retry_count = 0;
  flg_connect = FALSE;
  flg_restore = FALSE;

  isTry = FALSE;
  isSender = FALSE;
  
  cnt_retry = 0;
#ifdef DHC_DEBUG  
  OS_Printf("shutdown and retry_count cleared\n");
#endif
}
/***********************************************************************
����������
***********************************************************************/
extern void IRC_Init(void)
{
  dhc_init();
  dhc_read(buf); // IRC�Ɏc���Ă���f�[�^���N���A����ړI
  _init();
  myUnitNumber = 0xFF;
  unit_number = 0xFF;
  sRecvCallback = NULL;
}
/***********************************************************************
�ڑ��v���B�������������(���ۂɂ͂��̗v�����󂯂����̋t) ���Z���_�ɂȂ�܂��B
***********************************************************************/
extern void IRC_Connect(void)
{
  if(flg_connect){ return; } // �ڑ���
  if(isTry){ return; } isTry = TRUE;
    
  dhci_command(0xFC); // �ڑ��v��
  timeout = OS_GetTick(); // 2008/09/19 �^�C���A�E�g���ԍX�V
}
/***********************************************************************
�Z���_���ǂ����̊m�F
***********************************************************************/
extern BOOL IRC_IsSender(void)
{
  return (isSender);
}
/***********************************************************************
�ڑ�����Ă��邩�ǂ����̊m�F
***********************************************************************/
extern BOOL IRC_IsConnect(void)
{
  return (flg_connect);
}
/***********************************************************************
�ؒf����
***********************************************************************/
extern void IRC_Shutdown(void)
{
  if(flg_connect && (!isSender)){ dhci_write(NULL, 0, 0xF4, 0x00); } // �ؒf�R�}���h���M�B�R�}���h�̂ݑ���ꍇ�̓T�C�Y��0�ɂ��邱�ƁI

  _init();
}
/***********************************************************************
�f�[�^���M
***********************************************************************/
extern void IRC_Send(u8 *buf, u8 size, u8 command, u8 value)
{
  if(!flg_connect){ return; }
  dhc_write(buf, size, command, value);
}
/***********************************************************************
�v���Z�X�����֐�
***********************************************************************/
extern void IRC_Move(void)
{
  u32 size;

#ifdef DHC_DEBUG  
  static u8 count = 0;
//  SetTextColor(RGB555_GREEN);
#endif
  dhc_setreadwait(50); // 40 OK? 30 NG
  dhc_setwritewait(50); // 40 OK? 30 NG
  /*********************************************************************
  *********************************************************************/
  size = (u32)dhc_read(buf);
#ifdef DHC_DEBUG  
//  if(size){ // ���f�[�^�̐擪����������
//	u32 i, s;
//	s = size; if(s > 8){ s = 8; }
//	FillRect(0, 8, 8*9+8*3*8, 8, RGB555_BLACK);
//	DrawText(0, 8, "size %2d:", size);
//	for(i=0;i<s;i++){ DrawText((int)(8*9+8*3*i), 8, "%02X", buf[i]); }
//  }
#endif
  if(size > 1){ // 1�o�C�g�̃l�S�V�G�[�V�����R�}���h�̓`�F�b�N���Ȃ�
	u16 checksum, mysum;
	Header *pHeader;
	
	pHeader = (Header*)buf;
	checksum = (u16)(pHeader->sumL + (pHeader->sumH << 8)); 
	pHeader->sumL = 0; pHeader->sumH = 0; 
	mysum = _calcsum(buf, size);
	pHeader->sumL = (u8)(checksum&0xFF); pHeader->sumH = (u8)(checksum>>8); // �߂��Ă���
	if(checksum != mysum){ // �`�F�b�N�T���G���[
	  if(flg_connect){  // ���A�����J�n
#ifdef DHC_DEBUG  
		if((pHeader->command & 0xFE) != 0xFE){
		  OS_Printf("�`�F�b�N�T���G���[�ɂ��Đڑ��J�n ��M�R�}���h 0x%X RETRY-COUNT %d: size %d 0x%X(COM) -> 0x%X(CALC)\n", pHeader->command, retry_count, size, checksum, mysum); 
		}
#endif
		_restore(); 
	  }
	  size = 0; // �f�[�^���͂��Ȃ��������Ƃɂ���
	  //timeout = OS_GetTick(); // ���̏ꍇ�ł��^�C���A�E�g�͍X�V(�ŏ���FC���l������Έꂩ���ɂ܂Ƃ߂��邩��)
	}else{ // �`�F�b�N�T�����Ȃ�
#ifdef DHC_DEBUG  
	  if(retry_count){ OS_Printf("retry_count cleared\n"); }
#endif
	  retry_count = 0; 
	}
  }else{ // 1�o�C�g�ő����Ă���͍̂ŏ���0xFC�̂�
	if(buf[0] != 0xFC){
	  if(flg_connect){ 
		size = 0; 
		///*if(buf[0] != 0)*/{ OS_Printf("RECV 0x%X\n", buf[0]); }
	  } // �����̃��X�g�A���ĂԂƃm�C�Y�ł������X�g�A���������Ă���������
	}
  }
  /*********************************************************************
  �`�F�b�N�T���`�F�b�N���܂߂ăf�[�^���͂��Ă����ꍇ
  *********************************************************************/
  if(size){
	timeout = OS_GetTick(); // �^�C���A�E�g���ԍX�V
	switch(buf[0]){
	case 0xFC: // �ڑ��R�}���h
	  if(isTry){ break; } isTry = TRUE;
	  isSender = FALSE;
	  if((size > 1) && (flg_connect == 0)){ break; } // �ؒf���̍Đڑ��R�}���h�͖���
	  dhci_write(NULL, 0, 0xFA, myUnitNumber); // �R�}���h�̂ݑ���ꍇ�̓T�C�Y��0�ɂ��邱�ƁI
	  if(flg_connect){ _restore(); } // DHC �����ُ팟�o���Đڑ����ɍĐڑ��Ƃ��Ă��̃R�}���h�𑗂��Ă����ꍇ
#ifdef DHC_DEBUG  
	  if(flg_restore){
		OS_Printf("�Đڑ��v����M\n", buf[0]);
	  }else{
		OS_Printf("�V�K�ڑ��v����M -> ���X�|���X���M\n", buf[0]);
	  }
//	  if(flg_connect == FALSE){ ClearFrame(RGB555_BLACK); }
//	  DrawText(32, 0, ">REQ");
//	  DrawText(32+8*5, 0, "RES>");
#endif
	  break;
	case 0xFA: // �ڑ��v��������IRC�����M����(DHC �G�~�����[�g�p)
	  isSender = TRUE;
	  unit_number = buf[1]; // ���j�b�g�ԍ�
	  //OS_Printf("RESPONSE FROM IRC ACCEPT\n");
	  dhc_write(NULL, 0, 0xF8, myUnitNumber); // COMMAND_ACK �����̏���
#ifdef DHC_DEBUG  
	  OS_Printf("���X�|���X��M -> ACK ���M\n");
	 // DrawText(8*4*2, 0, ">RES ACK>");
#endif
	  flg_connect = TRUE; flg_restore = FALSE; isTry = FALSE;
	  break;
	case 0xF8: // �ڑ��v���������ACK(���ꂪ�͂��Đڑ�����)
#ifdef DHC_DEBUG  
	  if(flg_restore){
		OS_Printf("ACK ��M -> �Đڑ����� SIZE %d\n", size);
	  }else{
		OS_Printf("ACK ��M -> �V�K�ڑ����� SIZE %d\n", size);
	  }
	  //DrawText(32+8*5*2, 0, ">ACK");
#endif
	  unit_number = buf[1]; // ���j�b�g�ԍ�
	  if(flg_restore){ 
#ifdef DHC_DEBUG  
		u8 *p = dhc_getretrydata();
		OS_Printf("�f�[�^�đ��J�n �R�}���h 0x%X value 0x%X SUM 0x%X%X\n", *(p+1), *(p+2), *(p+3), *(p+4)); // �ŏ���1�o�C�g�̓f�o�C�X�I��
		//OS_Printf("RETRY COMMAND 0x%d value %d %s\n", sBackBuf[0], sBackBuf[1], &sBackBuf[4]);
#endif
		dhc_retry(); 
	  } // �O��̃f�[�^�����̂܂ܑ���
	  flg_connect = TRUE; flg_restore = FALSE; isTry = FALSE;
	  break;
	case 0xF6: // COMMAND_RETRY(DHC �G�~�����[�g�p)
	  flg_restore = TRUE;
	  break;
#if 0
	case 0: 
	  OS_Printf("command 0x%X: value 0x%X, SUM 0x%X, 0x%X size %d 0x%X, 0x%X\n", buf[0], buf[1], buf[2], buf[3], size, buf[4], buf[5]);
	  break; // �g�p�֎~
#endif
	case 0xFE: break; // �m�C�Y�΍�
	case 0xFF: break; // �m�C�Y�΍�
	default:
	  if(flg_restore == TRUE){ break; } // ���A���ɓ͂����ʏ�R�}���h�͖�������
	  if(flg_connect == FALSE){ break; }
	  sRecvCallback(&buf[4], (u8)size, buf[0], buf[1]);
	  break;
	}
  }else{ // �����f�[�^���͂��Ȃ�����(0�o�C�g)�̏ꍇ
	if(flg_restore && (retry_count < NUM_DS_RETRY) && isTry == FALSE){
	  if(OS_TicksToMilliSeconds(OS_GetTick() - timeout) > 50){ // �Đڑ��v����DHC�ɏo��
#ifdef DHC_DEBUG  
		if(isSender){
		  OS_Printf("�Đڑ��v�����M(CONNECT) TO IRC %d/%d\n", retry_count, NUM_DS_RETRY);
		}else{
		  OS_Printf("�Đڑ��v�����M(RETRY) TO DHC %d/%d\n", retry_count, NUM_DS_RETRY);
		}
#endif
		if(isSender){
		  dhci_command(0xFC); retry_count++; // �ڑ��v���R�}���h
		}else{
		  dhci_write(NULL, 0, 0xF6, retry_count++); // 0xF6 �̓��g���C�R�}���h �R�}���h�̂ݑ���ꍇ�̓T�C�Y��0�ɂ��邱�ƁI
		}
		timeout = OS_GetTick();
	  }
	}
	if(OS_TicksToMilliSeconds(OS_GetTick() - timeout) > 100){ // �^�C���A�E�g

    if(flg_restore && (retry_count < NUM_DS_RETRY)){
      OS_Printf("�^�C���A�E�g!!%d\n",retry_count);
      IRC_Shutdown();
	  }
    else{
      if(IRC_IsConnect()){ 
        IRC_Shutdown(); 
      }
      isTry = FALSE; // �ڑ��g���C���t���O���N���A
	  }
	}
  }
}
/***********************************************************************
�f�[�^��M�R�[���o�b�N�̐ݒ�
***********************************************************************/
extern void IRC_SetRecvCallback(IRCRecvCallback cb)
{
  sRecvCallback = cb;
}
/***********************************************************************
�g�[�^���̃��g���C�񐔂��擾����
***********************************************************************/
extern u32 IRC_GetRetryCount(void)
{
  return (cnt_retry);
}
/***********************************************************************
���g�̃��j�b�g�ԍ���ݒ肷��
***********************************************************************/
extern void IRC_SetUnitNumber(u8 id)
{
  myUnitNumber = id;
}
/***********************************************************************
�ʐM����̃��j�b�g�ԍ����擾����
***********************************************************************/
extern u8 IRC_GetUnitNumber(void)
{
  return (unit_number);
}
