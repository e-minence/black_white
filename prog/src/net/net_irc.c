//==============================================================================
/**
 * @file	net_irc.c
 * @brief	IRC����
 * @author	matsuda
 * @date	2008.10.10(��)
 */
//==============================================================================
#include "gflib.h"

#include "net/network_define.h"
#include <nitro/irc.h>
#include "net/net_irc.h"
#include "system/gfl_use.h"



#if 0 //DEBUG_ONLY_FOR_ohno

#define IRC_PRINT(...)  (void) ((OS_TPrintf(__VA_ARGS__)))

#else   //GFL_NET_DEBUG

#define IRC_PRINT(...)   ((void) 0)

#endif  // GFL_NET_DEBUG


//==============================================================================
//	�萔��`
//==============================================================================
///�ؒf�Ƃ��Ĕ��肷��܂ł̎���(���̎��ԓ��Ȃ�ؒf���Ă��Đڑ������݂�)
#define NET_IRC_SHOUT_DOWN_TIME		(60 * 5)

///�Đڑ�����ۂɁA�Đڑ����N�G�X�g�J�n����܂ł̃E�F�C�g
///(������̏I���A�����������Ȃǂ�����̂ŏ����Ԃ��󂯂�)
#define NET_IRC_RETRY_START_WAIT	(5)

///�Đڑ�����ۂɍs��IRC_Connect�����t���[���󂯂čēx���N�G�X�g�������邩
///���Ń��N�G�X�g���͂����Ƃ͂����Ă���ۂ����ǈꉞ
#define NET_IRC_CONNECT_REQ_WAIT	(5)

/*----------------------
!!value�l�̈���!!

����4�r�b�g���u���Ԃ̃f�[�^�܂ő��M���܂����v�Ƃ������M����̒l�A
���4�r�b�g���u���Ԃ̃f�[�^�܂Ŏ󂯎��܂����v�Ƃ�����M����̒l�Ƃ���B
���M�������͎�M������value�l�ł����Ɓu���Ԃ̃f�[�^���󂯎��܂����v�Ƃ����Ԏ���
�����Ȃ����莟�̃f�[�^�͑��M�o���Ȃ��悤�ɂ���
��IRC_Send�͎�ň�u�B�����肷��Ɛؒf�͂��Ă��Ȃ��̂ɑ��M�������s���Ă��܂��悤�Ȃ̂ŁB
------------------------*/
///���M�f�[�^�Ǘ�value�l�̃����W
#define SEND_CHECK_VALUE_RANGE		(0xf)	//4bit���̐��l�ɂ��鎖!!

#define _ID (POKEMON_IRC_CONNECTCODE)

//--------------------------------------------------------------
//	�G���[�R�[�h
//--------------------------------------------------------------
enum{
  IRC_ERRCODE_NULL,		///<�G���[�Ȃ�
  IRC_ERRCODE_SHUTDOWN,	///<�ؒf�G���[
};

#define _IRC_HEAD_SIZE (14)
#define _IRC_SEND_SIZE (IRC_TRNS_MAX - _IRC_HEAD_SIZE) 

typedef struct {
  u16 crc;       /// CRC����
  u8 command;    /// ���̒ʐM�̃R�}���h
  u8 value;      /// ���̒ʐM��value  ��ɓ����ʐM���ǂ����Ɏg�p     4
  u32 unique;    /// �őc�Ƀ����_���Ɍ��߂��L�[                      8
  u32 friendunique;    /// ����̃����_���Ɍ��߂��L�[               12
  u8 gsid;       /// GameServiceID       
  u8 dummy;                              // 14
  u8 buff[_IRC_SEND_SIZE];
} _NETIRC_DATA;


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
  IrcRecvFunc recieve_func;
  _NETIRC_DATA aSendBuff;
  u32 friendunique;     ///< ����̃��j�[�NID
  u32 timeout;		///<�Đڑ����̃^�C���A�E�g����
  u32 retry_time;		///<�Đڑ����̃^�C���J�E���^
  void *send_buf;		///<���M�f�[�^�ւ̃|�C���^
  IrcGSIDCallback* checkGSID;  ///< GSID�̓����r���K�v�̂΂����g��

  u8 initialize;		///<TRUE:����������
  u8 send_use;		///<���t���[���Ŋ���Send���g�p�������ǂ���(TRUE:�g�p����)
  u8 send_turn;		///<TRUE:���M�\�B�@FALSE:���M�͒ʐM����̃^�[��
  u8 send_lock;		///<TRUE:���M���b�N��(���葤������f�[�^���M��)

  u8 parent_MacAddress[6];	///<�e��MacAddress
  u8 friendmac[6];     ///< �����MAC

  u8 isSender;		///<TRUE:�ŏ��ɐڑ��v������(�Đڑ����͒l�͕ς��܂���)
  u8 connect;			///<TRUE:�q�����Ă���(�Đڑ�����TRUE)
  u8 send_size;		///<���M�T�C�Y
  u8 send_command;	///<���M�R�}���h

  u8 send_value;		///<���M�o�����[
  u8 send_action;		///<TRUE:���M���s��(send_buf==NULL�ł̓R�}���h�݂̂̑��M�̉\��������̂�)
  u8 my_value;		///<���������M���Ă���value�l�B�������M����x�ɃC���N�������g
  u8 last_value;		///<�Ō�Ɏ�M����value�l

  u8 retry_send_reserve;	///<�Đڑ��������ɑ��M���̃f�[�^���������ꍇ�A���M�\�񂷂�
  u8 err_code;			///<�G���[�R�[�h
  u8 my_unit_number;		///<�����̃��j�b�g�i���o�[
  u8 target_unit_number;	///<�ʐM����̃��j�b�g�i���o�[
}NET_IRC_SYS;


//==============================================================================
//	���[�J���ϐ�
//==============================================================================
static NET_IRC_SYS NetIrcSys = {0,0};


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
BOOL GFL_NET_IRC_SendTurnGet(void);
BOOL GFL_NET_IRC_SendLockFlagGet(void);
BOOL GFL_NET_IRC_SendCheck(void);
void GFL_NET_IRC_UpdateSendData(void);
static void IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 id);
static int IRC_TargetIDGet(void);


static void _IRC_SendRapper(u8* buf, u8 size, u8 command, u8 value)
{
  u16 crc;
  GF_ASSERT(size < _IRC_SEND_SIZE );

  NetIrcSys.aSendBuff.command = command;
  NetIrcSys.aSendBuff.value = value;
  if(buf){
    GFL_STD_MemCopy(buf, NetIrcSys.aSendBuff.buff, _IRC_SEND_SIZE);
  }

  crc = GFL_STD_CrcCalc(&NetIrcSys.aSendBuff.command, size + _IRC_HEAD_SIZE-2);
  NetIrcSys.aSendBuff.crc = crc;
  
  IRC_Send((const u8*)&NetIrcSys.aSendBuff, size + _IRC_HEAD_SIZE,  command);
}




//--------------------------------------------------------------
/**
 * @brief
 *
 * @param   irc_timeout		�ԊO���Đڑ����̃^�C���A�E�g����
 *
 * @retval
 *
 *
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Init(u32 irc_timeout,u8 gsid)
{
  GFL_STD_MemClear(&NetIrcSys, sizeof(NET_IRC_SYS));
  NetIrcSys.timeout = irc_timeout;
  NetIrcSys.my_unit_number = WM_GetNextTgid() & 0xff;
  IRC_Init(_ID);
  IRC_SetRecvCallback(IRC_ReceiveCallback);

  NetIrcSys.aSendBuff.unique = GFUser_GetPublicRand(0); //����
  if(NetIrcSys.aSendBuff.unique==0){
    NetIrcSys.aSendBuff.unique=1;
  }
  NetIrcSys.aSendBuff.gsid = gsid;
//  OS_GetMacAddress( NetIrcSys.aSendBuff.mac );
}

//--------------------------------------------------------------
/**
 * @brief
 *
 * @param   irc_timeout		�ԊO���Đڑ����̃^�C���A�E�g����
 *
 * @retval
 *
 *
 */
//--------------------------------------------------------------
void GFL_NET_IRC_ChangeTimeoutTime(u32 irc_timeout)
{
  NetIrcSys.timeout = irc_timeout;
}


//--------------------------------------------------------------
/**
 * @brief
 *
 * @param   none
 *
 * @retval
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Exit(void)
{
  IRC_PRINT("IRC Exit ��M\n");
  if(NetIrcSys.initialize == TRUE){
    IRC_Shutdown();
  }
  NetIrcSys.initialize = FALSE;
}

void GFL_NET_IRC_RecieveFuncSet(IrcRecvFunc recieve_func)
{
  NetIrcSys.recieve_func = recieve_func;
}

//--------------------------------------------------------------
/**
 * @brief   GSID��r�R�[���o�b�N 
 */
//--------------------------------------------------------------

void GFL_NET_IRC_SetGSIDCallback(IrcGSIDCallback* callback)
{
  NetIrcSys.checkGSID = callback;
}


//--------------------------------------------------------------
/**
 * @brief   �����������t���O���Z�b�g����
 */
//--------------------------------------------------------------
void GFL_NET_IRC_InitializeFlagSet(void)
{
  GF_ASSERT(NetIrcSys.initialize == FALSE);
  NetIrcSys.initialize = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �����������t���O���擾����
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_InitializeFlagGet(void)
{
  return NetIrcSys.initialize;
}

//--------------------------------------------------------------
/**
 * @brief   ���M�\�^�[���t���O���擾
 *
 * @retval  TRUE:���M�\�B�@FALSE:�s�\
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_SendTurnGet(void)
{
  return NetIrcSys.send_turn;
}

//--------------------------------------------------------------
/**
 * @brief   ���M���b�N�t���O���擾
 *
 * @retval  TRUE:���M���b�N���������Ă���
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_SendLockFlagGet(void)
{
  return NetIrcSys.send_lock;
}

//--------------------------------------------------------------
/**
 * @brief   ���M�\�����ׂ�
 *
 * @retval  TRUE:���M�\�B�@FALSE:�s�\
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_SendCheck(void)
{
  if(NetIrcSys.send_turn == TRUE && NetIrcSys.send_lock == FALSE
     && NetIrcSys.retry_time == 0 && NetIrcSys.send_action == 0){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�t�@���M
 *
 * @param   buf
 * @param   size
 * @param   command
 * @param   value
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Send(u8 *buf, u8 size, u8 command)
{
  u8 value;

  GF_ASSERT(NetIrcSys.send_turn == TRUE);

#if GFL_IRC_DEBUG
  if(command != GF_NET_COMMAND_CONTINUE){
    IRC_PRINT("IRC_SEND size=%d, command=%d, value=%d\n", size, command, value);
    {
      int i;
      for(i = 0; i < size; i++){
        IRC_PRINT("0x%2x, ", buf[i]);
      }
      IRC_PRINT("\n");
    }
  }
#endif

  value = NetIrcSys.my_value | (NetIrcSys.last_value << 4);
  _IRC_SendRapper(buf, size, command, value);
  NetIrcSys.send_buf = buf;
  NetIrcSys.send_size = size;
  NetIrcSys.send_command = command;
  NetIrcSys.send_value = NetIrcSys.my_value;
  NetIrcSys.send_action = TRUE;

  NetIrcSys.send_turn = 0;
}

//--------------------------------------------------------------
/**
 * @brief   send_turn������Ă��Ă���̂ɉ����f�[�^�𑗂��Ă��Ȃ��ꍇ�A���̊֐����g�p����
 * 			�ʐM�p���ׂ̈̃R�}���h�𑗐M����
 */
//--------------------------------------------------------------
void GFL_NET_IRC_CommandContinue(void)
{
  NET_IRC_SYS *nis = &NetIrcSys;

  if(nis->send_turn == TRUE){
    if(nis->retry_send_reserve == TRUE){	//�Đڑ���̗\�񂪂���ꍇ�͂�����𑗐M
      _IRC_SendRapper(nis->send_buf, nis->send_size, nis->send_command,
                      nis->send_value | (NetIrcSys.last_value << 4));
      IRC_PRINT("�\�񑗐M value = %d\n", nis->send_value);
      nis->retry_send_reserve = 0;
    }
    else{
      //�ʐM�p���ׂ̈�send_turn������
      _IRC_SendRapper(NULL, 0, GF_NET_COMMAND_CONTINUE,
                      nis->send_value | (NetIrcSys.last_value << 4));
    }
    nis->send_turn = 0;
  }
}

//--------------------------------------------------------------
/**
 * @brief   IRC�l�b�gID�擾
 *
 * @retval  ID
 */
//--------------------------------------------------------------
int GFL_NET_IRC_System_GetCurrentAid(void)
{
  if(NetIrcSys.isSender == TRUE){
    return GFL_NET_NO_PARENTMACHINE;
  }
  return 1;
}

//--------------------------------------------------------------
/**
 * @brief   IRC��M���ɌĂ΂��R�[���o�b�N
 *
 * @param   data
 * @param   size
 * @param   command
 * @param   value
 */
//--------------------------------------------------------------
static void IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 id)
{
  u16 crc;
  u8 nullmac[6]={0,0,0,0,0,0};
  int send_id;
  int send_value, receive_value;
  u8 value;
  _NETIRC_DATA* pData = (_NETIRC_DATA*)data;

  value = pData->value;
  send_value = value & 0xf;
  receive_value = value >> 4;
/*
  if(command != GF_NET_COMMAND_CONTINUE){
    IRC_PRINT("IRC��M�R�[���o�b�N�Ăяo��, size=%d, command=%d, send_value=%d, receive_value=%d\n", size,command,send_value,receive_value);
    {
      int i;
      for(i = 0; i < size; i++){
        IRC_PRINT("0x%2x, ", data[i]);
      }
      IRC_PRINT("\n");
    }
  }*/
  
  NetIrcSys.send_turn = TRUE;

  GF_ASSERT(size <= sizeof(_NETIRC_DATA));
  if(size==0){
    IRC_PRINT("size0\n");
    return;
  }
  if(size > sizeof(_NETIRC_DATA)){
    IRC_PRINT("sizeOVER\n");
    return;
  }
  
  crc = GFL_STD_CrcCalc(&pData->command, size-2);
  if(pData->crc != crc){
    IRC_PRINT("�ŏ����炱��ꂽ\n");
    return;
  }

  if(NetIrcSys.checkGSID){
    if(NetIrcSys.checkGSID(NetIrcSys.aSendBuff.gsid , pData->gsid)==FALSE){
      IRC_PRINT("GSID���ႤA %d %d\n",NetIrcSys.aSendBuff.gsid , pData->gsid);
      return;
    }
  }
  else if(NetIrcSys.aSendBuff.gsid != pData->gsid){//���e���Ⴄ
    IRC_PRINT("GSID���Ⴄ %d %d\n",NetIrcSys.aSendBuff.gsid , pData->gsid);
    return;
  }
  if((pData->friendunique == NetIrcSys.aSendBuff.unique)
     && (pData->unique == NetIrcSys.friendunique)){  //���ݓo�^�ς� ���Ȃ�
  }
  else if((pData->friendunique == 0) && (NetIrcSys.friendunique==0)){  //����̓o�^�܂� �������܂�
    NetIrcSys.aSendBuff.friendunique = pData->unique;
    NetIrcSys.friendunique = pData->unique;
    IRC_PRINT("�o�^1 %d %d\n",pData->unique, NetIrcSys.aSendBuff.unique);
    return;   //�o�^���͈�񖳎�
  }
  else if((pData->friendunique == NetIrcSys.aSendBuff.unique) && (NetIrcSys.friendunique==0)){  //����̓o�^�܂� �������܂�
    NetIrcSys.aSendBuff.friendunique = pData->unique;
    NetIrcSys.friendunique = pData->unique;
    IRC_PRINT("�o�^2 %d %d\n",pData->unique, NetIrcSys.aSendBuff.unique);
  }
  else{
    IRC_PRINT("UNIQUE�������� %d %d %d\n",NetIrcSys.friendunique ,pData->unique, NetIrcSys.aSendBuff.unique );
    return;   //���j�[�NID��������疳��
  }

  //-- �ԊO�����C�u���������Ŏg�p���Ă���V�X�e���R�}���h --//
  if(command >= 0xf0){
    if(command == 0xf4){
      IRC_PRINT("IRC�ؒf�R�}���h����M\n");
    }
    return; //�ԊO�����C�u���������Ŏg�p���Ă���V�X�e���R�}���h�ׁ̈A�����ł͖���
  }

  if(NetIrcSys.send_action == TRUE && NetIrcSys.retry_send_reserve == 0){
    //���炩�̕Ԏ�������������Ԃ��Ă��Ă���Ȃ��قǑ��M�����f�[�^�͌������ɓn�����Ƃ�����
    //�A�����C�u���������̃V�X�e���R�}���h�͐ؒf�R�}���h�������肷��̂ŁA
    //�V�X�e���R�}���h�̕Ԏ��͖�������
    IRC_PRINT("action�Ԏ���M my_value = %d, receive_value=%d\n", NetIrcSys.my_value, receive_value);
    if(receive_value == NetIrcSys.my_value){
      if(NetIrcSys.my_value < SEND_CHECK_VALUE_RANGE){
        NetIrcSys.my_value++;
      }
      else{
        NetIrcSys.my_value = 0;
      }
      NetIrcSys.send_action = 0;
    }
    else{
      NetIrcSys.retry_send_reserve = TRUE;  //�ēx���M�\��
    }
  }

  //�ԊO����p�̃V�X�e���R�}���h����
  switch(command){
  case GF_NET_COMMAND_CONTINUE:
    return;   //�ʐM�p���ׂ̈̉����R�}���h�ׁ̈A�����ŏI��
  default:
    break;
  }

  if(NetIrcSys.last_value == send_value){
    //�Ō�Ɏ󂯎����value�l�Ɠ���value�l�̏ꍇ�A�����f�[�^��2�x�����Ă��Ă���̂Ŗ�������
    //���Đڑ��������ɂ��̃P�[�X����������B
    IRC_PRINT("�ԊO���F����value����M %d\n",send_value);
    return;
  }

  
  send_id = IRC_TargetIDGet();
  if(NetIrcSys.recieve_func != NULL){
    if(NetIrcSys.recieve_func(send_id, (u16*)pData->buff, size - _IRC_HEAD_SIZE) == FALSE){
      IRC_PRINT("����ꂽ\n");
      return; //�f�[�^�����Ă���Ȃ�󂯎��Ȃ�
    }
  }

  IRC_PRINT("��M���� %d\n",send_value);
  NetIrcSys.last_value = send_value;
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�����ID���擾
 *
 * @retval  �����ID
 */
//--------------------------------------------------------------
static int IRC_TargetIDGet(void)
{
  if(GFL_NET_IRC_IsSender() == TRUE){
    return 1;	//�������e�Ȃ�q���炵���f�[�^�����Ȃ��̂�1�Œ�
  }

  //�������q�Ȃ�A�e���炵�����Ȃ��̂�GFL_NET_NO_PARENTMACHINE�Œ�
  return GFL_NET_NO_PARENTMACHINE;
}

//--------------------------------------------------------------
/**
 * @brief   �e��MacAddress�����[�N�ɕۑ�
 *
 * @param   mac_address		�e��MacAddress
 */
//--------------------------------------------------------------
void GFL_NET_IRC_ParentMacAddressSet(const u8 *mac_address)
{
  GFL_STD_MemCopy(mac_address, NetIrcSys.parent_MacAddress, 6);
}

//--------------------------------------------------------------
/**
 * @brief   �e��MacAddress�����o��
 *
 * @retval  �e��MacAddress��ێ����Ă��郏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
u8 * GFL_NET_IRC_ParentMacAddressGet(void)
{
  return NetIrcSys.parent_MacAddress;
}

//--------------------------------------------------------------
/**
 * @brief   �G���[���������Ă��邩
 *
 * @retval  TRUE:�G���[���������Ă���
 * @retval  FALSE:�G���[�Ȃ�
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_ErrorCheck(void)
{
  if(NetIrcSys.initialize == FALSE){
    return FALSE;	//�V�X�e���̏��������̂���Ă��Ȃ��̂ŃG���[���m�ȑO�̖��
  }
  if(NetIrcSys.err_code != IRC_ERRCODE_NULL){
//    IRC_PRINT("�ԊO���G���[�Ferr_code = %d\n", NetIrcSys.err_code);
    return TRUE;
  }
  if(NetIrcSys.connect == TRUE && GFL_NET_IRC_IsConnect() == FALSE){
//    IRC_PRINT("�ԊO���G���[�FGFL_NET_IRC_IsConnect\n");
    return TRUE;
  }
  return FALSE;	//�G���[�Ȃ�
}

//--------------------------------------------------------------
/**
 * @brief   ���߂Ă̐ڑ���̃��[�N�ݒ�
 */
//--------------------------------------------------------------
void GFL_NET_IRC_FirstConnect(void)
{
  NetIrcSys.connect = TRUE;
  NetIrcSys.isSender = IRC_IsSender();
  NetIrcSys.my_value = 0;
  NetIrcSys.last_value = SEND_CHECK_VALUE_RANGE-1;	//�ŏ��̃f�[�^�������Ǝ󂯎���悤��my_value�ƈႦ�Ή��ł��悢
  NetIrcSys.target_unit_number = IRC_GetUnitNumber();
  if(NetIrcSys.isSender == TRUE){
    IRC_PRINT("�ԊO���F�e�@�ɂȂ���\n");
  }
  else{
    IRC_PRINT("�ԊO���F�q�@�ɂȂ���\n");
    //���V�[�o�[����ŏ��̒ʐM���n�߂�
    NetIrcSys.send_turn = TRUE;
  }
  IRC_PRINT("�����̃��j�b�gNo=%d, ����̃��j�b�gNo=%d\n", NetIrcSys.my_unit_number, NetIrcSys.target_unit_number);
}

//--------------------------------------------------------------
/**
 * @brief   �ŏ��ɐڑ��v�������[�������ׂ�
 *
 * @retval  TRUE:�ŏ��ɐڑ��v�������[���ł���
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_IsSender(void)
{
  return NetIrcSys.isSender;
}

//--------------------------------------------------------------
/**
 * @brief   �ڑ������ǂ���(�Đڑ����͌q�����Ă���ƕԂ��܂�)
 *
 * @retval  TRUE:�ڑ����Ă���
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_IsConnect(void)
{
  if(IRC_IsConnect() == TRUE){
    //if(IRC_GetUnitNumber() == NetIrcSys.target_unit_number)
    {
      return TRUE;
    }
//    IRC_PRINT("�ڑ��`�F�b�N�F�^�[�Q�b�g���j�b�g�i���o�[���Ⴄ\n");
    return FALSE;	//�ؒf��������ɐڑ������Ƃ��̏󋵂����肦��
  }
  if((NetIrcSys.retry_time < NetIrcSys.timeout) || (NetIrcSys.retry_time == 0 && NetIrcSys.timeout == 0)){
    return TRUE;	//�Đڑ���
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �Đڑ����ȂǊ֌W�Ȃ��A�����ɍ��A�q�����Ă��邩
 *
 * @retval  TRUE:�ڑ����Ă���
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_IsConnectSystem(void)
{
  return IRC_IsConnect();
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO�����s����
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Move(void)
{
  NET_IRC_SYS *nis = &NetIrcSys;

  if(nis->connect == FALSE && (nis->retry_time >= nis->timeout && nis->retry_time > 0)){
    //�Đڑ������s���Ċ��S�ɏI����ԁBIRC_Move�𓮂����Ƃ܂����肪���g���C�������Ă����ꍇ
    //������͊��S�I���Ɉڍs�����̂Ɍ��������q����A�ƂȂ��Ă��܂��̂ł�����return����
    return;
  }

  if(nis->retry_time == 0){
    IRC_Move();
  }

  //�q�����Ă���ꍇ�A�ؒf���Ă��Ȃ������ׁA�ؒf���Ă���΍Đڑ������։�
  if(nis->connect == TRUE){
    if(nis->retry_time > 0){
      //�Đڑ���
      if(IRC_IsConnect() == TRUE){
        //�Đڑ�����
        nis->retry_time = 0;
        IRC_PRINT("�ԊO���F�Đڑ�OK!\n");
        if(IRC_IsSender() == TRUE){
          nis->send_turn = 0;
        }
        else{
          nis->send_turn = TRUE;
        }
        if(nis->send_action == TRUE){
          nis->retry_send_reserve = TRUE;	//�ؒf�O�ɑ������f�[�^���ēx���M�\��
        }
      }
      else{
        //�Đڑ��҂�
        if(nis->retry_time < NET_IRC_RETRY_START_WAIT){
          //���ۂɃ��g���C��������܂ŁA������̏����������Ȃǂ�����̂�
          //�����Ԃ��󂯂�
          nis->retry_time++;
          return;
        }
        else if(nis->retry_time == NET_IRC_RETRY_START_WAIT){
          IRC_Init(_ID);
          IRC_SetRecvCallback(IRC_ReceiveCallback);
          //IRC_SetUnitNumber(NetIrcSys.my_unit_number);
          nis->retry_time++;
          return;
        }

        IRC_Move();
        if(nis->retry_time < nis->timeout){
          if(nis->isSender == TRUE && nis->retry_time % NET_IRC_CONNECT_REQ_WAIT == 0){
//            IRC_PRINT("�ԊO���F�Đڑ�Connect���s\n");
            IRC_Connect();
          }
          nis->retry_time++;
        }
        else{	//�Đڑ��̎��Ԑ؂�
//          IRC_PRINT("�ԊO���F�Đڑ��̎��Ԑ؂�\n");
          nis->connect = FALSE;
          nis->err_code = IRC_ERRCODE_SHUTDOWN;
          GFI_NET_HANDLE_Delete(1-GFL_NET_IRC_System_GetCurrentAid());  // �n���h���폜
          IRC_Shutdown();
        }
      }
    }
    else{
      //�q�����Ă���
      if(IRC_IsConnect() == FALSE){
        if(nis->timeout > 0){
//          IRC_PRINT("�ԊO���F�ؒf�����B�Đڑ����܂�\n");
          IRC_Shutdown();
          nis->retry_time = 1;	//�Đڑ���
        }
        else{	//�Đڑ��Ȃ�
//          IRC_PRINT("�ԊO���F�ؒf�����B�Đڑ��͂��܂���\n");
          nis->connect = FALSE;
          nis->err_code = IRC_ERRCODE_SHUTDOWN;
          nis->retry_time = 0xffff;
          GFI_NET_HANDLE_Delete(1-GFL_NET_IRC_System_GetCurrentAid());  // �n���h���폜
          IRC_Shutdown();
        }
      }
    }
  }
}
